#include <stddef.h>

#include "MEC/MEC.h"
#include "MEC/MECTools.h"
#include "DNM/DNM.h"
#include "HIE/HIE_types.h"
#include "MTH/MTH_flt.h"
#include "MTH/MTH_xNeg.h"
#include "MTH/MTH3D.h"
#include "POS/POS_types.h"
#include "GMT/GMT_types.h"

extern DNM_tdstMecObstacle *g_a_stObstacle;
/* 0x00494290 */
extern long __cdecl COL_lHaveITakeSThgIntoTheMug (DNM_tdstMecObstacle *_a_stResultObstacle, HIE_tdstSuperObject *_p_stSupObj, POS_tdstCompletePosition *_p_stStartPosition, POS_tdstCompletePosition *_p_stEndPosition);

/* 0x00432A90 */
void __cdecl MEC_vImposeRotationVector(DNM_tdstDynamics *p_stDynamic,
                                       MTH3D_tdstVector *p_stX,
                                       MTH3D_tdstVector *p_stY,
                                       MTH3D_tdstVector *p_stZ)
{
    MTH3D_tdstMatrix *p_stImposeRotationMatrix;

    if (DNM_M_bDynamicsIsInit(p_stDynamic)) return;

    p_stImposeRotationMatrix = DNM_M_p_stDynamicsGetImposeRotationMatrix(p_stDynamic);
    MTH3D_M_vSetVectorsInMatrix(p_stImposeRotationMatrix, p_stX, p_stY, p_stZ);

    DNM_M_vDynamicsSetImposeRotation(p_stDynamic, 1  );
}

/* 0x00445F90 */
void __cdecl MEC_fn_vUpdateMatrix(DNM_tdstDynamics *p_stDynamic,
                                  POS_tdxHandleToPosition _hMatrix)
{
    MTH3D_tdstVector stI, stJ, stK;

    MTH3D_M_vSetVectorElements(&stI, DNM_M_p_stDynamicsGetScaleX(p_stDynamic),
                                      MTH_C_ZERO, MTH_C_ZERO);
    MTH3D_M_vSetVectorElements(&stJ, MTH_C_ZERO,
                                      DNM_M_p_stDynamicsGetScaleY(p_stDynamic),
                                      MTH_C_ZERO);
    MTH3D_M_vSetVectorElements(&stK, MTH_C_ZERO, MTH_C_ZERO,
                                      DNM_M_p_stDynamicsGetScaleZ(p_stDynamic));

    POS_fn_vSetScaleMatrix(_hMatrix, &stI, &stJ, &stK);
}

/* 0x0043E250 */
void __cdecl MEC_fn_vCancelTilt(DNM_tdstDynamics *p_stDynamic)
{
    MTH3D_tdstVector stInitialTranslation, stVector;
    MTH3D_tdstVector *p_stCurrentI, *p_stCurrentJ, *p_stCurrentK;
    MTH3D_tdstVector stOriginTranslation;
    MTH3D_tdstVector *p_stTranslation;
    POS_tdxHandleToPosition hCurrentMatrix;
    MTH_tdxReal xAngle;

    xAngle = DNM_M_xDynamicsGetTiltAngle(p_stDynamic);
    if (MTH_M_bEqual(xAngle,MTH_C_ZERO)) return;

    hCurrentMatrix = DNM_M_p_stDynamicsGetCurrentMatrix(p_stDynamic);
    POS_fn_vGetRotationVector(hCurrentMatrix, &p_stCurrentI, &p_stCurrentJ, &p_stCurrentK);
    p_stTranslation = POS_fn_p_stGetTranslationVector(hCurrentMatrix);
    MTH3D_M_vSetBaseJVector(&stVector);

    MTH3D_M_vMulScalarVector(&stOriginTranslation,
                              DNM_M_xDynamicsGetTiltOrigin(p_stDynamic),
                              p_stCurrentK);
    MTH3D_M_vCopyVector(&stInitialTranslation, p_stTranslation);
    MTH3D_M_vSubVector(p_stTranslation, p_stTranslation, &stOriginTranslation);
    POS_fn_vRotatePositionAroundAxis(hCurrentMatrix, &stVector, MTH_M_xNeg(xAngle));
    POS_fn_vSetTranslationVector(hCurrentMatrix, &stInitialTranslation);

    DNM_M_vDynamicsSetTiltAngle(p_stDynamic, MTH_C_ZERO);
}

static MTH_tdxReal MEC_fn_xNegOpt(MTH_tdxReal xA)
{
    unsigned long  xNeg;
    unsigned long *pA = (unsigned long *)&xA;

    xNeg = (*pA) + 0x80000000;
    return *(MTH_tdxReal *)(void *)&xNeg;
}

/* 0x00432B10 */
void __cdecl MEC_vImposeRelativeRotation(DNM_tdstDynamics *p_stDynamic,
                                         MTH3D_tdstMatrix *p_stRotationMatrix)
{
    MTH3D_tdstVector *p_stI, *p_stJ, *p_stK;
    MTH3D_tdstMatrix stMatrix;
    POS_tdxHandleToPosition hCurrentMatrix;

    if (DNM_M_bDynamicsIsInit(p_stDynamic)) return;
    hCurrentMatrix = DNM_M_p_stDynamicsGetCurrentMatrix(p_stDynamic);
    POS_fn_vGetRotationVector(hCurrentMatrix, &p_stI, &p_stJ, &p_stK);
    MTH3D_M_vSetVectorsInMatrix(&stMatrix, p_stI, p_stJ, p_stK);
    MTH3D_M_vMulMatrixMatrix(&stMatrix, &stMatrix, p_stRotationMatrix);
    MTH3D_M_vGetVectorsInMatrix(p_stI, p_stJ, p_stK, &stMatrix);
    MEC_fn_vUpdateMatrix(p_stDynamic, hCurrentMatrix);

    if (DNM_M_bDynamicsIsComplexSize(p_stDynamic))
        MEC_fn_vCancelTilt(p_stDynamic);
}

/* 0x00432D30 */
void __cdecl MEC_vImposeTranslation(DNM_tdstDynamics *p_stDynamic,
                                    MTH3D_tdstVector *p_stVector)
{
    MTH3D_tdstVector stVector;
    MTH3D_tdstVector *p_stCurrentTranslation;
    POS_tdxHandleToPosition hCurrentMatrix;

    if (DNM_M_bDynamicsIsInit(p_stDynamic)) return;

    DNM_M_vDynamicsSetImposeTranslation(p_stDynamic, 1  );
    hCurrentMatrix = DNM_M_p_stDynamicsGetCurrentMatrix(p_stDynamic);
    p_stCurrentTranslation = POS_fn_p_stGetTranslationVector(hCurrentMatrix);

    MTH3D_M_vSubVector(&stVector, p_stVector, p_stCurrentTranslation);
    MTH3D_M_vCopyVector(DNM_M_pDynamicsGetAddTranslation(p_stDynamic), &stVector);
    MTH3D_M_vNullVector(DNM_M_pDynamicsGetPreviousSpeed(p_stDynamic));
}

void __cdecl MEC_vInitTranslation(DNM_tdstDynamics *p_stDynamic,
                                  HIE_tdstSuperObject *hSupObj,
                                  MTH3D_tdstVector *p_stVector)
{
    POS_tdxHandleToPosition hCurrentMatrix, hPreviousMatrix;
    POS_tdxHandleToPosition hAbsolutePreviousMatrix, hPrevPreviousMatrix;
    POS_tdxHandleToPosition hGlobalMatrix;

    if (DNM_M_bDynamicsIsInit(p_stDynamic))
    {
        hGlobalMatrix = HIE_fn_hGetSuperObjectGlobalMatrix(hSupObj);
        POS_fn_vSetTranslationVector(hGlobalMatrix, p_stVector);
    }
    else
    {

        hCurrentMatrix  = DNM_M_p_stDynamicsGetCurrentMatrix(p_stDynamic);
        hPreviousMatrix = DNM_M_p_stDynamicsGetPreviousMatrix(p_stDynamic);
        hGlobalMatrix   = HIE_fn_hGetSuperObjectGlobalMatrix(hSupObj);

        POS_fn_vSetTranslationVector(hCurrentMatrix, p_stVector);
        POS_fn_vSetTranslationVector(hPreviousMatrix, p_stVector);
        POS_fn_vCopyMatrix(hGlobalMatrix, hCurrentMatrix);

        if (DNM_M_bDynamicsIsComplexSize(p_stDynamic))
        {
            hAbsolutePreviousMatrix = DNM_M_p_stDynamicsGetAbsolutePreviousMatrix(p_stDynamic);
            hPrevPreviousMatrix     = DNM_M_p_stDynamicsGetPrevPreviousMatrix(p_stDynamic);

            POS_fn_vSetTranslationVector(hAbsolutePreviousMatrix, p_stVector);
            POS_fn_vSetTranslationVector(hPrevPreviousMatrix, p_stVector);
        }
    }

    HIE_fn_vComputeNewRelativeMatrix(hSupObj);

    MTH3D_M_vNullVector(DNM_M_pDynamicsGetPreviousSpeed(p_stDynamic));
    MTH3D_M_vNullVector(DNM_M_pDynamicsGetAddTranslation(p_stDynamic));

    if (DNM_M_bDynamicsIsAdvancedSize(p_stDynamic))
    {
        MTH3D_M_vNullVector(DNM_M_pDynamicsGetInertiaTranslation(p_stDynamic));
    }
}

