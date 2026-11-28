#include <stddef.h>
#include "MTH/MTH_flt.h"
#include "MTH/MTH_xNeg.h"

#include "AI/AI_types.h"
#include "FON/fontext.h"
#include "GAM/GAM_types.h"
#include "DNM/DNM_types.h"
#include "DNM/DNM.h"
#include "MEC/MEC.h"

#include "AI/AITools.h"
#include "PRT/PRT_types.h"

#define _WP_D_WAYPOINT_FRIEND_
#include "AI/WPWayPt.h"
#undef _WP_D_WAYPOINT_FRIEND_

#include "GAM/cineinfo.h"

char *__cdecl fn_szGetStringFromTextOrStringParam(AI_tdstGetSetParam *p_stValue)
{
    char *pszString = NULL;

    if (p_stValue->eType == E_vt_String)
    {
        pszString = p_stValue->uParam.szString;
    }
    else if (p_stValue->eType == E_vt_Text)
    {
        pszString = FON_fn_szGetTextPointerForHandle(
            (FON_tdxHandleOfText)p_stValue->uParam.lValue);
    }

    return pszString;
}

/* 0x00482A00 */
void __cdecl fn_vGetCenterOfBoundingVolume(HIE_tdstSuperObject *hPerso, MTH3D_tdstVector *p_stVertex)
{
    GEO_tdxHandleToBoundingSphere hBoundingSphere;

    hBoundingSphere = (GEO_tdxHandleToBoundingSphere)hPerso->hBoundingVolume;

    if (hBoundingSphere != NULL)
    {

        MTH3D_M_vMulMatrixVector(p_stVertex, &HIE_fn_hGetSuperObjectGlobalMatrix(hPerso)->stRotationMatrix,
                                 &(hBoundingSphere->stCenterPoint));
        MTH3D_M_vAddVector(p_stVertex, p_stVertex, &HIE_fn_hGetSuperObjectGlobalMatrix(hPerso)->stTranslationVector);
    }
    else
    {

        MTH3D_M_vCopyVector(p_stVertex, &HIE_fn_hGetSuperObjectGlobalMatrix(hPerso)->stTranslationVector);
    }
}

void WP_fnv_WayPoint_ComputeLocation( WP_tdhWayPoint _hWayPoint, MTH3D_tdstVector*_p_stVertex);

/* 0x00482AD0 */
MTH_tdxReal __cdecl fn_xDistanceBetweenPersoAndWP(HIE_tdstSuperObject *p_SuperObjPerso, void *hWP)
{
    MTH3D_tdstVector stVertex1, stVertex2;

    POS_fn_vGetTranslationVector(HIE_fn_hGetSuperObjectGlobalMatrix(p_SuperObjPerso), &stVertex1);
    WP_fnv_WayPoint_ComputeLocation(hWP, &stVertex2);

    MTH3D_M_vSubVector(&stVertex1, &stVertex1, &stVertex2);
    return (MTH3D_M_xNormVector(&stVertex1));
}

#define AITOOLS_DISTANCE_TO_CENTER_OF_BOUNDING_VOLUME 0x80
#define AITOOLS_C_DISTANCE_X    1
#define AITOOLS_C_DISTANCE_Y    2
#define AITOOLS_C_DISTANCE_Z    4
#define AITOOLS_C_DISTANCE_XY   3
#define AITOOLS_C_DISTANCE_XZ   5
#define AITOOLS_C_DISTANCE_YZ   6
#define AITOOLS_C_DISTANCE_XYZ  7

