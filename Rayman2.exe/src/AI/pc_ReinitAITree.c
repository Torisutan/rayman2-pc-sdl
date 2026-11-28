#include "AI/AI_types.h"
#include "HIE/HIE_types.h"
#include "GAM/GAM_types.h"
#include "AI/AI_globals.h"

extern void __cdecl fn_vReinitAIWithFlags(AI_tdstMind *p_stMind, unsigned long eObjIniType);

void __cdecl AI_pc_fn_vReinitAIInTree(HIE_tdstSuperObject *p_stSO);

void __cdecl AI_pc_fn_vReinitAIInDynamicWorlds(void)
{
    AI_pc_fn_vReinitAIInTree(gp_stDynamicWorld);
    AI_pc_fn_vReinitAIInTree(gp_stInactiveDynamicWorld);
}

void __cdecl AI_pc_fn_vReinitAIInTree(HIE_tdstSuperObject *p_stSO)
{
    if (p_stSO->ulTypeOfLinkedObject == 2)
    {
        HIE_tdstEngineObject *p_stEngineObject = (HIE_tdstEngineObject *)p_stSO->hLinkedObject.p_Void;

        if (p_stEngineObject->h_Brain != NULL)
        {
            fn_vReinitAIWithFlags(AI_M_pstGetMindOfBrain((AI_tdstBrain *)p_stEngineObject->h_Brain), 0);
            return;
        }
    }
    else
    {
        HIE_tdstSuperObject *p_stChild;
        HIE_tdstSuperObject *p_stNextChild;

        /* 0x0047DA07 */
        for (p_stChild = p_stSO->p_stFirstChild,
             p_stNextChild = HIE_fn_hForceGetSuperObjectNextBrother(p_stChild);
             p_stChild != NULL;
             p_stChild = p_stNextChild,
             p_stNextChild = HIE_fn_hForceGetSuperObjectNextBrother(p_stNextChild))
        {
            AI_pc_fn_vReinitAIInTree(p_stChild);
        }
    }
}
