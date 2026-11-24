#pragma once
/*
 =======================================================================================
  Name        : SNA_file.h
  Description : Level data files in C-ANSI file mode (FILE_MODE_C_WIN95 not set)

  SNA.c, SNA_reloc.c and GAM_init.c read the level data through the Win32 file API.
  Without FILE_MODE_C_WIN95 those calls go to the C runtime instead (MMG/SNA_file.c),
  with the Win32 calls' results, so the call sites stay as they are. Include it after
  <windows.h>. Every handle these files use is opened and closed in them.
 =======================================================================================
 */
#include "SND/snddef.h"

#ifndef FILE_MODE_C_WIN95

#ifndef RAYMAN2_NO_PROTECTION
#error "C-ANSI file mode needs RAYMAN2_NO_PROTECTION: the protection calls the Win32 file API through its own stubs"
#endif /* RAYMAN2_NO_PROTECTION */

/**********************************************************************************************/
/* Name: SNA_pc_fn_hCreateFileC */
/* Goal: CreateFileA: read (GENERIC_READ) or create for writing (GENERIC_WRITE), */
/*       INVALID_HANDLE_VALUE on failure */
/**********************************************************************************************/
HANDLE __cdecl SNA_pc_fn_hCreateFileC(LPCSTR szPath, DWORD dwAccess, DWORD dwShare,
                                      LPSECURITY_ATTRIBUTES p_stSecurity, DWORD dwDisposition,
                                      DWORD dwAttributes, HANDLE hTemplate);

/**********************************************************************************************/
/* Name: SNA_pc_fn_bReadFileC */
/* Goal: ReadFile: TRUE unless a read error (end of file is TRUE with fewer bytes read) */
/**********************************************************************************************/
BOOL __cdecl SNA_pc_fn_bReadFileC(HANDLE hFile, LPVOID p_vBuffer, DWORD dwToRead,
                                  LPDWORD p_dwRead, LPOVERLAPPED p_stOverlapped);

/**********************************************************************************************/
/* Name: SNA_pc_fn_dwSetFilePointerC */
/* Goal: SetFilePointer (32-bit distance): the new position, 0xFFFFFFFF on failure */
/**********************************************************************************************/
DWORD __cdecl SNA_pc_fn_dwSetFilePointerC(HANDLE hFile, LONG lDistance, PLONG p_lDistanceHigh,
                                          DWORD dwMoveMethod);

/**********************************************************************************************/
/* Name: SNA_pc_fn_bCloseHandleC */
/* Goal: CloseHandle for a file of SNA_pc_fn_hCreateFileC */
/**********************************************************************************************/
BOOL __cdecl SNA_pc_fn_bCloseHandleC(HANDLE hFile);

/**********************************************************************************************/
/* Name: SNA_pc_fn_dwGetFileAttributesC, SNA_pc_fn_bSetFileAttributesC */
/* Goal: Get/SetFileAttributesA as SNA_pc_bOpenFile uses them (clear read-only before a */
/*       write): the C runtime has no attributes, every file reads as normal */
/**********************************************************************************************/
DWORD __cdecl SNA_pc_fn_dwGetFileAttributesC(LPCSTR szPath);
BOOL  __cdecl SNA_pc_fn_bSetFileAttributesC(LPCSTR szPath, DWORD dwAttributes);

#define CreateFileA         SNA_pc_fn_hCreateFileC
#define ReadFile            SNA_pc_fn_bReadFileC
#define SetFilePointer      SNA_pc_fn_dwSetFilePointerC
#define CloseHandle         SNA_pc_fn_bCloseHandleC
#define GetFileAttributesA  SNA_pc_fn_dwGetFileAttributesC
#define SetFileAttributesA  SNA_pc_fn_bSetFileAttributesC

#endif /* FILE_MODE_C_WIN95 */
