#include "GEO/GEO_types.h"
#include "GEO/Element.h"
#include "GMT/GMT_types.h"
#include "GMT/GmtFunct.h"
#include "GEO/GeoMem.h"

GEO_tdstElementSpheres *GEO_p_stAllocElementSpheres(ACP_tdxIndex xNbSpheres);

typedef void (__cdecl *tFUN_0042db20)(GMT_tdxHandleToGameMaterial, ACP_tdxHandleOfMaterial);

void GEO_vSetHandleOfIndexedSprite(
    ACP_tdxHandleOfObject  hObject,
    ACP_tdxHandleOfElement hElement,
    ACP_tdxIndex           xIndexOfSprite,
    ACP_tdxHandleOfSprite  hSprite)
{
    GEO_tdstElementSprite *p = (GEO_tdstElementSprite *)hObject->d_stListOfElements[hElement];
    p->d_stListOfSprites[xIndexOfSprite].hSprite = hSprite;
}

GEO_tdstElementIndexedTriangles *GEO_p_stAllocElementIndexedTriangles( ACP_tdxIndex xNbFaces, ACP_tdxIndex xNbElementUV);

void GEO_vCreateElementIndexedTriangles(
    ACP_tdxHandleOfObject   hObject,
    ACP_tdxHandleOfElement *p_hElement,
    ACP_tdxIndex            xNbFaces,
    ACP_tdxIndex            xNbElementUV)
{
    ACP_tdxIndex xElementToBuild;

    for (xElementToBuild = 0;
         xElementToBuild < hObject->xNbElements;
         xElementToBuild++)
        if (hObject->d_xListOfElementsTypes[xElementToBuild] == GEO_C_xElementNULL)
            break;
    if (xElementToBuild == hObject->xNbElements)
        return;
    hObject->d_xListOfElementsTypes[xElementToBuild] = GEO_C_xElementIndexedTriangles;
    *p_hElement = xElementToBuild;
    hObject->d_stListOfElements[xElementToBuild] = GEO_p_stAllocElementIndexedTriangles(xNbFaces, xNbElementUV);
}

/* 0x0041dd00 */
GEO_tdstElementIndexedTriangles *GEO_p_stAllocElementIndexedTriangles(
    ACP_tdxIndex xNbFaces,
    ACP_tdxIndex xNbElementUV)
{
    GEO_tdstElementIndexedTriangles *p_stLocalElement;

    if (xNbElementUV == 0)
        xNbElementUV = 1;

    GEO_M_CPAMalloc(p_stLocalElement, GEO_tdstElementIndexedTriangles *,
        sizeof(GEO_tdstElementIndexedTriangles), E_uwGEONotEnoughtMemory);

    GEO_M_CPAMalloc(p_stLocalElement->d_stListOfFacesTripled, GEO_tdstTripledIndex *,
        sizeof(GEO_tdstTripledIndex) * xNbFaces, E_uwGEONotEnoughtMemory);

    GEO_M_CPAMalloc(p_stLocalElement->d_stListOfFacesTripledIndexUV, GEO_tdstTripledIndex *,
        sizeof(GEO_tdstTripledIndex) * xNbFaces, E_uwGEONotEnoughtMemory);

    GEO_M_CPAMalloc(p_stLocalElement->d_stListOfFacesNormals, MTH3D_tdstVector *,
        sizeof(MTH3D_tdstVector) * xNbFaces, E_uwGEONotEnoughtMemory);

    GEO_M_CPAMalloc(p_stLocalElement->d_stListOfElementUV, ACP_tdst2DUVValues *,
        sizeof(ACP_tdst2DUVValues) * xNbElementUV, E_uwGEONotEnoughtMemory);

    p_stLocalElement->xNbElementUV = xNbElementUV;
    p_stLocalElement->xNbFaces = xNbFaces;

    p_stLocalElement->xNumberOfIndexsUsed = 0;
    p_stLocalElement->d_stListOfIndexUsedByThisElement = NULL;
    p_stLocalElement->xIndexOfParallelBox = GEO_C_xNoParallelBox;

    return p_stLocalElement;
}

