#include "GliDX6vf/GliDX6vf.h"
#include "GliDX6vf/GliDX6vf_State.h"

#define GLI_DDPF_ALPHAPIXELS 0x00000001u
#define GLI_DDPF_PALETTEINDEXED8 0x00000020u
#define GLI_DDPF_RGB 0x00000040u

#define GLI_DDPF_FLAGS_OFFSET 0x04
#define GLI_DDPF_RGB_BITCOUNT_OFFSET 0x0c
#define GLI_DDPF_G_MASK_OFFSET 0x14
#define GLI_DDPF_A_MASK_OFFSET 0x1c

#define GLI_CAPS_FEATURES_OFFSET 0x08

#define GLI_DWORD_AT(base, offset) (*(DWORD *)((unsigned char *)(base) + (offset)))

static HRESULT CALLBACK GLI_EnumTextureFormatsCallback(void *pixelFormat, void *context)
{
    (void)context;

    if ((GLI_DWORD_AT(pixelFormat, GLI_DDPF_FLAGS_OFFSET) & 0x000e0000u) != 0) {
        return 1;
    }

#ifdef GLI_DX6_FIXES
    if (GLI_g_TextureFormatCount < GLI_TEXTURE_FORMAT_RECORD_COUNT)
#endif
    {
        memset(GLI_g_TextureFormats[GLI_g_TextureFormatCount], 0, GLI_TEXTURE_FORMAT_RECORD_SIZE);
        memcpy(GLI_g_TextureFormats[GLI_g_TextureFormatCount], pixelFormat, GLI_TEXTURE_FORMAT_RECORD_SIZE);
    }
    GLI_g_TextureFormatCount++;
    return 1;
}

int __cdecl GLI_EnumTextureFormats(void)
{
    HRESULT result;
    long lCount;
    long i;
    unsigned char ucFallback;
    unsigned char *pFormat;
    DWORD ulFlags;

    GLI_g_TextureFormatCount = 0;
    result = GLI_g_pDirect3DDevice3->lpVtbl->EnumTextureFormats(
        GLI_g_pDirect3DDevice3, GLI_EnumTextureFormatsCallback, NULL);
    if (result != 0) {
        return result;
    }

    i = 0;
    memset(GLI_g_SelectedTextureFormats, 0xff, GLI_TEXTURE_FORMAT_SELECTED_COUNT);

    lCount = (long)GLI_g_TextureFormatCount;
#ifdef GLI_DX6_FIXES
    if (lCount > GLI_TEXTURE_FORMAT_RECORD_COUNT)
        lCount = GLI_TEXTURE_FORMAT_RECORD_COUNT;
#endif
    if (lCount > 0) {
        ucFallback = GLI_g_SelectedTextureFormats[0];
        do {
            pFormat = GLI_g_TextureFormats[i];
            ulFlags = GLI_DWORD_AT(pFormat, GLI_DDPF_FLAGS_OFFSET);
            if (ulFlags & GLI_DDPF_RGB) {
                if (GLI_DWORD_AT(pFormat, GLI_DDPF_RGB_BITCOUNT_OFFSET) == 32) {
                    if (ulFlags & GLI_DDPF_ALPHAPIXELS)
                        GLI_g_SelectedTextureFormats[5] = (unsigned char)i;
                    else
                        GLI_g_SelectedTextureFormats[4] = (unsigned char)i;
                } else if (GLI_DWORD_AT(pFormat, GLI_DDPF_RGB_BITCOUNT_OFFSET) == 16) {
                    if (ulFlags & GLI_DDPF_ALPHAPIXELS) {
                        if (GLI_DWORD_AT(pFormat, GLI_DDPF_A_MASK_OFFSET) != 0xf000 &&
                            GLI_DWORD_AT(pFormat, GLI_DDPF_A_MASK_OFFSET) != 0xf) {
                            if (GLI_DWORD_AT(pFormat, GLI_DDPF_A_MASK_OFFSET) == 1 ||
                                GLI_DWORD_AT(pFormat, GLI_DDPF_A_MASK_OFFSET) == 0x8000)
                                GLI_g_SelectedTextureFormats[1] = (unsigned char)i;
                            else if (ucFallback == 0xff)
                                ucFallback = (unsigned char)i;
                        } else
                            ucFallback = (unsigned char)i;
                    } else {

                        if (GLI_DWORD_AT(pFormat, GLI_DDPF_G_MASK_OFFSET) == 0x7e0)
                            GLI_g_SelectedTextureFormats[2] = (unsigned char)i;
                        else if (GLI_DWORD_AT(pFormat, GLI_DDPF_G_MASK_OFFSET) == 0x3e0) {
                            if (GLI_g_SelectedTextureFormats[2] != 0xff)
                                GLI_g_SelectedTextureFormats[2] = (unsigned char)i;
                        } else
                            continue;
                        if (ucFallback == 0xff)
                            ucFallback = (unsigned char)i;
                        if (GLI_g_SelectedTextureFormats[1] == 0xff)
                            GLI_g_SelectedTextureFormats[1] = (unsigned char)i;
                    }
                } else if (GLI_DWORD_AT(pFormat, GLI_DDPF_RGB_BITCOUNT_OFFSET) == 8 &&
                           (ulFlags & GLI_DDPF_PALETTEINDEXED8)) {
                    GLI_g_SelectedTextureFormats[3] = (unsigned char)i;
                }
            }
        } while (++i < lCount);
        GLI_g_SelectedTextureFormats[0] = ucFallback;
    }

    if (GLI_g_SelectedTextureFormats[3] == 0xff &&
        ((GLI_DWORD_AT(GLI_g_GliCaps, GLI_CAPS_FEATURES_OFFSET) & 1) != 0)) {
        GLI_DWORD_AT(GLI_g_GliCaps, GLI_CAPS_FEATURES_OFFSET) =
            GLI_DWORD_AT(GLI_g_GliCaps, GLI_CAPS_FEATURES_OFFSET) - 1;
    }

    return 0;
}
