#include <stddef.h>
#include <string.h>
#include "ACP/ACP_types.h"
#include "DNM/DNM_types.h"
#include "MTH/MTH_flt.h"
#include "MTH/MTH3D.h"
#include "POS/POS_types.h"
#include "GAM/GAM_types.h"
#include "AI/AI_types.h"
#include "DNM/DNM.h"
#include "MEC/MECTools.h"
#include "MEC/MECCol.h"

#ifndef MTH_M_xConstFloatToReal
#define MTH_M_xConstFloatToReal(_x)  ((MTH_tdxReal)(_x))
#endif

#ifndef C_xDNM_HANG_TYPE_GAME_MATERIAL
#define C_xDNM_HANG_TYPE_GAME_MATERIAL   (1 << 2)
#endif
#ifndef C_xDNM_HAND_TYPE_GAME_MATERIAL
#define C_xDNM_HAND_TYPE_GAME_MATERIAL   (1 << 3)
#endif
#ifndef C_xDNM_GI_TYPE_GAME_MATERIAL
#define C_xDNM_GI_TYPE_GAME_MATERIAL     (1 << 5)
#endif
#ifndef C_xDNM_CLIMB_TYPE_GAME_MATERIAL
#define C_xDNM_CLIMB_TYPE_GAME_MATERIAL  (1 << 6)
#endif

MTH_tdxReal g_xZWater = MTH_C_ZERO;

extern long __cdecl COL_lHaveITakeSThgIntoTheMug (DNM_tdstMecObstacle *_a_stResultObstacle,
                                                  HIE_tdstSuperObject *_p_stSupObj,
                                                  POS_tdstCompletePosition *_p_stStartPosition,
                                                  POS_tdstCompletePosition *_p_stEndPosition);

static __inline ACP_tdxBool fn_bBrainGetWarnMechanicsFlag(MS_tdxHandleToBrain h_Brain)
{
	return h_Brain->bWarnMechanicsFlag;
}

#define fn_bBrainWaterCollideSO(hSupObj)	fn_bBrainGetWarnMechanicsFlag(M_GetMSHandle(hSupObj,Brain));

#define C_WOT_ulGround        0x00000001UL
#define C_WOT_ulWall          0x00000004UL
#define C_WOT_ulCeiling       0x00000010UL
#define C_WOT_ulWater         0x00000040UL
#define C_WOT_ulForceMobile   0x00000080UL
#define C_WOT_ulMobile        0x00010000UL

#ifndef MTH_C_2
#define MTH_C_2  ((MTH_tdxReal)2.0)
#endif

