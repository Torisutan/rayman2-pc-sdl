#include <stddef.h>
#include "INT/Inters.h"
#include "MTH/MTH_flt.h"
#include "MTH/MTH_xNeg.h"
#include "MTH/MTH3D.h"
#include "COL/Inters.h"
#include "COL/Futil.h"

/* 0x00490160 */
ACP_tdxBool
INT_fn_bIntersectBoxWithBox(MTH3D_tdstVector *p_stMinPoint1,
                            MTH3D_tdstVector *p_stMaxPoint1,
                            MTH3D_tdstVector *p_stMinPoint2,
                            MTH3D_tdstVector *p_stMaxPoint2)
{
    return ( MTH_M_bLessEqual( MTH_M_xMax( MTH3D_M_xGetXofVector(p_stMinPoint1), MTH3D_M_xGetXofVector(p_stMinPoint2) ),
                               MTH_M_xMin( MTH3D_M_xGetXofVector(p_stMaxPoint1), MTH3D_M_xGetXofVector(p_stMaxPoint2) )
                             )
           ) &&
           ( MTH_M_bLessEqual( MTH_M_xMax( MTH3D_M_xGetYofVector(p_stMinPoint1), MTH3D_M_xGetYofVector(p_stMinPoint2) ),
                               MTH_M_xMin( MTH3D_M_xGetYofVector(p_stMaxPoint1), MTH3D_M_xGetYofVector(p_stMaxPoint2) )
                             )
           ) &&
           ( MTH_M_bLessEqual( MTH_M_xMax( MTH3D_M_xGetZofVector(p_stMinPoint1), MTH3D_M_xGetZofVector(p_stMinPoint2) ),
                               MTH_M_xMin( MTH3D_M_xGetZofVector(p_stMaxPoint1), MTH3D_M_xGetZofVector(p_stMaxPoint2) )
                             )
           );
}

/* 0x00490230 */
ACP_tdxBool
INT_fn_bIntersectSphereWithBox(MTH3D_tdstVector *p_stCenter,
                               MTH_tdxReal       xRadius,
                               MTH3D_tdstVector *p_stMinPoint,
                               MTH3D_tdstVector *p_stMaxPoint)
{
    MTH_tdxReal xDist;

    xDist = MTH_C_ZERO;

    if (p_stCenter->xX < p_stMinPoint->xX)
        xDist = MTH_M_xAdd(xDist, MTH_M_xSqr(MTH_M_xSub(p_stCenter->xX, p_stMinPoint->xX)));
    else if (p_stCenter->xX > p_stMaxPoint->xX)
        xDist = MTH_M_xAdd(xDist, MTH_M_xSqr(MTH_M_xSub(p_stCenter->xX, p_stMaxPoint->xX)));

    if (p_stCenter->xY < p_stMinPoint->xY)
        xDist = MTH_M_xAdd(xDist, MTH_M_xSqr(MTH_M_xSub(p_stCenter->xY, p_stMinPoint->xY)));
    else if (p_stCenter->xY > p_stMaxPoint->xY)
        xDist = MTH_M_xAdd(xDist, MTH_M_xSqr(MTH_M_xSub(p_stCenter->xY, p_stMaxPoint->xY)));

    if (p_stCenter->xZ < p_stMinPoint->xZ)
        xDist = MTH_M_xAdd(xDist, MTH_M_xSqr(MTH_M_xSub(p_stCenter->xZ, p_stMinPoint->xZ)));
    else if (p_stCenter->xZ > p_stMaxPoint->xZ)
        xDist = MTH_M_xAdd(xDist, MTH_M_xSqr(MTH_M_xSub(p_stCenter->xZ, p_stMaxPoint->xZ)));

    return (MTH_M_bLess(xDist, MTH_M_xSqr(xRadius)));
}

/* 0x004902F0 */
ACP_tdxBool
INT_fn_bIntersectSphereWithSphere(MTH3D_tdstVector *p_stCenter1,
                                  MTH_tdxReal       xRadius1,
                                  MTH3D_tdstVector *p_stCenter2,
                                  MTH_tdxReal       xRadius2)
{
    return MTH_M_bLessEqual( MTH3D_M_xVectorGapSqr(p_stCenter1, p_stCenter2),
                             MTH_M_xSqr(MTH_M_xAdd(xRadius1, xRadius2)) );
}

unsigned long
INT_fn_ulGetPositionPointWithBox(MTH3D_tdstVector *p_stPoint,
                                 MTH3D_tdstVector *p_stMinPoint,
                                 MTH3D_tdstVector *p_stMaxPoint)
{
    unsigned long ulOutCode = COL_C_ulNullMask;

    if (MTH_M_bLess(p_stPoint->xX,
                    p_stMinPoint->xX))
        ulOutCode |= COL_C_ulXMinMask;
    else if (MTH_M_bGreater(p_stPoint->xX,
                            p_stMaxPoint->xX))
        ulOutCode |= COL_C_ulXMaxMask;

    if (MTH_M_bLess(MTH3D_M_xGetYofVector(p_stPoint),
                    MTH3D_M_xGetYofVector(p_stMinPoint)))
        ulOutCode |= COL_C_ulYMinMask;
    else if (MTH_M_bGreater(MTH3D_M_xGetYofVector(p_stPoint),
                            MTH3D_M_xGetYofVector(p_stMaxPoint)))
        ulOutCode |= COL_C_ulYMaxMask;

    if (MTH_M_bLess(MTH3D_M_xGetZofVector(p_stPoint),
                    MTH3D_M_xGetZofVector(p_stMinPoint)))
        ulOutCode |= COL_C_ulZMinMask;
    else if (MTH_M_bGreater(MTH3D_M_xGetZofVector(p_stPoint),
                            MTH3D_M_xGetZofVector(p_stMaxPoint)))
        ulOutCode |= COL_C_ulZMaxMask;

    return ulOutCode;
}

/* 0x004903F0 */
static ACP_tdxBool
INT_fn_bInXYRange(MTH3D_tdstVector *p_stPoint,
                  MTH3D_tdstVector *p_stMinPoint,
                  MTH3D_tdstVector *p_stMaxPoint)
{
    if (MTH_M_bLess(MTH3D_M_xGetXofVector(p_stPoint), MTH3D_M_xGetXofVector(p_stMinPoint)))
        return FALSE;
    if (MTH_M_bGreater(MTH3D_M_xGetXofVector(p_stPoint), MTH3D_M_xGetXofVector(p_stMaxPoint)))
        return FALSE;
    if (MTH_M_bLess(MTH3D_M_xGetYofVector(p_stPoint), MTH3D_M_xGetYofVector(p_stMinPoint)))
        return FALSE;
    if (MTH_M_bGreater(MTH3D_M_xGetYofVector(p_stPoint), MTH3D_M_xGetYofVector(p_stMaxPoint)))
        return FALSE;

    return TRUE;
}

static ACP_tdxBool
INT_fn_bInYZRange(MTH3D_tdstVector *p_stPoint,
                  MTH3D_tdstVector *p_stMinPoint,
                  MTH3D_tdstVector *p_stMaxPoint)
{
    if (MTH_M_bLess(MTH3D_M_xGetYofVector(p_stPoint), MTH3D_M_xGetYofVector(p_stMinPoint)))
        return FALSE;
    if (MTH_M_bGreater(MTH3D_M_xGetYofVector(p_stPoint), MTH3D_M_xGetYofVector(p_stMaxPoint)))
        return FALSE;
    if (MTH_M_bLess(MTH3D_M_xGetZofVector(p_stPoint), MTH3D_M_xGetZofVector(p_stMinPoint)))
        return FALSE;
    if (MTH_M_bGreater(MTH3D_M_xGetZofVector(p_stPoint), MTH3D_M_xGetZofVector(p_stMaxPoint)))
        return FALSE;

    return TRUE;
}

