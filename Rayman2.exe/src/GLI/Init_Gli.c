#include "GLI/Cache.h"
#include "GLI/GLI_globals.h"
#include "GLI/Init_Gli.h"
#include "GLI/Init_Gli_helpers.h"
#include "GLI/Material.h"
#include "GLI/MatrixStack.h"
#include "GLI/RasterDefaults.h"
#include "GAM/GAM_types.h"
#include "GAM/GAM_pc.h"
#include "TMP/TmpMem.h"

#include "GLI/GlobalAlpha.h"
#include "GLI/Proj.h"
#include "GLI/Texture.h"
#include "GLI/cluster3_texture_table.h"
#include "GLI/cpu_timer.h"
#include "ERM/CpaError.h"
#include "GLI/GLD.h"
#include "GLI/Memory.h"
#include "GLI/texture_lifecycle.h"
#include "GLI/devinfo.h"

#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <string.h>
#include "GLI/GLI_pc.h"

/* 0x00403290 */
extern void __cdecl SND_PrintUsedStaticMemory(void);

/* 0x504840 */
typedef void (__cdecl *pfn_GLI_DRV_xInitDriver_t)(HWND, DWORD, DWORD, DWORD, DWORD);
/* 0x00504840 */
static pfn_GLI_DRV_xInitDriver_t GLI_g_pfn_xInitDriver_storage;
#define GLI_g_pfn_xInitDriver  (GLI_g_pfn_xInitDriver_storage)

HMODULE GLI_g_hDll;

typedef long (__cdecl *pfn_SetCommonData_t)(char const *, void *);

pfn_SetCommonData_t GLI_g_pfn_SetCommonData;

struct GLI_pc_tdstTexelPackSrc;
void __cdecl GLD_fn_vBlitStretched16b(long lWDst, long lHDst, unsigned short *pusBufferDst, long lPitch, long lWSrc, long lHSrc, unsigned short *pusBufferSrc);
void __cdecl GLI_pc_fn_vPackColourKeyTexels(struct GLI_pc_tdstTexelPackSrc *p_st, unsigned long *pulOut);
void __cdecl GLI_pc_fn_vCompressTex565(struct GLI_pc_tdstTexelPackSrc *p_st, unsigned long *pulOut);
void __cdecl GLI_pc_fn_vCompressTex4444(struct GLI_pc_tdstTexelPackSrc *p_st, unsigned long *pulOut);
void __cdecl GLI_pc_fn_vCompressTex1555(struct GLI_pc_tdstTexelPackSrc *p_st, unsigned long *pulOut);

typedef long (__cdecl *pfn_GetDllInfo_t)(char const *, void *);
typedef long (__cdecl *pfn_SetCommonFct_t)(char const *, void *);

void * __cdecl GLI_pc_fn_p_vTmpMallocSlot(uint32_t ulSize)
{
    return GLI_pc_fn_p_vAlloc(ulSize);
}

