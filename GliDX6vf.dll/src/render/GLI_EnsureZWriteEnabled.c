#include "GliDX6vf/GliDX6vf.h"
#include "GliDX6vf/GliDX6vf_State.h"
#include "GliDX6vf/GLI_DX6Constants.h"

void __cdecl GLI_EnsureZWriteEnabled(void)
{
    if (GLI_g_ZWriteEnableState == 0) {
        GLI_g_pDirect3DDevice3->lpVtbl->SetRenderState(GLI_g_pDirect3DDevice3, GLI_D3DRS_ZWRITEENABLE, 1);
        GLI_g_ZWriteEnableState = 1;
    }
}
