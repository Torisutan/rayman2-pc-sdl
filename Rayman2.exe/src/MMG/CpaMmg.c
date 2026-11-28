#include <windows.h>
#include <stdint.h>

#include "MMG/MMG_globals.h"
#include "MMG/MMG_module.h"
#include "MMG/MMG_alloc.h"
#include "ERM/CpaError.h"
#include "MMG/PrivMmg.h"

typedef struct Mmg_tdstBlockInfoPriv_PC {
    char *p_cBeginBlock;
    char *p_cEndBlock;
    char *p_cFirstFree;
    char *p_cMaxMem;
} Mmg_tdstBlockInfoPriv_PC;

typedef struct Mmg_tdstBlockHeaderWithoutFree {
    unsigned char ucBoundededHeaderSize;
    unsigned char ucAlignment;
    unsigned char ucMode;
} Mmg_tdstBlockHeaderWithoutFree;

#define C_uwAllocSize                       4
#define C_uwShiftAllocSize                  2
#define C_BlockWithoutFreeFlag              1
#define C_FillTheBlockWithCleanKeyFlag      2
#define C_Check_OverflowFlag                8
#define C_uaCleanKey                        0xAAAAAAAA
#define C_SizeOfOverflowCheckKey            4

#define Mmg_g_ucModuleId                    MMG_g_ucErmChannelId
#define E_uwMmgStartOfWarning               0x1d
#define E_uwMmgPbAlignmentBlock             2
#define E_uwMmgPbAllocateBlock              0x1f

#define Mmg_M_BigBoundSize(ulSize, ucAlignment) \
    if ((ulSize) & (unsigned long)((ucAlignment) - 1)) { \
        (ulSize) = ((ulSize) & (~((unsigned long)((ucAlignment) - 1)))) + (ucAlignment); \
    }

typedef unsigned long Mmg_unitAlloc;

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

#include <windows.h>
#include <string.h>
#include <stdint.h>

#include "MMG/MMG_globals.h"
#include "MMG/MMG_alloc.h"
#include "ERM/CpaError.h"
#include "MMG/CpaMmg.h"
#include "MMG/PrivMmg.h"

#define Mmg_g_ucModuleId MMG_g_ucErmChannelId

#define g_d_stCurrentModeInfo (g_p_d_stCurrentModeInfo)  /* 0x50b1e8 */

void *fn_p_vDynAlloc(uint32_t ulSize, unsigned char ucChannel);
void  fn_vDynFree(void *pUser, unsigned char ucChannel);

#include <windows.h>
#include <string.h>

#include "MMG/MMG_globals.h"
#include "MMG/MMG_module.h"
#include "MMG/MMG_pubaccess.h"
#include "MMG/MMG_alloc.h"
#include "ERM/CpaError.h"
#include "MMG/CpaMmg.h"
#include "MMG/PrivMmg.h"

typedef struct tdstBlockInfoPriv_PC {
    void *p_cBeginBlock;
    void *p_cEndBlock;
    void *p_cFirstFree;
    void *p_cMaxMem;
} tdstBlockInfoPriv_PC;

/* 0x00442650 */
void __cdecl Mmg_fn_vDeleteBlock(
    unsigned char ucBlockId,
    unsigned char ucModuleId,
    void         *p_stMyBlocksInfo)
{
    Mmg_tdstBlockInfoPriv_PC *p_stMyBlockInfo =
        (Mmg_tdstBlockInfoPriv_PC *)p_stMyBlocksInfo + ucBlockId;

    (void)ucModuleId;

    CPA_FREE(p_stMyBlockInfo->p_cBeginBlock);
    p_stMyBlockInfo->p_cBeginBlock = (char *)C_p_cBlockNotValidKey;
}

void __cdecl Mmg_fn_vTestMallocIntegrityAt(void *p_vMemBlock)
{
    (void)p_vMemBlock;
}

void __cdecl Mmg_fn_cCheckAlignmentValidity(void)
{
}

/* 0x00404540 */
extern void __cdecl Mmg_fn_vStopMmgModule(void);

void __cdecl FUN_00404540(void)
{
    Mmg_fn_vStopMmgModule();
}

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

