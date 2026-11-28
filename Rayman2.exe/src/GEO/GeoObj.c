#include "GEO/GEO_types.h"
#include "GEO/GeoObj.h"
#include "GEO/Element.h"
#include "GEO/GeoMem.h"
#include "TMP/TmpMem.h"
#include "MTH/MTH3D.h"
#include "GMT/GmtFunct.h"
#include <string.h>
#pragma intrinsic(memcpy)

#include "GEO/Fmd.h"

#undef MTH2D_M_vDivScalarVector
#define MTH2D_M_vDivScalarVector( VectDest, VectA, a)                   \
        {       (VectDest)->xX = MTH_M_xDiv((VectA)->xX, (a));          \
                (VectDest)->xY = MTH_M_xDiv((VectA)->xY, (a)); }
#undef MTH2D_M_vDivScalarMatrix
#define MTH2D_M_vDivScalarMatrix(MatDest, MatA, a)                              \
                {       MTH2D_M_vDivScalarVector( &((MatDest)->stCol_0),        \
                                        &((MatA)->stCol_0), (a));               \
                        MTH2D_M_vDivScalarVector( &((MatDest)->stCol_1),        \
                                                  &((MatA)->stCol_1), (a)); }
#undef MTH2D_M_xDetMatrix
#define MTH2D_M_xDetMatrix(MatA)                                        \
                MTH_M_xSub(                                             \
                MTH_M_xMul( (MatA)->stCol_0.xX, (MatA)->stCol_1.xY ),   \
                MTH_M_xMul( (MatA)->stCol_0.xY, (MatA)->stCol_1.xX ))
#undef MTH2D_M_vInverMatrix
#define MTH2D_M_vInverMatrix(MatDest, MatA)                                             \
        {                                                                               \
            MTH2D_tdstMatrix MatTmp;                                                    \
            MTH_tdxReal det;                                                            \
            MatTmp.stCol_0.xX = (MatA)->stCol_1.xY;                                     \
            MatTmp.stCol_0.xY = MTH_M_xNeg( (MatA)->stCol_0.xY );                       \
            MatTmp.stCol_1.xX = MTH_M_xNeg( (MatA)->stCol_1.xX );                       \
            MatTmp.stCol_1.xY = (MatA)->stCol_0.xX;                                     \
            det=MTH2D_M_xDetMatrix( MatA );                                             \
            MTH2D_M_vDivScalarMatrix(MatDest, &(MatTmp), det );                         \
            }
#undef MTH2D_M_vSetVectorElements
#define MTH2D_M_vSetVectorElements( VectDest, Vx, Vy)                   \
        {               (VectDest)->xX = (Vx);                          \
                        (VectDest)->xY = (Vy); }
#undef MTH2D_M_xGetXofVector
#define MTH2D_M_xGetXofVector( VectA )                                                  \
                        (VectA)->xX
#undef MTH2D_M_xGetYofVector
#define MTH2D_M_xGetYofVector( VectA )                                                  \
                        (VectA)->xY
#undef MTH2D_M_vSetVectorsInMatrix
#define MTH2D_M_vSetVectorsInMatrix( MatDest, VectA, VectB)             \
        {    (MatDest)->stCol_0.xX=(VectA)->xX;                         \
             (MatDest)->stCol_0.xY=(VectA)->xY;                         \
             (MatDest)->stCol_1.xX=(VectB)->xX;                         \
             (MatDest)->stCol_1.xY=(VectB)->xY; }
#undef MTH2D_M_vMulMatrixVectorWithoutBuffer
#define MTH2D_M_vMulMatrixVectorWithoutBuffer( VectDest, MatA, VectA)           \
        {     (VectDest)->xX = MTH_M_xAdd(                                      \
                                MTH_M_xMul( (MatA)->stCol_0.xX, (VectA)->xX),   \
                        MTH_M_xMul( (MatA)->stCol_1.xX, (VectA)->xY));          \
             (VectDest)->xY = MTH_M_xAdd(                                       \
                                MTH_M_xMul( (MatA)->stCol_0.xY, (VectA)->xX),   \
                        MTH_M_xMul( (MatA)->stCol_1.xY, (VectA)->xY)); }

/* 0x0041D5E0 */
extern void __cdecl GLI_xGetMaterialTextureScrollingCoef(ACP_tdxHandleOfMaterial hMaterial, long lNumberOfTexture,
                                                         long *p_bSwitch, MTH_tdxReal *p_fUSpeed,
                                                         MTH_tdxReal *p_fVSpeed);

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

void GEO_xComputeObjectNormals(GEO_tdstGeometricObject *p_stObj)
{
    MTH3D_tdstVector *p_stVertex, *p_stLastVertex, stNormalResult;
    GEO_tdstTripledIndex *p_stITStart, *p_stITLast;
    GEO_tdstElementIndexedTriangles *p_stLocalElement;
    ACP_tdxIndex      xIndex, xTriangleIndex;

    if (p_stObj->xNbPoints == 0)
        return;

    if (p_stObj->d_stListOfPointsNormals == NULL) {
        GEO_M_CPAMalloc(p_stObj->d_stListOfPointsNormals, MTH3D_tdstVector *,
            sizeof(MTH3D_tdstVector) * p_stObj->xNbPoints, E_uwGEONotEnoughtMemory);
    }

    p_stVertex     = p_stObj->d_stListOfPointsNormals;
    p_stLastVertex = p_stVertex + p_stObj->xNbPoints;
    for (; p_stVertex < p_stLastVertex; p_stVertex++)
        MTH3D_M_vNullVector(p_stVertex);

    for (xIndex = 0; xIndex < p_stObj->xNbElements; xIndex++) {
        if (p_stObj->d_xListOfElementsTypes[xIndex] == GEO_C_xElementIndexedTriangles) {
            p_stLocalElement = ((GEO_tdstElementIndexedTriangles *)p_stObj->d_stListOfElements[xIndex]);

            p_stITStart = p_stLocalElement->d_stListOfFacesTripled;
            p_stITLast = p_stITStart + p_stLocalElement->xNbFaces;
            for (xTriangleIndex = 0; p_stITStart < p_stITLast; p_stITStart++, xTriangleIndex++) {
                GEO_xComputeNormalWeightedBySurf(&stNormalResult,
                    p_stObj->d_stListOfPoints + p_stITStart->a3_xIndex[0],
                    p_stObj->d_stListOfPoints + p_stITStart->a3_xIndex[1],
                    p_stObj->d_stListOfPoints + p_stITStart->a3_xIndex[2]);
                p_stLocalElement->d_stListOfFacesNormals[xTriangleIndex] = stNormalResult;

                if (!MTH3D_M_bIsNullVector(&stNormalResult)) {
                    MTH3D_M_vAddVector(p_stObj->d_stListOfPointsNormals + p_stITStart->a3_xIndex[0],
                        p_stObj->d_stListOfPointsNormals + p_stITStart->a3_xIndex[0], &stNormalResult);
                    MTH3D_M_vAddVector(p_stObj->d_stListOfPointsNormals + p_stITStart->a3_xIndex[1],
                        p_stObj->d_stListOfPointsNormals + p_stITStart->a3_xIndex[1], &stNormalResult);
                    MTH3D_M_vAddVector(p_stObj->d_stListOfPointsNormals + p_stITStart->a3_xIndex[2],
                        p_stObj->d_stListOfPointsNormals + p_stITStart->a3_xIndex[2], &stNormalResult);
                    MTH3D_M_vNormalizeVector(&p_stLocalElement->d_stListOfFacesNormals[xTriangleIndex],
                        &p_stLocalElement->d_stListOfFacesNormals[xTriangleIndex]);
                }
            }
        }
        else if (p_stObj->d_xListOfElementsTypes[xIndex] == GEO_C_xElementFaceMapDescriptors) {

            GEO_tdstElementFaceMapDescriptors *p_stEl =
                (GEO_tdstElementFaceMapDescriptors *)p_stObj->d_stListOfElements[xIndex];
            for (xTriangleIndex = 0; xTriangleIndex < p_stEl->xNbFaces; xTriangleIndex++) {
                ACP_tdxIndex i0 = p_stEl->d_stListOfFacesQuadrupled[xTriangleIndex].stFaceTripled.a3_xIndex[0];
                ACP_tdxIndex i1 = p_stEl->d_stListOfFacesQuadrupled[xTriangleIndex].stFaceTripled.a3_xIndex[1];
                ACP_tdxIndex i2 = p_stEl->d_stListOfFacesQuadrupled[xTriangleIndex].stFaceTripled.a3_xIndex[2];

                GEO_xComputeNormalWeightedBySurf(&stNormalResult,
                    p_stObj->d_stListOfPoints + i0,
                    p_stObj->d_stListOfPoints + i1,
                    p_stObj->d_stListOfPoints + i2);
                p_stEl->d_stListOfFacesNormals[xTriangleIndex] = stNormalResult;

                if (!MTH3D_M_bIsNullVector(&stNormalResult)) {
                    MTH3D_M_vAddVector(p_stObj->d_stListOfPointsNormals + i0,
                        p_stObj->d_stListOfPointsNormals + i0, &stNormalResult);
                    MTH3D_M_vAddVector(p_stObj->d_stListOfPointsNormals + i1,
                        p_stObj->d_stListOfPointsNormals + i1, &stNormalResult);
                    MTH3D_M_vAddVector(p_stObj->d_stListOfPointsNormals + i2,
                        p_stObj->d_stListOfPointsNormals + i2, &stNormalResult);
                    MTH3D_M_vNormalizeVector(&p_stEl->d_stListOfFacesNormals[xTriangleIndex],
                        &p_stEl->d_stListOfFacesNormals[xTriangleIndex]);
                }
            }
        }
    }

    p_stVertex     = p_stObj->d_stListOfPointsNormals;
    p_stLastVertex = p_stVertex + p_stObj->xNbPoints;
    for (; p_stVertex < p_stLastVertex; p_stVertex++) {
        if (MTH3D_M_bIsNullVector(p_stVertex))
        {   MTH3D_M_vSetVectorElements(p_stVertex, MTH_C_ZERO, MTH_C_ZERO, MTH_C_ONE);   }
        else
            MTH3D_M_vNormalizeVector(p_stVertex, p_stVertex);
    }
}

