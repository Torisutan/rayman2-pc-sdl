#include <stddef.h>
#include <string.h>
#include "SND/sndres.h"
#include "SND/sndparam_pc.h"

long __cdecl SND_fn_lGetSwitchIndice(long objecttype, long objectid, tduRefRes uRes);

SndBool SND_fn_bIsResourceSwitch(tduRefRes res)
{
	return (((res.pstPtr!=NULL) && (res.pstPtr->eType==TYPE_SWITCH)) ? TRUE : FALSE);
}

tduRefRes SND_fn_tdGetEffectiveResourceSwitch(tduRefRes res_switch,struct SoundParam_ *par,unsigned char *pucScale_out)
{
	tdstSwitchElement*	pSwitchElement;
	tdstTypeSwitchMem*	pSwitchList;
	tduRefRes		uRes;
	long			indice;
	int			i;
	SndBool			bFound;
	unsigned int		uScale;
	int			iCount;
	SwitchParam* pstSwitch;

	uScale=1;
	iCount=0;
	uRes=res_switch;

	for (;;)
	{
		if ((uRes.pstPtr==SND_C_RES_FANTOME_PTR)
			|| (!SND_fn_bIsResourceSwitch(uRes)))
		{
			if (iCount>1)
				*pucScale_out=(unsigned char)(uScale/(unsigned int)iCount);
			else
				*pucScale_out=(unsigned char)uScale;
			return uRes;
		}
		{
			uScale=uScale*(unsigned int)uRes.pstPtr->ucVolume;
			iCount++;

			if (par->iFlags & C_SOUNDPARAM_SWITCH)
			{
				pstSwitch=&(SND_M_GetExtraParamFromSoundParam(par))->stSwitch;
				indice=SND_fn_lGetSwitchIndice(pstSwitch->lObjectType,pstSwitch->lObjectId,uRes);
			}
			else
				indice=uRes.pstPtr->uRes.stSwitch.hDefault;

			pSwitchList=&(uRes.pstPtr->uRes.stSwitch);
			bFound=FALSE;
			SNDLST2_M_StaticForEachElementOf(pSwitchList,pSwitchElement,i)
			{
				if (pSwitchElement->hIndice==indice)
				{
					uRes=pSwitchElement->uRes;
					bFound=TRUE;
					break;
				}
			}
			if (bFound==FALSE)
			{
				if (uRes.pstPtr->uRes.stSwitch.hDefault!=C_SWITCH_NODEFAULT)
				{
					bFound=FALSE;
					SNDLST2_M_StaticForEachElementOf(pSwitchList,pSwitchElement,i)
					{
						if (pSwitchElement->hIndice==uRes.pstPtr->uRes.stSwitch.hDefault)
						{
							uRes=pSwitchElement->uRes;
							bFound=TRUE;
							break;
						}
					}
					if (bFound==FALSE)

						uRes.pstPtr=SND_C_RES_FANTOME_PTR;
				}
				else
				{
					uRes.pstPtr=SND_C_RES_FANTOME_PTR;
				}
			}
		}
	}
}

#define E_ucSndBlockMain		((unsigned char)0)

typedef struct switch_voice_ switch_voice;
struct switch_voice_{
	long		lLowVoice;
	tduRefRes	res_low;
	tduRefRes	res_hi;
	long		prio;
	SND_td_pfn_vSoundCallback fn_callback;
	long		par_callback;
	SndBool		bValid;
	unsigned char	ucCachedScale;
};

void *SND_CALL SND_fn_pvMallocSndEx(unsigned char id_block, unsigned int ulNbBytes);
void SND_CALL SND_fn_vFreeSndEx(unsigned char id_block, void *pvPtr);

long SND_fn_lPlaySnd(tduRefRes res, struct SoundParam_ *par, long prio, SND_td_pfn_vSoundCallback fn_callback, long par_callback, SndBool *valid);

long SND_fn_lPlaySwitch(tduRefRes num_res,SoundParam* par,long prio,SND_td_pfn_vSoundCallback fn_callback,long par_callback)
{
	tduRefRes	low_res;
	switch_voice *pstSwitchVoice;
	unsigned char	ucScale;
	SoundParam	stSoundParam;

	pstSwitchVoice=SND_fn_pvMallocSndEx(E_ucSndBlockMain,sizeof(switch_voice));
	pstSwitchVoice->bValid=TRUE;

	low_res=SND_fn_tdGetEffectiveResourceSwitch(num_res,par,&ucScale);

	pstSwitchVoice->res_hi=num_res;
	pstSwitchVoice->prio=prio;
	pstSwitchVoice->ucCachedScale=ucScale;
	pstSwitchVoice->res_low=low_res;
	pstSwitchVoice->fn_callback=fn_callback;
	pstSwitchVoice->par_callback=par_callback;

	if (low_res.pstPtr!=SND_C_RES_FANTOME_PTR)
	{

		memcpy(&stSoundParam,par,SND_M_lGetSizeOfSoundParam(par));
		stSoundParam.ucVol=(unsigned char)(((int)stSoundParam.ucVol*(int)ucScale)>>7);
		stSoundParam.iFlags|=C_SOUNDPARAM_COEF_VOL;

		pstSwitchVoice->lLowVoice=SND_fn_lPlaySnd(low_res,&stSoundParam,prio,NULL,0,&pstSwitchVoice->bValid);

		if (pstSwitchVoice->lLowVoice!=C_PLAY_FAILED)
			return (long)pstSwitchVoice;
	}

	SND_fn_vFreeSndEx(E_ucSndBlockMain,pstSwitchVoice);

	return C_PLAY_FAILED;
}

