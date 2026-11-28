#include "POS/POS_types.h"
#include "GLI/light_st.h"
#include "GLI/GLI_BigGlobals.h"

/* 0x0050FBA0 */
extern unsigned long MTH_g_a1024_fInverse[1024];
#define GLI_g_aFastInverseZLUT_storage  MTH_g_a1024_fInverse

#define GLI_pc_xFastRecip(xValue) MTH_fn_fInverseOpt(xValue)

unsigned long __fastcall GLI_pc_fn_ulClassifyLightVsSphere(struct GLI_tdstLight_ *p_stLight,
                                                          GLI_tdstInternalGlobalValuesFor3dEngine *p_stGlobals)
{
    unsigned long ulResult = 0xFUL;
    MTH3D_tdstVector stDiff;
    float            xDist;

    if (p_stGlobals->stRadiusOfCurrentBoundingSphere == 0.0f) {
        return 0xFUL;
    }
    /* 0x42715B */
    MTH3D_M_vSubVector(&stDiff, &p_stLight->stMatrix.stTranslationVector,
                       &p_stGlobals->stCenterOfCurrentBoundingSphere);
    xDist = MTH_fn_fSquareRootOpt(MTH3D_M_xSqrVector(&stDiff));

    if (p_stLight->xFar < xDist - p_stGlobals->stRadiusOfCurrentBoundingSphere) {
        return 0;
    }
    if (xDist + p_stGlobals->stRadiusOfCurrentBoundingSphere < p_stLight->xNear) {
        ulResult = 3UL;
    }
    return ulResult;
}

void __cdecl GLI_pc_fn_vAccumulateLightColor(struct GLI_tdstLight_ *p_stLight, void *p_vUnused, float *p_xAccum)
{
    unsigned char ucMode = p_stLight->ucIsAlpha;

    if (ucMode == 1) {
        p_xAccum[3] = p_stLight->stColor.xA + p_xAccum[3];
        return;
    }
    p_xAccum[0] = p_stLight->stColor.xR + p_xAccum[0];
    p_xAccum[1] = p_stLight->stColor.xG + p_xAccum[1];
    p_xAccum[2] = p_stLight->stColor.xB + p_xAccum[2];
    if (p_stLight->ucIsAlpha == 0) {
        p_xAccum[3] = p_stLight->stColor.xA + p_xAccum[3];
    }
}

void __cdecl GLI_pc_fn_vOffsetVerticesByLightProjection(struct GLI_tdstLight_ *p_stLight, GEO_tdstGeometricObject *p_stVtxBlock,
                                                        float *p_xOut, GLI_tdstInternalGlobalValuesFor3dEngine *p_stGlobals)
{
    float axProj[2500];
    MTH3D_tdstMatrix *p_stMat;
    MTH3D_tdstVector *p_stIn;
    float *p_xWrite;
    float *p_xEnd;
    float xL3C;
    float xL48;
    float xL54;
    short wCount;
    int   iVtx;
    unsigned char ucMode;

    p_stMat = &p_stGlobals->gs_st_CurrentMatrix->stRotationMatrix;
    xL3C = p_stLight->stMatrix.stRotationMatrix.stCol_0.xZ;
    xL48 = p_stLight->stMatrix.stRotationMatrix.stCol_1.xZ;
    xL54 = p_stLight->stMatrix.stRotationMatrix.stCol_2.xZ;

    wCount = p_stVtxBlock->xNbPoints;
    p_stIn = p_stVtxBlock->d_stListOfPointsNormals;
    for (iVtx = 0; iVtx < (int)wCount; iVtx++) {
        axProj[iVtx] = (xL54 * p_stMat->stCol_0.xZ + xL48 * p_stMat->stCol_0.xY + xL3C * p_stMat->stCol_0.xX) * p_stIn->xX;
        axProj[iVtx] = (xL54 * p_stMat->stCol_1.xZ + xL48 * p_stMat->stCol_1.xY + xL3C * p_stMat->stCol_1.xX) * p_stIn->xY
                       + axProj[iVtx];
        axProj[iVtx] = (xL54 * p_stMat->stCol_2.xZ + xL48 * p_stMat->stCol_2.xY + xL3C * p_stMat->stCol_2.xX) * p_stIn->xZ
                       + axProj[iVtx];
        p_stIn++;
    }

    ucMode = p_stLight->ucIsAlpha;
    p_xWrite = p_xOut;
    p_xEnd = p_xOut + (int)wCount * 4;
    iVtx = 0;
    if (ucMode == 0) {
        while (p_xWrite < p_xEnd) {
            if ((*(unsigned long *)&axProj[iVtx]) & 0x80000000UL) {
                p_xWrite[0] = p_xWrite[0] - p_stLight->stColor.xR * axProj[iVtx];
                p_xWrite[1] = p_xWrite[1] - p_stLight->stColor.xG * axProj[iVtx];
                p_xWrite[2] = p_xWrite[2] - p_stLight->stColor.xB * axProj[iVtx];
                p_xWrite[3] = p_xWrite[3] - p_stLight->stColor.xA * axProj[iVtx];
            }
            p_xWrite += 4;
            iVtx++;
        }
    } else if (ucMode == 1) {
        while (p_xWrite < p_xEnd) {
            if ((*(unsigned long *)&axProj[iVtx]) & 0x80000000UL) {
                p_xWrite[3] = p_xWrite[3] - p_stLight->stColor.xA * axProj[iVtx];
            }
            p_xWrite += 4;
            iVtx++;
        }
    } else {
        while (p_xWrite < p_xEnd) {
            if ((*(unsigned long *)&axProj[iVtx]) & 0x80000000UL) {
                p_xWrite[0] = p_xWrite[0] - p_stLight->stColor.xR * axProj[iVtx];
                p_xWrite[1] = p_xWrite[1] - p_stLight->stColor.xG * axProj[iVtx];
                p_xWrite[2] = p_xWrite[2] - p_stLight->stColor.xB * axProj[iVtx];
            }
            p_xWrite += 4;
            iVtx++;
        }
    }
}

