#include "GEO/GEO_types.h"
#include "GEO/GeoObj.h"
#include "MTH/MTH_flt.h"
#include "MTH/MTH3D.h"
#include "POS/POS_types.h"
#include "HIE/HIE_types.h"
#include "MMG/MMG_alloc.h"
#include "GEO/GeoMem.h"
#include "GEO/GEO_globals.h"
#include "GLD/GLD_Pub.h"
#include "GLI/Vpt3D.h"
#include "POS/PosProto.h"

static float GEO_g_xZFar_storage;                       /* 0x4a7204 */
static float GEO_g_xZFarTransparencyZone_storage   = 10.0f; /* 0x49fc04 */
static float GEO_g_xOoZFarTransparencyZone_storage = 0.1f;  /* 0x49fc08 */
/* 0x4a71a8 */
static MTH3D_tdstVector gs_stGlobalNormPlaneLeft;
static MTH3D_tdstVector gs_stGlobalNormPlaneRight;
static MTH3D_tdstVector gs_stGlobalNormPlaneUp;
static MTH3D_tdstVector gs_stGlobalNormPlaneDown;
static MTH3D_tdstVector gs_stGlobalNormPlaneNear;
static MTH3D_tdstVector gs_stCameraPosInGlobal;
#define s_xCamRefX_storage (gs_stCameraPosInGlobal.xX)
#define s_xCamRefY_storage (gs_stCameraPosInGlobal.xY)
#define s_xCamRefZ_storage (gs_stCameraPosInGlobal.xZ)
#define s_xNearNX_storage  (gs_stGlobalNormPlaneNear.xX)
#define s_xNearNY_storage  (gs_stGlobalNormPlaneNear.xY)
#define s_xNearNZ_storage  (gs_stGlobalNormPlaneNear.xZ)
#define s_xLeftNX_storage  (gs_stGlobalNormPlaneLeft.xX)
#define s_xLeftNY_storage  (gs_stGlobalNormPlaneLeft.xY)
#define s_xLeftNZ_storage  (gs_stGlobalNormPlaneLeft.xZ)
#define s_xRightNX_storage (gs_stGlobalNormPlaneRight.xX)
#define s_xRightNY_storage (gs_stGlobalNormPlaneRight.xY)
#define s_xRightNZ_storage (gs_stGlobalNormPlaneRight.xZ)
#define s_xUpNX_storage    (gs_stGlobalNormPlaneUp.xX)
#define s_xUpNY_storage    (gs_stGlobalNormPlaneUp.xY)
#define s_xUpNZ_storage    (gs_stGlobalNormPlaneUp.xZ)
#define s_xDownNX_storage  (gs_stGlobalNormPlaneDown.xX)
#define s_xDownNY_storage  (gs_stGlobalNormPlaneDown.xY)
#define s_xDownNZ_storage  (gs_stGlobalNormPlaneDown.xZ)
#define GEO_g_xZFar                   (GEO_g_xZFar_storage)
#define GEO_g_xZFarTransparencyZone   (GEO_g_xZFarTransparencyZone_storage)
#define GEO_g_xOoZFarTransparencyZone (GEO_g_xOoZFarTransparencyZone_storage)
#define s_xCamRefX   (s_xCamRefX_storage)
#define s_xCamRefY   (s_xCamRefY_storage)
#define s_xCamRefZ   (s_xCamRefZ_storage)
#define s_xNearNX    (s_xNearNX_storage)
#define s_xNearNY    (s_xNearNY_storage)
#define s_xNearNZ    (s_xNearNZ_storage)
#define s_xLeftNX    (s_xLeftNX_storage)
#define s_xLeftNY    (s_xLeftNY_storage)
#define s_xLeftNZ    (s_xLeftNZ_storage)
#define s_xRightNX   (s_xRightNX_storage)
#define s_xRightNY   (s_xRightNY_storage)
#define s_xRightNZ   (s_xRightNZ_storage)
#define s_xUpNX      (s_xUpNX_storage)
#define s_xUpNY      (s_xUpNY_storage)
#define s_xUpNZ      (s_xUpNZ_storage)
#define s_xDownNX    (s_xDownNX_storage)
#define s_xDownNY    (s_xDownNY_storage)
#define s_xDownNZ    (s_xDownNZ_storage)
#define s_xConst0    (0.0f)
#define s_xConst1    (1.0f)

extern POS_tdstCompletePosition *HIE_hRetrieveCurrentMatrix(void);

