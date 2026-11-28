#include <stddef.h>
#include "COL/CsSphElt.h"
#include "COL/CsEltEdg.h"
#include "COL/OctreeGO.h"
#include "GEO/GeoObj.h"
#include "INT/Inters.h"
#include "POS/POS_types.h"
#include "GEO/GEO_types.h"
#include "GLI/geometric_helpers.h"
#include "MTH/MTH_flt.h"
#include "MTH/MTH_xNeg.h"
#include "MTH/MTH3D.h"

#define COL_M_vSetBitOfLong(lValue, lBitNumber) ((lValue) |= (1 << (lBitNumber)))

#define COL_CT_ASSERT(tag, cond) typedef char tag[(cond) ? 1 : -1]
COL_CT_ASSERT(cs_off_dinst0,  offsetof(COL_tdstGVForCollision, stDinST0Point)        == 0x240);
COL_CT_ASSERT(cs_off_dinst1,  offsetof(COL_tdstGVForCollision, stDinST1Point)        == 0x24C);
COL_CT_ASSERT(cs_off_dinst01, offsetof(COL_tdstGVForCollision, stDinST01Vector)      == 0x258);
COL_CT_ASSERT(cs_off_dynidxsph, offsetof(COL_tdstGVForCollision, p_stDynamicIndexedSphere) == 0x230);
COL_CT_ASSERT(cs_off_statidxsph, offsetof(COL_tdstGVForCollision, p_stStaticIndexedSphere) == 0x234);
COL_CT_ASSERT(cs_off_statcenter, offsetof(COL_tdstGVForCollision, p_stStaticCenter)  == 0x2B0);
COL_CT_ASSERT(cs_off_dynradius, offsetof(COL_tdstGVForCollision, xDynamicRadius)     == 0x2B4);
COL_CT_ASSERT(cs_off_hdynmat, offsetof(COL_tdstGVForCollision, hDynamicMaterial)     == 0x600);
COL_CT_ASSERT(cs_off_hstatmat, offsetof(COL_tdstGVForCollision, hStaticMaterial)     == 0x604);
COL_CT_ASSERT(cs_off_statelttri, offsetof(COL_tdstGVForCollision, p_stStaticElementIndexedTriangles) == 0x200);
COL_CT_ASSERT(cs_off_stattriidx, offsetof(COL_tdstGVForCollision, xStaticIndexedTriangleIndex)        == 0x21E);
COL_CT_ASSERT(cs_off_dyneltsph, offsetof(COL_tdstGVForCollision, p_stDynamicElementSpheres) == 0x20C);
COL_CT_ASSERT(cs_off_stateltsph, offsetof(COL_tdstGVForCollision, p_stStaticElementSpheres) == 0x210);
COL_CT_ASSERT(cs_off_dynsphidx, offsetof(COL_tdstGVForCollision, xDynamicIndexedSphereIndex) == 0x224);
COL_CT_ASSERT(cs_off_statsphidx, offsetof(COL_tdstGVForCollision, xStaticIndexedSphereIndex)  == 0x226);
COL_CT_ASSERT(cs_off_dyncenter, offsetof(COL_tdstGVForCollision, p_stDynamicCenter) == 0x2AC);
COL_CT_ASSERT(cs_off_bitsfield, offsetof(COL_tdstGVForCollision, lBitsFieldOfIndexedSpheresInCollision) == 0x22C);
COL_CT_ASSERT(cs_off_statedgeidx, offsetof(COL_tdstGVForCollision, xStaticEdgeIndex) == 0x1E6);
COL_CT_ASSERT(cs_off_vertex1, offsetof(COL_tdstGVForCollision, p_stVertex1)  == 0x298);
COL_CT_ASSERT(cs_off_vertex2, offsetof(COL_tdstGVForCollision, p_stVertex2)  == 0x29C);
COL_CT_ASSERT(cs_off_edgevec, offsetof(COL_tdstGVForCollision, stEdgeVector) == 0x2A0);
COL_CT_ASSERT(cs_off_dyngeomobj, offsetof(COL_tdstGVForCollision, p_stDynamicGeomObj) == 0x004);
COL_CT_ASSERT(cs_off_statgeomobj, offsetof(COL_tdstGVForCollision, p_stStaticGeomObj) == 0x010);
COL_CT_ASSERT(cs_off_d2st0, offsetof(COL_tdstGVForCollision, stD2ST0TransformMatrix) == 0x07C);
COL_CT_ASSERT(cs_off_scale, offsetof(COL_tdstGVForCollision, xStaticScale) == 0x1DC);
COL_CT_ASSERT(cs_off_octree, offsetof(COL_tdstGVForCollision, p_stOctree) == 0x608);
COL_CT_ASSERT(cs_off_nbselnodes, offsetof(COL_tdstGVForCollision, xNumberOfSelectedNodes) == 0x60C);
COL_CT_ASSERT(cs_off_selnodes, offsetof(COL_tdstGVForCollision, aDEF_pstSelectedNode) == 0x610);
COL_CT_ASSERT(cs_off_stateltidx, offsetof(COL_tdstGVForCollision, xStaticElementIndex) == 0x1EA);
COL_CT_ASSERT(cs_off_cases, offsetof(COL_tdstGVForCollision, ulSelectedCollisionCases) == 0x018);

#define COL_C_lUsesAllElements 0xFFFFFFFF
#define COL_M_bIsSetBitOfLong(lValue, lBitNumber) ((lValue) & (1 << (lBitNumber)))

