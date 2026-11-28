#include "SCT/Sector.h"
#include "POS/POS_types.h"
#include "MTH/MTH3D.h"
#include "GAM/GAM_types.h"
#include "GEO/GeoMem.h"

#define C_SectorMinPriority 0

#define GLI_C_lHasNoMirror							0x10000000

#define SCT_pc_FLT_AT(p, off)  (*(float *)((unsigned char *)(p) + (off)))
#define SCT_pc_ABS_F(x)        MTH_M_xAbs(x)

extern unsigned short GAM_g_uw5115d0_storage;
extern void          *g_a_hTextGeoObject[10];
extern unsigned long  AI_g_ulSpoMask;
extern unsigned char  GLI_pc_g_cSinusEffectState;
extern void __cdecl SHW_fn_vSentToViewPort(void *p_stVpt, void *p_stSuperObject,
                                                           long lUnused, unsigned long ulDrawModeMask,
                                                           void *hObject);
extern unsigned long __cdecl fn_ulStandardGameGetCustomBitsSO(void *hSupObj);
extern float __cdecl GEO_xGetZFarTransparencyLevel(void);
extern void __cdecl GEO_vSetPointOfObject(void *hObject, MTH3D_tdstVector *p_stPoint,
                                          short xIndexOfPoint);
extern void __cdecl GEO_vSetUVOfIndexedTriangles(void *hObject, short hElement, short xUVIdx,
                                                 void *p_stUV);
extern void __cdecl GEO_vSetFaceOfIndexedTriangles(void *hObject, short hElement, short xFace,
                                                   short x1, short x2, short x3);
extern void __cdecl GEO_vSetIndexedUVOfFaceOfIndexedTriangles(void *hObject, short hElement,
                                                              short xFace, short xUV1, short xUV2,
                                                              short xUV3);
extern unsigned char __cdecl INT_fn_bDetectIntersectSegmentWithBox(MTH3D_tdstVector *p_stVertex1,
                                                         MTH3D_tdstVector *p_stVect12,
                                                         MTH3D_tdstVector *p_stMinPoint,
                                                         MTH3D_tdstVector *p_stMaxPoint,
                                                         void *p_xNull);

#include "GAM/PlayAnim.h"
#include "SCT/SectExt.h"

/* 0x005040D4 */
float SCT_pc_g_xMirrorHeightOffset;

