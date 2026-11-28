#include "ACP/ACP_types.h"
#include "DNM/DNM_types.h"
#include "POS/POS_types.h"
#include "MTH/MTH_flt.h"
#include "MTH/MTH3D.h"
#include "MEC/MEC.h"
#include "MEC/MECTools.h"
#include "GMT/GmtFunct.h"

#define MEC_M_hGetMechanicsMaterial(_hGameMat) \
    GMT_fn_hGetMechanicsMaterial((GMT_tdxHandleToGameMaterial)(_hGameMat))

#ifndef MTH_M_xDoubleToReal
#define MTH_M_xDoubleToReal(_x)     ((MTH_tdxReal)(_x))
#endif
#ifndef MTH_M_xConstFloatToReal
#define MTH_M_xConstFloatToReal(_x) ((MTH_tdxReal)(_x))
#endif

void __cdecl MEC_fn_vComputeSpeed (DNM_tdstDynamics *p_stDynamic, MTH_tdxReal xDT)
{
	MTH3D_tdstVector stSpeed;

	POS_tdxHandleToPosition hPreviousMatrix, hCurrentMatrix;
	MTH3D_tdstVector *p_stCurrentTranslation, *p_stPreviousTranslation;

	hPreviousMatrix = DNM_M_p_stDynamicsGetPreviousMatrix (p_stDynamic);
	hCurrentMatrix = DNM_M_p_stDynamicsGetCurrentMatrix (p_stDynamic);

	p_stCurrentTranslation = POS_fn_p_stGetTranslationVector ( hCurrentMatrix );
	p_stPreviousTranslation = POS_fn_p_stGetTranslationVector ( hPreviousMatrix );

	MTH3D_M_vSubVector (&stSpeed,p_stCurrentTranslation,p_stPreviousTranslation);

	MTH3D_M_vDivScalarVector (&stSpeed, &stSpeed, xDT);

	DNM_M_vDynamicsSetPreviousSpeed (p_stDynamic, &stSpeed);
}

void __cdecl MEC_fn_vInitMatrix (DNM_tdstDynamics *p_stDynamic)
{
	POS_tdxHandleToPosition hCurrentMatrix, hPreviousMatrix, hAbsolutePreviousMatrix;
	POS_tdxHandleToPosition hPrevPreviousMatrix;

	hCurrentMatrix = DNM_M_p_stDynamicsGetCurrentMatrix (p_stDynamic);
	hPreviousMatrix = DNM_M_p_stDynamicsGetPreviousMatrix (p_stDynamic);

	if (DNM_M_bDynamicsIsComplexSize(p_stDynamic))
	{
		hAbsolutePreviousMatrix = DNM_M_p_stDynamicsGetAbsolutePreviousMatrix (p_stDynamic);
		hPrevPreviousMatrix	= DNM_M_p_stDynamicsGetPrevPreviousMatrix (p_stDynamic);
		POS_fn_vCopyMatrix ( hPrevPreviousMatrix, hPreviousMatrix );
		POS_fn_vCopyMatrix ( hAbsolutePreviousMatrix, hCurrentMatrix );
	}

	POS_fn_vCopyMatrix ( hPreviousMatrix, hCurrentMatrix );
}

void __cdecl MEC_fn_vCorrectMatrix (DNM_tdstDynamics *p_stDynamic, POS_tdxHandleToPosition hMatrix)
{
	MTH3D_tdstVector stI, stJ, stK;
	POS_fn_vNormalizeMatrix (hMatrix);
	MTH3D_M_vSetVectorElements ( &stI, DNM_M_p_stDynamicsGetScaleX(p_stDynamic), MTH_C_ZERO, MTH_C_ZERO );
	MTH3D_M_vSetVectorElements ( &stJ, MTH_C_ZERO, DNM_M_p_stDynamicsGetScaleY(p_stDynamic), MTH_C_ZERO );
	MTH3D_M_vSetVectorElements ( &stK, MTH_C_ZERO, MTH_C_ZERO, DNM_M_p_stDynamicsGetScaleZ(p_stDynamic) );
	POS_fn_vSetScaleMatrix (hMatrix, &stI, &stJ, &stK);
	return;
}

ACP_tdxBool __cdecl MEC_fn_bIsEqualTransformMatrix (POS_tdxHandleToPosition hMatrix1, POS_tdxHandleToPosition hMatrix2);

ACP_tdxBool __cdecl MEC_fn_bIsEqualMatrix (POS_tdxHandleToPosition hMatrix1, POS_tdxHandleToPosition hMatrix2)
{
	MTH3D_tdstVector *p_stTranslation1, *p_stTranslation2;

	p_stTranslation1 = POS_fn_p_stGetTranslationVector ( hMatrix1 );
	p_stTranslation2 = POS_fn_p_stGetTranslationVector ( hMatrix2 );

	return MTH3D_M_bEqualVector (p_stTranslation1,p_stTranslation2) && MEC_fn_bIsEqualTransformMatrix (hMatrix1,hMatrix2);
}

ACP_tdxBool __cdecl MEC_fn_bIsEqualTransformMatrix (POS_tdxHandleToPosition hMatrix1, POS_tdxHandleToPosition hMatrix2)
{
	MTH3D_tdstVector stI1, stJ1, stK1;
	MTH3D_tdstVector stI2, stJ2, stK2;

	POS_fn_vGetTransformMatrix ( hMatrix1, &stI1, &stJ1, &stK1 );
	POS_fn_vGetTransformMatrix ( hMatrix2, &stI2, &stJ2, &stK2 );

	return MTH3D_M_bEqualVector (&stI1,&stI2) && MTH3D_M_bEqualVector (&stJ1,&stJ2) && MTH3D_M_bEqualVector (&stK1,&stK2);
}

void __cdecl MEC_fn_vTranslateMatrixOn (DNM_tdstDynamics *p_stDynamic, MTH3D_tdstVector *p_stMatrixTranslation)
{
	POS_tdxHandleToPosition hCurrentMatrix, hPreviousMatrix;
	MTH3D_tdstVector *p_stCurrentVector, *p_stPreviousVector;
	long lx, ly, lz;

	hCurrentMatrix = DNM_M_p_stDynamicsGetCurrentMatrix (p_stDynamic);
	hPreviousMatrix = DNM_M_p_stDynamicsGetPreviousMatrix (p_stDynamic);

	p_stCurrentVector = POS_fn_p_stGetTranslationVector ( hCurrentMatrix );
	p_stPreviousVector = POS_fn_p_stGetTranslationVector ( hPreviousMatrix );

	lx = MTH_M_xRealToLong(MTH3D_M_xGetXofVector(p_stPreviousVector));
	ly = MTH_M_xRealToLong(MTH3D_M_xGetYofVector(p_stPreviousVector));
	lz = MTH_M_xRealToLong(MTH3D_M_xGetZofVector(p_stPreviousVector));

	MTH3D_M_vSetXofVector ( p_stMatrixTranslation, MTH_M_xLongToReal(lx) );
	MTH3D_M_vSetYofVector ( p_stMatrixTranslation, MTH_M_xLongToReal(ly) );
	MTH3D_M_vSetZofVector ( p_stMatrixTranslation, MTH_M_xLongToReal(lz) );

	MTH3D_M_vSubVector ( p_stCurrentVector, p_stCurrentVector, p_stMatrixTranslation );
	MTH3D_M_vSubVector ( p_stPreviousVector, p_stPreviousVector, p_stMatrixTranslation );
}

void __cdecl MEC_fn_vSetInitialFrameTranslation ( DNM_tdstDynamics *p_stDynamic, MTH3D_tdstVector *p_stVector)
{
	POS_tdxHandleToPosition hCurrentMatrix;
	MTH3D_tdstVector *p_stCurrentPosition;

	hCurrentMatrix = DNM_M_p_stDynamicsGetCurrentMatrix (p_stDynamic);

	p_stCurrentPosition = POS_fn_p_stGetTranslationVector ( hCurrentMatrix );
	MTH3D_M_vAddVector (p_stCurrentPosition, p_stCurrentPosition, p_stVector);
}

void __cdecl MEC_fn_vTranslateMatrixOff (DNM_tdstDynamics *p_stDynamic, MTH3D_tdstVector *p_stMatrixTranslation)
{
	POS_tdxHandleToPosition hCurrentMatrix;
	MTH3D_tdstVector *p_stCurrentVector;

	hCurrentMatrix = DNM_M_p_stDynamicsGetCurrentMatrix (p_stDynamic);
	p_stCurrentVector = POS_fn_p_stGetTranslationVector ( hCurrentMatrix );
	MTH3D_M_vAddVector ( p_stCurrentVector, p_stCurrentVector, p_stMatrixTranslation );
}

void __cdecl MEC_fn_vComputeStream ( MTH3D_tdstVector *p_stStream, DNM_tdstDynamics *p_stDynamic, MTH_tdxReal xDT, ACP_tdxBool bSlide)
{
	MTH_tdxReal xMultFactor;

	xMultFactor = MTH_M_xMul (xDT,DNM_M_xDynamicsGetStreamFactor(p_stDynamic));
	MTH3D_M_vMulScalarVector ( p_stStream, xMultFactor, DNM_M_pDynamicsGetStreamSpeed ( p_stDynamic ) );
	if (bSlide)
		MTH3D_M_vMulScalarVector ( p_stStream, DNM_M_xDynamicsGetPreviousSlide(p_stDynamic), p_stStream );
}

