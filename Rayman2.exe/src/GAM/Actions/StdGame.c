#include <stddef.h>

#include "GAM/GAM_types.h"
#include "GAM/GAM_globals.h"
#include "ERM/CpaError.h"
#include "SAI/SAI.h"
#include "GAM/PlayAnim.h"
#include <string.h>

static __inline void fn_v3dDataSetAbsoluteMatrix(MS_tdxHandleTo3dData _h_3dData,POS_tdstCompletePosition *_p_stMatrix)
{
	_h_3dData->p_stGLIObjectAbsoluteMatrix = _p_stMatrix;
}

#define C_TRANSPARENCY_ZONE_MIN     70
#define C_TRANSPARENCY_ZONE_MAX     80
#define C_TOOFARLIMIT_INIT          70

#define GAM_C_CustBitUsesTransparencyZone 0x00200000UL

#define GAM_C_CustBitRayman         0x80000000UL

#define GAM_C_SavedMiscFlags        (GAM_C_DesactivateAtAll|GAM_C_Activable|GAM_C_Active)

#define E_uwGameMiniStructObjectNull        ((unsigned short)0xE)
#define E_uwGameMiniStructAllocAlreadyAlloc ((unsigned short)0xD)
#define E_uwGameMiniStructNull              ((unsigned short)0xF)

void __cdecl fn_vStdGameInitValueSAI(MS_tdxHandleToStandardGame h_StdGame);

/* 0x00413200 */
extern void __cdecl fn_vGameFreeInHLM(void *p_vObject);

#define OTI_WhenGameStart   0
#define OTI_MapLoaded       1
#define OTI_ReinitTheMap    2
#define OTI_LoadSavedGame   3
#define OTI_PlayerDead      4
#define OTI_AlwaysCreated   5

unsigned char __cdecl fn_ucIsAnAlwaysObject(void *p_stEngineObject);     /* 0x0040BC60 */
unsigned char __cdecl fn_ucIsAnAlwaysModel(void *p_stEngineObject);      /* 0x0040BC80 */
ACP_tdxBool   __cdecl fn_bIsInFix(struct tdstEngineObject_ *p_stObject); /* 0x004123D0 */
void          __cdecl fn_vSectInfoInitForSingleCharacter(
                  struct HIE_tdstSuperObject *hCharacter, MTH3D_tdstVector *hPosition); /* 0x00412670 */

/* 0x00413130 */
unsigned long __cdecl fn_ulStdGameSizeOf(void)
{
    return(sizeof(struct tdstStandardGame_));
}

MS_tdxHandleToStandardGame __cdecl fn_h_StdGameRealAlloc(void);

/* 0x00413140 */
void __cdecl fn_vStdGameAlloc(struct tdstEngineObject_ *p_stObject)
{
    if (p_stObject == NULL)
    {
        ERM_fn_vClearLastError(0);
        ERM_fn_vUpdateLastError(0x30, g_ucGameModuleId, 0,
                                E_uwGameMiniStructObjectNull, -1, 0xff, 0xff, NULL);
    }

    if (p_stObject->h_StandardGame != NULL)
    {
        ERM_fn_vClearLastError(0);
        ERM_fn_vUpdateLastError(0x30, g_ucGameModuleId, 0,
                                E_uwGameMiniStructAllocAlreadyAlloc, -1, 0xff, 0xff, NULL);
    }

    p_stObject->h_StandardGame = fn_h_StdGameRealAlloc();
    fn_vStdGameInitValueSAI(p_stObject->h_StandardGame);
}

/* 0x004131C0 */
MS_tdxHandleToStandardGame __cdecl fn_h_StdGameRealAlloc(void)
{
    MS_tdxHandleToStandardGame h_StdGame;

    h_StdGame = (MS_tdxHandleToStandardGame)fnp_vGameMallocInHLM((unsigned int)fn_ulStdGameSizeOf());
    fn_vStandardGameSetCustomBits(h_StdGame,(unsigned long)GAM_C_CustBitUsesTransparencyZone);

    h_StdGame->ucTransparencyZoneMin = C_TRANSPARENCY_ZONE_MIN;

    h_StdGame->ucTransparencyZoneMax = C_TRANSPARENCY_ZONE_MAX;
    fn_vStandardGameSetIsDesactivateAtAll(h_StdGame,0);
    fn_vStandardGameSetIsActivable(h_StdGame,1);

    h_StdGame->ucHitPoints = 0;

    h_StdGame->ucHitPointsMax = 0;

    h_StdGame->ucTooFarLimit = C_TOOFARLIMIT_INIT;
    return(h_StdGame);
}