/* 0x004904B0 */
static ACP_tdxBool
INT_fn_bInXZRange(MTH3D_tdstVector *p_stPoint,
                  MTH3D_tdstVector *p_stMinPoint,
                  MTH3D_tdstVector *p_stMaxPoint)
{
    if (MTH_M_bLess(MTH3D_M_xGetXofVector(p_stPoint), MTH3D_M_xGetXofVector(p_stMinPoint)))
        return FALSE;
    if (MTH_M_bGreater(MTH3D_M_xGetXofVector(p_stPoint), MTH3D_M_xGetXofVector(p_stMaxPoint)))
        return FALSE;
    if (MTH_M_bLess(MTH3D_M_xGetZofVector(p_stPoint), MTH3D_M_xGetZofVector(p_stMinPoint)))
        return FALSE;
    if (MTH_M_bGreater(MTH3D_M_xGetZofVector(p_stPoint), MTH3D_M_xGetZofVector(p_stMaxPoint)))
        return FALSE;

    return TRUE;
}

/* 0x00490510 */
ACP_tdxBool
INT_fn_bGetInclusionPointInBox(MTH3D_tdstVector *p_stPoint,
                               MTH3D_tdstVector *p_stMinPoint,
                               MTH3D_tdstVector *p_stMaxPoint)
{
    if (MTH_M_bLess(MTH3D_M_xGetXofVector(p_stPoint), MTH3D_M_xGetXofVector(p_stMinPoint)))
        return FALSE;
    if (MTH_M_bGreater(MTH3D_M_xGetXofVector(p_stPoint), MTH3D_M_xGetXofVector(p_stMaxPoint)))
        return FALSE;
    if (MTH_M_bLess(MTH3D_M_xGetYofVector(p_stPoint), MTH3D_M_xGetYofVector(p_stMinPoint)))
        return FALSE;
    if (MTH_M_bGreater(MTH3D_M_xGetYofVector(p_stPoint), MTH3D_M_xGetYofVector(p_stMaxPoint)))
        return FALSE;
    if (MTH_M_bLess(MTH3D_M_xGetZofVector(p_stPoint), MTH3D_M_xGetZofVector(p_stMinPoint)))
        return FALSE;
    if (MTH_M_bGreater(MTH3D_M_xGetZofVector(p_stPoint), MTH3D_M_xGetZofVector(p_stMaxPoint)))
        return FALSE;

    return TRUE;
}

/* 0x004915E0 */
ACP_tdxBool
INT_fn_bGetInclusionPointInSphere(MTH3D_tdstVector *p_stPoint,
                                  MTH3D_tdstVector *p_stSphereCenter,
                                  MTH_tdxReal       xSphereRadius)
{
    return MTH_M_bLessEqual(MTH3D_M_xVectorGapSqr(p_stPoint, p_stSphereCenter),
                            MTH_M_xSqr(xSphereRadius));
}

/* 0x00491650 */
ACP_tdxBool
INT_fn_bFastRejectIntersectSegmentWithSphere(MTH3D_tdstVector *p_stVertex1,
                                             MTH3D_tdstVector *p_stVect12,
                                             MTH3D_tdstVector *p_stSphereCenter,
                                             MTH_tdxReal       xSphereRadius)
{
    MTH3D_tdstVector stM;
    MTH3D_tdstVector stMP;
    MTH_tdxReal      xNegSphereRadius;

    MTH3D_M_vSubVector(&stM, p_stVertex1, p_stSphereCenter);
    MTH3D_M_vAddVector(&stMP, &stM, p_stVect12);
    xNegSphereRadius = MTH_M_xNeg(xSphereRadius);

    if (MTH_M_bGreaterZero(p_stVect12->xX))
    {
        if (MTH_M_bGreater(stM.xX, xSphereRadius))    return FALSE;
        if (MTH_M_bLess(stMP.xX, xNegSphereRadius))   return FALSE;
    }
    else
    {
        if (MTH_M_bLess(stM.xX, xNegSphereRadius))    return FALSE;
        if (MTH_M_bGreater(stMP.xX, xSphereRadius))   return FALSE;
    }

    if (MTH_M_bGreaterZero(p_stVect12->xY))
    {
        if (MTH_M_bGreater(stM.xY, xSphereRadius))    return FALSE;
        if (MTH_M_bLess(stMP.xY, xNegSphereRadius))   return FALSE;
    }
    else
    {
        if (MTH_M_bLess(stM.xY, xNegSphereRadius))    return FALSE;
        if (MTH_M_bGreater(stMP.xY, xSphereRadius))   return FALSE;
    }

    if (MTH_M_bGreaterZero(p_stVect12->xZ))
    {
        if (MTH_M_bGreater(stM.xZ, xSphereRadius))    return FALSE;
        if (MTH_M_bLess(stMP.xZ, xNegSphereRadius))   return FALSE;
    }
    else
    {
        if (MTH_M_bLess(stM.xZ, xNegSphereRadius))    return FALSE;
        if (MTH_M_bGreater(stMP.xZ, xSphereRadius))   return FALSE;
    }

    return TRUE;
}

/* 0x00491810 */
ACP_tdxBool
INT_fn_bIntersectSegmentWithSphere(MTH3D_tdstVector *p_stVertex1,
                                   MTH3D_tdstVector *p_stVect12,
                                   MTH3D_tdstVector *p_stSphereCenter,
                                   MTH_tdxReal       xSphereRadius,
                                   MTH_tdxReal      *p_xT,
                                   MTH3D_tdstVector *p_stHit,
                                   MTH3D_tdstVector *p_stNormal)
{
    MTH3D_tdstVector stVectS1;
    MTH_tdxReal      xA, xB, xC, xT1, xT2, xT, xInvR;

    if (!INT_fn_bFastRejectIntersectSegmentWithSphere(p_stVertex1, p_stVect12,
                                                      p_stSphereCenter, xSphereRadius))
        return FALSE;

    MTH3D_M_vSubVector(&stVectS1, p_stVertex1, p_stSphereCenter);
    xA = MTH3D_M_xSqrVector(p_stVect12);
    xB = MTH3D_M_xDotProductVector(p_stVect12, &stVectS1);
    xB = MTH_M_xAdd(xB, xB);
    xC = MTH_M_xSub(MTH3D_M_xSqrVector(&stVectS1), MTH_M_xSqr(xSphereRadius));

    switch (INT_fn_xTrinomeResolution(xA, xB, xC, &xT1, &xT2))
    {
    case INT_C_xTwoSolutions:
        xT = MTH_M_xMin(xT1, xT2);
        if (MTH_M_bGreaterEqual(xT, MTH_C_ZERO) && MTH_M_bLessEqual(xT, MTH_C_ONE))
        {
            *p_xT = xT;
            MTH3D_M_vMulAddVector(p_stHit, xT, p_stVect12, p_stVertex1);
            MTH3D_M_vSubVector(p_stNormal, p_stHit, p_stSphereCenter);
            xInvR = MTH_M_xInv(xSphereRadius);
            MTH3D_M_vMulScalarVector(p_stNormal, xInvR, p_stNormal);
            return TRUE;
        }
        return FALSE;

    case INT_C_xOneSolution:
        if (MTH_M_bGreaterEqual(xT1, MTH_C_ZERO) && MTH_M_bLessEqual(xT1, MTH_C_ONE))
        {
            *p_xT = xT1;
            MTH3D_M_vMulAddVector(p_stHit, xT1, p_stVect12, p_stVertex1);
            MTH3D_M_vSubVector(p_stNormal, p_stHit, p_stSphereCenter);
            xInvR = MTH_M_xInv(xSphereRadius);
            MTH3D_M_vMulScalarVector(p_stNormal, xInvR, p_stNormal);
            return TRUE;
        }
        break;

    default:
        break;
    }

    return FALSE;
}

