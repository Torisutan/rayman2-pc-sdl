#pragma once

#include "MTH/MTH_types.h"

struct GLI_tdstTexture;
struct GLI_tdstInternalGlobalValuesFor3dEngine_;
struct GLI_tdstAligned2DVector_;
struct GLI_tdstAligned3DVector_;
struct GLI_tdst2DUVValues_;
struct GLD_tdstViewportAttributes;
struct GEO_tdstColor;
struct POS_tdstCompletePosition_;

/* 0x004a7280 */
extern void *GLI_g_hConcatTexturesFile;

/* 0x00503d50 */
extern void *GLI_gs_p_ConvertBufferMipMapping;

/* 0x00502660 */
extern unsigned long GLI_g_ulNumberOfLoadedTexture;

/* 0x00501644 */
extern long GLI_gs_lNumberOfTextureToCreate;

/* 0x00502680 */
extern struct GLI_tdstTexture *GLI_gs_aDEFTableOfTextureAlreadyRead[1024];

/* 0x00501660 */
extern unsigned long GLI_gs_aDEFTableOfTextureMemoryChannels[1024];

/* 0x00506b00 */
extern struct POS_tdstCompletePosition_ *GLI_g_p_stCurrentMatrix;

/* 0x005036ac */
extern struct GLI_tdstInternalGlobalValuesFor3dEngine_ *GLI_BIG_GLOBALS;

/* 0x00504524 */
extern void (*GLI_DRV_g_vSendSpriteToClipWithUV)(
    struct GLI_tdstAligned2DVector_ *a4_st2DVertex,
    MTH_tdxReal *a8_stUVVertex,
    MTH_tdxReal  xZ,
    struct GLI_tdstInternalGlobalValuesFor3dEngine_ *p_stGlobals);
#define GLI_DRV_vSendSpriteToClipWithUV_ (&GLI_DRV_g_vSendSpriteToClipWithUV)

/* 0x00504530 */
extern void (*GLI_DRV_g_vSendSpriteToClip)(
    struct GLI_tdstAligned2DVector_ *a4_st2DVertex,
    MTH_tdxReal  xZ,
    struct GLI_tdstInternalGlobalValuesFor3dEngine_ *p_stGlobals);
#define GLI_DRV_vSendSpriteToClip_ (&GLI_DRV_g_vSendSpriteToClip)

/* 0x00504838 */
extern void (*GLI_DRV_g_vSendSpriteToClipWithColors)(
    struct GLI_tdstAligned2DVector_ *a4_st2DVertex,
    unsigned long                  *a4_ulColors,
    unsigned long                   ulZBits,
    struct GLI_tdstInternalGlobalValuesFor3dEngine_ *p_stGlobals);
#define GLI_DRV_vSendSpriteToClipWithColors_ (&GLI_DRV_g_vSendSpriteToClipWithColors)

/* 0x00504538 */
extern void (*GLI_DRV_g_vFlipDeviceWithSyncro)(void);
#define GLI_DRV_vFlipDeviceWithSyncro_ (&GLI_DRV_g_vFlipDeviceWithSyncro)

/* 0x00504818 */
extern void (*GLI_DRV_g_vFlipDevice)(void);
#define GLI_DRV_vFlipDevice (&GLI_DRV_g_vFlipDevice)

/* 0x00504828 */
extern void (*GLI_DRV_g_vSendSingleTrianglesToClip)(
    struct GLI_tdstAligned2DVector_ *a3_st2DVertex,
    struct GLI_tdst2DUVValues_      *a3_stUV,
    struct GLI_tdstInternalGlobalValuesFor3dEngine_ *p_stGlobaleMT);
#define GLI_DRV_vSendSingleTrianglesToClip_ (&GLI_DRV_g_vSendSingleTrianglesToClip)

/* 0x00504834 */
extern void (*GLI_DRV_g_vSendSingleLineToClip)(
    struct GLD_tdstViewportAttributes *p_stVpt,
    struct GLI_tdstAligned3DVector_    *p_stVertex1,
    struct GLI_tdstAligned2DVector_    *p_st2DVertex1,
    struct GLI_tdstAligned3DVector_    *p_stVertex2,
    struct GLI_tdstAligned2DVector_    *p_st2DVertex2,
    struct GLI_tdstInternalGlobalValuesFor3dEngine_ *p_stGlobals,
    long                               lDrawModeMask,
    struct GEO_tdstColor              *p_stColor);
