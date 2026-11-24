#include "GliDX6vf/GliDX6vf.h"
#include "GliDX6vf/GliDX6vf_State.h"
#include "GliDX6vf/GLI_DiagLog.h"

__declspec(dllexport) long __cdecl GLI_DRV_bBeginScene(void)
{
    HRESULT hr;

    GLI_DiagNextFrame();
    GLI_M_DiagResetSends();

    hr = GLI_g_pDirect3DDevice3->lpVtbl->BeginScene(GLI_g_pDirect3DDevice3);
    GLI_DiagLog("bBeginScene BeginScene hr=0x%08x device=%p", (unsigned int)hr,
                (void *)GLI_g_pDirect3DDevice3);
    if (hr >= 0) {
        GLI_SetBeginSceneOpaqueTextureState();
        GLI_DrawBlackPolygons();
        return 1;
    }

    return 0;
}
