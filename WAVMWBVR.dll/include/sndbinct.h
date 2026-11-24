#ifndef _SND_BINARY_CONVERT_H_
#define _SND_BINARY_CONVERT_H_

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

void SND_fn_vInitBinaryConversionInterface(const char *szFileTitle);
void SND_fn_vDesInitBinaryConversionInterface(void);

#ifdef __cplusplus
}
#endif

#endif