#pragma optimize("y", off)
ACP_tdxBool __cdecl MEC_fn_bCollision ( long* p_lObstacle, DNM_tdstMecObstacle *p_stObstacle, DNM_tdstDynamics *p_stDynamic, HIE_tdstSuperObject *p_stSupObj, POS_tdstCompletePosition *p_stStartPosition, POS_tdstCompletePosition *p_stEndPosition, ACP_tdxBool bBackFace )
{
	long lCounter;
	DNM_tdstMecObstacle *p_stTmpObstacle;
	DNM_tdstMecObstacle stHandObstacle;
	POS_tdxHandleToPosition hSupObjMatrix;
	MTH3D_tdstVector stMove, stVector;
	MTH3D_tdstVector *p_stStartTranslation, *p_stEndTranslation;
	MTH3D_tdstVector *p_stX, *p_stY, *p_stZ;
	ACP_tdxBool bObstacleOk = FALSE;
	MTH_tdxReal xDotProd;
	ACP_tdxBool bWaterFace = FALSE;
	MTH_tdxReal xDotProdMax;

	*p_lObstacle = 0;
	xDotProdMax = MTH_C_MinusONE;

	*p_lObstacle = COL_lHaveITakeSThgIntoTheMug (p_stObstacle, p_stSupObj, p_stStartPosition, p_stEndPosition);

	if (!bBackFace)
		MEC_fn_vDeleteBaseBackFace ( *p_lObstacle, p_stObstacle, p_stDynamic );

	MEC_fn_vDeleteNoCollidedGroundFace (*p_lObstacle, p_stObstacle, p_stDynamic);

	if (*p_lObstacle <= 0)
		return FALSE;

	if (DNM_M_bDynamicsIsHanging(p_stDynamic))
		MEC_fn_vDeleteHangingObstacle (*p_lObstacle, p_stObstacle, p_stDynamic);

	p_stStartTranslation = POS_fn_p_stGetTranslationVector (p_stStartPosition);
	p_stEndTranslation = POS_fn_p_stGetTranslationVector (p_stEndPosition);
	MTH3D_M_vSubVector ( &stMove, p_stStartTranslation, p_stEndTranslation );

	MEC_fn_vComputeCollisionTranslation (*p_lObstacle, p_stObstacle, &stMove);

	hSupObjMatrix = HIE_fn_hGetSuperObjectGlobalMatrix(p_stSupObj);
	POS_fn_vGetRotationVector ( hSupObjMatrix, &p_stX, &p_stY, &p_stZ );

	for (lCounter = 0 ; lCounter < *p_lObstacle ; lCounter++)
	{
		p_stTmpObstacle = p_stObstacle+lCounter;

		if (MEC_fn_bIsObstacleValid(p_stTmpObstacle))
		{
			ACP_tdxBool bCollisionOk = TRUE;

			if (MEC_fn_bTypeOfGameMaterial (DNM_M_hObstacleGetMyMaterial(p_stTmpObstacle),C_xDNM_HAND_TYPE_GAME_MATERIAL))
			{
				bCollisionOk = FALSE;
				if (MEC_fn_bTypeOfGameMaterial (DNM_M_hObstacleGetCollidedMaterial(p_stTmpObstacle),C_xDNM_HANG_TYPE_GAME_MATERIAL) )
				{

					if (MEC_fn_bGroundObstacle(p_stDynamic,p_stTmpObstacle))
					{

						MTH3D_M_vSubVector (&stVector,DNM_M_p_stObstacleGetContact(p_stTmpObstacle),POS_fn_p_stGetTranslationVector(hSupObjMatrix));
						if (!MEC_fn_bIsNullVector(&stVector))
							MTH3D_M_vNormalizeVector (&stVector,&stVector);

						xDotProd = MTH_M_xAbs(MTH3D_M_xDotProductVector (p_stY,&stVector));
						if (xDotProd>xDotProdMax)
						{
							xDotProdMax = xDotProd;
							DNM_M_MecObstacleCopyClone ( &stHandObstacle, p_stTmpObstacle );
						}
					}
				}
			}

			else if (MEC_fn_bTypeOfGameMaterial (DNM_M_hObstacleGetCollidedMaterial(p_stTmpObstacle),C_xDNM_WATER_TYPE_GAME_MATERIAL))
			{
				g_xZWater = MTH3D_M_xGetZofVector(DNM_M_p_stObstacleGetContact(p_stTmpObstacle));

				if (MEC_fn_bTypeOfGameMaterial (DNM_M_hObstacleGetMyMaterial(p_stTmpObstacle),C_xDNM_WATER_TYPE_GAME_MATERIAL))
				{

					if (!bWaterFace)
					{

						xDotProd = MTH3D_M_xDotProductVector ( DNM_M_p_stObstacleGetNorm(p_stTmpObstacle),DNM_M_pObstacleGetZoneMove(p_stTmpObstacle) );

						if (  MTH_M_bGreater ( MTH_M_xAbs ( MTH3D_M_xGetZofVector (DNM_M_pObstacleGetZoneMove(p_stTmpObstacle))), 0.03 /* 0x49C808 */ ))
						{
							if ( MTH_M_bLess(xDotProd, MTH_C_ZERO) )
							{
								MEC_fn_vReportCollision (p_stDynamic, p_stSupObj, 1, p_stTmpObstacle, 0);

								bCollisionOk = fn_bBrainWaterCollideSO(p_stSupObj);
								bWaterFace = TRUE;
							}
							else
								bCollisionOk = FALSE;
						}
						else
						{
							MTH3D_M_vSubVector ( &stVector, DNM_M_p_stObstacleGetContact(p_stTmpObstacle), DNM_M_pObstacleGetZonePosition(p_stTmpObstacle) );
							MTH3D_M_vAddVector ( &stVector, &stVector, DNM_M_pObstacleGetZoneMove(p_stTmpObstacle) );

							xDotProd = MTH3D_M_xDotProductVector ( DNM_M_p_stObstacleGetNorm(p_stTmpObstacle),&stVector );

							if (MTH_M_bLessEqual(xDotProd,MTH_C_ZERO))
							{
								MEC_fn_vReportCollision (p_stDynamic, p_stSupObj, 1, p_stTmpObstacle, 0);
								bCollisionOk = fn_bBrainWaterCollideSO(p_stSupObj);
								bWaterFace = TRUE;
							}
							else
								bCollisionOk = FALSE;
						}

						if (MTH_M_bGreater(MTH3D_M_xGetZofVector(DNM_M_p_stObstacleGetNorm(p_stTmpObstacle)),MTH_C_ZERO))
							bCollisionOk = FALSE;

					}
					else
					{
						bCollisionOk = FALSE;
					}

				}
				else
					bCollisionOk = FALSE;

			}

			else
			{

				if (MEC_fn_bTypeOfGameMaterial (DNM_M_hObstacleGetCollidedMaterial(p_stTmpObstacle),C_xDNM_NO_COLLISION_TYPE_GAME_MATERIAL))
				{

					if (MEC_fn_bTypeOfGameMaterial (DNM_M_hObstacleGetCollidedMaterial(p_stTmpObstacle),C_xDNM_CLIMB_TYPE_GAME_MATERIAL))
						bCollisionOk = (ACP_tdxBool)DNM_M_bDynamicsIsClimb(p_stDynamic);

					else if (MEC_fn_bTypeOfGameMaterial (DNM_M_hObstacleGetCollidedMaterial(p_stTmpObstacle),C_xDNM_GI_TYPE_GAME_MATERIAL))
						bCollisionOk = (ACP_tdxBool)DNM_M_bDynamicsIsGi(p_stDynamic);

					else if (MEC_fn_bTestMaskGameMaterial(DNM_M_hObstacleGetCollidedMaterial(p_stTmpObstacle),C_xDNM_COLLISION_REPORT_MASK))
					{
						bCollisionOk=FALSE;
						MEC_fn_vReportCollision (p_stDynamic, p_stSupObj, 1, p_stTmpObstacle, 0);
					}

					else  bCollisionOk=FALSE;

				}
			}

			if (!bCollisionOk)
				MEC_fn_vInvalidateObstacle(p_stTmpObstacle);
			else
				bObstacleOk = TRUE;
		}
	}

	if (xDotProdMax != MTH_C_MinusONE)
		MEC_fn_vReportCollision (p_stDynamic, p_stSupObj, 1, &stHandObstacle, 0);

	return bObstacleOk;
}
#pragma optimize("", on)

