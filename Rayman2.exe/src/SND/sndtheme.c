#include <stddef.h>
#include <string.h>
#include "SND/sndres.h"
#include "SND/sndparam_pc.h"

#define SNDLST2_M_StaticGetPrevElement(_hElement)	((_hElement)-1)

typedef struct active_theme_ active_theme;
struct active_theme_{
	SndBool		bActive;
	SndBool		bTransAsked;
	long		lReserved_08;
	SndBool		bTransiting;
	SndBool		pc_bOutro;

	tduRefRes	uCurrentThemeRes;

	hThemePart	hLoopingPart;
	long		lLoopingPartIndice;
	long		lNbLoopLeft;

	long		pc_lNbThemeLoops;
	hThemePart	hCurrentPart;
	long		lCurrentPartIndice;
	long		lVoice;
	hThemePart	hNextPart;
	long		lNextPartIndice;
	tduRefRes	uNextRes;
	tduRefRes	uAskedThemeRes;
	tduRefRes	uGoingToThemeRes;
	tdstThemePart	*pTransitingPart;
	void		*pfnFctCallback;
	long		lParCallback;
	SndBool		*pbValid;

	struct SoundParam_	*pstParam;
	long		lSizeParam;
};

static SndBool		g_bWorkingTheme=FALSE;

#define E_ucSndBlockMain ((unsigned char)0)
void *SND_CALL SND_fn_pvMallocSndEx(unsigned char id_block, unsigned int ulNbBytes);
void *SND_CALL SND_fn_pvReAllocSndEx(unsigned char id_block, void *pvPtr, unsigned int ulNbBytes);
void SND_CALL SND_fn_vFreeSndEx(unsigned char id_block, void *pvPtr);

static active_theme	g_stTheme;

tduRefRes SND_fn_tduGetLowestResourceSnd(tduRefRes uHighRes, struct SoundParam_ *par, SndBool eval);
long SND_fn_lPlayTransitionSnd(tduRefRes FirstRes, tduRefRes NextRes, SND_td_pfn_vSoundCallback transition_callback, long param_callback, struct SoundParam_ *par);
SndBool SND_fn_bSetNextTransitionSnd(long voice, tduRefRes new_res);
SndBool SND_fn_bDoTransitionWithFadeSnd(long voice, tduRefRes new_res);
SndBool SND_fn_bDoTransitionWithFadeSnd2(long voice, tduRefRes new_res, SndReal rDuration);
void raj_soundparam_theme(active_theme* theme, struct SoundParam_* par);
extern long SND_pc_g_lSoundFixedValue;		/* 0x004B74A4 */
extern unsigned long SND_pc_g_ul4A2438;		/* 0x004A2438 */

void *SND_CALL SND_fn_pGetBinRes(unsigned long _ulBinId);

SndBool SND_fn_bSetParamTransitionSnd(long voice, struct SoundParam_ *par);	/* 0x004582C0 */

void __cdecl SND_fn_vStopTransitionSnd(long voice);	/* 0x00458410 */
extern tduRefRes SND_RES_FANTOME;

void __cdecl SND_fn_vPauseTransitionSnd(long voice);	/* 0x004584A0 */
void __cdecl SND_fn_vResumeTransitionSnd(long voice);	/* 0x00458470 */