void GEO_xComputeObjectNormals2(GEO_tdstGeometricObject *p_stObj)
{
    if (p_stObj->d_stListOfPointsNormals == NULL)
        return;

    GEO_xComputeObjectNormals(p_stObj);
}

/* 0x41c410 */
static void GEO_xComputeIndexUsedByElements(GEO_tdstGeometricObject *p_stObj)
{
    GEO_tdstElementIndexedTriangles *p_stLocalElement;
    ACP_tdxIndex  xIndex, xTriangleIndex;
    unsigned long xObjIndex, xIndexCounter;
    unsigned char aDEF_ulCountBuffer[128];

    if (p_stObj->xNbElements == 1)
        return;

    for (xIndex = 0; xIndex < p_stObj->xNbElements; xIndex++) {
        if (p_stObj->d_xListOfElementsTypes[xIndex] == GEO_C_xElementIndexedTriangles) {
            p_stLocalElement = (GEO_tdstElementIndexedTriangles *)p_stObj->d_stListOfElements[xIndex];

            memset(aDEF_ulCountBuffer, 0, ((p_stObj->xNbPoints + 7) >> 3));

            for (xTriangleIndex = 0; xTriangleIndex < p_stLocalElement->xNbFaces; xTriangleIndex++) {
                xObjIndex = p_stLocalElement->d_stListOfFacesTripled[xTriangleIndex].a3_xIndex[0];
                aDEF_ulCountBuffer[xObjIndex >> 3] |= (unsigned char)(1 << (xObjIndex & 7));
                xObjIndex = p_stLocalElement->d_stListOfFacesTripled[xTriangleIndex].a3_xIndex[1];
                aDEF_ulCountBuffer[xObjIndex >> 3] |= (unsigned char)(1 << (xObjIndex & 7));
                xObjIndex = p_stLocalElement->d_stListOfFacesTripled[xTriangleIndex].a3_xIndex[2];
                aDEF_ulCountBuffer[xObjIndex >> 3] |= (unsigned char)(1 << (xObjIndex & 7));
            }

            p_stLocalElement->xNumberOfIndexsUsed = 0;
            for (xObjIndex = 0; xObjIndex < (((unsigned long)p_stObj->xNbPoints) + 7) >> 3; xObjIndex++) {
                if (aDEF_ulCountBuffer[xObjIndex] & 0x01) p_stLocalElement->xNumberOfIndexsUsed++;
                if (aDEF_ulCountBuffer[xObjIndex] & 0x02) p_stLocalElement->xNumberOfIndexsUsed++;
                if (aDEF_ulCountBuffer[xObjIndex] & 0x04) p_stLocalElement->xNumberOfIndexsUsed++;
                if (aDEF_ulCountBuffer[xObjIndex] & 0x08) p_stLocalElement->xNumberOfIndexsUsed++;
                if (aDEF_ulCountBuffer[xObjIndex] & 0x10) p_stLocalElement->xNumberOfIndexsUsed++;
                if (aDEF_ulCountBuffer[xObjIndex] & 0x20) p_stLocalElement->xNumberOfIndexsUsed++;
                if (aDEF_ulCountBuffer[xObjIndex] & 0x40) p_stLocalElement->xNumberOfIndexsUsed++;
                if (aDEF_ulCountBuffer[xObjIndex] & 0x80) p_stLocalElement->xNumberOfIndexsUsed++;
            }

            if (p_stLocalElement->xNumberOfIndexsUsed == p_stObj->xNbPoints)
                continue;

            GEO_M_CPAMalloc(p_stLocalElement->d_stListOfIndexUsedByThisElement, ACP_tdxIndex *,
                sizeof(ACP_tdxIndex) * p_stLocalElement->xNumberOfIndexsUsed, E_uwGEONotEnoughtMemory);

            xIndexCounter = 0;
            for (xObjIndex = 0; xObjIndex < (((unsigned long)p_stObj->xNbPoints) + 7) >> 3; xObjIndex++) {
                if (aDEF_ulCountBuffer[xObjIndex] & 0x01) p_stLocalElement->d_stListOfIndexUsedByThisElement[xIndexCounter++] = (ACP_tdxIndex)(xObjIndex*8 + 0);
                if (aDEF_ulCountBuffer[xObjIndex] & 0x02) p_stLocalElement->d_stListOfIndexUsedByThisElement[xIndexCounter++] = (ACP_tdxIndex)(xObjIndex*8 + 1);
                if (aDEF_ulCountBuffer[xObjIndex] & 0x04) p_stLocalElement->d_stListOfIndexUsedByThisElement[xIndexCounter++] = (ACP_tdxIndex)(xObjIndex*8 + 2);
                if (aDEF_ulCountBuffer[xObjIndex] & 0x08) p_stLocalElement->d_stListOfIndexUsedByThisElement[xIndexCounter++] = (ACP_tdxIndex)(xObjIndex*8 + 3);
                if (aDEF_ulCountBuffer[xObjIndex] & 0x10) p_stLocalElement->d_stListOfIndexUsedByThisElement[xIndexCounter++] = (ACP_tdxIndex)(xObjIndex*8 + 4);
                if (aDEF_ulCountBuffer[xObjIndex] & 0x20) p_stLocalElement->d_stListOfIndexUsedByThisElement[xIndexCounter++] = (ACP_tdxIndex)(xObjIndex*8 + 5);
                if (aDEF_ulCountBuffer[xObjIndex] & 0x40) p_stLocalElement->d_stListOfIndexUsedByThisElement[xIndexCounter++] = (ACP_tdxIndex)(xObjIndex*8 + 6);
                if (aDEF_ulCountBuffer[xObjIndex] & 0x80) p_stLocalElement->d_stListOfIndexUsedByThisElement[xIndexCounter++] = (ACP_tdxIndex)(xObjIndex*8 + 7);
            }
        }
    }
}