static long GEO_lMainCullingSphere
(
	struct GLD_tdstViewportAttributes_	*p_stVpt ,
	MTH3D_tdstVector			*p_stShereCenter ,
	MTH_tdxReal					xRadius,
	POS_tdstCompletePosition	*p_stMatrix,
	POS_tdstCompletePosition	*p_stMatrixW,
	long _lCullingResult
)
{

  GLI_tdstSpecificAttributesFor3D	*p_stSpecifAttrib3D;
	MTH3D_tdstVector				stCenter;
	GLI_tdstCamera					*hCamera;
#ifdef RAYMAN2_FIX_BUGS
	/* FIX(BUGS.md#2) */
	/* only set below when the near result is not known yet, read by the far test either way;
	   retail then reads the bits of p_stMatrix, a tiny positive float: not beyond the far plane */
	MTH_tdxReal						xDotProduct = MTH_C_EpsilonPlus;
#else
	MTH_tdxReal						xDotProduct;
#endif /* RAYMAN2_FIX_BUGS */

	if (!POS_fn_ulIsNotScaledMatrix ( p_stMatrix ))
	{

		xRadius = MTH_M_xMul(xRadius,POS_fn_xGetMaxScale(p_stMatrix));
	}

	p_stSpecifAttrib3D = (GLI_tdstSpecificAttributesFor3D *) p_stVpt -> p_vSpecificToXD;
	hCamera = p_stSpecifAttrib3D->p_stCam;

	POS_fn_vMulMatrixVertex (&stCenter,p_stMatrix,p_stShereCenter);

	_lCullingResult |= GEO_C_lCullingIntersect;

	if ( !(_lCullingResult & GEO_C_lCullingNear) )
	{
		if ( MTH_M_bLess(MTH_M_xAdd(stCenter.xZ, xRadius), hCamera->xNear ))
		{
			return GEO_C_lCullingOut;
		}

		xDotProduct = MTH_M_xSub(stCenter.xZ, xRadius);

		if ( MTH_M_bGreater(xDotProduct, hCamera->xNear) )
		{
			_lCullingResult |= GEO_C_lCullingNear;
		}
	}

	if ( !(_lCullingResult & GEO_C_lCullingFar) )
	{
		if ( MTH_M_bDifferentZero(GEO_g_xZFar) )
		{
			if ( xDotProduct > GEO_g_xZFar )
			{
				GEO_g_xZFarTransparencyLevel = MTH_C_ZERO;
				return GEO_C_lCullingOut;
			}

			if ( MTH_M_bGreater(xDotProduct, MTH_M_xSub(GEO_g_xZFar, GEO_g_xZFarTransparencyZone )))
			{
				GEO_g_xZFarTransparencyLevel = MTH_M_xSub(MTH_C_ONE, (MTH_M_xMul(MTH_M_xAdd(MTH_M_xSub(MTH_M_xSub(stCenter.xZ, xRadius), GEO_g_xZFar), GEO_g_xZFarTransparencyZone), GEO_g_xOoZFarTransparencyZone)));
			}
			else
			{
				GEO_g_xZFarTransparencyLevel = MTH_C_ONE;

				_lCullingResult |= GEO_C_lCullingFar;
			}
		}
		else
		{
			GEO_g_xZFarTransparencyLevel = MTH_C_ONE;

			_lCullingResult |= GEO_C_lCullingFar;
		}
	}
	else
	{
		GEO_g_xZFarTransparencyLevel = MTH_C_ONE;
	}

	if ( !(_lCullingResult & GEO_C_lCullingLeft) )
	{
		xDotProduct = MTH3D_M_xDotProductVector(&stCenter, &hCamera->stNormPlaneRight);
		if (MTH_M_bGreater(xDotProduct, MTH_M_xAdd(xRadius, hCamera->xDistPlaneLeft)))
		{
			return GEO_C_lCullingOut;
		}
		if (MTH_M_bLess(xDotProduct, MTH_M_xSub(hCamera->xDistPlaneLeft, xRadius)))
		{
			_lCullingResult |= GEO_C_lCullingLeft;
		}
	}

	if ( !(_lCullingResult & GEO_C_lCullingRight) )
	{
		xDotProduct = MTH3D_M_xDotProductVector(&stCenter, &hCamera->stNormPlaneLeft);
		if (MTH_M_bGreater(xDotProduct, MTH_M_xAdd(xRadius, hCamera->xDistPlaneRight)))
		{
			return GEO_C_lCullingOut;
		}
		if (MTH_M_bLess(xDotProduct, MTH_M_xSub(hCamera->xDistPlaneRight, xRadius)))
		{
			_lCullingResult |= GEO_C_lCullingRight;
		}
	}

	if ( !(_lCullingResult & GEO_C_lCullingUp) )
	{
		xDotProduct = MTH3D_M_xDotProductVector(&stCenter, &hCamera->stNormPlaneUp);
		if (MTH_M_bGreater(xDotProduct, MTH_M_xAdd(xRadius, hCamera->xDistPlaneUp)))
		{
			return GEO_C_lCullingOut;
		}
		if (MTH_M_bLess(xDotProduct, MTH_M_xSub(hCamera->xDistPlaneUp, xRadius)))
		{
			_lCullingResult |= GEO_C_lCullingUp;
		}
	}

	if ( !(_lCullingResult & GEO_C_lCullingDown) )
	{
		xDotProduct = MTH3D_M_xDotProductVector(&stCenter, &hCamera->stNormPlaneDown);
		if (MTH_M_bGreater(xDotProduct, MTH_M_xAdd(xRadius, hCamera->xDistPlaneDown)))
		{
			return GEO_C_lCullingOut;
		}
		if (MTH_M_bLess(xDotProduct, MTH_M_xSub(hCamera->xDistPlaneDown, xRadius)))
		{
			_lCullingResult |= GEO_C_lCullingDown;
		}
	}

	return _lCullingResult;
}

MTH_tdxReal GEO_xGetZFar(void)
{
    return GEO_g_xZFar;
}

