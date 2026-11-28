#include <stdlib.h>
#include <math.h>

#include "AGO/AGO.h"
#include "GAM/GAM_types.h"
#include "GAM/GAM_globals.h"
#include "AGO/AGO_globals.h"
#include "GLI/u_Draw.h"
#include "GLI/GlobalAlpha.h"
#include "GLI/DisplayFrame.h"
#ifdef RAYMAN2_WIDESCREEN
#include "GLI/GLI_ratio.h"
/* Safe area box keys of the HUD tables' elements */
#define AGO_M_lSafeAreaKeyGauge(i)  (i)
#define AGO_M_lSafeAreaKeyIcone(i)  (MAX_GAUGES + (i))
#define AGO_M_lSafeAreaKeyText(i)   (MAX_GAUGES + MAX_ICONES + (i))
static void AGO_fn_vComputeSafeAreaAnchors(void);
#endif /* RAYMAN2_WIDESCREEN */

static const float AGO_asm_K_c9dc = 0.0f;   /* 0x49c9dc */
static const float AGO_asm_K_ca2c = 1.0f;   /* 0x49ca2c */
#define AGO_ASM_K_C9DC  [AGO_asm_K_c9dc]
#define AGO_ASM_K_CA2C  [AGO_asm_K_ca2c]

/* 0x00429670 */
extern void __cdecl GLI_vDraw2DSpriteWithPercent(GLD_tdstViewportAttributes *, float, float, float, float, void *);

#define AGO_call_vDisplayLogo AGO_vDisplayLogo

/* 0x00465C10 */
void __cdecl JFFTXT_vAffiche(GLD_tdstViewportAttributes *p_stVpt);

AGO_Gauge AGO_g_TheGauges[MAX_GAUGES];   /* 0x004E0F20 */
AGO_Icone AGO_g_TheIcones[MAX_ICONES];   /* 0x004E54A0 */

#define MAX_JFFTXT 50
typedef struct AGO_JFFTXT_
{
    char         *pString;
    MTH3D_tdstVector vPos;
    unsigned char ucTransparency;
    unsigned char bFrozenDisplay;
    unsigned char bWaveEffect;
    unsigned char bOther;
    long          xEffectParameter;
    unsigned char ucForcedColor;
    char          _pad19[0x03];
} AGO_JFFTXT;
AGO_JFFTXT AGO_g_JFFTXT[MAX_JFFTXT];     /* 0x004E1080 */
/* 0x004A07D4 */
float AGO_g_xZValueForSprite = 1.0f;

#define AGO_g_ulCurrentTimerCount GAM_g_ulAccumulatedTime  /* 0x5003EC */

#define AGO_C_xPercentScale   0.01f   /* 0x0049CA18 */
#define AGO_C_xFullPercent    100.0f  /* 0x0049CA1C */
#define AGO_C_xLogoThreshold  0.01f   /* 0x0049CA24 */
#define AGO_C_xZIcones        0.998f
#define AGO_C_xZLogo          0.999f
#define AGO_C_xZOverlay       1.0f
#define AGO_C_fSecondsPerTick 0.001f  /* 0x0049C9E0 */

/* 0x0045FFF0 */
MTH_tdxReal AGO_fGetTimeInSeconds(void)
{
    return (MTH_tdxReal)((float)AGO_g_ulCurrentTimerCount * AGO_C_fSecondsPerTick);
}

/* 0x00460600 */
void AGO_vDisplayIcones(GLD_tdstViewportAttributes *p_stVpt)
{
    int i;

    for (i = 0; i < MAX_ICONES; i++)
    {
        if ((!GAM_g_bEngineFrozen) || ((i > 14) && (i < 25)))
            if (AGO_g_TheIcones[i].Transparency > 0)
            {
#ifdef RAYMAN2_WIDESCREEN
                /* HUD (engine not frozen): the icon's group may go to a screen edge */
                GLI_fn_vSetSafeAreaAnchorOfBox(AGO_M_lSafeAreaKeyIcone(i));
#endif /* RAYMAN2_WIDESCREEN */
                GLI_vSetGlobalAlpha((float)AGO_g_TheIcones[i].Transparency);
                GLI_vDraw2DSpriteWithPercent(p_stVpt,
                    AGO_g_TheIcones[i].Coords[0], AGO_g_TheIcones[i].Coords[1],
                    AGO_g_TheIcones[i].Coords[2], AGO_g_TheIcones[i].Coords[3],
                    AGO_g_TheIcones[i].Material);
#ifdef RAYMAN2_WIDESCREEN
                GLI_g_eSafeAreaAnchor = GLI_C_eSafeAreaCentre;
#endif /* RAYMAN2_WIDESCREEN */
            }
    }
}

AGO_Part AGO_g_AGO[AGO_MAX_NUMBER_OF_AGOS + 1];   /* 0x004E5800 */

extern float gs_fCoefOfMultiplicationForAnimatedTexture;
#define AGO_g_ulVBLTime         (gs_fCoefOfMultiplicationForAnimatedTexture)

/* 0x004656E0 */
void AGO_Define_OrthogonalVectors(MTH3D_tdstVector *p_stDirection, MTH3D_tdstVector *p_stV1, MTH3D_tdstVector *p_stV2)
{
	MTH3D_tdstVector stV1, stV2, stV3;
	MTH_tdxReal xNorm1, xNorm2, xNorm3;

	stV1.xX = (-(p_stDirection->xY)); stV1.xY = p_stDirection->xX; stV1.xZ = MTH_C_ZERO;
	stV2.xX = (-(p_stDirection->xZ)); stV2.xY = MTH_C_ZERO; stV2.xZ = p_stDirection->xX;
	stV3.xX = MTH_C_ZERO; stV3.xY = (-(p_stDirection->xZ)); stV3.xZ = p_stDirection->xY;

	xNorm1 = MTH3D_M_xSqrVector(&stV1);
	xNorm2 = MTH3D_M_xSqrVector(&stV2);
	xNorm3 = MTH3D_M_xSqrVector(&stV3);

	if (MTH_M_bGreater(xNorm3, xNorm1))
	{
		stV1.xX = stV3.xX;
		stV1.xY = stV3.xY;
		stV1.xZ = stV3.xZ;
		xNorm1 = xNorm3;
	}
	if (MTH_M_bGreater(xNorm2, xNorm1))
	{
		stV1.xX = stV2.xX;
		stV1.xY = stV2.xY;
		stV1.xZ = stV2.xZ;
		xNorm1 = xNorm2;
	}

	MTH3D_M_vNormalizeVector(&stV1, &stV1);

	p_stV2->xX = MTH_M_xMulSubMul(p_stDirection->xY, stV1.xZ, p_stDirection->xZ, stV1.xY);
	p_stV2->xY = MTH_M_xMulSubMul(p_stDirection->xZ, stV1.xX, p_stDirection->xX, stV1.xZ);
	p_stV2->xZ = MTH_M_xMulSubMul(p_stDirection->xX, stV1.xY, p_stDirection->xY, stV1.xX);

	p_stV1->xX = stV1.xX;
	p_stV1->xY = stV1.xY;
	p_stV1->xZ = stV1.xZ;
}

/* 0x00460020 */
static long AGO_ulGetElderOne(void)
{

    unsigned long i;
    unsigned long ulOldestIndex;
    MTH_tdxReal fOldestTime;
    AGO_Part *p;

    ulOldestIndex = 0;
    fOldestTime = AGO_g_AGO[0].fCreationTime;

    i = 0;
    p = AGO_g_AGO;
    while ((p->ulType != AGO_STILL | AGO_REMANANCE3) && (i < AGO_MAX_NUMBER_OF_AGOS))
    {
        i++;
        p++;
    }

    if (i >= AGO_MAX_NUMBER_OF_AGOS - 1)
        return -1;

    ulOldestIndex = i;
    fOldestTime = p->fCreationTime;
    i++;

    /* 0x460020 */
    for (; i < AGO_MAX_NUMBER_OF_AGOS ; i++, p++)
    {
        if (p->ulType & AGO_MUSTDIE)
            return i;
        if ((p->fCreationTime < fOldestTime) &&
            (p->ulType != AGO_STILL | AGO_REMANANCE3))
        {
            ulOldestIndex = i;
            fOldestTime = p->fCreationTime;
        }
    }
    return ulOldestIndex;
}

static const unsigned long
  AGO_C_xR7_bits=0x40E00000u, AGO_C_xRandN_bits=0x38000100u, AGO_C_x3_5_bits=0x40600000u,
  AGO_C_x3_bits=0x40400000u, AGO_C_x1_5_bits=0x3FC00000u, AGO_C_xN1_bits=0xBF800000u,
  AGO_C_x0_5_bits=0x3F000000u, AGO_C_x0_005_bits=0x3BA3D70Au, AGO_C_x0_1_bits=0x3DCCCCCDu,
  AGO_C_x0_05_bits=0x3D4CCCCDu, AGO_C_xN5_bits=0xC0A00000u, AGO_C_xInv9_bits=0x3DE38E39u,
  AGO_C_xN0_5_bits=0xBF000000u;
#define AGO_C_xR7     (*(const float *)&AGO_C_xR7_bits)
#define AGO_C_xRandN  (*(const float *)&AGO_C_xRandN_bits)
#define AGO_C_x3_5    (*(const float *)&AGO_C_x3_5_bits)
#define AGO_C_x3      (*(const float *)&AGO_C_x3_bits)
#define AGO_C_x1_5    (*(const float *)&AGO_C_x1_5_bits)
#define AGO_C_xN1     (*(const float *)&AGO_C_xN1_bits)
#define AGO_C_x0_5    (*(const float *)&AGO_C_x0_5_bits)
#define AGO_C_x0_005  (*(const float *)&AGO_C_x0_005_bits)
#define AGO_C_x0_1    (*(const float *)&AGO_C_x0_1_bits)
#define AGO_C_x0_05   (*(const float *)&AGO_C_x0_05_bits)
#define AGO_C_xN5     (*(const float *)&AGO_C_xN5_bits)
#define AGO_C_xInv9   (*(const float *)&AGO_C_xInv9_bits)
#define AGO_C_xN0_5   (*(const float *)&AGO_C_xN0_5_bits)

AGO_Part *AGO_vCreatePart(unsigned long parm_ulType, MTH3D_tdstVector *pPosition,
                          MTH3D_tdstVector *pDirection, MTH_tdxReal TimeLimit,
                          MTH_tdxReal fScaleX, MTH_tdxReal fScaleY,
                          void *pVisualMaterial)
{
    unsigned long ulType9;
    AGO_Part     *p;

    if ((parm_ulType & AGO_ZELDA) && (AGO_g_ulNumberOfAgos != AGO_MAX_NUMBER_OF_AGOS))
    {
        if (AGO_g_NumberOfZeldaAgos == MAX_NUMBER_OF_ZELDA_AGOS)
            return NULL;
        AGO_g_NumberOfZeldaAgos++;
    }

    if (AGO_g_ulNumberOfAgos == AGO_MAX_NUMBER_OF_AGOS)
    {
        long i = AGO_ulGetElderOne();
        if (i == -1)
            return NULL;
        p = &AGO_g_AGO[i];
    }
    else
    {

        p = &AGO_g_AGO[AGO_g_ulNumberOfAgos++];
    }

    p->ulType          = parm_ulType;
    p->vPosition       = *pPosition;
    p->vInitialPosition = *pPosition;
    if (pDirection != NULL)
        p->vDirection  = *pDirection;
    AGO_g_ffFrameTime  = AGO_fGetTimeInSeconds();
    p->fCreationTime   = AGO_g_ffFrameTime;
    p->fScale[0]       = fScaleX;
    p->fScale[1]       = fScaleY;
    p->fTimeLimit      = TimeLimit;
    p->VisualMaterial  = pVisualMaterial;
    p->AnimTime        = (long)AGO_g_ulVBLTime;
    p->fAnimTimeScale  = AGO_g_AGO_AnimTimeScale;
    p->Transparency    = 255.0f;
    if ((parm_ulType == AGO_RAYMAN_PLOUF) && (pDirection != NULL))
        p->Transparency = pDirection->xX;
    if ((parm_ulType & AGO_SHADEMASK) == AGO_APPEAR)
        p->Transparency = 5.0f;

    ulType9 = parm_ulType & AGO_TYPEMASK;

    if (ulType9 == AGO_ROTATING_SLOW)
    {
        p->vInitialPosition.xX = (float)rand() * AGO_C_xR7 * AGO_C_xRandN - AGO_C_x3_5;
        p->vInitialPosition.xY = (float)rand() * AGO_C_xR7 * AGO_C_xRandN - AGO_C_x3_5;
        p->vInitialPosition.xZ = (float)rand() * AGO_C_xR7 * AGO_C_xRandN - AGO_C_x3_5;
    }
    if ((ulType9 == AGO_MOVINGSINXY) || (ulType9 == AGO_MOVINGSINXYUP))
    {
        float fZ;
        if ((parm_ulType & AGO_ZELDA) == 0)
        {
            p->vInitialPosition.xX = (float)rand() * AGO_C_xRandN - AGO_C_x0_5;
            fZ = (float)rand();
        }
        else
        {
            p->vInitialPosition.xX = (float)rand() * AGO_C_x3 * AGO_C_xRandN - AGO_C_x1_5;
            fZ = (float)rand() * AGO_C_x3;
        }
        p->vInitialPosition.xZ = fZ * AGO_C_xRandN - AGO_C_xN1;
    }
    if (ulType9 == AGO_SPIRALE)
    {
        p->vInitialPosition.xX = (float)rand() * AGO_C_x0_005 * AGO_C_xRandN;
        p->vInitialPosition.xY = (float)rand() * AGO_C_x0_1 * AGO_C_xRandN - AGO_C_x0_05;
        p->Transparency        = 200.0f;
        p->vInitialPosition.xZ = (float)rand() * AGO_C_xRandN - AGO_C_xN5;
    }
    if (ulType9 == AGO_GOTO)
    {

        p->fTimeLimit = 0.0f;
        p->vInitialPosition.xX = (p->vDirection.xX - p->vPosition.xX) * AGO_C_xInv9;
        p->vInitialPosition.xY = (p->vDirection.xY - p->vPosition.xY) * AGO_C_xInv9;
        p->vInitialPosition.xZ = (p->vDirection.xZ - p->vPosition.xZ) * AGO_C_xInv9;
    }
    if (ulType9 == AGO_2DSPIRALE)
    {
        MTH3D_tdstVector V1, V2;
        float fAngle, fSin, fCos, fSin1, fCos1;

        p->vInitialPosition = *pPosition;
        AGO_Define_OrthogonalVectors(pDirection, &V1, &V2);
        fAngle = (float)rand() * AGO_C_xR7 * AGO_C_xRandN;
        p->fAnimTimeScale = fAngle;

        *(float *)&p->AnimTime = (float)rand() * AGO_C_x0_5 * AGO_C_xRandN - AGO_C_xN0_5;
        fSin  = (float)sin((double)fAngle);
        fCos  = (float)cos((double)fAngle);
        fSin1 = fSin * TimeLimit;
        fCos1 = fCos * TimeLimit;

        p->vPosition.xX = fCos1 * V2.xX + fSin1 * V1.xX + pPosition->xX;
        p->vPosition.xY = fCos1 * V2.xY + fSin1 * V1.xY + pPosition->xY;
        p->vPosition.xZ = fCos1 * V2.xZ + fSin1 * V1.xZ + pPosition->xZ;
    }

    return p;
}

/* 0x00460490 */
void AGO_pc_vRemoveMustDieParts(void)
{
    AGO_Part *pDst;
    AGO_Part *pSrc;
    int       iRemoved;
    unsigned int i;

    i = (unsigned int)AGO_g_ulNumberOfAgos;
    iRemoved = 0;
    pDst = AGO_g_AGO;
    pSrc = pDst;

    for (; i > 0u; i--)
    {

        if ((pSrc->ulType & AGO_MUSTDIE) != 0)
        {
            if (pSrc->ulType & AGO_ZELDA)
                AGO_g_NumberOfZeldaAgos--;
            iRemoved++;
        }
        else
        {
            if (pSrc != pDst)
                *pDst = *pSrc;
            pDst++;
        }
        pSrc++;
    }
    AGO_g_ulNumberOfAgos -= iRemoved;
}

/* 0x004610F0 */
void AGO_vVectorMulAddInternal(MTH_tdxReal *pfdest, MTH_tdxReal *pfsrc, MTH_tdxReal mul)
{
    int i;
    for (i = 0; i < 3; i++)
        pfdest[i] = pfdest[i] + pfsrc[i] * mul;
}

#define AGO_g_bInGameMenu          (*(char *)&GAM_g_bInGameMenu)  /* 0x004B9BC8 */
/* 0x004A4074 */
static int AGO_g_MustReinitStaticVars_storage = 1;
#define AGO_g_MustReinitStaticVars (AGO_g_MustReinitStaticVars_storage)
static const unsigned long AGO_C_x6_bits = 0x40C00000u;
#define AGO_C_x6                   (*(const float *)&AGO_C_x6_bits)

/* 0x0042A360 */
extern float __cdecl GLI_fn_xGetGlobalAlpha(void);
#define GLI_vGetGlobalAlpha   GLI_fn_xGetGlobalAlpha

#define AGO_call_vAddParticle AGO_vAddParticle

#define AGO_call_vProcessPart AGO_vProcessPart

#define AGO_call_vDisplayTriangles AGO_vDisplayTriangles

void AGO_vProcessAllParts(GLD_tdstViewportAttributes *p_stVpt)
{
    AGO_Part     *p;
    unsigned long i;
    float         fAlphaTemp;

    AGO_g_ffFrameTime = AGO_fGetTimeInSeconds();
    if (AGO_g_ffLastFrameTime != 0.0f)
        AGO_g_fTimeDifference = AGO_g_ffFrameTime - AGO_g_ffLastFrameTime;
    else
        AGO_g_fTimeDifference = 0.0f;
    AGO_g_ffLastFrameTime = AGO_g_ffFrameTime;

    if (AGO_g_bInGameMenu && !AGO_g_LastFrozen && !AGO_g_bUsingMapMondeAGOs)
    {
        AGO_g_LastFrozen = 1;
        AGO_g_ulNumberOfAgos = 0;
        AGO_g_NumberOfZeldaAgos = 0;
    }
    else if (!AGO_g_bInGameMenu && AGO_g_LastFrozen && !AGO_g_bUsingMapMondeAGOs)
    {
        AGO_g_LastFrozen = 0;
        AGO_g_ulNumberOfAgos = 0;
        AGO_g_NumberOfZeldaAgos = 0;
    }

    if (AGO_g_bForceWhiteFog)
    {
        AGO_g_bForceWhiteFog++;
        if (AGO_g_bForceWhiteFog == 3)
            AGO_g_bForceWhiteFog = 0;
    }

    if (AGO_g_ulNumberOfAgos == 0)
    {
        AGO_call_vDisplayTriangles(p_stVpt);
        return;
    }

    AGO_g_OddFrame = (AGO_g_OddFrame == 0) ? 1 : 0;

    fAlphaTemp = GLI_vGetGlobalAlpha();

    for (i = 0; i < AGO_g_ulNumberOfAgos; i++)
    {
        p = &AGO_g_AGO[i];
        {
            int bDie = 0;
            if ((p->ulType & AGO_TIMELIMITED) &&
                (p->fTimeLimit < AGO_g_ffFrameTime - p->fCreationTime))
                bDie = 1;
            if ((p->ulType & AGO_ZLIMITED) &&
                (p->fTimeLimit < p->vPosition.xZ))
                bDie = 1;
            if (p->Transparency <= 0.0f)
                bDie = 1;
            if (bDie || (p->ulType & AGO_MUSTDIE))
            {
                if ((p->ulType & AGO_ENDASPLOUF) && (p->ulType & AGO_ZELDA) &&
                    (0.0f < (float)rand() * AGO_C_xR7 * AGO_C_xRandN - AGO_C_x6))
                    AGO_call_vAddParticle(0x44, &p->vPosition, &p->vDirection,
                                     p->VisualMaterial, 1.0f);
                p->ulType |= AGO_MUSTDIE;
            }
        }
    }
    AGO_pc_vRemoveMustDieParts();

    for (i = 0; i < AGO_g_ulNumberOfAgos; i++)
        AGO_call_vProcessPart(i, p_stVpt);

    AGO_call_vDisplayTriangles(p_stVpt);
    GLI_vSetGlobalAlpha(fAlphaTemp);
    AGO_g_MustReinitStaticVars = 0;
}

