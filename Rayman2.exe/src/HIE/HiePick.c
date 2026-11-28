#include "HIE/HIE_types.h"
#include "MTH/MTH_flt.h"
#include "HIE/HIE_globals.h"
#include "AI/AI_types.h"
#include "INT/Inters.h"
#include "INT/IntersGO.h"
#include "PO/POProto.h"
#include "PO/IPOProto.h"
#include "PCS/PCSStruc.h"
#include "GLI/VisuSet.h"

/* 0x4B9A94 */
MTH_tdxReal HiePick_VoidReal;

#include "HIE/HieSpObj.h"

static __inline ACP_tdxBool	HIE_fn_SO_bHasABoxBoundingVolume( struct HIE_tdstSuperObject *_hSO )
{
	return (ACP_tdxBool)(_hSO->ulFlags & HIE_C_Flag_ulTypeOfBoundingVolume);
}

static __inline ACP_tdxBool	HIE_fn_SO_bHasNoTransformationMatrix( struct HIE_tdstSuperObject *_hSO )
{
	return (ACP_tdxBool) (_hSO->ulFlags & HIE_C_Flag_ulNoTransformationMatrix);
}

#define C_ucTypeZdr 3

#define M_hGetRepositionZoneFromPo(Po) \
    (GLI_vGetVisualSetLOD(PO_fn_hGetVisualSet(Po), 0, &HiePick_VoidReal, &HiePick_GeoHandle), HiePick_GeoHandle)

/* 0x00413700 */
extern unsigned long __cdecl fn_ulStandardGameGetCustomBitsSO(void *hSupObj);
/* 0x0045D390 */
static __inline void HiePick_vCopyTranslationVector(MTH3D_tdstVector *_p_stTrs, MTH3D_tdstVector *_p_stMatrixTrs)
{
    *_p_stTrs = *_p_stMatrixTrs;
}
#define HiePick_vGetTranslationVector(_hMatrix, _p_stTrs) \
    HiePick_vCopyTranslationVector((_p_stTrs), POS_fn_vGetTranslationVectorReadOnly(_hMatrix))

#define GAM_C_CustBitShadowOnMe					0x00000200

/* 0x0042de30 */
extern void *__cdecl PO_fn_hGetCollideSet(void *_hPhysicalObject);

extern void HIE_fn_vPopMatrix(void);

/* 0x0040db90 */
extern void *__cdecl fn_hMakeOneObjectFromManyZdx(unsigned char ucZoneType, void *hCharacter);

/* 0x00497160 */
int PIC_bIntersectSegmentWithGeometricObject(MTH3D_tdstVector *p_stCameraPos, MTH3D_tdstVector *p_stVertex12,
                                             GEO_tdstGeometricObject *p_stObject, GLI_tdstPickedObject *p_stPickedObject);

/* 0x0041D240 */
void GEO_vGetNormalOfGeometricObjectElement(GEO_tdstGeometricObject *hObject, ACP_tdxIndex xElementIndex,
                                            ACP_tdxIndex xDataElementIndex, MTH3D_tdstVector *p_stNormal);

ACP_tdxBool fn_bDetectIntersectSegmentWithBoundingSphere(
    POS_tdstCompletePosition    *_p_stCurrentMatrix,
    MTH3D_tdstVector            *_p_stVertexA,
    MTH3D_tdstVector            *_p_stVectAB,
    GEO_tdxHandleToBoundingSphere _hBoundingSphere)
{

	MTH3D_tdstVector	stGlobalSphereCenter;
	MTH_tdxReal			xScaledRadius;

	MTH3D_M_vMulMatrixVector(&stGlobalSphereCenter, &_p_stCurrentMatrix->stRotationMatrix, GEO_fn_pGetCenterPointOfBoundingSphere(_hBoundingSphere));
	MTH3D_M_vAddVector(&stGlobalSphereCenter, POS_fn_vGetTranslationVectorReadOnly(_p_stCurrentMatrix), &stGlobalSphereCenter);

	xScaledRadius = MTH_M_xMul(_hBoundingSphere->xRadius, POS_fn_xGetMaxScale(_p_stCurrentMatrix));

	return INT_fn_bDetectIntersectSegmentWithSphere (_p_stVertexA, _p_stVectAB, &stGlobalSphereCenter, xScaledRadius);
}

