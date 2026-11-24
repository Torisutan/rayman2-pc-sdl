#include "SNDinc.h"

#define DLLWAV_MODULE
#include "sndxd.h"

#if defined(_DLL_COMPILATION_MODE)
#include "snddll.h"
#else
#include "sndvect.h"
#include "sndspace.h"
#ifdef USE_DBG_MODULE
#include "snddbg.h"
#endif
#endif

#include "sndres.h"
#include "sndxdwav.h"
#include "snd_pc.h"

typedef struct _micro_description *p_micro_description;

SNDLST2_M_DynamicUseListOf(p_micro_description);

typedef struct _micro_description{
	SndVector			pos;
	SndVector			vit;
	SndVector			dir_nor;
	SndVector			dir_tan;
	SndBool_field_decl(just_created);
	SndBool_field_decl(valid);
    SNDLST2_M_DynamicElementDeclaration(p_micro_description)
} micro_description;

typedef struct _micro_list{
    SNDLST2_M_DynamicAnchorDeclaration(p_micro_description) pList;
} micro_list;

static int g_iNbMicrosSxd;
static micro_list micros_sxd;

typedef struct _sample_actif *p_sample_actif;

SNDLST2_M_DynamicUseListOf(p_sample_actif);

typedef struct _sample_actif
{
    tduRefRes   uRes;
    long        lVoice;
    tdeTypeZip  eZip;
    SndBool     bStream;
    SNDLST2_M_DynamicElementDeclaration(p_sample_actif)
} sample_actif;

typedef struct _sample_list
{
    SNDLST2_M_DynamicAnchorDeclaration(p_sample_actif) pList;
} sample_list;

static sample_list samples_sxd;

/* 0x1000BEC0 */
static SND_tdhVirtualModule SND_pc_hVirtualModuleADPCM;

void SND_CALL SND_fn_vParam3Dto2D(SoundParam *par3D, SampleParam *par2D)
{
	SndReal			dist, doppler;
	unsigned char	vol, pan, space;
	int				volr, voll, volb, i, j;
	SndVector		vect;
    p_micro_description pMicro;

	if (par3D->iFlags & C_SOUNDPARAM_NO_POSITION)
	{
		if (par3D->iFlags & C_SOUNDPARAM_COEF_VOL)
			par2D->ucVol = par3D->ucVol;
		else
			par2D->ucVol = C_SOUNDVOL_MAXI;

		par2D->Freq = par3D->Freq;
		par2D->ucPan = par3D->ucPan;
		par2D->ucSpace = par3D->ucSpace;
	}
	else
	if (g_iNbMicrosSxd)
	{
		vol = 0;
		pan = C_SOUNDPAN_MEDIAN;
		space = C_SOUNDSPACE_FRONT;

        volr = 0;
        voll = 0;
        volb = 0;
        doppler = C_SNDREAL_1;

		SNDLST2_M_DynamicForEachElementOf(&micros_sxd.pList, pMicro, i)
        {
			if (!pMicro->just_created)
			{
				for (j = 0 ; j < par3D->iNbLinks ; j++)
				{
					if (par3D->astMicro[j].lId == (long)pMicro)
						break;
				}
				if (j < par3D->iNbLinks)
				{
					if (par3D->astMicro[j].bLinked)
					{
						vect.x = par3D->Pos.x - pMicro->pos.x;
						vect.y = par3D->Pos.y - pMicro->pos.y;
						vect.z = par3D->Pos.z - pMicro->pos.z;

						dist = SND_fn_rPseudoNormeVectorSnd(&vect);

						if (par3D->iFlags & C_SOUNDPARAM_ROLLOFF)
							vol = SND_fn_ucDistanceToVolumeEx(dist, &(SND_M_GetExtraParamFromSoundParam(par3D))->stRollOff);
						else
							vol=SND_fn_ucDistanceToVolume(dist);

						pan = SND_fn_ucPositionToPan(&(par3D->Pos),&(pMicro->pos),&(pMicro->dir_nor),&(pMicro->dir_tan));
						space = SND_fn_ucPositionToDolby(&(par3D->Pos),&(pMicro->pos),&(pMicro->dir_nor));
						doppler = SND_fn_rDopplerPitch(&(par3D->Pos),&(par3D->Vit),&(pMicro->pos),&(pMicro->vit));

						volr += M_VolFront(M_VolLeft(vol, pan), space);
						voll += M_VolFront(M_VolRight(vol, pan), space);
						volb += M_VolBack(vol, space);
					}
				}
			}
		}

		if (i==1)
		{
			par2D->ucVol = vol;
			par2D->ucPan = pan;
			par2D->ucSpace = space;
			par2D->Freq = doppler;
		}
		else
		{
			voll = voll / i; volr = volr / i; volb = volb / i;
			par2D->ucVol = M_GetVol(volr,voll,volb);
			par2D->ucPan = M_GetPan(volr,voll,volb);
			par2D->ucSpace = M_GetSpace(volr,voll,volb);

			par2D->Freq = C_SNDREAL_1;
		}

            par2D->Freq = SND_fn_rMulRealRealSnd(par2D->Freq, par3D->Freq);

        if (par3D->iFlags & C_SOUNDPARAM_COEF_VOL)
            par2D->ucVol = (unsigned char)(((long)par2D->ucVol * par3D->ucVol) >> 7);
	}
	else
	{
		memcpy(par2D, &C_SAMPLEPARAM_NEUTRE, sizeof(*par2D));
		par2D->Freq = par3D->Freq;
	}
	par2D->ucReverb = par3D->ucReverb;
}

/* 0x1000B4B0 */
SndReal SND_CALL SND_fn_rGetNormeSxd(SoundParam *par3D)
{
	unsigned char	vol, volmax, norme;
	int				j;
	SndVector		vect;
	SndReal			dist;
    p_micro_description pMicro;

	volmax = 0;
	norme = 0;
	if (g_iNbMicrosSxd && ((pMicro=SNDLST2_M_DynamicGetFirstElement(&micros_sxd.pList))!=NULL))
	{
		do
		{
			vol = 0;
			if (!pMicro->just_created)
			{
				for (j = 0 ; j < par3D->iNbLinks ; j++)
				{
					if (par3D->astMicro[j].lId == (long)pMicro)
						break;
				}
				if ((j < par3D->iNbLinks) && (par3D->astMicro[j].bLinked))
				{
					vect.x = par3D->Pos.x - pMicro->pos.x;
					vect.y = par3D->Pos.y - pMicro->pos.y;
					vect.z = par3D->Pos.z - pMicro->pos.z;

					dist = SND_fn_rPseudoNormeVectorSnd(&vect);

					if (par3D->iFlags & C_SOUNDPARAM_ROLLOFF)
						vol = SND_fn_ucDistanceToVolumeEx(dist, &(SND_M_GetExtraParamFromSoundParam(par3D))->stRollOff);
					else
						vol=SND_fn_ucDistanceToVolume(dist);
				}
			}
			if (volmax <= vol)
				volmax = vol;
		} while ((pMicro=SNDLST2_M_DynamicGetNextElement(pMicro))!=NULL);
		norme = volmax;
	}
	return M_IntToRealSnd(norme);
}

