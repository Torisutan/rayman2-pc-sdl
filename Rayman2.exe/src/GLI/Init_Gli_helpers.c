#include "GLI/Init_Gli_helpers.h"
#include "GLI/GLI_globals.h"
#include "GAM/GAM_types.h"
#include "GAM/GAM_pc.h"
#include "GLI/cpu_timer.h"
#include "GLI/GLD.h"
#include "GLD/GLD_Pub.h"

#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <ctype.h>
#include <stdio.h>
#include <string.h>
#include "FIL/FIL_pc.h"

#include "GLI/sprite.h"
void __cdecl GLI_vInitLensFlare (void)
{
    GLI_pc_g_stLensFlareTab.lNumberOfLensFlare = 0;
}

typedef struct GLI_pc_tdstStandardDisplayMode_
{
    unsigned char ucEnabled;
    unsigned char aucPad1[3];
    long          lModeIndex;
} GLI_pc_tdstStandardDisplayMode;

typedef struct GLI_pc_tdstDisplayMode_
{
    unsigned long ulFullScreen;
    unsigned long ulWidth;
    unsigned long ulHeight;
    unsigned long ulBpp;
} GLI_pc_tdstDisplayMode;

static long          GLI_pc_g_lCurrentModeIndex;
static GLI_pc_tdstStandardDisplayMode GLI_pc_a_stStandardDisplayMode[3];   /* 0x00504300 */

static GLI_pc_tdstDisplayMode GLI_pc_a_stDisplayMode[36];   /* 0x00504320 */

/* 0x00422060 */
unsigned long __cdecl GLI_fn_lIsFullScreenMode(void)
{
    return GLI_pc_a_stDisplayMode[GLI_pc_g_lCurrentModeIndex].ulFullScreen;
}

unsigned long __cdecl GLI_fn_lDisplayModeWidth(void)
{
    return GLI_pc_a_stDisplayMode[GLI_pc_g_lCurrentModeIndex].ulWidth;
}

unsigned long __cdecl GLI_fn_lDisplayModeHeight(void)
{
    return GLI_pc_a_stDisplayMode[GLI_pc_g_lCurrentModeIndex].ulHeight;
}

unsigned long __cdecl FUN_00422090(void)
{
    return GLI_pc_a_stDisplayMode[GLI_pc_g_lCurrentModeIndex].ulBpp;
}

void __cdecl GLI_pc_fn_vAddDisplayMode(long lModeId, long lWidth, long lHeight, long lBpp)
{
    GLI_pc_tdstDisplayMode         *p_stTable = GLI_pc_a_stDisplayMode;
    GLI_pc_tdstStandardDisplayMode *p_stStd   = GLI_pc_a_stStandardDisplayMode;
    long           lIndex   = 0;
    int            bDid640or800 = 0;

    while (lIndex < (long)GLI_pc_g_ulNbDisplayModes
           && (lWidth > (long)p_stTable[lIndex].ulWidth
               || (lWidth == (long)p_stTable[lIndex].ulWidth && lHeight > (long)p_stTable[lIndex].ulHeight))) {
        lIndex++;
    }

    if (lIndex != (long)GLI_pc_g_ulNbDisplayModes) {
        memmove(&p_stTable[lIndex + 1], &p_stTable[lIndex],
                (size_t)((GLI_pc_g_ulNbDisplayModes - lIndex) * sizeof(GLI_pc_tdstDisplayMode)));
    }

    if (lWidth == 0x280 && lHeight == 0x1E0 && lBpp == 0x10) {
        int bFlag800 = (p_stStd[1].ucEnabled != 0);
        p_stStd[0].ucEnabled = 1;
        p_stStd[0].lModeIndex = lIndex;
        if (bFlag800 && p_stStd[1].lModeIndex >= lIndex) {
            p_stStd[1].lModeIndex += 1;
        }
        bDid640or800 = 1;
    } else if (lWidth == 0x320 && lHeight == 0x258 && lBpp == 0x10) {
        p_stStd[1].ucEnabled = 1;
        p_stStd[1].lModeIndex = lIndex;
        bDid640or800 = 1;
    }

    if (bDid640or800) {

        if (p_stStd[2].ucEnabled != 0 && p_stStd[2].lModeIndex >= lIndex) {
            p_stStd[2].lModeIndex += 1;
        }
    } else if (lWidth == 0x400 && lHeight == 0x300 && lBpp == 0x10) {
        p_stStd[2].ucEnabled = 1;
        p_stStd[2].lModeIndex = lIndex;
    }

    GLI_pc_a_stDisplayMode[lIndex].ulFullScreen = (unsigned long)lModeId;
    GLI_pc_a_stDisplayMode[lIndex].ulWidth      = (unsigned long)lWidth;
    GLI_pc_a_stDisplayMode[lIndex].ulHeight     = (unsigned long)lHeight;
    GLI_pc_a_stDisplayMode[lIndex].ulBpp        = (unsigned long)lBpp;
    GLI_pc_g_ulNbDisplayModes++;
}

