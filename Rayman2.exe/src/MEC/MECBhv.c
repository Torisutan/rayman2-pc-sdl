#include <string.h>
#include "ACP/ACP_types.h"
#include "DNM/DNM_types.h"
#include "POS/POS_types.h"
#include "MTH/MTH_flt.h"
#include "MTH/MTH_xNeg.h"
#include "MTH/MTH3D.h"
#include "MEC/MECTools.h"
#include "MEC/MECCol.h"
#include "MEC/MEC.h"
#include "MEC/MECBhv.h"
#include "DNM/DNM.h"

extern unsigned char __cdecl fn_ucCollSetGetCollComputeFrequencySO (void *hSuperObject);

extern DNM_tdstMecObstacle COL_g_stCollisionCase_storage[];
DNM_tdstMecObstacle *g_a_stObstacle = COL_g_stCollisionCase_storage;

extern MTH_tdxReal g_xZWater;

#ifndef MTH_M_xDoubleToReal
#define MTH_M_xDoubleToReal(_x)     ((MTH_tdxReal)(_x))
#endif
#ifndef C_xDNM_MTH_EPSILON
#define C_xDNM_MTH_EPSILON MTH_M_xDoubleToReal(1e-3)
#endif
#ifndef C_xDNM_VERTICAL_FALL_LIMIT
#define C_xDNM_VERTICAL_FALL_LIMIT MTH_M_xDoubleToReal(0.07)
#endif

#ifndef MTH_M_xConstFloatToReal
#define MTH_M_xConstFloatToReal(_x)   ((MTH_tdxReal)(_x))
#endif
#ifndef C_xDNM_CLIMB_COS_ANGLE_LIMIT
#define C_xDNM_CLIMB_COS_ANGLE_LIMIT  (MTH_M_xConstFloatToReal(0.92718f))
#endif

#define C_WOT_ulGround    0x00000001UL
#define C_WOT_ulWall      0x00000004UL
#define C_WOT_ulCeiling   0x00000010UL
#define C_WOT_ulWater     0x00000040UL
#define C_WOT_ulMobile    0x00010000UL

#define C_ulDNM_CAN_PUSH_RAYMAN_CUSTOM_BIT   0x04000000UL
/* 0x00413700 */
extern unsigned long __cdecl fn_ulStandardGameGetCustomBitsSO (void *hSupObj);

void __cdecl MEC_fn_vCollideBaseMechanic ( DNM_tdstDynamics *p_stDynamic, HIE_tdstSuperObject *hSupObj, long lObstacle, DNM_tdstMecObstacle *a_stObstacle, MTH_tdxReal xDT )
{
	POS_tdxHandleToPosition hCurrentMatrix, hPreviousMatrix, hAbsolutePreviousMatrix;
	MTH3D_tdstVector stIncidentVector, stResultantVector;
	MTH3D_tdstVector stNormalizeIncidentVector;
	MTH3D_tdstVector stPosition;
	MTH3D_tdstVector *p_stPreviousPosition, *p_stCurrentPosition;

	MTH3D_tdstVector stBaseRebound, stBaseSlide;
	MTH3D_tdstVector stVector;
	MTH3D_tdstVector stExtremPosition;
	MTH3D_tdstVector stMoveVector;
	MTH3D_tdstVector stAdjustVector;
	MTH3D_tdstVector stNormalVector;
	MTH3D_tdstVector* p_stNormal;
	MTH_tdxReal xDotProd;
	MTH_tdxReal xNorm;
	MTH_tdxReal xRate;
	MTH_tdxReal xZWater;
	MTH_tdxReal xOneMinusRate;
	MTH_tdxReal xRealTmp;
	ACP_tdxBool bWallObstacle, bGroundObstacle;
	ACP_tdxBool bOnlyEdge = TRUE;
	ACP_tdxBool bWater = FALSE;
	ACP_tdxBool bCollisionOk = TRUE;
	MTH_tdxReal xSlide, xRebound;
	MTH_tdxReal xDotProdMax = MTH_C_MinusONE;
	ACP_tdxBool	bCantKeepWallSpeedZ = FALSE;
	ACP_tdxBool bWaterBlock = FALSE;

	MTH3D_tdstVector stVerticalVector;
	DNM_tdstMecObstacle *p_stObstacle, *p_stTmpObstacle, *p_stGroundObstacle, *p_stMobileObstacle;
	HIE_tdstSuperObject *hSupObjCol;
	long lCounter;

	long lObstacleCounter;
	long lGroundObstacle;
	long lWaterObstacle;

	hPreviousMatrix = DNM_M_p_stDynamicsGetPreviousMatrix (p_stDynamic);
	hCurrentMatrix = DNM_M_p_stDynamicsGetCurrentMatrix (p_stDynamic);
	if (DNM_M_bDynamicsIsComplexSize(p_stDynamic)) hAbsolutePreviousMatrix = DNM_M_p_stDynamicsGetAbsolutePreviousMatrix (p_stDynamic);
	p_stCurrentPosition = POS_fn_p_stGetTranslationVector ( hCurrentMatrix );
	p_stPreviousPosition = POS_fn_p_stGetTranslationVector ( hPreviousMatrix );

	p_stObstacle = NULL;
	p_stGroundObstacle = NULL;
	MTH3D_M_vNullVector (&stNormalVector);
	lWaterObstacle = 0;

	/* 0x00437E60 */
	g_lCallCounter++;
	if (g_lCallCounter==1)
	{
		p_stMobileObstacle = NULL;
		for (lCounter=0; lCounter<lObstacle && !p_stMobileObstacle; lCounter++)
		{
			p_stTmpObstacle = a_stObstacle+lCounter;
			if ( MEC_fn_bIsObstacleValid(p_stTmpObstacle) && ((DNM_M_eObstacleGetType(p_stTmpObstacle)&DNM_ObsType_Mobile)==DNM_ObsType_Mobile) && DNM_M_p_stObstacleGetObject(p_stTmpObstacle) )
			{
				hSupObjCol = MEC_fn_hGetFatherActor ((HIE_tdstSuperObject *)DNM_M_p_stObstacleGetObject (p_stTmpObstacle));
				if ((fn_ulStandardGameGetCustomBitsSO(hSupObjCol) & C_ulDNM_CAN_PUSH_RAYMAN_CUSTOM_BIT) == C_ulDNM_CAN_PUSH_RAYMAN_CUSTOM_BIT)
					p_stMobileObstacle = p_stTmpObstacle;
			}
		}

		if (p_stMobileObstacle && MEC_fn_bCollision(&lObstacle,g_a_stObstacle,p_stDynamic,hSupObj,hPreviousMatrix,hPreviousMatrix,TRUE))
		{
			MEC_fn_vDeleteBaseBackFace ( lObstacle,g_a_stObstacle, p_stDynamic );
			MEC_fn_vDeleteNullTranslationObstacle ( lObstacle, g_a_stObstacle );

			p_stTmpObstacle = NULL;
			for (lCounter=0; lCounter<lObstacle && !p_stTmpObstacle; lCounter++)
			{
				if ( MEC_fn_bIsObstacleValid(g_a_stObstacle+lCounter) && ((DNM_M_eObstacleGetType(g_a_stObstacle+lCounter)&DNM_ObsType_Mobile)==DNM_ObsType_Mobile) )
					p_stTmpObstacle = g_a_stObstacle+lCounter;
			}

			if (p_stTmpObstacle)
			{
				MTH3D_M_vSubVector ( &stIncidentVector, p_stCurrentPosition, p_stPreviousPosition );
				MTH3D_M_vCopyVector ( &stVector, DNM_M_xObstacleGetTranslation(p_stTmpObstacle) );
				MTH3D_M_vAddVector ( p_stCurrentPosition, p_stPreviousPosition, &stVector );

				if (MEC_fn_bCollision(&lObstacle,g_a_stObstacle,p_stDynamic,hSupObj,hPreviousMatrix,hCurrentMatrix,TRUE))
				{
					MEC_fn_vDeleteBaseBackFace ( lObstacle,g_a_stObstacle, p_stDynamic );
					MEC_fn_vDeleteNullTranslationObstacle ( lObstacle, g_a_stObstacle );
					MEC_fn_vComputeAdjustVector (&stAdjustVector, p_stDynamic, lObstacle, g_a_stObstacle, lObstacle);
					MTH3D_M_vAddVector ( p_stPreviousPosition, p_stCurrentPosition, &stAdjustVector );
					MTH3D_M_vAddVector ( p_stCurrentPosition, p_stPreviousPosition, &stIncidentVector );
				}
				else
				{
					MTH3D_M_vSubVector ( p_stPreviousPosition, p_stCurrentPosition, &stIncidentVector );
				}
			}

			MEC_fn_vBaseObstacleManagement ( p_stDynamic, hSupObj, xDT );
			return;
		}
	}

	if (DNM_M_bDynamicsIsCanFall(p_stDynamic))
	{
		for (lCounter=0; lCounter<lObstacle; lCounter++)
		{
			p_stTmpObstacle = a_stObstacle+lCounter;
			if (MEC_fn_bIsObstacleValid(p_stTmpObstacle))
			{
				if (MEC_fn_bGroundObstacle(p_stDynamic,p_stTmpObstacle) )
					MEC_fn_vInvalidateObstacle (p_stTmpObstacle);
			}
		}
	}
	else
	{

		for (lCounter=0; lCounter<lObstacle && !p_stGroundObstacle; lCounter++)
		{
			p_stTmpObstacle = a_stObstacle+lCounter;
			if (MEC_fn_bIsObstacleValid(p_stTmpObstacle))
			{
				if (!MEC_fn_bIsWater(p_stTmpObstacle) && MEC_fn_bGroundObstacle(p_stDynamic,p_stTmpObstacle) )
				{
					p_stGroundObstacle = p_stTmpObstacle;
					lGroundObstacle = lCounter;
				}
			}
		}
	}

	if (p_stGroundObstacle && DNM_M_bDynamicsIsSwim(p_stDynamic))
	{
		bWater = ( ( DNM_M_ulReportGetSurfaceState(&g_stReport) & C_WOT_ulWater ) == C_WOT_ulWater );

		if (!bWater)
		{
			MEC_fn_vGroundObstacleManagement ( p_stDynamic, hSupObj, xDT );
			DNM_M_ulReportSetSurfaceState (&g_stReport,DNM_M_ulReportGetSurfaceState(&g_stReport)&(~C_WOT_ulWater));
			return;
		}
	}

	/* 0x004382A5 */
	bOnlyEdge = TRUE;
	for (lCounter=0; lCounter<lObstacle && !p_stObstacle; lCounter++)
	{
		p_stTmpObstacle = a_stObstacle+lCounter;
		if (MEC_fn_bIsObstacleValid(p_stTmpObstacle))
		{
			p_stObstacle = p_stTmpObstacle;
			lObstacleCounter = lCounter;
		}
	}

	for (lCounter=0; lCounter<lObstacle; lCounter++)
	{
		p_stTmpObstacle = a_stObstacle+lCounter;

		if (MEC_fn_bIsObstacleValid(p_stTmpObstacle))
		{
			if ( MEC_fn_bIsWater(p_stTmpObstacle) && !lWaterObstacle )
				lWaterObstacle = lCounter;

			if (!MEC_fn_bIsEdge(p_stTmpObstacle))
				bOnlyEdge = FALSE;
		}
	}

	if (!p_stObstacle)
		return;

	if (p_stGroundObstacle && lGroundObstacle>lObstacleCounter)
		lObstacleCounter=lGroundObstacle;

	if (lWaterObstacle>lObstacleCounter)
		lObstacleCounter = lWaterObstacle;

	MEC_fn_vReportCollision (p_stDynamic,hSupObj,lObstacle,a_stObstacle,lObstacleCounter);

	if (!bOnlyEdge||DNM_M_bDynamicsIsCanFall(p_stDynamic))
		p_stNormal = DNM_M_p_stObstacleGetNorm (p_stObstacle);
	else
	{
		p_stNormal = &stNormalVector;
		for (lCounter=0; lCounter<lObstacle; lCounter++)
		{
			p_stTmpObstacle = a_stObstacle+lCounter;
			if (MEC_fn_bIsObstacleValid(p_stTmpObstacle))
				MTH3D_M_vAddVector ( p_stNormal, p_stNormal, DNM_M_p_stObstacleGetNorm(p_stTmpObstacle) );
		}

		if (!MTH3D_M_bIsNullVector(p_stNormal))
		{	MTH3D_M_vNormalizeVector ( p_stNormal, p_stNormal );	}
		else
			p_stNormal = DNM_M_p_stObstacleGetNorm(p_stObstacle);

		if (MEC_fn_bGroundNormal(p_stDynamic,p_stNormal) && (!p_stGroundObstacle ||  (p_stGroundObstacle&&MEC_fn_bIsEdge(p_stGroundObstacle)) ) )
		{

			for (lCounter=0; lCounter<lObstacle; lCounter++)
			{
				p_stTmpObstacle = a_stObstacle+lCounter;
				if (MEC_fn_bIsObstacleValid(p_stTmpObstacle))
				{
					MTH3D_M_vSubVector (&stVector,DNM_M_p_stObstacleGetContact(p_stTmpObstacle),DNM_M_pObstacleGetZonePosition(p_stTmpObstacle));
					MTH3D_M_vAddVector (&stVector, &stVector, DNM_M_pObstacleGetZoneMove(p_stTmpObstacle) );
					if (!MEC_fn_bIsNullVector(&stVector)) MTH3D_M_vNormalizeVector (&stVector,&stVector);
					xDotProd = MTH3D_M_xDotProductVector (&stVector,DNM_M_p_stObstacleGetNorm(p_stTmpObstacle));
					if (MTH_M_bGreater(xDotProd,xDotProdMax))
					{
						xDotProdMax = xDotProd;
						p_stNormal = DNM_M_p_stObstacleGetNorm(p_stTmpObstacle);
					}
				}
			}
		}
	}

	xSlide = MEC_fn_xGetSlide (p_stDynamic,p_stObstacle);
	xRebound = MEC_fn_xGetRebound (p_stDynamic,p_stObstacle);

	MTH3D_M_vNullVector (&stNormalizeIncidentVector);
	MTH3D_M_vNullVector (&stAdjustVector);

	MTH3D_M_vSetVectorElements (&stVerticalVector, MTH_C_ZERO, MTH_C_ZERO, MTH_C_ONE);
	xDotProd = MTH3D_M_xDotProductVector ( p_stNormal, &stVerticalVector );
	bWallObstacle = MTH_M_bGreaterEqual (xDotProd,MEC_fn_xWallLimit(p_stDynamic)) && MTH_M_bLessEqual (xDotProd,MEC_fn_xGroundLimit(p_stDynamic));
	bGroundObstacle = MTH_M_bGreaterEqual (xDotProd,MEC_fn_xGroundLimit(p_stDynamic));

	MTH3D_M_vSubVector ( &stIncidentVector, p_stCurrentPosition, p_stPreviousPosition );
	xNorm = MTH3D_M_xNormVector ( &stIncidentVector );

	xDotProd = MTH3D_M_xDotProductVector ( &stIncidentVector, p_stNormal );
	MTH3D_M_vMulScalarVector ( &stVector, MTH_M_xNeg(xDotProd), p_stNormal );
	MTH3D_M_vAddVector ( &stBaseSlide, &stIncidentVector, &stVector);
	MTH3D_M_vAddVector ( &stBaseRebound, &stBaseSlide, &stVector);

	MTH3D_M_vMulScalarVector ( &stResultantVector, xSlide, &stBaseSlide );
	MTH3D_M_vMulScalarVector ( &stVector, xRebound, &stBaseRebound );
	MTH3D_M_vAddVector ( &stResultantVector, &stResultantVector, &stVector );
	/* 0x004389DB */
	if (!MEC_fn_bIsNullVector(&stIncidentVector))
		MTH3D_M_vMulScalarVector(&stNormalizeIncidentVector,MTH_M_xInv(xNorm),&stIncidentVector);

	if (DNM_M_bDynamicsIsKeepWallSpeedZ(p_stDynamic) && bWallObstacle && !MTH_M_bEqual(MTH3D_M_xGetZofVector (&stResultantVector),MTH_C_ZERO))
	{
		MTH_tdxReal xZRate, xZResultantVector;

		xZRate = MTH3D_M_xGetZofVector (&stIncidentVector);
		xZResultantVector = MTH3D_M_xGetZofVector (&stResultantVector);
		if (!MTH_M_bEqual(xZResultantVector,MTH_C_ZERO))
		{
			xZRate = MTH_M_xDiv ( xZRate, xZResultantVector );
			if (MTH_M_bGreater(xZRate,MTH_C_MinusONE))
			{
				if ( MTH_M_bLessEqual(xZRate,MTH_C_ONE) )
				{

					MTH3D_M_vMulScalarVector ( &stResultantVector, xZRate, &stResultantVector );
				}
				else
				{

					MTH3D_tdstVector stU, stV, stN, stVector;
					MTH_tdxReal xU, xV, xN, xNorm2, xVExtrem, xVExtrem2;

					MTH3D_M_vCopyVector ( &stN, p_stNormal );
					MTH3D_M_vCrossProductVector ( &stU, &stVerticalVector, &stN );
					MTH3D_M_vCrossProductVector ( &stV, &stN, &stU );

					xU = MTH3D_M_xDotProductVector ( &stResultantVector, &stU );
					xV = MTH3D_M_xDotProductVector ( &stResultantVector, &stV );
					xN = MTH3D_M_xDotProductVector ( &stResultantVector, &stN );

					xVExtrem = MTH_M_xDiv ( MTH3D_M_xGetZofVector (&stIncidentVector), MTH3D_M_xGetZofVector (&stV) );

					xNorm2 = MTH_M_xAdd ( MTH_M_xMul(xU,xU), MTH_M_xMul(xV,xV) );
					xVExtrem2 = MTH_M_xMul( xVExtrem, xVExtrem );

					if ( MTH_M_bGreaterEqual (xNorm2, xVExtrem2) && MTH_M_bGreater ( MTH_M_xAbs(xU),C_xDNM_VERTICAL_FALL_LIMIT) )
					{

						xV = xVExtrem;
					}
					else
					{

						xV = xVExtrem;
					}

					MTH3D_M_vMulScalarVector ( &stResultantVector, xU, &stU );
					MTH3D_M_vMulScalarVector ( &stVector, xV, &stV );
					MTH3D_M_vAddVector ( &stResultantVector, &stResultantVector, &stVector );
					MTH3D_M_vMulScalarVector ( &stVector, xN, &stN );
					MTH3D_M_vAddVector ( &stResultantVector, &stResultantVector, &stVector );
				}
			}
			else
				bCantKeepWallSpeedZ = TRUE;
		}
	}

	xRate = DNM_M_xObstacleGetRate (p_stObstacle);

	bCollisionOk = MEC_fn_vComputeAdjustVector (&stAdjustVector, p_stDynamic, lObstacle, a_stObstacle, lObstacleCounter);

	if (DNM_M_bDynamicsIsHanging(p_stDynamic))
		MTH3D_M_vSetZofVector (&stAdjustVector,MTH_C_ZERO);

	/* 0x00438D2F */
	if ( DNM_M_bDynamicsIsSwim(p_stDynamic) && MEC_fn_bIsEdge(p_stObstacle) && MEC_fn_bIsCollide(p_stObstacle)
	  && MTH_M_bLess(MTH3D_M_xDotProductVector(p_stNormal,&stNormalizeIncidentVector),MTH_M_xConstFloatToReal(-0.42f)) )
	{
		MTH3D_M_vCopyVector (&stPosition, p_stPreviousPosition);
	}
	else
	{
		MTH3D_M_vAddVector (&stPosition, p_stCurrentPosition, &stAdjustVector);
	}

	if (DNM_M_bDynamicsIsSwim(p_stDynamic))
	{
		xZWater = MTH_M_xSub ( g_xZWater, MTH_M_xConstFloatToReal(1.2f) );
		if (MTH_M_bGreater(MTH3D_M_xGetZofVector(&stPosition),xZWater))
		{
			MTH3D_M_vSetZofVector ( &stPosition, MTH3D_M_xGetZofVector(p_stPreviousPosition) );
			bWaterBlock = TRUE;
		}
	}

	if (MTH_M_bGreaterEqual (xRate, MTH_C_ZERO))
		xOneMinusRate = MTH_M_xSub (MTH_C_ONE,xRate);
	else

			xOneMinusRate = MTH_C_ZERO;

	MTH3D_M_vMulScalarVector (&stVector, xOneMinusRate,&stResultantVector);

	if ( MTH_M_bGreater ( MTH3D_M_xNormVector(&stVector), DNM_M_xObstacleGetZoneRadius(p_stObstacle) ) )
	{
		MTH3D_M_vNormalizeVector ( &stVector, &stVector );
		MTH3D_M_vMulScalarVector ( &stVector, DNM_M_xObstacleGetZoneRadius(p_stObstacle), &stVector );
	}

	if (bWaterBlock)
	{
		xRealTmp = MTH3D_M_xNormVector (&stVector);
		MTH3D_M_vSetZofVector ( &stVector, MTH_C_ZERO );
		if (!MEC_fn_bIsNullVector(&stVector))
		{
			MTH3D_M_vNormalizeVector (&stVector,&stVector);
			MTH3D_M_vMulScalarVector (&stVector,xRealTmp,&stVector);
		}
	}

	MTH3D_M_vAddVector (&stExtremPosition, &stPosition, &stVector);

	MTH3D_M_vSubVector (&stMoveVector, &stExtremPosition, p_stPreviousPosition);
	xDotProd = MTH3D_M_xDotProductVector (p_stNormal,&stMoveVector);

	/* 0x00438E6D */
	if ( ( ( ((MTH_M_bGreater(xDotProd,C_xDNM_MTH_EPSILON) && bGroundObstacle && MTH_M_bGreaterZero(xRebound)) || ( p_stGroundObstacle && !MEC_fn_bGroundObstacle(p_stDynamic,p_stObstacle) ) ) && !DNM_M_bDynamicsIsCanFall(p_stDynamic) ) ) )
	{
		MTH3D_M_vCopyVector (p_stCurrentPosition,&stPosition);
		MTH3D_M_vSubVector (p_stPreviousPosition, p_stCurrentPosition, &stResultantVector);
	}
	else
	{
		MTH3D_M_vCopyVector (p_stCurrentPosition,&stExtremPosition);
		MTH3D_M_vSubVector (p_stPreviousPosition, p_stCurrentPosition, &stResultantVector);
	}

	if (DNM_M_bDynamicsIsComplexSize(p_stDynamic)) POS_fn_vSetTranslationVector ( hAbsolutePreviousMatrix, p_stPreviousPosition );

	MEC_fn_vComputeSpeed (p_stDynamic,xDT);
}

