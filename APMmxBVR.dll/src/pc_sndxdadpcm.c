#include "SNDinc.h"

#include "sndres.h"
#include "sndadpcm_pc.h"

#define SND_M_ADPCM_EXPORT	__declspec(dllexport)

/* 0x100E85E0 */
static int SND_pc_s_bInitSxdADPCM = 0;

SND_M_ADPCM_EXPORT long SND_CALL SND_fn_lPlaySxdADPCM(tduRefRes res,SoundParam *par,long prio,SND_td_pfn_vSoundCallback fn_callback,long par_callback)
{
	SampleParam	par2D;

	if (!SND_pc_s_bInitSxdADPCM)
		return -1;
	SND_fn_vParam3Dto2D(par,&par2D);
	return SND_pc_fn_lPlayADPCM(res,&par2D,prio,fn_callback,par_callback);
}

SND_M_ADPCM_EXPORT void SND_CALL SND_fn_vRemoveCallbackSxdADPCM(long voice)
{
	SND_pc_fn_vRemoveCallbackADPCM(voice);
}

SND_M_ADPCM_EXPORT SndBool SND_CALL SND_fn_bSetParamSxdADPCM(long voice,SoundParam *par)
{
	SampleParam	par2D;

	SND_fn_vParam3Dto2D(par,&par2D);
	return SND_pc_fn_bSetParamADPCM(voice,&par2D);
}

SND_M_ADPCM_EXPORT SndBool SND_CALL SND_fn_bTestIsPlayingSxdADPCM(long voice)
{
	return SND_pc_fn_bTestIsPlayingADPCM(voice);
}

SND_M_ADPCM_EXPORT void SND_CALL SND_fn_vStopSxdADPCM(long voice)
{
	SND_pc_fn_vStopADPCM(voice);
}

SND_M_ADPCM_EXPORT void SND_CALL SND_fn_vPauseSxdADPCM(long voice)
{
	SND_pc_fn_vPauseADPCM(voice);
}

SND_M_ADPCM_EXPORT void SND_CALL SND_fn_vResumeSxdADPCM(long voice)
{
	SND_pc_fn_vResumeADPCM(voice);
}

SND_M_ADPCM_EXPORT SndBool SND_CALL SND_fn_bIsResourceLoopingSxdADPCM(tduRefRes res)
{
	return SND_pc_fn_bIsResourceLoopingADPCM(res);
}

SND_M_ADPCM_EXPORT SndReal SND_CALL SND_fn_rGetPosSxdADPCM(long voice)
{
	return SND_pc_fn_rGetPosADPCM(voice);
}

SND_M_ADPCM_EXPORT SndReal SND_CALL SND_fn_rGetLengthSxdADPCM(long voice)
{
	return SND_pc_fn_rGetLengthADPCM(voice);
}

SND_M_ADPCM_EXPORT void SND_CALL SND_fn_vSetSoundVolumeSxdADPCM(unsigned char vol)
{
	SND_pc_fn_vSetSoundVolumeADPCM(vol);
}

SND_M_ADPCM_EXPORT unsigned char SND_CALL SND_fn_ucGetSoundVolumeSxdADPCM(void)
{
	return SND_pc_fn_ucGetSoundVolumeADPCM();
}

SND_M_ADPCM_EXPORT void SND_CALL SND_fn_vSetStereoSxdADPCM(SndBool active)
{
	SND_pc_fn_vSetStereoADPCM(active);
}

SND_M_ADPCM_EXPORT SndBool SND_CALL SND_fn_bGetStereoSxdADPCM(void)
{
	return SND_pc_fn_bGetStereoADPCM();
}

SND_M_ADPCM_EXPORT void SND_CALL SND_fn_vSetReverseStereoSxdADPCM(SndBool active)
{
	SND_pc_fn_vSetReverseStereoADPCM(active);
}

SND_M_ADPCM_EXPORT SndBool SND_CALL SND_fn_bGetReverseStereoSxdADPCM(void)
{
	return SND_pc_fn_bGetReverseStereoADPCM();
}

SND_M_ADPCM_EXPORT int SND_CALL SND_fn_iInitSxdADPCM(SND_tdstInitStruct *pInitStruct)
{
	SND_pc_s_bInitSxdADPCM = 1;
	SND_DllInitEntryPoints_StaticFunctions(pInitStruct->hProcessInstance);
	SND_pc_DllInitEntryPoints_SxdFunctions(SND_pc_hWavModule);
	return SND_pc_fn_iInitADPCM(pInitStruct);
}

SND_M_ADPCM_EXPORT SndBool SND_CALL SND_fn_bTestInitSxdADPCM(void)
{
	return SND_pc_fn_bTestInitADPCM();
}

SND_M_ADPCM_EXPORT void SND_CALL SND_fn_vDesInitSxdADPCM(void)
{
	SND_pc_fn_vDesInitADPCM();
}

SND_M_ADPCM_EXPORT void SND_CALL SND_fn_vSetEffectSxdADPCM(long num)
{
	SND_pc_fn_vSetEffectADPCM(num);
}