void GEO_vSetFaceOfIndexedTriangles(
    ACP_tdxHandleOfObject  hObject,
    ACP_tdxHandleOfElement hElement,
    ACP_tdxIndex           xIndexOfFace,
    ACP_tdxIndex           xPoint1,
    ACP_tdxIndex           xPoint2,
    ACP_tdxIndex           xPoint3)
{
    GEO_tdstElementIndexedTriangles *p =
        (GEO_tdstElementIndexedTriangles *)hObject->d_stListOfElements[hElement];
    p->d_stListOfFacesTripled[xIndexOfFace].a3_xIndex[0] = xPoint1;
    p->d_stListOfFacesTripled[xIndexOfFace].a3_xIndex[1] = xPoint2;
    p->d_stListOfFacesTripled[xIndexOfFace].a3_xIndex[2] = xPoint3;
}

void GEO_vSetIndexedUVOfFaceOfIndexedTriangles(
    ACP_tdxHandleOfObject  hObject,
    ACP_tdxHandleOfElement hElement,
    ACP_tdxIndex           xIndexOfFace,
    ACP_tdxIndex           xUV1,
    ACP_tdxIndex           xUV2,
    ACP_tdxIndex           xUV3)
{
    GEO_tdstElementIndexedTriangles *p =
        (GEO_tdstElementIndexedTriangles *)hObject->d_stListOfElements[hElement];
    p->d_stListOfFacesTripledIndexUV[xIndexOfFace].a3_xIndex[0] = xUV1;
    p->d_stListOfFacesTripledIndexUV[xIndexOfFace].a3_xIndex[1] = xUV2;
    p->d_stListOfFacesTripledIndexUV[xIndexOfFace].a3_xIndex[2] = xUV3;
}

void GEO_vSetUVOfIndexedTriangles(
    ACP_tdxHandleOfObject  hObject,
    ACP_tdxHandleOfElement hElement,
    ACP_tdxIndex           xIndexOfUV,
    ACP_tdst2DUVValues    *p_stUV)
{
    GEO_tdstElementIndexedTriangles *p =
        (GEO_tdstElementIndexedTriangles *)hObject->d_stListOfElements[hElement];
    p->d_stListOfElementUV[xIndexOfUV] = *p_stUV;
}

void GEO_vGetMaterialOfIndexedTriangles(
    ACP_tdxHandleOfObject    hObject,
    ACP_tdxHandleOfElement   hElement,
    ACP_tdxHandleOfMaterial *p_hMaterial)
{
    GEO_tdstElementIndexedTriangles *p =
        (GEO_tdstElementIndexedTriangles *)hObject->d_stListOfElements[hElement];
    *p_hMaterial = GMT_fn_hGetVisualMaterial(p->hMaterial);
}

void GEO_vSetGameMaterialOfIndexedTriangles(
    ACP_tdxHandleOfObject       hObject,
    ACP_tdxHandleOfElement      hElement,
    GMT_tdxHandleToGameMaterial hMaterial)
{
    GEO_tdstElementIndexedTriangles *p =
        (GEO_tdstElementIndexedTriangles *)hObject->d_stListOfElements[hElement];
    p->hMaterial = hMaterial;
}

void GEO_vGetGameMaterialOfIndexedTriangles(
    ACP_tdxHandleOfObject        hObject,
    ACP_tdxHandleOfElement       hElement,
    GMT_tdxHandleToGameMaterial *p_hMaterial)
{
    GEO_tdstElementIndexedTriangles *p =
        (GEO_tdstElementIndexedTriangles *)hObject->d_stListOfElements[hElement];
    *p_hMaterial = p->hMaterial;
}

GEO_tdstElementPoints *GEO_p_stAllocElementPoints(ACP_tdxIndex xNumberOfPoints);

