/*
 =======================================================================================
  Name        : sdl_ino.c
  Description : Input devices through SDL (USE_SDL)

  The SDL counterpart of the DirectInput parts of INO/INO.c (fenced #ifndef USE_SDL
  there; INO/INO_sdl.h sends the calls of the shared device layer here). The device
  states are filled as the DirectInput readers fill them:

  Keyboard  SDL's keyboard state, by key code (DIK_, see INO_sdl.h) through a table to
            SDL scancodes. The keys of the active key list are those of the PC setup.
  Mouse     motion since the last read, wheel (in WHEEL_DELTA units, as DirectInput
            reports it) and four buttons (the four of DirectInput's mouse format, in
            its order: left, right, middle, X1).
  Joystick  one per SDL gamepad present at init (DirectInput enumerates its joysticks
            once at init as well). SDL's mapping table gives every gamepad one layout,
            set out as DirectInput sets out an XInput gamepad: buttons A, B, X, Y,
            left and right shoulder, back, start, left and right stick; axes X/Y left
            stick, Rx/Ry right stick, range 0..65535 (DirectInput's default). The
            triggers share the Z axis there; which trigger moves it which way is
            unchecked (docs/PLAN.md), so Z stays centred and the triggers are unused.
            The d-pad is DirectInput's point of view, which the PC reader does not
            read either.

  As with DirectInput in its foreground mode, devices give no input while the window
  does not have the keyboard focus.
 =======================================================================================
 */
#ifdef USE_SDL

#include <SDL3/SDL.h>

#include "INO/INO.h"
#include "INO/INO_globals.h"
#include "INO/Joystick.h"
#include "INO/INO_sdl.h"

short __cdecl INO_pc_ReadMouse(INO_tdstDevice *pDevice);
short __cdecl INO_pc_ReadKeyboard(INO_tdstDevice *pDevice);
short __cdecl INO_pc_ReadJoystick(INO_tdstDevice *pDevice);
float __cdecl INO_pc_xGetMouseAxis(INO_tdstDevice *pDevice);

extern unsigned short INO_pc_g_a_wActiveKey_storage[];    /* INO/INO.c */

#define INO_sdl_C_wMaxActiveKey   0x80
#define INO_sdl_C_lWheelDelta     120
#define INO_sdl_C_ucNbMouseButton 4
#define INO_sdl_C_ucNbPadButton   10
#define INO_sdl_C_lAxisMax        65535

/* Key code (DIK_) -> SDL scancode; the PC set-1 scan codes, as SDL's own Windows table,
   except 0x45 NumLock and 0xC5 Pause, which DirectInput tells apart that way */
