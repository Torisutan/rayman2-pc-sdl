#include "GliDX6vf/GliDX6vf.h"
#include "GliDX6vf/GliDX6vf_State.h"

#include <string.h>

long __cdecl GLI_DRV_lSetCommonFct(char const *name, GLI_CommonFunction callback)
{
    if (strcmp(name, "SetGlobalAlpha") == 0) {
        GLI_g_SetGlobalAlpha = callback;
        return 1;
    }
    if (strcmp(name, "SerialProjection") == 0) {
        GLI_g_SerialProjection = callback;
        return 1;
    }
    if (strcmp(name, "IsGliInit") == 0) {
        GLI_g_IsGliInit = callback;
        return 1;
    }
    if (strcmp(name, "TmpMalloc") == 0) {
        GLI_g_TmpMalloc = callback;
        return 1;
    }
    if (strcmp(name, "TmpFree") == 0) {
        GLI_g_TmpFree = callback;
        return 1;
    }
    if (strcmp(name, "ErmPrintMessage") == 0) {
        GLI_g_ErmPrintMessage = callback;
        return 1;
    }
    if (strcmp(name, "ComputeTextureSize") == 0) {
        GLI_g_ComputeTextureSize = callback;
        return 1;
    }
    if (strcmp(name, "CompressTex565") == 0) {
        GLI_g_CompressTex565 = callback;
        return 1;
    }
    if (strcmp(name, "CompressTex4444") == 0) {
        GLI_g_CompressTex4444 = callback;
        return 1;
    }
    if (strcmp(name, "CompressTex1555") == 0) {
        GLI_g_CompressTex1555 = callback;
        return 1;
    }
    if (strcmp(name, "DivideBitmapSurfaceBy2") == 0) {
        GLI_g_DivideBitmapSurfaceBy2 = callback;
        return 1;
    }
    if (strcmp(name, "DivideBitmapSurfaceBy2NonZero") == 0) {
        GLI_g_DivideBitmapSurfaceBy2NonZero = callback;
        return 1;
    }
    if (strcmp(name, "DivideBitmapSurfaceBy2Palette") == 0) {
        GLI_g_DivideBitmapSurfaceBy2Palette = callback;
        return 1;
    }
    if (strcmp(name, "VerifyTextureIsReallyAlpha") == 0) {
        GLI_g_VerifyTextureIsReallyAlpha = callback;
        return 1;
    }
    if (strcmp(name, "LoadTextureInTexelField") == 0) {
        GLI_g_LoadTextureInTexelField = callback;
        return 1;
    }
    if (strcmp(name, "ReloadTextures") == 0) {
        GLI_g_ReloadTextures = callback;
        return 1;
    }
    if (strcmp(name, "AddDisplayMode") == 0) {
        GLI_g_AddDisplayMode = callback;
        return 1;
    }
    if (strcmp(name, "BlitStretched16b") == 0) {
        GLI_g_BlitStretched16b = callback;
        return 1;
    }
    if (strcmp(name, "GetDefinedPixelFormat") == 0) {
        GLI_g_GetDefinedPixelFormat = callback;
        return 1;
    }
    if (strcmp(name, "AddDefinedPixelFormat") == 0) {
        GLI_g_AddDefinedPixelFormat = callback;
        return 1;
    }
    if (strcmp(name, "ComputeWaitFrameForSmoothSynchro") == 0) {
        GLI_g_ComputeWaitFrameForSmoothSynchro = callback;
        return 1;
    }
    if (strcmp(name, "SimulateFlipSynchroWithoutVSync") == 0) {
        GLI_g_SimulateFlipSynchroWithoutVSync = callback;
        return 1;
    }
    if (strcmp(name, "ReadIniString") == 0) {
        GLI_g_ReadIniString = callback;
        return 1;
    }
    if (strcmp(name, "WriteIniString") == 0) {
        GLI_g_WriteIniString = callback;
        return 1;
    }
    if (strcmp(name, "UseMipMaps") == 0) {
        GLI_g_UseMipMaps = callback;
        return 1;
    }
    if (strcmp(name, "TimerGetCounter") == 0) {
        GLI_g_TimerGetCounter = callback;
        return 1;
    }
    if (strcmp(name, "TimerTickPerSecond") == 0) {
        GLI_g_TimerTickPerSecond = callback;
        return 1;
    }
    if (strcmp(name, "AddToProgressBar") == 0) {
        GLI_g_AddToProgressBar = callback;
        return 1;
    }
    return 0;
}
