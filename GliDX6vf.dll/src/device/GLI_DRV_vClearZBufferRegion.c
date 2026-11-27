#include "GliDX6vf/GliDX6vf.h"
#include "GliDX6vf/GliDX6vf_State.h"

__declspec(dllexport) void __cdecl
GLI_DRV_vClearZBufferRegion(LONG x0, LONG x1, LONG y0, LONG y1)
{
    GLI_tdstInternalGlobalValuesFor3dEngine *globals = GLI_g_GliBigGlobals;

    (void)x0;
    (void)x1;
    (void)y0;
    (void)y1;

    if (globals->bDrawMirrorSymetric == 0) {
        GLI_DRV_vClearDevice(1, 0, 0);
        GLI_SetBeginSceneOpaqueTextureState();
    }
}
