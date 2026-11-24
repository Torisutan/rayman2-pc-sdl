#include "GliDX6vf/GliDX6vf.h"

void __cdecl GLI_vDerivePixelFormatRecord(void *derivedRecord, void *pixelFormat)
{
    unsigned char *out;
    unsigned char *src;
    long shift;
    long n;

    out = (unsigned char *)derivedRecord;
    src = (unsigned char *)pixelFormat;

    *(unsigned short *)(out + 0x00) = 0;
    *(unsigned char *)(out + 0x02) = (unsigned char)(*(unsigned int *)(src + 0x04) & 1u);

    *(int *)(out + 0x04) = *(int *)(src + 0x0c);
    *(int *)(out + 0x08) = GLI_lMakeBitMask(*(int *)(out + 0x04));
    n = *(int *)(out + 0x04);
    *(int *)(out + 0x0c) = (n + (n < 0 ? 7 : 0)) >> 3;

    *(unsigned int *)(out + 0x18) = *(unsigned int *)(src + 0x10);
    shift = GLI_lFindFirstSetBit(*(unsigned int *)(out + 0x18));
    *(long *)(out + 0x14) = shift;
    *(unsigned int *)(out + 0x10) = GLI_lBitRunLength(*(unsigned int *)(out + 0x18) >> shift);

    *(unsigned int *)(out + 0x24) = *(unsigned int *)(src + 0x14);
    shift = GLI_lFindFirstSetBit(*(unsigned int *)(out + 0x24));
    *(long *)(out + 0x20) = shift;
    *(unsigned int *)(out + 0x1c) = GLI_lBitRunLength(*(unsigned int *)(out + 0x24) >> shift);

    *(unsigned int *)(out + 0x30) = *(unsigned int *)(src + 0x18);
    shift = GLI_lFindFirstSetBit(*(unsigned int *)(out + 0x30));
    *(long *)(out + 0x2c) = shift;
    *(unsigned int *)(out + 0x28) = GLI_lBitRunLength(*(unsigned int *)(out + 0x30) >> shift);

    if (*(unsigned char *)(out + 0x02) != 0) {
        *(unsigned int *)(out + 0x3c) = *(unsigned int *)(src + 0x1c);
        shift = GLI_lFindFirstSetBit(*(unsigned int *)(out + 0x3c));
        *(long *)(out + 0x38) = shift;
        *(unsigned int *)(out + 0x34) = GLI_lBitRunLength(*(unsigned int *)(out + 0x3c) >> shift);
    } else {
        *(unsigned int *)(out + 0x3c) = 0;
        *(unsigned int *)(out + 0x38) = 0;
        *(unsigned int *)(out + 0x34) = 0;
    }
}
