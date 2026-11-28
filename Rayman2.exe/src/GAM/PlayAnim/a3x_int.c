#include "MTH/MTH_flt.h"

typedef float SEB_xReal;
typedef SEB_xReal tdxMatrix33[3][3];
typedef SEB_xReal tdxQuater4[4];
typedef signed short tdxSShortQuater4[4];

enum { eQX, eQY, eQZ, eQW };

#define xExpandValue ((SEB_xReal) 3.0518509475997192e-05)

#include <math.h>

short GAM_pc_a_wSinTable[8192];   /* 0x004FB660 */

/* 0x0049C3B8 */
static const unsigned long GAM_pc_C_xTwoPiBits  = 0x40C90FDBu;
static const unsigned long GAM_pc_C_xInvStepBits = 0x39000000u;
#define GAM_pc_C_xTwoPi    (*(const float *)&GAM_pc_C_xTwoPiBits)
#define GAM_pc_C_xInvStep  (*(const float *)&GAM_pc_C_xInvStepBits)

static const volatile double GAM_pc_C_xNegHalf = -0.5;

#include "MTH/MTH3D.h"

static float MTH_pc_afMatrixTmp[9];   /* 0x004A70B0 */

/* 0x00418D20 */
typedef float tdxMatrix33[3][3];

/* 0x004190E0 */
extern void fn_v_CopyQuat(unsigned long *p_ulDst, const unsigned long *p_ulSrc);

static float GAM_pc_a4_xQuatTmp[4];   /* 0x004A70E0 */

/* 0x00418AB0 */
void __cdecl fn_v_InitSinTab(void)
{
    long   lIndex;
    short *pwCur;

    lIndex = 0;
    pwCur  = GAM_pc_a_wSinTable;
    do {
        *pwCur = (short)(long)(sin((double)((float)lIndex * GAM_pc_C_xTwoPi) * GAM_pc_C_xInvStep) * 32767.0 - GAM_pc_C_xNegHalf);
        pwCur++;
        lIndex++;
    } while ((long)pwCur < (long)&GAM_pc_a_wSinTable[4096]);
    lIndex = 4096;
    pwCur  = &GAM_pc_a_wSinTable[4096];
    do {
        *pwCur = (short)(long)(sin((double)((float)lIndex * GAM_pc_C_xTwoPi) * GAM_pc_C_xInvStep) * 32767.0 - 0.5);
        pwCur++;
        lIndex++;
    } while ((long)pwCur < (long)&GAM_pc_a_wSinTable[8192]);
}

void __cdecl fn_v_ExpandQuat(tdxQuater4 _a4_xQuatD, tdxSShortQuater4 _a4_xQuatS)
{	register SEB_xReal	r_xExpandValue	=	xExpandValue;
	_a4_xQuatD[0]=	_a4_xQuatS[0] * r_xExpandValue;
	_a4_xQuatD[1]=	_a4_xQuatS[1] * r_xExpandValue;
	_a4_xQuatD[2]=	_a4_xQuatS[2] * r_xExpandValue;
	_a4_xQuatD[3]=	_a4_xQuatS[3] * r_xExpandValue;
}

/* 0x00418BB0 */
void fn_v_CopyQuat(unsigned long *p_ulDst, const unsigned long *p_ulSrc)
{
    unsigned long ulTmp;

    ulTmp = p_ulSrc[0];
    p_ulDst[0] = ulTmp;
    ulTmp = p_ulSrc[1];
    p_ulDst[1] = ulTmp;
    ulTmp = p_ulSrc[2];
    p_ulDst[2] = ulTmp;
    ulTmp = p_ulSrc[3];
    p_ulDst[3] = ulTmp;
}

/* 0x00418BD0 */
void fn_v_MatrixByVector(float *p_fDst, const float *p_fMat, const float *p_fVect)
{

    p_fDst[0] = p_fVect[0] * p_fMat[0] + (p_fMat[3] * p_fVect[1] + p_fMat[6] * p_fVect[2]);
    p_fDst[1] = p_fVect[0] * p_fMat[1] + (p_fMat[4] * p_fVect[1] + p_fMat[7] * p_fVect[2]);
    p_fDst[2] = p_fMat[2] * p_fVect[0] + (p_fMat[5] * p_fVect[1] + p_fMat[8] * p_fVect[2]);
}

