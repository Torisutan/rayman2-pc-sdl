#ifndef _SNDDLL_H_
#define _SNDDLL_H_

#ifdef __cplusplus
extern "C" {
#endif

extern HMODULE SND_hMpegModule;
extern HMODULE SND_hRecordModule;
extern HMODULE SND_hDbgModule;
extern HMODULE SND_pc_hWavModule;
extern HMODULE SND_pc_hCdModule;
extern HMODULE SND_pc_hAdpcmModule;

void SND_DllInitEntryPoints_StaticFunctions(HMODULE hModule);
void SND_DllInitEntryPoints_MpegFunctions(HMODULE hModule);
void SND_DllInitEntryPoints_RecordFunctions(HMODULE hModule);
void SND_DllInitEntryPoints_DbgFunctions(HMODULE hModule);
void SND_pc_DllInitEntryPoints_AdpcmFunctions(HMODULE hModule);

void * SND_CALL SND_fn_vGetHModuleMpeg(void);
void * SND_CALL SND_fn_vGetHModuleWav(void);
void * SND_CALL SND_fn_vGetHModuleCd(void);
void * SND_CALL SND_fn_vGetHModuleDbg(void);
void * SND_CALL SND_fn_vGetHModuleAdpcm(void);
void * SND_CALL SND_fn_vGetHModuleRecord(void);

void SND_CALL SND_fn_vDisplayError(unsigned short uwErrNum,char * message);
void SND_CALL SND_fn_vDisplayErrorEx(unsigned short uwErrNum,char * message,int flags);
int SND_CALL SND_fn_iErreurInit(int idErr);
void SND_CALL _snd_assert(char* test,char* file,int line);
int SND_CALL snd_erreur_init(int num_err);

void SND_CALL SND_fn_vInitMallocSnd();
void SND_CALL SND_fn_vDesInitMallocSnd();
void* SND_CALL SND_fn_pvMallocSnd(unsigned long nb_octets);
void* SND_CALL SND_fn_pvReAllocSnd(void* ptr,unsigned long nb_octets);
void SND_CALL SND_fn_vFreeSnd(void* ptr);
void* SND_CALL SND_fn_pvMallocSndEx(unsigned char id,unsigned long nb_octets);
void* SND_CALL SND_fn_pvReAllocSndEx(unsigned char id,void* ptr,unsigned long nb_octets);
void SND_CALL SND_fn_vFreeSndEx(unsigned char id,void* ptr);
void SND_CALL SND_fn_vResizeMemoryBlockSnd(unsigned char id,unsigned long size);

SndBool SND_CALL SND_fn_bLoadDataInMem(void* adr,unsigned long ulNbBytes,char* czFileName,unsigned long ulOffset);
SndBool SND_CALL SND_fn_bLoadDataInMemAndSwap(void* adr,unsigned long ulNbBytes,char* czFileName,unsigned long ulOffset,SndBool bSwap);
SndBool SND_CALL SND_fn_bSetCurrentDirectory(char* czDir);
unsigned long SND_CALL SND_fn_dwFileSndLength(tdhSndFile hSndFile);
unsigned long SND_CALL SND_fn_dwWriteFileSnd(tdhSndFile hSndFile,unsigned long dwNbBytes,void * ptr);
void SND_CALL SND_fn_vAddDataDirectory(const char* czDir);
void SND_CALL SND_fn_vRemoveDataDirectory(const char* cdDir);
void SND_CALL SND_fn_vPurgeDataDirectory();
SndBool SND_CALL SND_fn_bGetOneDataDirectory(int id,char* czDir,int iMaxChar);
int SND_CALL SND_fn_iGetNumberOfDataDirectory();
int SND_CALL SND_fn_iGetIdDataDirectory(const char* szDir);
SndBool SND_CALL SND_fn_bIsDataDirectory(const char* szDir);
void SND_CALL SND_fn_vResolveFileName(char* RelativeName,char* AbsoluteName);
void SND_CALL SND_fn_vSetSoftDirectory(char* czDir);
char* SND_CALL SND_fn_szGetSoftDirectory(void);
unsigned long SND_CALL SND_fn_dwReadFileSnd(tdhSndFile hSndFile,unsigned long dwNbBytes,void * ptr);
unsigned long SND_CALL SND_fn_dwSeekFileSnd(tdhSndFile hSndFile,unsigned long dwOffset,unsigned long dwOrigin);
tdhSndFile SND_CALL SND_fn_hOpenFileReadSnd(char * czFileName);
void SND_CALL SND_fn_vCloseFileSnd(tdhSndFile hSndFile);

int SND_CALL SND_fn_bCanSetOptions(void);
SndBool SND_CALL SND_fn_bGetOptions(LPCSTR pszSection, LPCSTR pszKey, char *pszValue, DWORD ulSize);
void SND_CALL SND_fn_vSetOptions(LPCSTR pszSection, LPCSTR pszKey, LPCSTR pszValue);
int SND_CALL SND_fn_iGetNumberOfBanks(void);
int SND_CALL SND_fn_iGetNumberOfEventInBank(int bank);
SndBool SND_CALL SND_fn_bStartLoadBanks(void);
SndBool SND_CALL SND_fn_bStartLoadBanks2(int level);
SndBool SND_CALL SND_fn_bInitBank(int _iBankNum);
SndBool SND_CALL SND_fn_bInitBank2(int _iBankNum,int level);
SndBool SND_CALL SND_fn_bEndLoadBanks(void);
SND_tdstBlockEvent* SND_CALL SND_fn_pGetBinEvent(unsigned long _ulBinId);
tdstBlockResourceMem* SND_CALL SND_fn_pGetBinRes(unsigned long _ulBinId);
SndBool SND_CALL SND_fn_bLoadBinaryBank(int _iBankNum);
SndBool SND_CALL SND_fn_bUnLoadBinaryBank(int _iBankNum);
void SND_CALL SND_fn_vInitBankSnd(char *czPathName);
void SND_CALL SND_fn_vDesInitBankSnd(void);

unsigned char SND_CALL SND_fn_ucDistanceToVolume(SndReal dist);
unsigned char SND_CALL SND_fn_ucDistanceToVolumeEx(SndReal dist,RollOffParam* rolloff);
SndReal SND_CALL SND_fn_rDopplerPitch(SndVector *src_pos,SndVector *src_vit,SndVector *mic_pos,SndVector *mic_vit);
void SND_CALL SND_fn_vSetDopplerFactor(SndReal factor);
SndReal SND_CALL SND_fn_rGetDopplerFactor(void);
unsigned char SND_CALL SND_fn_ucPositionToPan(SndVector *src,SndVector *mic,SndVector *mic_nor,SndVector *mic_tan);
unsigned char SND_CALL SND_fn_ucPositionToDolby(SndVector *src,SndVector *mic,SndVector *mic_nor);
void SND_CALL SND_fn_vSetDefaultRollOff(RollOffParam* rolloff);
void SND_CALL SND_fn_vGetDefaultRollOff(RollOffParam* rolloff);

SndReal SND_CALL SND_fn_rMulRealRealSnd(SndReal r1,SndReal r2);
SndReal SND_CALL SND_fn_rMulRealRealQuickSnd(SndReal r1,SndReal r2);
SndReal SND_CALL SND_fn_rDivRealRealSnd(SndReal r1,SndReal r2);
SndReal SND_CALL SND_fn_rAbsRealSnd(SndReal r);

SndReal SND_CALL SND_fn_rNormeVectorSnd(SndVector *v);
SndReal SND_CALL SND_fn_rPseudoNormeVectorSnd(SndVector *v);
SndReal SND_CALL SND_fn_rScalaireVectorSnd(SndVector *v1,SndVector *v2);
SndReal SND_CALL SND_fn_rPseudoScalaireNormeVectorSnd(SndVector *v1,SndVector *v2);
void SND_CALL SND_fn_vProduitVectorSnd(SndVector *v1,SndVector *v2,SndVector *v);

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

SndBool SND_CALL dbgSND_fn_bGetAskedTheme(tduRefRes *asked_theme, SndBool *transiting);
SndBool SND_CALL dbgSND_fn_bGetThemePosition(tduRefRes *current_theme, long *id_part, long *id_loop);
void SND_CALL dbgSND_fn_vGetInfoForObjectSound(long id, long type, char *str, long size_ptr);
long SND_CALL dbgSND_fn_lGetSizeOfSoundParam(struct SoundParam_ *pObj);
void SND_CALL SND_fn_vSetResNameCallback(tdpfnResNameCallback pfn);
const char * SND_CALL SND_fn_czGetResNameFromEdId(unsigned long edId);
SndBool SND_CALL dbgSND_fn_bSetResourceStaticVolume(long lResId, long lVolume);
long SND_CALL dbgSND_fn_rGetDopplerFactor(void);
void SND_CALL dbgSND_fn_vSendRequestSound(void *pEvt);

SndBool SND_CALL SND_fn_bCanFreeDataMPEG(void);
long SND_CALL SND_fn_lPlayMPEG(tduRefRes res,SampleParam *par,long prio,SND_td_pfn_vSoundCallback fn_callback,long par_callback);
void SND_CALL SND_fn_vRemoveCallbackMPEG(long voice);
SndBool SND_CALL SND_fn_bSetParamMPEG(long voice,SampleParam *par);
SndBool SND_CALL SND_fn_bTestIsPlayingMPEG(long voice);
void SND_CALL SND_fn_vStopMPEG(long voice);
void SND_CALL SND_fn_vPauseMPEG(long voice);
void SND_CALL SND_fn_vResumeMPEG(long voice);
int SND_CALL SND_fn_iInitMPEG(SND_tdstInitStruct *pInitStruct);
SndBool SND_CALL SND_fn_bTestInitMPEG(void);
void SND_CALL SND_fn_vDesInitMPEG(void);
void SND_CALL SND_fn_vSynchroMPEG();
tduRefRes SND_CALL SND_fn_tdGetResForVoiceMPEG(long lParam1);
SndBool SND_CALL SND_fn_bLoadResScriptMPEG(tdstBlockResourceDisk *disk,tdstBlockResourceMem *mem);
SndBool SND_CALL SND_fn_bLoadResBinaryMPEG(tdstBlockResourceDisk *disk,tdstBlockResourceMem *mem,char *pDataBloc);
void SND_CALL SND_fn_vUnLoadResMPEG(tdstBlockResourceMem* mem);
SndBool SND_CALL SND_fn_bIsResLoadedMPEG(tdstBlockResourceMem *mem);
SndReal SND_CALL SND_fn_rGetPosMPEG(long voice);
SndReal SND_CALL SND_fn_rGetLengthMPEG(long voice);
long SND_CALL SND_fn_lPlayTransitionMPEG(tduRefRes FirstRes,tduRefRes NextRes,SND_td_pfn_vSoundCallback transition_callback,long param_callback,SampleParam* par);
SndBool SND_CALL SND_fn_bSetParamTransitionMPEG(long voice,SampleParam *par);
SndBool SND_CALL SND_fn_bSetNextTransitionMPEG(long voice,tduRefRes new_res);
SndBool SND_CALL SND_fn_bDoTransitionWithFadeMPEG(long voice,tduRefRes new_res);
SndBool SND_CALL SND_fn_bDoTransitionWithFadeMPEG2(long voice, tduRefRes new_res, SndReal rDuration);
void SND_CALL SND_fn_vStopTransitionMPEG(long voice);
void SND_CALL SND_fn_vPauseTransitionMPEG(long voice);
void SND_CALL SND_fn_vResumeTransitionMPEG(long voice);
void SND_CALL SND_fn_vReleaseDriverMPEG(void);
void SND_CALL SND_fn_vRestoreDriverMPEG(void);
long SND_CALL SND_fn_lCreateNewBufferExMPEG(SND_tdstFormat* pformat,SND_tdstCallback* pCallback,SoundParam* par,long user_id);
long SND_CALL SND_fn_lGetPosBufferExMPEG(long id_buffer);
long SND_CALL SND_fn_lPushBufferExMPEG(long id_buffer,SND_tdstStackBuffer* pStack);
void SND_CALL SND_fn_vSetDefaultRollOffMPEG(RollOffParam* rolloff);
SndBool SND_CALL SND_fn_bCheckVersionResourceMPEG(tdstBlockResourceDisk* disk);
SndBool SND_CALL SND_fn_bSetResourceStaticVolumeMPEG(tduRefRes res, unsigned char ucVolume);

SndBool SND_CALL SND_fn_bTestFormatRecord(long lParam1,long lParam2);
long SND_CALL SND_fn_lCreateBufferRecord(long lParam1,long lParam2);
SndBool SND_CALL SND_fn_bIsBufferFullDuplexRecord(long lParam1);
SndBool SND_CALL SND_fn_bStartRecord(long lParam1);
void SND_CALL SND_fn_vStopRecord(long lParam1);
SndBool SND_CALL SND_fn_bIsWorkingRecord(long lParam1);
long SND_CALL SND_fn_lPushBufferToRecord(long lParam1,long lParam2,long lParam3);
SndBool SND_CALL SND_fn_bManageMsgForRecord(long lParam1);
int SND_CALL SND_fn_iInitRecord(void *pInitStruct);
void SND_CALL SND_fn_vDesInitRecord(void);
unsigned char SND_CALL SND_fn_ucGetInstantVuMeterRecord(void);

void SND_CALL SND_fn_vLaunchSupervisater(SND_tdstInitStruct *pInitStruct);
void SND_CALL SND_fn_vKillSupervisater();
void SND_CALL SND_fn_vSetSupervisaterActive(void);
SndBool SND_CALL SND_fn_bIsSupervisaterActive(void);
void SND_CALL dbgSND_fn_vSetProjectTitleForSuperVisater(long lParam1);
void SND_CALL dbgSND_fn_vAddEventToObject(long indice,long type,long asso,SND_tduRefEvt evt);
void SND_CALL dbgSND_fn_vKillEventToObject(long indice,long asso);
void SND_CALL dbgSND_fn_vSetPosObject(long indice,SoundParam* par);
void SND_CALL dbgSND_fn_vAddMicro(long indice);
void SND_CALL dbgSND_fn_vKillMicro(long indice);
void SND_CALL dbgSND_fn_vSetPosMicro(long indice,MicroParam* par);
void SND_CALL dbgSND_fn_vAddHistoric(SND_tduRefEvt evt,long indice,long type_obj,long asso,tdstBlockResourceMem* res);
void SND_CALL dbgSND_fn_vKillVolumeLine(long line);
void SND_CALL dbgSND_fn_vAddVolumeLine(long line);
void SND_CALL dbgSND_fn_vSetVolumeLine(long line,long vol);
void SND_CALL dbgSND_fn_vLoadBank(long lParam1);
void SND_CALL dbgSND_fn_vUnLoadBank(long lParam1);
void SND_CALL dbgSND_fn_vAddVoiceSample(long id,tduRefRes res,SampleParam *par);
void SND_CALL dbgSND_fn_vSetParVoiceSample(long id,SampleParam *par);
void SND_CALL dbgSND_fn_vDelVoiceSample(long id);
void SND_CALL dbgSND_fn_vPauseVoiceSample(long lParam1);
void SND_CALL dbgSND_fn_vResumeVoiceSample(long lParam1);
void SND_CALL dbgSND_fn_vSetProcessTimeSample(float fTime);
void * SND_CALL dbgSND_fn_pstGetSelectedResource(void);
SndBool SND_CALL dbgSND_fn_bDoesSupervisaterSelectResource(void);
void SND_CALL dbgSND_fn_vRajListesSxdVirtual2(long lParam1);
void SND_CALL dbgSND_fn_vInitListesSxdVirtual(SND_tdhVirtualModule hVirtualModule,char *szName); /* 0x1000BEC0 */
void SND_CALL dbgSND_fn_vRefreshSupervisater(void);

#include "sndxdadpcm.h"

#ifdef __cplusplus
}
#endif

#endif
