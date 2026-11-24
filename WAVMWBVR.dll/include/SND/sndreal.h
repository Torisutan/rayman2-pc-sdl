#ifndef _SNDREAL_H_
#define	_SNDREAL_H_

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

#define M_MulIntRealSnd(i,r) ((i)*(r))
#define M_RealToDoubleSnd(r) (((double)r)/(0x10000))
#define M_DoubleToRealSnd(d) ((SndReal)((d)*0x10000))
#define M_DivIntIntRealSnd(a,b) ((SndReal)(((a)<<16)/(b)))
#define M_RealToIntSnd(r) ((r)>>16)
#define M_IntToRealSnd(r) ((r)<<16)
#define M_Fixed1616ToRealSnd(r) (r)

#define M_HighestIntAbsValueRealSnd (0x7FFF)
#define M_LowestDoubleAbsValueRealSnd (0.0000153)

#if defined(_DLL_COMPILATION_MODE) && !defined(SNDDLL_MODULE)
    #pragma message("-> sndreal : pointeurs de fonctions definis dans SNDDLL.H")
#else
DLL_IMPEXP CPA_EXPORT SndReal		SND_CALL SND_fn_rMulRealRealSnd(SndReal r1,SndReal r2);
DLL_IMPEXP CPA_EXPORT SndReal		SND_CALL SND_fn_rMulRealRealQuickSnd(SndReal r1,SndReal r2);
DLL_IMPEXP CPA_EXPORT SndReal		SND_CALL SND_fn_rDivRealRealSnd(SndReal r1,SndReal r2);
DLL_IMPEXP CPA_EXPORT SndReal		SND_CALL SND_fn_rSqrtRealSnd(SndReal r);
DLL_IMPEXP CPA_EXPORT SndReal		SND_CALL SND_fn_rAbsRealSnd(SndReal r);
DLL_IMPEXP CPA_EXPORT SndReal		SND_CALL SND_fn_rPseudoNormeRealSnd(SndReal r1,SndReal r2,SndReal r3);
DLL_IMPEXP CPA_EXPORT void			SND_CALL SND_fn_vSndRealTwinToSndReal(SndRealTwin,SndReal*,SndReal*);
DLL_IMPEXP CPA_EXPORT SndRealTwin	SND_CALL SND_fn_rtSndRealToSndRealTwin(SndReal,SndReal);
#endif

#ifdef __cplusplus
}
#endif

#endif
