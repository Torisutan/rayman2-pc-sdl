#include "GliDX6vf/GliDX6vf.h"
#include "GliDX6vf/GliDX6vf_State.h"

#include <string.h>

#define GLI_DDENUM_FLAGS 0x00000007u

#define GLI_REQUIRED_DX_VERSION 0x600

#define GLI_D3DDEVICEDESC_COLORMODEL_OFFSET 0x08

#define GLI_DDSD_HEIGHT_OFFSET 0x08
#define GLI_DDSD_WIDTH_OFFSET 0x0c
#define GLI_DDSD_RGB_BITCOUNT_OFFSET 0x54

#define GLI_DISPLAY_CAPS_DWORDS 0x0b

#ifdef GLI_DX6_FIXES

static int s_modeEmitted = 0;

#define GLI_COMPAT_MODE_MAX 64
static DWORD s_compatResW[GLI_COMPAT_MODE_MAX];
static DWORD s_compatResH[GLI_COMPAT_MODE_MAX];
static int s_compatResN = 0;
#endif

HRESULT WINAPI DirectDrawEnumerateExA(
    BOOL (CALLBACK *callback)(GUID *, char *, char *, void *, HMONITOR),
    void *context,
    DWORD flags);

HRESULT WINAPI DirectDrawCreate(
    GUID *lpGUID,
    GLI_IDirectDraw **lplpDD,
    void *pUnkOuter);

static const GUID GLI_IID_IDirectDraw4_DisplayConfig = {
    0x9c59509a, 0x39bd, 0x11d1, {0x8c, 0x4a, 0x00, 0xc0, 0x4f, 0xd9, 0x30, 0xc5}};
static const GUID GLI_IID_IDirect3D3_DisplayConfig = {
    0xbb223240, 0xe72b, 0x11d0, {0xa9, 0xb4, 0x00, 0xaa, 0x00, 0xc0, 0x99, 0x3e}};

/* 0x100087a0 */
static HRESULT CALLBACK GLI_DisplayConfigDeviceCallback(
    GUID *deviceGuid,
    char *deviceDescription,
    char *deviceName,
    void *hwDeviceDesc,
    void *helDeviceDesc,
    void *directDraw4)
{
    void *deviceDesc;
    DWORD isHardware;
    DWORD caps[GLI_DISPLAY_CAPS_DWORDS];

    if (*(DWORD *)((unsigned char *)hwDeviceDesc + GLI_D3DDEVICEDESC_COLORMODEL_OFFSET) == 0) {
        return 1;
    }

    GLI_g_DisplayConfigDeviceNode =
        GLI_g_pAddDisplayInfo(GLI_g_DisplayConfigDisplayNode, 0, 0, "dev_add", 0);
    GLI_g_pAddDisplayInfo(GLI_g_DisplayConfigDisplayNode, GLI_g_DisplayConfigDeviceNode, 0,
                          "dev_name", (long)(LONG_PTR)deviceName);
    GLI_g_pAddDisplayInfo(GLI_g_DisplayConfigDisplayNode, GLI_g_DisplayConfigDeviceNode, 0,
                          "dev_desc", (long)(LONG_PTR)deviceDescription);
    GLI_g_pAddDisplayInfo(GLI_g_DisplayConfigDisplayNode, GLI_g_DisplayConfigDeviceNode, 0,
                          "dev_param", (long)(LONG_PTR)deviceGuid);

    deviceDesc = hwDeviceDesc;
    if (*(DWORD *)((unsigned char *)hwDeviceDesc + GLI_D3DDEVICEDESC_COLORMODEL_OFFSET) != 0) {
        isHardware = 1u;
    } else {
        deviceDesc = helDeviceDesc;
        isHardware = 0u;
    }
    ZeroMemory(caps, sizeof(caps));
    GLI_ReadDirectDrawCaps(caps, isHardware, directDraw4);
    GLI_ReadDeviceCaps(caps, directDraw4, deviceDesc);
    GLI_g_pAddDisplayInfo(GLI_g_DisplayConfigDisplayNode, GLI_g_DisplayConfigDeviceNode, 0,
                          "dev_caps", (long)(LONG_PTR)caps);
    return 1;
}

/* 0x100086f0 */
static HRESULT CALLBACK GLI_DisplayConfigModeCallback(void *surfaceDesc, void *context)
{
    unsigned char *desc = (unsigned char *)surfaceDesc;
    long modeNode;

    (void)context;

    if (*(DWORD *)(desc + GLI_DDSD_RGB_BITCOUNT_OFFSET) == 0x10) {
#ifdef GLI_DX6_FIXES
        ++s_modeEmitted;
#endif

        modeNode = GLI_g_pAddDisplayInfo(GLI_g_DisplayConfigDisplayNode, 0, 0, "modeadd", 0);
        GLI_g_pAddDisplayInfo(GLI_g_DisplayConfigDisplayNode, 0, modeNode, "modefs", 1);
        GLI_g_pAddDisplayInfo(GLI_g_DisplayConfigDisplayNode, 0, modeNode, "modebpp",
                              (long)*(DWORD *)(desc + GLI_DDSD_RGB_BITCOUNT_OFFSET));
        GLI_g_pAddDisplayInfo(GLI_g_DisplayConfigDisplayNode, 0, modeNode, "modew",
                              (long)*(DWORD *)(desc + GLI_DDSD_WIDTH_OFFSET));
        GLI_g_pAddDisplayInfo(GLI_g_DisplayConfigDisplayNode, 0, modeNode, "modeh",
                              (long)*(DWORD *)(desc + GLI_DDSD_HEIGHT_OFFSET));
    }
    return 1;
}