sample_actif *new_sample_sxd(long voice,tdeTypeZip zip,SndBool stream,tduRefRes res)
{
    p_sample_actif sample;
    int i;

    SNDLST2_M_DynamicForEachElementOf(&samples_sxd.pList,sample,i)
    {
        if ((sample->lVoice==voice)
			&& (sample->eZip==zip)
			&& ((sample->bStream==stream) || (sample->eZip!=SAMPLE_PCM)))
        {
            sample->uRes=res;
            return sample;
        }
    }

    sample=SND_fn_pvMallocSndEx(E_ucSndBlockMain,sizeof(sample_actif));
    SNDLST2_M_DynamicInitElement(sample);
    SNDLST2_M_DynamicAddTail(&samples_sxd.pList,sample);
    sample->uRes=res;
    sample->lVoice=voice;
    sample->eZip=zip;
    sample->bStream=stream;

    return sample;
}

void del_sample_actif(p_sample_actif sample)
{
    SNDLST2_M_DynamicIsolate(sample);
    SND_fn_vFreeSndEx(E_ucSndBlockMain,sample);
}

long SND_CALL SND_fn_lPlaySxd(tduRefRes res,SoundParam *par,long prio,SND_td_pfn_vSoundCallback fn_callback,long par_callback)
{
    long            ret;
    sample_actif    *sample;
	SampleParam		par2D;

	switch (res.pstPtr->uRes.stSample.eZip)
	{
	case SAMPLE_PCM:
		if (res.pstPtr->uRes.stSample.bStream)
			ret=FUN_10006310(res,par,prio,fn_callback,par_callback);
		else
			ret=SND_fn_lPlaySxdWave(res,par,prio,fn_callback,par_callback);
		break;
	case SAMPLE_MPEG:
		SND_fn_vParam3Dto2D(par,&par2D);
		ret=SND_fn_lPlayMPEG(res,&par2D,prio,fn_callback,par_callback);
		break;
	case SAMPLE_ADPCM:
		ret=FUN_10007930(SND_pc_hVirtualModuleADPCM,res,par,prio,fn_callback,par_callback);
		break;
	default:
		SND_M_DisplayError(E_uwSndInvalidStaticParamForResource,"");
		ret=C_PLAY_FAILED;
	}

    if (ret!=C_PLAY_FAILED)
    {
        sample=new_sample_sxd(ret,res.pstPtr->uRes.stSample.eZip,res.pstPtr->uRes.stSample.bStream,res);

        return (long)sample;
    }
    else
        return C_PLAY_FAILED;
}

void SND_CALL SND_fn_vRemoveCallbackSxd(long voice)
{
	switch (((sample_actif*)voice)->eZip)
	{
	case SAMPLE_PCM:
		if (((sample_actif*)voice)->bStream)
			FUN_10006500(((sample_actif*)voice)->lVoice);
		else
			SND_fn_vRemoveCallbackSxdWave(((sample_actif*)voice)->lVoice);
		break;
	case SAMPLE_MPEG:
		SND_fn_vRemoveCallbackMPEG(((sample_actif*)voice)->lVoice);
		break;
	case SAMPLE_ADPCM:
		FUN_10007ac0(SND_pc_hVirtualModuleADPCM,((sample_actif*)voice)->lVoice);
		break;
	default:
		SND_M_DisplayError(E_uwSndInvalidStaticParamForResource,"");
	}
}

SndBool SND_CALL SND_fn_bSetParamSxd(long voice,SoundParam *par)
{
    SndBool ret;
	SampleParam par2D;

	switch (((sample_actif*)voice)->eZip)
	{
	case SAMPLE_PCM:
		if (((sample_actif*)voice)->bStream)
			ret=FUN_10006510(((sample_actif*)voice)->lVoice,par);
		else
			ret=SND_fn_bSetParamSxdWave(((sample_actif*)voice)->lVoice,par);
		break;
	case SAMPLE_MPEG:
		SND_fn_vParam3Dto2D(par,&par2D);
		ret=SND_fn_bSetParamMPEG(((sample_actif*)voice)->lVoice,&par2D);
		break;
	case SAMPLE_ADPCM:
		ret=FUN_10007af0(SND_pc_hVirtualModuleADPCM,((sample_actif*)voice)->lVoice,par);
		break;
	default:
		SND_M_DisplayError(E_uwSndInvalidStaticParamForResource,"");
		ret=FALSE;
	}

    if (!ret)
        del_sample_actif((p_sample_actif)voice);

    return ret;
}

SndBool SND_CALL SND_fn_bTestIsPlayingSxd(long voice)
{
    SndBool ret;

	switch (((sample_actif*)voice)->eZip)
	{
	case SAMPLE_PCM:
		if (((sample_actif*)voice)->bStream)
			ret=FUN_100065b0(((sample_actif*)voice)->lVoice);
		else
			ret=SND_fn_bTestIsPlayingSxdWave(((sample_actif*)voice)->lVoice);
		break;
	case SAMPLE_MPEG:
		ret=SND_fn_bTestIsPlayingMPEG(((sample_actif*)voice)->lVoice);
		break;
	case SAMPLE_ADPCM:
		ret=FUN_10007b90(SND_pc_hVirtualModuleADPCM,((sample_actif*)voice)->lVoice);
		break;
	default:
		SND_M_DisplayError(E_uwSndInvalidStaticParamForResource,"");
		ret=FALSE;
	}

    if (!ret)
        del_sample_actif((p_sample_actif)voice);

    return ret;
}

void SND_CALL SND_fn_vStopSxd(long voice)
{
	switch (((sample_actif*)voice)->eZip)
	{
	case SAMPLE_PCM:
		if (((sample_actif*)voice)->bStream)
			FUN_100065e0(((sample_actif*)voice)->lVoice);
		else
			SND_fn_vStopSxdWave(((sample_actif*)voice)->lVoice);
		break;
	case SAMPLE_MPEG:
		SND_fn_vStopMPEG(((sample_actif*)voice)->lVoice);
		break;
	case SAMPLE_ADPCM:
		FUN_10007c10(SND_pc_hVirtualModuleADPCM,((sample_actif*)voice)->lVoice);
		break;
	default:
		SND_M_DisplayError(E_uwSndInvalidStaticParamForResource,"");
	}

    del_sample_actif((p_sample_actif)voice);
}

void SND_CALL SND_fn_vPauseSxd(long voice)
{
	switch (((sample_actif*)voice)->eZip)
	{
	case SAMPLE_PCM:
		if (((sample_actif*)voice)->bStream)
			FUN_10006620(((sample_actif*)voice)->lVoice);
		else
			SND_fn_vPauseSxdWave(((sample_actif*)voice)->lVoice);
		break;
	case SAMPLE_MPEG:
		SND_fn_vPauseMPEG(((sample_actif*)voice)->lVoice);
		break;
	case SAMPLE_ADPCM:
		FUN_10007c80(SND_pc_hVirtualModuleADPCM,((sample_actif*)voice)->lVoice);
		break;
	default:
		SND_M_DisplayError(E_uwSndInvalidStaticParamForResource,"");
	}
}

