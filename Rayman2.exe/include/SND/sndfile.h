#ifndef SND_SNDFILE_H
#define SND_SNDFILE_H

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
