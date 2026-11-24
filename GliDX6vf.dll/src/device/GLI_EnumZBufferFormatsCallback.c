#include "GliDX6vf/GliDX6vf.h"
#include "GliDX6vf/GliDX6vf_State.h"

HRESULT CALLBACK GLI_EnumZBufferFormatsCallback(void *pixelFormat, void *context)
{
    DWORD *src;
    DWORD *dst;

    src = (DWORD *)pixelFormat;
    dst = (DWORD *)context;
    if (src[1] == GLI_DDPF_ZBUFFER) {
        memcpy(dst, src, 8 * sizeof(DWORD));
        return 0;
    }
    return 1;
}
