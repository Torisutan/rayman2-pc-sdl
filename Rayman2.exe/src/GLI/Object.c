#include "GLI/Camera.h"
#include "GLI/Object.h"
#include "GLI/GLI_globals.h"
#include "GLI/Material.h"
#include "GLI/mater_st.h"
#include "GLI/Proj.h"
#include "POS/POS_types.h"
#include "MTH/MTH3D.h"
#include "MTH/MTH_fopt.h"
#include "GLD/GLD_Pub.h"
#include "SCT/Sector.h"
#include "GLI/GLI_BigGlobals.h"
#include "GEO/GEO_types.h"
#include "GLI/light_st.h"
#ifdef RAYMAN2_WIDESCREEN
#include "GLI/GLI_ratio.h"
#endif /* RAYMAN2_WIDESCREEN */

struct GLD_tdstViewportAttributes
{
    unsigned long dwInitialHeight;
    unsigned long dwInitialWidth;
    unsigned long dwHeight;
    unsigned long dwWidth;
    unsigned long dwTopInPix;
    unsigned long dwBottomInPix;
    unsigned long dwLeftInPix;
    unsigned long dwRightInPix;
    unsigned long dwTopInPixForClip;
    unsigned long dwBottomInPixForClip;
    unsigned long dwLeftInPixForClip;
    unsigned long dwRightInPixForClip;
    unsigned long dwWidthInPercent;
    unsigned long dwHeightInPercent;
    unsigned long dwClipTopInPix;
    unsigned long dwClipBottomInPix;
    unsigned long dwClipLeftInPix;
    unsigned long dwClipRightInPix;
    unsigned long dwClipTopInPerMille;
    unsigned long dwClipBottomInPerMille;
    unsigned long dwClipLeftInPerMille;
    unsigned long dwClipRightInPerMille;
    int           iOffsetPosX;
    int           iOffsetPosY;
    char         *p_cVirtualScreen;
    long          lPitch;
    void         *p_vSpecificToXD;
    short         hDevice;
    short         hViewport;
};

void GEO_vTurnLookAt(GEO_tdstGeometricObject *p_stObj, void *hVpt, POS_tdstCompletePosition *p_stCurrentMatrix);
void __cdecl FUN_00428da0(int base, short *p, int idx, int arg4);
extern void *GLI_gs_aDEF_p_CurrentListOfLight[300];
/* 0x0041DAC0 */
ACP_tdxBool GEO_bIsLookAt(struct GEO_tdstGeometricObject_ *p_stObj);

void __cdecl GLI_vClearListOfLightInViewport(void *p_stVpt);
void __cdecl GLI_vAddLightToViewport(void *p_stVpt, void *p_stLight);
void __cdecl GLI_vSetGlobalAlpha(float xAlpha);
float __cdecl GLI_fn_xGetGlobalAlpha(void);
void __cdecl GEO_vGetMaterialOfIndexedTriangles(void *hObject, short hElement, void **p_hMaterial);
void __cdecl GLI_vDrawIndexedSprite(void *p_stVpt, unsigned long ulArg2, int *p_lSprite,
    struct GLI_tdstInternalGlobalValuesFor3dEngine_ *p_stGlobals);
void __cdecl GLI_pc_fn_vPackVertexColoursIdx(unsigned char *p_ucThis, short sCount, short *psIdx);
void __cdecl GLI_pc_fn_vPackVertexColoursAll(unsigned char *p_ucThis, short sCount);
void __cdecl GLI_pc_fn_vPackVertexColoursIdxRaw(unsigned char *p_ucThis, short sCount, short *psIdx);
void __cdecl GLI_pc_fn_vPackVertexColoursAllRaw(unsigned char *p_ucThis, short sCount);
void __cdecl GLI_pc_fn_vDrawSpriteQuadsOfElement(
    struct GLI_tdstInternalGlobalValuesFor3dEngine_ *p_stGlobals, short sIndex);
void __cdecl GLI_pc_fn_vSendTriangleElement(
    struct GLI_tdstInternalGlobalValuesFor3dEngine_ *p_stGlobals, short sIndex);

/* 0x0042A8C0 */
void __cdecl GLI_xSendObjectToViewport(
    struct GLD_tdstViewportAttributes *p_stVpt,
    void                              *p_stObj,
    void                              *p_stLight,
    long                               lDrawModeMask,
    long                               lHieFlags)
{
    struct GLI_tdstLight_ stAmbientLight;
    float         xSavedAlpha;

    stAmbientLight.lTypeOfLight = 4;
    stAmbientLight.b_lOnOff = 1;
    stAmbientLight.bValid = 1;
    stAmbientLight.stColor.xR = 0.0f;
    stAmbientLight.stColor.xG = 0.0f;
    stAmbientLight.stColor.xB = 0.02f;

    xSavedAlpha = GLI_fn_xGetGlobalAlpha();
    GLI_vSetGlobalAlpha(255.0f);
    GLI_vClearListOfLightInViewport(p_stVpt);
    GLI_vAddLightToViewport(p_stVpt, p_stLight);
    GLI_vAddLightToViewport(p_stVpt, &stAmbientLight);
    GLI_xSendObjectToViewportWithLights(p_stVpt, p_stObj, lDrawModeMask);
    GLI_vSetGlobalAlpha(xSavedAlpha);
    GLI_vClearListOfLightInViewport(p_stVpt);
}

void __cdecl GLI_xSendObjectElement(
    struct GLI_tdstInternalGlobalValuesFor3dEngine_ *p_stGlobals)
{

    struct GEO_tdstGeometricObject_ *p_stSub = p_stGlobals->p_stObj;

    short sIndex = 0;

    if (p_stSub->xNbElements <= sIndex) {
        return;
    }

    do {
        ACP_tdxIndex *p_aTypes = p_stSub->d_xListOfElementsTypes;

        int    iType    = p_aTypes[sIndex];

        if (iType == 1) {
            GLI_pc_fn_vSendTriangleElement(p_stGlobals, sIndex);
        } else if (iType == 3) {
            GLI_pc_fn_vDrawSpriteQuadsOfElement(p_stGlobals, sIndex);
        }

        p_stSub = p_stGlobals->p_stObj;
        sIndex++;
    } while (sIndex < p_stSub->xNbElements);
}

/* 0x0042A9E0 */
void __cdecl GLI_pc_fn_vDrawSpriteQuadsOfElement(
    struct GLI_tdstInternalGlobalValuesFor3dEngine_ *p_stGlobals,
    short                                          sIndex)
{
    GEO_tdstElementSprite *p_stElem;
    int            iEntry;
    unsigned long  ulOffset;

    p_stElem = (GEO_tdstElementSprite *)((void **)p_stGlobals->p_stObj->d_stListOfElements)[sIndex];
    iEntry = 0;
    if (p_stElem->xNbSprites > iEntry) {
        ulOffset = 0;
        do {
            GLI_vDrawIndexedSprite(p_stGlobals->p_stVpt,
                                      (unsigned long)p_stGlobals->p_stObj,
                                      (int *)((char *)p_stElem->d_stListOfSprites + ulOffset),
                                      p_stGlobals);
            iEntry++;
            ulOffset += sizeof(GEO_tdstIndexedSprite);
        } while (iEntry < p_stElem->xNbSprites);
    }
}

/* 0x0042AA30 */
void __cdecl GLI_pc_fn_vSendTriangleElement(
    struct GLI_tdstInternalGlobalValuesFor3dEngine_ *p_stGlobals,
    short                                          sIndex)
{
    struct GEO_tdstGeometricObject_ *p_stSub;
    GEO_tdstElementIndexedTriangles *p_stElem;

    p_stSub = p_stGlobals->p_stObj;
    GEO_vGetMaterialOfIndexedTriangles(p_stSub, sIndex,
        (void **)&p_stGlobals->hCurrentMaterial);
    GLI_vDoMaterialSelection(p_stGlobals);

    p_stSub  = p_stGlobals->p_stObj;
    p_stElem = (GEO_tdstElementIndexedTriangles *)((void **)p_stSub->d_stListOfElements)[sIndex];
    if (p_stGlobals->lCurrentDrawMask & 0x4000000UL) {
        if (p_stElem->d_stListOfIndexUsedByThisElement != 0) {
            GLI_pc_fn_vPackVertexColoursIdx((unsigned char *)p_stGlobals,
                                            p_stElem->xNumberOfIndexsUsed,
                                            p_stElem->d_stListOfIndexUsedByThisElement);
        } else {
            GLI_pc_fn_vPackVertexColoursAll((unsigned char *)p_stGlobals,
                                            p_stSub->xNbPoints);
        }
    } else {
        if (p_stElem->d_stListOfIndexUsedByThisElement != 0) {
            GLI_pc_fn_vPackVertexColoursIdxRaw((unsigned char *)p_stGlobals,
                                               p_stElem->xNumberOfIndexsUsed,
                                               p_stElem->d_stListOfIndexUsedByThisElement);
        } else {
            GLI_pc_fn_vPackVertexColoursAllRaw((unsigned char *)p_stGlobals,
                                               p_stSub->xNbPoints);
        }
    }

    (*(void (__cdecl **)(void *, void *))&GLI_DRV_g_xSendElementTIToClip_TRIANGLES)(
        ((void **)p_stGlobals->p_stObj->d_stListOfElements)[sIndex],
        p_stGlobals);
}

extern unsigned char GLI_M_g_bTakeCareOfAnimCoord_storage; /* 0x4a7274 */
extern unsigned char GLI_M_g_bOneShotAnim_storage;         /* 0x4a7278 */
extern unsigned char GLI_M_g_bAnimFinishedFlag_storage;    /* 0x4a7270 */
#define GLI_g_bTakeCareOfAnimCoord_ADDR  (&GLI_M_g_bTakeCareOfAnimCoord_storage)
#define GLI_g_bOneShotAnim_ADDR          (&GLI_M_g_bOneShotAnim_storage)
#define GLI_g_bAnimFinishedFlag_ADDR     (&GLI_M_g_bAnimFinishedFlag_storage)

