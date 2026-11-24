#include "GliDX6vf/GliDX6vf.h"
#include "GliDX6vf/GliDX6vf_State.h"
#include "GliDX6vf/GLI_DiagLog.h"

#define GLI_TEXTURE_CHANNEL_EMPTY 0xc0de0005u

void __cdecl GLI_DRV_vUnLoadTextures(void)
{
    int i;

    GLI_DiagLog("vUnLoadTextures ENTER");

    if (GLI_g_pDirect3DDevice3 != NULL) {
        GLI_g_pDirect3DDevice3->lpVtbl->SetTexture(GLI_g_pDirect3DDevice3, 0, NULL);
    }

    for (i = 0; i < 0x400; i++) {
        unsigned char *entry;
        GLI_IUnknownVtbl **texture;

        if (GLI_g_TextureMemoryChannels[i] == GLI_TEXTURE_CHANNEL_EMPTY) {
            continue;
        }
        entry = ((unsigned char **)GLI_g_TextureAlreadyRead)[i];
        if (entry == NULL || *(DWORD *)entry == 0) {
            continue;
        }
        texture = *(GLI_IUnknownVtbl ***)(entry + 0x10);
        if (texture != NULL) {
            ULONG refs;
            do {
                refs = (*texture)->Release(texture);
            } while ((LONG)refs > 0);
            *(void **)(entry + 0x10) = NULL;
            *(DWORD *)entry = 0;
        }
    }
    GLI_DiagLog("vUnLoadTextures EXIT");
}
