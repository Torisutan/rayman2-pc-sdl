#ifndef _SNDADPCM_PC_H_
#define _SNDADPCM_PC_H_

#ifdef __cplusplus
extern "C" {
#endif

#define SND_pc_C_lNbVoicesADPCM			30		/* 0x100E67D8 */
#define SND_pc_C_lNbThemeVoicesADPCM	1		/* 0x100E66F0 */
#define SND_pc_C_lStreamBufferSize		60000	/* 0x100163E8 */
#define SND_pc_C_lMixBufferSamples		3000

typedef struct SND_pc_tdstDecoderStateADPCM_
{
	long	a7_l[7];
} SND_pc_tdstDecoderStateADPCM;

typedef struct SND_pc_tdstHeaderADPCM_
{
	unsigned short					uwFormatTag;
	unsigned short					uwNbChannels;
	unsigned long					ulFreq;
	unsigned long					a3_ulUnused[3];
	unsigned long					ulSignature;
	unsigned long					ulUnused18;
	unsigned long					ulNbSamples;
	unsigned long					ulLoopStart;
	unsigned long					ulLoopLength;
	SND_pc_tdstDecoderStateADPCM	stState;
	SND_pc_tdstDecoderStateADPCM	stLoopState;
	unsigned long					ulUnused60;
} SND_pc_tdstHeaderADPCM;

typedef struct SND_pc_tdstVoiceADPCM_ SND_pc_tdstVoiceADPCM;
struct SND_pc_tdstVoiceADPCM_
{
	short							wNbChannels;
	unsigned short					uwFreq;
	SND_td_pfn_vSoundCallback		fn_callback;
	long							par_callback;
	SndBool							bVolable;
	SndBool							bPanable;
	SndBool							bPitchable;
	long							lStaticVolume;
	SndBool							bPaused;
	SndBool							bStream;
	SndBool							bLoop;
	SndBool							bActive;
	SndBool							bFade;
	SndBool							bFadeDone;
	SndBool							bCallbackDone;
	long							lVolume;
	long							lPan;
	unsigned long					ulStep;
	long							lGainLeft;
	long							lGainRight;
	unsigned long					ulNbSamples;
	unsigned long					ulTime;
	unsigned long					ulLoopStart;
	unsigned long					ulLoopLength;
	unsigned long					ulPos;
	char							*p_cData;
	char							*p_cCurrent;
	char							*p_cEnd;
	tdhSndFile						hFile;
	SND_pc_tdstDecoderStateADPCM	stState;
	SND_pc_tdstDecoderStateADPCM	stCurrentState;
	SND_pc_tdstDecoderStateADPCM	stLoopState;
	SND_pc_tdstVoiceADPCM			*p_stNext;
	unsigned long					ulFadeLength;
	unsigned long					ulFadeStep;
	tdstBlockResourceMem			*p_stRes;
	SND_pc_tdstVoiceADPCM			**pp_stHandle;
	SndBool							bTheme;
	SndBool							bEnded;
	unsigned long					ulEndTime;
};

/* 0x100055C0 */
void	FUN_100055c0(long *p_lMix,long *p_lMixEnd,void **pp_vDest);
void	FUN_10005960(long *p_lMix,long *p_lMixEnd,long *p_lDecoded,SND_pc_tdstVoiceADPCM *p_stVoice);
void	FUN_10005a70(unsigned long ulNbSamples,char **pp_cSrc,long **pp_lDest,SND_pc_tdstDecoderStateADPCM *p_stState);
void	FUN_10005c38(unsigned long ulNbSamples,char **pp_cSrc,long **pp_lDest,SND_pc_tdstDecoderStateADPCM *p_stState);

void	SND_pc_fn_vInitializeCriticalSectionADPCM(LPCRITICAL_SECTION p_stCriticalSection);
void	SND_pc_fn_vDeleteCriticalSectionADPCM(LPCRITICAL_SECTION p_stCriticalSection);
void	SND_pc_fn_vEnterCriticalSectionADPCM(LPCRITICAL_SECTION p_stCriticalSection);
void	SND_pc_fn_vLeaveCriticalSectionADPCM(LPCRITICAL_SECTION p_stCriticalSection);
long	SND_pc_fn_lOpenStreamADPCM(char *szFileName,SND_pc_tdstVoiceADPCM *p_stVoice,SND_pc_tdstHeaderADPCM **pp_stHeader);
long	SND_pc_fn_lCloseStreamADPCM(SND_pc_tdstVoiceADPCM *p_stVoice);
long	FUN_10001150(unsigned long ulNbSamples,SND_pc_tdstVoiceADPCM *p_stVoice);
void	SND_pc_fn_vSetVolumePanADPCM(SND_pc_tdstVoiceADPCM *p_stVoice,long lVolume,long lPan);
SndBool	SND_pc_fn_bIsReverseStereoADPCM(void);
void	SND_pc_fn_vApplyReverseStereoADPCM(SndBool bActive);
void	FUN_100012f0(SND_pc_tdstVoiceADPCM *p_stVoice,SndReal rDuration);
long	FUN_10001360(void);
long	FUN_100013a0(tdstBlockResourceMem *p_stRes,SampleParam *par,SND_td_pfn_vSoundCallback fn_callback,long par_callback,tdstBlockResourceMem *p_stNextRes,SND_pc_tdstVoiceADPCM ***ppp_stHandle,unsigned long ulTime);
long	SND_pc_fn_lStopVoiceADPCM(SND_pc_tdstVoiceADPCM **pp_stHandle);
void	FUN_10001700(SND_pc_tdstVoiceADPCM **pp_stHandle);
long	FUN_10001740(SND_pc_tdstVoiceADPCM *p_stVoice,unsigned long ulTime);
long	FUN_100018d0(SndBool bStream,unsigned long ulNbSamples,void *p_vDest,unsigned long ulTime);
long	FUN_10001bb0(unsigned long ulSignature);
long	FUN_10001c30(SND_pc_tdstVoiceADPCM **pp_stHandle,SampleParam *par);
long	FUN_10001cb0(SND_pc_tdstVoiceADPCM *p_stVoice,tdstBlockResourceMem *p_stRes,SND_pc_tdstHeaderADPCM **pp_stHeader);
long	FUN_10001dc0(SND_pc_tdstVoiceADPCM *p_stVoice,SampleParam *par);
long	SND_pc_fn_lPauseVoiceADPCM(SND_pc_tdstVoiceADPCM **pp_stHandle);
long	SND_pc_fn_lResumeVoiceADPCM(SND_pc_tdstVoiceADPCM **pp_stHandle);
SndBool	SND_pc_fn_bIsVoicePlayingADPCM(SND_pc_tdstVoiceADPCM **pp_stHandle);
long	SND_pc_fn_lRemoveCallbackVoiceADPCM(SND_pc_tdstVoiceADPCM **pp_stHandle);
long	FUN_10001fc0(SND_pc_tdstVoiceADPCM *p_stVoice,SND_pc_tdstVoiceADPCM **pp_stHandle,tdstBlockResourceMem *p_stRes,SND_td_pfn_vSoundCallback fn_callback,long par_callback);
long	FUN_10002040(SND_pc_tdstVoiceADPCM **pp_stVoice);
long	FUN_10002090(SND_pc_tdstVoiceADPCM *p_stVoice,SND_pc_tdstVoiceADPCM *p_stNext);
long	FUN_10002180(SND_pc_tdstVoiceADPCM **pp_stHandle,tdstBlockResourceMem *p_stRes);
SndBool	FUN_10002210(SND_pc_tdstVoiceADPCM **pp_stHandle,tdstBlockResourceMem *p_stRes,SndReal rDuration);
long	FUN_100022c0(SND_pc_tdstVoiceADPCM **pp_stHandle,unsigned long *p_ulLength);
long	FUN_10002320(SND_pc_tdstVoiceADPCM **pp_stHandle,double *p_dPos,unsigned long ulTime);
SndBool	FUN_10002480(SND_pc_tdstVoiceADPCM *p_stVoice,SND_pc_tdstVoiceADPCM **pp_stHandle);
void	FUN_100024e0(SND_pc_tdstVoiceADPCM *p_stVoice);
long	FUN_10002500(tdstBlockResourceDisk *p_stDisk,SND_pc_tdstHeaderADPCM *p_stHeader);
long	FUN_100025a0(SND_pc_tdstHeaderADPCM *p_stHeader,void *p_vDest);
void	FUN_10002680(tdstBlockResourceDisk *disk,tdstBlockResourceMem *mem,void *ptrBegin);
void	SND_CALL SND_pc_fn_vRefreshBufferMemoryADPCM(long user_id,unsigned long first,unsigned long nb,void *ptr_first_sample);
void	SND_CALL SND_pc_fn_vRefreshBufferStreamADPCM(long user_id,unsigned long first,unsigned long nb,void *ptr_first_sample);
SndBool	FUN_10003600(tdstBlockResourceDisk *disk,void *p_vData,tdstBlockResourceDisk *p_stDiskPCM);
SndBool	FUN_10003650(tdstBlockResourceDisk *disk,void *p_vData,void *p_vPCM);

long	SND_pc_fn_lPlayADPCM(tduRefRes res,SampleParam *par,long prio,SND_td_pfn_vSoundCallback fn_callback,long par_callback);
void	SND_pc_fn_vRemoveCallbackADPCM(long voice);
SndBool	SND_pc_fn_bSetParamADPCM(long voice,SampleParam *par);
SndBool	SND_pc_fn_bTestIsPlayingADPCM(long voice);
void	SND_pc_fn_vStopADPCM(long voice);
void	SND_pc_fn_vPauseADPCM(long voice);
void	SND_pc_fn_vResumeADPCM(long voice);
SndBool	SND_pc_fn_bIsResourceLoopingADPCM(tduRefRes res);
SndReal	SND_pc_fn_rGetPosADPCM(long voice);
SndReal	SND_pc_fn_rGetLengthADPCM(long voice);
unsigned char SND_pc_fn_ucGetSoundVolumeADPCM(void);
void	SND_pc_fn_vSetReverseStereoADPCM(SndBool active);
SndBool	SND_pc_fn_bGetReverseStereoADPCM(void);
int		SND_pc_fn_iInitADPCM(SND_tdstInitStruct *pInitStruct);
SndBool	SND_pc_fn_bGetStereoADPCM(void);
SndBool	SND_pc_fn_bTestInitADPCM(void);
void	SND_pc_fn_vDesInitADPCM(void);
long	SND_pc_fn_lCreateNewBufferADPCM(unsigned long nb_samples,unsigned short uwResolution,unsigned short uwNbChannels,unsigned long ulFreq,SampleParam* par,td_pfn_vRefreshBufferClient callback,long user_id);
SndReal	SND_pc_fn_rGetPosBufferADPCM(long id_buffer);
long	SND_pc_fn_lCreateNewBufferExADPCM(SND_tdstFormat* pformat,SND_tdstCallback* pCallback,SoundParam* par,long user_id);
long	SND_pc_fn_lGetPosBufferExADPCM(long id_buffer);
long	SND_pc_fn_lPushBufferExADPCM(long id_buffer,SND_tdstStackBuffer* pStack);
void	SND_pc_fn_vSetSoundVolumeADPCM(unsigned char vol);
void	SND_pc_fn_vSetStereoADPCM(SndBool active);
void	SND_pc_fn_vSetEffectADPCM(long num);
void	SND_pc_fn_vSetParamBufferADPCM(long id_buffer,SampleParam *par);
void	SND_pc_fn_vDeleteBufferADPCM(long id_buffer);
void	SND_pc_fn_vReleaseDriverADPCM(void);
void	SND_pc_fn_vRestoreDriverADPCM(void);
void	SND_pc_fn_vGetTargetLabelADPCM(char* name,int nb_char);
void	SND_pc_fn_vSetupTargetADPCM(void);
void	SND_pc_fn_vSynchroADPCM(void);
void	SND_pc_fn_vPauseBufferADPCM(long id_buffer);
void	SND_pc_fn_vResumeBufferADPCM(long id_buffer);
void	*SND_pc_fn_pvGetPtrBufferADPCM(long id_buffer);
SndBool	SND_pc_fn_bIsResLoadedADPCM(tdstBlockResourceMem *mem);
SndBool	SND_pc_fn_bCanFreeDataADPCM(void);
SndBool	SND_pc_fn_bCanSetupTargetADPCM(void);
SndBool	SND_pc_fn_bLoadResScriptADPCM(tdstBlockResourceDisk *disk,tdstBlockResourceMem *mem);
SndBool	SND_pc_fn_bLoadResBinaryADPCM(tdstBlockResourceDisk *disk,tdstBlockResourceMem *mem,char *pDataBloc);
SndBool	SND_pc_fn_bLoadResScriptAndConvertToWAVADPCM(tdstBlockResourceDisk *disk,tdstBlockResourceMem *mem);
SndBool	SND_pc_fn_bLoadResBinaryAndConvertToWAVADPCM(tdstBlockResourceDisk *disk,tdstBlockResourceMem *mem,char *pDataBloc);
void	SND_pc_fn_vUnLoadResADPCM(tdstBlockResourceMem *mem);
long	SND_pc_fn_lPlayTransitionADPCM(tduRefRes FirstRes,tduRefRes NextRes,SND_td_pfn_vSoundCallback transition_callback,long param_callback,SampleParam *par);
SndBool	SND_pc_fn_bSetParamTransitionADPCM(long voice,SampleParam *par);
SndBool	SND_pc_fn_bSetNextTransitionADPCM(long voice,tduRefRes new_res);
SndBool	SND_pc_fn_bDoTransitionWithFadeADPCM(long voice,tduRefRes new_res,SndReal rDuration);
void	SND_pc_fn_vStopTransitionADPCM(long voice);
void	SND_pc_fn_vPauseTransitionADPCM(long voice);
void	SND_pc_fn_vResumeTransitionADPCM(long voice);
SndBool	SND_pc_fn_bCheckVersionResourceADPCM(tdstBlockResourceDisk *disk);
long	SND_pc_fn_lGetNbVoiceWishedADPCM(void);
SndBool	SND_pc_fn_bSetResourceStaticVolumeADPCM(tduRefRes res,unsigned char ucVolume);

void SND_DllInitEntryPoints_StaticFunctions(HMODULE hModule);
void SND_pc_DllInitEntryPoints_SxdFunctions(HMODULE hModule);
extern HMODULE SND_pc_hWavModule;

void SND_CALL SND_fn_vParam3Dto2D(SoundParam *par3D,SampleParam *par2D);
long SND_CALL SND_fn_lCreateNewBufferExSxd(SND_tdstFormat* pformat,SND_tdstCallback* pCallback,SoundParam* par,long user_id);
void SND_CALL SND_fn_vDeleteBufferSxd(long id_buffer);
void SND_CALL SND_fn_vPauseBufferSxd(long id_buffer);
void SND_CALL SND_fn_vResumeBufferSxd(long id_buffer);
void SND_CALL SND_fn_vConvertResDiskToMemSxd(tdstBlockResourceDisk *disk,tdstBlockResourceMem *mem,void *ptrBegin);

#ifdef __cplusplus
}
#endif

#endif
