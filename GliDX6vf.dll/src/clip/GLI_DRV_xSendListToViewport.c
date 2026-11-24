#include "GliDX6vf/GliDX6vf.h"
#include "GliDX6vf/GliDX6vf_State.h"
#include "GliDX6vf/clip_staging.h"
#include "GliDX6vf/GLI_DiagLog.h"
#include "GliDX6vf/GLI_DX6Constants.h"

#define CTX_CURRENT_DRAW_MASK_OFFSET   0x10
#define CTX_CURRENT_TEXTURE_OFFSET     0x2d3b4
#define CTX_CURRENT_MATERIAL_OFFSET    0x2d3b8
#define CTX_ZLIST_OFFSET               0x2d3e4
#define CTX_ACTIVE_FOG_OFFSET          0x2d3ec
#define CTX_LAST_FOG_OFFSET            0x2d3f0
#define CTX_FOG_ON_OFFSET              0x2d3f4

#define ZLIST_BUCKET_ARRAY_OFFSET   8
#define ZLIST_SELECTOR_STRIDE       0x4000

#define ZNODE_PAYLOAD    0
#define ZNODE_TEXTURE    1
#define ZNODE_NEXT       2
#define ZNODE_DRAWMASK   3
#define ZNODE_FOG        4
#define ZNODE_MATERIAL   5

#define ZNODE_DRAWMASK_DIRECT_BIT   0x2u

#define GLI_VIEWINGLIST_ALPHA_RESET 0x43000000

#define CTX_DRAW_MASK_ZTEST_BIT     0x40000000u

#define GLI_TEX_AVAILABLE_OFFSET      0x00
#define GLI_TEX_SPECPARAM_OFFSET      0x10
#define GLI_TEX_CAPS_OFFSET           0x14
#define GLI_TEX_CYCLING_MODE_OFFSET   0x45

#define GLI_C_Tex_lNZTexture              0x00000002u
#define GLI_C_Tex_lAddTransparencyTexture 0x00000020u

static void GLI_ApplyViewingListInitialRenderState(void);
static void GLI_ApplyViewingListNodeRenderState(unsigned char *ctx);

#define GLI_CTXP ((unsigned char *)GLI_g_GliBigGlobals)

static void GLI_ApplyViewingListInitialRenderState(void)
{
    GLI_DisableZWriteIfEnabled();
    GLI_g_pDirect3DDevice3->lpVtbl->SetRenderState(GLI_g_pDirect3DDevice3, GLI_D3DRS_ALPHATESTENABLE, 0);
    GLI_g_pDirect3DDevice3->lpVtbl->SetRenderState(GLI_g_pDirect3DDevice3, GLI_D3DRS_ALPHABLENDENABLE, 1);
    GLI_g_pDirect3DDevice3->lpVtbl->SetRenderState(GLI_g_pDirect3DDevice3, GLI_D3DRS_SRCBLEND, GLI_D3DBLEND_SRCALPHA);
}

#undef GLI_CTXP
#define GLI_CTXP ctx

#define M_Big ((unsigned char *)GLI_g_GliBigGlobals)
#define M_Texture (*(unsigned char **)(GLI_CTXP + CTX_CURRENT_TEXTURE_OFFSET))

