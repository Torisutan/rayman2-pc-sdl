#include "GliDX6vf/GliDX6vf.h"
#include "GliDX6vf/GliDX6vf_State.h"

__declspec(dllexport) long __cdecl GLI_DRV_bPrepareForGliWindowed(HWND hwnd)
{
    HRESULT result;

    if (GLI_g_IsGliInit() == 0) {
        return 0;
    }

    GLI_ReleaseDeviceObjects(0);
    GLI_g_WindowHandle = hwnd;
    result = GLI_CreateOrResetDevice(hwnd, FALSE, 0, 0, 0);
    return result == 0;
}
