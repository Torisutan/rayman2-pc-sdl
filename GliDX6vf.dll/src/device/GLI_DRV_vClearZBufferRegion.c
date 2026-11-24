#include "GliDX6vf/GliDX6vf.h"
#include "GliDX6vf/GliDX6vf_State.h"

#define GLI_GMT_CLEAR_ZBUFFER_BLOCK_OFFSET 0x2d400

__declspec(dllexport) void __cdecl
GLI_DRV_vClearZBufferRegion(LONG x0, LONG x1, LONG y0, LONG y1)
{
    unsigned char *globals = (unsigned char *)GLI_g_GliBigGlobals;

    (void)x0;
    (void)x1;
    (void)y0;
    (void)y1;

    if (*(int *)(globals + GLI_GMT_CLEAR_ZBUFFER_BLOCK_OFFSET) == 0) {
        GLI_DRV_vClearDevice(1, 0, 0);
        GLI_SetBeginSceneOpaqueTextureState();
    }
}
