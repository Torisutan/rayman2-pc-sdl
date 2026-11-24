#include "GliDX6vf/GliDX6vf.h"
#include "GliDX6vf/GliDX6vf_State.h"
#include "GliDX6vf/GLI_DiagLog.h"

__declspec(dllexport) long __cdecl GLI_DRV_bEndScene(void)
{
    HRESULT hr = GLI_g_pDirect3DDevice3->lpVtbl->EndScene(GLI_g_pDirect3DDevice3);
    GLI_DiagLog("bEndScene EndScene hr=0x%08x sends=%lu", (unsigned int)hr, GLI_g_DiagSendCount);
    return hr >= 0;
}