/* 0x0044B410 */
void __cdecl SCT_pc_fn_vComputeMirrorMatrix(POS_tdstCompletePosition *_p_stSourceMatrix,
                                            POS_tdstCompletePosition *_p_stMirrorMatrix,
                                            MTH3D_tdstVector         *_p_stReflectedPoint,
                                            MTH3D_tdstVector         *_p_stPlaneNormal,
                                            MTH3D_tdstVector         *_p_stPlanePoint)
{
    const float C_xOne      =  1.0f;   /* 0x0049C8E0 */
    const float C_xMinusTwo = -2.0f;   /* 0x0049C8D8 */

    POS_tdstCompletePosition stInvSource;
    POS_tdstCompletePosition stReflection;
    POS_tdstCompletePosition stTmp;
    MTH3D_tdstVector         stI, stJ, stK;
    MTH3D_tdstVector         stTrans;
    MTH3D_tdstVector         stReflected;
    MTH3D_tdstVector         stD;
    MTH3D_tdstVector         stNt;
    float nx = _p_stPlaneNormal->xX;
    float ny = _p_stPlaneNormal->xY;
    float nz = _p_stPlaneNormal->xZ;
    float t;

    POS_fn_vSetIdentityMatrix(&stInvSource);
    POS_fn_vInvertMatrix(&stInvSource, _p_stSourceMatrix);
    POS_fn_vSetIdentityMatrix(&stReflection);

    stI.xX = C_xOne - (nx * nx + nx * nx);
    stI.xY = ny * nx * C_xMinusTwo;
    stI.xZ = nz * nx * C_xMinusTwo;
    stJ.xX = stI.xY;
    stJ.xY = C_xOne - (ny * ny + ny * ny);
    stJ.xZ = ny * nz * C_xMinusTwo;
    stK.xX = stI.xZ;
    stK.xY = stJ.xZ;
    stK.xZ = C_xOne - (nz * nz + nz * nz);
    POS_fn_vSetRotationMatrix(&stReflection, &stI, &stJ, &stK);

    POS_fn_vGetTranslationVector(&stInvSource, &stTrans);

    MTH3D_M_vSubVector(&stD, &stTrans, _p_stPlanePoint);
    t = (ny * stD.xY + nx * stD.xX + nz * stD.xZ) * C_xMinusTwo;
    stNt.xX = nx * t;
    stNt.xY = ny * t;
    stNt.xZ = nz * t;
    MTH3D_M_vAddVector(&stReflected, &stTrans, &stNt);

    stTmp = stInvSource;
    POS_fn_vMulMatrixMatrix(&stInvSource, &stReflection, &stTmp);
    POS_fn_vSetTranslationVector(&stInvSource, &stReflected);
    *_p_stReflectedPoint = stReflected;

    POS_fn_vSetIdentityMatrix(_p_stMirrorMatrix);
    POS_fn_vInvertMatrix(_p_stMirrorMatrix, &stInvSource);
}
void __cdecl SCT_pc_fn_vCollectMirrorsOfSectors(void *_hVpt, HIE_tdstSuperObject *_hSprObjSector,
                                                HIE_tdstSuperObject **_p_hHandleOfMirror,
                                                void **_p_hHandleOfGeometricMirror,
                                                HIE_tdstSuperObject **_p_hSectorList,
                                                short *_p_shNbOfSectors)
{
    SECT_tdxHandleOfSectorObject                 hSectorObject;
    SECT_tdxHandleOfElementLstCharacter          hCharacter;
    HIE_tdstSuperObject                         *hChild;
    SECT_tdxHandleOfElementLstGraphicInteraction hGraphicInteractionChild;
    HIE_tdstSuperObject                         *hNeighbour;
    short                shNbNeighbours;
    short                shLoop;
    short                shIdx;
    short                shCountBefore;

    *_p_shNbOfSectors = 0;
    hSectorObject = (SECT_tdxHandleOfSectorObject)HIE_fn_hGetSuperObjectObject(_hSprObjSector);
    if ((_hSprObjSector->lDrawModeMask & GLI_C_lHasNoMirror) == 0) {
        HIE_fn_hFindAllMirrorsInHierarchy(_hVpt, _hSprObjSector, _p_hHandleOfMirror,
                                          _p_hHandleOfGeometricMirror, _p_shNbOfSectors, 6);
    }
    for (hCharacter = LST2_M_DynamicGetFirstElement(&hSectorObject->stListOfCharacters);
         hCharacter != NULL;
         hCharacter = LST2_M_DynamicGetNextElement(hCharacter)) {
        hChild = hCharacter->hPointerOfCharacter;
        if ((hChild->lDrawModeMask & GLI_C_lHasNoMirror) == 0) {
            HIE_fn_hFindAllMirrorsInHierarchy(_hVpt, hChild, _p_hHandleOfMirror,
                                              _p_hHandleOfGeometricMirror, _p_shNbOfSectors, 6);
        }
    }

    shIdx = 0;
    if (0 < *_p_shNbOfSectors) {
        do {
            _p_hSectorList[shIdx] = _hSprObjSector;
            shIdx = (short)(shIdx + 1);
        } while (shIdx < *_p_shNbOfSectors);
    }

    hGraphicInteractionChild = LST2_M_StaticGetFirstElement(&hSectorObject->stListOfSectorsInGraphicInteraction);
    shNbNeighbours = (short)LST2_M_StaticGetNumberOfElements(&hSectorObject->stListOfSectorsInGraphicInteraction);
    if (0 < shNbNeighbours) {
        shLoop = shNbNeighbours;
        do {
            shCountBefore = *_p_shNbOfSectors;
            shIdx         = shCountBefore;
            hNeighbour    = hGraphicInteractionChild->hPointerOfSectorSO;
            if ((hNeighbour->lDrawModeMask & GLI_C_lHasNoMirror) == 0) {
                HIE_fn_hFindAllMirrorsInHierarchy(_hVpt, hNeighbour, _p_hHandleOfMirror,
                                                  _p_hHandleOfGeometricMirror, _p_shNbOfSectors, 6);
            }
            for (hCharacter = LST2_M_DynamicGetFirstElement(&((SECT_tdxHandleOfSectorObject)HIE_fn_hGetSuperObjectObject(hNeighbour))->stListOfCharacters);
                 hCharacter != NULL;
                 hCharacter = LST2_M_DynamicGetNextElement(hCharacter)) {
                hChild = hCharacter->hPointerOfCharacter;
                if ((hChild->lDrawModeMask & GLI_C_lHasNoMirror) == 0) {
                    HIE_fn_hFindAllMirrorsInHierarchy(_hVpt, hChild, _p_hHandleOfMirror,
                                                      _p_hHandleOfGeometricMirror,
                                                      _p_shNbOfSectors, 6);
                }
            }
            if (shCountBefore < *_p_shNbOfSectors) {
                do {
                    _p_hSectorList[shIdx] = hNeighbour;
                    shIdx = (short)(shIdx + 1);
                } while (shIdx < *_p_shNbOfSectors);
            }
            hGraphicInteractionChild = LST2_M_StaticGetNextElement(hGraphicInteractionChild);
            shLoop   = (short)(shLoop - 1);
        } while (shLoop != 0);
    }
}