void __cdecl COL_fn_vEnlargeSphereIfNecessary(COL_tdstGVForCollision *p_stGV)
{
    MTH_tdxReal      xSqrDistance;
    MTH_tdxReal      xDistance;
    MTH_tdxReal      xNewRadius;
    MTH_tdxReal      xCoef;
    MTH3D_tdstVector stDeplacement;

    p_stGV->xSwapRadius = p_stGV->xDynamicRadius;
    MTH3D_M_vCopyVector(&p_stGV->stSwapDinST0Point, &p_stGV->stDinST0Point);

    xSqrDistance = MTH3D_M_xSqrVector(&p_stGV->stDinST01Vector);

    /* 0x0049D0AC */
    if (MTH_M_bLess(xSqrDistance, (MTH_tdxReal)0.001f))
    {
        p_stGV->bUseEnlargeSphere = FALSE;
        return;
    }

    xDistance  = MTH_M_xSqrt(xSqrDistance);

    xNewRadius = MTH_M_xDiv(MTH_M_xAdd(xDistance, MTH_M_xMul(p_stGV->xDynamicRadius, MTH_C_2)), MTH_C_2);

    xCoef      = MTH_M_xDiv(MTH_M_xSub(xNewRadius, p_stGV->xDynamicRadius), xDistance);

    MTH3D_M_vMulScalarVector(&stDeplacement, xCoef, &p_stGV->stDinST01Vector);
    MTH3D_M_vAddVector(&p_stGV->stDinST0Point, &p_stGV->stDinST0Point, &stDeplacement);

    p_stGV->xDynamicRadius = xNewRadius;

    p_stGV->bUseEnlargeSphere = TRUE;
}

void __cdecl COL_fn_vUseInitialSphere(COL_tdstGVForCollision *p_stGV)
{
    MTH_tdxReal      xTmpRadius;
    MTH3D_tdstVector stTmpVector;

    if (p_stGV->bUseEnlargeSphere)
    {
        xTmpRadius          = p_stGV->xDynamicRadius;
        p_stGV->xDynamicRadius = p_stGV->xSwapRadius;
        p_stGV->xSwapRadius = xTmpRadius;

        MTH3D_M_vCopyVector(&stTmpVector, &p_stGV->stDinST0Point);
        MTH3D_M_vCopyVector(&p_stGV->stDinST0Point, &p_stGV->stSwapDinST0Point);
        MTH3D_M_vCopyVector(&p_stGV->stSwapDinST0Point, &stTmpVector);

        p_stGV->bUseEnlargeSphere = FALSE;
    }
}

void __cdecl COL_fn_vUseEnlargeSphere(COL_tdstGVForCollision *p_stGV)
{
    MTH_tdxReal      xTmpRadius;
    MTH3D_tdstVector stTmpVector;

    if (!p_stGV->bUseEnlargeSphere)
    {
        xTmpRadius          = p_stGV->xDynamicRadius;
        p_stGV->xDynamicRadius = p_stGV->xSwapRadius;
        p_stGV->xSwapRadius = xTmpRadius;

        MTH3D_M_vCopyVector(&stTmpVector, &p_stGV->stDinST0Point);
        MTH3D_M_vCopyVector(&p_stGV->stDinST0Point, &p_stGV->stSwapDinST0Point);
        MTH3D_M_vCopyVector(&p_stGV->stSwapDinST0Point, &stTmpVector);

        p_stGV->bUseEnlargeSphere = TRUE;
    }
}

