#include "GliDX6vf/GliDX6vf.h"
#include "GliDX6vf/GliDX6vf_State.h"

#define GLI_DDFLIP_WAIT 0x00000001u

__declspec(dllexport) void __cdecl GLI_DRV_vPrepareForGliFullScreen(HWND hwnd)
{
    if (GLI_g_IsGliInit() == 0) {
        return;
    }

    GLI_ReleaseDeviceObjects(0);
    GLI_g_WindowHandle = hwnd;
    GLI_CreateOrResetDevice(hwnd, TRUE, GLI_g_SavedWidth, GLI_g_SavedHeight, GLI_g_SavedBpp);
    GLI_DRV_vClearDevice(0, 1, 0);
    GLI_g_pPrimarySurface->lpVtbl->Flip(GLI_g_pPrimarySurface, NULL, GLI_DDFLIP_WAIT);
    GLI_g_ReloadTextures();
}
