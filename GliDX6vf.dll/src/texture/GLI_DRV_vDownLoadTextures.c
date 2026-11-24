#include "GliDX6vf/GliDX6vf.h"
#include "GliDX6vf/GliDX6vf_State.h"
#include "GliDX6vf/GLI_DiagLog.h"

#include <string.h>

#define TEX_AVAILABLE     0x00
#define TEX_BITMAP        0x08
#define TEX_COLORTABLE    0x0c
#define TEX_SPECPARAM     0x10
#define TEX_CAPS          0x14
#define TEX_HEIGHT        0x18
#define TEX_WIDTH         0x1a
#define TEX_CHROMAKEY     0x2c
#define TEX_NUMLOD        0x30

#define GLI_C_Tex_lNZTexture        0x02
#define GLI_C_Tex_lPaletteTexture   0x80

#define DDSD_SIZE         0x00
#define DDSD_FLAGS        0x04
#define DDSD_HEIGHT       0x08
#define DDSD_WIDTH        0x0c
#define DDSD_MIPMAPCOUNT  0x18
#define DDSD_PIXELFORMAT  0x48
#define DDSD_CAPS         0x68
#define DDSD_TEXTURESTAGE 0x78

#define TEXTURE_SLOT_SENTINEL   0xc0de0005u
#define TEXTURE_SLOT_COUNT      0x400
#define CREATESURFACE_RETRY_HR  ((HRESULT)0x8876017c)

typedef HRESULT (STDMETHODCALLTYPE *PFN_CreateSurface)(void *self, void *desc, void **surf, void *outer);
typedef HRESULT (STDMETHODCALLTYPE *PFN_CreatePalette)(void *self, DWORD flags, void *colorTable, void **pal, void *outer);
typedef HRESULT (STDMETHODCALLTYPE *PFN_QueryInterface)(void *self, const GUID *riid, void **out);
typedef HRESULT (STDMETHODCALLTYPE *PFN_GetAttachedSurface)(void *self, void *caps, void **surf);
typedef HRESULT (STDMETHODCALLTYPE *PFN_SetPalette)(void *self, void *pal);
typedef HRESULT (STDMETHODCALLTYPE *PFN_TextureLoad)(void *self, void *src);
typedef ULONG   (STDMETHODCALLTYPE *PFN_Release)(void *self);

#define VT(obj)            (*(void ***)(obj))
#define VFN(obj, off, T)   ((T)(VT(obj)[(off) / 4]))

static const GUID GLI_IID_IDirect3DTexture2 =
    { 0x93281502, 0x8cf8, 0x11d0, { 0x89, 0xab, 0x00, 0xa0, 0xc9, 0x05, 0x41, 0x29 } };

