











#include "SNDinc.h"

#ifndef NO_ACP_SOUND
#if defined(_DLL_COMPILATION_MODE)
#include "snddll.h"
#else
#include "sndthrd.h"
#endif
#include "sndres.h"
#define DLLWAV_MODULE
#include "sndxd.h"


typedef struct _SND_pc_tdstStreamBlock *SND_pc_p_tdstStreamBlock;

SNDLST2_M_DynamicUseListOf(SND_pc_p_tdstStreamBlock);

typedef struct _SND_pc_tdstStreamBlock {
	void			*pData;
	unsigned long	ulNbSamples;
	long			lPushedId;
	SNDLST2_M_DynamicElementDeclaration(SND_pc_p_tdstStreamBlock)
} SND_pc_tdstStreamBlock;


typedef struct _SND_pc_tdstStream *SND_pc_p_tdstStream;

SNDLST2_M_DynamicUseListOf(SND_pc_p_tdstStream);

typedef struct _SND_pc_tdstStream {
	long			id_buffer;
	long			lShift;
	tdhSndFile		hFile;
	long			lLastPushedId;
	SndBool			bEndOfFile;
	SndBool			bFinished;
	SndBool			bLoop;
	unsigned long	ulOffsetLoop;
	unsigned long	ulOffsetLast;
	unsigned long	ulOffsetCurrent;
	SND_td_pfn_vSoundCallback fn_callback;
	long			par_callback;
	SndBool			bCallback;
	SndBool			bStereo;
	SNDLST2_M_DynamicAnchorDeclaration(SND_pc_p_tdstStreamBlock) stBlocks;
	SNDLST2_M_DynamicElementDeclaration(SND_pc_p_tdstStream)
} SND_pc_tdstStream;

typedef struct _SND_pc_tdstStreamList {
	SNDLST2_M_DynamicAnchorDeclaration(SND_pc_p_tdstStream) pList;
} SND_pc_tdstStreamList;

#define SND_pc_C_StreamBlockSize 0x1400



SND_pc_tdstStreamList	SND_pc_stStreams = {0};
SndBool					SND_pc_bStreamInit = FALSE;

void FUN_10005fc0(SND_pc_tdstStreamBlock *pBlock)
{
	SNDLST2_M_DynamicIsolate(pBlock);
	SND_fn_vFreeSnd(pBlock->pData);
	SND_fn_vFreeSndEx(0,pBlock);
}

void FUN_10006030(SND_pc_tdstStream *pStream)
{
	SND_pc_tdstStreamBlock *pBlock;
	SND_tdstStackBuffer stStack;
	unsigned long ulRead;
	unsigned long ulLeft;
	unsigned long ulSize;

	if (!pStream->bEndOfFile)
	{
		pBlock = (SND_pc_tdstStreamBlock *)SND_fn_pvMallocSndEx(0,sizeof(SND_pc_tdstStreamBlock));
		SNDLST2_M_DynamicInitElement(pBlock);
		pBlock->ulNbSamples = SND_pc_C_StreamBlockSize >> pStream->lShift;
		pBlock->pData = SND_fn_pvMallocSnd(SND_pc_C_StreamBlockSize);
		ulSize = pStream->ulOffsetLast - pStream->ulOffsetCurrent;
		if (ulSize > SND_pc_C_StreamBlockSize)
			ulSize = SND_pc_C_StreamBlockSize;
		ulRead = SND_fn_dwReadFileSnd(pStream->hFile,ulSize,pBlock->pData);
		if (ulRead != SND_pc_C_StreamBlockSize)
		{
		if (pStream->bLoop)
		{
			pStream->ulOffsetCurrent = pStream->ulOffsetLoop;
			SND_fn_dwSeekFileSnd(pStream->hFile,pStream->ulOffsetLoop,0);
			ulLeft = SND_pc_C_StreamBlockSize - ulRead;
			while (ulLeft)
			{
				pStream->ulOffsetCurrent = pStream->ulOffsetLoop;
				ulRead = SND_fn_dwReadFileSnd(pStream->hFile,(ulLeft < pStream->ulOffsetLast - pStream->ulOffsetLoop) ? ulLeft : pStream->ulOffsetLast - pStream->ulOffsetLoop,(char *)pBlock->pData + SND_pc_C_StreamBlockSize - ulLeft);
				ulLeft -= ulRead;
				if (!ulRead || ulLeft <= 0)
					break;
			}
			pStream->ulOffsetCurrent = ulRead + pStream->ulOffsetLoop;
		}
		else
		{
			pBlock->ulNbSamples = ulRead >> pStream->lShift;
			pStream->lLastPushedId = pBlock->lPushedId;
			SND_fn_vCloseFileSnd(pStream->hFile);
			pStream->hFile = (tdhSndFile)-1;
			pStream->bEndOfFile = TRUE;
		}
		}
		else
			pStream->ulOffsetCurrent += SND_pc_C_StreamBlockSize;
		SNDLST2_M_DynamicIsolate(pBlock);
		SNDLST2_M_DynamicAddTail(&pStream->stBlocks,pBlock);
		stStack.ulNbSamples = pBlock->ulNbSamples;
		stStack.ptrData = pBlock->pData;
		pBlock->lPushedId = SND_fn_lPushBufferExSxd(pStream->id_buffer,&stStack);
		if (pStream->bEndOfFile)
			pStream->lLastPushedId = pBlock->lPushedId;
	}
	else if (pStream->bCallback)
	{
		pStream->bCallback = FALSE;
		pStream->fn_callback(pStream->par_callback);
	}
}