extern float GLI_M_g_xBaseTimeForAnim_storage;   /* 0x503d60 */
#define GLI_g_xBaseTimeForAnim_ADDR      (&GLI_M_g_xBaseTimeForAnim_storage)

extern float AGO_g_xZValueForSprite;   /* 0x4a07d4 */
#define GLI_g_fZValueForSprite_ADDR  (&AGO_g_xZValueForSprite)

extern float GLI_g_xBrightness_storage;       /* 0x4a0488 */
static const float GLI_OBJ_K_xHalf = 0.5f;    /* 0x49c64c */
#define GLI_BRIGHT_IMM  OFFSET GLI_g_xBrightness_storage
#define GLI_HALF_IMM    OFFSET GLI_OBJ_K_xHalf

static const float GLI_gs_xSpriteNearClip = 0.0f;   /* 0x0049c614 */
static const float GLI_gs_xSpriteFarClip  = 4.0f;   /* 0x0049c61c */
#define GLI_g_xSpriteNearClip_ADDR       (&GLI_gs_xSpriteNearClip)
#define GLI_g_xSpriteFarClip_ADDR        (&GLI_gs_xSpriteFarClip)

#define GLI_GLOBALS_lHierachDrawMask_OFFSET    0x000c
#define GLI_GLOBALS_lCurrentDrawMask_OFFSET    0x0010
#define GLI_GLOBALS_hCurrentMaterial_OFFSET    0x2d3b8
#define GLI_GLOBALS_xTextureDontAcceptFog_OFFSET 0x2d3f8
#define GLI_GLOBALS_bForceNoLight_OFFSET       0x2d3c0

#define GLI_SPRITE_MATERIAL_MASK   0xFFFFFBF7UL

/* 0x0042A410 */
typedef void (__cdecl *GLI_pfn_vDoMaterialSelection_t)(void *p_stGlobals);

typedef void (__cdecl *GLI_pfn_xPerspectiveProject_t)(
    void *p_stCam, long lCount, float *p_stSource, float *p_stDest);

unsigned char __cdecl GLI_vDrawWorldSprite(
    MTH3D_tdstVector           *pPosition,
    void                       *hMaterial,
    MTH_tdxReal                 xScaleX,
    MTH_tdxReal                 xScaleY,
    struct GLD_tdstViewportAttributes *p_stViewport,
    unsigned long               ulAnimTime)
{
    struct GLI_tdstInternalGlobalValuesFor3dEngine_ *p_stGlobals;

    void *p_vSpecificToXD = p_stViewport->p_vSpecificToXD;
    void *p_stCam         = *(void **)p_vSpecificToXD;

    MTH3D_tdstVector stSrc;
    float aTransformed[4];
    float aProjCenter[4];
    float aSpriteCorners[12];
    float aProjCorners[12];

    struct { float xX, xY, xZ, xW; } aQuad[4];
    MTH_tdxReal xAnimTime;

    *GLI_g_bTakeCareOfAnimCoord_ADDR = 1;
    *GLI_g_bOneShotAnim_ADDR         = 1;
    *GLI_g_bAnimFinishedFlag_ADDR    = 0;

    xAnimTime = MTH_M_xLongToReal(ulAnimTime);
    *GLI_g_xBaseTimeForAnim_ADDR = xAnimTime;

    stSrc.xX = pPosition->xX;
    stSrc.xY = pPosition->xY;
    stSrc.xZ = pPosition->xZ;
    POS_fn_vMulMatrixVertex((MTH3D_tdstVector *)aTransformed,
                            &((GLI_tdstCamera *)p_stCam)->stMatrix,
                            &stSrc);

    GLI_xSerialProjection(p_stCam, 1, aTransformed, aProjCenter);

    aSpriteCorners[0]  = xScaleX;
    aSpriteCorners[1]  = 0.0f;
    aSpriteCorners[2]  = aTransformed[2];
    aSpriteCorners[3]  = 0.0f;
    aSpriteCorners[4]  = 0.0f;
    aSpriteCorners[5]  = -xScaleY;
    aSpriteCorners[6]  = aTransformed[2];
    aSpriteCorners[7]  = 0.0f;
    aSpriteCorners[8]  = 0.0f;
    aSpriteCorners[9]  = 0.0f;

    aSpriteCorners[10] = aTransformed[2];
    aSpriteCorners[11] = 0.0f;

    {
        GLI_pfn_xPerspectiveProject_t fn_xPerspProject =
            (GLI_pfn_xPerspectiveProject_t)GLI_xSerialProjectionNoTrans;   /* 0x0042C7D0 */
        fn_xPerspProject(p_stCam, 3, aSpriteCorners, aProjCorners);
    }

    aQuad[0].xX = ((aProjCenter[0] - aProjCorners[0]) - aProjCorners[4]) + aProjCorners[8];
    aQuad[0].xY = ((aProjCenter[1] - aProjCorners[1]) - aProjCorners[5]) + aProjCorners[9];
    aQuad[1].xX = ((aProjCorners[0] + aProjCenter[0]) - aProjCorners[4]) + aProjCorners[8];
    aQuad[1].xY = ((aProjCorners[1] + aProjCenter[1]) - aProjCorners[5]) + aProjCorners[9];
    aQuad[2].xX = aProjCorners[0] + aProjCorners[4] + aProjCorners[8] + aProjCenter[0];
    aQuad[2].xY = aProjCorners[1] + aProjCorners[5] + aProjCorners[9] + aProjCenter[1];
    aQuad[3].xX = aProjCorners[4] + aProjCorners[8] + (aProjCenter[0] - aProjCorners[0]);
    aQuad[3].xY = aProjCorners[5] + aProjCorners[9] + (aProjCenter[1] - aProjCorners[1]);

    *GLI_g_bTakeCareOfAnimCoord_ADDR = 1;
    *GLI_g_bOneShotAnim_ADDR         = 1;
    *GLI_g_bAnimFinishedFlag_ADDR    = 0;
    *GLI_g_xBaseTimeForAnim_ADDR = xAnimTime;

    p_stGlobals = GLI_BIG_GLOBALS;
    p_stGlobals->hCurrentMaterial = hMaterial;

    {
        unsigned long ulSavedDrawMode;
        GLI_pfn_vDoMaterialSelection_t fn_vDoMaterialSelection =
            (GLI_pfn_vDoMaterialSelection_t)&GLI_vDoMaterialSelection;

        ulSavedDrawMode = (unsigned long)p_stGlobals->lHierachDrawMask;
        p_stGlobals->lHierachDrawMask = GLI_SPRITE_MATERIAL_MASK;

        p_stGlobals = GLI_BIG_GLOBALS;
        fn_vDoMaterialSelection(p_stGlobals);

        p_stGlobals = GLI_BIG_GLOBALS;
        p_stGlobals->xTextureDontAcceptFog = 1;
        p_stGlobals = GLI_BIG_GLOBALS;
        p_stGlobals->xWaterPlaneDistance = 0;

        if (*GLI_g_xSpriteNearClip_ADDR < aProjCenter[2] &&
            aProjCenter[2] <= *GLI_g_xSpriteFarClip_ADDR)
        {
            p_stGlobals = GLI_BIG_GLOBALS;
            (*GLI_DRV_vSendSpriteToClip_)(
                (struct GLI_tdstAligned2DVector_ *)&aQuad[0],
                aProjCenter[2],
                p_stGlobals);
        }

        p_stGlobals = GLI_BIG_GLOBALS;
        p_stGlobals->lHierachDrawMask = ulSavedDrawMode;
    }

    *GLI_g_bTakeCareOfAnimCoord_ADDR = 0;
    *GLI_g_bOneShotAnim_ADDR         = 0;

    /* 0x4a7270 */
    return *GLI_g_bAnimFinishedFlag_ADDR;
}

static const float GLI_gs_xOnePercent = 0.01f;   /* 0x0049c618 */
#define GLI_g_xOnePercent_ADDR  (&GLI_gs_xOnePercent)

unsigned char g_GLI_bForceAAAColor;   /* 0x004A72AC */
#define GLI_g_bDrawingText_ADDR ((const unsigned char *)&g_GLI_bForceAAAColor)

unsigned char GLI_g_aForcedAAAColor_ADDR[3];   /* 0x00503748 */

#define GLI_GLOBALS_ulColorInitForSprite_OFFSET  0x2d3d4

