#include "GAM/PlayAnim.h"
#include "POS/POS_types.h"
#include "MTH/MTH3D.h"
#include "MTH/MTH_flt.h"
#include "GAM/A3d.h"
#include "GAM/Actions/3dData.h"
#include "TMP/TmpMem.h"
#include "GEO/GeoMem.h"
#include "GEO/GEO_types.h"
#include "HIE/HIE_types.h"
#include "GAM/GAM_globals.h"
#include "MOR/MOR.h"

#define fn_p_GetEventsAddress(p_stAnim, ulFrame, hTable) \
    ((tdstEvent *(__cdecl *)(tdstAnim3d *, unsigned long, tdxHandleToObjectsTablesList)) \
        fn_hGetNextAlwaysActiveCharNode)((p_stAnim), (ulFrame), (hTable))

void __cdecl fn_vPlayLastEventsBeforeRestart(HIE_tdstSuperObject *);
void __cdecl fn_vDesInitAllEvents(HIE_tdstSuperObject *, tdxHandleToState, int);
/* 0x00418A20 */
void __cdecl fn_vAllocArrayOfElts3d(MS_tdxHandleTo3dData, unsigned short);

void __cdecl fn_vUpdateCurrentActivationForCharacter(HIE_tdstSuperObject *);
void __cdecl fn_vInitAllEvents(HIE_tdstSuperObject *, unsigned short);
unsigned char __cdecl GAM_fn_ucGetTypeOfEvent(tdstEvent *);
#define PLA_pfn_vUpdateActivationRaw fn_vUpdateCurrentActivationForCharacter
#define PLA_pfn_vInitAllEventsRaw    fn_vInitAllEvents

void __cdecl PLA_fn_vInitAllChildInHeapNewAnim(HIE_tdstSuperObject *, tdstAnim3d *, unsigned short);
void __cdecl PLA_fn_vDoFrame(tdstAnim3d *, unsigned short, HIE_tdstSuperObject *);
void __cdecl fn_vPlayEventsSinceLastOneNoLoop(HIE_tdstSuperObject *);
/* 0x00412060 */
void __cdecl fn_vTransferSpeedInformationPLA_DNM(HIE_tdstSuperObject *, short);
/* 0x0040EDF0 */
void __cdecl PLA_fn_vComputeNextFrame(HIE_tdstSuperObject *);

/* 0x0041A400 */
void __cdecl fn_vUpdateFrame(HIE_tdstSuperObject *, tdstFrame3d *, tdstAnim3d *, unsigned short);

/* 0x004192E0 */
tdstA3dGENERAL *__cdecl fn_p_GetA3dGENERAL(unsigned short uwNumOfA3dGENERAL, unsigned long ulFrameNumber);
unsigned char __cdecl GAM_fn_ucGetFirstCallOfEvent(tdstEvent *_p_stEvent);
/* 0x00418B50 */
void __cdecl fn_v_ExpandQuat(float *_a4_xQuatD, short *_a4_xQuatS);
void __cdecl fn_v_QuatToMatrix(float (*_a3a3_xMtx)[3], float *_a4_xQuat);
/* 0x004194D0 */
void __cdecl fn_vInitCSOList(CHN_tdxHandleToChannelArray *, unsigned long);

/* 0x0040F140 */
HIE_tdstSuperObject *__cdecl PLA_fn_hFindNextFreeSupObj(void);
void __cdecl PLA_fn_vReleaseSuperObjectInHeap(HIE_tdstSuperObject *);

/* 0x0040DFB0 */
tdstMorphData *__cdecl GAM_fn_p_stGetMorphData(tdstAnim3d *_p_stAnim, unsigned long _ulChannelNumber, unsigned short _uwFrameNumber);

void *__cdecl fn_hGetMorphedObjectInChannel(void *, unsigned short);

/* 0x00419580 */
void __cdecl fn_vUpdateSPOFlagsOfChannelForActivation(HIE_tdstSuperObject *, ACP_tdxBool);
void __cdecl fn_vUpdateFrameMatrix(HIE_tdstSuperObject *, tdstFrame3d *, tdstAnim3d *, unsigned short,
                                   unsigned char, CHN_tdxHandleToChannelArray);
/* 0x00419BD0 */
void __cdecl fn_v_InterpolBetweenKeys(tdstKeyFrame *, tdstKeyFrame *, tdstA3dChannel *, void *, unsigned short, float);

static const unsigned long A3X_xInterpParamScale_bits = 0x39000000u;
#define A3X_xInterpParamScale (*(const float *)&A3X_xInterpParamScale_bits)
#define A3X_xInterpParamOne   (-1.0f)

static int g_bAnimConstantSpeed_storage;
#define g_bAnimConstantSpeed (g_bAnimConstantSpeed_storage)

#define g_ulCurrentTimerCount GAM_g_ulAccumulatedTime

#define __PLA_NB_MAX_SUPOBJ__              512L

static HIE_tdstSuperObject *PLA_g_d_stSupObjHeap_storage;
static unsigned long PLA_g_ulNbAllocInSupObjHeap_storage;
static unsigned long PLA_g_ulCurrentPosInSupObjHeap_storage;
static unsigned short PLA_g_a_wStateOfSupObjHeap_storage[__PLA_NB_MAX_SUPOBJ__];
#define PLA_g_d_stSupObjHeap              (PLA_g_d_stSupObjHeap_storage)
#define PLA_g_ulNbAllocInSupObjHeap       (PLA_g_ulNbAllocInSupObjHeap_storage)
#define PLA_g_ulCurrentPosInSupObjHeap    (PLA_g_ulCurrentPosInSupObjHeap_storage)
#define PLA_g_a_wStateOfSupObjHeap        (PLA_g_a_wStateOfSupObjHeap_storage)

/* 0x004a705c */
static POS_tdstCompletePosition *PLA_g_d_stMatrixHeap_storage;
#define PLA_g_d_stMatrixHeap             (PLA_g_d_stMatrixHeap_storage)

/* 0x0040EF60 */
void __cdecl PLA_fn_vInitMatrixHeap(void)
{
    unsigned long i;

    for (i = 0; i < __PLA_NB_MAX_SUPOBJ__ + 1; i++)
    {
        POS_fn_vSetIdentityMatrix(&PLA_g_d_stMatrixHeap[i]);
    }
}

/* 0x0040EF80 */
void __cdecl PLA_fn_vCreateMatrixHeap(void)
{

    GEO_M_CPAMalloc(PLA_g_d_stMatrixHeap, GEO_tdxHandleToMatrix,
                    sizeof(POS_tdstCompletePosition) * (__PLA_NB_MAX_SUPOBJ__ + 1),
                    E_uwGEONotEnoughtMemory);
}

/* 0x0040F090 */
void __cdecl PLA_fn_vInitSupObjHeap(void)
{

    POS_tdstCompletePosition *p_stMatrix;
    HIE_tdstSuperObject       *p_stSO, *p_stSOStop;

    p_stMatrix = PLA_g_d_stMatrixHeap;
    p_stSO = PLA_g_d_stSupObjHeap;

    memset(PLA_g_a_wStateOfSupObjHeap, 0, sizeof(short) * __PLA_NB_MAX_SUPOBJ__);

    p_stSOStop = p_stSO + __PLA_NB_MAX_SUPOBJ__ + 1;
    while (p_stSO != p_stSOStop)
    {
        p_stSO->hLocalMatrix = p_stMatrix;
        p_stMatrix++;
        p_stSO++;
    }

    PLA_g_ulCurrentPosInSupObjHeap = 0L;
    PLA_g_ulNbAllocInSupObjHeap = 0L;
}

/* 0x0040F0E0 */
void __cdecl PLA_fn_vCreateSupObjHeap(void)
{

    PLA_g_d_stSupObjHeap = HIE_fn_p_stCreateSuperObjectTab(__PLA_NB_MAX_SUPOBJ__ + 1);
    g_Randomhandle = 0;
}

/* 0x0040F100 */
void __cdecl PLA_fn_vInitHeaps(void)
{
    PLA_fn_vInitMatrixHeap();
    PLA_fn_vInitSupObjHeap();
}

/* 0x0041B0F0 */
unsigned char __cdecl GAM_fn_ucGetFirstCallOfEvent(tdstEvent *_p_stEvent)
{
    return _p_stEvent->p_stfEventInTBL->ucFirstCall;
}

/* 0x004192E0 */
tdstA3dGENERAL *__cdecl fn_p_GetA3dGENERAL(unsigned short uwNumOfA3dGENERAL, unsigned long ulFrameNumber)
{
    (void)ulFrameNumber;
    return &p_stA3dGeneral[uwNumOfA3dGENERAL & 0xffff];
}

/* 0x004FB65C */
unsigned short *p_uwLastA3dGENERAL;

/* 0x0040DF10 */
extern unsigned long GAM_g_ulSkipStackAlloc;

unsigned char GAM_fn_ucGetMorphAmount(tdstAnim3d *_p_stAnim, unsigned long _ulChannelNumber, unsigned short _uwFrameNumber)
{
    tdstA3dGENERAL *p_stCurrentA3dGENERAL;
    tdstMorphData *p_stMorphArray;
    long i;

    if (GAM_g_ulSkipStackAlloc != 0)
        p_stCurrentA3dGENERAL = fn_p_GetA3dGENERAL(p_uwLastA3dGENERAL[_p_stAnim->uwNumOfA3dGENERAL], _uwFrameNumber);
    else
        p_stCurrentA3dGENERAL = fn_p_GetA3dGENERAL(_p_stAnim->uwNumOfA3dGENERAL, _uwFrameNumber);

    p_stMorphArray = &((tdstMorphData *)GAM_g_pAnimStack500288)[p_stCurrentA3dGENERAL->uwNumOfFirstMorphData];
    _p_stAnim->d_stMorphDataArray = p_stMorphArray;
    for (i = 0; i < p_stCurrentA3dGENERAL->uwNumberOfMorphData; ++i)
    {
        if ((p_stMorphArray[i].uwChannelNumber == _ulChannelNumber) && (p_stMorphArray[i].uwFrameNumber == _uwFrameNumber))
            return p_stMorphArray[i].ucMorphingAmount;
    }
    return 0;
}

/* 0x0040DFB0 */
tdstMorphData *__cdecl GAM_fn_p_stGetMorphData(tdstAnim3d *_p_stAnim, unsigned long _ulChannelNumber, unsigned short _uwFrameNumber)
{
    tdstA3dGENERAL *p_stCurrentA3dGENERAL;
    long i;

    if (GAM_g_ulSkipStackAlloc != 0)
        p_stCurrentA3dGENERAL = fn_p_GetA3dGENERAL(p_uwLastA3dGENERAL[_p_stAnim->uwNumOfA3dGENERAL], _uwFrameNumber);
    else
        p_stCurrentA3dGENERAL = fn_p_GetA3dGENERAL(_p_stAnim->uwNumOfA3dGENERAL, _uwFrameNumber);

    _p_stAnim->d_stMorphDataArray = &((tdstMorphData *)GAM_g_pAnimStack500288)[p_stCurrentA3dGENERAL->uwNumOfFirstMorphData];
    for (i = 0; i < p_stCurrentA3dGENERAL->uwNumberOfMorphData; ++i)
        if ((((tdstMorphData *)_p_stAnim->d_stMorphDataArray)[i].uwChannelNumber == _ulChannelNumber)
            && (((tdstMorphData *)_p_stAnim->d_stMorphDataArray)[i].uwFrameNumber == _uwFrameNumber))
            return &(((tdstMorphData *)_p_stAnim->d_stMorphDataArray)[i]);
    return NULL;
}

/* 0x0040E050 */
void __cdecl PLA_fn_vStopCurrentMorphAnim(HIE_tdstSuperObject *p_stSuperObject)
{
    MS_tdxHandleTo3dData h_Current3dData = M_GetMSHandle(p_stSuperObject, 3dData);
    tdstAnim3d *p_stCurrentAnim = fn_h3dDataGetCurrentState(h_Current3dData)->p_stAnim;
    unsigned short uwCurrentFrame = fn_uw3dDataGetCurrentFrame(h_Current3dData);
    short i;

    if (p_stCurrentAnim && p_stCurrentAnim->d_stMorphDataArray && fn_p_st3dDataGetCurrentFrame(h_Current3dData)->p_stArrayOfElts3d)
    {
        for (i = 0; i < p_stCurrentAnim->ucMaxNumberOfElements; ++i)
        {
            if (GAM_fn_ucGetMorphAmount(p_stCurrentAnim, i, uwCurrentFrame) < 100)
                MOR_fn_xStopMorph(p_stSuperObject, fn_p_st3dDataGetCurrentFrame(h_Current3dData)->p_stArrayOfElts3d[i].ucChannelNumber);
        }
    }
}

