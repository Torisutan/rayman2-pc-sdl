#include "SNDinc.h"

#ifndef NO_ACP_SOUND

#if defined(_DLL_COMPILATION_MODE)
#include "snddll.h"
#endif

#include "sndres.h"
#define DLLWAV_MODULE
#include "sndxd.h"

typedef struct _SND_pc_tdstTransBlock *SND_pc_p_tdstTransBlock;

SNDLST2_M_DynamicUseListOf(SND_pc_p_tdstTransBlock);

typedef struct _SND_pc_tdstTransBlock {
	void			*pData;
	long			lNbSamples;
	long			lPushedId;
	SNDLST2_M_DynamicElementDeclaration(SND_pc_p_tdstTransBlock)
} SND_pc_tdstTransBlock;

typedef struct _SND_pc_tdstTransition *SND_pc_p_tdstTransition;

SNDLST2_M_DynamicUseListOf(SND_pc_p_tdstTransition);

typedef struct _SND_pc_tdstTransition {
	long					id_buffer;
	SndBool					bFinished;
	SndBool					bFade;
	long					lShift;
	tdhSndFile				hFile;
	long					l14;
	unsigned long			ulOffsetLast;
	unsigned long			ulOffsetCurrent;
	tdstBlockResourceMem	*pCurRes;
	tdstBlockResourceMem	*pNextRes;
	tdstBlockResourceMem	*pFadeRes;
	unsigned long			ulFade;
	long					lFadeShift;
	SND_td_pfn_vSoundCallback transition_callback;
	long					param_callback;
	SNDLST2_M_DynamicAnchorDeclaration(SND_pc_p_tdstTransBlock) stBlocks;
	SNDLST2_M_DynamicElementDeclaration(SND_pc_p_tdstTransition)
} SND_pc_tdstTransition;

typedef struct _SND_pc_tdstTransitionList {
	SNDLST2_M_DynamicAnchorDeclaration(SND_pc_p_tdstTransition) pList;
} SND_pc_tdstTransitionList;

#define SND_pc_C_TransBlockSize 0x1400

SND_pc_tdstTransitionList	SND_pc_stTransitions = {0};
long						SND_pc_l26B2C = 0;
SND_pc_tdhLock				SND_pc_hTransMutex = NULL;
long						SND_pc_l26B34 = 0;

void SND_pc_fn_vLockTransitions(void)
{
	if (SND_pc_hTransMutex)
		if (SND_pc_M_ulWaitLock(SND_pc_hTransMutex,SND_pc_C_ulWaitForever) == SND_pc_C_ulWaitTimedOut)
			SND_pc_M_vBreak();
}

void SND_pc_fn_vUnlockTransitions(void)
{
	if (SND_pc_hTransMutex)
		SND_pc_M_bReleaseLock(SND_pc_hTransMutex);
}

void FUN_100068a0(SND_pc_tdstTransBlock *pBlock)
{
	SNDLST2_M_DynamicIsolate(pBlock);
	SND_fn_vFreeSnd(pBlock->pData);
	SND_fn_vFreeSndEx(0,pBlock);
}

