#include <stddef.h>

#include "AI/CAM_Base.h"
#include "POS/POS_types.h"
#include "GAM/GAM_types.h"
#include "GAM/ChanList.h"
#include "MTH/MTH_xNeg.h"

#define C_IAFlags_TargetIsAlreadyComputed  0x1000

/* 0x00414070 */
unsigned char __cdecl CAM_fn_cCineInfoGetChannel(void *_hCineInfoWork);

/* 0x004736B0 */
HIE_tdstSuperObject *CAM_fn_hGetFather(HIE_tdstSuperObject *_h_SuperObjPerso);

#define C_IAFlags_NoAverageMoveTgtPerso  0x0002

#define CAM_C_xEpsilonRefAxis  ((MTH_tdxReal)1.0e-4f)

/* 0x4E92FD */
char cRefAxisIsAlreadyComputed;

/* 0x4E92FC */
char cNoDynChangeTheta;

ACP_tdxBool CAM_fn_bEqualVectorWithEpsilon(MTH3D_tdstVector *_p_stVector1, MTH3D_tdstVector *_p_stVector2, MTH_tdxReal _xEpsilon);

/* 0x00473BB0 */
void CAM_fn_vGetPersoSightAxis(CAM_tdstUpdateCamera *_p_stStruct, MTH3D_tdstVector *_p_stAxis, MTH3D_tdstVector *_p_stAxisZ);

void CAM_fn_vComputeRefAxis(CAM_tdstUpdateCamera *_p_stStruct, MTH3D_tdstVector	*_p_stRefAxis)
{

	if (cRefAxisIsAlreadyComputed)
	{
		MTH3D_tdstVector stVectorNull;
		MTH3D_M_vNullVector(&stVectorNull);

		if (CAM_fn_bEqualVectorWithEpsilon(&_p_stStruct->hCineinfo->stForceRefAxisY, &stVectorNull, CAM_C_xEpsilonRefAxis))
		{
			MTH3D_M_vSetVectorElements(&_p_stStruct->hCineinfo->stForceRefAxisY, MTH_C_ZERO, -MTH_C_ONE, MTH_C_ZERO);
			MTH3D_M_vCopyVector(_p_stRefAxis, &_p_stStruct->hCineinfo->stForceRefAxisY);
		}
		else
		{
			MTH3D_M_vNormalizeVector(_p_stRefAxis, &_p_stStruct->hCineinfo->stForceRefAxisY);
		}

		if (CAM_fn_bEqualVectorWithEpsilon(&_p_stStruct->hCineinfo->stForceRefAxisZ, &stVectorNull, CAM_C_xEpsilonRefAxis))
		{
			MTH3D_M_vSetVectorElements(&_p_stStruct->hCineinfo->stForceRefAxisZ, MTH_C_ZERO, MTH_C_ZERO, MTH_C_ONE);
		}
		else
		{
			MTH3D_M_vNormalizeVector(&_p_stStruct->stCptPos.stRefAxisZ, &_p_stStruct->hCineinfo->stForceRefAxisZ);
		}
	}

	else if(_p_stStruct->hCineinfoWork->hSecondSuperObjectTargeted)
	{
		MTH3D_M_vSubVector(_p_stRefAxis, &_p_stStruct->stSecondTargetedPersoPos, &_p_stStruct->stTargetedPersoPos);
		MTH3D_M_vNormalizeVector(_p_stRefAxis, _p_stRefAxis);
		MTH3D_M_vSetVectorElements(&_p_stStruct->stCptPos.stRefAxisZ, MTH_C_ZERO, MTH_C_ZERO, MTH_C_ONE);
	}

	else
	{
		CAM_fn_vGetPersoSightAxis(_p_stStruct, _p_stRefAxis, &_p_stStruct->stCptPos.stRefAxisZ);
		MTH3D_M_vSetVectorElements(&_p_stStruct->stCptPos.stRefAxisZ, MTH_C_ZERO, MTH_C_ZERO, MTH_C_ONE);
		MTH3D_M_vNegVector(_p_stRefAxis, _p_stRefAxis);
	}
}

#include "GAM/PlayAnim.h"
static void CAM_fn_vComputeTargetedPersoPos(
	CAM_tdstUpdateCamera		*_p_stStruct,
	HIE_tdstSuperObject			*_hPerso,
	MTH3D_tdstVector			*_p_stTargetedPersoPos,
	MTH3D_tdstVector			*_p_stTargetedPersoRealPos)
{

	MTH3D_tdstVector			stTempVector;
	unsigned char				ucChannel;
	CHN_tdxHandleToChannelArray	hChannelList;
	MS_tdxHandleTo3dData		h3dData;

	if(_hPerso == NULL)
	{
		MTH3D_M_vNullVector(_p_stTargetedPersoPos);
		MTH3D_M_vNullVector(_p_stTargetedPersoRealPos);
		return;
	}

	ucChannel = CAM_fn_cCineInfoGetChannel(_p_stStruct->hCineinfoWork);
	if(ucChannel != C_ucUnknownChannel)
	{

		h3dData = M_GetMSHandle(_hPerso, 3dData);
		hChannelList = h3dData->hArrayOfChannels;
		_hPerso = hChannelList ? hChannelList[(unsigned int)(short)ucChannel].hSupObject : NULL;
	}

	POS_fn_vGetTranslationVector(HIE_fn_hGetSuperObjectGlobalMatrix(_hPerso), _p_stTargetedPersoRealPos);

	if ((_p_stStruct->hCineinfo->ucVolIAFlags & C_VolIAFlags_InitJustCopiedInCurrent) ||
		(_p_stStruct->hCineinfoWork->uwIAFlags & C_IAFlags_NoAverageMoveTgtPerso) ||
		((ucChannel == C_ucUnknownChannel) && (!M_GetMSHandle(_hPerso, Dynam))) ||
		(ucChannel != C_ucUnknownChannel))
	{
		MTH3D_M_vCopyVector(_p_stTargetedPersoPos, _p_stTargetedPersoRealPos);
	}
	else
	{
		CAM_fn_vAveragePosition(_p_stStruct->hCineinfo, CAM_C_xAverageForComputePos, _p_stTargetedPersoPos);
	}

	MTH3D_M_vCopyVector(&stTempVector, &_p_stStruct->hCineinfoWork->stShiftPos);
	/* 0x00483F1A */
	POS_fn_vRotateVector(&stTempVector, HIE_fn_hGetSuperObjectGlobalMatrix(_hPerso), &stTempVector);

	MTH3D_M_vAddVector(_p_stTargetedPersoPos, _p_stTargetedPersoPos, &stTempVector);
	MTH3D_M_vAddVector(_p_stTargetedPersoRealPos, _p_stTargetedPersoRealPos, &stTempVector);
}

void CAM_fn_vComputeReferencePoint(CAM_tdstUpdateCamera *_p_stStruct)
{

	if(_p_stStruct->hCineinfoWork->hSecondSuperObjectTargeted)
		_p_stStruct->hCineinfoWork->uwIAFlags |= C_IAFlags_NoAverageMoveTgtPerso;

	CAM_fn_vComputeTargetedPersoPos(_p_stStruct, _p_stStruct->hCineinfoWork->hSuperObjectTargeted,
			&_p_stStruct->stTargetedPersoPos, &_p_stStruct->stTargetedPersoRealPos);

	if(_p_stStruct->hCineinfoWork->hSecondSuperObjectTargeted)
	{
		CAM_fn_vComputeTargetedPersoPos(
			_p_stStruct,
			_p_stStruct->hCineinfoWork->hSecondSuperObjectTargeted,
			&_p_stStruct->stSecondTargetedPersoPos,
			&_p_stStruct->stSecondTargetedPersoRealPos);

		MTH3D_M_vLinearInterpolVector(
			&_p_stStruct->stTargetedPersoPos,
			&_p_stStruct->stTargetedPersoPos,
			&_p_stStruct->stSecondTargetedPersoPos,
			_p_stStruct->hCineinfo->xApexRatio);
		MTH3D_M_vLinearInterpolVector(
			&_p_stStruct->stTargetedPersoRealPos,
			&_p_stStruct->stTargetedPersoRealPos,
			&_p_stStruct->stSecondTargetedPersoRealPos,
			_p_stStruct->hCineinfo->xApexRatio);
	}
}

