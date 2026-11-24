#include "GAM/GAM_pc.h"
#include "GAM/GAM_globals.h"
#include "GAM/GAM_types.h"
#include "GLI/misc_trivial.h"
#include "GLI/Camera.h"
#include "POS/POS_types.h"
#include "MTH/MTH3D.h"
#include "ERM/CpaError.h"
#include "SND/SND.h"

void __cdecl MMG_fn_vInit(int bInitMMG);

#  define GAM_PC_SZ_WNDCLASS  "Rayman2"
#  define GAM_PC_SZ_WNDTITLE  "Rayman II"

#include "MMG/MMG_globals.h"

long __cdecl GLI_xIsGliInit(void);

/* 0x00421630 */
long __cdecl GLD_bReleaseViewportAndDevice(short hDev, short hVP);

int __cdecl GAM_pc_fn_bAreBothEventsIdle(void);

__declspec(dllimport) DWORD WINAPI timeGetTime(void);
#pragma comment(lib, "winmm.lib")

#include <io.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "port/regress.h"

/* 0x0049f07c */
HINSTANCE g_hInstance_storage = (HINSTANCE)-1;

/* USE_SDL: the window, message loop, engine thread, events and focus handling of this file
   (the blocks fenced #ifndef USE_SDL) are in GAM/sdl_main.c; the engine code stays shared */
#ifndef USE_SDL
/* 0x004e0d80 */
static HANDLE g_hSemDraw_storage, g_hEventMsgBox_storage, g_hEventExeFrame_storage,
              g_hEventExit_storage, g_hEventIsDead_storage, g_hEventIsPaused_storage;
#define g_hSemDraw        (&g_hSemDraw_storage)
#define g_hEventMsgBox    (&g_hEventMsgBox_storage)
#define g_hEventExeFrame  (&g_hEventExeFrame_storage)
#define g_hEventExit      (&g_hEventExit_storage)
#define g_hEventIsDead    (&g_hEventIsDead_storage)
#define g_hEventIsPaused  (&g_hEventIsPaused_storage)

/* 0x0049f0a8 */
static const char GAM_pc_g_szPauseTitle[]   = "Rayman2 - pause";
static const char GAM_pc_g_szRestoreTitle[] = "Rayman2 - restoring data... please wait";
#define GAM_PC_SZ_PAUSE_TITLE    GAM_pc_g_szPauseTitle
#define GAM_PC_SZ_RESTORE_TITLE  GAM_pc_g_szRestoreTitle
#endif /* USE_SDL */

/* 0x0049F084 */
void *GAM_pc_g_hEngineThread_storage = (void *)-1;

/* 0x4E0EC8 */
HANDLE g_hEngineEventExeFrame_storage, g_hEngineEventExit_storage,
       g_hEngineEventIsDead_storage, g_hEngineEventIsPaused_storage;

static unsigned long g_ulDisplayWidth_storage, g_ulDisplayHeight_storage;
#define g_ulDisplayWidth  (&g_ulDisplayWidth_storage)
#define g_ulDisplayHeight (&g_ulDisplayHeight_storage)

static unsigned long g_ulHwndGoneFlag_storage;   /* 0x004A6AA8 */
#define GAM_pc_g_ulHwndGoneFlag  g_ulHwndGoneFlag_storage

#ifndef USE_SDL
static unsigned long GAM_pc_g_ulPauseFlag_storage = 1;
static unsigned long GAM_pc_g_ulPausePending_storage = 1;
#define GAM_pc_g_ulPauseFlag     GAM_pc_g_ulPauseFlag_storage
#define GAM_pc_g_ulPausePending  GAM_pc_g_ulPausePending_storage

static unsigned long GAM_pc_g_ulPauseFlagMirror_storage;
static unsigned long GAM_pc_g_ulDebounce2s_storage;
static unsigned long GAM_pc_g_ulDebounce30s_storage;
#define GAM_pc_g_ulPauseFlagMirror  GAM_pc_g_ulPauseFlagMirror_storage
#define GAM_pc_g_ulDebounce2s       GAM_pc_g_ulDebounce2s_storage
#define GAM_pc_g_ulDebounce30s      GAM_pc_g_ulDebounce30s_storage

/* 0x004A6AAC */
static unsigned long GAM_pc_g_ulNoPauseThisTime_storage;
#define GAM_pc_g_ulNoPauseThisTime  GAM_pc_g_ulNoPauseThisTime_storage

/* 0x0049F0E4 */
static unsigned long GAM_pc_g_ulPendingCompan_storage = 1;
#define GAM_pc_g_ulPendingCompan  GAM_pc_g_ulPendingCompan_storage
#endif /* USE_SDL */

static unsigned long g_bDisplayFullScreen_storage;
#define g_bDisplayFullScreen  g_bDisplayFullScreen_storage

static short         p_hDevice_storage;             /* 0x004E0EA4 */
static short         p_hViewport_storage;           /* 0x004E0EB8 */
static unsigned long p_ulSpecificAttribs_storage;   /* 0x004E0EB0 */
#define              p_hDevice           (&p_hDevice_storage)
#define              p_hViewport         (&p_hViewport_storage)
#define              p_ulSpecificAttribs (&p_ulSpecificAttribs_storage)

/* 0x004013c0 */
void GAM_pc_fn_vGetAppContext(unsigned long *p_aContext)
{
    p_aContext[0] = (unsigned long)(*g_hInstance);
    p_aContext[1] = (unsigned long)(*g_hWnd);
    p_aContext[2] = 0;
}

/* 0x00401400 */
HINSTANCE GAM_pc_fn_hGetAppInstance(void)
{
    return *g_hInstance;
}

/* 0x004013a0 */
void __cdecl GAM_pc_fn_vSetDisplaySize(unsigned long ulWidth,
                                        unsigned long ulHeight)
{
    *g_ulDisplayWidth  = ulWidth;
    *g_ulDisplayHeight = ulHeight;
}

unsigned long GAM_pc_fn_ulGetDisplayWidth(void)
{
    return *g_ulDisplayWidth;
}

unsigned long GAM_pc_fn_ulGetDisplayHeight(void)
{
    return *g_ulDisplayHeight;
}

/* 0x004013b0 */
void __cdecl GAM_pc_fn_vSetDisplayFullScreen(unsigned long bFullScreen)
{
    g_bDisplayFullScreen = bFullScreen;
}

#ifndef USE_SDL
/* 0x00401820 */
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
extern long          __cdecl GLI_pc_fn_bApplyDeferredMode(void);   /* 0x00422300 */

