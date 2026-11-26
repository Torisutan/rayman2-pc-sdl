#include "GAM/GAM_types.h"
#include "GAM/InitEnum.h"
#include "GAM/Family.h"
#include "GAM/GAM_globals.h"
#include "GAM/PlayAnim.h"
#include "SND/SND.h"
#include "ERM/CpaError.h"
#include "MMG/MMG_alloc.h"
#include "ALW/ALW.h"
extern tdstStructureHandleFunction g_a_stStructureHandleFunction[];   /* 0x0049F4C8 */
#include "GLI/GLI_globals.h"
#include "ALW/ALW_globals.h"
#include "SAI/SAI.h"
#include <string.h>

/* 0x0045B760 */
extern void __cdecl HIE_fn_vInitMatrixStack(void);
#define GAM_p_vInitMatrixStack    HIE_fn_vInitMatrixStack
/* 0x00406620 */
void __cdecl fn_vInitAllObjectTree(int eObjIniType, HIE_tdstSuperObject *p_stSupObject);
#define GAM_p_vInitAllObjectTree  ((void (__cdecl *)(int, HIE_tdstSuperObject *))fn_vInitAllObjectTree)
/* 0x0040BC30 */
void __cdecl fn_vInitAlwaysModel(int eObjIniType);
#define GAM_p_vInitAlwaysModel(t) fn_vInitAlwaysModel((t))

#define GAM_C_OTI_MapLoaded       1

/* 0x00403290 */
void fn_vInitAllObjectsWhenNewGameStart(void)
{
}

/* 0x004062F0 */
void __cdecl fn_vInitAllObjectsWhenMapJustLoaded(void)
{
    GAM_p_vInitMatrixStack();
    GAM_p_vInitAllObjectTree(GAM_C_OTI_MapLoaded, gp_stDynamicWorld);
    GAM_p_vInitAlwaysModel(GAM_C_OTI_MapLoaded);
}

#define GAM_C_OTI_ReinitTheMap   2
#define GAM_C_OTI_PlayerDead     4

/* 0x00406310 */
static void fn_vInitAllObjectsWorld(int eObjIniType)
{
    GAM_p_vInitAllObjectTree(eObjIniType, gp_stDynamicWorld);
    GAM_p_vInitAllObjectTree(eObjIniType, gp_stInactiveDynamicWorld);
}

/* 0x00406470 */
extern void __cdecl CAM_fn_vReInitCameras(void);
void __cdecl fn_vForceReinitAllObjects(void)
{
    GAM_p_vInitMatrixStack();
    fn_vInitAllObjectsWorld(GAM_C_OTI_ReinitTheMap);
    GAM_p_vInitAlwaysModel(GAM_C_OTI_ReinitTheMap);
    CAM_fn_vReInitCameras();
}

/* 0x00406600 */
void __cdecl fn_vInitAllObjectsWhenPlayerDead(void)
{
    GAM_p_vInitMatrixStack();
    fn_vInitAllObjectsWorld(GAM_C_OTI_PlayerDead);
    GAM_p_vInitAlwaysModel(GAM_C_OTI_PlayerDead);
}

#define GAM_C_OTI_ReinitTheMap   2
#define GAM_C_OTI_LoadSavedGame  3
#define GAM_C_OTI_AlwaysCreated  4

/* 0x00406340 */
void __cdecl fn_vInitAllObjectsWhenSavedGameJustLoaded(void)
{
    GAM_p_vInitMatrixStack();
    fn_vInitAllObjectsWorld(GAM_C_OTI_LoadSavedGame);
    GAM_p_vInitAlwaysModel(GAM_C_OTI_LoadSavedGame);
}

#ifndef HIE_C_ulActor
#define HIE_C_ulActor            2
#endif
#define HIE_C_ulMaskSuperObjOrNoAction  0x101

extern void HIE_fn_vPopMatrix(void);
#define HIE_p_vPushMatrix(so)                HIE_fn_vPushMatrix((HIE_tdstSuperObject *)(so))
#define HIE_p_vInvalidateOneGlobalMatrix(so) HIE_fn_vInvalidateOneGlobalMatrix((HIE_tdstSuperObject *)(so))
#define HIE_p_vPopMatrix()                   HIE_fn_vPopMatrix()
/* 0x00405E50 */
void __cdecl fn_vInitOneObject(struct tdstEngineObject_ *p_stObject, int eObjIniType);
#define GAM_p_vInitOneObject(o, t)    fn_vInitOneObject((struct tdstEngineObject_ *)(o), (t))
/* 0x00405F00 */
void __cdecl fn_vDesinitOneObject(struct tdstEngineObject_ *p_stObject, int eObjIniType);
#define GAM_p_vDesinitOneObject(o, t)  fn_vDesinitOneObject((struct tdstEngineObject_ *)(o), (t))
/* 0x00412F20 */
void __cdecl fn_vDynamInit(struct tdstEngineObject_ *p_stObject, int eObjIniType);
#define GAM_p_vDynamInit(o, t)        fn_vDynamInit((struct tdstEngineObject_ *)(o), (t))
#define GAM_g_p_stCurrentMatrix       ((unsigned long *)GLI_g_p_stCurrentMatrix)

#define GEO_M_vCopyMatrix(a,b)                    {POS_fn_vCopyMatrix((a),(b));}
#define GAM_M_h3dDataGetFamily(h3d)               ((tdxHandleToFamilyList)(h3d)->h_Family)
#include "HIE/HieSpObj.h"