/* 0x004A0484 */
static long GLI_pc_gs_lDeferredModeSentinel = -1;

long __cdecl GLI_fn_lRequestNewDisplayMode(long lSlot)
{
    if (GLI_pc_gs_lDeferredModeSentinel != -1) {
        return 0;
    }
    if (GLI_pc_a_stStandardDisplayMode[lSlot].ucEnabled != 0 &&
        GLI_pc_a_stStandardDisplayMode[lSlot].lModeIndex == GLI_pc_g_lCurrentModeIndex) {
        return 1;
    }
    GLI_pc_gs_lDeferredModeSentinel = lSlot;

    GAM_pc_fn_vPostResumeAfterFocus();
}

long __cdecl GLI_pc_lFindDisplayMode(unsigned long ulWidth, unsigned long ulHeight, unsigned long ulBpp)
{
    GLI_pc_tdstDisplayMode *p_stTable = GLI_pc_a_stDisplayMode;
    long           lCount   = (long)GLI_pc_g_ulNbDisplayModes;
    long           lIndex;

    for (lIndex = 0; lIndex < lCount; lIndex++) {
        if (ulWidth  == p_stTable[lIndex].ulWidth &&
            ulHeight == p_stTable[lIndex].ulHeight &&
            ulBpp    == p_stTable[lIndex].ulBpp) {
            GLI_pc_g_lCurrentModeIndex = lIndex;
            return lIndex;
        }
    }
    return -1;
}

#include "ACP/ACP_types.h"
#include "GLI/VisuS_st.h"

struct GEO_tdstGeometricObject_ *__cdecl GLI_hGetVisualSetGeometricForADistance(GEO_tdstVisualSet *p_stVSE, MTH_tdxReal xThreshold)
{
    int i;
    int nKeys = (int)p_stVSE->xNbLodDefinitions;

    for (i = 0; i < nKeys - 1; i++) {
        if (p_stVSE->d_xThresholdsTable[i + 1] > xThreshold) {
            break;
        }
    }
    return p_stVSE->d_p_stLodDefinitions[i];
}

#  define GLI_PC_UBIINI_NAME  "\\Ubi.ini"

void __cdecl GLI_pc_fn_vReadIniString(const char *szSection, const char *szKey,
                                      unsigned long ulSize, char *szReturnBuf)
{
    char acPath[260];
#ifndef RAYMAN2_LOCAL_UBIINI
    GetWindowsDirectoryA(acPath, sizeof(acPath));
#else
    strcpy(acPath, ".");   /* ubi.ini in the working directory (the game folder) */
#endif /* RAYMAN2_LOCAL_UBIINI */
    strcat(acPath, GLI_PC_UBIINI_NAME);
    FIL_pc_M_ulGetProfileString(szSection, szKey, szReturnBuf, szReturnBuf, ulSize, acPath);
}

void __cdecl GLI_pc_fn_vWriteIniString(const char *szSection, const char *szKey,
                                       const char *szValue)
{
    char acPath[260];
#ifndef RAYMAN2_LOCAL_UBIINI
    GetWindowsDirectoryA(acPath, sizeof(acPath));
#else
    strcpy(acPath, ".");   /* ubi.ini in the working directory (the game folder) */
#endif /* RAYMAN2_LOCAL_UBIINI */
    strcat(acPath, GLI_PC_UBIINI_NAME);
    FIL_pc_M_bWriteProfileString(szSection, szKey, szValue, acPath);
}

static const char GLI_HELP_szDllFmt_storage[]      = "dll/%s%s.dll";
static const char GLI_HELP_szDllSuffix_storage[]   = "vf";
static const char GLI_HELP_szErrTitle_storage[]    = "%s setup error";
static const char GLI_HELP_szErrBodyProblem_storage[] =
    "Display initialisation error : problem during resolutions enumeration."
    "\n\nDriver : %s\nDevice : %s\n\nPlease run install.";
static const char GLI_HELP_szErrBodyCould_storage[] =
    "Display initialisation error : could not find any resolution."
    "\n\nDriver : %s\nDevice : %s\n\nPlease run install.";