void CAM_fn_vComputeTarget(CAM_tdstUpdateCamera *_p_stStruct)
{

	MTH3D_tdstVector			stTempLook;
    HIE_tdstSuperObject		   *h_Perso;
	unsigned char				ucChannel;
	CHN_tdxHandleToChannelArray hChannelList;
	MS_tdxHandleTo3dData		h3dData;

	if(_p_stStruct->hCineinfoWork->uwIAFlags & C_IAFlags_TargetIsAlreadyComputed)
	{
		MTH3D_M_vCopyVector(&_p_stStruct->stCptPos.stTarget, &_p_stStruct->hCineinfo->stForceTarget);
	}

	else if (_p_stStruct->hCineinfoWork->hSuperObjectTargeted == NULL)
	{
		MTH3D_M_vNullVector(&_p_stStruct->stCptPos.stTarget);
	}

	else if(_p_stStruct->hCineinfoWork->hSecondSuperObjectTargeted)
	{
		MTH3D_M_vCopyVector(&_p_stStruct->stCptPos.stTarget, &_p_stStruct->stTargetedPersoPos);
	}

	else
	{

		h_Perso = _p_stStruct->hCineinfoWork->hSuperObjectTargeted;
		if ((ucChannel = CAM_fn_cCineInfoGetChannel(_p_stStruct->hCineinfoWork)) != C_ucUnknownChannel)
		{

			h3dData = M_GetMSHandle(h_Perso, 3dData);
			hChannelList = h3dData->hArrayOfChannels;
			h_Perso = hChannelList ? hChannelList[(unsigned int)(short)ucChannel].hSupObject : NULL;
		}

		MTH3D_M_vCopyVector(&stTempLook, &_p_stStruct->hCineinfoWork->stShiftTarget);

		POS_fn_vRotateVector(&stTempLook, HIE_fn_hGetSuperObjectGlobalMatrix(h_Perso), &stTempLook);
		MTH3D_M_vAddVector(&_p_stStruct->stCptPos.stTarget, &_p_stStruct->stTargetedPersoPos, &stTempLook);
		/* 0x00484182 */
		MTH3D_M_vAddVector(&_p_stStruct->stCptPos.stTarget, &_p_stStruct->stTargetedPersoPos, &stTempLook);

	}
}

static void CAM_fn_vTakeCareOfFather(CAM_tdstUpdateCamera *_p_stStruct)
{
	HIE_tdstSuperObject			*hFather;

	hFather = CAM_fn_hGetFather(_p_stStruct->hCineinfoWork->hSuperObjectTargeted);
}

#define C_IAFlags_NoShiftUntilPosReached  0x0040
extern struct tdstInternalStructurCineinfo_ gst_Ideal;
void CAM_fn_vComputeMovePosWithDynTheta(CAM_tdstUpdateCamera *_p_stStruct);

void CAM_fn_vComputeRealWantedPos(CAM_tdstUpdateCamera *_p_stStruct)
{
    CAM_tdstUpdateCamera    stTempStruct;

    stTempStruct = *_p_stStruct;

    if(_p_stStruct->hCineinfo->ucPerIAFlags & C_PerIAFlags_VisibilityCopied)
    {
        gst_Ideal = *_p_stStruct->hCineinfoVisibility;

        gst_Ideal.xDistMin = _p_stStruct->hCineinfoWork->xDistMin;
        gst_Ideal.xDistMax = _p_stStruct->hCineinfoWork->xDistMax;
    }
    else
        gst_Ideal = *_p_stStruct->hCineinfoWork;

    gst_Ideal.uwIAFlags |= C_IAFlags_NoShiftUntilPosReached;
    stTempStruct.hCineinfoWork = &gst_Ideal;
    CAM_fn_vComputeMovePosWithDynTheta(&stTempStruct);
    MTH3D_M_vCopyVector(&_p_stStruct->stRealWantedCameraPos, &stTempStruct.stWantedCameraPos);
}

/* 0x00473660 */
ACP_tdxBool CAM_fn_bPersoOnFace(HIE_tdstSuperObject *_h_SuperObjPerso, MTH3D_tdstVector *_p_stNormal);
/* 0x004736F0 */
void CAM_fn_vComputeProjection(MTH3D_tdstVector *_p_sDestVector, MTH3D_tdstVector *_p_sSrcVector, MTH3D_tdstVector *_p_sNormVector);
MTH_tdxReal CAM_fn_xComputeRotation(MTH3D_tdstVector *_p_stDestVector, MTH3D_tdstVector *_p_stSrcVector, MTH3D_tdstVector *_p_stAxisVector);

char CAM_fn_cDynChangeTheta(CAM_tdstUpdateCamera *_p_stStruct, MTH3D_tdstVector *_p_stTestedPos)
{

	MTH3D_tdstVector		stMove, stRef, stSightAxis;
	MTH3D_tdstVector		stNormal;
	MTH_tdxReal				xAngle, xThetaAngle;
	MTH_tdxReal				xCos;

	if(!CAM_fn_bPersoOnFace(_p_stStruct->hCineinfoWork->hSuperObjectTargeted, &stNormal))
		return 0;

	CAM_fn_vComputeRefAxis(_p_stStruct, &stSightAxis);
	CAM_fn_vComputeProjection(&stMove, &stSightAxis, &stNormal);
	MTH3D_M_vNormalizeVector(&stMove, &stMove);

	MTH3D_M_vSetVectorElements(&stRef, MTH_C_ZERO, MTH_C_ZERO, MTH_C_ONE);
	xAngle = CAM_fn_xComputeRotation(&stRef, &stMove, NULL);
	xAngle = MTH_M_xSub(xAngle, MTH_C_PiBy2);

	MTH3D_M_vSubVector(&stRef, &_p_stStruct->stTargetedPersoPos, _p_stTestedPos);
	MTH3D_M_vNormalizeVector(&stRef, &stRef);
	xCos = MTH3D_M_xDotProductVector(&stRef, &stMove);
	xAngle = MTH_M_xMul(xAngle, xCos);

	if(_p_stStruct->hCineinfo->ucPerIAFlags & C_PerIAFlags_VisibilityCopied)
		xThetaAngle = _p_stStruct->hCineinfoVisibility->xAngleTheta;
	else

		xThetaAngle = _p_stStruct->hCineinfoWork->xAngleTheta;

	if(MTH_M_bLess(MTH_M_xMul(xThetaAngle, xAngle), MTH_C_ZERO))
	{
		xAngle = xAngle;
	}
	else if (MTH_M_bLessEqual(MTH_M_xAbs(xThetaAngle), MTH_M_xAbs(xAngle)))
	{
		xAngle = MTH_M_xSub(xAngle, xThetaAngle);
	}
	else
	{
		xAngle = MTH_C_ZERO;
		return 0;
	}

	_p_stStruct->hCineinfoWork->xAngleTheta = MTH_M_xAdd(xThetaAngle, xAngle);
	return 1;
}