void __cdecl SAI_pc_fn_vRemoveBoolIfType5(MS_tdxHandleToStandardGame h_StdGame);
void __cdecl fn_vStdGamePlayerFreeInitValueSAI(MS_tdxHandleToStandardGame h_StdGame);

void __cdecl fn_vStdGameFree(struct tdstEngineObject_ *p_stObject)
{
    if (p_stObject == NULL)
    {
        ERM_fn_vClearLastError(0);
        ERM_fn_vUpdateLastError(0x30, g_ucGameModuleId, 0,
                                E_uwGameMiniStructObjectNull, -1, 0xff, 0xff, NULL);
    }

    if (p_stObject->h_StandardGame == NULL)
    {
        ERM_fn_vClearLastError(0);
        ERM_fn_vUpdateLastError(0x30, g_ucGameModuleId, 0,
                                E_uwGameMiniStructNull, -1, 0xff, 0xff, NULL);
    }

    SAI_pc_fn_vRemoveBoolIfType5(p_stObject->h_StandardGame);
    if ((void *)p_stObject == HIE_fn_hGetSuperObjectObject(GAM_pc_g_hMainCharacterSO))
        fn_vStdGamePlayerFreeInitValueSAI(p_stObject->h_StandardGame);
    fn_vGameFreeInHLM(p_stObject->h_StandardGame);
    p_stObject->h_StandardGame = NULL;
}

void __cdecl SAI_pc_fn_vInsertBoolIfType5(MS_tdxHandleToStandardGame h_StdGame);
void __cdecl fn_vStdGamePlayerSavedValueSAI(MS_tdxHandleToStandardGame h_StdGame);
static __inline void fn_vStandardGameSetLastTrame(MS_tdxHandleToStandardGame h_StdGame, unsigned long ulLastTrame);
static __inline unsigned char fn_bIsAllSecondPassDone(MS_tdxHandleToStandardGame h_StdGame);
static __inline POS_tdstCompletePosition *fn_p_st3dDataGetAbsoluteMatrix(MS_tdxHandleTo3dData _h_3dData);

