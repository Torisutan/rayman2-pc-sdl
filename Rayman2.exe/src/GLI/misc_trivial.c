#include "GLI/misc_trivial.h"
#include "GLI/GLI_globals.h"
#include "FIL/File.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "HIE/HIE_globals.h"
#include "MMG/SNA_reloc.h"
#include "GLI/texture_lifecycle.h"
#include "MTH/MTH3D.h"
#include "GAM/GAM_types.h"
#include "GAM/GAM_globals.h"
#include "POS/POS_types.h"
#include "GLI/GlobalAlpha.h"
#include "GLI/Proj.h"
#include "GLI/sprite.h"

#define WIN32_LEAN_AND_MEAN
#include <windows.h>

static void *GLI_pc_g_p_vLensFlareRef;   /* 0x004A72A8 */
unsigned long GLI_g_ul4a72b0_storage;   /* 0x004A72B0 */
#define GLI_g_ul4a72b0   GLI_g_ul4a72b0_storage

void          __cdecl GLI_pc_fn_vLoadTexGlobalsTail(void);     /* 0x00423640 */
void          __cdecl GLI_pc_fn_vLoadTexGlobalsFull(void);     /* 0x004237f0 */
void          __cdecl HIE_pc_fn_vLoadScaledShortRecords(float *pfDst, const short *psSrc,
                                                        long lCount); /* 0x00431400 */
void          __cdecl GLI_pc_fn_vResetTextureSlotTables(void); /* 0x0042c6b0 */
void          __cdecl GLI_FatalExitOrSleep(void);              /* 0x004011c0 */
int           __cdecl Erm_fn_iMessageBox(const char *_szText, const char *_szTitle, unsigned int _uiStyle); /* 0x004028f0 */
void          __cdecl SNA_pc_vReloadGptTable(void);            /* 0x0044cf10 */
void          __cdecl SNA_pc_vReleaseGptTable(void);           /* 0x0044cf60 */
void          __cdecl SNA_pc_vRelocateGptSlot(unsigned long *_p_ulSlot); /* 0x0044d040 */

/* 0x00421820 */
void __cdecl FUN_00421820(void)
{
    if (SNA_g_ucLoadListMode == 0) {
        GLI_pc_fn_vLoadTexGlobalsTail();
        GLI_vComputeTextures();
    } else {
        GLI_pc_fn_vLoadTexGlobalsFull();
    }
    if (SNA_g_pGptContext != NULL) {
        SNA_pc_vFreeGptTable(SNA_g_pGptContext);
    }
    SNA_g_pGptContext = NULL;
}

/* 0x00424fe0 */
void __cdecl FUN_00424fe0(unsigned int arg)
{
    GLI_g_ul503d54 = arg & 0xff;
}

/* 0x00421860 */
void __cdecl FUN_00421860(unsigned int arg)
{
    FUN_00424fe0(arg);
}

/* 0x00420b00 */
void __cdecl GLI_pc_fn_vBeginScene(unsigned short hDevice)
{
    (*(void (__cdecl **)(void))GLI_DRV_bBeginScene)();
}

/* 0x00420b10 */
void __cdecl GLI_pc_fn_vEndScene(unsigned short hDevice)
{
    (*(void (__cdecl **)(void))GLI_DRV_bEndScene_)();
}

/* 0x00422020 */
int __cdecl GLI_fn_cWindowedModeIsOptimized(void)
{

    return (*(int (__cdecl **)(void))GLI_DRV_bWindowedModeIsOptimized)() != 0;
}

/* 0x00422030 */
void __cdecl GLI_fn_vOptimizedWindowedMode(void)
{
    (*(void (__cdecl **)(void))GLI_DRV_vOptimizedWindowedMode)();
}

/* 0x00422040 */
void __cdecl GLI_fn_vNonOptimizedWindowedMode(void)
{
    (*(void (__cdecl **)(void))GLI_DRV_vNonOptimizedWindowedMode)();
}

static const char GLI_CD_szInsertMsg[] = "Please insert the CD";

