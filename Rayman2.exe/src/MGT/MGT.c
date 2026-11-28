#include "MGT/MGT.h"
#include "MGT/MGTStruc.h"
#include "MGT/MGT_pending.h"
#include "POS/POS_types.h"
#include "GEO/GeoObj.h"
#include "MGT/MGT_globals.h"
#include "MTH/MTH3D.h"
#define D_MSMagnet_StructureDefine
#include "GAM/MSMag.h"
#include "PO/IPOProto.h"
#include "GLI/Memory.h"
#include "INT/Inters.h"
#include "GAM/GAM_types.h"

/* 0x0045C1D0 */
void __cdecl HIE_fn_vSetMagnetModification(struct HIE_tdstSuperObject *_hSprObj, unsigned char _xModified);
/* 0x0045CD30 */
void * __cdecl HIE_fn_p_stGetGeometricObjectFromSuperObject(struct HIE_tdstSuperObject *h_SuperObject);

/* 0x00500FC0 */
extern struct HIE_tdstSuperObject *SECT_hFatherSector;

/* 0x0049C840 */
typedef void          (__cdecl *tVoid)(void);
typedef void          (__cdecl *tVoidIntChar)(int, char);
typedef unsigned char (__cdecl *tGetByteInt)(int);
typedef float *       (__cdecl *tGetVecInt)(int);
typedef void          (__cdecl *tVoidIntFloatPtr)(int, float *);
typedef char          (__cdecl *tCharShortPtr)(short *);
typedef MGT_tdstMagnetModification * (__cdecl *tGetModByIdx)(int, short);

void __cdecl MGT_pc_fn_vReleaseActiveMagnetsClearMods(void);                 /* 0x00440D20 */
void __cdecl MGT_pc_fn_vResetPendingBitmapsAndUsed(void);                    /* 0x00441010 */
void __cdecl MGT_pc_fn_vDestroyUnusedPendingModifs(void);                    /* 0x00441060 */
void __cdecl MGT_pc_fn_vUpdateMagnetModifiedChildren(void);                  /* 0x00441420 */
unsigned char __cdecl MGT_pc_fn_ucRefreshMagnetModification(MGT_tdstMagnetModification *p_stMod); /* 0x004417A0 */
void __cdecl MGT_pc_fn_vReleaseActiveMagnetIfEmpty(short sMagnetIdx);       /* 0x00440C90 */
void __cdecl MGT_pc_fn_vClearModifiedObjectBitmap(MGT_tdstModifiedObject *p_stObj); /* 0x00441F80 */
void __cdecl MGT_pc_fn_vUpdateMagnetInfluenceTree(unsigned int *p_uSprObj); /* 0x00441450 */
void __cdecl MGT_pc_fn_vAccumulatePointDisplacement(unsigned long ulUnused, short sPointIdx,
                                                   int pBitmap, unsigned int uCount,
                                                   float *p_xPosA, float *p_xOut, float *p_xPosB,
                                                   float xNearDist, float xFarDist,
                                                   float xFalloff, float xScale);            /* 0x00441B10 */
void __cdecl MGT_pc_fn_vBlendPointDisplacementByRatio(unsigned long ulUnused, short sPointIdx,
                                                     int pBitmap, unsigned int uCount,
                                                     int iCount2, MTH3D_tdstVector *p_stPosA,
                                                     MTH3D_tdstVector *p_stOut);             /* 0x00441CD0 */
unsigned char __cdecl fn_bMSMagnetIsStrengthModified(void *h_MSMagnet);     /* 0x00415370 */
unsigned char __cdecl fn_bMSMagnetIsPositionModified(void *h_MSMagnet);     /* 0x004153A0 */

#include "SCT/Sector.h"

unsigned char __cdecl HIE_fn_bGetMagnetModification(struct HIE_tdstSuperObject *_hSprObj);
void __cdecl GEO_pc_fn_vExtendBoxWithPointsAndSprites(int *p_iObj, unsigned int *p_uMatrix,
                                                     float *p_xMax, float *p_xMin);

MGT_tdstMagnetModification *fn_hMSMagnetGetMagnetModification(MS_tdxHandleToMSMagnet h_MSMagnet, ACP_tdxIndex xIndex );

MTH3D_tdstVector * fn_hMSMagnetGetPosition(MS_tdxHandleToMSMagnet h_MSMagnet);

void fn_vMSMagnetSetPosition(MS_tdxHandleToMSMagnet h_MSMagnet,MTH3D_tdstVector *stPosition);

ACP_tdxBool fn_bMSMagnetIsNearFarModified(MS_tdxHandleToMSMagnet h_MSMagnet);

void fn_vMSMagnetSetPositionModification(MS_tdxHandleToMSMagnet h_MSMagnet,ACP_tdxBool _bModified);

void __cdecl MGT_pc_fn_vUpdateMagnetTarget(int iMagnet, MS_tdxHandleToMSMagnet p_stTarget, int iSlot);

/* 0x00440B50 */
void __cdecl MGT_fn_vAddObjectToMagnetList(struct HIE_tdstSuperObject *p_stObject)
{
    short   sCount = g_lNbActiveMagnets;
    short   sSlot  = sCount;
    int     iObj   = (int)p_stObject;
    int     iLoop  = 0;
    MS_tdxHandleToMSMagnet    hMagnet;
    POS_tdstCompletePosition *p_stMatrix;
    MTH3D_tdstVector         *p_stPosition;

    if (0 < sCount) {
        do {
            if (g_aActiveMagnets[(short)iLoop] == (int *)iObj) {
                sSlot = (short)iLoop;
                break;
            }
            if (sSlot == sCount && g_aActiveMagnets[(short)iLoop] == 0)
                sSlot = (short)iLoop;
            iLoop++;
        } while ((short)iLoop < sCount);
    }

    if (sSlot != 5) {
        g_aActiveMagnets[sSlot] = (int *)iObj;
        if (sSlot == sCount)
            g_lNbActiveMagnets = (short)(sCount + 1);
        hMagnet = M_GetMSHandle(p_stObject, MSMagnet);
        if (hMagnet != NULL) {
            fn_vMSMagnetSetStatus(hMagnet, 2);
            p_stMatrix = (POS_tdstCompletePosition *)p_stObject->hGlobalMatrix;
            p_stPosition = fn_hMSMagnetGetPosition(hMagnet);
            POS_fn_vGetTranslationVector(p_stMatrix, p_stPosition);
            MGT_pc_fn_vUpdateMagnetTarget(iObj, hMagnet, (int)sSlot);
        }
    }
}