long __cdecl GLI_fnl_InitDll(void)
{
    pfn_GetDllInfo_t  pfn_GetDllInfo;
    pfn_SetCommonFct_t pfn_SetCommonFct;

    GLI_g_hDll = GLI_pc_M_hLoadDriver((LPCSTR)GLI_pc_g_szDllFileName);
    if (GLI_g_hDll == NULL)
        return 0;

    pfn_GetDllInfo = (pfn_GetDllInfo_t)GLI_pc_M_pfnGetDriverProc(GLI_g_hDll,"GLI_DRV_lGetDllInfo");
    if (pfn_GetDllInfo == NULL)
        return 0;

    pfn_GetDllInfo("Name",        (void *)GLI_pc_g_szDllInfoScratch);
    if (strcmp((char *)GLI_pc_g_szDllInfoScratch, (char *)GLI_pc_g_szConfiguredDllName) != 0)
        return 0;

    pfn_GetDllInfo("Description", (void *)GLI_pc_g_szDllInfoScratch);
    pfn_GetDllInfo("Version",     (void *)GLI_pc_g_szDllVersion);
    pfn_GetDllInfo("Code",        (void *)GLI_pc_g_szDllCode);

    GLI_g_pfn_SetCommonData = (pfn_SetCommonData_t)GLI_pc_M_pfnGetDriverProc(GLI_g_hDll,"GLI_DRV_lSetCommonData");

    /* 0x005036ac */
    GLI_g_pfn_SetCommonData("GliBigGlobals",           (void *)GLI_BIG_GLOBALS);
    /* 0x00501660 */
    GLI_g_pfn_SetCommonData("TextureMemoryChannels",   (void *)GLI_gs_aDEFTableOfTextureMemoryChannels);
    /* 0x00502680 */
    GLI_g_pfn_SetCommonData("TextureAlreadyRead",      (void *)GLI_gs_aDEFTableOfTextureAlreadyRead);
    /* 0x00503d50 */
    GLI_g_pfn_SetCommonData("ConvertBufferMipMapping", GLI_gs_p_ConvertBufferMipMapping);
    /* 0x004a7208 */
    {
        extern unsigned long GLD_g_ulRenderCounter7208_storage;
        GLI_g_pfn_SetCommonData("RenderingCounterPointer", (void *)&GLD_g_ulRenderCounter7208_storage);
    }

    GLI_g_pfn_SetCommonData("DrawSem",                 (void *)fn_hGetDrawSemaphore());

    GLI_g_pfn_SetCommonData("GliCaps",                 (void *)&GLI_pc_g_stGliCaps);
    /* 0x00501644 */
    GLI_g_pfn_SetCommonData("NumberOfTextureToCreate", (void *)&GLI_gs_lNumberOfTextureToCreate);

    pfn_SetCommonFct = (pfn_SetCommonFct_t)GLI_pc_M_pfnGetDriverProc(GLI_g_hDll,"GLI_DRV_lSetCommonFct");

#  define GLI_CB(reimpl, rawaddr)  ((void *)&(reimpl))

    pfn_SetCommonFct("SetGlobalAlpha",                   GLI_CB(GLI_vSetGlobalAlpha, 0x0042a300));
    pfn_SetCommonFct("SerialProjection",                 GLI_CB(GLI_xSerialProjection, 0x0042c8a0));
    pfn_SetCommonFct("IsGliInit",                        GLI_CB(GLI_xIsGliInit, 0x00421810));
    pfn_SetCommonFct("TmpMalloc",                        GLI_CB(GLI_pc_fn_p_vTmpMallocSlot, 0x00422050)); /* 0x422050 */
    pfn_SetCommonFct("TmpFree",                          GLI_CB(GLI_pc_v_vTmpFree, 0x00419650));
    pfn_SetCommonFct("ErmPrintMessage",                  GLI_CB(Erm_fn_v_PrintErrMsg, 0x00403290));
    pfn_SetCommonFct("ComputeTextureSize",               GLI_CB(FUN_0042c6e0, 0x0042c6e0));
    pfn_SetCommonFct("CompressTex565",                   (void *)&GLI_pc_fn_vCompressTex565);
    pfn_SetCommonFct("CompressTex4444",                  (void *)&GLI_pc_fn_vCompressTex4444);
    pfn_SetCommonFct("CompressTex1555",                  (void *)&GLI_pc_fn_vCompressTex1555);
    pfn_SetCommonFct("DivideBitmapSurfaceBy2",           GLI_CB(GLI_DivideBitmapSurfaceBy2, 0x00425c90));
    pfn_SetCommonFct("DivideBitmapSurfaceBy2NonZero",    GLI_CB(GLI_DivideBitmapSurfaceBy2NonZero, 0x00425cd0));
    pfn_SetCommonFct("DivideBitmapSurfaceBy2Palette",    GLI_CB(GLI_DivideBitmapSurfaceBy2Palette, 0x00425d10));
    pfn_SetCommonFct("VerifyTextureIsReallyAlpha",       GLI_CB(GLI_pc_ulVerifyTextureIsReallyAlpha, 0x0042c250));
    pfn_SetCommonFct("LoadTextureInTexelField",          GLI_CB(GLI_xLoadTextureInTexelField, 0x00426550));
    pfn_SetCommonFct("ReloadTextures",                   GLI_CB(GLI_vReloadTextures, 0x00426a40)); /* 0x426a00 */
    pfn_SetCommonFct("AddDisplayMode",                   GLI_CB(GLI_pc_fn_vAddDisplayMode, 0x004220a0));
    pfn_SetCommonFct("BlitStretched16b",                 (void *)&GLD_fn_vBlitStretched16b);
    pfn_SetCommonFct("GetDefinedPixelFormat",            GLI_CB(GLD_fn_p_stGetDefinedPixelFormat, 0x0042b6f0));
    pfn_SetCommonFct("AddDefinedPixelFormat",            GLI_CB(GLD_fn_lAddDefinedPixelFormat, 0x0042b710));
    pfn_SetCommonFct("ComputeWaitFrameForSmoothSynchro", GLI_CB(GLD_pc_lComputeWaitFrameForSmoothSynchro, 0x00420e10));
    pfn_SetCommonFct("SimulateFlipSynchroWithoutVSync",  GLI_CB(GLD_pc_vSimulateFlipSynchroWithoutVSync, 0x00420ec0));
    pfn_SetCommonFct("ReadIniString",                    GLI_CB(GLI_pc_fn_vReadIniString, 0x00405930));
    pfn_SetCommonFct("WriteIniString",                   GLI_CB(GLI_pc_fn_vWriteIniString, 0x004059b0));
    pfn_SetCommonFct("TimerGetCounter",                  GLI_CB(fn_vTimerGetCounter, 0x0045e930));
    pfn_SetCommonFct("TimerTickPerSecond",               GLI_CB(fn_ulTimerTickPerSecond, 0x0045e8a0));
    pfn_SetCommonFct("AddToProgressBar",                 GLI_CB(GLI_AddToProgressBar, 0x0045f7e0));

#undef GLI_CB

    *(FARPROC *)GLI_DRV_vDoOpaqueTextureSelection_PTR = GLI_pc_M_pfnGetDriverProc(GLI_g_hDll,"GLI_DRV_vDoOpaqueTextureSelection");
    *(FARPROC *)GLI_DRV_vWrite16bBitmapToBackBuffer = GLI_pc_M_pfnGetDriverProc(GLI_g_hDll,"GLI_DRV_vWrite16bBitmapToBackBuffer");
    *(FARPROC *)GLI_DRV_vAddBlackPolygon = GLI_pc_M_pfnGetDriverProc(GLI_g_hDll,"GLI_DRV_vAddBlackPolygon");
    *(FARPROC *)GLI_DRV_vNoBlackPolygon = GLI_pc_M_pfnGetDriverProc(GLI_g_hDll,"GLI_DRV_vNoBlackPolygon");
    *(FARPROC *)GLI_DRV_vComputeFogEffect_PTR = GLI_pc_M_pfnGetDriverProc(GLI_g_hDll,"GLI_DRV_vComputeFogEffect");
    *(FARPROC *)GLI_DRV_bBeginScene = GLI_pc_M_pfnGetDriverProc(GLI_g_hDll,"GLI_DRV_bBeginScene");
    *GLI_DRV_bEndScene_              = (int  (*)(void))                                            GLI_pc_M_pfnGetDriverProc(GLI_g_hDll,"GLI_DRV_bEndScene");
    *GLI_DRV_vSendSpriteToClip_      = (void (*)(struct GLI_tdstAligned2DVector_ *, MTH_tdxReal, struct GLI_tdstInternalGlobalValuesFor3dEngine_ *))
                                       GLI_pc_M_pfnGetDriverProc(GLI_g_hDll,"GLI_DRV_vSendSpriteToClip");
    *GLI_DRV_vSendSpriteToClipWithUV_= (void (*)(struct GLI_tdstAligned2DVector_ *, MTH_tdxReal *, MTH_tdxReal, struct GLI_tdstInternalGlobalValuesFor3dEngine_ *))
                                       GLI_pc_M_pfnGetDriverProc(GLI_g_hDll,"GLI_DRV_vSendSpriteToClipWithUV");
    *(FARPROC *)GLI_DRV_vSendSpriteToClipWithColors_ = GLI_pc_M_pfnGetDriverProc(GLI_g_hDll,"GLI_DRV_vSendSpriteToClipWithColors");
    *GLI_DRV_vSendSingleLineToClip_  = (void (*)(struct GLD_tdstViewportAttributes *, struct GLI_tdstAligned3DVector_ *, struct GLI_tdstAligned2DVector_ *, struct GLI_tdstAligned3DVector_ *, struct GLI_tdstAligned2DVector_ *, struct GLI_tdstInternalGlobalValuesFor3dEngine_ *, long, struct GEO_tdstColor *))
                                       GLI_pc_M_pfnGetDriverProc(GLI_g_hDll,"GLI_DRV_vSendSingleLineToClip");
    *(FARPROC *)GLI_DRV_vDownLoadTextures = GLI_pc_M_pfnGetDriverProc(GLI_g_hDll,"GLI_DRV_vDownLoadTextures");
    *(FARPROC *)GLI_DRV_vUnLoadTextures = GLI_pc_M_pfnGetDriverProc(GLI_g_hDll,"GLI_DRV_vUnLoadTextures");
    *(FARPROC *)GLI_DRV_lGetSizeOfTexture = GLI_pc_M_pfnGetDriverProc(GLI_g_hDll,"GLI_DRV_lGetSizeOfTexture");
    *GLI_DRV_vFlipDevice             = (void (*)(void)) GLI_pc_M_pfnGetDriverProc(GLI_g_hDll,"GLI_DRV_vFlipDevice");
    *GLI_DRV_vFlipDeviceWithSyncro_  = (void (*)(void)) GLI_pc_M_pfnGetDriverProc(GLI_g_hDll,"GLI_DRV_vFlipDeviceWithSyncro");
    *(FARPROC *)GLI_DRV_vClearDevice = GLI_pc_M_pfnGetDriverProc(GLI_g_hDll,"GLI_DRV_vClearDevice");
    *(FARPROC *)&GLI_g_pfn_xInitDriver = GLI_pc_M_pfnGetDriverProc(GLI_g_hDll,"GLI_DRV_xInitDriver");
    *(FARPROC *)GLI_DRV_vCloseDriver = GLI_pc_M_pfnGetDriverProc(GLI_g_hDll,"GLI_DRV_vCloseDriver");
    *(FARPROC *)GLI_DRV_vClearZBufferRegion = GLI_pc_M_pfnGetDriverProc(GLI_g_hDll,"GLI_DRV_vClearZBufferRegion");
    *(FARPROC *)GLI_DRV_fnl_EnumModes = GLI_pc_M_pfnGetDriverProc(GLI_g_hDll,"GLI_DRV_fnl_EnumModes");
    *(FARPROC *)GLI_DRV_hChangeMode = GLI_pc_M_pfnGetDriverProc(GLI_g_hDll,"GLI_DRV_hChangeMode");
    *(FARPROC *)GLI_DRV_bLockDevice = GLI_pc_M_pfnGetDriverProc(GLI_g_hDll,"GLI_DRV_bLockDevice");
    *(FARPROC *)GLI_DRV_bUnlockDevice = GLI_pc_M_pfnGetDriverProc(GLI_g_hDll,"GLI_DRV_bUnlockDevice");
    *(FARPROC *)GLI_DRV_bPrepareForGliWindowed = GLI_pc_M_pfnGetDriverProc(GLI_g_hDll,"GLI_DRV_bPrepareForGliWindowed");
    *(FARPROC *)GLI_DRV_vPrepareForGliFullScreen = GLI_pc_M_pfnGetDriverProc(GLI_g_hDll,"GLI_DRV_vPrepareForGliFullScreen");
    *(FARPROC *)GLI_DRV_vActivateGli = GLI_pc_M_pfnGetDriverProc(GLI_g_hDll,"GLI_DRV_vActivateGli");
    *GLI_DRV_vReadaptDisplay_        = (void (*)(void)) GLI_pc_M_pfnGetDriverProc(GLI_g_hDll,"GLI_DRV_vReadaptDisplay");
    *(FARPROC *)GLI_DRV_bWindowedModeIsOptimized = GLI_pc_M_pfnGetDriverProc(GLI_g_hDll,"GLI_DRV_bWindowedModeIsOptimized");
    *(FARPROC *)GLI_DRV_vOptimizedWindowedMode = GLI_pc_M_pfnGetDriverProc(GLI_g_hDll,"GLI_DRV_vOptimizedWindowedMode");
    *(FARPROC *)GLI_DRV_vNonOptimizedWindowedMode = GLI_pc_M_pfnGetDriverProc(GLI_g_hDll,"GLI_DRV_vNonOptimizedWindowedMode");
    *(FARPROC *)GLI_DRV_xClearViewingList = GLI_pc_M_pfnGetDriverProc(GLI_g_hDll,"GLI_DRV_xClearViewingList");
    *GLI_DRV_xSendListToViewport_    = (void (*)(struct GLD_tdstViewportAttributes *)) GLI_pc_M_pfnGetDriverProc(GLI_g_hDll,"GLI_DRV_xSendListToViewport");
    *(FARPROC *)GLI_DRV_vSetZClip_PTR = GLI_pc_M_pfnGetDriverProc(GLI_g_hDll,"GLI_DRV_vSetZClip");
    *(FARPROC *)GLI_DRV_vSetClipWindow_PTR = GLI_pc_M_pfnGetDriverProc(GLI_g_hDll,"GLI_DRV_vSetClipWindow");
    *(FARPROC *)GLI_DRV_xSendElementTIToClip_TRIANGLES = GLI_pc_M_pfnGetDriverProc(GLI_g_hDll,"GLI_DRV_xSendElementTIToClip_TRIANGLES");

    *GLI_DRV_vSendSingleTrianglesToClip_ = (void (*)(struct GLI_tdstAligned2DVector_ *, struct GLI_tdst2DUVValues_ *, struct GLI_tdstInternalGlobalValuesFor3dEngine_ *))
                                           GLI_pc_M_pfnGetDriverProc(GLI_g_hDll,"GLI_DRV_xSendSingleTriangleToClip_TRIANGLES");

    *(FARPROC *)GLI_DRV_lRequest = GLI_pc_M_pfnGetDriverProc(GLI_g_hDll,"GLI_DRV_lRequest");

    return 1;
}

