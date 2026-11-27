#include "GliDX6vf/GliDX6vf.h"
#include "GliDX6vf/GliDX6vf_State.h"
#include "GliDX6vf/GLI_DX6Constants.h"

void __cdecl GLI_DisableZWriteIfEnabled(void)
{
    if (GLI_g_ZWriteEnableState != 0) {
        GLI_DRV_M_SetRenderState(GLI_D3DRS_ZWRITEENABLE, 0);
        GLI_g_ZWriteEnableState = 0;
    }
}