/* 0x00459460 */
SndBool	raj_next_part(active_theme *theme)
{
    tduRefRes           uRes;
    tdstBlockResourceMem *pstBlock;
	hThemePart		hPart;
	SndBool			bThemeLoopsDone=FALSE;
	long			lIndicePart;

	if (theme->bTransiting)
	{
		uRes=theme->uGoingToThemeRes;
		theme->bTransiting=FALSE;
		if (uRes.pstPtr==NULL)
		{
			theme->bTransAsked=TRUE;
			return FALSE;
		}
		theme->uCurrentThemeRes=uRes;
		if (theme->pc_bOutro)
		{
			theme->pc_bOutro=FALSE;
			theme->lNextPartIndice=0;
			theme->hNextPart=SNDLST2_M_StaticGetFirstElement(&(uRes.pstPtr->uRes.stTheme));
			theme->uNextRes=theme->hNextPart->uRes;
			return TRUE;
		}
	}

    pstBlock=theme->uCurrentThemeRes.pstPtr;

	if ((theme->hCurrentPart->bLoopEnd) && (theme->lNbLoopLeft))
	{
		theme->lNbLoopLeft--;

		if (theme->hLoopingPart)
		{
			theme->hNextPart=theme->hLoopingPart;
			theme->lNextPartIndice=theme->lLoopingPartIndice;
		}
		else
		{
			hPart=SNDLST2_M_StaticGetFirstElement(&(pstBlock->uRes.stTheme));
			theme->lNextPartIndice=0;
			theme->hNextPart=hPart;
		}
	}
	else
	{
		if (!theme->lNbLoopLeft)
			theme->hLoopingPart=NULL;

		theme->lNextPartIndice++;

		if ((unsigned long)theme->lNextPartIndice>=pstBlock->uRes.stTheme.ulNbParts)
		{
			if ((pstBlock->uRes.stTheme.ulNbLoops!=0)
			    && ((unsigned long)++theme->pc_lNbThemeLoops>=pstBlock->uRes.stTheme.ulNbLoops))
				bThemeLoopsDone=TRUE;
			if (bThemeLoopsDone)
			{
				if (pstBlock->uRes.stTheme.pstPartOutro!=NULL)
				{
					theme->pc_bOutro=TRUE;
					theme->bTransiting=TRUE;
					theme->uNextRes=pstBlock->uRes.stTheme.pstPartOutro->uRes;
					theme->uGoingToThemeRes=SND_RES_FANTOME;
				}
				else
				{
					theme->bTransAsked=TRUE;
					return FALSE;
				}
			}
			else
			{
				lIndicePart=pstBlock->uRes.stTheme.ulStartLoop;
				theme->lNextPartIndice=lIndicePart;
				theme->hNextPart=SNDLST2_M_StaticGetFirstElement(&(pstBlock->uRes.stTheme))+lIndicePart;
				theme->uNextRes=theme->hNextPart->uRes;

				hPart=theme->hNextPart;
				lIndicePart=pstBlock->uRes.stTheme.ulStartLoop;
				while (hPart)
				{
					if (hPart->bLoopStart)
					{
						theme->hLoopingPart=hPart;
						theme->lLoopingPartIndice=lIndicePart;
						theme->lNbLoopLeft=theme->hLoopingPart->lNbLoops;
						break;
					}
					hPart=SNDLST2_M_StaticGetPrevElement(hPart);
					lIndicePart--;
					if (lIndicePart<0)
						break;
					if (hPart && hPart->bLoopEnd)
						break;
				}
			}
			goto check_loop_start;
		}
		theme->hNextPart=SNDLST2_M_StaticGetNextElement(theme->hNextPart);
	}
	theme->uNextRes=theme->hNextPart->uRes;

check_loop_start:

	if (theme->hNextPart->bLoopStart && !theme->hLoopingPart)
	{
		theme->hLoopingPart=theme->hNextPart;
		theme->lLoopingPartIndice=theme->lNextPartIndice;
		theme->lNbLoopLeft=theme->hLoopingPart->lNbLoops;
	}

	return TRUE;
}

/* 0x004595D0 */
SndBool change_theme(active_theme *theme,tduRefRes new_theme)
{
	long		eTransition;
	tduRefRes	*pNextRes;
	SndBool		bRet=TRUE;

	if (theme->bTransAsked)
		eTransition=TRANSITION_ATENDOFPART;
	else
		eTransition=theme->hCurrentPart->eTransition;

	theme->pc_bOutro=FALSE;
	theme->bTransiting=TRUE;
	theme->bTransAsked=FALSE;
	theme->uGoingToThemeRes=new_theme;

	if (!new_theme.pstPtr->uRes.stTheme.ulNbParts)
	{
		theme->hNextPart=NULL;
		theme->lNextPartIndice=-1;
		theme->uNextRes=SND_RES_FANTOME;
		return FALSE;
	}

	theme->hLoopingPart=NULL;
	theme->lLoopingPartIndice=-1;
	theme->lNbLoopLeft=-1;
	theme->pc_lNbThemeLoops=0;

	theme->lNextPartIndice=0;
	theme->hNextPart=SNDLST2_M_StaticGetFirstElement(&(new_theme.pstPtr->uRes.stTheme));
	pNextRes=&theme->hNextPart->uRes;
	if ((eTransition==TRANSITION_OUTRO)
	    && (theme->uCurrentThemeRes.pstPtr->uRes.stTheme.pstPartOutro!=NULL))
	{
		theme->pc_bOutro=TRUE;
		pNextRes=&theme->uCurrentThemeRes.pstPtr->uRes.stTheme.pstPartOutro->uRes;
	}
	theme->uNextRes=*pNextRes;

	if (theme->hNextPart->bLoopStart)
	{
		theme->hLoopingPart=theme->hNextPart;
		theme->lLoopingPartIndice=theme->lNextPartIndice;
		theme->lNbLoopLeft=theme->hLoopingPart->lNbLoops;
	}
	if (eTransition!=TRANSITION_CROSSFADE)
		bRet=FALSE;
	return bRet;
}

