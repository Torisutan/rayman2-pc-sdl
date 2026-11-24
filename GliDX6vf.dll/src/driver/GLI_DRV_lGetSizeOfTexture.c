#include "GliDX6vf/GliDX6vf.h"
#include "GliDX6vf/GliDX6vf_State.h"

#include <string.h>

#define GLI_TEXTURE_FLAGS_OFFSET 0x14
#define GLI_TEXTURE_WIDTH_OFFSET 0x18
#define GLI_TEXTURE_HEIGHT_OFFSET 0x1a
#define GLI_TEXTURE_FIELD_30_OFFSET 0x30
#define GLI_TEXTURE_COST_TABLE_DWORD_COUNT \
    (GLI_TEXTURE_COST_FORMAT_COUNT * GLI_TEXTURE_COST_PLANE_COUNT * \
     GLI_TEXTURE_COST_DIMENSION_COUNT * GLI_TEXTURE_COST_DIMENSION_COUNT)

long __cdecl GLI_DRV_lGetSizeOfTexture(void *texture)
{
    unsigned long flags;
    char formatClass;
    char plane;
    unsigned int height;
    unsigned int width;

    if (GLI_g_SavedBpp == 0x20) {
        flags = *(unsigned long const *)((unsigned char const *)texture + GLI_TEXTURE_FLAGS_OFFSET);
        if ((flags & 0x80) != 0) {
            formatClass = 3;
        } else if ((flags & 0x118) != 0) {
            formatClass = 5;
        } else {
            formatClass = 4;
        }
    } else if (GLI_g_SavedBpp == 0x10) {
        flags = *(unsigned long const *)((unsigned char const *)texture + GLI_TEXTURE_FLAGS_OFFSET);
        if ((flags & 0x80) != 0) {
            formatClass = 3;
        } else if ((flags & 0x8) != 0) {
            formatClass = 0;
        } else if ((flags & 0x110) != 0) {
            formatClass = 1;
        } else {
            formatClass = 2;
        }
    } else {
        formatClass = 2;
    }
    plane = *(unsigned long const *)((unsigned char const *)texture + GLI_TEXTURE_FIELD_30_OFFSET) != 0;
    width = *(unsigned short const *)((unsigned char const *)texture + GLI_TEXTURE_WIDTH_OFFSET);
    height = *(unsigned short const *)((unsigned char const *)texture + GLI_TEXTURE_HEIGHT_OFFSET);
    return (long)GLI_ulGetTextureMemoryCost(formatClass, plane, height, width);
}
