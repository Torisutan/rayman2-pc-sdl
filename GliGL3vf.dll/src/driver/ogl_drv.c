/*
 ----------------------------------------------------------------------------------------
  ogl_drv.c : driver entry points of the OpenGL 3.3 driver that the Direct3D 6 driver
  implements with DirectDraw: driver information, initialisation, capabilities, display
  modes and the 16-bit bitmap blit. The rest of the driver interface comes from the
  Direct3D 6 driver's device-independent files (GliGL3vf.h).
 ----------------------------------------------------------------------------------------
 */
#include "GliGL3vf/GliGL3vf.h"

#include <SDL3/SDL.h>
#include <stdlib.h>
#include <string.h>

/* Viewing list of the engine, allocated by the driver (as the Direct3D 6 driver does) */
#define C_lViewingListBlockSize         0x10018
#define C_lViewingListSecondarySize     0x30000
#define C_lViewingListSecondaryOffset   0x10014

/* Driver capabilities (tdstGliCaps, GLI/devinfo.h) */
#define C_ulDriverCaps      (E_DRV_IsHardware | E_DRV_CanBeWindowed | E_DRV_SupportAlphaTest | E_DRV_ReadjustViewport)
#define C_ulTextureCaps     (E_TEX_SupportNonSquare | E_TEX_SupportMipMap)
/* The texture memory the engine may plan with: GL has no portable query, the driver manages
   its textures itself (docs/PLAN.md "OpenGL driver", open questions) */
#define C_ulTextureMemory   0x40000000u

/* DDPIXELFORMAT records of the screen formats (the engine's defined pixel formats) */
#define C_ulDDPF_RGB        0x00000040u

static GLI_tdstDllInfo GLI_g_stDefaultDllInfo = { "OpenGL3", "OpenGL 3.3 Driver", "1.0.0", 3 };

GLI_tdstDllInfo GLI_g_stDllInfo;    /* answered by GLI_DRV_lGetDllInfo */

void __cdecl GLI_vInitDllInfo(void)
{
    GLI_g_stDllInfo = GLI_g_stDefaultDllInfo;
}

#ifdef WIN32
BOOL WINAPI DllMain(HINSTANCE hInstance, DWORD dwReason, LPVOID pReserved)
{
    (void)hInstance;
    (void)dwReason;
    (void)pReserved;
    GLI_vInitDllInfo();
    return TRUE;
}
#endif /* WIN32 */

/*
 ----------------------------------------------------------------------------------------
  Description : driver information, as the Direct3D 6 driver's (Name, Description,
                Version, Code), and "Window": the window the driver renders to must be made
                for OpenGL (GAM/sdl_main.c asks before it creates the window)
 ----------------------------------------------------------------------------------------
 */
__declspec(dllexport) long __cdecl GLI_DRV_lGetDllInfo(char const *szName, void *pData)
{
    char const *szSource;

    if ( strcmp(szName, "Name") == 0 )
        szSource = GLI_g_stDllInfo.szName;
    else if ( strcmp(szName, "Description") == 0 )
        szSource = GLI_g_stDllInfo.szDescription;
    else if ( strcmp(szName, "Version") == 0 )
        szSource = GLI_g_stDllInfo.szVersion;
    else if ( strcmp(szName, "Code") == 0 )
    {
        *(long *)pData = GLI_g_stDllInfo.lCode;
        return 1;
    }
    else if ( strcmp(szName, "Window") == 0 )
        szSource = "OpenGL";
    else
        return 0;
    strcpy((char *)pData, szSource);
    return 1;
}

/*
 ----------------------------------------------------------------------------------------
  Description : the capabilities the engine reads (GLI_pc_g_stGliCaps), as the Direct3D 6
                driver reports them for a hardware device that renders windowed
 ----------------------------------------------------------------------------------------
 */
