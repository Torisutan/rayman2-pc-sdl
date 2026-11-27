#include <stddef.h>
#include <string.h>

#include "SND/SND.h"
#include "SND/SND_globals.h"

static SndBool g_bInitDoneSound;   /* 0x004B74EC */

static const char SND_C_szSound3DSrcFile[] = "..\\..\\..\\src\\sound3d.c";   /* 0x004A24E0 */

typedef void (__stdcall *t_SND_fn_vEnterCriticalSectionThreadSnd)(void);
typedef void (__stdcall *t_SND_fn_vQuitCriticalSectionThreadSnd)(void);
typedef int  (__stdcall *t_SND_fn_bEnterCriticalSectionForDriverThreadSnd)(void);
typedef void (__stdcall *t_SND_fn_vMouchardThreadsnd)(const char *szFile, int iLine);

#define SND_RT_fn_vEnterCriticalSectionThreadSnd \
    SND_fn_vEnterCriticalSectionThreadSnd_
#define SND_RT_fn_vQuitCriticalSectionThreadSnd \
    SND_fn_vQuitCriticalSectionThreadSnd
#define SND_RT_fn_vMouchardThreadsnd(szFile, iLine) \
    SND_fn_vMouchardThreadsnd((char *)(szFile), (long)(iLine))
#define SND_RT_fn_bEnterCriticalSectionForDriverThreadSnd \
    SND_fn_bEnterCriticalSectionForDriverThreadSnd

typedef void (__stdcall *t_SND_fn_vDisplayError)(unsigned short, const char *);

#define SND_RT_fn_vDisplayError(uwErr, szMsg) \
    SND_fn_vDisplayError((uwErr), (char *)(szMsg))

#define SND_C_szEmpty                     ("")
#include "SND/snderr.h"

#include "SND/sndres.h"

#include "SND/sndevent.h"
#include "SND/sndhard.h"

#include "SND/sndparam_pc.h"
#include "SND/sndreal.h"

typedef void    (__cdecl   *t_SND_DriverFn)(void);
typedef void    (__stdcall *t_SND_TransitionFn)(long);
typedef void    (__stdcall *t_SND_PerTypeFn)(long);
typedef SndBool (__stdcall *t_SND_PerTypeBoolFn)(long);
typedef SndBool (__stdcall *t_SND_PerTypeSetParamFn)(long, struct SoundParam_ *);
typedef SndReal (__stdcall *t_SND_PerTypeRealFn)(long);

#if defined(_DLL_COMPILATION_MODE)

t_SND_TransitionFn g_pfn_SND_PauseTransitionSxd;
t_SND_TransitionFn g_pfn_SND_ResumeTransitionSxd;
t_SND_DriverFn     g_pfn_SND_ReleaseDriverSxd;
t_SND_DriverFn     g_pfn_SND_RestoreDriverSxd;
t_SND_DriverFn     g_pfn_SND_ReleaseDriverMidi;
t_SND_DriverFn     g_pfn_SND_RestoreDriverMidi;
t_SND_TransitionFn g_pfn_SND_ResumeTransitionMidi;
t_SND_TransitionFn g_pfn_SND_PauseTransitionMidi;
t_SND_DriverFn     g_pfn_SND_ReleaseDriverCD;
t_SND_DriverFn     g_pfn_SND_RestoreDriverCD;

static t_SND_PerTypeFn g_pfn_SND_PauseSxd_s,  g_pfn_SND_ResumeSxd_s,
                       g_pfn_SND_PauseMidi_s, g_pfn_SND_ResumeMidi_s,
                       g_pfn_SND_PauseCD_s,   g_pfn_SND_ResumeCD_s;
#define g_pfn_SND_PauseSxd             g_pfn_SND_PauseSxd_s
#define g_pfn_SND_ResumeSxd            g_pfn_SND_ResumeSxd_s
#define g_pfn_SND_PauseMidi            g_pfn_SND_PauseMidi_s
#define g_pfn_SND_ResumeMidi           g_pfn_SND_ResumeMidi_s
#define g_pfn_SND_PauseCD              g_pfn_SND_PauseCD_s
#define g_pfn_SND_ResumeCD             g_pfn_SND_ResumeCD_s

t_SND_PerTypeFn     g_pfn_SND_StopSxd;
t_SND_PerTypeFn     g_pfn_SND_StopMidi;
t_SND_PerTypeFn     g_pfn_SND_StopCD;
t_SND_PerTypeBoolFn g_pfn_SND_TestIsPlayingSxd;
t_SND_PerTypeBoolFn g_pfn_SND_TestIsPlayingMidi;
t_SND_PerTypeBoolFn g_pfn_SND_TestIsPlayingCD;

t_SND_PerTypeSetParamFn g_pfn_SND_SetParamSxd;
t_SND_PerTypeSetParamFn g_pfn_SND_SetParamMidi;
t_SND_PerTypeSetParamFn g_pfn_SND_SetParamCD;
t_SND_PerTypeRealFn     g_pfn_SND_GetPosSxd;
t_SND_PerTypeRealFn     g_pfn_SND_GetPosMidi;
t_SND_PerTypeRealFn     g_pfn_SND_GetPosCD;
#endif

#include "SND/sndplay_pc.h"
#include "SND/sndplay.h"
#include "SND/sndspace.h"
#include "SND/sndldbin.h"

/* 0x004B8C98 */
void __cdecl SND_fn_vPauseSound(void)
{
    if (g_bInitDoneSound != 0)
    {
        SND_RT_fn_vEnterCriticalSectionThreadSnd();
        SND_RT_fn_vMouchardThreadsnd(SND_C_szSound3DSrcFile, 0xB00);
        SND_fn_vPauseAllSnd();
        SND_RT_fn_vQuitCriticalSectionThreadSnd();
    }
}

void __cdecl SND_fn_vResumeSound(void)
{
    if (g_bInitDoneSound != 0)
    {
        SND_RT_fn_vEnterCriticalSectionThreadSnd();
        SND_RT_fn_vMouchardThreadsnd(SND_C_szSound3DSrcFile, 0xB0E);
        SND_fn_vResumeAllSnd();
        SND_RT_fn_vQuitCriticalSectionThreadSnd();
    }
}

BOOL __cdecl SND_fn_bReleaseDriverSound(void)
{
    if (g_bInitDoneSound != 0)
    {
        if (SND_RT_fn_bEnterCriticalSectionForDriverThreadSnd() != 0)
        {
            SND_fn_vReleaseDriverSnd();
            SND_RT_fn_vQuitCriticalSectionThreadSnd();
            return TRUE;
        }
    }
    return FALSE;
}

BOOL __cdecl SND_fn_bRestoreDriverSound(void)
{
    if (g_bInitDoneSound != 0)
    {
        if (SND_RT_fn_bEnterCriticalSectionForDriverThreadSnd() != 0)
        {
            SND_fn_vRestoreDriverSnd();
            SND_RT_fn_vQuitCriticalSectionThreadSnd();
            return TRUE;
        }
    }
    return FALSE;
}

/* 0x00453790 */
void __cdecl SND_fn_vReleaseDriverSound(void)
{
    SND_fn_bReleaseDriverSound();
}

/* 0x004537A0 */
void __cdecl SND_fn_vRestoreDriverSound(void)
{
    SND_fn_bRestoreDriverSound();
}

void __stdcall SND_fn_vSetStereoSxd(SndBool active);
void __cdecl SND_fn_vSetStereoSnd(SndBool active);

void __cdecl SND_fn_vSetStereoSound(SndBool active)
{
    SND_fn_vSetStereoSnd(active);
}

/* 0x004537C0 */
SndBool __stdcall SND_fn_bGetStereoSxd(void);
SndBool __cdecl SND_fn_bGetStereoSnd(void);

SndBool __cdecl SND_fn_bGetStereoSound(void)
{
    return SND_fn_bGetStereoSnd();
}

SndBool __cdecl SND_fn_bIsMusicPlayingSnd(void);

/* 0x004537D0 */
SndBool __cdecl SND_fn_bIsMusicPlayingSound(void)
{
    return SND_fn_bIsMusicPlayingSnd();
}

#if defined(_DLL_COMPILATION_MODE)

void __stdcall SND_fn_vReleaseDriverSxd(void)
{
    if (g_pfn_SND_ReleaseDriverSxd == NULL) DebugBreak();
    g_pfn_SND_ReleaseDriverSxd();
}

void __stdcall SND_fn_vRestoreDriverSxd(void)
{
    if (g_pfn_SND_RestoreDriverSxd == NULL) DebugBreak();
    g_pfn_SND_RestoreDriverSxd();
}
void __stdcall SND_fn_vReleaseDriverMidi(void)
{
    if (g_pfn_SND_ReleaseDriverMidi == NULL) DebugBreak();
    g_pfn_SND_ReleaseDriverMidi();
}

#pragma optimize("", on)
void __stdcall SND_fn_vRestoreDriverMidi(void)
{
    if (g_pfn_SND_RestoreDriverMidi == NULL) DebugBreak();
    g_pfn_SND_RestoreDriverMidi();
}
void __stdcall SND_fn_vReleaseDriverCD(void)
{
    if (g_pfn_SND_ReleaseDriverCD == NULL) DebugBreak();
    g_pfn_SND_ReleaseDriverCD();
}

#pragma optimize("", on)
void __stdcall SND_fn_vRestoreDriverCD(void)
{
    if (g_pfn_SND_RestoreDriverCD == NULL) DebugBreak();
    g_pfn_SND_RestoreDriverCD();
}
#pragma optimize("", off)
#pragma optimize("", on)

/* 0x00458470 */
void __stdcall SND_fn_vPauseTransitionSxd(long voice)
{
    if (g_pfn_SND_PauseTransitionSxd == NULL) DebugBreak();
    g_pfn_SND_PauseTransitionSxd(voice);
}

void __stdcall SND_fn_vResumeTransitionSxd(long voice)
{
    if (g_pfn_SND_ResumeTransitionSxd == NULL) DebugBreak();
    g_pfn_SND_ResumeTransitionSxd(voice);
}

void __stdcall SND_fn_vPauseTransitionMidi(long voice)
{
    if (g_pfn_SND_PauseTransitionMidi == NULL) DebugBreak();
    g_pfn_SND_PauseTransitionMidi(voice);
}

void __stdcall SND_fn_vResumeTransitionMidi(long voice)
{
    if (g_pfn_SND_ResumeTransitionMidi == NULL) DebugBreak();
    g_pfn_SND_ResumeTransitionMidi(voice);
}
#pragma optimize("", on)

void __stdcall SND_fn_vPauseSxd(long voice_sxd)
{
    if (g_pfn_SND_PauseSxd == NULL) DebugBreak();
    g_pfn_SND_PauseSxd(voice_sxd);
}

/* 0x00455040 */
void __stdcall SND_fn_vResumeSxd(long voice_sxd)
{
    if (g_pfn_SND_ResumeSxd == NULL) DebugBreak();
    g_pfn_SND_ResumeSxd(voice_sxd);
}

void __stdcall SND_fn_vPauseMidi(long voice_sxd)
{
    if (g_pfn_SND_PauseMidi == NULL) DebugBreak();
    g_pfn_SND_PauseMidi(voice_sxd);
}

void __stdcall SND_fn_vResumeMidi(long voice_sxd)
{
    if (g_pfn_SND_ResumeMidi == NULL) DebugBreak();
    g_pfn_SND_ResumeMidi(voice_sxd);
}

void __stdcall SND_fn_vPauseCD(long voice_sxd)
{
    if (g_pfn_SND_PauseCD == NULL) DebugBreak();
    g_pfn_SND_PauseCD(voice_sxd);
}

void __stdcall SND_fn_vResumeCD(long voice_sxd)
{
    if (g_pfn_SND_ResumeCD == NULL) DebugBreak();
    g_pfn_SND_ResumeCD(voice_sxd);
}

static t_SND_PerTypeFn g_pfn_SND_SetStereoSxd_s;
#define g_pfn_SND_SetStereoSxd  g_pfn_SND_SetStereoSxd_s

void __stdcall SND_fn_vSetStereoSxd(SndBool active)
{
    if (g_pfn_SND_SetStereoSxd == NULL) DebugBreak();
    g_pfn_SND_SetStereoSxd(active);
}

/* 0x00455090 */
typedef SndBool (__stdcall *t_SND_GetBoolFn)(void);
static t_SND_GetBoolFn g_pfn_SND_GetStereoSxd_s;
#define g_pfn_SND_GetStereoSxd  g_pfn_SND_GetStereoSxd_s

SndBool __stdcall SND_fn_bGetStereoSxd(void)
{
    if (g_pfn_SND_GetStereoSxd == NULL) DebugBreak();
    return g_pfn_SND_GetStereoSxd();
}

/* 0x004550F0 */
static t_SND_PerTypeFn g_pfn_SND_SetEffectSxd_s;
#define g_pfn_SND_SetEffectSxd  g_pfn_SND_SetEffectSxd_s

void __stdcall SND_fn_vSetEffectSxd(long num)
{
    if (g_pfn_SND_SetEffectSxd == NULL) DebugBreak();
    g_pfn_SND_SetEffectSxd(num);
}

void __stdcall SND_fn_vStopSxd(long voice_sxd)
{
    if (g_pfn_SND_StopSxd == NULL) DebugBreak();
    g_pfn_SND_StopSxd(voice_sxd);
}

void __stdcall SND_fn_vStopMidi(long voice_sxd)
{
    if (g_pfn_SND_StopMidi == NULL) DebugBreak();
    g_pfn_SND_StopMidi(voice_sxd);
}

void __stdcall SND_fn_vStopCD(long voice_sxd)
{
    if (g_pfn_SND_StopCD == NULL) DebugBreak();
    g_pfn_SND_StopCD(voice_sxd);
}

SndBool __stdcall SND_fn_bTestIsPlayingSxd(long voice_sxd)
{
    if (g_pfn_SND_TestIsPlayingSxd == NULL) DebugBreak();
    return g_pfn_SND_TestIsPlayingSxd(voice_sxd);
}

SndBool __stdcall SND_fn_bTestIsPlayingMidi(long voice_sxd)
{
    if (g_pfn_SND_TestIsPlayingMidi == NULL) DebugBreak();
    return g_pfn_SND_TestIsPlayingMidi(voice_sxd);
}

SndBool __stdcall SND_fn_bTestIsPlayingCD(long voice_sxd)
{
    if (g_pfn_SND_TestIsPlayingCD == NULL) DebugBreak();
    return g_pfn_SND_TestIsPlayingCD(voice_sxd);
}

SndBool __stdcall SND_fn_bSetParamSxd(long voice_sxd, struct SoundParam_ *par)
{
    if (g_pfn_SND_SetParamSxd == NULL) DebugBreak();
    return g_pfn_SND_SetParamSxd(voice_sxd, par);
}

SndBool __stdcall SND_fn_bSetParamMidi(long voice_sxd, struct SoundParam_ *par)
{
    if (g_pfn_SND_SetParamMidi == NULL) DebugBreak();
    return g_pfn_SND_SetParamMidi(voice_sxd, par);
}

SndBool __stdcall SND_fn_bSetParamCD(long voice_sxd, struct SoundParam_ *par)
{
    if (g_pfn_SND_SetParamCD == NULL) DebugBreak();
    return g_pfn_SND_SetParamCD(voice_sxd, par);
}

SndReal __stdcall SND_fn_rGetPosSxd(long voice_sxd)
{
    if (g_pfn_SND_GetPosSxd == NULL) DebugBreak();
    return g_pfn_SND_GetPosSxd(voice_sxd);
}

SndReal __stdcall SND_fn_rGetPosMidi(long voice_sxd)
{
    if (g_pfn_SND_GetPosMidi == NULL) DebugBreak();
    return g_pfn_SND_GetPosMidi(voice_sxd);
}

SndReal __stdcall SND_fn_rGetPosCD(long voice_sxd)
{
    if (g_pfn_SND_GetPosCD == NULL) DebugBreak();
    return g_pfn_SND_GetPosCD(voice_sxd);
}

typedef int       (__stdcall *t_SND_IntPtrFn)(void *);
typedef long      (__stdcall *t_SND_LongPtrFn)(MicroParam *);
typedef void      (__stdcall *t_SND_VoidFn)(void);
typedef void      (__stdcall *t_SND_VoidPtrFn)(void *);
typedef void      (__stdcall *t_SND_UnLoadResFn)(tdstBlockResourceMem *);
typedef SndBool   (__stdcall *t_SND_LoadResFn)(tdstBlockResourceDisk *, tdstBlockResourceMem *, char *);
typedef SndBool   (__stdcall *t_SND_LongLongBoolFn)(long, long);
typedef SndBool   (__stdcall *t_SND_LongLongRealBoolFn)(long, long, SndReal);
typedef long      (__stdcall *t_SND_PlayTransitionFn)(tduRefRes, tduRefRes, SND_td_pfn_vSoundCallback, long, struct SoundParam_ *);
typedef void      (__stdcall *t_SND_RollOffFn)(RollOffParam *);

static t_SND_GetBoolFn g_pfn_SND_CanFreeDataMPEG_s;   /* 0x004B864C */
SndBool __stdcall SND_fn_bCanFreeDataMPEG(void)
{
    if (g_pfn_SND_CanFreeDataMPEG_s == NULL) DebugBreak();
    return g_pfn_SND_CanFreeDataMPEG_s();
}

static t_SND_IntPtrFn g_pfn_SND_InitSxd_s;            /* 0x004B8684 */
int __stdcall SND_fn_iInitSxd(void *pInitStruct)
{
    if (g_pfn_SND_InitSxd_s == NULL) DebugBreak();
    return g_pfn_SND_InitSxd_s(pInitStruct);
}

static t_SND_VoidFn g_pfn_SND_DesInitSxd_s;           /* 0x004B868C */
void __stdcall SND_fn_vDesInitSxd(void)
{
    if (g_pfn_SND_DesInitSxd_s == NULL) DebugBreak();
    g_pfn_SND_DesInitSxd_s();
}

static t_SND_LongPtrFn g_pfn_SND_CreateMicroSxd_s;    /* 0x004B8698 */
long __stdcall SND_fn_lCreateMicroSxd(MicroParam *par)
{
    if (g_pfn_SND_CreateMicroSxd_s == NULL) DebugBreak();
    return g_pfn_SND_CreateMicroSxd_s(par);
}

static t_SND_PerTypeFn g_pfn_SND_DestroyMicroSxd_s;   /* 0x004B86A0 */
void __stdcall SND_fn_vDestroyMicroSxd(long sxdId)
{
    if (g_pfn_SND_DestroyMicroSxd_s == NULL) DebugBreak();
    g_pfn_SND_DestroyMicroSxd_s(sxdId);
}

static t_SND_LoadResFn g_pfn_SND_LoadResBinarySxd_s;  /* 0x004B86D8 */
SndBool __stdcall SND_fn_bLoadResBinarySxd(tdstBlockResourceDisk *disk, tdstBlockResourceMem *mem, char *pDataBloc)
{
    if (g_pfn_SND_LoadResBinarySxd_s == NULL) DebugBreak();
    return g_pfn_SND_LoadResBinarySxd_s(disk, mem, pDataBloc);
}

static t_SND_UnLoadResFn g_pfn_SND_UnLoadResSxd_s;    /* 0x004B86DC */
void __stdcall SND_fn_vUnLoadResSxd(tdstBlockResourceMem *mem)
{
    if (g_pfn_SND_UnLoadResSxd_s == NULL) DebugBreak();
    g_pfn_SND_UnLoadResSxd_s(mem);
}

static t_SND_PlayTransitionFn g_pfn_SND_PlayTransitionExSxd_s; /* 0x004B86F0 */
long __stdcall SND_fn_lPlayTransitionExSxd(tduRefRes FirstRes, tduRefRes NextRes, SND_td_pfn_vSoundCallback transition_callback,
                                           long param_callback, struct SoundParam_ *par)
{
    if (g_pfn_SND_PlayTransitionExSxd_s == NULL) DebugBreak();
    return g_pfn_SND_PlayTransitionExSxd_s(FirstRes, NextRes, transition_callback, param_callback, par);
}

static t_SND_PerTypeSetParamFn g_pfn_SND_SetParamTransitionSxd_s; /* 0x004B86F4 */
SndBool __stdcall SND_fn_bSetParamTransitionSxd(long voice, struct SoundParam_ *par)
{
    if (g_pfn_SND_SetParamTransitionSxd_s == NULL) DebugBreak();
    return g_pfn_SND_SetParamTransitionSxd_s(voice, par);
}

static t_SND_LongLongBoolFn g_pfn_SND_SetNextTransitionSxd_s; /* 0x004B86F8 */
SndBool __stdcall SND_fn_bSetNextTransitionSxd(long voice, long new_res)
{
    if (g_pfn_SND_SetNextTransitionSxd_s == NULL) DebugBreak();
    return g_pfn_SND_SetNextTransitionSxd_s(voice, new_res);
}

static t_SND_LongLongBoolFn g_pfn_SND_DoTransitionWithFadeSxd_s; /* 0x004B86FC */
SndBool __stdcall SND_fn_bDoTransitionWithFadeSxd(long voice, long new_res)
{
    if (g_pfn_SND_DoTransitionWithFadeSxd_s == NULL) DebugBreak();
    return g_pfn_SND_DoTransitionWithFadeSxd_s(voice, new_res);
}

static t_SND_LongLongRealBoolFn g_pfn_SND_DoTransitionWithFadeSxd2_s; /* 0x004B8700 */
SndBool __stdcall SND_fn_bDoTransitionWithFadeSxd2(long voice, long new_res, SndReal rDuration)
{
    if (g_pfn_SND_DoTransitionWithFadeSxd2_s == NULL) DebugBreak();
    return g_pfn_SND_DoTransitionWithFadeSxd2_s(voice, new_res, rDuration);
}

static t_SND_PerTypeFn g_pfn_SND_StopTransitionSxd_s; /* 0x004B8704 */
void __stdcall SND_fn_vStopTransitionSxd(long voice)
{
    if (g_pfn_SND_StopTransitionSxd_s == NULL) DebugBreak();
    g_pfn_SND_StopTransitionSxd_s(voice);
}

static t_SND_GetBoolFn g_pfn_SND_CanFreeDataSample_s; /* 0x004B8730 */
SndBool __stdcall SND_fn_bCanFreeDataSample(void)
{
    if (g_pfn_SND_CanFreeDataSample_s == NULL) DebugBreak();
    return g_pfn_SND_CanFreeDataSample_s();
}

static t_SND_RollOffFn g_pfn_SND_SetDefaultRollOffSxd_s; /* 0x004B8734 */
void __stdcall SND_fn_vSetDefaultRollOffSxd(RollOffParam *rolloff)
{
    if (g_pfn_SND_SetDefaultRollOffSxd_s == NULL) DebugBreak();
    g_pfn_SND_SetDefaultRollOffSxd_s(rolloff);
}

static t_SND_LongLongBoolFn g_pfn_SND_SetResourceStaticVolumeSxd_s; /* 0x004B8760 */
SndBool __stdcall SND_fn_bSetResourceStaticVolumeSxd(long a, long b)
{
    if (g_pfn_SND_SetResourceStaticVolumeSxd_s == NULL) DebugBreak();
    return g_pfn_SND_SetResourceStaticVolumeSxd_s(a, b);
}

static t_SND_IntPtrFn g_pfn_SND_InitRecord_s;         /* 0x004B8784 */
int __stdcall SND_fn_iInitRecord(void *pInitStruct)
{
    if (g_pfn_SND_InitRecord_s == NULL) DebugBreak();
    return g_pfn_SND_InitRecord_s(pInitStruct);
}

static t_SND_VoidFn g_pfn_SND_DesInitRecord_s;        /* 0x004B8788 */
void __stdcall SND_fn_vDesInitRecord(void)
{
    if (g_pfn_SND_DesInitRecord_s == NULL) DebugBreak();
    g_pfn_SND_DesInitRecord_s();
}

static t_SND_LoadResFn g_pfn_SND_LoadResBinaryMidi_s; /* 0x004B8794 */
SndBool __stdcall SND_fn_bLoadResBinaryMidi(tdstBlockResourceDisk *disk, tdstBlockResourceMem *mem, char *pDataBloc)
{
    if (g_pfn_SND_LoadResBinaryMidi_s == NULL) DebugBreak();
    return g_pfn_SND_LoadResBinaryMidi_s(disk, mem, pDataBloc);
}

static t_SND_IntPtrFn g_pfn_SND_InitMidi_s;           /* 0x004B87A8 */
int __stdcall SND_fn_iInitMidi(void *pInitStruct)
{
    if (g_pfn_SND_InitMidi_s == NULL) DebugBreak();
    return g_pfn_SND_InitMidi_s(pInitStruct);
}

static t_SND_VoidFn g_pfn_SND_DesInitMidi_s;          /* 0x004B87B4 */
void __stdcall SND_fn_vDesInitMidi(void)
{
    if (g_pfn_SND_DesInitMidi_s == NULL) DebugBreak();
    g_pfn_SND_DesInitMidi_s();
}

static t_SND_UnLoadResFn g_pfn_SND_UnLoadResMidi_s;   /* 0x004B87C8 */
void __stdcall SND_fn_vUnLoadResMidi(tdstBlockResourceMem *mem)
{
    if (g_pfn_SND_UnLoadResMidi_s == NULL) DebugBreak();
    g_pfn_SND_UnLoadResMidi_s(mem);
}

static t_SND_PlayTransitionFn g_pfn_SND_PlayTransitionMidi_s; /* 0x004B87E0 */
long __stdcall SND_fn_lPlayTransitionMidi(tduRefRes FirstRes, tduRefRes NextRes, SND_td_pfn_vSoundCallback transition_callback,
                                          long param_callback, struct SoundParam_ *par)
{
    if (g_pfn_SND_PlayTransitionMidi_s == NULL) DebugBreak();
    return g_pfn_SND_PlayTransitionMidi_s(FirstRes, NextRes, transition_callback, param_callback, par);
}

static t_SND_PerTypeSetParamFn g_pfn_SND_SetParamTransitionMidi_s; /* 0x004B87E4 */
SndBool __stdcall SND_fn_bSetParamTransitionMidi(long voice, struct SoundParam_ *par)
{
    if (g_pfn_SND_SetParamTransitionMidi_s == NULL) DebugBreak();
    return g_pfn_SND_SetParamTransitionMidi_s(voice, par);
}

static t_SND_LongLongBoolFn g_pfn_SND_SetNextTransitionMidi_s; /* 0x004B87E8 */
SndBool __stdcall SND_fn_bSetNextTransitionMidi(long voice, long new_res)
{
    if (g_pfn_SND_SetNextTransitionMidi_s == NULL) DebugBreak();
    return g_pfn_SND_SetNextTransitionMidi_s(voice, new_res);
}

