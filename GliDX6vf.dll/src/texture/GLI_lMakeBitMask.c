#include "GliDX6vf/GliDX6vf.h"

int __cdecl GLI_lMakeBitMask(int bitCount)
{
    if (bitCount == 0) { return 0; }
    if (bitCount == 1) { return 0x00000001; }
    if (bitCount == 2) { return 0x00000003; }
    if (bitCount == 3) { return 0x00000007; }
    if (bitCount == 4) { return 0x0000000f; }
    if (bitCount == 5) { return 0x0000001f; }
    if (bitCount == 6) { return 0x0000003f; }
    if (bitCount == 7) { return 0x0000007f; }
    if (bitCount == 8) { return 0x000000ff; }
    if (bitCount == 9) { return 0x000001ff; }
    if (bitCount == 10) { return 0x000003ff; }
    if (bitCount == 11) { return 0x000007ff; }
    if (bitCount == 12) { return 0x00000fff; }
    if (bitCount == 13) { return 0x00001fff; }
    if (bitCount == 14) { return 0x00003fff; }
    if (bitCount == 15) { return 0x00007fff; }
    if (bitCount == 16) { return 0x0000ffff; }
    if (bitCount == 17) { return 0x0001ffff; }
    if (bitCount == 18) { return 0x0003ffff; }
    if (bitCount == 19) { return 0x0007ffff; }
    if (bitCount == 20) { return 0x000fffff; }
    if (bitCount == 21) { return 0x001fffff; }
    if (bitCount == 22) { return 0x003fffff; }
    if (bitCount == 23) { return 0x007fffff; }
    if (bitCount == 24) { return 0x00ffffff; }
    if (bitCount == 25) { return 0x01ffffff; }
    if (bitCount == 26) { return 0x03ffffff; }
    if (bitCount == 27) { return 0x07ffffff; }
    if (bitCount == 28) { return 0x0fffffff; }
    if (bitCount == 29) { return 0x1fffffff; }
    if (bitCount == 30) { return 0x3fffffff; }
    if (bitCount == 31) { return 0x7fffffff; }
    if (bitCount == 32) { return -1; }
    return 0;
}
