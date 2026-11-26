#include <math.h>
#include "GAM/GAM_types.h"
#include "GAM/HitPts.h"
#include "GAM/Caps.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "AI/AI_types.h"
#include "AI/Brain.h"
#include "POS/POS_types.h"
#include "GLI/Proj.h"
#include "GAM/MSWay.h"
#include "AI/AI_globals.h"
#include "GEO/GEO_types.h"
#include "GAM/PlayAnim.h"
#include "HIE/HIE_types.h"

#define HIE_fn_bIsSuperObjectValid(hSuperObject) ((hSuperObject) != (HIE_tdstSuperObject *)0)

#define AI_MAGIC_lReadLong(p)  ((p)->eType == 8 ? (long)(p)->uParam.xValue : (p)->uParam.lValue)

/* 0x00476130 */
AI_tdstNodeInterpret *__cdecl fn_p_stCode4PointsDeMagie(
    HIE_tdstSuperObject *p_SuperObjPerso, AI_tdstNodeInterpret *p_stTree, AI_tdstGetSetParam *p_stValue)
{
    unsigned long        eFuncId = *(unsigned long *)((char *)p_stTree - 8);
    long                 lValue = 0;
    long                 lNumberOfMagicPoints;
    AI_tdstGetSetParam   stParam;

    switch (eFuncId)
    {
    case 0x60:
        lValue = AI_MAGIC_iPoints;
        break;

    case 0x61:
        lValue = AI_MAGIC_iPointsMax;
        break;

    case 0x62:
        {

            p_stTree = fn_p_stEvalTree(p_SuperObjPerso, p_stTree, &stParam);

            lNumberOfMagicPoints = AI_MAGIC_lReadLong(&stParam);

            AI_MAGIC_iPoints += lNumberOfMagicPoints;

            if (AI_MAGIC_iPoints > AI_MAGIC_iPointsMax)
                AI_MAGIC_iPoints = AI_MAGIC_iPointsMax;

            lValue = AI_MAGIC_iPoints;
        }
        break;

    case 0x63:
        {
            p_stTree = fn_p_stEvalTree(p_SuperObjPerso, p_stTree, &stParam);
            lNumberOfMagicPoints = AI_MAGIC_lReadLong(&stParam);
            AI_MAGIC_iPointsMax += lNumberOfMagicPoints;

            lValue = AI_MAGIC_iPointsMax;
        }
        break;

    case 0x64:
        {
            p_stTree = fn_p_stEvalTree(p_SuperObjPerso, p_stTree, &stParam);
            lNumberOfMagicPoints = AI_MAGIC_lReadLong(&stParam);

            AI_MAGIC_iPoints -= lNumberOfMagicPoints;

            if (AI_MAGIC_iPoints < 0)
                AI_MAGIC_iPoints = 0;
            lValue = AI_MAGIC_iPoints;
        }
        break;

    case 0x65:
        {
            p_stTree = fn_p_stEvalTree(p_SuperObjPerso, p_stTree, &stParam);
            lNumberOfMagicPoints = AI_MAGIC_lReadLong(&stParam);
            AI_MAGIC_iPointsMax -= lNumberOfMagicPoints;

            if (AI_MAGIC_iPointsMax < 0)
                AI_MAGIC_iPointsMax = 0;
            lValue = AI_MAGIC_iPointsMax;
        }
        break;

    default:
        return p_stTree;
    }

    p_stValue->uParam.lValue = lValue;
    p_stValue->eType         = 6;
    return p_stTree;
}

#define AI_AIR_xReadReal(p)  ((p)->eType == 8 ? (p)->uParam.xValue : (float)(p)->uParam.lValue)

/* 0x00475F90 */
AI_tdstNodeInterpret *__cdecl fn_p_stCode4PointsDair(
    HIE_tdstSuperObject *p_SuperObjPerso, AI_tdstNodeInterpret *p_stTree, AI_tdstGetSetParam *p_stValue)
{
    unsigned long        eFuncId = *(unsigned long *)((char *)p_stTree - 8);
    float                xValue = MTH_C_ZERO;
    float                xNumberOfAirPoints;
    float                xSign = MTH_C_ONE;
    AI_tdstGetSetParam   stParam;

    switch (eFuncId)
    {
    case 0x66:
        xValue = AI_AIR_xPoints;
        break;

    case 0x67:
        xValue = AI_AIR_xPointsMax;
        break;

    case 0x6a:
        xSign = MTH_C_MinusONE;

    case 0x68:
        {

            p_stTree = fn_p_stEvalTree(p_SuperObjPerso, p_stTree, &stParam);
            xNumberOfAirPoints = MTH_M_xMul(xSign, AI_AIR_xReadReal(&stParam));

            xValue = MTH_M_xAdd(AI_AIR_xPoints, xNumberOfAirPoints);

            if (MTH_M_bGreater(xValue, AI_AIR_xPointsMax))
                xValue = AI_AIR_xPointsMax;
            else if (MTH_M_bLessZero(xValue))
                xValue = MTH_C_ZERO;

            AI_AIR_xPoints = xValue;
        }
        break;

    case 0x6b:
        xSign = MTH_C_MinusONE;

    case 0x69:
        {

            p_stTree = fn_p_stEvalTree(p_SuperObjPerso, p_stTree, &stParam);
            xNumberOfAirPoints = MTH_M_xMul(xSign, AI_AIR_xReadReal(&stParam));

            xValue = MTH_M_xAdd(AI_AIR_xPointsMax, xNumberOfAirPoints);

            if (MTH_M_bLessZero(xValue))
                xValue = MTH_C_ZERO;
            if (MTH_M_bLess(xValue, AI_AIR_xPoints))
                AI_AIR_xPoints = xValue;

            AI_AIR_xPointsMax = xValue;
        }
        break;

    default:
        return p_stTree;
    }

    p_stValue->uParam.xValue = xValue;
    p_stValue->eType         = 8;
    return p_stTree;
}

#define AI_M_pfn_fn_ucGetFunctionUltraOperator2 fn_ucGetFunctionUltraOperator

#define AI_M_pfn_fn_h_SectInfoGetCurrentSector  ((void *(__cdecl *)(void *))&fn_h_SectInfoGetCurrentSector)
/* 0x00478380 */
HIE_tdstSuperObject *fn_hGetNearestActorInSectorWithAnyCustombit(
    HIE_tdstSuperObject *_hMe,
    HIE_tdstSuperObject *_hSector,
    unsigned long _ulFilterCustomBits);

/* 0x00478450 */
AI_tdstNodeInterpret *__cdecl fn_p_stGetNearestActorInCurrentSector(
    HIE_tdstSuperObject *p_SuperObjPerso, AI_tdstNodeInterpret *p_stTree, AI_tdstGetSetParam *p_stValue);

/* 0x0040b200 */
extern long __cdecl GAM_fn_lGetNbPersoActive(void);

extern void __cdecl fn_vOnAIErm_M_UpdateLastError(unsigned short FatalErrorNum, char *szMsg);
#define AI_M_pfn_M_AIFatalError_fr2(uwError) \
    fn_vOnAIErm_M_UpdateLastError((unsigned short)(uwError), (char *)0)

/* 0x00478220 */
AI_tdstNodeInterpret *__cdecl fn_p_stGetNbActivePerso(
    HIE_tdstSuperObject *p_SuperObjPerso, AI_tdstNodeInterpret *p_stTree, AI_tdstGetSetParam *p_stValue)
{
    unsigned long eFuncId = *(unsigned long *)((char *)p_stTree - 8);

    if (eFuncId == 0x6f)
    {
        p_stValue->uParam.lValue = GAM_fn_lGetNbPersoActive();
        p_stValue->eType         = 6;
        return p_stTree;
    }
    AI_M_pfn_M_AIFatalError_fr2(0x27);
    return p_stTree;
}

#define AI_M_pfn_MSWay_lGetNumberWP    ((long (__cdecl *)(void *))MSWay_fn_lGetNumberWayPointInPath)
#define AI_M_pfn_MSWay_lGetCurrentIdx  ((long (__cdecl *)(void *))MSWay_fn_lGetCurrentIndexInPath)

#define AI_M_pfn_MSWay_hGetWP          ((void *(__cdecl *)(void *, long))&MSWay_fn_hGetWayPointOfPath)
extern float __cdecl WP_fnx_WayPoint_GetRadius(void *_hWayPoint);
/* 0x00487830 */
extern void __cdecl WP_fnv_WayPoint_GetVertex(void *_hWayPoint, void *_pstVector);
#define AI_M_pfn_WP_vGetVertex_rail    ((void (__cdecl *)(void *, float *))&WP_fnv_WayPoint_GetVertex)

/* 0x00477D20 */
AI_tdstNodeInterpret *__cdecl fn_p_stGetSituationOnRail(
    HIE_tdstSuperObject *p_SuperObjPerso, AI_tdstNodeInterpret *p_stTree, AI_tdstGetSetParam *p_stValue)
{
    unsigned long         eFuncId = *(unsigned long *)((char *)p_stTree - 8);
    HIE_tdstSuperObject *hNewPerso;
    AI_tdstGetSetParam   stParam;
    float                xDist;
    void                *h_MSWay;
    long                 lCurrentIndex;
    long                 lNbWp;
    void                *hWayPoint;
    long                 lValue = 0;
    MTH3D_tdstVector     stWpPosition;
    MTH3D_tdstVector     stPersoPosition;

    fn_vGetUltraOperatorPerso(
        AI_M_pfn_fn_ucGetFunctionUltraOperator2(eFuncId), p_SuperObjPerso, &hNewPerso);

    p_stTree = fn_p_stEvalTree(p_SuperObjPerso, p_stTree, &stParam);
    xDist    = stParam.uParam.xValue;

    h_MSWay = M_GetMSHandle(hNewPerso, MSWay);
    if (h_MSWay == NULL)
    {
        p_stValue->uParam.lValue = -1;
        p_stValue->eType         = 6;
        return p_stTree;
    }

    lNbWp = MSWay_fn_lGetNumberWayPointInPath(h_MSWay);

    lCurrentIndex = MSWay_fn_lGetCurrentIndexInPath(h_MSWay);

    if (lCurrentIndex == 1)
    {
        lValue = 1;

        hWayPoint = MSWay_fn_hGetWayPointOfPath(h_MSWay, 0);
    }
    else
    {
        if (lCurrentIndex == (lNbWp - 1))
        {
            lValue = 2;

            hWayPoint = MSWay_fn_hGetWayPointOfPath(h_MSWay, lCurrentIndex);
        }
        else
        {
            p_stValue->uParam.lValue = 0;
            p_stValue->eType         = 6;
            return p_stTree;
        }
    }

    if (MTH_M_bEqualZero(xDist))
        xDist = WP_fnx_WayPoint_GetRadius(hWayPoint);

    POS_fn_vGetTranslationVector(HIE_fn_hGetSuperObjectMatrix(hNewPerso), &stPersoPosition);

    WP_fnv_WayPoint_GetVertex(hWayPoint, &stWpPosition);

    if (!MTH3D_M_bEqualVector(&stWpPosition, &stPersoPosition) &&
        MTH_M_bLess(MTH3D_M_xVectorGap(&stWpPosition, &stPersoPosition), xDist))
    {
        p_stValue->uParam.lValue = lValue;
    }
    else
    {
        p_stValue->uParam.lValue = 0;
    }
    p_stValue->eType = 6;
    return p_stTree;
}

#define AI_M_pfn_fn_ulGetCapabilities ((unsigned long (__cdecl *)(void *))fn_ulGetCapabilities)

static __inline unsigned long HIE_fn_ulGetSuperObjectType(struct HIE_tdstSuperObject *_hSprObj);

/* 0x00477E90 */
AI_tdstNodeInterpret *__cdecl fn_p_stPersoCapabilityFunction(
    HIE_tdstSuperObject *p_SuperObjPerso, AI_tdstNodeInterpret *p_stTree, AI_tdstGetSetParam *p_stValue)
{
    unsigned long        eFuncId = AI_M_eFuncIdInterpret(p_stTree - 1);
    HIE_tdstSuperObject *hNewSuperObjPerso;
    void                *hEngineObj;

    fn_vGetUltraOperatorPerso(
        AI_M_pfn_fn_ucGetFunctionUltraOperator2(eFuncId), p_SuperObjPerso, &hNewSuperObjPerso);

    hEngineObj = M_GetEngineObject(hNewSuperObjPerso);
    p_stValue->uParam.lValue = (long)AI_M_pfn_fn_ulGetCapabilities(hEngineObj);
    p_stValue->eType         = 0x2b;
    return p_stTree;
}

/* 0x00477EE0 */
AI_tdstNodeInterpret *__cdecl fn_p_stCapabilityFunction(
    HIE_tdstSuperObject *p_SuperObjPerso, AI_tdstNodeInterpret *p_stTree, AI_tdstGetSetParam *p_stValue)
{
    AI_tdstNodeInterpret *p_stNext     = fn_p_stEvalTree(p_SuperObjPerso, p_stTree, p_stValue);
    unsigned char         ucBitNumber  = p_stValue->uParam.ucValue;

    p_stValue->uParam.lValue = (long)(1 << ucBitNumber);
    p_stValue->eType         = 0x2b;
    return p_stNext;
}

