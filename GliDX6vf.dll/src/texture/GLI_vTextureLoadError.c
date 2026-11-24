#include <stdio.h>

#include "GliDX6vf/GliDX6vf.h"
#include "GliDX6vf/GliDX6vf_State.h"

#define GLI_TEXTURE_ERROR_NAME_OFFSET 0x46

typedef ULONG (STDMETHODCALLTYPE *GLI_RawReleaseFunction)(void *self);

void __cdecl GLI_vTextureLoadError(void *textureRecord, unsigned int hr, char const *errorString)
{
    int releaseResult;
    char message[128];

    sprintf(message, "texture.c :%s : %X -> ", errorString, hr);
    GLI_g_ErmPrintMessage(message, (unsigned char *)textureRecord + GLI_TEXTURE_ERROR_NAME_OFFSET);

    if (GLI_g_pTexTempTexture != NULL) {
        do {
            releaseResult = (int)((GLI_RawReleaseFunction)(*(void ***)GLI_g_pTexTempTexture)[2])(GLI_g_pTexTempTexture);
        } while (releaseResult > 0);
    } else {
        if (GLI_g_pTexTempSurface != NULL) {
            ((GLI_RawReleaseFunction)(*(void ***)GLI_g_pTexTempSurface)[2])(GLI_g_pTexTempSurface);
        }
    }

    if (GLI_g_pTexDestTexture != NULL) {
        do {
            releaseResult = (int)((GLI_RawReleaseFunction)(*(void ***)GLI_g_pTexTempTexture)[2])(GLI_g_pTexDestTexture);
        } while (releaseResult > 0);
    } else {
        if (GLI_g_pTexDestSurface != NULL) {
            ((GLI_RawReleaseFunction)(*(void ***)GLI_g_pTexDestSurface)[2])(GLI_g_pTexDestSurface);
        }
    }
}
