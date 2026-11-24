#include "SNDinc.h"

#ifndef NO_ACP_SOUND

#if defined(_DLL_COMPILATION_MODE)
#include "snddll.h"
#endif

#include "sndres.h"
#define DLLWAV_MODULE
#include "sndxd.h"
#include "snd_pc.h"

typedef struct _SND_pc_tdstVirtualVoice *SND_pc_p_tdstVirtualVoice;
typedef struct _SND_pc_tdstVirtualReal *SND_pc_p_tdstVirtualReal;
typedef struct _SND_pc_tdstVirtualWait *SND_pc_p_tdstVirtualWait;

SNDLST2_M_DynamicUseListOf(SND_pc_p_tdstVirtualVoice);
SNDLST2_M_DynamicUseListOf(SND_pc_p_tdstVirtualReal);
SNDLST2_M_DynamicUseListOf(SND_pc_p_tdstVirtualWait);

typedef struct _SND_pc_tdstVirtualReal {
	SND_pc_p_tdstVirtualVoice		pVoice;
	long							l04;
	SNDLST2_M_DynamicElementDeclaration(SND_pc_p_tdstVirtualReal)
} SND_pc_tdstVirtualReal;

typedef struct _SND_pc_tdstVirtualWait {
	SND_pc_p_tdstVirtualVoice		pVoice;
	SNDLST2_M_DynamicElementDeclaration(SND_pc_p_tdstVirtualWait)
} SND_pc_tdstVirtualWait;

typedef struct _SND_pc_tdstVirtualVoice {
	long							hVoice;
	SndReal							rNorme;
	long							lPrio;
	long							lSeq;
	SND_td_pfn_vSoundCallback		fn_callback;
	long							par_callback;
	tduRefRes						res;
	long							lParSize;
	SoundParam						*pPar;
	long							bPlaying;
	long							bLinked;
	long							bPaused;
	SND_pc_tdstVirtualReal			*pReal;
	SND_pc_tdstVirtualWait			*pWait;
	SNDLST2_M_DynamicElementDeclaration(SND_pc_p_tdstVirtualVoice)
} SND_pc_tdstVirtualVoice;

typedef struct _SND_pc_tdstVirtualModuleList {
	struct _virtual_module			*pHead;
	struct _virtual_module			*pTail;
	long							lCount;
	long							bInit;
} SND_pc_tdstVirtualModuleList;

struct _virtual_module {
	SNDLST2_M_DynamicAnchorDeclaration(SND_pc_p_tdstVirtualVoice) stVoices;
	SNDLST2_M_DynamicAnchorDeclaration(SND_pc_p_tdstVirtualReal) stReals;
	SNDLST2_M_DynamicAnchorDeclaration(SND_pc_p_tdstVirtualWait) stWaits;
	long							lSeq;
	long							lNbVoices;
	SND_pc_tdstVirtualVoiceFunctions	fn;
	struct _virtual_module			*pNextModule;
	struct _virtual_module			*pPrevModule;
	SND_pc_tdstVirtualModuleList	*pList;
};

SND_pc_tdstVirtualModuleList SND_pc_ListVirtualModule = {0};

void FUN_10007050(SND_tdhVirtualModule hModule)
{
	SND_pc_tdstVirtualVoice *pVoice;
	SND_pc_tdstVirtualWait *pWait;
	SND_pc_tdstVirtualReal *pReal;

	for (pVoice = hModule->stVoices.hFirstElementDyn; pVoice; pVoice = pVoice->hNextBrotherDyn)
		for (pWait = hModule->stWaits.hFirstElementDyn; pWait; pWait = pWait->hNextBrotherDyn)
			;
	for (pReal = hModule->stReals.hFirstElementDyn; pReal; pReal = pReal->hNextBrotherDyn)
		;
}

void FUN_10007090(SND_pc_tdstVirtualVoice *pVoice,SoundParam *par)
{
	long lSize;

	lSize = par ? par->iNbLinks * 8 + 0x38 : 0;
	if (lSize)
	{
		if (lSize != pVoice->lParSize)
			pVoice->pPar = (SoundParam *)(pVoice->pPar ? SND_fn_pvReAllocSndEx(0,pVoice->pPar,lSize) : SND_fn_pvMallocSndEx(0,lSize));
		pVoice->lParSize = lSize;
		memcpy(pVoice->pPar,par,lSize);
	}
	else if (pVoice->pPar)
	{
		SND_fn_vFreeSndEx(0,pVoice->pPar);
		pVoice->pPar = NULL;
		pVoice->lParSize = 0;
	}
}