/* 0x00423640 */
void __cdecl GLI_pc_fn_vLoadTexGlobalsTail(void)
{
    FILE *pFile;
    int iDialog;
    size_t ulTotal;
    size_t ulCount;
    size_t ulIdx;
    unsigned long *pulSlot;
    char szBase[256];

    pFile = fopen(GLI_pc_g_aszTexArchiveFileName, "rb");
    if (pFile == NULL)
    {
        strcpy(szBase, GLI_pc_g_aszTexArchiveFileName);
        sprintf(GLI_pc_g_aszTexArchiveFileName, "%c\\\\%s", (int)g_cCDROM, szBase);
        pFile = fopen(GLI_pc_g_aszTexArchiveFileName, "rb");
        while (pFile == NULL)
        {
            iDialog = Erm_fn_iMessageBox(GLI_CD_szInsertMsg, GLI_CD_szInsertMsg, 0x40035);
            if (iDialog == 2)
            {
                GLI_FatalExitOrSleep();
            }
            pFile = fopen(GLI_pc_g_aszTexArchiveFileName, "rb");
        }
        strcpy(GLI_pc_g_aszTexArchiveFileName, szBase);
    }
    fread(&ulTotal, 4, 1, pFile);
    fread(&ulCount, 4, 1, pFile);
    fread(&GLI_gs_aDEFTableOfTextureAlreadyRead[ulTotal - ulCount], ulCount, 4, pFile);
    fread(&GLI_gs_lNumberOfTextureToCreate, 1, 4, pFile);
    fread(GLI_gs_aDEFTableOfTextureMemoryChannels, ulTotal, 4, pFile);
    fread(&GLI_g_ul503d54, 1, 4, pFile);
    fclose(pFile);
    SNA_pc_vReloadGptTable();
    ulIdx = ulTotal - ulCount;
    if (ulIdx < ulTotal)
    {
        pulSlot = (unsigned long *)GLI_gs_aDEFTableOfTextureAlreadyRead + ulIdx;
        do
        {
            SNA_pc_vRelocateGptSlot(pulSlot);
            ulIdx++;
            pulSlot++;
        } while (ulIdx < ulTotal);
    }
    SNA_pc_vReleaseGptTable();
}

/* 0x004237f0 */
void __cdecl GLI_pc_fn_vLoadTexGlobalsFull(void)
{
    FILE *pFile;
    int iDialog;
    unsigned long ulTotal;
    size_t ulCount;
    unsigned long ulIdx;
    unsigned long *pulSlot;
    char szBase[256];

    GLI_pc_fn_vResetTextureSlotTables();
    pFile = fopen(GLI_pc_g_aszTexArchiveFileName, "rb");
    if (pFile == NULL)
    {
        strcpy(szBase, GLI_pc_g_aszTexArchiveFileName);
        sprintf(GLI_pc_g_aszTexArchiveFileName, "%c\\\\%s", (int)g_cCDROM, szBase);
        pFile = fopen(GLI_pc_g_aszTexArchiveFileName, "rb");
        while (pFile == NULL)
        {
            iDialog = Erm_fn_iMessageBox(GLI_CD_szInsertMsg, GLI_CD_szInsertMsg, 0x40035);
            if (iDialog == 2)
            {
                GLI_FatalExitOrSleep();
            }
            pFile = fopen(GLI_pc_g_aszTexArchiveFileName, "rb");
        }
        strcpy(GLI_pc_g_aszTexArchiveFileName, szBase);
    }
    fread(&ulTotal, 4, 1, pFile);
    fread(&ulCount, 4, 1, pFile);
    fread(GLI_gs_aDEFTableOfTextureAlreadyRead, ulCount, 4, pFile);
    fread(&GLI_gs_lNumberOfTextureToCreate, 1, 4, pFile);
    fread(GLI_gs_aDEFTableOfTextureMemoryChannels, ulCount, 4, pFile);
    fread(&GLI_g_ul503d54, 1, 4, pFile);
    fclose(pFile);
    SNA_pc_vReloadGptTable();
    ulIdx = 0;
    if (ulIdx < ulTotal)
    {
        pulSlot = (unsigned long *)GLI_gs_aDEFTableOfTextureAlreadyRead;
        do
        {
            SNA_pc_vRelocateGptSlot(pulSlot);
            ulIdx++;
            pulSlot++;
        } while (ulIdx < ulTotal);
    }
    SNA_pc_vReleaseGptTable();
}

/* 0x004251b0 */
void __cdecl GLI_fn_vCloseBigFileTextures(void)
{
    FIL_fn_vCloseConcatFile(&GLI_g_hConcatTexturesFile);  /* 0x00493590 */
}

/* 0x00428da0 */
void __cdecl FUN_00428da0(int base, short *p, int idx, int arg4)
{
    HIE_pc_fn_vLoadScaledShortRecords(
        (float *)((char *)0 + idx * 0x10 + 0x14c14 + base), p, arg4);
}

/* 0x00428dd0 */
void __cdecl GLI_vSetLensFlareRef ( void *p_vRef )
{
    GLI_pc_g_p_vLensFlareRef = p_vRef;
}