float __cdecl GLI_pc_fn_xLightRampFactor(float xValue, float xOuterLo, float xInnerLo,
                                         float xInnerHi, float xOuterHi)
{
    float xDenom;
    float xRes = MTH_C_ONE;

    if ((xValue < xOuterLo) || (xValue > xOuterHi)) {
        return 0.0f;
    }
    if (xValue < xInnerLo) {
        xDenom = xInnerLo - xOuterLo;
        if (MTH_M_bDifferentZero(xDenom)) {
            xRes = (xValue - xOuterLo) * GLI_pc_xFastRecip(xDenom);
        }
    } else if (xValue > xInnerHi) {
        xDenom = xInnerHi - xOuterHi;
        if (MTH_M_bDifferentZero(xDenom)) {
            xRes = (xValue - xOuterHi) * GLI_pc_xFastRecip(xDenom);
        }
    }
    return xRes;
}

float __cdecl GLI_pc_fn_xLightBoxAttenuation(struct GLI_tdstLight_ *p_stLight, MTH3D_tdstVector *p_stPos,
                                             MTH3D_tdstVector *p_stRef)
{
    float xSgnA;
    float xSgnB;
    float xF1;
    float xF2;
    float xF3;
    float xRes;

    if (p_stLight->bIsPainting == 0) {
        /* 0x4287D9 */
        xSgnA = MTH_M_xSign(p_stPos->xZ);
        xSgnB = MTH_M_xSign(p_stRef->xZ);
        if (xSgnA == xSgnB) {
            return 0.0f;
        }
    }

    xF1 = GLI_pc_fn_xLightRampFactor(p_stPos->xX, p_stLight->xExter_Min_Pos.xX, p_stLight->xInter_Min_Pos.xX,
                                  p_stLight->xInter_Max_Pos.xX, p_stLight->xExter_Max_Pos.xX);
    if (xF1 == 0.0f) {
        return 0.0f;
    }
    xF2 = GLI_pc_fn_xLightRampFactor(p_stPos->xY, p_stLight->xExter_Min_Pos.xY, p_stLight->xInter_Min_Pos.xY,
                                  p_stLight->xInter_Max_Pos.xY, p_stLight->xExter_Max_Pos.xY);
    if (xF2 == 0.0f) {
        return 0.0f;
    }
    xF3 = GLI_pc_fn_xLightRampFactor(p_stPos->xZ, p_stLight->xExter_Min_Pos.xZ, p_stLight->xInter_Min_Pos.xZ,
                                  p_stLight->xInter_Max_Pos.xZ, p_stLight->xExter_Max_Pos.xZ);
    if (xF3 == 0.0f) {
        return 0.0f;
    }
    if (xF2 < xF1) {
        xF2 = xF1;
    }
    xRes = ((1.0f - xF3) * p_stLight->xIntensity_Min
            + (p_stLight->xIntensity_Max * xF2
               + (1.0f - xF2) * p_stLight->xIntensity_Min) * xF3);
    return xRes;
}

void __cdecl GLI_pc_fn_vAccumulateLightAttenuatedColor(struct GLI_tdstLight_ *p_stLight, GEO_tdstGeometricObject *p_stObj,
                                                       float *p_xAccum, GLI_tdstInternalGlobalValuesFor3dEngine *p_stEngine)
{
    POS_tdstCompletePosition stCombined;
    MTH3D_tdstVector         stOrigin;
    float                    xNear2;
    float                    xFar2;
    float                    xInvSpan;
    float                    xMaxScale;
    float                    xDx;
    float                    xDy;
    float                    xDz;
    float                    xDist2;
    float                    xFactor;
    GLI_tdstAligned3DVector *p_stVtx;
    GLI_tdstAligned3DVector *p_stVtxEnd;

    xNear2 = p_stLight->xNear * p_stLight->xNear;
    xFar2  = p_stLight->xFar * p_stLight->xFar;
    if (xNear2 == xFar2) {
        xInvSpan = 1.0f;
    } else {
        xInvSpan = 1.0f / (xFar2 - xNear2);
    }
    xMaxScale = POS_fn_xGetMaxScale(p_stEngine->gs_st_CurrentMatrix);
    POS_fn_vMulMatrixMatrix(&stCombined, p_stEngine->gs_st_CameraMatrix, &p_stLight->stMatrix);
    POS_fn_vGetTranslationVector(&stCombined, &stOrigin);

    p_stVtx    = p_stEngine->GLI_TurnedScaledDisplecedPoint;
    p_stVtxEnd = p_stVtx + p_stObj->xNbPoints;
    while (p_stVtxEnd > p_stVtx) {
        xDx = stOrigin.xX - p_stVtx->xX;
        xDy = stOrigin.xY - p_stVtx->xY;
        xDz = stOrigin.xZ - p_stVtx->xZ;
        xDist2 = xDx * xDx;
        if ((xDist2 < xFar2) && ((xDist2 = xDy * xDy + xDist2) < xFar2)) {
            xDist2 = xDz * xDz + xDist2;
            if (xNear2 <= xDist2) {
                if (xFar2 <= xDist2) {
                    p_stVtx++;
                    p_xAccum += 4;
                    continue;
                }
                xFactor = (xFar2 - xDist2) * xMaxScale * xInvSpan;
            } else {
                xFactor = xMaxScale;
                if (xDist2 == 0.0f) {
                    xFactor = 1.0f;
                }
            }
            if (p_stLight->ucIsAlpha != 1) {
                p_xAccum[0] = p_stLight->stColor.xR * xFactor + p_xAccum[0];
                p_xAccum[1] = p_stLight->stColor.xG * xFactor + p_xAccum[1];
                p_xAccum[2] = p_stLight->stColor.xB * xFactor + p_xAccum[2];
                if (p_stLight->ucIsAlpha != 0) {
                    p_stVtx++;
                    p_xAccum += 4;
                    continue;
                }
            }
            p_xAccum[3] = p_stLight->stColor.xA * xFactor + p_xAccum[3];
        }
        p_stVtx++;
        p_xAccum += 4;
    }
}

