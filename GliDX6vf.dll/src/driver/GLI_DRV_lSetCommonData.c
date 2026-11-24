#include "GliDX6vf/GliDX6vf.h"
#include "GliDX6vf/GliDX6vf_State.h"

#include <string.h>

long __cdecl GLI_DRV_lSetCommonData(char const *name, void *data)
{
    if (strcmp(name, "GliBigGlobals") == 0) {
        GLI_g_GliBigGlobals = data;
        return 1;
    }
    if (strcmp(name, "TextureMemoryChannels") == 0) {
        GLI_g_TextureMemoryChannels = (DWORD *)data;
        return 1;
    }
    if (strcmp(name, "TextureAlreadyRead") == 0) {
        GLI_g_TextureAlreadyRead = (void **)data;
        return 1;
    }
    if (strcmp(name, "ConvertBufferMipMapping") == 0) {
        GLI_g_ConvertBufferMipMapping = data;
        return 1;
    }
    if (strcmp(name, "RenderingCounterPointer") == 0) {
        GLI_g_RenderingCounterPointer = data;
        return 1;
    }
    if (strcmp(name, "DrawSem") == 0) {
        GLI_g_DrawSem = data;
        return 1;
    }
    if (strcmp(name, "GliCaps") == 0) {
        GLI_g_GliCaps = data;
        return 1;
    }
    if (strcmp(name, "NumberOfTextureToCreate") == 0) {
        GLI_g_NumberOfTextureToCreate = data;
        return 1;
    }
    return 0;
}