/* 0x00442420 */
void __cdecl Mmg_fn_vInitSpecificBlock(
    unsigned char  _ucBlockId,
    unsigned char  _ucModuleId,
    void          *_p_stMyBlocksInfo,
    unsigned long  _ulSize,
    unsigned long  _ulMaxNbStaticMalloc,
    unsigned char  _ucAlignment,
    unsigned char  _ucMode)
{
    Mmg_tdstBlockInfoPriv_PC *p_stMyBlockInfo =
        (Mmg_tdstBlockInfoPriv_PC *)_p_stMyBlocksInfo + _ucBlockId;
    char          *p_cBegin;
    unsigned long  ulI;
    Mmg_unitAlloc *p_uaPtr;
    unsigned char  ucAlignment;

    (void)_ucModuleId; (void)_ulMaxNbStaticMalloc;

    if (!(_ucMode & C_BlockWithoutFreeFlag))
    {
        ucAlignment = C_uwAllocSize;
    }
    else
    {
        ucAlignment = _ucAlignment;

        Mmg_M_BigBoundSize(_ulSize, ucAlignment);
        _ulSize += sizeof(Mmg_tdstBlockHeaderWithoutFree);
    }

    Mmg_M_BigBoundSize(_ulSize, ucAlignment);

    p_cBegin = (char *)CPA_MALLOC(_ulSize);

    ulI = (unsigned long)p_cBegin;
    Mmg_M_BigBoundSize(ulI, ucAlignment);
    if (p_cBegin != (char *)ulI)
    {

        ERM_fn_vUpdateLastError(E_uwMmgStartOfWarning, Mmg_g_ucModuleId, 0,
                                E_uwMmgPbAlignmentBlock, -1, 0xff, 0xff, (char *)0);
    }

    if (p_cBegin == NULL)
    {

        ERM_fn_vUpdateLastError(E_uwMmgStartOfWarning, Mmg_g_ucModuleId, 0,
                                E_uwMmgPbAllocateBlock, -1, 0xff, 0xff, (char *)0);
    }
    else
    {
        p_stMyBlockInfo->p_cBeginBlock = p_cBegin;
        p_stMyBlockInfo->p_cEndBlock   = p_cBegin + _ulSize - 1;

        if (!(_ucMode & C_BlockWithoutFreeFlag))
        {
            p_stMyBlockInfo->p_cFirstFree = p_cBegin;
            p_stMyBlockInfo->p_cMaxMem    = p_cBegin;

            p_uaPtr      = (Mmg_unitAlloc *)p_cBegin;
            *p_uaPtr     = (Mmg_unitAlloc)(_ulSize >> C_uwShiftAllocSize);
            *(p_uaPtr+1) = (Mmg_unitAlloc)0;

            if (_ucMode & C_FillTheBlockWithCleanKeyFlag)
            {
                for (p_uaPtr = ((Mmg_unitAlloc *)p_cBegin) + C_ucFreeHeaderSize,
                         _ulSize -= (unsigned long)(ucAlignment << 1);
                     _ulSize > 0;
                     _ulSize -= ucAlignment, p_uaPtr++)
                    *p_uaPtr = C_uaCleanKey;
            }
        }
        else
        {

            unsigned long lTemporaryValue;
            Mmg_tdstBlockHeaderWithoutFree *p_stInfoBlockHeader;

            p_stMyBlockInfo->p_cMaxMem = p_cBegin + sizeof(Mmg_tdstBlockHeaderWithoutFree);
            lTemporaryValue = (unsigned long)p_stMyBlockInfo->p_cMaxMem;
            Mmg_M_BigBoundSize(lTemporaryValue, ucAlignment);
            p_stMyBlockInfo->p_cMaxMem = (char *)lTemporaryValue;
            p_stMyBlockInfo->p_cMaxMem -= 1;

            p_stMyBlockInfo->p_cFirstFree = C_FirstFreeValueForBlockWithoutFree;

            if (_ucMode & C_FillTheBlockWithCleanKeyFlag)
            {
                for (p_cBegin = p_stMyBlockInfo->p_cMaxMem + 1;
                     p_cBegin <= p_stMyBlockInfo->p_cEndBlock; p_cBegin++)
                    *p_cBegin = (char)C_uaCleanKeyForBlockWithoutFree;
            }

            p_stInfoBlockHeader =
                (Mmg_tdstBlockHeaderWithoutFree *)p_stMyBlockInfo->p_cBeginBlock;

            ulI = sizeof(Mmg_tdstBlockHeaderWithoutFree);
            Mmg_M_BigBoundSize(ulI, ucAlignment);
            p_stInfoBlockHeader->ucBoundededHeaderSize = (unsigned char)ulI;
            p_stInfoBlockHeader->ucAlignment           = ucAlignment;
            p_stInfoBlockHeader->ucMode                = _ucMode;

            if ((unsigned long)(p_stMyBlockInfo->p_cEndBlock - p_stMyBlockInfo->p_cMaxMem) >= C_SizeOfOverflowCheckKey)
                Mmg_fn_vWriteOverflowCheckKey(p_stMyBlockInfo->p_cMaxMem + 1);
        }
    }
}