void __cdecl MEC_fn_vCrashAnalysis (DNM_tdstDynamics *p_stDynamic, long lObstacle, DNM_tdstMecObstacle *p_stObstacle)
{
	long lCounter;
	DNM_tdstMecObstacle *p_stTmpObstacle;
	DNM_tdstMecObstacle *p_stGroundObstacle, *p_stCeilObstacle;

	p_stGroundObstacle = NULL;
	p_stCeilObstacle = NULL;
	DNM_M_vDynamicsSetCrash(p_stDynamic, FALSE);

	p_stTmpObstacle = p_stObstacle;
	for (lCounter= 0 ; lCounter < lObstacle ; lCounter++)
	{
		if ( MEC_fn_bIsObstacleValid(p_stTmpObstacle) )
		{
			if (!p_stGroundObstacle && MEC_fn_bGroundObstacle(p_stDynamic,p_stTmpObstacle))
				p_stGroundObstacle = p_stTmpObstacle;

			if (!p_stCeilObstacle && MEC_fn_bCeilObstacle(p_stDynamic,p_stTmpObstacle))
				p_stCeilObstacle = p_stTmpObstacle;
		}
		p_stTmpObstacle++;
	}

	if ((p_stCeilObstacle == NULL) || (p_stGroundObstacle == NULL))
		return;

	if (((DNM_M_eObstacleGetType(p_stGroundObstacle) & DNM_ObsType_Mobile) && MEC_fn_bCanCrashActor(p_stGroundObstacle))
		||
		((DNM_M_eObstacleGetType(p_stCeilObstacle) & DNM_ObsType_Mobile) && MEC_fn_bCanCrashActor(p_stCeilObstacle)))
			DNM_M_vDynamicsSetCrash(p_stDynamic,TRUE);
}

