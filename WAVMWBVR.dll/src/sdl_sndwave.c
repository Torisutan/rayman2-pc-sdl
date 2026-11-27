/*
 * sdl_sndwave.c: the sample driver's output through SDL (USE_SDL), beside the waveOut output of
 * sndwave.c and the system mixer volume of pc_mixervol.c, which this file replaces in the SDL build.
 *
 * - Device: one SDL audio stream on the default playback device, in the format sndwave.c opens
 *   (22,050 Hz, stereo, 16 bit). sndwave.c writes its mixed buffers into it as it writes them to
 *   waveOut; no stream callback, so SDL's audio thread never waits on the driver's locks.
 * - Output thread: stands in for the waveOut thread; calls sndwave.c's feed step every few
 *   milliseconds, which tops the stream up as the waveOut thread tops up waveOut's queue.
 * - Lock order: the driver's sample lock, then gs_p_stDeviceLock; never the other way round.
 * - Volume: the game's sound volume (0..100, SND_fn_vSetSoundVolumeSample) is the stream's gain,
 *   where the PC driver sets the system mixer's wave line.
 */
#include "SNDinc.h"

#if defined(USE_SDL)

#include "sndmixvol_pc.h"
#include "sdl_sndwave.h"

#include <SDL3/SDL.h>

#define C_ulFeedPeriodMs	5
#define C_ulErrorOpen		1
#define C_ulErrorWrite		2

/* pc_mixervol.c's other globals, read by sndwave.c */
HWND				SND_pc_hMainWindow = NULL;
HINSTANCE			SND_pc_hFirstProcessInstance = NULL;
HINSTANCE			SND_pc_hProcessInstance = NULL;

static SDL_Mutex		*gs_p_stDeviceLock = NULL;
static SDL_AudioStream	*gs_p_stStream = NULL;
static SDL_Thread		*gs_p_stOutputThread = NULL;
static SDL_AtomicInt	gs_stStopOutput;
static void				(*gs_p_fn_vFeed)(void) = NULL;
static unsigned long	gs_ulVolume = 100;
static char				gs_szError[256] = "";

static void fn_vCreateDeviceLock(void)
{
	if (gs_p_stDeviceLock == NULL)
		gs_p_stDeviceLock = SDL_CreateMutex();
}

static void fn_vSetError(const char *szWhat)
{
	SDL_snprintf(gs_szError,sizeof(gs_szError),"%s: %s",szWhat,SDL_GetError());
}

/*---- output device */

unsigned long SND_fn_ulOpenDevice(void **p_hDevice,unsigned long ulFreq,unsigned short uwNbChannels,unsigned short uwResolution)
{
	SDL_AudioSpec stSpec;
	SDL_AudioStream *p_stStream;

	*p_hDevice = NULL;
	fn_vCreateDeviceLock();
	if (!SDL_InitSubSystem(SDL_INIT_AUDIO))
	{
		fn_vSetError("SDL audio");
		return C_ulErrorOpen;
	}
	stSpec.format = (uwResolution == 8) ? SDL_AUDIO_U8 : SDL_AUDIO_S16;
	stSpec.channels = uwNbChannels;
	stSpec.freq = (int)ulFreq;
	p_stStream = SDL_OpenAudioDeviceStream(SDL_AUDIO_DEVICE_DEFAULT_PLAYBACK,&stSpec,NULL,NULL);
	if (p_stStream == NULL)
	{
		fn_vSetError("SDL audio device");
		SDL_QuitSubSystem(SDL_INIT_AUDIO);
		return C_ulErrorOpen;
	}
	SDL_SetAudioStreamGain(p_stStream,(float)gs_ulVolume / 100.0f);
	SDL_LockMutex(gs_p_stDeviceLock);
	gs_p_stStream = p_stStream;
	SDL_UnlockMutex(gs_p_stDeviceLock);
	SDL_ResumeAudioStreamDevice(p_stStream);
	*p_hDevice = p_stStream;
	return 0;
}

unsigned long SND_fn_ulWriteDevice(void *hDevice,char *pData,unsigned long ulLength)
{
	unsigned long ulResult;

	ulResult = 0;
	SDL_LockMutex(gs_p_stDeviceLock);
	/* a buffer mixed while the device was being closed is dropped, as waveOut drops it */
	if (hDevice != NULL && hDevice == gs_p_stStream)
	{
		if (!SDL_PutAudioStreamData(gs_p_stStream,pData,(int)ulLength))
		{
			fn_vSetError("SDL audio write");
			ulResult = C_ulErrorWrite;
		}
	}
	SDL_UnlockMutex(gs_p_stDeviceLock);
	return ulResult;
}

unsigned long SND_fn_ulResetDevice(void *hDevice)
{
	SDL_LockMutex(gs_p_stDeviceLock);
	if (hDevice != NULL && hDevice == gs_p_stStream)
		SDL_ClearAudioStream(gs_p_stStream);
	SDL_UnlockMutex(gs_p_stDeviceLock);
	return 0;
}