/* 0x00499440 */
ACP_tdxBool __cdecl
COL_fn_bCollideStaticSphereWithStaticIndexedTriangle(COL_tdstGVForCollision *p_stGV,
                                                     ACP_tdxBool bFaceMapInsteadOfIndexedTriangle)
{
    GEO_tdstElementIndexedTriangles *p_stTri = p_stGV->p_stStaticElementIndexedTriangles;
    GEO_tdstTripledIndex            *p_stFace =
        p_stTri->d_stListOfFacesTripled + p_stGV->xStaticIndexedTriangleIndex;
    GEO_tdstIndexedSphere           *p_stDynamicSphere =
        (GEO_tdstIndexedSphere *)p_stGV->p_stDynamicIndexedSphere;

    MTH3D_tdstVector *p_stVertex1;
    MTH3D_tdstVector *p_stVertex2;
    MTH3D_tdstVector *p_stVertex3;
    MTH3D_tdstVector *p_stNormal;
    MTH_tdxReal       xDPlan;
    MTH3D_tdstVector  stTempNormal;
    MTH3D_tdstVector  stSegmentStartPoint;
    MTH3D_tdstVector  stSegmentVector;

    p_stVertex1 = p_stGV->p_stStaticGeomObj->d_stListOfPoints + p_stFace->a3_xIndex[0];
    p_stVertex2 = p_stGV->p_stStaticGeomObj->d_stListOfPoints + p_stFace->a3_xIndex[1];
    p_stVertex3 = p_stGV->p_stStaticGeomObj->d_stListOfPoints + p_stFace->a3_xIndex[2];
    p_stNormal  = &p_stTri->d_stListOfFacesNormals[p_stGV->xStaticIndexedTriangleIndex];

    xDPlan = MTH_M_xNeg(MTH3D_M_xDotProductVector(p_stNormal, p_stVertex1));

    COL_fn_vUseEnlargeSphere(p_stGV);

    MTH3D_M_vMulScalarVector(&stTempNormal, p_stGV->xDynamicRadius, p_stNormal);
    MTH3D_M_vAddVector(&stSegmentStartPoint, &p_stGV->stDinST0Point, &stTempNormal);
    MTH3D_M_vMulScalarVector(&stSegmentVector, MTH_C_Minus2, &stTempNormal);

    if (INT_fn_bDetectIntersectSegmentWithTriangle(&stSegmentStartPoint, &stSegmentVector,
                                                   p_stVertex1, p_stVertex2, p_stVertex3, p_stNormal, xDPlan))
    {
        MTH3D_tdstVector stMovementVector;

        COL_fn_vUseInitialSphere(p_stGV);
        MTH3D_M_vMulScalarVector(&stTempNormal, p_stGV->xDynamicRadius, p_stNormal);
        MTH3D_M_vMulScalarVector(&stSegmentVector, MTH_C_Minus2, &stTempNormal);

        MTH3D_M_vNegVector(&stMovementVector, &p_stGV->stDinST01Vector);
        MTH3D_M_vAddVector(&stSegmentStartPoint, &p_stGV->stDinST1Point, &stTempNormal);

        if (INT_fn_bDetectIntersectSegmentWithTriangle(&stSegmentStartPoint, &stSegmentVector,
                                                       p_stVertex1, p_stVertex2, p_stVertex3, p_stNormal, xDPlan))
        {

            MTH3D_tdstVector stT0AVector;
            MTH3D_tdstVector stT1AVector;
            MTH_tdxReal      xDotProduct;
            MTH_tdxReal      xDistance;
            MTH3D_tdstVector stTempDistance;
            MTH3D_tdstVector stTempHit;
            MTH3D_tdstVector stTempNormalOut;
            MTH3D_tdstVector stEndPosition;

            MTH3D_M_vSubVector(&stT0AVector, p_stVertex1, &p_stGV->stDinST0Point);
            MTH3D_M_vSubVector(&stT1AVector, p_stVertex1, &p_stGV->stDinST1Point);
            xDotProduct = MTH3D_M_xDotProductVector(&stT0AVector, p_stNormal);
            xDistance   = MTH_M_xAdd(xDotProduct, p_stGV->xDynamicRadius);

            if (MTH_M_bLessEqualZero(xDistance))
                return FALSE;

            MTH3D_M_vMulScalarVector(&stTempDistance, xDistance, p_stNormal);
            POS_fn_vMulMatrixVector(&stTempDistance, p_stGV->p_stStaticGeomObjMatrix, &stTempDistance);

            MTH3D_M_vMulScalarVector(&stTempHit, xDotProduct, p_stNormal);
            MTH3D_M_vAddVector(&stTempHit, &p_stGV->stDinST0Point, &stTempHit);

            if (!INT_fn_bGetPointInTriangle(&stTempHit, p_stVertex1, p_stVertex2, p_stVertex3, p_stNormal))
                return FALSE;

            POS_fn_vMulMatrixVertex(&stTempHit, p_stGV->p_stStaticGeomObjMatrix, &stTempHit);
            POS_fn_vRotateVector(&stTempNormalOut, p_stGV->p_stStaticGeomObjMatrix, p_stNormal);
            POS_fn_vMulMatrixVector(&stMovementVector, p_stGV->p_stStaticGeomObjMatrix, &stMovementVector);
            POS_fn_vMulMatrixVertex(&stEndPosition, p_stGV->p_stStaticGeomObjMatrix, &p_stGV->stDinST0Point);

            COL_fn_vAddInStaticCollisionTable(MTH_C_MinusONE, &stTempDistance, &stTempHit, &stTempNormalOut,
                                              p_stGV->hDynamicMaterial, p_stGV->hStaticMaterial,
                                              COL_C_xSphereEntity, COL_C_xTriangleEntity,
                                              p_stGV->p_vParameter1, p_stGV->sParameter2,
                                              &stMovementVector, p_stDynamicSphere->xRadius, &stEndPosition);
        }
        else
        {

            MTH3D_tdstVector stT1Center;
            MTH_tdxReal      xDotProduct;
            MTH_tdxReal      xT;
            MTH3D_tdstVector stHit;

            xDotProduct = MTH3D_M_xDotProductVector(p_stNormal, &stMovementVector);
            MTH3D_M_vMulAddVector(&stT1Center, MTH_M_xNeg(p_stGV->xDynamicRadius), p_stNormal, &p_stGV->stDinST1Point);

            if (INT_fn_bIntersectSegmentWithTriangle2(&stT1Center, &stMovementVector,
                                                      p_stVertex1, p_stVertex2, p_stVertex3,
                                                      p_stNormal, xDPlan, xDotProduct, &xT, &stHit))
            {
                MTH3D_tdstVector stTempHit;
                MTH3D_tdstVector stTempNormalOut;
                MTH3D_tdstVector stTempDistance;
                MTH3D_tdstVector stEndPosition;

                POS_fn_vMulMatrixVertex(&stTempHit, p_stGV->p_stStaticGeomObjMatrix, &stHit);
                POS_fn_vRotateVector(&stTempNormalOut, p_stGV->p_stStaticGeomObjMatrix, p_stNormal);
                MTH3D_M_vNullVector(&stTempDistance);
                POS_fn_vMulMatrixVector(&stMovementVector, p_stGV->p_stStaticGeomObjMatrix, &stMovementVector);
                POS_fn_vMulMatrixVertex(&stEndPosition, p_stGV->p_stStaticGeomObjMatrix, &p_stGV->stDinST0Point);

                COL_fn_vAddInStaticCollisionTable(xT, &stTempDistance, &stTempHit, &stTempNormalOut,
                                                  p_stGV->hDynamicMaterial, p_stGV->hStaticMaterial,
                                                  COL_C_xSphereEntity, COL_C_xTriangleEntity,
                                                  p_stGV->p_vParameter1, p_stGV->sParameter2,
                                                  &stMovementVector, p_stDynamicSphere->xRadius, &stEndPosition);
            }
            return FALSE;
        }
        return TRUE;
    }
    return FALSE;
}