#define AI_M_pfn_PLA_fn_uwGetNbFrameInCurrAnim PLA_fn_uwGetNbFrameInCurrAnim

/* 0x00477F90 */
AI_tdstNodeInterpret *__cdecl fn_p_stGetNbFrame(
    HIE_tdstSuperObject *p_SuperObjPerso, AI_tdstNodeInterpret *p_stTree, AI_tdstGetSetParam *p_stValue)
{
    unsigned long        eFuncId = *(unsigned long *)((char *)p_stTree - 8);
    HIE_tdstSuperObject *hNewSuperObjPerso;

    fn_vGetUltraOperatorPerso(
        AI_M_pfn_fn_ucGetFunctionUltraOperator2(eFuncId), p_SuperObjPerso, &hNewSuperObjPerso);

    if (eFuncId == 0x94)
    {
        p_stValue->uParam.lValue = AI_M_pfn_PLA_fn_uwGetNbFrameInCurrAnim(hNewSuperObjPerso);
        p_stValue->eType         = 6;
        return p_stTree;
    }
    AI_M_pfn_M_AIFatalError_fr2(0x27);
    return p_stTree;
}

/* 0x004784F0 */
AI_tdstNodeInterpret *__cdecl fn_p_stDotProduct(
    HIE_tdstSuperObject *p_SuperObjPerso, AI_tdstNodeInterpret *p_stTree, AI_tdstGetSetParam *p_stValue)
{
    AI_tdstNodeInterpret *p_stNext;
    MTH3D_tdstVector      stV1, *p_stV2;
    MTH_tdxReal           xReturnValue;

    p_stV2 = (MTH3D_tdstVector *)p_stValue->uParam.a_xVectorValue;

    p_stNext = fn_p_stEvalTree(p_SuperObjPerso, p_stTree, p_stValue);
    stV1 = *p_stV2;
    p_stNext = fn_p_stEvalTree(p_SuperObjPerso, p_stNext, p_stValue);

    xReturnValue = MTH3D_M_xDotProductVector(&stV1, p_stV2);
    M_Full_GetSetParam_Float(p_stValue, xReturnValue);
    return p_stNext;
}

#define AI_FR2_xNormOne     (1.0f)
#define AI_FR2_xNormEpsilon (1.0e-9f)

/* 0x00478000 */
AI_tdstNodeInterpret *__cdecl fn_p_stVectorOperations(HIE_tdstSuperObject *p_SuperObjPerso, AI_tdstNodeInterpret *p_stTree, AI_tdstGetSetParam *p_stValue)
{
	long eFuncId = AI_M_eFuncIdInterpret(p_stTree-1);
	MTH3D_tdstVector *p_stV = &M_GetSetParam_stVertexValue(p_stValue);

	M_EvalNextParameter(p_stValue);

	switch ( eFuncId )
	{

	case 0x96:
		{
			MTH3D_tdstVector stV1 = *p_stV, stV2;

			M_EvalNextParameter(p_stValue);
			stV2 = M_GetSetParam_stVectorValue(p_stValue);

			MTH3D_M_vCrossProductVectorWithoutBuffer(p_stV,&stV1,&stV2);

		}
		return p_stTree;
		break;

	case 0x97:
        if ( !(	MTH_M_bEqualWithEpsilon ( MTH3D_M_xGetXofVector(p_stV), MTH_C_ZERO, AI_FR2_xNormEpsilon /* 0x49CCD8 */ )
				&&	MTH_M_bEqualWithEpsilon ( MTH3D_M_xGetYofVector(p_stV), MTH_C_ZERO, AI_FR2_xNormEpsilon )
				&&	MTH_M_bEqualWithEpsilon ( MTH3D_M_xGetZofVector(p_stV), MTH_C_ZERO, AI_FR2_xNormEpsilon )
			  )
		   )
        {

            MTH3D_M_vNormalizeVector(p_stV, p_stV);

        }
		return (p_stTree);
		break;

	default:
		break;
	}

	AI_M_pfn_M_AIFatalError_fr2(0x27);
	return p_stTree;
}

/* 0x00478180 */
AI_tdstNodeInterpret *__cdecl fn_p_stGetSPOCoordinates(
    HIE_tdstSuperObject *p_SuperObjPerso, AI_tdstNodeInterpret *p_stTree, AI_tdstGetSetParam *p_stValue)
{
    long                  eFuncId = AI_M_eFuncIdInterpret(p_stTree - 1);
    AI_tdstGetSetParam    stParam;
    HIE_tdstSuperObject  *hSpo;

    M_EvalNextParameter(&stParam);
    hSpo = M_GetSetParam_SuperObjectValue(&stParam);

    if (eFuncId == 0x98)
    {
        MTH3D_tdstVector stCoordinates;

        if (HIE_fn_bIsSuperObjectValid(hSpo))
            POS_fn_vGetTranslationVector(HIE_fn_hGetSuperObjectMatrix(hSpo), &stCoordinates);
        else
            MTH3D_M_vNullVector(&stCoordinates);

        M_Full_GetSetParam_p_stVertex(p_stValue, &stCoordinates);

        return (p_stTree);
    }

    M_AIFatalError(E_uwAIFatalNotValidFunction);
    return(p_stTree);
}

/* 0x00478260 */
AI_tdstNodeInterpret *__cdecl fn_p_stTractionFactorFunction(
    HIE_tdstSuperObject *p_SuperObjPerso, AI_tdstNodeInterpret *p_stTree, AI_tdstGetSetParam *p_stValue)
{
    unsigned long        eFuncId = AI_M_eFuncIdInterpret(p_stTree - 1);
    HIE_tdstSuperObject *hNewSuperObjPerso;
    void                *hEngineObj;
    unsigned char        ucTractionFactor = 0;

    fn_vGetUltraOperatorPerso(
        AI_M_pfn_fn_ucGetFunctionUltraOperator2(eFuncId), p_SuperObjPerso, &hNewSuperObjPerso);

    hEngineObj = M_GetEngineObject(hNewSuperObjPerso);

    if (eFuncId == 0x99)

        ucTractionFactor =
            ((struct tdstStandardGame_ *)((struct tdstEngineObject_ *)hEngineObj)->h_StandardGame)->ucTractionFactor;

    p_stValue->uParam.ucValue = ucTractionFactor;
    p_stValue->eType          = 3;
    return p_stTree;
}

/* 0x00478560 */
AI_tdstNodeInterpret *__cdecl fn_p_stGetCPUCounter(
    HIE_tdstSuperObject *p_SuperObjPerso, AI_tdstNodeInterpret *p_stTree, AI_tdstGetSetParam *p_stValue)
{

    __asm {
        push edx
        _emit 0x0F
        _emit 0x31
        pop edx
        mov ecx, p_stValue
        mov [ecx], eax
        mov eax, p_stTree
        mov dword ptr [ecx+0Ch], 6
    }
}

/* 0x0047F800 */
extern long __cdecl WPG_fn_lGetLinkCapabilities(int *hGraph, int hSrcWayPoint, int hDstWayPoint, int *p_ulCapability);
#define AI_M_pfn_WPG_fn_lGetLinkCapabilities \
    ((long (__cdecl *)(void *, unsigned long, unsigned long, unsigned long *))&WPG_fn_lGetLinkCapabilities)
/* 0x0047F7A0 */
extern long __cdecl WPG_fn_lChangeLinkCapabilities(int *hGraph, int hSrcWayPoint, int hDstWayPoint, unsigned int ulCaps, int lValue);
extern long __cdecl WPG_fn_lGetLinkWeight(int *hGraph, int hSrcWayPoint, int hDstWayPoint, int *p_lWeight);
extern long __cdecl WPG_fn_lChangeLinkWeight(int *hGraph, int hSrcWayPoint, int hDstWayPoint, int lWeight);
#define AI_M_pfn_WPG_fn_lChangeLinkCapabilities WPG_fn_lChangeLinkCapabilities
#define AI_M_pfn_WPG_fn_lGetLinkWeight          WPG_fn_lGetLinkWeight
#define AI_M_pfn_WPG_fn_lChangeLinkWeight       WPG_fn_lChangeLinkWeight

#include "AI/GSParam.h"

/* 0x00476F20 */
AI_tdstNodeInterpret *__cdecl fn_p_stGraphFunction(
    HIE_tdstSuperObject *p_SuperObjPerso, AI_tdstNodeInterpret *p_stTree, AI_tdstGetSetParam *p_stValue)
{
    long                  eFuncId = *(long *)((char *)p_stTree - 8);
    void                 *hGraph;
    unsigned long         hWayPoint1, hWayPoint2;
    long                  lValue;
    long                  lWeight;
    unsigned long         ulNewCaps;

    p_stTree   = fn_p_stEvalTree(p_SuperObjPerso, p_stTree, p_stValue);
    hGraph     = p_stValue->uParam.p_stEngObjValue;
    p_stTree   = fn_p_stEvalTree(p_SuperObjPerso, p_stTree, p_stValue);
    hWayPoint1 = p_stValue->uParam.ulValue;
    p_stTree   = fn_p_stEvalTree(p_SuperObjPerso, p_stTree, p_stValue);
    hWayPoint2 = p_stValue->uParam.ulValue;

    if (hGraph == NULL)
    {
        if (eFuncId == 0x8C)
        {
            M_Full_GetSetParam_Caps(p_stValue, (unsigned long)0);
        }
        else
        {
            M_Full_GetSetParam_Integer(p_stValue, -1);
        }
        return(p_stTree);
    }

    switch (eFuncId)
    {
    case 0x8C:
        if (AI_M_pfn_WPG_fn_lGetLinkCapabilities(hGraph, hWayPoint1, hWayPoint2, &ulNewCaps) == -1)
        {
            M_Full_GetSetParam_Caps(p_stValue, (unsigned long)0);
        }
        else
        {
            M_Full_GetSetParam_Caps(p_stValue, ulNewCaps);
        }
        return(p_stTree);

    case 0x8D:
        p_stTree  = fn_p_stEvalTree(p_SuperObjPerso, p_stTree, p_stValue);
        ulNewCaps = p_stValue->uParam.ulValue;
        p_stTree  = fn_p_stEvalTree(p_SuperObjPerso, p_stTree, p_stValue);
        lValue    = p_stValue->uParam.lValue;
        lValue    = AI_M_pfn_WPG_fn_lChangeLinkCapabilities(hGraph, hWayPoint1, hWayPoint2, ulNewCaps, lValue);
        M_Full_GetSetParam_Integer(p_stValue, lValue);
        return(p_stTree);

    case 0x8E:
        if (AI_M_pfn_WPG_fn_lGetLinkWeight(hGraph, hWayPoint1, hWayPoint2, &lWeight) == -1)
        {
            M_Full_GetSetParam_Integer(p_stValue, -1);
        }
        else
        {
            M_Full_GetSetParam_Integer(p_stValue, lWeight);
        }
        return(p_stTree);

    case 0x8F:
        p_stTree = fn_p_stEvalTree(p_SuperObjPerso, p_stTree, p_stValue);
        lWeight  = p_stValue->uParam.lValue;
        lValue   = AI_M_pfn_WPG_fn_lChangeLinkWeight(hGraph, hWayPoint1, hWayPoint2, lWeight);
        M_Full_GetSetParam_Integer(p_stValue, lValue);
        return(p_stTree);

    default:
        break;
    }

    AI_M_pfn_M_AIFatalError_fr2(0x27);
    return p_stTree;
}

struct WP_tdstGraph_;
struct WP_tdstWayPoint_;
extern struct WP_tdstGraph_ *__cdecl WPG_fn_lShortestPath(struct WP_tdstGraph_ *, struct WP_tdstWayPoint_ *,
                                                          struct WP_tdstWayPoint_ *, unsigned long);
extern struct WP_tdstGraph_ *__cdecl WPG_fn_lBuildOrderedPath(struct WP_tdstGraph_ *, struct WP_tdstWayPoint_ *);
extern void __cdecl MSWay_fn_vSetPath(void *, struct WP_tdstGraph_ *, char);
extern void __cdecl MSWay_fn_vDesinitPath(void *);
#define AI_M_pfn_WPG_fn_lShortestPath(h, a, b, c) \
    WPG_fn_lShortestPath((struct WP_tdstGraph_ *)(h), (struct WP_tdstWayPoint_ *)(a), \
                         (struct WP_tdstWayPoint_ *)(b), (unsigned long)(c))
#define AI_M_pfn_WPG_fn_lBuildOrderedPath(h, a) \
    WPG_fn_lBuildOrderedPath((struct WP_tdstGraph_ *)(h), (struct WP_tdstWayPoint_ *)(a))

#define AI_M_pfn_WPG_fn_lNumberOfWaypointOfGraph WPG_fn_lNumberOfWaypointOfGraph
#define AI_M_pfn_MSWay_fn_vSetPath      MSWay_fn_vSetPath
#define AI_M_pfn_MSWay_fn_vDesinitPath  MSWay_fn_vDesinitPath

#include "HIE/HieSpObj.h"

#include "AI/WPgraphe.h"