/* 0x00432E60 */
void __cdecl MEC_fn_vImposeActorMatrix(HIE_tdstSuperObject *hSupObj)
{
    POS_tdxHandleToPosition hCurrentMatrix, hPreviousMatrix;
    POS_tdxHandleToPosition hAbsolutePreviousMatrix, hPrevPreviousMatrix;
    POS_tdxHandleToPosition hGlobalMatrix;
    DNM_tdstDynamics *p_stDynamic;

    p_stDynamic = fn_p_stDynamGetDynamicsSO(hSupObj);
    if (!p_stDynamic) return;

    hCurrentMatrix  = DNM_M_p_stDynamicsGetCurrentMatrix(p_stDynamic);
    hPreviousMatrix = DNM_M_p_stDynamicsGetPreviousMatrix(p_stDynamic);
    hGlobalMatrix   = HIE_fn_hGetSuperObjectGlobalMatrix(hSupObj);

    POS_fn_vCopyMatrix(hCurrentMatrix, hGlobalMatrix);
    POS_fn_vCopyMatrix(hPreviousMatrix, hGlobalMatrix);
    MTH3D_M_vNullVector(DNM_M_pDynamicsGetPreviousSpeed(p_stDynamic));
    DNM_M_vDynamicsSetActorMove(p_stDynamic, 1  );

    if (DNM_M_bDynamicsIsComplexSize(p_stDynamic))
    {
        hAbsolutePreviousMatrix = DNM_M_p_stDynamicsGetAbsolutePreviousMatrix(p_stDynamic);
        hPrevPreviousMatrix     = DNM_M_p_stDynamicsGetPrevPreviousMatrix(p_stDynamic);

        POS_fn_vCopyMatrix(hAbsolutePreviousMatrix, hGlobalMatrix);
        POS_fn_vCopyMatrix(hPrevPreviousMatrix, hGlobalMatrix);
    }
}

/* 0x004329B0 */
void __cdecl MEC_vImposeRotationMatrix(DNM_tdstDynamics *p_stDynamic,
                                       POS_tdxHandleToPosition hRotationMatrix)
{
    POS_tdxHandleToPosition hCurrentMatrix;
    MTH3D_tdstVector *p_stCurrentTranslation, *p_stMatrixTranslation;
    MTH3D_tdstVector stMove;

    if (DNM_M_bDynamicsIsInit(p_stDynamic)) return;

    hCurrentMatrix = DNM_M_p_stDynamicsGetCurrentMatrix(p_stDynamic);

    DNM_M_vDynamicsSetImposeRotation(p_stDynamic, 1  );
    DNM_M_vDynamicsSetImposeRotationMatrix(p_stDynamic,
        POS_fn_vGetRotationMatrixReadOnly(hRotationMatrix));

    p_stCurrentTranslation = POS_fn_p_stGetTranslationVector(hCurrentMatrix);
    p_stMatrixTranslation  = POS_fn_p_stGetTranslationVector(hRotationMatrix);

    MTH3D_M_vSubVector(&stMove, p_stMatrixTranslation, p_stCurrentTranslation);
    DNM_M_vDynamicsSetAddTranslation(p_stDynamic, &stMove);

    if (DNM_M_bDynamicsIsComplexSize(p_stDynamic))
        MEC_fn_vCancelTilt(p_stDynamic);
}

/* 0x00433210 */
typedef struct DNM_stDynamics_ DNM_tdstDynamics_local0, DNM_tdstDynamics_local1,
	DNM_tdstDynamics_local2, DNM_tdstDynamics_local3, DNM_tdstDynamics_local4,
	DNM_tdstDynamics_local5, DNM_tdstDynamics_local6, DNM_tdstDynamics_local7,
	DNM_tdstDynamics_local8, DNM_tdstDynamics_local9, DNM_tdstDynamics_local10,
	DNM_tdstDynamics_local11, DNM_tdstDynamics_local12, DNM_tdstDynamics_local13;

#include "MEC/DNMMth.h"

#define MEC_M_GetMSHandleDynam(hSupObj) \
    (*(MS_tdxHandleToDynam *)((char *)HIE_fn_hGetSuperObjectObject(hSupObj) + 0x08))

void __cdecl MEC_fn_vUpdateSpeed(HIE_tdstSuperObject *hSupObj)
{
	MS_tdxHandleToDynam h_Dynam;
	DNM_tdstDynamics *p_stDynamic;
	MTH3D_tdstVector *p_stSpeed;
	POS_tdxHandleToPosition hPreviousMatrix;
	MTH3D_tdstVector *p_stCurrentX, *p_stCurrentY, *p_stCurrentZ;
	MTH3D_tdstVector stPreviousY;
	MTH3D_tdstVector stAxis;
	MTH3D_tdstVector  stCrossProduct;
	MTH_tdstMove* p_stMove;
	MTH_tdxReal xDotProd, xAngle;

	h_Dynam = MEC_M_GetMSHandleDynam(hSupObj);
	if (!h_Dynam) return;
	p_stDynamic = fn_p_stDynamGetDynamics(h_Dynam);
	if ( !DNM_M_bDynamicsIsComplexSize(p_stDynamic) || !DNM_M_bDynamicsIsCollisionReport(p_stDynamic) ) return;

	p_stSpeed = DNM_M_pDynamicsGetPreviousSpeed (p_stDynamic);
	hPreviousMatrix = DNM_M_p_stDynamicsGetPreviousMatrix (p_stDynamic);

	POS_fn_vGetRotationVector ( hPreviousMatrix, &p_stCurrentX, &p_stCurrentY, &p_stCurrentZ );

	p_stMove = DNM_M_p_stReportGetAbsoluteCurrSpeed ( DNM_M_p_stDynamicsGetReport (p_stDynamic) );
	MTH3D_M_vCopyVector (&stPreviousY,MTH_M_p_stRotationGetAxis(MTH_M_p_stMoveGetAngular(p_stMove)));

	/* 0x00433290 */
	xDotProd = MTH3D_M_xDotProductVector (&stPreviousY,p_stCurrentY);
	MTH3D_M_vCrossProductVectorWithoutBuffer (&stCrossProduct, &stPreviousY, p_stCurrentY);

	xDotProd = MTH_M_xMin ( MTH_M_xMax ( xDotProd, MTH_C_MinusONE ), MTH_C_ONE );

	if (MTH_M_bGreater(MTH3D_M_xGetZofVector(&stCrossProduct),MTH_C_ZERO))
	{
		xAngle = MTH_M_xACos (xDotProd);
	}
	else
	{
		xAngle = MTH_M_xNeg (MTH_M_xACos(xDotProd));
	}

	MTH3D_M_vSetBaseKVector (&stAxis);
	MTH_M_MoveSetAngularPosition ( p_stMove, xAngle, &stAxis );
}

#ifndef DNM_CAM_C_FixedOrientation
#define DNM_CAM_C_FixedOrientation      0x0080
#endif

#include "MEC/macdpid.h"
#include "MEC/ParsData.h"
#include "MEC/DNMRepor.h"
#include "MEC/dnmdynam.h"

#define DNM_CAM_C_NoLinearParsing   0x0001
#define DNM_CAM_C_NoLinearInertia   0x0002
#define DNM_CAM_C_NoAngularParsing  0x0004
#define DNM_CAM_C_NoAngularInertia  0x0008
#define DNM_CAM_C_NoTargetParsing   0x0010
#define DNM_CAM_C_NoTargetInertia   0x0020
#ifndef C_WOT_ulWall
#define C_WOT_ulWall  0x00000004UL
#endif
#define DNM_CAM_C_NoObstacle                0x0040
#define DNM_CAM_C_NoCollisionWhenNotMoving  0x0100

#ifndef MTH_C_PiBy6
#define MTH_C_PiBy6          ((MTH_tdxReal)(3.141592653589793f / 6.0f))
#endif
#define MYH_C_PiMinusPiBy6   ((MTH_tdxReal)(3.141592653589793f - 3.141592653589793f / 6.0f))

