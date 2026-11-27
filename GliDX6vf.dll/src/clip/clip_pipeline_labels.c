#include "GliDX6vf/GliDX6vf_State.h"
#include "GliDX6vf/clip_staging.h"

#define GLI_CLIP_ONE        1.0f
#define GLI_INTERP_HI       0.98f
#define GLI_INTERP_LO       0.02f
#define GLI_INTERP_BIAS   (-49152.0f)

#define GLI_CAMERA_XNEAR_OFFSET 0x6c

#define GLI_PIPELINE_ADVANCE_AND_CALL() \
    do { GLI_g_pPipelineCursor++; (*GLI_g_pPipelineCursor)(); } while (0)

unsigned int __cdecl GLI_InterpolatePackedColor(unsigned int param_1,
                                                 unsigned int param_2,
                                                 float param_3)
{
    union { float f; unsigned int u; } conv;
    unsigned int bits;
    unsigned int t, sum;

    if (param_3 >= GLI_INTERP_HI) return param_2;
    if (param_3 <= GLI_INTERP_LO) return param_1;

    conv.f = param_3 - GLI_INTERP_BIAS;
    bits = conv.u;

    t = param_2;
    if ((bits & 0x80) == 0) t = param_1;
    sum = (t >> 1) & 0x7f7f7f7fu;
    t = param_2;
    if ((bits & 0x40) == 0) t = param_1;
    sum += (t >> 2) & 0x3f3f3f3fu;
    t = param_2;
    if ((bits & 0x20) == 0) t = param_1;
    sum += (t >> 3) & 0x1f1f1f1fu;
    t = param_2;
    if ((bits & 0x10) == 0) t = param_1;
    sum += (t >> 4) & 0x0f0f0f0fu;
    t = param_2;
    if ((bits & 0x08) == 0) t = param_1;
    sum += (t >> 5) & 0x07070707u;
    t = param_2;
    if ((bits & 0x04) == 0) t = param_1;
    sum += (t >> 6) & 0x03030303u;

    if ((bits & 0x02) != 0) {
        return sum + (param_2 >> 7 & 0x01010101u);
    }
    return sum + (param_1 >> 7 & 0x01010101u);
}

void __cdecl GLI_PL_BackfaceCullA(void)
{
    float *s = (float *)GLI_g_pStagingCursor;

    float cross = (s[0] - s[8]) * (s[17] - s[9]) - (s[1] - s[9]) * (s[16] - s[8]);
    if (cross < 0.0f) return;
    GLI_PIPELINE_ADVANCE_AND_CALL();
}

void __cdecl GLI_PL_BackfaceCullB(void)
{
    float *s = (float *)GLI_g_pStagingCursor;
    float cross = (s[0] - s[8]) * (s[17] - s[9]) - (s[1] - s[9]) * (s[16] - s[8]);

    if (cross > 0.0f) return;
    GLI_PIPELINE_ADVANCE_AND_CALL();
}

void __cdecl GLI_PL_BackfaceCull(void)
{
    if ((*(unsigned int *)(GLI_g_pStagingCursor + 0x0c) &
         *(unsigned int *)(GLI_g_pStagingCursor + 0x2c) &
         *(unsigned int *)(GLI_g_pStagingCursor + 0x4c) &
         0x80000000u) == 0) {
        GLI_PIPELINE_ADVANCE_AND_CALL();
    }
}

void __cdecl GLI_PL_XYGate(void)
{
    unsigned char *buf = (unsigned char *)GLI_g_pStagingCursor;
    unsigned int x0 = *(unsigned int *)(buf + 0x00);
    unsigned int x2 = *(unsigned int *)(buf + 0x40);
    unsigned int x1 = *(unsigned int *)(buf + 0x20);
    unsigned int y0, y1, y2, gate;

    if ((x0 & x2 & x1) & 0x80000000u) return;

    y0 = *(unsigned int *)(buf + 0x04);
    y1 = *(unsigned int *)(buf + 0x24);
    y2 = *(unsigned int *)(buf + 0x44);
    if ((y2 & y1 & y0) & 0x80000000u) return;

    gate = *(unsigned int *)&GLI_g_ClipXGate;
    if (!((int)x0 <= (int)gate || (int)x1 <= (int)gate || (int)x2 <= (int)gate)) return;

    gate = *(unsigned int *)&GLI_g_ClipYGate;
    if (!((int)y0 <= (int)gate || (int)y1 <= (int)gate || (int)y2 <= (int)gate)) return;

    GLI_PIPELINE_ADVANCE_AND_CALL();
}

void __cdecl GLI_PL_CopyUV(void)
{
    *(float *)((unsigned char *)GLI_g_pStagingCursor + 0x18) = *GLI_g_pUVSrc[0];
    *(float *)((unsigned char *)GLI_g_pStagingCursor + 0x1c) = GLI_g_pUVSrc[0][1];
    *(float *)((unsigned char *)GLI_g_pStagingCursor + 0x38) = *GLI_g_pUVSrc[1];
    *(float *)((unsigned char *)GLI_g_pStagingCursor + 0x3c) = GLI_g_pUVSrc[1][1];
    *(float *)((unsigned char *)GLI_g_pStagingCursor + 0x58) = *GLI_g_pUVSrc[2];
    *(float *)((unsigned char *)GLI_g_pStagingCursor + 0x5c) = GLI_g_pUVSrc[2][1];

    GLI_PIPELINE_ADVANCE_AND_CALL();
}

void __cdecl GLI_PL_CopyFog(void)
{
    *(unsigned int *)((unsigned char *)GLI_g_pStagingCursor + 0x14) =
        ((unsigned int *)GLI_g_GliBigGlobals->GLI_aDEF_stColorsRLIS)[GLI_g_pIndexArray[0]];
    *(unsigned int *)((unsigned char *)GLI_g_pStagingCursor + 0x34) =
        ((unsigned int *)GLI_g_GliBigGlobals->GLI_aDEF_stColorsRLIS)[GLI_g_pIndexArray[1]];
    *(unsigned int *)((unsigned char *)GLI_g_pStagingCursor + 0x54) =
        ((unsigned int *)GLI_g_GliBigGlobals->GLI_aDEF_stColorsRLIS)[GLI_g_pIndexArray[2]];

    GLI_PIPELINE_ADVANCE_AND_CALL();
}

