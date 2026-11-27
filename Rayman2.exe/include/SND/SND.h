#pragma once

#include <windows.h>

#include "SND/sndtype.h"

typedef union _SND_tduRefEvt {
    struct _SND_tdstBlockEvent *pstPtr;
    long  lIndex;
} SND_tduRefEvt;

#define SND_C_OBJET_FANTOME (-1)

#ifndef abss
#include <stdlib.h>
#define abss(a) abs(a)
#endif

void __cdecl SND_fn_vPauseSound(void);
void __cdecl SND_fn_vResumeSound(void);
BOOL __cdecl SND_fn_bReleaseDriverSound(void);
BOOL __cdecl SND_fn_bRestoreDriverSound(void);

void __cdecl SND_fn_vPauseAllSnd(void);
void __cdecl SND_fn_vResumeAllSnd(void);
void __cdecl SND_fn_vReleaseDriverSnd(void);
void __cdecl SND_fn_vRestoreDriverSnd(void);

void __stdcall SND_fn_vReleaseDriverCD(void);
void __stdcall SND_fn_vReleaseDriverMidi(void);
void __stdcall SND_fn_vReleaseDriverSxd(void);
void __stdcall SND_fn_vRestoreDriverCD(void);
void __stdcall SND_fn_vRestoreDriverMidi(void);
void __stdcall SND_fn_vRestoreDriverSxd(void);

void __stdcall SND_fn_vPauseSnd(long voie);
void __stdcall SND_fn_vResumeSnd(long voie);
void __cdecl   SND_fn_vPauseTransitionSnd(long voice);
void __cdecl   SND_fn_vResumeTransitionSnd(long voice);

void __stdcall SND_fn_vPauseTransitionSxd(long voice);
void __stdcall SND_fn_vResumeTransitionSxd(long voice);
void __stdcall SND_fn_vPauseTransitionMidi(long voice);
void __stdcall SND_fn_vResumeTransitionMidi(long voice);

void __stdcall SND_fn_vPauseSxd(long voice_sxd);
void __stdcall SND_fn_vResumeSxd(long voice_sxd);
void __stdcall SND_fn_vPauseMidi(long voice_sxd);
void __stdcall SND_fn_vResumeMidi(long voice_sxd);
void __stdcall SND_fn_vPauseCD(long voice_sxd);
void __stdcall SND_fn_vResumeCD(long voice_sxd);

void __stdcall SND_fn_vStopSxd(long voice_sxd);
void __stdcall SND_fn_vStopMidi(long voice_sxd);
void __stdcall SND_fn_vStopCD(long voice_sxd);
int  __stdcall SND_fn_bTestIsPlayingSxd(long voice_sxd);
int  __stdcall SND_fn_bTestIsPlayingMidi(long voice_sxd);
int  __stdcall SND_fn_bTestIsPlayingCD(long voice_sxd);

struct SoundParam_;
int  __stdcall SND_fn_bSetParamSxd (long voice_sxd, struct SoundParam_ *par);
int  __stdcall SND_fn_bSetParamMidi(long voice_sxd, struct SoundParam_ *par);
int  __stdcall SND_fn_bSetParamCD  (long voice_sxd, struct SoundParam_ *par);
long __stdcall SND_fn_rGetPosSxd   (long voice_sxd);
long __stdcall SND_fn_rGetPosMidi  (long voice_sxd);
long __stdcall SND_fn_rGetPosCD    (long voice_sxd);

void __stdcall SND_fn_vPauseSwitch(long voice);
void __stdcall SND_fn_vResumeSwitch(long voice);
void __stdcall SND_fn_vPauseRandom(long voice);
void __stdcall SND_fn_vResumeRandom(long voice);
void __stdcall SND_fn_vPauseTheme(long canal);
void __stdcall SND_fn_vResumeTheme(long canal);

void __stdcall SND_fn_vPauseSeq(long voice);
void __stdcall SND_fn_vResumeSeq(long voice);

struct SoundParam_;
int  __stdcall SND_fn_bSetParamSnd(long voie, struct SoundParam_ *par);
int  __cdecl   SND_fn_bTestIsPlayingSnd(long voie);
void __stdcall SND_fn_vStopSnd(long voie);
long __stdcall SND_fn_rGetPosSnd(long voie);
void __cdecl   desassocie_snd(long voice);
void __cdecl   invalidate_ptrvalid_snd(long voice);

void __stdcall SND_fn_vStopSwitch(long voice);
int  __stdcall SND_fn_bTestIsPlayingSwitch(long voice);
long __stdcall SND_fn_rGetPosSwitch(long voice);
void __stdcall SND_fn_vStopRandom(long voice);
int  __stdcall SND_fn_bTestIsPlayingRandom(long voice);
long __stdcall SND_fn_rGetPosRandom(long voice);

int  __stdcall SND_fn_bTestIsPlayingTheme(long canal);

struct SoundParam_;
int  __stdcall SND_fn_bSetParamSwitch(long voice, struct SoundParam_ *par);

void __cdecl SND_fn_vCopySoundParam(struct SoundParam_ *pDest,
                                    struct SoundParam_ *pSrc);

