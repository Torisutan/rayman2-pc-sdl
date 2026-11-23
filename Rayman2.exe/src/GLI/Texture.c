#include "GLI/Texture.h"
#include "GLI/Memory.h"
#include "GLI/Path.h"
#include "GLI/GLI_globals.h"
#include "GLI/GLD.h"
#include "GEO/GEO_globals.h"
#include "FIL/File.h"
#include "ERM/CpaError.h"
#include "GLI/ui_reset_redraw.h"
#include <string.h>
#include <windows.h>
#ifdef RAYMAN2_WIDESCREEN
#include "GLI/GLI_ratio.h"
#endif /* RAYMAN2_WIDESCREEN */
#ifdef RAYMAN2_FAST_LOADING
#include "GLI/GLI_fastload.h"
#endif /* RAYMAN2_FAST_LOADING */

extern unsigned char __cdecl FUN_0044f4e0(void);

static const unsigned long  k_xBP_One_bits      = 0x3F800000UL;       /* 0x0049c9a8 */
static const unsigned __int64 k_dBP_RecipFF_bits = 0x3F70101010101010ui64; /* 0x0049c9b0 */
static const unsigned long  k_xBP_255_bits      = 0x437F0000UL;       /* 0x0049c9b8 */
static const unsigned __int64 k_dBP_One_bits    = 0x3FF0000000000000ui64; /* 0x0049c9c0 */
static const unsigned __int64 k_dBP_Neg255_bits = 0xC06FE00000000000ui64; /* 0x0049c9c8 */
#define GLI_BP_K_xOne     (*(const float  *)&k_xBP_One_bits)
#define GLI_BP_K_dRecipFF (*(const double *)&k_dBP_RecipFF_bits)
#define GLI_BP_K_x255     (*(const float  *)&k_xBP_255_bits)
#define GLI_BP_K_dOne     (*(const double *)&k_dBP_One_bits)
#define GLI_BP_K_dNeg255  (*(const double *)&k_dBP_Neg255_bits)

#define GLI_HALF_SUM_MASK_32 0x7f7f7f7fu

/* 0x0045feb0 */
static long gli_ftol_truncate(float xVal)
{
    long  lResult;
    short uwOldCW, uwTrunCW;
    __asm {
        fnstcw word ptr [uwOldCW]
        mov    ax, word ptr [uwOldCW]
        or     ah, 0x0c
        mov    word ptr [uwTrunCW], ax
        fldcw  word ptr [uwTrunCW]
        fld    dword ptr [xVal]
        fistp  dword ptr [lResult]
        fldcw  word ptr [uwOldCW]
    }
    return lResult;
}

static __inline uint32_t GLI_AveragePair32(uint32_t ulA, uint32_t ulB)
{
    return ((ulB >> 1) & GLI_HALF_SUM_MASK_32) +
           ((ulA >> 1) & GLI_HALF_SUM_MASK_32);
}

#define GLI_M_vAveragePair32Chroma(Dst, ulA, ulB, ulChroma)                 \
    if ((((ulA) ^ (ulChroma)) & 0x00ffffffu) != 0) {                        \
        if ((((ulB) ^ (ulChroma)) & 0x00ffffffu) != 0) {                    \
            (Dst) = GLI_AveragePair32((ulA), (ulB));                        \
            if ((((ulChroma) ^ (Dst)) & 0x00ffffffu) == 0) (Dst) ^= 0x100u; \
        }                                                                   \
        else (Dst) = (ulA);                                                 \
    }                                                                       \
    else (Dst) = (ulB);

static uint32_t GLI_AveragePair32_Chroma(uint32_t ulA,
                                         uint32_t ulB,
                                         uint32_t ulChroma)
{
    uint32_t ulAverage;

    if (((ulA ^ ulChroma) & 0x00ffffffu) == 0) {
        return ulB;
    }
    if (((ulB ^ ulChroma) & 0x00ffffffu) == 0) {
        return ulA;
    }

    ulAverage = GLI_AveragePair32(ulA, ulB);
    if (((ulAverage ^ ulChroma) & 0x00ffffffu) == 0) {
        ulAverage ^= 0x100u;
    }
    return ulAverage;
}

static void GLI_DownscalePairs32_Horizontal(uint32_t *pSrc,
                                            uint32_t *pDst,
                                            int lWidth,
                                            int lHeight)
{
    int lHalfHeight = lHeight >> 1;
    uint32_t *pEnd;

    if (lHalfHeight <= 1) {
        return;
    }

    pEnd = pDst + (lHalfHeight * lWidth);
    while (pDst < pEnd) {
        *pDst++ = GLI_AveragePair32(pSrc[0], pSrc[1]);
        pSrc += 2;
    }
}

static void GLI_DownscaleRows32_Vertical(uint32_t *pSrc,
                                         uint32_t *pDst,
                                         int lWidth,
                                         int lHeight)
{
    int lOuter = lWidth >> 1;

    if (lOuter <= 1) {
        return;
    }

    do {
        int lInner = lHeight;
        if (lInner > 0) {
            do {
                *pDst++ = GLI_AveragePair32(pSrc[0], pSrc[lHeight]);
                ++pSrc;
                --lInner;
            } while (lInner != 0);
        }
        pSrc += lHeight;
        --lOuter;
    } while (lOuter != 0);
}

void __cdecl GLI_pc_fn_vDownscalePairs32_ChromaH(uint32_t *pSrc,
                              uint32_t *pDst,
                              int lWidth,
                              int lHeight,
                              uint32_t ulChroma)
{
    int lHalfHeight = lHeight >> 1;
    uint32_t *pEnd;

    if (lHalfHeight <= 1) {
        return;
    }

    pEnd = pDst + (lHalfHeight * lWidth);
    while (pDst < pEnd) {
        uint32_t ulA = pSrc[0], ulB = pSrc[1];
        GLI_M_vAveragePair32Chroma(*pDst, ulA, ulB, ulChroma)
        pDst++;
        pSrc += 2;
    }
}

void __cdecl GLI_pc_fn_vDownscaleRows32_ChromaV(uint32_t *pSrc,
                              uint32_t *pDst,
                              int lWidth,
                              int lHeight,
                              uint32_t ulChroma)
{
    int lOuter = lWidth >> 1;

    if (lOuter <= 1) {
        return;
    }

    do {
        int lInner = lHeight;
        if (lInner > 0) {
            do {
                *pDst++ = GLI_AveragePair32_Chroma(pSrc[0], pSrc[lHeight],
                                                   ulChroma);
                ++pSrc;
                --lInner;
            } while (lInner != 0);
        }
        pSrc += lHeight;
        --lOuter;
    } while (lOuter != 0);
}

void __cdecl GLI_DivideBitmapSurfaceBy2(uint32_t *pSrc,
                                        uint32_t *pDst,
                                        int lWidth,
                                        int lHeight)
{
    GLI_DownscalePairs32_Horizontal(pSrc, pDst, lWidth, lHeight);
    GLI_DownscaleRows32_Vertical(pSrc, pDst, lWidth >> 1, lHeight);
}

void __cdecl GLI_DivideBitmapSurfaceBy2NonZero(uint32_t *pSrc,
                                               uint32_t *pDst,
                                               int lWidth,
                                               int lHeight,
                                               uint32_t ulChroma)
{
    GLI_pc_fn_vDownscalePairs32_ChromaH(pSrc, pDst, lWidth, lHeight, ulChroma);
    GLI_pc_fn_vDownscaleRows32_ChromaV(pSrc, pDst, lWidth >> 1, lHeight, ulChroma);
}

