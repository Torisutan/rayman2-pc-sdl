#include "GliDX6vf/GliDX6vf.h"
#include "GliDX6vf/GliDX6vf_State.h"
#include "GliDX6vf/clip_staging.h"
#include "GliDX6vf/GLI_DiagLog.h"

#define CTX_XMAX_OFFSET         0x2d3a8
#define CTX_XMIN_OFFSET         0x2d3a4
#define CTX_YMIN_OFFSET         0x2d3ac
#define CTX_YMAX_OFFSET         0x2d3b0

#define CTX_DIFFUSE_OFFSET      0x2d3d4
#define CTX_SPECULAR_OFFSET     0x2d3d8
#define CTX_FOG_ALPHA_FLAG      0x2d3f8

__declspec(dllexport) void __cdecl
GLI_DRV_vSendSpriteToClip(float *param_1, float param_2, int param_3)
{
    float u_right, u_left, v_top, v_bot;
    unsigned int uVar8;

    GLI_M_DiagCountSend();
    if (GLI_g_ActivateGliFlag && GLI_DiagVerbose()) {
        GLI_DiagLog("send vSendSpriteToClip #%lu p1=%p z=0x%08x ctx18=0x%x",
                    GLI_g_DiagSendCount, (void *)param_1, *(unsigned int *)&param_2, param_3);
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
        u_right = (*(float *)((unsigned char *)GLI_g_GliBigGlobals + CTX_XMAX_OFFSET) - param_1[4]) / (x0_orig - param_1[4]);
        xmax_v = *(float *)((unsigned char *)GLI_g_GliBigGlobals + CTX_XMAX_OFFSET);
        param_1[0xc]  = xmax_v;
        param_1[0]    = xmax_v;
    } else {
        u_right = 1.0f;
    }

    if (*(int *)&param_1[4] < *(int *)&GLI_g_ClipXMin) {
        float xmin_v;
        u_left = ((*(float *)((unsigned char *)GLI_g_GliBigGlobals + CTX_XMIN_OFFSET) - param_1[4]) * u_right) / (param_1[0] - param_1[4]);
        xmin_v = *(float *)((unsigned char *)GLI_g_GliBigGlobals + CTX_XMIN_OFFSET);
        param_1[8]  = xmin_v;
        param_1[4]  = xmin_v;
    } else {
        u_left = 0.0f;
    }

    if (*(int *)&param_1[1] < *(int *)&GLI_g_ClipYMax) {
        float y0_orig = param_1[1];
        float ymin_v;
        v_top = (*(float *)((unsigned char *)GLI_g_GliBigGlobals + CTX_YMIN_OFFSET) - param_1[9]) / (y0_orig - param_1[9]);
        ymin_v = *(float *)((unsigned char *)GLI_g_GliBigGlobals + CTX_YMIN_OFFSET);
        param_1[5] = ymin_v;
        param_1[1] = ymin_v;
    } else {
        v_top = 1.0f;
    }

    if (*(int *)&param_1[9] > *(int *)&GLI_g_ClipYMin) {
        float ymax_v;
        v_bot = ((*(float *)((unsigned char *)GLI_g_GliBigGlobals + CTX_YMAX_OFFSET) - param_1[9]) * v_top) / (param_1[1] - param_1[9]);
        ymax_v = *(float *)((unsigned char *)GLI_g_GliBigGlobals + CTX_YMAX_OFFSET);
        param_1[0xd] = ymax_v;
        param_1[9]   = ymax_v;
    } else {
        v_bot = 0.0f;
    }

    *(float *) GLI_g_pStagingCursor          = param_1[0];
    *(float *)(GLI_g_pStagingCursor + 0x04)  = param_1[1];
    *(float *)(GLI_g_pStagingCursor + 0x20)  = param_1[4];
    *(float *)(GLI_g_pStagingCursor + 0x24)  = param_1[5];
    *(float *)(GLI_g_pStagingCursor + 0x40)  = param_1[8];
    *(float *)(GLI_g_pStagingCursor + 0x44)  = param_1[9];
    *(float *)(GLI_g_pStagingCursor + 0x60)  = param_1[0xc];
    *(float *)(GLI_g_pStagingCursor + 0x64)  = param_1[0xd];

    *(float *)(GLI_g_pStagingCursor + 0x0c) = param_2;
    *(float *)(GLI_g_pStagingCursor + 0x08) = param_2;
    *(float *)(GLI_g_pStagingCursor + 0x2c) = param_2;
    *(float *)(GLI_g_pStagingCursor + 0x28) = param_2;
    *(float *)(GLI_g_pStagingCursor + 0x4c) = param_2;
    *(float *)(GLI_g_pStagingCursor + 0x48) = param_2;
    *(float *)(GLI_g_pStagingCursor + 0x6c) = param_2;
    *(float *)(GLI_g_pStagingCursor + 0x68) = param_2;

    *(unsigned int *)(GLI_g_pStagingCursor + 0x10) = *(unsigned int *)((unsigned char *)param_3 + CTX_DIFFUSE_OFFSET);
    *(unsigned int *)(GLI_g_pStagingCursor + 0x30) = *(unsigned int *)((unsigned char *)param_3 + CTX_DIFFUSE_OFFSET);
    *(unsigned int *)(GLI_g_pStagingCursor + 0x50) = *(unsigned int *)((unsigned char *)param_3 + CTX_DIFFUSE_OFFSET);
    *(unsigned int *)(GLI_g_pStagingCursor + 0x70) = *(unsigned int *)((unsigned char *)param_3 + CTX_DIFFUSE_OFFSET);

    if (*(int *)((unsigned char *)param_3 + CTX_FOG_ALPHA_FLAG) != 0) {
        uVar8 = 0xff000000u;
    } else {
        uVar8 = *(unsigned int *)((unsigned char *)param_3 + CTX_SPECULAR_OFFSET);
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

        GLI_SendSpriteTrianglesViaClip((unsigned int *)param_1, param_3);
    }
}
