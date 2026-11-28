#include <string.h>
#include "POS/POS_types.h"
#include "GLI/light_fog.h"

/* 0x005036ac */
extern struct GLI_tdstInternalGlobalValuesFor3dEngine_ *GLI_BIG_GLOBALS;

/* 0x004A7248 */
int GLI_g_lGlobalFogIsOn_storage;

#include "GLI/light_st.h"

#include <math.h>
#include <string.h>
#include "POS/POS_types.h"
#include "GLI/light_fog.h"
#include "GLI/misc_trivial.h"
#include "HIE/HIE_globals.h"
#include "SAI/SAI.h"
#include "MMG/MMG_alloc.h"
#include "GEO/GEO_globals.h"
#include "ERM/CpaError.h"

void *GLI_gs_aDEF_p_CurrentListOfLight[C_lMaxLightInViewport];

unsigned char GLI_s_aucActiveLightForChar_00503C20[C_lMaxLightInViewport];
#define GLI_s_aActiveLightForChar GLI_s_aucActiveLightForChar_00503C20

#include "GLI/GlobalAlpha.h"
#include "GLI/GLI_globals.h"

#include "GLI/GLI_BigGlobals.h"
#define GLI_g_xInternalAlpha (GLI_BIG_GLOBALS->xGlobalAlpha)

#define GLI_K_xZero          (0.0f)
#define GLI_K_x255           (255.0f)

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

/* 0x0044AE80 */
void __cdecl GLI_vResetTableOfActiveLightsForChar(char _cValue)
{
    memset((void *)GLI_s_aActiveLightForChar, (int)_cValue, C_lMaxLightInViewport);
}

void __cdecl GLI_vSetLightActivityForChar(int iNum, unsigned char ucState)
{
    GLI_s_aActiveLightForChar[iNum] = ucState;
}

#define GLI_C_lParallelLight              1
#define GLI_C_lSphericalLight             2
#define GLI_C_lHotLight                   3
#define GLI_C_lAmbientLight               4
#define GLI_C_lParallelLight_With_Limit   5
#include "GLI/light_st.h"

unsigned char __cdecl GLI_bDoesLightAffectBV(struct GLI_tdstLight_ *p_stLight, MTH3D_tdstVector *p_stCenter, MTH_tdxReal xRadius)
{
    MTH3D_tdstVector stDist;
    MTH3D_tdstVector *p_stPos;
    MTH_tdxReal xDist;

    switch (p_stLight->lTypeOfLight)
    {
    case GLI_C_lSphericalLight:
    case GLI_C_lHotLight:
        p_stPos = &p_stLight->stMatrix.stTranslationVector;
        xRadius = MTH_M_xAdd(p_stLight->xFar, xRadius);
        MTH3D_M_vSubVector(&stDist, p_stCenter, p_stPos);
        xDist = MTH3D_M_xSqrVector(&stDist);
        xRadius = MTH_M_xSqr(xRadius);
        break;
    case GLI_C_lParallelLight_With_Limit:
        p_stPos = &p_stLight->stMatrix.stTranslationVector;
        xRadius = MTH_M_xAdd(p_stLight->xRadius, xRadius);
        MTH3D_M_vSubVector(&stDist, p_stCenter, p_stPos);
        xDist = MTH3D_M_xSqrVector(&stDist);
        xRadius = MTH_M_xSqr(xRadius);
        break;
    case GLI_C_lParallelLight:
    case GLI_C_lAmbientLight:
        return 1;
    default:
        return 1;
    }

    if (MTH_M_bGreater(xRadius, xDist))
        return 1;
    return 0;
}

void __cdecl GLI_vAddLightToViewport(struct GLD_tdstViewportAttributes_ *p_stVpt,
                                     struct GLI_tdstLight_ *p_stLight)
{
    long lNb = gs_CurrentNumberOfLightInViewport;
    if (lNb == C_lMaxLightInViewport)
    {
        return;
    }
    GLI_gs_aDEF_p_CurrentListOfLight[lNb] = p_stLight;
    gs_CurrentNumberOfLightInViewport = lNb + 1;
}

