/*
 ----------------------------------------------------------------------------------------
  ogl_tex.c : textures of the OpenGL 3.3 driver.

  The engine chooses a texture's pixel format among six the driver selects
  (GLI_g_SelectedTextureFormats, used by GLI_vSelectPixelFormatForTexture of the Direct3D 6
  driver): 0 = alpha 4444, 1 = alpha 1555, 2 = opaque 565, 3 = palette (none), 4 = X8R8G8B8,
  5 = A8R8G8B8 -- what the Direct3D 6 driver selects on a device that offers them all. The
  pixels are packed into that format as the Direct3D 6 driver packs them (colour key
  included), so the colours keep its precision, then widened to 8 bits per channel for GL.
  Key pixels of a colour-keyed texture carry alpha 0; the shader discards them while the
  colour key is on (Direct3D 6: SetColorKey on the surface, COLORKEYENABLE).
 ----------------------------------------------------------------------------------------
 */
#include "GliGL3vf/GliGL3vf.h"

#include <stdlib.h>
#include <string.h>

/* The engine's texture (GLI_tdstTexture, as the Direct3D 6 driver reads it) */
#define C_lTexAvailable     0x00
#define C_lTexBitmap        0x08
#define C_lTexSpecParam     0x10
#define C_lTexCaps          0x14
#define C_lTexHeight        0x18
#define C_lTexWidth         0x1a
#define C_lTexChromaKey     0x2c
#define C_lTexNbLod         0x30

#define C_ulTexNZTexture        0x02
#define C_ulTexPaletteTexture   0x80

#define C_ulSlotEmpty       0xc0de0005u
#define C_lNbSlots          0x400

/* DDPIXELFORMAT */
#define C_ulDDPF_ALPHAPIXELS    0x00000001u
#define C_ulDDPF_RGB            0x00000040u

#define M_TexUL(Tex, Off)   (*(unsigned long *)((unsigned char *)(Tex) + (Off)))
#define M_TexUW(Tex, Off)   (*(unsigned short *)((unsigned char *)(Tex) + (Off)))

typedef struct tdstPixelFormat_
{
    unsigned long ulSize, ulFlags, ulFourCC, ulBitCount;
    unsigned long ulRMask, ulGMask, ulBMask, ulAMask;
} tdstPixelFormat;

static const tdstPixelFormat gs_a_stFormats[] =
{
    { 0x20, C_ulDDPF_RGB | C_ulDDPF_ALPHAPIXELS, 0, 16, 0x0f00, 0x00f0, 0x000f, 0xf000 },
    { 0x20, C_ulDDPF_RGB | C_ulDDPF_ALPHAPIXELS, 0, 16, 0x7c00, 0x03e0, 0x001f, 0x8000 },
    { 0x20, C_ulDDPF_RGB,                        0, 16, 0xf800, 0x07e0, 0x001f, 0x0000 },
    { 0x20, C_ulDDPF_RGB,                        0, 32, 0x00ff0000, 0x0000ff00, 0x000000ff, 0 },
    { 0x20, C_ulDDPF_RGB | C_ulDDPF_ALPHAPIXELS, 0, 32, 0x00ff0000, 0x0000ff00, 0x000000ff, 0xff000000 }
};
#define C_lNbFormats    ((long)(sizeof(gs_a_stFormats) / sizeof(gs_a_stFormats[0])))

/*
 ----------------------------------------------------------------------------------------
  Description : the texture format records and the six selected ones, and the memory
                cost of each format and size (Direct3D 6: measured by creating surfaces;
                here the size of the pixels, mip levels as that driver counts them)
 ----------------------------------------------------------------------------------------
 */
