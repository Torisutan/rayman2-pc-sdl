#include "ACP/ACP_types.h"
#include "DNM/DNM_types.h"
#include "POS/POS_types.h"
#include "MTH/MTH3D.h"
#include "HIE/HIE_types.h"
#include "MEC/MECTools.h"
#include "MEC/MEC.h"
#include "MEC/MECPfm.h"

/* 0x00413110 */
extern DNM_tdstDynamics *__cdecl fn_p_stDynamGetDynamicsSO (HIE_tdstSuperObject *hSupObj);

#ifndef C_WOT_ulMobile
#define C_WOT_ulMobile    0x00010000UL
#endif

#ifndef HIE_fn_bIsSuperObjectValid
#define HIE_fn_bIsSuperObjectValid(_hSO)  ((_hSO) != (HIE_tdstSuperObject *)0)
#endif

/* 0x00413730 */
extern char __cdecl fn_bStandardGameIsAPlatform (void *hSupObj);

#define HIE_M_vIsolate(_hSprObj)  HIE_fn_vSuperObjectIsolate(_hSprObj)

#ifndef HIE_fn_vInvalidateSuperObject
#define HIE_fn_vInvalidateSuperObject(_pp)  (*(_pp) = (struct HIE_tdstSuperObject *)0)
#endif

/* 0x0040B040 */
extern void __cdecl fn_vReputCharacterSuperObjectAtTheWorld (struct HIE_tdstSuperObject *h_SupObject);

void __cdecl MEC_fn_vUpdatePlatform (DNM_tdstDynamics *p_stDynamic, HIE_tdstSuperObject *hSupObj, POS_tdxHandleToPosition hPreviousReferenceMatrix, POS_tdxHandleToPosition hInvPreviousReferenceMatrix )
{
	POS_tdxHandleToPosition hCurrentMatrix;
	HIE_tdstSuperObject *hChild;

	hCurrentMatrix = DNM_M_p_stDynamicsGetCurrentMatrix (p_stDynamic);

	for (hChild = hSupObj->p_stFirstChild; hChild != (HIE_tdstSuperObject *)0; hChild = hChild->p_stNextBrother)
	{
		DNM_tdstDynamics* p_stChildDynamic;

		if (hChild->ulTypeOfLinkedObject == HIE_C_ulActor)
		{

			p_stChildDynamic = fn_p_stDynamGetDynamicsSO (hChild);
			if (p_stChildDynamic)
			{
				void *hIdCard = DNM_M_pDynamicsGetCurrentIdCard (p_stChildDynamic);

				if (hIdCard)
				{
					if (DNM_fn_eIdentityCardGetType(hIdCard)==DNM_eBase)
						MEC_fn_vDynamicComputeAbsoluteMatrix (p_stChildDynamic,hChild,hPreviousReferenceMatrix,hInvPreviousReferenceMatrix,hCurrentMatrix);
				}
			}
		}
	}
}

ACP_tdxBool __cdecl MEC_fn_vBasePlatformManagement ( DNM_tdstDynamics *p_stDynamic, HIE_tdstSuperObject *hSupObj )
{
	DNM_tdstMecObstacle* p_stObstacle;
	HIE_tdstSuperObject *hGeomSuperObject;
	HIE_tdstSuperObject *hObstacleSuperObject;

	if (!MEC_fn_bIsBaseMechanic(p_stDynamic)) return FALSE;

	if ( DNM_M_ulReportGetSurfaceState (&g_stReport) & C_WOT_ulMobile )
	{
		p_stObstacle = DNM_M_p_stReportGetCharacter (&g_stReport);

		hGeomSuperObject = DNM_M_p_stObstacleGetObject (p_stObstacle);

		if (DNM_M_bDynamicsIsGi(p_stDynamic))
		{
			if (!MEC_fn_bCeilObstacle(p_stDynamic,p_stObstacle)) return FALSE;
		}
		else if (DNM_M_bDynamicsIsClimb(p_stDynamic))
		{
			if (!MEC_fn_bWallObstacle(p_stDynamic,p_stObstacle) ) return FALSE;
		}
		else if (!MEC_fn_bGroundObstacle (p_stDynamic,p_stObstacle)) return FALSE;

		if (HIE_fn_bIsSuperObjectValid(hGeomSuperObject))
		{

			hObstacleSuperObject = MEC_fn_hGetFatherActor (hGeomSuperObject);

			if (HIE_fn_bIsSuperObjectValid(hObstacleSuperObject))
			{
				if (fn_bStandardGameIsAPlatform(hObstacleSuperObject))
				{
					if (fn_bStandardGameIsAPlatform(hSupObj)) return FALSE;
					if (MEC_fn_bIsPlatform(p_stDynamic))
					{
						if (DNM_M_hDynamicsGetPlatformSO(p_stDynamic)==hObstacleSuperObject) return FALSE;
						MEC_fn_vCutPlatformLink (p_stDynamic,hSupObj);
						MEC_fn_vSetPlatformLink (p_stDynamic,hSupObj,hObstacleSuperObject);
						return TRUE;
					}
					MEC_fn_vSetPlatformLink (p_stDynamic,hSupObj,hObstacleSuperObject);
					return TRUE;
				}
			}
		}
	}
	if (MEC_fn_bIsPlatform(p_stDynamic))
	{
		MEC_fn_vCutPlatformLink (p_stDynamic,hSupObj);
		return TRUE;
	}
	return FALSE;
}

