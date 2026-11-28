#include "GAM/MSSound.h"
#include "GAM/GAM_types.h"
#include "SND/SND.h"

struct tdstMSSound_
{
    RollOffParam stRollOff;

};

/* 0x0042DB20 */
void __cdecl fn_vMSSoundSetSaturationDistance(MS_tdxHandleToMSSound h_MSSound, long xSaturationDistance)
{
    h_MSSound->stRollOff.rDistSatur = xSaturationDistance;
}

/* 0x0042DE40 */
void __cdecl fn_vMSSoundSetBackGroundDistance(MS_tdxHandleToMSSound h_MSSound, long xBackGroundDistance)
{
    h_MSSound->stRollOff.rDistBackGround = xBackGroundDistance;
}

long __cdecl fn_xMSSoundGetSaturationDistance(MS_tdxHandleToMSSound h_MSSound)
{
    return h_MSSound->stRollOff.rDistSatur;
}

long __cdecl fn_xMSSoundGetBackGroundDistance(MS_tdxHandleToMSSound h_MSSound)
{
    return h_MSSound->stRollOff.rDistBackGround;
}

/* 0x00411190 */
extern unsigned long __cdecl GAM_pc_fn_ulGetHolderSize8(void);
extern void *__cdecl fnp_vGameMallocInHLM(unsigned int ulSize);

extern unsigned char __cdecl SAI_fn_bInsertLongInTable(void *_p_vValue, unsigned short _eFlags);
extern unsigned char __cdecl SAI_fn_bRemoveValueFromTable(void *_p_vValue, unsigned short _eFlags);

#define C_Sound_SAIInitFlags ((unsigned short)0x0032)

#define E_uwGameMiniStructObjectNull        ((unsigned short)0xE)
#define E_uwGameMiniStructAllocAlreadyAlloc ((unsigned short)0xD)
#define E_uwGameMiniStructNull              ((unsigned short)0xF)
extern void ERM_fn_vClearLastError(unsigned long ulChannelId);
extern void ERM_fn_vUpdateLastError(unsigned short uwStartOfWarningsId,
                                    unsigned char  ucModuleId,
                                    unsigned long  ulChannelId,
                                    unsigned short uwErrNum,
                                    long           lDebugData,
                                    unsigned char  ucOpenInfoWindow,
                                    unsigned char  ucStopForDebug,
                                    char          *szPersonalMsg);

extern void __cdecl fn_vGameFreeInHLM(void *p_vObject);

void *__cdecl fn_h_MSSoundRealAlloc(void);
void __cdecl fn_vMSSoundInitValueSAI(MS_tdxHandleToMSSound h_MSSound);

/* 0x004156E0 */
void __cdecl fn_vMSSoundAlloc(struct tdstEngineObject_ *p_stObject)
{
    if (p_stObject == NULL)
    {
        ERM_fn_vClearLastError(0);
        ERM_fn_vUpdateLastError(0x30, g_ucGameModuleId, 0,
                                E_uwGameMiniStructObjectNull,
                                -1, 0xff, 0xff, NULL);
    }

    if (p_stObject->h_MSSound != NULL)
    {
        ERM_fn_vClearLastError(0);
        ERM_fn_vUpdateLastError(0x30, g_ucGameModuleId, 0,
                                E_uwGameMiniStructAllocAlreadyAlloc,
                                -1, 0xff, 0xff, NULL);
    }

    p_stObject->h_MSSound = fn_h_MSSoundRealAlloc();
    fn_vMSSoundInitValueSAI(p_stObject->h_MSSound);
}

/* 0x00415760 */
void *__cdecl fn_h_MSSoundRealAlloc(void)
{
    return fnp_vGameMallocInHLM((unsigned int)GAM_pc_fn_ulGetHolderSize8());
}

void __cdecl fn_vMSSoundFreeInitValueSAI(MS_tdxHandleToMSSound h_MSSound);

/* 0x00415770 */
void __cdecl fn_vMSSoundFree(struct tdstEngineObject_ *p_stObject)
{
    if (p_stObject == NULL)
    {
        ERM_fn_vClearLastError(0);
        ERM_fn_vUpdateLastError(0x30, g_ucGameModuleId, 0,
                                E_uwGameMiniStructObjectNull,
                                -1, 0xff, 0xff, NULL);
    }

    if (p_stObject->h_MSSound == NULL)
    {
        ERM_fn_vClearLastError(0);
        ERM_fn_vUpdateLastError(0x30, g_ucGameModuleId, 0,
                                E_uwGameMiniStructNull,
                                -1, 0xff, 0xff, NULL);
    }

    fn_vMSSoundFreeInitValueSAI(p_stObject->h_MSSound);
    fn_vGameFreeInHLM(p_stObject->h_MSSound);
    p_stObject->h_MSSound = NULL;
}