#define M_V ((GLI_tdstTLVertex *)GLI_g_pStagingCursor)
#define M_UV(i) ((GLI_tdst2DUVValues *)GLI_g_pUVSrc[i])
#define M_BITS(xFloat) (*(unsigned long *)&(xFloat))
#define M_Ctx GLI_g_GliBigGlobals
#define M_CtxVertex(i) (&M_Ctx->GLI_TurnedScaledDisplecedPoint[i])
#define M_CameraNear (*(float *)((unsigned char *)M_Ctx->p_stCurrentCamera + GLI_CAMERA_XNEAR_OFFSET))
void __cdecl GLI_PL_ZNearClip(void)
{
    GLI_tdstTLVertex a_stOut[4];
    GLI_tdstAligned3DVector stIn;
    GLI_tdstAligned2DVector stProj;
    GLI_tdstAligned3DVector *pIn, *pOut;
    float fT;
    unsigned long ulFlags, nOut, ulCur, ulPrev;
    GLI_PipelineLabel *pSave;

#ifdef GLI_DX6_FIXES

    memset(a_stOut, 0, sizeof(a_stOut));
#endif

    if (((M_BITS(M_V[0].rhw) | M_BITS(M_V[1].rhw) | M_BITS(M_V[2].rhw)) & 0x80000000) == 0)
        goto passthrough;

    ulFlags = M_BITS(M_V[2].rhw) >> 31;
    nOut = 0;
    ulPrev = 2;
    for (ulCur = 0; ulCur < 3; ulCur++) {
        ulFlags = (ulFlags << 1) | (M_BITS(M_V[ulCur].rhw) >> 31);
        switch (ulFlags & 3) {
        case 2:
            pIn = M_CtxVertex(GLI_g_pIndexArray[ulCur]);
            pOut = M_CtxVertex(GLI_g_pIndexArray[ulPrev]);
            fT = (M_CameraNear - pIn->xZ) / (pOut->xZ - pIn->xZ);
            stIn.xX = (pOut->xX - pIn->xX) * fT + pIn->xX;
            stIn.xY = (pOut->xY - pIn->xY) * fT + pIn->xY;
            stIn.xZ = M_Ctx->fZClipping;
            GLI_ProjNearPlaneCallback((unsigned int)M_Ctx->p_stCurrentCamera, 1, &stIn.xX, &stProj);
            a_stOut[nOut].sx = stProj.xX;
            a_stOut[nOut].sy = stProj.xY;
            a_stOut[nOut].sz = a_stOut[nOut].rhw = stProj.xOoZ;
            a_stOut[nOut].color = GLI_InterpolatePackedColor(M_V[ulCur].color, M_V[ulPrev].color, fT);
            a_stOut[nOut].specular = GLI_InterpolatePackedColor(M_V[ulCur].specular, M_V[ulPrev].specular, fT);
            a_stOut[nOut].tu = (M_UV(ulPrev)->xU - M_UV(ulCur)->xU) * fT + M_UV(ulCur)->xU;
            a_stOut[nOut].tv = (M_UV(ulPrev)->xV - M_UV(ulCur)->xV) * fT + M_UV(ulCur)->xV;
            nOut++;

        case 0:
            a_stOut[nOut] = M_V[ulCur];
            *(GLI_tdst2DUVValues *)&a_stOut[nOut].tu = *M_UV(ulCur);
            nOut++;
            break;
        case 1:
            pIn = M_CtxVertex(GLI_g_pIndexArray[ulPrev]);
            pOut = M_CtxVertex(GLI_g_pIndexArray[ulCur]);
            fT = (M_CameraNear - pIn->xZ) / (pOut->xZ - pIn->xZ);
            stIn.xX = (pOut->xX - pIn->xX) * fT + pIn->xX;
            stIn.xY = (pOut->xY - pIn->xY) * fT + pIn->xY;
            stIn.xZ = M_Ctx->fZClipping;
            GLI_ProjNearPlaneCallback((unsigned int)M_Ctx->p_stCurrentCamera, 1, &stIn.xX, &stProj);
            a_stOut[nOut].sx = stProj.xX;
            a_stOut[nOut].sy = stProj.xY;
            a_stOut[nOut].sz = a_stOut[nOut].rhw = stProj.xOoZ;
            a_stOut[nOut].color = GLI_InterpolatePackedColor(M_V[ulPrev].color, M_V[ulCur].color, fT);
            a_stOut[nOut].specular = GLI_InterpolatePackedColor(M_V[ulPrev].specular, M_V[ulCur].specular, fT);
            a_stOut[nOut].tu = (M_UV(ulCur)->xU - M_UV(ulPrev)->xU) * fT + M_UV(ulPrev)->xU;
            a_stOut[nOut].tv = (M_UV(ulCur)->xV - M_UV(ulPrev)->xV) * fT + M_UV(ulPrev)->xV;
            nOut++;
            break;
        }
        ulPrev = ulCur;
    }

    if (nOut == 4) {
        M_V[0] = a_stOut[0];
        M_V[1] = a_stOut[2];
        M_V[2] = a_stOut[3];
        GLI_g_pUVSrc[0] = &a_stOut[0].tu;
        GLI_g_pUVSrc[1] = &a_stOut[2].tu;
        GLI_g_pUVSrc[2] = &a_stOut[3].tu;
        pSave = GLI_g_pPipelineCursor;
        GLI_PIPELINE_ADVANCE_AND_CALL();
        GLI_g_pPipelineCursor = pSave;
        M_V[0] = a_stOut[0];
        M_V[1] = a_stOut[1];
        M_V[2] = a_stOut[2];
        GLI_g_pUVSrc[0] = &a_stOut[0].tu;
        GLI_g_pUVSrc[1] = &a_stOut[1].tu;
        GLI_g_pUVSrc[2] = &a_stOut[2].tu;
    } else if (nOut == 3) {
        M_V[0] = a_stOut[0];
        M_V[1] = a_stOut[1];
        M_V[2] = a_stOut[2];
        GLI_g_pUVSrc[0] = &a_stOut[0].tu;
        GLI_g_pUVSrc[1] = &a_stOut[1].tu;
        GLI_g_pUVSrc[2] = &a_stOut[2].tu;
    } else {
        return;
    }

passthrough:
    GLI_PIPELINE_ADVANCE_AND_CALL();
}
#undef M_CameraNear
#undef M_CtxVertex
#undef M_Ctx
#undef M_BITS
#undef M_UV
#undef M_V