float        *GLI_pc_g_pxLightColour;                 /* 0x004A729C */
float        *GLI_pc_g_pxAccumCursor;                 /* 0x004A72A0 */
float         GLI_pc_g_xLightFactor;                  /* 0x004A72A4 */
void        (*GLI_pc_g_pfnBlendLightColour)(void);    /* 0x004A7288 */
MTH3D_tdstVector GLI_pc_g_stLocalOrigin;              /* 0x004A7290 */

#include "MTH/MTH_fopt.h"
#define GLI_pc_xFastInvSqrt  MTH_fn_fInvSquareRootOpt

void __cdecl GLI_pc_fn_vAccumulateLightNormalGated(struct GLI_tdstLight_ *p_stLight, GEO_tdstGeometricObject *p_stVtxBlk,
                                                   unsigned long ulUnused, GLI_tdstInternalGlobalValuesFor3dEngine *p_stEngine)
{

    float  xNear2;
    float  xFar2;
    float  xInvSpan;
    MTH3D_tdstVector stDiff;
    MTH3D_tdstVector *p_xVtx;
    MTH3D_tdstVector *p_xNrm;
    MTH3D_tdstVector *p_xVtxEnd;
    float  xRsq;
    float  xSum;
    float  xInv;

    xNear2 = p_stLight->xNear * p_stLight->xNear;
    xFar2  = p_stLight->xFar * p_stLight->xFar;
    if (xNear2 != xFar2) {
        xInvSpan = 1.0f / (xFar2 - xNear2);
    } else {
        xInvSpan = 1.0f;
    }
    p_xVtx    = p_stVtxBlk->d_stListOfPoints;
    p_xVtxEnd = p_xVtx + p_stVtxBlk->xNbPoints;
    p_xNrm    = p_stVtxBlk->d_stListOfPointsNormals;

    while (p_xVtx < p_xVtxEnd) {
        MTH3D_M_vSubVector(&stDiff, p_xVtx, &GLI_pc_g_stLocalOrigin);
        if (((xSum = stDiff.xX * stDiff.xX) < xFar2)
            && ((xSum = stDiff.xY * stDiff.xY + xSum) < xFar2)) {
            xRsq = stDiff.xZ * stDiff.xZ + xSum;
            if (xRsq < xNear2) {
                GLI_pc_g_xLightFactor = MTH3D_M_xDotProductVector(p_xNrm, &stDiff);
                if (GLI_pc_g_xLightFactor < 0.0) {
                    xInv = GLI_pc_xFastInvSqrt(xRsq);
                    GLI_pc_g_xLightFactor = -(xInv * GLI_pc_g_xLightFactor);
                    (*GLI_pc_g_pfnBlendLightColour)();
                }
            } else if (xRsq < xFar2) {
                GLI_pc_g_xLightFactor = MTH3D_M_xDotProductVector(p_xNrm, &stDiff);
                if (GLI_pc_g_xLightFactor < 0.0) {
                    xInv = GLI_pc_xFastInvSqrt(xRsq);
                    GLI_pc_g_xLightFactor = (xRsq - xFar2) * xInv * xInvSpan * GLI_pc_g_xLightFactor;
                    (*GLI_pc_g_pfnBlendLightColour)();
                }
            }
        }
        p_xVtx++;
        GLI_pc_g_pxAccumCursor += 4;
        p_xNrm++;
    }
}

void __cdecl GLI_pc_fn_vAccumulateLightScaledNormalGated(struct GLI_tdstLight_ *p_stLight, GEO_tdstGeometricObject *p_stVtxBlk,
                                                         unsigned long ulUnused, GLI_tdstInternalGlobalValuesFor3dEngine *p_stEngine)
{
    POS_tdstCompletePosition stCombined;
    MTH3D_tdstVector         stOrigin;
    float                    xMaxScale;
    float                    xScaleDev;
    float                    xNear2;
    float                    xFar2;
    float                    xScale;
    MTH3D_tdstVector         stDiff;
    float                    xRsq;
    float                    xInv;
    MTH3D_tdstVector        *p_xNrm;
    GLI_tdstAligned3DVector *p_xEng;
    GLI_tdstAligned3DVector *p_xEngEnd;
    MTH3D_tdstVector        *p_xVtx;

    (void)ulUnused;
    xMaxScale = POS_fn_xGetMaxScale(p_stEngine->gs_st_CurrentMatrix);
    xScaleDev = MTH_M_xAbs(xMaxScale - 1.0f);
    if (xScaleDev < 0.05) {   /* 0x49C5F8 */
        GLI_pc_fn_vAccumulateLightNormalGated(p_stLight, p_stVtxBlk, ulUnused, p_stEngine);
        return;
    }
    {
        xNear2 = p_stLight->xNear * p_stLight->xNear;
        xFar2  = p_stLight->xFar * p_stLight->xFar;
        if (xNear2 != xFar2) {
            xScale = xMaxScale / (xFar2 - xNear2);
        } else {
            xScale = xMaxScale;
        }
        POS_fn_vMulMatrixMatrix(&stCombined, p_stEngine->gs_st_CameraMatrix, &p_stLight->stMatrix);
        POS_fn_vGetTranslationVector(&stCombined, &stOrigin);

        p_xNrm    = p_stVtxBlk->d_stListOfPointsNormals;
        p_xEng    = p_stEngine->GLI_TurnedScaledDisplecedPoint;
        p_xEngEnd = p_xEng + p_stVtxBlk->xNbPoints;
        p_xVtx    = p_stVtxBlk->d_stListOfPoints;

        while (p_xEng < p_xEngEnd) {

            MTH3D_M_vSubVector(&stDiff, &stOrigin, (MTH3D_tdstVector *)p_xEng);
            if (((xRsq = stDiff.xX * stDiff.xX) < xFar2)
                && ((xRsq = stDiff.xY * stDiff.xY + xRsq) < xFar2)) {
                xRsq = stDiff.xZ * stDiff.xZ + xRsq;
                if (xNear2 <= xRsq) {
                    if (xRsq < xFar2) {
                        MTH3D_M_vSubVector(&stDiff, p_xVtx, &GLI_pc_g_stLocalOrigin);
                        GLI_pc_g_xLightFactor = MTH3D_M_xDotProductVector(p_xNrm, &stDiff);
                        if (GLI_pc_g_xLightFactor < 0.0) {
                            xInv = GLI_pc_xFastInvSqrt(xRsq);
                            GLI_pc_g_xLightFactor = (xRsq - xFar2) * xInv * xScale * GLI_pc_g_xLightFactor;
                            (*GLI_pc_g_pfnBlendLightColour)();
                        }
                    }
                } else if (xRsq == 0.0f) {
                    GLI_pc_g_xLightFactor = 1.0f;
                    (*GLI_pc_g_pfnBlendLightColour)();
                } else {
                    MTH3D_M_vSubVector(&stDiff, p_xVtx, &GLI_pc_g_stLocalOrigin);
                    GLI_pc_g_xLightFactor = MTH3D_M_xDotProductVector(p_xNrm, &stDiff);
                    if (GLI_pc_g_xLightFactor < 0.0) {
                        xInv = GLI_pc_xFastInvSqrt(xRsq);
                        GLI_pc_g_xLightFactor = -(xInv * xMaxScale * GLI_pc_g_xLightFactor);
                        (*GLI_pc_g_pfnBlendLightColour)();
                    }
                }
            }
            p_xEng++;
            GLI_pc_g_pxAccumCursor += 4;
            p_xVtx++;
            p_xNrm++;
        }
    }
}

