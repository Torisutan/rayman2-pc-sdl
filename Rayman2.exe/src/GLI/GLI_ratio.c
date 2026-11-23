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

#include <stdlib.h>
#include <string.h>
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

/*
 ----------------------------------------------------------------------------------------
  Description   : 16 bit bitmap (lWidth x lHeight, 4:3) squeezed into the 4:3 band of a
                  bitmap of the same size, black on the sides: the driver stretches it to
                  the whole screen as before. Columns are picked, not filtered. The buffer
                  is kept between calls (the loading screen redraws at each progress step).
 ----------------------------------------------------------------------------------------
 */
static unsigned short *GLI_gs_p_uwBandBuffer = NULL;
static long            GLI_gs_lBandBufferSize = 0;

void *GLI_fn_p_vSqueezeBitmapToSafeArea(void *p_vBuffer, long lWidth, long lHeight)
{
    unsigned short *p_uwSource;
    unsigned short *p_uwDest;
    long            lBandLeft, lBandWidth, lSize, lLine, lColumn;

    lBandWidth = MTH_M_xRealToLong( MTH_M_xMul( MTH_M_xLongToReal(lWidth), GLI_g_xSafeAreaRatio ) );
    if ( lBandWidth <= 0 || lBandWidth >= lWidth )
        return p_vBuffer;
    lBandLeft = (lWidth - lBandWidth) >> 1;

    lSize = lWidth * lHeight * (long)sizeof(unsigned short);
    if ( lSize > GLI_gs_lBandBufferSize )
    {
        p_uwDest = (unsigned short *)realloc( GLI_gs_p_uwBandBuffer, (size_t)lSize );
        if ( p_uwDest == NULL )
            return p_vBuffer;
        GLI_gs_p_uwBandBuffer  = p_uwDest;
        GLI_gs_lBandBufferSize = lSize;
    }

    memset( GLI_gs_p_uwBandBuffer, 0, (size_t)lSize );
    p_uwSource = (unsigned short *)p_vBuffer;
    p_uwDest   = GLI_gs_p_uwBandBuffer + lBandLeft;
    for ( lLine = 0 ; lLine < lHeight ; lLine++ )
    {
        for ( lColumn = 0 ; lColumn < lBandWidth ; lColumn++ )
            p_uwDest[lColumn] = p_uwSource[(lColumn * lWidth) / lBandWidth];
        p_uwSource += lWidth;
        p_uwDest   += lWidth;
    }

    return GLI_gs_p_uwBandBuffer;
}

#endif /* RAYMAN2_WIDESCREEN */
