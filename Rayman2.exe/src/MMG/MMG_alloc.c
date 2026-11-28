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

/* 0x004431a0 */
void *__cdecl fn_p_vGenRealloc(void *p_vMemBlock, unsigned long ulSize, unsigned char ucChannel)
{
    unsigned char ucDispatch = g_d_stCurrentModeInfo[1 + ucChannel * 8u];

    if ((char)ucDispatch != -1) {
        return fn_p_vStaRealloc(p_vMemBlock, ulSize, ucChannel);
    }
    return fn_p_vDynRealloc(p_vMemBlock, ulSize, ucChannel);
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
