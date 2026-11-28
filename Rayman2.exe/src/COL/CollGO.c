#include <stddef.h>
#include "COL/CollGO.h"
#include "COL/CollGOUt.h"
#include "COL/CsSphElt.h"
#include "MTH/MTH_flt.h"
#include "MTH/MTH3D.h"
#include "GLI/geometric_helpers.h"

#define COL_CT_ASSERT(tag, cond) typedef char tag[(cond) ? 1 : -1]
COL_CT_ASSERT(gv_off_noTM,    offsetof(COL_tdstGVForCollision, bStaticGeomObjHasNoTransformationMatrix) == 0x000);
COL_CT_ASSERT(gv_off_dynObj,  offsetof(COL_tdstGVForCollision, p_stDynamicGeomObj)        == 0x004);
COL_CT_ASSERT(gv_off_t0,      offsetof(COL_tdstGVForCollision, p_stT0DynamicGeomObjMatrix) == 0x008);
COL_CT_ASSERT(gv_off_t1,      offsetof(COL_tdstGVForCollision, p_stT1DynamicGeomObjMatrix) == 0x00C);
COL_CT_ASSERT(gv_off_statObj, offsetof(COL_tdstGVForCollision, p_stStaticGeomObj)         == 0x010);
COL_CT_ASSERT(gv_off_statMat, offsetof(COL_tdstGVForCollision, p_stStaticGeomObjMatrix)   == 0x014);
COL_CT_ASSERT(gv_off_cases,   offsetof(COL_tdstGVForCollision, ulSelectedCollisionCases)  == 0x018);
COL_CT_ASSERT(gv_off_inv,     offsetof(COL_tdstGVForCollision, stInvMatrix)               == 0x024);
COL_CT_ASSERT(gv_off_d2st0,   offsetof(COL_tdstGVForCollision, stD2ST0TransformMatrix)    == 0x07C);
COL_CT_ASSERT(gv_off_d2st1,   offsetof(COL_tdstGVForCollision, stD2ST1TransformMatrix)    == 0x0D4);
COL_CT_ASSERT(gv_off_s2dt0,   offsetof(COL_tdstGVForCollision, stS2DT0TransformMatrix)    == 0x12C);
COL_CT_ASSERT(gv_off_s2dt1,   offsetof(COL_tdstGVForCollision, stS2DT1TransformMatrix)    == 0x184);
COL_CT_ASSERT(gv_off_scale,   offsetof(COL_tdstGVForCollision, xStaticScale)              == 0x1DC);
COL_CT_ASSERT(gv_off_octree,  offsetof(COL_tdstGVForCollision, p_stOctree)                == 0x608);
COL_CT_ASSERT(gv_off_surf,    offsetof(COL_tdstGVForCollision, bSurfacicDynamicElement)   == 0x930);
COL_CT_ASSERT(gv_off_sphonly, offsetof(COL_tdstGVForCollision, bDynamicSphereAndPointOnly) == 0x931);
COL_CT_ASSERT(gv_off_nbpts,   offsetof(COL_tdstGVForCollision, xNbStaticPoints)           == 0x932);
COL_CT_ASSERT(gv_off_nbedg,   offsetof(COL_tdstGVForCollision, xNbStaticEdges)            == 0xD1C);
COL_CT_ASSERT(gv_off_dyneltidx, offsetof(COL_tdstGVForCollision, xDynamicElementIndex)    == 0x1E8);
COL_CT_ASSERT(gv_off_dyneltsph, offsetof(COL_tdstGVForCollision, p_stDynamicElementSpheres) == 0x20C);

static COL_tdstGVForCollision COL_gs_stGVForCol;   /* 0x004F6F80 */

