#include "SND/sndres.h"
#include "SND/sndparam_pc.h"
#include <string.h>

#ifndef TRUE
#define TRUE 1
#endif
#ifndef FALSE
#define FALSE 0
#endif

#define C_NEVER_CHOOSEN -2
#define C_NOTHING_CHOOSEN -1

long __cdecl SND_fn_rGetRandomSnd(long max);
#define SND_RT_fn_rGetRandomSnd SND_fn_rGetRandomSnd

static tduRefRes SND_fn_tdGetEffectiveResourceRandom(tduRefRes res_random)
{
	tdstRandomElement *pRandomElement;
	tdstTypeRandomMem *pRandomList;
    tduRefRes       uRes;
    int             i,lLastElementChoosen=C_NOTHING_CHOOSEN,lPreviousElementChoosen,lPreviousResChoosen;
    SndBool         bFound;
	SndReal			rOffset;

	uRes = res_random;
	pRandomList = &(uRes.pstPtr->uRes.stRandom);

	lPreviousResChoosen = C_NEVER_CHOOSEN;
	lPreviousElementChoosen = C_NEVER_CHOOSEN;
	if (pRandomList->lLastResChoosen >= 0)
	{
		SNDLST2_M_StaticGetElementNumber(pRandomList, pRandomElement, pRandomList->lLastResChoosen,i);
		if (!pRandomElement->bCanBeChosenTwice)
			lPreviousResChoosen = pRandomList->lLastResChoosen;
	}
	if (pRandomList->lLastElementChoosen >= 0)
	{
		SNDLST2_M_StaticGetElementNumber(pRandomList, pRandomElement, pRandomList->lLastElementChoosen,i);
		if (!pRandomElement->bCanBeChosenTwice)
			lPreviousElementChoosen = pRandomList->lLastElementChoosen;
	}
	else if ((pRandomList->lLastElementChoosen == C_NOTHING_CHOOSEN) &&
			 (!pRandomList->bNothingCanBeChosenTwice))
		lPreviousElementChoosen = C_NOTHING_CHOOSEN;

	rOffset = SND_RT_fn_rGetRandomSnd(C_SNDREAL_1);

	bFound = FALSE;
	if ((rOffset >= pRandomList->rProbNothing) || (lPreviousElementChoosen == C_NOTHING_CHOOSEN))
	{
		rOffset -= pRandomList->rProbNothing;

		SNDLST2_M_StaticForEachElementOf(pRandomList, pRandomElement, i)
		{
			if ((rOffset <= pRandomElement->rProb) && (lPreviousResChoosen != i))
			{
				lLastElementChoosen = i;
				uRes = pRandomElement->uRes;
				bFound = TRUE;
				break;
			}
			rOffset -= pRandomElement->rProb;
		}
		if (!bFound)
		{
			if (lPreviousElementChoosen != 0)
			{
				lLastElementChoosen = 0;
				uRes = SNDLST2_M_StaticGetFirstElement(pRandomList)->uRes;
			}
			else
			{
				lLastElementChoosen = C_NOTHING_CHOOSEN;
				uRes.pstPtr = SND_C_RES_FANTOME_PTR;
			}
			bFound=TRUE;
		}
	}

	if (!bFound)
	{
		lLastElementChoosen = C_NOTHING_CHOOSEN;
		uRes.pstPtr = SND_C_RES_FANTOME_PTR;
	}

	pRandomList->lLastElementChoosen = lLastElementChoosen;
	if (lLastElementChoosen != C_NOTHING_CHOOSEN)
		pRandomList->lLastResChoosen = lLastElementChoosen;

    return uRes;
}

static tduRefRes SND_fn_tdGetResourceChoosenRandom(tduRefRes res_random)
{
	tdstRandomElement *pRandomElement;
	int i;

	if ((res_random.pstPtr->uRes.stRandom.lLastElementChoosen == C_NOTHING_CHOOSEN)
		|| (res_random.pstPtr->uRes.stRandom.lLastElementChoosen >= SNDLST2_M_StaticGetNumberOfElements(&res_random.pstPtr->uRes.stRandom)))

		return SND_RES_FANTOME;
	else
	{
		SNDLST2_M_StaticGetElementNumber(&res_random.pstPtr->uRes.stRandom,
			pRandomElement, res_random.pstPtr->uRes.stRandom.lLastElementChoosen, i);

		return pRandomElement->uRes;
	}
}