void __cdecl MEC_fn_vReportCollideActor (HIE_tdstSuperObject *hCollideSupObj, DNM_tdstMecObstacle *p_stCollideObstacle)
{
	DNM_tdstDynamics* p_stDynamic;
	DNM_tdstObstacle* p_stObstacle;
	MTH3D_tdstVector stVector;
	HIE_tdstSuperObject *hSupObj;

	hSupObj = (HIE_tdstSuperObject *)DNM_M_p_stObstacleGetObject (p_stCollideObstacle);

	if (hSupObj == NULL) return;
	hSupObj = MEC_fn_hGetFatherActor (hSupObj);

	p_stDynamic= fn_p_stDynamGetDynamicsSO (hSupObj);
	if (!p_stDynamic) return;

	if (!DNM_M_bDynamicsIsCollisionReport(p_stDynamic)) return;

	DNM_M_ulReportAddSurfaceState (DNM_M_p_stDynamicsGetReport(p_stDynamic),C_WOT_ulForceMobile);

	p_stObstacle = DNM_M_p_stReportGetCharacter ( DNM_M_p_stDynamicsGetReport(p_stDynamic) );
	DNM_M_xObstacleSetRate (p_stObstacle,0);
	MTH3D_M_vNegVector (&stVector,DNM_M_p_stObstacleGetNorm(p_stCollideObstacle));
	DNM_M_ObstacleSetNorm (p_stObstacle,&stVector);
	DNM_M_ObstacleSetContact (p_stObstacle,DNM_M_p_stObstacleGetContact(p_stCollideObstacle));
	DNM_M_p_stObstacleSetObject (p_stObstacle,hCollideSupObj);
	DNM_M_vObstacleSetCollidedMaterial (p_stObstacle,DNM_M_hObstacleGetMyMaterial(p_stCollideObstacle));
	DNM_M_vObstacleSetMyMaterial (p_stObstacle,DNM_M_hObstacleGetCollidedMaterial(p_stCollideObstacle));
}