static void __cdecl DNM_fn_vDynamicsCameraPositionParsing (
	MTH_tdxReal _xSpeed, MTH_tdxReal _xIncSpeed, MTH_tdxReal _xDecSpeed, MTH_tdxReal _xLastNorm,
	char _cIgnoreParsing, char _cIgnoreInertia,
	MTH3D_tdstVector *_p_stActualPos, MTH3D_tdstVector *_p_stShiftingSrc,
	MTH3D_tdstVector *_p_stDestPos, MTH3D_tdstVector *_p_stShiftingResult)
{
	MTH_tdxReal xNorm, xNormSrc, xTmpNorm;
	MTH_tdxReal xFactAccel, xInt;
	MTH_tdxReal xNormWanted;
	ACP_tdxBool bNormWantedIsModified = FALSE;
	MTH_tdxReal xScale;

	xNormWanted = xNormSrc = MTH3D_M_xNormVector(_p_stShiftingSrc);

	/* 0x0049C6F8 */
	if((_cIgnoreParsing) || (MTH_M_bLessEqual(xNormSrc, MTH_M_xConstFloatToReal(0.001f))))
	{
		MTH3D_M_vAddVector(_p_stDestPos, _p_stActualPos, _p_stShiftingSrc);
		MTH3D_M_vCopyVector(_p_stShiftingResult, _p_stShiftingSrc);
		return;
	}

	if (MTH_M_bEqualZero(_xSpeed))
	{
		MTH3D_M_vCopyVector(_p_stDestPos, _p_stActualPos);
		MTH3D_M_vNullVector(_p_stShiftingSrc);
		return;
	}

	MTH3D_M_vCopyVector(_p_stShiftingResult, _p_stShiftingSrc);

	if (MTH_M_bGreater(xNormSrc, _xSpeed))
	{
		xNormWanted = _xSpeed;
		bNormWantedIsModified = TRUE;
	}

	if (!_cIgnoreInertia)
	{

		xNorm = xNormSrc;
		if (!MTH_M_bIsNull(_xDecSpeed))
		{
			xInt = MTH_M_xMul(_xSpeed, _xDecSpeed);
			if (MTH_M_bLess(xNorm, xInt))
			{
				xNorm = MTH_M_xDiv(xNorm, _xDecSpeed);
				if (MTH_M_bGreater(xNorm, MTH_M_xConstFloatToReal(0.001f)) && MTH_M_bLess(xNorm, xNormWanted))
				{
					bNormWantedIsModified = TRUE;
					xNormWanted = xNorm;
				}
			}
		}

		if (!MTH_M_bIsNull(_xIncSpeed))
		{
			xFactAccel = MTH_M_xDiv(_xSpeed, _xIncSpeed);
			if (MTH_M_bGreater(xNormWanted, (xTmpNorm = MTH_M_xAdd(_xLastNorm, xFactAccel))))
			{
				bNormWantedIsModified = TRUE;
				xNormWanted = xTmpNorm;
			}
		}
	}

	if (bNormWantedIsModified)
	{
		xScale = MTH_M_xDiv(xNormWanted, xNormSrc);
		MTH3D_M_vMulScalarVector(_p_stShiftingResult, xScale, _p_stShiftingResult);
	}

	MTH3D_M_vAddVector(_p_stDestPos, _p_stActualPos, _p_stShiftingResult);
}

MTH_tdxReal __cdecl DNM_fn_xDynamicsCameraAngleParsing (
	MTH_tdxReal _xSpeed, MTH_tdxReal _xIncSpeed, MTH_tdxReal _xDecSpeed,
	MTH_tdxReal _xRequestAngle, MTH_tdxReal _xLastAngle,
	char _cIgnoreParsing, char _cIgnoreInertia)
{
	MTH_tdxReal xRetAngle, xInt, xAngle, xFactAccel;

	if (_cIgnoreParsing || MTH_M_bLess(_xRequestAngle, (MTH_tdxReal)0.001f))
		return _xRequestAngle;

	xRetAngle = _xRequestAngle;
	if (MTH_M_bGreater(xRetAngle, _xSpeed))
		xRetAngle = _xSpeed;

	if (!_cIgnoreInertia)
	{
		xAngle = _xRequestAngle;
		xInt = MTH_M_xMul(_xSpeed, _xDecSpeed);
		if (MTH_M_bLess(xAngle, xInt))
		{
			xAngle = MTH_M_xDiv(xAngle, _xDecSpeed);
			if (MTH_M_bGreater(xRetAngle, xAngle))
				xRetAngle = xAngle;
		}

		xFactAccel = MTH_M_xDiv(_xSpeed, _xIncSpeed);
		if (MTH_M_bGreater(xRetAngle, MTH_M_xAdd(_xLastAngle, xFactAccel)))
			xRetAngle = MTH_M_xAdd(_xLastAngle, xFactAccel);
	}

	return xRetAngle;
}

#ifndef MTH_M_RotationSetNull
#define MTH_M_RotationSetNull(_p_stRotation) do {\
	(_p_stRotation)->m_xAngle = MTH_C_ZERO;\
	MTH3D_M_vNullVector(&(_p_stRotation)->m_stAxis);\
} while (0)
#endif

MTH_tdstRotation * __cdecl MTH_p_stRotationComputeRotationWith2Vectors (MTH_tdstRotation *_p_stResult, MTH3D_tdstVector *_p_stSrcUnitVector, MTH3D_tdstVector *_p_stImaUnitVector)
{
	MTH3D_tdstVector *p_stAxis;
	MTH_tdxReal xCos;
	MTH_tdxReal xEpsilon = (MTH_tdxReal)1.0e-5f;

	p_stAxis = MTH_M_p_stRotationGetAxis(_p_stResult);
	MTH3D_M_vCrossProductVectorWithoutBuffer(p_stAxis, _p_stSrcUnitVector, _p_stImaUnitVector);

	if (MTH_M_bIsNullWithEpsilon(MTH3D_M_xGetXofVector(p_stAxis), xEpsilon) &&
	    MTH_M_bIsNullWithEpsilon(MTH3D_M_xGetYofVector(p_stAxis), xEpsilon) &&
	    MTH_M_bIsNullWithEpsilon(MTH3D_M_xGetZofVector(p_stAxis), xEpsilon))
	{
		if (MTH_M_bEqualWithEpsilon(MTH3D_M_xGetXofVector(_p_stSrcUnitVector), MTH3D_M_xGetXofVector(_p_stImaUnitVector), xEpsilon) &&
		    MTH_M_bEqualWithEpsilon(MTH3D_M_xGetYofVector(_p_stSrcUnitVector), MTH3D_M_xGetYofVector(_p_stImaUnitVector), xEpsilon) &&
		    MTH_M_bEqualWithEpsilon(MTH3D_M_xGetZofVector(_p_stSrcUnitVector), MTH3D_M_xGetZofVector(_p_stImaUnitVector), xEpsilon))
			MTH_M_RotationSetNull(_p_stResult);
		else
			MTH_M_xRotationSetAngle(_p_stResult, MTH_C_Pi);
	}
	else
	{
		MTH3D_M_vNormalizeVector(p_stAxis, p_stAxis);

		xCos = MTH3D_M_xDotProductVector(_p_stSrcUnitVector, _p_stImaUnitVector);
		if (MTH_M_bLessEqual(xCos, MTH_C_MinusONE))
			MTH_M_xRotationSetAngle(_p_stResult, MTH_C_Pi);
		else if (MTH_M_bGreaterEqual(xCos, MTH_C_ONE))
			MTH_M_RotationSetNull(_p_stResult);
		else
			MTH_M_xRotationSetAngle(_p_stResult, MTH_M_xACos(xCos));
	}

	return _p_stResult;
}

MTH3D_tdstVector * __cdecl MTH_p_stRotationMulVector (MTH_tdstRotation *_p_stRotation, MTH3D_tdstVector *_p_stSource, MTH3D_tdstVector *_p_stResult)
{
	MTH3D_tdstVector stTmp0, stTmp1;
	MTH3D_tdstVector *p_stAxis;
	MTH_tdxReal xCos, xSin, xValue, xAngle;

	xAngle = MTH_M_xRotationGetAngle(_p_stRotation);
	xCos = MTH_M_xCos(xAngle);
	xSin = MTH_M_xSin(xAngle);
	p_stAxis = MTH_M_p_stRotationGetAxis(_p_stRotation);

	MTH3D_M_vCrossProductVector(&stTmp0, p_stAxis, _p_stSource);
	MTH3D_M_vMulScalarVector(&stTmp0, xSin, &stTmp0);

	xValue = MTH_M_xMul(MTH3D_M_xDotProductVector(p_stAxis, _p_stSource), MTH_M_xSub(MTH_C_ONE, xCos));
	MTH3D_M_vCopyVector(&stTmp1, p_stAxis);
	MTH3D_M_vMulScalarVector(&stTmp1, xValue, &stTmp1);

	if(_p_stSource != _p_stResult)
		MTH3D_M_vCopyVector(_p_stResult, _p_stSource);
	MTH3D_M_vMulScalarVector(_p_stResult, xCos, _p_stResult);

	MTH3D_M_vAddVector(_p_stResult, _p_stResult, &stTmp1);
	MTH3D_M_vAddVector(_p_stResult, _p_stResult, &stTmp0);

	return _p_stResult;
}