unsigned long __cdecl GAM_pc_fn_vHandleActivateEnterFullScreen(int param_1)
{
    if (WaitForSingleObject(*g_hSemDraw, 500) == WAIT_TIMEOUT)
    {
        return 0;
    }

    if ((char)GLI_fn_cWindowedModeIsOptimized())
    {
        GAM_pc_fn_vSetDisplaySize(0x140, 200);
    }

    if (GLD_bSetFullScreenModeForDevice(*(unsigned short *)p_hDevice,
                                    GAM_pc_fn_ulGetDisplayWidth(),
                                    GAM_pc_fn_ulGetDisplayHeight(),
                                    0x10,
                                    (unsigned long)(*g_hWnd)) != 0)
    {
        if (GLD_bSwapDeviceMode(*(unsigned short *)p_hDevice, param_1) != 0)
        {
            GAM_pc_g_ulPauseFlag = 0;
            if ((unsigned char)GAM_fn_ucGetEngineMode() != E_EM_ModeStoppingProgram)
            {
                GAM_fn_vActualizeAllGameViewportSize();
            }
            ReleaseSemaphore(*g_hSemDraw, 1, NULL);

            switch ((unsigned char)GAM_fn_ucGetEngineMode())
            {
                case 1: case 3: case 5:
                    FUN_0045edf0();
                    break;
            }
            return 1;
        }
    }

    GAM_pc_fn_vSetDisplaySize(GLI_fn_lDisplayModeWidth(), GLI_fn_lDisplayModeHeight());
    ReleaseSemaphore(*g_hSemDraw, 1, NULL);
    return 0;
}

/* 0x00401910 */
unsigned long __cdecl GAM_pc_fn_vHandleActivateExitToWindowed(int param_1)
{
    if (WaitForSingleObject(*g_hSemDraw, 500) == WAIT_TIMEOUT)
    {
        return 0;
    }

    GAM_pc_fn_vSetDisplaySize(GLI_fn_lDisplayModeWidth(), GLI_fn_lDisplayModeHeight());

    if (GLD_bSetFullScreenModeForDevice(*(unsigned short *)p_hDevice,
                                    GAM_pc_fn_ulGetDisplayWidth(),
                                    GAM_pc_fn_ulGetDisplayHeight(),
                                    0x10,
                                    (unsigned long)(*g_hWnd)) != 0)
    {
        if (GLD_bSwapToWindowedMode(*(unsigned short *)p_hDevice, param_1) != 0)
        {
            if ((unsigned char)GAM_fn_ucGetEngineMode() != E_EM_ModeStoppingProgram)
            {
                GAM_fn_vActualizeAllGameViewportSize();
            }
            ReleaseSemaphore(*g_hSemDraw, 1, NULL);
            if ((unsigned char)GAM_fn_ucGetEngineMode() != E_EM_ModePlaying)
            {
                FUN_0045edf0();
            }
            GAM_pc_g_ulPauseFlag = 1;
            return 1;
        }
    }

    ReleaseSemaphore(*g_hSemDraw, 1, NULL);
    return 0;
}

/* 0x0049f0a8 */
unsigned long GAM_pc_fn_vDispatchWmActivate(void)
{

    HWND          hWnd;
    unsigned long ulResult;

    if (GLI_xIsGliInit() == 0)
    {
        return 0;
    }

    ulResult = 0;
    if (GAM_pc_g_ulPauseFlag != 0)
    {
        GAM_pc_fn_vHandleActivateEnterFullScreen(0);
        hWnd = (HWND)(*g_hWnd);
        GAM_g_stEngineStructure.eInputMode = 1;
        if (hWnd != NULL)
        {
            SetWindowTextA(hWnd, GAM_PC_SZ_PAUSE_TITLE);
            UpdateWindow(hWnd);
            DefWindowProcA(hWnd, WM_SYSCOMMAND, SC_MINIMIZE, 0);
        }
        else
        {
            GAM_pc_g_ulHwndGoneFlag = 1;
        }
    }
    else
    {
        hWnd = (HWND)(*g_hWnd);
        if (IsIconic(hWnd))
        {
            DefWindowProcA(hWnd, WM_SYSCOMMAND, SC_RESTORE, 0);
        }
        GAM_pc_fn_vHandleActivateExitToWindowed(0);
    }
    return ulResult;
}

extern void __cdecl SND_fn_vReleaseDriverSound(void);   /* 0x00453790 */
extern void __cdecl SND_fn_vRestoreDriverSound(void);   /* 0x004537A0 */
unsigned long __cdecl GAM_pc_fn_vEnterPauseOnDeactivate(int param_1)
{

    DWORD         ulNow;
    HWND          hWnd;
    unsigned long ulPauseFlag;

    if (GAM_pc_g_ulPausePending == 0)
    {
        return 0;
    }

    ulNow = timeGetTime();
    if (GAM_pc_g_ulPendingCompan != 0 && (ulNow - GAM_pc_g_ulDebounce30s) < 30000)
    {
        return 1;
    }
    if ((ulNow - GAM_pc_g_ulDebounce2s) < 2000)
    {
        return 1;
    }

    GAM_pc_fn_vResumeEngineThread();
    ulPauseFlag = GAM_pc_g_ulPauseFlag;
    GAM_pc_g_ulPausePending    = 0;
    GAM_pc_g_ulPauseFlagMirror = ulPauseFlag;

    if (param_1 != 0 && ulPauseFlag != 0)
    {
        GAM_pc_fn_vDispatchWmActivate();
    }

    hWnd = (HWND)(*g_hWnd);
    GAM_g_stEngineStructure.eInputMode = 1;
    if (hWnd != NULL)
    {
        SetWindowTextA(hWnd, GAM_PC_SZ_PAUSE_TITLE);
        UpdateWindow(hWnd);
    }
    SND_fn_vReleaseDriverSound();
    return 0;
}

/* 0x00401a80 */
void GAM_pc_fn_vResumeAfterFocusReturn(void)
{

    int iInitialPoll;

    if (GLI_xIsGliInit() == 0 || GAM_pc_g_ulPauseFlag == 0
        || GAM_pc_g_ulPausePending == 0)
    {
        return;
    }

    iInitialPoll = GAM_pc_fn_bAreBothEventsIdle();
    if (iInitialPoll == 0)
    {
        GAM_pc_fn_vResumeEngineThread();
        while (GAM_pc_fn_bAreBothEventsIdle() == 0)
        {

        }
    }

    if (WaitForSingleObject(*g_hSemDraw, 500) == WAIT_OBJECT_0)
    {
        GLI_pc_fn_bApplyDeferredMode();
        GAM_pc_fn_vSetDisplaySize(GLI_fn_lDisplayModeWidth(), GLI_fn_lDisplayModeHeight());
        ReleaseSemaphore(*g_hSemDraw, 1, NULL);
    }

    if (iInitialPoll == 0)
    {
        GAM_pc_fn_vEngineEventControl(3);
    }
}

/* 0x004A6A98 */
static int GAM_pc_g_bDesktopNot16Bpp;

unsigned long GAM_pc_fn_vProbeDisplayBpp(void)
{
    HDC hdc = GetDC(NULL);
    int iPlanes = GetDeviceCaps(hdc, PLANES);
    int iBits = iPlanes * GetDeviceCaps(hdc, BITSPIXEL);
    ReleaseDC(NULL, hdc);
    GAM_pc_g_bDesktopNot16Bpp = (iBits != 16);
    return 1;
}

