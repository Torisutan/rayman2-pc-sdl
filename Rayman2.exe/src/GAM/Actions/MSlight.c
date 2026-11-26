#include <string.h>
#include "GAM/GAM_types.h"
#include "GAM/GAM_globals.h"
#include "HIE/HIE_types.h"
#include "POS/POS_types.h"
#include "MTH/MTH3D.h"
#include "MTH/MTH_flt.h"
#include "GEO/GeoObj.h"
#include "GLI/light_fog.h"
#include "DNM/DNM.h"
#include "SAI/SAI.h"
#include "ERM/CpaError.h"
#include "GEO/Element.h"
#include "GLI/Material.h"

extern void __cdecl fn_vGameFreeInHLM(void *p_vObject);

#define GAM_h_MSLight(hSO)  (*(void **)((char *)HIE_fn_hGetSuperObjectObject(hSO) + 0x1c))

struct tdstMSLight_
{
    unsigned char    bLocalLight;
    unsigned char    bOnlyLocalLight;
    unsigned char    bGiroPhare;
    unsigned char    bPulsing;
    MTH3D_tdstVector stOffsetVect;
    MTH3D_tdstVector stAngle;
    MTH_tdxReal      xStepGiro;
    MTH_tdxReal      xStepPulse;
    MTH_tdxReal      xMaxRangePulse;
    MTH_tdxReal      xGiroAngle;
    struct GLI_tdstLight_ *p_stLight;
    void            *p_stVisuLight;
    void            *hLightInSector;

    unsigned char    ucNbRLIUse;
    unsigned char    ucFirstRLI;
    unsigned char    ucSecondRLI;
    MTH_tdxReal      ucBlendPercent;

};
typedef struct tdstMSLight_ *MS_tdxHandleToMSLight;

#define M_MSL(_h) ((MS_tdxHandleToMSLight)(_h))

/* 0x00407AA0 */
void *__cdecl fn_p_stMSLightGetLight(void *h_MSLight)
{
    return M_MSL(h_MSLight)->p_stLight;
}

/* 0x00416210 */
void __cdecl fn_vMSLightSetLocalLight(void *h, unsigned char bLocalLight)              { M_MSL(h)->bLocalLight = bLocalLight; }
unsigned char __cdecl fn_bMSLightGetLocalLight(void *h)                { return M_MSL(h)->bLocalLight; }      /* 0x00416220 */
/* 0x00416230 */
void          __cdecl fn_vMSLightSetOnlyLocalLight(void *h, unsigned char bOnlyLocalLight) { M_MSL(h)->bOnlyLocalLight = bOnlyLocalLight; }
unsigned char __cdecl fn_bMSLightGetOnlyLocalLight(void *h)            { return M_MSL(h)->bOnlyLocalLight; }   /* 0x00416240 */
unsigned char __cdecl fn_bMSLightGetGiroPhare(void *h)                 { return M_MSL(h)->bGiroPhare; }        /* 0x00416270 */
unsigned char __cdecl fn_bMSLightGetPulse(void *h)                     { return M_MSL(h)->bPulsing; }          /* 0x004162D0 */
MTH_tdxReal   __cdecl fn_xMSLightGetGiroStep(void *h)                  { return M_MSL(h)->xStepGiro; }         /* 0x00416280 */
MTH_tdxReal   __cdecl fn_xMSLightGetPulseStep(void *h)                 { return M_MSL(h)->xStepPulse; }        /* 0x004162E0 */
MTH_tdxReal   __cdecl fn_xMSLightGetPulseMaxRange(void *h)             { return M_MSL(h)->xMaxRangePulse; }    /* 0x00416300 */
MTH_tdxReal   __cdecl fn_xMSLightGetGiroAngle(void *h)                 { return M_MSL(h)->xGiroAngle; }        /* 0x004162A0 */
void          __cdecl fn_vMSLightSetGiroAngle(void *h, MTH_tdxReal v)  { M_MSL(h)->xGiroAngle = v; }           /* 0x00416290 */

/* 0x00416DB0 */
void *__cdecl HIE_fn_hGetSuperObjectLight(HIE_tdstSuperObject *_hSprObj)
{
    if (_hSprObj->ulTypeOfLinkedObject == HIE_C_ulActor)
    {
        void *hMSLight;

        hMSLight = ((struct tdstEngineObject_ *)HIE_fn_hGetSuperObjectObject(_hSprObj))->h_MSLight;

        return (hMSLight && fn_bMSLightGetLocalLight(hMSLight) ? fn_p_stMSLightGetLight(hMSLight) : NULL);
    }
    return NULL;
}