/* 0x004194B0 */
long WPG_fn_lNumberOfWaypointOfGraph (WP_tdHandleOfGraph hGraph) {
	return (LST2_M_DynamicGetNumberOfElements(&hGraph->m_hListOfNode)) ;
}

/* 0x004770F0 */
AI_tdstNodeInterpret *__cdecl fn_p_stGraphToWayFunction(
    HIE_tdstSuperObject *p_SuperObjPerso, AI_tdstNodeInterpret *p_stTree, AI_tdstGetSetParam *p_stValue)
{
    long                      eFuncId;
    void                     *h_MSWay;
    WP_tdHandleOfGraph        hGraph;
    long                      lValue;
    struct tdstEngineObject_ *hEngineObj;
    char                      bIsCircular;
    void                     *hPath;
    WP_tdhWayPoint            hWayPoint1, hWayPoint2;
    WP_tdhGraphNode           hCurrentNode;
    unsigned long             ulPersoCapability;
    HIE_tdstSuperObject      *hNewSuperObjPerso;
    int                       i;

    eFuncId = AI_M_eFuncIdInterpret(p_stTree - 1);
    fn_vGetUltraOperatorPerso(
        AI_M_pfn_fn_ucGetFunctionUltraOperator2(eFuncId), p_SuperObjPerso, &hNewSuperObjPerso);
    h_MSWay = M_GetMSHandle(hNewSuperObjPerso, MSWay);

    M_EvalNextParameter(p_stValue);
    hGraph = M_GetSetParam_GraphValue(p_stValue);

    lValue = 0;

    bIsCircular = 0;

    switch (eFuncId)
    {
    case 0x79:
        M_EvalNextParameter(p_stValue);
        hWayPoint1 = M_GetSetParam_WayPointValue(p_stValue);
        M_EvalNextParameter(p_stValue);
        hWayPoint2 = M_GetSetParam_WayPointValue(p_stValue);

        if (h_MSWay == NULL || hGraph == NULL)
        {
            M_Full_GetSetParam_Integer(p_stValue, 0L);
            return(p_stTree);
        }

        hEngineObj        = M_GetEngineObject(hNewSuperObjPerso);
        ulPersoCapability = AI_M_pfn_fn_ulGetCapabilities(hEngineObj);
        hPath             = AI_M_pfn_WPG_fn_lShortestPath(hGraph, hWayPoint1, hWayPoint2, ulPersoCapability);

        if (hPath != NULL)
        {
            AI_M_pfn_MSWay_fn_vSetPath(h_MSWay, hPath, 0);
            lValue = AI_M_pfn_WPG_fn_lNumberOfWaypointOfGraph(hPath);
        }
        else
        {
            AI_M_pfn_MSWay_fn_vDesinitPath(h_MSWay);
            lValue = 0;
        }

        M_Full_GetSetParam_Integer(p_stValue, lValue);
        return(p_stTree);

    case 0x7B:
        bIsCircular = 1;
    case 0x7A:
        {
            M_EvalNextParameter(p_stValue);
            hWayPoint1 = M_GetSetParam_WayPointValue(p_stValue);

            if (h_MSWay == NULL || hGraph == NULL)
            {
                M_Full_GetSetParam_Integer(p_stValue, 0L);
                return(p_stTree);
            }

            hPath = AI_M_pfn_WPG_fn_lBuildOrderedPath(hGraph, hWayPoint1);

            if (hPath != NULL)
            {
                AI_M_pfn_MSWay_fn_vSetPath(h_MSWay, hPath, bIsCircular);
                lValue = AI_M_pfn_WPG_fn_lNumberOfWaypointOfGraph(hPath);
            }
            else
            {
                AI_M_pfn_MSWay_fn_vDesinitPath(h_MSWay);
                lValue = 0;
            }

            M_Full_GetSetParam_Integer(p_stValue, lValue);
            return(p_stTree);
        }

    case 0x7D:
        bIsCircular = 1;
    case 0x7C:
        {
            if (h_MSWay == NULL || hGraph == NULL)
            {
                M_Full_GetSetParam_Integer(p_stValue, 0);
                return(p_stTree);
            }

            LST2_M_DynamicGetElementNumber(&hGraph->m_hListOfNode, hCurrentNode, 0, i);

            hPath = AI_M_pfn_WPG_fn_lBuildOrderedPath(hGraph, hCurrentNode->m_hWayPoint);

            if (hPath != NULL)
            {
                AI_M_pfn_MSWay_fn_vSetPath(h_MSWay, hPath, bIsCircular);
                lValue = AI_M_pfn_WPG_fn_lNumberOfWaypointOfGraph(hPath);
            }
            else
            {
                AI_M_pfn_MSWay_fn_vDesinitPath(h_MSWay);
                lValue = 0;
            }

            M_Full_GetSetParam_Integer(p_stValue, lValue);
            return(p_stTree);
        }

    default:
        break;
    }

    M_AIFatalError(E_uwAIFatalNotValidFunction);
    return(p_stTree);
}

#define AI_M_pfn_MSWay_lGetCurrentIndexInPath  ((long (__cdecl *)(void *))MSWay_fn_lGetCurrentIndexInPath)
#define AI_M_pfn_MSWay_lSetCurrentIndexInPath  ((long (__cdecl *)(void *, long))MSWay_fn_lSetCurrentIndexInPath)
#define AI_M_pfn_MSWay_lGetFirstIndexInPath    ((long (__cdecl *)(void *))MSWay_fn_lGetFirstIndexInPath)
#define AI_M_pfn_MSWay_lGetLastIndexInPath     ((long (__cdecl *)(void *))MSWay_fn_lGetLastIndexInPath)
#define AI_M_pfn_MSWay_lIncrementCurrentIndexInPath ((long (__cdecl *)(void *, long))MSWay_fn_lIncrementCurrentIndexInPath)

#define AI_M_pfn_MSWay_hGetWayPointOfPath      ((unsigned long (__cdecl *)(void *, long))&MSWay_fn_hGetWayPointOfPath)

#define AI_M_pfn_MSWay_ulGetCapsForNextWayPointOfPath ((unsigned long (__cdecl *)(void *, long))&MSWay_fn_ulGetCapsForNextWayPointOfPath)

#define AI_M_pfn_MSWay_lChangeCapsForNextWayPointOfPath ((long (__cdecl *)(void *, long, unsigned long, long))&MSWay_fn_lChangeCapsForNextWayPointOfPath)

#define AI_M_pfn_MSWay_lGetWeightForNextWayPointOfPath ((long (__cdecl *)(void *, long))&MSWay_fn_lGetWeightForNextWayPointOfPath)

#define AI_M_pfn_MSWay_lChangeWeightForNextWayPointOfPath ((long (__cdecl *)(void *, long, long))&MSWay_fn_lChangeWeightForNextWayPointOfPath)

#define AI_M_pfn_MSWay_lLookForWPInMSWay       ((long (__cdecl *)(void *, unsigned long))&MSWay_fn_lLookForWPInMSWay)

/* 0x00477300 */
AI_tdstNodeInterpret *__cdecl fn_p_stWayFunction(
    HIE_tdstSuperObject *p_SuperObjPerso, AI_tdstNodeInterpret *p_stTree, AI_tdstGetSetParam *p_stValue)
{
    long                  eFuncId = AI_M_eFuncIdInterpret(p_stTree - 1);
    HIE_tdstSuperObject  *hNewSuperObjPerso;
    void                 *h_MSWay;
    long                  lValue, lIndex, lWeight;
    unsigned long         ulNewCaps;

    fn_vGetUltraOperatorPerso(
        AI_M_pfn_fn_ucGetFunctionUltraOperator2(eFuncId), p_SuperObjPerso, &hNewSuperObjPerso);
    h_MSWay = M_GetMSHandle(hNewSuperObjPerso, MSWay);

    if (h_MSWay == NULL)
    {

        switch (eFuncId)
        {
        case 0x84:
            p_stValue->uParam.ulValue = 0;
            p_stValue->eType          = 0x0a;
            break;
        case 0x85:
            p_stValue->uParam.ulValue = 0;
            p_stValue->eType          = 0x2b;
            break;
        default:
            break;
        }
        p_stValue->uParam.lValue = -1;
        p_stValue->eType         = 6;
        return p_stTree;
    }

    switch (eFuncId)
    {
    case 0x7E:
        lValue = AI_M_pfn_MSWay_lGetCurrentIndexInPath(h_MSWay);
        p_stValue->uParam.lValue = lValue;
        p_stValue->eType         = 6;
        return p_stTree;

    case 0x7F:
        p_stTree = fn_p_stEvalTree(p_SuperObjPerso, p_stTree, p_stValue);
        lValue   = AI_M_pfn_MSWay_lSetCurrentIndexInPath(h_MSWay, p_stValue->uParam.lValue);
        p_stValue->uParam.lValue = lValue;
        p_stValue->eType         = 6;
        return p_stTree;

    case 0x80:
        lValue = AI_M_pfn_MSWay_lGetFirstIndexInPath(h_MSWay);
        p_stValue->uParam.lValue = lValue;
        p_stValue->eType         = 6;
        return p_stTree;

    case 0x81:
        lValue = AI_M_pfn_MSWay_lGetLastIndexInPath(h_MSWay);
        p_stValue->uParam.lValue = lValue;
        p_stValue->eType         = 6;
        return p_stTree;

    case 0x82:
        p_stTree = fn_p_stEvalTree(p_SuperObjPerso, p_stTree, p_stValue);
        lValue   = AI_M_pfn_MSWay_lIncrementCurrentIndexInPath(h_MSWay, p_stValue->uParam.lValue);
        p_stValue->uParam.lValue = lValue;
        p_stValue->eType         = 6;
        return p_stTree;

    case 0x83:
        p_stTree = fn_p_stEvalTree(p_SuperObjPerso, p_stTree, p_stValue);
        lValue   = AI_M_pfn_MSWay_lIncrementCurrentIndexInPath(h_MSWay, -(p_stValue->uParam.lValue));
        p_stValue->uParam.lValue = lValue;
        p_stValue->eType         = 6;
        return p_stTree;

    case 0x84:
        p_stTree = fn_p_stEvalTree(p_SuperObjPerso, p_stTree, p_stValue);
        p_stValue->uParam.ulValue = AI_M_pfn_MSWay_hGetWayPointOfPath(h_MSWay, p_stValue->uParam.lValue);
        p_stValue->eType          = 0x0a;
        return p_stTree;

    case 0x85:
        p_stTree = fn_p_stEvalTree(p_SuperObjPerso, p_stTree, p_stValue);
        ulNewCaps = AI_M_pfn_MSWay_ulGetCapsForNextWayPointOfPath(h_MSWay, p_stValue->uParam.lValue);
        p_stValue->uParam.ulValue = ulNewCaps;
        p_stValue->eType          = 0x2b;
        return p_stTree;

    case 0x86:
        p_stTree = fn_p_stEvalTree(p_SuperObjPerso, p_stTree, p_stValue);
        lIndex   = p_stValue->uParam.lValue;
        p_stTree = fn_p_stEvalTree(p_SuperObjPerso, p_stTree, p_stValue);
        ulNewCaps = p_stValue->uParam.ulValue;
        p_stTree = fn_p_stEvalTree(p_SuperObjPerso, p_stTree, p_stValue);
        lValue   = p_stValue->uParam.lValue;
        lValue   = AI_M_pfn_MSWay_lChangeCapsForNextWayPointOfPath(h_MSWay, lIndex, ulNewCaps, lValue);
        p_stValue->uParam.lValue = lValue;
        p_stValue->eType         = 6;
        return p_stTree;

    case 0x87:
        p_stTree = fn_p_stEvalTree(p_SuperObjPerso, p_stTree, p_stValue);
        lWeight  = AI_M_pfn_MSWay_lGetWeightForNextWayPointOfPath(h_MSWay, p_stValue->uParam.lValue);
        p_stValue->uParam.lValue = lWeight;
        p_stValue->eType         = 6;
        return p_stTree;

    case 0x88:
        p_stTree = fn_p_stEvalTree(p_SuperObjPerso, p_stTree, p_stValue);
        lIndex   = p_stValue->uParam.lValue;
        p_stTree = fn_p_stEvalTree(p_SuperObjPerso, p_stTree, p_stValue);
        lWeight  = p_stValue->uParam.lValue;
        lValue   = AI_M_pfn_MSWay_lChangeWeightForNextWayPointOfPath(h_MSWay, lIndex, lWeight);
        p_stValue->uParam.lValue = lValue;
        p_stValue->eType         = 6;
        return p_stTree;

    case 0x89:
        p_stTree = fn_p_stEvalTree(p_SuperObjPerso, p_stTree, p_stValue);
        lValue   = AI_M_pfn_MSWay_lLookForWPInMSWay(h_MSWay, p_stValue->uParam.ulValue);
        p_stValue->uParam.lValue = lValue;
        p_stValue->eType         = 6;
        return p_stTree;

    case 0x8A:
        p_stTree = fn_p_stEvalTree(p_SuperObjPerso, p_stTree, p_stValue);
        lIndex   = AI_M_pfn_MSWay_lLookForWPInMSWay(h_MSWay, p_stValue->uParam.ulValue);
        if (lIndex != -1)
            lValue = AI_M_pfn_MSWay_lSetCurrentIndexInPath(h_MSWay, lIndex);
        else
            lValue = -1;
        p_stValue->uParam.lValue = lValue;
        p_stValue->eType         = 6;
        return p_stTree;

    default:
        break;
    }

    AI_M_pfn_M_AIFatalError_fr2(0x27);
    return p_stTree;
}