static t_SND_PerTypeFn g_pfn_SND_StopTransitionMidi_s; /* 0x004B87EC */
void __stdcall SND_fn_vStopTransitionMidi(long voice)
{
    if (g_pfn_SND_StopTransitionMidi_s == NULL) DebugBreak();
    g_pfn_SND_StopTransitionMidi_s(voice);
}

static t_SND_LongLongBoolFn g_pfn_SND_DoTransitionWithFadeMidi_s; /* 0x004B87F8 */
SndBool __stdcall SND_fn_bDoTransitionWithFadeMidi(long voice, long new_res)
{
    if (g_pfn_SND_DoTransitionWithFadeMidi_s == NULL) DebugBreak();
    return g_pfn_SND_DoTransitionWithFadeMidi_s(voice, new_res);
}

static t_SND_LongLongRealBoolFn g_pfn_SND_DoTransitionWithFadeMidi2_s; /* 0x004B87FC */
SndBool __stdcall SND_fn_bDoTransitionWithFadeMidi2(long voice, long new_res, SndReal rDuration)
{
    if (g_pfn_SND_DoTransitionWithFadeMidi2_s == NULL) DebugBreak();
    return g_pfn_SND_DoTransitionWithFadeMidi2_s(voice, new_res, rDuration);
}

static t_SND_GetBoolFn g_pfn_SND_CanFreeDataMidi_s;   /* 0x004B8804 */
SndBool __stdcall SND_fn_bCanFreeDataMidi(void)
{
    if (g_pfn_SND_CanFreeDataMidi_s == NULL) DebugBreak();
    return g_pfn_SND_CanFreeDataMidi_s();
}

static t_SND_LongLongBoolFn g_pfn_SND_SetResourceStaticVolumeMidi_s; /* 0x004B8808 */
SndBool __stdcall SND_fn_bSetResourceStaticVolumeMidi(long a, long b)
{
    if (g_pfn_SND_SetResourceStaticVolumeMidi_s == NULL) DebugBreak();
    return g_pfn_SND_SetResourceStaticVolumeMidi_s(a, b);
}

static t_SND_LoadResFn g_pfn_SND_LoadResBinaryCd_s;   /* 0x004B8810 */
SndBool __stdcall SND_fn_bLoadResBinaryCd(tdstBlockResourceDisk *disk, tdstBlockResourceMem *mem, char *pDataBloc)
{
    if (g_pfn_SND_LoadResBinaryCd_s == NULL) DebugBreak();
    return g_pfn_SND_LoadResBinaryCd_s(disk, mem, pDataBloc);
}

static t_SND_IntPtrFn g_pfn_SND_InitCD_s;             /* 0x004B8820 */
int __stdcall SND_fn_iInitCD(void *pInitStruct)
{
    if (g_pfn_SND_InitCD_s == NULL) DebugBreak();
    return g_pfn_SND_InitCD_s(pInitStruct);
}

static t_SND_VoidFn g_pfn_SND_DesInitCD_s;            /* 0x004B8830 */
void __stdcall SND_fn_vDesInitCD(void)
{
    if (g_pfn_SND_DesInitCD_s == NULL) DebugBreak();
    g_pfn_SND_DesInitCD_s();
}

static t_SND_LongLongBoolFn g_pfn_SND_SetResourceStaticVolumeCD_s; /* 0x004B8860 */
SndBool __stdcall SND_fn_bSetResourceStaticVolumeCD(long a, long b)
{
    if (g_pfn_SND_SetResourceStaticVolumeCD_s == NULL) DebugBreak();
    return g_pfn_SND_SetResourceStaticVolumeCD_s(a, b);
}

static t_SND_VoidPtrFn g_pfn_SND_LaunchSupervisater_s; /* 0x004B8864 */
void __stdcall SND_fn_vLaunchSupervisater(void *pInitStruct)
{
    if (g_pfn_SND_LaunchSupervisater_s == NULL) DebugBreak();
    g_pfn_SND_LaunchSupervisater_s(pInitStruct);
}

static t_SND_VoidFn g_pfn_SND_KillSupervisater_s;     /* 0x004B8868 */
void __stdcall SND_fn_vKillSupervisater(void)
{
    if (g_pfn_SND_KillSupervisater_s == NULL) DebugBreak();
    g_pfn_SND_KillSupervisater_s();
}

static t_SND_VoidFn g_pfn_SND_SetSupervisaterActive_s; /* 0x004B886C */
void __stdcall SND_fn_vSetSupervisaterActive(void)
{
    if (g_pfn_SND_SetSupervisaterActive_s == NULL) DebugBreak();
    g_pfn_SND_SetSupervisaterActive_s();
}

static t_SND_PerTypeFn g_pfn_SND_KillMicro_s;          /* 0x004B8888 */
void __stdcall SND_fn_vKillMicro(long indice)
{
    if (g_pfn_SND_KillMicro_s == NULL) DebugBreak();
    g_pfn_SND_KillMicro_s(indice);
}

static t_SND_GetBoolFn g_pfn_SND_CanFreeDataADPCM_s;  /* 0x004B88D4 */
SndBool __stdcall SND_fn_bCanFreeDataADPCM(void)
{
    if (g_pfn_SND_CanFreeDataADPCM_s == NULL) DebugBreak();
    return g_pfn_SND_CanFreeDataADPCM_s();
}
#endif

/* 0x004584F0 */
typedef void (__stdcall *t_SND_fn_vFreeSndEx)(unsigned char, void *);
#define SND_RT_fn_vFreeSndEx SND_fn_vFreeSndEx
#define E_ucSndBlockMain      ((unsigned char)0)

/* 0x00457E60 */
long SND_fn_lPlaySnd(tduRefRes res,struct SoundParam_* par,long prio,SND_td_pfn_vSoundCallback fn_callback,long par_callback,SndBool* valid);

/* 0x004B749C */
typedef SndReal (*td_pc_pfn_rRetSoundCoordKey)(long key,long obj);
typedef struct SND_pc_tdstAllRetObjectSoundKey_ SND_pc_tdstAllRetObjectSoundKey;
struct SND_pc_tdstAllRetObjectSoundKey_ {
	td_pc_pfn_rRetSoundCoordKey	X,Y,Z,VX,VY,VZ;
	td_pfn_lRetSoundSwitch		Switch;
	long				bAbsolute;
};

/* 0x00453370 */
typedef struct tdstVolumeLine_ *ptdstVolumeLine;

SNDLST2_M_DynamicUseListOf(ptdstVolumeLine);

typedef struct tdstVolumeLine_{
        long lVol;
        long pc_lUnused;
        SNDLST2_M_DynamicElementDeclaration(ptdstVolumeLine)
}tdstVolumeLine;

typedef struct LISTE_tdstVolumeLine_{
        SNDLST2_M_DynamicAnchorDeclaration(ptdstVolumeLine) pList;
}LISTE_tdstVolumeLine;

static LISTE_tdstVolumeLine g_VolumeLines;   /* 0x004B7480 */
static int nb_volume_lines;   /* 0x004B74AC */

typedef struct tdstVolumeChanged_ *ptdstVolumeChanged;

SNDLST2_M_DynamicUseListOf(ptdstVolumeChanged);

typedef struct tdstVolumeChanged_{
        long lVolume;
		long lIndice;
        SNDLST2_M_DynamicElementDeclaration(ptdstVolumeChanged)
}tdstVolumeChanged;

static int nb_object_type;   /* 0x004B74B0 */

typedef struct {
        tdstAllRetObjectSound       pfnRetObjectSound;
        SND_pc_tdstAllRetObjectSoundKey pc_stRetObjectSoundKey;
        tdstVolumeLine*             pstVolumeLine;
		td_pfn_vCopyString          pfnInfoObject;
        td_pfn_bRetRollOffFactor    pfnRollOff;
        SND_tdstBlockEvent*         pstLastEvent;
        long                        lVolume;
        SNDLST2_M_DynamicAnchorDeclaration(ptdstVolumeChanged)  pListVolumeChanged;
} tdstTypeObjectInfos;

static tdstTypeObjectInfos *g_d_pstTypeObjectSoundInfos;   /* 0x004B749C */

long __cdecl fn_lGetVolumeForType(long type)
{
    return g_d_pstTypeObjectSoundInfos[type].lVolume;
}

long fn_lGetVolumeForObject(long indice,long type)
{
    ptdstVolumeChanged pVol;
    int i;
	SndBool	bFound=FALSE;

	SNDLST2_M_DynamicForEachElementOf(&g_d_pstTypeObjectSoundInfos[type].pListVolumeChanged,pVol,i)
	{
			if (pVol->lIndice==indice)
			{
				bFound=TRUE;
				break;
			}
	}
	if (bFound)
		return pVol->lVolume;
	else
		return C_SOUNDVOL_MAXI;
}

void __cdecl set_SwitchParam(long indice,long type,SoundParam* pstSoundParam)
{
        SwitchParam* pstSwitch;

        pstSwitch=&(SND_M_GetExtraParamFromSoundParam(pstSoundParam))->stSwitch;

        pstSoundParam->iFlags|=C_SOUNDPARAM_SWITCH;
        pstSwitch->lObjectType=type;
        pstSwitch->lObjectId=indice;
}

/* 0x00451FB0 */
long __cdecl SND_pc_fn_lCallTypeSubTableSlot6(long objectid, long lTypeSwitch, long objecttype)
{
    return g_d_pstTypeObjectSoundInfos[objecttype].pc_stRetObjectSoundKey.Switch(objectid, lTypeSwitch);
}

/* 0x004537E0 */
long __cdecl SND_fn_lGetSwitchIndice(long objecttype,long objectid,tduRefRes uRes)
{
        return g_d_pstTypeObjectSoundInfos[objecttype].pfnRetObjectSound.Switch(objectid,uRes.pstPtr->uRes.stSwitch.lTypeSwitch);
}

/* 0x00451FE0 */
SndVector __cdecl SND_pc_fn_stAddAxisCombinationToBase(SndVector stBase, SndVector stCoef,
                                                   SndVector stAxisX, SndVector stAxisY, SndVector stAxisZ)
{
    SndReal rX, rY, rZ;

    rX = SND_fn_rMulRealRealSnd(stCoef.z, stAxisZ.x);
    rX += SND_fn_rMulRealRealSnd(stCoef.y, stAxisY.x);
    rX += SND_fn_rMulRealRealSnd(stCoef.x, stAxisX.x);
    rY = SND_fn_rMulRealRealSnd(stCoef.z, stAxisZ.y);
    rY += SND_fn_rMulRealRealSnd(stCoef.y, stAxisY.y);
    rY += SND_fn_rMulRealRealSnd(stCoef.x, stAxisX.y);
    rZ = SND_fn_rMulRealRealSnd(stCoef.z, stAxisZ.z);
    rZ += SND_fn_rMulRealRealSnd(stCoef.y, stAxisY.z);
    rZ += SND_fn_rMulRealRealSnd(stCoef.x, stAxisX.z);

    stBase.x += rX;
    stBase.y += rY;
    stBase.z += rZ;
    return stBase;
}

void __cdecl get_RollOff(long indice,long type,SoundParam* pstSoundParam)
{
    RollOffParam* pstRollOff;

    pstRollOff=&(SND_M_GetExtraParamFromSoundParam(pstSoundParam))->stRollOff;
    pstSoundParam->iFlags&=~C_SOUNDPARAM_ROLLOFF;

    if (g_d_pstTypeObjectSoundInfos[type].pfnRollOff)
    {
            if ((*g_d_pstTypeObjectSoundInfos[type].pfnRollOff)(indice,pstRollOff))
            {
                    pstSoundParam->iFlags|=C_SOUNDPARAM_ROLLOFF;
            }
    }
}

static tdstAllRetMicroSound g_st_pfnRetMicroSound;   /* 0x004B74B8 */

static long HandleCallback=1;   /* 0x004A24D8 */

static SndBool g_bDopplerActive=TRUE;   /* 0x004A24DC */

/* 0x004519D0 */
void GetMicroParam(long id,MicroParam *par)
{
                par->pos.x=g_st_pfnRetMicroSound.X(id);
                par->pos.y=g_st_pfnRetMicroSound.Y(id);
                par->pos.z=g_st_pfnRetMicroSound.Z(id);
                par->vit.x=g_st_pfnRetMicroSound.VX(id);
                par->vit.y=g_st_pfnRetMicroSound.VY(id);
                par->vit.z=g_st_pfnRetMicroSound.VZ(id);
                par->dirnor.x=g_st_pfnRetMicroSound.DNX(id);
                par->dirnor.y=g_st_pfnRetMicroSound.DNY(id);
                par->dirnor.z=g_st_pfnRetMicroSound.DNZ(id);
                par->dirtan.x=g_st_pfnRetMicroSound.DTX(id);
                par->dirtan.y=g_st_pfnRetMicroSound.DTY(id);
                par->dirtan.z=g_st_pfnRetMicroSound.DTZ(id);

                                if ((par->dirnor.x==0)&&(par->dirnor.y==0)&&(par->dirnor.z==0))
                                {
                                        par->dirnor.x=C_SNDREAL_1;
                                }
                if ((par->dirtan.x==0)&&(par->dirtan.y==0)&&(par->dirtan.z==0))
                                {
                                        par->dirtan.y=C_SNDREAL_1;
                                }
}

#ifndef snd_assert
#define snd_assert(expr) ((void)0)
#endif

typedef struct ASSOCIATION_DESCRIPTION_ *pASSOCIATION_DESCRIPTION;

SNDLST2_M_DynamicUseListOf(pASSOCIATION_DESCRIPTION);

typedef struct ASSOCIATION_DESCRIPTION_{
                long                    pc_lKeyActive;
                long                    pc_lKey;
                long                    indice;
                long                    type;
                SND_tduRefEvt			evt;
                long                    prio;
                long                    voice;
                tduRefRes               res_hi;
				SndBool                 bValid;
                SndReal                 rDeTune;
                long                    lHandle;
				RollOffParam            stRollOff;
                SNDLST2_M_DynamicElementDeclaration(pASSOCIATION_DESCRIPTION)
} ASSOCIATION_DESCRIPTION;

typedef struct LISTE_ASSOCIATION_{
                SNDLST2_M_DynamicAnchorDeclaration(pASSOCIATION_DESCRIPTION) pList;
} LISTE_ASSOCIATION;

#define NB_ASSOCIATION_MAX 65

static LISTE_ASSOCIATION association;   /* 0x004B7470 */

pASSOCIATION_DESCRIPTION __cdecl get_association(long indice,long type,void* evt);
void __cdecl erase_association(pASSOCIATION_DESCRIPTION asso);
ASSOCIATION_DESCRIPTION* __cdecl pre_associe_sound(void);

void __cdecl associe_sound(pASSOCIATION_DESCRIPTION asso, long lPcExtra,
                           long indice, long type, long prio, long voice,
                           SND_tduRefEvt evt, tduRefRes res_hi, SndReal rDeTune, long lHandle);

pASSOCIATION_DESCRIPTION __cdecl get_association(long indice,long type,void* evt)
{
	pASSOCIATION_DESCRIPTION pAsso;
	int i=0;

	if (evt == NULL)
	{
		SNDLST2_M_DynamicForEachElementOf(&association.pList, pAsso, i)
		{
			if (pAsso->bValid
				&& (pAsso->indice == indice)
				&& (pAsso->type == type))
			break;
		}
	}
	else
	{
		SNDLST2_M_DynamicForEachElementOf(&association.pList, pAsso, i)
		{
			if (pAsso->bValid
				&& (pAsso->indice == indice)
				&& (pAsso->evt.pstPtr == evt)
				&& (pAsso->type == type))
			break;
        }
    }

    return pAsso;
}

void __cdecl fnSetLastEventForSoundObjectType(long type,SND_tdstBlockEvent* evt)
{
        if (type<nb_object_type && type>=0)
                g_d_pstTypeObjectSoundInfos[type].pstLastEvent=evt;
}

/* 0x00451790 */
SND_tdstBlockEvent* __cdecl SND_fn_pstGetLastEventForObjectTypeSound(long type)
{
        if (type<nb_object_type && type>=0)
                return g_d_pstTypeObjectSoundInfos[type].pstLastEvent;
        else
                return NULL;
}

void __cdecl fn_vChangeVolumeForType(long type,long lNewVolume)
{
        g_d_pstTypeObjectSoundInfos[type].lVolume=lNewVolume;
}

SndBool SND_fn_bInitDataLoadSnd(const char *_szFileTitle)
{
    return 1;
}

typedef void *(__stdcall *t_SND_fn_pvMallocSndEx)(unsigned char, long);
#define SND_RT_fn_pvMallocSndEx(id, sz) \
    SND_fn_pvMallocSndEx((id), (unsigned int)(sz))

void fn_vResetVolumeForObject(long indice,long type)
{
        ptdstVolumeChanged pVol;
        int i;
		SndBool	bFound=FALSE;

        SNDLST2_M_DynamicForEachElementOf(&g_d_pstTypeObjectSoundInfos[type].pListVolumeChanged,pVol,i)
        {
                if (pVol->lIndice==indice)
				{
					bFound=TRUE;
                    break;
				}
        }

		if (bFound)
		{
			SNDLST2_M_DynamicIsolate(pVol);
			SND_RT_fn_vFreeSndEx(E_ucSndBlockMain,pVol);
		}
}

void fn_vChangeVolumeForObject(long indice,long type,long lNewVolume)
{
        ptdstVolumeChanged pVol;
        int i;
		SndBool bFound=FALSE;

        if (lNewVolume==C_SOUNDVOL_MAXI)
        {
			fn_vResetVolumeForObject(indice,type);
        }
        else
        {

                SNDLST2_M_DynamicForEachElementOf(&g_d_pstTypeObjectSoundInfos[type].pListVolumeChanged,pVol,i)
                {
                        if (pVol->lIndice==indice)
						{
							bFound=TRUE;
                                break;
						}
                }

                if (!bFound)
                {
                        pVol=SND_RT_fn_pvMallocSndEx(E_ucSndBlockMain,sizeof(tdstVolumeChanged));
                        SNDLST2_M_DynamicInitElement(pVol);
                        SNDLST2_M_DynamicAddTail(&g_d_pstTypeObjectSoundInfos[type].pListVolumeChanged,pVol);
                        pVol->lIndice=indice;
                }

                pVol->lVolume=lNewVolume;
        }
}

void __cdecl SND_fn_vSetRollOffFunctionForTypeSound(long type,td_pfn_bRetRollOffFactor fct)
{
	if ((type>=0) && (type<nb_object_type))
		g_d_pstTypeObjectSoundInfos[type].pfnRollOff=fct;
}

void __cdecl SND_fn_vSetInfoForTypeSound(long type,td_pfn_vCopyString fct)
{
	if ((type>=0) && (type<nb_object_type))
		g_d_pstTypeObjectSoundInfos[type].pfnInfoObject=fct;
}

void __cdecl SND_fn_vGetInfoForObjectSound(long id, long type, char *str, long size_ptr)
{
    td_pfn_vCopyString pfnInfoObject = g_d_pstTypeObjectSoundInfos[type].pfnInfoObject;

    if (pfnInfoObject != NULL)
    {
        pfnInfoObject(id, str, size_ptr);
        return;
    }
    {
        /* 0x0049C17C */
        static const char SND_szNoInfo[] = "no info";
        strncpy(str, SND_szNoInfo, (size_t)size_ptr);
    }
}

/* 0x00403290 */
void __cdecl SND_PrintUsedStaticMemory(void)
{
}

static tdstAllRetObjectSound g_astTypeObjectSoundInfosDefault;
#define SND_M_g_astTypeObjectSoundInfosDefault (&g_astTypeObjectSoundInfosDefault)

void __cdecl fn_vUpdateTypeObjectSoundInfos(tdstAllRetObjectSound *ptrfct,
                                            tdstAllRetObjectSound *new_ptrfct)
{
    memcpy(ptrfct, SND_M_g_astTypeObjectSoundInfosDefault,
           sizeof(tdstAllRetObjectSound));

    if (new_ptrfct->X         != NULL) ptrfct->X         = new_ptrfct->X;
    if (new_ptrfct->Y         != NULL) ptrfct->Y         = new_ptrfct->Y;
    if (new_ptrfct->Z         != NULL) ptrfct->Z         = new_ptrfct->Z;
    if (new_ptrfct->VX        != NULL) ptrfct->VX        = new_ptrfct->VX;
    if (new_ptrfct->VY        != NULL) ptrfct->VY        = new_ptrfct->VY;
    if (new_ptrfct->VZ        != NULL) ptrfct->VZ        = new_ptrfct->VZ;
    if (new_ptrfct->REVERB    != NULL) ptrfct->REVERB    = new_ptrfct->REVERB;
    if (new_ptrfct->ExtraCoef != NULL) ptrfct->ExtraCoef = new_ptrfct->ExtraCoef;
    if (new_ptrfct->Switch    != NULL) ptrfct->Switch    = new_ptrfct->Switch;
    if (new_ptrfct->MicroLink != NULL) ptrfct->MicroLink = new_ptrfct->MicroLink;
}

/* 0x00403290 */
void __cdecl check_micro_fct(void)
{
}

void __cdecl SND_fn_vSetRetMicroSound(tdstAllRetMicroSound *ptrfct)
{
    SND_RT_fn_vEnterCriticalSectionThreadSnd();
    SND_RT_fn_vMouchardThreadsnd("..\\..\\..\\src\\sound3d.c", 0x9c1);
    check_micro_fct();

    memcpy(&g_st_pfnRetMicroSound,ptrfct,sizeof(tdstAllRetMicroSound));
    SND_RT_fn_vQuitCriticalSectionThreadSnd();
}

typedef void (__stdcall *t_SND_fn_vSetDopplerFactorSxd)(long);
#if defined(_DLL_COMPILATION_MODE)
static t_SND_fn_vSetDopplerFactorSxd g_pfn_SND_SetDopplerFactorSxd_s;
#define g_pfn_SND_SetDopplerFactorSxd  g_pfn_SND_SetDopplerFactorSxd_s

/* 0x00455110 */
void __stdcall SND_fn_vSetDopplerFactorSxd(long factor)
{
    if (g_pfn_SND_SetDopplerFactorSxd == NULL) DebugBreak();
    g_pfn_SND_SetDopplerFactorSxd(factor);
}
#endif

#define SND_C_NO_DOPPLER (M_IntToRealSnd(0))
typedef void (__cdecl *t_SND_fn_vSetDopplerFactorSnd)(long);

#define SND_RT_fn_vSetDopplerFactorSnd SND_fn_vSetDopplerFactorSnd

void __cdecl SND_fn_vSetDopplerFactorSound(long factor)
{
    g_bDopplerActive = (factor != SND_C_NO_DOPPLER);
    SND_RT_fn_vSetDopplerFactorSnd(factor);
}

/* 0x00455B50 */
typedef void (__stdcall *t_SND_fn_vKillEventToObject)(long indice, void *asso);

#if defined(_DLL_COMPILATION_MODE)
t_SND_fn_vKillEventToObject g_pfn_SND_KillEventToObject;
#define SND_M_g_pfnKillEventToObject  (g_pfn_SND_KillEventToObject)
/* 0x00455B50 */
void __stdcall SND_fn_vKillEventToObject(long indice, void *asso)
{
    if (SND_M_g_pfnKillEventToObject == NULL) DebugBreak();
    SND_M_g_pfnKillEventToObject(indice, asso);
}
#endif

void __cdecl erase_association(pASSOCIATION_DESCRIPTION asso)
{
#if defined(_DLL_COMPILATION_MODE) || defined(SUPERVISATER)
        SND_fn_vKillEventToObject(asso->indice,asso);
#endif

        SNDLST2_M_DynamicIsolate(asso);
        SND_RT_fn_vFreeSndEx(E_ucSndBlockMain,asso);
}

void __cdecl stop_un_son_objet(long indice,long type,SND_tduRefEvt evt)
{
    pASSOCIATION_DESCRIPTION pAsso;

    while ((pAsso = get_association(indice, type, evt.pstPtr)) != NULL)
    {
        if (pAsso->evt.pstPtr->bStopable)
            SND_fn_vStopSnd(pAsso->voice);
        else
            invalidate_ptrvalid_snd(pAsso->voice);
        erase_association(pAsso);
    }
}

void __cdecl stop_all_son_objet(long indice, long type)
{
    pASSOCIATION_DESCRIPTION pAsso;

    while ((pAsso = get_association(indice, type, NULL)) != NULL)
    {
        if (pAsso->evt.pstPtr->bStopable)
            SND_fn_vStopSnd(pAsso->voice);
        else
            invalidate_ptrvalid_snd(pAsso->voice);
        erase_association(pAsso);
    }
}

/* 0x00451EE0 */
void __cdecl stop_son_all(void)
{
	pASSOCIATION_DESCRIPTION pAsso,pAssoNext;
    int i=0;

    SNDLST2_M_DynamicForEachMovingElementOf(&association.pList,pAsso,pAssoNext,i)
    {
		if (pAsso->bValid)
			SND_fn_vStopSnd(pAsso->voice);
        erase_association(pAsso);
    }
}

/* 0x004530C0 */
void __cdecl SND_fn_vKillAllObjectSound(void)
{
    long type;

    SND_RT_fn_vEnterCriticalSectionThreadSnd();
    SND_RT_fn_vMouchardThreadsnd(SND_C_szSound3DSrcFile, 0x940);
    stop_son_all();
    for (type = 0; type < nb_object_type; type++)
        fn_vChangeVolumeForType(type, C_SOUNDVOL_MAXI);
    SND_RT_fn_vQuitCriticalSectionThreadSnd();
}

/* 0x00451E90 */
void __cdecl stop_son_type_all(long type)
{
	pASSOCIATION_DESCRIPTION pAsso,pAssoNext;
    int i=0;

    SNDLST2_M_DynamicForEachMovingElementOf(&association.pList,pAsso,pAssoNext,i)
    {
		if ((pAsso->bValid) && (pAsso->type==type))
        {
			SND_fn_vStopSnd(pAsso->voice);
            erase_association(pAsso);
        }
    }
}

/* 0x00453080 */
void __cdecl SND_fn_vKillAllObjectTypeSound(long type)
{
    SND_RT_fn_vEnterCriticalSectionThreadSnd();
    SND_RT_fn_vMouchardThreadsnd(SND_C_szSound3DSrcFile, 0x92C);
    stop_son_type_all(type);
    fn_vChangeVolumeForType(type, C_SOUNDVOL_MAXI);
    SND_RT_fn_vQuitCriticalSectionThreadSnd();
}