#define M_V ((GLI_tdstTLVertex *)GLI_g_pStagingCursor)
#define M_UV(i) ((GLI_tdst2DUVValues *)GLI_g_pUVSrc[i])
#define M_BITS(xFloat) (*(unsigned long *)&(xFloat))
#define M_CoordMinusBound(v, b) ((v) - (b))
#define M_BoundMinusCoord(v, b) ((b) - (v))

#define GLI_M_DefinePlaneClip(Name, Coord, M_Dist, xBound) \
void __cdecl Name(void) \
{ \
    GLI_tdstTLVertex a_stOut[4]; \
    float a_fRecip[4]; \
    GLI_tdst2DUVValues a_stUV[4]; \
    unsigned long ulFlags; \
    float fT, fCur, fPrev; \
    unsigned long nOut, i; \
    unsigned long ulCur, ulPrev; \
    GLI_tdst2DUVValues *pSrc; \
    GLI_PipelineLabel *pSave; \
 \
    if ((((M_Dist(M_BITS(M_V[0].Coord), M_BITS(xBound)) | M_BITS(M_V[0].Coord)) >> 31) \
         + ((M_Dist(M_BITS(M_V[1].Coord), M_BITS(xBound)) | M_BITS(M_V[1].Coord)) >> 31) \
         + ((M_Dist(M_BITS(M_V[2].Coord), M_BITS(xBound)) | M_BITS(M_V[2].Coord)) >> 31)) == 0) \
        goto passthrough; \
 \
    for (i = 0; i < 3; i++) { \
        pSrc = M_UV(i); \
        a_stUV[i + 1].xU = pSrc->xU * M_V[i].rhw; \
        a_stUV[i + 1].xV = pSrc->xV * M_V[i].rhw; \
        GLI_g_pUVSrc[i] = &a_stUV[i + 1].xU; \
    } \
 \
    fPrev = M_Dist(M_V[2].Coord, xBound); \
    ulFlags = M_BITS(fPrev) >> 31; \
    nOut = 0; \
    ulPrev = 2; \
    for (ulCur = 0; ulCur < 3; ulCur++) { \
        fCur = M_Dist(M_V[ulCur].Coord, xBound); \
        ulFlags = (ulFlags << 1) | (M_BITS(fCur) >> 31); \
        switch (ulFlags & 3) { \
        case 2:   \
            fT = fCur / (fCur - fPrev); \
            a_stOut[nOut].sx = (M_V[ulPrev].sx - M_V[ulCur].sx) * fT + M_V[ulCur].sx; \
            a_stOut[nOut].sy = (M_V[ulPrev].sy - M_V[ulCur].sy) * fT + M_V[ulCur].sy; \
            a_stOut[nOut].sz = a_stOut[nOut].rhw = (M_V[ulPrev].rhw - M_V[ulCur].rhw) * fT + M_V[ulCur].rhw; \
            a_fRecip[nOut] = GLI_CLIP_ONE / a_stOut[nOut].rhw; \
            a_stOut[nOut].Coord = xBound; \
            a_stOut[nOut].color = GLI_InterpolatePackedColor(M_V[ulCur].color, M_V[ulPrev].color, \
                                                             fT * a_fRecip[nOut] * M_V[ulPrev].rhw); \
            a_stOut[nOut].specular = GLI_InterpolatePackedColor(M_V[ulCur].specular, M_V[ulPrev].specular, \
                                                                M_V[ulPrev].rhw * fT * a_fRecip[nOut]); \
            a_stUV[nOut].xU = (M_UV(ulPrev)->xU - M_UV(ulCur)->xU) * fT + M_UV(ulCur)->xU; \
            a_stUV[nOut].xV = (M_UV(ulPrev)->xV - M_UV(ulCur)->xV) * fT + M_UV(ulCur)->xV; \
            nOut++; \
              \
        case 0:   \
            a_stOut[nOut] = M_V[ulCur]; \
            a_stUV[nOut] = *M_UV(ulCur); \
            a_fRecip[nOut] = GLI_CLIP_ONE / a_stOut[nOut].rhw; \
            nOut++; \
            break; \
        case 1:   \
            fT = fPrev / (fPrev - fCur); \
            a_stOut[nOut].sx = (M_V[ulCur].sx - M_V[ulPrev].sx) * fT + M_V[ulPrev].sx; \
            a_stOut[nOut].sy = (M_V[ulCur].sy - M_V[ulPrev].sy) * fT + M_V[ulPrev].sy; \
            a_stOut[nOut].sz = a_stOut[nOut].rhw = (M_V[ulCur].rhw - M_V[ulPrev].rhw) * fT + M_V[ulPrev].rhw; \
            a_fRecip[nOut] = GLI_CLIP_ONE / a_stOut[nOut].rhw; \
            a_stOut[nOut].Coord = xBound; \
            a_stOut[nOut].color = GLI_InterpolatePackedColor(M_V[ulPrev].color, M_V[ulCur].color, \
                                                             fT * a_fRecip[nOut] * M_V[ulCur].rhw); \
            a_stOut[nOut].specular = GLI_InterpolatePackedColor(M_V[ulPrev].specular, M_V[ulCur].specular, \
                                                                M_V[ulCur].rhw * fT * a_fRecip[nOut]); \
            a_stUV[nOut].xU = (M_UV(ulCur)->xU - M_UV(ulPrev)->xU) * fT + M_UV(ulPrev)->xU; \
            a_stUV[nOut].xV = (M_UV(ulCur)->xV - M_UV(ulPrev)->xV) * fT + M_UV(ulPrev)->xV; \
            nOut++; \
            break; \
        } \
        ulPrev = ulCur; \
        fPrev = fCur; \
    } \
 \
    for (i = 0; i < nOut; i++) { \
        a_stUV[i].xU = a_stUV[i].xU * a_fRecip[i]; \
        a_stUV[i].xV = a_stUV[i].xV * a_fRecip[i]; \
    } \
 \
    if (nOut == 4) { \
        M_V[0] = a_stOut[0]; \
        M_V[1] = a_stOut[2]; \
        M_V[2] = a_stOut[3]; \
        GLI_g_pUVSrc[0] = &a_stUV[0].xU; \
        GLI_g_pUVSrc[1] = &a_stUV[2].xU; \
        GLI_g_pUVSrc[2] = &a_stUV[3].xU; \
        pSave = GLI_g_pPipelineCursor; \
        GLI_PIPELINE_ADVANCE_AND_CALL(); \
        GLI_g_pPipelineCursor = pSave; \
        M_V[0] = a_stOut[0]; \
        M_V[1] = a_stOut[1]; \
        M_V[2] = a_stOut[2]; \
        GLI_g_pUVSrc[0] = &a_stUV[0].xU; \
        GLI_g_pUVSrc[1] = &a_stUV[1].xU; \
        GLI_g_pUVSrc[2] = &a_stUV[2].xU; \
    } else if (nOut == 3) { \
        M_V[0] = a_stOut[0]; \
        M_V[1] = a_stOut[1]; \
        M_V[2] = a_stOut[2]; \
        GLI_g_pUVSrc[0] = &a_stUV[0].xU; \
        GLI_g_pUVSrc[1] = &a_stUV[1].xU; \
        GLI_g_pUVSrc[2] = &a_stUV[2].xU; \
    } else { \
        return; \
    } \
 \
passthrough: \
    GLI_PIPELINE_ADVANCE_AND_CALL(); \
}