void __cdecl COL_fn_vCollideStaticSphereWithStaticSphere(COL_tdstGVForCollision *p_stGV);

/* 0x00499A80 */
void COL_fn_vCollideStaticElementSpheresWithStaticElementIndexedTriangles ( COL_tdstGVForCollision *p_stGV )
{

  for ( p_stGV->xDynamicIndexedSphereIndex = 0 ; p_stGV->xDynamicIndexedSphereIndex < p_stGV->p_stDynamicElementSpheres->xNbSpheres ; p_stGV->xDynamicIndexedSphereIndex++ )
  {
	if( (p_stGV->lBitsFieldOfIndexedSpheresInCollision == COL_C_lUsesAllElements) ||
		 COL_M_bIsSetBitOfLong( p_stGV->lBitsFieldOfIndexedSpheresInCollision, p_stGV->xDynamicIndexedSphereIndex )
	  )
	{

        p_stGV->p_stDynamicIndexedSphere = p_stGV->p_stDynamicElementSpheres->d_stListOfSpheres + p_stGV->xDynamicIndexedSphereIndex;

        p_stGV->p_stDynamicCenter = p_stGV->p_stDynamicGeomObj->d_stListOfPoints + p_stGV->p_stDynamicIndexedSphere->xCenterPoint;

        POS_fn_vMulMatrixVertex ( &p_stGV->stDinST0Point, &p_stGV->stD2ST0TransformMatrix, p_stGV->p_stDynamicCenter );

        POS_fn_vMulMatrixVertex ( &p_stGV->stDinST1Point, &p_stGV->stD2ST1TransformMatrix, p_stGV->p_stDynamicCenter );
        MTH3D_M_vSubVector ( &p_stGV->stDinST01Vector, &p_stGV->stDinST1Point, &p_stGV->stDinST0Point );

		p_stGV->xDynamicRadius = MTH_M_xDiv ( p_stGV->p_stDynamicIndexedSphere->xRadius, p_stGV->xStaticScale );

        COL_fn_vEnlargeSphereIfNecessary (p_stGV);

        for ( p_stGV->xStaticIndexedTriangleIndex = 0 ; p_stGV->xStaticIndexedTriangleIndex < p_stGV->p_stStaticElementIndexedTriangles->xNbFaces ; p_stGV->xStaticIndexedTriangleIndex++ )
        {
          p_stGV->hDynamicMaterial = p_stGV->p_stDynamicIndexedSphere->hMaterial;
          p_stGV->hStaticMaterial = p_stGV->p_stStaticElementIndexedTriangles->hMaterial;
          COL_fn_bCollideStaticSphereWithStaticIndexedTriangle ( p_stGV , FALSE);

			for ( p_stGV->xStaticEdgeIndex = 0 ; p_stGV->xStaticEdgeIndex < 3 ; p_stGV->xStaticEdgeIndex++ )
			{
			  p_stGV->p_stVertex1 = p_stGV->p_stStaticGeomObj->d_stListOfPoints + (p_stGV->p_stStaticElementIndexedTriangles->d_stListOfFacesTripled + p_stGV->xStaticIndexedTriangleIndex)->a3_xIndex[p_stGV->xStaticEdgeIndex];
			  p_stGV->p_stVertex2 = p_stGV->p_stStaticGeomObj->d_stListOfPoints + (p_stGV->p_stStaticElementIndexedTriangles->d_stListOfFacesTripled + p_stGV->xStaticIndexedTriangleIndex)->a3_xIndex[(p_stGV->xStaticEdgeIndex+1)%3];
			  MTH3D_M_vSubVector ( &p_stGV->stEdgeVector, p_stGV->p_stVertex2, p_stGV->p_stVertex1 );

			  p_stGV->p_stVertex2 = p_stGV->p_stStaticGeomObj->d_stListOfPoints + (p_stGV->p_stStaticElementIndexedTriangles->d_stListOfFacesTripled + p_stGV->xStaticIndexedTriangleIndex)->a3_xIndex[(p_stGV->xStaticEdgeIndex+2)%3];

			  COL_fn_vCollideStaticSphereWithStaticEdge (p_stGV, p_stGV->p_stStaticElementIndexedTriangles->d_stListOfFacesNormals + p_stGV->xStaticIndexedTriangleIndex);

			}
        }
    }
  }
}