static const unsigned short INO_sdl_a_uwKeyToScancode[256] =
{
    [0x01] = SDL_SCANCODE_ESCAPE,       [0x02] = SDL_SCANCODE_1,
    [0x03] = SDL_SCANCODE_2,            [0x04] = SDL_SCANCODE_3,
    [0x05] = SDL_SCANCODE_4,            [0x06] = SDL_SCANCODE_5,
    [0x07] = SDL_SCANCODE_6,            [0x08] = SDL_SCANCODE_7,
    [0x09] = SDL_SCANCODE_8,            [0x0A] = SDL_SCANCODE_9,
    [0x0B] = SDL_SCANCODE_0,            [0x0C] = SDL_SCANCODE_MINUS,
    [0x0D] = SDL_SCANCODE_EQUALS,       [0x0E] = SDL_SCANCODE_BACKSPACE,
    [0x0F] = SDL_SCANCODE_TAB,          [0x10] = SDL_SCANCODE_Q,
    [0x11] = SDL_SCANCODE_W,            [0x12] = SDL_SCANCODE_E,
    [0x13] = SDL_SCANCODE_R,            [0x14] = SDL_SCANCODE_T,
    [0x15] = SDL_SCANCODE_Y,            [0x16] = SDL_SCANCODE_U,
    [0x17] = SDL_SCANCODE_I,            [0x18] = SDL_SCANCODE_O,
    [0x19] = SDL_SCANCODE_P,            [0x1A] = SDL_SCANCODE_LEFTBRACKET,
    [0x1B] = SDL_SCANCODE_RIGHTBRACKET, [0x1C] = SDL_SCANCODE_RETURN,
    [0x1D] = SDL_SCANCODE_LCTRL,        [0x1E] = SDL_SCANCODE_A,
    [0x1F] = SDL_SCANCODE_S,            [0x20] = SDL_SCANCODE_D,
    [0x21] = SDL_SCANCODE_F,            [0x22] = SDL_SCANCODE_G,
    [0x23] = SDL_SCANCODE_H,            [0x24] = SDL_SCANCODE_J,
    [0x25] = SDL_SCANCODE_K,            [0x26] = SDL_SCANCODE_L,
    [0x27] = SDL_SCANCODE_SEMICOLON,    [0x28] = SDL_SCANCODE_APOSTROPHE,
    [0x29] = SDL_SCANCODE_GRAVE,        [0x2A] = SDL_SCANCODE_LSHIFT,
    [0x2B] = SDL_SCANCODE_BACKSLASH,    [0x2C] = SDL_SCANCODE_Z,
    [0x2D] = SDL_SCANCODE_X,            [0x2E] = SDL_SCANCODE_C,
    [0x2F] = SDL_SCANCODE_V,            [0x30] = SDL_SCANCODE_B,
    [0x31] = SDL_SCANCODE_N,            [0x32] = SDL_SCANCODE_M,
    [0x33] = SDL_SCANCODE_COMMA,        [0x34] = SDL_SCANCODE_PERIOD,
    [0x35] = SDL_SCANCODE_SLASH,        [0x36] = SDL_SCANCODE_RSHIFT,
    [0x37] = SDL_SCANCODE_KP_MULTIPLY,  [0x38] = SDL_SCANCODE_LALT,
    [0x39] = SDL_SCANCODE_SPACE,        [0x3A] = SDL_SCANCODE_CAPSLOCK,
    [0x3B] = SDL_SCANCODE_F1,           [0x3C] = SDL_SCANCODE_F2,
    [0x3D] = SDL_SCANCODE_F3,           [0x3E] = SDL_SCANCODE_F4,
    [0x3F] = SDL_SCANCODE_F5,           [0x40] = SDL_SCANCODE_F6,
    [0x41] = SDL_SCANCODE_F7,           [0x42] = SDL_SCANCODE_F8,
    [0x43] = SDL_SCANCODE_F9,           [0x44] = SDL_SCANCODE_F10,
    [0x45] = SDL_SCANCODE_NUMLOCKCLEAR, [0x46] = SDL_SCANCODE_SCROLLLOCK,
    [0x47] = SDL_SCANCODE_KP_7,         [0x48] = SDL_SCANCODE_KP_8,
    [0x49] = SDL_SCANCODE_KP_9,         [0x4A] = SDL_SCANCODE_KP_MINUS,
    [0x4B] = SDL_SCANCODE_KP_4,         [0x4C] = SDL_SCANCODE_KP_5,
    [0x4D] = SDL_SCANCODE_KP_6,         [0x4E] = SDL_SCANCODE_KP_PLUS,
    [0x4F] = SDL_SCANCODE_KP_1,         [0x50] = SDL_SCANCODE_KP_2,
    [0x51] = SDL_SCANCODE_KP_3,         [0x52] = SDL_SCANCODE_KP_0,
    [0x53] = SDL_SCANCODE_KP_PERIOD,    [0x56] = SDL_SCANCODE_NONUSBACKSLASH,
    [0x57] = SDL_SCANCODE_F11,          [0x58] = SDL_SCANCODE_F12,
    [0x9C] = SDL_SCANCODE_KP_ENTER,     [0x9D] = SDL_SCANCODE_RCTRL,
    [0xB5] = SDL_SCANCODE_KP_DIVIDE,    [0xB7] = SDL_SCANCODE_PRINTSCREEN,
    [0xB8] = SDL_SCANCODE_RALT,         [0xC5] = SDL_SCANCODE_PAUSE,
    [0xC7] = SDL_SCANCODE_HOME,         [0xC8] = SDL_SCANCODE_UP,
    [0xC9] = SDL_SCANCODE_PAGEUP,       [0xCB] = SDL_SCANCODE_LEFT,
    [0xCD] = SDL_SCANCODE_RIGHT,        [0xCF] = SDL_SCANCODE_END,
    [0xD0] = SDL_SCANCODE_DOWN,         [0xD1] = SDL_SCANCODE_PAGEDOWN,
    [0xD2] = SDL_SCANCODE_INSERT,       [0xD3] = SDL_SCANCODE_DELETE,
    [0xDB] = SDL_SCANCODE_LGUI,         [0xDC] = SDL_SCANCODE_RGUI,
    [0xDD] = SDL_SCANCODE_APPLICATION,
};

