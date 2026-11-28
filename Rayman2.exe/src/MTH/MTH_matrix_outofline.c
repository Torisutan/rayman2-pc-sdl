#include "MTH/MTH3D.h"

static float MTH_pc_afMatrixTmp[9];   /* 0x004A70B0 */

/* 0x00418D20 */
typedef float tdxMatrix33[3][3];

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

/* 0x004190A0 */
void fn_v_InterpolVect(float *p_fDst, const float *p_fA, const float *p_fB, float fT)
{
    p_fDst[0] = (p_fB[0] - p_fA[0]) * fT + p_fA[0];
    p_fDst[1] = (p_fB[1] - p_fA[1]) * fT + p_fA[1];
    p_fDst[2] = (p_fB[2] - p_fA[2]) * fT + p_fA[2];
}