/* 0x0043D950 */
void __cdecl MEC_fn_vSetPlatformLink (DNM_tdstDynamics *p_stDynamic, HIE_tdstSuperObject *hSupObj, HIE_tdstSuperObject *hFatherSupObj)
{
	if (DNM_M_bDynamicsIsLockPlatform (p_stDynamic)) return;
	DNM_M_vDynamicsSetPlatformSO (p_stDynamic, hFatherSupObj);
	HIE_M_vIsolate(hSupObj);
	HIE_fn_vSuperObjectAddTail(hFatherSupObj, hSupObj);
}

/* 0x0043DA00 */
void __cdecl MEC_fn_vCutPlatformLink (DNM_tdstDynamics *p_stDynamic, HIE_tdstSuperObject *hSupObj)
{
	if (DNM_M_bDynamicsIsLockPlatform (p_stDynamic)) return;
	HIE_fn_vInvalidateSuperObject (&DNM_M_hDynamicsGetPlatformSO (p_stDynamic));
	fn_vReputCharacterSuperObjectAtTheWorld (hSupObj);
}

void __cdecl MEC_fn_vForcePlatformLink (DNM_tdstDynamics *p_stDynamic, HIE_tdstSuperObject *hSupObj, HIE_tdstSuperObject *hFatherSupObj)
{
	MEC_fn_vSetPlatformLink ( p_stDynamic, hSupObj, hFatherSupObj);
	DNM_M_bDynamicsSetLockPlatform (p_stDynamic, TRUE);
}

/* 0x0043DA60 */
void __cdecl MEC_fn_vFreePlatformLink (DNM_tdstDynamics *p_stDynamic, HIE_tdstSuperObject *hSupObj)
{
	DNM_M_bDynamicsSetLockPlatform (p_stDynamic, FALSE);
	MEC_fn_vCutPlatformLink ( p_stDynamic, hSupObj );
}

void __cdecl MEC_fn_vDynamicComputeAbsoluteMatrix (
	DNM_tdstDynamics *p_stDynamic,
	HIE_tdstSuperObject *hSupObj,
	POS_tdxHandleToPosition hPreviousMatrix,
	POS_tdxHandleToPosition hInvPreviousMatrix,
	POS_tdxHandleToPosition hCurrentMatrix )
{
	POS_tdstCompletePosition stMatrix;
	POS_tdxHandleToPosition hMatrix;
	POS_tdxHandleToPosition hPreviousDynamicMatrix, hCurrentDynamicMatrix;
	MTH3D_tdstVector stVector;
	MTH3D_tdstVector *p_stI, *p_stJ, *p_stK;
	MTH3D_tdstVector *p_stContact;

	hPreviousDynamicMatrix = DNM_M_p_stDynamicsGetPreviousMatrix (p_stDynamic);
	hCurrentDynamicMatrix = DNM_M_p_stDynamicsGetCurrentMatrix (p_stDynamic);

	hMatrix = (POS_tdxHandleToPosition)&stMatrix;
	POS_fn_vMulMatrixMatrix (hMatrix, hCurrentMatrix, hInvPreviousMatrix);

	p_stContact = DNM_M_pDynamicsGetContact (p_stDynamic);
	POS_fn_vGetTranslationVector ( hCurrentDynamicMatrix, &stVector );
	MTH3D_M_vSubVector (&stVector,&stVector,p_stContact);
	MTH3D_M_vSetXofVector ( &stVector, MTH_C_ZERO );
	MTH3D_M_vSetYofVector ( &stVector, MTH_C_ZERO );

	POS_fn_vMulMatrixMatrix ( hPreviousDynamicMatrix, hMatrix, hPreviousDynamicMatrix );
	POS_fn_vMulMatrixMatrix ( hCurrentDynamicMatrix, hMatrix, hCurrentDynamicMatrix );

	if (!DNM_M_bDynamicsIsStickOnPlatform(p_stDynamic) && !MEC_fn_bIsEqualRotationMatrix (hPreviousMatrix,hCurrentMatrix) )
	{
		POS_fn_vGetRotationVector ( hPreviousDynamicMatrix, &p_stI, &p_stJ, &p_stK );
		MTH3D_M_vSetZofVector ( p_stJ, MTH_C_ZERO );
		MTH3D_M_vNormalizeVector ( p_stJ, p_stJ );
		MTH3D_M_vSetBaseKVector( p_stK );
		MTH3D_M_vCrossProductVectorWithoutBuffer(p_stI, p_stJ, p_stK);
		MTH3D_M_vNormalizeVector ( p_stI, p_stI );
		MEC_fn_vUpdateMatrix ( p_stDynamic, hPreviousDynamicMatrix );

		POS_fn_vGetRotationVector ( hCurrentDynamicMatrix, &p_stI, &p_stJ, &p_stK );
		MTH3D_M_vSetZofVector ( p_stJ, MTH_C_ZERO );
		MTH3D_M_vNormalizeVector ( p_stJ, p_stJ );
		MTH3D_M_vSetBaseKVector( p_stK );
		MTH3D_M_vCrossProductVectorWithoutBuffer(p_stI, p_stJ, p_stK);
		MTH3D_M_vNormalizeVector ( p_stI, p_stI );
		MEC_fn_vUpdateMatrix ( p_stDynamic, hCurrentDynamicMatrix );
	}

	POS_fn_vCopyMatrix ( HIE_fn_hGetSuperObjectGlobalMatrix (hSupObj), hCurrentDynamicMatrix );
	HIE_fn_vComputeNewRelativeMatrix (hSupObj);
}