void __cdecl MEC_fn_vReportCollision (DNM_tdstDynamics *p_stDynamic, HIE_tdstSuperObject *hSupObj,
                                      long lNbObstacle, DNM_tdstMecObstacle *a_stObstacle, long lObstacle)
{
	MTH_tdxReal xDotProduct;
	MTH_tdxReal xPreviousRate, xCurrentRate;
	MTH3D_tdstVector stVerticalVector;
	MTH3D_tdstVector* p_stNormalVector;
	DNM_tdstMecObstacle* p_stTmpObstacle = NULL;
	long lCounter;
	ACP_tdxBool bStop = FALSE;
	ACP_tdxBool bHurt = FALSE;

	ACP_tdxBool bObstacle = FALSE;
	ACP_tdxBool bCeil = FALSE;
	ACP_tdxBool bWall = FALSE;
	ACP_tdxBool bGround = FALSE;
	ACP_tdxBool bActor = FALSE;
	ACP_tdxBool bWater = FALSE;

	ACP_tdxBool bObstacleEdge;
	ACP_tdxBool bCeilEdge;
	ACP_tdxBool bWallEdge;
	ACP_tdxBool bGroundEdge;
	ACP_tdxBool bActorEdge;
	ACP_tdxBool bWaterEdge;

	ACP_tdxBool bEdge;

	MTH3D_M_vSetVectorElements (&stVerticalVector, MTH_C_ZERO, MTH_C_ZERO, MTH_C_ONE);
	xPreviousRate = MTH_C_2;

	for (lCounter = 0 ; lCounter < lNbObstacle && !bStop; lCounter++)
	{
		p_stTmpObstacle = a_stObstacle + lCounter;

		if (MEC_fn_bIsObstacleValid(p_stTmpObstacle))
		{
			xCurrentRate = DNM_M_xObstacleGetRate (p_stTmpObstacle);

			if ( lCounter <= lObstacle || MTH_M_bEqual(xPreviousRate,xCurrentRate) )
			{
				p_stNormalVector = DNM_M_p_stObstacleGetNorm (p_stTmpObstacle);
				bEdge = MEC_fn_bIsEdge (p_stTmpObstacle);
				xPreviousRate = xCurrentRate;

				if (!bObstacle)
				{
					DNM_M_MecReportSetObstacle (&g_stReport, p_stTmpObstacle);
					bObstacleEdge = bEdge;
					bObstacle = TRUE;
				}
				else if (bObstacleEdge && !bEdge)
				{
					DNM_M_MecReportSetObstacle (&g_stReport, p_stTmpObstacle);
					bObstacleEdge = FALSE;
				}

				if (
					((DNM_M_eObstacleGetType(p_stTmpObstacle) & DNM_ObsType_Mobile) == DNM_ObsType_Mobile )
				||	((DNM_M_eObstacleGetType(p_stTmpObstacle) & DNM_ObsType_MobileWall) == DNM_ObsType_MobileWall ))
				{
					if (!bActor)
					{
						DNM_M_MecReportSetCharacter (&g_stReport,p_stTmpObstacle);
						bActorEdge = bEdge;
						bActor = TRUE;
					}
					else if (bActorEdge && !bEdge)
					{
						DNM_M_MecReportSetCharacter (&g_stReport,p_stTmpObstacle);
						bActorEdge = FALSE;
					}
				}

				if ((MEC_fn_bTypeOfGameMaterial (DNM_M_hObstacleGetCollidedMaterial(p_stTmpObstacle),C_xDNM_WATER_TYPE_GAME_MATERIAL)
						&& MEC_fn_bTypeOfGameMaterial (DNM_M_hObstacleGetMyMaterial(p_stTmpObstacle),C_xDNM_WATER_TYPE_GAME_MATERIAL))
					||
					(MEC_fn_bTypeOfGameMaterial (DNM_M_hObstacleGetCollidedMaterial(p_stTmpObstacle),C_xDNM_NO_COLLISION_TYPE_GAME_MATERIAL)
						&& MEC_fn_bTestMaskGameMaterial(DNM_M_hObstacleGetCollidedMaterial(p_stTmpObstacle),C_xDNM_COLLISION_REPORT_MASK)))
				{

					if (!bWater)
					{
						DNM_M_MecReportSetWater (&g_stReport,p_stTmpObstacle);
						bWaterEdge = bEdge;
						bWater = TRUE;
					}
					else if (bWaterEdge && !bEdge)
					{
						DNM_M_MecReportSetWater (&g_stReport,p_stTmpObstacle);
						bWaterEdge = FALSE;
					}
				}
				else
				{

					xDotProduct = MTH3D_M_xDotProductVector ( p_stNormalVector, &stVerticalVector );

					if (xDotProduct > MEC_fn_xGroundLimit(p_stDynamic) )
					{

						if (!bGround)
						{
							DNM_M_MecReportSetGround (&g_stReport,p_stTmpObstacle);
							bGroundEdge = bEdge;
							bGround = TRUE;
						}
						else if (bGroundEdge && !bEdge)
						{
							DNM_M_MecReportSetGround (&g_stReport,p_stTmpObstacle);
							bGroundEdge = FALSE;
						}
					}

					else if (xDotProduct>MEC_fn_xWallLimit(p_stDynamic))
					{

						if (!bWall)
						{
							DNM_M_MecReportSetWall (&g_stReport,p_stTmpObstacle);
							bWallEdge = bEdge;
							bWall = TRUE;
						}
						else if (bWallEdge && !bEdge)
						{
							DNM_M_MecReportSetWall (&g_stReport,p_stTmpObstacle);
							bWallEdge = FALSE;
						}
					}
					else
					{

						if (!bCeil)
						{
							DNM_M_MecReportSetCeil (&g_stReport,p_stTmpObstacle);
							bCeilEdge = bEdge;
							bCeil = TRUE;
						}
						else if (bCeilEdge && !bEdge)
						{
							DNM_M_MecReportSetCeil (&g_stReport,p_stTmpObstacle);
							bCeilEdge = FALSE;
						}
					}
				}
			}

			else bStop=TRUE;
		}
	}

	if (bActor)
	{
		DNM_M_ulReportAddSurfaceState (&g_stReport,C_WOT_ulMobile);
		MEC_fn_vReportCollideActor (hSupObj,DNM_M_p_stReportGetCharacter(&g_stReport));
	}

	if (bWater)
		DNM_M_ulReportAddSurfaceState (&g_stReport,C_WOT_ulWater);

	if (bGround)
	{
		DNM_M_ulReportAddSurfaceState (&g_stReport,C_WOT_ulGround);
		if (DNM_M_bDynamicsIsAdvancedSize(p_stDynamic)) DNM_M_vDynamicsSetGroundNormal (p_stDynamic,DNM_M_p_stObstacleGetNorm(DNM_M_p_stReportGetGround(&g_stReport)));
	}

	if (bWall)
	{
		DNM_M_ulReportAddSurfaceState (&g_stReport,C_WOT_ulWall);
		if (DNM_M_bDynamicsIsAdvancedSize(p_stDynamic)) DNM_M_vDynamicsSetWallNormal (p_stDynamic,DNM_M_p_stObstacleGetNorm(DNM_M_p_stReportGetWall(&g_stReport)));
	}

	if (bCeil)
		DNM_M_ulReportAddSurfaceState (&g_stReport,C_WOT_ulCeiling);
}