#define GLI_DRV_vSendSingleLineToClip_ (&GLI_DRV_g_vSendSingleLineToClip)

/* 0x00504844 */
extern int (*GLI_DRV_g_bEndScene)(void);
#define GLI_DRV_bEndScene_ (&GLI_DRV_g_bEndScene)

/* 0x00504870 */
extern void (*GLI_DRV_g_xSendListToViewport)(
    struct GLD_tdstViewportAttributes *p_stVpt);
#define GLI_DRV_xSendListToViewport_ (&GLI_DRV_g_xSendListToViewport)

/* 0x00504878 */
extern void (*GLI_DRV_g_vReadaptDisplay)(void);
#define GLI_DRV_vReadaptDisplay_ (&GLI_DRV_g_vReadaptDisplay)

extern void *GLI_DRV_g_vWrite16bBitmapToBackBuffer;   /* 0x00504540 */
extern void *GLI_DRV_g_vAddBlackPolygon;              /* 0x0050483C */
extern void *GLI_DRV_g_vNoBlackPolygon;               /* 0x00504880 */
extern void *GLI_DRV_g_bBeginScene;                   /* 0x0050486C */
extern void *GLI_DRV_g_vDownLoadTextures;             /* 0x0050481C */
extern void *GLI_DRV_g_vUnLoadTextures;               /* 0x0050480C */
extern void *GLI_DRV_g_lGetSizeOfTexture;             /* 0x00504824 */
extern void *GLI_DRV_g_vClearDevice;                  /* 0x00504520 */
extern void *GLI_DRV_g_vCloseDriver;                  /* 0x00504868 */
extern void *GLI_DRV_g_vClearZBufferRegion;           /* 0x0050485C */
extern void *GLI_DRV_g_fnl_EnumModes;                 /* 0x00504850 */
extern void *GLI_DRV_g_hChangeMode;                   /* 0x00504820 */
extern void *GLI_DRV_g_bLockDevice;                   /* 0x00504860 */
extern void *GLI_DRV_g_bUnlockDevice;                 /* 0x0050453C */
extern void *GLI_DRV_g_bPrepareForGliWindowed;        /* 0x00504874 */
extern void *GLI_DRV_g_vPrepareForGliFullScreen;      /* 0x0050484C */
extern void *GLI_DRV_g_vActivateGli;                  /* 0x00504814 */
extern void *GLI_DRV_g_bWindowedModeIsOptimized;      /* 0x00504810 */
extern void *GLI_DRV_g_vOptimizedWindowedMode;        /* 0x0050487C */
extern void *GLI_DRV_g_vNonOptimizedWindowedMode;     /* 0x00504864 */
extern void *GLI_DRV_g_xClearViewingList;             /* 0x00504848 */
extern void *GLI_DRV_g_xSendElementTIToClip_TRIANGLES;/* 0x00504854 */
extern void *GLI_DRV_g_lRequest;                      /* 0x0050452C */
#define GLI_DRV_vWrite16bBitmapToBackBuffer      (&GLI_DRV_g_vWrite16bBitmapToBackBuffer)
#define GLI_DRV_vAddBlackPolygon                 (&GLI_DRV_g_vAddBlackPolygon)
#define GLI_DRV_vNoBlackPolygon                  (&GLI_DRV_g_vNoBlackPolygon)
#define GLI_DRV_bBeginScene                      (&GLI_DRV_g_bBeginScene)
#define GLI_DRV_vDownLoadTextures                (&GLI_DRV_g_vDownLoadTextures)
#define GLI_DRV_vUnLoadTextures                  (&GLI_DRV_g_vUnLoadTextures)
#define GLI_DRV_lGetSizeOfTexture                (&GLI_DRV_g_lGetSizeOfTexture)
#define GLI_DRV_vClearDevice                     (&GLI_DRV_g_vClearDevice)
#define GLI_DRV_vCloseDriver                     (&GLI_DRV_g_vCloseDriver)
#define GLI_DRV_vClearZBufferRegion              (&GLI_DRV_g_vClearZBufferRegion)
#define GLI_DRV_fnl_EnumModes                    (&GLI_DRV_g_fnl_EnumModes)
#define GLI_DRV_hChangeMode                      (&GLI_DRV_g_hChangeMode)
#define GLI_DRV_bLockDevice                      (&GLI_DRV_g_bLockDevice)
#define GLI_DRV_bUnlockDevice                    (&GLI_DRV_g_bUnlockDevice)
#define GLI_DRV_bPrepareForGliWindowed           (&GLI_DRV_g_bPrepareForGliWindowed)
#define GLI_DRV_vPrepareForGliFullScreen         (&GLI_DRV_g_vPrepareForGliFullScreen)
#define GLI_DRV_vActivateGli                     (&GLI_DRV_g_vActivateGli)
#define GLI_DRV_bWindowedModeIsOptimized         (&GLI_DRV_g_bWindowedModeIsOptimized)
#define GLI_DRV_vOptimizedWindowedMode           (&GLI_DRV_g_vOptimizedWindowedMode)
#define GLI_DRV_vNonOptimizedWindowedMode        (&GLI_DRV_g_vNonOptimizedWindowedMode)
#define GLI_DRV_xClearViewingList                (&GLI_DRV_g_xClearViewingList)
#define GLI_DRV_xSendElementTIToClip_TRIANGLES   (&GLI_DRV_g_xSendElementTIToClip_TRIANGLES)
#define GLI_DRV_lRequest                         (&GLI_DRV_g_lRequest)