void __cdecl MTH_fn_vVectorPlaneProjection (MTH3D_tdstVector *_p_stResult, MTH3D_tdstVector *_p_stSource, MTH3D_tdstVector *_p_stPlaneNorm, MTH3D_tdstVector *_p_stProjDirection);

MTH_tdxReal CAM_fn_xComputeHorizontalAngle(
	MTH3D_tdstVector	*_p_stOrgVector,
	MTH3D_tdstVector	*_p_stDestVector,
	MTH3D_tdstVector	*_p_stAxisZ)
{

	MTH3D_tdstVector	stVect1, stVect2, stAxisZ;
	MTH_tdstRotation	stRotation;
	MTH_tdxReal			xAngle;

	MTH3D_M_vCopyVector(&stVect1,_p_stOrgVector);
	MTH3D_M_vCopyVector(&stVect2,_p_stDestVector);
	MTH3D_M_vCopyVector(&stAxisZ,_p_stAxisZ);

	MTH_fn_vVectorPlaneProjection(&stVect1, &stVect1, &stAxisZ, &stAxisZ);

	if (!MEC_fn_bIsNullVector(&stVect1))
	{
		MTH3D_M_vNormalizeVector(&stVect1, &stVect1);
	}
	else
	{
		return(MTH_C_ZERO);
	}

	MTH_fn_vVectorPlaneProjection(&stVect2, &stVect2, &stAxisZ, &stAxisZ);

	if (!MEC_fn_bIsNullVector(&stVect2))
	{
		MTH3D_M_vNormalizeVector(&stVect2, &stVect2);
	}
	else
	{
		return(MTH_C_ZERO);
	}

	MTH_p_stRotationComputeRotationWith2Vectors(&stRotation, &stVect1, &stVect2);

	xAngle = MTH_M_xRotationGetAngle(&stRotation);

	if(MTH_M_bLessZero(MTH3D_M_xDotProductVector(MTH_M_p_stRotationGetAxis(&stRotation), &stAxisZ)))
		xAngle = MTH_M_xNeg(xAngle);

	return (xAngle);
}

MTH_tdxReal CAM_fn_xComputeVerticalAngle(
	MTH3D_tdstVector	*_p_stOrgVector,
	MTH3D_tdstVector	*_p_stDestVector,
	MTH3D_tdstVector	*_p_stAxisZ)
{

	MTH3D_tdstVector	stVector, stAxisZ, stProj;
	MTH_tdxReal			xOrgAngle, xDestAngle;
	MTH_tdstRotation	stRotation;
	MTH_tdxReal			xAngle;

	MTH3D_M_vCopyVector(&stAxisZ,_p_stAxisZ);

	MTH3D_M_vCopyVector(&stVector, _p_stOrgVector);

	/* 0x0049C6F8 */
	if(MTH_M_bIsNullWithEpsilon(MTH3D_M_xDotProductVector(&stVector, &stAxisZ), (MTH_tdxReal)0.001f))
		xOrgAngle = MTH_C_ZERO;
	else
	{
		MTH_fn_vVectorPlaneProjection(&stProj, &stVector, &stAxisZ, &stAxisZ);

		if (!MEC_fn_bIsNullVector(&stProj))
		{
			MTH3D_M_vNormalizeVector(&stVector, &stVector);

			MTH3D_M_vNormalizeVector(&stProj, &stProj);

			MTH_p_stRotationComputeRotationWith2Vectors(&stRotation, &stVector, &stProj);
			xOrgAngle = MTH_M_xRotationGetAngle(&stRotation);

			MTH_p_stRotationComputeRotationWith2Vectors(&stRotation, &stVector, &stAxisZ);
			xAngle = MTH_M_xRotationGetAngle(&stRotation);
			if (MTH_M_bGreater(xAngle, MTH_C_PiBy2))
				xOrgAngle = MTH_M_xNeg(xOrgAngle);
		}
		else
		{
			xOrgAngle = MTH_C_ZERO;
		}
	}

	MTH3D_M_vCopyVector(&stVector,_p_stDestVector);

	if(MTH_M_bEqualZero(MTH3D_M_xDotProductVector(&stVector, &stAxisZ)))
		xDestAngle = MTH_C_ZERO;
	else
	{
		MTH_fn_vVectorPlaneProjection(&stProj, &stVector, &stAxisZ, &stAxisZ);

		if (!MEC_fn_bIsNullVector(&stProj))
		{
			MTH3D_M_vNormalizeVector(&stVector, &stVector);

			MTH3D_M_vNormalizeVector(&stProj, &stProj);

			MTH_p_stRotationComputeRotationWith2Vectors(&stRotation, &stVector, &stProj);
			xDestAngle = MTH_M_xRotationGetAngle(&stRotation);

			MTH_p_stRotationComputeRotationWith2Vectors(&stRotation, &stVector, &stAxisZ);
			xAngle = MTH_M_xRotationGetAngle(&stRotation);
			if (MTH_M_bGreater(xAngle, MTH_C_PiBy2))
				xDestAngle = MTH_M_xNeg(xDestAngle);
		}
		else
		{
			xDestAngle = MTH_C_ZERO;
		}
	}

	xAngle = MTH_M_xSub(xDestAngle, xOrgAngle);
	return (xAngle);
}

static void __cdecl DNM_fn_vDynamicsTakeCareOfLastMove (MTH3D_tdstVector *_p_stShiftingResult, MTH3D_tdstVector *_p_stLastShifting, MTH_tdxReal _xCutAngleFactor)
{
	MTH3D_tdstVector stTempVector, stTempVector1;
	MTH_tdstRotation stRotation;
	MTH_tdxReal xNormShiftingResult, xNormLastShiftingResult;
	static MTH_tdxReal xOneOverTen = MTH_M_xConstFloatToReal(0.1f); /* 0x0049C708 */

	if(_xCutAngleFactor == MTH_C_ONE)
		return;

	xNormShiftingResult = MTH3D_M_xNormVector(_p_stShiftingResult);
	xNormLastShiftingResult = MTH3D_M_xNormVector(_p_stLastShifting);

	if ((MTH_M_bGreater(xNormShiftingResult, xOneOverTen)) &&
	    (MTH_M_bGreater(xNormLastShiftingResult, xOneOverTen)))
	{
		MTH3D_M_vNormalizeVector(&stTempVector, _p_stShiftingResult);
		MTH3D_M_vNormalizeVector(&stTempVector1, _p_stLastShifting);

		MTH_p_stRotationComputeRotationWith2Vectors(&stRotation, &stTempVector1, &stTempVector);

		stRotation.m_xAngle = MTH_M_xDiv(stRotation.m_xAngle, _xCutAngleFactor);
		MTH_p_stRotationMulVector(&stRotation, &stTempVector1, &stTempVector);
		MTH3D_M_vNormalizeVector(&stTempVector, &stTempVector);

		MTH3D_M_vMulScalarVector(_p_stShiftingResult, xNormShiftingResult, &stTempVector);
	}
}

void __cdecl MTH_fn_vVectorPlaneProjection (MTH3D_tdstVector *_p_stResult, MTH3D_tdstVector *_p_stSource, MTH3D_tdstVector *_p_stPlaneNorm, MTH3D_tdstVector *_p_stProjDirection)
{
	MTH3D_tdstVector stTmp;
	MTH_tdxReal xAlpha;
	MTH_tdxReal xDotProduct;

	if (_p_stProjDirection != _p_stPlaneNorm)
		xDotProduct = MTH3D_M_xDotProductVector(_p_stProjDirection, _p_stPlaneNorm);
	else
		xDotProduct = MTH_C_ONE;

	if (MTH_M_bDifferentZero(xDotProduct))
	{
		xAlpha = MTH_M_xNeg(MTH3D_M_xDotProductVector(_p_stSource, _p_stPlaneNorm));
		xAlpha = MTH_M_xDiv(xAlpha, xDotProduct);
		MTH3D_M_vMulScalarVector(&stTmp, xAlpha, _p_stProjDirection);
		if (_p_stResult != _p_stSource)
			MTH3D_M_vCopyVector(_p_stResult, _p_stSource);
		MTH3D_M_vAddVector(_p_stResult, _p_stResult, &stTmp);
	}
}