/* 0x0045A4A0 */
tduRefRes SND_fn_tduGetLowestResourceSnd(tduRefRes uHighRes, struct SoundParam_ *par, SndBool eval);

#define E_ucSndBlockMain		((unsigned char)0)

typedef struct random_voice_ random_voice;
struct random_voice_{
	long	lLowVoice;
	SndBool	bValid;
	tduRefRes	res;
};

void *SND_CALL SND_fn_pvMallocSndEx(unsigned char id_block, unsigned int ulNbBytes);
void SND_CALL SND_fn_vFreeSndEx(unsigned char id_block, void *pvPtr);

long SND_fn_lPlaySnd(tduRefRes res, struct SoundParam_ *par, long prio, void *fn_callback, long par_callback, SndBool *valid);

long SND_fn_lPlayRandom(tduRefRes num_res,struct SoundParam_ *par,long prio,void *fn_callback,long par_callback)
{
	tduRefRes	low_res;
	random_voice *pstRandomVoice;
	SoundParam	stSoundParam;

	pstRandomVoice=(random_voice *)SND_fn_pvMallocSndEx(E_ucSndBlockMain,sizeof(random_voice));
	pstRandomVoice->bValid=TRUE;
	pstRandomVoice->res=num_res;

	low_res=SND_fn_tdGetEffectiveResourceRandom(num_res);

	if (low_res.pstPtr!=SND_C_RES_FANTOME_PTR)
	{

		memcpy(&stSoundParam,par,SND_M_lGetSizeOfSoundParam(par));
		stSoundParam.ucVol=(unsigned char)(((int)stSoundParam.ucVol*(int)pstRandomVoice->res.pstPtr->ucVolume)>>7);
		stSoundParam.iFlags|=C_SOUNDPARAM_COEF_VOL;

		pstRandomVoice->lLowVoice=SND_fn_lPlaySnd(low_res,&stSoundParam,prio,fn_callback,par_callback,&pstRandomVoice->bValid);

		if (pstRandomVoice->lLowVoice!=C_PLAY_FAILED)
			return (long)pstRandomVoice;
	}

	SND_fn_vFreeSndEx(E_ucSndBlockMain,pstRandomVoice);

	return C_PLAY_FAILED;
}

SndBool SND_CALL SND_fn_bSetParamSnd(long voie, struct SoundParam_ *par);

SndBool SND_CALL SND_fn_bSetParamRandom(long voice,struct SoundParam_ *par)
{
	random_voice *pstRandomVoice = (random_voice *)voice;
	SoundParam	stSoundParam;

	memcpy(&stSoundParam,par,SND_M_lGetSizeOfSoundParam(par));
	stSoundParam.ucVol=(unsigned char)(((int)pstRandomVoice->res.pstPtr->ucVolume*(int)stSoundParam.ucVol)>>7);
	stSoundParam.iFlags|=C_SOUNDPARAM_COEF_VOL;

	if (pstRandomVoice->bValid && SND_fn_bSetParamSnd(pstRandomVoice->lLowVoice,&stSoundParam))
		return TRUE;
	else
	{
		SND_fn_vFreeSndEx(E_ucSndBlockMain,pstRandomVoice);

		return FALSE;
	}
}

SndBool __cdecl SND_fn_bTestIsPlayingSnd(long voie);
void SND_CALL SND_fn_vStopSnd(long voie);
void SND_CALL SND_fn_vPauseSnd(long voie);
void SND_CALL SND_fn_vResumeSnd(long voie);
SndReal SND_CALL SND_fn_rGetPosSnd(long voie);
void *SND_CALL SND_fn_pGetBinRes(unsigned long _ulBinId);

#define SND_C_POS_UNKNOWN -1

