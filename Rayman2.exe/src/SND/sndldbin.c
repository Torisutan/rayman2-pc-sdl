#include <stddef.h>
#include <stdio.h>
#include <string.h>
#include <windows.h>

#include "SND/snddef.h"
#include "SND/SND.h"
#include "SND/SND_globals.h"
#include "SND/snderr.h"
#include "SND/sndres.h"
#include "SND/sndevent.h"
#include "SND/sndhard.h"
#include "SND/sndplay.h"
#include "SND/sndldbin.h"
#include "SND/sndmem.h"
#include "SND/sndfile.h"
#include "SND/sndmidi.h"
#include "SND/sndxd.h"

SndBool __stdcall SND_fn_bCanFreeDataMPEG(void);
SndBool __stdcall SND_fn_bCanFreeDataADPCM(void);

#ifdef DISABLE_MPEG
#define SND_fn_bCanFreeDataMPEG()   FALSE
#endif
#ifdef DISABLE_MIDI
#define SND_fn_bCanFreeDataMidi()   FALSE
#endif

#define SND_RT_fn_vDisplayError(uwErr, szMsg) \
    SND_fn_vDisplayError((uwErr), (char *)(szMsg))
#define SND_C_szEmpty                     ("")

int __cdecl SND_pc_fn_bLinkBinRefField(SND_tdstBlockEvent *pEvent2);

typedef struct SND_tdstSndBankDescriptorList_
{
    struct SND_pc_tdstBankDescElt_ *hFirstElement;
    struct SND_pc_tdstBankDescElt_ *hLastElement;
    long                            lNumberOfElements;
}
SND_tdstSndBankDescriptorList;

static SND_tdstSndBankDescriptorList SND_g_stBankDescriptionList;   /* 0x00514D50 */
#define SND_pc_g_pstBankDescHead  (SND_g_stBankDescriptionList.hFirstElement)
#define SND_pc_g_pstBankDescTail  (SND_g_stBankDescriptionList.hLastElement)
#define SND_M_SND_g_iBankNumber   (SND_g_stBankDescriptionList.lNumberOfElements)

typedef struct SND_pc_tdstBankDescElt_
{
    int gap_00;
    int lId;
    unsigned long ulLevel;
    char szFileName[13];
    long lBNMCheckVersion;
    int eState;
    struct _tdstSndBankHead *p_stData;
    void *pDataPrimary;
    void *pDataMPEG;
    void *pDataMidi;
    void *pDataSample;
    void *pDataADPCM;
    struct SND_pc_tdstBankDescElt_ *pNext;
    struct SND_pc_tdstBankDescElt_ *pPrev;
    SND_tdstSndBankDescriptorList  *pvAnchor;
}
SND_pc_tdstBankDescElt;

/* 0x004B74F0 */
static int SND_pc_g_iBankLoadProgress;
static int SND_pc_g_iBankLoadCount = 1;

unsigned int __cdecl GLI_AddToProgressBar(int lIncrement);

static int SND_pc_g_iBankEndFlag = 1;   /* 0x004A2528 */

/* 0x00453850 */
void __cdecl SND_pc_fn_vSetSoundFixedGlobal(double dValue)
{
    long lFixed = (long)(dValue * 65536.0);
    SND_pc_g_ul4A2438 = 0;
    SND_pc_g_lSoundFixedValue = lFixed;
}

/* 0x00453870 */
void SND_pc_fn_vBuildBankName(int lBankId, char *szOut, int iMaxChar)
{
    char szBuf[0x100];

    sprintf(szBuf, "bnk_%d.BNM", lBankId);
    strncpy(szOut, szBuf, iMaxChar);
}

/* 0x004538C0 */
SND_pc_tdstBankDescElt *__cdecl SND_fn_pStGetBankDescriptor(long lId)
{
    SND_pc_tdstBankDescElt *pElt;

    pElt = SND_pc_g_pstBankDescHead;
    while (pElt != NULL)
    {
        if (pElt->lId == lId)
        {
            return pElt;
        }
        pElt = pElt->pNext;
    }

    return NULL;
}