#define OTI_WhenGameStart   0
#define OTI_MapLoaded       1
#define OTI_ReinitTheMap    2
#define OTI_LoadSavedGame   3
#define OTI_PlayerDead      4

void __cdecl fn_vMSSoundSavedValueSAI(void *h_MSSound);

/* 0x00415800 */
void __cdecl fn_vMSSoundInit(struct tdstEngineObject_ *p_stObject, int eObjectInit)
{
    if (p_stObject == NULL)
    {
        ERM_fn_vClearLastError(0);
        ERM_fn_vUpdateLastError(0x30, g_ucGameModuleId, 0,
                                E_uwGameMiniStructObjectNull,
                                -1, 0xff, 0xff, NULL);
    }

    if (p_stObject->h_MSSound == NULL)
    {
        ERM_fn_vClearLastError(0);
        ERM_fn_vUpdateLastError(0x30, g_ucGameModuleId, 0,
                                E_uwGameMiniStructNull,
                                -1, 0xff, 0xff, NULL);
    }

    switch (eObjectInit)
    {
    case OTI_WhenGameStart:
        break;
    case OTI_ReinitTheMap:
        break;
    case OTI_LoadSavedGame:
        break;
    case OTI_MapLoaded:
        fn_vMSSoundInitValueSAI(p_stObject->h_MSSound);
        fn_vMSSoundSavedValueSAI(p_stObject->h_MSSound);
        break;
    case OTI_PlayerDead:
        break;
    default:
        break;
    }
}

/* 0x00415890 */
void __cdecl fn_vMSSoundDesinit(struct tdstEngineObject_ *p_stObject, int eObjectInit)
{
    (void)eObjectInit;
    if (p_stObject == NULL)
    {
        ERM_fn_vClearLastError(0);
        ERM_fn_vUpdateLastError(0x30, g_ucGameModuleId, 0,
                                E_uwGameMiniStructObjectNull,
                                -1, 0xff, 0xff, NULL);
    }

    if (p_stObject->h_MSSound == NULL)
    {
        ERM_fn_vClearLastError(0);
        ERM_fn_vUpdateLastError(0x30, g_ucGameModuleId, 0,
                                E_uwGameMiniStructNull,
                                -1, 0xff, 0xff, NULL);
    }
}

/* 0x00415900 */
void __cdecl fn_vMSSoundCopyClone(struct tdstEngineObject_ *p_stObjectDest, struct tdstEngineObject_ *p_stObjectSrc)
{
    memcpy(p_stObjectDest->h_MSSound,
           p_stObjectSrc->h_MSSound,
           GAM_pc_fn_ulGetHolderSize8());
}

/* 0x00415930 */
int __cdecl fn_bRetRollOffFactor(long _lObj, struct _RollOffParam *_p_stRolloff)
{
    MS_tdxHandleToMSSound hMSSound;

    if (!_lObj || _lObj == SND_C_OBJET_FANTOME)
        return FALSE;

    hMSSound = M_GetMSHandle((HIE_tdstSuperObject *)_lObj, MSSound);
    if (!hMSSound)
        return FALSE;
    _p_stRolloff->rDistSatur      = hMSSound->stRollOff.rDistSatur;
    _p_stRolloff->rDistBackGround = hMSSound->stRollOff.rDistBackGround;
    return TRUE;
}

/* 0x00415960 */
void __cdecl fn_vMSSoundInitValueSAI(MS_tdxHandleToMSSound h_MSSound)
{
    SAI_fn_bInsertLongInTable((void *)&h_MSSound->stRollOff.rDistSatur, C_Sound_SAIInitFlags);
    SAI_fn_bInsertLongInTable((void *)&h_MSSound->stRollOff.rDistBackGround, C_Sound_SAIInitFlags);
}

/* 0x00415980 */
void __cdecl fn_vMSSoundFreeInitValueSAI(MS_tdxHandleToMSSound h_MSSound)
{
    SAI_fn_bRemoveValueFromTable((void *)&h_MSSound->stRollOff.rDistSatur, C_Sound_SAIInitFlags);
    SAI_fn_bRemoveValueFromTable((void *)&h_MSSound->stRollOff.rDistBackGround, C_Sound_SAIInitFlags);
}

void __cdecl fn_vMSSoundSavedValueSAI(void *h_MSSound)
{
    (void)h_MSSound;
}