void __cdecl fn_vInitAllObjectTree(int eObjIniType, HIE_tdstSuperObject *p_stSupObject)
{
    HIE_tdstSuperObject *p_stChild;
    long i;

    tdstEngineObject *p_stObject;
    unsigned long ulSuperObjectType;
    tdxHandleToFamilyList hFamily;
    struct tdst3dData_ *h3dData;

    HIE_p_vPushMatrix(p_stSupObject);
    GEO_M_vCopyMatrix((POS_tdxHandleToPosition)HIE_fn_hGetSuperObjectGlobalMatrix(p_stSupObject),
                      (POS_tdxHandleToPosition)GLI_g_p_stCurrentMatrix);
    HIE_p_vInvalidateOneGlobalMatrix(p_stSupObject);

    HIE_M_ForEachChildOf(p_stSupObject,p_stChild,i)
        fn_vInitAllObjectTree(eObjIniType,p_stChild);

    HIE_p_vPopMatrix();

    p_stObject = (tdstEngineObject *)HIE_fn_hGetSuperObjectObject(p_stSupObject);
    ulSuperObjectType = HIE_fn_ulGetSuperObjectType(p_stSupObject);

    if (p_stObject == NULL && (ulSuperObjectType & (HIE_C_ulSuperObject | HIE_C_ulNoAction)) == 0)
    {
        ERM_fn_vClearLastError(0);
        ERM_fn_vUpdateLastError(0x30, g_ucGameModuleId, 0, 10, -1, 0xff, 0xff, NULL);
    }

    if (ulSuperObjectType == HIE_C_ulActor)
    {
        switch (eObjIniType)
        {
        case OTI_ReinitTheMap:
            fn_vInitOneObject(p_stObject, eObjIniType);
            break;
        case OTI_MapLoaded:
        case OTI_LoadSavedGame:
            if (
                (   fn_eStandardGameGetInitFlagWhenDeadOrTaken(p_stObject->h_StandardGame) == OI_WhenPlayerGoOutOfActionZone
                 || fn_eStandardGameGetInitFlagWhenDeadOrTaken(p_stObject->h_StandardGame) == OI_Always
                 || fn_eStandardGameGetInitFlagWhenDeadOrTaken(p_stObject->h_StandardGame) == OI_WhenPlayerIsDead
                 || fn_eStandardGameGetInitFlagWhenDeadOrTaken(p_stObject->h_StandardGame) == OI_WhenMapJustLoaded
                 || fn_eStandardGameGetInitFlagWhenDeadOrTaken(p_stObject->h_StandardGame) == OI_WhenSavedGameJustLoaded
                )
                ||
                (     fn_eStandardGameGetInitFlagWhenDeadOrTaken(p_stObject->h_StandardGame) == OI_NeverBackWhenTaken
                  && !fn_bf1StandardGameGetIsDesactivateAtAll(p_stObject->h_StandardGame)
                )
               )
            {
                if (eObjIniType==OTI_LoadSavedGame&&fn_eStandardGameGetInitFlagWhenDeadOrTaken(p_stObject->h_StandardGame) == OI_WhenSavedGameJustLoaded)
                    fn_vInitOneObject(p_stObject,OTI_ReinitTheMap);
                else
                    fn_vInitOneObject(p_stObject,eObjIniType);
            }
            else
            {
                fn_vDesinitOneObject(p_stObject,eObjIniType);
            }

            h3dData=((struct tdst3dData_ *)((struct tdstEngineObject_ *)(HIE_fn_hGetSuperObjectObject(p_stSupObject)))->h_3dData);
            hFamily=GAM_M_h3dDataGetFamily(h3dData);
            HIE_fn_vSetSuperObjectBoundingVolume(p_stSupObject, hFamily->hBoundingSphere);

            break;
        case OTI_AlwaysCreated:
        case OTI_PlayerDead:
            if (
                (   fn_eStandardGameGetInitFlagWhenDeadOrTaken(p_stObject->h_StandardGame) == OI_WhenPlayerGoOutOfActionZone
                 || fn_eStandardGameGetInitFlagWhenDeadOrTaken(p_stObject->h_StandardGame) == OI_Always
                 || fn_eStandardGameGetInitFlagWhenDeadOrTaken(p_stObject->h_StandardGame) == OI_WhenPlayerIsDead
                )
                ||
                (	fn_eStandardGameGetInitFlagWhenDeadOrTaken(p_stObject->h_StandardGame) == OI_NeverBackWhenTaken
                 && !fn_bf1StandardGameGetIsDesactivateAtAll(p_stObject->h_StandardGame)
                )
                ||
                (   fn_eStandardGameGetInitFlagWhenDeadOrTaken(p_stObject->h_StandardGame) == OI_WhenMapJustLoaded
                 && !fn_bf1StandardGameGetIsDesactivateAtAll(p_stObject->h_StandardGame)
                )
               )
            {
                fn_vInitOneObject(p_stObject,eObjIniType);
            }
            else
            {
                fn_vDesinitOneObject(p_stObject,eObjIniType);
            }
            break;
        default:
            ERM_fn_vClearLastError(0);
            ERM_fn_vUpdateLastError(0x30, g_ucGameModuleId, 0, 0xb, -1, 0xff, 0xff, NULL);
            break;
        }

        HIE_p_vPushMatrix(p_stSupObject);
        HIE_p_vPopMatrix();

        if (((struct tdstEngineObject_ *)(HIE_fn_hGetSuperObjectObject(p_stSupObject)))->h_Dynam!=NULL)
        {
            fn_vDynamInit(p_stObject,eObjIniType);
        }
    }
}

#define ALW_g_h_LstAlwaysModel  (*(unsigned int **)&g_stAlways.ALW_h_LstAlwaysModel)

/* 0x0040BC30 */
void __cdecl fn_vInitAlwaysModel(int eObjIniType)
{
    unsigned int *p_stNode;

    for (p_stNode = ALW_g_h_LstAlwaysModel; p_stNode != (unsigned int *)0;
         p_stNode = (unsigned int *)*p_stNode) {
        GAM_p_vInitOneObject((void *)p_stNode[4], eObjIniType);
    }
}