long g_lGliInit = 0;

long __cdecl GLI_xIsGliInit(void)
{
    return g_lGliInit;
}

void __cdecl GLI_vCloseGli(void)
{
    g_lGliInit = 0;
    ((void (__cdecl *)(void))GLI_DRV_g_vCloseDriver)();
}

extern void __cdecl SND_fn_vReleaseDriverSound(void);
extern void __cdecl GAM_pc_fn_vPostWindowCmd(unsigned short);   /* 0x00401650 */

extern void (__cdecl *_imp__exit)(int iStatus);
#define GLI_RT_exit (*_imp__exit)

void __cdecl GLI_FatalExitOrSleep(void)
{

    if (*(long *)GAM_pc_g_hEngineThread_ADDR != -1) {
        if (GLI_xIsGliInit() != 0) {
            GLI_vCloseGli();
        }
        SND_fn_vReleaseDriverSound();
        GAM_pc_fn_vPostWindowCmd(1);             /* 0x00401650 */
        SetEvent(*g_hEngineEventIsDead);
        Sleep(0xFFFFFFFFu);
        return;
    }
    GLI_RT_exit(-2);
}

static long GLI_g_bFirstInitPending_storage = 1;   /* 0x0049FD0C */
#define GLI_g_bFirstInitPending  GLI_g_bFirstInitPending_storage