SND_M_ADPCM_EXPORT void SND_CALL SND_fn_vCreateMicroSxdADPCM(long lParam1,long lParam2)
{
}

SND_M_ADPCM_EXPORT void SND_CALL SND_fn_vSetMicroParamSxdADPCM(long sxdId,MicroParam* par)
{
}

SND_M_ADPCM_EXPORT void SND_CALL SND_fn_vDestroyMicroSxdADPCM(long sxdId)
{
}

SND_M_ADPCM_EXPORT long SND_CALL SND_fn_lCreateNewBufferSxdADPCM(unsigned long nb_samples,unsigned short uwResolution,unsigned short uwNbChannels,unsigned long ulFreq,SoundParam* par,td_pfn_vRefreshBufferClient callback,long user_id)
{
	SampleParam	par2D;

	if (!SND_pc_s_bInitSxdADPCM)
		return -1;
	SND_fn_vParam3Dto2D(par,&par2D);
	return SND_pc_fn_lCreateNewBufferADPCM(nb_samples,uwResolution,uwNbChannels,ulFreq,&par2D,callback,user_id);
}

SND_M_ADPCM_EXPORT void SND_CALL SND_fn_vSetParamBufferSxdADPCM(long id_buffer,SoundParam *par)
{
	SampleParam	par2D;

	SND_fn_vParam3Dto2D(par,&par2D);
	SND_pc_fn_vSetParamBufferADPCM(id_buffer,&par2D);
}

SND_M_ADPCM_EXPORT void SND_CALL SND_fn_vDeleteBufferSxdADPCM(long id_buffer)
{
	SND_pc_fn_vDeleteBufferADPCM(id_buffer);
}

SND_M_ADPCM_EXPORT void SND_CALL SND_fn_vPauseBufferSxdADPCM(long id_buffer)
{
	SND_pc_fn_vPauseBufferADPCM(id_buffer);
}

SND_M_ADPCM_EXPORT void SND_CALL SND_fn_vResumeBufferSxdADPCM(long id_buffer)
{
	SND_pc_fn_vResumeBufferADPCM(id_buffer);
}

SND_M_ADPCM_EXPORT void* SND_CALL SND_fn_pvGetPtrBufferSxdADPCM(long id_buffer)
{
	return SND_pc_fn_pvGetPtrBufferADPCM(id_buffer);
}

SND_M_ADPCM_EXPORT SndReal SND_CALL SND_fn_rGetPosBufferSxdADPCM(long id_buffer)
{
	return SND_pc_fn_rGetPosBufferADPCM(id_buffer);
}

SND_M_ADPCM_EXPORT SndBool SND_CALL SND_fn_bLoadResScriptSxdADPCM(tdstBlockResourceDisk *disk,tdstBlockResourceMem *mem)
{
	return SND_pc_fn_bLoadResScriptADPCM(disk,mem);
}

SND_M_ADPCM_EXPORT SndBool SND_CALL SND_fn_bLoadResBinarySxdADPCM(tdstBlockResourceDisk *disk,tdstBlockResourceMem *mem,char *pDataBloc)
{
	return SND_pc_fn_bLoadResBinaryADPCM(disk,mem,pDataBloc);
}

SND_M_ADPCM_EXPORT SndBool SND_CALL SND_fn_bLoadResScriptAndConvertToWAVSxdADPCM(tdstBlockResourceDisk *disk,tdstBlockResourceMem *mem)
{
	return SND_pc_fn_bLoadResScriptAndConvertToWAVADPCM(disk,mem);
}

SND_M_ADPCM_EXPORT SndBool SND_CALL SND_fn_bLoadResBinaryAndConvertToWAVSxdADPCM(tdstBlockResourceDisk *disk,tdstBlockResourceMem *mem,char *pDataBloc)
{
	return SND_pc_fn_bLoadResBinaryAndConvertToWAVADPCM(disk,mem,pDataBloc);
}

SND_M_ADPCM_EXPORT void SND_CALL SND_fn_vUnLoadResSxdADPCM(tdstBlockResourceMem* mem)
{
	SND_pc_fn_vUnLoadResADPCM(mem);
}

SND_M_ADPCM_EXPORT SndBool SND_CALL SND_fn_bIsResLoadedSxdADPCM(tdstBlockResourceMem *mem)
{
	return SND_pc_fn_bIsResLoadedADPCM(mem);
}

SND_M_ADPCM_EXPORT long SND_CALL SND_fn_lPlayTransitionSxdADPCM(tduRefRes FirstRes,tduRefRes NextRes,SND_td_pfn_vSoundCallback transition_callback,long param_callback,SoundParam* par)
{
	SampleParam	par2D;

	if (!SND_pc_s_bInitSxdADPCM)
		return -1;
	SND_fn_vParam3Dto2D(par,&par2D);
	return SND_pc_fn_lPlayTransitionADPCM(FirstRes,NextRes,transition_callback,param_callback,&par2D);
}