/* 0x004538E0 */
SND_pc_tdstBankDescElt *SND_pc_fn_pstCreateBankDescriptor(int lBankId, int lLevel)
{
    SND_pc_tdstBankDescElt *pElt;

    pElt = (SND_pc_tdstBankDescElt *)SND_fn_pvMallocSnd(0x48);
    memset(pElt, 0, 0x48);
    pElt->pPrev = NULL;
    pElt->pvAnchor = NULL;
    pElt->pNext = NULL;
    pElt->pPrev = SND_pc_g_pstBankDescTail;
    pElt->pvAnchor = &SND_g_stBankDescriptionList;
    if (SND_pc_g_pstBankDescHead == NULL)
        SND_pc_g_pstBankDescHead = pElt;
    else
        SND_pc_g_pstBankDescTail->pNext = pElt;
    SND_M_SND_g_iBankNumber = SND_M_SND_g_iBankNumber + 1;
    SND_pc_g_pstBankDescTail = pElt;
    pElt->lId = lBankId;
    SND_pc_fn_vBuildBankName(lBankId, pElt->szFileName, 13);
    pElt->eState = 0;
    pElt->p_stData = NULL;
    pElt->pDataPrimary = NULL;
    pElt->pDataMPEG = NULL;
    pElt->pDataMidi = NULL;
    pElt->pDataSample = NULL;
    if (lLevel == -1)
        pElt->ulLevel = 0xFFFFFFFFUL;
    else
        pElt->ulLevel = 1UL << lLevel;
    return pElt;
}

