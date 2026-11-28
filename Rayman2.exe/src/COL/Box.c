#include "GLI/geometric_helpers.h"
#include "MTH/MTH_flt.h"
#include "MTH/MTH3D.h"

#include "MTH/MTH_flt.h"
#include "MTH/MTH3D.h"
#include "COL/Box.h"

#define COL_C_xNbSidesPerBox 6

MTH3D_tdstVector COL_g_a6_stBoxSidesNormal[COL_C_xNbSidesPerBox] =
{
  { MTH_C_MinusONE,     MTH_C_ZERO,     MTH_C_ZERO },
  {      MTH_C_ONE,     MTH_C_ZERO,     MTH_C_ZERO },
  {     MTH_C_ZERO, MTH_C_MinusONE,     MTH_C_ZERO },
  {     MTH_C_ZERO,      MTH_C_ONE,     MTH_C_ZERO },
  {     MTH_C_ZERO,     MTH_C_ZERO, MTH_C_MinusONE },
  {     MTH_C_ZERO,     MTH_C_ZERO,      MTH_C_ONE }
};

/* 0x004A6880 */
GEO_tdstDoubledIndex COL_g_a12_stBoxEdges[COL_C_xNbEdgesPerBox] =
{
  { { COL_C_xXMinYMinZMin, COL_C_xXMaxYMinZMin } },
  { { COL_C_xXMinYMinZMax, COL_C_xXMaxYMinZMax } },
  { { COL_C_xXMinYMaxZMin, COL_C_xXMaxYMaxZMin } },
  { { COL_C_xXMinYMaxZMax, COL_C_xXMaxYMaxZMax } },
  { { COL_C_xXMinYMinZMin, COL_C_xXMinYMaxZMin } },
  { { COL_C_xXMinYMinZMax, COL_C_xXMinYMaxZMax } },
  { { COL_C_xXMaxYMinZMin, COL_C_xXMaxYMaxZMin } },
  { { COL_C_xXMaxYMinZMax, COL_C_xXMaxYMaxZMax } },
  { { COL_C_xXMinYMinZMin, COL_C_xXMinYMinZMax } },
  { { COL_C_xXMinYMaxZMin, COL_C_xXMinYMaxZMax } },
  { { COL_C_xXMaxYMinZMin, COL_C_xXMaxYMinZMax } },
  { { COL_C_xXMaxYMaxZMin, COL_C_xXMaxYMaxZMax } }
};

#include "COL/Box.h"
#include "COL/CollGOCo.h"

void __cdecl COL_fn_vMinMaxBox2VerticesBox ( COL_tda8st8VerticesBox a8_stBoxDest,
                                     MTH3D_tdstVector *p_stMinPoint,
                                     MTH3D_tdstVector *p_stMaxPoint )
{
  MTH3D_M_vSetVectorElements ( &(a8_stBoxDest[COL_C_xXMinYMinZMin]), p_stMinPoint->xX, p_stMinPoint->xY, p_stMinPoint->xZ );
  MTH3D_M_vSetVectorElements ( &(a8_stBoxDest[COL_C_xXMinYMinZMax]), p_stMinPoint->xX, p_stMinPoint->xY, p_stMaxPoint->xZ );
  MTH3D_M_vSetVectorElements ( &(a8_stBoxDest[COL_C_xXMinYMaxZMin]), p_stMinPoint->xX, p_stMaxPoint->xY, p_stMinPoint->xZ );
  MTH3D_M_vSetVectorElements ( &(a8_stBoxDest[COL_C_xXMinYMaxZMax]), p_stMinPoint->xX, p_stMaxPoint->xY, p_stMaxPoint->xZ );
  MTH3D_M_vSetVectorElements ( &(a8_stBoxDest[COL_C_xXMaxYMinZMin]), p_stMaxPoint->xX, p_stMinPoint->xY, p_stMinPoint->xZ );
  MTH3D_M_vSetVectorElements ( &(a8_stBoxDest[COL_C_xXMaxYMinZMax]), p_stMaxPoint->xX, p_stMinPoint->xY, p_stMaxPoint->xZ );
  MTH3D_M_vSetVectorElements ( &(a8_stBoxDest[COL_C_xXMaxYMaxZMin]), p_stMaxPoint->xX, p_stMaxPoint->xY, p_stMinPoint->xZ );
  MTH3D_M_vSetVectorElements ( &(a8_stBoxDest[COL_C_xXMaxYMaxZMax]), p_stMaxPoint->xX, p_stMaxPoint->xY, p_stMaxPoint->xZ );
}

