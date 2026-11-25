#include "GLI/GLD.h"
#include "GLI/Camera.h"
#include "GLI/Memory.h"
#include "GLI/GLI_globals.h"
#include "GAM/GAM_types.h"
#include "MMG/MMG_pubaccess.h"
#include "MMG/MMG_alloc.h"
#include "ERM/CpaError.h"
#include "TMR/TMR.h"
#include "GLI/cpu_timer.h"

#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include "GLD/GLD_Pub.h"
#include "GLD/Specif/DevVpt.h"
#include "GAM/GAM_pc_DrawLock.h"

static void *GLD_pc_gs_a_pDeviceTable[16];
#define GLD_pc_g_a_pDeviceTable (GLD_pc_gs_a_pDeviceTable)

/* 0x0049FC30 */
static unsigned char g_ucGLDModuleId_storage = 0xFF;
#define g_ucGLDModuleId  g_ucGLDModuleId_storage

static unsigned long GLD_pc_g_ulFirstTimeInit_storage = 1;   /* 0x0049FC44 */
#define GLD_pc_g_ulFirstTimeInit  GLD_pc_g_ulFirstTimeInit_storage

static unsigned short GLD_pc_g_wTimerHandle_storage = 0xBEEFu;
static float         GLD_pc_g_xTickToMsScale_storage;
static unsigned long GLD_pc_g_ulFrameTimeAcc722c_storage;
static unsigned long GLD_pc_g_ulFrameTimeAcc7230_storage;
#define GLD_pc_g_wTimerHandle        GLD_pc_g_wTimerHandle_storage
#define GLD_pc_g_xTickToMsScale      GLD_pc_g_xTickToMsScale_storage
#define GLD_pc_g_ulFrameTimeAcc722c  GLD_pc_g_ulFrameTimeAcc722c_storage
#define GLD_pc_g_ulFrameTimeAcc7230  GLD_pc_g_ulFrameTimeAcc7230_storage

/* 0x0049FC54 */
static const char GLD_g_szDriverVersion_storage[] = "V5.9.0 - August 11th 1997";
#define GLD_g_szDriverVersion  GLD_g_szDriverVersion_storage

static unsigned long GLD_g_ulDrawSync720C_storage;   /* 0x4A720C */
static unsigned long GLD_g_ulDrawSync7210_storage;   /* 0x4A7210 */
unsigned long        GLD_g_ulRenderCounter7208_storage; /* 0x4A7208 */
#define GLD_ADDR_720C  (&GLD_g_ulDrawSync720C_storage)
#define GLD_ADDR_7210  (&GLD_g_ulDrawSync7210_storage)
#define GLD_ADDR_7208  (&GLD_g_ulRenderCounter7208_storage)

#include "GLI/devinfo.h"

/* 0x004A7220 */
static unsigned char GLD_pc_g_ucOptionIndex_storage;
static unsigned char GLD_pc_g_ucOptionPercent_storage = 70;
#define GLD_pc_g_ucOptionIndex    GLD_pc_g_ucOptionIndex_storage
#define GLD_pc_g_ucOptionPercent  GLD_pc_g_ucOptionPercent_storage

int __cdecl _stricmp(const char *szA, const char *szB);
int __cdecl atoi(const char *szText);

void __cdecl GLD_vSetFrameSynchro(const char *szKey, const char *szValue, const char *szPercent)
{
    int iValue;

    if (_stricmp(szKey, "OFF") == 0) {
        GLD_pc_g_ucOptionIndex = 0xFF;
    }
    else {
        if (_stricmp(szKey, "ON") == 0) {
            iValue = atoi(szValue);
            if (iValue >= 0 && iValue <= 7) {
                GLD_pc_g_ucOptionIndex = (unsigned char)iValue;
            }
        }
    }
    iValue = atoi(szPercent);
    if (iValue >= 0 && iValue <= 0x64) {
        GLD_pc_g_ucOptionPercent = (unsigned char)iValue;
    }
}

BOOL __cdecl GLD_bIsDeviceHandleValid(short hDev)
{
    if (hDev >= 0 && hDev < 0x10) {
        return GLD_pc_g_a_pDeviceTable[hDev] != 0;
    }
    return FALSE;
}

void * __cdecl GLD_pGetViewport(short hDev, short hOldVP)
{
    struct GLD_stDevice *p_stDevice = ((struct GLD_stDevice **)GLD_pc_g_a_pDeviceTable)[hDev];
    return p_stDevice->pstViewports[hOldVP];
}

void * __cdecl GLD_pc_pGetDevice(short hDev)
{
    return ((void **)GLD_pc_g_a_pDeviceTable)[hDev];
}

long __cdecl GLD_bWriteToViewportFinished(short hDev_unused, short hVpPacked)
{
    int iPacked = (int)hVpPacked;
    short hDev = (short)(iPacked / 16);
    void *p_stDevice = GLD_pc_pGetDevice(hDev);
    ((struct GLD_stDevice *)p_stDevice)->lWriteInProgress = 0;
    (void)hDev_unused;
    return 1;
}

long __cdecl GLD_bClearDeviceWithChoice(short hDev_unused, short choice)
{

    {
        unsigned long ulBusy = *(volatile unsigned long *)GLD_ADDR_720C;
        *(volatile unsigned long *)GLD_ADDR_7210 = 1;
        if (ulBusy != 0) {
            for (;;) {   }
        }
    }
    {
        (*(void (__cdecl **)(int, int, int))GLI_DRV_vClearDevice)(
            (int)(choice & 2), (int)(choice & 1), 0);
    }
    *(volatile unsigned long *)GLD_ADDR_7210 = 0;
    (void)hDev_unused;
    return 1;
}

/* 0x004A721C */
static unsigned long GLD_pc_g_ulViewportClearValue;
#define GLD_PC_VIEWPORT_CLEAR_VALUE  GLD_pc_g_ulViewportClearValue

/* 0x00421620 */
long __cdecl GLD_bSetBackgroundColorForDevice(GLD_tdhDevice hDev,
                                              unsigned long crRGBA)
{
    (void)hDev;
    GLD_PC_VIEWPORT_CLEAR_VALUE = crRGBA;
    return 1;
}

long __cdecl GLD_bClearViewportWithChoice(GLD_tdhDevice hDev_unused,
                                          GLD_tdhViewport hViewport_unused,
                                          short choice)
{
    (*(void (__cdecl **)(int, int, int))GLI_DRV_vClearDevice)(
        (int)(choice & 2), (int)(choice & 1), (int)GLD_PC_VIEWPORT_CLEAR_VALUE);
    (void)hDev_unused;
    (void)hViewport_unused;
    return 1;
}

long __cdecl GLD_bClearDevice(short hDev)
{
    return GLD_bClearDeviceWithChoice(*(unsigned long *)&hDev, 3);
}

long __cdecl GLD_bSetFullScreenModeForDevice(GLD_tdhDevice hDev,
                                          DWORD ulWidth,
                                          DWORD ulHeight,
                                          DWORD ulBpp,
                                          HWND hWnd)
{
    struct GLD_stDevice *p_stDevice = (struct GLD_stDevice *)GLD_pc_pGetDevice((short)hDev);
    if (!p_stDevice) {
        return 0;
    }
    p_stDevice->stDevAttrib.dwFullScreenModeX = ulWidth;
    p_stDevice->stDevAttrib.dwFullScreenModeY = ulHeight;
    p_stDevice->stDevAttrib.dwFullScreenModeBpp = ulBpp;
    p_stDevice->stDevAttrib.hFullScreenModeWnd = hWnd;
    return 1;
}

long __cdecl GLD_bSwapToWindowedMode(short hDev, int bRebuildMode)
{

    if (*(volatile unsigned long *)GLD_ADDR_7210 != 0) {
        for (;;) {   }
    }
    *(volatile unsigned long *)GLD_ADDR_720C = 1;

    {
        GLD_tdstDevice *p_stDev = (GLD_tdstDevice *)GLD_pc_pGetDevice((short)hDev);
        if (!p_stDev) {
            *(volatile unsigned long *)GLD_ADDR_720C = 0;
            return 0;
        }
        p_stDev->stDevAttrib.bFullScreen = 1;

        if (bRebuildMode == 0) {
            (*(void (__cdecl **)(HWND, int))&GLI_DRV_g_vActivateGli)(p_stDev->stDevAttrib.hNormalModeWnd, 1);
            *(volatile unsigned long *)GLD_ADDR_720C = 0;
            return 1;
        }

        {
            long lResult;
            (*(void (__cdecl **)(HWND))&GLI_DRV_g_vPrepareForGliFullScreen)(p_stDev->stDevAttrib.hFullScreenModeWnd);
            lResult = GLD_pc_bRecreateDevice(hDev, (unsigned long *)p_stDev);
            *(volatile unsigned long *)GLD_ADDR_720C = 0;
            return lResult;
        }
    }
}

