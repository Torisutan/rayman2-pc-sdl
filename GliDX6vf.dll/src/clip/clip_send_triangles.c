#include "GliDX6vf/GliDX6vf.h"
#include "GliDX6vf/GliDX6vf_State.h"
#include "GliDX6vf/clip_staging.h"
#include "GliDX6vf/GLI_DiagLog.h"

#define CTX_FLAGS1_OFFSET   0x10
#define CTX_FLAGS2_OFFSET   0x18
#define CTX_FLAGS3_OFFSET   0x10
#define CTX_FOG_FLAG_OFFSET 0x2d3f4
#define CTX_TEX_PTR_OFFSET  0x2d3b4

__declspec(dllexport) void __cdecl
GLI_DRV_xSendElementTIToClip_TRIANGLES(void *element, void *pGMT)
{
    unsigned char *ctx;
    unsigned int ctx18;
    unsigned char *tex;
    DWORD bitmask;

    GLI_M_DiagCountSend();
    if (GLI_g_ActivateGliFlag && GLI_DiagVerbose()) {
        GLI_DiagLog("send xSendElementTIToClip_TRIANGLES #%lu element=%p pGMT=%p",
                    GLI_g_DiagSendCount, element, pGMT);
    }
    GLI_g_pElementBlock = element;
    GLI_SnapshotClipWindow();
    *(unsigned int *)((unsigned char *)pGMT + 0x10) |= 2u;
    GLI_g_VertexStride = 10.0f;
    GLI_ResetClipPipeline();

    bitmask = GLI_g_PipelineBitmask;

    GLI_g_PipelineLabels[GLI_g_PipelineCount++] = GLI_PL_LoadVertex;
    bitmask |= 0x2u;
    GLI_g_PipelineBitmask = bitmask;

    if ((*(unsigned int *)((unsigned char *)pGMT + 0x10) & 0x400000u) == 0)
        GLI_g_PipelineLabels[GLI_g_PipelineCount] = GLI_PL_LoadColorsFromCtx;
    else
        GLI_g_PipelineLabels[GLI_g_PipelineCount] = GLI_PL_LoadColorsFromElement;
    GLI_g_PipelineCount++;

    ctx = (unsigned char *)GLI_g_GliBigGlobals;

    if (*(int *)(ctx + CTX_FOG_FLAG_OFFSET) != 0) {
        GLI_g_PipelineLabels[GLI_g_PipelineCount++] = GLI_PL_CopyFog;
        bitmask |= 0x1u;
        GLI_g_PipelineBitmask = bitmask;
    }

    if (*(unsigned int *)(ctx + CTX_FLAGS2_OFFSET) & 0x100u) {
        GLI_g_PipelineLabels[GLI_g_PipelineCount++] = GLI_PL_BackfaceCull;
        bitmask |= 0x8u;
        GLI_g_PipelineBitmask = bitmask;
    }

    if ((*(unsigned int *)(ctx + CTX_FLAGS3_OFFSET) & 0x100000u) == 0) {
        GLI_g_PipelineLabels[GLI_g_PipelineCount++] = GLI_PL_CulledVertexCheck;
        bitmask |= 0x10u;
        GLI_g_PipelineBitmask = bitmask;
    }

    ctx18 = *(unsigned int *)(ctx + CTX_FLAGS2_OFFSET);
    {
        unsigned int low_nibble = ctx18 & 0xfu;
        if (low_nibble != 0) {
            if (low_nibble == 0xfu) {

                GLI_g_PipelineLabels[GLI_g_PipelineCount++] = GLI_PL_XYGate;
                bitmask |= 0x20u;
                GLI_g_PipelineBitmask = bitmask;
            } else {

                switch (ctx18 & 3u) {
                case 0:
                    break;
                case 1:
                    GLI_g_PipelineLabels[GLI_g_PipelineCount++] = GLI_PL_XSignCheck;
                    bitmask |= 0x40u;
                    GLI_g_PipelineBitmask = bitmask;
                    break;
                case 2:
                    GLI_g_PipelineLabels[GLI_g_PipelineCount++] = GLI_PL_XGateReject;
                    bitmask |= 0x80u;
                    GLI_g_PipelineBitmask = bitmask;
                    break;
                case 3:
                    GLI_g_PipelineLabels[GLI_g_PipelineCount++] = GLI_PL_XSignAndGate;
                    bitmask |= 0x100u;
                    GLI_g_PipelineBitmask = bitmask;
                    break;
                }

                ctx18 = *(unsigned int *)(ctx + CTX_FLAGS2_OFFSET);
                switch (ctx18 & 0xcu) {
                case 0:
                    break;
                case 4:
                    GLI_g_PipelineLabels[GLI_g_PipelineCount++] = GLI_PL_YGateReject2;
                    bitmask |= 0x200u;
                    GLI_g_PipelineBitmask = bitmask;
                    break;
                case 8:
                    GLI_g_PipelineLabels[GLI_g_PipelineCount++] = GLI_PL_YGateReject;
                    bitmask |= 0x400u;
                    GLI_g_PipelineBitmask = bitmask;
                    break;
                case 0xc:
                    GLI_g_PipelineLabels[GLI_g_PipelineCount++] = GLI_PL_YSignAndGate;
                    bitmask |= 0x800u;
                    GLI_g_PipelineBitmask = bitmask;
                    break;
                }
            }
        }
    }

    if (*(unsigned int *)(ctx + CTX_FLAGS2_OFFSET) & 0x200u) {
        if (*(unsigned int *)(ctx + CTX_FLAGS3_OFFSET) & 0x100000u) {
            GLI_g_PipelineLabels[GLI_g_PipelineCount++] = GLI_PL_FlagClipComposite;
            bitmask |= 0x1000u;
        } else {
            GLI_g_PipelineLabels[GLI_g_PipelineCount++] = GLI_PL_FlagClip;
            bitmask |= 0x2000u;
        }
        GLI_g_PipelineBitmask = bitmask;
    }

    if (*(unsigned int *)(ctx + CTX_FLAGS2_OFFSET) & 0x100u) {
        GLI_g_PipelineLabels[GLI_g_PipelineCount++] = GLI_PL_ZNearClip;
        bitmask |= 0x4000u;
        GLI_g_PipelineBitmask = bitmask;
    }

    if (*(unsigned int *)((unsigned char *)pGMT + 0x10) & 0x400u) {
        unsigned int sel;
        if (*(unsigned int *)((unsigned char *)pGMT + 0x0c) & 0x2000000u)
            sel = *(unsigned int *)((unsigned char *)pGMT + 0x10) & 0x80000u;
        else
            sel = (~*(unsigned int *)((unsigned char *)pGMT + 0x10) >> 0x13) & 1u;
        if (sel != 0) {
            GLI_g_PipelineLabels[GLI_g_PipelineCount++] = GLI_PL_BackfaceCullA;
            bitmask |= 0x8000u;
        } else {
            GLI_g_PipelineLabels[GLI_g_PipelineCount++] = GLI_PL_BackfaceCullB;
            bitmask |= 0x10000u;
        }
        GLI_g_PipelineBitmask = bitmask;
    }

    ctx18 = *(unsigned int *)(ctx + CTX_FLAGS2_OFFSET);
    if (ctx18 & 0xf0u) {

        switch (ctx18 & 0x30u) {
        case 0:
            break;
        case 0x20:
            GLI_g_PipelineLabels[GLI_g_PipelineCount++] = GLI_PL_XMaxClip;
            bitmask |= 0x20000u;
            GLI_g_PipelineBitmask = bitmask;
            break;
        case 0x30:

            GLI_g_PipelineLabels[GLI_g_PipelineCount++] = GLI_PL_XMaxClip;
            bitmask |= 0x20000u;

        case 0x10:
            GLI_g_PipelineLabels[GLI_g_PipelineCount++] = GLI_PL_XMinClip;
            bitmask |= 0x40000u;
            GLI_g_PipelineBitmask = bitmask;
            break;
        }

        ctx18 = *(unsigned int *)(ctx + CTX_FLAGS2_OFFSET);
        switch (ctx18 & 0xc0u) {
        case 0:
            break;
        case 0x40:
            GLI_g_PipelineLabels[GLI_g_PipelineCount++] = GLI_PL_YMaxClip;
            bitmask |= 0x100000u;
            GLI_g_PipelineBitmask = bitmask;
            break;
        case 0xc0:

            GLI_g_PipelineLabels[GLI_g_PipelineCount++] = GLI_PL_YMaxClip;
            bitmask |= 0x100000u;

        case 0x80:
            GLI_g_PipelineLabels[GLI_g_PipelineCount++] = GLI_PL_YMinClip;
            bitmask |= 0x200000u;
            GLI_g_PipelineBitmask = bitmask;
            break;
        }
    }

    tex = *(unsigned char **)((unsigned char *)pGMT + CTX_TEX_PTR_OFFSET);
    if (tex != NULL) {
        if (*(int *)(tex + 0x28) != 0) {
            GLI_g_UVScrollU = *(float *)(tex + 0x20);
            GLI_g_UVScrollV = *(float *)(*(unsigned char **)((unsigned char *)pGMT + CTX_TEX_PTR_OFFSET) + 0x24);
            if (GLI_g_UVScrollU == 0.0f && GLI_g_UVScrollV == 0.0f) {
                GLI_g_PipelineLabels[GLI_g_PipelineCount++] = GLI_PL_CopyUV;
                bitmask |= 0x2000000u;
                GLI_g_PipelineBitmask = bitmask;
            } else {
                GLI_g_PipelineLabels[GLI_g_PipelineCount++] = GLI_PL_CopyUVOffset;
                bitmask |= 0x1000000u;
                GLI_g_PipelineBitmask = bitmask;
            }
        } else {
            GLI_g_PipelineLabels[GLI_g_PipelineCount++] = GLI_PL_CopyUV;
            bitmask |= 0x2000000u;
            GLI_g_PipelineBitmask = bitmask;
        }

        tex = *(unsigned char **)((unsigned char *)pGMT + CTX_TEX_PTR_OFFSET);
        if (*(unsigned char *)(tex + 0x45) & 4u) {
            GLI_g_PipelineLabels[GLI_g_PipelineCount++] = GLI_PL_ScaleU;
        }
        tex = *(unsigned char **)((unsigned char *)pGMT + CTX_TEX_PTR_OFFSET);
        if (*(unsigned char *)(tex + 0x45) & 8u) {
            GLI_g_PipelineLabels[GLI_g_PipelineCount++] = GLI_PL_ScaleV;
        }
    }

    GLI_AppendTrianglePipelineTerminal();
    GLI_g_PipelineLabels[GLI_g_PipelineCount++] = GLI_PipelineTerminal;
    GLI_ResetStagingCursor();

    GLI_g_pIndexArray    = *(short **)((unsigned char *)element + 8);
    GLI_g_pAltIndexArray = *(short **)((unsigned char *)element + 0x0c);

    {
        short tri_count = *(short *)((unsigned char *)element + 4);
        if (tri_count != 0) {
            int n = (int)tri_count;
            do {
                GLI_g_pPipelineCursor = GLI_g_PipelineLabels;
                (*GLI_g_PipelineLabels[0])();
                GLI_g_pIndexArray    = (short *)((unsigned char *)GLI_g_pIndexArray    + 6);
                GLI_g_pAltIndexArray = (short *)((unsigned char *)GLI_g_pAltIndexArray + 6);
            } while (--n != 0);
        }
    }

    GLI_FlushStagedPrimitives();
}

