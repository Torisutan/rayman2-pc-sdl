#include "GliDX6vf/GliDX6vf_State.h"

GUID GLI_g_DirectDrawGuid = {0};
GUID GLI_g_DeviceGuid = {0};
GLI_IDirectDraw *GLI_g_pDirectDraw = NULL;
GLI_IDirectDraw4 *GLI_g_pDirectDraw4 = NULL;
GLI_IDirect3D3 *GLI_g_pDirect3D3 = NULL;
GLI_IDirect3DDevice3 *GLI_g_pDirect3DDevice3 = NULL;
GLI_IDirect3DViewport3 *GLI_g_pDirect3DViewport3 = NULL;
GLI_IDirectDrawSurface4 *GLI_g_pPrimarySurface = NULL;
GLI_IDirectDrawSurface4 *GLI_g_pBackBufferSurface = NULL;
GLI_IDirectDrawSurface4 *GLI_g_pZBufferSurface = NULL;
GLI_DDCAPS_DX6 GLI_g_HalCaps;
/* 0x1000DF44 */
BOOL GLI_g_IsFullscreen = TRUE;
BOOL GLI_g_ActivateGliFlag = TRUE;
LONG GLI_g_SavedWidth = 0;
LONG GLI_g_SavedHeight = 0;
LONG GLI_g_SavedBpp = 0;
RECT GLI_g_ClientRect = {0};
RECT GLI_g_ScreenRect = {0};
DWORD GLI_g_DeviceDescFlags = 0;
unsigned char GLI_g_DeviceDesc[0xfc] = {0};
HWND GLI_g_WindowHandle = NULL;
void *GLI_g_GliBigGlobals = NULL;
DWORD *GLI_g_TextureMemoryChannels = NULL;
void **GLI_g_TextureAlreadyRead = NULL;
void *GLI_g_ConvertBufferMipMapping = NULL;
void *GLI_g_RenderingCounterPointer = NULL;
void *GLI_g_DrawSem = NULL;
void *GLI_g_GliCaps = NULL;
void *GLI_g_NumberOfTextureToCreate = NULL;
void *GLI_g_pTexTempSurface = NULL;
void *GLI_g_pTexDestSurface = NULL;
void *GLI_g_pTexTempTexture = NULL;
void *GLI_g_pTexDestTexture = NULL;
DWORD GLI_g_TextureFormatCount = 0;
unsigned char GLI_g_TextureFormats[GLI_TEXTURE_FORMAT_RECORD_COUNT][GLI_TEXTURE_FORMAT_RECORD_SIZE];
unsigned char GLI_g_SelectedTextureFormats[GLI_TEXTURE_FORMAT_SELECTED_COUNT];
/* 0x1000D55C */
DWORD GLI_g_ZWriteEnableState = 1;
DWORD GLI_g_ZFuncGreaterequalState = 0;
unsigned char GLI_g_TextureStageColorOp = 0xff;
unsigned char GLI_g_TextureAddressMode = 0xff;
long GLI_g_BlackPolygonVertexCount = 0;
WORD GLI_g_FogEnableState = 0;
float GLI_g_fBlackPolygonRhw = 0.8f;
GLI_tdstTLVertex GLI_g_BlackPolygonVertices[GLI_BLACK_POLYGON_MAX_VERTICES];
float GLI_g_FogEnd = 0.0f;
float GLI_g_FogSlope = 0.0f;
float GLI_g_FogStart = 0.0f;
float GLI_g_FogOffset = 0.0f;
DWORD GLI_g_FogColor = 0;
DWORD GLI_g_FogAlphaNear = 0;
DWORD GLI_g_FogAlphaFar = 0;
DWORD GLI_g_TextureMemoryCostTable[GLI_TEXTURE_COST_FORMAT_COUNT]
                                  [GLI_TEXTURE_COST_PLANE_COUNT]
                                  [GLI_TEXTURE_COST_DIMENSION_COUNT]
                                  [GLI_TEXTURE_COST_DIMENSION_COUNT];