/* 0x00480050 */
WP_tdhWayPoint __cdecl WPG_fn_hGetWaypointOfGraph(WP_tdHandleOfGraph hGraph, int iIndex, long *_plTypeOfWP);
/* 0x00487860 */
extern void __cdecl WP_fnv_WayPoint_ComputeLocation(WP_tdhWayPoint _hWayPoint, MTH3D_tdstVector *_p_stVertex);
#define AI_M_pfn_WP_fnv_WayPoint_ComputeLocation \
    ((void (__cdecl *)(void *, float *))WP_fnv_WayPoint_ComputeLocation)

#define AI_M_pfn_HIE_bDetectIntersectSegmentWithSuperObject(a, b, c) \
    HIE_bDetectIntersectSegmentWithSuperObject((MTH3D_tdstVector *)(a), (MTH3D_tdstVector *)(b), (HIE_tdstSuperObject *)(c))

#include "GAM/Actions/SectInfo.h"
extern void *__cdecl fn_h_SectInfoGetCurrentSector(void *h_SectInfo);

/* 0x00477630 */
AI_tdstNodeInterpret *__cdecl fn_p_stCloserWPInGraph(HIE_tdstSuperObject *p_SuperObjPerso, AI_tdstNodeInterpret *p_stTree, AI_tdstGetSetParam *p_stValue)
{
	long                       eFuncId = AI_M_eFuncIdInterpret(p_stTree-1);
	long                       lValue, lIndex, lNbWPInGraph, lTypeOfWPToTest = 0, lTypeOfWP;
	WP_tdHandleOfGraph         hGraph;
	WP_tdhWayPoint		         hBestWP = (WP_tdhWayPoint)NULL;
	WP_tdhWayPoint		         hWayPoint = NULL, hWayPoint1 = NULL, hWayPoint2 = NULL;
	MTH_tdxReal			           xBestDist = MTH_C_MinusONE;
	MTH_tdxReal                xTmp;
	MTH3D_tdstVector	         stCurrentWPVector, stPosPerso, stTempVector;
	HIE_tdstSuperObject       *hCurrentSectorPerso;

	HIE_tdstSuperObject       *hNewSuperObjPerso;
	fn_vGetUltraOperatorPerso(fn_ucGetFunctionUltraOperator(eFuncId),p_SuperObjPerso,&hNewSuperObjPerso);

	M_EvalNextParameter(p_stValue);
	hGraph = M_GetSetParam_GraphValue(p_stValue) ;

	M_EvalNextParameter( p_stValue );
	lValue = M_GetSetParam_lValue(p_stValue) ;

	switch(eFuncId)
	{
	case 0x73:
		M_EvalNextParameter( p_stValue );
		lTypeOfWPToTest = (long)M_GetSetParam_CapsValue(p_stValue);

		M_EvalNextParameter( p_stValue );
		hWayPoint1 = M_GetSetParam_WayPointValue(p_stValue) ;
		M_EvalNextParameter( p_stValue );
		hWayPoint2 = M_GetSetParam_WayPointValue(p_stValue) ;

	case 0x72:
		{

			if (hGraph == NULL)
			{
				M_Full_GetSetParam_WayPoint(p_stValue, (WP_tdhWayPoint)NULL);
				return(p_stTree);
				break;
			}

			POS_fn_vGetTranslationVector(HIE_fn_hGetSuperObjectGlobalMatrix(hNewSuperObjPerso),&stPosPerso);

			hCurrentSectorPerso = fn_h_SectInfoGetCurrentSector(M_GetMSHandle(hNewSuperObjPerso, SectInfo));

			lNbWPInGraph = WPG_fn_lNumberOfWaypointOfGraph (hGraph) ;

			for ( lIndex = 0; lIndex < lNbWPInGraph; lIndex ++ )
			{
				hWayPoint = WPG_fn_hGetWaypointOfGraph(hGraph, lIndex, &lTypeOfWP);
				if
					(
					(eFuncId != 0x73)
					||
					(
					(hWayPoint != hWayPoint1)
					&& (hWayPoint != hWayPoint2)
					&& ((lTypeOfWPToTest & lTypeOfWP) == lTypeOfWPToTest)
					)
					)
				{

					WP_fnv_WayPoint_ComputeLocation( hWayPoint, &stCurrentWPVector);
					MTH3D_M_vSubVector(&stTempVector, &stCurrentWPVector, &stPosPerso);
					xTmp = MTH3D_M_xNormVector(&stTempVector);

					if
						(
						(MTH_M_bEqual(xBestDist, MTH_C_MinusONE))
						||
						(MTH_M_bLess(xTmp, xBestDist))
						)
					{
						if ( lValue )
						{

							if
								(
								!HIE_bDetectIntersectSegmentWithSuperObject(
								&stPosPerso,

								&stTempVector,
								hCurrentSectorPerso
								)
								)
							{
								xBestDist = MTH3D_M_xNormVector(&stTempVector);
								hBestWP = hWayPoint;
							}
						}
						else
						{
							xBestDist = MTH3D_M_xNormVector(&stTempVector);
							hBestWP = hWayPoint;
						}
					}
				}
			}
			M_Full_GetSetParam_WayPoint(p_stValue, hBestWP);
			return(p_stTree);
		}
		break;

#if !defined(_AI_EXCLUDE_NEVER_USED_)
	case 0x75:
		M_EvalNextParameter( p_stValue );
		lTypeOfWPToTest = (long)M_GetSetParam_CapsValue(p_stValue);

		M_EvalNextParameter( p_stValue );
		hWayPoint1 = M_GetSetParam_WayPointValue(p_stValue) ;
		M_EvalNextParameter( p_stValue );
		hWayPoint2 = M_GetSetParam_WayPointValue(p_stValue) ;

	case 0x74:
		{

			MTH3D_tdstVector stAxisVector,
				stPersoWaypointVector;

			AI_tdstGetSetParam  stParam;

			M_EvalNextParameter(&stParam);
			stAxisVector = M_GetSetParam_stVectorValue(&stParam);

			if (hGraph == NULL)
			{
				M_Full_GetSetParam_WayPoint(p_stValue, (WP_tdhWayPoint)NULL);
				return(p_stTree);
				break;
			}

			POS_fn_vGetTranslationVector(HIE_fn_hGetSuperObjectGlobalMatrix(hNewSuperObjPerso),&stPosPerso);

			hCurrentSectorPerso = fn_h_SectInfoGetCurrentSector(M_GetMSHandle(hNewSuperObjPerso, SectInfo));

			lNbWPInGraph = WPG_fn_lNumberOfWaypointOfGraph (hGraph) ;
			hBestWP	   = NULL;

			for ( lIndex = 0; lIndex < lNbWPInGraph; lIndex ++ )
			{
				hWayPoint = WPG_fn_hGetWaypointOfGraph(hGraph, lIndex, &lTypeOfWP);

				if (
					(eFuncId != 0x75) ||
					( (hWayPoint != hWayPoint1)	&& (hWayPoint != hWayPoint2) &&
					((lTypeOfWPToTest & lTypeOfWP) == lTypeOfWPToTest) )
					)
				{

					WP_fnv_WayPoint_ComputeLocation( hWayPoint, &stCurrentWPVector);

					MTH3D_M_vSubVector( &stPersoWaypointVector, &stCurrentWPVector, &stPosPerso );

					xTmp = MTH3D_M_xDotProductVector(&stAxisVector, &stPersoWaypointVector )/MTH3D_M_xNormVector(&stPersoWaypointVector );

					if ( (MTH_M_bEqual(xBestDist, MTH_C_MinusONE)) || (MTH_M_bGreater(xTmp, xBestDist)) )
					{
						if (lValue)
						{

							if (
								!HIE_bDetectIntersectSegmentWithSuperObject( &stPosPerso,

								&stPersoWaypointVector,
								hCurrentSectorPerso )
								)
							{
								xBestDist = xTmp;
								hBestWP   = hWayPoint;
							}
						}
						else
						{
							xBestDist = xTmp;
							hBestWP   = hWayPoint;
						}
					}
				}
			}

			M_Full_GetSetParam_WayPoint(p_stValue, hBestWP);
			return(p_stTree);
		}
		break;

#endif

	default:
		break;
  }

  M_AIFatalError(0x27  );

  return(p_stTree);
}

extern void *__cdecl WPG_fn_hGetArcList(int *hSrcNode);
#define AI_M_pfn_WPG_fn_hGetArcList(n)  WPG_fn_hGetArcList((int *)(n))

/* 0x00477A80 */
AI_tdstNodeInterpret *__cdecl fn_p_stTravelOnAGraph(
    HIE_tdstSuperObject *p_SuperObjPerso, AI_tdstNodeInterpret *p_stTree, AI_tdstGetSetParam *p_stValue)
{
    long                       eFuncId = *(long *)((char *)p_stTree - 8);
    long                       lIndex, lTypeOfWP;
    unsigned long              ulCapaToTest;
    WP_tdHandleOfGraph         hGraph;
    WP_tdhWayPoint             hWayPoint, hWayPoint2;
    WP_tdhGraphNode            hCurrentNode;
    WP_tdHandleOfListOfArc     hListOfArc = NULL;
    long                       lValue = 0;

    M_EvalNextParameter(p_stValue);
    hGraph = M_GetSetParam_GraphValue(p_stValue);

    lTypeOfWP = 0L;
    if ((eFuncId == 0x77) || (eFuncId == 0x78))
    {

        M_EvalNextParameter(p_stValue);
        lTypeOfWP = (long)M_GetSetParam_CapsValue(p_stValue);
    }

    M_EvalNextParameter(p_stValue);
    hWayPoint = M_GetSetParam_WayPointValue(p_stValue);

    ulCapaToTest = (unsigned long)0;
    if ((eFuncId == 0x76) || (eFuncId == 0x78))
    {

        M_EvalNextParameter(p_stValue);
        ulCapaToTest = (unsigned long)M_GetSetParam_CapsValue(p_stValue);
    }

    if ((eFuncId == 0x77) || (eFuncId == 0x78))
    {
        M_EvalNextParameter(p_stValue);
        lValue = (unsigned char)M_GetSetParam_lValue(p_stValue);
    }

    lIndex = -1;
    hWayPoint2 = (WP_tdhWayPoint)NULL;

    LST2_M_DynamicForEachElementOf(&hGraph->m_hListOfNode, hCurrentNode, lIndex) {
        if (hCurrentNode->m_hWayPoint == hWayPoint) {
            hWayPoint2 = hWayPoint;
            break;
        }
    }

    if (hWayPoint2) {
        hListOfArc = (WP_tdHandleOfListOfArc)AI_M_pfn_WPG_fn_hGetArcList(hCurrentNode);
    }

    switch (eFuncId)
    {
    case 0x76:
        {
            hWayPoint2 = (WP_tdhWayPoint)NULL;
            if (hListOfArc) {

                long lBestWeight = (long)(((unsigned long)(1 << 31)) - 1);
                WP_tdHandleOfArc hElement;
                LST2_M_DynamicForEachElementOf(&hListOfArc->m_hArc, hElement, lIndex) {

                    hWayPoint = hElement->m_hNode->m_hWayPoint;

                    if ((hElement->m_ulCapability & ulCapaToTest) && (hElement->m_lWeight < lBestWeight)) {
                        lBestWeight = hElement->m_lWeight;
                        hWayPoint2 = hWayPoint;
                    }
                }
            }
            M_Full_GetSetParam_WayPoint(p_stValue, hWayPoint2);
        }
        break;

    case 0x77:
    case 0x78:
        {
            long lNbOfConnectedWP = 0;

            if (lValue == 2)
            {
                LST2_M_DynamicForEachElementOf(&hGraph->m_hListOfNode, hCurrentNode, lIndex) {
                    hCurrentNode->m_lTypeOfWP &= ~lTypeOfWP;
                }
            }
            if (lValue == 3)
            {
                LST2_M_DynamicForEachElementOf(&hGraph->m_hListOfNode, hCurrentNode, lIndex) {
                    hCurrentNode->m_lTypeOfWP = 0;
                }
            }

            if (hListOfArc) {

                WP_tdHandleOfArc hElement;
                LST2_M_DynamicForEachElementOf(&hListOfArc->m_hArc, hElement, lIndex) {

                    if ((eFuncId != 0x78)
                        || (hElement->m_ulCapability & ulCapaToTest)) {
                        lNbOfConnectedWP++;

                        switch (lValue)
                        {
                        case 0:
                        case 2:
                            hElement->m_hNode->m_lTypeOfWP |= lTypeOfWP;
                            break;
                        case 1:
                        case 3:
                            hElement->m_hNode->m_lTypeOfWP = lTypeOfWP;
                            break;
                        case 4:
                            hElement->m_hNode->m_lTypeOfWP &= ~lTypeOfWP;
                            break;
                        case 5:
                            hElement->m_hNode->m_lTypeOfWP = 0;
                            break;
                        }

                    }
                }
            }
            M_Full_GetSetParam_Integer(p_stValue, lNbOfConnectedWP);

        }
        break;

    default:

        AI_M_pfn_M_AIFatalError_fr2(0x27);
        break;
    }

    return(p_stTree);
}