/* 0x004e0d88 */
void GAM_pc_fn_vEngineEventControl(unsigned short uwCmd)
{
    switch (uwCmd)
    {
        case 1:
            ResetEvent(*g_hEventExeFrame);
            SetEvent  (*g_hEventExit);
            break;
        case 2:
            ResetEvent(*g_hEventExeFrame);
            break;
        case 3:
            SetEvent  (*g_hEventExeFrame);
            break;
        case 4:
            PulseEvent(*g_hEventExeFrame);
            break;
    }
}

/* 0x004e0d94 */
void GAM_pc_fn_vResumeEngineThread(void)
{
    GAM_pc_fn_vEngineEventControl(2);
    ResumeThread       (*(HANDLE *)GAM_pc_g_hEngineThread_ADDR);
    WaitForSingleObject(*g_hEventIsPaused, 200);
}

/* 0x00401650 */
void GAM_pc_fn_vPostWindowCmd(unsigned short uwCmd)
{

    switch (uwCmd)
    {
        case 0x20:
            PostMessageA((HWND)(*g_hWnd), WM_QUIT, 0, 0);
            break;
        case 0x22:
            PostMessageA((HWND)(*g_hWnd), WM_USER, 0x22, 0);
            break;
        case 0x24:
            PostMessageA((HWND)(*g_hWnd), WM_USER, 0x24, 0);
            break;
        case 0x25:
            PostMessageA((HWND)(*g_hWnd), WM_USER, 0x25, 0);
            break;
    }
}

/* 0x004013e0 */
HANDLE fn_hGetDrawSemaphore(void)
{
    return *g_hSemDraw;
}

/* 0x00401420 */
int GAM_pc_fn_bInitDisplayAndSync(void)
{
    if (GAM_pc_fn_vProbeDisplayBpp() == 0)
    {
        return 0;
    }
    GAM_pc_fn_vCreateSyncObjects();
    return 1;
}

/* 0x00401410 */
void GAM_pc_fn_vPostResumeAfterFocus(void)
{
    GAM_pc_fn_vPostWindowCmd(0x25);
}

/* 0x004E0D80 */
void GAM_pc_fn_vCreateSyncObjects(void)
{
    *g_hSemDraw       = CreateSemaphoreA(NULL, 1, 1, "DRAWSEM");
    *g_hEventMsgBox   = CreateEventA(NULL, TRUE, FALSE, "MSGBOX");
    *g_hEventExeFrame = CreateEventA(NULL, TRUE, TRUE,  "EXEFRAMEEVT");
    *g_hEventExit     = CreateEventA(NULL, TRUE, FALSE, "EXITEVT");
    *g_hEventIsDead   = CreateEventA(NULL, TRUE, FALSE, "ISDEADEVT");
    *g_hEventIsPaused = CreateEventA(NULL, TRUE, FALSE, "ISPAUSEDEVT");
}

/* 0x004A6A90 */
void GAM_pc_fn_vOpenEngineEvents(void)
{
    *g_hEngineEventExeFrame = OpenEventA(EVENT_ALL_ACCESS, TRUE, "EXEFRAMEEVT");
    *g_hEngineEventExit     = OpenEventA(EVENT_ALL_ACCESS, TRUE, "EXITEVT");
    *g_hEngineEventIsDead   = OpenEventA(EVENT_ALL_ACCESS, TRUE, "ISDEADEVT");
    *g_hEngineEventIsPaused = OpenEventA(EVENT_ALL_ACCESS, TRUE, "ISPAUSEDEVT");
    GAM_g_ulQuitFlag = 1;   /* 0x004A6A90 */

}

/* 0x00401080 */
void FUN_00401080(void)
{
    GAM_pc_fn_vOpenEngineEvents();
}

/* 0x004012D0 */
int __cdecl GAM_pc_fn_bAreBothEventsIdle(void)
{
    HANDLE h[2];
    h[0] = *g_hEngineEventExeFrame;
    h[1] = *g_hEngineEventExit;
    return WaitForMultipleObjects(2, h, FALSE, 0) == WAIT_TIMEOUT;
}

/* 0x00401090 */
void GAM_pc_fn_vEngineThreadExit(void)
{
    GAM_pc_fn_vPostWindowCmd(0x20);
    SetEvent(*g_hEngineEventIsDead);
    Sleep(INFINITE);
}

/* 0x004046C0 */
int FUN_00401100(void)
{

    if (WaitForSingleObject(*g_hEngineEventExit, 0) == WAIT_OBJECT_0)
    {
        return 1;
    }
    return GAM_fn_bIsStoppingProgram() & 0xFF;
}

/* 0x00404BF0 */
int FUN_00401130(void)
{

    if (WaitForSingleObject(*g_hEngineEventExit, 0) == WAIT_OBJECT_0)
    {
        /* 0x00404BF0 */
        fn_vChangeEngineMode(E_EM_ModeStoppingProgram);
        return 1;
    }
    return GAM_fn_bIsDesInitMode() & 0xFF;
}
#else
void FUN_00401080(void);
void FUN_00401160(void);
#endif /* USE_SDL */

/* 0x401340 */
static void fn_vEmptyFunction(void) { }
void __cdecl GAM_fn_vEngine(void);
void __cdecl fn_vDisplayAll(void);
void __cdecl fn_vEngineReadInput(void);
static void (__cdecl *p_fn_vNetEngine)(void)   = fn_vEmptyFunction;

/* 0x0049F37C */
void (__cdecl *p_fn_vEngine)(void)             = fn_vEmptyFunction;
/* 0x0049F380 */
void (__cdecl *p_fn_vDisplayAll)(void)         = fn_vEmptyFunction;
static void (__cdecl *p_fn_vReadInput)(void)   = fn_vEngineReadInput;

void fn_vOneTrame(void)
{

    GAM_g_stEngineStructure.hGLDDevice   = (short)FUN_00401340(0);
    GAM_g_stEngineStructure.hGLDViewport = (short)FUN_00401350(0);
    GAM_g_stEngineStructure.hDrawSem     = fn_hGetDrawSemaphore();

    p_fn_vNetEngine();
    if (GAM_fn_bIsStoppingProgram())
        return;
    p_fn_vEngine();
    if (GAM_g_stEngineStructure.eEngineMode == E_EM_ModeStoppingProgram)
        return;
    p_fn_vDisplayAll();
    p_fn_vReadInput();
}

#ifndef USE_SDL
/* 0x00401310 */
void FUN_00401160(void)
{

    GAM_g_stEngineStructure.hGLDDevice   = FUN_00401340(0);
    GAM_g_stEngineStructure.hGLDViewport = FUN_00401350(0);
    GAM_g_stEngineStructure.hDrawSem     = fn_hGetDrawSemaphore();

    if (GAM_pc_fn_bAreBothEventsIdle() != 0)
    {
        SetEvent(*g_hEngineEventIsPaused);
        fn_vSetEngineInPaused();
        GAM_pc_fn_vWaitForEitherEngineEvent();
        fn_vResetEngineFromPaused();
        ResetEvent(*g_hEngineEventIsPaused);
    }
#ifdef RAYMAN2_REGRESS
    REGRESS_fn_vBeforeFrame();
#endif
    fn_vOneTrame();             /* 0x004049E0 */
}
#endif /* USE_SDL */

