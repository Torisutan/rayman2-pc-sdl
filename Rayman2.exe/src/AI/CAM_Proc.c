#include "GAM/cineinfo.h"

#include "MTH/MTH_flt.h"
#include "MTH/MTH3D.h"
#include "GAM/GAM_globals.h"
#include "AI/AI_types.h"
#include "AI/GSParam.h"
#include "GAM/GAM_types.h"
#include "AI/CAM_Base.h"
#include "AI/CAM_Tool.h"
#include "AI/WPWayPt.h"
#include "GAM/MSWay.h"
extern void __cdecl MEC_fn_vUpdateSpeed(HIE_tdstSuperObject *hSupObj);
extern void *__cdecl fn_p_stDynamGetDynamics(void *h_Dynam);
#include "GAM/STRUCTUR/Engmode.h"
extern void __cdecl CAM_fn_vUpdateGeneralCamera(struct CAM_tdstUpdateCamera_ *_p_stStruct);
extern void __cdecl CAM_fn_vSendParametersToParsing(void *_p_stStruct);

/* 0x0047DA40 */
AI_tdstNodeInterpret *__cdecl CAM_fn_p_stUpdatePosition(
    HIE_tdstSuperObject *_hSuperObjPerso, AI_tdstNodeInterpret *p_stTree)
{

    MS_tdxHandleToCineinfo      hCineinfo;
    CAM_tdstUpdateCamera        stStruct;
    MS_tdxHandleToDynam         h_Dynam;
    DNM_tdstDynamics            *p_stDynamics;

    hCineinfo = M_GetMSHandle(_hSuperObjPerso, Cineinfo);

    CAM_fn_vSetCineinfoWorkFromCurrent(hCineinfo);
    CAM_fn_vInitCameraStructure(_hSuperObjPerso,&stStruct);

    if ((CAM_g_hOldTargetedPerso) && (CAM_g_hOldTargetedPerso != hCineinfo->hWork->hSuperObjectTargeted))
            CAM_fn_vUpdateTargetPosition(hCineinfo);

    MEC_fn_vUpdateSpeed (stStruct.hCineinfoWork->hSuperObjectTargeted);

    if( ((h_Dynam = M_GetMSHandle(stStruct.hCineinfoWork->hSuperObjectTargeted, Dynam)) == NULL) ||
        ((p_stDynamics = fn_p_stDynamGetDynamics(h_Dynam)) == NULL)  )
    {
        return(p_stTree);
    }
    else if (!hCineinfo->bCanDoBestPos && DNM_M_bDynamicsIsActorMove(p_stDynamics))
    {
        hCineinfo->bCanDoBestPos = TRUE;
    }

    if (!MTH3D_M_bIsNullVector(MTH_M_p_stMoveGetLinear(DNM_M_p_stReportGetAbsoluteCurrPosition(DNM_M_p_stDynamicsGetReport(p_stDynamics)))))
    {
        CAM_fn_vRemindSpeed(hCineinfo,DNM_M_p_stReportGetAbsoluteCurrSpeed(DNM_M_p_stDynamicsGetReport(p_stDynamics)));
        CAM_fn_vRemindPos(stStruct.hCineinfo,DNM_M_p_stReportGetAbsoluteCurrPosition(DNM_M_p_stDynamicsGetReport(p_stDynamics)));
        CAM_fn_vRemindTime(stStruct.hCineinfo,M_xGetUsefulDeltaTimeInSeconds(GAM_g_stEngineStructure.stEngineTimer));
    }

    CAM_fn_vUpdateGeneralCamera(&stStruct);

    CAM_fn_vSendParametersToParsing(&stStruct);

    CAM_g_hOldTargetedPerso = hCineinfo->hWork->hSuperObjectTargeted;

    hCineinfo->ucVolIAFlags = 0;

    return (p_stTree);
}

void CAM_fn_vChangeARealParameter
(
	MS_tdxHandleToCineinfo		_hCineInfo,
	MTH_tdxReal					*_p_xInitialValue,
	MTH_tdxReal					*_p_xCurrentValue,
	MTH_tdxReal					*_p_xVisibilityValue,
	MTH_tdxReal					*_p_xWorkValue,
	long						_lTypeOfChange,
	long						_lChangingMode,
	MTH_tdxReal					_xValue
)
{

	MTH_tdxReal			xActualValue = MTH_C_ZERO;

	if (_lTypeOfChange == 1)
	{
		CAM_fn_vSetCineinfoWorkFromCurrent(_hCineInfo);
		_p_xCurrentValue = _p_xWorkValue;
	}

	switch(_lChangingMode)
	{
	case -1:
		xActualValue = *_p_xInitialValue;
		break;
	case 0:
		if (_hCineInfo->ucVolIAFlags & C_VolIAFlags_CurrentAlreadyCopiedInWork)
			xActualValue = MTH_M_xAdd(*_p_xWorkValue, _xValue);
		else
			xActualValue = MTH_M_xAdd(*_p_xCurrentValue, _xValue);
		break;
	case 1:
		xActualValue = _xValue;
		break;
	}

	*_p_xCurrentValue = xActualValue;

	if (_lTypeOfChange != 1)
	{
		*_p_xVisibilityValue = xActualValue;
		*_p_xWorkValue = xActualValue;
	}
}

void CAM_fn_vActivateCamera
(
	MS_tdxHandleToCineinfo	_hCineInfo,
	long					_lTypeOfChange,
	ACP_tdxBool				_bActive
)
{

    MS_tdxHandleToInternalCineinfo		hCineInfoCurrent;
    MS_tdxHandleToInternalCineinfo		hCineInfoTest;

	if (_lTypeOfChange == 1)
	{
		CAM_fn_vSetCineinfoWorkFromCurrent(_hCineInfo);
		hCineInfoCurrent = _hCineInfo->hWork;
		hCineInfoTest = hCineInfoCurrent;
	}
	else
	{
		hCineInfoCurrent = _hCineInfo->hCurrent;
		if(_hCineInfo->ucVolIAFlags & C_VolIAFlags_CurrentAlreadyCopiedInWork)
			hCineInfoTest = _hCineInfo->hWork;
		else
			hCineInfoTest = hCineInfoCurrent;
	}

	if (hCineInfoTest->bIsActive == _bActive)
		return;

	hCineInfoCurrent->bIsActive = _bActive;
	if(_bActive == FALSE)
		hCineInfoCurrent->eTypeOfViewport = CAM_e_NoViewport;

	if (_lTypeOfChange != 1)
	{
		if(_bActive == FALSE)
		{
			_hCineInfo->hVisibility->eTypeOfViewport = CAM_e_NoViewport;
			_hCineInfo->hWork->eTypeOfViewport = CAM_e_NoViewport;
		}
		_hCineInfo->hVisibility->bIsActive = _bActive;
		_hCineInfo->hWork->bIsActive = _bActive;
	}
}

void CAM_fn_vChangeAVectorParameter
(
	MS_tdxHandleToCineinfo		_hCineInfo,
	MTH3D_tdstVector			*_p_stInitialValue,
	MTH3D_tdstVector			*_p_stCurrentValue,
	MTH3D_tdstVector			*_p_stVisibilityValue,
	MTH3D_tdstVector			*_p_stWorkValue,
	long						_lTypeOfChange,
	long						_lChangingMode,
	MTH3D_tdstVector			*_p_stValue
)
{

	MTH3D_tdstVector	stActualValue;

	MTH3D_M_vNullVector(&stActualValue);

	if (_lTypeOfChange == 1)
	{
		CAM_fn_vSetCineinfoWorkFromCurrent(_hCineInfo);
		_p_stCurrentValue = _p_stWorkValue;
	}

	switch(_lChangingMode)
	{
	case -1:
		MTH3D_M_vCopyVector(&stActualValue, _p_stInitialValue);
		break;
	case 0:
		if (_hCineInfo->ucVolIAFlags & C_VolIAFlags_CurrentAlreadyCopiedInWork)
		{
			MTH3D_M_vAddVector(&stActualValue, _p_stWorkValue, _p_stValue);
		}
		else
		{
			MTH3D_M_vAddVector(&stActualValue, _p_stCurrentValue, _p_stValue);
		}
		break;
	case 1:
		MTH3D_M_vCopyVector(&stActualValue, _p_stValue);
		break;
	}

	MTH3D_M_vCopyVector(_p_stCurrentValue, &stActualValue);

	if (_lTypeOfChange != 1)
	{
		MTH3D_M_vCopyVector(_p_stVisibilityValue, &stActualValue);
		MTH3D_M_vCopyVector(_p_stWorkValue, &stActualValue);
	}
}