/* 0x00482B50 */
static MTH_tdxReal fn_xInternalDistance(MTH3D_tdstVector *p_stVector, unsigned char ucFlags)
{
    MTH_tdxReal xX, xY, xZ;
    MTH3D_tdstVector stVec = *p_stVector;

    xX = MTH3D_M_xGetXofVector(p_stVector);
    xY = MTH3D_M_xGetYofVector(p_stVector);
    xZ = MTH3D_M_xGetZofVector(p_stVector);

    switch (ucFlags)
    {

        case AITOOLS_C_DISTANCE_X:
            return (xX);
        case AITOOLS_C_DISTANCE_Y:
            return (xY);
        case AITOOLS_C_DISTANCE_Z:
            return (xZ);

        case AITOOLS_C_DISTANCE_XY:
            stVec.xZ = MTH_C_ZERO;
            return MTH3D_M_xNormVector(&stVec);
        case AITOOLS_C_DISTANCE_XZ:
            stVec.xY = MTH_C_ZERO;
            return MTH3D_M_xNormVector(&stVec);
        case AITOOLS_C_DISTANCE_YZ:
            stVec.xX = MTH_C_ZERO;
            return MTH3D_M_xNormVector(&stVec);
        default:
            return 0;
            break;
        case AITOOLS_C_DISTANCE_XYZ:
            return (MTH3D_M_xNormVector(p_stVector));
    }
}
#define AITOOLS_C_DISTANCE_MASK AITOOLS_C_DISTANCE_XYZ

/* 0x00482C50 */
MTH_tdxReal __cdecl fn_xExtendedDistanceBetweenPerso(HIE_tdstSuperObject *p_SuperObjPerso1, HIE_tdstSuperObject *p_SuperObjPerso2, unsigned char ucFlags)
{
    MTH3D_tdstVector stVertex1, stVertex2;
    POS_tdstCompletePosition stMatrix;

    if (ucFlags & AITOOLS_DISTANCE_TO_CENTER_OF_BOUNDING_VOLUME)
    {
        fn_vGetCenterOfBoundingVolume(p_SuperObjPerso1, &stVertex1);
        fn_vGetCenterOfBoundingVolume(p_SuperObjPerso2, &stVertex2);
    }
    else
    {
        POS_fn_vGetTranslationVector(HIE_fn_hGetSuperObjectGlobalMatrix(p_SuperObjPerso1), &stVertex1);
        POS_fn_vGetTranslationVector(HIE_fn_hGetSuperObjectGlobalMatrix(p_SuperObjPerso2), &stVertex2);
    }

    MTH3D_M_vSubVector(&stVertex1, &stVertex1, &stVertex2);

    ucFlags &= AITOOLS_C_DISTANCE_MASK;
    if (ucFlags != AITOOLS_C_DISTANCE_MASK)
    {

        POS_fn_vInvertIsoMatrix(&stMatrix, HIE_fn_hGetSuperObjectGlobalMatrix(p_SuperObjPerso2));
        POS_fn_vMulMatrixVector(&stVertex1, &stMatrix, &stVertex1);
    }
    return (fn_xInternalDistance(&stVertex1, ucFlags));
}

/* 0x00482D20 */
MTH_tdxReal __cdecl fn_xComputeDirectionAngle(MTH3D_tdstVector *p_stVector)
{
    MTH_tdxReal xRealX, xRealY, xAngle;

    xRealX = MTH3D_M_xGetXofVector(p_stVector);
    xRealY = MTH3D_M_xGetYofVector(p_stVector);

    if (xRealX) xAngle = MTH_M_xATan(MTH_M_xDiv(xRealY, xRealX));
    else if (MTH_M_bLess(xRealY, MTH_C_ZERO)) xAngle = MTH_M_xNeg(MTH_C_PiBy2);
    else xAngle = MTH_C_PiBy2;

    if (MTH_M_bLess(xRealX, MTH_C_ZERO)) xAngle = MTH_M_xAdd(xAngle, MTH_C_Pi);
    if (MTH_M_bLess(xAngle, MTH_C_ZERO))  xAngle = MTH_M_xAdd(xAngle, MTH_C_2Pi);

    xAngle = MTH_M_xAdd(xAngle, MTH_C_PiBy2);
    if (MTH_M_bGreater(xAngle, MTH_C_2Pi)) xAngle = MTH_M_xSub(xAngle, MTH_C_2Pi);

    return (xAngle);
}