/* 0x401340 */
unsigned short __cdecl FUN_00401340(int _unused)
{
    (void)_unused;
    return *(unsigned short *)p_hDevice;
}

unsigned short __cdecl FUN_00401350(int _unused)
{
    (void)_unused;
    return *(unsigned short *)p_hViewport;
}

/* 0x00401360 */
char *__cdecl GAM_pc_fn_szGetCmdLine(void)
{
    return GAM_g_szCmdLine;
}

#ifndef USE_SDL
void __cdecl GAM_pc_fn_vWaitForEitherEngineEvent(void)
{
    /* 0x4e0ec8 */
    HANDLE h[2];
    h[0] = *g_hEngineEventExeFrame;
    h[1] = *g_hEngineEventExit;
    WaitForMultipleObjects(2, h, FALSE, INFINITE);
}

/* 0x004E0ED8 */
static void *GAM_pc_g_a4_pfnEngineCallback[4];

void GAM_pc_fn_vChooseTheGoodInit(void)
{
    GAM_pc_g_ulPendingCompan = 1;

    /* 0x00403290 */
    GAM_pc_g_a4_pfnEngineCallback[1] = (void *)SND_PrintUsedStaticMemory;
    GAM_pc_g_a4_pfnEngineCallback[0] = (void *)SND_PrintUsedStaticMemory;
    GAM_pc_g_a4_pfnEngineCallback[3] = (void *)SND_PrintUsedStaticMemory;
    GAM_pc_g_a4_pfnEngineCallback[2] = (void *)SND_PrintUsedStaticMemory;

    GAM_pc_g_ulDebounce30s = timeGetTime();

    GAM_fn_vChooseTheGoodInit();                     /* 0x00404700 */

    GAM_pc_g_ulPendingCompan = 0;
}
#endif /* USE_SDL */

/* 0x004010F0 */
void GAM_pc_fn_vChooseTheGoodDesInit(void)
{
    fn_vChooseTheGoodDesInit();
}

/* 0x401540 */
unsigned int fn_uiEngineMainLoop(void)
{

    GLI_fn_vNonOptimizedWindowedMode();        /* 0x422040 */
    FUN_00401080();
    if (FUN_00401100() == 0)
    {
        do
        {
            GAM_pc_fn_vChooseTheGoodInit();    /* 0x4010b0 */
            GLI_fn_vOptimizedWindowedMode();   /* 0x422030 */
            if (FUN_00401130() == 0)
            {
                do
                {
                    FUN_00401160();
                } while (FUN_00401130() == 0);
            }
            GLI_fn_vNonOptimizedWindowedMode(); /* 0x422040 */
            GAM_pc_fn_vChooseTheGoodDesInit();
        } while (FUN_00401100() == 0);
    }
    GAM_pc_fn_vEngineThreadExit();             /* 0x401090 */

}

#ifndef USE_SDL
/* 0x00401540 */
DWORD WINAPI GAM_pc_fn_ulEngineThreadProc(LPVOID lpParameter)
{
    (void)lpParameter;
    fn_uiEngineMainLoop();
    return 0;
}

/* 0x00401540 */
void GAM_pc_fn_vCreateEngineThread(void)
{
    DWORD  ulThreadId;
    HANDLE hThread;

    hThread = CreateThread(NULL,
                           0,
                           GAM_pc_fn_ulEngineThreadProc,
                           NULL,
                           0,
                           &ulThreadId);
    *(HANDLE *)GAM_pc_g_hEngineThread_ADDR = hThread;
    SetThreadPriority(hThread, 0x80);
}

/* 0x00401d40 */
void GAM_pc_fn_vKillEngineThreadAfterExit(void)
{
    WaitForSingleObject(*g_hEventIsDead, INFINITE);
    TerminateThread(*(HANDLE *)GAM_pc_g_hEngineThread_ADDR, 0);
}
#endif /* USE_SDL */

/* 0x004E0EA4 */
unsigned long FUN_00401b20(void)
{
    /* 0x00404540 */
    extern void __cdecl FUN_00404540(void);

    fn_vChangeEngineMode(2);
    /* 0x00421630 */
    GLD_bReleaseViewportAndDevice(*(unsigned short *)p_hDevice,
                                  *(unsigned short *)p_hViewport);
    FUN_00404540();
    return 0;
}

#ifndef USE_SDL
/* 0x0049B1D0 */
int FUN_00402580(MSG *p_msg)
{
    int iRet = 0;

    switch (p_msg->message)
    {
    case 0x001C:
        if (p_msg->wParam != 0)
            iRet = GAM_pc_fn_vRestoreEngineOnActivate();  /* 0x49B1D0 */
        else
            iRet = GAM_pc_fn_vEnterPauseOnDeactivate(1);
        break;
    case 0x0400:
        iRet = GAM_pc_fn_vDispatchWmUser(p_msg->wParam, p_msg->lParam);
        break;
    }
    return iRet;
}
#endif /* USE_SDL */

/* 0x510BF0 */
void GAM_pc_fn_vFreeMmgTableAndHelperDll(void)
{
    /* 0x0049C080 */
    typedef void (__stdcall *pfn_iat_t)(void *);

    unsigned int *p_ulMMGTable = g_p_ulMMGTable;   /* 0x00510BA4 */
#ifdef RAYMAN2_FIX_BUGS
    /* FIX(BUGS.md#6) */
    /* the PSAPI handle was read from the table after it was freed (or through NULL) */
    if (p_ulMMGTable != NULL)
    {
        if (MMG_pc_g_ulPsapiLoaded != 0)
            FreeLibrary((HMODULE)(void *)(unsigned int)p_ulMMGTable[2]);
        free(p_ulMMGTable);
    }
#else
    if (p_ulMMGTable != NULL)
    {
        free(p_ulMMGTable);
    }
    if (MMG_pc_g_ulPsapiLoaded != 0)
    {

        FreeLibrary((HMODULE)(void *)(unsigned int)g_p_ulMMGTable[2]);
    }
#endif /* RAYMAN2_FIX_BUGS */
}

/* 0x00401D40 */
void FUN_00401050(void)
{
    extern long __cdecl GLD_bClearDevice(short hDev);
    extern long __cdecl GLD_bFlipDeviceWithSynchro(short hDev_unused, int eSynchro_unused);

    GLD_bClearDevice(*(unsigned short *)p_hDevice);
    GLD_bFlipDeviceWithSynchro(*(unsigned short *)p_hDevice, 0xFF);
}