/* 0x00477CC0 */
AI_tdstNodeInterpret *__cdecl fn_p_stMiscFuncOnTypeOfWP(
    HIE_tdstSuperObject *p_SuperObjPerso, AI_tdstNodeInterpret *p_stTree, AI_tdstGetSetParam *p_stValue)
{
	long                       lTypeOfWP, lIndex;
	WP_tdHandleOfGraph         hGraph;
	WP_tdhWayPoint		         hWayPoint;
	WP_tdhGraphNode            hCurrentNode, hTheNode;

	M_EvalNextParameter(p_stValue);
  hGraph = (WP_tdHandleOfGraph)M_GetSetParam_GraphValue(p_stValue) ;

	M_EvalNextParameter( p_stValue );
	hWayPoint = (WP_tdhWayPoint)M_GetSetParam_WayPointValue(p_stValue) ;

	lIndex = -1;
	hTheNode = (WP_tdhGraphNode)NULL;

	LST2_M_DynamicForEachElementOf(&hGraph->m_hListOfNode, hCurrentNode, lIndex) {
		if ( hCurrentNode->m_hWayPoint == hWayPoint ) {
      hTheNode = hCurrentNode;
			break ;
		}
	}

  lTypeOfWP = 0L;

	if ( hTheNode )
		lTypeOfWP = hTheNode->m_lTypeOfWP;

  M_Full_GetSetParam_Caps(p_stValue, lTypeOfWP);

	return(p_stTree);
}

extern void __cdecl fn_vGetScrollSpeedMatCollided(HIE_tdstSuperObject *p_stSuperObj, MTH3D_tdstVector *p_stVector,
                                                  unsigned long ulObstacleType);

/* 0x00477F10 */
AI_tdstNodeInterpret *__cdecl fn_p_stGetScrollSpeed(
    HIE_tdstSuperObject *p_SuperObjPerso, AI_tdstNodeInterpret *p_stTree, AI_tdstGetSetParam *p_stValue)
{
    long               eFuncId = *(long *)((char *)p_stTree - 8);
    AI_tdstGetSetParam stParam;

    if (eFuncId == 0x93)
    {
        MTH3D_tdstVector stVector;
        unsigned long    ulObstacleType;

        p_stTree       = fn_p_stEvalTree(p_SuperObjPerso, p_stTree, &stParam);
        ulObstacleType = stParam.uParam.ulValue;
        fn_vGetScrollSpeedMatCollided(p_SuperObjPerso, &stVector, ulObstacleType);

        MTH3D_M_vCopyVector((MTH3D_tdstVector *)p_stValue->uParam.a_xVectorValue, &stVector);
        p_stValue->eType = 0x15;
        return p_stTree;
    }

    AI_M_pfn_M_AIFatalError_fr2(0x27);
    return p_stTree;
}

extern ACP_tdxIndex __cdecl GEO_xGetCenterPointOfIndexedSphere(ACP_tdxHandleOfObject hObject,
                                                               ACP_tdxHandleOfElement hElement,
                                                               ACP_tdxIndex xNumberOfSphere);
extern void __cdecl GEO_vGetPointOfObject(ACP_tdxHandleOfObject hObject,
                                          MTH3D_tdstVector *p_stPoint, ACP_tdxIndex xIndexOfPoint);
extern void __cdecl GEO_vGetMinMaxPointOfIndexedAlignedBox(ACP_tdxHandleOfObject hObject,
                                                           ACP_tdxHandleOfElement hElement,
                                                           ACP_tdxIndex xNumberOfAlignedBox,
                                                           ACP_tdxIndex *p_xMinPoint,
                                                           ACP_tdxIndex *p_xMaxPoint);

/* 0x00476880 */
ACP_tdxBool __cdecl fn_vGetZoneCenter(ACP_tdxHandleOfObject hGeoObj, MTH3D_tdstVector *p_stZoneCenter)
{
    ACP_tdxBool bZoneIsValid;
    ACP_tdxIndex *p_xListOfTypes = hGeoObj->d_xListOfElementsTypes;

    switch (*p_xListOfTypes)
    {

    case GEO_C_xElementSpheres:
        {
            ACP_tdxIndex xCenter;

            xCenter = GEO_xGetCenterPointOfIndexedSphere(hGeoObj, 0, 0);
            GEO_vGetPointOfObject(hGeoObj, p_stZoneCenter, xCenter);
            bZoneIsValid = 1;
        }
        break;

    case GEO_C_xElementAlignedBoxes:
        {
            ACP_tdxIndex xMin, xMax;
            MTH3D_tdstVector stMin, stMax;

            GEO_vGetMinMaxPointOfIndexedAlignedBox(hGeoObj, 0, 0, &xMin, &xMax);
            GEO_vGetPointOfObject(hGeoObj, &stMin, xMin);
            GEO_vGetPointOfObject(hGeoObj, &stMax, xMax);

            MTH3D_M_vAddVector(p_stZoneCenter, &stMin, &stMax);
            MTH3D_M_vMulScalarVector(p_stZoneCenter, MTH_C_Inv2, p_stZoneCenter);
            bZoneIsValid = 1;
        }
        break;

    default:
        bZoneIsValid = 0;
        break;
    }
    return bZoneIsValid;
}

#define C_ucTypeZdd (unsigned char)0
#define C_ucTypeZdm (unsigned char)1
#define C_ucTypeZde (unsigned char)2
#define C_ucTypeZdr (unsigned char)3

#include "HIE/HieSpObj.h"

#include "POS/PosProto.h"

extern void *PO_fn_hGetCollideSet(void *_hPhysicalObject);
extern void *PCS_fn_hGetZddGeoObjOfPhysicalCollSet(void *_hPhyCollSet);
extern void *PCS_fn_hGetZdeGeoObjOfPhysicalCollSet(void *_hPhyCollSet);
extern void *PCS_fn_hGetZdrGeoObjOfPhysicalCollSet(void *_hPhyCollSet);
extern GMT_tdxHandleToGameMaterial fn_hIsTypeExistsInGeoObj(ACP_tdxHandleOfObject _hGeoObj, GMT_tdxMask _wMaterialMask);
extern ACP_tdxHandleOfObject fn_hIsThisZoneActive(HIE_tdstSuperObject *_hCharacter, unsigned char _ucZoneType, unsigned char _ucZoneNumber);

extern void *PCS_fn_hGetZdmGeoObjOfPhysicalCollSet(void *_hPhyCollSet);

/* 0x00476630 */
void fn_vInternGetZDxCenter(
    HIE_tdstSuperObject *_hCharacter,
    GMT_tdxMask _xMask,
    unsigned char _ucZoneType,
    ACP_tdxBool _bInGlobalCoordinates,
    MTH3D_tdstVector *_p_stReturnVector
)
{
    long lI, lMax;
    ACP_tdxHandleOfObject hGeoObj;
    ACP_tdxBool bFoundOne = 0;

    lMax = fn_lGetZdxListNumberOfElements(_ucZoneType, _hCharacter);
    for ( lI = 0; lI < lMax; lI ++ )
    {
        hGeoObj = fn_hGetGeometricZdxOfTypeAtIndex(_ucZoneType, (unsigned short)lI, _hCharacter);
        if ( hGeoObj && fn_hIsTypeExistsInGeoObj(hGeoObj, _xMask) && fn_hIsThisZoneActive(_hCharacter, _ucZoneType, (unsigned char) lI) )
        {
            if ( (bFoundOne = fn_vGetZoneCenter(hGeoObj, _p_stReturnVector)) != 0 )
            {
                if ( _bInGlobalCoordinates )
                {
                    POS_fn_vMulMatrixVertex(_p_stReturnVector,HIE_fn_hGetSuperObjectGlobalMatrix(_hCharacter),_p_stReturnVector);
                }
                else
                {
                    MTH3D_tdstMatrix stScaleMatrix;
                    POS_tdstCompletePosition *hGlobalMatrix = HIE_fn_hGetSuperObjectGlobalMatrix(_hCharacter);

                    if ( !POS_fn_ulIsNotScaledMatrix(hGlobalMatrix) )
                    {
                        POS_fn_vGetScaleMatrix(
                            hGlobalMatrix,
                            &stScaleMatrix.stCol_0,
                            &stScaleMatrix.stCol_1,
                            &stScaleMatrix.stCol_2
                        );
                        MTH3D_M_vMulMatrixVector(_p_stReturnVector, &stScaleMatrix, _p_stReturnVector);
                    }
                }
                break;
            }
        }
    }

    if ( !bFoundOne )
    {
        void *hPhysicalCollideSet;
        CHN_tdxHandleToChannel hChannel;

        HIE_tdstSuperObject *hModule;

        hChannel = fn_h3dDataGetFirstActiveChannel(M_GetMSHandle(_hCharacter,3dData));
        while (hChannel)
        {
            hModule = hChannel->hSupObject;
            if
            (
                hModule
                && ( (HIE_fn_ulGetSuperObjectType(hModule) & (HIE_C_ulPO | HIE_C_ulPO_Mirror)) )

                && ( ! ( HIE_M_xGetSuperObjectMember(hModule, ulFlags) & HIE_C_Flag_ulHidden ) )
            )
            {
                hPhysicalCollideSet = PO_fn_hGetCollideSet( (void *)HIE_fn_hGetSuperObjectObject(hModule) );
                switch ( _ucZoneType )
                {
                case C_ucTypeZde:
                    hGeoObj = PCS_fn_hGetZdeGeoObjOfPhysicalCollSet(hPhysicalCollideSet);
                    break;

#if !defined(_AI_EXCLUDE_NEVER_USED_)
                case C_ucTypeZdd:
                    hGeoObj = PCS_fn_hGetZddGeoObjOfPhysicalCollSet(hPhysicalCollideSet);
                    break;

                case C_ucTypeZdr:
                    hGeoObj = PCS_fn_hGetZdrGeoObjOfPhysicalCollSet(hPhysicalCollideSet);
                    break;

                case C_ucTypeZdm:
                    hGeoObj = PCS_fn_hGetZdmGeoObjOfPhysicalCollSet(hPhysicalCollideSet);
                    break;
#endif

                default:
                    hGeoObj = NULL;
                    break;
                }
                if ( hGeoObj && fn_hIsTypeExistsInGeoObj(hGeoObj, _xMask) && ( (bFoundOne = fn_vGetZoneCenter(hGeoObj, _p_stReturnVector)) != 0) )
                {
                    POS_tdstCompletePosition stGlobToPerso;

                    POS_fn_vMulMatrixVertex(_p_stReturnVector,HIE_fn_hGetSuperObjectGlobalMatrix(hModule),_p_stReturnVector);
                    if ( !_bInGlobalCoordinates )
                    {
                        POS_fn_vInvertMatrix(&stGlobToPerso,HIE_fn_hGetSuperObjectGlobalMatrix(_hCharacter));
                        POS_fn_vMulMatrixVertex(_p_stReturnVector,&stGlobToPerso,_p_stReturnVector);
                    }

                    break;
                }
            }
            hChannel = hChannel->p_stNextActiveChannel;
        }
    }
}

#define AI_M_pfn_GAM_hGetSuperObjectFromStdGame_fr2 fn_h_StandardGameGetSuperObject

/* 0x004782C0 */
AI_tdstNodeInterpret *__cdecl fn_p_stGetZDxCenter(
    HIE_tdstSuperObject *p_SuperObjPerso, AI_tdstNodeInterpret *p_stTree, AI_tdstGetSetParam *p_stValue)
{
    long                  eFuncId = AI_M_eFuncIdInterpret(p_stTree - 1);
    AI_tdstNodeInterpret *p_stNode;
    void                 *hCharacter, *engineObj;
    GMT_tdxMask           xMask;
    unsigned char         ucZoneType;

    p_stNode   = fn_p_stEvalTree(p_SuperObjPerso, p_stTree, p_stValue);
    engineObj  = p_stValue->uParam.p_stEngObjValue;
    hCharacter = AI_M_pfn_GAM_hGetSuperObjectFromStdGame_fr2(((struct tdstEngineObject_ *)engineObj)->h_StandardGame);

    p_stNode = fn_p_stEvalTree(p_SuperObjPerso, p_stNode, p_stValue);
    xMask    = (unsigned short)p_stValue->uParam.ulValue;

    p_stValue->uParam.a_xVectorValue[0] = 0.0f;
    p_stValue->uParam.a_xVectorValue[1] = 0.0f;
    p_stValue->uParam.a_xVectorValue[2] = 0.0f;

    switch (eFuncId)
    {
    case 0x9A: ucZoneType = 2; break;
    case 0x9B: ucZoneType = 1; break;
    case 0x9C: ucZoneType = 3; break;
    case 0x9D: ucZoneType = 0; break;
    default:   ucZoneType = 0xff; break;
    }

    p_stValue->eType = 0x15;
    fn_vInternGetZDxCenter(hCharacter, xMask, ucZoneType, 0,
                           (MTH3D_tdstVector *)p_stValue->uParam.a_xVectorValue);
    return p_stNode;
}