GLI_M_DefinePlaneClip(GLI_PL_XMinClip, sx, M_CoordMinusBound, GLI_g_ClipXMin)
GLI_M_DefinePlaneClip(GLI_PL_YMaxClip, sy, M_CoordMinusBound, GLI_g_ClipYMax)
GLI_M_DefinePlaneClip(GLI_PL_XMaxClip, sx, M_BoundMinusCoord, GLI_g_ClipXMax)
GLI_M_DefinePlaneClip(GLI_PL_YMinClip, sy, M_BoundMinusCoord, GLI_g_ClipYMin)

#undef GLI_M_DefinePlaneClip
#undef M_BoundMinusCoord
#undef M_CoordMinusBound
#undef M_BITS
#undef M_UV
#undef M_V

#define GLI_GEOOBJ_NB_POINTS_OFFSET 0x2c

#define GLI_FLAG_T_IO(prev, curr)  ((prev) / ((prev) - (curr)))

#define GLI_FLAG_T_OI(curr, prev)  ((curr) / ((curr) - (prev)))

void __cdecl GLI_PL_CopyUVOffset(void)
{
    *(float *)((unsigned char *)GLI_g_pStagingCursor + 0x18) = GLI_g_UVScrollU + *(float *)GLI_g_pUVSrc[0];
    *(float *)((unsigned char *)GLI_g_pStagingCursor + 0x1c) = ((float *)GLI_g_pUVSrc[0])[1] + GLI_g_UVScrollV;
    *(float *)((unsigned char *)GLI_g_pStagingCursor + 0x38) = GLI_g_UVScrollU + *(float *)GLI_g_pUVSrc[1];
    *(float *)((unsigned char *)GLI_g_pStagingCursor + 0x3c) = ((float *)GLI_g_pUVSrc[1])[1] + GLI_g_UVScrollV;
    *(float *)((unsigned char *)GLI_g_pStagingCursor + 0x58) = GLI_g_UVScrollU + *(float *)GLI_g_pUVSrc[2];
    *(float *)((unsigned char *)GLI_g_pStagingCursor + 0x5c) = ((float *)GLI_g_pUVSrc[2])[1] + GLI_g_UVScrollV;
    GLI_PIPELINE_ADVANCE_AND_CALL();
}

void __cdecl GLI_PL_ScaleU(void)
{
    *(float *)((unsigned char *)GLI_g_pStagingCursor + 0x18) *= GLI_UV_SCALE;
    *(float *)((unsigned char *)GLI_g_pStagingCursor + 0x38) *= GLI_UV_SCALE;
    *(float *)((unsigned char *)GLI_g_pStagingCursor + 0x58) *= GLI_UV_SCALE;
    GLI_PIPELINE_ADVANCE_AND_CALL();
}

void __cdecl GLI_PL_ScaleV(void)
{
    *(float *)((unsigned char *)GLI_g_pStagingCursor + 0x1c) =
        (*(float *)((unsigned char *)GLI_g_pStagingCursor + 0x1c) - GLI_UV_V_BIAS) * GLI_UV_SCALE;
    *(float *)((unsigned char *)GLI_g_pStagingCursor + 0x3c) =
        (*(float *)((unsigned char *)GLI_g_pStagingCursor + 0x3c) - GLI_UV_V_BIAS) * GLI_UV_SCALE;
    *(float *)((unsigned char *)GLI_g_pStagingCursor + 0x5c) =
        (*(float *)((unsigned char *)GLI_g_pStagingCursor + 0x5c) - GLI_UV_V_BIAS) * GLI_UV_SCALE;
    GLI_PIPELINE_ADVANCE_AND_CALL();
}

void __cdecl GLI_PL_LoadColorsFromCtx(void)
{
    short *idx = GLI_g_pIndexArray;
    GLI_tdstInternalGlobalValuesFor3dEngine *ctx = GLI_g_GliBigGlobals;
    GLI_g_pUVSrc[0] = (float *)&ctx->GLI_aDEF_stEnvUV[(short)idx[0]];
    GLI_g_pUVSrc[1] = (float *)&ctx->GLI_aDEF_stEnvUV[(short)idx[1]];
    GLI_g_pUVSrc[2] = (float *)&ctx->GLI_aDEF_stEnvUV[(short)idx[2]];
    GLI_PIPELINE_ADVANCE_AND_CALL();
}

void __cdecl GLI_PL_LoadColorsFromElement(void)
{
    unsigned char *elem = (unsigned char *)GLI_g_pElementBlock;
    short *alt = GLI_g_pAltIndexArray;
    GLI_g_pUVSrc[0] = (float *)(*(unsigned char **)(elem + 0x14) + (int)(short)alt[0] * 8);
    GLI_g_pUVSrc[1] = (float *)(*(unsigned char **)(elem + 0x14) + (int)(short)alt[1] * 8);
    GLI_g_pUVSrc[2] = (float *)(*(unsigned char **)(elem + 0x14) + (int)(short)alt[2] * 8);
    GLI_PIPELINE_ADVANCE_AND_CALL();
}