#ifndef USE_SDL
/* 0x004A6AA0 */
int GAM_pc_fn_vRestoreEngineOnActivate(void)
{
    unsigned long *p_ulPausePending  = &GAM_pc_g_ulPausePending;
    unsigned long *p_ulPauseFlagMirr = &GAM_pc_g_ulPauseFlagMirror;
    unsigned long *p_ulPauseFlag     = &GAM_pc_g_ulPauseFlag;
    unsigned char *p_bTitleOverride  = &GAM_g_stEngineStructure.eInputMode;
    HWND           hWnd;

    if (*p_ulPausePending == 0 && (hWnd = (HWND)(*g_hWnd)) != NULL)
    {
        SetWindowTextA(hWnd, "Rayman2 - restoring data... please wait");
        UpdateWindow(hWnd);
        if (*(HANDLE *)GAM_pc_g_hEngineThread_ADDR != INVALID_HANDLE_VALUE)
        {
            *p_ulPausePending = 1;
            if (*p_ulPauseFlagMirr != 0 && *p_ulPauseFlag == 0)
            {
                GAM_pc_fn_vDispatchWmActivate();
            }
            *p_bTitleOverride = 0;
            SetWindowTextA(hWnd, GAM_PC_SZ_WNDCLASS);  /* 0x0049F0A0 */
            UpdateWindow(hWnd);
            SND_fn_vRestoreDriverSound();
            GAM_pc_fn_vEngineEventControl(3);
            GAM_pc_g_ulDebounce2s = timeGetTime();
        }
    }
    return 0;
}

/* 0x004A6AAC */
typedef struct GAM_pc_tdstMessageBoxRequest_
{
    LPCSTR szText;
    LPCSTR szTitle;
    UINT   uiStyle;
    int    iResult;
} GAM_pc_tdstMessageBoxRequest;

int GAM_pc_fn_vDispatchWmUser(WPARAM wParam, LPARAM lParam)
{
    DWORD uVar3 = GAM_pc_g_ulPendingCompan;
    HWND  hWnd  = (HWND)(*g_hWnd);
    int   uRet  = 0;

    switch (wParam)
    {
        case 0x21:
            SetWindowTextA(hWnd, (LPCSTR)lParam);
            UpdateWindow(hWnd);
            free((void *)lParam);
            return 1;

        case 0x22:
            if (GAM_pc_g_ulPauseFlag == 0 &&
                GAM_pc_g_ulPausePending != 0)
            {
                return (int)GAM_pc_fn_vDispatchWmActivate();
            }
            break;

        case 0x24:
        {
            int iResult;
            GAM_pc_g_ulPendingCompan = 0;
            ShowCursor(TRUE);
            GAM_pc_fn_vEnterPauseOnDeactivate(1);
            GAM_pc_g_ulNoPauseThisTime = 1;       /* 0x004A6AAC */
            iResult = MessageBoxA(
                hWnd,
                ((GAM_pc_tdstMessageBoxRequest *)lParam)->szText,
                ((GAM_pc_tdstMessageBoxRequest *)lParam)->szTitle,
                ((GAM_pc_tdstMessageBoxRequest *)lParam)->uiStyle | MB_TOPMOST);
            ((GAM_pc_tdstMessageBoxRequest *)lParam)->iResult = iResult;
            GAM_pc_g_ulNoPauseThisTime = 0;       /* 0x004A6AAC */
            ShowCursor(FALSE);
            GAM_pc_g_ulPendingCompan = uVar3;
            return 1;
        }

        case 0x25:
            GAM_pc_fn_vResumeAfterFocusReturn();
            uRet = 1;
            break;
    }
    return uRet;
}

/* 0x004536D0 */
int GAM_pc_fn_vHandleWmPowerBroadcast(WPARAM wParam, LPARAM lParam,
                                      LRESULT *p_lResult)
{
    (void)wParam;

    switch (lParam)
    {
        case 0:
        case 1:
            *p_lResult = 1;
            return 1;

        case 2:
        case 3:
        case 9:
        case 10:
        case 0xB:
            break;

        case 4:
        case 5:
            if (GAM_pc_g_ulPausePending != 0 &&
                *(HANDLE *)GAM_pc_g_hEngineThread_ADDR != INVALID_HANDLE_VALUE)
            {
                WaitForSingleObject(*g_hSemDraw, INFINITE);
                SND_fn_vPauseSound();
                SuspendThread(*(HANDLE *)GAM_pc_g_hEngineThread_ADDR);
                ReleaseSemaphore(*g_hSemDraw, 1, NULL);
            }
            *p_lResult = 1;
            return 1;

        case 6:
        case 7:
        case 8:
            if (GAM_pc_g_ulPausePending != 0 &&
                *(HANDLE *)GAM_pc_g_hEngineThread_ADDR != INVALID_HANDLE_VALUE)
            {
                ResumeThread(*(HANDLE *)GAM_pc_g_hEngineThread_ADDR);
                SND_fn_vResumeSound();
            }
            *p_lResult = 1;
            return 1;

        default:
            *p_lResult = 0x424D5144;
    }
    return 1;
}

/* 0x00401550 */
void GAM_pc_fn_vSignalEngineAndPostQuit(void)
{

    unsigned long *p_ulPausePending = &GAM_pc_g_ulPausePending;

    DWORD ulWait;

    *p_ulPausePending = 0;
    SND_fn_vReleaseDriverSound();
    GAM_pc_fn_vEngineEventControl(1);
    ulWait = WaitForSingleObject(*g_hEventIsDead, 0);
    if (ulWait == WAIT_OBJECT_0)
    {
        PostMessageA((HWND)(*g_hWnd), WM_QUIT, 0, 0);
    }
}