/* 0x00423610 */
void __cdecl FUN_00423610(const char *name)
{
    strcpy((char *)GLI_pc_g_szTexArchiveFileName, name);
}

/* 0x005040B0 */
unsigned long GLI_pc_g_a3_xMirrorPlaneNormal[3];
unsigned long GLI_pc_g_xMirrorPlaneDistance;

/* 0x00423990 */
void __cdecl FUN_00423990(unsigned long arg1_unused, unsigned long *p_vec3, unsigned long scalar)
{
    (void)arg1_unused;
    GLI_pc_g_a3_xMirrorPlaneNormal[0] = p_vec3[0];
    GLI_pc_g_a3_xMirrorPlaneNormal[1] = p_vec3[1];
    GLI_pc_g_a3_xMirrorPlaneNormal[2] = p_vec3[2];
    GLI_pc_g_xMirrorPlaneDistance     = scalar;
}

/* 0x004239c0 */
void __cdecl FUN_004239c0(void)
{
    GLI_M_WaterDepthColorFlags |= 0x01u;
}

/* 0x004239d0 */
void __cdecl FUN_004239d0(void)
{
    GLI_M_WaterDepthColorFlags &= ~0x01u;
}

/* 0x004239e0 */
void __cdecl FUN_004239e0(void)
{
    GLI_M_WaterDepthColorFlags |= 0x20u;
}

/* 0x00423c00 */
void __cdecl GLI_fn_SinusEffect_vSetState(unsigned char v)
{
    GLI_pc_g_cSinusEffectState = v;
}

/* 0x00423c10 */
unsigned char __cdecl GLI_fn_SinusEffect_vGetState(void)
{
    return GLI_pc_g_cSinusEffectState;
}

/* 0x00423C20 */
void __cdecl GLI_fn_SinusEffect_vSetFlag(char _cFlag)
{
    g_cSinusEffectFlag = _cFlag;
}

/* 0x00423C30 */
char __cdecl GLI_fn_SinusEffect_vGetFlag(void)
{
    return g_cSinusEffectFlag;
}

/* 0x00423c40 */
char __cdecl GLI_fn_SinusEffect_cIsSecondViewportAffected(void)
{
    return (char)(g_cSinusEffectFlag & 0x01);
}

/* 0x00423C50 */
void __cdecl GLI_fn_SinusEffect_vSetFreq(long _lXFreq, long _lYFreq, long _lZFreq)
{
    GLI_M_a3_lSinEffectFreq[0] = _lXFreq;
    GLI_M_a3_lSinEffectFreq[1] = _lYFreq;
    GLI_M_a3_lSinEffectFreq[2] = _lZFreq;
}

/* 0x00423C70 */
void __cdecl GLI_fn_SinusEffect_vSetAmplitude(MTH3D_tdstVector *_p_stAmpVector)
{
    MTH3D_M_vCopyVector(&GLI_M_stSinEffectAmp, _p_stAmpVector);
}

/* 0x00423E60 */
void __cdecl GLI_vSetSinusEffectOnRLIAmp(long xRed, long xGreen, long xBlue)
{
    GLI_M_a3_lSinRLIAmp[0] = xRed;
    GLI_M_a3_lSinRLIAmp[1] = xGreen;
    GLI_M_a3_lSinRLIAmp[2] = xBlue;
}

/* 0x00423E80 */
void __cdecl GLI_vSetSinusEffectOnRLIBase(long xRed, long xGreen, long xBlue)
{
    GLI_M_a3_lSinRLIBase[0] = xRed;
    GLI_M_a3_lSinRLIBase[1] = xGreen;
    GLI_M_a3_lSinRLIBase[2] = xBlue;
}

/* 0x00423EA0 */
void __cdecl GLI_vSetSinEffetBase(long xBase)
{
    GLI_M_lSinEffectBase = xBase;
}

long                a3_lSinEffectFreqSaved[3];      /* 0x00504090 */
MTH3D_tdstVector    stSinEffectAmpSaved;            /* 0x00504030 */
char                g_cSinusEffectStateSaved;       /* 0x00504020 */
char                g_cSinusEffectFlagSaved;        /* 0x00504060 */

long                a3_xSinEffectBaseSaved[3];      /* 0x00504050 */
MTH3D_tdstVector    stSinEffectVector;              /* 0x00504010 */
MTH3D_tdstVector    stSinEffectVectorSaved;         /* 0x005040C0 */
long                xSinusEffectDepthStart;         /* 0x0050401C */
long                xSinusEffectDepthStartSaved;    /* 0x0050404C */

