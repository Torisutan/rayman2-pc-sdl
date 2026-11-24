#ifndef _SNDRNDOM_H_
#define _SNDRNDOM_H_

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

CPA_EXPORT long SND_fn_lPlayRandom(tduRefRes num_res,SoundParam* par,long prio,SND_td_pfn_vSoundCallback fn_callback,long par_callback);
CPA_EXPORT SndBool SND_CALL SND_fn_bSetParamRandom(long voice,SoundParam* par);
CPA_EXPORT SndBool SND_CALL SND_fn_bTestIsPlayingRandom(long voice);
CPA_EXPORT void SND_CALL SND_fn_vStopRandom(long voice);
CPA_EXPORT void SND_CALL SND_fn_vPauseRandom(long voice);
CPA_EXPORT void SND_CALL SND_fn_vResumeRandom(long voice);
CPA_EXPORT void SND_fn_vRemoveCallbackRandom(long voice);
CPA_EXPORT SndBool SND_fn_bLoadResScriptRandom(tdstBlockResourceDisk *disk,tdstBlockResourceMem *mem);
CPA_EXPORT SndBool SND_fn_bLoadResBinaryRandom(tdstBlockResourceDisk *disk,tdstBlockResourceMem *mem,char *pDataBloc);
CPA_EXPORT void SND_fn_vUnLoadResRandom(tdstBlockResourceMem* mem);
CPA_EXPORT SndReal SND_CALL SND_fn_rGetPosRandom(long voice);
CPA_EXPORT SndReal SND_CALL SND_fn_rGetLengthRandom(long voice);
CPA_EXPORT tduRefRes SND_fn_tduGetLowestResourceRandom(tduRefRes uHighRes,SoundParam* par,SndBool eval);

#ifdef __cplusplus
}
#endif

#endif
