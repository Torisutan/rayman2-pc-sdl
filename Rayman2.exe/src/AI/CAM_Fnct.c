#include <stddef.h>

#include "MTH/MTH_types.h"
#include "HIE/HIE_types.h"
#include "POS/POS_types.h"
#include "AI/AI_types.h"
#include "AI/AI_globals.h"
#include "AI/CAM_Base.h"
#include "GAM/GAM_types.h"
#include "GAM/cineinfo.h"
#include "GAM/TYPECAM.H"
#include "GAM/Actions/SectInfo.h"

struct tdstCineinfo_;

extern void __cdecl CAM_fn_vSetCineinfoWorkFromCurrent(struct tdstCineinfo_ *hCineinfo);

#define AI_CAM_M_pfn_fn_ucGetFunctionUltraOperator ((unsigned char (__cdecl *)(unsigned long))&fn_ucGetFunctionUltraOperator)
/* 0x00480FB0 */
extern void __cdecl fn_vOnAIErm_M_UpdateLastError(unsigned short FatalErrorNum, char *szMsg);
#define AI_M_pfn_M_AIFatalError_fn1(uwError) \
    fn_vOnAIErm_M_UpdateLastError((unsigned short)(uwError), (char *)0)

#define AI_CAM_C_VolIAFlags_CurrentAlreadyCopiedInWork  0x01
#define AI_CAM_C_PerIAFlags_VisibilityCopied            0x02

/* 0x0040B830 */
extern HIE_tdstSuperObject *__cdecl CAM_fn_hGetActiveCamera(long _lViewportNumber);
#define AI_CAM_M_pfn_hGetActiveCamera        ((void *(__cdecl *)(long))&CAM_fn_hGetActiveCamera)
extern void *__cdecl fn_h_SectInfoGetCurrentSector(void *h_SectInfo);   /* 0x00419B90 */
#define AI_CAM_M_pfn_hSectInfoGetCurrentSector fn_h_SectInfoGetCurrentSector
/* 0x0044C2D0 */
extern char __cdecl fn_cGetSectorCameraType(void *_hSprObjSector);
#define AI_CAM_M_pfn_cGetSectorCameraType    ((char  (__cdecl *)(void *))&fn_cGetSectorCameraType)
/* 0x0042DE30 */
extern void *__cdecl fn_p_stDynamGetParsingDatas(void *h_Dynam);
#define AI_CAM_M_pfn_p_stDynamGetParsingDatas fn_p_stDynamGetParsingDatas
#define AI_CAM_C_e_MainViewport  1

#define AI_CAM_M_pfn_vSetCineinfoWorkFromCurrent ((void (__cdecl *)(void *))CAM_fn_vSetCineinfoWorkFromCurrent)
/* 0x00474270 */
extern void __cdecl CAM_fn_vInitCameraStructure(void *_hSuperObjPerso, void *_p_stStruct);
#define AI_CAM_M_pfn_vInitCameraStructure        ((void (__cdecl *)(void *, void *))CAM_fn_vInitCameraStructure)
/* 0x00483F90 */
struct CAM_tdstUpdateCamera_;
void CAM_fn_vComputeReferencePoint(struct CAM_tdstUpdateCamera_ *_p_stStruct);
void CAM_fn_vComputeTarget(struct CAM_tdstUpdateCamera_ *_p_stStruct);
#define AI_CAM_M_pfn_vComputeReferencePoint(p_stStruct) CAM_fn_vComputeReferencePoint((struct CAM_tdstUpdateCamera_ *)(p_stStruct))
#define AI_CAM_M_pfn_vComputeTarget(p_stStruct) CAM_fn_vComputeTarget((struct CAM_tdstUpdateCamera_ *)(p_stStruct))
/* 0x00484540 */
void CAM_fn_vComputeRealWantedPos(struct CAM_tdstUpdateCamera_ *_p_stStruct);
#define AI_CAM_M_pfn_vBestPosUpdate1(p_stStruct) \
    CAM_fn_vComputeRealWantedPos((struct CAM_tdstUpdateCamera_ *)(p_stStruct))