void SND_CALL FUN_100061e0(long user_id,long poped_id)
{
	SND_pc_tdstStream *pStream;
	SND_pc_tdstStreamBlock *pBlock;

	SND_fn_vEnterCriticalSectionThreadSnd_();
#line 148 "..\\..\\..\\..\\..\\src\\Win95\\Sndstrm2.c"
	SND_fn_vMouchardThreadsnd(__FILE__,__LINE__);
	pStream = (SND_pc_tdstStream *)user_id;
	for (pBlock = pStream->stBlocks.hFirstElementDyn; pBlock; pBlock = pBlock->hNextBrotherDyn)
	{
		if (pBlock->lPushedId == poped_id)
		{
			FUN_10005fc0(pBlock);
			break;
		}
	}
	if (!pStream->bEndOfFile)
		FUN_10006030(pStream);
	else if (pStream->lLastPushedId == poped_id)
		pStream->bFinished = TRUE;
	SND_fn_vQuitCriticalSectionThreadSnd();
}

void FUN_10006250(SND_pc_tdstStream *pStream)
{
	SND_pc_tdstStreamBlock *pBlock;
	SND_pc_tdstStreamBlock *pNext;
	long i;

	SND_fn_vDeleteBufferSxd(pStream->id_buffer);
	pStream->id_buffer = -1;
	if (pStream->hFile != (tdhSndFile)-1)
		SND_fn_vCloseFileSnd(pStream->hFile);
	pStream->hFile = (tdhSndFile)-1;
	SNDLST2_M_DynamicForEachMovingElementOf(&pStream->stBlocks,pBlock,pNext,i)
	{
		FUN_10005fc0(pBlock);
	}
	if (pStream->bCallback)
	{
		pStream->bCallback = FALSE;
		pStream->fn_callback(pStream->par_callback);
	}
	SNDLST2_M_DynamicIsolate(pStream);
	SND_fn_vFreeSndEx(0,pStream);
}