void SND_CALL SND_fn_vResumeSxd(long voice)
{
	switch (((sample_actif*)voice)->eZip)
	{
	case SAMPLE_PCM:
		if (((sample_actif*)voice)->bStream)
			FUN_10006630(((sample_actif*)voice)->lVoice);
		else
			SND_fn_vResumeSxdWave(((sample_actif*)voice)->lVoice);
		break;
	case SAMPLE_MPEG:
		SND_fn_vResumeMPEG(((sample_actif*)voice)->lVoice);
		break;
	case SAMPLE_ADPCM:
		FUN_10007ca0(SND_pc_hVirtualModuleADPCM,((sample_actif*)voice)->lVoice);
		break;
	default:
		SND_M_DisplayError(E_uwSndInvalidStaticParamForResource,"");
	}
}

/* 0x1000BAA0 */
void SND_CALL SND_fn_vConvertResDiskToMemSxd(tdstBlockResourceDisk *disk,tdstBlockResourceMem *mem,void* ptrBegin)
{
	switch (disk->uRes.stSample.eZip)
	{
	case SAMPLE_PCM:
		SND_fn_vConvertResDiskToMemSxdWave(disk,mem,ptrBegin);
		break;
	default:
		SND_M_DisplayError(E_uwSndInvalidStaticParamForResource,"");
	}
}

/* 0x1000BAD0 */
SndBool SND_CALL SND_fn_bLoadResScriptSxd(tdstBlockResourceDisk *disk,tdstBlockResourceMem *mem)
{
	mem->uRes.stSample.eZip=disk->uRes.stSample.eZip;
	switch (disk->uRes.stSample.eZip)
	{
	case SAMPLE_PCM:
	case SAMPLE_AIFF:
		if (disk->uRes.stSample.bStream)
			return FUN_100067e0(disk,mem);
		else
			return SND_fn_bLoadResScriptSxdWave(disk,mem);
	case SAMPLE_MPEG:
		return SND_fn_bLoadResScriptMPEG(disk,mem);
	case SAMPLE_ADPCM:
	case SAMPLE_ADPCM_TO_CONVERT:
		if (SND_fn_b3DOneShootShouldBeConvertedSxdWave() && (disk->uRes.stSample.uwNbChannels==1))
			return SND_fn_bLoadResScriptAndConvertToWAVSxdADPCM(disk,mem);
		else
			return SND_fn_bLoadResScriptSxdADPCM(disk,mem);
	default:
		SND_M_DisplayError(E_uwSndInvalidStaticParamForResource,"");
		return FALSE;
	}
}

SndBool SND_CALL SND_fn_bLoadResBinarySxd(tdstBlockResourceDisk* disk,tdstBlockResourceMem* mem,char* data)
{
	mem->uRes.stSample.eZip=disk->uRes.stSample.eZip;
	switch (disk->uRes.stSample.eZip)
	{
	case SAMPLE_PCM:
		if (disk->uRes.stSample.bStream)
			return FUN_10006810(disk,mem,data);
		else
			return(SND_fn_bLoadResBinarySxdWave(disk,mem,data));
	case SAMPLE_MPEG:
		return SND_fn_bLoadResBinaryMPEG(disk,mem,data);
	case SAMPLE_ADPCM:
	case SAMPLE_ADPCM_TO_CONVERT:
		if (SND_fn_b3DOneShootShouldBeConvertedSxdWave() && (disk->uRes.stSample.uwNbChannels==1))
			return SND_fn_bLoadResBinaryAndConvertToWAVSxdADPCM(disk,mem,data);
		else
			return SND_fn_bLoadResBinarySxdADPCM(disk,mem,data);
	default:
		SND_M_DisplayError(E_uwSndInvalidStaticParamForResource,"");
		return FALSE;
	}
}

void SND_CALL SND_fn_vUnLoadResSxd(tdstBlockResourceMem* mem)
{
	switch (mem->uRes.stSample.eZip)
	{
	case SAMPLE_PCM:
		if (mem->uRes.stSample.bStream)
			FUN_10006830(mem);
		else
			SND_fn_vUnLoadResSxdWave(mem);
		break;
	case SAMPLE_MPEG:
		SND_fn_vUnLoadResMPEG(mem);
		break;
	case SAMPLE_ADPCM:
		SND_fn_vUnLoadResSxdADPCM(mem);
		break;
	default:
		SND_M_DisplayError(E_uwSndInvalidStaticParamForResource,"");
	}
}

/* 0x1000BC90 */
SndBool SND_CALL SND_fn_bIsResLoadedSxd(tdstBlockResourceMem *mem)
{
	switch (mem->uRes.stSample.eZip)
	{
	case SAMPLE_PCM:
		return SND_fn_bIsResLoadedSxdWave(mem);
	case SAMPLE_MPEG:
		return SND_fn_bIsResLoadedMPEG(mem);
	case SAMPLE_ADPCM:
		return SND_fn_bIsResLoadedSxdADPCM(mem);
	default:
		SND_M_DisplayError(E_uwSndInvalidStaticParamForResource,"");
		return FALSE;
	}
}

/* 0x1000BCE0 */
SndBool SND_CALL SND_fn_bIsResourceLoopingSxd(tduRefRes res)
{
	switch (res.pstPtr->uRes.stSample.eZip)
	{
	case SAMPLE_PCM:
		return SND_fn_bIsResourceLoopingSxdWave(res);
	case SAMPLE_MPEG:
		return FALSE;
	case SAMPLE_ADPCM:
		return SND_fn_bIsResourceLoopingSxdADPCM(res);
	default:
		return FALSE;
	}
}

SndReal SND_CALL SND_fn_rGetPosSxd(long voie)
{
	switch (((sample_actif*)voie)->eZip)
	{
	case SAMPLE_PCM:
		if (((sample_actif*)voie)->bStream)
			return FUN_100066c0(((sample_actif*)voie)->lVoice);
		else
			return SND_fn_rGetPosSxdWave(((sample_actif*)voie)->lVoice);
	case SAMPLE_MPEG:
		return SND_fn_rGetPosMPEG(((sample_actif*)voie)->lVoice);
	case SAMPLE_ADPCM:
		return FUN_10007cd0(SND_pc_hVirtualModuleADPCM,((sample_actif*)voie)->lVoice);
	default:
		SND_M_DisplayError(E_uwSndInvalidStaticParamForResource,"");
		return -1;
	}
}