void CAM_fn_vComputeMovePosWithDynTheta(struct CAM_tdstUpdateCamera_ *_p_stStruct);  /* 0x00484B00 */
#define AI_CAM_M_pfn_vBestPosUpdate2             ((void (__cdecl *)(void *))CAM_fn_vComputeMovePosWithDynTheta)
void CAM_fn_vForceFailureVisibility(struct CAM_tdstUpdateCamera_ *_p_stStruct);

#define AI_CAM_M_pfn_vBestPosUpdate3             ((void (__cdecl *)(void *))CAM_fn_vForceFailureVisibility)

#define AI_CAM_UC_hCineinfo        0x08
#define AI_CAM_UC_hCineinfoWork    0x0c
#define AI_CAM_UC_hCineinfoCurrent 0x10
#define AI_CAM_UC_stTarget         0xa0
#define AI_CAM_UC_SIZE             0xd0

#define AI_CAM_CI_stLastFailedWanted 0x1c
#define AI_CAM_CI_eState             0x60
#define AI_CAM_CI_bCanDoBestPos      0x17c
#define AI_CAM_ISC_uwIAFlags         0x72
#define AI_CAM_ISC_hSuperObjectTargeted 0x68
#define AI_CAM_C_IAFlags_NoDynamicTarget         0x0001
#define AI_CAM_C_IAFlags_TargetIsAlreadyComputed 0x1000

#define AI_CAM_c_CanTestStatic            (*(char *)AI_CAM_gp_cCanTestStatic)

extern char cRefAxisIsAlreadyComputed;   /* 0x004E92FD */

/* 0x00488500 */
MTH_tdxReal __cdecl CAM_fn_xGetARealParameter
(
    struct tdstCineinfo_       *_hCineInfo,
    MTH_tdxReal                *_p_xInitialValue,
    MTH_tdxReal                *_p_xCurrentValue,
    MTH_tdxReal                *_p_xWorkValue,
    MTH_tdxReal                *_p_xVisValue,
    long                        _lCurrentValue
)
{
    MTH_tdxReal         xValue = MTH_C_ZERO;

    if (_lCurrentValue == 1)
    {
        if (_hCineInfo->ucVolIAFlags & C_VolIAFlags_CurrentAlreadyCopiedInWork)
            xValue = *_p_xWorkValue;
        else
            xValue = *_p_xCurrentValue;
    }
    else if (_lCurrentValue == -1)
    {
        xValue = *_p_xInitialValue;
    }
    else
    {
        if (_hCineInfo->ucPerIAFlags & C_PerIAFlags_VisibilityCopied)
            xValue = *_p_xVisValue;
        else
            xValue = *_p_xCurrentValue;
    }

    return xValue;
}

MTH3D_tdstVector *__cdecl CAM_fn_p_stGetAVectorParameter
(
    struct tdstCineinfo_       *_hCineInfo,
    MTH3D_tdstVector           *_p_stInitialValue,
    MTH3D_tdstVector           *_p_stCurrentValue,
    MTH3D_tdstVector           *_p_stWorkValue,
    MTH3D_tdstVector           *_p_stVisValue,
    long                        _lCurrentValue
)
{
    MTH3D_tdstVector *p_stValue = NULL;

    if (_lCurrentValue == 1)
    {
        if (_hCineInfo->ucVolIAFlags & C_VolIAFlags_CurrentAlreadyCopiedInWork)
            p_stValue = _p_stWorkValue;
        else
            p_stValue = _p_stCurrentValue;
    }
    else if (_lCurrentValue == -1)
    {
        p_stValue = _p_stInitialValue;
    }
    else
    {
        if (_hCineInfo->ucPerIAFlags & C_PerIAFlags_VisibilityCopied)
            p_stValue = _p_stVisValue;
        else
            p_stValue = _p_stCurrentValue;
    }

    return p_stValue;
}

