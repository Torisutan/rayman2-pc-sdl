#include "GliDX6vf/GliDX6vf.h"
#include "GliDX6vf/GliDX6vf_State.h"

long __cdecl GLI_DRV_hChangeMode(BOOL fullscreen, LONG width, LONG height, LONG bpp)
{
    int result;

    result = GLI_g_IsGliInit();
    if (result == 0) {
        return 0;
    }

    GLI_ReleaseDeviceObjects(0);
    result = GLI_CreateOrResetDevice(GLI_g_WindowHandle, fullscreen, width, height, bpp);
    if (result == 0) {
        GLI_DestroyOrCleanupDevice();
    }
    return result;
}