void __cdecl COL_fn_vCollideStaticSphereWithStaticSphere(COL_tdstGVForCollision *p_stGV)
{
    MTH_tdxReal xRadiusSum;

    p_stGV->p_stStaticCenter = p_stGV->p_stStaticGeomObj->d_stListOfPoints + p_stGV->p_stStaticIndexedSphere->xCenterPoint;

    xRadiusSum = MTH_M_xAdd(p_stGV->xDynamicRadius, p_stGV->p_stStaticIndexedSphere->xRadius);

    if (INT_fn_bGetInclusionPointInSphere(&p_stGV->stDinST0Point, p_stGV->p_stStaticCenter, xRadiusSum))
    {

        MTH3D_tdstVector stMovementVector;

        MTH3D_M_vNegVector(&stMovementVector, &p_stGV->stDinST01Vector);

        COL_fn_vUseInitialSphere(p_stGV);
        xRadiusSum = MTH_M_xAdd(p_stGV->xDynamicRadius, p_stGV->p_stStaticIndexedSphere->xRadius);

        if (INT_fn_bGetInclusionPointInSphere(&p_stGV->stDinST1Point, p_stGV->p_stStaticCenter, xRadiusSum))
        {

            MTH3D_tdstVector stCenterToCenter;
            MTH3D_tdstVector stTempNormal;
            MTH3D_tdstVector stTempHit;
            MTH3D_tdstVector stTempDistance;
            MTH3D_tdstVector stEndPosition;

            MTH3D_M_vSubVector(&stCenterToCenter, &p_stGV->stDinST0Point, p_stGV->p_stStaticCenter);
            POS_fn_vMulMatrixVector(&stCenterToCenter, p_stGV->p_stStaticGeomObjMatrix, &stCenterToCenter);
            MTH3D_M_vNormalizeVector(&stTempNormal, &stCenterToCenter);

            POS_fn_vMulMatrixVertex(&stTempHit, p_stGV->p_stStaticGeomObjMatrix, p_stGV->p_stStaticCenter);
            MTH3D_M_vMulAddVector(&stTempHit, p_stGV->p_stStaticIndexedSphere->xRadius, &stTempNormal, &stTempHit);

            /* 0x0049D0C4 */
            MTH3D_M_vMulScalarVector(&stTempDistance, MTH_M_xSub(xRadiusSum, (MTH_tdxReal)-0.001f), &stTempNormal);
            MTH3D_M_vSubVector(&stTempDistance, &stTempDistance, &stCenterToCenter);

            POS_fn_vMulMatrixVector(&stMovementVector, p_stGV->p_stStaticGeomObjMatrix, &stMovementVector);
            POS_fn_vMulMatrixVertex(&stEndPosition, p_stGV->p_stStaticGeomObjMatrix, &p_stGV->stDinST0Point);

            COL_fn_vAddInStaticCollisionTable(MTH_C_MinusONE, &stTempDistance, &stTempHit, &stTempNormal,
                                              p_stGV->hDynamicMaterial, p_stGV->hStaticMaterial,
                                              COL_C_xSphereEntity, COL_C_xSphereEntity,
                                              p_stGV->p_vParameter1, p_stGV->sParameter2,
                                              &stMovementVector, p_stGV->p_stDynamicIndexedSphere->xRadius, &stEndPosition);
        }
        else
        {

            MTH_tdxReal      xT;
            MTH3D_tdstVector stHit;
            MTH3D_tdstVector stNormal;

            MTH3D_tdstVector stTempHit;
            MTH3D_tdstVector stTempNormal;
            MTH3D_tdstVector stTempDistance;
            MTH3D_tdstVector stEndPosition;

            POS_fn_vMulMatrixVertex(&stEndPosition, p_stGV->p_stStaticGeomObjMatrix, &p_stGV->stDinST0Point);

            if (INT_fn_bIntersectSegmentWithSphere(&p_stGV->stDinST1Point, &stMovementVector,
                                                   p_stGV->p_stStaticCenter, xRadiusSum, &xT, &stHit, &stNormal))
            {

                MTH3D_M_vLinearInterpolVector(&stHit, &stHit, p_stGV->p_stStaticCenter,
                                              MTH_M_xDiv(p_stGV->xDynamicRadius, xRadiusSum));
                POS_fn_vMulMatrixVertex(&stTempHit, p_stGV->p_stStaticGeomObjMatrix, &stHit);

                MTH3D_M_vMulMatrixVector(&stTempNormal, &p_stGV->p_stStaticGeomObjMatrix->stRotationMatrix, &stNormal);

                MTH3D_M_vNullVector(&stTempDistance);

                POS_fn_vMulMatrixVector(&stMovementVector, p_stGV->p_stStaticGeomObjMatrix, &stMovementVector);

                COL_fn_vAddInStaticCollisionTable(xT, &stTempDistance, &stTempHit, &stTempNormal,
                                                  p_stGV->hDynamicMaterial, p_stGV->hStaticMaterial,
                                                  COL_C_xSphereEntity, COL_C_xSphereEntity,
                                                  p_stGV->p_vParameter1, p_stGV->sParameter2,
                                                  &stMovementVector, p_stGV->p_stDynamicIndexedSphere->xRadius, &stEndPosition);
            }
        }
    }
}

/* 0x0049A0E0 */
void COL_fn_vCollideStaticElementSpheresWithStaticElementSpheres ( COL_tdstGVForCollision *p_stGV )
{

  for ( p_stGV->xDynamicIndexedSphereIndex = 0 ; p_stGV->xDynamicIndexedSphereIndex < p_stGV->p_stDynamicElementSpheres->xNbSpheres ; p_stGV->xDynamicIndexedSphereIndex++ )
  {

    p_stGV->p_stDynamicIndexedSphere = p_stGV->p_stDynamicElementSpheres->d_stListOfSpheres + p_stGV->xDynamicIndexedSphereIndex;

    p_stGV->p_stDynamicCenter = p_stGV->p_stDynamicGeomObj->d_stListOfPoints + p_stGV->p_stDynamicIndexedSphere->xCenterPoint;

    p_stGV->xDynamicRadius = MTH_M_xDiv ( p_stGV->p_stDynamicIndexedSphere->xRadius, p_stGV->xStaticScale );

    POS_fn_vMulMatrixVertex ( &p_stGV->stDinST0Point, &p_stGV->stD2ST0TransformMatrix, p_stGV->p_stDynamicCenter );

    POS_fn_vMulMatrixVertex ( &p_stGV->stDinST1Point, &p_stGV->stD2ST1TransformMatrix, p_stGV->p_stDynamicCenter );
    MTH3D_M_vSubVector ( &p_stGV->stDinST01Vector, &p_stGV->stDinST1Point, &p_stGV->stDinST0Point );

    COL_fn_vEnlargeSphereIfNecessary (p_stGV);

    for ( p_stGV->xStaticIndexedSphereIndex = 0 ; p_stGV->xStaticIndexedSphereIndex < p_stGV->p_stStaticElementSpheres->xNbSpheres ; p_stGV->xStaticIndexedSphereIndex++ )
    {

      p_stGV->p_stStaticIndexedSphere = p_stGV->p_stStaticElementSpheres->d_stListOfSpheres + p_stGV->xStaticIndexedSphereIndex;

      p_stGV->hDynamicMaterial = p_stGV->p_stDynamicIndexedSphere->hMaterial;
      p_stGV->hStaticMaterial = p_stGV->p_stStaticIndexedSphere->hMaterial;
      COL_fn_vCollideStaticSphereWithStaticSphere ( p_stGV );
    }
  }
}