#ifdef GLI_DX6_FIXES

static HRESULT CALLBACK GLI_DisplayConfigModeCallbackCompat(void *surfaceDesc, void *context)
{
    unsigned char *desc = (unsigned char *)surfaceDesc;
    DWORD w = *(DWORD *)(desc + GLI_DDSD_WIDTH_OFFSET);
    DWORD h = *(DWORD *)(desc + GLI_DDSD_HEIGHT_OFFSET);
    long modeNode;
    int i;

    (void)context;

    for (i = 0; i < s_compatResN; ++i) {
        if (s_compatResW[i] == w && s_compatResH[i] == h) {
            return 1;
        }
    }
    if (s_compatResN < GLI_COMPAT_MODE_MAX) {
        s_compatResW[s_compatResN] = w;
        s_compatResH[s_compatResN] = h;
        ++s_compatResN;
    }

    modeNode = GLI_g_pAddDisplayInfo(GLI_g_DisplayConfigDisplayNode, 0, 0, "modeadd", 0);
    GLI_g_pAddDisplayInfo(GLI_g_DisplayConfigDisplayNode, 0, modeNode, "modefs", 1);
    GLI_g_pAddDisplayInfo(GLI_g_DisplayConfigDisplayNode, 0, modeNode, "modebpp", 0x10);
    GLI_g_pAddDisplayInfo(GLI_g_DisplayConfigDisplayNode, 0, modeNode, "modew", (long)w);
    GLI_g_pAddDisplayInfo(GLI_g_DisplayConfigDisplayNode, 0, modeNode, "modeh", (long)h);
    return 1;
}
#endif

/* 0x100085c0 */
static BOOL CALLBACK GLI_DisplayConfigDriverCallback(
    GUID *driverGuid,
    char *driverDescription,
    char *driverName,
    void *context,
    HMONITOR monitor)
{
    GLI_IDirectDraw *directDraw;
    GLI_IDirectDraw4 *directDraw4;
    GLI_IDirect3D3 *direct3D3;

    (void)context;
    (void)monitor;

    direct3D3 = NULL;
    directDraw4 = NULL;
    directDraw = NULL;

    GLI_g_DisplayConfigDisplayNode = GLI_g_pAddDisplayInfo(0, 0, 0, "dispadd", 0);
    GLI_g_pAddDisplayInfo(GLI_g_DisplayConfigDisplayNode, 0, 0, "dispname",
                          (long)(LONG_PTR)driverName);
    GLI_g_pAddDisplayInfo(GLI_g_DisplayConfigDisplayNode, 0, 0, "dispdesc",
                          (long)(LONG_PTR)driverDescription);
    GLI_g_pAddDisplayInfo(GLI_g_DisplayConfigDisplayNode, 0, 0, "dispparam",
                          (long)(LONG_PTR)driverGuid);

    if (DirectDrawCreate(driverGuid, &directDraw, NULL) == 0) {
        if (directDraw->lpVtbl->QueryInterface(directDraw, &GLI_IID_IDirectDraw4_DisplayConfig,
                                               (void **)&directDraw4) == 0) {
            if (directDraw4->lpVtbl->QueryInterface(directDraw4, &GLI_IID_IDirect3D3_DisplayConfig,
                                                    (void **)&direct3D3) == 0) {
#ifdef GLI_DX6_FIXES
                int modesBefore = s_modeEmitted;
#endif
                direct3D3->lpVtbl->EnumDevices(direct3D3, GLI_DisplayConfigDeviceCallback,
                                               directDraw4);
                directDraw4->lpVtbl->EnumDisplayModes(directDraw4, 0, NULL, NULL,
                                                      GLI_DisplayConfigModeCallback);
#ifdef GLI_DX6_FIXES

                if (s_modeEmitted == modesBefore) {
                    s_compatResN = 0;
                    directDraw4->lpVtbl->EnumDisplayModes(directDraw4, 0, NULL, NULL,
                                                          GLI_DisplayConfigModeCallbackCompat);
                }
#endif
            }
        }
    }

    if (directDraw != NULL) {
        directDraw->lpVtbl->Release(directDraw);
    }
    if (direct3D3 != NULL) {
        direct3D3->lpVtbl->Release(direct3D3);
    }
    if (directDraw4 != NULL) {
        directDraw4->lpVtbl->Release(directDraw4);
    }
    return TRUE;
}

__declspec(dllexport) long __cdecl
GLI_DRV_fn_lGetAllDisplayConfig(GLI_tdfn_lAddDisplayInfo p_fn_lAddDisplayInfo)
{
    HRESULT hr;

    GLI_g_pAddDisplayInfo = p_fn_lAddDisplayInfo;
    p_fn_lAddDisplayInfo(0, 0, 0, "dll_bmp", 2);
    if (GLI_g_pAddDisplayInfo(0, 0, 0, "miscdxversiondetect", 0) < GLI_REQUIRED_DX_VERSION) {
        return 0;
    }
#ifdef GLI_DX6_FIXES
    s_modeEmitted = 0;
#endif
    hr = DirectDrawEnumerateExA(GLI_DisplayConfigDriverCallback, NULL, GLI_DDENUM_FLAGS);
    return !hr;
}
