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
