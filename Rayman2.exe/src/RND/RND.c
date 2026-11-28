#include <windows.h>
#include <stdlib.h>
#include <string.h>

#include "RND/RND.h"
#include "ERM/CpaError.h"
#include "MTH/MTH_flt.h"
#include "MMG/MMG_alloc.h"
#include "app/regress.h"
#include "TMR/sdl_timer.h"   /* USE_SDL: GetTickCount from the SDL timer */

extern void __cdecl Mmg_fn_cCheckAlignmentValidity(void);

extern unsigned char __cdecl FUN_0044f4e0(void);

/* 0x00449A50 */
void __cdecl RND_fn_vFirstInitRandom(void)
{
    if (g_ucRNDModuleId == 0xff)
        g_ucRNDModuleId = Erm_fn_ucInitErrMsg();
    Mmg_fn_v_InitMmg(g_ucRNDModuleId, 1, 0x186a0);
    Mmg_fn_cCheckAlignmentValidity();
    RND_g_stRandomStructure.hLastUsedIndex = 0;
}

/* 0x004a1f4c */
RND_tdxHandleToRandomIndex __cdecl RND_fn_hReserveANewHandleIndex(void)
{
    if (RND_g_stRandomStructure.hLastUsedIndex > RND_C_MaxIndex) {
        ERM_fn_vClearLastError(0);
        ERM_fn_vUpdateLastError(7, g_ucRNDModuleId, 0, 5,
                                -1, 0xff, 0xff, (char *)0);
    }
    RND_g_stRandomStructure.hLastUsedIndex++;
    return (RND_g_stRandomStructure.hLastUsedIndex - 1);
}

/* 0x00449AE0 */
void __cdecl RND_fn_vComputeRandomTable(void)
{
    unsigned long i;

    if (FUN_0044f4e0() == 2)
        srand(0xDEADBEEFu);
    else
#ifdef RAYMAN2_REGRESS
        srand(REGRESS_fn_bActive() ? REGRESS_C_ulRandSeed : GetTickCount());
#else
        srand(GetTickCount());
#endif /* RAYMAN2_REGRESS */

    RND_g_stRandomStructure.ulMaxValueInTable = 0;
    for (i = 0; i < RND_g_stRandomStructure.ulSizeOfTable; i++) {
        RND_g_stRandomStructure.p_ulTable[i] = (unsigned long)rand();
    }

    RND_fn_vRemapRandomTable();
}

/* 0x00449B40 */
void __cdecl RND_fn_vRemapRandomTable(void)
{
    unsigned long  i;
    unsigned long  ulMinValueInTable;

    ulMinValueInTable = RND_g_stRandomStructure.p_ulTable[0];
    RND_g_stRandomStructure.ulMaxValueInTable = 0;
    for (i = 0; i < RND_g_stRandomStructure.ulSizeOfTable; i++) {
        if (RND_g_stRandomStructure.p_ulTable[i] < ulMinValueInTable)
            ulMinValueInTable = RND_g_stRandomStructure.p_ulTable[i];
    }

    for (i = 0; i < RND_g_stRandomStructure.ulSizeOfTable; i++) {
        RND_g_stRandomStructure.p_ulTable[i] -= ulMinValueInTable;
        if (RND_g_stRandomStructure.p_ulTable[i] > RND_g_stRandomStructure.ulMaxValueInTable)
            RND_g_stRandomStructure.ulMaxValueInTable = RND_g_stRandomStructure.p_ulTable[i];
    }

    RND_g_stRandomStructure.ulMaxValueInTable += 1;
    RND_g_stRandomStructure.xInverseMaxValueInTable =
        1.0f / (float)RND_g_stRandomStructure.ulMaxValueInTable;

    memset(&RND_g_stRandomStructure.a_ulIndex[0], 0,
           RND_C_MaxIndex * sizeof(unsigned long));
}

/* 0x00449BE0 */
void RDN_fn_vSetRandomSeed(RND_tdxHandleToRandomIndex _lIndexNumber, long _lNewSeed)
{
    unsigned long ulValue;

    if (_lNewSeed < 0)
        ulValue = 0;
    else
        ulValue = (unsigned long)_lNewSeed % RND_g_stRandomStructure.ulSizeOfTable;
    RND_g_stRandomStructure.a_ulIndex[_lIndexNumber] = ulValue;
}

/* 0x00449C10 */
long __cdecl RND_fn_lGetLongRandomValue(RND_tdxHandleToRandomIndex _lIndexNumber,
                                        long _lMin, long _lMax)
{
    long lValue;

    lValue = (_lMin + ((_lMax + 1 - _lMin) *
              (long)RND_g_stRandomStructure.p_ulTable[RND_g_stRandomStructure.a_ulIndex[_lIndexNumber]]) /
              (RND_g_stRandomStructure.ulMaxValueInTable + 1));

    RND_g_stRandomStructure.a_ulIndex[_lIndexNumber]++;
    if (RND_g_stRandomStructure.a_ulIndex[_lIndexNumber] >= RND_g_stRandomStructure.ulSizeOfTable)
        RND_g_stRandomStructure.a_ulIndex[_lIndexNumber] = 0;

    return (lValue);
}

/* 0x00449C70 */
MTH_tdxReal __cdecl RND_fn_xGetRealRandomValue(RND_tdxHandleToRandomIndex _lIndexNumber,
                                               MTH_tdxReal _xMin, MTH_tdxReal _xMax)
{
    MTH_tdxReal xValue;

    xValue = MTH_M_xAdd(_xMin,
             MTH_M_xMul(MTH_M_xMul(MTH_M_xSub(_xMax, _xMin),
                 RND_g_stRandomStructure.p_ulTable[RND_g_stRandomStructure.a_ulIndex[_lIndexNumber]]),
                 RND_g_stRandomStructure.xInverseMaxValueInTable));

    RND_g_stRandomStructure.a_ulIndex[_lIndexNumber]++;
    if (RND_g_stRandomStructure.a_ulIndex[_lIndexNumber] >= RND_g_stRandomStructure.ulSizeOfTable)
        RND_g_stRandomStructure.a_ulIndex[_lIndexNumber] = 0;

    return (xValue);
}
