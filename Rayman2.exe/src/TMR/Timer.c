#include "GLI/int64_arithmetic.h"

#include <windows.h>

#include "TMR/TMR.h"
#include "ERM/CpaError.h"
#include "GLI/int64_arithmetic.h"

typedef int _tmr_size_check[(sizeof(TMR_tdstTimer) == 0x24) ? 1 : -1];

typedef void  (__cdecl *tFUN_0045E930)(short wType, LARGE_INTEGER *out);

#include "GLI/cpu_timer.h"

#include "TMR/TMR_globals.h"
#include "TMR/errTmr.h"

void __cdecl fn_vAddTimerValues(unsigned int *p_uDst, unsigned int *p_uSrc)
{
    unsigned int uNewLo = p_uDst[0] + p_uSrc[0];
    if (uNewLo < p_uDst[0]) {
        p_uDst[1] += 1;
    }
    p_uDst[0] = uNewLo;
    p_uDst[1] += p_uSrc[1];
}

void __cdecl fn_vSubTimerValues(unsigned int *p_uDst, unsigned int *p_uSrc)
{
    if (p_uDst[1] == p_uSrc[1]) {

        p_uDst[1] = 0;
        p_uDst[0] -= p_uSrc[0];
    } else if (p_uDst[0] >= p_uSrc[0]) {

        p_uDst[0] -= p_uSrc[0];
        p_uDst[1] -= p_uSrc[1];
    } else {

        unsigned int uNewLo = p_uDst[0] - p_uSrc[0];
        unsigned int uHmDec = p_uDst[1] - 1;
        p_uDst[0]  = uNewLo;
        p_uDst[1]  = uHmDec;
        p_uDst[1]  = uHmDec - p_uSrc[1];
    }
}

/* 0x0045DE70 */
short __stdcall TMR_fn_wCreateTimer( short wTimerType )
{
    short   i;

    if ( TimerLibOk == 0 )
    {
        Erm_M_UpdateLastError(Tmr, C_ucErmDefaultChannel, E_uwTmrLibraryNotInitialized, C_lErmNoDebugData, C_ucErmOpenInfoWindow, C_ucAllowStopForDebug, NULL);
        return (C_wTimerError);
    }
    if ( wTimerCount == C_uwTimerMaxCount )
    {
        Erm_M_UpdateLastError(Tmr, C_ucErmDefaultChannel, E_uwTmrMaxTimerCountReached, C_lErmNoDebugData, C_ucErmOpenInfoWindow, C_ucAllowStopForDebug, NULL);
        return (C_wTimerError);
    }
    if ( wTimerType > C_wTimerFrequencyHigh )
    {
        Erm_M_UpdateLastError(Tmr, C_ucErmDefaultChannel, E_uwTmrInvalidType, C_lErmNoDebugData, C_ucErmOpenInfoWindow, C_ucAllowStopForDebug, NULL);
        return (C_wTimerError);
    }
    if ( a3_ulTimerFrequency[wTimerType] == 0 )
    {
        Erm_M_UpdateLastError(Tmr, C_ucErmDefaultChannel, E_uwTmrTypeNotAvailable, C_lErmNoDebugData, C_ucErmOpenInfoWindow, C_ucAllowStopForDebug, NULL);
        return (C_wTimerError);
    }

    for ( i = 0 ; i < C_uwTimerMaxCount ; i++ )
        if ( a_stTimerTab[i].m_wTimerState == C_wTimerUnused ) break;
    if ( i == C_uwTimerMaxCount )
    {
        Erm_M_UpdateLastError(Tmr, C_ucErmDefaultChannel, E_uwTmrMaxTimerCountReached, C_lErmNoDebugData, C_ucErmOpenInfoWindow, C_ucAllowStopForDebug, NULL);
        return (C_wTimerError);
    }

    a_stTimerTab[i].m_wTimerType = wTimerType;
    a_stTimerTab[i].m_wTimerState = C_wTimerReady;
    wTimerCount++;

    return (i);
}