void __cdecl MEC_fn_vComputeInertiaTranslation ( DNM_tdstDynamics *p_stDynamic, MTH_tdxReal xDT )
{
	MTH3D_tdstVector stVector;
	ACP_tdxBool bSlide;

	bSlide = (DNM_M_xDynamicsGetStreamPriority(p_stDynamic) != MTH_C_ONE);
	MEC_fn_vComputeStream ( &stVector, p_stDynamic, xDT, bSlide);

	MTH3D_M_vAddVector ( &stVector, &stVector, DNM_M_pDynamicsGetAddTranslation (p_stDynamic) );

	DNM_M_vDynamicsSetInertiaTranslation ( p_stDynamic, &stVector );
}

ACP_tdxBool __cdecl MEC_fn_bNoMove (DNM_tdstDynamics *p_stDynamic)
{
	if ( !MTH3D_M_bIsNullVector(DNM_M_pDynamicsGetPreviousSpeed(p_stDynamic)) ) return FALSE;
	if ( DNM_M_bDynamicsIsGravity(p_stDynamic) ) return FALSE;
	if ( DNM_M_bDynamicsIsAnimation(p_stDynamic) ) return FALSE;
	if ( DNM_M_bDynamicsIsImposeSpeed(p_stDynamic) ) return FALSE;
	if ( DNM_M_bDynamicsIsImposeRotation(p_stDynamic) ) return FALSE;
	if ( DNM_M_bDynamicsIsProposeSpeed(p_stDynamic) ) return FALSE;

	if (DNM_M_bDynamicsIsAdvancedSize(p_stDynamic))
	{
		if ( DNM_M_bDynamicsIsInertia(p_stDynamic) ) return FALSE;
		if ( DNM_M_bDynamicsIsImposeRotation(p_stDynamic) ) return FALSE;
		if ( DNM_M_bDynamicsIsStream(p_stDynamic) ) return FALSE;
		if ( DNM_M_bDynamicsIsAddSpeed(p_stDynamic) ) return FALSE;
	}

	if (DNM_M_bDynamicsIsComplexSize(p_stDynamic))
	{
		if ( MEC_fn_bIsPlatform(p_stDynamic) ) return FALSE;
	}

	return TRUE;
}

void __cdecl MEC_fn_vSlowMode (DNM_tdstDynamics *p_stDynamic, long lMode)
{
	POS_tdxHandleToPosition hPreviousMatrix, hCurrentMatrix;
	MTH3D_tdstVector *p_stPreviousPosition, *p_stCurrentPosition;
	MTH3D_tdstVector *p_stSpeed;
	MTH3D_tdstVector stVector;
	MTH_tdxReal xInvMode;

	hCurrentMatrix = DNM_M_p_stDynamicsGetCurrentMatrix (p_stDynamic);
	hPreviousMatrix = DNM_M_p_stDynamicsGetPreviousMatrix (p_stDynamic);
	p_stPreviousPosition = POS_fn_p_stGetTranslationVector ( hPreviousMatrix );
	p_stCurrentPosition = POS_fn_p_stGetTranslationVector ( hCurrentMatrix );
	xInvMode = MTH_M_xInv (MTH_M_xDoubleToReal(lMode));

	MTH3D_M_vSubVector ( &stVector, p_stCurrentPosition, p_stPreviousPosition );
	MTH3D_M_vMulScalarVector ( &stVector, xInvMode , &stVector );
	MTH3D_M_vAddVector ( p_stCurrentPosition, p_stPreviousPosition, &stVector );

	p_stSpeed = DNM_M_pDynamicsGetPreviousSpeed (p_stDynamic);
	MTH3D_M_vMulScalarVector ( p_stSpeed, xInvMode , p_stSpeed );
}

void __cdecl MEC_fn_vNormalMode (DNM_tdstDynamics *p_stDynamic, long lMode)
{
	POS_tdxHandleToPosition hPreviousMatrix, hCurrentMatrix;
	MTH3D_tdstVector *p_stPreviousPosition, *p_stCurrentPosition;
	MTH3D_tdstVector *p_stSpeed;
	MTH3D_tdstVector stVector;

	hCurrentMatrix = DNM_M_p_stDynamicsGetCurrentMatrix (p_stDynamic);
	hPreviousMatrix = DNM_M_p_stDynamicsGetPreviousMatrix (p_stDynamic);
	p_stPreviousPosition = POS_fn_p_stGetTranslationVector ( hPreviousMatrix );
	p_stCurrentPosition = POS_fn_p_stGetTranslationVector ( hCurrentMatrix );

	MTH3D_M_vSubVector ( &stVector, p_stCurrentPosition, p_stPreviousPosition );
	MTH3D_M_vMulScalarVector ( &stVector, MTH_M_xDoubleToReal(lMode) , &stVector );
	MTH3D_M_vSubVector ( p_stPreviousPosition, p_stCurrentPosition, &stVector );

	p_stSpeed = DNM_M_pDynamicsGetPreviousSpeed (p_stDynamic);
	MTH3D_M_vMulScalarVector ( p_stSpeed, MTH_M_xDoubleToReal(lMode) , p_stSpeed );
}

void __cdecl MEC_fn_vRepeatMove (DNM_tdstDynamics *p_stDynamic)
{
	POS_tdxHandleToPosition hPreviousMatrix, hCurrentMatrix;
	MTH3D_tdstVector *p_stPreviousPosition, *p_stCurrentPosition;
	MTH3D_tdstVector stVector;

	hCurrentMatrix = DNM_M_p_stDynamicsGetCurrentMatrix (p_stDynamic);
	hPreviousMatrix = DNM_M_p_stDynamicsGetPreviousMatrix (p_stDynamic);
	p_stPreviousPosition = POS_fn_p_stGetTranslationVector ( hPreviousMatrix );
	p_stCurrentPosition = POS_fn_p_stGetTranslationVector ( hCurrentMatrix );

	MTH3D_M_vSubVector ( &stVector, p_stCurrentPosition, p_stPreviousPosition );
	MTH3D_M_vCopyVector ( p_stPreviousPosition, p_stCurrentPosition );
	MTH3D_M_vAddVector ( p_stCurrentPosition, p_stCurrentPosition, &stVector );
}

DNM_tdstMecMatCharacteristics * __cdecl DNM_fn_p_stMecMatCharacteristicsGiveBackSemantic (DNM_tdxHandleToMecMatCharacteristics _hMatCharacteristics)
{
	return (DNM_tdstMecMatCharacteristics *)_hMatCharacteristics;
}

void __cdecl MEC_fn_vAdjustSpeed (DNM_tdstDynamics *p_stDynamic, MTH_tdxReal xDT)
{
	POS_tdxHandleToPosition hPreviousMatrix, hCurrentMatrix;
	MTH3D_tdstVector *p_stPreviousPosition, *p_stCurrentPosition;
	MTH3D_tdstVector stTranslation;
	MTH_tdxReal xDotProd;

	hCurrentMatrix = DNM_M_p_stDynamicsGetCurrentMatrix (p_stDynamic);
	hPreviousMatrix = DNM_M_p_stDynamicsGetPreviousMatrix (p_stDynamic);

	p_stPreviousPosition = POS_fn_p_stGetTranslationVector ( hPreviousMatrix );
	p_stCurrentPosition = POS_fn_p_stGetTranslationVector ( hCurrentMatrix );

	MTH3D_M_vSubVector ( &stTranslation, p_stCurrentPosition, p_stPreviousPosition);

	if (!MEC_fn_bIsNullVector(&stTranslation))
	{	MTH3D_M_vNormalizeVector (&stTranslation, &stTranslation);	}
	else
		MTH3D_M_vNullVector (&stTranslation);

	xDotProd = MTH3D_M_xDotProductVector (&stTranslation, DNM_M_pDynamicsGetPreviousSpeed(p_stDynamic));
	MTH3D_M_vMulScalarVector (&stTranslation, MTH_M_xMul(xDotProd, xDT), &stTranslation);

	MTH3D_M_vAddVector (p_stCurrentPosition, p_stPreviousPosition, &stTranslation);
}

void __cdecl DNM_fn_vInvalidateMatCharacteristics (DNM_tdxHandleToMecMatCharacteristics *_p_hMatCharacteristics)
{
	*_p_hMatCharacteristics = NULL;
}

ACP_tdxBool __cdecl DNM_fn_bIsMatCharacteristicsValid (DNM_tdxHandleToMecMatCharacteristics _hMatCharacteristics)
{
	return (ACP_tdxBool)(_hMatCharacteristics != NULL);
}

ACP_tdxBool __cdecl MEC_fn_bWallNormal (DNM_tdstDynamics *p_stDynamic, MTH3D_tdstVector *p_stNormal)
{
	MTH_tdxReal xDotProduct;
	MTH3D_tdstVector stVerticalVector;

	MTH3D_M_vSetVectorElements (&stVerticalVector, MTH_C_ZERO, MTH_C_ZERO, MTH_C_ONE);
	xDotProduct = MTH3D_M_xDotProductVector ( p_stNormal, &stVerticalVector );
	if ( (xDotProduct > MEC_fn_xWallLimit(p_stDynamic)) && (xDotProduct < MEC_fn_xGroundLimit(p_stDynamic)) )
		return (ACP_tdxBool)1;
	else
		return (ACP_tdxBool)0;
}