void __cdecl GLI_PL_CulledVertexCheck(void)
{
    short *idx = GLI_g_pIndexArray;
    GLI_tdstInternalGlobalValuesFor3dEngine *ctx = GLI_g_GliBigGlobals;
    unsigned int f0 = *(unsigned int *)&ctx->GLI_aDEF_xWaterPlaneDepth[(short)idx[0]];
    unsigned int f1 = *(unsigned int *)&ctx->GLI_aDEF_xWaterPlaneDepth[(short)idx[1]];
    unsigned int f2 = *(unsigned int *)&ctx->GLI_aDEF_xWaterPlaneDepth[(short)idx[2]];
    if ((f0 & f1 & f2) & 0x80000000u) return;
    GLI_PIPELINE_ADVANCE_AND_CALL();
}

void __cdecl GLI_PL_XSignAndGate(void)
{
    unsigned int gate;
    unsigned char *stg = (unsigned char *)GLI_g_pStagingCursor;
    unsigned int v1x = *(unsigned int *)(stg + 0x20);
    unsigned int v2x = *(unsigned int *)(stg + 0x40);
    unsigned int v0x = *(unsigned int *)(stg + 0x00);
    if ((v0x & v1x & v2x) & 0x80000000u) return;
    gate = *(unsigned int *)&GLI_g_ClipXGate;

    if ((int)v0x > (int)gate && (int)v1x > (int)gate && (int)v2x > (int)gate) return;
    GLI_PIPELINE_ADVANCE_AND_CALL();
}

void __cdecl GLI_PL_XGateReject(void)
{
    unsigned int gate = *(unsigned int *)&GLI_g_ClipXGate;
    unsigned char *stg = (unsigned char *)GLI_g_pStagingCursor;
    if ((int)*(unsigned int *)(stg + 0x00) > (int)gate &&
        (int)*(unsigned int *)(stg + 0x20) > (int)gate &&
        (int)*(unsigned int *)(stg + 0x40) > (int)gate) return;
    GLI_PIPELINE_ADVANCE_AND_CALL();
}

void __cdecl GLI_PL_XSignCheck(void)
{
    unsigned char *stg = (unsigned char *)GLI_g_pStagingCursor;
    unsigned int v0x = *(unsigned int *)(stg + 0x00);
    unsigned int v1x = *(unsigned int *)(stg + 0x20);
    unsigned int v2x = *(unsigned int *)(stg + 0x40);
    if ((v0x & v1x & v2x) & 0x80000000u) return;
    GLI_PIPELINE_ADVANCE_AND_CALL();
}

void __cdecl GLI_PL_YSignAndGate(void)
{
    unsigned char *stg = (unsigned char *)GLI_g_pStagingCursor;
    unsigned int v0y = *(unsigned int *)(stg + 0x04);
    unsigned int v1y = *(unsigned int *)(stg + 0x24);
    unsigned int v2y = *(unsigned int *)(stg + 0x44);
    unsigned int gate;
    if ((v0y & v1y & v2y) & 0x80000000u) return;
    gate = *(unsigned int *)&GLI_g_ClipYGate;
    if ((int)v0y > (int)gate && (int)v1y > (int)gate && (int)v2y > (int)gate) return;
    GLI_PIPELINE_ADVANCE_AND_CALL();
}

void __cdecl GLI_PL_YGateReject(void)
{
    unsigned int gate = *(unsigned int *)&GLI_g_ClipYGate;
    unsigned char *stg = (unsigned char *)GLI_g_pStagingCursor;
    if ((int)*(unsigned int *)(stg + 0x04) > (int)gate &&
        (int)*(unsigned int *)(stg + 0x24) > (int)gate &&
        (int)*(unsigned int *)(stg + 0x44) > (int)gate) return;
    GLI_PIPELINE_ADVANCE_AND_CALL();
}

void __cdecl GLI_PL_YGateReject2(void)
{
    unsigned int gate = *(unsigned int *)&GLI_g_ClipYGate;
    unsigned char *stg = (unsigned char *)GLI_g_pStagingCursor;
    if ((int)*(unsigned int *)(stg + 0x04) > (int)gate &&
        (int)*(unsigned int *)(stg + 0x24) > (int)gate &&
        (int)*(unsigned int *)(stg + 0x44) > (int)gate) return;
    GLI_PIPELINE_ADVANCE_AND_CALL();
}

void __cdecl GLI_PL_LoadVertex(void)
{
    unsigned char *v;

    v = (unsigned char *)&GLI_g_GliBigGlobals->GLI_ScreenPoint[GLI_g_pIndexArray[0]];
    *(unsigned int *)((unsigned char *)GLI_g_pStagingCursor + 0x00) = *(unsigned int *)(v + 0);
    *(unsigned int *)((unsigned char *)GLI_g_pStagingCursor + 0x04) = *(unsigned int *)(v + 4);
    *(unsigned int *)((unsigned char *)GLI_g_pStagingCursor + 0x0c) = *(unsigned int *)(v + 8);
    *(unsigned int *)((unsigned char *)GLI_g_pStagingCursor + 0x08) = *(unsigned int *)((unsigned char *)GLI_g_pStagingCursor + 0x0c);
    *(unsigned int *)((unsigned char *)GLI_g_pStagingCursor + 0x10) = *(unsigned int *)(v + 12);

    v = (unsigned char *)&GLI_g_GliBigGlobals->GLI_ScreenPoint[GLI_g_pIndexArray[1]];
    *(unsigned int *)((unsigned char *)GLI_g_pStagingCursor + 0x20) = *(unsigned int *)(v + 0);
    *(unsigned int *)((unsigned char *)GLI_g_pStagingCursor + 0x24) = *(unsigned int *)(v + 4);
    *(unsigned int *)((unsigned char *)GLI_g_pStagingCursor + 0x2c) = *(unsigned int *)(v + 8);
    *(unsigned int *)((unsigned char *)GLI_g_pStagingCursor + 0x28) = *(unsigned int *)((unsigned char *)GLI_g_pStagingCursor + 0x2c);
    *(unsigned int *)((unsigned char *)GLI_g_pStagingCursor + 0x30) = *(unsigned int *)(v + 12);

    v = (unsigned char *)&GLI_g_GliBigGlobals->GLI_ScreenPoint[GLI_g_pIndexArray[2]];
    *(unsigned int *)((unsigned char *)GLI_g_pStagingCursor + 0x40) = *(unsigned int *)(v + 0);
    *(unsigned int *)((unsigned char *)GLI_g_pStagingCursor + 0x44) = *(unsigned int *)(v + 4);
    *(unsigned int *)((unsigned char *)GLI_g_pStagingCursor + 0x4c) = *(unsigned int *)(v + 8);
    *(unsigned int *)((unsigned char *)GLI_g_pStagingCursor + 0x48) = *(unsigned int *)((unsigned char *)GLI_g_pStagingCursor + 0x4c);
    *(unsigned int *)((unsigned char *)GLI_g_pStagingCursor + 0x50) = *(unsigned int *)(v + 12);

    GLI_PIPELINE_ADVANCE_AND_CALL();
}