void __cdecl CAM_fn_vComputeCameraSystemAxis (DNM_tdstDynamics *_p_stDynamics, HIE_tdstSuperObject *_h_SupObj, MTH3D_tdstVector *_p_stNewPosition, MTH3D_tdstVector *_p_stNewTarget, MTH3D_tdstVector *_p_stAxisX, MTH3D_tdstVector *_p_stAxisY, MTH3D_tdstVector *_p_stAxisZ)
{
	MTH3D_tdstVector stXVector, stYVector;
	static MTH_tdxReal xOneOverFive = (MTH_tdxReal)0.2f;

	(void)_p_stDynamics;

	MTH3D_M_vSubVector(_p_stAxisY, _p_stNewPosition, _p_stNewTarget);

	MTH3D_M_vCopyVector(&stYVector,_p_stAxisY);

	if (!MEC_fn_bIsNullVector(_p_stAxisY))
		MTH3D_M_vNormalizeVector(_p_stAxisY, _p_stAxisY);

	MTH_fn_vVectorPlaneProjection(&stYVector, &stYVector, _p_stAxisZ, _p_stAxisZ);

	if (MTH_M_bLess(MTH3D_M_xNormVector(&stYVector), xOneOverFive))
		POS_fn_vGetRotationMatrix(HIE_fn_hGetSuperObjectGlobalMatrix(_h_SupObj), &stXVector, &stYVector, _p_stAxisZ);

	MTH3D_M_vCrossProductVectorWithoutBuffer(_p_stAxisX, _p_stAxisY, _p_stAxisZ);
	if (MTH3D_M_bIsNullVector(_p_stAxisX))
	{
		POS_fn_vGetRotationMatrix(HIE_fn_hGetSuperObjectGlobalMatrix(_h_SupObj), &stXVector, &stYVector, _p_stAxisZ);
		MTH3D_M_vCrossProductVectorWithoutBuffer(_p_stAxisX, _p_stAxisY, _p_stAxisZ);
	}
	MTH3D_M_vNormalizeVector(_p_stAxisX, _p_stAxisX);

	MTH3D_M_vCrossProductVectorWithoutBuffer(_p_stAxisZ, _p_stAxisX, _p_stAxisY);
	MTH3D_M_vNormalizeVector(_p_stAxisZ, _p_stAxisZ);
}

DNM_tdstDynamics * __cdecl DNM_p_stDynamicsCameraUpdate (
	DNM_tdstDynamics *_p_stDynamics,
	DNM_tdstParsingDatas *_p_stExternData,
	HIE_tdstSuperObject *_h_SupObj,
	DNM_tdstMecObstacle *_p_stObstacle,
	MTH3D_tdstVector *_p_stNewPosition,
	MTH3D_tdstVector *_p_stNewTarget)
{
	MTH3D_tdstVector stXVector, stYVector, stZVector;
	POS_tdxHandleToPosition hMatrix;

	(void)_p_stExternData;
	(void)_p_stObstacle;

	hMatrix = HIE_fn_hGetSuperObjectMatrix(_h_SupObj);

	if(!(M_uwMACDPIDGetFlags(DNM_M_p_stDynamicsGetExternalDatas(_p_stDynamics)) & DNM_CAM_C_FixedOrientation))
	{
		MTH3D_M_vCopyVector(&stZVector, M_p_stMACDPIDGetRefAxisZ(DNM_M_p_stDynamicsGetExternalDatas(_p_stDynamics)));

		CAM_fn_vComputeCameraSystemAxis(_p_stDynamics, _h_SupObj, _p_stNewPosition, _p_stNewTarget, &stXVector, &stYVector, &stZVector);

		POS_fn_vSetRotationMatrix(hMatrix, &stXVector, &stYVector, &stZVector);
	}

	POS_fn_vSetTranslationVector(hMatrix, _p_stNewPosition);

	POS_fn_vCopyMatrix(HIE_fn_hGetSuperObjectGlobalMatrix(_h_SupObj), hMatrix);

	return _p_stDynamics;
}

DNM_tdstDynamics * __cdecl DNM_p_stDynamicsCameraParsing (
	DNM_tdstDynamics *_p_stDynamics,
	HIE_tdstSuperObject *_h_SupObj,
	DNM_tdstParsingDatas *_p_stExternData,
	void *_h_MecIdCard,
	MTH_tdxReal _xDT)
{
	DNM_tdstReport *p_stReport;
	tdstMACDPID *p_stDNMMACDPID;
	MTH3D_tdstVector stPreviousPos;
	MTH3D_tdstVector stShiftingSrc, stShiftingResult;
	MTH3D_tdstVector stTempVector, stYVector;
	POS_tdstCompletePosition *p_stGlobalMatrix;
	MTH_tdxReal xSpeed, xAngle, xNorm;
	MTH3D_tdstVector stTempVector1, stTempVector2;
	MTH_tdstRotation stRotation;
	MTH3D_tdstVector stDNMTempVector;

	(void)_h_MecIdCard;

	p_stReport = DNM_M_p_stDynamicsGetReport(_p_stDynamics);
	p_stDNMMACDPID = DNM_M_p_stDynamicsGetExternalDatas(_p_stDynamics);

	p_stGlobalMatrix = HIE_fn_hGetSuperObjectGlobalMatrix(_h_SupObj);
	POS_fn_vGetTranslationVector(p_stGlobalMatrix, &stPreviousPos);

	MTH3D_M_vCopyVector(&stShiftingSrc, M_p_stMACDPIDGetMovePos(p_stDNMMACDPID));

	xSpeed = M_xMACDPIDGetLinearSpeed(p_stDNMMACDPID);
	xSpeed = MTH_M_xMul(xSpeed, _xDT);

	MTH3D_M_vCopyVector(&stTempVector, DNM_M_p_stReportGetLastMove(p_stReport));
	MTH3D_M_vCopyVector(&stTempVector1, DNM_M_p_stCPDGetPosition(_p_stExternData));

	DNM_fn_vDynamicsCameraPositionParsing
	(
		xSpeed,
		M_xMACDPIDGetLinearIncreaseSpeed(p_stDNMMACDPID),
		M_xMACDPIDGetLinearDecreaseSpeed(p_stDNMMACDPID),

		MTH3D_M_xNormVector(&stTempVector),
		(char) (M_uwMACDPIDGetFlags(p_stDNMMACDPID) & DNM_CAM_C_NoLinearParsing),
		(char) (M_uwMACDPIDGetFlags(p_stDNMMACDPID) & DNM_CAM_C_NoLinearInertia),
		&stPreviousPos,
		&stShiftingSrc,
		&stTempVector1,
		&stShiftingResult
	);
	MTH3D_M_vCopyVector(DNM_M_p_stCPDGetPosition(_p_stExternData), &stTempVector1);

	if(!(M_uwMACDPIDGetFlags(p_stDNMMACDPID) & DNM_CAM_C_NoAngularParsing))
	{
		xSpeed = M_xMACDPIDGetAngularSpeed(p_stDNMMACDPID);
		xSpeed = MTH_M_xMul(xSpeed, _xDT);

		MTH3D_M_vSubVector(&stTempVector, M_p_stMACDPIDGetPosReferencePoint(p_stDNMMACDPID), &stPreviousPos);
		if (!MEC_fn_bIsNullVector(&stTempVector))
			MTH3D_M_vNormalizeVector(&stTempVector, &stTempVector);

		MTH3D_M_vCopyVector(&stTempVector1, M_p_stMACDPIDGetPosReferencePoint(p_stDNMMACDPID));
		MTH3D_M_vCopyVector(&stTempVector2, DNM_M_p_stCPDGetPosition(_p_stExternData));

		MTH3D_M_vSubVector(&stTempVector1, &stTempVector1, &stTempVector2);
		xNorm = MTH3D_M_xNormVector(&stTempVector1);
		if (!MTH_M_bEqualZero(xNorm))
			MTH3D_M_vNormalizeVector(&stTempVector1, &stTempVector1);

		MTH_p_stRotationComputeRotationWith2Vectors(&stRotation, &stTempVector, &stTempVector1);
		xAngle = MTH_M_xRotationGetAngle(&stRotation);

		DNM_M_xCPDGetLastAngleMoveCam(_p_stExternData) =
			DNM_fn_xDynamicsCameraAngleParsing
			(
				xSpeed,
				M_xMACDPIDGetAngularIncreaseSpeed(p_stDNMMACDPID),
				M_xMACDPIDGetAngularDecreaseSpeed(p_stDNMMACDPID),
				xAngle,
				DNM_M_xCPDGetLastAngleMoveCam(_p_stExternData),
				(char) (M_uwMACDPIDGetFlags(p_stDNMMACDPID) & DNM_CAM_C_NoAngularParsing),
				(char) (M_uwMACDPIDGetFlags(p_stDNMMACDPID) & DNM_CAM_C_NoAngularInertia)
			);

		if(DNM_M_xCPDGetLastAngleMoveCam(_p_stExternData))
		{
			stRotation.m_xAngle = DNM_M_xCPDGetLastAngleMoveCam(_p_stExternData);
			MTH_p_stRotationMulVector(&stRotation, &stTempVector, &stTempVector);
		}

		MTH3D_M_vMulScalarVector(&stTempVector, xNorm, &stTempVector);
		MTH3D_M_vNegVector(&stTempVector, &stTempVector);

		MTH3D_M_vAddVector(DNM_M_p_stCPDGetPosition(_p_stExternData), &stTempVector, M_p_stMACDPIDGetPosReferencePoint(p_stDNMMACDPID));
	}

	if
	(
		(!(M_uwMACDPIDGetFlags(p_stDNMMACDPID) & DNM_CAM_C_NoLinearParsing))
	||	(!(M_uwMACDPIDGetFlags(p_stDNMMACDPID) & DNM_CAM_C_NoAngularParsing))
	)
	{
		MTH3D_M_vSubVector(&stTempVector, DNM_M_p_stCPDGetPosition(_p_stExternData), &stPreviousPos);
		DNM_fn_vDynamicsTakeCareOfLastMove(&stTempVector, DNM_M_p_stReportGetLastMove(p_stReport), M_xMACDPIDGetCutAngleFactorPos(p_stDNMMACDPID));
		MTH3D_M_vAddVector(DNM_M_p_stCPDGetPosition(_p_stExternData), &stTempVector, &stPreviousPos);
	}

	if(M_uwMACDPIDGetFlags(p_stDNMMACDPID) & DNM_CAM_C_FixedOrientation)
	{

		POS_fn_vGetRotationMatrix(p_stGlobalMatrix, &stTempVector, &stYVector, &stTempVector);
		MTH3D_M_vNegVector(&stYVector, &stYVector);
		POS_fn_vGetTranslationVector(p_stGlobalMatrix, &stTempVector);

		MTH3D_M_vAddVector(DNM_M_p_stCPDGetTarget(_p_stExternData), &stYVector, &stTempVector);

		MTH3D_M_vNullVector(&stShiftingResult);
		MTH3D_M_vCopyVector(&stDNMTempVector, &stShiftingResult);
		DNM_M_vReportSetLastTgtMove(p_stReport, &stDNMTempVector);
	}

	else
	{

		MTH3D_M_vCopyVector(&stPreviousPos, DNM_M_p_stCPDGetTarget(_p_stExternData));

		MTH3D_M_vSubVector(&stShiftingSrc, M_p_stMACDPIDGetTarget(p_stDNMMACDPID), &stPreviousPos);

		xSpeed = M_xMACDPIDGetTargetSpeed(p_stDNMMACDPID);
		xSpeed = MTH_M_xMul(xSpeed, _xDT);

		MTH3D_M_vCopyVector(&stTempVector, DNM_M_p_stReportGetLastTgtMove(p_stReport));
		MTH3D_M_vCopyVector(&stTempVector1, DNM_M_p_stCPDGetTarget(_p_stExternData));
		DNM_fn_vDynamicsCameraPositionParsing
		(
			xSpeed,
			M_xMACDPIDGetTargetIncreaseSpeed(p_stDNMMACDPID),
			M_xMACDPIDGetTargetDecreaseSpeed(p_stDNMMACDPID),
			MTH3D_M_xNormVector(&stTempVector),
			(char) (M_uwMACDPIDGetFlags(p_stDNMMACDPID) & DNM_CAM_C_NoTargetParsing),
			(char) (M_uwMACDPIDGetFlags(p_stDNMMACDPID) & DNM_CAM_C_NoTargetInertia),
			&stPreviousPos,
			&stShiftingSrc,
			&stTempVector1,
			&stShiftingResult
		);

		DNM_fn_vDynamicsTakeCareOfLastMove(&stShiftingResult, DNM_M_p_stReportGetLastTgtMove(p_stReport), M_xMACDPIDGetCutAngleFactorTgt(p_stDNMMACDPID));
		MTH3D_M_vAddVector(DNM_M_p_stCPDGetTarget(_p_stExternData), &stShiftingResult, &stPreviousPos);
		DNM_M_vReportSetLastTgtMove(p_stReport, &stShiftingResult);
	}

	return _p_stDynamics;
}