/* 0x0044BC10 */
void __cdecl SECT_fn_vGetMinPointInBorder(void *_hSector, MTH3D_tdstVector *_p_stMinPoint)
{
    MTH3D_M_vCopyVector(_p_stMinPoint, &((SECT_tdxHandleOfSectorObject)_hSector)->stMinMaxPoints[0]);
}

/* 0x0044BC30 */
void __cdecl SECT_fn_vGetMaxPointInBorder(void *_hSector, MTH3D_tdstVector *_p_stMaxPoint)
{
    MTH3D_M_vCopyVector(_p_stMaxPoint, &((SECT_tdxHandleOfSectorObject)_hSector)->stMinMaxPoints[1]);
}

/* 0x0044BC50 */
void __cdecl SECT_fn_vGetCenterOfBorder(void *_hSector, MTH3D_tdstVector *_p_stCenter)
{
    MTH3D_M_vAddVector(_p_stCenter, &((SECT_tdxHandleOfSectorObject)_hSector)->stMinMaxPoints[0],
                                    &((SECT_tdxHandleOfSectorObject)_hSector)->stMinMaxPoints[1]);
    MTH3D_M_vDivScalarVector(_p_stCenter, _p_stCenter, MTH_C_2);
}

/* 0x0044BCC0 */
SECT_tdxHandleOfElementLstDNMLights __cdecl SECT_fn_hCreateElementLstDNMLights(void)
{
    SECT_tdxHandleOfElementLstDNMLights hDNMLightsElement;

    GEO_M_CPAMalloc(hDNMLightsElement, SECT_tdxHandleOfElementLstDNMLights,
                    sizeof(struct SECT_tdstListOfDNMLights_), E_uwGEONotEnoughtMemory);

    LST2_M_DynamicInitElement(hDNMLightsElement);
    return hDNMLightsElement;
}

/* 0x0044BDC0 */
int __cdecl SECT_fn_bIsInBorderBox(MTH3D_tdstVector *_p_stPosition, void *_hSector)
{
    MTH3D_tdstVector stMin, stMax;

    SECT_fn_vGetMinPointInBorder(_hSector, &stMin);
    SECT_fn_vGetMaxPointInBorder(_hSector, &stMax);

    if ((_p_stPosition->xX < stMin.xX) || (_p_stPosition->xX > stMax.xX))
        return FALSE;

    if ((_p_stPosition->xY < stMin.xY) || (_p_stPosition->xY > stMax.xY))
        return FALSE;

    if ((_p_stPosition->xZ < stMin.xZ) || (_p_stPosition->xZ > stMax.xZ))
        return FALSE;

    return TRUE;
}