ACP_tdxBool __cdecl MEC_fn_vComputeAdjustVector (MTH3D_tdstVector *p_stAdjustVector, DNM_tdstDynamics *p_stDynamic,
                                                 long lObstacle, DNM_tdstMecObstacle *p_stObstacle, long lObstacleMin)
{
	long lCounter;
	long xObstacleType;
	ACP_tdxBool bAdjustVector = (ACP_tdxBool)0;
	ACP_tdxBool bContinue;
	ACP_tdxBool bConflict = (ACP_tdxBool)0;

	DNM_tdstMecObstacle *p_stTmpObstacle, *p_stWaterObstacle;
	MTH_tdxReal xDotProd;
	MTH3D_tdstVector stNormalAdjustVector, stTangentAdjustVector, stVector;
	MTH3D_tdstVector stAdjustAxis;

	p_stWaterObstacle = NULL;
	MTH3D_M_vNullVector (p_stAdjustVector);

	for (lCounter=0; lCounter<lObstacle && !p_stWaterObstacle; lCounter++)
	{
		p_stTmpObstacle = p_stObstacle+lCounter;
		if (MEC_fn_bIsObstacleValid(p_stTmpObstacle) && MEC_fn_bIsWater(p_stTmpObstacle))
			p_stWaterObstacle = p_stTmpObstacle;
	}

	for (xObstacleType=DNM_ObsType_Scenery; xObstacleType<=DNM_ObsType_Mobile; xObstacleType++)
	{
		bContinue = (ACP_tdxBool)1;

		for (lCounter=0; lCounter<lObstacle && bContinue; lCounter++)
		{
			p_stTmpObstacle = p_stObstacle+lCounter;

			if ( (DNM_M_eObstacleGetType(p_stTmpObstacle) & xObstacleType) == xObstacleType )
			{
				if ( lCounter<=lObstacleMin || MEC_fn_bIsCollide(p_stTmpObstacle) )
				{
					if ( MEC_fn_bIsObstacleValid(p_stTmpObstacle) && !MEC_fn_bIsWater(p_stTmpObstacle) )
					{
						if (bAdjustVector)
						{
							MTH3D_M_vCopyVector (&stVector,DNM_M_xObstacleGetTranslation(p_stTmpObstacle));
							if (!MEC_fn_bIsNullVector(p_stAdjustVector)) { MTH3D_M_vNormalizeVector (&stAdjustAxis,p_stAdjustVector); }
							else MTH3D_M_vCopyVector (&stAdjustAxis,p_stAdjustVector);

							xDotProd = MTH3D_M_xDotProductVector (&stAdjustAxis,&stVector);

							if (MTH_M_bGreater(xDotProd,MTH_C_ZERO))
							{
								if (MTH_M_bLess(xDotProd,MTH3D_M_xNormVector(p_stAdjustVector)))
								{
									MTH3D_M_vMulScalarVector (&stNormalAdjustVector,xDotProd,&stAdjustAxis);
									MTH3D_M_vSubVector (&stTangentAdjustVector,&stVector,&stNormalAdjustVector);
									MTH3D_M_vAddVector (p_stAdjustVector, p_stAdjustVector, &stTangentAdjustVector );
								}
								else
								{
									MTH3D_M_vCopyVector (p_stAdjustVector,&stVector);
								}
							}
							else
							{
								bConflict = (ACP_tdxBool)1;
								if (xObstacleType==DNM_ObsType_Scenery)
								{
									MTH3D_M_vMulScalarVector (&stNormalAdjustVector,xDotProd,&stAdjustAxis);
									MTH3D_M_vSubVector (&stTangentAdjustVector,&stVector,&stNormalAdjustVector);
									MTH3D_M_vAddVector (p_stAdjustVector, p_stAdjustVector, &stTangentAdjustVector );
								}
							}
						}
						else
						{
							MTH3D_M_vCopyVector (p_stAdjustVector,DNM_M_xObstacleGetTranslation(p_stTmpObstacle));
							bAdjustVector = (ACP_tdxBool)1;
						}
					}
				}
				else
					bContinue = (ACP_tdxBool)0;
			}
		}
	}

	if (p_stWaterObstacle)
	{
		MTH3D_M_vNormalizeVector (&stAdjustAxis,DNM_M_p_stObstacleGetNorm(p_stWaterObstacle));
		xDotProd = MTH3D_M_xDotProductVector ( &stAdjustAxis, p_stAdjustVector );
		MTH3D_M_vMulScalarVector (&stNormalAdjustVector,MTH_M_xSub(xDotProd,MTH_M_xConstFloatToReal(0.01f)),&stAdjustAxis);
		MTH3D_M_vSubVector (p_stAdjustVector,p_stAdjustVector,&stNormalAdjustVector);
		MTH3D_M_vAddVector (p_stAdjustVector,p_stAdjustVector,DNM_M_xObstacleGetTranslation(p_stWaterObstacle));
	}

	return (ACP_tdxBool)(!bConflict);
}

