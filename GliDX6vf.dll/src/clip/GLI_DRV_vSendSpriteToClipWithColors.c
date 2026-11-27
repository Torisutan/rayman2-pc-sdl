#include "GliDX6vf/GliDX6vf.h"
#include "GliDX6vf/GliDX6vf_State.h"
#include "GliDX6vf/clip_staging.h"
#include "GliDX6vf/GLI_DiagLog.h"

__declspec(dllexport) void __cdecl
GLI_DRV_vSendSpriteToClipWithColors(float *param_1, unsigned int *param_2,
                                     float param_3, GLI_tdstInternalGlobalValuesFor3dEngine *p_stGlobals)
{
    float u_right, u_left, v_top, v_bot;
    unsigned int uVar8;

    GLI_M_DiagCountSend();
    if (GLI_g_ActivateGliFlag && GLI_DiagVerbose()) {
        GLI_DiagLog("send vSendSpriteToClipWithColors #%lu p1=%p colors=%p z=0x%08x ctx=0x%x",
                    GLI_g_DiagSendCount, (void *)param_1, (void *)param_2, *(unsigned int *)&param_3, (int)p_stGlobals);
    }
    GLI_ResetStagingCursor();

    if (param_1[1] == param_1[5]) {

    GLI_SnapshotClipWindow();

    if (*(int *)&param_1[1] > *(int *)&GLI_g_ClipYMin) return;
    if (*(int *)&param_1[9] < *(int *)&GLI_g_ClipYMax) return;
    if (*(int *)&param_1[0] < *(int *)&GLI_g_ClipXMin) return;
    if (*(int *)&param_1[4] > *(int *)&GLI_g_ClipXMax) return;

    if (*(int *)&param_1[0] > *(int *)&GLI_g_ClipXMax) {
        float x0_orig = param_1[0];
        float xmax_v;
        u_right = (GLI_g_GliBigGlobals->fXMaxClipping - param_1[4]) / (x0_orig - param_1[4]);
        xmax_v = GLI_g_GliBigGlobals->fXMaxClipping;
        param_1[0xc]  = xmax_v;
        param_1[0]    = xmax_v;
    } else {
        u_right = 1.0f;
    }

    if (*(int *)&param_1[4] < *(int *)&GLI_g_ClipXMin) {
        float xmin_v;
        u_left = ((GLI_g_GliBigGlobals->fXMinClipping - param_1[4]) * u_right) / (param_1[0] - param_1[4]);
        xmin_v = GLI_g_GliBigGlobals->fXMinClipping;
        param_1[8]  = xmin_v;
        param_1[4]  = xmin_v;
    } else {
        u_left = 0.0f;
    }

    if (*(int *)&param_1[1] < *(int *)&GLI_g_ClipYMax) {
        float y0_orig = param_1[1];
        float ymin_v;
        v_top = (GLI_g_GliBigGlobals->fYMinClipping - param_1[9]) / (y0_orig - param_1[9]);
        ymin_v = GLI_g_GliBigGlobals->fYMinClipping;
        param_1[5] = ymin_v;
        param_1[1] = ymin_v;
    } else {
        v_top = 1.0f;
    }

    if (*(int *)&param_1[9] > *(int *)&GLI_g_ClipYMin) {
        float ymax_v;
        v_bot = ((GLI_g_GliBigGlobals->fYMaxClipping - param_1[9]) * v_top) / (param_1[1] - param_1[9]);
        ymax_v = GLI_g_GliBigGlobals->fYMaxClipping;
        param_1[0xd] = ymax_v;
        param_1[9]   = ymax_v;
    } else {
        v_bot = 0.0f;
    }

    *(float *) GLI_g_pStagingCursor          = param_1[0];
    *(float *)(GLI_g_pStagingCursor + 0x04)  = param_1[1];
    *(float *)(GLI_g_pStagingCursor + 0x0c)  = param_3;
    *(float *)(GLI_g_pStagingCursor + 0x08)  = param_3;
    *(unsigned int *)(GLI_g_pStagingCursor + 0x10) = *param_2++;
    *(float *)(GLI_g_pStagingCursor + 0x20)  = param_1[4];
    *(float *)(GLI_g_pStagingCursor + 0x24)  = param_1[5];
    *(float *)(GLI_g_pStagingCursor + 0x2c)  = param_3;
    *(float *)(GLI_g_pStagingCursor + 0x28)  = param_3;
    *(unsigned int *)(GLI_g_pStagingCursor + 0x30) = *param_2++;
    *(float *)(GLI_g_pStagingCursor + 0x40)  = param_1[8];
    *(float *)(GLI_g_pStagingCursor + 0x44)  = param_1[9];
    *(float *)(GLI_g_pStagingCursor + 0x4c)  = param_3;
    *(float *)(GLI_g_pStagingCursor + 0x48)  = param_3;
    *(unsigned int *)(GLI_g_pStagingCursor + 0x50) = *param_2++;
    *(float *)(GLI_g_pStagingCursor + 0x60)  = param_1[0xc];
    *(float *)(GLI_g_pStagingCursor + 0x64)  = param_1[0xd];
    *(float *)(GLI_g_pStagingCursor + 0x6c)  = param_3;
    *(float *)(GLI_g_pStagingCursor + 0x68)  = param_3;
    *(unsigned int *)(GLI_g_pStagingCursor + 0x70) = *param_2++;

    if (p_stGlobals->xTextureDontAcceptFog != 0) {
        uVar8 = 0xff000000u;
    } else {
        uVar8 = p_stGlobals->ulSpecularColorForSprite;
    }
    *(unsigned int *)(GLI_g_pStagingCursor + 0x14) = uVar8;
    *(unsigned int *)(GLI_g_pStagingCursor + 0x34) = uVar8;
    *(unsigned int *)(GLI_g_pStagingCursor + 0x54) = uVar8;
    *(unsigned int *)(GLI_g_pStagingCursor + 0x74) = uVar8;

    *(float *)(GLI_g_pStagingCursor + 0x18) = u_right;
    *(float *)(GLI_g_pStagingCursor + 0x1c) = v_top;
    *(float *)(GLI_g_pStagingCursor + 0x38) = u_left;
    *(float *)(GLI_g_pStagingCursor + 0x3c) = v_top;
    *(float *)(GLI_g_pStagingCursor + 0x58) = u_left;
    *(float *)(GLI_g_pStagingCursor + 0x5c) = v_bot;
    *(float *)(GLI_g_pStagingCursor + 0x78) = u_right;
    *(float *)(GLI_g_pStagingCursor + 0x7c) = v_bot;

    GLI_FinalizeSpriteStaging();
    GLI_FlushStagedPrimitives();
    } else {

        GLI_SendSpriteTrianglesViaClip((unsigned int *)param_1, p_stGlobals);
    }
}