/* 0x0040C0C0 */
void __cdecl MGT_fn_vDeleteObjectFromMagnetList(struct HIE_tdstSuperObject *p_stObject)
{
    short sIdx;

    for (sIdx = 0; sIdx < g_lNbActiveMagnets; sIdx = (short)(sIdx + 1)) {
        if (g_aActiveMagnets[sIdx] == (int *)p_stObject) {
            short sNewCount = (short)(g_lNbActiveMagnets - 1);
            g_aActiveMagnets[sIdx] = 0;
            if (sNewCount != 0)
                g_lNbActiveMagnets = sNewCount;

            fn_vMSMagnetSetStatus((void *)*(int *)(*(int *)((int)p_stObject + 4) + 0x34), 0);
            return;
        }
    }
}

/* 0x00440C60 */
void __cdecl MGT_pc_fn_vClearActiveMagnet(short sMagnetIdx)
{
    short sCount;

    sCount = g_lNbActiveMagnets;
    g_aActiveMagnets[sMagnetIdx] = (int *)0;
    if ((int)sMagnetIdx == (int)sCount - 1) {
        g_lNbActiveMagnets = (short)(g_lNbActiveMagnets - 1);
    }
}

/* 0x00440C90 */
void __cdecl MGT_pc_fn_vReleaseActiveMagnetIfEmpty(short sMagnetIdx)
{
    int           iTarget;
    short         sSlot;
    unsigned char ucStatus;

    iTarget = (int)M_GetMSHandle((HIE_tdstSuperObject *)g_aActiveMagnets[sMagnetIdx], MSMagnet);
    if (iTarget != 0) {
        for (sSlot = 0; sSlot < 0x14; sSlot++) {
            if (fn_hMSMagnetGetMagnetModification((MS_tdxHandleToMSMagnet)iTarget, sSlot) != (MGT_tdstMagnetModification *)0)
                break;
        }
        if (sSlot != 0x14)
            return;
        ucStatus = fn_lMSMagnetGetStatus((void *)iTarget);
        if (ucStatus != 3)
            return;
        fn_vMSMagnetSetStatus((void *)iTarget, 0);
    }
    MGT_pc_fn_vClearActiveMagnet(sMagnetIdx);
}

/* 0x00440CF0 */
void __cdecl MGT_fn_vReleaseAllEmptyModifiedObjects(void)
{
    short                    sIdx     = 0;
    MGT_tdstModifiedObject **pp_stObj = g_aModifiedObjects;

    if (0 < g_lNbPendingMagnets) {
        do {
            if (*pp_stObj != 0)
                MGT_fn_vReleasePendingIfEmpty(sIdx);
            sIdx = (short)(sIdx + 1);
            pp_stObj++;
        } while ((short)sIdx < g_lNbPendingMagnets);
    }
}

/* 0x00440D20 */
void __cdecl MGT_pc_fn_vReleaseActiveMagnetsClearMods(void)
{
    short   sIdx;
    int   **pp_stActive;
    int     iTarget;

    sIdx = 0;
    pp_stActive = g_aActiveMagnets;
    if (0 < g_lNbActiveMagnets) {
        do {
            if (*pp_stActive != (int *)0) {
                iTarget = *(int *)(*(int *)((char *)*pp_stActive + 4) + 0x34);
                MGT_pc_fn_vReleaseActiveMagnetIfEmpty(sIdx);
                if (iTarget != 0) {
                    fn_vMSMagnetSetStrengthModification((void *)iTarget, 0);
                    fn_vMSMagnetSetNearFarModification((void *)iTarget, 0);
                }
            }
            sIdx = (short)(sIdx + 1);
            pp_stActive++;
        } while (sIdx < g_lNbActiveMagnets);
    }
}

void __cdecl MGT_fn_vClearTargetModificationsInfluencedBits(int p_target, ACP_tdxBool bFlag);
void __cdecl MGT_fn_vSetTargetModificationsUsed(int p_target, ACP_tdxBool bUsed);

