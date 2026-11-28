#include "GLI/texture_mipmap.h"
#include "GLI/GLI_globals.h"
#include "GLI/GLI_types.h"

/* 0x00425a90 */
void __cdecl GLI_pc_fn_vAverageDwordPairs(unsigned long *p_ulSrc, long *p_lDst, int lStride, int lSize)
{
    long *p_lEnd;
    long lHalf;

    lHalf = lSize >> 1;
    if (1 < lHalf) {
        p_lEnd = p_lDst + lHalf * lStride;
        for (; p_lDst < p_lEnd; p_lDst = p_lDst + 1) {
            *p_lDst = (long)(((p_ulSrc[1] >> 1) & 0x7f7f7f7ful) + ((*p_ulSrc >> 1) & 0x7f7f7f7ful));
            p_ulSrc = p_ulSrc + 2;
        }
    }
}

#include "GAM/MS_Micro.h"

unsigned char __cdecl fn_bMicroGetActivationFlag(void *h_MS_Micro)
{
   return( ((MS_tdxHandleToMicro)h_MS_Micro)->bIsActive );
}

/* 0x0042d1c0 */
int __cdecl GLI_pc_fn_lHalveTextureWidth(void *hTexture)
{
    typedef int (__cdecl *pfn_GetSize)(void *);
    int lSizeBefore;

    /* 0x00504824 */
    lSizeBefore = (*(pfn_GetSize *)GLI_DRV_lGetSizeOfTexture)(hTexture);
    ((GLI_tdstTexture *)hTexture)->uwWidth >>= 1;
    return lSizeBefore - (*(pfn_GetSize *)GLI_DRV_lGetSizeOfTexture)(hTexture);
}

/* 0x0042d1f0 */
int __cdecl GLI_pc_fn_lHalveTextureHeight(void *hTexture)
{
    typedef int (__cdecl *pfn_GetSize)(void *);
    int lSizeBefore;

    /* 0x00504824 */
    lSizeBefore = (*(pfn_GetSize *)GLI_DRV_lGetSizeOfTexture)(hTexture);
    ((GLI_tdstTexture *)hTexture)->uwHeight >>= 1;
    return lSizeBefore - (*(pfn_GetSize *)GLI_DRV_lGetSizeOfTexture)(hTexture);
}

/* 0x0042d220 */
int __cdecl GLI_pc_fn_lHalveTextureMipBoth(void *hTexture)
{
    typedef int (__cdecl *pfn_GetSize)(void *);
    int lSizeBefore;

    if (((GLI_tdstTexture *)hTexture)->uwHeight > 4 &&
        ((GLI_tdstTexture *)hTexture)->uwWidth > 4)
    {
        lSizeBefore = (*(pfn_GetSize *)GLI_DRV_lGetSizeOfTexture)(hTexture);
        ((GLI_tdstTexture *)hTexture)->uwHeight >>= 1;
        ((GLI_tdstTexture *)hTexture)->uwWidth >>= 1;
        ((GLI_tdstTexture *)hTexture)->ulCompressionCounter += 1;
        return lSizeBefore - (*(pfn_GetSize *)GLI_DRV_lGetSizeOfTexture)(hTexture);
    }
    return 0;
}