void FUN_10006910(SND_pc_tdstTransition *pTrans)
{
	SND_pc_tdstTransBlock *pBlock;
	SND_tdstStackBuffer stStack;
	char szFileName[256];
	unsigned long ulRead;
	unsigned long ulLeft;
	unsigned long ulSize;
	SndBool bDoFade;
	long i;
	long lValue;

	bDoFade = FALSE;
	if (!pTrans->bFinished)
	{
		pBlock = (SND_pc_tdstTransBlock *)SND_fn_pvMallocSndEx(0,sizeof(SND_pc_tdstTransBlock));
		SNDLST2_M_DynamicInitElement(pBlock);
		pBlock->lNbSamples = SND_pc_C_TransBlockSize >> pTrans->lShift;
		pBlock->pData = SND_fn_pvMallocSnd(SND_pc_C_TransBlockSize);
		ulLeft = SND_pc_C_TransBlockSize;
		do
		{
			if (pTrans->hFile == (tdhSndFile)-1 && !pTrans->pNextRes)
			{
				memset(pBlock->pData,0,ulLeft);
				ulLeft = 0;
			}
			else
			{
				if (pTrans->hFile == (tdhSndFile)-1)
				{
					pTrans->pCurRes = pTrans->pNextRes;
					pTrans->pNextRes = NULL;
					pTrans->ulOffsetLast = pTrans->pCurRes->uRes.stSample.uData.stStream.ulOffsetLast;
					pTrans->ulOffsetCurrent = pTrans->pCurRes->uRes.stSample.uData.stStream.ulOffsetFirst;
					pTrans->transition_callback(pTrans->param_callback);
					SND_fn_vResolveFileName(pTrans->pCurRes->uRes.stSample.uData.stStream.szFileName,szFileName);
					pTrans->hFile = SND_fn_hOpenFileReadSnd(szFileName);
					SND_fn_dwSeekFileSnd(pTrans->hFile,pTrans->ulOffsetCurrent,0);
				}
				ulSize = pTrans->ulOffsetLast - pTrans->ulOffsetCurrent;
				if (ulSize > SND_pc_C_TransBlockSize)
					ulSize = SND_pc_C_TransBlockSize;
				if (pTrans->bFade)
				{
					if (ulSize >= pTrans->ulFade)
						ulSize = pTrans->ulFade;
					bDoFade = TRUE;
				}
				ulRead = SND_fn_dwReadFileSnd(pTrans->hFile,ulSize,pBlock->pData);
				if (ulRead != SND_pc_C_TransBlockSize)
				{
					SND_fn_vCloseFileSnd(pTrans->hFile);
					pTrans->hFile = (tdhSndFile)-1;
					if (pTrans->bFade && ulRead == pTrans->ulFade)
					{
						pTrans->pNextRes = pTrans->pFadeRes;
						pTrans->pFadeRes = NULL;
					}
					if (pTrans->pNextRes)
					{
						pTrans->pCurRes = pTrans->pNextRes;
						pTrans->pNextRes = NULL;
						pTrans->ulOffsetLast = pTrans->pCurRes->uRes.stSample.uData.stStream.ulOffsetLast;
						pTrans->ulOffsetCurrent = pTrans->pCurRes->uRes.stSample.uData.stStream.ulOffsetFirst;
						SND_fn_vResolveFileName(pTrans->pCurRes->uRes.stSample.uData.stStream.szFileName,szFileName);
						pTrans->hFile = SND_fn_hOpenFileReadSnd(szFileName);
						SND_fn_dwSeekFileSnd(pTrans->hFile,pTrans->ulOffsetCurrent,0);
					}
					else
						memset((char *)pBlock->pData + ulRead,0,ulLeft - ulRead);
					if (!pTrans->bFade || ulRead == pTrans->ulFade)
					{
						pTrans->transition_callback(pTrans->param_callback);
						pTrans->bFade = FALSE;
					}
				}
				else
					pTrans->ulOffsetCurrent += SND_pc_C_TransBlockSize;
				if (bDoFade)
				{
					for (i = 0; i < (pBlock->lNbSamples << pTrans->lShift) / 4; i++)
					{
						lValue = ((long *)pBlock->pData)[i];
						((long *)pBlock->pData)[i] = (((lValue << 16 >> 16) * (long)pTrans->ulFade >> pTrans->lFadeShift) & 0xFFFF) | (((lValue >> 16) * (long)pTrans->ulFade >> pTrans->lFadeShift) << 16);
						if (pTrans->pCurRes->uRes.stSample.uwNbChannels == 2)
							pTrans->ulFade -= 2;
						else
							pTrans->ulFade--;
						if (!pTrans->ulFade)
							break;
					}
				}
				ulLeft = SND_pc_C_TransBlockSize - ulRead;
			}
		} while (ulLeft > 0);
		SNDLST2_M_DynamicIsolate(pBlock);
		SNDLST2_M_DynamicAddTail(&pTrans->stBlocks,pBlock);
		stStack.ulNbSamples = pBlock->lNbSamples;
		stStack.ptrData = pBlock->pData;
		pBlock->lPushedId = SND_fn_lPushBufferExSxd(pTrans->id_buffer,&stStack);
	}
}

