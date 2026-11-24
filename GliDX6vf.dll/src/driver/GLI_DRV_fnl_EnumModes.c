#include "GliDX6vf/GliDX6vf.h"
#include "GliDX6vf/GliDX6vf_State.h"

#include <string.h>

#define GLI_DDENUM_ATTACHEDSECONDARYDEVICES 0x00000001u
#define GLI_DDENUM_DETACHEDSECONDARYDEVICES 0x00000002u
#define GLI_DDENUM_NONDISPLAYDEVICES 0x00000004u
#define GLI_DISPLAY_MODE_RGB_BIT_COUNT_OFFSET 0x54u

HRESULT WINAPI DirectDrawEnumerateExA(
    BOOL (CALLBACK *callback)(GUID *, char *, char *, void *, HMONITOR),
    void *context,
    DWORD flags);

HRESULT WINAPI DirectDrawCreate(
    GUID *lpGUID,
    GLI_IDirectDraw **lplpDD,
    void *pUnkOuter);

static const GUID GLI_g_IID_IDirectDraw4_ForEnumModes = {
    0x9c59509a,
    0x39bd,
    0x11d1,
    {0x8c, 0x4a, 0x00, 0xc0, 0x4f, 0xd9, 0x30, 0xc5}
};

static const GUID GLI_g_IID_IDirect3D3_ForEnumModes = {
    0xbb223240,
    0xe72b,
    0x11d0,
    {0xa9, 0xb4, 0x00, 0xaa, 0x00, 0xc0, 0x99, 0x3e}
};

static unsigned char GLI_g_EnumModeFound;

static BOOL CALLBACK GLI_EnumDriverCallback(
    GUID *driverGuid,
    char *driverDescription,
    char *driverName,
    void *context,
    HMONITOR monitor)
{
    (void)driverDescription;
    (void)monitor;

    if (strcmp((char *)context, driverName) == 0) {
        if (driverGuid != NULL) {
            GLI_g_DirectDrawGuid = *driverGuid;
        } else {
            memset(&GLI_g_DirectDrawGuid, 0, sizeof(GLI_g_DirectDrawGuid));
        }
        GLI_g_EnumModeFound = 1;
        return FALSE;
    }
    return TRUE;
}

static HRESULT CALLBACK GLI_EnumDeviceCallback(
    GUID *deviceGuid,
    char *deviceDescription,
    char *deviceName,
    void *hardwareDesc,
    void *softwareDesc,
    void *context)
{
    DWORD *hardwareWords;

    (void)deviceDescription;

    if (strcmp(deviceName, (char *)context) == 0) {
        GLI_g_EnumModeFound = 1;
        GLI_g_DeviceGuid = *deviceGuid;
        hardwareWords = (DWORD *)hardwareDesc;
        if (hardwareWords[2] != 0) {
            GLI_g_DeviceDescFlags = 1;
            memcpy(GLI_g_DeviceDesc, hardwareDesc, sizeof(GLI_g_DeviceDesc));
        } else {
            GLI_g_DeviceDescFlags = 0;
            memcpy(GLI_g_DeviceDesc, softwareDesc, sizeof(GLI_g_DeviceDesc));
        }
        return 0;
    }
    return 1;
}

static HRESULT CALLBACK GLI_EnumDisplayModeCallback(void *surfaceDesc, void *context)
{
    DWORD *descWords;
    DWORD width;
    DWORD height;

    (void)context;

    descWords = (DWORD *)surfaceDesc;
    if (descWords[GLI_DISPLAY_MODE_RGB_BIT_COUNT_OFFSET / sizeof(DWORD)] == 0x10) {

        width  = descWords[0x0c / sizeof(DWORD)];
        height = descWords[0x08 / sizeof(DWORD)];
        GLI_g_AddDisplayMode(1, width, height, 16);
    }
    return 1;
}

long __cdecl GLI_DRV_fnl_EnumModes(char *driverName, char *deviceName)
{

    HRESULT hr;
    GLI_IDirectDraw4 *directDraw4;
    GLI_IDirect3D3 *direct3D3;

    direct3D3 = NULL;
    directDraw4 = NULL;

    GLI_g_EnumModeFound = FALSE;
    hr = DirectDrawEnumerateExA(
        GLI_EnumDriverCallback,
        driverName,
        GLI_DDENUM_ATTACHEDSECONDARYDEVICES |
            GLI_DDENUM_DETACHEDSECONDARYDEVICES |
            GLI_DDENUM_NONDISPLAYDEVICES);
    if (hr == 0 && GLI_g_EnumModeFound) {
        hr = DirectDrawCreate(&GLI_g_DirectDrawGuid, (GLI_IDirectDraw **)&driverName, NULL);
        if (hr == 0) {
            hr = ((GLI_IDirectDraw *)driverName)->lpVtbl->QueryInterface(
                (GLI_IDirectDraw *)driverName,
                &GLI_g_IID_IDirectDraw4_ForEnumModes,
                (void **)&directDraw4);
            ((GLI_IDirectDraw *)driverName)->lpVtbl->Release((GLI_IDirectDraw *)driverName);
            if (hr == 0) {
                hr = directDraw4->lpVtbl->QueryInterface(
                    directDraw4,
                    &GLI_g_IID_IDirect3D3_ForEnumModes,
                    (void **)&direct3D3);
                if (hr == 0) {
                    GLI_g_EnumModeFound = FALSE;
                    hr = direct3D3->lpVtbl->EnumDevices(direct3D3, GLI_EnumDeviceCallback, deviceName);
                    if (hr == 0 && GLI_g_EnumModeFound) {
                        hr = directDraw4->lpVtbl->EnumDisplayModes(
                            directDraw4,
                            0,
                            NULL,
                            NULL,
                            GLI_EnumDisplayModeCallback);
                    }
                }
                if (direct3D3 != NULL) {
                    direct3D3->lpVtbl->Release(direct3D3);
                }
                if (directDraw4 != NULL) {
                    directDraw4->lpVtbl->Release(directDraw4);
                }
                if (hr == 0 && GLI_g_EnumModeFound) {
                    return 1;
                }
            }
        }
    }
    return 0;
}
