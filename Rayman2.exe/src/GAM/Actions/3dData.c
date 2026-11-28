#include "GAM/GAM_types.h"
#include "GAM/PlayAnim.h"
#include "ACP/ACP_types.h"
#include "GAM/STRUCTUR/State.h"
#include "GAM/Actions/3dData.h"
#include "GAM/Family.h"
#include "SAI/SAI.h"
#include "ERM/CpaError.h"
#include "GAM/GAM_globals.h"
#include "RND/RND.h"
#include <string.h>
#include "TMP/TmpMem.h"
#include "GAM/InitEnum.h"

#define E_uwGameMiniStructObjectNull ((unsigned short)0xE)
#define E_uwGameMiniStructNull       ((unsigned short)0xF)
#define E_uwGameMiniStructAllocAlreadyAlloc ((unsigned short)0xD)

#define M_3dData_SAIFlags ((SAI_tdeFlags)(SAI_eInitWhenPlayerDead | SAI_eInitWhenReinitTheMap | SAI_eInitWhenMapLoaded))

#define C3D_M_h3dData(p) ((MS_tdxHandleTo3dData)(p)->h_3dData)

static __inline POS_tdstCompletePosition *fn_p_st3dDataGetMatrix(MS_tdxHandleTo3dData h_3dData)
    { return &(h_3dData->stGLIObjectMatrix); }
static __inline POS_tdstCompletePosition *fn_p_st3dDataGetMatrixInit(MS_tdxHandleTo3dData h_3dData)
    { return (POS_tdstCompletePosition *)SAI_fn_p_vGetTableValuePointerOf((void *)&h_3dData->stGLIObjectMatrix, M_3dData_SAIFlags); }
static __inline POS_tdstCompletePosition *fn_p_st3dDataGetAbsoluteMatrix(MS_tdxHandleTo3dData _h_3dData)
    { return _h_3dData->p_stGLIObjectAbsoluteMatrix; }
static __inline void fn_v3dDataSetCurrentObjectsTable(MS_tdxHandleTo3dData h_3dData, tdxHandleToObjectsTablesList h_CurrentObjectsTable)
    { h_3dData->h_CurrentObjectsTable = h_CurrentObjectsTable; }
static __inline tdxHandleToObjectsTablesList fn_h3dDataGetInitialObjectsTable(MS_tdxHandleTo3dData h_3dData)
    { return h_3dData->h_InitialObjectsTable; }
static __inline void fn_v3dDataSetInitialObjectsTable(MS_tdxHandleTo3dData h_3dData, tdxHandleToObjectsTablesList h_InitialObjectsTable)
    { h_3dData->h_InitialObjectsTable = h_InitialObjectsTable; }
static __inline tdxHandleToState fn_h3dDataGetInitialState(MS_tdxHandleTo3dData h_3dData)
    { return h_3dData->h_InitialState; }
static __inline void fn_v3dDataSetShadowScaleX(MS_tdxHandleTo3dData _h3dData, MTH_tdxReal _xScaleX)
    { _h3dData->xShadowScaleX = _xScaleX; }
static __inline void fn_v3dDataSetShadowScaleY(MS_tdxHandleTo3dData _h3dData, MTH_tdxReal _xScaleY)
    { _h3dData->xShadowScaleY = _xScaleY; }

/* 0x004078C0 */
extern void __cdecl fn_vGameFreeInHLM(void *p_vObject);

static __inline unsigned short fn_w3dDataGetSizeOfArrayOfElts3d(MS_tdxHandleTo3dData h_3dData)
{
    return(h_3dData->wSizeOfArrayOfElts3d);
}

/* 0x004194B0 */
void *__cdecl GAM_pc_fn_pvGetInitObjectsTable(tdstObjectsTablesList *p_stList)
{
    return p_stList->d_stInitObjectsTable;
}

MS_tdxHandleTo3dData __cdecl fn_h_3dDataRealAlloc(void);

/* 0x00403290 */
void __cdecl fn_v3dDataSavedValueSAI(MS_tdxHandleTo3dData h_3dData)
{
}

extern void __cdecl fn_vInitMorphList(CHN_tdxHandleToMorphList *p_hMorphList);
extern long __cdecl PLA_fn_vInitNewState(HIE_tdstSuperObject *p_stSuperObject, tdxHandleToState h_State);

void __cdecl fn_v3dDataInitValueSAI(MS_tdxHandleTo3dData h_3dData);