/* 0x0045DF90 */
short __stdcall TMR_fn_wStartTimer( short wTimerHandle )
{
    short          wTimerType;
    stTimerCount   stValue;

    if ( TimerLibOk == 0 )
    {
        Erm_M_UpdateLastError (Tmr, C_ucErmDefaultChannel, E_uwTmrLibraryNotInitialized, C_lErmNoDebugData, C_ucErmOpenInfoWindow, C_ucAllowStopForDebug, NULL);
        return (C_wTimerError);
    }
    if ( wTimerHandle >= C_uwTimerMaxCount )
    {
        Erm_M_UpdateLastError (Tmr, C_ucErmDefaultChannel, E_uwTmrInvalidHandle, C_lErmNoDebugData, C_ucErmOpenInfoWindow, C_ucAllowStopForDebug, NULL);
        return (C_wTimerError);
    }
    if ( a_stTimerTab[wTimerHandle].m_wTimerState == C_wTimerUnused )
    {
        Erm_M_UpdateLastError (Tmr, C_ucErmDefaultChannel, E_uwTmrInvalidHandle, C_lErmNoDebugData, C_ucErmOpenInfoWindow, C_ucAllowStopForDebug, NULL);
        return (C_wTimerError);
    }
    if ( a_stTimerTab[wTimerHandle].m_wTimerType > C_wTimerFrequencyHigh )
    {
        Erm_M_UpdateLastError (Tmr, C_ucErmDefaultChannel, E_uwTmrInvalidForEventTimer, C_lErmNoDebugData, C_ucErmOpenInfoWindow, C_ucAllowStopForDebug, NULL);
        return (C_wTimerError);
    }

    wTimerType = a_stTimerTab[wTimerHandle].m_wTimerType;
    fn_vTimerGetCounter(wTimerType,(LARGE_INTEGER *)&stValue);

    a_stTimerTab[wTimerHandle].m_stTimerCount_InitValue.m_ulLowPart = stValue.m_ulLowPart;
    a_stTimerTab[wTimerHandle].m_stTimerCount_InitValue.m_ulHighPart = stValue.m_ulHighPart;

    if ( a_stTimerTab[wTimerHandle].m_wTimerState != C_wTimerPaused )
    {
        a_stTimerTab[wTimerHandle].m_stTimerCount_TotalValue.m_ulLowPart = 0;
        a_stTimerTab[wTimerHandle].m_stTimerCount_TotalValue.m_ulHighPart = 0;
    }

    a_stTimerTab[wTimerHandle].m_wTimerState = C_wTimerRunning;

    return (0);
}

/* 0x0045E0C0 */
short __stdcall TMR_fn_wStopTimer( short wTimerHandle )
{
    short          wTimerType;
    stTimerCount   stValue;

    if ( TimerLibOk == 0 )
    {
        Erm_M_UpdateLastError (Tmr, C_ucErmDefaultChannel, E_uwTmrLibraryNotInitialized, C_lErmNoDebugData, C_ucErmOpenInfoWindow, C_ucAllowStopForDebug, NULL);
        return (C_wTimerError);
    }
    if ( wTimerHandle >= C_uwTimerMaxCount )
    {
        Erm_M_UpdateLastError (Tmr, C_ucErmDefaultChannel, E_uwTmrInvalidHandle, C_lErmNoDebugData, C_ucErmOpenInfoWindow, C_ucAllowStopForDebug, NULL);
        return (C_wTimerError);
    }
    if ( a_stTimerTab[wTimerHandle].m_wTimerState == C_wTimerUnused )
    {
        Erm_M_UpdateLastError (Tmr, C_ucErmDefaultChannel, E_uwTmrInvalidHandle, C_lErmNoDebugData, C_ucErmOpenInfoWindow, C_ucAllowStopForDebug, NULL);
        return (C_wTimerError);
    }
    if ( a_stTimerTab[wTimerHandle].m_wTimerType > C_wTimerFrequencyHigh )
    {
        Erm_M_UpdateLastError (Tmr, C_ucErmDefaultChannel, E_uwTmrInvalidForEventTimer, C_lErmNoDebugData, C_ucErmOpenInfoWindow, C_ucAllowStopForDebug, NULL);
        return (C_wTimerError);
    }

    wTimerType = a_stTimerTab[wTimerHandle].m_wTimerType;

    if ( a_stTimerTab[wTimerHandle].m_wTimerState == C_wTimerRunning )
    {
        fn_vTimerGetCounter(wTimerType,(LARGE_INTEGER *)&stValue);
        a_stTimerTab[wTimerHandle].m_stTimerCount_LastValue.m_ulLowPart = stValue.m_ulLowPart;
        a_stTimerTab[wTimerHandle].m_stTimerCount_LastValue.m_ulHighPart = stValue.m_ulHighPart;
    }

    if ( a_stTimerTab[wTimerHandle].m_wTimerState == C_wTimerPaused )
    {
        a_stTimerTab[wTimerHandle].m_stTimerCount_LastValue.m_ulLowPart =
          a_stTimerTab[wTimerHandle].m_stTimerCount_TotalValue.m_ulLowPart;
        a_stTimerTab[wTimerHandle].m_stTimerCount_LastValue.m_ulHighPart =
          a_stTimerTab[wTimerHandle].m_stTimerCount_TotalValue.m_ulHighPart;

        a_stTimerTab[wTimerHandle].m_stTimerCount_TotalValue.m_ulLowPart = 0;
        a_stTimerTab[wTimerHandle].m_stTimerCount_TotalValue.m_ulHighPart = 0;
        a_stTimerTab[wTimerHandle].m_stTimerCount_InitValue.m_ulLowPart = 0;
        a_stTimerTab[wTimerHandle].m_stTimerCount_InitValue.m_ulHighPart = 0;
    }

    a_stTimerTab[wTimerHandle].m_wTimerState = C_wTimerStop;

    return (0);
}