static short fn_xIntersectSegmentWithRepositioningSuperObject( MTH3D_tdstVector *p_stVertexA, MTH3D_tdstVector *p_stVectAB, HIE_tdstSuperObject *_hSprObj, HIE_tdstPickInfo *p_stPickInfo, short *p_xNbPickInfo);

short HIE_bIntersectSegmentWithSuperObject(
    MTH3D_tdstVector    *p_stVertexA,
    MTH3D_tdstVector    *p_stVectAB,
    HIE_tdstSuperObject *hSprObj,
    HIE_tdstPickInfo    *p_stPickInfo)
{
    short xNbPickInfo = 0, xReturnValue;

    HIE_fn_bLoadIdentity();
    xReturnValue = fn_xIntersectSegmentWithRepositioningSuperObject(
        p_stVertexA, p_stVectAB, hSprObj, p_stPickInfo, &xNbPickInfo);
    HIE_fn_bPopMatrix();
    return xReturnValue;
}

static short fn_xIntersectSegmentWithRepositioningSuperObject(
    MTH3D_tdstVector    *p_stVertexA,
    MTH3D_tdstVector    *p_stVectAB,
    HIE_tdstSuperObject *_hSprObj,
    HIE_tdstPickInfo    *p_stPickInfo,
    short               *p_xNbPickInfo)
{
    static GEO_tdxHandleToBoundingSphere hBoundingSphere;
    static GEO_tdxHandleToParallelBox    hParallelBox;
    static void                         *hSentObject;
    static MTH3D_tdstVector              stMinPoint;
    static MTH3D_tdstVector              stMaxPoint;
    static ACP_tdxBool                   bBoxBoundingVolume, bIntersect;
    short                                i;
    static long                          lType;
    HIE_tdstSuperObject                 *hChild;
    static MTH3D_tdstVector              stVertexALocal, stVectAB;
    static POS_tdstCompletePosition      stInvMatrix;
    static GLI_tdstPickedObject          stPickedObject;
    register unsigned long               ulSpoFlags;

    ulSpoFlags = _hSprObj->ulFlags;

    if (ulSpoFlags & HIE_C_Flag_NotPickable)
        return *p_xNbPickInfo;

    lType = HIE_fn_ulGetSuperObjectType(_hSprObj);

    if (lType != HIE_C_ulActor)
    {
        if (ulSpoFlags & AI_g_ulSpoMask)
            return *p_xNbPickInfo;
    }
    else
    {
        if (!fn_ulStandardGameGetCustomBitsSO(_hSprObj) & GAM_C_CustBitShadowOnMe)
        {
            return *p_xNbPickInfo;
        }
    }

    hBoundingSphere = NULL;
    hParallelBox    = NULL;
    hSentObject     = NULL;

    HIE_fn_vPushMatrix(_hSprObj);

    bBoxBoundingVolume = (unsigned char)(ulSpoFlags & HIE_C_Flag_ulTypeOfBoundingVolume);

    if (bBoxBoundingVolume) {
        hParallelBox = (GEO_tdxHandleToParallelBox)HIE_fn_hGetSuperObjectBoundingVolume(_hSprObj);
        if (hParallelBox) {

            MTH3D_tdstVector *p_stMinPoint;
            MTH3D_tdstVector *p_stMaxPoint;
            MTH3D_tdstVector  stTranslation;

            p_stMinPoint = GEO_fn_pGetMinPointOfParallelBox(hParallelBox);
            p_stMaxPoint = GEO_fn_pGetMaxPointOfParallelBox(hParallelBox);
            HiePick_vGetTranslationVector(HIE_fn_hGetSuperObjectGlobalMatrix(_hSprObj), &stTranslation);

            MTH3D_M_vAddVector(&stMinPoint, p_stMinPoint , &stTranslation);
            MTH3D_M_vAddVector(&stMaxPoint, p_stMaxPoint , &stTranslation);
        }
    } else {
        hBoundingSphere = (GEO_tdxHandleToBoundingSphere)HIE_fn_hGetSuperObjectBoundingVolume(_hSprObj);
    }

    if (!hBoundingSphere && !hParallelBox)
        bIntersect = TRUE;
    else if (!bBoxBoundingVolume)
        bIntersect = fn_bDetectIntersectSegmentWithBoundingSphere(
            HIE_fn_hGetSuperObjectGlobalMatrix(_hSprObj), p_stVertexA, p_stVectAB, hBoundingSphere);
    else
        bIntersect = INT_fn_bDetectIntersectSegmentWithBox(p_stVertexA, p_stVectAB, &stMinPoint, &stMaxPoint, NULL);

    if (bIntersect) {

        if (lType & (HIE_C_ulMirror | HIE_C_ulEDT_Geometric)) {
            hSentObject = HIE_fn_hGetSuperObjectObject(_hSprObj);
        } else if (lType & (HIE_C_ulPO | HIE_C_ulPO_Mirror)) {
            void *hCollSet = PO_fn_hGetCollideSet(HIE_fn_hGetSuperObjectObject(_hSprObj));
            hSentObject = hCollSet ? PCS_fn_hGetZdrGeoObjOfPhysicalCollSet(hCollSet) : NULL;
        } else if (lType & (HIE_C_ulIPO | HIE_C_ulIPO_Mirror)) {
            void *hCollSet = PO_fn_hGetCollideSet(
                IPO_fn_hGetPhysicalObject(HIE_fn_hGetSuperObjectObject(_hSprObj)));
            hSentObject = hCollSet ? PCS_fn_hGetZdrGeoObjOfPhysicalCollSet(hCollSet) : NULL;
        } else if (lType == HIE_C_ulActor) {
            hSentObject = fn_hMakeOneObjectFromManyZdx(C_ucTypeZdr, _hSprObj);
        }

        if (hSentObject) {
            if (ulSpoFlags & HIE_C_Flag_ulNoTransformationMatrix) {

                MTH3D_tdstVector *p_stMatTransVec;

                p_stMatTransVec = POS_fn_vGetTranslationVectorReadOnly(HIE_fn_hGetSuperObjectGlobalMatrix(_hSprObj));

                MTH3D_M_vSubVector (&stVertexALocal , p_stVertexA , p_stMatTransVec);

                MTH3D_M_vCopyVector(&stVectAB,p_stVectAB);
            } else {
                POS_fn_vInvertMatrix(&stInvMatrix, HIE_fn_hGetSuperObjectGlobalMatrix(_hSprObj));
                POS_fn_vMulMatrixVertex(&stVertexALocal, &stInvMatrix, p_stVertexA);
                POS_fn_vMulMatrixVector(&stVectAB, &stInvMatrix, p_stVectAB);
            }

            if (PIC_bIntersectSegmentWithGeometricObject(&stVertexALocal, &stVectAB,
                                                          hSentObject, &stPickedObject)) {
                if (*p_xNbPickInfo == 0) {
                    p_stPickInfo[0].stPickedObject = stPickedObject;
                    p_stPickInfo[0].hSprObject     = _hSprObj;
                    p_stPickInfo[0].hGeoObject      = hSentObject;
                    *p_xNbPickInfo = 1;
                    MTH3D_M_vSubVector(p_stVectAB,
                                       &stPickedObject.aDEF_stDataOfElement[0].stHit,
                                       p_stVertexA);
                } else {
                    if (stPickedObject.aDEF_stDataOfElement[0].xDistance <
                        p_stPickInfo[0].stPickedObject.aDEF_stDataOfElement[0].xDistance) {
                        p_stPickInfo[0].stPickedObject = stPickedObject;
                        p_stPickInfo[0].hSprObject     = _hSprObj;
                        p_stPickInfo[0].hGeoObject      = hSentObject;
                        MTH3D_M_vSubVector(p_stVectAB,
                                           &stPickedObject.aDEF_stDataOfElement[0].stHit,
                                           p_stVertexA);
                    }
                }
            }
        }
    }

    if (bIntersect || HIE_fn_SO_bCheckChildren(_hSprObj)) {
        HIE_M_ForEachChildOf(_hSprObj, hChild, i)
            fn_xIntersectSegmentWithRepositioningSuperObject(
                p_stVertexA, p_stVectAB, hChild, p_stPickInfo, p_xNbPickInfo);
    }

    HIE_fn_vPopMatrix();
    return *p_xNbPickInfo;
}