/* 0x0047DDE0 */
AI_tdstNodeInterpret *__cdecl CAM_fn_p_stChangeVectorParameter(
    HIE_tdstSuperObject *p_SuperObjPerso, AI_tdstNodeInterpret *p_stTree)
{
    MS_tdxHandleToCineinfo  hCineInfo;
    HIE_tdstSuperObject    *hCamera;
    long                    lChangingMode;
    long                    lTypeOfChange;
    AI_tdstGetSetParam      stValue;
    MTH3D_tdstVector       *p_stValue;
    MTH3D_tdstVector       *p_stInitialValue;
    MTH3D_tdstVector       *p_stCurrentValue;
    MTH3D_tdstVector       *p_stVisibilityValue;
    MTH3D_tdstVector       *p_stWorkValue;
    long                    eProcedureId=M_eProcedureIdInterpret(p_stTree-1);

    fn_vGetUltraOperatorPerso(fn_ucGetProcedureUltraOperator(eProcedureId),p_SuperObjPerso,&hCamera);

    hCineInfo = M_GetMSHandle(hCamera, Cineinfo);

    M_EvalNextParameter(&stValue);
    lTypeOfChange = M_GetSetParam_lValue(&stValue);

    M_EvalNextParameter(&stValue);
    lChangingMode = M_GetSetParam_lValue(&stValue);

    M_EvalNextParameter(&stValue);
    p_stValue = &M_GetSetParam_stVertexValue(&stValue);

    switch(eProcedureId)
    {

        case 0x1bd :
            p_stInitialValue = &(hCineInfo->hInit->stShiftTarget);
            p_stCurrentValue = &(hCineInfo->hCurrent->stShiftTarget);
            p_stVisibilityValue = &(hCineInfo->hVisibility->stShiftTarget);
            p_stWorkValue = &(hCineInfo->hWork->stShiftTarget);
            break;

        case 0x1be :
            p_stInitialValue = &(hCineInfo->hInit->stShiftPos);
            p_stCurrentValue = &(hCineInfo->hCurrent->stShiftPos);
            p_stVisibilityValue = &(hCineInfo->hVisibility->stShiftPos);
            p_stWorkValue = &(hCineInfo->hWork->stShiftPos);
            break;

        default :
            return (p_stTree);

    }

    CAM_fn_vChangeAVectorParameter
    (
        hCineInfo,
        p_stInitialValue,
        p_stCurrentValue,
        p_stVisibilityValue,
        p_stWorkValue,
        lTypeOfChange,
        lChangingMode,
        p_stValue
    );
    return (p_stTree);
}

/* 0x0047DEB0 */
AI_tdstNodeInterpret *__cdecl CAM_fn_p_stChangeRealParameter(
    HIE_tdstSuperObject *p_SuperObjPerso, AI_tdstNodeInterpret *p_stTree)
{
    MS_tdxHandleToCineinfo  hCineInfo;
    HIE_tdstSuperObject    *hCamera;
    MTH_tdxReal             xValue;
    long                    lChangingMode;
    long                    lTypeOfChange;
    AI_tdstGetSetParam      stValue;
    MTH_tdxReal            *p_xInitialValue=NULL;
    MTH_tdxReal            *p_xCurrentValue=NULL;
    MTH_tdxReal            *p_xVisibilityValue=NULL;
    MTH_tdxReal            *p_xWorkValue=NULL;
    long                    eProcedureId=M_eProcedureIdInterpret(p_stTree-1);

    fn_vGetUltraOperatorPerso(fn_ucGetProcedureUltraOperator(eProcedureId),p_SuperObjPerso,&hCamera);

    hCineInfo = M_GetMSHandle(hCamera, Cineinfo);

    M_EvalNextParameter(&stValue);
    lTypeOfChange = M_GetSetParam_lValue(&stValue);

    M_EvalNextParameter(&stValue);
    lChangingMode = M_GetSetParam_lValue(&stValue);

    M_EvalNextParameter(&stValue);
    xValue = M_GetSetParam_xValue(&stValue);

    switch(eProcedureId)
    {

        case 0x1bf :
            p_xInitialValue = &(hCineInfo->hInit->xDistMin);
            p_xCurrentValue = &(hCineInfo->hCurrent->xDistMin);
            p_xVisibilityValue = &(hCineInfo->hVisibility->xDistMin);
            p_xWorkValue = &(hCineInfo->hWork->xDistMin);
            break;

        case 0x1c0 :
            p_xInitialValue = &(hCineInfo->hInit->xDistMax);
            p_xCurrentValue = &(hCineInfo->hCurrent->xDistMax);
            p_xVisibilityValue = &(hCineInfo->hVisibility->xDistMax);
            p_xWorkValue = &(hCineInfo->hWork->xDistMax);
            break;

        case 0x1c1 :
            p_xInitialValue = &(hCineInfo->hInit->xBoundDistMin);
            p_xCurrentValue = &(hCineInfo->hCurrent->xBoundDistMin);
            p_xVisibilityValue = &(hCineInfo->hVisibility->xBoundDistMin);
            p_xWorkValue = &(hCineInfo->hWork->xBoundDistMin);
            break;

        case 0x1c2 :
            p_xInitialValue = &(hCineInfo->hInit->xBoundDistMax);
            p_xCurrentValue = &(hCineInfo->hCurrent->xBoundDistMax);
            p_xVisibilityValue = &(hCineInfo->hVisibility->xBoundDistMax);
            p_xWorkValue = &(hCineInfo->hWork->xBoundDistMax);
            break;

        case 0x1c3 :
            p_xInitialValue = &(hCineInfo->hInit->xAngleAlpha);
            p_xCurrentValue = &(hCineInfo->hCurrent->xAngleAlpha);
            p_xVisibilityValue = &(hCineInfo->hVisibility->xAngleAlpha);
            p_xWorkValue = &(hCineInfo->hWork->xAngleAlpha);
            break;

        case 0x1c4 :
            p_xInitialValue = &(hCineInfo->hInit->xAngleShiftAlpha);
            p_xCurrentValue = &(hCineInfo->hCurrent->xAngleShiftAlpha);
            p_xVisibilityValue = &(hCineInfo->hVisibility->xAngleShiftAlpha);
            p_xWorkValue = &(hCineInfo->hWork->xAngleShiftAlpha);
            break;

        case 0x1c5 :
            p_xInitialValue = &(hCineInfo->hInit->xAngleTheta);
            p_xCurrentValue = &(hCineInfo->hCurrent->xAngleTheta);
            p_xVisibilityValue = &(hCineInfo->hVisibility->xAngleTheta);
            p_xWorkValue = &(hCineInfo->hWork->xAngleTheta);
            break;

        case 0x1c6 :
            p_xInitialValue = &(hCineInfo->hInit->xAngleShiftTheta);
            p_xCurrentValue = &(hCineInfo->hCurrent->xAngleShiftTheta);
            p_xVisibilityValue = &(hCineInfo->hVisibility->xAngleShiftTheta);
            p_xWorkValue = &(hCineInfo->hWork->xAngleShiftTheta);
            break;

        case 0x1c7 :
            p_xInitialValue = &(hCineInfo->hInit->xLinearSpeed);
            p_xCurrentValue = &(hCineInfo->hCurrent->xLinearSpeed);
            p_xVisibilityValue = &(hCineInfo->hVisibility->xLinearSpeed);
            p_xWorkValue = &(hCineInfo->hWork->xLinearSpeed);
            break;

        case 0x1c8 :
            p_xInitialValue = &(hCineInfo->hInit->xLinearIncreaseSpeed);
            p_xCurrentValue = &(hCineInfo->hCurrent->xLinearIncreaseSpeed);
            p_xVisibilityValue = &(hCineInfo->hVisibility->xLinearIncreaseSpeed);
            p_xWorkValue = &(hCineInfo->hWork->xLinearIncreaseSpeed);
            break;

        case 0x1c9 :
            p_xInitialValue = &(hCineInfo->hInit->xLinearDecreaseSpeed);
            p_xCurrentValue = &(hCineInfo->hCurrent->xLinearDecreaseSpeed);
            p_xVisibilityValue = &(hCineInfo->hVisibility->xLinearDecreaseSpeed);
            p_xWorkValue = &(hCineInfo->hWork->xLinearDecreaseSpeed);
            break;

        case 0x1ca :
            p_xInitialValue = &(hCineInfo->hInit->xAngularSpeed);
            p_xCurrentValue = &(hCineInfo->hCurrent->xAngularSpeed);
            p_xVisibilityValue = &(hCineInfo->hVisibility->xAngularSpeed);
            p_xWorkValue = &(hCineInfo->hWork->xAngularSpeed);
            break;

        case 0x1cb :
            p_xInitialValue = &(hCineInfo->hInit->xAngularIncreaseSpeed);
            p_xCurrentValue = &(hCineInfo->hCurrent->xAngularIncreaseSpeed);
            p_xVisibilityValue = &(hCineInfo->hVisibility->xAngularIncreaseSpeed);
            p_xWorkValue = &(hCineInfo->hWork->xAngularIncreaseSpeed);
            break;

        case 0x1cc :
            p_xInitialValue = &(hCineInfo->hInit->xAngularDecreaseSpeed);
            p_xCurrentValue = &(hCineInfo->hCurrent->xAngularDecreaseSpeed);
            p_xVisibilityValue = &(hCineInfo->hVisibility->xAngularDecreaseSpeed);
            p_xWorkValue = &(hCineInfo->hWork->xAngularDecreaseSpeed);
            break;

        case 0x1cd :
            p_xInitialValue = &(hCineInfo->hInit->xTargetSpeed);
            p_xCurrentValue = &(hCineInfo->hCurrent->xTargetSpeed);
            p_xVisibilityValue = &(hCineInfo->hVisibility->xTargetSpeed);
            p_xWorkValue = &(hCineInfo->hWork->xTargetSpeed);
            break;

        case 0x1ce :
            p_xInitialValue = &(hCineInfo->hInit->xTargetIncreaseSpeed);
            p_xCurrentValue = &(hCineInfo->hCurrent->xTargetIncreaseSpeed);
            p_xVisibilityValue = &(hCineInfo->hVisibility->xTargetIncreaseSpeed);
            p_xWorkValue = &(hCineInfo->hWork->xTargetIncreaseSpeed);
            break;

        case 0x1cf :
            p_xInitialValue = &(hCineInfo->hInit->xTargetDecreaseSpeed);
            p_xCurrentValue = &(hCineInfo->hCurrent->xTargetDecreaseSpeed);
            p_xVisibilityValue = &(hCineInfo->hVisibility->xTargetDecreaseSpeed);
            p_xWorkValue = &(hCineInfo->hWork->xTargetDecreaseSpeed);
            break;

        case 0x1d0 :
            p_xInitialValue = &(hCineInfo->hInit->xFocal);
            p_xCurrentValue = &(hCineInfo->hCurrent->xFocal);
            p_xVisibilityValue = &(hCineInfo->hVisibility->xFocal);
            p_xWorkValue = &(hCineInfo->hWork->xFocal);
            break;

        case 0x1d1 :
            p_xInitialValue = &(hCineInfo->hInit->xZMin);
            p_xCurrentValue = &(hCineInfo->hCurrent->xZMin);
            p_xVisibilityValue = &(hCineInfo->hVisibility->xZMin);
            p_xWorkValue = &(hCineInfo->hWork->xZMin);
            break;

        case 0x1d2 :
            p_xInitialValue = &(hCineInfo->hInit->xZMax);
            p_xCurrentValue = &(hCineInfo->hCurrent->xZMax);
            p_xVisibilityValue = &(hCineInfo->hVisibility->xZMax);
            p_xWorkValue = &(hCineInfo->hWork->xZMax);
            break;

        default :
            break;
    }

    CAM_fn_vChangeARealParameter
    (
        hCineInfo,
        p_xInitialValue,
        p_xCurrentValue,
        p_xVisibilityValue,
        p_xWorkValue,
        lTypeOfChange,
        lChangingMode,
        xValue
    );
    return p_stTree;
}

