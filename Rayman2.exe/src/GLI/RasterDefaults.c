#include "GLI/RasterDefaults.h"
#include "GLI/GLI_globals.h"

#include <math.h>

static float               GLI_pc_xCosPhaseCoef;
static const unsigned long GLI_pc_ulCosPhaseTimeScaleBits = 0x3C51B717u;
#define GLI_M_xCosPhaseCoef       GLI_pc_xCosPhaseCoef
#define GLI_M_xCosPhaseTimeScale  (*(const float *)&GLI_pc_ulCosPhaseTimeScaleBits)

float               GLI_RD_g_xMagic7244_storage;   /* 0x004A7244 */
static unsigned char GLI_RD_g_bCosTableInit_storage = 1;   /* 0x004A06FC */
static long          GLI_RD_g_l06ec_storage;              /* 0x004A06EC */
static long          GLI_RD_g_l06f0_storage;              /* 0x004A06F0 */
static long          GLI_RD_g_l06f4_storage;              /* 0x004A06F4 */
long                GLI_RD_g_l06f8_storage;        /* 0x004A06F8 */
#define GLI_RD_g_xMagic7244     GLI_RD_g_xMagic7244_storage
#define GLI_RD_g_bCosTableInit  GLI_RD_g_bCosTableInit_storage
#define GLI_RD_g_l06ec          GLI_RD_g_l06ec_storage
#define GLI_RD_g_l06f0          GLI_RD_g_l06f0_storage
#define GLI_RD_g_l06f4          GLI_RD_g_l06f4_storage
#define GLI_RD_g_l06f8          GLI_RD_g_l06f8_storage

extern long xSinusEffectDepthStart;     /* 0x0050401C */
#define GLI_RD_g_l401c xSinusEffectDepthStart
static long GLI_RD_g_l405c;   /* 0x0050405C */
static long GLI_RD_g_l4084;   /* 0x00504084 */

extern unsigned long GLI_pc_g_a3_xMirrorPlaneNormal[3];
extern unsigned long GLI_pc_g_xMirrorPlaneDistance;

void __cdecl FUN_00423ab0(void)
{

    GLI_RD_g_xMagic7244 = (float)(pow(2.0, 23.0) * 3.0);

    if (GLI_RD_g_bCosTableInit != 0) {
        static const unsigned __int64 kStepBits = 0x3FB921FB60000000ui64;
        const double kStep = *(const double *)&kStepBits;
        float *p_aTable = GLI_M_p2CosTable;
        int i;
        for (i = 0; i < 64; i++) {
            p_aTable[i] = (float)(2.0 * cos((double)i * kStep));
        }
    }
    GLI_RD_g_bCosTableInit = 0;

    GLI_M_WaterDepthColorFlags = 0;     /* 0x504080 */
    GLI_RD_g_l405c = 0;
    GLI_RD_g_l4084 = 0x20;
    GLI_M_pWaterDepthColorCoef[0] = 0xbf19999a;   /* 0x504070 */
    GLI_M_pWaterDepthColorCoef[1] = 0xbe99999a;   /* 0x504074 */
    GLI_M_pWaterDepthColorCoef[2] = 0xbecccccd;   /* 0x504078 */
    GLI_RD_g_l06ec = 0x3f333333;
    GLI_RD_g_l06f0 = 0x3fb33333;
    GLI_RD_g_l06f4 = 0xc0a00000;
    GLI_RD_g_l06f8 = 0x40e00000;
    GLI_pc_g_a3_xMirrorPlaneNormal[0] = 0;
    GLI_pc_g_a3_xMirrorPlaneNormal[2] = 0;
    GLI_pc_g_a3_xMirrorPlaneNormal[1] = 0x3f800000;
    GLI_pc_g_xMirrorPlaneDistance     = 0;
    GLI_M_stSinEffectAmp.xX = 1.0f;                   /* 0x5040D0 */
    *(long *)&GLI_M_stSinEffectAmp.xY = 0;            /* 0x5040D4 */
    *(long *)&GLI_M_stSinEffectAmp.xZ = 0;            /* 0x5040D8 */
    GLI_M_a3_lSinEffectFreq[0]  = 1;   /* 0x5042E0 */
    GLI_M_a3_lSinEffectFreq[1]  = 0;   /* 0x5042E4 */
    GLI_M_a3_lSinEffectFreq[2]  = 0;   /* 0x5042E8 */
    GLI_M_a3_lSinEffectBase[0] = 0;    /* 0x5042F0 */
    GLI_M_a3_lSinEffectBase[1] = 0;    /* 0x5042F4 */
    GLI_M_a3_lSinEffectBase[2] = 0;    /* 0x5042F8 */
    GLI_RD_g_l401c = 0x41200000;
    GLI_pc_g_cSinusEffectState = 0;
    g_cSinusEffectFlag = 0;   /* 0x00504088 */
}

void __cdecl GLI_pc_fn_vUpdateCosPhase(unsigned long ulDeltaTime)
{

    MTH_tdxReal xFactor;
    float      *a3_xSinEffectBase = (float *)GLI_M_a3_lSinEffectBase;

    GLI_M_xCosPhaseCoef = xFactor = (float)ulDeltaTime * GLI_M_xCosPhaseTimeScale;
    a3_xSinEffectBase[0] += GLI_M_a3_lSinEffectFreq[0] * xFactor;
    a3_xSinEffectBase[1] += GLI_M_a3_lSinEffectFreq[1] * xFactor;
    a3_xSinEffectBase[2] += GLI_M_a3_lSinEffectFreq[2] * xFactor;
}
