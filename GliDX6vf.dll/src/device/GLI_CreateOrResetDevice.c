#include "GliDX6vf/GliDX6vf.h"
#include "GliDX6vf/GliDX6vf_State.h"
#include "GliDX6vf/GLI_DiagLog.h"
#include "GliDX6vf/GLI_DX6Constants.h"

#include <string.h>

#define GLI_DDSD_CAPS 0x00000001u
#define GLI_DDSD_HEIGHT 0x00000002u
#define GLI_DDSD_WIDTH 0x00000004u
#define GLI_DDSD_BACKBUFFERCOUNT 0x00000020u
#define GLI_DDSD_PIXELFORMAT 0x00001000u

#define GLI_DDSCAPS_BACKBUFFER 0x00000004u
#define GLI_DDSCAPS_COMPLEX 0x00000008u
#define GLI_DDSCAPS_FLIP 0x00000010u
#define GLI_DDSCAPS_OFFSCREENPLAIN 0x00000040u
#define GLI_DDSCAPS_PRIMARYSURFACE 0x00000200u
#define GLI_DDSCAPS_SYSTEMMEMORY 0x00000800u
#define GLI_DDSCAPS_3DDEVICE 0x00002000u
#define GLI_DDSCAPS_VIDEOMEMORY 0x00004000u
#define GLI_DDSCAPS_ZBUFFER 0x00020000u
#define GLI_DDSCAPS_LOCALVIDMEM 0x10000000u

#define GLI_DDSCL_ALLOWREBOOT 0x00000002u
#define GLI_DDSCL_NORMAL 0x00000008u
#define GLI_DDSCL_FULLSCREEN 0x00000001u
#define GLI_DDSCL_EXCLUSIVE 0x00000010u
#define GLI_DDSCL_FPUSETUP 0x00000800u

#define GLI_E_FAIL ((HRESULT)0x80004005u)

typedef struct GLI_DDSCAPS2 {
    DWORD dwCaps;
    DWORD dwCaps2;
    DWORD dwCaps3;
    DWORD dwCaps4;
} GLI_DDSCAPS2;

typedef struct GLI_DDPIXELFORMAT {
    DWORD words[8];
} GLI_DDPIXELFORMAT;

typedef struct GLI_DDSURFACEDESC2 {
    DWORD words[31];
} GLI_DDSURFACEDESC2;

typedef struct GLI_D3DVIEWPORT2 {
    DWORD dwSize;
    DWORD dwX;
    DWORD dwY;
    DWORD dwWidth;
    DWORD dwHeight;
    float dvClipX;
    float dvClipY;
    float dvClipWidth;
    float dvClipHeight;
    float dvMinZ;
    float dvMaxZ;
} GLI_D3DVIEWPORT2;

static const GUID GLI_g_IID_IDirect3D3 = {
    0xbb223240,
    0xe72b,
    0x11d0,
    {0xa9, 0xb4, 0x00, 0xaa, 0x00, 0xc0, 0x99, 0x3e}
};

#define GLI_GetSurfaceMemoryCaps() \
    ((unsigned char)GLI_lDeviceUsesVideoMemory() != 0 ? (GLI_DDSCAPS_LOCALVIDMEM | GLI_DDSCAPS_VIDEOMEMORY) \
                                                      : GLI_DDSCAPS_SYSTEMMEMORY)
#define GLI_GetAvailableVidMem(total, freeBytes) \
    GLI_QueryAvailableVidMem(GLI_g_pDirectDraw4, GLI_DDSCAPS_VIDEOMEMORY, (total), (freeBytes))
#define GLI_SetSurfaceDescDword(desc, byteOffset, value) \
    ((desc)->words[(byteOffset) / sizeof(DWORD)] = (value))
#define GLI_CopyPixelFormatToDesc(desc, pixelFormat) \
    memcpy(&(desc)->words[0x48 / sizeof(DWORD)], (pixelFormat), sizeof(*(pixelFormat)))