void GEO_vSetZFar(MTH_tdxReal _xZ)
{
    GEO_g_xZFar = _xZ;
}

MTH_tdxReal GEO_xGetZFarTransparencyZone(void)
{
    return GEO_g_xZFarTransparencyZone;
}

void GEO_vSetZFarTransparencyZone(MTH_tdxReal _xZ)
{
    GEO_g_xZFarTransparencyZone = _xZ;
    if (_xZ != s_xConst0)
        GEO_g_xOoZFarTransparencyZone = s_xConst1 / _xZ;
}

MTH_tdxReal GEO_xGetZFarTransparencyLevel(void)
{
    return GEO_g_xZFarTransparencyLevel;
}

void GEO_vCreateObjectListOfParallelBox(
    ACP_tdxHandleOfObject hObject,
    ACP_tdxIndex          xNumberOfParallelBoxes)
{
    ACP_tdxIndex xIndex;
    hObject->xNbParallelBoxes = xNumberOfParallelBoxes;
    if (hObject->xNbParallelBoxes == 0) return;
    if (hObject->d_stListOfParallelBoxes != NULL) return;
    GEO_M_CPAMalloc(hObject->d_stListOfParallelBoxes, GEO_tdstParallelBox *,
        sizeof(GEO_tdstParallelBox) * hObject->xNbParallelBoxes, E_uwGEONotEnoughtMemory);
    for (xIndex = 0; xIndex < hObject->xNbParallelBoxes; xIndex++) {

        hObject->d_stListOfParallelBoxes[xIndex].stMinPoint.xZ =
            hObject->d_stListOfParallelBoxes[xIndex].stMinPoint.xY =
            hObject->d_stListOfParallelBoxes[xIndex].stMinPoint.xX = MTH_C_InfinitPlus;
        hObject->d_stListOfParallelBoxes[xIndex].stMaxPoint.xZ =
            hObject->d_stListOfParallelBoxes[xIndex].stMaxPoint.xY =
            hObject->d_stListOfParallelBoxes[xIndex].stMaxPoint.xX = MTH_C_InfinitMinus;
    }
}

void GEO_xComputeNormalWeightedBySurf(
    MTH3D_tdstVector *p_stPResult,
    MTH3D_tdstVector *p_stPA,
    MTH3D_tdstVector *p_stPB,
    MTH3D_tdstVector *p_stPC)
{
    MTH3D_tdstVector stEdgeAB, stEdgeAC;

    MTH3D_M_vSubVector(&stEdgeAB, p_stPB, p_stPA);
    MTH3D_M_vSubVector(&stEdgeAC, p_stPC, p_stPA);

    MTH3D_M_vCrossProductVectorWithoutBuffer(p_stPResult, &stEdgeAB, &stEdgeAC);
}

void GEO_xComputeObjectNormals2(GEO_tdstGeometricObject *p_stObj)
{
    if (p_stObj->d_stListOfPointsNormals == NULL)
        return;

    GEO_xComputeObjectNormals(p_stObj);
}

/* 0x0041b8f0 */
void GEO_xGrowSphereWithPoint(
    MTH3D_tdstVector *p_stOutCenter, MTH_tdxReal *p_xOutRadius,
    MTH3D_tdstVector *p_stInCenter,  MTH_tdxReal  xInRadius,
    MTH3D_tdstVector *p_stPoint)
{
    MTH3D_tdstVector stDiff;
    MTH_tdxReal xDist, xT;

    MTH3D_M_vSubVector(&stDiff, p_stInCenter, p_stPoint);
    xDist = MTH_M_xSqrt(MTH3D_M_xDotProductVector(&stDiff, &stDiff));

    if (xDist <= xInRadius) {
        MTH3D_M_vCopyVector(p_stOutCenter, p_stInCenter);
        *p_xOutRadius = xInRadius;
        return;
    }

    xT = (1.0f - xInRadius / xDist) * 0.5f;
    MTH3D_M_vLinearInterpolVector(p_stOutCenter, p_stInCenter, p_stPoint, xT);
    *p_xOutRadius = (xDist + xInRadius) * 0.5f;
}

/* 0x0041b7c0 */
void GEO_xMergeSpheres(
    MTH3D_tdstVector *p_stOutCenter, MTH_tdxReal *p_xOutRadius,
    MTH3D_tdstVector *p_stCenterA,   MTH_tdxReal  xRadiusA,
    MTH3D_tdstVector *p_stCenterB,   MTH_tdxReal  xRadiusB)
{
    MTH3D_tdstVector stDiff;
    MTH_tdxReal xDist, xT;

    MTH3D_M_vSubVector(&stDiff, p_stCenterA, p_stCenterB);
    xDist = MTH_M_xSqrt(MTH3D_M_xDotProductVector(&stDiff, &stDiff));

    if (xDist + xRadiusA <= xRadiusB) {
        MTH3D_M_vCopyVector(p_stOutCenter, p_stCenterB);
        *p_xOutRadius = xRadiusB;
        return;
    }
    if (xDist + xRadiusB < xRadiusA) {
        MTH3D_M_vCopyVector(p_stOutCenter, p_stCenterA);
        *p_xOutRadius = xRadiusA;
        return;
    }

    xT = (1.0f - (xRadiusA - xRadiusB) / xDist) * 0.5f;
    MTH3D_M_vLinearInterpolVector(p_stOutCenter, p_stCenterA, p_stCenterB, xT);
    *p_xOutRadius = (xRadiusA + xDist + xRadiusB) * 0.5f;
}