void __cdecl GLI_pc_fn_vBlendLightColorRGBA(void);

void __cdecl GLI_pc_fn_vBlendLightColorRGB(void);

void __cdecl GLI_pc_fn_vBlendLightAlpha(void);

void __cdecl GLI_pc_fn_vDispatchLightAccumulation(struct GLI_tdstLight_ *p_stLight, GEO_tdstGeometricObject *p_stVtxBlk,
                                                  float *p_xOut, GLI_tdstInternalGlobalValuesFor3dEngine *p_stEngine)
{
    unsigned char ucColourMode;

    if (GLI_pc_fn_ulClassifyLightVsSphere(p_stLight, p_stEngine) != 0) {
        if (p_stLight->bIsPainting != 0) {
            GLI_pc_fn_vAccumulateLightAttenuatedColor(p_stLight, p_stVtxBlk, p_xOut, p_stEngine);
            return;
        }
        ucColourMode = p_stLight->ucIsAlpha;
        if (ucColourMode == 1) {
            GLI_pc_g_pfnBlendLightColour = GLI_pc_fn_vBlendLightAlpha;
        } else {
            GLI_pc_g_pfnBlendLightColour = GLI_pc_fn_vBlendLightColorRGB;
            if (ucColourMode != 2) {
                GLI_pc_g_pfnBlendLightColour = GLI_pc_fn_vBlendLightColorRGBA;
            }
        }
        GLI_pc_g_pxAccumCursor = p_xOut;
        GLI_pc_g_pxLightColour = &p_stLight->stColor.xR;

        if ((p_stEngine->lClippingModeMask & 0x800L) == 0) {
            POS_fn_vInvertMatrix(&p_stEngine->gs_st_CurrentMatrixInverted, p_stEngine->gs_st_CurrentMatrix);
            p_stEngine->lClippingModeMask = p_stEngine->lClippingModeMask | 0x800L;
        }

        MTH3D_M_vMulMatrixVector(&GLI_pc_g_stLocalOrigin,
                                 &p_stEngine->gs_st_CurrentMatrixInverted.stTransformMatrix,
                                 &p_stLight->stMatrix.stTranslationVector);
        MTH3D_M_vAddVector(&GLI_pc_g_stLocalOrigin,
                           &p_stEngine->gs_st_CurrentMatrixInverted.stTranslationVector,
                           &GLI_pc_g_stLocalOrigin);
        GLI_pc_fn_vAccumulateLightScaledNormalGated(p_stLight, p_stVtxBlk, (unsigned long)p_xOut, p_stEngine);
    }
}

/* 0x004A729C */
void __cdecl GLI_pc_fn_vBlendLightColorRGBA(void)
{
    GLI_pc_g_pxAccumCursor[0] = GLI_pc_g_pxLightColour[0] * GLI_pc_g_xLightFactor + GLI_pc_g_pxAccumCursor[0];
    GLI_pc_g_pxAccumCursor[1] = GLI_pc_g_pxLightColour[1] * GLI_pc_g_xLightFactor + GLI_pc_g_pxAccumCursor[1];
    GLI_pc_g_pxAccumCursor[2] = GLI_pc_g_pxLightColour[2] * GLI_pc_g_xLightFactor + GLI_pc_g_pxAccumCursor[2];
    GLI_pc_g_pxAccumCursor[3] = GLI_pc_g_pxLightColour[3] * GLI_pc_g_xLightFactor + GLI_pc_g_pxAccumCursor[3];
}

void __cdecl GLI_pc_fn_vBlendLightColorRGB(void)
{
    GLI_pc_g_pxAccumCursor[0] = GLI_pc_g_pxLightColour[0] * GLI_pc_g_xLightFactor + GLI_pc_g_pxAccumCursor[0];
    GLI_pc_g_pxAccumCursor[1] = GLI_pc_g_pxLightColour[1] * GLI_pc_g_xLightFactor + GLI_pc_g_pxAccumCursor[1];
    GLI_pc_g_pxAccumCursor[2] = GLI_pc_g_pxLightColour[2] * GLI_pc_g_xLightFactor + GLI_pc_g_pxAccumCursor[2];
}

void __cdecl GLI_pc_fn_vBlendLightAlpha(void)
{
    float *p_xColour = GLI_pc_g_pxLightColour;
    float *p_xAccum  = GLI_pc_g_pxAccumCursor;
    p_xAccum[3] = p_xColour[3] * GLI_pc_g_xLightFactor + p_xAccum[3];
}

unsigned long __fastcall GLI_pc_fn_ulTestLightRangeVsSphere(struct GLI_tdstLight_ *p_stLight,
                                                           GLI_tdstInternalGlobalValuesFor3dEngine *p_stGlobals)
{
    MTH3D_tdstVector stTrs;
    float            xDist;

    if (p_stGlobals->stRadiusOfCurrentBoundingSphere == 0.0f) {
        return 1UL;
    }
    POS_fn_vGetTranslationVector(&p_stLight->stMatrix, &stTrs);
    /* 0x427B5B */
    MTH3D_M_vSubVector(&stTrs, &stTrs, &p_stGlobals->stCenterOfCurrentBoundingSphere);
    xDist = MTH_fn_fSquareRootOpt(MTH3D_M_xSqrVector(&stTrs));

    if (xDist < p_stLight->xFar + p_stGlobals->stRadiusOfCurrentBoundingSphere) {
        return 1UL;
    }
    return 0UL;
}

