#ifndef _SND_LOAD_BINARY_H_
#define _SND_LOAD_BINARY_H_

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
    #pragma message("-> sndldbin : pointeurs de fonctions definis dans SNDDLL.H")
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

DLL_IMPEXP CPA_EXPORT SND_tdstBlockEvent *SND_fn_pGetBinEventFromEditorID(unsigned long _ulEdiId);

DLL_IMPEXP CPA_EXPORT void SND_CALL SND_fn_vDesInitBankSnd(void);

DLL_IMPEXP CPA_EXPORT SndBool SND_fn_bInitDataLoadSnd(const char *_szFileTitle);

#endif

#define BANK_ALL_LEVEL -1

typedef struct _EvtGrpDesc
{
	unsigned long ulId;
	unsigned long ulNbEvents;
	unsigned long ulOffset;
} EvtGrpDesc;

#ifndef NO_ACP_LDBIN
unsigned long SND_fn_ulGetEventGroupCount(void);
EvtGrpDesc *SND_fn_pstGetEventGroupData(void);
void SND_fn_vSetEventGroupData(EvtGrpDesc *_pData);
void SND_fn_vSetEventGroupCount(unsigned long _ulCount);

#endif

const char *SND_fn_pszGetSoundEventSectionNameFromPointer(SND_tdstBlockEvent* _pEvent);
unsigned long SND_fn_ulGetEventBinIdFromPointer(SND_tdstBlockEvent *_pEvent);
SND_tdxHandleToSoundEvent SND_fn_hGetSoundEventHandleFromEditorId(unsigned long _ulEvtId);
unsigned long SND_fn_ulGetEventBinIdFromSectionName(const char *szSectioName);

#define SND_INVALID_BINID 0xFFFFFFFF

#ifdef __cplusplus
}
#endif

#endif
