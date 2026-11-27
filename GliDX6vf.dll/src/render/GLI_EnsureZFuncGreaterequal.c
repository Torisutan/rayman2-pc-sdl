#include "GliDX6vf/GliDX6vf.h"
#include "GliDX6vf/GliDX6vf_State.h"
#include "GliDX6vf/GLI_DX6Constants.h"

void __cdecl GLI_EnsureZFuncGreaterequal(void)
{
    if (GLI_g_ZFuncGreaterequalState == 0) {
        GLI_DRV_M_SetRenderState(GLI_D3DRS_ZFUNC, GLI_D3DCMP_GREATEREQUAL);
        GLI_g_ZFuncGreaterequalState = 1;
    }
}
