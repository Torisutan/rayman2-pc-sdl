#include <string.h>
#include "GLI/GLI_globals.h"
#include "GLI/GLI_BigGlobals.h"
#include "GLI/Vpt3D.h"
#include "GLI/mater_st.h"
#include "GLI/Object.h"
#include "GLI/Proj.h"
#include "GLI/u_Draw.h"
#include "POS/POS_types.h"
#include "MTH/MTH3D.h"
#include "GLD/GLD_Pub.h"
#ifdef RAYMAN2_WIDESCREEN
#include "GLI/GLI_ratio.h"
#endif /* RAYMAN2_WIDESCREEN */

void POS_fn_vGetTransformMatrix(POS_tdstCompletePosition *_hMatrix,
                                MTH3D_tdstVector *_p_stI,
                                MTH3D_tdstVector *_p_stJ,
                                MTH3D_tdstVector *_p_stK);
void POS_fn_vGetTranslationVector(POS_tdstCompletePosition *_hMatrix,
                                  MTH3D_tdstVector *_p_stTrs);

void GLI_pc_vTransformPointsByObject(long lCount, float *p_xSrc, float *p_xDst, void *p_stObj)
{
    MTH3D_tdstVector stI;
    MTH3D_tdstVector stJ;
    MTH3D_tdstVector stK;
    MTH3D_tdstVector stT;
    float *p_xDstEnd;
    float *p_xDstCur;

    POS_fn_vGetTransformMatrix((POS_tdstCompletePosition *)p_stObj, &stI, &stJ, &stK);
    POS_fn_vGetTranslationVector((POS_tdstCompletePosition *)p_stObj, &stT);

    p_xDstEnd = p_xDst + lCount * 4;
    p_xDstCur = p_xDst;
    for (; p_xDstCur < p_xDstEnd; p_xDstCur = p_xDstCur + 4)
    {

        p_xDstCur[0] = p_xSrc[1] * stJ.xX + p_xSrc[0] * stI.xX + p_xSrc[2] * stK.xX + stT.xX;
        p_xDstCur[1] = p_xSrc[1] * stJ.xY + p_xSrc[0] * stI.xY + p_xSrc[2] * stK.xY + stT.xY;
        p_xDstCur[2] = p_xSrc[1] * stJ.xZ + p_xSrc[0] * stI.xZ + p_xSrc[2] * stK.xZ + stT.xZ;
        p_xSrc = p_xSrc + 3;
    }
}

void GLI_vDisplayWorldTriangle(struct GLD_tdstViewportAttributes_ *p_stViewport,
                               float c[3][3],
                               float uvs[3][2],
                               void *pVisualMaterial)
{
    GLI_tdstCamera *p_stCam;
    long lSavedDrawMode;
    float aSrc[9];
    float aCam[12];
    float aProj[12];
    unsigned long aUv[6];

    p_stCam = ((GLI_tdstSpecificAttributesFor3D *)p_stViewport->p_vSpecificToXD)->p_stCam;

    aSrc[0] = c[0][0];
    aSrc[1] = c[0][1];
    aSrc[2] = c[0][2];
    aSrc[3] = c[1][0];
    aSrc[4] = c[1][1];
    aSrc[5] = c[1][2];
    aSrc[6] = c[2][0];
    aSrc[7] = c[2][1];
    aSrc[8] = c[2][2];
    aUv[0] = *(unsigned long *)&uvs[0][0];
    aUv[1] = *(unsigned long *)&uvs[0][1];
    aUv[2] = *(unsigned long *)&uvs[1][0];
    aUv[3] = *(unsigned long *)&uvs[1][1];
    aUv[4] = *(unsigned long *)&uvs[2][0];
    aUv[5] = *(unsigned long *)&uvs[2][1];

    GLI_pc_vTransformPointsByObject(3, aSrc, aCam, &p_stCam->stMatrix);
    GLI_xSerialProjection(p_stCam, 3, aCam, aProj);

    GLI_BIG_GLOBALS->hCurrentMaterial = (struct GLI_tdstMaterial_ *)pVisualMaterial;

    lSavedDrawMode = GLI_BIG_GLOBALS->lHierachDrawMask;
    GLI_BIG_GLOBALS->lHierachDrawMask = lSavedDrawMode & 0xFFFFFBF7;
    GLI_BIG_GLOBALS->lCurrentDrawMask = GLI_BIG_GLOBALS->lHierachDrawMask;

    GLI_vDoMaterialSelection(GLI_BIG_GLOBALS);
    GLI_DRV_g_vSendSingleTrianglesToClip((struct GLI_tdstAligned2DVector_ *)aProj,
                                         (struct GLI_tdst2DUVValues_ *)aUv,
                                         GLI_BIG_GLOBALS);

    GLI_BIG_GLOBALS->lHierachDrawMask = lSavedDrawMode;
}

