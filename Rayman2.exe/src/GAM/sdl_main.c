/*
 =======================================================================================
  Name        : sdl_main.c
  Description : Application window, events and engine loop through SDL (USE_SDL)

  The SDL counterpart of the Win32 parts of GAM/pc_winmain.c (the blocks fenced
  #ifndef USE_SDL there), with the same GAM_pc.h functions. The engine loop runs on the
  main thread, as on the original console targets: no engine thread, the five engine
  events are flags, and the window events are pumped where the engine thread waited for
  them, before every frame (FUN_00401160). Requests the engine posted to the window
  thread (display mode change, activation) become SDL user events, served there too.

  On Windows the window's HWND goes to the renderer driver, DirectInput and the sound
  driver as before. With one thread there is no draw lock (GAM/GAM_pc_DrawLock.h): the
  draw semaphore is 0, as in the original engine's console build.
 =======================================================================================
 */
#ifdef USE_SDL

#define SDL_MAIN_HANDLED
#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>

#include "GAM/GAM_pc.h"
#include "GAM/GAM_globals.h"
#include "GAM/GAM_types.h"
#include "GLI/misc_trivial.h"
#include "ERM/CpaError.h"
#include "SND/SND.h"

#include <io.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "port/regress.h"

void __cdecl MMG_fn_vInit(int bInitMMG);
long __cdecl GLI_xIsGliInit(void);

extern void          __cdecl SND_fn_vReleaseDriverSound(void);   /* 0x00453790 */
extern void          __cdecl SND_fn_vRestoreDriverSound(void);   /* 0x004537A0 */
extern long          __cdecl GLD_bSetFullScreenModeForDevice(short hDev,
                                                             unsigned long ulWidth,
                                                             unsigned long ulHeight,
                                                             unsigned long ulBpp,
                                                             unsigned long ulHWnd);
extern long          __cdecl GLD_bSwapDeviceMode(short hDev, int bRebuildMode);
extern long          __cdecl GLD_bSwapToWindowedMode(short hDev, int bRebuildMode);
extern void          __cdecl FUN_0045edf0(void);
extern unsigned long __cdecl GLI_fn_lDisplayModeWidth(void);
extern unsigned long __cdecl GLI_fn_lDisplayModeHeight(void);
extern long          __cdecl GLI_pc_fn_bApplyDeferredMode(void);
extern void          __cdecl IPT_fn_vResetInputEntry(void);      /* 0x0042E090 */
extern void          __cdecl FUN_0042e150(void);                 /* 0x0042E150 */
void FUN_00401050(void);
unsigned long FUN_00401b20(void);
void GAM_pc_fn_vChooseTheGoodDesInit(void);

#define GAM_SDL_SZ_WNDCLASS       "Rayman2"
#define GAM_SDL_SZ_WNDTITLE       "Rayman II"
#define GAM_SDL_SZ_PAUSE_TITLE    "Rayman2 - pause"
#define GAM_SDL_SZ_RESTORE_TITLE  "Rayman2 - restoring data... please wait"

/* The window (*g_hWnd is its HWND on Windows, the SDL window itself elsewhere) */
static SDL_Window *GAM_sdl_g_p_stWindow = NULL;

/* SDL event type of the requests GAM_pc_fn_vPostWindowCmd posts (user.code = the command) */
static Uint32 GAM_sdl_g_ulWindowCmdEvent = 0;

/* The engine events, as flags: EXEFRAMEEVT starts set, the others clear */
static int GAM_sdl_g_bEventExeFrame = 1;
static int GAM_sdl_g_bEventExit     = 0;
static int GAM_sdl_g_bEventIsDead   = 0;
static int GAM_sdl_g_bEventIsPaused = 0;
static int GAM_sdl_g_bEventPulse    = 0;

/* Focus and pause state, as in pc_winmain.c */
static unsigned long GAM_sdl_g_ulPauseFlag       = 1;
static unsigned long GAM_sdl_g_ulPausePending    = 1;
static unsigned long GAM_sdl_g_ulPauseFlagMirror = 0;
static unsigned long GAM_sdl_g_ulDebounce2s      = 0;
static unsigned long GAM_sdl_g_ulDebounce30s     = 0;
static unsigned long GAM_sdl_g_ulPendingCompan   = 1;
static int           GAM_sdl_g_bDesktopNot16Bpp  = 0;
static void         *GAM_sdl_g_a4_pfnEngineCallback[4];