/* 0x004132A0 */
void __cdecl fn_vStdGameInit(struct tdstEngineObject_ *p_stObject, int eObjectInit)
{
    struct MTH3D_tdstVector_ hPosition;
    POS_tdstCompletePosition *hMatrix;
    ACP_tdxBool bIsAnAlways;

    if (p_stObject == NULL)
    {
        ERM_fn_vClearLastError(0);
        ERM_fn_vUpdateLastError(0x30, g_ucGameModuleId, 0,
                                E_uwGameMiniStructObjectNull, -1, 0xff, 0xff, NULL);
    }

    if (p_stObject->h_StandardGame == NULL)
    {
        ERM_fn_vClearLastError(0);
        ERM_fn_vUpdateLastError(0x30, g_ucGameModuleId, 0,
                                E_uwGameMiniStructNull, -1, 0xff, 0xff, NULL);
    }

    fn_vStandardGameSetLastTrame(p_stObject->h_StandardGame,0);

    bIsAnAlways = (ACP_tdxBool)fn_ucIsAnAlwaysObject(p_stObject);

    if (eObjectInit == OTI_ReinitTheMap || eObjectInit == OTI_PlayerDead ||
        (eObjectInit == OTI_MapLoaded && fn_bIsInFix(p_stObject)))
        p_stObject->h_StandardGame->ulCustomBits = p_stObject->h_StandardGame->ulSaveCustomBits;

    switch (eObjectInit)
    {
    case OTI_ReinitTheMap:
        if (!fn_ucIsAnAlwaysModel(p_stObject))
        {
            fn_vStandardGameSetIsDesactivateAtAll(p_stObject->h_StandardGame, 0);
            fn_vStandardGameSetIsActivable(p_stObject->h_StandardGame, 1);
            hMatrix = fn_p_st3dDataGetAbsoluteMatrix((MS_tdxHandleTo3dData)p_stObject->h_3dData);
            POS_fn_vGetTranslationVector(hMatrix, &hPosition);
            fn_vSectInfoInitForSingleCharacter(M_GetSuperObject(p_stObject), &hPosition);
        }
        break;
    case OTI_AlwaysCreated:
        if (!fn_ucIsAnAlwaysModel(p_stObject))
        {
            fn_vStandardGameSetIsActivable(p_stObject->h_StandardGame, 1);
            hMatrix = fn_p_st3dDataGetAbsoluteMatrix((MS_tdxHandleTo3dData)p_stObject->h_3dData);
            POS_fn_vGetTranslationVector(hMatrix, &hPosition);
            fn_vSectInfoInitForSingleCharacter(M_GetSuperObject(p_stObject), &hPosition);
        }
        break;
    case OTI_PlayerDead:
        if (!fn_ucIsAnAlwaysModel(p_stObject) &&
            (!fn_bf1StandardGameGetIsDesactivateAtAll(p_stObject->h_StandardGame) ||
             fn_bf1StandardGameGetIsActivable(p_stObject->h_StandardGame)))
        {
            fn_vStandardGameSetIsDesactivateAtAll(p_stObject->h_StandardGame, 0);
            fn_vStandardGameSetIsActivable(p_stObject->h_StandardGame, 1);
            hMatrix = fn_p_st3dDataGetAbsoluteMatrix((MS_tdxHandleTo3dData)p_stObject->h_3dData);
            POS_fn_vGetTranslationVector(hMatrix, &hPosition);
            fn_vSectInfoInitForSingleCharacter(M_GetSuperObject(p_stObject), &hPosition);
        }
        break;
    case OTI_LoadSavedGame:
        if (!fn_ucIsAnAlwaysModel(p_stObject))
        {
            if (fn_eStandardGameGetInitFlagWhenDeadOrTaken(p_stObject->h_StandardGame) == OI_NeverBackWhenTaken)
            {
                fn_vStandardGameSetIsActivable(p_stObject->h_StandardGame,
                    (unsigned char)(!fn_bf1StandardGameGetIsDesactivateAtAll(p_stObject->h_StandardGame)));
            }
            hMatrix = fn_p_st3dDataGetAbsoluteMatrix((MS_tdxHandleTo3dData)p_stObject->h_3dData);
            POS_fn_vGetTranslationVector(hMatrix, &hPosition);
            fn_vSectInfoInitForSingleCharacter(M_GetSuperObject(p_stObject), &hPosition);
        }
        break;
    case OTI_MapLoaded:
        SAI_pc_fn_vInsertBoolIfType5(p_stObject->h_StandardGame); /* 0x004135C0 */
        fn_vStandardGameSetIsActivable(p_stObject->h_StandardGame, 1);

        if (fn_ulStandardGameGetCustomBits(p_stObject->h_StandardGame) & GAM_C_CustBitRayman)
            fn_vStdGamePlayerSavedValueSAI(p_stObject->h_StandardGame);

    case OTI_WhenGameStart:
        if (!fn_bIsAllSecondPassDone(p_stObject->h_StandardGame))
            fn_vStdGameInitValueSAI(p_stObject->h_StandardGame);
        break;
    }

    {
        MS_tdxHandleToStandardGame h_StdGame = p_stObject->h_StandardGame;

        if (fn_ulStandardGameGetCustomBits(h_StdGame) & GAM_C_CustBitRayman)
        {

            if (eObjectInit == OTI_WhenGameStart)
            {
                h_StdGame->ucHitPoints = h_StdGame->ucSaveHitPoints;
            }
        }
        else
        {
            if (eObjectInit == OTI_ReinitTheMap || eObjectInit == OTI_PlayerDead || eObjectInit == OTI_WhenGameStart)
                h_StdGame->ucHitPoints = h_StdGame->ucSaveHitPoints;
        }

        if (eObjectInit == OTI_ReinitTheMap || eObjectInit == OTI_MapLoaded)
        {
            h_StdGame->ucMiscFlags = (unsigned char)((h_StdGame->ucMiscFlags & ~GAM_C_SavedMiscFlags) | h_StdGame->ucSaveMiscFlags);
        }
    }
}