static void GLI_ApplyViewingListNodeRenderState(unsigned char *ctx)
{
    unsigned char ucCyclingMode;

    if (*(unsigned int *)(GLI_CTXP + CTX_CURRENT_DRAW_MASK_OFFSET) & CTX_DRAW_MASK_ZTEST_BIT) {
        GLI_EnsureZFuncGreaterequal();
    } else {
        GLI_EnsureZFuncAlways();
    }

    if (*(int *)(M_Big + CTX_FOG_ON_OFFSET)) {
        if (*(void **)(M_Big + CTX_LAST_FOG_OFFSET) != *(void **)(M_Big + CTX_ACTIVE_FOG_OFFSET)) {
            GLI_UpdateFogParameters(*(float **)(M_Big + CTX_ACTIVE_FOG_OFFSET));
            *(void **)(M_Big + CTX_LAST_FOG_OFFSET) = *(void **)(M_Big + CTX_ACTIVE_FOG_OFFSET);
        }
        if (GLI_g_FogEnableState == 0) {
            GLI_g_pDirect3DDevice3->lpVtbl->SetRenderState(GLI_g_pDirect3DDevice3,GLI_D3DRS_FOGENABLE, 1);
            GLI_g_FogEnableState = 1;
        }
    } else {
        if (GLI_g_FogEnableState != 0) {
            GLI_g_pDirect3DDevice3->lpVtbl->SetRenderState(GLI_g_pDirect3DDevice3,GLI_D3DRS_FOGENABLE, 0);
            GLI_g_FogEnableState = 0;
        }
    }

    if (M_Texture != NULL && *(int *)(M_Texture + GLI_TEX_AVAILABLE_OFFSET) != 0) {
        if (M_Texture[GLI_TEX_CAPS_OFFSET] & GLI_C_Tex_lAddTransparencyTexture) {

            if (GLI_g_TextureStageColorOp != 1) {
                GLI_g_pDirect3DDevice3->lpVtbl->SetRenderState(GLI_g_pDirect3DDevice3,GLI_D3DRS_DESTBLEND, GLI_D3DBLEND_ONE);
                GLI_g_pDirect3DDevice3->lpVtbl->SetTextureStageState(GLI_g_pDirect3DDevice3,0, GLI_D3DTSS_COLOROP, GLI_D3DTOP_SELECTARG1);
                GLI_g_pDirect3DDevice3->lpVtbl->SetTextureStageState(GLI_g_pDirect3DDevice3,0, GLI_D3DTSS_COLORARG1, GLI_D3DTA_TEXTURE);
                GLI_g_pDirect3DDevice3->lpVtbl->SetTextureStageState(GLI_g_pDirect3DDevice3,0, GLI_D3DTSS_ALPHAOP, GLI_D3DTOP_SELECTARG1);
                GLI_g_pDirect3DDevice3->lpVtbl->SetTextureStageState(GLI_g_pDirect3DDevice3,0, GLI_D3DTSS_ALPHAARG1, GLI_D3DTA_TEXTURE);
                GLI_g_TextureStageColorOp = 1;
            }
            if (GLI_g_FogEnableState != 0) {
                GLI_g_pDirect3DDevice3->lpVtbl->SetRenderState(GLI_g_pDirect3DDevice3,GLI_D3DRS_FOGENABLE, 0);
                GLI_g_FogEnableState = 0;
            }
        } else {

            if (GLI_g_TextureStageColorOp != 2) {
                GLI_g_pDirect3DDevice3->lpVtbl->SetRenderState(GLI_g_pDirect3DDevice3,GLI_D3DRS_DESTBLEND, GLI_D3DBLEND_INVSRCALPHA);
                GLI_g_pDirect3DDevice3->lpVtbl->SetTextureStageState(GLI_g_pDirect3DDevice3,0, GLI_D3DTSS_COLOROP, GLI_D3DTOP_MODULATE);
                GLI_g_pDirect3DDevice3->lpVtbl->SetTextureStageState(GLI_g_pDirect3DDevice3,0, GLI_D3DTSS_COLORARG1, GLI_D3DTA_TEXTURE);
                GLI_g_pDirect3DDevice3->lpVtbl->SetTextureStageState(GLI_g_pDirect3DDevice3,0, GLI_D3DTSS_COLORARG2, GLI_D3DTA_DIFFUSE);
                GLI_g_pDirect3DDevice3->lpVtbl->SetTextureStageState(GLI_g_pDirect3DDevice3,0, GLI_D3DTSS_ALPHAOP, GLI_D3DTOP_MODULATE);
                GLI_g_pDirect3DDevice3->lpVtbl->SetTextureStageState(GLI_g_pDirect3DDevice3,0, GLI_D3DTSS_ALPHAARG1, GLI_D3DTA_TEXTURE);
                GLI_g_pDirect3DDevice3->lpVtbl->SetTextureStageState(GLI_g_pDirect3DDevice3,0, GLI_D3DTSS_ALPHAARG2, GLI_D3DTA_DIFFUSE);
                GLI_g_TextureStageColorOp = 2;
            }
        }

        if (M_Texture[GLI_TEX_CAPS_OFFSET] & GLI_C_Tex_lNZTexture) {
            GLI_g_pDirect3DDevice3->lpVtbl->SetRenderState(GLI_g_pDirect3DDevice3,GLI_D3DRS_COLORKEYENABLE, 1);
        } else {
            GLI_g_pDirect3DDevice3->lpVtbl->SetRenderState(GLI_g_pDirect3DDevice3,GLI_D3DRS_COLORKEYENABLE, 0);
        }

        ucCyclingMode = (unsigned char)(M_Texture[GLI_TEX_CYCLING_MODE_OFFSET] & 3);
        if (ucCyclingMode != GLI_g_TextureAddressMode) {
            GLI_g_TextureAddressMode = ucCyclingMode;
            switch (ucCyclingMode) {
            case 0:
                GLI_g_pDirect3DDevice3->lpVtbl->SetTextureStageState(GLI_g_pDirect3DDevice3,0, GLI_D3DTSS_ADDRESSU, GLI_D3DTADDRESS_CLAMP);
                GLI_g_pDirect3DDevice3->lpVtbl->SetTextureStageState(GLI_g_pDirect3DDevice3,0, GLI_D3DTSS_ADDRESSV, GLI_D3DTADDRESS_CLAMP);
                break;
            case 1:
                GLI_g_pDirect3DDevice3->lpVtbl->SetTextureStageState(GLI_g_pDirect3DDevice3,0, GLI_D3DTSS_ADDRESSU, GLI_D3DTADDRESS_CLAMP);
                GLI_g_pDirect3DDevice3->lpVtbl->SetTextureStageState(GLI_g_pDirect3DDevice3,0, GLI_D3DTSS_ADDRESSV, GLI_D3DTADDRESS_WRAP);
                break;
            case 2:
                GLI_g_pDirect3DDevice3->lpVtbl->SetTextureStageState(GLI_g_pDirect3DDevice3,0, GLI_D3DTSS_ADDRESSU, GLI_D3DTADDRESS_WRAP);
                GLI_g_pDirect3DDevice3->lpVtbl->SetTextureStageState(GLI_g_pDirect3DDevice3,0, GLI_D3DTSS_ADDRESSV, GLI_D3DTADDRESS_CLAMP);
                break;
            case 3:
                GLI_g_pDirect3DDevice3->lpVtbl->SetTextureStageState(GLI_g_pDirect3DDevice3,0, GLI_D3DTSS_ADDRESSU, GLI_D3DTADDRESS_WRAP);
                GLI_g_pDirect3DDevice3->lpVtbl->SetTextureStageState(GLI_g_pDirect3DDevice3,0, GLI_D3DTSS_ADDRESSV, GLI_D3DTADDRESS_WRAP);
                break;
            }
        }

        GLI_g_pDirect3DDevice3->lpVtbl->SetTexture(GLI_g_pDirect3DDevice3,0, *(void **)(M_Texture + GLI_TEX_SPECPARAM_OFFSET));
    } else {

        if (GLI_g_TextureStageColorOp == 1) {
            GLI_g_pDirect3DDevice3->lpVtbl->SetRenderState(GLI_g_pDirect3DDevice3,GLI_D3DRS_DESTBLEND, GLI_D3DBLEND_INVSRCALPHA);
        }
        GLI_g_pDirect3DDevice3->lpVtbl->SetTexture(GLI_g_pDirect3DDevice3,0, NULL);
        GLI_g_TextureStageColorOp = 0;
    }
}