#pragma optimize("y", off)
ACP_tdxBool HIE_bIntersectSegmentWithFirstSuperObject(
    MTH3D_tdstVector     *_p_stVertexA,
    MTH3D_tdstVector     *_p_stVectAB,
    HIE_tdstSuperObject  *hSprObj,
    MTH3D_tdstVector     *_p_stHit,
    MTH3D_tdstVector     *_p_stNormal,
    HIE_tdstSuperObject **_p_hSprObjHit)
{
    HIE_aDEF_stTabOfPickInfo aDEF_stIntersInfo;
    GEO_tdstGeometricObject  *p_stGeomObj;
    GLI_tdstDataOfElement    *p_stDataOfElement;
    MTH3D_tdstVector          stNormal;
    long                      lType;

    if (HIE_bIntersectSegmentWithSuperObject(_p_stVertexA, _p_stVectAB, hSprObj, aDEF_stIntersInfo) > 0) {
        p_stGeomObj       = (GEO_tdstGeometricObject *)aDEF_stIntersInfo[0].hGeoObject;
        p_stDataOfElement = &aDEF_stIntersInfo[0].stPickedObject.aDEF_stDataOfElement[0];

        lType = HIE_fn_ulGetSuperObjectType(aDEF_stIntersInfo[0].hSprObject);

        if (lType & (HIE_C_ulEDT_Geometric | HIE_C_ulPO | HIE_C_ulMirror | HIE_C_ulPO_Mirror |
                     HIE_C_ulActor | HIE_C_ulIPO | HIE_C_ulIPO_Mirror))
            *_p_hSprObjHit = aDEF_stIntersInfo[0].hSprObject;
        else
            *_p_hSprObjHit = NULL;

        MTH3D_M_vCopyVector(_p_stHit, &p_stDataOfElement->stHit);

        if (_p_hSprObjHit &&
            p_stGeomObj->d_xListOfElementsTypes[p_stDataOfElement->xElements] == GEO_C_xElementSpheres) {
            MTH3D_tdstVector        stSphereCenter;
            GEO_tdstElementSpheres *p_stElementSpheres =
                (GEO_tdstElementSpheres *)p_stGeomObj->d_stListOfElements[p_stDataOfElement->xElements];
            POS_fn_vMulMatrixVertex(
                &stSphereCenter,
                HIE_fn_hGetSuperObjectGlobalMatrix(*_p_hSprObjHit),
                &p_stGeomObj->d_stListOfPoints[
                    p_stElementSpheres->d_stListOfSpheres[p_stDataOfElement->xIndexOfFace].xCenterPoint]);
            MTH3D_M_vSubVector(_p_stNormal, _p_stHit, &stSphereCenter);
            MTH3D_M_vNormalizeVector(_p_stNormal, _p_stNormal);
        } else {
            GEO_vGetNormalOfGeometricObjectElement(p_stGeomObj, p_stDataOfElement->xElements,
                                                   p_stDataOfElement->xIndexOfFace, &stNormal);
            POS_fn_vMulMatrixVector(_p_stNormal,
                                    HIE_fn_hGetSuperObjectGlobalMatrix(*_p_hSprObjHit), &stNormal);
            MTH3D_M_vNormalizeVector(_p_stNormal, _p_stNormal);
        }

        return 1;
    } else {
        return 0;
    }
}
#pragma optimize("", on)