void GEO_xCreateElementPoints(
    ACP_tdxHandleOfObject   hObject,
    ACP_tdxHandleOfElement *p_hElement,
    ACP_tdxIndex            xNumberOfPoints)
{
    ACP_tdxIndex xElementToBuild;

    for (xElementToBuild = 0;
         xElementToBuild < hObject->xNbElements;
         xElementToBuild++)
        if (hObject->d_xListOfElementsTypes[xElementToBuild] == GEO_C_xElementNULL)
            break;
    if (xElementToBuild == hObject->xNbElements)
        return;
    hObject->d_xListOfElementsTypes[xElementToBuild] = GEO_C_xElementPoints;
    *p_hElement = xElementToBuild;
    hObject->d_stListOfElements[xElementToBuild] = GEO_p_stAllocElementPoints(xNumberOfPoints);
}

/* 0x0041e3e0 */
GEO_tdstElementPoints *GEO_p_stAllocElementPoints(ACP_tdxIndex xNumberOfPoints)
{
    GEO_tdstElementPoints *p_stLocalElement;

    GEO_M_CPAMalloc(p_stLocalElement, GEO_tdstElementPoints *,
        sizeof(GEO_tdstElementPoints), E_uwGEONotEnoughtMemory);

    p_stLocalElement->xNbPoints = xNumberOfPoints;
    GEO_M_CPAMalloc(p_stLocalElement->d_xListOfPointIndex, ACP_tdxIndex *,
        sizeof(ACP_tdxIndex) * xNumberOfPoints, E_uwGEONotEnoughtMemory);

    p_stLocalElement->xIndexOfParallelBox = GEO_C_xNoParallelBox;

    return p_stLocalElement;
}

GMT_tdxHandleToGameMaterial GEO_xGetGameMaterialOfElementPoints(
    ACP_tdxHandleOfObject  hObject,
    ACP_tdxHandleOfElement hElement)
{
    return ((GEO_tdstElementPoints *)hObject->d_stListOfElements[hElement])->hMaterial;
}

void GEO_vCreateElementSpheres(
    ACP_tdxHandleOfObject   hObject,
    ACP_tdxHandleOfElement *p_hElement,
    ACP_tdxIndex            xNbSpheres)
{
    ACP_tdxIndex xElementToBuild;

    for (xElementToBuild = 0;
         xElementToBuild < hObject->xNbElements;
         xElementToBuild++)
        if (hObject->d_xListOfElementsTypes[xElementToBuild] == GEO_C_xElementNULL)
            break;
    if (xElementToBuild == hObject->xNbElements)
        return;
    hObject->d_xListOfElementsTypes[xElementToBuild] = GEO_C_xElementSpheres;
    *p_hElement = xElementToBuild;
    hObject->d_stListOfElements[xElementToBuild] = GEO_p_stAllocElementSpheres(xNbSpheres);
}

void GEO_vSetCenterPointOfIndexedSphere(
    ACP_tdxHandleOfObject  hObject,
    ACP_tdxHandleOfElement hElement,
    ACP_tdxIndex           xNumberOfSphere,
    ACP_tdxIndex           xIndexOfPoint)
{
    ((GEO_tdstElementSpheres *)hObject->d_stListOfElements[hElement])
        ->d_stListOfSpheres[xNumberOfSphere].xCenterPoint = xIndexOfPoint;
}

ACP_tdxIndex GEO_xGetCenterPointOfIndexedSphere(
    ACP_tdxHandleOfObject  hObject,
    ACP_tdxHandleOfElement hElement,
    ACP_tdxIndex           xNumberOfSphere)
{
    return ((GEO_tdstElementSpheres *)hObject->d_stListOfElements[hElement])
        ->d_stListOfSpheres[xNumberOfSphere].xCenterPoint;
}

