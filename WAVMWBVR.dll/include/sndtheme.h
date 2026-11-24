#ifndef _SNDTHEME_H_
#define _SNDTHEME_H_

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

CPA_EXPORT void SND_fn_vInitTheme(void);
CPA_EXPORT void SND_fn_vDesInitTheme(void);
CPA_EXPORT long SND_fn_lPlayTheme(long canal,tduRefRes num_res,SoundParam* par,long prio,SND_td_pfn_vSoundCallback fn_callback,long par_callback,SndBool* active);
CPA_EXPORT SndBool SND_CALL SND_fn_bSetParamTheme(long canal,SoundParam* par);
CPA_EXPORT SndBool SND_CALL SND_fn_bTestIsPlayingTheme(long canal);
CPA_EXPORT SndBool SND_CALL SND_fn_bAnyPlayingTheme();
CPA_EXPORT void SND_CALL SND_fn_vStopTheme(long canal);
CPA_EXPORT void SND_CALL SND_fn_vPauseTheme(long canal);
CPA_EXPORT void SND_CALL SND_fn_vResumeTheme(long canal);
CPA_EXPORT void SND_fn_vRemoveCallbackTheme(long canal);
CPA_EXPORT SndBool SND_fn_bLoadResScriptTheme(tdstBlockResourceDisk *disk,tdstBlockResourceMem *mem);
CPA_EXPORT SndBool SND_fn_bLoadResBinaryTheme(tdstBlockResourceDisk *disk,tdstBlockResourceMem *mem,char *pDataBloc);
CPA_EXPORT void SND_fn_vUnLoadResTheme(tdstBlockResourceMem* mem);
CPA_EXPORT SndReal SND_CALL SND_fn_rGetPosTheme(long voice);
CPA_EXPORT SndReal SND_CALL SND_fn_rGetLengthTheme(long voice);

CPA_EXPORT SndBool SND_fn_bGetThemePosition(tduRefRes *current_theme,long* id_part,long* id_loop);
CPA_EXPORT SndBool SND_fn_bGetAskedTheme(tduRefRes *asked_theme,SndBool *transiting);

#define THEMEPOSITION_BRIDGEIN -1
#define THEMEPOSITION_NOBRIDGE -2

#ifdef __cplusplus
}
#endif

#endif
