#include "GLI/light_fog.h"

extern float AI_GLI_xFogNear;
extern float AI_GLI_xFogFar;
extern float AI_GLI_xFogInf;
extern float AI_GLI_xFogBlendNear;
extern float AI_GLI_xFogBlendFar;
extern float AI_GLI_a_xFogColor[4];

extern void  __cdecl GEO_vGetPointOfObject(void *hObject, MTH3D_tdstVector *p_stPoint, short xIndexOfPoint);
extern void  __cdecl GEO_fn_vComputeCameraParametersForCulling(void *p_stVpt, float xZFar);
extern float __cdecl fn_xGetSectorZFar(void *_hSprObjSector);
extern void  __cdecl SCT_pc_fn_vCollectMirrorsOfSectors(void *_hVpt, void *_hSprObjSector,
                                                        void *_p_hMirrorSO, void *_p_hGeomMirror,
                                                        void *_p_hSectorList, short *_p_shNb);
extern void  __cdecl SCT_pc_fn_vComputeMirrorMatrix(struct POS_tdstCompletePosition_ *_p_stSourceMatrix, struct POS_tdstCompletePosition_ *_p_stMirrorMatrix,
                                                    struct MTH3D_tdstVector_ *_p_stReflectedPoint, struct MTH3D_tdstVector_ *_p_stPlaneNormal,
                                                    struct MTH3D_tdstVector_ *_p_stPlanePoint);
extern void  __cdecl SCT_fn_vSendSectorWhereIAmToViewport(void *p_stCamPos, void *p_stVpt,
                                                          void *hSprObjSector, long lDrawMask);
extern void  __cdecl SECT_fn_vSendDynamicLightsFromWhereIAmToViewport(void *p_stVpt, char *p_stSprObj);
extern void  __cdecl SECT_fn_vSendFogToViewport(unsigned long ulVptUnused, char *p_stSprObj);
extern void  __cdecl GLI_vSetFog2(float xBlendStart, float xDepthStart, float xBlendEnd,
                                  float xDepthEnd, float xInfinite, unsigned long *p_ulColor);
extern void  __cdecl GLI_vSetTextureDontAcceptFog(void);
extern void  __cdecl GLI_xClearViewingList(void);
extern void  __cdecl GLI_xSendListToViewport(unsigned long arg);
extern void  __cdecl GLI_vClearListOfLightInViewport(void *p_stVpt);
extern void  __cdecl FUN_004225d0(unsigned long *p_dst, unsigned long *p_src);
extern void  __cdecl FUN_00423990(unsigned long arg1_unused, void *p_vec3, float xScalar);
extern void  __cdecl FUN_004239c0(void);
extern void  __cdecl FUN_004239d0(void);
extern void  __cdecl FUN_004239e0(void);
extern long  __cdecl GLD_bGetDeviceAttributes(short hDev, void *pstDevAttrib);
extern void  __cdecl GLI_xGetCameraMatrix(void *p_stCam, POS_tdstCompletePosition *p_stMatrix);
extern void  __cdecl GLI_xSetCameraMatrix(void *p_stCam, POS_tdstCompletePosition *p_stMatrix);
extern void  __cdecl GLI_xSetViewportCamera(short hDevice, short hViewport, long lCam);
extern void  __cdecl GLI_xGetViewportCamera(unsigned long hDevice, unsigned short hViewport, void **p_stCam);
extern void  __cdecl GLI_xAdjustCameraToViewport2(void *p_stDev, void *p_stVpt, void *p_stCam);
extern unsigned short __cdecl GLI_pc_fn_usClipQuadToScreenRect(void *p_stObj, POS_tdstCompletePosition *p_stMatA,
                                                               POS_tdstCompletePosition *p_stMatB, void *p_stCam,
                                                               void *p_stRect, unsigned long ulFlags);