/* 0x00434520 */
ACP_tdxBool __cdecl CAM_fn_bVectorIsNull (MTH3D_tdstVector *_p_stVector, MTH_tdxReal _xEpsilon)
{
	MTH_tdxReal xX, xY, xZ, xVal;

	xX = MTH_M_xAbs(MTH3D_M_xGetXofVector(_p_stVector));
	xY = MTH_M_xAbs(MTH3D_M_xGetYofVector(_p_stVector));
	xZ = MTH_M_xAbs(MTH3D_M_xGetZofVector(_p_stVector));

	xVal = MTH_M_xAdd(xX, MTH_M_xAdd(xY, xZ));

	return (MTH_M_bIsNullWithEpsilon(xVal, _xEpsilon));
}

char __cdecl MTH_bCompareVectors (MTH3D_tdstVector *_p_stFirst, MTH3D_tdstVector *_p_stSecond, MTH_tdxReal _xEpsilon)
{
	MTH3D_tdstVector stTempPoint;

	MTH3D_M_vCopyVector(&stTempPoint, _p_stFirst);
	MTH3D_M_vSubVector(&stTempPoint, &stTempPoint, _p_stSecond);
	return (char)CAM_fn_bVectorIsNull(&stTempPoint, _xEpsilon);
}

char __cdecl CAM_fn_cNormalHere (MTH3D_tdstVector *_p_stArray, MTH3D_tdstVector *_p_stVector, unsigned int uiNum)
{
	unsigned int uiIndex;

	for(uiIndex = 0; uiIndex < uiNum; uiIndex++)
	{
		if(MTH_M_bEqual((_p_stArray + uiIndex)->xX, _p_stVector->xX) &&
		   MTH_M_bEqual((_p_stArray + uiIndex)->xY, _p_stVector->xY) &&
		   MTH_M_bEqual((_p_stArray + uiIndex)->xZ, _p_stVector->xZ))
			return 1;
	}
	return 0;
}

static long CAM_lHaveITakeSThgIntoTheMug(
	DNM_tdstMecObstacle			*_p_stResultObstacle,
	HIE_tdstSuperObject *_p_stSupObj,
	POS_tdstCompletePosition	*_p_stStartPosition,
	POS_tdstCompletePosition	*_p_stEndPosition)
{
	long lCounter;
	long lCollision;
	DNM_tdstMecObstacle *p_stObstacle;
	DNM_tdstMecObstacle *p_stReturnObstacle;
	GMT_tdxHandleToGameMaterial hGameMat;
	GMT_tdxHandleToCollideMaterial hCollMat;
	GMT_tdxMask xCollMask;
	ACP_tdxBool bWater;
	char bLessDetected = 0;
	MTH3D_tdstVector stStartVec, stTempVec;

	POS_fn_vGetTranslationVector(_p_stStartPosition, &stStartVec);
	POS_fn_vGetTranslationVector(_p_stEndPosition, &stTempVec);

	lCollision = COL_lHaveITakeSThgIntoTheMug ( g_a_stObstacle, _p_stSupObj, _p_stStartPosition, _p_stEndPosition );

	for (lCounter=0; lCounter<lCollision; lCounter++)
	{
		bWater = FALSE;

		p_stObstacle = g_a_stObstacle+lCounter;

		hGameMat = DNM_M_hObstacleGetCollidedMaterial(p_stObstacle);
		if (GMT_fn_b_ulIsValid(hGameMat))
		{
			hCollMat = GMT_fn_hGetCollideMaterial(hGameMat);
			if (hCollMat != (GMT_tdxHandleToCollideMaterial)GMT_C_InvalidCollideMaterial)
			{
				xCollMask = GMT_fn_hGetCollideMaterialIdentifier(hCollMat);
				bWater = ((xCollMask & 0xC000) != 0);
			}
		}

		if (!bWater)
		{
			MTH3D_M_vSubVector(&stTempVec, DNM_M_p_stObstacleGetContact(p_stObstacle), &stStartVec);
			if(MTH_M_bLessEqual(MTH3D_M_xDotProductVector(&stTempVec, DNM_M_p_stObstacleGetNorm(p_stObstacle)), MTH_C_ZERO))
			{
				if(MTH_M_bGreaterEqual(DNM_M_xObstacleGetRate(p_stObstacle),MTH_C_ZERO))
				{
					if
					(

						MTH_M_bLessEqual(MTH3D_M_xDotProductVector(DNM_M_pObstacleGetZoneMove(p_stObstacle), DNM_M_p_stObstacleGetNorm(p_stObstacle)), MTH_M_xConstFloatToReal(-0.005f)
					))
					{
						*_p_stResultObstacle = *p_stObstacle;
						return 1;
					}
				}
				else
				{
					bLessDetected = 1;
					p_stReturnObstacle = p_stObstacle;
				}
			}
		}
	}

	if(bLessDetected)
	{
		*_p_stResultObstacle = *p_stReturnObstacle;
		return 1;
	}
	return 0;
}