void __cdecl GLI_vPopLightsInViewport(struct GLD_tdstViewportAttributes_ *p_stVpt,
                                      unsigned long lNbLightsToPop)
{
    gs_CurrentNumberOfLightInViewport -= lNbLightsToPop;
    if (gs_CurrentNumberOfLightInViewport < 0)
    {
        gs_CurrentNumberOfLightInViewport = 0;
    }
}

void __cdecl GLI_vClearListOfLightInViewport(struct GLD_tdstViewportAttributes_ *p_stVpt)
{
    gs_CurrentNumberOfLightInViewport = 0;
}

void __cdecl GLI_xSetViewportLightsOnOff(unsigned char bOnOff)
{
    int i;
    for (i = 0; i < gs_CurrentNumberOfLightInViewport; i++)
    {
        ((struct GLI_tdstLight_ *)GLI_gs_aDEF_p_CurrentListOfLight[i])->bValid = bOnOff;
    }
}

void __cdecl GLI_xCreateLight(void **p_hLight, long lTypeOfLight)
{
    long *p_stLight;
    long  i;

    if (GEO_g_bDynamicAllocation != 2) {
        Mmg_fn_v_SetModeAlloc(g_ucGEOModuleId, g_ucGEOMMemMallocMode, 0);
        *p_hLight = fn_p_vGenAlloc(0x100, 0);
    }
    if (GEO_g_bDynamicAllocation == 2) {
fallback_path:
        ERM_fn_vClearLastError(0);
        if (GEO_g_bDynamicAllocation == 1) {
            ERM_fn_vUpdateLastError(4, g_ucGEOModuleId, 0, 8, -1, 0xff, 0xff, NULL);
            ERM_fn_vClearLastError(0);
            GEO_g_bDynamicAllocation = 2;
        }
        Mmg_fn_v_SetModeAlloc(g_ucGEOModuleId, 0xff, 0);
        *p_hLight = fn_p_vGenAlloc(0x100, 0);
    } else if (GEO_g_bDynamicAllocation == 1) {
        if (*p_hLight != NULL) goto fill_block;
        goto fallback_path;
    }
    if (*p_hLight == NULL) {
        ERM_fn_vClearLastError(0);
        ERM_fn_vUpdateLastError(4, g_ucGEOModuleId, 0, 1, -1, 0xff, 0xff, NULL);
    }

fill_block:

    p_stLight = (long *)*p_hLight;
    for (i = 0x40; i != 0; i--) {
        *p_stLight++ = 0;
    }

    if (*p_hLight != NULL) {
        *(long *)((char *)*p_hLight + 0x08) = lTypeOfLight;
        *(long *)*p_hLight = 1;

        *(unsigned long *)((char *)*p_hLight + 0x94) = 0x3f4ccccd;
        *(unsigned long *)((char *)*p_hLight + 0x98) = 0x3f4ccccd;
        *(unsigned long *)((char *)*p_hLight + 0x9C) = 0x3f4ccccd;
        *(unsigned long *)((char *)*p_hLight + 0xA0) = 0x3f4ccccd;
        *(long *)((char *)*p_hLight + 0x8C) = 0;
        *(unsigned char *)((char *)*p_hLight + 0xA4) = 1;
        *(unsigned char *)((char *)*p_hLight + 0xA6) = 0;
        GLI_vSetObjectLighted(*p_hLight, 3);
        *(long *)((char *)*p_hLight + 0xF0) = 0;
        *(long *)((char *)*p_hLight + 0xF4) = 0;
        *(long *)((char *)*p_hLight + 0xF8) = 0;
        *(long *)((char *)*p_hLight + 0xFC) = 0;
    }
}

void __cdecl GLI_vCopyLight(struct GLI_tdstLight_ *p_stLightDst, struct GLI_tdstLight_ *p_stLightSrc)
{
    typedef struct { unsigned long aul[64]; } GLI_tdstLightBlock;
    *(GLI_tdstLightBlock *)p_stLightDst = *(GLI_tdstLightBlock *)p_stLightSrc;
}