void GEO_vSetRadiusOfIndexedSphere(
    ACP_tdxHandleOfObject  hObject,
    ACP_tdxHandleOfElement hElement,
    ACP_tdxIndex           xNumberOfSphere,
    MTH_tdxReal            xRadius)
{
    GEO_tdstElementSpheres *p = (GEO_tdstElementSpheres *)hObject->d_stListOfElements[hElement];
    p->d_stListOfSpheres[xNumberOfSphere].xRadius = xRadius;
}

/* 0x41e6f0 */
MTH_tdxReal GEO_xGetRadiusOfIndexedSphere(
    ACP_tdxHandleOfObject  hObject,
    ACP_tdxHandleOfElement hElement,
    ACP_tdxIndex           xNumberOfSphere)
{
    return ((GEO_tdstElementSpheres *)hObject->d_stListOfElements[hElement])
        ->d_stListOfSpheres[xNumberOfSphere].xRadius;
}

#define FUN_0042db20 ((tFUN_0042db20)&GMT_fn_vSetVisualMaterial)

void GEO_vSetMaterialOfIndexedSphere(
    ACP_tdxHandleOfObject   hObject,
    ACP_tdxHandleOfElement  hElement,
    ACP_tdxIndex            xNumberOfSphere,
    ACP_tdxHandleOfMaterial hMaterial)
{
    FUN_0042db20(((GEO_tdstElementSpheres *)hObject->d_stListOfElements[hElement])
        ->d_stListOfSpheres[xNumberOfSphere].hMaterial, hMaterial);
}

void GEO_vSetGameMaterialOfIndexedSphere(
    ACP_tdxHandleOfObject       hObject,
    ACP_tdxHandleOfElement      hElement,
    ACP_tdxIndex                xNumberOfSphere,
    GMT_tdxHandleToGameMaterial hMaterial)
{
    GEO_tdstElementSpheres *p = (GEO_tdstElementSpheres *)hObject->d_stListOfElements[hElement];
    p->d_stListOfSpheres[xNumberOfSphere].hMaterial = hMaterial;
}

ACP_tdxIndex GEO_lGetIndexedSphereNbSpheres(
    ACP_tdxHandleOfObject  hObject,
    ACP_tdxHandleOfElement hElement)
{
    return ((GEO_tdstElementSpheres *)hObject->d_stListOfElements[hElement])->xNbSpheres;
}

GMT_tdxHandleToGameMaterial GEO_hGetGameMaterialOfIndexedSphere(
    ACP_tdxHandleOfObject  hObject,
    ACP_tdxHandleOfElement hElement,
    ACP_tdxIndex           xNumberOfSphere)
{
    return ((GEO_tdstElementSpheres *)hObject->d_stListOfElements[hElement])
        ->d_stListOfSpheres[xNumberOfSphere].hMaterial;
}

GEO_tdstElementAlignedBoxes *GEO_p_stAllocElementAlignedBoxes(ACP_tdxIndex xNbAlignedBoxes);

void GEO_vCreateElementAlignedBoxes(
    ACP_tdxHandleOfObject   hObject,
    ACP_tdxHandleOfElement *p_hElement,
    ACP_tdxIndex            xNbAlignedBoxes)
{
    ACP_tdxIndex xElementToBuild;

    for (xElementToBuild = 0;
         xElementToBuild < hObject->xNbElements;
         xElementToBuild++)
        if (hObject->d_xListOfElementsTypes[xElementToBuild] == GEO_C_xElementNULL)
            break;
    if (xElementToBuild == hObject->xNbElements)
        return;
    hObject->d_xListOfElementsTypes[xElementToBuild] = GEO_C_xElementAlignedBoxes;
    *p_hElement = xElementToBuild;
    hObject->d_stListOfElements[xElementToBuild] = GEO_p_stAllocElementAlignedBoxes(xNbAlignedBoxes);
}

