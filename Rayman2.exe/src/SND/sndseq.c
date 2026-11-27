#include <string.h>
#include "SND/sndres.h"

#define E_ucSndBlockMain	((unsigned char)0)

#include "SND/sndparam_pc.h"

typedef struct _tdstActiveSequence tdstActiveSequence;
struct _tdstActiveSequence {
	SndBool		bActive;
	SndBool		bValid;
	SndBool		bPaused;
	SndBool		bNoAvailableTrack;
	SndBool		*pbValid;
	tduRefRes	uRefThisRes;
	tdstInfoTrackMem *pstRefCurrentRes;
	long		lCurrentTrack;
	long		lCurrentTrackVoice;
	unsigned long	uwNbLoops;
	unsigned long	uwStartLoop;
	unsigned long	uwEndLoop;
	SoundParam	*pstSoundParam;
	long		lPrio;
	SND_td_pfn_vSoundCallback pfnSoundCallback;
	long		lParSoundCallback;
	SndBool		bManagingSequence;
};

#define NB_MAX_SEQUENCE                                         10

static tdstActiveSequence stActiveSequence[NB_MAX_SEQUENCE];   /* 0x004B9620 */
#include "SND/snderr.h"

void *__stdcall SND_fn_pvReAllocSndEx(unsigned char id_block, void *pvPtr, unsigned int ulNbBytes);

void *__stdcall SND_fn_pvMallocSndEx(unsigned char id_block, unsigned int ulNbBytes);

void __stdcall SND_fn_vDisplayError(unsigned short uwErrNum, char *message);
void __stdcall SND_fn_vDisplayErrorEx(unsigned short uwErrNum, char *message, int flags);

struct SoundParam_;
long SND_fn_lPlaySnd(tduRefRes res, struct SoundParam_ *par, long prio, SND_td_pfn_vSoundCallback fn_callback, long par_callback, SndBool *valid);

static long get_SoundParam_member_size(SoundParam *par)
{
	return sizeof(SoundParam)+par->iNbLinks*sizeof(MicroLink);
}

static void refresh_SoundParam_member(tdstActiveSequence *seq,SoundParam *new_par)
{
	seq->pstSoundParam=SND_fn_pvReAllocSndEx(E_ucSndBlockMain,seq->pstSoundParam,get_SoundParam_member_size(new_par));
	memcpy(seq->pstSoundParam,new_par,get_SoundParam_member_size(new_par));
}

static void create_SoundParam_member(tdstActiveSequence *seq,SoundParam *new_par)
{
	seq->pstSoundParam=SND_fn_pvMallocSndEx(E_ucSndBlockMain,get_SoundParam_member_size(new_par));
	memcpy(seq->pstSoundParam,new_par,get_SoundParam_member_size(new_par));
}

void __stdcall SND_fn_vFreeSndEx(unsigned char id_block, void *pvPtr);

/* 0x0045AB10 */
static void free_SoundParam_member(tdstActiveSequence *seq)
{
    SND_fn_vFreeSndEx(E_ucSndBlockMain,seq->pstSoundParam);
}

/* 0x0045AB20 */
static void SND_fn_vConvertResDiskToMemSeq(tdstBlockResourceDisk *disk,tdstBlockResourceMem *mem,tdstInfoTrackList *ptr)
{
        mem->Id=disk->Id;
        mem->eType=disk->eType;
        mem->eStorage=disk->eStorage;
        mem->ucVolume=disk->ucVolume;
        mem->bIsLoaded=TRUE;

        mem->uRes.stSequence.bLoop = disk->uRes.stSequence.bLoop;
        mem->uRes.stSequence.ulNbElements = disk->uRes.stSequence.ulNbElements;

        {
            mem->uRes.stSequence.ulStartLoop = disk->uRes.stSequence.ulStartLoop;
            mem->uRes.stSequence.ulEndLoop = disk->uRes.stSequence.ulEndLoop;
            mem->uRes.stSequence.ulNbLoops = disk->uRes.stSequence.ulNbLoops - 1;
        }
}

void *__stdcall SND_fn_pGetBinRes(unsigned long _ulBinId);

