#include "SNDinc.h"

#include "snddll.h"

static tduRefRes SND_pc_stDummyRes = {0};

#define SND_M_DllFunction(ret, name, n, params, args, dflt)                         \
    static char SND_pc_sz_##name[] = "_" #name "@" #n;                              \
    typedef ret (SND_CALL *SND_pc_tdpfn_##name) params;                             \
    static SND_pc_tdpfn_##name SND_pc_pfn_##name = NULL;                            \
    ret SND_CALL name params                                                        \
    {                                                                               \
        if (SND_pc_pfn_##name == NULL)                                              \
            DebugBreak();                                                           \
        return SND_pc_pfn_##name args;                                              \
    }                                                                               \
    ret SND_CALL SND_pc_##name##_Dummy params                                       \
    {                                                                               \
        return dflt;                                                                \
    }

#define SND_M_DllProcedure(name, n, params, args)                                   \
    static char SND_pc_sz_##name[] = "_" #name "@" #n;                              \
    typedef void (SND_CALL *SND_pc_tdpfn_##name) params;                            \
    static SND_pc_tdpfn_##name SND_pc_pfn_##name = NULL;                            \
    void SND_CALL name params                                                       \
    {                                                                               \
        if (SND_pc_pfn_##name == NULL)                                              \
            DebugBreak();                                                           \
        SND_pc_pfn_##name args;                                                     \
    }                                                                               \
    void SND_CALL SND_pc_##name##_Dummy params                                      \
    {                                                                               \
    }

FARPROC SND_pc_fn_pfnGetProcAddress(HMODULE hModule, LPCSTR szProcName);

#define SND_M_DllLoad(hModule, name)                                                \
    if ((hModule) == NULL                                                           \
     || (SND_pc_pfn_##name = (SND_pc_tdpfn_##name)SND_pc_fn_pfnGetProcAddress(hModule, SND_pc_sz_##name)) == NULL) \
        SND_pc_pfn_##name = SND_pc_##name##_Dummy;

SND_M_DllFunction(void *, SND_fn_vGetHModuleMpeg, 0, (void), (), NULL)
SND_M_DllFunction(void *, SND_fn_vGetHModuleWav, 0, (void), (), NULL)
SND_M_DllFunction(void *, SND_fn_vGetHModuleCd, 0, (void), (), NULL)
SND_M_DllFunction(void *, SND_fn_vGetHModuleDbg, 0, (void), (), NULL)
SND_M_DllFunction(void *, SND_fn_vGetHModuleAdpcm, 0, (void), (), NULL)
SND_M_DllFunction(void *, SND_fn_vGetHModuleRecord, 0, (void), (), NULL)

SND_M_DllProcedure(SND_fn_vDisplayError, 8, (unsigned short uwErrNum,char * message), (uwErrNum,message))
SND_M_DllProcedure(SND_fn_vDisplayErrorEx, 12, (unsigned short uwErrNum,char * message,int flags), (uwErrNum,message,flags))
SND_M_DllFunction(int, SND_fn_iErreurInit, 4, (int idErr), (idErr), 0)
SND_M_DllProcedure(_snd_assert, 12, (char* test,char* file,int line), (test,file,line))
SND_M_DllFunction(int, snd_erreur_init, 4, (int num_err), (num_err), 0)

SND_M_DllProcedure(SND_fn_vInitMallocSnd, 0, (), ())
SND_M_DllProcedure(SND_fn_vDesInitMallocSnd, 0, (), ())
SND_M_DllFunction(void*, SND_fn_pvMallocSnd, 4, (unsigned long nb_octets), (nb_octets), NULL)
SND_M_DllFunction(void*, SND_fn_pvReAllocSnd, 8, (void* ptr,unsigned long nb_octets), (ptr,nb_octets), NULL)
SND_M_DllProcedure(SND_fn_vFreeSnd, 4, (void* ptr), (ptr))
SND_M_DllFunction(void*, SND_fn_pvMallocSndEx, 8, (unsigned char id,unsigned long nb_octets), (id,nb_octets), NULL)
SND_M_DllFunction(void*, SND_fn_pvReAllocSndEx, 12, (unsigned char id,void* ptr,unsigned long nb_octets), (id,ptr,nb_octets), NULL)
SND_M_DllProcedure(SND_fn_vFreeSndEx, 8, (unsigned char id,void* ptr), (id,ptr))
SND_M_DllProcedure(SND_fn_vResizeMemoryBlockSnd, 8, (unsigned char id,unsigned long size), (id,size))

SND_M_DllFunction(SndBool, SND_fn_bLoadDataInMem, 16, (void* adr,unsigned long ulNbBytes,char* czFileName,unsigned long ulOffset), (adr,ulNbBytes,czFileName,ulOffset), FALSE)
SND_M_DllFunction(SndBool, SND_fn_bLoadDataInMemAndSwap, 20, (void* adr,unsigned long ulNbBytes,char* czFileName,unsigned long ulOffset,SndBool bSwap), (adr,ulNbBytes,czFileName,ulOffset,bSwap), FALSE)
SND_M_DllFunction(SndBool, SND_fn_bSetCurrentDirectory, 4, (char* czDir), (czDir), FALSE)
SND_M_DllFunction(unsigned long, SND_fn_dwFileSndLength, 4, (tdhSndFile hSndFile), (hSndFile), 0)
SND_M_DllFunction(unsigned long, SND_fn_dwWriteFileSnd, 12, (tdhSndFile hSndFile,unsigned long dwNbBytes,void * ptr), (hSndFile,dwNbBytes,ptr), 0)
SND_M_DllProcedure(SND_fn_vAddDataDirectory, 4, (const char* czDir), (czDir))
SND_M_DllProcedure(SND_fn_vRemoveDataDirectory, 4, (const char* cdDir), (cdDir))
SND_M_DllProcedure(SND_fn_vPurgeDataDirectory, 0, (), ())
SND_M_DllFunction(SndBool, SND_fn_bGetOneDataDirectory, 12, (int id,char* czDir,int iMaxChar), (id,czDir,iMaxChar), FALSE)
SND_M_DllFunction(int, SND_fn_iGetNumberOfDataDirectory, 0, (), (), 0)
SND_M_DllFunction(int, SND_fn_iGetIdDataDirectory, 4, (const char* szDir), (szDir), 0)
SND_M_DllFunction(SndBool, SND_fn_bIsDataDirectory, 4, (const char* szDir), (szDir), FALSE)
SND_M_DllProcedure(SND_fn_vResolveFileName, 8, (char* RelativeName,char* AbsoluteName), (RelativeName,AbsoluteName))
SND_M_DllProcedure(SND_fn_vSetSoftDirectory, 4, (char* czDir), (czDir))
SND_M_DllFunction(char*, SND_fn_szGetSoftDirectory, 0, (void), (), NULL)
SND_M_DllFunction(unsigned long, SND_fn_dwReadFileSnd, 12, (tdhSndFile hSndFile,unsigned long dwNbBytes,void * ptr), (hSndFile,dwNbBytes,ptr), 0)
SND_M_DllFunction(unsigned long, SND_fn_dwSeekFileSnd, 12, (tdhSndFile hSndFile,unsigned long dwOffset,unsigned long dwOrigin), (hSndFile,dwOffset,dwOrigin), 0)
SND_M_DllFunction(tdhSndFile, SND_fn_hOpenFileReadSnd, 4, (char * czFileName), (czFileName), 0)
SND_M_DllProcedure(SND_fn_vCloseFileSnd, 4, (tdhSndFile hSndFile), (hSndFile))

SND_M_DllFunction(int, SND_fn_bCanSetOptions, 0, (void), (), 0)
SND_M_DllFunction(SndBool, SND_fn_bGetOptions, 16, (LPCSTR pszSection, LPCSTR pszKey, char *pszValue, DWORD ulSize), (pszSection,pszKey,pszValue,ulSize), FALSE)
SND_M_DllProcedure(SND_fn_vSetOptions, 12, (LPCSTR pszSection, LPCSTR pszKey, LPCSTR pszValue), (pszSection,pszKey,pszValue))
SND_M_DllFunction(int, SND_fn_iGetNumberOfBanks, 0, (void), (), 0)
SND_M_DllFunction(int, SND_fn_iGetNumberOfEventInBank, 4, (int bank), (bank), 0)
SND_M_DllFunction(SndBool, SND_fn_bStartLoadBanks, 0, (void), (), FALSE)
SND_M_DllFunction(SndBool, SND_fn_bStartLoadBanks2, 4, (int level), (level), FALSE)
SND_M_DllFunction(SndBool, SND_fn_bInitBank, 4, (int _iBankNum), (_iBankNum), FALSE)
SND_M_DllFunction(SndBool, SND_fn_bInitBank2, 8, (int _iBankNum,int level), (_iBankNum,level), FALSE)
SND_M_DllFunction(SndBool, SND_fn_bEndLoadBanks, 0, (void), (), FALSE)
SND_M_DllFunction(SND_tdstBlockEvent*, SND_fn_pGetBinEvent, 4, (unsigned long _ulBinId), (_ulBinId), NULL)
SND_M_DllFunction(tdstBlockResourceMem*, SND_fn_pGetBinRes, 4, (unsigned long _ulBinId), (_ulBinId), NULL)
SND_M_DllFunction(SndBool, SND_fn_bLoadBinaryBank, 4, (int _iBankNum), (_iBankNum), FALSE)
SND_M_DllFunction(SndBool, SND_fn_bUnLoadBinaryBank, 4, (int _iBankNum), (_iBankNum), FALSE)
SND_M_DllProcedure(SND_fn_vInitBankSnd, 4, (char *czPathName), (czPathName))
SND_M_DllProcedure(SND_fn_vDesInitBankSnd, 0, (void), ())

SND_M_DllFunction(unsigned char, SND_fn_ucDistanceToVolume, 4, (SndReal dist), (dist), 0)
SND_M_DllFunction(unsigned char, SND_fn_ucDistanceToVolumeEx, 8, (SndReal dist,RollOffParam* rolloff), (dist,rolloff), 0)
SND_M_DllFunction(SndReal, SND_fn_rDopplerPitch, 16, (SndVector *src_pos,SndVector *src_vit,SndVector *mic_pos,SndVector *mic_vit), (src_pos,src_vit,mic_pos,mic_vit), 0)
SND_M_DllProcedure(SND_fn_vSetDopplerFactor, 4, (SndReal factor), (factor))
SND_M_DllFunction(SndReal, SND_fn_rGetDopplerFactor, 0, (void), (), 0)
SND_M_DllFunction(unsigned char, SND_fn_ucPositionToPan, 16, (SndVector *src,SndVector *mic,SndVector *mic_nor,SndVector *mic_tan), (src,mic,mic_nor,mic_tan), 0)
SND_M_DllFunction(unsigned char, SND_fn_ucPositionToDolby, 12, (SndVector *src,SndVector *mic,SndVector *mic_nor), (src,mic,mic_nor), 0)
SND_M_DllProcedure(SND_fn_vSetDefaultRollOff, 4, (RollOffParam* rolloff), (rolloff))
SND_M_DllProcedure(SND_fn_vGetDefaultRollOff, 4, (RollOffParam* rolloff), (rolloff))

SND_M_DllFunction(SndReal, SND_fn_rMulRealRealSnd, 8, (SndReal r1,SndReal r2), (r1,r2), 0)
SND_M_DllFunction(SndReal, SND_fn_rMulRealRealQuickSnd, 8, (SndReal r1,SndReal r2), (r1,r2), 0)
SND_M_DllFunction(SndReal, SND_fn_rDivRealRealSnd, 8, (SndReal r1,SndReal r2), (r1,r2), 0)
SND_M_DllFunction(SndReal, SND_fn_rAbsRealSnd, 4, (SndReal r), (r), 0)

SND_M_DllFunction(SndReal, SND_fn_rNormeVectorSnd, 4, (SndVector *v), (v), 0)
SND_M_DllFunction(SndReal, SND_fn_rPseudoNormeVectorSnd, 4, (SndVector *v), (v), 0)
SND_M_DllFunction(SndReal, SND_fn_rScalaireVectorSnd, 8, (SndVector *v1,SndVector *v2), (v1,v2), 0)
SND_M_DllFunction(SndReal, SND_fn_rPseudoScalaireNormeVectorSnd, 8, (SndVector *v1,SndVector *v2), (v1,v2), 0)
SND_M_DllProcedure(SND_fn_vProduitVectorSnd, 12, (SndVector *v1,SndVector *v2,SndVector *v), (v1,v2,v))

SND_M_DllProcedure(SND_fn_vInitThreadSnd, 0, (void), ())
SND_M_DllProcedure(SND_fn_vDesInitThreadSnd, 0, (void), ())
SND_M_DllProcedure(SND_fn_vEnterCriticalSectionThreadSnd_, 0, (void), ())
SND_M_DllProcedure(SND_fn_vQuitCriticalSectionThreadSnd, 0, (void), ())
SND_M_DllProcedure(SND_fn_vMouchardThreadsnd, 8, (char *szFile, long lLine), (szFile,lLine))
SND_M_DllProcedure(SND_fn_vWaitForValueThreadSnd, 8, (unsigned long *p_ulValue, unsigned long ulExpected), (p_ulValue,ulExpected))
SND_M_DllFunction(SndBool, SND_fn_bTestSnd_Pentium, 0, (void), (), FALSE)
SND_M_DllFunction(SndBool, SND_fn_bTestSnd_MMX, 0, (void), (), FALSE)
SND_M_DllFunction(SndBool, SND_fn_bTestSnd_WinMM, 4, (WAVEFORMATEX *p_stFormat), (p_stFormat), FALSE)
SND_M_DllFunction(SndBool, SND_fn_bTestSnd_Win32, 0, (void), (), FALSE)
SND_M_DllFunction(SndBool, SND_fn_bTestSnd_WinNT, 0, (void), (), FALSE)

SND_M_DllFunction(SndBool, dbgSND_fn_bGetAskedTheme, 8, (tduRefRes *asked_theme, SndBool *transiting), (asked_theme,transiting), FALSE)
SND_M_DllFunction(SndBool, dbgSND_fn_bGetThemePosition, 12, (tduRefRes *current_theme, long *id_part, long *id_loop), (current_theme,id_part,id_loop), FALSE)
SND_M_DllProcedure(dbgSND_fn_vGetInfoForObjectSound, 16, (long id, long type, char *str, long size_ptr), (id,type,str,size_ptr))
SND_M_DllFunction(long, dbgSND_fn_lGetSizeOfSoundParam, 4, (struct SoundParam_ *pObj), (pObj), 0)
SND_M_DllProcedure(SND_fn_vSetResNameCallback, 4, (tdpfnResNameCallback pfn), (pfn))
SND_M_DllFunction(const char *, SND_fn_czGetResNameFromEdId, 4, (unsigned long edId), (edId), NULL)
SND_M_DllFunction(SndBool, dbgSND_fn_bSetResourceStaticVolume, 8, (long lResId, long lVolume), (lResId,lVolume), FALSE)
SND_M_DllFunction(long, dbgSND_fn_rGetDopplerFactor, 0, (void), (), C_SNDREAL_1)
SND_M_DllProcedure(dbgSND_fn_vSendRequestSound, 4, (void *pEvt), (pEvt))

SND_M_DllFunction(SndBool, SND_fn_bCanFreeDataMPEG, 0, (void), (), FALSE)
SND_M_DllFunction(long, SND_fn_lPlayMPEG, 20, (tduRefRes res,SampleParam *par,long prio,SND_td_pfn_vSoundCallback fn_callback,long par_callback), (res,par,prio,fn_callback,par_callback), -1)
SND_M_DllProcedure(SND_fn_vRemoveCallbackMPEG, 4, (long voice), (voice))
SND_M_DllFunction(SndBool, SND_fn_bSetParamMPEG, 8, (long voice,SampleParam *par), (voice,par), FALSE)
SND_M_DllFunction(SndBool, SND_fn_bTestIsPlayingMPEG, 4, (long voice), (voice), FALSE)
SND_M_DllProcedure(SND_fn_vStopMPEG, 4, (long voice), (voice))
SND_M_DllProcedure(SND_fn_vPauseMPEG, 4, (long voice), (voice))
SND_M_DllProcedure(SND_fn_vResumeMPEG, 4, (long voice), (voice))
SND_M_DllFunction(int, SND_fn_iInitMPEG, 4, (SND_tdstInitStruct *pInitStruct), (pInitStruct), 0)
SND_M_DllFunction(SndBool, SND_fn_bTestInitMPEG, 0, (void), (), FALSE)
SND_M_DllProcedure(SND_fn_vDesInitMPEG, 0, (void), ())
SND_M_DllProcedure(SND_fn_vSynchroMPEG, 0, (), ())
SND_M_DllFunction(tduRefRes, SND_fn_tdGetResForVoiceMPEG, 4, (long lParam1), (lParam1), SND_pc_stDummyRes)
SND_M_DllFunction(SndBool, SND_fn_bLoadResScriptMPEG, 8, (tdstBlockResourceDisk *disk,tdstBlockResourceMem *mem), (disk,mem), FALSE)
SND_M_DllFunction(SndBool, SND_fn_bLoadResBinaryMPEG, 12, (tdstBlockResourceDisk *disk,tdstBlockResourceMem *mem,char *pDataBloc), (disk,mem,pDataBloc), FALSE)
SND_M_DllProcedure(SND_fn_vUnLoadResMPEG, 4, (tdstBlockResourceMem* mem), (mem))
SND_M_DllFunction(SndBool, SND_fn_bIsResLoadedMPEG, 4, (tdstBlockResourceMem *mem), (mem), FALSE)
SND_M_DllFunction(SndReal, SND_fn_rGetPosMPEG, 4, (long voice), (voice), 0)
SND_M_DllFunction(SndReal, SND_fn_rGetLengthMPEG, 4, (long voice), (voice), 0)
SND_M_DllFunction(long, SND_fn_lPlayTransitionMPEG, 20, (tduRefRes FirstRes,tduRefRes NextRes,SND_td_pfn_vSoundCallback transition_callback,long param_callback,SampleParam* par), (FirstRes,NextRes,transition_callback,param_callback,par), -1)
SND_M_DllFunction(SndBool, SND_fn_bSetParamTransitionMPEG, 8, (long voice,SampleParam *par), (voice,par), FALSE)
SND_M_DllFunction(SndBool, SND_fn_bSetNextTransitionMPEG, 8, (long voice,tduRefRes new_res), (voice,new_res), FALSE)
SND_M_DllFunction(SndBool, SND_fn_bDoTransitionWithFadeMPEG, 8, (long voice,tduRefRes new_res), (voice,new_res), FALSE)
SND_M_DllFunction(SndBool, SND_fn_bDoTransitionWithFadeMPEG2, 12, (long voice, tduRefRes new_res, SndReal rDuration), (voice,new_res,rDuration), FALSE)
SND_M_DllProcedure(SND_fn_vStopTransitionMPEG, 4, (long voice), (voice))
SND_M_DllProcedure(SND_fn_vPauseTransitionMPEG, 4, (long voice), (voice))
SND_M_DllProcedure(SND_fn_vResumeTransitionMPEG, 4, (long voice), (voice))
SND_M_DllProcedure(SND_fn_vReleaseDriverMPEG, 0, (void), ())
SND_M_DllProcedure(SND_fn_vRestoreDriverMPEG, 0, (void), ())
SND_M_DllFunction(long, SND_fn_lCreateNewBufferExMPEG, 16, (SND_tdstFormat* pformat,SND_tdstCallback* pCallback,SoundParam* par,long user_id), (pformat,pCallback,par,user_id), -1)
SND_M_DllFunction(long, SND_fn_lGetPosBufferExMPEG, 4, (long id_buffer), (id_buffer), 0)
SND_M_DllFunction(long, SND_fn_lPushBufferExMPEG, 8, (long id_buffer,SND_tdstStackBuffer* pStack), (id_buffer,pStack), 0)
SND_M_DllProcedure(SND_fn_vSetDefaultRollOffMPEG, 4, (RollOffParam* rolloff), (rolloff))
SND_M_DllFunction(SndBool, SND_fn_bCheckVersionResourceMPEG, 4, (tdstBlockResourceDisk* disk), (disk), TRUE)
SND_M_DllFunction(SndBool, SND_fn_bSetResourceStaticVolumeMPEG, 8, (tduRefRes res, unsigned char ucVolume), (res,ucVolume), FALSE)

SND_M_DllFunction(SndBool, SND_fn_bTestFormatRecord, 8, (long lParam1,long lParam2), (lParam1,lParam2), FALSE)
SND_M_DllFunction(long, SND_fn_lCreateBufferRecord, 8, (long lParam1,long lParam2), (lParam1,lParam2), 0)
SND_M_DllFunction(SndBool, SND_fn_bIsBufferFullDuplexRecord, 4, (long lParam1), (lParam1), FALSE)
SND_M_DllFunction(SndBool, SND_fn_bStartRecord, 4, (long lParam1), (lParam1), FALSE)
SND_M_DllProcedure(SND_fn_vStopRecord, 4, (long lParam1), (lParam1))
SND_M_DllFunction(SndBool, SND_fn_bIsWorkingRecord, 4, (long lParam1), (lParam1), FALSE)
SND_M_DllFunction(long, SND_fn_lPushBufferToRecord, 12, (long lParam1,long lParam2,long lParam3), (lParam1,lParam2,lParam3), 0)
SND_M_DllFunction(SndBool, SND_fn_bManageMsgForRecord, 4, (long lParam1), (lParam1), FALSE)
SND_M_DllFunction(int, SND_fn_iInitRecord, 4, (void *pInitStruct), (pInitStruct), 0)
SND_M_DllProcedure(SND_fn_vDesInitRecord, 0, (void), ())
SND_M_DllFunction(unsigned char, SND_fn_ucGetInstantVuMeterRecord, 0, (void), (), 0)

SND_M_DllProcedure(SND_fn_vLaunchSupervisater, 4, (SND_tdstInitStruct *pInitStruct), (pInitStruct))
SND_M_DllProcedure(SND_fn_vKillSupervisater, 0, (), ())
SND_M_DllProcedure(SND_fn_vSetSupervisaterActive, 0, (void), ())
SND_M_DllFunction(SndBool, SND_fn_bIsSupervisaterActive, 0, (void), (), FALSE)
SND_M_DllProcedure(dbgSND_fn_vSetProjectTitleForSuperVisater, 4, (long lParam1), (lParam1))
SND_M_DllProcedure(dbgSND_fn_vAddEventToObject, 16, (long indice,long type,long asso,SND_tduRefEvt evt), (indice,type,asso,evt))
SND_M_DllProcedure(dbgSND_fn_vKillEventToObject, 8, (long indice,long asso), (indice,asso))
SND_M_DllProcedure(dbgSND_fn_vSetPosObject, 8, (long indice,SoundParam* par), (indice,par))
SND_M_DllProcedure(dbgSND_fn_vAddMicro, 4, (long indice), (indice))
SND_M_DllProcedure(dbgSND_fn_vKillMicro, 4, (long indice), (indice))
SND_M_DllProcedure(dbgSND_fn_vSetPosMicro, 8, (long indice,MicroParam* par), (indice,par))
SND_M_DllProcedure(dbgSND_fn_vAddHistoric, 20, (SND_tduRefEvt evt,long indice,long type_obj,long asso,tdstBlockResourceMem* res), (evt,indice,type_obj,asso,res))
SND_M_DllProcedure(dbgSND_fn_vKillVolumeLine, 4, (long line), (line))
SND_M_DllProcedure(dbgSND_fn_vAddVolumeLine, 4, (long line), (line))
SND_M_DllProcedure(dbgSND_fn_vSetVolumeLine, 8, (long line,long vol), (line,vol))
SND_M_DllProcedure(dbgSND_fn_vLoadBank, 4, (long lParam1), (lParam1))
SND_M_DllProcedure(dbgSND_fn_vUnLoadBank, 4, (long lParam1), (lParam1))
SND_M_DllProcedure(dbgSND_fn_vAddVoiceSample, 12, (long id,tduRefRes res,SampleParam *par), (id,res,par))
SND_M_DllProcedure(dbgSND_fn_vSetParVoiceSample, 8, (long id,SampleParam *par), (id,par))
SND_M_DllProcedure(dbgSND_fn_vDelVoiceSample, 4, (long id), (id))
SND_M_DllProcedure(dbgSND_fn_vPauseVoiceSample, 4, (long lParam1), (lParam1))
SND_M_DllProcedure(dbgSND_fn_vResumeVoiceSample, 4, (long lParam1), (lParam1))

SND_M_DllProcedure(dbgSND_fn_vSetProcessTimeSample, 4, (float fTime), (fTime))
SND_M_DllFunction(void *, dbgSND_fn_pstGetSelectedResource, 0, (void), (), NULL)
SND_M_DllFunction(SndBool, dbgSND_fn_bDoesSupervisaterSelectResource, 0, (void), (), FALSE)
SND_M_DllProcedure(dbgSND_fn_vRajListesSxdVirtual2, 4, (long lParam1), (lParam1))
SND_M_DllProcedure(dbgSND_fn_vInitListesSxdVirtual, 8, (SND_tdhVirtualModule hVirtualModule,char *szName), (hVirtualModule,szName))
SND_M_DllProcedure(dbgSND_fn_vRefreshSupervisater, 0, (void), ())

SND_M_DllFunction(SndBool, SND_fn_bCanFreeDataADPCM, 0, (void), (), FALSE)
SND_M_DllFunction(long, SND_fn_lPlaySxdADPCM, 20, (tduRefRes res,SoundParam *par,long prio,SND_td_pfn_vSoundCallback fn_callback,long par_callback), (res,par,prio,fn_callback,par_callback), -1)
SND_M_DllProcedure(SND_fn_vRemoveCallbackSxdADPCM, 4, (long voice), (voice))
SND_M_DllFunction(SndBool, SND_fn_bSetParamSxdADPCM, 8, (long voice,SoundParam *par), (voice,par), FALSE)
SND_M_DllFunction(SndBool, SND_fn_bTestIsPlayingSxdADPCM, 4, (long voice), (voice), FALSE)
SND_M_DllProcedure(SND_fn_vStopSxdADPCM, 4, (long voice), (voice))
SND_M_DllProcedure(SND_fn_vPauseSxdADPCM, 4, (long voice), (voice))
SND_M_DllProcedure(SND_fn_vResumeSxdADPCM, 4, (long voice), (voice))
SND_M_DllFunction(SndBool, SND_fn_bIsResourceLoopingSxdADPCM, 4, (tduRefRes res), (res), FALSE)
SND_M_DllFunction(SndReal, SND_fn_rGetPosSxdADPCM, 4, (long voice), (voice), 0)
SND_M_DllFunction(SndReal, SND_fn_rGetLengthSxdADPCM, 4, (long voice), (voice), 0)
SND_M_DllProcedure(SND_fn_vSetSoundVolumeSxdADPCM, 4, (unsigned char vol), (vol))
SND_M_DllFunction(unsigned char, SND_fn_ucGetSoundVolumeSxdADPCM, 0, (), (), 0)
SND_M_DllProcedure(SND_fn_vSetStereoSxdADPCM, 4, (SndBool active), (active))
SND_M_DllFunction(SndBool, SND_fn_bGetStereoSxdADPCM, 0, (void), (), FALSE)
SND_M_DllProcedure(SND_fn_vSetReverseStereoSxdADPCM, 4, (SndBool active), (active))
SND_M_DllFunction(SndBool, SND_fn_bGetReverseStereoSxdADPCM, 0, (void), (), FALSE)
SND_M_DllFunction(int, SND_fn_iInitSxdADPCM, 4, (SND_tdstInitStruct *pInitStruct), (pInitStruct), 0)
SND_M_DllFunction(SndBool, SND_fn_bTestInitSxdADPCM, 0, (void), (), FALSE)
SND_M_DllProcedure(SND_fn_vDesInitSxdADPCM, 0, (void), ())
SND_M_DllProcedure(SND_fn_vSetEffectSxdADPCM, 4, (long num), (num))
SND_M_DllProcedure(SND_fn_vCreateMicroSxdADPCM, 8, (long lParam1,long lParam2), (lParam1,lParam2))
SND_M_DllProcedure(SND_fn_vSetMicroParamSxdADPCM, 8, (long sxdId,MicroParam* par), (sxdId,par))
SND_M_DllProcedure(SND_fn_vDestroyMicroSxdADPCM, 4, (long sxdId), (sxdId))
SND_M_DllFunction(long, SND_fn_lCreateNewBufferSxdADPCM, 28, (unsigned long nb_samples,unsigned short uwResolution,unsigned short uwNbChannels,unsigned long ulFreq,SoundParam* par,td_pfn_vRefreshBufferClient callback,long user_id), (nb_samples,uwResolution,uwNbChannels,ulFreq,par,callback,user_id), -1)
SND_M_DllProcedure(SND_fn_vSetParamBufferSxdADPCM, 8, (long id_buffer,SoundParam *par), (id_buffer,par))
SND_M_DllProcedure(SND_fn_vDeleteBufferSxdADPCM, 4, (long id_buffer), (id_buffer))
SND_M_DllProcedure(SND_fn_vPauseBufferSxdADPCM, 4, (long id_buffer), (id_buffer))
SND_M_DllProcedure(SND_fn_vResumeBufferSxdADPCM, 4, (long id_buffer), (id_buffer))
SND_M_DllFunction(void*, SND_fn_pvGetPtrBufferSxdADPCM, 4, (long id_buffer), (id_buffer), NULL)
SND_M_DllFunction(SndReal, SND_fn_rGetPosBufferSxdADPCM, 4, (long id_buffer), (id_buffer), 0)
SND_M_DllFunction(SndBool, SND_fn_bLoadResScriptSxdADPCM, 8, (tdstBlockResourceDisk *disk,tdstBlockResourceMem *mem), (disk,mem), FALSE)
SND_M_DllFunction(SndBool, SND_fn_bLoadResBinarySxdADPCM, 12, (tdstBlockResourceDisk *disk,tdstBlockResourceMem *mem,char *pDataBloc), (disk,mem,pDataBloc), FALSE)
SND_M_DllFunction(SndBool, SND_fn_bLoadResScriptAndConvertToWAVSxdADPCM, 8, (tdstBlockResourceDisk *disk,tdstBlockResourceMem *mem), (disk,mem), FALSE)
SND_M_DllFunction(SndBool, SND_fn_bLoadResBinaryAndConvertToWAVSxdADPCM, 12, (tdstBlockResourceDisk *disk,tdstBlockResourceMem *mem,char *pDataBloc), (disk,mem,pDataBloc), FALSE)
SND_M_DllProcedure(SND_fn_vUnLoadResSxdADPCM, 4, (tdstBlockResourceMem* mem), (mem))
SND_M_DllFunction(SndBool, SND_fn_bIsResLoadedSxdADPCM, 4, (tdstBlockResourceMem *mem), (mem), FALSE)
SND_M_DllFunction(long, SND_fn_lPlayTransitionSxdADPCM, 20, (tduRefRes FirstRes,tduRefRes NextRes,SND_td_pfn_vSoundCallback transition_callback,long param_callback,SoundParam* par), (FirstRes,NextRes,transition_callback,param_callback,par), -1)
SND_M_DllFunction(SndBool, SND_fn_bSetParamTransitionSxdADPCM, 8, (long voice,SoundParam *par), (voice,par), FALSE)
SND_M_DllFunction(SndBool, SND_fn_bSetNextTransitionSxdADPCM, 8, (long voice,tduRefRes new_res), (voice,new_res), FALSE)
SND_M_DllFunction(SndBool, SND_fn_bDoTransitionWithFadeSxdADPCM, 8, (long voice,tduRefRes new_res), (voice,new_res), FALSE)
SND_M_DllFunction(SndBool, SND_fn_bDoTransitionWithFadeSxdADPCM2, 12, (long voice, tduRefRes new_res, SndReal rDuration), (voice,new_res,rDuration), FALSE)
SND_M_DllProcedure(SND_fn_vStopTransitionSxdADPCM, 4, (long voice), (voice))
SND_M_DllProcedure(SND_fn_vPauseTransitionSxdADPCM, 4, (long voice), (voice))
SND_M_DllProcedure(SND_fn_vResumeTransitionSxdADPCM, 4, (long voice), (voice))
SND_M_DllProcedure(SND_fn_vReleaseDriverSxdADPCM, 0, (void), ())
SND_M_DllProcedure(SND_fn_vRestoreDriverSxdADPCM, 0, (void), ())
SND_M_DllProcedure(SND_fn_vGetTargetLabelSxdADPCM, 8, (char* name,int nb_char), (name,nb_char))
SND_M_DllProcedure(SND_fn_vSetupTargetSxdADPCM, 0, (), ())
SND_M_DllFunction(SndBool, SND_fn_bCanSetupTargetSxdADPCM, 0, (), (), FALSE)
SND_M_DllProcedure(SND_fn_vSynchroSxdADPCM, 0, (), ())
SND_M_DllFunction(long, SND_fn_lCreateNewBufferExSxdADPCM, 16, (SND_tdstFormat* pformat,SND_tdstCallback* pCallback,SoundParam* par,long user_id), (pformat,pCallback,par,user_id), -1)
SND_M_DllFunction(long, SND_fn_lGetPosBufferExSxdADPCM, 4, (long id_buffer), (id_buffer), 0)
SND_M_DllFunction(long, SND_fn_lPushBufferExSxdADPCM, 8, (long id_buffer,SND_tdstStackBuffer* pStack), (id_buffer,pStack), 0)
SND_M_DllProcedure(SND_fn_vSetDefaultRollOffSxdADPCM, 4, (RollOffParam* rolloff), (rolloff))
SND_M_DllFunction(SndBool, SND_fn_bCheckVersionResourceSxdADPCM, 4, (tdstBlockResourceDisk* disk), (disk), TRUE)
SND_M_DllFunction(long, SND_fn_lGetNbVoiceWishedSxdADPCM, 0, (), (), 0)
SND_M_DllFunction(SndBool, SND_fn_bIsLibActiveSxdADPCM, 0, (void), (), FALSE)
SND_M_DllFunction(SndBool, SND_fn_bSetResourceStaticVolumeSxdADPCM, 8, (tduRefRes res, unsigned char ucVolume), (res,ucVolume), FALSE)

char SND_pc_szBuildDate[] = "Oct 15 1999-20:17:03";
char SND_pc_szVersion[] = "Moteur sonore ACP - Version M5.8.4";

HMODULE SND_pc_hWavModule = NULL;
HMODULE SND_pc_hCdModule = NULL;
HMODULE SND_hMpegModule = NULL;
HMODULE SND_hDbgModule = NULL;
HMODULE SND_pc_hAdpcmModule = NULL;
HMODULE SND_hRecordModule = NULL;

static int SND_pc_s_bStaticEntryPointsLoaded = 0;
static int SND_pc_s_bMpegEntryPointsLoaded = 0;
int SND_pc_l26AFC = 0;
int SND_pc_l26B00 = 0;
static int SND_pc_s_bDbgEntryPointsLoaded = 0;
static int SND_pc_s_bAdpcmEntryPointsLoaded = 0;
static int SND_pc_s_bRecordEntryPointsLoaded = 0;

/* 0x10003FA0 */
FARPROC SND_pc_fn_pfnGetProcAddress(HMODULE hModule, LPCSTR szProcName)
{
    FARPROC pfnProc;
    char szModuleName[256];
    char szMessage[256];

    pfnProc = GetProcAddress(hModule, szProcName);
    if (pfnProc == NULL)
    {
        GetModuleFileName(hModule, szModuleName, 256);
        sprintf(szMessage, "Function cannot be loaded dynamically:\n%.20s\nfrom module %.80s", szProcName, szModuleName);

        SND_fn_vDisplayError(E_uwSndSystemError, szMessage);
    }
    return pfnProc;
}

void SND_DllInitEntryPoints_StaticFunctions(HMODULE hModule)
{
    if (!SND_pc_s_bStaticEntryPointsLoaded)
    {
        SND_M_DllLoad(hModule, SND_fn_vDisplayError)
        SND_M_DllLoad(hModule, SND_fn_vDisplayErrorEx)
        SND_M_DllLoad(hModule, SND_fn_iErreurInit)
        SND_M_DllLoad(hModule, _snd_assert)
        SND_M_DllLoad(hModule, snd_erreur_init)
        SND_M_DllLoad(hModule, SND_fn_vGetHModuleMpeg)
        SND_hMpegModule = SND_fn_vGetHModuleMpeg();
        SND_M_DllLoad(hModule, SND_fn_vGetHModuleWav)
        SND_pc_hWavModule = SND_fn_vGetHModuleWav();
        SND_M_DllLoad(hModule, SND_fn_vGetHModuleCd)
        SND_pc_hCdModule = SND_fn_vGetHModuleCd();
        SND_M_DllLoad(hModule, SND_fn_vGetHModuleAdpcm)
        SND_pc_hAdpcmModule = SND_fn_vGetHModuleAdpcm();
        SND_M_DllLoad(hModule, SND_fn_vGetHModuleRecord)
        SND_hRecordModule = SND_fn_vGetHModuleRecord();
        SND_M_DllLoad(hModule, SND_fn_vInitMallocSnd)
        SND_M_DllLoad(hModule, SND_fn_vDesInitMallocSnd)
        SND_M_DllLoad(hModule, SND_fn_pvMallocSnd)
        SND_M_DllLoad(hModule, SND_fn_pvReAllocSnd)
        SND_M_DllLoad(hModule, SND_fn_vFreeSnd)
        SND_M_DllLoad(hModule, SND_fn_pvMallocSndEx)
        SND_M_DllLoad(hModule, SND_fn_pvReAllocSndEx)
        SND_M_DllLoad(hModule, SND_fn_vFreeSndEx)
        SND_M_DllLoad(hModule, SND_fn_vResizeMemoryBlockSnd)
        SND_M_DllLoad(hModule, SND_fn_bLoadDataInMem)
        SND_M_DllLoad(hModule, SND_fn_bLoadDataInMemAndSwap)
        SND_M_DllLoad(hModule, SND_fn_bSetCurrentDirectory)
        SND_M_DllLoad(hModule, SND_fn_dwFileSndLength)
        SND_M_DllLoad(hModule, SND_fn_dwWriteFileSnd)
        SND_M_DllLoad(hModule, SND_fn_vAddDataDirectory)
        SND_M_DllLoad(hModule, SND_fn_vRemoveDataDirectory)
        SND_M_DllLoad(hModule, SND_fn_vPurgeDataDirectory)
        SND_M_DllLoad(hModule, SND_fn_bGetOneDataDirectory)
        SND_M_DllLoad(hModule, SND_fn_iGetNumberOfDataDirectory)
        SND_M_DllLoad(hModule, SND_fn_iGetIdDataDirectory)
        SND_M_DllLoad(hModule, SND_fn_bIsDataDirectory)
        SND_M_DllLoad(hModule, SND_fn_vResolveFileName)
        SND_M_DllLoad(hModule, SND_fn_dwReadFileSnd)
        SND_M_DllLoad(hModule, SND_fn_dwSeekFileSnd)
        SND_M_DllLoad(hModule, SND_fn_hOpenFileReadSnd)
        SND_M_DllLoad(hModule, SND_fn_vCloseFileSnd)
        SND_M_DllLoad(hModule, SND_fn_vSetSoftDirectory)
        SND_M_DllLoad(hModule, SND_fn_szGetSoftDirectory)
        SND_M_DllLoad(hModule, SND_fn_bCanSetOptions)
        SND_M_DllLoad(hModule, SND_fn_bGetOptions)
        SND_M_DllLoad(hModule, SND_fn_vSetOptions)
        SND_M_DllLoad(hModule, SND_fn_iGetNumberOfBanks)
        SND_M_DllLoad(hModule, SND_fn_iGetNumberOfEventInBank)
        SND_M_DllLoad(hModule, SND_fn_bStartLoadBanks)
        SND_M_DllLoad(hModule, SND_fn_bStartLoadBanks2)
        SND_M_DllLoad(hModule, SND_fn_bInitBank)
        SND_M_DllLoad(hModule, SND_fn_bInitBank2)
        SND_M_DllLoad(hModule, SND_fn_bEndLoadBanks)
        SND_M_DllLoad(hModule, SND_fn_pGetBinEvent)
        SND_M_DllLoad(hModule, SND_fn_pGetBinRes)
        SND_M_DllLoad(hModule, SND_fn_bLoadBinaryBank)
        SND_M_DllLoad(hModule, SND_fn_bUnLoadBinaryBank)
        SND_M_DllLoad(hModule, SND_fn_vInitBankSnd)
        SND_M_DllLoad(hModule, SND_fn_vDesInitBankSnd)
        SND_M_DllLoad(hModule, SND_fn_ucPositionToDolby)
        SND_M_DllLoad(hModule, SND_fn_ucDistanceToVolume)
        SND_M_DllLoad(hModule, SND_fn_ucDistanceToVolumeEx)
        SND_M_DllLoad(hModule, SND_fn_rDopplerPitch)
        SND_M_DllLoad(hModule, SND_fn_ucPositionToPan)
        SND_M_DllLoad(hModule, SND_fn_vSetDefaultRollOff)
        SND_M_DllLoad(hModule, SND_fn_vGetDefaultRollOff)
        SND_M_DllLoad(hModule, SND_fn_rMulRealRealSnd)
        SND_M_DllLoad(hModule, SND_fn_rMulRealRealQuickSnd)
        SND_M_DllLoad(hModule, SND_fn_rDivRealRealSnd)
        SND_M_DllLoad(hModule, SND_fn_rAbsRealSnd)
        SND_M_DllLoad(hModule, SND_fn_rNormeVectorSnd)
        SND_M_DllLoad(hModule, SND_fn_rPseudoNormeVectorSnd)
        SND_M_DllLoad(hModule, SND_fn_rScalaireVectorSnd)
        SND_M_DllLoad(hModule, SND_fn_rPseudoScalaireNormeVectorSnd)
        SND_M_DllLoad(hModule, SND_fn_vProduitVectorSnd)
        SND_M_DllLoad(hModule, SND_fn_vInitThreadSnd)
        SND_M_DllLoad(hModule, SND_fn_vDesInitThreadSnd)
        SND_M_DllLoad(hModule, SND_fn_vEnterCriticalSectionThreadSnd_)
        SND_M_DllLoad(hModule, SND_fn_vQuitCriticalSectionThreadSnd)
        SND_M_DllLoad(hModule, SND_fn_vMouchardThreadsnd)
        SND_M_DllLoad(hModule, SND_fn_vWaitForValueThreadSnd)
        SND_M_DllLoad(hModule, SND_fn_bTestSnd_Pentium)
        SND_M_DllLoad(hModule, SND_fn_bTestSnd_MMX)
        SND_M_DllLoad(hModule, SND_fn_bTestSnd_WinMM)
        SND_M_DllLoad(hModule, SND_fn_bTestSnd_WinNT)
        SND_M_DllLoad(hModule, SND_fn_vGetHModuleDbg)
        SND_hDbgModule = SND_fn_vGetHModuleDbg();
        SND_pc_s_bStaticEntryPointsLoaded = 1;
    }
}

void SND_DllInitEntryPoints_MpegFunctions(HMODULE hModule)
{
    if (!SND_pc_s_bMpegEntryPointsLoaded)
    {
        SND_M_DllLoad(hModule, SND_fn_bCanFreeDataMPEG)
        SND_M_DllLoad(hModule, SND_fn_lPlayMPEG)
        SND_M_DllLoad(hModule, SND_fn_vRemoveCallbackMPEG)
        SND_M_DllLoad(hModule, SND_fn_bSetParamMPEG)
        SND_M_DllLoad(hModule, SND_fn_bTestIsPlayingMPEG)
        SND_M_DllLoad(hModule, SND_fn_vStopMPEG)
        SND_M_DllLoad(hModule, SND_fn_vPauseMPEG)
        SND_M_DllLoad(hModule, SND_fn_vResumeMPEG)
        SND_M_DllLoad(hModule, SND_fn_iInitMPEG)
        SND_M_DllLoad(hModule, SND_fn_bTestInitMPEG)
        SND_M_DllLoad(hModule, SND_fn_vDesInitMPEG)
        SND_M_DllLoad(hModule, SND_fn_tdGetResForVoiceMPEG)
        SND_M_DllLoad(hModule, SND_fn_bLoadResScriptMPEG)
        SND_M_DllLoad(hModule, SND_fn_bLoadResBinaryMPEG)
        SND_M_DllLoad(hModule, SND_fn_vUnLoadResMPEG)
        SND_M_DllLoad(hModule, SND_fn_bIsResLoadedMPEG)
        SND_M_DllLoad(hModule, SND_fn_lPlayTransitionMPEG)
        SND_M_DllLoad(hModule, SND_fn_bSetParamTransitionMPEG)
        SND_M_DllLoad(hModule, SND_fn_bSetNextTransitionMPEG)
        SND_M_DllLoad(hModule, SND_fn_bDoTransitionWithFadeMPEG)
        SND_M_DllLoad(hModule, SND_fn_bDoTransitionWithFadeMPEG2)
        SND_M_DllLoad(hModule, SND_fn_vStopTransitionMPEG)
        SND_M_DllLoad(hModule, SND_fn_vPauseTransitionMPEG)
        SND_M_DllLoad(hModule, SND_fn_vResumeTransitionMPEG)
        SND_M_DllLoad(hModule, SND_fn_vReleaseDriverMPEG)
        SND_M_DllLoad(hModule, SND_fn_vRestoreDriverMPEG)
        SND_M_DllLoad(hModule, SND_fn_rGetPosMPEG)
        SND_M_DllLoad(hModule, SND_fn_rGetLengthMPEG)
        SND_M_DllLoad(hModule, SND_fn_vSynchroMPEG)
        SND_M_DllLoad(hModule, SND_fn_lCreateNewBufferExMPEG)
        SND_M_DllLoad(hModule, SND_fn_lGetPosBufferExMPEG)
        SND_M_DllLoad(hModule, SND_fn_lPushBufferExMPEG)
        SND_M_DllLoad(hModule, SND_fn_vSetDefaultRollOffMPEG)
        SND_M_DllLoad(hModule, SND_fn_bCheckVersionResourceMPEG)
        SND_M_DllLoad(hModule, SND_fn_bSetResourceStaticVolumeMPEG)
        SND_pc_s_bMpegEntryPointsLoaded = 1;
    }
}

void SND_DllInitEntryPoints_RecordFunctions(HMODULE hModule)
{
    if (!SND_pc_s_bRecordEntryPointsLoaded)
    {
        SND_M_DllLoad(hModule, SND_fn_bTestFormatRecord)
        SND_M_DllLoad(hModule, SND_fn_lCreateBufferRecord)
        SND_M_DllLoad(hModule, SND_fn_bIsBufferFullDuplexRecord)
        SND_M_DllLoad(hModule, SND_fn_bStartRecord)
        SND_M_DllLoad(hModule, SND_fn_vStopRecord)
        SND_M_DllLoad(hModule, SND_fn_bIsWorkingRecord)
        SND_M_DllLoad(hModule, SND_fn_lPushBufferToRecord)
        SND_M_DllLoad(hModule, SND_fn_bManageMsgForRecord)
        SND_M_DllLoad(hModule, SND_fn_iInitRecord)
        SND_M_DllLoad(hModule, SND_fn_vDesInitRecord)
        SND_M_DllLoad(hModule, SND_fn_ucGetInstantVuMeterRecord)
        SND_pc_s_bRecordEntryPointsLoaded = 1;
    }
}

void SND_DllInitEntryPoints_DbgFunctions(HMODULE hModule)
{
    if (!SND_pc_s_bDbgEntryPointsLoaded)
    {
        SND_M_DllLoad(hModule, SND_fn_vLaunchSupervisater)
        SND_M_DllLoad(hModule, SND_fn_vKillSupervisater)
        SND_M_DllLoad(hModule, SND_fn_vSetSupervisaterActive)
        SND_M_DllLoad(hModule, SND_fn_bIsSupervisaterActive)
        SND_M_DllLoad(hModule, dbgSND_fn_vSetProjectTitleForSuperVisater)
        SND_M_DllLoad(hModule, dbgSND_fn_vAddEventToObject)
        SND_M_DllLoad(hModule, dbgSND_fn_vKillEventToObject)
        SND_M_DllLoad(hModule, dbgSND_fn_vSetPosObject)
        SND_M_DllLoad(hModule, dbgSND_fn_vAddMicro)
        SND_M_DllLoad(hModule, dbgSND_fn_vKillMicro)
        SND_M_DllLoad(hModule, dbgSND_fn_vSetPosMicro)
        SND_M_DllLoad(hModule, dbgSND_fn_vAddHistoric)
        SND_M_DllLoad(hModule, dbgSND_fn_vKillVolumeLine)
        SND_M_DllLoad(hModule, dbgSND_fn_vAddVolumeLine)
        SND_M_DllLoad(hModule, dbgSND_fn_vSetVolumeLine)
        SND_M_DllLoad(hModule, dbgSND_fn_vLoadBank)
        SND_M_DllLoad(hModule, dbgSND_fn_vUnLoadBank)
        SND_M_DllLoad(hModule, dbgSND_fn_vAddVoiceSample)
        SND_M_DllLoad(hModule, dbgSND_fn_vSetParVoiceSample)
        SND_M_DllLoad(hModule, dbgSND_fn_vDelVoiceSample)
        SND_M_DllLoad(hModule, dbgSND_fn_vPauseVoiceSample)
        SND_M_DllLoad(hModule, dbgSND_fn_vResumeVoiceSample)
        SND_M_DllLoad(hModule, dbgSND_fn_vSetProcessTimeSample)
        SND_M_DllLoad(hModule, dbgSND_fn_pstGetSelectedResource)
        SND_M_DllLoad(hModule, dbgSND_fn_bDoesSupervisaterSelectResource)
        SND_M_DllLoad(hModule, dbgSND_fn_vRajListesSxdVirtual2)
        SND_M_DllLoad(hModule, dbgSND_fn_vInitListesSxdVirtual)
        SND_M_DllLoad(hModule, dbgSND_fn_vRefreshSupervisater)
        SND_pc_s_bDbgEntryPointsLoaded = 1;
    }
}

void SND_pc_DllInitEntryPoints_AdpcmFunctions(HMODULE hModule)
{
    if (!SND_pc_s_bAdpcmEntryPointsLoaded)
    {
        SND_M_DllLoad(hModule, SND_fn_bCanFreeDataADPCM)
        SND_M_DllLoad(hModule, SND_fn_lPlaySxdADPCM)
        SND_M_DllLoad(hModule, SND_fn_vRemoveCallbackSxdADPCM)
        SND_M_DllLoad(hModule, SND_fn_bSetParamSxdADPCM)
        SND_M_DllLoad(hModule, SND_fn_bTestIsPlayingSxdADPCM)
        SND_M_DllLoad(hModule, SND_fn_vStopSxdADPCM)
        SND_M_DllLoad(hModule, SND_fn_vPauseSxdADPCM)
        SND_M_DllLoad(hModule, SND_fn_vResumeSxdADPCM)
        SND_M_DllLoad(hModule, SND_fn_rGetPosSxdADPCM)
        SND_M_DllLoad(hModule, SND_fn_rGetLengthSxdADPCM)
        SND_M_DllLoad(hModule, SND_fn_vSetSoundVolumeSxdADPCM)
        SND_M_DllLoad(hModule, SND_fn_vSetStereoSxdADPCM)
        SND_M_DllLoad(hModule, SND_fn_bGetStereoSxdADPCM)
        SND_M_DllLoad(hModule, SND_fn_vSetReverseStereoSxdADPCM)
        SND_M_DllLoad(hModule, SND_fn_bGetReverseStereoSxdADPCM)
        SND_M_DllLoad(hModule, SND_fn_iInitSxdADPCM)
        SND_M_DllLoad(hModule, SND_fn_bTestInitSxdADPCM)
        SND_M_DllLoad(hModule, SND_fn_vDesInitSxdADPCM)
        SND_M_DllLoad(hModule, SND_fn_vSetEffectSxdADPCM)
        SND_M_DllLoad(hModule, SND_fn_vCreateMicroSxdADPCM)
        SND_M_DllLoad(hModule, SND_fn_vSetMicroParamSxdADPCM)
        SND_M_DllLoad(hModule, SND_fn_vDestroyMicroSxdADPCM)
        SND_M_DllLoad(hModule, SND_fn_lCreateNewBufferSxdADPCM)
        SND_M_DllLoad(hModule, SND_fn_vSetParamBufferSxdADPCM)
        SND_M_DllLoad(hModule, SND_fn_vDeleteBufferSxdADPCM)
        SND_M_DllLoad(hModule, SND_fn_vPauseBufferSxdADPCM)
        SND_M_DllLoad(hModule, SND_fn_vResumeBufferSxdADPCM)
        SND_M_DllLoad(hModule, SND_fn_pvGetPtrBufferSxdADPCM)
        SND_M_DllLoad(hModule, SND_fn_rGetPosBufferSxdADPCM)
        SND_M_DllLoad(hModule, SND_fn_bLoadResScriptSxdADPCM)
        SND_M_DllLoad(hModule, SND_fn_bLoadResBinarySxdADPCM)
        SND_M_DllLoad(hModule, SND_fn_bLoadResScriptAndConvertToWAVSxdADPCM)
        SND_M_DllLoad(hModule, SND_fn_bLoadResBinaryAndConvertToWAVSxdADPCM)
        SND_M_DllLoad(hModule, SND_fn_vUnLoadResSxdADPCM)
        SND_M_DllLoad(hModule, SND_fn_bIsResLoadedSxdADPCM)
        SND_M_DllLoad(hModule, SND_fn_lPlayTransitionSxdADPCM)
        SND_M_DllLoad(hModule, SND_fn_bSetParamTransitionSxdADPCM)
        SND_M_DllLoad(hModule, SND_fn_bSetNextTransitionSxdADPCM)
        SND_M_DllLoad(hModule, SND_fn_bDoTransitionWithFadeSxdADPCM)
        SND_M_DllLoad(hModule, SND_fn_bDoTransitionWithFadeSxdADPCM2)
        SND_M_DllLoad(hModule, SND_fn_vStopTransitionSxdADPCM)
        SND_M_DllLoad(hModule, SND_fn_vPauseTransitionSxdADPCM)
        SND_M_DllLoad(hModule, SND_fn_vResumeTransitionSxdADPCM)
        SND_M_DllLoad(hModule, SND_fn_vReleaseDriverSxdADPCM)
        SND_M_DllLoad(hModule, SND_fn_vRestoreDriverSxdADPCM)
        SND_M_DllLoad(hModule, SND_fn_vGetTargetLabelSxdADPCM)
        SND_M_DllLoad(hModule, SND_fn_vSetupTargetSxdADPCM)
        SND_M_DllLoad(hModule, SND_fn_bCanSetupTargetSxdADPCM)
        SND_M_DllLoad(hModule, SND_fn_vSynchroSxdADPCM)
        SND_M_DllLoad(hModule, SND_fn_lCreateNewBufferExSxdADPCM)
        SND_M_DllLoad(hModule, SND_fn_lGetPosBufferExSxdADPCM)
        SND_M_DllLoad(hModule, SND_fn_lPushBufferExSxdADPCM)
        SND_M_DllLoad(hModule, SND_fn_vSetDefaultRollOffSxdADPCM)
        SND_M_DllLoad(hModule, SND_fn_bCheckVersionResourceSxdADPCM)
        SND_M_DllLoad(hModule, SND_fn_bIsResourceLoopingSxdADPCM)
        SND_M_DllLoad(hModule, SND_fn_lGetNbVoiceWishedSxdADPCM)
        SND_M_DllLoad(hModule, SND_fn_bIsLibActiveSxdADPCM)
        SND_M_DllLoad(hModule, SND_fn_bSetResourceStaticVolumeSxdADPCM)
        SND_pc_s_bAdpcmEntryPointsLoaded = 1;
    }
}