/* 0x41c760 */
static ACP_tdxHandleOfObject GEO_hAllocGeometricObject(
    ACP_tdxIndex xNbPoints, ACP_tdxIndex xNbElements)
{
    GEO_tdstGeometricObject *p_stObjectToCreate;

    GEO_M_CPAMalloc(p_stObjectToCreate, GEO_tdstGeometricObject *,
        sizeof(GEO_tdstGeometricObject), E_uwGEONotEnoughtMemory);

    p_stObjectToCreate->xNbElements = xNbElements;
    p_stObjectToCreate->xNbPoints   = xNbPoints;

    GEO_M_CPAMalloc(p_stObjectToCreate->d_xListOfElementsTypes, ACP_tdxIndex *,
        sizeof(ACP_tdxIndex) * xNbElements, E_uwGEONotEnoughtMemory);
    GEO_M_CPAMalloc(p_stObjectToCreate->d_stListOfElements, void **,
        sizeof(void *) * xNbElements, E_uwGEONotEnoughtMemory);
    GEO_M_CPAMalloc(p_stObjectToCreate->d_stListOfPoints, MTH3D_tdstVector *,
        sizeof(MTH3D_tdstVector) * xNbPoints, E_uwGEONotEnoughtMemory);
    GEO_M_CPAMalloc(p_stObjectToCreate->d_stListOfPointsNormals, MTH3D_tdstVector *,
        sizeof(MTH3D_tdstVector) * xNbPoints, E_uwGEONotEnoughtMemory);

    return (ACP_tdxHandleOfObject)p_stObjectToCreate;
}

/* 0x0041cc80 */
static ACP_tdxHandleOfObject GEO_hAllocTMPGeometricObject(
    ACP_tdxIndex xNbPoints, ACP_tdxIndex xNbElements)
{
    GEO_tdstGeometricObject *p_stObjectToCreate;

    p_stObjectToCreate =
        (GEO_tdstGeometricObject *)TMP_M_p_Malloc(sizeof(GEO_tdstGeometricObject));

    p_stObjectToCreate->xNbElements = xNbElements;
    p_stObjectToCreate->xNbPoints   = xNbPoints;

    p_stObjectToCreate->d_xListOfElementsTypes =
        (ACP_tdxIndex *)TMP_M_p_Malloc(sizeof(ACP_tdxIndex) * xNbElements);

    p_stObjectToCreate->d_stListOfElements =
        (void **)TMP_M_p_Malloc(sizeof(void *) * xNbElements);

    p_stObjectToCreate->d_stListOfPoints =
        (MTH3D_tdstVector *)TMP_M_p_Malloc(sizeof(MTH3D_tdstVector) * xNbPoints);

    p_stObjectToCreate->d_stListOfPointsNormals =
        (MTH3D_tdstVector *)TMP_M_p_Malloc(sizeof(MTH3D_tdstVector) * xNbPoints);

    return (ACP_tdxHandleOfObject)p_stObjectToCreate;
}

/* 0x0041ccf0 */
static void GEO_vInitGeometricObject(
    GEO_tdstGeometricObject *p_stObjectToCreate,
    ACP_tdxIndex xNbPoints, ACP_tdxIndex xNbElements)
{
    ACP_tdxIndex xInitCount;

    (void)xNbPoints;

    for (xInitCount = 0; xInitCount < xNbElements; xInitCount++)
        p_stObjectToCreate->d_xListOfElementsTypes[xInitCount] = GEO_C_xElementNULL;

    p_stObjectToCreate->p_stOctree = NULL;

    p_stObjectToCreate->xNbParallelBoxes        = 0;
    p_stObjectToCreate->d_stListOfParallelBoxes = NULL;

    p_stObjectToCreate->ulType = GEO_C_NotALookAt;
}

/* 0x41cd30 */
void GEO_vCreateGeometricObject(
    ACP_tdxHandleOfObject *p_hObject,
    ACP_tdxIndex xNbPoints, ACP_tdxIndex xNbElements)
{
    *p_hObject = GEO_hAllocGeometricObject(xNbPoints, xNbElements);
    GEO_vInitGeometricObject((GEO_tdstGeometricObject *)*p_hObject, xNbPoints, xNbElements);
}

/* 0x0041cd60 */
void GEO_vCreateTMPGeometricObject(
    ACP_tdxHandleOfObject *p_hObject,
    ACP_tdxIndex xNbPoints, ACP_tdxIndex xNbElements)
{
    *p_hObject = GEO_hAllocTMPGeometricObject(xNbPoints, xNbElements);
    GEO_vInitGeometricObject((GEO_tdstGeometricObject *)*p_hObject, xNbPoints, xNbElements);
}

ACP_tdxIndex GEO_xGetGeometricObjectNumberOfPoints(ACP_tdxHandleOfObject hObject)
{
    return hObject->xNbPoints;
}

/* 0x41cda0 */
void GEO_vSetPointOfObject(
    ACP_tdxHandleOfObject hObject,
    MTH3D_tdstVector *p_stPoint,
    ACP_tdxIndex xIndexOfPoint)
{
    *(hObject->d_stListOfPoints + xIndexOfPoint) = *p_stPoint;
}

/* 0x41cdd0 */
void GEO_vGetPointOfObject(
    ACP_tdxHandleOfObject hObject,
    MTH3D_tdstVector *p_stPoint,
    ACP_tdxIndex xIndexOfPoint)
{
    *p_stPoint = *(hObject->d_stListOfPoints + xIndexOfPoint);
}

/* 0x0041ce00 */
ACP_tdxIndex GEO_xGetElementType(ACP_tdxHandleOfObject hObject, ACP_tdxIndex xElement)
{
    return (hObject->d_xListOfElementsTypes[xElement]);
}

ACP_tdxIndex GEO_xGetGeometricObjectNumberOfElementsMax(ACP_tdxHandleOfObject hObject)
{
    return(hObject->xNbElements);
}

void GEO_vCreateObjectListOfPointsMaterial(ACP_tdxHandleOfObject hObject)
{
    if (hObject->d_hListOfPointsMaterial != NULL)
        return;

    GEO_M_CPAMalloc(
        hObject->d_hListOfPointsMaterial,
        GMT_tdxHandleToGameMaterial *,
        (hObject->xNbPoints) * sizeof(GMT_tdxHandleToGameMaterial),
        E_uwGEONotEnoughtMemory);
}

