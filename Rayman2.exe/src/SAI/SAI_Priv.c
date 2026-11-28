






























#include <string.h>

#include "SAI/SAI.h"
#include "ERM/CpaError.h"
#include "MMG/MMG_alloc.h"
#include "MMG/MMG_pubaccess.h"
#include "MMG/MMG_module.h"

#include "MMG/MMG_globals.h"
#include "MMG/CpaMmg.h"
#include "SAI/MmgSai.h"












/* 0x00403290 */
void __cdecl AI_fn_vMemoryLogFile(void *p_vPointer, unsigned char ucAction, char *szFile, unsigned long ulLine);


/* 0x0044a000 */



SAI_tdxHandleToListEntry __cdecl SAI_fn_hCreateAListEntry(void)
{
    SAI_tdxHandleToListEntry hListEntry;


    Mmg_fn_v_SetModeAlloc(g_ucSAIModuleId, SAI_g_ucMemoryBlockUsed, 0);
    hListEntry = (SAI_tdxHandleToListEntry)
        fn_p_vGenAlloc(sizeof(SAI_tdstListEntry), 0);



#line 73 "SAI_Priv.c"
    AI_fn_vMemoryLogFile(hListEntry, 0, __FILE__, 15);

    if (hListEntry == NULL)
    {

        ERM_fn_vClearLastError(0);
        ERM_fn_vUpdateLastError(8, g_ucSAIModuleId, 0, 4, -1, 0xff, 0xff, (char *)0);
    }

    memset(hListEntry, 0, sizeof(SAI_tdstListEntry));
    hListEntry->ucMemoryLevel = SAI_g_ucMemoryBlockUsed;

    return hListEntry;
}







/* 0x44a090 */



void __cdecl SAI_fn_vIsolateAllListEntryByMemoryLevel(
    SAI_tdstInternalStructure *p_stInternalStructure, unsigned char ucMemoryLevel)
{
    SAI_tdstListEntry *p_stPreviousListEntry = NULL;
    SAI_tdstListEntry *p_stListEntry         = p_stInternalStructure->p_stFirstEntry;

    while (p_stListEntry)
    {
        if (p_stListEntry->ucMemoryLevel == ucMemoryLevel)
        {

            if (p_stPreviousListEntry)
                p_stPreviousListEntry->p_stNextEntry = p_stListEntry->p_stNextEntry;
            else
                p_stInternalStructure->p_stFirstEntry = p_stListEntry->p_stNextEntry;

            if (p_stListEntry->p_stNextEntry == NULL)
                p_stInternalStructure->p_stLastEntry = p_stPreviousListEntry;
        }
        else
            p_stPreviousListEntry = p_stListEntry;

        p_stListEntry = p_stListEntry->p_stNextEntry;
    }


    Mmg_fn_vFreeBlock(ucMemoryLevel, g_ucSAIModuleId,
                      g_a_p_stMmgModuleBlocksInfo[g_ucSAIModuleId]);
}


/* 0x0044a0f0 */






SAI_tdeFlags __cdecl SAI_fn_eVerifyFlags(SAI_tdeFlags eFlags)
{

    eFlags &= SAI_C_UserFlags;

    if ((eFlags & SAI_ePlayerSaveTableValue && eFlags & SAI_ePlayerSaveCurrentValue)
     || (eFlags & SAI_eLevelSaveTableValue  && eFlags & SAI_eLevelSaveCurrentValue))
    {

        ERM_fn_vClearLastError(0);
        ERM_fn_vUpdateLastError(8, g_ucSAIModuleId, 0, 5, -1, 0xff, 0xff, (char *)0);
    }

    if (eFlags & SAI_ePlayerSaveTableValue || eFlags & SAI_ePlayerSaveCurrentValue)
        eFlags |= SAI_eInitWhenPlayerGameSavedLoaded;

    if (eFlags & SAI_eLevelSaveTableValue || eFlags & SAI_eLevelSaveCurrentValue)
        eFlags |= SAI_eInitWhenLevelGameSavedLoaded;

    if (eFlags & SAI_ePlayerSaveCurrentValue || eFlags & SAI_eLevelSaveCurrentValue)
    {
        if (eFlags & (SAI_eInitWhenMapLoaded | SAI_eInitWhenReinitTheMap | SAI_eInitWhenPlayerDead))
        {

            ERM_fn_vUpdateLastError(8, g_ucSAIModuleId, 0, 9, -1, 0xff, 0xff, (char *)0);
        }
    }

    return eFlags;
}


/* 0x0044a190 */



SAI_tdxHandleToListEntry __cdecl fn_hSearchAnElementInList(
    SAI_tdstInternalStructure *p_stInternalStructure,
    void *p_vData, SAI_tdeFlags eUserFlags)
{
    SAI_tdxHandleToListEntry hListEntry = p_stInternalStructure->p_stFirstEntry;

    while (hListEntry)
    {
        if (hListEntry->p_vDataPointer == p_vData &&
            (hListEntry->eFlags & SAI_C_UserFlags) == eUserFlags)
            return hListEntry;

        hListEntry = hListEntry->p_stNextEntry;
    }

    return NULL;
}