void __cdecl GLI_vStoreLightForInit(struct GLI_tdstLight_ *p_stLight, SAI_tdeFlags eUserFlags)
{
    SAI_fn_bInsertLongInTable((char *)&p_stLight->b_lOnOff, eUserFlags);
    SAI_fn_bInsertLongInTable((char *)&p_stLight->lTypeOfLight, eUserFlags);
    SAI_fn_bInsertFloatInTable((char *)&p_stLight->xFar, eUserFlags);
    SAI_fn_bInsertFloatInTable((char *)&p_stLight->xNear, eUserFlags);
    SAI_fn_bInsertFloatInTable((char *)&p_stLight->xLittleAlpha, eUserFlags);
    SAI_fn_bInsertFloatInTable((char *)&p_stLight->xBigAlpha, eUserFlags);
    SAI_fn_bInsertFloatInTable((char *)&p_stLight->xLittleTangent, eUserFlags);
    SAI_fn_bInsertFloatInTable((char *)&p_stLight->xBigTangent, eUserFlags);
    SAI_fn_bInsertFloatInTable((char *)&p_stLight->stColor.xR, eUserFlags);
    SAI_fn_bInsertFloatInTable((char *)&p_stLight->stColor.xG, eUserFlags);
    SAI_fn_bInsertFloatInTable((char *)&p_stLight->stColor.xB, eUserFlags);
    SAI_fn_bInsertFloatInTable((char *)&p_stLight->stColor.xA, eUserFlags);
    SAI_fn_bInsertCharInTable((char *)&p_stLight->ucObjectLighted, eUserFlags);
}

void __cdecl GLI_vRemoveLightForInit(struct GLI_tdstLight_ *p_stLight, SAI_tdeFlags eUserFlags)
{
    SAI_fn_bRemoveValueFromTable((char *)&p_stLight->b_lOnOff, eUserFlags);
    SAI_fn_bRemoveValueFromTable((char *)&p_stLight->lTypeOfLight, eUserFlags);
    SAI_fn_bRemoveValueFromTable((char *)&p_stLight->xFar, eUserFlags);
    SAI_fn_bRemoveValueFromTable((char *)&p_stLight->xNear, eUserFlags);
    SAI_fn_bRemoveValueFromTable((char *)&p_stLight->xLittleAlpha, eUserFlags);
    SAI_fn_bRemoveValueFromTable((char *)&p_stLight->xBigAlpha, eUserFlags);
    SAI_fn_bRemoveValueFromTable((char *)&p_stLight->xLittleTangent, eUserFlags);
    SAI_fn_bRemoveValueFromTable((char *)&p_stLight->xBigTangent, eUserFlags);
    SAI_fn_bRemoveValueFromTable((char *)&p_stLight->stColor.xR, eUserFlags);
    SAI_fn_bRemoveValueFromTable((char *)&p_stLight->stColor.xG, eUserFlags);
    SAI_fn_bRemoveValueFromTable((char *)&p_stLight->stColor.xB, eUserFlags);
    SAI_fn_bRemoveValueFromTable((char *)&p_stLight->stColor.xA, eUserFlags);
    SAI_fn_bRemoveValueFromTable((char *)&p_stLight->ucObjectLighted, eUserFlags);
}

void __cdecl GLI_xSetLightState(void *p_stLight, long b_lOnOff)   /* 0x00426F40 */
{
    *(long *)p_stLight = b_lOnOff;
}

void __cdecl GLI_xGetLightState(void *p_stLight, long *pb_lOnOff) /* 0x00426F50 */
{
    *pb_lOnOff = *(long *)p_stLight;
}

void __cdecl GLI_xSetLightColor(struct GLI_tdstLight_ *p_stLight, struct GEO_tdstColor_ *p_stColor) /* 0x00426F60 */
{
    p_stLight->stColor = *p_stColor;
}

void __cdecl GLI_vFogGetColor(void *p_stLight, void *p_stOutColor)
{

    *(GEO_tdstColor *)p_stOutColor = ((struct GLI_tdstLight_ *)p_stLight)->stColor;
}