/*
 ----------------------------------------------------------------------------------------
  Description   : Milliseconds for the focus debounce (timeGetTime in pc_winmain.c)
 ----------------------------------------------------------------------------------------
 */
static unsigned long GAM_sdl_fn_ulGetTime(void)
{
    return (unsigned long)SDL_GetTicks();
}

/*
 ----------------------------------------------------------------------------------------
  Description   : Window title (SetWindowTextA + UpdateWindow in pc_winmain.c)
 ----------------------------------------------------------------------------------------
 */
static void GAM_sdl_fn_vSetTitle(const char *szTitle)
{
    if ( GAM_sdl_g_p_stWindow != NULL )
    {
        SDL_SetWindowTitle(GAM_sdl_g_p_stWindow, szTitle);
    }
}

/*
 ----------------------------------------------------------------------------------------
  Description   : Fullscreen again on activation: the device back to its display mode
 ----------------------------------------------------------------------------------------
 */
unsigned long __cdecl GAM_pc_fn_vHandleActivateEnterFullScreen(int iFlag)
{
    short hDevice = (short)FUN_00401340(0);

    if ( (char)GLI_fn_cWindowedModeIsOptimized() )
    {
        GAM_pc_fn_vSetDisplaySize(0x140, 200);
    }

    if ( GLD_bSetFullScreenModeForDevice(hDevice,
                                         GAM_pc_fn_ulGetDisplayWidth(),
                                         GAM_pc_fn_ulGetDisplayHeight(),
                                         0x10,
                                         (unsigned long)(*g_hWnd)) != 0 )
    {
        if ( GLD_bSwapDeviceMode(hDevice, iFlag) != 0 )
        {
            GAM_sdl_g_ulPauseFlag = 0;
            if ( (unsigned char)GAM_fn_ucGetEngineMode() != E_EM_ModeStoppingProgram )
            {
                GAM_fn_vActualizeAllGameViewportSize();
            }

            switch ( (unsigned char)GAM_fn_ucGetEngineMode() )
            {
                case 1: case 3: case 5:
                    FUN_0045edf0();
                    break;
            }
            return 1;
        }
    }

    GAM_pc_fn_vSetDisplaySize(GLI_fn_lDisplayModeWidth(), GLI_fn_lDisplayModeHeight());
    return 0;
}

/*
 ----------------------------------------------------------------------------------------
  Description   : Windowed on deactivation: the device to the windowed mode
 ----------------------------------------------------------------------------------------
 */
unsigned long __cdecl GAM_pc_fn_vHandleActivateExitToWindowed(int iFlag)
{
    short hDevice = (short)FUN_00401340(0);

    GAM_pc_fn_vSetDisplaySize(GLI_fn_lDisplayModeWidth(), GLI_fn_lDisplayModeHeight());

    if ( GLD_bSetFullScreenModeForDevice(hDevice,
                                         GAM_pc_fn_ulGetDisplayWidth(),
                                         GAM_pc_fn_ulGetDisplayHeight(),
                                         0x10,
                                         (unsigned long)(*g_hWnd)) != 0 )
    {
        if ( GLD_bSwapToWindowedMode(hDevice, iFlag) != 0 )
        {
            if ( (unsigned char)GAM_fn_ucGetEngineMode() != E_EM_ModeStoppingProgram )
            {
                GAM_fn_vActualizeAllGameViewportSize();
            }
            if ( (unsigned char)GAM_fn_ucGetEngineMode() != E_EM_ModePlaying )
            {
                FUN_0045edf0();
            }
            GAM_sdl_g_ulPauseFlag = 1;
            return 1;
        }
    }

    return 0;
}

/*
 ----------------------------------------------------------------------------------------
  Description   : Activation change of the device mode; minimizes the window on the way
                  out
 ----------------------------------------------------------------------------------------
 */