long __cdecl GLD_bSwapDeviceMode(short hDev, int bRebuildMode)
{
    if (*(volatile unsigned long *)GLD_ADDR_7210 != 0) {
        for (;;) {   }
    }
    *(volatile unsigned long *)GLD_ADDR_720C = 1;

    {
        GLD_tdstDevice *p_stDev = (GLD_tdstDevice *)GLD_pc_pGetDevice((short)hDev);
        if (!p_stDev) {
            *(volatile unsigned long *)GLD_ADDR_720C = 0;
            return 0;
        }
        p_stDev->stDevAttrib.bFullScreen = 0;

        if (bRebuildMode == 0) {
            (*(void (__cdecl **)(HWND, int))&GLI_DRV_g_vActivateGli)(p_stDev->stDevAttrib.hNormalModeWnd, 0);
            *(volatile unsigned long *)GLD_ADDR_720C = 0;
            return 1;
        }

        {
            long lPrepOk = (*(long (__cdecl **)(HWND))&GLI_DRV_g_bPrepareForGliWindowed)(p_stDev->stDevAttrib.hNormalModeWnd);
            if (lPrepOk == 0) {
                *(volatile unsigned long *)GLD_ADDR_720C = 0;
                return 0;
            }
            {
                long lResult = GLD_pc_bRecreateDevice(hDev, (unsigned long *)p_stDev);
                *(volatile unsigned long *)GLD_ADDR_720C = 0;
                return lResult;
            }
        }
    }
}

typedef struct { unsigned long a[0x18]; } GLD_pc_tdstDevAttrCopy;

long __cdecl GLD_bGetDeviceAttributes(short hDev, void *pstDevAttrib)
{
    GLD_pc_tdstDevAttrCopy *p_stDev;

    *(volatile unsigned long *)GLD_ADDR_7210 = 1;
    if (*(volatile unsigned long *)GLD_ADDR_720C != 0) {
        for (;;) {   }
    }
    p_stDev = (GLD_pc_tdstDevAttrCopy *)GLD_pc_pGetDevice(hDev);
    if (p_stDev == NULL) {
        return 0;
    }
    *(GLD_pc_tdstDevAttrCopy *)pstDevAttrib = *p_stDev;
    *(volatile unsigned long *)GLD_ADDR_7210 = 0;
    return 1;
}

typedef struct { unsigned long a[0x1c]; } GLD_pc_tdstVptAttrCopy;

long __cdecl GLD_bGetViewportAttributes(GLD_tdhDevice hNotUsed, GLD_tdhViewport hVP, void *pstViewAttrib)
{
    short hDev;
    short hOldVP;
    GLD_pc_tdstVptAttrCopy *pstView;

    *(volatile unsigned long *)GLD_ADDR_7210 = 1;
    if (*(volatile unsigned long *)GLD_ADDR_720C != 0) {
        for (;;) {   }
    }
    hDev = (short)(hVP / 16);
    GLD_pc_pGetDevice(hDev);
    hOldVP = (short)(hVP % 16);
    pstView = (GLD_pc_tdstVptAttrCopy *)GLD_pGetViewport(hDev, hOldVP);
    pstView->a[0x19] = (unsigned long)-1;
    pstView->a[0x18] = 0;
    *(GLD_pc_tdstVptAttrCopy *)pstViewAttrib = *pstView;
    *(volatile unsigned long *)GLD_ADDR_7210 = 0;
    return 1;
}

long __cdecl GLD_bReleaseViewportAndDevice(short hDev, short hVP)
{

    GLD_bDeleteViewport(*(unsigned long *)&hDev, *(unsigned long *)&hVP);

    {
        unsigned long ulBusy = *(volatile unsigned long *)GLD_ADDR_720C;
        *(volatile unsigned long *)GLD_ADDR_7210 = 1;
        if (ulBusy != 0) {
            for (;;) {   }
        }
    }

    GLD_bDeleteDevice((short)hDev);
    return 1;
}

long __cdecl GLD_bDeleteViewport(short hDev_unused, short hVpPacked)
{
    int           iSignedPacked;
    short         sDev;
    short         sVp;
    int          *pDevice;
    int          *pViewport;
    int          *p_iScribble;
    int           iScribbleCount;

    *(volatile unsigned long *)GLD_ADDR_7210 = 1;
    if (*(volatile unsigned long *)GLD_ADDR_720C != 0) {
        for (;;) {   }
    }

    iSignedPacked = (int)(short)hVpPacked;
    sDev          = (short)(iSignedPacked / 16);
    pDevice       = (int *)GLD_pc_pGetDevice(sDev);

    sVp           = (short)(iSignedPacked % 16);
    pViewport = (int *)GLD_pGetViewport(sDev, sVp);

    p_iScribble = pViewport;
    for (iScribbleCount = 0x1d; iScribbleCount != 0; iScribbleCount--) {
        *p_iScribble = (int)0xCCCCCCCCu;
        p_iScribble++;
    }

    {
        DWORD (__cdecl *pfnChannelForTid)(DWORD, char) =
            (DWORD (__cdecl *)(DWORD, char))Mmg_fn_ucGiveChannelId;

        Mmg_fn_v_SetModeAlloc(g_ucGLDModuleId, 0xff, pfnChannelForTid(GetCurrentThreadId(), 0));
        fn_vGenFree(pViewport, pfnChannelForTid(GetCurrentThreadId(), 0));
    }

    ((struct GLD_stDevice *)pDevice)->pstViewports[sVp] = 0;

    *(volatile unsigned long *)GLD_ADDR_7210 = 0;
    (void)hDev_unused;
    return 1;
}

long __cdecl GLD_bDeleteDevice(short hDev)
{
    int          *pDevice;
    int          *pSlot;
    int           iSlotCount;
    int          *p_iScribble;
    int           iScribbleCount;

    pDevice = (int *)GLD_pc_pGetDevice(hDev);

    if (*(volatile unsigned long *)GLD_ADDR_720C == 0) {
        pSlot = pDevice + 0x18;
        iSlotCount = 0x10;
        do {
            if (*pSlot != 0) {
                int *pVp = (int *)*pSlot;
                p_iScribble = pVp;
                for (iScribbleCount = 0x1d; iScribbleCount != 0; iScribbleCount--) {
                    *p_iScribble = (int)0xCCCCCCCCu;
                    p_iScribble++;
                }
                *pSlot = 0;
            }
            pSlot++;
            iSlotCount--;
        } while (iSlotCount != 0);
    }

    p_iScribble = pDevice;
    for (iScribbleCount = 0x30; iScribbleCount != 0; iScribbleCount--) {
        *p_iScribble = (int)0xCCCCCCCCu;
        p_iScribble++;
    }

    {
        DWORD (__cdecl *pfnChannelForTid)(DWORD, char) =
            (DWORD (__cdecl *)(DWORD, char))Mmg_fn_ucGiveChannelId;

        Mmg_fn_v_SetModeAlloc(g_ucGLDModuleId, 0xff, pfnChannelForTid(GetCurrentThreadId(), 0));
        fn_vGenFree(pDevice, pfnChannelForTid(GetCurrentThreadId(), 0));
    }

    ((int *)GLD_pc_g_a_pDeviceTable)[hDev] = 0;
    return 1;
}

typedef struct GLD_tdstViewportAttributes_ GLD_tdstViewportAttributes;

