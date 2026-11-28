#include "HIE/HIE_types.h"
#include "HIE/HieSpObj.h"
#include "POS/POS_types.h"
#include "MTH/MTH_flt.h"
#include "GEO/GEO_types.h"
#include "GLI/GlobalAlpha.h"
#include "GLI/light_fog.h"
#include "GLI/misc_trivial.h"
#include "GLI/Object.h"
#include "HIE/HIE_globals.h"
#include "PO/POProto.h"
#include "PO/IPOProto.h"
#include "GLI/VisuSet.h"
#include "GEO/GEO_globals.h"
#include "MMG/MMG_alloc.h"
#include "MMG/MMG_pubaccess.h"
#include "ERM/CpaError.h"
#include "GLD/GLD_Pub.h"
#include "GLI/Vpt3D.h"
#include "GLI/GLI_BigGlobals.h"
#include "SPO/HieDrw.h"
#include "GLI/GLI_Defn.h"

POS_tdstCompletePosition *g_p_stCurrentInCamCoordsMatrix;   /* 0x00514D64 */

/* 0x00514D60 */
extern MTH_tdxReal HiePick_VoidReal;

#define GAM_C_CustBitUsesTransparencyZone 0x00200000UL
#define GAM_C_CustBitOutOfVisibility      0x00000800UL

#define g_aActiveLights_DIM  300

extern void *GLI_gs_aDEF_p_CurrentListOfLight[g_aActiveLights_DIM];
#define g_aActiveLights     ((int *)GLI_gs_aDEF_p_CurrentListOfLight)

#define HIE_M_MustBeDisplayed(_lDrawMask) \
    (((_lDrawMask & GLI_C_lIsNotVisibleInRealWorld)     &&  (_lDrawMask & GLI_C_lIsNotDrawingInMirror)) || \
     ((_lDrawMask & GLI_C_lIsNotVisibleInSymetricWorld) && !(_lDrawMask & GLI_C_lIsNotDrawingInMirror)))

extern void *__cdecl fn_hGetNextAlwaysActiveCharNode(void *hCharNode);

extern void HIE_fn_vPopMatrix(void);
extern void HIE_fn_vPopOnCameraMatrixStack(void);

#define HIE_g_p_MmgPoolRangeLo (MMG_g_p_GmtPalBlockInfo->p_cBeginBlock)
#define HIE_g_p_MmgPoolRangeHi (MMG_g_p_GmtPalBlockInfo->p_cEndBlock)

typedef struct HIE_pc_tdstFrameEntry_
{
    short  sSubCount;
    short  _pad02;
    void  *pSubArray;
} HIE_pc_tdstFrameEntry;

typedef struct HIE_pc_tdstFrameTable_
{
    short                  sCount;
    short                  _pad02;
    HIE_pc_tdstFrameEntry *pEntries;
} HIE_pc_tdstFrameTable;

short *HIE_pc_fn_p_stCreateFrameTable(short sCount);

/* 0x0042B2D0 */
short *GLI_hComputeBlendRLI(void *hVS, long lShadowIdx, long lMirrorIdx,
                                           void *p_vIndex, float xBlend);

/* 0x0042a980 */
extern void __cdecl FUN_0042a980(unsigned long arg, unsigned long argExtra);
/* 0x00506b00 */
extern struct POS_tdstCompletePosition_ *GLI_g_p_stCurrentMatrix;

#include "GLI/VisuS_st.h"

struct HIE_pc_tdstStdGameView_
{
    unsigned char  _pad00[0x24];
    unsigned long  ulCustomBits;
    unsigned char  ucPlatFormType;
    unsigned char  ucMiscFlags;
};

struct tdstFogParams_
{
    MTH_tdxReal xDepthStart;
    MTH_tdxReal xDepthEnd;
    MTH_tdxReal xInfinite;
};

/* 0x0042b1d0 */
static void HIE_pc_fn_vRenderLODFrameAtDistance(void *_hVpt, GEO_tdstVisualSet *_hVS, float _xDistance,
                                                long _lDrawMask)
{
    int    iLastIndex;
    int    iIndex;
    float *pfThreshold;
    void  *hFrame;

    FUN_0042a980(0, 0);
    iLastIndex = _hVS->xNbLodDefinitions - 1;
    iIndex = 0;
    if (0 < iLastIndex) {
        pfThreshold = _hVS->d_xThresholdsTable;
        do {
            pfThreshold = pfThreshold + 1;
            if (_xDistance < *pfThreshold) {
                break;
            }
            iIndex = iIndex + 1;
        } while (iIndex < iLastIndex);
    }
    hFrame = ((void **)(_hVS->d_p_stLodDefinitions))[iIndex];
    if (hFrame != NULL) {
        GLI_xSendObjectToViewportWithLights((struct GLD_tdstViewportAttributes *)_hVpt, hFrame,
                                            _lDrawMask);
    }
}

/* 0x0042b230 */
static void HIE_pc_fn_vRenderLODByCenterDistance(void *_hVpt, GEO_tdstVisualSet *_hVS,
                                                 MTH3D_tdstVector *_p_stCenter, long _lDrawMask)
{
    MTH3D_tdstVector stVector;

    POS_fn_vMulMatrixVertex(&stVector, GLI_g_p_stCurrentMatrix, _p_stCenter);
    POS_fn_vMulMatrixVertex(&stVector,
                            &((GLI_tdstSpecificAttributesFor3D *)
                              ((struct GLD_tdstViewportAttributes_ *)_hVpt)->p_vSpecificToXD)->p_stCam->stMatrix,
                            &stVector);
    HIE_pc_fn_vRenderLODFrameAtDistance(_hVpt, _hVS,
        MTH_M_xSqrt(stVector.xX * stVector.xX + stVector.xY * stVector.xY + stVector.xZ * stVector.xZ),
        _lDrawMask);
}

/* 0x00430E30 */
unsigned long *HIE_pc_fn_pulSetFrameTableEntry(void *p_stTable, short sIndex, long lCount)
{
    HIE_pc_tdstFrameEntry *pstEntry;
    short          sCount;
    unsigned long  ulCount;
    unsigned long  ulBytes;
    unsigned long *pulResult;
    unsigned long *pulWrite;
    unsigned short *pusWrite;
    long           lDwords;

    pstEntry = ((HIE_pc_tdstFrameTable *)p_stTable)->pEntries + sIndex;
    sCount = (short)lCount;
    pstEntry->sSubCount = sCount;
    ulCount = (unsigned long)(long)sCount;
    ulBytes = ulCount * 8;
    pulResult = (unsigned long *)lCount;

    if (ulBytes != 0)
    {
        if (GEO_g_bDynamicAllocation != 2) {
            Mmg_fn_v_SetModeAlloc(g_ucGEOModuleId, g_ucGEOMMemMallocMode, 0);
            pulResult = (unsigned long *)fn_p_vGenAlloc(ulBytes, 0);
        }
        if (GEO_g_bDynamicAllocation == 2) {
dynamic_alloc:
            ERM_fn_vClearLastError(0);
            if (GEO_g_bDynamicAllocation == 1) {
                ERM_fn_vUpdateLastError(4, g_ucGEOModuleId, 0, 8, -1, 0xff, 0xff, NULL);
                ERM_fn_vClearLastError(0);
                GEO_g_bDynamicAllocation = 2;
            }
            Mmg_fn_v_SetModeAlloc(g_ucGEOModuleId, 0xff, 0);
            pulResult = (unsigned long *)fn_p_vGenAlloc(ulBytes, 0);
check_null:
            if (pulResult == NULL) {
                ERM_fn_vClearLastError(0);
                ERM_fn_vUpdateLastError(4, g_ucGEOModuleId, 0, 1, -1, 0xff, 0xff, NULL);
            }
        } else {
            if (GEO_g_bDynamicAllocation != 1) goto check_null;
            if (pulResult == NULL) goto dynamic_alloc;
        }

        pulWrite = pulResult;
        for (lDwords = (long)((ulCount & 0x1fffffffUL) << 1); lDwords != 0; lDwords--) {
            *pulWrite = 0;
            pulWrite++;
        }
    }

    pstEntry->pSubArray = pulResult;
    if (0 < sCount) {
        pusWrite = (unsigned short *)pulResult;
        do {
            pusWrite[0] = 0;
            pusWrite[1] = 0;
            pusWrite[2] = 0;
            pusWrite[3] = 0;
            pusWrite += 4;
            ulCount--;
        } while (ulCount != 0);
    }
    return pulResult;
}