#define GLI_HELP_szDllFmt          GLI_HELP_szDllFmt_storage
#define GLI_HELP_szDllSuffix       GLI_HELP_szDllSuffix_storage
#define GLI_HELP_szErrTitle        GLI_HELP_szErrTitle_storage
#define GLI_HELP_szErrBodyProblem  GLI_HELP_szErrBodyProblem_storage
#define GLI_HELP_szErrBodyCould    GLI_HELP_szErrBodyCould_storage

void __cdecl GLI_fnv_SetDllFileName(const char *szBaseName)
{
    sprintf((char *)GLI_pc_g_szDllFileName,
            GLI_HELP_szDllFmt,
            szBaseName,
            GLI_HELP_szDllSuffix);
}

long __cdecl GLI_fn_lChooseDisplayMode(void)
{
    long (__cdecl *pfnFindMode)(unsigned long, unsigned long, unsigned long) =
        GLI_pc_lFindDisplayMode;
    GLI_pc_g_lCurrentModeIndex = -1;
    GLI_pc_g_lCurrentModeIndex = pfnFindMode(GLI_pc_g_ulRequestedWidth,
                                       GLI_pc_g_ulRequestedHeight,
                                       GLI_pc_g_ulRequestedBpp);
    if (GLI_pc_g_lCurrentModeIndex == -1) {
        GLI_pc_g_lCurrentModeIndex = pfnFindMode(0x280, 0x1E0, 0x10);
        if (GLI_pc_g_lCurrentModeIndex == -1) {
            GLI_pc_g_lCurrentModeIndex = 0;
        }
    }
    return 1;
}

extern int __cdecl Erm_fn_iMessageBox(const char *, const char *, int);
long __cdecl GLI_pc_bInitDisplayModes(void)
{
    char acTitle[256];
    char acBody[1024];

    GLI_pc_g_ulNbDisplayModes = 0;        /* 0x004A7238 */
    GLI_pc_a_stStandardDisplayMode[0].ucEnabled = 0;   /* 0x504300 */
    GLI_pc_a_stStandardDisplayMode[1].ucEnabled = 0;   /* 0x504308 */
    GLI_pc_a_stStandardDisplayMode[2].ucEnabled = 0;   /* 0x504310 */

    /* 0x00504850 */
    if (!((long (__cdecl *)(char *, char *))*GLI_DRV_fnl_EnumModes)(
            (char *)GLI_pc_g_szEnumModesDriver, (char *)GLI_pc_g_szEnumModesDevice)) {

        sprintf(acTitle, GLI_HELP_szErrTitle,
                (const char *)GLI_pc_g_szConfiguredDllName);
        sprintf(acBody, GLI_HELP_szErrBodyProblem,
                (const char *)GLI_pc_g_szEnumModesDriver, (const char *)GLI_pc_g_szEnumModesDevice);
        Erm_fn_iMessageBox(acBody, acTitle, 0x40010);
        return 0;
    }

    if (GLI_pc_g_ulNbDisplayModes == 0) {   /* 0x004A7238 */

        sprintf(acTitle, GLI_HELP_szErrTitle,
                (const char *)GLI_pc_g_szConfiguredDllName);
        sprintf(acBody, GLI_HELP_szErrBodyCould,
                (const char *)GLI_pc_g_szEnumModesDriver, (const char *)GLI_pc_g_szEnumModesDevice);
        Erm_fn_iMessageBox(acBody, acTitle, 0x40010);
        return 0;
    }

    return 1;
}

extern void __cdecl GLI_vCloseGli(void);
extern void __cdecl GLI_xInitGli(void);
extern void __cdecl GLI_vComputeTextures(void);
extern long __cdecl GLD_fn_bChangeDeviceDisplayMode(short hDev);
extern void __cdecl GAM_fn_vActualizeAllGameViewportSize(void);
long __cdecl GLI_pc_fn_bApplyDeferredMode(void)
{
    int idx = GLI_pc_gs_lDeferredModeSentinel;            /* 0x004a0484 */
    if (idx == -1 || idx > 2)
        return 0;
    if (GLI_pc_a_stStandardDisplayMode[idx].ucEnabled == 0)
        return 0;
    GLI_pc_g_lCurrentModeIndex = GLI_pc_a_stStandardDisplayMode[idx].lModeIndex;
    ((void (__cdecl *)(void))&GLI_vCloseGli)();
    GLI_xInitGli();
    GLI_vComputeTextures();
    GLD_fn_bChangeDeviceDisplayMode(0);
    GAM_fn_vActualizeAllGameViewportSize();
    GLI_pc_gs_lDeferredModeSentinel = -1;                 /* 0x004a0484 */
    return 1;
}

