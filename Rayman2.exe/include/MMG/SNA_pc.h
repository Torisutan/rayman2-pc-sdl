#ifndef __MMG_SNA_pc_h__
#define __MMG_SNA_pc_h__

#include "SND/snddef.h"
#ifndef FILE_MODE_C_WIN95
/* Level data files on the C runtime's streams (MMG/SNA_file.c). The values keep the Win32
   numbers of the other branch (the seek origins keep End = Current + 1, SNA.c relies on it);
   creation and attributes have no C runtime meaning. */
typedef void *SNA_pc_tdhFile;

#define SNA_pc_C_hInvalidFile           ((SNA_pc_tdhFile)(long)-1)
#define SNA_pc_C_ulAccessRead           0x80000000UL
#define SNA_pc_C_ulAccessWrite          0x40000000UL
#define SNA_pc_C_ulOpenExisting         3UL
#define SNA_pc_C_ulCreateAlways         2UL
#define SNA_pc_C_ulAttributeNormal      0x80UL
#define SNA_pc_C_ulAttributeReadOnly    0x01UL
#define SNA_pc_C_ulSeekBegin            0UL
#define SNA_pc_C_ulSeekCurrent          1UL
#define SNA_pc_C_ulSeekEnd              2UL

SNA_pc_tdhFile __cdecl SNA_fn_hOpenFile(const char *szPath, unsigned long ulAccess);
int __cdecl SNA_fn_bReadFile(SNA_pc_tdhFile hFile, void *p_vBuffer, unsigned long ulBytes,
                                 unsigned long *p_ulRead);
unsigned long __cdecl SNA_fn_ulSeekFile(SNA_pc_tdhFile hFile, long lDistance,
                                            unsigned long ulOrigin);
int __cdecl SNA_fn_bCloseFile(SNA_pc_tdhFile hFile);
unsigned long __cdecl SNA_fn_ulGetFileAttributes(const char *szPath);
int __cdecl SNA_fn_bSetFileAttributes(const char *szPath, unsigned long ulAttributes);
#define SNA_pc_M_hOpenFile(szPath, ulAccess, ulCreation, ulFlags) \
    SNA_fn_hOpenFile((szPath), (ulAccess))
#define SNA_pc_M_bReadFile(hFile, p_vBuffer, ulBytes, p_ulRead) \
    SNA_fn_bReadFile((hFile), (p_vBuffer), (ulBytes), (p_ulRead))
#define SNA_pc_M_ulSeekFile(hFile, lDistance, ulOrigin) \
    SNA_fn_ulSeekFile((hFile), (lDistance), (ulOrigin))
#define SNA_pc_M_bCloseFile(hFile) SNA_fn_bCloseFile(hFile)
#define SNA_pc_M_ulGetFileAttributes(szPath) SNA_fn_ulGetFileAttributes(szPath)
#define SNA_pc_M_bSetFileAttributes(szPath, ulAttributes) \
    SNA_fn_bSetFileAttributes((szPath), (ulAttributes))
#else /* original Win32 */
typedef HANDLE SNA_pc_tdhFile;

#define SNA_pc_C_hInvalidFile           INVALID_HANDLE_VALUE
#define SNA_pc_C_ulAccessRead           GENERIC_READ
#define SNA_pc_C_ulAccessWrite          GENERIC_WRITE
#define SNA_pc_C_ulOpenExisting         OPEN_EXISTING
#define SNA_pc_C_ulCreateAlways         CREATE_ALWAYS
#define SNA_pc_C_ulAttributeNormal      FILE_ATTRIBUTE_NORMAL
#define SNA_pc_C_ulAttributeReadOnly    FILE_ATTRIBUTE_READONLY
#define SNA_pc_C_ulSeekBegin            FILE_BEGIN
#define SNA_pc_C_ulSeekCurrent          FILE_CURRENT
#define SNA_pc_C_ulSeekEnd              FILE_END

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