unsigned short __cdecl fn_uwGetNbEngineFrameSinceLastMechEvent(HIE_tdstSuperObject *p_stSuperObject)
{
    return M_GetMSHandle(p_stSuperObject, 3dData)->uwNbEngineFrameSinceLastMechEvent;
}

/* 0x0040FE50 */
unsigned short __cdecl PLA_fn_uwGetNbFrameInCurrAnim(HIE_tdstSuperObject *p_stSuperObject)
{
    MS_tdxHandleTo3dData h_Current3dData = M_GetMSHandle(p_stSuperObject, 3dData);

    return fn_h3dDataGetCurrentState(h_Current3dData)->p_stAnim->uwNumberOfFrames;
}

/* 0x0040FF10 */
long __cdecl PLA_fn_bTestNewState(HIE_tdstSuperObject *p_stSuperObject,
                                  tdxHandleToState h_WantedState)
{
    MS_tdxHandleTo3dData  h_Current3dData = M_GetMSHandle(p_stSuperObject, 3dData);
    tdxHandleToTransition h_Transition;

    if (h_WantedState == fn_h3dDataGetFirstStateOfAction(h_Current3dData))
        return TRUE;

    h_Transition = fn_hIfExistTransitionalWithTargetState(
                       fn_h3dDataGetWantedState(h_Current3dData), h_WantedState);

    if (h_Transition == NULL)
    {
        if (fn_hIfExistProhibitedState(fn_h3dDataGetWantedState(h_Current3dData), h_WantedState))
            return FALSE;
        if (fn_h3dDataGetWantedState(h_Current3dData)->ucTransitionStatusFlag == C_ucProhibitedState)
            return FALSE;
    }

    return TRUE;
}

/* 0x0040FF70 */
long __cdecl PLA_fn_bTestCurrentAction(HIE_tdstSuperObject *p_stSuperObject,
                                       tdxHandleToState h_FirstStateOfAction)
{
    MS_tdxHandleTo3dData h_Current3dData = M_GetMSHandle(p_stSuperObject, 3dData);

    if (h_FirstStateOfAction == fn_h3dDataGetFirstStateOfAction(h_Current3dData))
        return TRUE;

    return FALSE;
}

/* 0x00410340 */
long __cdecl PLA_fn_bTestIfEndOfAnim(HIE_tdstSuperObject *p_stSuperObject)
{
    MS_tdxHandleTo3dData h_3dData = M_GetMSHandle(p_stSuperObject, 3dData);

    if (h_3dData->ucFlagEndOfAnim)
        return TRUE;

    return FALSE;
}

/* 0x0040F110 */
void __cdecl PLA_fn_vCreateAndInitHeaps(void)
{

    PLA_fn_vCreateMatrixHeap();
    PLA_fn_vCreateSupObjHeap();
    PLA_fn_vInitMatrixHeap();
    PLA_fn_vInitSupObjHeap();
    PLA_g_ulMaxOccupationOfSupObjHeap = 0L;
    PLA_g_ulMaxOccupationOfChannels = 0L;
}

ACP_tdxBool __cdecl PLA_fn_bSetNewState(HIE_tdstSuperObject *p_stSuperObject,
                                        tdxHandleToState h_WantedState,
                                        ACP_tdxBool _bForce,
                                        ACP_tdxBool _bHandleSkippedEventsIfRelevant,
                                        ACP_tdxBool _bPcSkipTransition)
{
    MS_tdxHandleTo3dData  h_Current3dData = M_GetMSHandle(p_stSuperObject, 3dData);
    tdxHandleToState      h_ResultState;
    tdxHandleToTransition h_Transition;

    if ((h_WantedState == fn_h3dDataGetFirstStateOfAction(h_Current3dData)) && !_bForce)
        return (TRUE);

    if (!_bPcSkipTransition)
    {
        h_Transition = fn_hIfExistTransitionalWithTargetState(
            fn_h3dDataGetWantedState(h_Current3dData), h_WantedState);

        if (h_Transition == NULL)
        {
            if (fn_hIfExistProhibitedState(fn_h3dDataGetWantedState(h_Current3dData), h_WantedState)) return (FALSE);
            if (fn_h3dDataGetWantedState(h_Current3dData)->ucTransitionStatusFlag == C_ucProhibitedState) return (FALSE);
            else h_ResultState = h_WantedState;
        }
        else h_ResultState = fn_hGetStateToGoInTransition(h_Transition);
    }
    else
    {
        h_ResultState = h_WantedState;
        h_Transition = NULL;
    }

    PLA_fn_vStopCurrentMorphAnim(p_stSuperObject);

    if (h_Transition && (fn_ucGetLinkingTypeInTransition(h_Transition) == C_ucProportionalLink))
    {
        unsigned long ulForcedFrame, ulNewStartTime;

        ulForcedFrame = fn_uw3dDataGetCurrentFrame(h_Current3dData);
        ulForcedFrame *= h_ResultState->p_stAnim->uwNumberOfFrames;
        ulForcedFrame /= fn_h3dDataGetCurrentState(h_Current3dData)->p_stAnim->uwNumberOfFrames;
        if (ulForcedFrame >= h_ResultState->p_stAnim->uwNumberOfFrames)
            ulForcedFrame = h_ResultState->p_stAnim->uwNumberOfFrames - 1;

        ulNewStartTime = g_ulCurrentTimerCount - fn_ul3dDataGetStartTime(h_Current3dData);
        ulNewStartTime *= h_ResultState->p_stAnim->uwNumberOfFrames;
        ulNewStartTime /= fn_h3dDataGetCurrentState(h_Current3dData)->p_stAnim->uwNumberOfFrames;
        ulNewStartTime = g_ulCurrentTimerCount - ulNewStartTime;
        fn_v3dDataSetStartTime(h_Current3dData, ulNewStartTime);

        fn_v3dDataSetForcedFrame(h_Current3dData, (unsigned short)ulForcedFrame);
        fn_v3dDataSetLastFrame(h_Current3dData, (unsigned short)ulForcedFrame);

        fn_v3dDataSetTimePreviousFrame(h_Current3dData,
            fn_ul3dDataGetStartTime(h_Current3dData) + ulForcedFrame * 1000 / fn_scGetSpeedInState(h_ResultState));

        fn_v3dDataSetSkipCurrentFrame(h_Current3dData, FALSE);
    }
    else
    {
        fn_v3dDataSetTimeDelay(h_Current3dData, 0);
        fn_v3dDataSetCurrentFrame(h_Current3dData, 0);
        fn_v3dDataSetTimePreviousFrame(h_Current3dData, g_ulCurrentTimerCount);

        fn_v3dDataSetSkipCurrentFrame(h_Current3dData, FALSE);
    }

    fn_v3dDataSetNbEngineFrameSinceLastMechEvent(h_Current3dData, LME_INVALID);

    fn_v3dDataSetStateInLastFrame(h_Current3dData, fn_h3dDataGetCurrentState(h_Current3dData));
    fn_v3dDataSetWantedState(h_Current3dData, h_ResultState);
    fn_v3dDataSetFirstStateOfAction(h_Current3dData, h_WantedState);
    fn_v3dDataSetFlagModifState(h_Current3dData, TRUE);

    fn_v3dDataSetFlagEndOfAnim(h_Current3dData, (unsigned char)
        (fn_uw3dDataGetForcedFrame(h_Current3dData) == h_ResultState->p_stAnim->uwNumberOfFrames));

    PLA_fn_bDoAnimPlayerForCharacter(p_stSuperObject, FALSE, _bHandleSkippedEventsIfRelevant);

    return TRUE;
}

/* 0x00449C10 */
extern long __cdecl RND_fn_lGetLongRandomValue(long _lIndexNumber, long _lMin, long _lMax);

/* 0x0040FC50 */
ACP_tdxBool __cdecl PLA_fn_bSetNewStateWithRandom(HIE_tdstSuperObject *p_stSuperObject,
                                                  tdxHandleToState h_WantedState)
{
    MS_tdxHandleTo3dData  h_Current3dData = M_GetMSHandle(p_stSuperObject, 3dData);
    tdxHandleToState      h_ResultState = NULL;
    tdstAnim3d           *p_stNextAnim;
    unsigned short        uwFrameNumber;
    tdxHandleToTransition h_Transition;

    if (h_WantedState == fn_h3dDataGetFirstStateOfAction(h_Current3dData)) return (TRUE);

    h_Transition = fn_hIfExistTransitionalWithTargetState(
        fn_h3dDataGetWantedState(h_Current3dData), h_WantedState);

    if (h_ResultState == NULL)
    {
        if (fn_hIfExistProhibitedState(fn_h3dDataGetWantedState(h_Current3dData), h_WantedState)) return (FALSE);
        if (fn_h3dDataGetWantedState(h_Current3dData)->ucTransitionStatusFlag == C_ucProhibitedState) return (FALSE);
        else h_ResultState = h_WantedState;
    }
    else h_ResultState = fn_hGetStateToGoInTransition(h_Transition);

    fn_v3dDataSetNbEngineFrameSinceLastMechEvent(h_Current3dData, LME_INVALID);

    fn_v3dDataSetStartTime(h_Current3dData, 0);

    fn_v3dDataSetStateInLastFrame(h_Current3dData, fn_h3dDataGetCurrentState(h_Current3dData));
    fn_v3dDataSetWantedState(h_Current3dData, h_ResultState);
    fn_v3dDataSetFirstStateOfAction(h_Current3dData, h_WantedState);
    fn_v3dDataSetFlagModifState(h_Current3dData, TRUE);

    p_stNextAnim = fn_h3dDataGetWantedState(h_Current3dData)->p_stAnim;
    uwFrameNumber = (unsigned char)RND_fn_lGetLongRandomValue((long)g_Randomhandle, 0,
                                                              p_stNextAnim->uwNumberOfFrames - 1);
    fn_v3dDataSetForcedFrame(h_Current3dData, uwFrameNumber);
    fn_v3dDataSetLastFrame(h_Current3dData, (short)(uwFrameNumber - 1));
    fn_v3dDataSetTimeDelay(h_Current3dData, 0);
    fn_v3dDataSetTimePreviousFrame(h_Current3dData, g_ulCurrentTimerCount);

    PLA_fn_bDoAnimPlayerForCharacter(p_stSuperObject, FALSE, FALSE);

    return (TRUE);
}

#pragma optimize("y", off)

