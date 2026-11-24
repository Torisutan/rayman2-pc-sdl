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

/* Where GLI_fn_xToSafeArea puts a 2D element: in the centred 4:3 band, or at a screen edge */
typedef enum GLI_tdeSafeAreaAnchor_
{
    GLI_C_eSafeAreaCentre = 0,
    GLI_C_eSafeAreaLeft,
    GLI_C_eSafeAreaRight
} GLI_tdeSafeAreaAnchor;

/* Anchor of the 2D element being drawn; centre unless a caller sets it for one element */
extern GLI_tdeSafeAreaAnchor GLI_g_eSafeAreaAnchor;

/* Number of boxes (and of keys, 0..GLI_C_lMaxSafeAreaBoxes - 1) one frame can group */
#define GLI_C_lMaxSafeAreaBoxes 128

/**********************************************************************************************/
/* Name: GLI_fn_vResetSafeAreaBoxes */
/* Goal: Forget the boxes of the previous frame (every key is centred again) */
/**********************************************************************************************/
void GLI_fn_vResetSafeAreaBoxes(void);

/**********************************************************************************************/
/* Name: GLI_fn_vAddSafeAreaBox */
/* Goal: Box of a 2D element (4:3 layout coordinates) to be drawn this frame, under lKey */
/**********************************************************************************************/
void GLI_fn_vAddSafeAreaBox(long lKey, MTH_tdxReal xLeft, MTH_tdxReal xTop, MTH_tdxReal xRight, MTH_tdxReal xBottom);

/**********************************************************************************************/
/* Name: GLI_fn_vComputeSafeAreaAnchors */
/* Goal: Group the boxes that touch; a group starting in the left third and ending in the */
/*       left half of the layout 0..xFull goes to the left edge (mirrored: right edge), */
/*       any other group stays centred */
/**********************************************************************************************/
void GLI_fn_vComputeSafeAreaAnchors(MTH_tdxReal xFull);

/**********************************************************************************************/
/* Name: GLI_fn_vSetSafeAreaAnchorOfBox */
/* Goal: Anchor the next 2D element with the anchor of its box's group (centre if no box) */
/**********************************************************************************************/
void GLI_fn_vSetSafeAreaAnchorOfBox(long lKey);

/**********************************************************************************************/
/* Name: GLI_fn_xComputeSafeAreaRatio */
/* Goal: Ratio of a surface (physical width and height) to 4:3, clamped to 1 */
/**********************************************************************************************/
MTH_tdxReal GLI_fn_xComputeSafeAreaRatio(MTH_tdxReal xWidth, MTH_tdxReal xHeight);

/**********************************************************************************************/
/* Name: GLI_fn_xToSafeArea */
/* Goal: Map a coordinate of the full width (0..xFull) into the 4:3 band, or keep its */
/*       distance (scaled like the band) to the anchored screen edge */
/**********************************************************************************************/
MTH_tdxReal GLI_fn_xToSafeArea(MTH_tdxReal xX, MTH_tdxReal xFull);

/**********************************************************************************************/
/* Name: GLI_fn_p_vSqueezeBitmapToSafeArea */
/* Goal: A 16 bit 4:3 bitmap squeezed into the 4:3 band of a same-size one, black sides */
/**********************************************************************************************/
void *GLI_fn_p_vSqueezeBitmapToSafeArea(void *p_vBuffer, long lWidth, long lHeight);

#endif /* RAYMAN2_WIDESCREEN */