/* 0x00461120 */
static const float AGO_k_f0 = 0.0f;
static const float AGO_k_f0_001 = 0.00100000005f;
static const float AGO_k_f7 = 7.0f;
static const float AGO_k_f3_05185em05 = 3.05185094e-05f;
static const float AGO_k_f3_5 = 3.5f;
static const float AGO_k_f3 = 3.0f;
static const float AGO_k_f1_5 = 1.5f;
static const float AGO_k_fMinus1 = -1.0f;
static const float AGO_k_f0_5 = 0.5f;
static const float AGO_k_f0_1 = 0.100000001f;
static const float AGO_k_f0_05_a = 0.0500000007f;
static const float AGO_k_fMinus0_5 = -0.5f;
static const float AGO_k_f100 = 100.0f;
static const float AGO_k_f0_01 = 0.00999999978f;
static const float AGO_k_f1 = 1.0f;
static const float AGO_k_f6 = 6.0f;
static const float AGO_k_f255 = 255.0f;
static const float AGO_k_f39_24 = 39.2400017f;
static const float AGO_k_f6_5 = 6.5f;
static const float AGO_k_f8 = 8.0f;
static const float AGO_k_fMinus1_6 = -1.60000002f;
static const float AGO_k_f0_25 = 0.25f;
static const float AGO_k_f1_51 = 1.50999999f;
static const float AGO_k_f1_1 = 1.10000002f;
static const float AGO_k_f1_01 = 1.00999999f;
static const float AGO_k_f0_9 = 0.899999976f;
static const float AGO_k_f0_98 = 0.980000019f;
static const float AGO_k_fMinus6_5 = -6.5f;
static const float AGO_k_fMinus0_2 = -0.200000003f;
static const float AGO_k_f0_707 = 0.707000017f;
static const float AGO_k_f4_5 = 4.5f;
static const float AGO_k_f1_3 = 1.29999995f;
static const float AGO_k_f3_7 = 3.70000005f;
static const float AGO_k_f9_5 = 9.5f;
static const float AGO_k_fMinus3_5 = -3.5f;
static const float AGO_k_fMinus5_3 = -5.30000019f;
static const float AGO_k_fMinus20_5 = -20.5f;
static const float AGO_k_fMinus60 = -60.0f;
static const float AGO_k_f16_5 = 16.5f;
static const float AGO_k_f5 = 5.0f;
static const float AGO_k_fMinus61_5 = -61.5f;
static const float AGO_k_f10 = 10.0f;
static const float AGO_k_f60 = 60.0f;
static const float AGO_k_f0_4_a = 0.400000036f;
static const float AGO_k_f160 = 160.0f;
static const float AGO_k_f260 = 260.0f;
static const float AGO_k_f350 = 350.0f;
static const float AGO_k_f850 = 850.0f;
static const float AGO_k_f2500 = 2500.0f;
static const float AGO_k_fMinus350 = -350.0f;
static const float AGO_k_fMinus100 = -100.0f;
static const float AGO_k_f155 = 155.0f;
static const float AGO_k_fMinus0_6 = -0.600000024f;
static const float AGO_k_fMinus0_7 = -0.699999988f;
static const float AGO_k_fMinus0_8 = -0.800000012f;
static const float AGO_k_f0_56 = 0.560000002f;
static const float AGO_k_fMinus0_53 = -0.529999971f;
static const float AGO_k_fMinus1_5 = -1.5f;
static const float AGO_k_fMinus0_98 = -0.980000019f;
static const float AGO_k_f0_23 = 0.230000004f;
static const float AGO_k_f0_16 = 0.159999996f;
static const float AGO_k_f0_13 = 0.129999995f;
static const float AGO_k_fMinus0_29 = -0.289999992f;
static const float AGO_k_fMinus0_63 = -0.629999995f;
static const float AGO_k_f0_4_b = 0.400000006f;
static const float AGO_k_f0_0004 = 0.00039999999f;
static const float AGO_k_fMinus0_4 = -0.400000006f;
static const float AGO_k_f0_6 = 0.600000024f;
static const float AGO_k_fMinus0_05 = -0.0500000007f;
static const float AGO_k_f0_8 = 0.800000012f;
static const float AGO_k_f4 = 4.0f;
static const float AGO_k_fMinus4 = -4.0f;
static const float AGO_k_fMinus0_3 = -0.300000012f;
static const float AGO_k_f0_2 = 0.200000003f;
static const float AGO_k_f0_05_b = 0.0499999821f;
static const float AGO_k_fMinus2 = -2.0f;
static const float AGO_k_f0_02 = 0.0200000014f;
static const float AGO_k_fMinus0_03 = -0.0299999993f;
static const float AGO_k_f2 = 2.0f;
static const float AGO_k_f0_09 = 0.0900000036f;
static const float AGO_k_fMinus0_01 = -0.00999999978f;
static const float AGO_k_f12_56 = 12.5600004f;
static const float AGO_k_f6_28 = 6.28000021f;
static const float AGO_k_f2_8 = 2.79999995f;
static const float AGO_k_f1_7 = 1.70000005f;
static const float AGO_k_f0_7 = 0.699999988f;
static const float AGO_k_f0_3_a = 0.300000012f;
static const float AGO_k_fMinus6_12346 = -6.123456f;
static const float AGO_k_f0_05_c = 0.0500000045f;
static const float AGO_k_fMinus0_1 = -0.100000001f;
static const float AGO_k_fMinus8 = -8.0f;
static const float AGO_k_fMinus2_5 = -2.5f;
static const float AGO_k_f0_15 = 0.150000006f;
static const float AGO_k_fMinus0_13 = -0.129999995f;
static const float AGO_k_f300 = 300.0f;
static const float AGO_k_f6_2 = 6.19999981f;
static const float AGO_k_fMinus0_19 = -0.189999998f;
static const float AGO_k_f0_08 = 0.0799999982f;
static const float AGO_k_fMinus0_02 = -0.0199999996f;
static const float AGO_k_f3_2 = 3.20000005f;
static const float AGO_k_f0_3_b = 0.299999982f;
static const float AGO_k_fMinus0_35 = -0.349999994f;
static const float AGO_k_f9 = 9.0f;
static const float AGO_k_f4_2 = 4.19999981f;
static const float AGO_k_f0_07 = 0.0699999928f;
static const float AGO_k_f0_57735 = 0.577350259f;
static const float AGO_k_fMinus6_9 = -6.9000001f;
static const float AGO_k_fMinus3_1564 = -3.15639997f;
static const float AGO_k_f16 = 16.0f;
/* 0x00463390 */
static const float AGO_k_f0_07_b = 0.07f;
static const float AGO_k_fMinus0_12 = -0.12f;   /* 0x49CB90 */
static const float AGO_k_f1_0999999 = 1.0999999f;
static const float AGO_k_f1_25 = 1.25f;

#define AGO_call_vDisplayPart AGO_vDisplayPart

#define AGO_FALLING               0x00000001
#define AGO_MOVING                0x00000003
#define AGO_GEISER                0x00000004
#define AGO_FALLING_SLOW          0x00000005
#define AGO_MOVING_AND_SLOWING    0x00000009
#define AGO_GOINGTO               0x0000000A
#define AGO_FASEGOINGTO           0x0000000E
#define AGO_FALLING_MEGAFAST      0x00000010
#define AGO_GOTO_STEP             MTH_M_xLongToReal(9)
#define AGO_SCALEMASK             0x00000F00
#define AGO_SCALING_SLOW          0x00000100
#define AGO_SCALING_MID           0x00000200
#define AGO_SCALING_FAST          0x00000300
#define AGO_SCALING_FASTEST       0x00000400
#define AGO_UNSCALING_FAST        0x00000500
#define AGO_UNSCALING_SLOW        0x00000600
#define AGO_UNSCALING_FASTEST     0x00000700
#define AGO_SCALING_MEGASLOW      0x00000800
#define AGO_SCALING_MEGAFASTY     0x00000900
#define AGO_SCALING_XTOY          0x00000A00
#define AGO_SCALING_YTOX          0x00000B00
#define AGO_SCALING_YTOX2         0x00000D00
#define AGO_BIG_RANDOM_SCALE      0x00000E00
#define AGO_SMALL_RANDOM_SCALE    0x00000F00
#define AGO_SHADE_SLOW            0x00001000
#define AGO_SHADE_MID             0x00002000
#define AGO_SHADE_FAST            0x00003000
#define AGO_SHADE_FASTEST         0x00004000
#define AGO_SHADE_MEGAFAST        0x00005000
#define AGO_SHADE_TWOFRAMES       0x00006000
#define AGO_WAIT                  0x00008000
#define AGO_RANDOM_SHADE          0x00009000
#define AGO_UNSHADE               0x0000A000
#define AGO_XYPLAN                0x10000000
#define AGO_RAYMAN_SMALL_PLOUF    (AGO_STILL | AGO_XYPLAN | AGO_SCALING_SLOW |  AGO_SHADE_FAST)
#define AGO_G                     MTH_M_xConstFloatToReal(9.81f)
#define AGO_G_SLOW                MTH_M_xConstFloatToReal(3.50f)

#define EXPLOSION_SPEED           MTH_M_xLongToReal(3)

#define MTH_M_xMul_32(a, b)       MTH_M_xMul((a), (b))
#define MTH_M_xMul3_32(a, b, c)   MTH_M_xMul_32(MTH_M_xMul_32((a), (b)), (c))

#define SINUS(f)                  ((MTH_tdxReal)sin(f))
#define COSINUS(f)                ((MTH_tdxReal)cos(f))

#undef FLOAT_RANGED_RANDOM
#define FLOAT_RANGED_RANDOM(mn, mx) \
    ((float)rand() * ((mx) - (mn)) * AGO_C_xRandN - (-(mn)))

#define AGO_vVectorMulAdd(pfdest, pfsrc, mul) AGO_vVectorMulAddInternal((MTH_tdxReal *)(pfdest), (MTH_tdxReal *)(pfsrc), (MTH_tdxReal)(mul))

void AGO_vProcessPart(unsigned long ulPartNumber, GLD_tdstViewportAttributes *p_stVpt)
{
    AGO_Part *p;
    unsigned long ulTemp;
    MTH_tdxReal TempValue;

    p = &AGO_g_AGO[ulPartNumber];

    ulTemp = (p->ulType & AGO_TYPEMASK);

    switch(ulTemp)
    {

        case AGO_STILL:
            break;

        /* 0x461152 */
        case AGO_FALLING_MEGAFAST:
                p->vDirection.xZ += -(MTH_M_xMul(MTH_M_xConstFloatToReal(39.24f), AGO_g_fTimeDifference));
        case AGO_GEISER:

        case AGO_FALLING:
                p->vDirection.xZ += -(MTH_M_xMul(AGO_G_SLOW, AGO_g_fTimeDifference));

        case AGO_FALLING_SLOW:
                p->vDirection.xZ += -(MTH_M_xMul(AGO_G_SLOW, AGO_g_fTimeDifference));

        case AGO_MOVING:
            {
                AGO_vVectorMulAdd(&p->vPosition.xX, &p->vDirection.xX, AGO_g_fTimeDifference);
            }
            break;

        case AGO_FASEGOINGTO:
            {
                MTH_tdxReal Val1, Val2;
                int Finished = 0;
                int i;
                MTH_tdxReal *pPos = &p->vPosition.xX;
                MTH_tdxReal *pDir = &p->vDirection.xX;
                MTH_tdxReal *pInitPos = &p->vInitialPosition.xX;

                for (i = 0 ; i < 3 ; i++)
                {
                    Val1 = pDir[i] - pPos[i];
                    if (MTH_M_xAbs(Val1) <= MTH_M_xConstFloatToReal(0.1f))
                        Finished++;
                    else
                    {
                        pInitPos[i] = pPos[i];
                        Val2 = MTH_M_xMul(MTH_M_xMul(MTH_M_xConstFloatToReal(6.5f), Val1), AGO_g_fTimeDifference);
                        if (MTH_M_xAbs(Val2) >= MTH_M_xAbs(Val1))
                        {
                            Finished++;
                            pPos[i] = pDir[i];
                        }
                        else
                            pPos[i] += Val2;
                    }
                }
                if (Finished)
                    p->ulType |= AGO_MUSTDIE;
            }
        break;

        case AGO_GOINGTO:
            {
                MTH_tdxReal Val1, Val2;
                int Finished = 0;
                int i;
                MTH_tdxReal *pPos = &p->vPosition.xX;
                MTH_tdxReal *pDir = &p->vDirection.xX;
                MTH_tdxReal *pInitPos = &p->vInitialPosition.xX;

                for (i = 0 ; i < 3 ; i++)
                {
                    Val1 = pDir[i] - pPos[i];
                    if (MTH_M_xAbs(Val1) <= MTH_M_xConstFloatToReal(0.1f))
                        Finished++;
                    else
                    {
                        pInitPos[i] = pPos[i];
                        Val2 = MTH_M_xMul3(MTH_M_xConstFloatToReal(1.5f), Val1, AGO_g_fTimeDifference);
                        if (MTH_M_xAbs(Val2) >= MTH_M_xAbs(Val1))
                        {
                            Finished++;
                            pPos[i] = pDir[i];
                        }
                        else
                            pPos[i] += Val2;
                    }
                }
                if (Finished)
                    p->ulType |= AGO_MUSTDIE;
            }
        break;

        case AGO_GOTO:
            {
                MTH3D_M_vAddVector(&p->vPosition, &p->vPosition, &p->vInitialPosition);
                p->fTimeLimit += MTH_C_ONE;
                if (p->fTimeLimit >= MTH_M_xSub(AGO_GOTO_STEP, MTH_C_ONE))
                    p->ulType |= AGO_MUSTDIE;
            }
        break;

        case AGO_MOVING_AND_SLOWING:
            {
                TempValue = MTH_M_xMul(MTH_M_xConstFloatToReal(-1.6f), AGO_g_fTimeDifference);
                p->vDirection.xX += TempValue;
                p->vDirection.xY += TempValue;
                p->vDirection.xZ += TempValue;

                AGO_vVectorMulAdd(&p->vPosition.xX, &p->vDirection.xX, AGO_g_fTimeDifference);
            }
            break;

        case AGO_MOVINGSINXYUP:
        case AGO_MOVINGSINXY:
            {
                AGO_vVectorMulAdd(&p->vPosition.xX, &p->vDirection.xX, AGO_g_fTimeDifference);
            }
            break;

        case AGO_SPIRALE:
            {
                if (p->vInitialPosition.xX < MTH_M_xConstFloatToReal(0.25f))
                {
                    p->vInitialPosition.xX = MTH_M_xMul(p->vInitialPosition.xX, MTH_M_xConstFloatToReal(1.51f));
                    p->vInitialPosition.xY = MTH_M_xMul(p->vInitialPosition.xY, MTH_M_xConstFloatToReal(1.1f));
                }
                else
                if (p->vInitialPosition.xX < MTH_M_xConstFloatToReal(0.5f))
                {
                    p->vInitialPosition.xX = MTH_M_xMul(p->vInitialPosition.xX, MTH_M_xConstFloatToReal(1.1f));

                    p->vInitialPosition.xY = MTH_M_xMul(p->vInitialPosition.xY, MTH_M_xConstFloatToReal(1.01f));
                }
                else
                {
                    p->vInitialPosition.xX = MTH_M_xMul(p->vInitialPosition.xX, MTH_M_xConstFloatToReal(0.9f));
                    p->vInitialPosition.xY = MTH_M_xMul(p->vInitialPosition.xY, MTH_M_xConstFloatToReal(0.98f));
                }
            }

            break;

        case AGO_2DSPIRALE:
        {
            MTH3D_tdstVector V1, V2;
            MTH_tdxReal Sin1, Cos1, Angle;

            p->fAnimTimeScale -= MTH_M_xMul(MTH_M_xMul((*((MTH_tdxReal *)(&p->AnimTime))), AGO_g_fTimeDifference), MTH_M_xConstFloatToReal(-6.5f));

            (*((MTH_tdxReal *)(&p->AnimTime))) = MTH_M_xSub((*((MTH_tdxReal *)(&p->AnimTime))), MTH_M_xMul(AGO_g_fTimeDifference, MTH_M_xConstFloatToReal(-0.20f)));
            p->fTimeLimit -= AGO_g_fTimeDifference;
            if (p->fTimeLimit <= MTH_M_xConstFloatToReal(0.01f))
            {
                p->ulType |= AGO_MUSTDIE;
            }

            Angle = p->fAnimTimeScale;
            Sin1 = MTH_M_xMul(SINUS(Angle), p->fTimeLimit);
            Cos1 = MTH_M_xMul(COSINUS(Angle), p->fTimeLimit);

            AGO_Define_OrthogonalVectors(&p->vDirection, &V1, &V2);
            p->vPosition.xX = p->vInitialPosition.xX + MTH_M_xMul(V1.xX, Sin1) + MTH_M_xMul(V2.xX, Cos1);
            p->vPosition.xY = p->vInitialPosition.xY + MTH_M_xMul(V1.xY, Sin1) + MTH_M_xMul(V2.xY, Cos1);
            p->vPosition.xZ = p->vInitialPosition.xZ + MTH_M_xMul(V1.xZ, Sin1) + MTH_M_xMul(V2.xZ, Cos1);
        }
        break;

        case AGO_ROTATING_SLOW:
        {
            MTH_tdxReal cosx, sinx, cosy,siny,cosz, sinz;
            MTH_tdxReal X,Y,Z;
            MTH_tdxReal pos0,pos1,pos2;

            X = MTH_M_xConstFloatToReal(0.707f);
            Y = MTH_M_xConstFloatToReal(0.50f);
            Z = MTH_M_xConstFloatToReal(0.50f);

            pos0 = MTH_M_xMul(p->vInitialPosition.xX, AGO_g_ffFrameTime);
            pos1 = MTH_M_xMul(p->vInitialPosition.xY, AGO_g_ffFrameTime);
            pos2 = MTH_M_xMul(p->vInitialPosition.xZ, AGO_g_ffFrameTime);

            cosx = COSINUS(pos0); sinx = SINUS(pos0);
            cosy = COSINUS(pos1); siny = SINUS(pos1);
            cosz = COSINUS(pos2); sinz = SINUS(pos2);

            p->vPosition.xX = p->vDirection.xX +
                MTH_M_xMul(p->fTimeLimit,
                (
                MTH_M_xMul(MTH_M_xMul_32(cosy, cosz), X) +
                MTH_M_xMul(MTH_M_xMul3_32(-siny, cosx, cosz) - MTH_M_xMul_32(cosx, sinz), Y) +
                MTH_M_xMul(MTH_M_xMul3_32(-siny, cosx, cosz) + MTH_M_xMul_32(sinx, sinz), Z)
                ));
            p->vPosition.xY = p->vDirection.xY +
                MTH_M_xMul(p->fTimeLimit,
                (
                MTH_M_xMul(MTH_M_xMul_32(cosy, sinz), X) +
                MTH_M_xMul(MTH_M_xMul3_32(-sinz, siny, sinx) + MTH_M_xMul_32(cosx, cosz), Y) +
                MTH_M_xMul(MTH_M_xMul3_32(-sinz, siny, cosx) - MTH_M_xMul_32(sinx, cosz), Z)
                ));
            p->vPosition.xZ = p->vDirection.xZ +
                MTH_M_xMul(p->fTimeLimit,
                (
                MTH_M_xMul(siny, X) +
                MTH_M_xMul(MTH_M_xMul_32(sinx, siny), Y) +
                MTH_M_xMul(MTH_M_xMul_32(cosx, cosy), Z)
                ));

            p->fTimeLimit -= MTH_M_xMul(MTH_M_xConstFloatToReal(4.5f), AGO_g_fTimeDifference);
            if (p->fTimeLimit <= MTH_M_xConstFloatToReal(0.01f))
                p->ulType |= AGO_MUSTDIE;
        }
        break;

        default: break;

    }

    if ((p->ulType & AGO_ENDASPLOUF) && (AGO_g_PloufMaterial != 0) && (!(p->ulType & AGO_ZELDA)))
    {
        if (p->vPosition.xZ <= p->fTimeLimit)
        {

            p->vPosition.xZ = p->fTimeLimit;
            p->ulType = AGO_RAYMAN_SMALL_PLOUF;
            p->fScale[0] = MTH_M_xConstFloatToReal(0.1f);
            p->fScale[1] = MTH_M_xConstFloatToReal(0.1f);
            p->fCreationTime = AGO_g_ffFrameTime;
            p->Transparency = MTH_M_xLongToReal(250);
            p->VisualMaterial = (void *)AGO_g_PloufMaterial;
        }
    }

    TempValue = MTH_C_ZERO;

    switch (p->ulType & AGO_SCALEMASK)
    {
        case AGO_SCALING_MEGAFASTY:
            p->fScale[0] -= MTH_M_xMul(MTH_M_xConstFloatToReal(-5.3f), AGO_g_fTimeDifference);
            p->fScale[1] -= MTH_M_xMul(MTH_M_xConstFloatToReal(-20.5f), AGO_g_fTimeDifference);
            break;

        case AGO_SCALING_XTOY:
            p->fScale[0] -= MTH_M_xMul(MTH_M_xConstFloatToReal(-60.0f), AGO_g_fTimeDifference);
            p->fScale[1] -= MTH_M_xMul3(EXPLOSION_SPEED, MTH_M_xConstFloatToReal(5.5f), AGO_g_fTimeDifference);
            if (p->fScale[0] >= MTH_C_5)
            {
                p->ulType &= (~AGO_SCALEMASK);
                p->ulType |= AGO_SCALING_YTOX;
            }
            break;
        case AGO_SCALING_YTOX:
            p->fScale[0] -= MTH_M_xMul3(EXPLOSION_SPEED, MTH_M_xLongToReal(20), AGO_g_fTimeDifference);

            if (p->fScale[0] <= MTH_C_ONE)
            {
                p->fScale[0] = MTH_C_2;
                p->ulType &= (~AGO_SCALEMASK);
                p->ulType |= AGO_SCALING_YTOX2;
            }
            break;
        case AGO_SCALING_YTOX2:
            p->fScale[0] -= MTH_M_xMul3(EXPLOSION_SPEED, MTH_M_xConstFloatToReal(5.5f), AGO_g_fTimeDifference);
            p->fScale[1] -= MTH_M_xMul(MTH_M_xConstFloatToReal(-61.5f), AGO_g_fTimeDifference);
            if (p->fScale[1] >= MTH_M_xLongToReal(10))
            {
                p->ulType |= AGO_MUSTDIE;
            }
            break;
        case AGO_SCALING_MEGASLOW:
            TempValue = MTH_M_xConstFloatToReal(0.05f);
            break;
        case AGO_SCALING_SLOW:
            TempValue = MTH_M_xConstFloatToReal(1.3f);
            break;
        case AGO_SCALING_MID:
            TempValue = MTH_M_xConstFloatToReal(3.7f);
            break;
        case AGO_SCALING_FAST:
            TempValue = MTH_M_xConstFloatToReal(6.5f);
            break;
        case AGO_SCALING_FASTEST:
            TempValue = MTH_M_xConstFloatToReal(9.5f);
            break;
        case AGO_UNSCALING_FAST:
            TempValue = MTH_M_xConstFloatToReal(-0.5f);
            break;
        case AGO_UNSCALING_FASTEST:
            TempValue = MTH_M_xConstFloatToReal(-3.5f);
            break;
        case AGO_UNSCALING_SLOW:
            TempValue = MTH_M_xConstFloatToReal(-0.2f);
            break;
        default:
            break;
    }

    if (TempValue != MTH_C_ZERO)
    {
        TempValue = MTH_M_xMul(TempValue, AGO_g_fTimeDifference);
        p->fScale[0] += TempValue;
        p->fScale[1] += TempValue;
    }

    if ( (p->ulType & AGO_SCALEMASK) == AGO_BIG_RANDOM_SCALE)
    {
        p->fScale[1]=p->fScale[0] = FLOAT_RANGED_RANDOM(MTH_C_ZERO, MTH_C_ONE);
    }
    else if ( (p->ulType & AGO_SCALEMASK) == AGO_SMALL_RANDOM_SCALE)
    {
        p->fScale[1]=p->fScale[0] = FLOAT_RANGED_RANDOM(MTH_M_xConstFloatToReal(0.2f), MTH_M_xConstFloatToReal(0.6f));
    }

    if (p->fScale[0] <= MTH_C_ZERO)
    {
        p->fScale[0] = MTH_M_xConstFloatToReal(0.01f);
        p->ulType |= AGO_MUSTDIE;
    }
    if (p->fScale[1] <= MTH_C_ZERO)
    {
        p->fScale[1] = MTH_M_xConstFloatToReal(0.01f);
        p->ulType |= AGO_MUSTDIE;
    }

    switch (p->ulType & AGO_SHADEMASK)
    {
        case AGO_WAIT:
            if (AGO_g_ffFrameTime-p->fCreationTime>p->fTimeLimit)
            {
                p->ulType&=~AGO_SHADEMASK;
                p->ulType|=AGO_SHADE_FAST;

            }
            break;
        case AGO_APPEAR:
            p->Transparency -= MTH_M_xMul(MTH_M_xConstFloatToReal(-350.0f), AGO_g_fTimeDifference);
            if (p->Transparency >= MTH_M_xLongToReal(255))
            {
                p->Transparency = MTH_M_xLongToReal(255);
                p->ulType &= ~AGO_SHADEMASK;
                p->ulType |= AGO_WAIT;
                p->fCreationTime = AGO_g_ffFrameTime;
            }
            break;
        case AGO_SHADE_SLOW:
            p->Transparency -= MTH_M_xMul(MTH_M_xLongToReal(100), AGO_g_fTimeDifference);
            break;
        case AGO_SHADE_MID:
            p->Transparency -= MTH_M_xMul(MTH_M_xLongToReal(160), AGO_g_fTimeDifference);
            break;
        case AGO_SHADE_FAST:
            p->Transparency -= MTH_M_xMul(MTH_M_xLongToReal(260), AGO_g_fTimeDifference);
            break;
        case AGO_SHADE_FASTEST:
            p->Transparency -= MTH_M_xMul(MTH_M_xLongToReal(350), AGO_g_fTimeDifference);
            break;
        case AGO_SHADE_MEGAFAST:
            p->Transparency -= MTH_M_xMul(MTH_M_xLongToReal(850), AGO_g_fTimeDifference);
            break;
        case AGO_SHADE_TWOFRAMES:
            p->Transparency -= MTH_M_xMul(MTH_M_xLongToReal(2500), AGO_g_fTimeDifference);
            break;
        case AGO_RANDOM_SHADE:
            p->Transparency = FLOAT_RANGED_RANDOM(MTH_M_xLongToReal(100), MTH_M_xLongToReal(255));
            break;
        case AGO_UNSHADE:
            p->Transparency -= MTH_M_xMul(MTH_M_xConstFloatToReal(-100.0f), AGO_g_fTimeDifference);
            break;
        default:
            break;
    }

    if ((p->Transparency < MTH_C_ZERO) && ((p->ulType & AGO_SHADEMASK) != AGO_APPEAR))
    {
        p->ulType |= AGO_MUSTDIE;
        p->Transparency = MTH_C_ZERO;
    }

    AGO_vDisplayPart(ulPartNumber, p_stVpt);
}