/* 0x00425d10 */
void __cdecl GLI_DivideBitmapSurfaceBy2Palette(unsigned char *pIndexBuffer,
                                               unsigned char *pPalette,
                                               int            lStride,
                                               int            lHeight)
{
    /* 0x0049c580 */
    static const unsigned __int64 k_dYIQ_bits[10] = {
        0x3FE2C8B439581062ui64,  /* 0x0049c580 */
        0xBFBD2F1A9FBE76C9ui64,  /* 0x0049c588 */
        0xBFD322D0E5604189ui64,  /* 0x0049c590 */
        0x3FE3126E978D4FDFui64,  /* 0x0049c598 */
        0x3FD189374BC6A7F0ui64,  /* 0x0049c5a0 */
        0x3FD49BA5E353F7CFui64,  /* 0x0049c5a8 */
        0x3FD3E76C8B439581ui64,  /* 0x0049c5b0 */
        0xBFCB020C49BA5E35ui64,  /* 0x0049c5b8 */
        0xBFE0B4395810624Eui64,  /* 0x0049c5c0 */
        0x3FD0000000000000ui64   /* 0x0049c5c8 */
    };
#define GLI_DP_K(i) (*(const double *)&k_dYIQ_bits[(i)])

    {
        float          afTable[768];
        float         *p_xfPal;
        unsigned char *p_ucPal;
        unsigned char *p_ucEnd;
        unsigned char *p_ucNextRow;
        unsigned char *p_ucRowEnd;
        unsigned char *p_ucDst;
        unsigned int   uBestIdx;
        float          xBestDist;
        unsigned int   uIdx;

        p_ucPal = pPalette + 2;
        p_xfPal = afTable + 1;
        while (p_ucPal < pPalette + 0x400 + 2) {
            float xG = (float)p_ucPal[-1];
            float xR = (float)p_ucPal[-2];
            float xB = (float)p_ucPal[ 0];
            p_xfPal[-1] = xG * GLI_DP_K(0) - xR * GLI_DP_K(1) - xB * GLI_DP_K(2);
            p_xfPal[ 0] = xB * GLI_DP_K(3) - xG * GLI_DP_K(4) - xR * GLI_DP_K(5);
            p_xfPal[ 1] = xR * GLI_DP_K(6) - (xB * GLI_DP_K(7) - xG * GLI_DP_K(8));
            p_ucPal     += 4;
            p_xfPal     += 3;
        }

        p_ucEnd = pIndexBuffer + lHeight * lStride;
        p_ucDst = pIndexBuffer;
        uBestIdx = ((unsigned int)(uintptr_t)p_ucEnd) & 0xff;
        while (pIndexBuffer < p_ucEnd) {
            p_ucNextRow = pIndexBuffer + lStride;
            p_ucRowEnd  = pIndexBuffer + lStride;
            while (pIndexBuffer < p_ucRowEnd) {
                unsigned int u0 = (unsigned int)pIndexBuffer[0];
                unsigned int u1 = (unsigned int)pIndexBuffer[1];
                unsigned int u2 = (unsigned int)p_ucNextRow[0];
                unsigned int u3 = (unsigned int)p_ucNextRow[1];
                pIndexBuffer += 2;
                p_ucNextRow  += 2;
                xBestDist = 10000.0f;
                uIdx = 0;
                p_xfPal = afTable;
                while (uIdx < 256) {
                    float xAvgY = (afTable[u0 * 3]     + afTable[u1 * 3]     +
                                   afTable[u2 * 3]     + afTable[u3 * 3])     * GLI_DP_K(9) - p_xfPal[0];
                    float xAvgI = (afTable[u0 * 3 + 1] + afTable[u1 * 3 + 1] +
                                   afTable[u2 * 3 + 1] + afTable[u3 * 3 + 1]) * GLI_DP_K(9) - p_xfPal[1];
                    float xAvgQ = (afTable[u0 * 3 + 2] + afTable[u1 * 3 + 2] +
                                   afTable[u2 * 3 + 2] + afTable[u3 * 3 + 2]) * GLI_DP_K(9) - p_xfPal[2];
                    float xDist = xAvgY * xAvgY + xAvgI * xAvgI + xAvgQ * xAvgQ;
                    if (xDist < xBestDist) {
                        uBestIdx  = uIdx;
                        xBestDist = xDist;
                    }
                    uIdx    += 1;
                    p_xfPal += 3;
                }
                *p_ucDst++ = (unsigned char)uBestIdx;
            }
            pIndexBuffer += lStride;
        }
    }
}

void __cdecl GLI_pc_fn_vExpandPixelsBy2_32(uint32_t *pBuffer, int lHeight, int lWidth)
{
    uint32_t *pDst;
    uint32_t *pSrc;

    pSrc = pBuffer + (lWidth * lHeight) - 1;
    pDst = (uint32_t *)((uint8_t *)pBuffer + ((lWidth * 8 - 4) * lHeight) - 4);

    while (pSrc > pBuffer) {
        uint32_t *pRowStart = pSrc - lHeight;
        uint32_t *pForwardDst = pDst + 1;

        while (pSrc > pRowStart) {
            *pForwardDst = *pSrc;
            *pDst = *pSrc;
            pSrc--;
            pForwardDst++;
            pDst--;
        }

        pDst -= lHeight;
        pSrc = pRowStart;
    }
}

void __cdecl GLI_pc_fn_vExpandRows32A(uint32_t *pBuffer, int lHeight, int lWidth)
{
    uint32_t *pOriginalEnd = pBuffer + (lWidth * lHeight);
    uint32_t *pCursor;
    uint32_t *pDstEnd;
    uint32_t *pRowStart;
    uint32_t *pRowEnd;
    int lDelta;

    if (pBuffer < pOriginalEnd) {
        pCursor = pBuffer;
        lDelta = (int)((uint8_t *)pOriginalEnd - (uint8_t *)pBuffer);
        do {
            *(uint32_t *)((uint8_t *)pCursor + lDelta) = *pCursor;
            ++pCursor;
        } while (pCursor < pOriginalEnd);
    }

    pDstEnd = pBuffer + (lWidth * lHeight * 2);
    pRowStart = pDstEnd - lHeight;
    pRowEnd = pDstEnd;

    while (pRowStart >= pOriginalEnd) {
        uint32_t *pSrc = pRowStart;
        while (pSrc < pRowEnd) {
            *pBuffer++ = *pSrc++;
        }
        pRowEnd = pRowStart;
        pRowStart -= lHeight;
    }
}

void __cdecl GLI_pc_fn_vExpandRows32B(uint32_t *pBuffer, int lHeight, int lWidth)
{
    uint32_t *pRowEnd = pBuffer + (lWidth * lHeight);
    uint32_t *pDst = pRowEnd;
    uint32_t *pRowStart = pRowEnd - lHeight;

    while (pRowStart >= pBuffer) {
        uint32_t *pSrc = pRowStart;
        while (pSrc < pRowEnd) {
            *pDst++ = *pSrc++;
        }
        pRowEnd = pRowStart;
        pRowStart -= lHeight;
    }
}

void __cdecl GLI_pc_fn_vReplaceLowAlphaPixels(uint32_t *pPixels,
                              int lHeight,
                              int lWidth,
                              uint32_t ulReplacement)
{
    uint32_t *pEnd = pPixels + (lWidth * lHeight);

    while (pPixels < pEnd) {
        if ((*pPixels & 0xff000000u) < 0xc0000000u) {
            *pPixels = ulReplacement;
        }
        ++pPixels;
    }
}

void __cdecl GLI_pc_fn_vApplyColorKeyAlpha(uint32_t *pPixels,
                              int lHeight,
                              int lWidth,
                              uint32_t ulKey)
{
    uint32_t *pEnd = pPixels + (lWidth * lHeight);

    while (pPixels < pEnd) {
        uint32_t ulPixel = *pPixels;
        if ((ulPixel & 0x00ffffffu) == ulKey) {
            *pPixels = ulPixel & 0x00ffffffu;
        } else {
            *pPixels = ulPixel | 0xff000000u;
        }
        ++pPixels;
    }
}

#define GLI_M_ColorKeySubNeighbour(ulPix)                                   \
    {                                                                       \
        ulNeigh = (ulPix);                                                  \
        if ((ulNeigh & 0xff000000u) != 0) {                                 \
            xR -= (float)((ulNeigh >> 16) & 0xffu) * (1.0f / -9.0f);        \
            xG -= (float)((ulNeigh >>  8) & 0xffu) * (1.0f / -9.0f);        \
            xB -= (float)( ulNeigh        & 0xffu) * (1.0f / -9.0f);        \
        }                                                                   \
    }

/* 0x004254b0 */
void __cdecl GLI_pc_fn_vColorKeyNeighborhoodPassA(uint32_t *pPixels,
                              uint32_t  ulWidth,
                              int       lHeight,
                              uint32_t  ulKey)
{
    uint32_t *p_ulCursor;
    uint32_t *p_ulRow;
    uint32_t  ulPixCount;
    uint32_t  ulRowOffset;
    uint32_t  ulCol;
    int       lYPrev, lYNext, lXPrev, lXNext;
    uint32_t  ulRgb;

    if ((ulWidth == 1) && (lHeight == 1)) {
        return;
    }

    ulPixCount = (uint32_t)lHeight * ulWidth;

    for (p_ulCursor = pPixels; p_ulCursor < pPixels + ulPixCount; p_ulCursor++) {
        ulRgb = *p_ulCursor & 0xffffffu;
        if (ulRgb == (ulKey & 0xffffffu)) {
            *p_ulCursor = ulRgb;
        } else {
            *p_ulCursor = *p_ulCursor | 0xff000000u;
        }
    }

    p_ulRow = pPixels;
    for (ulRowOffset = 0; ulRowOffset < ulPixCount; ulRowOffset += ulWidth, p_ulRow += ulWidth) {

        lYPrev = ((ulRowOffset != 0) ? -1 : 1) * ulWidth;

        lYNext = ((ulRowOffset == ((uint32_t)(lHeight - 1)) * ulWidth) ? -1 : 1) * ulWidth;

        ulCol      = 0;
        p_ulCursor = p_ulRow;
        while (ulCol < ulWidth) {

            lXNext = (ulCol == ulWidth - 1) ? -1 : 1;

            lXPrev = (ulCol != 0) ? -1 : 1;

            if ((*p_ulCursor & 0xff000000u) == 0) {

                float    xR, xG, xB;
                uint32_t ulNeigh;

                xR = 0.0f;
                xG = 0.0f;
                xB = 0.0f;
                ulNeigh = p_ulCursor[lXNext];
                if ((ulNeigh & 0xff000000u) != 0) {
                    xR = (float)((ulNeigh >> 16) & 0xffu) * (1.0f / 9.0f);
                    xG = (float)((ulNeigh >>  8) & 0xffu) * (1.0f / 9.0f);
                    xB = (float)( ulNeigh        & 0xffu) * (1.0f / 9.0f);
                }
                GLI_M_ColorKeySubNeighbour(p_ulCursor[lXPrev]);
                GLI_M_ColorKeySubNeighbour(p_ulCursor[lYNext]);
                GLI_M_ColorKeySubNeighbour(p_ulCursor[lYNext + lXNext]);
                GLI_M_ColorKeySubNeighbour(p_ulCursor[lYNext + lXPrev]);
                GLI_M_ColorKeySubNeighbour(p_ulCursor[lYPrev]);
                GLI_M_ColorKeySubNeighbour(p_ulCursor[lYPrev + lXNext]);
                GLI_M_ColorKeySubNeighbour(p_ulCursor[lYPrev + lXPrev]);

                /* 0x00425995 */
                *p_ulCursor = (((long)xG & 0xffffu) | (((long)xR & 0xffu) << 8)) << 8
                              | ((long)xB & 0xffffffu);
            } else {

                signed char cKeyNeighborCount = 0;
                if ((p_ulCursor[lXNext]            & 0xff000000u) == 0) cKeyNeighborCount = 1;
                if ((p_ulCursor[lXPrev]            & 0xff000000u) == 0) cKeyNeighborCount++;
                if ((p_ulCursor[lYNext]            & 0xff000000u) == 0) cKeyNeighborCount++;
                if ((p_ulCursor[lYNext + lXNext]   & 0xff000000u) == 0) cKeyNeighborCount++;
                if ((p_ulCursor[lYNext + lXPrev]   & 0xff000000u) == 0) cKeyNeighborCount++;
                if ((p_ulCursor[lYPrev]            & 0xff000000u) == 0) cKeyNeighborCount++;
                if ((p_ulCursor[lYPrev + lXNext]   & 0xff000000u) == 0) cKeyNeighborCount++;
                if ((p_ulCursor[lYPrev + lXPrev]   & 0xff000000u) == 0) cKeyNeighborCount++;
                if (cKeyNeighborCount >= 7) {
                    *p_ulCursor = *p_ulCursor & 0xffffffu;
                }
            }
            ulCol++;
            p_ulCursor++;
        }
    }
}

