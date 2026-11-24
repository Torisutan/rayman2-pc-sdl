#include "GliDX6vf/GliDX6vf.h"

#include <string.h>

#define GLI_SURFACE_DESC_SIZE 0x7c
#define GLI_DESC_BITCOUNT_OFFSET 0x54
#define GLI_DESC_MASK_58_OFFSET 0x58
#define GLI_DESC_MASK_5C_OFFSET 0x5c
#define GLI_DESC_MASK_60_OFFSET 0x60
#define GLI_DDCKEY_SRCBLT 0x00000008u

typedef struct GLI_SurfaceVtbl_ForColorKey {
    void *reserved0[22];
    HRESULT (STDMETHODCALLTYPE *GetSurfaceDesc)(void *self, void *desc);
    void *reserved1[6];
    HRESULT (STDMETHODCALLTYPE *SetColorKey)(void *self, DWORD flags, DWORD *colorKey);
} GLI_SurfaceVtbl_ForColorKey;

typedef struct GLI_Surface_ForColorKey {
    const GLI_SurfaceVtbl_ForColorKey *lpVtbl;
} GLI_Surface_ForColorKey;

int __cdecl GLI_lApplyTextureColorKey(void *texture, void *surface)
{
    unsigned char desc[GLI_SURFACE_DESC_SIZE];
    GLI_Surface_ForColorKey *ddSurface;
    DWORD mask58;
    DWORD mask5c;
    DWORD mask60;
    int shift58;
    int adjust58;
    int shift5c;
    int adjust5c;
    int shift60;
    int adjust60;
    DWORD keyPair[2];
    DWORD key;
    DWORD *fill;
    int fillCount;
    signed char *tex;
    HRESULT hr;

    fill = (DWORD *)desc;
    for (fillCount = 0x1f; fillCount != 0; fillCount = fillCount - 1) {
        *fill = 0;
        fill = fill + 1;
    }
    *(DWORD *)desc = GLI_SURFACE_DESC_SIZE;

    ddSurface = (GLI_Surface_ForColorKey *)surface;
    if (ddSurface->lpVtbl->GetSurfaceDesc(ddSurface, desc) != 0) {
        return 0;
    }

    if (*(DWORD *)(desc + GLI_DESC_BITCOUNT_OFFSET) == 8) {
        return 0;
    }

    mask58 = *(DWORD *)(desc + GLI_DESC_MASK_58_OFFSET);
    tex = (signed char *)texture;
    mask5c = *(DWORD *)(desc + GLI_DESC_MASK_5C_OFFSET);
    mask60 = *(DWORD *)(desc + GLI_DESC_MASK_60_OFFSET);
    GLI_vAnalyzeColorMask(mask58, &shift58, &adjust58);
    GLI_vAnalyzeColorMask(mask5c, &shift5c, &adjust5c);
    GLI_vAnalyzeColorMask(mask60, &shift60, &adjust60);
    key =
        (((DWORD)(int)(signed char)(tex[0x2c] >> adjust58) << shift58) & mask58) |
        (((DWORD)(int)(signed char)(tex[0x2d] >> adjust5c) << shift5c) & mask5c) |
        (((DWORD)(int)(signed char)(tex[0x2e] >> adjust60) << shift60) & mask60);
    keyPair[0] = key;
    keyPair[1] = key;

    hr = ddSurface->lpVtbl->SetColorKey(ddSurface, GLI_DDCKEY_SRCBLT, keyPair);
    return hr == 0;
}
