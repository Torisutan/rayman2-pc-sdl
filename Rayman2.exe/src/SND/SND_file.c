/*
 =======================================================================================
  Name        : SND_file.c
  Description : Sound files in C-ANSI file mode (FILE_MODE_C_WIN95 not set)

  The file calls of SND/sndfile.c (SND_pc_M_* in SND/sndfile.h), on the C runtime's streams.
  A handle is the FILE pointer. The banks (sndldbin.c), the streams and transitions of the
  sample driver and APM's ADPCM streams reach the files only through sndfile.c's API, so a
  handle never leaves this module's functions. Each function gives what the Win32 call of
  the other branch gives for the arguments sndfile.c passes: read or create-for-write opens,
  32-bit signed seeks from the start or the current position, plain reads, writes and
  closes, the working directory.
 =======================================================================================
 */
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "SND/snddef.h"

#ifndef FILE_MODE_C_WIN95

#if defined(WIN32)
#include <direct.h>
#define M_iChangeDir(szDir)				_chdir(szDir)
#define M_szGetDir(szDir,iMax)			_getcwd((szDir),(iMax))
#else
#include <unistd.h>
#define M_iChangeDir(szDir)				chdir(szDir)
#define M_szGetDir(szDir,iMax)			getcwd((szDir),(size_t)(iMax))
#endif /* WIN32 */

#include "SND/sndtype.h"
#include "SND/sndfile.h"

/* A failed seek or size (the Win32 branch gives INVALID_SET_FILE_POINTER / INVALID_FILE_SIZE) */
#define C_ulFileFailed					0xFFFFFFFFUL

/* A handle that is no open file (NULL, failed open): the Win32 calls fail on it and do nothing,
   while the C runtime would stop the process. The drivers seek and read a failed open. */
#define SND_M_bIsNoFile(hFile)			((hFile) == NULL || (hFile) == OPENFILESND_FAILED)

/*
 ----------------------------------------------------------------------------------------
  Description   : Write access opens for update ("r+b", the Win32 branch opens existing files
                  only), else for reading ("rb")
 ----------------------------------------------------------------------------------------
 */
tdhSndFile SND_fn_hOpenFileAnsi(const char *szName,unsigned long ulAccess)
{
	FILE *p_stFile = fopen(szName,(ulAccess & OPENFILESND_WRITE) ? "r+b" : "rb");

	return p_stFile != NULL ? (tdhSndFile)p_stFile : OPENFILESND_FAILED;
}

/*
 ----------------------------------------------------------------------------------------
  Description   : Synchronous read: the bytes read go to *p_ulRead; end of file is not an
                  error
 ----------------------------------------------------------------------------------------
 */
int SND_fn_bReadFileAnsi(tdhSndFile hFile,void *pvDst,unsigned long ulNb,unsigned long *p_ulRead)
{
	size_t ulRead;

	if (SND_M_bIsNoFile(hFile))
	{
		*p_ulRead = 0;
		errno = EBADF;
		return 0;
	}
	ulRead = fread(pvDst,1,ulNb,(FILE *)hFile);
	*p_ulRead = (unsigned long)ulRead;
	return ferror((FILE *)hFile) ? 0 : 1;
}

int SND_fn_bWriteFileAnsi(tdhSndFile hFile,const void *pvSrc,unsigned long ulNb,unsigned long *p_ulDone)
{
	size_t ulDone;

	if (SND_M_bIsNoFile(hFile))
	{
		*p_ulDone = 0;
		errno = EBADF;
		return 0;
	}
	ulDone = fwrite(pvSrc,1,ulNb,(FILE *)hFile);
	*p_ulDone = (unsigned long)ulDone;
	return ulDone == ulNb;
}

/*
 ----------------------------------------------------------------------------------------
  Description   : Seek by a 32-bit signed distance (SetFilePointer's: the callers pass
                  negative distances as unsigned long, SndAdpcm.c -8) from the start (0) or
                  the current position (1); the new position, or C_ulFileFailed
 ----------------------------------------------------------------------------------------
 */
unsigned long SND_fn_ulSeekFileAnsi(tdhSndFile hFile,unsigned long ulOffset,unsigned long ulOrigin)
{
	long lPosition;

	if (SND_M_bIsNoFile(hFile) || (ulOrigin != SEEKFILESND_BEGIN && ulOrigin != SEEKFILESND_CURRENT))
	{
		errno = EBADF;
		return C_ulFileFailed;
	}
	if (fseek((FILE *)hFile,(long)(int)ulOffset,ulOrigin == SEEKFILESND_BEGIN ? SEEK_SET : SEEK_CUR) != 0)
		return C_ulFileFailed;
	lPosition = ftell((FILE *)hFile);
	return lPosition < 0 ? C_ulFileFailed : (unsigned long)lPosition;
}

/*
 ----------------------------------------------------------------------------------------
  Description   : Size of the file in bytes; the position stays where it was
 ----------------------------------------------------------------------------------------
 */
unsigned long SND_fn_ulGetFileSizeAnsi(tdhSndFile hFile)
{
	long lPosition;
	long lSize;

	if (SND_M_bIsNoFile(hFile))
	{
		errno = EBADF;
		return C_ulFileFailed;
	}
	lPosition = ftell((FILE *)hFile);
	if (lPosition < 0 || fseek((FILE *)hFile,0,SEEK_END) != 0)
		return C_ulFileFailed;
	lSize = ftell((FILE *)hFile);
	fseek((FILE *)hFile,lPosition,SEEK_SET);
	return lSize < 0 ? C_ulFileFailed : (unsigned long)lSize;
}

/* closing no file fails and does nothing, as CloseHandle on an invalid handle */
int SND_fn_bCloseFileAnsi(tdhSndFile hFile)
{
	if (SND_M_bIsNoFile(hFile))
	{
		errno = EBADF;
		return 0;
	}
	return fclose((FILE *)hFile) == 0;
}

int SND_fn_bSetCurrentDirectoryAnsi(const char *szDir)
{
	return M_iChangeDir(szDir) == 0;
}

/* the length of the path written, 0 on failure (GetCurrentDirectoryA's success case) */
unsigned long SND_fn_ulGetCurrentDirectoryAnsi(char *szDir,unsigned long ulMax)
{
	if (ulMax == 0 || M_szGetDir(szDir,(int)ulMax) == NULL)
		return 0;
	return (unsigned long)strlen(szDir);
}

/*
 ----------------------------------------------------------------------------------------
  Description   : The text of the C runtime's last error into a new block (*pp_szText, free it
                  with SND_fn_vFreeErrorTextAnsi); its length, 0 when no block
 ----------------------------------------------------------------------------------------
 */
unsigned long SND_fn_ulFormatErrorTextAnsi(char **pp_szText)
{
	const char *szError = strerror(errno);
	size_t ulLength = strlen(szError);

	*pp_szText = (char *)malloc(ulLength + 1);
	if (*pp_szText == NULL)
		return 0;
	memcpy(*pp_szText,szError,ulLength + 1);
	return (unsigned long)ulLength;
}

void SND_fn_vFreeErrorTextAnsi(char *szText)
{
	free(szText);
}

#endif /* FILE_MODE_C_WIN95 */