/* 0x00440D80 */
void __cdecl MGT_fn_vTreatMagnets(void)
{
    int    *piMagnetEntry;
    MGT_tdstModifiedObject **pp_stObj;
    short   sMagnetIdx;
    short   sPendingIdx;
    MTH3D_tdstVector stPos;
    float   _scratch_20[3];
    float  *p_targetOffset;
    float  *p_inputVec;
    float  *p_outputVec;

    if (g_lNbPendingMagnets != 0)
        MGT_pc_fn_vResetPendingBitmapsAndUsed();

    sMagnetIdx    = 0;
    piMagnetEntry = (int *)g_aActiveMagnets;
    if (0 < g_lNbActiveMagnets) {
        do {
            MS_tdxHandleToMSMagnet p_target;

            if (*piMagnetEntry == 0) {
                p_target = NULL;
            } else {

                p_target = M_GetMSHandle((struct HIE_tdstSuperObject *)*piMagnetEntry, MSMagnet);
            }

            if (p_target != NULL) {
                unsigned char ucState;
                unsigned char ucInTransition;

                fn_lMSMagnetGetStatus(p_target);
                ucInTransition = fn_bMSMagnetIsNearFarModified(p_target);
                if (ucInTransition == 0) {
                    ucState = fn_lMSMagnetGetStatus(p_target);
                    switch (ucState) {
                        case 0:
                        case 3:
                            fn_vMSMagnetSetPositionModification(p_target, 0);
                            MGT_fn_vClearTargetModificationsInfluencedBits((int)p_target, 0);
                            MGT_fn_vSetTargetModificationsUsed((int)p_target, 1);
                            break;

                        case 1:
                        case 2: {
                            double distSq;
                            MTH3D_tdstVector stDiff;

                            POS_fn_vGetTranslationVector(
                                (POS_tdstCompletePosition *)((struct HIE_tdstSuperObject *)*piMagnetEntry)->hGlobalMatrix,
                                &stPos);
                            p_targetOffset = (float *)fn_hMSMagnetGetPosition(p_target);
                            p_outputVec    = (float *)&stPos;
                            p_inputVec     = _scratch_20;
                            (void)p_outputVec;
                            (void)p_inputVec;

                            MTH3D_M_vSubVector(&stDiff, &stPos, (MTH3D_tdstVector *)p_targetOffset);
                            distSq = (double)stDiff.xX * (double)stDiff.xX
                                   + (double)stDiff.xY * (double)stDiff.xY
                                   + (double)stDiff.xZ * (double)stDiff.xZ;

                            if (distSq <= 0.04) {
                                fn_vMSMagnetSetPositionModification(p_target, 0);
                                MGT_fn_vSetTargetModificationsUsed((int)p_target, 1);
                            } else {
                                fn_vMSMagnetSetPosition(p_target, &stPos);
                                fn_vMSMagnetSetPositionModification(p_target, 1);
                                MGT_pc_fn_vUpdateMagnetTarget(*piMagnetEntry, p_target, (int)sMagnetIdx);
                            }
                            break;
                        }

                        default:

                            break;
                    }
                } else {

                    POS_fn_vGetTranslationVector(
                        (POS_tdstCompletePosition *)((struct HIE_tdstSuperObject *)*piMagnetEntry)->hGlobalMatrix,
                        &stPos);
                    p_targetOffset = (float *)fn_hMSMagnetGetPosition(p_target);
                    p_outputVec    = (float *)&stPos;
                    p_inputVec     = _scratch_20;
                    (void)p_outputVec;
                    (void)p_inputVec;
                    fn_vMSMagnetSetPosition(p_target, &stPos);
                    MGT_pc_fn_vUpdateMagnetTarget(*piMagnetEntry, p_target, (int)sMagnetIdx);
                }
            }

            piMagnetEntry++;
            sMagnetIdx = (short)(sMagnetIdx + 1);
        } while ((short)sMagnetIdx < g_lNbActiveMagnets);
    }

    if (g_lNbPendingMagnets != 0)
        MGT_pc_fn_vDestroyUnusedPendingModifs();

    sPendingIdx = 0;
    pp_stObj    = g_aModifiedObjects;
    if (0 < g_lNbPendingMagnets) {
        do {
            MGT_tdstModifiedObject *p_stObj = *pp_stObj;
            if (p_stObj != (MGT_tdstModifiedObject *)0) {
                int i;
                int bNeedRefresh = 0;

                for (i = 0; i < MGT_MAX_NUMBEROF_MAGNETS; i++) {
                    MGT_tdstMagnetModification *p_stMod = p_stObj->aDEF_hMagnet[i];
                    if (p_stMod != (MGT_tdstMagnetModification *)0 &&
                        MGT_pc_fn_ucRefreshMagnetModification(p_stMod) != 0) {
                        bNeedRefresh = 1;
                    }
                }

                if (bNeedRefresh) {
                    GEO_vEndModifyObject2(
                        (ACP_tdxHandleOfObject)p_stObj->p_stMorphedObject);
                }
            }
            pp_stObj++;
            sPendingIdx = (short)(sPendingIdx + 1);
        } while ((short)sPendingIdx < g_lNbPendingMagnets);
    }

    if (g_lNbPendingMagnets != 0) {
        MGT_pc_fn_vUpdateMagnetModifiedChildren();
        MGT_fn_vReleaseAllEmptyModifiedObjects();
    }

    if (g_lNbActiveMagnets != 0)
        MGT_pc_fn_vReleaseActiveMagnetsClearMods();
}

/* 0x00441010 */
void __cdecl MGT_pc_fn_vResetPendingBitmapsAndUsed(void)
{
    short                    sIdx = 0;
    int                      iMag;
    MGT_tdstModifiedObject **pp_stObj = g_aModifiedObjects;
    MGT_tdstMagnetModification **pp_stMag;

    if (0 < g_lNbPendingMagnets) {
        do {
            if (*pp_stObj != (MGT_tdstModifiedObject *)0) {
                MGT_pc_fn_vClearModifiedObjectBitmap(*pp_stObj);
                pp_stMag = (*pp_stObj)->aDEF_hMagnet;
                for (iMag = MGT_MAX_NUMBEROF_MAGNETS; iMag != 0; iMag--) {
                    if (*pp_stMag != (MGT_tdstMagnetModification *)0)
                        (*pp_stMag)->bUsed = 0;
                    pp_stMag++;
                }
            }
            sIdx = (short)(sIdx + 1);
            pp_stObj++;
        } while (sIdx < g_lNbPendingMagnets);
    }
}

/* 0x00441060 */
void __cdecl MGT_pc_fn_vDestroyUnusedPendingModifs(void)
{
    short                       sIdx = 0;
    short                       sMag;
    MGT_tdstModifiedObject    **pp_stObj = g_aModifiedObjects;
    MGT_tdstMagnetModification *p_stMod;
    MGT_tdstMagnetModification **pp_stMag;

    if (0 < g_lNbPendingMagnets) {
        do {
            if (*pp_stObj != (MGT_tdstModifiedObject *)0) {
                MGT_pc_fn_vClearModifiedObjectBitmap(*pp_stObj);
                sMag = 0;
                pp_stMag = (*pp_stObj)->aDEF_hMagnet;
                do {
                    p_stMod = *pp_stMag;
                    if (p_stMod != (MGT_tdstMagnetModification *)0) {
                        if (p_stMod->bUsed != 0)
                            break;
                        MGT_pc_fn_vDestroyMagnetModification(p_stMod);
                    }
                    sMag = (short)(sMag + 1);
                    pp_stMag++;
                } while (sMag < MGT_MAX_NUMBEROF_MAGNETS);
                if (sMag == MGT_MAX_NUMBEROF_MAGNETS)
                    MGT_fn_vReleasePendingIfEmpty(sIdx);
            }
            sIdx = (short)(sIdx + 1);
            pp_stObj++;
        } while (sIdx < g_lNbPendingMagnets);
    }
}