static void fn_vUpdateViewportSizesInPixels( GLD_tdstDevice *p_stDev, GLD_tdstViewport *p_stVP )
{

    int    iLeft, iTop, iRight, iBottom, iClipLeft, iClipRight, iClipTop, iClipBottom, iWidth, iHeight;
    unsigned long ulWidthQ, ulHeightQ;

    ulWidthQ = (p_stVP->stViewAttrib.dwWidthInPercent * (p_stDev->stDevAttrib.dwWidth-1)) / 100;
    ulHeightQ = (p_stVP->stViewAttrib.dwHeightInPercent * (p_stDev->stDevAttrib.dwHeight-1)) / 100;
    iWidth = 1 + ulWidthQ;
    iHeight = 1 + ulHeightQ;

    p_stVP->stViewAttrib.dwWidth  = iWidth;
    p_stVP->stViewAttrib.dwHeight = iHeight;

    p_stVP->stViewAttrib.dwTopInPixForClip = 0;
    p_stVP->stViewAttrib.dwLeftInPixForClip = 0;
    p_stVP->stViewAttrib.dwBottomInPixForClip = iHeight - 1;
    p_stVP->stViewAttrib.dwRightInPixForClip = iWidth - 1;

    iLeft = p_stVP->stViewAttrib.dwLeftInPixForClip + p_stVP->stViewAttrib.iOffsetPosX;
    iRight = p_stVP->stViewAttrib.dwRightInPixForClip + p_stVP->stViewAttrib.iOffsetPosX;
    iTop = p_stVP->stViewAttrib.dwTopInPixForClip + p_stVP->stViewAttrib.iOffsetPosY;
    iBottom = p_stVP->stViewAttrib.dwBottomInPixForClip + p_stVP->stViewAttrib.iOffsetPosY;

    if((iRight < 0) || (iLeft > (int) (p_stDev->stDevAttrib.dwWidth - 1)))
        p_stVP->stViewAttrib.dwLeftInPixForClip = p_stVP->stViewAttrib.dwRightInPixForClip = 0;
    else if((iBottom < 0) || (iTop > (int) (p_stDev->stDevAttrib.dwHeight-1)))
        p_stVP->stViewAttrib.dwTopInPixForClip = p_stVP->stViewAttrib.dwBottomInPixForClip = 0;
    else
    {
        if(iLeft < 0)
        {
            p_stVP->stViewAttrib.dwLeftInPixForClip = 0;
            p_stVP->stViewAttrib.dwRightInPixForClip = iRight;
        }
        else if(iRight > (int) (p_stDev->stDevAttrib.dwWidth - 1))
        {
            p_stVP->stViewAttrib.dwLeftInPixForClip = iLeft;
            p_stVP->stViewAttrib.dwRightInPixForClip = p_stDev->stDevAttrib.dwWidth - 1;
        }
        else
        {
            p_stVP->stViewAttrib.dwLeftInPixForClip = iLeft;
            p_stVP->stViewAttrib.dwRightInPixForClip = iRight;
        }
        if(iTop < 0)
        {
            p_stVP->stViewAttrib.dwTopInPixForClip = 0;
            p_stVP->stViewAttrib.dwBottomInPixForClip = iBottom;
        }
        else if(iBottom > (int) (p_stDev->stDevAttrib.dwHeight - 1))
        {
            p_stVP->stViewAttrib.dwTopInPixForClip = iTop;
            p_stVP->stViewAttrib.dwBottomInPixForClip = p_stDev->stDevAttrib.dwHeight - 1;
        }
        else
        {
            p_stVP->stViewAttrib.dwTopInPixForClip = iTop;
            p_stVP->stViewAttrib.dwBottomInPixForClip = iBottom;
        }
    }

    iClipTop = (iHeight * p_stVP->stViewAttrib.dwClipTopInPerMille) / 1000;
    iClipBottom = (iHeight * p_stVP->stViewAttrib.dwClipBottomInPerMille) / 1000;
    iClipLeft = (iWidth * p_stVP->stViewAttrib.dwClipLeftInPerMille) / 1000;
    iClipRight = (iWidth * p_stVP->stViewAttrib.dwClipRightInPerMille) / 1000;
    iClipTop += iTop;
    iClipBottom += iTop;
    iClipLeft += iLeft;
    iClipRight += iLeft;

    if(iClipLeft < 0)
        iClipLeft = 0;
    if(iClipTop < 0)
        iClipTop = 0;

    if(iClipRight > (int) (p_stDev->stDevAttrib.dwWidth))
        iClipRight = (p_stDev->stDevAttrib.dwWidth);
    if(iClipBottom > (int) (p_stDev->stDevAttrib.dwHeight))
        iClipBottom = (p_stDev->stDevAttrib.dwHeight);

    p_stVP->stViewAttrib.dwClipTopInPix = iClipTop;
    p_stVP->stViewAttrib.dwClipBottomInPix = iClipBottom;
    p_stVP->stViewAttrib.dwClipLeftInPix = iClipLeft;
    p_stVP->stViewAttrib.dwClipRightInPix = iClipRight;

    p_stVP->bIsLocked = 0;

    p_stVP->stViewAttrib.dwLeftInPix   = p_stVP->stViewAttrib.dwTopInPix = 0;
    p_stVP->stViewAttrib.dwRightInPix  = p_stVP->stViewAttrib.dwWidth;
    p_stVP->stViewAttrib.dwBottomInPix = p_stVP->stViewAttrib.dwHeight;

    (*(void (__cdecl **)(void))GLI_DRV_vNoBlackPolygon)();
    if (p_stVP->stViewAttrib.dwClipTopInPix > 0) {
        (*(void (__cdecl **)(int, int, int, int))GLI_DRV_vAddBlackPolygon)(
            0, 0, (int)p_stVP->stViewAttrib.dwWidth, (int)p_stVP->stViewAttrib.dwClipTopInPix);
    }
    if (p_stVP->stViewAttrib.dwClipBottomInPix < p_stVP->stViewAttrib.dwHeight) {
        (*(void (__cdecl **)(int, int, int, int))GLI_DRV_vAddBlackPolygon)(
            0, (int)p_stVP->stViewAttrib.dwClipBottomInPix,
            (int)p_stVP->stViewAttrib.dwWidth, (int)p_stVP->stViewAttrib.dwHeight);
    }
    if (p_stVP->stViewAttrib.dwClipLeftInPix > 0) {
        (*(void (__cdecl **)(int, int, int, int))GLI_DRV_vAddBlackPolygon)(
            0, 0, (int)p_stVP->stViewAttrib.dwClipLeftInPix, (int)p_stVP->stViewAttrib.dwHeight);
    }
    if (p_stVP->stViewAttrib.dwClipRightInPix < p_stVP->stViewAttrib.dwWidth) {
        (*(void (__cdecl **)(int, int, int, int))GLI_DRV_vAddBlackPolygon)(
            (int)p_stVP->stViewAttrib.dwClipRightInPix, 0,
            (int)p_stVP->stViewAttrib.dwWidth, (int)p_stVP->stViewAttrib.dwHeight);
    }
}

typedef struct { unsigned long a[0x10]; } GLD_pc_tdstVpSlotCopy;
extern void __cdecl SND_PrintUsedStaticMemory(void);
#define GLD_pc_fn_vDebugReport                                                     \
    ((void (__cdecl *)(const char *, const char *, const char *))SND_PrintUsedStaticMemory)

long __cdecl GLD_pc_bRecreateDevice(short hDev, unsigned long *p_ulDevice)
{
    GLD_pc_tdstDevAttrCopy stAttrib;
    GLD_pc_tdstVpSlotCopy  stVpSlots;
    unsigned long          ulSavedBc;
    unsigned long         *p_ulNew;
    unsigned long         *p_ulSlot;
    int                    iSlot;

    stAttrib  = *(GLD_pc_tdstDevAttrCopy *)p_ulDevice;
    stVpSlots = *(GLD_pc_tdstVpSlotCopy *)((GLD_tdstDevice *)p_ulDevice)->pstViewports;
    ulSavedBc = p_ulDevice[0x2f];

    if (GLD_bDeleteDevice(hDev) == 0) {
        *(volatile unsigned long *)GLD_ADDR_720C = 0;
        ERM_fn_vUpdateLastError(0x34, g_ucGLDModuleId, 0, 0x19, -1, 0xFF, 0xFF,
                                (char *)GLD_g_szDriverVersion);
        GLD_pc_fn_vDebugReport("GLD - DevVpt", " GLD_bSwapDeviceMode",
                               "GLD_bDeleteDevice failed");
        return 0;
    }
    if (GLD_bCreateDevice((GLD_tdstDeviceAttributes *)&stAttrib, &hDev) == 0) {
        *(volatile unsigned long *)GLD_ADDR_720C = 0;
        GLD_pc_fn_vDebugReport("GLD - DevVpt", " GLD_bSwapDeviceMode",
                               "GLD_bCreateDevice failed");
        return 0;
    }

    p_ulNew = (unsigned long *)GLD_pc_pGetDevice(hDev);
    *(GLD_pc_tdstVpSlotCopy *)((GLD_tdstDevice *)p_ulNew)->pstViewports = stVpSlots;
    p_ulSlot = (unsigned long *)(((GLD_tdstDevice *)p_ulNew)->pstViewports + 15);
    for (iSlot = 0x10; iSlot != 0; iSlot--) {
        if (*p_ulSlot != 0) {
            fn_vUpdateViewportSizesInPixels((GLD_tdstDevice *)p_ulNew,
                                            (GLD_tdstViewport *)(*p_ulSlot));
        }
        p_ulSlot--;
    }
    p_ulNew[0x2f] = ulSavedBc;
    ((void (__cdecl *)(unsigned long, unsigned long, unsigned long, unsigned long))
        GLI_DRV_g_hChangeMode)(0, 0, 0, 0);
    return 1;
}