void __cdecl GLI_fn_vInitTextureFormats(void)
{
    static const unsigned char a_ucSelected[GLI_TEXTURE_FORMAT_SELECTED_COUNT] = { 0, 1, 2, 0xff, 3, 4 };
    tdstGliCaps *p_stCaps = (tdstGliCaps *)GLI_g_GliCaps;
    long lFormat, lPlane, lRow, lCol;

    memset(GLI_g_TextureFormats, 0, sizeof(GLI_g_TextureFormats));
    for ( lFormat = 0 ; lFormat < C_lNbFormats ; lFormat++ )
    {
        memcpy(GLI_g_TextureFormats[lFormat], &gs_a_stFormats[lFormat], GLI_TEXTURE_FORMAT_RECORD_SIZE);
    }
    GLI_g_TextureFormatCount = (DWORD)C_lNbFormats;
    memcpy(GLI_g_SelectedTextureFormats, a_ucSelected, sizeof(a_ucSelected));
    /* no palette format: no 8-bit palette textures (as the Direct3D 6 driver without one) */
    p_stCaps->ulTextureCaps = (tdeTextureCaps)(p_stCaps->ulTextureCaps & ~E_TEX_Support8P);

    for ( lFormat = 0 ; lFormat < GLI_TEXTURE_COST_FORMAT_COUNT ; lFormat++ )
    {
        tdstPixelFormat stFormat;
        unsigned long ulBytes;

        GLI_vGetSelectedTextureFormat((char)lFormat, &stFormat);
        ulBytes = stFormat.ulBitCount / 8;
        for ( lPlane = 0 ; lPlane < GLI_TEXTURE_COST_PLANE_COUNT ; lPlane++ )
        {
            for ( lRow = 0 ; lRow < GLI_TEXTURE_COST_DIMENSION_COUNT ; lRow++ )
            {
                for ( lCol = 0 ; lCol < GLI_TEXTURE_COST_DIMENSION_COUNT ; lCol++ )
                {
                    unsigned long ulW = 1ul << lRow, ulH = 1ul << lCol, ulCost = 0;
                    long lLevels = 1, l;

                    if ( lPlane != 0 && (p_stCaps->ulTextureCaps & E_TEX_SupportMipMap) )
                    {
                        lLevels = (lRow <= lCol ? lRow : lCol) + 1;
                    }
                    for ( l = 0 ; l < lLevels ; l++ )
                    {
                        ulCost += (ulW >> l) * (ulH >> l) * ulBytes;
                    }
                    GLI_g_TextureMemoryCostTable[lFormat][lPlane][lRow][lCol] = ulCost;
                }
            }
        }
    }
}

/* A source channel packed under a mask: shifted right by the adjustment, left by the shift
   (GLI_vAnalyzeColorMask of the Direct3D 6 driver), as its conversion does it */
#define M_Term(Src, Off, Adj, Sh, Mk) \
    ((((long)(*((signed char *)(Src) + (Off)) >> (Adj))) << (Sh)) & (Mk))

/* 8-bit value of a channel packed under ulMask */
static unsigned long fn_ulWiden(unsigned long ulPacked, unsigned long ulMask)
{
    unsigned long ulShift = 0, ulBits = 0, ulValue;

    if ( ulMask == 0 )
    {
        return 0xff;
    }
    while ( ((ulMask >> ulShift) & 1) == 0 )
    {
        ulShift++;
    }
    while ( ((ulMask >> (ulShift + ulBits)) & 1) != 0 )
    {
        ulBits++;
    }
    ulValue = (ulPacked & ulMask) >> ulShift;
    if ( ulBits >= 8 )
    {
        return (ulValue >> (ulBits - 8)) & 0xff;
    }
    /* bit replication, as the hardware widens 16-bit texels */
    ulValue <<= (8 - ulBits);
    return (ulValue | (ulValue >> ulBits) | (ulValue >> (2 * ulBits))) & 0xff;
}

/*
 ----------------------------------------------------------------------------------------
  Description : one level of the source (bytes R, G, B, A per texel) packed into the
                texture's format as GLI_lConvertTexturePixels of the Direct3D 6 driver
                packs it (key colour: alpha cleared, near misses moved off the key), then
                widened to bytes R, G, B, A for GL. Key texels of a format without alpha
                get alpha 0 (see the file header).
 ----------------------------------------------------------------------------------------
 */
