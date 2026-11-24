#ifndef _SND_LOAD_H_
#define _SND_LOAD_H_

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

#if defined(_DLL_COMPILATION_MODE) && !defined(SNDDLL_MODULE)
    #pragma message("-> sndload : pointeurs de fonctions definis dans SNDDLL.H")
#else

DLL_IMPEXP CPA_EXPORT int SND_CALL SND_fn_iGetNumberOfBanks(void);

DLL_IMPEXP CPA_EXPORT int SND_CALL SND_fn_iGetNumberOfEventInBank(int bank);

DLL_IMPEXP CPA_EXPORT SndBool SND_CALL SND_fn_bStartLoadBanks(void);
DLL_IMPEXP CPA_EXPORT SndBool SND_CALL SND_fn_bStartLoadBanks2(int level);

DLL_IMPEXP CPA_EXPORT SndBool SND_CALL SND_fn_bInitBank(int _iBankNum);
DLL_IMPEXP CPA_EXPORT SndBool SND_CALL SND_fn_bInitBank2(int _iBankNum,int level);

DLL_IMPEXP CPA_EXPORT SndBool SND_CALL SND_fn_bEndLoadBanks(void);

DLL_IMPEXP CPA_EXPORT SND_tdstBlockEvent* SND_CALL SND_fn_pGetBinEvent(unsigned long _ulBinId);
DLL_IMPEXP CPA_EXPORT tdstBlockResourceMem* SND_CALL SND_fn_pGetBinRes(unsigned long _ulBinId);

DLL_IMPEXP CPA_EXPORT SndBool SND_CALL SND_fn_bLoadBinaryBank(int _iBankNum);
DLL_IMPEXP CPA_EXPORT SndBool SND_CALL SND_fn_bUnLoadBinaryBank(int _iBankNum);

DLL_IMPEXP CPA_EXPORT void SND_CALL SND_fn_vInitBankSnd(char *czPathName);

DLL_IMPEXP CPA_EXPORT void SND_CALL SND_fn_vDesInitBankSnd(void);

DLL_IMPEXP CPA_EXPORT SndBool SND_fn_bInitDataLoadSnd(const char *_szFileTitle);

#endif

#define BANK_ALL_LEVEL -1
#define SND_INVALID_BINID 0xFFFFFFFF

CPA_EXPORT SndBool SND_fn_bIsResLoaded(tdstBlockResourceMem* _pBRMem);
CPA_EXPORT SndBool SND_fn_bIsEventLoaded(SND_tdstBlockEvent* _pEvent);

#ifdef SND_HYBRID_VERSION

unsigned long SND_fn_ulGetEventBinIdFromSectionName(const char *szSectionName);
const char* SND_fn_szGetEventSectionNameFromBinId(unsigned long _ulBinId);
unsigned long SND_fn_ulGetEventBinIdFromPointer(SND_tdstBlockEvent *_pEvent);
const char* SND_fn_szGetEventSectionNameFromPointer(SND_tdstBlockEvent *_pEvent);

unsigned long SND_fn_ulGetEventGroupId(const char *_szFileName);
const char *SND_fn_szGetEventGroupNameFromId(unsigned long _ulId);

unsigned char SND_fn_uc_GetSoundScriptMemLevel(void);
void	SND_fn_v_SetSoundScriptMemLevel(unsigned char);

BOOL SND_fn_bStartLogEventNames(const char *szFilePath,const char *szMapName);
BOOL SND_fn_bEndLogEventNames(void);

int SND_fn_iGetLoadedBankArray(int *paiBankListArray,int iArraySize);
void SND_fn_vLoadSoundMap(const char *_szMapName,SndBool _bSingleMap);

#endif

#ifdef SND_SCRIPT_VERSION

unsigned char SND_fn_uc_GetSoundScriptMemLevel(void);
void	SND_fn_v_SetSoundScriptMemLevel(unsigned char);

SCR_tde_Anl_ReturnValue SND_fn_eLoadResDisk(SCR_tdst_File_Description *p_fFile, char *szAction, char *szParams[], SCR_tde_Anl_Action cType);
SCR_tde_Anl_ReturnValue SND_fn_eLoadResM(SCR_tdst_File_Description *p_fFile, char *szAction, char *szParams[], SCR_tde_Anl_Action cType);
SCR_tde_Anl_ReturnValue SND_fn_eLoadEventM(SCR_tdst_File_Description *p_fFile, char *szAction, char *szParams[], SCR_tde_Anl_Action cType);
SCR_tde_Anl_ReturnValue SND_fn_eLoadTrackM(SCR_tdst_File_Description *p_fFile, char *szAction, char *szParams[], SCR_tde_Anl_Action cType);
SCR_tde_Anl_ReturnValue SND_fn_eLoadRefResM(SCR_tdst_File_Description *p_fFile, char *szAction, char *szParams[], SCR_tde_Anl_Action cType);
SCR_tde_Anl_ReturnValue SND_fn_eLoadPolyTrackM(SCR_tdst_File_Description *p_fFile, char *szAction, char *szParams[], SCR_tde_Anl_Action cType);
SCR_tde_Anl_ReturnValue SND_fn_eLoadPolyRefResM(SCR_tdst_File_Description *p_fFile, char *szAction, char *szParams[], SCR_tde_Anl_Action cType);
SCR_tde_Anl_ReturnValue SND_fn_eLoadSwitchListM(SCR_tdst_File_Description *p_fFile, char *szAction, char *szParams[], SCR_tde_Anl_Action cType);
SCR_tde_Anl_ReturnValue SND_fn_eLoadSwitchListElementM(SCR_tdst_File_Description *p_fFile, char *szAction, char *szParams[], SCR_tde_Anl_Action cType);