/* 0x00416D70 */
void __cdecl fn_vMSLightSetRLIParameter(void *hMSLight, long lNbRLI, long lIndex1, long lIndex2, long lPercent)
{
    if (hMSLight)
    {
        M_MSL(hMSLight)->ucNbRLIUse = (unsigned char)lNbRLI;
        M_MSL(hMSLight)->ucFirstRLI = (unsigned char)lIndex1;
        M_MSL(hMSLight)->ucSecondRLI = (unsigned char)lIndex2;
        M_MSL(hMSLight)->ucBlendPercent = (MTH_tdxReal)(unsigned char)lPercent;
    }
}
void          __cdecl fn_xMSLightSetPulseStep(void *h, MTH_tdxReal v)  { M_MSL(h)->xStepPulse = v; }           /* 0x004162F0 */
MTH3D_tdstVector *__cdecl fn_p_stMSLightGetOffset(void *h)             { return &M_MSL(h)->stOffsetVect; }     /* 0x00416330 */
/* 0x004161F0 */
void **__cdecl fn_pp_stMSLightGetLight(void *h_MSLight)
{
    return (void **)&M_MSL(h_MSLight)->p_stLight;
}
/* 0x00416200 */
void __cdecl fn_vMSLightSetLight(void *h_MSLight, void *p_stLight)
{
    M_MSL(h_MSLight)->p_stLight = p_stLight;
}
/* 0x00415FE0 */
void *__cdecl fn_hMSLightGetLightNodeInSector(void *_hMSLight)
{
    return M_MSL(_hMSLight)->hLightInSector;
}
/* 0x00415FF0 */
void __cdecl fn_vMSLightSetLightNodeInSector(void *h, void *hLightNode)
{
    M_MSL(h)->hLightInSector = hLightNode;
}
/* 0x00416340 */
MTH_tdxReal __cdecl fn_xMSLightGetOffsetX(void *h)
{
    return M_MSL(h)->stOffsetVect.xX;
}

MTH_tdxReal __cdecl fn_xMSLightGetOffsetY(void *h)
{
    return M_MSL(h)->stOffsetVect.xY;
}
MTH_tdxReal __cdecl fn_xMSLightGetOffsetZ(void *h)
{
    return M_MSL(h)->stOffsetVect.xZ;
}
/* 0x00416310 */
void __cdecl fn_vMSLightSetOffset(void *h, MTH_tdxReal xX, MTH_tdxReal xY, MTH_tdxReal xZ)
{
    M_MSL(h)->stOffsetVect.xX = xX;
    M_MSL(h)->stOffsetVect.xY = xY;
    M_MSL(h)->stOffsetVect.xZ = xZ;
}
void __cdecl fn_vMSLightGetDirection(void *h, MTH_tdxReal *px, MTH_tdxReal *py, MTH_tdxReal *pz) /* 0x00416350 */
{
    *px = M_MSL(h)->stAngle.xX;
    *py = M_MSL(h)->stAngle.xY;
    *pz = M_MSL(h)->stAngle.xZ;
}
void __cdecl fn_vMSLightSetGiroPhare(void *h, unsigned char bOnOff, MTH_tdxReal xStep) /* 0x00416250 */
{
    M_MSL(h)->bGiroPhare = bOnOff;
    M_MSL(h)->xStepGiro  = xStep;
}
void __cdecl fn_vMSLightSetPulse(void *h, unsigned char bOnOff, MTH_tdxReal xStep, MTH_tdxReal xMaxRange) /* 0x004162B0 */
{
    M_MSL(h)->bPulsing       = bOnOff;
    M_MSL(h)->xStepPulse     = xStep;
    M_MSL(h)->xMaxRangePulse = xMaxRange;
}

void __cdecl fn_vMSLightSetOnOff(void *h, unsigned char bOnOff)        { GLI_xSetLightState(fn_p_stMSLightGetLight(h), bOnOff); }   /* 0x00416040 */
unsigned char __cdecl fn_ucMSLightGetOnOff(void *h)                                                                                /* 0x00416020 */
{
    /* 0x416020 */
    GLI_xGetLightState(fn_p_stMSLightGetLight(h), (long *)(void *)&h);
    return (unsigned char)(unsigned long)h;
}
void __cdecl fn_vMSLightSetTypeOfLight(void *h, long lTypeOfLight)     { GLI_vSetLightType(fn_p_stMSLightGetLight(h), lTypeOfLight); }  /* 0x00416000 */
void __cdecl fn_vMSLightSetNearFar(void *h, MTH_tdxReal xNear, MTH_tdxReal xFar)                                                    /* 0x004160C0 */
{
    GLI_vSetLightNearFar(fn_p_stMSLightGetLight(h), xNear, xFar);
}