/* 0x0045AB70 */
SndBool SND_fn_bLoadResBinarySeq(tdstBlockResourceDisk *_pBRDisk,tdstBlockResourceMem *_pBRMem,char *pDataBloc)
{
    tdstInfoTrackMem *pTrackArray,*pTrackElement;

    int iPos;

    pTrackArray=(tdstInfoTrackMem *)(pDataBloc + _pBRDisk->ulDataOffset);

	SNDLST2_M_StaticSetFirstElement(&_pBRMem->uRes.stSequence.stResList,pTrackArray);
	SNDLST2_M_StaticSetNumberOfElements(&_pBRMem->uRes.stSequence.stResList,
				_pBRDisk->uRes.stSequence.ulNbElements);

    SNDLST2_M_StaticForEachElementOf( &_pBRMem->uRes.stSequence.stResList, pTrackElement, iPos)
    {
		pTrackElement->uRes.pstPtr=SND_fn_pGetBinRes(pTrackElement->uRes.Id);
    }

	SND_fn_vConvertResDiskToMemSeq(_pBRDisk,_pBRMem,NULL);
	return TRUE;
}

static SndBool bSequenceDriverOk;
static unsigned char ucSequenceCapacities;
static unsigned long iNbSequences;

/* 0x0045ABD0 */
int SND_fn_iInitSeq(void)
{
        int i;

        for (i=0;i<NB_MAX_SEQUENCE;i++)
        {
                stActiveSequence[i].bActive = FALSE;
                stActiveSequence[i].pstSoundParam=NULL;
                stActiveSequence[i].bPaused = FALSE;
                stActiveSequence[i].lCurrentTrack = -1;
                stActiveSequence[i].lCurrentTrackVoice = C_PLAY_FAILED;
                stActiveSequence[i].uwNbLoops = 0;
                stActiveSequence[i].uwStartLoop = 0;
                stActiveSequence[i].uwEndLoop = 0;
                stActiveSequence[i].lPrio = 0;
                stActiveSequence[i].pfnSoundCallback= NULL;
                stActiveSequence[i].lParSoundCallback=0;
                stActiveSequence[i].bNoAvailableTrack = TRUE;
        }

    bSequenceDriverOk=TRUE;
    ucSequenceCapacities = 0;
    iNbSequences = 0;

    return C_INIT_OK;
}

/* 0x0045AC30 */
void SND_fn_vDesInitSeq(void)
{
        int i;

        for (i=0;i<NB_MAX_SEQUENCE;i++)
        {
                if (stActiveSequence[i].pstSoundParam)
                        free_SoundParam_member(&stActiveSequence[i]);
                stActiveSequence[i].bActive = FALSE;
                stActiveSequence[i].bPaused = FALSE;
                stActiveSequence[i].lCurrentTrack = -1;
                stActiveSequence[i].lCurrentTrackVoice = C_PLAY_FAILED;
                stActiveSequence[i].uwNbLoops = 0;
                stActiveSequence[i].uwStartLoop = 0;
                stActiveSequence[i].uwEndLoop = 0;
                stActiveSequence[i].lPrio = 0;
                stActiveSequence[i].pfnSoundCallback= NULL;
                stActiveSequence[i].lParSoundCallback=0;
                stActiveSequence[i].bNoAvailableTrack = TRUE;
        }
}

typedef struct SND_pc_tdstSeqCallbackArg_ SND_pc_tdstSeqCallbackArg;
struct SND_pc_tdstSeqCallbackArg_
{
    void (__stdcall *pfnCallback)(long);
    long lParCallback;
};

static void *SND_pc_g_hSeqCallbackThread;                     /* 0x00514D3C */
static unsigned long SND_pc_g_dwSeqCallbackThreadId;          /* 0x00514D40 */
static SND_pc_tdstSeqCallbackArg SND_pc_g_stSeqCallbackArg;   /* 0x00514D48 */

void __stdcall SND_fn_vEnterCriticalSectionThreadSnd_(void);
void __stdcall SND_fn_vQuitCriticalSectionThreadSnd(void);
void __stdcall SND_fn_vMouchardThreadsnd(char *szFile, long lLine);