SND_pc_tdstVirtualWait *FUN_10007120(SND_tdhVirtualModule hModule,SND_pc_tdstVirtualVoice *pVoice)
{
	SND_pc_tdstVirtualWait *pWait;

	pWait = (SND_pc_tdstVirtualWait *)SND_fn_pvMallocSndEx(0,sizeof(SND_pc_tdstVirtualWait));
	SNDLST2_M_DynamicInitElement(pWait);
	pWait->pVoice = pVoice;
	SNDLST2_M_DynamicAddTail(&hModule->stWaits,pWait);
	return pWait;
}

void FUN_10007180(SND_pc_tdstVirtualWait *pWait)
{
	SNDLST2_M_DynamicIsolate(pWait);
	SND_fn_vFreeSndEx(0,pWait);
}

void FUN_100071e0(SND_tdhVirtualModule hModule,SND_pc_tdstVirtualReal *pReal)
{
	SND_pc_tdstVirtualVoice *pPrev;
	SND_pc_tdstVirtualReal *pNext;

	for (pPrev = pReal->pVoice->hPrevBrotherDyn; pPrev; pPrev = pPrev->hPrevBrotherDyn)
		if (pPrev->pReal)
			break;
	if (!pPrev)
	{
		SNDLST2_M_DynamicAddHead(&hModule->stReals,pReal);
	}
	else
	{
		pNext = pPrev->pReal->hNextBrotherDyn;
		SNDLST2_M_DynamicInsertBetween(pReal,pPrev->pReal,pNext);
	}
}

SND_pc_tdstVirtualReal *FUN_10007320(SND_tdhVirtualModule hModule,SND_pc_tdstVirtualVoice *pVoice)
{
	SND_pc_tdstVirtualReal *pReal;
	SND_td_pfn_vSoundCallback fn_callback;
	long par_callback;

	pReal = (SND_pc_tdstVirtualReal *)SND_fn_pvMallocSndEx(0,sizeof(SND_pc_tdstVirtualReal));
	SNDLST2_M_DynamicInitElement(pReal);
	pReal->pVoice = pVoice;
	FUN_100071e0(hModule,pReal);
	if (SND_fn_bIsResourceLoopingSxd(pVoice->res))
	{
		fn_callback = NULL;
		par_callback = 0;
		pVoice->bPlaying = 0;
	}
	else
	{
		fn_callback = pVoice->fn_callback;
		par_callback = pVoice->par_callback;
		pVoice->bPlaying = 1;
	}
	pVoice->hVoice = hModule->fn.pfn_lPlay(pVoice->res,pVoice->pPar,pVoice->lPrio,fn_callback,par_callback);
	return pReal;
}

void FUN_10007390(SND_tdhVirtualModule hModule,SND_pc_tdstVirtualReal *pReal)
{
	SNDLST2_M_DynamicIsolate(pReal);
	pReal->pVoice->pReal = NULL;
	if (pReal->pVoice->hVoice != -1)
		hModule->fn.pfn_vStop(pReal->pVoice->hVoice);
	SND_fn_vFreeSndEx(0,pReal);
}

SND_pc_tdstVirtualVoice *FUN_10007410(SND_tdhVirtualModule hModule)
{
	SND_pc_tdstVirtualVoice *pVoice;

	if (hModule->stWaits.hFirstElementDyn)
	{
		pVoice = hModule->stWaits.hFirstElementDyn->pVoice;
		FUN_10007180(hModule->stWaits.hFirstElementDyn);
	}
	else
	{
		pVoice = (SND_pc_tdstVirtualVoice *)SND_fn_pvMallocSndEx(0,sizeof(SND_pc_tdstVirtualVoice));
		pVoice->bLinked = 0;
	}
	SNDLST2_M_DynamicInitElement(pVoice);
	SNDLST2_M_DynamicAddTail(&hModule->stVoices,pVoice);
	pVoice->hVoice = -1;
	pVoice->pReal = NULL;
	pVoice->pPar = NULL;
	pVoice->lParSize = 0;
	pVoice->pWait = NULL;
	return pVoice;
}

void FUN_100074c0(SND_tdhVirtualModule hModule,SND_pc_tdstVirtualVoice *pVoice)
{
	SNDLST2_M_DynamicIsolate(pVoice);
	if (pVoice->pReal)
		FUN_10007390(hModule,pVoice->pReal);
	pVoice->hVoice = -1;
	if (pVoice->pPar)
	{
		SND_fn_vFreeSndEx(0,pVoice->pPar);
		pVoice->pPar = NULL;
		pVoice->lParSize = 0;
	}
}

