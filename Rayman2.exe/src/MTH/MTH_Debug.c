/*
 =======================================================================================
  Name        : MTH_Debug.c
  Description : Out-of-line MTH functions of the C branch

  MTH3D_M_fn_vMulMatrixMatrixWithoutBufferORG is the matrix product the C branch of
  MTH3D_M_vMulMatrixMatrixWithoutBuffer calls (MTH/MTH3dopt.h). The PC build takes the
  asm branch and never calls it, so the retail exe has none; the targets without x86
  inline assembly (MTH_C_DEV, MTH/MTH_def.h) need it. Body as in the original engine.
 =======================================================================================
 */
#include "MTH/MTH3D.h"

void MTH3D_M_fn_vMulMatrixMatrixWithoutBufferORG(MTH3D_tdstMatrix *MatDest, MTH3D_tdstMatrix *MatA, MTH3D_tdstMatrix *MatB)
{
	(MatDest)->stCol_0.xX =  MTH_M_xMulAddMulAddMul(
		(MatA)->stCol_0.xX, (MatB)->stCol_0.xX,
		(MatA)->stCol_1.xX, (MatB)->stCol_0.xY,
		(MatA)->stCol_2.xX, (MatB)->stCol_0.xZ );
	(MatDest)->stCol_0.xY =  MTH_M_xMulAddMulAddMul(
		(MatA)->stCol_0.xY, (MatB)->stCol_0.xX,
		(MatA)->stCol_1.xY, (MatB)->stCol_0.xY,
		(MatA)->stCol_2.xY, (MatB)->stCol_0.xZ );
	(MatDest)->stCol_0.xZ =  MTH_M_xMulAddMulAddMul(
		(MatA)->stCol_0.xZ, (MatB)->stCol_0.xX,
		(MatA)->stCol_1.xZ, (MatB)->stCol_0.xY,
		(MatA)->stCol_2.xZ, (MatB)->stCol_0.xZ );

	(MatDest)->stCol_1.xX =  MTH_M_xMulAddMulAddMul(
		(MatA)->stCol_0.xX, (MatB)->stCol_1.xX,
		(MatA)->stCol_1.xX, (MatB)->stCol_1.xY,
		(MatA)->stCol_2.xX, (MatB)->stCol_1.xZ );
	(MatDest)->stCol_1.xY =  MTH_M_xMulAddMulAddMul(
		(MatA)->stCol_0.xY, (MatB)->stCol_1.xX,
		(MatA)->stCol_1.xY, (MatB)->stCol_1.xY,
		(MatA)->stCol_2.xY, (MatB)->stCol_1.xZ );
	(MatDest)->stCol_1.xZ =  MTH_M_xMulAddMulAddMul(
		(MatA)->stCol_0.xZ, (MatB)->stCol_1.xX,
		(MatA)->stCol_1.xZ, (MatB)->stCol_1.xY,
		(MatA)->stCol_2.xZ, (MatB)->stCol_1.xZ );

	(MatDest)->stCol_2.xX =  MTH_M_xMulAddMulAddMul(
		(MatA)->stCol_0.xX, (MatB)->stCol_2.xX,
		(MatA)->stCol_1.xX, (MatB)->stCol_2.xY,
		(MatA)->stCol_2.xX, (MatB)->stCol_2.xZ );
	(MatDest)->stCol_2.xY =  MTH_M_xMulAddMulAddMul(
		(MatA)->stCol_0.xY, (MatB)->stCol_2.xX,
		(MatA)->stCol_1.xY, (MatB)->stCol_2.xY,
		(MatA)->stCol_2.xY, (MatB)->stCol_2.xZ );
	(MatDest)->stCol_2.xZ =  MTH_M_xMulAddMulAddMul(
		(MatA)->stCol_0.xZ, (MatB)->stCol_2.xX,
		(MatA)->stCol_1.xZ, (MatB)->stCol_2.xY,
		(MatA)->stCol_2.xZ, (MatB)->stCol_2.xZ );
}
