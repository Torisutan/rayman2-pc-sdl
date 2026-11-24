#include "GliDX6vf/GliDX6vf.h"
#include "GliDX6vf/GliDX6vf_State.h"
#include "GliDX6vf/GLI_DX6Constants.h"

#define GLI_ELEMENT_FLAGS_OFFSET 0x10
#define GLI_ELEMENT_STATE_OFFSET 0x18
#define GLI_ELEMENT_TEXTURE_SLOT_OFFSET 0x2d3b4
#define GLI_ELEMENT_DEFER_OPAQUE_OFFSET 0x2d3f8
#define GLI_ELEMENT_ALPHA_OFFSET 0x2d408
#define GLI_TEXTURE_D3D_HANDLE_OFFSET 0x10
#define GLI_TEXTURE_FLAGS_OFFSET 0x14
#define GLI_TEXTURE_ADDRESS_OFFSET 0x45

static __inline void GLI_SetAlphaTestMode(unsigned char mode)
{
    if (GLI_g_TextureStageColorOp != mode) {
        if (GLI_g_TextureStageColorOp == 2) {
            GLI_SetBeginSceneOpaqueTextureState();
        }
        GLI_g_pDirect3DDevice3->lpVtbl->SetRenderState(GLI_g_pDirect3DDevice3, GLI_D3DRS_ALPHATESTENABLE, mode == 5);
        GLI_g_TextureStageColorOp = mode;
    }
}

static __inline void GLI_SetModulatedAlphaState(void)
{
    GLI_g_pDirect3DDevice3->lpVtbl->SetRenderState(GLI_g_pDirect3DDevice3, GLI_D3DRS_ALPHATESTENABLE, 0);
    GLI_g_pDirect3DDevice3->lpVtbl->SetRenderState(GLI_g_pDirect3DDevice3, GLI_D3DRS_ALPHABLENDENABLE, 1);
    GLI_g_pDirect3DDevice3->lpVtbl->SetRenderState(GLI_g_pDirect3DDevice3, GLI_D3DRS_SRCBLEND, GLI_D3DBLEND_SRCALPHA);
    GLI_g_pDirect3DDevice3->lpVtbl->SetRenderState(GLI_g_pDirect3DDevice3, GLI_D3DRS_DESTBLEND, GLI_D3DBLEND_INVSRCALPHA);
    GLI_g_pDirect3DDevice3->lpVtbl->SetTextureStageState(GLI_g_pDirect3DDevice3, 0, GLI_D3DTSS_COLOROP, GLI_D3DTOP_MODULATE);
    GLI_g_pDirect3DDevice3->lpVtbl->SetTextureStageState(GLI_g_pDirect3DDevice3, 0, GLI_D3DTSS_COLORARG1, GLI_D3DTA_TEXTURE);
    GLI_g_pDirect3DDevice3->lpVtbl->SetTextureStageState(GLI_g_pDirect3DDevice3, 0, GLI_D3DTSS_COLORARG2, GLI_D3DTA_DIFFUSE);
    GLI_g_pDirect3DDevice3->lpVtbl->SetTextureStageState(GLI_g_pDirect3DDevice3, 0, GLI_D3DTSS_ALPHAOP, GLI_D3DTOP_MODULATE);
    GLI_g_pDirect3DDevice3->lpVtbl->SetTextureStageState(GLI_g_pDirect3DDevice3, 0, GLI_D3DTSS_ALPHAARG1, GLI_D3DTA_TEXTURE);
    GLI_g_pDirect3DDevice3->lpVtbl->SetTextureStageState(GLI_g_pDirect3DDevice3, 0, GLI_D3DTSS_ALPHAARG2, GLI_D3DTA_DIFFUSE);
    GLI_g_TextureStageColorOp = 2;
}

#define M_Big ((unsigned char *)GLI_g_GliBigGlobals)
#define M_Texture (*(unsigned char **)(elem + GLI_ELEMENT_TEXTURE_SLOT_OFFSET))

