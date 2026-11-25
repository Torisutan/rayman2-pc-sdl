/*
 =======================================================================================
  Name        : sdl_mmg.c
  Description : The MMG lock through SDL (USE_SDL)

  The SDL counterpart of the critical section the PC build takes in Mmg_MCreateCriticalSection,
  Mmg_MEnterCriticalSection, Mmg_MLeaveCriticalSection and Mmg_MDeleteCriticalSection
  (MMG/CpaMmg.h): an SDL mutex in g_csMmg (TypeCriticalSection, MMG/MMG_alloc.h). Like the
  critical section, it may be taken again by the thread that holds it.
 =======================================================================================
 */
#ifdef USE_SDL

#include <SDL3/SDL_mutex.h>

#include "MMG/MMG_alloc.h"
#include "MMG/CpaMmg.h"

/*
 ----------------------------------------------------------------------------------------
  Description   : Create the MMG lock (InitializeCriticalSection on PC)
 ----------------------------------------------------------------------------------------
 */
void __cdecl MMG_sdl_fn_vCreateCriticalSection(void)
{
    g_csMmg = SDL_CreateMutex();
}

/*
 ----------------------------------------------------------------------------------------
  Description   : Take the MMG lock (EnterCriticalSection on PC)
 ----------------------------------------------------------------------------------------
 */
void __cdecl MMG_sdl_fn_vEnterCriticalSection(void)
{
    SDL_LockMutex(g_csMmg);
}

/*
 ----------------------------------------------------------------------------------------
  Description   : Give the MMG lock back (LeaveCriticalSection on PC)
 ----------------------------------------------------------------------------------------
 */
void __cdecl MMG_sdl_fn_vLeaveCriticalSection(void)
{
    SDL_UnlockMutex(g_csMmg);
}

/*
 ----------------------------------------------------------------------------------------
  Description   : Delete the MMG lock (DeleteCriticalSection on PC)
 ----------------------------------------------------------------------------------------
 */
void __cdecl MMG_sdl_fn_vDeleteCriticalSection(void)
{
    SDL_DestroyMutex(g_csMmg);
    g_csMmg = NULL;
}

#endif /* USE_SDL */