/* 0x00488590 */
AI_tdstNodeInterpret *__cdecl CAM_fn_p_stGetVectorParameter(
    HIE_tdstSuperObject *p_SuperObjPerso, AI_tdstNodeInterpret *p_stTree, AI_tdstGetSetParam *p_stValue)
{
    HIE_tdstSuperObject        *hCamera;
    MS_tdxHandleToCineinfo      hCineInfo;
    long                        lCurrentValue;
    MTH3D_tdstVector           *p_stInitialValue = NULL;
    MTH3D_tdstVector           *p_stCurrentValue = NULL;
    MTH3D_tdstVector           *p_stWorkValue = NULL;
    MTH3D_tdstVector           *p_stVisValue = NULL;
    long                        eFuncId = AI_M_eFuncIdInterpret(p_stTree-1);

    fn_vGetUltraOperatorPerso(AI_CAM_M_pfn_fn_ucGetFunctionUltraOperator(eFuncId),p_SuperObjPerso,&hCamera);

    hCineInfo = M_GetMSHandle(hCamera, Cineinfo);

    M_EvalNextParameter(p_stValue);
    lCurrentValue = M_GetSetParam_lValue(p_stValue);

    switch(eFuncId)
    {
        case 0x11c:
            p_stInitialValue = &(hCineInfo->hInit->stShiftTarget);
            p_stCurrentValue = &(hCineInfo->hCurrent->stShiftTarget);
            p_stWorkValue = &(hCineInfo->hWork->stShiftTarget);
            p_stVisValue = &(hCineInfo->hVisibility->stShiftTarget);
            break;
        case 0x11d:
            p_stInitialValue = &(hCineInfo->hInit->stShiftPos);
            p_stCurrentValue = &(hCineInfo->hCurrent->stShiftPos);
            p_stWorkValue = &(hCineInfo->hWork->stShiftPos);
            p_stVisValue = &(hCineInfo->hVisibility->stShiftPos);
            break;
        default:
            break;
    }

    M_Full_GetSetParam_p_stVertex
    (
        p_stValue,
        CAM_fn_p_stGetAVectorParameter
        (
            hCineInfo,
            p_stInitialValue,
            p_stCurrentValue,
            p_stWorkValue,
            p_stVisValue,
            lCurrentValue
        )
    );

    return(p_stTree);
}

