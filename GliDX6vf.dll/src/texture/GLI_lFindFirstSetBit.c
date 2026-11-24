#include "GliDX6vf/GliDX6vf.h"

long __cdecl GLI_lFindFirstSetBit(unsigned int mask)
{
    if ((mask & 0x00000001u) != 0) { return 0; }
    if ((mask & 0x00000002u) != 0) { return 1; }
    if ((mask & 0x00000004u) != 0) { return 2; }
    if ((mask & 0x00000008u) != 0) { return 3; }
    if ((mask & 0x00000010u) != 0) { return 4; }
    if ((mask & 0x00000020u) != 0) { return 5; }
    if ((mask & 0x00000040u) != 0) { return 6; }
    if ((mask & 0x00000080u) != 0) { return 7; }
    if ((mask & 0x00000100u) != 0) { return 8; }
    if ((mask & 0x00000200u) != 0) { return 9; }
    if ((mask & 0x00000400u) != 0) { return 10; }
    if ((mask & 0x00000800u) != 0) { return 11; }
    if ((mask & 0x00001000u) != 0) { return 12; }
    if ((mask & 0x00002000u) != 0) { return 13; }
    if ((mask & 0x00004000u) != 0) { return 14; }
    if ((mask & 0x00008000u) != 0) { return 15; }
    if ((mask & 0x00010000u) != 0) { return 16; }
    if ((mask & 0x00020000u) != 0) { return 17; }
    if ((mask & 0x00040000u) != 0) { return 18; }
    if ((mask & 0x00080000u) != 0) { return 19; }
    if ((mask & 0x00100000u) != 0) { return 20; }
    if ((mask & 0x00200000u) != 0) { return 21; }
    if ((mask & 0x00400000u) != 0) { return 22; }
    if ((mask & 0x00800000u) != 0) { return 23; }
    if ((mask & 0x01000000u) != 0) { return 24; }
    if ((mask & 0x02000000u) != 0) { return 25; }
    if ((mask & 0x04000000u) != 0) { return 26; }
    if ((mask & 0x08000000u) != 0) { return 27; }
    if ((mask & 0x10000000u) != 0) { return 28; }
    if ((mask & 0x20000000u) != 0) { return 29; }
    if ((mask & 0x40000000u) != 0) { return 30; }
    return (mask & 0x80000000u) != 0 ? 31 : 0;
}