void __cdecl GLI_pc_fn_vDotVertexArrayWithVector(long lNbOfValues, GLI_tdstAligned3DVector *p_stSrc,
                                                 float *p_xDst, float *p_xVec)
{
    float *p_xEnd = p_xDst + lNbOfValues;
    if (p_xDst < p_xEnd) {
        do {
            *p_xDst++ = p_stSrc->xY * p_xVec[1] + p_stSrc->xX * p_xVec[0] + p_stSrc->xZ * p_xVec[2];
            p_stSrc++;
        } while (p_xDst < p_xEnd);
    }
}

void __cdecl GLI_pc_fn_vAccumulateLightBoxAttenuatedVertices(struct GLI_tdstLight_ *p_stLight, GEO_tdstGeometricObject *p_stVtxBlk,
                                                             float *p_xOut, GLI_tdstInternalGlobalValuesFor3dEngine *p_stEngine)
{
    POS_tdstCompletePosition stLightEng;
    POS_tdstCompletePosition stLightEngInv;
    POS_tdstCompletePosition stLightInv;
    POS_tdstCompletePosition stEngLightInv;
    MTH3D_tdstVector         stLightPos;
    MTH3D_tdstVector         stCentre;
    MTH3D_tdstVector         stVtxEng;
    MTH3D_tdstVector         stVtxLocal;
    MTH3D_tdstVector         stNrmLocal;
    float                    xDiff[3];
    float                    xR2;
    float                    xRsq;
    float                    xAtt;
    float                    xW;
    float                    xInvAbs;
    unsigned long            ulAbsBits;
    long                     lInit;
    long                     lVtx;
    long                     lCount;
    GLI_tdstAligned3DVector *p_xEng;
    MTH3D_tdstVector        *p_xNrm;

    POS_fn_vMulMatrixMatrix(&stLightEng, p_stEngine->gs_st_CameraMatrix, &p_stLight->stMatrix);
    POS_fn_vMulMatrixVertex(&stLightPos, &stLightEng, &p_stLight->xCenterBox);
    POS_fn_vMulMatrixVertex(&stCentre, p_stEngine->gs_st_CameraMatrix,
                            &p_stEngine->stCenterOfCurrentBoundingSphere);
    POS_fn_xGetMaxScale(p_stEngine->gs_st_CurrentMatrix);
    xR2 = p_stLight->xRadius * p_stLight->xRadius;

    lInit = 0;

    if ((*(unsigned long *)&p_stEngine->stRadiusOfCurrentBoundingSphere & 0x7FFFFFFFUL) != 0) {
        xDiff[0] = stLightPos.xX - stCentre.xX;
        xDiff[1] = stLightPos.xY - stCentre.xY;
        xDiff[2] = stLightPos.xZ - stCentre.xZ;
        xRsq = xDiff[1] * xDiff[1] + xDiff[0] * xDiff[0];
        xRsq = xDiff[2] * xDiff[2] + xRsq;
        if (MTH_fn_fSquareRootOpt(xRsq) < p_stEngine->stRadiusOfCurrentBoundingSphere + p_stLight->xRadius) {
            return;
        }
    }

    lVtx = 0;
    lCount = p_stVtxBlk->xNbPoints;
    if (lCount > 0) {
        p_xEng = p_stEngine->GLI_TurnedScaledDisplecedPoint;
        p_xNrm = p_stVtxBlk->d_stListOfPointsNormals;
        do {
            xDiff[0] = stLightPos.xX - p_xEng->xX;
            xDiff[1] = stLightPos.xY - p_xEng->xY;
            xDiff[2] = stLightPos.xZ - p_xEng->xZ;
            if ((xDiff[0] * xDiff[0] < xR2)
                && ((xRsq = xDiff[1] * xDiff[1] + xDiff[0] * xDiff[0]) < xR2)
                && ((xDiff[2] * xDiff[2] + xRsq) < xR2)) {
                if (lInit == 0) {
                    POS_fn_vInvertMatrix(&stLightEngInv, &stLightEng);
                    POS_fn_vInvertMatrix(&stLightInv, &p_stLight->stMatrix);
                    POS_fn_vMulMatrixMatrix(&stEngLightInv, p_stEngine->gs_st_CurrentMatrix, &stLightInv);
                    lInit = 1;
                }
                stVtxEng.xX = p_xEng->xX;
                stVtxEng.xY = p_xEng->xY;
                stVtxEng.xZ = p_xEng->xZ;
                POS_fn_vMulMatrixVertex(&stVtxLocal, &stLightEngInv, &stVtxEng);
                POS_fn_vMulMatrixVector(&stNrmLocal, &stEngLightInv, p_xNrm);
                xAtt = GLI_pc_fn_xLightBoxAttenuation(p_stLight, (MTH3D_tdstVector *)&stVtxLocal,
                                                      (MTH3D_tdstVector *)&stNrmLocal);
                if (p_stLight->bIsPainting == 0) {
                    ulAbsBits = *(unsigned long *)&stNrmLocal.xZ & 0x7FFFFFFFUL;
                    xInvAbs = *(float *)&ulAbsBits;
                    xW = xInvAbs * xAtt;
                } else {
                    xW = xAtt;
                }
                if (*(unsigned long *)&xW - 1UL < 0x7FFFFFFFUL) {
                    p_xOut[0] = p_stLight->stColor.xR * xW + p_xOut[0];
                    p_xOut[1] = p_stLight->stColor.xG * xW + p_xOut[1];
                    p_xOut[2] = p_stLight->stColor.xB * xW + p_xOut[2];
                    p_xOut[3] = p_stLight->stColor.xA * xW + p_xOut[3];
                }
            }
            lVtx++;
            p_xEng++;
            p_xOut += 4;
            p_xNrm++;
        } while (lVtx < lCount);
    }
}

