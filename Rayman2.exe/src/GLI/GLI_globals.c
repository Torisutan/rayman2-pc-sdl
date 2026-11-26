#include "GLI/GLI_globals.h"
#include "GLI/ErrGLI.h"

unsigned char g_ucGliModuleId = 0xFF;

/* 0x004a7280 */
void                                          *GLI_g_hConcatTexturesFile;

void                 *GLI_gs_p_ConvertBufferMipMapping;
unsigned long         GLI_g_ulNumberOfLoadedTexture;
long                  GLI_gs_lNumberOfTextureToCreate;

/* 0x00504088 */
char g_cSinusEffectFlag;

struct GLI_tdstTexture *GLI_gs_aDEFTableOfTextureAlreadyRead[1024];
unsigned long           GLI_gs_aDEFTableOfTextureMemoryChannels[1024];

/* 0x00506b00 */
struct POS_tdstCompletePosition_              *GLI_g_p_stCurrentMatrix;

/* 0x005036ac */
struct GLI_tdstInternalGlobalValuesFor3dEngine_ *GLI_BIG_GLOBALS;

void (*GLI_DRV_g_vSendSpriteToClipWithUV)(               /* 0x00504524 */
        struct GLI_tdstAligned2DVector_ *,
        MTH_tdxReal *,
        MTH_tdxReal,
        struct GLI_tdstInternalGlobalValuesFor3dEngine_ *);

void (*GLI_DRV_g_vSendSpriteToClip)(                     /* 0x00504530 */
        struct GLI_tdstAligned2DVector_ *,
        MTH_tdxReal,
        struct GLI_tdstInternalGlobalValuesFor3dEngine_ *);

void (*GLI_DRV_g_vSendSpriteToClipWithColors)(           /* 0x00504838 */
        struct GLI_tdstAligned2DVector_ *,
        unsigned long *,
        unsigned long,
        struct GLI_tdstInternalGlobalValuesFor3dEngine_ *);

void (*GLI_DRV_g_vFlipDeviceWithSyncro)(void);           /* 0x00504538 */

void (*GLI_DRV_g_vFlipDevice)(void);                     /* 0x00504818 */

void (*GLI_DRV_g_vSendSingleTrianglesToClip)(            /* 0x00504828 */
        struct GLI_tdstAligned2DVector_ *,
        struct GLI_tdst2DUVValues_      *,
        struct GLI_tdstInternalGlobalValuesFor3dEngine_ *);

void (*GLI_DRV_g_vSendSingleLineToClip)(                 /* 0x00504834 */
        struct GLD_tdstViewportAttributes *,
        struct GLI_tdstAligned3DVector_    *,
        struct GLI_tdstAligned2DVector_    *,
        struct GLI_tdstAligned3DVector_    *,
        struct GLI_tdstAligned2DVector_    *,
        struct GLI_tdstInternalGlobalValuesFor3dEngine_ *,
        long,
        struct GEO_tdstColor              *);

int (*GLI_DRV_g_bEndScene)(void);                        /* 0x00504844 */

void (*GLI_DRV_g_xSendListToViewport)(                   /* 0x00504870 */
        struct GLD_tdstViewportAttributes *);

void (*GLI_DRV_g_vReadaptDisplay)(void);                 /* 0x00504878 */