void __cdecl SND_fn_vKillObjectSound2(long indice, long type)
{
    SND_RT_fn_vEnterCriticalSectionThreadSnd();
    SND_RT_fn_vMouchardThreadsnd(SND_C_szSound3DSrcFile, 0x8FE);
    stop_all_son_objet(indice, type);
    fn_vResetVolumeForObject(indice, type);
    SND_RT_fn_vQuitCriticalSectionThreadSnd();
}

void __cdecl SND_fn_vChangeVolumeSound(long indice, long type, long lNewVolume)
{
    SND_RT_fn_vEnterCriticalSectionThreadSnd();
    SND_RT_fn_vMouchardThreadsnd(SND_C_szSound3DSrcFile, 0xCE8);
    fn_vChangeVolumeForObject(indice, type, lNewVolume);
    SND_fn_vSynchroSound();
    SND_RT_fn_vQuitCriticalSectionThreadSnd();
}

static int g_iNbActiveMicros;   /* 0x004B74A8 */

#define SND_C_EVT_CALLBACK_FAILED -1
#define SND_C_EVT_FAILED -2

/* 0x00452780 */
typedef void (__cdecl *t_SND_fn_get_param_objet)(
    long lPcExtra, long indice, long type, void *evt,
    void *pstSoundParam, long rDeTune);

/* 0x00458530 */
tduRefRes SND_fn_tduGetLowestResourceSnd(tduRefRes uHighRes, struct SoundParam_ *par, SndBool eval);
/* 0x00455C20 */
typedef void (__stdcall *t_SND_fn_vAddHistoric)(
    SND_tduRefEvt evt, long indice, long type, void *asso, tdstBlockResourceMem *res);

#if defined(_DLL_COMPILATION_MODE)
t_SND_fn_vAddHistoric g_pfn_SND_AddHistoric;
#define SND_M_g_pfnAddHistoric   (g_pfn_SND_AddHistoric)

/* 0x00455C20 */
void __stdcall SND_fn_vAddHistoric(SND_tduRefEvt evt, long indice, long type, void *asso, tdstBlockResourceMem *res)
{
    if (SND_M_g_pfnAddHistoric == NULL) DebugBreak();
    SND_M_g_pfnAddHistoric(evt, indice, type, asso, res);
}
#endif

#define EVT_FANTOME_PTR NULL

/* 0x00452D50 */
long __cdecl SND_fn_lSendRequestSoundIndexed(
    long lPcExtra, long indice, long type, SND_tduRefEvt evt, long prio, void *proc)
{
	SoundParam		*pstSoundParam;
	SoundParam		stSoundParam;
	tduRefRes		res_hi;
	tduRefRes		res_low;
	long            voice;
	SndBool         use_callback;
	SndBool         evt_ok;
	long            lHandle;
	SndReal         rDeTune;
	pASSOCIATION_DESCRIPTION asso = NULL;

	res_hi.pstPtr = NULL;
	if (evt.pstPtr == EVT_FANTOME_PTR)
		return SND_C_EVT_FAILED;

	SND_RT_fn_vEnterCriticalSectionThreadSnd();
	SND_RT_fn_vMouchardThreadsnd(SND_C_szSound3DSrcFile, 0x7CB);

	pstSoundParam = &stSoundParam;
	stSoundParam.iNbLinks = g_iNbActiveMicros;

	lHandle = HandleCallback++;
	if ((HandleCallback == SND_C_EVT_CALLBACK_FAILED) || (HandleCallback == SND_C_EVT_FAILED))
		HandleCallback = 1;

	voice = -1;
	use_callback = FALSE;
	evt_ok = TRUE;

	switch (evt.pstPtr->eType)
	{
    case EVT_SON_PLAY:
    case EVT_SON_PITCH:
    case EVT_SON_VOLUME:
    case EVT_SON_PAN:
    case EVT_SON_EXTRA:
		if (!(asso = pre_associe_sound()))
		{
	        evt_ok = FALSE;
	        break;
		}
		if (evt.pstPtr->eType == EVT_SON_PLAY)
	        res_hi.pstPtr = evt.pstPtr->uParams.stPlay.uResource.pstPtr;
		else if (evt.pstPtr->eType == EVT_SON_EXTRA)
	        res_hi.pstPtr = evt.pstPtr->uParams.stExtraAll.uResource.pstPtr;
		else
	        res_hi.pstPtr = evt.pstPtr->uParams.stExtraParam.uResource.pstPtr;

		if (res_hi.pstPtr != SND_C_RES_FANTOME_PTR)
		{

			rDeTune = fn_rGetDeTune(evt);

			get_param_objet(lPcExtra, indice, type, evt, pstSoundParam, rDeTune);

            if ((voice=SND_fn_lPlaySnd(res_hi,pstSoundParam,prio,proc,lHandle,&(asso->bValid)))!=C_PLAY_FAILED)
            {
				associe_sound(asso,lPcExtra,indice,type,prio,voice,evt,res_hi,rDeTune,lHandle);
				if (proc) use_callback=TRUE;
			}
			else
			{
				erase_association(asso);
				evt_ok=FALSE;
			}
		}
		else
		{
			erase_association(asso);
			evt_ok=FALSE;
		}
		break;
	case EVT_SON_STOP:

		stop_un_son_objet(indice,type,evt.pstPtr->uParams.stStop.uEvt);
		break;
	case EVT_SON_STOP_ALL:

		stop_all_son_objet(indice,type);
		break;
	case EVT_SON_STOP_N_GO:

		stop_un_son_objet(indice,type,evt.pstPtr->uParams.stStopNGo.uEvtStop);
		if ((lHandle=SND_fn_lSendRequestSoundIndexed(lPcExtra,indice,type,evt.pstPtr->uParams.stStopNGo.uEvtGo,prio,proc))==SND_C_EVT_FAILED)
		        evt_ok=FALSE;
		use_callback=TRUE;
		break;
	case EVT_SON_EFFET:

		SND_fn_vSetEffectSnd(evt.pstPtr->uParams.stEffect.iIdEffect);
		break;
	case EVT_SON_CHANGE_VOLUME:
		if (evt.pstPtr->uParams.stChangeVolume.bConcernWithObjectType)
		{
			fn_vChangeVolumeForType(type,evt.pstPtr->uParams.stChangeVolume.lNewVolume);
			SND_fn_vSynchroSound();
		}
		else
		{
			fn_vChangeVolumeForObject(indice,type,evt.pstPtr->uParams.stChangeVolume.lNewVolume);
			SND_fn_vSynchroSound();
		}
		break;
	default:
		SND_RT_fn_vDisplayError(E_uwSndInvalidTypeOfEvent,SND_C_szEmpty);
		evt_ok=FALSE;
		break;
	}

	if (res_hi.pstPtr!=NULL)
		res_low=SND_fn_tduGetLowestResourceSnd(res_hi,pstSoundParam,FALSE);
	else
		res_low.pstPtr=NULL;
#if defined(_DLL_COMPILATION_MODE) || defined(SUPERVISATER)
	SND_fn_vAddHistoric(evt,indice,type,asso,res_low.pstPtr);
#endif

	fnSetLastEventForSoundObjectType(type,evt.pstPtr);
	SND_RT_fn_vQuitCriticalSectionThreadSnd();

	if (!evt_ok)
		return SND_C_EVT_FAILED;
	if (proc && !use_callback)
		return SND_C_EVT_CALLBACK_FAILED;
	else
		return lHandle;
}

/* 0x00452D20 */
long __cdecl SND_fn_lSendRequestSound(long indice, long type, void *evt,
                                      long prio, void *proc)
{

    return SND_fn_lSendRequestSoundIndexed(-1, indice, type, *(SND_tduRefEvt *)&evt, prio, proc);
}

/* 0x00451AB0 */
ASSOCIATION_DESCRIPTION* __cdecl pre_associe_sound(void)
{
        pASSOCIATION_DESCRIPTION pAsso;
        int i;

        i=SNDLST2_M_DynamicGetNumberOfElements(&association.pList);
        if (i>=NB_ASSOCIATION_MAX)
        {
                SND_fn_vDisplayErrorEx(E_uwSndTooManyEvents,(char *)SND_C_szEmpty,SNDERR_DISPLAY_IF_WANTED);
                return NULL;
        }

        pAsso=(pASSOCIATION_DESCRIPTION)SND_RT_fn_pvMallocSndEx(E_ucSndBlockMain,sizeof(ASSOCIATION_DESCRIPTION));
        SNDLST2_M_DynamicInitElement(pAsso);
        return pAsso;
}

/* 0x00458830 */
long __cdecl SND_fn_rGetRandomSnd(long max);
#define SND_RT_fn_rGetRandomSnd SND_fn_rGetRandomSnd

#define SND_C_NO_DETUNING  0
#define C_SNDREAL_1    0x10000

/* 0x004527D0 */
SndReal __cdecl fn_rGetDeTune(SND_tduRefEvt evt)
{
        SndReal delta;

        switch (evt.pstPtr->eType)
        {
                case EVT_SON_PLAY:
                        delta=evt.pstPtr->uParams.stPlay.stDeTune.rDelta;
                        break;
                case EVT_SON_PITCH:
                case EVT_SON_VOLUME:
                case EVT_SON_PAN:
                        delta=evt.pstPtr->uParams.stExtraParam.stDeTune.rDelta;
                        break;
                default:
                        delta=NO_DETUNING;
                        break;
        }

        if (delta!=NO_DETUNING)
                return (C_SNDREAL_1+SND_RT_fn_rGetRandomSnd(delta*2)-delta);
        else
                return C_SNDREAL_1;
}

/* 0x00455B10 */
typedef void (__stdcall *t_SND_fn_vAddEventToObject)(long, long, void *, SND_tduRefEvt);

#if defined(_DLL_COMPILATION_MODE)
t_SND_fn_vAddEventToObject g_pfn_SND_AddEventToObject;
#define SND_M_g_pfnAddEventToObject  (g_pfn_SND_AddEventToObject)
/* 0x00455B10 */
void __stdcall SND_fn_vAddEventToObject(long indice, long type, void *asso, SND_tduRefEvt evt)
{
    if (SND_M_g_pfnAddEventToObject == NULL) DebugBreak();
    SND_M_g_pfnAddEventToObject(indice, type, asso, evt);
}
#endif

void __cdecl associe_sound(pASSOCIATION_DESCRIPTION asso, long lPcExtra,
                           long indice, long type, long prio, long voice,
                           SND_tduRefEvt evt, tduRefRes res_hi, SndReal rDeTune, long lHandle)
{
        int i;
        pASSOCIATION_DESCRIPTION pAsso,pAssoNext;

        SNDLST2_M_DynamicForEachMovingElementOf(&association.pList,pAsso,pAssoNext,i)
        {
                if (!(pAsso->bValid) || (pAsso->voice==voice))
                {
                        erase_association(pAsso);
                }
        }

		SNDLST2_M_DynamicForEachElementOf(&association.pList,pAsso,i)
		{
			if ((pAsso->indice==indice) && (pAsso->type==type))
				break;
		}
		if (pAsso)
		{
			pAssoNext=SNDLST2_M_DynamicGetNextElement(pAsso);
			SNDLST2_M_DynamicInsertBetween(asso,pAsso,pAssoNext);
		}
		else
		{
			SNDLST2_M_DynamicAddTail(&association.pList,asso);
		}

        asso->pc_lKeyActive=TRUE;
        asso->pc_lKey=lPcExtra;
        asso->indice=indice;
        asso->type=type;
        asso->prio=prio;
        asso->voice=voice;
        asso->evt=evt;
        asso->res_hi=res_hi;
        asso->bValid=TRUE;
        asso->rDeTune=rDeTune;
        asso->lHandle=lHandle;

#if defined(_DLL_COMPILATION_MODE) || defined(SUPERVISATER)
        SND_fn_vAddEventToObject(indice,type,asso,evt);
#endif
}

/* 0x00455150 */
typedef void (__stdcall *t_SND_fn_vSetMicroParamSxd)(long sndId, MicroParam *par);
#if defined(_DLL_COMPILATION_MODE)
t_SND_fn_vSetMicroParamSxd g_pfn_SND_SetMicroParamSxd;
#define SND_M_g_pfnSetMicroParamSxd  (g_pfn_SND_SetMicroParamSxd)
void __stdcall SND_fn_vSetMicroParamSxd(long sndId, MicroParam *par)
{
    if (SND_M_g_pfnSetMicroParamSxd == NULL) DebugBreak();
    SND_M_g_pfnSetMicroParamSxd(sndId, par);
}
#endif

/* 0x00455BF0 */
typedef void (__stdcall *t_SND_fn_vSetPosMicro)(void *pMicro, MicroParam *par);
#if defined(_DLL_COMPILATION_MODE)
t_SND_fn_vSetPosMicro g_pfn_SND_SetPosMicro;
#define SND_M_g_pfnSetPosMicro  (g_pfn_SND_SetPosMicro)
void __stdcall SND_fn_vSetPosMicro(void *pMicro, MicroParam *par)
{
    if (SND_M_g_pfnSetPosMicro == NULL) DebugBreak();
    SND_M_g_pfnSetPosMicro(pMicro, par);
}
#endif
/* 0x00452510 */
typedef void (__cdecl   *t_SND_fn_get_param_objet2)(long indice, long type,
                                                    void *evt, void *par,
                                                    long rDeTune, void *prefill);
#define SND_RT_fn_get_param_objet2 get_param_objet2

typedef void (__stdcall *t_SND_SynchroFn)(void);
#if defined(_DLL_COMPILATION_MODE)
static t_SND_SynchroFn g_pfn_SND_SynchroSxd_s;
#define g_pfn_SND_SynchroSxd  g_pfn_SND_SynchroSxd_s

void __stdcall SND_fn_vSynchroSxd(void)
{
    if (g_pfn_SND_SynchroSxd == NULL) DebugBreak();
    g_pfn_SND_SynchroSxd();
}

static t_SND_SynchroFn g_pfn_SND_SynchroMidi_s;
#define g_pfn_SND_SynchroMidi  g_pfn_SND_SynchroMidi_s

void __stdcall SND_fn_vSynchroMidi(void)
{
    if (g_pfn_SND_SynchroMidi == NULL) DebugBreak();
    g_pfn_SND_SynchroMidi();
}

/* 0x00455A60 */
static t_SND_SynchroFn g_pfn_SND_SynchroCD_s;
#define g_pfn_SND_SynchroCD  g_pfn_SND_SynchroCD_s

void __stdcall SND_fn_vSynchroCD(void)
{
    if (g_pfn_SND_SynchroCD == NULL) DebugBreak();
    g_pfn_SND_SynchroCD();
}
#endif

#define SND_RT_fn_vSynchroSnd SND_fn_vSynchroSnd

/* 0x4B74A8 */
void get_param_objet2(long indice,long type,SND_tduRefEvt evt,SoundParam* param,SndReal deTune,SoundParam* pstPreFilledParam)
{
        SndReal                 extra_par;
        long                    extra_res;
		SndReal                 rCoefA,rCoefB;
		long					lVolumeType,lVolumeObject;

		SND_fn_vCopySoundParam(param,pstPreFilledParam);

        if (g_d_pstTypeObjectSoundInfos[type].pstVolumeLine!=NULL)
        {
                param->ucVol=(unsigned char)g_d_pstTypeObjectSoundInfos[type].pstVolumeLine->lVol;
                param->iFlags|=C_SOUNDPARAM_COEF_VOL;
        }
		else
			param->ucVol=C_SOUNDVOL_MAXI;

		lVolumeType=fn_lGetVolumeForType(type);
		lVolumeObject=fn_lGetVolumeForObject(indice,type);
		if ((lVolumeType!=C_SOUNDVOL_MAXI) || (lVolumeObject!=C_SOUNDVOL_MAXI))
        {
			param->ucVol=(unsigned char)((param->ucVol*lVolumeType*lVolumeObject)>>14);
			param->iFlags|=C_SOUNDPARAM_COEF_VOL;
        }

		param->Freq=deTune;

        if ((evt.pstPtr->eType==EVT_SON_PITCH)
                || ((evt.pstPtr->eType==EVT_SON_EXTRA) && (evt.pstPtr->uParams.stExtraAll.rtCoefPitch)))
        {
            if (evt.pstPtr->eType==EVT_SON_PITCH)
            {
                    rCoefA=evt.pstPtr->uParams.stExtraParam.CoefA;
                    rCoefB=evt.pstPtr->uParams.stExtraParam.CoefB;
            }
            else
                    SND_fn_vSndRealTwinToSndReal(evt.pstPtr->uParams.stExtraAll.rtCoefPitch,&rCoefA,&rCoefB);

            (g_d_pstTypeObjectSoundInfos[type].pfnRetObjectSound.ExtraCoef)(indice,evt,&extra_par,NULL,NULL);
			extra_par=SND_fn_rMulRealRealSnd(extra_par,rCoefA)+rCoefB;
			param->Freq=SND_fn_rMulRealRealSnd(extra_par,param->Freq);
        }

        if ((evt.pstPtr->eType==EVT_SON_VOLUME)
                || ((evt.pstPtr->eType==EVT_SON_EXTRA) && (evt.pstPtr->uParams.stExtraAll.rtCoefVol)))
        {
            if (evt.pstPtr->eType==EVT_SON_VOLUME)
            {
                    rCoefA=evt.pstPtr->uParams.stExtraParam.CoefA;
                    rCoefB=evt.pstPtr->uParams.stExtraParam.CoefB;
            }
            else
                    SND_fn_vSndRealTwinToSndReal(evt.pstPtr->uParams.stExtraAll.rtCoefVol,&rCoefA,&rCoefB);

            (g_d_pstTypeObjectSoundInfos[type].pfnRetObjectSound.ExtraCoef)(indice,evt,NULL,NULL,&extra_par);
            extra_res=M_RealToIntSnd(SND_fn_rMulRealRealSnd(extra_par,rCoefA)+rCoefB);
            if (extra_res<0)
				param->ucVol=0;
            else
            {
                if (extra_res<=127)
                {
                    if (param->iFlags & C_SOUNDPARAM_COEF_VOL)
                        param->ucVol=(unsigned char)((extra_res*param->ucVol)>>7);
                    else
                        param->ucVol=(unsigned char)extra_res;
                }
                else
                {
                    if (!(param->iFlags & C_SOUNDPARAM_COEF_VOL))
                        param->ucVol=C_SOUNDVOL_MAXI;
                }
            }
            param->iFlags|=C_SOUNDPARAM_COEF_VOL;
        }

        if ((evt.pstPtr->eType==EVT_SON_PAN)
                || ((evt.pstPtr->eType==EVT_SON_EXTRA) && (evt.pstPtr->uParams.stExtraAll.rtCoefPan)))
        {
            if (evt.pstPtr->eType==EVT_SON_PAN)
            {
                    rCoefA=evt.pstPtr->uParams.stExtraParam.CoefA;
                    rCoefB=evt.pstPtr->uParams.stExtraParam.CoefB;
            }
            else
                    SND_fn_vSndRealTwinToSndReal(evt.pstPtr->uParams.stExtraAll.rtCoefPan,&rCoefA,&rCoefB);

            (g_d_pstTypeObjectSoundInfos[type].pfnRetObjectSound.ExtraCoef)(indice,evt,NULL,&extra_par,NULL);
            extra_res=M_RealToIntSnd(SND_fn_rMulRealRealSnd(extra_par,rCoefA)+rCoefB);
            if (extra_res<0)
				param->ucPan=0;
            else
            {
                    if (extra_res>127) param->ucPan=127;
                    else param->ucPan=(unsigned char)extra_res;
            }
            param->iFlags|=C_SOUNDPARAM_NO_POSITION;
        }
}

typedef void (__stdcall *t_SND_cbMicroPost)(long indice, void *param);  /* 0x455B80 */

typedef struct MICRO_DESCRIPTION_ *pMICRO_DESCRIPTION;

SNDLST2_M_DynamicUseListOf(pMICRO_DESCRIPTION);

typedef struct MICRO_DESCRIPTION_{
        long    sndId;
        SNDLST2_M_DynamicElementDeclaration(pMICRO_DESCRIPTION)
}MICRO_DESCRIPTION;

typedef struct LISTE_MICRO_{
        SNDLST2_M_DynamicAnchorDeclaration(pMICRO_DESCRIPTION) pList;
}LISTE_MICRO;

static LISTE_MICRO micros_sound;   /* 0x004B7490 */

#if defined(_DLL_COMPILATION_MODE)
t_SND_cbMicroPost g_pfn_SND_SetPosObject;
#define SND_M_g_pfnSetPosObject  (g_pfn_SND_SetPosObject)
void __stdcall SND_fn_vSetPosObject(long indice, void *param)
{
    if (SND_M_g_pfnSetPosObject == NULL) DebugBreak();
    SND_M_g_pfnSetPosObject(indice, param);
}
#endif

void __cdecl fill_pre_filled_param(long key,long indice,long type,SoundParam* pstPreFilledParam)
{
        int                     i;
        pMICRO_DESCRIPTION      pMicro;
        SndVector               stPosCoef, stObjPos, stAxisX, stAxisY, stAxisZ, stVit, stPos, stVitCoef;

		pstPreFilledParam->iFlags=0;

        if (key==-1)
        {

            if (indice!=SND_C_OBJET_FANTOME)
            {
                pstPreFilledParam->Pos.x=(g_d_pstTypeObjectSoundInfos[type].pfnRetObjectSound.X)(indice);
                pstPreFilledParam->Pos.y=(g_d_pstTypeObjectSoundInfos[type].pfnRetObjectSound.Y)(indice);
                pstPreFilledParam->Pos.z=(g_d_pstTypeObjectSoundInfos[type].pfnRetObjectSound.Z)(indice);
                pstPreFilledParam->Vit.x=(g_d_pstTypeObjectSoundInfos[type].pfnRetObjectSound.VX)(indice);
                pstPreFilledParam->Vit.y=(g_d_pstTypeObjectSoundInfos[type].pfnRetObjectSound.VY)(indice);
                pstPreFilledParam->Vit.z=(g_d_pstTypeObjectSoundInfos[type].pfnRetObjectSound.VZ)(indice);
                pstPreFilledParam->ucReverb=(g_d_pstTypeObjectSoundInfos[type].pfnRetObjectSound.REVERB)(indice);
                pstPreFilledParam->ucPan=C_SOUNDPAN_MEDIAN;
                pstPreFilledParam->ucSpace=C_SOUNDSPACE_FRONT;

				if (!g_bDopplerActive)
                    pstPreFilledParam->iFlags|=C_SOUNDPARAM_NO_DOPPLER;
            }
            else
            {
                pstPreFilledParam->ucPan=C_SOUNDPAN_MEDIAN;
                pstPreFilledParam->ucReverb=NO_REVERB;
                pstPreFilledParam->ucSpace=C_SOUNDSPACE_FRONT;
                pstPreFilledParam->iFlags|=(C_SOUNDPARAM_NO_POSITION | C_SOUNDPARAM_NO_DOPPLER);
            }
        }
        else
        {
            if (g_d_pstTypeObjectSoundInfos[type].pc_stRetObjectSoundKey.bAbsolute)
            {
                pstPreFilledParam->Pos.x=(g_d_pstTypeObjectSoundInfos[type].pc_stRetObjectSoundKey.X)(key,indice);
                pstPreFilledParam->Pos.y=(g_d_pstTypeObjectSoundInfos[type].pc_stRetObjectSoundKey.Y)(key,indice);
                pstPreFilledParam->Pos.z=(g_d_pstTypeObjectSoundInfos[type].pc_stRetObjectSoundKey.Z)(key,indice);
                pstPreFilledParam->Vit.x=(g_d_pstTypeObjectSoundInfos[type].pc_stRetObjectSoundKey.VX)(key,indice);
                pstPreFilledParam->Vit.y=(g_d_pstTypeObjectSoundInfos[type].pc_stRetObjectSoundKey.VY)(key,indice);
                pstPreFilledParam->Vit.z=(g_d_pstTypeObjectSoundInfos[type].pc_stRetObjectSoundKey.VZ)(key,indice);
                pstPreFilledParam->ucReverb=(g_d_pstTypeObjectSoundInfos[type].pfnRetObjectSound.REVERB)(indice);
            }
            else
            {
                (g_d_pstTypeObjectSoundInfos[type].pfnRetObjectSound.pc_AxisX)(indice,&stAxisX);
                (g_d_pstTypeObjectSoundInfos[type].pfnRetObjectSound.pc_AxisY)(indice,&stAxisY);
                (g_d_pstTypeObjectSoundInfos[type].pfnRetObjectSound.pc_AxisZ)(indice,&stAxisZ);
                stPosCoef.x=(g_d_pstTypeObjectSoundInfos[type].pc_stRetObjectSoundKey.X)(key,indice);
                stPosCoef.y=(g_d_pstTypeObjectSoundInfos[type].pc_stRetObjectSoundKey.Y)(key,indice);
                stPosCoef.z=(g_d_pstTypeObjectSoundInfos[type].pc_stRetObjectSoundKey.Z)(key,indice);
                stObjPos.x=(g_d_pstTypeObjectSoundInfos[type].pfnRetObjectSound.X)(indice);
                stObjPos.y=(g_d_pstTypeObjectSoundInfos[type].pfnRetObjectSound.Y)(indice);
                stObjPos.z=(g_d_pstTypeObjectSoundInfos[type].pfnRetObjectSound.Z)(indice);
                stPos=SND_pc_fn_stAddAxisCombinationToBase(stObjPos,stPosCoef,stAxisX,stAxisY,stAxisZ);

                stVitCoef.x=(g_d_pstTypeObjectSoundInfos[type].pc_stRetObjectSoundKey.VX)(key,indice);
                stVitCoef.y=(g_d_pstTypeObjectSoundInfos[type].pc_stRetObjectSoundKey.VY)(key,indice);
                stVitCoef.z=(g_d_pstTypeObjectSoundInfos[type].pc_stRetObjectSoundKey.VZ)(key,indice);
                stVit.x=(g_d_pstTypeObjectSoundInfos[type].pc_stRetObjectSoundKey.VX)(key,indice);
                stVit.y=(g_d_pstTypeObjectSoundInfos[type].pc_stRetObjectSoundKey.VY)(key,indice);
                stVit.z=(g_d_pstTypeObjectSoundInfos[type].pc_stRetObjectSoundKey.VZ)(key,indice);
                stVit=SND_pc_fn_stAddAxisCombinationToBase(stVit,stVitCoef,stAxisX,stAxisY,stAxisZ);

                pstPreFilledParam->Pos.x=stPos.x;
                pstPreFilledParam->Pos.y=stPos.y;
                pstPreFilledParam->Pos.z=stPos.z;
                pstPreFilledParam->Vit.x=stVit.x;
                pstPreFilledParam->Vit.y=stVit.y;
                pstPreFilledParam->Vit.z=stVit.z;
                pstPreFilledParam->ucReverb=(g_d_pstTypeObjectSoundInfos[type].pfnRetObjectSound.REVERB)(indice);
            }
            pstPreFilledParam->ucPan=C_SOUNDPAN_MEDIAN;
            pstPreFilledParam->ucSpace=C_SOUNDSPACE_FRONT;
        }

        SNDLST2_M_DynamicForEachElementOf(&micros_sound.pList,pMicro,i)
        {
                if (indice!=SND_C_OBJET_FANTOME)
                        pstPreFilledParam->astMicro[i].bLinked=(g_d_pstTypeObjectSoundInfos[type].pfnRetObjectSound.MicroLink)(indice,(long)pMicro);
                else
                        pstPreFilledParam->astMicro[i].bLinked=TRUE;
                pstPreFilledParam->astMicro[i].lId=pMicro->sndId;
        }

        get_RollOff(indice,type,pstPreFilledParam);

        set_SwitchParam(indice,type,pstPreFilledParam);

#if defined(_DLL_COMPILATION_MODE) || defined(SUPERVISATER)
        SND_fn_vSetPosObject(indice,pstPreFilledParam);
#endif
}