SndReal SND_CALL SND_fn_rGetLengthSxd(long voie)
{
	switch (((sample_actif*)voie)->eZip)
	{
	case SAMPLE_PCM:
		if (((sample_actif*)voie)->bStream)
			return FUN_100066f0(((sample_actif*)voie)->lVoice);
		else
			return SND_fn_rGetLengthSxdWave(((sample_actif*)voie)->lVoice);
	case SAMPLE_MPEG:
		return SND_fn_rGetLengthMPEG(((sample_actif*)voie)->lVoice);
	case SAMPLE_ADPCM:
		return FUN_10007cf0(SND_pc_hVirtualModuleADPCM,((sample_actif*)voie)->lVoice);
	default:
		SND_M_DisplayError(E_uwSndInvalidStaticParamForResource,"");
		return -1;
	}
}

/* 0x1000BE00 */
SndBool SND_CALL SND_fn_bCheckVersionResourceSxd(tdstBlockResourceDisk* disk)
{
	switch (disk->uRes.stSample.eZip)
	{
	case SAMPLE_PCM:
		return SND_fn_bCheckVersionResourceSxdWave(disk);
	case SAMPLE_MPEG:
		return SND_fn_bCheckVersionResourceMPEG(disk);
	case SAMPLE_ADPCM:
		return SND_fn_bCheckVersionResourceSxdADPCM(disk);
	default:
		SND_M_DisplayError(E_uwSndInvalidStaticParamForResource,"");
		return FALSE;
	}
}

#ifdef SUPERVISATER
void SND_CALL SND_fn_vSetSoundVolumeSxd(unsigned char vol)
{
	SND_fn_vSetSoundVolumeSxdWave(vol);
}
#endif

#ifdef SUPERVISATER
unsigned char SND_CALL SND_fn_ucGetSoundVolumeSxd(void)
{
	return SND_fn_ucGetSoundVolumeSxdWave();
}
#endif

/* 0x1000BE70 */
void SND_CALL SND_fn_vSetStereoSxd(SndBool active)
{
}

SndBool SND_CALL SND_fn_bGetStereoSxd(void)
{
	return FALSE;
}

void SND_CALL SND_fn_vSetReverseStereoSxd(SndBool active)
{
	SND_fn_vSetReverseStereoSxdWave(active);
	SND_fn_vSetReverseStereoSxdADPCM(active);
}

SndBool SND_CALL SND_fn_bGetReverseStereoSxd(void)
{
	return SND_fn_bGetReverseStereoSxdWave();
}

int SND_CALL SND_fn_iInitSxd(SND_tdstInitStruct *pInitStruct)
{
    int ret;
	SND_pc_tdstVirtualVoiceFunctions stFunctions;

#if defined(_DLL_COMPILATION_MODE)
    SND_DllInitEntryPoints_StaticFunctions((HMODULE)pInitStruct->hProcessInstance );
#ifndef DISABLE_MPEG
	SND_DllInitEntryPoints_MpegFunctions(SND_hMpegModule);
#endif
	/* 0x1000BEC0 */
	SND_pc_DllInitEntryPoints_AdpcmFunctions(SND_pc_hAdpcmModule);
#ifndef DISABLE_RECORD
	SND_DllInitEntryPoints_RecordFunctions(SND_hRecordModule);
#endif
	SND_DllInitEntryPoints_DbgFunctions(SND_hDbgModule);
#endif

	g_iNbMicrosSxd=0;
    SNDLST2_M_DynamicInitAnchor(&micros_sxd.pList);
    SNDLST2_M_DynamicInitAnchor(&samples_sxd.pList);

    if ((ret=SND_fn_iInitSxdWave(pInitStruct))==C_INIT_OK)
	{
		FUN_10006640(pInitStruct);
#ifndef DISABLE_MPEG
		SND_fn_iInitMPEG(pInitStruct);
#endif
		SND_fn_iInitSxdADPCM(pInitStruct);
		FUN_10007030(pInitStruct);
	}

	stFunctions.pfn_lPlay=SND_fn_lPlaySxdADPCM;
	stFunctions.pfn_vStop=SND_fn_vStopSxdADPCM;
	stFunctions.pfn_bSetParam=SND_fn_bSetParamSxdADPCM;
	stFunctions.pfn_vPause=SND_fn_vPauseSxdADPCM;
	stFunctions.pfn_vResume=SND_fn_vResumeSxdADPCM;
	stFunctions.pfn_rGetPos=SND_fn_rGetPosSxdADPCM;
	stFunctions.pfn_rGetLength=SND_fn_rGetLengthSxdADPCM;
	stFunctions.pfn_vRemoveCallback=SND_fn_vRemoveCallbackSxdADPCM;
	stFunctions.pfn_bTestIsPlaying=SND_fn_bTestIsPlayingSxdADPCM;
	SND_pc_hVirtualModuleADPCM=FUN_10007d10(SND_fn_lGetNbVoiceWishedSxdADPCM(),&stFunctions);
	dbgSND_fn_vInitListesSxdVirtual(SND_pc_hVirtualModuleADPCM,"Virtual voice for ADPCM");

    return ret;
}

/* 0x1000BFE0 */
SndBool SND_CALL SND_fn_bTestInitSxd(void)
{
	return SND_fn_bTestInitSxdWave();
}

void SND_CALL SND_fn_vDesInitSxd(void)
{
	FUN_10007dd0(SND_pc_hVirtualModuleADPCM);
	SND_fn_vDesInitSxdADPCM();
	SND_fn_vDesInitMPEG();
	FUN_10006680();
    SND_fn_vDesInitSxdWave();
}

/* 0x1000C020 */
void FUN_1000c020(void)
{
}

void SND_CALL SND_fn_vSetEffectSxd(long num)
{
	SND_fn_vSetEffectSxdWave(num);
}

void SND_CALL SND_fn_vSetDopplerFactorSxd(SndReal factor)
{
	SND_fn_vSetDopplerFactorSxdWave(factor);
}

long SND_CALL SND_fn_lCreateMicroSxd(MicroParam* par)
{
    p_micro_description pMicro;

    pMicro=SND_fn_pvMallocSndEx(E_ucSndBlockMain,sizeof(micro_description));
    SNDLST2_M_DynamicInitElement(pMicro);

    SNDLST2_M_DynamicAddTail(&micros_sxd.pList,pMicro);

	if (par==NULL)
	{
		pMicro->just_created=TRUE;
	}
	else
	{
		memcpy(&pMicro->pos,&par->pos,sizeof(SndVector));
		memcpy(&pMicro->vit,&par->vit,sizeof(SndVector));
		memcpy(&pMicro->dir_nor,&par->dirnor,sizeof(SndVector));
		memcpy(&pMicro->dir_tan,&par->dirtan,sizeof(SndVector));
	}

    g_iNbMicrosSxd++;

    SND_fn_vCreateMicroSxdWave(par,(long)pMicro);

    return (long)pMicro;
}

