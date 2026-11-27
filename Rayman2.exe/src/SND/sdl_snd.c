/*
 =======================================================================================
  Name        : sdl_snd.c
  Description : The system calls of the exe's sound module through SDL (USE_SDL)

  The SDL side of the SND_pc_M_* macros of SND/snddef.h (SND/sdl_snd.h).
  - Locks and events: SndThrd.c waits for the sound mutex or the error-display event in one
    call (WaitForMultipleObjects on PC). Here both are records under one guard mutex with one
    condition: every release, set or reset wakes the waiters, which look again at the records
    they wait for. A lock records its owner thread and its depth (recursive, as the Win32
    mutex); an event its state (manual reset: a wait does not clear it).
  - Thread ids: SDL's id of the calling thread, the same id MMG's channels and ERM's error
    slots get (MMG/sdl_mmg.c).
  - Sequence callback threads: an SDL thread, detached: the PC code never waits for or
    closes the handle it gets, it only tests it against NULL.
  - Driver tests: SDL plays any of the PCM formats the drivers ask for (it converts), so the
    output answers every format bit; the platform answers as Windows NT does today, so the
    sound initialisation goes the way it goes on the PC.
 =======================================================================================
 */
#if defined(USE_SDL)

#include <errno.h>
#include <SDL3/SDL.h>

#include "SND/sdl_snd.h"

#define C_ulPlatformWin32NT		2		/* OSVERSIONINFOA dwPlatformId of Windows NT */
#define C_ulAllPcmFormats		0xFFF	/* WAVE_FORMAT_1M08 .. WAVE_FORMAT_4S16 */

typedef struct SND_tdstWaitableSnd_
{
	int				bEvent;		/* 0: lock, 1: event */
	unsigned long	ulOwner;	/* lock: thread holding it (0: free) */
	unsigned long	ulDepth;	/* lock: times its owner took it */
	int				bSet;		/* event: set */
} SND_tdstWaitableSnd;

typedef struct SND_tdstThreadSnd_
{
	SND_tdpfnThreadProcSnd	pfnProc;
	void					*pvParam;
} SND_tdstThreadSnd;

static SDL_InitState	gs_stGuardInit;
static SDL_Mutex		*gs_p_stGuard = NULL;		/* guards every lock and event record */
static SDL_Condition	*gs_p_stChanged = NULL;		/* a lock was released or an event changed */

/*---- locks and events */

static int fn_bInitGuard(void)
{
	if (SDL_ShouldInit(&gs_stGuardInit))
	{
		gs_p_stGuard = SDL_CreateMutex();
		gs_p_stChanged = SDL_CreateCondition();
		SDL_SetInitialized(&gs_stGuardInit,gs_p_stGuard != NULL && gs_p_stChanged != NULL);
	}
	return gs_p_stGuard != NULL && gs_p_stChanged != NULL;
}

static void *fn_hCreateWaitable(int bEvent,unsigned long ulOwner)
{
	SND_tdstWaitableSnd *p_stObject;

	if (!fn_bInitGuard())
		return NULL;
	p_stObject = (SND_tdstWaitableSnd *)SDL_calloc(1,sizeof(SND_tdstWaitableSnd));
	if (p_stObject == NULL)
		return NULL;
	p_stObject->bEvent = bEvent;
	p_stObject->ulOwner = ulOwner;
	p_stObject->ulDepth = ulOwner != 0 ? 1 : 0;
	return p_stObject;
}

/* under the guard: take the object if it is free for this thread */
static int fn_bTakeWaitable(SND_tdstWaitableSnd *p_stObject,unsigned long ulThread)
{
	if (p_stObject->bEvent)
		return p_stObject->bSet;
	if (p_stObject->ulDepth != 0 && p_stObject->ulOwner != ulThread)
		return 0;
	p_stObject->ulOwner = ulThread;
	p_stObject->ulDepth++;
	return 1;
}

void *SND_fn_hCreateLockSnd(int bOwned)
{
	return fn_hCreateWaitable(0,bOwned ? SND_fn_ulGetCurrentThreadIdSnd() : 0);
}

int SND_fn_bReleaseLockSnd(void *hLock)
{
	SND_tdstWaitableSnd *p_stLock = (SND_tdstWaitableSnd *)hLock;
	int bDone = 0;

	if (p_stLock == NULL)
		return 0;
	SDL_LockMutex(gs_p_stGuard);
	if (p_stLock->ulDepth != 0 && p_stLock->ulOwner == SND_fn_ulGetCurrentThreadIdSnd())
	{
		if (--p_stLock->ulDepth == 0)
			p_stLock->ulOwner = 0;
		bDone = 1;
	}
	SDL_BroadcastCondition(gs_p_stChanged);
	SDL_UnlockMutex(gs_p_stGuard);
	return bDone;
}

int SND_fn_bDeleteLockSnd(void *hLock)
{
	if (hLock == NULL)
		return 0;
	SDL_free(hLock);
	return 1;
}

void *SND_fn_hCreateManualEventSnd(void)
{
	return fn_hCreateWaitable(1,0);
}

static int fn_bSetEventState(void *hEvent,int bSet)
{
	if (hEvent == NULL)
		return 0;
	SDL_LockMutex(gs_p_stGuard);
	((SND_tdstWaitableSnd *)hEvent)->bSet = bSet;
	SDL_BroadcastCondition(gs_p_stChanged);
	SDL_UnlockMutex(gs_p_stGuard);
	return 1;
}