/* 0x0040E9E0 */
ACP_tdxBool __cdecl PLA_fn_bDoAnimPlayerForCharacter(HIE_tdstSuperObject *p_stSuperObject,
                                                     ACP_tdxBool bNoDisplay,
                                                     ACP_tdxBool _bHandleSkippedEventsIfRelevant)
{
    MS_tdxHandleTo3dData h_Current3dData = M_GetMSHandle(p_stSuperObject, 3dData);
    struct tdstState_  *p_stCurrentState  = fn_h3dDataGetCurrentState(h_Current3dData);
    struct tdstState_  *p_stTmpState;
    struct tdstAnim3d_ *p_stCurrentAnim;
    ACP_tdxBool bDidChangeState = FALSE;
    unsigned long ulAddFrames = 0;
    POS_tdstCompletePosition *p_stCharMatrix;
    MTH3D_tdstVector stCharTransVect;
    unsigned char ucStateSpeed;

    if (p_stCurrentState == NULL)
        return FALSE;

    p_stCurrentAnim = p_stCurrentState->p_stAnim;

    if (p_stCurrentAnim == NULL)
    {
        PLA_fn_vComputeNextFrame(p_stSuperObject);
        return TRUE;
    }

    if (fn_uw3dDataGetCurrentFrame(h_Current3dData) == p_stCurrentAnim->uwNumberOfFrames
        || fn_uc3dDataGetFlagModifState(h_Current3dData) == TRUE)
    {
        if (fn_uc3dDataGetFlagModifState(h_Current3dData) == FALSE)
            fn_vPlayLastEventsBeforeRestart(p_stSuperObject);
        else
            fn_vDesInitAllEvents(p_stSuperObject, fn_h3dDataGetWantedState(h_Current3dData), TRUE);

        if (fn_uc3dDataGetRepeatAnimation(h_Current3dData) > 0 && fn_uc3dDataGetFlagModifState(h_Current3dData) == FALSE)
        {

            if (g_bAnimConstantSpeed)
                ulAddFrames = fn_uw3dDataGetCurrentFrame(h_Current3dData) + 1;
            else
                ulAddFrames = fn_uc3dDataGetFrameRate(h_Current3dData) * fn_ul3dDataGetTimeDelay(h_Current3dData) / 1000;

            ulAddFrames %= p_stCurrentAnim->uwNumberOfFrames;
            fn_v3dDataSetStartTime(h_Current3dData,
                g_ulCurrentTimerCount - (1000 * ulAddFrames) / fn_uc3dDataGetFrameRate(h_Current3dData));
            fn_v3dDataSetCurrentFrame(h_Current3dData, (unsigned short)ulAddFrames);
            fn_v3dDataSetLastFrame(h_Current3dData, -1);
            fn_v3dDataDecRepeatAnimation(h_Current3dData);
        }
        else
        {
            if (fn_uc3dDataGetFlagModifState(h_Current3dData) == FALSE)
                fn_v3dDataSetWantedState(h_Current3dData,
                    fn_hGetNextStateInState(fn_h3dDataGetCurrentState(h_Current3dData)));

            p_stTmpState = fn_h3dDataGetWantedState(h_Current3dData);

            if (fn_h3dDataGetCurrentState(h_Current3dData) != p_stTmpState
                && fn_uc3dDataGetFlagModifState(h_Current3dData) == FALSE)
                fn_vDesInitAllEvents(p_stSuperObject, NULL, FALSE);

            bDidChangeState = (ACP_tdxBool)(p_stCurrentState != p_stTmpState
                || fn_uc3dDataGetFlagModifState(h_Current3dData) == TRUE);

            if (bDidChangeState)
            {

                p_stCharMatrix = HIE_fn_hGetSuperObjectMatrix(p_stSuperObject);
                MTH3D_M_vSubVector(&stCharTransVect, POS_fn_p_stGetTranslationVector(p_stCharMatrix),
                                   &p_stTmpState->p_stAnim->stOffsetMatrix);
                MTH3D_M_vAddVector(&stCharTransVect, &stCharTransVect,
                                   &p_stCurrentState->p_stAnim->stOffsetMatrix);
                POS_fn_vSetTranslationVector(p_stCharMatrix, &stCharTransVect);
                POS_fn_vMulMatrixMatrix(HIE_fn_hGetSuperObjectGlobalMatrix(p_stSuperObject),
                    HIE_fn_hGetSuperObjectGlobalMatrix(HIE_fn_hGetSuperObjectFather(p_stSuperObject)),
                    p_stCharMatrix);

                if (p_stCurrentAnim->ucMaxNumberOfElements != p_stTmpState->p_stAnim->ucMaxNumberOfElements
                    || fn_p_st3dDataGetCurrentFrame(h_Current3dData)->p_stArrayOfElts3d == NULL)
                    fn_vAllocArrayOfElts3d(h_Current3dData, p_stTmpState->p_stAnim->ucMaxNumberOfElements);

                fn_p_st3dDataGetCurrentFrame(h_Current3dData)->p_stAnim = NULL;
                fn_v3dDataSetCurrentState(h_Current3dData, p_stTmpState);
                PLA_pfn_vUpdateActivationRaw(p_stSuperObject);
                p_stCurrentState = fn_h3dDataGetCurrentState(h_Current3dData);
                p_stCurrentAnim  = p_stCurrentState->p_stAnim;
                ucStateSpeed = fn_scGetSpeedInState(fn_h3dDataGetCurrentState(h_Current3dData));

                if (_bHandleSkippedEventsIfRelevant)
                {
                    tdstEvent *p_stEvent;
                    int i;
                    unsigned char ucEventCount;

                    PLA_pfn_vInitAllEventsRaw(p_stSuperObject, 0);

                    p_stEvent = fn_p_GetEventsAddress(p_stCurrentAnim, 0, fn_h3dDataGetCurrentObjectsTable(h_Current3dData));
                    if (p_stEvent)
                        for (i = 0;
                             (i < p_stCurrentAnim->ucNumberOfEvents)
                             && (p_stEvent->uwFrameNumber <= fn_uw3dDataGetForcedFrame(h_Current3dData));
                             i++, p_stEvent++)
                            if (GAM_fn_ucGetTypeOfEvent(p_stEvent) == C_ucSOUND_EVENT)
                            {
                                ucEventCount = GAM_fn_ucGetFirstCallOfEvent(p_stEvent);
                                fn_v3dDataSetEventActivationByIndex(h_Current3dData, i,
                                    (unsigned char)(ucEventCount ? ucEventCount : (unsigned char)1));
                            }
                }
                else
                {
                    PLA_pfn_vInitAllEventsRaw(p_stSuperObject, fn_uw3dDataGetForcedFrame(h_Current3dData));
                }

                fn_v3dDataSetFrameRate(h_Current3dData, ucStateSpeed);
            }
            else
            {

                ucStateSpeed = fn_uc3dDataGetFrameRate(h_Current3dData);
            }

            if (g_bAnimConstantSpeed)
                ulAddFrames = fn_uw3dDataGetCurrentFrame(h_Current3dData) + 1;
            else
                ulAddFrames = ucStateSpeed * fn_ul3dDataGetTimeDelay(h_Current3dData) / 1000;

            ulAddFrames %= p_stCurrentAnim->uwNumberOfFrames;

            if (bDidChangeState)
                fn_v3dDataSetStateJustModified(h_Current3dData, TRUE);

            if (fn_uw3dDataGetForcedFrame(h_Current3dData) != 0)
            {
                fn_v3dDataSetCurrentFrame(h_Current3dData, fn_uw3dDataGetForcedFrame(h_Current3dData));
                fn_v3dDataSetForcedFrame(h_Current3dData, 0);
            }
            else
            {
                fn_v3dDataSetStartTime(h_Current3dData,
                    fn_ul3dDataGetTimePreviousFrame(h_Current3dData) - fn_ul3dDataGetTimeDelay(h_Current3dData));
                fn_v3dDataSetCurrentFrame(h_Current3dData, (unsigned short)ulAddFrames);
                fn_v3dDataSetLastFrame(h_Current3dData, -1);
            }

            fn_v3dDataSetRepeatAnimation(h_Current3dData, p_stCurrentState->ucRepeatAnimation);
            fn_v3dDataSetFlagEndState(h_Current3dData, FALSE);
            fn_v3dDataSetFlagModifState(h_Current3dData, FALSE);
            fn_v3dDataSetWantedState(h_Current3dData, p_stCurrentState);
        }
    }

    if (!bNoDisplay)
    {
        PLA_fn_vInitAllChildInHeapNewAnim(p_stSuperObject, p_stCurrentAnim,
                                       fn_uw3dDataGetCurrentFrame(h_Current3dData));
        PLA_fn_vDoFrame(p_stCurrentAnim, fn_uw3dDataGetCurrentFrame(h_Current3dData), p_stSuperObject);
    }

    if (!_bHandleSkippedEventsIfRelevant)
        fn_vPlayEventsSinceLastOneNoLoop(p_stSuperObject);

    fn_vTransferSpeedInformationPLA_DNM(p_stSuperObject, fn_s3dDataGetLastFrame(h_Current3dData));

    PLA_fn_vComputeNextFrame(p_stSuperObject);
    return TRUE;
}
#pragma optimize("", on)

/* 0x0040EDF0 */
void __cdecl PLA_fn_vComputeNextFrame(HIE_tdstSuperObject *p_stSuperObject)
{
    MS_tdxHandleTo3dData h_Current3dData = M_GetMSHandle(p_stSuperObject, 3dData);
    struct tdstState_  *p_stCurrentState  = fn_h3dDataGetCurrentState(h_Current3dData);
    struct tdstAnim3d_ *p_stCurrentAnim;
    unsigned long  ulNextFrameNumber;
    unsigned short uwNextFrameInAnim;

    if (p_stCurrentState == NULL)
        return;

    p_stCurrentAnim = p_stCurrentState->p_stAnim;

    if (p_stCurrentAnim == NULL)
        return;

    fn_v3dDataSetLastFrame(h_Current3dData, fn_uw3dDataGetCurrentFrame(h_Current3dData));

    if (g_bAnimConstantSpeed)
        ulNextFrameNumber = fn_uw3dDataGetCurrentFrame(h_Current3dData) + 1;
    else
        ulNextFrameNumber = fn_uc3dDataGetFrameRate(h_Current3dData)
            * (g_ulCurrentTimerCount - fn_ul3dDataGetStartTime(h_Current3dData)) / 1000;

    if (ulNextFrameNumber > p_stCurrentAnim->uwNumberOfFrames)
    {
        if (!fn_uc3dDataGetFlagEndOfAnim(h_Current3dData))
        {

            fn_v3dDataSetTimeDelay(h_Current3dData,
                g_ulCurrentTimerCount - fn_ul3dDataGetStartTime(h_Current3dData)
                - (1000 * p_stCurrentAnim->uwNumberOfFrames) / fn_uc3dDataGetFrameRate(h_Current3dData));
            uwNextFrameInAnim = p_stCurrentAnim->uwNumberOfFrames;
        }
        else
        {
            uwNextFrameInAnim = 0;
            fn_v3dDataSetTimeDelay(h_Current3dData, 0);
            fn_v3dDataSetStartTime(h_Current3dData, g_ulCurrentTimerCount);
        }
    }
    else
    {
        fn_v3dDataSetTimeDelay(h_Current3dData, 0);
        uwNextFrameInAnim = (unsigned short)ulNextFrameNumber;
    }

    if ((fn_uw3dDataGetCurrentFrame(h_Current3dData) == uwNextFrameInAnim)
        && !fn_b3dDataGetStateJustModified(h_Current3dData)
        && (uwNextFrameInAnim != p_stCurrentAnim->uwNumberOfFrames))
        fn_v3dDataSetSkipCurrentFrame(h_Current3dData, TRUE);

    if (p_stCurrentAnim->uwNumberOfFrames == 1
        && !fn_b3dDataGetStateJustModified(h_Current3dData)
        && fn_hGetNextStateInState(p_stCurrentState) == p_stCurrentState)
    {
        fn_v3dDataSetSkipCurrentFrame(h_Current3dData, TRUE);
        fn_v3dDataSetCurrentFrame(h_Current3dData, 0);
    }
    else
        fn_v3dDataSetCurrentFrame(h_Current3dData, uwNextFrameInAnim);

    fn_v3dDataSetFlagEndOfAnim(h_Current3dData,
        (unsigned char)(uwNextFrameInAnim == p_stCurrentAnim->uwNumberOfFrames));

    if (fn_uc3dDataGetRepeatAnimation(h_Current3dData) == 0
        && fn_uw3dDataGetCurrentFrame(h_Current3dData) == p_stCurrentAnim->uwNumberOfFrames
        && fn_hGetNextStateInState(p_stCurrentState) == NULL)
        fn_v3dDataSetFlagEndState(h_Current3dData, TRUE);

    fn_v3dDataSetStateJustModified(h_Current3dData, FALSE);
    fn_v3dDataSetTimePreviousFrame(h_Current3dData, g_ulCurrentTimerCount);
}

/* 0x0040F260 */
void __cdecl PLA_fn_vUpdateTransparencyForModules(HIE_tdstSuperObject *p_stSuperObject)
{
    MS_tdxHandleTo3dData  h_Current3dData;
    tdstAChannel         *p_stChannel;
    HIE_tdstSuperObject  *p_stTargetSO;
    register ACP_tdxBool  bPersoTransparencyActive = (ACP_tdxBool)((HIE_fn_lGetSuperObjectDrawMask(p_stSuperObject) & GLI_C_lIsNotGrided) == GLI_C_lIsNotGrided);
    MTH_tdxReal           fTransparence = HIE_fn_fGetSuperObjectTransparenceLevel(p_stSuperObject);

    h_Current3dData = M_GetMSHandle(p_stSuperObject, 3dData);
    p_stChannel = fn_h3dDataGetFirstActiveChannel(h_Current3dData);
    while (p_stChannel)
    {

        p_stTargetSO = p_stChannel->hSupObject;

        if ((HIE_fn_SO_ulGetFlags(p_stTargetSO) & HIE_C_Flag_ModuleTransparency) != HIE_C_Flag_ModuleTransparency)
        {
            HIE_fn_vSetSuperObjectTransparenceLevel(p_stTargetSO, fTransparence);

            if (bPersoTransparencyActive)
            {

                HIE_fn_vSetSuperObjectDrawMask(p_stTargetSO, HIE_fn_lGetSuperObjectDrawMask(p_stTargetSO) | GLI_C_lIsNotGrided);
            }
        }
        p_stChannel = p_stChannel->p_stNextActiveChannel;
    }
}