#include "GLI/GLI_types.h"
#include "GLI/devinfo.h"

#define GLI_pc_C_ulHalfSumMask  0x7f7f7f7fUL
void __cdecl GLI_pc_fn_vColorKeyNeighborhoodPassB(int hTexture, uint32_t ulBuffer)
{
    uint32_t  ulWidth, ulHeight, ulKey;
    uint32_t  ulPixCount, ulRowOffset, ulCol;
    int       lYPrev, lYNext, lXPrev, lXNext;
    uint32_t *p_ulCursor;
    uint32_t *p_ulPix;
    GLI_tdstTexture *p_stTexture;

    p_ulPix     = (uint32_t *)(uintptr_t)ulBuffer;
    p_stTexture = (GLI_tdstTexture *)(intptr_t)hTexture;
    ulWidth  = (uint32_t)p_stTexture->uwWidth;
    ulHeight = (uint32_t)p_stTexture->uwHeight;
    ulKey    = p_stTexture->ulChromakeyColorRGBA;

    if ((ulWidth == 1) && (ulHeight == 1)) {
        return;
    }

    ulPixCount = ulWidth * ulHeight;

    ulRowOffset = 0;
    while (ulRowOffset < ulPixCount) {
        ulCol      = 0;
        p_ulCursor = p_ulPix + ulRowOffset;
        while (ulCol < ulWidth) {
            uint32_t ulRgb = *p_ulCursor & 0xffffffu;
            if (ulRgb == (ulKey & 0xffffffu)) {
                *p_ulCursor = ulRgb;
            } else {
                *p_ulCursor = *p_ulCursor | 0xff000000u;
            }
            ulCol++;
            p_ulCursor++;
        }
        ulRowOffset += ulWidth;
    }

    ulRowOffset = 0;
    while (ulRowOffset < ulPixCount) {

        lYPrev = (ulRowOffset == 0) ? (int)ulWidth : -(int)ulWidth;
        lYNext = (ulRowOffset == (ulHeight - 1) * ulWidth)
                     ? -(int)ulWidth : (int)ulWidth;

        ulCol = 0;
        while (ulCol < ulWidth) {
            int       lOff_self;
            uint32_t  ulAcc;
            uint32_t  ulNeigh;

            lXPrev = (ulCol == 0)            ?  1 : -1;
            lXNext = (ulCol == ulWidth - 1)  ? -1 :  1;
            lOff_self = (int)ulCol + (int)ulRowOffset;
            ulAcc = 0;

            if ((p_ulPix[lOff_self] & 0xff000000u) == 0) {

                int      lOff;

                lOff   = lOff_self + lXNext;
                ulNeigh = p_ulPix[lOff];
                if ((ulNeigh & 0xff000000u) != 0) {
                    ulAcc = (ulNeigh >> 1) & GLI_pc_C_ulHalfSumMask;
                }
                lOff   = lOff_self + lXPrev;
                ulNeigh = p_ulPix[lOff];
                if ((ulNeigh & 0xff000000u) != 0) {
                    ulAcc = ((ulAcc >> 1) & GLI_pc_C_ulHalfSumMask) + ((ulNeigh >> 1) & GLI_pc_C_ulHalfSumMask);
                }
                ulNeigh = p_ulPix[lYNext + lOff_self];
                if ((ulNeigh & 0xff000000u) != 0) {
                    ulAcc = ((ulAcc >> 1) & GLI_pc_C_ulHalfSumMask) + ((ulNeigh >> 1) & GLI_pc_C_ulHalfSumMask);
                }
                ulNeigh = p_ulPix[(lOff_self + lXNext) + lYNext];
                if ((ulNeigh & 0xff000000u) != 0) {
                    ulAcc = ((ulAcc >> 1) & GLI_pc_C_ulHalfSumMask) + ((ulNeigh >> 1) & GLI_pc_C_ulHalfSumMask);
                }
                ulNeigh = p_ulPix[lYNext + (lOff_self + lXPrev)];
                if ((ulNeigh & 0xff000000u) != 0) {
                    ulAcc = ((ulAcc >> 1) & GLI_pc_C_ulHalfSumMask) + ((ulNeigh >> 1) & GLI_pc_C_ulHalfSumMask);
                }
                ulNeigh = p_ulPix[lYPrev + lOff_self];
                if ((ulNeigh & 0xff000000u) != 0) {
                    ulAcc = ((ulAcc >> 1) & GLI_pc_C_ulHalfSumMask) + ((ulNeigh >> 1) & GLI_pc_C_ulHalfSumMask);
                }
                ulNeigh = p_ulPix[(lOff_self + lXNext) + lYPrev];
                if ((ulNeigh & 0xff000000u) != 0) {
                    ulAcc = ((ulAcc >> 1) & GLI_pc_C_ulHalfSumMask) + ((ulNeigh >> 1) & GLI_pc_C_ulHalfSumMask);
                }
                ulNeigh = p_ulPix[lYPrev + (lOff_self + lXPrev)];
                if ((ulNeigh & 0xff000000u) != 0) {
                    ulAcc = ((ulAcc >> 1) & GLI_pc_C_ulHalfSumMask) + ((ulNeigh >> 1) & GLI_pc_C_ulHalfSumMask);
                }
                p_ulPix[lOff_self] = ulAcc & 0xffffffu;
            }
            ulCol++;
        }
        ulRowOffset += ulWidth;
    }
}

/* 0x00493a40 */
void __cdecl FIL_LoadGFLikeImageToRecord(void       *hConcatFile,
                                         char       *p_szPath,
                                         char       *p_szFilename,
                                         FIL_tdstGF *pGF)
{
    struct FIL_tdstFileInConcatFile_ *hStream;
    FIL_tdstRealGFFileHeader hdr;
    unsigned char *pBitMap;
    unsigned long  uPlane, uPixel, k;
    unsigned char  ucByte, ucRepeatVal, ucRepeatCount;

    /* 0x493a5a */
    pGF->p_ucBitMap                      = (unsigned char *)0;
    pGF->p_ucPalette                     = (unsigned char *)0;
    pGF->stFileHeader.uwNbColorInPalette = 0;
    pGF->stFileHeader.ulWidth            = 0;
    pGF->stFileHeader.ulHeight           = 0;
    pGF->stFileHeader.ucBpp              = 0;
    pGF->stFileHeader.ucBpcInPalette     = 0;
    pGF->stFileHeader.ulImageSize        = 0;
    pGF->stFileHeader.ucFormat           = 0xff;

    hStream = (struct FIL_tdstFileInConcatFile_ *)
        FIL_fn_hOpenFileInConcatFile(hConcatFile, p_szPath, p_szFilename);
    if (hStream == NULL)
        return;

    FIL_fn_lReadFileInConcatFile(&hdr, 0xe, 1, hStream);

    pGF->stFileHeader.ulWidth     = hdr.ulWidth;
    pGF->stFileHeader.ulHeight    = hdr.ulHeight;
    pGF->stFileHeader.ulImageSize = hdr.ulWidth * hdr.ulHeight;
    pGF->stFileHeader.ucBpp       = hdr.ucBpp;
    pGF->stFileHeader.ucRLECode   = hdr.ucRLECode;
    pGF->stFileHeader.ucVersion   = 1;

    if (hdr.ucBpp == 1) {
        pGF->stFileHeader.uwNbColorInPalette = 0x100;
        pGF->stFileHeader.ucBpcInPalette     = 4;
    }

    if (pGF->stFileHeader.lChromakeyIndex == -1) {
        pGF->stFileHeader.ucRed   = 0;
        pGF->stFileHeader.ucGreen = 0;
        pGF->stFileHeader.ucBlue  = 0;
    }

    pGF->p_ucBitMap = (unsigned char *)GLI_pc_fn_p_vAlloc(
        (unsigned long)hdr.ucBpp *
        hdr.ulWidth *
        hdr.ulHeight);

    if (pGF->stFileHeader.uwNbColorInPalette != 0 &&
        pGF->stFileHeader.ucBpcInPalette     != 0) {
        pGF->p_ucPalette = (unsigned char *)GLI_pc_fn_p_vAlloc(
            (unsigned long)pGF->stFileHeader.uwNbColorInPalette *
            (unsigned long)pGF->stFileHeader.ucBpcInPalette);
        FIL_fn_lReadFileInConcatFile(
            pGF->p_ucPalette,
            (long)pGF->stFileHeader.ucBpcInPalette,
            (long)pGF->stFileHeader.uwNbColorInPalette,
            hStream);
    }

    pBitMap = pGF->p_ucBitMap;
    if (pGF->stFileHeader.ucBpp != 0) {
        for (uPlane = 0; uPlane < (unsigned long)pGF->stFileHeader.ucBpp; uPlane++) {
            uPixel = 0;
            while (uPixel < (unsigned long)pGF->stFileHeader.ulImageSize) {
                FIL_fn_lReadFileInConcatFile(&ucByte, 1, 1, hStream);
                if (ucByte == pGF->stFileHeader.ucRLECode) {
                    FIL_fn_lReadFileInConcatFile(&ucRepeatVal,   1, 1, hStream);
                    FIL_fn_lReadFileInConcatFile(&ucRepeatCount, 1, 1, hStream);
                    for (k = 0; k < (unsigned long)ucRepeatCount; k++) {
                        pBitMap[(uPixel + k) *
                                (unsigned long)pGF->stFileHeader.ucBpp + uPlane] = ucRepeatVal;
                    }
                    uPixel += (unsigned long)ucRepeatCount;
                } else {
                    pBitMap[uPixel * (unsigned long)pGF->stFileHeader.ucBpp + uPlane] = ucByte;
                    uPixel++;
                }
            }
        }
    }

    FIL_fn_vCloseFileInConcatFile(&hStream);
}