/* 0x00403290 */
LRESULT CALLBACK GAM_fn_WndProc(HWND hWnd, UINT uMsg, WPARAM wParam,
                                LPARAM lParam)
{
    /* 0x0042E090 */
    extern void __cdecl IPT_fn_vResetInputEntry(void);
    /* 0x0042E150 */
    extern void __cdecl FUN_0042e150(void);

    int     iHandled = 0;
    LRESULT lResult = -1;
    int     iReturnFromHandler = 0;

    if (uMsg < 0x11)
    {
        if (uMsg == 0x10)
        {
            PostMessageA((HWND)*g_hWnd, 2  , 0, 0);
            lResult = 1;
            iHandled = 1;
        }
        else if (uMsg == 2)
        {
            ShowCursor(TRUE);
            GAM_pc_fn_vSignalEngineAndPostQuit();
        }
        else if ((uMsg == 3 || uMsg == 5)
                 && GAM_pc_g_ulPausePending != 0
                 && GAM_pc_g_ulPauseFlag == 0)
        {
            lResult = 0;
            SND_PrintUsedStaticMemory();
            iHandled = 1;
        }
    }
    else if (uMsg < 0x20)
    {
        if (uMsg == 0x1F)
        {
            iReturnFromHandler = GAM_pc_fn_vEnterPauseOnDeactivate(1);
            iHandled = iReturnFromHandler;
        }
        else if (uMsg == 0x1C)
        {
            if (GAM_pc_g_ulNoPauseThisTime != 0)  /* 0x004A6AAC */
            {
                iHandled = 1;
            }
            else if (wParam != 0)
            {
                iReturnFromHandler = GAM_pc_fn_vRestoreEngineOnActivate();
                iHandled = iReturnFromHandler;
            }
            else
            {
                iReturnFromHandler = GAM_pc_fn_vEnterPauseOnDeactivate(1);
                iHandled = iReturnFromHandler;
            }
        }
    }
    else if (uMsg < 0x49)
    {
        if (uMsg == 0x48)
        {
            iReturnFromHandler =
                GAM_pc_fn_vHandleWmPowerBroadcast(wParam, lParam, &lResult);
            iHandled = iReturnFromHandler;
        }
        else if (uMsg == 0x24)
        {
            int iScreenWidth = GetSystemMetrics(0x1F  );

            if (((MINMAXINFO *)lParam)->ptMinTrackSize.x < 0x32)
            {
                ((MINMAXINFO *)lParam)->ptMinTrackSize.x = 0x32;
            }
            ((MINMAXINFO *)lParam)->ptMinTrackSize.y = iScreenWidth + 0x32;
            lResult = 0;
            iHandled = 0;
        }
    }
    else if (uMsg < 0xA1)
    {
        if (uMsg == 0xA0)
        {
            if (GAM_pc_g_ulPausePending != 0
                && GAM_pc_g_ulPauseFlag != 0)
            {
                iHandled = 1;
                lResult = 0;
            }
        }
        else if (uMsg == 0x86)
        {
            iHandled = 0;
        }
    }
    else if (uMsg < 0x105)
    {
        if (uMsg == 0x104)
        {
            if (wParam == 0xD)
            {
                IPT_fn_vResetInputEntry();   /* 0x42e090 */
                FUN_0042e150();
            }
            else if (wParam == 0x12
                     && GAM_pc_g_ulPausePending != 0
                     && GAM_pc_g_ulPauseFlag != 0)
            {
                iHandled = 1;
                lResult = 0;
            }
        }
        else if (uMsg >= 0xA1 && uMsg <= 0xA9
                 && GAM_pc_g_ulPausePending != 0
                 && GAM_pc_g_ulPauseFlag != 0)
        {
            iReturnFromHandler = GAM_pc_fn_vEnterPauseOnDeactivate(1);
            iHandled = iReturnFromHandler;
        }
    }
    else if (uMsg < 0x113)
    {
        if (uMsg == 0x112)
        {
            if (wParam < 0xF011)
            {
                if (wParam != 0xF010 && wParam != 0xF000)
                {

                }
            }
            else if (wParam < 0xF061)
            {
                if (wParam == 0xF060)
                {
                    PostMessageA((HWND)*g_hWnd, 2  , 0, 0);
                    iHandled = 1;
                    lResult = 1;
                }
                else if (wParam == 0xF020)
                {
                    iReturnFromHandler = GAM_pc_fn_vEnterPauseOnDeactivate(1);
                    iHandled = iReturnFromHandler;
                }

            }
            else if (wParam == 0xF100)
            {

                if (GAM_pc_g_ulPausePending != 0
                    && GAM_pc_g_ulPauseFlag != 0)
                {
                    iHandled = 1;
                    lResult = 0;
                }
            }
            else if (wParam == 0xF140 || wParam == 0xF170)
            {

                iHandled = 1;
            }
        }
    }
    else
    {
        if (uMsg == 0x218)
        {
            iReturnFromHandler =
                GAM_pc_fn_vHandleWmPowerBroadcast(wParam, lParam, &lResult);
            iHandled = iReturnFromHandler;
        }
        else if (uMsg == 0x400)    /* 0x00402532 */
        {
            iReturnFromHandler = GAM_pc_fn_vDispatchWmUser(wParam, lParam);
            iHandled = iReturnFromHandler;
        }

    }

    if (iHandled != 0)
    {
        return lResult;
    }
    return DefWindowProcA(hWnd, uMsg, wParam, lParam);
}
#endif /* USE_SDL */

/* 0x4e0eb0 */
long          __cdecl GLI_pc_bInitDisplayModes(void);
long          __cdecl GLI_fn_lChooseDisplayMode(void);
unsigned long __cdecl GLI_fn_lIsFullScreenMode(void);
unsigned long __cdecl GLI_fn_lDisplayModeWidth(void);
unsigned long __cdecl GLI_fn_lDisplayModeHeight(void);
struct GLI_tdstCamera_;
void          GLI_xAdjustCameraToViewport(short hDevice,
                                   short hViewport, struct GLI_tdstCamera_ *p_stCam);
long          __cdecl GLD_bCreateDevice(unsigned long *p_ulDevAttribs,
                                        short *p_hDev);
long          __cdecl GLD_bCreateViewport(short          hDev,
                                          unsigned long *p_ulViewAttribs,
                                          short         *p_hVP);
long          __cdecl GLD_bGiveSpecificAttributesToViewport(
                                          short          hDev_unused,
                                          short          hVpPacked,
                                          void          *p_vAttribs);

int __cdecl fn_bCreateMainDisplayScreen(void)
{
    unsigned long *const p_ulHwndGoneFlag    = &GAM_pc_g_ulHwndGoneFlag;  /* 0x004a6aa8 */

    unsigned long  aDevAttribs[24];
    unsigned long  aVpAttribs[28];
    RECT           rect;

    if (GLI_pc_bInitDisplayModes() == 0)
        exit(-1);
    if (GLI_fn_lChooseDisplayMode() == 0)
        exit(-1);

    /* 0x422080 */
    GAM_pc_fn_vSetDisplaySize(GLI_fn_lDisplayModeWidth(), GLI_fn_lDisplayModeHeight());

    GAM_pc_fn_vSetDisplayFullScreen(GLI_fn_lIsFullScreenMode());

    GetClientRect((HWND)*g_hWnd, &rect);

    aDevAttribs[0xc] = (*p_ulHwndGoneFlag == 0);
    aDevAttribs[7]   = GAM_pc_fn_ulGetDisplayWidth();
    aDevAttribs[8]   = GAM_pc_fn_ulGetDisplayHeight();
    aDevAttribs[0xa] = (unsigned long)*g_hWnd;
    aDevAttribs[0xb] = (unsigned long)*g_hWnd;
    aDevAttribs[9]   = 0x10;
    aDevAttribs[0xd] = 10;

    if (GLD_bCreateDevice(aDevAttribs, p_hDevice) == 0)
        return 0;

    aVpAttribs[0x15] = 1000;
    aVpAttribs[0x13] = 1000;
    aVpAttribs[0x14] = 0;
    aVpAttribs[0x12] = 0;
    aVpAttribs[0xc]  = 100;
    aVpAttribs[0xd]  = 100;
    aVpAttribs[0x16] = 0;
    aVpAttribs[0x17] = 0;

    if (GLD_bCreateViewport((unsigned long)(unsigned short)*p_hDevice, aVpAttribs, p_hViewport) == 0)
        return 0;

    if (GLD_bGiveSpecificAttributesToViewport((unsigned long)(unsigned short)*p_hDevice,
                                              (unsigned short)*p_hViewport,
                                              p_ulSpecificAttribs) == 0)
        return 0;

    if (*p_ulSpecificAttribs != 0)
        GLI_xAdjustCameraToViewport((unsigned long)(unsigned short)*p_hDevice,
                                    (unsigned short)*p_hViewport,
                                    (struct GLI_tdstCamera_ *)*p_ulSpecificAttribs);

    return 1;
}