void __cdecl get_param_objet(long key,long indice,long type,SND_tduRefEvt evt,SoundParam* param,SndReal deTune)
{
    SoundParam *pstPreFilledParam;
    SoundParam stSoundParam;

    pstPreFilledParam=&stSoundParam;
    stSoundParam.iNbLinks=g_iNbActiveMicros;

    fill_pre_filled_param(key,indice,type,pstPreFilledParam);

    get_param_objet2(indice,type,evt,param,deTune,pstPreFilledParam);
}

void __cdecl SND_fn_vSynchroSound(void)
{
        int i;
        MicroParam micpar;
        SoundParam *pstSoundParam,*pstPreFilledParam=NULL;
        pASSOCIATION_DESCRIPTION                pAsso,pAssoNext;
        pMICRO_DESCRIPTION               pMicro;
		long	prev_key=-1,prev_indice=-1,prev_type=-1;
		SoundParam stSoundParam,stPreFilledParam;

        SND_RT_fn_vEnterCriticalSectionThreadSnd();
        SND_RT_fn_vMouchardThreadsnd(SND_C_szSound3DSrcFile, 0x714);

        SNDLST2_M_DynamicForEachElementOf(&micros_sound.pList,pMicro,i)
        {
                GetMicroParam((long)pMicro,&micpar);

                SND_fn_vSetMicroParamSnd(pMicro->sndId,&micpar);

#if defined(_DLL_COMPILATION_MODE) || defined(SUPERVISATER)
                SND_fn_vSetPosMicro(pMicro,&micpar);
#endif
        }

		pstSoundParam=&stSoundParam;
		stSoundParam.iNbLinks=g_iNbActiveMicros;

        SNDLST2_M_DynamicForEachMovingElementOf(&association.pList,pAsso,pAssoNext,i)
        {
            if (pAsso->bValid)
            {

                if (pAsso->evt.pstPtr->bDynamic && pAsso->pc_lKeyActive)
                    pAsso->pc_lKeyActive=SND_pc_fn_lCallTypeSubTableSlot6(pAsso->pc_lKey,pAsso->indice,pAsso->type);
                if (pAsso->pc_lKeyActive)
                {
					if (!pstPreFilledParam)
					{
						pstPreFilledParam=&stPreFilledParam;
						stPreFilledParam.iNbLinks=g_iNbActiveMicros;
						prev_key=pAsso->pc_lKey;prev_indice=pAsso->indice;prev_type=pAsso->type;
						fill_pre_filled_param(prev_key,prev_indice,prev_type,pstPreFilledParam);
					}
					else
					{
						if (pAsso->pc_lKey!=prev_key || pAsso->indice!=prev_indice || pAsso->type!=prev_type)
						{
							prev_key=pAsso->pc_lKey;prev_indice=pAsso->indice;prev_type=pAsso->type;
							fill_pre_filled_param(prev_key,prev_indice,prev_type,pstPreFilledParam);
						}

					}

                    get_param_objet2(pAsso->indice,pAsso->type,pAsso->evt,pstSoundParam,pAsso->rDeTune,pstPreFilledParam);

					if (!SND_fn_bSetParamSnd(pAsso->voice,pstSoundParam))
                    {
						erase_association(pAsso);
                    }
                }
                else
                        if (!SND_fn_bTestIsPlayingSnd(pAsso->voice))
                                erase_association(pAsso);
            }
            else
            {
                    erase_association(pAsso);
            }
        }

		SND_RT_fn_vSynchroSnd();
        SND_RT_fn_vQuitCriticalSectionThreadSnd();
}

void __cdecl fn_vRajAllVolumeForLine(ptdstVolumeLine pVolumeLine)
{
    SoundParam stSoundParam;
    pASSOCIATION_DESCRIPTION pAsso, pAssoNext;

    SND_RT_fn_vEnterCriticalSectionThreadSnd();
    SND_RT_fn_vMouchardThreadsnd(SND_C_szSound3DSrcFile, 0x6A1);

    stSoundParam.iNbLinks = g_iNbActiveMicros;

    pAsso = SNDLST2_M_DynamicGetFirstElement(&association.pList);
    pAssoNext = (pAsso != NULL) ? SNDLST2_M_DynamicGetNextElement(pAsso) : NULL;
    while (pAsso != NULL)
    {
        if (pAsso->bValid == 0)
        {
            erase_association(pAsso);
        }
        else if (g_d_pstTypeObjectSoundInfos[pAsso->type].pstVolumeLine == pVolumeLine)
        {

            get_param_objet(pAsso->pc_lKey, pAsso->indice,
                            pAsso->type, pAsso->evt,
                            &stSoundParam, pAsso->rDeTune);
            if (!SND_fn_bSetParamSnd(pAsso->voice, &stSoundParam))
            {
                erase_association(pAsso);
            }
        }

        pAsso = pAssoNext;
        pAssoNext = (pAssoNext != NULL) ? SNDLST2_M_DynamicGetNextElement(pAssoNext) : NULL;
    }

    SND_RT_fn_vQuitCriticalSectionThreadSnd();
}

/* 0x00455CB0 */
typedef void (__stdcall *t_SND_fn_vSetVolumeLine)(void *pLine, long lNewVolume);

#if defined(_DLL_COMPILATION_MODE)
t_SND_fn_vSetVolumeLine g_pfn_SND_SetVolumeLine;
#define SND_M_g_pfnSetVolumeLine  (g_pfn_SND_SetVolumeLine)
void __stdcall SND_fn_vSetVolumeLine(void *pLine, long lNewVolume)
{
    if (SND_M_g_pfnSetVolumeLine == NULL) DebugBreak();
    SND_M_g_pfnSetVolumeLine(pLine, lNewVolume);
}
#endif

void __cdecl SND_fn_vSetVolumeLineSound(void *lLine, long lNewVolume)
{
	ptdstVolumeLine pLine;

	if (lLine != NULL)
	{
		pLine = (ptdstVolumeLine)lLine;

		{
			pLine->lVol = lNewVolume;
			fn_vRajAllVolumeForLine(pLine);

#if defined(_DLL_COMPILATION_MODE) || defined(SUPERVISATER)
			SND_fn_vSetVolumeLine(pLine, lNewVolume);
#endif
		}
	}
}

/* 0x00455C90 */
typedef void (__stdcall *t_SND_fn_vAddVolumeLine)(void *pLine);

#if defined(_DLL_COMPILATION_MODE)
t_SND_fn_vAddVolumeLine g_pfn_SND_AddVolumeLine;
#define SND_M_g_pfnAddVolumeLine  (g_pfn_SND_AddVolumeLine)
void __stdcall SND_fn_vAddVolumeLine(void *pLine)
{
    if (SND_M_g_pfnAddVolumeLine == NULL) DebugBreak();
    SND_M_g_pfnAddVolumeLine(pLine);
}
#endif

/* 0x00453370 */
void *__cdecl SND_fn_lAddVolumeLineSound(void)
{
        ptdstVolumeLine pLine;

        SND_RT_fn_vEnterCriticalSectionThreadSnd();
        SND_RT_fn_vMouchardThreadsnd(SND_C_szSound3DSrcFile, 0x9D3);

        if (!nb_volume_lines)
                SNDLST2_M_DynamicInitAnchor(&(g_VolumeLines.pList));

        pLine=SND_RT_fn_pvMallocSndEx(E_ucSndBlockMain,sizeof(tdstVolumeLine));
        memset(pLine,0,sizeof(tdstVolumeLine));
        SNDLST2_M_DynamicInitElement(pLine);
        SNDLST2_M_DynamicAddTail(&(g_VolumeLines.pList),pLine);
        pLine->lVol=C_SOUNDVOL_MAXI;

        nb_volume_lines++;

        SND_RT_fn_vQuitCriticalSectionThreadSnd();

#if defined(_DLL_COMPILATION_MODE) || defined(SUPERVISATER)
        SND_fn_vAddVolumeLine(pLine);
#endif

        return pLine;
}

long __cdecl SND_fn_lGetVolumeLineSound(void *lLine)
{
	ptdstVolumeLine pLine;

	if (lLine!=NULL)
	{
		pLine=(ptdstVolumeLine)lLine;
		return pLine->lVol;
	}
	else
		return C_SOUNDVOL_MAXI;
}

/* 0x004977A0 */
void *__stdcall SND_fn_pvReAllocSndEx(unsigned char id_block, void *pvPtr,
                                      unsigned int ulNbBytes);

/* 0x004534A0 */
void __cdecl SND_pc_fn_vCopyTypeSubTableDefault(long lTypeId);

#define SND_C_RES_FANTOME_PTR ((void *)0)
/* 0x004A24B8 */
static SND_pc_tdstAllRetObjectSoundKey SND_pc_g_stRetObjectSoundKeyDefault;

long __cdecl SND_fn_lAddObjectTypeSound2(void *ptrfct, void *lLine)
{
        long id_type;

    SND_RT_fn_vEnterCriticalSectionThreadSnd();
    SND_RT_fn_vMouchardThreadsnd(SND_C_szSound3DSrcFile, 0xA5D);

        if (nb_object_type==0)
        {
                id_type=0;
                g_d_pstTypeObjectSoundInfos=SND_RT_fn_pvMallocSndEx(E_ucSndBlockMain,sizeof(tdstTypeObjectInfos));
				nb_object_type=0;
		}
        else
        {
                id_type=nb_object_type;
                g_d_pstTypeObjectSoundInfos=(tdstTypeObjectInfos*)SND_fn_pvReAllocSndEx(E_ucSndBlockMain,g_d_pstTypeObjectSoundInfos,(id_type+1)*sizeof(tdstTypeObjectInfos));
        }

        memset(&g_d_pstTypeObjectSoundInfos[id_type],0,sizeof(tdstTypeObjectInfos));
		g_d_pstTypeObjectSoundInfos[id_type].pstLastEvent=SND_C_RES_FANTOME_PTR;
        SNDLST2_M_DynamicInitAnchor(&g_d_pstTypeObjectSoundInfos[id_type].pListVolumeChanged);
        g_d_pstTypeObjectSoundInfos[id_type].lVolume=C_SOUNDVOL_MAXI;

		if (lLine!=NULL)
                g_d_pstTypeObjectSoundInfos[id_type].pstVolumeLine=(tdstVolumeLine *)lLine;
        else
                g_d_pstTypeObjectSoundInfos[id_type].pstVolumeLine=NULL;

		nb_object_type++;

		fn_vUpdateTypeObjectSoundInfos(&g_d_pstTypeObjectSoundInfos[id_type].pfnRetObjectSound,(tdstAllRetObjectSound *)ptrfct);

    SND_pc_fn_vCopyTypeSubTableDefault(id_type);

    SND_RT_fn_vQuitCriticalSectionThreadSnd();
        return id_type;
}

#include <windows.h>
#include "FIL/FIL_pc.h"
#include "SND/sndfile.h"
#include "ERM/CpaError.h"
#include "MMG/MMG_alloc.h"
#include "MMG/MMG_pubaccess.h"
#include "MMG/MMG_module.h"
#include "MMG/MMG_globals.h"

#define SNDERR_MAX_DISPLAYED_ERROR  0x10

#define SND_M_UpdateLastError(ulChannelId, uwErrNum, lDebugData, ucOpenInfoWindow, ucStopForDebug, message) \
    SND_fn_vUpdateLastError(ulChannelId, uwErrNum, lDebugData, ucOpenInfoWindow, ucStopForDebug, message)
#define SND_M_ClearLastError(ulChannelId) Erm_M_ClearLastError(ulChannelId)
#define SND_M_CheckError(ulChannelId)     Erm_fn_uwCheckError(g_ucSndModuleId, ulChannelId)

/* 0x00451540 */
void SND_fn_vUpdateLastError(unsigned long ulChannelId, unsigned short uwErrNum,
                             long lDebugData, unsigned char ucOpenInfoWindow,
                             unsigned char ucStopForDebug, char *message)
{
    if (uwErrNum > SNDERR_MAX_DISPLAYED_ERROR)
    {
        Erm_M_UpdateLastError(Snd, ulChannelId, uwErrNum, lDebugData, 0, 0, message);
    }
    else
    {
        Erm_M_UpdateLastError(Snd, ulChannelId, uwErrNum, lDebugData,
                              ucOpenInfoWindow, ucStopForDebug, message);
    }
}

/* 0x00451450 */
void __stdcall SND_fn_vDisplayError(unsigned short uwErrNum, char *message)
{
    SND_fn_vEnterCriticalSectionForErrorDisplay();
    SND_M_UpdateLastError((unsigned long)GetCurrentThreadId(), uwErrNum,
                          C_lErmNoDebugData, C_ucErmOpenInfoWindow,
                          C_ucAllowStopForDebug, message);
    SND_M_CheckError((unsigned long)GetCurrentThreadId());
    SND_M_ClearLastError((unsigned long)GetCurrentThreadId());
    SND_fn_vQuitCriticalSectionForErrorDisplay();
}

/* 0x004514B0 */
void __stdcall SND_fn_vDisplayErrorEx(unsigned short uwErrNum, char *message,
                                      int flags)
{
    (void)flags;
    SND_fn_vDisplayError(uwErrNum, message);
}

void __cdecl SND_fn_vCheckMemory(void)
{
}

#define SND_M_fn_p_vGenAlloc(ulSize, ucChannel) \
    (((void *(__cdecl *)(unsigned long, unsigned char))fn_p_vGenAlloc)((ulSize), (ucChannel)))
#define SND_M_fn_vGenFree(p_vBlock, ucChannel) \
    (((void (__cdecl *)(void *, unsigned char))fn_vGenFree)((p_vBlock), (ucChannel)))

void __stdcall SND_fn_vFreeSndEx(unsigned char id_block, void *pvPtr)
{
    unsigned char ucChannel;

    GetCurrentThreadId();
    SND_fn_vCheckMemory();              /* 0x403290 */
    ucChannel = Mmg_fn_ucGiveChannelId(GetCurrentThreadId(), 0);
    Mmg_fn_v_SetModeAlloc(g_ucSndModuleId, id_block, ucChannel);
    Mmg_fn_vTestMallocIntegrityAt(pvPtr);   /* 0x403290 */
    SND_M_fn_vGenFree(pvPtr, ucChannel);

    SND_fn_vEnterCriticalSectionForErrorDisplay();
    if (Erm_fn_uwCheckError(MMG_g_ucErmChannelId,
                            (unsigned long)GetCurrentThreadId()) != 0) {
        ERM_fn_vClearLastError((unsigned long)GetCurrentThreadId());
        SND_fn_vDisplayError(8, (char *)SND_C_szEmpty);
    }
    SND_fn_vQuitCriticalSectionForErrorDisplay();

    Mmg_fn_vReleaseChannelId(ucChannel);
    SND_fn_vCheckMemory();              /* 0x403290 */
}

void __stdcall SND_fn_vInitMallocSnd(void)
{
    Mmg_fn_v_InitMmg(g_ucSndModuleId, 2, 0x186A0);
    Mmg_fn_vInitSpecificBlock(0, g_ucSndModuleId,
                              g_a_p_stMmgModuleBlocksInfo[g_ucSndModuleId],
                              0x19000, 0x186A0, 1, 0);
    Mmg_fn_vInitSpecificBlock(1, g_ucSndModuleId,
                              g_a_p_stMmgModuleBlocksInfo[g_ucSndModuleId],
                              0x2800, 0x186A0, 1, 0);

    SND_fn_vEnterCriticalSectionForErrorDisplay();
    if (Erm_fn_uwCheckError(g_ucSndModuleId,
                            (unsigned long)GetCurrentThreadId()) != 0) {
        ERM_fn_vClearLastError((unsigned long)GetCurrentThreadId());
        ERM_fn_vUpdateLastError(7, g_ucSndModuleId,
                                (unsigned long)GetCurrentThreadId(),
                                1, -1, 0xFF, 0xFF, SND_C_szEmpty);
    }
    SND_fn_vQuitCriticalSectionForErrorDisplay();
}

void __stdcall SND_fn_vDesInitMallocSnd(void)
{
    Mmg_fn_vFreeBlock(0, g_ucSndModuleId,
                      g_a_p_stMmgModuleBlocksInfo[g_ucSndModuleId]);
    Mmg_fn_vFreeBlock(1, g_ucSndModuleId,
                      g_a_p_stMmgModuleBlocksInfo[g_ucSndModuleId]);
    Mmg_fn_vDeleteBlock(0, g_ucSndModuleId,
                        g_a_p_stMmgModuleBlocksInfo[g_ucSndModuleId]);
    Mmg_fn_vDeleteBlock(1, g_ucSndModuleId,
                        g_a_p_stMmgModuleBlocksInfo[g_ucSndModuleId]);
}

void *__stdcall SND_fn_pvMallocSnd(unsigned long nb_octets)
{
    void          *ptr_temp;
    unsigned char  ucChannel;

    ucChannel = Mmg_fn_ucGiveChannelId(GetCurrentThreadId(), 0);
    Mmg_fn_v_SetModeAlloc(g_ucSndModuleId, 0xFF, ucChannel);
    ptr_temp = SND_M_fn_p_vGenAlloc(nb_octets, ucChannel);

    SND_fn_vEnterCriticalSectionForErrorDisplay();
    if (Erm_fn_uwCheckError(MMG_g_ucErmChannelId,
                            (unsigned long)GetCurrentThreadId()) != 0) {
        SND_PrintUsedStaticMemory();
        ERM_fn_vClearLastError((unsigned long)GetCurrentThreadId());
        SND_fn_vDisplayError(8, (char *)SND_C_szEmpty);
        ptr_temp = NULL;
    }
    SND_fn_vQuitCriticalSectionForErrorDisplay();

    Mmg_fn_vReleaseChannelId(ucChannel);
    return ptr_temp;
}

void *__stdcall SND_fn_pvReAllocSnd(void *pvPtr, unsigned long ulNbBytes)
{
    unsigned char  ucChannel;
    void          *pvNew;

    ucChannel = Mmg_fn_ucGiveChannelId(GetCurrentThreadId(), 0);
    Mmg_fn_v_SetModeAlloc(g_ucSndModuleId, 0xFF, ucChannel);
    pvNew = fn_p_vGenRealloc(pvPtr, ulNbBytes, ucChannel);

    SND_fn_vEnterCriticalSectionForErrorDisplay();
    if (Erm_fn_uwCheckError(MMG_g_ucErmChannelId,
                            (unsigned long)GetCurrentThreadId()) != 0) {
        SND_PrintUsedStaticMemory();
        ERM_fn_vClearLastError((unsigned long)GetCurrentThreadId());
        SND_fn_vDisplayError(8, (char *)SND_C_szEmpty);
    }
    SND_fn_vQuitCriticalSectionForErrorDisplay();

    Mmg_fn_vReleaseChannelId(ucChannel);
    return pvNew;
}

void __stdcall SND_fn_vFreeSnd(void *pvPtr)
{
    unsigned char ucChannel;

    ucChannel = Mmg_fn_ucGiveChannelId(GetCurrentThreadId(), 0);
    Mmg_fn_v_SetModeAlloc(g_ucSndModuleId, 0xFF, ucChannel);
    Mmg_fn_vTestMallocIntegrityAt(pvPtr);  /* 0x403290 */
    SND_M_fn_vGenFree(pvPtr, ucChannel);

    SND_fn_vEnterCriticalSectionForErrorDisplay();
    if (Erm_fn_uwCheckError(MMG_g_ucErmChannelId,
                            (unsigned long)GetCurrentThreadId()) != 0) {
        ERM_fn_vClearLastError((unsigned long)GetCurrentThreadId());
        SND_fn_vDisplayError(8, (char *)SND_C_szEmpty);
    }
    SND_fn_vQuitCriticalSectionForErrorDisplay();

    Mmg_fn_vReleaseChannelId(ucChannel);
}

void __stdcall SND_fn_vResizeMemoryBlockSnd(unsigned char id_block, unsigned long ulSize)
{
    Mmg_fn_vInitSpecificBlock(id_block, g_ucSndModuleId,
                              g_a_p_stMmgModuleBlocksInfo[g_ucSndModuleId],
                              ulSize, 0x186A0, 1, 0);
}

void *__stdcall SND_fn_pvMallocSndEx(unsigned char id_block, unsigned int ulNbBytes)
{
    unsigned char  ucChannel;
    void          *ptr_temp;

    SND_fn_vCheckMemory();             /* 0x403290 */
    ucChannel = Mmg_fn_ucGiveChannelId(GetCurrentThreadId(), 0);
    Mmg_fn_v_SetModeAlloc(g_ucSndModuleId, id_block, ucChannel);
    ptr_temp = SND_M_fn_p_vGenAlloc(ulNbBytes, ucChannel);

    SND_fn_vEnterCriticalSectionForErrorDisplay();
    if (Erm_fn_uwCheckError(MMG_g_ucErmChannelId,
                            (unsigned long)GetCurrentThreadId()) != 0) {
        SND_fn_vCheckMemory();
        ERM_fn_vClearLastError((unsigned long)GetCurrentThreadId());
        SND_fn_vDisplayError(8, (char *)SND_C_szEmpty);
        ptr_temp = NULL;
    }
    SND_fn_vQuitCriticalSectionForErrorDisplay();

    Mmg_fn_vReleaseChannelId(ucChannel);
    SND_fn_vCheckMemory();             /* 0x403290 */
    return ptr_temp;
}

void *__stdcall SND_fn_pvReAllocSndEx(unsigned char id_block, void *pvPtr,
                                      unsigned int ulNbBytes)
{
    unsigned char  ucChannel;
    void          *p_vPtr;

    SND_fn_vCheckMemory();             /* 0x403290 */
    ucChannel = Mmg_fn_ucGiveChannelId(GetCurrentThreadId(), 0);
    Mmg_fn_v_SetModeAlloc(g_ucSndModuleId, id_block, ucChannel);
    p_vPtr = fn_p_vGenRealloc(pvPtr, ulNbBytes, ucChannel);

    SND_fn_vEnterCriticalSectionForErrorDisplay();
    if (Erm_fn_uwCheckError(MMG_g_ucErmChannelId,
                            (unsigned long)GetCurrentThreadId()) != 0) {
        SND_fn_vCheckMemory();
        ERM_fn_vClearLastError((unsigned long)GetCurrentThreadId());
        SND_fn_vDisplayError(8, (char *)SND_C_szEmpty);
    }
    SND_fn_vQuitCriticalSectionForErrorDisplay();

    Mmg_fn_vReleaseChannelId(ucChannel);
    SND_fn_vCheckMemory();             /* 0x403290 */
    return p_vPtr;
}

#if defined(_DLL_COMPILATION_MODE)
typedef long (__stdcall *t_SND_PlayDriverFn)(long, void *, long, void *, long);

t_SND_PlayDriverFn SND_g_pfnPlaySxdDriver;
t_SND_PlayDriverFn SND_g_pfnPlayMidiDriver;
t_SND_PlayDriverFn SND_g_pfnPlayCDDriver;
#define SND_M_g_pfnPlaySxd   (SND_g_pfnPlaySxdDriver)
#define SND_M_g_pfnPlayMidi  (SND_g_pfnPlayMidiDriver)
#define SND_M_g_pfnPlayCD    (SND_g_pfnPlayCDDriver)

/* 0x00454F40 */
long __stdcall SND_fn_lPlaySxd(long a1, void *a2, long a3, void *a4, long a5)
{
    if (SND_M_g_pfnPlaySxd == NULL) DebugBreak();
    return SND_M_g_pfnPlaySxd(a1, a2, a3, a4, a5);
}

/* 0x00455580 */
long __stdcall SND_fn_lPlayMidi(long a1, void *a2, long a3, void *a4, long a5)
{
    if (SND_M_g_pfnPlayMidi == NULL) DebugBreak();
    return SND_M_g_pfnPlayMidi(a1, a2, a3, a4, a5);
}

/* 0x00455940 */
long __stdcall SND_pc_fn_lPlayCD(long a1, void *a2, long a3, void *a4, long a5)
{
    if (SND_M_g_pfnPlayCD == NULL) DebugBreak();
    return SND_M_g_pfnPlayCD(a1, a2, a3, a4, a5);
}

#endif
#include <stdio.h>
#if defined(_DLL_COMPILATION_MODE)

static long SND_g_lDbgProcsLoaded;              /* 0x004B8900 */