void __cdecl GLI_pc_fn_vLoadTextureSourceA(void *p_stTexture, char *p_szFileName, uint32_t *pBuffer)
{
    GLI_tdstTexture *pTex = (GLI_tdstTexture *)p_stTexture;
    FIL_tdstGF     stGF;
    char           szFile[260];
    unsigned char  ucBpp_shifted;
    uint32_t       ulPixCount;
    uint32_t       ulIdx;
    unsigned char *p_ucSrc;
    uint32_t       ulFlags;

    strcpy(szFile, p_szFileName);

    /* 0x004a07b0 */
    strcpy(strrchr(szFile, '.'), ".gf");

    stGF.stFileHeader.lChromakeyIndex = 0;
    stGF.p_ucBitMap  = NULL;
    stGF.p_ucPalette = NULL;

    FIL_LoadGFLikeImageToRecord(GLI_g_hConcatTexturesFile,
                                GLI_fn_szGetPathOfTexture(),
                                szFile,
                                &stGF);

    if (stGF.stFileHeader.ucBpp == 0) {
        return;
    }

    ucBpp_shifted = (unsigned char)(stGF.stFileHeader.ucBpp * 8);
    pTex->uwWidth       = (unsigned short)stGF.stFileHeader.ulWidth;
    pTex->uwRealWidth   = (unsigned short)stGF.stFileHeader.ulWidth;
    stGF.stFileHeader.ucBpp          = ucBpp_shifted;
    pTex->uwHeight      = (unsigned short)stGF.stFileHeader.ulHeight;
    pTex->uwRealHeight  = (unsigned short)stGF.stFileHeader.ulHeight;

    if (ucBpp_shifted == 32) {

        uint32_t ulAlphaAnd = 0xffffffffu;
        pTex->ulTextureCaps |= 0x08u;
        ulPixCount = (uint32_t)(stGF.stFileHeader.ulWidth & 0xffff)
                   * (uint32_t)(stGF.stFileHeader.ulHeight & 0xffff);
        p_ucSrc = stGF.p_ucBitMap + 1;
        for (ulIdx = 0; ulIdx < ulPixCount; ulIdx++) {
            uint32_t ulPixel =
                ((uint32_t)p_ucSrc[2] << 24) |
                ((uint32_t)p_ucSrc[-1] << 16) |
                ((uint32_t)p_ucSrc[0]  <<  8) |
                ((uint32_t)p_ucSrc[1]);
            *pBuffer++   = ulPixel;
            ulAlphaAnd  &= ulPixel;
            p_ucSrc     += 4;
        }
        if ((ulAlphaAnd & 0xf0000000u) == 0xf0000000u) {
            pTex->ulTextureCaps &= 0xfffffff7u;
        }
        ulFlags = pTex->ulTextureCaps;
        if ((ulFlags & 0x08u) != 0 && (ulFlags & 0x20u) != 0) {
            pTex->ulTextureCaps = ulFlags & 0xffffffdfu;
        }
    } else if (ucBpp_shifted == 24) {

        ulPixCount = (uint32_t)(stGF.stFileHeader.ulWidth & 0xffff)
                   * (uint32_t)(stGF.stFileHeader.ulHeight & 0xffff);
        if (ulPixCount > 0) {
            p_ucSrc = stGF.p_ucBitMap + 2;
            for (ulIdx = 0; ulIdx < ulPixCount; ulIdx++) {
                *pBuffer++ = ((uint32_t)p_ucSrc[-2] << 16) |
                             ((uint32_t)p_ucSrc[-1] <<  8) |
                             ((uint32_t)p_ucSrc[ 0]);
                p_ucSrc   += 3;
            }
        }
    } else if (ucBpp_shifted == 8) {

        if ((GLI_pc_g_stGliCaps.ulTextureCaps & E_TEX_Support8P) == 0) {

            void          *p_vPalToFree = pTex->p_vColorTable;
            unsigned char *p_ucPal      = (unsigned char *)p_vPalToFree;
            uint32_t       ulOff;
            for (ulOff = 0; ulOff < 0x400u; ulOff += 4) {
                unsigned char ucTmp = p_ucPal[ulOff];
                p_ucPal[ulOff]      = p_ucPal[ulOff + 2];
                p_ucPal[ulOff + 2]  = ucTmp;
            }
            ulPixCount = (uint32_t)(pTex->uwWidth) *
                         (uint32_t)(pTex->uwHeight);
            for (ulIdx = 0; ulIdx < ulPixCount; ulIdx++) {
                unsigned char ucIndex = ((unsigned char *)stGF.p_ucBitMap)[ulIdx];
                *pBuffer++ = *(uint32_t *)(p_ucPal + (uint32_t)ucIndex * 4);
            }

            GLI_pc_fn_vFree(&p_vPalToFree);
        } else {

            unsigned char *p_ucPal = (unsigned char *)stGF.p_ucPalette;
            uint32_t       ulOff;
            uint32_t       ulBytes;
            pTex->p_vColorTable = p_ucPal;
            for (ulOff = 3; ulOff < 0x400u; ulOff += 4) {
                p_ucPal[ulOff] = 0xffu;
            }
            ulBytes = ((uint32_t)(pTex->uwWidth) * 8 *
                       (uint32_t)(pTex->uwHeight)) >> 3;

            {
                uint32_t *p_ulDst = pBuffer;
                uint32_t *p_ulSrc = (uint32_t *)stGF.p_ucBitMap;
                uint32_t  ulDw    = ulBytes >> 2;
                uint32_t  ulB     = ulBytes & 3u;
                while (ulDw--) { *p_ulDst++ = *p_ulSrc++; }
                {
                    unsigned char *p_ucDst = (unsigned char *)p_ulDst;
                    unsigned char *p_ucS   = (unsigned char *)p_ulSrc;
                    while (ulB--) { *p_ucDst++ = *p_ucS++; }
                }
            }
        }
    } else {

        ERM_fn_vUpdateLastError(4, g_ucGEOModuleId, 0, 6, -1, 0xff, 0xff, p_szFileName);
    }

    GLI_pc_fn_vFreeIfNotNull((void **)&stGF.p_ucBitMap);
}

void __cdecl GLI_pc_fn_vLoadTextureSourceB(void *p_stTexture, char *p_szFileName, uint32_t *pBuffer)
{
    GLI_tdstTexture *pTex = (GLI_tdstTexture *)p_stTexture;
    FIL_tdstGF     stGF;
    void          *p_vTakeOwnership;
    char           szFile[260];
    unsigned char  ucBpp_shifted;
    uint32_t       ulPixCount;
    uint32_t       ulIdx;
    unsigned char *p_ucSrc;

    strcpy(szFile, p_szFileName);
    p_vTakeOwnership = NULL;
    strcpy(strrchr(szFile, '.'), ".gf");

    stGF.stFileHeader.lChromakeyIndex = 0;

    FIL_LoadGFLikeImageToRecord(GLI_g_hConcatTexturesFile,
                                GLI_fn_szGetPathOfTexture(),
                                szFile,
                                &stGF);

    p_vTakeOwnership = stGF.p_ucBitMap;
    /* 0x42652A */
    if (stGF.p_ucBitMap != NULL) {
        ucBpp_shifted = (unsigned char)(stGF.stFileHeader.ucBpp * 8);
        pTex->uwWidth      = (unsigned short)stGF.stFileHeader.ulWidth;
        pTex->uwRealWidth  = (unsigned short)stGF.stFileHeader.ulWidth;
        pTex->uwHeight     = (unsigned short)stGF.stFileHeader.ulHeight;
        pTex->uwRealHeight = (unsigned short)stGF.stFileHeader.ulHeight;
        stGF.stFileHeader.ucBpp = ucBpp_shifted;

        if (ucBpp_shifted == 32) {
            uint32_t ulAlphaAnd = 0xffffffffu;
            pTex->ulTextureCaps |= 0x08u;
            ulPixCount = (uint32_t)(stGF.stFileHeader.ulHeight & 0xffff)
                       * (uint32_t)(stGF.stFileHeader.ulWidth  & 0xffff);
            p_ucSrc = stGF.p_ucBitMap + 1;
            for (ulIdx = 0; ulIdx < ulPixCount; ulIdx++) {
                uint32_t ulPixel =
                    ((uint32_t)p_ucSrc[2] << 24) |
                    ((uint32_t)p_ucSrc[-1] << 16) |
                    ((uint32_t)p_ucSrc[0]  <<  8) |
                    ((uint32_t)p_ucSrc[1]);
                *pBuffer++   = ulPixel;
                ulAlphaAnd  &= ulPixel;
                p_ucSrc     += 4;
            }
            if ((ulAlphaAnd & 0xf0000000u) == 0xf0000000u) {
                pTex->ulTextureCaps &= 0xfffffff7u;
            }

        } else if (ucBpp_shifted == 24) {

            ulPixCount = (uint32_t)(stGF.stFileHeader.ulHeight & 0xffff)
                       * (uint32_t)(stGF.stFileHeader.ulWidth  & 0xffff);
            if (ulPixCount > 0) {
                p_ucSrc = stGF.p_ucBitMap + 2;
                for (ulIdx = 0; ulIdx < ulPixCount; ulIdx++) {
                    *pBuffer++ = ((uint32_t)p_ucSrc[-2] << 16) |
                                 ((uint32_t)p_ucSrc[-1] <<  8) |
                                 ((uint32_t)p_ucSrc[ 0]);
                    p_ucSrc   += 3;
                }
            }
        } else if (ucBpp_shifted == 8) {

            void *p_vPalette;

            pTex->ulTextureCaps |= 0x80u;
            p_vPalette = GLI_pc_fn_p_vAlloc(0x400);
            pTex->p_vColorTable = p_vPalette;
            memcpy(p_vPalette, stGF.p_ucBitMap, 0x400);
            memcpy(pBuffer, stGF.p_ucBitMap + 0x400,
                   (uint32_t)(pTex->uwWidth) *
                   (uint32_t)(pTex->uwHeight) * 8);
        } else {
            ERM_fn_vUpdateLastError(4, g_ucGEOModuleId, 0, 6, -1, 0xff, 0xff, p_szFileName);
        }
    }

    GLI_pc_fn_vFreeIfNotNull((void **)&p_vTakeOwnership);
}