void GEO_vComputeObjectListOfPointsMaterial(ACP_tdxHandleOfObject hObject)
{
    ACP_tdxIndex xElementIndex;
    ACP_tdxIndex xDataElementIndex;
    GEO_tdstElementIndexedTriangles   *p_stElementIndexedTriangles;
    GEO_tdstElementFaceMapDescriptors *p_stElementFaceMapDescriptors;
    GEO_tdstElementSpheres            *p_stElementSpheres;
    GEO_tdstElementAlignedBoxes       *p_stElementAlignedBoxes;
    GEO_tdstElementPoints             *p_stElementPoints;
    GEO_tdstElementLines              *p_stElementLines;
    ACP_tdxIndex xPointIndex;
    ACP_tdxIndex xGlobalPointIndex;
    GEO_tdstIndexedSphere     *p_stIndexedSphere;
    GEO_tdstIndexedAlignedBox *p_stIndexedAlignedBox;
    for (xElementIndex = 0; xElementIndex < hObject->xNbElements; xElementIndex++)
    {
        switch (*(hObject->d_xListOfElementsTypes + xElementIndex))
        {
          case GEO_C_xElementIndexedTriangles:
            p_stElementIndexedTriangles =
                (GEO_tdstElementIndexedTriangles *)(*(hObject->d_stListOfElements + xElementIndex));
            for (xDataElementIndex = 0;
                 xDataElementIndex < p_stElementIndexedTriangles->xNbFaces;
                 xDataElementIndex++)
            {
                for (xPointIndex = 0; xPointIndex < 3; xPointIndex++)
                {
                    xGlobalPointIndex =
                        (p_stElementIndexedTriangles->d_stListOfFacesTripled + xDataElementIndex)
                            ->a3_xIndex[xPointIndex];
                    if (hObject->d_hListOfPointsMaterial)
                        *((GMT_tdxHandleToGameMaterial *)hObject->d_hListOfPointsMaterial + xGlobalPointIndex) =
                            p_stElementIndexedTriangles->hMaterial;
                }
            }
            break;

          case GEO_C_xElementFaceMapDescriptors:
            p_stElementFaceMapDescriptors =
                (GEO_tdstElementFaceMapDescriptors *)(*(hObject->d_stListOfElements + xElementIndex));
            for (xDataElementIndex = 0;
                 xDataElementIndex < p_stElementFaceMapDescriptors->xNbFaces;
                 xDataElementIndex++)
            {
                GEO_tdstFaceMapTriangle *p_stFace =
                    p_stElementFaceMapDescriptors->d_stListOfFacesQuadrupled + xDataElementIndex;
                GMT_tdxHandleToGameMaterial hMaterial;
                GEO_xGetFaceMapDescriptorGameMaterial(p_stFace->hFaceMapDescriptor, &hMaterial);
                for (xPointIndex = 0; xPointIndex < 3; xPointIndex++)
                {
                    xGlobalPointIndex = p_stFace->stFaceTripled.a3_xIndex[xPointIndex];
                    if (hObject->d_hListOfPointsMaterial)
                        *((GMT_tdxHandleToGameMaterial *)hObject->d_hListOfPointsMaterial + xGlobalPointIndex) = hMaterial;
                }
            }
            break;

          case GEO_C_xElementSpheres:
            p_stElementSpheres =
                (GEO_tdstElementSpheres *)(*(hObject->d_stListOfElements + xElementIndex));
            for (xDataElementIndex = 0;
                 xDataElementIndex < p_stElementSpheres->xNbSpheres;
                 xDataElementIndex++)
            {
                p_stIndexedSphere = p_stElementSpheres->d_stListOfSpheres + xDataElementIndex;
                xGlobalPointIndex = p_stIndexedSphere->xCenterPoint;
                if (hObject->d_hListOfPointsMaterial)
                    *((GMT_tdxHandleToGameMaterial *)hObject->d_hListOfPointsMaterial + xGlobalPointIndex) = p_stIndexedSphere->hMaterial;
            }
            break;

          case GEO_C_xElementAlignedBoxes:
            p_stElementAlignedBoxes =
                (GEO_tdstElementAlignedBoxes *)(*(hObject->d_stListOfElements + xElementIndex));
            for (xDataElementIndex = 0;
                 xDataElementIndex < p_stElementAlignedBoxes->xNbAlignedBoxes;
                 xDataElementIndex++)
            {
                p_stIndexedAlignedBox =
                    p_stElementAlignedBoxes->d_stListOfAlignedBoxes + xDataElementIndex;
                xGlobalPointIndex = p_stIndexedAlignedBox->xMinPoint;
                if (hObject->d_hListOfPointsMaterial)
                    *((GMT_tdxHandleToGameMaterial *)hObject->d_hListOfPointsMaterial + xGlobalPointIndex) = p_stIndexedAlignedBox->hMaterial;
                xGlobalPointIndex = p_stIndexedAlignedBox->xMaxPoint;
                if (hObject->d_hListOfPointsMaterial)
                    *((GMT_tdxHandleToGameMaterial *)hObject->d_hListOfPointsMaterial + xGlobalPointIndex) = p_stIndexedAlignedBox->hMaterial;
            }
            break;

          case GEO_C_xElementPoints:
            p_stElementPoints =
                (GEO_tdstElementPoints *)(*(hObject->d_stListOfElements + xElementIndex));
            for (xDataElementIndex = 0;
                 xDataElementIndex < p_stElementPoints->xNbPoints;
                 xDataElementIndex++)
            {
                xGlobalPointIndex = *(p_stElementPoints->d_xListOfPointIndex + xDataElementIndex);
                if (hObject->d_hListOfPointsMaterial)
                    *((GMT_tdxHandleToGameMaterial *)hObject->d_hListOfPointsMaterial + xGlobalPointIndex) = p_stElementPoints->hMaterial;
            }
            break;

          case GEO_C_xElementLines:
            p_stElementLines =
                (GEO_tdstElementLines *)(*(hObject->d_stListOfElements + xElementIndex));
            for (xDataElementIndex = 0;
                 xDataElementIndex < p_stElementLines->xNbLines;
                 xDataElementIndex++)
            {
                xGlobalPointIndex =
                    (p_stElementLines->d_stListOfLineIndex + xDataElementIndex)->a2_xIndex[0];
                if (hObject->d_hListOfPointsMaterial)
                    *((GMT_tdxHandleToGameMaterial *)hObject->d_hListOfPointsMaterial + xGlobalPointIndex) =
                        *(p_stElementLines->d_hListOfLinesMaterial + xDataElementIndex);
                xGlobalPointIndex =
                    (p_stElementLines->d_stListOfLineIndex + xDataElementIndex)->a2_xIndex[1];
                if (hObject->d_hListOfPointsMaterial)
                    *((GMT_tdxHandleToGameMaterial *)hObject->d_hListOfPointsMaterial + xGlobalPointIndex) =
                        *(p_stElementLines->d_hListOfLinesMaterial + xDataElementIndex);
            }
            break;
        }
    }
}

/* 0x41d1a0 */
void GEO_vEndModifyObject(ACP_tdxHandleOfObject hObject)
{
    GEO_xComputeObjectNormals(hObject);           /* 0x0041bdd0 */
    GEO_xComputeIndexUsedByElements(hObject);     /* 0x0041c410 */
    GEO_vComputeObjectListOfParallelBox(hObject); /* 0x0041d9d0 */

    hObject->xBoudingSphereCenter = hObject->d_stListOfPoints[0];
    hObject->xBoudingSphereRadius = 0.0f;
    GEO_xComputeObjectSphereBox(                   /* 0x0041ba30 */
        hObject, &hObject->xBoudingSphereCenter, &hObject->xBoudingSphereRadius);
}

/* 0x0041d1f0 */
void GEO_vEndModifyObject2(ACP_tdxHandleOfObject hObject)
{
    GEO_xComputeObjectNormals2(hObject);           /* 0x0041c3f0 */
    GEO_vComputeObjectListOfParallelBox(hObject);  /* 0x0041d9d0 */

    hObject->xBoudingSphereCenter = hObject->d_stListOfPoints[0];
    hObject->xBoudingSphereRadius = 0.0f;
    GEO_xComputeObjectSphereBox(                   /* 0x0041ba30 */
        hObject, &hObject->xBoudingSphereCenter, &hObject->xBoudingSphereRadius);
}