void __cdecl MEC_fn_vBaseObstacleManagement ( DNM_tdstDynamics *p_stDynamic, HIE_tdstSuperObject *hSupObj, MTH_tdxReal xDT )
{
	long lObstacle;

	POS_tdxHandleToPosition hCurrentMatrix;
	POS_tdxHandleToPosition hInitialMatrix;
	POS_tdstCompletePosition stInitialMatrix;

	hCurrentMatrix = DNM_M_p_stDynamicsGetCurrentMatrix (p_stDynamic);

	if (fn_ucCollSetGetCollComputeFrequencySO(hSupObj)>1)
	{
		hInitialMatrix = &stInitialMatrix;
		POS_fn_vCopyMatrix (hInitialMatrix,DNM_M_p_stDynamicsGetPreviousMatrix (p_stDynamic));
		MEC_fn_vGetCollisionTranslation (p_stDynamic,hInitialMatrix);
	}
	else hInitialMatrix = DNM_M_p_stDynamicsGetPreviousMatrix (p_stDynamic);

	if(MEC_fn_bCollision(&lObstacle,g_a_stObstacle,p_stDynamic,hSupObj,hInitialMatrix,hCurrentMatrix,(ACP_tdxBool)DNM_M_bDynamicsIsHanging(p_stDynamic)))
	{
		MEC_fn_vEdgeAnalysis (p_stDynamic, lObstacle, g_a_stObstacle, NULL );
		MEC_fn_vCollideBaseMechanic ( p_stDynamic, hSupObj, lObstacle, g_a_stObstacle, xDT);
	}
	else
		DNM_M_vDynamicsSetStop (p_stDynamic,FALSE);
}

#ifndef C_xDNM_GI_TYPE_GAME_MATERIAL
#define C_xDNM_GI_TYPE_GAME_MATERIAL     (1 << 5)
#endif
#ifndef C_xDNM_CLIMB_TYPE_GAME_MATERIAL
#define C_xDNM_CLIMB_TYPE_GAME_MATERIAL  (1 << 6)
#endif
#define C_xDNM_CLIMB_DEFAULT_NORMAL_MOVE MTH_M_xConstFloatToReal(0.45f)
#define C_xDNM_GI_DEFAULT_NORMAL_MOVE    MTH_M_xConstFloatToReal(0.45f)

#define C_xDNM_SPIDER_SLOPE_LIMIT        MTH_M_xTan(1.0471975511965974)
#define C_xDNM_SPIDER_DEFAULT_NORMAL_MOVE MTH_M_xConstFloatToReal(0.5f)
#ifndef C_ulDNM_RAYMAN_ZDM_RADIUS
#define C_ulDNM_RAYMAN_ZDM_RADIUS        MTH_M_xConstFloatToReal(0.9f)   /* 0x0049C79C */
#endif
#define C_xDNM_CLIMB_SLOPE_LIMIT         MTH_M_xTan(1.0471975511965974)
#define C_xDNM_CLIMB_FIRST_NORMAL_MOVE   MTH_M_xConstFloatToReal(0.75f)  /* 0x0049C7B8 */

#define C_xDNM_NUL_MOVE                  MTH_M_xConstFloatToReal(1.0e-10f) /* 0x0049C7C4 */
#define C_xDNM_ON_GROUND_DEFAULT_NORMAL_MOVE MTH_M_xConstFloatToReal(0.45f) /* 0x0049C7A0 */

#ifndef HIE_fn_bIsSuperObjectValid
#define HIE_fn_bIsSuperObjectValid(_hSO)  ((_hSO) != (HIE_tdstSuperObject *)0)
#endif

#ifndef COL_C_xMaxNumberOfCollisions
#define COL_C_xMaxNumberOfCollisions 75
#endif
extern long COL_g_lMaxNumberOfCollisions_storage;
#define COL_g_lMaxNumberOfCollisions (COL_g_lMaxNumberOfCollisions_storage)

MTH3D_tdstVector g_stFirstMove;   /* 0x0050AC20 */

