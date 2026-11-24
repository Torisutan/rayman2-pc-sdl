#include "GliDX6vf/GliDX6vf.h"
#include "GliDX6vf/GliDX6vf_State.h"

unsigned int __cdecl GLI_lDeviceUsesVideoMemory(void)
{
    return GLI_g_DeviceDescFlags & 1u;
}