void __cdecl fn_vInitOneObject(struct tdstEngineObject_ *p_stObject, int eObjIniType)
{
    unsigned char *p_stHandleFn;
    long          *p_hMiniStruct;

    if (p_stObject->h_3dData == NULL || p_stObject->h_StandardGame == NULL)
    {

        ERM_fn_vClearLastError(0);
        ERM_fn_vUpdateLastError(0x30, g_ucGameModuleId, 0, 0x0C,
                                C_lErmNoDebugData, 0xFF, 0xFF, (char *)0);
    }
    if (*(long *)p_stObject->h_StandardGame == C_InvalidObjectType)
    {

        ERM_fn_vUpdateLastError(0x30, g_ucGameModuleId, 0, 0x47,
                                C_lErmNoDebugData, 0, 0, (char *)0);
        ERM_fn_vClearLastError(0);
    }

    p_stHandleFn  = (unsigned char *)&g_a_stStructureHandleFunction[0].p_fn_vInitFunction;
    p_hMiniStruct = (long *)p_stObject;
    do {
        if (*p_hMiniStruct != 0)
            (*(void (__cdecl **)(struct tdstEngineObject_ *, int))p_stHandleFn)
                (p_stObject, eObjIniType);
        p_stHandleFn  += 0x18;
        p_hMiniStruct += 1;
    } while (p_stHandleFn < (unsigned char *)&g_a_stStructureHandleFunction[14].p_fn_vInitFunction);

    if (eObjIniType == GAM_C_OTI_MapLoaded)
        p_stObject->h_StandardGame->ucMiscFlags |= GAM_C_AllSecondPassDone;
}

void __cdecl fn_vDesinitOneObject(struct tdstEngineObject_ *p_stObject, int eObjIniType)
{
    unsigned char *p_stHandleFn;
    long          *p_hMiniStruct;

    if (p_stObject->h_3dData == NULL || p_stObject->h_StandardGame == NULL)
    {

        ERM_fn_vClearLastError(0);
        ERM_fn_vUpdateLastError(0x30, g_ucGameModuleId, 0, 0x0C,
                                C_lErmNoDebugData, 0xFF, 0xFF, (char *)0);
    }
    if (*(long *)p_stObject->h_StandardGame == C_InvalidObjectType)
    {

        ERM_fn_vUpdateLastError(0x30, g_ucGameModuleId, 0, 0x47,
                                C_lErmNoDebugData, 0, 0, (char *)0);
        ERM_fn_vClearLastError(0);
    }

    p_stHandleFn  = (unsigned char *)&g_a_stStructureHandleFunction[0].p_fn_vDesinitFunction;
    p_hMiniStruct = (long *)p_stObject;
    do {
        if (*p_hMiniStruct != 0)
            (*(void (__cdecl **)(struct tdstEngineObject_ *, int))p_stHandleFn)
                (p_stObject, eObjIniType);
        p_stHandleFn  += 0x18;
        p_hMiniStruct += 1;
    } while (p_stHandleFn < (unsigned char *)&g_a_stStructureHandleFunction[14].p_fn_vDesinitFunction);
}

#include "DNM/DNM_types.h"

extern void __cdecl DNM_fn_vInitBaseMechanic(void *_p_stDynamic);
void __cdecl fn_vDynamInit(struct tdstEngineObject_ *p_stObject, int eObjIniType)
{

    if (eObjIniType >= 0 && eObjIniType <= 4)
    {
        DNM_fn_vInitBaseMechanic(((MS_tdxHandleToDynam)p_stObject->h_Dynam)->p_stDynamics);
        memset(((MS_tdxHandleToDynam)p_stObject->h_Dynam)->p_stParsingDatas, 0, 7 * 4);
    }
}

void __cdecl fn_vDesactiveAnObject(struct tdstEngineObject_ *p_stObject)
{
    PLA_fn_vDesInitAllChildOfCharacter(M_GetSuperObject(p_stObject), p_stObject);
    SND_fn_vKillObjectSound2((long)M_GetSuperObject(p_stObject), GAM_g_lSoundObjectTypeAnim);

    if (!fn_bf1StandardGameGetIsDesactivateAtAll(p_stObject->h_StandardGame) && M_ObjectIsActivable(p_stObject))
    {
        fn_vStandardGameSetIsActivable(p_stObject->h_StandardGame, 0);
        fn_vStandardGameSetIsDesactivateAtAll(p_stObject->h_StandardGame, 0);
        M_UnactivateObject(p_stObject);
    }
}

/* 0x00412670 */
extern void __cdecl fn_vSectInfoInitForSingleCharacter(HIE_tdstSuperObject *hCharacter, MTH3D_tdstVector *hPosition);
/* 0x00418A20 */
extern void __cdecl fn_vAllocArrayOfElts3d(MS_tdxHandleTo3dData h_3dData, unsigned short _wNumberOfElts3d);