/* 0x0047E200 */
AI_tdstNodeInterpret *__cdecl CAM_fn_p_stChangeTgtPerso(
    HIE_tdstSuperObject *p_SuperObjPerso, AI_tdstNodeInterpret *p_stTree)
{

    MS_tdxHandleToCineinfo          hCineInfo;
    MS_tdxHandleToInternalCineinfo  hCineInfoCurrent;
    HIE_tdstSuperObject            *hCamera;
    HIE_tdstSuperObject            *hNewSuperObjectTargeted;
    HIE_tdstSuperObject            *hOldSuperObjectTargeted;
    long                            lChangingMode;
    long                            lTypeOfChange;
    AI_tdstGetSetParam              stValue;
    long                            eProcedureId = M_eProcedureIdInterpret(p_stTree-1);

    fn_vGetUltraOperatorPerso(fn_ucGetProcedureUltraOperator(eProcedureId), p_SuperObjPerso, &hCamera);

    hCineInfo = M_GetMSHandle(hCamera, Cineinfo);

    M_EvalNextParameter(&stValue);
    lTypeOfChange = M_GetSetParam_lValue(&stValue);

    M_EvalNextParameter(&stValue);
    lChangingMode = M_GetSetParam_lValue(&stValue);

    M_EvalNextParameter(&stValue);
    hNewSuperObjectTargeted = fn_h_StandardGameGetSuperObject(stValue.uParam.p_stEngObjValue->h_StandardGame);

    if (lTypeOfChange == 1)
    {
        CAM_fn_vSetCineinfoWorkFromCurrent(hCineInfo);
        hCineInfoCurrent = hCineInfo->hWork;
    }
    else
    {
        hCineInfoCurrent = hCineInfo->hCurrent;
    }

    switch(lChangingMode)
    {
        case -1:
            hNewSuperObjectTargeted = hCineInfo->hInit->hSuperObjectTargeted;
            break;
        case 0:
            hNewSuperObjectTargeted = NULL;
            break;
        case 1:
            break;
    }

    hOldSuperObjectTargeted = hCineInfoCurrent->hSuperObjectTargeted;
    hCineInfoCurrent->hSuperObjectTargeted = hNewSuperObjectTargeted;

    if (lTypeOfChange != 1)
    {
        hCineInfo->hVisibility->hSuperObjectTargeted = hNewSuperObjectTargeted;
        hCineInfo->hWork->hSuperObjectTargeted = hNewSuperObjectTargeted;
    }

    if ((CAM_g_hOldTargetedPerso) && (CAM_g_hOldTargetedPerso != hNewSuperObjectTargeted))
        CAM_fn_vUpdateTargetPosition(hCineInfo);

    return p_stTree;
}

