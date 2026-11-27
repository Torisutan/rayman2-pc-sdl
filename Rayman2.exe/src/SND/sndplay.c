#include <stddef.h>
#include <string.h>
#include <windows.h>
#include "SND/sndres.h"
#include "SND/sndparam_pc.h"

#include "SND/snderr.h"
#define SND_M_DisplayError(err_num,message) SND_fn_vDisplayError(err_num,"")

#include "SND/sndplay_pc.h"

struct SoundParam_;
void __stdcall SND_fn_vDisplayError(unsigned short uwErrNum, char *message);
void *__stdcall SND_fn_pGetBinRes(unsigned long _ulBinId);
long __cdecl pre_associe_snd(void);
long __cdecl associe_snd(long id_asso, long voice_sxd, tdstBlockResourceMem *res, long prio, int *valid, long transition);
void SND_CALL SND_fn_vReleaseDriverCD(void);
void SND_CALL SND_fn_vReleaseDriverMidi(void);
void SND_CALL SND_fn_vReleaseDriverSxd(void);
void SND_CALL SND_fn_vRestoreDriverCD(void);
void SND_CALL SND_fn_vRestoreDriverMidi(void);
void SND_CALL SND_fn_vRestoreDriverSxd(void);
void __stdcall SND_fn_vStopTransitionSxd(long voice);
void __stdcall SND_fn_vStopTransitionMidi(long voice);
void __stdcall SND_fn_vPauseTransitionSxd(long voice);
void __stdcall SND_fn_vResumeTransitionSxd(long voice);
void __stdcall SND_fn_vPauseTransitionMidi(long voice);
void __stdcall SND_fn_vResumeTransitionMidi(long voice);
SndReal __stdcall SND_fn_rMulRealRealSnd(SndReal r1,SndReal r2);
SndReal __stdcall SND_fn_rDivRealRealSnd(SndReal r1,SndReal r2);
void __stdcall SND_fn_vGetDefaultRollOff(RollOffParam *rolloff);
void __stdcall SND_fn_vSetDopplerFactor(SndReal factor);
void __stdcall SND_fn_vDisplayErrorEx(unsigned short uwErrNum, char *message, int flags);
void __stdcall SND_fn_vFreeSndEx(unsigned char id_block, void *pvPtr);
void *__stdcall SND_fn_pvMallocSndEx(unsigned char id_block, unsigned int ulNbBytes);
SndBool __stdcall SND_fn_bGetStereoSxd(void);
void __stdcall SND_fn_vSetStereoSxd(SndBool active);
void __stdcall SND_fn_vSetEffectSxd(long num);
void __stdcall SND_fn_vSetMicroParamSxd(long sndId, MicroParam *par);
long __stdcall SND_fn_lCreateMicroSxd(MicroParam *par);
int __stdcall SND_fn_iInitSxd(void *pInitStruct);
int __stdcall SND_fn_iInitMidi(void *pInitStruct);
int __stdcall SND_fn_iInitCD(void *pInitStruct);
int __stdcall SND_fn_iInitRecord(void *pInitStruct);
int SND_fn_iInitSeq(void);
void SND_fn_vInitTheme(void);
void __stdcall SND_fn_vDesInitSxd(void);
void __stdcall SND_fn_vDesInitMidi(void);
void __stdcall SND_fn_vDesInitCD(void);
void __stdcall SND_fn_vDesInitRecord(void);
void SND_fn_vDesInitSeq(void);
void __cdecl SND_fn_vDesInitTheme(void);
void __stdcall SND_fn_vSetDefaultRollOffSxd(RollOffParam *rolloff);
void __stdcall SND_fn_vSetDopplerFactorSxd(long factor);
void __stdcall SND_fn_vSynchroSxd(void);
void __stdcall SND_fn_vSynchroMidi(void);
void __stdcall SND_fn_vSynchroCD(void);
void __stdcall SND_fn_vUnLoadResSxd(tdstBlockResourceMem *mem);
void __stdcall SND_fn_vUnLoadResMidi(tdstBlockResourceMem *mem);
void __cdecl SND_fn_vUnLoadResTheme(tdstBlockResourceMem *mem);
SndBool __stdcall SND_fn_bLoadResBinarySxd(tdstBlockResourceDisk *disk,tdstBlockResourceMem *mem,char *pDataBloc);
SndBool __stdcall SND_fn_bLoadResBinaryMidi(tdstBlockResourceDisk *disk,tdstBlockResourceMem *mem,char *pDataBloc);
SndBool __stdcall SND_fn_bLoadResBinaryCd(tdstBlockResourceDisk *disk,tdstBlockResourceMem *mem,char *pDataBloc);
SndBool __cdecl SND_fn_bLoadResBinarySeq(tdstBlockResourceDisk *_pBRDisk,tdstBlockResourceMem *_pBRMem,char *pDataBloc);
SndBool __cdecl SND_fn_bLoadResBinarySwitch(tdstBlockResourceDisk *_pBRDisk,tdstBlockResourceMem *_pBRMem,char *pDataBloc);
SndBool __cdecl SND_fn_bLoadResBinarySwitchOld(tdstBlockResourceDisk *_pBRDisk,tdstBlockResourceMem *_pBRMem,char *pDataBloc);
SndBool __cdecl SND_fn_bLoadResBinaryRandom(tdstBlockResourceDisk *_pBRDisk,tdstBlockResourceMem *_pBRMem,char *pDataBloc);
SndBool __cdecl SND_fn_bLoadResBinaryTheme(tdstBlockResourceDisk *_pBRDisk,tdstBlockResourceMem *_pBRMem,char *pDataBloc);
SndBool __cdecl SND_pc_fn_bLoadResRatioStride36(tdstBlockResourceDisk *_pBRDisk,tdstBlockResourceMem *_pBRMem,char *pDataBloc);
SndBool __stdcall SND_fn_bSetParamTransitionSxd(long voice, struct SoundParam_ *par);
SndBool __stdcall SND_fn_bSetParamTransitionMidi(long voice, struct SoundParam_ *par);
SndBool __stdcall SND_fn_bSetResourceStaticVolumeSxd(long a, long b);
SndBool __stdcall SND_fn_bSetResourceStaticVolumeMidi(long a, long b);
SndBool __stdcall SND_fn_bSetResourceStaticVolumeCD(long a, long b);
int __stdcall SND_pc_fn_iSetNodeByteStd(tdstBlockResourceMem *pRes, unsigned char ucVolume);
int __cdecl SND_pc_fn_iSetSequenceNodeByte(tdstBlockResourceMem *pRes, unsigned char ucVolume);
int __cdecl SND_pc_fn_iSetSplitNodeByte(tdstBlockResourceMem *pRes, unsigned char ucVolume);
int __cdecl SND_pc_fn_iSetSwitchNodeByte(tdstBlockResourceMem *pRes, unsigned char ucVolume);
int __cdecl SND_pc_fn_iSetThemeNodeByte(tdstBlockResourceMem *pRes, unsigned char ucVolume);
void __cdecl SND_fn_vStopTransitionSnd(long voice);
void __cdecl SND_fn_vPauseTransitionSnd(long voice);
void __cdecl SND_fn_vResumeTransitionSnd(long voice);
void __stdcall SND_fn_vPauseSxd(long voice_sxd);
void __stdcall SND_fn_vResumeSxd(long voice_sxd);
int  __stdcall SND_fn_bSetParamSxd (long voice_sxd, struct SoundParam_ *par);
void __stdcall SND_fn_vStopSxd(long voice_sxd);
long __stdcall SND_fn_rGetPosSxd   (long voice_sxd);
int  __stdcall SND_fn_bTestIsPlayingSxd(long voice_sxd);
void __stdcall SND_fn_vPauseMidi(long voice_sxd);
void __stdcall SND_fn_vResumeMidi(long voice_sxd);
int  __stdcall SND_fn_bSetParamMidi(long voice_sxd, struct SoundParam_ *par);
void __stdcall SND_fn_vStopMidi(long voice_sxd);
long __stdcall SND_fn_rGetPosMidi  (long voice_sxd);
int  __stdcall SND_fn_bTestIsPlayingMidi(long voice_sxd);
void __stdcall SND_fn_vPauseCD(long voice_sxd);
void __stdcall SND_fn_vResumeCD(long voice_sxd);
int  __stdcall SND_fn_bSetParamCD  (long voice_sxd, struct SoundParam_ *par);
void __stdcall SND_fn_vStopCD(long voice_sxd);
long __stdcall SND_fn_rGetPosCD    (long voice_sxd);
int  __stdcall SND_fn_bTestIsPlayingCD(long voice_sxd);
void __stdcall SND_fn_vPauseSeq(long voice);
void __stdcall SND_fn_vResumeSeq(long voice);
void __stdcall SND_fn_vPauseSwitch(long voice);
void __stdcall SND_fn_vResumeSwitch(long voice);
int  __stdcall SND_fn_bSetParamSwitch(long voice, struct SoundParam_ *par);
void __stdcall SND_fn_vStopSwitch(long voice);
long __stdcall SND_fn_rGetPosSwitch(long voice);
int  __stdcall SND_fn_bTestIsPlayingSwitch(long voice);
void __stdcall SND_fn_vPauseRandom(long voice);
void __stdcall SND_fn_vResumeRandom(long voice);
void __stdcall SND_fn_vStopRandom(long voice);
int  __stdcall SND_fn_bTestIsPlayingRandom(long voice);
long __stdcall SND_fn_rGetPosRandom(long voice);
void __stdcall SND_fn_vPauseTheme(long canal);
void __stdcall SND_fn_vResumeTheme(long canal);
int  __stdcall SND_fn_bTestIsPlayingTheme(long canal);
long __stdcall SND_fn_rGetPosTheme(long voie);   /* 0x00459C80 */
void __stdcall SND_pc_fn_vNopStd4(long a);
long __stdcall SND_fn_lPlayTransitionMidi(tduRefRes FirstRes, tduRefRes NextRes,
                                          SND_td_pfn_vSoundCallback transition_callback,
                                          long param_callback, struct SoundParam_ *par);
