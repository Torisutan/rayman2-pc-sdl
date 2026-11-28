#include <stddef.h>

#include "ACP/ACP_types.h"
#include "HIE/HIE_types.h"
#include "AI/AI_types.h"
#include "AI/AI_Proto.h"
#include "GAM/Actions/Brain.h"
#include "GAM/GAM_types.h"
#include "GAM/GAM_globals.h"
#include "GMT/GMT_types.h"

#include "ERM/CpaError.h"
#include "GAM/InitEnum.h"
#include "AI/Brain.h"

#define E_uwGameMiniStructObjectNull       ((unsigned short)0xE)
#define E_uwGameMiniStructAllocAlreadyAlloc ((unsigned short)0xD)
#define E_uwGameMiniStructNull              ((unsigned short)0xF)

/* 0x004078C0 */
extern void __cdecl fn_vGameFreeInHLM(void *p_vObject);

extern void __cdecl fn_vReinitAIWithFlags(AI_tdstMind *p_stMind, unsigned long eObjIniType);
extern void __cdecl AI_fn_vInsertDsgVarInSAI(AI_tdstMind *p_stMind);
extern unsigned char __cdecl fn_bIsInFix(void *p_stObject);

unsigned long __cdecl fn_ulBrainSizeOf(void)
{
    return sizeof(AI_tdstBrain);
}

MS_tdxHandleToBrain __cdecl fn_h_BrainRealAlloc(void);

void __cdecl fn_vBrainAlloc(HIE_tdstEngineObject *p_stObject)
{

    if (p_stObject == NULL)
    {
        ERM_fn_vClearLastError(0);
        ERM_fn_vUpdateLastError(0x30, g_ucGameModuleId, 0,
                                E_uwGameMiniStructObjectNull,
                                -1, 0xff, 0xff, NULL);
    }

    if (p_stObject->h_Brain != NULL)
    {
        ERM_fn_vClearLastError(0);
        ERM_fn_vUpdateLastError(0x30, g_ucGameModuleId, 0,
                                E_uwGameMiniStructAllocAlreadyAlloc,
                                -1, 0xff, 0xff, NULL);
    }

    p_stObject->h_Brain = fn_h_BrainRealAlloc();
}

void __cdecl fn_vFirstInitBrain(MS_tdxHandleToBrain h_Brain);

MS_tdxHandleToBrain __cdecl fn_h_BrainRealAlloc(void)
{
    MS_tdxHandleToBrain h_Brain;

    h_Brain = (MS_tdxHandleToBrain)fnp_vGameMallocInHLM((unsigned int)fn_ulBrainSizeOf());
    fn_vFirstInitBrain(h_Brain);

    return(h_Brain);
}

void __cdecl fn_vBrainFree(HIE_tdstEngineObject *p_stObject)
{
    if (p_stObject == NULL)
    {
        ERM_fn_vClearLastError(0);
        ERM_fn_vUpdateLastError(0x30, g_ucGameModuleId, 0,
                                E_uwGameMiniStructObjectNull,
                                -1, 0xff, 0xff, NULL);
    }

    if (p_stObject->h_Brain == NULL)
    {
        ERM_fn_vClearLastError(0);
        ERM_fn_vUpdateLastError(0x30, g_ucGameModuleId, 0,
                                E_uwGameMiniStructNull,
                                -1, 0xff, 0xff, NULL);
    }

    fn_vGameFreeInHLM((void *)p_stObject->h_Brain);
    p_stObject->h_Brain = NULL;
}

void __cdecl fn_vFirstInitBrain(MS_tdxHandleToBrain h_Brain)
{
    AI_tdstMind *p_stMind = fn_pstAllocMind();
    h_Brain->p_stMind = p_stMind;
    h_Brain->hLastCollidedGoThroughMaterial = GMT_C_ulInvalidHandle;
    h_Brain->bWarnMechanicsFlag = 0;
}

void __cdecl fn_vBrainInit(HIE_tdstEngineObject *p_stObject, unsigned long eObjectInit)
{
    if (p_stObject == NULL)
    {
        ERM_fn_vClearLastError(0);
        ERM_fn_vUpdateLastError(0x30, g_ucGameModuleId, 0,
                                E_uwGameMiniStructObjectNull,
                                -1, 0xff, 0xff, NULL);
    }

    if (p_stObject->h_Brain == NULL)
    {
        ERM_fn_vClearLastError(0);
        ERM_fn_vUpdateLastError(0x30, g_ucGameModuleId, 0,
                                E_uwGameMiniStructNull,
                                -1, 0xff, 0xff, NULL);
    }

    M_vSetLastCollidedGoThroughMaterialOfBrain(p_stObject->h_Brain, GMT_C_ulInvalidHandle);
    M_vSetWarnMechanicsFlagOfBrain(p_stObject->h_Brain, 0);

    switch (eObjectInit)
    {
    case OTI_AlwaysCreated:
        break;
    case OTI_PlayerDead:
    case OTI_ReinitTheMap:
    case OTI_LoadSavedGame:
        fn_vReinitAIWithFlags(M_pstGetMindOfBrain(p_stObject->h_Brain), eObjectInit);
        break;
    case OTI_MapLoaded:
    case OTI_WhenGameStart:
        if ((((MS_tdxHandleToStandardGame)p_stObject->hStandardGame)->ucMiscFlags & GAM_C_AllSecondPassDone) == 0
            && FUN_0044f4e0() != 1 && FUN_0044f4e0() != 4)
            fn_vSecondPassOfMindLoading(M_pstGetMindOfBrain(p_stObject->h_Brain),
                                        fn_bIsInFix(p_stObject), eObjectInit);
        else
            fn_vReinitAI(M_pstGetMindOfBrain(p_stObject->h_Brain));
        AI_fn_vInsertDsgVarInSAI(M_pstGetMindOfBrain(p_stObject->h_Brain));
        break;
    }
}

void __cdecl fn_vBrainDesinit(HIE_tdstEngineObject *p_stObject, int eObjectInit)
{
    if (p_stObject == NULL)
    {
        ERM_fn_vClearLastError(0);
        ERM_fn_vUpdateLastError(0x30, g_ucGameModuleId, 0,
                                E_uwGameMiniStructObjectNull,
                                -1, 0xff, 0xff, NULL);
    }

    if (p_stObject->h_Brain == NULL)
    {
        ERM_fn_vClearLastError(0);
        ERM_fn_vUpdateLastError(0x30, g_ucGameModuleId, 0,
                                E_uwGameMiniStructNull,
                                -1, 0xff, 0xff, NULL);
    }

}

void __cdecl fn_vBrainCopyClone(HIE_tdstEngineObject *p_stObjectDest,
                                   HIE_tdstEngineObject *p_stObjectSrc)
{
    AI_tdstMind *hMindSrc, *hMindDest;
    hMindSrc  = AI_M_pstGetMindOfBrain((AI_tdstBrain *)p_stObjectSrc->h_Brain);
    hMindDest = AI_M_pstGetMindOfBrain((AI_tdstBrain *)p_stObjectDest->h_Brain);
    fn_vSimulateMindAllocationFromAnotherMind(hMindSrc, hMindDest);
}