typedef void (__stdcall *t_SND_Void2Fn)(long, void *);
typedef void (__stdcall *t_SND_Void3Fn)(long, long, void *);
typedef void *(__stdcall *t_SND_PtrVoidFn)(void);
static t_SND_PerTypeFn g_pfn_SND_AddMicro_s;            /* 0x004B8884 */
static t_SND_PerTypeFn g_pfn_SND_KillVolumeLine_s;      /* 0x004B8894 */
static t_SND_PerTypeFn g_pfn_SND_LoadBank_s;            /* 0x004B88A0 */
static t_SND_PerTypeFn g_pfn_SND_UnLoadBank_s;          /* 0x004B88A4 */
static t_SND_Void3Fn   g_pfn_SND_AddVoiceSample_s;      /* 0x004B88A8 */
static t_SND_Void2Fn   g_pfn_SND_SetParVoiceSample_s;   /* 0x004B88AC */
static t_SND_PerTypeFn g_pfn_SND_DelVoiceSample_s;      /* 0x004B88B0 */
static t_SND_PerTypeFn g_pfn_SND_PauseVoiceSample_s;    /* 0x004B88B4 */
static t_SND_PerTypeFn g_pfn_SND_ResumeVoiceSample_s;   /* 0x004B88B8 */
static t_SND_PerTypeFn g_pfn_SND_SetProcessTimeSample_s;/* 0x004B88BC */
static t_SND_PtrVoidFn g_pfn_SND_GetSelectedResource_s; /* 0x004B88C0 */
static t_SND_GetBoolFn g_pfn_SND_DoesSelectResource_s;  /* 0x004B88C4 */
static t_SND_VoidPtrFn g_pfn_SND_RajListesSxdVirtual2_s;/* 0x004B88C8 */
static t_SND_Void2Fn   g_pfn_SND_InitListesSxdVirtual_s;/* 0x004B88CC */
static t_SND_VoidFn    g_pfn_SND_RefreshSupervisater_s; /* 0x004B88D0 */
static t_SND_VoidPtrFn g_pfn_SND_SetProjectTitle_s;     /* 0x004B8874 */
static t_SND_GetBoolFn g_pfn_SND_IsSupervisaterActive_s;/* 0x004B8870 */
#endif

/* 0x00455CE0 */
void __stdcall SND_pc_fn_vNopStd4(long a)                                 { (void)a; }
/* 0x00454F30 */
SndBool __stdcall SND_pc_fn_bNop0(void)                                   { return 0; }

#if defined(_DLL_COMPILATION_MODE)

/* 0x00455D10 */
FARPROC __cdecl SND_pc_fn_pfnGetDriverProc(HMODULE hModule, char *pszName)
{
    FARPROC pfn;
    char szModuleName[0x100];
    char szMsg[0x100];

    pfn = GetProcAddress(hModule, pszName);
    if (pfn == NULL)
    {
        GetModuleFileNameA(hModule, szModuleName, 0x100);
        sprintf(szMsg, "Function cannot be loaded dynamically:\n%.20s\nfrom module %.80s",
                pszName, szModuleName);
        SND_fn_vDisplayError(0x1d, szMsg);
    }
    return pfn;
}

#define SND_M_DbgSlotLoad(slot, type, name, fallback)                              \
    if (hModule != NULL)                                                           \
    {                                                                              \
        (slot) = (type)SND_pc_fn_pfnGetDriverProc(hModule, (name));                \
        if ((slot) == NULL) (slot) = (type)(fallback);                             \
    }                                                                              \
    else (slot) = (type)(fallback)

void __stdcall SND_pc_fn_vLaunchSupervisaterDefault(long a)                     { (void)a; }  /* 0x00455CE0 */
void __stdcall SND_pc_fn_vKillSupervisaterDefault(void)                         { }  /* 0x00403290 */
void __stdcall SND_pc_fn_vSetSupervisaterActiveDefault(void)                    { }  /* 0x00403290 */
SndBool __stdcall SND_pc_fn_bIsSupervisaterActiveDefault(void)                  { return 0; }  /* 0x00454F30 */
void __stdcall SND_pc_fn_vSetProjectTitleForSuperVisaterDefault(long a)         { (void)a; }  /* 0x00455CE0 */
void __stdcall SND_pc_fn_vAddEventToObjectDefault(long a, long b, long c, long d) { (void)a; (void)b; (void)c; (void)d; }  /* 0x00455B40 */
void __stdcall SND_pc_fn_vKillEventToObjectDefault(long a, long b)              { (void)a; (void)b; }  /* 0x00455180 */
void __stdcall SND_pc_fn_vSetPosObjectDefault(long a, long b)                   { (void)a; (void)b; }  /* 0x00455180 */
void __stdcall SND_pc_fn_vAddMicroDefault(long a)                               { (void)a; }  /* 0x00455CE0 */
void __stdcall SND_pc_fn_vKillMicroDefault(long a)                              { (void)a; }  /* 0x00455CE0 */
void __stdcall SND_pc_fn_vSetPosMicroDefault(long a, long b)                    { (void)a; (void)b; }  /* 0x00455180 */
void __stdcall SND_pc_fn_vAddHistoricDefault(long a, long b, long c, long d, long e) { (void)a; (void)b; (void)c; (void)d; (void)e; }  /* 0x00455C60 */
void __stdcall SND_pc_fn_vKillVolumeLineDefault(long a)                         { (void)a; }  /* 0x00455CE0 */
void __stdcall SND_pc_fn_vAddVolumeLineDefault(long a)                          { (void)a; }  /* 0x00455CE0 */
void __stdcall SND_pc_fn_vSetVolumeLineDefault(long a, long b)                  { (void)a; (void)b; }  /* 0x00455180 */
void __stdcall SND_pc_fn_vLoadBankDefault(long a)                               { (void)a; }  /* 0x00455CE0 */
void __stdcall SND_pc_fn_vUnLoadBankDefault(long a)                             { (void)a; }  /* 0x00455CE0 */
void __stdcall SND_pc_fn_vAddVoiceSampleDefault(long a, long b, long c)         { (void)a; (void)b; (void)c; }  /* 0x004551F0 */
void __stdcall SND_pc_fn_vSetParVoiceSampleDefault(long a, long b)              { (void)a; (void)b; }  /* 0x00455180 */
void __stdcall SND_pc_fn_vDelVoiceSampleDefault(long a)                         { (void)a; }  /* 0x00455CE0 */
void __stdcall SND_pc_fn_vPauseVoiceSampleDefault(long a)                       { (void)a; }  /* 0x00455CE0 */
void __stdcall SND_pc_fn_vResumeVoiceSampleDefault(long a)                      { (void)a; }  /* 0x00455CE0 */
void __stdcall SND_pc_fn_vSetProcessTimeSampleDefault(long a)                   { (void)a; }  /* 0x00455CE0 */
void * __stdcall SND_pc_fn_pstGetSelectedResourceDefault(void)                  { return 0; }  /* 0x00454F30 */
SndBool __stdcall SND_pc_fn_bDoesSupervisaterSelectResourceDefault(void)        { return 0; }  /* 0x00454F30 */
void __stdcall SND_pc_fn_vRajListesSxdVirtual2Default(long a)                   { (void)a; }  /* 0x00455CE0 */
void __stdcall SND_pc_fn_vInitListesSxdVirtualDefault(long a, long b)           { (void)a; (void)b; }  /* 0x00455180 */
void __stdcall SND_pc_fn_vRefreshSupervisaterDefault(void)                      { }  /* 0x00403290 */

/* 0x00457040 */
void __cdecl SND_pc_fn_vLoadDbgProcs(HMODULE hModule)
{
    if (SND_g_lDbgProcsLoaded != 0) return;

    SND_M_DbgSlotLoad(g_pfn_SND_LaunchSupervisater_s, t_SND_VoidPtrFn, "_SND_fn_vLaunchSupervisater@4", SND_pc_fn_vLaunchSupervisaterDefault);
    SND_M_DbgSlotLoad(g_pfn_SND_KillSupervisater_s, t_SND_VoidFn, "_SND_fn_vKillSupervisater@0", SND_pc_fn_vKillSupervisaterDefault);
    SND_M_DbgSlotLoad(g_pfn_SND_SetSupervisaterActive_s, t_SND_VoidFn, "_SND_fn_vSetSupervisaterActive@0", SND_pc_fn_vSetSupervisaterActiveDefault);
    SND_M_DbgSlotLoad(g_pfn_SND_IsSupervisaterActive_s, t_SND_GetBoolFn, "_SND_fn_bIsSupervisaterActive@0", SND_pc_fn_bIsSupervisaterActiveDefault);
    SND_M_DbgSlotLoad(g_pfn_SND_SetProjectTitle_s, t_SND_VoidPtrFn, "_dbgSND_fn_vSetProjectTitleForSuperVisater@4", SND_pc_fn_vSetProjectTitleForSuperVisaterDefault);
    SND_M_DbgSlotLoad(g_pfn_SND_AddEventToObject, t_SND_fn_vAddEventToObject, "_dbgSND_fn_vAddEventToObject@16", SND_pc_fn_vAddEventToObjectDefault);
    SND_M_DbgSlotLoad(g_pfn_SND_KillEventToObject, t_SND_fn_vKillEventToObject, "_dbgSND_fn_vKillEventToObject@8", SND_pc_fn_vKillEventToObjectDefault);
    SND_M_DbgSlotLoad(g_pfn_SND_SetPosObject, t_SND_cbMicroPost, "_dbgSND_fn_vSetPosObject@8", SND_pc_fn_vSetPosObjectDefault);
    SND_M_DbgSlotLoad(g_pfn_SND_AddMicro_s, t_SND_PerTypeFn, "_dbgSND_fn_vAddMicro@4", SND_pc_fn_vAddMicroDefault);
    SND_M_DbgSlotLoad(g_pfn_SND_KillMicro_s, t_SND_PerTypeFn, "_dbgSND_fn_vKillMicro@4", SND_pc_fn_vKillMicroDefault);
    SND_M_DbgSlotLoad(g_pfn_SND_SetPosMicro, t_SND_fn_vSetPosMicro, "_dbgSND_fn_vSetPosMicro@8", SND_pc_fn_vSetPosMicroDefault);
    SND_M_DbgSlotLoad(g_pfn_SND_AddHistoric, t_SND_fn_vAddHistoric, "_dbgSND_fn_vAddHistoric@20", SND_pc_fn_vAddHistoricDefault);
    SND_M_DbgSlotLoad(g_pfn_SND_KillVolumeLine_s, t_SND_PerTypeFn, "_dbgSND_fn_vKillVolumeLine@4", SND_pc_fn_vKillVolumeLineDefault);
    SND_M_DbgSlotLoad(g_pfn_SND_AddVolumeLine, t_SND_fn_vAddVolumeLine, "_dbgSND_fn_vAddVolumeLine@4", SND_pc_fn_vAddVolumeLineDefault);
    SND_M_DbgSlotLoad(g_pfn_SND_SetVolumeLine, t_SND_fn_vSetVolumeLine, "_dbgSND_fn_vSetVolumeLine@8", SND_pc_fn_vSetVolumeLineDefault);
    SND_M_DbgSlotLoad(g_pfn_SND_LoadBank_s, t_SND_PerTypeFn, "_dbgSND_fn_vLoadBank@4", SND_pc_fn_vLoadBankDefault);
    SND_M_DbgSlotLoad(g_pfn_SND_UnLoadBank_s, t_SND_PerTypeFn, "_dbgSND_fn_vUnLoadBank@4", SND_pc_fn_vUnLoadBankDefault);
    SND_M_DbgSlotLoad(g_pfn_SND_AddVoiceSample_s, t_SND_Void3Fn, "_dbgSND_fn_vAddVoiceSample@12", SND_pc_fn_vAddVoiceSampleDefault);
    SND_M_DbgSlotLoad(g_pfn_SND_SetParVoiceSample_s, t_SND_Void2Fn, "_dbgSND_fn_vSetParVoiceSample@8", SND_pc_fn_vSetParVoiceSampleDefault);
    SND_M_DbgSlotLoad(g_pfn_SND_DelVoiceSample_s, t_SND_PerTypeFn, "_dbgSND_fn_vDelVoiceSample@4", SND_pc_fn_vDelVoiceSampleDefault);
    SND_M_DbgSlotLoad(g_pfn_SND_PauseVoiceSample_s, t_SND_PerTypeFn, "_dbgSND_fn_vPauseVoiceSample@4", SND_pc_fn_vPauseVoiceSampleDefault);
    SND_M_DbgSlotLoad(g_pfn_SND_ResumeVoiceSample_s, t_SND_PerTypeFn, "_dbgSND_fn_vResumeVoiceSample@4", SND_pc_fn_vResumeVoiceSampleDefault);
    SND_M_DbgSlotLoad(g_pfn_SND_SetProcessTimeSample_s, t_SND_PerTypeFn, "_dbgSND_fn_vSetProcessTimeSample@4", SND_pc_fn_vSetProcessTimeSampleDefault);
    SND_M_DbgSlotLoad(g_pfn_SND_GetSelectedResource_s, t_SND_PtrVoidFn, "_dbgSND_fn_pstGetSelectedResource@0", SND_pc_fn_pstGetSelectedResourceDefault);
    SND_M_DbgSlotLoad(g_pfn_SND_DoesSelectResource_s, t_SND_GetBoolFn, "_dbgSND_fn_bDoesSupervisaterSelectResource@0", SND_pc_fn_bDoesSupervisaterSelectResourceDefault);
    SND_M_DbgSlotLoad(g_pfn_SND_RajListesSxdVirtual2_s, t_SND_VoidPtrFn, "_dbgSND_fn_vRajListesSxdVirtual2@4", SND_pc_fn_vRajListesSxdVirtual2Default);
    SND_M_DbgSlotLoad(g_pfn_SND_InitListesSxdVirtual_s, t_SND_Void2Fn, "_dbgSND_fn_vInitListesSxdVirtual@8", SND_pc_fn_vInitListesSxdVirtualDefault);
    SND_M_DbgSlotLoad(g_pfn_SND_RefreshSupervisater_s, t_SND_VoidFn, "_dbgSND_fn_vRefreshSupervisater@0", SND_pc_fn_vRefreshSupervisaterDefault);

    SND_g_lDbgProcsLoaded = 1;
}

static long SND_g_lCanFreeMPEGLoaded;           /* 0x004B88F4 */
static long SND_g_lCanFreeADPCMLoaded;          /* 0x004B8904 */

void __cdecl SND_pc_fn_vLoadCanFreeDataMPEG(HMODULE hModule)
{
    if (SND_g_lCanFreeMPEGLoaded == 0)
    {
        if (hModule == NULL ||
            (g_pfn_SND_CanFreeDataMPEG_s = (t_SND_GetBoolFn)SND_pc_fn_pfnGetDriverProc(hModule, "_SND_fn_bCanFreeDataMPEG@0")) == NULL)
        {
            g_pfn_SND_CanFreeDataMPEG_s = SND_pc_fn_bNop0;
        }
        SND_g_lCanFreeMPEGLoaded = 1;
    }
}

void __cdecl SND_pc_fn_vLoadCanFreeDataADPCM(HMODULE hModule)
{
    if (SND_g_lCanFreeADPCMLoaded == 0)
    {
        if (hModule == NULL ||
            (g_pfn_SND_CanFreeDataADPCM_s = (t_SND_GetBoolFn)SND_pc_fn_pfnGetDriverProc(hModule, "_SND_fn_bCanFreeDataADPCM@0")) == NULL)
        {
            g_pfn_SND_CanFreeDataADPCM_s = SND_pc_fn_bNop0;
        }
        SND_g_lCanFreeADPCMLoaded = 1;
    }
}
#endif

/* 0x0045B430 */
void SND_pc_fn_vProbeCpu(long *p_lHasCpuid, long *p_lHasMMX)
{
    __asm
    {
        pushfd
        mov     esi, p_lHasCpuid
        mov     edi, p_lHasMMX
        pop     eax
        mov     ebx, eax
        xor     eax, 0x200000
        push    eax
        popfd
        pushfd
        pop     eax
        push    ebx
        popfd
        and     eax, 0x200000
        and     ebx, 0x200000
        cmp     eax, ebx
        jnz     l_HasCpuid
        mov     dword ptr [esi], 0
        mov     dword ptr [edi], 0
        jmp     l_Done
    l_HasCpuid:
        mov     dword ptr [esi], 1
        mov     eax, 1
        _emit   0x0f
        _emit   0xa2
        test    edx, 0x800000
        jnz     l_HasMMX
        mov     dword ptr [edi], 0
        jmp     l_Done
    l_HasMMX:
        mov     dword ptr [edi], 1
    l_Done:
    }
}

/* 0x0045B490 */
SndBool __stdcall SND_fn_bTestSnd_Pentium(void)
{
    long lHasCpuid;
    long lHasMMX;

    SND_pc_fn_vProbeCpu(&lHasCpuid, &lHasMMX);
    return lHasCpuid;
}

/* 0x0045B4B0 */
SndBool __stdcall SND_fn_bTestSnd_MMX(void)
{
    long lHasCpuid;
    long lHasMMX;

    SND_pc_fn_vProbeCpu(&lHasCpuid, &lHasMMX);
    if (lHasCpuid && lHasMMX)
        return 1;
    return 0;
}

/* 0x0045B4F0 */
SndBool __stdcall SND_fn_bTestSnd_WinMM(WAVEFORMATEX *p_stFormat)
{
    unsigned long a_ulFormatBits[12];
    WAVEOUTCAPSA  stCaps;
    long          lRateIndex;

    a_ulFormatBits[0]  = WAVE_FORMAT_1M08;
    a_ulFormatBits[1]  = WAVE_FORMAT_1S08;
    a_ulFormatBits[2]  = WAVE_FORMAT_1M16;
    a_ulFormatBits[3]  = WAVE_FORMAT_1S16;
    a_ulFormatBits[4]  = WAVE_FORMAT_2M08;
    a_ulFormatBits[5]  = WAVE_FORMAT_2S08;
    a_ulFormatBits[6]  = WAVE_FORMAT_2M16;
    a_ulFormatBits[7]  = WAVE_FORMAT_2S16;
    a_ulFormatBits[8]  = WAVE_FORMAT_4M08;
    a_ulFormatBits[9]  = WAVE_FORMAT_4S08;
    a_ulFormatBits[10] = WAVE_FORMAT_4M16;
    a_ulFormatBits[11] = WAVE_FORMAT_4S16;
    if (waveOutGetDevCapsA(WAVE_MAPPER, &stCaps, sizeof(stCaps)) == 0)
    {
        if (p_stFormat->nSamplesPerSec <= 11025)
            lRateIndex = 0;
        else
            lRateIndex = (p_stFormat->nSamplesPerSec > 22050) ? 8 : 4;
        if (a_ulFormatBits[lRateIndex + (p_stFormat->nChannels - 1) + ((p_stFormat->wBitsPerSample >> 3) - 1) * 2]
            & stCaps.dwFormats)
            return 1;
    }
    return 0;
}

/* 0x0045B5C0 */
SndBool __stdcall SND_fn_bTestSnd_Win32(void)
{
    OSVERSIONINFOA stVersion;
    unsigned long *pulCur;
    int iCount;

    pulCur = (unsigned long *)&stVersion;
    for (iCount = 0x25; iCount != 0; iCount = iCount - 1)
    {
        *pulCur = 0;
        pulCur = pulCur + 1;
    }
    stVersion.dwOSVersionInfoSize = 0x94;
    GetVersionExA(&stVersion);
    return stVersion.dwPlatformId != 0;
}

/* 0x0045B600 */
SndBool __stdcall SND_fn_bTestSnd_WinNT(void)
{
    OSVERSIONINFOA stVersion;
    unsigned long *pulCur;
    int iCount;

    pulCur = (unsigned long *)&stVersion;
    for (iCount = 0x25; iCount != 0; iCount = iCount - 1)
    {
        *pulCur = 0;
        pulCur = pulCur + 1;
    }
    stVersion.dwOSVersionInfoSize = 0x94;
    GetVersionExA(&stVersion);
    return stVersion.dwPlatformId == 2;
}

static char SND_pc_szDllDir[0x100];       /* 0x004A3E18 */
static char SND_pc_szCpaIniPath[0x100];   /* 0x004B94B8 */

/* 0x004593C0 */
SndBool __stdcall SND_fn_bGetOptions(LPCSTR pszSection, LPCSTR pszKey, char *pszValue, DWORD ulSize)
{
    FIL_pc_M_ulGetProfileString(pszSection, pszKey, "None", pszValue, ulSize, SND_pc_szCpaIniPath);
    return strcmp(pszValue, "None") != 0;
}

/* 0x00459440 */
void __stdcall SND_fn_vSetOptions(LPCSTR pszSection, LPCSTR pszKey, LPCSTR pszValue)
{
    FIL_pc_M_bWriteProfileString(pszSection, pszKey, pszValue, SND_pc_szCpaIniPath);
}

#if defined(_DLL_COMPILATION_MODE)

#pragma intrinsic(strcmp)
HMODULE __cdecl SND_pc_fn_hLoadDriverDll(LPCSTR pszSection, HMODULE *phModules, int iCount)
{
    char szValue[0x100];
    char szDllPath[0x100];
    char szKey[0x100];
    FARPROC pfn;
    HMODULE hLib;
    int iIndex;
    int iProc;
    int bContinue;

    /* 0x004588B0 */
    pfn = NULL;
    FIL_pc_M_ulGetProfileString(pszSection, "Unconditionnal", "None", szValue, 0xff, SND_pc_szCpaIniPath);
    if (strcmp(szValue, "None") != 0)
    {
        sprintf(szDllPath, "%s\\%s%c%c%c.dll", SND_pc_szDllDir, szValue, 'b', 'v', 'r');
        hLib = LoadLibraryA(szDllPath);
        if (hLib == NULL)
        {
            SND_fn_vDisplayError(0x0f, szDllPath);
        }
        return hLib;
    }

    iIndex = 0;
    bContinue = 1;
    do
    {
        sprintf(szKey, "Test%x", iIndex);
        FIL_pc_M_ulGetProfileString(pszSection, szKey, "None", szValue, 0xff, SND_pc_szCpaIniPath);
        if (strcmp(szValue, "None") != 0)
        {
            for (iProc = 0; iProc < iCount; iProc++)
            {
                pfn = GetProcAddress(phModules[iProc], szValue);
                if (pfn != NULL) break;
            }
            if (pfn != NULL)
            {
                if (((int (__cdecl *)(void))pfn)())
                {
                    sprintf(szKey, "File%x", iIndex);
                    FIL_pc_M_ulGetProfileString(pszSection, szKey, "None", szValue, 0xff, SND_pc_szCpaIniPath);
                    bContinue = 0;
                }
            }
            else
            {
                SND_fn_vDisplayError(0x0e, szValue);
            }
            iIndex++;
        }
        else
        {
            FIL_pc_M_ulGetProfileString(pszSection, "Default", "None", szValue, 0xff, SND_pc_szCpaIniPath);
            bContinue = 0;
        }
    } while (bContinue);

    if (strcmp(szValue, "None") != 0)
    {
        sprintf(szDllPath, "%s\\%s%c%c%c.dll", SND_pc_szDllDir, szValue, 'b', 'v', 'r');
        hLib = LoadLibraryA(szDllPath);
        if (hLib == NULL)
        {
            GetLastError();
            SND_fn_vDisplayError(0x0f, szDllPath);
        }
        return hLib;
    }
    return NULL;
}
#endif

/* 0x00457B00 */
void __cdecl SND_fn_vUnLoadResTheme(tdstBlockResourceMem *mem)
{
    (void)mem;
}

/* 0x0045AB20 */
SndBool __cdecl SND_fn_bLoadResBinarySeq(tdstBlockResourceDisk *_pBRDisk,tdstBlockResourceMem *_pBRMem,char *pDataBloc);

/* 0x00459800 */
SndBool __cdecl SND_fn_bLoadResBinaryTheme(tdstBlockResourceDisk *_pBRDisk,tdstBlockResourceMem *_pBRMem,char *pDataBloc);

/* 0x00457BA0 */
SndBool __cdecl SND_fn_bLoadResBinarySwitch(tdstBlockResourceDisk *_pBRDisk,tdstBlockResourceMem *_pBRMem,char *pDataBloc);
SndBool __cdecl SND_fn_bLoadResBinarySwitchOld(tdstBlockResourceDisk *_pBRDisk,tdstBlockResourceMem *_pBRMem,char *pDataBloc);

/* 0x0045A440 */
SndBool __cdecl SND_fn_bLoadResBinaryRandom(tdstBlockResourceDisk *_pBRDisk,tdstBlockResourceMem *_pBRMem,char *pDataBloc);

/* 0x00457A10 */
SndBool __cdecl SND_fn_bLoadResBinarySnd(tdstBlockResourceDisk* disk,tdstBlockResourceMem* mem,char *pDataBloc);

#pragma intrinsic(strcmp)
void __cdecl SND_pc_fn_vLoadSxdDistances(long *plDist)
{
    char szValue[0x100];
    float fBack;
    float fSatur;

    FIL_pc_M_ulGetProfileString("Options", "SXD_DIST_SATUR", "None", szValue, 0xff, SND_pc_szCpaIniPath);
    if (strcmp(szValue, "None") == 0)
    {
        plDist[0] = 0x7ffffffeL;
    }
    else
    {
        sscanf(szValue, "%f", &fSatur);
        plDist[0] = (long)(fSatur * 65536.0f);
    }
    FIL_pc_M_ulGetProfileString("Options", "SXD_DIST_BACK", "None", szValue, 0xff, SND_pc_szCpaIniPath);
    if (strcmp(szValue, "None") == 0)
    {
        plDist[1] = 0x7ffffffeL;
    }
    else
    {
        sscanf(szValue, "%f", &fBack);
        plDist[1] = (long)(fBack * 65536.0f);
    }
}

#if defined(_DLL_COMPILATION_MODE)

static long SND_g_lRecordProcsLoaded;           /* 0x004B8908 */

typedef SndBool (__stdcall *t_SND_RecTestFormatFn)(void *, int);
typedef long    (__stdcall *t_SND_RecCreateBufferFn)(void *, void *);
typedef long    (__stdcall *t_SND_RecPushBufferFn)(long, void *, int);
typedef unsigned char (__stdcall *t_SND_UcharVoidFn)(void);

static t_SND_RecTestFormatFn   g_pfn_SND_TestFormatRecord_s;         /* 0x004B8764 */
static t_SND_RecCreateBufferFn g_pfn_SND_CreateBufferRecord_s;       /* 0x004B8768 */
static t_SND_PerTypeBoolFn     g_pfn_SND_IsBufferFullDuplexRecord_s; /* 0x004B876C */
static t_SND_PerTypeBoolFn     g_pfn_SND_StartRecord_s;              /* 0x004B8770 */
static t_SND_PerTypeFn         g_pfn_SND_StopRecord_s;               /* 0x004B8774 */
static t_SND_PerTypeBoolFn     g_pfn_SND_IsWorkingRecord_s;          /* 0x004B8778 */
static t_SND_RecPushBufferFn   g_pfn_SND_PushBufferToRecord_s;       /* 0x004B877C */
static t_SND_PerTypeBoolFn     g_pfn_SND_ManageMsgForRecord_s;       /* 0x004B8780 */
static t_SND_UcharVoidFn       g_pfn_SND_GetInstantVuMeterRecord_s;  /* 0x004B878C */