/* 0x00491D50 */
ACP_tdxBool
INT_fn_bIntersectSegmentWithCylinder(MTH3D_tdstVector *p_stVertex1,
                                     MTH3D_tdstVector *p_stVect12,
                                     MTH3D_tdstVector *p_stVertexA,
                                     MTH3D_tdstVector *p_stVectAB,
                                     MTH_tdxReal       xCylinderRadius,
                                     MTH_tdxReal      *p_xTSegment,
                                     MTH3D_tdstVector *p_stHitSegment,
                                     MTH3D_tdstVector *p_stHitLine)
{
    MTH3D_tdstVector stVectA1;
    MTH3D_tdstVector stVectP;
    MTH3D_tdstVector stVectQ;
    MTH_tdxReal      xTLine, xK, xL, xM, xTmp;
    MTH_tdxReal      xA, xB, xC, xT1, xT2;
    MTH_tdxReal      xMin, xMax;
    ACP_tdxBool      bGreaterZero1, bGreaterZero2;

    MTH3D_M_vAddVector(&stVectP,  p_stVertex1, p_stVect12);
    MTH3D_M_vAddVector(&stVectA1, p_stVertexA, p_stVectAB);

    bGreaterZero1 = MTH_M_bGreaterZero(p_stVect12->xX);
    bGreaterZero2 = MTH_M_bGreaterZero(p_stVectAB->xX);
    xMin = bGreaterZero1 ? p_stVertex1->xX : stVectP.xX;
    xMax = bGreaterZero2 ? stVectA1.xX : p_stVertexA->xX;
    if (MTH_M_bLess(MTH_M_xAdd(xMax, xCylinderRadius), xMin)) return FALSE;
    xMax = bGreaterZero1 ? stVectP.xX : p_stVertex1->xX;
    xMin = bGreaterZero2 ? p_stVertexA->xX : stVectA1.xX;
    if (MTH_M_bLess(xMax, MTH_M_xSub(xMin, xCylinderRadius))) return FALSE;

    bGreaterZero1 = MTH_M_bGreaterZero(p_stVect12->xY);
    bGreaterZero2 = MTH_M_bGreaterZero(p_stVectAB->xY);
    xMin = bGreaterZero1 ? p_stVertex1->xY : stVectP.xY;
    xMax = bGreaterZero2 ? stVectA1.xY : p_stVertexA->xY;
    if (MTH_M_bLess(MTH_M_xAdd(xMax, xCylinderRadius), xMin)) return FALSE;
    xMax = bGreaterZero1 ? stVectP.xY : p_stVertex1->xY;
    xMin = bGreaterZero2 ? p_stVertexA->xY : stVectA1.xY;
    if (MTH_M_bLess(xMax, MTH_M_xSub(xMin, xCylinderRadius))) return FALSE;

    bGreaterZero1 = MTH_M_bGreaterZero(p_stVect12->xZ);
    bGreaterZero2 = MTH_M_bGreaterZero(p_stVectAB->xZ);
    xMin = bGreaterZero1 ? p_stVertex1->xZ : stVectP.xZ;
    xMax = bGreaterZero2 ? stVectA1.xZ : p_stVertexA->xZ;
    if (MTH_M_bLess(MTH_M_xAdd(xMax, xCylinderRadius), xMin)) return FALSE;
    xMax = bGreaterZero1 ? stVectP.xZ : p_stVertex1->xZ;
    xMin = bGreaterZero2 ? p_stVertexA->xZ : stVectA1.xZ;
    if (MTH_M_bLess(xMax, MTH_M_xSub(xMin, xCylinderRadius))) return FALSE;

    MTH3D_M_vSubVector(&stVectA1, p_stVertex1, p_stVertexA);

    xM = MTH3D_M_xSqrVector(p_stVectAB);
    if (MTH_M_bEqualZero(xM))
        return FALSE;
    xM = MTH_M_xInv(xM);

    xK = MTH3D_M_xDotProductVector(p_stVectAB, p_stVect12);
    xL = MTH3D_M_xDotProductVector(p_stVectAB, &stVectA1);

    xTmp = MTH_M_xMul(xK, xM);
    MTH3D_M_vMulScalarVector(&stVectP, xTmp, p_stVectAB);

    if (MTH3D_M_bEqualVector(&stVectP, p_stVect12))
        return FALSE;

    MTH3D_M_vSubVector(&stVectP, &stVectP, p_stVect12);

    xTmp = MTH_M_xMul(xL, xM);
    MTH3D_M_vMulScalarVector(&stVectQ, xTmp, p_stVectAB);
    MTH3D_M_vSubVector(&stVectQ, &stVectQ, &stVectA1);

    xA = MTH3D_M_xSqrVector(&stVectP);
    xB = MTH3D_M_xDotProductVector(&stVectP, &stVectQ);
    xB = MTH_M_xAdd(xB, xB);
    xC = MTH_M_xSub(MTH3D_M_xSqrVector(&stVectQ), MTH_M_xSqr(xCylinderRadius));

    switch (INT_fn_xTrinomeResolution(xA, xB, xC, &xT1, &xT2))
    {
    case INT_C_xTwoSolutions:
        *p_xTSegment = MTH_M_xMin(xT1, xT2);
        if (!(MTH_M_bGreaterEqual(*p_xTSegment, MTH_C_ZERO) && MTH_M_bLessEqual(*p_xTSegment, MTH_C_ONE)))
            return FALSE;
        break;
    case INT_C_xNoSolution:
        return FALSE;
    case INT_C_xOneSolution:
        if (MTH_M_bGreaterEqual(xT1, MTH_C_ZERO) && MTH_M_bLessEqual(xT1, MTH_C_ONE))
            *p_xTSegment = xT1;
        else
            return FALSE;
        break;
    default:
        break;
    }

    MTH3D_M_vMulAddVector(p_stHitSegment, *p_xTSegment, p_stVect12, p_stVertex1);

    xTLine = MTH_M_xAdd(MTH_M_xMul(xK, *p_xTSegment), xL);
    xTLine = MTH_M_xMul(xTLine, xM);

    if (MTH_M_bGreaterEqual(xTLine, MTH_C_ZERO) && MTH_M_bLessEqual(xTLine, MTH_C_ONE))
    {
        MTH3D_M_vMulAddVector(p_stHitLine, xTLine, p_stVectAB, p_stVertexA);
        return TRUE;
    }
    return FALSE;
}

#define M_GetCoord(p_stVect,Num) \
	*( ((MTH_tdxReal*)p_stVect) + Num)

