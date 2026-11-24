











#include "SNDinc.h"

#ifndef NO_ACP_SOUND

#if defined(_DLL_COMPILATION_MODE)
#include "snddll.h"
#endif

#include "sndres.h"
#define DLLWAV_MODULE
#include "sndxd.h"
#include "sndwave.h"
#include "snd_pc.h"


long SND_CALL SND_fn_lPlaySxdWave(tduRefRes res,SoundParam *par,long prio,SND_td_pfn_vSoundCallback fn_callback,long par_callback)
{
	SampleParam	par2D;

	SND_fn_vParam3Dto2D(par,&par2D);

	return SND_fn_lPlaySample(res,&par2D,prio,fn_callback,par_callback);
}

void SND_CALL SND_fn_vRemoveCallbackSxdWave(long voice)
{
}

SndBool SND_CALL SND_fn_bSetParamSxdWave(long voice,SoundParam *par)
{
	SampleParam par2D;

	SND_fn_vParam3Dto2D(par,&par2D);

	return SND_fn_bSetParamSample(voice,&par2D);
}

SndBool SND_CALL SND_fn_bTestIsPlayingSxdWave(long voice)
{
	return SND_fn_bTestIsPlayingSample(voice);
}

void SND_CALL SND_fn_vStopSxdWave(long voice)
{
	SND_fn_vStopSample(voice);
}

void SND_CALL SND_fn_vPauseSxdWave(long voice)
{
	SND_fn_vPauseSample(voice);
}

void SND_CALL SND_fn_vResumeSxdWave(long voice)
{
	SND_fn_vResumeSample(voice);
}


int SND_fn_iInitSxdWave(SND_tdstInitStruct *pInitStruct)
{
	return SND_fn_iInitSample(pInitStruct);
}

/* 0x1000CFA0 */
SndBool SND_fn_bTestInitSxdWave(void)
{
	return SND_fn_bTestInitSample();
}

void SND_fn_vDesInitSxdWave(void)
{
	SND_fn_vDesInitSample();
}









void SND_fn_vSetDopplerFactorSxdWave(SndReal factor)
{
}


void SND_fn_vCreateMicroSxdWave(MicroParam* par,long idMicro)
{

}

void SND_fn_vSetMicroParamSxdWave(long sxdId,MicroParam* par)
{

}

void SND_fn_vDestroyMicroSxdWave(long sxdId)
{

}

/* 0x1000D000 */
SndBool SND_fn_bLoadResScriptSxdWave(tdstBlockResourceDisk *disk,tdstBlockResourceMem *mem)
{
	return SND_fn_bLoadResScriptSample(disk,mem);
}

SndBool SND_fn_bLoadResBinarySxdWave(tdstBlockResourceDisk* disk,tdstBlockResourceMem* mem,char *pDataBloc)
{
	    return(SND_fn_bLoadResBinarySample(disk,mem,pDataBloc));
}

/* 0x1000D040 */
void SND_fn_vConvertResDiskToMemSxdWave(tdstBlockResourceDisk *disk,tdstBlockResourceMem *mem,void* ptrBegin)
{
	SND_fn_vConvertResDiskToMemSample(disk,mem,ptrBegin);
}






void SND_fn_vUnLoadResSxdWave(tdstBlockResourceMem* mem)
{
    SND_fn_vUnLoadResSample(mem);
}

/* 0x1000D070 */
SndBool SND_fn_bIsResLoadedSxdWave(tdstBlockResourceMem *mem)
{
	return SND_fn_bIsResLoadedSample(mem);
}

/* 0x1000D080 */
long SND_fn_lCreateNewBufferSxdWave(unsigned long nb_samples,unsigned short uwResolution,unsigned short uwNbChannels,unsigned long ulFreq,SoundParam* par,td_pfn_vRefreshBufferClient callback,long user_id)
{
	SampleParam	par2D;
	SampleParam	*pPar2D;

	if (par!=NULL)
	{
		SND_fn_vParam3Dto2D(par,&par2D);
		pPar2D=&par2D;
	}
	else
		pPar2D=&C_SAMPLEPARAM_NEUTRE;

	return SND_fn_lCreateNewBufferSample(nb_samples,uwResolution,uwNbChannels,ulFreq,pPar2D,callback,user_id);
}

void SND_fn_vSetParamBufferSxdWave(long id_buffer,SoundParam *par)
{
	SampleParam	par2D;
	SampleParam	*pPar2D;

	if (par!=NULL)
	{
		SND_fn_vParam3Dto2D(par,&par2D);
		pPar2D=&par2D;
	}
	else
		pPar2D=NULL;

	SND_fn_vSetParamBufferSample(id_buffer,pPar2D);
}

void SND_fn_vDeleteBufferSxdWave(long id_buffer)
{

	SND_fn_vEnterCriticalSectionThreadSnd_();
#line 221 "..\\..\\..\\..\\..\\src\\sndxdwav.c"
	SND_fn_vMouchardThreadsnd(__FILE__,__LINE__);
	SND_fn_vDeleteBufferSample(id_buffer);
	SND_fn_vQuitCriticalSectionThreadSnd();
}

