#include "GLI/ui_reset_redraw.h"
#include "GLI/GLI_globals.h"
#include "GLI/PixFmt.h"
#include "GLI/Init_Gli_helpers.h"
#include "GLI/cluster3_texture_table.h"
#include "GLI/Memory.h"
#include "GLI/Texture.h"
#include "FIL/File.h"
#include "ERM/CpaError.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

void __cdecl GLI_pc_fn_vLoadProgressUiImage(void *p_stUiObject, char *p_szName, int iFmt)
{
    char                  szFile[260];
    char                  szFile2[260];
    char                 *p_szOut;
    short                 sLo;
    short                 sHi;
    short                 sLo2;
    short                 sHi2;
    short                 sPick;
    GLD_tdstPixelFormat   stDevFmt;
    FIL_tdstGF            stGF;
    char                 *p_szDot;

    p_szOut = szFile;
    if (iFmt != 1 && iFmt != 2 && iFmt != 3)
    {
        ERM_fn_vClearLastError(0);
        ERM_fn_vUpdateLastError(10, GLI_pc_g_ucErrorChannelId, 0, 6, -1, 0xff, 0xff, NULL);
    }
    if (GLI_pc_fn_szStristr(p_szName, "Random") != NULL)
    {
        sscanf(p_szName, "Random<%[^,],%hi,%hi,%[^,],%hi,%hi>",
               szFile, &sLo, &sHi, szFile2, &sLo2, &sHi2);
        if (GLI_pc_g_ucRandomPickEnabled != 0)
        {
            sHi = sHi2;
            sLo = sLo2;
            p_szOut = szFile2;
        }
        sPick = sHi;
        if (sHi != sLo)
        {
            srand((unsigned int)time(NULL));
            sPick = (short)((rand() * (sHi - sLo)) / 0x7fff + sLo);
        }
        sprintf(p_szOut, "%s%02d.gf", p_szOut, sPick);
    }
    else
    {
        strcpy(szFile, p_szName);
        p_szDot = strrchr(szFile, '.');
        strcpy(p_szDot, ".gf");
    }
    FIL_LoadGFLikeImageToRecord(GLI_pc_g_hCdFallbackFile, FUN_00492810(), p_szOut, &stGF);
    if (stGF.p_ucBitMap != NULL)
        GLI_pc_g_p_ucLoadingScreenBitmap = stGF.p_ucBitMap;
    ((GLI_pc_tdstProgressUi *)p_stUiObject)->p_vBuffer = GLI_pc_g_stProgressUi.p_vBuffer;
    strcpy(GLI_pc_g_stProgressUi.szName, p_szOut);
    GLD_vYInvertBitmap24b(GLI_pc_g_p_ucLoadingScreenBitmap, 0x280, 0x1e0);
    GLD_bGetPixelFormatOfDevice(&stDevFmt, (short)GLI_pc_g_uwOverlayDevice, (short)GLI_pc_g_uwOverlayViewport);
    if (stDevFmt.lPixelNbBits == 0x20)
    {
        GLI_pc_g_stProgressUi.lFormatKind = 0;
        GLD_vSetDefinedPixelFormat((unsigned long *)&GLI_pc_g_stProgressUi.stPixelFormat, 5);
        memcpy(GLI_pc_g_stProgressUi.p_vBuffer, GLI_pc_g_p_ucLoadingScreenBitmap,
               (size_t)GLI_pc_g_stProgressUi.lBufferSize);
    }
    else if (stDevFmt.lPixelNbBits == 0x10)
    {
        if (GLI_pc_g_stProgressUi.lBufferDimC == 0x280 && GLI_pc_g_stProgressUi.lBufferDim8 == 0x1e0)
            GLD_vBitmapConvert24bTo16bPixelFormat((unsigned short *)GLI_pc_g_stProgressUi.p_vBuffer,
                                                  GLI_pc_g_p_ucLoadingScreenBitmap, 0x280, 0x1e0);
        else
            GLD_vBitmapConvert24bTo16bPixelFormatAndFiltre((unsigned short *)GLI_pc_g_stProgressUi.p_vBuffer,
                                                           GLI_pc_g_stProgressUi.lBufferDimC,
                                                           GLI_pc_g_stProgressUi.lBufferDim8,
                                                           GLI_pc_g_p_ucLoadingScreenBitmap, 0x280, 0x1e0);
        GLI_pc_g_stProgressUi.lFormatKind = 2;
        GLD_vSetDefinedPixelFormat((unsigned long *)&GLI_pc_g_stProgressUi.stPixelFormat, 7);
        GLD_vBitmapConvert16bPixelFormat(&stDevFmt, &GLI_pc_g_stProgressUi.stPixelFormat,
                                         (unsigned short *)GLI_pc_g_stProgressUi.p_vBuffer,
                                         GLI_pc_g_stProgressUi.lBufferSize);
    }
    GLD_vCopyPixelFormat((unsigned long *)&GLI_pc_g_stProgressUi.stPixelFormat, (unsigned long *)&stDevFmt);
    *(GLI_pc_tdstProgressUi *)p_stUiObject = GLI_pc_g_stProgressUi;
    GLI_pc_fn_vFreeIfNotNull((void **)&GLI_pc_g_p_ucLoadingScreenBitmap);
}

/* 0x0045fb90 */
char * __cdecl GLI_pc_fn_szStristr(char *szHaystack, char *szNeedle)
{
    char *p_szScan      = szHaystack;
    char *p_szMatch     = szHaystack;

    char *p_szNeedle    = szNeedle;

    while (*p_szScan != '\0') {
        if (*p_szNeedle == '\0') {
            break;
        }
        {
            if (tolower(*p_szScan) ==
                tolower(*p_szNeedle)) {

                if (p_szMatch == NULL) {
                    p_szMatch = p_szScan;
                }
                p_szNeedle++;
            } else {

                p_szNeedle = szNeedle;
                p_szMatch  = NULL;
            }
            p_szScan++;
        }
    }

    return (*p_szNeedle == '\0') ? p_szMatch : NULL;
}

void __cdecl FUN_0045fc70(void *p_stUiObject);

void __cdecl FUN_0045fbf0(void *p_stUiObject, char *p_szName, int iFmt)
{
    FUN_0045fc70(p_stUiObject);
    GLI_pc_fn_vLoadProgressUiImage(p_stUiObject, p_szName, iFmt);
}

void __cdecl FUN_0045fc20(void *p_stUiObject, char *p_szName, long lMode)
{
    if (_stricmp((char *)p_stUiObject, p_szName) != 0)
    {
        FUN_0045fc70(p_stUiObject);
        strcpy((char *)p_stUiObject, p_szName);
        ((GLI_pc_tdstProgressUi *)p_stUiObject)->lFormatKind = lMode;
    }
}

void __cdecl FUN_0045fc70(void *p_stUiObject)
{
    ((GLI_pc_tdstProgressUi *)p_stUiObject)->ucProgressBarEnabled = 0;
    GLI_pc_g_stProgressUi.szName[0] = 0;
}
