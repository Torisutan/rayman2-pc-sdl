#include "GliDX6vf/GliDX6vf.h"
#include "GliDX6vf/GliDX6vf_State.h"

#include <string.h>

void __cdecl GLI_vGetSelectedTextureFormat(char formatIndex, void *pixelFormat)
{
    signed char index;
    signed char selected;

    index = (signed char)formatIndex;
    if (index < GLI_TEXTURE_FORMAT_SELECTED_COUNT) {
        selected = GLI_g_SelectedTextureFormats[index];
        if (selected != -1
#ifdef GLI_DX6_FIXES
            && selected < GLI_TEXTURE_FORMAT_RECORD_COUNT
#endif
        ) {
            memcpy(pixelFormat, GLI_g_TextureFormats[selected], GLI_TEXTURE_FORMAT_RECORD_SIZE);
            return;
        }
    }

    memcpy(pixelFormat, GLI_g_TextureFormats[0], GLI_TEXTURE_FORMAT_RECORD_SIZE);
}
