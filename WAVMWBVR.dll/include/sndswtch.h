#ifndef _SNDSWTCH_H_
#define _SNDSWTCH_H_

#ifdef __cplusplus
extern "C" {
#endif

#undef CPA_EXPORT
#if defined(CPA_WANTS_IMPORT)
#define CPA_EXPORT __declspec(dllimport)
#elif defined(CPA_WANTS_EXPORT)
#define CPA_EXPORT __declspec(dllexport)
#else
#define CPA_EXPORT
#endif

CPA_EXPORT void SND_fn_vInitSwitch(void);
CPA_EXPORT void SND_fn_vDesInitSwitch(void);
CPA_EXPORT long SND_fn_lPlaySwitch(tduRefRes num_res,SoundParam* par,long prio,SND_td_pfn_vSoundCallback fn_callback,long par_callback);
CPA_EXPORT SndBool SND_CALL SND_fn_bSetParamSwitch(long voice,SoundParam* par);
CPA_EXPORT SndBool SND_CALL SND_fn_bTestIsPlayingSwitch(long voice);
CPA_EXPORT void SND_CALL SND_fn_vStopSwitch(long voice);
CPA_EXPORT void SND_CALL SND_fn_vPauseSwitch(long voice);
CPA_EXPORT void SND_CALL SND_fn_vResumeSwitch(long voice);
CPA_EXPORT void SND_fn_vRemoveCallbackSwitch(long voice);
CPA_EXPORT SndBool SND_fn_bLoadResScriptSwitch(tdstBlockResourceDisk *disk,tdstBlockResourceMem *mem);
CPA_EXPORT SndBool SND_fn_bLoadResBinarySwitch(tdstBlockResourceDisk *disk,tdstBlockResourceMem *mem,char *pDataBloc);

CPA_EXPORT void SND_fn_vUnLoadResSwitch(tdstBlockResourceMem* mem);
CPA_EXPORT SndReal SND_CALL SND_fn_rGetPosSwitch(long voice);
CPA_EXPORT SndReal SND_CALL SND_fn_rGetLengthSwitch(long voice);
CPA_EXPORT tduRefRes SND_fn_tduGetLowestResourceSwitch(tduRefRes uHighRes,SoundParam* par,SndBool eval);
CPA_EXPORT SndBool SND_fn_bIsResourceSwitch(tduRefRes res);
CPA_EXPORT long SND_fn_lGetSwitchIndice(long objecttype,long objectid,tduRefRes uRes);

#ifdef __cplusplus
}
#endif

#endif
