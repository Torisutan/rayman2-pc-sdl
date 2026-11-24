#include "GliDX6vf/GliDX6vf.h"
#include "GliDX6vf/GliDX6vf_State.h"
#include "GliDX6vf/clip_staging.h"
#include "GliDX6vf/GLI_DX6Constants.h"
#include <string.h>

#define GLI_CTX_FLAGS1_OFFSET          0x10
#define GLI_CTX_FLAGS2_OFFSET          0x18
#define GLI_CTX_CLIP_EDGE_A_OFFSET     0x2d3a4
#define GLI_CTX_CLIP_EDGE_B_OFFSET     0x2d3a8
#define GLI_CTX_CLIP_EDGE_C_OFFSET     0x2d3ac
#define GLI_CTX_CLIP_EDGE_D_OFFSET     0x2d3b0
#define GLI_CTX_NODE_FIELD_B4_OFFSET   0x2d3b4

#define GLI_CTX_NODE_FIELD_B8_OFFSET   0x2d3b8
#define GLI_CTX_LIST_BLOCK_PTR_OFFSET  0x2d3e4
#define GLI_CTX_LIST_SELECTOR_OFFSET   0x2d3e8
#define GLI_CTX_NODE_FIELD_EC_OFFSET   0x2d3ec

#define GLI_LIST_NODE_CURSOR_OFFSET    0x4008
#define GLI_LIST_ALLOC_CURSOR_OFFSET   0x10010
#define GLI_LIST_ALLOC_BOUND_OFFSET    0x10014

#define GLI_BUCKET_KEY_SUBTRACT        0x2c8
#define GLI_BUCKET_KEY_MASK            0x0fff
#define GLI_BUCKET_SELECTOR_STRIDE     0x1000
#define GLI_BUCKET_ARRAY_BASE_OFFSET   8
#define GLI_LIST_NODE_SIZE             0x18

#define GLI_LIST_SPRITE_BOUND_ADDEND   0x2ff80  /* 0x10001340 */
#define GLI_LIST_TRIANGLE_BOUND_ADDEND 0x2ffa0  /* 0x100014f0 */

#define GLI_VERTEX_STRIDE_SHIFT 5

#define GLI_CTX_FLAGS2_LIST_MODE_BIT   0x8000u
#define GLI_CTX_FLAGS1_CLEAR_BIT2_MASK 0xfffffffdu
#define GLI_CTX_FLAGS1_NODE_FLAGS_MASK 0xf7ffffffu
#define GLI_CTX_FLAGS2_LOWNIBBLE_MASK  0x0fu

static void __cdecl GLI_InsertListNode(unsigned int param_1);
void __cdecl GLI_PipelineTerminal(void);  /* 0x10006290 */
static void __cdecl GLI_PipelineAdvanceCursor60(void);
static void __cdecl GLI_PipelineListAppend(void);

#define GLI_BIG_BYTES()        ((unsigned char *)GLI_g_GliBigGlobals)
#define GLI_LIST_BLOCK_BASE()  (*(unsigned char **)(GLI_BIG_BYTES() + GLI_CTX_LIST_BLOCK_PTR_OFFSET))
#define GLI_LIST_SELECTOR()    (*(int *)(GLI_BIG_BYTES() + GLI_CTX_LIST_SELECTOR_OFFSET))
#define GLI_LIST_NODE_PTR()    (*(unsigned char **)(GLI_LIST_BLOCK_BASE() + GLI_LIST_NODE_CURSOR_OFFSET))
#define GLI_BUCKET_SLOT_PTR(b) (GLI_LIST_BLOCK_BASE() + GLI_BUCKET_ARRAY_BASE_OFFSET + \
                                (GLI_LIST_SELECTOR() * GLI_BUCKET_SELECTOR_STRIDE + (int)(b)) * 4)

