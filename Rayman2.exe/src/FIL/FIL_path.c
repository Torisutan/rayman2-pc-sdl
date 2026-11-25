/*
 =======================================================================================
  Name        : FIL_path.c
  Description : The temp folder in C-ANSI file mode (FILE_MODE_C_WIN95 not set)

  GetTempPathA on the C runtime, behind FIL_pc_M_ulGetTempPath (FIL/FIL_pc.h). The engine
  asks once at start (GAM_init.c) and the saves write their TT.tmp there. The folder is
  the first of the variables TMP, TEMP (both set on Windows; GetTempPathA reads TMP, then
  TEMP) and TMPDIR (POSIX) that is set, else /tmp. Unlike GetTempPathA no separator is
  added: the caller removes a trailing one anyway.

  Result as GetTempPathA: the length copied, without the terminating zero; if szOut
  cannot hold the folder, the size it needs (with the zero) and szOut is left as it is.
 =======================================================================================
 */
#include <stdlib.h>
#include <string.h>
#include "SND/snddef.h"

#ifndef FILE_MODE_C_WIN95

#include "FIL/FIL_pc.h"

unsigned long __cdecl FIL_fn_ulGetTempPath(unsigned long ulSize, char *szOut)
{
    static const char *const a_szVariable[] = { "TMP", "TEMP", "TMPDIR" };
    const char   *szFolder = "/tmp";
    unsigned long ulLen;
    int           i;

    for (i = 0; i < (int)(sizeof(a_szVariable) / sizeof(a_szVariable[0])); i++)
    {
        const char *szValue = getenv(a_szVariable[i]);
        if (szValue != NULL && szValue[0] != '\0')
        {
            szFolder = szValue;
            break;
        }
    }

    ulLen = (unsigned long)strlen(szFolder);
    if (ulLen + 1 > ulSize)
        return ulLen + 1;
    memcpy(szOut, szFolder, ulLen + 1);
    return ulLen;
}

#endif /* FILE_MODE_C_WIN95 */