#define GLI_drawOriented(p, m, w, h, v, t, a, b) \
    GLI_vDrawWorldOrthoPlanarSprite((struct MTH3D_tdstVector_ *)(p), (void *)(m), (w), (h), (v), \
        *(unsigned long *)&(t), (struct MTH3D_tdstVector_ *)(a), (struct MTH3D_tdstVector_ *)(b), 0)
#define GLI_drawXYPlan(p, m, w, h, v, t) \
    GLI_vDrawWorldPlanarSprite((struct MTH3D_tdstVector_ *)(p), (void *)(m), (w), (h), (v), *(unsigned long *)&(t))
/* 0x00429890 */
extern unsigned char __cdecl GLI_vDrawWorldSprite(void *pPosition, void *hMaterial,
                                                  float xScaleX, float xScaleY,
                                                  void *p_stViewport, unsigned long ulAnimTime);
#define GLI_drawBillboard(p, m, w, h, v, t) \
    GLI_vDrawWorldSprite((p), (void *)(m), (w), (h), (v), *(unsigned long *)&(t))
/* 0x4E1068 */
void AGO_vDisplayPart(unsigned long ulPartNumber, GLD_tdstViewportAttributes *p_stVpt)
{
    unsigned long *pu = (unsigned long *)&AGO_g_AGO[ulPartNumber];
    float         *pf = (float *)pu;
    float          local_3c = 0.0f, local_38 = 0.0f, local_34 = 0.0f;
    float          vecC[3], vec18[3];
    float          posBase[3], dirB[3], triA[3], triB[3];
    unsigned long  uVar7, uVar8, t;
    int            iMat;
    unsigned char  cull;
    float          fVar13;
    (void)local_34;

    switch (pu[0] & 0xff)
    {
    case 7: case 0xb:
    {
        float a = (AGO_g_ffFrameTime - pf[2]) * pf[5];
        float c = (float)cos(a), s = (float)sin(a);
        local_38 = c * pf[3];
        local_3c = s * pf[3];
        fVar13   = pf[8];
        pf[6] = local_3c + pf[6];
        pf[7] = local_38 + pf[7];
        pf[8] = c * pf[3] + local_3c + fVar13;
        break;
    }
    case 8:
    {
        float a = (AGO_g_ffFrameTime - pf[2]) * pf[5] + pf[0xb];
        float c = (float)cos(a), s = (float)sin(a);
        local_38 = pf[3] * c;
        local_34 = pf[4] * s;
        local_3c = pf[4] * c + pf[3] * s;
        fVar13   = pf[8];
        pf[6] = local_3c + pf[6];
        pf[7] = local_38 + pf[7];
        pf[8] = local_34 + fVar13;
        break;
    }
    }

    if ((char)pu[0] == 0x0c) AGO_g_CurrentAnimTime = 1.0f;
    else                     AGO_g_CurrentAnimTime = pf[0xe];

    fVar13 = pf[0x11];
    if (AGO_k_f255 <= fVar13) fVar13 = 255.0f;
    GLI_vSetGlobalAlpha(fVar13);

    iMat = (int)pu[0x10];
    if (iMat == 0) return;

    uVar7 = pu[0];
    if ((char)uVar7 == 0x11)
    {
        GLI_vDisplayFrame(&pf[6], &pf[9], pf[0x11], p_stVpt);
        pu[0] |= 0x1000000;
    }
    else
    {
        cull = 0;
        uVar8 = uVar7 & 0xf0000000;
        if (uVar8 == 0x30000000)
        {
            float negY = -pf[0xa], mag, inv;
            vecC[0] = -pf[9];
            vecC[2] = 0.0f;
            mag = (float)sqrt(negY * negY + vecC[0] * vecC[0]);
            if (mag < AGO_k_f0_01) { pu[0] |= 0x1000000; goto LAB_undo; }
            inv = AGO_k_f1 / mag;
            vec18[2] = 0.0f;
            vec18[0] = -(inv * pf[0xa]);
            vec18[1] = inv * pf[9];
            vecC[0]  = inv * vecC[0];
            vecC[1]  = inv * negY;
            cull = GLI_drawOriented(&pf[6], iMat, pf[0xc], pf[0xd], p_stVpt, pf[1],
                                    vec18, vecC);
        }
        else if (uVar8 == 0x40000000 || uVar8 == 0x50000000 || uVar8 == 0x60000000)
        {

            float fMag2D;
            dirB[0] = pf[6] - pf[3];
            dirB[1] = pf[7] - pf[4];
            dirB[2] = pf[8] - pf[5];
            if (uVar8 == 0x60000000) dirB[2] = 0.0f;
            posBase[0] = pf[3] - dirB[0] * AGO_k_fMinus0_5;
            posBase[1] = pf[4] - dirB[1] * AGO_k_fMinus0_5;
            posBase[2] = pf[5] - dirB[2] * AGO_k_fMinus0_5;
            triA[1] = dirB[2];
            triA[2] = AGO_k_f0;
            if (uVar8 == 0x60000000) triA[1] = dirB[0];
            fMag2D = dirB[1] * dirB[1] + dirB[0] * dirB[0];
            if (AGO_k_f0_05_a <= fMag2D)
            {
                float mag3D = (float)sqrt((double)(dirB[2] * dirB[2] + fMag2D));
                float inv2D = AGO_k_f1 / (float)sqrt((double)fMag2D);
                float inv3D = AGO_k_f1 / mag3D;
                pf[0xc] = mag3D * AGO_k_f0_5;
                triA[0] = inv2D * -dirB[1];
                triA[1] = inv2D * triA[1];
                dirB[0] = inv3D * dirB[0];
                dirB[1] = inv3D * dirB[1];
                dirB[2] = inv3D * dirB[2];
                if (uVar8 == 0x60000000)
                {
                    if (AGO_g_bInGameMenu != 0) goto LAB_undo;
                    GLI_drawOriented(posBase, iMat, 0.4f, pf[0xc], p_stVpt, pf[1], triA, dirB);
                }
                else if (uVar8 == 0x50000000)
                {
                    GLI_drawOriented(posBase, iMat, 0.3f, pf[0xc], p_stVpt, pf[1], triA, dirB);
                    triB[0] = triA[2] * dirB[1] - triA[1] * dirB[2];
                    triB[1] = triA[0] * dirB[2] - triA[2] * dirB[0];
                    triB[2] = triA[1] * dirB[0] - triA[0] * dirB[1];
                    GLI_drawOriented(posBase, (int)pu[0x10], 0.3f, pf[0xc], p_stVpt, pf[1], triB, dirB);
                }
                else
                {
                    if (AGO_g_OddFrame == 0)
                    {
                        triB[0] = -(triA[1] * dirB[2]);
                        triB[1] = triA[0] * dirB[2];
                        triB[2] = triA[1] * dirB[0] - triA[0] * dirB[1];
                        GLI_drawOriented(posBase, iMat, 0.1f, pf[0xc], p_stVpt, pf[1], triB, dirB);
                    }
                    else
                    {
                        GLI_drawOriented(posBase, iMat, 0.1f, pf[0xc], p_stVpt, pf[1], triA, dirB);
                    }
                }
            }
            goto LAB_undo;
        }
        else if (uVar8 == 0x20000000)
        {
            float *pA, *pB;
            AGO_Define_OrthogonalVectors((MTH3D_tdstVector *)&pf[9],
                                         (MTH3D_tdstVector *)vecC,
                                         (MTH3D_tdstVector *)vec18);
            if (((pu[0] & 0x2000000) != 0x2000000) || (vecC[2] <= vec18[2])) {
                pA = vecC; pB = vec18;
            } else {
                pA = vec18; pB = vecC;
            }
            cull = GLI_drawOriented(&pf[6], iMat, pf[0xc], pf[0xd], p_stVpt, pf[1], pA, pB);
        }
        else if (uVar8 == 0x10000000)
        {
            cull = GLI_drawXYPlan(&pf[6], iMat, pf[0xc], pf[0xd], p_stVpt, pf[1]);
        }
        else
        {
            if (uVar7 & 0x8000000) pf[8] = pf[0xd] + pf[8];
            cull = GLI_drawBillboard(&pf[6], iMat, pf[0xc], pf[0xd], p_stVpt, pf[1]);
            if (cull) pu[0] |= 0x1000000;
            if (pu[0] & 0x8000000) pf[8] = pf[8] - pf[0xd];
            goto LAB_undo;
        }
        if (cull) pu[0] |= 0x1000000;
    }

LAB_undo:

    t = pu[0] & 0xff;
    if ((t == 7) || (t == 0xb))
    {
        fVar13 = pf[8];
        pf[6] = pf[6] - local_3c;
        pf[7] = pf[7] - local_38;
        pf[8] = fVar13 - (local_38 + local_3c);
    }
    if (t == 8)
    {
        fVar13 = pf[8];
        pf[6] = pf[6] - local_3c;
        pf[7] = pf[7] - local_38;
        pf[8] = fVar13 - local_34;
    }
}

#define AGO_call_vCreatePart AGO_vCreatePart

#define AGO_g_ExtraMaterial      ((void *)AGO_bb_Mat)

#define AGO_call_vCreateTriangle AGO_vCreateTriangle

#define AGO_call_CompareVector AGO_CompareVector

#define AGO_FALLING               0x00000001
#define AGO_MOVING                0x00000003
#define AGO_FALLING_SLOW          0x00000005
#define AGO_FASEGOINGTO           0x0000000E
#define AGO_SCALING_SLOW          0x00000100
#define AGO_SCALING_MID           0x00000200
#define AGO_SCALING_FAST          0x00000300
#define AGO_SCALING_FASTEST       0x00000400
#define AGO_UNSCALING_FAST        0x00000500
#define AGO_UNSCALING_SLOW        0x00000600
#define AGO_SCALING_MEGASLOW      0x00000800
#define AGO_SCALING_MEGAFASTY     0x00000900
#define AGO_SCALING_XTOY          0x00000A00
#define AGO_SHADE_SLOW            0x00001000
#define AGO_SHADE_MID             0x00002000
#define AGO_SHADE_FAST            0x00003000
#define AGO_SHADE_FASTEST         0x00004000
#define AGO_SHADE_MEGAFAST        0x00005000
#define AGO_SHADE_TWOFRAMES       0x00006000
#define AGO_REMANANCE2            0x50000000
#define AGO_BASEDSCALE            0x08000000
#define AGO_XYPLAN                0x10000000
#define AGO_ORTHOPLAN             0x20000000
#define AGO_RAYMAN_SMALL_PLOUF    (AGO_STILL | AGO_XYPLAN | AGO_SCALING_SLOW |  AGO_SHADE_FAST)
#define AGO_APPARITION_FUMEE      (AGO_MOVING | AGO_SCALING_FAST | AGO_SHADE_FAST)

#define AGO_GOINGTO               0x0000000A
#define AGO_ORIENTEDXYPLAN        0x30000000
#define AGO_REMANANCE             0x40000000
#define AGO_RAYMAN_PLOUF_SKI      (AGO_STILL | AGO_ORIENTEDXYPLAN | AGO_SCALING_SLOW |  AGO_SHADE_MEGAFAST)

#undef FLOAT_RANGED_RANDOM
#define FLOAT_RANGED_RANDOM(mn, mx) \
    ((float)rand() * ((mx) - (mn)) * AGO_C_xRandN - (-(mn)))

#undef FLOAT_RANGED_RANDOM
#define FLOAT_RANGED_RANDOM(mn, mx) \
    ((float)rand() * ((mx) - (mn)) * AGO_C_xRandN + (mn))

#ifndef AGO_SHADE_TWOFRAMES
#define AGO_SHADE_TWOFRAMES 0x00006000
#endif
#ifndef AGO_LOOPANIM
#define AGO_LOOPANIM        0x04000000
#endif
#ifndef AGO_REMANANCE2
#define AGO_REMANANCE2      0x50000000
#endif
#ifndef AGO_SHADE_MEGAFAST
#define AGO_SHADE_MEGAFAST  0x00005000
#endif
#ifndef AGO_SCALING_FASTEST
#define AGO_SCALING_FASTEST 0x00000400
#endif

void AddGauge(MTH_tdxReal *V1, MTH_tdxReal *V2, MTH_tdxReal Num, void *EmptyMat, void *FullMat);
void AddIcone(MTH_tdxReal *V1, MTH_tdxReal *V2, MTH_tdxReal Num, void *Mat);
void CreateType72AGO(MTH3D_tdstVector *p_stPosition, MTH3D_tdstVector *p_stDirection,
                     void *hVisualMaterial, MTH_tdxReal parameter);
void CreateType83AGO(MTH3D_tdstVector *p_stPosition, MTH3D_tdstVector *p_stDirection,
                     void *hVisualMaterial, MTH_tdxReal parameter);
void CreateType104AGO(MTH3D_tdstVector *p_stPosition, MTH3D_tdstVector *p_stDirection,
                      void *hVisualMaterial, MTH_tdxReal parameter);
void CreateType158AGO(MTH3D_tdstVector *p_stPosition, MTH3D_tdstVector *p_stDirection,
                      void *hVisualMaterial, MTH_tdxReal parameter);
void AGO_vAddLogoPart(MTH_tdxReal parameter, void *hVisualMaterial);
void AGO_vActivateLogoDisplay(MTH3D_tdstVector *pPosition, MTH3D_tdstVector *pDirection, MTH_tdxReal parameter);