/* 0x00430F90 */
short *HIE_pc_fn_p_stCreateFrameTable(short sCount)
{
    short        *psTable;
    unsigned long ulCount;
    unsigned long ulBytes;
    long          i;
    unsigned long *pulWord;
    unsigned char *pucByte;
    char         *pcEntry;

    psTable = NULL;
    if (GEO_g_bDynamicAllocation != 2) {
        Mmg_fn_v_SetModeAlloc(g_ucGEOModuleId, g_ucGEOMMemMallocMode, 0);
        psTable = (short *)fn_p_vGenAlloc(8, 0);
    }
    if (GEO_g_bDynamicAllocation == 2) {
fallback_header:
        ERM_fn_vClearLastError(0);
        if (GEO_g_bDynamicAllocation == 1) {
            ERM_fn_vUpdateLastError(4, g_ucGEOModuleId, 0, 8, -1, 0xff, 0xff, NULL);
            ERM_fn_vClearLastError(0);
            GEO_g_bDynamicAllocation = 2;
        }
        Mmg_fn_v_SetModeAlloc(g_ucGEOModuleId, 0xff, 0);
        psTable = (short *)fn_p_vGenAlloc(8, 0);
    } else if (GEO_g_bDynamicAllocation == 1) {
        if (psTable != NULL) goto fill_header;
        goto fallback_header;
    }
    if (psTable == NULL) {
        ERM_fn_vClearLastError(0);
        ERM_fn_vUpdateLastError(4, g_ucGEOModuleId, 0, 1, -1, 0xff, 0xff, NULL);
    }

fill_header:
    psTable[0] = 0;
    psTable[1] = 0;
    psTable[2] = 0;
    psTable[3] = 0;
    ulCount = (unsigned long)(long)sCount;
    psTable[0] = sCount;
    ulBytes = ulCount * 8;
    if (ulBytes == 0) goto entries_done;

    if (GEO_g_bDynamicAllocation != 2) {
        Mmg_fn_v_SetModeAlloc(g_ucGEOModuleId, g_ucGEOMMemMallocMode, 0);
        ((HIE_pc_tdstFrameTable *)psTable)->pEntries = fn_p_vGenAlloc(ulBytes, 0);
    }
    if (GEO_g_bDynamicAllocation == 2) {
fallback_entries:
        ERM_fn_vClearLastError(0);
        if (GEO_g_bDynamicAllocation == 1) {
            ERM_fn_vUpdateLastError(4, g_ucGEOModuleId, 0, 8, -1, 0xff, 0xff, NULL);
            ERM_fn_vClearLastError(0);
            GEO_g_bDynamicAllocation = 2;
        }
        Mmg_fn_v_SetModeAlloc(g_ucGEOModuleId, 0xff, 0);
        ((HIE_pc_tdstFrameTable *)psTable)->pEntries = fn_p_vGenAlloc(ulBytes, 0);
    } else if (GEO_g_bDynamicAllocation == 1) {
        if (((HIE_pc_tdstFrameTable *)psTable)->pEntries != NULL) goto zero_entries;
        goto fallback_entries;
    }
    if (((HIE_pc_tdstFrameTable *)psTable)->pEntries == NULL) {
        ERM_fn_vClearLastError(0);
        ERM_fn_vUpdateLastError(4, g_ucGEOModuleId, 0, 1, -1, 0xff, 0xff, NULL);
    }

zero_entries:
    pulWord = (unsigned long *)((HIE_pc_tdstFrameTable *)psTable)->pEntries;
    for (i = (long)(ulBytes >> 2); i != 0; i--) {
        *pulWord++ = 0;
    }
    pucByte = (unsigned char *)pulWord;
    for (i = (long)(ulBytes & 3); i != 0; i--) {
        *pucByte++ = 0;
    }

entries_done:
    if (0 < sCount) {
        pcEntry = (char *)((HIE_pc_tdstFrameTable *)psTable)->pEntries;
        i = 0;
        do {
            ulCount = ulCount - 1;
            *(short *)(pcEntry + i) = 0;
            *(void **)(pcEntry + i + 4) = NULL;
            i = i + 8;
        } while (ulCount != 0);
    }
    return psTable;
}

/* 0x004311B0 */
void HIE_pc_fn_vFreeFrameTable(HIE_pc_tdstFrameTable *psTable)
{
    short  sIndex;
    long   lOffset;
    void  *pEntry;

    sIndex = 0;
    if (0 < psTable->sCount) {
        do {
            lOffset = (long)sIndex * 8;
            pEntry = ((HIE_pc_tdstFrameEntry *)((char *)psTable->pEntries + lOffset))->pSubArray;
            if (pEntry != NULL) {
                Mmg_fn_v_SetModeAlloc(g_ucGEOModuleId, g_ucGEOMMemMallocMode, 0);
                if (GEO_g_bDynamicAllocation != '\0') {
                    Mmg_fn_vGiveInformationBlock(g_ucGEOModuleId, g_ucGEOMMemMallocMode,
                                                 (void *)MMG_g_p_GmtPalBlockInfo);
                    if (pEntry < (void *)HIE_g_p_MmgPoolRangeLo ||
                        pEntry > (void *)HIE_g_p_MmgPoolRangeHi) {
                        Mmg_fn_v_SetModeAlloc(g_ucGEOModuleId, 0xff, 0);
                    }
                }
                fn_vGenFree(pEntry, 0);
                if (Erm_fn_uwCheckError(MMG_g_ucErmChannelId, 0) != 0) {
                    ERM_fn_vClearLastError(0);
                    ERM_fn_vUpdateLastError(4, g_ucGEOModuleId, 0, 5, -1, 0xff, 0xff, NULL);
                }
                ((HIE_pc_tdstFrameEntry *)((char *)psTable->pEntries + lOffset))->pSubArray = NULL;
            }
            sIndex = (short)(sIndex + 1);
        } while (sIndex < psTable->sCount);
    }

    Mmg_fn_v_SetModeAlloc(g_ucGEOModuleId, g_ucGEOMMemMallocMode, 0);
    if (GEO_g_bDynamicAllocation != '\0') {
        Mmg_fn_vGiveInformationBlock(g_ucGEOModuleId, g_ucGEOMMemMallocMode,
                                     (void *)MMG_g_p_GmtPalBlockInfo);
        if ((void *)psTable->pEntries < (void *)HIE_g_p_MmgPoolRangeLo ||
            (void *)psTable->pEntries > (void *)HIE_g_p_MmgPoolRangeHi) {
            Mmg_fn_v_SetModeAlloc(g_ucGEOModuleId, 0xff, 0);
        }
    }
    fn_vGenFree(psTable->pEntries, 0);
    if (Erm_fn_uwCheckError(MMG_g_ucErmChannelId, 0) != 0) {
        ERM_fn_vClearLastError(0);
        ERM_fn_vUpdateLastError(4, g_ucGEOModuleId, 0, 5, -1, 0xff, 0xff, NULL);
    }
    psTable->pEntries = NULL;

    Mmg_fn_v_SetModeAlloc(g_ucGEOModuleId, g_ucGEOMMemMallocMode, 0);
    if (GEO_g_bDynamicAllocation != '\0') {
        Mmg_fn_vGiveInformationBlock(g_ucGEOModuleId, g_ucGEOMMemMallocMode,
                                     (void *)MMG_g_p_GmtPalBlockInfo);
        if ((void *)psTable < (void *)HIE_g_p_MmgPoolRangeLo ||
            (void *)psTable > (void *)HIE_g_p_MmgPoolRangeHi) {
            Mmg_fn_v_SetModeAlloc(g_ucGEOModuleId, 0xff, 0);
        }
    }
    fn_vGenFree(psTable, 0);
    if (Erm_fn_uwCheckError(MMG_g_ucErmChannelId, 0) != 0) {
        ERM_fn_vClearLastError(0);
        ERM_fn_vUpdateLastError(4, g_ucGEOModuleId, 0, 5, -1, 0xff, 0xff, NULL);
    }
}