/* 0x0045A330 */
SndBool SND_CALL SND_fn_bTestIsPlayingRandom(long voice)
{
	random_voice *pstRandomVoice = (random_voice *)voice;

	if (pstRandomVoice->bValid && SND_fn_bTestIsPlayingSnd(pstRandomVoice->lLowVoice))
		return TRUE;
	else
	{
		SND_fn_vFreeSndEx(E_ucSndBlockMain, pstRandomVoice);

		return FALSE;
	}
}

/* 0x0045A370 */
void SND_CALL SND_fn_vStopRandom(long voice)
{
	random_voice *pstRandomVoice = (random_voice *)voice;

	if (pstRandomVoice->bValid)
		SND_fn_vStopSnd(pstRandomVoice->lLowVoice);

	SND_fn_vFreeSndEx(E_ucSndBlockMain, pstRandomVoice);
}

/* 0x0045A390 */
void SND_CALL SND_fn_vPauseRandom(long voice)
{
	random_voice *pstRandomVoice = (random_voice *)voice;

	if (pstRandomVoice->bValid)
		SND_fn_vPauseSnd(pstRandomVoice->lLowVoice);
}

/* 0x0045A3B0 */
void SND_CALL SND_fn_vResumeRandom(long voice)
{
	random_voice *pstRandomVoice = (random_voice *)voice;

	if (pstRandomVoice->bValid)
		SND_fn_vResumeSnd(pstRandomVoice->lLowVoice);
}

/* 0x0045A3D0 */
SndReal SND_CALL SND_fn_rGetPosRandom(long voice)
{
	random_voice *pstRandomVoice = (random_voice *)voice;

	if (pstRandomVoice->bValid)
		return SND_fn_rGetPosSnd(pstRandomVoice->lLowVoice);
	else
		return SND_C_POS_UNKNOWN;
}

/* 0x0045A3F0 */
static void SND_fn_vConvertResDiskToMemRandom(tdstBlockResourceDisk* disk,tdstBlockResourceMem* mem,void* ptrBegin)
{
    mem->Id = disk->Id;
    mem->eType = disk->eType;
    mem->eStorage = disk->eStorage;
    mem->ucVolume = disk->ucVolume;
    mem->bIsLoaded = TRUE;

    mem->uRes.stRandom.ulNbElements = disk->uRes.stRandom.ulNbElements;
    mem->uRes.stRandom.rProbNothing = disk->uRes.stRandom.rProbNothing;
    mem->uRes.stRandom.bNothingCanBeChosenTwice = disk->uRes.stRandom.bNothingCanBeChosenTwice;

	mem->uRes.stRandom.lLastElementChoosen = C_NEVER_CHOOSEN;
	mem->uRes.stRandom.lLastResChoosen = C_NEVER_CHOOSEN;
}

/* 0x0045A440 */
SndBool SND_fn_bLoadResBinaryRandom(tdstBlockResourceDisk *_pBRDisk,tdstBlockResourceMem *_pBRMem,char *pDataBloc)
{
	tdstRandomElement *pRandomArray, *pRandomElement;
	int iPos;

	pRandomArray = (tdstRandomElement *)(pDataBloc + _pBRDisk->ulDataOffset);

	SNDLST2_M_StaticSetMember(&_pBRMem->uRes.stRandom, hFirstElement, pRandomArray);
	SNDLST2_M_StaticSetMember(&_pBRMem->uRes.stRandom, lNumberOfElements, _pBRDisk->uRes.stRandom.ulNbElements);

	SNDLST2_M_StaticForEachElementOf( &_pBRMem->uRes.stRandom, pRandomElement, iPos)
	{
	    pRandomElement->uRes.pstPtr = SND_fn_pGetBinRes(pRandomElement->uRes.Id);
	}

	SND_fn_vConvertResDiskToMemRandom(_pBRDisk, _pBRMem, NULL);
	return TRUE;
}

tduRefRes SND_fn_tduGetLowestResourceRandom(tduRefRes uHighRes, struct SoundParam_ *par, SndBool eval)
{
	tduRefRes uResRandom;

	if (eval)
		uResRandom = SND_fn_tdGetEffectiveResourceRandom(uHighRes);
	else
		uResRandom = SND_fn_tdGetResourceChoosenRandom(uHighRes);

	return SND_fn_tduGetLowestResourceSnd(uResRandom,par,eval);
}