static void fn_vReadCaps(tdstGliCaps *p_stCaps, long lBpp)
{
    GLint iMaxTexture = 0;
    unsigned long a_ulPixelFormat[8];
    const SDL_DisplayMode *p_stMode;

    p_stCaps->ulDriverCaps = (tdeDriverCaps)(p_stCaps->ulDriverCaps | C_ulDriverCaps);
    p_stCaps->ulTextureCaps = (tdeTextureCaps)(p_stCaps->ulTextureCaps | C_ulTextureCaps);
    if ( GLI_g_hGLContext != NULL )
    {
        GLI_g_stGL.p_fn_GetIntegerv(GL_MAX_TEXTURE_SIZE, &iMaxTexture);
    }
    p_stCaps->ulTextureMaxSize = iMaxTexture > 0 ? (unsigned long)iMaxTexture : 0x100;
    p_stCaps->ulTotalTextureMemory = C_ulTextureMemory;
    p_stCaps->ulTotalVideoMemory = C_ulTextureMemory;

    /* primary surface format: 565 in a 16-bit mode, X8R8G8B8 otherwise */
    memset(a_ulPixelFormat, 0, sizeof(a_ulPixelFormat));
    a_ulPixelFormat[0] = sizeof(a_ulPixelFormat);
    a_ulPixelFormat[1] = C_ulDDPF_RGB;
    if ( lBpp == 16 )
    {
        a_ulPixelFormat[3] = 16;
        a_ulPixelFormat[4] = 0xf800;
        a_ulPixelFormat[5] = 0x07e0;
        a_ulPixelFormat[6] = 0x001f;
    }
    else
    {
        a_ulPixelFormat[3] = 32;
        a_ulPixelFormat[4] = 0x00ff0000;
        a_ulPixelFormat[5] = 0x0000ff00;
        a_ulPixelFormat[6] = 0x000000ff;
    }
    p_stCaps->ucPrimarySurfacePixelFormat = GLI_lGetOrAddDefinedPixelFormat(a_ulPixelFormat);

    /* refresh rate of the display (Direct3D 6: measured over 70 vertical blanks); 0 when
       unknown, which the engine replaces by 60 Hz and its own frame timing */
    p_stMode = SDL_GetCurrentDisplayMode(GLI_g_p_stSDLWindow != NULL
                                         ? SDL_GetDisplayForWindow((SDL_Window *)GLI_g_p_stSDLWindow)
                                         : SDL_GetPrimaryDisplay());
    p_stCaps->xRefreshRate = (p_stMode != NULL) ? (float)SDL_floor(p_stMode->refresh_rate + 0.5f) : 0.0f;
}

__declspec(dllexport) void __cdecl
GLI_DRV_xInitDriver(HWND hwnd, BOOL fullscreen, LONG width, LONG height, int bpp)
{
    GLI_g_GliBigGlobals->p_TheZListe = malloc(C_lViewingListBlockSize);
    memset(GLI_g_GliBigGlobals->p_TheZListe, 0, C_lViewingListBlockSize);
    *(void **)((unsigned char *)GLI_g_GliBigGlobals->p_TheZListe + C_lViewingListSecondaryOffset) =
        malloc(C_lViewingListSecondarySize);
    GLI_g_GliBigGlobals->ulCurrentZTable = 0;
    GLI_DRV_xClearViewingList();

    GLI_g_WindowHandle = hwnd;
    if ( !GLI_fn_bCreateGLContext(hwnd) )
    {
        return;
    }
    SDL_ShowWindow((SDL_Window *)GLI_g_p_stSDLWindow);
    SDL_RaiseWindow((SDL_Window *)GLI_g_p_stSDLWindow);

    if ( GLI_CreateOrResetDevice(hwnd, fullscreen, width, height, bpp) == 0 )
    {
        GLI_g_ActivateGliFlag = 1;
    }
    memset(GLI_g_GliCaps, 0, sizeof(tdstGliCaps));
    fn_vReadCaps((tdstGliCaps *)GLI_g_GliCaps, fullscreen ? bpp : 32);
    GLI_fn_vInitTextureFormats();
}

