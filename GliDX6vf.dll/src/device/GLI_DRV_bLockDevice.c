#include "GliDX6vf/GliDX6vf.h"
#include "GliDX6vf/GliDX6vf_State.h"

#define GLI_DDSURFACEDESC2_SIZE 0x7cu
#define GLI_DDSURFACEDESC2_PITCH_OFFSET 0x10u
#define GLI_DDSURFACEDESC2_SURFACE_OFFSET 0x24u
#define GLI_DDLOCK_WAIT_READONLY 0x00000801u

__declspec(dllexport) long __cdecl GLI_DRV_bLockDevice(void **screen, LONG *pitch)
{
    DWORD desc[GLI_DDSURFACEDESC2_SIZE / sizeof(DWORD)];
    HRESULT hr;

    memset(desc, 0, sizeof(desc));
    desc[0] = GLI_DDSURFACEDESC2_SIZE;

    hr = GLI_g_pBackBufferSurface->lpVtbl->Lock(GLI_g_pBackBufferSurface,
                                                NULL,
                                                desc,
                                                GLI_DDLOCK_WAIT_READONLY,
                                                NULL);
    if (hr != 0) {
        return 0;
    }

    *screen = *(void **)((unsigned char *)desc + GLI_DDSURFACEDESC2_SURFACE_OFFSET);
    *pitch = *(LONG *)((unsigned char *)desc + GLI_DDSURFACEDESC2_PITCH_OFFSET);
    return 1;
}