/* 0x00405FA0 */
void __cdecl fn_vForceActiveAnObject(struct tdstEngineObject_ *p_stObject)
{
    struct MTH3D_tdstVector_  hPosition;
    POS_tdstCompletePosition *hMatrix;
    HIE_tdstSuperObject      *hFather;

    hFather = HIE_fn_hGetSuperObjectFather(M_GetSuperObject(p_stObject));
    if (hFather == gp_stInactiveDynamicWorld)
    {
        if (fn_ucStandardGameGetPlatFormType(M_GetMSHandle(M_GetSuperObject(p_stObject), StandardGame)) != 0)
        {
            HIE_fn_vSuperObjectAddHead(gp_stDynamicWorld, M_GetSuperObject(p_stObject));
        }
        else
        {
            HIE_fn_vSuperObjectAddTail(gp_stDynamicWorld, M_GetSuperObject(p_stObject));
        }
    }

    hMatrix = HIE_fn_hGetSuperObjectGlobalMatrix(HIE_fn_hGetSuperObjectFather(M_GetSuperObject(p_stObject)));
    POS_fn_vGetTranslationVector(HIE_fn_hGetSuperObjectMatrix(M_GetSuperObject(p_stObject)), &hPosition);
    POS_fn_vMulMatrixVertex(&hPosition, hMatrix, &hPosition);
    fn_vSectInfoInitForSingleCharacter(M_GetSuperObject(p_stObject), &hPosition);
    fn_vStandardGameSetIsActivable(p_stObject->h_StandardGame, 1);
    M_ActivateObject(p_stObject);

    {
        fn_v3dDataSetStartTime(p_stObject->h_3dData, GAM_g_stEngineStructure.stEngineTimer.ulCurrentTimerCount);
        fn_v3dDataSetTimePreviousFrame(p_stObject->h_3dData, GAM_g_stEngineStructure.stEngineTimer.ulCurrentTimerCount);
    }

    if (fn_p_st3dDataGetCurrentFrame(p_stObject->h_3dData)->p_stArrayOfElts3d == NULL)
    {
        tdstAnim3d *p_stCurrentAnim;

        if (fn_h3dDataGetCurrentState(p_stObject->h_3dData))
        {
            p_stCurrentAnim = fn_h3dDataGetCurrentState(p_stObject->h_3dData)->p_stAnim;
            if (p_stCurrentAnim)
            {
                fn_p_st3dDataGetCurrentFrame(p_stObject->h_3dData)->p_stAnim = NULL;
                fn_vAllocArrayOfElts3d(p_stObject->h_3dData, p_stCurrentAnim->ucMaxNumberOfElements);
            }
        }

        fn_p_st3dDataGetCurrentFrame(p_stObject->h_3dData)->p_stAnim = NULL;
    }
}

void __cdecl fn_vKillEngineObjectNotAlways(struct tdstEngineObject_ *p_stObject)
{
    PLA_fn_vDesInitAllChildOfCharacter(M_GetSuperObject(p_stObject), p_stObject);

    if (!fn_bf1StandardGameGetIsDesactivateAtAll(p_stObject->h_StandardGame))
    {

        fn_vStandardGameSetIsDesactivateAtAll(p_stObject->h_StandardGame, 1);

        if (fn_eStandardGameGetInitFlagWhenDeadOrTaken(p_stObject->h_StandardGame) == OI_NeverBackWhenTaken)
            fn_vStandardGameSetIsActivable(p_stObject->h_StandardGame, 0);
        else
            fn_vStandardGameSetIsActivable(p_stObject->h_StandardGame, 1);

        if (fn_eStandardGameGetInitFlagWhenDeadOrTaken(p_stObject->h_StandardGame) == OI_Always)
            GAM_p_vInitOneObject(p_stObject, GAM_C_OTI_AlwaysCreated);
        else
            M_UnactivateObject(p_stObject);
    }

    SND_fn_vKillObjectSound2((long)M_GetSuperObject(p_stObject), GAM_g_lSoundObjectTypeAnim);
}

unsigned long GAM_pc_g_ulNbFixObject_storage;
unsigned long GAM_pc_g_a_ulFixObject_storage[21];
#define GAM_pc_g_ulNbFixObject   GAM_pc_g_ulNbFixObject_storage
#define GAM_pc_g_a_ulFixObject   GAM_pc_g_a_ulFixObject_storage

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

struct tdstEngineObject_ *__cdecl fn_p_stFreeEngineObject(struct tdstEngineObject_ *p_stObject)
{
    unsigned char *p_stHandleFn;
    long          *p_hMiniStruct;

    p_stHandleFn  = (unsigned char *)&g_a_stStructureHandleFunction[13].p_fn_vFreeFunction;
    p_hMiniStruct = (long *)&p_stObject->h_MSMagnet;
    do {
        if (*p_hMiniStruct != 0)
            (*(void (__cdecl **)(struct tdstEngineObject_ *))p_stHandleFn)(p_stObject);
        p_stHandleFn  -= 0x18;
        p_hMiniStruct -= 1;
    } while ((long)p_stHandleFn >= (long)&g_a_stStructureHandleFunction[0].p_fn_vFreeFunction);

    fn_vGameFreeInHLM(p_stObject);
    return (struct tdstEngineObject_ *)0;
}

ACP_tdxBool __cdecl fn_bIsInFix(struct tdstEngineObject_ *p_stObject)
{
    unsigned char ucIndex;

    for (ucIndex = 0; (unsigned long)ucIndex < GAM_pc_g_ulNbFixObject; ucIndex++)
        if (GAM_pc_g_a_ulFixObject[ucIndex] == (unsigned long)p_stObject)
            return 1;
    return 0;
}

/* 0x00412410 */
unsigned long __cdecl GAM_pc_fn_ulGetHolderSize8(void)
{
    return 8;
}

/* 0x004111A0 */
void __cdecl fn_vMicroFree(void *p_pOwner)
{
    if (p_pOwner == (void *)0)
    {
        ERM_fn_vClearLastError(0);
        ERM_fn_vUpdateLastError(0x30, g_ucGameModuleId, 0, 0xe, -1, 0xff, 0xff, (char *)0);
    }
    if (((struct tdstEngineObject_ *)p_pOwner)->h_Micro == (void *)0)
    {
        ERM_fn_vClearLastError(0);
        ERM_fn_vUpdateLastError(0x30, g_ucGameModuleId, 0, 0xf, -1, 0xff, 0xff, (char *)0);
    }
    fn_vGameFreeInHLM(((struct tdstEngineObject_ *)p_pOwner)->h_Micro);
    ((struct tdstEngineObject_ *)p_pOwner)->h_Micro = (void *)0;
}