MTH_tdxReal __cdecl fn_xMSLightGetFar(void *h)                                                                                      /* 0x00416090 */
{
    MTH_tdxReal xFar,xNear;

    GLI_vGetLightNearFar(fn_p_stMSLightGetLight(h), &xNear, &xFar);
    return xFar;
}
void __cdecl fn_vMSLightSetColor(void *h, GEO_tdstColor *p_stColor)   { GLI_xSetLightColor(fn_p_stMSLightGetLight(h), p_stColor); } /* 0x00416190 */
void __cdecl fn_vMSLightSetColorRGBA(void *h, float xR, float xG, float xB, float xA)                                               /* 0x004161B0 */
{
    float aColor[4];
    aColor[0] = xR; aColor[1] = xG; aColor[2] = xB; aColor[3] = xA;
    fn_vMSLightSetColor(h, (GEO_tdstColor *)aColor);
}

void __cdecl fn_vMSLightSetLittleAlpha(void *h, MTH_tdxReal xAlpha)                                                                 /* 0x004160F0 */
{
    MTH_tdxReal xLittleIni, xBigIni;
    GLI_vGetLightAlphas(fn_p_stMSLightGetLight(h), &xLittleIni, &xBigIni);
    GLI_vSetLightAlphas(fn_p_stMSLightGetLight(h), xAlpha, xBigIni);
}
void __cdecl fn_vMSLightSetBigAlpha(void *h, MTH_tdxReal xAlpha)                                                                    /* 0x00416140 */
{
    MTH_tdxReal xLittleIni, xBigIni;
    GLI_vGetLightAlphas(fn_p_stMSLightGetLight(h), &xLittleIni, &xBigIni);
    GLI_vSetLightAlphas(fn_p_stMSLightGetLight(h), xLittleIni, xAlpha);
}

#define C_Light_SAIInitFlags ((SAI_tdeFlags)0x32)
#define E_uwGameMiniStructObjectNull_L ((unsigned short)0xE)
#define E_uwGameMiniStructNull_L       ((unsigned short)0xF)

/* 0x00416D10 */
unsigned char fn_ucGetNumberOfRLI (MS_tdxHandleToMSLight hMSLight,
                                   unsigned char *ucIndex1,
                                   unsigned char *ucIndex2,
                                   MTH_tdxReal *xPercent ) {
    if ( hMSLight ) {
        *ucIndex1 = hMSLight->ucFirstRLI ;
        *ucIndex2 = hMSLight->ucSecondRLI ;
        *xPercent = hMSLight->ucBlendPercent * 0.01f ;
        return (hMSLight->ucNbRLIUse) ;
    }
    return (0) ;
}

/* 0x00416D40 */
unsigned char fn_ucGetNumberOfRLIFromCharacter (HIE_tdstSuperObject *hSprObj, unsigned char *p_ucIndex1, unsigned char *p_ucIndex2, MTH_tdxReal *p_xPercent) {
    unsigned char ucNbRLIUse=0 ;

    if ( hSprObj->ulTypeOfLinkedObject == HIE_C_ulActor ) {
        ucNbRLIUse = fn_ucGetNumberOfRLI (M_GetMSHandle(hSprObj, MSLight), p_ucIndex1, p_ucIndex2, p_xPercent) ;
    }
    return (ucNbRLIUse) ;

}

#include "GLI/light_st.h"

extern void __cdecl GLI_vStoreLightForInit(void *p_stLight, SAI_tdeFlags eUserFlags);     /* 0x426DA0 */
extern void __cdecl GLI_vRemoveLightForInit(void *p_stLight, SAI_tdeFlags eUserFlags);    /* 0x426E70 */