/* 0x004886A0 */
AI_tdstNodeInterpret *__cdecl CAM_fn_p_stGetRealParameter(
    HIE_tdstSuperObject *p_SuperObjPerso, AI_tdstNodeInterpret *p_stTree, AI_tdstGetSetParam *p_stValue)
{
    unsigned long        eFuncId;
    HIE_tdstSuperObject *hCamera;
    MS_tdxHandleToCineinfo hCineInfo;
    AI_tdstNodeInterpret *p_stNext;
    long                 lCurrentValue;
    MTH_tdxReal         *p_xInitialValue = NULL;
    MTH_tdxReal         *p_xCurrentValue = NULL;
    MTH_tdxReal         *p_xWorkValue = NULL;
    MTH_tdxReal         *p_xVisValue = NULL;

    eFuncId = AI_M_eFuncIdInterpret(p_stTree-1);
    fn_vGetUltraOperatorPerso(
        AI_CAM_M_pfn_fn_ucGetFunctionUltraOperator(eFuncId), p_SuperObjPerso, &hCamera);

    hCineInfo = M_GetMSHandle(hCamera, Cineinfo);

    p_stNext = fn_p_stEvalTree(p_SuperObjPerso, p_stTree, p_stValue);
    lCurrentValue = M_GetSetParam_lValue(p_stValue);

    switch (eFuncId)
    {
    case 0x11e:
        p_xInitialValue = &(hCineInfo->hInit->xDistMin);
        p_xCurrentValue = &(hCineInfo->hCurrent->xDistMin);
        p_xWorkValue = &(hCineInfo->hWork->xDistMin);
        p_xVisValue = &(hCineInfo->hVisibility->xDistMin);
        break;
    case 0x11f:
        p_xInitialValue = &(hCineInfo->hInit->xDistMax);
        p_xCurrentValue = &(hCineInfo->hCurrent->xDistMax);
        p_xWorkValue = &(hCineInfo->hWork->xDistMax);
        p_xVisValue = &(hCineInfo->hVisibility->xDistMax);
        break;
    case 0x120:
        p_xInitialValue = &(hCineInfo->hInit->xBoundDistMin);
        p_xCurrentValue = &(hCineInfo->hCurrent->xBoundDistMin);
        p_xWorkValue = &(hCineInfo->hWork->xBoundDistMin);
        p_xVisValue = &(hCineInfo->hVisibility->xBoundDistMin);
        break;
    case 0x121:
        p_xInitialValue = &(hCineInfo->hInit->xBoundDistMax);
        p_xCurrentValue = &(hCineInfo->hCurrent->xBoundDistMax);
        p_xWorkValue = &(hCineInfo->hWork->xBoundDistMax);
        p_xVisValue = &(hCineInfo->hVisibility->xBoundDistMax);
        break;
    case 0x122:
        p_xInitialValue = &(hCineInfo->hInit->xAngleAlpha);
        p_xCurrentValue = &(hCineInfo->hCurrent->xAngleAlpha);
        p_xWorkValue = &(hCineInfo->hWork->xAngleAlpha);
        p_xVisValue = &(hCineInfo->hVisibility->xAngleAlpha);
        break;
    case 0x123:
        p_xInitialValue = &(hCineInfo->hInit->xAngleShiftAlpha);
        p_xCurrentValue = &(hCineInfo->hCurrent->xAngleShiftAlpha);
        p_xWorkValue = &(hCineInfo->hWork->xAngleShiftAlpha);
        p_xVisValue = &(hCineInfo->hVisibility->xAngleShiftAlpha);
        break;
    case 0x124:
        p_xInitialValue = &(hCineInfo->hInit->xAngleTheta);
        p_xCurrentValue = &(hCineInfo->hCurrent->xAngleTheta);
        p_xWorkValue = &(hCineInfo->hWork->xAngleTheta);
        p_xVisValue = &(hCineInfo->hVisibility->xAngleTheta);
        break;
    case 0x125:
        p_xInitialValue = &(hCineInfo->hInit->xAngleShiftTheta);
        p_xCurrentValue = &(hCineInfo->hCurrent->xAngleShiftTheta);
        p_xWorkValue = &(hCineInfo->hWork->xAngleShiftTheta);
        p_xVisValue = &(hCineInfo->hVisibility->xAngleShiftTheta);
        break;
    case 0x126:
        p_xInitialValue = &(hCineInfo->hInit->xLinearSpeed);
        p_xCurrentValue = &(hCineInfo->hCurrent->xLinearSpeed);
        p_xWorkValue = &(hCineInfo->hWork->xLinearSpeed);
        p_xVisValue = &(hCineInfo->hVisibility->xLinearSpeed);
        break;
    case 0x127:
        p_xInitialValue = &(hCineInfo->hInit->xLinearIncreaseSpeed);
        p_xCurrentValue = &(hCineInfo->hCurrent->xLinearIncreaseSpeed);
        p_xWorkValue = &(hCineInfo->hWork->xLinearIncreaseSpeed);
        p_xVisValue = &(hCineInfo->hVisibility->xLinearIncreaseSpeed);
        break;
    case 0x128:
        p_xInitialValue = &(hCineInfo->hInit->xLinearDecreaseSpeed);
        p_xCurrentValue = &(hCineInfo->hCurrent->xLinearDecreaseSpeed);
        p_xWorkValue = &(hCineInfo->hWork->xLinearDecreaseSpeed);
        p_xVisValue = &(hCineInfo->hVisibility->xLinearDecreaseSpeed);
        break;
    case 0x129:
        p_xInitialValue = &(hCineInfo->hInit->xAngularSpeed);
        p_xCurrentValue = &(hCineInfo->hCurrent->xAngularSpeed);
        p_xWorkValue = &(hCineInfo->hWork->xAngularSpeed);
        p_xVisValue = &(hCineInfo->hVisibility->xAngularSpeed);
        break;
    case 0x12a:
        p_xInitialValue = &(hCineInfo->hInit->xAngularIncreaseSpeed);
        p_xCurrentValue = &(hCineInfo->hCurrent->xAngularIncreaseSpeed);
        p_xWorkValue = &(hCineInfo->hWork->xAngularIncreaseSpeed);
        p_xVisValue = &(hCineInfo->hVisibility->xAngularIncreaseSpeed);
        break;
    case 0x12b:
        p_xInitialValue = &(hCineInfo->hInit->xAngularDecreaseSpeed);
        p_xCurrentValue = &(hCineInfo->hCurrent->xAngularDecreaseSpeed);
        p_xWorkValue = &(hCineInfo->hWork->xAngularDecreaseSpeed);
        p_xVisValue = &(hCineInfo->hVisibility->xAngularDecreaseSpeed);
        break;
    case 0x12c:
        p_xInitialValue = &(hCineInfo->hInit->xTargetSpeed);
        p_xCurrentValue = &(hCineInfo->hCurrent->xTargetSpeed);
        p_xWorkValue = &(hCineInfo->hWork->xTargetSpeed);
        p_xVisValue = &(hCineInfo->hVisibility->xTargetSpeed);
        break;
    case 0x12d:
        p_xInitialValue = &(hCineInfo->hInit->xTargetIncreaseSpeed);
        p_xCurrentValue = &(hCineInfo->hCurrent->xTargetIncreaseSpeed);
        p_xWorkValue = &(hCineInfo->hWork->xTargetIncreaseSpeed);
        p_xVisValue = &(hCineInfo->hVisibility->xTargetIncreaseSpeed);
        break;
    case 0x12e:
        p_xInitialValue = &(hCineInfo->hInit->xTargetDecreaseSpeed);
        p_xCurrentValue = &(hCineInfo->hCurrent->xTargetDecreaseSpeed);
        p_xWorkValue = &(hCineInfo->hWork->xTargetDecreaseSpeed);
        p_xVisValue = &(hCineInfo->hVisibility->xTargetDecreaseSpeed);
        break;
    case 0x12f:
        p_xInitialValue = &(hCineInfo->hInit->xFocal);
        p_xCurrentValue = &(hCineInfo->hCurrent->xFocal);
        p_xWorkValue = &(hCineInfo->hWork->xFocal);
        p_xVisValue = &(hCineInfo->hVisibility->xFocal);
        break;
    case 0x130:
        p_xInitialValue = &(hCineInfo->hInit->xZMin);
        p_xCurrentValue = &(hCineInfo->hCurrent->xZMin);
        p_xWorkValue = &(hCineInfo->hWork->xZMin);
        p_xVisValue = &(hCineInfo->hVisibility->xZMin);
        break;
    case 0x131:
        p_xInitialValue = &(hCineInfo->hInit->xZMax);
        p_xCurrentValue = &(hCineInfo->hCurrent->xZMax);
        p_xWorkValue = &(hCineInfo->hWork->xZMax);
        p_xVisValue = &(hCineInfo->hVisibility->xZMax);
        break;
    default:
        break;
    }

    p_stValue->uParam.xValue = CAM_fn_xGetARealParameter(hCineInfo, p_xInitialValue,
                                                         p_xCurrentValue, p_xWorkValue, p_xVisValue, lCurrentValue);
    p_stValue->eType = 8;
    return p_stNext;
}