/*
 ----------------------------------------------------------------------------------------
  Description : display modes of the primary display, each size once, at 16 bits per
                pixel: the Direct3D 6 driver lists only its 16-bit modes, and the engine's
                mode choice (ubi.ini GLI_Mode) names one of them. The driver and device
                names are those GLI_DRV_fn_lGetAllDisplayConfig reports; any is accepted.
 ----------------------------------------------------------------------------------------
 */
static long fn_lForEachDisplayMode(void (*p_fn_vAddMode)(long lWidth, long lHeight))
{
    SDL_DisplayMode **d_p_stModes;
    int iNbModes = 0;
    int i, j;

    d_p_stModes = SDL_GetFullscreenDisplayModes(SDL_GetPrimaryDisplay(), &iNbModes);
    if ( d_p_stModes == NULL )
    {
        return 0;
    }
    for ( i = iNbModes - 1 ; i >= 0 ; i-- )     /* SDL lists the largest first */
    {
        for ( j = iNbModes - 1 ; j > i ; j-- )
        {
            if ( d_p_stModes[j]->w == d_p_stModes[i]->w && d_p_stModes[j]->h == d_p_stModes[i]->h )
            {
                break;
            }
        }
        if ( j == i )
        {
            p_fn_vAddMode(d_p_stModes[i]->w, d_p_stModes[i]->h);
        }
    }
    SDL_free(d_p_stModes);
    return iNbModes > 0;
}

static void fn_vAddEngineMode(long lWidth, long lHeight)
{
    GLI_g_AddDisplayMode(1, lWidth, lHeight, 16);
}

__declspec(dllexport) long __cdecl GLI_DRV_fnl_EnumModes(char *driverName, char *deviceName)
{
    (void)driverName;
    (void)deviceName;
    return fn_lForEachDisplayMode(fn_vAddEngineMode);
}

static void fn_vAddConfigMode(long lWidth, long lHeight)
{
    long lMode;

    lMode = GLI_g_pAddDisplayInfo(GLI_g_DisplayConfigDisplayNode, 0, 0, "modeadd", 0);
    GLI_g_pAddDisplayInfo(GLI_g_DisplayConfigDisplayNode, 0, lMode, "modefs", 1);
    GLI_g_pAddDisplayInfo(GLI_g_DisplayConfigDisplayNode, 0, lMode, "modebpp", 16);
    GLI_g_pAddDisplayInfo(GLI_g_DisplayConfigDisplayNode, 0, lMode, "modew", lWidth);
    GLI_g_pAddDisplayInfo(GLI_g_DisplayConfigDisplayNode, 0, lMode, "modeh", lHeight);
}

/* The display configuration tree, shaped as the Direct3D 6 driver's: one display ("display",
   the primary), one device, its capabilities and its modes */
__declspec(dllexport) long __cdecl
GLI_DRV_fn_lGetAllDisplayConfig(GLI_tdfn_lAddDisplayInfo p_fn_lAddDisplayInfo)
{
    tdstGliCaps stCaps;

    GLI_g_pAddDisplayInfo = p_fn_lAddDisplayInfo;
    p_fn_lAddDisplayInfo(0, 0, 0, "dll_bmp", 2);

    GLI_g_DisplayConfigDisplayNode = GLI_g_pAddDisplayInfo(0, 0, 0, "dispadd", 0);
    GLI_g_pAddDisplayInfo(GLI_g_DisplayConfigDisplayNode, 0, 0, "dispname", (long)(LONG_PTR)"display");
    GLI_g_pAddDisplayInfo(GLI_g_DisplayConfigDisplayNode, 0, 0, "dispdesc", (long)(LONG_PTR)"Primary display");
    GLI_g_pAddDisplayInfo(GLI_g_DisplayConfigDisplayNode, 0, 0, "dispparam", 0);

    GLI_g_DisplayConfigDeviceNode = GLI_g_pAddDisplayInfo(GLI_g_DisplayConfigDisplayNode, 0, 0, "dev_add", 0);
    GLI_g_pAddDisplayInfo(GLI_g_DisplayConfigDisplayNode, GLI_g_DisplayConfigDeviceNode, 0,
                          "dev_name", (long)(LONG_PTR)"OpenGL 3.3");
    GLI_g_pAddDisplayInfo(GLI_g_DisplayConfigDisplayNode, GLI_g_DisplayConfigDeviceNode, 0,
                          "dev_desc", (long)(LONG_PTR)"OpenGL 3.3 core profile");
    GLI_g_pAddDisplayInfo(GLI_g_DisplayConfigDisplayNode, GLI_g_DisplayConfigDeviceNode, 0, "dev_param", 0);
    memset(&stCaps, 0, sizeof(stCaps));
    fn_vReadCaps(&stCaps, 16);
    GLI_g_pAddDisplayInfo(GLI_g_DisplayConfigDisplayNode, GLI_g_DisplayConfigDeviceNode, 0,
                          "dev_caps", (long)(LONG_PTR)&stCaps);

    return fn_lForEachDisplayMode(fn_vAddConfigMode);
}