#define CAM_C_MaxDNMNormals 5
DNM_tdstMecObstacle * __cdecl DNM_p_stObstacleCameraObstacle (
	DNM_tdstMecObstacle *_p_stObstacle,
	DNM_tdstDynamics *_p_stDynamics,
	DNM_tdstParsingDatas *_p_stExternData,
	HIE_tdstSuperObject *_hSupObj,
	MTH3D_tdstVector *_p_stWantedPosition,
	MTH3D_tdstVector *_p_stNewPosition)
{
	DNM_tdstReport *p_stReport;
	tdstMACDPID *p_stMACDPID;
	MTH_tdstRotation stRotation;
	long lNbCollisions;
	MTH_tdxReal xAngle, xNorm, xNormTempVector;
	MTH3D_tdstVector a_stNormals[CAM_C_MaxDNMNormals];
	MTH3D_tdstVector a_stCross[CAM_C_MaxDNMNormals];
	unsigned int uiNumNormals = 0, uiNumCross = 0;
	char cWantSecond, cNormalHere;

	MTH3D_tdstVector a_stVectors[11];
	MTH3D_tdstVector *p_stObstacleNorm = a_stVectors;
	MTH3D_tdstVector *p_stProjectionVector = a_stVectors + 1;
	MTH3D_tdstVector *p_stTempVector = a_stVectors + 2;
	MTH3D_tdstVector *p_stTempVector1 = a_stVectors + 3;
	MTH3D_tdstVector *p_stComputedCurrentPosition = a_stVectors + 4;
	MTH3D_tdstVector *p_stComputedWantedPosition = a_stVectors + 5;
	MTH3D_tdstVector *p_stLastMoveVector = a_stVectors + 6;
	MTH3D_tdstVector *p_stCurrentPosition = a_stVectors + 7;
	MTH3D_tdstVector *p_stImpactVector = a_stVectors + 8;
	MTH3D_tdstVector *p_stProjectedVector = a_stVectors + 9;
	MTH3D_tdstVector *p_stLastComputedCurrentPosition = a_stVectors + 10;

	POS_tdstCompletePosition stBeginMatrix;
	POS_tdstCompletePosition stEndMatrix;
	MTH3D_tdstVector stDNMTempVector;

	(void)_p_stExternData;

	p_stReport = DNM_M_p_stDynamicsGetReport(_p_stDynamics);
	p_stMACDPID = DNM_M_p_stDynamicsGetExternalDatas(_p_stDynamics);

	POS_fn_vGetTranslationVector(HIE_fn_hGetSuperObjectGlobalMatrix(_hSupObj), p_stCurrentPosition);

	MTH3D_M_vCopyVector(p_stComputedCurrentPosition, p_stCurrentPosition);
	MTH3D_M_vCopyVector(p_stComputedWantedPosition, _p_stWantedPosition);

	MTH3D_M_vCopyVector(p_stLastMoveVector, DNM_M_p_stReportGetLastMove(p_stReport));
	if (!MEC_fn_bIsNullVector(p_stLastMoveVector))
		MTH3D_M_vNormalizeVector(p_stLastMoveVector, p_stLastMoveVector);

	DNM_M_bReportSetCollisionFlagToFalse(p_stReport);
	DNM_M_ulReportSetSurfaceState(p_stReport, 0);
	cWantSecond = 0;

	if (M_uwMACDPIDGetFlags(p_stMACDPID) & DNM_CAM_C_NoObstacle)
	{
		MTH3D_M_vCopyVector(p_stComputedCurrentPosition, _p_stWantedPosition);
	}
	else
	{
		POS_fn_vSetIdentityMatrix(&stBeginMatrix);
		POS_fn_vSetIdentityMatrix(&stEndMatrix);

		if ( ! MTH_bCompareVectors(p_stComputedCurrentPosition, p_stComputedWantedPosition, (MTH_tdxReal)0.02f) ||
		     ! (M_uwMACDPIDGetFlags(p_stMACDPID) & DNM_CAM_C_NoCollisionWhenNotMoving) )
		{
			do
			{
				POS_fn_vSetTranslationVector(&stBeginMatrix, p_stComputedCurrentPosition);
				POS_fn_vSetTranslationVector(&stEndMatrix, p_stComputedWantedPosition);

				lNbCollisions = CAM_lHaveITakeSThgIntoTheMug(_p_stObstacle, _hSupObj, &stBeginMatrix, &stEndMatrix);

				if(lNbCollisions <= 0)
				{
					MTH3D_M_vCopyVector(p_stComputedCurrentPosition, p_stComputedWantedPosition);
					break;
				}

				DNM_M_ObstacleSetNorm(&p_stReport->m_stObstacle, DNM_M_p_stObstacleGetNorm(_p_stObstacle));
				MTH3D_M_vCopyVector(p_stObstacleNorm, DNM_M_p_stObstacleGetNorm(_p_stObstacle));

				MTH3D_M_vSubVector(p_stTempVector, _p_stWantedPosition, p_stCurrentPosition);
				xNormTempVector = MTH3D_M_xNormVector(p_stTempVector);

				if(!(DNM_M_bReportCollision(p_stReport)) && ! MTH_M_bIsNull(xNormTempVector))
				{
					DNM_M_bReportSetWallFlagToFalse(p_stReport);

					xAngle = MTH_M_xDiv(MTH3D_M_xDotProductVector(p_stTempVector, p_stObstacleNorm), xNormTempVector);
					if (MTH_M_bLess(xAngle, (MTH_tdxReal)-0.7f))
					{
						MTH3D_M_vSubVector(p_stTempVector1, M_p_stMACDPIDGetPosReferencePoint(p_stMACDPID), p_stCurrentPosition);
						MTH3D_M_vMulScalarVector(p_stTempVector, MTH_M_xInv(xNormTempVector), p_stTempVector);
						MTH3D_M_vNormalizeVector(p_stTempVector1, p_stTempVector1);
						xAngle = MTH3D_M_xDotProductVector(p_stTempVector, p_stTempVector1);
						if (MTH_M_bGreater(xAngle, (MTH_tdxReal)0.7f))
							DNM_M_bReportSetWallFlagToTrue(p_stReport);
					}
				}

				DNM_M_bReportSetCollisionFlagToTrue(p_stReport);
				DNM_M_ulReportSetSurfaceState(p_stReport, C_WOT_ulWall);

				if(MTH_M_bLess(DNM_M_xObstacleGetRate(_p_stObstacle), MTH_C_ZERO))
				{
					DNM_M_bReportSetCollisionFlagToTrue(p_stReport);
					xAngle = MTH3D_M_xNormVector(&_p_stObstacle->m_stTranslation);
					MTH3D_M_vMulScalarVector(&stDNMTempVector, MTH_M_xInv(xAngle), &_p_stObstacle->m_stTranslation);
					xAngle = MTH_M_xAdd(xAngle, (MTH_tdxReal)0.02f);
					MTH3D_M_vMulScalarVector(&stDNMTempVector, xAngle, &stDNMTempVector);
					MTH3D_M_vAddVector(p_stComputedCurrentPosition, &stDNMTempVector, p_stComputedWantedPosition);
					break;
				}

				MTH3D_M_vSubVector(p_stImpactVector, p_stComputedWantedPosition, p_stComputedCurrentPosition);
				MTH3D_M_vMulScalarVector(p_stImpactVector, DNM_M_xObstacleGetRate(_p_stObstacle), p_stImpactVector);
				MTH3D_M_vAddVector(p_stImpactVector, p_stImpactVector, p_stComputedCurrentPosition);

				cNormalHere = CAM_fn_cNormalHere(a_stNormals, p_stObstacleNorm, uiNumNormals);
				if(!cNormalHere)
				{
					if(uiNumNormals == CAM_C_MaxDNMNormals)
					{
						MTH3D_M_vCopyVector(p_stComputedCurrentPosition, p_stCurrentPosition);
						break;
					}

					MTH3D_M_vCopyVector(&a_stNormals[uiNumNormals], p_stObstacleNorm);
					uiNumNormals++;

					MTH3D_M_vSubVector(p_stProjectionVector, p_stImpactVector, DNM_M_p_stObstacleGetContact(_p_stObstacle));
					MTH3D_M_vNormalizeVector(p_stProjectionVector, p_stProjectionVector);

					MTH_p_stRotationComputeRotationWith2Vectors(&stRotation, p_stProjectionVector, p_stObstacleNorm);
					xAngle = MTH_M_xRotationGetAngle(&stRotation);
					if (MTH_M_bLess(xAngle, MTH_C_PiBy6))
						MTH3D_M_vCopyVector(p_stProjectionVector, p_stObstacleNorm);

					MTH3D_M_vSubVector(p_stTempVector, p_stImpactVector, p_stComputedCurrentPosition);
					if(!MEC_fn_bIsNullVector(p_stTempVector))
					{
						xNormTempVector = MTH3D_M_xNormVector(p_stTempVector);
						xNorm = MTH_M_xSub(xNormTempVector, (MTH_tdxReal)0.02f);
						if (MTH_M_bLessZero(xNorm))
						{	MTH3D_M_vNullVector(p_stTempVector);	}
						else
							MTH3D_M_vMulScalarVector(p_stTempVector, MTH_M_xDiv(xNorm, xNormTempVector), p_stTempVector);
						MTH3D_M_vCopyVector(p_stLastComputedCurrentPosition, p_stComputedCurrentPosition);
						MTH3D_M_vAddVector(p_stComputedCurrentPosition, p_stComputedCurrentPosition, p_stTempVector);
					}

					MTH3D_M_vSubVector(p_stProjectedVector, _p_stWantedPosition, p_stComputedCurrentPosition);
					MTH_fn_vVectorPlaneProjection(p_stProjectedVector, p_stProjectedVector, p_stProjectionVector, p_stProjectionVector);

					if((!MEC_fn_bIsNullVector(p_stLastMoveVector)) && (!MEC_fn_bIsNullVector(p_stProjectedVector)))
					{
						MTH3D_M_vNormalizeVector(p_stTempVector, p_stProjectedVector);
						MTH_p_stRotationComputeRotationWith2Vectors(&stRotation, p_stTempVector, p_stLastMoveVector);
						xAngle = MTH_M_xRotationGetAngle(&stRotation);
						if (MTH_M_bGreater(xAngle, MYH_C_PiMinusPiBy6))
							cWantSecond = 1;
					}
				}

				if((cWantSecond) || (cNormalHere))
				{
					if ((uiNumNormals <= 1)
					||  (MTH3D_M_bEqualVector(p_stObstacleNorm, &a_stNormals[uiNumNormals - 1]))
					||  (uiNumCross == CAM_C_MaxDNMNormals))
					{
						DNM_M_bReportSetWallFlagToTrue(p_stReport);
						break;
					}

					MTH3D_M_vCopyVector(p_stProjectedVector, _p_stWantedPosition);
					MTH3D_M_vSubVector(p_stProjectedVector, p_stProjectedVector, p_stComputedCurrentPosition);

					MTH3D_M_vCrossProductVector(p_stTempVector, &a_stNormals[uiNumNormals - 1], p_stObstacleNorm);
					MTH3D_M_vNormalizeVector(p_stTempVector, p_stTempVector);

					if(CAM_fn_cNormalHere(a_stCross, p_stTempVector, uiNumCross))
						break;

					MTH3D_M_vCopyVector(&a_stCross[uiNumCross], p_stTempVector);
					uiNumCross++;

					MTH_fn_vVectorPlaneProjection(p_stTempVector1, p_stProjectedVector, p_stTempVector, p_stTempVector);
					MTH3D_M_vSubVector(p_stProjectedVector, p_stProjectedVector, p_stTempVector1);
				}

				if (MTH3D_M_bIsNullVector(p_stProjectedVector))
				{
					MTH3D_M_vNullVector(p_stLastMoveVector);
				}
				else
				{
					MTH3D_M_vAddVector(p_stComputedWantedPosition, p_stComputedCurrentPosition, p_stProjectedVector);
					MTH3D_M_vNormalizeVector(p_stLastMoveVector, p_stProjectedVector);
				}

				if (MTH_bCompareVectors(p_stComputedCurrentPosition, p_stComputedWantedPosition, (MTH_tdxReal)0.02f))
					break;

			} while(1);
		}
	}

	MTH3D_M_vSubVector(p_stTempVector, p_stComputedCurrentPosition, p_stCurrentPosition);
	DNM_M_vReportSetLastMove(p_stReport, p_stTempVector);

	xAngle = MTH_C_ZERO;
	MTH3D_M_vSubVector(p_stTempVector, M_p_stMACDPIDGetPosReferencePoint(p_stMACDPID), p_stCurrentPosition);
	if(!MEC_fn_bIsNullVector(p_stTempVector))
	{
		MTH3D_M_vNormalizeVector(p_stTempVector, p_stTempVector);
		MTH3D_M_vSubVector(p_stTempVector1, M_p_stMACDPIDGetPosReferencePoint(p_stMACDPID), p_stComputedCurrentPosition);
		if(!MEC_fn_bIsNullVector(p_stTempVector1))
		{
			MTH3D_M_vNormalizeVector(p_stTempVector1, p_stTempVector1);
			MTH_p_stRotationComputeRotationWith2Vectors(&stRotation, p_stTempVector, p_stTempVector1);
			xAngle = MTH_M_xRotationGetAngle(&stRotation);
		}
	}
	DNM_M_xCPDGetLastAngleMoveCam(_p_stExternData) = xAngle;

	MTH3D_M_vCopyVector(_p_stNewPosition, p_stComputedCurrentPosition);

	(void)p_stLastComputedCurrentPosition;
	return _p_stObstacle;
}

