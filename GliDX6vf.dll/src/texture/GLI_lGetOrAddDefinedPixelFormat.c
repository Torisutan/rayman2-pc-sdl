#include "GliDX6vf/GliDX6vf.h"
#include "GliDX6vf/GliDX6vf_State.h"

unsigned char __cdecl GLI_lGetOrAddDefinedPixelFormat(void *pixelFormat)
{
    long index;
    unsigned char definedFormat[0x40];

#ifdef GLI_DX6_FIXES
    if (GLI_g_GetDefinedPixelFormat == NULL || GLI_g_AddDefinedPixelFormat == NULL) {
        return 0;
    }
#endif

    index = GLI_lFindDefinedPixelFormat(pixelFormat);
    if (index == -1) {
        GLI_vDerivePixelFormatRecord(definedFormat, pixelFormat);
        index = (long)((GLI_AddDefinedPixelFormatFunction)GLI_g_AddDefinedPixelFormat)(definedFormat);
        index &= (index == -1) - 1;
    }
    return (unsigned char)index;
}