void CreateType5AGO(MTH3D_tdstVector *pPos, MTH3D_tdstVector *pDir,
                      void *hVisualMaterial, float parameter)
{
    float pp[3], dd[3];
    (void)pDir;
    pPos->xZ = pPos->xZ - AGO_k_fMinus0_6;

    AGO_g_CurrentAnimTime = (float)rand() * AGO_k_f1_3 * AGO_k_f3_05185em05 - AGO_k_fMinus0_7;
    pp[0] = pPos->xX; pp[1] = pPos->xY - AGO_k_fMinus0_5; pp[2] = pPos->xZ - AGO_k_fMinus0_8;
    dd[0] = AGO_k_f1; dd[1] = AGO_k_f0_56; dd[2] = AGO_k_fMinus0_53;
    AGO_call_vCreatePart(0x1103, (MTH3D_tdstVector *)pp, (MTH3D_tdstVector *)dd, parameter, 0.8f, 0.8f, hVisualMaterial);

    AGO_g_CurrentAnimTime = (float)rand() * AGO_k_f1_3 * AGO_k_f3_05185em05 - AGO_k_fMinus0_7;
    pp[0] = pPos->xX; pp[1] = pPos->xY - AGO_k_f0_5; pp[2] = pPos->xZ - AGO_k_fMinus1_5;
    dd[0] = AGO_k_fMinus0_5; dd[1] = AGO_k_fMinus0_98; dd[2] = AGO_k_f0_23;
    AGO_call_vCreatePart(0x1103, (MTH3D_tdstVector *)pp, (MTH3D_tdstVector *)dd, parameter, 0.5f, 0.6f, hVisualMaterial);

    AGO_g_CurrentAnimTime = (float)rand() * AGO_k_f1_3 * AGO_k_f3_05185em05 - AGO_k_fMinus0_7;
    pp[0] = pPos->xX; pp[1] = pPos->xY - AGO_k_fMinus0_5; pp[2] = pPos->xZ - AGO_k_f0_5;
    dd[0] = AGO_k_f0_5; dd[1] = AGO_k_f0_16; dd[2] = AGO_k_f0_13;
    AGO_call_vCreatePart(0x1103, (MTH3D_tdstVector *)pp, (MTH3D_tdstVector *)dd, parameter, 0.3f, 0.3f, hVisualMaterial);

    AGO_g_CurrentAnimTime = (float)rand() * AGO_k_f1_3 * AGO_k_f3_05185em05 - AGO_k_fMinus0_7;
    pp[0] = pPos->xX; pp[1] = pPos->xY - AGO_k_f0_5; pp[2] = pPos->xZ;
    dd[0] = AGO_k_f1; dd[1] = AGO_k_fMinus0_29; dd[2] = AGO_k_fMinus0_63;
    AGO_call_vCreatePart(0x1103, (MTH3D_tdstVector *)pp, (MTH3D_tdstVector *)dd, parameter, 1.0f, 1.0f, hVisualMaterial);
}

#define FLOAT_RANGED_RANDOM_RETAIL(mn, mx) \
    ((float)rand() * ((mx) - (mn)) * AGO_C_xRandN - (-(mn)))

/* 0x00461D80 */
void CreateType8AGO(MTH3D_tdstVector *p_stPosition, MTH3D_tdstVector *p_stDirection, void *hVisualMaterial, float parameter)
{
	MTH3D_tdstVector Pos;
	MTH3D_tdstVector Direction;
	MTH3D_tdstVector Pos2;
	MTH_tdxReal fNorm;
	MTH_tdxReal vx,vy;

	MTH3D_M_vCopyVector(&Pos, p_stPosition);
	Pos.xZ = MTH_M_xAdd(Pos.xZ, MTH_M_xConstFloatToReal(0.2f));
	MTH3D_M_vCopyVector(&Pos2, &Pos);
	MTH3D_M_vCopyVector(&Direction, p_stDirection);

	/* 0x461D9D */
	vx = -(Direction.xY);
	vy = Direction.xX;

	fNorm = MTH_M_xSqrt(MTH_M_xSqrAddSqr(vx, vy));

	if (fNorm >= 0.01f)
	{
		fNorm = MTH_M_xDiv(MTH_M_xConstFloatToReal(0.4f), fNorm);
		vx = MTH_M_xMul(vx, fNorm);
		vy = MTH_M_xMul(vy, fNorm);

		fNorm = FLOAT_RANGED_RANDOM_RETAIL(MTH_M_xConstFloatToReal(-0.9f), MTH_M_xConstFloatToReal(0.0f));

 		Pos.xX = Pos.xX + -(vx) + MTH_M_xMul(fNorm, Direction.xX);
 		Pos.xY = Pos.xY + -(vy) + MTH_M_xMul(fNorm, Direction.xY);

		fNorm = FLOAT_RANGED_RANDOM_RETAIL(MTH_M_xConstFloatToReal(-0.9f), MTH_M_xConstFloatToReal(0.0f));
		Pos2.xX = Pos2.xX + vx + MTH_M_xMul(fNorm, Direction.xX);
		Pos2.xY = Pos2.xY + vy + MTH_M_xMul(fNorm, Direction.xY);

		AGO_vCreatePart( AGO_RAYMAN_PLOUF_SKI ,  &Pos,   &Direction, MTH_M_xLongToReal(200), MTH_M_xConstFloatToReal(0.2f), MTH_M_xConstFloatToReal(0.8f), hVisualMaterial );
		AGO_vCreatePart( AGO_RAYMAN_PLOUF_SKI ,  &Pos2,  &Direction, MTH_M_xLongToReal(200), MTH_M_xConstFloatToReal(0.2f), MTH_M_xConstFloatToReal(0.8f), hVisualMaterial );
	}
}

void CreateType16AGO(MTH3D_tdstVector *pPos, MTH3D_tdstVector *pDir, void *hVisualMaterial, float parameter)
{
    float dvx = pDir->xX - AGO_g_Type16PrevX;
    float dvy = pDir->xY - AGO_g_Type16PrevY;
    float mag2;
    AGO_g_Type16PrevX = pDir->xX;
    mag2 = dvy * dvy + dvx * dvx;
    AGO_g_Type16PrevY = pDir->xY;
    if (AGO_k_f0_0004 <= mag2)
    {
        float dx = pDir->xX, dy = pDir->xY;
        float ny = -dy;
        float flag = 0.0f;
        float mag = (float)sqrt(dx * dx + ny * ny);
        float pz  = pPos->xZ - AGO_k_fMinus0_2;
        if (AGO_k_f0 <= (dvx + dvy) * mag2) flag = 1.0f;
        if (AGO_k_f0_01 <= mag)
        {
            float posA[3], posB[3], outDir[3];
            float A  = mag * ny;
            float B  = mag * dx;
            float fA = A * AGO_k_f0_4_b;
            float fB = B * AGO_k_f0_4_b;
            float r1 = (float)rand() * AGO_k_f1_5 * AGO_k_f3_05185em05;
            float r2;
            posB[0] = (r1 * dx - fA) + pPos->xX;
            posB[1] = (r1 * dy - fB) + pPos->xY;
            r2 = (float)rand() * AGO_k_f1_3 * AGO_k_f3_05185em05;
            posB[0] = posB[0] - fA;
            posB[1] = posB[1] - fB;
            posB[2] = pz;
            posA[0] = ((r2 * dx - A * AGO_k_fMinus0_4) + pPos->xX) - A * AGO_k_fMinus0_4;
            posA[1] = ((r2 * dy - B * AGO_k_fMinus0_4) + pPos->xY) - B * AGO_k_fMinus0_4;
            posA[2] = pz;
            outDir[0] = dy * AGO_k_fMinus0_6;
            outDir[1] = dx * AGO_k_f0_6;
            outDir[2] = A * dy - dx * B;
            if (flag == AGO_k_f0)
                AGO_call_vCreatePart(0x22005300, (MTH3D_tdstVector *)posA,
                                     (MTH3D_tdstVector *)outDir, 200.0f, mag2, mag2, hVisualMaterial);
            else {
                outDir[2] = -outDir[2];
                AGO_call_vCreatePart(0x22005300, (MTH3D_tdstVector *)posB,
                                     (MTH3D_tdstVector *)outDir, 200.0f, mag2, mag2, hVisualMaterial);
            }
        }
    }
}

/* 0x00462230 */
void CreateType72AGO(MTH3D_tdstVector *p_stPosition, MTH3D_tdstVector *p_stDirection,
                     void *hVisualMaterial, MTH_tdxReal parameter)
{
    MTH3D_tdstVector Pos;
    MTH3D_tdstVector Direction;
    int i;
    MTH_tdxReal scale;

    MTH3D_M_vCopyVector(&Direction, p_stDirection);

    for (i = 0 ; i < 3 ; i++)
    {
        MTH_tdxReal x = FLOAT_RANGED_RANDOM_RETAIL(MTH_M_xConstFloatToReal(0.05f), MTH_M_xConstFloatToReal(0.95f));

        MTH3D_M_vLinearInterpolVectorORG(&Pos, p_stPosition, p_stDirection, x);
        scale = FLOAT_RANGED_RANDOM_RETAIL(MTH_M_xConstFloatToReal(0.2f), MTH_M_xConstFloatToReal(1.0f));
        AGO_vCreatePart(AGO_STILL | AGO_SHADE_MEGAFAST | AGO_SCALING_FASTEST, &Pos, &Direction, parameter, scale, scale, hVisualMaterial);
    }
}

void CreateType52AGO(MTH3D_tdstVector *pPos, MTH3D_tdstVector *pDir, void *hVisualMaterial, float parameter)
{
    float v[6];
    const float *ex1 = AGO_gExtraParameter1;
    float pin[3], din[3];
    float scale;
    int j;
    pin[0] = pPos->xX; pin[1] = pPos->xY; pin[2] = pPos->xZ;
    din[0] = pDir->xX; din[1] = pDir->xY; din[2] = pDir->xZ;
    for (j = 0; j < 3; j++)
    {
        v[3 + j] = ((float)rand() * AGO_k_f8 * AGO_k_f3_05185em05 - AGO_k_f4)
                   - (ex1[j] * AGO_k_fMinus0_3 + din[j] * AGO_k_fMinus4) + pin[j];
        v[j]     = ((float)rand() * AGO_k_f0_4_b * AGO_k_f3_05185em05 - AGO_k_f0_2)
                   + ex1[j];
    }
    scale = (float)rand() * AGO_k_f0_05_b * AGO_k_f3_05185em05 - AGO_k_fMinus0_4;
    AGO_call_vCreatePart(0x90003, (MTH3D_tdstVector *)&v[3], (MTH3D_tdstVector *)&v[0],
                         1.0f, scale, scale, hVisualMaterial);
}

#define AGO_FALLING             0x00000001

void CreateType60AGO(MTH3D_tdstVector *p_stPosition, MTH3D_tdstVector *p_stDirection, void *hVisualMaterial, float parameter)
{
	MTH3D_tdstVector Pos;
	MTH3D_tdstVector Direction;
	int i;
	MTH_tdxReal scale;

	/* 0x4B9AF8 */
	if (AGO_g_SplashFlag)
		return;

	MTH3D_M_vCopyVector(&Pos, p_stPosition);
	MTH3D_M_vCopyVector(&Direction, p_stDirection);

	for (i = 0 ; i < 10 ; i++)
	{
		Direction.xX = FLOAT_RANGED_RANDOM_RETAIL(MTH_M_xConstFloatToReal(-3.0f), MTH_M_xConstFloatToReal(3.0f));
		Direction.xY = FLOAT_RANGED_RANDOM_RETAIL(MTH_M_xConstFloatToReal(-3.0f), MTH_M_xConstFloatToReal(3.0f));
		Direction.xZ = FLOAT_RANGED_RANDOM_RETAIL(MTH_M_xConstFloatToReal(2.0f), MTH_M_xConstFloatToReal(6.0f));
		scale = FLOAT_RANGED_RANDOM_RETAIL(MTH_M_xConstFloatToReal(0.03f), MTH_M_xConstFloatToReal(0.05f));
		AGO_vCreatePart( AGO_FALLING | AGO_SHADE_SLOW | AGO_ENDASPLOUF,  &Pos,  &Direction,  Pos.xZ, scale, scale, hVisualMaterial);
	}
}

/* 0x00462580 */
void CreateType62AGO(MTH3D_tdstVector *p_stPosition, MTH3D_tdstVector *p_stDirection, void *hVisualMaterial, float parameter)
{
	MTH3D_tdstVector Pos;
	MTH3D_tdstVector Direction;
	int i;
	MTH_tdxReal scale;
	MTH_tdxReal time;
	MTH_tdxReal *p_xSrc;
	int lRand;

	p_xSrc = &p_stPosition->xX;
	Pos.xX = *p_xSrc++; Pos.xY = *p_xSrc++; Pos.xZ = *p_xSrc;
	p_xSrc = &p_stDirection->xX;
	Direction.xX = *p_xSrc++; Direction.xY = *p_xSrc++; Direction.xZ = *p_xSrc;

	for (i = 10 ; i != 0 ; i--)
	{

		lRand = rand(); Direction.xX = ((float)lRand + (float)lRand) * AGO_k_f3_05185em05 - AGO_k_f1;
		lRand = rand();
		Direction.xZ = MTH_C_ZERO;
		Direction.xY = ((float)lRand + (float)lRand) * AGO_k_f3_05185em05 - AGO_k_f1;

		scale = (float)rand() * AGO_k_f0_09 * AGO_k_f3_05185em05 - AGO_k_fMinus0_01;
		/* 0x4B9AF8 */
		if (AGO_g_SplashFlag)
		{

			time = (float)rand() * AGO_k_f0_8 * AGO_k_f3_05185em05 - AGO_k_fMinus0_2;
		}
		else
		{

			lRand = rand(); time = ((float)lRand + (float)lRand) * AGO_k_f3_05185em05 - AGO_k_fMinus1;
		}

		AGO_vCreatePart( AGO_MOVING | AGO_TIMELIMITED,  &Pos,  &Direction,  time, scale, scale, hVisualMaterial );
	}
}

/* 0x004626B0 */
void CreateType35AGO(MTH3D_tdstVector *p_stPosition, MTH3D_tdstVector *p_stDirection,
                       void *hVisualMaterial, float parameter)
{
	MTH3D_tdstVector Pos;
	MTH3D_tdstVector Direction;
	MTH3D_tdstVector stV1,stV2,stV3;
	MTH_tdxReal Mul1,Mul2;
	MTH_tdxReal cos1,sin1;

	MTH3D_M_vSubVector(&stV3, p_stPosition, p_stDirection);
	MTH3D_M_vNormalizeVector(&stV3, &stV3);

	AGO_Define_OrthogonalVectors(&stV3, &stV1, &stV2);

	Mul1 = FLOAT_RANGED_RANDOM_RETAIL(MTH_M_xConstFloatToReal(-6.28f), MTH_M_xConstFloatToReal(6.28f));

	cos1 = (MTH_tdxReal)cos((double)Mul1);
	sin1 = (MTH_tdxReal)sin((double)Mul1);

	stV3.xX = MTH_M_xMulAddMul(cos1, stV1.xX, sin1, stV2.xX);
	stV3.xY = MTH_M_xMulAddMul(cos1, stV1.xY, sin1, stV2.xY);
	stV3.xZ = MTH_M_xMulAddMul(cos1, stV1.xZ, sin1, stV2.xZ);

	Mul2 = MTH3D_M_xSqrVector(&stV3);

	if ((Mul2 < MTH_C_ONE) && (AGO_g_OddFrame))
	{
		Mul2 = MTH_M_xMul(Mul2, parameter);
	}
	else
	{
		Mul2 = MTH_M_xMul(MTH_M_xInvSqrt(Mul2), parameter);
	}
	MTH3D_M_vMulScalarVector(&Pos, Mul2, &stV3);
	MTH3D_M_vAddVector(&Pos, &Pos, p_stDirection);

	MTH3D_M_vCopyVector(&Direction, p_stPosition);

	AGO_vCreatePart( AGO_REMANANCE | AGO_GOINGTO ,  &Pos,  &Direction,  MTH_C_2 , MTH_M_xConstFloatToReal(0.9f), MTH_M_xConstFloatToReal(0.9f), hVisualMaterial );
}

/* 0x00462860 */
void CreateType15AGO(MTH3D_tdstVector *pPos, MTH3D_tdstVector *pDir,
                       void *hVisualMaterial, float parameter)
{
    float pin[3], din[3];
    int i, j;
    for (i = 8; i != 0; i--)
    {
        float v[6];
        float off, off2, sc;
        AGO_g_CurrentAnimTime = (float)rand() * AGO_k_f2_8 * AGO_k_f3_05185em05 - AGO_k_fMinus0_2;
        off  = (float)rand() * AGO_k_f1_3 * AGO_k_f3_05185em05 - AGO_k_f0_5;
        off2 = ((float)rand() * AGO_k_f1_7 * AGO_k_f3_05185em05 - AGO_k_fMinus0_5) * parameter;
        sc   = (float)rand() * AGO_k_f0_7 * AGO_k_f3_05185em05 - AGO_k_fMinus0_8;
        for (j = 0; j < 3; j++) {
            v[j]     = off2 * ((float *)pDir)[j];
            v[3 + j] = off * ((float *)pDir)[j] + ((float *)pPos)[j];
        }
        AGO_call_vCreatePart(0x3009, (MTH3D_tdstVector *)&v[3], (MTH3D_tdstVector *)&v[0],
                             off2, sc, sc, hVisualMaterial);
    }
}

/* 0x00462980 */
void CreateType158AGO(MTH3D_tdstVector *p_stPosition, MTH3D_tdstVector *p_stDirection,
                      void *hVisualMaterial, MTH_tdxReal parameter)
{
    float pos[3], dir[3];
    MTH_tdxReal scale;
    MTH_tdxReal offset2;
    int i, j;

#define FLOAT_RANGED_RANDOM_158(mn, mx) \
    ((float)rand() * ((mx) - (mn)) * AGO_C_xRandN - (-(mn)))
    for (i = 8; i; i--)
    {
        AGO_g_AGO_AnimTimeScale = FLOAT_RANGED_RANDOM_158(0.2f, 3.0f);
        offset2 = FLOAT_RANGED_RANDOM_158(4.0f, 6.5f);
        scale = parameter;
        for (j = 0; j < 3; j++)
        {
            dir[j] = (FLOAT_RANGED_RANDOM_158(-1.5f, 1.5f) + offset2) * ((float *)p_stDirection)[j];
            pos[j] = ((float)rand() * 0.3f * AGO_C_xRandN) * ((float *)p_stDirection)[j] + ((float *)p_stPosition)[j];
        }
        AGO_vCreatePart(0x2103, (MTH3D_tdstVector *)pos, (MTH3D_tdstVector *)dir,
                        offset2, scale, scale, hVisualMaterial);
    }
}

void CreateType21AGO(MTH3D_tdstVector *p_stPosition, MTH3D_tdstVector *p_stDirection,
                       void *hVisualMaterial, float parameter)
{
	MTH3D_tdstVector Pos;
	MTH3D_tdstVector Direction;
	int i;
	MTH_tdxReal fi;
	/* 0x4B9B18 */
	MTH_tdxReal xTemp;

	xTemp = MTH_M_xInv(parameter);

	Direction.xX = MTH_M_xMul(MTH_M_xSub(p_stDirection->xX, p_stPosition->xX), xTemp);
	Direction.xY = MTH_M_xMul(MTH_M_xSub(p_stDirection->xY, p_stPosition->xY), xTemp);
	Direction.xZ = MTH_M_xMul(MTH_M_xSub(p_stDirection->xZ, p_stPosition->xZ), xTemp);

	i = 0;
	fi = AGO_asm_K_c9dc;
	while (fi < parameter)
	{
		AGO_Part *p;

		Pos.xX = MTH_M_xAdd(p_stPosition->xX, MTH_M_xMul(MTH_M_xAdd(fi, AGO_g_Type21Offset), Direction.xX));
		Pos.xY = MTH_M_xAdd(p_stPosition->xY, MTH_M_xMul(MTH_M_xAdd(fi, AGO_g_Type21Offset), Direction.xY));
		Pos.xZ = MTH_M_xAdd(p_stPosition->xZ, MTH_M_xMul(MTH_M_xAdd(fi, AGO_g_Type21Offset), Direction.xZ));

		p = AGO_vCreatePart( AGO_STILL |  AGO_SHADE_TWOFRAMES,  &Pos,  NULL,  MTH_C_2 , MTH_M_xConstFloatToReal(0.7f), MTH_M_xConstFloatToReal(0.7f), hVisualMaterial);
		if (p)
			p->Transparency = MTH_M_xLongToReal(400);
	}

	AGO_g_Type21Offset = AGO_g_Type21Offset - AGO_g_fTimeDifference * AGO_k_fMinus6_12346;
	while (AGO_g_Type21Offset >= MTH_C_ONE)
		AGO_g_Type21Offset -= MTH_C_ONE;
}

/* 0x00462BB0 */
void CreateType65AGO(MTH3D_tdstVector *p_stPosition, MTH3D_tdstVector *p_stDirection,
                     void *hVisualMaterial, MTH_tdxReal parameter)
{
	MTH3D_tdstVector Pos;
	MTH3D_tdstVector Direction;
	MTH_tdxReal scale;

	MTH3D_M_vCopyVector(&Pos, p_stPosition);
	MTH3D_M_vCopyVector(&Direction, p_stDirection);

	/* 0x49c9dc */
	MTH3D_M_vSetVectorElements(&Direction, AGO_asm_K_c9dc, AGO_asm_K_ca2c, AGO_asm_K_c9dc);

	scale = FLOAT_RANGED_RANDOM_RETAIL(MTH_M_xConstFloatToReal(0.100f), MTH_M_xConstFloatToReal(0.150f));

	AGO_vCreatePart(AGO_2DSPIRALE, &Pos, &Direction, MTH_M_xMul(MTH_M_xLongToReal(3), parameter), scale, scale, hVisualMaterial);
}

