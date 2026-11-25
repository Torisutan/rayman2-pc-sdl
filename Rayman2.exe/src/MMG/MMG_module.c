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

void __cdecl Mmg_fn_vReleaseChannelId(unsigned char ucChannelId)
{
    /* 0x50B1E4 */
    MMG_M_p_TabChannelInfo[ucChannelId].ucGroupId = C_ucMmgInvalidGroup;
}

void __cdecl Mmg_fn_vTestMallocIntegrityAt(void *p_vMemBlock)
{
    (void)p_vMemBlock;
}

void __cdecl Mmg_fn_cCheckAlignmentValidity(void)
{
}

/* 0x00443280 */
void *__cdecl Mmg_fn_p_vGiveTheBeginingAddress(unsigned short uwModuleBlockId)
{
	unsigned char ucModuleId = uwModuleBlockId>>8;
	unsigned char ucBlockId = uwModuleBlockId & 0xFF;

	return (((tdstBlockInfo *)g_a_p_stMmgModuleBlocksInfo[ucModuleId])[ucBlockId].p_cBeginBlock);
}

/* 0x00404540 */
extern void __cdecl Mmg_fn_vStopMmgModule(void);

void __cdecl FUN_00404540(void)
{
    Mmg_fn_vStopMmgModule();
}
