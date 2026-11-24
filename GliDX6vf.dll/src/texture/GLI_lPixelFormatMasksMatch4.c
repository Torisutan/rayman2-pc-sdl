#include "GliDX6vf/GliDX6vf.h"

int __cdecl GLI_lPixelFormatMasksMatch4(void *derivedRecord, void *sourceRecord)
{
    unsigned char *derived;
    unsigned char *source;

    derived = (unsigned char *)derivedRecord;
    source = (unsigned char *)sourceRecord;
    if ((*(int *)(derived + 0x18) == *(int *)(source + 0x10)) &&
        (*(int *)(derived + 0x24) == *(int *)(source + 0x14)) &&
        (*(int *)(derived + 0x30) == *(int *)(source + 0x18)) &&
        (*(int *)(derived + 0x3c) == *(int *)(source + 0x1c))) {
        return 1;
    }
    return 0;
}