/* 0x00415CA0 */
void __cdecl fn_vLightInitValueSAI(MS_tdxHandleToMSLight _h_MSLight)
{
    GLI_vStoreLightForInit(_h_MSLight->p_stLight, C_Light_SAIInitFlags);
    SAI_fn_bInsertMTHVectorInTable((void *)&_h_MSLight->stOffsetVect, C_Light_SAIInitFlags);
    SAI_fn_bInsertMTHVectorInTable((void *)&_h_MSLight->stAngle, C_Light_SAIInitFlags);
    SAI_fn_bInsertCharInTable((void *)&_h_MSLight->bPulsing, C_Light_SAIInitFlags);
    SAI_fn_bInsertFloatInTable((void *)&_h_MSLight->xStepPulse, C_Light_SAIInitFlags);
    SAI_fn_bInsertFloatInTable((void *)&_h_MSLight->xMaxRangePulse, C_Light_SAIInitFlags);
    SAI_fn_bInsertCharInTable((void *)&_h_MSLight->bLocalLight, C_Light_SAIInitFlags);
    SAI_fn_bInsertCharInTable((void *)&_h_MSLight->bOnlyLocalLight, C_Light_SAIInitFlags);
    SAI_fn_bInsertCharInTable((void *)&_h_MSLight->bGiroPhare, C_Light_SAIInitFlags);
    SAI_fn_bInsertFloatInTable((void *)&_h_MSLight->xStepGiro, C_Light_SAIInitFlags);
    SAI_fn_bInsertFloatInTable((void *)&_h_MSLight->xGiroAngle, C_Light_SAIInitFlags);
    SAI_fn_bInsertArrayInTable((void *)&_h_MSLight->p_stLight->xInter_Min_Pos, sizeof(MTH3D_tdstVector), C_Light_SAIInitFlags);
    SAI_fn_bInsertArrayInTable((void *)&_h_MSLight->p_stLight->xInter_Max_Pos, sizeof(MTH3D_tdstVector), C_Light_SAIInitFlags);
    SAI_fn_bInsertArrayInTable((void *)&_h_MSLight->p_stLight->xExter_Min_Pos, sizeof(MTH3D_tdstVector), C_Light_SAIInitFlags);
    SAI_fn_bInsertArrayInTable((void *)&_h_MSLight->p_stLight->xExter_Max_Pos, sizeof(MTH3D_tdstVector), C_Light_SAIInitFlags);
    SAI_fn_bInsertFloatInTable((void *)&_h_MSLight->p_stLight->xIntensity_Min, C_Light_SAIInitFlags);
    SAI_fn_bInsertFloatInTable((void *)&_h_MSLight->p_stLight->xIntensity_Max, C_Light_SAIInitFlags);
}

/* 0x00415DC0 */
void __cdecl fn_vLightFreeInitValueSAI(MS_tdxHandleToMSLight _h_MSLight)
{
    GLI_vRemoveLightForInit(_h_MSLight->p_stLight, C_Light_SAIInitFlags);
    SAI_fn_bRemoveValueFromTable((void *)&_h_MSLight->stOffsetVect, C_Light_SAIInitFlags);
    SAI_fn_bRemoveValueFromTable((void *)&_h_MSLight->stAngle, C_Light_SAIInitFlags);
    SAI_fn_bRemoveValueFromTable((void *)&_h_MSLight->bPulsing, C_Light_SAIInitFlags);
    SAI_fn_bRemoveValueFromTable((void *)&_h_MSLight->xStepPulse, C_Light_SAIInitFlags);
    SAI_fn_bRemoveValueFromTable((void *)&_h_MSLight->xMaxRangePulse, C_Light_SAIInitFlags);
    SAI_fn_bRemoveValueFromTable((void *)&_h_MSLight->bLocalLight, C_Light_SAIInitFlags);
    SAI_fn_bRemoveValueFromTable((void *)&_h_MSLight->bOnlyLocalLight, C_Light_SAIInitFlags);
    SAI_fn_bRemoveValueFromTable((void *)&_h_MSLight->bGiroPhare, C_Light_SAIInitFlags);
    SAI_fn_bRemoveValueFromTable((void *)&_h_MSLight->xStepGiro, C_Light_SAIInitFlags);
    SAI_fn_bRemoveValueFromTable((void *)&_h_MSLight->xGiroAngle, C_Light_SAIInitFlags);
    SAI_fn_bRemoveValueFromTable((void *)&_h_MSLight->p_stLight->xInter_Min_Pos, C_Light_SAIInitFlags);
    SAI_fn_bRemoveValueFromTable((void *)&_h_MSLight->p_stLight->xInter_Max_Pos, C_Light_SAIInitFlags);
    SAI_fn_bRemoveValueFromTable((void *)&_h_MSLight->p_stLight->xExter_Min_Pos, C_Light_SAIInitFlags);
    SAI_fn_bRemoveValueFromTable((void *)&_h_MSLight->p_stLight->xExter_Max_Pos, C_Light_SAIInitFlags);
    SAI_fn_bRemoveValueFromTable((void *)&_h_MSLight->p_stLight->xIntensity_Min, C_Light_SAIInitFlags);
    SAI_fn_bRemoveValueFromTable((void *)&_h_MSLight->p_stLight->xIntensity_Max, C_Light_SAIInitFlags);
}