void CreateType28AGO(MTH3D_tdstVector *p_stPosition, MTH3D_tdstVector *p_stDirection,
                       void *hVisualMaterial, float parameter)
{
	MTH3D_tdstVector Pos;
	MTH3D_tdstVector Direction;
	MTH_tdxReal scale;

	/* 0x4B9B1C */
	AGO_g_ffFrameTime = AGO_fGetTimeInSeconds();
	if ((AGO_g_MustReinitStaticVars != 0) || (AGO_g_Timer28 == AGO_k_f0))
		AGO_g_Timer28 = AGO_g_ffFrameTime;
	if (AGO_g_Timer28 > AGO_g_ffFrameTime)
		return;
	AGO_g_Timer28 = AGO_g_ffFrameTime - AGO_k_fMinus0_05;

	Pos.xX = FLOAT_RANGED_RANDOM_RETAIL(-MTH_C_4, MTH_C_4) - p_stDirection->xX * AGO_k_fMinus8 + p_stPosition->xX;
	Pos.xY = (p_stPosition->xY - p_stDirection->xY * AGO_k_fMinus8) + FLOAT_RANGED_RANDOM_RETAIL(-MTH_C_4, MTH_C_4);
	Pos.xZ = (p_stPosition->xZ - p_stDirection->xZ * AGO_k_fMinus8) + FLOAT_RANGED_RANDOM_RETAIL(-MTH_C_5, -MTH_C_4);
	scale = FLOAT_RANGED_RANDOM(MTH_M_xMul(parameter, MTH_M_xConstFloatToReal(0.20f)), MTH_M_xMul(parameter, MTH_M_xConstFloatToReal(0.25f)));
	Direction.xX = FLOAT_RANGED_RANDOM_RETAIL(MTH_M_xConstFloatToReal(-0.2f), MTH_M_xConstFloatToReal(0.2f));
	Direction.xY = FLOAT_RANGED_RANDOM_RETAIL(MTH_M_xConstFloatToReal(-0.2f), MTH_M_xConstFloatToReal(0.2f));
	{ int rr = rand(); Direction.xZ = ((float)rr + (float)rr) * AGO_k_f3_05185em05 - AGO_k_fMinus2_5; }
	AGO_vCreatePart( AGO_ZELDA |  AGO_MOVINGSINXYUP |  AGO_APPEAR,  &Pos,  &Direction,  MTH_C_Inv2, scale, scale, hVisualMaterial );
}

/* 0x00462E30 */
void CreateType67AGO(MTH3D_tdstVector *p_stPosition, MTH3D_tdstVector *p_stDirection,
                       void *hVisualMaterial, float parameter)
{
	MTH3D_tdstVector Pos;
	MTH3D_tdstVector Direction;
	MTH_tdxReal scale;
	MTH_tdxReal ftmp;

	/* 0x4B9B20 */
	AGO_g_ffFrameTime = AGO_fGetTimeInSeconds();
	if ((AGO_g_MustReinitStaticVars != 0) || (AGO_g_Timer67 == AGO_k_f0))
		AGO_g_Timer67 = AGO_g_ffFrameTime;
	if (AGO_g_Timer67 > AGO_g_ffFrameTime)
		return;
	AGO_g_Timer67 = AGO_g_ffFrameTime - AGO_k_fMinus0_05;

	Pos.xX = p_stPosition->xX - (p_stDirection->xX * AGO_k_fMinus8 + FLOAT_RANGED_RANDOM_RETAIL(-MTH_C_4, MTH_C_4) * AGO_k_fMinus2);
	Pos.xY = (p_stPosition->xY - p_stDirection->xY * AGO_k_fMinus8) - FLOAT_RANGED_RANDOM_RETAIL(-MTH_C_4, MTH_C_4) * AGO_k_fMinus2;
	ftmp = parameter * AGO_k_f0_1;
	Pos.xZ = (p_stPosition->xZ - p_stDirection->xZ * AGO_k_fMinus8) - FLOAT_RANGED_RANDOM_RETAIL(-MTH_C_5, -MTH_C_4) * AGO_k_fMinus2;
	scale = (parameter * AGO_k_f0_15 - ftmp) * (float)rand() * AGO_k_f3_05185em05 + ftmp;
	Direction.xX = FLOAT_RANGED_RANDOM_RETAIL(MTH_M_xConstFloatToReal(-0.2f), MTH_M_xConstFloatToReal(0.2f));
	Direction.xY = FLOAT_RANGED_RANDOM_RETAIL(MTH_M_xConstFloatToReal(-0.2f), MTH_M_xConstFloatToReal(0.2f));
	{ int rr = rand(); Direction.xZ = ((float)rr + (float)rr) * AGO_k_f3_05185em05 - AGO_k_fMinus2_5; }
	AGO_vCreatePart( AGO_ENDASPLOUF | AGO_ZELDA |  AGO_MOVINGSINXYUP |  AGO_APPEAR,  &Pos,  &Direction,  MTH_C_Inv2, scale, scale, hVisualMaterial );
}

#define LIMIT_AGO_TO_EVERY(timelimit) \
{ \
	static MTH_tdxReal NextBubble = MTH_C_ZERO;					 \
	AGO_g_ffFrameTime = AGO_fGetTimeInSeconds();			 \
	if ((AGO_g_MustReinitStaticVars) || (NextBubble == MTH_C_ZERO)) NextBubble = AGO_g_ffFrameTime;	 \
	if (NextBubble > AGO_g_ffFrameTime) return;			 \
	NextBubble = AGO_g_ffFrameTime + timelimit;				 \
}

void CreateType83AGO(MTH3D_tdstVector *p_stPosition, MTH3D_tdstVector *p_stDirection,
                     void *hVisualMaterial, MTH_tdxReal parameter)
{
    MTH3D_tdstVector Pos;
    MTH3D_tdstVector Direction;
    MTH_tdxReal scale;

    LIMIT_AGO_TO_EVERY(MTH_M_xConstFloatToReal(0.05f));

    Pos.xX = p_stPosition->xX - (p_stDirection->xX * AGO_C_xN5 + FLOAT_RANGED_RANDOM(-2.0f, 2.0f) * -2.0f);
    Pos.xY = (p_stPosition->xY - p_stDirection->xY * AGO_C_xN5) - FLOAT_RANGED_RANDOM(-2.0f, 2.0f) * -2.0f;
    Pos.xZ = (p_stPosition->xZ - p_stDirection->xZ * AGO_C_xN5) - MTH_M_xLongToReal(4);
    scale = FLOAT_RANGED_RANDOM(MTH_M_xMul(parameter, MTH_M_xConstFloatToReal(0.20f)), MTH_M_xMul(parameter, MTH_M_xConstFloatToReal(0.25f)));
    Direction.xX = FLOAT_RANGED_RANDOM(MTH_M_xConstFloatToReal(-0.2f), MTH_M_xConstFloatToReal(0.2f));
    Direction.xY = FLOAT_RANGED_RANDOM(MTH_M_xConstFloatToReal(-0.2f), MTH_M_xConstFloatToReal(0.2f));
    Direction.xZ = FLOAT_RANGED_RANDOM(MTH_M_xConstFloatToReal(2.5f), MTH_M_xConstFloatToReal(4.5f));

    AGO_vCreatePart(AGO_ZELDA | AGO_MOVINGSINXY | AGO_APPEAR, &Pos, &Direction, MTH_C_Inv2, scale, scale, hVisualMaterial);
}

/* 0x00463210 */
void CreateType104AGO(MTH3D_tdstVector *p_stPosition, MTH3D_tdstVector *p_stDirection,
                      void *hVisualMaterial, MTH_tdxReal parameter)
{
	MTH3D_tdstVector Pos;
	MTH3D_tdstVector Direction;
	int i;

	MTH3D_M_vCopyVector(&Pos, p_stPosition);
	MTH3D_M_vCopyVector(&Direction, p_stDirection);

	for (i = 0 ; i < 10 ; i++)
	{
		Direction.xX = FLOAT_RANGED_RANDOM_RETAIL(MTH_C_MinusONE, MTH_C_ONE);
		Direction.xY = FLOAT_RANGED_RANDOM_RETAIL(MTH_C_MinusONE, MTH_C_ONE);
		Direction.xZ = MTH_C_ZERO;
		AGO_vCreatePart( AGO_FALLING | AGO_TIMELIMITED,  &Pos,  &Direction,  FLOAT_RANGED_RANDOM_RETAIL(MTH_C_ONE, MTH_C_3), MTH_C_ZERO, MTH_C_ZERO, hVisualMaterial );
	}
}

int __cdecl AGO_CompareVector(MTH3D_tdstVector *v1, MTH3D_tdstVector *v2)
{

	int i;
	for (i = 0; i < 3; i++)
		if (((float *)v1)[i] != ((float *)v2)[i])
			return 0;
	return 1;
}

/* 0x00463320 */
void AGO_vTransformHandVector(MTH3D_tdstVector *Position, MTH3D_tdstVector *Direction)
{

    float other[3];
    Position->xX = Position->xX - Direction->xX * -0.129999995f;
    Position->xY = Position->xY - Direction->xY * -0.129999995f;
    Position->xZ = (Position->xZ - Direction->xZ * -0.100000001f) - -0.200000003f;

    other[0] = Direction->xY;
    other[1] = -Direction->xX;
    other[2] = 0.0f;

    Position->xX = Position->xX - other[0] * -0.300000012f;
    Position->xY = Position->xY - other[1] * -0.300000012f;
}

void AGO_vAddParticleContd(unsigned long PartType, MTH3D_tdstVector *pPos, MTH3D_tdstVector *pDir, void *hVisualMaterial, float parameter);