/* 0x0045E210 */
short __stdcall TMR_fn_wSetTimerValue( short wTimerHandle, stTimerCount *p_stValue )
{
    short           wTimerType;

    if ( TimerLibOk == 0 )
    {
        Erm_M_UpdateLastError (Tmr, C_ucErmDefaultChannel, E_uwTmrLibraryNotInitialized, C_lErmNoDebugData, C_ucErmOpenInfoWindow, C_ucAllowStopForDebug, NULL);
        return (C_wTimerError);
    }
    if ( wTimerHandle >= C_uwTimerMaxCount )
    {
        Erm_M_UpdateLastError (Tmr, C_ucErmDefaultChannel, E_uwTmrInvalidHandle, C_lErmNoDebugData, C_ucErmOpenInfoWindow, C_ucAllowStopForDebug, NULL);
        return (C_wTimerError);
    }
    if ( a_stTimerTab[wTimerHandle].m_wTimerType > C_wTimerFrequencyHigh )
    {
        Erm_M_UpdateLastError (Tmr, C_ucErmDefaultChannel, E_uwTmrInvalidForEventTimer, C_lErmNoDebugData, C_ucErmOpenInfoWindow, C_ucAllowStopForDebug, NULL);
        return (C_wTimerError);
    }

    wTimerType = a_stTimerTab[wTimerHandle].m_wTimerType;

    a_stTimerTab[wTimerHandle].m_stTimerCount_TotalValue.m_ulLowPart = p_stValue->m_ulLowPart;
    a_stTimerTab[wTimerHandle].m_stTimerCount_TotalValue.m_ulHighPart = p_stValue->m_ulHighPart;

    a_stTimerTab[wTimerHandle].m_wTimerState = C_wTimerPaused;

    return (0);
}

/* 0x0045E2E0 */
short __stdcall TMR_fn_wReadTimer( short wTimerHandle, stTimerCount *p_stTimerCount)
{
    short           wTimerState;
    stTimerCount    stValue;

    if ( TimerLibOk == 0 )
    {
        Erm_M_UpdateLastError (Tmr, C_ucErmDefaultChannel, E_uwTmrLibraryNotInitialized, C_lErmNoDebugData, C_ucErmOpenInfoWindow, C_ucAllowStopForDebug, NULL);
        return (C_wTimerError);
    }
    if ( wTimerHandle >= C_uwTimerMaxCount )
    {
        Erm_M_UpdateLastError (Tmr, C_ucErmDefaultChannel, E_uwTmrInvalidHandle, C_lErmNoDebugData, C_ucErmOpenInfoWindow, C_ucAllowStopForDebug, NULL);
        return (C_wTimerError);
    }
    if ( a_stTimerTab[wTimerHandle].m_wTimerState == C_wTimerUnused )
    {
        Erm_M_UpdateLastError (Tmr, C_ucErmDefaultChannel, E_uwTmrInvalidHandle, C_lErmNoDebugData, C_ucErmOpenInfoWindow, C_ucAllowStopForDebug, NULL);
        return (C_wTimerError);
    }
    if ( a_stTimerTab[wTimerHandle].m_wTimerType > C_wTimerFrequencyHigh )
    {
        Erm_M_UpdateLastError (Tmr, C_ucErmDefaultChannel, E_uwTmrInvalidForEventTimer, C_lErmNoDebugData, C_ucErmOpenInfoWindow, C_ucAllowStopForDebug, NULL);
        return (C_wTimerError);
    }

    wTimerState = a_stTimerTab[wTimerHandle].m_wTimerState;

    switch ( wTimerState )
    {

    case C_wTimerRunning:
        fn_vTimerGetCounter(a_stTimerTab[wTimerHandle].m_wTimerType,(LARGE_INTEGER *)p_stTimerCount);
        stValue.m_ulLowPart = a_stTimerTab[wTimerHandle].m_stTimerCount_InitValue.m_ulLowPart;
        stValue.m_ulHighPart = a_stTimerTab[wTimerHandle].m_stTimerCount_InitValue.m_ulHighPart;
        fn_vSubTimerValues((unsigned int *)p_stTimerCount,(unsigned int *)&stValue);
        break;

    case C_wTimerStop:
        p_stTimerCount->m_ulLowPart = a_stTimerTab[wTimerHandle].m_stTimerCount_LastValue.m_ulLowPart;
        p_stTimerCount->m_ulHighPart = a_stTimerTab[wTimerHandle].m_stTimerCount_LastValue.m_ulHighPart;
        stValue.m_ulLowPart = a_stTimerTab[wTimerHandle].m_stTimerCount_InitValue.m_ulLowPart;
        stValue.m_ulHighPart = a_stTimerTab[wTimerHandle].m_stTimerCount_InitValue.m_ulHighPart;
        fn_vSubTimerValues((unsigned int *)p_stTimerCount,(unsigned int *)&stValue);
        break;

    case C_wTimerReady:
    case C_wTimerPaused:
        p_stTimerCount->m_ulHighPart = 0;
        p_stTimerCount->m_ulLowPart = 0;
        break;
    }

    stValue.m_ulLowPart = a_stTimerTab[wTimerHandle].m_stTimerCount_TotalValue.m_ulLowPart;
    stValue.m_ulHighPart = a_stTimerTab[wTimerHandle].m_stTimerCount_TotalValue.m_ulHighPart;
    fn_vAddTimerValues((unsigned int *)p_stTimerCount,(unsigned int *)&stValue);

    return (0);
}