void __cdecl fn_vLightSavedValueSAI(MS_tdxHandleToMSLight _h_MSLight)
{
}

/* 0x00415A80 */
void __cdecl fn_vLightFree(struct tdstEngineObject_ *p_stObject)
{
    if (p_stObject == NULL)
    {
        ERM_fn_vClearLastError(0);
        ERM_fn_vUpdateLastError(0x30, g_ucGameModuleId, 0, E_uwGameMiniStructObjectNull_L, -1, 0xff, 0xff, NULL);
    }
    if (p_stObject->h_MSLight == NULL)
    {
        ERM_fn_vClearLastError(0);
        ERM_fn_vUpdateLastError(0x30, g_ucGameModuleId, 0, E_uwGameMiniStructNull_L, -1, 0xff, 0xff, NULL);
    }
    fn_vLightFreeInitValueSAI(p_stObject->h_MSLight);
    fn_vGameFreeInHLM(p_stObject->h_MSLight);
    p_stObject->h_MSLight = NULL;
}

unsigned long __cdecl fn_ulLightSizeOf(void);

typedef struct MSL_tdstColorLocal_ { float xR; float xG; float xB; float xA; } MSL_tdstColorLocal;
extern void __cdecl GLI_xSetMaterialType(void *hMaterial, unsigned long xMType);   /* 0x42DB20 */

/* 0x00416940 */
GEO_tdstGeometricObject *__cdecl fn_pCreateSphere(float x, float y, float z, float xRadius)
{
    GEO_tdstGeometricObject *p_stSphere;
    MTH3D_tdstVector         stCenter;
    short                    hElement;
    void                    *hMaterial;
    MSL_tdstColorLocal       stColor;
    void                    *hGameMaterial;

    GEO_vCreateGeometricObject(&p_stSphere, 1, 1);
    MTH3D_M_vSetVectorElements(&stCenter, x, y, z);
    GEO_vSetPointOfObject(p_stSphere, &stCenter, 0);

    GEO_vCreateElementSpheres(p_stSphere, &hElement, 1);
    GEO_vSetCenterPointOfIndexedSphere(p_stSphere, hElement, 0, 0);
    GEO_vSetRadiusOfIndexedSphere(p_stSphere, hElement, 0, xRadius);

    hGameMaterial = GMT_fn_hCreateGameMaterial();
    GLI_xCreateMaterial(&hMaterial);
    GMT_fn_vSetVisualMaterial(hGameMaterial, hMaterial);

    stColor.xR = (float)0.100000;
    stColor.xG = (float)0.100000;
    stColor.xB = (float)0.100000;
    stColor.xA = (float)0;
    GLI_xSetMaterialAmbientCoef(hMaterial, &stColor);
    stColor.xR = (float)0.700000;
    stColor.xG = (float)0.700000;
    stColor.xB = (float)0.700000;
    stColor.xA = (float)0;
    GLI_xSetMaterialDiffuseCoef(hMaterial, &stColor);
    stColor.xR = (float)0.7;
    stColor.xG = (float)0.7;
    stColor.xB = (float)0.7;
    stColor.xA = (float)0;
    GLI_xSetMaterialSpecularCoef(hMaterial, 0, &stColor);
    stColor.xR = (float)0.8;
    stColor.xG = (float)0.8;
    stColor.xB = (float)0.8;
    stColor.xA = (float)-4e+8;
    GLI_xSetMaterialColor(hMaterial, &stColor);

    GLI_xSetMaterialType(hMaterial, 2042);
    GEO_vSetGameMaterialOfIndexedSphere(p_stSphere, hElement, 0, hGameMaterial);
    GEO_vSetMaterialOfIndexedSphere(p_stSphere, hElement, 0, hMaterial);
    GEO_vEndModifyObject(p_stSphere);
    return p_stSphere;
}