GLI_CommonFunction GLI_g_SetGlobalAlpha = NULL;
GLI_CommonFunction GLI_g_SerialProjection = NULL;
GLI_CommonFunction GLI_g_IsGliInit = NULL;
GLI_CommonFunction GLI_g_TmpMalloc = NULL;
GLI_CommonFunction GLI_g_TmpFree = NULL;
GLI_CommonFunction GLI_g_ErmPrintMessage = NULL;
GLI_CommonFunction GLI_g_ComputeTextureSize = NULL;
GLI_CommonFunction GLI_g_CompressTex565 = NULL;
GLI_CommonFunction GLI_g_CompressTex4444 = NULL;
GLI_CommonFunction GLI_g_CompressTex1555 = NULL;
GLI_CommonFunction GLI_g_DivideBitmapSurfaceBy2 = NULL;
GLI_CommonFunction GLI_g_DivideBitmapSurfaceBy2NonZero = NULL;
GLI_CommonFunction GLI_g_DivideBitmapSurfaceBy2Palette = NULL;
GLI_CommonFunction GLI_g_VerifyTextureIsReallyAlpha = NULL;
GLI_CommonFunction GLI_g_LoadTextureInTexelField = NULL;
GLI_CommonFunction GLI_g_ReloadTextures = NULL;
GLI_CommonFunction GLI_g_AddDisplayMode = NULL;
GLI_CommonFunction GLI_g_BlitStretched16b = NULL;
GLI_CommonFunction GLI_g_GetDefinedPixelFormat = NULL;
GLI_CommonFunction GLI_g_AddDefinedPixelFormat = NULL;
GLI_CommonFunction GLI_g_ComputeWaitFrameForSmoothSynchro = NULL;
GLI_CommonFunction GLI_g_SimulateFlipSynchroWithoutVSync = NULL;
GLI_CommonFunction GLI_g_ReadIniString = NULL;
GLI_CommonFunction GLI_g_WriteIniString = NULL;
GLI_CommonFunction GLI_g_UseMipMaps = NULL;
GLI_CommonFunction GLI_g_TimerGetCounter = NULL;
GLI_CommonFunction GLI_g_TimerTickPerSecond = NULL;
GLI_CommonFunction GLI_g_AddToProgressBar = NULL;

long (__cdecl *GLI_g_pAddDisplayInfo)(long, long, long, const char *, long) = NULL;
long GLI_g_DisplayConfigDisplayNode = 0;
long GLI_g_DisplayConfigDeviceNode = 0;

unsigned char GLI_g_StagingBuffer[GLI_STAGING_BUFFER_SIZE];
unsigned char *GLI_g_pStagingCursor = GLI_g_StagingBuffer;   /* 0x1000D020 */
/* 0x10001300 */
DWORD GLI_g_DrawFlags = 0x14;
GLI_PipelineLabel GLI_g_PipelineLabels[GLI_PIPELINE_LABEL_MAX];
DWORD GLI_g_PipelineCount = 0;
DWORD GLI_g_PipelineBitmask = 0;
float GLI_g_ClipXMin = 0.0f;
float GLI_g_ClipXMax = 0.0f;
float GLI_g_ClipYMin = 0.0f;
float GLI_g_ClipYMax = 0.0f;
float GLI_g_ClipYGate = 0.0f;
float GLI_g_ClipXGate = 0.0f;
GLI_PipelineLabel *GLI_g_pPipelineCursor = NULL;
float *GLI_g_pUVSrc[3] = { NULL, NULL, NULL };
short *GLI_g_pIndexArray    = NULL;
short *GLI_g_pAltIndexArray = NULL;
void  *GLI_g_pElementBlock  = NULL;
float  GLI_g_VertexStride         = 0.0f;
float  GLI_g_UVScrollU         = 0.0f;
float  GLI_g_UVScrollV         = 0.0f;