#include "GEO/GeoSprit.h"
void GEO_xComputeSpriteRadius(GEO_tdstIndexedSprite *p_stSprite, MTH_tdxReal *p_xOutRadius)
{
    MTH2D_tdstVector *p_xDimA = p_stSprite->hSprite->d_xSizeOfSprite;
    MTH2D_tdstVector *p_xDimB = p_stSprite->hSprite->d_xDisplacementOfSprite;
    MTH_tdxReal xW = (p_xDimA->xX + p_xDimB->xX) * p_stSprite->stSize.xX;
    MTH_tdxReal xH = (p_xDimB->xY + p_xDimA->xY) * p_stSprite->stSize.xY;

    *p_xOutRadius = MTH_M_xSqrt(xW * xW + xH * xH);
}

void GEO_xComputeObjectSphereBox(
    GEO_tdstGeometricObject *p_stObj,
    MTH3D_tdstVector        *p_stCenter,
    MTH_tdxReal             *p_xRadius)
{
    ACP_tdxIndex xPoint, xEl, xSub;

    for (xPoint = 0; xPoint < p_stObj->xNbPoints; xPoint++)
        GEO_xGrowSphereWithPoint(p_stCenter, p_xRadius,
            p_stCenter, *p_xRadius, p_stObj->d_stListOfPoints + xPoint);

    for (xEl = 0; xEl < p_stObj->xNbElements; xEl++) {
        if (p_stObj->d_xListOfElementsTypes[xEl] == GEO_C_xElementSprites) {
            GEO_tdstElementSprite *p_stEl =
                (GEO_tdstElementSprite *)p_stObj->d_stListOfElements[xEl];
            for (xSub = 0; xSub < p_stEl->xNbSprites; xSub++) {
                MTH3D_tdstVector stElCenter;
                MTH_tdxReal      xElRadius;
                ACP_tdxIndex     xCenterPoint;

                GEO_xComputeSpriteRadius(&p_stEl->d_stListOfSprites[xSub], &xElRadius);
                xCenterPoint = p_stEl->d_stListOfSprites[xSub].xCenterPoint;
                stElCenter = p_stObj->d_stListOfPoints[xCenterPoint];
                GEO_xMergeSpheres(p_stCenter, p_xRadius,
                    p_stCenter, *p_xRadius, &stElCenter, xElRadius);
            }
        }
        else if (p_stObj->d_xListOfElementsTypes[xEl] == GEO_C_xElementSpheres) {
            GEO_tdstElementSpheres *p_stEl =
                (GEO_tdstElementSpheres *)p_stObj->d_stListOfElements[xEl];
            for (xSub = 0; xSub < p_stEl->xNbSpheres; xSub++) {
                MTH3D_tdstVector stElCenter;
                MTH_tdxReal      xElRadius;
                ACP_tdxIndex     xCenterPoint;

                xCenterPoint = p_stEl->d_stListOfSpheres[xSub].xCenterPoint;
                stElCenter = p_stObj->d_stListOfPoints[xCenterPoint];
                xElRadius  = p_stEl->d_stListOfSpheres[xSub].xRadius;
                GEO_xMergeSpheres(p_stCenter, p_xRadius,
                    p_stCenter, *p_xRadius, &stElCenter, xElRadius);
            }
        }
    }
}

void GEO_vComputeObjectListOfParallelBox(ACP_tdxHandleOfObject hObject)
{
    ACP_tdxIndex xIndex;
    if (hObject->xNbParallelBoxes == 0) return;
    if (hObject->d_stListOfParallelBoxes == NULL)
        GEO_vCreateObjectListOfParallelBox(hObject, hObject->xNbParallelBoxes);
    for (xIndex = 0; xIndex < hObject->xNbParallelBoxes; xIndex++)
        GEO_vComputeObjectParallelBox(hObject, xIndex);
}

ACP_tdxBool GEO_bIsLookAt(GEO_tdstGeometricObject *p_stObj)
{
    return (ACP_tdxBool)p_stObj->ulType;
}