__declspec(dllexport) void __cdecl
GLI_DRV_vDownLoadTextures(LONG bRestore, LONG lTextureMode, LONG bReloading)
{
    int            stageSelector = 0;
    unsigned int   progressNext = (*(unsigned long *)GLI_g_NumberOfTextureToCreate / 10);
    int            useMipmap = *(int *)((unsigned char *)GLI_g_GliCaps + 8) & 4;
    int            useSecond = *(int *)((unsigned char *)GLI_g_GliCaps + 4) & 2;
    int            slot;
    unsigned char *colorTable;

    (void)bRestore;

    GLI_DiagLog("vDownLoadTextures ENTER bRestore=%ld lTextureMode=%ld bReloading=%ld",
                bRestore, lTextureMode, bReloading);

    for (slot = 0; slot < TEXTURE_SLOT_COUNT; slot++) {
        unsigned char *tex;
        DWORD          desc[0x1f];
        unsigned char *descb = (unsigned char *)desc;
        DWORD          totalScratch, freeMem, freeMem2, freeMemAfter;
        HRESULT        hr, hrCreate;

        if (((DWORD *)GLI_g_TextureMemoryChannels)[slot] == TEXTURE_SLOT_SENTINEL) {
            continue;
        }
        tex = ((unsigned char **)GLI_g_TextureAlreadyRead)[slot];
        if (tex == NULL) {
            continue;
        }

        GLI_g_pTexTempSurface = NULL;
        GLI_g_pTexDestSurface = NULL;
        GLI_g_pTexTempTexture = NULL;
        GLI_g_pTexDestTexture = NULL;

        GLI_g_LoadTextureInTexelField(tex, GLI_g_ConvertBufferMipMapping, bReloading);

        GLI_QueryAvailableVidMem(GLI_g_pDirectDraw4, 0x10405000, &totalScratch, &freeMem);
        if (useSecond != 0) {
            GLI_QueryAvailableVidMem(GLI_g_pDirectDraw4, 0x20405000, &totalScratch, &freeMem2);
            freeMem = freeMem + freeMem2;
        }

        memset(desc, 0, sizeof(desc));
        *(DWORD *)(descb + DDSD_SIZE)  = 0x7c;
        *(DWORD *)(descb + DDSD_FLAGS) = 0x101007;
        GLI_vSelectPixelFormatForTexture(*(DWORD *)(tex + TEX_CAPS), descb + DDSD_PIXELFORMAT);
        *(DWORD *)(descb + DDSD_WIDTH)  = *(unsigned short *)(tex + TEX_WIDTH);
        *(DWORD *)(descb + DDSD_HEIGHT) = *(unsigned short *)(tex + TEX_HEIGHT);
        *(DWORD *)(descb + DDSD_CAPS)         = 0x4001000;
        *(DWORD *)(descb + DDSD_TEXTURESTAGE) = (DWORD)stageSelector;
        if (useMipmap == 0 || *(int *)(tex + TEX_NUMLOD) == 0) {
            *(DWORD *)(descb + DDSD_MIPMAPCOUNT) = 0;
        } else {
            *(DWORD *)(descb + DDSD_CAPS)         = 0x4401008;
            *(DWORD *)(descb + DDSD_FLAGS)       |= 0x20000;
            *(DWORD *)(descb + DDSD_MIPMAPCOUNT)  = *(int *)(tex + TEX_NUMLOD) + 1;
        }
        *(DWORD *)(descb + DDSD_CAPS) |= (DWORD)GLI_ulGetTextureSurfaceCaps(tex, lTextureMode, useSecond);

        hrCreate = VFN(GLI_g_pDirectDraw4, 0x18, PFN_CreateSurface)(
                GLI_g_pDirectDraw4, desc, &GLI_g_pTexDestSurface, NULL);
        if (hrCreate == CREATESURFACE_RETRY_HR) {
            stageSelector = 1 - stageSelector;
            *(DWORD *)(descb + DDSD_TEXTURESTAGE) = (DWORD)stageSelector;
            hrCreate = VFN(GLI_g_pDirectDraw4, 0x18, PFN_CreateSurface)(
                    GLI_g_pDirectDraw4, desc, &GLI_g_pTexDestSurface, NULL);
        }
        if (hrCreate != 0 || GLI_g_pTexDestSurface == NULL) {
            GLI_vTextureLoadError(tex, hrCreate, "Create surface failed");
            continue;
        }

        *(DWORD *)(descb + DDSD_CAPS) = 0x1800;
        if (useMipmap != 0 && *(int *)(tex + TEX_NUMLOD) != 0) {
            *(DWORD *)(descb + DDSD_CAPS) = 0x401808;
        }
        hr = VFN(GLI_g_pDirectDraw4, 0x18, PFN_CreateSurface)(
                GLI_g_pDirectDraw4, desc, &GLI_g_pTexTempSurface, NULL);
        if (hr != 0 || GLI_g_pTexTempSurface == NULL) {
            GLI_vTextureLoadError(tex, hr, "Create surface temp failed");
            continue;
        }

        if (*(DWORD *)(tex + TEX_CAPS) & GLI_C_Tex_lPaletteTexture) {
            unsigned char *entry;
            void *palette;
            int i;

            colorTable = *(unsigned char **)(tex + TEX_COLORTABLE);
            entry = colorTable;
            for (i = 0x100; i != 0; i--) {
                unsigned char b0 = entry[0];
                entry[0] = entry[2];
                entry[2] = b0;
                entry += 4;
            }

            hr = VFN(GLI_g_pDirectDraw4, 0x14, PFN_CreatePalette)(
                GLI_g_pDirectDraw4, 0x4c, colorTable, &palette, NULL);
            hr = VFN(GLI_g_pTexTempSurface, 0x7c, PFN_SetPalette)(GLI_g_pTexTempSurface, palette);
            hr = VFN(GLI_g_pTexDestSurface, 0x7c, PFN_SetPalette)(GLI_g_pTexDestSurface, palette);
            hr = VFN(palette, 0x08, PFN_Release)(palette);
        }

        if ((*(DWORD *)(tex + TEX_CAPS) & GLI_C_Tex_lNZTexture) != 0 &&
            GLI_lApplyTextureColorKey(tex, GLI_g_pTexDestSurface) == 0) {
            GLI_vTextureLoadError(tex, hr, "SetColorKey failed");
            continue;
        }

        if (GLI_lConvertTexturePixels(*(unsigned int **)(tex + TEX_BITMAP), GLI_g_pTexTempSurface, tex) == 0) {
            GLI_vTextureLoadError(tex, hr, "Copy bitmap failed");
            continue;
        }

        if (useMipmap != 0) {
            unsigned short mipWidth  = *(unsigned short *)(tex + TEX_WIDTH);
            unsigned short mipHeight = *(unsigned short *)(tex + TEX_HEIGHT);
            long           levels    = *(long *)(tex + TEX_NUMLOD);
            void          *level     = GLI_g_pTexTempSurface;
            DWORD          caps[4];

            hr = 0;
            while (levels--) {
                if (*(DWORD *)(tex + TEX_CAPS) & GLI_C_Tex_lPaletteTexture) {
                    GLI_g_DivideBitmapSurfaceBy2Palette(
                        GLI_g_ConvertBufferMipMapping, colorTable, mipHeight, mipWidth);
                } else if (*(DWORD *)(tex + TEX_CAPS) & GLI_C_Tex_lNZTexture) {
                    GLI_g_DivideBitmapSurfaceBy2NonZero(
                        GLI_g_ConvertBufferMipMapping, GLI_g_ConvertBufferMipMapping,
                        mipHeight, mipWidth, *(DWORD *)(tex + TEX_CHROMAKEY));
                } else {
                    GLI_g_DivideBitmapSurfaceBy2(
                        GLI_g_ConvertBufferMipMapping, GLI_g_ConvertBufferMipMapping,
                        mipHeight, mipWidth);
                }

                caps[0] = 0x401000;
                hr = VFN(level, 0x30, PFN_GetAttachedSurface)(level, caps, &level);
                if (hr != 0) {
                    GLI_vTextureLoadError(tex, hr, "GetAttachedSurface failed");
                    break;
                }
                if (GLI_lConvertTexturePixels(GLI_g_ConvertBufferMipMapping, level, tex) == 0) {
                    GLI_vTextureLoadError(tex, hr, "Copy bitmap level failed");
                    break;
                }
                mipWidth  >>= 1;
                mipHeight >>= 1;
            }
            if (hr != 0) {
                continue;
            }
        }

        hrCreate = VFN(GLI_g_pTexDestSurface, 0x00, PFN_QueryInterface)(
                GLI_g_pTexDestSurface, &GLI_IID_IDirect3DTexture2, &GLI_g_pTexDestTexture);
        if (hrCreate != 0) {
            GLI_vTextureLoadError(tex, hrCreate, "QueryInterface texture2 failed");
            continue;
        }
        hrCreate = VFN(GLI_g_pTexTempSurface, 0x00, PFN_QueryInterface)(
                GLI_g_pTexTempSurface, &GLI_IID_IDirect3DTexture2, &GLI_g_pTexTempTexture);
        if (hrCreate != 0) {
            GLI_vTextureLoadError(tex, hrCreate, "QueryInterface TempTexture2 failed");
            continue;
        }
        hrCreate = VFN(GLI_g_pTexDestTexture, 0x14, PFN_TextureLoad)(GLI_g_pTexDestTexture, GLI_g_pTexTempTexture);
        if (hrCreate != 0) {
            GLI_vTextureLoadError(tex, hrCreate, "load failed");
            continue;
        }

        do {

        } while ((LONG)VFN(GLI_g_pTexTempTexture, 0x08, PFN_Release)(GLI_g_pTexTempTexture) > 0);
        *(void **)(tex + TEX_SPECPARAM) = GLI_g_pTexDestTexture;
        *(DWORD *)(tex + TEX_AVAILABLE) = 1;

        GLI_QueryAvailableVidMem(GLI_g_pDirectDraw4, 0x10405000, &totalScratch, &freeMemAfter);
        if (useSecond != 0) {
            GLI_QueryAvailableVidMem(GLI_g_pDirectDraw4, 0x20405000, &totalScratch, &freeMem2);
            freeMemAfter = freeMemAfter + freeMem2;
        }
        if (slot >= (int)progressNext) {
            GLI_g_AddToProgressBar(1);
            progressNext += (*(unsigned long *)GLI_g_NumberOfTextureToCreate / 10);
        }
    }
    GLI_DiagLog("vDownLoadTextures EXIT");
}
