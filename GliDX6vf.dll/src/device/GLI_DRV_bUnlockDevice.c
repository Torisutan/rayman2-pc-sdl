#include "GliDX6vf/GliDX6vf.h"
#include "GliDX6vf/GliDX6vf_State.h"

__declspec(dllexport) long __cdecl GLI_DRV_bUnlockDevice(void)
{
    return GLI_g_pBackBufferSurface->lpVtbl->Unlock(GLI_g_pBackBufferSurface, NULL) == 0;
}
