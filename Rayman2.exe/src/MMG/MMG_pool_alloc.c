#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <string.h>
#include <stdint.h>

#include "MMG/MMG_alloc.h"
#include "MMG/MMG_globals.h"
#include "ERM/CpaError.h"
#include "MMG/CpaMmg.h"
#include "MMG/PrivMmg.h"

/* 0x516148 */
uint8_t MMG_g_ucActiveMode;
uint8_t MMG_g_bAllocDisabled;
TypeCriticalSection g_csMmg;   /* 0x50B0C0 */

#define MMG_POOL_GUARD_DWORD 0xe123456eu

#define MMG_g_ucErrorContext  MMG_g_ucErmChannelId   /* 0x004A11C4 */

#define MMG_g_pHeapRouteTable (g_p_d_stCurrentModeInfo)

/* 0x516148 */
typedef struct MMG_tdstPoolMeta {
    uintptr_t ulConfig;
    uintptr_t ulEnd;
    uintptr_t ulFreeHead;
    uintptr_t ulCurrentEnd;
} MMG_tdstPoolMeta;

void __cdecl Mmg_fn_vWriteOverflowCheckKey(void *pDst)
{
    char *p_cTarget = (char *)pDst;
    char *p_cScan;
    char *p_cSource;
    unsigned long ulOverflowCheckKey = MMG_POOL_GUARD_DWORD;

    for (p_cScan = p_cTarget, p_cSource = (char *)(&ulOverflowCheckKey);
         p_cScan < p_cTarget + sizeof(uint32_t);
         p_cScan++, p_cSource++) {
        *p_cScan = *p_cSource;
    }
}

char __cdecl Mmg_fn_b_cIsItEqualToOverflowCheckKey(const void *pGuard)
{
    char *p_cTarget = (char *)pGuard;
    char *p_cScan;
    char *p_cSource;
    unsigned long ulOverflowCheckKey = MMG_POOL_GUARD_DWORD;

    for (p_cScan = p_cTarget, p_cSource = (char *)(&ulOverflowCheckKey);
         p_cScan < p_cTarget + sizeof(uint32_t);
         p_cScan++, p_cSource++) {
        if (*p_cScan != *p_cSource) break;
    }
    return (p_cScan == p_cTarget + sizeof(uint32_t));
}

void *__cdecl fn_p_vDynAlloc(uint32_t ulSize, unsigned char ucChannel)
{
    uint32_t ulDwordCount;
    uint32_t *pAlloc;
    uint32_t *pAligned;

    if (ulSize == 0) {
        return NULL;
    }

    Mmg_MEnterCriticalSection();

    if ((ulSize & 3u) != 0) {
        ulSize = (ulSize & 0xfffffffcu) + 4u;
    }

    ulDwordCount = (ulSize >> 2) + 1u;
    pAlloc = (uint32_t *)CPA_MALLOC(ulDwordCount * 4u);
    pAligned = pAlloc;
    if (((uintptr_t)pAlloc & 3u) != 0) {
        pAligned = (uint32_t *)((((uintptr_t)pAlloc) & ~(uintptr_t)3u) + 4u);
    }

    if (pAlloc != pAligned) {
        ERM_fn_vUpdateLastError(0x1d, MMG_g_ucErrorContext, 0, 3, -1, 0xff, 0xff, 0);
    }

    if (pAlloc == NULL) {
        ERM_fn_vUpdateLastError(0x1d, MMG_g_ucErrorContext, 0, 0x20, -1, 0xff, 0xff, 0);
        Mmg_MLeaveCriticalSection();
        return NULL;
    }

    *pAlloc = ulDwordCount;
    Mmg_MLeaveCriticalSection();
    return pAlloc + 1;
}

void __cdecl fn_vDynFree(void *pUser, unsigned char ucChannel)
{
    Mmg_MEnterCriticalSection();
    CPA_FREE((HGLOBAL)((uint8_t *)pUser - 4));
    Mmg_MLeaveCriticalSection();
}

#ifndef M_CheckPointer
#define M_CheckPointer(a)
#endif

