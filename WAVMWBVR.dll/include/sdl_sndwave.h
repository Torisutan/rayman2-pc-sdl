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

#endif /* USE_SDL */

#endif /* _SDL_SNDWAVE_H_ */