unsigned long GAM_pc_fn_vDispatchWmActivate(void)
{
    if ( GLI_xIsGliInit() == 0 )
    {
        return 0;
    }

    if ( GAM_sdl_g_ulPauseFlag != 0 )
    {
        GAM_pc_fn_vHandleActivateEnterFullScreen(0);
        GAM_g_stEngineStructure.eInputMode = 1;
        if ( GAM_sdl_g_p_stWindow != NULL )
        {
            GAM_sdl_fn_vSetTitle(GAM_SDL_SZ_PAUSE_TITLE);
#ifdef WIN32
            /* A fullscreen driver may have made the window a popup (GOG's OpenGL driver
               does). SDL is told: it puts its own style back when the window leaves the
               minimized state, and the game would return in a captioned window. */
            SDL_SetWindowBordered(GAM_sdl_g_p_stWindow,
                                  (GetWindowLongA((HWND)(*g_hWnd), GWL_STYLE) & WS_CAPTION) != 0);
#endif /* WIN32 */
            SDL_MinimizeWindow(GAM_sdl_g_p_stWindow);
        }
    }
    else
    {
        if ( GAM_sdl_g_p_stWindow != NULL
             && (SDL_GetWindowFlags(GAM_sdl_g_p_stWindow) & SDL_WINDOW_MINIMIZED) )
        {
            SDL_RestoreWindow(GAM_sdl_g_p_stWindow);
        }
        GAM_pc_fn_vHandleActivateExitToWindowed(0);
    }
    return 0;
}

/*
 ----------------------------------------------------------------------------------------
  Description   : Focus lost: the engine pauses before its next frame, sound released.
                  Not within 2 s of the last focus return, nor within 30 s of a level
                  init still running.
 ----------------------------------------------------------------------------------------
 */
unsigned long __cdecl GAM_pc_fn_vEnterPauseOnDeactivate(int iReDispatch)
{
    unsigned long ulNow;
    unsigned long ulPauseFlag;

    if ( GAM_sdl_g_ulPausePending == 0 )
    {
        return 0;
    }

    ulNow = GAM_sdl_fn_ulGetTime();
    if ( GAM_sdl_g_ulPendingCompan != 0 && (ulNow - GAM_sdl_g_ulDebounce30s) < 30000 )
    {
        return 1;
    }
    if ( (ulNow - GAM_sdl_g_ulDebounce2s) < 2000 )
    {
        return 1;
    }

    GAM_pc_fn_vResumeEngineThread();
    ulPauseFlag = GAM_sdl_g_ulPauseFlag;
    GAM_sdl_g_ulPausePending    = 0;
    GAM_sdl_g_ulPauseFlagMirror = ulPauseFlag;

    if ( iReDispatch != 0 && ulPauseFlag != 0 )
    {
        GAM_pc_fn_vDispatchWmActivate();
    }

    GAM_g_stEngineStructure.eInputMode = 1;
    GAM_sdl_fn_vSetTitle(GAM_SDL_SZ_PAUSE_TITLE);
    SND_fn_vReleaseDriverSound();
    return 0;
}

/*
 ----------------------------------------------------------------------------------------
  Description   : Focus back: device mode, title and sound restored, engine resumed
 ----------------------------------------------------------------------------------------
 */
int GAM_pc_fn_vRestoreEngineOnActivate(void)
{
    if ( GAM_sdl_g_ulPausePending == 0 && GAM_sdl_g_p_stWindow != NULL )
    {
        GAM_sdl_fn_vSetTitle(GAM_SDL_SZ_RESTORE_TITLE);
        GAM_sdl_g_ulPausePending = 1;
        if ( GAM_sdl_g_ulPauseFlagMirror != 0 && GAM_sdl_g_ulPauseFlag == 0 )
        {
            GAM_pc_fn_vDispatchWmActivate();
        }
        GAM_g_stEngineStructure.eInputMode = 0;
        GAM_sdl_fn_vSetTitle(GAM_SDL_SZ_WNDCLASS);
        SND_fn_vRestoreDriverSound();
        GAM_pc_fn_vEngineEventControl(3);
        GAM_sdl_g_ulDebounce2s = GAM_sdl_fn_ulGetTime();
    }
    return 0;
}

