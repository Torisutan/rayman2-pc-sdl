#ifndef SND_SNDFILE_H
#define SND_SNDFILE_H

#include "SND/snddef.h"
#ifndef FILE_MODE_C_WIN95
/* C-ANSI file mode (SND/SND_file.c): a handle is the C runtime's stream. The values keep the Win32
 * numbers of the other branch: the drivers test a failed open against INVALID_HANDLE_VALUE and seek
 * with 0 and 1 (sndldbin.c tests the open against -1 as well). The last error is SND/snddef.h's. */
#define OPENFILESND_READ		0x80000000UL
#define OPENFILESND_WRITE		0x40000000UL
#define OPENFILESND_FAILED		((tdhSndFile)(long)-1)
#define SEEKFILESND_BEGIN		0
#define SEEKFILESND_CURRENT		1

#define tdhSndFile				void *

tdhSndFile		SND_fn_hOpenFileAnsi(const char *szName,unsigned long ulAccess);
int				SND_fn_bReadFileAnsi(tdhSndFile hFile,void *pvDst,unsigned long ulNb,unsigned long *p_ulRead);
int				SND_fn_bWriteFileAnsi(tdhSndFile hFile,const void *pvSrc,unsigned long ulNb,unsigned long *p_ulDone);
unsigned long	SND_fn_ulSeekFileAnsi(tdhSndFile hFile,unsigned long ulOffset,unsigned long ulOrigin);
unsigned long	SND_fn_ulGetFileSizeAnsi(tdhSndFile hFile);
int				SND_fn_bCloseFileAnsi(tdhSndFile hFile);
int				SND_fn_bSetCurrentDirectoryAnsi(const char *szDir);
unsigned long	SND_fn_ulGetCurrentDirectoryAnsi(char *szDir,unsigned long ulMax);
unsigned long	SND_fn_ulFormatErrorTextAnsi(char **pp_szText);
void			SND_fn_vFreeErrorTextAnsi(char *szText);

#define SND_pc_M_hOpenFile(szName,ulAccess)				SND_fn_hOpenFileAnsi((szName),(ulAccess))
#define SND_pc_M_bReadFile(hFile,pvDst,ulNb,p_ulRead)	SND_fn_bReadFileAnsi((hFile),(pvDst),(ulNb),(p_ulRead))
#define SND_pc_M_bWriteFile(hFile,pvSrc,ulNb,p_ulDone)	SND_fn_bWriteFileAnsi((hFile),(pvSrc),(ulNb),(p_ulDone))
#define SND_pc_M_ulSeekFile(hFile,ulOffset,ulOrigin)	SND_fn_ulSeekFileAnsi((hFile),(ulOffset),(ulOrigin))
#define SND_pc_M_ulGetFileSize(hFile)					SND_fn_ulGetFileSizeAnsi(hFile)
#define SND_pc_M_bCloseFile(hFile)						SND_fn_bCloseFileAnsi(hFile)
#define SND_pc_M_bSetCurrentDirectory(szDir)			SND_fn_bSetCurrentDirectoryAnsi(szDir)
#define SND_pc_M_ulGetCurrentDirectory(szDir,ulMax)		SND_fn_ulGetCurrentDirectoryAnsi((szDir),(ulMax))
/* the C runtime's text of the failed call (errno); ulErr, the last error, gives no more */
#define SND_pc_M_ulFormatErrorText(ulErr,pp_szText)		SND_fn_ulFormatErrorTextAnsi(pp_szText)
#define SND_pc_M_vFreeErrorText(szText)					SND_fn_vFreeErrorTextAnsi(szText)
#else
#define OPENFILESND_READ		GENERIC_READ
#define OPENFILESND_WRITE		GENERIC_WRITE
#define OPENFILESND_FAILED		INVALID_HANDLE_VALUE
#define SEEKFILESND_BEGIN		FILE_BEGIN
#define SEEKFILESND_CURRENT		FILE_CURRENT

#define tdhSndFile				HANDLE

#define SND_pc_M_hOpenFile(szName,ulAccess)				CreateFileA((szName),(ulAccess),FILE_SHARE_READ,NULL,OPEN_EXISTING,0,NULL)
#define SND_pc_M_bReadFile(hFile,pvDst,ulNb,p_ulRead)	ReadFile((hFile),(pvDst),(ulNb),(p_ulRead),NULL)
#define SND_pc_M_bWriteFile(hFile,pvSrc,ulNb,p_ulDone)	WriteFile((hFile),(pvSrc),(ulNb),(p_ulDone),NULL)
#define SND_pc_M_ulSeekFile(hFile,ulOffset,ulOrigin)	SetFilePointer((hFile),(ulOffset),NULL,(ulOrigin))
#define SND_pc_M_ulGetFileSize(hFile)					GetFileSize((hFile),NULL)
#define SND_pc_M_bCloseFile(hFile)						CloseHandle(hFile)
#define SND_pc_M_bSetCurrentDirectory(szDir)			SetCurrentDirectoryA(szDir)
#define SND_pc_M_ulGetCurrentDirectory(szDir,ulMax)		GetCurrentDirectoryA((ulMax),(szDir))
#define SND_pc_M_ulGetLastError()						GetLastError()

#define SND_pc_M_ulFormatErrorText(ulErr,pp_szText)		FormatMessageA(0x1100,NULL,(ulErr),0x400,(LPSTR)(pp_szText),0,NULL)
#define SND_pc_M_vFreeErrorText(szText)					LocalFree(szText)
#endif /* FILE_MODE_C_WIN95 */

SndBool		__stdcall SND_fn_bLoadDataInMem(void* adr,unsigned long ulNbBytes,char* czFileName,unsigned long ulOffset);
SndBool		__stdcall SND_fn_bLoadDataInMemAndSwap(void* adr,unsigned long ulNbBytes,char* czFileName,unsigned long ulOffset,SndBool bSwap);
SndBool		__stdcall SND_fn_bSetCurrentDirectory(char* czDir);
SndBool		__stdcall SND_fn_bGetCurrentDirectory(char* czDir,int iMaxChar);
unsigned long	__stdcall SND_fn_dwFileSndLength(tdhSndFile	hSndFile);
unsigned long	__stdcall SND_fn_dwWriteFileSnd(tdhSndFile hSndFile,unsigned long dwNbBytes,void * ptr);

void      __stdcall SND_fn_vAddDataDirectory(const char* czDir);
void		__stdcall SND_fn_vRemoveDataDirectory(const char* cdDir);
void		__stdcall SND_fn_vPurgeDataDirectory(void);
SndBool   __stdcall SND_fn_bGetOneDataDirectory(int id,char* czDir,int iMaxChar);
int       __stdcall SND_fn_iGetNumberOfDataDirectory(void);
int       __stdcall SND_fn_iGetIdDataDirectory(const char* szDir);
SndBool   __stdcall SND_fn_bIsDataDirectory(const char* szDir);
void      __stdcall SND_fn_vResolveFileName(char* RelativeName,char* AbsoluteName);

void      __stdcall SND_fn_vSetSoftDirectory(char* czDir);
char*		__stdcall SND_fn_szGetSoftDirectory(void);

tdhSndFile	__stdcall SND_fn_hOpenFileReadSnd(char * czFileName);
unsigned long	__stdcall SND_fn_dwReadFileSnd(tdhSndFile hSndFile,unsigned long dwNbBytes,void * ptr);
unsigned long	__stdcall SND_fn_dwSeekFileSnd(tdhSndFile hSndFile,unsigned long dwOffset,unsigned long dwOrigin);
void			__stdcall SND_fn_vCloseFileSnd(tdhSndFile hSndFile);

#endif