extern HIE_tdstSuperObject *__cdecl CAM_fn_hGetActiveCamera(long _lViewportNumber);
extern void *__cdecl fn_h_SectInfoGetCurrentSector(void *h_SectInfo);

#include "SCT/Sector.h"

#pragma optimize("y", off)
ACP_tdxBool HIE_pc_fn_bIntersectMatrixSegmentInCameraSector(POS_tdstCompletePosition *p_stMatA,
                                                             POS_tdstCompletePosition *p_stMatB)
{
    POS_tdstCompletePosition stInv;
    MTH3D_tdstVector stT1, stT2, stAB, stHit, stNormal;
    HIE_tdstSuperObject *hCamera;
    HIE_tdstSuperObject *hSector;
    HIE_tdstSuperObject *hHitSO;
    SECT_tdxHandleOfElementLstGraphicInteraction p_stNode;
    HIE_tdstEngineObject *hEngine;
    SECT_tdstSector *hSectStruct;

    hCamera = CAM_fn_hGetActiveCamera(1);
    POS_fn_vInvertIsoMatrix(&stInv, p_stMatA);
    POS_fn_vGetTranslationVector(&stInv, &stT1);
    POS_fn_vGetTranslationVector(p_stMatB, &stT2);
    stAB.xX = stT2.xX - stT1.xX;
    stAB.xY = stT2.xY - stT1.xY;
    stAB.xZ = stT2.xZ - stT1.xZ;

    hEngine = (HIE_tdstEngineObject *)hCamera->hLinkedObject.p_Void;
    hSector = (HIE_tdstSuperObject *)fn_h_SectInfoGetCurrentSector(hEngine->hSectInfo);

    if (HIE_bIntersectSegmentWithFirstSuperObject(&stT1, &stAB, hSector,
                                                  &stHit, &stNormal, &hHitSO))
        return 1;

    hSectStruct = (SECT_tdstSector *)hSector->hLinkedObject.p_Void;
    p_stNode = LST2_M_StaticGetFirstElement(&hSectStruct->stListOfSectorsInGraphicInteraction);
    while (p_stNode != NULL) {
        if (HIE_bIntersectSegmentWithFirstSuperObject(&stT1, &stAB, p_stNode->hPointerOfSectorSO,
                                                      &stHit, &stNormal, &hHitSO))
            return 1;
        p_stNode = LST2_M_StaticGetNextElement(p_stNode);
    }
    return 0;
}

