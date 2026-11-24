/*
 =======================================================================================
  Name        : SNA_file.c
  Description : Level data files in C-ANSI file mode (FILE_MODE_C_WIN95 not set)

  The Win32 file calls of SNA.c, SNA_reloc.c and GAM_init.c, on the C runtime's
  streams (MMG/SNA_file.h). A handle is the FILE pointer. Each function gives what its
  Win32 counterpart gives for the arguments these files pass: read or create-for-write
  opens, 32-bit seeks from the start, the current position or the end, plain closes.
 =======================================================================================
 */
#include <windows.h>
#include <stdio.h>
#include "SND/snddef.h"

#ifndef FILE_MODE_C_WIN95

#include "MMG/SNA_file.h"

/* A handle that is no open file (NULL, INVALID_HANDLE_VALUE): the Win32 calls fail on it and
   do nothing, while the C runtime would stop the process */
#define M_SNA_bIsNoFile(hFile)  ((hFile) == NULL || (hFile) == INVALID_HANDLE_VALUE)

/*
 ----------------------------------------------------------------------------------------
  Description   : CreateFileA: GENERIC_WRITE creates or truncates ("wb", the callers pass
                  CREATE_ALWAYS with it), else the file is opened for reading ("rb",
                  OPEN_EXISTING). Share mode, security, attributes and template have no
                  C runtime equivalent and do not change what is read.
 ----------------------------------------------------------------------------------------
 */
HANDLE __cdecl SNA_pc_fn_hCreateFileC(LPCSTR szPath, DWORD dwAccess, DWORD dwShare,
                                      LPSECURITY_ATTRIBUTES p_stSecurity, DWORD dwDisposition,
                                      DWORD dwAttributes, HANDLE hTemplate)
{
    FILE *p_stFile;

    (void)dwShare; (void)p_stSecurity; (void)dwDisposition; (void)dwAttributes; (void)hTemplate;

    p_stFile = fopen(szPath, (dwAccess & GENERIC_WRITE) ? "wb" : "rb");
    return p_stFile != NULL ? (HANDLE)p_stFile : INVALID_HANDLE_VALUE;
}

/*
 ----------------------------------------------------------------------------------------
  Description   : ReadFile, synchronous: the bytes read go to *p_dwRead; end of file is
                  not an error
 ----------------------------------------------------------------------------------------
 */
BOOL __cdecl SNA_pc_fn_bReadFileC(HANDLE hFile, LPVOID p_vBuffer, DWORD dwToRead,
                                  LPDWORD p_dwRead, LPOVERLAPPED p_stOverlapped)
{
    FILE  *p_stFile = (FILE *)hFile;
    size_t ulRead;

    (void)p_stOverlapped;

    if ( M_SNA_bIsNoFile(hFile) )
    {
        if ( p_dwRead != NULL )
            *p_dwRead = 0;
        return FALSE;
    }
    ulRead = fread(p_vBuffer, 1, dwToRead, p_stFile);
    if ( p_dwRead != NULL )
        *p_dwRead = (DWORD)ulRead;
    return ferror(p_stFile) ? FALSE : TRUE;
}

/*
 ----------------------------------------------------------------------------------------
  Description   : SetFilePointer with a 32-bit distance (the callers pass no high part):
                  FILE_BEGIN, FILE_CURRENT or FILE_END; the new position, or
                  INVALID_SET_FILE_POINTER
 ----------------------------------------------------------------------------------------
 */
DWORD __cdecl SNA_pc_fn_dwSetFilePointerC(HANDLE hFile, LONG lDistance, PLONG p_lDistanceHigh,
                                          DWORD dwMoveMethod)
{
    FILE *p_stFile = (FILE *)hFile;
    int   iOrigin;
    long  lPosition;

    (void)p_lDistanceHigh;

    if ( M_SNA_bIsNoFile(hFile) )
        return INVALID_SET_FILE_POINTER;

    switch ( dwMoveMethod )
    {
    case FILE_BEGIN:   iOrigin = SEEK_SET; break;
    case FILE_CURRENT: iOrigin = SEEK_CUR; break;
    case FILE_END:     iOrigin = SEEK_END; break;
    default:           return INVALID_SET_FILE_POINTER;
    }
    if ( fseek(p_stFile, lDistance, iOrigin) != 0 )
        return INVALID_SET_FILE_POINTER;
    lPosition = ftell(p_stFile);
    return lPosition < 0 ? INVALID_SET_FILE_POINTER : (DWORD)lPosition;
}

/*
 ----------------------------------------------------------------------------------------
  Description   : CloseHandle for a file of SNA_pc_fn_hCreateFileC (FALSE for no file,
                  as Win32 gives for NULL: closing a stream that never opened is a no-op)
 ----------------------------------------------------------------------------------------
 */
BOOL __cdecl SNA_pc_fn_bCloseHandleC(HANDLE hFile)
{
    if ( M_SNA_bIsNoFile(hFile) )
        return FALSE;
    return fclose((FILE *)hFile) == 0 ? TRUE : FALSE;
}

/*
 ----------------------------------------------------------------------------------------
  Description   : Get/SetFileAttributesA: no attributes in the C runtime. Every file reads
                  as normal, so SNA_pc_bOpenFile has no read-only bit to clear.
 ----------------------------------------------------------------------------------------
 */
DWORD __cdecl SNA_pc_fn_dwGetFileAttributesC(LPCSTR szPath)
{
    (void)szPath;
    return 0;
}

BOOL __cdecl SNA_pc_fn_bSetFileAttributesC(LPCSTR szPath, DWORD dwAttributes)
{
    (void)szPath; (void)dwAttributes;
    return TRUE;
}

#endif /* FILE_MODE_C_WIN95 */