#define GLI_K_xRefreshRateMin  (40.0f)
#define GLI_K_xRefreshRateMax  (210.0f)

#include "GLI/GLI_BigGlobals.h"

void __cdecl GLI_xInitGli(void)
{

    g_lGliInit = 0;

    if (GLI_g_bFirstInitPending != 0) {
        GLI_g_bFirstInitPending = 0;

        GLI_BIG_GLOBALS = (struct GLI_tdstInternalGlobalValuesFor3dEngine_ *)GLI_pc_fn_p_vAlloc(0x2d40c);   /* 0x0045dc60 */

        GLI_BIG_GLOBALS->GLD_RenderingCounter   = -1;
        GLI_BIG_GLOBALS->p_stActiveFog          = 0;
        GLI_BIG_GLOBALS->p_stLastComputedFog    = 0;
        GLI_BIG_GLOBALS->xFogIsOn               = 0;
        GLI_BIG_GLOBALS->lAMirrorIsDetected     = 0;
        GLI_BIG_GLOBALS->bDrawMirrorSymetric    = 0;
        GLI_BIG_GLOBALS->lDisplayMenuIsDetected = 0;
        GLI_BIG_GLOBALS->xGlobalAlpha           = 0.0f;

        GLI_g_pfn_SetCommonData("GliBigGlobals", GLI_BIG_GLOBALS);

        GLI_xCreateMaterial((void **)GLI_pc_p_hDefaultMaterial);
        GLI_pc_fn_vEmptyTextureCache();
        SND_PrintUsedStaticMemory();
        FUN_00423ab0();
        SND_PrintUsedStaticMemory();
        GLI_vInitLensFlare();
        GLI_xInitMatrixStack();
        SND_PrintUsedStaticMemory();
    }

    GLI_BIG_GLOBALS->p_TheZListe = 0;
    GLI_pc_g_stGliCaps.xRefreshRate = 0.0f;

    GLI_g_pfn_xInitDriver((HWND)GAM_fn_hGetWindowHandle(), GLI_fn_lIsFullScreenMode(),
                          GLI_fn_lDisplayModeWidth(), GLI_fn_lDisplayModeHeight(), FUN_00422090());

    if ((GLI_pc_g_stGliCaps.xRefreshRate < GLI_K_xRefreshRateMin) ||
        !(GLI_pc_g_stGliCaps.xRefreshRate < GLI_K_xRefreshRateMax)) {
        long lCapsFlags;
        lCapsFlags = GLI_pc_g_stGliCaps.ulDriverCaps;
        lCapsFlags |= GLI_pc_C_ulDrvCapsRefreshRateDefaulted;
        GLI_pc_g_stGliCaps.xRefreshRate = 60.0f;
        GLI_pc_g_stGliCaps.ulDriverCaps = (tdeDriverCaps)lCapsFlags;
    }

    {
        __int64 i64Ticks = (__int64)(unsigned __int64)fn_ulTimerTickPerSecond(1);
        double  dBudget  = (double)i64Ticks / (double)GLI_pc_g_stGliCaps.xRefreshRate;
        GLI_pc_g_lTickBudget = (long)dBudget;   /* 0x004a7218 */
    }

    FUN_00409c80();

    g_lGliInit = 1;
}