int SND_fn_bSetEventSnd(void *hEvent)
{
	return fn_bSetEventState(hEvent,1);
}

int SND_fn_bResetEventSnd(void *hEvent)
{
	return fn_bSetEventState(hEvent,0);
}

int SND_fn_bDeleteEventSnd(void *hEvent)
{
	return SND_fn_bDeleteLockSnd(hEvent);
}

unsigned long SND_fn_ulWaitAnySnd(unsigned long ulNb,void **a_hObjects,unsigned long ulMs)
{
	unsigned long ulThread = SND_fn_ulGetCurrentThreadIdSnd();
	Uint64 ullStart = SDL_GetTicks();
	Uint64 ullElapsed;
	unsigned long ulIndex;

	SDL_LockMutex(gs_p_stGuard);
	for (;;)
	{
		for (ulIndex = 0; ulIndex < ulNb; ulIndex++)
		{
			if (fn_bTakeWaitable((SND_tdstWaitableSnd *)a_hObjects[ulIndex],ulThread))
			{
				SDL_UnlockMutex(gs_p_stGuard);
				return ulIndex;
			}
		}
		if (ulMs == SND_C_ulWaitForeverSnd)
		{
			SDL_WaitCondition(gs_p_stChanged,gs_p_stGuard);
			continue;
		}
		ullElapsed = SDL_GetTicks() - ullStart;
		if (ullElapsed >= ulMs)
			break;
		SDL_WaitConditionTimeout(gs_p_stChanged,gs_p_stGuard,(Sint32)(ulMs - ullElapsed));
	}
	SDL_UnlockMutex(gs_p_stGuard);
	return SND_C_ulWaitTimedOutSnd;
}

unsigned long SND_fn_ulWaitOneSnd(void *hObject,unsigned long ulMs)
{
	return SND_fn_ulWaitAnySnd(1,&hObject,ulMs);
}

/*---- critical section: a lock record, so that leaving it unheld does nothing (SndThrd.c's
 * SND_pc_fn_vDesInitErrorDisplayCs leaves it before deleting it, held or not) */

void SND_fn_vInitCriticalSectionSnd(void **p_hCs)
{
	*p_hCs = SND_fn_hCreateLockSnd(0);
}

void SND_fn_vDeleteCriticalSectionSnd(void **p_hCs)
{
	SND_fn_bDeleteLockSnd(*p_hCs);
	*p_hCs = NULL;
}

void SND_fn_vEnterCriticalSectionSnd(void **p_hCs)
{
	SND_fn_ulWaitOneSnd(*p_hCs,SND_C_ulWaitForeverSnd);
}

void SND_fn_vLeaveCriticalSectionSnd(void **p_hCs)
{
	SND_fn_bReleaseLockSnd(*p_hCs);
}

/*---- thread, time, errors */

void SND_fn_vSleepSnd(unsigned long ulMs)
{
	SDL_Delay((Uint32)ulMs);
}

unsigned long SND_fn_ulGetCurrentThreadIdSnd(void)
{
	return (unsigned long)SDL_GetCurrentThreadID();
}

/* the C runtime's last error (the sound files of SND/SND_file.c set it) */
unsigned long SND_fn_ulGetLastErrorSnd(void)
{
	return (unsigned long)errno;
}

static int SDLCALL fn_iRunThread(void *pvData)
{
	SND_tdstThreadSnd stThread = *(SND_tdstThreadSnd *)pvData;

	SDL_free(pvData);
	stThread.pfnProc(stThread.pvParam);
	return 0;
}

/* the result is only a token for the callers' NULL test: the thread is detached */
void *SND_fn_hCreateThreadSnd(SND_tdpfnThreadProcSnd pfnProc,void *pvParam,unsigned long *p_ulId)
{
	SND_tdstThreadSnd *p_stThread = (SND_tdstThreadSnd *)SDL_malloc(sizeof(SND_tdstThreadSnd));
	SDL_Thread *p_stSdlThread;

	if (p_stThread == NULL)
		return NULL;
	p_stThread->pfnProc = pfnProc;
	p_stThread->pvParam = pvParam;
	p_stSdlThread = SDL_CreateThread(fn_iRunThread,"SndSeqCallback",p_stThread);
	if (p_stSdlThread == NULL)
	{
		SDL_free(p_stThread);
		return NULL;
	}
	if (p_ulId != NULL)
		*p_ulId = (unsigned long)SDL_GetThreadID(p_stSdlThread);
	SDL_DetachThread(p_stSdlThread);
	return (void *)p_stSdlThread;
}

/*---- error box, debugger break, driver tests */

void SND_fn_vAssertBoxSnd(const char *szText,const char *szCaption)
{
	SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR,szCaption,szText,NULL);
}

void SND_fn_vBreakSnd(void)
{
	SDL_TriggerBreakpoint();
}

int SND_fn_bGetOsVersionSnd(SND_tdstOsVersionSnd *p_stVersion)
{
	p_stVersion->dwPlatformId = C_ulPlatformWin32NT;
	return 1;
}

unsigned long SND_fn_ulGetDeviceCapsSnd(SND_tdstDeviceCapsSnd *p_stCaps)
{
	p_stCaps->dwFormats = C_ulAllPcmFormats;
	return 0;
}

#endif /* USE_SDL */
