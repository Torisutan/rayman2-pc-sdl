#include "ACP/ACP_types.h"
#include "DNM/DNM_types.h"
#include "POS/POS_types.h"
#include <math.h>
#include "MTH/MTH_flt.h"
#include "MTH/MTH3D.h"
#include "GAM/GAM_types.h"
#include "MEC/MECTools.h"
#include "MEC/MECOpt.h"

#define MEC_C_xFrameNominal   (MTH_C_ONE / 60.0f)   /* 0x0049C74C */
#define MEC_C_xFrameEpsilon   1.0e-3f               /* 0x0049C740 */
#define MEC_C_dDecayExp       60.0                  /* 0x0049C750 */
#define MEC_C_xInertiaC0      0.0071428571f         /* 0x0049C7E0 */
#define MEC_C_xInertiaSmall   1.0e-6f               /* 0x0049C7DC */
#define MEC_C_xInertiaBig     1.0e-4f               /* 0x0049C7E8 */
#define MEC_C_xFpsLo          60.0f                 /* 0x0049C7D8 */
#define MEC_C_xFpsHi          200.0f                /* 0x0049C7E4 */

/* 0x004370A0 */
static MTH_tdxReal s_xConvertScaleFrameComp(MTH_tdxReal xValue, ACP_tdxBool _bFrameComp)
{
	MTH_tdxReal xResult;

	if (xValue >= MTH_C_ONE)
		xResult = xValue;
	else if (xValue > MTH_C_ZERO)
	{
		MTH_tdxReal xTmp = MTH_M_xSub(MTH_C_ONE, xValue);
		xTmp = MTH_M_xMul(xTmp, xTmp);
		xTmp = MTH_M_xMul(xTmp, xTmp);
		xResult = MTH_M_xSub(MTH_C_ONE, xTmp);
	}
	else
		xResult = MTH_C_ZERO;

	if (!_bFrameComp)
		return xResult;

	if (MTH_M_xAbs(MTH_M_xSub(g_xCachedFrameLength, MEC_C_xFrameNominal)) <= MEC_C_xFrameEpsilon)
		return xResult;

	return pow((double)xResult, (double)g_xCachedFrameLength * MEC_C_dDecayExp);
}

/* 0x0043DDA0 */
static void MEC_fn_vInertiaCoeff(DNM_tdstDynamics *p_stDynamic, MTH_tdxReal xDT,
	MTH_tdxReal xInertiaX, MTH_tdxReal xInertiaY, MTH_tdxReal xInertiaZ,
	POS_tdxHandleToPosition hMatrix, POS_tdxHandleToPosition hInvMatrix)
{
	POS_tdxHandleToPosition hCurrentMatrix, hPreviousMatrix;
	MTH3D_tdstVector *p_stCurrentPosition, *p_stPreviousPosition;
	MTH3D_tdstVector *p_stSpeed;
	MTH3D_tdstVector stMove, stSpeedMove, stBlend;
	MTH_tdxReal xFps, xEpsilon;

	hCurrentMatrix = DNM_M_p_stDynamicsGetCurrentMatrix(p_stDynamic);
	hPreviousMatrix = DNM_M_p_stDynamicsGetPreviousMatrix(p_stDynamic);
	p_stCurrentPosition = POS_fn_p_stGetTranslationVector(hCurrentMatrix);
	p_stPreviousPosition = POS_fn_p_stGetTranslationVector(hPreviousMatrix);

	MTH3D_M_vSubVector(&stMove, p_stCurrentPosition, p_stPreviousPosition);

	MEC_fn_vInvertMatrix(p_stDynamic, hInvMatrix, hMatrix);

	p_stSpeed = DNM_M_pDynamicsGetPreviousSpeed(p_stDynamic);
	MTH3D_M_vMulScalarVector(&stSpeedMove, xDT, p_stSpeed);
	if (!DNM_M_bDynamicsIsMechanicChange(p_stDynamic))
		MTH3D_M_vSubVector(&stSpeedMove, &stSpeedMove, DNM_M_pDynamicsGetInertiaTranslation(p_stDynamic));

	POS_fn_vRotateVector(&stMove, hInvMatrix, &stMove);
	POS_fn_vRotateVector(&stSpeedMove, hInvMatrix, &stSpeedMove);

	MTH3D_M_vSetXofVector(&stBlend, MTH_M_xAdd(MTH_M_xMul(MTH_M_xSub(MTH3D_M_xGetXofVector(&stSpeedMove), MTH3D_M_xGetXofVector(&stMove)), xInertiaX), MTH3D_M_xGetXofVector(&stMove)));
	MTH3D_M_vSetYofVector(&stBlend, MTH_M_xAdd(MTH_M_xMul(MTH_M_xSub(MTH3D_M_xGetYofVector(&stSpeedMove), MTH3D_M_xGetYofVector(&stMove)), xInertiaY), MTH3D_M_xGetYofVector(&stMove)));
	MTH3D_M_vSetZofVector(&stBlend, MTH_M_xAdd(MTH_M_xMul(MTH_M_xSub(MTH3D_M_xGetZofVector(&stSpeedMove), MTH3D_M_xGetZofVector(&stMove)), xInertiaZ), MTH3D_M_xGetZofVector(&stMove)));

	xFps = MTH_M_xDiv(MTH_C_ONE, xDT);

	POS_fn_vRotateVector(&stBlend, hMatrix, &stBlend);

	xEpsilon = MTH_M_xAdd(
		MTH_M_xMul(MTH_M_xMul(MTH_M_xSub(xFps, MEC_C_xFpsLo), MEC_C_xInertiaSmall), MEC_C_xInertiaC0),
		MTH_M_xMul(MTH_M_xMul(MTH_M_xSub(MEC_C_xFpsHi, xFps), MEC_C_xInertiaBig), MEC_C_xInertiaC0));
	MEC_fn_vRoundVector(&stBlend, xEpsilon);

	MTH3D_M_vAddVector(p_stCurrentPosition, p_stPreviousPosition, &stBlend);
}