#define CAM_C_xDynSpeed_LinearFactNorm		(CAM_g_stCameraConstants.xDynSpeed_LinearFactNorm)
#define CAM_C_xDynSpeed_LinearMax			(CAM_g_stCameraConstants.xDynSpeed_LinearMax)
#define CAM_C_xDynSpeed_LinearMulPerso		(CAM_g_stCameraConstants.xDynSpeed_LinearMulPerso)
#define CAM_C_xDynSpeed_AngularFactNorm		(CAM_g_stCameraConstants.xDynSpeed_AngularFactNorm)
#define CAM_C_xDynSpeed_AngularMax			(CAM_g_stCameraConstants.xDynSpeed_AngularMax)
#define CAM_C_xDynSpeed_AngularMulPerso		(CAM_g_stCameraConstants.xDynSpeed_AngularMulPerso)
#define CAM_C_xDynSpeed_TargetFactNorm		(CAM_g_stCameraConstants.xDynSpeed_TargetFactNorm)
#define CAM_C_xDynSpeed_TargetMax			(CAM_g_stCameraConstants.xDynSpeed_TargetMax)
#define CAM_C_xDynSpeed_TargetMulPerso		(CAM_g_stCameraConstants.xDynSpeed_TargetMulPerso)
#define C_IAFlags_NoDynSpeed				0x0080
#define C_IAFlags_PositionIsAlreadyComputed	0x2000
#define DNM_CAM_C_NoLinearParsing			0x0001
#define DNM_CAM_C_NoAngularParsing			0x0004
#define DNM_CAM_C_NoTargetParsing			0x0010
#include "MEC/DNMRepor.h"
#define DNM_M_p_stCPDGetTarget(_p_stParsingDatas)			(&(_p_stParsingDatas)->stVector)

extern void *__cdecl fn_p_stDynamGetParsingDatas(void *h_Dynam);
ACP_tdxBool CAM_fn_bPersoIsTurning(HIE_tdstSuperObject *_h_SuperObjPerso, MTH_tdxReal *_p_xAngularSpeed);
ACP_tdxBool CAM_fn_bPersoIsMoving(CAM_tdstUpdateCamera *_p_stStruct, MTH3D_tdstVector *_p_stMove);
MTH3D_tdstVector * __cdecl MTH_p_stRotationMulVector(MTH_tdstRotation *_p_stRotation, MTH3D_tdstVector *_p_stSource, MTH3D_tdstVector *_p_stResult);

#define C_IAFlags_NoShiftUntilPosReached  0x0040
#define C_PerIAFlags_NoShift              0x04

#define CAM_e_State_GoToComputedOptimal   1

#define CAM_C_xBaseCutAngle               (CAM_g_stCameraConstants.xBaseCutAngle)

/* 0x00473B10 */
MTH_tdxReal CAM_fn_xSetAngleInIntervalPiPi(MTH_tdxReal _xAngle);
MTH_tdxReal CAM_fn_xSetAngleInInterval02Pi(MTH_tdxReal _xAngle);
void CAM_fn_vComputeAnglesBetweenTwoVectors(MS_tdxHandleToInternalCineinfo _hIntCineInfo,
	MTH3D_tdstVector *_p_stSourceVector, MTH3D_tdstVector *_p_stTargetVector,
	MTH_tdxReal *_p_xHorizontalAngle, MTH_tdxReal *_p_xVerticalAngle, MTH3D_tdstVector *_p_stRefAxisZ);

static void CAM_fn_vBoundAngleWithTolerance
(
	CAM_tdstUpdateCamera	*_p_stStruct,
	MTH_tdxReal				*_p_xAngleToBound,
	MTH_tdxReal				*_p_xCutAngle,
	MTH_tdxReal				*_p_xDeltaAngle,
	MTH_tdxReal				_xRequestAngle,
	MTH_tdxReal				_xTolerance
)
{

	MTH_tdxReal	xAngleMin;
	MTH_tdxReal	xAngleMax;
	MTH_tdxReal	xDelta;
	MTH_tdxReal	xNewAngle;
	int			bInTolerance;

	if (MTH_M_bGreaterEqual(_xTolerance, MTH_C_Pi))
	{
		*_p_xCutAngle = *_p_xAngleToBound;
		return;
	}

	xNewAngle = _xRequestAngle;
	if (MTH_M_bGreater(_xTolerance, MTH_C_ZERO))
	{

		/* 0x00484258 */
		_xTolerance = MTH_M_xSub(_xTolerance, (MTH_tdxReal)-1.0e-4f);
		xAngleMin = MTH_M_xSub(_xRequestAngle, _xTolerance);
		if (MTH_M_bLessZero(xAngleMin))
			xAngleMin = MTH_M_xAdd(xAngleMin, MTH_C_2Pi);

		xAngleMax = MTH_M_xAdd(_xRequestAngle, _xTolerance);
		if (MTH_M_bGreater(xAngleMax, MTH_C_2Pi))
			xAngleMax = MTH_M_xSub(xAngleMax, MTH_C_2Pi);

		if (MTH_M_bGreater(xAngleMax, xAngleMin))
			bInTolerance = MTH_M_bInRangeEqual(*_p_xAngleToBound, xAngleMin, xAngleMax);
		else
			bInTolerance = !MTH_M_bInRange(*_p_xAngleToBound, xAngleMax, xAngleMin);

		if (!bInTolerance)
		{
			xDelta = MTH_M_xSub(*_p_xAngleToBound, _xRequestAngle);
			xDelta = CAM_fn_xSetAngleInIntervalPiPi(xDelta);

			if (MTH_M_bGreater(xDelta, MTH_C_ZERO))
				xNewAngle = xAngleMax;
			else
				xNewAngle = xAngleMin;
		}
		else
			xNewAngle = *_p_xAngleToBound;
	}

	xDelta = MTH_M_xSub(xNewAngle, *_p_xAngleToBound);
	xDelta = CAM_fn_xSetAngleInIntervalPiPi(xDelta);
	*_p_xDeltaAngle = xDelta;
	if(MTH_M_bGreaterEqual(MTH_M_xAbs(xDelta), CAM_C_xBaseCutAngle))
	{
		if (MTH_M_bGreaterZero(xDelta))
			*_p_xCutAngle = MTH_M_xAdd(*_p_xAngleToBound, CAM_C_xBaseCutAngle);
		else
			*_p_xCutAngle = MTH_M_xSub(*_p_xAngleToBound, CAM_C_xBaseCutAngle);
		*_p_xCutAngle  = CAM_fn_xSetAngleInInterval02Pi(*_p_xCutAngle);
	}
	else
		*_p_xCutAngle = xNewAngle;

	*_p_xAngleToBound = xNewAngle;
}