__declspec(dllexport) void __cdecl GLI_DRV_vDoOpaqueTextureSelection(void *element)
{
    unsigned char *elem;
    int hasTexture;
    unsigned char ucAddressMode;

    elem = (unsigned char *)element;
    if (M_Texture != NULL && *(int *)M_Texture != 0) {
        hasTexture = 1;
    } else {
        hasTexture = 0;
    }

    *(DWORD *)(elem + GLI_ELEMENT_DEFER_OPAQUE_OFFSET) = 0;

    if (*(DWORD *)(M_Big + 0x2d3fc) != 0) {
        *(DWORD *)(M_Big + 0x2d3fc) = 3;
    }
    if ((*(DWORD *)(M_Big + 0x2d3fc) == 0 && hasTexture && (M_Texture[GLI_TEXTURE_FLAGS_OFFSET] & 0x38)) ||
        (!(*(DWORD *)(elem + GLI_ELEMENT_FLAGS_OFFSET) & 8) && *(float *)(elem + GLI_ELEMENT_ALPHA_OFFSET) < 0.98f)) {
        *(DWORD *)(elem + GLI_ELEMENT_STATE_OFFSET) |= 0x8000;
        if (hasTexture && !(M_Texture[GLI_TEXTURE_FLAGS_OFFSET] & 8)) {
            *(DWORD *)(elem + GLI_ELEMENT_DEFER_OPAQUE_OFFSET) = 1;
        }
        return;
    }

    *(DWORD *)(elem + GLI_ELEMENT_STATE_OFFSET) &= 0xffff7fff;

    if (*(DWORD *)(elem + GLI_ELEMENT_FLAGS_OFFSET) & 0x40000000) {
        GLI_EnsureZFuncGreaterequal();
    } else {
        GLI_EnsureZFuncAlways();
    }

    if (*(DWORD *)(M_Big + 0x2d3f4)) {
        if (*(void **)(M_Big + 0x2d3f0) != *(void **)(M_Big + 0x2d3ec)) {
            GLI_UpdateFogParameters(*(float **)(M_Big + 0x2d3ec));
            *(void **)(M_Big + 0x2d3f0) = *(void **)(M_Big + 0x2d3ec);
        }
        if (GLI_g_FogEnableState == 0) {
            GLI_g_pDirect3DDevice3->lpVtbl->SetRenderState(GLI_g_pDirect3DDevice3, GLI_D3DRS_FOGENABLE, 1);
            GLI_g_FogEnableState = 1;
        }
    } else {
        if (GLI_g_FogEnableState != 0) {
            GLI_g_pDirect3DDevice3->lpVtbl->SetRenderState(GLI_g_pDirect3DDevice3, GLI_D3DRS_FOGENABLE, 0);
            GLI_g_FogEnableState = 0;
        }
    }

    if (*(DWORD *)(elem + GLI_ELEMENT_FLAGS_OFFSET) & 0x08000000) {
        GLI_EnsureZWriteEnabled();
    } else {
        GLI_DisableZWriteIfEnabled();
    }

    if (hasTexture) {
        if (*(DWORD *)(M_Texture + GLI_TEXTURE_FLAGS_OFFSET) & 0x100) {
            GLI_SetAlphaTestMode(5);
        } else if (*(BYTE *)(M_Big + 0x2d3fc) & 2) {
            GLI_SetModulatedAlphaState();
        } else {
            GLI_SetAlphaTestMode(4);
        }

        if (M_Texture[GLI_TEXTURE_FLAGS_OFFSET] & 2) {
            GLI_g_pDirect3DDevice3->lpVtbl->SetRenderState(GLI_g_pDirect3DDevice3, GLI_D3DRS_COLORKEYENABLE, 1);
        } else {
            GLI_g_pDirect3DDevice3->lpVtbl->SetRenderState(GLI_g_pDirect3DDevice3, GLI_D3DRS_COLORKEYENABLE, 0);
        }

        ucAddressMode = (unsigned char)(M_Texture[GLI_TEXTURE_ADDRESS_OFFSET] & 3);
        if (ucAddressMode != GLI_g_TextureAddressMode) {
            GLI_g_TextureAddressMode = ucAddressMode;
            switch (ucAddressMode) {
            case 0:
                GLI_g_pDirect3DDevice3->lpVtbl->SetTextureStageState(GLI_g_pDirect3DDevice3, 0, GLI_D3DTSS_ADDRESSU, GLI_D3DTADDRESS_CLAMP);
                GLI_g_pDirect3DDevice3->lpVtbl->SetTextureStageState(GLI_g_pDirect3DDevice3, 0, GLI_D3DTSS_ADDRESSV, GLI_D3DTADDRESS_CLAMP);
                break;
            case 1:
                GLI_g_pDirect3DDevice3->lpVtbl->SetTextureStageState(GLI_g_pDirect3DDevice3, 0, GLI_D3DTSS_ADDRESSU, GLI_D3DTADDRESS_CLAMP);
                GLI_g_pDirect3DDevice3->lpVtbl->SetTextureStageState(GLI_g_pDirect3DDevice3, 0, GLI_D3DTSS_ADDRESSV, GLI_D3DTADDRESS_WRAP);
                break;
            case 2:
                GLI_g_pDirect3DDevice3->lpVtbl->SetTextureStageState(GLI_g_pDirect3DDevice3, 0, GLI_D3DTSS_ADDRESSU, GLI_D3DTADDRESS_WRAP);
                GLI_g_pDirect3DDevice3->lpVtbl->SetTextureStageState(GLI_g_pDirect3DDevice3, 0, GLI_D3DTSS_ADDRESSV, GLI_D3DTADDRESS_CLAMP);
                break;
            case 3:
                GLI_g_pDirect3DDevice3->lpVtbl->SetTextureStageState(GLI_g_pDirect3DDevice3, 0, GLI_D3DTSS_ADDRESSU, GLI_D3DTADDRESS_WRAP);
                GLI_g_pDirect3DDevice3->lpVtbl->SetTextureStageState(GLI_g_pDirect3DDevice3, 0, GLI_D3DTSS_ADDRESSV, GLI_D3DTADDRESS_WRAP);
                break;
            }
        }

        GLI_g_pDirect3DDevice3->lpVtbl->SetTexture(GLI_g_pDirect3DDevice3, 0,
                                                   *(void **)(M_Texture + GLI_TEXTURE_D3D_HANDLE_OFFSET));
    } else {
        GLI_SetAlphaTestMode(4);
        GLI_g_pDirect3DDevice3->lpVtbl->SetTexture(GLI_g_pDirect3DDevice3, 0, NULL);
    }
}

#undef M_Texture
#undef M_Big