#define COL_C_xXMin 0
#define COL_C_xXMax 1
#define COL_C_xYMin 2
#define COL_C_xYMax 3
#define COL_C_xZMin 4
#define COL_C_xZMax 5
extern MTH3D_tdstVector COL_g_a6_stBoxSidesNormal[6];

ACP_tdxBool
INT_fn_bIntersectSegmentWithBox(MTH3D_tdstVector *p_stVertex1,
                                MTH3D_tdstVector *p_stVertex2,
                                MTH3D_tdstVector *p_stVect12,
                                MTH3D_tdstVector *p_stMinPoint,
                                MTH3D_tdstVector *p_stMaxPoint,
                                MTH_tdxReal      *p_xT,
                                MTH3D_tdstVector *p_stHit,
                                MTH3D_tdstVector *p_stNormal)
{
    unsigned long ulVertex1Test;
    unsigned long ulVertex2Test;
    unsigned long ulExploredPlan;

    ulVertex1Test = INT_fn_ulGetPositionPointWithBox(p_stVertex1, p_stMinPoint, p_stMaxPoint);
    if (ulVertex1Test == COL_C_ulNullMask)
        return FALSE;
    ulVertex2Test = INT_fn_ulGetPositionPointWithBox(p_stVertex2, p_stMinPoint, p_stMaxPoint);
    if (ulVertex1Test & ulVertex2Test)
        return FALSE;

    ulExploredPlan = ulVertex1Test;

    if (ulExploredPlan & COL_C_ulXMinMask) {
        *p_xT = MTH_M_xDiv(MTH_M_xSub(p_stMinPoint->xX, p_stVertex1->xX), p_stVect12->xX);
        p_stHit->xX = p_stMinPoint->xX;
        p_stHit->xY = MTH_M_xMulAdd(p_stVect12->xY, *p_xT, p_stVertex1->xY);
        p_stHit->xZ = MTH_M_xMulAdd(p_stVect12->xZ, *p_xT, p_stVertex1->xZ);

        if (INT_fn_bInYZRange(p_stHit, p_stMinPoint, p_stMaxPoint)) {
            MTH3D_M_vCopyVector(p_stNormal, &(COL_g_a6_stBoxSidesNormal[COL_C_xXMin]));
            return TRUE;
        }
    }

    if (ulExploredPlan & COL_C_ulXMaxMask) {
        *p_xT = MTH_M_xDiv(MTH_M_xSub(p_stMaxPoint->xX, p_stVertex1->xX), p_stVect12->xX);
        p_stHit->xX = p_stMaxPoint->xX;
        p_stHit->xY = MTH_M_xMulAdd(p_stVect12->xY, *p_xT, p_stVertex1->xY);
        p_stHit->xZ = MTH_M_xMulAdd(p_stVect12->xZ, *p_xT, p_stVertex1->xZ);

        if (INT_fn_bInYZRange(p_stHit, p_stMinPoint, p_stMaxPoint)) {
            MTH3D_M_vCopyVector(p_stNormal, &(COL_g_a6_stBoxSidesNormal[COL_C_xXMax]));
            return TRUE;
        }
    }

    if (ulExploredPlan & COL_C_ulYMinMask) {
        *p_xT = MTH_M_xDiv(MTH_M_xSub(p_stMinPoint->xY, p_stVertex1->xY), p_stVect12->xY);
        p_stHit->xX = MTH_M_xMulAdd(p_stVect12->xX, *p_xT, p_stVertex1->xX);
        p_stHit->xY = p_stMinPoint->xY;
        p_stHit->xZ = MTH_M_xMulAdd(p_stVect12->xZ, *p_xT, p_stVertex1->xZ);

        if (INT_fn_bInXZRange(p_stHit, p_stMinPoint, p_stMaxPoint)) {
            MTH3D_M_vCopyVector(p_stNormal, &(COL_g_a6_stBoxSidesNormal[COL_C_xYMin]));
            return TRUE;
        }
    }

    if (ulExploredPlan & COL_C_ulYMaxMask) {
        *p_xT = MTH_M_xDiv(MTH_M_xSub(p_stMaxPoint->xY, p_stVertex1->xY), p_stVect12->xY);
        p_stHit->xX = MTH_M_xMulAdd(p_stVect12->xX, *p_xT, p_stVertex1->xX);
        p_stHit->xY = p_stMaxPoint->xY;
        p_stHit->xZ = MTH_M_xMulAdd(p_stVect12->xZ, *p_xT, p_stVertex1->xZ);

        if (INT_fn_bInXZRange(p_stHit, p_stMinPoint, p_stMaxPoint)) {
            MTH3D_M_vCopyVector(p_stNormal, &(COL_g_a6_stBoxSidesNormal[COL_C_xYMax]));
            return TRUE;
        }
    }

    if (ulExploredPlan & COL_C_ulZMinMask) {
        *p_xT = MTH_M_xDiv(MTH_M_xSub(p_stMinPoint->xZ, p_stVertex1->xZ), p_stVect12->xZ);
        p_stHit->xX = MTH_M_xMulAdd(p_stVect12->xX, *p_xT, p_stVertex1->xX);
        p_stHit->xY = MTH_M_xMulAdd(p_stVect12->xY, *p_xT, p_stVertex1->xY);
        p_stHit->xZ = p_stMinPoint->xZ;

        if (INT_fn_bInXYRange(p_stHit, p_stMinPoint, p_stMaxPoint)) {
            MTH3D_M_vCopyVector(p_stNormal, &(COL_g_a6_stBoxSidesNormal[COL_C_xZMin]));
            return TRUE;
        }
    }

    if (ulExploredPlan & COL_C_ulZMaxMask) {
        *p_xT = MTH_M_xDiv(MTH_M_xSub(p_stMaxPoint->xZ, p_stVertex1->xZ), p_stVect12->xZ);
        p_stHit->xX = MTH_M_xMulAdd(p_stVect12->xX, *p_xT, p_stVertex1->xX);
        p_stHit->xY = MTH_M_xMulAdd(p_stVect12->xY, *p_xT, p_stVertex1->xY);
        p_stHit->xZ = p_stMaxPoint->xZ;

        if (INT_fn_bInXYRange(p_stHit, p_stMinPoint, p_stMaxPoint)) {
            MTH3D_M_vCopyVector(p_stNormal, &(COL_g_a6_stBoxSidesNormal[COL_C_xZMax]));
            return TRUE;
        }
    }

    return FALSE;
}

static unsigned char ucPlaneCrossingdetect(MTH3D_tdstVector *p_stVertex1, MTH3D_tdstVector *p_stVect12, MTH3D_tdstVector *p_stMinPoint, MTH3D_tdstVector *p_stMaxPoint, MTH_tdxReal xT, char cFace);