static void CAM_fn_vComputeCameraAxisAngles
(
	CAM_tdstUpdateCamera	*_p_stStruct,
	MTH3D_tdstVector		*_p_stCameraAxis,
	MTH3D_tdstVector		*_p_stCamTargetAxis,
	MTH_tdxReal				*_p_xHorAngle,
	MTH_tdxReal				*_p_xVerAngle
)
{

	MTH_tdxReal							xTemp, xShiftAlpha, xShiftTheta;
	MTH_tdxReal							xAngleTheta;
	MS_tdxHandleToInternalCineinfo		hCineInfoWork;

	hCineInfoWork = _p_stStruct->hCineinfoWork;

	CAM_fn_vComputeAnglesBetweenTwoVectors(hCineInfoWork, _p_stCameraAxis, _p_stCamTargetAxis,
			_p_xHorAngle, _p_xVerAngle, &_p_stStruct->stCptPos.stRefAxisZ);

	if ((_p_stStruct->hCineinfo->ucPerIAFlags & C_PerIAFlags_NoShift) ||
		(_p_stStruct->hCineinfoWork->uwIAFlags & C_IAFlags_NoShiftUntilPosReached))
	{
		xShiftAlpha = MTH_C_ZERO;
		xShiftTheta = MTH_C_ZERO;
	}
	else
	{
		xShiftAlpha = _p_stStruct->hCineinfoWork->xAngleShiftAlpha;
		xShiftTheta = _p_stStruct->hCineinfoWork->xAngleShiftTheta;
	}

	_p_stStruct->cHasCutAlpha = 0;

	CAM_fn_vBoundAngleWithTolerance(_p_stStruct, _p_xHorAngle, &_p_stStruct->xAngleAlphaCut,
			&_p_stStruct->xAngleDeltaAlphaCut, _p_stStruct->hCineinfoWork->xAngleAlpha, xShiftAlpha);

	if(!(MTH_M_bEqual(*_p_xHorAngle, _p_stStruct->xAngleAlphaCut)))
		_p_stStruct->cHasCutAlpha = 1;

	if(_p_stStruct->hCineinfo->eState == CAM_e_State_GoToComputedOptimal)
		xAngleTheta = _p_stStruct->hCineinfoCurrent->xAngleTheta;
	else
		xAngleTheta = _p_stStruct->hCineinfoWork->xAngleTheta;

	CAM_fn_vBoundAngleWithTolerance(_p_stStruct, _p_xVerAngle, &xTemp, &xTemp,
					xAngleTheta, xShiftTheta);
}

static void CAM_fn_vRotateCameraAxis(MTH3D_tdstVector *_p_stCameraAxis, MTH_tdxReal _xHorAngle, MTH_tdxReal _xVerAngle, MTH3D_tdstVector *_p_stRefAxisZ)
{
	MTH_tdstRotation	stRotation;

	stRotation.m_xAngle = _xHorAngle;
	MTH3D_M_vCopyVector(&stRotation.m_stAxis, _p_stRefAxisZ);
	MTH_p_stRotationMulVector(&stRotation, _p_stCameraAxis, _p_stCameraAxis);

	MTH3D_M_vCrossProductVectorWithoutBuffer(&stRotation.m_stAxis, _p_stCameraAxis, _p_stRefAxisZ);
	MTH3D_M_vNormalizeVector(&stRotation.m_stAxis, &stRotation.m_stAxis);
	stRotation.m_xAngle = _xVerAngle;
	MTH_p_stRotationMulVector(&stRotation, _p_stCameraAxis, _p_stCameraAxis);
}

void CAM_fn_vTakeCareOfCameraAltitude(MS_tdxHandleToCineinfo _hCineinfo, MTH3D_tdstVector *_p_stNewCameraPosition)
{
	struct tdstInternalStructurCineinfo_ *pWork = _hCineinfo->hWork;
	MTH_tdxReal		xZCamera;

	xZCamera = _p_stNewCameraPosition->xZ;

	if (MTH_M_bGreater(xZCamera, _hCineinfo->hWork->xZMax))
		_p_stNewCameraPosition->xZ = _hCineinfo->hWork->xZMax;

	if (MTH_M_bLess(xZCamera, _hCineinfo->hWork->xZMin))
		_p_stNewCameraPosition->xZ = _hCineinfo->hWork->xZMin;
}

void CAM_fn_vComputeMovePos(CAM_tdstUpdateCamera *_p_stStruct)
{

	MTH3D_tdstVector		stDir;
	MTH3D_tdstVector		stCamTargetVector;
	MTH_tdxReal				xNorm;
	MTH_tdxReal				xDistMax;
	MTH_tdxReal				xAlphaAngle, xThetaAngle;

	if (_p_stStruct->hCineinfoWork->uwIAFlags & C_IAFlags_PositionIsAlreadyComputed)
	{
		if(cRefAxisIsAlreadyComputed)
		{
			MTH3D_M_vNormalizeVector(&_p_stStruct->stCptPos.stRefAxisZ, &_p_stStruct->hCineinfo->stForceRefAxisZ);
		}
		else
		{
			MTH3D_M_vSetVectorElements(&_p_stStruct->stCptPos.stRefAxisZ, MTH_C_ZERO, MTH_C_ZERO, MTH_C_ONE);
		}

		cNoDynChangeTheta = 1;

		MTH3D_M_vCopyVector(&_p_stStruct->stWantedCameraPos, &_p_stStruct->hCineinfo->stForcePosition);
		MTH3D_M_vSubVector(&_p_stStruct->stCptPos.stMovePos, &_p_stStruct->stWantedCameraPos, &_p_stStruct->stCurrentCameraPos);
	}

	else if(!_p_stStruct->hCineinfoWork->hSuperObjectTargeted)
	{
		if(cRefAxisIsAlreadyComputed)
		{
			MTH3D_M_vNormalizeVector(&_p_stStruct->stCptPos.stRefAxisZ, &_p_stStruct->hCineinfo->stForceRefAxisZ);
		}
		else
		{
			MTH3D_M_vSetVectorElements(&_p_stStruct->stCptPos.stRefAxisZ, MTH_C_ZERO, MTH_C_ZERO, MTH_C_ONE);
		}

		CAM_fn_vComputeRefAxis(_p_stStruct, &stDir);
		MTH3D_M_vCopyVector(&_p_stStruct->stWantedCameraPos, &_p_stStruct->stCurrentCameraPos);
		MTH3D_M_vNullVector(&_p_stStruct->stCptPos.stMovePos);
	}

	else
	{

		MTH3D_M_vSubVector(&stCamTargetVector, &_p_stStruct->stCurrentCameraPos, &_p_stStruct->stTargetedPersoPos);

		if ((stCamTargetVector.xX == _p_stStruct->stCptPos.stRefAxisZ.xX) &&
			(stCamTargetVector.xY == _p_stStruct->stCptPos.stRefAxisZ.xY))

				stCamTargetVector.xX += (double)MTH_C_Inv2;

		CAM_fn_vComputeRefAxis(_p_stStruct, &stDir);

		CAM_fn_vComputeCameraAxisAngles(_p_stStruct, &stDir, &stCamTargetVector, &xAlphaAngle, &xThetaAngle);

		CAM_fn_vRotateCameraAxis(&stDir, xAlphaAngle, xThetaAngle, &_p_stStruct->stCptPos.stRefAxisZ);

		xNorm = MTH3D_M_xNormVector(&stCamTargetVector);

		xDistMax = _p_stStruct->hCineinfoWork->xDistMax;

		if (MTH_M_bLess( xNorm, _p_stStruct->hCineinfoWork->xDistMin))
			xNorm = _p_stStruct->hCineinfoWork->xDistMin;
		else if (MTH_M_bGreater(xNorm, xDistMax))
			xNorm = xDistMax;

		if (MTH_M_bLessEqual(xNorm,  MTH_M_xConstFloatToReal(0.01f)))
			xNorm = MTH_M_xConstFloatToReal(0.01f);

		MTH3D_M_vMulScalarVector(&stDir, xNorm, &stDir);

		MTH3D_M_vAddVector(&_p_stStruct->stWantedCameraPos, &_p_stStruct->stTargetedPersoPos, &stDir);
		CAM_fn_vTakeCareOfCameraAltitude(_p_stStruct->hCineinfo, &_p_stStruct->stWantedCameraPos);
		MTH3D_M_vSubVector(&_p_stStruct->stCptPos.stMovePos, &_p_stStruct->stWantedCameraPos, &_p_stStruct->stCurrentCameraPos);
	}
}