void __cdecl GLI_pc_fn_vAccumulateLightPointVertices(struct GLI_tdstLight_ *p_stLight, GEO_tdstGeometricObject *p_stVtxBlk,
                                                     float *p_xOut, GLI_tdstInternalGlobalValuesFor3dEngine *p_stEngine)
{
    float axDepth[2500];
    POS_tdstCompletePosition stMat;
    float axP[3];
    float axQ[3];
    float axR[3];
    float axS[3];
    float axT[3];
    GLI_tdstAligned3DVector *p_xVtx;
    MTH3D_tdstVector *p_xNrmBase;
    float *p_xCell;
    float xFar;
    float xNear;
    float xFarSq;
    float xNearSq;
    float xInvFarNear;
    float xInner;
    float xOuter;
    float xInvRange;
    float xVR;
    float xVQ;
    float xVS;
    float xPP;
    float xSqrtPP;
    int   bGrid;
    short wCount;
    int   iVtx;
    int   lW;
    int   lH;

    POS_fn_vInvertIsoMatrix(&stMat, p_stEngine->gs_st_CurrentMatrix);
    POS_fn_vMulMatrixMatrix(&stMat, &stMat, &p_stLight->stMatrix);
    POS_fn_vGetRotationMatrix(&stMat, (MTH3D_tdstVector *)axP, (MTH3D_tdstVector *)axQ,
                              (MTH3D_tdstVector *)axR);
    POS_fn_vMulMatrixMatrix(&stMat, p_stEngine->gs_st_CameraMatrix, &p_stLight->stMatrix);
    POS_fn_vGetTranslationVector(&stMat, (MTH3D_tdstVector *)axT);
    POS_fn_vGetRotationMatrix(&stMat, (MTH3D_tdstVector *)axR, (MTH3D_tdstVector *)axQ,
                              (MTH3D_tdstVector *)axS);

    xVR = axR[0] * axT[0] + axR[1] * axT[1] + axR[2] * axT[2];
    xVQ = axQ[0] * axT[0] + axQ[1] * axT[1] + axQ[2] * axT[2];
    xVS = axS[0] * axT[0] + axS[1] * axT[1] + axS[2] * axT[2];

    wCount = p_stVtxBlk->xNbPoints;
    GLI_pc_fn_vDotVertexArrayWithVector((long)wCount, p_stEngine->GLI_TurnedScaledDisplecedPoint,
                                        axDepth, axR);

    xFar = p_stLight->xFar;
    xNear = p_stLight->xNear;
    xFarSq = xFar * xFar;
    xNearSq = xNear * xNear;
    if (xFar - xNear == 0.0f) {
        xInvFarNear = 1.0f;
    } else {
        xInvFarNear = 1.0f / (xFarSq - xNearSq);
    }
    xInner = p_stLight->xLittleTangent;
    xOuter = p_stLight->xBigTangent;
    if (xOuter - xInner == 0.0f) {
        xInvRange = 1.0f;
    } else {
        xInvRange = 1.0f / (xOuter - xInner);
    }

    xPP = axP[0] * axP[0] + axP[1] * axP[1] + axP[2] * axP[2];
    xSqrtPP = MTH_fn_fSquareRootOpt(xPP);

    bGrid = (p_stLight->stZBuffer.p_xZBufferMap != NULL);
    p_xVtx = p_stEngine->GLI_TurnedScaledDisplecedPoint;
    p_xNrmBase = p_stVtxBlk->d_stListOfPointsNormals;
    p_xCell = p_stLight->stZBuffer.p_xMiddleZBufferMap;
    lW = p_stLight->stZBuffer.lSizeX;
    lH = p_stLight->stZBuffer.lSizeY;

    for (iVtx = 0; iVtx < (int)wCount; iVtx++) {
        float *p_xV = &p_xVtx[iVtx].xX;
        float *p_xN = &p_xNrmBase[iVtx].xX;
        float *p_xO = p_xOut + 4 * iVtx;
        float xDx;
        float xDy;
        float xDz;
        float xSq;
        float xSum;
        float xRatio;
        float xDist2;
        float xAtt;
        float xW;
        float xWgt;
        float xInv;
        float xAbs;
        int   lCellX;
        int   lCellY;
        int   lHalf;
        unsigned char ucMode;

        xDx = xVR - axDepth[iVtx];
        axDepth[iVtx] = 0.0f;
        if (!((xDx < xFar) && (0.0f < xDx))) {
            continue;
        }
        xDy = xVQ - (p_xV[0] * axQ[0] + p_xV[1] * axQ[1] + p_xV[2] * axQ[2]);
        if (!(xDy < xFar)) {
            continue;
        }
        xDz = xVS - (p_xV[0] * axS[0] + p_xV[1] * axS[1] + p_xV[2] * axS[2]);
        if (!((-xFar < xDy) && (xDz < xFar) && (-xFar < xDz))) {
            continue;
        }
        xSq = xDx * xDx;
        if (xSq == 0.0f) {
            continue;
        }
        xSum = xDz * xDz + xDy * xDy;
        xRatio = GLI_pc_xFastRecip(xSq) * xSum;
        if (!(xRatio < xOuter)) {
            continue;
        }
        xDist2 = xSq + xSum;
        if (!(xDist2 <= xFarSq)) {
            continue;
        }
        if (xNearSq <= xDist2) {
            xAtt = -((xDist2 - xFarSq) * xInvFarNear);
        } else {
            xAtt = 1.0f;
        }
        if (xInner < xRatio) {
            xAtt = -((xRatio - xOuter) * xInvRange * xAtt);
        }

        if (p_stLight->bIsPainting == 0) {
            if (xDist2 == 0.0f) {
                xW = 1.0f;
            } else {
                xW = (p_xN[1] * axP[1] + p_xN[0] * axP[0] + p_xN[2] * axP[2] + 1.0f) * 0.5f;
            }
        } else if (xDist2 == 0.0f) {
            xW = 1.0f;
        } else {
            xAbs = (xDist2 < 0.0f) ? -xDist2 : xDist2;
            xW = xSqrtPP * xAbs;
        }
        if (!(0.0f < xW)) {
            continue;
        }

        if (bGrid) {
            xInv = GLI_pc_xFastRecip(xDx);
            lCellX = (long)(p_stLight->stZBuffer.xCoefX * xDy * xInv);
            lCellY = (long)(p_stLight->stZBuffer.xCoefY * xDz * xInv);
            lHalf = lW >> 1;
            if (!((-lHalf < lCellX) && (lCellX < lHalf))) {
                continue;
            }
            lHalf = lH >> 1;
            if (!((-lHalf < lCellY) && (lCellY < lHalf))) {
                continue;
            }
            if (!(p_xCell[lCellY * lW + lCellX] <= xInv)) {
                continue;
            }
            xWgt = xW * (xW * xAtt);
        } else {
            xWgt = xW * xAtt;
        }

        axDepth[iVtx] = xDist2;
        ucMode = p_stLight->ucIsAlpha;
        if (ucMode != 1) {
            p_xO[0] = p_stLight->stColor.xR * xWgt + p_xO[0];
            p_xO[1] = p_stLight->stColor.xG * xWgt + p_xO[1];
            p_xO[2] = p_stLight->stColor.xB * xWgt + p_xO[2];
        }
        if (ucMode == 0 || ucMode == 1) {
            p_xO[3] = p_stLight->stColor.xA * xWgt + p_xO[3];
        }
    }
}