/* 0x00420a80 */
extern long __cdecl GLD_bGetViewportAttributes(short hNotUsed, short hVP, void *pstViewAttrib);

#define GAM_pc_g_p2DCamera  (GAM_g_stEngineStructure.p_stFixCamera)

/* 0x004052c0 */
void __cdecl fn_vCreateFixCamera(void)
{
    POS_tdstCompletePosition stCamMatrix;
    POS_tdstCompletePosition stRotMatrix;
    MTH3D_tdstVector         stTrans;

    GLI_xCreateCamera(&GAM_pc_g_p2DCamera, 0x67);
    GLI_xSetCameraAspectAndRatio(GAM_pc_g_p2DCamera, 1.5f, 0.75f);
    GLI_xSetCameraNearFar(GAM_pc_g_p2DCamera, 0.1f, 1000.0f);
    GLI_xGetCameraMatrix(GAM_pc_g_p2DCamera, (long *)&stCamMatrix);

    MTH3D_M_vSetVectorElements(&stTrans, 0.0f, 0.0f, 5.0f);
    POS_fn_vSetTranslationVector(&stCamMatrix, &stTrans);
    POS_fn_vRotateX(&stCamMatrix, 1.5707964f);

    MTH3D_M_vSetVectorElements(&stRotMatrix.stTranslationVector, 0.0f, 0.0f, 0.0f);
    MTH3D_M_vSetVectorElements(&stRotMatrix.stRotationMatrix.stCol_0, 1.0f, 0.0f, 0.0f);
    MTH3D_M_vSetVectorElements(&stRotMatrix.stRotationMatrix.stCol_1, 0.0f, 1.0f, 0.0f);
    MTH3D_M_vSetVectorElements(&stRotMatrix.stRotationMatrix.stCol_2, 0.0f, 0.0f, 1.0f);
    MTH3D_M_vSetVectorElements(&stRotMatrix.stTransformMatrix.stCol_0, 1.0f, 0.0f, 0.0f);
    MTH3D_M_vSetVectorElements(&stRotMatrix.stTransformMatrix.stCol_1, 0.0f, 1.0f, 0.0f);
    MTH3D_M_vSetVectorElements(&stRotMatrix.stTransformMatrix.stCol_2, 0.0f, 0.0f, 1.0f);
    stRotMatrix.ulType = POS_C_ulIdentityMatrixFlag;
    POS_fn_vRotateZ(&stRotMatrix, 3.1415927f);

    POS_fn_vMulMatrixMatrix(&stCamMatrix, &stRotMatrix, &stCamMatrix);
    POS_fn_vNormalizeMatrix(&stCamMatrix);
    GLI_xSetCameraMatrix(GAM_pc_g_p2DCamera, (long *)&stCamMatrix);
}

#include "GLD/GLD_Pub.h"
#define GAM_M_stFixViewportAttr (*(struct GLD_tdstViewportAttributes_ *)GAM_g_stEngineStructure.stFixViewportAttr)

void __cdecl fn_vCreateFixViewPort(short _hDevice)
{

	GAM_M_stFixViewportAttr.dwWidthInPercent	= 100;
	GAM_M_stFixViewportAttr.dwHeightInPercent	= 100;
	GAM_M_stFixViewportAttr.dwClipTopInPerMille = 0;
	GAM_M_stFixViewportAttr.dwClipBottomInPerMille = 1000;
	GAM_M_stFixViewportAttr.dwClipLeftInPerMille = 0;
	GAM_M_stFixViewportAttr.dwClipRightInPerMille = 1000;
	GAM_M_stFixViewportAttr.iOffsetPosX = 0;
	GAM_M_stFixViewportAttr.iOffsetPosY = 0;

	if (!GLD_bCreateViewport(_hDevice,(unsigned long *)&GAM_M_stFixViewportAttr,&GAM_g_stEngineStructure.hGLDFixViewport)) {
		ERM_fn_vClearLastError(0);
		ERM_fn_vUpdateLastError(0x30, g_ucGameModuleId, 0, 0x28, -1, 0xff, 0xff, NULL);
	}
	if (!GLD_bGiveSpecificAttributesToViewport(_hDevice,GAM_g_stEngineStructure.hGLDFixViewport,&GAM_g_stEngineStructure.stFixAttributes3D)) {
		ERM_fn_vClearLastError(0);
		ERM_fn_vUpdateLastError(0x30, g_ucGameModuleId, 0, 0x28, -1, 0xff, 0xff, NULL);
	}

	GLD_bGetViewportAttributes(_hDevice,GAM_g_stEngineStructure.hGLDFixViewport,&GAM_M_stFixViewportAttr);

	if(GAM_g_stEngineStructure.p_stFixCamera==NULL)
	{
		fn_vCreateFixCamera();
	}

	GLI_xAdjustCameraToViewport(_hDevice,GAM_g_stEngineStructure.hGLDFixViewport,GAM_g_stEngineStructure.p_stFixCamera);
	GLI_xSetViewportCamera(_hDevice,GAM_g_stEngineStructure.hGLDFixViewport,(long)GAM_g_stEngineStructure.p_stFixCamera);
}

/* 0x00405480 */
void fn_vCreateGameViewPortCamera(void)
{
	GLI_xCreateCamera ((GLI_tdstCamera **)&GAM_g_stEngineStructure.p_stGameViewportCamera,GLI_C_lPersCamWithoutDistorsion);
	GLI_xSetCameraAspectAndRatio (GAM_g_stEngineStructure.p_stGameViewportCamera, MTH_M_xConstFloatToReal(1.5f), MTH_M_xConstFloatToReal(0.75f));
	GLI_xSetViewportCamera( GAM_g_stEngineStructure.hGLDDevice,GAM_g_stEngineStructure.hGLDViewport,(long)GAM_g_stEngineStructure.p_stGameViewportCamera);
}

void __cdecl GAM_fn_vCreateGameDeviceAndViewPorts(void)
{
    GAM_g_stEngineStructure.p_stFixCamera=NULL;
    fn_vCreateFixViewPort(GAM_g_stEngineStructure.hGLDDevice);
    fn_vCreateGameViewPortCamera();
}

