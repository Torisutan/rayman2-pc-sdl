#ifndef SND_SNDFILE_H
#define SND_SNDFILE_H

SndBool		__stdcall SND_fn_bLoadDataInMem(void* adr,unsigned long ulNbBytes,char* czFileName,unsigned long ulOffset);
SndBool		__stdcall SND_fn_bLoadDataInMemAndSwap(void* adr,unsigned long ulNbBytes,char* czFileName,unsigned long ulOffset,SndBool bSwap);
SndBool		__stdcall SND_fn_bSetCurrentDirectory(char* czDir);
SndBool		__stdcall SND_fn_bGetCurrentDirectory(char* czDir,int iMaxChar);
unsigned long	__stdcall SND_fn_dwFileSndLength(HANDLE	hSndFile);
unsigned long	__stdcall SND_fn_dwWriteFileSnd(HANDLE hSndFile,unsigned long dwNbBytes,void * ptr);

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

HANDLE	__stdcall SND_fn_hOpenFileReadSnd(char * czFileName);
unsigned long	__stdcall SND_fn_dwReadFileSnd(HANDLE hSndFile,unsigned long dwNbBytes,void * ptr);
unsigned long	__stdcall SND_fn_dwSeekFileSnd(HANDLE hSndFile,unsigned long dwOffset,unsigned long dwOrigin);
void			__stdcall SND_fn_vCloseFileSnd(HANDLE hSndFile);

#endif