/* 0x0045AC90 */
static unsigned long __stdcall SND_pc_fn_ulSeqCallbackThread(SND_pc_tdstSeqCallbackArg *pArg)
{
    void (__stdcall *pfn)(long);
    long lPar;

    pfn = pArg->pfnCallback;
    lPar = pArg->lParCallback;
    SND_fn_vEnterCriticalSectionThreadSnd_();
    SND_fn_vMouchardThreadsnd("..\\..\\..\\src\\Sndseq2.c", 0x1BB);
    if (pfn != NULL)
    {
        pfn(lPar);
    }
    SND_fn_vQuitCriticalSectionThreadSnd();
    return 0;
}

int  __stdcall SND_fn_bSetParamSnd(long voie, struct SoundParam_ *par);
int  __cdecl   SND_fn_bTestIsPlayingSnd(long voie);
void __stdcall SND_fn_vNextElement(long voice);

static void test_next_element(long voice,SoundParam* par,SndBool bSetParam)
{
	SndBool bRet;

	if (stActiveSequence[voice].bValid)
	{
		if (bSetParam)
			bRet=SND_fn_bSetParamSnd(stActiveSequence[voice].lCurrentTrackVoice ,(struct SoundParam_ *)par);
		else
			bRet=SND_fn_bTestIsPlayingSnd(stActiveSequence[voice].lCurrentTrackVoice);
	}
	else
		bRet=FALSE;

	if (!bRet)
	{
		stActiveSequence[voice].lCurrentTrackVoice=C_PLAY_FAILED;

		SND_fn_vNextElement(voice);
	}
}

/* 0x0045AD30 */
long SND_fn_lPlaySeq(tduRefRes res,SoundParam *par,long prio,SND_td_pfn_vSoundCallback fn_callback,long par_callback,SndBool *valid)
{
	long voice;
	tdstInfoTrackMem * pstFirstRes;
	long result;

	if (valid!=NULL)
		*valid=FALSE;

	for (voice=0;voice<NB_MAX_SEQUENCE;voice++)
		if (!stActiveSequence[voice].bActive) break;
	if (voice==NB_MAX_SEQUENCE)
	{
		SND_fn_vDisplayErrorEx(E_uwSndTooManySequences,"",SNDERR_DISPLAY_IF_WANTED);
		return C_PLAY_FAILED;
	}

	stActiveSequence[voice].bActive=TRUE;

	if (res.pstPtr->uRes.stSequence.ulNbElements != 0)
	{
		if ((res.pstPtr->uRes.stSequence.ulStartLoop < res.pstPtr->uRes.stSequence.ulNbElements)
				&& (res.pstPtr->uRes.stSequence.ulEndLoop < res.pstPtr->uRes.stSequence.ulNbElements))
		{
			pstFirstRes=SNDLST2_M_StaticGetFirstElement(&(res.pstPtr->uRes.stSequence.stResList));
			stActiveSequence[voice].pstRefCurrentRes=pstFirstRes;

			result=SND_fn_lPlaySnd(pstFirstRes->uRes,(struct SoundParam_ *)par,prio,NULL,0,&(stActiveSequence[voice].bValid));

			if (result!=C_PLAY_FAILED)
			{

				stActiveSequence[voice].uRefThisRes=res;
				stActiveSequence[voice].pstRefCurrentRes=pstFirstRes;
				stActiveSequence[voice].uwNbLoops=res.pstPtr->uRes.stSequence.ulNbLoops;
				stActiveSequence[voice].uwStartLoop=res.pstPtr->uRes.stSequence.ulStartLoop;
				stActiveSequence[voice].uwEndLoop=res.pstPtr->uRes.stSequence.ulEndLoop;
				stActiveSequence[voice].bNoAvailableTrack=FALSE;
				stActiveSequence[voice].pbValid=valid;
				stActiveSequence[voice].lPrio=prio;
				if (stActiveSequence[voice].pstSoundParam!=NULL)
					refresh_SoundParam_member(&stActiveSequence[voice],par);
				else
					create_SoundParam_member(&stActiveSequence[voice],par);
				stActiveSequence[voice].pfnSoundCallback=fn_callback;
				stActiveSequence[voice].lParSoundCallback=par_callback;
				stActiveSequence[voice].bActive=TRUE;
				stActiveSequence[voice].bPaused=FALSE;
				stActiveSequence[voice].lCurrentTrack=0;
				stActiveSequence[voice].lCurrentTrackVoice=result;
				stActiveSequence[voice].bManagingSequence=FALSE;
				if (stActiveSequence[voice].pbValid!=NULL)
					*(stActiveSequence[voice].pbValid)=TRUE;
				return (voice);
			}
			else
			{
				stActiveSequence[voice].bActive=FALSE;
				return (C_PLAY_FAILED);
			}
		}
		else
		{
			SND_fn_vDisplayError(E_uwSndInvalidStaticParamForResource,"");
			return (C_PLAY_FAILED);
		}
	}
	return (C_PLAY_FAILED);
}