long __cdecl fn_lGetVolumeForType(long type);
long __cdecl fn_lGetVolumeForObject(long indice, long type);
void __cdecl set_SwitchParam(long indice, long type,
                             struct SoundParam_ *pstSoundParam);

/* 0x00451FB0 */
long __cdecl SND_pc_fn_lCallTypeSubTableSlot6(long objectid, long lTypeSwitch, long objecttype);

/* 0x00452B70 */
void __cdecl SND_fn_vSynchroSound(void);

/* 0x00458210 */
void __cdecl SND_fn_vSetMicroParamSnd(long sndId, MicroParam *par);

/* 0x00452510 */
void __cdecl get_param_objet2(long indice,long type,SND_tduRefEvt evt,struct SoundParam_* param,SndReal deTune,struct SoundParam_* pstPreFilledParam);

/* 0x004520A0 */
void __cdecl fill_pre_filled_param(long key,long indice,long type,struct SoundParam_* pstPreFilledParam);

/* 0x00452780 */
void __cdecl get_param_objet(long key,long indice,long type,SND_tduRefEvt evt,struct SoundParam_* param,SndReal deTune);

SndVector __cdecl SND_pc_fn_stAddAxisCombinationToBase(SndVector stBase, SndVector stCoef,
    SndVector stAxisX, SndVector stAxisY, SndVector stAxisZ);
void __cdecl get_RollOff(long indice, long type,
                         struct SoundParam_ *pstSoundParam);
void __cdecl GetMicroParam(long id,MicroParam *par);

SndReal __stdcall SND_fn_rMulRealRealSnd(SndReal r1,SndReal r2);
SndReal __stdcall SND_fn_rMulRealRealQuickSnd(SndReal r1,SndReal r2);
SndReal __stdcall SND_fn_rDivRealRealSnd(SndReal r1,SndReal r2);
void __stdcall SND_fn_vSndRealTwinToSndReal(SndRealTwin,SndReal*,SndReal*);
unsigned char __stdcall SND_fn_ucDistanceToVolume(SndReal dist);
SndReal __stdcall SND_fn_rDopplerPitch(SndVector *src_pos,SndVector *src_vit,SndVector *mic_pos,SndVector *mic_vit);
SndReal __stdcall SND_fn_rPseudoNormeVectorSnd(SndVector *v);
SndReal __stdcall SND_fn_rScalaireVectorSnd(SndVector *v1,SndVector *v2);

SndReal __stdcall SND_fn_rPseudoScalaireNormeVectorSnd(SndVector *v1,SndVector *v2);
void __stdcall SND_fn_vGetDefaultRollOff(RollOffParam *rolloff);
unsigned char __stdcall SND_fn_ucDistanceToVolumeEx(SndReal dist,RollOffParam* rolloff);
void __stdcall SND_fn_vSetDopplerFactor(SndReal factor);
unsigned char __stdcall SND_fn_ucPositionToPan(SndVector *src, SndVector *mic,
                                               SndVector *mic_nor, SndVector *mic_tan);
unsigned char __stdcall SND_fn_ucPositionToDolby(SndVector *src, SndVector *mic,
                                                 SndVector *mic_nor);

/* 0x00452D50 */
long __cdecl SND_fn_lSendRequestSoundIndexed(long lPcExtra,long indice,long type,SND_tduRefEvt evt,long prio,void *proc);

/* 0x00452D20 */
long __cdecl SND_fn_lSendRequestSound(long indice, long type, void *evt,
                                      long prio, void *proc);

/* 0x00451E20 */
void __cdecl stop_un_son_objet(long indice,long type,SND_tduRefEvt evt);
void __cdecl stop_all_son_objet(long indice, long type);

/* 0x00453040 */
void __cdecl SND_fn_vKillObjectSound2(long indice, long type);

/* 0x00453810 */
void __cdecl SND_fn_vChangeVolumeSound(long indice, long type, long lNewVolume);

/* 0x004527D0 */
SndReal __cdecl fn_rGetDeTune(SND_tduRefEvt evt);
void __cdecl SND_fn_vSetEffectSnd(int iIdEffect);

void __cdecl fnSetLastEventForSoundObjectType(long type, struct _SND_tdstBlockEvent *evt);
void __cdecl fn_vChangeVolumeForType(long type, long lNewVolume);
int  __stdcall SND_fn_iGetNumberOfBanks(void);

/* 0x00453370 */
void *__cdecl SND_fn_lAddVolumeLineSound(void);
/* 0x00453680 */
long __cdecl SND_fn_lGetVolumeLineSound(void *pLine);
/* 0x004534D0 */
long __cdecl SND_fn_lAddObjectTypeSound2(void *ptrfct, void *lLine);

void __cdecl fn_vResetVolumeForObject(long indice, long type);
void __cdecl fn_vChangeVolumeForObject(long indice, long type, long lNewVolume);
/* 0x004536A0 */
void __cdecl SND_fn_vSetVolumeLineSound(void *pLine, long lNewVolume);