/* 0x00416B10 */
GEO_tdstGeometricObject *__cdecl fn_pCreateCone(float xS, float yS, float zS, float xRadius)
{
    GEO_tdstGeometricObject *p_stSphere;
    MTH3D_tdstVector         stCenter;
    short                    hElement;
    void                    *hMaterial;
    MSL_tdstColorLocal       stColor;
    void                    *hGameMaterial;

    GEO_vCreateGeometricObject(&p_stSphere, 2, 1);
    MTH3D_M_vSetVectorElements(&stCenter, xS, yS, zS);
    GEO_vSetPointOfObject(p_stSphere, &stCenter, 0);
    MTH3D_M_vSetVectorElements(&stCenter, xS - 0.1f, yS, zS);
    GEO_vSetPointOfObject(p_stSphere, &stCenter, 1);
    GEO_vCreateElementCones(p_stSphere, &hElement, 1);
    GEO_vSetPointsOfIndexedCone(p_stSphere, hElement, 0, 1, 0);
    GEO_vSetRadiusOfIndexedCone(p_stSphere, hElement, 0, xRadius);
    hGameMaterial = GMT_fn_hCreateGameMaterial();
    GLI_xCreateMaterial(&hMaterial);
    GMT_fn_vSetVisualMaterial(hGameMaterial, hMaterial);

    stColor.xR = (float)0;
    stColor.xG = (float)0;
    stColor.xB = (float)0;
    stColor.xA = (float)0;
    GLI_xSetMaterialAmbientCoef(hMaterial, &stColor);
    stColor.xR = (float)0;
    stColor.xG = (float)0;
    stColor.xB = (float)0;
    stColor.xA = (float)0;
    GLI_xSetMaterialDiffuseCoef(hMaterial, &stColor);
    stColor.xR = (float)0;
    stColor.xG = (float)0;
    stColor.xB = (float)0;
    stColor.xA = (float)0;
    GLI_xSetMaterialSpecularCoef(hMaterial, 0, &stColor);
    stColor.xR = (float)0;
    stColor.xG = (float)0;
    stColor.xB = (float)0;
    stColor.xA = (float)0;
    GLI_xSetMaterialColor(hMaterial, &stColor);

    GLI_xSetMaterialType(hMaterial, 2042);
    GEO_vSetGameMaterialOfIndexedCone(p_stSphere, hElement, 0, hGameMaterial);
    GEO_vSetMaterialOfIndexedCone(p_stSphere, hElement, 0, hMaterial);
    GEO_vEndModifyObject(p_stSphere);
    return p_stSphere;
}

extern void __cdecl GLI_xCreateLight(void **p_hLight, long lTypeOfLight);   /* 0x426C00 */
void __cdecl fn_vMSCreateLightWithType(void *h_MSLight, long lTypeOfLight)
{
    GLI_xCreateLight(fn_pp_stMSLightGetLight(h_MSLight), lTypeOfLight);
}

extern void *__cdecl SECT_fn_hCreateElementLstDNMLights(void);   /* 0x44BCC0 */
void *__cdecl fn_h_LightRealAlloc(void)
{
    void *h_MSLight;

    h_MSLight = fnp_vGameMallocInHLM(fn_ulLightSizeOf());
    fn_vMSLightSetGiroAngle(h_MSLight, 0.0f);
    fn_vMSLightSetOffset(h_MSLight, 0.0f, 0.0f, 0.0f);
    fn_vMSCreateLightWithType(h_MSLight, 1);
    M_MSL(h_MSLight)->hLightInSector = SECT_fn_hCreateElementLstDNMLights();
    return h_MSLight;
}

/* 0x004159B0 */
void __cdecl fn_vLightAlloc(struct tdstEngineObject_ *p_stObject)
{
    if (p_stObject == NULL)
    {
        ERM_fn_vClearLastError(0);
        ERM_fn_vUpdateLastError(0x30, g_ucGameModuleId, 0, E_uwGameMiniStructObjectNull_L, -1, 0xff, 0xff, NULL);
    }
    if (p_stObject->h_MSLight != NULL)
    {
        ERM_fn_vClearLastError(0);
        ERM_fn_vUpdateLastError(0x30, g_ucGameModuleId, 0, 0x0D, -1, 0xff, 0xff, NULL);
    }
    p_stObject->h_MSLight = fn_h_LightRealAlloc();
    fn_vLightInitValueSAI(p_stObject->h_MSLight);
}