/* 0x0041D240 */
void
GEO_vGetNormalOfGeometricObjectElement
(
        GEO_tdstGeometricObject *hObject,
        ACP_tdxIndex xElementIndex,
        ACP_tdxIndex xDataElementIndex,
        MTH3D_tdstVector *p_stNormal
)
{
  GEO_tdstElementIndexedTriangles *p_stElementIndexedTriangles;
  GEO_tdstElementFaceMapDescriptors *p_stElementFaceMapDescriptors;

  MTH3D_M_vNullVector ( p_stNormal );

  switch ( hObject->d_xListOfElementsTypes[xElementIndex] )
  {
    case GEO_C_xElementIndexedTriangles :
      p_stElementIndexedTriangles = (GEO_tdstElementIndexedTriangles *)(hObject->d_stListOfElements[xElementIndex]);

      MTH3D_M_vCopyVector ( p_stNormal, (p_stElementIndexedTriangles->d_stListOfFacesNormals + xDataElementIndex) );
    break;

    case GEO_C_xElementFaceMapDescriptors :
      p_stElementFaceMapDescriptors = (GEO_tdstElementFaceMapDescriptors *)(hObject->d_stListOfElements[xElementIndex]);

      MTH3D_M_vCopyVector ( p_stNormal, (p_stElementFaceMapDescriptors->d_stListOfFacesNormals + xDataElementIndex) );
    break;
  }
}

ACP_tdxHandleOfObject GEO_fn_hTransformTableOfObjectInOneObjectForGAM(
   ACP_tdxHandleOfObject _hGeoObj,
   ACP_tdxHandleOfObject * a_hTableOfGeoObj,
   ACP_tdxIndex xNbOfgEOoBJ)
{
    ACP_tdxIndex                        xNbPoints,xNbP;
    GEO_tdstElementPoints               * p_stEPSource,* p_stEPTarget;
    ACP_tdxIndex                        * p_xIPSource ,* p_xIPTarget, *p_xIPSourceEnd;
    GEO_tdstElementSpheres              * p_stESSource,* p_stESTarget;
    GEO_tdstIndexedSphere               * p_stISSource,* p_stISTarget, *p_stISSourceEnd;
    GEO_tdstElementAlignedBoxes         * p_stEBSource,* p_stEBTarget;
    GEO_tdstIndexedAlignedBox           * p_stIBSource,* p_stIBTarget, *p_stIBSourceEnd;
    GEO_tdstElementCones                * p_stECSource,* p_stECTarget;
    GEO_tdstIndexedCone                 * p_stICSource,* p_stICTarget, *p_stICSourceEnd;
    ACP_tdxHandleOfObject               *p_hGeoObjInTableAtIndex, *p_hGeoObjInTableEnd;
    void                                **p_hCurrentElementTarget, **p_hCurrentElementTargetEnd;
    ACP_tdxIndex                        xNbSubElt, *p_xCurrentElementTypeTarget;
    unsigned long                       ulSize, ulSizeVector, ulSizeCone, ulSizeSphere, ulSizeBox;

    p_hCurrentElementTarget = _hGeoObj->d_stListOfElements;
    p_stESSource    = (GEO_tdstElementSpheres *)*p_hCurrentElementTarget++;
    p_stEBSource    = (GEO_tdstElementAlignedBoxes *)*p_hCurrentElementTarget++;
    p_stECSource    = (GEO_tdstElementCones *)*p_hCurrentElementTarget++;
    p_stEPSource    = (GEO_tdstElementPoints *)*p_hCurrentElementTarget;

    p_stISSource    = p_stESSource -> d_stListOfSpheres;
    p_stIBSource    = p_stEBSource -> d_stListOfAlignedBoxes;
    p_stICSource    = p_stECSource -> d_stListOfCones;
    p_xIPSource     = p_stEPSource -> d_xListOfPointIndex;

    xNbPoints       = 0;
    p_stESSource -> xNbSpheres      = 0;
    p_stEBSource -> xNbAlignedBoxes = 0;
    p_stECSource -> xNbCones        = 0;
    p_stEPSource -> xNbPoints       = 0;

    ulSizeVector = sizeof(MTH3D_tdstVector);
    ulSizeSphere = sizeof( GEO_tdstIndexedSphere );
    ulSizeCone   = sizeof( GEO_tdstIndexedCone);
    ulSizeBox    = sizeof( GEO_tdstIndexedAlignedBox );

    p_hGeoObjInTableAtIndex = a_hTableOfGeoObj;
    p_hGeoObjInTableEnd     = p_hGeoObjInTableAtIndex + xNbOfgEOoBJ;
    for( ; p_hGeoObjInTableAtIndex < p_hGeoObjInTableEnd ; p_hGeoObjInTableAtIndex++ )
	{
        xNbP = ( *p_hGeoObjInTableAtIndex ) -> xNbPoints;
        ulSize = ulSizeVector * xNbP;
		if(ulSize!=0)
		{
        memcpy( _hGeoObj -> d_stListOfPoints + xNbPoints,        (*p_hGeoObjInTableAtIndex) -> d_stListOfPoints,        ulSize );
        memcpy( _hGeoObj -> d_stListOfPointsNormals + xNbPoints, (*p_hGeoObjInTableAtIndex) -> d_stListOfPointsNormals, ulSize );

        p_hCurrentElementTarget     = (*p_hGeoObjInTableAtIndex)->d_stListOfElements;
        p_hCurrentElementTargetEnd  = p_hCurrentElementTarget + (ACP_tdxIndex)((*p_hGeoObjInTableAtIndex) -> xNbElements);
        p_xCurrentElementTypeTarget = (*p_hGeoObjInTableAtIndex)->d_xListOfElementsTypes;

        for( ; p_hCurrentElementTarget < p_hCurrentElementTargetEnd ; p_hCurrentElementTarget++,p_xCurrentElementTypeTarget++ )
		{
            switch( *p_xCurrentElementTypeTarget )
			{
			case GEO_C_xElementSpheres:
				p_stESTarget = (GEO_tdstElementSpheres*)*p_hCurrentElementTarget;
                p_stISTarget = ((GEO_tdstElementSpheres*)*p_hCurrentElementTarget) -> d_stListOfSpheres;
                xNbSubElt    = ((GEO_tdstElementSpheres*)*p_hCurrentElementTarget) -> xNbSpheres;
                memcpy( p_stISSource, p_stISTarget, ulSizeSphere * xNbSubElt );
                if( xNbPoints )
                {
                    p_stISSourceEnd = p_stISSource + xNbSubElt;
                    for( ; p_stISSource < p_stISSourceEnd ; p_stISSource++ )
                    {
                        p_stISSource -> xCenterPoint += xNbPoints;
                    }
                }
                else
                {
                    p_stISSource += xNbSubElt;
                }
                p_stESSource -> xNbSpheres += xNbSubElt;
				break;

			case GEO_C_xElementAlignedBoxes:
				p_stEBTarget = (GEO_tdstElementAlignedBoxes *)*p_hCurrentElementTarget;
                p_stIBTarget = ((GEO_tdstElementAlignedBoxes *)*p_hCurrentElementTarget) -> d_stListOfAlignedBoxes;
                xNbSubElt    = ((GEO_tdstElementAlignedBoxes *)*p_hCurrentElementTarget) -> xNbAlignedBoxes;
                memcpy( p_stIBSource, p_stIBTarget, ulSizeBox * xNbSubElt );
                if( xNbPoints )
                {
                    p_stIBSourceEnd = p_stIBSource + xNbSubElt;
                    for ( ; p_stIBSource < p_stIBSourceEnd ; p_stIBSource++ )
                    {
                        p_stIBSource -> xMinPoint += xNbPoints;
                        p_stIBSource -> xMaxPoint += xNbPoints;
                    }
                }
                else
                {
                    p_stIBSource += xNbSubElt;
                }
                p_stEBSource -> xNbAlignedBoxes += xNbSubElt;
				break;

			case GEO_C_xElementCones:
				p_stECTarget = (GEO_tdstElementCones *)*p_hCurrentElementTarget;
                p_stICTarget = ((GEO_tdstElementCones *)*p_hCurrentElementTarget) -> d_stListOfCones;
                xNbSubElt    = ((GEO_tdstElementCones *)*p_hCurrentElementTarget) -> xNbCones;
                memcpy( p_stICSource, p_stICTarget, ulSizeCone * xNbSubElt );
                if( xNbPoints )
                {
                    p_stICSourceEnd = p_stICSource + xNbSubElt;
                    for ( ; p_stICSource < p_stICSourceEnd ; p_stICSource++ )
                    {
                        p_stICSource -> xTopPoint   += xNbPoints;
                        p_stICSource -> xBasePoint  += xNbPoints;
                    }
                }
                else
                {
                    p_stICSource += xNbSubElt;
                }
                p_stECSource -> xNbCones += xNbSubElt;
				break;

			case GEO_C_xElementPoints:
                p_stEPTarget = ((GEO_tdstElementPoints *)*p_hCurrentElementTarget);

                p_stEPSource -> xFatness  = p_stEPTarget -> xFatness;
                p_stEPSource -> hMaterial = p_stEPTarget -> hMaterial;

                p_xIPTarget = p_stEPTarget -> d_xListOfPointIndex;
                xNbSubElt   = p_stEPTarget -> xNbPoints;
                p_xIPSourceEnd = p_xIPSource + xNbSubElt;
                for ( ; p_xIPSource < p_xIPSourceEnd ; p_xIPTarget++,p_xIPSource++ )
                {
                    *p_xIPSource = (*p_xIPTarget) + xNbPoints;
										if ( _hGeoObj->d_hListOfPointsMaterial )
                    ((GMT_tdxHandleToGameMaterial *)_hGeoObj->d_hListOfPointsMaterial)[ *p_xIPSource ] = p_stEPTarget -> hMaterial;
                }
                p_stEPSource -> xNbPoints += xNbSubElt;
				break;

			}

		}
        xNbPoints += xNbP;
   }
   }
   _hGeoObj->xNbPoints = xNbPoints;

   return _hGeoObj;
}

