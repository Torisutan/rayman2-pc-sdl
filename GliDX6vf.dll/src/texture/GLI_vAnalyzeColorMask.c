#include "GliDX6vf/GliDX6vf.h"

void __cdecl GLI_vAnalyzeColorMask(unsigned int mask, int *shiftOut, int *adjustOut)
{
    int inspected;

    inspected = 0;
    *adjustOut = 0;
    *shiftOut = 0;

    while (inspected != 32 && (mask & 1u) == 0) {
        mask >>= 1;
        ++*shiftOut;
        ++inspected;
    }

    while (inspected != 32 && (mask & 1u) == 1u) {
        mask >>= 1;
        ++*adjustOut;
        ++inspected;
    }

    *adjustOut = 8 - *adjustOut;
}