void __cdecl GLI_vDraw2DSpriteWithPercent(
    struct GLD_tdstViewportAttributes *p_stViewport,
    MTH_tdxReal XMin, MTH_tdxReal YMin,
    MTH_tdxReal XMax, MTH_tdxReal YMax,
    void *hMaterial)
{
    char *p_stGlobals = (char *)GLI_BIG_GLOBALS;

    unsigned int ulRightInPix  = p_stViewport->dwWidth;
    unsigned int ulBottomInPix = p_stViewport->dwHeight;

    float fWidth, fHeight;
    float fXMinScaled, fXMaxScaled, fYMinScaled, fYMaxScaled;
    fWidth  = MTH_M_xLongToReal(ulRightInPix);
    fXMinScaled = XMin * *GLI_g_xOnePercent_ADDR * fWidth;
    fXMaxScaled = XMax * *GLI_g_xOnePercent_ADDR * fWidth;
    fHeight = MTH_M_xLongToReal(ulBottomInPix);
    fYMinScaled = YMin * *GLI_g_xOnePercent_ADDR * fHeight;
    fYMaxScaled = YMax * *GLI_g_xOnePercent_ADDR * fHeight;
#ifdef RAYMAN2_WIDESCREEN
    /* in the 4:3 band, except full-width sprites (backgrounds, fades) */
    if ( GLI_g_xSafeAreaRatio < MTH_C_ONE
      && !( XMin <= MTH_C_ZERO && XMax >= MTH_M_xConstFloatToReal(100.0f) ) )
    {
        fXMinScaled = GLI_fn_xToSafeArea(fXMinScaled, fWidth);
        fXMaxScaled = GLI_fn_xToSafeArea(fXMaxScaled, fWidth);
    }
#endif /* RAYMAN2_WIDESCREEN */

    {

    struct { float xX, xY, xOoZ, _pad; } aQuad[4];
    aQuad[0].xX = fXMaxScaled;  aQuad[0].xY = fYMinScaled;  aQuad[0].xOoZ = 1.0f;
    aQuad[1].xX = fXMinScaled;  aQuad[1].xY = fYMinScaled;  aQuad[1].xOoZ = 1.0f;
    aQuad[2].xX = fXMinScaled;  aQuad[2].xY = fYMaxScaled;  aQuad[2].xOoZ = 1.0f;
    aQuad[3].xX = fXMaxScaled;  aQuad[3].xY = fYMaxScaled;  aQuad[3].xOoZ = 1.0f;

    *(void **)(p_stGlobals + GLI_GLOBALS_hCurrentMaterial_OFFSET) = hMaterial;
    p_stGlobals = (char *)GLI_BIG_GLOBALS;

    {
        void *hMat = *(void **)(p_stGlobals + GLI_GLOBALS_hCurrentMaterial_OFFSET);
        *(unsigned long *)hMat = GLI_SPRITE_MATERIAL_MASK;
    }
    p_stGlobals = (char *)GLI_BIG_GLOBALS;
    *(unsigned long *)(p_stGlobals + GLI_GLOBALS_lHierachDrawMask_OFFSET) = GLI_SPRITE_MATERIAL_MASK;
    p_stGlobals = (char *)GLI_BIG_GLOBALS;
    *(unsigned long *)(p_stGlobals + GLI_GLOBALS_lCurrentDrawMask_OFFSET) = GLI_SPRITE_MATERIAL_MASK;

    {
        GLI_pfn_vDoMaterialSelection_t fn_vDoMaterialSelection =
            (GLI_pfn_vDoMaterialSelection_t)&GLI_vDoMaterialSelection;
        p_stGlobals = (char *)GLI_BIG_GLOBALS;
        fn_vDoMaterialSelection(p_stGlobals);
    }
    p_stGlobals = (char *)GLI_BIG_GLOBALS;
    *(unsigned long *)(p_stGlobals + GLI_GLOBALS_xTextureDontAcceptFog_OFFSET) = 1;

    if (*GLI_g_bDrawingText_ADDR != 0) {

        unsigned long ulAlphaMask;
        unsigned long ulFullColor;
        unsigned long ulForcedColor;
        unsigned long aColors[4];

        p_stGlobals = (char *)GLI_BIG_GLOBALS;
        ulAlphaMask = *(unsigned long *)(p_stGlobals + GLI_GLOBALS_ulColorInitForSprite_OFFSET)
                    & 0xff000000UL;

        ulFullColor = ulAlphaMask | 0x00ffffffUL;

        /* 0x4297ae */
        {
            unsigned char r = GLI_g_aForcedAAAColor_ADDR[0];
            unsigned char g = GLI_g_aForcedAAAColor_ADDR[1];
            unsigned char b = GLI_g_aForcedAAAColor_ADDR[2];
            ulForcedColor = ulAlphaMask
                          | ((unsigned long)r << 16)
                          | ((unsigned long)g <<  8)
                          | (unsigned long)b;
        }

        aColors[0] = ulFullColor;
        aColors[1] = ulFullColor;
        aColors[2] = ulForcedColor;
        aColors[3] = ulForcedColor;

        p_stGlobals = (char *)GLI_BIG_GLOBALS;
        (*GLI_DRV_vSendSpriteToClipWithColors_)(
            (struct GLI_tdstAligned2DVector_ *)&aQuad[0],
            aColors,
            *(unsigned long *)GLI_g_fZValueForSprite_ADDR,
            (struct GLI_tdstInternalGlobalValuesFor3dEngine_ *)p_stGlobals);
    } else {

        p_stGlobals = (char *)GLI_BIG_GLOBALS;
        (*GLI_DRV_vSendSpriteToClip_)(
            (struct GLI_tdstAligned2DVector_ *)&aQuad[0],
            *(MTH_tdxReal *)GLI_g_fZValueForSprite_ADDR,
            (struct GLI_tdstInternalGlobalValuesFor3dEngine_ *)p_stGlobals);
    }

    }
}

/* 0x00429F70 */
static const double GLI_pc_dInvPi = 0.3183098861837907;

unsigned char __cdecl GLI_pc_fn_bSubmitAngleQuad(void *hCamSector, void *hCamera, void *p_stVpt)
{
    struct GLD_tdstViewportAttributes_ *p_stVptAttr;
    SECT_tdxHandleOfSectorObject hSectorObject;
    MTH3D_tdstVector a3_stVector[3];
    float          aVert[16];
    float          aUV[8];
    float          dfAlpha;
    unsigned long  ulSavedMode;

    p_stVptAttr = (struct GLD_tdstViewportAttributes_ *)p_stVpt;

    hSectorObject = (SECT_tdxHandleOfSectorObject)HIE_fn_hGetSuperObjectObject((HIE_tdstSuperObject *)hCamSector);
    if (hSectorObject->pSkyVisualMaterial == NULL) {
        return 0;
    }

    POS_fn_vGetTransformMatrix(
        HIE_fn_hGetSuperObjectGlobalMatrix((HIE_tdstSuperObject *)hCamera),
        &a3_stVector[0], &a3_stVector[1], &a3_stVector[2]);

    if (MTH3D_M_xGetZofVector(&a3_stVector[0]) > 0.9f) {
        dfAlpha = 0.0f;
    } else {
        dfAlpha = (float)-(atan2(MTH3D_M_xGetYofVector(&a3_stVector[0]),
                                 MTH3D_M_xGetXofVector(&a3_stVector[0])) * GLI_pc_dInvPi);
    }

    aVert[0]  = (float)p_stVptAttr->dwClipLeftInPix;
    aVert[1]  = (float)p_stVptAttr->dwClipTopInPix;
    aVert[4]  = (float)p_stVptAttr->dwClipRightInPix;
    aVert[9]  = (float)p_stVptAttr->dwClipBottomInPix;
    aVert[5]  = aVert[1];
    aVert[8]  = aVert[4];
    aVert[12] = aVert[0];
    aVert[13] = aVert[9];

    aUV[0] = dfAlpha;
    aUV[1] = MTH3D_M_xGetZofVector(&a3_stVector[2]);
    aUV[2] = dfAlpha - (-1.0f);
    aUV[3] = MTH3D_M_xGetZofVector(&a3_stVector[2]);
    aUV[4] = dfAlpha - (-1.0f);
    aUV[5] = MTH3D_M_xGetZofVector(&a3_stVector[2]) - (-1.0f);
    aUV[6] = dfAlpha;
    aUV[7] = aUV[5];

    GLI_vSetGlobalAlpha(255.0f);

    GLI_BIG_GLOBALS->hCurrentMaterial = hSectorObject->pSkyVisualMaterial;
    *(unsigned long *)GLI_BIG_GLOBALS->hCurrentMaterial = 0xfffffbffUL;
    GLI_BIG_GLOBALS->lHierachDrawMask = 0xfffffbffUL;
    GLI_BIG_GLOBALS->lCurrentDrawMask = 0xfffffbffUL;
    ulSavedMode = (unsigned long)GLI_BIG_GLOBALS->xFogIsOn;
    GLI_BIG_GLOBALS->xFogIsOn = 2;
    GLI_BIG_GLOBALS->ulSpecularColorForSprite =
        (unsigned long)hSectorObject->FogIntensity << 0x18;

    GLI_vDoMaterialSelection(GLI_BIG_GLOBALS);

    GLI_BIG_GLOBALS->ulColorInitForSprite =
        (GLI_BIG_GLOBALS->ulColorInitForSprite & 0xffffffUL)
        | ((unsigned long)hSectorObject->FogIntensity << 0x18);

    ((void (__cdecl *)(float, float, float, float, void *))GLI_DRV_g_vSetClipWindow)(
        (float)p_stVptAttr->dwClipLeftInPix,
        (float)p_stVptAttr->dwClipRightInPix,
        (float)p_stVptAttr->dwClipTopInPix,
        (float)p_stVptAttr->dwClipBottomInPix,
        GLI_BIG_GLOBALS);

    (*GLI_DRV_vSendSpriteToClipWithUV_)(
        (struct GLI_tdstAligned2DVector_ *)aVert,
        (MTH_tdxReal *)aUV,
        0.00006103515625f,
        (struct GLI_tdstInternalGlobalValuesFor3dEngine_ *)GLI_BIG_GLOBALS);

    GLI_BIG_GLOBALS->xFogIsOn = ulSavedMode;
    return 1;
}

#define GLI_PRJ_K_xNegOne (-1.0f)

#define GLI_PRJ_K_xOne    GLI_K_xOne   /* 0x0049c63c */

void __cdecl GLI_pc_vProjectVerticesToScreen(struct GLI_tdstInternalGlobalValuesFor3dEngine_ *p_stGlobals)
{
    float *p_xSrc;
    float *p_xSrcEnd;
    float *p_xDest;

    p_xSrc    = (float *)p_stGlobals->p_stObj->d_stListOfPointsNormals;
    p_xSrcEnd = p_xSrc + p_stGlobals->p_stObj->xNbPoints * 3;
    p_xDest   = &p_stGlobals->GLI_aDEF_stEnvUV[0].xU;
    if (p_xSrc < p_xSrcEnd) {
        do {
            p_xSrc += 3;
            p_xDest += 2;
            p_xDest[-2] = ((p_stGlobals->gs_st_CurrentMatrixMultiplyedWithCameraMatrix.stRotationMatrix.stCol_2.xX * p_xSrc[-1] +
                            p_stGlobals->gs_st_CurrentMatrixMultiplyedWithCameraMatrix.stRotationMatrix.stCol_1.xX * p_xSrc[-2] +
                            p_xSrc[-3] * p_stGlobals->gs_st_CurrentMatrixMultiplyedWithCameraMatrix.stRotationMatrix.stCol_0.xX) - GLI_PRJ_K_xNegOne) * GLI_PRJ_K_xHalf;
            p_xDest[-1] = (GLI_PRJ_K_xOne -
                           (p_xSrc[-3] * p_stGlobals->gs_st_CurrentMatrixMultiplyedWithCameraMatrix.stRotationMatrix.stCol_0.xY +
                            p_stGlobals->gs_st_CurrentMatrixMultiplyedWithCameraMatrix.stRotationMatrix.stCol_1.xY * p_xSrc[-2] +
                            p_stGlobals->gs_st_CurrentMatrixMultiplyedWithCameraMatrix.stRotationMatrix.stCol_2.xY * p_xSrc[-1])) * GLI_PRJ_K_xHalf;
        } while (p_xSrc < p_xSrcEnd);
    }
}