/* 0x00459670 */
void start_new_theme(active_theme *theme,tduRefRes theme_res)
{
    tdstTypeThemeMem     *pstThemeRes;
    hThemePart           hFirst;

    theme->uCurrentThemeRes=theme_res;
    theme->bTransiting=FALSE;
    theme->lReserved_08=0;
    theme->pc_bOutro=FALSE;
    theme->bTransAsked=FALSE;

	pstThemeRes=&(theme_res.pstPtr->uRes.stTheme);
	if (!pstThemeRes->ulNbParts)
	{
		theme->hCurrentPart=NULL;
        g_bWorkingTheme=FALSE;
        return ;
	}

	theme->hLoopingPart=NULL;
	theme->lLoopingPartIndice=-1;
	theme->lNbLoopLeft=-1;

	theme->pc_lNbThemeLoops=0;
	theme->lCurrentPartIndice=0;
	hFirst=SNDLST2_M_StaticGetFirstElement(pstThemeRes);
	theme->lNextPartIndice=0;
	theme->hCurrentPart=hFirst;
	theme->hNextPart=hFirst;
	theme->uNextRes=hFirst->uRes;

	if (hFirst->bLoopStart)
	{
		theme->hLoopingPart=hFirst;
		theme->lLoopingPartIndice=0;
		theme->lNbLoopLeft=hFirst->lNbLoops;
	}

    raj_next_part(theme);
}

static void SND_CALL fn_Transition(long par)
{
	active_theme *theme=(active_theme*)par;
	tduRefRes	NextRes;

	/* 0x004596E0 */
	if (g_bWorkingTheme || !theme->bActive || theme->bTransAsked)
		return;

	theme->lCurrentPartIndice=theme->lNextPartIndice;
	theme->hCurrentPart=theme->hNextPart;

	if (!raj_next_part(theme))
	{

		if (theme->pbValid!=NULL)
			theme->pbValid=FALSE;
		if (theme->pfnFctCallback!=NULL)
			(*((SND_td_pfn_vSoundCallback)theme->pfnFctCallback))(theme->lParCallback);
	}
	else
	{
		NextRes=SND_fn_tduGetLowestResourceSnd(theme->uNextRes,theme->pstParam,TRUE);
		SND_fn_bSetNextTransitionSnd(theme->lVoice,NextRes);
	}
}

long __cdecl SND_fn_bGetThemePosition(unsigned long *pulOut1,unsigned long *pulOut2,unsigned long *pulOut3)
{
	if (g_stTheme.bActive == 0)
	{
		return 0;
	}
	*pulOut1 = (unsigned long)g_stTheme.uCurrentThemeRes.pstPtr;
	*pulOut2 = (unsigned long)g_stTheme.lCurrentPartIndice;
	*pulOut3 = (unsigned long)g_stTheme.lNbLoopLeft;
	return 1;
}

long __cdecl SND_fn_bGetAskedTheme(unsigned long *pulOut1,unsigned long *pulOut2)
{
	if (g_stTheme.bActive == 0 || g_stTheme.lReserved_08 == 0)
	{
		return 0;
	}
	*pulOut1 = (unsigned long)g_stTheme.uAskedThemeRes.pstPtr;
	*pulOut2 = (unsigned long)g_stTheme.bTransiting;
	return 1;
}

void SND_fn_vInitTheme(void)
{
	active_theme *theme=&g_stTheme;

	theme->pstParam=NULL;
	theme->lSizeParam=0;
	theme->bActive=FALSE;
}

