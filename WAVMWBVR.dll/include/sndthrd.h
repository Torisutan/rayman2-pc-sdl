#ifndef _SNDTHRD_H_
#define _SNDTHRD_H_

#ifdef __cplusplus
extern "C" {
#endif

void SND_CALL SND_fn_vInitThreadSnd(void);
void SND_CALL SND_fn_vDesInitThreadSnd(void);
void SND_CALL SND_fn_vEnterCriticalSectionThreadSnd_(void);
void SND_CALL SND_fn_vQuitCriticalSectionThreadSnd(void);
void SND_CALL SND_fn_vMouchardThreadsnd(char *szFile, long lLine);
void SND_CALL SND_fn_vWaitForValueThreadSnd(unsigned long *p_ulValue, unsigned long ulExpected);
SndBool SND_CALL SND_fn_bTestSnd_Pentium(void);
SndBool SND_CALL SND_fn_bTestSnd_MMX(void);
SndBool SND_CALL SND_fn_bTestSnd_WinMM(WAVEFORMATEX *p_stFormat);
SndBool SND_CALL SND_fn_bTestSnd_Win32(void);
SndBool SND_CALL SND_fn_bTestSnd_WinNT(void);

#ifdef __cplusplus
}
#endif

#endif