/* 0x0049A7D0 */
ACP_tdxBool COL_fn_bCollideStaticElementSpheresWithStaticBoundingVolumes( COL_tdstGVForCollision *p_stGV , long *_aDEF_lBitsFieldInCollision,
                                                                          ACP_tdxBool _bPartialResult)
{
	ACP_tdxIndex				xIndexBox , xIndexSphere;
	long						*p_lCurrentBitsField;
	GEO_tdxHandleToParallelBox	hBox;
	MTH3D_tdstVector			*p_stMinPointBox , *p_stMaxPointBox;
	MTH3D_tdstVector			*p_stCenter , stFirstCenter ,
								 stMinPointSphere , stMaxPointSphere ,
								 stMinPointLocalSphere , stMaxPointLocalSphere ;
	MTH_tdxReal					xRadius;
	ACP_tdxBool					bResult;

	bResult = FALSE;

	p_stCenter = p_stGV->p_stDynamicGeomObj->d_stListOfPoints + p_stGV -> p_stDynamicElementSpheres -> d_stListOfSpheres [ 0 ] . xCenterPoint;
	POS_fn_vMulMatrixVertex ( & stFirstCenter , & p_stGV -> stD2ST0TransformMatrix , p_stCenter );

	xRadius = MTH_M_xDiv ( p_stGV -> p_stDynamicElementSpheres -> d_stListOfSpheres [ 0 ] . xRadius, p_stGV -> xStaticScale );

	COL_fn_vComputeBoundingBoxOfSphere ( & stMinPointSphere , & stMaxPointSphere , & stFirstCenter , xRadius );

	for ( xIndexSphere = 1 ; xIndexSphere < p_stGV -> p_stDynamicElementSpheres -> xNbSpheres ; xIndexSphere++ )
	{

		p_stCenter = p_stGV->p_stDynamicGeomObj->d_stListOfPoints + p_stGV -> p_stDynamicElementSpheres -> d_stListOfSpheres [ xIndexSphere ] . xCenterPoint;
		POS_fn_vMulMatrixVertex ( & stFirstCenter , & p_stGV -> stD2ST0TransformMatrix , p_stCenter );

		xRadius = MTH_M_xDiv ( p_stGV -> p_stDynamicElementSpheres -> d_stListOfSpheres [ xIndexSphere ] . xRadius, p_stGV -> xStaticScale );

		COL_fn_vComputeBoundingBoxOfSphereAndBox ( & stMinPointSphere , & stMaxPointSphere , & stFirstCenter , xRadius ,
                                                   & stMinPointSphere , & stMaxPointSphere );
	}

	for ( xIndexBox = 0 , hBox = p_stGV->p_stStaticGeomObj->d_stListOfParallelBoxes , p_lCurrentBitsField = _aDEF_lBitsFieldInCollision ;
		  xIndexBox < p_stGV->p_stStaticGeomObj->xNbParallelBoxes ;
		  xIndexBox++ , hBox++, p_lCurrentBitsField++ )
	{
		*p_lCurrentBitsField = 0;

		p_stMinPointBox = & hBox -> stMinPoint ;
		p_stMaxPointBox = & hBox -> stMaxPoint ;

		if( COL_fn_bDetectCollisionBetweenTwoParallelBoxes( & stMaxPointSphere , & stMinPointSphere , p_stMaxPointBox , p_stMinPointBox ) )
		{
			for ( xIndexSphere = 0 ; xIndexSphere < p_stGV -> p_stDynamicElementSpheres -> xNbSpheres ; xIndexSphere++ )
			{

				p_stCenter = p_stGV->p_stDynamicGeomObj->d_stListOfPoints + p_stGV -> p_stDynamicElementSpheres -> d_stListOfSpheres [ xIndexSphere ] . xCenterPoint;
				POS_fn_vMulMatrixVertex ( & stFirstCenter , & p_stGV -> stD2ST0TransformMatrix , p_stCenter );

				xRadius = MTH_M_xDiv ( p_stGV -> p_stDynamicElementSpheres -> d_stListOfSpheres [ xIndexSphere ] . xRadius, p_stGV -> xStaticScale );

				COL_fn_vComputeBoundingBoxOfSphere ( & stMinPointLocalSphere , & stMaxPointLocalSphere , & stFirstCenter , xRadius );
				if( COL_fn_bDetectCollisionBetweenTwoParallelBoxes( & stMaxPointLocalSphere , & stMinPointLocalSphere , p_stMaxPointBox , p_stMinPointBox ) )
				{
					COL_M_vSetBitOfLong( *p_lCurrentBitsField, xIndexSphere );
					bResult = TRUE;
                    if (_bPartialResult)  return TRUE;
				}
			}
		}
	}
	return bResult;
}