/* 0x00421420 */
long __cdecl GLD_fn_bChangeDeviceDisplayMode(GLD_tdhDevice hDev)
{
    GLD_pc_tdstDevAttrCopy stAttrib;
    GLD_pc_tdstVpSlotCopy  stVpSlots;
    unsigned long          ulSavedBc;
    unsigned long         *p_ulDevice;
    unsigned long         *p_ulSlot;
    int                    iSlot;

    p_ulDevice = (unsigned long *)GLD_pc_pGetDevice(hDev);
    if (GLI_fn_lIsFullScreenMode() == 0 && ((GLD_tdstDevice *)p_ulDevice)->stDevAttrib.bFullScreen == 0)
        return 1;
    if (GLI_fn_lIsFullScreenMode() != 0 && ((GLD_tdstDevice *)p_ulDevice)->stDevAttrib.bFullScreen != 0 &&
        GLI_fn_lDisplayModeWidth() == ((GLD_tdstDevice *)p_ulDevice)->stDevAttrib.dwFullScreenModeX &&
        GLI_fn_lDisplayModeHeight() == ((GLD_tdstDevice *)p_ulDevice)->stDevAttrib.dwFullScreenModeY)
        return 1;

    stAttrib  = *(GLD_pc_tdstDevAttrCopy *)p_ulDevice;
    stVpSlots = *(GLD_pc_tdstVpSlotCopy *)((GLD_tdstDevice *)p_ulDevice)->pstViewports;
    ulSavedBc = p_ulDevice[0x2f];
    if (*(volatile unsigned long *)GLD_ADDR_7210 != 0) {
        for (;;) {   }
    }
    *(volatile unsigned long *)GLD_ADDR_720C = 1;
    ((GLD_tdstDeviceAttributes *)stAttrib.a)->dwFullScreenModeX = GLI_fn_lDisplayModeWidth();
    ((GLD_tdstDeviceAttributes *)stAttrib.a)->dwFullScreenModeY = GLI_fn_lDisplayModeHeight();

    if (GLD_bDeleteDevice(hDev) == 0) {
        *(volatile unsigned long *)GLD_ADDR_720C = 0;
        ERM_fn_vUpdateLastError(0x34, g_ucGLDModuleId, 0, 0x19, -1, 0xFF, 0xFF,
                                (char *)GLD_g_szDriverVersion);
        GLD_pc_fn_vDebugReport("GLD - DevVpt", " GLD_fn_bChangeDeviceDisplayMode",
                               "GLD_bDeleteDevice failed");
        return 0;
    }
    if (GLD_bCreateDevice((GLD_tdstDeviceAttributes *)&stAttrib, &hDev) == 0) {
        *(volatile unsigned long *)GLD_ADDR_720C = 0;
        GLD_pc_fn_vDebugReport("GLD - DevVpt", " GLD_fn_bChangeDeviceDisplayMode",
                               "GLD_bCreateDevice failed");
        return 0;
    }

    p_ulDevice = (unsigned long *)GLD_pc_pGetDevice(hDev);
    *(GLD_pc_tdstVpSlotCopy *)((GLD_tdstDevice *)p_ulDevice)->pstViewports = stVpSlots;
    p_ulSlot = (unsigned long *)(((GLD_tdstDevice *)p_ulDevice)->pstViewports + 15);
    for (iSlot = 0x10; iSlot != 0; iSlot--) {
        if (*p_ulSlot != 0) {
            fn_vUpdateViewportSizesInPixels((GLD_tdstDevice *)p_ulDevice,
                                            (GLD_tdstViewport *)(*p_ulSlot));
        }
        p_ulSlot--;
    }
    p_ulDevice[0x2f] = ulSavedBc;
    *(volatile unsigned long *)GLD_ADDR_720C = 0;
    return 1;
}

long __cdecl GLD_bSetViewportPosition(short hNotUsed, short hVP, void *pstViewAttrib)
{
    short hDev;
    short hOldVP;
    void *p_stDev;
    GLD_tdstViewport *pstView;

    *(volatile unsigned long *)GLD_ADDR_7210 = 1;
    if (*(volatile unsigned long *)GLD_ADDR_720C != 0) {
        for (;;) {   }
    }
    hDev = (short)(hVP / 16);
    p_stDev = GLD_pc_pGetDevice(hDev);
    hOldVP = (short)(hVP % 16);
    pstView = (GLD_tdstViewport *)GLD_pGetViewport(hDev, hOldVP);
    memset(pstView, 0, sizeof(GLD_tdstViewport));
    memcpy(&pstView->stViewAttrib, pstViewAttrib, sizeof(GLD_tdstViewportAttributes));
    fn_vUpdateViewportSizesInPixels((GLD_tdstDevice *)p_stDev, pstView);
    pstView->stViewAttrib.p_cVirtualScreen = NULL;
    pstView->stViewAttrib.lPitch = -1;
    memcpy(pstViewAttrib, &pstView->stViewAttrib, sizeof(GLD_tdstViewportAttributes));
    *(volatile unsigned long *)GLD_ADDR_7210 = 0;
    return 1;
}

/* 0x00420a40 */
long __cdecl fn_bGetVPHandle(GLD_tdhDevice hDev, GLD_tdhViewport *phVP)
{
    GLD_tdhViewport hVP;
    void          **pstDevice;

    pstDevice = (void **)GLD_pc_pGetDevice(hDev);

    for (hVP = 0; hVP < 16; hVP++)
    {
        if (!(pstDevice + 0x60 / 4)[hVP])
        {
            *phVP = hVP;
            return 1;
        }
    }

    *phVP = -1;
    return 0;
}

long __cdecl GLD_bCreateViewport(GLD_tdhDevice  hDev,
                                  unsigned long *p_ulViewAttribs,
                                  GLD_tdhViewport *p_hVP)
{

    void            *p_stDev;
    unsigned long   *pstView;
    GLD_tdhViewport  hOldVP;

    p_stDev = GLD_pc_pGetDevice((short)hDev);
    if (!fn_bGetVPHandle((short)hDev, &hOldVP)) {
        *p_hVP = -1;
        memset(p_ulViewAttribs, 0xCC, 0x70);
        ERM_fn_vUpdateLastError(0x34, g_ucGLDModuleId, 0, 0x20, -1, 0xFF, 0xFF,
                                (char *)GLD_g_szDriverVersion);   /* 0x49FC54 */
        return 0;
    }

    Mmg_fn_v_SetModeAlloc(g_ucGLDModuleId, 0xff, Mmg_fn_ucGiveChannelId(GetCurrentThreadId(), 0));

    pstView = (unsigned long *)((void *(__cdecl *)(unsigned long, unsigned char))fn_p_vGenAlloc)(
                  0x74, Mmg_fn_ucGiveChannelId(GetCurrentThreadId(), 0));
    if (!pstView) {
        *p_hVP = -1;
        memset(p_ulViewAttribs, 0xCC, 0x70);
        return 0;
    }

    memset(pstView, 0, 0x74);
    memcpy(pstView, p_ulViewAttribs, 0x70);
    fn_vUpdateViewportSizesInPixels((GLD_tdstDevice *)p_stDev, (GLD_tdstViewport *)pstView);

    pstView[0]    = pstView[2];
    pstView[1]    = pstView[3];
    pstView[0x1a] = 0;
    pstView[0x18] = 0;
    pstView[0x1c] = 0;
    pstView[0x19] = 0xFFFFFFFFu;
    memcpy(p_ulViewAttribs, pstView, 0x70);

    *(unsigned long **)(((unsigned char **)GLD_pc_g_a_pDeviceTable)[(short)hDev] + 0x60
                        + hOldVP * 4) = pstView;
    *p_hVP = (GLD_tdhViewport)((hDev << 4) + hOldVP);
    return 1;
}

