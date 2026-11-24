#include "GliDX6vf/GliDX6vf.h"

#define GLI_GMT_Z_CLIP_OFFSET 0x2d39c
#define GLI_GMT_INV_Z_CLIP_OFFSET 0x2d3a0

__declspec(dllexport) void __cdecl GLI_DRV_vSetZClip(float zClip, void *pGMT)
{
    unsigned char *base = (unsigned char *)pGMT;

    *(float *)(base + GLI_GMT_Z_CLIP_OFFSET) = zClip;
    *(float *)(base + GLI_GMT_INV_Z_CLIP_OFFSET) = 1.0f / zClip;
}