void POS_fn_vGetRotationMatrixCol2(POS_tdstCompletePosition *_hMatrix, MTH3D_tdstVector *_p_stCol2);
#pragma optimize("y", off)
void GEO_vTurnLookAt(
    GEO_tdstGeometricObject  *p_stObj,
    void                     *hVpt,
    POS_tdstCompletePosition *p_stCurrentMatrix)
{
    POS_tdstCompletePosition stCamInv;
    MTH3D_tdstVector stDir, stUp, stRight, stNewDir;
    float fScale;

    POS_fn_vInvertIsoMatrix(&stCamInv,
        &((GLI_tdstSpecificAttributesFor3D *)((struct GLD_tdstViewportAttributes_ *)hVpt)->p_vSpecificToXD)->p_stCam->stMatrix);

    MTH3D_M_vSubVector(&stDir, &stCamInv.stTranslationVector, &p_stCurrentMatrix->stTranslationVector);

    fScale = MTH_M_xInvSqrt(MTH3D_M_xDotProductVector(&stDir, &stDir));
    MTH3D_M_vMulScalarVector(&stDir, fScale, &stDir);

    if (p_stObj->ulType & 2) {
        POS_fn_vGetRotationMatrixCol2(p_stCurrentMatrix, &stUp);
    } else {
        stUp.xX = stCamInv.stTransformMatrix.stCol_1.xX;
        stUp.xY = stCamInv.stTransformMatrix.stCol_1.xY;
        stUp.xZ = stCamInv.stTransformMatrix.stCol_1.xZ;
    }

    stRight.xX = stUp.xY * stDir.xZ - stUp.xZ * stDir.xY;
    stRight.xY = stUp.xZ * stDir.xX - stUp.xX * stDir.xZ;
    stRight.xZ = stUp.xX * stDir.xY - stUp.xY * stDir.xX;

    fScale = MTH_M_xSqrt(stRight.xX*stRight.xX + stRight.xY*stRight.xY + stRight.xZ*stRight.xZ);
    if (MTH_C_ZERO < fScale) {
        fScale = MTH_C_ONE / fScale;
        stRight.xX *= fScale;
        stRight.xY *= fScale;
        stRight.xZ *= fScale;

        stNewDir.xX = stRight.xY * stUp.xZ - stRight.xZ * stUp.xY;
        stNewDir.xY = stRight.xZ * stUp.xX - stRight.xX * stUp.xZ;
        stNewDir.xZ = stRight.xX * stUp.xY - stRight.xY * stUp.xX;

        POS_fn_vSetRotationMatrix(p_stCurrentMatrix, &stNewDir, &stRight, &stUp);
    }
}
#pragma optimize("", on)

void GEO_fn_vComputeCameraParametersForCulling(void *p_stVpt, MTH_tdxReal _xZFar)
{
    POS_tdstCompletePosition stMatrixInverse;
    GLI_tdstCamera          *hCamera;
    MTH3D_tdstMatrix        *p_stMatrix;

    hCamera = ((GLI_tdstSpecificAttributesFor3D *)((struct GLD_tdstViewportAttributes_ *)p_stVpt)->p_vSpecificToXD)->p_stCam;

    POS_fn_vInvertIsoMatrix(&stMatrixInverse, &hCamera->stMatrix);
    p_stMatrix = &stMatrixInverse.stTransformMatrix;

    MTH3D_M_vSetVectorElements(&gs_stCameraPosInGlobal, MTH_C_ZERO, MTH_C_ZERO, MTH_C_MinusONE);
    MTH3D_M_vMulMatrixVector(&gs_stGlobalNormPlaneNear, p_stMatrix, &gs_stCameraPosInGlobal);

    MTH3D_M_vCopyVector(&gs_stCameraPosInGlobal, &stMatrixInverse.stTranslationVector);

    MTH3D_M_vMulMatrixVector(&gs_stGlobalNormPlaneLeft, p_stMatrix, &hCamera->stNormPlaneRight);

    MTH3D_M_vMulMatrixVector(&gs_stGlobalNormPlaneRight, p_stMatrix, &hCamera->stNormPlaneLeft);

    MTH3D_M_vMulMatrixVector(&gs_stGlobalNormPlaneUp, p_stMatrix, &hCamera->stNormPlaneUp);

    MTH3D_M_vMulMatrixVector(&gs_stGlobalNormPlaneDown, p_stMatrix, &hCamera->stNormPlaneDown);

    GEO_g_xZFar = _xZFar;
}

long GEO_lCullingSphere(
    void                    *p_stVpt,
    MTH3D_tdstVector        *p_stSphereCenter,
    MTH_tdxReal              xRadius,
    POS_tdstCompletePosition *p_stMatrix,
    long                     _lCullingresult)
{
    POS_tdstCompletePosition stCombined;
    POS_fn_vMulMatrixMatrix(
        &stCombined,
        &((GLI_tdstSpecificAttributesFor3D *)((struct GLD_tdstViewportAttributes_ *)p_stVpt)->p_vSpecificToXD)->p_stCam->stMatrix,
        p_stMatrix);
    return GEO_lMainCullingSphere(
        p_stVpt, p_stSphereCenter, xRadius,
        &stCombined, p_stMatrix, _lCullingresult);
}

long GEO_lCullingSphereNoMMM(
    void                    *p_stVpt,
    MTH3D_tdstVector        *p_stShereCenter,
    MTH_tdxReal              xRadius,
    POS_tdstCompletePosition *p_stMatrix,
    long                     _lCullingresult)
{
    return GEO_lMainCullingSphere(
        p_stVpt, p_stShereCenter, xRadius,
        HIE_hRetrieveCurrentMatrix(), p_stMatrix, _lCullingresult);
}