void __cdecl MEC_fn_vGroundObstacleManagement ( DNM_tdstDynamics *p_stDynamic, HIE_tdstSuperObject *hSupObj, MTH_tdxReal xDT )
{
	long lObstacle, lObstacle2;
	long lCounter, lObstacleCounter;

	MTH_tdxReal xSlide;
	MTH_tdxReal xFactor;
	MTH_tdxReal xSpeedZ;

	POS_tdstCompletePosition a2_stPosition[2];
	POS_tdstCompletePosition *p_stPOS_DownPosition = a2_stPosition;
	POS_tdstCompletePosition *p_stPOS_UpPosition = a2_stPosition+1;
	POS_tdstCompletePosition stPreviousMatrix;
	POS_tdxHandleToPosition hCurrentMatrix, hPreviousMatrix;

	MTH3D_tdstVector stVector;
	MTH3D_tdstVector stUpVector, stDownVector;
	MTH3D_tdstVector stMove;
	MTH3D_tdstVector stVertical;
	MTH3D_tdstVector stNormalMove;
	MTH3D_tdstVector stTangentMove;
	MTH3D_tdstVector stAbsoluteTangentMove;
	MTH3D_tdstVector stAdjustVector, stAdjustVector1;
	MTH3D_tdstVector stWallMove;
	MTH3D_tdstVector stSafeTranslation;
	MTH3D_tdstVector stGroundNormal;
	MTH3D_tdstVector stContactVector;
	MTH_tdxReal xCosAngle;

	MTH3D_tdstVector *p_stCurrentTranslation;
	MTH3D_tdstVector *p_stPreviousTranslation;
	MTH3D_tdstVector *p_stUpTranslation;
	MTH3D_tdstVector *p_stDownTranslation;
	MTH3D_tdstVector *p_stNormal;

	ACP_tdxBool bStep;
	ACP_tdxBool bFirstGround;
	ACP_tdxBool bOk = TRUE;
	ACP_tdxBool bWallCheck = FALSE;

	MTH_tdxReal xMinSlope;
	MTH_tdxReal xMaxSlope;
	MTH_tdxReal xDotProduct;
	MTH_tdxReal xNormTangentMove;
	MTH_tdxReal xWallDistance = MTH_C_InfinitPlus;
	MTH_tdxReal xTmpReal;

	DNM_tdstMecObstacle *p_stGroundObstacle, *p_stWallObstacle, *p_stTmpObstacle, *p_stWallObstacle2, *p_stMobileObstacle;
	DNM_tdstObstacle *p_stObstacle;
	HIE_tdstSuperObject *hSupObjCol;

	xMinSlope = MEC_fn_xMinSlope ( p_stDynamic);
	xMaxSlope = MEC_fn_xMaxSlope ( p_stDynamic);
	MTH3D_M_vSetVectorElements( &stVertical, MTH_C_ZERO, MTH_C_ZERO, MTH_C_ONE);
	MTH3D_M_vNullVector (&stAdjustVector);
	p_stGroundObstacle = NULL;
	p_stWallObstacle = NULL;
	p_stObstacle = NULL;

	hCurrentMatrix = DNM_M_p_stDynamicsGetCurrentMatrix (p_stDynamic);
	hPreviousMatrix = DNM_M_p_stDynamicsGetPreviousMatrix (p_stDynamic);

	if (DNM_M_bDynamicsIsCollisionReport(p_stDynamic))
		p_stObstacle = DNM_M_p_stReportGetGround (DNM_M_p_stDynamicsGetReport (p_stDynamic));

	POS_fn_vCopyMatrix(p_stPOS_DownPosition, hCurrentMatrix);
	POS_fn_vCopyMatrix(p_stPOS_UpPosition, hPreviousMatrix);

	if (fn_ucCollSetGetCollComputeFrequencySO(hSupObj)>1)
	{
		hPreviousMatrix = &stPreviousMatrix;
		POS_fn_vCopyMatrix (hPreviousMatrix,DNM_M_p_stDynamicsGetPreviousMatrix (p_stDynamic));
		MEC_fn_vGetCollisionTranslation (p_stDynamic,hPreviousMatrix);
	}

	p_stCurrentTranslation = POS_fn_p_stGetTranslationVector ( hCurrentMatrix );
	p_stPreviousTranslation = POS_fn_p_stGetTranslationVector ( hPreviousMatrix );
	MTH3D_M_vSubVector (&stMove,p_stCurrentTranslation,p_stPreviousTranslation);

	if (DNM_M_bDynamicsIsMechanicChange(p_stDynamic) && !g_lCallCounter)
	{
		if (MEC_fn_bCollision(&lObstacle,g_a_stObstacle,p_stDynamic,hSupObj,hPreviousMatrix,hPreviousMatrix,TRUE))
		{
			MEC_fn_vDeleteBaseBackFace ( lObstacle,g_a_stObstacle, p_stDynamic );
			MEC_fn_vDeleteNullTranslationObstacle ( lObstacle, g_a_stObstacle );
			MEC_fn_vComputeAdjustVector (&stAdjustVector, p_stDynamic, lObstacle, g_a_stObstacle, lObstacle);
			MTH3D_M_vAddVector (p_stPreviousTranslation,p_stPreviousTranslation,&stAdjustVector);
		}
	}

	g_lCallCounter++;
	if (g_lCallCounter>5)
	{
		DNM_M_vDynamicsSetForceSafeWalk (p_stDynamic,TRUE);

		if (MEC_fn_bCollision(&lObstacle,g_a_stObstacle,p_stDynamic,hSupObj,hPreviousMatrix,hPreviousMatrix,TRUE))
		{
			MEC_fn_vDeleteBaseBackFace ( lObstacle,g_a_stObstacle, p_stDynamic );
			MEC_fn_vDeleteNullTranslationObstacle ( lObstacle, g_a_stObstacle );
			MEC_fn_vComputeAdjustVector (&stAdjustVector, p_stDynamic, lObstacle, g_a_stObstacle, lObstacle);
			MTH3D_M_vAddVector (p_stPreviousTranslation,p_stPreviousTranslation,&stAdjustVector);
		}

		MTH3D_M_vCopyVector (p_stCurrentTranslation,p_stPreviousTranslation);
		MTH3D_M_vNullVector ( DNM_M_pDynamicsGetPreviousSpeed(p_stDynamic) );
		return;
	}

	p_stNormal = &stVertical;

	if (p_stObstacle && !DNM_M_bDynamicsIsMechanicChange(p_stDynamic))
	{
		p_stNormal = DNM_M_p_stObstacleGetNorm ((DNM_tdstMecObstacle *)p_stObstacle);
		if (MTH3D_M_bIsNullVector(p_stNormal))
			p_stNormal = &stVertical;
	}

	xDotProduct = MTH3D_M_xDotProductVector (&stMove,p_stNormal);
	MTH3D_M_vMulScalarVector (&stNormalMove,xDotProduct,p_stNormal);
	MTH3D_M_vSubVector (&stTangentMove,&stMove,&stNormalMove);

	xNormTangentMove = MTH3D_M_xNormVector(&stTangentMove);

	if (g_lCallCounter==1)
	{
		g_bHighSpeed = (unsigned char)(MTH_M_bGreater(xNormTangentMove,C_ulDNM_RAYMAN_ZDM_RADIUS) || DNM_M_bDynamicsIsForceSafeWalk(p_stDynamic) || DNM_M_bDynamicsIsMechanicChange(p_stDynamic));
		DNM_M_vDynamicsSetForceSafeWalk ( p_stDynamic, FALSE );
	}
	else if (!g_bHighSpeed)
	{
		if (DNM_M_bDynamicsIsForceSafeWalk(p_stDynamic))
			g_bHighSpeed = TRUE;
	}

	if (DNM_M_bDynamicsIsComplexSize(p_stDynamic) && MEC_fn_bIsPlatform(p_stDynamic))
	{
		MTH3D_M_vSubVector (&stAbsoluteTangentMove,p_stCurrentTranslation,POS_fn_p_stGetTranslationVector(DNM_M_p_stDynamicsGetAbsolutePreviousMatrix(p_stDynamic)));
		xDotProduct = MTH3D_M_xDotProductVector (&stAbsoluteTangentMove,p_stNormal);
		MTH3D_M_vMulScalarVector (&stVector,xDotProduct,p_stNormal);
		MTH3D_M_vSubVector (&stAbsoluteTangentMove,&stAbsoluteTangentMove,&stVector);
	}
	else
		MTH3D_M_vCopyVector ( &stAbsoluteTangentMove, &stTangentMove );

	if ( MTH_M_bLess(xNormTangentMove,C_xDNM_NUL_MOVE) && MTH_M_bGreater(MTH3D_M_xNormVector(&stNormalMove),C_xDNM_NUL_MOVE) && ( !DNM_M_bDynamicsIsComplexSize(p_stDynamic) || !MEC_fn_bIsPlatform(p_stDynamic) ) && !DNM_M_bDynamicsIsMechanicChange(p_stDynamic) )
	{
		MEC_fn_vBaseObstacleManagement ( p_stDynamic, hSupObj, xDT );

		if (DNM_M_ulReportGetSurfaceState(&g_stReport)&C_WOT_ulGround)
		{
			p_stGroundObstacle = DNM_M_p_stReportGetGround (&g_stReport);
			xSlide = MEC_fn_xGetSlide (p_stDynamic,p_stGroundObstacle);
			if (MTH_M_bEqual(xSlide,MTH_C_ZERO)) DNM_M_vDynamicsSetNoGravity (p_stDynamic,TRUE);
		}

		return;
	}

	xSpeedZ = MTH_M_xMul ( MTH3D_M_xGetZofVector (DNM_M_pDynamicsGetPreviousSpeed(p_stDynamic)), xDT );

	xFactor = MTH_M_xMul (xMinSlope, xNormTangentMove);
	if (MTH_M_bGreater (xFactor,MTH_M_xNeg(C_xDNM_ON_GROUND_DEFAULT_NORMAL_MOVE))) xFactor = MTH_M_xNeg(C_xDNM_ON_GROUND_DEFAULT_NORMAL_MOVE);

	if (DNM_M_bDynamicsIsMechanicChange(p_stDynamic))
		if (MTH_M_bGreater (xFactor,xSpeedZ)) xFactor = xSpeedZ;

	p_stDownTranslation = POS_fn_p_stGetTranslationVector ( p_stPOS_DownPosition );
	MTH3D_M_vCopyVector ( p_stDownTranslation, p_stPreviousTranslation);
	MTH3D_M_vAddVector (p_stDownTranslation,p_stDownTranslation,&stTangentMove);
	MTH3D_M_vMulScalarVector (&stDownVector,xFactor,p_stNormal);
	MTH3D_M_vAddVector (p_stDownTranslation,p_stDownTranslation,&stDownVector);

	lObstacle2 = 0;
	if (g_bHighSpeed)
	{
		p_stUpTranslation = POS_fn_p_stGetTranslationVector ( p_stPOS_UpPosition );
		MTH3D_M_vCopyVector ( p_stUpTranslation, p_stPreviousTranslation);
		MTH3D_M_vAddVector (p_stUpTranslation,p_stUpTranslation,&stTangentMove);

		if (MEC_fn_bCollision(&lObstacle2,g_a_stObstacle,p_stDynamic,hSupObj,hPreviousMatrix,p_stPOS_UpPosition,TRUE))
		{
			for (lCounter=0; lCounter<lObstacle2; lCounter++)
			{
				p_stTmpObstacle = g_a_stObstacle+lCounter;
				if (MEC_fn_bIsObstacleValid(p_stTmpObstacle))
				{
					if (!MEC_fn_bWallObstacle(p_stDynamic,p_stTmpObstacle))
						MEC_fn_vInvalidateObstacle (p_stTmpObstacle);
				}
			}
		}
	}

	xFactor = MTH_M_xMul (xMaxSlope, xNormTangentMove);
	if (MTH_M_bLess (xFactor,C_xDNM_ON_GROUND_DEFAULT_NORMAL_MOVE)) xFactor = C_xDNM_ON_GROUND_DEFAULT_NORMAL_MOVE;

	p_stUpTranslation = POS_fn_p_stGetTranslationVector ( p_stPOS_UpPosition );
	MTH3D_M_vCopyVector ( p_stUpTranslation, p_stPreviousTranslation);
	MTH3D_M_vAddVector (p_stUpTranslation,p_stUpTranslation,&stTangentMove);
	MTH3D_M_vMulScalarVector (&stUpVector,xFactor,p_stNormal);
	MTH3D_M_vAddVector (p_stUpTranslation,p_stUpTranslation,&stUpVector);

	COL_g_lMaxNumberOfCollisions = COL_C_xMaxNumberOfCollisions - lObstacle2;
	MEC_fn_bCollision(&lObstacle,g_a_stObstacle+lObstacle2,p_stDynamic,hSupObj,p_stPOS_UpPosition,p_stPOS_DownPosition,TRUE);
	lObstacle += lObstacle2;
	COL_g_lMaxNumberOfCollisions = COL_C_xMaxNumberOfCollisions;

	if (lObstacle)
	{
		/* 0x436610 */
		p_stGroundObstacle = MEC_fn_pSelectGround (p_stDynamic,lObstacle-lObstacle2,g_a_stObstacle+lObstacle2,&stTangentMove,&bFirstGround);

		if (p_stGroundObstacle)
			bStep = MEC_fn_bDetectStep (p_stDynamic, lObstacle-lObstacle2, g_a_stObstacle+lObstacle2, p_stGroundObstacle, &stAbsoluteTangentMove );
		else
			bStep = FALSE;

		if (!bStep && (g_lCallCounter==1)) MEC_fn_vEdgeAnalysis (p_stDynamic, lObstacle-lObstacle2, g_a_stObstacle+lObstacle2, p_stGroundObstacle);

		if (bFirstGround) MEC_fn_vDeleteWallEdge (lObstacle, g_a_stObstacle, p_stDynamic);
		MEC_fn_vDeleteUnderGroundWall (lObstacle, g_a_stObstacle, p_stDynamic,p_stGroundObstacle);
		MEC_fn_vDeleteOnGroundBackFace (lObstacle, g_a_stObstacle, p_stDynamic, &stAbsoluteTangentMove, p_stGroundObstacle );

		if (MEC_fn_bMainActor(hSupObj) && (g_lCallCounter==1))
			MEC_fn_vCrashAnalysis ( p_stDynamic,lObstacle, g_a_stObstacle );

		p_stWallObstacle = NULL;
		p_stMobileObstacle = NULL;
		lObstacleCounter = 0;

		if (lObstacle2)
		{
			for (lCounter=0; lCounter<lObstacle2; lCounter++)
			{
				p_stTmpObstacle = g_a_stObstacle+lCounter;
				if (MEC_fn_bIsObstacleValid(p_stTmpObstacle))
				{
					if ( MEC_fn_bIsCollide(p_stTmpObstacle) && !MEC_fn_bGroundObstacle(p_stDynamic,p_stTmpObstacle) )
						bWallCheck = TRUE;
				}
			}
		}

		for (lCounter=0; lCounter<lObstacle; lCounter++)
		{
			p_stTmpObstacle = g_a_stObstacle+lCounter;
			if (MEC_fn_bIsObstacleValid(p_stTmpObstacle))
			{
				if (MEC_fn_bGroundObstacle (p_stDynamic,p_stTmpObstacle))
				{
					if (p_stGroundObstacle==p_stTmpObstacle)
						lObstacleCounter = lCounter;
				}
				else
				{
					if ( ((DNM_M_eObstacleGetType(p_stTmpObstacle)&DNM_ObsType_Mobile)==DNM_ObsType_Mobile) && !p_stMobileObstacle )
					{
						hSupObjCol = (HIE_tdstSuperObject *)DNM_M_p_stObstacleGetObject (p_stTmpObstacle);

						if (HIE_fn_bIsSuperObjectValid(hSupObjCol))
						{
							hSupObjCol = MEC_fn_hGetFatherActor (hSupObjCol);

							if ((fn_ulStandardGameGetCustomBitsSO(hSupObjCol) & C_ulDNM_CAN_PUSH_RAYMAN_CUSTOM_BIT) == C_ulDNM_CAN_PUSH_RAYMAN_CUSTOM_BIT)
							{
								if (!MEC_fn_bGroundObstacle(p_stDynamic,p_stTmpObstacle))
									p_stMobileObstacle = p_stTmpObstacle;
								if (MEC_fn_bCeilObstacle(p_stDynamic,p_stTmpObstacle) && (g_lCallCounter==1))
									MEC_fn_vInvalidateObstacle (p_stTmpObstacle);
							}
						}
					}

					if (MEC_fn_bWallObstacle (p_stDynamic,p_stTmpObstacle))
					{
						MTH3D_M_vSubVector ( &stVector, DNM_M_p_stObstacleGetContact(p_stTmpObstacle), p_stPreviousTranslation );

						xTmpReal = MTH_M_xAdd ( MTH_M_xSqr(MTH3D_M_xGetXofVector(&stVector)),MTH_M_xSqr(MTH3D_M_xGetYofVector(&stVector)) );

						if (MTH_M_bLess(xTmpReal,xWallDistance))
						{
							p_stWallObstacle = p_stTmpObstacle;
							lObstacleCounter = lCounter;
							xWallDistance = xTmpReal;
						}
					}
					else
					{

						if (!p_stWallObstacle)
						{
							p_stWallObstacle = p_stTmpObstacle;
							lObstacleCounter = lCounter;
						}
					}
				}
			}
		}

		MEC_fn_vReportCollision (p_stDynamic,hSupObj,lObstacle,g_a_stObstacle,lObstacleCounter);

		if (DNM_M_bDynamicsIsCanFall(p_stDynamic) && DNM_M_bDynamicsIsNeverFall(p_stDynamic) && !p_stWallObstacle)
		{
			MTH3D_M_vAddVector (p_stCurrentTranslation,p_stPreviousTranslation,&stTangentMove);
			MEC_fn_vComputeSpeed (p_stDynamic,xDT);
			return;
		}

		if (p_stGroundObstacle)
		{
			xSlide = MEC_fn_xGetSlide (p_stDynamic,p_stGroundObstacle);
			if (MTH_M_bEqual(xSlide,MTH_C_ZERO)) DNM_M_vDynamicsSetNoGravity (p_stDynamic,TRUE);

			if(!p_stWallObstacle)
			{

				MTH3D_M_vAddVector (&stVector, p_stDownTranslation, DNM_M_xObstacleGetTranslation (p_stGroundObstacle));
				POS_fn_vSetTranslationVector ( hCurrentMatrix, &stVector );

				if ( (g_lCallCounter==1) && ( DNM_M_bDynamicsIsCollisionReport(p_stDynamic) && ((DNM_M_ulReportGetPrevSurfaceState(DNM_M_p_stDynamicsGetReport(p_stDynamic))&C_WOT_ulWall)!=C_WOT_ulWall) ))
					DNM_M_vDynamicsSetWallAdjust(p_stDynamic,TRUE);
			}
			else
			{
				if ( p_stMobileObstacle && (g_lCallCounter==1) )
				{

					DNM_M_vDynamicsSetForceSafeWalk (p_stDynamic,TRUE);
					MTH3D_M_vCopyVector (&stGroundNormal, DNM_M_p_stObstacleGetNorm(p_stGroundObstacle));
					p_stWallObstacle2 = NULL;

					if(MEC_fn_bCollision(&lObstacle,g_a_stObstacle,p_stDynamic,hSupObj,hPreviousMatrix,hPreviousMatrix,TRUE))
					{
						MEC_fn_vDeleteBaseBackFace ( lObstacle,g_a_stObstacle, p_stDynamic );
						MEC_fn_vDeleteNullTranslationObstacle ( lObstacle, g_a_stObstacle );

						for (lCounter=0; lCounter<lObstacle && !p_stWallObstacle2; lCounter++)
						{
							p_stTmpObstacle = g_a_stObstacle + lCounter;
							if ( MEC_fn_bIsObstacleValid(p_stTmpObstacle) && ((DNM_M_eObstacleGetType(p_stTmpObstacle)&DNM_ObsType_Mobile)==DNM_ObsType_Mobile) )
								p_stWallObstacle2 = p_stTmpObstacle;
						}

						if (p_stWallObstacle2)
						{
							xDotProduct = MTH_M_xSub ( MTH3D_M_xDotProductVector (DNM_M_xObstacleGetTranslation(p_stWallObstacle2),&stGroundNormal), MTH_M_xConstFloatToReal(0.001f) );
							MTH3D_M_vMulScalarVector (&stVector,xDotProduct,&stGroundNormal);
							MTH3D_M_vSubVector (&stVector,DNM_M_xObstacleGetTranslation(p_stWallObstacle2),&stVector);
							MTH3D_M_vAddVector ( p_stCurrentTranslation, p_stPreviousTranslation, &stVector );

							if(MEC_fn_bCollision(&lObstacle,g_a_stObstacle,p_stDynamic,hSupObj,hPreviousMatrix,hCurrentMatrix,TRUE))
							{
								MEC_fn_vDeleteBaseBackFace ( lObstacle,g_a_stObstacle, p_stDynamic );
								MEC_fn_vDeleteNullTranslationObstacle ( lObstacle, g_a_stObstacle );
								MEC_fn_vComputeAdjustVector (&stAdjustVector, p_stDynamic, lObstacle, g_a_stObstacle, lObstacle);
								MTH3D_M_vAddVector ( p_stPreviousTranslation, p_stCurrentTranslation, &stAdjustVector );
								MTH3D_M_vAddVector ( p_stCurrentTranslation, p_stPreviousTranslation, &stMove );
							}
						}
					}

					if (!MEC_fn_bIsNullVector(&stMove))
						MEC_fn_vGroundObstacleManagement ( p_stDynamic, hSupObj, xDT );
				}
				else
				{
					if (DNM_M_bDynamicsIsWallAdjust(p_stDynamic) && ( DNM_M_bDynamicsIsCollisionReport(p_stDynamic) && ((DNM_M_ulReportGetPrevSurfaceState(DNM_M_p_stDynamicsGetReport(p_stDynamic))&C_WOT_ulWall)!=C_WOT_ulWall) ) )
					{
						bWallCheck=TRUE;
						DNM_M_vDynamicsSetWallAdjust(p_stDynamic,FALSE);
					}

					DNM_M_vDynamicsSetForceSafeWalk (p_stDynamic,TRUE);

					if ( ( DNM_M_bDynamicsIsMechanicChange(p_stDynamic) || (DNM_M_bDynamicsIsComplexSize(p_stDynamic) && MEC_fn_bIsPlatform(p_stDynamic)) || bWallCheck ) && MEC_fn_bIsCollide(p_stWallObstacle) && MTH_M_bEqualZero(xSlide) )
					{
						bOk = MEC_fn_vComputeAdjustVector (&stAdjustVector, p_stDynamic, lObstacle-lObstacle2, g_a_stObstacle+lObstacle2, lObstacle-lObstacle2);
						if (lObstacle2)
						{
							bOk |= MEC_fn_vComputeAdjustVector (&stAdjustVector1, p_stDynamic, lObstacle2, g_a_stObstacle, lObstacle2);
						}
						else
						{
							MTH3D_M_vNullVector (&stAdjustVector1);
						}

						if ( !lObstacle2 || MEC_fn_bIsNullVector(&stAdjustVector) || MEC_fn_bIsNullVector(&stAdjustVector1) )
						{
							MTH3D_M_vAddVector ( &stAdjustVector, &stAdjustVector, &stAdjustVector1 );
						}
						else
						{
							MTH3D_M_vNormalizeVector ( &stVector, &stAdjustVector1 );
							xDotProduct = MTH3D_M_xDotProductVector (&stAdjustVector,&stVector);
							if (MTH_M_bLess(xDotProduct,MTH3D_M_xNormVector(&stAdjustVector1)))
							{
								MTH3D_M_vMulScalarVector (&stVector,xDotProduct,&stVector);
								MTH3D_M_vSubVector (&stAdjustVector,&stAdjustVector,&stVector);
								MTH3D_M_vAddVector (&stAdjustVector,&stAdjustVector,&stAdjustVector1);
							}
						}

						/* 0x0043C6BD */
						if (!DNM_M_bDynamicsIsSwim(p_stDynamic) && !DNM_M_bDynamicsIsMechanicChange(p_stDynamic))
						{
							xDotProduct = MTH3D_M_xDotProductVector (&stAdjustVector,DNM_M_p_stObstacleGetNorm(p_stGroundObstacle));
							MTH3D_M_vMulScalarVector (&stVector,xDotProduct,DNM_M_p_stObstacleGetNorm(p_stGroundObstacle));
							MTH3D_M_vSubVector (&stAdjustVector,&stAdjustVector,&stVector);
							MTH3D_M_vAddVector (&stAdjustVector,&stAdjustVector,DNM_M_xObstacleGetTranslation (p_stGroundObstacle));

							if (MTH_M_bGreater(MTH3D_M_xGetZofVector(DNM_M_p_stObstacleGetNorm(p_stWallObstacle)),MTH_M_xConstFloatToReal(0.5f)))
							{
								MTH3D_M_vSubVector (&stContactVector,DNM_M_p_stObstacleGetContact(p_stWallObstacle),DNM_M_p_stObstacleGetContact(p_stGroundObstacle));
								xCosAngle = MTH3D_M_xDotProductVector (DNM_M_p_stObstacleGetNorm(p_stWallObstacle),DNM_M_p_stObstacleGetNorm(p_stGroundObstacle));
								xTmpReal = MTH_M_xDiv ( MTH3D_M_xDotProductVector (DNM_M_p_stObstacleGetNorm(p_stGroundObstacle),&stContactVector), MTH_M_xTan(MTH_M_xACos(xCosAngle)) );
								MTH3D_M_vMulScalarVector (&stVector,xCosAngle,DNM_M_p_stObstacleGetNorm(p_stGroundObstacle));
								MTH3D_M_vSubVector (&stVector,DNM_M_p_stObstacleGetNorm(p_stWallObstacle),&stVector);
								if (MEC_fn_bIsNullVector(&stVector))
								{	MTH3D_M_vNullVector (&stVector);	}
								else
								{	MTH3D_M_vNormalizeVector (&stVector,&stVector);	}
								MTH3D_M_vMulScalarVector (&stVector,xTmpReal,&stVector);
								MTH3D_M_vAddVector (&stAdjustVector,&stAdjustVector,&stVector);
							}
						}

						MTH3D_M_vAddVector (&stSafeTranslation,  p_stDownTranslation, &stAdjustVector);
						MTH3D_M_vSubVector (&stVector, &stSafeTranslation, p_stPreviousTranslation);
						MTH3D_M_vSubVector (&stMove,&stMove,&stVector);
					}
					else

						MTH3D_M_vCopyVector (&stSafeTranslation,p_stPreviousTranslation);

					MTH3D_M_vCopyVector (p_stCurrentTranslation,&stSafeTranslation);

					MEC_fn_vWallObstacleManagement ( &stWallMove, p_stDynamic, &stMove, DNM_M_p_stObstacleGetNorm(p_stGroundObstacle), DNM_M_p_stObstacleGetNorm(p_stWallObstacle), xDT, FALSE );

					if (bOk && !(DNM_M_bDynamicsIsComplexSize(p_stDynamic) && MEC_fn_bIsPlatform(p_stDynamic)))
					{
						MTH3D_M_vSubVector ( &stVector, &stMove, &stWallMove );
						{
							if (!MEC_fn_bIsNullVector(&stWallMove))
								MEC_fn_vGroundObstacleManagement (p_stDynamic, hSupObj, xDT);
							else
							{

								MTH3D_M_vCopyVector (p_stCurrentTranslation,&stSafeTranslation);
							}
						}
					}
					else
					{
						if (MEC_fn_bCollision(&lObstacle,g_a_stObstacle,p_stDynamic,hSupObj,hCurrentMatrix,hCurrentMatrix,TRUE))
						{
							MEC_fn_vDeleteBaseBackFace ( lObstacle,g_a_stObstacle, p_stDynamic );
							MEC_fn_vDeleteNullTranslationObstacle ( lObstacle, g_a_stObstacle );

							for (lCounter=0; lCounter<lObstacle; lCounter++)
							{
								p_stTmpObstacle = g_a_stObstacle + lCounter;
								if ( MEC_fn_bIsObstacleValid(p_stTmpObstacle) && ((DNM_M_eObstacleGetType(p_stTmpObstacle)&DNM_ObsType_Mobile)==DNM_ObsType_Mobile) && !p_stMobileObstacle && DNM_M_p_stObstacleGetObject(p_stTmpObstacle) )
								{
									hSupObjCol = MEC_fn_hGetFatherActor ((HIE_tdstSuperObject *)DNM_M_p_stObstacleGetObject (p_stTmpObstacle));
									if ( ((fn_ulStandardGameGetCustomBitsSO(hSupObjCol) & C_ulDNM_CAN_PUSH_RAYMAN_CUSTOM_BIT) == C_ulDNM_CAN_PUSH_RAYMAN_CUSTOM_BIT) && MEC_fn_bCeilObstacle(p_stDynamic,p_stTmpObstacle) )
										MEC_fn_vInvalidateObstacle (p_stTmpObstacle);
								}
							}
							MEC_fn_vComputeAdjustVector (&stAdjustVector, p_stDynamic, lObstacle, g_a_stObstacle, lObstacle);
							MTH3D_M_vAddVector (p_stCurrentTranslation,p_stCurrentTranslation,&stAdjustVector);
						}

						MEC_fn_vComputeSpeed (p_stDynamic,xDT);
						return;
					}
				}
			}
		}
		else
		{
			if ( p_stWallObstacle )
			{
				if ( DNM_M_bDynamicsIsCollisionReport(p_stDynamic) && ((DNM_M_ulReportGetPrevSurfaceState(DNM_M_p_stDynamicsGetReport(p_stDynamic))&C_WOT_ulWall)!=C_WOT_ulWall) )
					bWallCheck=TRUE;

				MEC_fn_vComputeAdjustVector (&stAdjustVector, p_stDynamic, lObstacle-lObstacle2, g_a_stObstacle+lObstacle2, lObstacle-lObstacle2);
				if (lObstacle2)
					MEC_fn_vComputeAdjustVector (&stAdjustVector1, p_stDynamic, lObstacle2, g_a_stObstacle, lObstacle2);
				else
					MTH3D_M_vNullVector (&stAdjustVector1);

				if ( !lObstacle2 || MEC_fn_bIsNullVector(&stAdjustVector) || MEC_fn_bIsNullVector(&stAdjustVector1) )
				{
					MTH3D_M_vAddVector ( &stAdjustVector, &stAdjustVector, &stAdjustVector1 );
				}
				else
				{
					MTH3D_M_vNormalizeVector ( &stVector, &stAdjustVector1 );
					xDotProduct = MTH3D_M_xDotProductVector (&stAdjustVector,&stVector);
					if (MTH_M_bLess(xDotProduct,MTH3D_M_xNormVector(&stAdjustVector1)))
					{
						MTH3D_M_vMulScalarVector (&stVector,xDotProduct,&stVector);
						MTH3D_M_vSubVector (&stAdjustVector,&stAdjustVector,&stVector);
						MTH3D_M_vAddVector (&stAdjustVector,&stAdjustVector,&stAdjustVector1);
					}
				}

				MTH3D_M_vAddVector (&stSafeTranslation,  p_stDownTranslation, &stAdjustVector);

				MTH3D_M_vSubVector (&stVector, &stSafeTranslation, p_stPreviousTranslation);
				MTH3D_M_vSubVector (&stMove,&stMove,&stVector);

				DNM_M_vDynamicsSetForceSafeWalk (p_stDynamic,TRUE);

				MTH3D_M_vCopyVector (p_stCurrentTranslation,&stSafeTranslation);

				MEC_fn_vWallObstacleManagement ( &stWallMove, p_stDynamic, &stMove, p_stNormal, DNM_M_p_stObstacleGetNorm(p_stWallObstacle), xDT, FALSE );

				if (!MEC_fn_bIsNullVector(&stWallMove)) MEC_fn_vGroundObstacleManagement ( p_stDynamic, hSupObj, xDT );
				else
					MTH3D_M_vCopyVector (p_stCurrentTranslation,&stSafeTranslation);
			}
			else
			{
				DNM_M_vDynamicsSetForceSafeWalk (p_stDynamic,TRUE);

				if ( MTH_M_bGreater(MTH3D_M_xNormVector(&stNormalMove),C_ulDNM_RAYMAN_ZDM_RADIUS) && MTH_M_bLessZero(MTH3D_M_xGetZofVector(&stNormalMove)) )
					POS_fn_vCopyMatrix ( hCurrentMatrix, p_stPOS_DownPosition );

				if (DNM_M_bDynamicsIsNeverFall(p_stDynamic))
					MTH3D_M_vAddVector (p_stCurrentTranslation,p_stPreviousTranslation,&stTangentMove);

				if (g_lCallCounter>1)
					POS_fn_vCopyMatrix ( hCurrentMatrix, hPreviousMatrix );

				DNM_M_vDynamicsSetStop (p_stDynamic,FALSE);
			}
		}

		MEC_fn_vComputeSpeed (p_stDynamic,xDT);
		return;
	}
	DNM_M_vDynamicsSetForceSafeWalk (p_stDynamic,TRUE);

	if ( MTH_M_bGreater(MTH3D_M_xNormVector(&stNormalMove),C_ulDNM_RAYMAN_ZDM_RADIUS) && MTH_M_bLessZero(MTH3D_M_xGetZofVector(&stNormalMove)) )
		POS_fn_vCopyMatrix ( hCurrentMatrix, p_stPOS_DownPosition );

	if (DNM_M_bDynamicsIsNeverFall(p_stDynamic))
		MTH3D_M_vAddVector (p_stCurrentTranslation,p_stPreviousTranslation,&stTangentMove);

	if (g_lCallCounter>1)
		POS_fn_vCopyMatrix ( hCurrentMatrix, hPreviousMatrix );

	DNM_M_vDynamicsSetStop (p_stDynamic,FALSE);
	MEC_fn_vComputeSpeed (p_stDynamic,xDT);
	return;
}