typedef DNM_tdstDynamics * (__cdecl *t_DNM_p_stDynamicsCameraParsing)(
    DNM_tdstDynamics *, HIE_tdstSuperObject *, DNM_tdstParsingDatas *,
    void *, MTH_tdxReal);
/* 0x00434660 */
typedef DNM_tdstMecObstacle * (__cdecl *t_DNM_p_stObstacleCameraObstacle)(
    DNM_tdstMecObstacle *, DNM_tdstDynamics *, DNM_tdstParsingDatas *,
    HIE_tdstSuperObject *, MTH3D_tdstVector *, MTH3D_tdstVector *, MTH_tdxReal);
typedef DNM_tdstDynamics * (__cdecl *t_DNM_p_stDynamicsCameraUpdate)(
    DNM_tdstDynamics *, DNM_tdstParsingDatas *, HIE_tdstSuperObject *,
    DNM_tdstMecObstacle *, MTH3D_tdstVector *, MTH3D_tdstVector *);

#define DNM_RT_p_stDynamicsCameraParsing  ((t_DNM_p_stDynamicsCameraParsing)DNM_p_stDynamicsCameraParsing)

#define DNM_RT_p_stObstacleCameraObstacle ((t_DNM_p_stObstacleCameraObstacle)DNM_p_stObstacleCameraObstacle)

#define DNM_RT_p_stDynamicsCameraUpdate   ((t_DNM_p_stDynamicsCameraUpdate)DNM_p_stDynamicsCameraUpdate)

DNM_tdstDynamics * __cdecl DNM_p_stDynamicsCameraMechanics(
    DNM_tdstDynamics *_p_stDynamics,
    HIE_tdstSuperObject *_hSupObj,
    DNM_tdstParsingDatas *_p_stParsingDatas,
    void *_h_MecIdCard,
    MTH_tdxReal _xDT)
{
	DNM_tdstMecObstacle		stObstacle;
	MTH3D_tdstVector		stPosition, stTarget, stPosition1;

	DNM_p_stDynamicsCameraParsing(
		_p_stDynamics,
		_hSupObj,
		_p_stParsingDatas,
		_h_MecIdCard,
		_xDT
	);

	MTH3D_M_vCopyVector(&stPosition1, DNM_M_p_stCPDGetPosition(_p_stParsingDatas));
	if (DNM_RT_p_stObstacleCameraObstacle(
			&stObstacle,
			_p_stDynamics,
			_p_stParsingDatas,
			_hSupObj,
			&stPosition1,
			&stPosition,
			_xDT
			) == NULL)
		return NULL;

	MTH3D_M_vCopyVector(DNM_M_p_stCPDGetPosition(_p_stParsingDatas), &stPosition1);

	MTH3D_M_vCopyVector(&stTarget, DNM_M_p_stCPDGetTarget(_p_stParsingDatas));
	if (DNM_p_stDynamicsCameraUpdate(
			_p_stDynamics,
			_p_stParsingDatas,
			_hSupObj,
			&stObstacle,
			&stPosition,
			&stTarget
		) == NULL)
		return NULL;

	MTH3D_M_vCopyVector(DNM_M_p_stCPDGetTarget(_p_stParsingDatas), &stTarget);

	return _p_stDynamics;
}