#define AI_M_pfn_POS_vMulMatrixVertex   ((void (__cdecl *)(float *, void *, float *))POS_fn_vMulMatrixVertex)

#define AI_FR2_xDegToRad     (0.017453292f)   /* 0x49cccc */
#define AI_FR2_xHalf         (0.5f)           /* 0x49cc9c */
#define AI_FR2_xPiBy2        (1.5707964f)     /* 0x49ccb4 */
#define AI_FR2_dColinearEps  (0.002)          /* 0x49ccd0 */
#define AI_FR2_xInfinitPlus  (3.4028220e38f)  /* 0x49ccbc */
#define AI_FR2_xInfinitMinus (-1.70141102e38f) /* 0x49ccc0 */
#define AI_FR2_xBigNeg       (-3.40282205e38f) /* 0x49ccb8 */

#include "SCT/SectExt.h"

/* 0x00478380 */
HIE_tdstSuperObject *fn_hGetNearestActorInSectorWithAnyCustombit(
  HIE_tdstSuperObject *_hMe,
  HIE_tdstSuperObject *_hSector,
  unsigned long _ulFilterCustomBits
)
{
	SECT_tdxHandleOfElementLstCharacter hCharacterElement;
	HIE_tdstSuperObject			*hCharacterSO=NULL;
	HIE_tdstSuperObject			*hCloserSO=NULL;
	MTH_tdxReal							xBestDist= 3.4028220466e38f;
	int									i;
	MTH_tdxReal							xTmp;
	MTH3D_tdstVector					stMyPosition,stActorsPosition;

	POS_fn_vGetTranslationVector( HIE_fn_hGetSuperObjectGlobalMatrix( _hMe), &stMyPosition );

	SECT_M_ForEachCharListInSector(_hSector, hCharacterElement, i)
	{
		hCharacterSO = SECT_GetCharacterInList( hCharacterElement );

		if ( hCharacterSO != _hMe )
		{

			if ( (fn_ulStandardGameGetCustomBits(M_GetMSHandle(hCharacterSO,StandardGame)) & _ulFilterCustomBits) != 0 )
			{

				POS_fn_vGetTranslationVector( HIE_fn_hGetSuperObjectGlobalMatrix( hCharacterSO ), &stActorsPosition );

				xTmp = MTH3D_M_xVectorGapSqr(&stActorsPosition, &stMyPosition);
				if ( MTH_M_bLess(xTmp, xBestDist) )
				{
					xBestDist = xTmp;
					hCloserSO = hCharacterSO;
				}
			}
		}
	}
  return hCloserSO;
}

/* 0x00478450 */
AI_tdstNodeInterpret *__cdecl fn_p_stGetNearestActorInCurrentSector(
    HIE_tdstSuperObject *p_SuperObjPerso, AI_tdstNodeInterpret *p_stTree, AI_tdstGetSetParam *p_stValue)
{
    unsigned long         eFuncId = *(unsigned long *)((char *)p_stTree - 8);
    HIE_tdstSuperObject  *hNewSuperObjPerso;
    AI_tdstGetSetParam    stParam;
    AI_tdstNodeInterpret *p_stNext;
    unsigned long         ulCustomBitValue;
    void                 *hSector;
    int                  *hCloserSO;
    int                  *hVal;

    fn_vGetUltraOperatorPerso(
        AI_M_pfn_fn_ucGetFunctionUltraOperator2(eFuncId), p_SuperObjPerso, &hNewSuperObjPerso);

    p_stNext = fn_p_stEvalTree(p_SuperObjPerso, p_stTree, &stParam);
    ulCustomBitValue = (stParam.eType == 8) ? (unsigned long)(long)stParam.uParam.xValue
                                            : (unsigned long)stParam.uParam.lValue;

    hSector  = AI_M_pfn_fn_h_SectInfoGetCurrentSector(((struct tdstEngineObject_ *)(HIE_fn_hGetSuperObjectObject(hNewSuperObjPerso)))->h_SectInfo);
    hCloserSO = (int *)fn_hGetNearestActorInSectorWithAnyCustombit(hNewSuperObjPerso, hSector, ulCustomBitValue);

    hVal = hCloserSO;
    if (hVal != (int *)0)
    {
        if (*hVal != 2) hVal = (int *)0;
        else hVal = (int *)hVal[1];
    }
    p_stValue->uParam.lValue = (long)hVal;
    p_stValue->eType = 0x0b;
    return p_stNext;
}

#define AI_FR2_vCross(r, a, b) do {                  \
    float _cx = (a)[1] * (b)[2] - (a)[2] * (b)[1];   \
    float _cy = (a)[2] * (b)[0] - (a)[0] * (b)[2];   \
    float _cz = (a)[0] * (b)[1] - (a)[1] * (b)[0];   \
    (r)[0] = _cx; (r)[1] = _cy; (r)[2] = _cz;        \
} while (0)

/* 0x476300 */
static HIE_tdstSuperObject *AI_fn_h_GetClosestPersoInSector(
    HIE_tdstSuperObject *_hCloserSO, MTH_tdxReal *_p_xBestDist, MTH_tdxReal *_p_xPrevWeight,
    HIE_tdstSuperObject *_hMe, MTH3D_tdstVector *_p_stMyVector, MTH3D_tdstVector *_p_stActorSighting,
    POS_tdstCompletePosition *_p_stGlobToPerso, HIE_tdstSuperObject *_hSector,
    unsigned long _ulFilterCustomBits, MTH_tdxReal _xXWeight, MTH_tdxReal _x2DWeight,
    MTH_tdxReal _x3DWeight, GMT_tdxMask _xMask, MTH_tdxReal _xCosXoY, MTH_tdxReal _xSinYoZ,
    long _eFuncId)
{
	SECT_tdxHandleOfElementLstCharacter hCharacterElement;
	HIE_tdstSuperObject *hCharacterSO;
	int j;
#if !defined(_AI_EXCLUDE_NEVER_USED_)
  MTH_tdxReal xTmp;
#endif
  MTH3D_tdstVector stVector;
  MS_tdxHandleToStandardGame hStdGame;

	SECT_M_ForEachCharListInSector(_hSector, hCharacterElement, j)
	{
		hCharacterSO = SECT_GetCharacterInList( hCharacterElement );

		if ( hCharacterSO != _hMe )
		{

			hStdGame = M_GetMSHandle(hCharacterSO,StandardGame);

			if( hStdGame &&
				!fn_bf1StandardGameGetIsDesactivateAtAll(hStdGame) &&
				( (fn_ulStandardGameGetCustomBits(hStdGame) & _ulFilterCustomBits) == _ulFilterCustomBits ) )
			{

				POS_fn_vGetTranslationVector( HIE_fn_hGetSuperObjectGlobalMatrix( hCharacterSO ), &stVector );
				if ( _xMask )
					fn_vInternGetZDxCenter(hCharacterSO, _xMask, C_ucTypeZde, TRUE, &stVector);

#if !defined(_AI_EXCLUDE_NEVER_USED_)

				if ( _eFuncId == 0x6c   )
		        {

					xTmp = MTH3D_M_xVectorGap(&stVector, _p_stMyVector);
					if ( MTH_M_bLess(xTmp, *_p_xBestDist) )
					{
						*_p_xBestDist = xTmp;
						_hCloserSO = hCharacterSO;
					}
				}

				else if ( _eFuncId == 0x6e   )
				{
					MTH3D_M_vSubVector ( &stVector, _p_stMyVector, &stVector );
					MTH3D_M_vNormalizeVector ( &stVector, &stVector );
					xTmp = MTH3D_M_xDotProductVector ( _p_stActorSighting, &stVector );
					if ( MTH_M_bGreater(xTmp, MTH_C_Inv2) )
					{
						xTmp = MTH3D_M_xVectorGap(&stVector, _p_stMyVector);
						if ( MTH_M_bLess(xTmp,*_p_xBestDist) )
						{
							*_p_xBestDist = xTmp;
							_hCloserSO = hCharacterSO;
						}
					}
				}
				else
#endif

				{
					MTH3D_tdstVector stTargetPosition;
					MTH_tdxReal	xNorm ,xX, xY, xZ, xWeight = MTH_C_ZERO;
					MTH_tdxReal	xNormXoY, xAngleYoZ ;

					POS_fn_vMulMatrixVertex(&stTargetPosition,_p_stGlobToPerso,&stVector);
					MTH3D_M_vGetVectorElements(&xX,&xY,&xZ,&stTargetPosition);

					stTargetPosition.xZ = MTH_C_ZERO;
					xNormXoY = MTH3D_M_xNormVector(&stTargetPosition);
					stTargetPosition.xZ = xZ;

					if ( xNormXoY )
						xAngleYoZ = MTH_M_xATan ( MTH_M_xDiv ( xZ , xNormXoY ) ) ;
					else
						xAngleYoZ = MTH_C_PiBy2 ;

					if
					(
						MTH_M_bGreater ( MTH_M_xNeg ( xY ) , MTH_M_xMul ( _xCosXoY , xNormXoY ) )
						&& MTH_M_bLess ( MTH_M_xSin ( xAngleYoZ ), _xSinYoZ )
					)
					{

						if ( MTH_M_bGreaterEqualZero ( xY ) )
						{
							if ( MTH_M_bEqualZero(xX) )
							{

								xWeight = MTH_C_InfinitMinus ;
							}
							else
							{

								xWeight = MTH_M_xNeg ( MTH_M_xAbs ( MTH_M_xDiv ( xY , xX ) ) ) ;
							}
						}
						else
						{

							if ( MTH_M_bEqualZero(xX) )
							{
								xWeight = MTH_C_InfinitPlus;
							}
							else
							{
  								xX = MTH_M_xAbs(xX);
								xNorm = MTH3D_M_xNormVector(&stTargetPosition);

								xWeight = MTH_M_xDiv(_xXWeight, xX);
								xWeight = MTH_M_xAdd(xWeight, MTH_M_xDiv(_x2DWeight, MTH_M_xSqrt(MTH_M_xAdd(MTH_M_xSqr(xX),MTH_M_xSqr(xY)))));
								xWeight = MTH_M_xAdd(xWeight, MTH_M_xDiv(_x3DWeight, xNorm));
							}
						}

						if ( MTH_M_bGreater(xWeight,*_p_xPrevWeight) )
						{
							*_p_xPrevWeight = xWeight;
							_hCloserSO = hCharacterSO;
						}
					}
				}
			}
		}
	}
	return _hCloserSO ;
}

#define SECT_M_ForEachGraphicNodeInGraphicInteractionList(_hSector,_hGraList,_i) \
for \
( \
	_i=SECT_fn_iGetNumberOfNodesInGraphicInteractionList(_hSector), \
	(_hGraList)=SECT_fn_hGetFirstGraphicNodeInGraphicList(_hSector); \
	_i>0;  \
	_i--,(_hGraList)=SECT_fn_hGetNextGraphicNodeInGraphicList(_hGraList) \
)

static __inline int SECT_fn_iGetNumberOfNodesInGraphicInteractionList(HIE_tdstSuperObject *hSector)
{
	return LST2_M_StaticGetNumberOfElements(&((SECT_tdxHandleOfSectorObject)HIE_fn_hGetSuperObjectObject(hSector))->stListOfSectorsInGraphicInteraction);
}

static __inline SECT_tdxHandleOfElementLstGraphicInteraction SECT_fn_hGetFirstGraphicNodeInGraphicList(HIE_tdstSuperObject *_hSector)
{
  return(LST2_M_StaticGetFirstElement( &((SECT_tdxHandleOfSectorObject)HIE_fn_hGetSuperObjectObject(_hSector))->stListOfSectorsInGraphicInteraction));
}

static __inline SECT_tdxHandleOfElementLstGraphicInteraction SECT_fn_hGetNextGraphicNodeInGraphicList(SECT_tdxHandleOfElementLstGraphicInteraction _GraphicNode)
{
  return(LST2_M_StaticGetNextElement( _GraphicNode));
}

static __inline HIE_tdstSuperObject *SECT_GetSectorInGraphicList(SECT_tdxHandleOfElementLstGraphicInteraction hGraphicList)
{
  return(hGraphicList->hPointerOfSectorSO);
}

