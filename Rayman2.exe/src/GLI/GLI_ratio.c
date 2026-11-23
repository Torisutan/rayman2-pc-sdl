/*
 =======================================================================================
  Name        : GLI_ratio.c
  Description : Screen ratio and 4:3 safe area (wide display modes)

  The ratio comes from the resolution: the physical height over the physical width of a
  surface (pixels weighted by the device pixel dimensions), compared to 4:3. The 3D view
  widens by it (Hor+) and the 2D elements are drawn in the centred 4:3 band it defines.
 =======================================================================================
 */
#ifdef RAYMAN2_WIDESCREEN

#include "MTH/MTH_flt.h"
#include "GLI/GLI_ratio.h"

#define GLI_C_xRatio4_3     MTH_M_xConstFloatToReal(0.75f)

MTH_tdxReal GLI_g_xSafeAreaRatio = MTH_C_ONE;

/*
 ----------------------------------------------------------------------------------------
  Description   : Ratio of a surface to 4:3, 1 when it is 4:3 or taller
 ----------------------------------------------------------------------------------------
 */
MTH_tdxReal GLI_fn_xComputeSafeAreaRatio(MTH_tdxReal xWidth, MTH_tdxReal xHeight)
{
    MTH_tdxReal xRatio;

    if ( xWidth <= MTH_C_ZERO )
        return MTH_C_ONE;

    xRatio = MTH_M_xDiv( xHeight, MTH_M_xMul( xWidth, GLI_C_xRatio4_3 ) );
    if ( xRatio > MTH_C_ONE )
        return MTH_C_ONE;
    return xRatio;
}

/*
 ----------------------------------------------------------------------------------------
  Description   : Coordinate of the full width (0..xFull) mapped into the 4:3 band
 ----------------------------------------------------------------------------------------
 */
MTH_tdxReal GLI_fn_xToSafeArea(MTH_tdxReal xX, MTH_tdxReal xFull)
{
    return MTH_M_xAdd( MTH_M_xMul( xX, GLI_g_xSafeAreaRatio ),
                       MTH_M_xMul( MTH_M_xMul( xFull, MTH_M_xSub( MTH_C_ONE, GLI_g_xSafeAreaRatio ) ), MTH_C_Inv2 ) );
}

#endif /* RAYMAN2_WIDESCREEN */