void FUN_10007550(SND_tdhVirtualModule hModule,SND_pc_tdstVirtualVoice *pVoice)
{
	FUN_100074c0(hModule,pVoice);
	SND_fn_vFreeSndEx(0,pVoice);
}

void FUN_10007570(SND_tdhVirtualModule hModule,SND_pc_tdstVirtualVoice *pVoice)
{
	FUN_100074c0(hModule,pVoice);
	if (pVoice->bLinked)
		pVoice->pWait = FUN_10007120(hModule,pVoice);
	else
		FUN_10007550(hModule,pVoice);
}

void FUN_100075b0(SND_tdhVirtualModule hModule,SND_pc_tdstVirtualVoice *pVoice)
{
	SND_pc_tdstVirtualVoice *pPos;
	SND_pc_tdstVirtualVoice *pPrev;

	SNDLST2_M_DynamicIsolate(pVoice);
	if (pVoice->pReal)
		SNDLST2_M_DynamicIsolate(pVoice->pReal);
	for (pPos = hModule->stVoices.hFirstElementDyn; pPos && pPos->lPrio > pVoice->lPrio; pPos = pPos->hNextBrotherDyn)
		;
	if (pPos && pPos->lPrio == pVoice->lPrio)
	{
		for (; pPos && pPos->rNorme > pVoice->rNorme; pPos = pPos->hNextBrotherDyn)
			;
		if (pPos && pPos->rNorme == pVoice->rNorme)
			for (; pPos && pPos->lSeq > pVoice->lSeq; pPos = pPos->hNextBrotherDyn)
				;
	}
	if (!pPos)
	{
		SNDLST2_M_DynamicAddTail(&hModule->stVoices,pVoice);
	}
	else
	{
		pPrev = pPos->hPrevBrotherDyn;
		SNDLST2_M_DynamicInsertBetween(pVoice,pPrev,pPos);
	}
	if (pVoice->pReal)
		FUN_100071e0(hModule,pVoice->pReal);
}

void FUN_100077c0(SND_tdhVirtualModule hModule)
{
	SND_pc_tdstVirtualVoice *pVoice;
	SND_pc_tdstVirtualVoice *pLast;
	SND_pc_tdstVirtualReal *pReal;
	SND_pc_tdstVirtualReal *pNext;
	long i;

	for (i = 0, pVoice = hModule->stVoices.hFirstElementDyn; pVoice && i < hModule->lNbVoices; pVoice = pVoice->hNextBrotherDyn, i++)
		;
	pLast = pVoice ? pVoice->hPrevBrotherDyn : hModule->stVoices.hLastElementDyn;
	for (; pVoice; pVoice = pVoice->hNextBrotherDyn)
		if (pVoice->pReal)
			break;
	pReal = pVoice ? pVoice->pReal : NULL;
	while (pReal)
	{
		pNext = pReal->hNextBrotherDyn;
		if (!SND_fn_bIsResourceLoopingSxd(pReal->pVoice->res))
			FUN_10007570(hModule,pReal->pVoice);
		else
			FUN_10007390(hModule,pReal);
		pReal = pNext;
	}
	for (pVoice = pLast; pVoice; pVoice = pVoice->hPrevBrotherDyn)
		if (!pVoice->pReal && !pVoice->bPaused)
			pVoice->pReal = FUN_10007320(hModule,pVoice);
}

SND_pc_tdstVirtualVoice *FUN_10007870(SND_tdhVirtualModule hModule,SND_pc_tdstVirtualVoice *pOther,SND_pc_tdstVirtualVoice *pVoice)
{
	pOther->hVoice = -1;
	FUN_100074c0(hModule,pOther);
	SNDLST2_M_DynamicIsolate(pVoice);
	*pOther = *pVoice;
	pVoice->pReal = NULL;
	pVoice->pPar = NULL;
	pVoice->lParSize = 0;
	FUN_10007570(hModule,pVoice);
	if (pOther->pReal)
		pOther->pReal->pVoice = pOther;
	FUN_100075b0(hModule,pOther);
	FUN_100077c0(hModule);
	return pOther;
}