void __cdecl MEC_fn_vGiObstacleManagement ( DNM_tdstDynamics *p_stDynamic, HIE_tdstSuperObject *hSupObj, MTH_tdxReal xDT )
{
	long lObstacle;
	long lCounter=0;
	long lCounter1=0;
	long lFace=0;
	ACP_tdxBool bGiFace = FALSE;

	MTH_tdxReal xNormalMove;
	MTH_tdxReal	xSlope;
	MTH_tdxReal	xDotProd;

	MTH3D_tdstVector *p_stCurrentTranslation;
	MTH3D_tdstVector *p_stPreviousTranslation;
	MTH3D_tdstVector *p_stDownTranslation;
	MTH3D_tdstVector *p_stUpTranslation;
	MTH3D_tdstVector *p_stGiNormal, *p_stGiContact;
	MTH3D_tdstVector stVector;
	MTH3D_tdstVector stNormalMove, stTangentMove, stMove, stSlideMove;
	MTH3D_tdstVector stVerticalVector;
	MTH3D_tdstVector stAdjustVector;

	DNM_tdstMecObstacle *p_stTmpObstacle;
	DNM_tdstMecObstacle *p_stGiObstacle, *p_stSlideObstacle;
	DNM_tdstObstacle *p_stPreviousCeilObstacle;

	POS_tdstCompletePosition a2_stPosition[2];
	POS_tdstCompletePosition *p_stPOS_DownPosition = a2_stPosition;
	POS_tdstCompletePosition *p_stPOS_UpPosition = a2_stPosition+1;
 	POS_tdxHandleToPosition hCurrentMatrix, hPreviousMatrix;

	xSlope = MEC_fn_xMaxSlope (p_stDynamic);

	hCurrentMatrix = DNM_M_p_stDynamicsGetCurrentMatrix (p_stDynamic);
	hPreviousMatrix = DNM_M_p_stDynamicsGetPreviousMatrix (p_stDynamic);
	MTH3D_M_vSetVectorElements( &stVerticalVector, MTH_C_ZERO, MTH_C_ZERO, MTH_C_ONE );

	POS_fn_vCopyMatrix(p_stPOS_UpPosition, hCurrentMatrix);
	POS_fn_vCopyMatrix(p_stPOS_DownPosition, hCurrentMatrix);

	p_stCurrentTranslation = POS_fn_p_stGetTranslationVector ( hCurrentMatrix );
	p_stPreviousTranslation = POS_fn_p_stGetTranslationVector ( hPreviousMatrix );
	MTH3D_M_vSubVector (&stMove,p_stCurrentTranslation,p_stPreviousTranslation);

	g_lCallCounter++;
	if (g_lCallCounter>5)
	{

		MTH3D_M_vCopyVector (p_stCurrentTranslation,p_stPreviousTranslation);
		MTH3D_M_vNullVector ( DNM_M_pDynamicsGetPreviousSpeed(p_stDynamic) );
		return;
	}

	xNormalMove = MTH_M_xMul ( xSlope, MTH3D_M_xNormVector(&stMove) );

	if (MTH_M_bLess (xNormalMove,C_xDNM_CLIMB_DEFAULT_NORMAL_MOVE)) xNormalMove = C_xDNM_GI_DEFAULT_NORMAL_MOVE;

	p_stGiNormal = NULL;
	p_stGiContact = NULL;
	p_stPreviousCeilObstacle = DNM_M_p_stReportGetCeil (DNM_M_p_stDynamicsGetReport(p_stDynamic));
	if (p_stPreviousCeilObstacle)
	{
		p_stGiNormal = DNM_M_p_stObstacleGetNorm(p_stPreviousCeilObstacle);
		p_stGiContact = DNM_M_p_stObstacleGetContact(p_stPreviousCeilObstacle);
	}
	else
	{
		p_stGiNormal = &stVerticalVector;
	}

	MTH3D_M_vMulScalarVector ( &stNormalMove, MTH3D_M_xDotProductVector( &stMove, p_stGiNormal ), p_stGiNormal);
	MTH3D_M_vSubVector ( &stTangentMove, &stMove, &stNormalMove );
	MTH3D_M_vMulScalarVector ( &stVector, xNormalMove, p_stGiNormal);

	p_stDownTranslation = POS_fn_p_stGetTranslationVector ( p_stPOS_DownPosition );
	MTH3D_M_vCopyVector ( p_stDownTranslation, p_stPreviousTranslation );
	MTH3D_M_vAddVector ( p_stDownTranslation, p_stDownTranslation, &stTangentMove );
	MTH3D_M_vAddVector ( p_stDownTranslation, p_stDownTranslation, &stVector );

	p_stUpTranslation = POS_fn_p_stGetTranslationVector ( p_stPOS_UpPosition );
	MTH3D_M_vCopyVector ( p_stUpTranslation, p_stPreviousTranslation );
	MTH3D_M_vAddVector ( p_stUpTranslation, p_stUpTranslation, &stTangentMove );
	MTH3D_M_vSubVector ( p_stUpTranslation, p_stUpTranslation, &stVector );

	p_stGiObstacle = NULL;
	p_stSlideObstacle = NULL;
	p_stPreviousCeilObstacle = NULL;

	if(MEC_fn_bCollision(&lObstacle,g_a_stObstacle,p_stDynamic,hSupObj,p_stPOS_DownPosition,p_stPOS_UpPosition,TRUE))
	{
		MEC_fn_vDeleteGiBackFace (lObstacle,g_a_stObstacle,p_stDynamic,&stMove);

		for (lCounter=0; lCounter<lObstacle && (!p_stGiObstacle || !bGiFace); lCounter++)
		{
			p_stTmpObstacle = g_a_stObstacle + lCounter;
			if (MEC_fn_bIsObstacleValid(p_stTmpObstacle))
			{
				if ( (MEC_fn_bTypeOfGameMaterial((GMT_tdxHandleToGameMaterial)DNM_M_hObstacleGetCollidedMaterial(p_stTmpObstacle), C_xDNM_GI_TYPE_GAME_MATERIAL)) && !(MEC_fn_bTypeOfGameMaterial((GMT_tdxHandleToGameMaterial)DNM_M_hObstacleGetCollidedMaterial(p_stTmpObstacle), C_xDNM_NO_COLLISION_TYPE_GAME_MATERIAL)) && (MEC_fn_bCeilObstacle (p_stDynamic,p_stTmpObstacle)) )
				{
					if (!p_stGiObstacle)
						p_stGiObstacle = p_stTmpObstacle;
					if (!MEC_fn_bIsEdge(p_stTmpObstacle))
						bGiFace = TRUE;
				}
			}
		}

		if (p_stGiObstacle)
		{
			p_stGiNormal = DNM_M_p_stObstacleGetNorm(p_stGiObstacle);
			p_stGiContact = DNM_M_p_stObstacleGetContact(p_stGiObstacle);
		}

		for (lCounter1=0; lCounter1<lObstacle && !p_stSlideObstacle; lCounter1++)
		{
			p_stTmpObstacle = g_a_stObstacle + lCounter1;
			if (MEC_fn_bIsObstacleValid(p_stTmpObstacle))
			{
				if
				(
					!MEC_fn_bTypeOfGameMaterial((GMT_tdxHandleToGameMaterial)DNM_M_hObstacleGetCollidedMaterial(p_stTmpObstacle), C_xDNM_GI_TYPE_GAME_MATERIAL)
				||	MEC_fn_bTypeOfGameMaterial((GMT_tdxHandleToGameMaterial)DNM_M_hObstacleGetCollidedMaterial(p_stTmpObstacle), C_xDNM_NO_COLLISION_TYPE_GAME_MATERIAL)
				)
				{
					xDotProd = MTH3D_M_xDotProductVector ( DNM_M_p_stObstacleGetNorm(p_stTmpObstacle), p_stGiNormal );
					if (MTH_M_bLess(xDotProd,MTH_M_xConstFloatToReal(0.707f)))
					{
						MTH3D_M_vSubVector ( &stVector, DNM_M_p_stObstacleGetContact(p_stTmpObstacle), p_stGiContact );
						xDotProd = MTH3D_M_xDotProductVector ( &stVector, p_stGiNormal );
						if (MTH_M_bGreater ( xDotProd, MTH_M_xConstFloatToReal(-0.01f) ))
							p_stSlideObstacle = p_stTmpObstacle;
					}
				}
			}
		}

		if (p_stSlideObstacle && lCounter1>lCounter) lCounter=lCounter1;

		MEC_fn_vReportCollision (p_stDynamic,hSupObj,lObstacle,g_a_stObstacle,lCounter-1);

		if (p_stGiObstacle && MEC_fn_bIsEdge(p_stGiObstacle) && !p_stSlideObstacle && !bGiFace)
		{
			for ( lCounter1=0; lCounter1<lObstacle; lCounter1++ )
			{
				p_stTmpObstacle = g_a_stObstacle+lCounter1;

				if (MEC_fn_bIsObstacleValid(p_stTmpObstacle))
				{
					MTH3D_M_vSubVector ( &stVector, p_stGiContact, DNM_M_p_stObstacleGetContact(p_stTmpObstacle) );
					if (MEC_fn_bIsNullVector(&stVector))
					{
						lFace++;
					}
				}
			}

			if (lFace==1 && !MEC_fn_bIsDoubleEdge(p_stGiObstacle))
			{

				if (!DNM_M_bDynamicsIsMechanicChange(p_stDynamic)) DNM_M_vDynamicsSetStop (p_stDynamic,TRUE);
				MTH3D_M_vCopyVector (p_stCurrentTranslation,p_stPreviousTranslation);
				MTH3D_M_vNullVector ( DNM_M_pDynamicsGetPreviousSpeed(p_stDynamic) );

				if (DNM_M_bDynamicsIsCollisionReport(p_stDynamic))
					DNM_M_ulReportSetSurfaceState(DNM_M_p_stDynamicsGetReport(p_stDynamic),DNM_M_ulReportGetPrevSurfaceState (DNM_M_p_stDynamicsGetReport(p_stDynamic)) );
				return;
			}
		}

		if (p_stGiObstacle)
		{
			if(!p_stSlideObstacle)
			{
				MTH3D_M_vAddVector (&stVector, p_stUpTranslation, DNM_M_xObstacleGetTranslation (p_stGiObstacle));
				POS_fn_vSetTranslationVector ( hCurrentMatrix, &stVector );
			}
			else
			{

				if ( p_stSlideObstacle && MEC_fn_bIsCollide(p_stSlideObstacle) && (g_lCallCounter>2) )
				{
					MEC_fn_vComputeAdjustVector (&stAdjustVector, p_stDynamic, lObstacle, g_a_stObstacle, lCounter);
					MTH3D_M_vAddVector (p_stCurrentTranslation, p_stUpTranslation, &stAdjustVector);
					MTH3D_M_vSubVector (&stVector, p_stCurrentTranslation, p_stPreviousTranslation);
					MTH3D_M_vSubVector (&stMove,&stMove,&stVector);
					MTH3D_M_vCopyVector (p_stPreviousTranslation,p_stCurrentTranslation);
				}
				else

					MTH3D_M_vCopyVector (p_stCurrentTranslation,p_stPreviousTranslation);

				MEC_fn_vWallObstacleManagement ( &stSlideMove, p_stDynamic, &stMove, DNM_M_p_stObstacleGetNorm(p_stGiObstacle), DNM_M_p_stObstacleGetNorm(p_stSlideObstacle), xDT, FALSE );

				if (!MEC_fn_bIsNullVector(&stSlideMove))
				{
					MEC_fn_vGiObstacleManagement (p_stDynamic, hSupObj, xDT);
					MEC_fn_vComputeSpeed (p_stDynamic,xDT);
					return;
				}
				else MTH3D_M_vCopyVector (p_stCurrentTranslation,p_stPreviousTranslation);
			}
		}
		else
		{
			if (p_stSlideObstacle)
			{

				if ( p_stSlideObstacle && MEC_fn_bIsCollide(p_stSlideObstacle) )
				{
					MEC_fn_vComputeAdjustVector (&stAdjustVector, p_stDynamic, lObstacle, g_a_stObstacle, lCounter);
					MTH3D_M_vAddVector (p_stCurrentTranslation, p_stUpTranslation, &stAdjustVector);
					MTH3D_M_vSubVector (&stVector, p_stCurrentTranslation, p_stPreviousTranslation);
					MTH3D_M_vSubVector (&stMove,&stMove,&stVector);
					MTH3D_M_vCopyVector (p_stPreviousTranslation,p_stCurrentTranslation);
				}
				else

					MTH3D_M_vCopyVector (p_stCurrentTranslation,p_stPreviousTranslation);

				MEC_fn_vWallObstacleManagement ( &stSlideMove, p_stDynamic, &stMove, p_stGiNormal, DNM_M_p_stObstacleGetNorm(p_stSlideObstacle), xDT, FALSE );

				if (!MEC_fn_bIsNullVector(&stSlideMove))
				{
					MEC_fn_vGiObstacleManagement ( p_stDynamic, hSupObj, xDT );
					MEC_fn_vComputeSpeed (p_stDynamic,xDT);
					return;
				}
				else MTH3D_M_vCopyVector (p_stCurrentTranslation,p_stPreviousTranslation);
			}
			else
			{
				POS_fn_vCopyMatrix ( hCurrentMatrix, hPreviousMatrix );
				MTH3D_M_vNullVector ( DNM_M_pDynamicsGetPreviousSpeed(p_stDynamic) );
				if (!DNM_M_bDynamicsIsMechanicChange(p_stDynamic)) DNM_M_vDynamicsSetStop ( p_stDynamic, TRUE );

				if (DNM_M_bDynamicsIsCollisionReport(p_stDynamic))
					DNM_M_ulReportSetSurfaceState(DNM_M_p_stDynamicsGetReport(p_stDynamic),DNM_M_ulReportGetPrevSurfaceState (DNM_M_p_stDynamicsGetReport(p_stDynamic)) );
				return;
			}
		}

		MEC_fn_vComputeSpeed (p_stDynamic,xDT);
		return;
	}

	POS_fn_vCopyMatrix ( hCurrentMatrix, hPreviousMatrix );
	MTH3D_M_vNullVector ( DNM_M_pDynamicsGetPreviousSpeed(p_stDynamic) );
	if (!DNM_M_bDynamicsIsMechanicChange(p_stDynamic)) DNM_M_vDynamicsSetStop (p_stDynamic,TRUE);

	if (DNM_M_bDynamicsIsCollisionReport(p_stDynamic))
		DNM_M_ulReportSetSurfaceState(DNM_M_p_stDynamicsGetReport(p_stDynamic),DNM_M_ulReportGetPrevSurfaceState (DNM_M_p_stDynamicsGetReport(p_stDynamic)) );

	return;
}