void *GLI_DRV_g_vWrite16bBitmapToBackBuffer;   /* 0x00504540 */
void *GLI_DRV_g_vAddBlackPolygon;              /* 0x0050483C */
void *GLI_DRV_g_vNoBlackPolygon;               /* 0x00504880 */
void *GLI_DRV_g_bBeginScene;                   /* 0x0050486C */
void *GLI_DRV_g_vDownLoadTextures;             /* 0x0050481C */
void *GLI_DRV_g_vUnLoadTextures;               /* 0x0050480C */
void *GLI_DRV_g_lGetSizeOfTexture;             /* 0x00504824 */
void *GLI_DRV_g_vClearDevice;                  /* 0x00504520 */
void *GLI_DRV_g_vCloseDriver;                  /* 0x00504868 */
void *GLI_DRV_g_vClearZBufferRegion;           /* 0x0050485C */
void *GLI_DRV_g_fnl_EnumModes;                 /* 0x00504850 */
void *GLI_DRV_g_hChangeMode;                   /* 0x00504820 */
void *GLI_DRV_g_bLockDevice;                   /* 0x00504860 */
void *GLI_DRV_g_bUnlockDevice;                 /* 0x0050453C */
void *GLI_DRV_g_bPrepareForGliWindowed;        /* 0x00504874 */
void *GLI_DRV_g_vPrepareForGliFullScreen;      /* 0x0050484C */
void *GLI_DRV_g_vActivateGli;                  /* 0x00504814 */
void *GLI_DRV_g_bWindowedModeIsOptimized;      /* 0x00504810 */
void *GLI_DRV_g_vOptimizedWindowedMode;        /* 0x0050487C */
void *GLI_DRV_g_vNonOptimizedWindowedMode;     /* 0x00504864 */
void *GLI_DRV_g_xClearViewingList;             /* 0x00504848 */
void *GLI_DRV_g_xSendElementTIToClip_TRIANGLES;/* 0x00504854 */
void *GLI_DRV_g_lRequest;                      /* 0x0050452C */

char GLI_pc_g_aszConfiguredDllName[0x40];
char GLI_pc_g_aszDllInfoScratch[0x100];
char GLI_pc_g_aszDllVersion[0x10];
char GLI_pc_g_aszDllCode[0x4];
char GLI_pc_g_aszDllFileName[0x104];

char GLI_pc_g_aszEnumModesDriver[0x20];
char GLI_pc_g_aszEnumModesDevice[0x20];

/* 0x004A05C0 */
char GLI_pc_g_aszTexArchiveFileName[0x100] = "BINARY_TEX_GLOBALS.BIN";

/* 0x004A4024 */
unsigned char GLI_pc_g_aucErrorChannelId = 0xff;
unsigned char GLI_pc_g_aucRandomPickEnabled;

const float GLI_K_xOne = 1.0f;   /* 0x0049C63C */

const float GLI_K_xOneOver255 = (float)(1.0 / 255.0);   /* 0x0049C638 */
const float GLI_PRJ_K_xHalf   = 0.5f;                    /* 0x0049C64C */

unsigned long GLI_pc_g_ulRequestedWidth;
unsigned long GLI_pc_g_ulRequestedHeight;
unsigned long GLI_pc_g_ulRequestedBpp;

#include "GLI/sprite.h"
tdstLensFlareTab GLI_pc_g_stLensFlareTab;   /* 0x005036C0 */

#include "GLI/devinfo.h"
tdstGliCaps GLI_pc_g_stGliCaps;   /* 0x00503680 */

long          GLI_pc_g_a3_xWaterDepthColorCoef[3];
unsigned long GLI_pc_g_ulWaterDepthColorFlags;

MTH3D_tdstVector GLI_pc_stSinEffectAmp;
float GLI_pc_a64_x2CosTable[64];
long  GLI_pc_a3_lSinEffectFreq[3];
long  GLI_pc_a3_lSinEffectBase[3];
long  GLI_pc_a3_lSinRLIAmp[3];
long  GLI_pc_a3_lSinRLIBase[3];
long  GLI_pc_lSinEffectBase;

unsigned short GLI_pc_g_uwOverlayDevice;     /* 0x004B9AAC */
unsigned short GLI_pc_g_uwOverlayViewport;   /* 0x004B9AB0 */
void          *GLI_pc_g_hOverlaySemaphore;
GLI_pc_tdstProgressUi GLI_pc_g_stProgressUi;   /* 0x005163E0 */
unsigned char *GLI_pc_g_p_ucLoadingScreenBitmap;
void          *GLI_pc_g_hCdFallbackFile;

/* 0x0050409C */
unsigned char GLI_pc_g_cSinusEffectState;

void *GLI_DRV_g_vDoOpaqueTextureSelection;
void *GLI_DRV_g_vSetZClip;
void *GLI_DRV_g_vSetClipWindow;
void *GLI_DRV_g_vComputeFogEffect;

void *GLI_pc_g_hDefaultMaterial;

/* 0x00503d54 */
unsigned long GLI_g_ul503d54;

/* 0x004A7238 */
unsigned long GLI_pc_g_ulNbDisplayModes;

long GLI_pc_g_lTickBudget;   /* 0x004A7218 */