/* 0x0044BE50 */
HIE_tdstSuperObject *__cdecl SECT_fn_hResearchInWhatSectorIAm(HIE_tdstSuperObject *SECT_hFatherSector,
                                                              MTH3D_tdstVector *p_stAbsolutePosition)
{
    SECT_tdxHandleOfSectorObject hSectorObject;
    HIE_tdstSuperObject *hSectorWhereIAm;
    HIE_tdstSuperObject *hVirtualSectorWhereIAm;
    HIE_tdstSuperObject *hCurrentSector;
    HIE_tdstSuperObject *hLastSector = NULL;
    MTH3D_tdstVector stDistance;
    ACP_tdxIndex xInd;
    MTH_tdxReal xNearDistance;
    MTH_tdxReal xCurrentDistance;
    MTH_tdxReal xVirtualDistance;
    char cCurrentPriority;
    char cVirtualPriority;

    cCurrentPriority = C_SectorMinPriority;
    cVirtualPriority = C_SectorMinPriority;
    xCurrentDistance = MTH_C_InfinitPlus;
    xVirtualDistance = MTH_C_InfinitPlus;
    xNearDistance = MTH_C_InfinitPlus;
    hSectorWhereIAm = NULL;
    hVirtualSectorWhereIAm = NULL;

    HIE_M_ForEachChildOf(SECT_hFatherSector,hCurrentSector,xInd)
    {

        hSectorObject = (SECT_tdxHandleOfSectorObject) HIE_fn_hGetSuperObjectObject(hCurrentSector);

        hLastSector = hCurrentSector;

        if (SECT_fn_bIsInBorderBox(p_stAbsolutePosition, hSectorObject))
        {
            SECT_fn_vGetCenterOfBorder(hSectorObject, &stDistance);
            MTH3D_M_vSubVector(&stDistance, &stDistance, p_stAbsolutePosition);
            xNearDistance = MTH3D_M_xNormVector(&stDistance);
            if (!hSectorObject->xVirtual)
            {
                if (hSectorObject->cPriority > cCurrentPriority)
                {
                    hSectorWhereIAm = hCurrentSector;
                    xCurrentDistance = xNearDistance;
                    cCurrentPriority = hSectorObject->cPriority;
                }
                else if ((hSectorObject->cPriority == cCurrentPriority) && (xNearDistance < xCurrentDistance))
                {
                    hSectorWhereIAm = hCurrentSector;
                    xCurrentDistance = xNearDistance;
                }
            }
            else
            {
                if (hSectorObject->cPriority > cVirtualPriority)
                {
                    hVirtualSectorWhereIAm = hCurrentSector;
                    xVirtualDistance = xNearDistance;
                    cVirtualPriority = hSectorObject->cPriority;
                }
                else if ((hSectorObject->cPriority == cVirtualPriority) && (xNearDistance < xVirtualDistance))
                {
                    hVirtualSectorWhereIAm = hCurrentSector;
                    xVirtualDistance = xNearDistance;
                }
            }
        }
    }

    if (hSectorWhereIAm == NULL)
        hSectorWhereIAm = hVirtualSectorWhereIAm;

    if (hSectorWhereIAm == NULL)
        hSectorWhereIAm = hLastSector;

    return hSectorWhereIAm;
}

void SCT_fn_vTestChangeSectorWithHistory (MTH3D_tdstVector			 *_p_stBeforeAbsolutePosition,
                                          MTH3D_tdstVector			 *_p_stAfterAbsolutePosition,
                                          HIE_tdstSuperObject		**_p_hSectorSOWhereIWas)
{
	SECT_tdxHandleOfElementLstCollisionInteraction	hCollisionNode;
	SECT_tdxHandleOfSectorObject					hSectorObject;
	SECT_tdxHandleOfSectorObject					hCurrentSectorObject;
	HIE_tdstSuperObject								*hCurrentSector, *hCollideSector;
	long											iInd;

    hCurrentSector = *_p_hSectorSOWhereIWas;
	hCurrentSectorObject = (SECT_tdxHandleOfSectorObject) HIE_fn_hGetSuperObjectObject(hCurrentSector);

	if (SECT_fn_bIsInBorderBox(_p_stAfterAbsolutePosition, hCurrentSectorObject) && !hCurrentSectorObject->xVirtual)
	{

		SECT_M_ForEachCollisionNodeInCollisionInteractionList(hCurrentSector, hCollisionNode, iInd)
		{

			hCollideSector = SECT_GetSectorInCollisionList(hCollisionNode);
			hSectorObject = (SECT_tdxHandleOfSectorObject) HIE_fn_hGetSuperObjectObject(hCollideSector);

			if (hSectorObject->cPriority <= hCurrentSectorObject->cPriority)
				return;

			else if( !hSectorObject->xVirtual && SECT_fn_bIsInBorderBox(_p_stAfterAbsolutePosition,hSectorObject) )

			{
				*_p_hSectorSOWhereIWas = hCollideSector;
				return;
			}
		}

		return;
	}

	SECT_M_ForEachCollisionNodeInCollisionInteractionList(hCurrentSector, hCollisionNode, iInd)
	{

		hCollideSector = SECT_GetSectorInCollisionList(hCollisionNode);
		hSectorObject = (SECT_tdxHandleOfSectorObject) HIE_fn_hGetSuperObjectObject(hCollideSector);

		if( !hSectorObject->xVirtual && SECT_fn_bIsInBorderBox(_p_stAfterAbsolutePosition,hSectorObject) )

		{
			*_p_hSectorSOWhereIWas = hCollideSector;
			return;
		}
	}

	*_p_hSectorSOWhereIWas = SECT_fn_hResearchInWhatSectorIAm(HIE_fn_hGetSuperObjectFather(hCurrentSector), _p_stAfterAbsolutePosition);

}

