/*
 =======================================================================================
  Name        : sdl_timer.c
  Description : Timer counters through SDL (USE_SDL)

  The SDL counterpart of the PC timer functions in GLI/cpu_timer.c and GLI/timer_sub2.c
  (fenced #ifndef USE_SDL there), as the original engine's console targets give them:
  fn_vTimerGetCounter, fn_ulTimerTickPerSecond, fn_ulTimerEventTickPerSecond,
  TMR_fn_ulFastGetInternalCounter, and GetTickCount where the system gives none (not on WIN32; TMR/sdl_timer.h).
  TMR_fn_wInitLibrary stays common.

  C_wTimerFrequencyLow    milliseconds (timeGetTime on PC)
  C_wTimerFrequencyMedium SDL's performance counter (the Win32 performance counter
                          on Windows, as on PC)
  C_wTimerFrequencyHigh   SDL's performance counter as well. On PC it is the CPU's
                          time stamp counter, whose rate is measured at init by a one
                          second busy wait; the engine and the Direct3D 6 driver read
                          only the medium channel.
  The system timer period (timeBeginPeriod on PC) is SDL's: 1 ms by default.
 =======================================================================================
 */
#ifdef USE_SDL

#include <SDL3/SDL_timer.h>

#include "GLI/cpu_timer.h"
#include "TMR/TMR_types.h"
#include "TMR/sdl_timer.h"
#include "TMR/Specif/counter.h"
#include "app/regress.h"

/*
 ----------------------------------------------------------------------------------------
  Description   : Ticks per second of the timer events (the system timer period on PC)
 ----------------------------------------------------------------------------------------
 */
unsigned long __cdecl fn_ulTimerEventTickPerSecond(short wTimerType)
{
    switch ( wTimerType )
    {
        case C_wTimerFrequencyLow:
            return 1000ul;
        default:
            return 0ul;
    }
}

/*
 ----------------------------------------------------------------------------------------
  Description   : Current count of a timer channel
 ----------------------------------------------------------------------------------------
 */
void __cdecl fn_vTimerGetCounter(short wTimerType, LARGE_INTEGER *p_stOut)
{
#ifdef RAYMAN2_REGRESS
    if ( REGRESS_fn_bGetCounter(wTimerType, p_stOut) )
        return;
#endif /* RAYMAN2_REGRESS */

    switch ( wTimerType )
    {
        case C_wTimerFrequencyLow:
            p_stOut->u.LowPart  = (DWORD)SDL_GetTicks();
            p_stOut->u.HighPart = 0;
            break;
        case C_wTimerFrequencyMedium:
        case C_wTimerFrequencyHigh:
            p_stOut->QuadPart = (LONGLONG)SDL_GetPerformanceCounter();
            break;
    }
}

/*
 ----------------------------------------------------------------------------------------
  Description   : Ticks per second of a timer channel
 ----------------------------------------------------------------------------------------
 */
unsigned long __cdecl fn_ulTimerTickPerSecond(short wTimerType)
{
    switch ( wTimerType )
    {
        case C_wTimerFrequencyLow:
            return 1000ul;
        case C_wTimerFrequencyMedium:
        case C_wTimerFrequencyHigh:
            return (unsigned long)SDL_GetPerformanceFrequency();
        default:
            return 0ul;
    }
}

/*
 ----------------------------------------------------------------------------------------
  Description   : The timer library's fast counter: the low 32 bits of SDL's performance
                  counter (PC: of the CPU's time stamp counter)
 ----------------------------------------------------------------------------------------
 */
unsigned long TMR_fn_ulFastGetInternalCounter(void)
{
    return (unsigned long)SDL_GetPerformanceCounter();
}

#ifndef WIN32
/*
 ----------------------------------------------------------------------------------------
  Description   : GetTickCount: milliseconds since SDL's start
 ----------------------------------------------------------------------------------------
 */
unsigned long GetTickCount(void)
{
    return (unsigned long)SDL_GetTicks();
}
#endif /* WIN32 */

#endif /* USE_SDL */