/* 0x00503d54 */
extern unsigned long GLI_g_ul503d54;

/* 0x004A7238 */
extern unsigned long GLI_pc_g_ulNbDisplayModes;

/* 0x004A7218 */
extern long GLI_pc_g_lTickBudget;

extern char GLI_pc_g_aszConfiguredDllName[0x40];
extern char GLI_pc_g_aszDllInfoScratch[0x100];
extern char GLI_pc_g_aszDllVersion[0x10];
extern char GLI_pc_g_aszDllCode[0x4];
extern char GLI_pc_g_aszDllFileName[0x104];
#define GLI_pc_g_szConfiguredDllName (GLI_pc_g_aszConfiguredDllName)
#define GLI_pc_g_szDllInfoScratch    (GLI_pc_g_aszDllInfoScratch)
#define GLI_pc_g_szDllVersion        (GLI_pc_g_aszDllVersion)
#define GLI_pc_g_szDllCode           (GLI_pc_g_aszDllCode)
#define GLI_pc_g_szDllFileName       (GLI_pc_g_aszDllFileName)

/* 0x00504088 */
extern char g_cSinusEffectFlag;

extern char GLI_pc_g_aszEnumModesDriver[0x20];
extern char GLI_pc_g_aszEnumModesDevice[0x20];
#define GLI_pc_g_szEnumModesDriver (GLI_pc_g_aszEnumModesDriver)
#define GLI_pc_g_szEnumModesDevice (GLI_pc_g_aszEnumModesDevice)

/* 0x004A4024 */
extern unsigned char GLI_pc_g_aucErrorChannelId;
extern unsigned char GLI_pc_g_aucRandomPickEnabled;
#define GLI_pc_g_ucErrorChannelId    GLI_pc_g_aucErrorChannelId
#define GLI_pc_g_ucRandomPickEnabled GLI_pc_g_aucRandomPickEnabled

/* 0x004A05C0 */
extern char GLI_pc_g_aszTexArchiveFileName[0x100];
#define GLI_pc_g_szTexArchiveFileName (GLI_pc_g_aszTexArchiveFileName)

/* 0x0049C63C */
extern const float GLI_K_xOne;

/* 0x0049C638 */
extern const float GLI_K_xOneOver255;
extern const float GLI_PRJ_K_xHalf;

extern unsigned long GLI_pc_g_ulRequestedWidth;   /* 0x005047FC */
extern unsigned long GLI_pc_g_ulRequestedHeight;  /* 0x00504800 */
extern unsigned long GLI_pc_g_ulRequestedBpp;     /* 0x00504808 */