#define PLA_M_SuperObjectIsolate(hSO)                                               \
    do {                                                                            \
        if ((hSO)->p_stNextBrother != NULL)                                         \
            (hSO)->p_stNextBrother->p_stPrevBrother = (hSO)->p_stPrevBrother;       \
        if ((hSO)->p_stPrevBrother != NULL)                                         \
            (hSO)->p_stPrevBrother->p_stNextBrother = (hSO)->p_stNextBrother;       \
        if ((hSO)->p_stFather != NULL)                                              \
        {                                                                           \
            (hSO)->p_stFather->lNumberOfChildren--;                                 \
            if ((hSO)->p_stFather->p_stFirstChild == (hSO))                         \
                (hSO)->p_stFather->p_stFirstChild = (hSO)->p_stNextBrother;         \
            if ((hSO)->p_stFather->p_stLastChild == (hSO))                          \
                (hSO)->p_stFather->p_stLastChild = (hSO)->p_stPrevBrother;          \
        }                                                                           \
        (hSO)->p_stNextBrother = NULL;                                              \
        (hSO)->p_stPrevBrother = NULL;                                              \
        (hSO)->p_stFather      = NULL;                                              \
    } while (0)

/* 0x0040F2C0 */
void __cdecl PLA_fn_vInitAllChildInHeapNewAnim(HIE_tdstSuperObject *p_stSuperObject,
                                               tdstAnim3d *p_stCurrentAnim,
                                               unsigned short _uwFrameNumber)
{
    MS_tdxHandleTo3dData         h_Current3dData;
    tdstFrame3d                 *p_stCurrentFrame3d;
    tdstElement3d               *p_stElt3d;
    tdstElement3d               *p_stStopElt3d;
    register tdstAChannel       *p_stNewChannel;
    unsigned long                ulNextChannelNumber;
    tdstCouple                  *p_stNewCouples, *p_stOldCouples;

    CHN_tdxHandleToChannelArray  hChannelArray;
    register tdstAChannel       *p_stLastFrameChannel;
    register tdstAChannel       *p_stLastFrameChannelForLink;
    BOOL                         bHieChanged;

    h_Current3dData = M_GetMSHandle(p_stSuperObject, 3dData);
    hChannelArray = fn_h3dDataGetChannelSOList(h_Current3dData);
    p_stCurrentFrame3d = fn_p_st3dDataGetCurrentFrame(h_Current3dData);

    p_stOldCouples = fn_p_st3dDataGetCurrentHieCouples(h_Current3dData);

    fn_vUpdateFrame(p_stSuperObject, p_stCurrentFrame3d, p_stCurrentAnim, _uwFrameNumber);

    p_stElt3d = p_stCurrentFrame3d->p_stArrayOfElts3d;
    p_stNewCouples = p_stCurrentFrame3d->stHierarchy.d_stCouples;

    if (hChannelArray == NULL)
    {

        fn_vInitCSOList(&hChannelArray, fn_ul3dDataGetNumberOfChannels(h_Current3dData));
        fn_v3dDataSetChannelSOList(h_Current3dData, hChannelArray);

        fn_v3dDataSetFirstActiveChannel(h_Current3dData, NULL);
    }

    if (p_stOldCouples != p_stNewCouples)
    {
        register unsigned long  i;

        bHieChanged = TRUE;

        if (p_stOldCouples)
        {

            i = fn_ul3dDataGetCurrentHieNbCouples(h_Current3dData);
            while (i--)
            {

                hChannelArray[p_stOldCouples->wChild].p_stFatherChannel = NULL;

                p_stOldCouples++;
            }
        }

        i = p_stCurrentFrame3d->stHierarchy.ulNbOfCouples;

        fn_v3dDataSetCurrentHieCouples(h_Current3dData, p_stNewCouples);
        fn_v3dDataSetCurrentHieNbCouples(h_Current3dData, i);
        while (i--)
        {

            hChannelArray[p_stNewCouples->wChild].p_stFatherChannel = hChannelArray + p_stNewCouples->wFather;

            p_stNewCouples++;
        }

    }
    else
    {
        bHieChanged = FALSE;
    }

    p_stStopElt3d = p_stElt3d + p_stCurrentAnim->ucMaxNumberOfElements;

    if (p_stElt3d == p_stStopElt3d)
    {

        p_stNewChannel = NULL;
    }
    else
    {
        ulNextChannelNumber = p_stElt3d->ucChannelNumber;
        p_stNewChannel = (ulNextChannelNumber == C_ucUnknownChannel) ? NULL : hChannelArray + ulNextChannelNumber;
    }

    p_stLastFrameChannel = fn_h3dDataGetFirstActiveChannel(h_Current3dData);
    p_stLastFrameChannelForLink = NULL;

    while (p_stNewChannel || p_stLastFrameChannel)
    {
        if (p_stNewChannel == p_stLastFrameChannel)
        {

            if (bHieChanged)
            {

                if (p_stLastFrameChannel->p_stFatherChannel)
                {

                    if (p_stLastFrameChannel->p_stFatherChannel->hSupObject == NULL)
                    {

                        p_stLastFrameChannel->p_stFatherChannel->hSupObject = PLA_fn_hFindNextFreeSupObj();
                    }
                    HIE_fn_vSuperObjectAddTail(p_stLastFrameChannel->p_stFatherChannel->hSupObject, p_stLastFrameChannel->hSupObject);
                }
                else
                {
                    HIE_fn_vSuperObjectAddTail(p_stSuperObject, p_stLastFrameChannel->hSupObject);
                }
            }

            p_stLastFrameChannelForLink = p_stLastFrameChannel;
            p_stLastFrameChannel = p_stLastFrameChannel->p_stNextActiveChannel;
            p_stElt3d++;
            if (p_stElt3d == p_stStopElt3d)
            {
                p_stNewChannel = NULL;
            }
            else
            {
                ulNextChannelNumber = p_stElt3d->ucChannelNumber;
                p_stNewChannel = (ulNextChannelNumber == C_ucUnknownChannel) ? NULL : hChannelArray + ulNextChannelNumber;
            }
        }
        else if (p_stLastFrameChannel == NULL || (p_stNewChannel != NULL && p_stLastFrameChannel > p_stNewChannel))
        {

            if (p_stNewChannel->hSupObject == NULL)
            {
                p_stNewChannel->hSupObject = PLA_fn_hFindNextFreeSupObj();
            }

            if (p_stNewChannel->p_stFatherChannel)
            {
                if (p_stNewChannel->p_stFatherChannel->hSupObject == NULL)
                {

                    p_stNewChannel->p_stFatherChannel->hSupObject = PLA_fn_hFindNextFreeSupObj();
                }
                HIE_fn_vSuperObjectAddTail(p_stNewChannel->p_stFatherChannel->hSupObject, p_stNewChannel->hSupObject);
            }
            else
            {
                HIE_fn_vSuperObjectAddTail(p_stSuperObject, p_stNewChannel->hSupObject);
            }

            if (p_stLastFrameChannelForLink)
            {

                p_stNewChannel->p_stNextActiveChannel = p_stLastFrameChannelForLink->p_stNextActiveChannel;
                p_stLastFrameChannelForLink->p_stNextActiveChannel = p_stNewChannel;
            }
            else
            {

                p_stNewChannel->p_stNextActiveChannel = fn_h3dDataGetFirstActiveChannel(h_Current3dData);
                fn_v3dDataSetFirstActiveChannel(h_Current3dData, p_stNewChannel);
            }

            p_stLastFrameChannelForLink = p_stNewChannel;

            p_stElt3d++;
            if (p_stElt3d == p_stStopElt3d)
            {
                p_stNewChannel = NULL;
            }
            else
            {
                ulNextChannelNumber = p_stElt3d->ucChannelNumber;
                p_stNewChannel = (ulNextChannelNumber == C_ucUnknownChannel) ? NULL : hChannelArray + ulNextChannelNumber;
            }
        }
        else if (p_stNewChannel == NULL || p_stLastFrameChannel < p_stNewChannel)
        {

            if (p_stLastFrameChannel->hZoomSupObject)
            {

                PLA_M_SuperObjectIsolate(p_stLastFrameChannel->hZoomSupObject);
                PLA_fn_vReleaseSuperObjectInHeap(p_stLastFrameChannel->hZoomSupObject);
                p_stLastFrameChannel->hZoomSupObject = NULL;
            }

            PLA_M_SuperObjectIsolate(p_stLastFrameChannel->hSupObject);
            PLA_fn_vReleaseSuperObjectInHeap(p_stLastFrameChannel->hSupObject);
            p_stLastFrameChannel->hSupObject = NULL;

            if (p_stLastFrameChannelForLink)
            {
                p_stLastFrameChannelForLink->p_stNextActiveChannel = p_stLastFrameChannel->p_stNextActiveChannel;
            }
            else
            {

                fn_v3dDataSetFirstActiveChannel(h_Current3dData, p_stLastFrameChannel->p_stNextActiveChannel);
            }

            p_stLastFrameChannel = p_stLastFrameChannel->p_stNextActiveChannel;
        }
    }
}

#define PLA_M_vDynamicIsolateSO(hElement)                                                       \
{                                                                                               \
    if ((hElement)->p_stNextBrother != NULL)                                                    \
        (hElement)->p_stNextBrother->p_stPrevBrother = (hElement)->p_stPrevBrother;             \
    if ((hElement)->p_stPrevBrother != NULL)                                                    \
        (hElement)->p_stPrevBrother->p_stNextBrother = (hElement)->p_stNextBrother;             \
    if ((hElement)->p_stFather != NULL)                                                         \
    {                                                                                           \
        (hElement)->p_stFather->lNumberOfChildren--;                                            \
        if ((hElement)->p_stFather->p_stFirstChild == (hElement))                               \
            (hElement)->p_stFather->p_stFirstChild = (hElement)->p_stNextBrother;               \
        if ((hElement)->p_stFather->p_stLastChild == (hElement))                                \
            (hElement)->p_stFather->p_stLastChild = (hElement)->p_stPrevBrother;                \
    }                                                                                           \
    (hElement)->p_stNextBrother = NULL;                                                         \
    (hElement)->p_stPrevBrother = NULL;                                                         \
    (hElement)->p_stFather = NULL;                                                              \
}

#include "SPO/HieDef.h"