extern CPA_EXPORT tdstBlockResourceMem* SND_fn_pCreateResM(void);
extern CPA_EXPORT void SND_fn_vCopyResM(tdstBlockResourceMem*_pDest,tdstBlockResourceMem* _pSrc);
extern CPA_EXPORT void SND_fn_vDestroyResM(tdstBlockResourceMem* _pObj);

extern CPA_EXPORT tdstSndPolyResource* SND_fn_pCreatePolyResM(void);
extern CPA_EXPORT void SND_fn_vCopyPolyResM(tdstSndPolyResource*_pDest,tdstSndPolyResource* _pSrc);
extern CPA_EXPORT void SND_fn_vDestroyPolyResM(tdstSndPolyResource* _pObj);

extern CPA_EXPORT SND_tdstBlockEvent* SND_fn_pCreateEventM(void);
extern CPA_EXPORT void SND_fn_vCopyEventM(SND_tdstBlockEvent*_pDest,SND_tdstBlockEvent* _pSrc);
extern CPA_EXPORT void SND_fn_vDestroyEventM(SND_tdstBlockEvent* _pObj);

extern CPA_EXPORT tdstSndPolyEvent* SND_fn_pCreatePolyEventM(void);
extern CPA_EXPORT void SND_fn_vCopyPolyEventM(tdstSndPolyEvent*_pDest,tdstSndPolyEvent* _pSrc);
extern CPA_EXPORT void SND_fn_vDestroyPolyEventM(tdstSndPolyEvent* _pObj);

extern CPA_EXPORT tdstInfoTrackMem* SND_fn_pCreateTrackM(void);
extern CPA_EXPORT void SND_fn_vCopyTrackM(tdstInfoTrackMem*_pDest,tdstInfoTrackMem* _pSrc);
extern CPA_EXPORT void SND_fn_vDestroyTrackM(tdstInfoTrackMem* _pObj);

extern CPA_EXPORT tdstInfoTrackList* SND_fn_pCreatePolyTrackM(void);
extern CPA_EXPORT void SND_fn_vCopyPolyTrackM(tdstInfoTrackList*_pDest,tdstInfoTrackList* _pSrc);
extern CPA_EXPORT void SND_fn_vDestroyPolyTrackM(tdstInfoTrackList* _pObj);

extern CPA_EXPORT tdstSwitchElement* SND_fn_pCreateSwitchElementM(void);
extern CPA_EXPORT void SND_fn_vCopySwitchElementM(tdstSwitchElement*_pDest,tdstSwitchElement* _pSrc);
extern CPA_EXPORT void SND_fn_vDestroySwitchElementM(tdstSwitchElement* _pObj);
extern CPA_EXPORT tdstTypeSwitchMem* SND_fn_pCreateSwitchListM(void);
extern CPA_EXPORT void SND_fn_vCopySwitchListM(tdstTypeSwitchMem*_pDest,tdstTypeSwitchMem* _pSrc);
extern CPA_EXPORT void SND_fn_vDestroySwitchListM(tdstTypeSwitchMem* _pObj);

extern CPA_EXPORT tdstRandomElement* SND_fn_pCreateRandomElementM(void);
extern CPA_EXPORT void SND_fn_vCopyRandomElementM(tdstRandomElement*_pDest,tdstRandomElement* _pSrc);
extern CPA_EXPORT void SND_fn_vDestroyRandomElementM(tdstRandomElement* _pObj);
extern CPA_EXPORT tdstTypeRandomMem* SND_fn_pCreateRandomListM(void);
extern CPA_EXPORT void SND_fn_vCopyRandomListM(tdstTypeRandomMem*_pDest,tdstTypeRandomMem* _pSrc);
extern CPA_EXPORT void SND_fn_vDestroyRandomListM(tdstTypeRandomMem* _pObj);

extern CPA_EXPORT tdstRangeSplitListMem* SND_fn_pCreatePolyRangeM(void);
extern CPA_EXPORT void SND_fn_vCopyPolyRangeM(tdstRangeSplitListMem*_pDest,tdstRangeSplitListMem* _pSrc);
extern CPA_EXPORT void SND_fn_vDestroyPolyRangeM(tdstRangeSplitListMem* _pObj);

extern CPA_EXPORT tdstRangeSplitMem* SND_fn_pCreateRangeM(void);
extern CPA_EXPORT void SND_fn_vCopyRangeM(tdstRangeSplitMem*_pDest,tdstRangeSplitMem* _pSrc);
extern CPA_EXPORT void SND_fn_vDestroyRangeM(tdstRangeSplitMem* _pObj);

extern CPA_EXPORT tdstThemeBridge* SND_fn_pCreateThemeBridge(void);
extern CPA_EXPORT void SND_fn_vCopyThemeBridge(tdstThemeBridge*_pDest,tdstThemeBridge* _pSrc);
extern CPA_EXPORT void SND_fn_vDestroyThemeBridge(tdstThemeBridge* _pObj);
extern CPA_EXPORT tdstThemePart* SND_fn_pCreateThemePart(void);
extern CPA_EXPORT void SND_fn_vCopyThemePart(tdstThemePart*_pDest,tdstThemePart* _pSrc);
extern CPA_EXPORT void SND_fn_vDestroyThemePart(tdstThemePart* _pObj);
extern CPA_EXPORT tdstTypeThemeMem* SND_fn_pCreateTheme(void);
extern CPA_EXPORT void SND_fn_vCopyTheme(tdstTypeThemeMem*_pDest,tdstTypeThemeMem* _pSrc);
extern CPA_EXPORT void SND_fn_vDestroyTheme(tdstTypeThemeMem* _pObj);

#endif

#ifdef __cplusplus
}
#endif

#endif