/* Gamepad buttons in DirectInput's order for an XInput gamepad */
static const SDL_GamepadButton INO_sdl_a_eGamepadButton[INO_sdl_C_ucNbPadButton] =
{
    SDL_GAMEPAD_BUTTON_SOUTH,      SDL_GAMEPAD_BUTTON_EAST,
    SDL_GAMEPAD_BUTTON_WEST,       SDL_GAMEPAD_BUTTON_NORTH,
    SDL_GAMEPAD_BUTTON_LEFT_SHOULDER, SDL_GAMEPAD_BUTTON_RIGHT_SHOULDER,
    SDL_GAMEPAD_BUTTON_BACK,       SDL_GAMEPAD_BUTTON_START,
    SDL_GAMEPAD_BUTTON_LEFT_STICK, SDL_GAMEPAD_BUTTON_RIGHT_STICK,
};

typedef struct INO_sdl_tdstSlot_
{
    INO_tdstDevice *p_stDevice;
    SDL_Gamepad    *p_stGamepad;
} INO_sdl_tdstSlot;

static INO_sdl_tdstSlot  INO_sdl_g_a_stSlot[INO_C_wNbMaxDevice];
static SDL_JoystickID    INO_sdl_g_a_xGamepadId[INO_C_wNbMaxDevice];
static short             INO_sdl_g_wNbGamepad;
static short             INO_sdl_g_bInited;
static float             INO_sdl_g_xWheel;

/*
 ----------------------------------------------------------------------------------------
  Description   : Wheel events, added up until the next mouse read
 ----------------------------------------------------------------------------------------
 */
static bool SDLCALL INO_sdl_bWatchEvent(void *p_vUserData, SDL_Event *p_stEvent)
{
    (void)p_vUserData;
    if ( p_stEvent->type == SDL_EVENT_MOUSE_WHEEL )
    {
        if ( p_stEvent->wheel.direction == SDL_MOUSEWHEEL_FLIPPED )
        {
            INO_sdl_g_xWheel -= p_stEvent->wheel.y;
        }
        else
        {
            INO_sdl_g_xWheel += p_stEvent->wheel.y;
        }
    }
    return true;
}

/*
 ----------------------------------------------------------------------------------------
  Description   : Whether the devices may give input (DirectInput's foreground mode)
 ----------------------------------------------------------------------------------------
 */
static int INO_sdl_bHasFocus(void)
{
    return SDL_GetKeyboardFocus() != NULL;
}

/*
 ----------------------------------------------------------------------------------------
  Description   : The slot of a device, or -1
 ----------------------------------------------------------------------------------------
 */
static short INO_sdl_wFindSlot(INO_tdstDevice *pDevice)
{
    short wSlot;

    for ( wSlot = 0; wSlot < INO_C_wNbMaxDevice; wSlot++ )
    {
        if ( INO_sdl_g_a_stSlot[wSlot].p_stDevice == pDevice )
        {
            return wSlot;
        }
    }
    return -1;
}

/*
 ----------------------------------------------------------------------------------------
  Description   : Gamepad subsystem, gamepads present now, wheel events
 ----------------------------------------------------------------------------------------
 */
short __cdecl INO_sdl_bInit(HINSTANCE hInstance, HWND hWindow)
{
    SDL_JoystickID *p_xId;
    int             iCount;
    int             i;

    (void)hInstance; (void)hWindow;

    if ( INO_sdl_g_bInited )
    {
        return 1;
    }

    SDL_memset(INO_sdl_g_a_stSlot, 0, sizeof(INO_sdl_g_a_stSlot));
    INO_sdl_g_wNbGamepad = 0;
    INO_sdl_g_xWheel     = 0.0f;

    /* Without gamepads the keyboard and the mouse still work */
    if ( SDL_InitSubSystem(SDL_INIT_GAMEPAD) )
    {
        p_xId = SDL_GetGamepads(&iCount);
        if ( p_xId != NULL )
        {
            /* The keyboard and the mouse take a slot each */
            for ( i = 0; i < iCount && INO_sdl_g_wNbGamepad < INO_C_wNbMaxDevice - 2; i++ )
            {
                INO_sdl_g_a_xGamepadId[INO_sdl_g_wNbGamepad++] = p_xId[i];
            }
            SDL_free(p_xId);
        }
    }

    SDL_AddEventWatch(INO_sdl_bWatchEvent, NULL);
    INO_sdl_g_bInited = 1;
    return 1;
}