long GEO_lCullingBox(
	void								*p_stVpt,
	MTH3D_tdstVector			*p_stMin,
	MTH3D_tdstVector			*p_stMax,
	long						_lCullingResult)
{

	MTH3D_tdstVector				stMinWithCamera, stMaxWithCamera;
	MTH_tdxReal						xCompute, xValue0, xValue1, xValue2, xValue3, xValue4, xValue5;
	GLI_tdstCamera					*hCamera;
	ACP_tdxBool						bComputed = FALSE;

	hCamera = ((GLI_tdstSpecificAttributesFor3D *) ((struct GLD_tdstViewportAttributes_ *)p_stVpt)->p_vSpecificToXD)->p_stCam;

	MTH3D_M_vSubVector( &stMinWithCamera, p_stMin, &gs_stCameraPosInGlobal );
	MTH3D_M_vSubVector( &stMaxWithCamera, p_stMax, &gs_stCameraPosInGlobal );

	_lCullingResult |= GEO_C_lCullingIntersect;

	if ( !(_lCullingResult & GEO_C_lCullingNear) )
	{
		xValue0 = MTH_M_xMul(stMinWithCamera.xX, gs_stGlobalNormPlaneNear.xX);
		xValue1 = MTH_M_xMul(stMinWithCamera.xY, gs_stGlobalNormPlaneNear.xY);
		xValue2 = MTH_M_xMul(stMinWithCamera.xZ, gs_stGlobalNormPlaneNear.xZ);
		xValue3 = MTH_M_xMul(stMaxWithCamera.xX, gs_stGlobalNormPlaneNear.xX);
		xValue4 = MTH_M_xMul(stMaxWithCamera.xY, gs_stGlobalNormPlaneNear.xY);
		xValue5 = MTH_M_xMul(stMaxWithCamera.xZ, gs_stGlobalNormPlaneNear.xZ);

		xCompute = MTH_M_xMin(xValue0, xValue3) + MTH_M_xMin(xValue1, xValue4) + MTH_M_xMin(xValue2, xValue5);

		if ( MTH_M_bGreater(xCompute, (-(hCamera->xNear))))
		{
			return GEO_C_lCullingOut;
		}

		xCompute = MTH_M_xMax(xValue0, xValue3) + MTH_M_xMax(xValue1, xValue4) + MTH_M_xMax(xValue2, xValue5);
		if ( MTH_M_bLess(xCompute, (-(hCamera->xNear))))
		{
			_lCullingResult |= GEO_C_lCullingNear;
		}
		bComputed = TRUE;
	}

	if ( !(_lCullingResult & GEO_C_lCullingFar) )
	{
		if ( MTH_M_bDifferentZero(GEO_g_xZFar) )
		{
			if ( !bComputed )
			{
				xValue0 = MTH_M_xMul(stMinWithCamera.xX, gs_stGlobalNormPlaneNear.xX);
				xValue1 = MTH_M_xMul(stMinWithCamera.xY, gs_stGlobalNormPlaneNear.xY);
				xValue2 = MTH_M_xMul(stMinWithCamera.xZ, gs_stGlobalNormPlaneNear.xZ);
				xValue3 = MTH_M_xMul(stMaxWithCamera.xX, gs_stGlobalNormPlaneNear.xX);
				xValue4 = MTH_M_xMul(stMaxWithCamera.xY, gs_stGlobalNormPlaneNear.xY);
				xValue5 = MTH_M_xMul(stMaxWithCamera.xZ, gs_stGlobalNormPlaneNear.xZ);

				xCompute = -(MTH_M_xMax(xValue0, xValue3) + MTH_M_xMax(xValue1, xValue4) + MTH_M_xMax(xValue2, xValue5));
			}
			else
			{
				xCompute = -(xCompute);
			}

			if ( MTH_M_bGreater(xCompute, GEO_g_xZFar ))
			{
				return GEO_C_lCullingOut;
			}

			GEO_g_xZFarTransparencyLevel = MTH_M_xSub(GEO_g_xZFar, GEO_g_xZFarTransparencyZone);
			if ( MTH_M_bGreater(xCompute, GEO_g_xZFarTransparencyLevel ))
				GEO_g_xZFarTransparencyLevel = MTH_M_xSub(MTH_C_ONE, (MTH_M_xMul(MTH_M_xSub(xCompute, GEO_g_xZFarTransparencyLevel), GEO_g_xOoZFarTransparencyZone)));
			else
			{
				GEO_g_xZFarTransparencyLevel = MTH_C_ONE;

				_lCullingResult |= GEO_C_lCullingFar;
			}
		}
		else
		{
			GEO_g_xZFarTransparencyLevel = MTH_C_ONE;

			_lCullingResult |= GEO_C_lCullingFar;
		}
	}
	else
	{
		GEO_g_xZFarTransparencyLevel = MTH_C_ONE;
	}

	if ( !(_lCullingResult & GEO_C_lCullingLeft) )
	{
		xValue0 = MTH_M_xMul(stMinWithCamera.xX, gs_stGlobalNormPlaneLeft.xX);
		xValue1 = MTH_M_xMul(stMinWithCamera.xY, gs_stGlobalNormPlaneLeft.xY);
		xValue2 = MTH_M_xMul(stMinWithCamera.xZ, gs_stGlobalNormPlaneLeft.xZ);
		xValue3 = MTH_M_xMul(stMaxWithCamera.xX, gs_stGlobalNormPlaneLeft.xX);
		xValue4 = MTH_M_xMul(stMaxWithCamera.xY, gs_stGlobalNormPlaneLeft.xY);
		xValue5 = MTH_M_xMul(stMaxWithCamera.xZ, gs_stGlobalNormPlaneLeft.xZ);

		xCompute = MTH_M_xMin(xValue0, xValue3) + MTH_M_xMin(xValue1, xValue4) + MTH_M_xMin(xValue2, xValue5);
		if ( MTH_M_bGreater(xCompute, hCamera->xDistPlaneLeft ))
		{
			return GEO_C_lCullingOut;
		}

		xCompute = MTH_M_xMax(xValue0, xValue3) + MTH_M_xMax(xValue1, xValue4) + MTH_M_xMax(xValue2, xValue5);
		if (MTH_M_bLess(xCompute, hCamera->xDistPlaneLeft ))
		{
			_lCullingResult |= GEO_C_lCullingLeft;
		}
	}

	if ( !(_lCullingResult & GEO_C_lCullingRight) )
	{
		xValue0 = MTH_M_xMul(stMinWithCamera.xX, gs_stGlobalNormPlaneRight.xX);
		xValue1 = MTH_M_xMul(stMinWithCamera.xY, gs_stGlobalNormPlaneRight.xY);
		xValue2 = MTH_M_xMul(stMinWithCamera.xZ, gs_stGlobalNormPlaneRight.xZ);
		xValue3 = MTH_M_xMul(stMaxWithCamera.xX, gs_stGlobalNormPlaneRight.xX);
		xValue4 = MTH_M_xMul(stMaxWithCamera.xY, gs_stGlobalNormPlaneRight.xY);
		xValue5 = MTH_M_xMul(stMaxWithCamera.xZ, gs_stGlobalNormPlaneRight.xZ);

		xCompute = MTH_M_xMin(xValue0, xValue3) + MTH_M_xMin(xValue1, xValue4) + MTH_M_xMin(xValue2, xValue5);
		if (MTH_M_bGreater(xCompute, hCamera->xDistPlaneRight ))
		{
			return GEO_C_lCullingOut;
		}

		xCompute = MTH_M_xMax(xValue0, xValue3) + MTH_M_xMax(xValue1, xValue4) + MTH_M_xMax(xValue2, xValue5);
		if (MTH_M_bLess(xCompute, hCamera->xDistPlaneRight ))
		{
			_lCullingResult |= GEO_C_lCullingRight;
		}
	}

	if ( !(_lCullingResult & GEO_C_lCullingUp) )
	{
		xValue0 = MTH_M_xMul(stMinWithCamera.xX, gs_stGlobalNormPlaneUp.xX);
		xValue1 = MTH_M_xMul(stMinWithCamera.xY, gs_stGlobalNormPlaneUp.xY);
		xValue2 = MTH_M_xMul(stMinWithCamera.xZ, gs_stGlobalNormPlaneUp.xZ);
		xValue3 = MTH_M_xMul(stMaxWithCamera.xX, gs_stGlobalNormPlaneUp.xX);
		xValue4 = MTH_M_xMul(stMaxWithCamera.xY, gs_stGlobalNormPlaneUp.xY);
		xValue5 = MTH_M_xMul(stMaxWithCamera.xZ, gs_stGlobalNormPlaneUp.xZ);

		xCompute = MTH_M_xMin(xValue0, xValue3) + MTH_M_xMin(xValue1, xValue4) + MTH_M_xMin(xValue2, xValue5);
		if (MTH_M_bGreater(xCompute, hCamera->xDistPlaneUp ))
		{
			return GEO_C_lCullingOut;
		}

		xCompute = MTH_M_xMax(xValue0, xValue3) + MTH_M_xMax(xValue1, xValue4) + MTH_M_xMax(xValue2, xValue5);
		if (MTH_M_bLess(xCompute, hCamera->xDistPlaneUp ))
		{
			_lCullingResult |= GEO_C_lCullingUp;
		}
	}

	if ( !(_lCullingResult & GEO_C_lCullingDown) )
	{
		xValue0 = MTH_M_xMul(stMinWithCamera.xX, gs_stGlobalNormPlaneDown.xX);
		xValue1 = MTH_M_xMul(stMinWithCamera.xY, gs_stGlobalNormPlaneDown.xY);
		xValue2 = MTH_M_xMul(stMinWithCamera.xZ, gs_stGlobalNormPlaneDown.xZ);
		xValue3 = MTH_M_xMul(stMaxWithCamera.xX, gs_stGlobalNormPlaneDown.xX);
		xValue4 = MTH_M_xMul(stMaxWithCamera.xY, gs_stGlobalNormPlaneDown.xY);
		xValue5 = MTH_M_xMul(stMaxWithCamera.xZ, gs_stGlobalNormPlaneDown.xZ);

		xCompute = MTH_M_xMin(xValue0, xValue3) + MTH_M_xMin(xValue1, xValue4) + MTH_M_xMin(xValue2, xValue5);
		if (MTH_M_bGreater(xCompute, hCamera->xDistPlaneDown ))
		{
			return GEO_C_lCullingOut;
		}

		xCompute = MTH_M_xMax(xValue0, xValue3) + MTH_M_xMax(xValue1, xValue4) + MTH_M_xMax(xValue2, xValue5);
		if (MTH_M_bLess(xCompute, hCamera->xDistPlaneDown ))
		{
			_lCullingResult |= GEO_C_lCullingDown;
		}
	}

	return _lCullingResult;
}