/*
 ----------------------------------------------------------------------------------------
  Description : a bitmap into the back buffer (loading screens, videos). Direct3D 6: in a
                32-bit mode the source is 24-bit (B, G, R) copied to the top left corner;
                in a 16-bit mode a 565 source stretched over the destination rectangle by
                the engine's BlitStretched16b. Here the same pixels, drawn as a quad.
 ----------------------------------------------------------------------------------------
 */
__declspec(dllexport) void __cdecl
GLI_DRV_vWrite16bBitmapToBackBuffer(void *pSrc, LONG lW, LONG lH,
                                    LONG lDestLeft, LONG lDestTop,
                                    LONG lDestRight, LONG lDestBottom)
{
    unsigned long *d_ulRGBA;
    long lDW, lDH, i;

    if ( GLI_g_SavedBpp == 0x20 )
    {
        const unsigned char *p_ucSrc = (const unsigned char *)pSrc;

        d_ulRGBA = (unsigned long *)malloc((size_t)lW * (size_t)lH * 4);
        if ( d_ulRGBA == NULL )
        {
            return;
        }
        for ( i = 0 ; i < lW * lH ; i++ )
        {
            d_ulRGBA[i] = 0xff000000u | ((unsigned long)p_ucSrc[0] << 16) |
                          ((unsigned long)p_ucSrc[1] << 8) | (unsigned long)p_ucSrc[2];
            p_ucSrc += 3;
        }
        GLI_fn_vDrawBitmap(d_ulRGBA, lW, lH, 0, 0, lW - 1, lH - 1);
        free(d_ulRGBA);
        return;
    }

    lDW = (lDestRight - lDestLeft) + 1;
    lDH = (lDestBottom - lDestTop) + 1;
    if ( lDW <= 0 || lDH <= 0 )
    {
        return;
    }
    d_ulRGBA = (unsigned long *)malloc((size_t)lDW * (size_t)lDH * 4);
    if ( d_ulRGBA == NULL )
    {
        return;
    }
    /* the engine stretches the 565 source into the 16-bit buffer (first half of the
       32-bit one, as wide as the destination), then each pixel is widened to 8 bits */
    GLI_g_BlitStretched16b(lDW, lDH, (unsigned short *)d_ulRGBA, lDW, lW, lH, pSrc);
    for ( i = lDW * lDH - 1 ; i >= 0 ; i-- )
    {
        unsigned long ulP = ((unsigned short *)d_ulRGBA)[i];
        unsigned long ulR = (ulP >> 11) & 0x1f, ulG = (ulP >> 5) & 0x3f, ulB = ulP & 0x1f;

        ulR = (ulR << 3) | (ulR >> 2);
        ulG = (ulG << 2) | (ulG >> 4);
        ulB = (ulB << 3) | (ulB >> 2);
        d_ulRGBA[i] = 0xff000000u | (ulB << 16) | (ulG << 8) | ulR;
    }
    GLI_fn_vDrawBitmap(d_ulRGBA, lDW, lDH, lDestLeft, lDestTop, lDestRight, lDestBottom);
    free(d_ulRGBA);
}