void SND_CALL SND_fn_vSetMicroParamSxd(long sxdId,MicroParam* par)
{
    p_micro_description pMicro;
    int i;

    SNDLST2_M_DynamicForEachElementOf(&micros_sxd.pList,pMicro,i)
    {
        if ((long)pMicro==sxdId) break;
    }
    if (pMicro!=NULL)
	{
		pMicro->just_created=FALSE;
		memcpy(&pMicro->pos,&par->pos,sizeof(SndVector));
		memcpy(&pMicro->vit,&par->vit,sizeof(SndVector));
		memcpy(&pMicro->dir_nor,&par->dirnor,sizeof(SndVector));
		memcpy(&pMicro->dir_tan,&par->dirtan,sizeof(SndVector));
	}

    SND_fn_vSetMicroParamSxdWave(sxdId,par);
}

void SND_CALL SND_fn_vDestroyMicroSxd(long sxdId)
{
    p_micro_description pMicro=(p_micro_description)sxdId;

    if (pMicro!=NULL)
    {
        SNDLST2_M_DynamicIsolate(pMicro);
        SND_fn_vFreeSndEx(E_ucSndBlockMain,pMicro);
        g_iNbMicrosSxd--;
    }

    SND_fn_vDestroyMicroSxdWave(sxdId);
}

/* 0x1000C240 */
typedef struct _SND_pc_tdstBufferSxd *SND_pc_p_tdstBufferSxd;

SNDLST2_M_DynamicUseListOf(SND_pc_p_tdstBufferSxd);

typedef struct _SND_pc_tdstBufferSxd {
	SND_tduTypeCallback	eType;
	long				lIdBuffer;
	SNDLST2_M_DynamicElementDeclaration(SND_pc_p_tdstBufferSxd)
} SND_pc_tdstBufferSxd;

long SND_pc_fn_lCreateStackBuffer(SND_tdstFormat* pformat,SND_tdstCallback* pCallback,SoundParam* par,long user_id);
SndReal SND_pc_fn_rGetPosStackBuffer(long id_buffer);
long SND_pc_fn_lPushStackBuffer(long id_buffer,SND_tdstStackBuffer* pStack);
void SND_pc_fn_vSetParamStackBuffer(long id_buffer,SoundParam *par);
void SND_pc_fn_vDeleteStackBuffer(long id_buffer);
void SND_pc_fn_vPauseStackBuffer(long id_buffer);
void SND_pc_fn_vResumeStackBuffer(long id_buffer);
long SND_pc_fn_lGetPosStackBuffer(long id_buffer);

long SND_CALL SND_fn_lCreateNewBufferSxd(unsigned long nb_samples,unsigned short uwResolution,unsigned short uwNbChannels,unsigned long ulFreq,SoundParam* par,td_pfn_vRefreshBufferClient callback,long user_id)
{
	SND_tdstFormat		stFormat;
	SND_tdstCallback	stCallback;

	memset(&stFormat,0,sizeof(stFormat));
	stFormat.uFormat.stPCM.ulNbSamples=nb_samples;
	stFormat.uFormat.stPCM.uwResolution=uwResolution;
	stFormat.uFormat.stPCM.uwNbChannels=uwNbChannels;
	stFormat.uFormat.stPCM.ulFreq=ulFreq;
	memset(&stCallback,0,sizeof(stCallback));
	stCallback.eType=BUFFER_ASYNCHRONE;
	stCallback.uCallback.CallbackASynchrone=callback;
	stFormat.eZip=SAMPLE_PCM;

	return SND_fn_lCreateNewBufferExSxd(&stFormat,&stCallback,par,user_id);
}

void SND_CALL SND_fn_vSetParamBufferSxd(long id_buffer,SoundParam *par)
{
	if (id_buffer!=C_PLAY_FAILED)
	{
		if (((SND_pc_tdstBufferSxd*)id_buffer)->eType==BUFFER_STACK)
			SND_pc_fn_vSetParamStackBuffer(((SND_pc_tdstBufferSxd*)id_buffer)->lIdBuffer,par);
		else
			SND_fn_vSetParamBufferSxdWave(((SND_pc_tdstBufferSxd*)id_buffer)->lIdBuffer,par);
	}
}

void SND_CALL SND_fn_vDeleteBufferSxd(long id_buffer)
{
	if (id_buffer!=C_PLAY_FAILED)
	{
		if (((SND_pc_tdstBufferSxd*)id_buffer)->eType==BUFFER_STACK)
			SND_pc_fn_vDeleteStackBuffer(((SND_pc_tdstBufferSxd*)id_buffer)->lIdBuffer);
		else
			SND_fn_vDeleteBufferSxdWave(((SND_pc_tdstBufferSxd*)id_buffer)->lIdBuffer);
	}
}

void SND_CALL SND_fn_vPauseBufferSxd(long id_buffer)
{
	if (id_buffer!=C_PLAY_FAILED)
	{
		if (((SND_pc_tdstBufferSxd*)id_buffer)->eType==BUFFER_STACK)
			SND_pc_fn_vPauseStackBuffer(((SND_pc_tdstBufferSxd*)id_buffer)->lIdBuffer);
		else
			SND_fn_vPauseBufferSxdWave(((SND_pc_tdstBufferSxd*)id_buffer)->lIdBuffer);
	}
}

void SND_CALL SND_fn_vResumeBufferSxd(long id_buffer)
{
	if (id_buffer!=C_PLAY_FAILED)
	{
		if (((SND_pc_tdstBufferSxd*)id_buffer)->eType==BUFFER_STACK)
			SND_pc_fn_vResumeStackBuffer(((SND_pc_tdstBufferSxd*)id_buffer)->lIdBuffer);
		else
			SND_fn_vResumeBufferSxdWave(((SND_pc_tdstBufferSxd*)id_buffer)->lIdBuffer);
	}
}

void* SND_CALL SND_fn_pvGetPtrBufferSxd(long id_buffer)
{
	if ((id_buffer!=C_PLAY_FAILED) && (((SND_pc_tdstBufferSxd*)id_buffer)->eType!=BUFFER_STACK))
		return SND_fn_pvGetPtrBufferSxdWave(((SND_pc_tdstBufferSxd*)id_buffer)->lIdBuffer);
	return NULL;
}

SndReal SND_CALL SND_fn_rGetPosBufferSxd(long id_buffer)
{
	if (id_buffer!=C_PLAY_FAILED)
	{
		if (((SND_pc_tdstBufferSxd*)id_buffer)->eType==BUFFER_STACK)
			return SND_pc_fn_rGetPosStackBuffer(((SND_pc_tdstBufferSxd*)id_buffer)->lIdBuffer);
		else
			return SND_fn_rGetPosBufferSxdWave(((SND_pc_tdstBufferSxd*)id_buffer)->lIdBuffer);
	}
	return -1;
}

