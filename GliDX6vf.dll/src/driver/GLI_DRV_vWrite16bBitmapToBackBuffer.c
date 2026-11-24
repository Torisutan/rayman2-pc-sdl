#include "GliDX6vf/GliDX6vf.h"
#include "GliDX6vf/GliDX6vf_State.h"

#include <string.h>

#define DDSD_SIZE_OFFSET      0x00
#define DDSD_PITCH_OFFSET     0x10
#define DDSD_SURFACE_OFFSET   0x24

#define DDSD_PITCH(d)         (*(LONG *)((unsigned char *)(d) + DDSD_PITCH_OFFSET))

#define GLI_DDLOCK_WAIT_NOSYSLOCK   0x801

__declspec(dllexport) void __cdecl
GLI_DRV_vWrite16bBitmapToBackBuffer(void *pSrc, LONG lW, LONG lH,
                                    LONG lDestLeft, LONG lDestTop,
                                    LONG lDestRight, LONG lDestBottom)
{
    DWORD          desc[0x1f];
    HRESULT        hr;
    unsigned char *surface;

    memset(desc, 0, sizeof(desc));
    *(DWORD *)((unsigned char *)desc + DDSD_SIZE_OFFSET) = 0x7c;
    hr = GLI_g_pBackBufferSurface->lpVtbl->Lock(
            GLI_g_pBackBufferSurface, NULL, desc, GLI_DDLOCK_WAIT_NOSYSLOCK, NULL);
    if (hr != 0) {
        GLI_DestroyOrCleanupDevice();   /* 0x1000acb0 */
        memset(desc, 0, sizeof(desc));
        *(DWORD *)((unsigned char *)desc + DDSD_SIZE_OFFSET) = 0x7c;
        hr = GLI_g_pBackBufferSurface->lpVtbl->Lock(
                GLI_g_pBackBufferSurface, NULL, desc, GLI_DDLOCK_WAIT_NOSYSLOCK, NULL);
        if (hr != 0) {
            return;
        }
    }

    surface = *(unsigned char **)((unsigned char *)desc + DDSD_SURFACE_OFFSET);

    if (GLI_g_SavedBpp == 0x20) {

        const unsigned char *src = (const unsigned char *)pSrc;
        unsigned char       *dst = surface;
        LONG row, col;

        DDSD_PITCH(desc) -= lW * 4;
        for (row = lH; row > 0; row--) {
            for (col = lW; col > 0; col--) {
                *dst++ = *src++;
                *dst++ = *src++;
                *dst++ = *src++;
                dst++;
            }
        }
    } else {

        GLI_g_BlitStretched16b(
            (lDestRight  - lDestLeft) + 1,
            (lDestBottom - lDestTop)  + 1,
            surface + (lDestTop * (DDSD_PITCH(desc) >> 1) + lDestLeft) * 2,
            DDSD_PITCH(desc) >> 1,
            lW,
            lH,
            pSrc);
    }

    GLI_g_pBackBufferSurface->lpVtbl->Unlock(GLI_g_pBackBufferSurface, NULL);
}