/* 0x004889C0 */
AI_tdstNodeInterpret *__cdecl CAM_fn_p_stGetTargetedSuperObject(
    HIE_tdstSuperObject *p_SuperObjPerso, AI_tdstNodeInterpret *p_stTree, AI_tdstGetSetParam *p_stValue)
{
    unsigned long        eFuncId = AI_M_eFuncIdInterpret(p_stTree-1);
    HIE_tdstSuperObject *hCamera;
    MS_tdxHandleToCineinfo hCineInfo;
    HIE_tdstSuperObject *hSuperObjectTargeted;
    AI_tdstNodeInterpret *p_stNext;
    long                 lCurrentValue;

    fn_vGetUltraOperatorPerso(
        AI_CAM_M_pfn_fn_ucGetFunctionUltraOperator(eFuncId), p_SuperObjPerso, &hCamera);
    hCineInfo = M_GetMSHandle(hCamera, Cineinfo);

    p_stNext = fn_p_stEvalTree(p_SuperObjPerso, p_stTree, p_stValue);
    lCurrentValue = M_GetSetParam_lValue(p_stValue);

    if (lCurrentValue == 1)
    {
        if (hCineInfo->ucVolIAFlags & C_VolIAFlags_CurrentAlreadyCopiedInWork)
            hSuperObjectTargeted = hCineInfo->hWork->hSuperObjectTargeted;
        else
            hSuperObjectTargeted = hCineInfo->hCurrent->hSuperObjectTargeted;
    }
    else if (lCurrentValue == -1)
    {
        hSuperObjectTargeted = hCineInfo->hInit->hSuperObjectTargeted;
    }
    else
    {
        if (hCineInfo->ucPerIAFlags & C_PerIAFlags_VisibilityCopied)
            hSuperObjectTargeted = hCineInfo->hVisibility->hSuperObjectTargeted;
        else
            hSuperObjectTargeted = hCineInfo->hCurrent->hSuperObjectTargeted;
    }

    p_stValue->uParam.p_stEngObjValue = (hSuperObjectTargeted == NULL) ? NULL :
        ((hSuperObjectTargeted->ulTypeOfLinkedObject != AI_C_HIE_C_ulActor) ? NULL :
         hSuperObjectTargeted->hLinkedObject.p_Void);
    p_stValue->eType = 0x0b;
    return p_stNext;
}

