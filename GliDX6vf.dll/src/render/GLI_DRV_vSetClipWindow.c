#include "GliDX6vf/GliDX6vf.h"
#include "GliDX6vf/GliDX6vf_State.h"

__declspec(dllexport) void __cdecl GLI_DRV_vSetClipWindow(float x0, float x1, float y0, float y1, GLI_tdstInternalGlobalValuesFor3dEngine *p_stGlobaleMT)
{
    p_stGlobaleMT->fXMinClipping = x0;
    p_stGlobaleMT->fXMaxClipping = x1;
    p_stGlobaleMT->fYMinClipping = y0;
    p_stGlobaleMT->fYMaxClipping = y1;
}