#ifndef USE_SDL
/* 0x00402150 */
int GAM_fn_bCreateMainWindow(HINSTANCE hInstance, int iShowCmd)
{
    WNDCLASSA wc;
    ATOM      atomClass;
    int       iCxFixedFrame, iCyFixedFrame, iCyCaption;
    HWND      hWnd;

    wc.style         = 0x3003;
    wc.lpfnWndProc   = GAM_fn_WndProc;
    wc.cbClsExtra    = 0;
    wc.cbWndExtra    = 0;
    wc.hInstance     = hInstance;
#ifdef RAYMAN2_WINDOW_ICON
    /* the exe's icon group 1 (taskbar, title bar) instead of the system one; DEVIATIONS 12 */
    wc.hIcon         = LoadIconA(hInstance, (LPCSTR)1);
#else
    wc.hIcon         = LoadIconA(NULL, (LPCSTR)0x7F00);
#endif /* RAYMAN2_WINDOW_ICON */
    wc.hCursor       = LoadCursorA(NULL, (LPCSTR)0x7F00);
    wc.hbrBackground = (HBRUSH)6;
    wc.lpszMenuName  = NULL;
    wc.lpszClassName = GAM_PC_SZ_WNDCLASS;  /* 0x0049F0A0 */

    atomClass = RegisterClassA(&wc);
    if (atomClass == 0)
    {
        return 0;
    }

    iCxFixedFrame = GetSystemMetrics(0x20  );
    iCyFixedFrame = GetSystemMetrics(0x21  );
    iCyCaption    = GetSystemMetrics(4     );

    hWnd = CreateWindowExA(
        0,
        GAM_PC_SZ_WNDCLASS,  /* 0x0049F0A0 */
        GAM_PC_SZ_WNDCLASS,
        0x80CF0000UL,
        1, 1,
        iCxFixedFrame * 2 + 0x140,
        iCyCaption + 200 + iCyFixedFrame * 2,
        NULL, NULL, hInstance, NULL);
    if (hWnd == NULL)
    {
        return 0;
    }

    *g_hWnd = (HANDLE)hWnd;
    SetWindowTextA(hWnd, GAM_PC_SZ_WNDTITLE);  /* 0x0049F140 */
    {
        int iCmdShow = (GAM_pc_g_ulHwndGoneFlag == 0) ? iShowCmd : 6  ;
        ShowWindow(hWnd, iCmdShow);
    }
    UpdateWindow(hWnd);
    Erm_fn_vSetWndHdl4Request(hWnd);

    return fn_bCreateMainDisplayScreen();
}

extern int   (__cdecl *_imp__access)(const char *pszPath, int iMode);
extern FILE *(__cdecl *_imp__fopen)(const char *pszPath, const char *pszMode);
extern int   (__cdecl *_imp__fprintf)(FILE *pFile, const char *pszFmt, ...);
extern int   (__cdecl *_imp__fclose)(FILE *pFile);

unsigned long GAM_pc_g_ulMainThreadId;   /* 0x004A6A94 */

int __stdcall GAM_pc_fn_WinMain(HINSTANCE hInstance,
                      HINSTANCE hPrevInstance,
                      char     *p_szCmdLine,
                      int       iShowCmd)
{

    char     szIniPath[260];
    FILE    *pFile;
    HANDLE   hMutex;
    BOOL     bScreensaverWasActive;
    MSG      msg;
    int      iGetMsg;

    /* 0x518D83 */
    __asm {
        _emit 0x90
        _emit 0x90
        _emit 0x90
        _emit 0x90
        _emit 0x90
        _emit 0x90
        _emit 0x90
        _emit 0x90
        _emit 0x90
        _emit 0x90
        _emit 0x90
        _emit 0x90
        _emit 0x90
        _emit 0x90
        _emit 0x90
        _emit 0x90
        _emit 0x90
        _emit 0x90
        _emit 0x90
        _emit 0x90
        _emit 0x90
        _emit 0x90
        _emit 0x90
        _emit 0x90
        _emit 0x90
        _emit 0x90
        _emit 0x90
        _emit 0x90
        _emit 0x90
        _emit 0x90
        _emit 0x90
        _emit 0x90
        _emit 0x90
        _emit 0x90
        _emit 0x90
        _emit 0x90
        _emit 0x90
        _emit 0x90
        _emit 0x90
        _emit 0x90
        _emit 0x90
        _emit 0x90
        _emit 0x90
        _emit 0x90
        _emit 0x90
        _emit 0x90
        _emit 0x90
        _emit 0x90
        _emit 0x90
        _emit 0x90
        _emit 0x90
        _emit 0x90
        _emit 0x90
        _emit 0x90
        _emit 0x90
        _emit 0x90
        _emit 0x90
        _emit 0x90
        _emit 0x90
        _emit 0x90
    }
#ifndef RAYMAN2_LOCAL_UBIINI
    GetWindowsDirectoryA(szIniPath, 0x104);
    strcat(szIniPath, "\\UbiSoft");
    if ((*_imp__access)(szIniPath, 0) != 0)
    {
        CreateDirectoryA(szIniPath, NULL);
    }
#else
    strcpy(szIniPath, ".");   /* ubi.ini in the working directory (the game folder) */
#endif /* RAYMAN2_LOCAL_UBIINI */
    strcat(szIniPath, "\\ubi.ini");
    if ((*_imp__access)(szIniPath, 0) != 0)
    {
        pFile = (*_imp__fopen)(szIniPath, "wt");
        if (pFile != NULL)
        {
            (*_imp__fprintf)(pFile, "[Rayman2]\r\nChoose = 1\r\n");
            (*_imp__fclose)(pFile);
        }
    }

    if (hPrevInstance != NULL)
    {
        return -1;
    }

    hMutex = CreateMutexA(NULL, TRUE, "RAYMAN2EXE");
    (void)hMutex;
    if (GetLastError() == ERROR_ALREADY_EXISTS)
    {
        return -1;
    }

    if (GAM_pc_fn_bInitDisplayAndSync() == 0)
    {
        return -1;
    }

    /* 0x401EB7 */
    *g_hInstance = hInstance;
    GAM_pc_g_ulMainThreadId = GetCurrentThreadId();

    /* 0x004E0DA0 */
    strcpy(GAM_g_szCmdLine, p_szCmdLine);

    SetErrorMode(SEM_FAILCRITICALERRORS);

    MMG_fn_vInit(1);

    fn_vInitEngineWhenInitApplication();

    if (GAM_fn_bCreateMainWindow(hInstance, iShowCmd) != 0)
    {

        FUN_00401050();

        SystemParametersInfoA(SPI_GETSCREENSAVEACTIVE, 0,
                              &bScreensaverWasActive, 0);
        SystemParametersInfoA(SPI_SETSCREENSAVEACTIVE, 0,
                              (PVOID)0, 0);

        GAM_pc_fn_vCreateEngineThread();

        while (GAM_pc_fn_ulGetQuitFlag() == 0)
        {
        }

        ShowCursor(FALSE);

        for (iGetMsg = GetMessageA(&msg, (HWND)0, 0, 0);
             iGetMsg != 0;
             iGetMsg = GetMessageA(&msg, (HWND)0, 0, 0))
        {
            if (FUN_00402580(&msg) == 0)
            {
                TranslateMessage(&msg);
                DispatchMessageA(&msg);
            }
        }

        ShowCursor(TRUE);

        GAM_pc_fn_vKillEngineThreadAfterExit();
        SystemParametersInfoA(SPI_SETSCREENSAVEACTIVE,
                              bScreensaverWasActive, (PVOID)0, 0);
        FUN_00401b20();
        GAM_pc_fn_vFreeMmgTableAndHelperDll();
        return 0;
    }

    return -1;
}
#endif /* USE_SDL */
