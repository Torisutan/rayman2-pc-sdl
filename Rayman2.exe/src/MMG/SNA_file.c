/*
 =======================================================================================
  Name        : SNA_file.c
  Description : Level data files in C-ANSI file mode (FILE_MODE_C_WIN95 not set)

  The file calls of SNA.c, SNA_reloc.c and GAM_init.c (SNA_pc_M_* in MMG/SNA_pc.h), on
  the C runtime's streams. A handle is the FILE pointer. Each function gives what the
  Win32 call of the other branch gives for the arguments these files pass: read or
  create-for-write opens, 32-bit seeks from the start, the current position or the end,
  plain closes.
 =======================================================================================
 */
#include <stdio.h>
#include "SND/snddef.h"

#ifndef FILE_MODE_C_WIN95

#ifndef RAYMAN2_NO_PROTECTION
#error "C-ANSI file mode needs RAYMAN2_NO_PROTECTION: the protection calls the Win32 file API through its own stubs"
#endif /* RAYMAN2_NO_PROTECTION */

#include "MMG/SNA_pc.h"

/* A failed seek (the Win32 branch gives INVALID_SET_FILE_POINTER; no caller tests it) */
#define C_ulSeekFailed   0xFFFFFFFFUL

/* A handle that is no open file (NULL, failed open): the Win32 calls fail on it and do
   nothing, while the C runtime would stop the process */
#define SNA_M_bIsNoFile(hFile)  ((hFile) == NULL || (hFile) == SNA_pc_C_hInvalidFile)

/*
 ----------------------------------------------------------------------------------------
  Description   : Write access creates or truncates ("wb", the callers create always with
                  it), else the file is opened for reading ("rb", existing files only).
                  Creation mode and attributes have no C runtime equivalent and do not
                  change what is read.
 ----------------------------------------------------------------------------------------
 */
SNA_pc_tdhFile __cdecl SNA_fn_hOpenFile(const char *szPath, unsigned long ulAccess)
{
    FILE *p_stFile = fopen(szPath, (ulAccess & SNA_pc_C_ulAccessWrite) ? "wb" : "rb");

    return p_stFile != NULL ? (SNA_pc_tdhFile)p_stFile : SNA_pc_C_hInvalidFile;
}

/*
 ----------------------------------------------------------------------------------------
  Description   : Synchronous read: the bytes read go to *p_ulRead; end of file is not an
                  error
 ----------------------------------------------------------------------------------------
 */
int __cdecl SNA_fn_bReadFile(SNA_pc_tdhFile hFile, void *p_vBuffer, unsigned long ulBytes,
                                 unsigned long *p_ulRead)
{
    FILE  *p_stFile = (FILE *)hFile;
    size_t ulRead;

    if ( SNA_M_bIsNoFile(hFile) )
    {
        if ( p_ulRead != NULL )
            *p_ulRead = 0;
        return 0;
    }
    ulRead = fread(p_vBuffer, 1, ulBytes, p_stFile);
    if ( p_ulRead != NULL )
        *p_ulRead = (unsigned long)ulRead;
    return ferror(p_stFile) ? 0 : 1;
}

/*
 ----------------------------------------------------------------------------------------
  Description   : Seek with a 32-bit distance from the start, the current position or the
                  end; the new position, or C_ulSeekFailed
 ----------------------------------------------------------------------------------------
 */
unsigned long __cdecl SNA_fn_ulSeekFile(SNA_pc_tdhFile hFile, long lDistance,
                                            unsigned long ulOrigin)
{
    FILE *p_stFile = (FILE *)hFile;
    int   iOrigin;
    long  lPosition;

    if ( SNA_M_bIsNoFile(hFile) )
        return C_ulSeekFailed;

    switch ( ulOrigin )
    {
    case SNA_pc_C_ulSeekBegin:   iOrigin = SEEK_SET; break;
    case SNA_pc_C_ulSeekCurrent: iOrigin = SEEK_CUR; break;
    case SNA_pc_C_ulSeekEnd:     iOrigin = SEEK_END; break;
    default:                     return C_ulSeekFailed;
    }
    if ( fseek(p_stFile, lDistance, iOrigin) != 0 )
        return C_ulSeekFailed;
    lPosition = ftell(p_stFile);
    return lPosition < 0 ? C_ulSeekFailed : (unsigned long)lPosition;
}

/*
 ----------------------------------------------------------------------------------------
  Description   : Close a file of SNA_fn_hOpenFile (0 for no file, as Win32 gives for
                  NULL: closing a stream that never opened is a no-op)
 ----------------------------------------------------------------------------------------
 */
int __cdecl SNA_fn_bCloseFile(SNA_pc_tdhFile hFile)
{
    if ( SNA_M_bIsNoFile(hFile) )
        return 0;
    return fclose((FILE *)hFile) == 0;
}

/*
 ----------------------------------------------------------------------------------------
  Description   : File attributes: none in the C runtime. Every file reads as normal, so
                  SNA_pc_bOpenFile has no read-only bit to clear.
 ----------------------------------------------------------------------------------------
 */
unsigned long __cdecl SNA_fn_ulGetFileAttributes(const char *szPath)
{
    (void)szPath;
    return 0;
}

int __cdecl SNA_fn_bSetFileAttributes(const char *szPath, unsigned long ulAttributes)
{
    (void)szPath; (void)ulAttributes;
    return 1;
}

#endif /* FILE_MODE_C_WIN95 */