/* 0x0043DF70 */
void __cdecl MEC_fn_vInertia(DNM_tdstDynamics *p_stDynamic, MTH_tdxReal xDT,
	POS_tdxHandleToPosition hMatrix, POS_tdxHandleToPosition hInvMatrix)
{
	if (DNM_M_bDynamicsIsInertia(p_stDynamic))
	{
		MTH_tdxReal xInertiaX = s_xConvertScaleFrameComp(DNM_M_xDynamicsGetInertiaFactorX(p_stDynamic), 1);
		MTH_tdxReal xInertiaY = s_xConvertScaleFrameComp(DNM_M_xDynamicsGetInertiaFactorY(p_stDynamic), 1);
		MTH_tdxReal xInertiaZ = s_xConvertScaleFrameComp(DNM_M_xDynamicsGetInertiaFactorZ(p_stDynamic), 1);
		MEC_fn_vInertiaCoeff(p_stDynamic, xDT, xInertiaX, xInertiaY, xInertiaZ, hMatrix, hInvMatrix);
	}

	if (DNM_M_bDynamicsIsOnGround(p_stDynamic) && DNM_M_bDynamicsIsSlidingGround(p_stDynamic))
	{
		MTH_tdxReal xSlide = s_xConvertScaleFrameComp(DNM_M_xDynamicsGetPreviousSlide(p_stDynamic), 1);
		MTH_tdxReal xSlideFactorX = MTH_M_xMul(s_xConvertScaleFrameComp(DNM_M_xDynamicsGetSlideFactorX(p_stDynamic), 0), xSlide);
		MTH_tdxReal xSlideFactorY = MTH_M_xMul(s_xConvertScaleFrameComp(DNM_M_xDynamicsGetSlideFactorY(p_stDynamic), 0), xSlide);
		MTH_tdxReal xSlideFactorZ = MTH_M_xMul(s_xConvertScaleFrameComp(DNM_M_xDynamicsGetSlideFactorZ(p_stDynamic), 0), xSlide);
		MEC_fn_vInertiaCoeff(p_stDynamic, xDT, xSlideFactorX, xSlideFactorY, xSlideFactorZ, hMatrix, hInvMatrix);
	}
}

