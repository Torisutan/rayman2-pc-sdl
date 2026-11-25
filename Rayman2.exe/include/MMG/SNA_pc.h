#ifndef __MMG_SNA_pc_h__
#define __MMG_SNA_pc_h__

#include "SND/snddef.h"
#ifndef FILE_MODE_C_WIN95
/* Level data files on the C runtime's streams (MMG/SNA_file.c). The callers pass the Win32
   values (access, creation, attributes, seek origin) and test the Win32 results, so the
   functions take and give those values. */
typedef void *SNA_pc_tdhFile;
SNA_pc_tdhFile __cdecl SNA_pc_fn_hOpenFileC(const char *szPath, unsigned long ulAccess);
int __cdecl SNA_pc_fn_bReadFileC(SNA_pc_tdhFile hFile, void *p_vBuffer, unsigned long ulBytes,
                                 unsigned long *p_ulRead);
unsigned long __cdecl SNA_pc_fn_ulSeekFileC(SNA_pc_tdhFile hFile, long lDistance,
                                            unsigned long ulOrigin);
int __cdecl SNA_pc_fn_bCloseFileC(SNA_pc_tdhFile hFile);
unsigned long __cdecl SNA_pc_fn_ulGetFileAttributesC(const char *szPath);
int __cdecl SNA_pc_fn_bSetFileAttributesC(const char *szPath, unsigned long ulAttributes);
#define SNA_pc_M_hOpenFile(szPath, ulAccess, ulCreation, ulFlags) \
    SNA_pc_fn_hOpenFileC((szPath), (ulAccess))
#define SNA_pc_M_bReadFile(hFile, p_vBuffer, ulBytes, p_ulRead) \
    SNA_pc_fn_bReadFileC((hFile), (p_vBuffer), (ulBytes), (p_ulRead))
#define SNA_pc_M_ulSeekFile(hFile, lDistance, ulOrigin) \
    SNA_pc_fn_ulSeekFileC((hFile), (lDistance), (ulOrigin))
#define SNA_pc_M_bCloseFile(hFile) SNA_pc_fn_bCloseFileC(hFile)
#define SNA_pc_M_ulGetFileAttributes(szPath) SNA_pc_fn_ulGetFileAttributesC(szPath)
#define SNA_pc_M_bSetFileAttributes(szPath, ulAttributes) \
    SNA_pc_fn_bSetFileAttributesC((szPath), (ulAttributes))
#else /* original Win32 */
typedef HANDLE SNA_pc_tdhFile;

#define SNA_pc_M_hOpenFile(szPath, ulAccess, ulCreation, ulFlags) \
    CreateFileA((szPath), (ulAccess), 0, NULL, (ulCreation), (ulFlags), NULL)
#define SNA_pc_M_bReadFile(hFile, p_vBuffer, ulBytes, p_ulRead) \
    ReadFile((hFile), (p_vBuffer), (ulBytes), (p_ulRead), NULL)
#define SNA_pc_M_ulSeekFile(hFile, lDistance, ulOrigin) \
    SetFilePointer((hFile), (lDistance), NULL, (ulOrigin))
#define SNA_pc_M_bCloseFile(hFile) CloseHandle(hFile)
#define SNA_pc_M_ulGetFileAttributes(szPath) GetFileAttributesA(szPath)
#define SNA_pc_M_bSetFileAttributes(szPath, ulAttributes) SetFileAttributesA((szPath), (ulAttributes))
#endif /* FILE_MODE_C_WIN95 */

#endif
