/*
 * sdl_sndwave.h: the sample driver's output through SDL (USE_SDL), beside the waveOut output of
 * sndwave.c. The device functions answer the SND_pc_M_*Device macros of SND/snddef.h; results
 * as waveOut's: 0 = done, anything else an error number for SND_fn_ulGetDeviceErrorText.
 */
#ifndef _SDL_SNDWAVE_H_
#define _SDL_SNDWAVE_H_

#if defined(USE_SDL)

/* output device */
unsigned long	SND_fn_ulOpenDevice(void **p_hDevice,unsigned long ulFreq,unsigned short uwNbChannels,unsigned short uwResolution);
unsigned long	SND_fn_ulWriteDevice(void *hDevice,char *pData,unsigned long ulLength);
unsigned long	SND_fn_ulResetDevice(void *hDevice);
unsigned long	SND_fn_ulCloseDevice(void *hDevice);
unsigned long	SND_fn_ulGetDeviceErrorText(unsigned long ulError,char *szText,unsigned long ulSize);
/* bytes written and not yet played (-1: no device) */
long			SND_fn_lGetQueuedDevice(void);

/* the output thread: calls pfnFeed every few milliseconds until stopped */
int				SND_fn_bStartOutputThread(void (*pfnFeed)(void));
void			SND_fn_vStopOutputThread(void);

/* the system side of the SND_pc_M_* macros: waits return 0 when done, SND_C_ulWaitTimedOut after
 * ulMs milliseconds (SND_C_ulWaitForever: no limit) */
#define SND_C_ulWaitForever		0xFFFFFFFFUL
#define SND_C_ulWaitTimedOut	0x102UL
#define SND_C_ulDriverThreadId	1	/* the one thread id of the SDL output (snddef.h) */

typedef struct SND_tdstPerfCounter_
{
	long long	QuadPart;	/* counter ticks, as the Win32 counter the mix-time statistics read */
} SND_tdstPerfCounter;

/* lock: recursive, as the Win32 mutex it stands for; bOwned = taken by the creating thread */
void *			SND_fn_hCreateLock(int bOwned);
unsigned long	SND_fn_ulWaitLock(void *hLock,unsigned long ulMs);
int				SND_fn_bReleaseLock(void *hLock);
int				SND_fn_bDeleteLock(void *hLock);
/* event: auto-reset, created not set; the wait breaks after 50 s, as SND_pc_fn_ulWaitObjectOrBreak */
void *			SND_fn_hCreateEvent(void);
int				SND_fn_bSetEvent(void *hEvent);
unsigned long	SND_fn_ulWaitEventOrBreak(void *hEvent);
int				SND_fn_bDeleteEvent(void *hEvent);
/* timing, the voice-end callback thread (detached), the error box, the debugger break */
int				SND_fn_bGetPerfCounter(SND_tdstPerfCounter *p_stCounter);
int				SND_fn_bGetPerfFrequency(SND_tdstPerfCounter *p_stFrequency);
void			SND_fn_vStartCallbackThread(void (*pfnProc)(void *),void *pvParam);
void			SND_fn_vErrorBox(const char *szText,const char *szCaption);
void			SND_fn_vBreak(void);

#endif /* USE_SDL */

#endif /* _SDL_SNDWAVE_H_ */