ACP_tdxBool __cdecl MEC_fn_bGroundNormal (DNM_tdstDynamics *p_stDynamic, MTH3D_tdstVector *p_stNormal)
{
	MTH_tdxReal xDotProduct;
	MTH3D_tdstVector stVerticalVector;

	MTH3D_M_vSetVectorElements (&stVerticalVector, MTH_C_ZERO, MTH_C_ZERO, MTH_C_ONE);
	xDotProduct = MTH3D_M_xDotProductVector ( p_stNormal, &stVerticalVector );
	if (xDotProduct >= MEC_fn_xGroundLimit(p_stDynamic))
		return (ACP_tdxBool)1;
	else
		return (ACP_tdxBool)0;
}

ACP_tdxBool __cdecl MEC_fn_bCeilObstacle (DNM_tdstDynamics *p_stDynamic, DNM_tdstMecObstacle *p_stObstacle)
{
	MTH_tdxReal xDotProduct;
	MTH3D_tdstVector stVerticalVector;
	MTH3D_tdstVector *p_stNormalVector;

	MTH3D_M_vSetVectorElements (&stVerticalVector, MTH_C_ZERO, MTH_C_ZERO, MTH_C_ONE);
	p_stNormalVector = DNM_M_p_stObstacleGetNorm (p_stObstacle);

	xDotProduct = MTH3D_M_xDotProductVector ( p_stNormalVector, &stVerticalVector );
	/* 0x435F10 */
	if (xDotProduct < MEC_fn_xWallLimit(p_stDynamic))
		return (ACP_tdxBool)1;
	else
		return (ACP_tdxBool)0;
}

ACP_tdxBool __cdecl MEC_fn_bDynamicsNeedCollide (DNM_tdstDynamics *p_stDynamic)
{
	POS_tdxHandleToPosition hCurrentMatrix, hPreviousMatrix;
	MTH3D_tdstVector *p_stCurrentPosition, *p_stPreviousPosition;
	MTH3D_tdstVector *p_stCurrentI, *p_stCurrentJ, *p_stCurrentK;
	MTH3D_tdstVector *p_stPreviousI, *p_stPreviousJ, *p_stPreviousK;

	hCurrentMatrix = DNM_M_p_stDynamicsGetCurrentMatrix (p_stDynamic);
	hPreviousMatrix = DNM_M_p_stDynamicsGetPreviousMatrix (p_stDynamic);
	p_stCurrentPosition = POS_fn_p_stGetTranslationVector ( hCurrentMatrix );
	p_stPreviousPosition = POS_fn_p_stGetTranslationVector ( hPreviousMatrix );
	POS_fn_vGetRotationVector ( hCurrentMatrix, &p_stCurrentI, &p_stCurrentJ, &p_stCurrentK );
	POS_fn_vGetRotationVector ( hPreviousMatrix, &p_stPreviousI, &p_stPreviousJ, &p_stPreviousK );

	return
		!MTH3D_M_bEqualVector (p_stCurrentPosition,p_stPreviousPosition)
	||	!MTH3D_M_bEqualVector (p_stCurrentI,p_stPreviousI)
	||	!MTH3D_M_bEqualVector (p_stCurrentJ,p_stPreviousJ)
	||	!MTH3D_M_bEqualVector (p_stCurrentK,p_stPreviousK);
}

#define C_xDNM_SLIDE_TYPE_GAME_MATERIAL   (1 << 0)

MTH_tdxReal __cdecl MEC_fn_xGetSlide (DNM_tdstDynamics *p_stDynamic, DNM_tdstMecObstacle *p_stObstacle)
{
	MTH_tdxReal xSlide;
	GMT_tdxHandleToGameMaterial          hGameMaterial;
	DNM_tdxHandleToMecMatCharacteristics hMatCharacteristics;

	hGameMaterial = NULL;
	hMatCharacteristics = NULL;

	if (p_stObstacle == NULL)
		return MTH_C_ZERO;

	hGameMaterial = (GMT_tdxHandleToGameMaterial)DNM_M_hObstacleGetCollidedMaterial (p_stObstacle);
	if (hGameMaterial != NULL)
		hMatCharacteristics = MEC_M_hGetMechanicsMaterial(hGameMaterial);

	xSlide = MTH_C_ZERO;

	if (hGameMaterial && MEC_fn_bTypeOfGameMaterial (hGameMaterial, C_xDNM_SLIDE_TYPE_GAME_MATERIAL))
		xSlide = MTH_C_ONE;

	if (hMatCharacteristics)
		xSlide = DNM_M_xObstacleGetSlide (p_stObstacle);

	if (DNM_M_bDynamicsIsCollisionControl(p_stDynamic))
		xSlide = DNM_M_xDynamicsGetSlide (p_stDynamic);

	return xSlide;
}

#define C_xDNM_REBOUND_TYPE_GAME_MATERIAL  (1 << 1)

MTH_tdxReal __cdecl MEC_fn_xGetRebound (DNM_tdstDynamics *p_stDynamic, DNM_tdstMecObstacle *p_stObstacle)
{
	MTH_tdxReal xRebound;
	GMT_tdxHandleToGameMaterial			 hGameMaterial;
	DNM_tdxHandleToMecMatCharacteristics hMatCharacteristics;

	hGameMaterial = hMatCharacteristics = NULL;

	if (p_stObstacle == NULL)
		return MTH_C_ZERO;

	hGameMaterial = (GMT_tdxHandleToGameMaterial)DNM_M_hObstacleGetCollidedMaterial (p_stObstacle);
	if (hGameMaterial != NULL)
		hMatCharacteristics = MEC_M_hGetMechanicsMaterial(hGameMaterial);

	xRebound = MTH_C_ZERO;

	if (hGameMaterial && MEC_fn_bTypeOfGameMaterial (hGameMaterial,C_xDNM_REBOUND_TYPE_GAME_MATERIAL))
		xRebound = MTH_C_ONE;

	if (hMatCharacteristics)
		xRebound = DNM_M_xObstacleGetRebound (p_stObstacle);

	if (DNM_M_bDynamicsIsCollisionControl(p_stDynamic))
		xRebound = DNM_M_xDynamicsGetRebound (p_stDynamic);

	return xRebound;
}

void __cdecl MEC_fn_vComputeCollisionTranslation ( long lObstacle, DNM_tdstMecObstacle *p_stObstacle, MTH3D_tdstVector *p_stMove )
{
	long lCounter;
	MTH_tdxReal xRate;
	DNM_tdstMecObstacle *p_stTmpObstacle;
	MTH3D_tdstVector stVector;

	for (lCounter=0; lCounter<lObstacle; lCounter++)
	{
		p_stTmpObstacle = p_stObstacle+lCounter;
		if (MEC_fn_bIsObstacleValid(p_stTmpObstacle))
		{
			xRate = DNM_M_xObstacleGetRate(p_stTmpObstacle);
			if (MTH_M_bGreater(xRate, MTH_C_ONE))
			{
				DNM_M_xObstacleSetRate (p_stTmpObstacle, MTH_C_ONE);

				MTH3D_M_vNormalizeVector (&stVector, DNM_M_pObstacleGetZoneMove(p_stTmpObstacle));
				MTH3D_M_vMulScalarVector (&stVector, DNM_M_xObstacleGetZoneRadius(p_stTmpObstacle), &stVector);
				MTH3D_M_vAddVector (&stVector, &stVector, DNM_M_pObstacleGetZonePosition(p_stTmpObstacle));
				DNM_M_ObstacleSetContact (p_stTmpObstacle, &stVector);
			}
			else if (MTH_M_bGreaterEqual(xRate, MTH_C_ZERO))

				{
					DNM_M_xObstacleGetTranslation (p_stTmpObstacle)->xX = MTH_M_xMul(p_stMove->xX, MTH_M_xSub(MTH_C_ONE,DNM_M_xObstacleGetRate(p_stTmpObstacle)));
					DNM_M_xObstacleGetTranslation (p_stTmpObstacle)->xY = MTH_M_xMul(p_stMove->xY, MTH_M_xSub(MTH_C_ONE,DNM_M_xObstacleGetRate(p_stTmpObstacle)));
					DNM_M_xObstacleGetTranslation (p_stTmpObstacle)->xZ = MTH_M_xMul(p_stMove->xZ, MTH_M_xSub(MTH_C_ONE,DNM_M_xObstacleGetRate(p_stTmpObstacle)));
				}
		}
	}
}

void __cdecl MEC_fn_vAdjustScaleVector (DNM_tdstDynamics *p_stDynamic, MTH3D_tdstVector *p_stVector)
{
	MTH3D_M_vSetXofVector ( p_stVector, MTH_M_xMul(MTH3D_M_xGetXofVector (p_stVector),DNM_M_p_stDynamicsGetScaleX(p_stDynamic)) );
	MTH3D_M_vSetYofVector ( p_stVector, MTH_M_xMul(MTH3D_M_xGetYofVector (p_stVector),DNM_M_p_stDynamicsGetScaleY(p_stDynamic)) );
	MTH3D_M_vSetZofVector ( p_stVector, MTH_M_xMul(MTH3D_M_xGetZofVector (p_stVector),DNM_M_p_stDynamicsGetScaleZ(p_stDynamic)) );
}