/* 0x0047E2E0 */
AI_tdstNodeInterpret *__cdecl CAM_fn_p_stChangeSecondTgtPerso(
    HIE_tdstSuperObject *p_SuperObjPerso, AI_tdstNodeInterpret *p_stTree)
{

    MS_tdxHandleToCineinfo          hCineInfo;
    MS_tdxHandleToInternalCineinfo  hCineInfoCurrent;
    HIE_tdstSuperObject            *hCamera;
    HIE_tdstSuperObject            *hNewSuperObjectTargeted;
    MTH_tdxReal                     xRatio;
    long                            lChangingMode;
    long                            lTypeOfChange;
    AI_tdstGetSetParam              stValue;
    long                            eProcedureId = M_eProcedureIdInterpret(p_stTree-1);

    fn_vGetUltraOperatorPerso(fn_ucGetProcedureUltraOperator(eProcedureId), p_SuperObjPerso, &hCamera);

    hCineInfo = M_GetMSHandle(hCamera, Cineinfo);

    M_EvalNextParameter(&stValue);
    lTypeOfChange = M_GetSetParam_lValue(&stValue);

    M_EvalNextParameter(&stValue);
    lChangingMode = M_GetSetParam_lValue(&stValue);

    M_EvalNextParameter(&stValue);
    hNewSuperObjectTargeted = fn_h_StandardGameGetSuperObject(stValue.uParam.p_stEngObjValue->h_StandardGame);

    M_EvalNextParameter(&stValue);
    xRatio = M_GetSetParam_xValue(&stValue);

    if(hCineInfo->ucPerIAFlags & C_PerIAFlags_VisibilityCopied)
    {
        CAM_fn_vSetCineinfoCurrentFromVisibility(hCineInfo);
        if(hCineInfo->ucVolIAFlags & C_VolIAFlags_CurrentAlreadyCopiedInWork)
            CAM_fn_vSetCineinfoWorkFromVisibility(hCineInfo);
    }

    if (lTypeOfChange == 1)
    {
        CAM_fn_vSetCineinfoWorkFromCurrent(hCineInfo);
        hCineInfoCurrent = hCineInfo->hWork;
    }
    else
    {
        hCineInfoCurrent = hCineInfo->hCurrent;
    }

    switch(lChangingMode)
    {
        case -1:
            hNewSuperObjectTargeted = hCineInfo->hInit->hSuperObjectTargeted;
            break;
        case 0:
            hNewSuperObjectTargeted = NULL;
            break;
        case 1:
            break;
    }

    hCineInfoCurrent->hSecondSuperObjectTargeted = hNewSuperObjectTargeted;

    if (lTypeOfChange != 1)
    {
        hCineInfo->hVisibility->hSecondSuperObjectTargeted = hNewSuperObjectTargeted;
        hCineInfo->hWork->hSecondSuperObjectTargeted = hNewSuperObjectTargeted;
    }

    hCineInfo->xApexRatio = xRatio;

    return p_stTree;
}

/* 0x0047E400 */
AI_tdstNodeInterpret *__cdecl CAM_fn_p_stChangeChannel(
    HIE_tdstSuperObject *p_SuperObjPerso, AI_tdstNodeInterpret *p_stTree)
{

    HIE_tdstSuperObject            *hCamera;
    MS_tdxHandleToCineinfo          hCineInfo;
    MS_tdxHandleToInternalCineinfo  hCineInfoCurrent;
    long                            lChangingMode;
    long                            lTypeOfChange;
    char                            cChannelNumber;
    AI_tdstGetSetParam              stValue;
    long                            eProcedureId = M_eProcedureIdInterpret(p_stTree-1);

    fn_vGetUltraOperatorPerso(fn_ucGetProcedureUltraOperator(eProcedureId), p_SuperObjPerso, &hCamera);

    hCineInfo = M_GetMSHandle(hCamera, Cineinfo);

    M_EvalNextParameter(&stValue);
    lTypeOfChange = M_GetSetParam_lValue(&stValue);

    M_EvalNextParameter(&stValue);
    lChangingMode = M_GetSetParam_lValue(&stValue);

    M_EvalNextParameter(&stValue);
    cChannelNumber = (char) M_GetSetParam_lModuleValue(&stValue);

    if (lTypeOfChange == 1)
    {
        CAM_fn_vSetCineinfoWorkFromCurrent(hCineInfo);
        hCineInfoCurrent = hCineInfo->hWork;
    }
    else
    {
        hCineInfoCurrent = hCineInfo->hCurrent;
    }

    switch(lChangingMode)
    {
        case -1:
            cChannelNumber =  hCineInfo->hInit->cChannel;
            break;
        case 0 :
            cChannelNumber = -1;
            break;
        case 1 :
            break;
    }

    hCineInfoCurrent->cChannel = cChannelNumber;

    if (lTypeOfChange != 1)
    {
        hCineInfo->hVisibility->cChannel = cChannelNumber;
        hCineInfo->hWork->cChannel = cChannelNumber;
    }

    return p_stTree;
}

/* 0x0047E4C0 */
AI_tdstNodeInterpret *__cdecl CAM_fn_p_stActivate(
    HIE_tdstSuperObject *p_SuperObjPerso, AI_tdstNodeInterpret *p_stTree)
{

    MS_tdxHandleToCineinfo  hCineInfo;
    HIE_tdstSuperObject    *hCamera;
    AI_tdstGetSetParam      stValue;
    ACP_tdxBool             bActive;
    long                    lTypeOfChange;
    long                    lValue;
    long                    eProcedureId = M_eProcedureIdInterpret(p_stTree-1);

    fn_vGetUltraOperatorPerso(fn_ucGetProcedureUltraOperator(eProcedureId), p_SuperObjPerso, &hCamera);

    hCineInfo = M_GetMSHandle(hCamera, Cineinfo);

    M_EvalNextParameter(&stValue);
    lTypeOfChange = M_GetSetParam_lValue(&stValue);

    M_EvalNextParameter(&stValue);
    lValue = M_GetSetParam_lValue(&stValue);
    if(lValue == 0)
        bActive = FALSE;
    else
        bActive = TRUE;

    CAM_fn_vActivateCamera(hCineInfo, lTypeOfChange, bActive);

    return p_stTree;
}
#include "GAM/ToolCam.h"
extern void __cdecl CAM_fn_vAssignCameraToAViewport(long _lViewportNumber, void *_hCamera);
extern void __cdecl GAM_fn_vChangeCharacterForMicro(void *_hCharacter);

/* 0x0047E540 */
AI_tdstNodeInterpret *__cdecl CAM_fn_p_stAssociateViewport(
    HIE_tdstSuperObject *p_SuperObjPerso, AI_tdstNodeInterpret *p_stTree)
{

    MS_tdxHandleToCineinfo  hNewCineInfo = NULL;
    MS_tdxHandleToCineinfo  hOldCineInfo = NULL;
    HIE_tdstSuperObject    *hNewCamera;
    HIE_tdstSuperObject    *hOldCamera;
    long                    lViewport;
    long                    lDeactived;
    AI_tdstGetSetParam      stValue;

    M_EvalNextParameter(&stValue);
    lViewport = M_GetSetParam_lValue(&stValue);

    M_EvalNextParameter(&stValue);
    lDeactived = M_GetSetParam_lValue(&stValue);

    M_EvalNextParameter(&stValue);
    hNewCamera = fn_h_StandardGameGetSuperObject(stValue.uParam.p_stEngObjValue->h_StandardGame);

    hOldCamera = CAM_fn_hGetActiveCamera(lViewport);

    if (hNewCamera == hOldCamera)
        return (p_stTree);
    if ((lViewport > CAM_Nb_Viewport) || (lViewport == CAM_e_NoViewport))
        return (p_stTree);

    if (hNewCamera != NULL)
        hNewCineInfo = M_GetMSHandle(hNewCamera, Cineinfo);
    if (hOldCamera != NULL)
        hOldCineInfo = M_GetMSHandle(hOldCamera, Cineinfo);

    if (hNewCamera != NULL)
    {
        CAM_fn_vActivateCamera(hNewCineInfo, 0, TRUE);

        hNewCineInfo->hWork->eTypeOfViewport = (enum e_ucListViewport_) lViewport;
        CAM_fn_vAssignCameraToAViewport(lViewport, hNewCamera);
        hNewCineInfo->hCurrent->eTypeOfViewport = (enum e_ucListViewport_) lViewport;
        hNewCineInfo->hVisibility->eTypeOfViewport = (enum e_ucListViewport_) lViewport;

        GAM_fn_vChangeCharacterForMicro(hNewCamera);
    }

    if ((hOldCamera != NULL) && (GAM_g_stEngineStructure.h_StdCamCaracter != hOldCamera))
    {
        if (lDeactived == 1)
            CAM_fn_vActivateCamera(hOldCineInfo, 0, FALSE);

        hOldCineInfo->hCurrent->eTypeOfViewport = CAM_e_NoViewport;
        hOldCineInfo->hVisibility->eTypeOfViewport = CAM_e_NoViewport;
        hOldCineInfo->hWork->eTypeOfViewport = CAM_e_NoViewport;
    }
    return (p_stTree);
}