/* 0x00423EB0 */
void __cdecl GLI_vSaveSinusContext(void)
{
    a3_lSinEffectFreqSaved[0] = GLI_M_a3_lSinEffectFreq[0];
    a3_lSinEffectFreqSaved[1] = GLI_M_a3_lSinEffectFreq[1];
    a3_lSinEffectFreqSaved[2] = GLI_M_a3_lSinEffectFreq[2];
    a3_xSinEffectBaseSaved[0] = GLI_M_a3_lSinEffectBase[0];
    a3_xSinEffectBaseSaved[1] = GLI_M_a3_lSinEffectBase[1];
    a3_xSinEffectBaseSaved[2] = GLI_M_a3_lSinEffectBase[2];
    stSinEffectAmpSaved = GLI_M_stSinEffectAmp;
    stSinEffectVectorSaved = stSinEffectVector;
    xSinusEffectDepthStartSaved = xSinusEffectDepthStart;
    g_cSinusEffectStateSaved = (char)GLI_pc_g_cSinusEffectState;
    g_cSinusEffectFlagSaved = g_cSinusEffectFlag;
}

/* 0x00423F60 */
void __cdecl GLI_vRestoreSinusContext(void)
{
    GLI_M_a3_lSinEffectFreq[0] = a3_lSinEffectFreqSaved[0];
    GLI_M_a3_lSinEffectFreq[1] = a3_lSinEffectFreqSaved[1];
    GLI_M_a3_lSinEffectFreq[2] = a3_lSinEffectFreqSaved[2];
    GLI_M_a3_lSinEffectBase[0] = a3_xSinEffectBaseSaved[0];
    GLI_M_a3_lSinEffectBase[1] = a3_xSinEffectBaseSaved[1];
    GLI_M_a3_lSinEffectBase[2] = a3_xSinEffectBaseSaved[2];
    GLI_M_stSinEffectAmp = stSinEffectAmpSaved;
    stSinEffectVector = stSinEffectVectorSaved;
    xSinusEffectDepthStart = xSinusEffectDepthStartSaved;
    GLI_pc_g_cSinusEffectState = (unsigned char)g_cSinusEffectStateSaved;
    g_cSinusEffectFlag = g_cSinusEffectFlagSaved;
}

/* 0x00421e80 */
void __cdecl GLI_fnv_SetDllName(const char *name)
{
    strcpy((char *)GLI_pc_g_szConfiguredDllName, name);
}

/* 0x00421eb0 */
void __cdecl GLI_fnv_SetDriverName(const char *name)
{
    strcpy((char *)GLI_pc_g_szEnumModesDriver, name);
}

/* 0x00421ee0 */
void __cdecl GLI_fnv_SetDeviceName(const char *name)
{
    strcpy((char *)GLI_pc_g_szEnumModesDevice, name);
}

static unsigned long GLI_pc_g_ulModeRequestedFlag;   /* 0x00504804 */

void __cdecl GLI_fnv_SetModeByString(char *param_1)
{
    char *p;

    *param_1 = '1';
    GLI_pc_g_ulModeRequestedFlag = 1;
    p = strchr(param_1, '-');
    if (p != (char *)0)
    {
        GLI_pc_g_ulRequestedWidth = (unsigned long)atoi(p + 1);
        p = strchr(p + 1, 'x');
        if (p != (char *)0)
        {
            GLI_pc_g_ulRequestedHeight = (unsigned long)atoi(p + 1);
            p = strchr(p + 1, 'x');
            if (p != (char *)0)
                GLI_pc_g_ulRequestedBpp = (unsigned long)atoi(p + 1);
        }
    }
}

long GLI_pc_g_lTexturesMemMode = 6;   /* 0x004a077c */

void __cdecl GLI_pc_fn_vParseTexturesMemIni(char *szValue)
{
    if (!_stricmp(szValue, "Vram"))
        GLI_pc_g_lTexturesMemMode = 0;
    else if (!_stricmp(szValue, "Vram+"))
        GLI_pc_g_lTexturesMemMode = 1;
    else if (!_stricmp(szValue, "Vram4"))
        GLI_pc_g_lTexturesMemMode = 2;
    else if (!_stricmp(szValue, "Vram8"))
        GLI_pc_g_lTexturesMemMode = 3;
    else if (!_stricmp(szValue, "Agp"))
        GLI_pc_g_lTexturesMemMode = 6;
    else if (!_stricmp(szValue, "Agp4"))
        GLI_pc_g_lTexturesMemMode = 4;
    else if (!_stricmp(szValue, "Agp8"))
        GLI_pc_g_lTexturesMemMode = 5;
    else
        GLI_pc_g_lTexturesMemMode = 6;
}