int __cdecl SND_pc_fn_bLoadBankBnmFile(SND_pc_tdstBankDescElt *pElt)
{
    tdhSndFile hFile;
    tdstSndBankHead stHead;
    char szPath[256];
    unsigned long ulSec0;
    unsigned long ulSzMpeg;
    unsigned long ulSzMidi;
    unsigned long ulSzSample;
    unsigned long ulSzAdpcm;
    unsigned long ulAcc;
    int iIdx;
    int iVar2;

    pElt->eState = 2;
    SND_fn_vResolveFileName((char *)pElt->szFileName, szPath);
    hFile = SND_fn_hOpenFileReadSnd(szPath);
    if (hFile == OPENFILESND_FAILED)
    {
        SND_fn_vDisplayError(10, szPath);
        return 0;
    }
    /* 0x00453990 */
    SND_fn_dwReadFileSnd(hFile, sizeof(tdstSndBankHead), &stHead);
    pElt->lBNMCheckVersion = (long)stHead.stEventList.hFirstElementOpt;
    ulSzMpeg = stHead.ulOffsetMidi - stHead.ulOffsetMPEG;
    ulSzMidi = stHead.ulOffsetSample - stHead.ulOffsetMidi;
    ulSzSample = stHead.ulOffsetADPCM - stHead.ulOffsetSample;
    if ((long)stHead.stEventList.hFirstElementOpt == 0x24)
    {
        ulSzAdpcm = 0;
    }
    else
    {
        ulSzAdpcm = stHead.ulOffsetStreaming - stHead.ulOffsetADPCM;
    }
    ulSec0 = stHead.ulOffsetMPEG;
    iVar2 = SND_fn_bCanFreeDataMPEG();
    if (iVar2 == 0)
    {
        ulSec0 = ulSec0 + ulSzMpeg;
    }
    iVar2 = SND_fn_bCanFreeDataMidi();
    if (iVar2 == 0)
    {
        ulSec0 = ulSec0 + ulSzMidi;
    }
    iVar2 = SND_fn_bCanFreeDataSample();
    if (iVar2 == 0)
    {
        ulSec0 = ulSec0 + ulSzSample;
    }
    iVar2 = SND_fn_bCanFreeDataADPCM();
    if (iVar2 == 0)
    {
        ulSec0 = ulSec0 + ulSzAdpcm;
    }
    pElt->p_stData = (tdstSndBankHead *)SND_fn_pvMallocSnd(ulSec0);
    *pElt->p_stData = stHead;

    SND_fn_dwReadFileSnd(hFile, stHead.ulOffsetMPEG - sizeof(tdstSndBankHead), pElt->p_stData + 1);
    pElt->pDataPrimary = pElt->p_stData + 1;

    ulAcc = stHead.ulOffsetMPEG;
    if (!SND_fn_bCanFreeDataMPEG())
    {
        pElt->pDataMPEG = (char *)pElt->p_stData + ulAcc;
        ulAcc += ulSzMpeg;
    }
    else if (ulSzMpeg != 0)
    {
        pElt->pDataMPEG = SND_fn_pvMallocSnd(ulSzMpeg);
    }
    else
    {
        pElt->pDataMPEG = NULL;
    }
    if (ulSzMpeg != 0)
    {
        SND_fn_dwReadFileSnd(hFile, ulSzMpeg, pElt->pDataMPEG);
    }

    if (!SND_fn_bCanFreeDataMidi())
    {
        pElt->pDataMidi = (unsigned char *)pElt->p_stData + ulAcc;
        ulAcc += ulSzMidi;
    }
    else if (ulSzMidi != 0)
    {
        pElt->pDataMidi = SND_fn_pvMallocSnd(ulSzMidi);
    }
    else
    {
        pElt->pDataMidi = NULL;
    }
    if (ulSzMidi != 0)
    {
        SND_fn_dwReadFileSnd(hFile, ulSzMidi, pElt->pDataMidi);
    }

    if (!SND_fn_bCanFreeDataSample())
    {
        pElt->pDataSample = (unsigned char *)pElt->p_stData + ulAcc;
        ulAcc += ulSzSample;
    }
    else if (ulSzSample != 0)
    {
        pElt->pDataSample = SND_fn_pvMallocSnd(ulSzSample);
    }
    else
    {
        pElt->pDataSample = NULL;
    }
    if (ulSzSample != 0)
    {
        SND_fn_dwReadFileSnd(hFile, ulSzSample, pElt->pDataSample);
    }

    if (!SND_fn_bCanFreeDataADPCM())
    {
        pElt->pDataADPCM = (unsigned char *)pElt->p_stData + ulAcc;
    }
    else if (ulSzAdpcm != 0)
    {
        pElt->pDataADPCM = SND_fn_pvMallocSnd(ulSzAdpcm);
    }
    else
    {
        pElt->pDataADPCM = NULL;
    }
    if (ulSzAdpcm != 0)
    {
        SND_fn_dwReadFileSnd(hFile, ulSzAdpcm, pElt->pDataADPCM);
    }

    SND_fn_vCloseFileSnd(hFile);

    pElt->p_stData->stEventList.hFirstElementOpt =
        (SND_tdstBlockEvent *)((char *)pElt->p_stData->stEventList.hFirstElementOpt + (long)pElt->p_stData);
    pElt->p_stData->stResList.hFirstElementOpt =
        (tduBlockResource *)((char *)pElt->p_stData->stResList.hFirstElementOpt + (long)pElt->p_stData);

    pElt->pDataPrimary = pElt->p_stData->stResList.hFirstElementOpt + pElt->p_stData->stResList.lNumberOfElementsOpt;
    return 1;
}

