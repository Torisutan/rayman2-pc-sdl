#include "GAM/cineinfo.h"

#include "MTH/MTH_flt.h"
#include "MTH/MTH3D.h"

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

#include "AI/AI_types.h"
#include "GAM/GAM_types.h"
#include "AI/CAM_Base.h"
#include "AI/CAM_Tool.h"
#include "AI/WPWayPt.h"
#include "GAM/MSWay.h"

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
