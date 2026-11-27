#include "GliDX6vf/GliDX6vf.h"
#include "GliDX6vf/GliDX6vf_State.h"

#define GLI_GEOOBJ_NB_POINTS_OFFSET 0x2c

/* 0x10008c50 */
__declspec(dllexport) void __cdecl GLI_DRV_vComputeFogEffect(GLI_tdstInternalGlobalValuesFor3dEngine *p_stGlobaleMT)
{
    GLI_tdstAligned3DVector *vertex;
    GLI_tdstAligned3DVector *end;
    DWORD *output;
    union { float f; DWORD ul; } uFog;

    if (GLI_g_GliBigGlobals->xFogIsOn == 0)
        return;

    if (GLI_g_GliBigGlobals->p_stLastComputedFog != GLI_g_GliBigGlobals->p_stActiveFog) {
        GLI_UpdateFogParameters((float *)GLI_g_GliBigGlobals->p_stActiveFog);
        GLI_g_GliBigGlobals->p_stLastComputedFog = GLI_g_GliBigGlobals->p_stActiveFog;
    }

    vertex = p_stGlobaleMT->GLI_TurnedScaledDisplecedPoint;
    end = vertex + *(short *)((unsigned char *)p_stGlobaleMT->p_stObj + GLI_GEOOBJ_NB_POINTS_OFFSET);
    output = (DWORD *)p_stGlobaleMT->GLI_aDEF_stColorsRLIS;
    for (; vertex < end; vertex++, output++) {
        if (vertex->xZ >= GLI_g_FogEnd)
            *output = GLI_g_FogAlphaFar;
        else if (vertex->xZ <= GLI_g_FogOffset)
            *output = 0xff000000u;
        else {
            uFog.f = 1.0f - (vertex->xZ - GLI_g_FogOffset) * GLI_g_FogSlope
                     - -49152.0f;
            if (uFog.ul & 0x0000ff00u)
                *output = 0xff000000u;
            else
                *output = uFog.ul << 24;
        }
    }
}