/* 0x100065c0 */
__declspec(dllexport) void __cdecl
GLI_DRV_xSendSingleTriangleToClip_TRIANGLES(void *pVertices, void *pUVSrc, void *pGMT)
{
    unsigned char      *gmt = (unsigned char *)pGMT;
    const unsigned int *src = (const unsigned int *)pVertices;
    short               indices[3];

    GLI_M_DiagCountSend();
    if (GLI_g_ActivateGliFlag && GLI_DiagVerbose()) {
        GLI_DiagLog("send xSendSingleTriangleToClip_TRIANGLES #%lu pVertices=%p pUVSrc=%p pGMT=%p",
                    GLI_g_DiagSendCount, pVertices, pUVSrc, pGMT);
    }
    indices[0] = 0;
    indices[1] = 1;
    indices[2] = 2;

    *(unsigned int *)(gmt + 0x10) |= 2u;

    GLI_ResetClipPipeline();

    if (*(int *)((unsigned char *)GLI_g_GliBigGlobals + CTX_FOG_FLAG_OFFSET) != 0) {
        GLI_g_PipelineLabels[GLI_g_PipelineCount++] = GLI_PL_CopyFog;
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
    GLI_g_PipelineLabels[GLI_g_PipelineCount++] = GLI_PipelineTerminal;

    GLI_ResetStagingCursor();

    *(unsigned int *)(GLI_g_pStagingCursor + 0x00) = src[0];
    *(unsigned int *)(GLI_g_pStagingCursor + 0x04) = src[1];
    *(unsigned int *)(GLI_g_pStagingCursor + 0x0c) = src[2];
    *(unsigned int *)(GLI_g_pStagingCursor + 0x08) = *(unsigned int *)(GLI_g_pStagingCursor + 0x0c);
    *(unsigned int *)(GLI_g_pStagingCursor + 0x20) = src[4];
    *(unsigned int *)(GLI_g_pStagingCursor + 0x24) = src[5];
    *(unsigned int *)(GLI_g_pStagingCursor + 0x2c) = src[6];
    *(unsigned int *)(GLI_g_pStagingCursor + 0x28) = *(unsigned int *)(GLI_g_pStagingCursor + 0x2c);
    *(unsigned int *)(GLI_g_pStagingCursor + 0x40) = src[8];
    *(unsigned int *)(GLI_g_pStagingCursor + 0x44) = src[9];
    *(unsigned int *)(GLI_g_pStagingCursor + 0x4c) = src[10];
    *(unsigned int *)(GLI_g_pStagingCursor + 0x48) = *(unsigned int *)(GLI_g_pStagingCursor + 0x4c);
    *(unsigned int *)(GLI_g_pStagingCursor + 0x10) = 0xffffffffu;
    *(unsigned int *)(GLI_g_pStagingCursor + 0x30) = 0xffffffffu;
    *(unsigned int *)(GLI_g_pStagingCursor + 0x50) = 0xffffffffu;

    GLI_g_pUVSrc[0]       = (float *)pUVSrc;
    GLI_g_pUVSrc[1]       = (float *)((unsigned char *)pUVSrc + 8);
    GLI_g_pUVSrc[2]       = (float *)((unsigned char *)pUVSrc + 0x10);
    GLI_g_pIndexArray     = indices;
    GLI_g_pPipelineCursor = GLI_g_PipelineLabels;

    (*GLI_g_PipelineLabels[0])();
    GLI_FlushStagedPrimitives();
}
