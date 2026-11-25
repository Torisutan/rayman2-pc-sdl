#pragma once
/*
 =======================================================================================
  Name        : sdl_timer.h
  Description : Timer platform functions through SDL (USE_SDL)

  The original engine's console timer code declares GetTickCount itself, so RND/RND.c
  and GLI/GLI_fastload.c call it on every target. On WIN32 the system's serves
  (<windows.h>); other targets get it from TMR/sdl_timer.c.
 =======================================================================================
 */
#ifdef USE_SDL

#ifndef WIN32
/**********************************************************************************************/
/* Name: GetTickCount */
/* Goal: milliseconds, counted from SDL's start */
/**********************************************************************************************/
unsigned long GetTickCount(void);
#endif /* WIN32 */

#endif /* USE_SDL */