long __stdcall SND_fn_lPlayTransitionExSxd(tduRefRes FirstRes, tduRefRes NextRes,
                                           SND_td_pfn_vSoundCallback transition_callback,
                                           long param_callback, struct SoundParam_ *par);
SndBool __stdcall SND_fn_bSetNextTransitionMidi(long voice, tduRefRes new_res);
SndBool __stdcall SND_fn_bSetNextTransitionSxd(long voice, tduRefRes new_res);
SndBool __stdcall SND_fn_bDoTransitionWithFadeMidi(long voice, tduRefRes new_res);
SndBool __stdcall SND_fn_bDoTransitionWithFadeSxd(long voice, tduRefRes new_res);

tduRefRes SND_RES_FANTOME = {0};

long SND_fn_lPlaySnd(tduRefRes res,struct SoundParam_* par,long prio,SND_td_pfn_vSoundCallback fn_callback,long par_callback,SndBool* valid);
long __stdcall SND_fn_lPlaySxd(long a1, void *a2, long a3, void *a4, long a5);
long __stdcall SND_fn_lPlayMidi(long a1, void *a2, long a3, void *a4, long a5);
long __stdcall SND_pc_fn_lPlayCD(long a1, void *a2, long a3, void *a4, long a5);
long SND_fn_lPlaySeq(tduRefRes res,struct SoundParam_ *par,long prio,SND_td_pfn_vSoundCallback fn_callback,long par_callback,SndBool *valid);
long SND_fn_lPlaySwitch(tduRefRes num_res,struct SoundParam_ *par,long prio,void *fn_callback,long par_callback);
long SND_fn_lPlayRandom(tduRefRes num_res,struct SoundParam_ *par,long prio,void *fn_callback,long par_callback);
long SND_fn_lPlayTheme(long canal,tduRefRes theme_res,struct SoundParam_ *par,long prio,void *fn_callback,long par_callback,SndBool *valid);

typedef struct SND_tdstIndexedInstance_ *pSND_tdstIndexedInstance;

SNDLST2_M_DynamicUseListOf(pSND_tdstIndexedInstance);

typedef struct SND_tdstIndexedInstance_
{
    long                            lVoice;
    void                           *pZone;
    long                            bParamActive;
    SNDLST2_M_DynamicElementDeclaration(pSND_tdstIndexedInstance)
} SND_tdstIndexedInstance;

typedef struct SND_tdstIndexedRequest_
{
    tdstBlockResourceMem *pstSplitRes;
    void                 *pParam1;
    SndBool              *pbValid;
    long                  lState;
    SNDLST2_M_DynamicAnchorTo(pSND_tdstIndexedInstance) anchor;
} SND_tdstIndexedRequest;

#define SND_C_NB_INDEXED_REQUESTS  0x20
static SND_tdstIndexedRequest SND_pc_g_aIndexedRequests[SND_C_NB_INDEXED_REQUESTS];   /* 0x004B8918 */

typedef void SND_CALL td_fn_vPauseFunc(long);
typedef void SND_CALL td_fn_vResumeFunc(long);
typedef SndBool SND_CALL td_fn_bSetParamFunc(long,SoundParam*);
typedef void SND_CALL td_fn_vStopFunc(long);
typedef SndReal SND_CALL td_fn_rGetPosFunc(long);
typedef SndBool SND_CALL td_fn_bTestIsPlayingFunc(long);

typedef struct _td_st_Func
{
	td_fn_vPauseFunc *pfnPause;
	td_fn_vResumeFunc *pfnResume;
	td_fn_bSetParamFunc *pfnSetParam;
	td_fn_vStopFunc *pfnStop;
	td_fn_rGetPosFunc *pfnGetPos;
	td_fn_bTestIsPlayingFunc *pfnTestIsPlaying;
} td_st_Func;

static td_st_Func a_fnFunc[NB_TYPES_RESOURCES];

#define NB_VOICE_MAX                  64

static voice_description voice_snd[NB_VOICE_MAX];   /* 0x004B8C98 */

