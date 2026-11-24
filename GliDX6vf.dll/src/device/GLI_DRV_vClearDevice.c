#include "GliDX6vf/GliDX6vf.h"
#include "GliDX6vf/GliDX6vf_State.h"
#include "GliDX6vf/GLI_DiagLog.h"

#define GLI_D3DCLEAR_TARGET 0x00000001u
#define GLI_D3DCLEAR_ZBUFFER 0x00000002u

__declspec(dllexport) void __cdecl
GLI_DRV_vClearDevice(int clearZBuffer, int clearColor, unsigned int color)
{
    DWORD flags;
    DWORD clearColorValue;

    if (GLI_g_IsGliInit() == 0) {
        GLI_DiagLog("vClearDevice SKIP — !IsGliInit");
        return;
    }
    GLI_DiagLog("vClearDevice z=%d c=%d color=0x%08x viewport=%p",
                clearZBuffer, clearColor, color, (void *)GLI_g_pDirect3DViewport3);

    clearColorValue = ((color & 0x000000ffu) << 16) |
                      (color & 0x0000ff00u) |
                      ((color >> 16) & 0x000000ffu);

    flags = ((clearZBuffer != 0) ? GLI_D3DCLEAR_ZBUFFER : 0u) |
            ((clearColor != 0) ? GLI_D3DCLEAR_TARGET : 0u);

    GLI_g_pDirect3DViewport3->lpVtbl->Clear2(GLI_g_pDirect3DViewport3,
                                             1,
                                             &GLI_g_ClientRect,
                                             flags,
                                             clearColorValue,
                                             0.0f,
                                             0);
}