/* 0x00420890 */
long __cdecl GLD_bGetDeviceAttributes(short hDev, void *pstDevAttrib);
/* 0x00420a80 */
long __cdecl GLD_bGetViewportAttributes(short hNotUsed, short hVP, void *pstViewAttrib);
/* 0x00422ab0 */
struct GLI_tdstCamera_;
void __cdecl GLI_xAdjustCameraToViewport2(void *p_stDev, void *p_stVpt, struct GLI_tdstCamera_ *p_stCam);

typedef struct GLD_tdstDeviceAttributes_
{
    DWORD           dwHeight;
    DWORD           dwWidth;
    float           xInvPixelDimensionX ;
    float           xInvPixelDimensionY ;
    char            *p_cVirtualScreen;
    LONG            lPitch;
    short           *p_sZBuffer;
    DWORD           dwFullScreenModeX;
    DWORD           dwFullScreenModeY;
    DWORD           dwFullScreenModeBpp;
    HWND            hFullScreenModeWnd;
    HWND            hNormalModeWnd;
    BOOL            bFullScreen;
    unsigned int    uiTypeDriver;
    POINT           pClientOnPrimary;
    RECT            rtClientRect;
    RECT            rtWindowRect;
} GLD_tdstDeviceAttributes;

static struct GLI_tdstCamera_ *p_stCamAntiBug;   /* 0x004A723C */

void GLI_xAdjustCameraToViewport( GLD_tdhDevice hDevice, GLD_tdhViewport hViewport, struct GLI_tdstCamera_ *p_stCam )
{
	GLD_tdstDeviceAttributes        stDevAttrib;
	struct GLD_tdstViewportAttributes_ stViewAttrib;

	if ( !GLD_bGetDeviceAttributes( hDevice, &stDevAttrib ) )
		return;

	if ( !GLD_bGetViewportAttributes( hDevice, hViewport, &stViewAttrib ) )
		return;

	if( p_stCam == NULL )
		p_stCam = p_stCamAntiBug;
	else
		p_stCamAntiBug = p_stCam;

	GLI_xAdjustCameraToViewport2( &stDevAttrib, &stViewAttrib, p_stCam );
}

/* 0x004221D0 */
long __cdecl pc_GLI_lIsResolutionAvailable(long lSlot)
{
    if (lSlot < 0 || lSlot > 2)
        return 0;
    return (char)GLI_pc_a_stStandardDisplayMode[lSlot].ucEnabled;
}

/* 0x004221F0 */
long __cdecl pc_GLI_lGetNbResolutions(void)
{
    return (long)GLI_pc_g_ulNbDisplayModes;
}

/* 0x00422200 */
long __cdecl pc_GLI_lGetCurrentResolution(void)
{
    long lSlot;

    for (lSlot = 0; lSlot < 3; lSlot++)
    {
        if (GLI_pc_a_stStandardDisplayMode[lSlot].ucEnabled != 0
            && GLI_pc_a_stStandardDisplayMode[lSlot].lModeIndex == GLI_pc_g_lCurrentModeIndex)
            return lSlot;
    }
    return 3;
}

/* 0x00422360 */
long __cdecl pc_GLI_lSaveVideoModeToConfig(void)
{
    char szMode[32];

    sprintf(szMode, "%d - %d x %d x %d", (int)GLI_fn_lIsFullScreenMode(), (int)GLI_fn_lDisplayModeWidth(), (int)GLI_fn_lDisplayModeHeight(), (int)FUN_00422090());
    GLI_pc_fn_vWriteIniString("Rayman2", "GLI_Mode", szMode);
    return 1;
}

/* 0x00422230 */
long __cdecl pc_GLI_bGetResolutionSize(long lMode, long *p_lWidth, long *p_lHeight)
{
    if (lMode < 0 || lMode >= (long)GLI_pc_g_ulNbDisplayModes)
        return 0;
    *p_lWidth  = GLI_pc_a_stDisplayMode[lMode].ulWidth;
    *p_lHeight = GLI_pc_a_stDisplayMode[lMode].ulHeight;
    return 1;
}

/* 0x00422270 */
long __cdecl pc_GLI_bGetResolutionName(long lMode, char *szName)
{
    long lWidth, lHeight;

    if (!pc_GLI_bGetResolutionSize(lMode, &lWidth, &lHeight))
        return 0;
    sprintf(szName, "%dx%d", (int)lWidth, (int)lHeight);
    return 1;
}