static const unsigned long GLI_OBJ_K_dOne_bits[2]   = { 0x00000000UL, 0x3FF00000UL }; /* 0x0049C650 */
static const unsigned long GLI_OBJ_K_x255_bits      = 0x437F0000UL;   /* 0x0049C644 */
static const unsigned long GLI_OBJ_K_xM49152_bits   = 0xC7400000UL;   /* 0x0049C658 */
static const unsigned long GLI_OBJ_K_xMinus2_bits   = 0xC0000000UL;   /* 0x0049C660 */
static const unsigned long GLI_OBJ_K_xMinusEps_bits = 0xB8D1B717UL;   /* 0x0049C664 */
static const unsigned long GLI_OBJ_K_xM0_9_bits     = 0xBF666666UL;   /* 0x0049C668 */
extern long __cdecl _ftol(void);

void __cdecl GLI_pc_fn_vComputeObjectClipFlags();             /* 0x0042A720 */
void __cdecl GLI_pc_fn_vTransformObjectVerticesSinusClipped(); /* 0x0042CCF0 */
void __cdecl GLI_pc_fn_vTransformObjectVerticesSinus();       /* 0x0042D020 */
void __cdecl GLI_pc_fn_vTransformObjectVerticesClipped();     /* 0x0042CB30 */
void __cdecl GLI_pc_fn_vTransformObjectVertices();            /* 0x0042CEC0 */
void __cdecl GLI_pc_fn_vSetSinusEffectVertexColors();         /* 0x00423D00 */
void __cdecl GLI_vSendObjectToLights();                       /* 0x00428CB0 */
extern unsigned long GLI_g_ul4a72b0_storage;                   /* 0x004A72B0 */
extern int g_lActiveLightCount;                                /* 0x00503D4C */
void __cdecl GLI_vDoMaterialSelection(
    struct GLI_tdstInternalGlobalValuesFor3dEngine_ *p_stGlobals)
{
    struct GLI_tdstTexture    *p_stTexture;
    unsigned long              ulColor;
    unsigned long              ulFlatMode;
    unsigned long              ulBits;
    float                      xVar11;
    float                      xTmp;
    long                       lR;
    long                       lG;
    long                       lB;

    if (((p_stGlobals->lHierachDrawMask & 0x100) == 0) || (p_stGlobals->hCurrentMaterial == 0))
    {
        p_stGlobals->hCurrentMaterial = GLI_pc_g_hDefaultMaterial;
    }
    p_stGlobals->lCurrentDrawMask = p_stGlobals->lHierachDrawMask & (long)p_stGlobals->hCurrentMaterial->ulMaterialType;
    GLI_pc_vRefreshOneCyclingTexture(p_stGlobals->hCurrentMaterial);
    p_stGlobals->p_stCurrentTexture = p_stGlobals->hCurrentMaterial->p_stTexture;
    (*(void (__cdecl **)(void *))GLI_DRV_vDoOpaqueTextureSelection_PTR)(p_stGlobals);
    if ((p_stGlobals->lCurrentDrawMask & 0x400000) == 0)
    {
        GLI_pc_vProjectVerticesToScreen(p_stGlobals);
    }

    ulFlatMode = p_stGlobals->lCurrentDrawMask & 8;
    if (ulFlatMode != 0)
    {
        xVar11 = GLI_K_xOne;
        ulColor = 0xffffffff;
    }
    else if (p_stGlobals->xGlobalAlpha == 1.0)
    {
        xVar11 = GLI_PRJ_K_xHalf;
        ulColor = 0xffffffff;
    }
    else
    {
        xTmp = p_stGlobals->xGlobalAlpha - (-49152.0f);
        ulBits = *(unsigned long *)&xTmp;
        lR = (long)(p_stGlobals->hCurrentMaterial->stAmbient.xR * 255.0f);
        lG = (long)(p_stGlobals->hCurrentMaterial->stAmbient.xG * 255.0f);
        lB = (long)(p_stGlobals->hCurrentMaterial->stAmbient.xB * 255.0f);
        ulColor = (((ulBits << 8) | ((unsigned long)lR & 0xff)) << 8 | ((unsigned long)lG & 0xff)) << 8
                | ((unsigned long)lB & 0xff);
        xVar11 = p_stGlobals->xGlobalAlpha * GLI_PRJ_K_xHalf;
    }

    p_stTexture = p_stGlobals->p_stCurrentTexture;
    if (p_stTexture != 0 && (p_stTexture->ulTextureCaps & 0x200) != 0)
    {
        lG = (long)(p_stGlobals->hCurrentMaterial->stAmbient.xG * 255.0f);
        lR = (long)(p_stGlobals->hCurrentMaterial->stAmbient.xR * 255.0f);
        lB = (long)(p_stGlobals->hCurrentMaterial->stAmbient.xB * 255.0f);
        ulColor = ((((unsigned long)lG & 0xff) | (((unsigned long)lR & 0xff) << 8)) << 8)
                | (ulColor & 0xff000000)
                | ((unsigned long)lB & 0xff);
    }

    p_stGlobals->ulColorInitForSprite = ulColor;
    p_stGlobals->GLI_stColorsAdd = p_stGlobals->hCurrentMaterial->stAmbient;
    p_stGlobals->GLI_stColorsMul = p_stGlobals->hCurrentMaterial->stDiffuse;
    p_stGlobals->GLI_stColorsAdd.xR = p_stGlobals->GLI_aDEF_stColorsRLIA.xR * p_stGlobals->GLI_stColorsMul.xR
                                    + p_stGlobals->GLI_stColorsAdd.xR;
    p_stGlobals->GLI_stColorsAdd.xG = p_stGlobals->GLI_aDEF_stColorsRLIA.xG * p_stGlobals->GLI_stColorsMul.xG
                                    + p_stGlobals->GLI_stColorsAdd.xG;
    p_stGlobals->GLI_stColorsAdd.xB = p_stGlobals->GLI_aDEF_stColorsRLIA.xB * p_stGlobals->GLI_stColorsMul.xB
                                    + p_stGlobals->GLI_stColorsAdd.xB;
    p_stGlobals->GLI_stColorsAdd.xA = p_stGlobals->GLI_aDEF_stColorsRLIA.xA * p_stGlobals->GLI_stColorsMul.xA
                                    + p_stGlobals->GLI_stColorsAdd.xA;

    if (ulFlatMode != 0)
    {
        p_stGlobals->GLI_stColorsMul.xA = 0.0f;
        p_stGlobals->GLI_stColorsAdd.xA = 1.0f;
    }
    else if ((p_stGlobals->lCurrentDrawMask & 0x4000000) != 0)
    {
        p_stGlobals->GLI_stColorsAdd.xA = xVar11;
        p_stGlobals->GLI_stColorsMul.xA = 0.0f;
    }

    p_stTexture = p_stGlobals->p_stCurrentTexture;
    if (p_stTexture != (struct GLI_tdstTexture *)1 && p_stTexture != 0
        && (p_stTexture->ulTextureCaps & 0x20) != 0
        && (p_stGlobals->lCurrentDrawMask & 0x4000000) != 0)
    {
        p_stGlobals->GLI_stColorsAdd.xR = xVar11;
        p_stGlobals->GLI_stColorsMul.xR = 0.0f;
        p_stGlobals->GLI_stColorsAdd.xG = xVar11;
        p_stGlobals->GLI_stColorsMul.xG = 0.0f;
        p_stGlobals->GLI_stColorsAdd.xB = xVar11;
        p_stGlobals->GLI_stColorsMul.xB = 0.0f;
        p_stGlobals->GLI_stColorsAdd.xA = xVar11;
        p_stGlobals->GLI_stColorsMul.xA = 0.0f;
    }

    xTmp = GLI_g_xBrightness_storage - GLI_PRJ_K_xHalf;
    p_stGlobals->GLI_stColorsAdd.xR = xTmp + p_stGlobals->GLI_stColorsAdd.xR;
    p_stGlobals->GLI_stColorsAdd.xG = xTmp + p_stGlobals->GLI_stColorsAdd.xG;
    p_stGlobals->GLI_stColorsAdd.xB = xTmp + p_stGlobals->GLI_stColorsAdd.xB;
    p_stGlobals->GLI_stColorsMul.xR = xTmp + p_stGlobals->GLI_stColorsMul.xR;
    p_stGlobals->GLI_stColorsMul.xG = xTmp + p_stGlobals->GLI_stColorsMul.xG;
    p_stGlobals->GLI_stColorsMul.xB = xTmp + p_stGlobals->GLI_stColorsMul.xB;
}