#include <windows.h>

/* 0x0045AED0 */
void __stdcall SND_fn_vNextElement(long voice)
{
        tdstInfoTrackMem * pstNextRes=NULL;
        long result;
        unsigned long ulEndLoopingTrack;
        unsigned long ulEndingTrack;
        SndBool sequence_is_finished;
		int i;
		SND_pc_tdstThreadSecurity stSecurity;

        stActiveSequence[voice].bManagingSequence=TRUE;

        result=C_PLAY_FAILED;
        sequence_is_finished=FALSE;

        if (stActiveSequence[voice].bActive)
        {
                do
                {

                        stActiveSequence[voice].lCurrentTrackVoice = C_PLAY_FAILED;

												ulEndLoopingTrack = stActiveSequence[voice].uRefThisRes.pstPtr->uRes.stSequence.ulEndLoop;
												ulEndingTrack = stActiveSequence[voice].uRefThisRes.pstPtr->uRes.stSequence.ulNbElements - 1;

												if (stActiveSequence[voice].uwNbLoops != (unsigned long)-1)
												{
													if (stActiveSequence[voice].lCurrentTrack < ulEndLoopingTrack)
													{
														pstNextRes = SNDLST2_M_StaticGetNextElement(stActiveSequence[voice].pstRefCurrentRes);
														stActiveSequence[voice].lCurrentTrack++;
													}
													else
													{
														SNDLST2_M_StaticGetElementNumber(&(stActiveSequence[voice].uRefThisRes.pstPtr->uRes.stSequence.stResList),pstNextRes,(long)stActiveSequence[voice].uRefThisRes.pstPtr->uRes.stSequence.ulStartLoop,i);
														stActiveSequence[voice].lCurrentTrack=(short)stActiveSequence[voice].uRefThisRes.pstPtr->uRes.stSequence.ulStartLoop;
														stActiveSequence[voice].uwNbLoops--;
													}
												}
												else
												{
													if (stActiveSequence[voice].lCurrentTrack < ulEndingTrack)
													{
														pstNextRes = SNDLST2_M_StaticGetNextElement(stActiveSequence[voice].pstRefCurrentRes);
														stActiveSequence[voice].lCurrentTrack++;
													}
													else
													{
														if (stActiveSequence[voice].uRefThisRes.pstPtr->uRes.stSequence.bLoop)
														{
															SNDLST2_M_StaticGetElementNumber(&(stActiveSequence[voice].uRefThisRes.pstPtr->uRes.stSequence.stResList),pstNextRes,0,i);
															stActiveSequence[voice].lCurrentTrack=(short)0;
															stActiveSequence[voice].uwNbLoops = stActiveSequence[voice].uRefThisRes.pstPtr->uRes.stSequence.ulNbLoops;
														}
														else
														{
															sequence_is_finished=TRUE;
															stActiveSequence[voice].bActive=FALSE;
														}
													}

												}

                        if (!sequence_is_finished)
                        {

                          result=SND_fn_lPlaySnd(pstNextRes->uRes,(struct SoundParam_ *)stActiveSequence[voice].pstSoundParam,stActiveSequence[voice].lPrio,NULL,0,&(stActiveSequence[voice].bValid));

													if (result != C_PLAY_FAILED)
													{
														stActiveSequence[voice].bActive = TRUE;
														stActiveSequence[voice].pstRefCurrentRes = pstNextRes;
														stActiveSequence[voice].lCurrentTrackVoice = result;
														stActiveSequence[voice].bNoAvailableTrack = FALSE;
													}
                        }
												else
												{
  												if (stActiveSequence[voice].pfnSoundCallback)
													{

														SND_pc_M_vInitThreadSecurity(stSecurity);

														SND_pc_g_stSeqCallbackArg.pfnCallback = stActiveSequence[voice].pfnSoundCallback;
														SND_pc_g_stSeqCallbackArg.lParCallback = stActiveSequence[voice].lParSoundCallback;
														SND_pc_g_hSeqCallbackThread = SND_pc_M_hCreateThread(&stSecurity,
															(SND_pc_tdpfnThreadProc)SND_pc_fn_ulSeqCallbackThread,
															&SND_pc_g_stSeqCallbackArg, &SND_pc_g_dwSeqCallbackThreadId);
														if (SND_pc_g_hSeqCallbackThread == NULL)
															SND_fn_vDisplayError(0x27, "");
														stActiveSequence[voice].pfnSoundCallback = NULL;
														stActiveSequence[voice].lParSoundCallback = 0;
													}
												}
                } while (!sequence_is_finished && (result==C_PLAY_FAILED));
        }

        if (sequence_is_finished || (result==C_PLAY_FAILED))
        {
                *(stActiveSequence[voice].pbValid)=FALSE;
        }

        stActiveSequence[voice].bManagingSequence=FALSE;
}