static void fn_vConvertLevel(const unsigned long *p_ulSource, unsigned long *p_ulDest, long lWidth,
                             long lHeight, const tdstPixelFormat *p_stFormat, unsigned char *p_ucTexture)
{
    int lRShift, lGShift, lBShift, lAShift, lRAdjust, lGAdjust, lBAdjust, lAAdjust;
    unsigned long ulKey = 0, ulPacked;
    long bKey;
    long i;

    GLI_vAnalyzeColorMask(p_stFormat->ulRMask, &lRShift, &lRAdjust);
    GLI_vAnalyzeColorMask(p_stFormat->ulGMask, &lGShift, &lGAdjust);
    GLI_vAnalyzeColorMask(p_stFormat->ulBMask, &lBShift, &lBAdjust);
    GLI_vAnalyzeColorMask(p_stFormat->ulAMask, &lAShift, &lAAdjust);

    bKey = p_stFormat->ulBitCount == 16 && (M_TexUL(p_ucTexture, C_lTexCaps) & C_ulTexNZTexture) != 0;
    if ( bKey )
    {
        ulKey = M_Term(p_ucTexture, C_lTexChromaKey + 0, lRAdjust, lRShift, p_stFormat->ulRMask) |
                M_Term(p_ucTexture, C_lTexChromaKey + 2, lBAdjust, lBShift, p_stFormat->ulBMask) |
                M_Term(p_ucTexture, C_lTexChromaKey + 1, lGAdjust, lGShift, p_stFormat->ulGMask);
    }

    for ( i = 0 ; i < lWidth * lHeight ; i++ )
    {
        long bIsKey = 0;

        ulPacked = M_Term(p_ulSource, 0, lRAdjust, lRShift, p_stFormat->ulRMask) |
                   M_Term(p_ulSource, 1, lGAdjust, lGShift, p_stFormat->ulGMask) |
                   M_Term(p_ulSource, 2, lBAdjust, lBShift, p_stFormat->ulBMask) |
                   M_Term(p_ulSource, 3, lAAdjust, lAShift, p_stFormat->ulAMask);
        if ( p_stFormat->ulBitCount == 16 )
        {
            ulPacked &= 0xffff;
            if ( bKey && (ulPacked & ~p_stFormat->ulAMask & 0xffff) == ulKey )
            {
                if ( ((M_TexUL(p_ucTexture, C_lTexChromaKey) ^ *p_ulSource) & 0xffffff) != 0 )
                {
                    ulPacked = ((1ul << lGShift) ^ ulPacked) & 0xffff;   /* near miss */
                }
                else
                {
                    ulPacked = ~p_stFormat->ulAMask & ulPacked & 0xffff; /* the key itself */
                    bIsKey = 1;
                }
            }
        }
        p_ulDest[i] = fn_ulWiden(ulPacked, p_stFormat->ulRMask) |
                      (fn_ulWiden(ulPacked, p_stFormat->ulGMask) << 8) |
                      (fn_ulWiden(ulPacked, p_stFormat->ulBMask) << 16) |
                      ((bIsKey ? 0ul : fn_ulWiden(ulPacked, p_stFormat->ulAMask)) << 24);
        p_ulSource++;
    }
}

/*
 ----------------------------------------------------------------------------------------
  Description : every texture the engine has read, into GL (Direct3D 6: a surface per
                texture in the selected format, its mip levels, then Load into the device)
 ----------------------------------------------------------------------------------------
 */