SndBool SND_CALL SND_fn_bSetParamSnd(long voie, struct SoundParam_ *par);
SndBool __cdecl SND_fn_bTestIsPlayingSnd(long voie);
void SND_CALL SND_fn_vStopSnd(long voie);
void SND_CALL SND_fn_vPauseSnd(long voie);
void SND_CALL SND_fn_vResumeSnd(long voie);
SndReal SND_CALL SND_fn_rGetPosSnd(long voie);

/* 0x0045A6F0 */
SndBool SND_CALL SND_fn_bSetParamSwitch(long voice,SoundParam* par)
{
	switch_voice *pstSwitchVoice=(switch_voice*)voice;
	tduRefRes	new_res;
	unsigned char	ucScale=0x7F;
	SndBool		bResult=FALSE;
	SoundParam	stSoundParam;

	if (pstSwitchVoice->res_hi.pstPtr->uRes.stSwitch.bDynamic)
		new_res=SND_fn_tdGetEffectiveResourceSwitch(pstSwitchVoice->res_hi,par,&ucScale);
	else
		new_res=pstSwitchVoice->res_low;

	memcpy(&stSoundParam,par,SND_M_lGetSizeOfSoundParam(par));

	if (new_res.pstPtr==pstSwitchVoice->res_low.pstPtr)
	{
		stSoundParam.ucVol=(unsigned char)(((int)stSoundParam.ucVol*(int)pstSwitchVoice->ucCachedScale)>>7);
		stSoundParam.iFlags|=C_SOUNDPARAM_COEF_VOL;
		if (pstSwitchVoice->bValid && SND_fn_bSetParamSnd(pstSwitchVoice->lLowVoice,&stSoundParam))
			bResult=TRUE;
	}
	else
	{
		SND_fn_vStopSnd(pstSwitchVoice->lLowVoice);
		stSoundParam.ucVol=(unsigned char)(((int)stSoundParam.ucVol*(int)ucScale)>>7);
		stSoundParam.iFlags|=C_SOUNDPARAM_COEF_VOL;
		pstSwitchVoice->lLowVoice=SND_fn_lPlaySnd(new_res,&stSoundParam,pstSwitchVoice->prio,NULL,0,&pstSwitchVoice->bValid);
		pstSwitchVoice->res_low=new_res;
		pstSwitchVoice->ucCachedScale=ucScale;

		if (pstSwitchVoice->lLowVoice!=C_PLAY_FAILED)
			bResult=TRUE;
	}

	if (bResult)
		return TRUE;

	if (pstSwitchVoice->fn_callback)
		(*pstSwitchVoice->fn_callback)(pstSwitchVoice->par_callback);

	SND_fn_vFreeSndEx(E_ucSndBlockMain,pstSwitchVoice);

	return FALSE;
}

/* 0x0045A830 */
SndBool SND_CALL SND_fn_bTestIsPlayingSwitch(long voice)
{
	switch_voice *pstSwitchVoice=(switch_voice*)voice;

	if (pstSwitchVoice->bValid && SND_fn_bTestIsPlayingSnd(pstSwitchVoice->lLowVoice))
		return TRUE;
	else
	{
		if (pstSwitchVoice->fn_callback)
			(*pstSwitchVoice->fn_callback)(pstSwitchVoice->par_callback);

		SND_fn_vFreeSndEx(E_ucSndBlockMain,pstSwitchVoice);

		return FALSE;
	}
}

/* 0x0045A870 */
void SND_CALL SND_fn_vStopSwitch(long voice)
{
	switch_voice *pstSwitchVoice=(switch_voice*)voice;

	if (pstSwitchVoice->bValid)
		SND_fn_vStopSnd(pstSwitchVoice->lLowVoice);

	if (pstSwitchVoice->fn_callback)
		(*pstSwitchVoice->fn_callback)(pstSwitchVoice->par_callback);

	SND_fn_vFreeSndEx(E_ucSndBlockMain,pstSwitchVoice);
}

