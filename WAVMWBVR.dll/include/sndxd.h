#ifndef _SNDXD_H_
#define _SNDXD_H_

#ifdef __cplusplus
extern "C" {
#endif

#undef DLL_IMPEXP
#undef CPA_EXPORT
#ifdef _DLL_COMPILATION_MODE
	#define DLL_IMPEXP __declspec(dllexport)
	#define CPA_EXPORT
#else
	#define DLL_IMPEXP

	#if defined(CPA_WANTS_IMPORT)
	#define CPA_EXPORT __declspec(dllimport)
	#elif defined(CPA_WANTS_EXPORT)
	#define CPA_EXPORT __declspec(dllexport)
	#else
	#define CPA_EXPORT
	#endif

#endif

#if defined(_DLL_COMPILATION_MODE) && !defined(DLLWAV_MODULE)
    #pragma message("-> sndxd : functions pointers defined in SNDDLL.H")
#else
	DLL_IMPEXP CPA_EXPORT long		SND_CALL SND_fn_lPlaySxd(tduRefRes res,SoundParam *par,long prio,SND_td_pfn_vSoundCallback fn_callback,long par_callback);
	DLL_IMPEXP CPA_EXPORT void		SND_CALL SND_fn_vRemoveCallbackSxd(long voice);
	DLL_IMPEXP CPA_EXPORT SndBool	SND_CALL SND_fn_bSetParamSxd(long voice,SoundParam *par);
	DLL_IMPEXP CPA_EXPORT SndBool	SND_CALL SND_fn_bTestIsPlayingSxd(long voice);
	DLL_IMPEXP CPA_EXPORT void		SND_CALL SND_fn_vStopSxd(long voice);
	DLL_IMPEXP CPA_EXPORT void		SND_CALL SND_fn_vPauseSxd(long voice);
	DLL_IMPEXP CPA_EXPORT void		SND_CALL SND_fn_vResumeSxd(long voice);

	DLL_IMPEXP CPA_EXPORT SndBool   SND_CALL SND_fn_bIsResourceLoopingSxd(tduRefRes res);
	DLL_IMPEXP CPA_EXPORT SndReal	SND_CALL SND_fn_rGetPosSxd(long voice);
	DLL_IMPEXP CPA_EXPORT SndReal	SND_CALL SND_fn_rGetLengthSxd(long voice);
	DLL_IMPEXP CPA_EXPORT void		SND_CALL SND_fn_vSetSoundVolumeSxd(unsigned char vol);
	DLL_IMPEXP CPA_EXPORT unsigned char SND_CALL SND_fn_ucGetSoundVolumeSxd();

	DLL_IMPEXP CPA_EXPORT void		SND_CALL SND_fn_vSetStereoSxd(SndBool active);
	DLL_IMPEXP CPA_EXPORT SndBool	SND_CALL SND_fn_bGetStereoSxd(void);
	DLL_IMPEXP CPA_EXPORT void		SND_CALL SND_fn_vSetReverseStereoSxd(SndBool active);
	DLL_IMPEXP CPA_EXPORT SndBool	SND_CALL SND_fn_bGetReverseStereoSxd(void);

	DLL_IMPEXP CPA_EXPORT int		SND_CALL SND_fn_iInitSxd(SND_tdstInitStruct *pInitStruct);
	DLL_IMPEXP CPA_EXPORT SndBool	SND_CALL SND_fn_bTestInitSxd(void);
	DLL_IMPEXP CPA_EXPORT void		SND_CALL SND_fn_vDesInitSxd(void);

	DLL_IMPEXP CPA_EXPORT void		SND_CALL SND_fn_vSetDopplerFactorSxd(SndReal factor);

	DLL_IMPEXP CPA_EXPORT void		SND_CALL SND_fn_vSetEffectSxd(long num);

	DLL_IMPEXP CPA_EXPORT long		SND_CALL SND_fn_lCreateMicroSxd(MicroParam* par);
	DLL_IMPEXP CPA_EXPORT void		SND_CALL SND_fn_vSetMicroParamSxd(long sxdId,MicroParam* par);
	DLL_IMPEXP CPA_EXPORT void		SND_CALL SND_fn_vDestroyMicroSxd(long sxdId);

	DLL_IMPEXP CPA_EXPORT long		SND_CALL SND_fn_lCreateNewBufferSxd(unsigned long nb_samples,unsigned short uwResolution,unsigned short uwNbChannels,unsigned long ulFreq,SoundParam* par,td_pfn_vRefreshBufferClient callback,long user_id);
	DLL_IMPEXP CPA_EXPORT void		SND_CALL SND_fn_vSetParamBufferSxd(long id_buffer,SoundParam *par);
	DLL_IMPEXP CPA_EXPORT void		SND_CALL SND_fn_vDeleteBufferSxd(long id_buffer);
	DLL_IMPEXP CPA_EXPORT void		SND_CALL SND_fn_vPauseBufferSxd(long id_buffer);
	DLL_IMPEXP CPA_EXPORT void		SND_CALL SND_fn_vResumeBufferSxd(long id_buffer);
	DLL_IMPEXP CPA_EXPORT void*		SND_CALL SND_fn_pvGetPtrBufferSxd(long id_buffer);
	DLL_IMPEXP CPA_EXPORT SndReal	SND_CALL SND_fn_rGetPosBufferSxd(long id_buffer);
	DLL_IMPEXP CPA_EXPORT long		SND_CALL SND_fn_lCreateNewBufferExSxd(SND_tdstFormat* pformat,SND_tdstCallback* pCallback,SoundParam* par,long user_id);
	DLL_IMPEXP CPA_EXPORT long		SND_CALL SND_fn_lGetPosBufferExSxd(long id_buffer);
	DLL_IMPEXP CPA_EXPORT long		SND_CALL SND_fn_lPushBufferExSxd(long id_buffer,SND_tdstStackBuffer* pStack);

	DLL_IMPEXP CPA_EXPORT void SND_CALL SND_fn_vConvertResDiskToMemSxd(tdstBlockResourceDisk *disk
							 ,tdstBlockResourceMem *mem
							 ,void* ptrBegin);

	DLL_IMPEXP CPA_EXPORT SndBool SND_CALL SND_fn_bLoadResScriptSxd(tdstBlockResourceDisk *disk,tdstBlockResourceMem *mem);
	DLL_IMPEXP CPA_EXPORT SndBool SND_CALL SND_fn_bLoadResBinarySxd(tdstBlockResourceDisk *disk,tdstBlockResourceMem *mem,char *pDataBloc);

	DLL_IMPEXP CPA_EXPORT void SND_CALL SND_fn_vUnLoadResSxd(tdstBlockResourceMem* mem);
	DLL_IMPEXP CPA_EXPORT SndBool SND_CALL SND_fn_bIsResLoadedSxd(tdstBlockResourceMem *mem);

	DLL_IMPEXP CPA_EXPORT long SND_CALL SND_fn_lPlayTransitionExSxd(tduRefRes FirstRes,tduRefRes NextRes,SND_td_pfn_vSoundCallback transition_callback,long param_callback,SoundParam* par);
	DLL_IMPEXP CPA_EXPORT SndBool SND_CALL SND_fn_bSetParamTransitionSxd(long voice,SoundParam *par);
	DLL_IMPEXP CPA_EXPORT SndBool SND_CALL SND_fn_bSetNextTransitionSxd(long voice,tduRefRes new_res);
	DLL_IMPEXP CPA_EXPORT SndBool SND_CALL SND_fn_bDoTransitionWithFadeSxd(long voice,tduRefRes new_res);
	DLL_IMPEXP CPA_EXPORT SndBool SND_CALL SND_fn_bDoTransitionWithFadeSxd2( long voice, tduRefRes new_res, SndReal rDuration );
	DLL_IMPEXP CPA_EXPORT void SND_CALL SND_fn_vStopTransitionSxd(long voice);
	DLL_IMPEXP CPA_EXPORT void SND_CALL SND_fn_vPauseTransitionSxd(long voice);
	DLL_IMPEXP CPA_EXPORT void SND_CALL SND_fn_vResumeTransitionSxd(long voice);

#ifndef NO_ACP_LDBIN
	DLL_IMPEXP CPA_EXPORT SndBool SND_CALL SND_fn_bCanFreeDataSample(void);
#endif

	DLL_IMPEXP CPA_EXPORT void SND_CALL SND_fn_vReleaseDriverSxd(void);
	DLL_IMPEXP CPA_EXPORT void SND_CALL SND_fn_vRestoreDriverSxd(void);
	DLL_IMPEXP CPA_EXPORT void SND_CALL SND_fn_vForceReleaseDriverSxd(void);
	DLL_IMPEXP CPA_EXPORT void SND_CALL SND_fn_vForceRestoreDriverSxd(void);

	DLL_IMPEXP CPA_EXPORT void SND_CALL SND_fn_vGetTargetLabelSxd(char* name,int nb_char);
	DLL_IMPEXP CPA_EXPORT void SND_CALL SND_fn_vSetupTargetSxd();
	DLL_IMPEXP CPA_EXPORT SndBool SND_CALL SND_fn_bCanSetupTargetSxd();

	DLL_IMPEXP CPA_EXPORT void SND_CALL SND_fn_vSynchroSxd();

	DLL_IMPEXP CPA_EXPORT void		SND_CALL SND_fn_vParam3Dto2D(SoundParam *par3D,SampleParam *par2D);
	DLL_IMPEXP CPA_EXPORT SndReal	SND_CALL SND_fn_rGetNormeSxd(SoundParam *par3D);

	DLL_IMPEXP CPA_EXPORT void		SND_CALL SND_fn_vSetDefaultRollOffSxd(RollOffParam* rolloff);

	DLL_IMPEXP CPA_EXPORT SndBool	SND_CALL SND_fn_bCheckVersionResourceSxd(tdstBlockResourceDisk* disk);

	DLL_IMPEXP CPA_EXPORT int		SND_CALL SND_fn_iInitBnkSxd(long num);
	DLL_IMPEXP CPA_EXPORT SndBool	SND_CALL SND_fn_bTestInitBnkSxd(long num);
	DLL_IMPEXP CPA_EXPORT void		SND_CALL SND_fn_vDesInitBnkSxd(long num);
	DLL_IMPEXP CPA_EXPORT void		SND_CALL SND_fn_vDesInitAllBnkSxd(void);
	DLL_IMPEXP CPA_EXPORT long		SND_CALL SND_fn_lPlayTransitionSxd(tduRefRes FirstRes,tduRefRes NextRes,SND_td_pfn_vSoundCallback transition_callback,SoundParam* par);

	DLL_IMPEXP CPA_EXPORT long		SND_CALL SND_fn_lGetNbVoiceOptimumSxd();
	DLL_IMPEXP CPA_EXPORT long		SND_CALL SND_fn_lGetNbVoiceWishedSxd();
	DLL_IMPEXP CPA_EXPORT void		SND_CALL SND_fn_vSetNbVoiceWishedSxd(tdeTypeZip ezip,long nb);
	DLL_IMPEXP CPA_EXPORT tdeTypeZip	SND_CALL SND_fn_eGetMainFormatSxd();
	/* 0x1000CE60 */
	DLL_IMPEXP CPA_EXPORT SndBool	SND_CALL SND_fn_bSetResourceStaticVolumeSxd(tduRefRes res,unsigned char ucVolume);
#endif

#ifdef __cplusplus
}
#endif

#endif
