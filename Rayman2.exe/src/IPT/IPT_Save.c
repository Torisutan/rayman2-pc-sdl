#include <stdio.h>
#include "IPT/IPT.h"
#include "IPT/IPT_Save.h"

/* 0x0042E0C0 */
__declspec(dllimport) int __cdecl _stricmp(const char *szA, const char *szB);

/* 0x0042EA10 */
extern unsigned int (__cdecl *_imp__fread)(void *pBuffer, unsigned int uSize, unsigned int uCount, FILE *p_stFile);

#define IPT_C_swKeyWordKeyPressed   ((short)0xf006)

#define IPT_C_swKeyWordPadKey       ((short)0xf01a)

/* 0x0042E8F0 */
unsigned char __cdecl IPT_fn_bSaveOptionsInFile(FILE *p_stFile)
{
    unsigned long          ulI;
    unsigned char          ucLength;
    IPT_tdstEntryElement  *hEntryElement;
    IPT_tdstKeyWordElement *hKeyWord;

    for (ulI = 0; ulI < IPT_g_hInputStructure.ulNumberOfEntryElement; ulI++)
    {
        hEntryElement = &IPT_g_hInputStructure.d_stEntryElementArray[ulI];
        if (hEntryElement->pc_bSaveInOptions != 0
            && hEntryElement->p_szEntryName != NULL
            && hEntryElement->d_stKeyWordElementArray != NULL)
        {
            ucLength = (unsigned char)(strlen(hEntryElement->p_szEntryName) + 1);
            fwrite(&ucLength, 1, 1, p_stFile);
            fwrite(hEntryElement->p_szEntryName, 1, ucLength, p_stFile);
            hKeyWord = hEntryElement->d_stKeyWordElementArray;
            if (hKeyWord[0].u_ElementUnion.swValue == IPT_C_swKeyWordKeyPressed)
            {
                fwrite(&hKeyWord[1].u_ElementUnion.swValue, 2, 1, p_stFile);
            }
            else if (hKeyWord[0].u_ElementUnion.swValue == IPT_C_swKeyWordPadKey)
            {
                fwrite(&hKeyWord[1].u_ElementUnion.swValue, 2, 1, p_stFile);
                fwrite(&hKeyWord[2].u_ElementUnion.swValue, 2, 1, p_stFile);
            }
        }
    }
    ucLength = 0xff;
    fwrite(&ucLength, 1, 1, p_stFile);
    return IPT_C_TRUE;
}

/* 0x0042E0C0 */
IPT_tdstEntryElement *__cdecl IPT_pc_fn_p_stFindEntryByName(char *szName)
{
    IPT_tdstEntryElement *hFound;
    unsigned long         ulI;

    hFound = NULL;
    for (ulI = 0; ulI < IPT_g_hInputStructure.ulNumberOfEntryElement; ulI++)
    {
        if (IPT_g_hInputStructure.d_stEntryElementArray[ulI].p_szEntryName != NULL
            && _stricmp(IPT_g_hInputStructure.d_stEntryElementArray[ulI].p_szEntryName, szName) == 0)
        {
            hFound = &IPT_g_hInputStructure.d_stEntryElementArray[ulI];
        }
    }
    return hFound;
}

unsigned char __cdecl IPT_fn_bLoadOptionsFromFile(FILE *p_stFile)
{
    unsigned char          ucLength;
    char                   szName[256];
    IPT_tdstEntryElement  *hEntryElement;
    unsigned int         (__cdecl *pfnFread)(void *, unsigned int, unsigned int, FILE *);

    pfnFread = _imp__fread;
#ifdef RAYMAN2_FIX_BUGS
    /* FIX(BUGS.md#7) */
    /* A damaged file: an unknown entry name dereferenced NULL, a short read repeated the last
       length forever, a name without its terminator was read past its end and a key code
       outside 0..255 indexed past p_ucKeyboardCounter. Stop at the first such entry (the size
       of an unknown entry's values is not known, so the rest cannot be read); the entries read
       so far stay, the others keep their current values. */
    if (pfnFread(&ucLength, 1, 1, p_stFile) != 1)
        return IPT_C_TRUE;
    while (ucLength != 0xff)
    {
        short swValue1, swValue2;

        if (pfnFread(szName, 1, ucLength, p_stFile) != ucLength)
            break;
        szName[ucLength] = '\0';
        hEntryElement = IPT_pc_fn_p_stFindEntryByName(szName);
        if (hEntryElement == NULL || hEntryElement->d_stKeyWordElementArray == NULL)
            break;
        if (hEntryElement->d_stKeyWordElementArray[0].u_ElementUnion.swValue
            == IPT_C_swKeyWordKeyPressed)
        {
            if (pfnFread(&swValue1, 2, 1, p_stFile) != 1
                || (unsigned short)swValue1 >= sizeof(IPT_g_hInputStructure.p_ucKeyboardCounter))
                break;
            hEntryElement->d_stKeyWordElementArray[1].u_ElementUnion.swValue = swValue1;
        }
        else if (hEntryElement->d_stKeyWordElementArray[0].u_ElementUnion.swValue
                 == IPT_C_swKeyWordPadKey)
        {
            if (pfnFread(&swValue1, 2, 1, p_stFile) != 1 || pfnFread(&swValue2, 2, 1, p_stFile) != 1)
                break;
            hEntryElement->d_stKeyWordElementArray[1].u_ElementUnion.swValue = swValue1;
            hEntryElement->d_stKeyWordElementArray[2].u_ElementUnion.swValue = swValue2;
        }
        if (pfnFread(&ucLength, 1, 1, p_stFile) != 1)
            break;
    }
#else
    pfnFread(&ucLength, 1, 1, p_stFile);
    while (ucLength != 0xff)
    {
        pfnFread(szName, 1, ucLength, p_stFile);
        hEntryElement = IPT_pc_fn_p_stFindEntryByName(szName);
        if (hEntryElement->d_stKeyWordElementArray[0].u_ElementUnion.swValue
            == IPT_C_swKeyWordKeyPressed)
        {
            pfnFread(&hEntryElement->d_stKeyWordElementArray[1].u_ElementUnion.swValue,
                     2, 1, p_stFile);
        }
        else if (hEntryElement->d_stKeyWordElementArray[0].u_ElementUnion.swValue
                 == IPT_C_swKeyWordPadKey)
        {
            pfnFread(&hEntryElement->d_stKeyWordElementArray[1].u_ElementUnion.swValue,
                     2, 1, p_stFile);
            pfnFread(&hEntryElement->d_stKeyWordElementArray[2].u_ElementUnion.swValue,
                     2, 1, p_stFile);
        }
        pfnFread(&ucLength, 1, 1, p_stFile);
    }
#endif /* RAYMAN2_FIX_BUGS */
    return IPT_C_TRUE;
}
