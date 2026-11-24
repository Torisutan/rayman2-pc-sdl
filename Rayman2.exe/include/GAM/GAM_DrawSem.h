#pragma once
/*
 =======================================================================================
  Name        : GAM_DrawSem.h
  Description : The draw semaphore with USE_SDL

  The draw semaphore (fn_hGetDrawSemaphore) keeps viewport and camera changes and the
  flips of the window thread and the engine thread apart on PC. GLD.c, Basic.c,
  GAM_GameEng.c, ToolCam.c, Texture.c and ui_state.c take and give it back through
  WaitForSingleObject and ReleaseSemaphore, on no other object. With USE_SDL it is an
  SDL semaphore (GAM/sdl_main.c) and those calls go to it, so the call sites stay.
 =======================================================================================
 */
#ifdef USE_SDL

#ifdef _WIN32
#include <windows.h>   /* first: the macros below would rename its own declarations */
#endif /* _WIN32 */

/**********************************************************************************************/
/* Name: GAM_sdl_fn_ulWaitDrawSemaphore */
/* Goal: WaitForSingleObject on the draw semaphore: WAIT_OBJECT_0 once taken, WAIT_TIMEOUT */
/*       when ulMilliseconds (INFINITE: no limit) ran out first */
/**********************************************************************************************/
unsigned long __cdecl GAM_sdl_fn_ulWaitDrawSemaphore(void *hSemaphore, unsigned long ulMilliseconds);

/**********************************************************************************************/
/* Name: GAM_sdl_fn_bReleaseDrawSemaphore */
/* Goal: ReleaseSemaphore on the draw semaphore (maximum count 1): FALSE, and no change, */
/*       when the count would go above it */
/**********************************************************************************************/
int __cdecl GAM_sdl_fn_bReleaseDrawSemaphore(void *hSemaphore, long lReleaseCount,
                                             long *p_lPreviousCount);

#define WaitForSingleObject  GAM_sdl_fn_ulWaitDrawSemaphore
#define ReleaseSemaphore     GAM_sdl_fn_bReleaseDrawSemaphore

#endif /* USE_SDL */
