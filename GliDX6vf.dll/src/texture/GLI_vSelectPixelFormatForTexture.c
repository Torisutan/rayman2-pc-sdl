#include "GliDX6vf/GliDX6vf.h"
#include "GliDX6vf/GliDX6vf_State.h"

void __cdecl GLI_vSelectPixelFormatForTexture(unsigned int textureFlags, void *pixelFormat)
{
    if (GLI_g_SavedBpp == 0x20) {
        if ((textureFlags & 0x80) != 0) {
            GLI_vGetSelectedTextureFormat(3, pixelFormat);
            return;
        }
        if ((textureFlags & 0x118) != 0) {
            GLI_vGetSelectedTextureFormat(5, pixelFormat);
            return;
        }
        GLI_vGetSelectedTextureFormat(4, pixelFormat);
        return;
    }

    if (GLI_g_SavedBpp == 0x10) {
        if ((textureFlags & 0x80) != 0) {
            GLI_vGetSelectedTextureFormat(3, pixelFormat);
            return;
        }
        if ((textureFlags & 0x8) != 0) {
            GLI_vGetSelectedTextureFormat(0, pixelFormat);
            return;
        }
        if ((textureFlags & 0x110) != 0) {
            GLI_vGetSelectedTextureFormat(1, pixelFormat);
            return;
        }
        GLI_vGetSelectedTextureFormat(2, pixelFormat);
    }
}
