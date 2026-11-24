#include "GliDX6vf/GliDX6vf.h"
#include "GliDX6vf/GliDX6vf_State.h"

#define GLI_DDSCL_NORMAL 0x00000008u

int __cdecl GLI_ReleaseDeviceObjects(int releaseDirectDraw)
{
    int result;

    GLI_DRV_vUnLoadTextures();
    if (GLI_g_pDirect3DViewport3 != NULL) {
        result = (int)GLI_g_pDirect3DViewport3->lpVtbl->Release(GLI_g_pDirect3DViewport3);
        GLI_g_pDirect3DViewport3 = NULL;
    } else {
        result = releaseDirectDraw;
    }
    if (GLI_g_pDirect3DDevice3 != NULL) {
        result = (int)GLI_g_pDirect3DDevice3->lpVtbl->Release(GLI_g_pDirect3DDevice3);
        GLI_g_pDirect3DDevice3 = NULL;
    }
    if (GLI_g_pZBufferSurface != NULL) {
        result = (int)GLI_g_pZBufferSurface->lpVtbl->Release(GLI_g_pZBufferSurface);
        GLI_g_pZBufferSurface = NULL;
    }
    if (GLI_g_pBackBufferSurface != NULL) {
        result = (int)GLI_g_pBackBufferSurface->lpVtbl->Release(GLI_g_pBackBufferSurface);
        GLI_g_pBackBufferSurface = NULL;
    }
    if (GLI_g_pPrimarySurface != NULL) {
        result = (int)GLI_g_pPrimarySurface->lpVtbl->Release(GLI_g_pPrimarySurface);
        GLI_g_pPrimarySurface = NULL;
    }
    if (GLI_g_pDirect3D3 != NULL) {
        result = (int)GLI_g_pDirect3D3->lpVtbl->Release(GLI_g_pDirect3D3);
        GLI_g_pDirect3D3 = NULL;
    }
    if ((releaseDirectDraw & 0xff) != 0) {
        GLI_g_pDirectDraw4->lpVtbl->SetCooperativeLevel(GLI_g_pDirectDraw4, GLI_g_WindowHandle, GLI_DDSCL_NORMAL);
        if (GLI_g_pDirectDraw4 != NULL) {
#ifdef GLI_DX6_FIXES
            result = (int)GLI_g_pDirectDraw4->lpVtbl->Release(GLI_g_pDirectDraw4);
#else

            do
                result = (int)GLI_g_pDirectDraw4->lpVtbl->Release(GLI_g_pDirectDraw4);
            while (result > 0);
#endif
            GLI_g_pDirectDraw4 = NULL;
        }
    }
    return result;
}
