#include "FON/FON_globals.h"
#include "GAM/GAM_types.h"
#include "FON/fontext.h"
#include "RND/RND.h"

#include <stddef.h>

/* 0x0040355c */
void __cdecl FUN_00407760(void)
{
    FON_g_stGeneral->uwNumberOfLanguages = 0;
    FON_g_stGeneral->d_sLanguageArray    = 0;
    FON_g_stGeneral->p_stCommonLanguage  = 0;
    FON_g_stGeneral->ulElapsedTime       = 0;
    FON_g_stGeneral->hRandomHandle       = RND_fn_hReserveANewHandleIndex();
}

/* 0x00419360 */
extern void FON_fn_vDesinitFONMemory(void);
void FON_fn_vDesInitFonts(void)
{
    FON_fn_vDesinitFONMemory();
}

/* 0x004829D0 */
char *__cdecl FON_fn_szGetTextPointerForHandle(FON_tdxHandleOfText hString)
{
    if (hString == FON_C_xInvalidTextHandle)
        return NULL;

    if (hString < FON_C_HandleOffsetForCommonLanguage)
    {
        /* 0x00500FA8 */
        return FON_g_stGeneral->d_sLanguageArray[*(volatile unsigned long *)&GAM_g_ucCurrentLanguage & 0xFF].d_pTextsArray[hString];
    }

    return FON_g_stGeneral->p_stCommonLanguage->
               d_pTextsArray[hString - FON_C_HandleOffsetForCommonLanguage];
}