__declspec(dllexport) void __cdecl
GLI_DRV_vDownLoadTextures(LONG bRestore, LONG lTextureMode, LONG bReloading)
{
    tdstGliCaps *p_stCaps = (tdstGliCaps *)GLI_g_GliCaps;
    unsigned long ulProgressStep = *(unsigned long *)GLI_g_NumberOfTextureToCreate / 10;
    unsigned long ulProgressNext = ulProgressStep;
    long bUseMipMap = (p_stCaps->ulTextureCaps & E_TEX_SupportMipMap) != 0;
    long lSlot;

    (void)bRestore;
    (void)lTextureMode;     /* where the Direct3D 6 driver puts its surfaces */

    for ( lSlot = 0 ; lSlot < C_lNbSlots ; lSlot++ )
    {
        unsigned char *p_ucTexture;
        GLI_tdstGLTexture *p_stGLTexture;
        tdstPixelFormat stFormat;
        unsigned long *d_ulPixels;
        long lWidth, lHeight, lLevels, lLevel;
        unsigned char *p_ucColorTable = NULL;

        if ( GLI_g_TextureMemoryChannels[lSlot] == C_ulSlotEmpty )
        {
            continue;
        }
        p_ucTexture = ((unsigned char **)GLI_g_TextureAlreadyRead)[lSlot];
        if ( p_ucTexture == NULL )
        {
            continue;
        }

        GLI_g_LoadTextureInTexelField(p_ucTexture, GLI_g_ConvertBufferMipMapping, bReloading);

        GLI_vSelectPixelFormatForTexture(M_TexUL(p_ucTexture, C_lTexCaps), &stFormat);
        lWidth = M_TexUW(p_ucTexture, C_lTexWidth);
        lHeight = M_TexUW(p_ucTexture, C_lTexHeight);
        lLevels = (bUseMipMap && *(long *)(p_ucTexture + C_lTexNbLod) != 0)
                  ? *(long *)(p_ucTexture + C_lTexNbLod) : 0;
        if ( M_TexUL(p_ucTexture, C_lTexCaps) & C_ulTexPaletteTexture )
        {
            p_ucColorTable = *(unsigned char **)(p_ucTexture + 0x0c);
        }

        d_ulPixels = (unsigned long *)malloc((size_t)lWidth * (size_t)lHeight * 4);
        p_stGLTexture = (GLI_tdstGLTexture *)malloc(sizeof(GLI_tdstGLTexture));
        if ( d_ulPixels == NULL || p_stGLTexture == NULL )
        {
            free(d_ulPixels);
            free(p_stGLTexture);
            GLI_vTextureLoadError(p_ucTexture, 0, "Create surface failed");
            continue;
        }
        memset(p_stGLTexture, 0, sizeof(*p_stGLTexture));
        p_stGLTexture->bAlphaFormat = stFormat.ulAMask != 0;
        p_stGLTexture->bColorKey = (M_TexUL(p_ucTexture, C_lTexCaps) & C_ulTexNZTexture) != 0;
        p_stGLTexture->bMipMaps = lLevels != 0;
        p_stGLTexture->lAddressMode = -1;

        GLI_g_stGL.p_fn_GenTextures(1, &p_stGLTexture->uiName);
        GLI_g_stGL.p_fn_BindTexture(GL_TEXTURE_2D, p_stGLTexture->uiName);
        GLI_g_stGL.p_fn_TexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        GLI_g_stGL.p_fn_TexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER,
                                      lLevels != 0 ? GL_LINEAR_MIPMAP_LINEAR : GL_LINEAR);
        GLI_g_stGL.p_fn_TexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAX_LEVEL, lLevels);

        fn_vConvertLevel(*(unsigned long **)(p_ucTexture + C_lTexBitmap), d_ulPixels, lWidth, lHeight,
                         &stFormat, p_ucTexture);
        GLI_g_stGL.p_fn_TexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, lWidth, lHeight, 0, GL_RGBA,
                                   GL_UNSIGNED_BYTE, d_ulPixels);

        /* mip levels: the engine halves its conversion buffer in place, level by level */
        for ( lLevel = 1 ; lLevel <= lLevels ; lLevel++ )
        {
            if ( p_ucColorTable != NULL )
            {
                GLI_g_DivideBitmapSurfaceBy2Palette(GLI_g_ConvertBufferMipMapping, p_ucColorTable, lHeight, lWidth);
            }
            else if ( M_TexUL(p_ucTexture, C_lTexCaps) & C_ulTexNZTexture )
            {
                GLI_g_DivideBitmapSurfaceBy2NonZero(GLI_g_ConvertBufferMipMapping, GLI_g_ConvertBufferMipMapping,
                                                    lHeight, lWidth, M_TexUL(p_ucTexture, C_lTexChromaKey));
            }
            else
            {
                GLI_g_DivideBitmapSurfaceBy2(GLI_g_ConvertBufferMipMapping, GLI_g_ConvertBufferMipMapping,
                                             lHeight, lWidth);
            }
            lWidth = lWidth > 1 ? lWidth >> 1 : 1;
            lHeight = lHeight > 1 ? lHeight >> 1 : 1;
            fn_vConvertLevel((const unsigned long *)GLI_g_ConvertBufferMipMapping, d_ulPixels, lWidth, lHeight,
                             &stFormat, p_ucTexture);
            GLI_g_stGL.p_fn_TexImage2D(GL_TEXTURE_2D, lLevel, GL_RGBA8, lWidth, lHeight, 0, GL_RGBA,
                                       GL_UNSIGNED_BYTE, d_ulPixels);
        }
        free(d_ulPixels);

        *(GLI_tdstGLTexture **)(p_ucTexture + C_lTexSpecParam) = p_stGLTexture;
        M_TexUL(p_ucTexture, C_lTexAvailable) = 1;

        if ( (unsigned long)lSlot >= ulProgressNext )
        {
            GLI_g_AddToProgressBar(1);
            ulProgressNext += ulProgressStep;
        }
    }
}

__declspec(dllexport) void __cdecl GLI_DRV_vUnLoadTextures(void)
{
    long lSlot;

    GLI_fn_vSetTexture(0, NULL);
    if ( GLI_g_TextureMemoryChannels == NULL || GLI_g_TextureAlreadyRead == NULL )
    {
        return;
    }
    for ( lSlot = 0 ; lSlot < C_lNbSlots ; lSlot++ )
    {
        unsigned char *p_ucTexture;
        GLI_tdstGLTexture *p_stGLTexture;

        if ( GLI_g_TextureMemoryChannels[lSlot] == C_ulSlotEmpty )
        {
            continue;
        }
        p_ucTexture = ((unsigned char **)GLI_g_TextureAlreadyRead)[lSlot];
        if ( p_ucTexture == NULL || M_TexUL(p_ucTexture, C_lTexAvailable) == 0 )
        {
            continue;
        }
        p_stGLTexture = *(GLI_tdstGLTexture **)(p_ucTexture + C_lTexSpecParam);
        if ( p_stGLTexture != NULL )
        {
            if ( GLI_g_hGLContext != NULL )
            {
                GLI_g_stGL.p_fn_DeleteTextures(1, &p_stGLTexture->uiName);
            }
            free(p_stGLTexture);
            *(GLI_tdstGLTexture **)(p_ucTexture + C_lTexSpecParam) = NULL;
            M_TexUL(p_ucTexture, C_lTexAvailable) = 0;
        }
    }
}