/* 0x00431400 */
void HIE_pc_fn_vLoadScaledShortRecords(float *pfDst, const short *psSrc, long lCount)
{
    float       *pfEnd;
    float       *pf;
    const short *ps;

    pfEnd = pfDst + lCount * 4;
    pf    = pfDst;
    ps    = psSrc;
    if (pf < pfEnd)
    {
        do
        {
            *pf++ = (float)*ps++ * 0.00390625f;
            *pf++ = (float)*ps++ * 0.00390625f;
            *pf++ = (float)*ps++ * 0.00390625f;
            *pf++ = (float)*ps++ * 0.00390625f;
        } while (pf < pfEnd);
    }
}

/* 0x00431490 */
void HIE_pc_fn_vStoreScaledShortRecords(short *psDst, const float *pfSrc, long lCount)
{
    long lX;
    long lY;
    long lZ;
    long lW;
    long lLeft;

    lLeft = lCount;
    if (lLeft != 0)
    {
        do
        {
            lX = (long)(pfSrc[0] * 256.0f);
            lY = (long)(pfSrc[1] * 256.0f);
            lZ = (long)(pfSrc[2] * 256.0f);
            lW = (long)(pfSrc[3] * 256.0f);
            if (lX < -0x8000) lX = -0x8000; else if (lX > 0x7fff) lX = 0x7fff;
            if (lY < -0x8000) lY = -0x8000; else if (lY > 0x7fff) lY = 0x7fff;
            if (lZ < -0x8000) lZ = -0x8000; else if (lZ > 0x7fff) lZ = 0x7fff;
            if (lW < -0x8000) lW = -0x8000; else if (lW > 0x7fff) lW = 0x7fff;
            psDst[0] = (short)lX;
            psDst[1] = (short)lY;
            psDst[2] = (short)lZ;
            psDst[3] = (short)lW;
            psDst += 4;
            pfSrc += 4;
            lLeft--;
        } while (lLeft != 0);
    }
}

/* 0x004315a0 */
static short HIE_pc_fn_sGetFrameTableCount(short *p_sTable)
{
    return *p_sTable;
}

/* 0x004315B0 */
short __cdecl HIE_pc_fn_sGetFrameTableEntryCount(void *p_stTable, short sIndex)
{
    return ((HIE_pc_tdstFrameTable *)p_stTable)->pEntries[sIndex].sSubCount;
}

/* 0x004315d0 */
unsigned long HIE_pc_fn_ulGetFrameTableEntry(void *p_stTable, short sIndex)
{
    return *(unsigned long *)&((HIE_pc_tdstFrameTable *)p_stTable)->pEntries[sIndex].pSubArray;
}

/* 0x004315F0 */
short *ISI_hComputeBlendRLI(short *psSrcA, short *psSrcB, void *p_vIndex, float xBlend)
{
    short         *psDst;
    char          *pDstArr;
    char          *pSrcArrA;
    char          *pSrcArrB;
    short         *psA;
    short         *psB;
    short         *psD;
    void          *pBlock;
    long           lOff;
    short          sCount;
    unsigned long  ulBytes;
    long           i;
    unsigned long *pulWord;
    unsigned char *pucByte;
    float          xOneMinus;

    psDst = HIE_pc_fn_p_stCreateFrameTable(psSrcA[0]);
    lOff = (long)((short)(long)p_vIndex) * 8;
    pDstArr  = (char *)((HIE_pc_tdstFrameTable *)psDst)->pEntries;
    pSrcArrA = (char *)((HIE_pc_tdstFrameTable *)psSrcA)->pEntries;
    pSrcArrB = (char *)((HIE_pc_tdstFrameTable *)psSrcB)->pEntries;

    sCount = *(short *)(pSrcArrA + lOff);
    *(short *)(pDstArr + lOff) = sCount;
    ulBytes = (unsigned long)((long)sCount * 8);
    pBlock = p_vIndex;

    if (ulBytes != 0) {
        if (GEO_g_bDynamicAllocation != 2) {
            Mmg_fn_v_SetModeAlloc(g_ucGEOModuleId, g_ucGEOMMemMallocMode, 0);
            pBlock = fn_p_vGenAlloc(ulBytes, 0);
        }
        if (GEO_g_bDynamicAllocation == 2) {
fallback_blend:
            ERM_fn_vClearLastError(0);
            if (GEO_g_bDynamicAllocation == 1) {
                ERM_fn_vUpdateLastError(4, g_ucGEOModuleId, 0, 8, -1, 0xff, 0xff, NULL);
                ERM_fn_vClearLastError(0);
                GEO_g_bDynamicAllocation = 2;
            }
            Mmg_fn_v_SetModeAlloc(g_ucGEOModuleId, 0xff, 0);
            pBlock = fn_p_vGenAlloc(ulBytes, 0);
        } else if (GEO_g_bDynamicAllocation == 1) {
            if (pBlock != NULL) goto zero_blend;
            goto fallback_blend;
        }
        if (pBlock == NULL) {
            ERM_fn_vClearLastError(0);
            ERM_fn_vUpdateLastError(4, g_ucGEOModuleId, 0, 1, -1, 0xff, 0xff, NULL);
        }

zero_blend:
        pulWord = (unsigned long *)pBlock;
        for (i = (long)(ulBytes >> 2); i != 0; i--) {
            *pulWord++ = 0;
        }
        pucByte = (unsigned char *)pulWord;
        for (i = (long)(ulBytes & 3); i != 0; i--) {
            *pucByte++ = 0;
        }
    }

    *(void **)(pDstArr + lOff + 4) = pBlock;
    psA = *(short **)(pSrcArrA + lOff + 4);
    psB = *(short **)(pSrcArrB + lOff + 4);
    psD = (short *)pBlock;
    xOneMinus = 1.0f - xBlend;

    if (sCount > 0) {
        for (i = (long)sCount; i != 0; i--) {
            psD[0] = (short)((float)psB[0] * xBlend + (float)psA[0] * xOneMinus);
            psD[1] = (short)((float)psB[1] * xBlend + (float)psA[1] * xOneMinus);
            psD[2] = (short)((float)psB[2] * xBlend + (float)psA[2] * xOneMinus);
            psD[3] = (short)((float)psB[3] * xBlend + (float)psA[3] * xOneMinus);
            psA = psA + 4;
            psB = psB + 4;
            psD = psD + 4;
        }
    }
    return psDst;
}