#pragma optimize("y", off)
void __cdecl MEC_fn_vSpiderObstacleManagement ( DNM_tdstDynamics *p_stDynamic, HIE_tdstSuperObject *hSupObj, MTH_tdxReal xDT )
{
	long lObstacle;
	long lCounter;
	DNM_tdstMecObstacle *p_stObstacle, *p_stTmpObstacle;
	MTH_tdxReal xNormalMove;
	MTH3D_tdstVector stVector;

	POS_tdstCompletePosition a2_stPosition[2];
	POS_tdstCompletePosition *p_stPOS_StartPosition = a2_stPosition;
	POS_tdstCompletePosition *p_stPOS_EndPosition = a2_stPosition+1;
 	POS_tdxHandleToPosition hCurrentMatrix, hPreviousMatrix;
	MTH3D_tdstVector *p_stCurrentTranslation;
	MTH3D_tdstVector *p_stPreviousTranslation;
	MTH3D_tdstVector *p_stStartTranslation;
	MTH3D_tdstVector *p_stEndTranslation;
	MTH3D_tdstVector stMove;
	MTH3D_tdstVector *p_stX, *p_stY, *p_stZ;

	hCurrentMatrix = DNM_M_p_stDynamicsGetCurrentMatrix (p_stDynamic);
	hPreviousMatrix = DNM_M_p_stDynamicsGetPreviousMatrix (p_stDynamic);

	POS_fn_vCopyMatrix(p_stPOS_StartPosition, hCurrentMatrix);
	POS_fn_vCopyMatrix(p_stPOS_EndPosition, hCurrentMatrix);

	p_stCurrentTranslation = POS_fn_p_stGetTranslationVector ( hCurrentMatrix );
	p_stPreviousTranslation = POS_fn_p_stGetTranslationVector ( hPreviousMatrix );
	MTH3D_M_vSubVector (&stMove,p_stCurrentTranslation,p_stPreviousTranslation);

	POS_fn_vGetRotationVector ( hCurrentMatrix, &p_stX, &p_stY, &p_stZ );
	xNormalMove = MTH_M_xMul ( C_xDNM_SPIDER_SLOPE_LIMIT, MTH3D_M_xNormVector(&stMove) );
	if (MTH_M_bLess (xNormalMove,C_xDNM_SPIDER_DEFAULT_NORMAL_MOVE))
		xNormalMove = C_xDNM_SPIDER_DEFAULT_NORMAL_MOVE;

	p_stStartTranslation = POS_fn_p_stGetTranslationVector ( p_stPOS_StartPosition );
	MTH3D_M_vMulScalarVector (&stVector, xNormalMove, p_stZ);
	MTH3D_M_vAddVector ( p_stStartTranslation, p_stCurrentTranslation, &stVector );

	p_stEndTranslation = POS_fn_p_stGetTranslationVector ( p_stPOS_EndPosition );
	MTH3D_M_vMulScalarVector (&stVector, xNormalMove, p_stZ);
	MTH3D_M_vSubVector ( p_stEndTranslation, p_stCurrentTranslation, &stVector );

	if(MEC_fn_bCollision(&lObstacle,g_a_stObstacle,p_stDynamic,hSupObj,p_stPOS_StartPosition,p_stPOS_EndPosition,TRUE))
	{
		MTH3D_tdstVector* p_stNormal;
		MTH3D_tdstVector stRotationAxis;

		MEC_fn_vComputeAdjustVector (&stVector, p_stDynamic, lObstacle, g_a_stObstacle, lObstacle);
		MTH3D_M_vAddVector (p_stCurrentTranslation,p_stEndTranslation,&stVector);

		p_stObstacle = NULL;
		for (lCounter=0; lCounter<lObstacle && !p_stObstacle; lCounter++)
		{
			p_stTmpObstacle = g_a_stObstacle + lCounter;
			if (MEC_fn_bIsObstacleValid(p_stTmpObstacle)) p_stObstacle = p_stTmpObstacle;
		}

		POS_fn_vGetRotationVector ( hCurrentMatrix, &p_stX, &p_stY, &p_stZ );
		p_stNormal = DNM_M_p_stObstacleGetNorm (p_stObstacle);

		MTH3D_M_vCrossProductVectorWithoutBuffer (&stRotationAxis, p_stY, p_stZ);
		MTH3D_M_vCopyVector ( p_stZ, p_stNormal );
		MTH3D_M_vCrossProductVectorWithoutBuffer (p_stY, p_stZ, &stRotationAxis);
		MTH3D_M_vCrossProductVectorWithoutBuffer (p_stX, p_stY, p_stZ);
		MEC_fn_vUpdateMatrix ( p_stDynamic, hCurrentMatrix );

		MEC_fn_vReportCollision (p_stDynamic,hSupObj,lObstacle,g_a_stObstacle,lObstacle-1);
		MEC_fn_vComputeSpeed (p_stDynamic,xDT);
		return;
	}

	MEC_fn_vComputeSpeed (p_stDynamic,xDT);
}
#pragma optimize("", on)