/* 0x0045E480 */
short __stdcall TMR_fn_wResetTimer( short wTimerHandle )
{
    if ( TimerLibOk == 0 )
    {
        Erm_M_UpdateLastError (Tmr, C_ucErmDefaultChannel, E_uwTmrLibraryNotInitialized, C_lErmNoDebugData, C_ucErmOpenInfoWindow, C_ucAllowStopForDebug, NULL);
        return (C_wTimerError);
    }
    if ( wTimerHandle >= C_uwTimerMaxCount )
    {
        Erm_M_UpdateLastError (Tmr, C_ucErmDefaultChannel, E_uwTmrInvalidHandle, C_lErmNoDebugData, C_ucErmOpenInfoWindow, C_ucAllowStopForDebug, NULL);
        return (C_wTimerError);
    }
    if ( a_stTimerTab[wTimerHandle].m_wTimerState == C_wTimerUnused )
    {
        Erm_M_UpdateLastError (Tmr, C_ucErmDefaultChannel, E_uwTmrInvalidHandle, C_lErmNoDebugData, C_ucErmOpenInfoWindow, C_ucAllowStopForDebug, NULL);
        return (C_wTimerError);
    }
    if ( a_stTimerTab[wTimerHandle].m_wTimerType > C_wTimerFrequencyHigh )
    {
        Erm_M_UpdateLastError (Tmr, C_ucErmDefaultChannel, E_uwTmrInvalidForEventTimer, C_lErmNoDebugData, C_ucErmOpenInfoWindow, C_ucAllowStopForDebug, NULL);
        return (C_wTimerError);
    }

    a_stTimerTab[wTimerHandle].m_wTimerState = C_wTimerReady;

    a_stTimerTab[wTimerHandle].m_stTimerCount_InitValue.m_ulLowPart = 0;
    a_stTimerTab[wTimerHandle].m_stTimerCount_InitValue.m_ulHighPart = 0;
    a_stTimerTab[wTimerHandle].m_stTimerCount_LastValue.m_ulLowPart = 0;
    a_stTimerTab[wTimerHandle].m_stTimerCount_LastValue.m_ulHighPart = 0;
    a_stTimerTab[wTimerHandle].m_stTimerCount_TotalValue.m_ulLowPart = 0;
    a_stTimerTab[wTimerHandle].m_stTimerCount_TotalValue.m_ulHighPart = 0;

    return (0);
}

/* 0x0051616C */
short __stdcall TMR_fn_wGetTimerFrequency( short wTimerHandle , unsigned long *p_ulFrequency )
{
    if ( TimerLibOk == 0 )
    {
        Erm_M_UpdateLastError (Tmr, C_ucErmDefaultChannel, E_uwTmrLibraryNotInitialized, C_lErmNoDebugData, C_ucErmOpenInfoWindow, C_ucAllowStopForDebug, NULL);
        return (C_wTimerError);
    }
    if ( a_stTimerTab[wTimerHandle].m_wTimerState == C_wTimerUnused )
    {
        Erm_M_UpdateLastError (Tmr, C_ucErmDefaultChannel, E_uwTmrInvalidHandle, C_lErmNoDebugData, C_ucErmOpenInfoWindow, C_ucAllowStopForDebug, NULL);
        return (C_wTimerError);
    }

    if ( a_stTimerTab[wTimerHandle].m_wTimerType <= C_wTimerFrequencyHigh )
        *p_ulFrequency = a3_ulTimerFrequency[a_stTimerTab[wTimerHandle].m_wTimerType];
    else
        /* 0x45E609 */
        *p_ulFrequency = a3_ulEventFrequency[a_stTimerTab[wTimerHandle].m_wTimerType - C_wTimerEvent];

    return (0);
}