/* 0x0042af70 */
static void HIE_pc_fn_vRenderLODFrame(void *_hVpt, GEO_tdstVisualSet *_hVS, MTH3D_tdstVector *_p_stCenter,
                                      long _lDrawMask, void *_pFrameTable)
{
    float aVector[3];
    float fDistance;
    short sIndex;
    short sCount;
    int   iLast;
    int   iIdx;
    GLI_tdstCamera *pCamera;
    void *hFrame;

    FUN_0042a980(0, 0);
    if (_hVS->xNbLodDefinitions != 1 ||
        (GLI_BIG_GLOBALS->xFogIsOn != 0 &&
         GLI_BIG_GLOBALS->p_stActiveFog->xInfinite < 50000.0f))
    {
        POS_fn_vMulMatrixVertex((MTH3D_tdstVector *)aVector, GLI_g_p_stCurrentMatrix,
                                _p_stCenter);
        pCamera = ((GLI_tdstSpecificAttributesFor3D *)
                   ((struct GLD_tdstViewportAttributes_ *)_hVpt)->p_vSpecificToXD)->p_stCam;
        POS_fn_vMulMatrixVertex((MTH3D_tdstVector *)aVector,
                                &pCamera->stMatrix,
                                (MTH3D_tdstVector *)aVector);
        fDistance = MTH_M_xSqrt(aVector[2] * aVector[2] + aVector[0] * aVector[0] + aVector[1] * aVector[1]);
        if (GLI_BIG_GLOBALS->xFogIsOn != 0 &&
            GLI_BIG_GLOBALS->p_stActiveFog->xInfinite <= fDistance)
        {
            return;
        }
        iLast = _hVS->xNbLodDefinitions - 1;
        sIndex = 0;
        if (0 < iLast) {
            iIdx = 0;
            do {
                if (fDistance < _hVS->d_xThresholdsTable[iIdx + 1]) {
                    break;
                }
                sIndex = sIndex + 1;
                iIdx = (int)sIndex;
            } while (iIdx < iLast);
        }
    }
    else {
        sIndex = 0;
    }
    if (_pFrameTable != NULL) {
        sCount = HIE_pc_fn_sGetFrameTableCount((short *)_pFrameTable);
        if (sIndex <= sCount) {
            FUN_0042a980(HIE_pc_fn_ulGetFrameTableEntry(_pFrameTable, sIndex), 0);
        }
    }
    hFrame = ((void **)(_hVS->d_p_stLodDefinitions))[sIndex];
    if (hFrame != NULL) {
        GLI_xSendObjectToViewportWithLights((struct GLD_tdstViewportAttributes *)_hVpt, hFrame,
                                            _lDrawMask);
    }
}

/* 0x0042b0c0 */
static void HIE_pc_fn_vRenderLODByRadius(void *_hVpt, GEO_tdstVisualSet *_hVS, MTH3D_tdstVector *_p_stCenter,
                                         float _fRadius, long _lDrawMask)
{
    float aVector[3];
    float fDistance;
    int   iLast;
    int   iIdx;
    GLI_tdstCamera *pCamera;
    void *hFrame;

    FUN_0042a980(0, 0);
    if (_hVS->xNbLodDefinitions != 1 ||
        (GLI_BIG_GLOBALS->xFogIsOn != 0 &&
         GLI_BIG_GLOBALS->p_stActiveFog->xInfinite < 50000.0f))
    {
        POS_fn_vMulMatrixVertex((MTH3D_tdstVector *)aVector, GLI_g_p_stCurrentMatrix,
                                _p_stCenter);
        pCamera = ((GLI_tdstSpecificAttributesFor3D *)
                   ((struct GLD_tdstViewportAttributes_ *)_hVpt)->p_vSpecificToXD)->p_stCam;
        POS_fn_vMulMatrixVertex((MTH3D_tdstVector *)aVector,
                                &pCamera->stMatrix,
                                (MTH3D_tdstVector *)aVector);
        fDistance = MTH_M_xSqrt(aVector[2] * aVector[2] + aVector[0] * aVector[0] + aVector[1] * aVector[1]);
        if (GLI_BIG_GLOBALS->xFogIsOn != 0 &&
            GLI_BIG_GLOBALS->p_stActiveFog->xInfinite <= fDistance)
        {
            return;
        }
        iLast = _hVS->xNbLodDefinitions - 1;
        iIdx = 0;
        if (0 < iLast) {
            do {
                if (fDistance < _hVS->d_xThresholdsTable[iIdx + 1]) {
                    break;
                }
                iIdx = iIdx + 1;
            } while (iIdx < iLast);
        }
    }
    else {
        iIdx = 0;
    }
    hFrame = ((void **)(_hVS->d_p_stLodDefinitions))[iIdx];
    if (hFrame != NULL) {
        GLI_xSendObjectToViewportWithLights((struct GLD_tdstViewportAttributes *)_hVpt, hFrame,
                                            _lDrawMask);
    }
}

void HIE_fn_vSendSuperObjectToViewport(void *_hVpt,
                                             HIE_tdstSuperObject *_hSprObj,
                                             long _lDrawMask)
{
    float fGlobalAlphaTemp;
    float fCullingTransparency;
    unsigned long ulType;
    GEO_tdxHandleToBoundingSphere hBV;
    MTH3D_tdstVector stCenter;
    float fRadius;
    void *hVS;
    void *hPO;
    void *hFrame;
    void *hMirrorFrame;
    int iLODCount;

    GLI_vSetLensFlareRef(_hSprObj);
    fGlobalAlphaTemp = GLI_fn_xGetGlobalAlpha();
    fCullingTransparency = GEO_xGetZFarTransparencyLevel();
    GLI_vSetGlobalAlpha(MTH_M_xMul(_hSprObj->fTransparenceLevel, fCullingTransparency));

    ulType = _hSprObj->ulTypeOfLinkedObject;

    if (ulType == HIE_C_ulPO) {
        hBV = (GEO_tdxHandleToBoundingSphere)PO_fn_hGetBoundingVolume(_hSprObj->hLinkedObject.p_Void);
        MTH3D_M_vCopyVector(&stCenter, GEO_fn_pGetCenterPointOfBoundingSphere(hBV));
        fRadius = GEO_fn_xGetRadiusOfBoundingSphere(hBV);
        hVS = PO_fn_hGetVisualSet(_hSprObj->hLinkedObject.p_Void);
        iLODCount = GLI_lGetVisualSetNumbertOfLOD(hVS);
        if (HIE_g_ucRenderMode == 0 && iLODCount == 1) {
            hFrame = (void *)GLI_hGetRLI(hVS, 0);
            HIE_pc_fn_vRenderLODFrame(_hVpt, hVS, &stCenter, _lDrawMask, hFrame);
            GLI_vSetGlobalAlpha(fGlobalAlphaTemp); return;
        }
        if (HIE_g_ucRenderMode == 1) {
            hFrame = (void *)GLI_hGetRLI(hVS, (int)HIE_g_ucShadowIdx);
            HIE_pc_fn_vRenderLODFrame(_hVpt, hVS, &stCenter, _lDrawMask, hFrame);
            GLI_vSetGlobalAlpha(fGlobalAlphaTemp); return;
        }
        if (HIE_g_ucRenderMode == 2) {
            hMirrorFrame = GLI_hComputeBlendRLI(hVS, (int)HIE_g_ucShadowIdx,
                                                               (int)HIE_g_ucMirrorIdx, NULL,
                                                               HIE_g_xMirrorBlend);
            if (hMirrorFrame != NULL) {
                HIE_pc_fn_vRenderLODFrame(_hVpt, hVS, &stCenter, _lDrawMask, hMirrorFrame);
                HIE_pc_fn_vFreeFrameTable((HIE_pc_tdstFrameTable *)hMirrorFrame);
                GLI_vSetGlobalAlpha(fGlobalAlphaTemp); return;
            }
            if (hBV != NULL) HIE_pc_fn_vRenderLODByRadius(_hVpt, hVS, &stCenter, fRadius, _lDrawMask);
            else              HIE_pc_fn_vRenderLODByCenterDistance(_hVpt, hVS, &stCenter, _lDrawMask);
            GLI_vSetGlobalAlpha(fGlobalAlphaTemp); return;
        }

        if (hBV != NULL) HIE_pc_fn_vRenderLODByRadius(_hVpt, hVS, &stCenter, fRadius, _lDrawMask);
        else              HIE_pc_fn_vRenderLODByCenterDistance(_hVpt, hVS, &stCenter, _lDrawMask);
        GLI_vSetGlobalAlpha(fGlobalAlphaTemp); return;
    }

    if (ulType == HIE_C_ulIPO) {
        hPO = IPO_fn_hGetPhysicalObject(_hSprObj->hLinkedObject.p_Void);
        hBV = (GEO_tdxHandleToBoundingSphere)PO_fn_hGetBoundingVolume(hPO);
        MTH3D_M_vCopyVector(&stCenter, GEO_fn_pGetCenterPointOfBoundingSphere(hBV));
        hFrame = fn_hGetNextAlwaysActiveCharNode(_hSprObj->hLinkedObject.p_Void);
        hVS = PO_fn_hGetVisualSet(hPO);
        HIE_pc_fn_vRenderLODFrame(_hVpt, hVS, &stCenter, _lDrawMask, hFrame);
        GLI_vSetGlobalAlpha(fGlobalAlphaTemp); return;
    }

    if (ulType == HIE_C_ulIPO_Mirror) {
        if (DAT_005115c9 != '\0') {
            hPO = IPO_fn_hGetPhysicalObject(_hSprObj->hLinkedObject.p_Void);
            hBV = (GEO_tdxHandleToBoundingSphere)PO_fn_hGetBoundingVolume(hPO);
            MTH3D_M_vCopyVector(&stCenter, GEO_fn_pGetCenterPointOfBoundingSphere(hBV));
            hFrame = fn_hGetNextAlwaysActiveCharNode(_hSprObj->hLinkedObject.p_Void);
            FUN_00422000(1);
            hVS = PO_fn_hGetVisualSet(hPO);
            HIE_pc_fn_vRenderLODFrame(_hVpt, hVS, &stCenter, _lDrawMask, hFrame);
            FUN_00422000(0);
            GLI_vSetGlobalAlpha(fGlobalAlphaTemp); return;
        }
    } else if (ulType & 0x480) {

        GLI_xSendObjectToViewportWithLights(_hVpt, _hSprObj->hLinkedObject.p_Void, _lDrawMask);
    }

    GLI_vSetGlobalAlpha(fGlobalAlphaTemp);
}