/* 0x004425d0 */
void __cdecl Mmg_fn_vFreeBlock(
    unsigned char ucBlockId,
    unsigned char ucModuleId,
    void         *p_stMyBlocksInfo)
{
    Mmg_tdstBlockInfoPriv_PC *p_stMyBlockInfo =
        (Mmg_tdstBlockInfoPriv_PC *)p_stMyBlocksInfo + ucBlockId;
    char          *p_cBegin = p_stMyBlockInfo->p_cBeginBlock;
    unsigned long *p_uaPtr;

    (void)ucModuleId;

    if (p_stMyBlockInfo->p_cFirstFree != C_FirstFreeValueForBlockWithoutFree)
    {

        p_uaPtr  = (unsigned long *)p_cBegin;
        *p_uaPtr = ((unsigned long)(p_stMyBlockInfo->p_cEndBlock - p_cBegin + 1)) >> C_uwShiftAllocSize;
        p_uaPtr++;
        *p_uaPtr = 0;
        p_stMyBlockInfo->p_cFirstFree = p_cBegin;
        p_stMyBlockInfo->p_cMaxMem    = p_cBegin - 1;
    }
    else
    {

        Mmg_tdstBlockHeaderWithoutFree *p_stInfoBlockHeader =
            (Mmg_tdstBlockHeaderWithoutFree *)p_cBegin;

        p_stMyBlockInfo->p_cMaxMem =
            p_cBegin + p_stInfoBlockHeader->ucBoundededHeaderSize - 1;

        if (p_stInfoBlockHeader->ucMode & C_FillTheBlockWithCleanKeyFlag)
        {
            for (p_cBegin = p_stMyBlockInfo->p_cMaxMem + 1;
                 p_cBegin <= p_stMyBlockInfo->p_cEndBlock; p_cBegin++)
                *p_cBegin = (char)C_uaCleanKeyForBlockWithoutFree;
        }

        if ((p_stInfoBlockHeader->ucMode & C_Check_OverflowFlag) &&
            (unsigned long)(p_stMyBlockInfo->p_cEndBlock - p_stMyBlockInfo->p_cMaxMem) >= C_SizeOfOverflowCheckKey)
        {
            Mmg_fn_vWriteOverflowCheckKey(p_stMyBlockInfo->p_cMaxMem + 1);
        }
    }
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

/* 0x00442770 */
void *__cdecl fn_p_vDynRealloc(void *p_vMemBlock, unsigned long ulSize, unsigned char ucChannel)
{
    unsigned long *p_uaTmp;
    unsigned long *p_uaSizeMalloc;
    long           lDiffSize;

    Mmg_MEnterCriticalSection();

    if (ulSize == 0) {

        fn_vDynFree(p_vMemBlock, ucChannel);
        Mmg_MLeaveCriticalSection();
        return ((void *)0);
    }

    if (ulSize & (unsigned long)(C_uwAllocSize - 1)) {
        ulSize = (ulSize & (~((unsigned long)(C_uwAllocSize - 1)))) + C_uwAllocSize;
    }
    ulSize >>= C_uwShiftAllocSize;
    ulSize += (C_ucMallocHeaderSize + C_ucMallocFooterSize);

    p_uaSizeMalloc = ((unsigned long *)p_vMemBlock) - C_ucMallocHeaderSize;
    lDiffSize = (long)(*p_uaSizeMalloc - ulSize);

    if (lDiffSize == 0) {

        Mmg_MLeaveCriticalSection();
        return (p_vMemBlock);
    }
    else if (lDiffSize > 0) {

        ulSize -= (C_ucMallocHeaderSize + C_ucMallocFooterSize);
        ulSize <<= C_uwShiftAllocSize;

        p_uaTmp = (unsigned long *)fn_p_vDynAlloc((ulSize - (C_ucMallocHeaderSize + C_ucMallocFooterSize)) << C_uwShiftAllocSize, ucChannel);

        memcpy((void *)p_uaTmp, p_vMemBlock, ((unsigned long)((*p_uaSizeMalloc - (unsigned long)lDiffSize - C_ucMallocFooterSize - C_ucMallocHeaderSize))) << C_uwShiftAllocSize);

        fn_vDynFree(p_vMemBlock, ucChannel);

        Mmg_MLeaveCriticalSection();
        return ((void *)p_uaTmp);
    }
    else {

        lDiffSize = -lDiffSize;

        p_uaTmp = (unsigned long *)fn_p_vDynAlloc((ulSize - (C_ucMallocHeaderSize + C_ucMallocFooterSize)) << C_uwShiftAllocSize, ucChannel);

        memcpy((void *)p_uaTmp, p_vMemBlock, (*p_uaSizeMalloc - C_ucMallocFooterSize - C_ucMallocHeaderSize) << C_uwShiftAllocSize);

        fn_vDynFree(p_vMemBlock, ucChannel);

        Mmg_MLeaveCriticalSection();
        return ((void *)p_uaTmp);
    }
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

/* 0x00442b60 */
void * fn_p_vStaRealloc (void *p_vMemBlock, unsigned long ulSize, unsigned char ucChannel){
        tdstBlockInfoPriv * p_stMyBlockInfo = (tdstBlockInfoPriv *) ((tdstCurrentModeInfo *) g_d_stCurrentModeInfo)[ucChannel].p_vTdstBlockInfoOrTdstDynInfo;
        unitAlloc * p_uaTmp;
        unitAlloc * p_uaSizeMalloc;
        unitAlloc *p_uaBeforeBeforeFreeSpace;
        unitAlloc *p_uaBeforeFreeSpace;
        unitAlloc *p_uaAfterFreeSpace;
        unitAlloc *p_uaAfterAfterFreeSpace;
        unitAlloc uaBeforeFreeSize, uaAfterFreeSize;
        long lDiffSize;
        unsigned char b_ucFound = C_ucFalse;

        /* 0x442B84 */
        if (p_stMyBlockInfo->p_cFirstFree == C_FirstFreeValueForBlockWithoutFree){
            ERM_fn_vUpdateLastError(0x1d, Mmg_g_ucModuleId, 0, 0x1c, -1, 0xff, 0xff, (char *)0);
        }

        Mmg_MEnterCriticalSection();

        {

            if (ulSize == 0){

                    fn_vStaFree (p_vMemBlock, ucChannel);
                    Mmg_MLeaveCriticalSection();
                    return ((void * )0);
            }

            Mmg_M_BigBoundSize (ulSize, C_uwAllocSize);
            ulSize >>= C_uwShiftAllocSize;
            ulSize += (C_ucMallocHeaderSize + C_ucMallocFooterSize);

            p_uaSizeMalloc = ((unitAlloc *) p_vMemBlock) - C_ucMallocHeaderSize;
            lDiffSize = (long)(*p_uaSizeMalloc - ulSize);

            p_uaBeforeBeforeFreeSpace = p_uaBeforeFreeSpace = (unitAlloc *) p_stMyBlockInfo->p_cFirstFree;
            b_ucFound = C_ucFalse;
            while ((p_uaBeforeFreeSpace != (unitAlloc *) 0 ) && (p_uaBeforeFreeSpace < p_uaSizeMalloc) && !b_ucFound ){
                      if ((*p_uaBeforeFreeSpace)>=(unitAlloc) ulSize){
                              b_ucFound = C_ucTrue;
                      }
                      else {
                            p_uaBeforeBeforeFreeSpace = p_uaBeforeFreeSpace;
                            p_uaBeforeFreeSpace = (unitAlloc *) *(p_uaBeforeFreeSpace+1);
                      }
            }
            if (b_ucFound){

                    ulSize -= (C_ucMallocHeaderSize + C_ucMallocFooterSize);
                    ulSize <<= C_uwShiftAllocSize;

                    p_uaTmp = (unitAlloc *) fn_p_vStaAlloc (ulSize, ucChannel);

                    memcpy ((void *) p_uaTmp, p_vMemBlock, ulSize);

                    fn_vStaFree (p_vMemBlock, ucChannel);

                    p_uaSizeMalloc = p_uaTmp - C_ucMallocHeaderSize;
                    if ((char*)(p_uaSizeMalloc + *p_uaSizeMalloc)-1>=p_stMyBlockInfo->p_cMaxMem){

                            p_stMyBlockInfo->p_cMaxMem=(char*)(p_uaSizeMalloc + *p_uaSizeMalloc)-1;
                    }

                    Mmg_MLeaveCriticalSection();
                    return ((void *) p_uaTmp);
            }

            p_uaBeforeBeforeFreeSpace = p_uaBeforeFreeSpace = p_uaAfterFreeSpace = (unitAlloc *) p_stMyBlockInfo->p_cFirstFree;
            while ((p_uaAfterFreeSpace != (unitAlloc *) 0 ) && (p_uaAfterFreeSpace < (unitAlloc *) p_vMemBlock) ){
                    p_uaBeforeBeforeFreeSpace = p_uaBeforeFreeSpace;
                    p_uaBeforeFreeSpace = p_uaAfterFreeSpace;
                    p_uaAfterFreeSpace = (unitAlloc *) *(p_uaAfterFreeSpace+1);
            }

            if ((lDiffSize == 0) && ((p_uaBeforeFreeSpace != (unitAlloc *) 0) && (p_uaBeforeFreeSpace + *p_uaBeforeFreeSpace == p_uaSizeMalloc))){

                    p_uaTmp = p_uaSizeMalloc + *p_uaSizeMalloc - *p_uaBeforeFreeSpace;

                    memmove (p_uaBeforeFreeSpace, p_uaSizeMalloc, (*p_uaSizeMalloc - (unsigned long)lDiffSize - C_ucMallocFooterSize)<< C_uwShiftAllocSize );
                    *(p_uaTmp+1) = (unitAlloc) p_uaAfterFreeSpace;
                    *p_uaTmp = (unitAlloc) (((unitAlloc *)p_vMemBlock) - p_uaBeforeFreeSpace - C_ucMallocHeaderSize);

                    p_vMemBlock = (void *) (p_uaBeforeFreeSpace + C_ucMallocHeaderSize);
                    p_uaSizeMalloc = p_uaBeforeFreeSpace;

                    if (p_uaBeforeBeforeFreeSpace == p_uaBeforeFreeSpace){
                            p_stMyBlockInfo->p_cFirstFree = (char *) p_uaTmp;
                    }
                    else {
                            *(p_uaBeforeBeforeFreeSpace +1) = (unitAlloc) p_uaTmp;
                    }

                    if ((p_uaTmp + *p_uaTmp)== p_uaAfterFreeSpace){
                            *p_uaTmp = *p_uaTmp + *p_uaAfterFreeSpace;
                            *(p_uaTmp+1) = *(p_uaAfterFreeSpace + 1);
                    }

                    *p_uaSizeMalloc = (unitAlloc)ulSize;
                    p_vMemBlock = (void *)(p_uaSizeMalloc + C_ucMallocHeaderSize);

                    if ((char*)(p_uaSizeMalloc + *p_uaSizeMalloc)-1>=p_stMyBlockInfo->p_cMaxMem){

                            p_stMyBlockInfo->p_cMaxMem=(char*)(p_uaSizeMalloc + *p_uaSizeMalloc)-1;
                    }

                    Mmg_MLeaveCriticalSection();
                    return (p_vMemBlock);
            }

            else if (lDiffSize > 0){

                    if ((p_uaBeforeFreeSpace != (unitAlloc *) 0) && (p_uaBeforeFreeSpace + *p_uaBeforeFreeSpace == p_uaSizeMalloc)){

                            p_uaTmp = p_uaSizeMalloc + *p_uaSizeMalloc - lDiffSize - *p_uaBeforeFreeSpace;

                            memmove (p_uaBeforeFreeSpace, p_uaSizeMalloc, (*p_uaSizeMalloc - (unsigned long)lDiffSize - C_ucMallocFooterSize)<< C_uwShiftAllocSize );
                            *(p_uaTmp+1) = (unitAlloc) p_uaAfterFreeSpace;
                            *p_uaTmp = (unitAlloc)(((unitAlloc *)p_vMemBlock) - p_uaBeforeFreeSpace - C_ucMallocHeaderSize + lDiffSize);

                            p_vMemBlock = (void *) (p_uaBeforeFreeSpace + C_ucMallocHeaderSize);
                            p_uaSizeMalloc = p_uaBeforeFreeSpace;

                            if (p_uaBeforeBeforeFreeSpace == p_uaBeforeFreeSpace){
                                    p_stMyBlockInfo->p_cFirstFree = (char *) p_uaTmp;
                            }
                            else {
                                    *(p_uaBeforeBeforeFreeSpace +1) = (unitAlloc) p_uaTmp;
                            }

                            if ((p_uaTmp + *p_uaTmp)== p_uaAfterFreeSpace){
                                    *p_uaTmp = *p_uaTmp + *p_uaAfterFreeSpace;
                                    *(p_uaTmp+1) = *(p_uaAfterFreeSpace + 1);
                            }
                    }

                    else if (p_uaSizeMalloc + *p_uaSizeMalloc == p_uaAfterFreeSpace ){

                            p_uaTmp = p_uaSizeMalloc + *p_uaSizeMalloc - lDiffSize;

                            *p_uaTmp = (unitAlloc)lDiffSize + *p_uaAfterFreeSpace;

                            *(p_uaTmp+1) = *(p_uaAfterFreeSpace+1);

                            if (p_uaAfterFreeSpace == p_uaBeforeFreeSpace ){
                                    p_stMyBlockInfo->p_cFirstFree = (char *) p_uaTmp;
                            }
                            else {
                                    *(p_uaBeforeFreeSpace + 1)  = (unitAlloc) p_uaTmp;
                            }
                    }

                    else if (lDiffSize >= (long)C_ucFreeHeaderSize){

                            p_uaTmp = p_uaSizeMalloc + *p_uaSizeMalloc - lDiffSize;

                            *p_uaTmp = (unitAlloc)lDiffSize;
                            *(p_uaTmp+1) = (unitAlloc) p_uaAfterFreeSpace;

                            if (p_uaBeforeFreeSpace == p_uaAfterFreeSpace){
                                    p_stMyBlockInfo->p_cFirstFree = (char *) p_uaTmp;
                            }
                            else {
                                    *(p_uaBeforeFreeSpace + 1)  = (unitAlloc) p_uaTmp;
                            }
                    }
                    else {
                        Mmg_MLeaveCriticalSection();
                        return (p_vMemBlock);
                    }

                    *p_uaSizeMalloc = (unitAlloc)ulSize;
                    p_vMemBlock = (void *)(p_uaSizeMalloc + C_ucMallocHeaderSize);

                    if ((char*)(p_uaSizeMalloc + *p_uaSizeMalloc)-1>=p_stMyBlockInfo->p_cMaxMem){

                            p_stMyBlockInfo->p_cMaxMem=(char*)(p_uaSizeMalloc + *p_uaSizeMalloc)-1;
                    }

                    Mmg_MLeaveCriticalSection();
                    return (p_vMemBlock);
            }

            else if (lDiffSize < 0){

                    lDiffSize = -lDiffSize;

                    if ( ( (p_uaBeforeFreeSpace +(*p_uaBeforeFreeSpace)) == p_uaSizeMalloc ) &&
                             ((uaBeforeFreeSize = *p_uaBeforeFreeSpace) >= (unitAlloc)lDiffSize) )
                    {

                            *p_uaBeforeFreeSpace -= ((unitAlloc) lDiffSize);

                            if (*p_uaBeforeFreeSpace < (unitAlloc) C_ucFreeHeaderSize)
                            {

                                    ulSize += (*p_uaBeforeFreeSpace);
                                    lDiffSize = lDiffSize + (long)(*p_uaBeforeFreeSpace);

                                    if (p_uaBeforeFreeSpace == p_uaBeforeBeforeFreeSpace){
                                            p_stMyBlockInfo->p_cFirstFree = (char *) p_uaAfterFreeSpace;
                                    }
                                    else {

                                            *(p_uaBeforeBeforeFreeSpace + 1) = (unitAlloc) p_uaAfterFreeSpace;
                                    }

                                    memmove (p_uaBeforeFreeSpace, p_uaSizeMalloc, (*p_uaSizeMalloc - C_ucMallocFooterSize)<< C_uwShiftAllocSize );

                                    p_uaSizeMalloc = p_uaBeforeFreeSpace;
                                    p_vMemBlock = (void *) (p_uaBeforeFreeSpace + C_ucMallocHeaderSize);
                            }
                            else {

                                    p_uaTmp = p_uaBeforeFreeSpace + *p_uaSizeMalloc + lDiffSize;

                                    memmove (p_uaBeforeFreeSpace, p_uaSizeMalloc, (*p_uaSizeMalloc - C_ucMallocFooterSize)<< C_uwShiftAllocSize );

                                    *p_uaTmp = (unitAlloc) (uaBeforeFreeSize - (unsigned long)lDiffSize);
                                    *(p_uaTmp +1) = (unitAlloc) p_uaAfterFreeSpace;

                                    if (p_uaBeforeBeforeFreeSpace == p_uaBeforeFreeSpace){
                                            p_stMyBlockInfo->p_cFirstFree = (char *) p_uaTmp;
                                    }
                                    else {
                                            *(p_uaBeforeBeforeFreeSpace +1) = (unitAlloc) p_uaTmp;
                                    }

                                    if ((p_uaTmp + *p_uaTmp)== p_uaAfterFreeSpace){
                                            *p_uaTmp = *p_uaTmp + *p_uaAfterFreeSpace;
                                            *(p_uaTmp+1) = *(p_uaAfterFreeSpace + 1);
                                    }

                                    p_uaSizeMalloc = p_uaBeforeFreeSpace;
                                    p_vMemBlock = (void *) (p_uaBeforeFreeSpace + C_ucMallocHeaderSize);
                            }

                            *p_uaSizeMalloc = (unitAlloc)ulSize;

                            if ((char*)(p_uaSizeMalloc + *p_uaSizeMalloc)-1>=p_stMyBlockInfo->p_cMaxMem)
                            {

                                    p_stMyBlockInfo->p_cMaxMem=(char*)(p_uaSizeMalloc + *p_uaSizeMalloc)-1;
                            }

                            Mmg_MLeaveCriticalSection();
                            return (p_vMemBlock);
                    }

                    else if ( ( p_uaSizeMalloc + *p_uaSizeMalloc == p_uaAfterFreeSpace) &&
                                 ((p_uaBeforeFreeSpace +(*p_uaBeforeFreeSpace)) == p_uaSizeMalloc )&&
                                 ((uaBeforeFreeSize + (uaAfterFreeSize = *p_uaAfterFreeSpace)) >= (unitAlloc) lDiffSize) )
                    {

                            p_uaAfterAfterFreeSpace = (unitAlloc *) (*(p_uaAfterFreeSpace+1));

                            *p_uaBeforeFreeSpace = (unitAlloc) (uaBeforeFreeSize + uaAfterFreeSize - (unitAlloc)lDiffSize);

                            if (*p_uaBeforeFreeSpace < (unitAlloc) C_ucFreeHeaderSize)
                            {

                                    ulSize += (*p_uaBeforeFreeSpace);
                                    lDiffSize = lDiffSize + (long)(*p_uaBeforeFreeSpace);

                                    if (p_uaBeforeFreeSpace == p_uaBeforeBeforeFreeSpace){
                                            p_stMyBlockInfo->p_cFirstFree = (char *) *(p_uaAfterFreeSpace+1);
                                    }
                                    else {

                                            *(p_uaBeforeBeforeFreeSpace + 1) = *(p_uaAfterFreeSpace+1);
                                    }

                                    memmove (p_uaBeforeFreeSpace, p_uaSizeMalloc, (*p_uaSizeMalloc - C_ucMallocFooterSize)<< C_uwShiftAllocSize );

                                    p_uaSizeMalloc = p_uaBeforeFreeSpace;
                                    p_vMemBlock = (void *) (p_uaBeforeFreeSpace + C_ucMallocHeaderSize);
                            }

                            else {

                                    p_uaTmp = p_uaBeforeFreeSpace + *p_uaSizeMalloc + lDiffSize;

                                    memmove (p_uaBeforeFreeSpace, p_uaSizeMalloc, (*p_uaSizeMalloc - C_ucMallocFooterSize)<< C_uwShiftAllocSize );

                                    *p_uaTmp = (unitAlloc) (uaBeforeFreeSize + uaAfterFreeSize - (unitAlloc)lDiffSize);
                                    *(p_uaTmp +1) = (unitAlloc) p_uaAfterAfterFreeSpace;

                                    if (p_uaBeforeBeforeFreeSpace == p_uaBeforeFreeSpace){
                                            p_stMyBlockInfo->p_cFirstFree = (char *) p_uaTmp;
                                    }
                                    else {
                                            *(p_uaBeforeBeforeFreeSpace +1) = (unitAlloc) p_uaTmp;
                                    }

                                    p_uaSizeMalloc = p_uaBeforeFreeSpace;
                                    p_vMemBlock = (void *) (p_uaBeforeFreeSpace + C_ucMallocHeaderSize);
                            }

                            *p_uaSizeMalloc = (unitAlloc)ulSize;

                            if ((char*)(p_uaSizeMalloc + *p_uaSizeMalloc)-1>=p_stMyBlockInfo->p_cMaxMem)
                            {

                                    p_stMyBlockInfo->p_cMaxMem=(char*)(p_uaSizeMalloc + *p_uaSizeMalloc)-1;
                            }

                            Mmg_MLeaveCriticalSection();
                            return (p_vMemBlock);
                    }

                    else if ( ( p_uaSizeMalloc + *p_uaSizeMalloc == p_uaAfterFreeSpace) &&
                                 ((uaAfterFreeSize = *p_uaAfterFreeSpace) >= (unitAlloc) lDiffSize) )
                    {

                            *p_uaAfterFreeSpace -= ((unitAlloc) lDiffSize);

                            if (*p_uaAfterFreeSpace < (unitAlloc) C_ucFreeHeaderSize)
                            {

                                    ulSize += (*p_uaAfterFreeSpace);
                                    lDiffSize = lDiffSize + (long)(*p_uaAfterFreeSpace);

                                    if (p_uaAfterFreeSpace == p_uaBeforeFreeSpace){
                                            p_stMyBlockInfo->p_cFirstFree = (char *) *(p_uaAfterFreeSpace+1);
                                    }
                                    else {

                                            *(p_uaBeforeFreeSpace + 1) = *(p_uaAfterFreeSpace + 1);
                                    }
                            }
                            else {
                                    p_uaTmp = (unitAlloc *) *(p_uaAfterFreeSpace + 1);
                                    *(p_uaAfterFreeSpace + lDiffSize) = *p_uaAfterFreeSpace;
                                    *(p_uaAfterFreeSpace + lDiffSize + 1) = (unitAlloc) p_uaTmp;

                                    if (p_uaBeforeFreeSpace == p_uaAfterFreeSpace){
                                            p_stMyBlockInfo->p_cFirstFree = (char *) (p_uaAfterFreeSpace + lDiffSize);
                                    }
                                    else {
                                            *(p_uaBeforeFreeSpace+1) = (unitAlloc) (p_uaAfterFreeSpace + lDiffSize);
                                    }
                            }

                            *p_uaSizeMalloc = (unitAlloc)ulSize;

                            if ((char*)(p_uaSizeMalloc + *p_uaSizeMalloc)-1>=p_stMyBlockInfo->p_cMaxMem)
                            {

                                    p_stMyBlockInfo->p_cMaxMem=(char*)(p_uaSizeMalloc + *p_uaSizeMalloc)-1;
                            }

                            Mmg_MLeaveCriticalSection();
                            return (p_vMemBlock);
                    }

                    else{
                            ulSize -= (C_ucMallocHeaderSize + C_ucMallocFooterSize);
                            ulSize <<= C_uwShiftAllocSize;

                            p_uaTmp = (unitAlloc *) fn_p_vStaAlloc (ulSize, ucChannel);

                            if (p_uaTmp != NULL) {
                                    memcpy ((void *) p_uaTmp, p_vMemBlock, (*p_uaSizeMalloc - C_ucMallocFooterSize - C_ucMallocHeaderSize)<< C_uwShiftAllocSize);

                                    fn_vStaFree (p_vMemBlock, ucChannel);

                                    p_uaSizeMalloc = p_uaTmp - C_ucMallocHeaderSize;
                                    if ((char*)(p_uaSizeMalloc + *p_uaSizeMalloc)-1>=p_stMyBlockInfo->p_cMaxMem)
                                    {

                                            p_stMyBlockInfo->p_cMaxMem=(char*)(p_uaSizeMalloc + *p_uaSizeMalloc)-1;
                                    }
                            }

                            Mmg_MLeaveCriticalSection();
                            return ((void *) p_uaTmp);
                    }
            }

            Mmg_MLeaveCriticalSection();
            return (p_vMemBlock);
        }
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

/* 0x004431a0 */
void *__cdecl fn_p_vGenRealloc(void *p_vMemBlock, unsigned long ulSize, unsigned char ucChannel)
{
    unsigned char ucDispatch = g_d_stCurrentModeInfo[1 + ucChannel * 8u];

    if ((char)ucDispatch != -1) {
        return fn_p_vStaRealloc(p_vMemBlock, ulSize, ucChannel);
    }
    return fn_p_vDynRealloc(p_vMemBlock, ulSize, ucChannel);
}

/* 0x004431E0 */
void __cdecl Mmg_fn_vWhereIs(void           *p_vPtr,
                             unsigned short *p_uwModuleBlockId,
                             void          **pp_vBeginAdr)
{
    unsigned char ucModuleId;
    unsigned char ucBlockId;
    *p_uwModuleBlockId = 0xFFFF;

    for (ucModuleId = 0;
         ucModuleId < g_ucErmNbOfModule && *p_uwModuleBlockId == 0xFFFF;
         ucModuleId++)
    {
        tdstBlockInfoPriv_PC *a_stMyBlockInfo =
            (tdstBlockInfoPriv_PC *)g_a_p_stMmgModuleBlocksInfo[ucModuleId];

        if (a_stMyBlockInfo != NULL) {
            for (ucBlockId = 0;
                 ucBlockId < g_a_ucBlocksNbInModule[ucModuleId] &&
                 *p_uwModuleBlockId == 0xFFFF;
                 ucBlockId++, a_stMyBlockInfo++)
            {
                if (a_stMyBlockInfo != NULL &&
                    a_stMyBlockInfo->p_cBeginBlock != C_p_cBlockNotValidKey) {
                    if (p_vPtr >= a_stMyBlockInfo->p_cBeginBlock &&
                        p_vPtr <= a_stMyBlockInfo->p_cEndBlock) {
                        *p_uwModuleBlockId =
                            (unsigned short)(((unsigned int)ucModuleId << 8) |
                                             (unsigned int)ucBlockId);
                        *pp_vBeginAdr = a_stMyBlockInfo->p_cBeginBlock;
                    }
                }
            }
        }
    }
}

/* 0x00443280 */
void *__cdecl Mmg_fn_p_vGiveTheBeginingAddress(unsigned short uwModuleBlockId)
{
	unsigned char ucModuleId = uwModuleBlockId>>8;
	unsigned char ucBlockId = uwModuleBlockId & 0xFF;

	return (((tdstBlockInfo *)g_a_p_stMmgModuleBlocksInfo[ucModuleId])[ucBlockId].p_cBeginBlock);
}

/* 0x004432B0 */
unsigned char __cdecl Mmg_fn_ucGiveChannelId(unsigned long ulThreadId,
                                             unsigned char ucGroupId)
{

    unsigned char    ucI = 1;
    unsigned char    bFound = 0;
    tdstChannelInfo *p_stChannelInfo = MMG_M_p_TabChannelInfo + 1;

    Mmg_MEnterCriticalSection();

    while (ucI < g_stMmgGeneralChannelInfo_storage.ucNbChannelUsed &&
           !bFound &&
           p_stChannelInfo->ucGroupId == C_ucMmgDefaultGroup)
    {
        if ((p_stChannelInfo->ulThreadId == ulThreadId) &&
            (p_stChannelInfo->ucGroupId == ucGroupId)) {
            bFound = 0xFF;
        } else {
            ucI++;
            p_stChannelInfo++;
        }
    }

    if (!bFound) {
        p_stChannelInfo->ulThreadId = ulThreadId;
        p_stChannelInfo->ucGroupId  = ucGroupId;
        if (ucI == g_stMmgGeneralChannelInfo_storage.ucNbChannelUsed) {
            g_stMmgGeneralChannelInfo_storage.ucNbChannelUsed++;
        }
    }

    Mmg_MLeaveCriticalSection();
    return ucI;
}

void __cdecl Mmg_fn_vReleaseChannelId(unsigned char ucChannelId)
{
    /* 0x50B1E4 */
    MMG_M_p_TabChannelInfo[ucChannelId].ucGroupId = C_ucMmgInvalidGroup;
}

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

/* 0x004433C0 */
void __cdecl Mmg_fn_vFirstInitMmgModule(unsigned char ucMaxNbChannel)
{
    unsigned char ucI;

    g_p_d_stCurrentModeInfo =
        (uint8_t *)CPA_MALLOC((unsigned long)ucMaxNbChannel * 8);

    MMG_M_p_TabChannelInfo =
        (tdstChannelInfo *)CPA_MALLOC((unsigned long)ucMaxNbChannel * 8);

    for (ucI = 0; ucI < C_ucNbOfMaxModule; ucI++) {
        g_a_p_stMmgModuleBlocksInfo[ucI] = NULL;
    }

    g_stMmgGeneralChannelInfo_storage.ucMaxNbChannel  = ucMaxNbChannel;
    g_stMmgGeneralChannelInfo_storage.ucNbChannelUsed = 1;

    Mmg_MCreateCriticalSection();
}

void __cdecl Mmg_fn_v_StopMmg(unsigned char ucModuleId);

/* 0x00443420 */
void __cdecl Mmg_fn_vStopMmgModule(void)
{
    unsigned char ucI;

    for (ucI = 0; ucI < g_ucErmNbOfModule; ucI++) {
        Mmg_fn_v_StopMmg(ucI);
    }

    Mmg_MDeleteCriticalSection();

    CPA_FREE((HGLOBAL)g_p_d_stCurrentModeInfo);
    g_p_d_stCurrentModeInfo = NULL;

    CPA_FREE((HGLOBAL)MMG_M_p_TabChannelInfo);
    MMG_M_p_TabChannelInfo = NULL;
}

/* 0x00443490 */
void __cdecl Mmg_fn_v_InitMmg(unsigned char ucModuleId,
                              unsigned char ucMaxBlocksNb,
                              unsigned long ulMaxNbDynamicMalloc)
{
    unsigned char  ucI;
    unsigned long *pulFill;

    (void)ulMaxNbDynamicMalloc;

    if (ucMaxBlocksNb > 0) {
        pulFill = (unsigned long *)CPA_MALLOC((unsigned long)ucMaxBlocksNb << 4);
        g_a_p_stMmgModuleBlocksInfo[ucModuleId] =
            (struct tdstBlockInfoPriv *)pulFill;

        for (ucI = 0; ucI < ucMaxBlocksNb; ucI++) {
            *pulFill = 0xffffffffu;
            pulFill += 4;
        }

        g_a_ucBlocksNbInModule[ucModuleId] = ucMaxBlocksNb;
    }
}

void __cdecl Mmg_fn_v_StopMmg(unsigned char ucModuleId)
{
    unsigned char ucI;

    if (g_a_p_stMmgModuleBlocksInfo[ucModuleId] != NULL) {

        for (ucI = 0; ucI < g_a_ucBlocksNbInModule[ucModuleId]; ucI++) {

            if (((Mmg_tdstBlockInfoPriv_PC *)g_a_p_stMmgModuleBlocksInfo[ucModuleId])[ucI].p_cBeginBlock != C_p_cBlockNotValidKey) {

                Mmg_fn_vDeleteBlock(ucI, ucModuleId, g_a_p_stMmgModuleBlocksInfo[ucModuleId]);
            }
        }

        CPA_FREE(g_a_p_stMmgModuleBlocksInfo[ucModuleId]);
        g_a_p_stMmgModuleBlocksInfo[ucModuleId] = NULL;
    }
}

/* 0x00443560 */
void __cdecl Mmg_fn_vGiveInformationBlock(unsigned char  ucModuleId,
                                          unsigned char  ucBlockId,
                                          tdstBlockInfo *p_stMyBlockInfo)
{
    tdstBlockInfoPriv_PC *p_stPriv =
        (tdstBlockInfoPriv_PC *)g_a_p_stMmgModuleBlocksInfo[ucModuleId]
        + ucBlockId;

    p_stMyBlockInfo->p_cBeginBlock = p_stPriv->p_cBeginBlock;
    p_stMyBlockInfo->p_cEndBlock   = p_stPriv->p_cEndBlock;
    p_stMyBlockInfo->p_cFirstFree  = p_stPriv->p_cFirstFree;
    p_stMyBlockInfo->p_cMaxMem     = p_stPriv->p_cMaxMem;
}

/* 0x004435A0 */
void __cdecl Mmg_fn_vSaveInformationBlock(unsigned char  ucModuleId,
                                          unsigned char  ucBlockId,
                                          tdstBlockInfo *p_stMyBlockInfo)
{
    tdstBlockInfoPriv_PC *p_stPriv =
        (tdstBlockInfoPriv_PC *)g_a_p_stMmgModuleBlocksInfo[ucModuleId]
        + ucBlockId;

    p_stPriv->p_cBeginBlock = p_stMyBlockInfo->p_cBeginBlock;
    p_stPriv->p_cEndBlock   = p_stMyBlockInfo->p_cEndBlock;
    p_stPriv->p_cFirstFree  = p_stMyBlockInfo->p_cFirstFree;
    p_stPriv->p_cMaxMem     = p_stMyBlockInfo->p_cMaxMem;
}
