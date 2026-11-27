#ifndef _SNDMIXVOL_PC_H_
#define _SNDMIXVOL_PC_H_

long			SND_pc_fn_lOpenMixerVolume(SND_pc_tdhDevice hWaveOut);
unsigned long	SND_pc_fn_ulGetMixerVolumeMax(void);
unsigned long	SND_pc_fn_ulGetMixerVolumeMin(void);
unsigned long	SND_pc_fn_ulGetMixerVolume(void);
void			SND_pc_fn_vSetMixerVolume(unsigned long ulVolume);
long			SND_pc_fn_lIsMixerVolumeOk(void);
long			SND_pc_fn_lCloseMixerVolume(void);

#endif