extern void *g_p_stCurrentInCamCoordsMatrix;   /* 0x00514D64 */
/* 0x0042AAE0 */
void __cdecl GLI_xSendObjectToViewportWithLights(
    struct GLD_tdstViewportAttributes *p_stVpt,
    void                              *p_stObj,
    long                               lDrawModeMask)
{
    struct GEO_tdstGeometricObject_ *p_stGeo;
    struct GLI_tdstCamera_          *p_stCam;
    POS_tdstCompletePosition         stSavedMatrix;
    unsigned long                    ulSavedRadius;
    unsigned long                    ulTexOverride;
    long                             lMask;
    long                             lCount;
    long                             i;

    p_stGeo = (struct GEO_tdstGeometricObject_ *)p_stObj;
    if ((short)p_stGeo->xNbPoints > 2500)
    {
        return;
    }

    lMask = lDrawModeMask | 0x100000;
    GLI_BIG_GLOBALS->p_stVpt = (struct GLD_tdstViewportAttributes_ *)p_stVpt;
    GLI_BIG_GLOBALS->lHierachDrawMask = lMask;
    GLI_BIG_GLOBALS->lClippingModeMask = 0;
    GLI_BIG_GLOBALS->p_stObj = p_stGeo;
    GLI_BIG_GLOBALS->gs_st_CurrentMatrix = (POS_tdstCompletePosition *)GLI_g_p_stCurrentMatrix;
    p_stCam = *(struct GLI_tdstCamera_ **)p_stVpt->p_vSpecificToXD;
    GLI_BIG_GLOBALS->gs_st_CameraMatrix = &p_stCam->stMatrix;
    GLI_BIG_GLOBALS->p_stCurrentCamera = p_stCam;
    (*(void (__cdecl **)(unsigned long, void *))GLI_DRV_vSetZClip_PTR)(
        *(unsigned long *)&p_stCam->xNear, GLI_BIG_GLOBALS);
    (*(void (__cdecl **)(float, float, float, float, void *))GLI_DRV_vSetClipWindow_PTR)(
        (float)(__int64)p_stVpt->dwClipLeftInPix,
        (float)(__int64)p_stVpt->dwClipRightInPix,
        (float)(__int64)p_stVpt->dwClipTopInPix,
        (float)(__int64)p_stVpt->dwClipBottomInPix,
        GLI_BIG_GLOBALS);

    if (GEO_bIsLookAt(p_stGeo))
    {
        stSavedMatrix = *(POS_tdstCompletePosition *)GLI_g_p_stCurrentMatrix;
        GEO_vTurnLookAt(p_stGeo, p_stVpt, (POS_tdstCompletePosition *)GLI_g_p_stCurrentMatrix);
        POS_fn_vMulMatrixMatrix(&GLI_BIG_GLOBALS->gs_st_CurrentMatrixMultiplyedWithCameraMatrix,
                                &p_stCam->stMatrix,
                                GLI_BIG_GLOBALS->gs_st_CurrentMatrix);
    }
    else
    {
        GLI_BIG_GLOBALS->gs_st_CurrentMatrixMultiplyedWithCameraMatrix =
            *(POS_tdstCompletePosition *)g_p_stCurrentInCamCoordsMatrix;
    }

    if ((GLI_pc_g_cSinusEffectState == 1 && (lMask & 0x80000000) == 0)
        || (GLI_pc_g_cSinusEffectState == 2 && (lMask & 0x80000000) != 0)
        || GLI_pc_g_cSinusEffectState == 3)
    {
        ulSavedRadius = *(unsigned long *)&p_stGeo->xBoudingSphereRadius;
        p_stGeo->xBoudingSphereRadius = p_stGeo->xBoudingSphereRadius
            - (((GLI_pc_stSinEffectAmp.xX + GLI_pc_stSinEffectAmp.xY) + GLI_pc_stSinEffectAmp.xZ)
               * (*(const float *)&GLI_OBJ_K_xMinus2_bits));
        GLI_pc_fn_vComputeObjectClipFlags(GLI_BIG_GLOBALS);
        *(unsigned long *)&p_stGeo->xBoudingSphereRadius = ulSavedRadius;
        if ((GLI_BIG_GLOBALS->lClippingModeMask & 0x100) != 0)
        {
            GLI_pc_fn_vTransformObjectVerticesSinusClipped(GLI_BIG_GLOBALS);
        }
        else
        {
            GLI_pc_fn_vTransformObjectVerticesSinus(GLI_BIG_GLOBALS);
        }
    }
    else
    {
        GLI_pc_fn_vComputeObjectClipFlags(GLI_BIG_GLOBALS);
        if ((GLI_BIG_GLOBALS->lClippingModeMask & 0x100) != 0)
        {
            GLI_pc_fn_vTransformObjectVerticesClipped(GLI_BIG_GLOBALS);
        }
        else
        {
            GLI_pc_fn_vTransformObjectVertices(GLI_BIG_GLOBALS);
        }
    }

    ulTexOverride = GLI_g_ul4a72b0_storage;
    if (ulTexOverride != 0 && (lMask & 0x1000) != 0)
    {
        lCount = (short)p_stGeo->xNbPoints;
        FUN_00428da0((int)GLI_BIG_GLOBALS, (short *)ulTexOverride, 0, lCount);
        if ((lMask & 0x20000000) == 0 || GLI_pc_g_cSinusEffectState == 3)
        {
            GLI_pc_fn_vSetSinusEffectVertexColors(GLI_BIG_GLOBALS, 0, lCount);
        }
        GLI_g_ul4a72b0_storage = 0;
    }
    else
    {
        lCount = (short)p_stGeo->xNbPoints;
        for (i = 0; i < lCount; i++)
        {
            GLI_BIG_GLOBALS->GLI_aDEF_stColorsRLID[i].xR = 0.0f;
            GLI_BIG_GLOBALS->GLI_aDEF_stColorsRLID[i].xG = 0.0f;
            GLI_BIG_GLOBALS->GLI_aDEF_stColorsRLID[i].xB = 0.0f;
            GLI_BIG_GLOBALS->GLI_aDEF_stColorsRLID[i].xA = 0.0f;
        }
    }

    GLI_vSendObjectToLights(GLI_gs_aDEF_p_CurrentListOfLight,
                            p_stGeo,
                            &GLI_BIG_GLOBALS->GLI_aDEF_stColorsRLID,
                            &GLI_BIG_GLOBALS->GLI_aDEF_stColorsRLIS,
                            &GLI_BIG_GLOBALS->GLI_aDEF_stColorsRLIA,
                            g_lActiveLightCount,
                            GLI_BIG_GLOBALS);
    (*(void (__cdecl **)(void *))GLI_DRV_vComputeFogEffect_PTR)(GLI_BIG_GLOBALS);

    if ((GLI_BIG_GLOBALS->lHierachDrawMask & 0x40000) == 0)
    {
        lCount = (short)p_stGeo->xNbPoints;
        for (i = 0; i < lCount; i++)
        {
            GLI_BIG_GLOBALS->GLI_ScreenPoint[i].xOoZ = GLI_BIG_GLOBALS->GLI_ScreenPoint[i].xOoZ
                                                      - (*(const float *)&GLI_OBJ_K_xMinusEps_bits);
        }
    }

    if (GLI_BIG_GLOBALS->lDisplayMenuIsDetected != 0)
    {
        lCount = (short)p_stGeo->xNbPoints;
        for (i = 0; i < lCount; i++)
        {
            GLI_BIG_GLOBALS->GLI_ScreenPoint[i].xOoZ = GLI_BIG_GLOBALS->GLI_ScreenPoint[i].xOoZ
                                                      - (*(const float *)&GLI_OBJ_K_xM0_9_bits);
        }
    }

    GLI_xSendObjectElement(GLI_BIG_GLOBALS);
    if (GEO_bIsLookAt(p_stGeo))
    {
        *(POS_tdstCompletePosition *)GLI_g_p_stCurrentMatrix = stSavedMatrix;
    }
}

extern float GLI_RD_g_xMagic7244_storage;
extern long  GLI_RD_g_l06f8_storage;

void GEO_vGetPointOfObject(void *hObject, float *p_stPoint, short xIndexOfPoint);
void GLI_pc_vTransformPointsByObject(long lCount, float *p_xSrc, float *p_xDst, void *p_stObj);
void __cdecl GLI_pc_fn_vOffsetPointBySinusEffect(float *p_xPoint);