/* 0x0043E0A0 */
void __cdecl MEC_fn_vAddTiltSpeed ( DNM_tdstDynamics *p_stDynamic, MTH_tdxReal xDt )
{
	MTH3D_tdstVector *p_stPreviousI, *p_stPreviousJ, *p_stPreviousK;
	MTH3D_tdstVector *p_stCurrentI, *p_stCurrentJ, *p_stCurrentK;
	MTH3D_tdstVector stInitialTranslation, stVector;
	MTH3D_tdstVector stOriginTranslation;
	MTH3D_tdstVector *p_stCurrentTranslation;
	MTH_tdxReal xDotProd, xAngle;
	MTH_tdxReal xTiltAngle, xInitTiltAngle;
	MTH_tdxReal xInertia;

	POS_tdxHandleToPosition hCurrentMatrix;
	POS_tdxHandleToPosition hPreviousMatrix;

	hCurrentMatrix = DNM_M_p_stDynamicsGetCurrentMatrix (p_stDynamic);
	hPreviousMatrix = DNM_M_p_stDynamicsGetPreviousMatrix (p_stDynamic);

	xInertia = DNM_M_xDynamicsGetTiltInertia ( p_stDynamic );

	POS_fn_vGetRotationVector ( hPreviousMatrix, &p_stPreviousI, &p_stPreviousJ, &p_stPreviousK);
	POS_fn_vGetRotationVector ( hCurrentMatrix, &p_stCurrentI, &p_stCurrentJ, &p_stCurrentK);

	xDotProd = MTH3D_M_xDotProductVector ( p_stPreviousI, p_stCurrentJ );
	xAngle = MTH_M_xMul ( MTH_M_xMul ( xDotProd, DNM_M_xDynamicsGetTiltIntensity(p_stDynamic)) , MTH3D_M_xNormVector(DNM_M_pDynamicsGetPreviousSpeed(p_stDynamic)) );
	xAngle = MTH_M_xDiv ( xAngle, MTH_M_xLongToReal(10) );

	xTiltAngle = DNM_M_xDynamicsGetTiltAngle(p_stDynamic);
	xInitTiltAngle = xTiltAngle;

	xTiltAngle = MTH_M_xAdd ( MTH_M_xMul(xTiltAngle,xInertia), MTH_M_xMul(xAngle,MTH_M_xSub(MTH_C_ONE,xInertia)) );
	xAngle = MTH_M_xSub ( xTiltAngle, xInitTiltAngle );

	if (MTH_M_bDifferent ( xAngle, MTH_C_ZERO ) )
	{
		DNM_M_vDynamicsSetTiltAngle ( p_stDynamic, xTiltAngle );

		MTH3D_M_vSetBaseJVector ( &stVector );

		MTH3D_M_vMulScalarVector ( &stOriginTranslation, DNM_M_xDynamicsGetTiltOrigin(p_stDynamic), p_stCurrentK );

		p_stCurrentTranslation = POS_fn_p_stGetTranslationVector (hCurrentMatrix);
		MTH3D_M_vCopyVector (&stInitialTranslation,p_stCurrentTranslation);

		MTH3D_M_vCopyVector (p_stCurrentTranslation,&stOriginTranslation);
		POS_fn_vRotatePositionAroundAxis (hCurrentMatrix, &stVector, xAngle);
		MTH3D_M_vSubVector (&stVector, p_stCurrentTranslation, &stOriginTranslation);

		MTH3D_M_vAddVector (p_stCurrentTranslation, &stInitialTranslation, &stVector);
	}
}