/* 0x00426550 */
void __cdecl GLI_xLoadTextureInTexelField(void *p_stTexture,
                                          uint32_t *pBuffer,
                                          int bHideProgress)
{
    GLI_tdstTexture *pTex = (GLI_tdstTexture *)p_stTexture;
    unsigned short  uwSavedHeight, uwSavedWidth;
    unsigned char   ucCyclingMode;
    uint32_t        ulCaps;
    unsigned short  uwWorkW, uwWorkH;

    if (p_stTexture == NULL) {
        return;
    }

    GLI_g_ulNumberOfLoadedTexture++;
    if (bHideProgress == 0
            && FUN_0044f4e0() != 1
            && GLI_g_ulNumberOfLoadedTexture % 0x14u == 0) {
        GLI_AddToProgressBar(1);
    }

    uwSavedHeight = pTex->uwHeight;
    uwSavedWidth  = pTex->uwWidth;
    ulCaps        = pTex->ulTextureCaps;

    if ((ulCaps & 0x80000000u) != 0) {
        GLI_pc_fn_vLoadTextureSourceA(p_stTexture, pTex->szFileName, pBuffer);
    } else if ((ulCaps & 0x40000000u) != 0) {
        GLI_pc_fn_vLoadTextureSourceB(p_stTexture, pTex->szFileName, pBuffer);
    } else {
        return;
    }

    pTex->uwHeight = uwSavedHeight;
    pTex->p_vBitMap = pBuffer;
    pTex->uwWidth = uwSavedWidth;

    if ((pTex->ucCyclingMode & 0x10) != 0) {
        pTex->ucCyclingMode |= 0x0c;
    }
    ucCyclingMode = pTex->ucCyclingMode;
    if ((ucCyclingMode & 0x0c) != 0 && (GLI_pc_g_stGliCaps.ulTextureCaps & E_TEX_SupportMirror) == 0) {
        if ((ucCyclingMode & 0x04) != 0) {
            if ((ucCyclingMode & 0x10) != 0) {
                pTex->ucCyclingMode = ucCyclingMode & 0xfbu;
            }
            pTex->ucCyclingMode |= 0x01u;
            GLI_pc_fn_vExpandPixelsBy2_32(pBuffer,
                             (int)pTex->uwRealWidth,
                             (int)pTex->uwRealHeight);
            pTex->uwRealWidth = (unsigned short)(pTex->uwRealWidth << 1);
        }
        ucCyclingMode = pTex->ucCyclingMode;
        if ((ucCyclingMode & 0x08) != 0) {

            pTex->ucCyclingMode = ucCyclingMode;
            if ((ucCyclingMode & 0x10) == 0) {
                GLI_pc_fn_vExpandRows32A(pBuffer,
                                 (int)pTex->uwRealWidth,
                                 (int)pTex->uwRealHeight);
            } else {
                pTex->ucCyclingMode = ucCyclingMode & 0xf7u;
                GLI_pc_fn_vExpandRows32B(pBuffer,
                                 (int)pTex->uwRealWidth,
                                 (int)pTex->uwRealHeight);
            }
            pTex->uwRealHeight = (unsigned short)(pTex->uwRealHeight << 1);
        }
    }

    if ((pTex->ulTextureCaps & 0x400u) != 0) {
        int lHalfPixels;
        pTex->uwRealHeight =
            (unsigned short)(pTex->uwRealHeight >> 1);
        lHalfPixels = (int)pTex->uwRealWidth *
                      (int)pTex->uwRealHeight;
        memmove(pBuffer, pBuffer + lHalfPixels, (size_t)lHalfPixels * 4);
    }

    uwWorkW = pTex->uwRealHeight;
    uwWorkH = pTex->uwRealWidth;
    ulCaps  = pTex->ulTextureCaps;
    if ((ulCaps & 2u) == 0 || (ulCaps & 8u) != 0) {

        while (pTex->uwWidth < uwWorkH) {
            GLI_DownscalePairs32_Horizontal(pBuffer, pBuffer, (int)uwWorkW, (int)uwWorkH);
            uwWorkH = (unsigned short)(uwWorkH >> 1);
        }
        while (pTex->uwHeight < uwWorkW) {
            GLI_DownscaleRows32_Vertical(pBuffer, pBuffer, (int)uwWorkW, (int)uwWorkH);
            uwWorkW = (unsigned short)(uwWorkW >> 1);
        }
    } else {

        while (pTex->uwWidth < uwWorkH) {
            GLI_pc_fn_vDownscalePairs32_ChromaH(pBuffer, pBuffer, (int)uwWorkW, (int)uwWorkH, pTex->ulChromakeyColorRGBA);
            uwWorkH = (unsigned short)(uwWorkH >> 1);
        }
        while (pTex->uwHeight < uwWorkW) {
            GLI_pc_fn_vDownscaleRows32_ChromaV(pBuffer, pBuffer, (int)uwWorkW, (int)uwWorkH, pTex->ulChromakeyColorRGBA);
            uwWorkW = (unsigned short)(uwWorkW >> 1);
        }
    }

    ulCaps = pTex->ulTextureCaps;
    if ((ulCaps & 0x902u) != 0) {
        if ((GLI_pc_g_stGliCaps.ulDriverCaps & E_DRV_SupportAlphaTest) != 0) {
            if ((ulCaps & 0x808u) != 0) {
                pTex->ulTextureCaps = (ulCaps & 0xffffffe5u) | 0x900u;
                return;
            }
            if ((ulCaps & 0x10u) == 0) {
                GLI_pc_fn_vApplyColorKeyAlpha(pBuffer,
                                 (int)pTex->uwWidth,
                                 (int)pTex->uwHeight,
                                 pTex->ulChromakeyColorRGBA);
            } else {
                GLI_pc_fn_vColorKeyNeighborhoodPassA(pBuffer,
                                 (uint32_t)pTex->uwWidth,
                                 (int)pTex->uwHeight,
                                 pTex->ulChromakeyColorRGBA);
            }
            pTex->ulTextureCaps =
                (pTex->ulTextureCaps & 0xffffffedu) | 0x100u;
            return;
        }
        if ((ulCaps & 0x808u) != 0) {
            GLI_pc_fn_vReplaceLowAlphaPixels(pBuffer,
                             (int)pTex->uwRealWidth,
                             (int)pTex->uwRealHeight,
                             pTex->ulChromakeyColorRGBA);
            pTex->ulTextureCaps =
                (pTex->ulTextureCaps & 0xffffffe7u) | 0x800u;
            return;
        }
        if ((ulCaps & 0x10u) != 0) {
            GLI_pc_fn_vColorKeyNeighborhoodPassB((int)(intptr_t)p_stTexture, (uint32_t)(uintptr_t)pBuffer);
        }
    }
}

/* 0x0045f030 */
extern long __cdecl _ftol(void);