/* 0x00421f90 */
void __cdecl GLI_xClearViewingList(void)
{
    (*(void (__cdecl **)(void))GLI_DRV_xClearViewingList)();
}

/* 0x00421fa0 */
void __cdecl GLI_xSendListToViewport(unsigned long arg)
{
    (*(void (__cdecl **)(unsigned long))GLI_DRV_xSendListToViewport_)(arg);
}

/* 0x00421fb0 */
void __cdecl GLI_ClearZBufferRegion(long lXStart,long lXEnd,long lYStart,long lYEnd)
{
    ((void (__cdecl *)(long, long, long, long))
        GLI_DRV_g_vClearZBufferRegion)(lXStart, lXEnd, lYStart, lYEnd);
}

#include "GLI/GLI_BigGlobals.h"

/* 0x00422000 */
void __cdecl FUN_00422000(int arg)
{
    GLI_BIG_GLOBALS->lAMirrorIsDetected = (arg != 0) ? 3 : 0;
}

/* 0x004225d0 */
void __cdecl FUN_004225d0(unsigned long *p_dst, unsigned long *p_src)
{
    memcpy(p_dst, p_src, 0x37 * 4);
}

/* 0x00408580 */
unsigned long __cdecl GAM_pc_fn_ulGetEngineTrameNumber(void)
{
    return GAM_g_ulEngineTrameNumber;
}

/* 0x00428de0 */
char __cdecl GLI_cRegisterLensFlare (void *p_vRef)
{
    long i;
    long lNb;

    if (p_vRef != NULL) {
        lNb = GLI_pc_g_stLensFlareTab.lNumberOfLensFlare;
        for (i = 0; i < lNb; i++) {
            if (GLI_pc_g_stLensFlareTab.p_vReference[i] == p_vRef) return (char)i;
        }

        if (lNb < GLI_C_MaxLensFlare) {
            GLI_pc_g_stLensFlareTab.p_vReference[lNb] = p_vRef;
            GLI_pc_g_stLensFlareTab.lApparitionTrame[GLI_pc_g_stLensFlareTab.lNumberOfLensFlare] =
                (long)GAM_pc_fn_ulGetEngineTrameNumber();
            GLI_pc_g_stLensFlareTab.lCounter[GLI_pc_g_stLensFlareTab.lNumberOfLensFlare] = 0;
            GLI_pc_g_stLensFlareTab.bIntersect[GLI_pc_g_stLensFlareTab.lNumberOfLensFlare] = 0;
            return (char)(++GLI_pc_g_stLensFlareTab.lNumberOfLensFlare - 1);
        }
    }
    return -1;
}

#pragma intrinsic(memcpy)

/* 0x00428e60 */
void __cdecl GLI_vUnregisterLensFlare(void *p_vRef)
{
    long i;
    long lNb;

    if (p_vRef != NULL) {
        lNb = GLI_pc_g_stLensFlareTab.lNumberOfLensFlare;
        for (i = 0; i < lNb; i++) {
            if (GLI_pc_g_stLensFlareTab.p_vReference[i] == p_vRef) {
                if (i + 1 < lNb)
                    memcpy(&GLI_pc_g_stLensFlareTab.p_vReference[i],
                           &GLI_pc_g_stLensFlareTab.p_vReference[i + 1],
                           (lNb - i) * sizeof(void *));
                GLI_pc_g_stLensFlareTab.lNumberOfLensFlare--;
                return;
            }
        }
    }
}

/* 0x00428ed0 */
long __cdecl GLI_lLensFlareExist(void *p_vRef)
{
    long i;

    if (p_vRef != NULL) {
        for (i = 0; i < GLI_pc_g_stLensFlareTab.lNumberOfLensFlare; i++) {
            if (GLI_pc_g_stLensFlareTab.p_vReference[i] == p_vRef) {
                return i;
            }
        }
    }
    return -1;
}

/* 0x0042a980 */
void __cdecl FUN_0042a980(unsigned long arg, unsigned long argExtra)
{
    GLI_g_ul4a72b0 = arg;
}

#include "GLI/VisuS_st.h"

/* 0x0042af60 */
int __cdecl GLI_lGetVisualSetNumbertOfLOD(void *p)
{
    return (int)((GEO_tdstVisualSet *)p)->xNbLodDefinitions;
}