/* 0x0042BC20 */
void __cdecl GLD_fn_vBlitStretched16b(long lWDst, long lHDst, unsigned short *pusBufferDst, long lPitch, long lWSrc, long lHSrc, unsigned short *pusBufferSrc)
{
    long            alYMap[0x800];
    long            alXMap[0x800];
    long            lSum;
    long            lCount;
    long            lQuot;
    long           *plMap;
    long           *plRow;
    long            lRow;
    long            lCol;
    long            lBase;

    if (0 < lWDst) {
        lSum = 0;
        plMap = alXMap;
        lCount = lWDst;
        do {
            lQuot = lSum / lWDst;
            lSum += lWSrc;
            *plMap = lQuot;
            lCount--;
            plMap++;
        } while (lCount != 0);
    }
    if (0 < lHDst) {
        lSum = 0;
        plMap = alYMap;
        lCount = lHDst;
        do {
            lQuot = lSum / lHDst;
            lSum += lHSrc;
            *plMap = lQuot * lWSrc;
            lCount--;
            plMap++;
        } while (lCount != 0);

        plRow = alYMap;
        lRow = lHDst;
        do {
            if (0 < lWDst) {
                lBase = *plRow;
                plMap = alXMap;
                lCol = lWDst;
                do {
                    *pusBufferDst = pusBufferSrc[lBase + *plMap];
                    pusBufferDst++;
                    plMap++;
                    lCol--;
                } while (lCol != 0);
            }
            pusBufferDst += lPitch - lWDst;
            plRow++;
            lRow--;
        } while (lRow != 0);
    }
}