int __cdecl SND_fn_bEndInitBank(SND_pc_tdstBankDescElt *pElt)
{
    tdstSndBankHead *pBankHead;
    SND_tdstBlockEvent *pEvent2;
	int iPos;
	tduBlockResource        *pRes;
	tdstBlockResourceDisk   stBRDisk;
	int iPos2;
    char *pDataBloc;
    int iRet;
    int bValid;

    pBankHead = pElt->p_stData;
    pDataBloc = NULL;
    iRet = 1;

	SNDLST2_M_StaticForEachElementOf( &pBankHead->stEventList, pEvent2, iPos2)
	{
		SND_pc_fn_bLinkBinRefField(pEvent2);
	}

	SNDLST2_M_StaticForEachElementOf( &pElt->p_stData->stResList, pRes, iPos)
	{

		memcpy(&stBRDisk, pRes, sizeof(tdstBlockResourceDisk));

		bValid = TRUE;
		switch (stBRDisk.eType)
		{
		case TYPE_SAMPLE:
			if (stBRDisk.uRes.stSample.bStream)
			{
				pDataBloc = NULL;
			}
			else
			{
				switch (stBRDisk.uRes.stSample.eZip)
				{
				case SAMPLE_PCM:
					pDataBloc = pElt->pDataSample;
					break;
				case SAMPLE_MPEG:
					pDataBloc = pElt->pDataMPEG;
					break;
				case SAMPLE_ADPCM:
					pDataBloc = pElt->pDataADPCM;
					break;
				default:
					SND_fn_vDisplayError(E_uwSndInvalidStaticParamForResource, "");
					break;
				}
			}
			break;
		case TYPE_MIDI:
			pDataBloc = pElt->pDataMidi;
			break;
		case TYPE_CDAUDIO:
		case TYPE_SEQUENCE:
		case TYPE_SWITCH_OLD:
		case TYPE_SPLIT:
		case TYPE_SWITCH:
		case TYPE_RANDOM:
		case TYPE_THEME:
			pDataBloc = pElt->pDataPrimary;
			break;
		default:
			SND_fn_vDisplayError(E_uwSndInvalidTypeOfResource, "");
			bValid = FALSE;
			pRes->stDisk.eType = TYPE_INVALID;
			iRet = FALSE;
			break;
		}

		if (bValid)
		{
			SND_fn_bLoadResBinarySnd(&stBRDisk, &pRes->stMem, pDataBloc);
		}
	}

    if (SND_fn_bCanFreeDataMPEG() && pElt->pDataMPEG != NULL)
    {
        SND_fn_vFreeSnd(pElt->pDataMPEG);
    }
    pElt->pDataMPEG = NULL;
    if (SND_fn_bCanFreeDataMidi() && pElt->pDataMidi != NULL)
    {
        SND_fn_vFreeSnd(pElt->pDataMidi);
    }
    pElt->pDataMidi = NULL;
    if (SND_fn_bCanFreeDataSample() && pElt->pDataSample != NULL)
    {
        SND_fn_vFreeSnd(pElt->pDataSample);
    }
    pElt->pDataSample = NULL;
    if (SND_fn_bCanFreeDataADPCM() && pElt->pDataADPCM != NULL)
    {
        SND_fn_vFreeSnd(pElt->pDataADPCM);
    }
    pElt->pDataADPCM = NULL;
    pElt->pDataPrimary = NULL;
    return iRet;
}

int __cdecl SND_pc_fn_bLinkBinRefField(SND_tdstBlockEvent *pEvent2)
{
    switch (pEvent2->eType)
    {
        case EVT_SON_PLAY:
            if (pEvent2->uParams.stPlay.uResource.Id != SND_INVALID_BINID)
            {
                pEvent2->uParams.stPlay.uResource.pstPtr = SND_fn_pGetBinRes(pEvent2->uParams.stPlay.uResource.Id);
                return 1;
            }
            break;
        case EVT_SON_STOP:
            if (pEvent2->uParams.stStop.uEvt.lIndex != (long)SND_INVALID_BINID)
            {
                pEvent2->uParams.stStop.uEvt.pstPtr = SND_fn_pGetBinEvent(pEvent2->uParams.stStop.uEvt.lIndex);
                return 1;
            }
            break;
        case EVT_SON_STOP_ALL:
        case EVT_SON_EFFET:
        case EVT_SON_CHANGE_VOLUME:
            return 1;
        case EVT_SON_STOP_N_GO:
        case EVT_SON_STOP_N_GO_CROSSFADE:
            if (pEvent2->uParams.stStopNGo.uEvtStop.lIndex != (long)SND_INVALID_BINID)
                pEvent2->uParams.stStopNGo.uEvtStop.pstPtr = SND_fn_pGetBinEvent(pEvent2->uParams.stStopNGo.uEvtStop.lIndex);
            else
                pEvent2->uParams.stStopNGo.uEvtStop.pstPtr = NULL;
            if (pEvent2->uParams.stStopNGo.uEvtGo.lIndex != (long)SND_INVALID_BINID)
            {
                pEvent2->uParams.stStopNGo.uEvtGo.pstPtr = SND_fn_pGetBinEvent(pEvent2->uParams.stStopNGo.uEvtGo.lIndex);
                return 1;
            }
            pEvent2->uParams.stStopNGo.uEvtGo.pstPtr = NULL;
            return 1;
        case EVT_SON_PITCH:
        case EVT_SON_VOLUME:
        case EVT_SON_PAN:
            if (pEvent2->uParams.stExtraParam.uResource.Id != SND_INVALID_BINID)
            {
                pEvent2->uParams.stExtraParam.uResource.pstPtr = SND_fn_pGetBinRes(pEvent2->uParams.stExtraParam.uResource.Id);
                return 1;
            }
            break;
        case EVT_SON_EXTRA:
            if (pEvent2->uParams.stExtraAll.uResource.Id != SND_INVALID_BINID)
            {
                pEvent2->uParams.stExtraAll.uResource.pstPtr = SND_fn_pGetBinRes(pEvent2->uParams.stExtraAll.uResource.Id);
                return 1;
            }
            break;
        default:
            SND_RT_fn_vDisplayError(0x13, SND_C_szEmpty);
            return 0;
    }

    pEvent2->uParams.stPlay.uResource.pstPtr = NULL;
    return 1;
}