void SND_fn_vPauseBufferSxdWave(long id_buffer)
{
	SND_fn_vPauseBufferSample(id_buffer);
}

void SND_fn_vResumeBufferSxdWave(long id_buffer)
{
	SND_fn_vResumeBufferSample(id_buffer);
}

void* SND_fn_pvGetPtrBufferSxdWave(long id_buffer)
{
	return SND_fn_pvGetPtrBufferSample(id_buffer);
}

SndReal SND_fn_rGetPosBufferSxdWave(long id_buffer)
{
	return SND_fn_rGetPosBufferSample(id_buffer);
}

long SND_fn_lCreateNewBufferExSxdWave(SND_tdstFormat* pformat,SND_tdstCallback* pCallback,SoundParam* par,long user_id)
{
	SampleParam	par2D;
	SampleParam	*pPar2D;

	if (par!=NULL)
	{
		SND_fn_vParam3Dto2D(par,&par2D);
		pPar2D=&par2D;
	}
	else
		pPar2D=&C_SAMPLEPARAM_NEUTRE;

	return SND_fn_lCreateNewBufferExSample(pformat,pCallback,pPar2D,user_id);
}

long SND_fn_lGetPosBufferExSxdWave(long id_buffer)
{
	return SND_fn_lGetPosBufferExSample(id_buffer);
}

long SND_fn_lPushBufferExSxdWave(long id_buffer,SND_tdstStackBuffer* pStack)
{
	return SND_fn_lPushBufferExSample(id_buffer,pStack);
}

/* 0x1000D1F0 */
void SND_fn_vReleaseDriverSxdWave(void)
{
	SND_fn_vReleaseDriverSample();
}

void SND_fn_vRestoreDriverSxdWave(void)
{
	SND_fn_vRestoreDriverSample();
}

void SND_fn_vForceReleaseDriverSxdWave(void)
{
	SND_fn_vForceReleaseDriverSample();
}

void SND_fn_vForceRestoreDriverSxdWave(void)
{
	SND_fn_vForceRestoreDriverSample();
}








SndReal SND_CALL SND_fn_rGetPosSxdWave(long voie)
{
	return SND_fn_rGetPosSample(voie);
}







SndReal SND_CALL SND_fn_rGetLengthSxdWave(long voie)
{
	return SND_fn_rGetLengthSample(voie);
}






/* 0x1000D250 */
void SND_fn_vGetTargetLabelSxdWave(char* name,int nb_char)
{
	SND_fn_vGetTargetLabelSample(name,nb_char);
}

void SND_fn_vSetupTargetSxdWave()
{
	SND_fn_vSetupTargetSample();
}

SndBool SND_fn_bCanSetupTargetSxdWave()
{
	return SND_fn_bCanSetupTargetSample();
}

void SND_fn_vSetReverseStereoSxdWave(SndBool active)
{
	SND_fn_vSetReverseSample(active);
}

SndBool SND_fn_bGetReverseStereoSxdWave(void)
{
	return SND_fn_bGetReverseSample();
}

void SND_fn_vSetEffectSxdWave(long num)
{
	SND_fn_vSetEffectSample(num);
}


#ifdef SUPERVISATER
void SND_fn_vSetSoundVolumeSxdWave(unsigned char vol)
{
	SND_fn_vSetSoundVolumeSample(vol);
}
#endif



#ifdef SUPERVISATER
unsigned char SND_fn_ucGetSoundVolumeSxdWave(void)
{
	return SND_fn_ucGetSoundVolumeSample();
}
#endif





void SND_fn_vSynchroSxdWave()
{
	SND_fn_vSynchroSample();
}

void SND_fn_vSetDefaultRollOffSxdWave(RollOffParam* rolloff)
{

}

/* 0x1000D300 */
SndBool SND_fn_bCheckVersionResourceSxdWave(tdstBlockResourceDisk* disk)
{
	return TRUE;
}

void SND_fn_vSetNbVoiceWishedSxdWave(long nb)
{
	SND_fn_vSetNbVoiceWishedSample(nb);
}

long SND_fn_lGetNbVoiceOptimumSxdWave()
{
	return SND_fn_lGetNbVoiceOptimumSample();
}

long SND_fn_lGetNbVoiceWishedSxdWave()
{
	return SND_fn_lGetNbVoiceWishedSample();
}

SndBool SND_fn_bIsResourceLoopingSxdWave(tduRefRes res)
{
	return SND_fn_bIsResourceLoopingSample(res);
}

SndBool SND_fn_b3DOneShootShouldBeConvertedSxdWave()
{
	return FALSE;
}

SndBool SND_fn_bSetResourceStaticVolumeSxdWave(tduRefRes res,unsigned char ucVolume)
{
	return FUN_1000aef0(res,ucVolume);
}

#endif
