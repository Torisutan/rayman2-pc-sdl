#include "GliDX6vf/GliDX6vf.h"
#include "GliDX6vf/GliDX6vf_State.h"
#include "GliDX6vf/GLI_DX6Constants.h"

/* 0x10008a80 */
void __cdecl GLI_UpdateFogParameters(float *fogParams)
{
    float fNear;
    float fFar;
    unsigned long ulColor;

    fNear = fogParams[3] * (1.0f / 255.0f);
    fFar = fogParams[4] * (1.0f / 255.0f);
    GLI_g_FogStart = fogParams[0];
    GLI_g_FogEnd = fogParams[1];
    GLI_g_FogSlope = GLI_g_FogEnd - GLI_g_FogStart;
    if (GLI_g_FogSlope != 0.0f) {
        GLI_g_FogSlope = (fFar - fNear) / GLI_g_FogSlope;
        if (fNear == 0.0f)
            GLI_g_FogOffset = GLI_g_FogStart;
        else
            GLI_g_FogOffset = (fFar * GLI_g_FogStart - fNear * GLI_g_FogEnd) / (fFar - fNear);
    } else {
        GLI_g_FogSlope = 0.0f;
        GLI_g_FogOffset = 0.0f;
    }

    if (fogParams[5] < 0.0f)
        fogParams[5] = 0.0f;
    else if (fogParams[5] > 1.0f)
        fogParams[5] = 1.0f;
    if (fogParams[6] < 0.0f)
        fogParams[6] = 0.0f;
    else if (fogParams[6] > 1.0f)
        fogParams[6] = 1.0f;
    if (fogParams[7] < 0.0f)
        fogParams[7] = 0.0f;
    else if (fogParams[7] > 1.0f)
        fogParams[7] = 1.0f;

    ulColor = (((long)((fogParams[5]) * 255.0f) << 8) | (long)(fogParams[6] * 255.0f)) << 8 |
              (long)(fogParams[7] * 255.0f);
    GLI_g_FogColor = ulColor;
    GLI_g_FogAlphaNear = (long)((1.0f - fNear) * 255.0f) << 24;
    GLI_g_FogAlphaFar = (long)((1.0f - fFar) * 255.0f) << 24;

    GLI_DRV_M_SetRenderState(GLI_D3DRS_FOGCOLOR, ulColor);
}
