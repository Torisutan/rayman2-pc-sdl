#ifndef _SNDSPACE_H_
#define _SNDSPACE_H_

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
    #pragma message("-> sndspace : functions pointers defined in SNDDLL.H")
#else
DLL_IMPEXP unsigned char SND_CALL SND_fn_ucDistanceToVolume(SndReal dist);
DLL_IMPEXP unsigned char SND_CALL SND_fn_ucDistanceToVolumeEx(SndReal dist,RollOffParam* rolloff);
DLL_IMPEXP SndReal		 SND_CALL SND_fn_rDopplerPitch(SndVector *src_pos,SndVector *src_vit,SndVector *mic_pos,SndVector *mic_vit);
DLL_IMPEXP void			 SND_CALL SND_fn_vSetDopplerFactor(SndReal factor);
DLL_IMPEXP unsigned char SND_CALL SND_fn_ucPositionToPan(SndVector *src,SndVector *mic,SndVector *mic_nor,SndVector *mic_tan);
DLL_IMPEXP unsigned char SND_CALL SND_fn_ucPositionToDolby(SndVector *src,SndVector *mic,SndVector *mic_nor);

DLL_IMPEXP void SND_CALL SND_fn_vSetDefaultRollOff(RollOffParam* rolloff);
DLL_IMPEXP void SND_CALL SND_fn_vGetDefaultRollOff(RollOffParam* rolloff);
#endif

#ifdef __cplusplus
}
#endif

#endif
