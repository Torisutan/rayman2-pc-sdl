#include "GliDX6vf/GliDX6vf.h"
#include "GliDX6vf/GliDX6vf_State.h"

int __cdecl GLI_lFindDefinedPixelFormat(void *pixelFormat)
{
    unsigned char *source;
    int index;

    source = (unsigned char *)pixelFormat;
    if ((*(unsigned char *)(source + 0x04) & 0x40) == 0) {
        return -1;
    }

    index = 0;
    for (;;) {
        unsigned char *definedFormat;

        definedFormat = (unsigned char *)((GLI_GetDefinedPixelFormatFunction)GLI_g_GetDefinedPixelFormat)(index);
        if (definedFormat == NULL) {
            return -1;
        }
        if (*(int *)(definedFormat + 0x04) != *(int *)(source + 0x0c)) {
            ++index;
            continue;
        }
        if ((*(unsigned char *)(source + 0x04) & 1) == 0) {
            if (*(char *)(definedFormat + 0x02) == 0 &&
                (char)GLI_lPixelFormatMasksMatch3(definedFormat, source) != 0) {
                return index;
            }
        } else {
            if (*(char *)(definedFormat + 0x02) != 0 &&
                (char)GLI_lPixelFormatMasksMatch4(definedFormat, source) != 0) {
                return index;
            }
        }
        ++index;
    }
}