void AGO_vAddParticle(unsigned long PartType,
					  MTH3D_tdstVector *p_stPosition,
					  MTH3D_tdstVector *p_stDirection,
					  void *hVisualMaterial,
					  MTH_tdxReal parameter)
{
	MTH3D_tdstVector Pos;
	MTH3D_tdstVector Direction;
	int i;
	MTH_tdxReal scale;

	AGO_g_AGO_AnimTimeScale = MTH_C_ONE;

	MTH3D_M_vCopyVector(&Pos, p_stPosition);
	MTH3D_M_vCopyVector(&Direction, p_stDirection);

	switch(PartType)
	{

		case 1:
			parameter += MTH_C_ONE;
			AGO_vCreatePart( AGO_STILL |  AGO_TIMELIMITED,  &Pos,  NULL,  MTH_C_2 , parameter, parameter, hVisualMaterial );
		break;

		case 2:
			parameter += MTH_C_ONE;
			AGO_vCreatePart( AGO_STILL |  AGO_TIMELIMITED,  &Pos,  NULL,  Direction.xX , parameter, parameter, hVisualMaterial );
		break;

		case 5:
			CreateType5AGO(p_stPosition, p_stDirection, hVisualMaterial, parameter);
		break;

		case 7:
			AGO_g_PloufMaterial = (int)hVisualMaterial;

			{
				AGO_g_ffFrameTime = AGO_fGetTimeInSeconds();
				if (AGO_g_Timer7 == MTH_C_ZERO)
					AGO_g_Timer7 = AGO_g_ffFrameTime;
				if (AGO_g_Timer7 > AGO_g_ffFrameTime)
					return;
				AGO_g_Timer7 = AGO_g_ffFrameTime + MTH_M_xConstFloatToReal(0.2f);
			}

			if (Direction.xX > MTH_M_xLongToReal(300))
				Direction.xX = MTH_M_xLongToReal(250);

			if (AGO_g_SplashFlag)
			{
				AGO_vCreatePart( AGO_RAYMAN_SMALL_PLOUF ,  &Pos,  &Direction,  Direction.xX, Direction.xZ, Direction.xZ, hVisualMaterial);
			}
			else
			{
				AGO_vCreatePart( AGO_RAYMAN_PLOUF ,  &Pos,  &Direction,  Direction.xX, Direction.xZ, Direction.xZ, hVisualMaterial);
			}
		break;

		case 8:
			CreateType8AGO(p_stPosition, p_stDirection, hVisualMaterial, parameter);
		break;

		case 9:
		{
			scale = FLOAT_RANGED_RANDOM(MTH_C_Inv2, MTH_M_xConstFloatToReal(0.8f));
			AGO_vCreatePart( AGO_STILL | AGO_SHADE_FASTEST | AGO_UNSCALING_FAST,  &Pos,  NULL,  parameter, scale, scale, hVisualMaterial );
			for (i = 8 ; i ; i--)
			{
				Direction.xX = FLOAT_RANGED_RANDOM(-MTH_C_2, MTH_C_2);
				Direction.xY = FLOAT_RANGED_RANDOM(-MTH_C_2, MTH_C_2);
				Direction.xZ = FLOAT_RANGED_RANDOM(MTH_M_xConstFloatToReal(-0.2f), MTH_M_xLongToReal(6));
				scale = FLOAT_RANGED_RANDOM(MTH_M_xConstFloatToReal(0.19f), MTH_M_xConstFloatToReal(0.35f));
				AGO_vCreatePart( AGO_FALLING | AGO_SHADE_FAST,  &Pos,  &Direction,  parameter, scale, scale, hVisualMaterial );
			}
		}
		break;

		case 10:
		{

			AGO_g_ffFrameTime = AGO_fGetTimeInSeconds();
			if ((AGO_g_MustReinitStaticVars) || (AGO_g_Timer10 == MTH_C_ZERO))
				AGO_g_Timer10 = AGO_g_ffFrameTime;
			if (AGO_g_Timer10 > AGO_g_ffFrameTime)
				return;
			AGO_g_Timer10 = AGO_g_ffFrameTime + MTH_M_xConstFloatToReal(0.1f);
			MTH3D_M_vSetVectorElements(&Direction, MTH_C_ZERO, MTH_C_ZERO, MTH_C_ZERO);
			scale = MTH_M_xConstFloatToReal(0.40f);
			AGO_vCreatePart( AGO_FALLING_SLOW | AGO_UNSCALING_SLOW | AGO_SHADE_SLOW ,  &Pos,  &Direction, parameter, scale, scale, hVisualMaterial );
		}
		break;

		case 11:
			{
				MTH_tdxReal fRadius = p_stDirection->xX;
				MTH_tdxReal cos1,sin1;
				int Number = MTH_M_xRealToLong(parameter);

				for (i = 0 ; i < Number ; i++)
				{
					MTH_tdxReal angle = MTH_M_xDiv(MTH_M_xMul(MTH_M_xLongToReal(i), MTH_M_xConstFloatToReal(6.28f)), MTH_M_xLongToReal(Number));

					cos1 = (float)cos(angle);
					sin1 = (float)sin(angle);
					AGO_g_AGO_AnimTimeScale=FLOAT_RANGED_RANDOM(MTH_M_xConstFloatToReal(0.2f), MTH_C_3);
					Pos.xX = p_stPosition->xX + MTH_M_xMul(fRadius, cos1);
					Pos.xY = p_stPosition->xY + MTH_M_xMul(fRadius, sin1);
					Pos.xZ = p_stPosition->xZ;
					Direction.xX = MTH_M_xMul(fRadius, cos1);
					Direction.xY = MTH_M_xMul(fRadius, sin1);
					Direction.xZ = MTH_M_xConstFloatToReal(1.53f);
					scale = FLOAT_RANGED_RANDOM(MTH_M_xConstFloatToReal(0.2f), MTH_M_xConstFloatToReal(1.2f));
					AGO_vCreatePart( AGO_APPARITION_FUMEE ,  &Pos,  &Direction,  parameter, scale, scale, hVisualMaterial );
				}
			}
		break;

		case 15:
			{
				CreateType15AGO(p_stPosition, p_stDirection, hVisualMaterial, parameter);
			}
		break;

		case 16:
			CreateType16AGO(p_stPosition, p_stDirection, hVisualMaterial, parameter);
		break;

		case 19:
		{
			MTH_tdxReal fNorm;

			fNorm = MTH3D_M_xSqrVector(p_stDirection);
			if (fNorm <= MTH_M_xConstFloatToReal(0.001f))
				return;
			MTH3D_M_vNormalizeVector(p_stDirection, p_stDirection);

			MTH3D_M_vCopyVector(&Direction, p_stDirection);
			scale = MTH_C_Inv2;
			AGO_vCreatePart( AGO_STILL | AGO_ORTHOPLAN | AGO_SCALING_SLOW | AGO_SHADE_MID ,  &Pos,  &Direction, parameter, scale, scale, hVisualMaterial );
		}
		break;

		case 20:
		{
			MTH_tdxReal fNorm;

			fNorm = MTH3D_M_xSqrVector(p_stDirection);
			if (fNorm <= MTH_M_xConstFloatToReal(0.001f))
				return;
			MTH3D_M_vNormalizeVector(p_stDirection, p_stDirection);

			MTH3D_M_vCopyVector(&Direction, p_stDirection);
			scale = MTH_C_Inv2;
			AGO_vCreatePart( AGO_STILL | AGO_ORTHOPLAN | AGO_SCALING_FASTEST | AGO_SHADE_MEGAFAST ,  &Pos,  &Direction, parameter, scale, scale, hVisualMaterial );
		}
		break;

		case 21:
		{
				CreateType21AGO(p_stPosition, p_stDirection, hVisualMaterial, parameter);
		}
		break;

		case 23:
		{
			int j;

			AGO_vTransformHandVector(&Pos, &Direction);
			MTH3D_M_vCopyVector(&Direction, p_stPosition);

			scale = FLOAT_RANGED_RANDOM(MTH_M_xConstFloatToReal(0.10f), MTH_M_xConstFloatToReal(0.15f));

			for (j = 0; j < 3; j++)
				((float *)&Pos)[j] = ((float *)&Direction)[j] + FLOAT_RANGED_RANDOM(MTH_C_MinusONE, MTH_C_ONE);

			AGO_vCreatePart( AGO_FASEGOINGTO ,  &Pos,  &Direction,  parameter , scale, scale, hVisualMaterial);
		}
		break;

		case 26:
			{

				AGO_g_ffFrameTime = AGO_fGetTimeInSeconds();
				if ((AGO_g_MustReinitStaticVars) || (AGO_g_Timer1A == MTH_C_ZERO))
					AGO_g_Timer1A = AGO_g_ffFrameTime;
				if (AGO_g_Timer1A > AGO_g_ffFrameTime)
					return;
				AGO_g_Timer1A = AGO_g_ffFrameTime + FLOAT_RANGED_RANDOM(MTH_C_ZERO, MTH_C_Inv2);

				if (Pos.xZ > parameter)
					return;

				scale = FLOAT_RANGED_RANDOM(MTH_M_xConstFloatToReal(0.02f), MTH_M_xConstFloatToReal(0.1f));
				Direction.xX = FLOAT_RANGED_RANDOM(MTH_C_MinusONE, MTH_C_ONE);
				Direction.xY = FLOAT_RANGED_RANDOM(MTH_C_MinusONE, MTH_C_ONE);
				Direction.xZ = FLOAT_RANGED_RANDOM(MTH_C_ONE, MTH_C_4);
				AGO_vCreatePart( AGO_MOVINGSINXY |  AGO_ZLIMITED | AGO_SCALING_MEGASLOW,  &Pos,  &Direction,  parameter , scale, scale, hVisualMaterial );

				Pos.xY += MTH_M_xConstFloatToReal(0.2f);
				Pos.xZ += MTH_M_xConstFloatToReal(0.2f);
				if (Pos.xZ < parameter)
					AGO_vCreatePart( AGO_MOVINGSINXY |  AGO_ZLIMITED | AGO_SCALING_MEGASLOW,  &Pos,  &Direction,  parameter , scale, scale, hVisualMaterial );
				Pos.xY -= MTH_C_Inv2;
				Pos.xZ += MTH_M_xConstFloatToReal(0.1f);
				if (Pos.xZ < parameter)
					AGO_vCreatePart( AGO_MOVINGSINXY |  AGO_ZLIMITED | AGO_SCALING_MEGASLOW,  &Pos,  &Direction,  parameter , scale, scale, hVisualMaterial );

			}
		break;

		case 27:
			{

				AGO_g_ffFrameTime = AGO_fGetTimeInSeconds();
				if ((AGO_g_MustReinitStaticVars) || (AGO_g_Timer1B == MTH_C_ZERO))
					AGO_g_Timer1B = AGO_g_ffFrameTime;
				if (AGO_g_Timer1B > AGO_g_ffFrameTime)
					return;
				AGO_g_Timer1B = AGO_g_ffFrameTime + FLOAT_RANGED_RANDOM(MTH_C_ZERO, MTH_C_Inv2);

				if (parameter > MTH_M_xLongToReal(7))
					parameter = MTH_M_xLongToReal(7);

				scale = FLOAT_RANGED_RANDOM(MTH_M_xConstFloatToReal(0.02f), MTH_M_xConstFloatToReal(0.1f));
				Direction.xX = FLOAT_RANGED_RANDOM(MTH_C_MinusONE, MTH_C_ONE);
				Direction.xY = FLOAT_RANGED_RANDOM(MTH_C_MinusONE, MTH_C_ONE);
				Direction.xZ = FLOAT_RANGED_RANDOM(MTH_C_2, MTH_C_4);
				AGO_vCreatePart( AGO_MOVINGSINXY |  AGO_TIMELIMITED | AGO_SCALING_MEGASLOW,  &Pos,  &Direction,  parameter , scale, scale, hVisualMaterial );

				Pos.xY += MTH_M_xConstFloatToReal(0.2f);
				Pos.xZ += MTH_M_xConstFloatToReal(0.2f);
				AGO_vCreatePart( AGO_MOVINGSINXY |  AGO_TIMELIMITED | AGO_SCALING_MEGASLOW,  &Pos,  &Direction,  parameter , scale, scale, hVisualMaterial );
				Pos.xY -= MTH_C_Inv2;
				Pos.xZ += MTH_M_xConstFloatToReal(0.1f);
				AGO_vCreatePart( AGO_MOVINGSINXY |  AGO_TIMELIMITED | AGO_SCALING_MEGASLOW,  &Pos,  &Direction,  parameter , scale, scale, hVisualMaterial );

			}
		break;

		case 28:
			{
				CreateType28AGO(p_stPosition, p_stDirection, hVisualMaterial, parameter);
			}
		break;

		case 29:
			{
				int j;

				for (j = 0; j < 3; j++)
				{
					((float *)&Pos)[j] = ((float *)p_stPosition)[j] + MTH_M_xMul(MTH_M_xLongToReal(8), ((float *)p_stDirection)[j]) + FLOAT_RANGED_RANDOM(-MTH_C_4, MTH_C_4);
					((float *)&Direction)[j] = FLOAT_RANGED_RANDOM(-MTH_C_Inv2, MTH_C_Inv2);
				}

				scale = FLOAT_RANGED_RANDOM(MTH_M_xConstFloatToReal(0.12f), MTH_M_xConstFloatToReal(0.19f));
				AGO_vCreatePart( AGO_ZELDA |  AGO_MOVINGSINXY |  AGO_APPEAR,  &Pos,  &Direction,  MTH_C_Inv2 , scale, scale, hVisualMaterial );
			}
		break;

		case 30:
		{
			scale = FLOAT_RANGED_RANDOM(MTH_C_Inv2, MTH_M_xConstFloatToReal(0.8f));
			AGO_vCreatePart( AGO_STILL | AGO_SHADE_FASTEST | AGO_UNSCALING_FAST,  &Pos,  NULL,  parameter, scale, scale, hVisualMaterial );
			for (i = MTH_M_xRealToLong(parameter) ; i ; i--)
			{
				Direction.xX = FLOAT_RANGED_RANDOM(-MTH_C_2, MTH_C_2);
				Direction.xY = FLOAT_RANGED_RANDOM(-MTH_C_2, MTH_C_2);
				Direction.xZ = FLOAT_RANGED_RANDOM(MTH_M_xConstFloatToReal(-0.2f), MTH_C_3);
				scale = FLOAT_RANGED_RANDOM(MTH_M_xConstFloatToReal(0.10f), MTH_M_xConstFloatToReal(0.15f));
				AGO_vCreatePart( AGO_FALLING | AGO_SHADE_FAST,  &Pos,  &Direction,  parameter, scale, scale, hVisualMaterial );
			}
		}
		break;

		case 31:
		{

			AGO_g_ffFrameTime = AGO_fGetTimeInSeconds();
			if ((AGO_g_MustReinitStaticVars) || (AGO_g_Timer1F == MTH_C_ZERO))
				AGO_g_Timer1F = AGO_g_ffFrameTime;
			if (AGO_g_Timer1F > AGO_g_ffFrameTime)
				return;
			AGO_g_Timer1F = AGO_g_ffFrameTime + MTH_C_Inv2;

			Direction.xX = FLOAT_RANGED_RANDOM(-MTH_C_2, MTH_C_2);
			Direction.xY = FLOAT_RANGED_RANDOM(-MTH_C_2, MTH_C_2);
			Direction.xZ = FLOAT_RANGED_RANDOM(MTH_C_ZERO, MTH_M_xLongToReal(6));
			scale = FLOAT_RANGED_RANDOM(MTH_M_xConstFloatToReal(0.35f), MTH_M_xConstFloatToReal(0.65f));
			AGO_vCreatePart( AGO_SPIRALE | AGO_SHADE_SLOW,  &Pos,  &Direction,  parameter, scale, scale, hVisualMaterial );
		}
		break;

		case 33:
		{
			scale = FLOAT_RANGED_RANDOM(-parameter,parameter);
			Pos.xX -= MTH_M_xMul(scale, p_stDirection->xY);
			Pos.xY += MTH_M_xMul(scale, p_stDirection->xX);
			AGO_vCreatePart( AGO_STILL |  AGO_SHADE_FAST,  &Pos,  NULL,  MTH_C_2, MTH_C_2, MTH_C_2, hVisualMaterial );
		}
		break;

		case 35:
		{
			CreateType35AGO(p_stPosition, p_stDirection, hVisualMaterial, parameter);
		}
		break;

		case 39:
			{
				scale = MTH_M_xMul(p_stDirection->xY, MTH_C_Inv2);
				AGO_vCreatePart( AGO_STILL | AGO_SCALING_SLOW | AGO_BASEDSCALE | AGO_SHADE_SLOW,  &Pos,  NULL,  MTH_M_xConstFloatToReal(0.01f) , scale, scale, hVisualMaterial );
			}
			break;

		case 42:
			{
				AGO_g_AGO_AnimTimeScale = FLOAT_RANGED_RANDOM(MTH_M_xConstFloatToReal(0.7f), MTH_M_xConstFloatToReal(1.8f));
				scale = FLOAT_RANGED_RANDOM(MTH_M_xConstFloatToReal(0.2f), MTH_M_xConstFloatToReal(0.8f));
				Pos.xX += FLOAT_RANGED_RANDOM(MTH_C_MinusONE, MTH_C_ONE);
				Pos.xY += FLOAT_RANGED_RANDOM(MTH_C_MinusONE, MTH_C_ONE);
				Direction.xX = FLOAT_RANGED_RANDOM(MTH_M_xConstFloatToReal(-0.2f), MTH_M_xConstFloatToReal(0.2f));
				Direction.xY = FLOAT_RANGED_RANDOM(MTH_M_xConstFloatToReal(-0.2f), MTH_M_xConstFloatToReal(0.2f));
				Direction.xZ = FLOAT_RANGED_RANDOM(MTH_C_ONE, MTH_C_2);
				AGO_vCreatePart( AGO_MOVING | AGO_SCALING_SLOW,  &Pos,  &Direction,  MTH_M_xConstFloatToReal(0.01f), scale, scale, hVisualMaterial );
			}
			break;

		case 45:
			{
				for (i = 9 ; i ; i--)
				{
					Direction.xX = FLOAT_RANGED_RANDOM(-MTH_M_xMul(MTH_C_2, parameter), MTH_M_xMul(MTH_C_2, parameter));
					Direction.xY = FLOAT_RANGED_RANDOM(-MTH_M_xMul(MTH_C_2, parameter), MTH_M_xMul(MTH_C_2, parameter));
					Direction.xZ = FLOAT_RANGED_RANDOM(-MTH_M_xMul(MTH_M_xConstFloatToReal(0.2f), parameter), MTH_M_xMul(MTH_M_xLongToReal(6), parameter));
					scale = FLOAT_RANGED_RANDOM(MTH_M_xMul(MTH_M_xConstFloatToReal(0.8f), parameter), MTH_M_xMul(MTH_M_xConstFloatToReal(0.16f), parameter));
					AGO_vCreatePart( AGO_FALLING | AGO_SHADE_MID  | AGO_UNSCALING_FAST ,  &Pos,  &Direction,  parameter, scale, scale, hVisualMaterial );
				}
			}
			break;

		case 46:
			{
				AGO_g_AGO_AnimTimeScale = MTH_M_xConstFloatToReal(0.7f);
				AGO_vCreatePart( AGO_STILL | AGO_SCALING_SLOW,  &Pos,  NULL,  MTH_M_xConstFloatToReal(0.01f) , MTH_M_xMul(MTH_C_Inv2, parameter), MTH_M_xMul(MTH_C_Inv2, parameter), hVisualMaterial );
				Pos.xX += FLOAT_RANGED_RANDOM(-MTH_M_xMul(MTH_C_2, parameter), MTH_M_xMul(MTH_C_2, parameter));
				Pos.xY += FLOAT_RANGED_RANDOM(-MTH_M_xMul(MTH_C_2, parameter), MTH_M_xMul(MTH_C_2, parameter));
				AGO_g_AGO_AnimTimeScale = MTH_M_xConstFloatToReal(1.25f);
				AGO_vCreatePart( AGO_STILL | AGO_SCALING_SLOW,  &Pos,  NULL,  MTH_M_xConstFloatToReal(0.01f) , MTH_M_xMul(MTH_C_Inv2, parameter), MTH_M_xMul(MTH_C_Inv2, parameter), hVisualMaterial );
			}
			break;

		case 47:
			{
				AGO_vCreatePart( AGO_STILL | AGO_SCALING_FASTEST | AGO_SHADE_TWOFRAMES,  &Pos,  NULL,  MTH_M_xConstFloatToReal(0.01f), parameter, parameter, hVisualMaterial );
			}
			break;

		case 48:
			{
				if (parameter >= MTH_M_xLongToReal(9))
				{
					AGO_vCreatePart( AGO_STILL | AGO_XYPLAN | AGO_SCALING_FASTEST |  AGO_SHADE_MID,  &Pos,  NULL,  MTH_M_xConstFloatToReal(0.1f) , MTH_M_xMul(parameter, MTH_M_xConstFloatToReal(0.7f)), MTH_M_xMul(parameter, MTH_M_xConstFloatToReal(0.7f)), hVisualMaterial );
				}
				else
				{
					AGO_vCreatePart( AGO_STILL | AGO_XYPLAN | AGO_SCALING_FASTEST |  AGO_SHADE_FAST,  &Pos,  NULL,  MTH_M_xConstFloatToReal(0.1f) , MTH_M_xMul(MTH_M_xConstFloatToReal(0.05f), parameter), MTH_M_xMul(MTH_M_xConstFloatToReal(0.05f), parameter), hVisualMaterial );
				}
			}
			break;

		case 49:
			{
				AGO_Part *p;
				p = AGO_vCreatePart( AGO_STILL | AGO_SCALING_XTOY | AGO_SHADE_MEGAFAST,  &Pos,  NULL,  MTH_M_xLongToReal(255), MTH_M_xMul(MTH_M_xConstFloatToReal(0.7f), parameter), MTH_M_xMul(MTH_M_xConstFloatToReal(0.7f), parameter), hVisualMaterial );
				if (p)
					p->Transparency = MTH_M_xLongToReal(300);
			}
			break;

		case 50:
			{
				MTH3D_M_vCopyVector(&AGO_g_ExtraParameterVec1, &Pos);
				MTH3D_M_vCopyVector(&AGO_g_ExtraParameterVec2, &Direction);
				AGO_bb_Param = parameter;
				AGO_bb_Mat = (unsigned long)hVisualMaterial;
			}
			break;

		case 51:
			{
				AGO_Part *p;
				p = AGO_vCreatePart(AGO_STILL | AGO_REMANANCE2 | AGO_SHADE_MEGAFAST,  &Pos,  NULL,  MTH_M_xConstFloatToReal(0.1f) , MTH_C_ONE, MTH_C_ONE, hVisualMaterial );
				if (p)
					MTH3D_M_vCopyVector(&p->vInitialPosition, &p->vDirection);
			}
			break;

		case 52:
			{
				CreateType52AGO(p_stPosition, p_stDirection, hVisualMaterial, parameter);
			}
		break;

		case 55:
			{
				int j;
				scale = Direction.xX;
				i = MTH_M_xRealToLong(parameter) + 1;
				for ( ; i ; i--)
				{

					for (j = 0; j < 3; j++)
						((float *)&Direction)[j] = FLOAT_RANGED_RANDOM(-parameter, parameter);
					AGO_vCreatePart( AGO_MOVING |  AGO_SHADE_FAST,  &Pos, &Direction,  MTH_C_ONE , scale, scale, hVisualMaterial );
				}
			}
		break;

		case 60:
			{
				CreateType60AGO(p_stPosition, p_stDirection, hVisualMaterial, parameter);
			}
		break;

		case 62:
			{
				CreateType62AGO(p_stPosition, p_stDirection, hVisualMaterial, parameter);
			}
		break;

		case 65:
			{
				CreateType65AGO(p_stPosition, p_stDirection, hVisualMaterial, parameter);
			}
		break;

		case 67:
			{
				CreateType67AGO(p_stPosition, p_stDirection, hVisualMaterial, parameter);
			}
		break;

		default:
			AGO_vAddParticleContd(PartType, p_stPosition, p_stDirection, hVisualMaterial, parameter);
			break;
	}
}

void AGO_vAddParticleContd(unsigned long PartType, MTH3D_tdstVector *pPos,
                           MTH3D_tdstVector *pDir, void *hVisualMaterial, float parameter)
{
    float             local_48[18];
    float             dirx;
    AGO_Part         *p;
    MTH3D_tdstVector *pos = (MTH3D_tdstVector *)&local_48[3];
    MTH3D_tdstVector *dir = (MTH3D_tdstVector *)&local_48[0];

    AGO_g_CurrentAnimTime = 1.0f;
    local_48[3] = pPos->xX; local_48[4] = pPos->xY; local_48[5] = pPos->xZ;
    dirx        = pDir->xX;
    local_48[0] = dirx; local_48[1] = pDir->xY; local_48[2] = pDir->xZ;

    switch (PartType)
    {
    case 0x44:
    {
        int k;
        for (k = 6; k != 0; k--)
        {
            int rr; float f;
            rr = rand(); local_48[0] = ((float)rr + (float)rr) * AGO_k_f3_05185em05 - AGO_k_f1;
            rr = rand(); local_48[1] = ((float)rr + (float)rr) * AGO_k_f3_05185em05 - AGO_k_f1;
            local_48[2] = (float)rand() * AGO_k_f4_2 * AGO_k_f3_05185em05 - AGO_k_f0_2;
            f = (float)rand() * AGO_k_f0_07 * AGO_k_f3_05185em05 - AGO_k_fMinus0_05;
            AGO_call_vCreatePart(0x3005, pos, dir, parameter, f, f, hVisualMaterial);
        }
        return;
    }
    case 69:
    {
        int j;

        AGO_vTransformHandVector(pos, dir);

        for (j = 0; j < 3; j++)
        {
            local_48[0] = local_48[3] + ((float)rand() * 3.0f * AGO_k_f3_05185em05 - 1.5f);
            local_48[1] = local_48[4] + ((float)rand() * 3.0f * AGO_k_f3_05185em05 - 1.5f);
            local_48[2] = local_48[5] + ((float)rand() * 3.0f * AGO_k_f3_05185em05 - 1.5f);
            p = AGO_call_vCreatePart(AGO_STILL | AGO_REMANANCE2 | AGO_SHADE_TWOFRAMES, pos, NULL,
                                     0.1f, 5.0f, 5.0f, hVisualMaterial);
            if (p)
                MTH3D_M_vCopyVector(&p->vInitialPosition, dir);
        }
        return;
    }
    case 71:
    {
        MTH_tdxReal scale;

        AGO_vTransformHandVector(pos, dir);

        rand();
        scale = parameter * 0.3f;
        p = AGO_call_vCreatePart(AGO_STILL | AGO_SHADE_TWOFRAMES, pos, NULL, 0.1f, scale, scale,
                                 hVisualMaterial);
        if (p)
            p->Transparency = 500.0f;
        return;
    }
    case 72:
        CreateType72AGO(pPos, pDir, hVisualMaterial, parameter);
        return;
    case 83:
        CreateType83AGO(pPos, pDir, hVisualMaterial, parameter);
        return;
    case 104:
        CreateType104AGO(pPos, pDir, hVisualMaterial, parameter);
        return;
    case 120:
        AddGauge(&local_48[3], &local_48[0], parameter, AGO_g_ExtraMaterial, hVisualMaterial);
        return;
    case 125:
        AddIcone(&local_48[3], &local_48[0], parameter, hVisualMaterial);
        return;
    case 158:
        CreateType158AGO(pPos, pDir, hVisualMaterial, parameter);
        return;
    case 170:
        AGO_vAddLogoPart(parameter, hVisualMaterial);
        return;
    case 171:
        AGO_vActivateLogoDisplay(pPos, pDir, parameter);
        return;
    case 0x63:
    {
        float f = (float)rand() * AGO_k_f0_3_a * AGO_k_f3_05185em05 - AGO_k_fMinus0_5;
        int count, k;
        AGO_call_vCreatePart(0x4500, pos, NULL, parameter, f, f, hVisualMaterial);
        count = (int)parameter;
        for (k = count; k != 0; k--)
        {
            local_48[0] = (float)rand() * AGO_k_f4 * AGO_k_f3_05185em05 - AGO_k_f2;
            local_48[1] = (float)rand() * AGO_k_f4 * AGO_k_f3_05185em05 - AGO_k_f2;
            local_48[2] = (float)rand() * AGO_k_f6_2 * AGO_k_f3_05185em05 - AGO_k_f0_2;
            f = (float)rand() * AGO_k_f0_16 * AGO_k_f3_05185em05 - AGO_k_fMinus0_19;
            AGO_call_vCreatePart(0x3001, pos, dir, parameter, f, f, hVisualMaterial);
        }
        return;
    }
    case 0x6e:
        p = AGO_call_vCreatePart(0x11, pos, dir, parameter, 1.0f, 1.0f, hVisualMaterial);
        /* 0x00464d3b */
        if (p) p->Transparency = parameter;
        return;
    case 0x69:
    {
        int n = (int)parameter + 1, j;
        if (n != 0)
            do {
                for (j = 0; j < 3; j++)
                    local_48[j] = (float)rand() * (parameter + parameter) * AGO_k_f3_05185em05 - parameter;
                AGO_call_vCreatePart(0x3e10, pos, dir, 1.0f, dirx, dirx, hVisualMaterial);
                n--;
            } while (n != 0);
        return;
    }
    case 0x9b:
    {
        int n = (int)parameter + 1, j;
        if (n != 0)
            do {
                for (j = 0; j < 3; j++)
                    local_48[j] = (float)rand() * (parameter + parameter) * AGO_k_f3_05185em05 - parameter;
                AGO_call_vCreatePart(0x5003, pos, dir, 1.0f, dirx, dirx, hVisualMaterial);
                n--;
            } while (n != 0);
        return;
    }
    case 0x9c:
        *(unsigned long *)&AGO_g_CurrentAnimTime = 0x3eaaaaaa;
        AGO_call_vCreatePart(0x10000, pos, NULL, 2.0f,
                             parameter - AGO_k_fMinus1, parameter - AGO_k_fMinus1,
                             hVisualMaterial);
        return;
    case 0x9d:
        AGO_call_vCreatePart(0x10006000, pos, NULL, 2.0f, parameter, parameter, hVisualMaterial);
        return;
    case 0x97:
    {
        float f = (float)rand() * AGO_k_f0_4_b * AGO_k_f3_05185em05 - AGO_k_fMinus0_1;
        p = AGO_call_vCreatePart(0x1a003, pos, dir, parameter, f, f, hVisualMaterial);
        if (p) p->Transparency = 0.01f;
        return;
    }
    case 0xa7:
        AGO_g_SplashFlag = (char)((AGO_k_f0_01 < parameter) ? 1 : 0);
        return;

    case 0x84: /* 0x49CBB4 */

    {
        float V  = AGO_k_f0_57735;
        float dt = AGO_g_fTimeDifference;
        float cX = (float)cos((double)AGO_g_Angle132X);
        float cY = (float)cos((double)AGO_g_Angle132Y);
        float cZ = (float)cos((double)AGO_g_Angle132Z);
        float sX = (float)sin((double)AGO_g_Angle132X);
        float sY = (float)sin((double)AGO_g_Angle132Y);
        float sZ = (float)sin((double)AGO_g_Angle132Z);
        float scale, invn, zTmp, xTmp;
        scale = (parameter * AGO_k_f0_8 - parameter * AGO_k_f0_6)
                * (float)rand() * AGO_k_f3_05185em05 + parameter * AGO_k_f0_6;
        local_48[0] = cX * V - sY * V;
        local_48[1] = (cZ + cY + sX) * V - sX * V;
        local_48[2] = (sZ * V - (cY * V - cX * V)) - cY * V;
        AGO_g_Angle132X = AGO_g_Angle132X + dt;
        AGO_g_Angle132Y = AGO_g_Angle132Y - dt * AGO_k_fMinus6_9;
        AGO_g_Angle132Z = AGO_g_Angle132Z - dt * AGO_k_fMinus3_1564;
        zTmp = local_48[2];
        invn = AGO_k_f1 / (float)sqrt((double)(local_48[2] * local_48[2]
                + local_48[1] * local_48[1] + local_48[0] * local_48[0]));
        local_48[0] = invn * local_48[0];
        local_48[1] = invn * local_48[1];
        local_48[2] = invn * zTmp;
        AGO_call_vCreatePart(0x20004000, pos, dir, 0.1f, scale, scale, hVisualMaterial);
        xTmp = local_48[0];
        local_48[3] = local_48[3] + pDir->xX;
        local_48[4] = local_48[4] + pDir->xY;
        local_48[5] = local_48[5] + pDir->xZ;
        local_48[0] = local_48[2];
        local_48[2] = xTmp;
        AGO_call_vCreatePart(0x20004000, pos, dir, 0.1f, scale, scale, hVisualMaterial);
        return;
    }
    case 0xa3:

    {
        unsigned long n = AGO_g_ulNumberOfAgos;
        unsigned long k;
        if (n != 0)
        {
            for (k = 0; k < n; k++)
            {
                AGO_Part *p2 = &AGO_g_AGO[k];
                if ((p2->ulType == AGO_STILL) | AGO_REMANANCE3)
                {
                    if (AGO_call_CompareVector(&p2->vInitialPosition, pos) &&
                        AGO_call_CompareVector(&p2->vPosition, dir))
                        return;
                    if (AGO_call_CompareVector(&p2->vInitialPosition, dir) &&
                        AGO_call_CompareVector(&p2->vPosition, pos))
                        return;
                }
                n = AGO_g_ulNumberOfAgos;
            }
        }
        p = &AGO_g_AGO[n];
        AGO_call_vCreatePart(0x60000000, pos, NULL, 0.1f, 1.0f, 1.0f, hVisualMaterial);
        p->vInitialPosition.xX = local_48[0];
        p->vInitialPosition.xY = local_48[1];
        p->vInitialPosition.xZ = local_48[2];
        AGO_g_bUsingMapMondeAGOs = 1;
        return;
    }
    case 0xaf:

        if (parameter != AGO_k_f0)
        {
            float fHalf = parameter * AGO_k_f0_5;
            float fNorm = AGO_k_f0;
            int   j;
            for (j = 0; j < 3; j++)
            {
                float diff   = local_48[j] - local_48[j + 3];
                float oldDir = local_48[j];
                local_48[j]        = diff;
                local_48[0xf + j]  = oldDir;
                fNorm = diff * diff + fNorm;
            }
            if (fNorm != AGO_k_f0)
            {
                float invn = AGO_k_f1 / (float)sqrt((double)fNorm);
                int   iter;
                local_48[0] = invn * local_48[0];
                local_48[1] = invn * local_48[1];
                local_48[2] = invn * local_48[2];
                AGO_Define_OrthogonalVectors((MTH3D_tdstVector *)&local_48[0],
                                             (MTH3D_tdstVector *)&local_48[0xc],
                                             (MTH3D_tdstVector *)&local_48[9]);
                for (iter = 12; iter != 0; iter--)
                {
                    float fN3 = 0.0f;
                    float scale;
                    int   m;
                    for (m = 0; m < 3; m++)
                    {
                        float a  = (float)rand() * AGO_k_f16 * AGO_k_f3_05185em05 - AGO_k_f8;
                        float v2 = local_48[9 + m];
                        float b  = (float)rand() * AGO_k_f16 * AGO_k_f3_05185em05 - AGO_k_f8;
                        float v3 = b * local_48[0xc + m] + a * v2;
                        fN3 = v3 * v3 + fN3;
                        local_48[6 + m] = v3;
                    }
                    if (fN3 != AGO_k_f0)
                    {
                        float invn3 = AGO_k_f1 / (float)sqrt((double)fN3);
                        for (m = 0; m < 3; m++)
                            local_48[6 + m] = invn3 * fHalf * local_48[6 + m] + local_48[0xf + m];
                    }
                    scale = (float)rand() * AGO_k_f0_25 * AGO_k_f3_05185em05 - AGO_k_fMinus0_05;
                    AGO_call_vCreatePart(0x12, (MTH3D_tdstVector *)&local_48[6],
                                         (MTH3D_tdstVector *)&local_48[3], 2.0f,
                                         scale, scale, hVisualMaterial);
                }
            }
        }
        return;
    case 0x1f4:
    {
        unsigned long IntParam = (unsigned long)(long)parameter;
        AGO_call_vCreateTriangle((float *)AGO_gExtraParameter1,
                                 (float *)AGO_gExtraParameter2, &local_48[3],
                                 AGO_g_ExtraMaterial,
                                 0.0f, 0.0f, 1.0f, 0.0f, 1.0f, 1.0f,
                                 AGO_g_ExtraFloatParameter, IntParam);
        if (IntParam & 1)
            AGO_call_vCreateTriangle(&local_48[0], (float *)AGO_gExtraParameter1,
                                     &local_48[3], AGO_g_ExtraMaterial,
                                     0.0f, 1.0f, 0.0f, 0.0f, 1.0f, 1.0f,
                                     AGO_g_ExtraFloatParameter, IntParam);
        return;
    }
    case 0x64: case 0x65: case 0x66: case 0x67:
    case 0x104: case 0x105: case 0x12c:
        return;

    case 0:
    default:
        AGO_call_vCreatePart(AGO_STILL | AGO_TIMELIMITED | AGO_LOOPANIM, pos, NULL, 2.0f, 1.0f, 1.0f,
                             hVisualMaterial);
        return;
    }
}

