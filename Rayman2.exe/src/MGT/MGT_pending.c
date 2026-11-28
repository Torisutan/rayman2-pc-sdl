#include <stdint.h>

#include "MGT/MGTStruc.h"
#include "MGT/MGT_pending.h"
#include "MGT/MGT_globals.h"

#include "GLI/Memory.h"
#include "GEO/GeoObj.h"
#include "GAM/MSMag.h"

#define MGT_M_uiBitmapByteCount(xNbPoints) \
    ((unsigned int)(((unsigned short)(xNbPoints) & 7) != 0) + \
     (unsigned int)((int)(xNbPoints) >> 3))

void __cdecl MGT_pc_fn_vLinkMagnetModification(MGT_tdstMagnetModification *p_stMod);

/* 0x00441FC0 */
void __cdecl MGT_pc_fn_vUnlinkMagnetModification(MGT_tdstMagnetModification *p_stMod)
{
    MGT_tdstModifiedObject *p_stObj = g_aModifiedObjects[p_stMod->xIndexOfModifiedObject];
    short sIdx;

    for (sIdx = 0; sIdx < MGT_MAX_NUMBEROF_MAGNETS; sIdx++) {
        if (p_stObj->aDEF_hMagnet[sIdx] == p_stMod) {
            p_stObj->aDEF_hMagnet[sIdx] = 0;
            return;
        }
    }
}

/* 0x00441FF0 */
void __cdecl MGT_pc_fn_vDestroyModifiedObject(short sModuleIdx)
{
    MGT_tdstModifiedObject *p_stObj = g_aModifiedObjects[sModuleIdx];
    GEO_tdstGeometricObject *p_stGeo = p_stObj->p_stMorphedObject;

    GLI_pc_fn_vFree((void **)&p_stObj->d_cFieldBitsAlreadyInfluencedPoints);
    memcpy(p_stGeo->d_stListOfPoints, p_stObj->d_stOriginalPosition, p_stGeo->xNbPoints * 12);
    GLI_pc_fn_vFree((void **)&p_stObj->d_stOriginalPosition);
    p_stGeo->p_stOctree = p_stObj->p_stOctree;
    GEO_vEndModifyObject2(p_stGeo);
    g_aModifiedObjects[sModuleIdx] = 0;
    if (sModuleIdx == g_lNbPendingMagnets - 1)
        g_lNbPendingMagnets--;
    GLI_pc_fn_vFree((void **)&p_stObj);
}

#include "GAM/GAM_types.h"

void __cdecl MGT_pc_fn_vDestroyMagnetModification(MGT_tdstMagnetModification *p_stMod);

/* 0x00442090 */
void __cdecl MGT_fn_vDestroySubByTag(short sModuleIdx, short sSubTag)
{
    MGT_tdstModifiedObject      *p_stObj  = g_aModifiedObjects[sModuleIdx];
    MGT_tdstMagnetModification **p_pstSlot = p_stObj->aDEF_hMagnet;
    short                        sIdx     = 0;

    do {
        MGT_tdstMagnetModification *p_stMod = *p_pstSlot;
        if (p_stMod != (MGT_tdstMagnetModification *)0 &&
            p_stMod->xIndexOfActivatedMagnet == sSubTag) {
            MGT_pc_fn_vDestroyMagnetModification(*p_pstSlot);
            return;
        }
        sIdx++;
        p_pstSlot++;
    } while (sIdx < MGT_MAX_NUMBEROF_MAGNETS);
}

/* 0x004420D0 */
void __cdecl MGT_fn_vReleasePendingIfEmpty(short sModuleIdx)
{
    MGT_tdstModifiedObject *p_stObj = g_aModifiedObjects[sModuleIdx];
    short                   sIdx;

    for (sIdx = 0; sIdx < MGT_MAX_NUMBEROF_MAGNETS; sIdx++) {
        if (p_stObj->aDEF_hMagnet[sIdx] != (MGT_tdstMagnetModification *)0)
            break;
    }

    if (sIdx == MGT_MAX_NUMBEROF_MAGNETS)
        MGT_pc_fn_vDestroyModifiedObject(sModuleIdx);
}

MGT_tdstMagnetModification *__cdecl MGT_pc_fn_p_stAllocMagnetModification(short sActivatedMagnet, short sModuleIdx);

/* 0x00442110 */
MGT_tdstMagnetModification *__cdecl MGT_pc_fn_p_stFindOrAllocMagnetModification(short sSubTag, short sModuleIdx, void *p_stGeo)
{
    MGT_tdstModifiedObject *p_stObj   = g_aModifiedObjects[sModuleIdx];
    short                   sFreeSlot = 0x14;
    short                   sIdx      = 0;

    /* 0x00441630 */
    (void)p_stGeo;

    do {
        MGT_tdstMagnetModification *p_stMod = p_stObj->aDEF_hMagnet[sIdx];
        if (p_stMod != (MGT_tdstMagnetModification *)0) {
            if (p_stMod->xIndexOfActivatedMagnet == sSubTag)
                return p_stMod;
        } else {
            if (sFreeSlot == 0x14)
                sFreeSlot = sIdx;
        }
        sIdx++;
    } while (sIdx < MGT_MAX_NUMBEROF_MAGNETS);

    return MGT_pc_fn_p_stAllocMagnetModification(sSubTag, sModuleIdx);
}