long FUN_10007930(SND_tdhVirtualModule hModule,tduRefRes res,SoundParam *par,long prio,SND_td_pfn_vSoundCallback fn_callback,long par_callback)
{
	long bFound = 0;
	long bLinked = 1;
	SND_pc_tdstVirtualVoice *pVoice;
	SND_pc_tdstVirtualVoice *pOther;
	SND_pc_tdstVirtualReal *pReal;
	SND_pc_tdstVirtualWait *pWait;

	pVoice = FUN_10007410(hModule);
	pVoice->rNorme = SND_fn_rGetNormeSxd(par);
	if (hModule->lSeq >= 0x10000000)
	{
		for (pOther = hModule->stVoices.hFirstElementDyn; pOther; pOther = pOther->hNextBrotherDyn)
		{
			pOther->lSeq = max(0,pOther->lSeq - 0x08000000);
		}
		hModule->lSeq = 0x08000000;
	}
	pVoice->lSeq = hModule->lSeq;
	hModule->lSeq++;
	pVoice->lPrio = prio;
	pVoice->fn_callback = fn_callback;
	pVoice->par_callback = par_callback;
	pVoice->bPlaying = 0;
	pVoice->res = res;
	pVoice->lParSize = par->iNbLinks * 8 + 0x38;
	pVoice->pPar = (SoundParam *)SND_fn_pvMallocSndEx(0,pVoice->lParSize);
	memcpy(pVoice->pPar,par,pVoice->lParSize);
	pVoice->bPaused = 0;
	FUN_100075b0(hModule,pVoice);
	FUN_100077c0(hModule);

	if ((pVoice->pReal && pVoice->hVoice == -1) || (!pVoice->pReal && !SND_fn_bIsResourceLoopingSxd(pVoice->res)))
	{
		FUN_10007570(hModule,pVoice);
		FUN_10007050(hModule);
		return -1;
	}

	for (pReal = hModule->stReals.hFirstElementDyn; pReal; pReal = pReal->hNextBrotherDyn)
	{
		pOther = pReal->pVoice;
		if (pOther->hVoice == pVoice->hVoice && pOther != pVoice)
		{
			pVoice = FUN_10007870(hModule,pReal->pVoice,pVoice);
			bFound = 1;
			break;
		}
	}
	if (!bFound && hModule->stWaits.hFirstElementDyn)
	{
		pWait = hModule->stWaits.hFirstElementDyn;
		pVoice = FUN_10007870(hModule,pWait->pVoice,pVoice);
		FUN_10007180(pWait);
	}
	pVoice->bLinked = bLinked;
	FUN_10007050(hModule);
	dbgSND_fn_vRajListesSxdVirtual2((long)hModule);
	return (long)pVoice;
}

void FUN_10007ac0(SND_tdhVirtualModule hModule,SND_pc_tdstVirtualVoice *pVoice)
{
	if (pVoice->pReal && pVoice->bPlaying)
		hModule->fn.pfn_vRemoveCallback(pVoice->hVoice);
	pVoice->fn_callback = 0;
}

SndBool FUN_10007af0(SND_tdhVirtualModule hModule,SND_pc_tdstVirtualVoice *pVoice,SoundParam *par)
{
	SndBool bOk;

	if (pVoice->pWait)
		bOk = 0;
	else if (pVoice->pReal)
		bOk = hModule->fn.pfn_bSetParam(pVoice->hVoice,par);
	else
		bOk = 1;

	if (!bOk)
	{
		if (!pVoice->pReal && pVoice->fn_callback && !pVoice->bPlaying)
			pVoice->fn_callback(pVoice->par_callback);
		if (pVoice->pWait)
			FUN_10007180(pVoice->pWait);
		pVoice->hVoice = -1;
		FUN_10007550(hModule,pVoice);
	}
	else
	{
		FUN_10007090(pVoice,par);
		pVoice->rNorme = SND_fn_rGetNormeSxd(par);
		FUN_100075b0(hModule,pVoice);
	}
	return bOk;
}

SndBool FUN_10007b90(SND_tdhVirtualModule hModule,SND_pc_tdstVirtualVoice *pVoice)
{
	SndBool bOk;

	if (pVoice->pWait)
		bOk = 0;
	else if (pVoice->pReal)
		bOk = hModule->fn.pfn_bTestIsPlaying(pVoice->hVoice);
	else
		bOk = 1;

	if (!bOk)
	{
		if (!pVoice->pReal && pVoice->fn_callback && !pVoice->bPlaying)
			pVoice->fn_callback(pVoice->par_callback);
		if (pVoice->pWait)
			FUN_10007180(pVoice->pWait);
		pVoice->hVoice = -1;
		FUN_10007550(hModule,pVoice);
	}
	return bOk;
}