#pragma optimize("y", off)
void __cdecl MEC_fn_vOrient ( DNM_tdstDynamics *p_stDynamic, MTH3D_tdstVector *p_stNormal )
{
	POS_tdxHandleToPosition hCurrentMatrix;
	MTH3D_tdstVector stVerticalVector;
	MTH3D_tdstVector *p_stX, *p_stY, *p_stZ;
	MTH3D_tdstVector stOldZ, stDelta;

	hCurrentMatrix = DNM_M_p_stDynamicsGetCurrentMatrix (p_stDynamic);
	MTH3D_M_vSetBaseKVector (&stVerticalVector);

	POS_fn_vGetRotationVector ( hCurrentMatrix, &p_stX, &p_stY, &p_stZ );

	MTH3D_M_vMulScalarVector ( &stOldZ, C_ulDNM_RAYMAN_ZDM_RADIUS, p_stZ );

	MTH3D_M_vCopyVector ( p_stY, p_stNormal );
	MTH3D_M_vCrossProductVectorWithoutBuffer (p_stX, p_stY, &stVerticalVector);
	MTH3D_M_vCrossProductVectorWithoutBuffer (p_stZ, p_stX, p_stY );

	MTH3D_M_vMulScalarVector ( &stDelta, C_ulDNM_RAYMAN_ZDM_RADIUS, p_stZ );
	MTH3D_M_vSubVector ( &stDelta, &stOldZ, &stDelta );
	MTH3D_M_vAddVector ( POS_fn_p_stGetTranslationVector ( hCurrentMatrix ), POS_fn_p_stGetTranslationVector ( hCurrentMatrix ), &stDelta );

	MEC_fn_vUpdateMatrix ( p_stDynamic, hCurrentMatrix );
}
#pragma optimize("", on)