/* 0x0041e810 */
GEO_tdstElementAlignedBoxes *GEO_p_stAllocElementAlignedBoxes(ACP_tdxIndex xNbAlignedBoxes)
{
    GEO_tdstElementAlignedBoxes *p_stLocalElement;

    GEO_M_CPAMalloc(p_stLocalElement, GEO_tdstElementAlignedBoxes *,
        sizeof(GEO_tdstElementAlignedBoxes), E_uwGEONotEnoughtMemory);

    GEO_M_CPAMalloc(p_stLocalElement->d_stListOfAlignedBoxes, GEO_tdstIndexedAlignedBox *,
        sizeof(GEO_tdstIndexedAlignedBox) * xNbAlignedBoxes, E_uwGEONotEnoughtMemory);

    p_stLocalElement->xNbAlignedBoxes = xNbAlignedBoxes;
    p_stLocalElement->xIndexOfParallelBox = GEO_C_xNoParallelBox;

    return p_stLocalElement;
}

void GEO_vGetMinMaxPointOfIndexedAlignedBox(
    ACP_tdxHandleOfObject  hObject,
    ACP_tdxHandleOfElement hElement,
    ACP_tdxIndex           xNumberOfAlignedBox,
    ACP_tdxIndex          *p_xMinPoint,
    ACP_tdxIndex          *p_xMaxPoint)
{

    *p_xMinPoint = ((GEO_tdstElementAlignedBoxes *)hObject->d_stListOfElements[hElement])
                       ->d_stListOfAlignedBoxes[xNumberOfAlignedBox].xMinPoint;
    *p_xMaxPoint = ((GEO_tdstElementAlignedBoxes *)hObject->d_stListOfElements[hElement])
                       ->d_stListOfAlignedBoxes[xNumberOfAlignedBox].xMaxPoint;
}

GMT_tdxHandleToGameMaterial GEO_vGetGameMaterialOfIndexedAlignedBox(
    ACP_tdxHandleOfObject  hObject,
    ACP_tdxHandleOfElement hElement,
    ACP_tdxIndex           xNumberOfAlignedBox)
{
    return ((GEO_tdstElementAlignedBoxes *)hObject->d_stListOfElements[hElement])
        ->d_stListOfAlignedBoxes[xNumberOfAlignedBox].hMaterial;
}

void GEO_vCreateElementCones(
    ACP_tdxHandleOfObject   hObject,
    ACP_tdxHandleOfElement *p_hElement,
    ACP_tdxIndex            xNbCones)
{
    ACP_tdxIndex xElementToBuild;

    for (xElementToBuild = 0;
         xElementToBuild < hObject->xNbElements;
         xElementToBuild++)
        if (hObject->d_xListOfElementsTypes[xElementToBuild] == GEO_C_xElementNULL)
            break;
    if (xElementToBuild == hObject->xNbElements)
        return;
    hObject->d_xListOfElementsTypes[xElementToBuild] = GEO_C_xElementCones;
    *p_hElement = xElementToBuild;

    hObject->d_stListOfElements[xElementToBuild] = GEO_p_stAllocElementSpheres(xNbCones);
}

/* 0x0041ead0 */
GEO_tdstElementSpheres *GEO_p_stAllocElementSpheres(ACP_tdxIndex xNbSpheres)
{
    GEO_tdstElementSpheres *p_stLocalElement;

    GEO_M_CPAMalloc(p_stLocalElement, GEO_tdstElementSpheres *,
        sizeof(GEO_tdstElementSpheres), E_uwGEONotEnoughtMemory);

    GEO_M_CPAMalloc(p_stLocalElement->d_stListOfSpheres, GEO_tdstIndexedSphere *,
        sizeof(GEO_tdstIndexedSphere) * xNbSpheres, E_uwGEONotEnoughtMemory);

    p_stLocalElement->xNbSpheres = xNbSpheres;
    p_stLocalElement->xIndexOfParallelBox = GEO_C_xNoParallelBox;

    return p_stLocalElement;
}