void __stdcall SND_fn_vStopSnd(long voie);

/* 0x0045B0B0 */
void __stdcall SND_fn_vStopSeq(long voice)
{
		SND_pc_tdstThreadSecurity stSecurity;

        if (stActiveSequence[voice].bActive)
        {
                stActiveSequence[voice].bActive=FALSE;

                if (stActiveSequence[voice].bValid)
					SND_fn_vStopSnd(stActiveSequence[voice].lCurrentTrackVoice);

  			if (stActiveSequence[voice].pfnSoundCallback)
				{

					SND_pc_M_vInitThreadSecurity(stSecurity);

					SND_pc_g_stSeqCallbackArg.pfnCallback = stActiveSequence[voice].pfnSoundCallback;
					SND_pc_g_stSeqCallbackArg.lParCallback = stActiveSequence[voice].lParSoundCallback;
					SND_pc_g_hSeqCallbackThread = SND_pc_M_hCreateThread(&stSecurity,
						(SND_pc_tdpfnThreadProc)SND_pc_fn_ulSeqCallbackThread,
						&SND_pc_g_stSeqCallbackArg, &SND_pc_g_dwSeqCallbackThreadId);
					if (SND_pc_g_hSeqCallbackThread == NULL)
						SND_fn_vDisplayError(0x27, "");
					stActiveSequence[voice].pfnSoundCallback = NULL;
					stActiveSequence[voice].lParSoundCallback = 0;
				}

                stActiveSequence[voice].bActive = FALSE;
                *(stActiveSequence[voice].pbValid)= FALSE;
                stActiveSequence[voice].lCurrentTrack = -1;
                stActiveSequence[voice].lCurrentTrackVoice = C_PLAY_FAILED;
        }
}

void __stdcall SND_fn_vPauseSnd(long voie);
void __stdcall SND_fn_vResumeSnd(long voie);

/* 0x0045B180 */
void __stdcall SND_fn_vPauseSeq(long voice)
{

        if (stActiveSequence[voice].bActive)
        {

                if ((stActiveSequence[voice].bValid) && (!stActiveSequence[voice].bPaused))
                {
					SND_fn_vPauseSnd(stActiveSequence[voice].lCurrentTrackVoice);
                    stActiveSequence[voice].bPaused = TRUE;
                }
        }

}

/* 0x0045B1D0 */
void __stdcall SND_fn_vResumeSeq(long voice)
{

        if (stActiveSequence[voice].bActive)
        {

                if (stActiveSequence[voice].bPaused)
                {
                    SND_fn_vResumeSnd(stActiveSequence[voice].lCurrentTrackVoice);
                    stActiveSequence[voice].bPaused = FALSE;
                }
        }
}

/* 0x0045B210 */
SndBool __stdcall SND_fn_bSetParamSeq(long voice,SoundParam *par)
{

        if (stActiveSequence[voice].bActive)
        {
            refresh_SoundParam_member(&stActiveSequence[voice],par);
			test_next_element(voice,par,TRUE);
            return stActiveSequence[voice].bActive;
        }
        else
                return FALSE;
}

/* 0x0045B260 */
SndBool __stdcall SND_fn_bTestIsPlayingSeq(long voice)
{
	test_next_element(voice,NULL,FALSE);

    return (stActiveSequence[voice].bActive);
}
