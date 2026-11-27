/*
 =======================================================================================
  Name        : sdl_snd.h
  Description : The system calls of the exe's sound module through SDL (USE_SDL)

  The SDL side of the SND_pc_M_* macros of SND/snddef.h: the sound mutex, the error-display
  lock and event (SndThrd.c), the sequence callback threads (sndseq.c), the assert box, the
  debugger break and the driver tests (SND.c). The sample driver has its own (sdl_sndwave.c):
  in the DLL build the exe does not contain it.
  Waits answer as the Win32 waits of the other branch: 0..n-1 = the object that ended the
  wait, SND_C_ulWaitTimedOutSnd after ulMs milliseconds (SND_C_ulWaitForeverSnd: no limit).
 =======================================================================================
 */
#ifndef SND_SDL_SND_H
#define SND_SDL_SND_H

#if defined(USE_SDL)

#define SND_C_ulWaitForeverSnd		0xFFFFFFFFUL
#define SND_C_ulWaitTimedOutSnd		0x102UL

/* OSVERSIONINFOA's layout: SND_fn_bTestSnd_Win32/_WinNT clear 0x25 longs of it */
typedef struct SND_tdstOsVersionSnd_
{
	unsigned long	dwOSVersionInfoSize;
	unsigned long	dwMajorVersion;
	unsigned long	dwMinorVersion;
	unsigned long	dwBuildNumber;
	unsigned long	dwPlatformId;
	char			szCSDVersion[128];
} SND_tdstOsVersionSnd;

/* WAVEFORMATEX's layout: the PCM format a sound driver (the retail DLLs included) hands
   SND_fn_bTestSnd_WinMM */
typedef struct SND_tdstWaveFormatSnd_
{
	unsigned short	wFormatTag;
	unsigned short	nChannels;
	unsigned long	nSamplesPerSec;
	unsigned long	nAvgBytesPerSec;
	unsigned short	nBlockAlign;
	unsigned short	wBitsPerSample;
	unsigned short	cbSize;
} SND_tdstWaveFormatSnd;

/* the field SND_fn_bTestSnd_WinMM reads: one bit per PCM format (WAVE_FORMAT_1M08..4S16) */
typedef struct SND_tdstDeviceCapsSnd_
{
	unsigned long	dwFormats;
} SND_tdstDeviceCapsSnd;

typedef unsigned long (__stdcall *SND_tdpfnThreadProcSnd)(void *pvParam);

/* lock: recursive, owned by one thread, as the Win32 mutex; bOwned = taken by the creating thread */
void *			SND_fn_hCreateLockSnd(int bOwned);
int				SND_fn_bReleaseLockSnd(void *hLock);
int				SND_fn_bDeleteLockSnd(void *hLock);
/* event: manual reset, created not set */
void *			SND_fn_hCreateManualEventSnd(void);
int				SND_fn_bSetEventSnd(void *hEvent);
int				SND_fn_bResetEventSnd(void *hEvent);
int				SND_fn_bDeleteEventSnd(void *hEvent);
/* wait until one of ulNb locks or events is free (a lock is then taken) */
unsigned long	SND_fn_ulWaitAnySnd(unsigned long ulNb,void **a_hObjects,unsigned long ulMs);
unsigned long	SND_fn_ulWaitOneSnd(void *hObject,unsigned long ulMs);
/* critical section: a lock (recursive; leaving it unheld does nothing) */
void			SND_fn_vInitCriticalSectionSnd(void **p_hCs);
void			SND_fn_vDeleteCriticalSectionSnd(void **p_hCs);
void			SND_fn_vEnterCriticalSectionSnd(void **p_hCs);
void			SND_fn_vLeaveCriticalSectionSnd(void **p_hCs);
/* thread, time, errors */
void			SND_fn_vSleepSnd(unsigned long ulMs);
unsigned long	SND_fn_ulGetCurrentThreadIdSnd(void);
unsigned long	SND_fn_ulGetLastErrorSnd(void);
void *			SND_fn_hCreateThreadSnd(SND_tdpfnThreadProcSnd pfnProc,void *pvParam,unsigned long *p_ulId);
/* error box, debugger break, driver tests */
void			SND_fn_vAssertBoxSnd(const char *szText,const char *szCaption);
void			SND_fn_vBreakSnd(void);
int				SND_fn_bGetOsVersionSnd(SND_tdstOsVersionSnd *p_stVersion);
unsigned long	SND_fn_ulGetDeviceCapsSnd(SND_tdstDeviceCapsSnd *p_stCaps);

#endif /* USE_SDL */

#endif /* SND_SDL_SND_H */