/* 0x00482DC0 */
MTH_tdxReal __cdecl fn_xComputeBestEcartDirectionAngle(MTH_tdxReal xAngleFollowed, MTH_tdxReal xAngleToFollow)
{
    MTH_tdxReal xDeltaAngle = MTH_M_xSub(xAngleToFollow, xAngleFollowed);

    if (MTH_M_bGreater(xDeltaAngle, MTH_C_ZERO))
    {
        if (MTH_M_bGreater(xDeltaAngle, MTH_C_Pi))
        {
            xDeltaAngle = MTH_M_xSub(xDeltaAngle, MTH_C_2Pi);
        }
    }
    else
    {
        if (MTH_M_bLess(xDeltaAngle, MTH_M_xNeg(MTH_C_Pi)))
        {
            xDeltaAngle = MTH_M_xAdd(xDeltaAngle, MTH_C_2Pi);
        }
    }
    return (xDeltaAngle);
}

/* 0x00482E20 */
void *__cdecl fn_pstGetDNMReport(HIE_tdstSuperObject *p_SuperObjPerso)
{
    MS_tdxHandleToDynam h_Dynam;
    DNM_tdstDynamics   *p_stDynamics;

    if ((h_Dynam = M_GetMSHandle(p_SuperObjPerso, Dynam)) == NULL)
        return NULL;
    if ((p_stDynamics = (DNM_tdstDynamics *)fn_p_stDynamGetDynamics(h_Dynam)) == NULL)
        return NULL;
    return DNM_M_p_stDynamicsGetReport(p_stDynamics);
}

/* 0x00482E50 */
float __cdecl fn_xRealModuloAround(float xFrom, float xModulus, float xValue)
{
    xValue = xValue - xFrom;
    while (xValue < 0.0f)
        xValue = xValue + xModulus;
    while (xValue >= xModulus)
        xValue = xValue - xModulus;
    xValue = xValue + xFrom;
    return xValue;
}

/* 0x00482EA0 */
unsigned char fn_ucIsThereFloatIfYesConvertItToInteger(AI_tdstGetSetParam *p_stParam)
{
	unsigned char ucIsThereFloat = (unsigned char) M_IsFloatType(p_stParam);

	if (ucIsThereFloat)
	{
		long lValue;

		if ((M_GetSetParam_xValue(p_stParam) < MTH_C_MAX_LONG) &&
			(M_GetSetParam_xValue(p_stParam) > (-MTH_C_MAX_LONG)))
			lValue = MTH_M_xRealToLong(M_GetSetParam_xValue(p_stParam));
		else if (M_GetSetParam_xValue(p_stParam) > 0)
			lValue = MTH_C_MAX_LONG;
		else
			lValue =- MTH_C_MAX_LONG;

		M_Full_GetSetParam_Integer(p_stParam, lValue);
	}
	return(ucIsThereFloat);
}

/* 0x00482F10 */
unsigned char __cdecl fn_ucAreThereFloatIfYesConvertThem(AI_tdstGetSetParam *p_stParam1,
                                                         AI_tdstGetSetParam *p_stParam2)
{
    unsigned char ucIsThereFloat = (unsigned char)(p_stParam1->eType == E_vt_Float);
    if (ucIsThereFloat)
    {
        if (p_stParam2->eType != E_vt_Float)
            p_stParam2->uParam.xValue = (float)(long)p_stParam2->uParam.lValue;
    }
    else
    {
        ucIsThereFloat = (unsigned char)(p_stParam2->eType == E_vt_Float);
        if (ucIsThereFloat)
            p_stParam1->uParam.xValue = (float)(long)p_stParam1->uParam.lValue;
    }
    return ucIsThereFloat;
}

/* 0x00482F50 */
unsigned char __cdecl fn_ucInternalTurn(HIE_tdstSuperObject *p_SuperObjPerso, MTH_tdxReal xAngle)
{
    DNM_tdstDynamics* pstDynamic;
    MS_tdxHandleToDynam h_Dynam;

    h_Dynam = M_GetMSHandle(p_SuperObjPerso,Dynam);

    if (h_Dynam)
    {
        pstDynamic = fn_p_stDynamGetDynamics(h_Dynam);

        if (pstDynamic)
        {
            MEC_vTurnAroundZAxis ( pstDynamic, xAngle );
            return TRUE;
        }
    }
    return FALSE;
}

