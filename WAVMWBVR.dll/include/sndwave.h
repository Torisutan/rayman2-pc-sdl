#ifndef _SNDWAVE_H_
#define _SNDWAVE_H_

#ifdef __cplusplus
extern "C" {
#endif

CPA_EXPORT long SND_fn_lPlaySample(tduRefRes res,SampleParam *par,long prio,SND_td_pfn_vSoundCallback fn_callback,long par_callback);
CPA_EXPORT void SND_fn_vRemoveCallbackSample(long voice);
CPA_EXPORT SndBool SND_fn_bSetParamSample(long voice,SampleParam *par);
CPA_EXPORT SndBool SND_fn_bTestIsPlayingSample(long voice);
CPA_EXPORT void SND_fn_vStopSample(long voice);
CPA_EXPORT void SND_fn_vPauseSample(long voice);
CPA_EXPORT void SND_fn_vResumeSample(long voice);
CPA_EXPORT SndReal SND_fn_rGetPosSample(long voice);
CPA_EXPORT SndReal SND_fn_rGetLengthSample(long voice);

CPA_EXPORT void SND_fn_vSetSoundVolumeSample(unsigned char vol);
CPA_EXPORT unsigned char SND_fn_ucGetSoundVolumeSample(void);

CPA_EXPORT void SND_fn_vSetStereoSample(SndBool active);
CPA_EXPORT SndBool SND_fn_bGetStereoSample(void);
CPA_EXPORT void SND_fn_vSetReverseSample(SndBool active);
CPA_EXPORT SndBool SND_fn_bGetReverseSample(void);

CPA_EXPORT int SND_fn_iInitSample(SND_tdstInitStruct *pInitStruct);
CPA_EXPORT SndBool SND_fn_bTestInitSample(void);
CPA_EXPORT void SND_fn_vDesInitSample(void);

CPA_EXPORT int SND_fn_iInitBnkSample(long num);
CPA_EXPORT SndBool SND_fn_bTestInitBnkSample(long num);
CPA_EXPORT void SND_fn_vDesInitBnkSample(long num);
CPA_EXPORT void SND_fn_vDesInitAllBnkSample(void);

CPA_EXPORT void SND_fn_vSetEffectSample(long num);

CPA_EXPORT tduRefRes SND_fn_tdGetResForVoiceSample(long voice);

CPA_EXPORT long SND_fn_lCreateNewBufferSample(unsigned long nb_samples,unsigned short uwResolution,unsigned short uwNbChannels,unsigned long ulFreq,SampleParam* par,td_pfn_vRefreshBufferClient callback,long user_id);
CPA_EXPORT void SND_fn_vSetParamBufferSample(long id_buffer,SampleParam *par);
CPA_EXPORT void SND_fn_vDeleteBufferSample(long id_buffer);
CPA_EXPORT void SND_fn_vPauseBufferSample(long id_buffer);
CPA_EXPORT void SND_fn_vResumeBufferSample(long id_buffer);
CPA_EXPORT void* SND_fn_pvGetPtrBufferSample(long id_buffer);
CPA_EXPORT SndReal SND_fn_rGetPosBufferSample(long id_buffer);
CPA_EXPORT long	SND_fn_lCreateNewBufferExSample(SND_tdstFormat* pformat,SND_tdstCallback* pCallback,SampleParam* par,long user_id);
CPA_EXPORT long	SND_fn_lGetPosBufferExSample(long id_buffer);
CPA_EXPORT long	SND_fn_lPushBufferExSample(long id_buffer,SND_tdstStackBuffer* pStack);

CPA_EXPORT void SND_fn_vConvertResDiskToMemSample(tdstBlockResourceDisk *disk,tdstBlockResourceMem *mem,void* ptrBegin);
CPA_EXPORT SndBool SND_fn_bLoadResScriptSample(tdstBlockResourceDisk *disk,tdstBlockResourceMem *mem);
CPA_EXPORT SndBool SND_fn_bLoadResBinarySample(tdstBlockResourceDisk *disk,tdstBlockResourceMem *mem,char *pDataBloc);

CPA_EXPORT void SND_fn_vUnLoadResSample(tdstBlockResourceMem* mem);
CPA_EXPORT SndBool SND_fn_bIsResLoadedSample(tdstBlockResourceMem *mem);

CPA_EXPORT long SND_fn_lPlayTransitionSample(tduRefRes FirstRes,tduRefRes NextRes,SND_td_pfn_vSoundCallback transition_callback,long param_callback,SampleParam* par);
CPA_EXPORT SndBool SND_fn_bSetParamTransitionSample(long voice,SampleParam* par);
CPA_EXPORT SndBool SND_fn_bSetNextTransitionSample(long voice,tduRefRes new_res);
CPA_EXPORT SndBool SND_fn_bDoTransitionWithFadeSample(long voice,tduRefRes new_res);
CPA_EXPORT SndBool SND_fn_bDoTransitionWithFadeSample2( long voice, tduRefRes new_res, SndReal rDuration );
CPA_EXPORT void SND_fn_vStopTransitionSample(long voice);
CPA_EXPORT void SND_fn_vPauseTransitionSample(long voice);
CPA_EXPORT void SND_fn_vResumeTransitionSample(long voice);

CPA_EXPORT void SND_fn_vReleaseDriverSample(void);
CPA_EXPORT void SND_fn_vRestoreDriverSample(void);
CPA_EXPORT void SND_fn_vForceReleaseDriverSample(void);
CPA_EXPORT void SND_fn_vForceRestoreDriverSample(void);

void SND_fn_vGetTargetLabelSample(char* name,int nb_char);
void SND_fn_vSetupTargetSample(void);
SndBool SND_fn_bCanSetupTargetSample(void);

void SND_fn_vSynchroSample();
void SND_fn_vSetSoundVolumeSample(unsigned char vol);
unsigned char SND_fn_ucGetSoundVolumeSample(void);

long SND_fn_lGetNbVoiceOptimumSample();
long SND_fn_lGetNbVoiceWishedSample();
void SND_fn_vSetNbVoiceWishedSample(long nb);
SndBool SND_fn_bIsResourceLoopingSample(tduRefRes res);
/* 0x1000AEF0 */
SndBool FUN_1000aef0(tduRefRes res,unsigned char ucVolume);

#ifdef __cplusplus
}
#endif

#endif