ACP_tdxBool __cdecl COL_fn_bDetectCollisionStaticGeomObjWithStaticGeomObj(
    GEO_tdxHandleToBoundingSphere  _hDynamicGeometricObject,
    POS_tdstCompletePosition      *_p_stDynamicGeometricObjectMatrix,
    void                          *_hStaticGeometricObject,
    POS_tdstCompletePosition      *_p_stStaticGeometricObjectMatrix,
    ACP_tdxBool                    _bStaticGeomObjHasNoTransformationMatrix,
    ACP_tdxBool                    _bDynamicGeomObjHasZoomInsteadOfScale,
    ACP_tdxBool                    _bHasABoxBoundingVolume)
{
    MTH3D_tdstVector  stDinGPoint;
    MTH3D_tdstVector  stSinGPoint;
    MTH3D_tdstVector *p_stSMaxBoxPoint;
    MTH3D_tdstVector *p_stSMinBoxPoint;
    MTH3D_tdstVector  stMaxDinGPoint;
    MTH3D_tdstVector  stMinDinGPoint;
    MTH3D_tdstVector  stMaxSinGPoint;
    MTH3D_tdstVector  stMinSinGPoint;

    MTH_tdxReal xRadiusSum;
    MTH_tdxReal xRadiusDinG;

    (void)_bDynamicGeomObjHasZoomInsteadOfScale;

    POS_fn_vMulMatrixVertex(&stDinGPoint, _p_stDynamicGeometricObjectMatrix,
                            GEO_fn_pGetCenterPointOfBoundingSphere(_hDynamicGeometricObject));

    if (!_bHasABoxBoundingVolume)
    {

        if (_bStaticGeomObjHasNoTransformationMatrix)
        {
            MTH3D_M_vAddVector(&stSinGPoint,
                GEO_fn_pGetCenterPointOfBoundingSphere((GEO_tdxHandleToBoundingSphere)_hStaticGeometricObject),
                POS_fn_vGetTranslationVectorReadOnly(_p_stStaticGeometricObjectMatrix));
        }
        else
        {
            POS_fn_vMulMatrixVertex(&stSinGPoint, _p_stStaticGeometricObjectMatrix,
                GEO_fn_pGetCenterPointOfBoundingSphere((GEO_tdxHandleToBoundingSphere)_hStaticGeometricObject));
        }

        xRadiusSum = MTH_M_xAdd(

            MTH_M_xMul(GEO_fn_xGetRadiusOfBoundingSphere(_hDynamicGeometricObject),
                       POS_fn_xGetMaxScale(_p_stDynamicGeometricObjectMatrix)),

            _bStaticGeomObjHasNoTransformationMatrix
                ? GEO_fn_xGetRadiusOfBoundingSphere((GEO_tdxHandleToBoundingSphere)_hStaticGeometricObject)
                : MTH_M_xMul(GEO_fn_xGetRadiusOfBoundingSphere((GEO_tdxHandleToBoundingSphere)_hStaticGeometricObject),
                             POS_fn_xGetMaxScale(_p_stStaticGeometricObjectMatrix)));

        return MTH_M_bLessEqual(MTH3D_M_xVectorGapSqr(&stDinGPoint, &stSinGPoint),
                                MTH_M_xSqr(xRadiusSum));
    }
    else
    {

        p_stSMaxBoxPoint = GEO_fn_pGetMaxPointOfParallelBox((GEO_tdxHandleToParallelBox)_hStaticGeometricObject);
        p_stSMinBoxPoint = GEO_fn_pGetMinPointOfParallelBox((GEO_tdxHandleToParallelBox)_hStaticGeometricObject);

        MTH3D_M_vAddVector(&stMaxSinGPoint,
            POS_fn_vGetTranslationVectorReadOnly(_p_stStaticGeometricObjectMatrix), p_stSMaxBoxPoint);
        MTH3D_M_vAddVector(&stMinSinGPoint,
            POS_fn_vGetTranslationVectorReadOnly(_p_stStaticGeometricObjectMatrix), p_stSMinBoxPoint);

        xRadiusDinG = MTH_M_xMul(GEO_fn_xGetRadiusOfBoundingSphere(_hDynamicGeometricObject),
                                 POS_fn_xGetMaxScale(_p_stDynamicGeometricObjectMatrix));
        COL_fn_vComputeBoundingBoxOfSphere(&stMinDinGPoint, &stMaxDinGPoint, &stDinGPoint, xRadiusDinG);

        return COL_fn_bDetectCollisionBetweenTwoParallelBoxes(&stMaxSinGPoint, &stMinSinGPoint,
                                                              &stMaxDinGPoint, &stMinDinGPoint);
    }
}

void __cdecl COL_fn_vInitCollideStaticGeomObjWithStaticGeomObj(COL_tdstGVForCollision *p_stGV)
{

    POS_fn_vSetIdentityMatrix(&p_stGV->stInvMatrix);
    POS_fn_vSetIdentityMatrix(&p_stGV->stD2ST0TransformMatrix);
    POS_fn_vSetIdentityMatrix(&p_stGV->stD2ST1TransformMatrix);
    POS_fn_vSetIdentityMatrix(&p_stGV->stS2DT0TransformMatrix);
    POS_fn_vSetIdentityMatrix(&p_stGV->stS2DT1TransformMatrix);

    POS_fn_vInvertMatrix(&p_stGV->stInvMatrix, p_stGV->p_stStaticGeomObjMatrix);
    POS_fn_vMulMatrixMatrix(&p_stGV->stD2ST0TransformMatrix, &p_stGV->stInvMatrix, p_stGV->p_stT0DynamicGeomObjMatrix);
    POS_fn_vMulMatrixMatrix(&p_stGV->stD2ST1TransformMatrix, &p_stGV->stInvMatrix, p_stGV->p_stT1DynamicGeomObjMatrix);

    p_stGV->bSurfacicDynamicElement = COL_fn_bGetSurfaceInGeometricObject(p_stGV->p_stDynamicGeomObj);
    if (!p_stGV->bSurfacicDynamicElement)
    {
        p_stGV->ulSelectedCollisionCases &= ~COL_C_ulDynamicEltWithStaticPts;
        p_stGV->ulSelectedCollisionCases &= ~COL_C_ulDynamicEdgWithStaticEdg;
        p_stGV->ulSelectedCollisionCases &= ~COL_C_ulDynamicEdgWithStaticElt;
        p_stGV->ulSelectedCollisionCases &= ~COL_C_ulDynamicEltWithStaticEdg;
        p_stGV->ulSelectedCollisionCases &= ~COL_C_ulDynamicEltWithStaticElt;
    }

    p_stGV->bDynamicSphereAndPointOnly = COL_fn_bGetSphereAndPointOnlyInGeometricObject(p_stGV->p_stDynamicGeomObj);

    p_stGV->p_stOctree = p_stGV->p_stStaticGeomObj->p_stOctree;
    if (p_stGV->p_stOctree != NULL)
    {
        p_stGV->xNbStaticPoints = 0;
        p_stGV->xNbStaticEdges  = 0;
    }

    if (p_stGV->bStaticGeomObjHasNoTransformationMatrix)
    {
        p_stGV->xStaticScale = MTH_C_ONE;
    }
    else
    {
        p_stGV->xStaticScale = POS_fn_xGetMaxScale(p_stGV->p_stStaticGeomObjMatrix);
    }
}