unsigned short __cdecl GLI_pc_fn_usClipQuadToScreenRect(void *p_stObj,
                                                        unsigned long *p_stMatA,
                                                        unsigned long *p_stMatB,
                                                        GLI_tdstCamera *p_stCam,
                                                        void *p_stRect,
                                                        unsigned long ulFlags)
{
    float          afPtCam[12];
    float          afPtXf[16];
    float          afPtClip[20];
    float          afPtScr[20];
    unsigned long  aulMat[22];
    unsigned char  aucFlag[4];
    float         *pfVar8;
    float          fZMin;
    float          fZClip;
    float          fThr;
    float          fMinX;
    float          fMaxX;
    float          fMinY;
    float          fMaxY;
    float          xMinXB;
    float          xMaxXB;
    float          xMinYB;
    float          xMaxYB;
    float          xSel;
    long           lSel;
    long           lSelIdx;
    short          sIdx;
    short          sPtCount;
    int            iVar9;
    int            iVar12;
    int            iNext;
    int            iPrev;
    int            iOut;
    int            iInside;
    int            nPts;
    unsigned char  ucSinus;
    float          fT;
    float          fStatA;
    float          fStatB;

    xMinXB = (float)((struct GLD_tdstViewportAttributes_ *)p_stRect)->dwClipLeftInPix;
    xMaxXB = (float)((struct GLD_tdstViewportAttributes_ *)p_stRect)->dwClipRightInPix;
    xMinYB = (float)((struct GLD_tdstViewportAttributes_ *)p_stRect)->dwClipTopInPix;
    xMaxYB = (float)((struct GLD_tdstViewportAttributes_ *)p_stRect)->dwClipBottomInPix;

    sPtCount = ((struct GEO_tdstGeometricObject_ *)p_stObj)->xNbPoints;
    sIdx = sPtCount;
    pfVar8 = afPtCam;
    iVar9 = 4;
    do {
        sIdx = (short)(sIdx - 1);
        GEO_vGetPointOfObject(p_stObj, pfVar8, sIdx);
        pfVar8 += 3;
        iVar9--;
    } while (iVar9 != 0);
    POS_fn_vSetIdentityMatrix((POS_tdstCompletePosition *)aulMat);
    POS_fn_vMulMatrixMatrix((POS_tdstCompletePosition *)aulMat,
                            (POS_tdstCompletePosition *)p_stMatB,
                            (POS_tdstCompletePosition *)p_stMatA);
    GLI_pc_vTransformPointsByObject(4, afPtCam, afPtXf, aulMat);

    fZMin = p_stCam->xFar;
    ucSinus = GLI_pc_g_cSinusEffectState;
    pfVar8 = afPtXf + 2;
    lSelIdx = 0;
    iVar9 = 4;
    do {
        if ((((ucSinus == 1) && ((ulFlags & 0x80000000UL) == 0)) ||
             ((ucSinus == 2) && ((ulFlags & 0x80000000UL) != 0))) ||
            (ucSinus == 3)) {
            GLI_pc_fn_vOffsetPointBySinusEffect(pfVar8 - 2);
            ucSinus = GLI_pc_g_cSinusEffectState;
        }
        if (*pfVar8 <= fZMin) {
            fZMin = *pfVar8;
        }
        if (p_stCam->xNear <= *pfVar8) {
            aucFlag[lSelIdx] = 0;
        } else {
            aucFlag[lSelIdx] = 1;
        }
        pfVar8 += 4;
        lSelIdx++;
        iVar9--;
    } while (iVar9 != 0);
    fThr = p_stCam->xNear;
    if (fZMin <= fThr) {
        fZMin = fThr;
    }
    p_stCam->xNear = fZMin;
    fZClip = fZMin;

    iInside = (aucFlag[0] == 0) && (aucFlag[1] == 0) && (aucFlag[2] == 0) && (aucFlag[3] == 0);

    if (!iInside) {

        if ((aucFlag[0] != 0) && (aucFlag[1] != 0) && (aucFlag[2] != 0) && (aucFlag[3] != 0)) {
            return (unsigned short)((unsigned short)aucFlag[1] << 8);
        }

        nPts = 0;
        iVar12 = 1;
        pfVar8 = afPtXf;
        iVar9 = 4;
        do {
            if (aucFlag[iVar12 - 1] == 0) {
                iOut = nPts;
                afPtClip[iOut * 4]     = pfVar8[0];
                afPtClip[iOut * 4 + 1] = pfVar8[1];
                afPtClip[iOut * 4 + 2] = pfVar8[2];
                nPts++;
            } else {
                iNext = iVar12 & 3;
                if (aucFlag[iNext] == 0) {
                    iOut = nPts;
                    fT = (fZClip - pfVar8[2]) / afPtXf[iNext * 4 + 2];
                    nPts++;
                    afPtClip[iOut * 4]     = afPtXf[iNext * 4] * fT + pfVar8[0];
                    afPtClip[iOut * 4 + 1] = afPtXf[iNext * 4 + 1] * fT + pfVar8[1];
                    afPtClip[iOut * 4 + 2] = fZClip;
                }
                iPrev = (iVar12 + 2) & 3;
                if (aucFlag[iPrev] == 0) {
                    iOut = nPts;
                    fT = (fZClip - pfVar8[2]) / afPtXf[iPrev * 4 + 2];
                    afPtClip[iOut * 4]     = afPtXf[iPrev * 4] * fT + pfVar8[0];
                    afPtClip[iOut * 4 + 1] = afPtXf[iPrev * 4 + 1] * fT + pfVar8[1];
                    afPtClip[iOut * 4 + 2] = fZClip;
                    nPts++;
                }
            }
            iVar12++;
            pfVar8 += 4;
            iVar9--;
        } while (iVar9 != 0);
        /* 0x42348D */
        GLI_xSerialProjection(p_stCam, nPts, afPtClip, afPtScr);
        fMinX = xMaxXB;
        fMaxX = xMinXB;
        fMinY = xMaxYB;
        fMaxY = xMinXB;
        if (0 < nPts) {
            pfVar8 = afPtScr + 1;
            iVar9 = nPts;
            do {
                if (pfVar8[-1] < fMinX) {
                    fMinX = pfVar8[-1];
                }
                if (fMaxX < pfVar8[-1]) {
                    fMaxX = pfVar8[-1];
                }
                if (*pfVar8 < fMinY) {
                    fMinY = *pfVar8;
                }
                if (fMaxY < *pfVar8) {
                    fMaxY = *pfVar8;
                }
                pfVar8 += 4;
                iVar9--;
            } while (iVar9 != 0);
        }
        if ((fMinX >= fMaxX) && (fMinX != fMaxX)) {
            fStatA = fMinX;
            fStatB = fMaxX;
            return (unsigned short)((((fStatA < fStatB) ? 1UL : 0UL) << 8) |
                                    (((fStatA != fStatA || fStatB != fStatB) ? 1UL : 0UL) << 10) |
                                    (((fStatA == fStatB) ? 1UL : 0UL) << 14));
        }
        if ((fMinY >= fMaxY) && (fMinY != fMaxY)) {
            fStatA = fMinY;
            fStatB = fMaxY;
            return (unsigned short)((((fStatA < fStatB) ? 1UL : 0UL) << 8) |
                                    (((fStatA != fStatA || fStatB != fStatB) ? 1UL : 0UL) << 10) |
                                    (((fStatA == fStatB) ? 1UL : 0UL) << 14));
        }
        fMinX = fMinX - 1.0f;
        fMaxX = fMaxX + 1.0f;
        fMinY = fMinY - 1.0f;
        fMaxY = fMaxY + 1.0f;
        if (fMinX < xMinXB) {
            fMinX = xMinXB;
        }
        if (xMaxXB < fMaxX) {
            fMaxX = xMaxXB;
        }
        if (fMinY < xMinYB) {
            fMinY = xMinYB;
        }
        xSel = fMaxY;
        if (xMaxYB < fMaxY) {
            xSel = xMaxYB;
        }
        ((struct GLD_tdstViewportAttributes_ *)p_stRect)->dwClipLeftInPix = (long)fMinX;
        ((struct GLD_tdstViewportAttributes_ *)p_stRect)->dwClipRightInPix = (long)fMaxX;
        ((struct GLD_tdstViewportAttributes_ *)p_stRect)->dwClipTopInPix = (long)fMinY;
        lSel = (long)xSel;
        ((struct GLD_tdstViewportAttributes_ *)p_stRect)->dwClipBottomInPix = lSel;
        return (unsigned short)((((unsigned long)lSel) & 0xff00UL) | 1UL);
    } else {
        GLI_xSerialProjection(p_stCam, 4, afPtXf, afPtScr);
        fMinX = xMaxXB;
        fMaxX = xMinXB;
        fMinY = xMaxYB;
        fMaxY = xMinXB;

        pfVar8 = afPtScr + 1;
        iVar9 = 4;
        do {
            if (pfVar8[-1] < fMinX) {
                fMinX = pfVar8[-1];
            }
            if (fMaxX < pfVar8[-1]) {
                fMaxX = pfVar8[-1];
            }
            if (*pfVar8 < fMinY) {
                fMinY = *pfVar8;
            }
            if (fMaxY < *pfVar8) {
                fMaxY = *pfVar8;
            }
            pfVar8 += 4;
            iVar9--;
        } while (iVar9 != 0);
        if ((fMinX >= fMaxX) && (fMinX != fMaxX)) {
            fStatA = fMinX;
            fStatB = fMaxX;
            return (unsigned short)((((fStatA < fStatB) ? 1UL : 0UL) << 8) |
                                    (((fStatA != fStatA || fStatB != fStatB) ? 1UL : 0UL) << 10) |
                                    (((fStatA == fStatB) ? 1UL : 0UL) << 14));
        }
        if ((fMinY >= fMaxY) && (fMinY != fMaxY)) {
            fStatA = fMinY;
            fStatB = fMaxY;
            return (unsigned short)((((fStatA < fStatB) ? 1UL : 0UL) << 8) |
                                    (((fStatA != fStatA || fStatB != fStatB) ? 1UL : 0UL) << 10) |
                                    (((fStatA == fStatB) ? 1UL : 0UL) << 14));
        }
        fMinX = fMinX - 1.0f;
        fMaxX = fMaxX + 1.0f;
        fMinY = fMinY - 1.0f;
        fMaxY = fMaxY + 1.0f;
        if (fMinX < xMinXB) {
            fMinX = xMinXB;
        }
        if (xMaxXB < fMaxX) {
            fMaxX = xMaxXB;
        }
        if (fMinY < xMinYB) {
            fMinY = xMinYB;
        }
        xSel = fMaxY;
        if (xMaxYB < fMaxY) {
            xSel = xMaxYB;
        }
        ((struct GLD_tdstViewportAttributes_ *)p_stRect)->dwClipLeftInPix = (long)fMinX;
        ((struct GLD_tdstViewportAttributes_ *)p_stRect)->dwClipRightInPix = (long)fMaxX;
        ((struct GLD_tdstViewportAttributes_ *)p_stRect)->dwClipTopInPix = (long)fMinY;
        lSel = (long)xSel;
        ((struct GLD_tdstViewportAttributes_ *)p_stRect)->dwClipBottomInPix = lSel;
        return (unsigned short)((((unsigned long)lSel) & 0xff00UL) | 1UL);
    }
}

static float GLI_pc_xSinusPhaseScale = 1.0f; /* 0x004A0700 */

void __cdecl GLI_pc_fn_vOffsetPointBySinusEffect(float *p_xPoint)
{
    float  fVar1;
    float  fNewX;
    float  fNewY;
    float  fIdx;
    float *pfBase = (float *)GLI_M_a3_lSinEffectBase;

    if (GLI_BIG_GLOBALS->lDisplayMenuIsDetected != 0) {
        fVar1 = *(float *)&GLI_M_lSinEffectBase;
    } else {
        fVar1 = GLI_RD_g_xMagic7244_storage;
    }

    fIdx = p_xPoint[2] + (p_xPoint[1] + (fVar1 + pfBase[0]));
    fNewX = GLI_M_p2CosTable[(*(unsigned long *)&fIdx) & 0x3f] * GLI_M_stSinEffectAmp.xX
            + p_xPoint[0];
    fIdx = p_xPoint[2] + (p_xPoint[0] + fVar1 + pfBase[1]);
    fNewY = GLI_M_p2CosTable[(*(unsigned long *)&fIdx) & 0x3f] * GLI_M_stSinEffectAmp.xY
            + p_xPoint[1];
    fIdx = p_xPoint[1] + (p_xPoint[0] + fVar1 + pfBase[2]);
    p_xPoint[2] = GLI_M_p2CosTable[(*(unsigned long *)&fIdx) & 0x3f] * GLI_M_stSinEffectAmp.xZ
                + p_xPoint[2];
    p_xPoint[0] = fNewX;
    p_xPoint[1] = fNewY;
}