/* 0x00476960 */
AI_tdstNodeInterpret *__cdecl fn_p_stCode4PersoLePlusProche(
    HIE_tdstSuperObject *p_SuperObjPerso, AI_tdstNodeInterpret *p_stTree, AI_tdstGetSetParam *p_stValue)
{
    AI_tdstGetSetParam    stParam;
    HIE_tdstSuperObject  *hCurrentSectorPerso;
    HIE_tdstSuperObject  *hGraphicSectorSO;
    int                  *hCloserSO = (int *)0;
    SECT_tdxHandleOfElementLstGraphicInteraction hGraphicElement;
    MTH3D_tdstVector      stMyVector;
    POS_tdstCompletePosition stGlobToPerso;
    float                 xBestDist   = AI_FR2_xInfinitPlus;
    unsigned long         ulCustomBitValue;
    float                 xXWeight = 0.0f, x2DWeight = 0.0f, x3DWeight = 0.0f;
    float                 xPrevWeight = AI_FR2_xInfinitMinus;
    MTH3D_tdstVector      stDirection;
    MTH3D_tdstVector     *p_stActorSighting;
    MTH3D_tdstVector     *p_stTmpVector1, *p_stTmpVector2;
    unsigned long         xMask = 0;
    float                 xCosXoY = 0.8944f, xSinYoZ = 0.4473f;
    unsigned long         eFuncId = AI_M_eFuncIdInterpret(p_stTree - 1);
    int                   i;
    HIE_tdstSuperObject  *hNewSuperObjPerso;

    fn_vGetUltraOperatorPerso(
        AI_M_pfn_fn_ucGetFunctionUltraOperator2(eFuncId), p_SuperObjPerso, &hNewSuperObjPerso);

    M_EvalNextParameter(&stParam);

    ulCustomBitValue = M_ReturnParam_lValue(&stParam);

    p_stActorSighting = NULL;

    switch (eFuncId)
    {
    case 0x71:
    case 0x70:

        M_EvalNextParameter(&stParam);
        xXWeight = M_ReturnParam_xValue(&stParam);

        M_EvalNextParameter(&stParam);
        x2DWeight = M_ReturnParam_xValue(&stParam);

        M_EvalNextParameter(&stParam);
        x3DWeight = M_ReturnParam_xValue(&stParam);

        M_EvalNextParameter(&stParam);
        stDirection = M_GetSetParam_stVectorValue(&stParam);
        xMask = (unsigned long) 0x0001;

        if (0x71 == eFuncId)
        {
            M_EvalNextParameter(&stParam);
            xCosXoY = M_ReturnParam_xValue(&stParam);
            xCosXoY = MTH_M_xCos(MTH_M_xDegToRad(xCosXoY));

            M_EvalNextParameter(&stParam);
            xSinYoZ = M_ReturnParam_xValue(&stParam);
            xSinYoZ = MTH_M_xSin(MTH_M_xDegToRad(xSinYoZ));
        }

    case 0x6c:
    case 0x6e:
        {

            hCurrentSectorPerso = AI_M_pfn_fn_h_SectInfoGetCurrentSector(
                ((struct tdstEngineObject_ *)(HIE_fn_hGetSuperObjectObject(hNewSuperObjPerso)))->h_SectInfo);

            if (eFuncId == 0x6c)
            {

                POS_fn_vGetTranslationVector( HIE_fn_hGetSuperObjectGlobalMatrix( hNewSuperObjPerso), &stMyVector );
            }

            else if (eFuncId == 0x6e)
            {
                POS_tdxHandleToPosition hActorMatrix = HIE_fn_hGetSuperObjectGlobalMatrix(hNewSuperObjPerso);
                POS_fn_vGetTranslationVector( hActorMatrix, &stMyVector );
                POS_fn_vGetRotationVector ( hActorMatrix, &p_stTmpVector1, &p_stActorSighting, &p_stTmpVector2 );
            }

            else
            {
                MTH3D_tdstVector vI,vJ,vK;
                POS_tdstCompletePosition stGlobPerso;

                POS_fn_vCopyMatrix(&stGlobPerso, HIE_fn_hGetSuperObjectGlobalMatrix(hNewSuperObjPerso));

                if (!MTH3D_M_bIsNullVector(&stDirection)) {
                    POS_fn_vGetRotationMatrix(&stGlobPerso,&vI,&vJ,&vK);

                    MTH3D_M_vNegVector(&stDirection, &stDirection);
                    MTH3D_M_vNormalizeVector(&vJ, &stDirection);

                    if (MTH_M_bEqualWithEpsilon(MTH3D_M_xDotProductVector(&vJ, &vK), MTH_C_ONE, AI_FR2_dColinearEps)) {

                        MTH3D_M_vCrossProductVectorWithoutBuffer(&vK, &vI, &vJ);

                        MTH3D_M_vCrossProductVectorWithoutBuffer(&vI, &vJ, &vK);
                    }

                    else {

                        MTH3D_M_vCrossProductVectorWithoutBuffer(&vI, &vJ, &vK);

                        MTH3D_M_vCrossProductVectorWithoutBuffer(&vK, &vI, &vJ);
                    }

                    POS_fn_vSetRotationMatrix(&stGlobPerso,&vI,&vJ,&vK);
                }

                POS_fn_vInvertIsoMatrix(&stGlobToPerso,&stGlobPerso);
            }

            hCloserSO = (int *)AI_fn_h_GetClosestPersoInSector(
                (HIE_tdstSuperObject *)hCloserSO, &xBestDist, &xPrevWeight, hNewSuperObjPerso, &stMyVector,
                p_stActorSighting, &stGlobToPerso, hCurrentSectorPerso, ulCustomBitValue,
                xXWeight, x2DWeight, x3DWeight, xMask, xCosXoY, xSinYoZ, eFuncId);

            SECT_M_ForEachGraphicNodeInGraphicInteractionList(hCurrentSectorPerso, hGraphicElement, i)
            {
                hGraphicSectorSO = SECT_GetSectorInGraphicList( hGraphicElement );

                hCloserSO = (int *)AI_fn_h_GetClosestPersoInSector(
                    (HIE_tdstSuperObject *)hCloserSO, &xBestDist, &xPrevWeight, hNewSuperObjPerso, &stMyVector,
                    p_stActorSighting, &stGlobToPerso, hGraphicSectorSO, ulCustomBitValue,
                    xXWeight, x2DWeight, x3DWeight, xMask, xCosXoY, xSinYoZ, eFuncId);
            }
        }

        M_Full_GetSetParam_Perso(p_stValue, (HIE_tdstSuperObject *)hCloserSO);
        return(p_stTree);

    default:
        break;
    }

    AI_M_pfn_M_AIFatalError_fr2(0x27);
    return(p_stTree);
}

extern unsigned char TFMT_g_stInfoTextAffiche_storage[];   /* 0x004FFDC0 */

typedef struct AI_TA_tdstInfoTextAffiche_
{
    long            bFirstInit;
    unsigned long   ulCurrentDelay;
    unsigned long   ulTimer;
    long            lEvent;
    short           wCurrentLetterInString;
    short           wPad;
} AI_TA_tdstInfoTextAffiche;
#define AI_TA_BASE (TFMT_g_stInfoTextAffiche_storage)
#define AI_TA_g_stInfoTextAffiche ((AI_TA_tdstInfoTextAffiche *)AI_TA_BASE)
#define AI_TA_ucFirstInit(n)    (AI_TA_g_stInfoTextAffiche[n].bFirstInit)
#define AI_TA_ulCurrentDelay(n) (AI_TA_g_stInfoTextAffiche[n].ulCurrentDelay)
#define AI_TA_ulTimer(n)        (AI_TA_g_stInfoTextAffiche[n].ulTimer)
#define AI_TA_lEvent(n)         (AI_TA_g_stInfoTextAffiche[n].lEvent)
#define AI_TA_wPosInString(n)   (AI_TA_g_stInfoTextAffiche[n].wCurrentLetterInString)

extern HIE_tdstSuperObject *TFMT_gp_SuperObjPersoGenerated_storage[];   /* 0x004FF8C0 */
#define AI_TA_hGenerated(n)     (TFMT_gp_SuperObjPersoGenerated_storage[(n)])

#define AI_TA_ulCurrentTimerCount GAM_g_ulAccumulatedTime  /* 0x5003EC */

extern char		TEXT_gc_ViewportFixed;

#include "GLD/GLD_Pub.h"

#define AI_TA_xInv640  (0.0015625f)     /* 0x49cca4 */
#define AI_TA_xInv480  (0.0020833334f)  /* 0x49cca8 */
#define AI_TA_xInv1000 (0.001f)         /* 0x49ccac */
#define AI_TA_xHalf    (0.5f)           /* 0x49cc9c */

void __cdecl TFMT_vUpdateSpacingTable(char *szText);
void __cdecl TFMT_vUpdateScaleTable(char *szText);
unsigned char __cdecl TFMT_ucNumberOfCurrentTextAffiche(HIE_tdstSuperObject *p_SuperObjPersoTreated);

extern void *__cdecl fn_p_stDynamGetDynamics(void *h_Dynam);
#define AI_M_pfn_TFMT_vUpdateSpacingTable         TFMT_vUpdateSpacingTable
#define AI_M_pfn_TFMT_vUpdateScaleTable            TFMT_vUpdateScaleTable

#define AI_M_pfn_fn_p_stSkipThisArgument           fn_p_stSkipThisArgument
#define AI_M_pfn_TFMT_ucNumberOfCurrentTextAffiche TFMT_ucNumberOfCurrentTextAffiche
/* 0x0040bc80 */
extern unsigned char __cdecl fn_ucIsAnAlwaysModel(void *p_stEngineObject);
#define AI_M_pfn_fn_ucIsAnAlwaysModel              fn_ucIsAnAlwaysModel

#define M_ObjectGetPersonalType(p_stObject) ((p_stObject)->h_StandardGame->otObjectPersonalType)

#define M_ObjectGetModelType(p_stObject) ((p_stObject)->h_StandardGame->otObjectModelType)
/* 0x40bcc0 */
extern void *__cdecl fn_p_stAllocateAlways(int otObjectModelType, void *p_stFather, void *_hGenerator, unsigned short uwAction, void *p_stMatrix);
#define AI_M_pfn_fn_p_stAllocateAlways             ((void *(__cdecl *)(int, void *, void *, int, void *))&fn_p_stAllocateAlways)
/* 0x00409A50 */
extern unsigned char __cdecl TFMT_ucSetNumber4CurrentTextAffiche(void);
#define AI_M_pfn_TFMT_ucSetNumber4CurrentTextAffiche TFMT_ucSetNumber4CurrentTextAffiche
/* 0x00409720 */
unsigned long TFMT_wPositionModulesAccordingToText2(void *p_stStatus, void *hSuperObjPerso, char *pszText,
                                                    long lStart, long lEnd, unsigned char *bFirstTimeZ,
                                                    short *pwCurrentChar, unsigned long *ulDelay);
#define AI_M_pfn_TFMT_wPositionModulesAccordingToText2 TFMT_wPositionModulesAccordingToText2
/* 0x00408F20 */
extern void __cdecl TEXT_vComputeLengthOgString(char *p_szString, void *p_stStatus);
#define AI_M_pfn_TEXT_vComputeLengthOgString       ((void (__cdecl *)(char *, void *))TEXT_vComputeLengthOgString)
/* 0x432da0 */
extern void __cdecl MEC_vInitTranslation(void *p_stDynamic, void *hSupObj, void *p_stVector);
#define AI_M_pfn_MEC_vInitTranslation              ((void (__cdecl *)(void *, void *, float *))&MEC_vInitTranslation)
#define AI_M_pfn_PLA_fn_vRefreshGlobalMatrixUnderCharacter PLA_fn_vRefreshGlobalMatrixUnderCharacter

#define AI_M_pfn_HIE_fn_SO_vSetSuperimposedFlag    HIE_fn_SO_vSetSuperimposedFlag
#define AI_M_pfn_HIE_fn_SO_vClearSuperimposedFlag  HIE_fn_SO_vClearSuperimposedFlag
#define AI_M_pfn_HIE_fn_vComputeNewRelativeMatrix  HIE_fn_vComputeNewRelativeMatrix
#define AI_M_pfn_GLI_vGet3DVertexFromScreenPos     GLI_vGet3DVertexFromScreenPos

typedef struct AI_TA_tdstStatus_ {
    float xScreenWidth;
    char  pad[0x2c - 4];
    char  cJustificationMode;
    char  pad2[3];
} AI_TA_tdstStatus;

static void *AI_fr2_p_stGetMindOfSuperObj(HIE_tdstSuperObject *p_stPerso)
{
    return AI_M_stGetMindOfSuperObj(p_stPerso);
}