/* 0x0044a1d0 */





unsigned char __cdecl SAI_fn_bFreeAnElementFromList(
    SAI_tdstInternalStructure *p_stInternalStructure, void *p_stData, SAI_tdeFlags eUserFlags)
{
    SAI_tdstListEntry *p_stPreviousListEntry = NULL;
    SAI_tdstListEntry *p_stListEntry         = p_stInternalStructure->p_stFirstEntry;
    unsigned char      bFound                = SAI_FALSE;

    eUserFlags = SAI_fn_eVerifyFlags(eUserFlags);

    while (p_stListEntry)
    {
        if (p_stListEntry->p_vDataPointer == p_stData &&
            (p_stListEntry->eFlags & SAI_C_UserFlags) == eUserFlags)
        {
            bFound = SAI_TRUE;
            break;
        }

        p_stPreviousListEntry = p_stListEntry;
        p_stListEntry         = p_stListEntry->p_stNextEntry;
    }

    if (bFound)
    {

        if (p_stPreviousListEntry)
            p_stPreviousListEntry->p_stNextEntry = p_stListEntry->p_stNextEntry;
        else
            p_stInternalStructure->p_stFirstEntry = p_stListEntry->p_stNextEntry;

        if (p_stListEntry->p_stNextEntry == NULL)
            p_stInternalStructure->p_stLastEntry = p_stPreviousListEntry;
    }

    return bFound;
}


/* 0x0044a240 */



unsigned char __cdecl SAI_fn_bInsertAnElementInList(
    SAI_tdstInternalStructure *p_stInternalStructure,
    void *p_stData, unsigned long ulSizeOfData,
    SAI_tdeFlags eTypeOfData, SAI_tdeFlags eUserFlags)
{
    unsigned char bReturn = SAI_TRUE;
    SAI_tdxHandleToListEntry hListEntry;
    unsigned short uwBlocId;
    void *p_vBeginBloc;

    eUserFlags = SAI_fn_eVerifyFlags(eUserFlags);
    if ((hListEntry = fn_hSearchAnElementInList(p_stInternalStructure, p_stData, eUserFlags)) == NULL)
    {
        hListEntry = SAI_fn_hCreateAListEntry();



        if (eTypeOfData == SAI_eType1)
        {
            hListEntry->p_stNextEntry = NULL;
            if (p_stInternalStructure->p_stLastEntry != NULL)
                p_stInternalStructure->p_stLastEntry->p_stNextEntry = hListEntry;
            else
                p_stInternalStructure->p_stFirstEntry = hListEntry;
            p_stInternalStructure->p_stLastEntry = hListEntry;
        }
        else
        {
            hListEntry->p_stNextEntry = p_stInternalStructure->p_stFirstEntry;
            p_stInternalStructure->p_stFirstEntry = hListEntry;
            if (p_stInternalStructure->p_stLastEntry == NULL)
                p_stInternalStructure->p_stLastEntry = hListEntry;
        }
    }

    hListEntry->p_vDataPointer = p_stData;

    switch (eTypeOfData)
    {
     case SAI_eType8:
        hListEntry->eFlags = (SAI_tdeFlags)(eUserFlags | SAI_eType8);
        hListEntry->uData.ucCharData = *(unsigned char *)p_stData;
        break;
     case SAI_eType16:
        hListEntry->eFlags = (SAI_tdeFlags)(eUserFlags | SAI_eType16);
        hListEntry->uData.uwShortData = *(unsigned short *)p_stData;
        break;
     case SAI_eType32:
        hListEntry->eFlags = (SAI_tdeFlags)(eUserFlags | SAI_eType32);
        hListEntry->uData.ulLongData = *(unsigned long *)p_stData;
        break;
     case SAI_eType64:
        hListEntry->eFlags = (SAI_tdeFlags)(eUserFlags | SAI_eType64);
        hListEntry->uData.i64Long64Data = *(SAI_tdLong64 *)p_stData;
        break;
     case SAI_eTypePointer:
        hListEntry->eFlags = (SAI_tdeFlags)(eUserFlags | SAI_eTypePointer);
        if (*(void **)p_stData != NULL)
        {
            Mmg_fn_vWhereIs(*(void **)p_stData, &uwBlocId, &p_vBeginBloc);
            hListEntry->uData.stPointerData.uwBlocAndModuleID = uwBlocId;
            if (uwBlocId == 0xffff)
                hListEntry->uData.stPointerData.p_vPointer = (void *)*(long *)p_stData;
            else
                hListEntry->uData.stPointerData.p_vPointer =
                    (void *)((*(long *)p_stData) - (long)p_vBeginBloc);
        }
        else
        {
            hListEntry->uData.stPointerData.uwBlocAndModuleID = 0xffff;
            hListEntry->uData.stPointerData.p_vPointer = NULL;
        }
        break;
     case SAI_eTypeXX:
        hListEntry->eFlags = (SAI_tdeFlags)(eUserFlags | SAI_eTypeXX);
        if (hListEntry->uData.stArrayData.p_ucPointer == NULL)
        {
#line 251
            M_SAIAlloc(hListEntry->uData.stArrayData.p_ucPointer,unsigned char*,ulSizeOfData);
            if (hListEntry->uData.stArrayData.p_ucPointer == NULL)
            {

                ERM_fn_vClearLastError(0);
                ERM_fn_vUpdateLastError(8, g_ucSAIModuleId, 0, 4, -1, 0xff, 0xff, (char *)0);
            }
            hListEntry->uData.stArrayData.ulArraySize = ulSizeOfData;
        }
        if (hListEntry->uData.stArrayData.ulArraySize == ulSizeOfData)
            memcpy(hListEntry->uData.stArrayData.p_ucPointer, (char *)p_stData, ulSizeOfData);
        else
        {

            ERM_fn_vClearLastError(0);
            ERM_fn_vUpdateLastError(8, g_ucSAIModuleId, 0, 6, -1, 0xff, 0xff, (char *)0);
        }
        break;
     case SAI_eType1:
        hListEntry->eFlags = (SAI_tdeFlags)(eUserFlags | SAI_eType1);
        hListEntry->uData.stBooleanData.ucBitPosition =
            SAI_fn_ucComputeBitPosition((unsigned char)ulSizeOfData);
        hListEntry->uData.stBooleanData.ucCharData =
            (unsigned char)((unsigned char)ulSizeOfData & *(unsigned char *)p_stData);
        break;
     default:
        bReturn = SAI_FALSE;
        break;
    }

    return bReturn;
}