#define GLI_pc_M_ulPackTexel(x) \
    ((((x) & 0xfc00ul) + ((x) & 0xf8ul) * 0x2000ul) * 0x800ul + (((x) >> 3) & 0x1f0000ul))

typedef struct GLI_pc_tdstTexelPackSrc {
    unsigned char    aucPad0[8];
    unsigned long   *pulTexelPairs;
    unsigned char    aucPad1[0x14 - 0x0C];
    unsigned char    ucFlags;
    unsigned char    aucPad1b[0x18 - 0x15];
    short            sSizeA;
    unsigned short   usSizeB;
    unsigned char    aucPad2[0x2C - 0x1C];
    unsigned long    ulKeyColour;
} GLI_pc_tdstTexelPackSrc;

void __cdecl GLI_pc_fn_vCompressTex565(GLI_pc_tdstTexelPackSrc *p_st, unsigned long *pulOut)
{
    unsigned long  ulFirst;
    unsigned long  ulSecond;
    unsigned long  ulCount;
    unsigned long  ulIdx;
    unsigned long *pulPair;

    if ((p_st->ucFlags & 0x2) != 0) {
        GLI_pc_fn_vPackColourKeyTexels(p_st, pulOut);
        return;
    }
    if (p_st->usSizeB == 1 && p_st->sSizeA == 1) {
        ulFirst = p_st->pulTexelPairs[0];
        *pulOut = GLI_pc_M_ulPackTexel(ulFirst) >> 16;
        return;
    }

    ulCount = (unsigned long)(((long)((unsigned long)p_st->usSizeB * (unsigned long)(unsigned short)p_st->sSizeA)) >> 1);
    ulIdx = 0;
    if (ulCount != 0) {
        do {
            pulPair = p_st->pulTexelPairs + ulIdx * 2;
            ulFirst = pulPair[0];
            ulSecond = pulPair[1];
            *pulOut = GLI_pc_M_ulPackTexel(ulSecond) + (GLI_pc_M_ulPackTexel(ulFirst) >> 16);
            pulOut++;
            ulIdx++;
        } while (ulIdx < ulCount);
    }
}