/* 0x0042b2b0 */
unsigned long __cdecl GLI_hGetRLI(void *p, unsigned int idx)
{
    int iIdx = (int)(idx & 0xff);
    int iCount = ((GEO_tdstVisualSet *)p)->lNumberOfRLI;
    unsigned long *p_uArr;
    if (iIdx < iCount)
    {
        p_uArr = (unsigned long *)((GEO_tdstVisualSet *)p)->d_hRLI;
        return p_uArr[iIdx];
    }
    return 0;
}

/* 0x426B80 */
extern ACP_tdxBool HIE_pc_fn_bIntersectMatrixSegmentInCameraSector(POS_tdstCompletePosition *p_stMatA,
                                                                   POS_tdstCompletePosition *p_stMatB);

extern void __cdecl GLI_vDoMaterialSelection(struct GLI_tdstInternalGlobalValuesFor3dEngine_ *p_stGlobals);

#include "GEO/GeoSprit.h"
#include "GMT/GmtFunct.h"

/* 0x00428F10 */
void __cdecl GLI_pc_fn_vDrawSpriteFlashQuad(unsigned long ulArg1,
                                             unsigned long ulArg2,
                                             int *p_lSprite,
                                             struct GLI_tdstInternalGlobalValuesFor3dEngine_ *p_stGlobals)
{
    ACP_tdxHandleOfSprite pObj;
    float         *pVec;
    int            lKey;
    signed char    cIdx;
    unsigned long  ulFrame;
    unsigned long  ulIdxU;
    unsigned long  ulRes;
    unsigned char  cFlagNew;
    unsigned char  cFlagStored;
    float          xW;
    float          xSx;
    float          xSy;
    float          xScX;
    float          xScY;
    float          xAlpha;
    float         *pPosX;
    float         *pPosY;
    float          aQuad[16];
    short          sIdx;
    int            iState;
    int            iSlot;
    ACP_tdxHandleOfMaterial hMaterial;
    unsigned long  ulFlagsG;
    int            bSkipStore;

    (void)ulArg1;
    (void)ulArg2;

    lKey =GLI_lLensFlareExist(GLI_pc_g_p_vLensFlareRef);
    cIdx = (signed char)lKey;
    ulFrame = GAM_pc_fn_ulGetEngineTrameNumber();
    ulIdxU = (unsigned long)(long)cIdx;
    bSkipStore = 0;

    if (((ulFrame ^ ulIdxU) & 1UL) == 0) {
        if (cIdx < 0) return;
        if (ulFrame - (unsigned long)GLI_pc_g_stLensFlareTab.lApparitionTrame[ulIdxU] < 8UL) {
            cFlagNew = GLI_pc_g_stLensFlareTab.bIntersect[ulIdxU];
            bSkipStore = 1;
        } else {
            ulRes = (unsigned long)HIE_pc_fn_bIntersectMatrixSegmentInCameraSector(
                        p_stGlobals->gs_st_CameraMatrix,
                        p_stGlobals->gs_st_CurrentMatrix);
            cFlagNew = (unsigned char)ulRes;
            xW = (float)(ulRes & 0xffUL);
            if ((float)(int)(signed char)GLI_pc_g_stLensFlareTab.bIntersect[ulIdxU] != xW) {
                GLI_pc_g_stLensFlareTab.lCounter[ulIdxU] = 0;
            }
        }
        if (!bSkipStore) {
            GLI_pc_g_stLensFlareTab.bIntersect[ulIdxU] = cFlagNew;
        }
    } else {
        ulRes = (unsigned long)HIE_pc_fn_bIntersectMatrixSegmentInCameraSector(
                    p_stGlobals->gs_st_CameraMatrix,
                    p_stGlobals->gs_st_CurrentMatrix);
        cFlagNew = (unsigned char)ulRes;
        xW = (float)(ulRes & 0xffUL);
        if (cIdx < 0) {
            bSkipStore = 1;
        } else {
            if ((float)(int)(signed char)GLI_pc_g_stLensFlareTab.bIntersect[ulIdxU] != xW) {
                GLI_pc_g_stLensFlareTab.lCounter[ulIdxU] = 0;
            }
            GLI_pc_g_stLensFlareTab.bIntersect[ulIdxU] = cFlagNew;
        }
    }
    cFlagStored = cFlagNew;