void __cdecl fn_vMicroDesinit(void *p_pOwner)
{
    if (p_pOwner == (void *)0)
    {
        ERM_fn_vClearLastError(0);
        ERM_fn_vUpdateLastError(0x30, g_ucGameModuleId, 0, 0xe, -1, 0xff, 0xff, (char *)0);
    }
    if (((struct tdstEngineObject_ *)p_pOwner)->h_Micro == (void *)0)
    {
        ERM_fn_vClearLastError(0);
        ERM_fn_vUpdateLastError(0x30, g_ucGameModuleId, 0, 0xf, -1, 0xff, 0xff, (char *)0);
    }
}

unsigned long __cdecl fn_ulSectInfoSizeOf(void)
{
    return 0x14;
}

void __cdecl fn_vSectInfoFree(void *p_pOwner)
{
    if (p_pOwner == (void *)0)
    {
        ERM_fn_vClearLastError(0);
        ERM_fn_vUpdateLastError(0x30, g_ucGameModuleId, 0, 0xe, -1, 0xff, 0xff, (char *)0);
    }
    if (((struct tdstEngineObject_ *)p_pOwner)->h_SectInfo == (void *)0)
    {
        ERM_fn_vClearLastError(0);
        ERM_fn_vUpdateLastError(0x30, g_ucGameModuleId, 0, 0xf, -1, 0xff, 0xff, (char *)0);
    }
    fn_vGameFreeInHLM(((struct tdstEngineObject_ *)p_pOwner)->h_SectInfo);
    ((struct tdstEngineObject_ *)p_pOwner)->h_SectInfo = (void *)0;
}

extern void *__cdecl SECT_fn_hCreateElementLstDNMLights(void);

void *__cdecl GAM_pc_fn_p_stAllocLinkedHolder(void)
{
    unsigned long *pulHolder;
    void *pElement;

    pulHolder = (unsigned long *)fnp_vGameMallocInHLM((unsigned int)fn_ulSectInfoSizeOf());
    pElement = SECT_fn_hCreateElementLstDNMLights();
    pulHolder[1] = (unsigned long)pElement;
    return pulHolder;
}

void __cdecl fn_vSectInfoAlloc(void *p_pOwner)
{
    if (p_pOwner == (void *)0)
    {
        ERM_fn_vClearLastError(0);
        ERM_fn_vUpdateLastError(0x30, g_ucGameModuleId, 0, 0xe, -1, 0xff, 0xff, (char *)0);
    }
    if (((struct tdstEngineObject_ *)p_pOwner)->h_SectInfo != (void *)0)
    {
        ERM_fn_vClearLastError(0);
        ERM_fn_vUpdateLastError(0x30, g_ucGameModuleId, 0, 0xd, -1, 0xff, 0xff, (char *)0);
    }
    ((struct tdstEngineObject_ *)p_pOwner)->h_SectInfo = GAM_pc_fn_p_stAllocLinkedHolder();
}

extern void __cdecl fn_v_DynamicFree(void *param_1);

void __cdecl fn_vDynamFree(void *p_pOwner)
{
    if (p_pOwner == (void *)0)
    {
        ERM_fn_vClearLastError(0);
        ERM_fn_vUpdateLastError(0x30, g_ucGameModuleId, 0, 0xe, -1, 0xff, 0xff, (char *)0);
    }
    if (((struct tdstEngineObject_ *)p_pOwner)->h_Dynam == (void *)0)
    {
        ERM_fn_vClearLastError(0);
        ERM_fn_vUpdateLastError(0x30, g_ucGameModuleId, 0, 0xf, -1, 0xff, 0xff, (char *)0);
    }
    fn_v_DynamicFree(((struct tdstEngineObject_ *)p_pOwner)->h_Dynam);
    fn_vGameFreeInHLM(((struct tdstEngineObject_ *)p_pOwner)->h_Dynam);
    ((struct tdstEngineObject_ *)p_pOwner)->h_Dynam = (void *)0;
}

void __cdecl fn_vDynamDesinit(void *p_pOwner, long lMode)
{
    void *pHolder;
    DNM_tdstDynamics *p_stDynamics;

    if (p_pOwner == (void *)0)
    {
        ERM_fn_vClearLastError(0);
        ERM_fn_vUpdateLastError(0x30, g_ucGameModuleId, 0, 0xe, -1, 0xff, 0xff, (char *)0);
    }
    if (((struct tdstEngineObject_ *)p_pOwner)->h_Dynam == (void *)0)
    {
        ERM_fn_vClearLastError(0);
        ERM_fn_vUpdateLastError(0x30, g_ucGameModuleId, 0, 0xf, -1, 0xff, 0xff, (char *)0);
    }
    switch (lMode)
    {
    case 1:
        pHolder = ((struct tdstEngineObject_ *)p_pOwner)->h_Dynam;
        p_stDynamics = ((MS_tdxHandleToDynam)pHolder)->p_stDynamics;
        if ((DNM_M_ulEndFlags(p_stDynamics) & DNM_C_Always) != 0)
            fn_v_DynamicFree(pHolder);
        break;
    }
}