long SND_CALL SND_fn_lCreateNewBufferExSxd(SND_tdstFormat* pformat,SND_tdstCallback* pCallback,SoundParam* par,long user_id)
{
	long					id;
	SND_pc_tdstBufferSxd	*pBuffer;

	if (pformat->eZip==SAMPLE_PCM)
	{
		if (pCallback->eType==BUFFER_STACK)
			id=SND_pc_fn_lCreateStackBuffer(pformat,pCallback,par,user_id);
		else
			id=SND_fn_lCreateNewBufferExSxdWave(pformat,pCallback,par,user_id);
	}
	else
	{
		SND_fn_vDisplayError(E_uwSndInvalidTypeOfResource,"BufferClient must be in PCM format.");
		id=C_PLAY_FAILED;
	}

	if (id!=C_PLAY_FAILED)
	{
		pBuffer=SND_fn_pvMallocSndEx(E_ucSndBlockMain,sizeof(SND_pc_tdstBufferSxd));
		SNDLST2_M_DynamicInitElement(pBuffer);
		pBuffer->eType=pCallback->eType;
		pBuffer->lIdBuffer=id;
		return (long)pBuffer;
	}
	return C_PLAY_FAILED;
}

long SND_CALL SND_fn_lGetPosBufferExSxd(long id_buffer)
{
	if (id_buffer!=C_PLAY_FAILED)
	{
		if (((SND_pc_tdstBufferSxd*)id_buffer)->eType==BUFFER_STACK)
			return SND_pc_fn_lGetPosStackBuffer(((SND_pc_tdstBufferSxd*)id_buffer)->lIdBuffer);
		else
			return SND_fn_lGetPosBufferExSxdWave(((SND_pc_tdstBufferSxd*)id_buffer)->lIdBuffer);
	}
	return -1;
}

long SND_CALL SND_fn_lPushBufferExSxd(long id_buffer,SND_tdstStackBuffer* pStack)
{
	if (id_buffer!=C_PLAY_FAILED)
		return SND_pc_fn_lPushStackBuffer(((SND_pc_tdstBufferSxd*)id_buffer)->lIdBuffer,pStack);
	return C_PLAY_FAILED;
}

typedef struct {
    tdeTypeZip	eZip;
    long		lVoice;
} transition;

/* 0x1000C4E0 */
long SND_CALL SND_fn_lPlayTransitionSxd(tduRefRes FirstRes,tduRefRes NextRes,SND_td_pfn_vSoundCallback transition_callback,SoundParam* par)
{
	return C_PLAY_FAILED;
}

/* 0x1000C4F0 */
long SND_CALL SND_fn_lPlayTransitionExSxd(tduRefRes FirstRes,tduRefRes NextRes,SND_td_pfn_vSoundCallback transition_callback,long param_callback,SoundParam* par)
{
	transition	*pTransition;
	SampleParam	par2D;
	SampleParam	*pPar2D;

	pTransition=SND_fn_pvMallocSndEx(E_ucSndBlockMain,sizeof(transition));
	switch (FirstRes.pstPtr->uRes.stSample.eZip)
	{
	case SAMPLE_PCM:
		pTransition->lVoice=FUN_10006d30(FirstRes,NextRes,transition_callback,param_callback,par);
		pTransition->eZip=FirstRes.pstPtr->uRes.stSample.eZip;
		break;
	case SAMPLE_MPEG:
		pPar2D=(SampleParam*)par;
		if (par!=NULL)
		{
			SND_fn_vParam3Dto2D(par,&par2D);
			pPar2D=&par2D;
		}
		pTransition->lVoice=SND_fn_lPlayTransitionMPEG(FirstRes,NextRes,transition_callback,param_callback,pPar2D);
		pTransition->eZip=FirstRes.pstPtr->uRes.stSample.eZip;
		break;
	case SAMPLE_ADPCM:
		pTransition->lVoice=SND_fn_lPlayTransitionSxdADPCM(FirstRes,NextRes,transition_callback,param_callback,par);
		pTransition->eZip=FirstRes.pstPtr->uRes.stSample.eZip;
		break;
	default:
		SND_M_DisplayError(E_uwSndInvalidStaticParamForResource,"");
		pTransition->lVoice=C_PLAY_FAILED;
	}

	if (pTransition->lVoice!=C_PLAY_FAILED)
		return (long)pTransition;
	SND_fn_vFreeSndEx(E_ucSndBlockMain,pTransition);
	return C_PLAY_FAILED;
}

SndBool SND_CALL SND_fn_bSetParamTransitionSxd(long voice,SoundParam *par)
{
	SampleParam	par2D;
	SampleParam	*pPar2D;

	switch (((transition*)voice)->eZip)
	{
	case SAMPLE_PCM:
		return FUN_10006ec0(((transition*)voice)->lVoice,par);
	case SAMPLE_MPEG:
		if (par==NULL)
			pPar2D=NULL;
		else
		{
			SND_fn_vParam3Dto2D(par,&par2D);
			pPar2D=&par2D;
		}
		return SND_fn_bSetParamTransitionMPEG(((transition*)voice)->lVoice,pPar2D);
	case SAMPLE_ADPCM:
		return SND_fn_bSetParamTransitionSxdADPCM(((transition*)voice)->lVoice,par);
	default:
		SND_M_DisplayError(E_uwSndInvalidStaticParamForResource,"");
		return FALSE;
	}
}

SndBool SND_CALL SND_fn_bSetNextTransitionSxd(long voice,tduRefRes new_res)
{
	switch (((transition*)voice)->eZip)
	{
	case SAMPLE_PCM:
		return FUN_10006f10(((transition*)voice)->lVoice,new_res);
	case SAMPLE_MPEG:
		return SND_fn_bSetNextTransitionMPEG(((transition*)voice)->lVoice,new_res);
	case SAMPLE_ADPCM:
		return SND_fn_bSetNextTransitionSxdADPCM(((transition*)voice)->lVoice,new_res);
	default:
		SND_M_DisplayError(E_uwSndInvalidStaticParamForResource,"");
		return FALSE;
	}
}

SndBool SND_CALL SND_fn_bDoTransitionWithFadeSxd(long voice,tduRefRes new_res)
{
	switch (((transition*)voice)->eZip)
	{
	case SAMPLE_PCM:
		return FUN_10006fa0(((transition*)voice)->lVoice,new_res);
	case SAMPLE_MPEG:
		return SND_fn_bDoTransitionWithFadeMPEG(((transition*)voice)->lVoice,new_res);
	case SAMPLE_ADPCM:
		return SND_fn_bDoTransitionWithFadeSxdADPCM(((transition*)voice)->lVoice,new_res);
	default:
		SND_M_DisplayError(E_uwSndInvalidStaticParamForResource,"");
		return FALSE;
	}
}

SndBool SND_CALL SND_fn_bDoTransitionWithFadeSxd2( long voice, tduRefRes new_res, SndReal rDuration )
{
	switch (((transition*)voice)->eZip)
	{
	case SAMPLE_PCM:
		return FUN_10006f40(((transition*)voice)->lVoice,new_res,rDuration);
	case SAMPLE_MPEG:
		return SND_fn_bDoTransitionWithFadeMPEG2(((transition*)voice)->lVoice,new_res,rDuration);
	case SAMPLE_ADPCM:
		return SND_fn_bDoTransitionWithFadeSxdADPCM2(((transition*)voice)->lVoice,new_res,rDuration);
	default:
		SND_M_DisplayError(E_uwSndInvalidStaticParamForResource,"");
		return FALSE;
	}
}