/* 0x00459800 */
static void SND_fn_vConvertResDiskToMemTheme(tdstBlockResourceDisk* disk
                                      ,tdstBlockResourceMem *mem
                                      ,void* ptrBegin)
{
    mem->Id=disk->Id;
    mem->eType=disk->eType;
    mem->eStorage=disk->eStorage;
    mem->ucVolume=disk->ucVolume;
    mem->bIsLoaded=TRUE;

    mem->uRes.stTheme.ulStartLoop = disk->uRes.stTheme.ulStartLoop;
    mem->uRes.stTheme.ulNbParts= disk->uRes.stTheme.ulNbParts;
    mem->uRes.stTheme.ulNbLoops = disk->uRes.stTheme.ulNbLoops;
    mem->uRes.stTheme.rFadeDuration = disk->uRes.stTheme.rFadeDuration;
}

/* 0x00459840 */
SndBool SND_fn_bLoadResBinaryTheme(tdstBlockResourceDisk *_pBRDisk,tdstBlockResourceMem *_pBRMem,char *pDataBloc)
{

    tdstThemePart *pPartArray,*pPart;

    int iPos;

    pPartArray=(tdstThemePart *)(pDataBloc + _pBRDisk->ulDataOffset);

    SNDLST2_M_StaticSetFirstElement(&_pBRMem->uRes.stTheme,pPartArray);
    SNDLST2_M_StaticSetNumberOfElements(&_pBRMem->uRes.stTheme, _pBRDisk->uRes.stTheme.ulNbParts);

    SNDLST2_M_StaticForEachElementOf( &_pBRMem->uRes.stTheme, pPart, iPos)
    {
        pPart->uRes.pstPtr=SND_fn_pGetBinRes(pPart->uRes.Id);
    }

    if (_pBRDisk->uRes.stTheme.lOffsetPartOutro != -1)
    {
        _pBRMem->uRes.stTheme.pstPartOutro=(tdstThemePartOutro *)((char *)pPartArray + _pBRDisk->uRes.stTheme.lOffsetPartOutro);
        _pBRMem->uRes.stTheme.pstPartOutro->uRes.pstPtr=SND_fn_pGetBinRes(_pBRMem->uRes.stTheme.pstPartOutro->uRes.Id);
    }
    else
        _pBRMem->uRes.stTheme.pstPartOutro=NULL;

    SND_fn_vConvertResDiskToMemTheme(_pBRDisk,_pBRMem,NULL);
    return TRUE;
}

void raj_soundparam_theme(active_theme* theme,struct SoundParam_* par)
{
	int size;

	if (par)
		size=SND_M_lGetSizeOfSoundParam(par);
	else
		size=0;

	if (size)
	{
		if (size!=theme->lSizeParam)
		{
			if (theme->pstParam)
				theme->pstParam=SND_fn_pvReAllocSndEx(E_ucSndBlockMain,theme->pstParam,size);
			else
				theme->pstParam=SND_fn_pvMallocSndEx(E_ucSndBlockMain,size);
		}
		theme->lSizeParam=size;
		memcpy(theme->pstParam,par,size);
	}
	else
	{
		if (theme->pstParam)
		{
			SND_fn_vFreeSndEx(E_ucSndBlockMain,theme->pstParam);
			theme->pstParam=NULL;
			theme->lSizeParam=0;
		}
	}
}