/* 0x00442850 */
void *__cdecl fn_p_vStaAlloc (unsigned long ulSize, unsigned char ucChannel){

        tdstBlockInfoPriv * p_stMyBlockInfo = (tdstBlockInfoPriv *) ((tdstCurrentModeInfo *) g_p_d_stCurrentModeInfo)[ucChannel].p_vTdstBlockInfoOrTdstDynInfo;

        if (ulSize == 0){
                return NULL;
        }

        Mmg_MEnterCriticalSection();

        M_CheckPointer(p_stMyBlockInfo);

        if (p_stMyBlockInfo->p_cFirstFree != C_FirstFreeValueForBlockWithoutFree){
                unitAlloc *p_uaMyFreeSpace;
                unitAlloc *p_uaBeforeMyFreeSpace;
                unsigned char b_ucFound = C_ucFalse;

                Mmg_M_BigBoundSize (ulSize, C_uwAllocSize);
                ulSize >>= C_uwShiftAllocSize;
                ulSize += (C_ucMallocHeaderSize + C_ucMallocFooterSize);

                p_uaMyFreeSpace = (unitAlloc *) p_stMyBlockInfo->p_cFirstFree;
                p_uaBeforeMyFreeSpace = p_uaMyFreeSpace;

                while ((p_uaMyFreeSpace != (unitAlloc *) 0 ) && !b_ucFound){
                          if ((*p_uaMyFreeSpace)>=(unitAlloc) ulSize){
                                  b_ucFound = C_ucTrue;
                          }
                          else {
                                p_uaBeforeMyFreeSpace = p_uaMyFreeSpace;
                                p_uaMyFreeSpace = (unitAlloc *) *(p_uaMyFreeSpace+1);
                          }
                }
                if (b_ucFound){

                        *p_uaMyFreeSpace -= ((unitAlloc) ulSize);

                        if (*p_uaMyFreeSpace < (unitAlloc) C_ucFreeHeaderSize)
                        {

                                ulSize += (*p_uaMyFreeSpace);

                                if (p_uaMyFreeSpace == p_uaBeforeMyFreeSpace){
                                        p_stMyBlockInfo->p_cFirstFree = (char *) *(p_uaMyFreeSpace + 1);
                                }
                                else {

                                        *(p_uaBeforeMyFreeSpace + 1) = *(p_uaMyFreeSpace + 1);
                                }
                        }
                        else {
                                *(p_uaMyFreeSpace+ulSize)= *p_uaMyFreeSpace;

                                *(p_uaMyFreeSpace+ulSize+1)= *(p_uaMyFreeSpace+1);

                                if (p_uaMyFreeSpace == (unitAlloc *) p_stMyBlockInfo->p_cFirstFree)
                                {
                                        p_stMyBlockInfo->p_cFirstFree = (char *) (p_uaMyFreeSpace+ulSize);
                                }
                                else {

                                        *(p_uaBeforeMyFreeSpace + 1) = (unitAlloc) (p_uaMyFreeSpace + ulSize);
                                }
                        }

                        *p_uaMyFreeSpace = (unitAlloc)ulSize;

                        if ((char *)(p_uaMyFreeSpace+ *p_uaMyFreeSpace) -1 >=p_stMyBlockInfo->p_cMaxMem)
                        {

                                p_stMyBlockInfo->p_cMaxMem=(char *)(p_uaMyFreeSpace+ *p_uaMyFreeSpace)-1;
                        }

                        Mmg_MLeaveCriticalSection();
                        return ( (void *) (p_uaMyFreeSpace + C_ucMallocHeaderSize) );
                }
                else{

                        ERM_fn_vUpdateLastError(0x1d, MMG_g_ucErmChannelId, 0, 0x23, -1, 0xff, 0xff, (char *)0);
                        Mmg_MLeaveCriticalSection();
                        return (NULL);
                }

        }
        else{
                unsigned long ulI;
                tdstInformationHeaderOfBolckWithoutFree *p_stInfoHeaderBolck =
                        (tdstInformationHeaderOfBolckWithoutFree *) p_stMyBlockInfo->p_cBeginBlock;

                Mmg_M_BigBoundSize (ulSize, p_stInfoHeaderBolck->ucAlignment);

                ulI = p_stMyBlockInfo->p_cEndBlock-p_stMyBlockInfo->p_cMaxMem;
                if (ulSize > ulI) {

                        ERM_fn_vUpdateLastError(0x1d, MMG_g_ucErmChannelId, 0, 0x23, -1, 0xff, 0xff, (char *)0);
                        Mmg_MLeaveCriticalSection();
                        return (NULL);
                }

                if ((p_stInfoHeaderBolck->ucMode & C_Check_OverflowFlag) &&
                        (ulI >= sizeof (C_TypeOfOverflowCheckKeyForBlockWithoutFree)) &&
                        (!Mmg_fn_b_cIsItEqualToOverflowCheckKey(p_stMyBlockInfo->p_cMaxMem+1)) ) {

                        ERM_fn_vUpdateLastError(0x1d, MMG_g_ucErmChannelId, 0, 0x1b, -1, 0xff, 0xff, (char *)0);
                }

                if (p_stInfoHeaderBolck->ucMode & C_FillTheBlockWithCleanKeyFlag){
                        char * p_cTmp;
                        for (p_cTmp = p_stMyBlockInfo->p_cMaxMem+1;
                                        (p_cTmp <= p_stMyBlockInfo->p_cMaxMem+sizeof(C_TypeOfOverflowCheckKeyForBlockWithoutFree)) &&
                                                (p_cTmp <= p_stMyBlockInfo->p_cEndBlock);
                                        p_cTmp++){
                                *p_cTmp = (char)C_uaCleanKeyForBlockWithoutFree;
                        }
                }

                if (p_stInfoHeaderBolck->ucMode & C_Check_AlignementFlag){
                        ulI = ulSize;
                        Mmg_M_BigBoundSize (ulSize, p_stInfoHeaderBolck->ucAlignment);

                        if (ulI != ulSize) {

                                ERM_fn_vUpdateLastError(0x1d, MMG_g_ucErmChannelId, 0, 0x1a, -1, 0xff, 0xff, (char *)0);
                        }
                }

                ulI = (unsigned long) p_stMyBlockInfo->p_cMaxMem + 1;

                p_stMyBlockInfo->p_cMaxMem += ulSize;

                if ((p_stMyBlockInfo->p_cEndBlock - p_stMyBlockInfo->p_cMaxMem) >= sizeof(C_TypeOfOverflowCheckKeyForBlockWithoutFree)){
                        Mmg_fn_vWriteOverflowCheckKey(p_stMyBlockInfo->p_cMaxMem+1);
                }
                Mmg_MLeaveCriticalSection();
                return ((void*) ulI);
        }

}