void __cdecl MGT_pc_fn_vClearMagnetModificationTree(int p_stSuper);
unsigned char __cdecl MGT_pc_fn_ucTreatMagnetTree(MS_tdxHandleToMSMagnet p_stTarget, int iSubTag, float fRadiusSq, struct HIE_tdstSuperObject *p_stSuper);
void __cdecl MGT_pc_fn_vUpdateMagnetTarget(int iMagnet, MS_tdxHandleToMSMagnet p_stTarget, int iSlot)
{
    float           fFar, fFarSq;
    struct HIE_tdstSuperObject *p_stSector;
    SECT_tdxHandleOfElementLstCollisionInteraction hCollisionNode;
    short           sSlot;
    int             iCount;
    long            lCount;

    MGT_fn_vClearTargetModificationsInfluencedBits((int)p_stTarget, 1);
    MGT_fn_vSetTargetModificationsUsed((int)p_stTarget, 0);
    fFar = fn_xMSMagnetGetFar(p_stTarget);
    fFarSq = MTH_M_xSqr(fFar);
    MGT_pc_fn_vClearMagnetModificationTree((int)SECT_hFatherSector);

    p_stSector = (struct HIE_tdstSuperObject *)fn_h_SectInfoGetCurrentSector(
                  ((struct tdstEngineObject_ *)((struct HIE_tdstSuperObject *)iMagnet)->hLinkedObject.p_Void)->h_SectInfo);
    MGT_pc_fn_ucTreatMagnetTree(p_stTarget, iSlot, fFarSq, p_stSector);

    lCount         = SECT_fn_iGetNumberOfNodesInCollisionList(p_stSector);
    hCollisionNode = SECT_fn_hGetFirstCollisionNodeInCollisionList(p_stSector);
    sSlot          = (short)lCount;
    if (0 < sSlot) {
        iCount = (int)sSlot;
        do {
            MGT_pc_fn_ucTreatMagnetTree(p_stTarget, iSlot, fFarSq,
                                        SECT_GetSectorInCollisionList(hCollisionNode));
            hCollisionNode = SECT_fn_hGetNextCollisionNodeInCollisionList(hCollisionNode);
            iCount--;
        } while (iCount != 0);
    }
    sSlot = 0;
    while (sSlot < 0x14 && fn_hMSMagnetGetMagnetModification(p_stTarget, sSlot) == (MGT_tdstMagnetModification *)0)
        sSlot = (short)(sSlot + 1);
    if (sSlot == 0x14) {
        if (fn_lMSMagnetGetStatus(p_stTarget) == 3) {
            fn_vMSMagnetSetStatus(p_stTarget, 0);
            MGT_pc_fn_vClearActiveMagnet((short)iSlot);
        }
    }
}

short __cdecl MGT_pc_fn_sFindModifiedObjectSlot(void *p_stGeo);
unsigned char __cdecl MGT_pc_fn_ucMarkInfluencedPoints(short sSubTag, float fRadiusSq, GEO_tdstGeometricObject *p_stGeo, short *psModIdx, float *p_stPos, struct HIE_tdstSuperObject *h_SuperObject);

/* 0x004411B0 */
unsigned char __cdecl MGT_pc_fn_ucTreatMagnetTree(MS_tdxHandleToMSMagnet p_stTarget, int iSubTag,
                                                  float fRadiusSq, struct HIE_tdstSuperObject *p_stSuper)
{
    unsigned char   ucResult = 0;
    unsigned char   ucModified = 0;
    unsigned char   bGo = 1;
    unsigned char   ucFlags;
    POS_tdstCompletePosition *p_stMatrix;
    struct HIE_tdstSuperObject *p_stChild;
    POS_tdstCompletePosition stInv;
    MTH3D_tdstVector *p_stOffset;
    GEO_tdxHandleToBoundingSphere hSphere;
    GEO_tdxHandleToParallelBox hBox;
    MTH3D_tdstVector *p_stPos;
    MTH3D_tdstVector stLocalPos;
    MTH3D_tdstVector stCenter;
    MTH3D_tdstVector stBoxMin;
    MTH3D_tdstVector stBoxMax;
    float           fFar;
    float           fRadius;
    void           *p_stGeo;
    short           sModIdx;

    p_stGeo = HIE_fn_p_stGetGeometricObjectFromSuperObject(p_stSuper);
    if (p_stGeo != (void *)0) {
        ucFlags = (unsigned char)p_stSuper->ulFlags;
        sModIdx = MGT_pc_fn_sFindModifiedObjectSlot(p_stGeo);
        p_stMatrix = (POS_tdstCompletePosition *)p_stSuper->hGlobalMatrix;
        p_stOffset = fn_hMSMagnetGetPosition(p_stTarget);
        if ((ucFlags & HIE_C_Flag_ulTypeOfBoundingVolume) == 0) {
            POS_fn_vInvertMatrix(&stInv, p_stMatrix);
            POS_fn_vMulMatrixVertex(&stLocalPos, &stInv, p_stOffset);
            hSphere = (GEO_tdxHandleToBoundingSphere)p_stSuper->hBoundingVolume;
            if (hSphere != NULL) {
                MTH3D_M_vCopyVector(&stCenter, &hSphere->stCenterPoint);
                fRadius = hSphere->xRadius;
                fFar = fn_xMSMagnetGetFar(p_stTarget);
                bGo = (unsigned char)INT_fn_bIntersectSphereWithSphere(
                          &stLocalPos, fFar, &stCenter, fRadius);
            }
        } else {
            hBox = (GEO_tdxHandleToParallelBox)p_stSuper->hBoundingVolume;
            if (hBox != NULL) {
                p_stPos = &p_stMatrix->stTranslationVector;
                MTH3D_M_vAddVector(&stBoxMin, p_stPos, &hBox->stMinPoint);
                MTH3D_M_vAddVector(&stBoxMax, p_stPos, &hBox->stMaxPoint);
                fFar = fn_xMSMagnetGetFar(p_stTarget);
                bGo = (unsigned char)INT_fn_bIntersectSphereWithBox(
                          p_stOffset, fFar, &stBoxMin, &stBoxMax);
            }
            if (bGo != 0) {
                POS_fn_vInvertMatrix(&stInv, p_stMatrix);
                POS_fn_vMulMatrixVertex(&stLocalPos, &stInv, p_stOffset);
            }
        }

        if (bGo != 0) {
            ucModified = MGT_pc_fn_ucMarkInfluencedPoints((short)iSubTag, fRadiusSq,
                                                          (GEO_tdstGeometricObject *)p_stGeo,
                                                          &sModIdx, (float *)&stLocalPos,
                                                          p_stSuper);
            if (ucModified != 0)
                HIE_fn_vSetMagnetModification(p_stSuper, 1);
        }
        if (sModIdx != -1 && ucModified == 0 && fn_lMSMagnetGetStatus(p_stTarget) == 3)
            MGT_fn_vDestroySubByTag(sModIdx, (short)iSubTag);
        ucResult = ucModified;
    }

    if (bGo != 0) {
        p_stChild = p_stSuper->p_stFirstChild;
        if (p_stChild != (struct HIE_tdstSuperObject *)0) {
            do {
                ucResult = (unsigned char)(ucResult | MGT_pc_fn_ucTreatMagnetTree(p_stTarget, iSubTag,
                                                                                  fRadiusSq, p_stChild));
                p_stChild = p_stChild->p_stNextBrother;
            } while (p_stChild != (struct HIE_tdstSuperObject *)0);
        }
    }
    HIE_fn_vSetMagnetModification(p_stSuper, ucResult);
    return ucResult;
}