void __cdecl COL_fn_vComputeBoundingBoxOfSphere(MTH3D_tdstVector *p_stMinBounding, MTH3D_tdstVector *p_stMaxBounding,
                                                MTH3D_tdstVector *p_stSphereCenter, MTH_tdxReal xSphereRadius)
{
  MTH3D_M_vSubScalarVector ( p_stMinBounding, xSphereRadius, p_stSphereCenter );
  MTH3D_M_vAddScalarVector ( p_stMaxBounding, xSphereRadius, p_stSphereCenter );
}

void __cdecl COL_fn_vComputeBoundingBoxOfSphereAndBox ( MTH3D_tdstVector *p_stMinBounding,
                                                MTH3D_tdstVector *p_stMaxBounding,
                                                MTH3D_tdstVector *p_stSphereCenter,
                                                MTH_tdxReal xSphereRadius,
                                                MTH3D_tdstVector *p_stMinPoint,
                                                MTH3D_tdstVector *p_stMaxPoint )
{
  p_stMinBounding->xX = MTH_M_xMin ( MTH_M_xSub ( p_stSphereCenter->xX, xSphereRadius ), p_stMinPoint->xX );
  p_stMinBounding->xY = MTH_M_xMin ( MTH_M_xSub ( p_stSphereCenter->xY, xSphereRadius ), p_stMinPoint->xY );
  p_stMinBounding->xZ = MTH_M_xMin ( MTH_M_xSub ( p_stSphereCenter->xZ, xSphereRadius ), p_stMinPoint->xZ );

  p_stMaxBounding->xX = MTH_M_xMax ( MTH_M_xAdd ( p_stSphereCenter->xX, xSphereRadius ), p_stMaxPoint->xX );
  p_stMaxBounding->xY = MTH_M_xMax ( MTH_M_xAdd ( p_stSphereCenter->xY, xSphereRadius ), p_stMaxPoint->xY );
  p_stMaxBounding->xZ = MTH_M_xMax ( MTH_M_xAdd ( p_stSphereCenter->xZ, xSphereRadius ), p_stMaxPoint->xZ );
}

ACP_tdxBool __cdecl COL_fn_bDetectCollisionBetweenTwoParallelBoxes(MTH3D_tdstVector * _p_stFirstBoxMaxPoint,
                                                                   MTH3D_tdstVector * _p_stFirstBoxMinPoint,
                                                                   MTH3D_tdstVector * _p_stSecondBoxMaxPoint,
                                                                   MTH3D_tdstVector * _p_stSecondBoxMinPoint)
{
	if ( MTH_M_bGreater (_p_stSecondBoxMaxPoint->xX, _p_stFirstBoxMinPoint->xX) &&
		 MTH_M_bLess (_p_stSecondBoxMinPoint->xX, _p_stFirstBoxMaxPoint->xX) &&
		 MTH_M_bGreater (_p_stSecondBoxMaxPoint->xY, _p_stFirstBoxMinPoint->xY) &&
		 MTH_M_bLess (_p_stSecondBoxMinPoint->xY, _p_stFirstBoxMaxPoint->xY) &&
		 MTH_M_bGreater (_p_stSecondBoxMaxPoint->xZ, _p_stFirstBoxMinPoint->xZ) &&
		 MTH_M_bLess (_p_stSecondBoxMinPoint->xZ, _p_stFirstBoxMaxPoint->xZ))
		return TRUE;
	else
		return FALSE;

}