static void __cdecl GLI_InsertListNode(unsigned int param_1)
{
    unsigned int bucket;
    unsigned int *p_min, *p_max;

    bucket = ((param_1 >> 16) - GLI_BUCKET_KEY_SUBTRACT) & GLI_BUCKET_KEY_MASK;
    bucket &= 0xffff;

    p_min = (unsigned int *)(GLI_LIST_BLOCK_BASE() + GLI_LIST_SELECTOR() * 4);
    if ((int)*p_min > (int)bucket) {
        *p_min = bucket;
    }

    p_max = (unsigned int *)(GLI_LIST_BLOCK_BASE() + 4 + GLI_LIST_SELECTOR() * 4);
    if ((int)*p_max < (int)bucket) {
        *p_max = bucket;
    }

    *(unsigned int *)(GLI_LIST_NODE_PTR() + 0x00) = *(unsigned int *)(GLI_LIST_BLOCK_BASE() + GLI_LIST_ALLOC_CURSOR_OFFSET);
    *(unsigned int *)(GLI_LIST_NODE_PTR() + 0x04) = *(unsigned int *)(GLI_BIG_BYTES() + GLI_CTX_NODE_FIELD_B4_OFFSET);
    *(unsigned int *)(GLI_LIST_NODE_PTR() + 0x14) = *(unsigned int *)(GLI_BIG_BYTES() + GLI_CTX_NODE_FIELD_B8_OFFSET);
    *(unsigned int *)(GLI_LIST_NODE_PTR() + 0x0c) = *(unsigned int *)(GLI_BIG_BYTES() + GLI_CTX_FLAGS1_OFFSET) & GLI_CTX_FLAGS1_NODE_FLAGS_MASK;
    *(unsigned int *)(GLI_LIST_NODE_PTR() + 0x10) = *(unsigned int *)(GLI_BIG_BYTES() + GLI_CTX_NODE_FIELD_EC_OFFSET);

    *(unsigned int *)(GLI_LIST_NODE_PTR() + 0x08) = *(unsigned int *)GLI_BUCKET_SLOT_PTR(bucket);
    *(unsigned int *)GLI_BUCKET_SLOT_PTR(bucket) = (unsigned int)GLI_LIST_NODE_PTR();

    *(int *)(GLI_LIST_BLOCK_BASE() + GLI_LIST_NODE_CURSOR_OFFSET) += GLI_LIST_NODE_SIZE;
}

/* 0x10002750 */
void __cdecl GLI_PipelineTerminal(void)
{

}

/* 0x100014e0 */
static void __cdecl GLI_PipelineAdvanceCursor60(void)
{
    GLI_g_pStagingCursor += 0x60;
}

/* 0x100014f0 */
static void __cdecl GLI_PipelineListAppend(void)
{
    unsigned char *ctx        = (unsigned char *)GLI_g_GliBigGlobals;
    unsigned char *list_block = *(unsigned char **)(ctx + GLI_CTX_LIST_BLOCK_PTR_OFFSET);
    unsigned char *alloc_cur, *alloc_bnd, *limit;
    unsigned int   v0, v1, v2, key;
    unsigned char *stage;

#ifdef GLI_DX6_FIXES

    if (list_block == NULL) {
        GLI_g_pStagingCursor += 0x60;
        return;
    }
#endif

    alloc_bnd  = *(unsigned char **)(list_block + GLI_LIST_ALLOC_BOUND_OFFSET);
    limit      = alloc_bnd + GLI_LIST_TRIANGLE_BOUND_ADDEND;
    alloc_cur  = *(unsigned char **)(list_block + GLI_LIST_ALLOC_CURSOR_OFFSET);

    if (alloc_cur > limit) {
        return;
    }

    memcpy(alloc_cur, GLI_g_pStagingCursor, 0x60);

    stage = GLI_g_pStagingCursor;
    v0  = *(unsigned int *)(stage + 0x0c);
    v1  = *(unsigned int *)(stage + 0x2c);
    key = v0;
    if (v0 >= v1) key = v1;
    v2  = *(unsigned int *)(stage + 0x4c);
    if (key > v2) key = v2;

    GLI_InsertListNode(key);

    *(int *)(*(unsigned char **)((unsigned char *)GLI_g_GliBigGlobals + GLI_CTX_LIST_BLOCK_PTR_OFFSET)
             + GLI_LIST_ALLOC_CURSOR_OFFSET) += 0x60;
}

/* 0x100012f0 */
void __cdecl GLI_ResetStagingCursor(void)
{
    GLI_g_pStagingCursor = GLI_g_StagingBuffer;
}

/* 0x10001300 */
void __cdecl GLI_FlushStagedPrimitives(void)
{
    int count = (int)(GLI_g_pStagingCursor - GLI_g_StagingBuffer) >> GLI_VERTEX_STRIDE_SHIFT;
    if (count != 0) {
        GLI_g_pDirect3DDevice3->lpVtbl->DrawPrimitive(
            GLI_g_pDirect3DDevice3,
            GLI_D3DPT_TRIANGLELIST,
            GLI_D3DFVF_TLVERTEX,
            GLI_g_StagingBuffer,
            (DWORD)count,
            GLI_g_DrawFlags);
    }
}

/* 0x100013f0 */
void __cdecl GLI_DrawStagedLine(void)
{
    GLI_g_pDirect3DDevice3->lpVtbl->DrawPrimitive(
        GLI_g_pDirect3DDevice3,
        GLI_D3DPT_LINELIST,
        GLI_D3DFVF_TLVERTEX,
        GLI_g_pStagingCursor,
        2,
        GLI_D3DDP_DONOTLIGHT);
}

/* 0x10001410 */
void __cdecl GLI_CopyStagedBlock60(const void *src)
{
    memcpy(GLI_g_pStagingCursor, src, 0x60);
    GLI_g_pStagingCursor += 0x60;
}