/* 0x0045A8A0 */
void SND_CALL SND_fn_vPauseSwitch(long voice)
{
	switch_voice *pstSwitchVoice=(switch_voice*)voice;

	if (pstSwitchVoice->bValid)
		SND_fn_vPauseSnd(pstSwitchVoice->lLowVoice);
}

/* 0x0045A8C0 */
void SND_CALL SND_fn_vResumeSwitch(long voice)
{
	switch_voice *pstSwitchVoice=(switch_voice*)voice;

	if (pstSwitchVoice->bValid)
		SND_fn_vResumeSnd(pstSwitchVoice->lLowVoice);
}

#define SND_C_POS_UNKNOWN -1
SndReal SND_CALL SND_fn_rGetPosSwitch(long voice)
{
	switch_voice *pstSwitchVoice=(switch_voice*)voice;

	if (pstSwitchVoice->bValid)
		return SND_fn_rGetPosSnd(pstSwitchVoice->lLowVoice);
	else
		return SND_C_POS_UNKNOWN;
}

void *SND_CALL SND_fn_pGetBinRes(unsigned long _ulBinId);

/* 0x0045A900 */
void __cdecl SND_fn_vConvertResDiskToMemSwitch(tdstBlockResourceDisk* disk,tdstBlockResourceMem* mem,void* ptrBegin)
{
        mem->Id=disk->Id;
        mem->eType=disk->eType;
        mem->eStorage=disk->eStorage;
        mem->ucVolume=disk->ucVolume;
        mem->bIsLoaded=TRUE;

        mem->uRes.stSwitch.ulNbElements = disk->uRes.stSwitch.ulNbElements;
        mem->uRes.stSwitch.lTypeSwitch = disk->uRes.stSwitch.lTypeSwitch;
        mem->uRes.stSwitch.hDefault = disk->uRes.stSwitch.hDefault;
		mem->uRes.stSwitch.bDynamic=disk->uRes.stSwitch.bDynamic;
}

/* 0x0045A940 */
SndBool __cdecl SND_fn_bLoadResBinarySwitch(tdstBlockResourceDisk *_pBRDisk,tdstBlockResourceMem *_pBRMem,char *pDataBloc)
{
        tdstSwitchElement *pSwitchArray,*pSwitchElement;

        int iPos;

        pSwitchArray=(tdstSwitchElement *)(pDataBloc + _pBRDisk->ulDataOffset);

        SNDLST2_M_StaticSetMember(&_pBRMem->uRes.stSwitch,hFirstElement,pSwitchArray);
		SNDLST2_M_StaticSetMember(&_pBRMem->uRes.stSwitch, lNumberOfElements, _pBRDisk->uRes.stSwitch.ulNbElements);

        SNDLST2_M_StaticForEachElementOf( &_pBRMem->uRes.stSwitch, pSwitchElement, iPos)
        {
            pSwitchElement->uRes.pstPtr=SND_fn_pGetBinRes(pSwitchElement->uRes.Id);
        }

        SND_fn_vConvertResDiskToMemSwitch(_pBRDisk,_pBRMem,NULL);
        return TRUE;
}

/* 0x0045A9A0 */
SndBool SND_fn_bLoadResBinarySwitchOld(tdstBlockResourceDisk *_pBRDisk,tdstBlockResourceMem *_pBRMem,char *pDataBloc)
{
	tdstBlockResourceDisk stDisk;

	memcpy(&stDisk,_pBRDisk,sizeof(stDisk));

	stDisk.eType=TYPE_SWITCH;

    stDisk.uRes.stSwitch.ulNbElements=_pBRDisk->uRes.stSwitchOld.ulNbElements;
    stDisk.uRes.stSwitch.lTypeSwitch=_pBRDisk->uRes.stSwitchOld.lTypeSwitch;
    stDisk.uRes.stSwitch.hDefault=_pBRDisk->uRes.stSwitchOld.hDefault;
	stDisk.uRes.stSwitch.bDynamic=_pBRDisk->uRes.stSwitchOld.bDynamic;

	stDisk.uRes.stSwitch.bMaintainIfSwitched=FALSE;

	return SND_fn_bLoadResBinarySwitch(&stDisk,_pBRMem,pDataBloc);
}

/* 0x0045AA00 */
tduRefRes SND_fn_tduGetLowestResourceSnd(tduRefRes uHighRes, struct SoundParam_ *par, SndBool eval);

tduRefRes SND_fn_tduGetLowestResourceSwitch(tduRefRes uHighRes, struct SoundParam_ *par, SndBool eval)
{
	return SND_fn_tduGetLowestResourceSnd(SND_fn_tdGetEffectiveResourceSwitch(uHighRes,par,(unsigned char *)&eval),par,eval);
}