static const float GLI_pc_xNegOneConst = -1.0f;

void __cdecl GLI_pc_fn_vAddWaterDepthColorToVertices(GLI_tdstInternalGlobalValuesFor3dEngine *p_stGlobals,
                                                    long   lFirstVertex,
                                                    long   lNbVertices)
{
    float          fVar1;
    float          fVar2;
    GEO_tdstColor *p_stColor;
    MTH_tdxReal   *p_xDepth;
    GEO_tdstColor *p_stLastColor;

    if (((GLI_M_WaterDepthColorFlags & 1UL) != 0) && ((GLI_M_WaterDepthColorFlags & 0x40UL) == 0)) {
        fVar1 = GLI_pc_xNegOneConst / *(float *)&GLI_RD_g_l06f8_storage;
        p_stColor = p_stGlobals->GLI_aDEF_stColorsRLID + lFirstVertex;
        p_stLastColor = p_stColor + lNbVertices;
        p_xDepth = p_stGlobals->GLI_aDEF_xWaterPlaneDepth + lFirstVertex;
        for (; p_stColor < p_stLastColor; p_stColor++) {

            if (*(long *)p_xDepth < 0) {
                /* 0x504070 */
                if (*p_xDepth <= -(*(float *)&GLI_RD_g_l06f8_storage)) {
                    p_stColor->xR = p_stColor->xR + *(float *)&GLI_M_pWaterDepthColorCoef[0];
                    p_stColor->xG = *(float *)&GLI_M_pWaterDepthColorCoef[1] + p_stColor->xG;
                    fVar2 = *(float *)&GLI_M_pWaterDepthColorCoef[2];
                } else {
                    fVar2 = *p_xDepth * fVar1;
                    p_stColor->xR = fVar2 * *(float *)&GLI_M_pWaterDepthColorCoef[0] + p_stColor->xR;
                    p_stColor->xG = fVar2 * *(float *)&GLI_M_pWaterDepthColorCoef[1] + p_stColor->xG;
                    fVar2 = fVar2 * *(float *)&GLI_M_pWaterDepthColorCoef[2];
                }
                p_stColor->xB = fVar2 + p_stColor->xB;
            }
            p_xDepth++;
        }
    }
}

void __cdecl GLI_pc_fn_vSetSinusEffectVertexColors(char *p_stGlobals,
                                                  long   lFirstVertex,
                                                  long   lNbVertices)
{
    float *pfRLIAmp  = (float *)GLI_M_a3_lSinRLIAmp;
    float *pfRLIBase = (float *)GLI_M_a3_lSinRLIBase;
    float *pfBase    = (float *)GLI_M_a3_lSinEffectBase;
    float *pfVar2;
    float *pfVar4;
    float *pfVar1;
    float  fIdx;
    unsigned long uIdx;
    float *pfCos;

    pfVar2 = (float *)(p_stGlobals + lFirstVertex * 0x10 + 0x14c14);
    pfVar4 = pfVar2 + lNbVertices * 4;
    pfVar1 = *(float **)(*(char **)(p_stGlobals + 4));
    for (; pfVar2 < pfVar4;) {

        fIdx = (pfBase[0] + (pfVar1[1] + pfVar1[2] + *pfVar1)
                + *(float *)&GLI_RD_g_xMagic7244_storage) * GLI_pc_xSinusPhaseScale;
        uIdx = (*(unsigned long *)&fIdx) & 0x3f;

        pfCos = &GLI_M_p2CosTable[uIdx];
        *pfVar2++     = pfRLIAmp[0] * *pfCos + pfRLIBase[0];
        *pfVar2++     = pfRLIAmp[1] * *pfCos + pfRLIBase[1];
        *pfVar2++     = pfRLIAmp[2] * *pfCos + pfRLIBase[2];
        pfVar2 = pfVar2 + 1;
        pfVar1 = pfVar1 + 3;
    }
}

/* 0x0050FBA0 */
extern unsigned long MTH_g_a1024_fInverse[1024];
#define GLI_g_aFastInverseZLUT_storage  MTH_g_a1024_fInverse

void __cdecl GLI_pc_fn_vComputeObjectClipFlags(char *p_stGlobals)
{
    char             *p_stObj;
    GLI_tdstCamera   *p_stCam;
    float             fVar1;
    float             fVar3;
    MTH3D_tdstVector  stCentre;
    struct GLI_tdstInternalGlobalValuesFor3dEngine_ *p_stG;

    p_stG = (struct GLI_tdstInternalGlobalValuesFor3dEngine_ *)p_stGlobals;
    p_stObj = (char *)p_stG->p_stObj;
    POS_fn_vMulMatrixVertex(&p_stG->stCenterOfCurrentBoundingSphere,
                            p_stG->gs_st_CurrentMatrix,
                            &((struct GEO_tdstGeometricObject_ *)p_stObj)->xBoudingSphereCenter);
    p_stObj = (char *)p_stG->p_stObj;
    fVar3 = POS_fn_xGetMaxScale(p_stG->gs_st_CurrentMatrix);
    fVar1 = ((struct GEO_tdstGeometricObject_ *)p_stObj)->xBoudingSphereRadius;
    p_stG->lClippingModeMask = 0;
    p_stG->stRadiusOfCurrentBoundingSphere = fVar3 * fVar1;
    if (p_stG->stRadiusOfCurrentBoundingSphere == 0.0f) {
        p_stG->lClippingModeMask = 0x10f;
        return;
    }
    POS_fn_vMulMatrixVertex(&stCentre,
                            &p_stG->gs_st_CurrentMatrixMultiplyedWithCameraMatrix,
                            &((struct GEO_tdstGeometricObject_ *)p_stObj)->xBoudingSphereCenter);

    if (stCentre.xZ - p_stG->stRadiusOfCurrentBoundingSphere
        <= GLI_BIG_GLOBALS->p_stCurrentCamera->xNear) {
        p_stG->lClippingModeMask |= 0x100UL;
    }
    p_stCam = p_stG->p_stCurrentCamera;
    if (p_stCam->xDistPlaneLeft - p_stG->stRadiusOfCurrentBoundingSphere
        <= p_stCam->stNormPlaneLeft.xZ * stCentre.xZ
         + p_stCam->stNormPlaneLeft.xX * stCentre.xX
         + p_stCam->stNormPlaneLeft.xY * stCentre.xY) {
        p_stG->lClippingModeMask |= 2UL;
    }
    if (GLI_BIG_GLOBALS->p_stCurrentCamera->xDistPlaneRight - p_stG->stRadiusOfCurrentBoundingSphere
        <= p_stCam->stNormPlaneRight.xZ * stCentre.xZ
         + p_stCam->stNormPlaneRight.xX * stCentre.xX
         + p_stCam->stNormPlaneRight.xY * stCentre.xY) {
        p_stG->lClippingModeMask |= 1UL;
    }
    if (p_stCam->xDistPlaneUp - p_stG->stRadiusOfCurrentBoundingSphere
        <= p_stCam->stNormPlaneUp.xZ * stCentre.xZ
         + p_stCam->stNormPlaneUp.xX * stCentre.xX
         + p_stCam->stNormPlaneUp.xY * stCentre.xY) {
        p_stG->lClippingModeMask |= 4UL;
    }
    if (p_stCam->xDistPlaneDown - p_stG->stRadiusOfCurrentBoundingSphere
        <= p_stCam->stNormPlaneDown.xZ * stCentre.xZ
         + p_stCam->stNormPlaneDown.xX * stCentre.xX
         + p_stCam->stNormPlaneDown.xY * stCentre.xY) {
        p_stG->lClippingModeMask |= 8UL;
    }
}

void __cdecl GLI_pc_fn_vTransformObjectVertices(char *p_stGlobals)
{
    GLI_tdstCamera   *p_stCam;
    MTH3D_tdstVector  stI, stJ, stK, stT;
    float             fVar2, fVar3, fVar5;
    float             fScaleX, fScaleY;
    float            *pfVar6;
    float            *pfVar7;
    float            *pfVar4;

    p_stCam = ((struct GLI_tdstInternalGlobalValuesFor3dEngine_ *)p_stGlobals)->p_stCurrentCamera;
    fScaleX = p_stCam->stScale.xX * p_stCam->xScreen;
    fScaleY = p_stCam->stScale.xY * p_stCam->xScreen;
    POS_fn_vGetTransformMatrix((POS_tdstCompletePosition *)(p_stGlobals + 0x2d344), &stI, &stJ, &stK);
    fVar3 = stK.xY; fVar2 = stK.xX; fVar5 = stJ.xX;
    stJ.xX = stI.xY; stK.xX = stI.xZ; stI.xY = fVar5;
    stK.xY = stJ.xZ; stI.xZ = fVar2; stJ.xZ = fVar3;
    POS_fn_vGetTranslationVector((POS_tdstCompletePosition *)(p_stGlobals + 0x2d344), &stT);

    pfVar7 = *(float **)(*(char **)(p_stGlobals + 4));
    pfVar6 = (float *)(p_stGlobals + 0x2c);
    pfVar4 = pfVar6 + *(short *)(*(char **)(p_stGlobals + 4) + 0x2c) * 4;
    for (; pfVar6 < pfVar4; pfVar6 = pfVar6 + 4) {
        pfVar6[2] = pfVar7[2] * stK.xZ + pfVar7[0] * stK.xX + pfVar7[1] * stK.xY + stT.xZ;
        pfVar6[0] = pfVar7[2] * stI.xZ + pfVar7[0] * stI.xX + pfVar7[1] * stI.xY + stT.xX;
        pfVar6[1] = pfVar7[2] * stJ.xZ + pfVar7[0] * stJ.xX + pfVar7[1] * stJ.xY + stT.xY;
        /* 0x42CFA2 */
        fVar5 = MTH_fn_fInverseOpt(pfVar6[2]);
        pfVar6[10000] = pfVar6[0] * fVar5 * fScaleX + p_stCam->stTrans.xX;
        pfVar6[0x2711] = fVar5 * fScaleY * pfVar6[1] + p_stCam->stTrans.xY;
        pfVar6[0x2712] = fVar5 * 0.25f;
        pfVar7 = pfVar7 + 3;
    }
}