void __cdecl GLD_pc_fn_vExitCallback(void)
{
    GLD_bReleaseViewportAndDevice(0, 0);
    exit(-4);
}

void __cdecl GLD_pc_fn_vRestartFrameTimer(void)
{
    if (GLD_pc_g_wTimerHandle == 0xBEEFu) {
        unsigned long ulFrequency;

        GLD_pc_g_wTimerHandle = TMR_fn_wCreateTimer(1);
        TMR_fn_wGetTimerFrequency(GLD_pc_g_wTimerHandle, &ulFrequency);

        GLD_pc_g_xTickToMsScale = 1000.0f / (float)ulFrequency;
    }

    GLD_pc_g_ulFrameTimeAcc7230 = 0;
    GLD_pc_g_ulFrameTimeAcc722c = 0;
    TMR_fn_wResetTimer((short)GLD_pc_g_wTimerHandle);
    TMR_fn_wStartTimer((short)GLD_pc_g_wTimerHandle);
}

long __cdecl GLD_pc_lComputeWaitFrameForSmoothSynchro(long lMode)
{
    stTimerCount  stCount;
    unsigned long ulBudget;
    unsigned long ulVal;
    unsigned long ulQuotient;

    TMR_fn_wReadTimer(GLD_pc_g_wTimerHandle, &stCount);

    if (stCount.m_ulHighPart == 0) {
        ulBudget = (unsigned long)GLI_pc_g_lTickBudget;
        if ((unsigned char)lMode != 0) {
            ulVal = stCount.m_ulLowPart + ulBudget;
        } else {
            ulVal = (ulBudget >> 1) + stCount.m_ulLowPart;
        }
        ulQuotient = ulVal / ulBudget;

        if (ulQuotient > 1 && ulQuotient < 6) {
            float xParam = GLI_pc_g_stGliCaps.xRefreshRate;

            GLD_pc_g_ulFrameTimeAcc722c = 1;
            GLD_pc_g_ulFrameTimeAcc7230 = (unsigned long)((long)xParam * 15);
        }

        if (GLD_pc_g_ulFrameTimeAcc722c != 0 && ulQuotient == 1) {
            if (--GLD_pc_g_ulFrameTimeAcc7230 == 0) {
                GLD_pc_g_ulFrameTimeAcc722c = 0;
                return 2;   /* 0x420EA1 */
            }
        }
        return 1;
    }
    return 1;
}

void __cdecl GLD_pc_vSimulateFlipSynchroWithoutVSync(long lFrameCount)
{
    stTimerCount  stCount;
    unsigned long ulBudget;
    unsigned long ulTarget;

    TMR_fn_wReadTimer((short)GLD_pc_g_wTimerHandle, &stCount);
    ulBudget = (unsigned long)GLI_pc_g_lTickBudget;

    ulTarget = (unsigned long)(lFrameCount * (long)ulBudget)
             - stCount.m_ulLowPart % ulBudget;

    if (stCount.m_ulHighPart != 0) {
        return;
    }
    if (stCount.m_ulLowPart <= 0) {
        return;
    }

    if ((float)ulTarget * GLD_pc_g_xTickToMsScale >= 500.0f) {
        return;
    }

    ulTarget += stCount.m_ulLowPart;

    do {
        TMR_fn_wReadTimer((short)GLD_pc_g_wTimerHandle, &stCount);
    } while (stCount.m_ulLowPart < ulTarget && stCount.m_ulHighPart == 0);
}

long __cdecl GLD_pc_bFirstTimeInit(void)
{
    int           iSlot;
    void        **ppDeviceTable;

    Erm_fn_vSetCallBackForExit(GLD_pc_fn_vExitCallback);

    if (g_ucGLDModuleId == 0xFF) {
        g_ucGLDModuleId = Erm_fn_ucInitErrMsg();
    }

    Mmg_fn_v_InitMmg(g_ucGLDModuleId, 4, 0x186A0u);
    Mmg_fn_v_SetModeAlloc(g_ucGLDModuleId, 0xFF, 0);

    ppDeviceTable = (void **)GLD_pc_g_a_pDeviceTable;
    for (iSlot = 0; iSlot < 0x10; iSlot++) {
        ppDeviceTable[iSlot] = NULL;
    }

    if ((short)TMR_fn_wInitLibrary() == 0x7FFF) {
        ERM_fn_vUpdateLastError(0x34, g_ucGLDModuleId, 0, 0x36, -1, 0xFF, 0xFF,
                                (char *)GLD_g_szDriverVersion);
        return 0;
    }

    GLD_pc_fn_vRestartFrameTimer();
    return 1;
}

long __cdecl GLD_pc_lFindFreeDeviceHandle(short *p_hDev)
{
    short sSlot;

    for (sSlot = 0; sSlot < 0x10; sSlot++) {
        if (((void **)GLD_pc_g_a_pDeviceTable)[sSlot] == NULL) {
            *p_hDev = sSlot;
            return 1;
        }
    }
    *p_hDev = -1;
    return 0;
}

long __cdecl GLD_bCreateDevice(GLD_tdstDeviceAttributes *pstDevAttrib, short *p_hDev)
{
    int            iInitOk;
    short          sScan;
    int           *pDevice;
    DWORD          ulTid;
    unsigned char  ucChannel;

    if (pstDevAttrib->hNormalModeWnd == 0) { pstDevAttrib->hNormalModeWnd = pstDevAttrib->hFullScreenModeWnd; }
    if (pstDevAttrib->hFullScreenModeWnd == 0) { pstDevAttrib->hFullScreenModeWnd = pstDevAttrib->hNormalModeWnd; }

    if (GLD_pc_g_ulFirstTimeInit != 0) {
        iInitOk = GLD_pc_bFirstTimeInit();
        if (iInitOk == 0) {
            return 0;
        }
        GLD_pc_g_ulFirstTimeInit = 0;
    }

    if (pstDevAttrib->bFullScreen != 0) {
        for (sScan = 0; sScan < 0x10; sScan++) {
            GLD_tdstDevice *p_stExistingDev = ((GLD_tdstDevice **)GLD_pc_g_a_pDeviceTable)[sScan];
            if (p_stExistingDev != NULL &&
                p_stExistingDev->stDevAttrib.bFullScreen != 0) {
                pstDevAttrib->bFullScreen = 0;
                break;
            }
        }
    }

    {
        DWORD (__cdecl *pfnChannelForTid)(DWORD, char) =
            (DWORD (__cdecl *)(DWORD, char))Mmg_fn_ucGiveChannelId;
        Mmg_fn_v_SetModeAlloc(g_ucGLDModuleId, 0xff, pfnChannelForTid(GetCurrentThreadId(), 0));
        pDevice = (int *)fn_p_vGenAlloc(0xc0, pfnChannelForTid(GetCurrentThreadId(), 0));
    }

    memset(pDevice, 0, 0xc0);

    pstDevAttrib->dwWidth  = pstDevAttrib->dwFullScreenModeX;
    pstDevAttrib->dwHeight = pstDevAttrib->dwFullScreenModeY;

    {
        GLD_pc_lFindFreeDeviceHandle(p_hDev);
    }

    ((int **)GLD_pc_g_a_pDeviceTable)[*p_hDev] = pDevice;

    pstDevAttrib->xInvPixelDimensionX = 0.0003125f;
    pstDevAttrib->xInvPixelDimensionY = 0.0003125f;
    pstDevAttrib->p_cVirtualScreen    = NULL;
    pstDevAttrib->lPitch              = -1;

    memcpy(pDevice, pstDevAttrib, 0x60);

    pDevice[0x28] = 0;

    pDevice[0x2f] = 0;

    return 1;
}

