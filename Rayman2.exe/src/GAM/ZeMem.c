#include "GAM/GAM_types.h"
#include "MMG/MMG_alloc.h"

extern void __cdecl Mmg_fn_cCheckAlignmentValidity(void);

/* 0x004077A0 */
void __cdecl fn_vFirstInitGameMemory(void)
{
    Mmg_fn_v_InitMmg(g_ucGameModuleId, 2, 0x186a0);
    Mmg_fn_cCheckAlignmentValidity();
}
#include "GAM/GAM_globals.h"

void __cdecl GAM_fn_vSetGameMemoryInLevel(void)
{
    GAM_g_ucHLMMemoryBlock = 1;
}
#include "ERM/CpaError.h"

void __cdecl fn_vGameFreeInHLM(void *p_vObject)
{
    if (p_vObject == (void *)0)
    {
        ERM_fn_vUpdateLastError(0x30, g_ucGameModuleId, 0, 0x46, -1, 0, 0, (char *)0);
        ERM_fn_vClearLastError(0);
        return;
    }
    Mmg_fn_v_SetModeAlloc(g_ucGameModuleId, GAM_g_ucHLMMemoryBlock, 0);
    fn_vGenFree(p_vObject, 0);
}
