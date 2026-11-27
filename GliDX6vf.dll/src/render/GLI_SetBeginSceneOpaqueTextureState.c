#include "GliDX6vf/GliDX6vf.h"
#include "GliDX6vf/GliDX6vf_State.h"
#include "GliDX6vf/GLI_DX6Constants.h"

void __cdecl GLI_SetBeginSceneOpaqueTextureState(void)
{
    GLI_EnsureZWriteEnabled();
    GLI_DRV_M_SetRenderState(GLI_D3DRS_ALPHABLENDENABLE, 0);
    GLI_DRV_M_SetTextureStageState(0, GLI_D3DTSS_COLOROP, GLI_D3DTOP_MODULATE);
    GLI_DRV_M_SetTextureStageState(0, GLI_D3DTSS_COLORARG1, GLI_D3DTA_TEXTURE);
    GLI_DRV_M_SetTextureStageState(0, GLI_D3DTSS_COLORARG2, GLI_D3DTA_DIFFUSE);
    GLI_DRV_M_SetTextureStageState(0, GLI_D3DTSS_ALPHAOP, GLI_D3DTOP_MODULATE);
    GLI_DRV_M_SetTextureStageState(0, GLI_D3DTSS_ALPHAARG1, GLI_D3DTA_TEXTURE);
    GLI_DRV_M_SetTextureStageState(0, GLI_D3DTSS_ALPHAARG2, GLI_D3DTA_DIFFUSE);
    GLI_DRV_M_SetRenderState(GLI_D3DRS_ALPHATESTENABLE, 0);
    GLI_g_TextureStageColorOp = 4;
}