#undef M_Texture
#undef M_Big

#undef GLI_CTXP
#define GLI_CTXP ((unsigned char *)GLI_g_GliBigGlobals)

__declspec(dllexport) void __cdecl GLI_DRV_xSendListToViewport(void *pVpt)
{
    int            selectorByteOffset;
    unsigned int   bucketBaseOffset;
#ifdef GLI_DX6_FIXES

    unsigned long  drainedNodes = 0;
#endif

    (void)pVpt;

    if (*(void **)(GLI_CTXP +CTX_ZLIST_OFFSET) == NULL) {
        GLI_DiagLog("xSendListToViewport SKIP — z-list NULL");
        return;
    }
    GLI_DiagLog("xSendListToViewport ENTER vpt=%p", pVpt);

    *(int  *)(GLI_CTXP +CTX_CURRENT_DRAW_MASK_OFFSET) = -1;
    *(void **)(GLI_CTXP +CTX_CURRENT_TEXTURE_OFFSET)  = NULL;
    GLI_g_SetGlobalAlpha(GLI_VIEWINGLIST_ALPHA_RESET);
    GLI_ResetStagingCursor();
    GLI_ApplyViewingListInitialRenderState();

    selectorByteOffset = 0;
    bucketBaseOffset   = 0;
    do {
        unsigned char *zlist     = *(unsigned char **)(GLI_CTXP +CTX_ZLIST_OFFSET);
        int            minBucket = *(int *)(zlist + selectorByteOffset);
        int            maxBucket = *(int *)(zlist + selectorByteOffset + 4);

        if (maxBucket >= minBucket) {
            unsigned int **bucketArray =
                (unsigned int **)(zlist + ZLIST_BUCKET_ARRAY_OFFSET + bucketBaseOffset);
            unsigned int **pBucket;

            for (pBucket = bucketArray + minBucket; pBucket <= bucketArray + maxBucket; pBucket++) {
                unsigned int *node;

                for (node = *pBucket; node != NULL; node = (unsigned int *)node[ZNODE_NEXT]) {
#ifdef GLI_DX6_FIXES
                    ++drainedNodes;

                    if (GLI_g_ActivateGliFlag && GLI_DiagVerbose()) {
                        GLI_DiagLog("drain n=%lu node=%p next=%p pay=%p tex=%p dm=0x%x",
                                    drainedNodes, (void *)node, (void *)node[ZNODE_NEXT],
                                    (void *)node[ZNODE_PAYLOAD], (void *)node[ZNODE_TEXTURE],
                                    node[ZNODE_DRAWMASK]);
                    }
#endif

                    if (*(unsigned int *)(GLI_CTXP +CTX_CURRENT_TEXTURE_OFFSET)   != node[ZNODE_TEXTURE]  ||
                        *(unsigned int *)(GLI_CTXP +CTX_CURRENT_DRAW_MASK_OFFSET) != node[ZNODE_DRAWMASK] ||
                        *(unsigned int *)(GLI_CTXP +CTX_LAST_FOG_OFFSET)          != node[ZNODE_FOG]) {

                        GLI_FlushStagedPrimitives();

                        *(unsigned int *)(GLI_CTXP +CTX_CURRENT_DRAW_MASK_OFFSET) = node[ZNODE_DRAWMASK];
                        *(unsigned int *)(GLI_CTXP +CTX_CURRENT_TEXTURE_OFFSET)   = node[ZNODE_TEXTURE];
                        *(unsigned int *)(GLI_CTXP +CTX_CURRENT_MATERIAL_OFFSET)  = node[ZNODE_MATERIAL];

                        if (node[ZNODE_FOG] != 0) {
                            *(unsigned int *)(GLI_CTXP +CTX_FOG_ON_OFFSET)     = 1;
                            *(unsigned int *)(GLI_CTXP +CTX_ACTIVE_FOG_OFFSET) = node[ZNODE_FOG];
                        } else {
                            *(unsigned int *)(GLI_CTXP +CTX_LAST_FOG_OFFSET)   = 0;
                            *(unsigned int *)(GLI_CTXP +CTX_ACTIVE_FOG_OFFSET) = 0;
                            *(unsigned int *)(GLI_CTXP +CTX_FOG_ON_OFFSET)     = 0;
                        }

                        GLI_ResetStagingCursor();
                        GLI_ApplyViewingListNodeRenderState(GLI_CTXP);
                    }

                    if (node[ZNODE_DRAWMASK] & ZNODE_DRAWMASK_DIRECT_BIT) {
                        GLI_CopyStagedBlock60((const void *)node[ZNODE_PAYLOAD]);
                    } else {
                        GLI_CopyStagedBlockC0((const void *)node[ZNODE_PAYLOAD]);
                    }
                }

                *pBucket = NULL;
            }
        }

        bucketBaseOffset   += ZLIST_SELECTOR_STRIDE;
        selectorByteOffset += 4;
    } while (bucketBaseOffset < ZLIST_SELECTOR_STRIDE);

#ifdef GLI_DX6_FIXES
    GLI_DiagLog("xSendListToViewport drained nodes=%lu -> final flush", drainedNodes);
#endif
    GLI_FlushStagedPrimitives();
    GLI_DRV_xClearViewingList();
    GLI_DiagLog("xSendListToViewport EXIT");
}