#define GLI_SetDescCaps(desc, caps) GLI_SetSurfaceDescDword((desc), 0x68, (caps))

int __cdecl GLI_CreateOrResetDevice(HWND hwnd, BOOL fullscreen, LONG width, LONG height, LONG bpp)
{
    HRESULT hr;
    GLI_IDirectDrawClipper *clipper;
    DWORD totalVidMem;
    DWORD freeVidMem;
    GLI_D3DVIEWPORT2 viewport;
    GLI_DDSURFACEDESC2 desc;
    GLI_DDSCAPS2 caps;
    GLI_DDPIXELFORMAT zPixelFormat;

    GLI_DiagLog("CreateOrResetDevice ENTER hwnd=%p fullscreen=%d w=%ld h=%ld bpp=%ld",
                (void *)hwnd, fullscreen, width, height, bpp);

    GLI_g_IsFullscreen = fullscreen;
    if (fullscreen) {
        GLI_g_SavedWidth = width;
        GLI_g_SavedHeight = height;
        GLI_g_SavedBpp = bpp;
    }

    hr = GLI_g_pDirectDraw4->lpVtbl->QueryInterface(
        GLI_g_pDirectDraw4,
        &GLI_g_IID_IDirect3D3,
        (void **)&GLI_g_pDirect3D3);
    GLI_DiagLog("CreateOrResetDevice QueryInterface(IDirect3D3) hr=0x%08x pD3D3=%p",
                (unsigned int)hr, (void *)GLI_g_pDirect3D3);
    if (hr < 0) {
        goto done;
    }

    memset(&zPixelFormat, 0, sizeof(zPixelFormat));
    GLI_g_pDirect3D3->lpVtbl->EnumZBufferFormats(
        GLI_g_pDirect3D3,
        &GLI_g_DeviceGuid,
        GLI_EnumZBufferFormatsCallback,
        &zPixelFormat);
    GLI_DiagLog("CreateOrResetDevice EnumZBufferFormats picked dwSize=0x%lx (need 0x20)",
                (unsigned long)zPixelFormat.words[0]);
    if (zPixelFormat.words[0] != 0x20u) {
        return GLI_E_FAIL;
    }

    hr = GLI_g_pDirectDraw4->lpVtbl->SetCooperativeLevel(
        GLI_g_pDirectDraw4,
        fullscreen ? hwnd : NULL,
        GLI_DDSCL_FPUSETUP | GLI_DDSCL_ALLOWREBOOT |
            (fullscreen ? (GLI_DDSCL_FULLSCREEN | GLI_DDSCL_EXCLUSIVE) : GLI_DDSCL_NORMAL));
    GLI_DiagLog("CreateOrResetDevice SetCooperativeLevel hr=0x%08x", (unsigned int)hr);
    if (hr < 0) {
        goto done;
    }

    if (fullscreen) {
        hr = GLI_g_pDirectDraw4->lpVtbl->SetDisplayMode(
            GLI_g_pDirectDraw4,
            (DWORD)width,
            (DWORD)height,
            (DWORD)bpp,
            0,
            0);
        GLI_DiagLog("CreateOrResetDevice SetDisplayMode hr=0x%08x", (unsigned int)hr);
        if (hr < 0) {
            goto done;
        }

        SetRect(&GLI_g_ClientRect, 0, 0, width, height);
        GLI_g_ScreenRect = GLI_g_ClientRect;
    } else {
        GetClientRect(hwnd, &GLI_g_ScreenRect);
        GetClientRect(hwnd, &GLI_g_ClientRect);
        ClientToScreen(hwnd, (POINT *)&GLI_g_ScreenRect.left);
        ClientToScreen(hwnd, (POINT *)&GLI_g_ScreenRect.right);
    }
    GLI_DiagLog("CreateOrResetDevice rects: client=(%ld,%ld,%ld,%ld) screen=(%ld,%ld,%ld,%ld)",
                GLI_g_ClientRect.left, GLI_g_ClientRect.top,
                GLI_g_ClientRect.right, GLI_g_ClientRect.bottom,
                GLI_g_ScreenRect.left, GLI_g_ScreenRect.top,
                GLI_g_ScreenRect.right, GLI_g_ScreenRect.bottom);

    memset(&desc, 0, sizeof(desc));
    GLI_SetSurfaceDescDword(&desc, 0x00, 0x7c);
    GLI_SetSurfaceDescDword(&desc, 0x04, GLI_DDSD_CAPS);
    GLI_SetDescCaps(&desc, GLI_DDSCAPS_PRIMARYSURFACE);
    if (fullscreen) {
        GLI_SetSurfaceDescDword(&desc, 0x04, GLI_DDSD_CAPS | GLI_DDSD_BACKBUFFERCOUNT);
        GLI_SetDescCaps(&desc, GLI_DDSCAPS_PRIMARYSURFACE | GLI_DDSCAPS_FLIP |
            GLI_DDSCAPS_COMPLEX | GLI_DDSCAPS_3DDEVICE);
        GLI_SetSurfaceDescDword(&desc, 0x14, 1);
    }
    GLI_SetDescCaps(&desc, desc.words[0x68 / sizeof(DWORD)] | GLI_GetSurfaceMemoryCaps());

    GLI_GetAvailableVidMem(&totalVidMem, &freeVidMem);
    hr = GLI_g_pDirectDraw4->lpVtbl->CreateSurface(
        GLI_g_pDirectDraw4,
        &desc,
        &GLI_g_pPrimarySurface,
        NULL);
    GLI_DiagLog("CreateOrResetDevice CreateSurface(primary) hr=0x%08x surf=%p (vidmem free=%lu)",
                (unsigned int)hr, (void *)GLI_g_pPrimarySurface, (unsigned long)freeVidMem);
    if (hr != 0) {
        goto done;
    }

    GLI_GetAvailableVidMem(&totalVidMem, &freeVidMem);
    if (fullscreen) {
        memset(&caps, 0, sizeof(caps));
        caps.dwCaps = GLI_DDSCAPS_BACKBUFFER;
        hr = GLI_g_pPrimarySurface->lpVtbl->GetAttachedSurface(
            GLI_g_pPrimarySurface,
            &caps,
            &GLI_g_pBackBufferSurface);
        GLI_DiagLog("CreateOrResetDevice GetAttachedSurface(back) hr=0x%08x surf=%p",
                    (unsigned int)hr, (void *)GLI_g_pBackBufferSurface);
        if (hr != 0) {
            goto done;
        }
    } else {
        clipper = NULL;
        hr = GLI_g_pDirectDraw4->lpVtbl->CreateClipper(GLI_g_pDirectDraw4, 0, &clipper, NULL);
        GLI_DiagLog("CreateOrResetDevice CreateClipper hr=0x%08x clipper=%p",
                    (unsigned int)hr, (void *)clipper);
        if (hr != 0) {
            goto done;
        }
        clipper->lpVtbl->SetHWnd(clipper, 0, hwnd);
        GLI_g_pPrimarySurface->lpVtbl->SetClipper(GLI_g_pPrimarySurface, clipper);
        if (clipper != NULL) {
            clipper->lpVtbl->Release(clipper);
        }

        GLI_SetSurfaceDescDword(&desc, 0x04, GLI_DDSD_CAPS | GLI_DDSD_HEIGHT | GLI_DDSD_WIDTH);
        GLI_SetSurfaceDescDword(&desc, 0x08, (DWORD)GLI_g_ClientRect.bottom);
        GLI_SetSurfaceDescDword(&desc, 0x0c, (DWORD)GLI_g_ClientRect.right);
        GLI_SetDescCaps(&desc, GLI_DDSCAPS_OFFSCREENPLAIN | GLI_DDSCAPS_3DDEVICE | GLI_GetSurfaceMemoryCaps());
        hr = GLI_g_pDirectDraw4->lpVtbl->CreateSurface(
            GLI_g_pDirectDraw4,
            &desc,
            &GLI_g_pBackBufferSurface,
            NULL);
        GLI_DiagLog("CreateOrResetDevice CreateSurface(back, windowed) hr=0x%08x surf=%p",
                    (unsigned int)hr, (void *)GLI_g_pBackBufferSurface);
        if (hr != 0) {
            goto done;
        }
    }

    GLI_SetSurfaceDescDword(&desc, 0x04, GLI_DDSD_CAPS | GLI_DDSD_HEIGHT | GLI_DDSD_WIDTH | GLI_DDSD_PIXELFORMAT);
    GLI_SetSurfaceDescDword(&desc, 0x08, (DWORD)(GLI_g_ScreenRect.bottom - GLI_g_ScreenRect.top));
    GLI_SetSurfaceDescDword(&desc, 0x0c, (DWORD)(GLI_g_ScreenRect.right - GLI_g_ScreenRect.left));
    GLI_CopyPixelFormatToDesc(&desc, &zPixelFormat);
    GLI_SetDescCaps(&desc, GLI_DDSCAPS_ZBUFFER | GLI_GetSurfaceMemoryCaps());
    hr = GLI_g_pDirectDraw4->lpVtbl->CreateSurface(
        GLI_g_pDirectDraw4,
        &desc,
        &GLI_g_pZBufferSurface,
        NULL);
    GLI_DiagLog("CreateOrResetDevice CreateSurface(zbuffer) hr=0x%08x surf=%p",
                (unsigned int)hr, (void *)GLI_g_pZBufferSurface);
    if (hr != 0) {
        goto done;
    }

    GLI_GetAvailableVidMem(&totalVidMem, &freeVidMem);
    hr = GLI_g_pBackBufferSurface->lpVtbl->AddAttachedSurface(GLI_g_pBackBufferSurface, GLI_g_pZBufferSurface);
    GLI_DiagLog("CreateOrResetDevice AddAttachedSurface(z->back) hr=0x%08x", (unsigned int)hr);
    if (hr != 0) {
        goto done;
    }

    hr = GLI_g_pDirect3D3->lpVtbl->CreateDevice(
        GLI_g_pDirect3D3,
        &GLI_g_DeviceGuid,
        GLI_g_pBackBufferSurface,
        &GLI_g_pDirect3DDevice3,
        NULL);
    GLI_DiagLog("CreateOrResetDevice CreateDevice hr=0x%08x device=%p",
                (unsigned int)hr, (void *)GLI_g_pDirect3DDevice3);
    if (hr < 0) {
        goto done;
    }

    hr = GLI_g_pDirect3D3->lpVtbl->CreateViewport(
        GLI_g_pDirect3D3,
        &GLI_g_pDirect3DViewport3,
        NULL);
    GLI_DiagLog("CreateOrResetDevice CreateViewport hr=0x%08x viewport=%p",
                (unsigned int)hr, (void *)GLI_g_pDirect3DViewport3);
    if (hr < 0) {
        goto done;
    }

    GLI_g_pDirect3DDevice3->lpVtbl->AddViewport(GLI_g_pDirect3DDevice3, GLI_g_pDirect3DViewport3);

    memset(&viewport, 0, sizeof(viewport));
    viewport.dwSize = 0x2c;
    viewport.dwWidth = (DWORD)(GLI_g_ScreenRect.right - GLI_g_ScreenRect.left);
    viewport.dwHeight = (DWORD)(GLI_g_ScreenRect.bottom - GLI_g_ScreenRect.top);
    viewport.dvClipX = -1.0f;
    viewport.dvClipY = 1.0f;
    viewport.dvClipWidth = 2.0f;
    viewport.dvClipHeight = 2.0f;
    viewport.dvMaxZ = 1.0f;
    GLI_g_pDirect3DViewport3->lpVtbl->SetViewport2(GLI_g_pDirect3DViewport3, &viewport);
    GLI_g_pDirect3DDevice3->lpVtbl->SetCurrentViewport(GLI_g_pDirect3DDevice3, GLI_g_pDirect3DViewport3);

    GLI_g_pDirect3DDevice3->lpVtbl->SetRenderState(GLI_g_pDirect3DDevice3, GLI_D3DRS_ZENABLE, 1);
    GLI_g_pDirect3DDevice3->lpVtbl->SetRenderState(GLI_g_pDirect3DDevice3, GLI_D3DRS_ZFUNC, GLI_D3DCMP_GREATEREQUAL);
    GLI_g_pDirect3DDevice3->lpVtbl->SetRenderState(GLI_g_pDirect3DDevice3, GLI_D3DRS_DITHERENABLE, 1);
    GLI_g_pDirect3DDevice3->lpVtbl->SetRenderState(GLI_g_pDirect3DDevice3, GLI_D3DRS_SPECULARENABLE, 0);
    GLI_g_pDirect3DDevice3->lpVtbl->SetRenderState(GLI_g_pDirect3DDevice3, GLI_D3DRS_TEXTUREPERSPECTIVE, 1);
    GLI_g_pDirect3DDevice3->lpVtbl->SetRenderState(GLI_g_pDirect3DDevice3, GLI_D3DRS_ANTIALIAS, 0);
    GLI_g_pDirect3DDevice3->lpVtbl->SetRenderState(GLI_g_pDirect3DDevice3, GLI_D3DRS_CULLMODE, GLI_D3DCULL_NONE);
    GLI_g_pDirect3DDevice3->lpVtbl->SetTextureStageState(GLI_g_pDirect3DDevice3, 0, GLI_D3DTSS_COLORARG1, GLI_D3DTA_TEXTURE);
    GLI_g_pDirect3DDevice3->lpVtbl->SetTextureStageState(GLI_g_pDirect3DDevice3, 0, GLI_D3DTSS_COLORARG2, GLI_D3DTA_DIFFUSE);
    GLI_g_pDirect3DDevice3->lpVtbl->SetTextureStageState(GLI_g_pDirect3DDevice3, 0, GLI_D3DTSS_COLOROP, GLI_D3DTOP_MODULATE);
    GLI_g_pDirect3DDevice3->lpVtbl->SetTextureStageState(GLI_g_pDirect3DDevice3, 0, GLI_D3DTSS_MINFILTER, GLI_D3DTFN_LINEAR);
    GLI_g_pDirect3DDevice3->lpVtbl->SetTextureStageState(GLI_g_pDirect3DDevice3, 0, GLI_D3DTSS_MAGFILTER, GLI_D3DTFG_LINEAR);
    GLI_g_pDirect3DDevice3->lpVtbl->SetTextureStageState(GLI_g_pDirect3DDevice3, 0, GLI_D3DTSS_MIPFILTER, GLI_D3DTFP_LINEAR);
    GLI_g_pDirect3DDevice3->lpVtbl->SetRenderState(GLI_g_pDirect3DDevice3, GLI_D3DRS_ALPHATESTENABLE, 0);
    GLI_g_pDirect3DDevice3->lpVtbl->SetRenderState(GLI_g_pDirect3DDevice3, GLI_D3DRS_ALPHAFUNC, GLI_D3DCMP_GREATEREQUAL);
    GLI_g_pDirect3DDevice3->lpVtbl->SetRenderState(GLI_g_pDirect3DDevice3, GLI_D3DRS_ALPHAREF, 0xc0);

    GLI_DiagLog("CreateOrResetDevice SUCCESS — viewport/device/render-states set");
    hr = 0;

done:
    return hr;
}