#include "SCT/Sector.h"
void __cdecl GAM_pc_fn_vStopMatchingSounds(long lKey, void *p_pObject)
{
    SECT_tdxHandleOfSectorObject pStruct;
    SCT_tdxHandleOfLstSoundEvent plNode;
    unsigned long ulCount;

    pStruct = (SECT_tdxHandleOfSectorObject)((HIE_tdstSuperObject *)p_pObject)->hLinkedObject.p_Void;
    ulCount = LST2_M_StaticGetNumberOfElements(&pStruct->stListOfSoundEvent);
    plNode = LST2_M_StaticGetFirstElement(&pStruct->stListOfSoundEvent);
    for (; ulCount > 0; ulCount--)
    {
        if ((long)plNode->hPointerOfSectorSO == lKey || plNode->hPointerOfSectorSO == NULL)
            SND_fn_lSendRequestSound(-1, GAM_g_lSoundObjectTypeMusic, plNode->uSoundEvent, 0, (void *)0);
        plNode = LST2_M_StaticGetNextElement(plNode);
    }
}

void __cdecl GAM_fn_vSetGameMemoryInLevel(void)
{
    GAM_g_ucHLMMemoryBlock = 1;
}

#pragma intrinsic(memcpy)
void __cdecl fn_vMicroCopyClone(void *p_pDst, void *p_pSrc)
{
    unsigned long ulSize;

    ulSize = GAM_pc_fn_ulGetHolderSize8();
    memcpy(((struct tdstEngineObject_ *)p_pDst)->h_Micro, ((struct tdstEngineObject_ *)p_pSrc)->h_Micro, ulSize);
}

long *__cdecl GAM_pc_fn_p_vReleaseTypeTwoBlock(long *plBlock)
{
    long lTag;
    long lObject;

    lTag = plBlock[0];
    lObject = plBlock[1];
    if (lTag == 2)
    {
        fn_p_stFreeEngineObject((struct tdstEngineObject_ *)lObject);
        HIE_fn_vDestroySuperObject((HIE_tdstSuperObject *)plBlock);
        return plBlock;
    }
    ERM_fn_vUpdateLastError(0x30, g_ucGameModuleId, 0, 0x39, -1, 0xff, 0xff, (char *)0);
    return plBlock;
}

extern unsigned char __cdecl fn_ucIsAnAlwaysObject(void *p_stEngineObject);

#define GAM_M_GetEngineObject(_hSO) \
    ((_hSO)->ulTypeOfLinkedObject != 2 ? (struct tdstEngineObject_ *)0 \
                                       : (struct tdstEngineObject_ *)(_hSO)->hLinkedObject.p_Void)
void __cdecl fn_vDestroyAllHierarchy(HIE_tdstSuperObject *p_stSuperObject)
{
    HIE_tdstSuperObject *p_stChild;
    HIE_tdstSuperObject *p_stNextChild;

    p_stChild     = p_stSuperObject->p_stFirstChild;
    p_stNextChild = HIE_fn_hForceGetSuperObjectNextBrother(p_stChild);
    while (p_stChild != NULL)
    {
        fn_vDestroyAllHierarchy(p_stChild);
        p_stChild     = p_stNextChild;
        p_stNextChild = HIE_fn_hForceGetSuperObjectNextBrother(p_stNextChild);
    }

    HIE_fn_vIsolate(p_stSuperObject);

    switch (p_stSuperObject->ulTypeOfLinkedObject)
    {
    case 2:
        if (!fn_ucIsAnAlwaysObject(GAM_M_GetEngineObject(p_stSuperObject)))
        {
            if (fn_bIsInFix(GAM_M_GetEngineObject(p_stSuperObject)) || FUN_0044f4e0() == 1)
                fn_vDesinitOneObject(GAM_M_GetEngineObject(p_stSuperObject), GAM_C_OTI_MapLoaded);
            else
                GAM_pc_fn_p_vReleaseTypeTwoBlock((long *)p_stSuperObject);
        }
        break;
    }
}

#include "SCT/SectExt.h"

void __cdecl fn_vReInitActorsWhenSectorBecomesActive(HIE_tdstSuperObject *_hSector)
{
	SECT_tdxHandleOfElementLstCharacter hCharacterList,hNextCharacterList;
	HIE_tdstSuperObject *hTreatCharacter;
	struct tdstEngineObject_ *p_stTreatObject;
	long j;

	for
	(
		j=LST2_M_DynamicGetNumberOfElements( &((SECT_tdxHandleOfSectorObject)HIE_fn_hGetSuperObjectObject(_hSector))->stListOfCharacters),(hCharacterList)=SECT_fn_hGetFirstCharListInSector(_hSector),
		(hNextCharacterList)=(hCharacterList)?SECT_fn_hGetNextCharListInSector(hCharacterList):NULL;
		hCharacterList&&j;
		j--,hCharacterList=hNextCharacterList,hNextCharacterList?hNextCharacterList=SECT_fn_hGetNextCharListInSector(hNextCharacterList):NULL
	)
	{
		hTreatCharacter = SECT_GetCharacterInList(hCharacterList);
		p_stTreatObject = (struct tdstEngineObject_ *)HIE_fn_hGetSuperObjectObject(hTreatCharacter);
		if(
			(fn_bf1StandardGameGetIsDesactivateAtAll(p_stTreatObject->h_StandardGame)
			 && ((fn_eStandardGameGetInitFlagWhenDeadOrTaken(p_stTreatObject->h_StandardGame) == OI_WhenPlayerGoOutOfActionZone)
				 ||
				 (fn_eStandardGameGetInitFlagWhenDeadOrTaken(p_stTreatObject->h_StandardGame) == OI_Always)
				)
			)
			||
			(!fn_bf1StandardGameGetIsDesactivateAtAll(p_stTreatObject->h_StandardGame)
			 && (fn_eStandardGameGetInitFlagWhenOutOfZone(p_stTreatObject->h_StandardGame) == OI_WhenPlayerGoOutOfActionZone)
			)
		   )
		{
			if( !fn_ucIsAnAlwaysObject(p_stTreatObject)
              )
				fn_vInitOneObject(p_stTreatObject,OTI_WhenGoOutOfZone);
		}
	}
}