__declspec(naked) void __cdecl GLI_BlendPixelIntoUiSurface(
    int p_stUiObject, int x, int y, float *p_xBlendDesc)
{
    (void)p_stUiObject; (void)x; (void)y; (void)p_xBlendDesc;
    __asm {
        /* 0x0045f030 */
        sub  esp, 0x18
        push ebx
        mov  ebx, dword ptr [esp + 0x24]
        push ebp
        push esi
        test ebx, ebx
        push edi
        jl   ret_path
        mov  esi, dword ptr [esp + 0x2c]
        mov  ecx, dword ptr [esi + 0x210]
        cmp  ebx, ecx
        jge  ret_path
        mov  edi, dword ptr [esp + 0x34]
        test edi, edi
        jl   ret_path
        cmp  edi, dword ptr [esi + 0x214]
        jge  ret_path

        mov  eax, dword ptr [esi + 0x218]
        cmp  eax, 1
        jnz  fmt2_or_3

        mov  edx, edi
        mov  eax, dword ptr [esp + 0x38]
        imul edx, ecx
        fld  dword ptr [k_xBP_One_bits]
        fld  dword ptr [eax]
        fld  dword ptr [eax + 0x4]
        fld  dword ptr [eax + 0x8]
        fxch st(3)
        fsub dword ptr [eax + 0xc]
        mov  ebp, dword ptr [esi + 0x20c]
        add  edx, ebx
        xor  ecx, ecx
        mov  cl, byte ptr [ebp + edx*4]
        lea  ebp, [ebp + edx*4]
        mov  dword ptr [esp + 0x38], ecx
        xor  edx, edx
        fild dword ptr [esp + 0x38]
        mov  dl, byte ptr [ebp + 0x1]
        xor  ecx, ecx
        fxch st(3)
        fmul st(0), st(1)
        mov  dword ptr [esp + 0x38], edx
        mov  cl, byte ptr [ebp + 0x2]
        fild dword ptr [esp + 0x38]
        fxch st(3)
        fmul st(0), st(2)
        fxch st(3)
        fmul qword ptr [k_dBP_RecipFF_bits]
        mov  dword ptr [esp + 0x38], ecx
        fmul dword ptr [eax + 0xc]
        faddp st(3), st(0)
        fxch st(4)
        fmul st(0), st(1)
        fild dword ptr [esp + 0x38]
        fxch st(3)
        fstp st(2)
        fxch st(3)
        fmul qword ptr [k_dBP_RecipFF_bits]
        fld  dword ptr [eax + 0xc]
        fxch st(3)
        fmul qword ptr [k_dBP_RecipFF_bits]
        fmul dword ptr [eax + 0xc]
        faddp st(4), st(0)
        fxch st(2)
        fmulp st(2), st(0)
        fxch
        faddp st(3), st(0)
        fxch
        fstp dword ptr [esp + 0x20]
        fxch
        fst  dword ptr [esp + 0x38]
        fmul dword ptr [k_xBP_255_bits]
        call _ftol
        fmul dword ptr [k_xBP_255_bits]
        mov  byte ptr [ebp], al
        call _ftol
        mov  edx, edi
        mov  ecx, dword ptr [esi + 0x20c]
        imul edx, dword ptr [esi + 0x210]
        fld  dword ptr [esp + 0x20]
        fmul dword ptr [k_xBP_255_bits]
        add  edx, ebx
        mov  byte ptr [ecx + edx*4 + 0x1], al
        call _ftol
        imul edi, dword ptr [esi + 0x210]
        mov  edx, dword ptr [esi + 0x20c]
        add  edi, ebx
        mov  byte ptr [edx + edi*4 + 0x2], al

        pop  edi
        pop  esi
        pop  ebp
        pop  ebx
        add  esp, 0x18
        ret

    fmt2_or_3:
        cmp  eax, 2
        jnz  fmt3_or_end

        imul edi, ecx
        mov  ecx, dword ptr [esp + 0x30]
        mov  edx, dword ptr [esi + 0x22c]
        mov  eax, dword ptr [esi + 0x238]
        add  edi, ecx
        mov  ecx, dword ptr [esi + 0x20c]
        mov  ebx, 0x8
        mov  ebp, 0x8
        sub  ebx, edx
        mov  edx, dword ptr [esi + 0x244]
        sub  ebp, eax
        mov  eax, 0x8
        lea  edi, [ecx + edi*2]
        sub  eax, edx
        mov  dx, word ptr [esi + 0x24c]
        mov  cx, word ptr [edi]
        mov  dword ptr [esp + 0x2c], edi
        mov  dword ptr [esp + 0x30], ecx
        and  edx, ecx
        mov  cx, word ptr [esi + 0x248]
        mov  edi, dword ptr [esp + 0x38]
        shr  dx, cl
        fld  dword ptr [edi + 0xc]
        mov  ecx, eax
        mov  dword ptr [esp + 0x10], eax
        fsubr qword ptr [k_dBP_One_bits]
        shl  edx, cl
        and  edx, 0xffff
        mov  dword ptr [esp + 0x38], edx
        fild dword ptr [esp + 0x38]
        fmul dword ptr [edi + 0xc]
        fld  dword ptr [edi + 0x8]
        fmul st(0), st(2)
        fmul qword ptr [k_dBP_Neg255_bits]
        fsubp st(1), st(0)
        call _ftol

        mov  dx, word ptr [esi + 0x240]
        mov  ecx, dword ptr [esp + 0x30]
        and  edx, ecx
        mov  cx, word ptr [esi + 0x23c]
        shr  dx, cl
        mov  ecx, ebp
        movzx ax, al
        shl  edx, cl
        mov  word ptr [esp + 0x14], ax
        and  edx, 0xffff
        mov  dword ptr [esp + 0x38], edx
        fild dword ptr [esp + 0x38]
        fmul dword ptr [edi + 0xc]
        fld  dword ptr [edi + 0x4]
        fmul st(0), st(2)
        fmul qword ptr [k_dBP_Neg255_bits]
        fsubp st(1), st(0)
        call _ftol

        mov  dx, word ptr [esi + 0x234]
        mov  ecx, dword ptr [esp + 0x30]
        and  edx, ecx
        mov  cx, word ptr [esi + 0x230]
        shr  dx, cl
        mov  ecx, ebx
        movzx ax, al
        shl  edx, cl
        mov  word ptr [esp + 0x34], ax
        and  edx, 0xffff
        mov  dword ptr [esp + 0x38], edx
        fild dword ptr [esp + 0x38]
        fmul dword ptr [edi + 0xc]
        fld  dword ptr [edi]
        fmul st(0), st(2)
        fmul qword ptr [k_dBP_Neg255_bits]
        fsubp st(1), st(0)
        fstp st(1)
        call _ftol

        movzx ax, al
        mov  cx, bx
        shr  ax, cl
        mov  ecx, dword ptr [esi + 0x230]
        shl  eax, cl
        mov  edx, dword ptr [esp + 0x34]
        mov  cx, bp
        shr  dx, cl
        mov  ecx, dword ptr [esi + 0x23c]
        shl  edx, cl
        mov  cx, word ptr [esp + 0x10]
        or   eax, edx
        mov  edx, dword ptr [esp + 0x14]
        shr  dx, cl
        mov  ecx, dword ptr [esi + 0x248]
        shl  edx, cl
        mov  ecx, dword ptr [esp + 0x2c]
        or   eax, edx
        mov  word ptr [ecx], ax
        pop  edi
        pop  esi
        pop  ebp
        pop  ebx
        add  esp, 0x18
        ret

    fmt3_or_end:
        cmp  eax, 3
        jnz  ret_path

        mov  edx, edi
        mov  eax, dword ptr [esp + 0x38]
        imul edx, ecx
        fld  dword ptr [k_xBP_One_bits]
        fld  dword ptr [eax + 0x8]
        fld  dword ptr [eax]
        fld  dword ptr [eax + 0x4]
        mov  ecx, dword ptr [esi + 0x20c]
        add  edx, ebx
        lea  ebp, [ecx + edx*2]
        mov  dword ptr [esp + 0x2c], ebp
        mov  dl, byte ptr [ebp]
        mov  cl, byte ptr [ebp + 0x1]
        mov  byte ptr [esp + 0x38], dl
        mov  byte ptr [esp + 0x39], cl
        mov  ecx, dword ptr [esp + 0x38]
        mov  edx, ecx
        mov  ebp, ecx
        shr  edx, 7
        and  edx, 0xf8
        and  ebp, 0x1f
        mov  dword ptr [esp + 0x30], edx
        fild dword ptr [esp + 0x30]
        fxch st(4)
        fsub dword ptr [eax + 0xc]
        fxch st(4)
        fmul qword ptr [k_dBP_RecipFF_bits]
        shl  ebp, 3
        mov  dword ptr [esp + 0x30], ebp
        fild dword ptr [esp + 0x30]
        fxch st(5)
        fstp dword ptr [esp + 0x30]
        shr  ecx, 3
        and  ecx, 0x7c
        fxch st(3)
        fmul dword ptr [esp + 0x30]
        mov  dword ptr [esp + 0x34], ecx
        fild dword ptr [esp + 0x34]
        fxch st(5)
        fmul qword ptr [k_dBP_RecipFF_bits]
        fmul dword ptr [eax + 0xc]
        fxch st(3)
        fmul dword ptr [esp + 0x30]
        faddp st(3), st(0)
        fxch st(4)
        fmul qword ptr [k_dBP_RecipFF_bits]
        fxch st(3)
        fst  dword ptr [esp + 0x34]
        fmul dword ptr [eax + 0xc]
        fxch st(3)
        fst  dword ptr [esp + 0x34]
        fxch st(4)
        faddp st(3), st(0)
        fxch st(3)
        fmul dword ptr [eax + 0xc]
        fxch st(2)
        fst  dword ptr [esp + 0x20]
        fmul dword ptr [k_xBP_255_bits]
        fxch
        fxch st(3)
        fmul dword ptr [esp + 0x30]
        faddp st(2), st(0)
        call _ftol
        fmul dword ptr [k_xBP_255_bits]
        movzx bp, al
        and  ebp, 0x1f8
        shl  ebp, 5
        call _ftol
        fmul dword ptr [k_xBP_255_bits]
        movzx ax, al
        and  eax, 0x3ff8
        add  ebp, eax
        call _ftol
        shr  al, 3
        xor  cx, cx
        mov  cl, al
        lea  edx, [ecx + ebp*4]
        mov  ecx, dword ptr [esp + 0x2c]
        mov  word ptr [esp + 0x38], dx
        mov  al, byte ptr [esp + 0x38]
        mov  byte ptr [ecx], al
        imul edi, dword ptr [esi + 0x210]
        mov  edx, dword ptr [esi + 0x20c]
        mov  al, byte ptr [esp + 0x39]
        add  edi, ebx
        mov  byte ptr [edx + edi*2 + 0x1], al

    ret_path:
        pop  edi
        pop  esi
        pop  ebp
        pop  ebx
        add  esp, 0x18
        ret
    }
}