void __cdecl GLI_xSetLightMatrix(void *p_stLight, POS_tdstCompletePosition *p_stMatrix) /* 0x00426FC0 */
{

    ((struct GLI_tdstLight_ *)p_stLight)->stMatrix = *p_stMatrix;
}

void __cdecl GLI_vSetLightNearFar(void *p_stLight, float xNear, float xFar) /* 0x00426FE0 */
{
    ((struct GLI_tdstLight_ *)p_stLight)->xNear = xNear;
    ((struct GLI_tdstLight_ *)p_stLight)->xFar = xFar;
}

void __cdecl GLI_vGetLightNearFar(void *p_stLight, float *p_xNear, float *p_xFar) /* 0x00427000 */
{
    *p_xNear = ((struct GLI_tdstLight_ *)p_stLight)->xNear;
    *p_xFar  = ((struct GLI_tdstLight_ *)p_stLight)->xFar;
}

void __cdecl GLI_vSetLightAlphas(void *p_stLight, float xLittleAlpha, float xBigAlpha) /* 0x00427020 */
{
    ((struct GLI_tdstLight_ *)p_stLight)->xLittleAlpha = xLittleAlpha;
    ((struct GLI_tdstLight_ *)p_stLight)->xBigAlpha = xBigAlpha;
    ((struct GLI_tdstLight_ *)p_stLight)->xLittleTangent = (float)tan(xLittleAlpha * 0.5);
    ((struct GLI_tdstLight_ *)p_stLight)->xBigTangent = (float)tan(xBigAlpha    * 0.5);
}

void __cdecl GLI_vGetLightAlphas(void *p_stLight, float *p_xLittleAlpha, float *p_xBigAlpha) /* 0x00427060 */
{
    *p_xLittleAlpha = ((struct GLI_tdstLight_ *)p_stLight)->xLittleAlpha;
    *p_xBigAlpha    = ((struct GLI_tdstLight_ *)p_stLight)->xBigAlpha;
}

void __cdecl GLI_vSetLightType(void *p_stLight, long lNewType) /* 0x00427080 */
{

    ((struct GLI_tdstLight_ *)p_stLight)->lTypeOfLight = lNewType;
}

void __cdecl GLI_xGetLightBackgroundColor(struct GLI_tdstLight_ *p_stLight, struct GEO_tdstColor_ *p_stColor)
{
    *p_stColor = p_stLight->stBackgroundColor;
}

unsigned char __cdecl GLI_ucGetObjectLighted(void *p_stLight)
{
    return ((struct GLI_tdstLight_ *)p_stLight)->ucObjectLighted;
}

void __cdecl GLI_vSetObjectLighted(void *p_stLight, unsigned char value)
{
    ((struct GLI_tdstLight_ *)p_stLight)->ucObjectLighted = value;
}

float __cdecl GLI_fn_xFogGetNear(struct GLI_tdstLight_ *p_stLight)       /* 0x004270E0 */
{
    return p_stLight->xNear;
}

float __cdecl GLI_fn_xFogGetInfinite(struct GLI_tdstLight_ *p_stLight)   /* 0x004270F0 */
{
    return p_stLight->xLittleAlpha;
}

float __cdecl GLI_fn_xFogGetBlendNear(struct GLI_tdstLight_ *p_stLight)  /* 0x00427100 */
{
    return p_stLight->xBigAlpha;
}

float __cdecl GLI_fn_xFogGetBlendFar(struct GLI_tdstLight_ *p_stLight)   /* 0x00427110 */
{
    return p_stLight->xIntensity_Min;
}

void __cdecl GLI_vSetGlobalAlpha(float xAlpha)
{
    GLI_g_xInternalAlpha = xAlpha * GLI_K_xOneOver255;
    if (GLI_g_xInternalAlpha > 1.0f) {
        GLI_g_xInternalAlpha = 1.0f;
    }
    if (GLI_g_xInternalAlpha < GLI_K_xZero) {
        GLI_g_xInternalAlpha = GLI_K_xZero;
    }
}

float __cdecl GLI_fn_xGetGlobalAlpha(void)
{
    return GLI_g_xInternalAlpha * GLI_K_x255;
}
