#pragma once
/*
 =======================================================================================
  Name        : GLI_ratio.h
  Description : Screen ratio and 4:3 safe area (wide display modes)
 =======================================================================================
 */
#ifdef RAYMAN2_WIDESCREEN

#include "MTH/MTH_flt.h"

/* Ratio of the screen to 4:3 (1 at 4:3 and taller), set by GLI_xAdjustCameraToViewport2 */
extern MTH_tdxReal GLI_g_xSafeAreaRatio;

/**********************************************************************************************/
/* Name: GLI_fn_xComputeSafeAreaRatio */
/* Goal: Ratio of a surface (physical width and height) to 4:3, clamped to 1 */
/**********************************************************************************************/
MTH_tdxReal GLI_fn_xComputeSafeAreaRatio(MTH_tdxReal xWidth, MTH_tdxReal xHeight);

/**********************************************************************************************/
/* Name: GLI_fn_xToSafeArea */
/* Goal: Map a coordinate of the full width (0..xFull) into the centred 4:3 band */
/**********************************************************************************************/
MTH_tdxReal GLI_fn_xToSafeArea(MTH_tdxReal xX, MTH_tdxReal xFull);

/**********************************************************************************************/
/* Name: GLI_fn_p_vSqueezeBitmapToSafeArea */
/* Goal: A 16 bit 4:3 bitmap squeezed into the 4:3 band of a same-size one, black sides */
/**********************************************************************************************/
void *GLI_fn_p_vSqueezeBitmapToSafeArea(void *p_vBuffer, long lWidth, long lHeight);

#endif /* RAYMAN2_WIDESCREEN */
