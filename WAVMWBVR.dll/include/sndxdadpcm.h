#ifndef _SNDXDADPCM_H_
#define _SNDXDADPCM_H_

#ifdef __cplusplus
extern "C" {
#endif

SndBool SND_CALL SND_fn_bCanFreeDataADPCM(void);
long SND_CALL SND_fn_lPlaySxdADPCM(tduRefRes res,SoundParam *par,long prio,SND_td_pfn_vSoundCallback fn_callback,long par_callback);
void SND_CALL SND_fn_vRemoveCallbackSxdADPCM(long voice);
SndBool SND_CALL SND_fn_bSetParamSxdADPCM(long voice,SoundParam *par);
SndBool SND_CALL SND_fn_bTestIsPlayingSxdADPCM(long voice);
void SND_CALL SND_fn_vStopSxdADPCM(long voice);
void SND_CALL SND_fn_vPauseSxdADPCM(long voice);
void SND_CALL SND_fn_vResumeSxdADPCM(long voice);
SndBool SND_CALL SND_fn_bIsResourceLoopingSxdADPCM(tduRefRes res);
SndReal SND_CALL SND_fn_rGetPosSxdADPCM(long voice);
SndReal SND_CALL SND_fn_rGetLengthSxdADPCM(long voice);
void SND_CALL SND_fn_vSetSoundVolumeSxdADPCM(unsigned char vol);
unsigned char SND_CALL SND_fn_ucGetSoundVolumeSxdADPCM();
void SND_CALL SND_fn_vSetStereoSxdADPCM(SndBool active);
SndBool SND_CALL SND_fn_bGetStereoSxdADPCM(void);
void SND_CALL SND_fn_vSetReverseStereoSxdADPCM(SndBool active);
SndBool SND_CALL SND_fn_bGetReverseStereoSxdADPCM(void);
int SND_CALL SND_fn_iInitSxdADPCM(SND_tdstInitStruct *pInitStruct);
SndBool SND_CALL SND_fn_bTestInitSxdADPCM(void);
void SND_CALL SND_fn_vDesInitSxdADPCM(void);
void SND_CALL SND_fn_vSetEffectSxdADPCM(long num);
void SND_CALL SND_fn_vCreateMicroSxdADPCM(long lParam1,long lParam2);
void SND_CALL SND_fn_vSetMicroParamSxdADPCM(long sxdId,MicroParam* par);
void SND_CALL SND_fn_vDestroyMicroSxdADPCM(long sxdId);
long SND_CALL SND_fn_lCreateNewBufferSxdADPCM(unsigned long nb_samples,unsigned short uwResolution,unsigned short uwNbChannels,unsigned long ulFreq,SoundParam* par,td_pfn_vRefreshBufferClient callback,long user_id);
void SND_CALL SND_fn_vSetParamBufferSxdADPCM(long id_buffer,SoundParam *par);
void SND_CALL SND_fn_vDeleteBufferSxdADPCM(long id_buffer);
void SND_CALL SND_fn_vPauseBufferSxdADPCM(long id_buffer);
void SND_CALL SND_fn_vResumeBufferSxdADPCM(long id_buffer);
void* SND_CALL SND_fn_pvGetPtrBufferSxdADPCM(long id_buffer);
SndReal SND_CALL SND_fn_rGetPosBufferSxdADPCM(long id_buffer);
SndBool SND_CALL SND_fn_bLoadResScriptSxdADPCM(tdstBlockResourceDisk *disk,tdstBlockResourceMem *mem);
SndBool SND_CALL SND_fn_bLoadResBinarySxdADPCM(tdstBlockResourceDisk *disk,tdstBlockResourceMem *mem,char *pDataBloc);
SndBool SND_CALL SND_fn_bLoadResScriptAndConvertToWAVSxdADPCM(tdstBlockResourceDisk *disk,tdstBlockResourceMem *mem); /* 0x1000BAD0 */
SndBool SND_CALL SND_fn_bLoadResBinaryAndConvertToWAVSxdADPCM(tdstBlockResourceDisk *disk,tdstBlockResourceMem *mem,char *pDataBloc); /* 0x1000BB70 */
void SND_CALL SND_fn_vUnLoadResSxdADPCM(tdstBlockResourceMem* mem);
SndBool SND_CALL SND_fn_bIsResLoadedSxdADPCM(tdstBlockResourceMem *mem);
long SND_CALL SND_fn_lPlayTransitionSxdADPCM(tduRefRes FirstRes,tduRefRes NextRes,SND_td_pfn_vSoundCallback transition_callback,long param_callback,SoundParam* par);
SndBool SND_CALL SND_fn_bSetParamTransitionSxdADPCM(long voice,SoundParam *par);
SndBool SND_CALL SND_fn_bSetNextTransitionSxdADPCM(long voice,tduRefRes new_res);
SndBool SND_CALL SND_fn_bDoTransitionWithFadeSxdADPCM(long voice,tduRefRes new_res);
SndBool SND_CALL SND_fn_bDoTransitionWithFadeSxdADPCM2(long voice, tduRefRes new_res, SndReal rDuration);
void SND_CALL SND_fn_vStopTransitionSxdADPCM(long voice);
void SND_CALL SND_fn_vPauseTransitionSxdADPCM(long voice);
void SND_CALL SND_fn_vResumeTransitionSxdADPCM(long voice);
void SND_CALL SND_fn_vReleaseDriverSxdADPCM(void);
void SND_CALL SND_fn_vRestoreDriverSxdADPCM(void);
void SND_CALL SND_fn_vGetTargetLabelSxdADPCM(char* name,int nb_char);
void SND_CALL SND_fn_vSetupTargetSxdADPCM();
SndBool SND_CALL SND_fn_bCanSetupTargetSxdADPCM();
void SND_CALL SND_fn_vSynchroSxdADPCM();
long SND_CALL SND_fn_lCreateNewBufferExSxdADPCM(SND_tdstFormat* pformat,SND_tdstCallback* pCallback,SoundParam* par,long user_id);
long SND_CALL SND_fn_lGetPosBufferExSxdADPCM(long id_buffer);
long SND_CALL SND_fn_lPushBufferExSxdADPCM(long id_buffer,SND_tdstStackBuffer* pStack);
void SND_CALL SND_fn_vSetDefaultRollOffSxdADPCM(RollOffParam* rolloff);
SndBool SND_CALL SND_fn_bCheckVersionResourceSxdADPCM(tdstBlockResourceDisk* disk);
long SND_CALL SND_fn_lGetNbVoiceWishedSxdADPCM();
SndBool SND_CALL SND_fn_bIsLibActiveSxdADPCM(void);
SndBool SND_CALL SND_fn_bSetResourceStaticVolumeSxdADPCM(tduRefRes res, unsigned char ucVolume);

#ifdef __cplusplus
}
#endif

#endif