void SND_CALL SND_fn_vStopTransitionSxd(long voice)
{
	switch (((transition*)voice)->eZip)
	{
	case SAMPLE_PCM:
		FUN_10006fc0(((transition*)voice)->lVoice);
		break;
	case SAMPLE_MPEG:
		SND_fn_vStopTransitionMPEG(((transition*)voice)->lVoice);
		break;
	case SAMPLE_ADPCM:
		SND_fn_vStopTransitionSxdADPCM(((transition*)voice)->lVoice);
		break;
	default:
		SND_M_DisplayError(E_uwSndInvalidStaticParamForResource,"");
	}
}

void SND_CALL SND_fn_vPauseTransitionSxd(long voice)
{
	switch (((transition*)voice)->eZip)
	{
	case SAMPLE_PCM:
		FUN_10006ff0(((transition*)voice)->lVoice);
		break;
	case SAMPLE_MPEG:
		SND_fn_vPauseTransitionMPEG(((transition*)voice)->lVoice);
		break;
	case SAMPLE_ADPCM:
		SND_fn_vPauseTransitionSxdADPCM(((transition*)voice)->lVoice);
		break;
	default:
		SND_M_DisplayError(E_uwSndInvalidStaticParamForResource,"");
	}
}

void SND_CALL SND_fn_vResumeTransitionSxd(long voice)
{
	switch (((transition*)voice)->eZip)
	{
	case SAMPLE_PCM:
		FUN_10007010(((transition*)voice)->lVoice);
		break;
	case SAMPLE_MPEG:
		SND_fn_vResumeTransitionMPEG(((transition*)voice)->lVoice);
		break;
	case SAMPLE_ADPCM:
		SND_fn_vResumeTransitionSxdADPCM(((transition*)voice)->lVoice);
		break;
	default:
		SND_M_DisplayError(E_uwSndInvalidStaticParamForResource,"");
	}
}

/* 0x1000C880 */
void SND_CALL SND_fn_vReleaseDriverSxd(void)
{
	SND_fn_vReleaseDriverMPEG();
	SND_fn_vReleaseDriverSxdADPCM();
	SND_fn_vReleaseDriverSxdWave();
}

void SND_CALL SND_fn_vRestoreDriverSxd(void)
{
	SND_fn_vRestoreDriverSxdWave();
	SND_fn_vRestoreDriverSxdADPCM();
	SND_fn_vRestoreDriverMPEG();
}

void SND_CALL SND_fn_vForceReleaseDriverSxd(void)
{
	SND_fn_vForceReleaseDriverSxdWave();
}

void SND_CALL SND_fn_vForceRestoreDriverSxd(void)
{
	SND_fn_vForceRestoreDriverSxdWave();
}

void SND_CALL SND_fn_vGetTargetLabelSxd(char* name,int nb_char)
{
	SND_fn_vGetTargetLabelSxdWave(name,nb_char);
}

void SND_CALL SND_fn_vSetupTargetSxd()
{
	SND_fn_vSetupTargetSxdWave();
}

SndBool SND_CALL SND_fn_bCanSetupTargetSxd()
{
	return SND_fn_bCanSetupTargetSxdWave();
}

void SND_CALL SND_fn_vSynchroSxd()
{
	FUN_10007e30(SND_pc_hVirtualModuleADPCM);
	SND_fn_vSynchroSxdWave();
}

/* 0x1000C920 */
int SND_CALL SND_fn_iInitBnkSxd(long num)
{
	return C_INIT_FAILED;
}

SndBool SND_CALL SND_fn_bTestInitBnkSxd(long num)
{
	return FALSE;
}

void SND_CALL SND_fn_vDesInitBnkSxd(long num)
{
}

void SND_CALL SND_fn_vDesInitAllBnkSxd(void)
{
}

void SND_CALL SND_fn_vSetDefaultRollOffSxd(RollOffParam* rolloff)
{
	SND_fn_vSetDefaultRollOffSxdWave(rolloff);
#ifndef DISABLE_MPEG
	SND_fn_vSetDefaultRollOffMPEG(rolloff);
#endif
	SND_fn_vSetDefaultRollOffSxdADPCM(rolloff);
}

/* 0x1000C980 */
typedef struct _SND_pc_tdstStackElement *SND_pc_p_tdstStackElement;

SNDLST2_M_DynamicUseListOf(SND_pc_p_tdstStackElement);

typedef struct _SND_pc_tdstStackElement {
	void			*pvData;
	unsigned long	ulNbSamples;
	void			*pvCurrent;
	unsigned long	ulNbLeft;
	long			lId;
	SndBool			bReady;
	SNDLST2_M_DynamicElementDeclaration(SND_pc_p_tdstStackElement)
} SND_pc_tdstStackElement;

typedef struct _SND_pc_tdstStackBuffer {
	long				lIdBuffer;
	SND_tdstFormat		stFormat;
	SND_tdstCallback	stCallback;
	long				lUserId;
	SNDLST2_M_DynamicAnchorDeclaration(SND_pc_p_tdstStackElement) hStack;
	long				lShift;
	long				lPos;
} SND_pc_tdstStackBuffer;

static long SND_pc_lNextStackId_28074;

void SND_CALL SND_pc_fn_vRefreshStackBuffer(long user_id,unsigned long first,unsigned long nb,void* ptr_first_sample)
{
	SND_pc_tdstStackBuffer		*pBuffer=(SND_pc_tdstStackBuffer*)user_id;
	SND_pc_tdstStackElement		*pElement;
	unsigned long				ulNbLeft,ulNb;
	char						*pDst;

	pElement=SNDLST2_M_DynamicGetFirstElement(&pBuffer->hStack);
	if ((pElement==NULL) || !pElement->bReady)
	{
		memset(ptr_first_sample,0,nb<<pBuffer->lShift);
	}
	else
	{
		ulNbLeft=nb;
		pDst=(char*)ptr_first_sample;
		while (ulNbLeft>0)
		{
			ulNb=pElement->ulNbLeft;
			if (ulNb>=ulNbLeft)
				ulNb=ulNbLeft;
			memcpy(pDst,pElement->pvCurrent,ulNb<<pBuffer->lShift);
			pElement->pvCurrent=(char*)pElement->pvCurrent+(ulNb<<pBuffer->lShift);
			pDst+=(ulNb<<pBuffer->lShift);
			pElement->ulNbLeft-=ulNb;
			ulNbLeft-=ulNb;
			if (pElement->ulNbLeft<=0)
			{
				pBuffer->stCallback.uCallback.CallbackStack(pBuffer->lUserId,pElement->lId);
				SNDLST2_M_DynamicIsolate(pElement);
				SND_fn_vFreeSndEx(E_ucSndBlockMain,pElement);
				pElement=SNDLST2_M_DynamicGetFirstElement(&pBuffer->hStack);
				if (pElement==NULL)
				{
					memset(pDst,0,ulNbLeft<<pBuffer->lShift);
					ulNbLeft=0;
				}
			}
		}
	}
	pBuffer->lPos+=nb;
}

