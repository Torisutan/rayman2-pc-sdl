#include "GliDX6vf/GliDX6vf.h"
#include "GliDX6vf/GliDX6vf_State.h"
#include "GliDX6vf/GLI_DiagLog.h"

#include <stdlib.h>
#include <string.h>
#include <float.h>

#define GLI_VIEWING_LIST_BLOCK_SIZE 0x10018
#define GLI_VIEWING_LIST_SECONDARY_ALLOC_SIZE 0x30000
#define GLI_VIEWING_LIST_SECONDARY_ALLOC_OFFSET 0x10014
#define GLI_CAPS_INIT_DWORD_COUNT 0x0b

__declspec(dllexport) void __cdecl
GLI_DRV_xInitDriver(HWND hwnd, BOOL fullscreen, LONG width, LONG height, int bpp)
{
    unsigned char *globals;
    int initHr;
    int devHr;

    GLI_DiagLog("xInitDriver ENTER hwnd=%p fullscreen=%d w=%ld h=%ld bpp=%d",
                (void *)hwnd, fullscreen, width, height, bpp);
    GLI_DiagLog("xInitDriver fpu_control=0x%04x ddGuid={%08lx} devGuid={%08lx} devDescFlags=%lu",
                (unsigned int)(_controlfp(0, 0) & 0xffff),
                (unsigned long)GLI_g_DirectDrawGuid.Data1,
                (unsigned long)GLI_g_DeviceGuid.Data1,
                (unsigned long)GLI_g_DeviceDescFlags);

    globals = (unsigned char *)GLI_g_GliBigGlobals;
    GLI_DiagLog("xInitDriver gliBigGlobals=%p", (void *)globals);

    GLI_g_GliBigGlobals->p_TheZListe = malloc(GLI_VIEWING_LIST_BLOCK_SIZE);
    memset(GLI_g_GliBigGlobals->p_TheZListe, 0, GLI_VIEWING_LIST_BLOCK_SIZE);

    *(void **)((unsigned char *)GLI_g_GliBigGlobals->p_TheZListe +
               GLI_VIEWING_LIST_SECONDARY_ALLOC_OFFSET) =
        malloc(GLI_VIEWING_LIST_SECONDARY_ALLOC_SIZE);
    GLI_g_GliBigGlobals->ulCurrentZTable = 0;

    GLI_DRV_xClearViewingList();

    GLI_g_WindowHandle = hwnd;
    ShowWindow(hwnd, 1);
    UpdateWindow(GLI_g_WindowHandle);
    SetFocus(GLI_g_WindowHandle);

#ifdef GLI_DX6_FIXES
    SetForegroundWindow(GLI_g_WindowHandle);
#endif
    GLI_DiagLog("xInitDriver SetForegroundWindow done (fg=%p)", (void *)GetForegroundWindow());

    initHr = GLI_InitDirectDraw();
    GLI_DiagLog("xInitDriver GLI_InitDirectDraw hr=0x%08x pDD4=%p halCaps.dwSize=0x%lx",
                (unsigned int)initHr, (void *)GLI_g_pDirectDraw4,
                (unsigned long)GLI_g_HalCaps.dwSize);
    if (initHr == 0) {
        devHr = GLI_CreateOrResetDevice(GLI_g_WindowHandle, fullscreen, width, height, bpp);
        GLI_DiagLog("xInitDriver GLI_CreateOrResetDevice hr=0x%08x", (unsigned int)devHr);
        memset(GLI_g_GliCaps, 0, GLI_CAPS_INIT_DWORD_COUNT * sizeof(DWORD));
        GLI_ReadDirectDrawCaps(GLI_g_GliCaps, GLI_ulGetDeviceDescFlags(), GLI_g_pDirectDraw4);
        GLI_ReadDeviceCaps(GLI_g_GliCaps, GLI_g_pDirectDraw4, GLI_pGetDeviceDesc());
        GLI_EnumTextureFormats();
        GLI_vSelectTextureFormats();

#ifdef GLI_DX6_FIXES
        if (devHr == 0) {
            GLI_g_ActivateGliFlag = 1;
        }
#endif
        GLI_DiagLog("xInitDriver caps/format/cost tables built; texFormatCount=%lu gliCaps[4]=0x%02x activate=%ld EXIT",
                    (unsigned long)GLI_g_TextureFormatCount,
                    GLI_g_GliCaps ? ((unsigned char *)GLI_g_GliCaps)[4] : 0,
                    (long)GLI_g_ActivateGliFlag);
    } else {
        GLI_DiagLog("xInitDriver ABORT — InitDirectDraw failed, device not created");
    }
}