void __cdecl COL_fn_vComputeCollisionStaticGeomObjWithStaticGeomObj(COL_tdstGVForCollision *p_stGV)
{
    COL_fn_vInitCollideStaticGeomObjWithStaticGeomObj(p_stGV);

    if (p_stGV->ulSelectedCollisionCases & COL_C_ulDynamicEltWithStaticElt)
    {
        for (p_stGV->xDynamicElementIndex = 0;
             p_stGV->xDynamicElementIndex < p_stGV->p_stDynamicGeomObj->xNbElements;
             p_stGV->xDynamicElementIndex++)
        {
            switch (p_stGV->p_stDynamicGeomObj->d_xListOfElementsTypes[p_stGV->xDynamicElementIndex])
            {
            case GEO_C_xElementSpheres:
                if (p_stGV->ulSelectedCollisionCases & COL_C_ulDynamicSphWithStaticElt)
                {
                    p_stGV->p_stDynamicElementSpheres =
                        (GEO_tdstElementSpheres *)(p_stGV->p_stDynamicGeomObj->d_stListOfElements[p_stGV->xDynamicElementIndex]);

                    if (((GEO_tdstElementSpheres *)p_stGV->p_stDynamicElementSpheres)->xNbSpheres)
                        COL_fn_vCollideStaticElementSpheresWithStaticElements(p_stGV);
                }
                break;
            }
        }
    }
}
#define COL_g_stGVForCol  COL_gs_stGVForCol

void __cdecl COL_fn_vCollideStaticGeomObj1WithStaticGeomObj2(
    ACP_tdxHandleOfObject     _hGeometricObject1,
    POS_tdstCompletePosition *_p_stGeometricObjectStartMatrix1,
    POS_tdstCompletePosition *_p_stGeometricObjectEndMatrix1,
    ACP_tdxHandleOfObject     _hGeometricObject2,
    POS_tdstCompletePosition *_p_stGeometricObjectMatrix2,
    void                     *_p_vParameter1,
    short                     _sParameter2,
    ACP_tdxBool               _bStaticGeomObjHasNoTransformationMatrix,
    ACP_tdxBool               _bDynamicGeomObjHasZoomInsteadOfScale)
{
    COL_g_stGVForCol.bStaticGeomObjHasNoTransformationMatrix = _bStaticGeomObjHasNoTransformationMatrix;
    COL_g_stGVForCol.bDynamicGeomObjHasZoomInsteadOfScale    = _bDynamicGeomObjHasZoomInsteadOfScale;
    COL_g_stGVForCol.p_stDynamicGeomObj                      = _hGeometricObject1;
    COL_g_stGVForCol.p_stT0DynamicGeomObjMatrix             = _p_stGeometricObjectEndMatrix1;
    COL_g_stGVForCol.p_stT1DynamicGeomObjMatrix             = _p_stGeometricObjectStartMatrix1;
    COL_g_stGVForCol.p_stStaticGeomObj                      = _hGeometricObject2;
    COL_g_stGVForCol.p_stStaticGeomObjMatrix               = _p_stGeometricObjectMatrix2;
    COL_g_stGVForCol.ulSelectedCollisionCases              = COL_C_ulDynamicAllWithStaticAll;
    COL_g_stGVForCol.p_vParameter1                         = _p_vParameter1;
    COL_g_stGVForCol.sParameter2                           = _sParameter2;

    if (_hGeometricObject1 == NULL ||
        _hGeometricObject2 == NULL ||
        _hGeometricObject1->xNbPoints   == 0 ||
        _hGeometricObject2->xNbPoints   == 0 ||
        _hGeometricObject1->xNbElements == 0 ||
        _hGeometricObject2->xNbElements == 0)
    {
        return;
    }

    COL_fn_vComputeCollisionStaticGeomObjWithStaticGeomObj(&COL_g_stGVForCol);
}
