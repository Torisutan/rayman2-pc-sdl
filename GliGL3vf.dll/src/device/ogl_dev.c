/*
 ----------------------------------------------------------------------------------------
  ogl_dev.c : device of the OpenGL 3.3 driver.

  The device layer the shared Direct3D 6 driver files call (GLI_CreateOrResetDevice,
  GLI_ReleaseDeviceObjects, GLI_DestroyOrCleanupDevice: used by GLI_DRV_hChangeMode,
  GLI_DRV_bPrepareForGliWindowed, GLI_DRV_vActivateGli, GLI_DRV_vCloseDriver) and the
  device entry points of that driver, each with the Direct3D 6 driver's behaviour:
    - fullscreen: a real mode switch (Direct3D 6: SetDisplayMode with an exclusive
      cooperative level), here SDL's fullscreen mode of the window;
    - windowed: the client area of the window;
    - flips: swap of the GL context's buffers; synchronised flips wait for the vertical
      blank (swap interval 1) and for the extra frames the engine asks for;
    - surfaces are never lost: the restore path only reloads the textures.
  The context is created once on the engine's window (SDL), at GLI_DRV_xInitDriver, and lives
  until GLI_DRV_vCloseDriver; resets change its window, not the context.
 ----------------------------------------------------------------------------------------
 */
#include "GliGL3vf/GliGL3vf.h"

#include <SDL3/SDL.h>

#define C_hrFail    ((int)0x80004005)

void *GLI_g_p_stSDLWindow = NULL;
void *GLI_g_hGLContext = NULL;
long  GLI_g_lDrawableWidth = 0;
long  GLI_g_lDrawableHeight = 0;

static int gs_iSwapInterval = -1;

/*
 ----------------------------------------------------------------------------------------
  Description : the SDL window behind the engine's window handle. On Windows the engine
                hands the window's real HWND; elsewhere GAM/sdl_main.c puts the SDL window
                itself in the HWND (as the original's console types headers declare it).
 ----------------------------------------------------------------------------------------
 */
static SDL_Window *fn_p_stGetSDLWindow(HWND hWnd)
{
#ifdef WIN32
    SDL_Window **d_p_stWindows;
    SDL_Window *p_stFound = NULL;
    int iNbWindows = 0;
    int i;

    d_p_stWindows = SDL_GetWindows(&iNbWindows);
    if ( d_p_stWindows == NULL )
    {
        return NULL;
    }
    for ( i = 0 ; i < iNbWindows ; i++ )
    {
        if ( SDL_GetPointerProperty(SDL_GetWindowProperties(d_p_stWindows[i]),
                                    SDL_PROP_WINDOW_WIN32_HWND_POINTER, NULL) == (void *)hWnd )
        {
            p_stFound = d_p_stWindows[i];
            break;
        }
    }
    SDL_free(d_p_stWindows);
    return p_stFound;
#else
    return (SDL_Window *)hWnd;
#endif /* WIN32 */
}

/*
 ----------------------------------------------------------------------------------------
  Description : OpenGL 3.3 core context on the engine's window, then the renderer
 ----------------------------------------------------------------------------------------
 */
long __cdecl GLI_fn_bCreateGLContext(HWND hWnd)
{
    if ( GLI_g_hGLContext != NULL )
    {
        return 1;
    }
    GLI_g_p_stSDLWindow = fn_p_stGetSDLWindow(hWnd);
    if ( GLI_g_p_stSDLWindow == NULL )
    {
        SDL_LogError(SDL_LOG_CATEGORY_RENDER, "GliGL3: no SDL window for the engine's window");
        return 0;
    }
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
    SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
    GLI_g_hGLContext = SDL_GL_CreateContext((SDL_Window *)GLI_g_p_stSDLWindow);
    if ( GLI_g_hGLContext == NULL )
    {
        SDL_LogError(SDL_LOG_CATEGORY_RENDER, "GliGL3: %s", SDL_GetError());
        return 0;
    }
    SDL_GL_MakeCurrent((SDL_Window *)GLI_g_p_stSDLWindow, (SDL_GLContext)GLI_g_hGLContext);
    gs_iSwapInterval = -1;
    if ( !GLI_fn_bInitRenderer() )
    {
        GLI_fn_vDestroyGLContext();
        return 0;
    }
    return 1;
}

void __cdecl GLI_fn_vDestroyGLContext(void)
{
    if ( GLI_g_hGLContext != NULL )
    {
        GLI_fn_vCloseRenderer();
        SDL_GL_MakeCurrent((SDL_Window *)GLI_g_p_stSDLWindow, NULL);
        SDL_GL_DestroyContext((SDL_GLContext)GLI_g_hGLContext);
        GLI_g_hGLContext = NULL;
    }
}