    xW = 1.0f;
    if (cFlagStored == 0) {
        iSlot = GLI_cRegisterLensFlare(GLI_pc_g_p_vLensFlareRef);
        GLI_pc_g_stLensFlareTab.lApparitionTrame[iSlot] = (long)ulFrame;
        iState = GLI_pc_g_stLensFlareTab.lCounter[iSlot];
        if (iState < 4) {
            GLI_pc_g_stLensFlareTab.lCounter[iSlot] = iState + 1;
            xW = (float)iState * 0.125f;
        }
    } else {
        if (cIdx < 0) return;
        if (7UL < ulFrame - (unsigned long)GLI_pc_g_stLensFlareTab.lApparitionTrame[ulIdxU]) {
            GLI_vUnregisterLensFlare(GLI_pc_g_p_vLensFlareRef);
            return;
        }
        xW = (float)(unsigned long)(8UL - (ulFrame - (unsigned long)GLI_pc_g_stLensFlareTab.lApparitionTrame[ulIdxU])) * 0.125f;
    }

    pObj = ((GEO_tdstIndexedSprite *)p_lSprite)->hSprite;
    pVec = &pObj->d_xSizeOfSprite->xX;
    xSx = pVec[0];
    xSy = -pVec[1];
    if (3UL < p_stGlobals->gs_st_CurrentMatrixMultiplyedWithCameraMatrix.ulType) {
        xScX = POS_fn_xGetXScale(&p_stGlobals->gs_st_CurrentMatrixMultiplyedWithCameraMatrix);
        xScY = POS_fn_xGetYScale(&p_stGlobals->gs_st_CurrentMatrixMultiplyedWithCameraMatrix);
        xSx = xScX * pVec[0];
        xSy = -pVec[1] * xScY;
    }

    sIdx = (short)((GEO_tdstIndexedSprite *)p_lSprite)->xCenterPoint;
    pPosX = &p_stGlobals->GLI_ScreenPoint[sIdx].xX;
    pPosY = &p_stGlobals->GLI_ScreenPoint[sIdx].xY;

    aQuad[0]  = xSx * xW + *pPosX;
    aQuad[1]  = xSy * xW + *pPosY;
    aQuad[4]  = -xSx * xW + *pPosX;
    aQuad[5]  = xSy * xW + *pPosY;
    aQuad[8]  = -xSx * xW + *pPosX;
    aQuad[9]  = *pPosY + -xSy * xW;
    aQuad[12] = xSx * xW + *pPosX;
    aQuad[13] = *pPosY + -xSy * xW;

    if (xW < 100.0f) {
        xAlpha = GLI_fn_xGetGlobalAlpha();
        GLI_vSetGlobalAlpha((float)(xAlpha * xW));
    }

    hMaterial = GMT_fn_hGetVisualMaterial(*pObj->d_hMaterial);
    ulFlagsG = (unsigned long)p_stGlobals->lHierachDrawMask;
    p_stGlobals->hCurrentMaterial = hMaterial;
    p_stGlobals->lHierachDrawMask = (long)(ulFlagsG & 0xfffffbffUL);
    p_stGlobals->ulSpecularColorForSprite = ((unsigned long *)p_stGlobals->GLI_aDEF_stColorsRLIS)[sIdx];
    GLI_vDoMaterialSelection(p_stGlobals);
    (*GLI_DRV_vSendSpriteToClip_)((struct GLI_tdstAligned2DVector_ *)aQuad, 1.0f, p_stGlobals);
    GLI_pc_g_p_vLensFlareRef = NULL;
    p_stGlobals->lHierachDrawMask = (long)ulFlagsG;
}

#include "GLD/GLD_Pub.h"

/* 0x004291E0 */
void __cdecl GLI_vDrawIndexedSprite(void *p_stVpt,
                                        unsigned long ulArg2,
                                        int *p_lSprite,
                                        struct GLI_tdstInternalGlobalValuesFor3dEngine_ *p_stGlobals)
{
    ACP_tdxHandleOfSprite pObj;
    float         *pVec;
    int            iIdx16;
    short          sIdx;
    GEO_tdstIndexedSprite *p_stISpr;
    float          aCorners[12];
    float          aProj[12];
    float          aQuad[16];
    float          xScX;
    float          xScY;
    float          xZ;
    float          xPosX;
    float          xPosY;
    float          xA0;
    float          xA4;
    float          xV94;
    float          xV90;
    ACP_tdxHandleOfMaterial hMaterial;
    unsigned long  ulFlagsG;
    void          *pCam;

    p_stISpr = (GEO_tdstIndexedSprite *)p_lSprite;
    pObj = p_stISpr->hSprite;
    sIdx = (short)p_stISpr->xCenterPoint;
    iIdx16 = (int)sIdx * 0x10;

