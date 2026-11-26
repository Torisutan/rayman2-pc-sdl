#include "GLI/CdFileLoader.h"
#include "GLI/GLI_globals.h"
#include "GAM/GAM_globals.h"
#include "FIL/File.h"

#include <string.h>
#include <stdio.h>

#define GLI_pc_p_hCdFallbackFile (&GLI_pc_g_hCdFallbackFile)

static const char GLI_CD_szInsertMsg[] = "Please insert the CD";
static const char GLI_CD_szPathFmt[]   = "%c\\\\%s";

extern int  __cdecl Erm_fn_iMessageBox(const char *, const char *, int);
extern void __cdecl GLI_FatalExitOrSleep(void);
void __cdecl GLI_pc_fn_vOpenFileWithCdFallback(char *path)
{
    typedef int   (__cdecl *pfn_msgbox_t)(const char *body,
                                            const char *title,
                                            unsigned int uFlags);
    typedef void  (__cdecl *pfn_exit_t  )(void);

    pfn_msgbox_t fn_004028f0 = (pfn_msgbox_t)&Erm_fn_iMessageBox;
    pfn_exit_t   fn_004011c0 = (pfn_exit_t  )&GLI_FatalExitOrSleep;

    char  szSaved[256];
    void *pHandle;

    pHandle = FIL_fn_vOpenConcatFile(path);
    *GLI_pc_p_hCdFallbackFile = pHandle;

    if (pHandle == NULL) {
        strcpy(szSaved, path);
        sprintf(path,
                GLI_CD_szPathFmt,
                (int)g_cCDROM,
                szSaved);
        pHandle = FIL_fn_vOpenConcatFile(path);
        *GLI_pc_p_hCdFallbackFile = pHandle;

        while (pHandle == NULL) {
            int iRc = fn_004028f0(
                GLI_CD_szInsertMsg,
                GLI_CD_szInsertMsg,
                0x40035u);
            if (iRc == 2) {
                fn_004011c0();
            }
            pHandle = FIL_fn_vOpenConcatFile(path);
            *GLI_pc_p_hCdFallbackFile = pHandle;
        }

        strcpy(path, szSaved);
    }
}
