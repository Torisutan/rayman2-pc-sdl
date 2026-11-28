#include "GAM/GAM_types.h"
#include "MTH/MTH_types.h"
#include "MTH/MTH_flt.h"
#include "ERM/CpaError.h"

#define E_uwGameMiniStructObjectNull        ((unsigned short)0xE)
#define E_uwGameMiniStructAllocAlreadyAlloc ((unsigned short)0xD)
#define E_uwGameMiniStructNull              ((unsigned short)0xF)

extern void *__cdecl fnp_vGameMallocInHLM(unsigned int ulSize);
extern void __cdecl fn_vGameFreeInHLM(void *p_vObject);

struct tdstAnimEffect_
{

  MTH3D_tdstVector stShiftPhase, stShiftMax;

  MTH3D_tdstVector stShiftPlus;
};

typedef struct tdstAnimEffect_ * MS_tdxHandleToAnimEffect;

/* 0x00415430 */
unsigned long __cdecl fn_ulAnimEffectSizeOf(void)
{
    return sizeof(struct tdstAnimEffect_);
}

MS_tdxHandleToAnimEffect __cdecl fn_h_AnimEffectRealAlloc(void);
void __cdecl fn_vAnimEffectInitValueSAI(MS_tdxHandleToAnimEffect h_AnimEffect);

/* 0x00415440 */
void __cdecl fn_vAnimEffectAlloc(struct tdstEngineObject_ *p_stObject)
{
    if (p_stObject == NULL)
    {
        ERM_fn_vClearLastError(0);
        ERM_fn_vUpdateLastError(0x30, g_ucGameModuleId, 0,
                                E_uwGameMiniStructObjectNull,
                                -1, 0xff, 0xff, NULL);
    }

    if (p_stObject->h_AnimEffect != NULL)
    {
        ERM_fn_vClearLastError(0);
        ERM_fn_vUpdateLastError(0x30, g_ucGameModuleId, 0,
                                E_uwGameMiniStructAllocAlreadyAlloc,
                                -1, 0xff, 0xff, NULL);
    }

    p_stObject->h_AnimEffect = fn_h_AnimEffectRealAlloc();
    fn_vAnimEffectInitValueSAI(p_stObject->h_AnimEffect);
}

void __cdecl fn_vAnimEffectSetShiftPhase(MS_tdxHandleToAnimEffect h_AnimEffect, MTH3D_tdstVector _stVector);
void __cdecl fn_vAnimEffectSetShiftMax(MS_tdxHandleToAnimEffect h_AnimEffect, MTH3D_tdstVector _stVector);
void __cdecl fn_vAnimEffectSetShiftPlus(MS_tdxHandleToAnimEffect h_AnimEffect, MTH3D_tdstVector _stVector);

/* 0x004154C0 */
MS_tdxHandleToAnimEffect __cdecl fn_h_AnimEffectRealAlloc(void)
{
    MS_tdxHandleToAnimEffect h_AnimEffect;
    MTH3D_tdstVector stVector = { MTH_C_ZERO, MTH_C_ZERO, MTH_C_ZERO };

    h_AnimEffect = (MS_tdxHandleToAnimEffect)fnp_vGameMallocInHLM((unsigned int)fn_ulAnimEffectSizeOf());
    fn_vAnimEffectSetShiftPhase(h_AnimEffect, stVector);
    fn_vAnimEffectSetShiftMax(h_AnimEffect, stVector);
    fn_vAnimEffectSetShiftPlus(h_AnimEffect, stVector);
    return h_AnimEffect;
}

/* 0x403290 */
void __cdecl fn_vAnimEffectInitValueSAI(MS_tdxHandleToAnimEffect h_AnimEffect)
{
    (void)h_AnimEffect;
}

/* 0x403290 */
void __cdecl fn_vAnimEffectFreeInitValueSAI(MS_tdxHandleToAnimEffect h_AnimEffect)
{
    (void)h_AnimEffect;
}

#define M_GameFreeInHLM(p_vPointer) { fn_vGameFreeInHLM((void *)(p_vPointer)); p_vPointer = NULL; }

void __cdecl fn_vAnimEffectFree(struct tdstEngineObject_ *p_stObject)
{
    if (p_stObject == NULL)
    {
        ERM_fn_vClearLastError(0);
        ERM_fn_vUpdateLastError(0x30, g_ucGameModuleId, 0,
                                E_uwGameMiniStructObjectNull,
                                -1, 0xff, 0xff, NULL);
    }

    if (p_stObject->h_AnimEffect == NULL)
    {
        ERM_fn_vClearLastError(0);
        ERM_fn_vUpdateLastError(0x30, g_ucGameModuleId, 0,
                                E_uwGameMiniStructNull,
                                -1, 0xff, 0xff, NULL);
    }

    fn_vAnimEffectFreeInitValueSAI(p_stObject->h_AnimEffect);
    M_GameFreeInHLM(p_stObject->h_AnimEffect);
}

/* 0x004155E0 */
void __cdecl fn_vAnimEffectDesinit(struct tdstEngineObject_ *p_stObject, int eObjectInit)
{
    (void)eObjectInit;
    if (p_stObject == NULL)
    {
        ERM_fn_vClearLastError(0);
        ERM_fn_vUpdateLastError(0x30, g_ucGameModuleId, 0,
                                E_uwGameMiniStructObjectNull,
                                -1, 0xff, 0xff, NULL);
    }

    if (p_stObject->h_AnimEffect == NULL)
    {
        ERM_fn_vClearLastError(0);
        ERM_fn_vUpdateLastError(0x30, g_ucGameModuleId, 0,
                                E_uwGameMiniStructNull,
                                -1, 0xff, 0xff, NULL);
    }
}

/* 0x00415650 */
void __cdecl fn_vAnimEffectCopyClone(struct tdstEngineObject_ *p_stObjectDest, struct tdstEngineObject_ *p_stObjectSrc)
{
    memcpy(p_stObjectDest->h_AnimEffect,
           p_stObjectSrc->h_AnimEffect,
           fn_ulAnimEffectSizeOf());
}

/* 0x00415680 */
void __cdecl fn_vAnimEffectSetShiftPhase(MS_tdxHandleToAnimEffect h_AnimEffect, MTH3D_tdstVector _stVector)
{
  h_AnimEffect->stShiftPhase = _stVector;
}

/* 0x004156A0 */
void __cdecl fn_vAnimEffectSetShiftMax(MS_tdxHandleToAnimEffect h_AnimEffect, MTH3D_tdstVector _stVector)
{
  h_AnimEffect->stShiftMax = _stVector;
}

/* 0x004156C0 */
void __cdecl fn_vAnimEffectSetShiftPlus(MS_tdxHandleToAnimEffect h_AnimEffect, MTH3D_tdstVector _stVector)
{
  h_AnimEffect->stShiftPlus = _stVector;
}