/*
 ----------------------------------------------------------------------------------------
  Description   : Everything INO_sdl_bInit took, given back
 ----------------------------------------------------------------------------------------
 */
void __cdecl INO_sdl_fn_vRelease(void)
{
    short wSlot;

    if ( !INO_sdl_g_bInited )
    {
        return;
    }
    for ( wSlot = 0; wSlot < INO_C_wNbMaxDevice; wSlot++ )
    {
        if ( INO_sdl_g_a_stSlot[wSlot].p_stDevice != NULL )
        {
            INO_sdl_wReleaseDevice(INO_sdl_g_a_stSlot[wSlot].p_stDevice);
        }
    }
    SDL_RemoveEventWatch(INO_sdl_bWatchEvent, NULL);
    if ( SDL_WasInit(SDL_INIT_GAMEPAD) )
    {
        SDL_QuitSubSystem(SDL_INIT_GAMEPAD);
    }
    INO_sdl_g_wNbGamepad = 0;
    INO_sdl_g_bInited    = 0;
}

/*
 ----------------------------------------------------------------------------------------
  Description   : Number of devices of a type: one keyboard, one mouse (DirectInput's
                  system keyboard and mouse), the gamepads found at init
 ----------------------------------------------------------------------------------------
 */
short __cdecl INO_fn_wGetNbDevices(unsigned short uwType)
{
    if ( !INO_sdl_g_bInited )
    {
        return 0;
    }
    switch ( uwType & 0xFFFF )
    {
        case INO_C_uwKeyboard:
        case INO_C_uwMouse:
            return 1;
        case INO_C_uwJoystick:
            return INO_sdl_g_wNbGamepad;
        default:
            return 0;
    }
}

/*
 ----------------------------------------------------------------------------------------
  Description   : Keyboard: active key list and layout as in the PC setup
                  (INO_pc_wSetupKeyboardDI)
 ----------------------------------------------------------------------------------------
 */
static short INO_sdl_wSetupKeyboard(INO_tdstDevice *pDevice)
{
    short          wKey;
    unsigned int   uiIndex;
    unsigned char  ucProbe;

    pDevice->p_stState->ucStatus |= INO_C_ucConnected;

    for ( uiIndex = 0; uiIndex < INO_sdl_C_wMaxActiveKey; uiIndex++ )
    {
        INO_pc_g_a_wActiveKey_storage[uiIndex] = 0;
    }
    INO_pc_g_uwActiveKeyCount = 0;

    for ( wKey = 1; wKey < 0x58; wKey++ )
    {
        INO_fn_wAddActiveKey(wKey);
    }
    INO_fn_wAddActiveKey(0x69);
    INO_fn_wAddActiveKey(0x9C);
    INO_fn_wAddActiveKey(0x9D);
    INO_fn_wAddActiveKey(0xB5);
    INO_fn_wAddActiveKey(0xB7);
    INO_fn_wAddActiveKey(0xB8);
    INO_fn_wAddActiveKey(0xC7);
    INO_fn_wAddActiveKey(0xC5);
    INO_fn_wAddActiveKey(0xC8);
    INO_fn_wAddActiveKey(0xC9);
    INO_fn_wAddActiveKey(0xCB);
    INO_fn_wAddActiveKey(0xCD);
    INO_fn_wAddActiveKey(0xCF);
    INO_fn_wAddActiveKey(0xD0);
    INO_fn_wAddActiveKey(0xD1);
    INO_fn_wAddActiveKey(0xD2);
    INO_fn_wAddActiveKey(0xD3);
    INO_fn_wAddActiveKey(0xDB);
    INO_fn_wAddActiveKey(0xDC);
    INO_fn_wAddActiveKey(0xDD);

    /* AZERTY when the key at Q's place gives an 'a' */
    ucProbe = (unsigned char)INO_sdl_ulTranslateKeyToAscii(0x10, pDevice);
    ((INO_tdstKeyboardCaps *)pDevice->p_stCaps)->wIsAzerty =
        (unsigned short)(ucProbe == 0x61 || ucProbe == 0x41);
    return 1;
}