/* 0x0047E670 */
AI_tdstNodeInterpret *__cdecl CAM_fn_p_stSetIAFlags(
    HIE_tdstSuperObject *p_SuperObjPerso, AI_tdstNodeInterpret *p_stTree)
{

    MS_tdxHandleToCineinfo  hCineInfo;
    HIE_tdstSuperObject    *hCamera;
    long                    lChangingMode;
    long                    lTypeOfChange;
    AI_tdstGetSetParam      stValue;
    unsigned short          uwFlag = 0;
    long                    eProcedureId = M_eProcedureIdInterpret(p_stTree-1);

    fn_vGetUltraOperatorPerso(fn_ucGetProcedureUltraOperator(eProcedureId), p_SuperObjPerso, &hCamera);

    hCineInfo = M_GetMSHandle(hCamera, Cineinfo);

    M_EvalNextParameter(&stValue);
    lTypeOfChange = M_GetSetParam_lValue(&stValue);

    M_EvalNextParameter(&stValue);
    lChangingMode = M_GetSetParam_lValue(&stValue);

    switch (eProcedureId)
    {
        case 0x1d8:
            uwFlag = hCineInfo->hInit->uwIAFlags;
            break;

        case 0x1d9:
            uwFlag = C_IAFlags_NoDynamicTarget;
            if (lChangingMode == -1)
            {
                if (hCineInfo->hInit->uwIAFlags & C_IAFlags_NoDynamicTarget)
                    lChangingMode = 1;
                else
                    lChangingMode = 0;
            }
            break;

        case 0x1da:
            uwFlag = C_IAFlags_NoAverageMoveTgtPerso;
            if (lChangingMode == -1)
            {
                if (hCineInfo->hInit->uwIAFlags & C_IAFlags_NoAverageMoveTgtPerso)
                    lChangingMode = 1;
                else
                    lChangingMode = 0;
            }
            break;

        case 0x1db:
            uwFlag = C_IAFlags_NoParseCutAngle;
            if (lChangingMode == -1)
            {
                if (hCineInfo->hInit->uwIAFlags & C_IAFlags_NoParseCutAngle)
                    lChangingMode = 1;
                else
                    lChangingMode = 0;
            }
            break;

        case 0x1dc:
            uwFlag = C_IAFlags_NoVisibility;
            if (lChangingMode == -1)
            {
                if (hCineInfo->hInit->uwIAFlags & C_IAFlags_NoVisibility)
                    lChangingMode = 1;
                else
                    lChangingMode = 0;
            }
            break;

        case 0x1dd:
            uwFlag = C_IAFlags_NoVisibilityWithDynHie;
            if (lChangingMode == -1)
            {
                if (hCineInfo->hInit->uwIAFlags & C_IAFlags_NoVisibilityWithDynHie)
                    lChangingMode = 1;
                else
                    lChangingMode = 0;
            }
            break;

        case 0x1de:
            uwFlag = C_IAFlags_NoDynChangeTheta;
            if (lChangingMode == -1)
            {
                if (hCineInfo->hInit->uwIAFlags & C_IAFlags_NoDynChangeTheta)
                    lChangingMode = 1;
                else
                    lChangingMode = 0;
            }
            break;

        case 0x1df:
            uwFlag = C_IAFlags_NoShiftUntilPosReached;
            if (lChangingMode == -1)
            {
                if (hCineInfo->hInit->uwIAFlags & C_IAFlags_NoShiftUntilPosReached)
                    lChangingMode = 1;
                else
                    lChangingMode = 0;
            }
            break;

        case 0x1e0:
            uwFlag = C_IAFlags_NoDynSpeed;
            if (lChangingMode == -1)
            {
                if (hCineInfo->hInit->uwIAFlags & C_IAFlags_NoDynSpeed)
                    lChangingMode = 1;
                else
                    lChangingMode = 0;
            }
            break;

        default:
            break;
    }

    CAM_fn_vSetResetInternalFlag
    (
        hCineInfo,
        &(hCineInfo->hCurrent->uwIAFlags),
        &(hCineInfo->hVisibility->uwIAFlags),
        &(hCineInfo->hWork->uwIAFlags),
        lTypeOfChange,
        lChangingMode,
        uwFlag
    );

    return p_stTree;
}

#define eProc_Cam_ResetDNMFlags                     0x1e1
#define eProc_Cam_SetFlagNoLinearParsing            0x1e2
#define eProc_Cam_SetFlagNoLinearInertia            0x1e3
#define eProc_Cam_SetFlagNoAngularParsing           0x1e4
#define eProc_Cam_SetFlagNoAngularInertia           0x1e5
#define eProc_Cam_SetFlagNoTargetParsing            0x1e6
#define eProc_Cam_SetFlagNoTargetInertia            0x1e7
#define eProc_Cam_SetFlagFixedOrientation           0x1e8
#define eProc_Cam_SetFlagNoObstacle                 0x1e9
#define eProc_Cam_SetFlagNoCollisionWhenNotMoving   0x1ea
AI_tdstNodeInterpret *__cdecl CAM_fn_p_stSetDNMFlags(HIE_tdstSuperObject *p_SuperObjPerso, AI_tdstNodeInterpret *p_stTree)
{

    MS_tdxHandleToCineinfo			hCineInfo;
	HIE_tdstSuperObject				*hCamera;
	long							lChangingMode;
	long							lTypeOfChange;
	AI_tdstGetSetParam				stValue;
	unsigned short					uwFlag = 0;
    long							eProcedureId=M_eProcedureIdInterpret(p_stTree-1);

	fn_vGetUltraOperatorPerso(fn_ucGetProcedureUltraOperator(eProcedureId),p_SuperObjPerso,&hCamera);

	hCineInfo = M_GetMSHandle(hCamera, Cineinfo);

 	M_EvalNextParameter(&stValue);
	lTypeOfChange = M_GetSetParam_lValue(&stValue);

	M_EvalNextParameter(&stValue);
	lChangingMode = M_GetSetParam_lValue(&stValue);

	switch (eProcedureId)
	{

		case eProc_Cam_ResetDNMFlags :
			uwFlag = hCineInfo->hInit->uwDNMFlags;
			break;

		case eProc_Cam_SetFlagNoLinearParsing :
			uwFlag = DNM_CAM_C_NoLinearParsing;
			if (lChangingMode == -1)
			{
				if(hCineInfo->hInit->uwDNMFlags & DNM_CAM_C_NoLinearParsing)
					lChangingMode = 1;
				else
					lChangingMode = 0;
			}
			break;

		case eProc_Cam_SetFlagNoLinearInertia :
			uwFlag = DNM_CAM_C_NoLinearInertia;
			if (lChangingMode == -1)
			{
				if(hCineInfo->hInit->uwDNMFlags & DNM_CAM_C_NoLinearInertia)
					lChangingMode = 1;
				else
					lChangingMode = 0;
			}
			break;

		case eProc_Cam_SetFlagNoAngularParsing :
			uwFlag = DNM_CAM_C_NoAngularParsing;
			if (lChangingMode == -1)
			{
				if(hCineInfo->hInit->uwDNMFlags & DNM_CAM_C_NoAngularParsing)
					lChangingMode = 1;
				else
					lChangingMode = 0;
			}
			break;

		case eProc_Cam_SetFlagNoAngularInertia :
			uwFlag = DNM_CAM_C_NoAngularInertia;
			if (lChangingMode == -1)
			{
				if(hCineInfo->hInit->uwDNMFlags & DNM_CAM_C_NoAngularInertia)
					lChangingMode = 1;
				else
					lChangingMode = 0;
			}
			break;

		case eProc_Cam_SetFlagNoTargetParsing :
			uwFlag = DNM_CAM_C_NoTargetParsing;
			if (lChangingMode == -1)
			{
				if(hCineInfo->hInit->uwDNMFlags & DNM_CAM_C_NoTargetParsing)
					lChangingMode = 1;
				else
					lChangingMode = 0;
			}
			break;

		case eProc_Cam_SetFlagNoTargetInertia :
			uwFlag = DNM_CAM_C_NoTargetInertia;
			if (lChangingMode == -1)
			{
				if(hCineInfo->hInit->uwDNMFlags & DNM_CAM_C_NoTargetInertia)
					lChangingMode = 1;
				else
					lChangingMode = 0;
			}
			break;

		case eProc_Cam_SetFlagFixedOrientation :
			uwFlag = DNM_CAM_C_FixedOrientation;
			if (lChangingMode == -1)
			{
				if(hCineInfo->hInit->uwDNMFlags & DNM_CAM_C_FixedOrientation)
					lChangingMode = 1;
				else
					lChangingMode = 0;
			}
			break;

		case eProc_Cam_SetFlagNoObstacle:
			uwFlag = DNM_CAM_C_NoObstacle;
			if (lChangingMode == -1)
			{
				if(hCineInfo->hInit->uwDNMFlags & DNM_CAM_C_NoObstacle)
					lChangingMode = 1;
				else
					lChangingMode = 0;
			}
			break;

		case eProc_Cam_SetFlagNoCollisionWhenNotMoving:
			uwFlag = DNM_CAM_C_NoCollisionWhenNotMoving;
			if (lChangingMode == -1)
			{
				if(hCineInfo->hInit->uwDNMFlags & DNM_CAM_C_NoCollisionWhenNotMoving)
					lChangingMode = 1;
				else
					lChangingMode = 0;
			}
			break;

		default :
			break;
	}

	CAM_fn_vSetResetInternalFlag
	(
		hCineInfo,
		&(hCineInfo->hCurrent->uwDNMFlags),
		&(hCineInfo->hVisibility->uwDNMFlags),
		&(hCineInfo->hWork->uwDNMFlags),
		lTypeOfChange,
		lChangingMode,
		uwFlag
	);

    return (p_stTree);
}

