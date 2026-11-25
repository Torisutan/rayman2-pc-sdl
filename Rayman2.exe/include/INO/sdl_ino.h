#pragma once
/*
 =======================================================================================
  Name        : sdl_ino.h
  Description : Input device backend through SDL (USE_SDL)

  INO/INO.c keeps the device layer (devices, historic records, key frames, demo
  readers); its DirectInput parts are fenced #ifndef USE_SDL. With USE_SDL this header,
  included by INO/INO.c, sends the calls of the common code to INO/sdl_ino.c, so the
  call sites stay. The INO_fn_ functions of the DirectInput part (INO_fn_wGetNbDevices,
  INO_fn_wReadKeyboard) keep their names in INO/sdl_ino.c.

  Key codes stay DirectInput key codes (DIK_, the PC set-1 scan codes, extended keys
  with bit 7) on every target: the key values in the level data and the options file
  index the keyboard state with them (IPT).
 =======================================================================================
 */
#ifdef USE_SDL

#include "INO/INO.h"

/**********************************************************************************************/
/* Name: INO_fn_bInitSDLInput */
/* Goal: Gamepad subsystem, the gamepads present now (DirectInput enumerates them once at */
/*       init as well), mouse wheel events. The arguments are unused. */
/**********************************************************************************************/
short __cdecl INO_fn_bInitSDLInput(HINSTANCE hInstance, HWND hWindow);

/**********************************************************************************************/
/* Name: INO_fn_vReleaseSDLInput */
/* Goal: Gamepads closed, gamepad subsystem and wheel events released */
/**********************************************************************************************/
void __cdecl INO_fn_vReleaseSDLInput(void);

/**********************************************************************************************/
/* Name: INO_fn_wAssignDeviceSlot */
/* Goal: A device to a free slot of its type (keyboard, mouse, the next gamepad): read */
/*       function, caps and status set. The slot, or -1. */
/**********************************************************************************************/
short __cdecl INO_fn_wAssignDeviceSlot(INO_tdstDevice *pDevice);

/**********************************************************************************************/
/* Name: INO_fn_wReleaseDeviceSlot */
/* Goal: The slot of a device freed (its gamepad closed) */
/**********************************************************************************************/
short __cdecl INO_fn_wReleaseDeviceSlot(INO_tdstDevice *pDevice);

/**********************************************************************************************/
/* Name: INO_fn_wRefreshDeviceCaps */
/* Goal: Connected status of a device: 1 when connected */
/**********************************************************************************************/
short __cdecl INO_fn_wRefreshDeviceCaps(INO_tdstDevice *pDevice);

/**********************************************************************************************/
/* Name: INO_fn_ulTranslateKeyToAscii */
/* Goal: Character of a key code in the current keyboard layout, no modifier applied, */
/*       0 for a key without one */
/**********************************************************************************************/
unsigned int __cdecl INO_fn_ulTranslateKeyToAscii(unsigned int uiKey, INO_tdstDevice *pDevice);

/**********************************************************************************************/
/* Name: INO_fn_wReadMouseSDL */
/* Goal: Mouse motion since the last read, wheel and buttons into the mouse state */
/**********************************************************************************************/
short __cdecl INO_fn_wReadMouseSDL(INO_tdstDevice *pDevice);

/**********************************************************************************************/
/* Name: INO_fn_wReadJoystickSDL */
/* Goal: Gamepad axes and buttons into the joystick state */
/**********************************************************************************************/
short __cdecl INO_fn_wReadJoystickSDL(INO_tdstDevice *pDevice);

#define INO_pc_bInitDirectInput            INO_fn_bInitSDLInput
#define INO_pc_fn_vReleaseDirectInput      INO_fn_vReleaseSDLInput
#define INO_pc_wAssignDeviceSlot           INO_fn_wAssignDeviceSlot
#define INO_pc_wReleaseDeviceSlot          INO_fn_wReleaseDeviceSlot
#define INO_pc_wRefreshDeviceCaps          INO_fn_wRefreshDeviceCaps
#define INO_pc_ulTranslateScanCodeToAscii  INO_fn_ulTranslateKeyToAscii
#define INO_pc_wReadMouseDI                INO_fn_wReadMouseSDL
#define INO_pc_wReadJoystickDI             INO_fn_wReadJoystickSDL

#endif /* USE_SDL */