/* 0x00475680 */
AI_tdstNodeInterpret *__cdecl fn_p_stCode4TextAffiche(
    HIE_tdstSuperObject *p_SuperObjPerso, AI_tdstNodeInterpret *p_stTree, AI_tdstGetSetParam *p_stValue)
{
    AI_tdstGetSetParam    stParam;
    long                  lEvent = 0;
    struct tdstEngineObject_ *p_stEngineObject = NULL;
    HIE_tdstSuperObject  *p_SuperObjPersoGenerated = NULL, *p_SuperObjPersoTreated = NULL;
    void                 *h_Dynam = NULL;
    POS_tdstCompletePosition *hPersoGlobalMatrix = NULL;
    AI_TA_tdstStatus      stStatus;
    MTH3D_tdstVector      stVertex, stTextPosition;
    MTH_tdxReal           xPosX = MTH_C_ZERO, xPosY = MTH_C_ZERO, xPosZ = MTH_C_ZERO;
    long                  lPosX, lPosY, lPosZ;
    long                  lStart = 0, lEnd = -1;
    MTH_tdxReal           xxPosX, xxPosY, xxPosZ;
    unsigned long         ulTimer = 0, ulDeltaTimer = 0;
    unsigned long         ulDelay;
    char                 *p_szString = NULL, szCopy[600];
    char                  asz_Num[20];
    unsigned char         ucPersoId, ucNumberOfCurrentTextAffiche = 0, ucDelayId;
    short                 wPositionInString = 0;
    unsigned char         bFirstTime = TRUE;

    ucPersoId = (unsigned char) M_ucVarIdInterpret(p_stTree);
    M_EvalNextParameter(&stParam);
    p_SuperObjPersoTreated = M_GetSetParam_p_stEngineObjValue(&stParam) ? (HIE_tdstSuperObject *)M_GetSetParam_p_stSupObjValue(&stParam) : NULL;

    M_EvalNextParameter(&stParam);
    p_stEngineObject = M_GetSetParam_p_stEngineObjValue(&stParam);

    M_EvalNextParameter(&stParam);
    stTextPosition = M_GetSetParam_stVectorValue(&stParam);
    lPosX = (long)stTextPosition.xX;
    lPosY = (long)stTextPosition.xY;
    lPosZ = (long)stTextPosition.xZ;
    MTH3D_M_vGetVectorElements( &xxPosX, &xxPosY, &xxPosZ, &stTextPosition);

    M_EvalNextParameter(&stParam);
    p_szString = fn_szGetStringFromTextOrStringParam(&stParam);
    if (p_szString == NULL)
        p_szString = "No Text";

    if ( *p_szString == '/' && *(p_szString+1) == '-' )
    {
        char *szCur ;
        long i ;

        i = 0 ;
        szCur = p_szString + 2 ;
        switch ( *szCur )
        {
        case 'W' :
        case 'w' :
            AI_M_pfn_TFMT_vUpdateSpacingTable(szCur+1);
            break ;

        case 'Z' :
        case 'z' :
            AI_M_pfn_TFMT_vUpdateScaleTable(szCur+1);
            break ;
        }

        M_Full_GetSetParam_Integer(p_stValue, lEvent );
        return AI_M_pfn_fn_p_stSkipThisArgument(p_stTree) ;
    }

    strcpy(szCopy, p_szString);

    {
        char	*psz_Temp, *psz_Copy;
        int		i_Num;

        psz_Copy = szCopy;
        psz_Temp = p_szString;
        while(*psz_Temp)
        {
            if(psz_Temp[0] == '/')
            {
                if(psz_Temp[1] == '%')
                {
                    switch(psz_Temp[2])
                    {

                    case 'i':
                    case 'I':
                        i_Num = atoi(psz_Temp + 3);
                        _itoa(AI_TEXT_gai_IntParameters[i_Num], asz_Num, 10);
                        strcpy(psz_Copy, asz_Num);
                        psz_Copy += strlen(asz_Num);
                        while(*psz_Temp != ':')
                            psz_Temp++;
                        psz_Temp++;
                        continue;
                        break;

                    case 'f':
                    case 'F':
                        i_Num = atoi(psz_Temp + 3);
                        sprintf(asz_Num, "%.3f", AI_TEXT_gax_RealParameters[i_Num]);
                        strcpy(psz_Copy, asz_Num);
                        psz_Copy += strlen(asz_Num);
                        while(*psz_Temp != ':')
                            psz_Temp++;
                        psz_Temp++;
                        continue;
                        break;
                    }
                }
            }
            *psz_Copy++ = *psz_Temp++;
        }
        *psz_Copy = 0;
        p_szString = szCopy ;
    }

    ucDelayId = (unsigned char) M_ucVarIdInterpret(p_stTree);
    M_EvalNextParameter(&stParam);
    ulDelay = M_GetSetParam_ulValue(&stParam);

    if ( p_SuperObjPersoTreated != NULL )
    {

        ucNumberOfCurrentTextAffiche = AI_M_pfn_TFMT_ucNumberOfCurrentTextAffiche( p_SuperObjPersoTreated );

        wPositionInString = AI_TA_wPosInString(ucNumberOfCurrentTextAffiche);

        bFirstTime = AI_TA_ucFirstInit(ucNumberOfCurrentTextAffiche);
        lEvent = AI_TA_lEvent(ucNumberOfCurrentTextAffiche);

        if ( wPositionInString >= (short)strlen(p_szString) )
        {

            M_Full_GetSetParam_Integer(p_stValue, lEvent );
            return(p_stTree);
        }
    }

    if (p_SuperObjPersoTreated==NULL)
    {
        MTH3D_M_vFillVector( &stVertex, MTH_C_ONE );

        if (p_stEngineObject!= NULL)
        {
            if (fn_ucIsAnAlwaysModel(p_stEngineObject))
            {
                POS_tdstCompletePosition stMatrix;

                int otObjectModelType = M_ObjectGetModelType(p_stEngineObject);

                POS_fn_vSetIdentityMatrix(&stMatrix);
                POS_fn_vSetTranslationVector(&stMatrix, &stVertex);

                p_SuperObjPersoGenerated = (HIE_tdstSuperObject *)AI_M_pfn_fn_p_stAllocateAlways(otObjectModelType, gp_stDynamicWorld, p_SuperObjPerso, 0, &stMatrix);

                if ( p_SuperObjPersoGenerated != NULL )
                {
                    register unsigned long ulTmpDelay = ulDelay;

                    fn_v3dDataSetFlagEndState(M_GetMSHandle(p_SuperObjPersoGenerated,3dData),TRUE);

                    ucNumberOfCurrentTextAffiche = AI_M_pfn_TFMT_ucSetNumber4CurrentTextAffiche();

                    ulTimer = AI_TA_ulCurrentTimerCount;
                    AI_TA_ulTimer(ucNumberOfCurrentTextAffiche) = ulTimer;

                    AI_TA_ulCurrentDelay(ucNumberOfCurrentTextAffiche) = ulDelay;

                    AI_TA_hGenerated(ucNumberOfCurrentTextAffiche) = p_SuperObjPersoGenerated;

                    M_Full_GetSetParam_Perso(&stParam, p_SuperObjPersoGenerated);
                    ucPersoId = fn_ucSetDsgVar(ucPersoId, 0, AI_M_stGetMindOfSuperObj(p_SuperObjPerso), &stParam);

                    if ( !ulDelay )
                    {
                        wPositionInString = -1;
                        lEvent = AI_M_pfn_TFMT_wPositionModulesAccordingToText2(&stStatus, p_SuperObjPersoGenerated, p_szString, lStart, lEnd, &bFirstTime, &wPositionInString, &ulDelay);
                        wPositionInString = (short)strlen(p_szString);
                    }
                    else
                    {
                        lEvent = AI_M_pfn_TFMT_wPositionModulesAccordingToText2(&stStatus, p_SuperObjPersoGenerated, p_szString, lStart, lEnd, &bFirstTime, &wPositionInString, &ulDelay);
                    }

                    if( ulDelay != ulTmpDelay )
                    {
                        if((int) ulDelay < 0)
                            ulDelay = (unsigned long)(-((int)ulDelay));
                        else
                            AI_TA_ulCurrentDelay(ucNumberOfCurrentTextAffiche) = ulDelay;
                    }

                    AI_M_pfn_TEXT_vComputeLengthOgString(p_szString, &stStatus);

                    h_Dynam = ((struct tdstEngineObject_ *)(HIE_fn_hGetSuperObjectObject(p_SuperObjPersoGenerated)))->h_Dynam;

                    hPersoGlobalMatrix = (POS_tdstCompletePosition *)HIE_fn_hGetSuperObjectGlobalMatrix(p_SuperObjPersoGenerated);

                    if(TEXT_gc_ViewportFixed)
                    {
                        float xScaleW = (float)AI_TA_dwScreenWidth  * AI_TA_xInv640;
                        float xScaleH = (float)AI_TA_dwScreenHeight * AI_TA_xInv480;

                        AI_M_pfn_HIE_fn_SO_vSetSuperimposedFlag(p_SuperObjPersoGenerated);
                        xPosY = MTH_M_xDiv( lPosY, MTH_M_xConstFloatToReal(1000.0f)) ;
                        xPosZ = MTH_M_xDiv( lPosZ, MTH_M_xConstFloatToReal(1000.0f) );
                        if ( stStatus.cJustificationMode == 'c' )
                        {
                            xPosX = MTH_M_xDiv( MTH_M_xConstFloatToReal(500.0f), MTH_M_xConstFloatToReal(1000.0f) );
                            AI_M_pfn_GLI_vGet3DVertexFromScreenPos( AI_TA_p_stFixViewportAttr, &stVertex, xPosX, xPosY, xPosZ);

                            stVertex.xX = (stVertex.xX - stStatus.xScreenWidth * AI_TA_xHalf) * xScaleW;
                        }
                        else
                        {
                            xPosX = MTH_M_xDiv( lPosX, MTH_M_xConstFloatToReal(1000.0f) ) ;
                            AI_M_pfn_GLI_vGet3DVertexFromScreenPos( AI_TA_p_stFixViewportAttr, &stVertex, xPosX, xPosY, xPosZ);
                        }

                        hPersoGlobalMatrix->ulType = 4;
                        hPersoGlobalMatrix->stTransformMatrix.stCol_0.xX = xScaleW;
                        hPersoGlobalMatrix->stTransformMatrix.stCol_2.xZ = xScaleH;
                    }
                    else
                    {
                        AI_M_pfn_HIE_fn_SO_vClearSuperimposedFlag(p_SuperObjPersoGenerated);
                        stVertex.xX = xxPosX;
                        stVertex.xY = xxPosY;
                        stVertex.xZ = xxPosZ;
                    }

                    POS_fn_vSetTranslationVector(hPersoGlobalMatrix, &stVertex);
                    if ( h_Dynam )
                    {
                        void *p_stDynamics = fn_p_stDynamGetDynamics(h_Dynam);   /* 0x419B90 */
                        AI_M_pfn_MEC_vInitTranslation(p_stDynamics, p_SuperObjPersoGenerated, (float *)&stVertex);
                    }

                    AI_M_pfn_HIE_fn_vComputeNewRelativeMatrix(p_SuperObjPersoGenerated);

                    AI_M_pfn_PLA_fn_vRefreshGlobalMatrixUnderCharacter(p_SuperObjPersoGenerated);

                }
            }
        }
    }
    else
    {

        ulTimer      = AI_TA_ulTimer(ucNumberOfCurrentTextAffiche);
        ulDeltaTimer = AI_TA_ulCurrentTimerCount-ulTimer;

        p_SuperObjPersoGenerated = p_SuperObjPersoTreated;
        bFirstTime               = AI_TA_ucFirstInit(ucNumberOfCurrentTextAffiche);

        if ( ulDeltaTimer>=ulDelay)
        {
            unsigned long ulTmpDelay = AI_TA_ulCurrentDelay(ucNumberOfCurrentTextAffiche);
            ulDelay = ulTmpDelay;

            if (!ulDelay)
            {
                while( (wPositionInString<(short)strlen(p_szString)) && (!ulDelay) )
                {
                    lEvent = AI_M_pfn_TFMT_wPositionModulesAccordingToText2(&stStatus, p_SuperObjPersoGenerated, p_szString, lStart, lEnd, &bFirstTime, &wPositionInString, &ulDelay);
                }
            }
            else
            {
                lEvent = AI_M_pfn_TFMT_wPositionModulesAccordingToText2(&stStatus, p_SuperObjPersoGenerated, p_szString, lStart, lEnd, &bFirstTime, &wPositionInString, &ulDelay);
            }
            AI_TA_ulTimer(ucNumberOfCurrentTextAffiche) = AI_TA_ulCurrentTimerCount;

            if( ulDelay != ulTmpDelay )
            {
                if((int) ulDelay < 0)
                    ulDelay = (unsigned long)(-((int)ulDelay));
                else
                    AI_TA_ulCurrentDelay(ucNumberOfCurrentTextAffiche) = ulDelay;
            }
            AI_TA_ulTimer(ucNumberOfCurrentTextAffiche) = AI_TA_ulCurrentTimerCount;
        }
        else
        {
            if (lEvent)
            {
                register unsigned long  ulTmpDelay = AI_TA_ulCurrentDelay(ucNumberOfCurrentTextAffiche);
                unsigned long  ulCptDelay = ulTmpDelay;

                lEvent = AI_M_pfn_TFMT_wPositionModulesAccordingToText2(&stStatus, p_SuperObjPersoGenerated, p_szString, lStart, lEnd, &bFirstTime, &wPositionInString, &ulCptDelay);
                if( ulCptDelay != ulTmpDelay )
                {
                    if((int) ulCptDelay < 0)
                        ulCptDelay = (unsigned long)(-((int)ulCptDelay));
                    else
                        AI_TA_ulCurrentDelay(ucNumberOfCurrentTextAffiche) = ulCptDelay;
                    ulDelay = ulCptDelay;
                }
            }
        }

    }

    AI_TA_ucFirstInit(ucNumberOfCurrentTextAffiche)   = bFirstTime;
    AI_TA_lEvent(ucNumberOfCurrentTextAffiche)        = lEvent;
    AI_TA_wPosInString(ucNumberOfCurrentTextAffiche)  = wPositionInString;

    M_Full_GetSetParam_Integer( &stParam, ulDelay );
    ucDelayId = fn_ucSetDsgVar(ucDelayId, 0, AI_M_stGetMindOfSuperObj(p_SuperObjPerso), &stParam);

    M_Full_GetSetParam_Integer(p_stValue, lEvent);
    return(p_stTree);
}