SndBool __stdcall SND_pc_fn_bTestFormatRecordDefault(long a, long b)           { (void)a; (void)b; return 0; }
long __stdcall SND_pc_fn_lCreateBufferRecordDefault(long a, long b)            { (void)a; (void)b; return 0; }
SndBool __stdcall SND_pc_fn_bIsBufferFullDuplexRecordDefault(long a)           { (void)a; return 0; }
SndBool __stdcall SND_pc_fn_bStartRecordDefault(long a)                        { (void)a; return 0; }
void __stdcall SND_pc_fn_vStopRecordDefault(long a)                            { (void)a; }
SndBool __stdcall SND_pc_fn_bIsWorkingRecordDefault(long a)                    { (void)a; return 0; }
long __stdcall SND_pc_fn_lPushBufferToRecordDefault(long a, long b, long c)    { (void)a; (void)b; (void)c; return 0; }
SndBool __stdcall SND_pc_fn_bManageMsgForRecordDefault(long a)                 { (void)a; return 0; }
int __stdcall SND_pc_fn_iInitRecordDefault(void *a)                            { (void)a; return 0; }
void __stdcall SND_pc_fn_vDesInitRecordDefault(void)                           { }
unsigned char __stdcall SND_pc_fn_ucGetInstantVuMeterRecordDefault(void)       { return 0; }

void __cdecl SND_pc_fn_vLoadRecordProcs(HMODULE hModule)
{
    if (SND_g_lRecordProcsLoaded != 0) return;

    SND_M_DbgSlotLoad(g_pfn_SND_TestFormatRecord_s, t_SND_RecTestFormatFn, "_SND_fn_bTestFormatRecord@8", SND_pc_fn_bTestFormatRecordDefault);
    SND_M_DbgSlotLoad(g_pfn_SND_CreateBufferRecord_s, t_SND_RecCreateBufferFn, "_SND_fn_lCreateBufferRecord@8", SND_pc_fn_lCreateBufferRecordDefault);
    SND_M_DbgSlotLoad(g_pfn_SND_IsBufferFullDuplexRecord_s, t_SND_PerTypeBoolFn, "_SND_fn_bIsBufferFullDuplexRecord@4", SND_pc_fn_bIsBufferFullDuplexRecordDefault);
    SND_M_DbgSlotLoad(g_pfn_SND_StartRecord_s, t_SND_PerTypeBoolFn, "_SND_fn_bStartRecord@4", SND_pc_fn_bStartRecordDefault);
    SND_M_DbgSlotLoad(g_pfn_SND_StopRecord_s, t_SND_PerTypeFn, "_SND_fn_vStopRecord@4", SND_pc_fn_vStopRecordDefault);
    SND_M_DbgSlotLoad(g_pfn_SND_IsWorkingRecord_s, t_SND_PerTypeBoolFn, "_SND_fn_bIsWorkingRecord@4", SND_pc_fn_bIsWorkingRecordDefault);
    SND_M_DbgSlotLoad(g_pfn_SND_PushBufferToRecord_s, t_SND_RecPushBufferFn, "_SND_fn_lPushBufferToRecord@12", SND_pc_fn_lPushBufferToRecordDefault);
    SND_M_DbgSlotLoad(g_pfn_SND_ManageMsgForRecord_s, t_SND_PerTypeBoolFn, "_SND_fn_bManageMsgForRecord@4", SND_pc_fn_bManageMsgForRecordDefault);
    SND_M_DbgSlotLoad(g_pfn_SND_InitRecord_s, t_SND_IntPtrFn, "_SND_fn_iInitRecord@4", SND_pc_fn_iInitRecordDefault);
    SND_M_DbgSlotLoad(g_pfn_SND_DesInitRecord_s, t_SND_VoidFn, "_SND_fn_vDesInitRecord@0", SND_pc_fn_vDesInitRecordDefault);
    SND_M_DbgSlotLoad(g_pfn_SND_GetInstantVuMeterRecord_s, t_SND_UcharVoidFn, "_SND_fn_ucGetInstantVuMeterRecord@0", SND_pc_fn_ucGetInstantVuMeterRecordDefault);

    SND_g_lRecordProcsLoaded = 1;
}

static long SND_g_lMidiCdProcsLoaded;           /* 0x004B88FC */

typedef long (__stdcall *t_SND_Play7Fn)(long, long, long, long, long, long, long);

static t_SND_LongLongBoolFn g_pfn_SND_LoadResScriptMidi_s;          /* 0x004B8790 */
static t_SND_PerTypeBoolFn  g_pfn_SND_IsResLoadedMidi_s;            /* 0x004B8798 */
static t_SND_GetBoolFn      g_pfn_SND_TestInitMidi_s;               /* 0x004B87A0 */
static t_SND_Play7Fn        g_pfn_SND_PlayMidiWithFade_s;           /* 0x004B87B0 */
static t_SND_PerTypeFn      g_pfn_SND_RemoveCallbackMidi_s;         /* 0x004B87C0 */
static t_SND_PerTypeRealFn  g_pfn_SND_GetLengthMidi_s;              /* 0x004B87D8 */
static t_SND_PerTypeBoolFn  g_pfn_SND_CheckVersionResourceMidi_s;   /* 0x004B8800 */
static t_SND_LongLongBoolFn g_pfn_SND_LoadResScriptCd_s;            /* 0x004B880C */
static t_SND_GetBoolFn      g_pfn_SND_TestInitCD_s;                 /* 0x004B8818 */
static t_SND_PerTypeFn      g_pfn_SND_RemoveCallbackCD_s;           /* 0x004B8838 */
static t_SND_PerTypeRealFn  g_pfn_SND_GetLengthCD_s;                /* 0x004B8854 */
static t_SND_PerTypeBoolFn  g_pfn_SND_CheckVersionResourceCD_s;     /* 0x004B885C */

SndBool __stdcall SND_pc_fn_bLoadResScriptMidiDefault(long a, long b)           { (void)a; (void)b; return 0; }  /* 0x00454FC0 */
SndBool __stdcall SND_pc_fn_bLoadResBinaryMidiDefault(long a, long b, long c)   { (void)a; (void)b; (void)c; return 0; }  /* 0x00455230 */
SndBool __stdcall SND_pc_fn_bIsResLoadedMidiDefault(long a)                     { (void)a; return 0; }  /* 0x00454FF0 */
SndBool __stdcall SND_pc_fn_bSetParamMidiDefault(long a, long b)                { (void)a; (void)b; return 0; }  /* 0x00454FC0 */
SndBool __stdcall SND_pc_fn_bTestInitMidiDefault(void)                          { return 0; }  /* 0x00454F30 */
SndBool __stdcall SND_pc_fn_bTestIsPlayingMidiDefault(long a)                   { (void)a; return 0; }  /* 0x00454FF0 */
int __stdcall SND_pc_fn_iInitMidiDefault(long a)                                { (void)a; return 0; }  /* 0x00454FF0 */
long __stdcall SND_pc_fn_lPlayMidiDefault(long a, long b, long c, long d, long e) { (void)a; (void)b; (void)c; (void)d; (void)e; return -1; }  /* 0x00454F80 */
long __stdcall SND_pc_fn_lPlayMidiWithFadeDefault(long a, long b, long c, long d, long e, long f, long g) { (void)a; (void)b; (void)c; (void)d; (void)e; (void)f; (void)g; return 0; }  /* 0x004555C0 */
void __stdcall SND_pc_fn_vDesInitMidiDefault(void)                              { }  /* 0x00403290 */
void __stdcall SND_pc_fn_vPauseMidiDefault(long a)                              { (void)a; }  /* 0x00455CE0 */
void __stdcall SND_pc_fn_vResumeMidiDefault(long a)                             { (void)a; }  /* 0x00455CE0 */
void __stdcall SND_pc_fn_vRemoveCallbackMidiDefault(long a)                     { (void)a; }  /* 0x00455CE0 */
void __stdcall SND_pc_fn_vStopMidiDefault(long a)                               { (void)a; }  /* 0x00455CE0 */
void __stdcall SND_pc_fn_vUnLoadResMidiDefault(long a)                          { (void)a; }  /* 0x00455CE0 */
SndBool __stdcall SND_pc_fn_bCanFreeDataMidiDefault(void)                       { return 0; }  /* 0x00454F30 */
void __stdcall SND_pc_fn_vReleaseDriverMidiDefault(void)                        { }  /* 0x00403290 */
void __stdcall SND_pc_fn_vRestoreDriverMidiDefault(void)                        { }  /* 0x00403290 */
long __stdcall SND_pc_fn_rGetPosMidiDefault(long a)                             { (void)a; return 0; }  /* 0x00454FF0 */
long __stdcall SND_pc_fn_rGetLengthMidiDefault(long a)                          { (void)a; return 0; }  /* 0x00454FF0 */
void __stdcall SND_pc_fn_vSynchroMidiDefault(void)                              { }  /* 0x00403290 */
long __stdcall SND_pc_fn_lPlayTransitionMidiDefault(long a, long b, long c, long d, long e) { (void)a; (void)b; (void)c; (void)d; (void)e; return -1; }  /* 0x00454F80 */
SndBool __stdcall SND_pc_fn_bSetParamTransitionMidiDefault(long a, long b)      { (void)a; (void)b; return 0; }  /* 0x00454FC0 */
SndBool __stdcall SND_pc_fn_bSetNextTransitionMidiDefault(long a, long b)       { (void)a; (void)b; return 0; }  /* 0x00454FC0 */
void __stdcall SND_pc_fn_vStopTransitionMidiDefault(long a)                     { (void)a; }  /* 0x00455CE0 */
void __stdcall SND_pc_fn_vResumeTransitionMidiDefault(long a)                   { (void)a; }  /* 0x00455CE0 */
void __stdcall SND_pc_fn_vPauseTransitionMidiDefault(long a)                    { (void)a; }  /* 0x00455CE0 */
SndBool __stdcall SND_pc_fn_bDoTransitionWithFadeMidiDefault(long a, long b)    { (void)a; (void)b; return 0; }  /* 0x00454FC0 */
SndBool __stdcall SND_pc_fn_bDoTransitionWithFadeMidi2Default(long a, long b, long c) { (void)a; (void)b; (void)c; return 0; }  /* 0x00455230 */
SndBool __stdcall SND_pc_fn_bLoadResScriptCdDefault(long a, long b)             { (void)a; (void)b; return 0; }  /* 0x00454FC0 */
SndBool __stdcall SND_pc_fn_bLoadResBinaryCdDefault(long a, long b, long c)     { (void)a; (void)b; (void)c; return 0; }  /* 0x00455230 */
SndBool __stdcall SND_pc_fn_bSetParamCDDefault(long a, long b)                  { (void)a; (void)b; return 0; }  /* 0x00454FC0 */
SndBool __stdcall SND_pc_fn_bTestInitCDDefault(void)                            { return 0; }  /* 0x00454F30 */
SndBool __stdcall SND_pc_fn_bTestIsPlayingCDDefault(long a)                     { (void)a; return 0; }  /* 0x00454FF0 */
int __stdcall SND_pc_fn_iInitCDDefault(long a)                                  { (void)a; return 0; }  /* 0x00454FF0 */
long __stdcall SND_pc_fn_lPlayCDDefault(long a, long b, long c, long d, long e) { (void)a; (void)b; (void)c; (void)d; (void)e; return -1; }  /* 0x00454F80 */
void __stdcall SND_pc_fn_vDesInitCDDefault(void)                                { }  /* 0x00403290 */
void __stdcall SND_pc_fn_vPauseCDDefault(long a)                                { (void)a; }  /* 0x00455CE0 */
void __stdcall SND_pc_fn_vRemoveCallbackCDDefault(long a)                       { (void)a; }  /* 0x00455CE0 */
void __stdcall SND_pc_fn_vResumeCDDefault(long a)                               { (void)a; }  /* 0x00455CE0 */
void __stdcall SND_pc_fn_vStopCDDefault(long a)                                 { (void)a; }  /* 0x00455CE0 */
void __stdcall SND_pc_fn_vReleaseDriverCDDefault(void)                          { }  /* 0x00403290 */
void __stdcall SND_pc_fn_vRestoreDriverCDDefault(void)                          { }  /* 0x00403290 */
long __stdcall SND_pc_fn_rGetPosCDDefault(long a)                               { (void)a; return 0; }  /* 0x00454FF0 */
long __stdcall SND_pc_fn_rGetLengthCDDefault(long a)                            { (void)a; return 0; }  /* 0x00454FF0 */
void __stdcall SND_pc_fn_vSynchroCDDefault(void)                                { }  /* 0x00403290 */
SndBool __stdcall SND_pc_fn_bCheckVersionResourceCDDefault(long a)              { (void)a; return 1; }  /* 0x00455460 */
SndBool __stdcall SND_pc_fn_bCheckVersionResourceMidiDefault(long a)            { (void)a; return 1; }  /* 0x00455460 */
SndBool __stdcall SND_pc_fn_bSetResourceStaticVolumeCDDefault(long a, long b)   { (void)a; (void)b; return 0; }  /* 0x00454FC0 */
SndBool __stdcall SND_pc_fn_bSetResourceStaticVolumeMidiDefault(long a, long b) { (void)a; (void)b; return 0; }  /* 0x00454FC0 */

void __cdecl SND_pc_fn_vLoadMidiCdProcs(HMODULE hModule)
{
    if (SND_g_lMidiCdProcsLoaded != 0) return;

    SND_M_DbgSlotLoad(g_pfn_SND_LoadResScriptMidi_s, t_SND_LongLongBoolFn, "_SND_fn_bLoadResScriptMidi@8", SND_pc_fn_bLoadResScriptMidiDefault);
    SND_M_DbgSlotLoad(g_pfn_SND_LoadResBinaryMidi_s, t_SND_LoadResFn, "_SND_fn_bLoadResBinaryMidi@12", SND_pc_fn_bLoadResBinaryMidiDefault);
    SND_M_DbgSlotLoad(g_pfn_SND_IsResLoadedMidi_s, t_SND_PerTypeBoolFn, "_SND_fn_bIsResLoadedMidi@4", SND_pc_fn_bIsResLoadedMidiDefault);
    SND_M_DbgSlotLoad(g_pfn_SND_SetParamMidi, t_SND_PerTypeSetParamFn, "_SND_fn_bSetParamMidi@8", SND_pc_fn_bSetParamMidiDefault);
    SND_M_DbgSlotLoad(g_pfn_SND_TestInitMidi_s, t_SND_GetBoolFn, "_SND_fn_bTestInitMidi@0", SND_pc_fn_bTestInitMidiDefault);
    SND_M_DbgSlotLoad(g_pfn_SND_TestIsPlayingMidi, t_SND_PerTypeBoolFn, "_SND_fn_bTestIsPlayingMidi@4", SND_pc_fn_bTestIsPlayingMidiDefault);
    SND_M_DbgSlotLoad(g_pfn_SND_InitMidi_s, t_SND_IntPtrFn, "_SND_fn_iInitMidi@4", SND_pc_fn_iInitMidiDefault);
    SND_M_DbgSlotLoad(SND_M_g_pfnPlayMidi, t_SND_PlayDriverFn, "_SND_fn_lPlayMidi@20", SND_pc_fn_lPlayMidiDefault);
    SND_M_DbgSlotLoad(g_pfn_SND_PlayMidiWithFade_s, t_SND_Play7Fn, "_SND_fn_lPlayMidiWithFade@28", SND_pc_fn_lPlayMidiWithFadeDefault);
    SND_M_DbgSlotLoad(g_pfn_SND_DesInitMidi_s, t_SND_VoidFn, "_SND_fn_vDesInitMidi@0", SND_pc_fn_vDesInitMidiDefault);
    SND_M_DbgSlotLoad(g_pfn_SND_PauseMidi, t_SND_PerTypeFn, "_SND_fn_vPauseMidi@4", SND_pc_fn_vPauseMidiDefault);
    SND_M_DbgSlotLoad(g_pfn_SND_ResumeMidi, t_SND_PerTypeFn, "_SND_fn_vResumeMidi@4", SND_pc_fn_vResumeMidiDefault);
    SND_M_DbgSlotLoad(g_pfn_SND_RemoveCallbackMidi_s, t_SND_PerTypeFn, "_SND_fn_vRemoveCallbackMidi@4", SND_pc_fn_vRemoveCallbackMidiDefault);
    SND_M_DbgSlotLoad(g_pfn_SND_StopMidi, t_SND_PerTypeFn, "_SND_fn_vStopMidi@4", SND_pc_fn_vStopMidiDefault);
    SND_M_DbgSlotLoad(g_pfn_SND_UnLoadResMidi_s, t_SND_UnLoadResFn, "_SND_fn_vUnLoadResMidi@4", SND_pc_fn_vUnLoadResMidiDefault);
    SND_M_DbgSlotLoad(g_pfn_SND_CanFreeDataMidi_s, t_SND_GetBoolFn, "_SND_fn_bCanFreeDataMidi@0", SND_pc_fn_bCanFreeDataMidiDefault);
    SND_M_DbgSlotLoad(g_pfn_SND_ReleaseDriverMidi, t_SND_DriverFn, "_SND_fn_vReleaseDriverMidi@0", SND_pc_fn_vReleaseDriverMidiDefault);
    SND_M_DbgSlotLoad(g_pfn_SND_RestoreDriverMidi, t_SND_DriverFn, "_SND_fn_vRestoreDriverMidi@0", SND_pc_fn_vRestoreDriverMidiDefault);
    SND_M_DbgSlotLoad(g_pfn_SND_GetPosMidi, t_SND_PerTypeRealFn, "_SND_fn_rGetPosMidi@4", SND_pc_fn_rGetPosMidiDefault);
    SND_M_DbgSlotLoad(g_pfn_SND_GetLengthMidi_s, t_SND_PerTypeRealFn, "_SND_fn_rGetLengthMidi@4", SND_pc_fn_rGetLengthMidiDefault);
    SND_M_DbgSlotLoad(g_pfn_SND_SynchroMidi, t_SND_SynchroFn, "_SND_fn_vSynchroMidi@0", SND_pc_fn_vSynchroMidiDefault);
    SND_M_DbgSlotLoad(g_pfn_SND_PlayTransitionMidi_s, t_SND_PlayTransitionFn, "_SND_fn_lPlayTransitionMidi@20", SND_pc_fn_lPlayTransitionMidiDefault);
    SND_M_DbgSlotLoad(g_pfn_SND_SetParamTransitionMidi_s, t_SND_PerTypeSetParamFn, "_SND_fn_bSetParamTransitionMidi@8", SND_pc_fn_bSetParamTransitionMidiDefault);
    SND_M_DbgSlotLoad(g_pfn_SND_SetNextTransitionMidi_s, t_SND_LongLongBoolFn, "_SND_fn_bSetNextTransitionMidi@8", SND_pc_fn_bSetNextTransitionMidiDefault);
    SND_M_DbgSlotLoad(g_pfn_SND_StopTransitionMidi_s, t_SND_PerTypeFn, "_SND_fn_vStopTransitionMidi@4", SND_pc_fn_vStopTransitionMidiDefault);
    SND_M_DbgSlotLoad(g_pfn_SND_ResumeTransitionMidi, t_SND_TransitionFn, "_SND_fn_vResumeTransitionMidi@4", SND_pc_fn_vResumeTransitionMidiDefault);
    SND_M_DbgSlotLoad(g_pfn_SND_PauseTransitionMidi, t_SND_TransitionFn, "_SND_fn_vPauseTransitionMidi@4", SND_pc_fn_vPauseTransitionMidiDefault);
    SND_M_DbgSlotLoad(g_pfn_SND_DoTransitionWithFadeMidi_s, t_SND_LongLongBoolFn, "_SND_fn_bDoTransitionWithFadeMidi@8", SND_pc_fn_bDoTransitionWithFadeMidiDefault);
    SND_M_DbgSlotLoad(g_pfn_SND_DoTransitionWithFadeMidi2_s, t_SND_LongLongRealBoolFn, "_SND_fn_bDoTransitionWithFadeMidi2@12", SND_pc_fn_bDoTransitionWithFadeMidi2Default);
    SND_M_DbgSlotLoad(g_pfn_SND_LoadResScriptCd_s, t_SND_LongLongBoolFn, "_SND_fn_bLoadResScriptCd@8", SND_pc_fn_bLoadResScriptCdDefault);
    SND_M_DbgSlotLoad(g_pfn_SND_LoadResBinaryCd_s, t_SND_LoadResFn, "_SND_fn_bLoadResBinaryCd@12", SND_pc_fn_bLoadResBinaryCdDefault);
    SND_M_DbgSlotLoad(g_pfn_SND_SetParamCD, t_SND_PerTypeSetParamFn, "_SND_fn_bSetParamCD@8", SND_pc_fn_bSetParamCDDefault);
    SND_M_DbgSlotLoad(g_pfn_SND_TestInitCD_s, t_SND_GetBoolFn, "_SND_fn_bTestInitCD@0", SND_pc_fn_bTestInitCDDefault);
    SND_M_DbgSlotLoad(g_pfn_SND_TestIsPlayingCD, t_SND_PerTypeBoolFn, "_SND_fn_bTestIsPlayingCD@4", SND_pc_fn_bTestIsPlayingCDDefault);
    SND_M_DbgSlotLoad(g_pfn_SND_InitCD_s, t_SND_IntPtrFn, "_SND_fn_iInitCD@4", SND_pc_fn_iInitCDDefault);
    SND_M_DbgSlotLoad(SND_M_g_pfnPlayCD, t_SND_PlayDriverFn, "_SND_fn_lPlayCD@20", SND_pc_fn_lPlayCDDefault);
    SND_M_DbgSlotLoad(g_pfn_SND_DesInitCD_s, t_SND_VoidFn, "_SND_fn_vDesInitCD@0", SND_pc_fn_vDesInitCDDefault);
    SND_M_DbgSlotLoad(g_pfn_SND_PauseCD, t_SND_PerTypeFn, "_SND_fn_vPauseCD@4", SND_pc_fn_vPauseCDDefault);
    SND_M_DbgSlotLoad(g_pfn_SND_RemoveCallbackCD_s, t_SND_PerTypeFn, "_SND_fn_vRemoveCallbackCD@4", SND_pc_fn_vRemoveCallbackCDDefault);
    SND_M_DbgSlotLoad(g_pfn_SND_ResumeCD, t_SND_PerTypeFn, "_SND_fn_vResumeCD@4", SND_pc_fn_vResumeCDDefault);
    SND_M_DbgSlotLoad(g_pfn_SND_StopCD, t_SND_PerTypeFn, "_SND_fn_vStopCD@4", SND_pc_fn_vStopCDDefault);
    SND_M_DbgSlotLoad(g_pfn_SND_ReleaseDriverCD, t_SND_DriverFn, "_SND_fn_vReleaseDriverCD@0", SND_pc_fn_vReleaseDriverCDDefault);
    SND_M_DbgSlotLoad(g_pfn_SND_RestoreDriverCD, t_SND_DriverFn, "_SND_fn_vRestoreDriverCD@0", SND_pc_fn_vRestoreDriverCDDefault);
    SND_M_DbgSlotLoad(g_pfn_SND_GetPosCD, t_SND_PerTypeRealFn, "_SND_fn_rGetPosCD@4", SND_pc_fn_rGetPosCDDefault);
    SND_M_DbgSlotLoad(g_pfn_SND_GetLengthCD_s, t_SND_PerTypeRealFn, "_SND_fn_rGetLengthCD@4", SND_pc_fn_rGetLengthCDDefault);
    SND_M_DbgSlotLoad(g_pfn_SND_SynchroCD, t_SND_SynchroFn, "_SND_fn_vSynchroCD@0", SND_pc_fn_vSynchroCDDefault);
    SND_M_DbgSlotLoad(g_pfn_SND_SynchroMidi, t_SND_SynchroFn, "_SND_fn_vSynchroMidi@0", SND_pc_fn_vSynchroMidiDefault);
    SND_M_DbgSlotLoad(g_pfn_SND_CheckVersionResourceCD_s, t_SND_PerTypeBoolFn, "_SND_fn_bCheckVersionResourceCD@4", SND_pc_fn_bCheckVersionResourceCDDefault);
    SND_M_DbgSlotLoad(g_pfn_SND_CheckVersionResourceMidi_s, t_SND_PerTypeBoolFn, "_SND_fn_bCheckVersionResourceMidi@4", SND_pc_fn_bCheckVersionResourceMidiDefault);
    SND_M_DbgSlotLoad(g_pfn_SND_SetResourceStaticVolumeCD_s, t_SND_LongLongBoolFn, "_SND_fn_bSetResourceStaticVolumeCD@8", SND_pc_fn_bSetResourceStaticVolumeCDDefault);
    SND_M_DbgSlotLoad(g_pfn_SND_SetResourceStaticVolumeMidi_s, t_SND_LongLongBoolFn, "_SND_fn_bSetResourceStaticVolumeMidi@8", SND_pc_fn_bSetResourceStaticVolumeMidiDefault);

    SND_g_lMidiCdProcsLoaded = 1;
}

static long SND_g_lSxdProcsLoaded;              /* 0x004B88F8 */

typedef long (__stdcall *t_SND_Play4Fn)(long, long, long, long);
typedef void *(__stdcall *t_SND_PtrPerTypeFn)(long);