static int g_lCurrentGlobalPauseInstance = 1;   /* 0x004A3CC8 */

static SndBool g_iInitSxd;
static SndBool g_iInitMidi;
static SndBool g_iInitCD;

#define E_ucSndBlockMain      ((unsigned char)0)

#define SND_RT_fn_vDisplayError(uwErr, szMsg) \
    SND_fn_vDisplayError((uwErr), (char *)(szMsg))

#define SND_C_szEmpty                     ("")

SndBool __stdcall SND_fn_bSetParamSnd(long voie, struct SoundParam_ *par);
void __stdcall SND_fn_vStopSnd(long voie);

#define SND_C_NB_ZONE_MAX		10

void __cdecl SND_fn_vCopySoundParam(struct SoundParam_ *pDest,
                                    struct SoundParam_ *pSrc)
{
    memcpy(pDest, pSrc, SND_M_lGetSizeOfSoundParam(pSrc));
}

long __cdecl pre_associe_snd(void)
{
    int i;

    for (i = 0; i < NB_VOICE_MAX; i++)
    {
        if (voice_snd[i].bFree)
            break;
    }

    if (i == NB_VOICE_MAX)
    {
        SND_fn_vDisplayErrorEx(E_uwSndTooManyVoices, (char *)SND_C_szEmpty,
                               SNDERR_DISPLAY_IF_WANTED);
        return C_PLAY_FAILED;
    }

    voice_snd[i].bFree                = 0;
    voice_snd[i].bAssociation_running = 1;
    voice_snd[i].bValid               = 1;
    voice_snd[i].voice_sxd            = C_PLAY_FAILED;
    return (long)&voice_snd[i];
}

void __cdecl desassocie_snd(long voice)
{
    voice_description *pDesc = (voice_description *)voice;

    if (pDesc->pbValid != NULL)
    {
        *(pDesc->pbValid) = 0;
        pDesc->pbValid = NULL;
    }
    pDesc->bFree = 1;
}

void __cdecl invalidate_ptrvalid_snd(long voice)
{
    voice_description *pDesc = (voice_description *)voice;
    pDesc->pbValid = NULL;
}

long __cdecl associe_snd(long id_asso, long voice_sxd, tdstBlockResourceMem *res,
                         long prio, int *valid, long transition)
{
    int i;
    voice_description *voice_local;
    voice_description *pAsso = (voice_description *)id_asso;

    for (i = 0, voice_local = &voice_snd[0]; i < NB_VOICE_MAX; i++, voice_local++)
    {
        if ((voice_local->bFree == 0) &&
            (((voice_local->voice_sxd == C_PLAY_FAILED) &&
              (voice_local->bAssociation_running == 0))
             || ((voice_sxd != C_PLAY_FAILED) &&
                 (voice_local->voice_sxd == voice_sxd) &&
                 (res->eType == voice_local->res.pstPtr->eType))
             || (!voice_local->bValid)))
        {
            desassocie_snd((long)voice_local);
        }
    }

    if (voice_sxd != C_PLAY_FAILED)
    {
        pAsso->voice_sxd            = voice_sxd;
        pAsso->res.pstPtr             = res;
        pAsso->bFree               = 0;
        pAsso->bAssociation_running = 0;
        pAsso->bValid              = 1;
        pAsso->pbValid             = valid;
        pAsso->bTransition         = transition;
        pAsso->lPauseInstance      = 0;
        if (pAsso->pbValid != NULL)
            *(pAsso->pbValid) = 1;
        return id_asso;
    }

    pAsso->bFree   = 1;
    pAsso->pbValid = NULL;
    return C_PLAY_FAILED;
}

typedef struct SND_tdstIndexedOwner_
{
    unsigned char         aReserved_00[0x10];
    SNDLST2_M_DynamicAnchorTo(pSND_tdstIndexedInstance) anchor;
} SND_tdstIndexedOwner;

SND_tdstIndexedInstance *__cdecl SND_pc_fn_p_stAddIndexedInstance(SND_tdstIndexedOwner *pOwner)
{
    SND_tdstIndexedInstance *pNode =
        (SND_tdstIndexedInstance *)SND_fn_pvMallocSndEx(E_ucSndBlockMain, 0x18);

    pNode->pZone = NULL;
    SNDLST2_M_DynamicInitElement(pNode);
    SNDLST2_M_DynamicAddTail(&pOwner->anchor, pNode);

    return pNode;
}

/* 0x00457670 */
void __cdecl SND_pc_fn_vEraseIndexedInstance(SND_tdstIndexedInstance *pNode)
{
    SNDLST2_M_DynamicIsolate(pNode);

    SND_fn_vFreeSndEx(E_ucSndBlockMain, pNode);
}

/* 0x004576D0 */
void __cdecl SND_pc_fn_vReleaseIndexedRequest(SND_tdstIndexedRequest *pReq)
{
    pReq->lState = 1;
    if (pReq->pbValid != NULL)
    {
        *pReq->pbValid = 0;
        pReq->pbValid  = NULL;
    }
}

SND_tdstIndexedRequest *__cdecl SND_pc_fn_p_stAllocIndexedRequest(
    tdstBlockResourceMem *pstSplitRes, void *pParam1, SndBool *pbValid)
{
    int i;
    int j;
    SND_tdstIndexedInstance *pNode, *pNext;
    SND_tdstIndexedRequest  *pReq;

    pReq = SND_pc_g_aIndexedRequests;
    i = SND_C_NB_INDEXED_REQUESTS;
    do {
        if (pReq->lState == 0)
        {
            SNDLST2_M_DynamicForEachMovingElementOf(&pReq->anchor, pNode, pNext, j)
            {
                if (pNode->bParamActive == 0)
                    SND_pc_fn_vEraseIndexedInstance(pNode);
            }
        }
        pReq++;
    } while (--i);

    for (i = 0; i < SND_C_NB_INDEXED_REQUESTS; i++)
    {
        if (SND_pc_g_aIndexedRequests[i].lState != 0)
            break;
    }

    if (i == SND_C_NB_INDEXED_REQUESTS)
    {
        SND_fn_vDisplayErrorEx(E_uwSndTooManySplits,
                               (char *)SND_C_szEmpty, SNDERR_DISPLAY_IF_WANTED);
        return (SND_tdstIndexedRequest *)-1;
    }

    pReq = &SND_pc_g_aIndexedRequests[i];
    pReq->lState                   = 0;
    SNDLST2_M_DynamicInitAnchor(&pReq->anchor);
    pReq->pstSplitRes              = pstSplitRes;
    pReq->pParam1                  = pParam1;
    pReq->pbValid                  = pbValid;
    return pReq;
}

