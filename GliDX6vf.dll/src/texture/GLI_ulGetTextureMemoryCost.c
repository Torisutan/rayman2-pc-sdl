#include "GliDX6vf/GliDX6vf.h"
#include "GliDX6vf/GliDX6vf_State.h"

unsigned long __cdecl GLI_ulGetTextureMemoryCost(char formatClass, char plane, unsigned int height, unsigned int width)
{
    signed char heightClass;
    signed char widthClass;

    heightClass = (signed char)GLI_lFindFirstSetBit(height);
    widthClass = (signed char)GLI_lFindFirstSetBit(width);
    return ((unsigned long *)GLI_g_TextureMemoryCostTable)[
        (int)plane * 0x51 + (int)formatClass * 0xa2 + (int)heightClass * 9 + (int)widthClass];
}