/* 0x00415B10 */
void __cdecl fn_vLightInit(struct tdstEngineObject_ *p_stObject, int eObjectInit)
{
    POS_tdstCompletePosition *p_stMatrix, stMatrix2;
    HIE_tdstSuperObject *hSuperObjFather;

    if (p_stObject == NULL)
    {
        ERM_fn_vClearLastError(0);
        ERM_fn_vUpdateLastError(0x30, g_ucGameModuleId, 0, E_uwGameMiniStructObjectNull_L, -1, 0xff, 0xff, NULL);
    }
    if (p_stObject->h_MSLight == NULL)
    {
        ERM_fn_vClearLastError(0);
        ERM_fn_vUpdateLastError(0x30, g_ucGameModuleId, 0, E_uwGameMiniStructNull_L, -1, 0xff, 0xff, NULL);
    }

    switch (eObjectInit)
    {
    case 1:
        fn_vLightInitValueSAI(p_stObject->h_MSLight);
        fn_vLightSavedValueSAI(p_stObject->h_MSLight);
    case 0:
        if (GLI_lGetLightType(fn_p_stMSLightGetLight(p_stObject->h_MSLight)) != 3)
        {
            M_MSL(p_stObject->h_MSLight)->p_stVisuLight = fn_pCreateSphere(fn_xMSLightGetOffsetX(p_stObject->h_MSLight), fn_xMSLightGetOffsetY(p_stObject->h_MSLight), fn_xMSLightGetOffsetZ(p_stObject->h_MSLight), 0.2f);
        }
        else
        {
            M_MSL(p_stObject->h_MSLight)->p_stVisuLight = fn_pCreateCone(fn_xMSLightGetOffsetX(p_stObject->h_MSLight), fn_xMSLightGetOffsetY(p_stObject->h_MSLight), fn_xMSLightGetOffsetZ(p_stObject->h_MSLight), 1.0f);
        }

    case 2:
    case 3:
    case 4:
    case 5:
        hSuperObjFather = M_GetSuperObject(p_stObject);
        if (hSuperObjFather != NULL)
        {
            p_stMatrix = HIE_fn_hGetSuperObjectMatrix(hSuperObjFather);
            POS_fn_vInvertIsoMatrix(&stMatrix2, p_stMatrix);
            POS_fn_vNormalizeMatrix(&stMatrix2);
            GLI_xSetLightMatrix(fn_p_stMSLightGetLight(p_stObject->h_MSLight), &stMatrix2);
        }
        break;
    }
}

/* 0x004159A0 */
unsigned long __cdecl fn_ulLightSizeOf(void)
{
    return 0x40;
}

extern void __cdecl GLI_vCopyLight(void *p_stLightDst, void *p_stLightSrc);   /* 0x426D80 */

/* 0x00415EE0 */
void __cdecl fn_vLightDesinit(struct tdstEngineObject_ *p_stObject, int eObjectInit)
{
    if (p_stObject == NULL)
    {
        ERM_fn_vClearLastError(0);
        ERM_fn_vUpdateLastError(0x30, g_ucGameModuleId, 0, E_uwGameMiniStructObjectNull_L, -1, 0xff, 0xff, NULL);
    }
    if (p_stObject->h_MSLight == NULL)
    {
        ERM_fn_vClearLastError(0);
        ERM_fn_vUpdateLastError(0x30, g_ucGameModuleId, 0, E_uwGameMiniStructNull_L, -1, 0xff, 0xff, NULL);
    }
}

/* 0x00415F50 */
void __cdecl fn_vLightCopyClone(struct tdstEngineObject_ *_p_stObjectDest, struct tdstEngineObject_ *_p_stObjectSrc)
{
    void *p_stLight;
    void *hLightInSector;

    p_stLight      = fn_p_stMSLightGetLight(_p_stObjectDest->h_MSLight);
    hLightInSector = fn_hMSLightGetLightNodeInSector(_p_stObjectDest->h_MSLight);

    memcpy(_p_stObjectDest->h_MSLight, _p_stObjectSrc->h_MSLight, fn_ulLightSizeOf());

    fn_vMSLightSetLight(_p_stObjectDest->h_MSLight, p_stLight);
    GLI_vCopyLight(fn_p_stMSLightGetLight(_p_stObjectDest->h_MSLight),
                   fn_p_stMSLightGetLight(_p_stObjectSrc->h_MSLight));
    fn_vMSLightSetLightNodeInSector(_p_stObjectDest->h_MSLight, hLightInSector);
}

/* 0x00426FC0 */
struct GLD_tdstViewportAttributes;
extern void __cdecl GLI_xSendObjectToViewport(struct GLD_tdstViewportAttributes *p_stVpt, void *p_stObj,
                                              void *p_stLight, long lDrawModeMask,
                                              long lHieFlags); /* 0x0042A8C0 */

/* 0x4a7090 */
static unsigned char g_bShowLight_storage;
#define g_bShowLight             (g_bShowLight_storage)

#define GAM_g_p_stViewportAttr   ((void *)GAM_g_stEngineStructure.stViewportAttr)

#define GAM_p_stVisuLight(h)     (M_MSL(h)->p_stVisuLight)