/*
 ----------------------------------------------------------------------------------------
  Description   : Mouse: no axis ranges (a relative device), GLD link cleared as in the
                  PC setup
 ----------------------------------------------------------------------------------------
 */
static short INO_sdl_wSetupMouse(INO_tdstDevice *pDevice)
{
    pDevice->p_stState->ucStatus |= INO_C_ucConnected;
    pDevice->p_stCaps->ucNbButtons = INO_sdl_C_ucNbMouseButton;
    pDevice->p_stState->ucStatus &= (unsigned char)~INO_C_ucGLDLink;
    return 1;
}

/*
 ----------------------------------------------------------------------------------------
  Description   : Gamepad: opened, caps as for DirectInput's view of an XInput gamepad
 ----------------------------------------------------------------------------------------
 */
static short INO_sdl_wSetupJoystick(INO_tdstDevice *pDevice, short wSlot)
{
    INO_pc_tdstAxisDeviceCaps *pCaps;
    short                      wUsed;
    short                      wAxis;
    short                      wPad;

    /* The next gamepad found at init that no slot has yet */
    for ( wPad = 0; wPad < INO_sdl_g_wNbGamepad; wPad++ )
    {
        for ( wUsed = 0; wUsed < INO_C_wNbMaxDevice; wUsed++ )
        {
            if ( INO_sdl_g_a_stSlot[wUsed].p_stGamepad != NULL
                 && SDL_GetGamepadID(INO_sdl_g_a_stSlot[wUsed].p_stGamepad)
                    == INO_sdl_g_a_xGamepadId[wPad] )
            {
                break;
            }
        }
        if ( wUsed == INO_C_wNbMaxDevice )
        {
            break;
        }
    }
    if ( wPad == INO_sdl_g_wNbGamepad )
    {
        return 0;
    }

    INO_sdl_g_a_stSlot[wSlot].p_stGamepad = SDL_OpenGamepad(INO_sdl_g_a_xGamepadId[wPad]);
    if ( INO_sdl_g_a_stSlot[wSlot].p_stGamepad == NULL )
    {
        return 0;
    }
    if ( SDL_GamepadConnected(INO_sdl_g_a_stSlot[wSlot].p_stGamepad) )
    {
        pDevice->p_stState->ucStatus |= INO_C_ucConnected;
    }

    pCaps = (INO_pc_tdstAxisDeviceCaps *)pDevice->p_stCaps;
    pCaps->stDev.ucNbButtons = INO_sdl_C_ucNbPadButton;
    pCaps->ucNbAxes = 5;
    pCaps->ucNbPOVs = 1;
    /* X, Y, Z, Rx, Ry; Rz absent. Range 0..65535: centre, half range, quarter range
       as INO_pc_wSetupJoystickDI derives them */
    for ( wAxis = 0; wAxis < 5; wAxis++ )
    {
        pCaps->a_stAxis[wAxis].m_lCenter    = INO_sdl_C_lAxisMax >> 1;
        pCaps->a_stAxis[wAxis].m_lRange     = INO_sdl_C_lAxisMax >> 1;
        pCaps->a_stAxis[wAxis].m_lThreshold = INO_sdl_C_lAxisMax >> 2;
        pCaps->stDev.ulCapsFlags |= 1UL << wAxis;
    }
    return 1;
}

/*
 ----------------------------------------------------------------------------------------
  Description   : A device to a free slot: read function, caps, status
 ----------------------------------------------------------------------------------------
 */