/* 0x004413F0 */
void __cdecl MGT_pc_fn_vClearMagnetModificationTree(int p_stSuper)
{
    int iChild;

    HIE_fn_vSetMagnetModification((struct HIE_tdstSuperObject *)p_stSuper, 0);
    for (iChild = (int)((struct HIE_tdstSuperObject *)p_stSuper)->p_stFirstChild; iChild != 0;
         iChild = (int)((struct HIE_tdstSuperObject *)iChild)->p_stNextBrother) {
        MGT_pc_fn_vClearMagnetModificationTree(iChild);
    }
}

/* 0x00441420 */
void __cdecl MGT_pc_fn_vUpdateMagnetModifiedChildren(void)
{
    HIE_tdstSuperObject *p_stSprObj;

    for (p_stSprObj = SECT_hFatherSector->p_stFirstChild;
         p_stSprObj != (HIE_tdstSuperObject *)0;
         p_stSprObj = p_stSprObj->p_stNextBrother) {
        if (HIE_fn_bGetMagnetModification(p_stSprObj) != 0)
            MGT_pc_fn_vUpdateMagnetInfluenceTree((unsigned int *)p_stSprObj);
    }
}

/* 0x00441450 */
void __cdecl MGT_pc_fn_vUpdateMagnetInfluenceTree(unsigned int *p_uSprObj)
{
    unsigned char bHasMagnet;
    void         *p_stGeo;
    unsigned int *p_uMat;
    unsigned int *p_uChild;
    float        *pfM;
    float        *pfOut;
    float        *pfSrc;
    float         aCenter[3];
    float         fRadius;
    float         aVecA[3];
    float         aVecB[3];

    bHasMagnet = HIE_fn_bGetMagnetModification((struct HIE_tdstSuperObject *)p_uSprObj);
    if (bHasMagnet != 0) {
        p_stGeo = HIE_fn_p_stGetGeometricObjectFromSuperObject((struct HIE_tdstSuperObject *)p_uSprObj);
        p_uMat = (unsigned int *)p_uSprObj[9];
        pfM = (float *)(p_uMat + 0xd);
        if ((p_uSprObj[0xc] & 0x10) == 0) {
            pfOut = (float *)p_uSprObj[0xd];
            if (pfOut != (float *)0) {
                aCenter[0] = pfOut[0];
                aCenter[1] = pfOut[1];
                aCenter[2] = pfOut[2];
                fRadius = pfOut[3];
                if (p_stGeo == (void *)0) {
                    aCenter[0] = 0.0f;
                    aCenter[1] = 0.0f;
                    aCenter[2] = 0.0f;
                    fRadius = 0.0f;
                } else {
                    GEO_xComputeObjectSphereBox((GEO_tdstGeometricObject *)p_stGeo,
                                                (MTH3D_tdstVector *)aCenter, &fRadius);
                }
            }
        } else {
            pfOut = (float *)p_uSprObj[0xd];
            if (pfOut != (float *)0) {
                if (p_stGeo == (void *)0) {
                    pfOut[3] = 0.0f;
                    pfOut[4] = 0.0f;
                    pfOut[5] = 0.0f;
                    pfOut[0] = 0.0f;
                    pfOut[1] = 0.0f;
                    pfOut[2] = 0.0f;
                } else {
                    pfSrc = (float *)*(int *)p_stGeo;
                    aVecA[0] = pfSrc[0];
                    aVecA[1] = pfSrc[1];
                    aVecA[2] = pfSrc[2];
                    aVecB[0] = pfSrc[0];
                    aVecB[1] = pfSrc[1];
                    aVecB[2] = pfSrc[2];
                    /* 0x00441450 */
                    MTH3D_M_vMulMatrixVectorWithoutBuffer((MTH3D_tdstVector *)(pfOut + 3),
                                                          (MTH3D_tdstMatrix *)pfM,
                                                          (MTH3D_tdstVector *)aVecA);
                    MTH3D_M_vMulMatrixVectorWithoutBuffer((MTH3D_tdstVector *)pfOut,
                                                          (MTH3D_tdstMatrix *)pfM,
                                                          (MTH3D_tdstVector *)aVecB);
                    GEO_pc_fn_vExtendBoxWithPointsAndSprites((int *)p_stGeo, p_uMat,
                                                             pfOut + 3, pfOut);
                }
            }
        }
        for (p_uChild = (unsigned int *)p_uSprObj[2]; p_uChild != (unsigned int *)0;
             p_uChild = (unsigned int *)p_uChild[5]) {
            MGT_pc_fn_vUpdateMagnetInfluenceTree(p_uChild);
        }
    }
}

short __cdecl MGT_pc_fn_sAllocModifiedObject(void *p_stGeo);

/* 0x00441630 */
unsigned char __cdecl MGT_pc_fn_ucMarkInfluencedPoints(short sSubTag, float fRadiusSq,
                                                       GEO_tdstGeometricObject *p_stGeo,
                                                       short *psModIdx, float *p_stPos,
                                                       struct HIE_tdstSuperObject *h_SuperObject)
{
    unsigned char               ucResult = 0;
    MGT_tdstMagnetModification *p_stMod  = (MGT_tdstMagnetModification *)0;
    short                       sPt      = 0;
    float                      *p_stVert;
    float                       aDelta[3];
    double                      dDist;   /* 0x441630 */
    unsigned char               ucBit;