/* 0x005036C0 */
extern int           GLI_pc_aSpriteRegKey[10];
extern int           GLI_pc_aSpriteRegValue[10];
extern int           GLI_pc_aSpriteRegState[10];
extern unsigned char GLI_pc_aSpriteRegFlag[10];
extern int           GLI_pc_lSpriteRegCount;

extern long          GLI_pc_g_a3_xWaterDepthColorCoef[3];   /* 0x504070 */
extern unsigned long GLI_pc_g_ulWaterDepthColorFlags;       /* 0x504080 */
#define GLI_M_pWaterDepthColorCoef  GLI_pc_g_a3_xWaterDepthColorCoef
#define GLI_M_WaterDepthColorFlags  GLI_pc_g_ulWaterDepthColorFlags

extern MTH3D_tdstVector GLI_pc_stSinEffectAmp;    /* 0x5040D0 */
extern float GLI_pc_a64_x2CosTable[64];           /* 0x5040E0 */
extern long  GLI_pc_a3_lSinEffectFreq[3];         /* 0x5042E0 */
extern long  GLI_pc_a3_lSinEffectBase[3];         /* 0x5042F0 */
#define GLI_M_stSinEffectAmp    GLI_pc_stSinEffectAmp
#define GLI_M_p2CosTable        GLI_pc_a64_x2CosTable
#define GLI_M_a3_lSinEffectFreq GLI_pc_a3_lSinEffectFreq
#define GLI_M_a3_lSinEffectBase GLI_pc_a3_lSinEffectBase

extern long GLI_pc_a3_lSinRLIAmp[3];    /* 0x504040 */
extern long GLI_pc_a3_lSinRLIBase[3];   /* 0x5040A0 */
#define GLI_M_a3_lSinRLIAmp   GLI_pc_a3_lSinRLIAmp
#define GLI_M_a3_lSinRLIBase  GLI_pc_a3_lSinRLIBase

extern long GLI_pc_lSinEffectBase;    /* 0x004A7240 */
#define GLI_M_lSinEffectBase  GLI_pc_lSinEffectBase

extern unsigned short GLI_pc_g_uwOverlayDevice;            /* 0x004B9AAC */
extern unsigned short GLI_pc_g_uwOverlayViewport;          /* 0x004B9AB0 */
extern void          *GLI_pc_g_hOverlaySemaphore;          /* 0x004B9AB4 */

#include "GLI/PixFmt.h"
typedef struct GLI_pc_stProgressUi
{
    char                szName[0x208];
    long                lBufferSize;     /* 0x5165E8 */
    void               *p_vBuffer;       /* 0x5165EC */
    long                lBufferDimC;     /* 0x5165F0 */
    long                lBufferDim8;     /* 0x5165F4 */
    long                lFormatKind;     /* 0x5165F8 */
    GLD_tdstPixelFormat stPixelFormat;   /* 0x5165FC */
    /* 0x45f7e0 */
    unsigned char       ucProgressBarEnabled;
    unsigned char       aucPad25D[3];
    unsigned long       ulProgressBarDivisor;
    unsigned long       ulProgressBarAccum;
    unsigned long       ulProgressBarLastPos;
    unsigned long       ulProgressBarCurPos;
    long                lBarXStart;
    long                lBarYStart;
    long                lBarXEnd;
    long                lBarYEnd;
    float               a_xBorderBlend[4];
    float               a_xBackgroundBlend[4];
    float               a_xCornerBlend[4][4];
} GLI_pc_tdstProgressUi;
extern GLI_pc_tdstProgressUi GLI_pc_g_stProgressUi;   /* 0x005163E0 */

extern unsigned char *GLI_pc_g_p_ucLoadingScreenBitmap;    /* 0x004B9AB8 */
extern void          *GLI_pc_g_hCdFallbackFile;            /* 0x004B9ABC */
#define GLI_AA_uwOverlayWidth     OFFSET GLI_pc_g_uwOverlayDevice
#define GLI_AA_uwOverlayHeight    OFFSET GLI_pc_g_uwOverlayViewport
#define GLI_AA_hOverlaySemaphore  OFFSET GLI_pc_g_hOverlaySemaphore
#define GLI_AA_ProgressUiObject   OFFSET GLI_pc_g_stProgressUi