typedef SndReal (*td_pfn_rRetSoundCoord)(long obj);
typedef unsigned char (*td_pfn_ucRetSoundMidi)(long obj);
typedef void (*td_pfn_vRetSoundExtraCoef)(long obj,SND_tduRefEvt evt,SndReal* pitch,SndReal* pan,SndReal* vol);
typedef long (*td_pfn_lRetSoundLong)(long obj);
typedef long   (*td_pfn_lRetSoundSwitch)(long obj,long type_switch);
typedef SndBool (*td_pfn_bRetSoundLink)(long obj,long micro);
typedef void (*td_pfn_vCopyString)(long obj,char* str,long size_str);
typedef SndBool (*td_pfn_bRetRollOffFactor)(long obj,RollOffParam* rolloff);

typedef void (*td_pc_pfn_vRetSoundAxis)(long obj,SndVector* axis);

typedef struct _tdstAllRetObjectSound tdstAllRetObjectSound;
struct _tdstAllRetObjectSound{
	td_pfn_rRetSoundCoord		X,Y,Z,VX,VY,VZ;
	td_pfn_ucRetSoundMidi		REVERB;
	td_pfn_vRetSoundExtraCoef	ExtraCoef;
	td_pfn_lRetSoundSwitch		Switch;
    td_pfn_bRetSoundLink        MicroLink;
	td_pc_pfn_vRetSoundAxis		pc_AxisX,pc_AxisY,pc_AxisZ;
};

typedef struct _tdstAllRetMicroSound tdstAllRetMicroSound;
struct _tdstAllRetMicroSound{
	td_pfn_rRetSoundCoord		X,Y,Z,VX,VY,VZ;
	td_pfn_rRetSoundCoord		DNX,DNY,DNZ,DTX,DTY,DTZ;
	td_pfn_lRetSoundLong		Filtre;
};

void __cdecl SND_fn_vSetRollOffFunctionForTypeSound(long type,td_pfn_bRetRollOffFactor fct);
/* 0x004535D0 */
void __cdecl SND_fn_vSetInfoForTypeSound(long type,td_pfn_vCopyString fct);
/* 0x00453600 */
void __cdecl SND_fn_vGetInfoForObjectSound(long id, long type, char *str, long size_ptr);
/* 0x00403290 */
void __cdecl SND_PrintUsedStaticMemory(void);
/* 0x00453330 */
void __cdecl SND_fn_vSetRetMicroSound(tdstAllRetMicroSound *ptrfct);
void __cdecl fn_vUpdateTypeObjectSoundInfos(tdstAllRetObjectSound *ptrfct,
                                            tdstAllRetObjectSound *new_ptrfct);

/* 0x452A90 */
void __cdecl SND_fn_vSetDopplerFactorSound(long factor);

int __cdecl SND_fn_bIsMusicPlayingSound(void);

void __stdcall SND_fn_vInitThreadSnd(void);                        /* 0x00454A00 */
void __stdcall SND_fn_vDesInitThreadSnd(void);                     /* 0x00454A40 */
void __stdcall SND_fn_vMouchardThreadsnd(char *szFile, long lLine);/* 0x00454A70 */
void __stdcall SND_pc_fn_vMouchardErrorDisplay(char *szFile, long lLine); /* 0x00454AA0 */
void __stdcall SND_fn_vEnterCriticalSectionThreadSnd_(void);       /* 0x00454AD0 */
int  __stdcall SND_fn_bTryToEnterCriticalSectionThreadSnd(void);   /* 0x00454B70 */
int  __stdcall SND_fn_bEnterCriticalSectionForDriverThreadSnd(void); /* 0x00454D30 */
void __stdcall SND_fn_vQuitCriticalSectionThreadSnd(void);         /* 0x00454C20 */
void __stdcall SND_fn_vWaitForValueThreadSnd(unsigned long *p_ulValue,
                                             unsigned long ulExpected); /* 0x00454C40 */
void __cdecl   SND_pc_fn_vInitErrorDisplayCs(void);                /* 0x00454C80 */
void __cdecl   SND_pc_fn_vDesInitErrorDisplayCs(void);             /* 0x00454CA0 */
void __stdcall SND_fn_vEnterCriticalSectionForErrorDisplay(void);  /* 0x00454CE0 */
void __stdcall SND_fn_vQuitCriticalSectionForErrorDisplay(void);   /* 0x00454D10 */

void __stdcall SND_fn_vDisplayError(unsigned short uwErrNum, char *message);   /* 0x00451450 */
void __stdcall SND_fn_vDisplayErrorEx(unsigned short uwErrNum, char *message,
                                      int flags);                              /* 0x004514B0 */

void __stdcall SND_fn_vFreeSndEx(unsigned char id_block, void *pvPtr);   /* 0x004978F0 */
void *__stdcall SND_fn_pvMallocSndEx(unsigned char id_block, unsigned int ulNbBytes); /* 0x00497660 */
void __cdecl   SND_fn_vCheckMemory(void);   /* 0x403290 */

void SND_fn_vUpdateLastError(unsigned long ulChannelId, unsigned short uwErrNum,
                             long lDebugData, unsigned char ucOpenInfoWindow,
                             unsigned char ucStopForDebug, char *message);     /* 0x00451540 */
