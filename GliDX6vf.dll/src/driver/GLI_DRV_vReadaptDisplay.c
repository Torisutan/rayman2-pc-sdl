#include "GliDX6vf/GliDX6vf.h"
#include "GliDX6vf/GliDX6vf_State.h"

__declspec(dllexport) void __cdecl GLI_DRV_vReadaptDisplay(void)
{
    RECT rect;
    LONG screenHeight;
    LONG screenWidth;
    LONG clientHeight;
    LONG clientWidth;

    GetClientRect(GLI_g_WindowHandle, &rect);

    screenHeight = GLI_g_ScreenRect.bottom - GLI_g_ScreenRect.top;
    screenWidth = GLI_g_ScreenRect.right - GLI_g_ScreenRect.left;
    clientHeight = rect.bottom - rect.top;
    clientWidth = rect.right - rect.left;
    if (screenWidth == clientWidth && screenHeight == clientHeight) {
        ClientToScreen(GLI_g_WindowHandle, (POINT *)&rect.left);
        ClientToScreen(GLI_g_WindowHandle, (POINT *)&rect.right);
        GLI_g_ScreenRect = rect;
        return;
    }

    GLI_ReleaseDeviceObjects(0);
    if (GLI_CreateOrResetDevice(GLI_g_WindowHandle, FALSE, 0, 0, 0) == 0) {
        GLI_DestroyOrCleanupDevice();
    }
}