int SND_CALL SND_fn_iGetNumberOfBanks(void)
{
    return SND_M_SND_g_iBankNumber;
}

int SND_CALL SND_fn_iGetNumberOfEventInBank(long lBankId)
{
    SND_pc_tdstBankDescElt *pBankdesc;

    pBankdesc = SND_fn_pStGetBankDescriptor(lBankId);
    if (pBankdesc != NULL)
    {
        return pBankdesc->p_stData->stEventList.lNumberOfElementsOpt;
    }
    return 0;
}

void SND_CALL SND_fn_vInitBankSnd(char *czPathName)
{
    SND_pc_g_pstBankDescHead = NULL;
    SND_pc_g_pstBankDescTail = NULL;
    SND_M_SND_g_iBankNumber = 0;
}

void SND_CALL SND_fn_vDesInitBankSnd(void)
{
    SND_pc_tdstBankDescElt *pElt;
    SND_pc_tdstBankDescElt *pNext;

    for (pElt = SND_pc_g_pstBankDescHead, pNext = (pElt != NULL) ? pElt->pNext : NULL;
         pElt != NULL;
         pElt = pNext, pNext = (pNext != NULL) ? pNext->pNext : NULL)
    {
        SND_fn_bUnLoadBinaryBank(pElt->lId);
    }
}

SND_tdstBlockEvent *SND_CALL SND_fn_pGetBinEvent(unsigned long ulBinId)
{
    unsigned long ulEventPos;
    tdstSndBankHead *pBank;
    SND_tdstBlockEvent *pEvent;
    SND_pc_tdstBankDescElt *pBankdesc;
    int i;

    if (ulBinId == SND_INVALID_BINID)
        return NULL;

    ulEventPos = ulBinId & 0xFFFF;

    pBankdesc = SND_fn_pStGetBankDescriptor(ulBinId >> 16);
    if (pBankdesc != NULL)
    {
        pBank = pBankdesc->p_stData;
        if ((long)ulEventPos < SNDLST2_M_StaticGetNumberOfElements(&pBank->stEventList))
        {
            SNDLST2_M_StaticGetElementNumber(&pBank->stEventList, pEvent, (long)ulEventPos, i);
            return(pEvent);
        }
    }
    return NULL;
}