/* 0x004180F0 */
void __cdecl fn_v3dDataAlloc(struct tdstEngineObject_ *p_stObject)
{
    if (p_stObject == NULL)
    {
        ERM_fn_vClearLastError(0);
        ERM_fn_vUpdateLastError(0x30, g_ucGameModuleId, 0,
                                E_uwGameMiniStructObjectNull, -1, 0xff, 0xff, NULL);
    }

    if (p_stObject->h_3dData != NULL)
    {
        ERM_fn_vClearLastError(0);
        ERM_fn_vUpdateLastError(0x30, g_ucGameModuleId, 0,
                                E_uwGameMiniStructAllocAlreadyAlloc, -1, 0xff, 0xff, NULL);
    }

    p_stObject->h_3dData = fn_h_3dDataRealAlloc();
    fn_v3dDataInitValueSAI((MS_tdxHandleTo3dData)p_stObject->h_3dData);
}

void __cdecl fn_v3dDataSetBrainComputationFrequency(MS_tdxHandleTo3dData h_3dData, unsigned char _ucFrequency);
void __cdecl fn_v3dDataSetLightComputationFrequency(MS_tdxHandleTo3dData h_3dData, unsigned char _ucFrequency);
unsigned long __cdecl fn_ul3dDataSizeOf(void);

/* 0x00418170 */
MS_tdxHandleTo3dData __cdecl fn_h_3dDataRealAlloc(void)
{
    MS_tdxHandleTo3dData h_3dData;

    h_3dData = (MS_tdxHandleTo3dData)fnp_vGameMallocInHLM((unsigned int)fn_ul3dDataSizeOf());
    POS_fn_vSetIdentityMatrix(fn_p_st3dDataGetMatrix(h_3dData));

    fn_v3dDataSetFlagEndState(h_3dData, 0);
    fn_v3dDataSetFlagEndOfAnim(h_3dData, 0);
    fn_v3dDataSetFlagModifState(h_3dData, 0);
    fn_v3dDataSetForcedFrame(h_3dData, 0);

    fn_p_st3dDataGetCurrentFrame(h_3dData)->p_stArrayOfElts3d = NULL;
    fn_v3dDataSetSizeOfArrayOfElts3d(h_3dData, 0);
    h_3dData->hMorphChannelList = NULL;

    fn_v3dDataSetStateInLastFrame(h_3dData, NULL);
    fn_v3dDataSetWantedState(h_3dData, NULL);
    fn_v3dDataSetCurrentState(h_3dData, NULL);

    fn_v3dDataSetCurrentObjectsTable(h_3dData, NULL);
    fn_v3dDataSetInitialObjectsTable(h_3dData, NULL);

    h_3dData->ulStartTime = GAM_g_stEngineStructure.stEngineTimer.ulCurrentTimerCount;
    h_3dData->bSkipCurrentFrame = 0;
    h_3dData->ulTimeDelay = 0;
    h_3dData->sLastFrame = -1;
    h_3dData->bStateJustModified = 0;
    h_3dData->uwNbEngineFrameSinceLastMechEvent = LME_INVALID;
    h_3dData->ucFrameRate = 0;

    h_3dData->lDrawMask = GLI_C_lAllIsEnable;
    h_3dData->lDrawMaskInit = GLI_C_lAllIsEnable;

    fn_v3dDataSetShadowScaleX(h_3dData, MTH_C_ONE);
    fn_v3dDataSetShadowScaleY(h_3dData, MTH_C_ONE);

    h_3dData->p_stShadowMaterial = NULL;
    h_3dData->p_stShadowTexture = NULL;
    h_3dData->xShadowQuality = 2;
    *(unsigned long *)&h_3dData->stSHWDeformationVector.xX = 0;
    *(unsigned long *)&h_3dData->stSHWDeformationVector.xY = 0;
    *(unsigned long *)&h_3dData->stSHWDeformationVector.xZ = 0;
    *(unsigned long *)&h_3dData->xSHWHeight = 0x3E4CCCCDUL;

    fn_v3dDataSetBrainComputationFrequency(h_3dData, 1);
    h_3dData->cBrainCounter = -1;

    h_3dData->uwBrainMainCounter = (unsigned short)RND_fn_lGetLongRandomValue(0, 1, 0x11);

    fn_v3dDataSetLightComputationFrequency(h_3dData, 1);
    h_3dData->cLightCounter = -1;

    h_3dData->ucTransparency = 255;

    return(h_3dData);
}