unsigned long SND_fn_ulCloseDevice(void *hDevice)
{
	SndBool bClosed;

	bClosed = FALSE;
	SDL_LockMutex(gs_p_stDeviceLock);
	if (hDevice != NULL && hDevice == gs_p_stStream)
	{
		/* after SDL_Quit the stream is already gone with the audio subsystem */
		if (SDL_WasInit(SDL_INIT_AUDIO))
			SDL_DestroyAudioStream(gs_p_stStream);
		gs_p_stStream = NULL;
		bClosed = TRUE;
	}
	SDL_UnlockMutex(gs_p_stDeviceLock);
	if (bClosed && SDL_WasInit(SDL_INIT_AUDIO))
		SDL_QuitSubSystem(SDL_INIT_AUDIO);
	return 0;
}

unsigned long SND_fn_ulGetDeviceErrorText(unsigned long ulError,char *szText,unsigned long ulSize)
{
	(void)ulError;
	SDL_strlcpy(szText,gs_szError,ulSize);
	return 0;
}

long SND_fn_lGetQueuedDevice(void)
{
	long lQueued;

	lQueued = -1;
	SDL_LockMutex(gs_p_stDeviceLock);
	if (gs_p_stStream != NULL)
		lQueued = SDL_GetAudioStreamQueued(gs_p_stStream);
	SDL_UnlockMutex(gs_p_stDeviceLock);
	return lQueued;
}

/*---- output thread */

static int SDLCALL fn_iOutputThread(void *pvParam)
{
	(void)pvParam;
	while (!SDL_GetAtomicInt(&gs_stStopOutput))
	{
		gs_p_fn_vFeed();
		SDL_Delay(C_ulFeedPeriodMs);
	}
	return 0;
}

int SND_fn_bStartOutputThread(void (*pfnFeed)(void))
{
	fn_vCreateDeviceLock();
	if (gs_p_stDeviceLock == NULL)
		return FALSE;
	gs_p_fn_vFeed = pfnFeed;
	SDL_SetAtomicInt(&gs_stStopOutput,0);
	gs_p_stOutputThread = SDL_CreateThread(fn_iOutputThread,"SndOutput",NULL);
	return gs_p_stOutputThread != NULL;
}

void SND_fn_vStopOutputThread(void)
{
	if (gs_p_stOutputThread == NULL)
		return;
	SDL_SetAtomicInt(&gs_stStopOutput,1);
	SDL_WaitThread(gs_p_stOutputThread,NULL);
	gs_p_stOutputThread = NULL;
}

/*---- volume (pc_mixervol.c's functions, 0..100) */

long SND_pc_fn_lOpenMixerVolume(SND_pc_tdhDevice hWaveOut)
{
	(void)hWaveOut;
	return 0;
}

unsigned long SND_pc_fn_ulGetMixerVolumeMax(void)
{
	return 100;
}

unsigned long SND_pc_fn_ulGetMixerVolumeMin(void)
{
	return 0;
}

unsigned long SND_pc_fn_ulGetMixerVolume(void)
{
	return gs_ulVolume;
}

void SND_pc_fn_vSetMixerVolume(unsigned long ulVolume)
{
	gs_ulVolume = (ulVolume > 100) ? 100 : ulVolume;
	SDL_LockMutex(gs_p_stDeviceLock);
	if (gs_p_stStream != NULL)
		SDL_SetAudioStreamGain(gs_p_stStream,(float)gs_ulVolume / 100.0f);
	SDL_UnlockMutex(gs_p_stDeviceLock);
}

long SND_pc_fn_lIsMixerVolumeOk(void)
{
	return 1;
}

long SND_pc_fn_lCloseMixerVolume(void)
{
	return 0;
}

/*---- locks, events, timing, the callback thread, the error box (the SND_pc_M_* macros of SND/snddef.h) */
#define C_ulBreakWaitMs		50000	/* SND_pc_fn_ulWaitObjectOrBreak's limit before it breaks */

typedef struct SND_tdstEvent_
{
	SDL_Mutex		*p_stLock;
	SDL_Condition	*p_stCondition;
	int				bSet;
} SND_tdstEvent;

typedef struct SND_tdstCallbackThread_
{
	void			(*pfnProc)(void *);
	void			*pvParam;
} SND_tdstCallbackThread;

void *SND_fn_hCreateLock(int bOwned)
{
	SDL_Mutex *p_stLock = SDL_CreateMutex();
	if (p_stLock != NULL && bOwned)
		SDL_LockMutex(p_stLock);
	return p_stLock;
}

unsigned long SND_fn_ulWaitLock(void *hLock,unsigned long ulMs)
{
	Uint64 ullStart;

	if (ulMs == SND_C_ulWaitForever)
	{
		SDL_LockMutex((SDL_Mutex *)hLock);
		return 0;
	}
	ullStart = SDL_GetTicks();
	while (!SDL_TryLockMutex((SDL_Mutex *)hLock))
	{
		if (SDL_GetTicks() - ullStart >= ulMs)
			return SND_C_ulWaitTimedOut;
		SDL_Delay(1);
	}
	return 0;
}

int SND_fn_bReleaseLock(void *hLock)
{
	SDL_UnlockMutex((SDL_Mutex *)hLock);
	return 1;
}