/* 0x004577A0 */
void __cdecl SND_pc_fn_vCopyParamWithVolumeLines(tdstRangeSplitMem *pRange,
                                                 SoundParam *pSrc,
                                                 SoundParam *pDst)
{
    SND_fn_vCopySoundParam(pDst, pSrc);
    pDst->iFlags |= C_SOUNDPARAM_COEF_FREQ;
    pDst->Freq = SND_fn_rMulRealRealSnd(pSrc->Freq, pRange->rCoefPitch);

    if ((pSrc->Freq <= pRange->rPitchA) || (pSrc->Freq >= pRange->rPitchD))
        pDst->ucVol = 0;
    else if ((pSrc->Freq >= pRange->rPitchB) && (pSrc->Freq <= pRange->rPitchC))
        pDst->ucVol = (unsigned char)((pSrc->ucVol * pRange->ucVolume) >> 7);
    else if (pSrc->Freq < pRange->rPitchB)
        pDst->ucVol = (unsigned char)SND_fn_rMulRealRealSnd(pSrc->Freq - pRange->rPitchA, pRange->rCoefMagAB);
    else if (pSrc->Freq > pRange->rPitchC)
        pDst->ucVol = (unsigned char)SND_fn_rMulRealRealSnd(pSrc->Freq - pRange->rPitchD, pRange->rCoefMagCD);
}

int __cdecl SND_pc_fn_iCountChain(SND_tdstIndexedRequest *pReq)
{
    int iCount;
    SND_tdstIndexedInstance *pNode;

    SNDLST2_M_DynamicForEachElementOf(&pReq->anchor, pNode, iCount)
    {
    }
    return iCount;
}

/* 0x00457860 */
int __cdecl SND_pc_fn_lMatchIndexedZones(SND_tdstIndexedRequest *pReq, struct SoundParam_ *par)
{
	tdstRangeSplitMem	*pZone;
	tdstRangeSplitMem	*aZone[SND_C_NB_ZONE_MAX];
	SND_tdstIndexedInstance	*aInst[SND_C_NB_ZONE_MAX];
	struct SoundParam_	stSoundParam;
	SND_tdstIndexedInstance	*pCur;
	SND_tdstIndexedInstance	*pNext;
	SND_tdstIndexedInstance	*pNew;
	int		iNbZones;
	int		iNbMatch;
	int		i;
	int		k;

	iNbMatch=0;
	stSoundParam.iNbLinks=par->iNbLinks;
	if ((pReq==(SND_tdstIndexedRequest *)-1) || (pReq->lState!=0))
		return 0;

	iNbZones=SNDLST2_M_StaticGetNumberOfElements(&pReq->pstSplitRes->uRes.stSplit.stRangeList);
	pZone=SNDLST2_M_StaticGetFirstElement(&pReq->pstSplitRes->uRes.stSplit.stRangeList);

	for (i=0;i<iNbZones;i++)
	{
		if ((pZone->rPitchA<=par->Freq) && (pZone->rPitchD>=par->Freq))
		{
			aZone[iNbMatch]=pZone;
			iNbMatch++;
		}
		pZone++;
	}

	for (i=0;i<iNbMatch;i++)
		aInst[i]=NULL;

	SNDLST2_M_DynamicForEachMovingElementOf(&pReq->anchor,pCur,pNext,i)
	{
		k=0;
		while ((pCur->pZone!=aZone[k]) && (k<iNbMatch))
			k++;
		if (k==iNbMatch)
		{
			if (pCur->bParamActive!=0)
			{
				pCur->bParamActive=0;
				SND_fn_vStopSnd(pCur->lVoice);
			}
			SND_pc_fn_vEraseIndexedInstance(pCur);
		}
		else
			aInst[k]=pCur;
	}

	for (k=0;k<iNbMatch;k++)
	{
		pCur=aInst[k];
		if (pCur!=NULL)
		{
			SND_pc_fn_vCopyParamWithVolumeLines(aZone[k],par,&stSoundParam);
			if (pCur->bParamActive)
				pCur->bParamActive=SND_fn_bSetParamSnd(pCur->lVoice,&stSoundParam);
			else
				SND_pc_fn_vEraseIndexedInstance(pCur);
		}
		else
		{
			pNew=SND_pc_fn_p_stAddIndexedInstance((SND_tdstIndexedOwner *)pReq);
			pNew->pZone=aZone[k];
			SND_pc_fn_vCopyParamWithVolumeLines(aZone[k],par,&stSoundParam);
			pNew->lVoice=SND_fn_lPlaySnd(aZone[k]->uRes,&stSoundParam,(long)pReq->pParam1,NULL,0,(SndBool *)&pNew->bParamActive);
		}
	}

	return iNbMatch;
}

SndBool __cdecl SND_fn_bLoadResBinarySnd(tdstBlockResourceDisk* disk,tdstBlockResourceMem* mem,char *pDataBloc)
{

	mem->eType=disk->eType;

	switch (disk->eType)
	{
	case TYPE_SAMPLE:
	        return(SND_fn_bLoadResBinarySxd(disk,mem,pDataBloc));
	        break;
	case TYPE_MIDI:
#ifndef DISABLE_MIDI
	        return(SND_fn_bLoadResBinaryMidi(disk,mem,pDataBloc));
#else
	        return FALSE;
#endif
	        break;
	case TYPE_CDAUDIO:
#ifndef DISABLE_CD
	        return(SND_fn_bLoadResBinaryCd(disk,mem,pDataBloc));
#else
	        return FALSE;
#endif
	        break;
	case TYPE_SEQUENCE:
	        return(SND_fn_bLoadResBinarySeq(disk,mem,pDataBloc));
	        break;
	case TYPE_SWITCH_OLD:
	        return(SND_fn_bLoadResBinarySwitchOld(disk,mem,pDataBloc));
	        break;
	case TYPE_SWITCH:
	        return(SND_fn_bLoadResBinarySwitch(disk,mem,pDataBloc));
	        break;
	case TYPE_SPLIT:
	        return(SND_pc_fn_bLoadResRatioStride36(disk,mem,pDataBloc));
	        break;
	case TYPE_THEME:
	        return(SND_fn_bLoadResBinaryTheme(disk,mem,pDataBloc));
	        break;
	case TYPE_RANDOM:
	        return(SND_fn_bLoadResBinaryRandom(disk,mem,pDataBloc));
	        break;
	default:
	        SND_fn_vDisplayError(0x15, "");
	        return FALSE;
	        break;
	}
}

void __cdecl SND_fn_vStopBeforeUnLoadResSnd(tdstBlockResourceMem *pRes)
{
    int i;

    for (i = 0; i < NB_VOICE_MAX; i++)
    {
        if (!voice_snd[i].bFree && (voice_snd[i].res.pstPtr == pRes) && voice_snd[i].bValid)
        {
            if (voice_snd[i].bTransition)
                SND_fn_vStopTransitionSnd((long)&voice_snd[i]);
            else
                SND_fn_vStopSnd((long)&voice_snd[i]);
        }
    }
}

void __cdecl SND_fn_vUnLoadResSnd(tdstBlockResourceMem *mem)
{
    switch (mem->eType)
    {
        case TYPE_SAMPLE:
            SND_fn_vUnLoadResSxd(mem);
            break;
        case TYPE_MIDI:
#ifndef DISABLE_MIDI
            SND_fn_vUnLoadResMidi(mem);
#endif
            break;
        case 0xB:
            SND_fn_vUnLoadResTheme(mem);
            break;
        default:
            break;
    }
    mem->eType = 0;
}

