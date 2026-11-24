#include "GliDX6vf/GliDX6vf.h"

__declspec(dllexport) void __cdecl GLI_DRV_vCloseDriver(void)
{
    GLI_ReleaseDeviceObjects(1);
}