long SND_pc_fn_lCreateStackBuffer(SND_tdstFormat* pformat,SND_tdstCallback* pCallback,SoundParam* par,long user_id)
{
	SND_pc_tdstStackBuffer	*pBuffer;
	SND_tdstCallback		stCallback;

	pBuffer=SND_fn_pvMallocSndEx(E_ucSndBlockMain,sizeof(SND_pc_tdstStackBuffer));
	pBuffer->stFormat=*pformat;
	pBuffer->stCallback=*pCallback;
	pBuffer->lUserId=user_id;
	pBuffer->lShift=pformat->uFormat.stPCM.uwNbChannels-2+(pformat->uFormat.stPCM.uwResolution>>3);
	SNDLST2_M_DynamicInitAnchor(&pBuffer->hStack);
	pBuffer->lPos=0;
	stCallback.eType=BUFFER_SYNCHRONE;
	stCallback.uCallback.CallbackSynchrone=SND_pc_fn_vRefreshStackBuffer;
	stCallback.uInfo.rCallbackPeriod=0;
	pBuffer->lIdBuffer=SND_fn_lCreateNewBufferExSxd(&pBuffer->stFormat,&stCallback,par,(long)pBuffer);
	return (long)pBuffer;
}

SndReal SND_pc_fn_rGetPosStackBuffer(long id_buffer)
{
	double	dPos;

	dPos=(double)SND_pc_fn_lGetPosStackBuffer(id_buffer)/(double)((SND_pc_tdstStackBuffer*)id_buffer)->stFormat.uFormat.stPCM.ulFreq;
	if ((long)dPos<M_HighestIntAbsValueRealSnd)
		return M_DoubleToRealSnd(dPos);
	return -1;
}

long SND_pc_fn_lPushStackBuffer(long id_buffer,SND_tdstStackBuffer* pStack)
{
	SND_pc_tdstStackElement	*pElement;

	pElement=SND_fn_pvMallocSndEx(E_ucSndBlockMain,sizeof(SND_pc_tdstStackElement));
	SNDLST2_M_DynamicInitElement(pElement);
	pElement->pvData=pStack->ptrData;
	pElement->ulNbSamples=pStack->ulNbSamples;
	pElement->pvCurrent=pElement->pvData;
	pElement->ulNbLeft=pElement->ulNbSamples;
	pElement->bReady=FALSE;
	pElement->lId=SND_pc_lNextStackId_28074++;
	if (SND_pc_lNextStackId_28074>=0x7FFFFFFF)
		SND_pc_lNextStackId_28074=0;
	SNDLST2_M_DynamicAddTail(&((SND_pc_tdstStackBuffer*)id_buffer)->hStack,pElement);
	pElement->bReady=TRUE;
	return pElement->lId;
}

void SND_pc_fn_vSetParamStackBuffer(long id_buffer,SoundParam *par)
{
	SND_fn_vSetParamBufferSxd(((SND_pc_tdstStackBuffer*)id_buffer)->lIdBuffer,par);
}

void SND_pc_fn_vDeleteStackBuffer(long id_buffer)
{
	SND_pc_tdstStackElement	*pElement,*pNextElement;
	int						i;

	SND_fn_vDeleteBufferSxd(((SND_pc_tdstStackBuffer*)id_buffer)->lIdBuffer);
	SNDLST2_M_DynamicForEachMovingElementOf(&((SND_pc_tdstStackBuffer*)id_buffer)->hStack,pElement,pNextElement,i)
	{
		SNDLST2_M_DynamicIsolate(pElement);
		SND_fn_vFreeSndEx(E_ucSndBlockMain,pElement);
	}
	SND_fn_vFreeSndEx(E_ucSndBlockMain,(void*)id_buffer);
}

void SND_pc_fn_vPauseStackBuffer(long id_buffer)
{
	SND_fn_vPauseBufferSxd(((SND_pc_tdstStackBuffer*)id_buffer)->lIdBuffer);
}

void SND_pc_fn_vResumeStackBuffer(long id_buffer)
{
	SND_fn_vResumeBufferSxd(((SND_pc_tdstStackBuffer*)id_buffer)->lIdBuffer);
}

long SND_pc_fn_lGetPosStackBuffer(long id_buffer)
{
	return ((SND_pc_tdstStackBuffer*)id_buffer)->lPos;
}

/* 0x1000CDD0 */
tdeTypeZip SND_pc_fn_eGetAdpcmOrPcmFormat(void)
{
	return SND_fn_bIsLibActiveSxdADPCM() ? SAMPLE_ADPCM : SAMPLE_PCM;
}

/* 0x1000CDE0 */
void SND_CALL SND_fn_vSetNbVoiceWishedSxd(tdeTypeZip ezip,long nb)
{
	switch (ezip)
	{
	case SAMPLE_PCM:
		break;
	case SAMPLE_ADPCM:
		if (!SND_fn_b3DOneShootShouldBeConvertedSxdWave())
			FUN_10007e50(SND_pc_hVirtualModuleADPCM,nb);
		break;
	default:
		SND_fn_vDisplayError(E_uwSndInvalidStaticParamForResource,"This Zip type doesn't manage with SNDVIRT");
	}
}

tdeTypeZip SND_CALL SND_fn_eGetMainFormatSxd()
{
	if (!SND_fn_b3DOneShootShouldBeConvertedSxdWave() && SND_fn_bIsLibActiveSxdADPCM())
		return SAMPLE_ADPCM;
	return SAMPLE_PCM;
}

long SND_CALL SND_fn_lGetNbVoiceWishedSxd()
{
	return SND_fn_lGetNbVoiceWishedSxdWave();
}

long SND_CALL SND_fn_lGetNbVoiceOptimumSxd()
{
	return SND_fn_lGetNbVoiceOptimumSxdWave();
}

SndBool SND_CALL SND_fn_bSetResourceStaticVolumeSxd(tduRefRes res,unsigned char ucVolume)
{
	SndBool ret;

	switch (res.pstPtr->uRes.stSample.eZip)
	{
	case SAMPLE_PCM:
		if (res.pstPtr->uRes.stSample.bStream)
			ret=FUN_10006840(res,ucVolume);
		else
			ret=SND_fn_bSetResourceStaticVolumeSxdWave(res,ucVolume);
		break;
	case SAMPLE_MPEG:
		ret=SND_fn_bSetResourceStaticVolumeMPEG(res,ucVolume);
		break;
	case SAMPLE_ADPCM:
		ret=SND_fn_bSetResourceStaticVolumeSxdADPCM(res,ucVolume);
		break;
	default:
		SND_M_DisplayError(E_uwSndInvalidStaticParamForResource,"");
		ret=FALSE;
	}
	return ret;
}