long FUN_10006310(tduRefRes res,SoundParam *par,long prio,SND_td_pfn_vSoundCallback fn_callback,long par_callback)
{
	SND_pc_tdstStream *pStream;
	SND_tdstFormat stFormat;
	SND_tdstCallback stCallback;
	char szFileName[256];
	SoundParam *pPar;
	int i;

	if (SND_pc_bStreamInit)
	{
	pStream = (SND_pc_tdstStream *)SND_fn_pvMallocSndEx(0,sizeof(SND_pc_tdstStream));
	SNDLST2_M_DynamicInitElement(pStream);
	SNDLST2_M_DynamicAddTail(&SND_pc_stStreams.pList,pStream);
	SNDLST2_M_DynamicInitAnchor(&pStream->stBlocks);

	stFormat.eZip = SAMPLE_PCM;
	stFormat.uFormat.stPCM.ulNbSamples = 0;
	stFormat.uFormat.stPCM.uwResolution = res.pstPtr->uRes.stSample.uwResolution;
	stFormat.uFormat.stPCM.uwNbChannels = res.pstPtr->uRes.stSample.uwNbChannels;
	stFormat.uFormat.stPCM.ulFreq = res.pstPtr->uRes.stSample.ulFreq;
	stCallback.eType = BUFFER_STACK;
	stCallback.uCallback.CallbackStack = FUN_100061e0;
	stCallback.uInfo.lNbSampleToPush = 0;

	pStream->bLoop = res.pstPtr->uRes.stSample.bLoop;
	pStream->ulOffsetLoop = res.pstPtr->uRes.stSample.uData.stStream.ulOffsetLoop;
	pStream->ulOffsetLast = res.pstPtr->uRes.stSample.uData.stStream.ulOffsetLast;
	pStream->ulOffsetCurrent = res.pstPtr->uRes.stSample.uData.stStream.ulOffsetFirst;
	SND_fn_vResolveFileName(res.pstPtr->uRes.stSample.uData.stStream.szFileName,szFileName);
	pStream->hFile = SND_fn_hOpenFileReadSnd(szFileName);
	SND_fn_dwSeekFileSnd(pStream->hFile,pStream->ulOffsetCurrent,0);

	pPar = (SoundParam *)SND_fn_pvMallocSnd(SND_M_lGetSizeOfSoundParam(par));
	memcpy(pPar,par,SND_M_lGetSizeOfSoundParam(par));
	if (stFormat.uFormat.stPCM.uwNbChannels == 2)
	{
		pPar->ucVol = par->ucVol >> 1;
		pPar->iFlags |= 4;
	}
	if (stFormat.uFormat.stPCM.uwNbChannels == 2)
		pStream->bStereo = TRUE;
	else
		pStream->bStereo = FALSE;
	pStream->lLastPushedId = -1;
	pStream->bFinished = FALSE;
	pStream->bEndOfFile = FALSE;
	pStream->lShift = (stFormat.uFormat.stPCM.uwResolution >> 3) + stFormat.uFormat.stPCM.uwNbChannels - 2;
	pStream->id_buffer = SND_fn_lCreateNewBufferExSxd(&stFormat,&stCallback,pPar,(long)pStream);
	SND_fn_vFreeSnd(pPar);
	if (fn_callback)
	{
		pStream->fn_callback = fn_callback;
		pStream->par_callback = par_callback;
		pStream->bCallback = TRUE;
	}
	else
		pStream->bCallback = FALSE;
	for (i = 0; i < 4; i++)
		FUN_10006030(pStream);
	return (long)pStream;
	}
	return C_PLAY_FAILED;
}

void FUN_10006500(long voice)
{
}

SndBool FUN_10006510(long voice,SoundParam *par)
{
	SoundParam *pPar;

	if (voice == -1)
		return FALSE;
	if (((SND_pc_tdstStream *)voice)->bFinished)
	{
		FUN_10006250((SND_pc_tdstStream *)voice);
		return FALSE;
	}
	pPar = (SoundParam *)SND_fn_pvMallocSnd(SND_M_lGetSizeOfSoundParam(par));
	memcpy(pPar,par,SND_M_lGetSizeOfSoundParam(par));
	if (((SND_pc_tdstStream *)voice)->bStereo)
	{
		pPar->ucVol = par->ucVol >> 1;
		pPar->iFlags |= 4;
	}
	SND_fn_vSetParamBufferSxd(((SND_pc_tdstStream *)voice)->id_buffer,pPar);
	SND_fn_vFreeSnd(pPar);
	return TRUE;
}

SndBool FUN_100065b0(long voice)
{
	if (voice == -1)
		return FALSE;
	if (((SND_pc_tdstStream *)voice)->bFinished)
	{
		FUN_10006250((SND_pc_tdstStream *)voice);
		return FALSE;
	}
	return TRUE;
}

void FUN_100065e0(long voice)
{
	if (voice != -1)
	{
		((SND_pc_tdstStream *)voice)->bFinished = TRUE;
		if (((SND_pc_tdstStream *)voice)->bCallback)
		{
			((SND_pc_tdstStream *)voice)->bCallback = FALSE;
			((SND_pc_tdstStream *)voice)->fn_callback(((SND_pc_tdstStream *)voice)->par_callback);
		}
		FUN_10006250((SND_pc_tdstStream *)voice);
	}
}

