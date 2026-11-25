#pragma once
/*
 =======================================================================================
  Name        : GLI_linkdrv.h
  Description : Renderer drivers linked into the exe (USE_GLI_DX6)
 =======================================================================================
 */
#ifdef USE_GLI_DX6

#include <windows.h>

/**********************************************************************************************/
/* Name: GLI_fn_hLoadDriver */
/* Goal: LoadLibraryA for the renderer driver: a driver linked into the exe when one has this */
/*       file name (GLI_fnv_SetDllFileName), else the DLL from the driver folder */
/**********************************************************************************************/
HMODULE __stdcall GLI_fn_hLoadDriver(const char *szFileName);

/**********************************************************************************************/
/* Name: GLI_fn_pfnGetDriverProc */
/* Goal: GetProcAddress for the driver GLI_fn_hLoadDriver returned last */
/**********************************************************************************************/
FARPROC __stdcall GLI_fn_pfnGetDriverProc(HMODULE hDriver, const char *szName);

#endif /* USE_GLI_DX6 */