void CAM_fn_vComputeMovePosWithDynTheta(CAM_tdstUpdateCamera *_p_stStruct)
{
	CAM_fn_vComputeMovePos(_p_stStruct);

	if ((!((_p_stStruct)->hCineinfo->ucVolIAFlags & C_VolIAFlags_InitJustCopiedInCurrent)) &&
		((_p_stStruct)->hCineinfoWork->hSuperObjectTargeted) &&
		(!cNoDynChangeTheta))
	{
		if(CAM_fn_cDynChangeTheta(_p_stStruct, &_p_stStruct->stWantedCameraPos))
			CAM_fn_vComputeMovePos(_p_stStruct);
	}
}

typedef int CAM_tdxCtxKnob0, CAM_tdxCtxKnob1, CAM_tdxCtxKnob2, CAM_tdxCtxKnob3;
void CAM_fn_vComputeSpeed(CAM_tdstUpdateCamera *_p_stStruct)
{

	MTH3D_tdstVector			stTempVector, stTempVector1, stMoveTgt;
	MTH_tdxReal					xNorm, xAngle, xDot, xNorm2;

	if(_p_stStruct->hCineinfoWork->uwIAFlags & C_IAFlags_NoDynSpeed)
	{
		_p_stStruct->stCptPos.xLinearSpeed = _p_stStruct->hCineinfoWork->xLinearSpeed;
		_p_stStruct->stCptPos.xAngularSpeed = _p_stStruct->hCineinfoWork->xAngularSpeed;
		_p_stStruct->stCptPos.xTargetSpeed = _p_stStruct->hCineinfoWork->xTargetSpeed;
		return;
	}

	if ((_p_stStruct->hCineinfoWork->uwIAFlags & C_IAFlags_PositionIsAlreadyComputed) ||
		(_p_stStruct->hCineinfoWork->hSuperObjectTargeted == NULL))
	{
		_p_stStruct->stCptPos.xLinearSpeed = _p_stStruct->hCineinfoWork->xLinearSpeed;
		_p_stStruct->stCptPos.xAngularSpeed = _p_stStruct->hCineinfoWork->xAngularSpeed;
	}
	else
	{

		xNorm = MTH_C_ZERO;
		if(!(_p_stStruct->hCineinfoWork->uwDNMFlags & DNM_CAM_C_NoLinearParsing))
		{
			MTH3D_M_vSubVector(&stTempVector, &_p_stStruct->stWantedCameraPos, &_p_stStruct->stCurrentCameraPos);
			xNorm = MTH3D_M_xNormVector(&stTempVector);

			xNorm = MTH_M_xMul(xNorm, CAM_C_xDynSpeed_LinearFactNorm);
			if(MTH_M_bGreater(xNorm, CAM_C_xDynSpeed_LinearMax))
				xNorm = CAM_C_xDynSpeed_LinearMax;

			CAM_fn_bPersoIsMoving(_p_stStruct, &stTempVector);
			xNorm2 = MTH3D_M_xNormVector(&stTempVector);
			if(MTH_M_bLess(xNorm, MTH_M_xMul(xNorm2, CAM_C_xDynSpeed_LinearMulPerso)))
				xNorm = MTH_M_xMul(xNorm2, CAM_C_xDynSpeed_LinearMulPerso);
		}
		_p_stStruct->stCptPos.xLinearSpeed = xNorm;

		if(!(_p_stStruct->hCineinfoWork->uwDNMFlags & DNM_CAM_C_NoAngularParsing))
		{
			xNorm = MTH_C_ZERO;
			MTH3D_M_vSubVector(&stTempVector, &_p_stStruct->stTargetedPersoPos, &_p_stStruct->stCurrentCameraPos);
			if(!MTH3D_M_bIsNullVector(&stTempVector))
			{
				if (!MTH3D_M_bIsNullVector(&stTempVector))
					MTH3D_M_vNormalizeVector(&stTempVector, &stTempVector);
				MTH3D_M_vSubVector(&stTempVector1, &_p_stStruct->stTargetedPersoPos, &_p_stStruct->stWantedCameraPos);
				if (!MTH3D_M_bIsNullVector(&stTempVector1))
					MTH3D_M_vNormalizeVector(&stTempVector1, &stTempVector1);
				xNorm = CAM_fn_xComputeRotation(&stTempVector, &stTempVector1, NULL);
			}

			xNorm = MTH_M_xMul(xNorm, CAM_C_xDynSpeed_AngularFactNorm);
			if(MTH_M_bGreater(xNorm, CAM_C_xDynSpeed_AngularMax))
				xNorm = CAM_C_xDynSpeed_AngularMax;

			CAM_fn_bPersoIsTurning(_p_stStruct->hCineinfoWork->hSuperObjectTargeted, &xAngle);
			if(MTH_M_bLess(xNorm, MTH_M_xMul(xAngle, CAM_C_xDynSpeed_AngularMulPerso)))
				xNorm = xAngle;

			xAngle = _p_stStruct->hCineinfoWork->xDistMin;
			MTH3D_M_vSubVector(&stTempVector, &_p_stStruct->stTargetedPersoPos, &_p_stStruct->stCurrentCameraPos);
			xAngle = MTH_M_xSub(xAngle, MTH3D_M_xNormVector(&stTempVector));
			if(MTH_M_bGreater(xAngle, MTH_C_ONE))
				xNorm = MTH_M_xDiv(xNorm, xAngle);
			else if(MTH_M_bLess(xAngle, MTH_M_xNeg(MTH_C_ONE)))
				xNorm = MTH_M_xMul(xNorm, MTH_M_xAbs(xAngle));
		}
		_p_stStruct->stCptPos.xAngularSpeed = xNorm;
	}

	if ((_p_stStruct->hCineinfo->ucPerIAFlags & C_IAFlags_TargetIsAlreadyComputed) ||
		(_p_stStruct->hCineinfoWork->hSuperObjectTargeted == NULL))
	{
		_p_stStruct->stCptPos.xTargetSpeed = _p_stStruct->hCineinfoWork->xTargetSpeed;
	}
	else
	{
		xNorm = MTH_C_ZERO;
		MTH3D_M_vNullVector(&stMoveTgt);
		if(!(_p_stStruct->hCineinfoWork->uwDNMFlags & DNM_CAM_C_NoTargetParsing))
		{
			MTH3D_M_vCopyVector(&stTempVector, DNM_M_p_stCPDGetTarget((DNM_tdstParsingDatas *)fn_p_stDynamGetParsingDatas(M_GetMSHandle(_p_stStruct->hSuperObjCamera, Dynam))));
			MTH3D_M_vSubVector(&stTempVector, &_p_stStruct->stCptPos.stTarget, &stTempVector);
			xNorm = MTH3D_M_xNormVector(&stTempVector);
			MTH3D_M_vCopyVector(&stMoveTgt, &stTempVector);
		}

		xNorm = MTH_M_xMul(xNorm, CAM_C_xDynSpeed_TargetFactNorm);
		if(MTH_M_bGreater(xNorm, CAM_C_xDynSpeed_TargetMax))
			xNorm = CAM_C_xDynSpeed_TargetMax;

		CAM_fn_bPersoIsMoving(_p_stStruct, &stTempVector);
		if(MTH_M_bLess(xNorm, MTH_M_xMul(MTH3D_M_xNormVector(&stTempVector), CAM_C_xDynSpeed_TargetMulPerso)))
			xNorm = MTH_M_xMul(MTH3D_M_xNormVector(&stTempVector), CAM_C_xDynSpeed_TargetMulPerso);

		_p_stStruct->stCptPos.xTargetSpeed = xNorm;

		if((!MTH3D_M_bIsNullVector(&stMoveTgt)) && (!MTH3D_M_bIsNullVector(DNM_M_p_stReportGetLastTgtMove(_p_stStruct->p_stReport))))
		{
			xDot = MTH3D_M_xDotProductVector(&stMoveTgt, DNM_M_p_stReportGetLastTgtMove(_p_stStruct->p_stReport));
			if(MTH_M_bLess(xDot, MTH_M_xConstFloatToReal(-0.9f)))
			{
				_p_stStruct->hCineinfoWork->xTargetDecreaseSpeed = MTH_M_xLongToReal(40);
				_p_stStruct->hCineinfoWork->xTargetIncreaseSpeed = MTH_M_xLongToReal(40);
			}
		}
	}
}