/* 0x00416DB0 */
extern void *__cdecl HIE_fn_hGetSuperObjectLight(HIE_tdstSuperObject *_hSprObj);
extern void __cdecl GLI_xSetViewportLightsOnOff(unsigned char bOnOff);
extern void __cdecl GLI_vPopLightsInViewport(struct GLD_tdstViewportAttributes_ *p_stVpt,
                                             unsigned long lNbLightsToPop);

#pragma optimize("y", off)
void HIE_fn_vSendStaticWorldToViewport(void *_hVpt,
                                        HIE_tdstSuperObject *_hSprObj,
                                        long _DrawMask,
                                        long _lCullingResult)
{
    ACP_tdxIndex i;
    HIE_tdstSuperObject *hChild;
    long lCullingResult;
    GEO_tdxHandleToParallelBox hParallelBox;
    MTH3D_tdstVector *p_stBdvMinPoint, *p_stBdvMaxPoint;
    MTH3D_tdstVector stMinPoint, stMaxPoint;

    _DrawMask &= HIE_fn_lGetSuperObjectDrawMask(_hSprObj);

    HIE_fn_vPushMatrix(_hSprObj);
    HIE_fn_vPushOnCameraMatrixStack(_hSprObj);

    if (_lCullingResult != GEO_C_lCullingOut)
    {

        if (HIE_M_MustBeDisplayed(_DrawMask) && !HIE_fn_SO_bIsHidden( _hSprObj ))
        {

            HIE_fn_vSendSuperObjectToViewport(_hVpt, _hSprObj, _DrawMask);
        }
    }

    HIE_M_ForEachChildOf(_hSprObj, hChild, i)
    {
        hParallelBox = (GEO_tdxHandleToParallelBox) HIE_fn_hGetSuperObjectBoundingVolume(hChild);
        if ( hParallelBox )
        {
            POS_tdstCompletePosition *hMatrix;

            p_stBdvMinPoint = GEO_fn_pGetMinPointOfParallelBox(hParallelBox);
            p_stBdvMaxPoint = GEO_fn_pGetMaxPointOfParallelBox(hParallelBox);

            hMatrix = HIE_fn_hGetSuperObjectGlobalMatrix(hChild);
            if(!POS_fn_ulIsIdentityMatrix(hMatrix))
            {
                MTH3D_tdstVector *p_stTransVec;

                p_stTransVec = POS_fn_vGetTranslationVectorReadOnly(hMatrix);
                MTH3D_M_vAddVector(&stMinPoint, p_stTransVec, p_stBdvMinPoint);
                MTH3D_M_vAddVector(&stMaxPoint, p_stTransVec, p_stBdvMaxPoint);
                p_stBdvMinPoint = &stMinPoint;
                p_stBdvMaxPoint = &stMaxPoint;
            }

            lCullingResult = GEO_lCullingBox(_hVpt, p_stBdvMinPoint, p_stBdvMaxPoint, _lCullingResult);
        }
        else
        {
            lCullingResult = GEO_C_lCullingIntersect;
        }
        if ( (lCullingResult != GEO_C_lCullingOut) || HIE_fn_SO_bCheckChildren(hChild) )
        {
            HIE_fn_vSendStaticWorldToViewport(_hVpt,
                hChild,
                _DrawMask,
                lCullingResult);
        }
    }

    HIE_fn_vPopMatrix();
    HIE_fn_vPopOnCameraMatrixStack();
}
#pragma optimize("", on)

/* 0x00413700 */
extern unsigned long __cdecl fn_ulStandardGameGetCustomBitsSO(void *hSupObj);

/* 0x00413770 */
extern unsigned char __cdecl fn_ucStandardGameGetTransparencyZoneMaxSO(struct HIE_tdstSuperObject *hSupObj);
extern unsigned char __cdecl fn_ucStandardGameGetTransparencyZoneMinSO(struct HIE_tdstSuperObject *hSupObj);

/* 0x00416D40 */
extern unsigned char fn_ucGetNumberOfRLIFromCharacter(HIE_tdstSuperObject *hSprObj,
                                                      unsigned char *p_ucIndex1,
                                                      unsigned char *p_ucIndex2,
                                                      MTH_tdxReal *p_xPercent);

/* 0x00418920 */
static void HIE_pc_fn_vStepWrappingCounter(HIE_tdstSuperObject *hSprObj)
{
    signed char *pcObj;
    char         cVar2;

    if (hSprObj != NULL) {
        pcObj = (signed char *)*(void **)hSprObj->hLinkedObject.p_Void;
        if (pcObj != NULL) {
            cVar2 = (char)(pcObj[0x104] + 1);
            pcObj[0x104] = cVar2;
            if ((int)cVar2 == (unsigned int)(unsigned char)pcObj[0x103]) {
                pcObj[0x104] = 0;
            }
        }
    }
}

/* 0x00418960 */
char __cdecl FUN_00418960(void)
{
    return 1;
}

POS_tdstCompletePosition *HIE_hRetrieveCurrentMatrix(void)
{
    return HIE_g_p_stCurrentInCamCoordsMatrix;
}