void __cdecl SND_pc_fn_vComputeResRatios(tdstRangeSplitMem *pRange)
{
    SndReal rDiff;

    rDiff = pRange->rPitchB - pRange->rPitchA;
    if (rDiff != 0)
    {
        pRange->rCoefMagAB = SND_fn_rDivRealRealSnd((SndReal)pRange->ucVolume, rDiff);
    }
    else
    {
        pRange->rCoefMagAB = C_SNDREAL_1;
    }
    rDiff = pRange->rPitchC - pRange->rPitchD;
    if (rDiff != 0)
    {
        pRange->rCoefMagCD = SND_fn_rDivRealRealSnd((SndReal)pRange->ucVolume, rDiff);
    }
    else
    {
        pRange->rCoefMagCD = C_SNDREAL_1;
    }
}

/* 0x00457BF0 */
void *__cdecl SND_pc_fn_lPlaySplit(tduRefRes res,struct SoundParam_ *par,long prio,void *fn_callback,long par_callback,SndBool *valid)
{
	SND_tdstIndexedRequest	*pReq;
	int		iNbMatch;

	pReq=(SND_tdstIndexedRequest *)SND_pc_fn_p_stAllocIndexedRequest(res.pstPtr,(void *)prio,valid);
	iNbMatch=SND_pc_fn_lMatchIndexedZones(pReq,par);

	if ((iNbMatch==0) && !res.pstPtr->uRes.stSplit.bStayActive)
	{
		SND_pc_fn_vReleaseIndexedRequest(pReq);
		return (void *)-1;
	}
	return (void *)pReq;
}

/* 0x00457C40 */
SndBool __stdcall SND_pc_fn_bSetParamSplit(long voie, struct SoundParam_ *par)
{
	SND_tdstIndexedRequest	*pReq=(SND_tdstIndexedRequest *)voie;

	if ((SND_pc_fn_lMatchIndexedZones(pReq,par)==0)
		&& !pReq->pstSplitRes->uRes.stSplit.bStayActive)
	{
		SND_pc_fn_vReleaseIndexedRequest(pReq);
		return 0;
	}
	return (SndBool)pReq;
}

int __stdcall SND_pc_fn_iHasChainWork(SND_tdstIndexedRequest *pReq)
{
    int iCount;

    iCount = SND_pc_fn_iCountChain(pReq);

    if ((iCount == 0) && !pReq->pstSplitRes->uRes.stSplit.bStayActive)
    {
        return FALSE;
    }
    return TRUE;
}

void __stdcall SND_pc_fn_vReleaseIndexedOwner(SND_tdstIndexedRequest *pOwner)
{
    SND_tdstIndexedInstance *pNode;
    SND_tdstIndexedInstance *pNext;
    int i;

    SNDLST2_M_DynamicForEachMovingElementOf(&pOwner->anchor, pNode, pNext, i)
    {
        if (pNode->bParamActive)
        {
            pNode->bParamActive = 0;
            SND_fn_vStopSnd(pNode->lVoice);
        }
        SND_pc_fn_vEraseIndexedInstance(pNode);
    }
    SND_pc_fn_vReleaseIndexedRequest(pOwner);
}

void __cdecl SND_pc_fn_vCopyResHdrTwo(tdstBlockResourceDisk* disk,tdstBlockResourceMem* mem,void* ptrBegin)
{
        mem->Id=disk->Id;
        mem->eType=disk->eType;
        mem->eStorage=disk->eStorage;
        mem->ucVolume=disk->ucVolume;
        mem->bIsLoaded=TRUE;

        mem->uRes.stSplit.ulNbRanges = disk->uRes.stSplit.ulNbRanges;
        mem->uRes.stSplit.bStayActive = disk->uRes.stSplit.bStayActive;
}

SndBool __cdecl SND_pc_fn_bLoadResRatioStride36(tdstBlockResourceDisk *_pBRDisk,tdstBlockResourceMem *_pBRMem,char *pDataBloc)
{
        tdstRangeSplitMem *pRangeArray,*pRangeElement;

        int iPos;

        pRangeArray=(tdstRangeSplitMem *)(pDataBloc + _pBRDisk->ulDataOffset);

        SNDLST2_M_StaticSetFirstElement(&_pBRMem->uRes.stSplit.stRangeList,pRangeArray);
        SNDLST2_M_StaticSetNumberOfElements(&_pBRMem->uRes.stSplit.stRangeList,_pBRDisk->uRes.stSplit.ulNbRanges);

        SNDLST2_M_StaticForEachElementOf( &_pBRMem->uRes.stSplit.stRangeList, pRangeElement, iPos)
        {
            pRangeElement->uRes.pstPtr=SND_fn_pGetBinRes(pRangeElement->uRes.Id);
            SND_pc_fn_vComputeResRatios(pRangeElement);
        }

        SND_pc_fn_vCopyResHdrTwo(_pBRDisk,_pBRMem,NULL);
        return FALSE;
}

static void pre_initSnd(void)
{
    int i;

    for (i = 0; i < NB_VOICE_MAX; i++)
    {
        voice_snd[i].bFree = 1;
    }
    for (i = 0; i < SND_C_NB_INDEXED_REQUESTS; i++)
    {
        SND_pc_g_aIndexedRequests[i].lState = 1;
    }
}

int SND_fn_iInitSnd(void *pInitStruct)
{
    RollOffParam rolloff;

    pre_initSnd();

    g_iInitSxd = SND_fn_iInitSxd(pInitStruct);

    SND_fn_vGetDefaultRollOff(&rolloff);
    SND_fn_vSetDefaultRollOffSxd(&rolloff);

#ifndef DISABLE_MIDI
    g_iInitMidi = SND_fn_iInitMidi(pInitStruct);
#endif

#ifndef DISABLE_CD
    g_iInitCD = SND_fn_iInitCD(pInitStruct);
#endif

#ifndef DISABLE_RECORD
    SND_fn_iInitRecord(pInitStruct);
#endif

    SND_fn_iInitSeq();
    SND_fn_vInitTheme();

    return C_INIT_OK;
}

void __cdecl SND_fn_vSetDopplerFactorSnd(long factor)
{
    SND_fn_vSetDopplerFactor(factor);
    SND_fn_vSetDopplerFactorSxd(factor);
}