void SND_CALL FUN_10006c20(long user_id,long poped_id)
{
	SND_pc_tdstTransition *pTrans;
	SND_pc_tdstTransBlock *pBlock;

	SND_pc_fn_vLockTransitions();
	pTrans = (SND_pc_tdstTransition *)user_id;
	for (pBlock = pTrans->stBlocks.hFirstElementDyn; pBlock; pBlock = pBlock->hNextBrotherDyn)
	{
		if (pBlock->lPushedId == poped_id)
		{
			FUN_100068a0(pBlock);
			break;
		}
	}
	FUN_10006910(pTrans);
	SND_pc_fn_vUnlockTransitions();
}

void FUN_10006c70(SND_pc_tdstTransition *pTrans)
{
	SND_pc_tdstTransBlock *pBlock;
	SND_pc_tdstTransBlock *pNext;
	long i;

	SND_fn_vDeleteBufferSxd(pTrans->id_buffer);
	pTrans->id_buffer = -1;
	if (pTrans->hFile != (tdhSndFile)-1)
		SND_fn_vCloseFileSnd(pTrans->hFile);
	pTrans->hFile = (tdhSndFile)-1;
	SNDLST2_M_DynamicForEachMovingElementOf(&pTrans->stBlocks,pBlock,pNext,i)
	{
		FUN_100068a0(pBlock);
	}
	SNDLST2_M_DynamicIsolate(pTrans);
	SND_fn_vFreeSndEx(0,pTrans);
}

long FUN_10006d30(tduRefRes FirstRes,tduRefRes NextRes,SND_td_pfn_vSoundCallback transition_callback,long param_callback,SoundParam *par)
{
	SND_pc_tdstTransition *pTrans;
	SND_tdstFormat stFormat;
	SND_tdstCallback stCallback;
	char szFileName[256];
	int i;

	if (!FirstRes.pstPtr->uRes.stSample.bStream || !NextRes.pstPtr->uRes.stSample.bStream)
		return C_PLAY_FAILED;
	SND_pc_fn_vLockTransitions();
	pTrans = (SND_pc_tdstTransition *)SND_fn_pvMallocSndEx(0,sizeof(SND_pc_tdstTransition));
	SNDLST2_M_DynamicInitElement(pTrans);
	SNDLST2_M_DynamicAddTail(&SND_pc_stTransitions.pList,pTrans);
	SNDLST2_M_DynamicInitAnchor(&pTrans->stBlocks);

	stFormat.eZip = SAMPLE_PCM;
	stFormat.uFormat.stPCM.ulNbSamples = 0;
	stFormat.uFormat.stPCM.uwResolution = FirstRes.pstPtr->uRes.stSample.uwResolution;
	stFormat.uFormat.stPCM.uwNbChannels = FirstRes.pstPtr->uRes.stSample.uwNbChannels;
	stFormat.uFormat.stPCM.ulFreq = FirstRes.pstPtr->uRes.stSample.ulFreq;
	stCallback.eType = BUFFER_STACK;
	stCallback.uCallback.CallbackStack = FUN_10006c20;
	stCallback.uInfo.lNbSampleToPush = 0;
	pTrans->transition_callback = transition_callback;
	pTrans->param_callback = param_callback;
	pTrans->l14 = -1;
	pTrans->id_buffer = SND_fn_lCreateNewBufferExSxd(&stFormat,&stCallback,par,(long)pTrans);

	SND_fn_vResolveFileName(FirstRes.pstPtr->uRes.stSample.uData.stStream.szFileName,szFileName);
	pTrans->pNextRes = NextRes.pstPtr;
	pTrans->pCurRes = FirstRes.pstPtr;
	pTrans->ulOffsetLast = FirstRes.pstPtr->uRes.stSample.uData.stStream.ulOffsetLast;
	pTrans->ulOffsetCurrent = FirstRes.pstPtr->uRes.stSample.uData.stStream.ulOffsetFirst;
	pTrans->bFade = FALSE;
	pTrans->hFile = SND_fn_hOpenFileReadSnd(szFileName);
	pTrans->bFinished = FALSE;
	pTrans->lShift = (stFormat.uFormat.stPCM.uwResolution >> 3) + stFormat.uFormat.stPCM.uwNbChannels - 2;
	SND_fn_dwSeekFileSnd(pTrans->hFile,pTrans->ulOffsetCurrent,0);
	for (i = 0; i < 3; i++)
		FUN_10006910(pTrans);
	SND_pc_fn_vUnlockTransitions();
	return (long)pTrans;
}