void __cdecl GLI_fn_vSwapBuffers(long bSynchro)
{
    int iInterval = bSynchro ? 1 : 0;

    if ( GLI_g_hGLContext == NULL )
    {
        return;
    }
    if ( iInterval != gs_iSwapInterval )
    {
        SDL_GL_SetSwapInterval(iInterval);
        gs_iSwapInterval = iInterval;
    }
    SDL_GL_SwapWindow((SDL_Window *)GLI_g_p_stSDLWindow);
}

/* Client and screen rectangles of the window in pixels, and the drawable size */
static void fn_vReadWindowRects(void)
{
    int iW = 0, iH = 0, iX = 0, iY = 0;

    SDL_GetWindowSizeInPixels((SDL_Window *)GLI_g_p_stSDLWindow, &iW, &iH);
    SDL_GetWindowPosition((SDL_Window *)GLI_g_p_stSDLWindow, &iX, &iY);
    GLI_g_ClientRect.left = 0;
    GLI_g_ClientRect.top = 0;
    GLI_g_ClientRect.right = iW;
    GLI_g_ClientRect.bottom = iH;
    GLI_g_ScreenRect.left = iX;
    GLI_g_ScreenRect.top = iY;
    GLI_g_ScreenRect.right = iX + iW;
    GLI_g_ScreenRect.bottom = iY + iH;
    GLI_g_lDrawableWidth = iW;
    GLI_g_lDrawableHeight = iH;
}

/*
 ----------------------------------------------------------------------------------------
  Description : (re)creates the device for fullscreen (the mode width x height) or for the
                window's client area, as the Direct3D 6 driver's function of that name.
                Returns 0 on success, an error code otherwise.
 ----------------------------------------------------------------------------------------
 */
int __cdecl GLI_CreateOrResetDevice(HWND hwnd, BOOL fullscreen, LONG width, LONG height, LONG bpp)
{
    SDL_Window *p_stWindow;

    GLI_g_IsFullscreen = fullscreen;
    if ( fullscreen )
    {
        GLI_g_SavedWidth = width;
        GLI_g_SavedHeight = height;
        GLI_g_SavedBpp = bpp;
    }
    if ( !GLI_fn_bCreateGLContext(hwnd) )
    {
        return C_hrFail;
    }
    p_stWindow = (SDL_Window *)GLI_g_p_stSDLWindow;

    if ( fullscreen )
    {
        SDL_DisplayMode stMode;

        if ( !SDL_GetClosestFullscreenDisplayMode(SDL_GetDisplayForWindow(p_stWindow), (int)width,
                                                  (int)height, 0.0f, false, &stMode)
             || !SDL_SetWindowFullscreenMode(p_stWindow, &stMode)
             || !SDL_SetWindowFullscreen(p_stWindow, true) )
        {
            SDL_LogError(SDL_LOG_CATEGORY_RENDER, "GliGL3: fullscreen %ldx%ld: %s", (long)width,
                         (long)height, SDL_GetError());
            return C_hrFail;
        }
        SDL_SyncWindow(p_stWindow);
        fn_vReadWindowRects();
        GLI_g_ClientRect.right = width;
        GLI_g_ClientRect.bottom = height;
        GLI_g_ScreenRect = GLI_g_ClientRect;
    }
    else
    {
        if ( SDL_GetWindowFlags(p_stWindow) & SDL_WINDOW_FULLSCREEN )
        {
            SDL_SetWindowFullscreen(p_stWindow, false);
            SDL_SyncWindow(p_stWindow);
        }
        fn_vReadWindowRects();
    }

    GLI_fn_vResetRenderStates();
    GLI_fn_vSetViewport(GLI_g_ClientRect.right - GLI_g_ClientRect.left,
                        GLI_g_ClientRect.bottom - GLI_g_ClientRect.top,
                        GLI_g_lDrawableWidth, GLI_g_lDrawableHeight);
    return 0;
}

/*
 ----------------------------------------------------------------------------------------
  Description : releases the device: the textures always (the engine loads them again
                after a reset), the context and fullscreen with bReleaseAll (the Direct3D 6
                driver releases DirectDraw and its exclusive mode then)
 ----------------------------------------------------------------------------------------
 */
int __cdecl GLI_ReleaseDeviceObjects(int releaseDirectDraw)
{
    GLI_DRV_vUnLoadTextures();
    if ( (releaseDirectDraw & 0xff) != 0 )
    {
        if ( GLI_g_p_stSDLWindow != NULL
             && (SDL_GetWindowFlags((SDL_Window *)GLI_g_p_stSDLWindow) & SDL_WINDOW_FULLSCREEN) )
        {
            SDL_SetWindowFullscreen((SDL_Window *)GLI_g_p_stSDLWindow, false);
        }
        GLI_fn_vDestroyGLContext();
    }
    return 0;
}