void FUN_10007c10(SND_tdhVirtualModule hModule,SND_pc_tdstVirtualVoice *pVoice)
{
	if (pVoice->pWait)
		FUN_10007180(pVoice->pWait);
	if (pVoice->pReal)
		FUN_10007390(hModule,pVoice->pReal);
	if (pVoice->fn_callback && !pVoice->bPlaying)
		pVoice->fn_callback(pVoice->par_callback);
	FUN_10007550(hModule,pVoice);
	FUN_100077c0(hModule);
	FUN_10007050(hModule);
	dbgSND_fn_vRajListesSxdVirtual2((long)hModule);
}

void FUN_10007c80(SND_tdhVirtualModule hModule,SND_pc_tdstVirtualVoice *pVoice)
{
	pVoice->bPaused = 1;
	if (pVoice->pReal)
		hModule->fn.pfn_vPause(pVoice->hVoice);
}

void FUN_10007ca0(SND_tdhVirtualModule hModule,SND_pc_tdstVirtualVoice *pVoice)
{
	pVoice->bPaused = 0;
	if (pVoice->pReal)
		hModule->fn.pfn_vResume(pVoice->hVoice);
	else
		FUN_100077c0(hModule);
}

SndReal FUN_10007cd0(SND_tdhVirtualModule hModule,SND_pc_tdstVirtualVoice *pVoice)
{
	if (pVoice->pReal)
		return hModule->fn.pfn_rGetPos(pVoice->hVoice);
	return -1;
}

SndReal FUN_10007cf0(SND_tdhVirtualModule hModule,SND_pc_tdstVirtualVoice *pVoice)
{
	if (pVoice->pReal)
		return hModule->fn.pfn_rGetLength(pVoice->hVoice);
	return -1;
}

SND_tdhVirtualModule FUN_10007d10(long lNbVoices,SND_pc_tdstVirtualVoiceFunctions *pFunctions)
{
	SND_tdhVirtualModule hModule;

	if (SND_pc_ListVirtualModule.bInit == 0)
	{
		SND_pc_ListVirtualModule.bInit = 1;
		SND_pc_ListVirtualModule.pHead = 0;
		SND_pc_ListVirtualModule.pTail = 0;
		SND_pc_ListVirtualModule.lCount = 0;
	}
	hModule = (SND_tdhVirtualModule)SND_fn_pvMallocSndEx(0,sizeof(struct _virtual_module));
	hModule->pPrevModule = 0;
	hModule->pList = 0;
	hModule->pNextModule = 0;
	hModule->pPrevModule = SND_pc_ListVirtualModule.pTail;
	hModule->pList = &SND_pc_ListVirtualModule;
	if (SND_pc_ListVirtualModule.pHead == 0)
		SND_pc_ListVirtualModule.pHead = hModule;
	else
		SND_pc_ListVirtualModule.pTail->pNextModule = hModule;
	SND_pc_ListVirtualModule.pTail = hModule;
	SND_pc_ListVirtualModule.lCount++;
	SNDLST2_M_DynamicInitAnchor(&hModule->stWaits);
	SNDLST2_M_DynamicInitAnchor(&hModule->stReals);
	SNDLST2_M_DynamicInitAnchor(&hModule->stVoices);
	hModule->lSeq = 0;
	hModule->lNbVoices = lNbVoices;
	hModule->fn = *pFunctions;
	return hModule;
}

long FUN_10007dc0(void)
{
	return 1;
}

void FUN_10007dd0(SND_tdhVirtualModule hModule)
{
	if (hModule->pNextModule)
		hModule->pNextModule->pPrevModule = hModule->pPrevModule;
	if (hModule->pPrevModule)
		hModule->pPrevModule->pNextModule = hModule->pNextModule;
	if (hModule->pList)
	{
		hModule->pList->lCount--;
		if (hModule->pList->pHead == hModule)
			hModule->pList->pHead = hModule->pNextModule;
		if (hModule->pList->pTail == hModule)
			hModule->pList->pTail = hModule->pPrevModule;
	}
	hModule->pNextModule = 0;
	hModule->pPrevModule = 0;
	hModule->pList = 0;
	SND_fn_vFreeSndEx(0,hModule);
}

void FUN_10007e30(SND_tdhVirtualModule hModule)
{
	FUN_100077c0(hModule);
	FUN_10007050(hModule);
	dbgSND_fn_vRajListesSxdVirtual2((long)hModule);
}

void FUN_10007e50(SND_tdhVirtualModule hModule,long lNbVoices)
{
	hModule->lNbVoices = lNbVoices;
}

#endif
