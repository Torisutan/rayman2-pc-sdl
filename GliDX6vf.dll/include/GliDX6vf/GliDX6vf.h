#pragma once

#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <objbase.h>

#if defined(_MSC_VER) && _MSC_VER < 1200
typedef HANDLE HMONITOR;
typedef long LONG_PTR;
#endif

#ifdef __cplusplus
extern "C" {
#endif

int __cdecl GLI_InitDirectDraw(void);
__declspec(dllexport) long __cdecl GLI_DRV_lGetDllInfo(char const *name, void *data);
__declspec(dllexport) long __cdecl GLI_DRV_lGetSizeOfTexture(void *texture);
__declspec(dllexport) long __cdecl GLI_DRV_lSetCommonFct(char const *name, int (__cdecl *callback)());
__declspec(dllexport) long __cdecl GLI_DRV_lSetCommonData(char const *name, void *data);
__declspec(dllexport) long __cdecl GLI_DRV_fnl_EnumModes(char *driverName, char *deviceName);

typedef long (__cdecl *GLI_tdfn_lAddDisplayInfo)(long displayNode, long deviceNode,
                                                 long modeNode, const char *property,
                                                 long value);
__declspec(dllexport) long __cdecl GLI_DRV_fn_lGetAllDisplayConfig(
    GLI_tdfn_lAddDisplayInfo p_fn_lAddDisplayInfo);
__declspec(dllexport) long __cdecl GLI_DRV_hChangeMode(BOOL fullscreen, LONG width, LONG height, LONG bpp);
__declspec(dllexport) long __cdecl GLI_DRV_bLockDevice(void **screen, LONG *pitch);
__declspec(dllexport) long __cdecl GLI_DRV_bUnlockDevice(void);
__declspec(dllexport) long __cdecl GLI_DRV_bBeginScene(void);
__declspec(dllexport) long __cdecl GLI_DRV_bEndScene(void);
__declspec(dllexport) long __cdecl GLI_DRV_bPrepareForGliWindowed(HWND hwnd);
__declspec(dllexport) long __cdecl GLI_DRV_bWindowedModeIsOptimized(void);
__declspec(dllexport) void __cdecl GLI_DRV_vClearDevice(int clearZBuffer, int clearColor, unsigned int color);
__declspec(dllexport) void __cdecl GLI_DRV_vClearZBufferRegion(LONG x0, LONG x1, LONG y0, LONG y1);
__declspec(dllexport) void __cdecl GLI_DRV_vSetZClip(float zClip, void *pGMT);
__declspec(dllexport) void __cdecl GLI_DRV_vSetClipWindow(float x0, float x1, float y0, float y1, void *pGMT);
__declspec(dllexport) void __cdecl GLI_DRV_vAddBlackPolygon(int left, int top, int right, int bottom);
__declspec(dllexport) void __cdecl GLI_DRV_vNoBlackPolygon(void);
__declspec(dllexport) void __cdecl GLI_DRV_vOptimizedWindowedMode(void);
__declspec(dllexport) void __cdecl GLI_DRV_vNonOptimizedWindowedMode(void);
__declspec(dllexport) void __cdecl GLI_DRV_vComputeFogEffect(void *pBG);
__declspec(dllexport) void __cdecl GLI_DRV_vDoOpaqueTextureSelection(void *element);
__declspec(dllexport) void __cdecl GLI_DRV_vFlipDevice(void);
__declspec(dllexport) void __cdecl GLI_DRV_vFlipDeviceWithSyncro(void);
__declspec(dllexport) void __cdecl GLI_DRV_vReadaptDisplay(void);
__declspec(dllexport) void __cdecl GLI_DRV_vPrepareForGliFullScreen(HWND hwnd);
__declspec(dllexport) void __cdecl GLI_DRV_vActivateGli(HWND hwnd, int active);
__declspec(dllexport) void __cdecl GLI_DRV_vUnLoadTextures(void);
__declspec(dllexport) void __cdecl GLI_DRV_vDownLoadTextures(LONG bRestore, LONG lTextureMode, LONG bReloading);
__declspec(dllexport) void __cdecl GLI_DRV_vWrite16bBitmapToBackBuffer(void *pSrc, LONG lW, LONG lH, LONG lDestLeft, LONG lDestTop, LONG lDestRight, LONG lDestBottom);
__declspec(dllexport) void __cdecl GLI_DRV_vGetStats(unsigned long *out0, unsigned long *out1);
__declspec(dllexport) void __cdecl GLI_DRV_vCloseDriver(void);
__declspec(dllexport) void __cdecl GLI_DRV_xClearViewingList(void);
__declspec(dllexport) void __cdecl GLI_DRV_xSendListToViewport(void *pVpt);
__declspec(dllexport) void __cdecl GLI_DRV_xSendElementTIToClip_TRIANGLES(void *element, void *pGMT);
__declspec(dllexport) void __cdecl GLI_DRV_xSendSingleTriangleToClip_TRIANGLES(void *pVertices, void *pUVSrc, void *pGMT);
__declspec(dllexport) void __cdecl GLI_DRV_vSendSingleLineToClip(void *param_1, void *param_2, void *param_3, void *param_4, void *param_5, void *param_6, long param_7, void *param_8);
__declspec(dllexport) void __cdecl GLI_DRV_vSendSpriteToClip(float *param_1, float param_2, int param_3);
__declspec(dllexport) void __cdecl GLI_DRV_vSendSpriteToClipWithColors(float *param_1, unsigned int *param_2, float param_3, int param_4);
__declspec(dllexport) void __cdecl GLI_DRV_vSendSpriteToClipWithUV(float *param_1, float *param_2, float param_3, int param_4);
__declspec(dllexport) void __cdecl GLI_DRV_xInitDriver(HWND hwnd, BOOL fullscreen, LONG width, LONG height, int bpp);
int __cdecl GLI_CreateOrResetDevice(HWND hwnd, BOOL fullscreen, LONG width, LONG height, LONG bpp);
int __cdecl GLI_DestroyOrCleanupDevice(void);
int __cdecl GLI_ReleaseDeviceObjects(int releaseDirectDraw);
void __cdecl GLI_DrawBlackPolygons(void);
void __cdecl GLI_UpdateFogParameters(float *fogParams);
void __cdecl GLI_EnsureZFuncGreaterequal(void);
void __cdecl GLI_EnsureZFuncAlways(void);
void __cdecl GLI_SetBeginSceneOpaqueTextureState(void);
void __cdecl GLI_EnsureZWriteEnabled(void);
void __cdecl GLI_DisableZWriteIfEnabled(void);

HRESULT __cdecl GLI_QueryAvailableVidMem(void *directDraw4, DWORD caps, DWORD *total, DWORD *freeBytes); /* 0x10007360 */
void __cdecl GLI_vSortValuesAscending(unsigned int *values, int count);                                 /* 0x100073a0 */
HRESULT __cdecl GLI_ReadDirectDrawCaps(void *gliCaps, DWORD capsSelector, void *directDraw4);            /* 0x100073e0 */
HRESULT __cdecl GLI_ReadDeviceCaps(void *gliCaps, void *directDraw4, void *deviceDesc);                  /* 0x10007630 */
unsigned int __cdecl GLI_lDeviceUsesVideoMemory(void);                                                   /* 0x10008590 */
unsigned int __cdecl GLI_ulGetDeviceDescFlags(void);                                                     /* 0x100085a0 */
void *__cdecl GLI_pGetDeviceDesc(void);                                                                  /* 0x100085b0 */

int __cdecl GLI_EnumTextureFormats(void);                                  /* 0x10007e10 */
void __cdecl GLI_vGetSelectedTextureFormat(char formatIndex, void *pixelFormat); /* 0x10007fa0 */
void __cdecl GLI_vSelectTextureFormats(void);                              /* 0x10007ff0 */

long __cdecl GLI_lFindFirstSetBit(unsigned int mask);                      /* 0x10007750 */
int __cdecl GLI_lMakeBitMask(int bitCount);                                /* 0x100078d0 */
unsigned int __cdecl GLI_lBitRunLength(unsigned int mask);                 /* 0x10007a40 */
int __cdecl GLI_lPixelFormatMasksMatch3(void *derivedRecord, void *sourceRecord); /* 0x10007be0 */
int __cdecl GLI_lPixelFormatMasksMatch4(void *derivedRecord, void *sourceRecord); /* 0x10007c20 */
void __cdecl GLI_vDerivePixelFormatRecord(void *derivedRecord, void *pixelFormat); /* 0x10007c60 */
int __cdecl GLI_lFindDefinedPixelFormat(void *pixelFormat);                /* 0x10007d50 */
unsigned char __cdecl GLI_lGetOrAddDefinedPixelFormat(void *pixelFormat);  /* 0x10007dc0 */
unsigned long __cdecl GLI_ulGetTextureMemoryCost(char formatClass, char plane, unsigned int height, unsigned int width); /* 0x100082a0 */

int __cdecl GLI_lConvertTexturePixels(unsigned int *sourcePixels, void *surface, void *textureRecord); /* 0x10009810 */
void __cdecl GLI_vTextureLoadError(void *textureRecord, unsigned int hr, char const *errorString);     /* 0x10009f10 */
void __cdecl GLI_vAnalyzeColorMask(unsigned int mask, int *shiftOut, int *adjustOut);                  /* 0x10009d10 */
void __cdecl GLI_vSelectPixelFormatForTexture(unsigned int textureFlags, void *pixelFormat);           /* 0x10009d60 */
unsigned long __cdecl GLI_ulGetTextureSurfaceCaps(void *texture, int textureMode, int allowNonLocal);  /* 0x10009e10 */
int __cdecl GLI_lApplyTextureColorKey(void *texture, void *surface);                                   /* 0x10009fc0 */
HRESULT CALLBACK GLI_EnumZBufferFormatsCallback(void *pixelFormat, void *context);

/* 0x1000d3f8 */
typedef struct GLI_tdstDllInfo_
{
    char szName[64];
    char szDescription[256];
    char szVersion[16];
    long lCode;
} GLI_tdstDllInfo;

extern GLI_tdstDllInfo GLI_g_stDllInfo;
void __cdecl GLI_vInitDllInfo(void);                                                    /* 0x10008910 */

#ifdef __cplusplus
}
#endif
