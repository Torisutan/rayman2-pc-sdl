#include "GliDX6vf/GliDX6vf.h"
#include "GliDX6vf/GliDX6vf_State.h"
#include "GliDX6vf/GLI_DiagLog.h"

#define GLI_DDERR_SURFACELOST ((HRESULT)0x887601c2u)
#define GLI_DDERR_WASSTILLDRAWING ((HRESULT)0x8876021cu)
#define GLI_DD_OK ((HRESULT)0)
#define GLI_DDBLT_WAIT 0x01000000u
#define GLI_DDFLIP_WAIT 0x00000001u
#define GLI_DDGBS_ISBLTDONE 0x00000001u
#define GLI_GLICAPS_SOFTWARE_SYNC_FLAG 0x40u

__declspec(dllexport) void __cdecl GLI_DRV_vFlipDeviceWithSyncro(void)
{
    HRESULT hr;
    int waitFrames;
    int i;
    int bltIter;

    if (GLI_g_IsGliInit() == 0 || GLI_g_ActivateGliFlag == 0) {
        GLI_DiagLog("vFlipDeviceWithSyncro SKIP — init=%d activate=%ld",
                    GLI_g_IsGliInit(), (long)GLI_g_ActivateGliFlag);
        return;
    }

    GLI_DiagLog("vFlipDeviceWithSyncro ENTER fs=%d primary=%p back=%p caps4=0x%02x",
                GLI_g_IsFullscreen, (void *)GLI_g_pPrimarySurface,
                (void *)GLI_g_pBackBufferSurface,
                GLI_g_GliCaps ? ((unsigned char *)GLI_g_GliCaps)[4] : 0);

    *(DWORD *)((unsigned char *)GLI_g_GliBigGlobals + 0x2d3fc) = 0;
    *(DWORD *)((unsigned char *)GLI_g_GliBigGlobals + 0x2d404) = 0;

    hr = GLI_g_pBackBufferSurface->lpVtbl->GetFlipStatus(GLI_g_pBackBufferSurface, GLI_DDGBS_ISBLTDONE);
    while (hr == GLI_DDERR_WASSTILLDRAWING) {
        hr = GLI_g_pBackBufferSurface->lpVtbl->GetFlipStatus(GLI_g_pBackBufferSurface, GLI_DDGBS_ISBLTDONE);
    }
    GLI_DiagLog("vFlipDeviceWithSyncro GetFlipStatus done hr=0x%08x", (unsigned int)hr);

    if (GLI_g_pPrimarySurface == NULL) {
        GLI_DiagLog("vFlipDeviceWithSyncro EXIT — primary NULL");
        return;
    }

    bltIter = 0;
    do {
        if (GLI_g_IsFullscreen != 0) {
            hr = GLI_g_pPrimarySurface->lpVtbl->Flip(GLI_g_pPrimarySurface, NULL, GLI_DDFLIP_WAIT);
        } else {
            hr = GLI_g_pPrimarySurface->lpVtbl->Blt(GLI_g_pPrimarySurface,
                                                   &GLI_g_ScreenRect,
                                                   GLI_g_pBackBufferSurface,
                                                   &GLI_g_ClientRect,
                                                   GLI_DDBLT_WAIT,
                                                   NULL);
        }

        if (bltIter < 4) {
            GLI_DiagLog("vFlipDeviceWithSyncro %s iter=%d hr=0x%08x",
                        GLI_g_IsFullscreen ? "Flip" : "Blt", bltIter, (unsigned int)hr);
        } else if (bltIter == 4) {
            GLI_DiagLog("vFlipDeviceWithSyncro present loop still spinning (hr=0x%08x) — further iters muted",
                        (unsigned int)hr);
        }
        ++bltIter;

        if (hr == GLI_DDERR_SURFACELOST) {
            GLI_DiagLog("vFlipDeviceWithSyncro SURFACELOST -> DestroyOrCleanupDevice");
            GLI_DestroyOrCleanupDevice();
        }
    } while (hr != GLI_DD_OK);

    if ((((unsigned char *)GLI_g_GliCaps)[4] & GLI_GLICAPS_SOFTWARE_SYNC_FLAG) != 0) {
        GLI_DiagLog("vFlipDeviceWithSyncro SimulateFlipSynchroWithoutVSync(1)");
        GLI_g_SimulateFlipSynchroWithoutVSync(1);
    } else {
        GLI_DiagLog("vFlipDeviceWithSyncro WaitForVerticalBlank");
        GLI_g_pDirectDraw4->lpVtbl->WaitForVerticalBlank(GLI_g_pDirectDraw4, 1, NULL);
    }

    waitFrames = GLI_g_ComputeWaitFrameForSmoothSynchro(0) - 1;
    GLI_DiagLog("vFlipDeviceWithSyncro waitFrames=%d", waitFrames);
    if (waitFrames != 0) {
        if ((((unsigned char *)GLI_g_GliCaps)[4] & GLI_GLICAPS_SOFTWARE_SYNC_FLAG) != 0) {
            GLI_g_SimulateFlipSynchroWithoutVSync(waitFrames);
            GLI_DiagLog("vFlipDeviceWithSyncro EXIT (sim wait)");
            return;
        }

        i = waitFrames;
        while (i--)
            GLI_g_pDirectDraw4->lpVtbl->WaitForVerticalBlank(GLI_g_pDirectDraw4, 1, NULL);
    }
    GLI_DiagLog("vFlipDeviceWithSyncro EXIT");
}
