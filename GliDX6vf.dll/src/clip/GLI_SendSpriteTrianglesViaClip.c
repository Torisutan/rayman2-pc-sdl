#include "GliDX6vf/GliDX6vf.h"
#include "GliDX6vf/GliDX6vf_State.h"
#include "GliDX6vf/clip_staging.h"

#define FUN10006290_CTX_FLAGS1_OFFSET   0x10
#define FUN10006290_CTX_FOG_ENABLE      0x2d3f4
#define FUN10006290_CTX_DIFFUSE_OFFSET  0x2d3d4

void __cdecl GLI_SendSpriteTrianglesViaClip(unsigned int *param_1, int param_2)
{

    short idx[6];

    float uv_vals[6];

    idx[0] = 0;  idx[1] = 1;  idx[2] = 2;
    idx[3] = 3;  idx[4] = 2;  idx[5] = 0;

    *(unsigned int *)((unsigned char *)param_2 + FUN10006290_CTX_FLAGS1_OFFSET) |= 2u;

    GLI_ResetClipPipeline();

    if (*(int *)((unsigned char *)GLI_g_GliBigGlobals + FUN10006290_CTX_FOG_ENABLE) != 0) {
        GLI_g_PipelineLabels[GLI_g_PipelineCount] = GLI_PL_CopyFog;
        GLI_g_PipelineCount++;
    }

    GLI_g_PipelineLabels[GLI_g_PipelineCount++] = GLI_PL_BackfaceCull;
    GLI_g_PipelineLabels[GLI_g_PipelineCount++] = GLI_PL_XYGate;
    GLI_g_PipelineLabels[GLI_g_PipelineCount++] = GLI_PL_ZNearClip;
    GLI_g_PipelineLabels[GLI_g_PipelineCount++] = GLI_PL_XMaxClip;
    GLI_g_PipelineLabels[GLI_g_PipelineCount++] = GLI_PL_XMinClip;
    GLI_g_PipelineLabels[GLI_g_PipelineCount++] = GLI_PL_YMinClip;
    GLI_g_PipelineLabels[GLI_g_PipelineCount++] = GLI_PL_YMaxClip;
    GLI_g_PipelineLabels[GLI_g_PipelineCount++] = GLI_PL_CopyUV;

    GLI_AppendTrianglePipelineTerminal();

    GLI_g_PipelineLabels[GLI_g_PipelineCount] = GLI_PipelineTerminal;
    GLI_g_PipelineCount++;

    GLI_ResetStagingCursor();

    *(unsigned int *) GLI_g_pStagingCursor          = param_1[0];
    *(unsigned int *)(GLI_g_pStagingCursor + 0x04)  = param_1[1];
    *(unsigned int *)(GLI_g_pStagingCursor + 0x0c)  = param_1[2];
    *(unsigned int *)(GLI_g_pStagingCursor + 0x08)  =
        *(unsigned int *)(GLI_g_pStagingCursor + 0x0c);

    *(unsigned int *)(GLI_g_pStagingCursor + 0x20)  = param_1[4];
    *(unsigned int *)(GLI_g_pStagingCursor + 0x24)  = param_1[5];
    *(unsigned int *)(GLI_g_pStagingCursor + 0x2c)  = param_1[6];
    *(unsigned int *)(GLI_g_pStagingCursor + 0x28)  =
        *(unsigned int *)(GLI_g_pStagingCursor + 0x2c);

    *(unsigned int *)(GLI_g_pStagingCursor + 0x40)  = param_1[8];
    *(unsigned int *)(GLI_g_pStagingCursor + 0x44)  = param_1[9];
    *(unsigned int *)(GLI_g_pStagingCursor + 0x4c)  = param_1[10];
    *(unsigned int *)(GLI_g_pStagingCursor + 0x48)  =
        *(unsigned int *)(GLI_g_pStagingCursor + 0x4c);

    *(unsigned int *)(GLI_g_pStagingCursor + 0x10)  =
        *(unsigned int *)((unsigned char *)param_2 + FUN10006290_CTX_DIFFUSE_OFFSET);
    *(unsigned int *)(GLI_g_pStagingCursor + 0x30)  =
        *(unsigned int *)((unsigned char *)param_2 + FUN10006290_CTX_DIFFUSE_OFFSET);
    *(unsigned int *)(GLI_g_pStagingCursor + 0x50)  =
        *(unsigned int *)((unsigned char *)param_2 + FUN10006290_CTX_DIFFUSE_OFFSET);
    GLI_g_pUVSrc[0] = &uv_vals[0];
    GLI_g_pUVSrc[1] = &uv_vals[2];
    GLI_g_pUVSrc[2] = &uv_vals[4];

    uv_vals[0] = 1.0f;  uv_vals[1] = 1.0f;
    uv_vals[2] = 0.0f;  uv_vals[3] = 1.0f;
    uv_vals[4] = 0.0f;  uv_vals[5] = 0.0f;

    GLI_g_pIndexArray    = idx;
    GLI_g_pPipelineCursor = &GLI_g_PipelineLabels[0];
    (*GLI_g_PipelineLabels[0])();

    *(unsigned int *) GLI_g_pStagingCursor          = param_1[12];
    *(unsigned int *)(GLI_g_pStagingCursor + 0x04)  = param_1[13];
    *(unsigned int *)(GLI_g_pStagingCursor + 0x0c)  = param_1[14];
    *(unsigned int *)(GLI_g_pStagingCursor + 0x08)  =
        *(unsigned int *)(GLI_g_pStagingCursor + 0x0c);

    *(unsigned int *)(GLI_g_pStagingCursor + 0x20)  = param_1[8];
    *(unsigned int *)(GLI_g_pStagingCursor + 0x24)  = param_1[9];
    *(unsigned int *)(GLI_g_pStagingCursor + 0x2c)  = param_1[10];
    *(unsigned int *)(GLI_g_pStagingCursor + 0x28)  =
        *(unsigned int *)(GLI_g_pStagingCursor + 0x2c);

    *(unsigned int *)(GLI_g_pStagingCursor + 0x40)  = param_1[0];
    *(unsigned int *)(GLI_g_pStagingCursor + 0x44)  = param_1[1];
    *(unsigned int *)(GLI_g_pStagingCursor + 0x4c)  = param_1[2];
    *(unsigned int *)(GLI_g_pStagingCursor + 0x48)  =
        *(unsigned int *)(GLI_g_pStagingCursor + 0x4c);

    *(unsigned int *)(GLI_g_pStagingCursor + 0x10)  =
        *(unsigned int *)((unsigned char *)param_2 + FUN10006290_CTX_DIFFUSE_OFFSET);
    *(unsigned int *)(GLI_g_pStagingCursor + 0x30)  =
        *(unsigned int *)((unsigned char *)param_2 + FUN10006290_CTX_DIFFUSE_OFFSET);
    *(unsigned int *)(GLI_g_pStagingCursor + 0x50)  =
        *(unsigned int *)((unsigned char *)param_2 + FUN10006290_CTX_DIFFUSE_OFFSET);
    GLI_g_pUVSrc[0] = &uv_vals[0];
    GLI_g_pUVSrc[1] = &uv_vals[2];
    GLI_g_pUVSrc[2] = &uv_vals[4];

    uv_vals[0] = 1.0f;  uv_vals[1] = 0.0f;
    uv_vals[2] = 0.0f;  uv_vals[3] = 0.0f;
    uv_vals[4] = 1.0f;  uv_vals[5] = 1.0f;

    GLI_g_pIndexArray += 3;

    GLI_g_pPipelineCursor = &GLI_g_PipelineLabels[0];
    (*GLI_g_PipelineLabels[0])();

    GLI_FlushStagedPrimitives();
}
