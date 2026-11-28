#include <string.h>
#include "POS/POS_types.h"
#include "GLI/light_fog.h"

/* 0x005036ac */
extern struct GLI_tdstInternalGlobalValuesFor3dEngine_ *GLI_BIG_GLOBALS;

/* 0x004A7248 */
int GLI_g_lGlobalFogIsOn_storage;

#include "GLI/light_st.h"

/* 0x00487850 */
MTH_tdxReal	GLI_fn_xFogGetFar(struct GLI_tdstLight_ *p_stLight)
{
	return p_stLight->xFar;
}

long __cdecl GLI_lGetLightType(void *p_stLight)
{
    return ((struct GLI_tdstLight_ *)p_stLight)->lTypeOfLight;
}

#include "GLI/GLI_BigGlobals.h"

typedef struct GLI_pc_tdstFogColor_ { unsigned long a_ulRGBA[4]; } GLI_pc_tdstFogColor;

struct tdstFogParams_
{
    MTH_tdxReal xDepthStart;
    MTH_tdxReal xDepthEnd;
    MTH_tdxReal xInfinite;
    MTH_tdxReal xBlendStart;
    MTH_tdxReal xBlendEnd;
    GEO_tdstColor stColor;
};

struct tdstFogParams_ GLI_pc_a_ucFogRing[16];
struct tdstFogParams_ *GLI_pc_p_cFogRingCursor = GLI_pc_a_ucFogRing;
struct tdstFogParams_ *GLI_pc_p_cFogRingEnd    = GLI_pc_a_ucFogRing + 16;

void __cdecl GLI_vSetFog2(float xBlendStart, float xDepthStart, float xBlendEnd,
                          float xDepthEnd, float xInfinite, unsigned long *p_ulColor)
{
    struct tdstFogParams_ *p_stSlot;

#define p_stEntry GLI_pc_p_cFogRingCursor

    if (xDepthEnd < xDepthStart) {
        p_stEntry->xDepthStart = xDepthEnd;
        p_stEntry->xDepthEnd = xDepthStart;
    } else {
        p_stEntry->xDepthStart = xDepthStart;
        p_stEntry->xDepthEnd = xDepthEnd;
    }
    if (xInfinite < p_stEntry->xDepthEnd) {
        p_stEntry->xInfinite = xDepthEnd;
    } else {
        p_stEntry->xInfinite = xInfinite;
    }

    if (xBlendStart < 0.0f) {
        xBlendStart = 0.0f;
    } else if (xBlendStart > 255.0f) {
        xBlendStart = 255.0f;
    }
    if (xBlendEnd < 0.0f) {
        xBlendEnd = 0.0f;
    } else if (xBlendEnd > 255.0f) {
        xBlendEnd = 255.0f;
    }
    p_stEntry->xBlendStart = xBlendStart;
    p_stEntry->xBlendEnd = xBlendEnd;

    *(GLI_pc_tdstFogColor *)&p_stEntry->stColor = *(GLI_pc_tdstFogColor *)p_ulColor;

    for (p_stSlot = GLI_pc_a_ucFogRing; p_stSlot < p_stEntry; p_stSlot++) {
        if (memcmp(p_stEntry, p_stSlot, sizeof(struct tdstFogParams_)) == 0) {
            GLI_BIG_GLOBALS->p_stActiveFog = p_stSlot;
            return;
        }
    }

    GLI_BIG_GLOBALS->p_stActiveFog = p_stEntry;
    GLI_pc_p_cFogRingCursor = p_stEntry + 1;
    if (GLI_pc_p_cFogRingCursor == GLI_pc_p_cFogRingEnd) {
        GLI_pc_p_cFogRingCursor = GLI_pc_a_ucFogRing;
    }
#undef p_stEntry
}

/* 0x00424180 */
void __cdecl GLI_vFogOn(void)
{
    struct GLI_tdstInternalGlobalValuesFor3dEngine_ *p_stGlobals = GLI_BIG_GLOBALS;
    p_stGlobals->xFogIsOn = 1;
}

/* 0x00424190 */
void __cdecl GLI_vFogOff(void)
{
    /* 0x005036ac */
    GLI_BIG_GLOBALS->p_stLastComputedFog = 0;
    /* 0x005036ac */
    GLI_BIG_GLOBALS->p_stActiveFog = 0;
    /* 0x005036ac */
    GLI_BIG_GLOBALS->xFogIsOn = 0;
}