static MTH_tdxReal CAM_fn_xOppositeAngle(CAM_tdstUpdateCamera *_p_stStruct)
{

	MTH_tdxReal		xNewAngle;

	xNewAngle = MTH_M_xMul(CAM_C_xBaseCutAngle, MTH_C_2);
	if(MTH_M_bGreater(_p_stStruct->xAngleDeltaAlphaCut, MTH_C_ZERO))
		xNewAngle = MTH_M_xNeg(xNewAngle);
	xNewAngle = MTH_M_xAdd(_p_stStruct->xAngleAlphaCut, xNewAngle);
	xNewAngle = CAM_fn_xSetAngleInInterval02Pi(xNewAngle);
	return xNewAngle;
}

#define CONSTANT_NOVISPROJ		(CAM_g_stCameraConstants.wFlags & 0x0008)
void CAM_fn_vComputeCineinfoCurrentForAPosition(CAM_tdstUpdateCamera *_p_stStruct, MTH3D_tdstVector *_p_stPosition);

void CAM_fn_vConstraintMoveAfterVisFailure(CAM_tdstUpdateCamera *_p_stStruct)
{

	MTH3D_tdstVector			stTempVector, stTempVector1;

	if((_p_stStruct->cTgtPersoIsMovingRelative) && (!(CONSTANT_NOVISPROJ)))
	{
		MTH3D_M_vSubVector(&stTempVector, &_p_stStruct->stWantedCameraPos, &_p_stStruct->hCineinfo->stLastFailedWanted);
		MTH3D_M_vSubVector(&stTempVector1, &_p_stStruct->stTargetedPersoPos, &_p_stStruct->hCineinfo->stLastFailedWanted);

		if((!MTH3D_M_bIsNullVector(&stTempVector)) && (!MTH3D_M_bIsNullVector(&stTempVector1)))
		{
			MTH3D_M_vNormalizeVector(&stTempVector1, &stTempVector1);
			MTH3D_M_vMulScalarVector(&stTempVector, MTH3D_M_xDotProductVector(&stTempVector, &stTempVector1), &stTempVector1);
			MTH3D_M_vAddVector(&stTempVector, &stTempVector, &_p_stStruct->hCineinfo->stLastFailedWanted);
		}
		else
		{
			MTH3D_M_vCopyVector(&stTempVector, &_p_stStruct->hCineinfo->stLastFailedWanted);
		}
	}

	else
	{
		MTH3D_M_vCopyVector(&stTempVector, &_p_stStruct->hCineinfo->stLastFailedWanted);
	}

	MTH3D_M_vSubVector(&_p_stStruct->stCptPos.stMovePos, &stTempVector, &_p_stStruct->stCurrentCameraPos);
	MTH3D_M_vCopyVector(&_p_stStruct->stWantedCameraPos, &stTempVector);
	CAM_fn_vComputeCineinfoCurrentForAPosition(_p_stStruct, &stTempVector);

	MTH3D_M_vCopyVector(&_p_stStruct->hCineinfo->stLastFailedWanted, &_p_stStruct->stWantedCameraPos);
}

char cJustBetterPos = 0;

#define C_IAFlags_NoVisibility                  0x0008
#define C_mode_MUSTSEE                          1
#define C_mode_MUSTGO                           2
#define CAM_e_State_GoToComputedOptimal         1
char CAM_fn_cIsWantedPosIncorrect(CAM_tdstUpdateCamera *_p_stStruct, MTH3D_tdstVector *_p_stWantedPos, char _cMode);
void CAM_fn_vInitCameraStructure(HIE_tdstSuperObject *_hSuperObjPerso, CAM_tdstUpdateCamera *_p_stStruct);

static char CAM_fn_cConstraintMoveCutTestAngle(CAM_tdstUpdateCamera *_p_stStruct, MTH_tdxReal _xAngle)
{

	struct tdstInternalStructurCineinfo_	stTemp;
	CAM_tdstUpdateCamera					stStruct;
	char									cMode;
	MTH3D_tdstVector						stTempVector, stTempVector1;

	stTemp = *_p_stStruct->hCineinfoWork;
	stTemp.xAngleAlpha = _xAngle;
	CAM_fn_vInitCameraStructure(_p_stStruct->hSuperObjCamera, &stStruct);
	stStruct.hCineinfoWork = &stTemp;

	CAM_fn_vComputeReferencePoint(&stStruct);
	CAM_fn_vComputeMovePosWithDynTheta(&stStruct);

	if(cJustBetterPos)
	{
		MTH3D_M_vSubVector(&stTempVector, &_p_stStruct->hCineinfo->stLastFailedOrigin, &_p_stStruct->stCurrentCameraPos);
		MTH3D_M_vSubVector(&stTempVector1, &_p_stStruct->hCineinfo->stLastFailedOrigin, &stStruct.stWantedCameraPos);
		if(MTH_M_bGreater(MTH3D_M_xNormVector(&stTempVector), MTH3D_M_xNormVector(&stTempVector1)))
			return 0;
	}

	cMode = 0;
	if ((_p_stStruct->hCineinfo->ucPerIAFlags & C_PerIAFlags_NoShift) &&
		(_p_stStruct->hCineinfo->eState == CAM_e_State_GoToComputedOptimal))
	{
		cMode = C_mode_MUSTGO|C_mode_MUSTSEE;

		if(_p_stStruct->hCineinfoWork->uwIAFlags & C_IAFlags_NoVisibility)
			cMode &= ~C_mode_MUSTSEE;
	}

	if (CAM_fn_cIsWantedPosIncorrect(&stStruct, &stStruct.stWantedCameraPos, cMode))
		return 0;

	MTH3D_M_vCopyVector(&_p_stStruct->stWantedCameraPos, &stStruct.stWantedCameraPos);
	MTH3D_M_vCopyVector(&_p_stStruct->stCptPos.stMovePos, &stStruct.stCptPos.stMovePos);
	return 1;
}

#define CONSTANT_CUT_COL		(CAM_g_stCameraConstants.wFlags & 0x0001)

