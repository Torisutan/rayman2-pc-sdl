#include "GliDX6vf/GliDX6vf.h"
#include "GliDX6vf/GliDX6vf_State.h"

#define GLI_VIEWING_LIST_FIRST_MIN_OFFSET 0x0
#define GLI_VIEWING_LIST_FIRST_MAX_OFFSET 0x4
#define GLI_VIEWING_LIST_BUCKET_HEAD_OFFSET 0x4008
#define GLI_VIEWING_LIST_BUCKET_STORAGE_OFFSET 0x400c
#define GLI_VIEWING_LIST_ALLOC_CURSOR_OFFSET 0x10010
#define GLI_VIEWING_LIST_ALLOC_BASE_OFFSET 0x10014

#define M_ZList ((unsigned char *)GLI_g_GliBigGlobals->p_TheZListe)

void __cdecl GLI_DRV_xClearViewingList(void)
{
    unsigned long i;

    if (M_ZList != NULL) {
        *(void **)(M_ZList + GLI_VIEWING_LIST_ALLOC_CURSOR_OFFSET) =
            *(void **)(M_ZList + GLI_VIEWING_LIST_ALLOC_BASE_OFFSET);
        *(void **)(M_ZList + GLI_VIEWING_LIST_BUCKET_HEAD_OFFSET) =
            M_ZList + GLI_VIEWING_LIST_BUCKET_STORAGE_OFFSET;

        for (i = 0; i < 1; i++) {
            ((long *)(M_ZList + GLI_VIEWING_LIST_FIRST_MIN_OFFSET))[i] = 0x1000;
            ((long *)(M_ZList + GLI_VIEWING_LIST_FIRST_MAX_OFFSET))[i] = 0;
        }
    }
}

#undef M_ZList
