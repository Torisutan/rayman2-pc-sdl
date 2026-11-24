#include "GliDX6vf/GliDX6vf.h"
#include "GliDX6vf/GliDX6vf_State.h"

__declspec(dllexport) void __cdecl GLI_DRV_vNoBlackPolygon(void)
{
    GLI_g_BlackPolygonVertexCount = 0;
}
