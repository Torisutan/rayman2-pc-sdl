#pragma once

#define GLI_pc_M_hLoadDriver(szDllPath)            LoadLibraryA(szDllPath)
#define GLI_pc_M_pfnGetDriverProc(hDriver, szName) GetProcAddress((hDriver), (szName))