/* 0x0047EA00 */
AI_tdstNodeInterpret *__cdecl CAM_fn_p_stShowInfo(
    HIE_tdstSuperObject *p_SuperObjPerso, AI_tdstNodeInterpret *p_stTree)
{

    MS_tdxHandleToCineinfo  hCineInfo;
    HIE_tdstSuperObject    *hCamera;
    long                    lMode;
    AI_tdstGetSetParam      stValue;
    long                    eProcedureId = M_eProcedureIdInterpret(p_stTree-1);

    fn_vGetUltraOperatorPerso(fn_ucGetProcedureUltraOperator(eProcedureId), p_SuperObjPerso, &hCamera);

    hCineInfo = M_GetMSHandle(hCamera, Cineinfo);

    M_EvalNextParameter(&stValue);
    lMode = M_GetSetParam_lValue(&stValue);

    switch(lMode)
    {
    case 0:	hCineInfo->ucPerIAFlags &= ~C_PerIAFlags_ShowInfo;	break;
    case 1:	hCineInfo->ucPerIAFlags |= C_PerIAFlags_ShowInfo;	break;
    }

    return p_stTree;
}

/* 0x0047EA70 */
AI_tdstNodeInterpret *__cdecl CAM_fn_p_stChangeConstants(HIE_tdstSuperObject *p_SuperObjPerso, AI_tdstNodeInterpret *p_stTree)
{

    MS_tdxHandleToCineinfo		hCineInfo;
	HIE_tdstSuperObject			*hCamera;
	register long				lMode;
	MTH_tdxReal					xVal;

	AI_tdstGetSetParam			stValue;
    long						eProcedureId = M_eProcedureIdInterpret(p_stTree-1);
    int iValue;

	fn_vGetUltraOperatorPerso(fn_ucGetProcedureUltraOperator(eProcedureId),p_SuperObjPerso,&hCamera);

	M_EvalNextParameter(&stValue);
	lMode = M_GetSetParam_lValue(&stValue);

	M_EvalNextParameter(&stValue);
	xVal = M_GetSetParam_xValue(&stValue);

	if (lMode == 39)
	{
		hCineInfo = M_GetMSHandle(hCamera, Cineinfo);
		iValue = MTH_M_xRealToLong(xVal);
		if(iValue & 0x0002)
		{
			hCineInfo->ucVolIAFlags &= ~(C_VolIAFlags_CurrentAlreadyCopiedInWork);
			CAM_g_stCameraConstants.wFlags &= ~iValue;
			return p_stTree;
		}
		else
		{
			CAM_g_stCameraConstants.wFlags |= iValue;
			return p_stTree;
		}
	}

	if (lMode < 17)
	{

		*( ((MTH_tdxReal*)(&CAM_g_stCameraConstants.xRayCameraSphereToGo1)) + (lMode-1)) = xVal;
		return p_stTree;
	}
	if (lMode < 23)
	{

		iValue = MTH_M_xRealToLong(xVal);
		*( ((short*)(&CAM_g_stCameraConstants.xNumRayCameraCanGo)) + (lMode-17)) = ((short)iValue);
		return p_stTree;
	}
	if (lMode < 30)
	{

		return p_stTree;
	}
	if (lMode < 39)
	{

		*( ((MTH_tdxReal*)(&CAM_g_stCameraConstants.xDynSpeed_LinearFactNorm)) + (lMode-30)) = xVal;
		return p_stTree;
	}
	if (lMode == 40)
	{

		iValue = MTH_M_xRealToLong(xVal);
		CAM_g_stCameraConstants.wFlags &= ~((short) iValue);
		return p_stTree;
	}
	if (lMode < 44)
	{

		*( ((MTH_tdxReal*)(&CAM_g_stCameraConstants.xBaseCutAngle)) + (lMode-41)) = xVal;
		return p_stTree;
	}
	if (lMode == 44)
	{
		hCineInfo = M_GetMSHandle(hCamera, Cineinfo);
		hCineInfo -> bCanDoBestPos = xVal ? TRUE : FALSE;
		return p_stTree;
	}
	return p_stTree;
}

/* 0x0047EBD0 */
AI_tdstNodeInterpret *__cdecl CAM_fn_p_stConstants(
    HIE_tdstSuperObject *p_SuperObjPerso, AI_tdstNodeInterpret *p_stTree)
{
    unsigned long procId = (unsigned long)M_eProcedureIdInterpret(p_stTree-1);
    (void)p_SuperObjPerso;

    switch (procId)
    {
    case 0x1ec:
        CAM_memcpy(&CAM_g_stCopyCameraConstants, &CAM_g_stCameraConstants, sizeof(CAM_tdstCameraConstants));
        break;
    case 0x1ed:
        CAM_memcpy(&CAM_g_stCameraConstants, &CAM_g_stCopyCameraConstants, sizeof(CAM_tdstCameraConstants));
        break;
    default:
        break;
    }
    return p_stTree;
}

/* 0x0047EC20 */
AI_tdstNodeInterpret *__cdecl CAM_fn_p_stSetForce(
    HIE_tdstSuperObject *p_SuperObjPerso, AI_tdstNodeInterpret *p_stTree)
{

    MS_tdxHandleToCineinfo          hCineInfo;
    HIE_tdstSuperObject             *hCamera;
    long                            lChangingMode;
    long                            lTypeOfChange;
    AI_tdstGetSetParam              stValue;
    unsigned short                  uwFlag = 0;
    long                            eProcedureId = M_eProcedureIdInterpret(p_stTree-1);
    MTH3D_tdstVector                *p_stValue, stTempVector;

    fn_vGetUltraOperatorPerso(fn_ucGetProcedureUltraOperator(eProcedureId),p_SuperObjPerso,&hCamera);

    hCineInfo = M_GetMSHandle(hCamera, Cineinfo);

    M_EvalNextParameter(&stValue);
    lTypeOfChange = M_GetSetParam_lValue(&stValue);

    M_EvalNextParameter(&stValue);
    lChangingMode = M_GetSetParam_lValue(&stValue);

    M_EvalNextParameter(&stValue);
    p_stValue = &M_GetSetParam_stVertexValue(&stValue);
    MTH3D_M_vCopyVector(&stTempVector, p_stValue);

    switch (eProcedureId)
    {
        case 0x1ef:

            if(lChangingMode == 1)
            {
                MTH3D_M_vCopyVector(&hCineInfo->stForceTarget, p_stValue);
            }
            uwFlag = C_IAFlags_TargetIsAlreadyComputed;
            break;
        case 0x1f0:

            if(lChangingMode == 1)
            {
                MTH3D_M_vCopyVector(&hCineInfo->stForcePosition, p_stValue);
            }
            uwFlag = C_IAFlags_PositionIsAlreadyComputed;
            break;
        case 0x1f1:

            M_EvalNextParameter(&stValue);
            p_stValue = &M_GetSetParam_stVertexValue(&stValue);

            if(lChangingMode == 1)
            {
                MTH3D_M_vCopyVector(&hCineInfo->stForceRefAxisY, &stTempVector);
                MTH3D_M_vCopyVector(&hCineInfo->stForceRefAxisZ, p_stValue);
            }
            uwFlag = C_IAFlags_RefAxisIsAlreadyComputed;
            break;
        default :
            break;
    }

    CAM_fn_vSetResetInternalFlag
    (
        hCineInfo,
        &(hCineInfo->hCurrent->uwIAFlags),
        &(hCineInfo->hVisibility->uwIAFlags),
        &(hCineInfo->hWork->uwIAFlags),
        lTypeOfChange,
        lChangingMode,
        uwFlag
    );

    return (p_stTree);
}