/*
 ----------------------------------------------------------------------------------------
  Description   : Display mode change the engine asked for (GLI_fn_lRequestNewDisplayMode).
                  Served between two frames, so the engine needs no pause around it.
 ----------------------------------------------------------------------------------------
 */
void GAM_pc_fn_vResumeAfterFocusReturn(void)
{
    if ( GLI_xIsGliInit() == 0 || GAM_sdl_g_ulPauseFlag == 0
         || GAM_sdl_g_ulPausePending == 0 )
    {
        return;
    }

    GLI_pc_fn_bApplyDeferredMode();
    GAM_pc_fn_vSetDisplaySize(GLI_fn_lDisplayModeWidth(), GLI_fn_lDisplayModeHeight());
}

/*
 ----------------------------------------------------------------------------------------
  Description   : Desktop depth (read by nothing, kept as on PC)
 ----------------------------------------------------------------------------------------
 */
unsigned long GAM_pc_fn_vProbeDisplayBpp(void)
{
    const SDL_DisplayMode *p_stMode = SDL_GetDesktopDisplayMode(SDL_GetPrimaryDisplay());

    GAM_sdl_g_bDesktopNot16Bpp = (p_stMode == NULL || SDL_BITSPERPIXEL(p_stMode->format) != 16);
    return 1;
}

/*
 ----------------------------------------------------------------------------------------
  Description   : The engine events: 1 exit, 2 pause at the next frame, 3 run, 4 one frame
 ----------------------------------------------------------------------------------------
 */
void GAM_pc_fn_vEngineEventControl(unsigned short uwCmd)
{
    switch ( uwCmd )
    {
        case 1:
            GAM_sdl_g_bEventExeFrame = 0;
            GAM_sdl_g_bEventExit     = 1;
            break;
        case 2:
            GAM_sdl_g_bEventExeFrame = 0;
            break;
        case 3:
            GAM_sdl_g_bEventExeFrame = 1;
            break;
        case 4:
            GAM_sdl_g_bEventPulse = 1;
            break;
    }
}

/*
 ----------------------------------------------------------------------------------------
  Description   : Engine to pause at its next frame (no thread to resume)
 ----------------------------------------------------------------------------------------
 */
void GAM_pc_fn_vResumeEngineThread(void)
{
    GAM_pc_fn_vEngineEventControl(2);
}

/*
 ----------------------------------------------------------------------------------------
  Description   : A request to the window: 0x22 activation, 0x25 display mode change,
                  served before the next frame. 0x20 (quit) needs nothing: the loop ends.
 ----------------------------------------------------------------------------------------
 */
void GAM_pc_fn_vPostWindowCmd(unsigned short uwCmd)
{
    SDL_Event stEvent;

    if ( GAM_sdl_g_ulWindowCmdEvent == 0 || (uwCmd != 0x22 && uwCmd != 0x25) )
    {
        return;
    }

    SDL_zero(stEvent);
    stEvent.type      = GAM_sdl_g_ulWindowCmdEvent;
    stEvent.user.code = uwCmd;
    SDL_PushEvent(&stEvent);
}

/*
 ----------------------------------------------------------------------------------------
  Description   : The draw semaphore: none with one thread (0, as on the original
                  engine's consoles); the renderer driver gets it as "DrawSem" and keeps
                  it unused
 ----------------------------------------------------------------------------------------
 */
HANDLE fn_hGetDrawSemaphore(void)
{
    return NULL;
}

/*
 ----------------------------------------------------------------------------------------
  Description   : Desktop depth (no draw semaphore with one thread)
 ----------------------------------------------------------------------------------------
 */
int GAM_pc_fn_bInitDisplayAndSync(void)
{
    if ( GAM_pc_fn_vProbeDisplayBpp() == 0 )
    {
        return 0;
    }
    GAM_pc_fn_vCreateSyncObjects();
    return 1;
}

void GAM_pc_fn_vPostResumeAfterFocus(void)
{
    GAM_pc_fn_vPostWindowCmd(0x25);
}