/* 0x00442160 */
MGT_tdstMagnetModification *__cdecl MGT_pc_fn_p_stAllocMagnetModification(short sActivatedMagnet, short sModuleIdx)
{
    MGT_tdstModifiedObject *p_stObj = g_aModifiedObjects[sModuleIdx];
    int                     iMagnet = (int)M_GetMSHandle((struct HIE_tdstSuperObject *)g_aActiveMagnets[sActivatedMagnet], MSMagnet);
    MGT_tdstMagnetModification *p_stMod;
    uint32_t               *p_ulDur;

    p_stMod = (MGT_tdstMagnetModification *)GLI_pc_fn_p_vAlloc(0x18);
    p_stMod->xIndexOfActivatedMagnet = sActivatedMagnet;
    p_stMod->xIndexOfModifiedObject  = sModuleIdx;
    p_stMod->h_SuperObject = 0;
    p_stMod->xNbPoints = p_stObj->p_stMorphedObject->xNbPoints;
    MGT_pc_fn_vLinkMagnetModification(p_stMod);
    fn_vMSMagnetAddMagnetModification((void *)iMagnet, p_stMod);
    p_stMod->d_cFieldBitsInfluencedPoints =
        (char *)GLI_pc_fn_p_vAlloc(MGT_M_uiBitmapByteCount(p_stMod->xNbPoints));
    MGT_fn_vClearSubBitmapB(p_stMod);
    p_stMod->d_cFieldBitsModifiedPoints =
        (char *)GLI_pc_fn_p_vAlloc(MGT_M_uiBitmapByteCount(p_stMod->xNbPoints));
    MGT_fn_vClearSubBitmapA(p_stMod);
    p_ulDur = (uint32_t *)GLI_pc_fn_p_vAlloc((unsigned int)((int)p_stMod->xNbPoints << 2));
    p_stMod->d_ulCurrentDuration = (unsigned long *)p_ulDur;
    memset(p_ulDur, 0, (unsigned int)((int)p_stMod->xNbPoints << 2));
    return p_stMod;
}

/* 0x00442260 */
void __cdecl MGT_fn_vClearSubBitmapA(void *p_stSub)
{
    MGT_tdstMagnetModification *p_stMod = (MGT_tdstMagnetModification *)p_stSub;

    memset(p_stMod->d_cFieldBitsModifiedPoints, 0,
           ((p_stMod->xNbPoints & 7) != 0) + (p_stMod->xNbPoints >> 3));
}

/* 0x004422A0 */
void __cdecl MGT_fn_vClearSubBitmapB(void *p_stSub)
{
    MGT_tdstMagnetModification *p_stMod = (MGT_tdstMagnetModification *)p_stSub;

    memset(p_stMod->d_cFieldBitsInfluencedPoints, 0,
           ((p_stMod->xNbPoints & 7) != 0) + (p_stMod->xNbPoints >> 3));
}

/* 0x004422E0 */
void __cdecl MGT_pc_fn_vDestroyMagnetModification(MGT_tdstMagnetModification *p_stMod)
{
    GLI_pc_fn_vFree((void **)&p_stMod->d_cFieldBitsInfluencedPoints);
    GLI_pc_fn_vFree((void **)&p_stMod->d_cFieldBitsModifiedPoints);
    GLI_pc_fn_vFree((void **)&p_stMod->d_ulCurrentDuration);
    if (p_stMod->xIndexOfModifiedObject != -1)
        MGT_pc_fn_vUnlinkMagnetModification(p_stMod);
    if (p_stMod->xIndexOfActivatedMagnet != -1 &&
        g_aActiveMagnets[p_stMod->xIndexOfActivatedMagnet] != 0) {
        int iMagnet = (int)M_GetMSHandle((struct HIE_tdstSuperObject *)g_aActiveMagnets[p_stMod->xIndexOfActivatedMagnet], MSMagnet);
        if (iMagnet != 0)
            fn_vMSMagnetDeleteMagnetModification((void *)iMagnet, p_stMod);
    }
    GLI_pc_fn_vFree((void **)&p_stMod);
}

/* 0x00442370 */
unsigned char __cdecl MGT_fn_bIsSubBitmapANonZero(void *p_stSub)
{
    MGT_tdstMagnetModification *p_stMod = (MGT_tdstMagnetModification *)p_stSub;
    short          sByteCount = (short)MGT_M_uiBitmapByteCount(p_stMod->xNbPoints);
    short          sIdx       = 0;

    for (sIdx = 0; sIdx < sByteCount; sIdx++) {
        if (p_stMod->d_cFieldBitsModifiedPoints[sIdx] != 0)
            break;
    }

    return (unsigned char)(sIdx != sByteCount);
}
