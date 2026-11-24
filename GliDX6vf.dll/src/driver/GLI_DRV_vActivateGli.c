#include "GliDX6vf/GliDX6vf.h"
#include "GliDX6vf/GliDX6vf_State.h"
#include "GliDX6vf/GLI_DiagLog.h"

__declspec(dllexport) void __cdecl GLI_DRV_vActivateGli(HWND hwnd, int active)
{
    GLI_DiagLog("vActivateGli hwnd=%p active=%d (flag was %ld)",
                (void *)hwnd, active, (long)GLI_g_ActivateGliFlag);
    GLI_g_ActivateGliFlag = active;

    if (active != 0) {
        GLI_DRV_vPrepareForGliFullScreen(hwnd);
        GLI_DiagLog("vActivateGli vPrepareForGliFullScreen done");
    }
}
