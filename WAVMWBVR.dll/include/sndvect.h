#ifndef _SNDVECT_H_
#define _SNDVECT_H_

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
    #pragma message("-> sndvect : functions pointers defined in SNDDLL.H")
#else
DLL_IMPEXP SndReal SND_CALL SND_fn_rNormeVectorSnd(SndVector *v);
DLL_IMPEXP SndReal SND_CALL SND_fn_rPseudoNormeVectorSnd(SndVector *v);
DLL_IMPEXP SndReal SND_CALL SND_fn_rScalaireVectorSnd(SndVector *v1,SndVector *v2);
DLL_IMPEXP SndReal SND_CALL SND_fn_rPseudoScalaireNormeVectorSnd(SndVector *v1,SndVector *v2);
DLL_IMPEXP void SND_CALL SND_fn_vProduitVectorSnd(SndVector *v1,SndVector *v2,SndVector *v);
#endif

#ifdef __cplusplus
}
#endif

#endif
