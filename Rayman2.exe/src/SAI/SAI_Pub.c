#include <stddef.h>

#include "SAI/SAI.h"

/* 0x00449cd0 */
unsigned char __cdecl SAI_fn_bInsertBoolInTable(void *_p_vValue, unsigned char ucMask, SAI_tdeFlags _eFlags)
{
    return SAI_fn_bInsertAnElementInList(SAI_g_p_stInternalStructure, _p_vValue, (unsigned long)ucMask, SAI_eType1, _eFlags);
}

unsigned char __cdecl SAI_fn_bInsertCharInTable(void *_p_vValue, SAI_tdeFlags _eFlags)
{
    return SAI_fn_bInsertAnElementInList(SAI_g_p_stInternalStructure, _p_vValue, 1, SAI_eType8, _eFlags);
}

unsigned char __cdecl SAI_fn_bInsertLongInTable(void *_p_vValue, SAI_tdeFlags _eFlags)
{
    return SAI_fn_bInsertAnElementInList(SAI_g_p_stInternalStructure, _p_vValue, 4, SAI_eType32, _eFlags);
}

unsigned char __cdecl SAI_fn_bInsertPointerInTable(void *_p_vValue, SAI_tdeFlags _eFlags)
{
    return SAI_fn_bInsertAnElementInList(SAI_g_p_stInternalStructure, _p_vValue, 8, SAI_eTypePointer, _eFlags);
}

/* 0x00449d60 */
unsigned char __cdecl SAI_fn_bInsertFloatInTable(void *_p_vValue, SAI_tdeFlags _eFlags)
{
    return SAI_fn_bInsertLongInTable(_p_vValue, _eFlags);
}

/* 0x00449d80 */
unsigned char __cdecl SAI_fn_bInsertMTHVectorInTable(void *_p_vValue, SAI_tdeFlags _eFlags)
{
    return SAI_fn_bInsertArrayInTable(_p_vValue, 3 * 4, _eFlags);
}

/* 0x00449da0 */
unsigned char __cdecl SAI_fn_bRemoveValueFromTable(void *_p_vValue, SAI_tdeFlags _eFlags)
{
    return SAI_fn_bFreeAnElementFromList(SAI_g_p_stInternalStructure, _p_vValue, _eFlags);
}

/* 0x00449dc0 */
void *__cdecl SAI_fn_p_vGetTableValuePointerOf(void *_p_vValue, SAI_tdeFlags _eFlags)
{
    void *p_vReturn = NULL;
    SAI_tdxHandleToListEntry hListEntry;
    SAI_tdeFlags eInternalFlags;

    eInternalFlags = SAI_fn_eVerifyFlags(_eFlags);
    if ((hListEntry = fn_hSearchAnElementInList(SAI_g_p_stInternalStructure, _p_vValue, eInternalFlags)) != NULL)
    {
        switch (hListEntry->eFlags & SAI_C_TypesFlags)
        {
         case SAI_eTypeXX:
            p_vReturn = (void *)hListEntry->uData.stArrayData.p_ucPointer;
            break;
         case SAI_eType1:
         case SAI_eType8:
         case SAI_eType16:
         case SAI_eType32:
         case SAI_eType64:
         case SAI_eTypePointer:

            p_vReturn = (void *)&hListEntry->uData.ucCharData;
            break;
        }
    }
    return p_vReturn;
}

unsigned char __cdecl SAI_fn_bInsertArrayInTable(void *_p_vValue, unsigned long _ulSize, SAI_tdeFlags _eFlags)
{
    return SAI_fn_bInsertAnElementInList(SAI_g_p_stInternalStructure, _p_vValue, _ulSize, SAI_eTypeXX, _eFlags);
}

/* 0x00449e80 */
unsigned char __cdecl SAI_fn_bInitAllWhen(SAI_tdeFlags _eUserFlags)
{
    return SAI_fn_bInitAllTableWhen(SAI_g_p_stInternalStructure, _eUserFlags);
}

/* 0x00449ea0 */
unsigned char __cdecl SAI_fn_ucPlayerSaveValues(char *_szFileName)
{
    return SAI_fn_ucSaveAllGameTableValues(SAI_g_p_stInternalStructure, _szFileName,
                                           SAI_ePlayerSaveTableValue | SAI_ePlayerSaveCurrentValue);
}
