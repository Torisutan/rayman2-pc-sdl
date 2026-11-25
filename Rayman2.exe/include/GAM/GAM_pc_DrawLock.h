#pragma once

#if defined(USE_SDL)
/* One engine thread (GAM/sdl_main.c): nothing to guard, no draw lock, as in the original
   engine's console build (fn_hGetDrawSemaphore gives 0) */
#define GAM_pc_M_ulWaitDrawSemaphore(hSemaphore)
#define GAM_pc_M_bReleaseDrawSemaphore(hSemaphore)
#else /* original Win32 */
#define GAM_pc_M_ulWaitDrawSemaphore(hSemaphore)   WaitForSingleObject((hSemaphore), 0xFFFFFFFF)
#define GAM_pc_M_bReleaseDrawSemaphore(hSemaphore) ReleaseSemaphore((hSemaphore), 1, 0)
#endif /* USE_SDL */