    if (0 < p_stGeo->xNbPoints) {
        p_stVert = (float *)p_stGeo->d_stListOfPoints;
        do {
            aDelta[0] = p_stPos[0] - p_stVert[0];
            aDelta[1] = p_stPos[1] - p_stVert[1];
            aDelta[2] = p_stPos[2] - p_stVert[2];
            dDist = (double)aDelta[0] * (double)aDelta[0];
            if (dDist < (double)fRadiusSq) {
                dDist = (double)aDelta[1] * (double)aDelta[1] + dDist;
                if (dDist < (double)fRadiusSq) {
                    dDist = (double)aDelta[2] * (double)aDelta[2] + dDist;
                    if (dDist < (double)fRadiusSq) {
                        if (p_stMod == (MGT_tdstMagnetModification *)0) {
                            if (*psModIdx == -1)
                                *psModIdx = MGT_pc_fn_sAllocModifiedObject(p_stGeo);
                            p_stMod = MGT_pc_fn_p_stFindOrAllocMagnetModification(sSubTag, *psModIdx, p_stGeo);
                            if (p_stMod != (MGT_tdstMagnetModification *)0) {
                                p_stMod->h_SuperObject = h_SuperObject;
                                p_stMod->bUsed = 1;
                                ucResult = 1;
                            }
                        }
                        if (p_stMod != (MGT_tdstMagnetModification *)0) {
                            ucBit = (unsigned char)(1 << (sPt & 7));
                            ((unsigned char *)p_stMod->d_cFieldBitsModifiedPoints)[sPt >> 3] |= ucBit;
                            ((unsigned char *)p_stMod->d_cFieldBitsInfluencedPoints)[sPt >> 3] |= ucBit;
                        }
                    }
                }
            }
            sPt++;
            p_stVert += 3;
        } while (sPt < p_stGeo->xNbPoints);
    }
    return ucResult;
}

/* 0x00441740 */
void __cdecl MGT_fn_vClearTargetModificationsInfluencedBits(int p_target, ACP_tdxBool bFlag)
{
    short sSlot = 0;
    do {
        MGT_tdstMagnetModification *p_stMod = fn_hMSMagnetGetMagnetModification((MS_tdxHandleToMSMagnet)p_target, sSlot);
        if (p_stMod != (MGT_tdstMagnetModification *)0)
            MGT_fn_vClearSubBitmapB(p_stMod);
        sSlot = (short)(sSlot + 1);
    } while (sSlot < 0x14);
}

/* 0x00441770 */
void __cdecl MGT_fn_vSetTargetModificationsUsed(int p_target, ACP_tdxBool bUsed)
{
    short sSlot = 0;
    do {
        MGT_tdstMagnetModification *p_stMod = fn_hMSMagnetGetMagnetModification((MS_tdxHandleToMSMagnet)p_target, sSlot);
        if (p_stMod != (MGT_tdstMagnetModification *)0)
            p_stMod->bUsed = bUsed;
        sSlot = (short)(sSlot + 1);
    } while (sSlot < 0x14);
}