long SND_fn_lPlaySnd(tduRefRes res,struct SoundParam_* par,long prio,SND_td_pfn_vSoundCallback fn_callback,long par_callback,SndBool* valid)
{
	long voice_sxd;
	long pre_voice_snd;

	pre_voice_snd = pre_associe_snd();
	if (pre_voice_snd == C_PLAY_FAILED)
		return C_PLAY_FAILED;

	switch (res.pstPtr->eType)
	{
	case TYPE_SAMPLE:
		voice_sxd = SND_fn_lPlaySxd((long)res.Id, (void *)par, prio, (void *)fn_callback, par_callback);
		break;
	case TYPE_MIDI:
#ifndef DISABLE_MIDI
		voice_sxd = SND_fn_lPlayMidi((long)res.Id, (void *)par, prio, (void *)fn_callback, par_callback);
#else
		voice_sxd = C_PLAY_FAILED;
#endif
		break;
	case TYPE_CDAUDIO:
#ifndef DISABLE_CD
		voice_sxd = SND_pc_fn_lPlayCD((long)res.Id, (void *)par, prio, (void *)fn_callback, par_callback);
#else
		voice_sxd = C_PLAY_FAILED;
#endif
		break;
	case TYPE_SEQUENCE:
		voice_sxd = SND_fn_lPlaySeq(res, par, prio, fn_callback, par_callback, &((voice_description*)pre_voice_snd)->bValid);
		break;
	case TYPE_SPLIT:
		voice_sxd = (long)SND_pc_fn_lPlaySplit(res, par, prio, (void *)fn_callback, par_callback, &((voice_description*)pre_voice_snd)->bValid);
		break;
	case TYPE_THEME:
		voice_sxd = SND_fn_lPlayTheme(C_PLAY_FAILED, res, par, prio, (void *)fn_callback, par_callback, &((voice_description*)pre_voice_snd)->bValid);
		break;
	case TYPE_RANDOM:
		voice_sxd = SND_fn_lPlayRandom(res, par, prio, (void *)fn_callback, par_callback);
		break;
	case TYPE_SWITCH:
		voice_sxd = SND_fn_lPlaySwitch(res, par, prio, (void *)fn_callback, par_callback);
		break;
	case TYPE_INVALID:
	case TYPE_SWITCH_OLD:
	case TYPE_THEME_OLD:
	case TYPE_THEME_OLD2:
	default:
		SND_M_DisplayError(E_uwSndInvalidTypeOfResource, "");
		voice_sxd = C_PLAY_FAILED;
		break;
	}

	return associe_snd(pre_voice_snd, voice_sxd, res.pstPtr, prio, valid, FALSE);
}

SndBool __stdcall SND_fn_bSetParamSnd(long voie, struct SoundParam_ *par)
{
    voice_description *p = (voice_description *)voie;
    SndBool ret;

    ret = a_fnFunc[p->res.pstPtr->eType].pfnSetParam(p->voice_sxd, par);

    if (ret == 0)
        desassocie_snd(voie);

    return ret;
}

SndBool SND_fn_bTestIsPlayingSnd(long voie)
{
	SndBool ret;

	ret = (*a_fnFunc[((voice_description*)voie)->res.pstPtr->eType].pfnTestIsPlaying)(((voice_description*)voie)->voice_sxd);

	if (ret==FALSE)
		desassocie_snd(voie);

	return ret;
}

void __stdcall SND_fn_vStopSnd(long voie)
{
    voice_description *p = (voice_description *)voie;

    if (!p->bFree)
        a_fnFunc[p->res.pstPtr->eType].pfnStop(p->voice_sxd);

    desassocie_snd(voie);
}

void __stdcall SND_fn_vPauseSnd(long voie)
{
    voice_description *p = (voice_description *)voie;
    a_fnFunc[p->res.pstPtr->eType].pfnPause(p->voice_sxd);
}

void __stdcall SND_fn_vResumeSnd(long voie)
{
    voice_description *p = (voice_description *)voie;
    a_fnFunc[p->res.pstPtr->eType].pfnResume(p->voice_sxd);
}

void __cdecl SND_fn_vPauseAllSnd(void)
{
    voice_description *voie;
    int i;

    for (i = 0, voie = &voice_snd[0]; i < NB_VOICE_MAX; i++, voie++)
    {
        if (voie->bValid && !voie->bFree && !voie->lPauseInstance)
        {
            voie->lPauseInstance = g_lCurrentGlobalPauseInstance;

            if (voie->bTransition)
                SND_fn_vPauseTransitionSnd((long)voie);
            else
                SND_fn_vPauseSnd((long)voie);
        }
    }

    g_lCurrentGlobalPauseInstance++;
}

void __cdecl SND_fn_vResumeAllSnd(void)
{
    voice_description *voie;
    int i;

    g_lCurrentGlobalPauseInstance--;

    for (i = 0, voie = &voice_snd[0]; i < NB_VOICE_MAX; i++, voie++)
    {
        if (voie->bValid && !voie->bFree &&
            (voie->lPauseInstance <= g_lCurrentGlobalPauseInstance))
        {
            voie->lPauseInstance = 0;

            if (voie->bTransition)
                SND_fn_vResumeTransitionSnd((long)voie);
            else
                SND_fn_vResumeSnd((long)voie);
        }
    }
}

void __cdecl SND_fn_vSetStereoSnd(SndBool active)
{
    SND_fn_vSetStereoSxd(active);
}

/* 0x00458180 */
SndBool __cdecl SND_fn_bGetStereoSnd(void)
{
    return SND_fn_bGetStereoSxd();
}

SndBool __stdcall SND_fn_bAnyPlayingTheme(void);

/* 0x00458190 */
SndBool __cdecl SND_fn_bIsMusicPlayingSnd(void)
{
    return SND_fn_bAnyPlayingTheme();
}

void __cdecl SND_fn_vDesInitSnd(void)
{
    SND_fn_vDesInitTheme();
    SND_fn_vDesInitSeq();
#ifndef DISABLE_RECORD
    SND_fn_vDesInitRecord();
#endif
#ifndef DISABLE_CD
    if (g_iInitCD == C_INIT_OK)
    {
        SND_fn_vDesInitCD();
    }
#endif
#ifndef DISABLE_MIDI
    if (g_iInitMidi == C_INIT_OK)
    {
        SND_fn_vDesInitMidi();
    }
#endif
    if (g_iInitSxd == C_INIT_OK)
    {
        SND_fn_vDesInitSxd();
    }
}

/* 0x004581E0 */
void __cdecl SND_fn_vSetEffectSnd(int iIdEffect)
{
    SND_fn_vSetEffectSxd(iIdEffect);
}

long __cdecl SND_fn_lCreateMicroSnd(MicroParam *par)
{
    return SND_fn_lCreateMicroSxd(par);
}

/* 0x00458200 */
void __stdcall SND_fn_vDestroyMicroSxd(long sxdId);
void SND_fn_vDestroyMicroSnd(long sndId)
{
	SND_fn_vDestroyMicroSxd(sndId);
}

/* 0x00458210 */
void __cdecl SND_fn_vSetMicroParamSnd(long sndId, MicroParam *par)
{
    SND_fn_vSetMicroParamSxd(sndId, par);
}

