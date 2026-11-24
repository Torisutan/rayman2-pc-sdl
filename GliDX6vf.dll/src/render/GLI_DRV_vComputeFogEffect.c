#include "GliDX6vf/GliDX6vf.h"
#include "GliDX6vf/GliDX6vf_State.h"

#define GLI_GMT_CURRENT_FOG_PARAMS_OFFSET 0x2d3ec
#define GLI_GMT_CACHED_FOG_PARAMS_OFFSET 0x2d3f0
#define GLI_GMT_FOG_EFFECT_ENABLE_OFFSET 0x2d3f4
#define GLI_FOG_VERTEX_RECORDS_OFFSET 0x2c
#define GLI_FOG_VERTEX_COUNT_OWNER_OFFSET 0x04
#define GLI_FOG_VERTEX_COUNT_OFFSET 0x2c
#define GLI_FOG_VERTEX_DEPTH_OFFSET 0x08
#define GLI_FOG_VERTEX_STRIDE 0x10
#define GLI_FOG_OUTPUT_OFFSET 0x1e854

/* 0x10008c50 */
__declspec(dllexport) void __cdecl GLI_DRV_vComputeFogEffect(void *pBG)
{
    unsigned char *vertex;
    unsigned char *end;
    DWORD *output;
    union { float f; DWORD ul; } uFog;

    if (*(int *)((unsigned char *)GLI_g_GliBigGlobals + GLI_GMT_FOG_EFFECT_ENABLE_OFFSET) == 0)
        return;

    if (*(void **)((unsigned char *)GLI_g_GliBigGlobals + GLI_GMT_CACHED_FOG_PARAMS_OFFSET) !=
        *(void **)((unsigned char *)GLI_g_GliBigGlobals + GLI_GMT_CURRENT_FOG_PARAMS_OFFSET)) {
        GLI_UpdateFogParameters(*(float **)((unsigned char *)GLI_g_GliBigGlobals + GLI_GMT_CURRENT_FOG_PARAMS_OFFSET));
        *(void **)((unsigned char *)GLI_g_GliBigGlobals + GLI_GMT_CACHED_FOG_PARAMS_OFFSET) =
            *(void **)((unsigned char *)GLI_g_GliBigGlobals + GLI_GMT_CURRENT_FOG_PARAMS_OFFSET);
    }

    vertex = (unsigned char *)pBG + GLI_FOG_VERTEX_RECORDS_OFFSET;
    end = vertex + *(short *)(*(unsigned char **)((unsigned char *)pBG + GLI_FOG_VERTEX_COUNT_OWNER_OFFSET) +
                              GLI_FOG_VERTEX_COUNT_OFFSET) * GLI_FOG_VERTEX_STRIDE;
    output = (DWORD *)((unsigned char *)pBG + GLI_FOG_OUTPUT_OFFSET);
    for (; vertex < end; vertex += GLI_FOG_VERTEX_STRIDE, output++) {
        if (*(float *)(vertex + GLI_FOG_VERTEX_DEPTH_OFFSET) >= GLI_g_FogEnd)
            *output = GLI_g_FogAlphaFar;
        else if (*(float *)(vertex + GLI_FOG_VERTEX_DEPTH_OFFSET) <= GLI_g_FogOffset)
            *output = 0xff000000u;
        else {
            uFog.f = 1.0f - (*(float *)(vertex + GLI_FOG_VERTEX_DEPTH_OFFSET) - GLI_g_FogOffset) * GLI_g_FogSlope
                     - -49152.0f;
            if (uFog.ul & 0x0000ff00u)
                *output = 0xff000000u;
            else
                *output = uFog.ul << 24;
        }
    }
}