/*
 ----------------------------------------------------------------------------------------
  Description   : Nothing to create: no draw semaphore with one thread, the engine
                  events are the flags above
 ----------------------------------------------------------------------------------------
 */
void GAM_pc_fn_vCreateSyncObjects(void)
{
}

/*
 ----------------------------------------------------------------------------------------
  Description   : Engine side of the events: nothing to open; the loop has started
 ----------------------------------------------------------------------------------------
 */
void GAM_pc_fn_vOpenEngineEvents(void)
{
    GAM_g_ulQuitFlag = 1;
}

void FUN_00401080(void)
{
    GAM_pc_fn_vOpenEngineEvents();
}

/*
 ----------------------------------------------------------------------------------------
  Description   : Neither run nor exit asked: the engine has to pause
 ----------------------------------------------------------------------------------------
 */
int __cdecl GAM_pc_fn_bAreBothEventsIdle(void)
{
    return GAM_sdl_g_bEventExeFrame == 0 && GAM_sdl_g_bEventExit == 0
           && GAM_sdl_g_bEventPulse == 0;
}

/*
 ----------------------------------------------------------------------------------------
  Description   : End of the engine loop: back to GAM_pc_fn_WinMain
 ----------------------------------------------------------------------------------------
 */
void GAM_pc_fn_vEngineThreadExit(void)
{
    GAM_sdl_g_bEventIsDead = 1;
}

/*
 ----------------------------------------------------------------------------------------
  Description   : End of the game: exit asked, or the engine stops
 ----------------------------------------------------------------------------------------
 */
int FUN_00401100(void)
{
    if ( GAM_sdl_g_bEventExit )
    {
        return 1;
    }
    return GAM_fn_bIsStoppingProgram() & 0xFF;
}

/*
 ----------------------------------------------------------------------------------------
  Description   : End of the level loop: exit asked (the engine stops), or deinit
 ----------------------------------------------------------------------------------------
 */
int FUN_00401130(void)
{
    if ( GAM_sdl_g_bEventExit )
    {
        fn_vChangeEngineMode(E_EM_ModeStoppingProgram);
        return 1;
    }
    return GAM_fn_bIsDesInitMode() & 0xFF;
}

/*
 ----------------------------------------------------------------------------------------
  Description   : Quit: sound released, the engine leaves its loops
 ----------------------------------------------------------------------------------------
 */
void GAM_pc_fn_vSignalEngineAndPostQuit(void)
{
    GAM_sdl_g_ulPausePending = 0;
    SND_fn_vReleaseDriverSound();
    GAM_pc_fn_vEngineEventControl(1);
}

/*
 ----------------------------------------------------------------------------------------
  Description   : One SDL event, as GAM_fn_WndProc treats its Win32 counterpart
 ----------------------------------------------------------------------------------------
 */
static void GAM_sdl_fn_vDispatchEvent(const SDL_Event *p_stEvent)
{
    if ( p_stEvent->type == GAM_sdl_g_ulWindowCmdEvent && GAM_sdl_g_ulWindowCmdEvent != 0 )
    {
        switch ( p_stEvent->user.code )
        {
            case 0x22:
                if ( GAM_sdl_g_ulPauseFlag == 0 && GAM_sdl_g_ulPausePending != 0 )
                {
                    GAM_pc_fn_vDispatchWmActivate();
                }
                break;
            case 0x25:
                GAM_pc_fn_vResumeAfterFocusReturn();
                break;
        }
        return;
    }

    switch ( p_stEvent->type )
    {
        /* WM_CLOSE, SC_CLOSE -> WM_DESTROY */
        case SDL_EVENT_QUIT:
            SDL_ShowCursor();
            GAM_pc_fn_vSignalEngineAndPostQuit();
            break;

        /* WM_ACTIVATEAPP */
        case SDL_EVENT_WINDOW_FOCUS_GAINED:
            GAM_pc_fn_vRestoreEngineOnActivate();
            break;
        case SDL_EVENT_WINDOW_FOCUS_LOST:
        /* SC_MINIMIZE */
        case SDL_EVENT_WINDOW_MINIMIZED:
            GAM_pc_fn_vEnterPauseOnDeactivate(1);
            break;

        /* WM_SYSKEYDOWN VK_RETURN: Alt+Enter does not reach the game */
        case SDL_EVENT_KEY_DOWN:
            if ( p_stEvent->key.scancode == SDL_SCANCODE_RETURN
                 && (p_stEvent->key.mod & SDL_KMOD_ALT) )
            {
                IPT_fn_vResetInputEntry();
                FUN_0042e150();
            }
            break;
    }
}