long SND_fn_lPlayTransitionSnd(tduRefRes FirstRes,tduRefRes NextRes,SND_td_pfn_vSoundCallback transition_callback,long param_callback,struct SoundParam_* par)
{
    long voice_sxd;
    long pre_voice_snd;

    if ((pre_voice_snd=pre_associe_snd())==C_PLAY_FAILED)
		return C_PLAY_FAILED;

	switch (FirstRes.pstPtr->eType)
	{
	/* 0x00455280 */
	case TYPE_SAMPLE:
        voice_sxd=SND_fn_lPlayTransitionExSxd(FirstRes,NextRes,transition_callback,param_callback,par);
		break;
#ifndef DISABLE_THEME_MIDI
	case TYPE_MIDI:
#ifndef DISABLE_MIDI
        voice_sxd=SND_fn_lPlayTransitionMidi(FirstRes,NextRes,transition_callback,param_callback,par);
#else
        voice_sxd=C_PLAY_FAILED;
#endif
		break;
#endif
	default:
        SND_M_DisplayError(E_uwSndInvalidTypeOfResource,"");
        voice_sxd=C_PLAY_FAILED;
		break;
	}

    return associe_snd(pre_voice_snd,voice_sxd,FirstRes.pstPtr,0,NULL,TRUE);
}

SndBool SND_fn_bSetParamTransitionSnd(long voice,struct SoundParam_ *par)
{
    SndBool ret;

    switch (((voice_description*)voice)->res.pstPtr->eType)
	{
	/* 0x004582C0 */
	case TYPE_SAMPLE:
        ret=SND_fn_bSetParamTransitionSxd(((voice_description*)voice)->voice_sxd,par);
		break;
#ifndef DISABLE_THEME_MIDI
	case TYPE_MIDI:
#ifndef DISABLE_MIDI
        ret=SND_fn_bSetParamTransitionMidi(((voice_description*)voice)->voice_sxd,par);
#else
        ret=FALSE;
#endif
		break;
#endif
	default:
        SND_M_DisplayError(E_uwSndInvalidTypeOfResource,"");
        ret=FALSE;
		break;
	}

    if (ret==FALSE)
		desassocie_snd(voice);

    return ret;
}

SndBool SND_fn_bSetNextTransitionSnd(long voice,tduRefRes new_res)
{
    SndBool ret;

    /* 0x00458320 */
    switch (((voice_description*)voice)->res.pstPtr->eType)
	{
	case TYPE_SAMPLE:
        ret=SND_fn_bSetNextTransitionSxd(((voice_description*)voice)->voice_sxd,new_res);
		break;
#ifndef DISABLE_THEME_MIDI
	case TYPE_MIDI:
#ifndef DISABLE_MIDI
        ret=SND_fn_bSetNextTransitionMidi(((voice_description*)voice)->voice_sxd,new_res);
#else
        ret=FALSE;
#endif
		break;
#endif
	default:
        SND_M_DisplayError(E_uwSndInvalidTypeOfResource,"");
        ret=FALSE;
		break;
	}

	((voice_description*)voice)->res.pstPtr=new_res.pstPtr;

    return ret;
}

SndBool SND_fn_bDoTransitionWithFadeSnd(long voice,tduRefRes new_res)
{
    SndBool ret;

    switch (((voice_description*)voice)->res.pstPtr->eType)
	{
	/* 0x00458370 */
	case TYPE_SAMPLE:
        ret=SND_fn_bDoTransitionWithFadeSxd(((voice_description*)voice)->voice_sxd,new_res);
		break;
#ifndef DISABLE_THEME_MIDI
	case TYPE_MIDI:
#ifndef DISABLE_MIDI
        ret=SND_fn_bDoTransitionWithFadeMidi(((voice_description*)voice)->voice_sxd,new_res);
#else
        ret=FALSE;
#endif
		break;
#endif
	default:
        SND_M_DisplayError(E_uwSndInvalidTypeOfResource,"");
        ret=FALSE;
		break;
	}

	((voice_description*)voice)->res.pstPtr=new_res.pstPtr;
    return ret;
}

/* 0x004583C0 */
SndBool __stdcall SND_fn_bDoTransitionWithFadeSxd2(long voice,tduRefRes new_res,SndReal rDuration);
SndBool __stdcall SND_fn_bDoTransitionWithFadeMidi2(long voice,tduRefRes new_res,SndReal rDuration);

SndBool SND_fn_bDoTransitionWithFadeSnd2(long voice,tduRefRes new_res,SndReal rDuration)
{
    SndBool ret;

    switch (((voice_description*)voice)->res.pstPtr->eType)
    {
    case TYPE_SAMPLE:
        ret=SND_fn_bDoTransitionWithFadeSxd2(((voice_description*)voice)->voice_sxd,new_res,rDuration);
        break;
    case TYPE_MIDI:
#ifndef DISABLE_MIDI
        ret=SND_fn_bDoTransitionWithFadeMidi2(((voice_description*)voice)->voice_sxd,new_res,rDuration);
#else
        ret=FALSE;
#endif
        break;
    default:
        SND_M_DisplayError(E_uwSndInvalidTypeOfResource,"");
        ret=FALSE;
        break;
    }

    return ret;
}

void __cdecl SND_fn_vStopTransitionSnd(long voice)
{
    voice_description *p = (voice_description *)voice;

    if (!p->bFree)
    {
        switch (p->res.pstPtr->eType)
        {
            case TYPE_SAMPLE:
                SND_fn_vStopTransitionSxd(p->voice_sxd);
                break;
            case TYPE_MIDI:
#ifndef DISABLE_MIDI
                SND_fn_vStopTransitionMidi(p->voice_sxd);
#endif
                break;
            default:
                SND_RT_fn_vDisplayError(E_uwSndInvalidTypeOfResource, SND_C_szEmpty);
                break;
        }
    }
    desassocie_snd(voice);
}

void __cdecl SND_fn_vResumeTransitionSnd(long voice)
{
    voice_description *p = (voice_description *)voice;
    switch (p->res.pstPtr->eType)
    {

        case TYPE_SAMPLE:
            SND_fn_vResumeTransitionSxd(p->voice_sxd);
            break;
        case TYPE_MIDI:
#ifndef DISABLE_MIDI
            SND_fn_vResumeTransitionMidi(p->voice_sxd);
#endif
            break;
        default:
            SND_RT_fn_vDisplayError(E_uwSndInvalidTypeOfResource, SND_C_szEmpty);
            break;
    }
}

void __cdecl SND_fn_vPauseTransitionSnd(long voice)
{
    voice_description *p = (voice_description *)voice;
    switch (p->res.pstPtr->eType)
    {

        case TYPE_SAMPLE:
            SND_fn_vPauseTransitionSxd(p->voice_sxd);
            break;
        case TYPE_MIDI:
#ifndef DISABLE_MIDI
            SND_fn_vPauseTransitionMidi(p->voice_sxd);
#endif
            break;
        default:
            SND_RT_fn_vDisplayError(E_uwSndInvalidTypeOfResource, SND_C_szEmpty);
            break;
    }
}

void __cdecl SND_fn_vReleaseDriverSnd(void)
{
#ifndef DISABLE_CD
    SND_fn_vReleaseDriverCD();
#endif
#ifndef DISABLE_MIDI
    SND_fn_vReleaseDriverMidi();
#endif
    SND_fn_vReleaseDriverSxd();
}

void __cdecl SND_fn_vRestoreDriverSnd(void)
{
    SND_fn_vRestoreDriverSxd();
#ifndef DISABLE_MIDI
    SND_fn_vRestoreDriverMidi();
#endif
#ifndef DISABLE_CD
    SND_fn_vRestoreDriverCD();
#endif
}

