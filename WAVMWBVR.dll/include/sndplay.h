#ifndef _SNDPLAY_H_
#define _SNDPLAY_H_

#ifdef __cplusplus
extern "C" {
#endif

#undef CPA_EXPORT

#if defined(CPA_WANTS_IMPORT)
#define CPA_EXPORT __declspec(dllimport)
#elif defined(CPA_WANTS_EXPORT)
#define CPA_EXPORT __declspec(dllexport)
#else
#define CPA_EXPORT
#endif

 CPA_EXPORT long SND_fn_lPlaySnd(tduRefRes num_res,SoundParam* par,long prio,SND_td_pfn_vSoundCallback fn_callback,long par_callback,SndBool* active);
 CPA_EXPORT SndBool SND_CALL SND_fn_bSetParamSnd(long voie,SoundParam* par);
 CPA_EXPORT SndBool SND_fn_bTestIsPlayingSnd(long voie);
 CPA_EXPORT void SND_CALL SND_fn_vStopSnd(long voie);
 CPA_EXPORT void SND_CALL SND_fn_vPauseSnd(long voie);
 CPA_EXPORT void SND_CALL SND_fn_vResumeSnd(long voie);
 CPA_EXPORT void SND_fn_vRemoveCallbackSnd(long voie);
 CPA_EXPORT SndReal SND_CALL SND_fn_rGetPosSnd(long voice);
 CPA_EXPORT SndReal SND_CALL SND_fn_rGetLengthSnd(long voice);

 CPA_EXPORT void SND_fn_vPauseAllSnd(void);
 CPA_EXPORT void SND_fn_vResumeAllSnd(void);
 CPA_EXPORT void SND_fn_bDoFadeSndTheme(long voice);

 CPA_EXPORT void SND_fn_vSetSoundVolumeSnd(char new_vol);
 CPA_EXPORT unsigned char SND_fn_ucGetSoundVolumeSnd(void);
 CPA_EXPORT void SND_fn_vSetStereoSnd(SndBool active);
 CPA_EXPORT SndBool SND_fn_bGetStereoSnd(void);
 CPA_EXPORT	SndBool	SND_fn_bIsMusicPlayingSnd(void);
 CPA_EXPORT void SND_fn_vSetReverseStereoSnd(SndBool active);
 CPA_EXPORT SndBool SND_fn_bGetReverseStereoSnd(void);

 CPA_EXPORT void SND_fn_vSetDopplerFactorSnd(SndReal factor);
 CPA_EXPORT void SND_fn_vSetDefaultRollOffSnd(RollOffParam *rolloff);

 CPA_EXPORT int SND_fn_iInitSnd(SND_tdstInitStruct *pInitStruct);
 CPA_EXPORT SndBool SND_fn_bTestFinInitSnd(void);
 CPA_EXPORT void SND_fn_vDesInitSnd(void);

 CPA_EXPORT int SND_fn_iInitBnkSnd(long num);
 CPA_EXPORT SndBool SND_fn_bTestFinInitBnkSnd(long num);
 CPA_EXPORT void SND_fn_vDesInitBnkSnd(long num);
 CPA_EXPORT void SND_fn_vDesInitAllBnkSnd(void);

 CPA_EXPORT void SND_fn_vSetEffectSnd(long id);

 CPA_EXPORT long SND_fn_lCreateMicroSnd(MicroParam* par);
 CPA_EXPORT void SND_fn_vSetMicroParamSnd(long sndId,MicroParam* par);
 CPA_EXPORT void SND_fn_vDestroyMicroSnd(long sndId);

#ifndef NO_ACP_LDBIN
 CPA_EXPORT SndBool SND_fn_bLoadResBinarySnd(tdstBlockResourceDisk *disk,tdstBlockResourceMem *mem,char *pDataBloc);
#endif

 CPA_EXPORT void SND_fn_vUnLoadResSnd(tdstBlockResourceMem* mem);
 CPA_EXPORT void SND_fn_vStopBeforeUnLoadResSnd(tdstBlockResourceMem* mem);

 CPA_EXPORT void SND_fn_vConvertOneResDiskToMem(tdstBlockResourceDisk *disk,
																		tdstBlockResourceMem *mem,
																		void* ptrBegin);

 CPA_EXPORT SndBool SND_fn_bIsResourceSwitch(tduRefRes res);

 CPA_EXPORT SoundParam* SND_fn_pCreateSoundParam(int nb_micro);
 CPA_EXPORT void SND_fn_vCopySoundParam(SoundParam*_pDest,SoundParam* _pSrc);
 CPA_EXPORT void SND_fn_vDestroySoundParam(SoundParam* _pObj);
	void invalidate_ptrvalid_snd(long voice);

 CPA_EXPORT long SND_fn_lPlayTransitionSnd(tduRefRes FirstRes,tduRefRes NextRes,SND_td_pfn_vSoundCallback transition_callback,long param_callback,SoundParam* par);
 CPA_EXPORT SndBool SND_fn_bSetNextTransitionSnd(long voice,tduRefRes new_res);
 CPA_EXPORT SndBool SND_fn_bDoTransitionWithFadeSnd(long voice,tduRefRes new_res);
 CPA_EXPORT SndBool SND_fn_bDoTransitionWithFadeSnd2( long voice, tduRefRes new_res, SndReal rDuration );
 CPA_EXPORT void SND_fn_vStopTransitionSnd(long voice);
 CPA_EXPORT void SND_fn_vPauseTransitionSnd(long voice);
 CPA_EXPORT void SND_fn_vResumeTransitionSnd(long voice);
 CPA_EXPORT SndBool SND_fn_bSetParamTransitionSnd(long voice,SoundParam *par);

CPA_EXPORT void SND_fn_vSynchroSnd();

CPA_EXPORT void SND_fn_vReleaseDriverSnd();
CPA_EXPORT void SND_fn_vRestoreDriverSnd();
CPA_EXPORT void SND_fn_vForceReleaseDriverSnd();
CPA_EXPORT void SND_fn_vForceRestoreDriverSnd();

CPA_EXPORT void SND_fn_vGetTargetLabelSnd(char* name,int nb_char);
CPA_EXPORT void SND_fn_vSetupTargetSnd();
CPA_EXPORT SndBool SND_fn_bCanSetupTargetSnd();

CPA_EXPORT long SND_fn_lCreateNewBufferSnd(unsigned long nb_samples,unsigned short uwResolution,unsigned short uwNbChannels,unsigned long ulFreq,SoundParam* par,td_pfn_vRefreshBufferClient callback,long user_id);
CPA_EXPORT void	SND_fn_vDeleteBufferSnd(long id_buffer);
CPA_EXPORT void SND_fn_vPauseBufferSnd(long id_buffer);
CPA_EXPORT void SND_fn_vResumeBufferSnd(long id_buffer);
CPA_EXPORT SndReal SND_fn_rGetPosBufferSnd(long id_buffer);
CPA_EXPORT void SND_fn_vSetParamBufferSnd(long id_buffer,SoundParam* par);
CPA_EXPORT long SND_fn_lCreateNewBufferExSnd(SND_tdstFormat* pformat,SND_tdstCallback* pCallback,SoundParam* par,long user_id);
CPA_EXPORT long SND_fn_lGetPosBufferExSnd(long id_buffer);
CPA_EXPORT long SND_fn_lPushBufferExSnd(long id_buffer,SND_tdstStackBuffer* pStack);
CPA_EXPORT tduRefRes SND_fn_tduGetLowestResourceSnd(tduRefRes uHighRes,SoundParam* par,SndBool eval);

CPA_EXPORT long	SND_fn_lGetNbVoiceOptimumSnd();
CPA_EXPORT long	SND_fn_lGetNbVoiceWishedSnd();
CPA_EXPORT void	SND_fn_vSetNbVoiceWishedSnd(long nb);

CPA_EXPORT tduRefRes SND_fn_uGenerateResourceSnd(void* data,SND_tdstFormat* pformat,SndBool loop);
CPA_EXPORT void SND_fn_vDestroyResourceSnd(tduRefRes uRes);

CPA_EXPORT SndBool	SND_fn_bTestFormatRecordSnd(SND_tdstFormat* pformat,int flags);
CPA_EXPORT long		SND_fn_lCreateBufferRecordSnd(SND_tdstFormat* pformat,SND_tdstCallback* pCallback);
CPA_EXPORT SndBool	SND_fn_bIsBufferFullDuplexRecordSnd(long id_buffer);
CPA_EXPORT SndBool	SND_fn_bStartRecordSnd(long id_buffer);
CPA_EXPORT void		SND_fn_vStopRecordSnd(long id_buffer);
CPA_EXPORT SndBool	SND_fn_bIsWorkingRecordSnd(long id_buffer);
CPA_EXPORT long		SND_fn_lPushBufferToRecordSnd(long id_buffer,void* ptr,int size_ptr);
CPA_EXPORT SndBool	SND_fn_bManageMsgForRecordSnd(long p_msg);
CPA_EXPORT unsigned char SND_fn_ucGetInstantVuMeterRecordSnd(void);

#ifdef __cplusplus
}
#endif

#endif