#define M_V ((GLI_tdstTLVertex *)GLI_g_pStagingCursor)
#define M_UV(i) ((GLI_tdst2DUVValues *)GLI_g_pUVSrc[i])
#define M_BITS(xFloat) (*(unsigned long *)&(xFloat))
#define M_Ctx GLI_g_GliBigGlobals
#define M_CtxVertex(i) (&M_Ctx->GLI_TurnedScaledDisplecedPoint[i])
#define M_CtxProjected(i) (&M_Ctx->GLI_ScreenPoint[i])
#define M_Flag(i) (M_Ctx->GLI_aDEF_xWaterPlaneDepth[i])

#define M_PlaneA (*(float *)&M_Ctx->xWaterPlaneDistance)
#define M_PlaneNX (M_Ctx->stWaterPlanNormale.xX)
#define M_PlaneNY (M_Ctx->stWaterPlanNormale.xY)
#define M_PlaneNZ (M_Ctx->stWaterPlanNormale.xZ)
void __cdecl GLI_PL_FlagClip(void)
{
    GLI_tdstTLVertex a_stOut[4];
    short a_sOutIndex[4];
    short a_sIndex[3];
    short *pSaveIndex;
    GLI_tdstAligned3DVector *pIn, *pOut, *pNew;
    GLI_tdstAligned2DVector *pProj;
    float fT, fCur, fPrev, fDist;
    unsigned long ulFlags, nOut, ulCur, ulPrev, nOutside;
    short sNew;
    GLI_PipelineLabel *pSave;

    nOutside = (M_BITS(M_Flag(GLI_g_pIndexArray[0])) >> 31) + (M_BITS(M_Flag(GLI_g_pIndexArray[1])) >> 31)
               + (M_BITS(M_Flag(GLI_g_pIndexArray[2])) >> 31);
    if (nOutside == 3 || nOutside == 0) {
        GLI_PIPELINE_ADVANCE_AND_CALL();
        return;
    }

    pSaveIndex = GLI_g_pIndexArray;
    nOut = 0;
    sNew = *(short *)((unsigned char *)M_Ctx->p_stObj + GLI_GEOOBJ_NB_POINTS_OFFSET);
    pNew = M_CtxVertex(sNew);
    pProj = M_CtxProjected(sNew);
    fPrev = M_Flag(GLI_g_pIndexArray[2]);
    ulFlags = M_BITS(fPrev) >> 31;
    ulPrev = 2;
    for (ulCur = 0; ulCur < 3; ulCur++) {
        fCur = M_Flag(GLI_g_pIndexArray[ulCur]);
        ulFlags = (ulFlags << 1) | (M_BITS(fCur) >> 31);
        switch (ulFlags & 3) {
        case 2:
            fT = fCur / (fCur - fPrev);
            pIn = M_CtxVertex(GLI_g_pIndexArray[ulCur]);
            pOut = M_CtxVertex(GLI_g_pIndexArray[ulPrev]);
            pNew->xX = (pOut->xX - pIn->xX) * fT + pIn->xX;
            pNew->xY = (pOut->xY - pIn->xY) * fT + pIn->xY;
            pNew->xZ = (pOut->xZ - pIn->xZ) * fT + pIn->xZ;
            fDist = M_PlaneNX * pNew->xX + M_PlaneNY * pNew->xY + M_PlaneNZ * pNew->xZ - M_PlaneA;
            pNew->xX -= fDist * M_PlaneNX;
            pNew->xY -= M_PlaneNY * fDist;
            pNew->xZ -= M_PlaneNZ * fDist;
            a_stOut[nOut].sx = pProj->xX;
            a_stOut[nOut].sy = pProj->xY;
            a_stOut[nOut].sz = a_stOut[nOut].rhw = pProj->xOoZ;
            a_stOut[nOut].color = GLI_InterpolatePackedColor(M_V[ulCur].color, M_V[ulPrev].color,
                                                             M_V[ulPrev].rhw * fT / a_stOut[nOut].rhw);
            a_stOut[nOut].specular = GLI_InterpolatePackedColor(M_V[ulCur].specular, M_V[ulPrev].specular,
                                                                M_V[ulPrev].rhw * fT / a_stOut[nOut].rhw);
            a_stOut[nOut].tu = (M_UV(ulPrev)->xU - M_UV(ulCur)->xU) * fT + M_UV(ulCur)->xU;
            a_stOut[nOut].tv = (M_UV(ulPrev)->xV - M_UV(ulCur)->xV) * fT + M_UV(ulCur)->xV;
            a_sOutIndex[nOut] = sNew;
            nOut++;
            sNew++;
            pNew++;
            pProj++;

        case 0:
            a_stOut[nOut] = M_V[ulCur];
            a_sOutIndex[nOut] = GLI_g_pIndexArray[ulCur];
            *(GLI_tdst2DUVValues *)&a_stOut[nOut].tu = *M_UV(ulCur);
            nOut++;
            break;
        case 1:
            fT = fPrev / (fPrev - fCur);
            pIn = M_CtxVertex(GLI_g_pIndexArray[ulPrev]);
            pOut = M_CtxVertex(GLI_g_pIndexArray[ulCur]);
            pNew->xX = (pOut->xX - pIn->xX) * fT + pIn->xX;
            pNew->xY = (pOut->xY - pIn->xY) * fT + pIn->xY;
            pNew->xZ = (pOut->xZ - pIn->xZ) * fT + pIn->xZ;
            fDist = M_PlaneNX * pNew->xX + M_PlaneNY * pNew->xY + M_PlaneNZ * pNew->xZ - M_PlaneA;
            pNew->xX -= fDist * M_PlaneNX;
            pNew->xY -= M_PlaneNY * fDist;
            pNew->xZ -= M_PlaneNZ * fDist;
            a_stOut[nOut].sx = pProj->xX;
            a_stOut[nOut].sy = pProj->xY;
            a_stOut[nOut].sz = a_stOut[nOut].rhw = pProj->xOoZ;
            a_stOut[nOut].color = GLI_InterpolatePackedColor(M_V[ulPrev].color, M_V[ulCur].color,
                                                             M_V[ulCur].rhw * fT / a_stOut[nOut].rhw);
            a_stOut[nOut].specular = GLI_InterpolatePackedColor(M_V[ulPrev].specular, M_V[ulCur].specular,
                                                                M_V[ulCur].rhw * fT / a_stOut[nOut].rhw);
            a_stOut[nOut].tu = (M_UV(ulCur)->xU - M_UV(ulPrev)->xU) * fT + M_UV(ulPrev)->xU;
            a_stOut[nOut].tv = (M_UV(ulCur)->xV - M_UV(ulPrev)->xV) * fT + M_UV(ulPrev)->xV;
            a_sOutIndex[nOut] = sNew;
            nOut++;
            sNew++;
            pNew++;
            pProj++;
            break;
        }
        ulPrev = ulCur;
        fPrev = fCur;
    }

    GLI_g_pIndexArray = a_sIndex;
    if (nOut == 4) {
        GLI_g_pIndexArray[0] = a_sOutIndex[0];
        GLI_g_pIndexArray[1] = a_sOutIndex[2];
        GLI_g_pIndexArray[2] = a_sOutIndex[3];
        M_V[0] = a_stOut[0];
        M_V[1] = a_stOut[2];
        M_V[2] = a_stOut[3];
        GLI_g_pUVSrc[0] = &a_stOut[0].tu;
        GLI_g_pUVSrc[1] = &a_stOut[2].tu;
        GLI_g_pUVSrc[2] = &a_stOut[3].tu;
        pSave = GLI_g_pPipelineCursor;
        GLI_PIPELINE_ADVANCE_AND_CALL();
        GLI_g_pPipelineCursor = pSave;
        GLI_g_pIndexArray[0] = a_sOutIndex[0];
        GLI_g_pIndexArray[1] = a_sOutIndex[1];
        GLI_g_pIndexArray[2] = a_sOutIndex[2];
        M_V[0] = a_stOut[0];
        M_V[1] = a_stOut[1];
        M_V[2] = a_stOut[2];
        GLI_g_pUVSrc[0] = &a_stOut[0].tu;
        GLI_g_pUVSrc[1] = &a_stOut[1].tu;
        GLI_g_pUVSrc[2] = &a_stOut[2].tu;
    } else {
        GLI_g_pIndexArray[0] = a_sOutIndex[0];
        GLI_g_pIndexArray[1] = a_sOutIndex[1];
        GLI_g_pIndexArray[2] = a_sOutIndex[2];
        M_V[0] = a_stOut[0];
        M_V[1] = a_stOut[1];
        M_V[2] = a_stOut[2];
        GLI_g_pUVSrc[0] = &a_stOut[0].tu;
        GLI_g_pUVSrc[1] = &a_stOut[1].tu;
        GLI_g_pUVSrc[2] = &a_stOut[2].tu;
    }
    GLI_PIPELINE_ADVANCE_AND_CALL();
    GLI_g_pIndexArray = pSaveIndex;
}