/* 0x0045f530 */
__declspec(naked) void __cdecl GLI_FillProgressBarSpan(
        int          ui_object,
        unsigned int old_progress,
        unsigned int new_progress)
{
    (void)ui_object; (void)old_progress; (void)new_progress;
    __asm {
        mov  eax, dword ptr [esp + 0x8]
        mov  ecx, dword ptr [esp + 0xc]
        sub  esp, 0x58
        cmp  eax, ecx
        push ebx
        push ebp
        push esi
        jae  exit_noedi
        mov  esi, dword ptr [esp + 0x68]
        mov  eax, dword ptr [esi + 0x27c]
        mov  ebp, dword ptr [esi + 0x274]
        mov  ebx, dword ptr [esi + 0x270]
        mov  edx, dword ptr [esi + 0x278]
        mov  ecx, eax
        sub  edx, ebx
        sub  ecx, ebp
        add  ebp, 2
        mov  dword ptr [esp + 0x1c], ecx
        lea  ecx, [eax - 2]
        cmp  ebp, ecx
        mov  dword ptr [esp + 0x20], edx
        mov  dword ptr [esp + 0x18], ebp
        ja   exit_noedi

        push edi
    row_init:
        mov  edi, dword ptr [esi + 0x274]
        sub  eax, ebp
        mov  dword ptr [esp + 0x6c], eax
        mov  eax, dword ptr [esp + 0x70]
        mov  ecx, ebp
        sub  ecx, edi
        lea  edi, [eax + ebx + 2]
        mov  eax, dword ptr [esp + 0x74]
        lea  eax, [eax + ebx + 1]
        cmp  edi, eax
        ja   next_row_jmp
        mov  ebp, dword ptr [esp + 0x20]
        mov  dword ptr [esp + 0x34], 0
        sub  ebp, ecx
        mov  ecx, dword ptr [esp + 0x20]
        mov  eax, ecx
        mov  dword ptr [esp + 0x28], ebp
        sub  eax, dword ptr [esp + 0x6c]
        imul ecx, edx
        mov  dword ptr [esp + 0x30], ecx
        mov  dword ptr [esp + 0x2c], eax
        fild qword ptr [esp + 0x30]
        fstp dword ptr [esp + 0x6c]
        jmp  pixel_compute

    pixel_loop_back:
        mov  ebp, dword ptr [esp + 0x28]
        mov  edx, dword ptr [esp + 0x24]

    pixel_compute:
        mov  ecx, dword ptr [esi + 0x278]
        mov  eax, edi
        sub  eax, ebx
        sub  ecx, edi
        mov  ebx, edx
        sub  edx, ecx
        sub  ebx, eax
        mov  eax, dword ptr [esp + 0x2c]
        mov  dword ptr [esp + 0x14], edx
        mov  ecx, eax
        imul eax, dword ptr [esp + 0x14]
        imul edx, ebp
        fld  dword ptr [esi + 0x2c8]
        fld  dword ptr [esi + 0x2d8]
        mov  dword ptr [esp + 0x10], ebx
        mov  dword ptr [esp + 0x18], ecx
        imul ecx, dword ptr [esp + 0x10]
        imul ebx, ebp
        mov  dword ptr [esp + 0x38], eax
        xor  eax, eax
        mov  dword ptr [esp + 0x3c], eax
        mov  dword ptr [esp + 0x40], ecx
        fild qword ptr [esp + 0x38]
        mov  dword ptr [esp + 0x44], eax
        mov  dword ptr [esp + 0x48], edx
        fild qword ptr [esp + 0x40]
        mov  dword ptr [esp + 0x4c], eax
        mov  dword ptr [esp + 0x50], ebx
        fild qword ptr [esp + 0x48]
        mov  dword ptr [esp + 0x54], eax
        fild qword ptr [esp + 0x50]
        fld  st(1)
        fxch
        fst  dword ptr [esp + 0x10]
        fxch st(4)
        fst  dword ptr [esp + 0x14]
        fxch st(3)
        fst  dword ptr [esp + 0x18]
        fxch st(3)
        fmul dword ptr [esi + 0x2d0]
        fdiv dword ptr [esp + 0x6c]
        fxch st(3)
        fmul dword ptr [esi + 0x2c0]
        fdiv dword ptr [esp + 0x6c]
        faddp st(3), st(0)
        fld  dword ptr [esp + 0x14]
        fxch
        fmul dword ptr [esi + 0x2b0]
        fdiv dword ptr [esp + 0x6c]
        faddp st(3), st(0)
        fld  dword ptr [esp + 0x18]
        fld  dword ptr [esp + 0x10]
        fmul dword ptr [esi + 0x2a0]
        fdiv dword ptr [esp + 0x6c]
        faddp st(4), st(0)
        fxch st(3)
        fstp dword ptr [esp + 0x58]
        fmul dword ptr [esi + 0x2d4]
        fld  dword ptr [esp + 0x10]
        fxch
        fdiv dword ptr [esp + 0x6c]
        fxch st(3)
        fmul dword ptr [esi + 0x2c4]
        fdiv dword ptr [esp + 0x6c]
        faddp st(3), st(0)
        fxch st(5)
        fmul dword ptr [esp + 0x18]
        fld  st(1)
        fxch st(4)
        fmul dword ptr [esi + 0x2a4]
        fdiv dword ptr [esp + 0x6c]
        faddp st(3), st(0)
        fxch st(4)
        fmul dword ptr [esp + 0x14]
        fxch st(4)
        fdiv dword ptr [esp + 0x6c]
        fxch st(4)
        fdiv dword ptr [esp + 0x6c]
        fxch st(5)
        fmul dword ptr [esi + 0x2a8]
        fld  st(1)
        fxch st(4)
        fmul dword ptr [esi + 0x2b4]
        mov  eax, dword ptr [esi + 0x278]
        add  eax, -2
        fdiv dword ptr [esp + 0x6c]
        cmp  edi, eax
        faddp st(3), st(0)
        fxch st(5)
        faddp st(4), st(0)
        fxch st(4)
        fdiv dword ptr [esp + 0x6c]
        faddp st(3), st(0)
        fstp dword ptr [esp + 0x5c]
        fmul dword ptr [esi + 0x2b8]
        fdiv dword ptr [esp + 0x6c]
        faddp st(1), st(0)
        fstp dword ptr [esp + 0x60]
        fld  dword ptr [esi + 0x2ac]
        fld  dword ptr [esi + 0x2bc]
        fmul st, st(2)
        fld  dword ptr [esi + 0x2dc]
        fxch st(2)
        fmul dword ptr [esp + 0x10]
        fxch
        fdiv dword ptr [esp + 0x6c]
        fxch
        fdiv dword ptr [esp + 0x6c]
        fxch st(2)
        fmul dword ptr [esp + 0x14]
        fld  dword ptr [esi + 0x2cc]
        fmul dword ptr [esp + 0x18]
        fxch
        fdiv dword ptr [esp + 0x6c]
        fxch
        fdiv dword ptr [esp + 0x6c]
        fxch st(3)
        faddp st(2), st(0)
        faddp st(1), st(0)
        fxch
        faddp st(1), st(0)
        fstp dword ptr [esp + 0x64]
        fstp st(0)

        ja   skip_blend
        mov  ebp, dword ptr [esp + 0x1c]
        lea  ecx, [esp + 0x58]
        push ecx
        push ebp
        push edi
        push esi
        call GLI_BlendPixelIntoUiSurface      /* 0x0045f030 */
        add  esp, 0x10
        jmp  after_blend

    skip_blend:
        mov  ebp, dword ptr [esp + 0x1c]

    after_blend:
        mov  ebx, dword ptr [esi + 0x270]
        mov  edx, dword ptr [esp + 0x74]
        inc  edi
        lea  eax, [edx + ebx + 1]
        cmp  edi, eax
        jbe  pixel_loop_back

        mov  edx, dword ptr [esp + 0x24]
    next_row_jmp:
        mov  eax, dword ptr [esi + 0x27c]
        inc  ebp
        mov  dword ptr [esp + 0x1c], ebp
        lea  ecx, [eax - 2]
        cmp  ebp, ecx
        jbe  row_init
        pop  edi
    exit_noedi:
        pop  esi
        pop  ebp
        pop  ebx
        add  esp, 0x58
        ret
    }
}

