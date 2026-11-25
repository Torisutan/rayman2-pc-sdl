/*
 =======================================================================================
  Name        : sdl_mmg.c
  Description : The MMG lock and the calling thread's id through SDL (USE_SDL)

  The SDL counterpart of the critical section the PC build takes in Mmg_MCreateCriticalSection,
  Mmg_MEnterCriticalSection, Mmg_MLeaveCriticalSection and Mmg_MDeleteCriticalSection
  (MMG/CpaMmg.h): an SDL mutex in g_csMmg (TypeCriticalSection, MMG/MMG_alloc.h). Like the
  critical section, it may be taken again by the thread that holds it.

  MMG_pc_M_ulGetCurrentThreadId (MMG/MMG_pubaccess.h), GetCurrentThreadId on PC: SDL's id of
  the calling thread, the system's thread id on Windows (the sound code still asks for its
  channel with GetCurrentThreadId; both give the same id, so the same channel).
 =======================================================================================
 */
#ifdef USE_SDL

#include <SDL3/SDL_mutex.h>
#include <SDL3/SDL_thread.h>

#include "MMG/MMG_alloc.h"
#include "MMG/CpaMmg.h"
#include "MMG/MMG_pubaccess.h"

/*
 ----------------------------------------------------------------------------------------
  Description   : Create the MMG lock (InitializeCriticalSection on PC)
 ----------------------------------------------------------------------------------------
 */
void __cdecl MMG_fn_vCreateCriticalSection(void)
{
    g_csMmg = SDL_CreateMutex();
}

/*
 ----------------------------------------------------------------------------------------
  Description   : Take the MMG lock (EnterCriticalSection on PC)
 ----------------------------------------------------------------------------------------
 */
void __cdecl MMG_fn_vEnterCriticalSection(void)
{
    SDL_LockMutex(g_csMmg);
}

/*
 ----------------------------------------------------------------------------------------
  Description   : Give the MMG lock back (LeaveCriticalSection on PC)
 ----------------------------------------------------------------------------------------
 */
void __cdecl MMG_fn_vLeaveCriticalSection(void)
{
    SDL_UnlockMutex(g_csMmg);
}

/*
 ----------------------------------------------------------------------------------------
  Description   : Delete the MMG lock (DeleteCriticalSection on PC)
 ----------------------------------------------------------------------------------------
 */
void __cdecl MMG_fn_vDeleteCriticalSection(void)
{
    SDL_DestroyMutex(g_csMmg);
    g_csMmg = NULL;
}

/*
 ----------------------------------------------------------------------------------------
  Description   : Id of the calling thread, for its MMG channel (GetCurrentThreadId on PC)
 ----------------------------------------------------------------------------------------
 */
unsigned long __cdecl MMG_fn_ulGetCurrentThreadId(void)
{
    return (unsigned long)SDL_GetCurrentThreadID();
}

#endif /* USE_SDL */