tdstBlockResourceMem *SND_CALL SND_fn_pGetBinRes(unsigned long _ulBinId)
{
    unsigned long ulResPos;
    tdstSndBankHead *pBank;
    tduBlockResource *pRes;
    SND_pc_tdstBankDescElt *pBankdesc;
    int i;

    if (_ulBinId == SND_INVALID_BINID)
        return NULL;

    ulResPos = _ulBinId&0xFFFF;

    pBankdesc = SND_fn_pStGetBankDescriptor(_ulBinId>>16);
    if (pBankdesc != NULL)
    {
        /* 0x453FB0 */
        pBank=pBankdesc->p_stData;
        if ((long)ulResPos < SNDLST2_M_StaticGetNumberOfElements(&pBank->stResList))
        {
            SNDLST2_M_StaticGetElementNumber(&pBank->stResList, pRes, (long)ulResPos, i);
            return(&pRes->stMem);
        }
    }
    return NULL;
}

SndBool SND_CALL SND_fn_bStartLoadBanks(void)
{
    return SND_fn_bStartLoadBanks2(-1);
}

SndBool SND_CALL SND_fn_bStartLoadBanks2(int level)
{
    SND_pc_tdstBankDescElt *pElt;

    SND_pc_g_iBankLoadProgress = 10;
    SND_pc_g_iBankLoadCount = 0;
    for (pElt = SND_pc_g_pstBankDescHead; pElt != NULL; pElt = pElt->pNext)
    {
        if (level == -1)
            pElt->ulLevel = 0;
        else
            pElt->ulLevel = pElt->ulLevel & ~(1 << level);
        if ((pElt->ulLevel == 0) && (pElt->gap_00 == 0))
        {
            pElt->eState = 3;
        }
    }
    return 1;
}

SndBool SND_CALL SND_fn_bInitBank(int _iBankNum)
{
    return SND_fn_bInitBank2(_iBankNum, -1);
}

SndBool SND_CALL SND_fn_bInitBank2(int _iBankNum, int level)
{
    SND_pc_tdstBankDescElt *pElt;
    int iState;

    SND_pc_g_iBankLoadCount = SND_pc_g_iBankLoadCount + 1;
    pElt = SND_fn_pStGetBankDescriptor(_iBankNum);
    if (pElt == NULL)
    {
        pElt = SND_pc_fn_pstCreateBankDescriptor(_iBankNum, level);
    }
    else if (level == -1)
    {
        pElt->ulLevel = 0xFFFFFFFFUL;
    }
    else
    {
        pElt->ulLevel = pElt->ulLevel & (1 << level);
    }
    (void)iState;
    switch (pElt->eState)
    {
    case 0:
        pElt->eState = 4;
        break;
    case 1:
    case 3:
        pElt->eState = 1;
        break;
    }
    return 1;
}

SndBool SND_CALL SND_fn_bEndLoadBanks(void)
{
    SND_pc_tdstBankDescElt *pNext;
    SND_pc_tdstBankDescElt *pCur;
    SND_pc_tdstBankDescElt *pPrevCur;
    int iVar2;

    long lProgressSum = 0;
    (void)pPrevCur;
    for (pCur = SND_pc_g_pstBankDescHead, pNext = (pCur != NULL) ? pCur->pNext : NULL;
         pCur != NULL;
         pCur = pNext, pNext = (pNext != NULL) ? pNext->pNext : NULL)
    {
        switch (pCur->eState)
        {
        case 1:
            iVar2 = SND_pc_g_iBankLoadProgress / SND_pc_g_iBankLoadCount;
            GLI_AddToProgressBar(iVar2);
            lProgressSum += iVar2;
            SND_pc_g_iBankLoadProgress = SND_pc_g_iBankLoadProgress + (10 - iVar2 * SND_pc_g_iBankLoadCount);
            break;
        case 3:
            SND_fn_bUnLoadBinaryBank(pCur->lId);
            break;
        case 4:
            iVar2 = SND_pc_fn_bLoadBankBnmFile(pCur);
            if (iVar2 == 0)
            {
                SND_fn_bUnLoadBinaryBank(pCur->lId);
            }
            else
            {
                pCur->eState = 2;
            }
            if (SND_pc_g_iBankEndFlag != 0)
            {
                pCur->gap_00 = 1;
            }
            else
            {
                iVar2 = SND_pc_g_iBankLoadProgress / SND_pc_g_iBankLoadCount;
                GLI_AddToProgressBar(iVar2);
                lProgressSum += iVar2;
                SND_pc_g_iBankLoadProgress = SND_pc_g_iBankLoadProgress + (10 - iVar2 * SND_pc_g_iBankLoadCount);
            }
            break;
        }
    }
    SND_pc_g_iBankEndFlag = 0;
    for (pNext = SND_pc_g_pstBankDescHead; pNext != NULL; pNext = pNext->pNext)
    {
        switch (pNext->eState)
        {
        case 1:
            break;
        case 2:
            SND_fn_bEndInitBank(pNext);
            pNext->eState = 1;
            break;
        }
    }
    return 1;
}