/* 0x00488A80 */
AI_tdstNodeInterpret *__cdecl CAM_fn_p_stGetTypeOfViewport(
    HIE_tdstSuperObject *p_SuperObjPerso, AI_tdstNodeInterpret *p_stTree, AI_tdstGetSetParam *p_stValue)
{
    unsigned long        eFuncId = AI_M_eFuncIdInterpret(p_stTree-1);
    HIE_tdstSuperObject *hCamera;
    MS_tdxHandleToCineinfo hCineInfo;
    AI_tdstNodeInterpret *p_stNext;
    long                 lTypeOfViewport;
    long                 lCurrentValue;

    fn_vGetUltraOperatorPerso(
        AI_CAM_M_pfn_fn_ucGetFunctionUltraOperator(eFuncId), p_SuperObjPerso, &hCamera);
    hCineInfo = M_GetMSHandle(hCamera, Cineinfo);

    p_stNext = fn_p_stEvalTree(p_SuperObjPerso, p_stTree, p_stValue);
    lCurrentValue = M_GetSetParam_lValue(p_stValue);

    if (lCurrentValue == 1)
    {
        if (hCineInfo->ucVolIAFlags & C_VolIAFlags_CurrentAlreadyCopiedInWork)
            lTypeOfViewport = (long)(hCineInfo->hWork->eTypeOfViewport);
        else
            lTypeOfViewport = (long)(hCineInfo->hCurrent->eTypeOfViewport);
    }
    else if (lCurrentValue == -1)
    {
        lTypeOfViewport = (long)(hCineInfo->hInit->eTypeOfViewport);
    }
    else
    {
        if (hCineInfo->ucPerIAFlags & C_PerIAFlags_VisibilityCopied)
            lTypeOfViewport = (long)(hCineInfo->hVisibility->eTypeOfViewport);
        else
            lTypeOfViewport = (long)(hCineInfo->hCurrent->eTypeOfViewport);
    }

    M_Full_GetSetParam_Integer(p_stValue, lTypeOfViewport);
    return p_stNext;
}
extern char cNoDynChangeTheta;           /* 0x004E92FC */

/* 0x00488B50 */
AI_tdstNodeInterpret *__cdecl CAM_fn_p_stGetSectorCameraType(
    HIE_tdstSuperObject *p_SuperObjPerso, AI_tdstNodeInterpret *p_stTree, AI_tdstGetSetParam *p_stValue)
{
    unsigned long        eFuncId = AI_M_eFuncIdInterpret(p_stTree-1);
    HIE_tdstSuperObject *hCamera;
    MS_tdxHandleToSectInfo hCamCineinfo;
    HIE_tdstSuperObject *hCurrentSectorCamera;
    long                 lType;

    fn_vGetUltraOperatorPerso(
        AI_CAM_M_pfn_fn_ucGetFunctionUltraOperator(eFuncId), p_SuperObjPerso, &hCamera);

    hCamCineinfo = M_GetMSHandle(hCamera, SectInfo);
    hCurrentSectorCamera = (HIE_tdstSuperObject *)fn_h_SectInfoGetCurrentSector(hCamCineinfo);

    lType = (long)fn_cGetSectorCameraType((void *)hCurrentSectorCamera);
    p_stValue->uParam.lValue = lType;
    p_stValue->eType = 6;
    return p_stTree;
}

