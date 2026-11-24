#include "GliDX6vf/GliDX6vf.h"

unsigned int __cdecl GLI_lBitRunLength(unsigned int mask)
{
    if (mask == 0x00000000u) { return 0; }
    if (mask == 0x00000001u) { return 1; }
    if (mask == 0x00000003u) { return 2; }
    if (mask == 0x00000007u) { return 3; }
    if (mask == 0x0000000fu) { return 4; }
    if (mask == 0x0000001fu) { return 5; }
    if (mask == 0x0000003fu) { return 6; }
    if (mask == 0x0000007fu) { return 7; }
    if (mask == 0x000000ffu) { return 8; }
    if (mask == 0x000001ffu) { return 9; }
    if (mask == 0x000003ffu) { return 10; }
    if (mask == 0x000007ffu) { return 11; }
    if (mask == 0x00000fffu) { return 12; }
    if (mask == 0x00001fffu) { return 13; }
    if (mask == 0x00003fffu) { return 14; }
    if (mask == 0x00007fffu) { return 15; }
    if (mask == 0x0000ffffu) { return 16; }
    if (mask == 0x0001ffffu) { return 17; }
    if (mask == 0x0003ffffu) { return 18; }
    if (mask == 0x0007ffffu) { return 19; }
    if (mask == 0x000fffffu) { return 20; }
    if (mask == 0x001fffffu) { return 21; }
    if (mask == 0x003fffffu) { return 22; }
    if (mask == 0x007fffffu) { return 23; }
    if (mask == 0x00ffffffu) { return 24; }
    if (mask == 0x01ffffffu) { return 25; }
    if (mask == 0x03ffffffu) { return 26; }
    if (mask == 0x07ffffffu) { return 27; }
    if (mask == 0x0fffffffu) { return 28; }
    if (mask == 0x1fffffffu) { return 29; }
    if (mask == 0x3fffffffu) { return 30; }
    if (mask == 0x7fffffffu) { return 31; }
    if (mask == 0xffffffffu) { return 32; }
    return 0;
}