AGO_Triangles AGO_g_pAGO_Triangles[AGO_MAX_TRIANGLES];   /* 0x004E1620 */

struct GLD_tdstViewportAttributes_
{
    unsigned long dwInitialHeight;
    unsigned long dwInitialWidth;
    unsigned long dwHeight;
    unsigned long dwWidth;
    unsigned long dwTopInPix;
    unsigned long dwBottomInPix;
    unsigned long dwLeftInPix;
    unsigned long dwRightInPix;
    unsigned long dwTopInPixForClip;
    unsigned long dwBottomInPixForClip;
    unsigned long dwLeftInPixForClip;
    unsigned long dwRightInPixForClip;
    unsigned long dwWidthInPercent;
    unsigned long dwHeightInPercent;
    unsigned long dwClipTopInPix;
    unsigned long dwClipBottomInPix;
    unsigned long dwClipLeftInPix;
    unsigned long dwClipRightInPix;
    unsigned long dwClipTopInPercent;
    unsigned long dwClipBottomInPercent;
    unsigned long dwClipLeftInPercent;
    unsigned long dwClipRightInPercent;
    int           iOffsetPosX;
    int           iOffsetPosY;
    char         *p_cVirtualScreen;
    long          lPitch;
    void         *p_vSpecificToXD;
    short         hDevice;
    short         hViewport;
};

float AGO_g_LogoPosition[4];        /* 0x004E1600 */
void *AGO_g_LogoMaterials[6 * 3];   /* 0x004E1020 */

#define GLI_vDraw2DSpritePixels GLI_vDraw2DSprite

/* 0x00465850 */
void AGO_vAddLogoPart(MTH_tdxReal parameter, void *hVisualMaterial)
{
    AGO_g_LogoMaterials[((long)(parameter))] = hVisualMaterial;
}

/* 0x00465870 */
void AGO_vActivateLogoDisplay(MTH3D_tdstVector *pPosition, MTH3D_tdstVector *pDirection, MTH_tdxReal parameter)
{
    AGO_g_LogoPosition[0] = pPosition->xX;
    AGO_g_LogoPosition[1] = pPosition->xY;
    AGO_g_LogoPosition[2] = pDirection->xX;
    AGO_g_LogoPosition[3] = pDirection->xY;
    AGO_g_xLogoTransparency = parameter;
}

void AGO_vDisplayLogo(GLD_tdstViewportAttributes *p_stVpt)
{

    int i, j;
    MTH_tdxReal x, y, dx, dy, xNext, yNext;

    if (GAM_g_bEngineFrozen)
        return;

    dx = (MTH_tdxReal)floor((AGO_g_LogoPosition[2] - AGO_g_LogoPosition[0])
                            * p_stVpt->dwRightInPix * 0.00166666659f);
    dy = (MTH_tdxReal)floor((AGO_g_LogoPosition[3] - AGO_g_LogoPosition[1])
                            * p_stVpt->dwBottomInPix * 0.00333333318f);
    GLI_vSetGlobalAlpha(AGO_g_xLogoTransparency);
    x = (MTH_tdxReal)floor(p_stVpt->dwRightInPix * AGO_g_LogoPosition[0] * 0.01f);

    for (i = 0 ; i < 6 ; i++)
    {
        y = (MTH_tdxReal)floor(p_stVpt->dwBottomInPix * AGO_g_LogoPosition[1] * 0.01f);
        xNext = x + dx;
        for (j = 0 ; j < 3 ; j++)
        {
            yNext = y + dy;
            GLI_vDraw2DSpritePixels(p_stVpt, xNext, x, y, yNext, AGO_g_LogoMaterials[j + 3 * i]);
            y = yNext;
        }
        x = xNext;
    }
}

void AGO_vDisplayGAUGES(GLD_tdstViewportAttributes *p_stVpt)
{
    int i;

#ifdef RAYMAN2_WIDESCREEN
    AGO_fn_vComputeSafeAreaAnchors();
#endif /* RAYMAN2_WIDESCREEN */

    for (i = 0; i < MAX_GAUGES; i++)
    {
        if (AGO_g_TheGauges[i].Transparency > 0)
        {
            MTH_tdxReal diffx = AGO_g_TheGauges[i].Coords[2] - AGO_g_TheGauges[i].Coords[0];
            MTH_tdxReal diffy = AGO_g_TheGauges[i].Coords[3] - AGO_g_TheGauges[i].Coords[1];
            MTH_tdxReal position;

#ifdef RAYMAN2_WIDESCREEN
            /* HUD (engine not frozen): the gauge's group may go to a screen edge */
            GLI_fn_vSetSafeAreaAnchorOfBox(AGO_M_lSafeAreaKeyGauge(i));
#endif /* RAYMAN2_WIDESCREEN */
            if (diffx <= diffy)
            {
                position = AGO_g_TheGauges[i].Coords[3] - diffy * AGO_g_TheGauges[i].Position * AGO_C_xPercentScale;
                GLI_vSetGlobalAlpha((float)AGO_g_TheGauges[i].Transparency);
                GLI_vDraw2DSpriteWithPercent(p_stVpt,
                    AGO_g_TheGauges[i].Coords[0], position, AGO_g_TheGauges[i].Coords[2], AGO_g_TheGauges[i].Coords[3],
                    AGO_g_TheGauges[i].FullMaterial);
                if (AGO_g_TheGauges[i].Position < AGO_C_xFullPercent)
                    GLI_vDraw2DSpriteWithPercent(p_stVpt,
                        AGO_g_TheGauges[i].Coords[0], AGO_g_TheGauges[i].Coords[1], AGO_g_TheGauges[i].Coords[2], position,
                        AGO_g_TheGauges[i].EmptyMaterial);
            }
            else
            {
                position = diffx * AGO_g_TheGauges[i].Position * AGO_C_xPercentScale + AGO_g_TheGauges[i].Coords[0];
                GLI_vSetGlobalAlpha((float)AGO_g_TheGauges[i].Transparency);
                GLI_vDraw2DSpriteWithPercent(p_stVpt,
                    AGO_g_TheGauges[i].Coords[0], AGO_g_TheGauges[i].Coords[1], position, AGO_g_TheGauges[i].Coords[3],
                    AGO_g_TheGauges[i].FullMaterial);
                if (AGO_g_TheGauges[i].Position < AGO_C_xFullPercent)
                    GLI_vDraw2DSpriteWithPercent(p_stVpt,
                        position, AGO_g_TheGauges[i].Coords[1], AGO_g_TheGauges[i].Coords[2], AGO_g_TheGauges[i].Coords[3],
                        AGO_g_TheGauges[i].EmptyMaterial);
            }
#ifdef RAYMAN2_WIDESCREEN
            GLI_g_eSafeAreaAnchor = GLI_C_eSafeAreaCentre;
#endif /* RAYMAN2_WIDESCREEN */
        }
    }

    AGO_g_xZValueForSprite = AGO_C_xZIcones;
    AGO_vDisplayIcones(p_stVpt);

    if (AGO_C_xLogoThreshold < AGO_g_xLogoTransparency)
    {
        AGO_g_xZValueForSprite = AGO_C_xZLogo;
        AGO_call_vDisplayLogo(p_stVpt);
    }

    AGO_g_xZValueForSprite = AGO_C_xZOverlay;
    JFFTXT_vAffiche(p_stVpt); /* 0x465C10 */
}

void __cdecl InitGauges(void)
{
    int i;
    for (i = 0; i < MAX_GAUGES; i++)
    {
        AGO_g_TheGauges[i].Transparency = 0;
        AGO_g_TheGauges[i].Position     = 0;
    }
}

void __cdecl InitIcones(void)
{
    int i;
    for (i = 0; i < MAX_ICONES; i++)
        AGO_g_TheIcones[i].Transparency = 0;
}

/* 0x00460530 */
void AddGauge(MTH_tdxReal *V1, MTH_tdxReal *V2, MTH_tdxReal Num,
                      void *EmptyMat, void *FullMat)
{
    long lIdx;

    lIdx = (long)Num;
    AGO_g_TheGauges[lIdx].Coords[0] = V1[0];
    AGO_g_TheGauges[lIdx].Coords[1] = V1[1];
    AGO_g_TheGauges[lIdx].Coords[2] = V2[0];
    AGO_g_TheGauges[lIdx].Coords[3] = V2[1];
    AGO_g_TheGauges[lIdx].EmptyMaterial = EmptyMat;
    AGO_g_TheGauges[lIdx].FullMaterial = FullMat;
    AGO_g_TheGauges[lIdx].Position = V1[2];
    AGO_g_TheGauges[lIdx].Transparency = (long)V2[2];
}

/* 0x004605A0 */
void AddIcone(MTH_tdxReal *V1, MTH_tdxReal *V2, MTH_tdxReal Num, void *Mat)
{
    long lIdx;

    lIdx = (long)Num;
    AGO_g_TheIcones[lIdx].Coords[0] = V1[0];
    AGO_g_TheIcones[lIdx].Coords[1] = V1[1];
    AGO_g_TheIcones[lIdx].Coords[2] = V2[0];
    AGO_g_TheIcones[lIdx].Coords[3] = V2[1];
    AGO_g_TheIcones[lIdx].Material = Mat;
    AGO_g_TheIcones[lIdx].Transparency = (long)V2[2];
}

void __cdecl JFFTXT_vInit(void)
{
    int i;
    for (i = 0; i < MAX_JFFTXT; i++)
    {
        AGO_g_JFFTXT[i].pString          = NULL;
        AGO_g_JFFTXT[i].ucTransparency   = 0;
        AGO_g_JFFTXT[i].bFrozenDisplay   = 0;
        AGO_g_JFFTXT[i].bWaveEffect      = 0;
        AGO_g_JFFTXT[i].bOther           = 0;
        AGO_g_JFFTXT[i].xEffectParameter = 0;
    }
}

#define JFFTXT_FADEIN        0x01
#define JFFTXT_FADEOUT       0x02
#define JFFTXT_SELECTED      0x04
#define JFFTXT_AUTOFRAME     0x08
#define JFFTXT_FORCED_COLOR  0x10

void __cdecl JFFTXT_vAddText(int StringNumber, MTH3D_tdstVector *p_stPosition, unsigned char *pszString, MTH_tdxReal xParameter)
{
    AGO_JFFTXT *p;

    if (StringNumber < MAX_JFFTXT)
    {
        p = &(AGO_g_JFFTXT[StringNumber]);
        p->pString = (char *)pszString;
        p->ucTransparency = (unsigned char)xParameter;
        p->bFrozenDisplay = 0;
        MTH3D_M_vCopyVector(&p->vPos, p_stPosition);
        p->bWaveEffect = 0;
        p->bOther = 0;

    }
}

void __cdecl JFFTXT_vExtendText(int StringNumber, int ID, MTH_tdxReal xParameter)
{
    AGO_JFFTXT *p;

    if (StringNumber < MAX_JFFTXT)
    {
        p = &(AGO_g_JFFTXT[StringNumber]);

        switch(ID)
        {
        case 0:
            p->ucTransparency = (unsigned char)xParameter;
            break;
        case 1:

            p->bWaveEffect = ((unsigned char)MTH_M_xRealToLong(xParameter) == 0) ? (unsigned char)0 : (unsigned char)1;
            break;
        case 2:
            p->bOther = JFFTXT_FADEIN;
            if (MTH_M_xRealToLong(p->ucTransparency) == 0)
                p->ucTransparency = (unsigned char)MTH_C_ONE;
            break;
        case 3:
            p->bOther = JFFTXT_FADEOUT;
            break;
        case 4:
            p->vPos.xZ = xParameter;
            break;
        case 5:
            if (MTH_M_xRealToLong(xParameter) == 0)
            {
                p->bOther &= ~JFFTXT_SELECTED;
            }
            else
            {
                p->bOther |= JFFTXT_SELECTED;
                p->xEffectParameter = MTH_M_xRealToLong(xParameter);
            }
            break;
        case 6:
            /* 0x465B37 */
            if (xParameter == AGO_asm_K_ca2c)
                p->bOther |= JFFTXT_AUTOFRAME;
            else
                p->bOther &= ~JFFTXT_AUTOFRAME;
            break;
        case 7:
            {
                switch (MTH_M_xRealToLong(xParameter))
                {
                case 0:
                    p->bOther &= ~JFFTXT_FORCED_COLOR;
                    p->ucForcedColor = 0;
                    break;
                case 1:
                    p->bOther |= JFFTXT_FORCED_COLOR;
                    p->ucForcedColor = 1;
                    break;
                case 2:
                    p->bOther |= JFFTXT_FORCED_COLOR;
                    p->ucForcedColor = 2;
                    break;
                case 3:
                    p->bOther |= JFFTXT_FORCED_COLOR;
                    p->ucForcedColor = 3;
                    break;
                }
            }
            break;
        }
    }
}

void __cdecl JFFTXT_vModifyCaracter(int StringNumber, int Position, unsigned char newcar)
{
    AGO_JFFTXT *p;

    if (StringNumber < MAX_JFFTXT)
    {
        p = &(AGO_g_JFFTXT[StringNumber]);

        if (p->pString != NULL)
        {
            p->pString[Position] = newcar;
        }
    }
}

unsigned int __cdecl JFFTXT_toupper(unsigned int c)
{
    if ((c >= 'a') && (c <= 'z'))
    {
        c = c - 'a' + 'A';
    }
    return c;
}

int __cdecl JFFTXT_iGetStringLength(unsigned char *pt, int dx)
{
    int i = 0;
    int Length = 0;
    unsigned char c;

    c = pt[i];

    while (c != '\0')
    {
        if (c == '/')
        {
            c = JFFTXT_toupper(pt[++i]);
            if (c == 'L')
                return Length;

            while (c != ':')
            {
                c = pt[++i];
            }
        }
        else
        {
            if (c == 32)
            {
                Length += MTH_M_xRealToLong(MTH_M_xMul(MTH_M_xLongToReal(dx), MTH_M_xConstFloatToReal(0.6f)));
            }
            else
            {
                Length += dx;
            }
        }
        c = pt[++i];
    }

    return (Length);
}

void __cdecl JFFTXT_GetSizeValues(MTH_tdxReal size, int *dx, int *dy, int *cx, int *cy)
{
    size = MTH_M_xSub(size, MTH_M_xLongToReal(15));
    *cy = MTH_M_xRealToLong(MTH_M_xSub(MTH_M_xLongToReal(38), MTH_M_xMul(size, MTH_M_xConstFloatToReal(-2.5f))));
    size = MTH_M_xMul(size, MTH_C_2);
    *dx = MTH_M_xRealToLong(MTH_M_xAdd(size, MTH_M_xLongToReal(32)));
    *cx = MTH_M_xRealToLong(MTH_M_xAdd(size, MTH_M_xLongToReal(32)));
    *dy = MTH_M_xRealToLong(MTH_M_xSub(MTH_M_xLongToReal(46), MTH_M_xMul(size, MTH_M_xConstFloatToReal(-2.0f))));
}