/* 0x00488BB0 */
AI_tdstNodeInterpret *__cdecl CAM_fn_p_stGetCameraOfViewport(
    HIE_tdstSuperObject *p_SuperObjPerso, AI_tdstNodeInterpret *p_stTree, AI_tdstGetSetParam *p_stValue)
{
    unsigned long        eFuncId = AI_M_eFuncIdInterpret(p_stTree-1);
    AI_tdstNodeInterpret *p_stNext = p_stTree;

#define CAM_M_vFullGetSetParamPerso(p_stValue, hPerso)                                         \
    (p_stValue)->uParam.p_stEngObjValue = ((hPerso) == NULL) ? NULL                             \
        : ((((HIE_tdstSuperObject *)(hPerso))->ulTypeOfLinkedObject != AI_C_HIE_C_ulActor)      \
            ? NULL : (struct tdstEngineObject_ *)(((HIE_tdstSuperObject *)(hPerso))->hLinkedObject.p_Void));  \
    (p_stValue)->eType = 0x0b
    switch (eFuncId)
    {
    case 0x134:
        p_stNext = fn_p_stEvalTree(p_SuperObjPerso, p_stTree, p_stValue);
        CAM_M_vFullGetSetParamPerso(p_stValue, CAM_fn_hGetActiveCamera(p_stValue->uParam.lValue));
        return p_stNext;
    case 0x135:
        CAM_M_vFullGetSetParamPerso(p_stValue, CAM_fn_hGetActiveCamera(AI_CAM_C_e_MainViewport));
        return p_stTree;
    default:
        break;
    }
#undef CAM_M_vFullGetSetParamPerso
    AI_M_pfn_M_AIFatalError_fn1(0x27);
    return p_stTree;
}

/* 0x00488CB0 */
AI_tdstNodeInterpret *__cdecl CAM_fn_p_stComputeTargetWithTgtPerso(
    HIE_tdstSuperObject *p_SuperObjPerso, AI_tdstNodeInterpret *p_stTree, AI_tdstGetSetParam *p_stValue)
{
    unsigned long        eFuncId = AI_M_eFuncIdInterpret(p_stTree-1);
    HIE_tdstSuperObject *hCamera;
    MS_tdxHandleToCineinfo hCineInfo;
    CAM_tdstUpdateCamera stStruct;
    unsigned short       uwMemoFlags;

    fn_vGetUltraOperatorPerso(
        AI_CAM_M_pfn_fn_ucGetFunctionUltraOperator(eFuncId), p_SuperObjPerso, &hCamera);
    hCineInfo = (MS_tdxHandleToCineinfo)M_GetMSHandle(hCamera, Cineinfo);

    CAM_fn_vSetCineinfoWorkFromCurrent(hCineInfo);
    CAM_fn_vInitCameraStructure(hCamera, &stStruct);

    if (stStruct.hCineinfoCurrent->hSuperObjectTargeted == NULL)
    {

        M_Full_GetSetParam_p_stVertex(p_stValue, NULL);
        return p_stTree;
    }

    uwMemoFlags = stStruct.hCineinfoWork->uwIAFlags;
    stStruct.hCineinfoWork->uwIAFlags |= AI_CAM_C_IAFlags_NoDynamicTarget;
    stStruct.hCineinfoWork->uwIAFlags &= ~AI_CAM_C_IAFlags_TargetIsAlreadyComputed;

    CAM_fn_vComputeReferencePoint(&stStruct);
    CAM_fn_vComputeTarget(&stStruct);

    stStruct.hCineinfoWork->uwIAFlags = uwMemoFlags;

    MTH3D_M_vCopyVector((MTH3D_tdstVector *)p_stValue->uParam.a_xVectorValue, &stStruct.stCptPos.stTarget);
    p_stValue->eType = 0x15;
    return p_stTree;
}
#define AI_CAM_c_RefAxisIsAlreadyComputed cRefAxisIsAlreadyComputed
#define AI_CAM_c_NoDynChangeTheta         cNoDynChangeTheta

#define DNM_M_p_stCPDGetTarget(_p_stParsingDatas)           (&(_p_stParsingDatas)->stVector)

