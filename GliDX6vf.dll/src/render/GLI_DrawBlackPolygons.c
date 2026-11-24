#include "GliDX6vf/GliDX6vf.h"
#include "GliDX6vf/GliDX6vf_State.h"
#include "GliDX6vf/GLI_DX6Constants.h"

void __cdecl GLI_DrawBlackPolygons(void)
{
    if (GLI_g_BlackPolygonVertexCount != 0) {
        GLI_g_pDirect3DDevice3->lpVtbl->SetTexture(GLI_g_pDirect3DDevice3, 0, NULL);
        GLI_g_pDirect3DDevice3->lpVtbl->SetRenderState(GLI_g_pDirect3DDevice3, GLI_D3DRS_FOGENABLE, 0);
        GLI_g_FogEnableState = 0;
        GLI_g_pDirect3DDevice3->lpVtbl->DrawPrimitive(GLI_g_pDirect3DDevice3,
                                                      GLI_D3DPT_TRIANGLELIST,
                                                      GLI_D3DFVF_TLVERTEX,
                                                      GLI_g_BlackPolygonVertices,
                                                      GLI_g_BlackPolygonVertexCount,
                                                      GLI_D3DDP_DONOTCLIP | GLI_D3DDP_DONOTLIGHT);
    }
}