/* 0x0044a500 */






unsigned char __cdecl SAI_fn_bRestoreAnElementFromList(SAI_tdxHandleToListEntry hListEntry)
{
    unsigned char bReturn = SAI_TRUE;
    void *p_vBeginBloc;

    switch (hListEntry->eFlags & SAI_C_TypesFlags)
    {
     case SAI_eType8:
        *(char *)hListEntry->p_vDataPointer = hListEntry->uData.ucCharData;
        break;
     case SAI_eType16:
        *(short *)hListEntry->p_vDataPointer = hListEntry->uData.uwShortData;
        break;
     case SAI_eType32:
        *(long *)hListEntry->p_vDataPointer = hListEntry->uData.ulLongData;
        break;
     case SAI_eType64:
        *(SAI_tdLong64 *)hListEntry->p_vDataPointer = hListEntry->uData.i64Long64Data;
        break;
     case SAI_eTypePointer:
        if (hListEntry->uData.stPointerData.uwBlocAndModuleID == 0xffff)
            *(void **)hListEntry->p_vDataPointer = hListEntry->uData.stPointerData.p_vPointer;
        else
        {
            p_vBeginBloc = Mmg_fn_p_vGiveTheBeginingAddress(hListEntry->uData.stPointerData.uwBlocAndModuleID);
            *(void **)hListEntry->p_vDataPointer =
                (void *)((long)p_vBeginBloc + (long)hListEntry->uData.stPointerData.p_vPointer);
        }
        break;
     case SAI_eTypeXX:
        memcpy((char *)hListEntry->p_vDataPointer,
               hListEntry->uData.stArrayData.p_ucPointer,
               hListEntry->uData.stArrayData.ulArraySize);
        break;
     case SAI_eType1:
        *(unsigned char *)hListEntry->p_vDataPointer =
            (unsigned char)(*(unsigned char *)hListEntry->p_vDataPointer
                            & ~hListEntry->uData.stBooleanData.ucCharData);
        *(unsigned char *)hListEntry->p_vDataPointer =
            (unsigned char)(*(unsigned char *)hListEntry->p_vDataPointer
                            | hListEntry->uData.stBooleanData.ucCharData);
        break;
     default:
        bReturn = SAI_FALSE;
        break;
    }

    return bReturn;
}


/* 0x0044a600 */



unsigned char __cdecl SAI_fn_bInitAllTableWhen(
    SAI_tdstInternalStructure *p_stInternalStructure, SAI_tdeFlags eUserFlags)
{
    SAI_tdxHandleToListEntry hListEntry = p_stInternalStructure->p_stFirstEntry;

    while (hListEntry)
    {
        if ((hListEntry->eFlags & eUserFlags) == eUserFlags)
            SAI_fn_bRestoreAnElementFromList(hListEntry);
        hListEntry = hListEntry->p_stNextEntry;
    }

    return SAI_TRUE;
}


/* 0x0044a630 */



unsigned char __cdecl SAI_fn_ucComputeBitPosition(unsigned char _ucMask)
{
    unsigned char ucPos = 0;


    for (ucPos = 0; ucPos < 8; ucPos++) {
        if (_ucMask & 1)
            return ucPos;
        _ucMask >>= 1;
    }

    return 10;
}



/* 0x00403290 */


void __cdecl SAI_fn_vMemoryLogFile(void *p_vPointer, unsigned char ucAction, char *szFile, unsigned long ulLine)
{
    (void)p_vPointer;
    (void)ucAction;
    (void)szFile;
    (void)ulLine;
}