/* 0x004417A0 */
unsigned char __cdecl MGT_pc_fn_ucRefreshMagnetModification(MGT_tdstMagnetModification *p_stMod)
{

    unsigned char                    ucRet;
    unsigned char                    ucRemoved;
    unsigned char                    ucFlag;
    int                              iBit;
    int                              iByte;
    short                            sPt = 0;
    MGT_tdstModifiedObject          *p_stObj;
    struct GEO_tdstGeometricObject_ *p_stGeo;
    char                            *p_cObjBits;
    unsigned long                   *pulDur;
    unsigned long                    ulDelta;
    unsigned long                    ulAmt;
    unsigned long                    ulVal;
    int                              iTarget;
    MTH3D_tdstVector                *p_stPosA;
    MTH3D_tdstVector                *p_stOut;
    float                           *pxOffset;
    MTH_tdxReal                      xFar;
    MTH_tdxReal                      xNear;
    MTH_tdxReal                      xStrength;
    MTH_tdxReal                      xFalloff;
    MTH_tdxReal                      xScale;
    MTH3D_tdstVector                 stDisp;
    POS_tdstCompletePosition         stInv;

    if (p_stMod == (MGT_tdstMagnetModification *)0 || MGT_fn_bIsSubBitmapANonZero(p_stMod) == 0)
        return 0;

    p_stObj    = g_aModifiedObjects[p_stMod->xIndexOfModifiedObject];
    ulDelta    = GAM_g_ulUsefulDeltaTime;
    p_cObjBits = p_stObj->d_cFieldBitsAlreadyInfluencedPoints;
    p_stGeo    = p_stObj->p_stMorphedObject;

    if (g_aActiveMagnets[p_stMod->xIndexOfActivatedMagnet] == (int *)0) {

#ifdef RAYMAN2_FIX_BUGS
        /* FIX(BUGS.md#3) */
        /* only set when a point is blended; start them as the active-magnet path does */
        ucRet = 0;
        ucRemoved = 0;
#endif /* RAYMAN2_FIX_BUGS */
        p_stPosA = p_stObj->d_stOriginalPosition;
        p_stOut  = p_stGeo->d_stListOfPoints;
        pulDur   = p_stMod->d_ulCurrentDuration;
        for (; sPt < p_stGeo->xNbPoints; sPt++) {
            iBit = sPt & 7;
            iByte = (int)sPt >> 3;
            if (((1 << iBit) & p_stMod->d_cFieldBitsModifiedPoints[iByte]) != 0
                && ((1 << iBit) & p_stMod->d_cFieldBitsInfluencedPoints[iByte]) == 0
                && *pulDur > 0) {
                ulAmt = *pulDur;
                if (ulAmt >= ulDelta)
                    ulAmt = ulDelta;
                *pulDur -= ulAmt;
                MGT_pc_fn_vBlendPointDisplacementByRatio((unsigned long)p_stMod, sPt, (int)p_cObjBits,
                                                         *pulDur, (int)ulDelta, p_stPosA, p_stOut);
                ucRet = 1;
                if (*pulDur == 0) {
                    ucRemoved = 1;
                    p_stMod->d_cFieldBitsModifiedPoints[iByte] &= ~(1 << iBit);
                }
            }
            p_stPosA++;
            p_stOut++;
            pulDur++;
        }
    } else {

        iTarget = (int)((struct tdstEngineObject_ *)((struct HIE_tdstSuperObject *)g_aActiveMagnets[p_stMod->xIndexOfActivatedMagnet])->hLinkedObject.p_Void)->h_MSMagnet;
        if (iTarget == 0)
            return 0;

        xFar = fn_xMSMagnetGetFar((void *)iTarget);
        xNear = fn_xMSMagnetGetNear((void *)iTarget);
        xStrength = fn_xMSMagnetGetStrength((void *)iTarget);
        xFalloff = xFar - xNear;

        if (fn_bMSMagnetIsNearFarModified((MS_tdxHandleToMSMagnet)iTarget) != 0
            || fn_bMSMagnetIsStrengthModified((void *)iTarget) != 0
            || fn_bMSMagnetIsPositionModified((void *)iTarget) != 0)
            ucFlag = 1;
        else
            ucFlag = 0;

        /* 0x441933 */
        if (MTH_M_bDifferentZero(xFalloff))
            xFalloff = 1.0f / xFalloff;
        else
            xFalloff = 1.0f;

        xScale = 1.0f / (MTH_tdxReal)((MS_tdxHandleToMSMagnet)iTarget)->ulDuration;

        POS_fn_vInvertMatrix(&stInv,
                             (POS_tdstCompletePosition *)p_stMod->h_SuperObject->hGlobalMatrix);
        pxOffset = (float *)fn_hMSMagnetGetPosition((MS_tdxHandleToMSMagnet)iTarget);
        POS_fn_vMulMatrixVertex(&stDisp, &stInv, (MTH3D_tdstVector *)pxOffset);

        pulDur = p_stMod->d_ulCurrentDuration;
        p_stGeo = p_stObj->p_stMorphedObject;
        ucRet = 0;
        p_stPosA = p_stObj->d_stOriginalPosition;
        p_stOut = p_stGeo->d_stListOfPoints;
        ucRemoved = 0;
        for (; sPt < p_stGeo->xNbPoints; sPt++) {
            iBit = sPt & 7;
            iByte = (int)sPt >> 3;
            if (((1 << iBit) & p_stMod->d_cFieldBitsModifiedPoints[iByte]) != 0) {
                ulVal = *pulDur;
                if (((1 << iBit) & p_stMod->d_cFieldBitsInfluencedPoints[iByte]) == 0) {
                    if (ulVal > 0) {
                        ulAmt = ulVal;
                        if (ulAmt >= ulDelta)
                            ulAmt = ulDelta;
                        *pulDur -= ulAmt;
                        MGT_pc_fn_vBlendPointDisplacementByRatio((unsigned long)p_stMod, sPt, (int)p_cObjBits,
                                                                 *pulDur, (int)ulDelta, p_stPosA, p_stOut);
                        ucRet = 1;
                        if (*pulDur == 0) {
                            ucRemoved = 1;
                            p_stMod->d_cFieldBitsModifiedPoints[iByte] &= ~(1 << iBit);
                        }
                    }
                } else if (ulVal < ((MS_tdxHandleToMSMagnet)iTarget)->ulDuration || ucFlag != 0) {
                    *pulDur = ulVal + ulDelta;
                    if (((MS_tdxHandleToMSMagnet)iTarget)->ulDuration < ulVal + ulDelta)
                        *pulDur = ((MS_tdxHandleToMSMagnet)iTarget)->ulDuration;
                    MGT_pc_fn_vAccumulatePointDisplacement((unsigned long)p_stMod, sPt, (int)p_cObjBits,
                                                           (unsigned int)*pulDur, (float *)p_stPosA,
                                                           (float *)p_stOut, pxOffset,
                                                           xNear, xStrength, xFalloff, xScale);
                    ucRet = 1;
                }
            }
            p_stPosA++;
            p_stOut++;
            pulDur++;
        }
    }

    if (ucRemoved != 0 && MGT_fn_bIsSubBitmapANonZero(p_stMod) == 0)
        MGT_pc_fn_vDestroyMagnetModification(p_stMod);
    return ucRet;
}

/* 0x00441B10 */
void __cdecl MGT_pc_fn_vAccumulatePointDisplacement(unsigned long ulUnused, short sPointIdx,
                                                   int pBitmap, unsigned int uCount,
                                                   float *p_xPosA, float *p_xOut, float *p_xPosB,
                                                   float xNearDist, float xFarDist,
                                                   float xFalloff, float xScale)
{
    float         fDist, fInvDist, fAmount;
    float         aDiff[3];
    float         fDirX, fDirY, fDirZ;
    float         aAmount[3];
    float         aTmp[3];
    unsigned char *pucByte;
    unsigned char  ucBit;

    fAmount = (float)uCount * xScale;
    MTH3D_M_vSubVector((MTH3D_tdstVector *)aDiff, (MTH3D_tdstVector *)p_xPosB, (MTH3D_tdstVector *)p_xPosA);

    fDist = MTH_M_xSqrt(aDiff[0] * aDiff[0] + aDiff[1] * aDiff[1] + aDiff[2] * aDiff[2]);
    fInvDist = 1.0f / fDist;
    if (xNearDist <= fDist) {
        fAmount = (1.0f - (fDist - xNearDist) * xFalloff) * xFarDist * fAmount;
    } else if (xFarDist <= fDist) {
        fAmount = fAmount * xFarDist;
    } else {
        fAmount = fAmount * fDist;
    }

    fDirX = fInvDist * aDiff[0];
    fDirY = fInvDist * aDiff[1];
    fDirZ = fInvDist * aDiff[2];
    aAmount[0] = fDirX * fAmount;
    aAmount[1] = fDirY * fAmount;
    aAmount[2] = fDirZ * fAmount;

    ucBit = (unsigned char)(sPointIdx & 7);
    pucByte = (unsigned char *)(((int)sPointIdx >> 3) + pBitmap);
    if (((int)(char)*pucByte & (1 << ucBit)) != 0) {
        MTH3D_M_vAddVector((MTH3D_tdstVector *)aTmp, (MTH3D_tdstVector *)p_xPosA, (MTH3D_tdstVector *)aAmount);
        MTH3D_M_vAddVector((MTH3D_tdstVector *)p_xOut, (MTH3D_tdstVector *)p_xOut, (MTH3D_tdstVector *)aTmp);
        p_xOut[0] = p_xOut[0] * 0.5f;
        p_xOut[1] = p_xOut[1] * 0.5f;
        p_xOut[2] = p_xOut[2] * 0.5f;
        return;
    }
    MTH3D_M_vAddVector((MTH3D_tdstVector *)p_xOut, (MTH3D_tdstVector *)p_xPosA, (MTH3D_tdstVector *)aAmount);
    *pucByte = (unsigned char)(*pucByte | (1 << ucBit));
}