static void CAM_fn_vConstraintMoveCutAngles(CAM_tdstUpdateCamera *_p_stStruct)
{

	MTH_tdxReal		xNewAngle;
	char			cWantedSens;

	if(_p_stStruct->hCineinfo->cLastCutAngleSens == -2)
		return;

	if(DNM_M_bReportCollision(_p_stStruct->p_stReport) && (!(CONSTANT_CUT_COL)))
	{
		_p_stStruct->hCineinfo->cLastCutAngleSens = -2;
		return;
	}

	if ((MTH_M_bLess(MTH_M_xMul(_p_stStruct->hCineinfo->xLastDeltaCut, _p_stStruct->xAngleDeltaAlphaCut), MTH_C_ZERO)) &&
		(MTH_M_bLess(MTH_M_xAbs(_p_stStruct->xAngleDeltaAlphaCut), MTH_M_xSub(MTH_C_Pi, CAM_C_xBaseCutAngle))))
	{
		_p_stStruct->hCineinfo->cLastCutAngleSens = 0;
	}

	if(_p_stStruct->hCineinfo->cLastCutAngleSens == 0)
		_p_stStruct->hCineinfo->xLastDeltaCut = _p_stStruct->xAngleDeltaAlphaCut;

	xNewAngle = _p_stStruct->xAngleAlphaCut;
	if((_p_stStruct->hCineinfo->cLastCutAngleSens == -1) && (MTH_M_bGreater(_p_stStruct->xAngleDeltaAlphaCut, MTH_C_ZERO)))
		xNewAngle = CAM_fn_xOppositeAngle(_p_stStruct);
	if((_p_stStruct->hCineinfo->cLastCutAngleSens == 1) && (MTH_M_bLess(_p_stStruct->xAngleDeltaAlphaCut, MTH_C_ZERO)))
		xNewAngle = CAM_fn_xOppositeAngle(_p_stStruct);

	if(CAM_fn_cConstraintMoveCutTestAngle(_p_stStruct, xNewAngle))
	{
		cWantedSens = _p_stStruct->hCineinfo->cLastCutAngleSens;
		if(cWantedSens == 0)
		{

			if(MTH_M_bGreater(_p_stStruct->xAngleDeltaAlphaCut, MTH_C_ZERO))
				_p_stStruct->hCineinfo->cLastCutAngleSens = 1;
			else
				_p_stStruct->hCineinfo->cLastCutAngleSens = -1;
		}
		return;
	}

	if(_p_stStruct->hCineinfo->cLastCutAngleSens == 0)
	{
		if(MTH_M_bGreaterEqual(MTH_M_xAbs(_p_stStruct->xAngleDeltaAlphaCut), MTH_M_xSub(MTH_C_Pi, CAM_C_xBaseCutAngle)))
		{
			xNewAngle = CAM_fn_xOppositeAngle(_p_stStruct);
			if(CAM_fn_cConstraintMoveCutTestAngle(_p_stStruct, xNewAngle))
			{

				if(MTH_M_bGreater(_p_stStruct->xAngleDeltaAlphaCut, MTH_C_ZERO))
					_p_stStruct->hCineinfo->cLastCutAngleSens = -1;
				else
					_p_stStruct->hCineinfo->cLastCutAngleSens = 1;
				return;
			}
		}
	}

	_p_stStruct->hCineinfo->cLastCutAngleSens = -2;
}

#define C_IAFlags_NoParseCutAngle			0x0004

char CAM_fn_cCanResetConstraintMoveVisFailure(CAM_tdstUpdateCamera *_p_stStruct, MTH3D_tdstVector *_p_stWanted);

void CAM_fn_vConstraintNoMove(CAM_tdstUpdateCamera *_p_stStruct, MTH3D_tdstVector *_p_stWanted)
{
}

#define C_PerIAFlags_ConstraintMoveVisFailure	0x08

void CAM_fn_vConstraintMove(CAM_tdstUpdateCamera *_p_stStruct)
{

	if ((_p_stStruct->hCineinfo->ucVolIAFlags & C_VolIAFlags_InitJustCopiedInCurrent) ||
		(_p_stStruct->hCineinfoWork->uwIAFlags & C_IAFlags_PositionIsAlreadyComputed) ||
		(_p_stStruct->hCineinfoWork->hSuperObjectTargeted == NULL))
		return;

	if(CAM_fn_cCanResetConstraintMoveVisFailure(_p_stStruct, &_p_stStruct->stWantedCameraPos))
	{
		_p_stStruct->hCineinfo->ucPerIAFlags &= ~C_PerIAFlags_ConstraintMoveVisFailure;
	}

	if(_p_stStruct->hCineinfo->ucPerIAFlags & C_PerIAFlags_ConstraintMoveVisFailure)
	{
		CAM_fn_vConstraintMoveAfterVisFailure(_p_stStruct);
		return;
	}

	if ((_p_stStruct->cHasCutAlpha) &&
		(!(_p_stStruct->hCineinfoWork->uwIAFlags & C_IAFlags_NoParseCutAngle)))
	{
		CAM_fn_vConstraintMoveCutAngles(_p_stStruct);
	}
	else
	{
		_p_stStruct->hCineinfo->cLastCutAngleSens = 0;
	}

	if (_p_stStruct->hCineinfo->eState == CAM_e_State_GoToComputedOptimal)
	{
		CAM_fn_vConstraintNoMove(_p_stStruct, &_p_stStruct->stWantedCameraPos);
	}
}

#define CONSTANT_NORMALIFJOIN                   (CAM_g_stCameraConstants.wFlags & 0x0010)
#define CAM_C_xMinLinearSpeedCamera             (CAM_g_stCameraConstants.xMinLinearSpeedCamera)
#define CAM_C_xEpsilonForEqualVectors           (CAM_g_stCameraConstants.xEpsilonForEqualVectors)
#define CAM_C_xTickTimeFindBetterPosIfNoMove \
	(MTH_M_xMul(MTH_M_xMul(MTH_M_xLongToReal(CAM_g_stCameraConstants.xTickFindBetterPosIfNoMove),MTH_M_xConstFloatToReal(1000)),GAM_g_xFrameLength))
#define DNM_CAM_C_NoObstacle                    0x0040
#define C_IAFlags_NoDynChangeTheta              0x0020
#define C_IAFlags_RefAxisIsAlreadyComputed      0x4000
#define CAM_e_State_GoToOptimal                 0
#define CAM_e_State_FailureVisibility           2
#define CAM_e_TempState_GhostMode               4
#define CAM_e_TempState_ApexMode                5
extern char AI_CAM_gs_cCanTestStatic;
#define cCanTestStatic                          AI_CAM_gs_cCanTestStatic
extern unsigned char GAM_g_ucIsEdInGhostMode;
void CAM_fn_vTestBadDistance(CAM_tdstUpdateCamera *_p_stStruct);
void CAM_fn_vTestBadVisibility(CAM_tdstUpdateCamera *_p_stStruct);
void CAM_fn_vTestOppositeCollision(CAM_tdstUpdateCamera *_p_stStruct);
void CAM_fn_vComputeFailureVisibility(CAM_tdstUpdateCamera *_p_stStruct);
void CAM_fn_vCheckForABetterPos(CAM_tdstUpdateCamera *_p_stStruct);