/* Restore after a lost device: a GL context loses nothing; the textures are reloaded as the
   Direct3D 6 driver does after restoring its surfaces */
int __cdecl GLI_DestroyOrCleanupDevice(void)
{
    GLI_g_ReloadTextures();
    return 0;
}

/* The back buffer is not accessible to the CPU (the exe never locks it) */
__declspec(dllexport) long __cdecl GLI_DRV_bLockDevice(void **screen, LONG *pitch)
{
    (void)screen;
    (void)pitch;
    return 0;
}

__declspec(dllexport) long __cdecl GLI_DRV_bUnlockDevice(void)
{
    return 0;
}

__declspec(dllexport) void __cdecl GLI_DRV_vClearDevice(int clearZBuffer, int clearColor, unsigned int color)
{
    if ( GLI_g_IsGliInit() == 0 )
    {
        return;
    }
    GLI_fn_vClearBuffers(clearZBuffer, clearColor, color);
}

__declspec(dllexport) void __cdecl GLI_DRV_vFlipDevice(void)
{
    if ( GLI_g_IsGliInit() == 0 || GLI_g_ActivateGliFlag == 0 )
    {
        return;
    }
    GLI_g_GliBigGlobals->lAMirrorIsDetected = 0;
    GLI_g_GliBigGlobals->lDisplayMenuIsDetected = 0;
    /* Direct3D 6: Flip (waits for the vertical blank) in fullscreen, Blt in a window */
    GLI_fn_vSwapBuffers(GLI_g_IsFullscreen);
}

#define C_ulGliCapsSoftwareSync     0x40u

__declspec(dllexport) void __cdecl GLI_DRV_vFlipDeviceWithSyncro(void)
{
    tdstGliCaps *p_stCaps = (tdstGliCaps *)GLI_g_GliCaps;
    int iWaitFrames;

    if ( GLI_g_IsGliInit() == 0 || GLI_g_ActivateGliFlag == 0 )
    {
        return;
    }
    GLI_g_GliBigGlobals->lAMirrorIsDetected = 0;
    GLI_g_GliBigGlobals->lDisplayMenuIsDetected = 0;

    if ( (p_stCaps->ulDriverCaps & C_ulGliCapsSoftwareSync) != 0 )
    {
        /* no measured refresh rate: the engine times the frame itself */
        GLI_fn_vSwapBuffers(0);
        GLI_g_SimulateFlipSynchroWithoutVSync(1);
    }
    else
    {
        GLI_fn_vSwapBuffers(1);
    }

    /* Further frames to wait (Direct3D 6: further vertical blanks); a GL swap cannot wait
       for a blank without presenting, so they are timed as without vertical sync */
    iWaitFrames = GLI_g_ComputeWaitFrameForSmoothSynchro(0) - 1;
    if ( iWaitFrames > 0 )
    {
        GLI_g_SimulateFlipSynchroWithoutVSync(iWaitFrames);
    }
}

__declspec(dllexport) void __cdecl GLI_DRV_vPrepareForGliFullScreen(HWND hwnd)
{
    if ( GLI_g_IsGliInit() == 0 )
    {
        return;
    }
    GLI_ReleaseDeviceObjects(0);
    GLI_g_WindowHandle = hwnd;
    GLI_CreateOrResetDevice(hwnd, TRUE, GLI_g_SavedWidth, GLI_g_SavedHeight, GLI_g_SavedBpp);
    GLI_DRV_vClearDevice(0, 1, 0);
    GLI_fn_vSwapBuffers(1);
    GLI_g_ReloadTextures();
}

/* The window was moved or resized: same size, only the position is taken; else a new
   windowed device, as the Direct3D 6 driver */
__declspec(dllexport) void __cdecl GLI_DRV_vReadaptDisplay(void)
{
    int iW = 0, iH = 0;

    if ( GLI_g_p_stSDLWindow == NULL )
    {
        return;
    }
    SDL_GetWindowSizeInPixels((SDL_Window *)GLI_g_p_stSDLWindow, &iW, &iH);
    if ( (GLI_g_ScreenRect.right - GLI_g_ScreenRect.left) == iW
         && (GLI_g_ScreenRect.bottom - GLI_g_ScreenRect.top) == iH )
    {
        fn_vReadWindowRects();
        return;
    }
    GLI_ReleaseDeviceObjects(0);
    if ( GLI_CreateOrResetDevice(GLI_g_WindowHandle, FALSE, 0, 0, 0) != 0 )
    {
        GLI_DestroyOrCleanupDevice();
    }
}

__declspec(dllexport) long __cdecl GLI_DRV_bBeginScene(void)
{
    GLI_SetBeginSceneOpaqueTextureState();
    GLI_DrawBlackPolygons();
    return 1;
}

__declspec(dllexport) long __cdecl GLI_DRV_bEndScene(void)
{
    return 1;
}