#define HIE_fn_vSetSuperObjectObjectAndType(_hSprObj, _hObj, _lType) \
{\
  HIE_M_vSetSuperObjectMember(_hSprObj, ulTypeOfLinkedObject, _hObj ? _lType : HIE_C_ulSuperObject);\
	HIE_M_vSetSuperObjectMember(_hSprObj, hLinkedObject.p_Void, _hObj);\
}
void __cdecl PLA_fn_vDoFrame(tdstAnim3d *p_stCurrentAnim, unsigned short uwCurrentFrame,
                             HIE_tdstSuperObject *p_stSuperObject)
{
    tdstFrame3d                  *p_stCurrentFrame3d;
    tdstElement3d                *p_stElt3d;
    tdstElement3d                *p_stElt3dStop;
    MS_tdxHandleTo3dData          h_Current3dData;
    HIE_tdstSuperObject          *p_stTargetSO;
    tdstAChannel                 *p_stChannel;
    tdstObjectsTableElement      *d_stCurrentObjectsTable, *p_stCurrentObjectsTableElement;
    short                         wElement;
    MTH_tdxReal                   f_Transparence;
    void                         *hPhysicalObject;

    h_Current3dData = M_GetMSHandle(p_stSuperObject, 3dData);

    f_Transparence = HIE_fn_fGetSuperObjectTransparenceLevel(p_stSuperObject);
    d_stCurrentObjectsTable = fn_h3dDataGetCurrentObjectsTable(h_Current3dData)->d_stObjectsTable;

    p_stCurrentFrame3d = fn_p_st3dDataGetCurrentFrame(h_Current3dData);
    fn_vUpdateFrame(p_stSuperObject, p_stCurrentFrame3d, p_stCurrentAnim, uwCurrentFrame);

    p_stElt3d = p_stCurrentFrame3d->p_stArrayOfElts3d;
    p_stElt3dStop = p_stElt3d + p_stCurrentAnim->ucMaxNumberOfElements;

    while (p_stElt3d != p_stElt3dStop)
    {
        if (p_stElt3d->ucChannelNumber == C_ucUnknownChannel)
        {
            p_stElt3d++;
            continue;
        }

        p_stChannel = fn_hGetCSOForChannel(fn_h3dDataGetChannelSOList(h_Current3dData), p_stElt3d->ucChannelNumber);
        p_stTargetSO = p_stChannel->hSupObject;

        if (p_stElt3d->eTypeOfElement == TE_GraphicObject && p_stElt3d->bf1ActiveStatus)
        {
            wElement = (short)p_stElt3d->wElement;
            p_stCurrentObjectsTableElement = d_stCurrentObjectsTable + wElement;

            if (p_stCurrentObjectsTableElement->wTypeOfTarget != C_wTdO_Undefined)
            {
                if (p_stCurrentObjectsTableElement->p_stCustomZoom)
                {
                    MTH3D_tdstVector stCustomZoomI, stCustomZoomJ, stCustomZoomK;
                    MTH_tdxReal f0, f4, f8;
                    HIE_tdstSuperObject *p_stSO_Child;

                    if (p_stChannel->hZoomSupObject == NULL)
                    {

                        HIE_fn_vSetSuperObjectObjectAndType(p_stTargetSO, NULL, HIE_C_ulSuperObject);
                        p_stSO_Child = PLA_fn_hFindNextFreeSupObj();
                        HIE_fn_vSuperObjectAddTail(p_stTargetSO, p_stSO_Child);
                        p_stChannel->hZoomSupObject = p_stSO_Child;
                    }
                    else
                    {
                        p_stSO_Child = p_stChannel->hZoomSupObject;
                    }

                    if (p_stCurrentAnim->d_stMorphDataArray == NULL)
                    {
                        if (p_stCurrentObjectsTableElement->wTypeOfTarget == C_wTdO_PhysicalObject)
                        {
                            hPhysicalObject = fn_hGetMorphedObjectInChannel(fn_h3dDataGetMorphList(h_Current3dData), p_stElt3d->ucChannelNumber);
                            if (!hPhysicalObject)
                                hPhysicalObject = p_stCurrentObjectsTableElement->h_Target;
                            HIE_fn_vSetSuperObjectObjectAndType(p_stSO_Child, hPhysicalObject, HIE_C_ulPO);
                        }
                        else if (p_stCurrentObjectsTableElement->wTypeOfTarget == C_wTdO_Mirror)
                        {
                            HIE_fn_vSetSuperObjectObjectAndType(p_stSO_Child, p_stCurrentObjectsTableElement->h_Target, HIE_C_ulPO_Mirror);
                        }
                    }

                    else
                    {
                        if (p_stCurrentObjectsTableElement->wTypeOfTarget == C_wTdO_PhysicalObject)
                        {
                            short xMorphTaskIndex;
                            tdstMorphData *p_stMorphData;

                            p_stMorphData = GAM_fn_p_stGetMorphData(p_stCurrentAnim, p_stElt3d->ucChannelNumber, uwCurrentFrame);

                            if (p_stMorphData == NULL)
                            {
                                MOR_fn_xStopMorph(p_stSuperObject, p_stElt3d->ucChannelNumber);
                                HIE_fn_vSetSuperObjectObjectAndType(p_stSO_Child, p_stCurrentObjectsTableElement->h_Target, HIE_C_ulPO);
                            }
                            else
                            {
                                if (p_stMorphData->ucMorphingAmount < 100)
                                {
                                    xMorphTaskIndex = MOR_fn_xMorphing(p_stSuperObject, wElement,
                                        p_stMorphData->ucTarget, p_stMorphData->ucMorphingAmount, p_stElt3d->ucChannelNumber);
                                    if (xMorphTaskIndex != -1)
                                    {
                                        MOR_fn_vDoMorph(xMorphTaskIndex);
                                        hPhysicalObject = MOR_fn_hGetMorphedObject(xMorphTaskIndex);
                                        HIE_fn_vSetSuperObjectObjectAndType(p_stSO_Child, hPhysicalObject, HIE_C_ulPO);
                                    }
                                    else
                                    {
                                        HIE_fn_vSetSuperObjectObjectAndType(p_stSO_Child, d_stCurrentObjectsTable[wElement].h_Target, HIE_C_ulPO);
                                    }
                                }
                                else
                                {
                                    MOR_fn_xStopMorph(p_stSuperObject, p_stElt3d->ucChannelNumber);
                                    hPhysicalObject = d_stCurrentObjectsTable[p_stMorphData->ucTarget].h_Target;
                                    HIE_fn_vSetSuperObjectObjectAndType(p_stSO_Child, hPhysicalObject, HIE_C_ulPO);
                                }
                            }
                        }
                        else if (p_stCurrentObjectsTableElement->wTypeOfTarget == C_wTdO_Mirror)
                        {
                            HIE_fn_vSetSuperObjectObjectAndType(p_stSO_Child, p_stCurrentObjectsTableElement->h_Target, HIE_C_ulPO_Mirror);
                        }
                    }

                    HIE_fn_vSetSuperObjectBoundingVolume(p_stSO_Child, NULL);

                    POS_fn_vSetIdentityMatrix(HIE_fn_hGetSuperObjectMatrix(p_stSO_Child));
                    MTH3D_M_vGetVectorElements(&f0, &f4, &f8, p_stCurrentObjectsTableElement->p_stCustomZoom);
                    MTH3D_M_vSetVectorElements(&stCustomZoomI, f0, MTH_C_ZERO, MTH_C_ZERO);
                    MTH3D_M_vSetVectorElements(&stCustomZoomJ, MTH_C_ZERO, f4, MTH_C_ZERO);
                    MTH3D_M_vSetVectorElements(&stCustomZoomK, MTH_C_ZERO, MTH_C_ZERO, f8);
                    POS_fn_vSetScaleMatrix(HIE_fn_hGetSuperObjectMatrix(p_stSO_Child), &stCustomZoomI, &stCustomZoomJ, &stCustomZoomK);

                    if (MTH_M_bLessZero(MTH_M_xMul(f0, MTH_M_xMul(f4, f8))))
                    {
                        HIE_fn_vSetSuperObjectDrawMask(p_stSO_Child, HIE_fn_lGetSuperObjectDrawMask(p_stSO_Child) & (GLI_C_lAllIsEnable - (GLI_C_lNotInvertBackfaces)));
                    }

                    if ((HIE_fn_SO_ulGetFlags(p_stSO_Child) & HIE_C_Flag_ModuleTransparency) != HIE_C_Flag_ModuleTransparency)
                    {
                        if ((HIE_fn_lGetSuperObjectDrawMask(p_stSuperObject) & GLI_C_lIsNotGrided) == GLI_C_lIsNotGrided)
                        {
                            if (p_stElt3d->ucTransparency == 0xFF)
                            {
                                HIE_fn_vSetSuperObjectTransparenceLevel(p_stSO_Child, f_Transparence);
                                HIE_fn_vSetSuperObjectDrawMask(p_stSO_Child, HIE_fn_lGetSuperObjectDrawMask(p_stSO_Child) | GLI_C_lIsNotGrided);
                            }
                            else
                            {
                                HIE_fn_vSetSuperObjectTransparenceLevel(p_stSO_Child, MTH_M_xLongToReal(p_stElt3d->ucTransparency));
                                HIE_fn_vSetSuperObjectDrawMask(p_stSO_Child, HIE_fn_lGetSuperObjectDrawMask(p_stSO_Child) & ~GLI_C_lIsNotGrided);
                            }
                        }
                        else
                            HIE_fn_vSetSuperObjectTransparenceLevel(p_stSO_Child, f_Transparence);
                    }
                }

                else
                {

                    if (p_stChannel->hZoomSupObject != NULL)
                    {
                        PLA_M_vDynamicIsolateSO(p_stChannel->hZoomSupObject);
                        PLA_fn_vReleaseSuperObjectInHeap(p_stChannel->hZoomSupObject);
                        p_stChannel->hZoomSupObject = NULL;
                    }

                    if (p_stCurrentAnim->d_stMorphDataArray == NULL)
                    {
                        if (p_stCurrentObjectsTableElement->wTypeOfTarget == C_wTdO_PhysicalObject)
                        {
                            hPhysicalObject = fn_hGetMorphedObjectInChannel(fn_h3dDataGetMorphList(h_Current3dData), p_stElt3d->ucChannelNumber);
                            if (!hPhysicalObject)
                                hPhysicalObject = p_stCurrentObjectsTableElement->h_Target;
                            HIE_fn_vSetSuperObjectObjectAndType(p_stTargetSO, hPhysicalObject, HIE_C_ulPO);
                        }
                        else if (p_stCurrentObjectsTableElement->wTypeOfTarget == C_wTdO_Mirror)
                        {
                            HIE_fn_vSetSuperObjectObjectAndType(p_stTargetSO, p_stCurrentObjectsTableElement->h_Target, HIE_C_ulPO_Mirror);
                        }
                    }

                    else
                    {
                        if (p_stCurrentObjectsTableElement->wTypeOfTarget == C_wTdO_PhysicalObject)
                        {
                            short xMorphTaskIndex;
                            tdstMorphData *p_stMorphData;

                            p_stMorphData = GAM_fn_p_stGetMorphData(p_stCurrentAnim, p_stElt3d->ucChannelNumber, uwCurrentFrame);

                            if (p_stMorphData == NULL)
                            {
                                MOR_fn_xStopMorph(p_stSuperObject, p_stElt3d->ucChannelNumber);
                                HIE_fn_vSetSuperObjectObjectAndType(p_stTargetSO, p_stCurrentObjectsTableElement->h_Target, HIE_C_ulPO);
                            }
                            else
                            {
                                if (p_stMorphData->ucMorphingAmount < 100)
                                {
                                    xMorphTaskIndex = MOR_fn_xMorphing(p_stSuperObject, wElement,
                                        p_stMorphData->ucTarget, p_stMorphData->ucMorphingAmount, p_stElt3d->ucChannelNumber);
                                    if (xMorphTaskIndex != -1)
                                    {
                                        MOR_fn_vDoMorph(xMorphTaskIndex);
                                        hPhysicalObject = MOR_fn_hGetMorphedObject(xMorphTaskIndex);
                                        HIE_fn_vSetSuperObjectObjectAndType(p_stTargetSO, hPhysicalObject, HIE_C_ulPO);
                                    }
                                    else
                                    {
                                        HIE_fn_vSetSuperObjectObjectAndType(p_stTargetSO, d_stCurrentObjectsTable[wElement].h_Target, HIE_C_ulPO);
                                    }
                                }
                                else
                                {
                                    MOR_fn_xStopMorph(p_stSuperObject, p_stElt3d->ucChannelNumber);
                                    hPhysicalObject = d_stCurrentObjectsTable[p_stMorphData->ucTarget].h_Target;
                                    HIE_fn_vSetSuperObjectObjectAndType(p_stTargetSO, hPhysicalObject, HIE_C_ulPO);
                                }
                            }
                        }
                        else if (p_stCurrentObjectsTableElement->wTypeOfTarget == C_wTdO_Mirror)
                        {
                            HIE_fn_vSetSuperObjectObjectAndType(p_stTargetSO, p_stCurrentObjectsTableElement->h_Target, HIE_C_ulPO_Mirror);
                        }
                    }

                    HIE_fn_vSetSuperObjectBoundingVolume(p_stTargetSO, NULL);
                }
            }

            else
            {
                HIE_fn_vSetSuperObjectObjectAndType(p_stTargetSO, NULL, HIE_C_ulSuperObject);
            }
        }

        if ((p_stElt3d->eTypeOfElement == TE_EmptyObject) ||
            (p_stElt3d->eTypeOfElement == TE_Event) ||
            (p_stElt3d->eTypeOfElement == TE_Fake) ||
            !p_stElt3d->bf1ActiveStatus)
        {

            if (p_stChannel->hZoomSupObject)
            {
                PLA_M_vDynamicIsolateSO(p_stChannel->hZoomSupObject);
                PLA_fn_vReleaseSuperObjectInHeap(p_stChannel->hZoomSupObject);
                p_stChannel->hZoomSupObject = NULL;
            }
            HIE_fn_vSetSuperObjectObjectAndType(p_stTargetSO, NULL, HIE_C_ulSuperObject);
        }

        if ((HIE_fn_SO_ulGetFlags(p_stTargetSO) & HIE_C_Flag_ModuleTransparency) != HIE_C_Flag_ModuleTransparency)
        {
            if ((HIE_fn_lGetSuperObjectDrawMask(p_stSuperObject) & GLI_C_lIsNotGrided) == GLI_C_lIsNotGrided)
            {
                if (p_stElt3d->ucTransparency == 0xFF)
                {
                    HIE_fn_vSetSuperObjectTransparenceLevel(p_stTargetSO, f_Transparence);
                    HIE_fn_vSetSuperObjectDrawMask(p_stTargetSO, HIE_fn_lGetSuperObjectDrawMask(p_stTargetSO) | GLI_C_lIsNotGrided);
                }
                else
                {
                    HIE_fn_vSetSuperObjectTransparenceLevel(p_stTargetSO, MTH_M_xLongToReal(p_stElt3d->ucTransparency));
                    HIE_fn_vSetSuperObjectDrawMask(p_stTargetSO, HIE_fn_lGetSuperObjectDrawMask(p_stTargetSO) & ~GLI_C_lIsNotGrided);
                }
            }
            else
                HIE_fn_vSetSuperObjectTransparenceLevel(p_stTargetSO, f_Transparence);
        }
        fn_vUpdateSPOFlagsOfChannelForActivation(
            p_stChannel->hZoomSupObject ? p_stChannel->hZoomSupObject : p_stChannel->hSupObject,
            p_stChannel->bActiveChannel);

        p_stElt3d++;
    }

    fn_vUpdateFrameMatrix(p_stSuperObject, p_stCurrentFrame3d, p_stCurrentAnim, uwCurrentFrame,
        (unsigned char)fn_b3dDataHasAnimMatrixChanged(h_Current3dData), fn_h3dDataGetChannelSOList(h_Current3dData));
    fn_v3dDataResetAnimMatrixHasChanged(h_Current3dData);

    PLA_fn_vRefreshGlobalMatrixUnderCharacter(p_stSuperObject);
}
#undef HIE_fn_vSetSuperObjectObjectAndType