void __cdecl fn_v3dDataFreeInitValueSAI(MS_tdxHandleTo3dData h_3dData);

/* 0x004182A0 */
void __cdecl fn_v3dDataFree(HIE_tdstEngineObject *p_stObject)
{
    if (p_stObject == NULL)
    {
        ERM_fn_vClearLastError(0);
        ERM_fn_vUpdateLastError(0x30, g_ucGameModuleId, 0,
                                E_uwGameMiniStructObjectNull,
                                -1, 0xff, 0xff, NULL);
    }

    if (p_stObject->h3dData == NULL)
    {
        ERM_fn_vClearLastError(0);
        ERM_fn_vUpdateLastError(0x30, g_ucGameModuleId, 0,
                                E_uwGameMiniStructNull,
                                -1, 0xff, 0xff, NULL);
    }

    fn_v3dDataFreeInitValueSAI((MS_tdxHandleTo3dData)p_stObject->h3dData);

    PLA_fn_vDesInitAllChildOfCharacter(NULL, (struct tdstEngineObject_ *)p_stObject);
    fn_vFreeMorphList((void *)&(((MS_tdxHandleTo3dData)p_stObject->h3dData)->hMorphChannelList));

    fn_vGameFreeInHLM(p_stObject->h3dData);
    p_stObject->h3dData = NULL;
}

void __cdecl GAM_pc_fn_vCopyArrayOfElts20(tdstObjectsTablesList *p_stList);