/* 0x0047EA70 */
extern char	cRefAxisIsAlreadyComputed;
extern char	cNoDynChangeTheta;

/* 0x0047ED90 */
AI_tdstNodeInterpret *__cdecl CAM_fn_p_stReset(
    HIE_tdstSuperObject *p_SuperObjPerso, AI_tdstNodeInterpret *p_stTree)
{

    HIE_tdstSuperObject    *hCamera;
    MS_tdxHandleToCineinfo  hCineinfo;
    long                    eProcedureId = M_eProcedureIdInterpret(p_stTree-1);

    fn_vGetUltraOperatorPerso(fn_ucGetProcedureUltraOperator(eProcedureId), p_SuperObjPerso, &hCamera);

    hCineinfo = M_GetMSHandle(hCamera, Cineinfo);

    CAM_fn_vInitCompleteCineinfo(hCineinfo);

    return p_stTree;
}
void CAM_fn_vComputeRealWantedPos(void *_p_stStruct);
extern void __cdecl CAM_fn_vComputeMovePosWithDynTheta(struct CAM_tdstUpdateCamera_ *_p_stStruct);
extern void __cdecl CAM_fn_vForceFailureVisibility(struct CAM_tdstUpdateCamera_ *_p_stStruct);
extern void __cdecl CAM_fn_vComputeFailureVisibility(struct CAM_tdstUpdateCamera_ *_p_stStruct);

/* 0x0047EDD0 */
AI_tdstNodeInterpret *__cdecl CAM_fn_p_stForceBestPos(
    HIE_tdstSuperObject *p_SuperObjPerso, AI_tdstNodeInterpret *p_stTree)
{

    HIE_tdstSuperObject         *hCamera;
    MS_tdxHandleToCineinfo      hCineinfo;
    CAM_tdstUpdateCamera        stStruct;
    long                        eProcedureId = M_eProcedureIdInterpret(p_stTree-1);

    fn_vGetUltraOperatorPerso(fn_ucGetProcedureUltraOperator(eProcedureId),p_SuperObjPerso,&hCamera);

    hCineinfo = M_GetMSHandle(hCamera, Cineinfo);

    if (!hCineinfo->bCanDoBestPos)
    {

        MTH3D_M_vCopyVector(&hCineinfo->stForcePosition, POS_fn_vGetTranslationVectorReadOnly(hCamera->hGlobalMatrix));
        CAM_fn_vSetResetInternalFlag
        (
            hCineinfo,
            &(hCineinfo->hCurrent->uwIAFlags),
            &(hCineinfo->hVisibility->uwIAFlags),
            &(hCineinfo->hWork->uwIAFlags),
            1,
            1,
            C_IAFlags_PositionIsAlreadyComputed
        );
        return(p_stTree);
    }

    CAM_fn_vSetCineinfoWorkFromCurrent(hCineinfo);
    CAM_fn_vInitCameraStructure(hCamera, &stStruct);

    cRefAxisIsAlreadyComputed = stStruct.hCineinfoWork->uwIAFlags & C_IAFlags_RefAxisIsAlreadyComputed ? 1 : 0;
    cNoDynChangeTheta = stStruct.hCineinfoWork->uwIAFlags & C_IAFlags_NoDynChangeTheta ? 1 : 0;
    AI_CAM_gs_cCanTestStatic = 1;

    CAM_fn_vComputeReferencePoint(&stStruct);
    CAM_fn_vComputeTarget(&stStruct);
    CAM_fn_vComputeRealWantedPos(&stStruct);
    CAM_fn_vComputeMovePosWithDynTheta(&stStruct);

    if(stStruct.hCineinfoCurrent->hSuperObjectTargeted == NULL)
        return(p_stTree);

    CAM_fn_vForceFailureVisibility(&stStruct);
    CAM_fn_vComputeFailureVisibility(&stStruct);
    stStruct.hCineinfoWork->uwDNMFlags |= (DNM_CAM_C_NoLinearParsing|DNM_CAM_C_NoAngularParsing);

    hCineinfo->bCanDoBestPos = FALSE;

    return (p_stTree);
}

/* 0x0047EF20 */
AI_tdstNodeInterpret *__cdecl CAM_fn_p_stForceNormalState(
    HIE_tdstSuperObject *p_SuperObjPerso, AI_tdstNodeInterpret *p_stTree)
{

    HIE_tdstSuperObject    *hCamera;
    MS_tdxHandleToCineinfo  hCineinfo;
    long                    eProcedureId = M_eProcedureIdInterpret(p_stTree-1);

    fn_vGetUltraOperatorPerso(fn_ucGetProcedureUltraOperator(eProcedureId), p_SuperObjPerso, &hCamera);

    hCineinfo = M_GetMSHandle(hCamera, Cineinfo);

    if (hCineinfo->ucPerIAFlags & C_PerIAFlags_VisibilityCopied)
        CAM_fn_vSetCineinfoCurrentFromVisibility(hCineinfo);

    hCineinfo->ucPerIAFlags &= ~C_PerIAFlags_ConstraintMoveVisFailure;
    hCineinfo->eState = CAM_e_State_GoToOptimal;

    return p_stTree;
}