short __cdecl INO_sdl_wAssignDevice(INO_tdstDevice *pDevice)
{
    short wSlot;
    short wSetup;

    wSlot = INO_sdl_wFindSlot(NULL);
    if ( wSlot < 0 )
    {
        return -1;
    }
    INO_sdl_g_a_stSlot[wSlot].p_stDevice = pDevice;

    switch ( pDevice->uwType & 0xFF )
    {
        case INO_C_uwMouse:
            pDevice->pfnRead = INO_pc_ReadMouse;
            wSetup = INO_sdl_wSetupMouse(pDevice);
            break;
        case INO_C_uwKeyboard:
            pDevice->pfnRead = INO_pc_ReadKeyboard;
            wSetup = INO_sdl_wSetupKeyboard(pDevice);
            break;
        case INO_C_uwJoystick:
            pDevice->pfnRead = INO_pc_ReadJoystick;
            wSetup = INO_sdl_wSetupJoystick(pDevice, wSlot);
            break;
        default:
            wSetup = 0;
            break;
    }

    if ( wSetup == 0 )
    {
        INO_sdl_g_a_stSlot[wSlot].p_stDevice = NULL;
        return -1;
    }

    if ( INO_sdl_bHasFocus() )
    {
        pDevice->p_stState->ucStatus |= INO_C_ucAcquired;
    }
    else
    {
        pDevice->p_stState->ucStatus &= (unsigned char)~INO_C_ucAcquired;
    }
    return wSlot;
}

/*
 ----------------------------------------------------------------------------------------
  Description   : The slot of a device freed
 ----------------------------------------------------------------------------------------
 */
short __cdecl INO_sdl_wReleaseDevice(INO_tdstDevice *pDevice)
{
    short wSlot = INO_sdl_wFindSlot(pDevice);

    if ( wSlot < 0 )
    {
        return 0;
    }
    if ( INO_sdl_g_a_stSlot[wSlot].p_stGamepad != NULL )
    {
        SDL_CloseGamepad(INO_sdl_g_a_stSlot[wSlot].p_stGamepad);
    }
    INO_sdl_g_a_stSlot[wSlot].p_stDevice  = NULL;
    INO_sdl_g_a_stSlot[wSlot].p_stGamepad = NULL;
    return 1;
}

/*
 ----------------------------------------------------------------------------------------
  Description   : Connected status: keyboard and mouse always, a gamepad while SDL has it
 ----------------------------------------------------------------------------------------
 */
short __cdecl INO_sdl_wRefreshDeviceCaps(INO_tdstDevice *pDevice)
{
    short             wSlot = INO_sdl_wFindSlot(pDevice);
    INO_tdstDevState *pState = pDevice->p_stState;

    if ( wSlot < 0 )
    {
        return 0;
    }
    if ( INO_sdl_g_a_stSlot[wSlot].p_stGamepad != NULL
         && !SDL_GamepadConnected(INO_sdl_g_a_stSlot[wSlot].p_stGamepad) )
    {
        pState->ucStatus &= (unsigned char)~INO_C_ucConnected;
        return 0;
    }
    pState->ucStatus |= INO_C_ucConnected;
    return 1;
}

/*
 ----------------------------------------------------------------------------------------
  Description   : Character of a key, no modifier applied (the PC reader passes an
                  empty modifier state as well). Keypad digits and decimal with NumLock
                  on, keypad operators always; other keys from SDL's keymap.
 ----------------------------------------------------------------------------------------
 */
unsigned int __cdecl INO_sdl_ulTranslateKeyToAscii(unsigned int uiKey, INO_tdstDevice *pDevice)
{
    SDL_Scancode eScancode;
    SDL_Keycode  xKey;
    int          bNumLock;

    (void)pDevice;

    if ( uiKey >= 256 || INO_sdl_a_uwKeyToScancode[uiKey] == 0 )
    {
        return 0;
    }
    eScancode = (SDL_Scancode)INO_sdl_a_uwKeyToScancode[uiKey];
    bNumLock  = (SDL_GetModState() & SDL_KMOD_NUM) != 0;

    switch ( eScancode )
    {
        case SDL_SCANCODE_KP_DIVIDE:   return '/';
        case SDL_SCANCODE_KP_MULTIPLY: return '*';
        case SDL_SCANCODE_KP_MINUS:    return '-';
        case SDL_SCANCODE_KP_PLUS:     return '+';
        case SDL_SCANCODE_KP_PERIOD:   return bNumLock ? '.' : 0;
        case SDL_SCANCODE_KP_0:        return bNumLock ? '0' : 0;
        default:
            if ( eScancode >= SDL_SCANCODE_KP_1 && eScancode <= SDL_SCANCODE_KP_9 )
            {
                return bNumLock ? (unsigned int)('1' + (eScancode - SDL_SCANCODE_KP_1)) : 0;
            }
            break;
    }

    /* Characters of the Latin-1 range only (the PC reader returns one ANSI byte);
       0x7F is the delete key, which gives no character on PC */
    xKey = SDL_GetKeyFromScancode(eScancode, SDL_KMOD_NONE, false);
    if ( xKey == 0 || xKey >= 0x100 || xKey == 0x7F )
    {
        return 0;
    }
    return (unsigned int)xKey;
}