/* 0x0049A250 */
void COL_fn_vCollideStaticElementSpheresWithStaticOctree ( COL_tdstGVForCollision *p_stGV )
{
	COL_tdxFaceIndex	*p_xListIndex;

	ACP_tdxIndex		xNodeIndex;
	COL_tdxFaceIndex	xIndex;
	ACP_tdxIndex		xDataElementIndex;

	MTH3D_tdstVector	stMinPoint;
	MTH3D_tdstVector	stMaxPoint;
	ACP_tdxIndex		xNbElements;

	for ( p_stGV->xDynamicIndexedSphereIndex = 0 ; p_stGV->xDynamicIndexedSphereIndex < p_stGV->p_stDynamicElementSpheres->xNbSpheres ; p_stGV->xDynamicIndexedSphereIndex++ )
	{

		p_stGV->p_stDynamicIndexedSphere = p_stGV->p_stDynamicElementSpheres->d_stListOfSpheres + p_stGV->xDynamicIndexedSphereIndex;

		p_stGV->p_stDynamicCenter = p_stGV->p_stDynamicGeomObj->d_stListOfPoints + p_stGV->p_stDynamicIndexedSphere->xCenterPoint;

		p_stGV->xDynamicRadius = MTH_M_xDiv ( p_stGV->p_stDynamicIndexedSphere->xRadius, p_stGV->xStaticScale );

		POS_fn_vMulMatrixVertex ( &p_stGV->stDinST0Point, &p_stGV->stD2ST0TransformMatrix, p_stGV->p_stDynamicCenter );

		POS_fn_vMulMatrixVertex ( &p_stGV->stDinST1Point, &p_stGV->stD2ST1TransformMatrix, p_stGV->p_stDynamicCenter );
		MTH3D_M_vSubVector ( &p_stGV->stDinST01Vector, &p_stGV->stDinST1Point, &p_stGV->stDinST0Point );

		COL_fn_vEnlargeSphereIfNecessary (p_stGV);

		COL_fn_vComputeBoundingBoxOfSphere ( &stMinPoint, &stMaxPoint, &p_stGV->stDinST0Point, p_stGV->xDynamicRadius );

		COL_fn_vExploreOctreeWithBox ( p_stGV->p_stOctree, &stMinPoint, &stMaxPoint,
			p_stGV->aDEF_pstSelectedNode, &p_stGV->xNumberOfSelectedNodes );

		COL_g_lFacesTagCounter ++;

		for ( xNodeIndex = 0 ; xNodeIndex < p_stGV->xNumberOfSelectedNodes ; xNodeIndex++ )
		{

			xNbElements  = *((COL_tdxFaceIndexDouble*)p_stGV->aDEF_pstSelectedNode[xNodeIndex]->d_xFaceIndexList);
			p_xListIndex = (COL_tdxFaceIndex*)(((COL_tdxFaceIndexDouble*)p_stGV->aDEF_pstSelectedNode[xNodeIndex]->d_xFaceIndexList)+1);

			for (xIndex = 0 ; xIndex < xNbElements ; xIndex++)
			{
				if ((*p_xListIndex) < COL_C_MaxIndex)
					p_stGV->xStaticElementIndex = (ACP_tdxIndex) (* (p_xListIndex ++));
				else
				{
					ACP_tdxIndex xData;

					xData=(ACP_tdxIndex)(* (p_xListIndex ++));
					xData<<=8;
					xData+=(ACP_tdxIndex)(* (p_xListIndex ++));
					xData&=~COL_C_OverflowIndex;
					p_stGV->xStaticElementIndex = xData;
				}

				if ((*p_xListIndex) < COL_C_MaxIndex)
					xDataElementIndex = (ACP_tdxIndex) (* (p_xListIndex ++));
				else
				{
					ACP_tdxIndex xData;

					xData=(ACP_tdxIndex)(* (p_xListIndex ++));
					xData<<=8;
					xData+=(ACP_tdxIndex)(* (p_xListIndex ++));
					xData&=~COL_C_OverflowIndex;
					xDataElementIndex = xData;
				}

				if
					(
					COL_g_d_lTaggedFacesTable[p_stGV->p_stOctree->d_xElementBasesTable[p_stGV->xStaticElementIndex] + xDataElementIndex]
					!= COL_g_lFacesTagCounter
					)
				{
					switch ( p_stGV->p_stStaticGeomObj->d_xListOfElementsTypes[p_stGV->xStaticElementIndex] )
					{
					case GEO_C_xElementIndexedTriangles :
						if ( p_stGV->ulSelectedCollisionCases & COL_C_ulDynamicSphWithStaticITr )
						{
							p_stGV->p_stStaticElementIndexedTriangles = (GEO_tdstElementIndexedTriangles *)(p_stGV->p_stStaticGeomObj->d_stListOfElements[p_stGV->xStaticElementIndex]);

							p_stGV->xStaticIndexedTriangleIndex = xDataElementIndex;

							p_stGV->hDynamicMaterial = p_stGV->p_stDynamicIndexedSphere->hMaterial;
							p_stGV->hStaticMaterial = p_stGV->p_stStaticElementIndexedTriangles->hMaterial;

							if (COL_fn_bCollideStaticSphereWithStaticIndexedTriangle ( p_stGV , FALSE))  break;
						}

						if ( p_stGV->ulSelectedCollisionCases & COL_C_ulDynamicSphWithStaticEdg )
						{

							for ( p_stGV->xStaticEdgeIndex = 0 ; p_stGV->xStaticEdgeIndex < 3 ; p_stGV->xStaticEdgeIndex++ )
							{
								p_stGV->p_stVertex1 = p_stGV->p_stStaticGeomObj->d_stListOfPoints + (p_stGV->p_stStaticElementIndexedTriangles->d_stListOfFacesTripled + xDataElementIndex)->a3_xIndex[p_stGV->xStaticEdgeIndex];
								p_stGV->p_stVertex2 = p_stGV->p_stStaticGeomObj->d_stListOfPoints + (p_stGV->p_stStaticElementIndexedTriangles->d_stListOfFacesTripled + xDataElementIndex)->a3_xIndex[(p_stGV->xStaticEdgeIndex+1)%3];
								MTH3D_M_vSubVector ( &p_stGV->stEdgeVector, p_stGV->p_stVertex2, p_stGV->p_stVertex1 );

								p_stGV->p_stVertex2 = p_stGV->p_stStaticGeomObj->d_stListOfPoints + (p_stGV->p_stStaticElementIndexedTriangles->d_stListOfFacesTripled + xDataElementIndex)->a3_xIndex[(p_stGV->xStaticEdgeIndex+2)%3];

								p_stGV->hDynamicMaterial = p_stGV->p_stDynamicIndexedSphere->hMaterial;
								p_stGV->hStaticMaterial = p_stGV->p_stStaticElementIndexedTriangles->hMaterial;
								COL_fn_vCollideStaticSphereWithStaticEdge (p_stGV, p_stGV->p_stStaticElementIndexedTriangles->d_stListOfFacesNormals + p_stGV->xStaticIndexedTriangleIndex);
							}
						}
						break;

					case GEO_C_xElementSpheres :
						if ( p_stGV->ulSelectedCollisionCases & COL_C_ulDynamicSphWithStaticSph )
						{
							p_stGV->p_stStaticElementSpheres = (GEO_tdstElementSpheres *)(p_stGV->p_stStaticGeomObj->d_stListOfElements[p_stGV->xStaticElementIndex]);

							p_stGV->xStaticIndexedSphereIndex = xDataElementIndex;

							p_stGV->p_stStaticIndexedSphere = p_stGV->p_stStaticElementSpheres->d_stListOfSpheres + p_stGV->xStaticIndexedSphereIndex;

							p_stGV->hDynamicMaterial = p_stGV->p_stDynamicIndexedSphere->hMaterial;
							p_stGV->hStaticMaterial = p_stGV->p_stStaticIndexedSphere->hMaterial;
							COL_fn_vCollideStaticSphereWithStaticSphere ( p_stGV );
						}
						break;
					}

					COL_g_d_lTaggedFacesTable[p_stGV->p_stOctree->d_xElementBasesTable[p_stGV->xStaticElementIndex] + xDataElementIndex]
						= COL_g_lFacesTagCounter;
				}
				}
    }
  }
}