void __cdecl fn_vInitObjectsWhenPlayerChangeSector(HIE_tdstSuperObject *hCharacter,
                                           HIE_tdstSuperObject *hCurrentSector,
                                           HIE_tdstSuperObject *hPreviousSector)
{
	long i,j;
	HIE_tdstSuperObject *hListInActiveSector;
	HIE_tdstSuperObject *hListInPreviousSector;
	SECT_tdxHandleOfElementLstActivityInteraction hActivityListInActiveSector;
	SECT_tdxHandleOfElementLstActivityInteraction hActivityListInPreviousSector;
	unsigned char bSectorWasAndIsActive;

	SECT_M_ForEachActivityNodeInActivityInteractionList(hCurrentSector,hActivityListInActiveSector,i)
	{
		bSectorWasAndIsActive = FALSE;
		hListInActiveSector = SECT_GetSectorInActivityList(hActivityListInActiveSector);
		if (hListInActiveSector==hPreviousSector)
			bSectorWasAndIsActive = TRUE;
		else
		{
			SECT_M_ForEachActivityNodeInActivityInteractionList(hPreviousSector,hActivityListInPreviousSector,j)
			{
				hListInPreviousSector = SECT_GetSectorInActivityList(hActivityListInPreviousSector);
				if (hListInPreviousSector==hListInActiveSector)
					bSectorWasAndIsActive = TRUE;
			}
		}
		if (bSectorWasAndIsActive == FALSE)
		{
			fn_vReInitActorsWhenSectorBecomesActive(hListInActiveSector);
		}
	}

	bSectorWasAndIsActive = FALSE;
	SECT_M_ForEachActivityNodeInActivityInteractionList(hPreviousSector,hActivityListInPreviousSector,j)
	{
		hListInPreviousSector = SECT_GetSectorInActivityList(hActivityListInPreviousSector);
		if (hListInPreviousSector==hCurrentSector)
			bSectorWasAndIsActive = TRUE;
	}
	if (bSectorWasAndIsActive == FALSE)
	{
		fn_vReInitActorsWhenSectorBecomesActive(hCurrentSector);
	}
}

void __cdecl fn_vKillAllAlwaysModel(void)
{
    int *p_stNode;
    int *p_stNext;

    p_stNode = (int *)ALW_g_h_LstAlwaysModel;
    p_stNext = (p_stNode != (int *)0) ? (int *)*p_stNode : (int *)0;

    while (p_stNode != (int *)0)
    {
        if (fn_bIsInFix((struct tdstEngineObject_ *)p_stNode[4]) != 0)
        {
            fn_vDesinitOneObject((struct tdstEngineObject_ *)p_stNode[4], 1);
        }
        else
        {
            int  iObject;

            if (p_stNode[0] != 0)
                ((int *)p_stNode[0])[1] = p_stNode[1];
            if (p_stNode[1] != 0)
                ((int *)p_stNode[1])[0] = p_stNode[0];

            if (p_stNode[2] != 0)
            {
                ((int *)p_stNode[2])[2] -= 1;
                if ((int *)((int *)p_stNode[2])[0] == p_stNode)
                    ((int *)p_stNode[2])[0] = p_stNode[0];
                if ((int *)((int *)p_stNode[2])[1] == p_stNode)
                    ((int *)p_stNode[2])[1] = p_stNode[1];
            }

            iObject = p_stNode[4];
            p_stNode[0] = 0;
            p_stNode[1] = 0;
            p_stNode[2] = 0;
            p_stNode[4] = (int)fn_p_stFreeEngineObject((struct tdstEngineObject_ *)iObject);
        }

        p_stNode = p_stNext;
        p_stNext = (p_stNode != (int *)0) ? (int *)*p_stNode : (int *)0;
    }
}

/* 0x00453080 */
extern void __cdecl SND_fn_vKillAllObjectTypeSound(long type);

void __cdecl fn_vSnifThePlayerIsDead(void)
{

    POS_fn_vSetIdentityMatrix(&GAM_g_stEngineStructure.stMainCharacterPosition);
    POS_fn_vSetIdentityMatrix(&GAM_g_stEngineStructure.stMainCameraPosition);
    GAM_g_stEngineStructure.bResurection = FALSE;

    SND_fn_vKillAllObjectTypeSound( GAM_g_lSoundObjectTypeAnim );
    fn_vChangeEngineMode(E_EM_ModePlayerDead);
}

#define M_3dData_SAIFlags ((SAI_tdeFlags)(SAI_eInitWhenPlayerDead | SAI_eInitWhenReinitTheMap | SAI_eInitWhenMapLoaded))

#define GAM_M_p_st3dDataGetMatrixInit(h_3dData) \
    ((POS_tdstCompletePosition *)SAI_fn_p_vGetTableValuePointerOf((void *)&(h_3dData)->stGLIObjectMatrix, M_3dData_SAIFlags))

