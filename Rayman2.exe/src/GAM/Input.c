#include "AI/AI_types.h"
#include "AI/AI_globals.h"

#include <stdlib.h>

static const unsigned long AI_pc_C_xPadScaleBits = 0x38000100u;   /* 0x0049C32C */
#define AI_pc_C_xPadScale (*(const volatile float *)&AI_pc_C_xPadScaleBits)

void __cdecl fnInvertPad(void)
{
    AI_PAD_lCurXValue = -AI_PAD_lCurXValue;
    AI_PAD_lCurYValue = -AI_PAD_lCurYValue;
}

void __cdecl fnRandomPad(void)
{
    int    iRand2;
    double xFactor;

    iRand2  = rand() << 1;
    xFactor = (double)iRand2 * (double)AI_pc_C_xPadScale;
    AI_PAD_lCurXValue = (long)((double)AI_PAD_lCurXValue * xFactor);
    AI_PAD_lCurYValue = (long)((double)AI_PAD_lCurYValue * xFactor);
}
#include "GAM/GAM_types.h"
#include "GAM/STRUCTUR/Engmode.h"
void __cdecl IPT_fn_vReadInput(void);
void __cdecl fn_vAnalyse3DOSSpecialInputActions(void);

void __cdecl fn_vEngineReadInput(void)
{
	if (GAM_g_stEngineStructure.eInputMode==IM_Normal)
	{
		IPT_fn_vReadInput();
		fn_vAnalyse3DOSSpecialInputActions();

	}
}

/* 0x0049F9F0 */
void (__cdecl *AI_pc_g_a2_pfnPadCheat[2])(void) =
{
    fnInvertPad,
    fnRandomPad
};