void __cdecl MEC_fn_vImposeSpeed
(
	DNM_tdstDynamics *p_stDynamic,
	MTH_tdxReal xDT,
	POS_tdxHandleToPosition hMatrix,
	POS_tdxHandleToPosition hInvMatrix
)
{
	POS_tdxHandleToPosition hPreviousMatrix, hCurrentMatrix;
	MTH3D_tdstVector *p_stPreviousPosition, *p_stCurrentPosition;
	MTH3D_tdstVector stTranslation;
	MTH3D_tdstVector* pstImposeSpeed;
	MTH3D_tdstVector stImposeMove;

	hCurrentMatrix = DNM_M_p_stDynamicsGetCurrentMatrix (p_stDynamic);
	hPreviousMatrix = DNM_M_p_stDynamicsGetPreviousMatrix (p_stDynamic);

	pstImposeSpeed = DNM_M_pDynamicsGetImposeSpeed ( p_stDynamic );
	MTH3D_M_vMulScalarVector ( &stImposeMove, xDT, pstImposeSpeed );

	p_stPreviousPosition = POS_fn_p_stGetTranslationVector ( hPreviousMatrix );
	p_stCurrentPosition = POS_fn_p_stGetTranslationVector ( hCurrentMatrix );
	MTH3D_M_vSubVector ( &stTranslation, p_stCurrentPosition, p_stPreviousPosition);

	if (DNM_M_bDynamicsIsRelativeImposeSpeed(p_stDynamic) && !MTH3D_M_bIsNullVector(&stTranslation))
	{
		MEC_fn_vInvertMatrix (p_stDynamic,hInvMatrix,hMatrix);
		POS_fn_vRotateVector ( &stTranslation, hInvMatrix, &stTranslation );
	}

	if (DNM_M_bDynamicsIsImposeSpeedX(p_stDynamic)) MTH3D_M_vSetXofVector (&stTranslation,MTH3D_M_xGetXofVector(&stImposeMove));
	if (DNM_M_bDynamicsIsImposeSpeedY(p_stDynamic)) MTH3D_M_vSetYofVector (&stTranslation,MTH3D_M_xGetYofVector(&stImposeMove));
	if (DNM_M_bDynamicsIsImposeSpeedZ(p_stDynamic)) MTH3D_M_vSetZofVector (&stTranslation,MTH3D_M_xGetZofVector(&stImposeMove));

	if (DNM_M_bDynamicsIsRelativeImposeSpeed(p_stDynamic) && !MTH3D_M_bIsNullVector(&stTranslation)) POS_fn_vRotateVector ( &stTranslation, hMatrix, &stTranslation );

	MTH3D_M_vAddVector ( p_stCurrentPosition, p_stPreviousPosition, &stTranslation );
}

void __cdecl MEC_fn_vAddSpeed
(
	DNM_tdstDynamics *p_stDynamic,
	MTH_tdxReal xDT,
	POS_tdxHandleToPosition hMatrix,
	POS_tdxHandleToPosition hInvMatrix
)
{
	POS_tdxHandleToPosition hPreviousMatrix, hCurrentMatrix;
	MTH3D_tdstVector *p_stPreviousPosition, *p_stCurrentPosition;
	MTH3D_tdstVector stTranslation;
	MTH3D_tdstVector* pstAddSpeed;
	MTH3D_tdstVector stAddMove;

	hCurrentMatrix = DNM_M_p_stDynamicsGetCurrentMatrix (p_stDynamic);
	hPreviousMatrix =  DNM_M_p_stDynamicsGetPreviousMatrix (p_stDynamic);

	pstAddSpeed = DNM_M_pDynamicsGetAddSpeed ( p_stDynamic );
	MTH3D_M_vMulScalarVector ( &stAddMove, xDT, pstAddSpeed );

	p_stPreviousPosition = POS_fn_p_stGetTranslationVector ( hPreviousMatrix );
	p_stCurrentPosition = POS_fn_p_stGetTranslationVector ( hCurrentMatrix );
	MTH3D_M_vSubVector ( &stTranslation, p_stCurrentPosition, p_stPreviousPosition);

	if (DNM_M_bDynamicsIsRelativeAddSpeed(p_stDynamic) && !MTH3D_M_bIsNullVector(&stTranslation))
	{
		MEC_fn_vInvertMatrix (p_stDynamic,hInvMatrix,hMatrix);
		POS_fn_vRotateVector ( &stTranslation, hInvMatrix, &stTranslation );
	}

	if (DNM_M_bDynamicsIsAddSpeedX(p_stDynamic)) MTH3D_M_vSetXofVector (&stTranslation,MTH_M_xAdd(MTH3D_M_xGetXofVector(&stTranslation),MTH3D_M_xGetXofVector(&stAddMove)));
	if (DNM_M_bDynamicsIsAddSpeedY(p_stDynamic)) MTH3D_M_vSetYofVector (&stTranslation,MTH_M_xAdd(MTH3D_M_xGetYofVector(&stTranslation),MTH3D_M_xGetYofVector(&stAddMove)));
	if (DNM_M_bDynamicsIsAddSpeedZ(p_stDynamic)) MTH3D_M_vSetZofVector (&stTranslation,MTH_M_xAdd(MTH3D_M_xGetZofVector(&stTranslation),MTH3D_M_xGetZofVector(&stAddMove)));

	if (DNM_M_bDynamicsIsRelativeAddSpeed(p_stDynamic) && !MTH3D_M_bIsNullVector(&stTranslation)) POS_fn_vRotateVector ( &stTranslation, hMatrix, &stTranslation );

	MTH3D_M_vAddVector ( p_stCurrentPosition, p_stPreviousPosition, &stTranslation );
}