long __cdecl GLD_bGiveSpecificAttributesToViewport(short hDev_unused,
                                                    short hVpPacked,
                                                    void          *p_vAttribs)
{
    int             iSignedPacked;
    short           sDev;
    short           sVp;
    int            *pViewport;

    iSignedPacked = (int)(short)hVpPacked;
    sDev          = (short)(iSignedPacked / 16);

    (void)GLD_pc_pGetDevice(sDev);

    sVp = (short)(iSignedPacked % 16);

    pViewport = (int *)GLD_pGetViewport(sDev, sVp);
    ((struct GLD_stViewport *)pViewport)->stViewAttrib.p_vSpecificToXD = p_vAttribs;

    (void)hDev_unused;
    return 1;
}

long __cdecl GLD_bRequestWriteToViewport(short hDev_unused,
                                          short hVpPacked,
                                          unsigned long *p_ulOutAttribs,
                                          unsigned long *p_ulOutCanWrite)
{
    int             iSignedPacked;
    short           sDev;
    short           sVp;
    int            *pDevice;
    unsigned long  *pVp;

    *(volatile unsigned long *)GLD_ADDR_7210 = 1;
    if (*(volatile unsigned long *)GLD_ADDR_720C != 0) {
        for (;;) {   }
    }

    iSignedPacked = (int)(short)hVpPacked;
    sDev          = (short)(iSignedPacked / 16);
    pDevice       = (int *)GLD_pc_pGetDevice(sDev);

    sVp           = (short)(iSignedPacked % 16);
    pVp           = (unsigned long *)GLD_pGetViewport(sDev, sVp);

    *p_ulOutCanWrite = 1;
    *(GLD_pc_tdstVptAttrCopy *)p_ulOutAttribs = *(GLD_pc_tdstVptAttrCopy *)pVp;

    ((struct GLD_stDevice *)pDevice)->lWriteInProgress = 1;
    *(volatile unsigned long *)GLD_ADDR_7210 = 0;

    (void)hDev_unused;
    return 1;
}

long __cdecl GLD_bWriteToViewportFinished2D(short hDev_unused,
                                             short hVpPacked)
{
    int             iSignedPacked;
    short           sDev;
    short           sVp;
    int            *pDevice;
    int            *pVp;

    iSignedPacked = (int)(short)hVpPacked;
    sDev          = (short)(iSignedPacked / 16);
    pDevice       = (int *)GLD_pc_pGetDevice(sDev);

    sVp           = (short)(iSignedPacked % 16);
    pVp           = (int *)GLD_pGetViewport(sDev, sVp);

    ((void (__cdecl *)(void))GLI_DRV_g_bUnlockDevice)();

    ((struct GLD_stViewport *)pVp)->bIsLocked = 0;
    ((struct GLD_stDevice *)pDevice)->lWriteInProgress = 0;

    (void)hDev_unused;
    return 1;
}

long __cdecl GLD_bFlipDeviceWithSynchro(GLD_tdhDevice hDev_unused, int eSynchro_unused)
{

    GLD_g_ulRenderCounter7208_storage++;

    GLD_g_ulDrawSync7210_storage = 1;
    while (GLD_g_ulDrawSync720C_storage)
        ;

    {
        void (__cdecl *pfnDrvFlip)(void) =
            (void (__cdecl *)(void))*GLI_DRV_vFlipDeviceWithSyncro_;
        pfnDrvFlip();
    }

    GLD_g_ulDrawSync7210_storage = 0;

    TMR_fn_wResetTimer((short)GLD_pc_g_wTimerHandle);
    TMR_fn_wStartTimer((short)GLD_pc_g_wTimerHandle);

    return 1;
}

long __cdecl GLD_bRequestWriteToViewport2D(short hDev_unused,
                                            short hVpPacked,
                                            unsigned long *p_ulOutAttribs,
                                            long          *p_lOutCanWrite)
{
    int             iSignedPacked;
    short           sDev;
    short           sVp;
    int            *pDevice;
    struct GLD_stViewport *pVp;
    unsigned long  *pCursor;
    int             iLoop;
    long            lDriverOk;

    *(volatile unsigned long *)GLD_ADDR_7210 = 1;
    if (*(volatile unsigned long *)GLD_ADDR_720C != 0) {
        for (;;) {   }
    }

    iSignedPacked = (int)(short)hVpPacked;
    sDev          = (short)(iSignedPacked / 16);
    pDevice       = (int *)GLD_pc_pGetDevice(sDev);

    *p_lOutCanWrite = 0;
    pCursor = p_ulOutAttribs;
    for (iLoop = 0x1c; iLoop != 0; iLoop--) {
        *pCursor = 0xCCCCCCCCu;
        pCursor++;
    }

    sVp      = (short)(iSignedPacked % 16);
    pVp      = (struct GLD_stViewport *)GLD_pGetViewport(sDev, sVp);

    *(GLD_pc_tdstVptAttrCopy *)p_ulOutAttribs = *(GLD_pc_tdstVptAttrCopy *)pVp;

    lDriverOk = ((long (__cdecl *)(unsigned long *, unsigned long *))GLI_DRV_g_bLockDevice)((unsigned long *)&pVp->stViewAttrib.p_cVirtualScreen,
                                                                                         (unsigned long *)&pVp->stViewAttrib.lPitch);
    *p_lOutCanWrite = lDriverOk;

    if (lDriverOk == 1) {

        *(GLD_pc_tdstVptAttrCopy *)p_ulOutAttribs = *(GLD_pc_tdstVptAttrCopy *)pVp;

        pVp->bIsLocked = 1;

        ((struct GLD_stDevice *)pDevice)->lWriteInProgress = 1;
        *(volatile unsigned long *)GLD_ADDR_7210 = 0;
        (void)hDev_unused;
        return 1;
    }

    *(volatile unsigned long *)GLD_ADDR_7210 = 0;
    (void)hDev_unused;
    return 0;
}

void __cdecl GLD_pc_fn_vWriteScanlineRgb24(float *p_xSrc, unsigned char *p_ucDst, unsigned long ulCount)
{
    float *p_xEnd;

    p_xEnd = p_xSrc + ulCount * 4;
    for (; p_xSrc < p_xEnd; p_xSrc += 4) {
        /* 0x0042B430 */
        p_ucDst[0] = (unsigned char)(long)p_xSrc[3];
        p_ucDst++;
        p_ucDst[0] = (unsigned char)(long)p_xSrc[2];
        p_ucDst++;
        p_ucDst[0] = (unsigned char)(long)p_xSrc[1];
        p_ucDst++;
    }
}

void __cdecl GLD_pc_fn_vUnpackScanline16ToFloat(GLD_tdpstPixelFormat p_stFmt, unsigned short *p_usSrc,
                                                float *p_xDst, long lCount, float xScale)
{
    unsigned short *p_usEnd;
    unsigned long ulBitsR;
    unsigned long ulBitsG;
    unsigned long ulBitsB;

    ulBitsR = p_stFmt->lRedNbBits;
    ulBitsG = p_stFmt->lGreenNbBits;
    ulBitsB = p_stFmt->lBlueNbBits;

    p_usEnd = p_usSrc + lCount;
    ulBitsR = 8 - ulBitsR;
    ulBitsG = 8 - ulBitsG;
    ulBitsB = 8 - ulBitsB;
    for (; p_usSrc < p_usEnd; p_usSrc++) {
        p_xDst[1] = (float)((((unsigned long)*p_usSrc >> *(unsigned char *)&p_stFmt->lRedShift)
                             << ulBitsR) & 0xff) * xScale;
        p_xDst[2] = (float)((((unsigned long)*p_usSrc >> *(unsigned char *)&p_stFmt->lGreenShift)
                             << ulBitsG) & 0xff) * xScale;
        p_xDst[3] = (float)((((unsigned long)*p_usSrc >> *(unsigned char *)&p_stFmt->lBlueShift)
                             << ulBitsB) & 0xff) * xScale;
        p_xDst += 4;
    }
    *p_xDst = p_xDst[-4];
    p_xDst++;
    *p_xDst = p_xDst[-4];
    p_xDst++;
    *p_xDst = p_xDst[-4];
    p_xDst++;
    *p_xDst = p_xDst[-4];
}