/* 0x00488DC0 */
AI_tdstNodeInterpret *__cdecl CAM_fn_p_stComputeCurrentTarget(
    HIE_tdstSuperObject *p_SuperObjPerso, AI_tdstNodeInterpret *p_stTree, AI_tdstGetSetParam *p_stValue)
{
    unsigned long        eFuncId = AI_M_eFuncIdInterpret(p_stTree-1);
    HIE_tdstSuperObject *hCamera;
    MS_tdxHandleToDynam  h_Dynam;
    DNM_tdstParsingDatas *p_stParsingDatas;

    fn_vGetUltraOperatorPerso(
        AI_CAM_M_pfn_fn_ucGetFunctionUltraOperator(eFuncId), p_SuperObjPerso, &hCamera);

    h_Dynam = M_GetMSHandle(hCamera, Dynam);
    if (h_Dynam != NULL)
    {
        p_stParsingDatas = AI_CAM_M_pfn_p_stDynamGetParsingDatas(h_Dynam);
        if (p_stParsingDatas != NULL)
        {
            MTH3D_tdstVector *p_stTarget = DNM_M_p_stCPDGetTarget(p_stParsingDatas);
            p_stValue->uParam.a_xVectorValue[0] = p_stTarget->xX;
            p_stValue->uParam.a_xVectorValue[1] = p_stTarget->xY;
            p_stValue->uParam.a_xVectorValue[2] = p_stTarget->xZ;
            p_stValue->eType = 0x15;
        }
    }
    return p_stTree;
}

/* 0x00488E20 */
AI_tdstNodeInterpret *__cdecl CAM_fn_p_stGetBestPos(
    HIE_tdstSuperObject *p_SuperObjPerso, AI_tdstNodeInterpret *p_stTree, AI_tdstGetSetParam *p_stValue)
{
    HIE_tdstSuperObject    *hCamera;
    struct tdstCineinfo_   *hCineinfo;
    long                    eFuncId = AI_M_eFuncIdInterpret(p_stTree-1);
    MTH3D_tdstVector        stVec;
    CAM_tdstUpdateCamera    stStruct;

    fn_vGetUltraOperatorPerso(fn_ucGetFunctionUltraOperator(eFuncId), p_SuperObjPerso, &hCamera);

    hCineinfo = M_GetMSHandle(hCamera, Cineinfo);

    if (!hCineinfo->bCanDoBestPos)
    {

        M_Full_GetSetParam_p_stVertex(p_stValue, POS_fn_p_stGetTranslationVector(hCamera->hGlobalMatrix));

        return(p_stTree);
    }

    CAM_fn_vSetCineinfoWorkFromCurrent(hCineinfo);
    CAM_fn_vInitCameraStructure(hCamera, &stStruct);

    cRefAxisIsAlreadyComputed = stStruct.hCineinfoWork->uwIAFlags & C_IAFlags_RefAxisIsAlreadyComputed ? 1 : 0;
    cNoDynChangeTheta = stStruct.hCineinfoWork->uwIAFlags & C_IAFlags_NoDynChangeTheta ? 1 : 0;
    AI_CAM_c_CanTestStatic = 1;

    CAM_fn_vComputeReferencePoint(&stStruct);
    CAM_fn_vComputeTarget(&stStruct);
    CAM_fn_vComputeRealWantedPos(&stStruct);
    CAM_fn_vComputeMovePosWithDynTheta(&stStruct);

    if(stStruct.hCineinfoCurrent->hSuperObjectTargeted == NULL)
        return(p_stTree);

    CAM_fn_vForceFailureVisibility(&stStruct);
    if(stStruct.hCineinfo->eState == CAM_e_State_FailureVisibility)
    {
        MTH3D_M_vCopyVector(&stVec, &stStruct.hCineinfo->stLastFailedWanted);
        hCineinfo->bCanDoBestPos = FALSE;
    }
    else
    {
        MTH3D_M_vNullVector(&stVec);
    }
    stStruct.hCineinfo->eState = CAM_e_State_GoToOptimal;

    M_Full_GetSetParam_p_stVertex(p_stValue, &stVec);

    return(p_stTree);
}
