#ifndef _SNDMIX_PC_H_
#define _SNDMIX_PC_H_

struct _SND_pc_tdstSampleVoice;

/* 0x100086f0 */
void	SND_pc_fn_vClearReverbSend(long lOffset,long lLength);

void	SND_pc_fn_vMixMono16NativeSend(struct _SND_pc_tdstSampleVoice *pVoice,char *pBuffer,long lLength,long bSide,long lOffset,long lUnused);

void	SND_pc_fn_vMixMono16StepSend(struct _SND_pc_tdstSampleVoice *pVoice,char *pBuffer,long lLength,long bSide,long lOffset,long lUnused);
/* 0x10007e60 */
void	SND_pc_fn_vMixReverbFir32(char *pBuffer,short *pwCoefs,long lLength,long lUnused);

void	SND_pc_fn_vMixFirHistory(char *pSrc,char *pDst,short *pwCoefs,long lNbFrames,long lLength);

void	SND_pc_fn_vApplyEchoDelay(char *pBuffer,long lLength,long lMode);

void	SND_pc_fn_vApplySweptComb(char *pBuffer,long lLength,long lUnused);

void	SND_pc_fn_vApplySideEcho(char *pBuffer,long lLength);

void	SND_pc_fn_vFillBuffer(char *pBuffer,long lValue,long lLength);

/* 0x100083f0 */
void	SND_pc_fn_vMixMono16Native(struct _SND_pc_tdstSampleVoice *pVoice,char *pBuffer,long lLength,long lUnused,long bAdd);

void	SND_pc_fn_vMixMono16HalfRate(struct _SND_pc_tdstSampleVoice *pVoice,char *pBuffer,long lLength,long lUnused,long bAdd);

void	SND_pc_fn_vMixMono16DoubleRate(struct _SND_pc_tdstSampleVoice *pVoice,char *pBuffer,long lLength,long lUnused,long bAdd);

void	SND_pc_fn_vMixMono16Step(struct _SND_pc_tdstSampleVoice *pVoice,char *pBuffer,long lLength,long bSide,long bAdd);

void	SND_pc_fn_vMixMono16StepSwapPan(struct _SND_pc_tdstSampleVoice *pVoice,char *pBuffer,long lLength,long bSide,long bAdd);

void	SND_pc_fn_vMixMono16Echo(struct _SND_pc_tdstSampleVoice *pVoice,char *pBuffer,long lLength,long lSlot,long lUnused);

void	SND_pc_fn_vMixMono16EchoSide(struct _SND_pc_tdstSampleVoice *pVoice,char *pBuffer,long lLength,long lSlot,long lUnused);

void	SND_pc_fn_vMixMono16Comb(struct _SND_pc_tdstSampleVoice *pVoice,char *pBuffer,long lLength,long lSlot,long lUnused);

/* 0x10008360 */
void	SND_pc_fn_vMixStereo16Native(struct _SND_pc_tdstSampleVoice *pVoice,char *pBuffer,long lLength,long bSide,long bAdd);

void	SND_pc_fn_vMixStereo16HalfRate(struct _SND_pc_tdstSampleVoice *pVoice,char *pBuffer,long lLength,long bSide,long bAdd);

void	SND_pc_fn_vMixStereo16DoubleRate(struct _SND_pc_tdstSampleVoice *pVoice,char *pBuffer,long lLength,long bSide,long bAdd);

void	SND_pc_fn_vMixStereo16Step(struct _SND_pc_tdstSampleVoice *pVoice,char *pBuffer,long lLength,long bSide,long bAdd);

#define SND_pc_g_a_wReverbSend		SND_pc_a2B160_2222
#define SND_pc_g_pReverbSendCursor	SND_pc_a27C90[3]
#define SND_pc_g_lSweptCombIndex	SND_pc_a27C90[0]
#define SND_pc_g_lEchoLoopCount		SND_pc_a27C90[2]
#define SND_pc_g_lEchoRingIndex		SND_pc_l27C8C
#define SND_pc_g_a_wEchoRing		SND_pc_a45E20
#define SND_pc_g_a_wSweptCombRing	SND_pc_a3DE00_8880
#define SND_pc_g_a_wSideBuffer		SND_pc_a560A0_9930
#define SND_pc_g_a_wVoiceEchoRing	SND_pc_a35DA0_5989
#define SND_pc_g_a_lVoiceEchoIndex	SND_pc_a35D80_779
#define SND_pc_g_a_lVoiceEchoSideIndex	SND_pc_a35D90_618
#define SND_pc_g_a_wVoiceCombRing	SND_pc_a4E080
#define SND_pc_g_a_lVoiceCombIndex	SND_pc_a45E18
#define SND_pc_g_a_lVoiceCombDelay	SND_pc_a81020_10
#define SND_pc_g_lSweepCounter		SND_pc_l21E30
#define SND_pc_g_lSweepDelay		SND_pc_l21E34
#define SND_pc_g_lSweepStep			SND_pc_l21E38
#define SND_pc_g_a_wFirScratch0		SND_pc_q21FA0
#define SND_pc_g_a_wFirScratch1		SND_pc_q21FB0
#define SND_pc_g_a_wFirHistory		SND_pc_a21FC0

#endif