void __cdecl GLI_pc_fn_vPackColourKeyTexels(GLI_pc_tdstTexelPackSrc *p_st, unsigned long *pulOut)
{
    unsigned long  ulKey;
    unsigned long  ulKeyPacked;
    unsigned long  ulFirst;
    unsigned long  ulSecond;
    unsigned long  ulPackFirst;
    unsigned long  ulPackSecond;
    unsigned long  ulCount;
    unsigned long  ulIdx;
    unsigned long *pulPair;

    if (p_st->usSizeB == 1 && p_st->sSizeA == 1) {
        ulFirst = p_st->pulTexelPairs[0];
        *pulOut = GLI_pc_M_ulPackTexel(ulFirst) >> 16;
        return;
    }

    ulKey       = p_st->ulKeyColour;
    ulKeyPacked = GLI_pc_M_ulPackTexel(ulKey);
    ulCount     = (unsigned long)(((long)((unsigned long)p_st->usSizeB * (unsigned long)p_st->sSizeA)) >> 1);
    ulIdx       = 0;

    if (ulCount != 0) {
        do {
            pulPair = p_st->pulTexelPairs + ulIdx * 2;
            ulFirst = pulPair[0];
            ulPackFirst = GLI_pc_M_ulPackTexel(ulFirst);
            if (ulPackFirst == ulKeyPacked && ulFirst != ulKey) {
                ulPackFirst ^= 0x200000ul;
            }
            ulSecond = pulPair[1];
            ulPackSecond = GLI_pc_M_ulPackTexel(ulSecond);
            if (ulPackSecond == ulKeyPacked && ulSecond != ulKey) {
                ulPackSecond ^= 0x200000ul;
            }
            pulOut[ulIdx] = ulPackSecond + (ulPackFirst >> 16);
            ulIdx++;
        } while (ulIdx < ulCount);
    }
}