void HIE_fn_vSendCharacterModulesToViewPort(void *hVpt,
                                             HIE_tdstSuperObject *hSprObj,
                                             long DrawMask)
{
    HIE_tdstSuperObject *hChild;
    void *hLight;

    if (hSprObj->ulFlags & HIE_C_Flag_Superimposed)
        return;

    HIE_fn_vPushMatrix(hSprObj);
    HIE_fn_vPushOnCameraMatrixStack(hSprObj);

    if ((unsigned char)(hSprObj->ulFlags >> 16) & 0x02)
        GLI_xSetViewportLightsOnOff(0);
    hLight = HIE_fn_hGetSuperObjectLight(hSprObj);
    if (hLight)
        GLI_vAddLightToViewport(hVpt, hLight);

    DrawMask &= hSprObj->lDrawModeMask;
    if (HIE_M_MustBeDisplayed(DrawMask) && !(hSprObj->ulFlags & 0x02UL))
        HIE_fn_vSendSuperObjectToViewport(hVpt, hSprObj, DrawMask);

    for (hChild = hSprObj->p_stFirstChild;
         hChild != NULL;
         hChild = hChild->p_stNextBrother)
    {
        if (hChild->ulTypeOfLinkedObject != HIE_C_ulActor)
            HIE_fn_vSendCharacterModulesToViewPort(hVpt, hChild, DrawMask);
    }

    if (hLight)
        GLI_vPopLightsInViewport(hVpt, 1);
    if ((unsigned char)(hSprObj->ulFlags >> 16) & 0x02)
        GLI_xSetViewportLightsOnOff(1);

    HIE_fn_vPopMatrix();
    HIE_fn_vPopOnCameraMatrixStack();
}

/* 0x0045CDA0 */
void HIE_vSetCharactereRLI ( unsigned char ucNbRLI, unsigned char ucIndex1, unsigned char ucIndex2, MTH_tdxReal xPercent)
{
    HIE_g_ucRenderMode = ucNbRLI;
    HIE_g_ucShadowIdx  = (unsigned char)(ucIndex1 - 1);
    HIE_g_ucMirrorIdx  = (unsigned char)(ucIndex2 - 1);
    HIE_g_xMirrorBlend = xPercent;
}

/* 0x00426A80 */
extern unsigned char __cdecl GLI_bDoesLightAffectBV(struct GLI_tdstLight_ *p_stLight,
                                                    MTH3D_tdstVector *p_stCenter, float xRadius);

#pragma optimize("y", off)

#define SPO_M_UpdateLightsBeforeSending(_hSO,_hLight,_hVpt) \
{                                                           \
    if (((_hSO)->ulFlags & HIE_C_Flag_ExcluLight))          \
        GLI_xSetViewportLightsOnOff(0);                     \
    _hLight=HIE_fn_hGetSuperObjectLight(_hSO);              \
    if(_hLight)                                             \
        GLI_vAddLightToViewport(_hVpt,_hLight);             \
}

#define SPO_M_UpdateLightsAfterSending(_hSO,_hLight,_hVpt)  \
{                                                           \
    if(_hLight)                                             \
    {                                                       \
        GLI_vPopLightsInViewport(_hVpt , 1);                \
    }                                                       \
    if (((_hSO)->ulFlags & HIE_C_Flag_ExcluLight))          \
        GLI_xSetViewportLightsOnOff(1);                     \
}

void HIE_fn_vSendCharacterToViewport
(
    void                                *_hVpt,
    HIE_tdstSuperObject                 *_hSprObj,
    long                                _DrawMask,
    long                                _lCullingResult
)
{
    ACP_tdxIndex                        i;
    HIE_tdstSuperObject                 *hChild;
    void                                *hLight;
    void                                *hBoundingSphere = NULL;
    int                                 bCanComputeLight , bDisplayed , bOnPlatform;
    MTH_tdxReal                         xTransparencyZoneLevel,xTransparencyZoneMax,xMaxZone,xMinZone;
    unsigned long                       ulCustomBit , ulFlags , ulDrawMask;
    unsigned char                       ucIndex1, ucIndex2, ucNbRLIUse ;
    MTH_tdxReal                         xPercent ;
    MTH3D_tdstVector                    *p_stSphereCenter;
    MTH_tdxReal                         xSphereRadius;
    HIE_tdstSuperObject                 *hFather;
    struct HIE_pc_tdstStdGameView_      *hStdGame;

    int                                 cRLIEnabled;
    MTH3D_tdstVector                    stTransformedCenter;
    MTH_tdxReal                         xScale;
    int                                 iLight;
    int                                 bInRange;

    if ( HIE_fn_SO_bIsSuperimposed (_hSprObj) || (((struct HIE_pc_tdstStdGameView_ *)((HIE_tdstEngineObject *)_hSprObj->hLinkedObject.p_Void)->hStandardGame)->ucMiscFlags & 4) != 4 )
    {
        return ;
    }

    ulCustomBit = fn_ulStandardGameGetCustomBitsSO(_hSprObj);

    hStdGame = (struct HIE_pc_tdstStdGameView_ *)((HIE_tdstEngineObject *)_hSprObj->hLinkedObject.p_Void)->hStandardGame;

    if( HIE_fn_SO_bIsHidden(_hSprObj) )
    {

        hStdGame->ulCustomBits = ulCustomBit | GAM_C_CustBitOutOfVisibility;
        hStdGame->ucMiscFlags &= 0xdf;
        return;
    }

    bOnPlatform = FALSE;

    hFather = _hSprObj->p_stFather;
    if( hFather && (hFather->ulTypeOfLinkedObject == HIE_C_ulActor) && (((struct HIE_pc_tdstStdGameView_ *)((HIE_tdstEngineObject *)hFather->hLinkedObject.p_Void)->hStandardGame)->ucPlatFormType != 0) )
    {
        bOnPlatform = TRUE;
#if !defined(U64_NO_PUSH_MATRIX)
        HIE_fn_vPushMatrix (hFather);
#endif
        HIE_fn_vPushOnCameraMatrixStack(hFather);
    }

#if !defined(U64_NO_PUSH_MATRIX)
    HIE_fn_vPushMatrix (_hSprObj);
#endif
    HIE_fn_vPushOnCameraMatrixStack(_hSprObj);

    hBoundingSphere = (void *)HIE_fn_hGetSuperObjectBoundingVolume(_hSprObj);
    if (hBoundingSphere)
    {
        p_stSphereCenter = GEO_fn_pGetCenterPointOfBoundingSphere(hBoundingSphere);
        xSphereRadius = GEO_fn_xGetRadiusOfBoundingSphere(hBoundingSphere);
    }

    if( ulCustomBit & GAM_C_CustBitUsesTransparencyZone )
    {
        xTransparencyZoneMax    = GEO_xGetZFar();
        xTransparencyZoneLevel  = GEO_xGetZFarTransparencyZone();
        xMaxZone                = MTH_M_xLongToReal(fn_ucStandardGameGetTransparencyZoneMaxSO(_hSprObj));
        xMinZone                = MTH_M_xLongToReal(fn_ucStandardGameGetTransparencyZoneMinSO(_hSprObj));

        if( MTH_M_bIsNull(xTransparencyZoneMax) )
        {
            GEO_vSetZFar( xMaxZone );
            GEO_vSetZFarTransparencyZone( MTH_M_xSub( xMaxZone , xMinZone ) );
        }
        else if( MTH_M_bLess( xMinZone , xTransparencyZoneMax ) )
        {
            GEO_vSetZFar( MTH_M_xMin(xTransparencyZoneMax , xMaxZone) );
            GEO_vSetZFarTransparencyZone( MTH_M_xSub( GEO_xGetZFar() , xMinZone ) );
        }
        ulFlags = HIE_fn_SO_ulGetFlags(_hSprObj);
        HIE_fn_SO_vSetFlags(_hSprObj, ulFlags | HIE_C_Flag_ModuleTransparency);
        ulDrawMask = HIE_fn_lGetSuperObjectDrawMask(_hSprObj);
        HIE_fn_vSetSuperObjectDrawMask(_hSprObj , ulDrawMask & ~GLI_C_lIsNotGrided );

        _lCullingResult &= ~(GEO_C_lCullingNear | GEO_C_lCullingFar);
    }

    if( (_lCullingResult != GEO_C_lCullingIn) && hBoundingSphere )
        _lCullingResult = GEO_lCullingSphereNoMMM(_hVpt, p_stSphereCenter, xSphereRadius, HIE_fn_hGetSuperObjectGlobalMatrix(_hSprObj), _lCullingResult);
    else
        _lCullingResult  = GEO_C_lCullingIn;

    _DrawMask |= ~HIE_C_InheritableFlags;
    _DrawMask &= HIE_fn_lGetSuperObjectDrawMask(_hSprObj);
    hStdGame = (struct HIE_pc_tdstStdGameView_ *)((HIE_tdstEngineObject *)_hSprObj->hLinkedObject.p_Void)->hStandardGame;

    if ( HIE_M_MustBeDisplayed(_DrawMask) && !HIE_fn_SO_bIsHidden( _hSprObj ) && (_lCullingResult != GEO_C_lCullingOut) )
    {
        ulCustomBit &= ~GAM_C_CustBitOutOfVisibility;
        bDisplayed = TRUE;

        hStdGame->ucMiscFlags |= 0x20;

    }
    else
    {

        ulCustomBit |= GAM_C_CustBitOutOfVisibility;
        bDisplayed = FALSE;

        hStdGame->ucMiscFlags &= 0xdf;

    }

    hStdGame->ulCustomBits = ulCustomBit;

    if( bDisplayed && (_hSprObj->lNumberOfChildren > 0) )
    {

        ucNbRLIUse = fn_ucGetNumberOfRLIFromCharacter (_hSprObj, &ucIndex1, &ucIndex2, &xPercent) ;
        HIE_vSetCharactereRLI ( ucNbRLIUse, ucIndex1, ucIndex2, xPercent ) ;

        bCanComputeLight = TRUE;

        if ( ucNbRLIUse > 0 )
        {
            GLI_xSetViewportLightsOnOff(0);
        }
        else
        {
            SPO_M_UpdateLightsBeforeSending(_hSprObj , hLight , _hVpt);
        }

        HIE_pc_fn_vStepWrappingCounter(_hSprObj);
        cRLIEnabled = FUN_00418960();
        if (hBoundingSphere == NULL) {
            GLI_vResetTableOfActiveLightsForChar((char)(cRLIEnabled != 0));
        } else {
            xScale = POS_fn_xGetMaxScale((POS_tdstCompletePosition *)_hSprObj->hGlobalMatrix);
            xSphereRadius = xScale * xSphereRadius;
            POS_fn_vMulMatrixVertex(&stTransformedCenter,
                                    (POS_tdstCompletePosition *)_hSprObj->hGlobalMatrix,
                                    p_stSphereCenter);
            for (iLight = 0; iLight < g_lActiveLightCount; iLight++) {
                bInRange = GLI_bDoesLightAffectBV((struct GLI_tdstLight_ *)g_aActiveLights[iLight],
                                        &stTransformedCenter, xSphereRadius);
                GLI_vSetLightActivityForChar(iLight, (unsigned char)((char)bInRange != '\0'));
            }
        }

        HIE_M_ForEachChildOf(_hSprObj, hChild, i)
        if (HIE_fn_ulGetSuperObjectType(hChild)!=HIE_C_ulActor)
            HIE_fn_vSendCharacterModulesToViewPort(_hVpt, hChild, _DrawMask );

        if ( ucNbRLIUse > 0 )
        {
            GLI_xSetViewportLightsOnOff(1);
        }
        else
        {
            SPO_M_UpdateLightsAfterSending(_hSprObj , hLight , _hVpt);
        }

        HIE_vSetCharactereRLI ( 0, 0, 0, 0 ) ;

    }

    if( ulCustomBit & GAM_C_CustBitUsesTransparencyZone )
    {
        GEO_vSetZFar( xTransparencyZoneMax );
        GEO_vSetZFarTransparencyZone( xTransparencyZoneLevel );
        HIE_fn_SO_vSetFlags(_hSprObj, ulFlags);
        HIE_fn_vSetSuperObjectDrawMask(_hSprObj , ulDrawMask );
    }

#if !defined(U64_NO_PUSH_MATRIX)
    HIE_fn_vPopMatrix();
#endif
    HIE_fn_vPopOnCameraMatrixStack();

    if( bOnPlatform )
    {
#if !defined(U64_NO_PUSH_MATRIX)
        HIE_fn_vPopMatrix ();
#endif
        HIE_fn_vPopOnCameraMatrixStack();
    }

}
#pragma optimize("", on)

