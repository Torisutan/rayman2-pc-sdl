#ifndef SND_SNDLDBIN_H_
#define SND_SNDLDBIN_H_

SndBool SND_CALL SND_fn_bStartLoadBanks2(int level);
SndBool SND_CALL SND_fn_bInitBank2(int _iBankNum,int level);
SND_tdstBlockEvent* SND_CALL SND_fn_pGetBinEvent(unsigned long _ulBinId);
tdstBlockResourceMem* SND_CALL SND_fn_pGetBinRes(unsigned long _ulBinId);
SndBool SND_CALL SND_fn_bUnLoadBinaryBank(int _iBankNum);

void SND_CALL SND_fn_vInitBankSnd(char *czPathName);

void SND_CALL SND_fn_vDesInitBankSnd(void);

/* 0x00454370 */
int  __stdcall SND_fn_bCanSetOptions(void);

#endif