/* 0x00442A70 */
void __cdecl fn_vStaFree (void *p_vMemBlock, uint32_t ulChannel){
        tdstBlockInfoPriv * p_stMyBlockInfo = (tdstBlockInfoPriv *) ((tdstCurrentModeInfo *) g_p_d_stCurrentModeInfo)[(unsigned char)ulChannel].p_vTdstBlockInfoOrTdstDynInfo;
        unitAlloc * p_uaNextFree;
        unitAlloc * p_uaSizeMalloc;
        unitAlloc *p_uaAfterFreeSpace;
        unitAlloc *p_uaBeforeFreeSpace;

        if (p_stMyBlockInfo->p_cFirstFree == C_FirstFreeValueForBlockWithoutFree){

                ERM_fn_vUpdateLastError(0x1d, MMG_g_ucErmChannelId, 0, 0x1c, -1, 0xff, 0xff, (char *)0);
        }

        Mmg_MEnterCriticalSection();

        p_uaBeforeFreeSpace = p_uaAfterFreeSpace = (unitAlloc *) p_stMyBlockInfo->p_cFirstFree;
        while ((p_uaAfterFreeSpace != (unitAlloc *) 0 ) && (p_uaAfterFreeSpace < (unitAlloc *) p_vMemBlock) ){
                p_uaBeforeFreeSpace = p_uaAfterFreeSpace;
                p_uaAfterFreeSpace = (unitAlloc *) *(p_uaAfterFreeSpace+1);
        }

        p_uaNextFree = ((unitAlloc *) p_vMemBlock) - C_ucMallocHeaderSize + 1;
        p_uaSizeMalloc = ((unitAlloc *) p_vMemBlock) - C_ucMallocHeaderSize;

        if (p_uaBeforeFreeSpace == p_uaAfterFreeSpace) {
                if ( (p_uaSizeMalloc + *p_uaSizeMalloc) == p_uaAfterFreeSpace ){
                        *(p_uaNextFree-1) = *(p_uaNextFree-1) + *p_uaAfterFreeSpace;
                        *p_uaNextFree = *(p_uaAfterFreeSpace + 1);
                }
                else{
                        *p_uaNextFree = (unitAlloc) p_uaAfterFreeSpace;
                }

                p_stMyBlockInfo->p_cFirstFree = (char *) (((unitAlloc *) p_vMemBlock) - C_ucMallocHeaderSize);
        }

        else {

                if ( (p_uaSizeMalloc + *p_uaSizeMalloc) == p_uaAfterFreeSpace ){
                        *(p_uaNextFree-1) = *(p_uaNextFree-1) + *p_uaAfterFreeSpace;
                        *p_uaNextFree = *(p_uaAfterFreeSpace + 1);
                }
                else {
                        *p_uaNextFree = *(p_uaBeforeFreeSpace +1);
                }

                if ( (p_uaBeforeFreeSpace  + *p_uaBeforeFreeSpace) == p_uaSizeMalloc ){
                        *(p_uaBeforeFreeSpace) =*(p_uaBeforeFreeSpace) + *(p_uaNextFree-1);
                        *(p_uaBeforeFreeSpace +1) = *p_uaNextFree;
                }
                else{
                        *(p_uaBeforeFreeSpace +1) = (unitAlloc)(((unitAlloc*) p_vMemBlock) - C_ucMallocHeaderSize);
                }
        }

        Mmg_MLeaveCriticalSection();
}

