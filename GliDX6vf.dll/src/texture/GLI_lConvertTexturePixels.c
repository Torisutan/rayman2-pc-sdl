#include "GliDX6vf/GliDX6vf.h"
#include "GliDX6vf/GliDX6vf_State.h"

#define GLI_DDSURFACEDESC2_SIZE 0x7c
#define GLI_DDSD_HEIGHT_OFFSET 0x08
#define GLI_DDSD_WIDTH_OFFSET 0x0c
#define GLI_DDSD_PITCH_OFFSET 0x10
#define GLI_DDSD_SURFACE_OFFSET 0x24
#define GLI_DDSD_RGB_BITCOUNT_OFFSET 0x54
#define GLI_DDSD_R_MASK_OFFSET 0x58
#define GLI_DDSD_G_MASK_OFFSET 0x5c
#define GLI_DDSD_B_MASK_OFFSET 0x60
#define GLI_DDSD_A_MASK_OFFSET 0x64
#define GLI_DDERR_WASSTILLDRAWING 0x8876021c

#define GLI_SRC_BYTE(p, off) (*(signed char *)((unsigned char *)(p) + (off)))
#define GLI_TERM32(p, off, adj, sh, mk) ((((long)(GLI_SRC_BYTE(p, off) >> (adj))) << (sh)) & (mk))

#define M_DescL(off) (*(long *)(desc + (off)))
#define M_DescUL(off) (*(unsigned long *)(desc + (off)))
#define M_Height M_DescL(GLI_DDSD_HEIGHT_OFFSET)
#define M_Width M_DescL(GLI_DDSD_WIDTH_OFFSET)
#define M_Pitch M_DescL(GLI_DDSD_PITCH_OFFSET)
#define M_Surface (*(unsigned char **)(desc + GLI_DDSD_SURFACE_OFFSET))
#define M_BitCount M_DescUL(GLI_DDSD_RGB_BITCOUNT_OFFSET)
#define M_TexKey(off) (*(signed char *)((unsigned char *)textureRecord + (off)))

int __cdecl GLI_lConvertTexturePixels(unsigned int *sourcePixels, void *surface, void *textureRecord)
{
    unsigned char desc[GLI_DDSURFACEDESC2_SIZE];
    GLI_IDirectDrawSurface4 *ddSurface;
    unsigned long aMask, bMask, gMask, rMask;
    long rShift, gShift, bShift, aShift;
    long rAdjust, gAdjust, bAdjust, aAdjust;
    long y, x;
    unsigned char *row;
    unsigned long key, packed;

    ddSurface = (GLI_IDirectDrawSurface4 *)surface;
    memset(desc, 0, sizeof(desc));
    *(unsigned long *)(desc + 0x00) = GLI_DDSURFACEDESC2_SIZE;

    do {
    } while (ddSurface->lpVtbl->Lock(ddSurface, NULL, desc, 0, NULL) == (HRESULT)GLI_DDERR_WASSTILLDRAWING);

    if (M_BitCount == 8) {
        memcpy(M_Surface, sourcePixels, M_Width * M_Height);
    } else {
        rMask = M_DescUL(GLI_DDSD_R_MASK_OFFSET);
        gMask = M_DescUL(GLI_DDSD_G_MASK_OFFSET);
        bMask = M_DescUL(GLI_DDSD_B_MASK_OFFSET);
        aMask = M_DescUL(GLI_DDSD_A_MASK_OFFSET);
        GLI_vAnalyzeColorMask(rMask, &rShift, &rAdjust);
        GLI_vAnalyzeColorMask(gMask, &gShift, &gAdjust);
        GLI_vAnalyzeColorMask(bMask, &bShift, &bAdjust);
        GLI_vAnalyzeColorMask(aMask, &aShift, &aAdjust);

        switch (M_BitCount) {
        case 16:
            row = M_Surface;
            if (textureRecord != NULL && (*(unsigned char *)((unsigned char *)textureRecord + 0x14) & 2)) {
                key = (((long)(M_TexKey(0x2c) >> rAdjust) << rShift) & rMask) |
                      (((long)(M_TexKey(0x2e) >> bAdjust) << bShift) & bMask) |
                      (((long)(M_TexKey(0x2d) >> gAdjust) << gShift) & gMask);
                for (y = 0; y < M_Height; y++) {
                    for (x = 0; x < M_Width; x++) {
                        packed = GLI_TERM32(sourcePixels, 0, rAdjust, rShift, rMask) |
                                 GLI_TERM32(sourcePixels, 1, gAdjust, gShift, gMask) |
                                 GLI_TERM32(sourcePixels, 2, bAdjust, bShift, bMask) |
                                 GLI_TERM32(sourcePixels, 3, aAdjust, aShift, aMask);
                        ((unsigned short *)row)[x] = (unsigned short)packed;
                        if ((((unsigned short)packed & ~aMask) == (unsigned short)key) &&
                            ((*(unsigned long *)((unsigned char *)textureRecord + 0x2c) ^ *sourcePixels) & 0xffffff)) {
                            ((unsigned short *)row)[x] = (unsigned short)((1 << gShift) ^ packed);
                        } else if (((unsigned short)packed & ~aMask) == (unsigned short)key) {
                            ((unsigned short *)row)[x] = (unsigned short)(~aMask & packed);
                        }
                        sourcePixels++;
                    }
                    row += M_Pitch;
                }
            } else {
                for (y = 0; y < M_Height; y++) {
                    for (x = 0; x < M_Width; x++) {
                        ((unsigned short *)row)[x] = (unsigned short)(
                            GLI_TERM32(sourcePixels, 0, rAdjust, rShift, rMask) |
                            GLI_TERM32(sourcePixels, 1, gAdjust, gShift, gMask) |
                            GLI_TERM32(sourcePixels, 2, bAdjust, bShift, bMask) |
                            GLI_TERM32(sourcePixels, 3, aAdjust, aShift, aMask));
                        sourcePixels++;
                    }
                    row += M_Pitch;
                }
            }
            break;
        case 24:
            row = M_Surface;
            for (y = 0; y < M_Height; y++) {
                for (x = 0; x < M_Width; x++) {
                    *(unsigned long *)(row + x * 3) =
                        GLI_TERM32(sourcePixels, 1, gAdjust, gShift, gMask) |
                        GLI_TERM32(sourcePixels, 2, bAdjust, bShift, bMask) |
                        GLI_TERM32(sourcePixels, 0, rAdjust, rShift, rMask);
                    sourcePixels++;
                }
                row += M_Pitch;
            }
            break;
        case 32:
            row = M_Surface;
            for (y = 0; y < M_Height; y++) {
                for (x = 0; x < M_Width; x++) {
                    ((unsigned long *)row)[x] = GLI_TERM32(sourcePixels, 0, rAdjust, rShift, rMask) |
                                                GLI_TERM32(sourcePixels, 1, gAdjust, gShift, gMask) |
                                                GLI_TERM32(sourcePixels, 2, bAdjust, bShift, bMask) |
                                                GLI_TERM32(sourcePixels, 3, aAdjust, aShift, aMask);
                    sourcePixels++;
                }
                row += M_Pitch;
            }
            break;
        }
    }

    return ddSurface->lpVtbl->Unlock(ddSurface, M_Surface) == 0;
}

#undef M_TexKey
#undef M_BitCount
#undef M_Surface
#undef M_Pitch
#undef M_Width
#undef M_Height
#undef M_DescUL
#undef M_DescL