/* 0x00413520 */
void __cdecl fn_vStdGameDesinit(struct tdstEngineObject_ *p_stObject, int eObjectInit)
{
    if (p_stObject == NULL)
    {
        ERM_fn_vClearLastError(0);
        ERM_fn_vUpdateLastError(0x30, g_ucGameModuleId, 0,
                                E_uwGameMiniStructObjectNull, -1, 0xff, 0xff, NULL);
    }

    if (p_stObject->h_StandardGame == NULL)
    {
        ERM_fn_vClearLastError(0);
        ERM_fn_vUpdateLastError(0x30, g_ucGameModuleId, 0,
                                E_uwGameMiniStructNull, -1, 0xff, 0xff, NULL);
    }

    switch (eObjectInit)
    {
    case 1:
    case 2:
    case 3:
    case 4:
    case 5:
        p_stObject->h_StandardGame->ucMiscFlags &= (unsigned char)~GAM_C_Activable;
        break;
    default:
        break;
    }
}

/* 0x004135A0 */
void __cdecl fn_vStdGameInitValueSAI(MS_tdxHandleToStandardGame h_StdGame)
{
    h_StdGame->ucSaveHitPoints    = h_StdGame->ucHitPoints;
    h_StdGame->ucSaveHitPointsMax = h_StdGame->ucHitPointsMax;
    h_StdGame->ucSaveMiscFlags    = (unsigned char)(h_StdGame->ucMiscFlags & GAM_C_SavedMiscFlags);
    h_StdGame->ulSaveCustomBits   = h_StdGame->ulCustomBits;
}

/* 0x004135C0 */
void __cdecl SAI_pc_fn_vInsertBoolIfType5(MS_tdxHandleToStandardGame h_StdGame)
{

    if (h_StdGame->eInitFlagWhenDeadOrTaken == 5) {
        SAI_fn_bInsertBoolInTable(&h_StdGame->ucMiscFlags, 1, (SAI_tdeFlags)0x200);
    }
}

/* 0x004135E0 */
void __cdecl fn_vStdGamePlayerSavedValueSAI(MS_tdxHandleToStandardGame h_StdGame)
{
    SAI_fn_bInsertCharInTable((void *)&h_StdGame->ucHitPoints, SAI_ePlayerSaveCurrentValue);
    SAI_fn_bInsertCharInTable((void *)&h_StdGame->ucHitPointsMax, SAI_ePlayerSaveCurrentValue);
    SAI_fn_bInsertCharInTable((void *)&h_StdGame->ucHitPointsMaxMax, SAI_ePlayerSaveCurrentValue);
}

/* 0x00413620 */
void __cdecl SAI_pc_fn_vRemoveBoolIfType5(MS_tdxHandleToStandardGame h_StdGame)
{

    if (h_StdGame->eInitFlagWhenDeadOrTaken == 5) {
        SAI_fn_bRemoveValueFromTable(&h_StdGame->ucMiscFlags, (SAI_tdeFlags)0x200);
    }
}

/* 0x00413640 */
void __cdecl fn_vStdGamePlayerFreeInitValueSAI(MS_tdxHandleToStandardGame h_StdGame)
{
    SAI_fn_bRemoveValueFromTable((void *)&h_StdGame->ucHitPoints, SAI_ePlayerSaveCurrentValue);
    SAI_fn_bRemoveValueFromTable((void *)&h_StdGame->ucHitPointsMax, SAI_ePlayerSaveCurrentValue);
    SAI_fn_bRemoveValueFromTable((void *)&h_StdGame->ucHitPointsMaxMax, SAI_ePlayerSaveCurrentValue);
}

/* 0x00413680 */
void __cdecl fn_vSetHitPointsToInitValue(struct tdstEngineObject_ *_p_stObject)
{

   _p_stObject -> h_StandardGame -> ucHitPoints = _p_stObject -> h_StandardGame -> ucSaveHitPoints;

}

/* 0x00413690 */
void __cdecl fn_vSetHitPointsMaxToInitValue(struct tdstEngineObject_ *_p_stObject)
{

   _p_stObject -> h_StandardGame -> ucHitPointsMax = _p_stObject -> h_StandardGame -> ucSaveHitPointsMax;

}