/* 0x0042B480 */
void __cdecl GLI_pc_fn_vBlendScanlineRGB(float *p_xTable, float *p_xOut,
                                         unsigned long ulCountA, unsigned long ulCountB, float xWeight);

unsigned char GLD_pc_g_a_ucRgbScanline[0x8000];        /* 0x004A72B8 */
unsigned char GLD_pc_g_a_ucFloatRgbScanline[0x8000];   /* 0x004AF2B8 */
static const float GLI_pc_g_xBlendOne;                  /* 0x0049C674 */
extern long __cdecl _ftol(void);

void __cdecl GLD_vGetFrontBufferIn24BitsAA(
    short hGLDDevice, short hGLDViewport,
    unsigned long lWidth, unsigned long lHeight,
    unsigned char *p_lBufferDst)
{
    GLD_tdstPixelFormat stFmt;
    GLD_pc_tdstVptAttrCopy stAttr;
    struct GLD_tdstViewportAttributes_ *p_stAttr;
    long lRequestOk;
    long lCanWrite;
    long lPitchWords;
    unsigned long ulWidth;
    unsigned long ulRowBytes;
    unsigned long ulCount;
    unsigned long ulRows;
    unsigned long ulPrevIdx;
    unsigned long *p_ulZero;
    unsigned short *p_usSrc;
    long lIdx;
    float xPos;
    float xPrevPos;
    float xStep;
    float xW;

    p_stAttr = (struct GLD_tdstViewportAttributes_ *)&stAttr;
    GLD_bGetPixelFormatOfDevice(&stFmt, hGLDDevice, hGLDViewport);
    lRequestOk = GLD_bRequestWriteToViewport2D(hGLDDevice, hGLDViewport,
                                               (unsigned long *)&stAttr, &lCanWrite);
    if (lRequestOk != 0 && lCanWrite != 0) {
        lPitchWords = p_stAttr->lPitch >> 1;
        p_usSrc = (unsigned short *)p_stAttr->p_cVirtualScreen;

        if (stFmt.lPixelNbBits == 0x20) {

            if (p_stAttr->dwHeight != lHeight) {
                return;
            }
            for (; lHeight != 0; lHeight--) {
                for (ulWidth = lWidth; ulWidth != 0; ulWidth--) {
                    p_lBufferDst[0] = (unsigned char)p_usSrc[0];
                    p_lBufferDst[1] = (unsigned char)(p_usSrc[0] >> 8);
                    p_lBufferDst[2] = (unsigned char)p_usSrc[1];
                    p_lBufferDst += 3;
                    p_usSrc += 2;
                }
            }
        } else if (stFmt.lPixelNbBits == 0x10) {

            ulWidth = lWidth;
            if (lWidth < p_stAttr->dwWidth) {
                ulWidth = p_stAttr->dwWidth;
            }
            if (lHeight == p_stAttr->dwHeight) {
                if (lHeight != 0) {
                    ulRowBytes = ulWidth << 2;
                    ulRows = lHeight;
                    do {
                        if (ulRowBytes != 0) {
                            p_ulZero = (unsigned long *)GLD_pc_g_a_ucRgbScanline;
                            for (ulCount = ulRowBytes & 0x3fffffff; ulCount != 0; ulCount--) {
                                *p_ulZero++ = 0;
                            }
                            p_ulZero = (unsigned long *)GLD_pc_g_a_ucFloatRgbScanline;
                            for (ulCount = ulRowBytes & 0x3fffffff; ulCount != 0; ulCount--) {
                                *p_ulZero++ = 0;
                            }
                        }
                        GLD_pc_fn_vUnpackScanline16ToFloat(&stFmt, p_usSrc,
                            (float *)GLD_pc_g_a_ucFloatRgbScanline, (long)ulWidth, 1.0f);
                        GLI_pc_fn_vBlendScanlineRGB((float *)GLD_pc_g_a_ucFloatRgbScanline,
                            (float *)GLD_pc_g_a_ucRgbScanline, p_stAttr->dwWidth, lWidth, 1.0f);
                        GLD_pc_fn_vWriteScanlineRgb24((float *)GLD_pc_g_a_ucRgbScanline,
                            p_lBufferDst, lWidth);
                        p_lBufferDst += lWidth * 3;
                        p_usSrc += lPitchWords;
                        ulRows--;
                    } while (ulRows != 0);
                }
            } else if (p_stAttr->dwHeight < lHeight) {

                xPos = 0.0f;
                xPrevPos = 0.0f;
                ulPrevIdx = 0;
                xStep = (float)p_stAttr->dwHeight / (float)(long)lHeight;
                if (lHeight != 0) {
                    ulRows = lHeight;
                    do {
                        lIdx = (long)xPos;
                        if ((unsigned long)lIdx != ulPrevIdx) {
                            p_usSrc += lPitchWords;
                        }
                        ulRowBytes = ulWidth * 4;
                        if (ulRowBytes != 0) {
                            p_ulZero = (unsigned long *)GLD_pc_g_a_ucRgbScanline;
                            for (ulCount = ulRowBytes; ulCount != 0; ulCount--) {
                                *p_ulZero++ = 0;
                            }
                        }
                        GLD_pc_fn_vUnpackScanline16ToFloat(&stFmt, p_usSrc,
                            (float *)GLD_pc_g_a_ucFloatRgbScanline, (long)ulWidth, 1.0f);
                        xW = xPos - (float)lIdx;
                        GLI_pc_fn_vBlendScanlineRGB((float *)GLD_pc_g_a_ucFloatRgbScanline,
                            (float *)GLD_pc_g_a_ucRgbScanline, p_stAttr->dwWidth, lWidth,
                            GLI_pc_g_xBlendOne - xW);
                        GLD_pc_fn_vUnpackScanline16ToFloat(&stFmt, p_usSrc + lPitchWords,
                            (float *)GLD_pc_g_a_ucFloatRgbScanline, (long)ulWidth, 1.0f);
                        GLI_pc_fn_vBlendScanlineRGB((float *)GLD_pc_g_a_ucFloatRgbScanline,
                            (float *)GLD_pc_g_a_ucRgbScanline, p_stAttr->dwWidth, lWidth, xW);
                        GLD_pc_fn_vWriteScanlineRgb24((float *)GLD_pc_g_a_ucRgbScanline,
                            p_lBufferDst, lWidth);
                        xPos = xPos + xStep;
                        p_lBufferDst += lWidth * 3;
                        ulPrevIdx = (unsigned long)lIdx;
                        ulRows--;
                    } while (ulRows != 0);
                }
            } else if (lHeight < p_stAttr->dwHeight) {

                xPos = 0.0f;
                xPrevPos = 0.0f;
                ulPrevIdx = 0;
                ulRowBytes = ulWidth * 4;
                if (ulRowBytes != 0) {
                    p_ulZero = (unsigned long *)GLD_pc_g_a_ucRgbScanline;
                    for (ulCount = (ulWidth & 0xfffffff) << 2; ulCount != 0; ulCount--) {
                        *p_ulZero++ = 0;
                    }
                    p_ulZero = (unsigned long *)GLD_pc_g_a_ucFloatRgbScanline;
                    for (ulCount = (ulWidth & 0xfffffff) << 2; ulCount != 0; ulCount--) {
                        *p_ulZero++ = 0;
                    }
                }
                if (p_stAttr->dwHeight != 0) {
                    ulRows = p_stAttr->dwHeight;
                    do {
                        lIdx = (long)xPos;
                        xStep = (float)lHeight / (float)(long)p_stAttr->dwHeight;
                        xW = xStep;
                        if ((unsigned long)lIdx != ulPrevIdx) {
                            GLI_pc_fn_vBlendScanlineRGB((float *)GLD_pc_g_a_ucFloatRgbScanline,
                                (float *)GLD_pc_g_a_ucRgbScanline, p_stAttr->dwWidth, lWidth,
                                (float)lIdx - xPrevPos);
                            GLD_pc_fn_vWriteScanlineRgb24((float *)GLD_pc_g_a_ucRgbScanline,
                                p_lBufferDst, lWidth);
                            if (ulRowBytes != 0) {
                                p_ulZero = (unsigned long *)GLD_pc_g_a_ucRgbScanline;
                                for (ulCount = ulRowBytes; ulCount != 0; ulCount--) {
                                    *p_ulZero++ = 0;
                                }
                            }
                            p_lBufferDst += lWidth * 3;
                            xW = xPos - (float)lIdx;
                        }
                        xPrevPos = xPos;
                        GLD_pc_fn_vUnpackScanline16ToFloat(&stFmt, p_usSrc,
                            (float *)GLD_pc_g_a_ucFloatRgbScanline, (long)ulWidth, 1.0f);
                        GLI_pc_fn_vBlendScanlineRGB((float *)GLD_pc_g_a_ucFloatRgbScanline,
                            (float *)GLD_pc_g_a_ucRgbScanline, p_stAttr->dwWidth, lWidth, xW);
                        xPos = xPos + xStep;
                        p_usSrc += lPitchWords;
                        ulRows--;
                        ulPrevIdx = (unsigned long)lIdx;
                    } while (ulRows != 0);
                }
            }
        }
        GLD_bWriteToViewportFinished2D(hGLDDevice, hGLDViewport);
    }
}