#ifndef C_xDNM_EDGE_GROUND_LIMIT
#define C_xDNM_EDGE_GROUND_LIMIT  (MTH_M_xConstFloatToReal(0.9848f))
#endif

typedef int MEC_tdxCtxKnob0, MEC_tdxCtxKnob1, MEC_tdxCtxKnob2, MEC_tdxCtxKnob3;
void __cdecl MEC_fn_vEdgeAnalysis ( DNM_tdstDynamics *p_stDynamic, long lObstacle, DNM_tdstMecObstacle *p_stObstacle, DNM_tdstMecObstacle *p_stGround )
{
	long lCounter;
	MTH3D_tdstVector stContact, stVector, stEdgeVector, stVector1;
	DNM_tdstMecObstacle *p_stTmpObstacle;
	DNM_tdstMecObstacle *p_stNotGround = NULL;
	ACP_tdxBool bNotGround = FALSE;

	long lGround = 0;

	MTH_tdxReal xDotProd;

	DNM_M_bDynamicsSetCanFall (p_stDynamic,FALSE);

	if ( !p_stGround )
	{
		for
		(
			lCounter = 0, p_stTmpObstacle = p_stObstacle;
			lCounter < lObstacle && !p_stGround;
			lCounter ++, p_stTmpObstacle ++
		)
		{
			if ( MEC_fn_bIsObstacleValid(p_stTmpObstacle) && MEC_fn_bGroundObstacle(p_stDynamic,p_stTmpObstacle) )
			{
				if ( MEC_fn_bIsEdge(p_stTmpObstacle) )
				{
					p_stGround = p_stTmpObstacle;
				}
				else return;
			}
		}

		if (!p_stGround)
			return;
	}
	else
	{
		if ( !MEC_fn_bIsEdge(p_stGround) )
			return;
	}

	if (MEC_fn_bIsDoubleEdge(p_stGround)) return;

	MTH3D_M_vCopyVector (&stContact,DNM_M_p_stObstacleGetContact(p_stGround));

	MTH3D_M_vAddVector (&stVector1, DNM_M_pObstacleGetZonePosition(p_stGround), DNM_M_xObstacleGetTranslation(p_stGround));
	MTH3D_M_vSubVector (&stVector1, &stVector1, &stContact);
	MTH3D_M_vNormalizeVector (&stVector1,&stVector1);

	for
	(
		lCounter = 0, p_stTmpObstacle = p_stObstacle;
		lCounter < lObstacle;
		lCounter ++, p_stTmpObstacle ++
	)
	{
		if ( MEC_fn_bIsObstacleValid(p_stTmpObstacle) )
		{
			if ( MEC_fn_bGroundObstacle(p_stDynamic,p_stTmpObstacle) )
			{
				if ( MEC_fn_bIsEdge(p_stTmpObstacle) )
				{
					MTH3D_M_vSubVector (&stVector,&stContact,DNM_M_p_stObstacleGetContact(p_stTmpObstacle));
					if ( MEC_fn_bIsNullVector(&stVector) ) lGround++;
				}
				else return;
			}
			else
			{
				if (MEC_fn_bIsLowEdge(p_stTmpObstacle))
				{
					MTH3D_M_vSubVector (&stVector,&stContact,DNM_M_p_stObstacleGetContact(p_stTmpObstacle));
					if ( MEC_fn_bIsNullVector(&stVector) )
					{
						if ( MTH_M_bGreater(DNM_M_xDynamicsGetPreviousSlide (p_stDynamic),MTH_M_xConstFloatToReal(0.5f)) )
						{
							xDotProd = MTH3D_M_xDotProductVector ( DNM_M_p_stObstacleGetNorm(p_stGround), DNM_M_p_stObstacleGetNorm(p_stTmpObstacle) );
							if (MTH_M_bGreater(xDotProd,MTH_M_xConstFloatToReal(0.707f)))
								return;
						}

						xDotProd = MTH3D_M_xDotProductVector ( DNM_M_p_stObstacleGetNorm(p_stTmpObstacle), &stVector1 );

						if ( !p_stNotGround && MTH_M_bGreaterZero(xDotProd) )
						{
							bNotGround = TRUE;
							p_stNotGround = p_stTmpObstacle;
						}
					}
				}
				else if (MEC_fn_bWallObstacle(p_stDynamic,p_stTmpObstacle) && !MEC_fn_bIsHighEdge(p_stTmpObstacle))
				{
					MTH3D_M_vSubVector (&stVector,&stContact,DNM_M_p_stObstacleGetContact(p_stTmpObstacle));
					xDotProd = MTH3D_M_xDotProductVector ( DNM_M_p_stObstacleGetNorm(p_stGround), &stVector );
					if (MTH_M_bLessZero(xDotProd))
					{
						xDotProd = MTH3D_M_xDotProductVector ( DNM_M_p_stObstacleGetNorm(p_stTmpObstacle), &stVector );
						/* 0x0049C828 */
						if (MTH_M_bLess(xDotProd, MTH_M_xConstFloatToReal(-0.001f)))
							return;
					}
				}
			}
		}
	}

	if (lGround >= 2)
		return;

	if ( bNotGround )
	{
		MTH3D_M_vAddVector (&stEdgeVector,DNM_M_p_stObstacleGetNorm(p_stGround),DNM_M_p_stObstacleGetNorm(p_stNotGround));
		bNotGround = !MEC_fn_bIsNullVector(&stEdgeVector);
	}

	if ( bNotGround )
	{
		MTH_tdxReal xDotProd2;
		MTH3D_M_vNormalizeVector (&stEdgeVector,&stEdgeVector);
		xDotProd2 = MTH3D_M_xDotProductVector (&stVector1,&stEdgeVector);

		if ( MTH_M_bLessZero(xDotProd2) )
			return;
	}

	if ( MTH_M_bLess(MTH3D_M_xGetZofVector(&stVector1), C_xDNM_EDGE_GROUND_LIMIT) )
	{
		DNM_M_bDynamicsSetCanFall(p_stDynamic,TRUE);

		if (DNM_M_bDynamicsIsComplexSize(p_stDynamic) )
		{
			MTH3D_M_vSetZofVector ( &stVector1, MTH_C_ZERO );
			if (!MEC_fn_bIsNullVector(&stVector1)) { MTH3D_M_vNormalizeVector (&stVector1,&stVector1); }
			else MTH3D_M_vNullVector (&stVector1);
			DNM_M_vDynamicsSetFallTranslation (p_stDynamic,&stVector1);
		}
	}
}