void __cdecl MEC_fn_vProposeSpeed(DNM_tdstDynamics *p_stDynamic, MTH_tdxReal xDT,
	POS_tdxHandleToPosition hMatrix, POS_tdxHandleToPosition hInvMatrix)
{
	POS_tdxHandleToPosition hPreviousMatrix, hCurrentMatrix;
	MTH3D_tdstVector *p_stPreviousPosition, *p_stCurrentPosition;
	MTH3D_tdstVector stTranslation;
	MTH3D_tdstVector* pstProposeSpeed;
	MTH3D_tdstVector stProposeMove;

	hCurrentMatrix = DNM_M_p_stDynamicsGetCurrentMatrix (p_stDynamic);
	hPreviousMatrix = DNM_M_p_stDynamicsGetPreviousMatrix (p_stDynamic);

	pstProposeSpeed = DNM_M_pDynamicsGetProposeSpeed ( p_stDynamic );

	MTH3D_M_vMulScalarVector ( &stProposeMove, xDT, pstProposeSpeed );

	p_stPreviousPosition = POS_fn_p_stGetTranslationVector ( hPreviousMatrix );
	p_stCurrentPosition = POS_fn_p_stGetTranslationVector ( hCurrentMatrix );

	MTH3D_M_vSubVector ( &stTranslation, p_stCurrentPosition, p_stPreviousPosition);

	if (DNM_M_bDynamicsIsRelativeProposeSpeed(p_stDynamic) && !MTH3D_M_bIsNullVector(&stTranslation))
	{
		MEC_fn_vInvertMatrix (p_stDynamic,hInvMatrix,hMatrix);
		POS_fn_vRotateVector ( &stTranslation, hInvMatrix, &stTranslation );
	}

	if (DNM_M_bDynamicsIsProposeSpeedX(p_stDynamic)) MTH3D_M_vSetXofVector (&stTranslation, MTH3D_M_xGetXofVector(&stProposeMove));
	if (DNM_M_bDynamicsIsProposeSpeedY(p_stDynamic)) MTH3D_M_vSetYofVector (&stTranslation, MTH3D_M_xGetYofVector(&stProposeMove));
	if (DNM_M_bDynamicsIsProposeSpeedZ(p_stDynamic)) MTH3D_M_vSetZofVector (&stTranslation, MTH3D_M_xGetZofVector(&stProposeMove));

	if (DNM_M_bDynamicsIsRelativeProposeSpeed(p_stDynamic) && !MTH3D_M_bIsNullVector(&stTranslation))
		POS_fn_vRotateVector ( &stTranslation, hMatrix, &stTranslation );

	MTH3D_M_vAddVector ( p_stCurrentPosition, p_stPreviousPosition, &stTranslation );
}

void __cdecl MEC_fn_vGravitySpeed(DNM_tdstDynamics *p_stDynamic, MTH_tdxReal xDT)
{
	MTH_tdxReal xGravity = DNM_M_xDynamicsGetGravityFactor(p_stDynamic);
	MTH3D_tdstVector *p_stPreviousTranslation, *p_stCurrentTranslation;
	MTH3D_tdstVector *p_stSpeed;
	POS_tdxHandleToPosition hPreviousMatrix, hCurrentMatrix;
	MTH_tdxReal xZ;

	if (DNM_M_bDynamicsIsNoGravity(p_stDynamic))
		return;

	hCurrentMatrix = DNM_M_p_stDynamicsGetCurrentMatrix (p_stDynamic);
	hPreviousMatrix = DNM_M_p_stDynamicsGetPreviousMatrix (p_stDynamic);

	p_stPreviousTranslation = POS_fn_p_stGetTranslationVector ( hPreviousMatrix );
	p_stCurrentTranslation = POS_fn_p_stGetTranslationVector ( hCurrentMatrix );

	p_stSpeed = DNM_M_pDynamicsGetPreviousSpeed ( p_stDynamic );

	/* 0x0043DF40 */
	xGravity = MTH_M_xMul ( MTH_M_xMul ( xGravity, MTH_C_MinusONE ), xDT );
	xGravity = MTH_M_xAdd ( xGravity, MTH3D_M_xGetZofVector(p_stSpeed) );
	xGravity = MTH_M_xMul ( xGravity, xDT );

	xZ = MTH_M_xAdd (MTH3D_M_xGetZofVector(p_stPreviousTranslation), xGravity);

	MTH3D_M_vSetZofVector ( p_stCurrentTranslation, xZ );
}