void GEO_vSetPointsOfIndexedCone(
    ACP_tdxHandleOfObject  hObject,
    ACP_tdxHandleOfElement hElement,
    ACP_tdxIndex           xNumberOfCone,
    ACP_tdxIndex           xTopPoint,
    ACP_tdxIndex           xBasePoint)
{

    ((GEO_tdstElementCones *)hObject->d_stListOfElements[hElement])
        ->d_stListOfCones[xNumberOfCone].xTopPoint = xTopPoint;
    ((GEO_tdstElementCones *)hObject->d_stListOfElements[hElement])
        ->d_stListOfCones[xNumberOfCone].xBasePoint = xBasePoint;
}

void GEO_vGetPointsOfIndexedCone(
    ACP_tdxHandleOfObject  hObject,
    ACP_tdxHandleOfElement hElement,
    ACP_tdxIndex           xNumberOfCone,
    ACP_tdxIndex          *p_xTopPoint,
    ACP_tdxIndex          *p_xBasePoint)
{

    *p_xTopPoint = ((GEO_tdstElementCones *)hObject->d_stListOfElements[hElement])
                       ->d_stListOfCones[xNumberOfCone].xTopPoint;
    *p_xBasePoint = ((GEO_tdstElementCones *)hObject->d_stListOfElements[hElement])
                        ->d_stListOfCones[xNumberOfCone].xBasePoint;
}

void GEO_vSetRadiusOfIndexedCone(
    ACP_tdxHandleOfObject  hObject,
    ACP_tdxHandleOfElement hElement,
    ACP_tdxIndex           xNumberOfCone,
    MTH_tdxReal            xRadius)
{
    GEO_tdstElementCones *p = (GEO_tdstElementCones *)hObject->d_stListOfElements[hElement];
    p->d_stListOfCones[xNumberOfCone].xBaseRadius = xRadius;
}

void GEO_vSetMaterialOfIndexedCone(
    ACP_tdxHandleOfObject   hObject,
    ACP_tdxHandleOfElement  hElement,
    ACP_tdxIndex            xNumberOfCone,
    ACP_tdxHandleOfMaterial hMaterial)
{
    FUN_0042db20(((GEO_tdstElementCones *)hObject->d_stListOfElements[hElement])
        ->d_stListOfCones[xNumberOfCone].hMaterial, hMaterial);
}

void GEO_vSetGameMaterialOfIndexedCone(
    ACP_tdxHandleOfObject       hObject,
    ACP_tdxHandleOfElement      hElement,
    ACP_tdxIndex                xNumberOfCone,
    GMT_tdxHandleToGameMaterial hMaterial)
{
    GEO_tdstElementCones *p = (GEO_tdstElementCones *)hObject->d_stListOfElements[hElement];
    p->d_stListOfCones[xNumberOfCone].hMaterial = hMaterial;
}

GMT_tdxHandleToGameMaterial GEO_vGetGameMaterialOfIndexedCone(
    ACP_tdxHandleOfObject  hObject,
    ACP_tdxHandleOfElement hElement,
    ACP_tdxIndex           xNumberOfCone)
{
    GEO_tdstElementCones *p = (GEO_tdstElementCones *)hObject->d_stListOfElements[hElement];
    return p->d_stListOfCones[xNumberOfCone].hMaterial;
}

ACP_tdxIndex GEO_lGetAlignedBoxNbBoxes(
    ACP_tdxHandleOfObject  hObject,
    ACP_tdxHandleOfElement hElement)
{
    return ((( GEO_tdstElementAlignedBoxes *)hObject->d_stListOfElements[ hElement ])->xNbAlignedBoxes);
}

ACP_tdxIndex GEO_lGetConeNbCones(
    ACP_tdxHandleOfObject  hObject,
    ACP_tdxHandleOfElement hElement)
{
    return ((( GEO_tdstElementCones *)hObject->d_stListOfElements[ hElement ])->xNbCones);
}