void GEO_vGetFaceSurfaceSpeed(ACP_tdxHandleOfObject hObject,
                              ACP_tdxIndex xElement,
                              ACP_tdxIndex xFace,
                              MTH3D_tdstVector *p_vSpeed)
{
    GEO_tdstElementIndexedTriangles *p_stElement;
    GEO_tdstTripledIndex *p_stListOfTripled;
    MTH3D_tdstVector *p_vA, *p_vB, *p_vC;
    ACP_tdst2DUVValues *p_uvA, *p_uvB, *p_uvC;
    long bScrollMode;
    MTH_tdxReal xUSpeed, xVSpeed;
    MTH2D_tdstVector vABCSpeed2D;

    MTH3D_M_vNullVector(p_vSpeed);

    if (hObject->d_xListOfElementsTypes[xElement] != GEO_C_xElementIndexedTriangles)
        return;

    p_stElement = hObject->d_stListOfElements[xElement];

    GLI_xGetMaterialTextureScrollingCoef(
        GMT_fn_hGetVisualMaterial(p_stElement->hMaterial),
        0,
        &bScrollMode,
        &xUSpeed,
        &xVSpeed);

    xUSpeed = MTH_M_xMul(MTH_M_xLongToReal(60), MTH_M_xNeg(xUSpeed));
    xVSpeed = MTH_M_xMul(MTH_M_xLongToReal(60), MTH_M_xNeg(xVSpeed));

    p_stListOfTripled = p_stElement->d_stListOfFacesTripled + xFace;
    p_vA = hObject->d_stListOfPoints + p_stListOfTripled->a3_xIndex[0];
    p_vB = hObject->d_stListOfPoints + p_stListOfTripled->a3_xIndex[1];
    p_vC = hObject->d_stListOfPoints + p_stListOfTripled->a3_xIndex[2];

    p_stListOfTripled = p_stElement->d_stListOfFacesTripledIndexUV + xFace;
    p_uvA = p_stElement->d_stListOfElementUV + p_stListOfTripled->a3_xIndex[0];
    p_uvB = p_stElement->d_stListOfElementUV + p_stListOfTripled->a3_xIndex[1];
    p_uvC = p_stElement->d_stListOfElementUV + p_stListOfTripled->a3_xIndex[2];

    {
        MTH2D_tdstMatrix mUVToABC, mABCToUV;
        MTH2D_tdstVector vUVSpeed, vAB, vAC;

        MTH2D_M_vSetVectorElements(&vUVSpeed, xUSpeed, xVSpeed);

        MTH2D_M_vSetVectorElements(&vAB, p_uvB->xU - p_uvA->xU, p_uvB->xV - p_uvA->xV);
        MTH2D_M_vSetVectorElements(&vAC, p_uvC->xU - p_uvA->xU, p_uvC->xV - p_uvA->xV);

        MTH2D_M_vSetVectorsInMatrix(&mABCToUV, &vAB, &vAC);
        MTH2D_M_vInverMatrix(&mUVToABC, &mABCToUV);

        MTH2D_M_vMulMatrixVectorWithoutBuffer(&vABCSpeed2D, &mUVToABC, &vUVSpeed);
    }

    {
        MTH3D_tdstMatrix mABCToXYZ;
        MTH3D_tdstVector vABCSpeed3D, vAB, vAC, vN;

        MTH3D_M_vSetVectorElements(&vABCSpeed3D, MTH2D_M_xGetXofVector(&vABCSpeed2D), MTH2D_M_xGetYofVector(&vABCSpeed2D), MTH_C_ZERO);

        MTH3D_M_vSubVector(&vAB, p_vB, p_vA);
        MTH3D_M_vSubVector(&vAC, p_vC, p_vA);
        MTH3D_M_vNullVector(&vN);

        MTH3D_M_vSetVectorsInMatrix(&mABCToXYZ, &vAB, &vAC, &vN);

        MTH3D_M_vMulMatrixVectorWithoutBuffer(p_vSpeed, &mABCToXYZ, &vABCSpeed3D);
    }
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

void GEO_vComputeObjectListOfParallelBox(ACP_tdxHandleOfObject hObject)
{
    ACP_tdxIndex xIndex;
    if (hObject->xNbParallelBoxes == 0) return;
    if (hObject->d_stListOfParallelBoxes == NULL)
        GEO_vCreateObjectListOfParallelBox(hObject, hObject->xNbParallelBoxes);
    for (xIndex = 0; xIndex < hObject->xNbParallelBoxes; xIndex++)
        GEO_vComputeObjectParallelBox(hObject, xIndex);
}

/* 0x0045dc60 */
GEO_tdxHandleToParallelBox GEO_hGetParallelBox(
    ACP_tdxHandleOfObject hObject,
    ACP_tdxIndex          xParallelBox)
{
    if (xParallelBox < hObject->xNbParallelBoxes)
        return hObject->d_stListOfParallelBoxes + xParallelBox;
    return NULL;
}

ACP_tdxIndex GEO_xGetParallelBoxIndexOfElement(
    ACP_tdxHandleOfObject hObject,
    ACP_tdxIndex          xIndexOfElement)
{
    switch (hObject->d_xListOfElementsTypes[xIndexOfElement]) {
    case GEO_C_xElementIndexedTriangles:
        return ((GEO_tdstElementIndexedTriangles *)(hObject->d_stListOfElements[xIndexOfElement]))->xIndexOfParallelBox;
    case GEO_C_xElementFaceMapDescriptors:
        return ((GEO_tdstElementFaceMapDescriptors *)(hObject->d_stListOfElements[xIndexOfElement]))->xIndexOfParallelBox;
    case GEO_C_xElementTMeshes:
        return ((GEO_tdstElementTMeshes *)(hObject->d_stListOfElements[xIndexOfElement]))->xIndexOfParallelBox;
    case GEO_C_xElementPoints:
        return ((GEO_tdstElementPoints *)(hObject->d_stListOfElements[xIndexOfElement]))->xIndexOfParallelBox;
    case GEO_C_xElementLines:
        return ((GEO_tdstElementLines *)(hObject->d_stListOfElements[xIndexOfElement]))->xIndexOfParallelBox;
    case GEO_C_xElementSprites:
    case GEO_C_xElementSpheres:
    case GEO_C_xElementAlignedBoxes:
    case GEO_C_xElementCones:

        return ((GEO_tdstElementSpheres *)(hObject->d_stListOfElements[xIndexOfElement]))->xIndexOfParallelBox;
    case 10:
        return GEO_C_xNoParallelBox;
    case 11:
        return GEO_C_xNoParallelBox;
    default:
        return GEO_C_xNoParallelBox;
    }
}

void GEO_xGetFaceMapDescriptorGameMaterial(ACP_tdxHandleOfFMD hFmd, GMT_tdxHandleToGameMaterial *p_hMaterial)
{
    *p_hMaterial = hFmd->hMaterial;
}

/* 0x0041faa0 */
void GEO_fn_vUpdateMinMaxBoxWithPoint(
    MTH3D_tdstVector *p_stMinPoint,
    MTH3D_tdstVector *p_stMaxPoint,
    MTH3D_tdstVector *p_stPoint)
{
    MTH3D_tdstVector *p_stPointToTest = p_stPoint;

    if( p_stPointToTest->xX < p_stMinPoint->xX )
    {
        p_stMinPoint->xX = p_stPointToTest->xX;
    }
    else if( p_stPointToTest->xX > p_stMaxPoint->xX )
    {
        p_stMaxPoint->xX = p_stPointToTest->xX;
    }
    if( MTH3D_M_xGetYofVector( p_stPointToTest ) < MTH3D_M_xGetYofVector( p_stMinPoint ) )
    {
        MTH3D_M_vSetYofVector( p_stMinPoint,  MTH3D_M_xGetYofVector( p_stPointToTest ) );
    }
    else if( MTH3D_M_xGetYofVector( p_stPointToTest ) > MTH3D_M_xGetYofVector( p_stMaxPoint ) )
    {
        MTH3D_M_vSetYofVector( p_stMaxPoint,  MTH3D_M_xGetYofVector( p_stPointToTest ) );
    }
    if( MTH3D_M_xGetZofVector( p_stPointToTest ) < MTH3D_M_xGetZofVector( p_stMinPoint ) )
    {
        MTH3D_M_vSetZofVector( p_stMinPoint,  MTH3D_M_xGetZofVector( p_stPointToTest ) );
    }
    else if( MTH3D_M_xGetZofVector( p_stPointToTest ) > MTH3D_M_xGetZofVector( p_stMaxPoint ) )
    {
        MTH3D_M_vSetZofVector( p_stMaxPoint,  MTH3D_M_xGetZofVector( p_stPointToTest ) );
    }
}

void GEO_vComputeObjectParallelBox(
    ACP_tdxHandleOfObject hObject,
    ACP_tdxIndex          xParallelBox)
{
    GEO_tdstElementIndexedTriangles     *p_stETIT;
    GEO_tdstElementPoints               *p_stEIP;
    GEO_tdstElementLines                *p_stEL;
    GEO_tdstElementSpheres              *p_stES;
    GEO_tdstElementAlignedBoxes         *p_stEAB;
    GEO_tdstElementCones                *p_stEC;
    GEO_tdstElementSprite               *p_stESt;
    GEO_tdstElementFaceMapDescriptors   *p_stEFMD;
    ACP_tdxIndex                        xIndexElement , xIndexInElement;
    MTH3D_tdstVector                    *p_stMinPoint , *p_stMaxPoint;
    GEO_tdxHandleToParallelBox          hParallelBox;
    BOOL                                bInit;

    bInit = FALSE;
    hParallelBox = GEO_hGetParallelBox( hObject, xParallelBox );

    if( hParallelBox == NULL )  return;

    p_stMinPoint = & hParallelBox -> stMinPoint;
    p_stMaxPoint = & hParallelBox -> stMaxPoint;

    for ( xIndexElement = 0 ; xIndexElement < hObject -> xNbElements ; xIndexElement++ )
    {
        if( GEO_xGetParallelBoxIndexOfElement( hObject, xIndexElement ) == xParallelBox )
        {
            switch ( hObject -> d_xListOfElementsTypes[ xIndexElement ] )
            {
            case GEO_C_xElementIndexedTriangles:
                p_stETIT = (GEO_tdstElementIndexedTriangles*) ( hObject -> d_stListOfElements[ xIndexElement ] );
                if( !bInit && p_stETIT -> xNbFaces )
                {
                    MTH3D_M_vCopyVector( p_stMinPoint, hObject -> d_stListOfPoints + p_stETIT -> d_stListOfFacesTripled [ 0 ] . a3_xIndex[0] );
                    MTH3D_M_vCopyVector( p_stMaxPoint, hObject -> d_stListOfPoints + p_stETIT -> d_stListOfFacesTripled [ 0 ] . a3_xIndex[0] );
                    bInit = TRUE;
                }
                for ( xIndexInElement = 0 ; xIndexInElement < p_stETIT -> xNbFaces ; xIndexInElement++ )
                {
                    GEO_fn_vUpdateMinMaxBoxWithPoint( p_stMinPoint, p_stMaxPoint, hObject -> d_stListOfPoints + p_stETIT -> d_stListOfFacesTripled [ xIndexInElement ] . a3_xIndex[0] );
                    GEO_fn_vUpdateMinMaxBoxWithPoint( p_stMinPoint, p_stMaxPoint, hObject -> d_stListOfPoints + p_stETIT -> d_stListOfFacesTripled [ xIndexInElement ] . a3_xIndex[1] );
                    GEO_fn_vUpdateMinMaxBoxWithPoint( p_stMinPoint, p_stMaxPoint, hObject -> d_stListOfPoints + p_stETIT -> d_stListOfFacesTripled [ xIndexInElement ] . a3_xIndex[2] );
                }
                break;

            case GEO_C_xElementFaceMapDescriptors:
                p_stEFMD = (GEO_tdstElementFaceMapDescriptors*) ( hObject -> d_stListOfElements[ xIndexElement ] );
                if( !bInit && p_stEFMD -> xNbFaces )
                {
                    MTH3D_M_vCopyVector( p_stMinPoint, hObject -> d_stListOfPoints + p_stEFMD -> d_stListOfFacesQuadrupled [ 0 ] . stFaceTripled . a3_xIndex[0] );
                    MTH3D_M_vCopyVector( p_stMaxPoint, hObject -> d_stListOfPoints + p_stEFMD -> d_stListOfFacesQuadrupled [ 0 ] . stFaceTripled . a3_xIndex[0] );
                    bInit = TRUE;
                }
                for ( xIndexInElement = 0 ; xIndexInElement < p_stEFMD -> xNbFaces ; xIndexInElement++ )
                {
                    GEO_fn_vUpdateMinMaxBoxWithPoint( p_stMinPoint, p_stMaxPoint, hObject -> d_stListOfPoints + p_stEFMD -> d_stListOfFacesQuadrupled [ xIndexInElement ] . stFaceTripled . a3_xIndex[0] );
                    GEO_fn_vUpdateMinMaxBoxWithPoint( p_stMinPoint, p_stMaxPoint, hObject -> d_stListOfPoints + p_stEFMD -> d_stListOfFacesQuadrupled [ xIndexInElement ] . stFaceTripled . a3_xIndex[1] );
                    GEO_fn_vUpdateMinMaxBoxWithPoint( p_stMinPoint, p_stMaxPoint, hObject -> d_stListOfPoints + p_stEFMD -> d_stListOfFacesQuadrupled [ xIndexInElement ] . stFaceTripled . a3_xIndex[2] );
                }
                break;
            case GEO_C_xElementSprites:
                p_stESt = (GEO_tdstElementSprite*) ( hObject -> d_stListOfElements[ xIndexElement ] );

                if( !bInit && p_stESt -> xNbSprites )
                {
                    MTH3D_M_vCopyVector( p_stMinPoint, hObject -> d_stListOfPoints + p_stESt -> d_stListOfSprites [ 0 ] . xCenterPoint );
                    MTH3D_M_vCopyVector( p_stMaxPoint, hObject -> d_stListOfPoints + p_stESt -> d_stListOfSprites [ 0 ] . xCenterPoint );
                    bInit = TRUE;
                }
                for ( xIndexInElement = 0 ; xIndexInElement < p_stESt -> xNbSprites ; xIndexInElement++ )
                {
                    GEO_fn_vUpdateMinMaxBoxWithPoint( p_stMinPoint, p_stMaxPoint, hObject -> d_stListOfPoints + p_stESt -> d_stListOfSprites [ xIndexInElement ] . xCenterPoint );
                }
                break;
            case GEO_C_xElementPoints:
                p_stEIP = (GEO_tdstElementPoints*) ( hObject -> d_stListOfElements[ xIndexElement ] );
                if( !bInit && p_stEIP -> xNbPoints )
                {
                    MTH3D_M_vCopyVector( p_stMinPoint, hObject -> d_stListOfPoints + p_stEIP -> d_xListOfPointIndex [ 0 ] );
                    MTH3D_M_vCopyVector( p_stMaxPoint, hObject -> d_stListOfPoints + p_stEIP -> d_xListOfPointIndex [ 0 ] );
                    bInit = TRUE;
                }
                for ( xIndexInElement = 0 ; xIndexInElement < p_stEIP -> xNbPoints ; xIndexInElement++ )
                {
                    GEO_fn_vUpdateMinMaxBoxWithPoint( p_stMinPoint, p_stMaxPoint, hObject -> d_stListOfPoints + p_stEIP -> d_xListOfPointIndex [ xIndexInElement ] );
                }
                break;
            case GEO_C_xElementLines:
                p_stEL = (GEO_tdstElementLines*) ( hObject -> d_stListOfElements[ xIndexElement ] );
                if( !bInit && p_stEL -> xNbLines )
                {
                    MTH3D_M_vCopyVector( p_stMinPoint, hObject -> d_stListOfPoints + p_stEL -> d_stListOfLineIndex [ 0 ] . a2_xIndex[ 0 ] );
                    MTH3D_M_vCopyVector( p_stMaxPoint, hObject -> d_stListOfPoints + p_stEL -> d_stListOfLineIndex [ 0 ] . a2_xIndex[ 0 ] );
                    bInit = TRUE;
                }
                for ( xIndexInElement = 0 ; xIndexInElement < p_stEL -> xNbLines ; xIndexInElement++ )
                {
                    GEO_fn_vUpdateMinMaxBoxWithPoint( p_stMinPoint, p_stMaxPoint, hObject -> d_stListOfPoints + p_stEL -> d_stListOfLineIndex [ xIndexInElement ] . a2_xIndex[ 0 ] );
                    GEO_fn_vUpdateMinMaxBoxWithPoint( p_stMinPoint, p_stMaxPoint, hObject -> d_stListOfPoints + p_stEL -> d_stListOfLineIndex [ xIndexInElement ] . a2_xIndex[ 1 ] );
                }
                break;
            case GEO_C_xElementSpheres:
                p_stES = (GEO_tdstElementSpheres*) ( hObject -> d_stListOfElements[ xIndexElement ] );
                if( !bInit && p_stES -> xNbSpheres )
                {
                    MTH3D_M_vCopyVector( p_stMinPoint, hObject -> d_stListOfPoints + p_stES -> d_stListOfSpheres [ 0 ] . xCenterPoint );
                    MTH3D_M_vCopyVector( p_stMaxPoint, hObject -> d_stListOfPoints + p_stES -> d_stListOfSpheres [ 0 ] . xCenterPoint );
                    bInit = TRUE;
                }
                for ( xIndexInElement = 0 ; xIndexInElement < p_stES -> xNbSpheres ; xIndexInElement++ )
                {
                    GEO_fn_vUpdateMinMaxBoxWithPoint( p_stMinPoint, p_stMaxPoint, hObject -> d_stListOfPoints + p_stES -> d_stListOfSpheres [ xIndexInElement ] . xCenterPoint );
                }
                break;
            case GEO_C_xElementAlignedBoxes:
                p_stEAB = (GEO_tdstElementAlignedBoxes*) ( hObject -> d_stListOfElements[ xIndexElement ] );
                if( !bInit && p_stEAB -> xNbAlignedBoxes )
                {
                    MTH3D_M_vCopyVector( p_stMinPoint, hObject -> d_stListOfPoints + p_stEAB -> d_stListOfAlignedBoxes [ 0 ] . xMinPoint );
                    MTH3D_M_vCopyVector( p_stMaxPoint, hObject -> d_stListOfPoints + p_stEAB -> d_stListOfAlignedBoxes [ 0 ] . xMinPoint );
                    bInit = TRUE;
                }
                for ( xIndexInElement = 0 ; xIndexInElement < p_stEAB -> xNbAlignedBoxes ; xIndexInElement++ )
                {
                    GEO_fn_vUpdateMinMaxBoxWithPoint( p_stMinPoint, p_stMaxPoint, hObject -> d_stListOfPoints + p_stEAB -> d_stListOfAlignedBoxes [ xIndexInElement ] . xMinPoint );
                    GEO_fn_vUpdateMinMaxBoxWithPoint( p_stMinPoint, p_stMaxPoint, hObject -> d_stListOfPoints + p_stEAB -> d_stListOfAlignedBoxes [ xIndexInElement ] . xMaxPoint );
                }
                break;
            case GEO_C_xElementCones:
                p_stEC = (GEO_tdstElementCones*) ( hObject -> d_stListOfElements[ xIndexElement ] );
                if( !bInit && p_stEC -> xNbCones )
                {
                    MTH3D_M_vCopyVector( p_stMinPoint, hObject -> d_stListOfPoints + p_stEC -> d_stListOfCones [ 0 ] . xTopPoint );
                    MTH3D_M_vCopyVector( p_stMaxPoint, hObject -> d_stListOfPoints + p_stEC -> d_stListOfCones [ 0 ] . xTopPoint );
                    bInit = TRUE;
                }
                for ( xIndexInElement = 0 ; xIndexInElement < p_stEC -> xNbCones ; xIndexInElement++ )
                {
                    GEO_fn_vUpdateMinMaxBoxWithPoint( p_stMinPoint, p_stMaxPoint, hObject -> d_stListOfPoints + p_stEC -> d_stListOfCones [ xIndexInElement ] . xTopPoint );
                    GEO_fn_vUpdateMinMaxBoxWithPoint( p_stMinPoint, p_stMaxPoint, hObject -> d_stListOfPoints + p_stEC -> d_stListOfCones [ xIndexInElement ] . xBasePoint );
                }
                break;
            }
        }
    }
}
