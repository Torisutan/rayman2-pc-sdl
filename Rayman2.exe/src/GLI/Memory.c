#include "GLI/Memory.h"
#include "MMG/MMG_alloc.h"
#include "MMG/MMG_globals.h"

#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <string.h>

#include "ERM/CpaError.h"

#include "GLI/ErrGLI.h"

void __cdecl GLI_pc_fn_vFree(void **ppUser);

/* 0x0045DBE0 */
void __cdecl GLI_pc_fn_vInitGliBlock0(uint32_t ulSize)
{
    Mmg_fn_vInitSpecificBlock(0, g_ucGliModuleId, g_a_p_stMmgModuleBlocksInfo[g_ucGliModuleId],
                              ulSize, 100000, 1, 0);
    MMG_g_ucActiveMode = 0;
}

/* 0x0045DC20 */
void __cdecl GLI_pc_fn_vInitGliBlock1(uint32_t ulSize)
{
    Mmg_fn_vInitSpecificBlock(1, g_ucGliModuleId, g_a_p_stMmgModuleBlocksInfo[g_ucGliModuleId],
                              ulSize, 100000, 1, 0);
    MMG_g_ucActiveMode = 1;
}

/* 0x516148 */
void *__cdecl GLI_pc_fn_p_vAlloc(uint32_t ulSize)
{
    void *pUser = NULL;

    Mmg_fn_v_SetModeAlloc(g_ucGliModuleId, MMG_g_ucActiveMode, 0);
    if (MMG_g_bAllocDisabled == 0) {
        pUser = fn_p_vGenAlloc(ulSize, 0);
        if (pUser != NULL) {
            goto zero_allocation;
        }
    }

    ERM_fn_vUpdateLastError(5, g_ucGliModuleId, 0, 6, -1, 0xff, 0xff, NULL);

zero_allocation:

    memset(pUser, 0, ulSize);
    return pUser;
}

void __cdecl GLI_pc_fn_vFree(void **ppUser)
{
    if (*ppUser == NULL) {
        ERM_fn_vUpdateLastError(5, g_ucGliModuleId, 0, 9, -1, 0, 0, NULL);
        ERM_fn_vClearLastError(0);
    }

    Mmg_fn_v_SetModeAlloc(g_ucGliModuleId, MMG_g_ucActiveMode, 0);
    fn_vGenFree(*ppUser, 0);
    *ppUser = NULL;
}