void __cdecl MEC_fn_vAddStream (
	DNM_tdstDynamics *p_stDynamic,
	MTH_tdxReal xDT,
	ACP_tdxBool bSlide
)
{
	POS_tdxHandleToPosition hPreviousMatrix, hCurrentMatrix;
	MTH3D_tdstVector *p_stCurrentPosition;
	MTH3D_tdstVector stTranslation;

	hCurrentMatrix = DNM_M_p_stDynamicsGetCurrentMatrix (p_stDynamic);
	hPreviousMatrix = DNM_M_p_stDynamicsGetPreviousMatrix (p_stDynamic);

	MEC_fn_vComputeStream ( &stTranslation, p_stDynamic, xDT, bSlide);

	p_stCurrentPosition = POS_fn_p_stGetTranslationVector ( hCurrentMatrix );
	MTH3D_M_vAddVector ( p_stCurrentPosition, p_stCurrentPosition, &stTranslation );
	(void)hPreviousMatrix;
}

void __cdecl MEC_fn_vImposeRotation	( DNM_tdstDynamics *p_stDynamic )
{
	MTH3D_tdstMatrix* p_stImposeRotationMatrix;
	POS_tdxHandleToPosition hCurrentMatrix;
	MTH3D_tdstVector stI, stJ, stK;

	hCurrentMatrix = DNM_M_p_stDynamicsGetCurrentMatrix (p_stDynamic);
	p_stImposeRotationMatrix = DNM_M_p_stDynamicsGetImposeRotationMatrix (p_stDynamic);

	MTH3D_M_vGetVectorsInMatrix ( &stI, &stJ, &stK, p_stImposeRotationMatrix );
	POS_fn_vSetRotationMatrix ( hCurrentMatrix, &stI, &stJ, &stK );
}

void __cdecl MEC_fn_vAdditionnalTranslation ( DNM_tdstDynamics *p_stDynamic )
{
	POS_tdxHandleToPosition hCurrentMatrix;
	MTH3D_tdstVector *p_stTranslation;
	MTH3D_tdstVector* p_stAdditionnalTranslation;

	hCurrentMatrix = DNM_M_p_stDynamicsGetCurrentMatrix (p_stDynamic);
	p_stAdditionnalTranslation = DNM_M_pDynamicsGetAddTranslation (p_stDynamic);

	p_stTranslation = POS_fn_p_stGetTranslationVector ( hCurrentMatrix );
	MTH3D_M_vAddVector ( p_stTranslation, p_stTranslation, p_stAdditionnalTranslation);
}

