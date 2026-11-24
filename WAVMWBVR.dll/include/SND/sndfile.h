#ifndef _SNDFILE_H_
#define _SNDFILE_H_

#ifdef __cplusplus
extern "C" {
#endif

#undef DLL_IMPEXP
#undef CPA_EXPORT
#ifdef _DLL_COMPILATION_MODE
	#define DLL_IMPEXP __declspec(dllexport)
	#define CPA_EXPORT
#else
	#define DLL_IMPEXP

	#if defined(CPA_WANTS_IMPORT)
	#define CPA_EXPORT __declspec(dllimport)
	#elif defined(CPA_WANTS_EXPORT)
	#define CPA_EXPORT __declspec(dllexport)
	#else
	#define CPA_EXPORT
	#endif

#endif

#ifdef FILE_MODE_C_WIN95
#include <windows.h>
#endif

#ifdef FILE_MODE_C_WIN95
	#define OPENFILESND_READ		GENERIC_READ
	#define OPENFILESND_WRITE		GENERIC_WRITE
	#define OPENFILESND_FAILED		INVALID_HANDLE_VALUE
	#define SEEKFILESND_BEGIN		FILE_BEGIN
	#define SEEKFILESND_CURRENT		FILE_CURRENT
#else
	#ifdef DREAMCAST
		#define OPENFILESND_READ		0
		#define OPENFILESND_WRITE		0
		#define OPENFILESND_FAILED		NULL
		#define SEEKFILESND_BEGIN		FILE_SEEK_SET
		#define SEEKFILESND_CURRENT		FILE_SEEK_CUR
	#else
		#ifdef FILE_MODE_C_STANDARD
			#define OPENFILESND_READ		_O_RD|_O_BINARY
			#define OPENFILESND_WRITE		_O_CREAT|_O_RDWR|_O_BINARY
			#define OPENFILESND_FAILED		-1
			#define SEEKFILESND_BEGIN		FILE_BEGIN
			#define SEEKFILESND_CURRENT		FILE_CURRENT
		#else
			#define OPENFILESND_READ		1
			#define OPENFILESND_WRITE		2
			#define OPENFILESND_FAILED		3
			#define SEEKFILESND_BEGIN		4
			#define SEEKFILESND_CURRENT		5
		#endif
	#endif
#endif

typedef struct {
	unsigned char day;
	unsigned char	month;
	unsigned char	year;
}tdstDate;

typedef struct {
	unsigned char hour;
	unsigned char	minute;
	unsigned char	second;
}tdstTime;

#if defined(_DLL_COMPILATION_MODE) && !defined(SNDDLL_MODULE)
    #pragma message("-> sndfile : pointeurs de fonctions definis dans SNDDLL.H")
#else
DLL_IMPEXP CPA_EXPORT SndBool		SND_CALL SND_fn_bLoadDataInMem(void* adr,unsigned long ulNbBytes,char* czFileName,unsigned long ulOffset);
DLL_IMPEXP CPA_EXPORT SndBool		SND_CALL SND_fn_bLoadDataInMemAndSwap(void* adr,unsigned long ulNbBytes,char* czFileName,unsigned long ulOffset,SndBool bSwap);
DLL_IMPEXP CPA_EXPORT SndBool		SND_CALL SND_fn_bSetCurrentDirectory(char* czDir);
DLL_IMPEXP CPA_EXPORT SndBool		SND_CALL SND_fn_bGetCurrentDirectory(char* czDir,int iMaxChar);
DLL_IMPEXP CPA_EXPORT unsigned long	SND_CALL SND_fn_dwFileSndLength(tdhSndFile	hSndFile);
DLL_IMPEXP CPA_EXPORT unsigned long	SND_CALL SND_fn_dwWriteFileSnd(tdhSndFile hSndFile,unsigned long dwNbBytes,void * ptr);

DLL_IMPEXP CPA_EXPORT void      SND_CALL SND_fn_vAddDataDirectory(const char* czDir);
DLL_IMPEXP CPA_EXPORT void		SND_CALL SND_fn_vRemoveDataDirectory(const char* cdDir);
DLL_IMPEXP CPA_EXPORT void		SND_CALL SND_fn_vPurgeDataDirectory();
DLL_IMPEXP CPA_EXPORT SndBool   SND_CALL SND_fn_bGetOneDataDirectory(int id,char* czDir,int iMaxChar);
DLL_IMPEXP CPA_EXPORT int       SND_CALL SND_fn_iGetNumberOfDataDirectory();
DLL_IMPEXP CPA_EXPORT int       SND_CALL SND_fn_iGetIdDataDirectory(const char* szDir);
DLL_IMPEXP CPA_EXPORT SndBool   SND_CALL SND_fn_bIsDataDirectory(const char* szDir);
DLL_IMPEXP CPA_EXPORT void      SND_CALL SND_fn_vResolveFileName(char* RelativeName,char* AbsoluteName);

DLL_IMPEXP CPA_EXPORT void      SND_CALL SND_fn_vSetSoftDirectory(char* czDir);
DLL_IMPEXP CPA_EXPORT char*		SND_CALL SND_fn_szGetSoftDirectory(void);

DLL_IMPEXP CPA_EXPORT tdhSndFile	SND_CALL SND_fn_hOpenFileReadSnd(char * czFileName);
DLL_IMPEXP CPA_EXPORT unsigned long	SND_CALL SND_fn_dwReadFileSnd(tdhSndFile hSndFile,unsigned long dwNbBytes,void * ptr);
DLL_IMPEXP CPA_EXPORT unsigned long	SND_CALL SND_fn_dwSeekFileSnd(tdhSndFile hSndFile,unsigned long dwOffset,unsigned long dwOrigin);
DLL_IMPEXP CPA_EXPORT void			SND_CALL SND_fn_vCloseFileSnd(tdhSndFile hSndFile);
DLL_IMPEXP CPA_EXPORT unsigned long SND_fn_dwGetSizeFileSnd(tdhSndFile hSndFile);

DLL_IMPEXP CPA_EXPORT tdhSndFileAsync	SND_CALL SND_fn_hOpenFileReadAsyncSnd(char * czFileName);
DLL_IMPEXP CPA_EXPORT unsigned long		SND_CALL SND_fn_dwReadFileAsyncSnd(tdhSndFileAsync hSndFile,unsigned long dwNbBytes,unsigned long dwOffset,void * ptr);
DLL_IMPEXP CPA_EXPORT void				SND_CALL SND_fn_vCloseFileAsyncSnd(tdhSndFileAsync hSndFile);
DLL_IMPEXP CPA_EXPORT SndBool			SND_CALL SND_fn_bTestLastAsyncReadSnd(tdhSndFileAsync hSndFile);
DLL_IMPEXP CPA_EXPORT void				SND_CALL SND_fn_vAbortLastAsyncReadSnd(tdhSndFileAsync hSndFile);

#endif

#ifdef __cplusplus
}
#endif

#endif