MTH_tdxReal __cdecl MEC_fn_vRoundReal (MTH_tdxReal xReal, MTH_tdxReal xEpsilon)
{
	long lValue;
	MTH_tdxReal xValue;

	xValue = MTH_M_xDiv (xReal, xEpsilon);
	lValue = MTH_M_xRealToLong (xValue);
	xReal = MTH_M_xLongToReal (lValue);
	return MTH_M_xMul (xReal, xEpsilon);
}

void __cdecl MEC_fn_vRoundVector (MTH3D_tdstVector *p_stVector, MTH_tdxReal xEpsilon)
{
	MTH3D_M_vSetXofVector ( p_stVector, MEC_fn_vRoundReal ( MTH3D_M_xGetXofVector(p_stVector), xEpsilon ) );
	MTH3D_M_vSetYofVector ( p_stVector, MEC_fn_vRoundReal ( MTH3D_M_xGetYofVector(p_stVector), xEpsilon ) );
	MTH3D_M_vSetZofVector ( p_stVector, MEC_fn_vRoundReal ( MTH3D_M_xGetZofVector(p_stVector), xEpsilon ) );
}

void __cdecl MEC_fn_vDeleteBaseBackFace ( long lObstacle, DNM_tdstMecObstacle *p_stObstacle, DNM_tdstDynamics *p_stDynamic )
{
	MTH3D_tdstVector  stVector;
	long lCounter;
	MTH_tdxReal xDotProd1, xDotProd2;

	for (lCounter = 0; lCounter < lObstacle; lCounter++)
	{
		DNM_tdstMecObstacle *p_stCurrentObstacle = p_stObstacle + lCounter;

		if ( MEC_fn_bIsObstacleValid(p_stCurrentObstacle) && !MEC_fn_bIsWater(p_stCurrentObstacle) )
		{
			if (!MEC_fn_bIsNullVector(DNM_M_pObstacleGetZoneMove(p_stCurrentObstacle)))
			{	MTH3D_M_vNormalizeVector (&stVector, DNM_M_pObstacleGetZoneMove(p_stCurrentObstacle));	}
			else
				MTH3D_M_vNullVector (&stVector);

			xDotProd2 = MTH3D_M_xDotProductVector ( DNM_M_p_stObstacleGetNorm(p_stCurrentObstacle), &stVector );

			/* 0x49C768 */
			if ( MTH_M_bGreater(xDotProd2, MTH_M_xConstFloatToReal(0.707f)) )
				MEC_fn_vInvalidateObstacle (p_stCurrentObstacle);
			else if (!DNM_M_bDynamicsIsHanging(p_stDynamic))
			{
				if (MTH_M_bEqualZero(DNM_M_xObstacleGetZoneRadius(p_stCurrentObstacle)))
					MEC_fn_vInvalidateObstacle(p_stCurrentObstacle);
				else
				{
					MTH3D_M_vSubVector ( &stVector, DNM_M_p_stObstacleGetContact(p_stCurrentObstacle), DNM_M_pObstacleGetZonePosition(p_stCurrentObstacle) );
					MTH3D_M_vAddVector ( &stVector, &stVector, DNM_M_pObstacleGetZoneMove(p_stCurrentObstacle) );
					xDotProd1 = MTH3D_M_xDotProductVector ( DNM_M_p_stObstacleGetNorm(p_stCurrentObstacle),&stVector );

					/* 0x49C738 */
					if (MTH_M_bGreater(xDotProd1, MTH_C_ZERO))
						MEC_fn_vInvalidateObstacle(p_stCurrentObstacle);
				}
			}
		}
	}
}

#ifndef C_xDNM_NO_COLLISION_WITH_GROUND
#define C_xDNM_NO_COLLISION_WITH_GROUND  (1 << 4)
#endif

void __cdecl MEC_fn_vDeleteNoCollidedGroundFace ( long lObstacle, DNM_tdstMecObstacle *p_stObstacle, DNM_tdstDynamics *p_stDynamic )
{
	long lCounter;
	DNM_tdstMecObstacle *p_stCurrentObstacle;

	for (lCounter = 0; lCounter < lObstacle; lCounter++)
	{
		p_stCurrentObstacle = p_stObstacle + lCounter;

		if (MEC_fn_bIsObstacleValid(p_stCurrentObstacle))
		{
			if (MEC_fn_bGroundObstacle(p_stDynamic, p_stCurrentObstacle))
			{
				if (MEC_fn_bTypeOfGameMaterial((GMT_tdxHandleToGameMaterial)DNM_M_hObstacleGetMyMaterial(p_stCurrentObstacle), C_xDNM_NO_COLLISION_WITH_GROUND))
					MEC_fn_vInvalidateObstacle(p_stCurrentObstacle);
			}
		}
	}
}

HIE_tdstSuperObject *__cdecl MEC_fn_hGetFatherActor (HIE_tdstSuperObject *hSupObj)
{
	HIE_tdstSuperObject *hActor = hSupObj;

	while (hActor->ulTypeOfLinkedObject != HIE_C_ulActor)
	{
		hActor = HIE_fn_hGetSuperObjectFather (hActor);
		if (hActor == NULL)
			return hActor;
	}
	return hActor;
}

#pragma optimize("y", off)
void __cdecl MEC_fn_vDeleteHangingObstacle ( long lObstacle, DNM_tdstMecObstacle *p_stObstacle, DNM_tdstDynamics *p_stDynamic )
{
	long lCounter;
	MTH3D_tdstVector  stVector;
	DNM_tdstMecObstacle *p_stTmpObstacle;
	DNM_tdstMecObstacle *p_stGroundObstacle = NULL;
	MTH_tdxReal xZ = MTH_C_InfinitPlus;
	MTH_tdxReal xHandZPosition;

	for (lCounter=0; lCounter<lObstacle; lCounter++)
	{
		p_stTmpObstacle = p_stObstacle + lCounter;
		if (MEC_fn_bIsObstacleValid(p_stTmpObstacle)&&MEC_fn_bGroundObstacle(p_stDynamic,p_stTmpObstacle))
		{
			if ( MTH_M_bLess ( MTH3D_M_xGetZofVector(DNM_M_p_stObstacleGetContact(p_stTmpObstacle)), xZ ) )
			{
				p_stGroundObstacle = p_stTmpObstacle;
				xZ = MTH3D_M_xGetZofVector(DNM_M_p_stObstacleGetContact(p_stTmpObstacle));
			}
		}
	}

	if (MEC_fn_bIsPlatform(p_stDynamic))
		xHandZPosition = MTH_M_xAdd ( DNM_M_xDynamicsGetHangingLimit(p_stDynamic), MTH3D_M_xGetZofVector(POS_fn_p_stGetTranslationVector(HIE_fn_hGetSuperObjectGlobalMatrix(DNM_M_hDynamicsGetPlatformSO(p_stDynamic)))) );
	else
		xHandZPosition = DNM_M_xDynamicsGetHangingLimit(p_stDynamic);

	for (lCounter=0; lCounter<lObstacle; lCounter++)
	{
		p_stTmpObstacle = p_stObstacle + lCounter;
		if (MEC_fn_bIsObstacleValid(p_stTmpObstacle))
		{
			if ( !MEC_fn_bGroundObstacle(p_stDynamic,p_stTmpObstacle) && MTH_M_bLessEqual(DNM_M_xObstacleGetRate(p_stTmpObstacle),MTH_C_ONE) && !MEC_fn_bIsEdge(p_stTmpObstacle) )
			{
				if (p_stGroundObstacle)
				{
					MTH3D_M_vSubVector ( &stVector, DNM_M_p_stObstacleGetContact(p_stTmpObstacle), DNM_M_p_stObstacleGetContact(p_stGroundObstacle) );
					if (MTH_M_bLess ( MTH3D_M_xDotProductVector(DNM_M_p_stObstacleGetNorm(p_stGroundObstacle),&stVector), MTH_M_xConstFloatToReal(0.005f) ))
						MEC_fn_vInvalidateObstacle(p_stTmpObstacle);
				}
				else
				{
					if (MTH_M_bLessEqual(MTH3D_M_xGetZofVector(DNM_M_p_stObstacleGetContact(p_stTmpObstacle)), xHandZPosition ))
						MEC_fn_vInvalidateObstacle(p_stTmpObstacle);
				}
			}
			else
				MEC_fn_vInvalidateObstacle(p_stTmpObstacle);
		}
	}
}
#pragma optimize("", on)

#ifndef C_xDNM_GI_TYPE_GAME_MATERIAL
#define C_xDNM_GI_TYPE_GAME_MATERIAL  (1 << 5)
#endif

