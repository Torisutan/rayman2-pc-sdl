#include "GLI/timer_sub2.h"
#include "TMR/TMR_globals.h"

#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <mmsystem.h>

#ifndef USE_SDL   /* the SDL counterpart of the PC timer functions: TMR/sdl_timer.c */

void __cdecl TMR_pc_fn_vReadTSC(unsigned __int64 *p_uOut)
{

    __asm {
        _emit 0x0f
        _emit 0x31
        mov   ebx, p_uOut
        mov   dword ptr [ebx], eax
        mov   dword ptr [ebx + 4], edx
    }
}

#if defined(WIN64) || defined(LINUX) || defined(OSX) || defined(PSP) || defined(WIIU)
/* the timer library's fast counter (TMR/Specif/counter.h) for AI/FuncRay2.c's C branch: the low
 * 32 bits of the time stamp counter, as that function's PC form reads them inline */
unsigned long TMR_fn_ulFastGetInternalCounter(void)
{
    unsigned __int64 u64Counter;

    TMR_pc_fn_vReadTSC(&u64Counter);
    return (unsigned long)u64Counter;
}
#endif /* WIN64 || LINUX || OSX || PSP || WIIU */

unsigned long __cdecl TMR_pc_fn_ulCalibrateTscFreq(void)
{
    typedef DWORD (__stdcall *pfn_timeGetTime)(void);
    pfn_timeGetTime pfn_GetTime = timeGetTime;

    unsigned __int64 u64Start;
    unsigned __int64 u64End;
    DWORD            ulT0;
    DWORD            ulElapsed;
    unsigned long    ulStartLo;
    unsigned long    ulEndLo;
    unsigned long    ulDelta;

    ulT0 = pfn_GetTime();
    TMR_pc_fn_vReadTSC(&u64Start);
    ulElapsed = pfn_GetTime() - ulT0;
    while (ulElapsed < 1000) {
        ulElapsed = pfn_GetTime() - ulT0;
    }
    TMR_pc_fn_vReadTSC(&u64End);

    ulStartLo = (unsigned long)u64Start;
    ulEndLo   = (unsigned long)u64End;
    if (ulStartLo < ulEndLo) {
        ulDelta = ulEndLo - ulStartLo;
    } else {
        ulDelta = (ulEndLo - ulStartLo) - 1;
    }

    if (ulDelta < 63000000)  return 60000000;
    if (ulDelta < 68000000)  return 66600000;
    if (ulDelta < 80000000)  return 75000000;
    if (ulDelta < 94000000)  return 90000000;
    if (ulDelta < 106000000) return 100000000;
    if (ulDelta < 125000000) return 120000000;
    if (ulDelta < 138000000) return 0x07f1ff20;
    if (ulDelta < 155000000) return 150000000;
    if (ulDelta < 170000000) return 0x09ee1d40;
    if (ulDelta < 185000000) return 180000000;
    if (ulDelta < 205000000) return 200000000;
    if (ulDelta < 240000000) return 0x0de7e020;
    if (ulDelta < 272000000) return 0x0fe3fe40;
    if (ulDelta < 310000000) return 300000000;
    if (339999999 < ulDelta) {
        if (ulDelta < 356000000) return 350000000;
        if (ulDelta < 380000000) return 0x15d9df40;
        if (ulDelta < 425000000) return 400000000;
        if (474999999 < ulDelta) {
            if (ulDelta < 525000000) return 500000000;
            if (574999999 < ulDelta) {
                if (ulDelta < 625000000) {
                    ulDelta = 600000000;
                }
                return ulDelta;
            }
            return 550000000;
        }
        return 450000000;
    }
    return 0x13ddc120;
}

void __cdecl TMR_pc_fn_vSetupTimerPeriod(void)
{
    typedef MMRESULT (__stdcall *pfn_timeGetDevCaps)(LPTIMECAPS, UINT);
    typedef MMRESULT (__stdcall *pfn_timeBeginPeriod)(UINT);

    {
        TIMECAPS tc;
        pfn_timeGetDevCaps  pfn_GetCaps;
        pfn_timeBeginPeriod pfn_BeginPeriod;
        unsigned int uChosen;

        if (TMR_pc_g_bTimerPeriodSet != 0) {
            return;
        }

        pfn_GetCaps     = timeGetDevCaps;
        pfn_BeginPeriod = timeBeginPeriod;

        if (pfn_GetCaps(&tc, sizeof(tc)) != TIMERR_NOERROR) {
            TMR_pc_g_ulTimerPeriod   = 0;
            TMR_pc_g_bTimerPeriodSet = 1;
            return;
        }

        uChosen = (((tc.wPeriodMin > 1) ? tc.wPeriodMin : 1u) < tc.wPeriodMax)
                  ? ((tc.wPeriodMin > 1) ? tc.wPeriodMin : 1u)
                  : tc.wPeriodMax;

        TMR_pc_g_ulTimerPeriod = uChosen;
        pfn_BeginPeriod(uChosen);
        TMR_pc_g_bTimerPeriodSet = 1;
    }
}

#endif /* USE_SDL */