/* 0x004136A0 */
void __cdecl fn_vStdGameCopyClone(struct tdstEngineObject_ *_p_stObjectDest,
                                  struct tdstEngineObject_ *_p_stObjectSrc)
{
    memcpy(_p_stObjectDest->h_StandardGame, _p_stObjectSrc->h_StandardGame,
           fn_ulStdGameSizeOf());
}

/* 0x004136D0 */
struct HIE_tdstSuperObject *__cdecl fn_h_StandardGameGetSuperObject(
    MS_tdxHandleToStandardGame h_StdGame)
{
    return h_StdGame ? h_StdGame->h_SuperObject : NULL;
}

/* 0x004136E0 */
void __cdecl fn_vStandardGameSetSuperObject(
    MS_tdxHandleToStandardGame h_StdGame,
    struct HIE_tdstSuperObject *h_SuperObject)
{
    h_StdGame->h_SuperObject=h_SuperObject;

    if( h_SuperObject )
    {
        fn_v3dDataSetAbsoluteMatrix(
            M_GetMSHandle(h_SuperObject,3dData),
            HIE_fn_hGetSuperObjectGlobalMatrix(h_SuperObject) );
    }
}

/* 0x00413700 */
unsigned long __cdecl fn_ulStandardGameGetCustomBitsSO(struct HIE_tdstSuperObject *hSupObj)
{
    return fn_ulStandardGameGetCustomBits(M_GetMSHandle(hSupObj,StandardGame));
}

/* 0x00413710 */
void __cdecl fn_vStandardGameSetCustomBitsSO(struct HIE_tdstSuperObject *hSupObj, unsigned long ulCustomBits)
{
    fn_vStandardGameSetCustomBits(M_GetMSHandle(hSupObj,StandardGame),ulCustomBits);
}

/* 0x00413730 */
char __cdecl fn_bStandardGameIsAPlatform(struct HIE_tdstSuperObject *hSupObj)
{
    return (ACP_tdxBool) (fn_ucStandardGameGetPlatFormType(M_GetMSHandle(hSupObj,StandardGame))==C_ucPlatFormType_StandardPlatForm);
}

/* 0x00413750 */
unsigned char __cdecl fn_ucStandardGameGetTransparencyZoneMinSO(struct HIE_tdstSuperObject *hSupObj)
{
    register MS_tdxHandleToStandardGame hStdGame = M_GetMSHandle(hSupObj,StandardGame);
    return (unsigned char)(hStdGame ? hStdGame->ucTransparencyZoneMin : C_TRANSPARENCY_ZONE_MIN);
}

/* 0x00413770 */
unsigned char __cdecl fn_ucStandardGameGetTransparencyZoneMaxSO(struct HIE_tdstSuperObject *hSupObj)
{
    register MS_tdxHandleToStandardGame hStdGame = M_GetMSHandle(hSupObj,StandardGame);
    return (unsigned char)(hStdGame ? hStdGame->ucTransparencyZoneMax : C_TRANSPARENCY_ZONE_MAX);
}

/* 0x00413790 */
unsigned char __cdecl fn_ucStdGameCheckValidity(struct HIE_tdstSuperObject *_h_SprObj)
{
    if (M_GetMSHandle(_h_SprObj, StandardGame))
        return (1);
    return (0);
}

/* 0x00419BA0 */
unsigned int GAM_pc_fn_ulStdGameAlwaysActiveViaTwoHops(struct HIE_tdstSuperObject *p_stObjectChain)
{
    return (M_GetMSHandle(p_stObjectChain,StandardGame)->ucMiscFlags & GAM_C_AlwaysActive) >> 6;
}

static __inline void fn_vStandardGameSetLastTrame(MS_tdxHandleToStandardGame h_StdGame, unsigned long ulLastTrame)
{ h_StdGame->ulLastTrame=ulLastTrame; }

static __inline unsigned char fn_bIsAllSecondPassDone(MS_tdxHandleToStandardGame h_StdGame)
    { return(unsigned char)(h_StdGame->ucMiscFlags & GAM_C_AllSecondPassDone ? TRUE : FALSE); }

static __inline POS_tdstCompletePosition *fn_p_st3dDataGetAbsoluteMatrix(MS_tdxHandleTo3dData _h_3dData)
{
    return((_h_3dData->p_stGLIObjectAbsoluteMatrix));
}