void __cdecl MEC_fn_vDeleteGiBackFace ( long lObstacle, DNM_tdstMecObstacle *p_stObstacle, DNM_tdstDynamics *p_stDynamic, MTH3D_tdstVector *p_stVector )
{
	long lCounter;
	MTH_tdxReal xDotProd;
	MTH3D_tdstVector stVerticalVector;

	MTH3D_M_vSetBaseKVector (&stVerticalVector);

	for (lCounter=0; lCounter<lObstacle; lCounter++)
	{
		DNM_tdstMecObstacle *p_stCurrentObstacle = p_stObstacle + lCounter;

		if (MEC_fn_bIsObstacleValid(p_stCurrentObstacle))
		{
			if ( MEC_fn_bTypeOfGameMaterial(DNM_M_hObstacleGetCollidedMaterial (p_stCurrentObstacle),C_xDNM_GI_TYPE_GAME_MATERIAL) && !MEC_fn_bTypeOfGameMaterial(DNM_M_hObstacleGetCollidedMaterial (p_stCurrentObstacle),C_xDNM_NO_COLLISION_TYPE_GAME_MATERIAL)  )
			{
				xDotProd = MTH3D_M_xDotProductVector ( DNM_M_p_stObstacleGetNorm(p_stCurrentObstacle),&stVerticalVector );

				if ( MTH_M_bGreater(xDotProd,MTH_C_ZERO) )
					MEC_fn_vInvalidateObstacle(p_stCurrentObstacle);
			}
			else if (MEC_fn_bCeilObstacle(p_stDynamic,p_stCurrentObstacle))
			{
				MEC_fn_vInvalidateObstacle(p_stCurrentObstacle);
			}
			else if (MEC_fn_bWallObstacle(p_stDynamic,p_stCurrentObstacle))
			{
				xDotProd = MTH3D_M_xDotProductVector ( DNM_M_p_stObstacleGetNorm(p_stCurrentObstacle), p_stVector );
				if ( MTH_M_bGreater(xDotProd,MTH_C_ZERO) )
					MEC_fn_vInvalidateObstacle(p_stCurrentObstacle);
			}
		}
	}
}

#ifndef C_xDNM_CLIMB_TYPE_GAME_MATERIAL
#define C_xDNM_CLIMB_TYPE_GAME_MATERIAL  (1 << 6)
#endif
#ifndef C_xDNM_MTH_EPSILON
#define C_xDNM_MTH_EPSILON               MTH_M_xConstFloatToReal(1.0e-3f)
#endif

void __cdecl MEC_fn_vDeleteClimbBackFace ( long lObstacle, DNM_tdstMecObstacle *p_stObstacle, DNM_tdstDynamics *p_stDynamic, MTH3D_tdstVector *p_stVector )
{
	long lCounter;
	MTH_tdxReal xDotProd;

	for (lCounter=0; lCounter<lObstacle; lCounter++)
	{
		DNM_tdstMecObstacle *p_stCurrentObstacle = p_stObstacle + lCounter;

		if (MEC_fn_bIsObstacleValid(p_stCurrentObstacle))
		{
			POS_tdxHandleToPosition hCurrentMatrix;
			MTH3D_tdstVector *p_stX, *p_stY, *p_stZ;

			hCurrentMatrix = DNM_M_p_stDynamicsGetCurrentMatrix (p_stDynamic);
			POS_fn_vGetRotationVector ( hCurrentMatrix, &p_stX, &p_stY, &p_stZ );

			xDotProd = MTH3D_M_xDotProductVector ( DNM_M_p_stObstacleGetNorm(p_stCurrentObstacle), p_stY );

			if ( MEC_fn_bTypeOfGameMaterial(DNM_M_hObstacleGetCollidedMaterial (p_stCurrentObstacle),C_xDNM_CLIMB_TYPE_GAME_MATERIAL) && !MEC_fn_bTypeOfGameMaterial(DNM_M_hObstacleGetCollidedMaterial (p_stCurrentObstacle),C_xDNM_NO_COLLISION_TYPE_GAME_MATERIAL) )
			{
				/* 0x49C760 */
				if ( MTH_M_bLess(xDotProd,MTH_M_xConstFloatToReal(-0.707f)) )
					MEC_fn_vInvalidateObstacle(p_stCurrentObstacle);
			}
			else
			{
				if ( MTH_M_bLess(xDotProd,MTH_M_xConstFloatToReal(-0.95f)) )
					MEC_fn_vInvalidateObstacle(p_stCurrentObstacle);
				else
				{
					xDotProd = MTH3D_M_xDotProductVector ( DNM_M_p_stObstacleGetNorm(p_stCurrentObstacle),p_stVector );

					if ( MTH_M_bGreater(xDotProd,C_xDNM_MTH_EPSILON) )
						MEC_fn_vInvalidateObstacle(p_stCurrentObstacle);
				}
			}

		}
	}
}

void __cdecl MEC_fn_vDeleteNullTranslationObstacle ( long lObstacle, DNM_tdstMecObstacle *a_stObstacle )
{
	long lCounter;
	DNM_tdstMecObstacle *p_stTmpObstacle;

	for (lCounter = 0; lCounter < lObstacle; lCounter++)
	{
		p_stTmpObstacle = a_stObstacle + lCounter;
		if ( MEC_fn_bIsObstacleValid(p_stTmpObstacle) && MEC_fn_bIsNullVector(DNM_M_xObstacleGetTranslation(p_stTmpObstacle)) )
			MEC_fn_vInvalidateObstacle(p_stTmpObstacle);
	}
}

#pragma optimize("y", off)
void __cdecl MEC_fn_vDeleteUnderGroundWall ( long lObstacle, DNM_tdstMecObstacle *p_stObstacle, DNM_tdstDynamics *p_stDynamic, DNM_tdstMecObstacle *p_stGroundObstacle )
{
	long lCounter;
	DNM_tdstMecObstacle *p_stTmpObstacle, *p_stGroundEdge;
	MTH3D_tdstVector *p_stNormal;
	MTH3D_tdstVector stContact, stVector;
	MTH_tdxReal xDotProd;

	if (!p_stGroundObstacle) return;

	p_stGroundEdge = NULL;
	for (lCounter=0; lCounter<lObstacle && !p_stGroundEdge; lCounter++)
	{
		p_stTmpObstacle = p_stObstacle+lCounter;

		if (MEC_fn_bIsObstacleValid(p_stTmpObstacle))
		{
			if
			(
				MEC_fn_bGroundObstacle (p_stDynamic,p_stTmpObstacle)
			&&	MEC_fn_bIsEdge (p_stTmpObstacle)
			&&	MTH_M_bLessEqual(MTH3D_M_xGetZofVector(DNM_M_p_stObstacleGetContact(p_stTmpObstacle)),MTH3D_M_xGetZofVector(DNM_M_p_stObstacleGetContact(p_stGroundObstacle)))
			)
			p_stGroundEdge = p_stTmpObstacle;
		}
	}

	p_stNormal = DNM_M_p_stObstacleGetNorm (p_stGroundObstacle);
	MTH3D_M_vCopyVector ( &stContact,DNM_M_p_stObstacleGetContact (p_stGroundEdge ? p_stGroundEdge : p_stGroundObstacle) );

	for (lCounter=0; lCounter<lObstacle; lCounter++)
	{
		p_stTmpObstacle = p_stObstacle+lCounter;

		if (MEC_fn_bIsObstacleValid(p_stTmpObstacle))
		{

			if (MEC_fn_bWallObstacle(p_stDynamic,p_stTmpObstacle))
			{
				MTH3D_M_vSubVector ( &stVector, DNM_M_p_stObstacleGetContact(p_stTmpObstacle), &stContact);
				xDotProd = MTH3D_M_xDotProductVector (&stVector,p_stNormal);

				if ( MTH_M_bLessEqualZero ( xDotProd ) )
					MEC_fn_vInvalidateObstacle(p_stTmpObstacle);
			}
		}
	}
}
#pragma optimize("", on)

/* 0x00413700 */
extern unsigned long __cdecl fn_ulStandardGameGetCustomBitsSO (void *hSupObj);

#ifndef C_ulDNM_RAYMAN_CUSTOM_BIT
#define C_ulDNM_RAYMAN_CUSTOM_BIT  0x80000000UL
#endif

#ifndef C_ulDNM_NO_CRASH_CUSTOM_BIT
#define C_ulDNM_NO_CRASH_CUSTOM_BIT  0x40UL
#endif
#ifndef HIE_fn_bIsSuperObjectValid
#define HIE_fn_bIsSuperObjectValid(_hSO)  ((_hSO) != (HIE_tdstSuperObject *)0)
#endif

ACP_tdxBool __cdecl MEC_fn_bCanCrashActor (DNM_tdstMecObstacle *p_stCollideObstacle)
{
	HIE_tdstSuperObject *hSupObj;

	hSupObj = (HIE_tdstSuperObject *)DNM_M_p_stObstacleGetObject (p_stCollideObstacle);
	if (!HIE_fn_bIsSuperObjectValid(hSupObj))
		return FALSE;
	hSupObj = MEC_fn_hGetFatherActor (hSupObj);
	return (ACP_tdxBool)((fn_ulStandardGameGetCustomBitsSO(hSupObj) & C_ulDNM_NO_CRASH_CUSTOM_BIT) != C_ulDNM_NO_CRASH_CUSTOM_BIT);
}