extern void  __cdecl POS_fn_vSetIdentityMatrix(POS_tdstCompletePosition *p_stPos);
extern void  __cdecl POS_fn_vMulMatrixVertex(MTH3D_tdstVector *_p_stDest, POS_tdstCompletePosition *_hMatrix, MTH3D_tdstVector *_p_stSource);
extern void  __cdecl POS_fn_vMulMatrixVector(MTH3D_tdstVector *_p_stDest, POS_tdstCompletePosition *_hMatrix, MTH3D_tdstVector *_p_stSource);
extern void  __cdecl POS_fn_vGetRotationMatrix(POS_tdstCompletePosition *_hMatrix, MTH3D_tdstVector *_p_stI, MTH3D_tdstVector *_p_stJ, MTH3D_tdstVector *_p_stK);
extern void  __cdecl POS_fn_vSetRotationMatrix(POS_tdstCompletePosition *_hMatrix, MTH3D_tdstVector *_p_stI, MTH3D_tdstVector *_p_stJ, MTH3D_tdstVector *_p_stK);
extern void  __cdecl AGO_vProcessAllParts(void *p_stVpt);
extern void  __cdecl HIE_fn_vSendSuperObjectToViewport(void *_hVpt, void *_hSprObj, long _lDrawMask);
extern int   __cdecl HIE_fn_bInitCameraMatrixStack(void *_hVpt);
extern void  __cdecl HIE_fn_vPopMatrix(void);
extern void  __cdecl HIE_fn_vPopOnCameraMatrixStack(void);
extern char  DAT_005115c9;

#include "GLD/GLD_Pub.h"
#include "HIE/HIE_types.h"
#include "GEO/GEO_types.h"
#include "SCT/Sector.h"
#include "MTH/MTH3D.h"

#define SCT_C_xNbMaxOfMirrorsInVisibleSectors 6

void GEO_vGetNormalOfGeometricObjectElement(void *p_stGeomObj, short xElements,
                                            short xIndexOfFace, MTH3D_tdstVector *p_stNormal);