/*
 ----------------------------------------------------------------------------------------
  Description   : Keyboard read (INO_fn_wReadKeyboard): the other key buffer becomes the
                  current one and takes the keys down now
 ----------------------------------------------------------------------------------------
 */
short __cdecl INO_fn_wReadKeyboard(INO_tdstDevice *hDevice)
{
    INO_tdstKeyboardState *pState = (INO_tdstKeyboardState *)hDevice->p_stState;
    const bool            *p_bKeys;
    unsigned char         *pBuf;
    int                    iKey;

    if ( (pState->ucStatus & INO_C_ucConnected) == 0 || INO_sdl_wFindSlot(hDevice) < 0 )
    {
        return 0;
    }

    hDevice->ulLastTimeCount += 1;
    pState->wCurrentTab = (unsigned short)(1 - (short)pState->wCurrentTab);

    if ( !INO_sdl_bHasFocus() )
    {
        pState->ucStatus &= (unsigned char)~INO_C_ucAcquired;
        return 0;
    }
    pState->ucStatus |= INO_C_ucAcquired;
    pState->ucStatus &= (unsigned char)~INO_C_ucAccessError;

    p_bKeys = SDL_GetKeyboardState(NULL);
    pBuf    = pState->a_ucTabKey[pState->wCurrentTab];
    for ( iKey = 0; iKey < 256; iKey++ )
    {
        pBuf[iKey] = (INO_sdl_a_uwKeyToScancode[iKey] != 0
                      && p_bKeys[INO_sdl_a_uwKeyToScancode[iKey]]) ? INO_C_ucKeyDown : 0;
    }
    return 1;
}

/*
 ----------------------------------------------------------------------------------------
  Description   : A mouse axis scaled by the mouse's axis factor, within +-255
                  (as the PC reader clamps it)
 ----------------------------------------------------------------------------------------
 */
static long INO_sdl_lClampMouseAxis(INO_tdstDevice *pDevice, long lAxis)
{
    long lValue = (long)(INO_pc_xGetMouseAxis(pDevice) * lAxis);

    if ( lValue > 0xFF )
    {
        return 0xFF;
    }
    if ( lValue < -0xFF )
    {
        return -0xFF;
    }
    return lValue;
}

/*
 ----------------------------------------------------------------------------------------
  Description   : Mouse read: motion since the last read, wheel, buttons
 ----------------------------------------------------------------------------------------
 */
short __cdecl INO_sdl_wReadMouse(INO_tdstDevice *pDevice)
{
    INO_pc_tdstMouseState *pstMouse = (INO_pc_tdstMouseState *)pDevice->p_stState;
    SDL_MouseButtonFlags   ulSdlButtons;
    float                  xDx;
    float                  xDy;
    long                   lWheel;

    if ( (pstMouse->ucStatus & INO_C_ucConnected) == 0 || INO_sdl_wFindSlot(pDevice) < 0 )
    {
        return 0;
    }

    /* Read always, so that motion while unfocused is not handed out later */
    ulSdlButtons = SDL_GetRelativeMouseState(&xDx, &xDy);
    lWheel = (long)(INO_sdl_g_xWheel * INO_sdl_C_lWheelDelta);
    INO_sdl_g_xWheel = 0.0f;

    if ( !INO_sdl_bHasFocus() )
    {
        pstMouse->ucStatus &= (unsigned char)~INO_C_ucAcquired;
        return 0;
    }
    pstMouse->ucStatus = (unsigned char)((pstMouse->ucStatus & ~INO_C_ucAccessError) | INO_C_ucAcquired);

    pstMouse->lRawX = INO_sdl_lClampMouseAxis(pDevice, (long)xDx);
    pstMouse->lRawY = INO_sdl_lClampMouseAxis(pDevice, (long)xDy);
    pstMouse->lRawZ = INO_sdl_lClampMouseAxis(pDevice, lWheel);

    pstMouse->ulButtons = 0;
    if ( ulSdlButtons & SDL_BUTTON_LMASK )  pstMouse->ulButtons |= 1u << 0;
    if ( ulSdlButtons & SDL_BUTTON_RMASK )  pstMouse->ulButtons |= 1u << 1;
    if ( ulSdlButtons & SDL_BUTTON_MMASK )  pstMouse->ulButtons |= 1u << 2;
    if ( ulSdlButtons & SDL_BUTTON_X1MASK ) pstMouse->ulButtons |= 1u << 3;
    return 1;
}