void __cdecl MEC_fn_vDeleteWallEdge ( long lObstacle, DNM_tdstMecObstacle *p_stObstacle, DNM_tdstDynamics *p_stDynamic )
{
	long lCounter;
	long lCounter1;
	DNM_tdstMecObstacle *p_stTmpObstacle;
	DNM_tdstMecObstacle *p_stTmpObstacle1;
	MTH3D_tdstVector stVector;
	ACP_tdxBool bRemoveEdge = TRUE;

	for (lCounter=0; lCounter<lObstacle; lCounter++)
	{
		p_stTmpObstacle = p_stObstacle + lCounter;
		if (MEC_fn_bIsObstacleValid(p_stTmpObstacle))
		{
			if (MEC_fn_bWallObstacle(p_stDynamic,p_stTmpObstacle))
			{
				if ( (DNM_M_xObstacleGetCollidedEntity(p_stTmpObstacle)==COL_C_xLowEdgeEntity) )
				{
					if (!MEC_fn_bIsDoubleEdge(p_stTmpObstacle))
					{
						for (lCounter1=0; lCounter1<lObstacle; lCounter1++)
						{
							if (lCounter1!=lCounter)
							{
								p_stTmpObstacle1 = p_stObstacle + lCounter1;
								if (MEC_fn_bIsObstacleValid(p_stTmpObstacle1))
								{
									if (MEC_fn_bIsEdge(p_stTmpObstacle1) && MEC_fn_bWallObstacle(p_stDynamic,p_stTmpObstacle1))
									{
										MTH3D_M_vSubVector (&stVector,DNM_M_p_stObstacleGetContact(p_stTmpObstacle),DNM_M_p_stObstacleGetContact(p_stTmpObstacle1));
										if (MEC_fn_bIsNullVector(&stVector))
											bRemoveEdge = FALSE;
									}
								}
							}
						}
						if (bRemoveEdge)
							MEC_fn_vInvalidateObstacle (p_stTmpObstacle);
					}
				}
			}
		}
	}
}

#ifndef C_ulDNM_RAYMAN_ZDM_RADIUS
#define C_ulDNM_RAYMAN_ZDM_RADIUS  MTH_M_xConstFloatToReal(0.9f)   /* 0x0049C75C */
#endif

void __cdecl MEC_fn_vDeleteOnGroundBackFace ( long lObstacle, DNM_tdstMecObstacle *p_stObstacle, DNM_tdstDynamics *p_stDynamic, MTH3D_tdstVector *p_stVector, DNM_tdstMecObstacle *p_stGroundObstacle )
{
	MTH3D_tdstVector stVector, stAdjustVector;
	long lCounter;
	MTH_tdxReal xDotProd;
	DNM_tdstMecObstacle *p_stCurrentObstacle;

	for (lCounter=0; lCounter<lObstacle; lCounter++)
	{
		p_stCurrentObstacle = p_stObstacle + lCounter;

		if (MEC_fn_bIsObstacleValid(p_stCurrentObstacle))
		{
			if (MEC_fn_bWallObstacle(p_stDynamic,p_stCurrentObstacle))
			{
				xDotProd = MTH3D_M_xDotProductVector ( DNM_M_p_stObstacleGetNorm(p_stCurrentObstacle),p_stVector );
				if ( MTH_M_bLessEqual(xDotProd,MTH_C_ZERO))
				{
					MTH3D_M_vSubVector (&stVector,DNM_M_p_stObstacleGetContact(p_stCurrentObstacle),POS_fn_p_stGetTranslationVector(DNM_M_p_stDynamicsGetPreviousMatrix(p_stDynamic)));
					MTH3D_M_vMulScalarVector ( &stAdjustVector, DNM_M_xObstacleGetZoneRadius(p_stCurrentObstacle), DNM_M_pDynamicsGetGroundNormal(p_stDynamic) );
					MTH3D_M_vSubVector ( &stVector, &stVector, &stAdjustVector );

					xDotProd = MTH3D_M_xDotProductVector ( DNM_M_p_stObstacleGetNorm(p_stCurrentObstacle),&stVector );

					if ( MTH_M_bGreater(xDotProd,MTH_C_ZERO) )
						MEC_fn_vInvalidateObstacle(p_stCurrentObstacle);
				}
				else MEC_fn_vInvalidateObstacle(p_stCurrentObstacle);
			}
			else if (!MEC_fn_bGroundObstacle(p_stDynamic,p_stCurrentObstacle))
			{
				if (p_stGroundObstacle)
				{
					MTH3D_M_vSubVector (&stVector,DNM_M_p_stObstacleGetContact(p_stCurrentObstacle),DNM_M_p_stObstacleGetContact(p_stGroundObstacle));
					xDotProd = MTH3D_M_xDotProductVector ( DNM_M_p_stObstacleGetNorm(p_stGroundObstacle),&stVector );
					if (MTH_M_bLess(xDotProd,C_ulDNM_RAYMAN_ZDM_RADIUS))
						MEC_fn_vInvalidateObstacle(p_stCurrentObstacle);
				}
			}
		}
	}
}

ACP_tdxBool __cdecl MEC_fn_bMainActor (HIE_tdstSuperObject *hSupObj)
{

	return (ACP_tdxBool)((fn_ulStandardGameGetCustomBitsSO(hSupObj) & C_ulDNM_RAYMAN_CUSTOM_BIT) == C_ulDNM_RAYMAN_CUSTOM_BIT);
}

#ifndef C_xDNM_ON_GROUND_STEP
#define C_xDNM_ON_GROUND_STEP  MTH_M_xConstFloatToReal(0.45f)   /* 0x0049C748 */
#endif

DNM_tdstMecObstacle * __cdecl MEC_fn_pSelectGround (
	DNM_tdstDynamics    *p_stDynamic,
	long                 lObstacle,
	DNM_tdstMecObstacle *p_stObstacle,
	MTH3D_tdstVector    *p_stMove,
	ACP_tdxBool         *bFirstGround )
{
	long lCounter;
	DNM_tdstMecObstacle *p_stTmpObstacle;
	MTH3D_tdstVector stVector;
	DNM_tdstMecObstacle *p_stGroundObstacle = NULL;
	MTH_tdxReal xGroundDeltaRate, xDeltaRate;

	*bFirstGround = TRUE;

	for (lCounter=0; lCounter<lObstacle; lCounter++)
	{
		p_stTmpObstacle = p_stObstacle+lCounter;

		if (MEC_fn_bIsObstacleValid (p_stTmpObstacle))
		{
			if (MEC_fn_bGroundObstacle(p_stDynamic,p_stTmpObstacle))
			{
				if (!p_stGroundObstacle)
				{
					p_stGroundObstacle = p_stTmpObstacle;
					if (DNM_M_bDynamicsIsMechanicChange(p_stDynamic) && !MEC_fn_bIsCollide(p_stGroundObstacle))
						return p_stGroundObstacle;
					xGroundDeltaRate = MTH_M_xAbs ( MTH_M_xSub ( DNM_M_xObstacleGetRate(p_stGroundObstacle), MTH_M_xConstFloatToReal(0.5f) ) );
				}
				else
				{
					MTH3D_M_vSubVector ( &stVector, DNM_M_p_stObstacleGetContact(p_stGroundObstacle), DNM_M_p_stObstacleGetContact(p_stTmpObstacle) );
					if (MTH_M_bGreater(MTH3D_M_xGetZofVector(&stVector),C_xDNM_ON_GROUND_STEP) &&
					    MTH_M_bLess(MTH3D_M_xDotProductVector(DNM_M_p_stObstacleGetNorm(p_stTmpObstacle), p_stMove), MTH_M_xConstFloatToReal(0.001f)))
					{
						MTH_tdxReal xMoveNorm, xHyp, xRatio;

						xMoveNorm = MTH3D_M_xNormVector (p_stMove);
						xHyp = MTH_M_xSqrt (MTH_M_xAbs (MTH3D_M_xGetZofVector(&stVector)) * MTH_M_xAbs (MTH3D_M_xGetZofVector(&stVector)) + xMoveNorm * xMoveNorm);
						xRatio = MTH_C_ONE;
						if (MTH_M_xAbs (xHyp) != MTH_C_ZERO)
							xRatio = xMoveNorm / xHyp;
						if (MTH_M_bLess (xRatio, MTH3D_M_xDotProductVector (DNM_M_p_stObstacleGetNorm (p_stGroundObstacle), DNM_M_p_stObstacleGetNorm (p_stTmpObstacle))))
						{
							xDeltaRate = MTH_M_xAbs ( MTH_M_xSub ( DNM_M_xObstacleGetRate(p_stTmpObstacle), MTH_M_xConstFloatToReal(0.5f) ) );
							if (MTH_M_bLess (xDeltaRate, xGroundDeltaRate))
							{
								*bFirstGround = FALSE;
								xGroundDeltaRate = xDeltaRate;
								p_stGroundObstacle = p_stTmpObstacle;
							}
						}
					}
				}
			}
		}
	}

	return p_stGroundObstacle;
}

