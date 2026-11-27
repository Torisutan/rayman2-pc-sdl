#include "GliDX6vf/GliDX6vf.h"
#include "GliDX6vf/GliDX6vf_State.h"

__declspec(dllexport) void __cdecl GLI_DRV_vSetZClip(float zClip, GLI_tdstInternalGlobalValuesFor3dEngine *p_stGlobaleMT)
{
    p_stGlobaleMT->fZClipping = zClip;
    p_stGlobaleMT->fWClipping = 1.0f / zClip;
}