SndBool SND_CALL SND_fn_bLoadBinaryBank(int _iBankNum)
{
    SND_pc_tdstBankDescElt *pElt;
    int iVar2;

    pElt = SND_fn_pStGetBankDescriptor((long)_iBankNum);
    if (pElt == NULL)
    {
        pElt = SND_pc_fn_pstCreateBankDescriptor(_iBankNum, -1);
    }
    if (pElt->eState != 1)
    {
        pElt->eState = 4;
        iVar2 = SND_pc_fn_bLoadBankBnmFile(pElt);
        if (iVar2 != 0)
        {
            pElt->eState = 2;
            iVar2 = SND_fn_bEndInitBank(pElt);
            if (iVar2 != 0)
            {
                pElt->eState = 1;
                return 1;
            }
        }
        SND_fn_bUnLoadBinaryBank(_iBankNum);
        return 0;
    }
    return 1;
}

SndBool SND_CALL SND_fn_bUnLoadBinaryBank(int _iBankNum)
{
    SND_pc_tdstBankDescElt *pElt;
    tduBlockResource *pRes;
    int iPos;

    pElt = SND_fn_pStGetBankDescriptor(_iBankNum);
    if (pElt == NULL)
        return 0;

    if (pElt->p_stData != NULL)
    {
        SNDLST2_M_StaticForEachElementOf( &pElt->p_stData->stResList, pRes, iPos)
        {
            SND_fn_vStopBeforeUnLoadResSnd(&pRes->stMem);
        }
        SNDLST2_M_StaticForEachElementOf( &pElt->p_stData->stResList, pRes, iPos)
        {
            SND_fn_vUnLoadResSnd(&pRes->stMem);
        }
        SND_fn_vFreeSnd(pElt->p_stData);
    }

    if (pElt->pNext != NULL)
        pElt->pNext->pPrev = pElt->pPrev;
    if (pElt->pPrev != NULL)
        pElt->pPrev->pNext = pElt->pNext;

    if (pElt->pvAnchor != NULL)
    {
        pElt->pvAnchor->lNumberOfElements--;
        if (pElt->pvAnchor->hFirstElement == pElt)
            pElt->pvAnchor->hFirstElement = pElt->pNext;
        if (pElt->pvAnchor->hLastElement == pElt)
            pElt->pvAnchor->hLastElement = pElt->pPrev;
    }

    pElt->pNext = NULL;
    pElt->pPrev = NULL;
    pElt->pvAnchor = NULL;
    SND_fn_vFreeSnd(pElt);
    return 1;
}

/* 0x00454370 */
int __stdcall SND_fn_bCanSetOptions(void)
{
    return 1;
}

/* 0x00454380 */
tdstBlockResourceMem *__cdecl SND_pc_fn_pstFindBankItemById(long lKey)
{
    SND_pc_tdstBankDescElt *pElt;
    tdstSndBankHead *pHead;
    tduBlockResource *pRes;
    long i;

    pElt = SND_pc_g_pstBankDescHead;
    while (pElt != NULL)
    {
        pHead = pElt->p_stData;
        SNDLST2_M_StaticForEachElementOf(&pHead->stResList, pRes, i)
        {
            if (pRes->stMem.Id == lKey)
            {
                return &pRes->stMem;
            }
        }
        pElt = pElt->pNext;
    }

    return NULL;
}