void __cdecl GLI_pc_fn_vTransformObjectVerticesClipped(char *p_stGlobals)
{
    GLI_tdstCamera   *p_stCam;
    MTH3D_tdstVector  stI, stJ, stK, stT;
    float             fVar2, fVar3, fVar5;
    float             fScaleX, fScaleY;
    float            *pfVar6;
    float            *pfVar7;
    float            *pfVar8;
    float            *pfVar4;
    unsigned long     ulFar;

    p_stCam = ((struct GLI_tdstInternalGlobalValuesFor3dEngine_ *)p_stGlobals)->p_stCurrentCamera;
    fScaleX = p_stCam->stScale.xX * p_stCam->xScreen;
    ulFar   = 0x45fa0000UL;
    fScaleY = p_stCam->stScale.xY * p_stCam->xScreen;
    POS_fn_vGetTransformMatrix((POS_tdstCompletePosition *)(p_stGlobals + 0x2d344), &stI, &stJ, &stK);
    fVar3 = stK.xY; fVar2 = stK.xX; fVar5 = stJ.xX;
    stJ.xX = stI.xY; stK.xX = stI.xZ; stI.xY = fVar5;
    stK.xY = stJ.xZ; stI.xZ = fVar2; stJ.xZ = fVar3;
    POS_fn_vGetTranslationVector((POS_tdstCompletePosition *)(p_stGlobals + 0x2d344), &stT);

    pfVar7 = *(float **)(*(char **)(p_stGlobals + 4));
    pfVar8 = (float *)(p_stGlobals + 0x2c);
    pfVar4 = pfVar8 + *(short *)(*(char **)(p_stGlobals + 4) + 0x2c) * 4;
    if (pfVar8 < pfVar4) {
        pfVar6 = (float *)(p_stGlobals + 0x9c74);
        do {
            *pfVar8 = pfVar7[2] * stI.xZ + pfVar7[0] * stI.xX + pfVar7[1] * stI.xY + stT.xX;
            pfVar6[-0x2711] = pfVar7[2] * stJ.xZ + pfVar7[0] * stJ.xX + pfVar7[1] * stJ.xY + stT.xY;
            pfVar6[-10000] = pfVar7[2] * stK.xZ + pfVar7[0] * stK.xX + pfVar7[1] * stK.xY + stT.xZ;

            if (*(int *)&pfVar6[-10000] < *(int *)&p_stCam->xNear) {
                *(unsigned long *)&pfVar6[0] |= 0x80000000UL;
                *(unsigned long *)&pfVar6[-2] = (~*(unsigned long *)&pfVar8[0] & 0x80000000UL) | ulFar;
                *(unsigned long *)&pfVar6[-1] = (~*(unsigned long *)&pfVar6[-0x2711] & 0x80000000UL) | ulFar;
            } else {

                fVar5 = MTH_fn_fInverseOpt(pfVar6[-10000]);
                pfVar6[-2] = fVar5 * fScaleX * pfVar8[0] + p_stCam->stTrans.xX;
                pfVar6[-1] = fVar5 * fScaleY * pfVar6[-0x2711] + p_stCam->stTrans.xY;
                pfVar6[0] = fVar5 * 0.25f;
            }
            pfVar8 = pfVar8 + 4;
            pfVar6 = pfVar6 + 4;
            pfVar7 = pfVar7 + 3;
        } while (pfVar8 < pfVar4);
    }
}

void __cdecl GLI_pc_fn_vTransformObjectVerticesSinus(char *p_stGlobals)
{
    GLI_tdstCamera   *p_stCam;
    MTH3D_tdstVector  stI, stJ, stK, stT;
    float             fVar2, fVar3, fVar4;
    float             fScaleX, fScaleY;
    float            *pfVar5;
    float            *pfVar6;
    float            *pfVar7;
    float            *pfEnd;
    float             fZ;
    float             fRecip;

    p_stCam = ((struct GLI_tdstInternalGlobalValuesFor3dEngine_ *)p_stGlobals)->p_stCurrentCamera;
    fScaleX = p_stCam->stScale.xX * p_stCam->xScreen;
    fScaleY = p_stCam->stScale.xY * p_stCam->xScreen;
    POS_fn_vGetTransformMatrix((POS_tdstCompletePosition *)(p_stGlobals + 0x2d344), &stI, &stJ, &stK);
    fVar4 = stK.xY; fVar3 = stK.xX; fVar2 = stJ.xX;
    stJ.xX = stI.xY; stK.xX = stI.xZ; stI.xY = fVar2;
    stK.xY = stJ.xZ; stJ.xZ = fVar4; stI.xZ = fVar3;
    POS_fn_vGetTranslationVector((POS_tdstCompletePosition *)(p_stGlobals + 0x2d344), &stT);

    pfVar5 = (float *)(p_stGlobals + 0x2c);
    pfVar7 = *(float **)(*(char **)(p_stGlobals + 4));
    pfEnd  = pfVar5 + *(short *)(*(char **)(p_stGlobals + 4) + 0x2c) * 4;
    if (pfVar5 < pfEnd) {
        do {
            pfVar5[0] = pfVar7[2] * stI.xZ + pfVar7[0] * stI.xX + pfVar7[1] * stI.xY + stT.xX;
            pfVar5[1] = pfVar7[2] * stJ.xZ + pfVar7[0] * stJ.xX + pfVar7[1] * stJ.xY + stT.xY;
            pfVar5[2] = pfVar7[2] * stK.xZ + pfVar7[0] * stK.xX + pfVar7[1] * stK.xY + stT.xZ;
            GLI_pc_fn_vOffsetPointBySinusEffect(pfVar5);
            fZ = pfVar5[2];

            fRecip = MTH_fn_fInverseOpt(fZ);
            pfVar5[10000] = pfVar5[0] * fRecip * fScaleX + p_stCam->stTrans.xX;
            pfVar6 = pfVar5 + 4;
            pfVar7 = pfVar7 + 3;
            pfVar5[0x2711] = fRecip * fScaleY * pfVar5[1] + p_stCam->stTrans.xY;
            pfVar5[0x2712] = p_stCam->xNear * fRecip;
            pfVar5 = pfVar6;
        } while (pfVar6 < pfEnd);
    }
}

void __cdecl GLI_pc_fn_vTransformObjectVerticesSinusClipped(char *p_stGlobals)
{
    GLI_tdstCamera   *p_stCam;
    MTH3D_tdstVector  stI, stJ, stK, stT;
    float             fVar2, fVar3, fVar4;
    float             fScaleX, fScaleY;
    float            *pfVar5;
    float            *pfVar6;
    float            *pfVar7;
    float            *pfEnd;
    float            *pfVtx;
    float             fZ;
    float             fRecip;
    unsigned long     ulFar;

    p_stCam = ((struct GLI_tdstInternalGlobalValuesFor3dEngine_ *)p_stGlobals)->p_stCurrentCamera;
    fScaleX = p_stCam->stScale.xX * p_stCam->xScreen;
    ulFar   = 0x45fa0000UL;
    fScaleY = p_stCam->stScale.xY * p_stCam->xScreen;
    POS_fn_vGetTransformMatrix((POS_tdstCompletePosition *)(p_stGlobals + 0x2d344), &stI, &stJ, &stK);
    fVar4 = stK.xY; fVar3 = stK.xX; fVar2 = stJ.xX;
    stJ.xX = stI.xY; stK.xX = stI.xZ; stI.xY = fVar2;
    stK.xY = stJ.xZ; stJ.xZ = fVar4; stI.xZ = fVar3;
    POS_fn_vGetTranslationVector((POS_tdstCompletePosition *)(p_stGlobals + 0x2d344), &stT);

    pfVar5 = (float *)(p_stGlobals + 0x2c);
    pfVar7 = *(float **)(*(char **)(p_stGlobals + 4));
    pfEnd  = pfVar5 + *(short *)(*(char **)(p_stGlobals + 4) + 0x2c) * 4;
    if (pfVar5 < pfEnd) {
        pfVar6 = (float *)(p_stGlobals + 0x9c74);
        do {
            pfVtx = pfVar5;
            *pfVar5 = pfVar7[2] * stI.xZ + pfVar7[0] * stI.xX + pfVar7[1] * stI.xY + stT.xX;
            pfVar6[-0x2711] = pfVar7[2] * stJ.xZ + pfVar7[0] * stJ.xX + pfVar7[1] * stJ.xY + stT.xY;
            pfVar6[-10000] = pfVar7[2] * stK.xZ + pfVar7[0] * stK.xX + pfVar7[1] * stK.xY + stT.xZ;
            GLI_pc_fn_vOffsetPointBySinusEffect(pfVtx);

            if (*(int *)&pfVar6[-10000] < *(int *)&p_stCam->xNear) {
                *(unsigned long *)&pfVar6[0] |= 0x80000000UL;
                *(unsigned long *)&pfVar6[-2] = (~*(unsigned long *)&pfVtx[0] & 0x80000000UL) | ulFar;
                *(unsigned long *)&pfVar6[-1] = (~*(unsigned long *)&pfVar6[-0x2711] & 0x80000000UL) | ulFar;
                pfVtx = pfVar5;
            } else {
                fZ = pfVar6[-10000];

                fRecip = MTH_fn_fInverseOpt(fZ);
                pfVar6[-2] = fRecip * fScaleX * *pfVtx + p_stCam->stTrans.xX;
                pfVar6[-1] = fRecip * fScaleY * pfVar6[-0x2711] + p_stCam->stTrans.xY;
                pfVar6[0] = fRecip * 0.25f;
            }
            pfVar5 = pfVtx + 4;
            pfVar6 = pfVar6 + 4;
            pfVar7 = pfVar7 + 3;
        } while (pfVar5 < pfEnd);
    }
}