/* 0x00482F80 */
void fn_vConvertParamToColor(struct AI_tdstGetSetParam_ *p_stParam,struct GEO_tdstColor_ *p_stColor)
{
	MTH_tdxReal lValue, fValue, fFactor = MTH_M_xConstFloatToReal(0.01f);

	lValue = MTH_M_xLongToReal(M_GetSetParam_RedValue(p_stParam));
	if (MTH_M_bLess(lValue, MTH_C_ZERO))
		fValue = MTH_C_ZERO;
	else if (MTH_M_bGreater(lValue, MTH_M_xLongToReal(100)))
		fValue = MTH_C_ONE;
	else
		fValue = MTH_M_xMul(lValue, fFactor);
	p_stColor->xR = fValue;

	lValue = MTH_M_xLongToReal(M_GetSetParam_GreenValue(p_stParam));
	if (MTH_M_bLess(lValue, MTH_C_ZERO))
		fValue = MTH_C_ZERO;
	else if (MTH_M_bGreater(lValue, MTH_M_xLongToReal(100)))
		fValue = MTH_C_ONE;
	else
		fValue = MTH_M_xMul(lValue, fFactor);
	p_stColor->xG = fValue;

	lValue = MTH_M_xLongToReal(M_GetSetParam_BlueValue(p_stParam));
	if (MTH_M_bLess(lValue, MTH_C_ZERO))
		fValue = MTH_C_ZERO;
	else if (MTH_M_bGreater(lValue, MTH_M_xLongToReal(100)))
		fValue = MTH_C_ONE;
	else
		fValue = MTH_M_xMul(lValue, fFactor);
	p_stColor->xB = fValue;

	lValue = MTH_M_xLongToReal(M_GetSetParam_AlphaValue(p_stParam));
	if (MTH_M_bLess(lValue, MTH_C_ZERO))
		fValue = MTH_C_ZERO;
	else if (MTH_M_bGreater(lValue, MTH_M_xLongToReal(100)))
		fValue = MTH_C_ONE;
	else
		fValue = MTH_M_xMul(lValue, fFactor);
	p_stColor->xA = fValue;
}

/* 0x004830C0 */
void fn_vConvertColorToParam(struct GEO_tdstColor_ *p_stColor,struct AI_tdstGetSetParam_ *p_stParam)
{
	M_GetSetParam_RedValue(p_stParam)	=(unsigned char) (100*p_stColor->xR);
	M_GetSetParam_GreenValue(p_stParam)	=(unsigned char) (100*p_stColor->xG);
	M_GetSetParam_BlueValue(p_stParam)	=(unsigned char) (100*p_stColor->xB);
	M_GetSetParam_AlphaValue(p_stParam)	=(unsigned char) (100*p_stColor->xA);
	M_GetSetParam_Type(p_stParam)=E_vt_Color;
}

/* 0x00487830 */
void WP_fnv_WayPoint_GetVertex
(
 WP_tdhWayPoint _hWayPoint,
 MTH3D_tdstVector*_pstVector
)
{
 WP_M_vWayPointGetVertex(_hWayPoint,_pstVector);
}

/* 0x00487850 */
MTH_tdxReal WP_fnx_WayPoint_GetRadius
(WP_tdhWayPoint _hWayPoint)
{
 return WP_M_xWayPointGetRadius(_hWayPoint);
}

/* 0x00487860 */
void WP_fnv_WayPoint_ComputeLocation( WP_tdhWayPoint _hWayPoint, MTH3D_tdstVector*_p_stVertex)
{
	MTH3D_tdstVector stWPVertex;
	POS_tdxHandleToPosition hGlobalMatrix;

	if (WP_M_hWayPointGetSuperObject(_hWayPoint))
	{
		WP_M_vWayPointGetVertex(_hWayPoint,&stWPVertex);
		hGlobalMatrix = HIE_fn_hGetSuperObjectGlobalMatrix (WP_M_hWayPointGetSuperObject(_hWayPoint));
		POS_fn_vMulMatrixVertex( _p_stVertex, HIE_fn_hGetSuperObjectGlobalMatrix (WP_M_hWayPointGetSuperObject(_hWayPoint)), &stWPVertex );
	}
	else
		WP_M_vWayPointGetVertex(_hWayPoint,_p_stVertex);
}