/* 0x00424860 */
void __cdecl GLI_xSetMaterialAmbientCoef(void *hMaterial, void *p_stColor);
void __cdecl GLI_xSetMaterialDiffuseCoef(void *hMaterial, void *p_stColor);
void __cdecl GLI_xSetMaterialSpecularCoef(void *hMaterial, long lExp, void *p_stColor);

void GLI_vDraw2DSprite(struct GLD_tdstViewportAttributes_ *p_stVpt,
                                float fParam2,
                                float fParam3,
                                float fParam4,
                                float fParam5,
                                void *p_vMaterial)
{
    float aColor[3];
    float aVtx[4][4];
    float fI2;
    float fI3;
    float fI4;
    float fI5;

    aColor[0] = 0.0f;
    aColor[1] = 0.0f;
    aColor[2] = 0.0f;
    GLI_xSetMaterialAmbientCoef(p_vMaterial, aColor);

    aColor[0] = 1.0f;
    aColor[1] = 1.0f;
    aColor[2] = 1.0f;
    GLI_xSetMaterialDiffuseCoef(p_vMaterial, aColor);

    aColor[0] = 0.0f;
    aColor[1] = 0.0f;
    aColor[2] = 0.0f;
    GLI_xSetMaterialSpecularCoef(p_vMaterial, 0, aColor);

#ifdef RAYMAN2_WIDESCREEN
    /* in the 4:3 band, except full-width sprites */
    fI2 = MTH_M_xLongToReal(p_stVpt->dwWidth);
    if ( GLI_g_xSafeAreaRatio < MTH_C_ONE
      && !( fParam3 <= MTH_C_ZERO && fParam2 >= fI2 ) )
    {
        fParam2 = GLI_fn_xToSafeArea(fParam2, fI2);
        fParam3 = GLI_fn_xToSafeArea(fParam3, fI2);
    }
#endif /* RAYMAN2_WIDESCREEN */

    /* 0x429569 */
    fI2 = (float)(long)fParam2;
    fI4 = (float)(long)fParam4;
    fI3 = (float)(long)fParam3;

    aVtx[0][0] = fI2;
    aVtx[0][1] = fI4;
    aVtx[0][2] = 1.0f;
    aVtx[1][0] = fI3;
    aVtx[1][1] = fI4;
    aVtx[1][2] = 1.0f;
    fI5 = (float)(long)fParam5;
    aVtx[2][0] = fI3;
    aVtx[2][1] = fI5;
    aVtx[2][2] = 1.0f;
    aVtx[3][0] = fI2;
    aVtx[3][1] = fI5;
    aVtx[3][2] = 1.0f;

    GLI_BIG_GLOBALS->hCurrentMaterial = (struct GLI_tdstMaterial_ *)p_vMaterial;
    GLI_BIG_GLOBALS->hCurrentMaterial->ulMaterialType = 0xFFFFFBFFUL;
    GLI_BIG_GLOBALS->lCurrentDrawMask = 0xFFFFFBFF;
    GLI_BIG_GLOBALS->lHierachDrawMask = 0xFFFFFBFF;

    GLI_vDoMaterialSelection(GLI_BIG_GLOBALS);
    GLI_BIG_GLOBALS->xTextureDontAcceptFog = 1;
    (*GLI_DRV_g_vSendSpriteToClip)((struct GLI_tdstAligned2DVector_ *)aVtx,
                                   1.0f,
                                   GLI_BIG_GLOBALS);
}

extern unsigned char GLI_M_g_bAnimFinishedFlag_storage;     /* 0x4a7270 */
extern unsigned char GLI_M_g_bTakeCareOfAnimCoord_storage;  /* 0x4a7274 */
extern unsigned char GLI_M_g_bOneShotAnim_storage;          /* 0x4a7278 */
extern float         GLI_M_g_xBaseTimeForAnim_storage;      /* 0x503d60 */

