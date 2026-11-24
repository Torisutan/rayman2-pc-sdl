#include "GliDX6vf/GliDX6vf.h"
#include "GliDX6vf/GliDX6vf_State.h"
#include "GliDX6vf/GLI_DiagLog.h"

HRESULT WINAPI DirectDrawCreate(
    GUID *lpGUID,
    GLI_IDirectDraw **lplpDD,
    void *pUnkOuter);

static const GUID GLI_g_IID_IDirectDraw4 = {
    0x9c59509a,
    0x39bd,
    0x11d1,
    {0x8c, 0x4a, 0x00, 0xc0, 0x4f, 0xd9, 0x30, 0xc5}
};

int __cdecl GLI_InitDirectDraw(void)
{
    HRESULT hr;

    hr = DirectDrawCreate(&GLI_g_DirectDrawGuid, &GLI_g_pDirectDraw, NULL);
    GLI_DiagLog("InitDirectDraw DirectDrawCreate hr=0x%08x pDD=%p",
                (unsigned int)hr, (void *)GLI_g_pDirectDraw);
    if (hr == 0) {
        hr = GLI_g_pDirectDraw->lpVtbl->QueryInterface(
            GLI_g_pDirectDraw,
            &GLI_g_IID_IDirectDraw4,
            (void **)&GLI_g_pDirectDraw4);
        GLI_DiagLog("InitDirectDraw QueryInterface(IDirectDraw4) hr=0x%08x pDD4=%p",
                    (unsigned int)hr, (void *)GLI_g_pDirectDraw4);
        if (hr >= 0) {
            if (GLI_g_pDirectDraw != NULL) {
                GLI_g_pDirectDraw->lpVtbl->Release(GLI_g_pDirectDraw);
                GLI_g_pDirectDraw = NULL;
            }
            GLI_g_HalCaps.dwSize = GLI_DDCAPS_DX6_SIZE;
            GLI_g_pDirectDraw4->lpVtbl->GetCaps(GLI_g_pDirectDraw4, &GLI_g_HalCaps, NULL);
            hr = 0;
        }
    }
    return hr;
}
