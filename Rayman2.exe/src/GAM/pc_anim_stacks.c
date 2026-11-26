#include "GAM/GAM_types.h"
#include "GAM/A3d.h"
#include "MMG/MMG_alloc.h"
#include "GEO/GEO_globals.h"
#include "ERM/CpaError.h"
#include <stdio.h>
#include <string.h>

/* 0x00500274 */
int   *p_stFramesKF;
short *p_stAngularSpeedQuats;
/* 0x500278 */
tdstA3dGENERAL  *p_stA3dGeneral;
tdstA3dChannel  *p_stChannels;
tdstA3dFrame    *p_stFrames;
tdstCouple      *p_stHierarchies;
tdstOnlyFrame   *p_stOnlyFrames;
MTH3D_tdstVector *p_stSpeedVectors;
tdstNTTO        *p_stNTTO;
tdstKeyFrame    *p_stKeyFrames;

#include "GAM/a3x_mem.h"
tdstStackInfos	tdstStacks[eStackNbMax]=
{
	{	0,	0,	0, 0 },
	{	0,	0,	0, 0 },
	{	0,	0,	0, 0 },
	{	0,	0,	0, 0 },
	{	0,	0,	0, 0 },
	{	0,	0,	0, 0 },
	{	0,	0,	0, 0 },
	{	0,	0,	0, 0 },
	{	0,	0,	0, 0 },
	{	0,	0,	0, 0 },
	{	0,	0,	0, 0 },
	{	0,	0,	0, 0 }
};
/* 0x500288 */
void           *GAM_g_pAnimStack500288;
void           *GAM_g_pAnimStack500298;

#define GAM_g_ucMmgModuleId       g_ucGEOModuleId        /* 0x0049fbbc */
#define GAM_g_ucMmgMode           g_ucGEOMMemMallocMode  /* 0x0049fbc0 */
#define GAM_g_bDynamicAllocation  GEO_g_bDynamicAllocation  /* 0x00501524 */

/* 0x4a7104 */
unsigned long GAM_g_ulSkipStackAlloc;

#include "GEO/GeoMem.h"

/* 0x0049F654 */
static const char GAM_AS_szStackSizeErr[] =
    "To Load Animation All stacks must be sized, correct Game.dsc for stack %d";

/* 0x00418AB0 */
extern void __cdecl fn_v_InitSinTab(void);

extern void __cdecl GEO_xSelectMemoryChannel(unsigned char);
void FUN_00406820(void)
{

    unsigned char  ucSavedMode;
    int            iStack;
    unsigned long *pulCount;
    int            iRemaining;

    char szMsg[52];

    if (GAM_g_ulSkipStackAlloc != 0 || FUN_0044f4e0() == 1)
        goto build_sine_table;

    ucSavedMode = GAM_g_ucMmgMode;
    GEO_xSelectMemoryChannel(1);

    iStack     = 0;
    pulCount   = &tdstStacks[eStackA3dGENERAL].ulMaxPos;
    iRemaining = 12;
    do {
        if (*pulCount == 0) {
            sprintf(szMsg, (char *)GAM_AS_szStackSizeErr, iStack);
            ERM_fn_vUpdateLastError(0x30, g_ucGameModuleId, 0, 0x1c, -1, 0xff, 0xff, szMsg);
        }
        iStack++;
        pulCount += sizeof(tdstStackInfos) / sizeof(unsigned long);
        iRemaining--;
    } while (iRemaining != 0);

    GEO_M_CPAMalloc(p_stA3dGeneral, tdstA3dGENERAL *, tdstStacks[eStackA3dGENERAL].ulMaxPos * sizeof(tdstA3dGENERAL), E_uwGEONotEnoughtMemory);
    GEO_M_CPAMalloc(p_stSpeedVectors, MTH3D_tdstVector *, tdstStacks[eStackVertexes].ulMaxPos * sizeof(MTH3D_tdstVector), E_uwGEONotEnoughtMemory);
    GEO_M_CPAMalloc(p_stAngularSpeedQuats, short *, tdstStacks[eStackQuaternions].ulMaxPos * 0x08, E_uwGEONotEnoughtMemory);
    GEO_M_CPAMalloc(p_stHierarchies, tdstCouple *, tdstStacks[eStackHierarchies].ulMaxPos * sizeof(tdstCouple), E_uwGEONotEnoughtMemory);
    GEO_M_CPAMalloc(p_stNTTO, tdstNTTO *, tdstStacks[eStackNTTO].ulMaxPos * sizeof(tdstNTTO), E_uwGEONotEnoughtMemory);
    GEO_M_CPAMalloc(p_stOnlyFrames, tdstOnlyFrame *, tdstStacks[eStackOnlyFrames].ulMaxPos * sizeof(tdstOnlyFrame), E_uwGEONotEnoughtMemory);
    GEO_M_CPAMalloc(p_stChannels, tdstA3dChannel *, tdstStacks[eStackChannels].ulMaxPos * sizeof(tdstA3dChannel), E_uwGEONotEnoughtMemory);
    GEO_M_CPAMalloc(p_stFrames, tdstA3dFrame *, tdstStacks[eStackFrames].ulMaxPos * sizeof(tdstA3dFrame), E_uwGEONotEnoughtMemory);
    GEO_M_CPAMalloc(p_stFramesKF, int *, tdstStacks[eStackFramesKF].ulMaxPos * sizeof(int), E_uwGEONotEnoughtMemory);
    GEO_M_CPAMalloc(p_stKeyFrames, tdstKeyFrame *, tdstStacks[eStackKeyFrames].ulMaxPos * sizeof(tdstKeyFrame), E_uwGEONotEnoughtMemory);
    GEO_M_CPAMalloc(GAM_g_pAnimStack500298, void *, tdstStacks[eStackEvents].ulMaxPos * 0x0c, E_uwGEONotEnoughtMemory);
    GEO_M_CPAMalloc(GAM_g_pAnimStack500288, void *, tdstStacks[eStackMorphData].ulMaxPos * 0x08, E_uwGEONotEnoughtMemory);

    GEO_xSelectMemoryChannel(ucSavedMode);

build_sine_table:
    fn_v_InitSinTab();
}