/* 0x00503C20 */
extern unsigned char GLI_s_aucActiveLightForChar_00503C20[300];
/* 0x004239F0 */
extern void __cdecl GLI_pc_fn_vAddWaterDepthColorToVertices(GLI_tdstInternalGlobalValuesFor3dEngine *p_stGlobals,
                                                            long lFirstVertex,
                                                            long lNbVertices);

void __cdecl GLI_vSendObjectToLights(struct GLI_tdstLight_ **p_pstLights, GEO_tdstGeometricObject *p_stVtxBlk,
                                     float *p_xOut, void *p_vUnused,
                                     GEO_tdstColor *p_stColorRLIA, int lCount, GLI_tdstInternalGlobalValuesFor3dEngine *p_stEngine)
{
    int lLight;

    p_stColorRLIA->xR = 0.0f;
    p_stColorRLIA->xG = 0.0f;
    p_stColorRLIA->xB = 0.0f;
    p_stColorRLIA->xA = 0.0f;

    for (lLight = 0; lLight < lCount; lLight++) {
        struct GLI_tdstLight_ *p_stLight = p_pstLights[lLight];
        long lType;

        if (p_stLight == 0) {
            continue;
        }
        if (p_stLight->bValid == 0) {
            continue;
        }
        if (GLI_s_aucActiveLightForChar_00503C20[lLight] == 0) {
            continue;
        }
        lType = p_stLight->lTypeOfLight;
        if (lType == 4) {
            GLI_pc_fn_vAccumulateLightColor(p_stLight, p_stVtxBlk, &p_stColorRLIA->xR);
        } else if (lType == 2) {
            GLI_pc_fn_vDispatchLightAccumulation(p_stLight, p_stVtxBlk, p_xOut, p_stEngine);
        } else if (lType == 1) {
            GLI_pc_fn_vOffsetVerticesByLightProjection(p_stLight, p_stVtxBlk, p_xOut, p_stEngine);
        } else if (lType == 5) {
            GLI_pc_fn_vAccumulateLightBoxAttenuatedVertices(p_stLight, p_stVtxBlk, p_xOut, p_stEngine);
        } else if (lType == 3) {
            if (GLI_pc_fn_ulTestLightRangeVsSphere(p_stLight, p_stEngine) != 0) {
                GLI_pc_fn_vAccumulateLightPointVertices(p_stLight, p_stVtxBlk, p_xOut, p_stEngine);
            }
        }
    }
    GLI_pc_fn_vAddWaterDepthColorToVertices(p_stEngine, 0L, (long)p_stVtxBlk->xNbPoints);
}

#define GLI_pc_M_ulBits(f)        (*(unsigned long *)&(f))
#define GLI_pc_M_ulChan(f) \
    ((GLI_pc_M_ulBits(f) & 0x400000ul) == 0ul ? 0ul : \
     (((GLI_pc_M_ulBits(f) >> 8) & 0xfful) == 0ul ? (GLI_pc_M_ulBits(f) & 0xfful) : 0xfful))
#define GLI_pc_M_ulAlphaRaw(f) \
    ((GLI_pc_M_ulBits(f) & 0x400000ul) == 0ul ? 0ul : \
     (((GLI_pc_M_ulBits(f) >> 8) & 0xfful) == 0ul ? (GLI_pc_M_ulBits(f) << 24) : 0xff000000ul))

void __cdecl GLI_pc_fn_vPackVertexColoursIdx(GLI_tdstInternalGlobalValuesFor3dEngine *p_stThis, short sCount, short *psIdx)
{
    short        *psEnd;
    float         fBaseR;
    float         fBaseG;
    float         fBaseB;
    float         fBaseA;
    unsigned long ulAlpha;
    int           iOff;
    float         fR;
    float         fG;
    float         fB;

    psEnd  = psIdx + sCount;
    fBaseR = p_stThis->GLI_stColorsAdd.xR - -24576.0f;
    fBaseG = p_stThis->GLI_stColorsAdd.xG - -24576.0f;
    fBaseB = p_stThis->GLI_stColorsAdd.xB - -24576.0f;
    fBaseA = p_stThis->GLI_stColorsAdd.xA - -24576.0f;
    ulAlpha = GLI_pc_M_ulChan(fBaseA);

    for (; psIdx < psEnd; psIdx++) {
        iOff = (int)*psIdx;
        fR = p_stThis->GLI_aDEF_stColorsRLID[iOff].xR * p_stThis->GLI_stColorsMul.xR + fBaseR;
        fG = p_stThis->GLI_aDEF_stColorsRLID[iOff].xG * p_stThis->GLI_stColorsMul.xG + fBaseG;
        fB = p_stThis->GLI_aDEF_stColorsRLID[iOff].xB * p_stThis->GLI_stColorsMul.xB + fBaseB;
        p_stThis->GLI_ScreenPoint[iOff].ulPackedColor =
            (ulAlpha << 0x18) | GLI_pc_M_ulChan(fB) | (GLI_pc_M_ulChan(fG) << 0x8) | (GLI_pc_M_ulChan(fR) << 0x10);
    }
}

