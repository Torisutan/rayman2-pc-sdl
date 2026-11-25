#pragma once

#include "MMG/MMG_globals.h"

#ifdef __cplusplus
extern "C" {
#endif

void __cdecl Mmg_fn_vWhereIs(void           *p_vPtr,
                             unsigned short *p_uwModuleBlockId,
                             void          **pp_vBeginAdr);

unsigned char __cdecl Mmg_fn_ucGiveChannelId(unsigned long ulThreadId,
                                             unsigned char ucGroupId);

#if defined(USE_SDL)
/* The calling thread's id with USE_SDL (MMG/sdl_mmg.c) */
unsigned long __cdecl MMG_fn_ulGetCurrentThreadId(void);
#define MMG_pc_M_ulGetCurrentThreadId() MMG_fn_ulGetCurrentThreadId()
#else
#define MMG_pc_M_ulGetCurrentThreadId() GetCurrentThreadId()
#endif /* USE_SDL */

void __cdecl Mmg_fn_vFirstInitMmgModule(unsigned char ucMaxNbChannel);

void __cdecl Mmg_fn_vStopMmgModule(void);

void __cdecl Mmg_fn_vGiveInformationBlock(unsigned char  ucModuleId,
                                          unsigned char  ucBlockId,
                                          tdstBlockInfo *p_stMyBlockInfo);

void __cdecl Mmg_fn_vSaveInformationBlock(unsigned char  ucModuleId,
                                          unsigned char  ucBlockId,
                                          tdstBlockInfo *p_stMyBlockInfo);

#ifdef __cplusplus
}
#endif