/*
 ----------------------------------------------------------------------------------------
  Description   : The window events since the last frame
 ----------------------------------------------------------------------------------------
 */
static void GAM_sdl_fn_vPumpEvents(void)
{
    SDL_Event stEvent;

    while ( SDL_PollEvent(&stEvent) )
    {
        GAM_sdl_fn_vDispatchEvent(&stEvent);
    }
}

/*
 ----------------------------------------------------------------------------------------
  Description   : Paused: window events only, until the engine may run or has to exit
 ----------------------------------------------------------------------------------------
 */
void __cdecl GAM_pc_fn_vWaitForEitherEngineEvent(void)
{
    SDL_Event stEvent;

    while ( GAM_pc_fn_bAreBothEventsIdle() )
    {
        if ( SDL_WaitEvent(&stEvent) )
        {
            GAM_sdl_fn_vDispatchEvent(&stEvent);
        }
    }
    GAM_sdl_g_bEventPulse = 0;
}

/*
 ----------------------------------------------------------------------------------------
  Description   : One frame: window events, the pause when asked, then fn_vOneTrame
 ----------------------------------------------------------------------------------------
 */
void FUN_00401160(void)
{
    GAM_g_stEngineStructure.hGLDDevice   = FUN_00401340(0);
    GAM_g_stEngineStructure.hGLDViewport = FUN_00401350(0);
    GAM_g_stEngineStructure.hDrawSem     = fn_hGetDrawSemaphore();

    GAM_sdl_fn_vPumpEvents();

    if ( GAM_pc_fn_bAreBothEventsIdle() != 0 )
    {
        GAM_sdl_g_bEventIsPaused = 1;
        fn_vSetEngineInPaused();
        GAM_pc_fn_vWaitForEitherEngineEvent();
        fn_vResetEngineFromPaused();
        GAM_sdl_g_bEventIsPaused = 0;
    }
    else
    {
        GAM_sdl_g_bEventPulse = 0;
    }
#ifdef RAYMAN2_REGRESS
    REGRESS_fn_vBeforeFrame();
#endif /* RAYMAN2_REGRESS */
    fn_vOneTrame();
}

/*
 ----------------------------------------------------------------------------------------
  Description   : Level init, with the 30 s focus debounce while it runs
 ----------------------------------------------------------------------------------------
 */
void GAM_pc_fn_vChooseTheGoodInit(void)
{
    GAM_sdl_g_ulPendingCompan = 1;

    GAM_sdl_g_a4_pfnEngineCallback[1] = (void *)SND_PrintUsedStaticMemory;
    GAM_sdl_g_a4_pfnEngineCallback[0] = (void *)SND_PrintUsedStaticMemory;
    GAM_sdl_g_a4_pfnEngineCallback[3] = (void *)SND_PrintUsedStaticMemory;
    GAM_sdl_g_a4_pfnEngineCallback[2] = (void *)SND_PrintUsedStaticMemory;

    GAM_sdl_g_ulDebounce30s = GAM_sdl_fn_ulGetTime();

    GAM_fn_vChooseTheGoodInit();

    GAM_sdl_g_ulPendingCompan = 0;
}

/*
 ----------------------------------------------------------------------------------------
  Description   : The window (320x200 client, resizable) and the display device on it.
                  The window is shown once the device exists: on PC the small window
                  stays on screen while the driver starts. ERM gets no window: its
                  message boxes come from the main thread.
 ----------------------------------------------------------------------------------------
 */