void __cdecl SECT_fn_vIsolateCharListNode(void *_hNode)
{

    LST2_M_DynamicIsolate((SECT_tdxHandleOfElementLstCharacter)_hNode);
}

/* 0x0044c0d0 */
void __cdecl SECT_fn_vAddTailCharListNode(HIE_tdstSuperObject *hSector,
                                          SECT_tdxHandleOfElementLstCharacter hNode)
{
    LST2_M_DynamicAddTail(&((SECT_tdstSector *)HIE_fn_hGetSuperObjectObject(hSector))->stListOfCharacters, hNode);
}

/* 0x0044c170 */
void __cdecl SECT_fn_vAddHeadCharListNode(HIE_tdstSuperObject *hSector,
                                          SECT_tdxHandleOfElementLstCharacter hNode)
{
    LST2_M_DynamicAddHead(&((SECT_tdstSector *)HIE_fn_hGetSuperObjectObject(hSector))->stListOfCharacters, hNode);
}

/* 0x0044c210 */
void __cdecl SECT_fn_vAddTailDNMLightsListNode(HIE_tdstSuperObject *hSector,
                                               SECT_tdxHandleOfElementLstDNMLights hNode)
{
    LST2_M_DynamicAddTail(&((SECT_tdstSector *)HIE_fn_hGetSuperObjectObject(hSector))->stListOfDNMLights, hNode);
}

/* 0x0044C2C0 */
char __cdecl SECT_fn_bIsThisSectorVirtual(void *_hSprObjSector)
{
    return ((SECT_tdxHandleOfSectorObject)HIE_fn_hGetSuperObjectObject((HIE_tdstSuperObject *)_hSprObjSector))->xVirtual;
}

/* 0x0044C2D0 */
char __cdecl fn_cGetSectorCameraType(void *_hSprObjSector)
{
    return ((SECT_tdxHandleOfSectorObject)HIE_fn_hGetSuperObjectObject((HIE_tdstSuperObject *)_hSprObjSector))->cCameraType;
}

/* 0x0044C2E0 */
float __cdecl fn_xGetSectorZFar(void *_hSprObjSector)
{
    return ((SECT_tdxHandleOfSectorObject)HIE_fn_hGetSuperObjectObject((HIE_tdstSuperObject *)_hSprObjSector))->xZFar;
}

/* 0x0044C2F0 */
void *__cdecl SECT_fn_p_stGetMinPointInBorder(void *_hSector)
{
    return &((SECT_tdxHandleOfSectorObject)_hSector)->stMinMaxPoints[0];
}

/* 0x0044C300 */
void *__cdecl SECT_fn_p_stGetMaxPointInBorder(void *_hSector)
{
    return &((SECT_tdxHandleOfSectorObject)_hSector)->stMinMaxPoints[1];
}

/* 0x0044C540 */
MTH_tdxReal SHW_fn_vComputeZofShadowPoint (MTH3D_tdstVector	*_p_stPickedPoint,
										   MTH3D_tdstVector	*_p_stGlobalNormalOfPickedFace,
										   MTH_tdxReal		_xInvGlobalNormalxZ,
										   MTH_tdxReal		_xOffsetX,
										   MTH_tdxReal		_xOffsetY)
{
	return (MTH_M_xSub (_p_stPickedPoint->xZ,
		MTH_M_xMul(_xInvGlobalNormalxZ,
			   MTH_M_xAdd(
				  MTH_M_xMul(_p_stGlobalNormalOfPickedFace->xX, MTH_M_xSub(_xOffsetX, _p_stPickedPoint->xX)),
				  MTH_M_xMul(_p_stGlobalNormalOfPickedFace->xY, MTH_M_xSub(_xOffsetY, _p_stPickedPoint->xY))
											 )
								  )
						)
		   );
}

