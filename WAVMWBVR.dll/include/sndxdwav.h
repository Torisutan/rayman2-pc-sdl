#ifndef _SNDXDWAV_H_
#define _SNDXDWAV_H_

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

	CPA_EXPORT long		SND_CALL SND_fn_lPlaySxdWave(tduRefRes res,SoundParam *par,long prio,SND_td_pfn_vSoundCallback fn_callback,long par_callback);
	CPA_EXPORT void		SND_CALL SND_fn_vRemoveCallbackSxdWave(long voice);
	CPA_EXPORT SndBool	SND_CALL SND_fn_bSetParamSxdWave(long voice,SoundParam *par);
	CPA_EXPORT SndBool	SND_CALL SND_fn_bTestIsPlayingSxdWave(long voice);
	CPA_EXPORT void		SND_CALL SND_fn_vStopSxdWave(long voice);
	CPA_EXPORT void		SND_CALL SND_fn_vPauseSxdWave(long voice);
	CPA_EXPORT void		SND_CALL SND_fn_vResumeSxdWave(long voice);

	CPA_EXPORT SndBool  SND_fn_bIsResourceLoopingSxdWave(tduRefRes res);
	CPA_EXPORT SndReal	SND_CALL SND_fn_rGetPosSxdWave(long voice);
	CPA_EXPORT SndReal	SND_CALL SND_fn_rGetLengthSxdWave(long voice);
	CPA_EXPORT void		SND_fn_vSetSoundVolumeSxdWave(unsigned char vol);
	CPA_EXPORT unsigned char SND_fn_ucGetSoundVolumeSxdWave();

	CPA_EXPORT void		SND_fn_vSetStereoSxdWave(SndBool active);
	CPA_EXPORT SndBool	SND_fn_bGetStereoSxdWave(void);
	CPA_EXPORT void		SND_fn_vSetReverseStereoSxdWave(SndBool active);
	CPA_EXPORT SndBool	SND_fn_bGetReverseStereoSxdWave(void);
	CPA_EXPORT void		SND_fn_vSetDopplerFactorSxdWave(SndReal factor);

	CPA_EXPORT int		SND_fn_iInitSxdWave(SND_tdstInitStruct *pInitStruct);
	CPA_EXPORT SndBool	SND_fn_bTestInitSxdWave(void);
	CPA_EXPORT void		SND_fn_vDesInitSxdWave(void);

	CPA_EXPORT void		SND_fn_vSetEffectSxdWave(long num);

	CPA_EXPORT void		SND_fn_vCreateMicroSxdWave(MicroParam* par,long id);
	CPA_EXPORT void		SND_fn_vSetMicroParamSxdWave(long sxdId,MicroParam* par);
	CPA_EXPORT void		SND_fn_vDestroyMicroSxdWave(long sxdId);

	CPA_EXPORT long		SND_fn_lCreateNewBufferSxdWave(unsigned long nb_samples,unsigned short uwResolution,unsigned short uwNbChannels,unsigned long ulFreq,SoundParam* par,td_pfn_vRefreshBufferClient callback,long user_id);
	CPA_EXPORT void		SND_fn_vSetParamBufferSxdWave(long id_buffer,SoundParam *par);
	CPA_EXPORT void		SND_fn_vDeleteBufferSxdWave(long id_buffer);
	CPA_EXPORT void		SND_fn_vPauseBufferSxdWave(long id_buffer);
	CPA_EXPORT void		SND_fn_vResumeBufferSxdWave(long id_buffer);
	CPA_EXPORT void*	SND_fn_pvGetPtrBufferSxdWave(long id_buffer);
	CPA_EXPORT SndReal	SND_fn_rGetPosBufferSxdWave(long id_buffer);

	CPA_EXPORT long		SND_fn_lCreateNewBufferExSxdWave(SND_tdstFormat* pformat,SND_tdstCallback* pCallback,SoundParam* par,long user_id);
	CPA_EXPORT long		SND_fn_lGetPosBufferExSxdWave(long id_buffer);
	CPA_EXPORT long		SND_fn_lPushBufferExSxdWave(long id_buffer,SND_tdstStackBuffer* pStack);

	CPA_EXPORT SndBool	SND_fn_bLoadResScriptSxdWave(tdstBlockResourceDisk *disk,tdstBlockResourceMem *mem);
	CPA_EXPORT SndBool	SND_fn_bLoadResBinarySxdWave(tdstBlockResourceDisk *disk,tdstBlockResourceMem *mem,char *pDataBloc);
	CPA_EXPORT 	void	SND_fn_vConvertResDiskToMemSxdWave(tdstBlockResourceDisk *disk,tdstBlockResourceMem *mem,void* ptrBegin);

	CPA_EXPORT void		SND_fn_vUnLoadResSxdWave(tdstBlockResourceMem* mem);
	CPA_EXPORT SndBool	SND_fn_bIsResLoadedSxdWave(tdstBlockResourceMem *mem);

	CPA_EXPORT long		SND_fn_lPlayTransitionSxdWave(tduRefRes FirstRes,tduRefRes NextRes,SND_td_pfn_vSoundCallback transition_callback,long param_callback,SoundParam* par);
	CPA_EXPORT SndBool	SND_fn_bSetParamTransitionSxdWave(long voice,SoundParam *par);
	CPA_EXPORT SndBool	SND_fn_bSetNextTransitionSxdWave(long voice,tduRefRes new_res);
	CPA_EXPORT SndBool	SND_fn_bDoTransitionWithFadeSxdWave(long voice,tduRefRes new_res);
	CPA_EXPORT SndBool	SND_fn_bDoTransitionWithFadeSxdWave2(long voice,tduRefRes new_res,SndReal rFadeDuration);
	CPA_EXPORT void		SND_fn_vStopTransitionSxdWave(long voice);
	CPA_EXPORT void		SND_fn_vPauseTransitionSxdWave(long voice);
	CPA_EXPORT void		SND_fn_vResumeTransitionSxdWave(long voice);

	CPA_EXPORT void		SND_fn_vGetTargetLabelSxdWave(char* name,int nb_char);
	CPA_EXPORT void		SND_fn_vSetupTargetSxdWave();
	CPA_EXPORT SndBool	SND_fn_bCanSetupTargetSxdWave();

	CPA_EXPORT void		SND_fn_vReleaseDriverSxdWave(void);
	CPA_EXPORT void		SND_fn_vRestoreDriverSxdWave(void);
	CPA_EXPORT void		SND_fn_vForceReleaseDriverSxdWave(void);
	CPA_EXPORT void		SND_fn_vForceRestoreDriverSxdWave(void);

	CPA_EXPORT void		SND_fn_vSynchroSxdWave();

	CPA_EXPORT void		SND_fn_vSetDefaultRollOffSxdWave(RollOffParam* rolloff);

	CPA_EXPORT SndBool  SND_fn_bCheckVersionResourceSxdWave(tdstBlockResourceDisk* disk);

	CPA_EXPORT SndBool	SND_fn_b3DOneShootShouldBeConvertedSxdWave();
	CPA_EXPORT long		SND_fn_lGetNbVoiceOptimumSxdWave();
	CPA_EXPORT long		SND_fn_lGetNbVoiceWishedSxdWave();
	CPA_EXPORT void		SND_fn_vSetNbVoiceWishedSxdWave(long nb);
	/* 0x1000D360 */
	CPA_EXPORT SndBool	SND_fn_bSetResourceStaticVolumeSxdWave(tduRefRes res,unsigned char ucVolume);
#ifdef __cplusplus
}
#endif

#endif