void SCT_fn_vSendSectorWhereIAmToViewportWithMirror(MTH3D_tdstVector *p_stAbsolutePositionOfCamera,
                                                    unsigned short hDevice,
                                                    unsigned short hViewport,
                                                    void *p_stVpt,
                                                    void *hSprObjSector,
                                                    unsigned long lDrawMask)
{
    unsigned long  aulCamCopy[55];
    unsigned long  aulDevAttr[24];
    POS_tdstCompletePosition stMatrix;
    POS_tdstCompletePosition *a_hMirrorMatrix[SCT_C_xNbMaxOfMirrorsInVisibleSectors];
    void          *apvSectorElem[SCT_C_xNbMaxOfMirrorsInVisibleSectors];
    void          *apvMirrorSO[SCT_C_xNbMaxOfMirrorsInVisibleSectors];
    void          *apvGeomMirror[SCT_C_xNbMaxOfMirrorsInVisibleSectors];
    MTH3D_tdstVector stI;
    MTH3D_tdstVector stJ;
    MTH3D_tdstVector stK;
    MTH3D_tdstVector stWorldPoint;
    MTH3D_tdstVector stLocalPoint;
    MTH3D_tdstVector stNormal;
    MTH3D_tdstVector stReflected;
    unsigned char  acVisible[8];
    void          *pvCam;
    void         **ppvGeom;
    void         **ppvSectorElem;
    void         **ppvMirrorSO;
    void          *pvElemSO;
    unsigned char *pucVisible;
    struct GLD_tdstViewportAttributes_ *pstVptAttr;
    unsigned long  ulSavedVpt10;
    unsigned long  ulSavedVpt11;
    unsigned long  ulSavedVpt0E;
    unsigned long  ulSavedVpt0F;
    unsigned long  ulMinVpt10;
    unsigned long  ulMinVpt0E;
    unsigned long  ulMaxVpt11;
    unsigned long  ulMaxVpt0F;
    unsigned short usClip;
    float          fZFar;
    short          shCount;
    int            i;
    int            iIdx;

    pstVptAttr = (struct GLD_tdstViewportAttributes_ *)p_stVpt;

    fZFar = fn_xGetSectorZFar(hSprObjSector);
    GEO_fn_vComputeCameraParametersForCulling(p_stVpt, fZFar);
    SCT_pc_fn_vCollectMirrorsOfSectors(p_stVpt, hSprObjSector, (void *)apvMirrorSO,
                                       (void *)apvGeomMirror, (void *)apvSectorElem, &shCount);

    if (GLI_g_cGlobalFogIsOn != 0) {
        GLI_vSetFog2(AI_GLI_xFogBlendNear, AI_GLI_xFogNear, AI_GLI_xFogBlendFar,
                     AI_GLI_xFogFar, AI_GLI_xFogInf, (unsigned long *)AI_GLI_a_xFogColor);
        GLI_vFogOn();
    }

    if (shCount != 0) {
        ulSavedVpt10 = pstVptAttr->dwClipLeftInPix;
        ulSavedVpt11 = pstVptAttr->dwClipRightInPix;
        ulSavedVpt0E = pstVptAttr->dwClipTopInPix;
        ulSavedVpt0F = pstVptAttr->dwClipBottomInPix;
        ulMinVpt10 = 0x10000;
        ulMaxVpt11 = 0;
        ulMinVpt0E = 0x10000;
        ulMaxVpt0F = 0;

        GLD_bGetDeviceAttributes((unsigned long)(short)hDevice, (void *)aulDevAttr);

        for (i = 0; (short)i < shCount; i++) {
            iIdx = (int)(short)i;
            acVisible[iIdx] = 1;
            pucVisible = acVisible + iIdx;
            ppvMirrorSO = (void **)&apvMirrorSO[iIdx];
            ppvGeom = &apvGeomMirror[iIdx];
            ppvSectorElem = &apvSectorElem[iIdx];

            a_hMirrorMatrix[iIdx] = ((HIE_tdstSuperObject *)apvMirrorSO[iIdx])->hGlobalMatrix;
            GEO_vGetPointOfObject(apvGeomMirror[iIdx], &stLocalPoint, 0);
            POS_fn_vMulMatrixVertex(&stWorldPoint, a_hMirrorMatrix[iIdx], &stLocalPoint);
            GEO_vGetNormalOfGeometricObjectElement(apvGeomMirror[iIdx], 0, 0, &stLocalPoint);
            POS_fn_vGetRotationMatrix(a_hMirrorMatrix[iIdx], &stI, &stJ, &stK);
            POS_fn_vSetIdentityMatrix(&stMatrix);
            POS_fn_vSetRotationMatrix(&stMatrix, &stI, &stJ, &stK);
            POS_fn_vMulMatrixVector(&stNormal, &stMatrix, &stLocalPoint);

            /* 0x44B90E */
            MTH3D_M_vSubVector(&stI, p_stAbsolutePositionOfCamera, &stWorldPoint);
            if (stI.xX * stNormal.xX + stI.xY * stNormal.xY + stI.xZ * stNormal.xZ < 0.0f) {
                *pucVisible = 0;
            }

            if (*pucVisible != 0) {
                GLI_xGetViewportCamera((unsigned long)hDevice, hViewport, &pvCam);
                FUN_004225d0(aulCamCopy, (unsigned long *)pvCam);
                GLI_xGetCameraMatrix((void *)aulCamCopy, &stMatrix);
                SCT_pc_fn_vComputeMirrorMatrix(&stMatrix, &stMatrix, &stReflected,
                                               &stNormal, &stWorldPoint);
                GLI_xSetCameraMatrix((void *)aulCamCopy, &stMatrix);
                GLI_xSetViewportCamera((unsigned long)hDevice, hViewport, (long)aulCamCopy);

                usClip = GLI_pc_fn_usClipQuadToScreenRect(apvGeomMirror[iIdx], a_hMirrorMatrix[iIdx],
                                                          &stMatrix, (void *)aulCamCopy, p_stVpt, lDrawMask);
                if ((char)usClip == 0) {
                    *pucVisible = 0;
                }

                GLI_xAdjustCameraToViewport2((void *)aulDevAttr, p_stVpt, (void *)aulCamCopy);
                fZFar = fn_xGetSectorZFar(hSprObjSector);
                GEO_fn_vComputeCameraParametersForCulling(p_stVpt, fZFar);

                if (*pucVisible != 0) {
                    pvElemSO = ((HIE_tdstSuperObject *)*ppvSectorElem)->hLinkedObject.p_Void;

                    if (((GEO_tdstGeometricObject *)*ppvGeom)->xNbElements == 1) {
                        FUN_00423990((unsigned long)p_stVpt, &stNormal,
                                     stWorldPoint.xX * stNormal.xX + stWorldPoint.xY * stNormal.xY + stWorldPoint.xZ * stNormal.xZ);
                    }
                    else {
                        FUN_004239d0();
                    }

                    SCT_fn_vSendSectorWhereIAmToViewport(&stReflected, p_stVpt,
                        (((SECT_tdxHandleOfSectorObject)pvElemSO)->xVirtual == 0) ? *ppvSectorElem : hSprObjSector,
                        (long)(lDrawMask + 0xFDF00000UL));

                    if (pstVptAttr->dwClipLeftInPix < ulMinVpt10) {
                        ulMinVpt10 = pstVptAttr->dwClipLeftInPix;
                    }
                    if (pstVptAttr->dwClipTopInPix < ulMinVpt0E) {
                        ulMinVpt0E = pstVptAttr->dwClipTopInPix;
                    }
                    if (ulMaxVpt11 < pstVptAttr->dwClipRightInPix) {
                        ulMaxVpt11 = pstVptAttr->dwClipRightInPix;
                    }
                    if (ulMaxVpt0F < pstVptAttr->dwClipBottomInPix) {
                        ulMaxVpt0F = pstVptAttr->dwClipBottomInPix;
                    }
                    pstVptAttr->dwClipRightInPix = ulSavedVpt11;
                    pstVptAttr->dwClipLeftInPix = ulSavedVpt10;
                    pstVptAttr->dwClipTopInPix = ulSavedVpt0E;
                    pstVptAttr->dwClipBottomInPix = ulSavedVpt0F;

                    if (((GEO_tdstGeometricObject *)*ppvGeom)->xNbElements != 1) {
                        FUN_004239c0();
                    }
                    GLI_xSendListToViewport((unsigned long)p_stVpt);
                    GLI_xClearViewingList();
                    GLI_xSetViewportCamera((unsigned long)hDevice, hViewport, (long)pvCam);

                    DAT_005115c9 = 1;
                    FUN_004239e0();
                    HIE_fn_bInitCameraMatrixStack(p_stVpt);
                    HIE_fn_vPushMatrix(*ppvMirrorSO);
                    HIE_fn_vPushOnCameraMatrixStack(*ppvMirrorSO);
                    SECT_fn_vSendDynamicLightsFromWhereIAmToViewport(p_stVpt, (char *)*ppvSectorElem);
                    SECT_fn_vSendFogToViewport((unsigned long)p_stVpt, (char *)*ppvSectorElem);
                    HIE_fn_vSendSuperObjectToViewport(p_stVpt, *ppvMirrorSO, (long)lDrawMask);
                    GLI_vClearListOfLightInViewport(p_stVpt);
                    HIE_fn_vPopMatrix();
                    HIE_fn_vPopOnCameraMatrixStack();
                    DAT_005115c9 = 0;
                }
            }
        }
    }

    fZFar = fn_xGetSectorZFar(hSprObjSector);
    GEO_fn_vComputeCameraParametersForCulling(p_stVpt, fZFar);
    SCT_fn_vSendSectorWhereIAmToViewport(p_stAbsolutePositionOfCamera, p_stVpt, hSprObjSector, (long)lDrawMask);
    AGO_vProcessAllParts(p_stVpt);
}