SndReal __stdcall SND_fn_rGetPosSnd(long voie)
{
    voice_description *p = (voice_description *)voie;

    if (p->bValid)
        return a_fnFunc[p->res.pstPtr->eType].pfnGetPos(p->voice_sxd);
    else
        return -2;
}

/* 0x00458520 */
void __cdecl SND_fn_vSynchroSnd(void)
{
    SND_fn_vSynchroSxd();
#ifndef DISABLE_CD
    SND_fn_vSynchroCD();
#endif
#ifndef DISABLE_MIDI
    SND_fn_vSynchroMidi();
#endif
}

tduRefRes SND_fn_tduGetLowestResourceRandom(tduRefRes uHighRes, struct SoundParam_ *par, SndBool eval);
tduRefRes SND_fn_tduGetLowestResourceSwitch(tduRefRes uHighRes, struct SoundParam_ *par, SndBool eval);

tduRefRes SND_fn_tduGetLowestResourceSnd(tduRefRes uHighRes, struct SoundParam_ *par, SndBool eval)
{
	if (uHighRes.pstPtr==SND_C_RES_FANTOME_PTR) return uHighRes;

	switch (uHighRes.pstPtr->eType)
	{
	case TYPE_SAMPLE:
	case TYPE_MIDI:
	case TYPE_CDAUDIO:
	case TYPE_SEQUENCE:
	case TYPE_THEME:
		return uHighRes;
	case TYPE_RANDOM:
		return SND_fn_tduGetLowestResourceRandom(uHighRes,par,eval);
	case TYPE_SWITCH:
		return SND_fn_tduGetLowestResourceSwitch(uHighRes,par,eval);
	case TYPE_SPLIT:
	case TYPE_INVALID:
	default:
		SND_M_DisplayError(E_uwSndInvalidTypeOfResource,"");
		return SND_RES_FANTOME;
	}
}

int __cdecl SND_pc_fn_iSetResourceVolumeByType(tdstBlockResourceMem *pRes, long lVolume)
{
    unsigned char ucVolume;

    ucVolume = (unsigned char)lVolume;
    if (pRes != NULL)
    {
        switch (pRes->eType)
        {
        case TYPE_SAMPLE:
            return SND_fn_bSetResourceStaticVolumeSxd((long)pRes, lVolume);
        case TYPE_MIDI:
#ifndef DISABLE_MIDI
            return SND_fn_bSetResourceStaticVolumeMidi((long)pRes, lVolume);
#else
            return FALSE;
#endif
        case TYPE_CDAUDIO:
#ifndef DISABLE_CD
            return SND_fn_bSetResourceStaticVolumeCD((long)pRes, lVolume);
#else
            return FALSE;
#endif
        case TYPE_SEQUENCE:
            return SND_pc_fn_iSetSequenceNodeByte(pRes, ucVolume);
        case TYPE_SWITCH:
            return SND_pc_fn_iSetSwitchNodeByte(pRes, ucVolume);
        case TYPE_SPLIT:
            return SND_pc_fn_iSetSplitNodeByte(pRes, ucVolume);
        case TYPE_RANDOM:
            return SND_pc_fn_iSetNodeByteStd(pRes, ucVolume);
        case TYPE_THEME:
            return SND_pc_fn_iSetThemeNodeByte(pRes, ucVolume);
        }
    }
    return FALSE;
}

/* 0x00459BE0 */
void __stdcall SND_fn_vStopTheme(long canal);

/* 0x0045A2A0 */
SndBool __stdcall SND_fn_bSetParamRandom(long voice, struct SoundParam_ *par);

/* 0x0045ACD0 */
SndBool __stdcall SND_fn_bSetParamSeq(long voice, struct SoundParam_ *par);
SndBool __stdcall SND_fn_bTestIsPlayingSeq(long voice);

/* 0x0045B0B0 */
void __stdcall SND_fn_vStopSeq(long voice);

/* 0x004598C0 */
SndBool __stdcall SND_fn_bSetParamTheme(long canal, struct SoundParam_ *par);

#define SND_M_PFN_V(f)  ((td_fn_vStopFunc *)(f))
#define SND_M_PFN_R(f)  ((td_fn_rGetPosFunc *)(f))
#define SND_M_PFN_T(f)  ((td_fn_bTestIsPlayingFunc *)(f))

#define SND_fn_rGetPosSeq SND_fn_rGetPosTheme

static td_st_Func a_fnFunc[NB_TYPES_RESOURCES] =
{
      { 0, 0, 0, 0, 0, 0 },

    { SND_fn_vPauseSxd, SND_fn_vResumeSxd, SND_fn_bSetParamSxd, SND_fn_vStopSxd,
      SND_fn_rGetPosSxd, SND_fn_bTestIsPlayingSxd },

#ifndef DISABLE_MIDI
    { SND_fn_vPauseMidi, SND_fn_vResumeMidi, SND_fn_bSetParamMidi, SND_fn_vStopMidi,
      SND_fn_rGetPosMidi, SND_fn_bTestIsPlayingMidi },
#else
    { NULL, NULL, NULL, NULL, NULL, NULL },
#endif

#ifndef DISABLE_CD
    { SND_fn_vPauseCD, SND_fn_vResumeCD, SND_fn_bSetParamCD, SND_fn_vStopCD,
      SND_fn_rGetPosCD, SND_fn_bTestIsPlayingCD },
#else
    { NULL, NULL, NULL, NULL, NULL, NULL },
#endif

    { SND_fn_vPauseSeq, SND_fn_vResumeSeq, SND_fn_bSetParamSeq, SND_fn_vStopSeq,
      SND_fn_rGetPosSeq, SND_fn_bTestIsPlayingSeq },
      { 0, 0, 0, 0, 0, 0 },

    { SND_M_PFN_V(SND_pc_fn_vNopStd4), SND_M_PFN_V(SND_pc_fn_vNopStd4), SND_pc_fn_bSetParamSplit,
      SND_M_PFN_V(SND_pc_fn_vReleaseIndexedOwner), SND_fn_rGetPosTheme,
      SND_M_PFN_T(SND_pc_fn_iHasChainWork) },
      { 0, 0, 0, 0, 0, 0 },

    { SND_fn_vPauseSwitch, SND_fn_vResumeSwitch, SND_fn_bSetParamSwitch,
      SND_fn_vStopSwitch, SND_fn_rGetPosSwitch, SND_fn_bTestIsPlayingSwitch },
      { 0, 0, 0, 0, 0, 0 },

    { SND_fn_vPauseRandom, SND_fn_vResumeRandom, SND_fn_bSetParamRandom,
      SND_fn_vStopRandom, SND_fn_rGetPosRandom, SND_fn_bTestIsPlayingRandom },

    { SND_fn_vPauseTheme, SND_fn_vResumeTheme, SND_fn_bSetParamTheme,
      SND_M_PFN_V(SND_fn_vStopTheme), SND_fn_rGetPosTheme,
      SND_fn_bTestIsPlayingTheme }
};