void __cdecl fn_vSnifThePlayerIsDeadWithResurrectionAt(HIE_tdxHandleToSuperObject _hCharacterPositionOne, HIE_tdxHandleToSuperObject _hCharacterPositionTwo)
{
    HIE_tdxHandleToSuperObject hMaincharacter = GAM_pc_g_hMainCharacterSO;

    GAM_g_stEngineStructure.bResurection = FALSE;

    if (_hCharacterPositionOne && hMaincharacter)
    {

        POS_fn_vCopyMatrix(
            &GAM_g_stEngineStructure.stMainCharacterPosition,
            GAM_M_p_st3dDataGetMatrixInit(M_GetMSHandle(hMaincharacter,3dData)));

        POS_fn_vCopyMatrix(
            GAM_M_p_st3dDataGetMatrixInit(M_GetMSHandle(hMaincharacter,3dData)),
            HIE_fn_hGetSuperObjectGlobalMatrix(_hCharacterPositionOne));
        GAM_g_stEngineStructure.bResurection = TRUE;
    }
    else if (hMaincharacter)
    {

        POS_fn_vCopyMatrix(
            &GAM_g_stEngineStructure.stMainCharacterPosition,
            GAM_M_p_st3dDataGetMatrixInit(M_GetMSHandle(hMaincharacter,3dData)));
        GAM_g_stEngineStructure.bResurection = TRUE;
    }

    if (_hCharacterPositionTwo && GAM_g_stEngineStructure.h_StdCamCaracter)
    {

        POS_fn_vCopyMatrix(
            &GAM_g_stEngineStructure.stMainCameraPosition,
            GAM_M_p_st3dDataGetMatrixInit(M_GetMSHandle(GAM_g_stEngineStructure.h_StdCamCaracter,3dData)));

        POS_fn_vCopyMatrix(
            GAM_M_p_st3dDataGetMatrixInit(M_GetMSHandle(GAM_g_stEngineStructure.h_StdCamCaracter,3dData)),
            HIE_fn_hGetSuperObjectGlobalMatrix(_hCharacterPositionTwo));
        GAM_g_stEngineStructure.bResurection = TRUE;
    }
    else if (GAM_g_stEngineStructure.h_StdCamCaracter)
    {

        POS_fn_vCopyMatrix(
            &GAM_g_stEngineStructure.stMainCameraPosition,
            GAM_M_p_st3dDataGetMatrixInit(M_GetMSHandle(GAM_g_stEngineStructure.h_StdCamCaracter,3dData)));
        GAM_g_stEngineStructure.bResurection = TRUE;
    }

    SND_fn_vKillAllObjectTypeSound( GAM_g_lSoundObjectTypeAnim );
    fn_vChangeEngineMode(E_EM_ModePlayerDead);
}

/* 0x004079B0 */
void __cdecl fn_vLastDesinitFamilyStructure(void)
{
    tdstFamilyList       *p_stThisFamily;
    tdstFamilyList       *p_stNextFamily;
    tdstFamilyListAnchor *p_stFather;

    p_stThisFamily = GAM_g_stEngineStructure.hFamilyList_hFirstElementDyn;

    p_stNextFamily = (p_stThisFamily != NULL) ? p_stThisFamily->hNextBrother : NULL;
    while (p_stThisFamily != NULL)
    {
        if (p_stThisFamily->hNextBrother != NULL)
            p_stThisFamily->hNextBrother->hPrevBrother = p_stThisFamily->hPrevBrother;
        if (p_stThisFamily->hPrevBrother != NULL)
            p_stThisFamily->hPrevBrother->hNextBrother = p_stThisFamily->hNextBrother;
        p_stFather = p_stThisFamily->hFather;
        if (p_stFather != NULL)
        {
            p_stFather->lNumberOfElementsDyn = p_stFather->lNumberOfElementsDyn + -1;
            if (p_stThisFamily->hFather->hFirstElementDyn == p_stThisFamily)
                p_stThisFamily->hFather->hFirstElementDyn = p_stThisFamily->hNextBrother;
            if (p_stThisFamily->hFather->hLastElementDyn == p_stThisFamily)
                p_stThisFamily->hFather->hLastElementDyn = p_stThisFamily->hPrevBrother;
        }
        p_stThisFamily->hNextBrother = NULL;
        p_stThisFamily->hPrevBrother = NULL;
        p_stThisFamily->hFather = NULL;
        p_stThisFamily = p_stNextFamily;
        p_stNextFamily = (p_stNextFamily != NULL) ? p_stNextFamily->hNextBrother : NULL;
    }
}

void __cdecl fn_vDesinitFamilyStructureForPriority(unsigned char ucPriority)
{
    tdstFamilyList *p_stThisFamily;
    tdstFamilyList *p_stNextFamily;

    p_stThisFamily = GAM_g_stEngineStructure.hFamilyList_hFirstElementDyn;
    p_stNextFamily = (p_stThisFamily != NULL) ? p_stThisFamily->hNextBrother : NULL;

    while (p_stThisFamily != NULL)
    {
        if (p_stThisFamily->ucPriority == GAM_g_ucHLMMemoryBlock)
        {
            if (p_stThisFamily->hNextBrother != NULL)
                p_stThisFamily->hNextBrother->hPrevBrother = p_stThisFamily->hPrevBrother;
            if (p_stThisFamily->hPrevBrother != NULL)
                p_stThisFamily->hPrevBrother->hNextBrother = p_stThisFamily->hNextBrother;
            if (p_stThisFamily->hFather != NULL)
            {
                p_stThisFamily->hFather->lNumberOfElementsDyn -= 1;
                if (p_stThisFamily->hFather->hFirstElementDyn == p_stThisFamily)
                    p_stThisFamily->hFather->hFirstElementDyn = p_stThisFamily->hNextBrother;
                if (p_stThisFamily->hFather->hLastElementDyn == p_stThisFamily)
                    p_stThisFamily->hFather->hLastElementDyn = p_stThisFamily->hPrevBrother;
            }
            p_stThisFamily->hNextBrother = NULL;
            p_stThisFamily->hPrevBrother = NULL;
            p_stThisFamily->hFather = NULL;
        }

        p_stThisFamily = p_stNextFamily;
        p_stNextFamily = (p_stThisFamily != NULL) ? p_stThisFamily->hNextBrother : NULL;
    }
}

/* 0x005115D0 */
unsigned short GAM_g_uw5115d0_storage;
#define GAM_g_uw5115d0  GAM_g_uw5115d0_storage

/* 0x0044C440 */
void __cdecl FUN_0044c440(void)
{
    GAM_g_uw5115d0 = 0;
}