ACP_tdxBool
INT_fn_bDetectIntersectSegmentWithBox(MTH3D_tdstVector *p_stVertex1,
                                      MTH3D_tdstVector *p_stVect12,
                                      MTH3D_tdstVector *p_stMinPoint,
                                      MTH3D_tdstVector *p_stMaxPoint,
                                      MTH_tdxReal      *_p_xT)
{
    unsigned long      ulVertex1Test;
    unsigned long      ulVertex2Test;
    unsigned long      ulExploredPlan;
    MTH_tdxReal        xT;
    MTH_tdxReal       *p_xT;
    MTH3D_tdstVector   stVertex2;
    ACP_tdxBool        bResult;
    ACP_tdxBool        bWithxT;

    if (_p_xT == NULL) {
        bWithxT = FALSE;
        p_xT    = &xT;
    } else {
        p_xT    = _p_xT;
        bWithxT = TRUE;
    }

    ulVertex1Test = INT_fn_ulGetPositionPointWithBox(p_stVertex1, p_stMinPoint, p_stMaxPoint);
    if (ulVertex1Test == COL_C_ulNullMask) {
        *p_xT = MTH_C_ZERO;
        return TRUE;
    }

    MTH3D_M_vAddVector(&stVertex2, p_stVertex1, p_stVect12);

    ulVertex2Test = INT_fn_ulGetPositionPointWithBox(&stVertex2, p_stMinPoint, p_stMaxPoint);
    if (ulVertex2Test == COL_C_ulNullMask) {
        *p_xT = 1;
        return TRUE;
    }

    if (ulVertex1Test & ulVertex2Test) {

        return FALSE;
    }

    ulExploredPlan = ulVertex1Test | ulVertex2Test;

    if (!(ulExploredPlan & (COL_C_ulXMinMask | COL_C_ulXMaxMask | COL_C_ulYMinMask | COL_C_ulYMaxMask))) {
        if (!bWithxT) return TRUE;
        if (ulVertex1Test & COL_C_ulZMinMask)
            *p_xT = MTH_M_xSub(p_stMinPoint->xZ, p_stVertex1->xZ);
        else
            *p_xT = MTH_M_xSub(p_stMaxPoint->xZ, p_stVertex1->xZ);
        *p_xT = MTH_M_xDiv(*p_xT, p_stVect12->xZ);
        return TRUE;
    }
    if (!(ulExploredPlan & (COL_C_ulXMinMask | COL_C_ulXMaxMask | COL_C_ulZMinMask | COL_C_ulZMaxMask))) {
        if (!bWithxT) return TRUE;
        if (ulVertex1Test & COL_C_ulYMinMask)
            *p_xT = MTH_M_xSub(p_stMinPoint->xY, p_stVertex1->xY);
        else
            *p_xT = MTH_M_xSub(p_stMaxPoint->xY, p_stVertex1->xY);
        *p_xT = MTH_M_xDiv(*p_xT, p_stVect12->xY);
        return TRUE;
    }
    if (!(ulExploredPlan & (COL_C_ulZMinMask | COL_C_ulZMaxMask | COL_C_ulYMinMask | COL_C_ulYMaxMask))) {
        if (!bWithxT) return TRUE;
        if (ulVertex1Test & COL_C_ulXMinMask)
            *p_xT = MTH_M_xSub(p_stMinPoint->xX, p_stVertex1->xX);
        else
            *p_xT = MTH_M_xSub(p_stMaxPoint->xX, p_stVertex1->xX);
        *p_xT = MTH_M_xDiv(*p_xT, p_stVect12->xX);
        return TRUE;
    }

    ulExploredPlan = ulVertex1Test;

    if (ulExploredPlan & COL_C_ulXMinMask) {
        *p_xT = MTH_M_xSub(p_stMinPoint->xX, p_stVertex1->xX);
        *p_xT = MTH_M_xDiv(*p_xT, p_stVect12->xX);
        if ((bResult = ucPlaneCrossingdetect(p_stVertex1, p_stVect12, p_stMinPoint, p_stMaxPoint, *p_xT, 'X')) < 2)
            return bResult;
    }
    if (ulExploredPlan & COL_C_ulXMaxMask) {
        *p_xT = MTH_M_xSub(p_stMaxPoint->xX, p_stVertex1->xX);
        *p_xT = MTH_M_xDiv(*p_xT, p_stVect12->xX);
        if ((bResult = ucPlaneCrossingdetect(p_stVertex1, p_stVect12, p_stMinPoint, p_stMaxPoint, *p_xT, 'X')) < 2)
            return bResult;
    }
    if (ulExploredPlan & COL_C_ulYMinMask) {
        *p_xT = MTH_M_xSub(p_stMinPoint->xY, p_stVertex1->xY);
        *p_xT = MTH_M_xDiv(*p_xT, p_stVect12->xY);
        if ((bResult = ucPlaneCrossingdetect(p_stVertex1, p_stVect12, p_stMinPoint, p_stMaxPoint, *p_xT, 'Y')) < 2)
            return bResult;
    }
    if (ulExploredPlan & COL_C_ulYMaxMask) {
        *p_xT = MTH_M_xSub(p_stMaxPoint->xY, p_stVertex1->xY);
        *p_xT = MTH_M_xDiv(*p_xT, p_stVect12->xY);
        if ((bResult = ucPlaneCrossingdetect(p_stVertex1, p_stVect12, p_stMinPoint, p_stMaxPoint, *p_xT, 'Y')) < 2)
            return bResult;
    }
    if (ulExploredPlan & COL_C_ulZMinMask) {
        *p_xT = MTH_M_xSub(p_stMinPoint->xZ, p_stVertex1->xZ);
        *p_xT = MTH_M_xDiv(*p_xT, p_stVect12->xZ);
        if ((bResult = ucPlaneCrossingdetect(p_stVertex1, p_stVect12, p_stMinPoint, p_stMaxPoint, *p_xT, 'Z')) < 2)
            return bResult;
    }
    if (ulExploredPlan & COL_C_ulZMaxMask) {
        *p_xT = MTH_M_xSub(p_stMaxPoint->xZ, p_stVertex1->xZ);
        *p_xT = MTH_M_xDiv(*p_xT, p_stVect12->xZ);
        if ((bResult = ucPlaneCrossingdetect(p_stVertex1, p_stVect12, p_stMinPoint, p_stMaxPoint, *p_xT, 'Z')) < 2)
            return bResult;
    }
    return FALSE;
}

static unsigned char ucPlaneCrossingdetect(MTH3D_tdstVector	*p_stVertex1,
										   MTH3D_tdstVector	*p_stVect12,
										   MTH3D_tdstVector	*p_stMinPoint,
										   MTH3D_tdstVector	*p_stMaxPoint,
										   MTH_tdxReal			xT,
										   char				cFace)