#define GLI_pc_M_ulPack4444Texel(x) \
    ((((x) & 0xf000ul) + ((x) & 0xf0ul) * 0x1000ul) * 0x100ul + ((x) & 0xf0000000ul) + (((x) >> 4) & 0xf0000ul))

void __cdecl GLI_pc_fn_vCompressTex4444(GLI_pc_tdstTexelPackSrc *p_st, unsigned long *pulOut)
{
    unsigned long  ulFirst;
    unsigned long  ulSecond;
    unsigned long  ulIdx;
    unsigned long *pulPair;

    ulIdx = 0;
    if ((((long)((unsigned long)(unsigned short)p_st->sSizeA * (unsigned long)p_st->usSizeB)) >> 1) != 0) {
        do {
            pulPair = p_st->pulTexelPairs + ulIdx * 2;
            ulFirst = pulPair[0];
            ulSecond = pulPair[1];
            *pulOut = GLI_pc_M_ulPack4444Texel(ulSecond) + (GLI_pc_M_ulPack4444Texel(ulFirst) >> 16);
            pulOut++;
            ulIdx++;
        } while (ulIdx < (unsigned long)(((long)((unsigned long)(unsigned short)p_st->sSizeA * (unsigned long)p_st->usSizeB)) >> 1));
    }
}

#define GLI_pc_M_ulPack1555Texel(x) \
    ((((x) & 0xf800ul) + ((x) & 0xf8ul) * 0x2000ul) * 0x400ul + ((x) & 0x80000000ul) + (((x) >> 3) & 0x1f0000ul))

void __cdecl GLI_pc_fn_vCompressTex1555(GLI_pc_tdstTexelPackSrc *p_st, unsigned long *pulOut)
{
    unsigned long  ulFirst;
    unsigned long  ulSecond;
    unsigned long  ulCount;
    unsigned long  ulIdx;
    unsigned long *pulPair;

    if (p_st->usSizeB == 1 && p_st->sSizeA == 1) {
        ulFirst = p_st->pulTexelPairs[0];
        *pulOut = GLI_pc_M_ulPack1555Texel(ulFirst) >> 16;
        return;
    }

    ulCount = (unsigned long)(((long)((unsigned long)p_st->usSizeB * (unsigned long)(unsigned short)p_st->sSizeA)) >> 1);
    ulIdx = 0;
    if (ulCount != 0) {
        do {
            pulPair = p_st->pulTexelPairs + ulIdx * 2;
            ulFirst = pulPair[0];
            ulSecond = pulPair[1];
            *pulOut = GLI_pc_M_ulPack1555Texel(ulSecond) + (GLI_pc_M_ulPack1555Texel(ulFirst) >> 16);
            pulOut++;
            ulIdx++;
        } while (ulIdx < ulCount);
    }
}