#define GLI_cGlobalFogIsOn  (*(char *)GLI_g_cGlobalFogIsOn_ADDR)

static void HIE_fn_vSendSPOToFixViewport
(
    void                                *_hVpt,
    HIE_tdstSuperObject                 *_hSprObj,
    long                                _DrawMask,
    long                                _lCullingResult
)
{
    ACP_tdxIndex                        i;
    HIE_tdstSuperObject                 *hChild;
    void                                *hLight;
    void                                *hBoundingSphere = NULL;
    int                                 bDisplayed;
    MTH3D_tdstVector                    stSphereCenter;
    MTH_tdxReal                         xSphereRadius;
    unsigned long                       ulCustomBit;
    unsigned char                       ucIndex1, ucIndex2, ucNbRLIUse ;
    MTH_tdxReal                         xPercent ;

    HIE_fn_vPushMatrix (_hSprObj);

    HIE_fn_vPushOnCameraMatrixStack(_hSprObj);

    if ( HIE_fn_ulGetSuperObjectType(_hSprObj) == HIE_C_ulActor )
    {
        if( _hSprObj->lNumberOfChildren > 0)
        {

            struct HIE_pc_tdstStdGameView_ *hStdGame = (struct HIE_pc_tdstStdGameView_ *)((HIE_tdstEngineObject *)_hSprObj->hLinkedObject.p_Void)->hStandardGame;

            hBoundingSphere = (void *)HIE_fn_hGetSuperObjectBoundingVolume(_hSprObj);
            if ( hBoundingSphere )
            {
                MTH3D_M_vCopyVector(&stSphereCenter, GEO_fn_pGetCenterPointOfBoundingSphere(hBoundingSphere));
                xSphereRadius = GEO_fn_xGetRadiusOfBoundingSphere(hBoundingSphere);
            }

            if( (_lCullingResult != GEO_C_lCullingIn) && hBoundingSphere )
                _lCullingResult = GEO_lCullingSphereNoMMM(_hVpt, &stSphereCenter, xSphereRadius, HIE_fn_hGetSuperObjectGlobalMatrix(_hSprObj), _lCullingResult);
            else
                _lCullingResult = GEO_C_lCullingIn;

            _DrawMask |= ~HIE_C_InheritableFlags;
            _DrawMask &= HIE_fn_lGetSuperObjectDrawMask(_hSprObj);
            ulCustomBit = fn_ulStandardGameGetCustomBitsSO(_hSprObj);

            if ( HIE_M_MustBeDisplayed(_DrawMask) && !HIE_fn_SO_bIsHidden( _hSprObj ) && (_lCullingResult != GEO_C_lCullingOut))
            {
                ulCustomBit &= ~GAM_C_CustBitOutOfVisibility;
                bDisplayed = TRUE;

                hStdGame->ucMiscFlags |= 0x20;

            }
            else
            {

                ulCustomBit |= GAM_C_CustBitOutOfVisibility;
                bDisplayed = FALSE;

                hStdGame->ucMiscFlags &= 0xdf;

            }

            hStdGame->ulCustomBits = ulCustomBit;

            if(bDisplayed)
            {

                SPO_M_UpdateLightsBeforeSending(_hSprObj , hLight , _hVpt);
                ucNbRLIUse = fn_ucGetNumberOfRLIFromCharacter (_hSprObj, &ucIndex1, &ucIndex2, &xPercent) ;
                HIE_vSetCharactereRLI ( ucNbRLIUse, ucIndex1, ucIndex2, xPercent ) ;

                HIE_M_ForEachChildOf(_hSprObj, hChild, i)
                if (HIE_fn_ulGetSuperObjectType(hChild)!=HIE_C_ulActor)
                {
                    if (HIE_fn_SO_ulGetFlags(_hSprObj) & HIE_C_Flag_ulRenderOnNearPlane)
                        HIE_fn_SO_vSetFlags(hChild, HIE_fn_SO_ulGetFlags(hChild) | HIE_C_Flag_ulRenderOnNearPlane);
                    HIE_fn_vSendSPOToFixViewport(_hVpt, hChild, _DrawMask , _lCullingResult);
                }

                SPO_M_UpdateLightsAfterSending(_hSprObj , hLight , _hVpt);
                HIE_vSetCharactereRLI(0, 0, 0, 0) ;

            }
        }
        else
        {

        }
    }
    else
    {

        _DrawMask &= HIE_fn_lGetSuperObjectDrawMask(_hSprObj);
        if ( HIE_M_MustBeDisplayed(_DrawMask) && !HIE_fn_SO_bIsHidden( _hSprObj ) )
            HIE_fn_vSendSuperObjectToViewport(_hVpt ,_hSprObj, _DrawMask);

        HIE_M_ForEachChildOf(_hSprObj, hChild, i)
        if ( HIE_fn_ulGetSuperObjectType(hChild) != HIE_C_ulActor )
            HIE_fn_vSendSPOToFixViewport(_hVpt, hChild, _DrawMask, _lCullingResult);

    }

    HIE_fn_vPopMatrix ();
    HIE_fn_vPopOnCameraMatrixStack();

}