ACP_tdxBool __cdecl MEC_fn_bDetectStep ( DNM_tdstDynamics *p_stDynamic, long lObstacle, DNM_tdstMecObstacle *p_stObstacle, DNM_tdstMecObstacle *p_stGroundObstacle, MTH3D_tdstVector *p_stMoveDirection )
{
	long lCounter;
	ACP_tdxBool bStep = FALSE;
	ACP_tdxBool bStepCandidate = FALSE;
	ACP_tdxBool bFindGround = FALSE;
	MTH_tdxReal xDotProd;
	MTH3D_tdstVector stVector;
	DNM_tdstMecObstacle *p_stTmpObstacle;

	if (!MEC_fn_bIsEdge(p_stGroundObstacle))
		return FALSE;

	for (lCounter=0; lCounter<lObstacle; lCounter++)
	{
		p_stTmpObstacle = p_stObstacle+lCounter;

		if (MEC_fn_bIsObstacleValid (p_stTmpObstacle))
		{
			if (p_stTmpObstacle==p_stGroundObstacle)
				bFindGround = TRUE;

			else if (MEC_fn_bWallObstacle(p_stDynamic,p_stTmpObstacle))
			{
				MTH3D_M_vSubVector ( &stVector, DNM_M_p_stObstacleGetContact(p_stGroundObstacle), DNM_M_p_stObstacleGetContact(p_stTmpObstacle) );
				if (MTH3D_M_bIsNullVector(&stVector))
				{
					xDotProd = MTH3D_M_xDotProductVector (DNM_M_p_stObstacleGetNorm(p_stTmpObstacle),p_stMoveDirection);
					if (MTH_M_bLessZero(xDotProd))
						bStepCandidate=TRUE;
				}
			}
			else if (MEC_fn_bGroundObstacle(p_stDynamic,p_stTmpObstacle))
			{
				MTH3D_M_vSubVector ( &stVector, DNM_M_p_stObstacleGetContact(p_stGroundObstacle), DNM_M_p_stObstacleGetContact(p_stTmpObstacle) );
				if (MTH_M_bLess(MTH3D_M_xGetZofVector(&stVector),C_xDNM_ON_GROUND_STEP) && !MTH3D_M_bIsNullVector(&stVector))
				{
					if (bStepCandidate && bFindGround)
					{
						bStep = TRUE;
						bStepCandidate = FALSE;
					}
				}
			}
		}
	}
	return bStep;
}

ACP_tdxBool __cdecl MEC_fn_bIsEqualRotationMatrix (POS_tdxHandleToPosition hMatrix1, POS_tdxHandleToPosition hMatrix2)
{
	MTH3D_tdstVector *p_stI1, *p_stJ1, *p_stK1;
	MTH3D_tdstVector *p_stI2, *p_stJ2, *p_stK2;

	POS_fn_vGetRotationVector ( hMatrix1, &p_stI1, &p_stJ1, &p_stK1 );
	POS_fn_vGetRotationVector ( hMatrix2, &p_stI2, &p_stJ2, &p_stK2 );

	return (ACP_tdxBool)(
		(MTH_M_bEqual((p_stI1)->xX, (p_stI2)->xX)) && (MTH_M_bEqual((p_stI1)->xY, (p_stI2)->xY)) && (MTH_M_bEqual((p_stI1)->xZ, (p_stI2)->xZ)) &&
		(MTH_M_bEqual((p_stJ1)->xX, (p_stJ2)->xX)) && (MTH_M_bEqual((p_stJ1)->xY, (p_stJ2)->xY)) && (MTH_M_bEqual((p_stJ1)->xZ, (p_stJ2)->xZ)) &&
		(MTH_M_bEqual((p_stK1)->xX, (p_stK2)->xX)) && (MTH_M_bEqual((p_stK1)->xY, (p_stK2)->xY)) && (MTH_M_bEqual((p_stK1)->xZ, (p_stK2)->xZ)));
}

/* 0x00440690 */
void *__cdecl DNM_p_stReportDefaultInit(void *_p_stReport)
{
    unsigned long *p_ul = (unsigned long *)_p_stReport;
    int            i;

    for (i = 0x5b; i != 0; i = i - 1)
    {
        *p_ul = 0;
        p_ul = p_ul + 1;
    }
    return _p_stReport;
}

#include "MEC/dnmdynam.h"
#include <string.h>
void __cdecl DNM_fn_vDynamicsInitDefault(DNM_tdstDynamics *_p_stDynamic)
{
    DNM_tdstReport *p_stReport = NULL;
    unsigned long   ulEndFlags = 0;

    memset(&g_stReport, 0, sizeof(DNM_tdstMecReport));

    _p_stDynamic->m_tdstDynamicsBaseBlock.ulFlags = 0;
    _p_stDynamic->m_tdstDynamicsBaseBlock.ulEndFlags &= (DNM_C_CollisionReport|DNM_C_Always|0xF);
    ulEndFlags = _p_stDynamic->m_tdstDynamicsBaseBlock.ulEndFlags;

    if (DNM_M_bDynamicsIsCollisionReport(_p_stDynamic))
    {
        p_stReport = DNM_M_p_stDynamicsGetReport(_p_stDynamic);
        DNM_p_stReportDefaultInit(p_stReport);
    }

    if (DNM_M_bDynamicsIsComplexSize(_p_stDynamic)) memset ( _p_stDynamic, 0, sizeof(DNM_tdstComplexDynamics) );
    else if (DNM_M_bDynamicsIsAdvancedSize(_p_stDynamic)) memset ( _p_stDynamic, 0, sizeof(DNM_tdstAdvancedDynamics) );
    else memset ( _p_stDynamic, 0, sizeof(DNM_tdstBaseDynamics) );

    _p_stDynamic->m_tdstDynamicsBaseBlock.ulEndFlags = ulEndFlags;
    if (p_stReport)
        DNM_M_p_stDynamicsGetReport(_p_stDynamic) = p_stReport;

    DNM_M_vDynamicsSetInit(_p_stDynamic, TRUE);

    DNM_M_vDynamicsSetNbFrame(_p_stDynamic, 1);

    if (DNM_M_bDynamicsIsAdvancedSize(_p_stDynamic))
    {
        DNM_M_vDynamicsSetStreamFactor(_p_stDynamic, MTH_C_ONE);
    }

    if (DNM_M_bDynamicsIsComplexSize(_p_stDynamic))
    {

        DNM_M_vDynamicsSetPlatformSO(_p_stDynamic, (void *)0);
    }

    memset(&g_stReport, 0, sizeof(DNM_tdstMecReport));
}

/* 0x00435B70 */
void __cdecl DNM_fn_vInitBaseMechanic(void *_p_stDynamic)
{
    DNM_fn_vDynamicsInitDefault(_p_stDynamic);
}

typedef struct DNM_stMecBaseIdCard
{
    DNM_tdeMechanicsID m_eIdentity;
    unsigned long  ulFlags;
    MTH_tdxReal    m_xGravity;
    MTH_tdxReal    m_xSlide;
    MTH_tdxReal    m_xRebound;
    MTH_tdxReal    m_xSlopeLimit;
    MTH_tdxReal    m_xInertiaX;
    MTH_tdxReal    m_xInertiaY;
    MTH_tdxReal    m_xInertiaZ;
    MTH_tdxReal    m_xTiltIntensity;
    MTH_tdxReal    m_xTiltInertia;
    MTH_tdxReal    m_xTiltOrigin;
    MTH3D_tdstVector m_stMaxSpeed;
} DNM_tdstMecBaseIdCard;

typedef struct DNM_stMecBaseIdCard *DNM_tdxHandleToMecIdentityCard;

#define DNM_Card_C_Animation        0x00000001UL
#define DNM_Card_C_Collide          0x00000002UL
#define DNM_Card_C_Gravity          0x00000004UL
#define DNM_Card_C_Tilt             0x00000008UL
#define DNM_Card_C_Gi               0x00000010UL
#define DNM_Card_C_OnGround         0x00000020UL
#define DNM_Card_C_Climb            0x00000040UL
#define DNM_Card_C_Spider           0x00000080UL
#define DNM_Card_C_Shoot            0x00000100UL
#define DNM_Card_C_CollisionControl 0x00000200UL
#define DNM_Card_C_KeepWallSpeedZ   0x00000400UL
#define DNM_Card_C_SpeedLimit       0x00000800UL
#define DNM_Card_C_Inertia          0x00001000UL
#define DNM_Card_C_Stream           0x00002000UL
#define DNM_Card_C_StickOnPlatform  0x00004000UL
#define DNM_Card_C_Scale            0x00008000UL
#define DNM_Card_C_Swim             0x00020000UL