/* 0x00418C40 */
void fn_v_DiaByMatrix(float *p_fDst, const float *p_fVect, const float *p_fMat)
{
    float fVar1;

    fVar1 = p_fVect[0];
    p_fDst[0] = fVar1 * p_fMat[0];
    p_fDst[3] = p_fMat[3] * fVar1;
    p_fDst[6] = p_fMat[6] * fVar1;
    fVar1 = p_fVect[1];
    p_fDst[1] = p_fMat[1] * fVar1;
    p_fDst[4] = p_fMat[4] * fVar1;
    p_fDst[7] = p_fMat[7] * fVar1;
    fVar1 = p_fVect[2];
    p_fDst[2] = p_fMat[2] * fVar1;
    p_fDst[5] = p_fMat[5] * fVar1;
    p_fDst[8] = p_fMat[8] * fVar1;
}

/* 0x00418CB0 */
void fn_v_MatrixByDia(float *p_fDst, const float *p_fMat, const float *p_fVect)
{
    float fVar1;

    fVar1 = p_fVect[0];
    p_fDst[0] = fVar1 * p_fMat[0];
    p_fDst[1] = p_fMat[1] * fVar1;
    p_fDst[2] = p_fMat[2] * fVar1;
    fVar1 = p_fVect[1];
    p_fDst[3] = p_fMat[3] * fVar1;
    p_fDst[4] = p_fMat[4] * fVar1;
    p_fDst[5] = p_fMat[5] * fVar1;
    fVar1 = p_fVect[2];
    p_fDst[6] = p_fMat[6] * fVar1;
    p_fDst[7] = p_fMat[7] * fVar1;
    p_fDst[8] = p_fMat[8] * fVar1;
}
void	fn_v_MultMatrix(	tdxMatrix33	_a3a3_xMtxR,
							tdxMatrix33	_a3a3_xMtx1,
							tdxMatrix33	_a3a3_xMtx2 )
{
	_a3a3_xMtxR[0][0]=	_a3a3_xMtx1[0][0] * _a3a3_xMtx2[0][0] +
						_a3a3_xMtx1[1][0] * _a3a3_xMtx2[0][1] +
						_a3a3_xMtx1[2][0] * _a3a3_xMtx2[0][2];
	_a3a3_xMtxR[0][1]=	_a3a3_xMtx1[0][1] * _a3a3_xMtx2[0][0] +
						_a3a3_xMtx1[1][1] * _a3a3_xMtx2[0][1] +
						_a3a3_xMtx1[2][1] * _a3a3_xMtx2[0][2];
	_a3a3_xMtxR[0][2]=	_a3a3_xMtx1[0][2] * _a3a3_xMtx2[0][0] +
						_a3a3_xMtx1[1][2] * _a3a3_xMtx2[0][1] +
						_a3a3_xMtx1[2][2] * _a3a3_xMtx2[0][2];

	_a3a3_xMtxR[1][0]=	_a3a3_xMtx1[0][0] * _a3a3_xMtx2[1][0] +
						_a3a3_xMtx1[1][0] * _a3a3_xMtx2[1][1] +
						_a3a3_xMtx1[2][0] * _a3a3_xMtx2[1][2];
	_a3a3_xMtxR[1][1]=	_a3a3_xMtx1[0][1] * _a3a3_xMtx2[1][0] +
						_a3a3_xMtx1[1][1] * _a3a3_xMtx2[1][1] +
						_a3a3_xMtx1[2][1] * _a3a3_xMtx2[1][2];
	_a3a3_xMtxR[1][2]=	_a3a3_xMtx1[0][2] * _a3a3_xMtx2[1][0] +
						_a3a3_xMtx1[1][2] * _a3a3_xMtx2[1][1] +
						_a3a3_xMtx1[2][2] * _a3a3_xMtx2[1][2];

	_a3a3_xMtxR[2][0]=	_a3a3_xMtx1[0][0] * _a3a3_xMtx2[2][0] +
						_a3a3_xMtx1[1][0] * _a3a3_xMtx2[2][1] +
						_a3a3_xMtx1[2][0] * _a3a3_xMtx2[2][2];
	_a3a3_xMtxR[2][1]=	_a3a3_xMtx1[0][1] * _a3a3_xMtx2[2][0] +
						_a3a3_xMtx1[1][1] * _a3a3_xMtx2[2][1] +
						_a3a3_xMtx1[2][1] * _a3a3_xMtx2[2][2];
	_a3a3_xMtxR[2][2]=	_a3a3_xMtx1[0][2] * _a3a3_xMtx2[2][0] +
						_a3a3_xMtx1[1][2] * _a3a3_xMtx2[2][1] +
						_a3a3_xMtx1[2][2] * _a3a3_xMtx2[2][2];
}

