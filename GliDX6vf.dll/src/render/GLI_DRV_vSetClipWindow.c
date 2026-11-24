#include "GliDX6vf/GliDX6vf.h"

#define GLI_GMT_CLIP_X0_OFFSET 0x2d3a4
#define GLI_GMT_CLIP_X1_OFFSET 0x2d3a8
#define GLI_GMT_CLIP_Y0_OFFSET 0x2d3ac
#define GLI_GMT_CLIP_Y1_OFFSET 0x2d3b0

__declspec(dllexport) void __cdecl GLI_DRV_vSetClipWindow(float x0, float x1, float y0, float y1, void *pGMT)
{
    unsigned char *base = (unsigned char *)pGMT;

    *(float *)(base + GLI_GMT_CLIP_X0_OFFSET) = x0;
    *(float *)(base + GLI_GMT_CLIP_X1_OFFSET) = x1;
    *(float *)(base + GLI_GMT_CLIP_Y0_OFFSET) = y0;
    *(float *)(base + GLI_GMT_CLIP_Y1_OFFSET) = y1;
}