#define M_SetTriangle(a_stV, a_sIdx, i0, i1, i2) \
    do { \
        GLI_g_pIndexArray[0] = a_sIdx[i0]; \
        GLI_g_pIndexArray[1] = a_sIdx[i1]; \
        GLI_g_pIndexArray[2] = a_sIdx[i2]; \
        M_V[0] = a_stV[i0]; \
        M_V[1] = a_stV[i1]; \
        M_V[2] = a_stV[i2]; \
        GLI_g_pUVSrc[0] = &a_stV[i0].tu; \
        GLI_g_pUVSrc[1] = &a_stV[i1].tu; \
        GLI_g_pUVSrc[2] = &a_stV[i2].tu; \
    } while (0)
#define M_SendTriangleAndRestore() \
    do { \
        pSave = GLI_g_pPipelineCursor; \
        GLI_PIPELINE_ADVANCE_AND_CALL(); \
        GLI_g_pPipelineCursor = pSave; \
    } while (0)
void __cdecl GLI_PL_FlagClipComposite(void)
{
    GLI_tdstTLVertex a_stFront[4];
    GLI_tdstTLVertex a_stBack[4];
    short a_sFrontIndex[4];
    short a_sBackIndex[4];
    short a_sIndex[3];
    short *pSaveIndex;
    GLI_tdstAligned3DVector *pIn, *pOut, *pNew;
    GLI_tdstAligned2DVector *pProj;
    float fT, fCur, fPrev, fDist;
    unsigned long ulFlags, nFront, nBack, ulCur, ulPrev, nOutside;
    short sNew;
    GLI_PipelineLabel *pSave;

    nOutside = (M_BITS(M_Flag(GLI_g_pIndexArray[0])) >> 31) + (M_BITS(M_Flag(GLI_g_pIndexArray[1])) >> 31)
               + (M_BITS(M_Flag(GLI_g_pIndexArray[2])) >> 31);
    if (nOutside == 3 || nOutside == 0) {
        GLI_PIPELINE_ADVANCE_AND_CALL();
        return;
    }

    pSaveIndex = GLI_g_pIndexArray;
    nFront = 0;
    nBack = 0;
    sNew = *(short *)((unsigned char *)M_Ctx->p_stObj + GLI_GEOOBJ_NB_POINTS_OFFSET);
    pNew = M_CtxVertex(sNew);
    pProj = M_CtxProjected(sNew);
    fPrev = M_Flag(GLI_g_pIndexArray[2]);
    ulFlags = M_BITS(fPrev) >> 31;
    ulPrev = 2;
    for (ulCur = 0; ulCur < 3; ulCur++) {
        fCur = M_Flag(GLI_g_pIndexArray[ulCur]);
        ulFlags = (ulFlags << 1) | (M_BITS(fCur) >> 31);
        switch (ulFlags & 3) {
        case 2:
            fT = fCur / (fCur - fPrev);
            pIn = M_CtxVertex(GLI_g_pIndexArray[ulCur]);
            pOut = M_CtxVertex(GLI_g_pIndexArray[ulPrev]);
            pNew->xX = (pOut->xX - pIn->xX) * fT + pIn->xX;
            pNew->xY = (pOut->xY - pIn->xY) * fT + pIn->xY;
            pNew->xZ = (pOut->xZ - pIn->xZ) * fT + pIn->xZ;
            fDist = M_PlaneNX * pNew->xX + M_PlaneNY * pNew->xY + M_PlaneNZ * pNew->xZ - M_PlaneA;
            pNew->xX -= fDist * M_PlaneNX;
            pNew->xY -= M_PlaneNY * fDist;
            pNew->xZ -= M_PlaneNZ * fDist;
            GLI_ProjNearPlaneCallback((unsigned int)M_Ctx->p_stCurrentCamera, 1, &pNew->xX, pProj);
            a_stFront[nFront].sx = pProj->xX;
            a_stFront[nFront].sy = pProj->xY;
            a_stFront[nFront].sz = a_stFront[nFront].rhw = pProj->xOoZ;
            a_stBack[nBack].sx = pProj->xX;
            a_stBack[nBack].sy = pProj->xY;
            a_stBack[nBack].sz = a_stBack[nBack].rhw = pProj->xOoZ;
            a_stBack[nBack].color = a_stFront[nFront].color =
                GLI_InterpolatePackedColor(M_V[ulCur].color, M_V[ulPrev].color,
                                           M_V[ulPrev].rhw * fT / a_stFront[nFront].rhw);
            a_stFront[nFront].tu = (M_UV(ulPrev)->xU - M_UV(ulCur)->xU) * fT + M_UV(ulCur)->xU;
            a_stFront[nFront].tv = (M_UV(ulPrev)->xV - M_UV(ulCur)->xV) * fT + M_UV(ulCur)->xV;
            a_stBack[nBack].tu = (M_UV(ulPrev)->xU - M_UV(ulCur)->xU) * fT + M_UV(ulCur)->xU;
            a_stBack[nBack].tv = (M_UV(ulPrev)->xV - M_UV(ulCur)->xV) * fT + M_UV(ulCur)->xV;
            a_sBackIndex[nBack] = sNew;
            a_sFrontIndex[nFront] = sNew;
            nFront++;
            nBack++;
            sNew++;
            pNew++;
            pProj++;

        case 0:
            a_stFront[nFront] = M_V[ulCur];
            a_sFrontIndex[nFront] = GLI_g_pIndexArray[ulCur];
            *(GLI_tdst2DUVValues *)&a_stFront[nFront].tu = *M_UV(ulCur);
            nFront++;
            break;
        case 1:
            fT = fPrev / (fPrev - fCur);
            pIn = M_CtxVertex(GLI_g_pIndexArray[ulPrev]);
            pOut = M_CtxVertex(GLI_g_pIndexArray[ulCur]);
            pNew->xX = (pOut->xX - pIn->xX) * fT + pIn->xX;
            pNew->xY = (pOut->xY - pIn->xY) * fT + pIn->xY;
            pNew->xZ = (pOut->xZ - pIn->xZ) * fT + pIn->xZ;
            fDist = M_PlaneNX * pNew->xX + M_PlaneNY * pNew->xY + M_PlaneNZ * pNew->xZ - M_PlaneA;
            pNew->xX -= fDist * M_PlaneNX;
            pNew->xY -= M_PlaneNY * fDist;
            pNew->xZ -= M_PlaneNZ * fDist;
            GLI_ProjNearPlaneCallback((unsigned int)M_Ctx->p_stCurrentCamera, 1, &pNew->xX, pProj);
            a_stFront[nFront].sx = pProj->xX;
            a_stFront[nFront].sy = pProj->xY;
            a_stFront[nFront].sz = a_stFront[nFront].rhw = pProj->xOoZ;
            a_stBack[nBack].sx = pProj->xX;
            a_stBack[nBack].sy = pProj->xY;
            a_stBack[nBack].sz = a_stBack[nBack].rhw = pProj->xOoZ;
            a_stBack[nBack].color = a_stFront[nFront].color =
                GLI_InterpolatePackedColor(M_V[ulPrev].color, M_V[ulCur].color,
                                           M_V[ulCur].rhw * fT / a_stFront[nFront].rhw);
            a_stFront[nFront].tu = (M_UV(ulCur)->xU - M_UV(ulPrev)->xU) * fT + M_UV(ulPrev)->xU;
            a_stFront[nFront].tv = (M_UV(ulCur)->xV - M_UV(ulPrev)->xV) * fT + M_UV(ulPrev)->xV;
            a_stBack[nBack].tu = (M_UV(ulCur)->xU - M_UV(ulPrev)->xU) * fT + M_UV(ulPrev)->xU;
            a_stBack[nBack].tv = (M_UV(ulCur)->xV - M_UV(ulPrev)->xV) * fT + M_UV(ulPrev)->xV;
            a_sBackIndex[nBack] = sNew;
            a_sFrontIndex[nFront] = sNew;
            nFront++;
            nBack++;
            sNew++;
            pNew++;
            pProj++;

        case 3:
            a_stBack[nBack] = M_V[ulCur];
            a_sBackIndex[nBack] = GLI_g_pIndexArray[ulCur];
            *(GLI_tdst2DUVValues *)&a_stBack[nBack].tu = *M_UV(ulCur);
            nBack++;
            break;
        }
        ulPrev = ulCur;
        fPrev = fCur;
    }

    GLI_g_pIndexArray = a_sIndex;
    if (nFront == 4) {
        M_SetTriangle(a_stFront, a_sFrontIndex, 0, 2, 3);
        M_SendTriangleAndRestore();
        M_SetTriangle(a_stFront, a_sFrontIndex, 0, 1, 2);
        M_SendTriangleAndRestore();
        M_SetTriangle(a_stBack, a_sBackIndex, 0, 1, 2);
    } else {
        M_SetTriangle(a_stBack, a_sBackIndex, 0, 2, 3);
        M_SendTriangleAndRestore();
        M_SetTriangle(a_stBack, a_sBackIndex, 0, 1, 2);
        M_SendTriangleAndRestore();
        M_SetTriangle(a_stFront, a_sFrontIndex, 0, 1, 2);
    }
    GLI_PIPELINE_ADVANCE_AND_CALL();
    GLI_g_pIndexArray = pSaveIndex;
}
#undef M_SendTriangleAndRestore
#undef M_SetTriangle
#undef M_PlaneNZ
#undef M_PlaneNY
#undef M_PlaneNX
#undef M_PlaneA
#undef M_Flag
#undef M_CtxProjected
#undef M_CtxVertex
#undef M_Ctx
#undef M_BITS
#undef M_UV
#undef M_V