void __cdecl MEC_fn_vWallObstacleManagement
(
	MTH3D_tdstVector* pstResultMove,
	DNM_tdstDynamics *p_stDynamic,
	MTH3D_tdstVector* pstMove,
	MTH3D_tdstVector* pstGroundNormal,
	MTH3D_tdstVector* pstWallNormal,
	MTH_tdxReal xDT,
	ACP_tdxBool bAngleLimit
)
{
	MTH3D_tdstVector stDirection;
	MTH3D_tdstVector stWallNormalAdjust;
	MTH3D_tdstVector stVector;
	POS_tdxHandleToPosition hCurrentMatrix;
	MTH3D_tdstVector *p_stCurrentTranslation;
	MTH_tdxReal xDotProd1, xDotProd2, xFactor;

	(void)xDT;

	if ( MEC_fn_bIsNullVector(pstMove) )
	{
		MTH3D_M_vNullVector (pstResultMove);
		return;
	}

	hCurrentMatrix = DNM_M_p_stDynamicsGetCurrentMatrix (p_stDynamic);
	p_stCurrentTranslation = POS_fn_p_stGetTranslationVector ( hCurrentMatrix );
	MTH3D_M_vCrossProductVectorWithoutBuffer ( &stDirection, pstGroundNormal, pstWallNormal );

	if (MEC_fn_bIsNullVector(&stDirection))
	{
		MTH3D_M_vNullVector (pstResultMove);
		return;
	}

	MTH3D_M_vNormalizeVector (&stDirection,&stDirection);
	xDotProd1 = MTH3D_M_xDotProductVector (&stDirection, pstMove);

	if (bAngleLimit && (g_lCallCounter==1))
	{
		if (!MEC_fn_bIsNullVector(pstMove))
		{	MTH3D_M_vNormalizeVector (&stVector,pstMove);	}
		else
			MTH3D_M_vNullVector(&stVector);

		xDotProd2 = MTH_M_xNeg(MTH3D_M_xDotProductVector (&stVector,pstWallNormal));

		xFactor = MTH_M_xMul(MTH3D_M_xNormVector(pstMove),MTH_M_xSign(xDotProd1));
		if (MTH_M_bGreater(xDotProd2,C_xDNM_CLIMB_COS_ANGLE_LIMIT))
			/* 0x0049C794 */
			xFactor = MTH_M_xMul ( xFactor, MTH_M_xMul(MTH_M_xSub(MTH_C_ONE,xDotProd2),MTH_M_xConstFloatToReal(13.7324905f)) );
	}
	else
		xFactor = xDotProd1;

	MTH3D_M_vMulScalarVector ( pstResultMove, xFactor, &stDirection);

	MTH3D_M_vMulScalarVector ( &stWallNormalAdjust, MTH_M_xMul(MTH3D_M_xNormVector(pstResultMove),MTH_M_xConstFloatToReal(0.01f)), pstWallNormal );
	MTH3D_M_vAddVector ( pstResultMove, pstResultMove, &stWallNormalAdjust );

	MTH3D_M_vAddVector ( p_stCurrentTranslation, p_stCurrentTranslation, pstResultMove);
}

void __cdecl MEC_fn_vClimbObstacleManagement ( DNM_tdstDynamics *p_stDynamic, HIE_tdstSuperObject *hSupObj, MTH_tdxReal xDT )
{
	long lObstacle;
	long lCounter=0;
	long lCounter1=0;
	long lCounter2=0;

	ACP_tdxBool bStop = FALSE;

	MTH_tdxReal xNormalMove;
	MTH_tdxReal	xSlope;
	MTH_tdxReal	xDotProd;
	MTH_tdxReal	xDotProdMin = MTH_C_ZERO;

	MTH3D_tdstVector *p_stX, *p_stY, *p_stZ;
	MTH3D_tdstVector *p_stCurrentTranslation;
	MTH3D_tdstVector *p_stPreviousTranslation;
	MTH3D_tdstVector *p_stBackTranslation;
	MTH3D_tdstVector *p_stFrontTranslation;
	MTH3D_tdstVector *p_stNormal;
	MTH3D_tdstVector *p_stCollideSlideObstacle = NULL;

	MTH3D_tdstVector stVector;
	MTH3D_tdstVector stAdjustVector;
	MTH3D_tdstVector stMove, stNormalMove, stTangentMove, stSlideMove;
	MTH3D_tdstVector stVerticalVector;
	MTH3D_tdstVector stSafeTranslation;
	MTH3D_tdstVector stNormMove;
	MTH3D_tdstVector stNormTangentMove;

	DNM_tdstMecObstacle *p_stTmpObstacle;
	DNM_tdstMecObstacle *p_stClimbObstacle, *p_stSlideObstacle;
	DNM_tdstMecObstacle *p_stEdgeObstacle;
	DNM_tdstMecObstacle *p_stGroundObstacle;

	POS_tdstCompletePosition a2_stPosition[2];
	POS_tdstCompletePosition *p_stPOS_BackPosition = a2_stPosition;
	POS_tdstCompletePosition *p_stPOS_FrontPosition = a2_stPosition+1;
 	POS_tdxHandleToPosition hCurrentMatrix, hPreviousMatrix;

	xSlope = C_xDNM_CLIMB_SLOPE_LIMIT;

	hCurrentMatrix = DNM_M_p_stDynamicsGetCurrentMatrix (p_stDynamic);
	hPreviousMatrix = DNM_M_p_stDynamicsGetPreviousMatrix (p_stDynamic);
	MTH3D_M_vSetVectorElements( &stVerticalVector, MTH_C_ZERO, MTH_C_ZERO, MTH_C_ONE );

	if (DNM_M_bDynamicsIsMechanicChange	(p_stDynamic))
	{
		p_stNormal = DNM_M_pDynamicsGetWallNormal (p_stDynamic);
		if (!MTH3D_M_bIsNullVector(p_stNormal))
			MEC_fn_vOrient ( p_stDynamic, p_stNormal );
	}

	POS_fn_vCopyMatrix(p_stPOS_FrontPosition, hCurrentMatrix);
	POS_fn_vCopyMatrix(p_stPOS_BackPosition, hCurrentMatrix);

	p_stCurrentTranslation = POS_fn_p_stGetTranslationVector ( hCurrentMatrix );
	p_stPreviousTranslation = POS_fn_p_stGetTranslationVector ( hPreviousMatrix );
	MTH3D_M_vSubVector (&stMove,p_stCurrentTranslation,p_stPreviousTranslation);

	if (!MEC_fn_bIsNullVector(&stMove))
	{	MTH3D_M_vNormalizeVector(&stNormMove,&stMove);	}
	else
		MTH3D_M_vNullVector(&stNormMove);

	g_lCallCounter++;
	if (g_lCallCounter>5)
	{

		MTH3D_M_vCopyVector (p_stCurrentTranslation,p_stPreviousTranslation);
		MTH3D_M_vNullVector ( DNM_M_pDynamicsGetPreviousSpeed(p_stDynamic) );
		return;
	}

	POS_fn_vGetRotationVector ( hCurrentMatrix, &p_stX, &p_stY, &p_stZ );
	MTH3D_M_vMulScalarVector ( &stNormalMove, MTH3D_M_xDotProductVector( &stMove, DNM_M_pDynamicsGetWallNormal (p_stDynamic)), DNM_M_pDynamicsGetWallNormal (p_stDynamic));
	MTH3D_M_vSubVector ( &stTangentMove, &stMove, &stNormalMove );

	if (g_lCallCounter==1)
		MTH3D_M_vCopyVector ( &g_stFirstMove, &stTangentMove );

	if (!MEC_fn_bIsNullVector(&stTangentMove))
	{	MTH3D_M_vNormalizeVector(&stNormTangentMove,&stTangentMove);	}
	else
		MTH3D_M_vNullVector(&stNormTangentMove);

	if (DNM_M_bDynamicsIsMechanicChange	(p_stDynamic)) xNormalMove = C_xDNM_CLIMB_FIRST_NORMAL_MOVE;
	else xNormalMove = MTH_M_xMul ( xSlope, MTH3D_M_xNormVector(&stTangentMove) );

	if (MTH_M_bLess (xNormalMove,C_xDNM_CLIMB_DEFAULT_NORMAL_MOVE)) xNormalMove = C_xDNM_CLIMB_DEFAULT_NORMAL_MOVE;

	p_stBackTranslation = POS_fn_p_stGetTranslationVector ( p_stPOS_BackPosition );
	MTH3D_M_vAddVector ( p_stBackTranslation, p_stPreviousTranslation, &stTangentMove );
	MTH3D_M_vMulScalarVector (&stVector, xNormalMove, DNM_M_pDynamicsGetWallNormal (p_stDynamic));
	MTH3D_M_vAddVector ( p_stBackTranslation, p_stBackTranslation, &stVector );

	p_stFrontTranslation = POS_fn_p_stGetTranslationVector ( p_stPOS_FrontPosition );
	MTH3D_M_vAddVector ( p_stFrontTranslation, p_stPreviousTranslation, &stTangentMove );
	MTH3D_M_vSubVector ( p_stFrontTranslation, p_stFrontTranslation, &stVector );

	p_stClimbObstacle = NULL;
	p_stSlideObstacle = NULL;
	p_stEdgeObstacle = NULL;
	p_stGroundObstacle = NULL;

	if(MEC_fn_bCollision(&lObstacle,g_a_stObstacle,p_stDynamic,hSupObj,p_stPOS_BackPosition,p_stPOS_FrontPosition,TRUE))
	{

		MEC_fn_vDeleteClimbBackFace (lObstacle,g_a_stObstacle,p_stDynamic,&stNormTangentMove);

		for (lCounter=0; lCounter<lObstacle && !p_stClimbObstacle; lCounter++)
		{
			p_stTmpObstacle = g_a_stObstacle + lCounter;
			if (MEC_fn_bIsObstacleValid(p_stTmpObstacle))
			{
				if ( (MEC_fn_bTypeOfGameMaterial(DNM_M_hObstacleGetCollidedMaterial(p_stTmpObstacle), C_xDNM_CLIMB_TYPE_GAME_MATERIAL)) && !(MEC_fn_bTypeOfGameMaterial(DNM_M_hObstacleGetCollidedMaterial(p_stTmpObstacle), C_xDNM_NO_COLLISION_TYPE_GAME_MATERIAL)) && (MEC_fn_bWallObstacle (p_stDynamic,p_stTmpObstacle)) )
					p_stClimbObstacle = p_stTmpObstacle;
			}
		}

		for (lCounter=0; lCounter<lObstacle && !p_stGroundObstacle; lCounter++)
		{
			p_stTmpObstacle = g_a_stObstacle + lCounter;
			if (MEC_fn_bIsObstacleValid(p_stTmpObstacle))
			{
				if (MEC_fn_bGroundObstacle(p_stDynamic,p_stTmpObstacle))
				{
					p_stGroundObstacle = p_stTmpObstacle;
					MEC_fn_vReportCollision (p_stDynamic,hSupObj,1,p_stGroundObstacle,0);
				}
			}
		}

		if (p_stClimbObstacle)
		{
			bStop = FALSE;
			for ( lCounter1=0; lCounter1<lObstacle && !bStop; lCounter1++)
			{
				p_stTmpObstacle = g_a_stObstacle + lCounter1;
				if (MEC_fn_bIsObstacleValid(p_stTmpObstacle))
				{
					xDotProd = MTH3D_M_xDotProductVector ( DNM_M_p_stObstacleGetNorm(p_stTmpObstacle), DNM_M_p_stObstacleGetNorm(p_stClimbObstacle) );
					if (MTH_M_bLess(xDotProd,MTH_M_xConstFloatToReal(0.707f)))
					{
						MTH3D_M_vSubVector ( &stVector, DNM_M_p_stObstacleGetContact(p_stTmpObstacle), DNM_M_p_stObstacleGetContact(p_stClimbObstacle) );
						xDotProd = MTH3D_M_xDotProductVector ( &stVector, DNM_M_p_stObstacleGetNorm(p_stClimbObstacle) );
						if (MTH_M_bGreater ( xDotProd, MTH_M_xConstFloatToReal(0.1f) ))
						{
							xDotProd = MTH3D_M_xDotProductVector ( &stNormTangentMove, DNM_M_p_stObstacleGetNorm(p_stTmpObstacle));

							if
							(
								MEC_fn_bTypeOfGameMaterial ( DNM_M_hObstacleGetCollidedMaterial(p_stTmpObstacle), C_xDNM_CLIMB_TYPE_GAME_MATERIAL )
							&&	!MEC_fn_bTypeOfGameMaterial(DNM_M_hObstacleGetCollidedMaterial(p_stTmpObstacle), C_xDNM_NO_COLLISION_TYPE_GAME_MATERIAL)
							)
							{
								p_stSlideObstacle=NULL;
								bStop = TRUE;

							}
							else if (MTH_M_bLess(xDotProd,xDotProdMin))
							{
								xDotProdMin = xDotProd;
								p_stSlideObstacle = p_stTmpObstacle;
							}
						}
					}
				}
			}
		}

		if
		(
			p_stSlideObstacle
		&&	MEC_fn_bTypeOfGameMaterial ( DNM_M_hObstacleGetCollidedMaterial(p_stSlideObstacle), C_xDNM_CLIMB_TYPE_GAME_MATERIAL )
		&&	!MEC_fn_bTypeOfGameMaterial(DNM_M_hObstacleGetCollidedMaterial(p_stSlideObstacle), C_xDNM_NO_COLLISION_TYPE_GAME_MATERIAL)
		&&  MEC_fn_bWallObstacle (p_stDynamic,p_stSlideObstacle)
		)
		{
			xDotProd = MTH3D_M_xDotProductVector ( DNM_M_p_stObstacleGetNorm(p_stSlideObstacle), DNM_M_p_stObstacleGetNorm(p_stClimbObstacle) );
			if (!MTH_M_bEqual(xDotProd,MTH_C_ZERO))
				p_stSlideObstacle = NULL;
		}

		if
		(
			DNM_M_bDynamicsIsCollisionReport(p_stDynamic)
		&& ( (DNM_M_ulReportGetPrevSurfaceState(DNM_M_p_stDynamicsGetReport(p_stDynamic)) & C_WOT_ulGround) != C_WOT_ulGround )
		&& !DNM_M_bDynamicsIsMechanicChange(p_stDynamic)
		&& MTH_M_bLessZero (MTH3D_M_xGetZofVector(&stTangentMove))
		)
		{
			for (lCounter2=0; lCounter2<lObstacle; lCounter2++)
			{
				p_stTmpObstacle = g_a_stObstacle + lCounter2;
				if (MEC_fn_bIsObstacleValid(p_stTmpObstacle))
				{
					if
					(
						!MEC_fn_bTypeOfGameMaterial (DNM_M_hObstacleGetCollidedMaterial(p_stTmpObstacle),C_xDNM_NO_COLLISION_TYPE_GAME_MATERIAL)
						&&	MEC_fn_bGroundObstacle(p_stDynamic,p_stTmpObstacle)
					)
					{
						MTH3D_M_vCopyVector (p_stCurrentTranslation,p_stPreviousTranslation);
						MTH3D_M_vNullVector ( DNM_M_pDynamicsGetPreviousSpeed(p_stDynamic) );
						DNM_M_vDynamicsSetStop (p_stDynamic,TRUE);
						MEC_fn_vReportCollision (p_stDynamic,hSupObj,lObstacle,g_a_stObstacle,lCounter1-1);
						return;
					}
				}
			}
		}

		if (p_stSlideObstacle && lCounter1>lCounter) lCounter=lCounter1;

		MEC_fn_vReportCollision (p_stDynamic,hSupObj,lObstacle,g_a_stObstacle,lCounter-1);

		if (p_stClimbObstacle)
		{
			DNM_M_vDynamicsSetWallNormal ( p_stDynamic, DNM_M_p_stObstacleGetNorm(p_stClimbObstacle) );

			if(!p_stSlideObstacle)
			{
				if (!MEC_fn_bIsEdge(p_stClimbObstacle))
				{
					MTH3D_M_vAddVector (&stVector, p_stFrontTranslation, DNM_M_xObstacleGetTranslation (p_stClimbObstacle));
					POS_fn_vSetTranslationVector ( hCurrentMatrix, &stVector );
				}
				else
				{

					for (lCounter=0; lCounter<lObstacle && !p_stSlideObstacle; lCounter++)
					{
						p_stTmpObstacle = g_a_stObstacle + lCounter;
						if (MEC_fn_bIsObstacleValid(p_stTmpObstacle))
						{
							if ( MEC_fn_bWallObstacle ( p_stDynamic, p_stTmpObstacle ) && !MEC_fn_bIsEdge( p_stTmpObstacle ) )
								p_stSlideObstacle = p_stTmpObstacle;
						}
					}

					if (p_stSlideObstacle)
					{
						MTH3D_M_vAddVector (&stVector, p_stFrontTranslation, DNM_M_xObstacleGetTranslation (p_stSlideObstacle));
						POS_fn_vSetTranslationVector ( hCurrentMatrix, &stVector );
						p_stSlideObstacle = NULL;
					}
					else
					{
						MTH3D_M_vAddVector (&stVector, p_stFrontTranslation, DNM_M_xObstacleGetTranslation (p_stClimbObstacle));
						POS_fn_vSetTranslationVector ( hCurrentMatrix, &stVector );
					}
				}
			}
			else
			{
				if ( p_stSlideObstacle && MEC_fn_bIsCollide(p_stSlideObstacle) && ( (g_lCallCounter>2) || DNM_M_bDynamicsIsMechanicChange(p_stDynamic) ) )
				{
					MEC_fn_vComputeAdjustVector (&stAdjustVector, p_stDynamic, lObstacle, g_a_stObstacle, lCounter-1);

					xDotProd = MTH3D_M_xDotProductVector (&stAdjustVector,DNM_M_p_stObstacleGetNorm(p_stClimbObstacle));
					MTH3D_M_vMulScalarVector (&stVector,xDotProd,DNM_M_p_stObstacleGetNorm(p_stClimbObstacle));
					MTH3D_M_vSubVector (&stAdjustVector,&stAdjustVector,&stVector);
					MTH3D_M_vAddVector (&stAdjustVector,&stAdjustVector,DNM_M_xObstacleGetTranslation (p_stClimbObstacle));

					MTH3D_M_vAddVector (&stSafeTranslation, p_stFrontTranslation, &stAdjustVector);
					MTH3D_M_vSubVector (&stVector, &stSafeTranslation, p_stPreviousTranslation);
					MTH3D_M_vSubVector (&stMove,&stMove,&stVector);

				}
				else

					MTH3D_M_vCopyVector (&stSafeTranslation,p_stPreviousTranslation);

				MTH3D_M_vCopyVector (p_stCurrentTranslation,&stSafeTranslation);

				MEC_fn_vWallObstacleManagement ( &stSlideMove, p_stDynamic, &stMove, DNM_M_p_stObstacleGetNorm(p_stClimbObstacle), DNM_M_p_stObstacleGetNorm(p_stSlideObstacle), xDT, TRUE );

				xDotProd = MTH3D_M_xDotProductVector (&g_stFirstMove,&stSlideMove);

				if (!MEC_fn_bIsNullVector(&stSlideMove)&&MTH_M_bGreaterEqualZero(xDotProd))
				{
					MEC_fn_vClimbObstacleManagement (p_stDynamic, hSupObj, xDT);
					MEC_fn_vComputeSpeed (p_stDynamic,xDT);
					return;
				}
				else
					MTH3D_M_vCopyVector (p_stCurrentTranslation,&stSafeTranslation);
			}
		}
		else
		{
			if (p_stSlideObstacle)
			{
				{
					MEC_fn_vComputeAdjustVector (&stAdjustVector, p_stDynamic, lObstacle, g_a_stObstacle, lCounter);

					MTH3D_M_vAddVector (&stSafeTranslation, p_stFrontTranslation, &stAdjustVector);
					MTH3D_M_vSubVector (&stVector, &stSafeTranslation, p_stPreviousTranslation);
					MTH3D_M_vSubVector (&stMove,&stMove,&stVector);

				}

				MTH3D_M_vCopyVector (p_stCurrentTranslation,&stSafeTranslation);

				MEC_fn_vWallObstacleManagement ( &stSlideMove, p_stDynamic, &stMove, p_stY, DNM_M_p_stObstacleGetNorm(p_stSlideObstacle), xDT, TRUE );

				xDotProd = MTH3D_M_xDotProductVector (&g_stFirstMove,&stSlideMove);
				if (MTH_M_bLessZero(xDotProd))
				{

					MTH3D_M_vCopyVector (p_stCurrentTranslation,p_stPreviousTranslation);
					MTH3D_M_vNullVector ( DNM_M_pDynamicsGetPreviousSpeed(p_stDynamic) );
					return;
				}

				if (!MEC_fn_bIsNullVector(&stSlideMove))
				{
					MEC_fn_vClimbObstacleManagement ( p_stDynamic, hSupObj, xDT );
					MEC_fn_vComputeSpeed (p_stDynamic,xDT);
					return;
				}
				else
					MTH3D_M_vCopyVector (p_stCurrentTranslation,&stSafeTranslation);
			}
			else
			{
				POS_fn_vCopyMatrix ( hCurrentMatrix, hPreviousMatrix );
				MTH3D_M_vNullVector ( DNM_M_pDynamicsGetPreviousSpeed(p_stDynamic) );
				DNM_M_vDynamicsSetStop ( p_stDynamic, TRUE );

				if (DNM_M_bDynamicsIsCollisionReport(p_stDynamic))
					DNM_M_ulReportSetSurfaceState(DNM_M_p_stDynamicsGetReport(p_stDynamic),DNM_M_ulReportGetPrevSurfaceState (DNM_M_p_stDynamicsGetReport(p_stDynamic)) );
				return;
			}
		}

		if ( p_stClimbObstacle && !MEC_fn_bIsEdge(p_stClimbObstacle) )
		{
			p_stNormal = DNM_M_p_stObstacleGetNorm (p_stClimbObstacle);
			MTH3D_M_vSubVector (&stVector,p_stNormal,p_stY);
			if (!MEC_fn_bIsNullVector(&stVector))
				MEC_fn_vOrient ( p_stDynamic, p_stNormal );
		}

		MEC_fn_vComputeSpeed (p_stDynamic,xDT);
		return;
	}

	POS_fn_vCopyMatrix ( hCurrentMatrix, hPreviousMatrix );
	MTH3D_M_vNullVector ( DNM_M_pDynamicsGetPreviousSpeed(p_stDynamic) );
	DNM_M_vDynamicsSetStop (p_stDynamic,TRUE);

	if (DNM_M_bDynamicsIsCollisionReport(p_stDynamic))
		DNM_M_ulReportSetSurfaceState(DNM_M_p_stDynamicsGetReport(p_stDynamic),DNM_M_ulReportGetPrevSurfaceState (DNM_M_p_stDynamicsGetReport(p_stDynamic)) );

	return;
}

