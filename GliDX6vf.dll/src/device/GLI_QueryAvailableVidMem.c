#include "GliDX6vf/GliDX6vf.h"
#include "GliDX6vf/GliDX6vf_State.h"

HRESULT __cdecl GLI_QueryAvailableVidMem(void *directDraw4, DWORD caps, DWORD *total, DWORD *freeBytes)
{
    DWORD capsBlock[4];
    GLI_IDirectDraw4 *ddraw;

    memset(capsBlock, 0, sizeof(capsBlock));
    capsBlock[0] = caps;
    ddraw = (GLI_IDirectDraw4 *)directDraw4;
    return ddraw->lpVtbl->GetAvailableVidMem(ddraw, capsBlock, total, freeBytes);
}