void __cdecl JFFTXT_vGetStringExtents(AGO_JFFTXT *p, MTH3D_tdstVector *x1, MTH3D_tdstVector *x2)
{
    int i = 0;
    unsigned char *pt;
    unsigned char c;
    MTH_tdxReal x, y;
    int dx, dy;
    int cx, cy;
    int bCentering = 0;

    pt = (unsigned char *)p->pString;

    x = p->vPos.xX;
    y = p->vPos.xY;

    /* 0x465E54 */
    x2->xX = x1->xX = x;
    x2->xY = x1->xY = y;

    JFFTXT_GetSizeValues(p->vPos.xZ, &dx, &dy, &cx, &cy);

    c = JFFTXT_toupper(pt[i]);

    while (c != '\0')
    {
        if (c == '/')
        {
            c = JFFTXT_toupper(pt[++i]);
            if (c == '\0')
                return;

            if (c == 'L')
            {
                c = pt[++i];
                if (c == '\0')
                    return;
                if (c == ':')
                {
                    if (bCentering)
                        x = MTH_M_xLongToReal((1000 - JFFTXT_iGetStringLength(&pt[i+1], dx)) >> 1);
                    else
                        x = p->vPos.xX;

                    if (MTH_M_bLess(x, x1->xX))
                        x1->xX = x;
                    y = MTH_M_xAdd(y, MTH_M_xLongToReal(dy));
                    if (MTH_M_bGreater(y, x2->xY))
                        x2->xY = y;
                }
                else
                    return;
            }
            else if (c == 'O')
            {
                c = pt[++i];
                if (c == '\0')
                    return;

                while (c != ':')
                {
                    c = pt[++i];
                    if (c == '\0')
                        return;
                }
            }
            else if (c == 'C')
            {
                c = pt[++i];
                if (c == '\0')
                    return;
                if (c == ':')
                {
                    bCentering ^= 1;
                    if (bCentering)
                    {
                        x = MTH_M_xLongToReal((1000-JFFTXT_iGetStringLength(&pt[i+1], dx))>>1);
                        if (MTH_M_bLess(x, x1->xX))
                            x1->xX = x;
                        if (MTH_M_bGreater(x, x2->xX))
                            x2->xX = x;
                    }
                }
                else
                    return;
            }
        }
        else if (c == 32)
        {
            /* 0x466000 */
            x = MTH_M_xSub(x, MTH_M_xLongToReal(MTH_M_xRealToLong(MTH_M_xMul(MTH_M_xLongToReal(dx), MTH_M_xConstFloatToReal(-0.6f)))));
        }
        else
        {
            x = MTH_M_xAdd(x, MTH_M_xLongToReal(dx));
        }

        if (MTH_M_bGreater(x, x2->xX))
            x2->xX = x;

        c = JFFTXT_toupper(pt[++i]);
    }

    x1->xX = MTH_M_xMul(MTH_M_xSub(x1->xX, MTH_M_xLongToReal(10)), MTH_M_xConstFloatToReal(0.1f));
    x1->xY = MTH_M_xMul(MTH_M_xSub(x1->xY, MTH_M_xLongToReal(10)), MTH_M_xConstFloatToReal(0.1f));
    x2->xX = MTH_M_xMul(MTH_M_xAdd(x2->xX, MTH_M_xLongToReal(10)), MTH_M_xConstFloatToReal(0.1f));
    /* 0x466088 */
    x2->xY = MTH_M_xMul(MTH_M_xAdd(MTH_M_xAdd(MTH_M_xLongToReal(dy), x2->xY), MTH_M_xLongToReal(10)), MTH_M_xConstFloatToReal(0.1f));
}

#define JFFTXT_pc_BLUE_R   110
#define JFFTXT_pc_BLUE_G   190
#define JFFTXT_pc_BLUE_B   255
#define JFFTXT_pc_RED_R    250
#define JFFTXT_pc_RED_G    70
#define JFFTXT_pc_RED_B    70
#define JFFTXT_pc_YELLOW_R 255
#define JFFTXT_pc_YELLOW_G 255
#define JFFTXT_pc_YELLOW_B 120

extern unsigned char GLI_g_aForcedAAAColor_ADDR[3];
extern unsigned char g_GLI_bForceAAAColor;
extern void *__cdecl GLI_pGetVisualMaterialForLetter(int c);

void __cdecl JFFTXT_vDrawString(GLD_tdstViewportAttributes *p_stVpt, AGO_JFFTXT *p)
{
    int i = 0;
    unsigned char *pt;
    unsigned int c;
    MTH_tdxReal x, y;
    int dx, dy;
    int cx, cy;
    void *hVisualMaterial;
    unsigned int CurrentOffset = 0;
    int bCentering = 0;
    MTH_tdxReal Waveeffect = MTH_C_ZERO;
    int RealChar = 1;

    g_GLI_bForceAAAColor = 1;

    pt = (unsigned char *)p->pString;

    if (p->bOther & JFFTXT_FORCED_COLOR)
    {
        switch (p->ucForcedColor)
        {
        case 1:
            GLI_g_aForcedAAAColor_ADDR[0] = JFFTXT_pc_BLUE_R;
            GLI_g_aForcedAAAColor_ADDR[1] = JFFTXT_pc_BLUE_G;
            GLI_g_aForcedAAAColor_ADDR[2] = JFFTXT_pc_BLUE_B;
            break;
        case 2:
            GLI_g_aForcedAAAColor_ADDR[0] = JFFTXT_pc_RED_R;
            GLI_g_aForcedAAAColor_ADDR[1] = JFFTXT_pc_RED_G;
            GLI_g_aForcedAAAColor_ADDR[2] = JFFTXT_pc_RED_B;
            break;
        case 3:
            GLI_g_aForcedAAAColor_ADDR[0] = JFFTXT_pc_YELLOW_R;
            GLI_g_aForcedAAAColor_ADDR[1] = JFFTXT_pc_YELLOW_G;
            GLI_g_aForcedAAAColor_ADDR[2] = JFFTXT_pc_YELLOW_B;
            break;
        }
    }
    else
    {
        GLI_g_aForcedAAAColor_ADDR[0] = JFFTXT_pc_BLUE_R;
        GLI_g_aForcedAAAColor_ADDR[1] = JFFTXT_pc_BLUE_G;
        GLI_g_aForcedAAAColor_ADDR[2] = JFFTXT_pc_BLUE_B;
    }

    if (p->bWaveEffect)
    {
        Waveeffect = MTH_M_xMul(MTH_M_xLongToReal(7), MTH_M_xSin(MTH_M_xMul(AGO_g_ffFrameTime, MTH_M_xLongToReal(5))));
    }

    x = p->vPos.xX;
    y = p->vPos.xY;

    JFFTXT_GetSizeValues(p->vPos.xZ, &dx, &dy, &cx, &cy);

    GLI_vSetGlobalAlpha(p->ucTransparency);

    c = JFFTXT_toupper(pt[i]);

    while (c != '\0')
    {
        if (c == '/')
        {
            c = JFFTXT_toupper(pt[++i]);
            if (c == '\0')
                return;

            if (c == 'L')
            {
                c = pt[++i];
                if (c == '\0')
                    return;
                if (c == ':')
                {
                    if (bCentering)
                        x = MTH_M_xLongToReal((1000 - JFFTXT_iGetStringLength(&pt[i + 1], dx)) >> 1);
                    else
                        x = p->vPos.xX;
                    y = MTH_M_xAdd(y, MTH_M_xLongToReal(dy));
                }
                else
                    return;
            }
            else if (c == 'O')
            {
                CurrentOffset = 0;

                c = pt[++i];
                if (c == '\0')
                    return;

                while (c != ':')
                {
                    CurrentOffset *= 10;
                    CurrentOffset += c - '0';
                    c = pt[++i];
                    if (c == '\0')
                        return;
                }

                if ((CurrentOffset == 0) || (CurrentOffset == 600))
                {
                    GLI_g_aForcedAAAColor_ADDR[0] = JFFTXT_pc_BLUE_R;
                    GLI_g_aForcedAAAColor_ADDR[1] = JFFTXT_pc_BLUE_G;
                    GLI_g_aForcedAAAColor_ADDR[2] = JFFTXT_pc_BLUE_B;
                }
                else if (CurrentOffset == 200)
                {
                    GLI_g_aForcedAAAColor_ADDR[0] = JFFTXT_pc_RED_R;
                    GLI_g_aForcedAAAColor_ADDR[1] = JFFTXT_pc_RED_G;
                    GLI_g_aForcedAAAColor_ADDR[2] = JFFTXT_pc_RED_B;
                }
                else if (CurrentOffset == 400)
                {
                    GLI_g_aForcedAAAColor_ADDR[0] = JFFTXT_pc_YELLOW_R;
                    GLI_g_aForcedAAAColor_ADDR[1] = JFFTXT_pc_YELLOW_G;
                    GLI_g_aForcedAAAColor_ADDR[2] = JFFTXT_pc_YELLOW_B;
                }
            }
            else if (c == 'C')
            {
                c = pt[++i];
                if (c == '\0')
                    return;
                if (c == ':')
                {
                    bCentering ^= 1;
                    if (bCentering)
                        x = MTH_M_xLongToReal((1000 - JFFTXT_iGetStringLength(&pt[i + 1], dx)) >> 1);
                }
                else
                    return;
            }
        }
        else if (c == 32)
        {

            x = MTH_M_xSub(x, MTH_M_xLongToReal(MTH_M_xRealToLong(MTH_M_xMul(MTH_M_xLongToReal(dx), MTH_M_xConstFloatToReal(-0.6f)))));
            RealChar++;
        }
        else
        {
            unsigned int c2;
            MTH_tdxReal ftempy;

            c2 = (unsigned int)(c);
            if (c > 160)
                c2 -= 32;
            c2 -= 32;

            hVisualMaterial = GLI_pGetVisualMaterialForLetter(c2 + CurrentOffset);

            if (hVisualMaterial != NULL)
            {
                if (CurrentOffset >= 800)
                    g_GLI_bForceAAAColor = 0;

                if (p->bOther & JFFTXT_SELECTED)
                {
                    if (RealChar == p->xEffectParameter)
                    {
                        GLI_vSetGlobalAlpha(p->ucTransparency >> 1);

                        Waveeffect = MTH_M_xMul(MTH_M_xLongToReal(7), MTH_M_xSin(MTH_M_xMul(AGO_g_ffFrameTime, MTH_M_xLongToReal(5))));
                    }
                    else
                    {
                        GLI_vSetGlobalAlpha(p->ucTransparency);
                        Waveeffect = MTH_C_ZERO;
                    }
                }

                ftempy = y;

                if (c == '.')
                {
                    ftempy += MTH_M_xLongToReal(cy >> 2);
                }
                else if (c == ',')
                {
                    ftempy += MTH_M_xLongToReal(cy >> 1);
                }
                else if (c == '\'')
                {
                    ftempy -= MTH_M_xLongToReal(cy >> 2);
                }

                GLI_vDraw2DSpriteWithPercent(p_stVpt,
                    MTH_M_xMul(x, MTH_M_xConstFloatToReal(0.1f)),
                    MTH_M_xMul(MTH_M_xSub(ftempy, Waveeffect), MTH_M_xConstFloatToReal(0.1f)),
                    MTH_M_xMul(MTH_M_xAdd(x, MTH_M_xLongToReal(cx)), MTH_M_xConstFloatToReal(0.1f)),
                    MTH_M_xMul(MTH_M_xAdd(MTH_M_xAdd(ftempy, MTH_M_xLongToReal(cy)), Waveeffect), MTH_M_xConstFloatToReal(0.1f)),
                    hVisualMaterial);

                g_GLI_bForceAAAColor = 1;
            }

            x = MTH_M_xAdd(x, MTH_M_xLongToReal(dx));
            RealChar++;
        }
        c = JFFTXT_toupper(pt[++i]);
    }
}
#ifdef RAYMAN2_WIDESCREEN
/*
 ----------------------------------------------------------------------------------------
  Description   : Edge anchors of this frame's HUD (wide display modes): the boxes of
                  the gauges, icons and strings drawn while the engine is not frozen
                  (the original's own test for the in-game HUD) are grouped where they
                  touch, so an icon and its counter go to a screen edge together
 ----------------------------------------------------------------------------------------
 */
static void AGO_fn_vComputeSafeAreaAnchors(void)
{
    MTH3D_tdstVector stMin, stMax;
    int i;

    GLI_fn_vResetSafeAreaBoxes();
    if (GAM_g_bEngineFrozen)
        return;

    for (i = 0; i < MAX_GAUGES; i++)
        if (AGO_g_TheGauges[i].Transparency > 0)
            GLI_fn_vAddSafeAreaBox(AGO_M_lSafeAreaKeyGauge(i),
                                   AGO_g_TheGauges[i].Coords[0], AGO_g_TheGauges[i].Coords[1],
                                   AGO_g_TheGauges[i].Coords[2], AGO_g_TheGauges[i].Coords[3]);
    for (i = 0; i < MAX_ICONES; i++)
        if (AGO_g_TheIcones[i].Transparency > 0)
            GLI_fn_vAddSafeAreaBox(AGO_M_lSafeAreaKeyIcone(i),
                                   AGO_g_TheIcones[i].Coords[0], AGO_g_TheIcones[i].Coords[1],
                                   AGO_g_TheIcones[i].Coords[2], AGO_g_TheIcones[i].Coords[3]);
    for (i = 0; i < MAX_JFFTXT; i++)
        if ((AGO_g_JFFTXT[i].ucTransparency != 0) && (AGO_g_JFFTXT[i].pString != NULL))
        {
            JFFTXT_vGetStringExtents(&AGO_g_JFFTXT[i], &stMin, &stMax);
            GLI_fn_vAddSafeAreaBox(AGO_M_lSafeAreaKeyText(i), stMin.xX, stMin.xY, stMax.xX, stMax.xY);
        }

    GLI_fn_vComputeSafeAreaAnchors(MTH_M_xConstFloatToReal(100.0f));
}
#endif /* RAYMAN2_WIDESCREEN */

/* 0x00466520 */
void AGO_vTrianglesInit(void)
{
    AGO_g_NbAGO_Triangles = 0;
}

void AGO_vCreateTriangle(float c1[3], float c2[3], float c3[3], void *VisualMat,
                         float U1, float V1, float U2, float V2, float U3, float V3,
                         float DelayTime, unsigned long Parameter)
{
    AGO_Triangles *p;
    int            i;

    if (AGO_g_NbAGO_Triangles == AGO_MAX_TRIANGLES)
    {
        unsigned long Least = AGO_g_pAGO_Triangles[0].CreationTime;
        int           LeastIndex = 0;
        for (i = 0; i < AGO_MAX_TRIANGLES; i++)
            if (AGO_g_pAGO_Triangles[i].CreationTime < Least)
            {
                Least = AGO_g_pAGO_Triangles[i].CreationTime;
                LeastIndex = i;
            }
        p = &AGO_g_pAGO_Triangles[LeastIndex];
    }
    else
    {
        p = &AGO_g_pAGO_Triangles[AGO_g_NbAGO_Triangles++];
    }

    p->CreationTime = AGO_g_NbEngineFramesSinceStart;
    for (i = 0; i < 3; i++)
    {
        p->c[0][i] = c1[i];
        p->c[1][i] = c2[i];
        p->c[2][i] = c3[i];
    }
    p->uv[0][0] = U1; p->uv[0][1] = V1;
    p->uv[1][0] = U2; p->uv[1][1] = V2;
    p->uv[2][0] = U3; p->uv[2][1] = V3;
    p->fTransparency  = 255.0f;
    p->VisualMaterial = VisualMat;
    p->DelayTime      = (unsigned long)DelayTime;
    p->type           = 0;
    if (Parameter & 0x02)
        p->type |= AGO_TRI_FADE;
}

void AGO_vDisplayTriangles(GLD_tdstViewportAttributes *p_stVpt)
{
    AGO_Triangles *p;
    unsigned long  i;

    AGO_g_NbEngineFramesSinceStart++;

    for (i = 0; i < AGO_g_NbAGO_Triangles; i++)
    {
        p = &AGO_g_pAGO_Triangles[i];

        if (p->type & AGO_TRI_FADE)
        {
            p->fTransparency=255.0f-255.0f*((float)(AGO_g_NbEngineFramesSinceStart-p->CreationTime-1))/((float)(p->DelayTime));
        }

        GLI_vSetGlobalAlpha(p->fTransparency);

        GLI_vDisplayWorldTriangle(
            p_stVpt,
            p->c,
            p->uv,
            p->VisualMaterial);

        if (AGO_g_NbEngineFramesSinceStart-p->CreationTime>=p->DelayTime)
        {
            p->type|=0x80000000;
        }
    }

    i = 0;
    while (i < AGO_g_NbAGO_Triangles)
    {
        p = &AGO_g_pAGO_Triangles[i];
        if (p->type & 0x80000000)
        {
            unsigned long j;
            for (j = i + 1; j < AGO_g_NbAGO_Triangles; j++)
                AGO_g_pAGO_Triangles[j - 1] = AGO_g_pAGO_Triangles[j];
            AGO_g_NbAGO_Triangles--;
        }
        else
        {
            i++;
        }
    }
}

void __cdecl JFFTXT_vAffiche(GLD_tdstViewportAttributes *p_stVpt)
{
    AGO_JFFTXT *p;
    int i;

    for (i = 0; i < MAX_JFFTXT; i++)
    {
        p = &(AGO_g_JFFTXT[i]);

        if ((p->ucTransparency != 0) && (p->pString != NULL))
        {
            if ((i < 15) || (!GAM_g_bEngineFrozen))
            {
                if (p->bOther & JFFTXT_FADEIN)
                {
                    long trsp;
                    trsp = MTH_M_xRealToLong(MTH_M_xSub(MTH_M_xLongToReal(p->ucTransparency), MTH_M_xMul(AGO_g_fTimeDifference, MTH_M_xLongToReal(-200))));
                    if (trsp >= 255)
                    {
                        trsp = 255;
                        p->bOther &= ~JFFTXT_FADEIN;
                    }
                    p->ucTransparency = (unsigned char)trsp;
                }
                else if (p->bOther & JFFTXT_FADEOUT)
                {
                    long trsp;
                    trsp = MTH_M_xRealToLong(MTH_M_xSub(MTH_M_xLongToReal(p->ucTransparency), MTH_M_xMul(AGO_g_fTimeDifference, MTH_M_xLongToReal(200))));
                    if (trsp <= 0)
                    {
                        trsp = 0;
                        p->bOther &= ~JFFTXT_FADEOUT;
                    }
                    p->ucTransparency = (unsigned char)trsp;
                }

#ifdef RAYMAN2_WIDESCREEN
                /* HUD (engine not frozen): the string's group may go to a screen edge */
                GLI_fn_vSetSafeAreaAnchorOfBox(AGO_M_lSafeAreaKeyText(i));
#endif /* RAYMAN2_WIDESCREEN */
                if (p->bOther & JFFTXT_AUTOFRAME)
                {
                    MTH3D_tdstVector x1, x2;

                    JFFTXT_vGetStringExtents(p, &x1, &x2);

                    GLI_vDisplayFrame((float *)&x1, (float *)&x2,
                        MTH_M_xLongToReal(MTH_M_xRealToLong(MTH_M_xMul(MTH_M_xLongToReal(p->ucTransparency), MTH_M_xConstFloatToReal(0.7f)))),
                        p_stVpt);
                }
                JFFTXT_vDrawString(p_stVpt, p);
#ifdef RAYMAN2_WIDESCREEN
                GLI_g_eSafeAreaAnchor = GLI_C_eSafeAreaCentre;
#endif /* RAYMAN2_WIDESCREEN */
            }
        }
    }
    g_GLI_bForceAAAColor = 0;
}

void __cdecl AGO_vInit(void)
{
    AGO_g_ulNumberOfAgos       = 0;   /* 0x4B9AD0 */
    AGO_g_bUsingMapMondeAGOs   = 0;   /* 0x4B9AE0 */
    AGO_g_ffLastFrameTime      = 0;   /* 0x4B9B00 */
    AGO_g_NumberOfZeldaAgos    = 0;   /* 0x4B9B08 */
    AGO_g_MustReinitStaticVars = 1;   /* 0x4A4074 */
    AGO_g_PloufMaterial        = 0;   /* 0x4B9AD8 */
    AGO_g_Timer7               = 0;   /* 0x4B9B0C */
    AGO_g_SplashFlag           = 0;   /* 0x4B9AF8 */
    AGO_g_xLogoTransparency    = 0;   /* 0x4B9B10 */

    InitGauges();
    InitIcones();
    JFFTXT_vInit();
    AGO_vTrianglesInit();
}