int GLI_vDrawWorldPlanarSprite(MTH3D_tdstVector *pPosition,
                               void *pVisualMaterial,
                               MTH_tdxReal fscaleX,
                               MTH_tdxReal fscaleY,
                               struct GLD_tdstViewportAttributes_ *_p_stViewport,
                               unsigned long AnimTime)
{
    GLI_tdstCamera *p_stCam;
    GLI_tdstAligned3DVector *p_stTurned;
    float aPts[15];
    float aProj[20];
    float fSavedBase;
    unsigned char ucSavedByte;
    long lSavedDrawMode;

    p_stCam = ((GLI_tdstSpecificAttributesFor3D *)_p_stViewport->p_vSpecificToXD)->p_stCam;

    aPts[0] = pPosition->xX - fscaleX;
    aPts[1] = pPosition->xY - fscaleY;
    aPts[2] = pPosition->xZ;
    aPts[3] = pPosition->xX + fscaleX;
    aPts[4] = pPosition->xY - fscaleY;
    aPts[5] = pPosition->xZ;
    aPts[6] = pPosition->xX + fscaleX;
    aPts[7] = fscaleY + pPosition->xY;
    aPts[8] = pPosition->xZ;
    aPts[9] = pPosition->xX - fscaleX;
    aPts[10] = fscaleY + pPosition->xY;
    aPts[11] = pPosition->xZ;
    aPts[12] = pPosition->xX;
    aPts[13] = pPosition->xY;
    aPts[14] = pPosition->xZ;

    p_stTurned = GLI_BIG_GLOBALS->GLI_TurnedScaledDisplecedPoint;
    fSavedBase = p_stTurned->xX;
    ucSavedByte = *(unsigned char *)&p_stTurned->xY;
    GLI_pc_vTransformPointsByObject(5, aPts, (float *)p_stTurned, &p_stCam->stMatrix);
    GLI_xSerialProjection(p_stCam, 5, (float *)GLI_BIG_GLOBALS->GLI_TurnedScaledDisplecedPoint, aProj);

    GLI_M_g_xBaseTimeForAnim_storage = (float)AnimTime;
    GLI_M_g_bTakeCareOfAnimCoord_storage = 1;
    GLI_M_g_bOneShotAnim_storage = 1;
    GLI_M_g_bAnimFinishedFlag_storage = 0;

    GLI_BIG_GLOBALS->hCurrentMaterial = (struct GLI_tdstMaterial_ *)pVisualMaterial;
    lSavedDrawMode = GLI_BIG_GLOBALS->lHierachDrawMask;
    GLI_BIG_GLOBALS->lHierachDrawMask = lSavedDrawMode & 0xFFFFFBF7;
    GLI_BIG_GLOBALS->lCurrentDrawMask = GLI_BIG_GLOBALS->lHierachDrawMask;

    GLI_vDoMaterialSelection(GLI_BIG_GLOBALS);
    GLI_BIG_GLOBALS->xTextureDontAcceptFog = 1;
    GLI_BIG_GLOBALS->xWaterPlaneDistance = 1;
    (*GLI_DRV_g_vSendSpriteToClip)((struct GLI_tdstAligned2DVector_ *)aProj,
                                   aProj[18],
                                   GLI_BIG_GLOBALS);
    GLI_BIG_GLOBALS->xWaterPlaneDistance = 0;

    p_stTurned =GLI_BIG_GLOBALS->GLI_TurnedScaledDisplecedPoint;
    p_stTurned->xX = fSavedBase;
    *(unsigned char *)&p_stTurned->xY = ucSavedByte;
    GLI_BIG_GLOBALS->lHierachDrawMask = lSavedDrawMode;

    GLI_M_g_bTakeCareOfAnimCoord_storage = 0;
    GLI_M_g_bOneShotAnim_storage = 0;
    return GLI_M_g_bAnimFinishedFlag_storage;
}