void __cdecl MEC_fn_vPositionLimit ( DNM_tdstDynamics *p_stDynamic, MTH3D_tdstVector *p_stSafePreviousPosition )
{
	POS_tdxHandleToPosition hPreviousMatrix, hCurrentMatrix;
	MTH3D_tdstVector *p_stPreviousPosition, *p_stCurrentPosition;

	if (!DNM_M_bDynamicsIsLimit(p_stDynamic)) return;

	hCurrentMatrix = DNM_M_p_stDynamicsGetCurrentMatrix (p_stDynamic);
	hPreviousMatrix = DNM_M_p_stDynamicsGetPreviousMatrix (p_stDynamic);
	p_stPreviousPosition = POS_fn_p_stGetTranslationVector ( hPreviousMatrix );
	p_stCurrentPosition = POS_fn_p_stGetTranslationVector ( hCurrentMatrix );

	if (DNM_M_bDynamicsIsLimitX(p_stDynamic))
	{
		MTH_tdxReal	xXMin, xXMax, xX;
		xXMin = MTH_M_xMin ( MTH3D_M_xGetXofVector (p_stPreviousPosition), MTH3D_M_xGetXofVector (p_stCurrentPosition) );
		xXMax = MTH_M_xMax ( MTH3D_M_xGetXofVector (p_stPreviousPosition), MTH3D_M_xGetXofVector (p_stCurrentPosition) );
		xX = MTH3D_M_xGetXofVector ( DNM_M_pDynamicsGetLimit(p_stDynamic) );

		if ( MTH_M_bLessEqual(xXMin,xX)  && MTH_M_bLess(xX,xXMax) )
		{
			MTH3D_M_vSetXofVector (p_stCurrentPosition,MTH3D_M_xGetXofVector(p_stSafePreviousPosition));
		}
	}

	if (DNM_M_bDynamicsIsLimitY(p_stDynamic))
	{
		MTH_tdxReal	xYMin, xYMax, xY;
		xYMin = MTH_M_xMin ( MTH3D_M_xGetYofVector (p_stPreviousPosition), MTH3D_M_xGetYofVector (p_stCurrentPosition) );
		xYMax = MTH_M_xMax ( MTH3D_M_xGetYofVector (p_stPreviousPosition), MTH3D_M_xGetYofVector (p_stCurrentPosition) );
		xY = MTH3D_M_xGetYofVector ( DNM_M_pDynamicsGetLimit(p_stDynamic) );

		if ( MTH_M_bLessEqual (xYMin,xY)  && MTH_M_bLess (xY,xYMax) )
		{
			MTH3D_M_vSetYofVector (p_stCurrentPosition,MTH3D_M_xGetYofVector(p_stSafePreviousPosition));
		}
	}

	if (DNM_M_bDynamicsIsLimitZ(p_stDynamic))
	{
		MTH_tdxReal	xZMin, xZMax, xZ;
		xZMin = MTH_M_xMin ( MTH3D_M_xGetZofVector (p_stPreviousPosition), MTH3D_M_xGetZofVector (p_stCurrentPosition) );
		xZMax = MTH_M_xMax ( MTH3D_M_xGetZofVector (p_stPreviousPosition), MTH3D_M_xGetZofVector (p_stCurrentPosition) );
		xZ = MTH3D_M_xGetZofVector ( DNM_M_pDynamicsGetLimit(p_stDynamic) );

		if ( MTH_M_bLessEqual(xZMin,xZ)  && MTH_M_bLess(xZ,xZMax) )
		{
			MTH3D_M_vSetZofVector (p_stCurrentPosition,MTH3D_M_xGetZofVector(p_stSafePreviousPosition));
		}
	}
}

void __cdecl MEC_fn_vSpeedLimit
(
	DNM_tdstDynamics *p_stDynamic,
	MTH_tdxReal xDT,
	POS_tdxHandleToPosition hMatrix,
	POS_tdxHandleToPosition hInvMatrix
)
{
	POS_tdxHandleToPosition hPreviousMatrix, hCurrentMatrix;
	MTH3D_tdstVector *p_stPreviousPosition, *p_stCurrentPosition;
	MTH3D_tdstVector stMove;
	MTH3D_tdstVector stCorrectMove;
	MTH3D_tdstVector stMaxMove;

	hCurrentMatrix = DNM_M_p_stDynamicsGetCurrentMatrix (p_stDynamic);
	hPreviousMatrix = DNM_M_p_stDynamicsGetPreviousMatrix (p_stDynamic);

	p_stPreviousPosition = POS_fn_p_stGetTranslationVector ( hPreviousMatrix );
	p_stCurrentPosition = POS_fn_p_stGetTranslationVector ( hCurrentMatrix );
	MTH3D_M_vSubVector ( &stMove, p_stCurrentPosition, p_stPreviousPosition );

	MEC_fn_vInvertMatrix (p_stDynamic,hInvMatrix,hMatrix);
	POS_fn_vRotateVector ( &stMove, hInvMatrix, &stMove );

	MTH3D_M_vCopyVector ( &stMaxMove, DNM_M_pDynamicsGetMaxSpeed(p_stDynamic) );
	MTH3D_M_vMulScalarVector ( &stMaxMove, xDT, &stMaxMove );
	MTH3D_M_vCopyVector ( &stCorrectMove, &stMove );

	if (MTH_M_bGreater(MTH_M_xAbs(MTH3D_M_xGetXofVector(&stMove)),MTH3D_M_xGetXofVector(&stMaxMove)))
		MTH3D_M_vSetXofVector ( &stCorrectMove, MTH_M_xMul ( MTH3D_M_xGetXofVector(&stMaxMove), MTH_M_xSign(MTH3D_M_xGetXofVector(&stMove))) );

	if (MTH_M_bGreater(MTH_M_xAbs(MTH3D_M_xGetYofVector(&stMove)),MTH3D_M_xGetYofVector(&stMaxMove)))
		MTH3D_M_vSetYofVector ( &stCorrectMove, MTH_M_xMul ( MTH3D_M_xGetYofVector(&stMaxMove), MTH_M_xSign(MTH3D_M_xGetYofVector(&stMove))) );

	if (MTH_M_bGreater(MTH_M_xAbs(MTH3D_M_xGetZofVector(&stMove)),MTH3D_M_xGetZofVector(&stMaxMove)))
		MTH3D_M_vSetZofVector ( &stCorrectMove, MTH_M_xMul ( MTH3D_M_xGetZofVector(&stMaxMove), MTH_M_xSign(MTH3D_M_xGetZofVector(&stMove))) );

	POS_fn_vRotateVector ( &stCorrectMove, hMatrix, &stCorrectMove );

	MTH3D_M_vAddVector ( p_stCurrentPosition, p_stPreviousPosition, &stCorrectMove );
}