#pragma optimize("y", off)
ACP_tdxBool HIE_bDetectIntersectSegmentWithSuperObject(
    MTH3D_tdstVector    *p_stVertexA,
    MTH3D_tdstVector    *p_stVectAB,
    HIE_tdstSuperObject *_hSprObj)
{
    static GEO_tdxHandleToBoundingSphere hBoundingSphere;
    static GEO_tdxHandleToParallelBox    hParallelBox;
    static MTH3D_tdstVector              stMinPoint;
    static MTH3D_tdstVector              stMaxPoint;
    static MTH3D_tdstVector             *p_stTranslation;
    static ACP_tdxBool                   bBoxBoundingVolume, bIntersect;
    static void                         *hSentObject;
    HIE_tdstSuperObject                 *hChild;
    static long                          lType;
    static MTH3D_tdstVector              stVertexALocal, stVectAB;
    static POS_tdstCompletePosition      stInvMatrix;

    hBoundingSphere = NULL;
    hParallelBox    = NULL;
    hSentObject     = NULL;

    p_stTranslation = POS_fn_vGetTranslationVectorReadOnly(HIE_fn_hGetSuperObjectGlobalMatrix(_hSprObj));

    bBoxBoundingVolume = HIE_fn_SO_bHasABoxBoundingVolume(_hSprObj);

    if (bBoxBoundingVolume) {
        hParallelBox = (GEO_tdxHandleToParallelBox)HIE_fn_hGetSuperObjectBoundingVolume(_hSprObj);
        if (hParallelBox) {
            MTH3D_tdstVector *p_stMinPoint;
            MTH3D_tdstVector *p_stMaxPoint;

            p_stMinPoint = GEO_fn_pGetMinPointOfParallelBox(hParallelBox);
            p_stMaxPoint = GEO_fn_pGetMaxPointOfParallelBox(hParallelBox);

            MTH3D_M_vAddVector(&stMinPoint, p_stMinPoint , p_stTranslation);
            MTH3D_M_vAddVector(&stMaxPoint, p_stMaxPoint , p_stTranslation);
        }
    } else {
        hBoundingSphere = (GEO_tdxHandleToBoundingSphere)HIE_fn_hGetSuperObjectBoundingVolume(_hSprObj);
    }

    if (!hBoundingSphere && !hParallelBox)
        bIntersect = TRUE;
    else if (!bBoxBoundingVolume)
        bIntersect = fn_bDetectIntersectSegmentWithBoundingSphere(
            HIE_fn_hGetSuperObjectGlobalMatrix(_hSprObj), p_stVertexA, p_stVectAB, hBoundingSphere);
    else
        bIntersect = INT_fn_bDetectIntersectSegmentWithBox(p_stVertexA, p_stVectAB, &stMinPoint, &stMaxPoint, NULL);

    if (bIntersect) {
        lType = HIE_fn_ulGetSuperObjectType(_hSprObj);

        if (lType & (HIE_C_ulMirror | HIE_C_ulEDT_Geometric)) {
            hSentObject = HIE_fn_hGetSuperObjectObject(_hSprObj);
        } else if (lType & (HIE_C_ulPO | HIE_C_ulPO_Mirror)) {
            hSentObject = M_hGetRepositionZoneFromPo(
                (void *)HIE_fn_hGetSuperObjectObject(_hSprObj));
        } else if (lType & (HIE_C_ulIPO | HIE_C_ulIPO_Mirror)) {
            hSentObject = M_hGetRepositionZoneFromPo(
                IPO_fn_hGetPhysicalObject(HIE_fn_hGetSuperObjectObject(_hSprObj)));
        } else if (lType == HIE_C_ulActor) {
            hSentObject = fn_hMakeOneObjectFromManyZdx(C_ucTypeZdr, _hSprObj);
        }

        if (hSentObject) {
            if (HIE_fn_SO_bHasNoTransformationMatrix(_hSprObj)) {
                MTH3D_M_vSubVector(&stVertexALocal, p_stVertexA, p_stTranslation);
                MTH3D_M_vCopyVector(&stVectAB, p_stVectAB);
            } else {
                POS_fn_vInvertMatrix(&stInvMatrix, HIE_fn_hGetSuperObjectGlobalMatrix(_hSprObj));

                POS_fn_vMulMatrixVertex(&stVertexALocal, &stInvMatrix, p_stVertexA);

                POS_fn_vMulMatrixVector(&stVectAB, &stInvMatrix, p_stVectAB);
            }

            if (INT_fn_bDetectIntersectSegmentWithFaceOfGeometricObject(
                    &stVertexALocal, &stVectAB, hSentObject))
                return TRUE;
        }
    }

    if (bIntersect || HIE_fn_SO_bCheckChildren(_hSprObj)) {
        for (hChild = _hSprObj->p_stFirstChild; hChild; hChild = hChild->p_stNextBrother) {
            if (HIE_fn_ulGetSuperObjectType(hChild) != HIE_C_ulActor &&
                HIE_bDetectIntersectSegmentWithSuperObject(p_stVertexA, p_stVectAB, hChild))
                return 1;
        }
    }

    return 0;
}
#pragma optimize("", on)