void fn_v_RotDiaInvRot(float *p_fDst, const float *p_fMat, const float *p_fVect)
{
    float fVar1;
    float fVar2;
    float fVar3;

    fn_v_DiaByMatrix(MTH_pc_afMatrixTmp, p_fVect, p_fMat);
    p_fDst[0] = p_fMat[2] * MTH_pc_afMatrixTmp[2] + (MTH_pc_afMatrixTmp[0] * p_fMat[0] + p_fMat[1] * MTH_pc_afMatrixTmp[1]);
    p_fDst[1] = MTH_pc_afMatrixTmp[2] * p_fMat[5] + (MTH_pc_afMatrixTmp[1] * p_fMat[4] + MTH_pc_afMatrixTmp[0] * p_fMat[3]);
    fVar3 = MTH_pc_afMatrixTmp[1] * p_fMat[7];
    fVar1 = MTH_pc_afMatrixTmp[0] * p_fMat[6];
    fVar2 = MTH_pc_afMatrixTmp[2] * p_fMat[8];
    p_fDst[3] = p_fDst[1];
    p_fDst[2] = fVar2 + fVar1 + fVar3;
    p_fDst[4] = MTH_pc_afMatrixTmp[5] * p_fMat[5] + (MTH_pc_afMatrixTmp[3] * p_fMat[3] + MTH_pc_afMatrixTmp[4] * p_fMat[4]);
    fVar3 = MTH_pc_afMatrixTmp[4] * p_fMat[7];
    fVar1 = MTH_pc_afMatrixTmp[3] * p_fMat[6];
    fVar2 = MTH_pc_afMatrixTmp[5] * p_fMat[8];
    p_fDst[6] = p_fDst[2];
    p_fDst[5] = fVar2 + fVar1 + fVar3;
    p_fDst[7] = p_fDst[5];
    p_fDst[8] = ((p_fVect[2] + p_fVect[1] + p_fVect[0]) - p_fDst[0]) - p_fDst[4];
}

void __cdecl fn_v_QuatToMatrix(tdxMatrix33 _a3a3_xMtx, tdxQuater4 _a4_xQuat)
{
    register SEB_xReal  r_xOne;
    register SEB_xReal  xXS, xYS, xZS;

    static SEB_xReal    xWX;   /* 0x004A70A4 */
    static SEB_xReal    xWY;   /* 0x004A70D8 */
    static SEB_xReal    xWZ;   /* 0x004A70F8 */
    static SEB_xReal    xXX;   /* 0x004A70A0 */
    static SEB_xReal    xXY;   /* 0x004A70F0 */
    static SEB_xReal    xXZ;   /* 0x004A70F4 */
    static SEB_xReal    xYY;   /* 0x004A70A8 */
    static SEB_xReal    xYZ;   /* 0x004A70D4 */
    static SEB_xReal    xZZ;   /* 0x004A70AC */

    xXS= _a4_xQuat[eQX];        xXS+= xXS;
    xYS= _a4_xQuat[eQY];        xYS+= xYS;
    xZS= _a4_xQuat[eQZ];        xZS+= xZS;

    xWX= _a4_xQuat[eQW] * xXS;  xWY= _a4_xQuat[eQW] * xYS;  xWZ= _a4_xQuat[eQW] * xZS;
    xXX= _a4_xQuat[eQX] * xXS;  xXY= _a4_xQuat[eQX] * xYS;  xXZ= _a4_xQuat[eQX] * xZS;
    xYY= _a4_xQuat[eQY] * xYS;  xYZ= _a4_xQuat[eQY] * xZS;  xZZ= _a4_xQuat[eQZ] * xZS;

    r_xOne= MTH_C_ONE;
    _a3a3_xMtx[0][0]=   r_xOne - (xYY + xZZ);
    _a3a3_xMtx[1][0]=   xXY  + xWZ;
    _a3a3_xMtx[2][0]=   xXZ  - xWY;

    _a3a3_xMtx[0][1]=   xXY  - xWZ;
    _a3a3_xMtx[1][1]=   r_xOne - (xXX + xZZ);
    _a3a3_xMtx[2][1]=   xYZ  + xWX;

    _a3a3_xMtx[0][2]=   xXZ  + xWY;
    _a3a3_xMtx[1][2]=   xYZ  - xWX;
    _a3a3_xMtx[2][2]=   r_xOne - (xXX + xYY);
}