/* 0x00418340 */
void __cdecl fn_v3dDataInit(struct tdstEngineObject_ *p_stObject, int eObjectInit)
{
    tdxHandleToObjectsTablesList hOTLE;

    if (p_stObject == NULL)
    {
        ERM_fn_vClearLastError(0);
        ERM_fn_vUpdateLastError(0x30, g_ucGameModuleId, 0,
                                E_uwGameMiniStructObjectNull, -1, 0xff, 0xff, NULL);
    }

    if (p_stObject->h_3dData == NULL)
    {
        ERM_fn_vClearLastError(0);
        ERM_fn_vUpdateLastError(0x30, g_ucGameModuleId, 0,
                                E_uwGameMiniStructNull, -1, 0xff, 0xff, NULL);
    }

    if ((eObjectInit == OTI_LoadSavedGame) || (eObjectInit == OTI_Always))
    {
        HIE_tdstSuperObject *hFather;
        HIE_tdstSuperObject *hCharacter;

        fn_v3dDataSetMatrix(p_stObject->h_3dData, fn_p_st3dDataGetMatrixInit(p_stObject->h_3dData));

        hCharacter = fn_h_StandardGameGetSuperObject(p_stObject->h_StandardGame);

        if (hCharacter && (hFather = HIE_fn_hGetSuperObjectFather(hCharacter)) != NULL)
        {

            POS_fn_vMulMatrixMatrix(
                fn_p_st3dDataGetAbsoluteMatrix(p_stObject->h_3dData),
                HIE_fn_hGetSuperObjectGlobalMatrix(hFather),
                fn_p_st3dDataGetMatrix(p_stObject->h_3dData));
        }
    }

    {
        HIE_tdstSuperObject *h_SupObject;

        h_SupObject = M_GetSuperObject(p_stObject);
        if (h_SupObject)
        {
            h_SupObject->fTransparenceLevel = MTH_M_xLongToReal(C3D_M_h3dData(p_stObject)->ucTransparency);
        }
    }

    switch (eObjectInit)
    {
    case OTI_WhenGameStart:
        break;
    case OTI_MapLoaded:
        fn_v3dDataInitValueSAI(p_stObject->h_3dData);
        fn_v3dDataSavedValueSAI(p_stObject->h_3dData);
        C3D_M_h3dData(p_stObject)->hArrayOfChannels = NULL;
        C3D_M_h3dData(p_stObject)->ulNumberOfChannels = ((tdstFamilyList *)C3D_M_h3dData(p_stObject)->h_Family)->ulNumberOfChannels;

    case OTI_LoadSavedGame:
    case OTI_PlayerDead:
    case OTI_ReinitTheMap:
        C3D_M_h3dData(p_stObject)->ucAnimMatrixHasChanged = TRUE;
        fn_vTakeControlOfChannel(p_stObject, (unsigned short)-1, FALSE);
        fn_v3dDataSetStartTime(C3D_M_h3dData(p_stObject), 0);
        fn_v3dDataSetTimeDelay(C3D_M_h3dData(p_stObject), 0);
        fn_v3dDataSetTimePreviousFrame(C3D_M_h3dData(p_stObject), 0);

        PLA_fn_vDesInitAllChildOfCharacter(M_GetSuperObject(p_stObject), p_stObject);

        memset(&C3D_M_h3dData(p_stObject)->stFrame3d, 0, sizeof(tdstFrame3d));

        fn_v3dDataSetCurrentHieCouples(C3D_M_h3dData(p_stObject), NULL);
        fn_v3dDataSetCurrentHieNbCouples(C3D_M_h3dData(p_stObject), 0);
        fn_v3dDataSetSizeOfArrayOfElts3d(C3D_M_h3dData(p_stObject), 0);

        C3D_M_h3dData(p_stObject)->lLastComputeFrame = 0;

        C3D_M_h3dData(p_stObject)->bStateJustModified = FALSE;
        fn_v3dDataSetFrameRate(C3D_M_h3dData(p_stObject), 0);

        fn_v3dDataSetCurrentState(C3D_M_h3dData(p_stObject), NULL);

        C3D_M_h3dData(p_stObject)->stFrame3d.p_stArrayOfElts3d = NULL;
        C3D_M_h3dData(p_stObject)->stFrame3d.p_stAnim = NULL;

        C3D_M_h3dData(p_stObject)->lDrawMask = C3D_M_h3dData(p_stObject)->lDrawMaskInit;
        C3D_M_h3dData(p_stObject)->cBrainCounter = -1;
        C3D_M_h3dData(p_stObject)->bSkipCurrentFrame = FALSE;
        C3D_M_h3dData(p_stObject)->cLightCounter = -1;

    case OTI_AlwaysCreated:
        fn_vInitMorphList((CHN_tdxHandleToMorphList *)&C3D_M_h3dData(p_stObject)->hMorphChannelList);

        hOTLE = fn_h3dDataGetInitialObjectsTable(C3D_M_h3dData(p_stObject));
        if (hOTLE != NULL && GAM_pc_fn_pvGetInitObjectsTable(hOTLE) != NULL)
            GAM_pc_fn_vCopyArrayOfElts20(hOTLE);

        C3D_M_h3dData(p_stObject)->uwBrainMainCounter = (unsigned short)RND_fn_lGetLongRandomValue(0, 1, 0x11);

        fn_v3dDataSetBrainComputationFrequency(C3D_M_h3dData(p_stObject), 1);
        break;
    }

    if (M_GetSuperObject(p_stObject) != NULL)
    {
        PLA_fn_vInitNewState(M_GetSuperObject(p_stObject), fn_h3dDataGetInitialState(C3D_M_h3dData(p_stObject)));
        HIE_fn_vSetSuperObjectDrawMask(M_GetSuperObject(p_stObject), C3D_M_h3dData(p_stObject)->lDrawMask);
    }
}

/* 0x00418600 */
void __cdecl fn_v3dDataDesinit(HIE_tdstEngineObject *p_stObject, int eObjectInit)
{
    if (p_stObject == NULL)
    {
        ERM_fn_vClearLastError(0);
        ERM_fn_vUpdateLastError(0x30, g_ucGameModuleId, 0,
                                E_uwGameMiniStructObjectNull,
                                -1, 0xff, 0xff, NULL);
    }

    if (p_stObject->h3dData == NULL)
    {
        ERM_fn_vClearLastError(0);
        ERM_fn_vUpdateLastError(0x30, g_ucGameModuleId, 0,
                                E_uwGameMiniStructNull,
                                -1, 0xff, 0xff, NULL);
    }

    switch (eObjectInit)
    {
    case 1:
        PLA_fn_vDesInitAllChildOfCharacter(
            fn_h_StandardGameGetSuperObject((MS_tdxHandleToStandardGame)p_stObject->hStandardGame),
            (struct tdstEngineObject_ *)p_stObject);
        fn_vFreeMorphList((void *)&(((MS_tdxHandleTo3dData)p_stObject->h3dData)->hMorphChannelList));
        break;
    default:
        break;
    }
}

unsigned long __cdecl fn_ul3dDataSizeOf(void)
{
    return (unsigned long)0x118;
}

