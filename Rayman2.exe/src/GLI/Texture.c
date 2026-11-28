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

#define GLI_M_ColorKeySubNeighbour(ulPix)                                   \
    {                                                                       \
        ulNeigh = (ulPix);                                                  \
        if ((ulNeigh & 0xff000000u) != 0) {                                 \
            xR -= (float)((ulNeigh >> 16) & 0xffu) * (1.0f / -9.0f);        \
            xG -= (float)((ulNeigh >>  8) & 0xffu) * (1.0f / -9.0f);        \
            xB -= (float)( ulNeigh        & 0xffu) * (1.0f / -9.0f);        \
        }                                                                   \
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

/* 0x00425ae0 */
void __cdecl GLI_pc_fn_vAverageDwordPairsOfRows(unsigned long *p_ulSrc, long *p_lDst, int lSize, int lRowLen)
{
    int lRows;
    int lCol;

    lRows = lSize >> 1;
    if (1 < lRows && 0 < lRows) {
        do {
            lCol = lRowLen;
            if (0 < lRowLen) {
                do {
                    *p_lDst = (long)(((p_ulSrc[lRowLen] >> 1) & 0x7f7f7f7ful) + ((*p_ulSrc >> 1) & 0x7f7f7f7ful));
                    p_lDst = p_lDst + 1;
                    p_ulSrc = p_ulSrc + 1;
                    lCol = lCol - 1;
                } while (lCol != 0);
            }
            p_ulSrc = p_ulSrc + lRowLen;
            lRows = lRows - 1;
        } while (lRows != 0);
    }
}

void __cdecl FIL_LoadGFLikeImageToRecord(void *hConcatFile, char *p_szPath, char *p_szFilename, FIL_tdstGF *pGF);

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

void __cdecl GLI_pc_fn_vReplaceLowAlphaPixels(uint32_t *pPixels, int lHeight, int lWidth, uint32_t ulReplacement);

void __cdecl GLI_pc_fn_vApplyColorKeyAlpha(uint32_t *pPixels, int lHeight, int lWidth, uint32_t ulKey);

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
void __cdecl GLI_BlendPixelIntoUiSurface(int p_stUiObject,
                                         int x,
                                         int y,
                                         float *p_xBlendDesc)
{
    GLI_pc_tdstProgressUi *p_stUi;
    GLD_tdstPixelFormat   *p_stPf;
    unsigned char         *pucBuf;
    unsigned char         *pucPix;
    unsigned short        *pusPix;
    unsigned long          ulIdx;
    unsigned long          ulPix;
    unsigned long          ulOut;
    long                   lWidth;
    long                   lFormat;
    long                   lOld;
    long                   lT1;
    long                   lG8;
    long                   lB8;
    unsigned char          ucTerm0;
    unsigned char          ucTerm1;
    unsigned char          ucTerm2;
    float                  xA0;
    float                  xA1;
    float                  xA2;
    float                  xA3;
    float                  xOneMinusA3f;
    float                  xTerm2f;
    double                 dOneMinusA3;
    double                 dRecipFF;
    double                 dTerm0;
    double                 dTerm1;
    double                 dTerm2;

    p_stUi = (GLI_pc_tdstProgressUi *)p_stUiObject;
    if (x < 0 || x >= p_stUi->lBufferDimC || y < 0 || y >= p_stUi->lBufferDim8) {
        return;
    }
    lWidth = p_stUi->lBufferDimC;
    lFormat = p_stUi->lFormatKind;
    pucBuf = (unsigned char *)p_stUi->p_vBuffer;
    ulIdx = (unsigned long)(y * lWidth + x);
    xA0 = p_xBlendDesc[0];
    xA1 = p_xBlendDesc[1];
    xA2 = p_xBlendDesc[2];
    xA3 = p_xBlendDesc[3];

    if (lFormat == 1) {
        pucPix = pucBuf + ulIdx * 4;
        dOneMinusA3 = (double)GLI_BP_K_xOne - (double)xA3;
        dRecipFF = GLI_BP_K_dRecipFF;
        dTerm0 = (double)xA0 * dOneMinusA3 + ((double)pucPix[0] * dRecipFF) * (double)xA3;
        dTerm1 = (double)xA1 * dOneMinusA3 + ((double)pucPix[1] * dRecipFF) * (double)xA3;
        dTerm2 = (double)xA2 * dOneMinusA3 + ((double)pucPix[2] * dRecipFF) * (double)xA3;
        xTerm2f = (float)dTerm2;
        pucPix[0] = (unsigned char)(long)(dTerm0 * GLI_BP_K_x255);
        pucPix[1] = (unsigned char)(long)(dTerm1 * GLI_BP_K_x255);
        pucPix[2] = (unsigned char)(long)((double)xTerm2f * GLI_BP_K_x255);
        return;
    }

    if (lFormat == 2) {
        p_stPf = &p_stUi->stPixelFormat;
        pusPix = (unsigned short *)pucBuf + ulIdx;
        ulPix = (unsigned long)*pusPix;
        dOneMinusA3 = GLI_BP_K_dOne - (double)xA3;

        lOld = (long)(((ulPix & p_stPf->ulBlueMask) >> p_stPf->lBlueShift)
                      << (8 - p_stPf->lBlueNbBits));
        ucTerm2 = (unsigned char)(long)((double)lOld * (double)xA3
                                        - ((double)xA2 * dOneMinusA3) * GLI_BP_K_dNeg255);

        lOld = (long)(((ulPix & p_stPf->ulGreenMask) >> p_stPf->lGreenShift)
                      << (8 - p_stPf->lGreenNbBits));
        ucTerm1 = (unsigned char)(long)((double)lOld * (double)xA3
                                        - ((double)xA1 * dOneMinusA3) * GLI_BP_K_dNeg255);

        lOld = (long)(((ulPix & p_stPf->ulRedMask) >> p_stPf->lRedShift)
                      << (8 - p_stPf->lRedNbBits));
        ucTerm0 = (unsigned char)(long)((double)lOld * (double)xA3
                                        - ((double)xA0 * dOneMinusA3) * GLI_BP_K_dNeg255);

        ulOut = (((unsigned long)ucTerm0 >> (8 - p_stPf->lRedNbBits)) << p_stPf->lRedShift)
              | (((unsigned long)ucTerm1 >> (8 - p_stPf->lGreenNbBits)) << p_stPf->lGreenShift)
              | (((unsigned long)ucTerm2 >> (8 - p_stPf->lBlueNbBits)) << p_stPf->lBlueShift);
        *pusPix = (unsigned short)(ulOut & 0xffffu);
        return;
    }

    if (lFormat == 3) {
        pucPix = pucBuf + ulIdx * 2;
        ulPix = (unsigned long)pucPix[0] | ((unsigned long)pucPix[1] << 8);
        lT1 = (long)((ulPix >> 7) & 0xf8u);
        lG8 = (long)((ulPix >> 3) & 0x7cu);
        lB8 = (long)((ulPix & 0x1fu) << 3);
        xOneMinusA3f = (float)((double)GLI_BP_K_xOne - (double)xA3);
        dRecipFF = GLI_BP_K_dRecipFF;
        dTerm0 = ((double)lB8 * dRecipFF) * (double)xA3 + (double)xA0 * (double)xOneMinusA3f;
        dTerm1 = ((double)lG8 * dRecipFF) * (double)xA3 + (double)xA1 * (double)xOneMinusA3f;
        dTerm2 = ((double)lT1 * dRecipFF) * (double)xA3 + (double)xA2 * (double)xOneMinusA3f;
        ucTerm2 = (unsigned char)(long)(dTerm2 * GLI_BP_K_x255);
        ucTerm1 = (unsigned char)(long)(dTerm1 * GLI_BP_K_x255);
        ucTerm0 = (unsigned char)(long)(dTerm0 * GLI_BP_K_x255);
        ulOut = ((((unsigned long)ucTerm2 & 0xf8u) << 5)
                 + ((unsigned long)ucTerm1 & 0xf8u)) * 4u
              + ((unsigned long)ucTerm0 >> 3);
        pucPix[0] = (unsigned char)(ulOut & 0xffu);
        pucPix[1] = (unsigned char)((ulOut >> 8) & 0xffu);
        return;
    }
}

void __cdecl GLI_FillProgressBarSpan(int ui_object, unsigned int old_progress, unsigned int new_progress);

/* 0x0045f420 */
void __cdecl GLI_DrawProgressBarFrameAndFill(int ui_object)
{
    GLI_pc_tdstProgressUi *p_stUi;
    float *p_xBlend;
    unsigned long ulX;
    unsigned long ulY;
    unsigned long ulXEnd;
    unsigned long ulYEnd;
    long lYTop;
    long lYBottom;

    if (GLI_pc_g_lProgressBarDivisor == 0) {
        GLI_pc_g_lProgressBarDivisor = 100;
    }
    p_stUi = (GLI_pc_tdstProgressUi *)ui_object;
    ulY = (unsigned long)p_stUi->lBarYStart;
    ulYEnd = (unsigned long)p_stUi->lBarYEnd;
    if (ulY <= ulYEnd) {
        do {
            ulX = (unsigned long)p_stUi->lBarXStart;
            ulXEnd = (unsigned long)p_stUi->lBarXEnd;
            if (ulX <= ulXEnd) {
                p_xBlend = p_stUi->a_xBackgroundBlend;
                do {
                    GLI_BlendPixelIntoUiSurface(ui_object, (int)ulX, (int)ulY, p_xBlend);
                    ulXEnd = (unsigned long)p_stUi->lBarXEnd;
                    ulX++;
                } while (ulX <= ulXEnd);
            }
            ulYEnd = (unsigned long)p_stUi->lBarYEnd;
            ulY++;
        } while (ulY <= ulYEnd);
    }
    ulX = (unsigned long)p_stUi->lBarXStart + 1;
    ulXEnd = (unsigned long)p_stUi->lBarXEnd - 1;
    if (ulX <= ulXEnd) {
        p_xBlend = p_stUi->a_xBorderBlend;
        do {
            lYTop = p_stUi->lBarYStart;
            GLI_BlendPixelIntoUiSurface(ui_object, (int)ulX, lYTop, p_xBlend);
            lYBottom = p_stUi->lBarYEnd;
            GLI_BlendPixelIntoUiSurface(ui_object, (int)ulX, lYBottom, p_xBlend);
            ulXEnd = (unsigned long)p_stUi->lBarXEnd - 1;
            ulX++;
        } while (ulX <= ulXEnd);
    }
    ulY = (unsigned long)p_stUi->lBarYStart;
    ulYEnd = (unsigned long)p_stUi->lBarYEnd;
    if (ulY <= ulYEnd) {
        p_xBlend = p_stUi->a_xBorderBlend;
        do {
            lYTop = p_stUi->lBarXStart;
            GLI_BlendPixelIntoUiSurface(ui_object, lYTop, (int)ulY, p_xBlend);
            lYBottom = p_stUi->lBarXEnd;
            GLI_BlendPixelIntoUiSurface(ui_object, lYBottom, (int)ulY, p_xBlend);
            ulYEnd = (unsigned long)p_stUi->lBarYEnd;
            ulY++;
        } while (ulY <= ulYEnd);
    }
    GLI_FillProgressBarSpan((int)&GLI_pc_g_stProgressUi, 0, GLI_pc_g_lProgressBarCurPos);
}

/* 0x0045f530 */
void __cdecl GLI_FillProgressBarSpan(int ui_object,
                                     unsigned int old_progress,
                                     unsigned int new_progress)
{
    GLI_pc_tdstProgressUi *p_stUi;
    float (*pxCorner)[4];
    float a_xDesc[4];
    float xDiv;
    float xW2Pf;
    float xW2Qf;
    float xQRf;
    double dDiv;
    long lXStart;
    long lXEnd;
    long lYStart;
    long lYEnd;
    long lWidth;
    long lHeight;
    long lY;
    long lRowOff;
    long lHRem;
    long lRowW;
    long lW2;
    long lX;
    long lXLimit;
    long lP;
    long lQ;
    long lW2P;
    long lW2Q;
    long lPR;
    long lQR;

    if (old_progress >= new_progress) {
        return;
    }
    p_stUi = (GLI_pc_tdstProgressUi *)ui_object;
    pxCorner = p_stUi->a_xCornerBlend;
    lXStart = p_stUi->lBarXStart;
    lXEnd = p_stUi->lBarXEnd;
    lYStart = p_stUi->lBarYStart;
    lYEnd = p_stUi->lBarYEnd;
    lWidth = lXEnd - lXStart;
    lHeight = lYEnd - lYStart;
    lY = lYStart + 2;

    while ((unsigned long)lY <= (unsigned long)(lYEnd - 2)) {
        lRowOff = lY - p_stUi->lBarYStart;
        lHRem = lYEnd - lY;
        lX = (long)((unsigned long)old_progress + (unsigned long)lXStart + 2u);
        lXLimit = (long)((unsigned long)new_progress + (unsigned long)lXStart + 1u);
        if ((unsigned long)lX <= (unsigned long)lXLimit) {
            lRowW = lHeight - lRowOff;
            lW2 = lHeight - lHRem;
            xDiv = (float)(lHeight * lWidth);
            dDiv = (double)xDiv;
            do {
                lXEnd = p_stUi->lBarXEnd;
                lP = lWidth - (lXEnd - lX);
                lQ = lWidth - (lX - lXStart);
                lW2P = lW2 * lP;
                lW2Q = lW2 * lQ;
                lPR = lP * lRowW;
                lQR = lQ * lRowW;
                xW2Pf = (float)lW2P;
                xW2Qf = (float)lW2Q;
                xQRf = (float)lQR;

                a_xDesc[0] = (float)(((((double)lW2P * pxCorner[3][0]) / dDiv
                                       + ((double)lW2Q * pxCorner[2][0]) / dDiv)
                                       + ((double)lPR * pxCorner[1][0]) / dDiv)
                                       + ((double)xQRf * pxCorner[0][0]) / dDiv);
                a_xDesc[1] = (float)(((((double)xW2Pf * pxCorner[3][1]) / dDiv
                                       + ((double)xW2Qf * pxCorner[2][1]) / dDiv)
                                       + ((double)lQR * pxCorner[0][1]) / dDiv)
                                       + ((double)lPR * pxCorner[1][1]) / dDiv);
                a_xDesc[2] = (float)(((((double)xW2Qf * pxCorner[2][2]) / dDiv
                                       + ((double)xW2Pf * pxCorner[3][2]) / dDiv)
                                       + ((double)xQRf * pxCorner[0][2]) / dDiv)
                                       + ((double)lPR * pxCorner[1][2]) / dDiv);
                a_xDesc[3] = (float)(((((double)lPR * pxCorner[1][3]) / dDiv
                                       + ((double)xQRf * pxCorner[0][3]) / dDiv)
                                       + ((double)xW2Pf * pxCorner[3][3]) / dDiv)
                                       + ((double)xW2Qf * pxCorner[2][3]) / dDiv);

                if ((unsigned long)lX <= (unsigned long)(lXEnd - 2)) {
                    GLI_BlendPixelIntoUiSurface(ui_object, (int)lX, (int)lY, a_xDesc);
                }
                lXStart = p_stUi->lBarXStart;
                lX++;
            } while ((unsigned long)lX <= (unsigned long)(new_progress + (unsigned long)lXStart + 1u));
        }
        lYEnd = p_stUi->lBarYEnd;
        lY++;
    }
}

/* 0x0045f7e0 */
unsigned int __cdecl GLI_AddToProgressBar(int lIncrement)
{
    unsigned long ulStart;
    unsigned char ucEnabled;
    unsigned long ulSpan;
    unsigned long ulAccum;
    unsigned long ulNewPos;
    unsigned long ulLastPos;

#ifdef RAYMAN2_FIX_BUGS
    /* FIX(BUGS.md#8) */
    /* with the bar disabled, the result was never set: return the bar position, as when enabled */
    ulNewPos = GLI_pc_g_lProgressBarCurPos;
#endif /* RAYMAN2_FIX_BUGS */
    ulStart = GLI_pc_g_lProgressBarStart;
    ucEnabled = GLI_pc_g_bProgressBarEnabled;
    ulSpan = GLI_pc_g_lProgressBarEnd - ulStart - 3;
    if (ucEnabled != 0) {
        ulLastPos = GLI_pc_g_lProgressBarLastPos;
        ulAccum = GLI_pc_g_lProgressBarAccum + (unsigned long)lIncrement;
        ulNewPos = (ulSpan * ulAccum) / GLI_pc_g_lProgressBarDivisor;
        GLI_pc_g_lProgressBarAccum = ulAccum;
        GLI_pc_g_lProgressBarCurPos = ulNewPos;
        if (ulNewPos > ulLastPos && ulNewPos - ulLastPos > (ulSpan / 10) / 5) {
            GLI_FillProgressBarSpan((int)&GLI_pc_g_stProgressUi, ulLastPos, ulNewPos);
#ifdef RAYMAN2_FAST_LOADING
            GLI_fn_vRefreshProgressUiIfDue((char *)&GLI_pc_g_stProgressUi); /* bounded rate, the bar is in the buffer */
#else
            GLI_RefreshUiSurfaceIfVisible((char *)&GLI_pc_g_stProgressUi);
#endif /* RAYMAN2_FAST_LOADING */
            GLI_pc_g_lProgressBarLastPos = GLI_pc_g_lProgressBarCurPos;
        }
    }
    return ulNewPos;
}

void __cdecl GLI_pc_fn_vWrite16bBitmapToBackBuffer(void *p_vBuffer, long lPitch, long lHeight, long lArg4, long lArg5, long lArg6, long lArg7);
#include "GLI/mater_st.h"

/* 0x00424bb0 */
void __cdecl GLI_vLockAnimatedTexture(void *hMaterial)
{
    ((ACP_tdxHandleOfMaterial)hMaterial)->xIsLocked = 1;
}

/* 0x00424bc0 */
void __cdecl GLI_vUnLockAnimatedTexture(void *hMaterial)
{
    ((ACP_tdxHandleOfMaterial)hMaterial)->xIsLocked = 0;
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

#include "GAM/GAM_pc_DrawLock.h"
void __cdecl GLI_RefreshUiSurfaceIfVisible(char *ui_object)
{
    unsigned long a_ulViewAttr[0x1c];
    GLI_pc_tdstProgressUi *p_stUi;
    unsigned long ulCurPos;
    unsigned long ulLastPos;
    void *hSemaphore;

    p_stUi = (GLI_pc_tdstProgressUi *)ui_object;
    if (_stricmp(ui_object, (char *)&GLI_pc_g_stProgressUi) != 0) {
        FUN_0045fbf0(ui_object, ui_object, p_stUi->lFormatKind);
    }
    if (p_stUi->ucProgressBarEnabled == 0) {
        return;
    }
    ulCurPos = p_stUi->ulProgressBarCurPos;
    ulLastPos = p_stUi->ulProgressBarLastPos;
    if (ulCurPos <= ulLastPos) {
        return;
    }
    hSemaphore = GLI_pc_g_hOverlaySemaphore;
    GAM_pc_M_ulWaitDrawSemaphore(hSemaphore);
    GLD_bGetViewportAttributes(GLI_pc_g_uwOverlayDevice, GLI_pc_g_uwOverlayViewport, a_ulViewAttr);
    GLI_pc_fn_vWrite16bBitmapToBackBuffer(p_stUi->p_vBuffer, p_stUi->lBufferDimC, p_stUi->lBufferDim8,
                                          (long)a_ulViewAttr[0xa], (long)a_ulViewAttr[8],
                                          (long)a_ulViewAttr[0xb], (long)a_ulViewAttr[9]);
    GLD_bFlipDeviceWithSynchro(GLI_pc_g_uwOverlayDevice, 0xff);
    hSemaphore = GLI_pc_g_hOverlaySemaphore;
    GAM_pc_M_bReleaseDrawSemaphore(hSemaphore);
}

/* 0x0045efb0 */
void __cdecl GLI_RefreshUiSurfaceUnsynchronized(char *ui_object)
{
    unsigned long a_ulViewAttr[0x1c];
    GLI_pc_tdstProgressUi *p_stUi;

    p_stUi = (GLI_pc_tdstProgressUi *)ui_object;
    if (_stricmp(ui_object, (char *)&GLI_pc_g_stProgressUi) != 0) {
        FUN_0045fbf0(ui_object, ui_object, p_stUi->lFormatKind);
    }
    GLD_bGetViewportAttributes(GLI_pc_g_uwOverlayDevice, GLI_pc_g_uwOverlayViewport, a_ulViewAttr);
    GLI_pc_fn_vWrite16bBitmapToBackBuffer(p_stUi->p_vBuffer, p_stUi->lBufferDimC, p_stUi->lBufferDim8,
                                          (long)a_ulViewAttr[0xa], (long)a_ulViewAttr[8],
                                          (long)a_ulViewAttr[0xb], (long)a_ulViewAttr[9]);
}
