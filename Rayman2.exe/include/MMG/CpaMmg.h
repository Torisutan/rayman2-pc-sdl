#ifndef __MMG_CpaMmg_h__
#define __MMG_CpaMmg_h__

#define C_ucMmgDefaultChannel (0)

#define C_ucTrue (0xFF)
#define C_ucFalse (0)

#define C_uwAllocSize (4)
#define C_uwShiftAllocSize (2)

#define unitAlloc unsigned long

#define Mmg_M_BigBoundSize(ulSize, ucAlignment) \
 \
        \
				 \
                if ( (ulSize) & (unsigned long)(ucAlignment-1)) {\
				 \
                  \
                  \
                (ulSize) = ((ulSize) & (~((unsigned long)(ucAlignment-1)))) + ucAlignment;\
                }

#define C_FillTheBlockWithCleanKeyFlag 2
#define C_Check_AlignementFlag 4
#define C_Check_OverflowFlag 8

#define Mmg_M_SetModeAlloc4Ch(ModuleAbreviation, ucNewMode, C_ucMmgDefaultChannel) Mmg_fn_v_SetModeAlloc(g_uc##ModuleAbreviation##ModuleId, ucNewMode, C_ucMmgDefaultChannel)

#define Mmg_fn_p_vAlloc4Ch(ulSize, ucChannel) fn_p_vGenAlloc((ulSize), (ucChannel))

#define Mmg_MEnterCriticalSection() EnterCriticalSection (&g_csMmg)
#define Mmg_MLeaveCriticalSection() LeaveCriticalSection (&g_csMmg)

/* System memory of the MMG blocks, as in the original engine's CpaMmg.c: GlobalAlloc(GMEM_FIXED)
   and GlobalFree on the PC. The MMG files call them written out; without FILE_MODE_C_WIN95 those
   calls go to the C runtime (docs/PLAN.md Phase 5, step 3). */
#include "SND/snddef.h"
#ifdef FILE_MODE_C_WIN95
#define CPA_MALLOC(size)  GlobalAlloc(GMEM_FIXED, size)
#define CPA_FREE(ptr)     GlobalFree(ptr)
#else
#ifdef _WIN32
#include <windows.h>   /* first: the macros below would rename its own declarations */
#endif /* _WIN32 */
#include <stdlib.h>
#define CPA_MALLOC(size)  malloc(size)
#define CPA_FREE(ptr)     free(ptr)
#define GlobalAlloc(uFlags, ulSize)     ((HGLOBAL)(((uFlags) & GMEM_ZEROINIT) ? calloc(1, (ulSize)) : CPA_MALLOC(ulSize)))
#define GlobalFree(hMem)  (CPA_FREE((void *)(hMem)), (HGLOBAL)NULL)
#endif /* FILE_MODE_C_WIN95 */

/* The MMG lock: with USE_SDL an SDL mutex (MMG/MMG_alloc.h). The MMG files call the Win32
   critical section functions on g_csMmg written out, as Mmg_MEnterCriticalSection does;
   those calls go to the mutex. */
#if defined(USE_SDL)
#ifdef _WIN32
#include <windows.h>   /* first: the macros below would rename its own declarations */
#endif /* _WIN32 */
#include <SDL3/SDL_mutex.h>
#define InitializeCriticalSection(p_csLock)  ((void)(*(p_csLock) = SDL_CreateMutex()))
#define EnterCriticalSection(p_csLock)       SDL_LockMutex(*(p_csLock))
#define LeaveCriticalSection(p_csLock)       SDL_UnlockMutex(*(p_csLock))
#define DeleteCriticalSection(p_csLock)      (SDL_DestroyMutex(*(p_csLock)), (void)(*(p_csLock) = NULL))
#endif /* USE_SDL */

#endif