/* 0x004190A0 */
void fn_v_InterpolVect(float *p_fDst, const float *p_fA, const float *p_fB, float fT)
{
    p_fDst[0] = (p_fB[0] - p_fA[0]) * fT + p_fA[0];
    p_fDst[1] = (p_fB[1] - p_fA[1]) * fT + p_fA[1];
    p_fDst[2] = (p_fB[2] - p_fA[2]) * fT + p_fA[2];
}

void fn_v_InterpolQuatWithOmega(float *_a4_xQuatD, float *_a4_xQuatS1, const unsigned long *_a4_xQuatS2, float _xT, int _swOmega)
{
    short  swCosOmega;
    short  swOmega;
    short  swTbyOmega;
    short  sSinDiff;
    short  sSinTby;
    float  xCoef1;
    double xCoef2;
    double xSinOmega;
    float  xCos;
    float  xSin;

    fn_v_CopyQuat((unsigned long *)GAM_pc_a4_xQuatTmp, _a4_xQuatS2);

    swOmega    = (short)_swOmega;
    swCosOmega = GAM_pc_a_wSinTable[(unsigned long)(_swOmega + 0x800) & 0x1fffUL];

    if (swCosOmega < 0) {
        GAM_pc_a4_xQuatTmp[0] = -GAM_pc_a4_xQuatTmp[0];
        GAM_pc_a4_xQuatTmp[1] = -GAM_pc_a4_xQuatTmp[1];
        GAM_pc_a4_xQuatTmp[2] = -GAM_pc_a4_xQuatTmp[2];
        GAM_pc_a4_xQuatTmp[3] = -GAM_pc_a4_xQuatTmp[3];
        swCosOmega = -swCosOmega;
        swOmega    = (short)(0x1000 - swOmega);
    }

    if (-0x7ffe < swCosOmega) {
        if (GAM_pc_a_wSinTable[swOmega & 0x1fff] == 0) {
            xCoef1 = (float)(1.0f - _xT);
            xCoef2 = _xT;
        }
        else {
            xSinOmega = 1.0f / GAM_pc_a_wSinTable[swOmega & 0x1fff];
            swTbyOmega = (short)(long)((double)swOmega * (double)_xT);
            sSinDiff = GAM_pc_a_wSinTable[((int)swOmega - (int)swTbyOmega) & 0x1fff];
            sSinTby  = GAM_pc_a_wSinTable[(int)swTbyOmega & 0x1fff];
            xCoef1 = (float)((double)sSinDiff * xSinOmega);
            xCoef2 = xSinOmega * (double)sSinTby;
        }
        _a4_xQuatD[0] = (float)(xCoef1 * _a4_xQuatS1[0] + (double)GAM_pc_a4_xQuatTmp[0] * xCoef2);
        _a4_xQuatD[1] = (float)(_a4_xQuatS1[1] * xCoef1 + (double)GAM_pc_a4_xQuatTmp[1] * xCoef2);
        _a4_xQuatD[2] = (float)(_a4_xQuatS1[2] * xCoef1 + (double)GAM_pc_a4_xQuatTmp[2] * xCoef2);
        _a4_xQuatD[3] = (float)(_a4_xQuatS1[3] * xCoef1 + (double)GAM_pc_a4_xQuatTmp[3] * xCoef2);
        return;
    }

    swTbyOmega = (short)(long)(_xT * 2048.0f);
    xCos = (float)GAM_pc_a_wSinTable[(0x800 - swTbyOmega) & 0x1fff];
    xSin = (float)GAM_pc_a_wSinTable[swTbyOmega & 0x1fff];
    _a4_xQuatD[0] = xCos * _a4_xQuatS1[0] - GAM_pc_a4_xQuatTmp[1] * xSin;
    _a4_xQuatD[1] = _a4_xQuatS1[1] * xCos + GAM_pc_a4_xQuatTmp[0] * xSin;
    _a4_xQuatD[2] = _a4_xQuatS1[2] * xCos - GAM_pc_a4_xQuatTmp[3] * xSin;
    _a4_xQuatD[3] = GAM_pc_a4_xQuatTmp[2];
}
