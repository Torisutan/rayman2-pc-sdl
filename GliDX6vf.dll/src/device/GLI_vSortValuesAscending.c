#include "GliDX6vf/GliDX6vf.h"

void __cdecl GLI_vSortValuesAscending(unsigned int *values, int count)
{
    unsigned int *end = values + count;
    unsigned int *outer;

    for (outer = values; outer < end - 1; ++outer) {
        unsigned int *inner;

        for (inner = outer; inner < end; ++inner) {
            unsigned int current = *outer;

            if (current > *inner) {
                *outer = *inner;
                *inner = current;
            }
        }
    }
}