int SND_fn_bDeleteLock(void *hLock)
{
	SDL_DestroyMutex((SDL_Mutex *)hLock);
	return 1;
}

void *SND_fn_hCreateEvent(void)
{
	SND_tdstEvent *p_stEvent = (SND_tdstEvent *)SDL_calloc(1,sizeof(SND_tdstEvent));

	if (p_stEvent == NULL)
		return NULL;
	p_stEvent->p_stLock = SDL_CreateMutex();
	p_stEvent->p_stCondition = SDL_CreateCondition();
	if (p_stEvent->p_stLock == NULL || p_stEvent->p_stCondition == NULL)
	{
		SND_fn_bDeleteEvent(p_stEvent);
		return NULL;
	}
	return p_stEvent;
}

int SND_fn_bSetEvent(void *hEvent)
{
	SND_tdstEvent *p_stEvent = (SND_tdstEvent *)hEvent;

	SDL_LockMutex(p_stEvent->p_stLock);
	p_stEvent->bSet = 1;
	SDL_SignalCondition(p_stEvent->p_stCondition);
	SDL_UnlockMutex(p_stEvent->p_stLock);
	return 1;
}

unsigned long SND_fn_ulWaitEventOrBreak(void *hEvent)
{
	SND_tdstEvent *p_stEvent = (SND_tdstEvent *)hEvent;
	Uint64 ullStart = SDL_GetTicks();
	Uint64 ullElapsed;
	unsigned long ulResult = 0;

	SDL_LockMutex(p_stEvent->p_stLock);
	while (!p_stEvent->bSet)
	{
		ullElapsed = SDL_GetTicks() - ullStart;
		if (ullElapsed >= C_ulBreakWaitMs)
		{
			ulResult = SND_C_ulWaitTimedOut;
			break;
		}
		SDL_WaitConditionTimeout(p_stEvent->p_stCondition,p_stEvent->p_stLock,(Sint32)(C_ulBreakWaitMs - ullElapsed));
	}
	p_stEvent->bSet = 0;	/* auto-reset */
	SDL_UnlockMutex(p_stEvent->p_stLock);
	if (ulResult == SND_C_ulWaitTimedOut)
		SND_fn_vBreak();
	return ulResult;
}

int SND_fn_bDeleteEvent(void *hEvent)
{
	SND_tdstEvent *p_stEvent = (SND_tdstEvent *)hEvent;

	if (p_stEvent == NULL)
		return 0;
	if (p_stEvent->p_stCondition != NULL)
		SDL_DestroyCondition(p_stEvent->p_stCondition);
	if (p_stEvent->p_stLock != NULL)
		SDL_DestroyMutex(p_stEvent->p_stLock);
	SDL_free(p_stEvent);
	return 1;
}

void SND_fn_vInitCriticalSection(void **p_hCs)
{
	*p_hCs = SDL_CreateMutex();
}

void SND_fn_vDeleteCriticalSection(void **p_hCs)
{
	SDL_DestroyMutex((SDL_Mutex *)*p_hCs);
	*p_hCs = NULL;
}

void SND_fn_vEnterCriticalSection(void **p_hCs)
{
	SDL_LockMutex((SDL_Mutex *)*p_hCs);
}

void SND_fn_vLeaveCriticalSection(void **p_hCs)
{
	SDL_UnlockMutex((SDL_Mutex *)*p_hCs);
}

int SND_fn_bGetPerfCounter(SND_tdstPerfCounter *p_stCounter)
{
	p_stCounter->QuadPart = (long long)SDL_GetPerformanceCounter();
	return 1;
}

int SND_fn_bGetPerfFrequency(SND_tdstPerfCounter *p_stFrequency)
{
	p_stFrequency->QuadPart = (long long)SDL_GetPerformanceFrequency();
	return 1;
}

static int SDLCALL fn_iRunCallbackThread(void *pvData)
{
	SND_tdstCallbackThread stThread = *(SND_tdstCallbackThread *)pvData;

	SDL_free(pvData);
	stThread.pfnProc(stThread.pvParam);
	return 0;
}

void SND_fn_vStartCallbackThread(void (*pfnProc)(void *),void *pvParam)
{
	SND_tdstCallbackThread *p_stThread = (SND_tdstCallbackThread *)SDL_malloc(sizeof(SND_tdstCallbackThread));
	SDL_Thread *p_stSdlThread;

	if (p_stThread == NULL)
		return;
	p_stThread->pfnProc = pfnProc;
	p_stThread->pvParam = pvParam;
	p_stSdlThread = SDL_CreateThread(fn_iRunCallbackThread,"SndCallback",p_stThread);
	if (p_stSdlThread == NULL)
	{
		SDL_free(p_stThread);
		return;
	}
	SDL_DetachThread(p_stSdlThread);
}

void SND_fn_vErrorBox(const char *szText,const char *szCaption)
{
	SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR,szCaption,szText,NULL);
}

void SND_fn_vBreak(void)
{
	SDL_TriggerBreakpoint();
}

#endif /* USE_SDL */