/* 0x00413700 */
extern unsigned long __cdecl fn_ulStandardGameGetCustomBitsSO(void *hSupObj);

/* 0x0040FA60 */
void __cdecl PLA_fn_vRefreshGlobalMatrixOfAnimsHierarchy(HIE_tdstSuperObject *_hSprObj)
{
    register long lType;
    HIE_tdstSuperObject *hChild;

    lType = _hSprObj->ulTypeOfLinkedObject;

    if (lType & (HIE_C_ulSuperObject | HIE_C_ulPO | HIE_C_ulPO_Mirror))
    {
        POS_fn_vMulMatrixMatrix(HIE_fn_hGetSuperObjectGlobalMatrix(_hSprObj), HIE_fn_hGetSuperObjectGlobalMatrix(HIE_fn_hGetSuperObjectFather(_hSprObj)), HIE_fn_hGetSuperObjectMatrix(_hSprObj));
    }
    else return;

    for (hChild = _hSprObj->p_stFirstChild; hChild != NULL; hChild = hChild->p_stNextBrother)
        PLA_fn_vRefreshGlobalMatrixOfAnimsHierarchy(hChild);
}

/* 0x0040FA30 */
void __cdecl PLA_fn_vRefreshGlobalMatrixUnderCharacter(HIE_tdstSuperObject *_hSprObj)
{
    HIE_tdstSuperObject *hChild;

    if (fn_ulStandardGameGetCustomBitsSO(_hSprObj) & GAM_C_CustBitNeedModuleMatrices)
    {
        for (hChild = _hSprObj->p_stFirstChild; hChild != NULL; hChild = hChild->p_stNextBrother)
            PLA_fn_vRefreshGlobalMatrixOfAnimsHierarchy(hChild);
    }
}

/* 0x0040F140 */
HIE_tdstSuperObject *__cdecl PLA_fn_hFindNextFreeSupObj(void)
{
    HIE_tdstSuperObject *p_stSO_Child;

    PLA_g_ulNbAllocInSupObjHeap++;

    if (PLA_g_ulNbAllocInSupObjHeap >= __PLA_NB_MAX_SUPOBJ__)
    {

        p_stSO_Child = HIE_fn_hGetElementFromSuperObjectTab(PLA_g_d_stSupObjHeap, __PLA_NB_MAX_SUPOBJ__);
    }
    else
    {
        while (PLA_g_a_wStateOfSupObjHeap[PLA_g_ulCurrentPosInSupObjHeap])
        {
            PLA_g_ulCurrentPosInSupObjHeap++;
            if (PLA_g_ulCurrentPosInSupObjHeap == __PLA_NB_MAX_SUPOBJ__)
                PLA_g_ulCurrentPosInSupObjHeap = 0L;
        }

        PLA_g_a_wStateOfSupObjHeap[PLA_g_ulCurrentPosInSupObjHeap] = 1;
        p_stSO_Child = HIE_fn_hGetElementFromSuperObjectTab(PLA_g_d_stSupObjHeap, PLA_g_ulCurrentPosInSupObjHeap);
    }

    HIE_fn_vSetSuperObjectDrawMask(p_stSO_Child, GLI_C_lAllIsEnable);
    HIE_fn_SO_vSetFlags(p_stSO_Child, 0);

    if (PLA_g_ulMaxOccupationOfSupObjHeap < PLA_g_ulNbAllocInSupObjHeap)
        PLA_g_ulMaxOccupationOfSupObjHeap++;

    HIE_fn_vSuperObjectIsolate(p_stSO_Child);

    p_stSO_Child->ulTypeOfLinkedObject = HIE_C_ulSuperObject;
    p_stSO_Child->hLinkedObject.p_Void = NULL;

    return p_stSO_Child;
}

/* 0x0040F220 */
void __cdecl PLA_fn_vReleaseSuperObjectInHeap(HIE_tdstSuperObject *_hSupObj)
{
    long lPos;

    if (_hSupObj != HIE_fn_hGetElementFromSuperObjectTab(PLA_g_d_stSupObjHeap, __PLA_NB_MAX_SUPOBJ__))
    {
        lPos = HIE_fn_hGetIndexFromSuperObjectTabAndElement(PLA_g_d_stSupObjHeap, _hSupObj);
        PLA_g_a_wStateOfSupObjHeap[lPos] = 0;
    }

    PLA_g_ulNbAllocInSupObjHeap--;
}

/* 0x004194D0 */
void __cdecl fn_vInitCSOList(CHN_tdxHandleToChannelArray *p_hChannelArray, unsigned long ulNumberOfChannels)
{
    unsigned long ulI;

    *p_hChannelArray = (CHN_tdxHandleToChannelArray)TMP_M_p_Malloc(sizeof(tdstAChannel) * ulNumberOfChannels);
    for (ulI = 0; ulI < ulNumberOfChannels; ulI++)
    {
        (*p_hChannelArray)[ulI].bActiveChannel = TRUE;
        (*p_hChannelArray)[ulI].bControlledChannel = FALSE;
    }
}