/* 0x004186B0 */
void __cdecl fn_v3dDataInitValueSAI(MS_tdxHandleTo3dData h_3dData)
{
    SAI_fn_bInsertArrayInTable((void *)&h_3dData->stGLIObjectMatrix,
                               sizeof(POS_tdstCompletePosition), M_3dData_SAIFlags);
}

/* 0x004186D0 */
void __cdecl fn_v3dDataFreeInitValueSAI(MS_tdxHandleTo3dData h_3dData)
{
    SAI_fn_bRemoveValueFromTable((void *)&h_3dData->stGLIObjectMatrix, M_3dData_SAIFlags);
}

/* 0x004186F0 */
void __cdecl fn_v3dDataCopyClone(struct tdstEngineObject_ *_p_stObjectDest,
                                 struct tdstEngineObject_ *_p_stObjectSrc)
{
    void *pv_tmp1 = ((MS_tdxHandleTo3dData)_p_stObjectDest->h_3dData)->hArrayOfChannels;
    void *pv_tmp2 = ((MS_tdxHandleTo3dData)_p_stObjectDest->h_3dData)->stFrame3d.p_stArrayOfElts3d;
    void *pv_tmp3 = ((MS_tdxHandleTo3dData)_p_stObjectDest->h_3dData)->hFirstActiveChannel;
    void *pv_tmp4 = ((MS_tdxHandleTo3dData)_p_stObjectDest->h_3dData)->hMorphChannelList;
    unsigned short wSize = fn_w3dDataGetSizeOfArrayOfElts3d((MS_tdxHandleTo3dData)_p_stObjectDest->h_3dData);

    memcpy(_p_stObjectDest->h_3dData, _p_stObjectSrc->h_3dData, fn_ul3dDataSizeOf());

    ((MS_tdxHandleTo3dData)_p_stObjectDest->h_3dData)->hArrayOfChannels = pv_tmp1;
    ((MS_tdxHandleTo3dData)_p_stObjectDest->h_3dData)->stFrame3d.p_stArrayOfElts3d = pv_tmp2;
    ((MS_tdxHandleTo3dData)_p_stObjectDest->h_3dData)->hFirstActiveChannel = pv_tmp3;
    ((MS_tdxHandleTo3dData)_p_stObjectDest->h_3dData)->hMorphChannelList = pv_tmp4;
    fn_v3dDataSetSizeOfArrayOfElts3d((MS_tdxHandleTo3dData)_p_stObjectDest->h_3dData, wSize);
}

/* 0x00418790 */
void __cdecl fn_v3dDataSetFrameRate(MS_tdxHandleTo3dData h_3dData, unsigned char ucFrameRate)
{
    unsigned long ulNbFrames;

    if (ucFrameRate > 1)
    {
        ulNbFrames = (GAM_g_ulAccumulatedTime - h_3dData->ulStartTime) * h_3dData->ucFrameRate;
        h_3dData->ulStartTime = GAM_g_ulAccumulatedTime - ulNbFrames / ucFrameRate;
    }
    h_3dData->ucFrameRate = ucFrameRate;
}

/* 0x004187E0 */
void __cdecl fn_v3dDataFactorFrameRate(MS_tdxHandleTo3dData _h3dData, unsigned long _ulFactor100)
{
    unsigned long ulNbFrames;
    unsigned char ucFrameRate =
        (unsigned char)(fn_scGetSpeedInState(fn_h3dDataGetCurrentState(_h3dData)) * _ulFactor100 / 100L);

    if (ucFrameRate > 1)
    {
        ulNbFrames = (GAM_g_ulAccumulatedTime - _h3dData->ulStartTime) * _h3dData->ucFrameRate;
        _h3dData->ulStartTime = GAM_g_ulAccumulatedTime - ulNbFrames / ucFrameRate;
        _h3dData->ucFrameRate = ucFrameRate;
    }
}

/* 0x00418850 */
void __cdecl fn_v3dDataSetBrainComputationFrequency(MS_tdxHandleTo3dData h_3dData, unsigned char _ucFrequency)
{
    h_3dData->ucBrainComputationFrequency = _ucFrequency;
    if (h_3dData->cBrainCounter >= h_3dData->ucBrainComputationFrequency)
        h_3dData->cBrainCounter = 0;
}

