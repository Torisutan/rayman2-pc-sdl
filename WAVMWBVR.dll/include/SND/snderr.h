#ifndef _SNDERR_H_
#define _SNDERR_H_

#ifndef NO_ACP_ERROR
#define SNDERR_USE_ACP
#endif

#ifdef __cplusplus
extern "C" {
#endif


#define SNDERR_DISPLAY_ALLWAYS 1
#define SNDERR_DISPLAY_ONLY_ONCE 2
#define SNDERR_DISPLAY_IF_WANTED 3


#define __FATAL_ERR_SND__
#define __WARNING1_ERR_SND__
#define __WARNING2_ERR_SND__
#define __WARNING3_ERR_SND__





typedef enum e_uwSndErrNumber_{
	E_uwSndFatalErr,
#ifdef __FATAL_ERR_SND__

	E_uwSndMemoryBlockError,
	E_uwSndThreadDeadLockError,
	E_uwSndThreadReentranceDetectedError,
    E_uwSndInitNotDoneError,
    E_uwSndBadHostConfiguration,
	E_uwSndFatalError4,
#endif
	E_uwSndStartOfWarning,
#ifdef __WARNING1_ERR_SND__

	E_uwSndMemoryAllocError,
	E_uwSndMemoryReAllocError,
	E_uwSndCannotOpenFile,
	E_uwSndErrorReadingFile,
	E_uwSndINIFileNotFound,
#endif
#ifdef __WARNING2_ERR_SND__

	E_uwSndScriptLoadError,

	E_uwSndDLLCannotFindFunction,
	E_uwSndDLLCannotLoadDLL,
	E_uwSndEndOfWarningLevel2,
#endif
#ifdef __WARNING3_ERR_SND__

	E_uwSndResourceMissing,
	E_uwSndTooManyEvents,
	E_uwSndInvalidTypeOfEvent,
	E_uwSndTooManyVoices,
	E_uwSndInvalidTypeOfResource,
	E_uwSndTooManySequences,
	E_uwSndTooManySamples,
	E_uwSndTooManySwitchs,
	E_uwSndTooManySplits,
	E_uwSndInvalidDynParamForResource,
	E_uwSndInvalidStaticParamForResource,
	E_uwSndThreadMultiThreadDetectedError,
	E_uwSndSystemError,
    E_uwSndWaveCreateThread,

  E_uwSndCDLoadTrack,
  E_uwSndCDStatus,
  E_uwSndCDSet,
  E_uwSndCDPlay,
  E_uwSndCDStop,
  E_uwSndCDNotifyFailure,
  E_uwSndCDNotifyAborted,
  E_uwSndCDNotifySuperseded,
  E_uwSndCDCreateThread,
  E_uwSndCDClose,
  E_uwSndCDFade,
  E_uwSndCDSeek,
  E_uwSndCDNbTracks,
  E_uwSndCDRegisterWndClass,

  E_uwSndMIDILoadTrack,
  E_uwSndMIDIUnloadTrack,
  E_uwSndMIDIStatus,
  E_uwSndMIDISet,
  E_uwSndMIDIPlay,
  E_uwSndMIDIStop,
  E_uwSndMIDINotifyFailure,
  E_uwSndMIDINotifyAborted,
  E_uwSndMIDINotifySuperseded,
  E_uwSndMIDICreateThread,
  E_uwSndMIDIOpen,
  E_uwSndMIDIClose,
  E_uwSndMIDIFade,
  E_uwSndMIDISeek,
  E_uwSndMIDINbTracks,
  E_uwSndMIDIAllTracksUsed,

  E_uwSndMixerNoMixer,
  E_uwSndMixerNoId,
  E_uwSndMixerOpen,
  E_uwSndMixerCapacities,
  E_uwSndMixerDesinit,
  E_uwSndEventNotInABank,
  E_uwSndEventUsesUnloadedBank,

	E_uwSndEndOfWarningLevel3,
#endif
	E_uwSndErrNumber
} e_uwSndErrNumber;








#ifndef RETAIL

	#if defined(DEBUG) || !defined(NDEBUG)
		#define DEBUG_SON
	#endif

#endif


void SND_fn_vUpdateLastError(unsigned long ulChannelId,
							 unsigned short uwErrNum,
							 long lDebugData,
							 unsigned char ucOpenInfoWindow,
							 unsigned char ucStopForDebug,
                             char* message);

#define SND_M_UpdateLastError(ulChannelId,uwErrNum,lDebugData,ucOpenInfoWindow,ucStopForDebug,message)	\
	SND_fn_vUpdateLastError(ulChannelId, uwErrNum, lDebugData, ucOpenInfoWindow, ucStopForDebug, message)

#ifdef SNDERR_USE_ACP
#define SND_M_ClearLastError(ulChannelId)	\
	Erm_M_ClearLastError(ulChannelId)
#define SND_M_CheckError(ulChannelId)	\
	Erm_M_uwCheckError(Snd,ulChannelId)

#else

void SND_fn_vClearLastError(unsigned long ulChannelId);
unsigned short SND_fn_vCheckError(unsigned long ulChannelId);
#define SND_M_ClearLastError(ulChannelId)	\
	SND_fn_vClearLastError(ulChannelId)
#define SND_M_CheckError(ulChannelId)	\
	SND_fn_vCheckError(ulChannelId)

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
    #pragma message("-> snderr : pointeurs de fonctions definis dans SNDDLL.H")
#else
	DLL_IMPEXP CPA_EXPORT void SND_CALL SND_fn_vInitErrorSnd(SND_tdstInitStruct *pInitStruct);
	DLL_IMPEXP CPA_EXPORT void SND_CALL SND_fn_vDesInitErrorSnd();
    DLL_IMPEXP CPA_EXPORT void SND_CALL SND_fn_vDisplayError(unsigned short uwErrNum,char * message);
    DLL_IMPEXP CPA_EXPORT int SND_CALL snd_erreur_init(int num_err);

    DLL_IMPEXP CPA_EXPORT int SND_CALL SND_fn_iErreurInit(int idErr);
    DLL_IMPEXP CPA_EXPORT void SND_CALL _snd_assert(char* test,char* file,int line);
	#ifdef DEBUG_SON
		#define snd_assert(fct) if (!(fct)) _snd_assert(#fct, __FILE__, __LINE__)
	#else
		#define snd_assert(fct)
	#endif

    DLL_IMPEXP CPA_EXPORT void SND_CALL SND_fn_vDisplayErrorEx(unsigned short uwErrNum,char * message,int flags);

#endif


#ifdef SND_DISPLAY_ERROR_MESSAGE
#define SND_M_DisplayError(err_num,message) SND_fn_vDisplayError(err_num,message)
#define SND_M_DisplayErrorEx(err_num,message,flags) SND_fn_vDisplayErrorEx(err_num,message,flags)
#else
#define SND_M_DisplayError(err_num,message) SND_fn_vDisplayError(err_num,"")
#define SND_M_DisplayErrorEx(err_num,message,flags) SND_fn_vDisplayErrorEx(err_num,"",flags)
#endif


#define SND_ERREUR_INIT_BADSNDDRIVER 30
#define SND_ERREUR_INIT_SNDDRIVERBUSY 31
#define SND_ERREUR_INIT_BADCDDRIVER 32
#define SND_ERREUR_INIT_CDDRIVERBUSY 33




#define INIT_RETRY 2
#define INIT_IGNORE 3
#define INIT_FAILED 4
#define INIT_PLAY	5
#define INIT_PLAYASIF	6




#ifdef __cplusplus
}
#endif

#endif