extern void GLI_xAdjustCameraToViewport(short hDevice, short hViewport, struct GLI_tdstCamera_ *p_stCam);
void __cdecl fn_vChangeViewPortPercent(
    unsigned long _ulMode,
    unsigned long _ulWhat,
    unsigned long _ulWidth,
    unsigned long _ulHeight,
    unsigned long _ulClipTop,
    unsigned long _ulClipLeft,
    unsigned long _ulClipBottom,
    unsigned long _ulClipRight,
    int           _iPosX,
    int           _iPosY)
{
    GLD_tdstViewportAttributes  stViewAttrib;
    struct GLI_tdstCamera_      *p_stCam;

    GAM_pc_M_ulWaitDrawSemaphore(GAM_g_stEngineStructure.hDrawSem);

  {

    if(_ulMode == 1)
        GLI_xGetViewportCamera(GAM_g_stEngineStructure.hGLDDevice, GAM_g_stEngineStructure.hGLDViewport, &p_stCam);
    else
        GLI_xGetViewportCamera(GAM_g_stEngineStructure.hGLDDevice, GAM_g_stEngineStructure.hGLDFixViewport, &p_stCam);

    if(p_stCam!=NULL)
    {
        GLD_bGetViewportAttributes(GAM_g_stEngineStructure.hGLDDevice, GAM_g_stEngineStructure.hGLDViewport, &stViewAttrib);

        if(_ulWhat == 1)
        {
            stViewAttrib.dwWidthInPercent = _ulWidth;
            stViewAttrib.dwHeightInPercent = _ulHeight;
        }
        else if(_ulWhat == 2)
        {
            stViewAttrib.dwClipTopInPerMille = _ulClipTop;
            stViewAttrib.dwClipLeftInPerMille = _ulClipLeft;
            stViewAttrib.dwClipBottomInPerMille = _ulClipBottom;
            stViewAttrib.dwClipRightInPerMille = _ulClipRight;
        }
        else if(_ulWhat == 3)
        {
            stViewAttrib.iOffsetPosX = _iPosX;
            stViewAttrib.iOffsetPosY = _iPosY;
        }
        else if ( _ulWhat == 4 )
        {
            stViewAttrib.iOffsetPosX = (_iPosX * (int) stViewAttrib.dwWidth) / 1000;
            stViewAttrib.iOffsetPosY = (_iPosY * (int) stViewAttrib.dwHeight) / 1000;
        }

        if(_ulMode == 1)
        {
            GLD_bSetViewportPosition(GAM_g_stEngineStructure.hGLDDevice, GAM_g_stEngineStructure.hGLDViewport, &stViewAttrib);
            GLI_xAdjustCameraToViewport(GAM_g_stEngineStructure.hGLDDevice, GAM_g_stEngineStructure.hGLDViewport, p_stCam);
        }
        else
        {
            GLD_bSetViewportPosition(GAM_g_stEngineStructure.hGLDDevice, GAM_g_stEngineStructure.hGLDFixViewport, &stViewAttrib);
            GLI_xAdjustCameraToViewport(GAM_g_stEngineStructure.hGLDDevice, GAM_g_stEngineStructure.hGLDFixViewport, p_stCam);
        }
    }
    }

    GAM_pc_M_bReleaseDrawSemaphore(GAM_g_stEngineStructure.hDrawSem);
}

long __cdecl _ftol(void);

static const float GLI_pc_g_xBlendZero = 0.0f;   /* 0x0049C670 */
static const float GLI_pc_g_xBlendOne  = 1.0f;   /* 0x0049C674 */

void __cdecl GLI_pc_fn_vBlendScanlineRGB(float *p_xTable, float *p_xOut,
                                         unsigned long ulCountA, unsigned long ulCountB, float xWeight)
{
    float *p_xOutCur;
    float *p_xSrc;
    float *p_xPix;
    unsigned long ulLoop;
    unsigned long ulPrevIdx;
    long lIdx;
    float xPos;
    float xPrevPos;
    float xStep;
    float xF;
    float xG;
    float xW;
    float xF1;
    float xF2;
    float xF3;
    float xC1;
    float xC2;
    float xC3;

    if (ulCountA == ulCountB) {

        if (ulCountA != 0) {
            p_xOutCur = p_xOut + 2;
            ulLoop = ulCountA;
            do {
                p_xTable[0] = xWeight;
                p_xOutCur[-1] = p_xTable[1] * xWeight + p_xOutCur[-1];
                p_xOutCur[0] = p_xTable[2] * xWeight + p_xOutCur[0];
                p_xOutCur[1] = p_xTable[3] * xWeight + p_xOutCur[1];
                p_xOutCur += 4;
                p_xTable += 4;
                ulLoop--;
            } while (ulLoop != 0);
        }
    } else if (ulCountB < ulCountA) {

        xStep = (float)ulCountB / ulCountA;
        xPos = 0.0f;
        xPrevPos = 0.0f;
        ulPrevIdx = 0;
        if (ulCountA != 0) {
            p_xSrc = p_xTable + 2;
            ulLoop = ulCountA;
            do {
                lIdx = (long)xPos;
                xF1 = p_xSrc[-1];
                xF2 = p_xSrc[0];
                xF3 = p_xSrc[1];
                p_xPix = p_xOut + lIdx * 4;
                if ((unsigned long)lIdx == ulPrevIdx) {
                    xF = xStep;
                } else {
                    xW = ((float)lIdx - xPrevPos) * xWeight;
                    p_xPix[-4] = xW + p_xPix[-4];
                    p_xPix[-3] = xF1 * xW + p_xPix[-3];
                    p_xPix[-2] = xF2 * xW + p_xPix[-2];
                    p_xPix[-1] = xF3 * xW + p_xPix[-1];
                    xF = xPos - (float)lIdx;
                }
                xW = xF * xWeight;
                p_xPix[0] = xW + p_xPix[0];
                p_xPix[1] = xF1 * xW + p_xPix[1];
                p_xPix[2] = xF2 * xW + p_xPix[2];
                p_xPix[3] = xF3 * xW + p_xPix[3];
                ulPrevIdx = (unsigned long)lIdx;
                xPrevPos = xPos;
                xPos = xPos + xStep;
                p_xSrc += 4;
                ulLoop--;
            } while (ulLoop != 0);
        }
    } else {

        xStep = (float)ulCountA / ulCountB;
        xPos = 0.0f;
        if (ulCountB != 0) {
            p_xOutCur = p_xOut;
            ulLoop = ulCountB;
            do {
                lIdx = (long)xPos;
                p_xSrc = p_xTable + lIdx * 4;
                xF = xPos - (float)lIdx;
                xG = GLI_pc_g_xBlendOne - xF;
                xC1 = p_xSrc[1] * xG + p_xSrc[5] * xF;
                xC2 = p_xSrc[2] * xG + p_xSrc[6] * xF;
                xC3 = p_xSrc[3] * xG + p_xSrc[7] * xF;
                p_xOutCur[0] = xWeight + p_xOutCur[0];
                p_xOutCur[1] = xC1 * xWeight + p_xOutCur[1];
                p_xOutCur[2] = xC2 * xWeight + p_xOutCur[2];
                p_xOutCur[3] = xC3 * xWeight + p_xOutCur[3];
                p_xOutCur += 4;
                xPos = xPos + xStep;
                ulLoop--;
            } while (ulLoop != 0);
        }
    }
}