ACP_tdxBool HIE_fn_bSendSPOToFixViewport(void *_hVpt, long _lDrawMask)
{
    char cSaveFog;
    unsigned char ucSinusState;
    short xI;
    HIE_tdstSuperObject **ppSPO;

    cSaveFog = GLI_cGlobalFogIsOn;
    GLI_cGlobalFogIsOn = 0;
    GLI_vFogOff();
    ucSinusState = GLI_fn_SinusEffect_vGetState();
    if (!GLI_fn_SinusEffect_cIsSecondViewportAffected())
        GLI_fn_SinusEffect_vSetState(0);

    HIE_fn_bInitCameraMatrixStack(_hVpt);

    ppSPO = (HIE_tdstSuperObject **)HIE_gd_hFixSPO;
    for (xI = 0; xI < HIE_g_xFixIndex; xI++, ppSPO++) {
        HIE_fn_vSendSPOToFixViewport(_hVpt, *ppSPO,
            (long)((unsigned long)(*ppSPO)->lDrawModeMask & (unsigned long)_lDrawMask), 0);
    }

    GLI_fn_SinusEffect_vSetState(ucSinusState);
    GLI_cGlobalFogIsOn = cSaveFog;
    if (cSaveFog)
        GLI_vFogOn();

    return 1;
}

/* 0x0045CD30 */
void *HIE_fn_p_stGetGeometricObjectFromSuperObject(HIE_tdstSuperObject *h_SuperObject)
{
    unsigned long   ulSOType;
    void           *h_PO;
    MTH_tdxReal     p_xThreshold;
    void           *p_stObject;

    p_stObject = NULL;

    ulSOType = h_SuperObject->ulTypeOfLinkedObject;

    if (ulSOType == HIE_C_ulEDT_Geometric)
    {
        p_stObject = h_SuperObject->hLinkedObject.p_Void;
    }
    else if (ulSOType & (HIE_C_ulPO | HIE_C_ulPO_Mirror))
    {
        h_PO = h_SuperObject->hLinkedObject.p_Void;
        GLI_vGetVisualSetLOD(PO_fn_hGetVisualSet(h_PO), 0, &p_xThreshold, &p_stObject);
    }
    else if (ulSOType & (HIE_C_ulIPO | HIE_C_ulIPO_Mirror))
    {
        h_PO = IPO_fn_hGetPhysicalObject(h_SuperObject->hLinkedObject.p_Void);
        GLI_vGetVisualSetLOD(PO_fn_hGetVisualSet(h_PO), 0, &p_xThreshold, &p_stObject);
    }
    return p_stObject;
}

#pragma optimize("y", off)
void HIE_fn_hFindAllMirrorsInHierarchy(void *hVpt, HIE_tdstSuperObject *hSprObj,
    HIE_tdstSuperObject **p_hHandleOfMirror, void **p_hHandleOfGeometricMirror,
    short *p_shNbOfMirrors, short shNbMaxOfMirrors)
{
    unsigned long ulType;
    int bOBB;
    float *pBV;
    long lCulling;
    HIE_tdstSuperObject *hChild;
    MTH3D_tdstVector stMin, stMax;
    MTH3D_tdstVector *pTranslation;
    POS_tdstCompletePosition *hMatrix;
    short sCount;
    void *pPOData;
    POS_tdstCompletePosition stLocalMatrix;

    pBV = (float *)hSprObj->hBoundingVolume;
    bOBB = (hSprObj->ulFlags & HIE_C_Flag_ulTypeOfBoundingVolume) != 0;

    /* 0x0045C850 */
    if (pBV != NULL) {
        if (!bOBB) {
            lCulling = GEO_lCullingSphere(hVpt, (MTH3D_tdstVector *)pBV, pBV[3],
                                           hSprObj->hGlobalMatrix, 0);
        } else {

            hMatrix = (POS_tdstCompletePosition *)hSprObj->hGlobalMatrix;
            pTranslation = POS_fn_vGetTranslationVectorReadOnly(hMatrix);
            stMin.xX = pBV[0] + pTranslation->xX; stMin.xY = pBV[1] + pTranslation->xY;
            stMin.xZ = pBV[2] + pTranslation->xZ;
            stMax.xX = pBV[3] + pTranslation->xX; stMax.xY = pBV[4] + pTranslation->xY;
            stMax.xZ = pBV[5] + pTranslation->xZ;
            /* 0x0045C850 */
            POS_fn_vSetIdentityMatrix(&stLocalMatrix);
            lCulling = GEO_lCullingBox(hVpt, &stMin, &stMax, 0);
        }

        if (lCulling == GEO_C_lCullingOut)
            return;
    }

    ulType = hSprObj->ulTypeOfLinkedObject;
    if ((ulType & 0x250) != 0) {
        sCount = *p_shNbOfMirrors;
        if (sCount < shNbMaxOfMirrors) {
            p_hHandleOfMirror[sCount] = hSprObj;
            if (ulType == HIE_C_ulMirror) {
                p_hHandleOfGeometricMirror[sCount] = hSprObj->hLinkedObject.p_Void;
            } else if (ulType == HIE_C_ulPO_Mirror || ulType == HIE_C_ulIPO_Mirror) {
                /* 0x0045C850 */
                pPOData = IPO_fn_hGetPhysicalObject(hSprObj->hLinkedObject.p_Void);
                if (ulType == HIE_C_ulIPO_Mirror)
                    pPOData = IPO_fn_hGetPhysicalObject(pPOData);
                GLI_vGetVisualSetLOD(pPOData, 0, &HiePick_VoidReal, &HiePick_GeoHandle);
                p_hHandleOfGeometricMirror[sCount] = HiePick_GeoHandle;
            }
            (*p_shNbOfMirrors)++;
        }
    }

    for (hChild = hSprObj->p_stFirstChild; hChild != NULL; hChild = hChild->p_stNextBrother) {
        HIE_fn_hFindAllMirrorsInHierarchy(hVpt, hChild, p_hHandleOfMirror,
                                           p_hHandleOfGeometricMirror,
                                           p_shNbOfMirrors, shNbMaxOfMirrors);
    }
}
#pragma optimize("", on)