/* 0x0045f420 */
__declspec(naked) void __cdecl GLI_DrawProgressBarFrameAndFill(int ui_object)
{
    (void)ui_object;
    __asm {
        mov  eax, dword ptr [GLI_AA_ProgressBarDivisor]  /* 0x00516640 */
        push ebx
        push ebp
        push esi
        test eax, eax
        push edi
        jnz  skip_init
        mov  dword ptr [GLI_AA_ProgressBarDivisor], 0x64
    skip_init:
        mov  esi, dword ptr [esp + 0x14]
        mov  ebx, dword ptr [esi + 0x274]
        mov  eax, dword ptr [esi + 0x27c]
        cmp  ebx, eax
        ja   skip_fill
    fill_row:
        mov  edi, dword ptr [esi + 0x270]
        mov  eax, dword ptr [esi + 0x278]
        cmp  edi, eax
        ja   fill_row_done
        lea  ebp, [esi + 0x290]
    fill_pixel:
        push ebp
        push ebx
        push edi
        push esi
        call GLI_BlendPixelIntoUiSurface
        mov  eax, dword ptr [esi + 0x278]
        add  esp, 0x10
        inc  edi
        cmp  edi, eax
        jbe  fill_pixel
    fill_row_done:
        mov  eax, dword ptr [esi + 0x27c]
        inc  ebx
        cmp  ebx, eax
        jbe  fill_row
    skip_fill:
        mov  edi, dword ptr [esi + 0x270]
        mov  eax, dword ptr [esi + 0x278]
        inc  edi
        dec  eax
        cmp  edi, eax
        ja   skip_hborder
        lea  ebx, [esi + 0x280]
    hborder_col:
        mov  ecx, dword ptr [esi + 0x274]
        push ebx
        push ecx
        push edi
        push esi
        call GLI_BlendPixelIntoUiSurface
        mov  edx, dword ptr [esi + 0x27c]
        add  esp, 0x10
        push ebx
        push edx
        push edi
        push esi
        call GLI_BlendPixelIntoUiSurface
        mov  eax, dword ptr [esi + 0x278]
        add  esp, 0x10
        inc  edi
        dec  eax
        cmp  edi, eax
        jbe  hborder_col
    skip_hborder:
        mov  edi, dword ptr [esi + 0x274]
        mov  eax, dword ptr [esi + 0x27c]
        cmp  edi, eax
        ja   skip_vborder
        lea  ebx, [esi + 0x280]
    vborder_row:
        mov  ecx, dword ptr [esi + 0x270]
        push ebx
        push edi
        push ecx
        push esi
        call GLI_BlendPixelIntoUiSurface
        mov  edx, dword ptr [esi + 0x278]
        add  esp, 0x10
        push ebx
        push edi
        push edx
        push esi
        call GLI_BlendPixelIntoUiSurface
        mov  eax, dword ptr [esi + 0x27c]
        add  esp, 0x10
        inc  edi
        cmp  edi, eax
        jbe  vborder_row
    skip_vborder:
        mov  eax, dword ptr [GLI_AA_ProgressBarCurPos]  /* 0x0051664c */
        push eax
        push 0
        push GLI_AA_ProgressUiObject
        call GLI_FillProgressBarSpan
        add  esp, 0x0c
        pop  edi
        pop  esi
        pop  ebp
        pop  ebx
        ret
    }
}

/* 0x0045f7e0 */
__declspec(naked) unsigned int __cdecl GLI_AddToProgressBar(int lIncrement)
{
    (void)lIncrement;
    __asm {
        mov  edx, dword ptr [GLI_AA_ProgressBarStart]
        mov  al,  byte ptr [GLI_AA_ProgressBarEnabled]
        push ebx
        push esi
        mov  esi, dword ptr [GLI_AA_ProgressBarEnd]
        push edi
        sub  esi, edx
        sub  esi, 3
        test al, al
        jz   ap_exit

        mov  ecx, dword ptr [GLI_AA_ProgressBarAccum]
        mov  edx, dword ptr [esp + 0x10]
        add  ecx, edx
        mov  eax, esi
        imul eax, ecx
        xor  edx, edx
        mov  edi, dword ptr [GLI_AA_ProgressBarLastPos]
        div  dword ptr [GLI_AA_ProgressBarDivisor]
        mov  dword ptr [GLI_AA_ProgressBarAccum], ecx
        mov  ecx, eax
        cmp  ecx, edi
        mov  dword ptr [GLI_AA_ProgressBarCurPos], ecx
        jbe  ap_exit

        mov  eax, 0xcccccccd
        mov  ebx, ecx
        mul  esi
        shr  edx, 3
        mov  eax, 0xcccccccd
        sub  ebx, edi
        mul  edx
        shr  edx, 2
        cmp  ebx, edx
        jbe  ap_exit

        push ecx
        push edi
        push GLI_AA_ProgressUiObject
        call GLI_FillProgressBarSpan
        add  esp, 0x0c
        push GLI_AA_ProgressUiObject
#ifdef RAYMAN2_FAST_LOADING
        call GLI_pc_fn_vRefreshProgressUiIfDue    /* bounded rate, the bar is in the buffer */
#else
        call GLI_RefreshUiSurfaceIfVisible
#endif /* RAYMAN2_FAST_LOADING */
        mov  eax, dword ptr [GLI_AA_ProgressBarCurPos]
        add  esp, 0x4
        mov  dword ptr [GLI_AA_ProgressBarLastPos], eax
    ap_exit:
        pop  edi
        pop  esi
        pop  ebx
        ret
    }
}

/* 0x00421fd0 */
void __cdecl GLI_pc_fn_vWrite16bBitmapToBackBuffer(void *p_vBuffer, long lPitch, long lHeight,
                                                   long lArg4, long lArg5, long lArg6, long lArg7)
{
#ifdef RAYMAN2_WIDESCREEN
    /* the bitmap (vignette, 4:3) is squeezed into its 4:3 band; size and rect unchanged */
    if ( GLI_g_xSafeAreaRatio < MTH_C_ONE )
        p_vBuffer = GLI_fn_p_vSqueezeBitmapToSafeArea(p_vBuffer, lPitch, lHeight);
#endif /* RAYMAN2_WIDESCREEN */
    ((void (__cdecl *)(void *, long, long, long, long, long, long))GLI_DRV_g_vWrite16bBitmapToBackBuffer)
        (p_vBuffer, lPitch, lHeight, lArg4, lArg5, lArg6, lArg7);
}

/* 0x0045eed0 */
__declspec(naked) void __cdecl GLI_RefreshUiSurfaceIfVisible(char *ui_object)
{
    (void)ui_object;
    __asm {
        sub  esp, 0x70
        push esi
        mov  esi, dword ptr [esp + 0x78]
        push GLI_AA_ProgressUiObject
        push esi
        call _stricmp                         /* 0x5163e0 */
        add  esp, 8
        test eax, eax
        jz   skip_self_refresh
        mov  eax, dword ptr [esi + 0x218]
        push eax
        push esi
        push esi
        call FUN_0045fbf0
        add  esp, 0x0c
    skip_self_refresh:
        mov  al,  byte ptr [esi + 0x25c]
        test al,  al
        jz   refresh_exit
        mov  ecx, dword ptr [esi + 0x26c]
        mov  eax, dword ptr [esi + 0x268]
        cmp  ecx, eax
        jbe  refresh_exit

        mov  edx, dword ptr [GLI_AA_hOverlaySemaphore]
        push -1
        push edx
        call dword ptr [WaitForSingleObject]
        mov  cx,  word ptr [GLI_AA_uwOverlayHeight]
        mov  dx,  word ptr [GLI_AA_uwOverlayWidth]
        lea  eax, [esp + 0x4]
        push eax
        push ecx
        push edx
        call GLD_bGetViewportAttributes
        mov  eax, dword ptr [esp + 0x34]
        mov  ecx, dword ptr [esp + 0x3c]
        mov  edx, dword ptr [esp + 0x30]
        add  esp, 0x0c
        push eax
        mov  eax, dword ptr [esp + 0x30]
        push ecx
        mov  ecx, dword ptr [esi + 0x214]
        push edx
        mov  edx, dword ptr [esi + 0x210]
        push eax
        mov  eax, dword ptr [esi + 0x20c]
        push ecx
        push edx
        push eax
        call GLI_pc_fn_vWrite16bBitmapToBackBuffer
        mov  cx,  word ptr [GLI_AA_uwOverlayWidth]
        add  esp, 0x1c
        push 0xff
        push ecx
        call GLD_bFlipDeviceWithSynchro       /* 0x420F50 */
        mov  edx, dword ptr [GLI_AA_hOverlaySemaphore]
        add  esp, 8
        push 0
        push 1
        push edx
        call dword ptr [ReleaseSemaphore]
    refresh_exit:
        pop  esi
        add  esp, 0x70
        ret
    }
}

/* 0x0045efb0 */
__declspec(naked) void __cdecl GLI_RefreshUiSurfaceUnsynchronized(char *ui_object)
{
    (void)ui_object;
    __asm {
        sub  esp, 0x70
        push esi
        mov  esi, dword ptr [esp + 0x78]
        push GLI_AA_ProgressUiObject
        push esi
        call _stricmp                         /* 0x5163e0 */
        add  esp, 8
        test eax, eax
        jz   skip_self_refresh
        mov  eax, dword ptr [esi + 0x218]
        push eax
        push esi
        push esi
        call FUN_0045fbf0
        add  esp, 0x0c
    skip_self_refresh:
        mov  dx,  word ptr [GLI_pc_g_uwOverlayViewport]
        mov  ax,  word ptr [GLI_pc_g_uwOverlayDevice]
        lea  ecx, [esp + 0x4]
        push ecx
        push edx
        push eax
        call GLD_bGetViewportAttributes
        mov  ecx, dword ptr [esp + 0x34]
        mov  edx, dword ptr [esp + 0x3c]
        mov  eax, dword ptr [esp + 0x30]
        add  esp, 0x0c
        push ecx
        mov  ecx, dword ptr [esp + 0x30]
        push edx
        mov  edx, dword ptr [esi + 0x214]
        push eax
        mov  eax, dword ptr [esi + 0x210]
        push ecx
        mov  ecx, dword ptr [esi + 0x20c]
        push edx
        push eax
        push ecx
        call GLI_pc_fn_vWrite16bBitmapToBackBuffer
        add  esp, 0x1c
        pop  esi
        add  esp, 0x70
        ret
    }
}