/* 0x00441CD0 */
void __cdecl MGT_pc_fn_vBlendPointDisplacementByRatio(unsigned long ulUnused, short sPointIdx,
                                                     int pBitmap, unsigned int uCount,
                                                     int iCount2, MTH3D_tdstVector *p_stPosA,
                                                     MTH3D_tdstVector *p_stOut)
{

    MTH3D_tdstVector stDiff, stDelta, stTmp;
    MTH_tdxReal    xRatio;
    unsigned char *pucByte;
    int            iBit;

    MTH3D_M_vSubVector(&stDiff, p_stOut, p_stPosA);
    xRatio = (MTH_tdxReal)uCount / (MTH_tdxReal)(iCount2 + uCount);
    iBit = sPointIdx & 7;
    pucByte = (unsigned char *)(((int)sPointIdx >> 3) + pBitmap);
    MTH3D_M_vMulScalarVector(&stDelta, xRatio, &stDiff);
    if (((int)(char)*pucByte & (1 << iBit)) != 0) {
        MTH3D_M_vAddVector(&stTmp, p_stPosA, &stDelta);
        MTH3D_M_vAddVector(p_stOut, p_stOut, &stTmp);
        MTH3D_M_vMulScalarVector(p_stOut, 0.5f, p_stOut);
        return;
    }
    MTH3D_M_vAddVector(p_stOut, p_stPosA, &stDelta);
    *pucByte = (unsigned char)(*pucByte | (1 << iBit));
}

/* 0x00441E20 */
short __cdecl MGT_pc_fn_sAllocModifiedObject(void *p_stGeo)
{
    MGT_tdstModifiedObject *pObj;
    GEO_tdstGeometricObject *pbGeo;
    short                   sCount;
    short                   sIdx;
    MGT_tdstModifiedObject **pp_stSlot;

#define MGT_pc_M_xNbPointsOfGeo(pbGeo) ((pbGeo)->xNbPoints)
    pbGeo = (GEO_tdstGeometricObject *)p_stGeo;
    pObj = (MGT_tdstModifiedObject *)GLI_pc_fn_p_vAlloc(0x28);
    pObj->p_stMorphedObject = (struct GEO_tdstGeometricObject_ *)p_stGeo;
    pObj->aDEF_hMagnet[0] = (MGT_tdstMagnetModification *)0;
    pObj->aDEF_hMagnet[1] = (MGT_tdstMagnetModification *)0;
    pObj->aDEF_hMagnet[2] = (MGT_tdstMagnetModification *)0;
    pObj->aDEF_hMagnet[3] = (MGT_tdstMagnetModification *)0;
    pObj->aDEF_hMagnet[4] = (MGT_tdstMagnetModification *)0;

    pObj->d_cFieldBitsAlreadyInfluencedPoints = (char *)GLI_pc_fn_p_vAlloc(
        (unsigned int)((int)MGT_pc_M_xNbPointsOfGeo(pbGeo) >> 3) +
        (unsigned int)(((unsigned short)MGT_pc_M_xNbPointsOfGeo(pbGeo) & 7) != 0));
    MGT_pc_fn_vClearModifiedObjectBitmap(pObj);

    pObj->d_stOriginalPosition = (struct MTH3D_tdstVector_ *)GLI_pc_fn_p_vAlloc(
        MGT_pc_M_xNbPointsOfGeo(pbGeo) * 0x0C);
    memcpy(pObj->d_stOriginalPosition, pbGeo->d_stListOfPoints, MGT_pc_M_xNbPointsOfGeo(pbGeo) * 0x0C);
#undef MGT_pc_M_xNbPointsOfGeo

    pObj->p_stOctree = (struct COL_tdstOctree_ *)pbGeo->p_stOctree;
    pbGeo->p_stOctree = NULL;
    pObj->h_SuperObject = (struct HIE_tdstSuperObject *)0;

    sCount = g_lNbPendingMagnets;
    pp_stSlot = g_aModifiedObjects;
    for (sIdx = 0; sIdx < sCount; sIdx++, pp_stSlot++) {
        if (*pp_stSlot == (MGT_tdstModifiedObject *)0) {
            *pp_stSlot = pObj;
            return sIdx;
        }
    }
    if (sCount < 0x14) {
        sCount++;
        *pp_stSlot = pObj;
        g_lNbPendingMagnets = sCount;
        return sIdx;
    }
    return -1;
}

/* 0x00441F00 */
short __cdecl MGT_pc_fn_sFindModifiedObjectSlot(void *p_stGeo)
{
    short sIdx;
    MGT_tdstModifiedObject **ppObj;

    sIdx = 0;
    ppObj = g_aModifiedObjects;
    if (0 < g_lNbPendingMagnets) {
        do {
            if (*ppObj != (MGT_tdstModifiedObject *)0 &&
                *(void **)*ppObj == p_stGeo) {
                return sIdx;
            }
            sIdx++;
            ppObj++;
        } while (sIdx < g_lNbPendingMagnets);
    }
    return -1;
}

/* 0x00441F40 */
void __cdecl MGT_pc_fn_vLinkMagnetModification(MGT_tdstMagnetModification *p_stMod)
{
    MGT_tdstModifiedObject *p_stObj = g_aModifiedObjects[p_stMod->xIndexOfModifiedObject];
    short sIdx;

    for (sIdx = 0; sIdx < MGT_MAX_NUMBEROF_MAGNETS; sIdx++) {
        if (p_stObj->aDEF_hMagnet[sIdx] == 0) {
            p_stObj->aDEF_hMagnet[sIdx] = p_stMod;
            return;
        }
    }
}

/* 0x00441F80 */
void __cdecl MGT_pc_fn_vClearModifiedObjectBitmap(MGT_tdstModifiedObject *p_stObj)
{
#define MGT_pc_M_xNbPointsOfMorphed(p_stObj) ((p_stObj)->p_stMorphedObject->xNbPoints)
    memset(p_stObj->d_cFieldBitsAlreadyInfluencedPoints, 0,
           ((MGT_pc_M_xNbPointsOfMorphed(p_stObj) & 7) != 0) + (MGT_pc_M_xNbPointsOfMorphed(p_stObj) >> 3));
#undef MGT_pc_M_xNbPointsOfMorphed
}