void *__cdecl fn_p_vGenAlloc(uint32_t ulSize, uint32_t ulHeapId)
{
    uint32_t ulSlot = (ulHeapId & 0xffu) * 8u;

    if (*(int8_t *)(g_p_d_stCurrentModeInfo + ulSlot + 1u) != -1) {
        return fn_p_vStaAlloc(ulSize, ulHeapId);
    }
    return fn_p_vDynAlloc(ulSize, (unsigned char)ulHeapId);
}

void __cdecl fn_vGenFree(void *pUser, uint32_t ulHeapId)
{
    uint32_t ulSlot = (ulHeapId & 0xffu) * 8u;

    if (*(int8_t *)(g_p_d_stCurrentModeInfo + ulSlot + 1u) != -1) {
        fn_vStaFree(pUser, ulHeapId);
        return;
    }
    fn_vDynFree(pUser, (unsigned char)ulHeapId);
}

typedef struct Mmg_tdstBlockInfoPriv_PC {
    char *p_cBeginBlock;
    char *p_cEndBlock;
    char *p_cFirstFree;
    char *p_cMaxMem;
} Mmg_tdstBlockInfoPriv_PC;

/* 0x00443360 */
void __cdecl Mmg_fn_v_SetModeAlloc(uint8_t ucModuleId,
                              uint8_t ucNewMode,
                              unsigned char ucChannel)
{
    (((tdstCurrentModeInfo *)g_p_d_stCurrentModeInfo)[ucChannel]).ucMode = ucNewMode;
    (((tdstCurrentModeInfo *)g_p_d_stCurrentModeInfo)[ucChannel]).ucModuleId = ucModuleId;
    if (ucNewMode != 0xff) {
        (((tdstCurrentModeInfo *)g_p_d_stCurrentModeInfo)[ucChannel]).p_vTdstBlockInfoOrTdstDynInfo =
            (void *)(((Mmg_tdstBlockInfoPriv_PC **)g_a_p_stMmgModuleBlocksInfo)[ucModuleId] + ucNewMode);
    }
}