void __cdecl MEC_fn_vGroundGuess (DNM_tdstDynamics *p_stDynamic, MTH_tdxReal xDT)
{
	POS_tdxHandleToPosition hCurrentMatrix, hPreviousMatrix;
	MTH3D_tdstVector* p_stNormal;
	MTH3D_tdstVector *p_stCurrentTranslation, *p_stPreviousTranslation;
	MTH3D_tdstVector stMove, stVector;
	MTH_tdxReal xDotProd;

	p_stNormal = DNM_M_pDynamicsGetGroundNormal (p_stDynamic);
	if (MTH3D_M_bIsNullVector(p_stNormal)) return;

	hCurrentMatrix = DNM_M_p_stDynamicsGetCurrentMatrix (p_stDynamic);
	hPreviousMatrix = DNM_M_p_stDynamicsGetPreviousMatrix (p_stDynamic);

	p_stCurrentTranslation = POS_fn_p_stGetTranslationVector ( hCurrentMatrix );
	p_stPreviousTranslation = POS_fn_p_stGetTranslationVector ( hPreviousMatrix );
	MTH3D_M_vSubVector (&stMove,p_stCurrentTranslation,p_stPreviousTranslation);

	xDotProd = MTH3D_M_xDotProductVector (&stMove,p_stNormal);
	MTH3D_M_vNormalizeVector (&stVector,p_stNormal);
	MTH3D_M_vMulScalarVector (&stVector,xDotProd,&stVector);
	MTH3D_M_vSubVector (&stMove,&stMove,&stVector);

	MTH3D_M_vAddVector (p_stCurrentTranslation,p_stPreviousTranslation,&stMove);
	MEC_fn_vComputeSpeed (p_stDynamic,xDT);
}

#include "DNM/DNMObsta.h"
#include "DNM/DNMRepor.h"

void __cdecl MEC_fn_vFillCollisionReport (DNM_tdstDynamics *p_stDynamic)
{
	DNM_tdstReport *p_stReport;

	if (!DNM_M_bDynamicsIsCollisionReport(p_stDynamic)) return;
	if (DNM_M_ulReportGetSurfaceState(&g_stReport)==0) return;

	p_stReport = DNM_M_p_stDynamicsGetReport (p_stDynamic);

	DNM_M_ReportSetObstacle ( p_stReport, DNM_M_p_stReportGetObstacle(&g_stReport) );

	if ( DNM_M_ulReportGetSurfaceState(&g_stReport) & C_WOT_ulMobile )
	{
		DNM_M_ulReportAddSurfaceState (p_stReport,C_WOT_ulMobile);
		DNM_M_ReportSetCharacter ( p_stReport, DNM_M_p_stReportGetCharacter(&g_stReport) );
	}

	if ( DNM_M_ulReportGetSurfaceState(&g_stReport) & C_WOT_ulWater )
	{
		DNM_M_ulReportAddSurfaceState (p_stReport,C_WOT_ulWater);
		DNM_M_ReportSetWater ( p_stReport, DNM_M_p_stReportGetWater(&g_stReport) );
	}

	if ( DNM_M_ulReportGetSurfaceState(&g_stReport) & C_WOT_ulGround )
	{
		DNM_M_ulReportAddSurfaceState (p_stReport,C_WOT_ulGround);
		DNM_M_ReportSetGround ( p_stReport, DNM_M_p_stReportGetGround(&g_stReport) );
	}

	if ( DNM_M_ulReportGetSurfaceState(&g_stReport) & C_WOT_ulWall )
	{
		DNM_M_ulReportAddSurfaceState (p_stReport,C_WOT_ulWall);
		DNM_M_ReportSetWall ( p_stReport, DNM_M_p_stReportGetWall(&g_stReport) );
	}

	if ( DNM_M_ulReportGetSurfaceState(&g_stReport) & C_WOT_ulCeiling )
	{
		DNM_M_ulReportAddSurfaceState (p_stReport,C_WOT_ulCeiling);
		DNM_M_ReportSetCeil ( p_stReport, DNM_M_p_stReportGetCeil(&g_stReport) );
	}

}