void CAM_fn_vUpdateGeneralCamera(CAM_tdstUpdateCamera *_p_stStruct)
{

	MTH3D_tdstVector	stMoveVector;

recom:

	cCanTestStatic = 1;

	CAM_fn_vTakeCareOfFather(_p_stStruct);

	MTH3D_M_vCopyVector(&stMoveVector, DNM_M_p_stReportGetLastMove(_p_stStruct->p_stReport));
	if(MTH_M_bLess(MTH3D_M_xNormVector(&stMoveVector), CAM_C_xMinLinearSpeedCamera))
	{
		_p_stStruct->hCineinfo->ucVolIAFlags |= C_VolIAFlags_CameraNotMove;

		if(MTH_M_bLess(_p_stStruct->hCineinfo->xCounter4,CAM_C_xTickTimeFindBetterPosIfNoMove))
		{
			_p_stStruct->hCineinfo->xCounter4 = MTH_M_xAdd(_p_stStruct->hCineinfo->xCounter4,MTH_M_xLongToReal(GAM_g_ulUsefulDeltaTime));
		}
	}
	else
	{
		_p_stStruct->hCineinfo->xCounter4 = MTH_C_ZERO;
	}

	if (GAM_g_ucIsEdInGhostMode)
	{
		CAM_fn_vInitCompleteCineinfo(_p_stStruct->hCineinfo);
		*_p_stStruct->hCineinfo->hWork = *_p_stStruct->hCineinfo->hCurrent;
		_p_stStruct->hCineinfoWork->xDistMin = MTH_C_2;
		_p_stStruct->hCineinfoWork->xDistMax = MTH_C_2;
		_p_stStruct->hCineinfoWork->xAngleAlpha = MTH_C_ZERO;
		_p_stStruct->hCineinfoWork->xAngleShiftAlpha = MTH_C_ZERO;
		_p_stStruct->hCineinfoWork->xAngleTheta = MTH_C_ZERO;
		_p_stStruct->hCineinfoWork->xAngleShiftTheta = MTH_C_ZERO;
		_p_stStruct->hCineinfoWork->uwDNMFlags = DNM_CAM_C_NoLinearParsing|DNM_CAM_C_NoAngularParsing|DNM_CAM_C_NoTargetParsing|DNM_CAM_C_NoObstacle;
		_p_stStruct->hCineinfoWork->uwIAFlags = C_IAFlags_NoVisibility|C_IAFlags_NoAverageMoveTgtPerso;
		_p_stStruct->hCineinfo->ucPerIAFlags = 0;
		_p_stStruct->hCineinfo->ucVolIAFlags = 0;
		_p_stStruct->hCineinfo->eState = CAM_e_TempState_GhostMode;
	}

	if ((_p_stStruct->hCineinfoWork->hSuperObjectTargeted) &&
		(_p_stStruct->hCineinfoWork->hSecondSuperObjectTargeted))
	{
		_p_stStruct->hCineinfo->ucPerIAFlags &= ~C_PerIAFlags_ConstraintMoveVisFailure;
		_p_stStruct->hCineinfoWork->uwIAFlags |= C_IAFlags_NoVisibility;
		_p_stStruct->hCineinfo->eState = CAM_e_TempState_ApexMode;
	}

	cRefAxisIsAlreadyComputed = _p_stStruct->hCineinfoWork->uwIAFlags & C_IAFlags_RefAxisIsAlreadyComputed ? 1 : 0;
	cNoDynChangeTheta = _p_stStruct->hCineinfoWork->uwIAFlags & C_IAFlags_NoDynChangeTheta ? 1 : 0;

	CAM_fn_vComputeReferencePoint(_p_stStruct);
	CAM_fn_vComputeTarget(_p_stStruct);
	CAM_fn_vComputeRealWantedPos(_p_stStruct);

	CAM_fn_vComputeMovePos(_p_stStruct);

	if ((!((_p_stStruct)->hCineinfo->ucVolIAFlags & C_VolIAFlags_InitJustCopiedInCurrent)) &&
		((_p_stStruct)->hCineinfoWork->hSuperObjectTargeted) &&
		(!cNoDynChangeTheta) &&
		(!(_p_stStruct->hCineinfo->ucPerIAFlags & C_PerIAFlags_ConstraintMoveVisFailure)) &&
		(!(_p_stStruct->hCineinfoWork->uwIAFlags & C_IAFlags_PositionIsAlreadyComputed)))
	{
		if(CAM_fn_cDynChangeTheta(_p_stStruct, &_p_stStruct->stWantedCameraPos))
			CAM_fn_vComputeMovePos(_p_stStruct);
	}

	if ((_p_stStruct->hCineinfo->ucPerIAFlags & C_PerIAFlags_NoShift) ||
		(_p_stStruct->hCineinfoWork->uwIAFlags & C_IAFlags_NoShiftUntilPosReached))
	{
		if ((_p_stStruct->hCineinfoWork->uwIAFlags & C_IAFlags_TargetIsAlreadyComputed) ||
			(_p_stStruct->hCineinfoWork->uwIAFlags & C_IAFlags_PositionIsAlreadyComputed) ||
			(_p_stStruct->hCineinfo->ucVolIAFlags & C_VolIAFlags_InitJustCopiedInCurrent) ||
			CAM_fn_bEqualVectorWithEpsilon(&_p_stStruct->stCurrentCameraPos,
												&_p_stStruct->stWantedCameraPos,
												CAM_C_xEpsilonForEqualVectors))
		{
			_p_stStruct->hCineinfoCurrent->uwIAFlags &= ~C_IAFlags_NoShiftUntilPosReached;
			_p_stStruct->hCineinfoWork->uwIAFlags &= ~C_IAFlags_NoShiftUntilPosReached;
			_p_stStruct->hCineinfo->ucPerIAFlags &= ~C_PerIAFlags_NoShift;
			_p_stStruct->hCineinfo->cLastCutAngleSens = 0;
		}
	}

	if ((CONSTANT_NORMALIFJOIN) &&
		(_p_stStruct->hCineinfo->eState == CAM_e_State_GoToComputedOptimal) &&
		(_p_stStruct->cTgtPersoIsMovingRelative))
	{
		if (CAM_fn_bEqualVectorWithEpsilon(
				&_p_stStruct->stCurrentCameraPos,
				&_p_stStruct->stWantedCameraPos,
				CAM_C_xEpsilonForEqualVectors))
		{
			if (_p_stStruct->hCineinfo->ucPerIAFlags & C_PerIAFlags_VisibilityCopied)
				CAM_fn_vSetCineinfoCurrentFromVisibility(_p_stStruct->hCineinfo);

			_p_stStruct->hCineinfo->ucPerIAFlags &= ~C_PerIAFlags_ConstraintMoveVisFailure;
			_p_stStruct->hCineinfo->ucVolIAFlags &= ~C_VolIAFlags_CurrentAlreadyCopiedInWork;
			_p_stStruct->hCineinfo->eState = CAM_e_State_GoToOptimal;

			goto recom;
		}
	}

	switch(_p_stStruct->hCineinfo->eState)
	{
		case CAM_e_State_GoToOptimal:
		case CAM_e_State_GoToComputedOptimal:

			CAM_fn_vTestBadVisibility(_p_stStruct);
			CAM_fn_vTestBadDistance(_p_stStruct);
			CAM_fn_vTestOppositeCollision(_p_stStruct);
			break;
		default:
		  break;
	}

	switch(_p_stStruct->hCineinfo->eState)
	{
		case CAM_e_State_GoToOptimal:

			CAM_fn_vConstraintMove(_p_stStruct);
			break;

		case CAM_e_State_GoToComputedOptimal:

			CAM_fn_vCheckForABetterPos(_p_stStruct);
			CAM_fn_vConstraintMove(_p_stStruct);
			break;

		case CAM_e_State_FailureVisibility:
			CAM_fn_vComputeFailureVisibility(_p_stStruct);
			break;

		case CAM_e_TempState_ApexMode:

			_p_stStruct->hCineinfo->eState = CAM_e_State_GoToOptimal;
			break;

		case CAM_e_TempState_GhostMode:

			_p_stStruct->hCineinfo->eState = CAM_e_State_GoToOptimal;
			break;

		default:
			break;
	}

	CAM_fn_vComputeSpeed(_p_stStruct);

	if ((_p_stStruct->hCineinfoWork->uwIAFlags & C_IAFlags_TargetIsAlreadyComputed) ||
		(_p_stStruct->hCineinfoWork->uwIAFlags & C_IAFlags_PositionIsAlreadyComputed) ||
		(_p_stStruct->hCineinfo->ucVolIAFlags & C_VolIAFlags_InitJustCopiedInCurrent) ||
		(_p_stStruct->hCineinfo->eState == CAM_e_State_GoToOptimal))
	{
		MTH3D_M_vNullVector(&_p_stStruct->hCineinfo->stLastFailedOrigin);
		MTH3D_M_vNullVector(&_p_stStruct->hCineinfo->stLastFailedWanted);
		_p_stStruct->hCineinfo->xCounter1 = MTH_C_ZERO;
		_p_stStruct->hCineinfo->xCounter3 = MTH_C_ZERO;
		_p_stStruct->hCineinfo->ucPerIAFlags &= ~C_PerIAFlags_ConstraintMoveVisFailure;
	}

	cJustBetterPos = 0;
}
