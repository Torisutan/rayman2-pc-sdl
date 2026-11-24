#include "GliDX6vf/GliDX6vf.h"
#include "GliDX6vf/GliDX6vf_State.h"
#include "GliDX6vf/GLI_DiagLog.h"

int __cdecl GLI_DestroyOrCleanupDevice(void)
{
    HRESULT hr;

    GLI_DiagLog("DestroyOrCleanupDevice ENTER primary=%p back=%p z=%p",
                (void *)GLI_g_pPrimarySurface, (void *)GLI_g_pBackBufferSurface,
                (void *)GLI_g_pZBufferSurface);

    if (GLI_g_pPrimarySurface != NULL) {
        hr = GLI_g_pPrimarySurface->lpVtbl->IsLost(GLI_g_pPrimarySurface);
        if (hr != 0) {
            GLI_DiagLog("DestroyOrCleanupDevice Restore surf=%p (IsLost hr=0x%08x)",
                        (void *)GLI_g_pPrimarySurface, (unsigned int)hr);
            GLI_g_pPrimarySurface->lpVtbl->Restore(GLI_g_pPrimarySurface);
        }
    }
    if (GLI_g_pBackBufferSurface != NULL) {
        hr = GLI_g_pBackBufferSurface->lpVtbl->IsLost(GLI_g_pBackBufferSurface);
        if (hr != 0) {
            GLI_DiagLog("DestroyOrCleanupDevice Restore surf=%p (IsLost hr=0x%08x)",
                        (void *)GLI_g_pBackBufferSurface, (unsigned int)hr);
            GLI_g_pBackBufferSurface->lpVtbl->Restore(GLI_g_pBackBufferSurface);
        }
    }
    if (GLI_g_pZBufferSurface != NULL) {
        hr = GLI_g_pZBufferSurface->lpVtbl->IsLost(GLI_g_pZBufferSurface);
        if (hr != 0) {
            GLI_DiagLog("DestroyOrCleanupDevice Restore surf=%p (IsLost hr=0x%08x)",
                        (void *)GLI_g_pZBufferSurface, (unsigned int)hr);
            GLI_g_pZBufferSurface->lpVtbl->Restore(GLI_g_pZBufferSurface);
        }
    }
    GLI_g_ReloadTextures();
    GLI_DiagLog("DestroyOrCleanupDevice EXIT (textures reloaded)");
    return 0;
}