/* 0x0050409C */
extern unsigned char GLI_pc_g_cSinusEffectState;
#define GLI_AA_cSinusEffectState  OFFSET GLI_pc_g_cSinusEffectState

extern void *GLI_DRV_g_vDoOpaqueTextureSelection;   /* 0x00504528 */
extern void *GLI_DRV_g_vSetZClip;                   /* 0x0050482C */
extern void *GLI_DRV_g_vSetClipWindow;              /* 0x00504534 */
extern void *GLI_DRV_g_vComputeFogEffect;           /* 0x00504830 */
#define GLI_DRV_vDoOpaqueTextureSelection_PTR (&GLI_DRV_g_vDoOpaqueTextureSelection)
#define GLI_DRV_vSetZClip_PTR                 (&GLI_DRV_g_vSetZClip)
#define GLI_DRV_vSetClipWindow_PTR            (&GLI_DRV_g_vSetClipWindow)
#define GLI_DRV_vComputeFogEffect_PTR         (&GLI_DRV_g_vComputeFogEffect)
#define GLI_AA_DrvDoOpaqueTextureSelection    OFFSET GLI_DRV_g_vDoOpaqueTextureSelection
#define GLI_AA_DrvSetZClip                    OFFSET GLI_DRV_g_vSetZClip
#define GLI_AA_DrvSetClipWindow               OFFSET GLI_DRV_g_vSetClipWindow
#define GLI_AA_DrvComputeFogEffect            OFFSET GLI_DRV_g_vComputeFogEffect

/* 0x005036B0 */
extern void *GLI_pc_g_hDefaultMaterial;
#define GLI_pc_p_hDefaultMaterial (&GLI_pc_g_hDefaultMaterial)
#define GLI_AA_DefaultMaterial    OFFSET GLI_pc_g_hDefaultMaterial

/* 0x0051663C */
extern unsigned char GLI_pc_g_bProgressBarEnabled;
extern unsigned long GLI_pc_g_lProgressBarDivisor;
extern unsigned long GLI_pc_g_lProgressBarAccum;
extern unsigned long GLI_pc_g_lProgressBarLastPos;
extern unsigned long GLI_pc_g_lProgressBarCurPos;
extern unsigned long GLI_pc_g_lProgressBarStart;
extern unsigned long GLI_pc_g_lProgressBarEnd;
#define GLI_pc_g_bProgressBarEnabled  (GLI_pc_g_stProgressUi.ucProgressBarEnabled)
#define GLI_pc_g_lProgressBarDivisor  (GLI_pc_g_stProgressUi.ulProgressBarDivisor)
#define GLI_pc_g_lProgressBarAccum    (GLI_pc_g_stProgressUi.ulProgressBarAccum)
#define GLI_pc_g_lProgressBarLastPos  (GLI_pc_g_stProgressUi.ulProgressBarLastPos)
#define GLI_pc_g_lProgressBarCurPos   (GLI_pc_g_stProgressUi.ulProgressBarCurPos)
#define GLI_pc_g_lProgressBarStart    (GLI_pc_g_stProgressUi.lBarXStart)
#define GLI_pc_g_lProgressBarEnd      (GLI_pc_g_stProgressUi.lBarXEnd)

#define GLI_AA_ProgressBarEnabled  GLI_pc_g_stProgressUi.ucProgressBarEnabled
#define GLI_AA_ProgressBarDivisor  GLI_pc_g_stProgressUi.ulProgressBarDivisor
#define GLI_AA_ProgressBarAccum    GLI_pc_g_stProgressUi.ulProgressBarAccum
#define GLI_AA_ProgressBarLastPos  GLI_pc_g_stProgressUi.ulProgressBarLastPos
#define GLI_AA_ProgressBarCurPos   GLI_pc_g_stProgressUi.ulProgressBarCurPos
#define GLI_AA_ProgressBarStart    GLI_pc_g_stProgressUi.lBarXStart
#define GLI_AA_ProgressBarEnd      GLI_pc_g_stProgressUi.lBarXEnd