    if (pObj != 0 && 0.0f <= p_stGlobals->GLI_ScreenPoint[sIdx].xOoZ &&
        (*(unsigned char *)pObj->d_xSpriteDrawMode & GEO_C_lSpriteDrawDisable) == 0) {
        pVec = &pObj->d_xSizeOfSprite->xX;
        xA4 = pVec[0];
        xA0 = 0.0f;
        xV94 = 0.0f;
        xV90 = -pVec[1];

        if ((*(unsigned char *)pObj->d_xSpriteDrawMode & GEO_C_lSpriteDrawModeLensFlare) != 0) {
            GLI_pc_fn_vDrawSpriteFlashQuad((unsigned long)p_stVpt, ulArg2, p_lSprite, p_stGlobals);
            return;
        }

        if (3UL < p_stGlobals->gs_st_CurrentMatrixMultiplyedWithCameraMatrix.ulType) {
            xScX = POS_fn_xGetXScale(&p_stGlobals->gs_st_CurrentMatrixMultiplyedWithCameraMatrix);
            xScY = POS_fn_xGetYScale(&p_stGlobals->gs_st_CurrentMatrixMultiplyedWithCameraMatrix);
            pVec = &p_stISpr->hSprite->d_xSizeOfSprite->xX;
            xA4 = xScX * pVec[0];
            xV90 = (-pVec[1]) * xScY;
        }

        xZ = p_stGlobals->GLI_TurnedScaledDisplecedPoint[sIdx].xZ;
        aCorners[0] = xA4;
        aCorners[1] = xA0;
        aCorners[2] = xZ;
        aCorners[4] = xV94;
        aCorners[5] = xV90;
        aCorners[6] = xZ;
        aCorners[8] = 0.0f;
        aCorners[9] = 0.0f;
        aCorners[10] = xZ;

        if ((*(unsigned char *)pObj->d_xSpriteDrawMode & GEO_C_lSpriteDrawMode2DScaled) == 0) {
            xPosX = p_stGlobals->GLI_ScreenPoint[sIdx].xX;
            xPosY = p_stGlobals->GLI_ScreenPoint[sIdx].xY;
            aQuad[0]  = (xA4 + xV94) + xPosX;
            aQuad[1]  = (xA0 + xV90) + xPosY;
            aQuad[4]  = (xPosX - xA4) + xV94;
            aQuad[5]  = (xPosY - xA0) + xV90;
            aQuad[8]  = (xPosX - xA4) - xV94;
            aQuad[9]  = (xPosY - xA0) - xV90;
            aQuad[12] = (xA4 + xPosX) - xV94;
            aQuad[13] = (xA0 + xPosY) - xV90;
        } else {
            pCam = *(void **)((struct GLD_tdstViewportAttributes_ *)p_stVpt)->p_vSpecificToXD;
            GLI_xSerialProjectionNoTrans(pCam, 3, aCorners, aProj);
            xPosX = p_stGlobals->GLI_ScreenPoint[sIdx].xX;
            xPosY = p_stGlobals->GLI_ScreenPoint[sIdx].xY;
            aQuad[0]  = ((xPosX - aProj[0]) - aProj[4]) + aProj[8];
            aQuad[1]  = ((xPosY - aProj[1]) - aProj[5]) + aProj[9];
            aQuad[4]  = ((aProj[0] + xPosX) - aProj[4]) + aProj[8];
            aQuad[5]  = ((aProj[1] + xPosY) - aProj[5]) + aProj[9];
            aQuad[8]  = aProj[0] + aProj[4] + aProj[8] + xPosX;
            aQuad[9]  = aProj[1] + aProj[5] + aProj[9] + xPosY;
            aQuad[12] = ((xPosX - aProj[0]) + aProj[4]) + aProj[8];
            aQuad[13] = ((xPosY - aProj[1]) + aProj[5]) + aProj[9];
        }

        hMaterial = GMT_fn_hGetVisualMaterial(*pObj->d_hMaterial);
        ulFlagsG = (unsigned long)p_stGlobals->lHierachDrawMask;
        p_stGlobals->hCurrentMaterial = hMaterial;
        p_stGlobals->lHierachDrawMask = (long)(ulFlagsG & 0xfffffbffUL);
        p_stGlobals->ulSpecularColorForSprite = ((unsigned long *)p_stGlobals->GLI_aDEF_stColorsRLIS)[sIdx];
        GLI_vDoMaterialSelection(p_stGlobals);
        (*GLI_DRV_vSendSpriteToClip_)((struct GLI_tdstAligned2DVector_ *)aQuad,
                                      p_stGlobals->GLI_ScreenPoint[sIdx].xOoZ,
                                      p_stGlobals);
        p_stGlobals->lHierachDrawMask = (long)ulFlagsG;
    }
}