static t_SND_PerTypeFn      g_pfn_SND_RemoveCallbackSxd_s;          /* 0x004B8654 */
static t_SND_PerTypeFn      g_pfn_SND_SetSoundVolumeSxd_s;          /* 0x004B866C */
static t_SND_UcharVoidFn    g_pfn_SND_GetSoundVolumeSxd_s;          /* 0x004B8670 */
static t_SND_PerTypeFn      g_pfn_SND_SetReverseStereoSxd_s;        /* 0x004B867C */
static t_SND_GetBoolFn      g_pfn_SND_GetReverseStereoSxd_s;        /* 0x004B8680 */
static t_SND_GetBoolFn      g_pfn_SND_TestInitSxd_s;                /* 0x004B8688 */
static t_SND_Play7Fn        g_pfn_SND_CreateNewBufferSxd_s;         /* 0x004B86A8 */
static t_SND_Void2Fn        g_pfn_SND_SetParamBufferSxd_s;          /* 0x004B86AC */
static t_SND_PerTypeFn      g_pfn_SND_DeleteBufferSxd_s;            /* 0x004B86B0 */
static t_SND_PerTypeFn      g_pfn_SND_PauseBufferSxd_s;             /* 0x004B86B4 */
static t_SND_PerTypeFn      g_pfn_SND_ResumeBufferSxd_s;            /* 0x004B86B8 */
static t_SND_PtrPerTypeFn   g_pfn_SND_GetPtrBufferSxd_s;            /* 0x004B86BC */
static t_SND_Play4Fn        g_pfn_SND_CreateNewBufferExSxd_s;       /* 0x004B86C4 */
static t_SND_PerTypeRealFn  g_pfn_SND_GetPosBufferExSxd_s;          /* 0x004B86C8 */
static t_SND_LongLongBoolFn g_pfn_SND_PushBufferExSxd_s;            /* 0x004B86CC */
static t_SND_Void3Fn        g_pfn_SND_ConvertResDiskToMemSxd_s;     /* 0x004B86D0 */
static t_SND_LongLongBoolFn g_pfn_SND_LoadResScriptSxd_s;           /* 0x004B86D4 */
static t_SND_PerTypeBoolFn  g_pfn_SND_IsResLoadedSxd_s;             /* 0x004B86E0 */
static t_SND_DriverFn       g_pfn_SND_ForceReleaseDriverSxd_s;      /* 0x004B8718 */
static t_SND_DriverFn       g_pfn_SND_ForceRestoreDriverSxd_s;      /* 0x004B871C */
static t_SND_PerTypeRealFn  g_pfn_SND_GetLengthSxd_s;               /* 0x004B86EC */
static t_SND_PerTypeRealFn  g_pfn_SND_GetPosBufferSxd_s;            /* 0x004B86C0 */
static t_SND_Void2Fn        g_pfn_SND_GetTargetLabelSxd_s;          /* 0x004B8720 */
static t_SND_VoidFn         g_pfn_SND_SetupTargetSxd_s;             /* 0x004B8724 */
static t_SND_GetBoolFn      g_pfn_SND_CanSetupTargetSxd_s;          /* 0x004B8728 */
static t_SND_Void2Fn        g_pfn_SND_Param3Dto2D_s;                /* 0x004B872C */
static t_SND_PerTypeBoolFn  g_pfn_SND_CheckVersionResourceSxd_s;    /* 0x004B874C */
static t_SND_Void2Fn        g_pfn_SND_SetNbVoiceWishedSxd_s;        /* 0x004B8750 */
static t_SND_GetBoolFn      g_pfn_SND_GetNbVoiceOptimumSxd_s;       /* 0x004B8758 */
static t_SND_GetBoolFn      g_pfn_SND_GetNbVoiceWishedSxd_s;        /* 0x004B8754 */
static t_SND_GetBoolFn      g_pfn_SND_GetMainFormatSxd_s;           /* 0x004B875C */

long __stdcall SND_pc_fn_lPlaySxdDefault(long a, long b, long c, long d, long e) { (void)a; (void)b; (void)c; (void)d; (void)e; return -1; }  /* 0x00454F80 */
void __stdcall SND_pc_fn_vRemoveCallbackSxdDefault(long a)                      { (void)a; }  /* 0x00455CE0 */
SndBool __stdcall SND_pc_fn_bSetParamSxdDefault(long a, long b)                 { (void)a; (void)b; return 0; }  /* 0x00454FC0 */
SndBool __stdcall SND_pc_fn_bTestIsPlayingSxdDefault(long a)                    { (void)a; return 0; }  /* 0x00454FF0 */
void __stdcall SND_pc_fn_vStopSxdDefault(long a)                                { (void)a; }  /* 0x00455CE0 */
void __stdcall SND_pc_fn_vPauseSxdDefault(long a)                               { (void)a; }  /* 0x00455CE0 */
void __stdcall SND_pc_fn_vResumeSxdDefault(long a)                              { (void)a; }  /* 0x00455CE0 */
void __stdcall SND_pc_fn_vSetSoundVolumeSxdDefault(long a)                      { (void)a; }  /* 0x00455CE0 */
unsigned char __stdcall SND_pc_fn_ucGetSoundVolumeSxdDefault(void)              { return 0; }  /* 0x00455060 */
void __stdcall SND_pc_fn_vSetStereoSxdDefault(long a)                           { (void)a; }  /* 0x00455CE0 */
SndBool __stdcall SND_pc_fn_bGetStereoSxdDefault(void)                          { return 0; }  /* 0x00454F30 */
void __stdcall SND_pc_fn_vSetReverseStereoSxdDefault(long a)                    { (void)a; }  /* 0x00455CE0 */
SndBool __stdcall SND_pc_fn_bGetReverseStereoSxdDefault(void)                   { return 0; }  /* 0x00454F30 */
int __stdcall SND_pc_fn_iInitSxdDefault(long a)                                 { (void)a; return 0; }  /* 0x00454FF0 */
SndBool __stdcall SND_pc_fn_bTestInitSxdDefault(void)                           { return 0; }  /* 0x00454F30 */
void __stdcall SND_pc_fn_vDesInitSxdDefault(void)                               { }  /* 0x00403290 */
void __stdcall SND_pc_fn_vSetDopplerFactorSxdDefault(long a)                    { (void)a; }  /* 0x00455CE0 */
void __stdcall SND_pc_fn_vSetEffectSxdDefault(long a)                           { (void)a; }  /* 0x00455CE0 */
long __stdcall SND_pc_fn_lCreateMicroSxdDefault(long a)                         { (void)a; return 0; }  /* 0x00454FF0 */
void __stdcall SND_pc_fn_vSetMicroParamSxdDefault(long a, long b)               { (void)a; (void)b; }  /* 0x00455180 */
void __stdcall SND_pc_fn_vDestroyMicroSxdDefault(long a)                        { (void)a; }  /* 0x00455CE0 */
long __stdcall SND_pc_fn_lCreateNewBufferSxdDefault(long a, long b, long c, long d, long e, long f, long g) { (void)a; (void)b; (void)c; (void)d; (void)e; (void)f; (void)g; return -1; }  /* 0x004551D0 */
void __stdcall SND_pc_fn_vSetParamBufferSxdDefault(long a, long b)              { (void)a; (void)b; }  /* 0x00455180 */
void __stdcall SND_pc_fn_vDeleteBufferSxdDefault(long a)                        { (void)a; }  /* 0x00455CE0 */
void __stdcall SND_pc_fn_vPauseBufferSxdDefault(long a)                         { (void)a; }  /* 0x00455CE0 */
void __stdcall SND_pc_fn_vResumeBufferSxdDefault(long a)                        { (void)a; }  /* 0x00455CE0 */
void * __stdcall SND_pc_fn_pvGetPtrBufferSxdDefault(long a)                     { (void)a; return 0; }  /* 0x00454FF0 */
long __stdcall SND_pc_fn_lCreateNewBufferExSxdDefault(long a, long b, long c, long d) { (void)a; (void)b; (void)c; (void)d; return -1; }  /* 0x004551E0 */
long __stdcall SND_pc_fn_lGetPosBufferExSxdDefault(long a)                      { (void)a; return 0; }  /* 0x00454FF0 */
long __stdcall SND_pc_fn_lPushBufferExSxdDefault(long a, long b)                { (void)a; (void)b; return 0; }  /* 0x00454FC0 */
void __stdcall SND_pc_fn_vConvertResDiskToMemSxdDefault(long a, long b, long c) { (void)a; (void)b; (void)c; }  /* 0x004551F0 */
SndBool __stdcall SND_pc_fn_bLoadResScriptSxdDefault(long a, long b)            { (void)a; (void)b; return 0; }  /* 0x00454FC0 */
SndBool __stdcall SND_pc_fn_bLoadResBinarySxdDefault(long a, long b, long c)    { (void)a; (void)b; (void)c; return 0; }  /* 0x00455230 */
void __stdcall SND_pc_fn_vUnLoadResSxdDefault(long a)                           { (void)a; }  /* 0x00455CE0 */
SndBool __stdcall SND_pc_fn_bIsResLoadedSxdDefault(long a)                      { (void)a; return 0; }  /* 0x00454FF0 */
long __stdcall SND_pc_fn_lPlayTransitionExSxdDefault(long a, long b, long c, long d, long e) { (void)a; (void)b; (void)c; (void)d; (void)e; return -1; }  /* 0x00454F80 */
SndBool __stdcall SND_pc_fn_bSetParamTransitionSxdDefault(long a, long b)       { (void)a; (void)b; return 0; }  /* 0x00454FC0 */
SndBool __stdcall SND_pc_fn_bSetNextTransitionSxdDefault(long a, long b)        { (void)a; (void)b; return 0; }  /* 0x00454FC0 */
SndBool __stdcall SND_pc_fn_bDoTransitionWithFadeSxdDefault(long a, long b)     { (void)a; (void)b; return 0; }  /* 0x00454FC0 */
SndBool __stdcall SND_pc_fn_bDoTransitionWithFadeSxd2Default(long a, long b, long c) { (void)a; (void)b; (void)c; return 0; }  /* 0x00455230 */
void __stdcall SND_pc_fn_vStopTransitionSxdDefault(long a)                      { (void)a; }  /* 0x00455CE0 */
void __stdcall SND_pc_fn_vPauseTransitionSxdDefault(long a)                     { (void)a; }  /* 0x00455CE0 */
void __stdcall SND_pc_fn_vResumeTransitionSxdDefault(long a)                    { (void)a; }  /* 0x00455CE0 */
SndBool __stdcall SND_pc_fn_bCanFreeDataSampleDefault(void)                     { return 0; }  /* 0x00454F30 */
void __stdcall SND_pc_fn_vReleaseDriverSxdDefault(void)                         { }  /* 0x00403290 */
void __stdcall SND_pc_fn_vRestoreDriverSxdDefault(void)                         { }  /* 0x00403290 */
void __stdcall SND_pc_fn_vForceReleaseDriverSxdDefault(void)                    { }  /* 0x00403290 */
void __stdcall SND_pc_fn_vForceRestoreDriverSxdDefault(void)                    { }  /* 0x00403290 */
long __stdcall SND_pc_fn_rGetPosSxdDefault(long a)                              { (void)a; return 0; }  /* 0x00454FF0 */
long __stdcall SND_pc_fn_rGetLengthSxdDefault(long a)                           { (void)a; return 0; }  /* 0x00454FF0 */
long __stdcall SND_pc_fn_rGetPosBufferSxdDefault(long a)                        { (void)a; return 0; }  /* 0x00454FF0 */
void __stdcall SND_pc_fn_vGetTargetLabelSxdDefault(long a, long b)              { (void)a; (void)b; }  /* 0x00455180 */
void __stdcall SND_pc_fn_vSetupTargetSxdDefault(void)                           { }  /* 0x00403290 */
SndBool __stdcall SND_pc_fn_bCanSetupTargetSxdDefault(void)                     { return 0; }  /* 0x00454F30 */
void __stdcall SND_pc_fn_vSynchroSxdDefault(void)                               { }  /* 0x00403290 */
void __stdcall SND_pc_fn_vParam3Dto2DDefault(long a, long b)                    { (void)a; (void)b; }  /* 0x00455180 */
void __stdcall SND_pc_fn_vSetDefaultRollOffSxdDefault(long a)                   { (void)a; }  /* 0x00455CE0 */
SndBool __stdcall SND_pc_fn_bCheckVersionResourceSxdDefault(long a)             { (void)a; return 1; }  /* 0x00455460 */
void __stdcall SND_pc_fn_vSetNbVoiceWishedSxdDefault(long a, long b)            { (void)a; (void)b; }  /* 0x00455180 */
long __stdcall SND_pc_fn_lGetNbVoiceOptimumSxdDefault(void)                     { return 0; }  /* 0x00454F30 */
long __stdcall SND_pc_fn_lGetNbVoiceWishedSxdDefault(void)                      { return 0; }  /* 0x00454F30 */
long __stdcall SND_pc_fn_eGetMainFormatSxdDefault(void)                         { return 0; }  /* 0x00454F30 */
SndBool __stdcall SND_pc_fn_bSetResourceStaticVolumeSxdDefault(long a, long b)  { (void)a; (void)b; return 0; }  /* 0x00454FC0 */

void __cdecl SND_pc_fn_vLoadSxdProcs(HMODULE hModule)
{
    if (SND_g_lSxdProcsLoaded != 0) return;

    SND_M_DbgSlotLoad(SND_M_g_pfnPlaySxd, t_SND_PlayDriverFn, "_SND_fn_lPlaySxd@20", SND_pc_fn_lPlaySxdDefault);
    SND_M_DbgSlotLoad(g_pfn_SND_RemoveCallbackSxd_s, t_SND_PerTypeFn, "_SND_fn_vRemoveCallbackSxd@4", SND_pc_fn_vRemoveCallbackSxdDefault);
    SND_M_DbgSlotLoad(g_pfn_SND_SetParamSxd, t_SND_PerTypeSetParamFn, "_SND_fn_bSetParamSxd@8", SND_pc_fn_bSetParamSxdDefault);
    SND_M_DbgSlotLoad(g_pfn_SND_TestIsPlayingSxd, t_SND_PerTypeBoolFn, "_SND_fn_bTestIsPlayingSxd@4", SND_pc_fn_bTestIsPlayingSxdDefault);
    SND_M_DbgSlotLoad(g_pfn_SND_StopSxd, t_SND_PerTypeFn, "_SND_fn_vStopSxd@4", SND_pc_fn_vStopSxdDefault);
    SND_M_DbgSlotLoad(g_pfn_SND_PauseSxd, t_SND_PerTypeFn, "_SND_fn_vPauseSxd@4", SND_pc_fn_vPauseSxdDefault);
    SND_M_DbgSlotLoad(g_pfn_SND_ResumeSxd, t_SND_PerTypeFn, "_SND_fn_vResumeSxd@4", SND_pc_fn_vResumeSxdDefault);
    SND_M_DbgSlotLoad(g_pfn_SND_SetSoundVolumeSxd_s, t_SND_PerTypeFn, "_SND_fn_vSetSoundVolumeSxd@4", SND_pc_fn_vSetSoundVolumeSxdDefault);
    SND_M_DbgSlotLoad(g_pfn_SND_GetSoundVolumeSxd_s, t_SND_UcharVoidFn, "_SND_fn_ucGetSoundVolumeSxd@0", SND_pc_fn_ucGetSoundVolumeSxdDefault);
    SND_M_DbgSlotLoad(g_pfn_SND_SetStereoSxd_s, t_SND_PerTypeFn, "_SND_fn_vSetStereoSxd@4", SND_pc_fn_vSetStereoSxdDefault);
    SND_M_DbgSlotLoad(g_pfn_SND_GetStereoSxd_s, t_SND_GetBoolFn, "_SND_fn_bGetStereoSxd@0", SND_pc_fn_bGetStereoSxdDefault);
    SND_M_DbgSlotLoad(g_pfn_SND_SetReverseStereoSxd_s, t_SND_PerTypeFn, "_SND_fn_vSetReverseStereoSxd@4", SND_pc_fn_vSetReverseStereoSxdDefault);
    SND_M_DbgSlotLoad(g_pfn_SND_GetReverseStereoSxd_s, t_SND_GetBoolFn, "_SND_fn_bGetReverseStereoSxd@0", SND_pc_fn_bGetReverseStereoSxdDefault);
    SND_M_DbgSlotLoad(g_pfn_SND_InitSxd_s, t_SND_IntPtrFn, "_SND_fn_iInitSxd@4", SND_pc_fn_iInitSxdDefault);
    SND_M_DbgSlotLoad(g_pfn_SND_TestInitSxd_s, t_SND_GetBoolFn, "_SND_fn_bTestInitSxd@0", SND_pc_fn_bTestInitSxdDefault);
    SND_M_DbgSlotLoad(g_pfn_SND_DesInitSxd_s, t_SND_VoidFn, "_SND_fn_vDesInitSxd@0", SND_pc_fn_vDesInitSxdDefault);
    SND_M_DbgSlotLoad(g_pfn_SND_SetDopplerFactorSxd, t_SND_fn_vSetDopplerFactorSxd, "_SND_fn_vSetDopplerFactorSxd@4", SND_pc_fn_vSetDopplerFactorSxdDefault);
    SND_M_DbgSlotLoad(g_pfn_SND_SetEffectSxd, t_SND_PerTypeFn, "_SND_fn_vSetEffectSxd@4", SND_pc_fn_vSetEffectSxdDefault);
    SND_M_DbgSlotLoad(g_pfn_SND_CreateMicroSxd_s, t_SND_LongPtrFn, "_SND_fn_lCreateMicroSxd@4", SND_pc_fn_lCreateMicroSxdDefault);
    SND_M_DbgSlotLoad(g_pfn_SND_SetMicroParamSxd, t_SND_fn_vSetMicroParamSxd, "_SND_fn_vSetMicroParamSxd@8", SND_pc_fn_vSetMicroParamSxdDefault);
    SND_M_DbgSlotLoad(g_pfn_SND_DestroyMicroSxd_s, t_SND_PerTypeFn, "_SND_fn_vDestroyMicroSxd@4", SND_pc_fn_vDestroyMicroSxdDefault);
    SND_M_DbgSlotLoad(g_pfn_SND_CreateNewBufferSxd_s, t_SND_Play7Fn, "_SND_fn_lCreateNewBufferSxd@28", SND_pc_fn_lCreateNewBufferSxdDefault);
    SND_M_DbgSlotLoad(g_pfn_SND_SetParamBufferSxd_s, t_SND_Void2Fn, "_SND_fn_vSetParamBufferSxd@8", SND_pc_fn_vSetParamBufferSxdDefault);
    SND_M_DbgSlotLoad(g_pfn_SND_DeleteBufferSxd_s, t_SND_PerTypeFn, "_SND_fn_vDeleteBufferSxd@4", SND_pc_fn_vDeleteBufferSxdDefault);
    SND_M_DbgSlotLoad(g_pfn_SND_PauseBufferSxd_s, t_SND_PerTypeFn, "_SND_fn_vPauseBufferSxd@4", SND_pc_fn_vPauseBufferSxdDefault);
    SND_M_DbgSlotLoad(g_pfn_SND_ResumeBufferSxd_s, t_SND_PerTypeFn, "_SND_fn_vResumeBufferSxd@4", SND_pc_fn_vResumeBufferSxdDefault);
    SND_M_DbgSlotLoad(g_pfn_SND_GetPtrBufferSxd_s, t_SND_PtrPerTypeFn, "_SND_fn_pvGetPtrBufferSxd@4", SND_pc_fn_pvGetPtrBufferSxdDefault);
    SND_M_DbgSlotLoad(g_pfn_SND_CreateNewBufferExSxd_s, t_SND_Play4Fn, "_SND_fn_lCreateNewBufferExSxd@16", SND_pc_fn_lCreateNewBufferExSxdDefault);
    SND_M_DbgSlotLoad(g_pfn_SND_GetPosBufferExSxd_s, t_SND_PerTypeRealFn, "_SND_fn_lGetPosBufferExSxd@4", SND_pc_fn_lGetPosBufferExSxdDefault);
    SND_M_DbgSlotLoad(g_pfn_SND_PushBufferExSxd_s, t_SND_LongLongBoolFn, "_SND_fn_lPushBufferExSxd@8", SND_pc_fn_lPushBufferExSxdDefault);
    SND_M_DbgSlotLoad(g_pfn_SND_ConvertResDiskToMemSxd_s, t_SND_Void3Fn, "_SND_fn_vConvertResDiskToMemSxd@12", SND_pc_fn_vConvertResDiskToMemSxdDefault);
    SND_M_DbgSlotLoad(g_pfn_SND_LoadResScriptSxd_s, t_SND_LongLongBoolFn, "_SND_fn_bLoadResScriptSxd@8", SND_pc_fn_bLoadResScriptSxdDefault);
    SND_M_DbgSlotLoad(g_pfn_SND_LoadResBinarySxd_s, t_SND_LoadResFn, "_SND_fn_bLoadResBinarySxd@12", SND_pc_fn_bLoadResBinarySxdDefault);
    SND_M_DbgSlotLoad(g_pfn_SND_UnLoadResSxd_s, t_SND_UnLoadResFn, "_SND_fn_vUnLoadResSxd@4", SND_pc_fn_vUnLoadResSxdDefault);
    SND_M_DbgSlotLoad(g_pfn_SND_IsResLoadedSxd_s, t_SND_PerTypeBoolFn, "_SND_fn_bIsResLoadedSxd@4", SND_pc_fn_bIsResLoadedSxdDefault);
    SND_M_DbgSlotLoad(g_pfn_SND_PlayTransitionExSxd_s, t_SND_PlayTransitionFn, "_SND_fn_lPlayTransitionExSxd@20", SND_pc_fn_lPlayTransitionExSxdDefault);
    SND_M_DbgSlotLoad(g_pfn_SND_SetParamTransitionSxd_s, t_SND_PerTypeSetParamFn, "_SND_fn_bSetParamTransitionSxd@8", SND_pc_fn_bSetParamTransitionSxdDefault);
    SND_M_DbgSlotLoad(g_pfn_SND_SetNextTransitionSxd_s, t_SND_LongLongBoolFn, "_SND_fn_bSetNextTransitionSxd@8", SND_pc_fn_bSetNextTransitionSxdDefault);
    SND_M_DbgSlotLoad(g_pfn_SND_DoTransitionWithFadeSxd_s, t_SND_LongLongBoolFn, "_SND_fn_bDoTransitionWithFadeSxd@8", SND_pc_fn_bDoTransitionWithFadeSxdDefault);
    SND_M_DbgSlotLoad(g_pfn_SND_DoTransitionWithFadeSxd2_s, t_SND_LongLongRealBoolFn, "_SND_fn_bDoTransitionWithFadeSxd2@12", SND_pc_fn_bDoTransitionWithFadeSxd2Default);
    SND_M_DbgSlotLoad(g_pfn_SND_StopTransitionSxd_s, t_SND_PerTypeFn, "_SND_fn_vStopTransitionSxd@4", SND_pc_fn_vStopTransitionSxdDefault);
    SND_M_DbgSlotLoad(g_pfn_SND_PauseTransitionSxd, t_SND_TransitionFn, "_SND_fn_vPauseTransitionSxd@4", SND_pc_fn_vPauseTransitionSxdDefault);
    SND_M_DbgSlotLoad(g_pfn_SND_ResumeTransitionSxd, t_SND_TransitionFn, "_SND_fn_vResumeTransitionSxd@4", SND_pc_fn_vResumeTransitionSxdDefault);
    SND_M_DbgSlotLoad(g_pfn_SND_CanFreeDataSample_s, t_SND_GetBoolFn, "_SND_fn_bCanFreeDataSample@0", SND_pc_fn_bCanFreeDataSampleDefault);
    SND_M_DbgSlotLoad(g_pfn_SND_ReleaseDriverSxd, t_SND_DriverFn, "_SND_fn_vReleaseDriverSxd@0", SND_pc_fn_vReleaseDriverSxdDefault);
    SND_M_DbgSlotLoad(g_pfn_SND_RestoreDriverSxd, t_SND_DriverFn, "_SND_fn_vRestoreDriverSxd@0", SND_pc_fn_vRestoreDriverSxdDefault);
    SND_M_DbgSlotLoad(g_pfn_SND_ForceReleaseDriverSxd_s, t_SND_DriverFn, "_SND_fn_vForceReleaseDriverSxd@0", SND_pc_fn_vForceReleaseDriverSxdDefault);
    SND_M_DbgSlotLoad(g_pfn_SND_ForceRestoreDriverSxd_s, t_SND_DriverFn, "_SND_fn_vForceRestoreDriverSxd@0", SND_pc_fn_vForceRestoreDriverSxdDefault);
    SND_M_DbgSlotLoad(g_pfn_SND_GetPosSxd, t_SND_PerTypeRealFn, "_SND_fn_rGetPosSxd@4", SND_pc_fn_rGetPosSxdDefault);
    SND_M_DbgSlotLoad(g_pfn_SND_GetLengthSxd_s, t_SND_PerTypeRealFn, "_SND_fn_rGetLengthSxd@4", SND_pc_fn_rGetLengthSxdDefault);
    SND_M_DbgSlotLoad(g_pfn_SND_GetPosBufferSxd_s, t_SND_PerTypeRealFn, "_SND_fn_rGetPosBufferSxd@4", SND_pc_fn_rGetPosBufferSxdDefault);
    SND_M_DbgSlotLoad(g_pfn_SND_GetTargetLabelSxd_s, t_SND_Void2Fn, "_SND_fn_vGetTargetLabelSxd@8", SND_pc_fn_vGetTargetLabelSxdDefault);
    SND_M_DbgSlotLoad(g_pfn_SND_SetupTargetSxd_s, t_SND_VoidFn, "_SND_fn_vSetupTargetSxd@0", SND_pc_fn_vSetupTargetSxdDefault);
    SND_M_DbgSlotLoad(g_pfn_SND_CanSetupTargetSxd_s, t_SND_GetBoolFn, "_SND_fn_bCanSetupTargetSxd@0", SND_pc_fn_bCanSetupTargetSxdDefault);
    SND_M_DbgSlotLoad(g_pfn_SND_SynchroSxd, t_SND_SynchroFn, "_SND_fn_vSynchroSxd@0", SND_pc_fn_vSynchroSxdDefault);
    SND_M_DbgSlotLoad(g_pfn_SND_Param3Dto2D_s, t_SND_Void2Fn, "_SND_fn_vParam3Dto2D@8", SND_pc_fn_vParam3Dto2DDefault);
    SND_M_DbgSlotLoad(g_pfn_SND_SetDefaultRollOffSxd_s, t_SND_RollOffFn, "_SND_fn_vSetDefaultRollOffSxd@4", SND_pc_fn_vSetDefaultRollOffSxdDefault);
    SND_M_DbgSlotLoad(g_pfn_SND_CheckVersionResourceSxd_s, t_SND_PerTypeBoolFn, "_SND_fn_bCheckVersionResourceSxd@4", SND_pc_fn_bCheckVersionResourceSxdDefault);
    SND_M_DbgSlotLoad(g_pfn_SND_SetNbVoiceWishedSxd_s, t_SND_Void2Fn, "_SND_fn_vSetNbVoiceWishedSxd@8", SND_pc_fn_vSetNbVoiceWishedSxdDefault);
    SND_M_DbgSlotLoad(g_pfn_SND_GetNbVoiceOptimumSxd_s, t_SND_GetBoolFn, "_SND_fn_lGetNbVoiceOptimumSxd@0", SND_pc_fn_lGetNbVoiceOptimumSxdDefault);
    SND_M_DbgSlotLoad(g_pfn_SND_GetNbVoiceWishedSxd_s, t_SND_GetBoolFn, "_SND_fn_lGetNbVoiceWishedSxd@0", SND_pc_fn_lGetNbVoiceWishedSxdDefault);
    SND_M_DbgSlotLoad(g_pfn_SND_GetMainFormatSxd_s, t_SND_GetBoolFn, "_SND_fn_eGetMainFormatSxd@0", SND_pc_fn_eGetMainFormatSxdDefault);
    SND_M_DbgSlotLoad(g_pfn_SND_SetResourceStaticVolumeSxd_s, t_SND_LongLongBoolFn, "_SND_fn_bSetResourceStaticVolumeSxd@8", SND_pc_fn_bSetResourceStaticVolumeSxdDefault);

    SND_g_lSxdProcsLoaded = 1;
}
#endif