void __cdecl
COL_fn_vCollideStaticElementSpheresWithStaticElements(COL_tdstGVForCollision *p_stGV)
{
    long         a_DEF_lBitsFieldOfCollisionsWithBoundingBox[COL_C_xMaxStaticBoundingBoxes];
    ACP_tdxIndex xIndexBox;

    if (p_stGV->p_stStaticGeomObj->xNbParallelBoxes &&
        !COL_fn_bCollideStaticElementSpheresWithStaticBoundingVolumes(
            p_stGV, a_DEF_lBitsFieldOfCollisionsWithBoundingBox,
            (ACP_tdxBool)(p_stGV->p_stOctree != NULL)))
        return;

    if (p_stGV->p_stOctree == NULL)
    {
        for (p_stGV->xStaticElementIndex = 0;
             p_stGV->xStaticElementIndex < p_stGV->p_stStaticGeomObj->xNbElements;
             p_stGV->xStaticElementIndex++)
        {
            if (p_stGV->p_stStaticGeomObj->xNbParallelBoxes)
            {
                xIndexBox = GEO_xGetParallelBoxIndexOfElement(p_stGV->p_stStaticGeomObj, p_stGV->xStaticElementIndex);
                p_stGV->lBitsFieldOfIndexedSpheresInCollision =
                    (xIndexBox == GEO_C_xNoParallelBox) ? COL_C_lUsesAllElements
                                                        : a_DEF_lBitsFieldOfCollisionsWithBoundingBox[xIndexBox];
                if (p_stGV->lBitsFieldOfIndexedSpheresInCollision == 0)
                    continue;
            }
            else
            {
                p_stGV->lBitsFieldOfIndexedSpheresInCollision = COL_C_lUsesAllElements;
            }

            switch (p_stGV->p_stStaticGeomObj->d_xListOfElementsTypes[p_stGV->xStaticElementIndex])
            {
            case GEO_C_xElementIndexedTriangles:
                if (p_stGV->ulSelectedCollisionCases & COL_C_ulDynamicSphWithStaticITr)
                {
                    p_stGV->p_stStaticElementIndexedTriangles =
                        (GEO_tdstElementIndexedTriangles *)(p_stGV->p_stStaticGeomObj->d_stListOfElements[p_stGV->xStaticElementIndex]);
                    COL_fn_vCollideStaticElementSpheresWithStaticElementIndexedTriangles(p_stGV);
                }
                break;
            case GEO_C_xElementSpheres:
                if (p_stGV->ulSelectedCollisionCases & COL_C_ulDynamicSphWithStaticSph)
                {
                    p_stGV->p_stStaticElementSpheres =
                        (GEO_tdstElementSpheres *)(p_stGV->p_stStaticGeomObj->d_stListOfElements[p_stGV->xStaticElementIndex]);
                    COL_fn_vCollideStaticElementSpheresWithStaticElementSpheres(p_stGV);
                }
                break;
            }
        }
    }
    else
    {
        if (p_stGV->ulSelectedCollisionCases & COL_C_ulDynamicSphWithStaticElt)
        {
            COL_fn_vCollideStaticElementSpheresWithStaticOctree(p_stGV);
        }
    }
}
