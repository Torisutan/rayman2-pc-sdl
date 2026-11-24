#include "GliDX6vf/GliDX6vf.h"
#include "GliDX6vf/GliDX6vf_State.h"

#define GLI_TEXTURE_MEM_LOCAL_REQUEST 0x10405000u
#define GLI_TEXTURE_MEM_NONLOCAL_REQUEST 0x20405000u
#define GLI_TEXTURE_MEM_LOCAL_RESULT 0x10004000u
#define GLI_TEXTURE_MEM_NONLOCAL_RESULT 0x20004000u
#define GLI_TEXTURE_MEM_FALLBACK_RESULT 0x00000800u
#define GLI_TEXTURE_MEM_LOCAL_MARGIN 0x1400u

unsigned long __cdecl GLI_ulGetTextureSurfaceCaps(void *texture, int textureMode, int allowNonLocal)
{
    DWORD total;
    DWORD available;
    DWORD required;

    required = (DWORD)GLI_DRV_lGetSizeOfTexture(texture);
    if (textureMode == 0 || textureMode == 1 || textureMode == 2 || textureMode == 3) {
        GLI_QueryAvailableVidMem(GLI_g_pDirectDraw4, GLI_TEXTURE_MEM_LOCAL_REQUEST, &total, &available);
        if (available > required + GLI_TEXTURE_MEM_LOCAL_MARGIN) {
            return GLI_TEXTURE_MEM_LOCAL_RESULT;
        }

        GLI_QueryAvailableVidMem(GLI_g_pDirectDraw4, GLI_TEXTURE_MEM_NONLOCAL_REQUEST, &total, &available);
        if (allowNonLocal != 0 && available > required) {
            return GLI_TEXTURE_MEM_NONLOCAL_RESULT;
        }
    } else {
        GLI_QueryAvailableVidMem(GLI_g_pDirectDraw4, GLI_TEXTURE_MEM_NONLOCAL_REQUEST, &total, &available);
        if (allowNonLocal != 0 && available > required) {
            return GLI_TEXTURE_MEM_NONLOCAL_RESULT;
        }

        GLI_QueryAvailableVidMem(GLI_g_pDirectDraw4, GLI_TEXTURE_MEM_LOCAL_REQUEST, &total, &available);
        if (available > required + GLI_TEXTURE_MEM_LOCAL_MARGIN) {
            return GLI_TEXTURE_MEM_LOCAL_RESULT;
        }
    }

    return GLI_TEXTURE_MEM_FALLBACK_RESULT;
}