/* 0x00419510 */
void __cdecl fn_vFreeCSOList(CHN_tdxHandleToChannelArray *p_hChannelArray)
{
    CHN_tdxHandleToChannelArray hArray = *p_hChannelArray;

    if (hArray == NULL)
        return;

    TMP_M_Free(hArray);
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

/* 0x0040F840 */
void __cdecl PLA_fn_vDesInitAllChildOfCharacter(HIE_tdstSuperObject *p_stSuperObject,
                                                struct tdstEngineObject_ *p_stObject)
{
    MS_tdxHandleTo3dData         h_Current3dData;
    tdstAnim3d                  *p_stCurrentAnim = NULL;
    CHN_tdxHandleToChannelArray  hArray;
    CHN_tdxHandleToChannel       p_stChannel;
    tdstFrame3d                 *p_stCurrentFrame3d;

    if (p_stObject == NULL)
        return;

    h_Current3dData = (MS_tdxHandleTo3dData)p_stObject->h_3dData;

    if (fn_h3dDataGetCurrentState(h_Current3dData) != NULL)
        p_stCurrentAnim = fn_h3dDataGetCurrentState(h_Current3dData)->p_stAnim;

    p_stChannel = fn_h3dDataGetFirstActiveChannel(h_Current3dData);
    hArray      = fn_h3dDataGetChannelSOList(h_Current3dData);
    if (hArray != NULL)
    {
        while (p_stChannel != NULL)
        {

            if (p_stSuperObject != NULL && p_stCurrentAnim != NULL
                && p_stCurrentAnim->d_stMorphDataArray != NULL)
                MOR_fn_xStopMorph(p_stSuperObject, (unsigned short)(p_stChannel - hArray));

            if (p_stChannel->hZoomSupObject != NULL)
            {
                PLA_M_vDynamicIsolateSO(p_stChannel->hZoomSupObject);
                PLA_fn_vReleaseSuperObjectInHeap(p_stChannel->hZoomSupObject);
                p_stChannel->hZoomSupObject = NULL;
            }

            if (p_stChannel->hSupObject != NULL)
            {
                PLA_M_vDynamicIsolateSO(p_stChannel->hSupObject);
                PLA_fn_vReleaseSuperObjectInHeap(p_stChannel->hSupObject);
                p_stChannel->hSupObject = NULL;
            }
            p_stChannel = p_stChannel->p_stNextActiveChannel;
        }
    }

    fn_v3dDataSetSkipCurrentFrame(h_Current3dData, FALSE);

    if (p_stSuperObject != NULL)
        fn_vDesInitAllEvents(p_stSuperObject, NULL, TRUE);

    fn_vFreeCSOList(&hArray);
    fn_v3dDataSetChannelSOList(h_Current3dData, NULL);
    fn_v3dDataSetFirstActiveChannel(h_Current3dData, NULL);

    fn_vFreeArrayOfElts3d(h_Current3dData);

    fn_p_st3dDataGetCurrentFrame(h_Current3dData)->p_stAnim = NULL;
    p_stCurrentFrame3d = fn_p_st3dDataGetCurrentFrame(h_Current3dData);

    if (p_stCurrentFrame3d->p_stAngularSpeedMatrix != NULL)
    {
        TMP_fn_vFree(&p_stCurrentFrame3d->p_stAngularSpeedMatrix);
        p_stCurrentFrame3d->p_stAngularSpeedMatrix = NULL;
    }

    fn_v3dDataSetCurrentHieCouples(h_Current3dData, NULL);
    fn_v3dDataSetCurrentHieNbCouples(h_Current3dData, 0);
    fn_v3dDataSetFlagModifState(h_Current3dData, TRUE);
    fn_v3dDataSetSkipCurrentFrame(h_Current3dData, FALSE);
}

/* 0x00419580 */
void __cdecl fn_vUpdateSPOFlagsOfChannelForActivation(HIE_tdstSuperObject *_hSOChannel, ACP_tdxBool _bActivate)
{
    unsigned long ulSpoFlags;
    if (_hSOChannel)
    {
        ulSpoFlags = HIE_fn_SO_ulGetFlags(_hSOChannel);
        if (_bActivate)
        {

            HIE_fn_SO_vSetFlags(_hSOChannel,
                ulSpoFlags & ~(HIE_C_Flag_NotPickable | HIE_C_Flag_ulHidden | HIE_C_Flag_ulNoShadowOnMe));
        }
        else
        {

            HIE_fn_SO_vSetFlags(_hSOChannel,
                ulSpoFlags | HIE_C_Flag_NotPickable | HIE_C_Flag_ulHidden | HIE_C_Flag_ulNoShadowOnMe);
        }
    }
}

MTH3D_tdstVector *__cdecl fn_p_stGetLinearSpeedVector(tdstAnim3d *_p_stAnim3d, unsigned short _uwFrame)
{
    unsigned short uwNumOfA3dGENERAL;
    tdstA3dGENERAL *p_stCurrentA3dGENERAL;
    tdstOnlyFrame *p_stCurrentOnlyFrame;

    if (_uwFrame >= _p_stAnim3d->uwNumberOfFrames)
        return NULL;

    uwNumOfA3dGENERAL = _p_stAnim3d->uwNumOfA3dGENERAL;
    p_stCurrentA3dGENERAL = fn_p_GetA3dGENERAL(uwNumOfA3dGENERAL, _uwFrame);

    _uwFrame -= p_stCurrentA3dGENERAL->uwStartFrame;

    p_stCurrentOnlyFrame = &p_stOnlyFrames[p_stCurrentA3dGENERAL->uwNumOfFirstOnlyFrame + _uwFrame];

    if (p_stCurrentOnlyFrame->uwNumOfSpeedVector == SEB_Invalid)
        return NULL;
    return &p_stSpeedVectors[p_stCurrentOnlyFrame->uwNumOfSpeedVector];
}

/* 0x0040FF90 */
MTH3D_tdstVector *__cdecl PLA_fn_stGetLinearSpeedVector(HIE_tdstSuperObject *p_stSuperObject, short sLastFrame)
{
    MS_tdxHandleTo3dData h_Current3dData;
    struct tdstState_ *p_stCurrentState;
    tdstAnim3d *p_stCurrentAnim;
    static MTH3D_tdstVector stSpeedVector;
    int i;

    h_Current3dData = M_GetMSHandle(p_stSuperObject, 3dData);
    p_stCurrentState = fn_h3dDataGetCurrentState(h_Current3dData);
    p_stCurrentAnim = p_stCurrentState->p_stAnim;

    if (p_stCurrentAnim != NULL)
    {
        MTH3D_tdstVector *p_stVector;
        int iFramePassed;
        MTH_tdxReal xInvFramePassed;

        iFramePassed = 1;

        p_stVector = fn_p_stGetLinearSpeedVector(p_stCurrentAnim, fn_uw3dDataGetCurrentFrame(h_Current3dData));
        if (p_stVector)
        {
            MTH3D_M_vCopyVector(&stSpeedVector, p_stVector);
        }
        else
        {
            MTH3D_M_vFillVector(&stSpeedVector, 0);
        }

        for (i = sLastFrame + 1; i < fn_uw3dDataGetCurrentFrame(h_Current3dData); i++, iFramePassed++)
        {
            p_stVector = fn_p_stGetLinearSpeedVector(p_stCurrentAnim, (unsigned short)i);
            if (p_stVector)
            {
                MTH3D_M_vAddVector(&stSpeedVector, &stSpeedVector, p_stVector);
            }
        }
        xInvFramePassed = MTH_M_xInv(MTH_M_xLongToReal(iFramePassed));

        MTH3D_M_vMulScalarVector(&stSpeedVector, xInvFramePassed, &stSpeedVector);

        MTH3D_M_vMulScalarVector(&stSpeedVector, MTH_M_xLongToReal(fn_uc3dDataGetFrameRate(h_Current3dData)), &stSpeedVector);
        return &stSpeedVector;
    }
    else
        return NULL;
}

void __cdecl fn_v_QuatToMatrix(float (*)[3], float *);
MTH3D_tdstMatrix *__cdecl fn_p_stGetAngularSpeedMatrix(tdstAnim3d *_p_stAnim3d, unsigned short _uwFrame,
                                                       MTH3D_tdstMatrix *p_stMatrix)
{
    unsigned short uwNumOfA3dGENERAL;
    tdstA3dGENERAL *p_stCurrentA3dGENERAL;
    tdstOnlyFrame *p_stCurrentOnlyFrame;

    if (_uwFrame >= _p_stAnim3d->uwNumberOfFrames)
        return NULL;

    uwNumOfA3dGENERAL = _p_stAnim3d->uwNumOfA3dGENERAL;
    p_stCurrentA3dGENERAL = fn_p_GetA3dGENERAL(uwNumOfA3dGENERAL, _uwFrame);

    _uwFrame -= p_stCurrentA3dGENERAL->uwStartFrame;

    p_stCurrentOnlyFrame = &p_stOnlyFrames[p_stCurrentA3dGENERAL->uwNumOfFirstOnlyFrame + _uwFrame];

    if (p_stCurrentOnlyFrame->uwNumOfAngularSpeedQuat == SEB_Invalid)
    {
        return NULL;
    }
    else
    {
        float a4_xQuatOri[4];

        fn_v_ExpandQuat(a4_xQuatOri, &p_stAngularSpeedQuats[p_stCurrentOnlyFrame->uwNumOfAngularSpeedQuat * 4]);

        fn_v_QuatToMatrix((float (*)[3])(p_stMatrix), a4_xQuatOri);

        return p_stMatrix;
    }
}

/* 0x004100E0 */
MTH3D_tdstMatrix *__cdecl PLA_fn_stGetAngularSpeedMatrix(HIE_tdstSuperObject *p_stSuperObject, short sLastFrame)
{
    MS_tdxHandleTo3dData h_Current3dData;
    struct tdstState_ *p_stCurrentState;
    tdstAnim3d *p_stCurrentAnim;
    static MTH3D_tdstMatrix stSpeedMatrix;
    int i;
    ACP_tdxBool bSpeedMatrix = FALSE;

    h_Current3dData = M_GetMSHandle(p_stSuperObject, 3dData);
    p_stCurrentState = fn_h3dDataGetCurrentState(h_Current3dData);
    p_stCurrentAnim = p_stCurrentState->p_stAnim;

    if (p_stCurrentAnim != NULL)
    {
        MTH3D_tdstMatrix stMatrix;

        MTH3D_M_vSetIdentityMatrix(&stSpeedMatrix);
        for (i = sLastFrame + 1; i <= fn_uw3dDataGetCurrentFrame(h_Current3dData); i++)
        {
            if (fn_p_stGetAngularSpeedMatrix(p_stCurrentAnim, (unsigned short)i, &stMatrix))
            {
                MTH3D_M_vMulMatrixMatrix(&stSpeedMatrix, &stSpeedMatrix, &stMatrix);
                bSpeedMatrix = TRUE;
            }
        }
    }
    return bSpeedMatrix ? &stSpeedMatrix : NULL;
}

#define LME_INVALID 65535
static __inline int fn_b3dDataGetSkipCurrentFrame(MS_tdxHandleTo3dData h_3dData)
    { return h_3dData->bSkipCurrentFrame; }
static __inline void fn_v3dDataSetUserEventFlags(MS_tdxHandleTo3dData h_3dData, unsigned char ucValue)
    { h_3dData->ucUserEventFlags = ucValue; }

typedef struct { unsigned char _pad_ec[0xec]; long lLastComputeFrame; } tdstLastCompute3dView;
static __inline long fn_l3dDataGetLastComputeEngineFrame(MS_tdxHandleTo3dData h_3dData)
    { return ((tdstLastCompute3dView *)h_3dData)->lLastComputeFrame; }
static __inline void fn_v3dDataSetLastComputeEngineFrame(MS_tdxHandleTo3dData h_3dData, long lValue)
    { ((tdstLastCompute3dView *)h_3dData)->lLastComputeFrame = lValue; }

/* 0x0040E800 */
ACP_tdxBool __cdecl PLA_fn_bSkipCurrentFrame(HIE_tdstSuperObject *p_stSuperObject)
{
    MS_tdxHandleTo3dData h_Current3dData;
    struct tdstState_ *p_stCurrentState;
    tdstAnim3d *p_stCurrentAnim;
    unsigned long ulNbFrames;
    unsigned short uwNbFrames;
    unsigned short NbFramesSLME;

    h_Current3dData = M_GetMSHandle(p_stSuperObject, 3dData);
    p_stCurrentState = fn_h3dDataGetCurrentState(h_Current3dData);

    if (p_stCurrentState == NULL)
    {
        return TRUE;
    }
    p_stCurrentAnim = p_stCurrentState->p_stAnim;

    if (p_stCurrentAnim == NULL)
        return FALSE;

    if (!fn_uc3dDataGetFrameRate(h_Current3dData))
        return TRUE;

    fn_v3dDataSetUserEventFlags(h_Current3dData, 0);

    if (fn_l3dDataGetLastComputeEngineFrame(h_Current3dData) == 0)
    {

        fn_v3dDataSetStartTime(h_Current3dData, g_ulCurrentTimerCount - GAM_g_ulPauseTimeAccumulated + fn_ul3dDataGetStartTime(h_Current3dData));
        fn_v3dDataSetTimePreviousFrame(h_Current3dData, g_ulCurrentTimerCount);
    }

    fn_v3dDataSetLastComputeEngineFrame(h_Current3dData, HIE_gs_lCurrentFrame);

    NbFramesSLME = h_Current3dData->uwNbEngineFrameSinceLastMechEvent;
    if (NbFramesSLME != LME_INVALID)
    {
        NbFramesSLME++;
        fn_v3dDataSetNbEngineFrameSinceLastMechEvent(h_Current3dData, NbFramesSLME);
    }

    if (fn_b3dDataGetSkipCurrentFrame(h_Current3dData))
    {
        if (fn_p_stGetLinearSpeedVector(p_stCurrentAnim, fn_uw3dDataGetCurrentFrame(h_Current3dData)))
        {
            fn_vTransferSpeedInformationPLA_DNM(p_stSuperObject, fn_s3dDataGetLastFrame(h_Current3dData));
        }

        fn_v3dDataSetSkipCurrentFrame(h_Current3dData, FALSE);

        if (g_bAnimConstantSpeed)
            ulNbFrames = fn_uw3dDataGetCurrentFrame(h_Current3dData) + 1;
        else
            ulNbFrames = fn_uc3dDataGetFrameRate(h_Current3dData) * (g_ulCurrentTimerCount - fn_ul3dDataGetStartTime(h_Current3dData)) / 1000;

        if (ulNbFrames > p_stCurrentAnim->uwNumberOfFrames)
        {
            fn_v3dDataSetTimeDelay(h_Current3dData, g_ulCurrentTimerCount - 1000 * p_stCurrentAnim->uwNumberOfFrames / fn_uc3dDataGetFrameRate(h_Current3dData) - fn_ul3dDataGetStartTime(h_Current3dData));
            uwNbFrames = p_stCurrentAnim->uwNumberOfFrames;
        }
        else
        {
            fn_v3dDataSetTimeDelay(h_Current3dData, 0);
            uwNbFrames = (unsigned short)ulNbFrames;
        }

        if ((fn_uw3dDataGetCurrentFrame(h_Current3dData) == uwNbFrames) && (uwNbFrames != p_stCurrentAnim->uwNumberOfFrames))
            fn_v3dDataSetSkipCurrentFrame(h_Current3dData, TRUE);

        if (p_stCurrentAnim->uwNumberOfFrames == 1 &&
            !fn_b3dDataGetStateJustModified(h_Current3dData) &&
            fn_hGetNextStateInState(p_stCurrentState) == p_stCurrentState)
        {
            fn_v3dDataSetSkipCurrentFrame(h_Current3dData, TRUE);
            fn_v3dDataSetCurrentFrame(h_Current3dData, 0);
        }
        else
            fn_v3dDataSetCurrentFrame(h_Current3dData, uwNbFrames);

        if (fn_uw3dDataGetCurrentFrame(h_Current3dData) == p_stCurrentAnim->uwNumberOfFrames)
            fn_v3dDataSetFlagEndOfAnim(h_Current3dData, TRUE);

        if (fn_uc3dDataGetRepeatAnimation(h_Current3dData) == 0
            && fn_uw3dDataGetCurrentFrame(h_Current3dData) == p_stCurrentAnim->uwNumberOfFrames
            && fn_hGetNextStateInState(p_stCurrentState) == NULL)
        {
            fn_v3dDataSetFlagEndState(h_Current3dData, TRUE);
        }
        return TRUE;
    }

    else
        return FALSE;
}

/* 0x0041A400 */
void __cdecl fn_vUpdateFrame(HIE_tdstSuperObject *_p_stSuperObject, tdstFrame3d *_p_stFrame,
                             tdstAnim3d *_p_stAnim3d, unsigned short _uwFrame)
{
    tdstA3dGENERAL  *p_stCurrentA3dGENERAL;
    tdstOnlyFrame   *p_stCurrentOnlyFrame;
    tdstA3dChannel  *p_stCurrentChannel;
    tdstElement3d   *p_stCurrentElement3d;
    unsigned short   uwT_NTTO;
    unsigned short   uwCnt1;
    unsigned short   uwCorrectedFrame;

    unsigned short   uwNumOfA3dGENERAL;

    (void)_p_stSuperObject;
    uwNumOfA3dGENERAL = _p_stAnim3d->uwNumOfA3dGENERAL;

    if (_uwFrame >= _p_stAnim3d->uwNumberOfFrames)
        return;

    if (_p_stFrame->p_stAnim == _p_stAnim3d && _p_stFrame->uwFrameNumber == _uwFrame)
        return;

    _p_stFrame->p_stAnim = _p_stAnim3d;
    _p_stFrame->uwFrameNumber = _uwFrame;
    _p_stFrame->ucMatrixUpdated = FALSE;

    p_stCurrentA3dGENERAL = fn_p_GetA3dGENERAL(uwNumOfA3dGENERAL, _uwFrame);
    uwCorrectedFrame = (unsigned short)(_uwFrame - p_stCurrentA3dGENERAL->uwStartFrame);

    p_stCurrentChannel = &p_stChannels[p_stCurrentA3dGENERAL->uwNumOfFirstChannel];
    p_stCurrentOnlyFrame = &p_stOnlyFrames[p_stCurrentA3dGENERAL->uwNumOfFirstOnlyFrame + uwCorrectedFrame];
    uwT_NTTO = p_stCurrentOnlyFrame->uwNumOfSavedFrame;

    if (p_stCurrentOnlyFrame->uwNumOfAngularSpeedQuat == SEB_Invalid)
    {
        if (_p_stFrame->p_stAngularSpeedMatrix != NULL)
            TMP_fn_vFree(&_p_stFrame->p_stAngularSpeedMatrix);
        _p_stFrame->p_stAngularSpeedMatrix = NULL;
    }
    else
    {
        float a4_xQuat[4];
        if (_p_stFrame->p_stAngularSpeedMatrix == NULL)
            _p_stFrame->p_stAngularSpeedMatrix = TMP_M_p_Malloc(0x24);
        fn_v_ExpandQuat(a4_xQuat,
            &p_stAngularSpeedQuats[p_stCurrentOnlyFrame->uwNumOfAngularSpeedQuat * 4]);
        fn_v_QuatToMatrix((float (*)[3])_p_stFrame->p_stAngularSpeedMatrix, a4_xQuat);
    }

    if (p_stCurrentOnlyFrame->uwNumOfSpeedVector == SEB_Invalid)
        _p_stFrame->p_stLinearSpeedVector = NULL;
    else
        _p_stFrame->p_stLinearSpeedVector = &p_stSpeedVectors[p_stCurrentOnlyFrame->uwNumOfSpeedVector];

    if (p_stCurrentOnlyFrame->uwHierarchyNbCouples == 0)
        _p_stFrame->stHierarchy.d_stCouples = NULL;
    else
        _p_stFrame->stHierarchy.d_stCouples = &p_stHierarchies[p_stCurrentOnlyFrame->uwNumOfFirstHierarchyCouple];
    _p_stFrame->stHierarchy.ulNbOfCouples = p_stCurrentOnlyFrame->uwHierarchyNbCouples;

    p_stCurrentElement3d = _p_stFrame->p_stArrayOfElts3d;
    for (uwCnt1 = 0; uwCnt1 < p_stCurrentA3dGENERAL->uwNumberOfChannels; uwCnt1++)
    {
        tdstA3dFrame *p_stCurrentFrame = &p_stFrames[p_stCurrentChannel->uwNumOfFirstFrame + uwT_NTTO];

        if (p_stCurrentFrame->uwNumOfNTTO != (unsigned short)0xFFFF)
        {
            tdstNTTO *p_stTheNTTO = &p_stNTTO[p_stCurrentFrame->uwNumOfNTTO];
            p_stCurrentElement3d->wElement = p_stTheNTTO->ucNumberInTable;
            p_stCurrentElement3d->ucChannelNumber = (unsigned char)p_stCurrentChannel->uwChannelNumber;
            p_stCurrentElement3d->bf1ActiveStatus = TRUE;
            p_stCurrentElement3d->eTypeOfElement = (unsigned char)(p_stTheNTTO->uwTypeOfObject & 0x00FF);
            p_stCurrentElement3d->ucTransparency = p_stTheNTTO->ucTransparency;
        }
        else
        {

            p_stCurrentElement3d->wElement = 0x7FFF;
            p_stCurrentElement3d->ucChannelNumber = (unsigned char)p_stCurrentChannel->uwChannelNumber;
            p_stCurrentElement3d->bf1ActiveStatus = TRUE;
            p_stCurrentElement3d->eTypeOfElement = TE_EmptyObject;
            p_stCurrentElement3d->ucTransparency = 0;
        }

        p_stCurrentChannel++;
        p_stCurrentElement3d++;
    }
}

/* 0x0041A610 */
void __cdecl fn_vUpdateFrameMatrix(HIE_tdstSuperObject *_p_stSuperObject, tdstFrame3d *_p_stFrame,
                                   tdstAnim3d *_p_stAnim3d, unsigned short _uwFrame,
                                   unsigned char ucForceUpdate, tdstAChannel *d_stChannelArray)
{
    tdstA3dGENERAL *p_stCurrentA3dGENERAL;
    tdstA3dChannel *p_stCurrentChannel;
    unsigned short  uwCnt1;
    unsigned short  uwStartFrame;

    fn_vUpdateFrame(_p_stSuperObject, _p_stFrame, _p_stAnim3d, _uwFrame);

    if (_p_stFrame->ucMatrixUpdated && !ucForceUpdate)
        return;
    _p_stFrame->ucMatrixUpdated = TRUE;

    p_stCurrentA3dGENERAL = fn_p_GetA3dGENERAL(_p_stAnim3d->uwNumOfA3dGENERAL, _uwFrame);
    uwStartFrame = p_stCurrentA3dGENERAL->uwStartFrame;
    p_stCurrentChannel = &p_stChannels[p_stCurrentA3dGENERAL->uwNumOfFirstChannel];

    for (uwCnt1 = 0; uwCnt1 < p_stCurrentA3dGENERAL->uwNumberOfChannels; uwCnt1++)
    {
        if (p_stCurrentChannel->uwChannelNumber != SEB_Invalid &&
            !d_stChannelArray[p_stCurrentChannel->uwChannelNumber].bControlledChannel)
        {
            tdstKeyFrame *p_stOldKeyFrame;
            tdstKeyFrame *p_stCurrentKeyFrame;
            float         xDT;

            p_stOldKeyFrame = &p_stKeyFrames[p_stFramesKF[
                (unsigned short)(_uwFrame - uwStartFrame) + p_stCurrentChannel->ulNumOfFirstFrameKF]];

            if ((p_stOldKeyFrame->uwMask & SEB_xMaskLastKey) == 0)
            {
                p_stCurrentKeyFrame = p_stOldKeyFrame + 1;
                if (p_stCurrentKeyFrame->uwFrameNumber != p_stOldKeyFrame->uwFrameNumber)
                    xDT = (float)(int)(_uwFrame - p_stOldKeyFrame->uwFrameNumber)
                        / (float)(int)(p_stCurrentKeyFrame->uwFrameNumber - p_stOldKeyFrame->uwFrameNumber);
                else
                    xDT = MTH_C_ZERO;
            }
            else if ((p_stOldKeyFrame->uwMask & SEB_xTypeWrapLastKey) == 0)
            {
                xDT = MTH_C_ZERO;
                p_stCurrentKeyFrame = p_stOldKeyFrame;
            }
            else
            {
                p_stCurrentKeyFrame = &p_stKeyFrames[p_stFramesKF[p_stCurrentChannel->ulNumOfFirstFrameKF]];
                xDT = (float)(int)(_uwFrame - p_stOldKeyFrame->uwFrameNumber)
                    / (float)(int)(p_stCurrentA3dGENERAL->uwNumberOfFrames
                                   + p_stCurrentKeyFrame->uwFrameNumber - p_stOldKeyFrame->uwFrameNumber);
            }

            if (p_stOldKeyFrame->swInterpolationParameter != 0 && xDT != MTH_C_ZERO)
            {
                float fTmp = (float)(int)p_stCurrentKeyFrame->swInterpolationParameter * A3X_xInterpParamScale;
                xDT = ((fTmp * xDT - A3X_xInterpParamOne) - fTmp) * xDT;
            }

            fn_v_InterpolBetweenKeys(p_stOldKeyFrame, p_stCurrentKeyFrame, p_stCurrentChannel,
                d_stChannelArray[p_stCurrentChannel->uwChannelNumber].hSupObject->hLocalMatrix, 0, xDT);
        }

        p_stCurrentChannel++;
    }
}

long PLA_fn_bTestIfEndOfAction(HIE_tdstSuperObject *hSuperObj)
{
	MS_tdxHandleTo3dData h_Current3dData = M_GetMSHandle(hSuperObj,3dData);

	if(    fn_uc3dDataGetFlagEndState(h_Current3dData)
		&& fn_h3dDataGetCurrentState(h_Current3dData)==fn_h3dDataGetWantedState(h_Current3dData) )
	{
		return(TRUE);
	}
	return(FALSE);
}

#define PLA_C_wRandomFrame  (-32768)

void __cdecl PLA_fn_vSetCurrFrame(HIE_tdstSuperObject *p_stSuperObject, short _wFrame)
{
    MS_tdxHandleTo3dData  h_Current3dData = M_GetMSHandle(p_stSuperObject, 3dData);
    unsigned char         ucFrameRate = fn_uc3dDataGetFrameRate(h_Current3dData), ucFrameRate2 = ucFrameRate;

    unsigned short        uwMaxFrame = (unsigned short)(fn_h3dDataGetCurrentState(h_Current3dData)->p_stAnim->uwNumberOfFrames);

    if (_wFrame == PLA_C_wRandomFrame)
    {
        _wFrame = (short)RND_fn_lGetLongRandomValue((long)g_Randomhandle, 0, uwMaxFrame - 1);
    }

    else
    {
        if (_wFrame < 0)
        {
            _wFrame = 0;
        }
        else if (_wFrame >= uwMaxFrame)
        {
            _wFrame = (short)(uwMaxFrame - 1);
        }
    }

    fn_v3dDataSetForcedFrame(h_Current3dData, _wFrame);
    fn_v3dDataSetLastFrame(h_Current3dData, (short)(_wFrame - 1));

    fn_v3dDataSetFlagModifState(h_Current3dData, TRUE);
    fn_v3dDataSetWantedState(h_Current3dData, fn_h3dDataGetCurrentState(h_Current3dData));

    fn_v3dDataSetFlagEndOfAnim(h_Current3dData, FALSE);
    fn_v3dDataSetFlagEndState(h_Current3dData, FALSE);

    if (!ucFrameRate)
    {

        ucFrameRate2 = fn_scGetSpeedInState(fn_h3dDataGetCurrentState(h_Current3dData));
        fn_v3dDataSetFrameRate(h_Current3dData, ucFrameRate2);
    }

    fn_v3dDataSetStartTime(h_Current3dData,
        g_ulCurrentTimerCount - (1000 * _wFrame) / ucFrameRate2);
    fn_v3dDataSetTimePreviousFrame(h_Current3dData,
        g_ulCurrentTimerCount - 1000 / ucFrameRate2);

    PLA_fn_bDoAnimPlayerForCharacter(p_stSuperObject, FALSE, FALSE);

    fn_v3dDataSetFrameRate(h_Current3dData, ucFrameRate);
}

#include "GAM/Family.h"
#define PLA_M_pFamilyStateFirstOf3dData(_h3d) \
    (((tdxHandleToFamilyList)((MS_tdxHandleTo3dData)(_h3d))->h_Family)->hForStateArray_first)

void __cdecl fn_vUpdateCurrentActivationForCharacter(HIE_tdstSuperObject *_hCharacter);

void fn_vInitAllEvents(HIE_tdstSuperObject *p_stSuperObject, unsigned short uwFirstFrame);

BOOL __cdecl PLA_fn_vInitNewState(HIE_tdstSuperObject *p_stSuperObject, tdxHandleToState h_State)
{
    MS_tdxHandleTo3dData h_Current3dData = M_GetMSHandle(p_stSuperObject, 3dData);

    if (PLA_M_pFamilyStateFirstOf3dData(h_Current3dData) == NULL)
        return FALSE;

    fn_v3dDataSetFlagEndState(h_Current3dData, FALSE);
    fn_v3dDataSetFlagModifState(h_Current3dData, TRUE);

    fn_v3dDataSetCurrentFrame(h_Current3dData, 0);
    fn_v3dDataSetLastFrame(h_Current3dData, 0);

    fn_v3dDataSetWantedState(h_Current3dData, h_State);

    if (h_State->p_stAnim)
    {
        fn_vAllocArrayOfElts3d(h_Current3dData, h_State->p_stAnim->ucMaxNumberOfElements);
    }
    else
    {
        fn_vFreeArrayOfElts3d(h_Current3dData);
    }

    fn_v3dDataSetCurrentState(h_Current3dData, h_State);
    fn_vUpdateCurrentActivationForCharacter(p_stSuperObject);
    fn_v3dDataSetFirstStateOfAction(h_Current3dData, h_State);
    fn_v3dDataSetFrameRate(h_Current3dData, fn_scGetSpeedInState(h_State));
    if (h_State->p_stAnim)
    {
        fn_vInitAllEvents(p_stSuperObject, 0);
    }
    return TRUE;
}