SND_M_ADPCM_EXPORT SndBool SND_CALL SND_fn_bSetParamTransitionSxdADPCM(long voice,SoundParam *par)
{
	SampleParam	par2D;

	SND_fn_vParam3Dto2D(par,&par2D);
	return SND_pc_fn_bSetParamTransitionADPCM(voice,&par2D);
}

SND_M_ADPCM_EXPORT SndBool SND_CALL SND_fn_bSetNextTransitionSxdADPCM(long voice,tduRefRes new_res)
{
	return SND_pc_fn_bSetNextTransitionADPCM(voice,new_res);
}

SND_M_ADPCM_EXPORT SndBool SND_CALL SND_fn_bDoTransitionWithFadeSxdADPCM(long voice,tduRefRes new_res)
{
	return SND_pc_fn_bDoTransitionWithFadeADPCM(voice,new_res,C_SNDREAL_1);
}

SND_M_ADPCM_EXPORT SndBool SND_CALL SND_fn_bDoTransitionWithFadeSxdADPCM2(long voice,tduRefRes new_res,SndReal rDuration)
{
	return SND_pc_fn_bDoTransitionWithFadeADPCM(voice,new_res,rDuration);
}

SND_M_ADPCM_EXPORT void SND_CALL SND_fn_vStopTransitionSxdADPCM(long voice)
{
	SND_pc_fn_vStopTransitionADPCM(voice);
}

SND_M_ADPCM_EXPORT void SND_CALL SND_fn_vPauseTransitionSxdADPCM(long voice)
{
	SND_pc_fn_vPauseTransitionADPCM(voice);
}

SND_M_ADPCM_EXPORT void SND_CALL SND_fn_vResumeTransitionSxdADPCM(long voice)
{
	SND_pc_fn_vResumeTransitionADPCM(voice);
}

SND_M_ADPCM_EXPORT SndBool SND_CALL SND_fn_bCanFreeDataADPCM(void)
{
	return SND_pc_fn_bCanFreeDataADPCM();
}

SND_M_ADPCM_EXPORT void SND_CALL SND_fn_vReleaseDriverSxdADPCM(void)
{
	SND_pc_fn_vReleaseDriverADPCM();
}

SND_M_ADPCM_EXPORT void SND_CALL SND_fn_vRestoreDriverSxdADPCM(void)
{
	SND_pc_fn_vRestoreDriverADPCM();
}

SND_M_ADPCM_EXPORT void SND_CALL SND_fn_vGetTargetLabelSxdADPCM(char* name,int nb_char)
{
	SND_pc_fn_vGetTargetLabelADPCM(name,nb_char);
}

SND_M_ADPCM_EXPORT void SND_CALL SND_fn_vSetupTargetSxdADPCM(void)
{
	SND_pc_fn_vSetupTargetADPCM();
}

SND_M_ADPCM_EXPORT SndBool SND_CALL SND_fn_bCanSetupTargetSxdADPCM(void)
{
	return SND_pc_fn_bCanSetupTargetADPCM();
}

SND_M_ADPCM_EXPORT void SND_CALL SND_fn_vSynchroSxdADPCM(void)
{
	SND_pc_fn_vSynchroADPCM();
}

SND_M_ADPCM_EXPORT long SND_CALL SND_fn_lCreateNewBufferExSxdADPCM(SND_tdstFormat* pformat,SND_tdstCallback* pCallback,SoundParam* par,long user_id)
{
	if (!SND_pc_s_bInitSxdADPCM)
		return -1;
	return SND_pc_fn_lCreateNewBufferExADPCM(pformat,pCallback,par,user_id);
}

SND_M_ADPCM_EXPORT long SND_CALL SND_fn_lGetPosBufferExSxdADPCM(long id_buffer)
{
	return SND_pc_fn_lGetPosBufferExADPCM(id_buffer);
}

SND_M_ADPCM_EXPORT long SND_CALL SND_fn_lPushBufferExSxdADPCM(long id_buffer,SND_tdstStackBuffer* pStack)
{
	return SND_pc_fn_lPushBufferExADPCM(id_buffer,pStack);
}

SND_M_ADPCM_EXPORT void SND_CALL SND_fn_vSetDefaultRollOffSxdADPCM(RollOffParam* rolloff)
{
}

SND_M_ADPCM_EXPORT SndBool SND_CALL SND_fn_bCheckVersionResourceSxdADPCM(tdstBlockResourceDisk* disk)
{
	return SND_pc_fn_bCheckVersionResourceADPCM(disk);
}

SND_M_ADPCM_EXPORT long SND_CALL SND_fn_lGetNbVoiceWishedSxdADPCM(void)
{
	return SND_pc_fn_lGetNbVoiceWishedADPCM();
}

/* 0x10002BD0 */
SND_M_ADPCM_EXPORT SndBool SND_CALL SND_fn_bIsLibActiveSxdADPCM(void)
{
	return TRUE;
}

SND_M_ADPCM_EXPORT SndBool SND_CALL SND_fn_bSetResourceStaticVolumeSxdADPCM(tduRefRes res,unsigned char ucVolume)
{
	return SND_pc_fn_bSetResourceStaticVolumeADPCM(res,ucVolume);
}