#if defined(_DLL_COMPILATION_MODE)

HMODULE SND_pc_hModuleSxd;               /* 0x004B88D8 */
HMODULE SND_pc_hModuleCd;                /* 0x004B88DC */
HMODULE SND_pc_hModuleMpeg;              /* 0x004B88E0 */
HMODULE SND_pc_hModuleDbg;               /* 0x004B88E4 */
HMODULE SND_pc_hModuleAdpcm;             /* 0x004B88E8 */
HMODULE SND_pc_hModuleRecord;            /* 0x004B88EC */
#endif
static long SND_pc_lMpegOn;                     /* 0x004B94A0 */
static long SND_pc_lCdOn;                       /* 0x004B94A4 */
static long SND_pc_lWavOn;                      /* 0x004B94A8 */
static long SND_pc_lAdpcmOn;                    /* 0x004B94AC */
static long SND_pc_lRecordOn;                   /* 0x004B94B0 */
static long SND_pc_lSupervisaterOn;             /* 0x004B94B4 */
static long SND_pc_lSoundOnOff;                 /* 0x004A3F18 */

void __cdecl SND_pc_fn_vLoadDriversFromIni(long *plAppInfo)
{
    char szValue[0x100];
#if defined(_DLL_COMPILATION_MODE)
    char szKey[0x100];
    char szPath[0x100];
    HMODULE hModules[10];
#endif
    long lDistances[2];
#if defined(_DLL_COMPILATION_MODE)
    int iCount;
    int iIndex;
    int bContinue;
#endif

    strncpy(SND_pc_szDllDir, SND_fn_szGetSoftDirectory(), 0xff);
    strncpy(SND_pc_szCpaIniPath, SND_pc_szDllDir, 0xff);
    strncat(SND_pc_szCpaIniPath, "\\", 0xff - strlen(SND_pc_szCpaIniPath));
    strncat(SND_pc_szCpaIniPath, "SND_CPA.INI", 0xff - strlen(SND_pc_szCpaIniPath));

    if (SND_fn_bTestSnd_Win32() == 0)
    {
        SND_fn_vDisplayError(4, "Windows needed");
    }

    FIL_pc_M_ulGetProfileString(NULL, NULL, "None", szValue, 0xff, SND_pc_szCpaIniPath);
    if (szValue[0] == '\0' || strcmp(szValue, "None") == 0)
    {
        sprintf(szValue, "The INI file doesn't exist (%s)", SND_pc_szCpaIniPath);
        SND_fn_vDisplayError(0xc, szValue);
    }

    FIL_pc_M_ulGetProfileString("Options", "Sound_ON_OFF", "None", szValue, 0x100, SND_pc_szCpaIniPath);
    if (strcmp(szValue, "OFF") == 0)
    {
        SND_pc_lSoundOnOff = 0;
        SND_pc_lAdpcmOn = 0;
        SND_pc_lMpegOn = 0;
        SND_pc_lCdOn = 0;
        SND_pc_lWavOn = 0;
        SND_pc_lRecordOn = 0;
        SND_pc_lSupervisaterOn = 0;
    }
    else
    {
        FIL_pc_M_ulGetProfileString("Options", "MPEG_ON_OFF", "None", szValue, 0x100, SND_pc_szCpaIniPath);
        if (strcmp(szValue, "ON") == 0)
        {
            SND_pc_lMpegOn = 1;
        }
        FIL_pc_M_ulGetProfileString("Options", "CD_ON_OFF", "None", szValue, 0x100, SND_pc_szCpaIniPath);
        if (strcmp(szValue, "ON") == 0)
        {
            SND_pc_lCdOn = 1;
        }
        FIL_pc_M_ulGetProfileString("Options", "WAV_ON_OFF", "None", szValue, 0x100, SND_pc_szCpaIniPath);
        if (strcmp(szValue, "ON") == 0)
        {
            SND_pc_lWavOn = 1;
        }
        FIL_pc_M_ulGetProfileString("Options", "ADPCM_ON_OFF", "None", szValue, 0x100, SND_pc_szCpaIniPath);
        if (strcmp(szValue, "ON") == 0)
        {
            SND_pc_lAdpcmOn = 1;
        }
        FIL_pc_M_ulGetProfileString("Options", "RECORD_ON_OFF", "None", szValue, 0x100, SND_pc_szCpaIniPath);
        if (strcmp(szValue, "ON") == 0)
        {
            SND_pc_lRecordOn = 1;
        }
    }

#if defined(_DLL_COMPILATION_MODE)
    hModules[0] = (HMODULE)plAppInfo[0];
    bContinue = 1;
    iCount = 1;
    /* 0x00458CB0 */
    while (bContinue && iCount < 10)
    {
        sprintf(szKey, "File%x", iCount - 1);
        FIL_pc_M_ulGetProfileString("DLL_Test", szKey, "None", szValue, 0x100, SND_pc_szCpaIniPath);
        if (strcmp(szValue, "None") != 0)
        {
            sprintf(szPath, "%s\\%s%c%c%c.dll", SND_pc_szDllDir, szValue, 'b', 'v', 'r');
            hModules[iCount] = LoadLibraryA(szPath);
            if (hModules[iCount] == NULL)
            {
                SND_fn_vDisplayError(0xf, szPath);
            }
            iCount++;
        }
        else
        {
            bContinue = 0;
        }
    }

    if (SND_pc_lWavOn) SND_pc_hModuleSxd = SND_pc_fn_hLoadDriverDll("DLL_Sxd", hModules, iCount);
    else SND_pc_hModuleSxd = NULL;

    if (SND_pc_lCdOn) SND_pc_hModuleCd = SND_pc_fn_hLoadDriverDll("DLL_Cd", hModules, iCount);
    else SND_pc_hModuleCd = NULL;

    if (SND_pc_lMpegOn) SND_pc_hModuleMpeg = SND_pc_fn_hLoadDriverDll("DLL_MPEG", hModules, iCount);
    else SND_pc_hModuleMpeg = NULL;

    if (SND_pc_lAdpcmOn) SND_pc_hModuleAdpcm = SND_pc_fn_hLoadDriverDll("DLL_Adpcm", hModules, iCount);
    else SND_pc_hModuleAdpcm = NULL;

    if (SND_pc_lRecordOn) SND_pc_hModuleRecord = SND_pc_fn_hLoadDriverDll("DLL_Record", hModules, iCount);
    else SND_pc_hModuleRecord = NULL;

    SND_pc_fn_vLoadSxdProcs(SND_pc_hModuleSxd);
    SND_pc_fn_vLoadMidiCdProcs(SND_pc_hModuleCd);
    SND_pc_fn_vLoadCanFreeDataMPEG(SND_pc_hModuleMpeg);
    SND_pc_fn_vLoadCanFreeDataADPCM(SND_pc_hModuleAdpcm);
    SND_pc_fn_vLoadRecordProcs(SND_pc_hModuleRecord);

    if (iCount > 1)
    {
        for (iIndex = 1; iIndex < iCount; iIndex = iIndex + 1)
        {
            FreeLibrary(hModules[iIndex]);
        }
    }
#endif

    SND_pc_fn_vLoadSxdDistances(lDistances);
    SND_fn_vSetDefaultRollOff((RollOffParam *)lDistances);

    FIL_pc_M_ulGetProfileString("Options", "SUPERVISATER_ON_OFF", "None", szValue, 0x100, SND_pc_szCpaIniPath);
    if (strcmp(szValue, "ON") == 0)
    {
        SND_pc_lSupervisaterOn = 1;
    }
#if defined(_DLL_COMPILATION_MODE)
    if (SND_pc_lSupervisaterOn)
    {
        strncpy(szPath, SND_pc_szDllDir, 0x100);
        strncat(szPath, "\\SNDdbgV.DLL", 0xff - strlen(szPath));
        SND_pc_hModuleDbg = LoadLibraryA(szPath);
    }
    else
    {
        SND_pc_hModuleDbg = NULL;
    }
    SND_pc_fn_vLoadDbgProcs(SND_pc_hModuleDbg);
    if (SND_pc_lSupervisaterOn != 0)
    {
        SND_fn_vSetSupervisaterActive();
    }
#endif
}

void __cdecl SND_fn_vDestroyMicroSnd(long sndId);

void __cdecl SND_fn_vDestroyMicroSound(long Id)
{
        SND_RT_fn_vEnterCriticalSectionThreadSnd();
        SND_RT_fn_vMouchardThreadsnd(SND_C_szSound3DSrcFile, 0x98d);

        g_iNbActiveMicros--;

        SNDLST2_M_DynamicIsolate((pMICRO_DESCRIPTION)Id);
        SND_fn_vDestroyMicroSnd(((pMICRO_DESCRIPTION)Id)->sndId);
        SND_fn_vFreeSndEx(E_ucSndBlockMain,(pMICRO_DESCRIPTION)Id);

#if defined(_DLL_COMPILATION_MODE) || defined(SUPERVISATER)
        SND_fn_vKillMicro(Id);
#endif
        SND_RT_fn_vQuitCriticalSectionThreadSnd();
}

static void init_association(void)
{
        SNDLST2_M_DynamicInitAnchor(&association.pList);
}

static void init_object_type(void)
{
    nb_object_type = 0;
    SND_fn_lAddObjectTypeSound2((void *)SND_M_g_astTypeObjectSoundInfosDefault, (void *)0);
}

SndReal fn_rRetMicroSoundSndReal0Default(long indice)
{
    (void)indice;
    return 0;
}

static long fn_rRetMicroSoundSndReal1Default(long indice)
{
    (void)indice;
    return 0x10000;
}

long fn_lRetMicroSoundFILTREDefault(long indice)
{
    (void)indice;
    return 0;
}

static void init_micro_fct(void)
{
        SNDLST2_M_DynamicInitAnchor(&micros_sound.pList);
        g_iNbActiveMicros=0;

        g_st_pfnRetMicroSound.X=fn_rRetMicroSoundSndReal0Default;
        g_st_pfnRetMicroSound.Y=fn_rRetMicroSoundSndReal0Default;
        g_st_pfnRetMicroSound.Z=fn_rRetMicroSoundSndReal0Default;
        g_st_pfnRetMicroSound.VX=fn_rRetMicroSoundSndReal0Default;
        g_st_pfnRetMicroSound.VY=fn_rRetMicroSoundSndReal0Default;
        g_st_pfnRetMicroSound.VZ=fn_rRetMicroSoundSndReal0Default;
        g_st_pfnRetMicroSound.DTX=fn_rRetMicroSoundSndReal1Default;
        g_st_pfnRetMicroSound.DTY=fn_rRetMicroSoundSndReal0Default;
        g_st_pfnRetMicroSound.DTZ=fn_rRetMicroSoundSndReal0Default;
        g_st_pfnRetMicroSound.DNX=fn_rRetMicroSoundSndReal0Default;
        g_st_pfnRetMicroSound.DNY=fn_rRetMicroSoundSndReal0Default;
        g_st_pfnRetMicroSound.DNZ=fn_rRetMicroSoundSndReal1Default;
        g_st_pfnRetMicroSound.Filtre=fn_lRetMicroSoundFILTREDefault;
}

/* 0x0045ABD0 */
int SND_fn_iInitSeq(void);

void SND_fn_vInitTheme(void);

/* 0x00457DE0 */
typedef struct SND_pc_tdstSndErrorOccurElement
{
    SndBool bOccured;
    SndBool bDontDisplay;
    SndBool bDONTAskToBeDisplayed;
} SND_pc_tdstSndErrorOccurElement;

static SND_pc_tdstSndErrorOccurElement SND_g_astErrorOccurArray[69];

/* 0x004B746C */
static HWND SND_pc_hMainWindow;

/* 0x0049F14C */
extern unsigned char g_ucErmModuleId;
#define SND_pc_ucErmUnregisteredId g_ucErmModuleId

void __stdcall SND_fn_vInitErrorSnd(void *pInitStruct)
{

    typedef struct _SND_tdstInitStruct
    {
        HINSTANCE hProcessInstance;
        HWND hMainWindow;
        HANDLE hMutexDX;
    } SND_tdstInitStruct;

    SND_pc_hMainWindow = ((SND_tdstInitStruct *)pInitStruct)->hMainWindow;

    if (g_ucSndModuleId == C_ucModuleNotInitialized)
    {
        g_ucSndModuleId = Erm_fn_ucInitErrMsg();
    }
    else
    {
        ERM_fn_vUpdateLastError((unsigned short)3, SND_pc_ucErmUnregisteredId,
                                0, 4, -1, 0xff, 0, NULL);
    }
    memset(SND_g_astErrorOccurArray, 0, sizeof(SND_g_astErrorOccurArray));
}

void __stdcall SND_fn_vDesInitErrorSnd(void)
{
}

void __stdcall _snd_assert(char *test, char *file, int line)
{
    char titre[256];
    char texte[256];

    DebugBreak();
    strcpy(titre, "Sound-Assertion failed in ");
    strcat(titre, file);
    sprintf(texte, " (Line n\xb0 %d)", line);
    strcat(titre, texte);

    SND_fn_vEnterCriticalSectionForErrorDisplay();
    MessageBoxA(SND_pc_hMainWindow, test, titre, 0x40000);
    SND_fn_vQuitCriticalSectionForErrorDisplay();
    DebugBreak();
}

int __stdcall snd_erreur_init(int num_err)
{
    int iRet;

    (void)num_err;
    SND_fn_vEnterCriticalSectionForErrorDisplay();
    iRet = Erm_fn_iMessageBox(
        "To continue with sound, close the application that is using the sound driver, and select Retry.\n"
        "To continue without sound, select Cancel.",
        "Sound-Driver is busy", 0x40005);
    switch (iRet)
    {
    case 2:
        iRet = 3;
        break;
    case 3:
        iRet = 4;
        break;
    case 4:
        iRet = 2;
        break;
    case 5:
        iRet = 3;
        break;
    default:
        iRet = 3;
        break;
    }
    SND_fn_vQuitCriticalSectionForErrorDisplay();
    return iRet;
}

int __stdcall SND_fn_iErreurInit(int idErr)
{
    int iRet;

    (void)idErr;
    SND_fn_vEnterCriticalSectionForErrorDisplay();
    iRet = Erm_fn_iMessageBox(
        "Insert a CD or\n"
        "Close the application using your CD and try again ...\n"
        "else you'll not be able to hear any music",
        "CD-Driver Problem", 0x40005);
    switch (iRet)
    {
    case 2:
        iRet = 3;
        break;
    case 3:
        iRet = 4;
        break;
    case 4:
        iRet = 2;
        break;
    case 5:
        iRet = 3;
        break;
    default:
        iRet = 3;
        break;
    }
    SND_fn_vQuitCriticalSectionForErrorDisplay();
    return iRet;
}

int SND_fn_iInitSound(void *pInitStruct)
{
    int iRet;

    SND_fn_vInitThreadSnd();
    SND_fn_vInitErrorSnd(pInitStruct);
    SND_fn_vInitMallocSnd();
    SND_pc_fn_vLoadDriversFromIni((long *)pInitStruct);
    init_object_type();
    init_micro_fct();
    init_association();

    iRet = SND_fn_iInitSnd(pInitStruct);

    SND_fn_vInitBankSnd(NULL);
    SND_fn_vQuitCriticalSectionThreadSnd();
#if defined(_DLL_COMPILATION_MODE) || defined(SUPERVISATER)
    SND_fn_vLaunchSupervisater(pInitStruct);
#endif

    g_bInitDoneSound = 1;

    return iRet;
}

#if defined(_DLL_COMPILATION_MODE)
void __stdcall SND_fn_vKillVolumeLine(long indice)
{
    if (g_pfn_SND_KillVolumeLine_s == NULL) DebugBreak();
    g_pfn_SND_KillVolumeLine_s(indice);
}
#endif

void SND_pc_fn_vFreeObjectTypeTable(void)
{
    SND_RT_fn_vFreeSndEx(E_ucSndBlockMain, g_d_pstTypeObjectSoundInfos);
}

void SND_pc_fn_vFreeVolumeLines(void)
{
    ptdstVolumeLine pLine, pLineNext;
    int i;

    SNDLST2_M_DynamicForEachMovingElementOf(&(g_VolumeLines.pList), pLine, pLineNext, i)
    {
        SND_RT_fn_vFreeSndEx(E_ucSndBlockMain, pLine);
#if defined(_DLL_COMPILATION_MODE) || defined(SUPERVISATER)
        SND_fn_vKillVolumeLine((long)pLine);
#endif
    }
    nb_volume_lines = 0;
}

/* 0x0045AB10 */
void SND_fn_vDesInitSeq(void);

void __cdecl SND_fn_vDestroyAllMicroSound(void)
{
    int i;
    pMICRO_DESCRIPTION pMicro;
    pMICRO_DESCRIPTION pNextMicro;

    SND_RT_fn_vEnterCriticalSectionThreadSnd();
    SND_RT_fn_vMouchardThreadsnd(SND_C_szSound3DSrcFile, 0x9a8);

    g_iNbActiveMicros=0;
    SNDLST2_M_DynamicForEachMovingElementOf(&micros_sound.pList,pMicro,pNextMicro,i)
    {
#if defined(_DLL_COMPILATION_MODE) || defined(SUPERVISATER)
        SND_fn_vKillMicro((long)pMicro);
#endif
        SND_fn_vDestroyMicroSnd(pMicro->sndId);
        SNDLST2_M_DynamicIsolate(pMicro);
        SND_fn_vFreeSndEx(E_ucSndBlockMain,pMicro);
    }

    SND_RT_fn_vQuitCriticalSectionThreadSnd();
}

void __cdecl SND_fn_vDesInitTheme(void)
{
}

#if defined(_DLL_COMPILATION_MODE)
void __cdecl SND_pc_fn_vFreeDriverDlls(void)
{
    if (SND_pc_hModuleSxd != NULL)
    {
        FreeLibrary(SND_pc_hModuleSxd);
    }
    if (SND_pc_hModuleCd != NULL)
    {
        FreeLibrary(SND_pc_hModuleCd);
    }
    if (SND_pc_hModuleMpeg != NULL)
    {
        FreeLibrary(SND_pc_hModuleMpeg);
    }
    if (SND_pc_hModuleDbg != NULL)
    {
        FreeLibrary(SND_pc_hModuleDbg);
    }
    if (SND_pc_hModuleAdpcm != NULL)
    {
        FreeLibrary(SND_pc_hModuleAdpcm);
    }
    if (SND_pc_hModuleRecord != NULL)
    {
        FreeLibrary(SND_pc_hModuleRecord);
    }
}
#endif

void __cdecl SND_fn_vDesInitSound(void)
{
#if defined(_DLL_COMPILATION_MODE) || defined(SUPERVISATER)
    SND_fn_vKillSupervisater();
#endif
    if (g_bInitDoneSound != 0)
    {
        g_bInitDoneSound = 0;
        SND_fn_vDestroyAllMicroSound();
        SND_RT_fn_vEnterCriticalSectionThreadSnd();
        SND_RT_fn_vMouchardThreadsnd(SND_C_szSound3DSrcFile, 0x639);
        stop_son_all();
        SND_pc_fn_vFreeObjectTypeTable();
        SND_pc_fn_vFreeVolumeLines();
        SND_fn_vDesInitBankSnd();
        SND_fn_vDesInitSnd();
        SND_fn_vDesInitThreadSnd();
        SND_fn_vDesInitMallocSnd();
        SND_PrintUsedStaticMemory();
#if defined(_DLL_COMPILATION_MODE)
        SND_pc_fn_vFreeDriverDlls();
#endif
    }
}

#if defined(_DLL_COMPILATION_MODE)
void __stdcall SND_fn_vAddMicro(long indice)
{
    if (g_pfn_SND_AddMicro_s == NULL) DebugBreak();
    g_pfn_SND_AddMicro_s(indice);
}
#endif

long __cdecl SND_fn_lCreateMicroSound(void)
{
        long lRet=C_CREATEMICRO_FAILED;
        pMICRO_DESCRIPTION pMicro;

        SND_RT_fn_vEnterCriticalSectionThreadSnd();
        SND_RT_fn_vMouchardThreadsnd(SND_C_szSound3DSrcFile, 0x967);

		if (g_iNbActiveMicros==NB_MICRO_MAX)
		{
			SND_fn_vDisplayError(E_uwSndSystemError,"Too many microphone");
			SND_RT_fn_vQuitCriticalSectionThreadSnd();
			return C_CREATEMICRO_FAILED;
		}

        g_iNbActiveMicros++;

        pMicro=SND_fn_pvMallocSndEx(E_ucSndBlockMain,sizeof(MICRO_DESCRIPTION));
        SNDLST2_M_DynamicInitElement(pMicro);
        SNDLST2_M_DynamicAddTail(&micros_sound.pList,pMicro);

        pMicro->sndId=SND_fn_lCreateMicroSnd(NULL);
        lRet=(long)pMicro;

#if defined(_DLL_COMPILATION_MODE) || defined(SUPERVISATER)
        SND_fn_vAddMicro(lRet);
#endif
        SND_RT_fn_vQuitCriticalSectionThreadSnd();
        return lRet;
}

/* 0x00457C80 */
int __stdcall SND_pc_fn_iSetNodeByteStd(tdstBlockResourceMem *pRes, unsigned char ucVolume)
{
    pRes->ucVolume = ucVolume;
    return TRUE;
}

int __cdecl SND_pc_fn_iSetSequenceNodeByte(tdstBlockResourceMem *pRes, unsigned char ucVolume)
{
    pRes->ucVolume = ucVolume;
    return TRUE;
}

int __cdecl SND_pc_fn_iSetSplitNodeByte(tdstBlockResourceMem *pRes, unsigned char ucVolume)
{
    pRes->ucVolume = ucVolume;
    return TRUE;
}

int __cdecl SND_pc_fn_iSetSwitchNodeByte(tdstBlockResourceMem *pRes, unsigned char ucVolume)
{
    pRes->ucVolume = ucVolume;
    return TRUE;
}

int __cdecl SND_pc_fn_iSetThemeNodeByte(tdstBlockResourceMem *pRes, unsigned char ucVolume)
{
    pRes->ucVolume = ucVolume;
    return TRUE;
}

/* 0x004582C0 */
void __cdecl SND_pc_fn_vSetUnitVecX(long lUnused, long *pVec)
{
    if (pVec != NULL)
    {
        pVec[0] = 0x10000;
        pVec[1] = 0;
        pVec[2] = 0;
    }
}

void __cdecl SND_pc_fn_vSetUnitVecY(long lUnused, long *pVec)
{
    if (pVec != NULL)
    {
        pVec[0] = 0;
        pVec[1] = 0x10000;
        pVec[2] = 0;
    }
}

void __cdecl SND_pc_fn_vSetUnitVecZ(long lUnused, long *pVec)
{
    if (pVec != NULL)
    {
        pVec[0] = 0;
        pVec[1] = 0;
        pVec[2] = 0x10000;
    }
}

void __cdecl SND_pc_fn_vSetThreeRealsToOne(long lUnused0, long lUnused1,
                                           long *pA, long *pB, long *pC)
{
    if (pA != NULL)
    {
        *pA = 0x10000;
    }
    if (pB != NULL)
    {
        *pB = 0x10000;
    }
    if (pC != NULL)
    {
        *pC = 0x10000;
    }
}

static long SND_pc_g_lUnnamedA207C = 3;   /* 0x004A207C */

long __cdecl SND_pc_fn_lGetTwicePlus25(void)
{
    return SND_pc_g_lUnnamedA207C * 2 + 0x19;
}

void __cdecl SND_pc_fn_vCopyTypeSubTableDefault(long lTypeId)
{
    memcpy(&g_d_pstTypeObjectSoundInfos[lTypeId].pc_stRetObjectSoundKey,
           &SND_pc_g_stRetObjectSoundKeyDefault, sizeof(SND_pc_tdstAllRetObjectSoundKey));
}

/* 0x004A3BA8 */
static unsigned char fn_ucRetObjectSoundREVERBDefault(long indice)
{
    (void)indice;
    return 0;
}

/* 0x004A2480 */
static tdstAllRetObjectSound g_astTypeObjectSoundInfosDefault =
{
    (td_pfn_rRetSoundCoord)SND_pc_fn_bNop0, (td_pfn_rRetSoundCoord)SND_pc_fn_bNop0,
    (td_pfn_rRetSoundCoord)SND_pc_fn_bNop0, (td_pfn_rRetSoundCoord)SND_pc_fn_bNop0,
    (td_pfn_rRetSoundCoord)SND_pc_fn_bNop0, (td_pfn_rRetSoundCoord)SND_pc_fn_bNop0,
    fn_ucRetObjectSoundREVERBDefault,
    (td_pfn_vRetSoundExtraCoef)SND_pc_fn_vSetThreeRealsToOne,
    (td_pfn_lRetSoundSwitch)SND_pc_fn_bNop0,
    (td_pfn_bRetSoundLink)SND_fn_bCanSetOptions,
    (td_pc_pfn_vRetSoundAxis)SND_pc_fn_vSetUnitVecX, (td_pc_pfn_vRetSoundAxis)SND_pc_fn_vSetUnitVecY,
    (td_pc_pfn_vRetSoundAxis)SND_pc_fn_vSetUnitVecZ
};

static SND_pc_tdstAllRetObjectSoundKey SND_pc_g_stRetObjectSoundKeyDefault =   /* 0x004A24B8 */
{
    (td_pc_pfn_rRetSoundCoordKey)SND_pc_fn_bNop0, (td_pc_pfn_rRetSoundCoordKey)SND_pc_fn_bNop0,
    (td_pc_pfn_rRetSoundCoordKey)SND_pc_fn_bNop0, (td_pc_pfn_rRetSoundCoordKey)SND_pc_fn_bNop0,
    (td_pc_pfn_rRetSoundCoordKey)SND_pc_fn_bNop0, (td_pc_pfn_rRetSoundCoordKey)SND_pc_fn_bNop0,
    (td_pfn_lRetSoundSwitch)SND_fn_bCanSetOptions,
    1
};
