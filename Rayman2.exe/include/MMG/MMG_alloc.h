#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>

extern uint8_t MMG_g_ucActiveMode;
extern uint8_t MMG_g_bAllocDisabled;

#include <windows.h>

extern CRITICAL_SECTION g_csMmg;

void *__cdecl fn_p_vDynAlloc(uint32_t ulSize, unsigned char);
void  __cdecl fn_vDynFree(void *pUser, unsigned char);

void *__cdecl fn_p_vStaAlloc(unsigned long ulSize, unsigned char ulHeapId);
void  __cdecl fn_vStaFree(void *pUser, uint32_t ulHeapId);
void *__cdecl fn_p_vGenAlloc(uint32_t ulSize, uint32_t ulHeapId);
void  __cdecl fn_vGenFree(void *pUser, uint32_t ulHeapId);
void  __cdecl Mmg_fn_v_SetModeAlloc(uint8_t ucModuleId, uint8_t ucNewMode, unsigned char ucChannel);

void  __cdecl Mmg_fn_vWriteOverflowCheckKey(void *pDst);
char  __cdecl Mmg_fn_b_cIsItEqualToOverflowCheckKey(const void *pGuard);

#define Mmg_fn_p_vAlloc(ulSize)        fn_p_vGenAlloc((ulSize), 0)
#define Mmg_fn_vFree(p_vMmgBlock)      fn_vGenFree((p_vMmgBlock), 0)

void  __cdecl Mmg_fn_v_InitMmg(unsigned char ucModuleId,
                               unsigned char ucMaxBlocksNb,
                               unsigned long ulMaxNbDynamicMalloc);

void  __cdecl Mmg_fn_vInitSpecificBlock(unsigned char  ucBlockId,
                                        unsigned char  ucModuleId,
                                        void          *p_stMyBlocksInfo,
                                        unsigned long  ulSize,
                                        unsigned long  ulMaxNbStaticMalloc,
                                        unsigned char  ucAlignment,
                                        unsigned char  ucMode);

void *__cdecl fn_p_vDynRealloc(void *p_vMemBlock, unsigned long ulSize, unsigned char);
void *__cdecl fn_p_vStaRealloc(void *p_vMemBlock, unsigned long ulSize, unsigned char ucChannel);
void *__cdecl fn_p_vGenRealloc(void *p_vMemBlock, unsigned long ulSize, unsigned char ucChannel);

#ifdef __cplusplus
}
#endif
