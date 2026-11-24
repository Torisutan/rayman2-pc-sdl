#ifndef _SNDMIDI_H_
#define _SNDMIDI_H_

#ifdef __cplusplus
extern "C" {
#endif

extern char SND_gszMidiMDXFakePartName[13];

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

#if defined(_DLL_COMPILATION_MODE) && !defined(DLLCD_MODULE)
    #pragma message("-> sndmidi : functions pointers defined in SNDDLL.H")
#else

DLL_IMPEXP SndBool  SND_CALL SND_fn_bLoadResScriptMidi(tdstBlockResourceDisk *disk,tdstBlockResourceMem *mem);
DLL_IMPEXP SndBool  SND_CALL SND_fn_bLoadResBinaryMidi(tdstBlockResourceDisk *disk,tdstBlockResourceMem *mem,char *pDataBloc);
DLL_IMPEXP SndBool  SND_CALL SND_fn_bIsResLoadedMidi(tdstBlockResourceMem* _pBRMem);
DLL_IMPEXP SndBool  SND_CALL SND_fn_bSetParamMidi(long voice,SoundParam *par);
DLL_IMPEXP SndBool  SND_CALL SND_fn_bTestInitMidi(void);
DLL_IMPEXP SndBool  SND_CALL SND_fn_bTestIsPlayingMidi(long voice);

DLL_IMPEXP int      SND_CALL SND_fn_iInitMidi(SND_tdstInitStruct *pInitStruct);
DLL_IMPEXP long	    SND_CALL SND_fn_lPlayMidi(tduRefRes res,SoundParam *par,long prio,SND_td_pfn_vSoundCallback fn_callback,long par_callback);
DLL_IMPEXP long     SND_CALL SND_fn_lPlayMidiWithFade(tduRefRes res,SoundParam *par,long prio,SND_td_pfn_vSoundCallback fn_callback,long par_callback,unsigned long ulFadeIn,unsigned long ulFadeOut);
DLL_IMPEXP long     SND_CALL SND_fn_lPlayMidiWithFadeIn(tduRefRes res,SoundParam *par,long prio,SND_td_pfn_vSoundCallback fn_callback,long par_callback,unsigned long ulFadeIn);

DLL_IMPEXP void     SND_CALL SND_fn_vDesInitMidi(void);

DLL_IMPEXP void		SND_CALL SND_fn_vPauseMidi(long voice);
DLL_IMPEXP void     SND_CALL SND_fn_vResumeMidi(long voice);
DLL_IMPEXP void     SND_CALL SND_fn_vRemoveCallbackMidi(long voice);

DLL_IMPEXP void     SND_CALL SND_fn_vStopMidi(long voice);
DLL_IMPEXP void     SND_CALL SND_fn_vStopMidiWithFadeOut(long voice,unsigned long ulFadeOut);
void SND_fn_vDoFadeMidi(long voice,SndReal rDuration );

DLL_IMPEXP void	    SND_CALL SND_fn_vUnLoadResMidi(tdstBlockResourceMem* mem);
DLL_IMPEXP SndReal	SND_CALL SND_fn_rGetPosMidi(long voice);
DLL_IMPEXP SndReal	SND_CALL SND_fn_rGetLengthMidi(long voice);

#ifndef NO_ACP_LDBIN
DLL_IMPEXP SndBool  SND_CALL SND_fn_bCanFreeDataMidi(void);
#endif
DLL_IMPEXP void		SND_CALL SND_fn_vReleaseDriverMidi(void);
DLL_IMPEXP void		SND_CALL SND_fn_vRestoreDriverMidi(void);

DLL_IMPEXP void			SND_CALL SND_fn_vSynchroMidi();

DLL_IMPEXP long		SND_CALL 	SND_fn_lPlayTransitionMidi(tduRefRes FirstRes,tduRefRes NextRes,SND_td_pfn_vSoundCallback transition_callback,long param_callback,SoundParam* par);
DLL_IMPEXP SndBool	SND_CALL SND_fn_bSetParamTransitionMidi(long voice,SoundParam* par);
DLL_IMPEXP SndBool	SND_CALL SND_fn_bSetNextTransitionMidi(long voice,tduRefRes new_res);
DLL_IMPEXP void			SND_CALL SND_fn_vStopTransitionMidi(long voice);
DLL_IMPEXP void			SND_CALL SND_fn_vResumeTransitionMidi(long voice);
DLL_IMPEXP void			SND_CALL SND_fn_vPauseTransitionMidi(long voice);
DLL_IMPEXP SndBool	SND_CALL  SND_fn_bDoTransitionWithFadeMidi(long voice,tduRefRes new_res);
DLL_IMPEXP SndBool	SND_CALL  SND_fn_bDoTransitionWithFadeMidi2( long voice, tduRefRes new_res, SndReal rDuration );

DLL_IMPEXP SndBool SND_CALL  SND_fn_bCheckVersionResourceMidi(tdstBlockResourceDisk* disk);
#endif

#ifdef __cplusplus
}
#endif

#endif