SndBool FUN_10006ec0(long voice,SoundParam *par)
{
	SndBool bOk;

	SND_pc_fn_vLockTransitions();
	if (voice == -1)
		bOk = FALSE;
	else if (((SND_pc_tdstTransition *)voice)->bFinished)
	{
		FUN_10006c70((SND_pc_tdstTransition *)voice);
		bOk = FALSE;
	}
	else
	{
		SND_fn_vSetParamBufferSxd(((SND_pc_tdstTransition *)voice)->id_buffer,par);
		bOk = TRUE;
	}
	SND_pc_fn_vUnlockTransitions();
	return bOk;
}

SndBool FUN_10006f10(long voice,tduRefRes new_res)
{
	if (!new_res.pstPtr->uRes.stSample.bStream)
		return FALSE;
	SND_pc_fn_vLockTransitions();
	((SND_pc_tdstTransition *)voice)->pNextRes = new_res.pstPtr;
	SND_pc_fn_vUnlockTransitions();
	return TRUE;
}

SndBool FUN_10006f40(long voice,tduRefRes new_res,SndReal rDuration)
{
	SND_pc_tdstTransition *pTrans;
	long lShift;

	if (new_res.pstPtr->uRes.stSample.bStream)
	{
		SND_pc_fn_vLockTransitions();
		pTrans = (SND_pc_tdstTransition *)voice;
		pTrans->pFadeRes = new_res.pstPtr;
		if (!pTrans->bFade)
		{
			pTrans->bFade = TRUE;
			pTrans->ulFade = (pTrans->pCurRes->uRes.stSample.ulFreq * rDuration) >> 16;
			lShift = 0;
			while (pTrans->ulFade)
			{
				lShift++;
				pTrans->ulFade >>= 1;
			}
			pTrans->lFadeShift = lShift;
			pTrans->ulFade = 1 << lShift;
		}
		SND_pc_fn_vUnlockTransitions();
	}
	return FALSE;
}

SndBool FUN_10006fa0(long voice,tduRefRes new_res)
{
	return FUN_10006f40(voice,new_res,0x10000);
}

void FUN_10006fc0(long voice)
{
	SND_pc_fn_vLockTransitions();
	if (voice != -1)
	{
		((SND_pc_tdstTransition *)voice)->bFinished = TRUE;
		FUN_10006c70((SND_pc_tdstTransition *)voice);
	}
	SND_pc_fn_vUnlockTransitions();
}

void FUN_10006ff0(long voice)
{
	SND_pc_fn_vLockTransitions();
	SND_fn_vPauseBufferSxd(((SND_pc_tdstTransition *)voice)->id_buffer);
	SND_pc_fn_vUnlockTransitions();
}

void FUN_10007010(long voice)
{
	SND_pc_fn_vLockTransitions();
	SND_fn_vResumeBufferSxd(((SND_pc_tdstTransition *)voice)->id_buffer);
	SND_pc_fn_vUnlockTransitions();
}

int SND_pc_fn_iCreateTransitionLock(SND_tdstInitStruct *pInitStruct)
{
	SND_pc_hTransMutex = SND_pc_M_hCreateLock(FALSE);
	return SND_pc_hTransMutex ? C_INIT_OK : C_INIT_FAILED;
}

#endif
