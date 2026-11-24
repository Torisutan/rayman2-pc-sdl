#include "GliDX6vf/GliDX6vf.h"
#include "GliDX6vf/GliDX6vf_State.h"

#define GLI_DDSD_CAPS_HEIGHT_WIDTH_PIXELFORMAT_MIPMAPCOUNT 0x00001007u
#define GLI_DDSD_TEXTURE_STAGE_FLAGS 0x00020000u

#define GLI_SURFACE_DESC_SIZE 0x7c
#define GLI_SURFACE_DESC_FLAGS_OFFSET 0x04
#define GLI_SURFACE_DESC_HEIGHT_OFFSET 0x08
#define GLI_SURFACE_DESC_WIDTH_OFFSET 0x0c
#define GLI_SURFACE_DESC_MIPMAP_COUNT_OFFSET 0x18
#define GLI_SURFACE_DESC_PIXEL_FORMAT_OFFSET 0x48
#define GLI_SURFACE_DESC_CAPS_OFFSET 0x68

#define GLI_DDSCAPS_LOCAL_TEXTURE_MIPMAP_VIDEOMEMORY 0x10405000u
#define GLI_DDSCAPS_NONLOCAL_TEXTURE_MIPMAP_VIDEOMEMORY 0x20405000u
#define GLI_DDSCAPS_TEXTURE_MANAGED_EXTRA 0x00400008u

#define GLI_DDSCAPS_LOCAL_TEXTURE_ALLOCONLOAD_VIDEOMEMORY 0x14005000u

#define GLI_CAPS_FLAGS_OFFSET 0x04
#define GLI_CAPS_FEATURES_OFFSET 0x08

#define GLI_DWORD_AT(base, offset) (*(DWORD *)((unsigned char *)(base) + (offset)))

static __inline DWORD GLI_QueryTextureMemory(GLI_IDirectDraw4 *ddraw, DWORD *scratchTotal,
                                             DWORD *nonLocalFree)
{
    DWORD freeBytes;

    GLI_QueryAvailableVidMem(ddraw, GLI_DDSCAPS_LOCAL_TEXTURE_MIPMAP_VIDEOMEMORY,
                             scratchTotal, &freeBytes);
    if ((GLI_DWORD_AT(GLI_g_GliCaps, GLI_CAPS_FLAGS_OFFSET) & 0x2) != 0) {
        GLI_QueryAvailableVidMem(ddraw, GLI_DDSCAPS_NONLOCAL_TEXTURE_MIPMAP_VIDEOMEMORY,
                                 scratchTotal, nonLocalFree);
        freeBytes += *nonLocalFree;
    }
    return freeBytes;
}

void __cdecl GLI_vSelectTextureFormats(void)
{
    signed char formatIndex;
    unsigned char surfaceDesc[GLI_SURFACE_DESC_SIZE];
    DWORD scratchTotal;
    DWORD nonLocalFree;

    ZeroMemory(surfaceDesc, sizeof(surfaceDesc));
    GLI_DWORD_AT(surfaceDesc, 0) = GLI_SURFACE_DESC_SIZE;

    for (formatIndex = 0; formatIndex < GLI_TEXTURE_COST_FORMAT_COUNT; ++formatIndex) {
        signed char plane;

        GLI_DWORD_AT(surfaceDesc, GLI_SURFACE_DESC_FLAGS_OFFSET) =
            GLI_DDSD_CAPS_HEIGHT_WIDTH_PIXELFORMAT_MIPMAPCOUNT;
        GLI_DWORD_AT(surfaceDesc, GLI_SURFACE_DESC_CAPS_OFFSET) =
            GLI_DDSCAPS_LOCAL_TEXTURE_ALLOCONLOAD_VIDEOMEMORY;
        GLI_vGetSelectedTextureFormat((char)formatIndex, surfaceDesc + GLI_SURFACE_DESC_PIXEL_FORMAT_OFFSET);

        for (plane = 0; plane < GLI_TEXTURE_COST_PLANE_COUNT; ++plane) {
            signed char row;
            DWORD rowBit;

            if (plane != 0) {
                if ((GLI_DWORD_AT(GLI_g_GliCaps, GLI_CAPS_FEATURES_OFFSET) & 0x4) == 0) {
                    DWORD *dst;
                    int copyRow;
                    int copyCol;

                    dst = (DWORD *)GLI_g_TextureMemoryCostTable[formatIndex][1];
                    for (copyRow = GLI_TEXTURE_COST_DIMENSION_COUNT; copyRow != 0; copyRow = copyRow - 1) {
                        for (copyCol = GLI_TEXTURE_COST_DIMENSION_COUNT; copyCol != 0; copyCol = copyCol - 1) {
                            *dst = dst[-(GLI_TEXTURE_COST_DIMENSION_COUNT * GLI_TEXTURE_COST_DIMENSION_COUNT)];
                            dst = dst + 1;
                        }
                    }
                    continue;
                }
                GLI_DWORD_AT(surfaceDesc, GLI_SURFACE_DESC_CAPS_OFFSET) |=
                    GLI_DDSCAPS_TEXTURE_MANAGED_EXTRA;
                GLI_DWORD_AT(surfaceDesc, GLI_SURFACE_DESC_FLAGS_OFFSET) |=
                    GLI_DDSD_TEXTURE_STAGE_FLAGS;
            }

            row = 0;
            rowBit = 1;
            do {
                signed char column;
                DWORD colBit;

                column = 0;
                colBit = 1;
                do {
                    GLI_IDirectDrawSurface4 *surface;
                    DWORD beforeFree;
                    DWORD afterFree;
                    HRESULT result;

                    GLI_DWORD_AT(surfaceDesc, GLI_SURFACE_DESC_WIDTH_OFFSET) = rowBit;
                    if (plane != 0) {
                        GLI_DWORD_AT(surfaceDesc, GLI_SURFACE_DESC_MIPMAP_COUNT_OFFSET) =
                            (row <= column ? row : column) + 1;
                    }
                    GLI_DWORD_AT(surfaceDesc, GLI_SURFACE_DESC_HEIGHT_OFFSET) = colBit;

                    beforeFree = GLI_QueryTextureMemory(GLI_g_pDirectDraw4, &scratchTotal, &nonLocalFree);
                    result = GLI_g_pDirectDraw4->lpVtbl->CreateSurface(
                        GLI_g_pDirectDraw4, surfaceDesc, &surface, NULL);
                    if (result == 0 && surface != NULL) {
                        LONG refs;

                        afterFree = GLI_QueryTextureMemory(GLI_g_pDirectDraw4, &scratchTotal, &nonLocalFree);
                        GLI_g_TextureMemoryCostTable[formatIndex][plane][row][column] =
                            beforeFree - afterFree;
                        do {
                            refs = (LONG)surface->lpVtbl->Release(surface);
                        } while (refs != 0);
                    } else {
                        GLI_g_TextureMemoryCostTable[formatIndex][plane][row][column] = 0;
                    }

                    colBit = colBit << 1;
                    column = column + 1;
                } while (column < GLI_TEXTURE_COST_DIMENSION_COUNT);
                rowBit = rowBit << 1;
                row = row + 1;
            } while (row < GLI_TEXTURE_COST_DIMENSION_COUNT);
        }
    }
}
