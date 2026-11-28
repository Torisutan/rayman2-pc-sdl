#include <stddef.h>

#include "ACP/ACP_types.h"
#include "AI/AI_types.h"
#include "AI/StrIntel.h"
#include "ERM/CpaError.h"
#include "MMG/MMG_globals.h"

#define M_IsTheEndOfTree(p_Tree)   ((p_Tree)->eType == 8)

/* 0x00480950 */
AI_tdstNodeInterpret *__cdecl fn_p_stSwapNextParameter(AI_tdstNodeInterpret *p_stTree)
{
    unsigned char ucDepth = M_DepthInterpret(p_stTree);

    p_stTree++;
    while ((M_DepthInterpret(p_stTree) >= ucDepth) && (!M_IsTheEndOfTree(p_stTree)))
        p_stTree++;
    return p_stTree;
}

/* 0x00480980 */
AI_tdstNodeInterpret *__cdecl fn_p_stSwapNextParameter2(AI_tdstNodeInterpret *p_stTree)
{
    unsigned char ucDepth = M_DepthInterpret(p_stTree - 1);

    while ((M_DepthInterpret(p_stTree) > ucDepth) && (!M_IsTheEndOfTree(p_stTree)))
        p_stTree++;
    return p_stTree;
}

/* 0x00480FB0 */
void __cdecl fn_vOnAIErm_M_UpdateLastError(unsigned short FatalErrorNum, char *szMsg)
{
    ERM_fn_vUpdateLastError(0x42, g_ucAIModuleId, 0, FatalErrorNum,
                            C_lErmNoDebugData, C_ucErmOpenInfoWindow,
                            C_ucAllowStopForDebug, szMsg);
}

void __cdecl fn_vErrorOnAIAlloc(void)
{
    if (Erm_fn_uwCheckError(MMG_g_ucErmChannelId, 0))
    {
        ERM_fn_vClearLastError(0);
        fn_vOnAIErm_M_UpdateLastError((unsigned short)0xC, (char *)0);
    }
}

void __cdecl fn_vErrorOnAIFree(void)
{
    if (Erm_fn_uwCheckError(MMG_g_ucErmChannelId, 0))
    {
        ERM_fn_vClearLastError(0);
        ERM_fn_vUpdateLastError(0x42, g_ucAIModuleId, 0, 0x43, -1, 0xff, 0xff, (char *)0);
    }
}

/* 0x00481060 */
void __cdecl fn_vReinitComport(AI_tdstIntelligence *p_stIntelligence)
{
    (void)fn_ucChangeComportIntell(p_stIntelligence,
                                      (AI_tdstComport *)p_stIntelligence->ulNoComportInit);
    AI_M_GetPrevComport(p_stIntelligence) = AI_M_GetCurrentComport(p_stIntelligence);
}

void __cdecl fn_vInitActionTable(AI_tdstIntelligence *p_stIntelligence);

unsigned char __cdecl fn_ucChangeComportIntell(
    AI_tdstIntelligence *p_stIntelligence,
    AI_tdstComport      *p_stComport)
{

    AI_M_GetPrevComport(p_stIntelligence) = AI_M_GetCurrentComport(p_stIntelligence);
    AI_M_GetCurrentComport(p_stIntelligence) = p_stComport;
    fn_vInitActionTable(p_stIntelligence);

    if (AI_M_GetComportSchedule(p_stComport) == NULL)
        AI_M_GetCurrentSchedule(p_stIntelligence) = NULL;
    else
        AI_M_GetCurrentSchedule(p_stIntelligence) = AI_M_GetTreeComportSchedule(p_stComport);

    return TRUE;
}

/* 0x004810C0 */
unsigned char __cdecl fn_ucGetTreeNbParam(AI_tdstNodeInterpret *p_stTree)
{
    unsigned char ucNbParam = 0;
    unsigned char ucDepth   = M_DepthInterpret(p_stTree - 1);

    while ((M_DepthInterpret(p_stTree) > ucDepth) && (!M_IsTheEndOfTree(p_stTree)))
    {
        if (M_DepthInterpret(p_stTree) == ucDepth + 1)
            ucNbParam++;
        p_stTree++;
    }
    return (ucNbParam);
}

/* 0x004826E0 */
unsigned char __cdecl fn_ucIsRuleAndNodeInActionTable(AI_tdstIntelligence *p_stIntelligence,
                                                      unsigned char ucNumRule,
                                                      AI_tdstNodeInterpret *p_stNode)
{
    unsigned char ucNbEntries = AI_M_ActionTableNbEntries(p_stIntelligence);

    if (AI_M_IsActionTableUsed(p_stIntelligence))
    {
        unsigned char            ucCurrentEntry;
        AI_tdstActionTableEntry *p_stEntry = &AI_M_ActionTableEntryN(p_stIntelligence, 0);

        for (ucCurrentEntry = 0; ucCurrentEntry < ucNbEntries; ucCurrentEntry++, p_stEntry++)
        {
            if (p_stEntry->bUsed)
            {
                if (p_stEntry->ucNumRule == ucNumRule)
                {
                    if (p_stEntry->p_stNode == p_stNode)
                        return ucCurrentEntry;
                }
            }
        }
    }
    return ucNbEntries;
}

/* 0x00482730 */
unsigned char __cdecl fn_ucFindPlaceInActionTable(AI_tdstIntelligence *p_stIntelligence)
{
    unsigned char ucCurrentEntry;
    unsigned char ucNbEntries = AI_M_ActionTableNbEntries(p_stIntelligence);

    if (AI_M_ActionTableNbEntriesUsed(p_stIntelligence) < ucNbEntries)
    {
        AI_tdstActionTableEntry *p_stEntry = &AI_M_ActionTableEntryN(p_stIntelligence, 0);

        for (ucCurrentEntry = 0; ucCurrentEntry < ucNbEntries; ucCurrentEntry++, p_stEntry++)
        {
            if (!p_stEntry->bUsed)
                return ucCurrentEntry;
        }
    }
    return ucNbEntries;
}

/* 0x00482940 */
void __cdecl fn_vInitActionTable(AI_tdstIntelligence *p_stIntelligence)
{
    unsigned char ucNEntry;
    unsigned char ucNbEntries;

    AI_M_ActionTableNbEntriesUsed(p_stIntelligence) = 0;
    AI_M_ActionTableCurrentEntry(p_stIntelligence) = 0;
    ucNbEntries = AI_M_ActionTableNbEntries(p_stIntelligence);
    for (ucNEntry = 0; ucNEntry < ucNbEntries; ucNEntry++)
    {
        fn_vInitActionTableEntry(p_stIntelligence, ucNEntry, 0, NULL, 0);
    }
}
