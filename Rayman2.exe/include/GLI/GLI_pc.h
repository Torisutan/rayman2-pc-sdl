#pragma once

#ifdef USE_GLI_DX6
/* A driver linked into the exe answers GLI_fnl_InitDll's lookups; any other driver is
   still loaded from the driver folder (GLI/GLI_linkdrv.c, DEVIATIONS 13). */
#include "GLI/GLI_linkdrv.h"
#define GLI_pc_M_hLoadDriver(szDllPath)            GLI_fn_hLoadDriver(szDllPath)
#define GLI_pc_M_pfnGetDriverProc(hDriver, szName) GLI_fn_pfnGetDriverProc((hDriver), (szName))
#else /* original Win32 */
#define GLI_pc_M_hLoadDriver(szDllPath)            LoadLibraryA(szDllPath)
#define GLI_pc_M_pfnGetDriverProc(hDriver, szName) GetProcAddress((hDriver), (szName))
#endif /* USE_GLI_DX6 */