int GLI_vDrawWorldOrthoPlanarSprite(MTH3D_tdstVector *pPosition,
                                    void *pVisualMaterial,
                                    MTH_tdxReal fscaleX,
                                    MTH_tdxReal fscaleY,
                                    struct GLD_tdstViewportAttributes_ *_p_stViewport,
                                    unsigned long AnimTime,
                                    MTH3D_tdstVector *p_stV1,
                                    MTH3D_tdstVector *p_stV2,
                                    int FollowZ)
{
    GLI_tdstCamera *p_stCam;
    float aPts[15];
    GLI_tdstAligned3DVector a5_stSaved[5];
    float aProj[20];
    float x1, y1, z1;
    float x2, y2, z2;
    long lSavedDrawMode;

    p_stCam = ((GLI_tdstSpecificAttributesFor3D *)_p_stViewport->p_vSpecificToXD)->p_stCam;

    x1 = (fscaleX * p_stV1->xX);
    y1 = (fscaleX * p_stV1->xY);
    z1 = (fscaleX * p_stV1->xZ);
    x2 = (fscaleY * p_stV2->xX);
    y2 = (fscaleY * p_stV2->xY);
    z2 = (fscaleY * p_stV2->xZ);

    aPts[0] = x2 + (pPosition->xX - x1);
    aPts[1] = y2 + (pPosition->xY - y1);
    aPts[2] = z2 + (pPosition->xZ - z1);
    aPts[3] = pPosition->xX + x2 + x1;
    aPts[4] = y2 + y1 + pPosition->xY;
    aPts[5] = z2 + z1 + pPosition->xZ;
    aPts[6] = (pPosition->xX + x1) - x2;
    aPts[7] = (y1 + pPosition->xY) - y2;
    aPts[8] = (z1 + pPosition->xZ) - z2;
    aPts[9] = (pPosition->xX - x1) - x2;
    aPts[10] = (pPosition->xY - y1) - y2;
    aPts[11] = (pPosition->xZ - z1) - z2;
    aPts[12] = pPosition->xX;
    aPts[13] = pPosition->xY;
    aPts[14] = pPosition->xZ;

    memcpy(a5_stSaved, GLI_BIG_GLOBALS->GLI_TurnedScaledDisplecedPoint, sizeof(a5_stSaved));

    GLI_pc_vTransformPointsByObject(5, aPts, (float *)GLI_BIG_GLOBALS->GLI_TurnedScaledDisplecedPoint,
                                    &p_stCam->stMatrix);
    GLI_xSerialProjection(p_stCam, 5, (float *)GLI_BIG_GLOBALS->GLI_TurnedScaledDisplecedPoint, aProj);

    GLI_M_g_xBaseTimeForAnim_storage = (float)AnimTime;
    GLI_M_g_bTakeCareOfAnimCoord_storage = 1;
    GLI_M_g_bOneShotAnim_storage = 1;
    GLI_M_g_bAnimFinishedFlag_storage = 0;

    GLI_BIG_GLOBALS->hCurrentMaterial = (struct GLI_tdstMaterial_ *)pVisualMaterial;
    lSavedDrawMode = GLI_BIG_GLOBALS->lHierachDrawMask;
    GLI_BIG_GLOBALS->lHierachDrawMask = lSavedDrawMode & 0xFFFFFBF7;
    GLI_BIG_GLOBALS->lCurrentDrawMask = GLI_BIG_GLOBALS->lHierachDrawMask;

    GLI_vDoMaterialSelection(GLI_BIG_GLOBALS);
    GLI_BIG_GLOBALS->xTextureDontAcceptFog = 1;
    GLI_BIG_GLOBALS->xWaterPlaneDistance = 1;
    (*GLI_DRV_g_vSendSpriteToClip)((struct GLI_tdstAligned2DVector_ *)aProj,
                                   aProj[18],
                                   GLI_BIG_GLOBALS);
    GLI_BIG_GLOBALS->xWaterPlaneDistance = 0;

    memcpy(GLI_BIG_GLOBALS->GLI_TurnedScaledDisplecedPoint, a5_stSaved, sizeof(a5_stSaved));
    GLI_BIG_GLOBALS->lHierachDrawMask = lSavedDrawMode;

    GLI_M_g_bTakeCareOfAnimCoord_storage = 0;
    GLI_M_g_bOneShotAnim_storage = 0;
    return GLI_M_g_bAnimFinishedFlag_storage;
}