{
	unsigned long idx1,idx2;
	MTH_tdxReal xTmp;

	switch (cFace)
	{
	case 'X':
		idx1 = 1;
		idx2 = 2;
		break;
	case 'Y':
		idx1 = 0;
		idx2 = 2;
		break;
	case 'Z':
		idx1 = 0;
		idx2 = 1;
		break;
	default:
		break;
	}

	xTmp = MTH_M_xMulAdd (M_GetCoord(p_stVect12,idx1), xT, M_GetCoord(p_stVertex1,idx1));
	if (MTH_M_bGreaterZero(M_GetCoord(p_stVect12,idx1)))
	{

		if (MTH_M_bGreater (xTmp,M_GetCoord(p_stMaxPoint,idx1)) )
			return 0;

		if (MTH_M_bGreaterEqual(xTmp,M_GetCoord(p_stMinPoint,idx1)) )
		{

			xTmp = MTH_M_xAdd (M_GetCoord(p_stVertex1,idx2), MTH_M_xMul(M_GetCoord(p_stVect12,idx2), xT));
			if (MTH_M_bGreaterZero(M_GetCoord(p_stVect12,idx2)))
			{

				if (MTH_M_bGreater (xTmp,M_GetCoord(p_stMaxPoint,idx2)) )
					return 0;

				if (MTH_M_bGreaterEqual(xTmp,M_GetCoord(p_stMinPoint,idx2)) )
				{
					return 1;
				}

			}
			else
			{

				if (MTH_M_bLess (xTmp,M_GetCoord(p_stMinPoint,idx2)) )
					return 0;

				if (MTH_M_bLessEqual(xTmp,M_GetCoord(p_stMaxPoint,idx2)) )
				{
					return 1;
				}

			}
		}

	}
	else
	{

		if (MTH_M_bLess (xTmp,M_GetCoord(p_stMinPoint,idx1)) )
			return 0;

		if (MTH_M_bLessEqual(xTmp,M_GetCoord(p_stMaxPoint,idx1)) )
		{

			xTmp = MTH_M_xAdd (M_GetCoord(p_stVertex1,idx2), MTH_M_xMul(M_GetCoord(p_stVect12,idx2), xT));
			if (MTH_M_bGreaterZero(M_GetCoord(p_stVect12,idx2)))
			{

				if (MTH_M_bGreater (xTmp,M_GetCoord(p_stMaxPoint,idx2)) )
					return 0;

				if (MTH_M_bGreaterEqual(xTmp,M_GetCoord(p_stMinPoint,idx2)) )
				{
					return 1;
				}

			}
			else
			{

				if (MTH_M_bLess (xTmp,M_GetCoord(p_stMinPoint,idx2)) )
					return 0;

				if (MTH_M_bLessEqual(xTmp,M_GetCoord(p_stMaxPoint,idx2)) )
				{
					return 1;
				}

			}
		}

	}
	return 2;
}

ACP_tdxBool
INT_fn_bDetectIntersectSegmentWithSphere(MTH3D_tdstVector *p_stVertexA,
                                         MTH3D_tdstVector *p_stVectAB,
                                         MTH3D_tdstVector *p_stSphereCenter,
                                         MTH_tdxReal       xSphereRadius)
{
    MTH3D_tdstVector stVectCA;
    MTH3D_tdstVector stVectCB;
    MTH_tdxReal      xdist, xTemp;
    MTH_tdxReal      xNegSphereRadius;
    MTH_tdxReal      xSpecialRadius;
    MTH_tdxReal      NormAB;

    /* 0x00491A50 */
    MTH3D_M_vSubVector(&stVectCA, p_stVertexA, p_stSphereCenter);
    MTH3D_M_vAddVector(&stVectCB, &stVectCA, p_stVectAB);
    xNegSphereRadius = MTH_M_xNeg(xSphereRadius);
    xSpecialRadius   = MTH_M_xSqr(xSphereRadius);

    if (MTH_M_bLessEqual(MTH3D_M_xSqrVector(&stVectCB), xSpecialRadius))
        return TRUE;

    if (MTH_M_bLessEqual(MTH3D_M_xSqrVector(&stVectCA), xSpecialRadius))
        return TRUE;

    if (MTH_M_bGreaterZero(p_stVectAB->xX)) {
        if (MTH_M_bGreater(stVectCA.xX, xSphereRadius))    return FALSE;
        if (MTH_M_bLess(stVectCB.xX, xNegSphereRadius))    return FALSE;
    } else {
        if (MTH_M_bLess(stVectCA.xX, xNegSphereRadius))    return FALSE;
        if (MTH_M_bGreater(stVectCB.xX, xSphereRadius))    return FALSE;
    }
    if (MTH_M_bGreaterZero(p_stVectAB->xY)) {
        if (MTH_M_bGreater(stVectCA.xY, xSphereRadius))    return FALSE;
        if (MTH_M_bLess(stVectCB.xY, xNegSphereRadius))    return FALSE;
    } else {
        if (MTH_M_bLess(stVectCA.xY, xNegSphereRadius))    return FALSE;
        if (MTH_M_bGreater(stVectCB.xY, xSphereRadius))    return FALSE;
    }
    if (MTH_M_bGreaterZero(p_stVectAB->xZ)) {
        if (MTH_M_bGreater(stVectCA.xZ, xSphereRadius))    return FALSE;
        if (MTH_M_bLess(stVectCB.xZ, xNegSphereRadius))    return FALSE;
    } else {
        if (MTH_M_bLess(stVectCA.xZ, xNegSphereRadius))    return FALSE;
        if (MTH_M_bGreater(stVectCB.xZ, xSphereRadius))    return FALSE;
    }

    xTemp = MTH3D_M_xDotProductVector(&stVectCA, p_stVectAB);
    if (MTH_M_bGreater(xTemp, MTH_C_ZERO))
        return FALSE;

    NormAB = MTH3D_M_xSqrVector(p_stVectAB);
    if (MTH_M_bGreater(MTH_M_xNeg(xTemp), NormAB))
        return FALSE;

    xSpecialRadius = MTH_M_xMul(xSpecialRadius, NormAB);

    xdist = MTH_M_xSub(MTH_M_xMul(stVectCA.xX, p_stVectAB->xY),
                       MTH_M_xMul(stVectCA.xY, p_stVectAB->xX));
    xdist = MTH_M_xSqr(xdist);
    if (MTH_M_bGreaterEqual(xdist, xSpecialRadius))
        return FALSE;

    xTemp = MTH_M_xSub(MTH_M_xMul(stVectCA.xX, p_stVectAB->xZ),
                       MTH_M_xMul(stVectCA.xZ, p_stVectAB->xX));
    xdist = MTH_M_xAdd(MTH_M_xSqr(xTemp), xdist);
    if (MTH_M_bGreaterEqual(xdist, xSpecialRadius))
        return FALSE;

    xTemp = MTH_M_xSub(MTH_M_xMul(stVectCA.xY, p_stVectAB->xZ),
                       MTH_M_xMul(stVectCA.xZ, p_stVectAB->xY));
    xdist = MTH_M_xAdd(MTH_M_xSqr(xTemp), xdist);
    if (MTH_M_bGreaterEqual(xdist, xSpecialRadius))
        return FALSE;

    return TRUE;
}

#define FUT_M_xMaxValue3(A, B, C) \
    ((((A) > (B)) && ((A) > (C))) ? (A) : (((B) > (C)) ? (B) : (C)))
#define FUT_M_xMinValue3(A, B, C) \
    ((((A) < (B)) && ((A) < (C))) ? (A) : (((B) < (C)) ? (B) : (C)))