/* 0x10001440 */
void __cdecl GLI_CopyStagedBlockC0(const void *src)
{

    memcpy(GLI_g_pStagingCursor, src, 0x80);

    memcpy(GLI_g_pStagingCursor + 0x80, (const unsigned char *)src + 0x40, 0x20);

    memcpy(GLI_g_pStagingCursor + 0xa0, src, 0x20);

    GLI_g_pStagingCursor += 0xc0;
}

/* 0x10001340 */
void __cdecl GLI_FinalizeSpriteStaging(void)
{
    unsigned char *ctx = (unsigned char *)GLI_g_GliBigGlobals;

    if ((*(unsigned int *)(ctx + GLI_CTX_FLAGS2_OFFSET) & GLI_CTX_FLAGS2_LIST_MODE_BIT) != 0) {

        *(unsigned int *)(ctx + GLI_CTX_FLAGS1_OFFSET) &= GLI_CTX_FLAGS1_CLEAR_BIT2_MASK;
        {
            unsigned char *alloc_cur, *alloc_bnd;

#ifdef GLI_DX6_FIXES

            if (GLI_LIST_BLOCK_BASE() == NULL) {
                memcpy(GLI_g_pStagingCursor + 0x80, GLI_g_pStagingCursor + 0x40, 0x20);
                memcpy(GLI_g_pStagingCursor + 0xa0, GLI_g_pStagingCursor, 0x20);
                GLI_g_pStagingCursor += 0xc0;
                return;
            }
#endif

            alloc_bnd   = *(unsigned char **)(GLI_LIST_BLOCK_BASE() + GLI_LIST_ALLOC_BOUND_OFFSET);
            alloc_cur   = *(unsigned char **)(GLI_LIST_BLOCK_BASE() + GLI_LIST_ALLOC_CURSOR_OFFSET);

            if (alloc_cur <= alloc_bnd + GLI_LIST_SPRITE_BOUND_ADDEND) {
                memcpy(alloc_cur, GLI_g_pStagingCursor, 0x80);

                GLI_InsertListNode(*(unsigned int *)(GLI_g_pStagingCursor + 0x0c));

                *(int *)(GLI_LIST_BLOCK_BASE() + GLI_LIST_ALLOC_CURSOR_OFFSET) += 0x80;
            }
        }
    } else {

        memcpy(GLI_g_pStagingCursor + 0x80, GLI_g_pStagingCursor + 0x40, 0x20);
        memcpy(GLI_g_pStagingCursor + 0xa0, GLI_g_pStagingCursor, 0x20);
        GLI_g_pStagingCursor += 0xc0;
    }
}

/* 0x100014a0 */
void __cdecl GLI_AppendTrianglePipelineTerminal(void)
{
    DWORD          count;

    if ((*(unsigned int *)((unsigned char *)GLI_g_GliBigGlobals + GLI_CTX_FLAGS2_OFFSET) & GLI_CTX_FLAGS2_LIST_MODE_BIT) != 0) {
        count = GLI_g_PipelineCount;
        GLI_g_PipelineLabels[count] = GLI_PipelineListAppend;
    } else {
        count = GLI_g_PipelineCount;
        GLI_g_PipelineLabels[count] = GLI_PipelineAdvanceCursor60;
    }
    GLI_g_PipelineCount = count + 1;
}

/* 0x10002760 */
void __cdecl GLI_ResetClipPipeline(void)
{
    GLI_g_PipelineLabels[0] = GLI_PipelineTerminal;
    GLI_g_PipelineBitmask   = 0;
    GLI_g_PipelineCount     = 0;
}

/* 0x10001560 */
void __cdecl GLI_SnapshotClipWindow(void)
{
    unsigned char *ctx = (unsigned char *)GLI_g_GliBigGlobals;
    unsigned int   old_flags;

    GLI_g_ClipXGate = *(float *)(ctx + GLI_CTX_CLIP_EDGE_B_OFFSET);
    GLI_g_ClipYGate = *(float *)(ctx + GLI_CTX_CLIP_EDGE_D_OFFSET);
    GLI_g_ClipXMin = *(float *)(ctx + GLI_CTX_CLIP_EDGE_A_OFFSET);
    GLI_g_ClipYMax = *(float *)(ctx + GLI_CTX_CLIP_EDGE_C_OFFSET);
    GLI_g_ClipXMax = *(float *)(ctx + GLI_CTX_CLIP_EDGE_B_OFFSET);
    GLI_g_ClipYMin = *(float *)(ctx + GLI_CTX_CLIP_EDGE_D_OFFSET);

    old_flags = *(unsigned int *)(ctx + GLI_CTX_FLAGS2_OFFSET);
    *(unsigned int *)(ctx + GLI_CTX_FLAGS2_OFFSET) =
        old_flags | ((old_flags & GLI_CTX_FLAGS2_LOWNIBBLE_MASK) << 4);
}