/* 0x00418880 */
void __cdecl fn_v3dDataUpdateBrainCounter(HIE_tdstSuperObject *_hCharacter)
{
    if (_hCharacter && M_GetMSHandle(_hCharacter, 3dData))
    {
        MS_tdxHandleTo3dData h_3dData = M_GetMSHandle(_hCharacter, 3dData);

        h_3dData->cBrainCounter++;
        if (h_3dData->cBrainCounter >= h_3dData->ucBrainComputationFrequency)
            h_3dData->cBrainCounter = 0;
    }
}

ACP_tdxBool __cdecl fn_b3dDataCanComputeBrain(HIE_tdstSuperObject *_hCharacter)
{
    if (_hCharacter && M_GetMSHandle(_hCharacter, 3dData))
    {
        MS_tdxHandleTo3dData h_3dData = M_GetMSHandle(_hCharacter, 3dData);

        return (ACP_tdxBool)(h_3dData->cBrainCounter == 0);
    }
    return TRUE;
}

/* 0x004188F0 */
void __cdecl fn_v3dDataSetLightComputationFrequency(MS_tdxHandleTo3dData h_3dData, unsigned char _ucFrequency)
{
    h_3dData->ucLightComputationFrequency = _ucFrequency;
    if (h_3dData->cLightCounter >= h_3dData->ucLightComputationFrequency)
        h_3dData->cLightCounter = 0;
}

/* 0x00418960 */
char __cdecl FUN_00418960(void)
{
    return 1;
}

/* 0x00418970 */
long __cdecl fn_b3dDataGetUserEventFlag(MS_tdxHandleTo3dData h_3dData, unsigned char _ucFlagIndex,
                                        MTH3D_tdstVector *_p_stPosition)
{
    unsigned char ucMask = (unsigned char) (1 << _ucFlagIndex);
    if ( _p_stPosition )
        *_p_stPosition = h_3dData->stLastEventGlobalPosition;
    return (h_3dData->ucUserEventFlags & ucMask);
}

void __cdecl fn_v3dDataSetUserEventFlag(MS_tdxHandleTo3dData h_3dData, unsigned char _ucFlagIndex, int _bSet, MTH3D_tdstVector *_p_stPosition)
{
    unsigned char ucMask = (unsigned char) (1 << _ucFlagIndex);
    if (_bSet)
        h_3dData->ucUserEventFlags |= ucMask;
    else
        h_3dData->ucUserEventFlags &= ~ucMask;
    if ( _p_stPosition )
        h_3dData->stLastEventGlobalPosition = *_p_stPosition;
}

/* 0x00418A20 */
void __cdecl fn_vAllocArrayOfElts3d(MS_tdxHandleTo3dData h_3dData, unsigned short _wNumberOfElts3d)
{
    unsigned short wCurrentSize = h_3dData->wSizeOfArrayOfElts3d;

    if (_wNumberOfElts3d > wCurrentSize)
    {
        if (fn_p_st3dDataGetCurrentFrame(h_3dData)->p_stArrayOfElts3d)
        {
            TMP_M_Free(fn_p_st3dDataGetCurrentFrame(h_3dData)->p_stArrayOfElts3d);
        }
        fn_p_st3dDataGetCurrentFrame(h_3dData)->p_stArrayOfElts3d =
            (struct tdstElement3d_ *)TMP_M_p_Malloc(sizeof(struct tdstElement3d_) * _wNumberOfElts3d);
        fn_v3dDataSetSizeOfArrayOfElts3d(h_3dData, _wNumberOfElts3d);
    }
}

/* 0x00418A70 */
void __cdecl fn_vFreeArrayOfElts3d(MS_tdxHandleTo3dData h_3dData)
{
    struct tdstElement3d_ *p_stElts3d = fn_p_st3dDataGetCurrentFrame(h_3dData)->p_stArrayOfElts3d;

    if (p_stElts3d)
        TMP_M_Free(p_stElts3d);

    fn_p_st3dDataGetCurrentFrame(h_3dData)->p_stArrayOfElts3d = NULL;
    fn_v3dDataSetSizeOfArrayOfElts3d(h_3dData, 0);
}

/* 0x00419480 */
void __cdecl GAM_pc_fn_vCopyArrayOfElts20(tdstObjectsTablesList *p_stList)
{
    memcpy(p_stList->d_stObjectsTable, p_stList->d_stInitObjectsTable,
           (unsigned int)p_stList->wNumberOfElement * 20);
}