#define DNM_M_bBaseIdCardIsAnimation(c)          ((c)->ulFlags & DNM_Card_C_Animation ? TRUE : FALSE)
#define DNM_M_bBaseIdCardIsCollide(c)            ((c)->ulFlags & DNM_Card_C_Collide ? TRUE : FALSE)
#define DNM_M_bBaseIdCardIsGravity(c)            ((c)->ulFlags & DNM_Card_C_Gravity ? TRUE : FALSE)
#define DNM_M_bBaseIdCardIsTilt(c)               ((c)->ulFlags & DNM_Card_C_Tilt ? TRUE : FALSE)
#define DNM_M_bBaseIdCardIsGi(c)                 ((c)->ulFlags & DNM_Card_C_Gi ? TRUE : FALSE)
#define DNM_M_bBaseIdCardIsOnGround(c)           ((c)->ulFlags & DNM_Card_C_OnGround ? TRUE : FALSE)
#define DNM_M_bBaseIdCardIsClimb(c)              ((c)->ulFlags & DNM_Card_C_Climb ? TRUE : FALSE)
#define DNM_M_bBaseIdCardIsSpider(c)             ((c)->ulFlags & DNM_Card_C_Spider ? TRUE : FALSE)
#define DNM_M_bBaseIdCardIsShoot(c)              ((c)->ulFlags & DNM_Card_C_Shoot ? TRUE : FALSE)
#define DNM_M_bBaseIdCardIsCollisionControl(c)   ((c)->ulFlags & DNM_Card_C_CollisionControl ? TRUE : FALSE)
#define DNM_M_bBaseIdCardIsKeepWallSpeedZ(c)     ((c)->ulFlags & DNM_Card_C_KeepWallSpeedZ ? TRUE : FALSE)
#define DNM_M_bBaseIdCardIsSpeedLimit(c)         ((c)->ulFlags & DNM_Card_C_SpeedLimit ? TRUE : FALSE)
#define DNM_M_bBaseIdCardIsInertia(c)            ((c)->ulFlags & DNM_Card_C_Inertia ? TRUE : FALSE)
#define DNM_M_bBaseIdCardIsStream(c)             ((c)->ulFlags & DNM_Card_C_Stream ? TRUE : FALSE)
#define DNM_M_bBaseIdCardIsStickOnPlatform(c)    ((c)->ulFlags & DNM_Card_C_StickOnPlatform ? TRUE : FALSE)
#define DNM_M_bBaseIdCardIsScale(c)              ((c)->ulFlags & DNM_Card_C_Scale ? TRUE : FALSE)
#define DNM_M_bBaseIdCardIsSwim(c)               ((c)->ulFlags & DNM_Card_C_Swim ? TRUE : FALSE)

#define DNM_M_xBaseIdCardGetGravityFactor(c)     ((c)->m_xGravity)
#define DNM_M_xBaseIdCardGetSlide(c)             ((c)->m_xSlide)
#define DNM_M_xBaseIdCardGetRebound(c)           ((c)->m_xRebound)
#define DNM_M_xBaseIdCardGetSlopeLimit(c)        ((c)->m_xSlopeLimit)
#define DNM_M_xBaseIdCardGetInertiaFactorX(c)    ((c)->m_xInertiaX)
#define DNM_M_xBaseIdCardGetInertiaFactorY(c)    ((c)->m_xInertiaY)
#define DNM_M_xBaseIdCardGetInertiaFactorZ(c)    ((c)->m_xInertiaZ)
#define DNM_M_xBaseIdCardGetTiltIntensity(c)     ((c)->m_xTiltIntensity)
#define DNM_M_xBaseIdCardGetTiltInertia(c)       ((c)->m_xTiltInertia)
#define DNM_M_xBaseIdCardGetTiltOrigin(c)        ((c)->m_xTiltOrigin)
#define DNM_M_pBaseIdCardGetMaxSpeed(c)          (&(c)->m_stMaxSpeed)

/* 0x00435B80 */
void __cdecl DNM_fn_vInitDynamic(DNM_tdstDynamics *p_stDynamic, DNM_tdxHandleToMecIdentityCard hIdCard)
{
    DNM_tdstMecBaseIdCard* p_IdCard = (DNM_tdstMecBaseIdCard*)hIdCard;

    DNM_M_vDynamicsSetAnimation(p_stDynamic,DNM_M_bBaseIdCardIsAnimation(p_IdCard));
    DNM_M_vDynamicsSetCollide(p_stDynamic,DNM_M_bBaseIdCardIsCollide(p_IdCard));
    DNM_M_vDynamicsSetGravity(p_stDynamic,DNM_M_bBaseIdCardIsGravity(p_IdCard));
    DNM_M_vDynamicsSetTilt(p_stDynamic,DNM_M_bBaseIdCardIsTilt(p_IdCard));
    DNM_M_vDynamicsSetGi(p_stDynamic,DNM_M_bBaseIdCardIsGi(p_IdCard));
    DNM_M_vDynamicsSetOnGround(p_stDynamic,DNM_M_bBaseIdCardIsOnGround(p_IdCard));
    DNM_M_vDynamicsSetClimb(p_stDynamic,DNM_M_bBaseIdCardIsClimb(p_IdCard));
    DNM_M_vDynamicsSetSpider(p_stDynamic,DNM_M_bBaseIdCardIsSpider(p_IdCard));
    DNM_M_vDynamicsSetShoot(p_stDynamic,DNM_M_bBaseIdCardIsShoot(p_IdCard));
    DNM_M_vDynamicsSetSwim(p_stDynamic,DNM_M_bBaseIdCardIsSwim(p_IdCard));
    DNM_M_vDynamicsSetCollisionControl(p_stDynamic,DNM_M_bBaseIdCardIsCollisionControl(p_IdCard));
    DNM_M_vDynamicsSetKeepWallSpeedZ(p_stDynamic,DNM_M_bBaseIdCardIsKeepWallSpeedZ(p_IdCard));
    DNM_M_vDynamicsSetSpeedLimit(p_stDynamic,DNM_M_bBaseIdCardIsSpeedLimit(p_IdCard));
    DNM_M_vDynamicsSetInertia(p_stDynamic,DNM_M_bBaseIdCardIsInertia(p_IdCard));
    DNM_M_vDynamicsSetStream(p_stDynamic,DNM_M_bBaseIdCardIsStream(p_IdCard));
    DNM_M_vDynamicsSetStickOnPlatform(p_stDynamic,DNM_M_bBaseIdCardIsStickOnPlatform(p_IdCard));
    DNM_M_vDynamicsSetScaleOption(p_stDynamic,DNM_M_bBaseIdCardIsScale(p_IdCard));
    DNM_M_vDynamicsSetGravityFactor(p_stDynamic,DNM_M_xBaseIdCardGetGravityFactor(p_IdCard));
    DNM_M_vDynamicsSetSlopeLimit(p_stDynamic,DNM_M_xBaseIdCardGetSlopeLimit(p_IdCard));
    DNM_M_bDynamicsSetMechanicChange(p_stDynamic,TRUE);
    DNM_M_vDynamicsSetNeverFall(p_stDynamic,FALSE);
    DNM_M_vDynamicsSetForceSafeWalk ( p_stDynamic, FALSE );

    {
        register MTH_tdxReal xSlopeLimit = MTH_M_xSqrt (MTH_M_xDiv (MTH_C_ONE,MTH_M_xAdd(MTH_C_ONE,MTH_M_xSqr(DNM_M_xDynamicsGetSlopeLimit(p_stDynamic)))));
        DNM_M_vDynamicsSetCosSlope ( p_stDynamic, xSlopeLimit );
    }

    if (DNM_M_bDynamicsIsAdvancedSize(p_stDynamic))
    {
        DNM_M_vDynamicsSetSlide(p_stDynamic,DNM_M_xBaseIdCardGetSlide(p_IdCard));
        DNM_M_vDynamicsSetRebound(p_stDynamic,DNM_M_xBaseIdCardGetRebound(p_IdCard));
        DNM_M_vDynamicsSetInertiaFactorX(p_stDynamic,DNM_M_xBaseIdCardGetInertiaFactorX(p_IdCard));
        DNM_M_vDynamicsSetInertiaFactorY(p_stDynamic,DNM_M_xBaseIdCardGetInertiaFactorY(p_IdCard));
        DNM_M_vDynamicsSetInertiaFactorZ(p_stDynamic,DNM_M_xBaseIdCardGetInertiaFactorZ(p_IdCard));
        DNM_M_vDynamicsSetMaxSpeed(p_stDynamic,DNM_M_pBaseIdCardGetMaxSpeed(p_IdCard));

        DNM_M_vDynamicsSetCollisionTranslation(p_stDynamic, POS_fn_p_stGetTranslationVector(DNM_M_p_stDynamicsGetCurrentMatrix(p_stDynamic)));
    }

    else if (DNM_M_bDynamicsIsComplexSize(p_stDynamic))
    {
        DNM_M_vDynamicsSetTiltIntensity(p_stDynamic,DNM_M_xBaseIdCardGetTiltIntensity(p_IdCard));
        DNM_M_vDynamicsSetTiltInertia(p_stDynamic,DNM_M_xBaseIdCardGetTiltInertia(p_IdCard));
        DNM_M_vDynamicsSetTiltOrigin(p_stDynamic,DNM_M_xBaseIdCardGetTiltOrigin(p_IdCard));
    }
}

#define DNM_M_eBaseIdCardGetIdentity(c)          ((c)->m_eIdentity)
#define DNM_M_vDynamicsSetCurrentIdCard(p, h)    (DNM_M_pDynamicsGetCurrentIdCard(p) = (h))

void DNM_fn_vChangeIdCard (DNM_tdstDynamics *_p_stDynamics,DNM_tdxHandleToMecIdentityCard _hNewIdCard)
{
    if (DNM_M_eBaseIdCardGetIdentity((DNM_tdstMecBaseIdCard*)(_hNewIdCard)) == DNM_eBase)
        DNM_fn_vInitDynamic (_p_stDynamics, _hNewIdCard);
    DNM_M_vDynamicsSetCurrentIdCard(_p_stDynamics,_hNewIdCard);
}