void FUN_10006620(long voice)
{
}

void FUN_10006630(long voice)
{
}

int FUN_10006640(SND_tdstInitStruct *pInitStruct)
{
	SND_pc_bStreamInit = TRUE;
	SNDLST2_M_DynamicInitAnchor(&SND_pc_stStreams.pList);
	return C_INIT_OK;
}

SndBool FUN_10006670(void)
{
	return TRUE;
}

void FUN_10006680(void)
{
	SND_pc_tdstStream *pStream;
	SND_pc_tdstStream *pNext;
	long i;

	SNDLST2_M_DynamicForEachMovingElementOf(&SND_pc_stStreams.pList,pStream,pNext,i)
	{
		FUN_10006250(pStream);
	}
}

SndReal FUN_100066c0(long voice)
{
	if (voice != -1)
	{
		if (!((SND_pc_tdstStream *)voice)->bFinished)
			return SND_fn_rGetPosBufferSxd(((SND_pc_tdstStream *)voice)->id_buffer);
		return -2;
	}
	return -1;
}

SndReal FUN_100066f0(long voice)
{
	return -1;
}

void FUN_10006700(tdstBlockResourceDisk *disk,tdstBlockResourceMem *mem,void *ptrBegin)
{
	mem->Id = disk->Id;
	mem->eType = disk->eType;
	mem->eStorage = disk->eStorage;
	mem->ucVolume = disk->ucVolume;
	mem->bIsLoaded = TRUE;
	mem->uRes.stSample.eZip = SAMPLE_PCM;
	mem->uRes.stSample.bPitchable = disk->uRes.stSample.bPitchable;
	mem->uRes.stSample.bVolable = disk->uRes.stSample.bVolable;
	mem->uRes.stSample.bPanable = disk->uRes.stSample.bPanable;
	mem->uRes.stSample.bSpacable = disk->uRes.stSample.bSpacable;
	mem->uRes.stSample.bReverbable = disk->uRes.stSample.bReverbable;
	mem->uRes.stSample.bStream = disk->uRes.stSample.bStream;
	mem->uRes.stSample.bLoop = disk->uRes.stSample.bLoop;
	mem->uRes.stSample.ulFreq = disk->uRes.stSample.ulFreq;
	mem->uRes.stSample.uwResolution = disk->uRes.stSample.uwResolution;
	mem->uRes.stSample.uwNbChannels = disk->uRes.stSample.uwNbChannels;
	strcpy(mem->uRes.stSample.uData.stStream.szFileName,disk->uRes.stSample.szFileName);
	mem->uRes.stSample.uData.stStream.ulOffsetFirst = disk->ulDataOffset;
	if (!disk->uRes.stSample.bLoop)
	{
		mem->uRes.stSample.uData.stStream.ulOffsetLoop = 0;
		mem->uRes.stSample.uData.stStream.ulOffsetLast = disk->ulDataSize + disk->ulDataOffset;
	}
	else
	{
		mem->uRes.stSample.uData.stStream.ulOffsetLoop = disk->uRes.stSample.ulStartLoop + disk->ulDataOffset;
		mem->uRes.stSample.uData.stStream.ulOffsetLast = disk->uRes.stSample.pc_ulLoopLength + disk->uRes.stSample.ulStartLoop + disk->ulDataOffset;
	}
}

SndBool FUN_100067e0(tdstBlockResourceDisk *disk,tdstBlockResourceMem *mem)
{
	FUN_10006700(disk,mem,NULL);
	mem->eStorage = 0;
	return TRUE;
}

SndBool FUN_10006810(tdstBlockResourceDisk *disk,tdstBlockResourceMem *mem,char *pDataBloc)
{
	FUN_10006700(disk,mem,NULL);
	return TRUE;
}

void FUN_10006830(tdstBlockResourceMem *mem)
{
	mem->uRes.stSample.uData.stMem.pvPtrFirst = NULL;
	mem->eType = 0;
}

SndBool FUN_10006840(tduRefRes res,unsigned char ucVolume)
{
	res.pstPtr->ucVolume = ucVolume;
	return TRUE;
}

#endif