void __cdecl SHW_fn_vDrawShadow(void *p_stVpt, void *p_stSuperObject,
                                                void *p_stSectorSuperObject, unsigned long ulDrawMask)
{
    unsigned char      *p_ucSO;
    unsigned char      *p_ucSectorSO;
    unsigned char      *p_ucSectorObj;
    unsigned char      *p_ucElem;
    unsigned char      *p_ucFather;
    unsigned char      *p_ucCand;
    unsigned char      *p_ucEngine;
    unsigned char      *p_ucObj;
    unsigned char      *p_ucPair;
    unsigned char      *p_ucLink;
    void               *hHitSO;
    POS_tdstCompletePosition *p_stMatrix;
    unsigned long       ulCustomBits;
    float               xSizeA;
    float               xSizeB;
    float               xSizeB2;
    float               xInvZ;
    float               xLen;
    float               xInvLen;
    float               xScale;
    float               xAxisX;
    float               xAxisY;
    float               xDepthX;
    float               xDepthY;
    float               xUV[2];
    int                 bHit;
    int                 iCount;
    long                i;
    long                j;
    SECT_tdxHandleOfElementLstCharacter               hCharacterList;
    SECT_tdxHandleOfElementLstCollisionInteraction    hCollisionElement;
    SECT_tdxHandleOfElementLstActivityInteraction     hInteractionElement;
    MTH3D_tdstVector    stOrigin;
    MTH3D_tdstVector    stEnd;
    MTH3D_tdstVector    stBoxMin;
    MTH3D_tdstVector    stBoxMax;
    MTH3D_tdstVector    stHit;
    MTH3D_tdstVector    stNormal;
    MTH3D_tdstVector    stVertex;
    MTH3D_tdstVector    stDouble;

    p_ucSO       = (unsigned char *)p_stSuperObject;
    p_ucSectorSO = (unsigned char *)p_stSectorSuperObject;

    if (M_GetMSHandle((HIE_tdstSuperObject *)p_ucSO, 3dData)->p_stShadowMaterial != 0
        && ((((HIE_tdstSuperObject *)p_ucSO)->ulFlags) & 0x20UL) == 0
        && ((((struct tdstEngineObject_ *)((HIE_tdstSuperObject *)p_ucSO)->hLinkedObject.p_Void)->h_StandardGame)->ucMiscFlags & 4) != 0
        && ((((ulCustomBits = fn_ulStandardGameGetCustomBitsSO(p_stSuperObject)) & 0x200000UL) == 0)
            || GEO_xGetZFarTransparencyLevel() != 0.0f)
        && M_GetMSHandle((HIE_tdstSuperObject *)p_ucSO, 3dData)->xShadowQuality != 0
        && (short)GAM_g_uw5115d0_storage < 10)
    {
        p_ucEngine   = (unsigned char *)((HIE_tdstSuperObject *)p_ucSO)->hLinkedObject.p_Void;
        p_stMatrix   = (POS_tdstCompletePosition *)((HIE_tdstSuperObject *)p_ucSO)->hGlobalMatrix;
        bHit         = 0;
        hHitSO       = 0;
        POS_fn_vGetTranslationVector(p_stMatrix, &stOrigin);
        p_ucElem = (unsigned char *)g_a_hTextGeoObject[(short)GAM_g_uw5115d0_storage];

        AI_g_ulSpoMask = 0;
        stOrigin.xZ = stOrigin.xZ + 1.8f;
        p_ucFather = (unsigned char *)((HIE_tdstSuperObject *)p_ucSO)->p_stFather;
        xSizeA = SCT_pc_ABS_F(((MS_tdxHandleTo3dData)((struct tdstEngineObject_ *)p_ucEngine)->h_3dData)->xShadowScaleX);
        xSizeB = SCT_pc_ABS_F(((MS_tdxHandleTo3dData)((struct tdstEngineObject_ *)p_ucEngine)->h_3dData)->xShadowScaleY);
        stEnd.xX = 0.0f;
        stEnd.xY = 0.0f;
        stEnd.xZ = -14.5f;

        p_ucSectorObj = (unsigned char *)((HIE_tdstSuperObject *)p_ucSectorSO)->hLinkedObject.p_Void;

        if ((((HIE_tdstSuperObject *)p_ucFather)->ulTypeOfLinkedObject & HIE_C_ulActor) == 0) {

            SECT_M_ForEachCharListInSector((HIE_tdstSuperObject *)p_ucSectorSO, hCharacterList, i)
            {
                p_ucCand = (unsigned char *)SECT_GetCharacterInList(hCharacterList);
                if (p_ucCand != p_ucSO
                    && (fn_ulStandardGameGetCustomBitsSO(p_ucCand) & 0x200UL) != 0
                    && fn_bf1StandardGameGetIsActive(M_GetMSHandle((HIE_tdstSuperObject *)p_ucCand, StandardGame))
                    && HIE_bIntersectSegmentWithFirstSuperObject(&stOrigin, &stEnd,
                           (HIE_tdstSuperObject *)p_ucCand, &stHit, &stNormal,
                           (HIE_tdstSuperObject **)&hHitSO) != 0)
                {
                    bHit = bHit || (stNormal.xZ > 0.5f);
                }
            }

            AI_g_ulSpoMask = HIE_C_Flag_ulNoShadowOnMe;
            if (HIE_bIntersectSegmentWithFirstSuperObject(&stOrigin, &stEnd,
                    (HIE_tdstSuperObject *)p_ucSectorSO, &stHit, &stNormal,
                    (HIE_tdstSuperObject **)&hHitSO) != 0)
            {
                bHit = bHit || (stNormal.xZ > 0.5f);
            }
            AI_g_ulSpoMask = 0;

            SECT_M_ForEachActivityNodeInActivityInteractionList((HIE_tdstSuperObject *)p_ucSectorSO, hInteractionElement, i)
            {
                HIE_tdstSuperObject         *hSector;
                SECT_tdxHandleOfSectorObject hSectorObject;

                hSector = SECT_GetSectorInActivityList(hInteractionElement);
                hSectorObject = (SECT_tdxHandleOfSectorObject)HIE_fn_hGetSuperObjectObject(hSector);
                SECT_fn_vGetMinPointInBorder(hSectorObject, &stBoxMin);
                SECT_fn_vGetMaxPointInBorder(hSectorObject, &stBoxMax);
                if (INT_fn_bDetectIntersectSegmentWithBox(&stOrigin, &stEnd, &stBoxMin,
                                                          &stBoxMax, 0) != 0)
                {
                    SECT_M_ForEachCharListInSector(hSector, hCharacterList, j)
                    {
                        p_ucCand = (unsigned char *)SECT_GetCharacterInList(hCharacterList);
                        if ((fn_ulStandardGameGetCustomBitsSO(p_ucCand) & 0x200UL) != 0
                            && fn_bf1StandardGameGetIsActive(M_GetMSHandle((HIE_tdstSuperObject *)p_ucCand, StandardGame))
                            && HIE_bIntersectSegmentWithFirstSuperObject(&stOrigin, &stEnd,
                                   (HIE_tdstSuperObject *)p_ucCand, &stHit, &stNormal,
                                   (HIE_tdstSuperObject **)&hHitSO) != 0)
                        {
                            bHit = bHit || (stNormal.xZ > 0.5f);
                        }
                    }
                }
            }

            AI_g_ulSpoMask = HIE_C_Flag_ulNoShadowOnMe;
            SECT_M_ForEachCollisionNodeInCollisionInteractionList((HIE_tdstSuperObject *)p_ucSectorSO, hCollisionElement, i)
            {
                if (HIE_bIntersectSegmentWithFirstSuperObject(&stOrigin, &stEnd,
                        SECT_GetSectorInCollisionList(hCollisionElement), &stHit, &stNormal,
                        (HIE_tdstSuperObject **)&hHitSO) != 0)
                {
                    bHit = bHit || (stNormal.xZ > 0.5f);
                }
            }
        }
        else {

            if ((fn_ulStandardGameGetCustomBitsSO(p_ucFather) & 0x200UL) != 0
                && ((((struct tdstEngineObject_ *)((HIE_tdstSuperObject *)p_ucFather)->hLinkedObject.p_Void)->h_StandardGame)->ucMiscFlags & 4) != 0
                && HIE_bIntersectSegmentWithFirstSuperObject(&stOrigin, &stEnd,
                       (HIE_tdstSuperObject *)p_ucFather, &stHit, &stNormal,
                       (HIE_tdstSuperObject **)&hHitSO) != 0)
            {
                if (stNormal.xZ <= 0.5f) {
                    bHit = 0;
                }
                else {
                    bHit = 1;
                }
            }
        }

        if (bHit && 0.5f < stNormal.xZ) {
            xInvZ = 1.0f / stNormal.xZ;
            POS_fn_vGetRotationMatrix(p_stMatrix, &stBoxMin, &stVertex, &stVertex);
            if (SCT_pc_ABS_F(stBoxMin.xX) < 0.1f && SCT_pc_ABS_F(stBoxMin.xY) < 0.1f) {
                stBoxMin.xX = 0.70710677f;
                stBoxMin.xY = 0.70710677f;
            }
            xLen = MTH_M_xSqrt(stBoxMin.xY * stBoxMin.xY + stBoxMin.xX * stBoxMin.xX);
            stBoxMin.xZ = 0.0f;
            xInvLen = 1.0f / xLen;
            xScale = 1.0f / ((stOrigin.xZ - stHit.xZ) * 0.3f - -1.0f);
            stBoxMin.xX = stBoxMin.xX * xInvLen;
            stBoxMin.xY = stBoxMin.xY * xInvLen;
            xSizeA = xSizeA * xScale;
            xSizeB2 = xSizeB * xScale;
            xAxisX = stBoxMin.xX * xSizeA;
            xAxisY = stBoxMin.xY * xSizeA;
            xDepthX = stBoxMin.xX * xSizeB2;
            xDepthY = stBoxMin.xY * xSizeB2;

            if (((((GLI_pc_g_cSinusEffectState == 1)
                   && (((HIE_tdstSuperObject *)hHitSO)->lDrawModeMask & 0x80000000UL) == 0)
                  || (GLI_pc_g_cSinusEffectState == 2
                      && (((HIE_tdstSuperObject *)hHitSO)->lDrawModeMask & 0x80000000UL) != 0))
                 || GLI_pc_g_cSinusEffectState == 3)
                && (stHit.xZ = stHit.xZ + SCT_pc_g_xMirrorHeightOffset,
                    stOrigin.xZ - stHit.xZ < 0.3))
            {
                stHit.xZ = stOrigin.xZ - 0.3f;
            }

            stVertex.xX = (xAxisX - xDepthY) + stHit.xX;
            stVertex.xY = (xAxisY + xDepthX) + stHit.xY;
            stHit.xZ = stHit.xZ - -0.03f;
            stVertex.xZ = SHW_fn_vComputeZofShadowPoint(&stHit, &stNormal, xInvZ,
                                                      stVertex.xX, stVertex.xY);

            GEO_vSetPointOfObject(p_ucElem, &stVertex, 0);
            xUV[0] = 0.0f;
            xUV[1] = 0.0f;
            GEO_vSetUVOfIndexedTriangles(p_ucElem, 0, 0, xUV);

            stDouble.xX = stHit.xX + stHit.xX;
            stDouble.xY = stHit.xY + stHit.xY;
            stDouble.xZ = stHit.xZ + stHit.xZ;
            stVertex.xX = stDouble.xX - stVertex.xX;
            stVertex.xY = stDouble.xY - stVertex.xY;
            stVertex.xZ = stDouble.xZ - stVertex.xZ;
            GEO_vSetPointOfObject(p_ucElem, &stVertex, 2);
            xUV[0] = 1.0f;
            xUV[1] = 1.0f;
            GEO_vSetUVOfIndexedTriangles(p_ucElem, 0, 2, xUV);

            stVertex.xX = xDepthY + xAxisX + stHit.xX;
            stVertex.xY = (xAxisY - xDepthX) + stHit.xY;
            stVertex.xZ = SHW_fn_vComputeZofShadowPoint(&stHit, &stNormal, xInvZ,
                                                      stVertex.xX, stVertex.xY);
            GEO_vSetPointOfObject(p_ucElem, &stVertex, 1);
            xUV[0] = 1.0f;
            xUV[1] = 0.0f;
            GEO_vSetUVOfIndexedTriangles(p_ucElem, 0, 1, xUV);

            stVertex.xX = stDouble.xX - stVertex.xX;
            stVertex.xY = stDouble.xY - stVertex.xY;
            stVertex.xZ = stDouble.xZ - stVertex.xZ;
            GEO_vSetPointOfObject(p_ucElem, &stVertex, 3);
            xUV[0] = 0.0f;
            xUV[1] = 1.0f;
            GEO_vSetUVOfIndexedTriangles(p_ucElem, 0, 3, xUV);

            GEO_vSetFaceOfIndexedTriangles(p_ucElem, 0, 0, 0, 3, 1);
            GEO_vSetIndexedUVOfFaceOfIndexedTriangles(p_ucElem, 0, 0, 0, 3, 1);
            GEO_vSetFaceOfIndexedTriangles(p_ucElem, 0, 1, 1, 3, 2);
            GEO_vSetIndexedUVOfFaceOfIndexedTriangles(p_ucElem, 0, 1, 1, 3, 2);

            ((GEO_tdstGeometricObject *)p_ucElem)->xNbPoints = 4;
            ((GEO_tdstElementIndexedTriangles *)((GEO_tdstGeometricObject *)p_ucElem)->d_stListOfElements[0])->xNbFaces = 2;
            SHW_fn_vSentToViewPort(p_stVpt, p_stSuperObject,
                                                   (long)p_stSectorSuperObject, ulDrawMask,
                                                   p_ucElem);
            GAM_g_uw5115d0_storage = (unsigned short)(GAM_g_uw5115d0_storage + 1);
        }
    }
}
