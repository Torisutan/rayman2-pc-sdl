#ifndef _SNDMEM_H_
#define _SNDMEM_H_

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

typedef enum e_ucSndStaticBlocks_{
	E_ucSndBlockMain,
	E_ucSndBlockMap,
#if defined(U64) || defined(NITRO)
	E_ucSnd64BlockMain,
	E_ucTmpSnd64BlockMain,
#endif
	E_ucSndMaxBlocksNb
} e_ucSndStaticBlocks;

#if defined(_DLL_COMPILATION_MODE) && !defined(SNDDLL_MODULE)
    #pragma message("-> sndmem : pointeurs de fonctions definis dans SNDDLL.H")
#else
    DLL_IMPEXP CPA_EXPORT void  SND_CALL SND_fn_vInitMallocSnd();
    DLL_IMPEXP CPA_EXPORT void  SND_CALL SND_fn_vDesInitMallocSnd();

    DLL_IMPEXP CPA_EXPORT void* SND_CALL SND_fn_pvMallocSnd(unsigned long nb_octets);
    DLL_IMPEXP CPA_EXPORT void* SND_CALL SND_fn_pvReAllocSnd(void* ptr,unsigned long nb_octets);
    DLL_IMPEXP CPA_EXPORT void  SND_CALL SND_fn_vFreeSnd(void* ptr);
    DLL_IMPEXP CPA_EXPORT void* SND_CALL SND_fn_pvMallocSndEx(unsigned char id,unsigned long nb_octets);
    DLL_IMPEXP CPA_EXPORT void* SND_CALL SND_fn_pvReAllocSndEx(unsigned char id,void* ptr,unsigned long nb_octets);
    DLL_IMPEXP CPA_EXPORT void  SND_CALL SND_fn_vFreeSndEx(unsigned char id,void* ptr);

    DLL_IMPEXP CPA_EXPORT void  SND_CALL SND_fn_vResizeMemoryBlockSnd(unsigned char id,unsigned long size);

	CPA_EXPORT void SND_PrintUsedStaticMemory(void);

#ifdef MEMORY_ACCES_MANAGEMENT_MODE
#define MEMORY_ACCES_MANAGEMENT_FLAG 0x80
#define MEMORY_ACCES_MANAGEMENT_BLOCK(i) (0x80+(i<<4))
#define SND_fn_vLockMemoryWriteSnd(a) SND_fn_vLockMemoryWriteSnd_(a)
#define SND_fn_vLockMemoryReadSnd(a) SND_fn_vLockMemoryReadSnd_(a)
#define SND_fn_vUnLockMemoryWriteSnd(a) SND_fn_vUnLockMemoryWriteSnd_(a)
#define SND_fn_vUnLockMemoryReadSnd(a) SND_fn_vUnLockMemoryReadSnd_(a)
    DLL_IMPEXP CPA_EXPORT void SND_CALL SND_fn_vLockMemoryWriteSnd_(void* ptr);
    DLL_IMPEXP CPA_EXPORT void SND_CALL SND_fn_vLockMemoryReadSnd_(void* ptr);
    DLL_IMPEXP CPA_EXPORT void SND_CALL SND_fn_vUnLockMemoryWriteSnd_(void* ptr);
    DLL_IMPEXP CPA_EXPORT void SND_CALL SND_fn_vUnLockMemoryReadSnd_(void* ptr);
#else
#define MEMORY_ACCES_MANAGEMENT_FLAG 0
#define MEMORY_ACCES_MANAGEMENT_BLOCK(i) 0
#define SND_fn_vLockMemoryWriteSnd(a)
#define SND_fn_vLockMemoryReadSnd(a)
#define SND_fn_vUnLockMemoryWriteSnd(a)
#define SND_fn_vUnLockMemoryReadSnd(a)
#endif
#endif

#ifdef __cplusplus
}
#endif

#endif