void __cdecl MEC_fn_vAdjustScaleData
(
	DNM_tdstDynamics *p_stDynamic,
	POS_tdxHandleToPosition hMatrix,
	POS_tdxHandleToPosition hInvMatrix
)
{
	POS_tdxHandleToPosition hPreviousMatrix, hCurrentMatrix;
	MTH3D_tdstVector *p_stPreviousPosition, *p_stCurrentPosition;
	MTH3D_tdstVector stTranslation;

	hCurrentMatrix = DNM_M_p_stDynamicsGetCurrentMatrix (p_stDynamic);
	hPreviousMatrix = DNM_M_p_stDynamicsGetPreviousMatrix (p_stDynamic);

	MEC_fn_vInvertMatrix (p_stDynamic,hInvMatrix,hMatrix);

	p_stPreviousPosition = POS_fn_p_stGetTranslationVector ( hPreviousMatrix );
	p_stCurrentPosition = POS_fn_p_stGetTranslationVector ( hCurrentMatrix );
	MTH3D_M_vSubVector ( &stTranslation, p_stCurrentPosition, p_stPreviousPosition);

	POS_fn_vRotateVector ( &stTranslation, hInvMatrix, &stTranslation );
	MEC_fn_vAdjustScaleVector ( p_stDynamic, &stTranslation );
	POS_fn_vRotateVector ( &stTranslation, hMatrix, &stTranslation );

	MTH3D_M_vAddVector ( p_stCurrentPosition, p_stPreviousPosition, &stTranslation );

	MTH3D_M_vCopyVector ( &stTranslation, DNM_M_pDynamicsGetImposeSpeed(p_stDynamic) );
	if (!DNM_M_bDynamicsIsRelativeImposeSpeed(p_stDynamic)) POS_fn_vRotateVector ( &stTranslation, hInvMatrix, &stTranslation );
	MEC_fn_vAdjustScaleVector ( p_stDynamic, &stTranslation );
	if (!DNM_M_bDynamicsIsRelativeImposeSpeed(p_stDynamic)) POS_fn_vRotateVector ( &stTranslation, hMatrix, &stTranslation );
	DNM_M_vDynamicsSetImposeSpeed (p_stDynamic,&stTranslation);

	MTH3D_M_vCopyVector ( &stTranslation, DNM_M_pDynamicsGetProposeSpeed(p_stDynamic) );
	if (!DNM_M_bDynamicsIsRelativeProposeSpeed(p_stDynamic)) POS_fn_vRotateVector ( &stTranslation, hInvMatrix, &stTranslation );
	MEC_fn_vAdjustScaleVector ( p_stDynamic, &stTranslation );
	if (!DNM_M_bDynamicsIsRelativeImposeSpeed(p_stDynamic)) POS_fn_vRotateVector ( &stTranslation, hMatrix, &stTranslation );
	DNM_M_vDynamicsSetProposeSpeed (p_stDynamic,&stTranslation);

	MTH3D_M_vCopyVector ( &stTranslation, DNM_M_pDynamicsGetAddSpeed(p_stDynamic) );
	if (!DNM_M_bDynamicsIsRelativeAddSpeed(p_stDynamic)) POS_fn_vRotateVector ( &stTranslation, hInvMatrix, &stTranslation );
	MEC_fn_vAdjustScaleVector ( p_stDynamic, &stTranslation );
	if (!DNM_M_bDynamicsIsRelativeImposeSpeed(p_stDynamic)) POS_fn_vRotateVector ( &stTranslation, hMatrix, &stTranslation );
	DNM_M_vDynamicsSetAddSpeed (p_stDynamic,&stTranslation);
}