/* 0x00459950 */
long SND_fn_lPlayTheme(long canal,tduRefRes theme_res,struct SoundParam_* par,long prio,SND_td_pfn_vSoundCallback fn_callback,long par_callback,SndBool* valid)
{
	struct SoundParam_	*pstSoundPar;
	active_theme		*theme=&g_stTheme;
	tduRefRes		FirstRes,NextRes;
	struct SoundParam_	stSoundPar;

	raj_soundparam_theme(theme,par);

	if ((theme->bActive) && (theme_res.pstPtr==theme->uCurrentThemeRes.pstPtr) && !theme->bTransiting)
	{
		goto L_return_theme;	/* 0x00459B3A */
	}
	if (theme->bTransiting && (theme->uGoingToThemeRes.pstPtr!=NULL))
		goto L_return_theme;

	g_bWorkingTheme=TRUE;

	pstSoundPar=&stSoundPar;
	memcpy(pstSoundPar,par,SND_M_lGetSizeOfSoundParam(par));

	if (theme->bActive)
	{/* 0x004599CD */
		if (change_theme(theme,theme_res))
		{
			NextRes=SND_fn_tduGetLowestResourceSnd(theme->uNextRes,theme->pstParam,TRUE);
			if (!SND_pc_g_ul4A2438)
			{
				SND_pc_g_ul4A2438=1;
				SND_fn_bDoTransitionWithFadeSnd2(theme->lVoice,NextRes,SND_pc_g_lSoundFixedValue);
			}
			else
				SND_fn_bDoTransitionWithFadeSnd(theme->lVoice,NextRes);
		}
		else
		{
			NextRes=SND_fn_tduGetLowestResourceSnd(theme->uNextRes,theme->pstParam,TRUE);
			SND_fn_bSetNextTransitionSnd(theme->lVoice,NextRes);
		}
	}
	else
	{
		start_new_theme(theme,theme_res);

		theme->pfnFctCallback=(void *)fn_callback;
		theme->pbValid=valid;
		theme->lParCallback=par_callback;

		pstSoundPar->ucVol=(unsigned char)(((long)pstSoundPar->ucVol*theme_res.pstPtr->ucVolume)>>7);
		pstSoundPar->iFlags|=C_SOUNDPARAM_COEF_VOL;

		FirstRes=SND_fn_tduGetLowestResourceSnd(theme->hCurrentPart->uRes,pstSoundPar,TRUE);
		NextRes=SND_fn_tduGetLowestResourceSnd(theme->uNextRes,pstSoundPar,TRUE);

		theme->lVoice=SND_fn_lPlayTransitionSnd(FirstRes,NextRes,fn_Transition,(long)theme,pstSoundPar);
	}

	if (!theme->bActive)
	{
		if (theme->lVoice!=C_PLAY_FAILED)
		{
			theme->bActive=TRUE;
			*valid=TRUE;
		}
		else
			*valid=FALSE;
	}

	g_bWorkingTheme=FALSE;

	if (theme->bActive)
	{
L_return_theme:
		return (long)theme;
	}
	return C_PLAY_FAILED;
}

SndBool SND_CALL SND_fn_bSetParamTheme(long canal,struct SoundParam_* par)
{
	struct SoundParam_ *pstSoundPar;
	active_theme *pTheme;
	struct SoundParam_ stSoundPar;

	pTheme=(active_theme*)canal;

	raj_soundparam_theme(pTheme,par);

	pstSoundPar=&stSoundPar;
	memcpy(pstSoundPar,par,SND_M_lGetSizeOfSoundParam(par));

	pstSoundPar->ucVol=(unsigned char)(((long)pstSoundPar->ucVol*pTheme->uCurrentThemeRes.pstPtr->ucVolume)>>7);

	SND_fn_bSetParamTransitionSnd(pTheme->lVoice,pstSoundPar);

	return pTheme->bActive;
}

/* 0x00459BC0 */
SndBool SND_CALL SND_fn_bTestIsPlayingTheme(long canal)
{
	active_theme *pTheme;

	pTheme=(active_theme*)canal;

    return pTheme->bActive;
}

SndBool SND_CALL SND_fn_bAnyPlayingTheme(void)
{
	return g_stTheme.bActive;
}

void SND_CALL SND_fn_vStopTheme(long canal)
{
	active_theme *pTheme;

	pTheme=(active_theme*)canal;

	g_bWorkingTheme=TRUE;

	if (pTheme->bActive)
		SND_fn_vStopTransitionSnd(pTheme->lVoice);

	if (pTheme->pfnFctCallback)
		(*((SND_td_pfn_vSoundCallback)pTheme->pfnFctCallback))(pTheme->lParCallback);

	pTheme->bActive=FALSE;
	pTheme->bTransiting=FALSE;
	pTheme->uGoingToThemeRes=SND_RES_FANTOME;

	g_bWorkingTheme=FALSE;
}

/* 0x00459C40 */
void SND_CALL SND_fn_vPauseTheme(long canal)
{
	active_theme *pTheme;

	pTheme=(active_theme*)canal;

    if (pTheme->bActive)
         SND_fn_vPauseTransitionSnd(pTheme->lVoice);
}

void SND_CALL SND_fn_vResumeTheme(long canal)
{
	active_theme *pTheme;

	pTheme=(active_theme*)canal;

    if (pTheme->bActive)
        SND_fn_vResumeTransitionSnd(pTheme->lVoice);
}

#define SND_C_POS_UNKNOWN -1

/* 0x00459C80 */
SndReal SND_CALL SND_fn_rGetPosTheme(long voie)
{
        return SND_C_POS_UNKNOWN;
}
