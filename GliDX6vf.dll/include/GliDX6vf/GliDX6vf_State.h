#pragma once

#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <objbase.h>

#include "GLI/GLI_BigGlobals.h"

#ifdef __cplusplus
extern "C" {
#endif

#define GLI_DDCAPS_DX6_SIZE 0x17c
#define GLI_DDPF_ZBUFFER 0x00000400u
#define GLI_TEXTURE_FORMAT_RECORD_SIZE 0x20
#define GLI_TEXTURE_FORMAT_RECORD_COUNT 20
#define GLI_TEXTURE_FORMAT_SELECTED_COUNT 6
#define GLI_TEXTURE_COST_FORMAT_COUNT 6
#define GLI_TEXTURE_COST_PLANE_COUNT 2
#define GLI_TEXTURE_COST_DIMENSION_COUNT 9
#define GLI_BLACK_POLYGON_VERTEX_SIZE 0x20
#define GLI_BLACK_POLYGON_MAX_VERTICES 24

typedef struct GLI_tdstTLVertex_
{
    float sx, sy, sz, rhw;
    DWORD color, specular;
    float tu, tv;
} GLI_tdstTLVertex;

typedef struct GLI_IDirectDraw GLI_IDirectDraw;
typedef struct GLI_IDirectDraw4 GLI_IDirectDraw4;
typedef struct GLI_IDirectDrawClipper GLI_IDirectDrawClipper;
typedef struct GLI_IDirectDrawSurface4 GLI_IDirectDrawSurface4;
typedef struct GLI_IDirect3D3 GLI_IDirect3D3;
typedef struct GLI_IDirect3DDevice3 GLI_IDirect3DDevice3;
typedef struct GLI_IDirect3DViewport3 GLI_IDirect3DViewport3;

typedef int (__cdecl *GLI_CommonFunction)();
typedef void (__cdecl *GLI_TimerGetCounterFunction)(short timerType, LARGE_INTEGER *counter);
typedef unsigned long (__cdecl *GLI_TimerTickPerSecondFunction)(short timerType);
typedef void *(__cdecl *GLI_GetDefinedPixelFormatFunction)(long formatIndex);
typedef long (__cdecl *GLI_AddDefinedPixelFormatFunction)(void *pixelFormat);

typedef struct GLI_IUnknownVtbl {
    HRESULT (STDMETHODCALLTYPE *QueryInterface)(void *self, const GUID *riid, void **ppvObj);
    ULONG (STDMETHODCALLTYPE *AddRef)(void *self);
    ULONG (STDMETHODCALLTYPE *Release)(void *self);
} GLI_IUnknownVtbl;

typedef struct GLI_IDirectDraw4Vtbl {
    HRESULT (STDMETHODCALLTYPE *QueryInterface)(void *self, const GUID *riid, void **ppvObj);
    ULONG (STDMETHODCALLTYPE *AddRef)(void *self);
    ULONG (STDMETHODCALLTYPE *Release)(void *self);
    void *Compact;
    HRESULT (STDMETHODCALLTYPE *CreateClipper)(void *self, DWORD flags, GLI_IDirectDrawClipper **clipper, void *outer);
    void *CreatePalette;
    HRESULT (STDMETHODCALLTYPE *CreateSurface)(void *self, void *surfaceDesc, GLI_IDirectDrawSurface4 **surface, void *outer);
    void *DuplicateSurface;
    HRESULT (STDMETHODCALLTYPE *EnumDisplayModes)(void *self, DWORD flags, void *surfaceDesc, void *context, HRESULT (CALLBACK *callback)(void *, void *));
    void *EnumSurfaces;
    void *FlipToGDISurface;
    HRESULT (STDMETHODCALLTYPE *GetCaps)(void *self, void *halCaps, void *helCaps);
    void *GetDisplayMode;
    void *GetFourCCCodes;
    void *GetGDISurface;
    void *GetMonitorFrequency;
    void *GetScanLine;
    void *GetVerticalBlankStatus;
    void *Initialize;
    void *RestoreDisplayMode;
    HRESULT (STDMETHODCALLTYPE *SetCooperativeLevel)(void *self, HWND hwnd, DWORD flags);
    HRESULT (STDMETHODCALLTYPE *SetDisplayMode)(void *self, DWORD width, DWORD height, DWORD bpp, DWORD refresh, DWORD flags);
    HRESULT (STDMETHODCALLTYPE *WaitForVerticalBlank)(void *self, DWORD flags, HANDLE eventHandle);
    HRESULT (STDMETHODCALLTYPE *GetAvailableVidMem)(void *self, void *caps, DWORD *total, DWORD *free);
} GLI_IDirectDraw4Vtbl;

typedef struct GLI_IDirectDrawClipperVtbl {
    HRESULT (STDMETHODCALLTYPE *QueryInterface)(void *self, const GUID *riid, void **ppvObj);
    ULONG (STDMETHODCALLTYPE *AddRef)(void *self);
    ULONG (STDMETHODCALLTYPE *Release)(void *self);
    void *GetClipList;
    void *GetHWnd;
    void *Initialize;
    void *IsClipListChanged;
    void *SetClipList;
    HRESULT (STDMETHODCALLTYPE *SetHWnd)(void *self, DWORD flags, HWND hwnd);
} GLI_IDirectDrawClipperVtbl;

typedef struct GLI_IDirectDrawSurface4Vtbl {
    HRESULT (STDMETHODCALLTYPE *QueryInterface)(void *self, const GUID *riid, void **ppvObj);
    ULONG (STDMETHODCALLTYPE *AddRef)(void *self);
    ULONG (STDMETHODCALLTYPE *Release)(void *self);
    HRESULT (STDMETHODCALLTYPE *AddAttachedSurface)(void *self, GLI_IDirectDrawSurface4 *surface);
    void *AddOverlayDirtyRect;
    HRESULT (STDMETHODCALLTYPE *Blt)(void *self, RECT *destRect, GLI_IDirectDrawSurface4 *srcSurface, RECT *srcRect, DWORD flags, void *fx);
    void *BltBatch;
    void *BltFast;
    void *DeleteAttachedSurface;
    void *EnumAttachedSurfaces;
    void *EnumOverlayZOrders;
    HRESULT (STDMETHODCALLTYPE *Flip)(void *self, GLI_IDirectDrawSurface4 *targetOverride, DWORD flags);
    HRESULT (STDMETHODCALLTYPE *GetAttachedSurface)(void *self, void *caps, GLI_IDirectDrawSurface4 **surface);
    void *GetBltStatus;
    void *GetCaps;
    void *GetClipper;
    void *GetColorKey;
    void *GetDC;
    HRESULT (STDMETHODCALLTYPE *GetFlipStatus)(void *self, DWORD flags);
    void *GetOverlayPosition;
    void *GetPalette;
    HRESULT (STDMETHODCALLTYPE *GetPixelFormat)(void *self, void *pixelFormat);
    void *GetSurfaceDesc;
    void *Initialize;
    HRESULT (STDMETHODCALLTYPE *IsLost)(void *self);
    HRESULT (STDMETHODCALLTYPE *Lock)(void *self, RECT *rect, void *surfaceDesc, DWORD flags, HANDLE eventHandle);
    void *ReleaseDC;
    HRESULT (STDMETHODCALLTYPE *Restore)(void *self);
    HRESULT (STDMETHODCALLTYPE *SetClipper)(void *self, GLI_IDirectDrawClipper *clipper);
    void *SetColorKey;
    void *SetOverlayPosition;
    void *SetPalette;
    HRESULT (STDMETHODCALLTYPE *Unlock)(void *self, void *surfaceData);
} GLI_IDirectDrawSurface4Vtbl;

typedef struct GLI_IDirect3D3Vtbl {
    HRESULT (STDMETHODCALLTYPE *QueryInterface)(void *self, const GUID *riid, void **ppvObj);
    ULONG (STDMETHODCALLTYPE *AddRef)(void *self);
    ULONG (STDMETHODCALLTYPE *Release)(void *self);
    HRESULT (STDMETHODCALLTYPE *EnumDevices)(void *self, HRESULT (CALLBACK *callback)(GUID *, char *, char *, void *, void *, void *), void *context);
    void *CreateLight;
    void *CreateMaterial;
    HRESULT (STDMETHODCALLTYPE *CreateViewport)(void *self, GLI_IDirect3DViewport3 **viewport, void *outer);
    void *FindDevice;
    HRESULT (STDMETHODCALLTYPE *CreateDevice)(void *self, const GUID *clsid, GLI_IDirectDrawSurface4 *surface, GLI_IDirect3DDevice3 **device, void *outer);
    void *CreateVertexBuffer;
    HRESULT (STDMETHODCALLTYPE *EnumZBufferFormats)(void *self, const GUID *clsid, HRESULT (CALLBACK *callback)(void *, void *), void *context);
} GLI_IDirect3D3Vtbl;

typedef struct GLI_IDirect3DDevice3Vtbl {
    HRESULT (STDMETHODCALLTYPE *QueryInterface)(void *self, const GUID *riid, void **ppvObj);
    ULONG (STDMETHODCALLTYPE *AddRef)(void *self);
    ULONG (STDMETHODCALLTYPE *Release)(void *self);
    void *GetCaps;
    void *GetStats;
    HRESULT (STDMETHODCALLTYPE *AddViewport)(void *self, GLI_IDirect3DViewport3 *viewport);
    void *DeleteViewport;
    void *NextViewport;
    HRESULT (STDMETHODCALLTYPE *EnumTextureFormats)(void *self, HRESULT (CALLBACK *callback)(void *, void *), void *context);
    HRESULT (STDMETHODCALLTYPE *BeginScene)(void *self);
    HRESULT (STDMETHODCALLTYPE *EndScene)(void *self);
    void *GetDirect3D;
    HRESULT (STDMETHODCALLTYPE *SetCurrentViewport)(void *self, GLI_IDirect3DViewport3 *viewport);
    void *GetCurrentViewport;
    void *SetRenderTarget;
    void *GetRenderTarget;
    void *Begin;
    void *BeginIndexed;
    void *Vertex;
    void *Index;
    void *End;
    void *GetRenderState;
    HRESULT (STDMETHODCALLTYPE *SetRenderState)(void *self, DWORD state, DWORD value);
    void *GetLightState;
    void *SetLightState;
    void *SetTransform;
    void *GetTransform;
    void *MultiplyTransform;
    HRESULT (STDMETHODCALLTYPE *DrawPrimitive)(void *self, DWORD primitiveType, DWORD vertexTypeDesc, void *vertices, DWORD vertexCount, DWORD flags);
    void *DrawIndexedPrimitive;
    void *SetClipStatus;
    void *GetClipStatus;
    void *DrawPrimitiveStrided;
    void *DrawIndexedPrimitiveStrided;
    void *DrawPrimitiveVB;
    void *DrawIndexedPrimitiveVB;
    void *ComputeSphereVisibility;
    void *GetTexture;
    HRESULT (STDMETHODCALLTYPE *SetTexture)(void *self, DWORD stage, void *texture);
    void *GetTextureStageState;
    HRESULT (STDMETHODCALLTYPE *SetTextureStageState)(void *self, DWORD stage, DWORD type, DWORD value);
} GLI_IDirect3DDevice3Vtbl;

typedef struct GLI_IDirect3DViewport3Vtbl {
    HRESULT (STDMETHODCALLTYPE *QueryInterface)(void *self, const GUID *riid, void **ppvObj);
    ULONG (STDMETHODCALLTYPE *AddRef)(void *self);
    ULONG (STDMETHODCALLTYPE *Release)(void *self);
    void *Initialize;
    void *GetViewport;
    void *SetViewport;
    void *TransformVertices;
    void *LightElements;
    void *SetBackground;
    void *GetBackground;
    void *SetBackgroundDepth;
    void *GetBackgroundDepth;
    void *Clear;
    void *AddLight;
    void *DeleteLight;
    void *NextLight;
    void *GetViewport2;
    HRESULT (STDMETHODCALLTYPE *SetViewport2)(void *self, void *viewport);
    void *SetBackgroundDepth2;
    void *GetBackgroundDepth2;
    HRESULT (STDMETHODCALLTYPE *Clear2)(void *self, DWORD count, RECT *rects, DWORD flags, DWORD color, float z, DWORD stencil);
} GLI_IDirect3DViewport3Vtbl;

struct GLI_IDirectDraw {
    const GLI_IUnknownVtbl *lpVtbl;
};

struct GLI_IDirectDraw4 {
    const GLI_IDirectDraw4Vtbl *lpVtbl;
};

struct GLI_IDirectDrawClipper {
    const GLI_IDirectDrawClipperVtbl *lpVtbl;
};

struct GLI_IDirectDrawSurface4 {
    const GLI_IDirectDrawSurface4Vtbl *lpVtbl;
};

struct GLI_IDirect3D3 {
    const GLI_IDirect3D3Vtbl *lpVtbl;
};

struct GLI_IDirect3DDevice3 {
    const GLI_IDirect3DDevice3Vtbl *lpVtbl;
};

struct GLI_IDirect3DViewport3 {
    const GLI_IDirect3DViewport3Vtbl *lpVtbl;
};

typedef struct GLI_DDCAPS_DX6 {
    DWORD dwSize;
    unsigned char payload[GLI_DDCAPS_DX6_SIZE - sizeof(DWORD)];
} GLI_DDCAPS_DX6;

extern GUID GLI_g_DirectDrawGuid;
extern GUID GLI_g_DeviceGuid;
extern GLI_IDirectDraw *GLI_g_pDirectDraw;
extern GLI_IDirectDraw4 *GLI_g_pDirectDraw4;
extern GLI_IDirect3D3 *GLI_g_pDirect3D3;
extern GLI_IDirect3DDevice3 *GLI_g_pDirect3DDevice3;
extern GLI_IDirect3DViewport3 *GLI_g_pDirect3DViewport3;
extern GLI_IDirectDrawSurface4 *GLI_g_pPrimarySurface;
extern GLI_IDirectDrawSurface4 *GLI_g_pBackBufferSurface;
extern GLI_IDirectDrawSurface4 *GLI_g_pZBufferSurface;
extern GLI_DDCAPS_DX6 GLI_g_HalCaps;
extern BOOL GLI_g_IsFullscreen;
extern BOOL GLI_g_ActivateGliFlag;
extern LONG GLI_g_SavedWidth;
extern LONG GLI_g_SavedHeight;
extern LONG GLI_g_SavedBpp;
extern RECT GLI_g_ClientRect;
extern RECT GLI_g_ScreenRect;
extern DWORD GLI_g_DeviceDescFlags;
extern unsigned char GLI_g_DeviceDesc[0xfc];
extern HWND GLI_g_WindowHandle;
extern GLI_tdstInternalGlobalValuesFor3dEngine *GLI_g_GliBigGlobals;
extern DWORD *GLI_g_TextureMemoryChannels;
extern void **GLI_g_TextureAlreadyRead;
extern void *GLI_g_ConvertBufferMipMapping;
extern void *GLI_g_RenderingCounterPointer;
extern void *GLI_g_DrawSem;
extern void *GLI_g_GliCaps;
extern void *GLI_g_NumberOfTextureToCreate;

extern void *GLI_g_pTexTempSurface;
extern void *GLI_g_pTexDestSurface;
extern void *GLI_g_pTexTempTexture;
extern void *GLI_g_pTexDestTexture;
extern DWORD GLI_g_TextureFormatCount;
extern unsigned char GLI_g_TextureFormats[GLI_TEXTURE_FORMAT_RECORD_COUNT][GLI_TEXTURE_FORMAT_RECORD_SIZE];
extern unsigned char GLI_g_SelectedTextureFormats[GLI_TEXTURE_FORMAT_SELECTED_COUNT];
extern DWORD GLI_g_ZWriteEnableState;
extern DWORD GLI_g_ZFuncGreaterequalState;
extern unsigned char GLI_g_TextureStageColorOp;
extern unsigned char GLI_g_TextureAddressMode;
extern long GLI_g_BlackPolygonVertexCount;   /* 0x1000f1cc */
extern WORD GLI_g_FogEnableState;
extern float GLI_g_fBlackPolygonRhw;                                              /* 0x1000d568 */
extern GLI_tdstTLVertex GLI_g_BlackPolygonVertices[GLI_BLACK_POLYGON_MAX_VERTICES];  /* 0x1000f400 */
extern float GLI_g_FogEnd;
extern float GLI_g_FogSlope;
extern float GLI_g_FogStart;
extern float GLI_g_FogOffset;
extern DWORD GLI_g_FogColor;
extern DWORD GLI_g_FogAlphaNear;
extern DWORD GLI_g_FogAlphaFar;
extern DWORD GLI_g_TextureMemoryCostTable[GLI_TEXTURE_COST_FORMAT_COUNT]
                                        [GLI_TEXTURE_COST_PLANE_COUNT]
                                        [GLI_TEXTURE_COST_DIMENSION_COUNT]
                                        [GLI_TEXTURE_COST_DIMENSION_COUNT];
extern GLI_CommonFunction GLI_g_SetGlobalAlpha;
extern GLI_CommonFunction GLI_g_SerialProjection;
extern GLI_CommonFunction GLI_g_IsGliInit;
extern GLI_CommonFunction GLI_g_TmpMalloc;
extern GLI_CommonFunction GLI_g_TmpFree;
extern GLI_CommonFunction GLI_g_ErmPrintMessage;
extern GLI_CommonFunction GLI_g_ComputeTextureSize;
extern GLI_CommonFunction GLI_g_CompressTex565;
extern GLI_CommonFunction GLI_g_CompressTex4444;
extern GLI_CommonFunction GLI_g_CompressTex1555;
extern GLI_CommonFunction GLI_g_DivideBitmapSurfaceBy2;
extern GLI_CommonFunction GLI_g_DivideBitmapSurfaceBy2NonZero;
extern GLI_CommonFunction GLI_g_DivideBitmapSurfaceBy2Palette;
extern GLI_CommonFunction GLI_g_VerifyTextureIsReallyAlpha;
extern GLI_CommonFunction GLI_g_LoadTextureInTexelField;
extern GLI_CommonFunction GLI_g_ReloadTextures;
extern GLI_CommonFunction GLI_g_AddDisplayMode;
extern GLI_CommonFunction GLI_g_BlitStretched16b;
extern GLI_CommonFunction GLI_g_GetDefinedPixelFormat;
extern GLI_CommonFunction GLI_g_AddDefinedPixelFormat;
extern GLI_CommonFunction GLI_g_ComputeWaitFrameForSmoothSynchro;
extern GLI_CommonFunction GLI_g_SimulateFlipSynchroWithoutVSync;
extern GLI_CommonFunction GLI_g_ReadIniString;
extern GLI_CommonFunction GLI_g_WriteIniString;
extern GLI_CommonFunction GLI_g_UseMipMaps;
extern GLI_CommonFunction GLI_g_TimerGetCounter;
extern GLI_CommonFunction GLI_g_TimerTickPerSecond;
extern GLI_CommonFunction GLI_g_AddToProgressBar;

extern long (__cdecl *GLI_g_pAddDisplayInfo)(long, long, long, const char *, long);
extern long GLI_g_DisplayConfigDisplayNode;
extern long GLI_g_DisplayConfigDeviceNode;

#define GLI_STAGING_BUFFER_SIZE  0x17710
#define GLI_PIPELINE_LABEL_MAX   64

typedef void (__cdecl *GLI_PipelineLabel)(void);

/* 0x1000fa20 */
extern unsigned char GLI_g_StagingBuffer[GLI_STAGING_BUFFER_SIZE];
/* 0x1000d020 */
extern unsigned char *GLI_g_pStagingCursor;
/* 0x1000d01c */
extern DWORD GLI_g_DrawFlags;
/* 0x1000df60 */
extern GLI_PipelineLabel GLI_g_PipelineLabels[GLI_PIPELINE_LABEL_MAX];
/* 0x1000df54 */
extern DWORD GLI_g_PipelineCount;
/* 0x1000fa18 */
extern DWORD GLI_g_PipelineBitmask;

extern float GLI_g_ClipXMin;
extern float GLI_g_ClipXMax;
extern float GLI_g_ClipYMin;
extern float GLI_g_ClipYMax;
extern float GLI_g_ClipYGate;
extern float GLI_g_ClipXGate;

extern GLI_PipelineLabel *GLI_g_pPipelineCursor;

extern float *GLI_g_pUVSrc[3];

extern short *GLI_g_pIndexArray;

extern short *GLI_g_pAltIndexArray;

extern void *GLI_g_pElementBlock;

extern float GLI_g_VertexStride; /* 0x1000dfb0 */

extern float GLI_g_UVScrollU;
extern float GLI_g_UVScrollV;

#define GLI_UV_SCALE   0.5f
#define GLI_UV_V_BIAS  (-1.0f)

typedef void (__cdecl *GLI_ProjCallback)(unsigned int ctxField, int mode,
                                         float *xy_in, void *xyzrhw_out);
#define GLI_ProjNearPlaneCallback ((GLI_ProjCallback)GLI_g_SerialProjection)

#ifdef __cplusplus
}
#endif