/*
 ----------------------------------------------------------------------------------------
  Description   : A gamepad axis (-32768..32767) as a DirectInput axis (0..65535)
 ----------------------------------------------------------------------------------------
 */
static long INO_sdl_lPadAxis(SDL_Gamepad *p_stGamepad, SDL_GamepadAxis eAxis)
{
    return (long)SDL_GetGamepadAxis(p_stGamepad, eAxis) + 32768;
}

/*
 ----------------------------------------------------------------------------------------
  Description   : Gamepad read: axes relative to their centre, buttons
 ----------------------------------------------------------------------------------------
 */
short __cdecl INO_sdl_wReadJoystick(INO_tdstDevice *pDevice)
{
    INO_pc_tdstJoyState       *pstJoy = (INO_pc_tdstJoyState *)pDevice->p_stState;
    INO_pc_tdstAxisDeviceCaps *pstCaps = (INO_pc_tdstAxisDeviceCaps *)pDevice->p_stCaps;
    SDL_Gamepad               *p_stGamepad;
    short                      wSlot;
    int                        i;

    if ( (pstJoy->ucStatus & INO_C_ucConnected) == 0 )
    {
        return 0;
    }
    wSlot = INO_sdl_wFindSlot(pDevice);
    if ( wSlot < 0 || INO_sdl_g_a_stSlot[wSlot].p_stGamepad == NULL )
    {
        return 0;
    }
    p_stGamepad = INO_sdl_g_a_stSlot[wSlot].p_stGamepad;

    if ( !INO_sdl_bHasFocus() )
    {
        pstJoy->ucStatus &= (unsigned char)~INO_C_ucAcquired;
        return 0;
    }
    if ( !SDL_GamepadConnected(p_stGamepad) )
    {
        pstJoy->ucStatus &= (unsigned char)~INO_C_ucAcquired;
        return 0;
    }
    pstJoy->ucStatus = (unsigned char)((pstJoy->ucStatus & ~INO_C_ucAccessError) | INO_C_ucAcquired);

    pstJoy->a_lAxis[0] = INO_sdl_lPadAxis(p_stGamepad, SDL_GAMEPAD_AXIS_LEFTX) - pstCaps->a_stAxis[0].m_lCenter;
    pstJoy->a_lAxis[1] = INO_sdl_lPadAxis(p_stGamepad, SDL_GAMEPAD_AXIS_LEFTY) - pstCaps->a_stAxis[1].m_lCenter;
    pstJoy->a_lAxis[2] = 0;
    pstJoy->a_lAxis[3] = INO_sdl_lPadAxis(p_stGamepad, SDL_GAMEPAD_AXIS_RIGHTX) - pstCaps->a_stAxis[3].m_lCenter;
    pstJoy->a_lAxis[4] = INO_sdl_lPadAxis(p_stGamepad, SDL_GAMEPAD_AXIS_RIGHTY) - pstCaps->a_stAxis[4].m_lCenter;
    pstJoy->a_lAxis[5] = 0;

    pstJoy->ulButtons = 0UL;
    for ( i = 0; i < INO_sdl_C_ucNbPadButton && i < pDevice->p_stCaps->ucNbButtons; i++ )
    {
        if ( SDL_GetGamepadButton(p_stGamepad, INO_sdl_a_eGamepadButton[i]) )
        {
            pstJoy->ulButtons |= 1UL << i;
        }
    }
    if ( (pstJoy->uwDirection & C_ulJoyZDown) == C_ulJoyZDown )
    {
        pstJoy->ulButtons |= 0x10UL;
    }
    return 1;
}

#endif /* USE_SDL */