/* 0x0047EF80 */
AI_tdstNodeInterpret *__cdecl CAM_fn_p_stForceMovingOnRail(HIE_tdstSuperObject *p_SuperObjPerso, AI_tdstNodeInterpret *p_stTree)
{

	HIE_tdstSuperObject			*hCamera;
    MS_tdxHandleToCineinfo		hCineinfo;
 	CAM_tdstUpdateCamera		stStruct;
	long						lCurrentIndex;
	long						lNbWp;
	WP_tdhWayPoint				hWayPoint;
	WP_tdhWayPoint				hNextWayPoint =  NULL;
	WP_tdhWayPoint				hPreviousWayPoint = NULL;
	WP_tdhWayPoint				hOldPreviousWayPoint = NULL;
	MS_tdxHandleToMSWay			h_MSWay;
	MTH3D_tdstVector			stTargetPosition;
	MTH3D_tdstVector			stWpPosition;
	MTH3D_tdstVector			stPreviousWpPosition;
	MTH3D_tdstVector			stOldPreviousWpPosition;
	MTH3D_tdstVector			stNextWpPosition;
	MTH3D_tdstVector			stWantedPosition;

	MTH_tdxReal					xDistMin, xDistMax, xDistCurrent, xDistWanted = 0;
	MTH_tdxReal					xDist1, xDist2, xDist3;
	char						cCas = 0;
	ACP_tdxBool					bWantedPositionIsAlreadyComputed = FALSE;
	long						eProcedureId = M_eProcedureIdInterpret(p_stTree-1);

	static long					lSens = 1;   /* 0x004A410C */

	fn_vGetUltraOperatorPerso(fn_ucGetProcedureUltraOperator(eProcedureId),p_SuperObjPerso,&hCamera);

	hCineinfo = M_GetMSHandle(hCamera, Cineinfo);

	h_MSWay = M_GetMSHandle(hCamera, MSWay);
	if ( h_MSWay == NULL)
	    return (p_stTree);

	CAM_fn_vSetCineinfoWorkFromCurrent(hCineinfo);
	CAM_fn_vInitCameraStructure(hCamera, &stStruct);

	 xDistMin = hCineinfo->hWork->xDistMin;
	 xDistMax = hCineinfo->hWork->xDistMax;

	 if (MTH_M_bEqual(xDistMin, xDistMax))
		cCas = 1;

 	POS_fn_vGetTranslationVector(HIE_fn_hGetSuperObjectMatrix(hCineinfo->hWork->hSuperObjectTargeted), &stTargetPosition);

	xDistCurrent = MTH3D_M_xVectorGap(&stStruct.stCurrentCameraPos, &stTargetPosition);

	if (cCas)
	{
		if (!MTH_M_bEqualWithEpsilon(xDistMin, xDistCurrent, MTH_M_xConstFloatToReal(0.1f)))
			xDistWanted = xDistMin;
		else
		{	MTH3D_M_vCopyVector(&stWantedPosition, &stStruct.stCurrentCameraPos);
			bWantedPositionIsAlreadyComputed = TRUE;
		}
	}
	else
	{
		if (MTH_M_bLess(xDistCurrent, xDistMin))
			xDistWanted = xDistMin;
		else
		{
			if (MTH_M_bGreater(xDistCurrent, xDistMax))
				xDistWanted = xDistMax;
			else
			{	MTH3D_M_vCopyVector(&stWantedPosition, &stStruct.stCurrentCameraPos);
				xDistWanted = xDistMax;
				bWantedPositionIsAlreadyComputed = TRUE;
			}
		}
	}

	MTH3D_M_vCopyVector(&stWantedPosition, &stStruct.stCurrentCameraPos);
	if (!bWantedPositionIsAlreadyComputed)
	{
		CAM_fn_vComputeReferencePoint(&stStruct);

		lCurrentIndex = MSWay_fn_lGetCurrentIndexInPath(h_MSWay);
		hWayPoint = MSWay_fn_hGetWayPointOfPath (h_MSWay,lCurrentIndex);

		lNbWp = MSWay_fn_lGetNumberWayPointInPath(h_MSWay);

		WP_fnv_WayPoint_GetVertex(hWayPoint, &stWpPosition);

		if (CAM_fn_bEqualVectorWithEpsilon(&stWpPosition, &stStruct.stCurrentCameraPos, WP_fnx_WayPoint_GetRadius(hWayPoint)))

		{
			hPreviousWayPoint = MSWay_fn_hGetWayPointOfPath (h_MSWay,lCurrentIndex-1);
			if (hPreviousWayPoint)
				WP_fnv_WayPoint_GetVertex(hPreviousWayPoint, &stOldPreviousWpPosition);
			else
				MTH3D_M_vCopyVector(&stOldPreviousWpPosition, &stWpPosition);

			hWayPoint = MSWay_fn_hGetWayPointOfPath (h_MSWay,lCurrentIndex+1);
			MTH3D_M_vCopyVector(&stPreviousWpPosition, &stWpPosition);

			if (hWayPoint)
				WP_fnv_WayPoint_GetVertex(hWayPoint, &stWpPosition);

			xDist1= Cam_fn_xComputeProjectionRail(&stStruct.stTargetedPersoPos, &stPreviousWpPosition, &stOldPreviousWpPosition);

			xDist2 = Cam_fn_xComputeProjectionRail(&stStruct.stTargetedPersoPos, &stPreviousWpPosition, &stWpPosition);

			if (xDist1 <= xDist2)
			{
				lSens = -1;
				MSWay_fn_lIncrementCurrentIndexInPath(h_MSWay, -1);

				Cam_fn_vComputeMovingOnRail(&stStruct,&stWantedPosition,&stPreviousWpPosition, &stOldPreviousWpPosition,xDistWanted);
			}
			else
			{

				lSens = 1;
				MSWay_fn_lIncrementCurrentIndexInPath(h_MSWay, 1);
				Cam_fn_vComputeMovingOnRail(&stStruct,&stWantedPosition,&stWpPosition,&stPreviousWpPosition,xDistWanted);
			}
		}

		else
		{

			hPreviousWayPoint = MSWay_fn_hGetWayPointOfPath (h_MSWay,lCurrentIndex-lSens);
			if (hPreviousWayPoint)
				WP_fnv_WayPoint_GetVertex(hPreviousWayPoint, &stPreviousWpPosition);
			else
			{
				hPreviousWayPoint = hWayPoint;
				MTH3D_M_vCopyVector(&stPreviousWpPosition, &stWpPosition);
			}

			xDist1 = Cam_fn_xComputeProjectionRail(&stStruct.stTargetedPersoPos, &stPreviousWpPosition, &stWpPosition);

			hOldPreviousWayPoint = MSWay_fn_hGetWayPointOfPath (h_MSWay,lCurrentIndex - (2*lSens));
			if (hOldPreviousWayPoint)
				WP_fnv_WayPoint_GetVertex(hOldPreviousWayPoint, &stOldPreviousWpPosition);
			else
			{
				hOldPreviousWayPoint = hPreviousWayPoint;
				MTH3D_M_vCopyVector(&stOldPreviousWpPosition, &stPreviousWpPosition);
			}

			xDist2 = Cam_fn_xComputeProjectionRail(&stStruct.stTargetedPersoPos, &stPreviousWpPosition, &stOldPreviousWpPosition);

			hNextWayPoint = MSWay_fn_hGetWayPointOfPath (h_MSWay,lCurrentIndex+lSens);
			if (hNextWayPoint)
			{
				WP_fnv_WayPoint_GetVertex(hNextWayPoint, &stNextWpPosition);
			}
			else
			{
				hNextWayPoint = hWayPoint;
				MTH3D_M_vCopyVector(&stNextWpPosition, &stWpPosition);
			}

			xDist3 = Cam_fn_xComputeProjectionRail(&stStruct.stTargetedPersoPos, &stNextWpPosition, &stWpPosition);

			if (xDist3 <= xDist1)
			{
				if (xDist3 <= xDist2)
				{
					MTH3D_M_vCopyVector(&stWantedPosition, &stWpPosition);
				}
				else
				{
					if (xDist1 <= xDist2)
					{
						Cam_fn_vComputeMovingOnRail(&stStruct,&stWantedPosition,&stWpPosition,&stPreviousWpPosition,xDistWanted);
					}
					else
					{
						if (CAM_fn_bEqualVectorWithEpsilon(&stPreviousWpPosition, &stStruct.stCurrentCameraPos, WP_fnx_WayPoint_GetRadius(hPreviousWayPoint)))
						{
							Cam_fn_vComputeMovingOnRail(&stStruct,&stWantedPosition,&stPreviousWpPosition,&stOldPreviousWpPosition,xDistWanted);
							lSens = -lSens;
							MSWay_fn_lIncrementCurrentIndexInPath(h_MSWay, lSens);
						}
						else
						{
							MTH3D_M_vCopyVector(&stWantedPosition, &stPreviousWpPosition);
						}
					}
				}
			}
			else
			{
				if (xDist1 <= xDist2)
				{
					Cam_fn_vComputeMovingOnRail(&stStruct,&stWantedPosition,&stWpPosition,&stPreviousWpPosition,xDistWanted);
				}
				else
				{
					if (CAM_fn_bEqualVectorWithEpsilon(&stPreviousWpPosition, &stStruct.stCurrentCameraPos, WP_fnx_WayPoint_GetRadius(hPreviousWayPoint)))
					{
						lSens = -lSens;
						MSWay_fn_lIncrementCurrentIndexInPath(h_MSWay, lSens);
						Cam_fn_vComputeMovingOnRail(&stStruct,&stWantedPosition,&stPreviousWpPosition,&stOldPreviousWpPosition,xDistWanted);
					}
					else
					{
						MTH3D_M_vCopyVector(&stWantedPosition, &stPreviousWpPosition);
					}
				}
			}
		}
	}

	MTH3D_M_vCopyVector(&hCineinfo->stForcePosition, &stWantedPosition);
	CAM_fn_vSetResetInternalFlag
	(
		hCineinfo,
		&(hCineinfo->hCurrent->uwIAFlags),
		&(hCineinfo->hVisibility->uwIAFlags),
		&(hCineinfo->hWork->uwIAFlags),
		1,
		1,
		C_IAFlags_PositionIsAlreadyComputed
	);

    return (p_stTree);
}

/* 0x0047F560 */
AI_tdstNodeInterpret *__cdecl CAM_fn_p_stCameraMode(
    HIE_tdstSuperObject *p_SuperObjPerso, AI_tdstNodeInterpret *p_stTree)
{
    AI_tdstGetSetParam    stParam;
    AI_tdstNodeInterpret *p_stNext = fn_p_stEvalTree(p_SuperObjPerso, p_stTree, &stParam);

    GAM_g_stEngineStructure.cCameraMode = stParam.uParam.cValue;
    return p_stNext;
}