#pragma optimize("y", off)
void __cdecl fn_vUpdateLight(HIE_tdstSuperObject *hSupObject)
{

    void *h_MSLight = GAM_h_MSLight(hSupObject);

    if ((h_MSLight)&&(fn_ucMSLightGetOnOff(h_MSLight)))
    {
      POS_tdstCompletePosition *p_stMatrix,stMatrix2,stMatrix3;
      POS_tdstCompletePosition stMatrixD;
      MTH3D_tdstVector stDirection;
      MTH3D_tdstVector stTransVect,stOffsetVect;
      static MTH_tdxReal x=0,y=0,z=0;   /* 0x4A7094 */
      MTH_tdxReal step=MTH_C_Inv5;
      MTH_tdxReal xAnglePerso;

	 xAnglePerso=fn_xComputeAngleOfPerso(hSupObject);
	 p_stMatrix=HIE_fn_hGetSuperObjectGlobalMatrix(hSupObject);
	 POS_fn_vCopyMatrix(&stMatrix2,p_stMatrix);

	 {
		 memcpy(&stOffsetVect,fn_p_stMSLightGetOffset(h_MSLight),sizeof(MTH3D_tdstVector));

		 POS_fn_vSetIdentityMatrix(&stMatrix3);
		 POS_fn_vSetTranslationVector(&stMatrix3,&stOffsetVect);
		 fn_vTurnMatrixZ(&stMatrix3,xAnglePerso);
		 POS_fn_vGetTranslationVector(&stMatrix3,&stOffsetVect);

		 POS_fn_vGetTranslationVector(p_stMatrix,&stTransVect);
		 MTH3D_M_vAddVector(&stTransVect,&stTransVect,&stOffsetVect);

		 fn_vMSLightGetDirection(h_MSLight,&x,&y,&z);
		 fn_vTurnMatrixZ(&stMatrix2,z);

		 fn_vTurnMatrixX(&stMatrix2,x);
		 if (y)
		 fn_vTurnMatrixY(&stMatrix2,y);

		 POS_fn_vSetTranslationVector( &stMatrix2,&stTransVect);

		 if (g_bShowLight)
		 {
			 MTH3D_M_vSetVectorElements(&stDirection,-1,0,0);
			 POS_fn_vSetIdentityMatrix(&stMatrixD);
			 POS_fn_vSetTranslationVector(&stMatrixD,&stDirection);
			 fn_vTurnMatrixX(&stMatrixD,x);
			 fn_vTurnMatrixY(&stMatrixD,y);
			 fn_vTurnMatrixZ(&stMatrixD,z);
		 }

	 }

	 if (fn_bMSLightGetGiroPhare(h_MSLight))
	 {
		y=fn_xMSLightGetGiroAngle(h_MSLight);
		fn_vTurnMatrixZ(&stMatrix2,y);

		if (g_bShowLight)
			fn_vTurnMatrixZ(&stMatrixD,y);

		POS_fn_vSetTranslationVector( &stMatrix2,&stTransVect);
		y+=fn_xMSLightGetGiroStep(h_MSLight);
		if (y>MTH_C_2Pi)
		{
			y=0;
		}
		fn_vMSLightSetGiroAngle(h_MSLight,y);
	 }

	 if (fn_bMSLightGetPulse(h_MSLight))
	 {
		 x=fn_xMSLightGetFar(h_MSLight);
		 step=fn_xMSLightGetPulseStep(h_MSLight);
		 x+=step;
		 if ((x>fn_xMSLightGetPulseMaxRange(h_MSLight))||(x<0))
		 {	step=-step;
			x+=2*step;
		 }
		 fn_xMSLightSetPulseStep(h_MSLight,step);
		 GLI_vSetLightNearFar(fn_p_stMSLightGetLight(h_MSLight),0,x);
	 }

	 GLI_xSetLightMatrix ( fn_p_stMSLightGetLight(h_MSLight) , &stMatrix2);

	if (g_bShowLight)
	{
		POS_fn_vGetTranslationVector(&stMatrixD,&stDirection);
		MTH3D_M_vAddVector(&stDirection,&stDirection,fn_p_stMSLightGetOffset(h_MSLight));
		GEO_vSetPointOfObject(GAM_p_stVisuLight(h_MSLight),&stDirection,1);
		GLI_xSendObjectToViewport(GAM_g_p_stViewportAttr,GAM_p_stVisuLight(h_MSLight),gp_stLight,0xffffffbf  , HIE_fn_SO_ulGetFlags(hSupObject));
	}
    }
}
#pragma optimize("", on)
