#include "GLI/Cache.h"
#include "GLI/GLI_globals.h"
#include "FIL/File.h"
#include "GAM/GAM_globals.h"
#include <string.h>
#include <stdio.h>

void __cdecl GLI_pc_fn_vResetTextureSlotTables(void)
{
    int i;

    for (i = 0; i < 0x400; i++) {
        ((unsigned long *)GLI_gs_aDEFTableOfTextureAlreadyRead)[i] = 0;
    }
    for (i = 0; i < 0x400; i++) {
        GLI_gs_aDEFTableOfTextureMemoryChannels[i] = 0xc0de0005;
    }
}

void __cdecl GLI_pc_fn_vEmptyTextureCache(void)
{
    GLI_pc_fn_vResetTextureSlotTables();
    GLI_gs_lNumberOfTextureToCreate = 0;
}

/* 0x0042c670 */
void __cdecl GLI_pc_fn_vReleaseTextureSlotsOfKey(unsigned long ulKey)
{
    int i;

    for (i = 0; i < 0x400; i++) {
        if (GLI_gs_aDEFTableOfTextureMemoryChannels[i] == (ulKey & 0xffu)) {
            GLI_gs_aDEFTableOfTextureMemoryChannels[i] = 0xc0de0005;
            ((unsigned long *)GLI_gs_aDEFTableOfTextureAlreadyRead)[i] = 0;
        }
    }
}

/* 0x004A06C0 */
static const char GLI_CD_szInsertMsg[] = "Please insert the CD";

extern int  __cdecl Erm_fn_iMessageBox(const char *, const char *, int);
extern void __cdecl GLI_FatalExitOrSleep(void);
void __cdecl GLI_fn_vOpenBigFileTextures(char *p_szArchivePath)
{
    char szOriginalPath[256];

    GLI_g_hConcatTexturesFile = FIL_fn_vOpenConcatFile(p_szArchivePath);
    if (GLI_g_hConcatTexturesFile == NULL) {

        strcpy(szOriginalPath, p_szArchivePath);

        sprintf(p_szArchivePath, "%c\\\\%s",
                (int)g_cCDROM, szOriginalPath);

        GLI_g_hConcatTexturesFile = FIL_fn_vOpenConcatFile(p_szArchivePath);
        while (GLI_g_hConcatTexturesFile == NULL) {
            int iResult = ((int (__cdecl *)(const char *, const char *, int))&Erm_fn_iMessageBox)(
                GLI_CD_szInsertMsg,
                GLI_CD_szInsertMsg,
                0x40035);
            if (iResult == 2) {
                ((void (__cdecl *)(void))&GLI_FatalExitOrSleep)();
            }
            GLI_g_hConcatTexturesFile = FIL_fn_vOpenConcatFile(p_szArchivePath);
        }

        strcpy(p_szArchivePath, szOriginalPath);
    }
}