int GAM_fn_bCreateMainWindow(HINSTANCE hInstance, int iShowCmd)
{
    (void)hInstance; (void)iShowCmd;

    GAM_sdl_g_p_stWindow = SDL_CreateWindow(GAM_SDL_SZ_WNDTITLE, 0x140, 200,
                                             SDL_WINDOW_RESIZABLE | SDL_WINDOW_HIDDEN);
    if ( GAM_sdl_g_p_stWindow == NULL )
    {
        return 0;
    }

#ifdef WIN32
    *g_hWnd = (HANDLE)SDL_GetPointerProperty(SDL_GetWindowProperties(GAM_sdl_g_p_stWindow),
                                             SDL_PROP_WINDOW_WIN32_HWND_POINTER, NULL);
#else
    /* No Win32 window: the engine and its linked-in driver get SDL's window as their HWND,
       which the target's types header declares (as the original's console headers do) */
    *g_hWnd = (HANDLE)GAM_sdl_g_p_stWindow;
#endif /* WIN32 */
    if ( *g_hWnd == NULL )
    {
        return 0;
    }

    if ( !fn_bCreateMainDisplayScreen() )
    {
        return 0;
    }
    SDL_ShowWindow(GAM_sdl_g_p_stWindow);
    return 1;
}

/*
 ----------------------------------------------------------------------------------------
  Description   : Application entry: ubi.ini, single instance, SDL, engine init, window,
                  then the engine loop on this thread
 ----------------------------------------------------------------------------------------
 */
int __stdcall GAM_pc_fn_WinMain(HINSTANCE hInstance,
                                HINSTANCE hPrevInstance,
                                char     *p_szCmdLine,
                                int       iShowCmd)
{
    char  szIniPath[260];
    FILE *p_stFile;
    int   iResult = -1;

#if defined(WIN32) && !defined(RAYMAN2_LOCAL_UBIINI)
    GetWindowsDirectoryA(szIniPath, 0x104);
    strcat(szIniPath, "\\UbiSoft");
    if ( _access(szIniPath, 0) != 0 )
    {
        CreateDirectoryA(szIniPath, NULL);
    }
#else
    /* ubi.ini in the working directory (the game folder); other targets too, until the
       user folder is decided (docs/PLAN.md "User folder") */
    strcpy(szIniPath, ".");
#endif /* WIN32 && !RAYMAN2_LOCAL_UBIINI */
    strcat(szIniPath, "\\ubi.ini");
    if ( _access(szIniPath, 0) != 0 )
    {
        p_stFile = fopen(szIniPath, "wt");
        if ( p_stFile != NULL )
        {
            fprintf(p_stFile, "[Rayman2]\r\nChoose = 1\r\n");
            fclose(p_stFile);
        }
    }

    if ( hPrevInstance != NULL )
    {
        return -1;
    }

#ifdef WIN32
    CreateMutexA(NULL, TRUE, "RAYMAN2EXE");
    if ( GetLastError() == ERROR_ALREADY_EXISTS )
    {
        return -1;
    }
#endif /* WIN32 */

    SDL_SetMainReady();
    if ( !SDL_Init(SDL_INIT_VIDEO) )
    {
        return -1;
    }
    GAM_sdl_g_ulWindowCmdEvent = SDL_RegisterEvents(1);

    if ( GAM_pc_fn_bInitDisplayAndSync() == 0 )
    {
        SDL_Quit();
        return -1;
    }

    *g_hInstance = hInstance;
    strcpy(GAM_g_szCmdLine, p_szCmdLine);

#ifdef WIN32
    SetErrorMode(SEM_FAILCRITICALERRORS);
#endif /* WIN32 */

    MMG_fn_vInit(1);

    fn_vInitEngineWhenInitApplication();

    if ( GAM_fn_bCreateMainWindow(hInstance, iShowCmd) != 0 )
    {
        FUN_00401050();

        /* the screensaver stays off while SDL video is up */
        SDL_HideCursor();
        fn_uiEngineMainLoop();
        SDL_ShowCursor();

        FUN_00401b20();
        GAM_pc_fn_vFreeMmgTableAndHelperDll();
        iResult = 0;
    }

    if ( GAM_sdl_g_p_stWindow != NULL )
    {
        SDL_DestroyWindow(GAM_sdl_g_p_stWindow);
    }
    SDL_Quit();
    return iResult;
}

#endif /* USE_SDL */
