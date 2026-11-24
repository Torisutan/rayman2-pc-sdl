#pragma once
/*
 =======================================================================================
  Name        : FIL_ini.h
  Description : Ini files in C-ANSI file mode (FILE_MODE_C_WIN95 not set)

  GAM_init.c, GLI/Init_Gli_helpers.c and SND/SND.c read and write ubi.ini and the
  sound ini through the Win32 profile string API. Without FILE_MODE_C_WIN95 those
  calls go to FIL/FIL_ini.c, which gives the Win32 results for the same files, so the
  call sites stay as they are.
 =======================================================================================
 */
#include "SND/snddef.h"

#ifndef FILE_MODE_C_WIN95

#include <windows.h>

/**********************************************************************************************/
/* Name: FIL_pc_fn_ulGetProfileStringC */
/* Goal: GetPrivateProfileStringA: the value of szKey in [szSection] (blanks and enclosing */
/*       quotes removed), else szDefault; with szKey NULL the key names of the section, with */
/*       szSection NULL the section names, each ended by '\0' and the list by a second '\0'. */
/*       Names compare without case. Returns the characters copied, without the last '\0'. */
/**********************************************************************************************/
DWORD __cdecl FIL_pc_fn_ulGetProfileStringC(LPCSTR szSection, LPCSTR szKey, LPCSTR szDefault,
                                            LPSTR szReturn, DWORD ulSize, LPCSTR szFileName);

/**********************************************************************************************/
/* Name: FIL_pc_fn_bWriteProfileStringC */
/* Goal: WritePrivateProfileStringA: set szKey in [szSection] (the section is added at the */
/*       end when missing, the file created); szValue NULL removes the key, szKey NULL the */
/*       whole section */
/**********************************************************************************************/
BOOL __cdecl FIL_pc_fn_bWriteProfileStringC(LPCSTR szSection, LPCSTR szKey, LPCSTR szValue,
                                            LPCSTR szFileName);

#define GetPrivateProfileStringA    FIL_pc_fn_ulGetProfileStringC
#define WritePrivateProfileStringA  FIL_pc_fn_bWriteProfileStringC

#endif /* FILE_MODE_C_WIN95 */