/* 0x0041B4F0 */
void __cdecl GEO_pc_fn_vExtendBoxWithPointsAndSprites(int *p_iObj, unsigned int *p_uMatrix,
                                                     float *p_xMax, float *p_xMin)
{
    float  *pfPoint;
    float  *pfM;
    float  *pfSpriteSize;
    GEO_tdstElementSprite *piSpriteSet;
    float  *pfSpritePt;
    float   fX, fY;
    float   fRadius;
    float   fScale;
    MTH3D_tdstVector stPt;
    MTH3D_tdstVector stSp;
    int     iPoint;
    int     iElem;
    int     iOffset;
    int     iSprite;

    pfPoint = (float *)((GEO_tdstGeometricObject *)p_iObj)->d_stListOfPoints;
    iPoint = 0;
    if (0 < (short)((GEO_tdstGeometricObject *)p_iObj)->xNbPoints) {
        pfM = (float *)&((POS_tdstCompletePosition *)p_uMatrix)->stTransformMatrix;
        do {
            MTH3D_M_vMulMatrixVector(&stPt, (MTH3D_tdstMatrix *)pfM, (MTH3D_tdstVector *)pfPoint);
            if (stPt.xX < p_xMin[0]) {
                p_xMin[0] = stPt.xX;
            }
            if (p_xMax[0] < stPt.xX) {
                p_xMax[0] = stPt.xX;
            }
            if (stPt.xY < p_xMin[1]) {
                p_xMin[1] = stPt.xY;
            }
            if (p_xMax[1] < stPt.xY) {
                p_xMax[1] = stPt.xY;
            }
            if (stPt.xZ < p_xMin[2]) {
                p_xMin[2] = stPt.xZ;
            }
            if (p_xMax[2] < stPt.xZ) {
                p_xMax[2] = stPt.xZ;
            }
            iPoint = iPoint + 1;
            pfPoint = pfPoint + 3;
        } while (iPoint < (short)((GEO_tdstGeometricObject *)p_iObj)->xNbPoints);
    }

    iElem = 0;
    if (0 < ((GEO_tdstGeometricObject *)p_iObj)->xNbElements) {
        do {
            if (((GEO_tdstGeometricObject *)p_iObj)->d_xListOfElementsTypes[iElem] == GEO_C_xElementSprites) {
                piSpriteSet = (GEO_tdstElementSprite *)((GEO_tdstGeometricObject *)p_iObj)->d_stListOfElements[iElem];
                iSprite = 0;
                if (0 < piSpriteSet->xNbSprites) {
                    pfM = (float *)&((POS_tdstCompletePosition *)p_uMatrix)->stTransformMatrix;
                    iOffset = 0;
                    do {
                        pfSpriteSize = &((GEO_tdstIndexedSprite *)((char *)piSpriteSet->d_stListOfSprites + iOffset))->hSprite->d_xSizeOfSprite->xX;
                        fX = pfSpriteSize[0];
                        fY = pfSpriteSize[1];
                        fScale = POS_fn_xGetMaxScale((POS_tdstCompletePosition *)p_uMatrix);
                        pfSpritePt = (float *)(((GEO_tdstGeometricObject *)p_iObj)->d_stListOfPoints +
                                               ((GEO_tdstIndexedSprite *)((char *)piSpriteSet->d_stListOfSprites + iOffset))->xCenterPoint);
                        fRadius = MTH_M_xSqrt(fY * fY + fX * fX) * 0.5f * fScale;
                        {
                            float  tX;
                            float  tY;
                            float  tZ;
                            float  lo;
                            float  hi;
                            MTH3D_M_vMulMatrixVector(&stSp, (MTH3D_tdstMatrix *)pfM, (MTH3D_tdstVector *)pfSpritePt);
                            tX = stSp.xX;
                            tY = stSp.xY;
                            tZ = stSp.xZ;
                            lo = tX - fRadius;
                            if (p_xMin[0] < lo) {
                                lo = p_xMin[0];
                            }
                            p_xMin[0] = lo;
                            lo = tY - fRadius;
                            if (p_xMin[1] < lo) {
                                lo = p_xMin[1];
                            }
                            p_xMin[1] = lo;
                            lo = tZ - fRadius;
                            if (p_xMin[2] < lo) {
                                lo = p_xMin[2];
                            }
                            p_xMin[2] = lo;
                            hi = tX + fRadius;
                            if (hi < p_xMax[0]) {
                                hi = p_xMax[0];
                            }
                            p_xMax[0] = hi;
                            hi = tY + fRadius;
                            if (hi < p_xMax[1]) {
                                hi = p_xMax[1];
                            }
                            p_xMax[1] = hi;
                            hi = tZ + fRadius;
                            if (hi < p_xMax[2]) {
                                hi = p_xMax[2];
                            }
                            p_xMax[2] = hi;
                        }
                        iOffset = iOffset + 0x2c;
                        iSprite = iSprite + 1;
                    } while (iSprite < piSpriteSet->xNbSprites);
                }
            }
            iElem = iElem + 1;
        } while (iElem < ((GEO_tdstGeometricObject *)p_iObj)->xNbElements);
    }
}