/* 0x00490CD0 */
ACP_tdxBool
INT_fn_bGetPointInTriangle(MTH3D_tdstVector *p_stPoint,
                           MTH3D_tdstVector *p_stVertex0,
                           MTH3D_tdstVector *p_stVertex1,
                           MTH3D_tdstVector *p_stVertex2,
                           MTH3D_tdstVector *p_stNormal)
{
    MTH3D_tdstVector stVectP0;
    MTH3D_tdstVector stVectP1;
    MTH3D_tdstVector stVectP2;
    MTH_tdxReal      xDet;

    MTH3D_M_vSubVector(&stVectP0, p_stVertex0, p_stPoint);
    MTH3D_M_vSubVector(&stVectP1, p_stVertex1, p_stPoint);
    MTH3D_M_vSubVector(&stVectP2, p_stVertex2, p_stPoint);

    if (MTH_M_xAbs(p_stNormal->xZ) >= MTH_C_InvSqrt3)
    {
        xDet = MTH3D_M_xDetXYVector(&stVectP0, &stVectP1);
        if (!FUT_M_bSameSign(xDet, p_stNormal->xZ)) return FALSE;
        xDet = MTH3D_M_xDetXYVector(&stVectP1, &stVectP2);
        if (!FUT_M_bSameSign(xDet, p_stNormal->xZ)) return FALSE;
        xDet = MTH3D_M_xDetXYVector(&stVectP2, &stVectP0);
        return FUT_M_bSameSign(xDet, p_stNormal->xZ);
    }
    else if (MTH_M_xAbs(p_stNormal->xX) >= MTH_C_InvSqrt3)
    {
        xDet = MTH3D_M_xDetYZVector(&stVectP0, &stVectP1);
        if (!FUT_M_bSameSign(xDet, p_stNormal->xX)) return FALSE;
        xDet = MTH3D_M_xDetYZVector(&stVectP1, &stVectP2);
        if (!FUT_M_bSameSign(xDet, p_stNormal->xX)) return FALSE;
        xDet = MTH3D_M_xDetYZVector(&stVectP2, &stVectP0);
        return FUT_M_bSameSign(xDet, p_stNormal->xX);
    }
    else
    {
        xDet = MTH3D_M_xDetZXVector(&stVectP0, &stVectP1);
        if (!FUT_M_bSameSign(xDet, p_stNormal->xY)) return FALSE;
        xDet = MTH3D_M_xDetZXVector(&stVectP1, &stVectP2);
        if (!FUT_M_bSameSign(xDet, p_stNormal->xY)) return FALSE;
        xDet = MTH3D_M_xDetZXVector(&stVectP2, &stVectP0);
        return FUT_M_bSameSign(xDet, p_stNormal->xY);
    }
}

/* 0x00490FA0 */
static ACP_tdxBool
INT_fn_bFastRejectIntersectSegmentWithTriangle(MTH3D_tdstVector *p_stVertexA,
                                               MTH3D_tdstVector *p_stVectAB,
                                               MTH3D_tdstVector *p_stVertex1,
                                               MTH3D_tdstVector *p_stVertex2,
                                               MTH3D_tdstVector *p_stVertex3)
{
    MTH_tdxReal      xMin;
    MTH_tdxReal      xMax;
    MTH3D_tdstVector stVertexB;
    ACP_tdxBool      bGreaterZero;

    MTH3D_M_vAddVector(&stVertexB, p_stVertexA, p_stVectAB);

    bGreaterZero = MTH_M_bGreaterZero(p_stVectAB->xX);
    xMin = bGreaterZero ? p_stVertexA->xX : stVertexB.xX;
    xMax = FUT_M_xMaxValue3(p_stVertex1->xX, p_stVertex2->xX, p_stVertex3->xX);
    if (MTH_M_bLess(xMax, xMin)) return FALSE;
    xMax = bGreaterZero ? stVertexB.xX : p_stVertexA->xX;
    xMin = FUT_M_xMinValue3(p_stVertex1->xX, p_stVertex2->xX, p_stVertex3->xX);
    if (MTH_M_bLess(xMax, xMin)) return FALSE;

    bGreaterZero = MTH_M_bGreaterZero(p_stVectAB->xY);
    xMin = bGreaterZero ? p_stVertexA->xY : stVertexB.xY;
    xMax = FUT_M_xMaxValue3(p_stVertex1->xY, p_stVertex2->xY, p_stVertex3->xY);
    if (MTH_M_bLess(xMax, xMin)) return FALSE;
    xMax = bGreaterZero ? stVertexB.xY : p_stVertexA->xY;
    xMin = FUT_M_xMinValue3(p_stVertex1->xY, p_stVertex2->xY, p_stVertex3->xY);
    if (MTH_M_bLess(xMax, xMin)) return FALSE;

    bGreaterZero = MTH_M_bGreaterZero(p_stVectAB->xZ);
    xMin = bGreaterZero ? p_stVertexA->xZ : stVertexB.xZ;
    xMax = FUT_M_xMaxValue3(p_stVertex1->xZ, p_stVertex2->xZ, p_stVertex3->xZ);
    if (MTH_M_bLess(xMax, xMin)) return FALSE;
    xMax = bGreaterZero ? stVertexB.xZ : p_stVertexA->xZ;
    xMin = FUT_M_xMinValue3(p_stVertex1->xZ, p_stVertex2->xZ, p_stVertex3->xZ);
    if (MTH_M_bLess(xMax, xMin)) return FALSE;

    return TRUE;
}

/* 0x00491380 */
ACP_tdxBool
INT_fn_bIntersectSegmentWithTriangle2(MTH3D_tdstVector *p_stVertexA,
                                      MTH3D_tdstVector *p_stVectAB,
                                      MTH3D_tdstVector *p_stVertex1,
                                      MTH3D_tdstVector *p_stVertex2,
                                      MTH3D_tdstVector *p_stVertex3,
                                      MTH3D_tdstVector *p_stNormal,
                                      MTH_tdxReal       xDPlan,
                                      MTH_tdxReal       xDotProduct,
                                      MTH_tdxReal      *p_xT,
                                      MTH3D_tdstVector *p_stHit)
{
    MTH_tdxReal xDivid;
    MTH_tdxReal xT;

    if (!INT_fn_bFastRejectIntersectSegmentWithTriangle(p_stVertexA, p_stVectAB,
                                                        p_stVertex1, p_stVertex2, p_stVertex3))
        return FALSE;

    xDivid = MTH_M_xAdd(MTH3D_M_xDotProductVector(p_stNormal, p_stVertexA), xDPlan);

    /* 0x0049CE28 */
    if (MTH_M_bIsNullWithEpsilon(xDivid, (MTH_tdxReal)0.001f) &&
        MTH_M_bLess(xDotProduct, (MTH_tdxReal)-0.001f))
    {
        *p_xT = MTH_C_ZERO;
        *p_stHit = *p_stVertexA;
        return TRUE;
    }

    if (MTH_M_bGreaterEqualZero(xDivid) && MTH_M_bLess(xDivid, MTH_M_xNeg(xDotProduct)))
    {
        xT = MTH_M_xNeg(MTH_M_xDiv(xDivid, xDotProduct));
        MTH3D_M_vMulAddVector(p_stHit, xT, p_stVectAB, p_stVertexA);
        if (INT_fn_bGetPointInTriangle(p_stHit, p_stVertex1, p_stVertex2, p_stVertex3, p_stNormal))
        {
            *p_xT = xT;
            return TRUE;
        }
        return FALSE;
    }
    return FALSE;
}

#pragma optimize("y", off)

