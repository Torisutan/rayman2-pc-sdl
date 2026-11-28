#include <string.h>

#include "AI/AI_types.h"

unsigned char g_ucUseDefaultActionReturn;
unsigned char g_ucNewActionReturn;

/* 0x004826A0 */
unsigned char __cdecl fn_ucIsRuleInActionTable(AI_tdstIntelligence *p_stIntelligence,
                                               unsigned char ucNumRule)
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
                    return ucCurrentEntry;
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

/* 0x00482760 */
void __cdecl fn_vUseCurrentActionTableEntry(AI_tdstIntelligence *p_stIntelligence, unsigned char ucNRule,
                                            AI_tdstNodeInterpret *p_stNode)
{
    unsigned char ucCurrentEntry = AI_M_ActionTableCurrentEntry(p_stIntelligence);
    if (!AI_M_ActionTableUseFlag(p_stIntelligence, ucCurrentEntry))
    {
        fn_vInitActionTableEntry(p_stIntelligence, ucCurrentEntry, ucNRule, p_stNode, 1);
        AI_M_ActionTableNbEntriesUsed(p_stIntelligence)++;
    }
}

/* 0x004827B0 */
void __cdecl fn_vInitActionTableEntry(AI_tdstIntelligence *p_stIntelligence, unsigned char ucNEntry,
                                      unsigned char ucNumRule, AI_tdstNodeInterpret *p_stNode,
                                      unsigned char bfUsed)
{
    AI_M_ActionTableUseActionReturn(p_stIntelligence, ucNEntry) = g_ucUseDefaultActionReturn;
    AI_M_ActionTableNewActionReturn(p_stIntelligence, ucNEntry) = g_ucNewActionReturn;
    AI_M_ActionTableUseFlag(p_stIntelligence, ucNEntry)         = bfUsed;
    AI_M_ActionTableNumNode(p_stIntelligence, ucNEntry)         = p_stNode;
    AI_M_ActionTableNumRule(p_stIntelligence, ucNEntry)         = ucNumRule;
    memset(AI_M_ActionTableParam(p_stIntelligence, ucNEntry), 0,
           sizeof(AI_M_ActionTableParam(p_stIntelligence, ucNEntry)));
}

/* 0x00482820 */
void __cdecl fn_vUnuseCurrentActionTableEntry(AI_tdstIntelligence *p_stIntelligence)
{
    unsigned char ucCurrentEntry = AI_M_ActionTableCurrentEntry(p_stIntelligence);
    if (AI_M_ActionTableUseFlag(p_stIntelligence, ucCurrentEntry))
    {
        fn_vInitActionTableEntry(p_stIntelligence, ucCurrentEntry, 0, NULL, 0);
        AI_M_ActionTableNbEntriesUsed(p_stIntelligence)--;
    }
}

/* 0x00482870 */
void __cdecl fn_vUnuseAllRulesWhichHaveGreaterNumRule(AI_tdstIntelligence *p_stIntelligence,
                                                      unsigned char ucNumRule)
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
                if (p_stEntry->ucNumRule > ucNumRule)
                {
                    fn_vInitActionTableEntry(p_stIntelligence, ucCurrentEntry, 0, NULL, 0);
                    AI_M_ActionTableNbEntriesUsed(p_stIntelligence)--;
                }
            }
        }
    }
}

/* 0x004828E0 */
void __cdecl fn_vUnuseAllRulesFromSchedule(AI_tdstIntelligence *p_stIntelligence)
{
    unsigned char ucNbEntries;

    if (AI_M_IsActionTableUsed(p_stIntelligence))
    {
        unsigned char            ucCurrentEntry;
        AI_tdstActionTableEntry *p_stEntry;

        ucNbEntries = AI_M_ActionTableNbEntries(p_stIntelligence);
        p_stEntry   = &AI_M_ActionTableEntryN(p_stIntelligence, 0);

        for (ucCurrentEntry = 0; ucCurrentEntry < ucNbEntries; ucCurrentEntry++, p_stEntry++)
        {
            if (p_stEntry->bUsed)
            {
                if (p_stEntry->ucNumRule == 0)
                {
                    fn_vInitActionTableEntry(p_stIntelligence, ucCurrentEntry, 0, NULL, 0);
                    ucNbEntries--;
                }
            }
        }
        AI_M_ActionTableNbEntriesUsed(p_stIntelligence) = ucNbEntries;
    }
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

/* 0x00482990 */
AI_tdstNodeInterpret *__cdecl fn_p_stGetTableAction(AI_tdstIntelligence *p_stIntelligence,
                                                    unsigned char ucNEntry)
{
    g_ucUseDefaultActionReturn = AI_M_ActionTableUseActionReturn(p_stIntelligence, ucNEntry);
    g_ucNewActionReturn        = AI_M_ActionTableNewActionReturn(p_stIntelligence, ucNEntry);
    return (AI_tdstNodeInterpret *)AI_M_ActionTableNumNode(p_stIntelligence, ucNEntry);
}
