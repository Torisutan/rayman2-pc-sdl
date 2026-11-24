#ifndef _SNDRND_H_
#define	_SNDRND_H_

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

CPA_EXPORT SndReal SND_fn_rGetRandomSnd(SndReal max);

#ifdef __cplusplus
}
#endif

#endif