ACP_tdxBool INT_fn_bIntersectSegmentWithTriangle ( MTH3D_tdstVector *p_stVertexA,
                                                   MTH3D_tdstVector *p_stVectAB,
                                                   MTH3D_tdstVector *p_stVertex1,
                                                   MTH3D_tdstVector *p_stVertex2,
                                                   MTH3D_tdstVector *p_stVertex3,
                                                   MTH3D_tdstVector *p_stNormal,
                                                   MTH_tdxReal xDPlan,
                                                   MTH_tdxReal *p_xT,
                                                   MTH3D_tdstVector *p_stHit )
{
	MTH_tdxReal xDotProdPara;
	MTH_tdxReal xDivid;
	MTH_tdxReal xT;

#if 1
	if ( !INT_fn_bFastRejectIntersectSegmentWithTriangle(p_stVertexA, p_stVectAB, p_stVertex1, p_stVertex2, p_stVertex3) )
		return FALSE;
#endif

	xDotProdPara = MTH3D_M_xDotProductVector ( p_stNormal, p_stVectAB );

	if ( MTH_M_bLessZero ( xDotProdPara ) )
	{

		xDivid = MTH_M_xAdd ( MTH3D_M_xDotProductVector ( p_stNormal, p_stVertexA ), xDPlan );

		/* 0x00491250 */
		if ( ( MTH_M_bGreaterZero ( xDivid ) ) && ( MTH_M_bLess ( xDivid, MTH_M_xNeg ( xDotProdPara ) ) ) )
		{

			xT = MTH_M_xNeg ( MTH_M_xDiv ( xDivid, xDotProdPara ) );

			MTH3D_M_vMulAddVector ( p_stHit, xT, p_stVectAB, p_stVertexA );

			if ( INT_fn_bGetPointInTriangle ( p_stHit, p_stVertex1, p_stVertex2, p_stVertex3, p_stNormal ) )
			{
				*p_xT = xT;
				return TRUE;
			}
			else
			{
				return FALSE;
			}
		}
		else
		{

			return FALSE;
		}
	}
	else
	{
		return FALSE;
	}
}

MTH_tdxReal INT_fn_xDistancePointToLine ( MTH3D_tdstVector *p_stPoint, MTH3D_tdstVector *p_stVertexA, MTH3D_tdstVector *p_stVectAB, MTH_tdxReal *p_xT, MTH3D_tdstVector *p_stHit );

ACP_tdxBool INT_fn_bGetInclusionPointInCone ( MTH3D_tdstVector *p_stPoint,
                                              MTH3D_tdstVector *p_stTopPoint,
                                              MTH3D_tdstVector *p_stBasePoint,
                                              MTH_tdxReal xBaseRadius )
{
  MTH_tdxReal xT;
  MTH3D_tdstVector stHit;
  MTH_tdxReal xDist;

  MTH3D_tdstVector	stTopBaseVector;

  MTH3D_M_vSubVector(&stTopBaseVector, p_stBasePoint, p_stTopPoint);
  xDist = INT_fn_xDistancePointToLine ( p_stPoint, p_stTopPoint, &stTopBaseVector,
                                        &xT, &stHit );

  return ( ( MTH_M_bInUnitEqual ( xT ) ) && ( MTH_M_bLessEqual ( xDist, MTH_M_xMul ( xT, xBaseRadius ) ) ) );
}

MTH_tdxReal INT_fn_xDistancePointToLine ( MTH3D_tdstVector *p_stPoint,
                                          MTH3D_tdstVector *p_stVertexA,
                                          MTH3D_tdstVector *p_stVectAB,
                                          MTH_tdxReal *p_xT,
                                          MTH3D_tdstVector *p_stHit )
{
  MTH3D_tdstVector stVectAP;

  if ( !(MTH3D_M_bIsNullVector ( p_stVectAB )) )
  {
    MTH3D_M_vSubVector ( &stVectAP, p_stPoint, p_stVertexA );

    *p_xT = MTH_M_xDiv ( MTH3D_M_xDotProductVector ( p_stVectAB, &stVectAP ), MTH3D_M_xSqrVector ( p_stVectAB ) );

    MTH3D_M_vMulAddVector ( p_stHit, *p_xT, p_stVectAB, p_stVertexA );

  }
  else
  {
    *p_xT = MTH_C_ZERO;

    MTH3D_M_vCopyVector ( p_stHit, p_stVertexA );
  }

  return MTH3D_M_xVectorGap ( p_stHit, p_stPoint );

}

#pragma optimize("", on)

#pragma optimize("y", off)
ACP_tdxBool
INT_fn_bDetectIntersectSegmentWithTriangle(MTH3D_tdstVector *p_stVertexA,
                                           MTH3D_tdstVector *p_stVectAB,
                                           MTH3D_tdstVector *p_stVertex1,
                                           MTH3D_tdstVector *p_stVertex2,
                                           MTH3D_tdstVector *p_stVertex3,
                                           MTH3D_tdstVector *p_stNormal,
                                           MTH_tdxReal       xDPlan)
{
    MTH_tdxReal      xDotProdPara;
    MTH_tdxReal      xT;
    MTH3D_tdstVector stHit;

    if (!INT_fn_bFastRejectIntersectSegmentWithTriangle(
            p_stVertexA, p_stVectAB, p_stVertex1, p_stVertex2, p_stVertex3))
        return FALSE;

    /* 0x004914D0 */
    xDotProdPara = (p_stVectAB->xY * p_stNormal->xY)
                 + (p_stNormal->xX * p_stVectAB->xX)
                 + (p_stVectAB->xZ * p_stNormal->xZ);
    if (!MTH_M_bLessZero(xDotProdPara))
        return FALSE;

    xT = MTH_M_xNeg(MTH_M_xDiv(
              (p_stVertexA->xY * p_stNormal->xY)
            + (p_stNormal->xX * p_stVertexA->xX)
            + (p_stVertexA->xZ * p_stNormal->xZ)
            + xDPlan,
              xDotProdPara));

    if (!(MTH_M_bGreaterEqual(xT, MTH_C_ZERO) && MTH_M_bLessEqual(xT, MTH_C_ONE)))
        return FALSE;

    MTH3D_M_vMulAddVector(&stHit, xT, p_stVectAB, p_stVertexA);
    return INT_fn_bGetPointInTriangle(&stHit, p_stVertex1, p_stVertex2,
                                      p_stVertex3, p_stNormal);
}
#pragma optimize("", on)

/* 0x00490050 */
ACP_tdxIndex
INT_fn_xTrinomeResolution(MTH_tdxReal xA, MTH_tdxReal xB, MTH_tdxReal xC,
                          MTH_tdxReal *p_xT1, MTH_tdxReal *p_xT2)
{
    MTH_tdxReal xDelta;
    MTH_tdxReal xDeltaSqrt;
    MTH_tdxReal xDiv;

    if (MTH_M_bDifferentZero(xA))
    {
        xDelta = MTH_M_xSub(MTH_M_xMul(xB, xB),
                            MTH_M_xMul(MTH_M_xMul(xC, xA), MTH_C_4));
        if (MTH_M_bGreaterZero(xDelta))
        {
            xDeltaSqrt = MTH_M_xSqrt(xDelta);
            xB         = MTH_M_xNeg(xB);
            xDiv       = MTH_M_xDiv(MTH_C_ONE, MTH_M_xAdd(xA, xA));
            *p_xT1 = MTH_M_xMul(MTH_M_xAdd(xDeltaSqrt, xB), xDiv);
            *p_xT2 = MTH_M_xMul(MTH_M_xSub(xB, xDeltaSqrt), xDiv);
            return INT_C_xTwoSolutions;
        }
        else if (MTH_M_bLessZero(xDelta))
        {
            return INT_C_xNoSolution;
        }
        else if (MTH_M_bEqualZero(xDelta))
        {
            *p_xT1 = MTH_M_xNeg(MTH_M_xDiv(xB, MTH_M_xAdd(xA, xA)));
            return INT_C_xOneSolution;
        }
    }
    else
    {
        if (MTH_M_bDifferentZero(xB))
        {
            *p_xT1 = MTH_M_xNeg(MTH_M_xDiv(xC, xB));
            return INT_C_xOneSolution;
        }
        else
        {
            return INT_C_xNoSolution;
        }
    }

    return INT_C_xNoSolution;
}
