#pragma once
/*
 =======================================================================================
  Name        : sdl_timer.h
  Description : Timer platform functions through SDL (USE_SDL)

  The original engine's console timer code gives GetTickCount itself. With USE_SDL it
  comes from TMR/sdl_timer.c: RND/RND.c and GLI/GLI_fastload.c call GetTickCount as
  before, the name goes to TMR_sdl_fn_ulGetTickCount.
 =======================================================================================
 */
#ifdef USE_SDL

#ifdef _WIN32
#include <windows.h>   /* first: the macro below would rename its own declaration */
#endif /* _WIN32 */

/**********************************************************************************************/
/* Name: TMR_sdl_fn_ulGetTickCount */
/* Goal: GetTickCount: milliseconds, counted from SDL's start instead of the system's */
/**********************************************************************************************/
unsigned long __cdecl TMR_sdl_fn_ulGetTickCount(void);

#define GetTickCount  TMR_sdl_fn_ulGetTickCount

#endif /* USE_SDL */
