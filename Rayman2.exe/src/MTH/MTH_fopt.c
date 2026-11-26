#include <math.h>
#include "MTH/MTH_fopt.h"
#include "MTH/MTH_globals.h"

unsigned long MTH_g_a2048_fInvSquareRootTable[2048];   /* 0x0050BB60 */
unsigned long MTH_g_a2048_fSquareRootTable[1024*2];    /* 0x0050DB80 */
unsigned long MTH_g_a1024_fInverse[1024];              /* 0x0050FBA0 */

/* 0x004445E0 */
static void MTH_pc_vBuildSquareRootTable(void)
{
    float xTmp;
    int   i;
    for (i = 0; i < 1024; i++) {
        xTmp = (float)sqrt(i * (1.0f / 1024.0f) - -1.0);
        MTH_g_a2048_fSquareRootTable[i + 1024] = *(long *)&xTmp & 0x7FFFFF;
        xTmp = (float)(sqrt(2.0) * sqrt(i * (1.0f / 1024.0f) - -1.0));
        MTH_g_a2048_fSquareRootTable[i] = *(long *)&xTmp & 0x7FFFFF;
    }
}

/* 0x00444660 */
static void MTH_pc_vBuildInverseTable(void)
{
    float xTmp;
    int   i;
    for (i = 0; i < 1024; i++) {
        xTmp = 1.0f / (i * (1.0f / 1024.0f) - -1.0f);
        MTH_g_a1024_fInverse[i] = *(unsigned long *)&xTmp & 0x7FFFFF;
    }
    MTH_g_a1024_fInverse[0] = 0x800000;
}

/* 0x0050bb60 */
static void MTH_pc_vBuildInvSquareRootTable(void)
{
    float xTmp;
    int   i;
    for (i = 0; i < 1024; i++) {
        xTmp = (float)(2.0 / sqrt(i * (1.0f / 1024.0f) - -1.0));
        MTH_g_a2048_fInvSquareRootTable[i + 1024] = *(unsigned long *)&xTmp & 0x7FFFFF;
        xTmp = (float)(sqrt(2.0) / sqrt(i * (1.0f / 1024.0f) - -1.0));
        MTH_g_a2048_fInvSquareRootTable[i] = (*(unsigned long *)&xTmp & 0x7FFFFF) + 0x800000;
    }
    MTH_g_a2048_fInvSquareRootTable[1024] = 0x800000;
}

/* 0x004b72d8 */
void MTH_fn_vInit(void)
{

    MTH_gs_dDecal = pow(2.0, 51.0) * 3.0;

    if (MTH_g_bTablesInitialised == 0) {
        MTH_pc_vBuildSquareRootTable();      /* 0x004445e0 */
        MTH_pc_vBuildInverseTable();         /* 0x00444660 */
        MTH_pc_vBuildInvSquareRootTable();   /* 0x004446c0 */
        MTH_g_bTablesInitialised = 1;
    }
}