void __cdecl GLI_pc_fn_vPackVertexColoursAll(GLI_tdstInternalGlobalValuesFor3dEngine *p_stThis, short sCount)
{
    float         fBaseR;
    float         fBaseG;
    float         fBaseB;
    float         fBaseA;
    unsigned long ulAlpha;
    float         fR;
    float         fG;
    float         fB;
    GEO_tdstColor *p_stCol;
    GLI_tdstAligned2DVector *p_stOut;
    short         sLeft;

    fBaseR = p_stThis->GLI_stColorsAdd.xR - -24576.0f;
    fBaseG = p_stThis->GLI_stColorsAdd.xG - -24576.0f;
    fBaseB = p_stThis->GLI_stColorsAdd.xB - -24576.0f;
    fBaseA = p_stThis->GLI_stColorsAdd.xA - -24576.0f;
    ulAlpha = GLI_pc_M_ulChan(fBaseA);

    if (sCount != 0) {
        p_stCol = p_stThis->GLI_aDEF_stColorsRLID;
        p_stOut = p_stThis->GLI_ScreenPoint;
        sLeft  = sCount;
        do {
            fR = p_stCol->xR * p_stThis->GLI_stColorsMul.xR + fBaseR;
            fG = p_stCol->xG * p_stThis->GLI_stColorsMul.xG + fBaseG;
            fB = p_stCol->xB * p_stThis->GLI_stColorsMul.xB + fBaseB;
            p_stOut->ulPackedColor = (ulAlpha << 0x18) | GLI_pc_M_ulChan(fB) | (GLI_pc_M_ulChan(fG) << 0x8) | (GLI_pc_M_ulChan(fR) << 0x10);
            p_stCol++;
            p_stOut++;
            sLeft--;
        } while (sLeft != 0);
    }
}

void __cdecl GLI_pc_fn_vPackVertexColoursIdxRaw(GLI_tdstInternalGlobalValuesFor3dEngine *p_stThis, short sCount, short *psIdx)
{
    float         fBaseR;
    float         fBaseG;
    float         fBaseB;
    float         fBaseA;
    int           iOff;
    short         sLeft;
    unsigned long ulOut;
    float         fR;
    float         fG;
    float         fB;
    float         fA;

    fBaseR = p_stThis->GLI_stColorsAdd.xR - -24576.0f;
    fBaseG = p_stThis->GLI_stColorsAdd.xG - -24576.0f;
    fBaseB = p_stThis->GLI_stColorsAdd.xB - -24576.0f;
    fBaseA = p_stThis->GLI_stColorsAdd.xA - -24576.0f;

    if (sCount != 0) {
        sLeft = sCount;
        do {
            iOff = (int)*psIdx;
            fR = p_stThis->GLI_aDEF_stColorsRLID[iOff].xR * p_stThis->GLI_stColorsMul.xR + fBaseR;
            fG = p_stThis->GLI_aDEF_stColorsRLID[iOff].xG * p_stThis->GLI_stColorsMul.xG + fBaseG;
            fB = p_stThis->GLI_aDEF_stColorsRLID[iOff].xB * p_stThis->GLI_stColorsMul.xB + fBaseB;
            fA = p_stThis->GLI_aDEF_stColorsRLID[iOff].xA * p_stThis->GLI_stColorsMul.xA + fBaseA;
            ulOut = GLI_pc_M_ulChan(fR) << 0x10;
            ulOut = ulOut | (GLI_pc_M_ulChan(fG) << 0x8);
            ulOut = ulOut | GLI_pc_M_ulChan(fB);
            ulOut = ulOut | GLI_pc_M_ulAlphaRaw(fA);
            p_stThis->GLI_ScreenPoint[iOff].ulPackedColor = ulOut;
            psIdx++;
            sLeft--;
        } while (sLeft != 0);
    }
}

void __cdecl GLI_pc_fn_vPackVertexColoursAllRaw(GLI_tdstInternalGlobalValuesFor3dEngine *p_stThis, short sCount)
{
    float         fBaseR;
    float         fBaseG;
    float         fBaseB;
    float         fBaseA;
    GEO_tdstColor *p_stCol;
    GLI_tdstAligned2DVector *p_stOut;
    short         sLeft;
    float         fR;
    float         fG;
    float         fB;
    float         fA;

    fBaseR = p_stThis->GLI_stColorsAdd.xR - -24576.0f;
    fBaseG = p_stThis->GLI_stColorsAdd.xG - -24576.0f;
    fBaseB = p_stThis->GLI_stColorsAdd.xB - -24576.0f;
    fBaseA = p_stThis->GLI_stColorsAdd.xA - -24576.0f;

    if (sCount != 0) {
        p_stOut = p_stThis->GLI_ScreenPoint;
        p_stCol = p_stThis->GLI_aDEF_stColorsRLID;
        sLeft  = sCount;
        do {
            p_stOut->ulPackedColor = 0;
            fR = p_stCol->xR * p_stThis->GLI_stColorsMul.xR + fBaseR;
            fG = p_stCol->xG * p_stThis->GLI_stColorsMul.xG + fBaseG;
            fB = p_stCol->xB * p_stThis->GLI_stColorsMul.xB + fBaseB;
            fA = p_stCol->xA * p_stThis->GLI_stColorsMul.xA + fBaseA;
            p_stOut->ulPackedColor = GLI_pc_M_ulChan(fR) << 0x10;
            p_stOut->ulPackedColor = p_stOut->ulPackedColor | (GLI_pc_M_ulChan(fG) << 0x8);
            p_stOut->ulPackedColor = p_stOut->ulPackedColor | GLI_pc_M_ulChan(fB);
            p_stOut->ulPackedColor = p_stOut->ulPackedColor | GLI_pc_M_ulAlphaRaw(fA);
            p_stCol++;
            p_stOut++;
            sLeft--;
        } while (sLeft != 0);
    }
}
