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
GLI_tdeSafeAreaAnchor GLI_g_eSafeAreaAnchor = GLI_C_eSafeAreaCentre;

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
  Description   : Coordinate of the full width (0..xFull) mapped into the 4:3 band; an
                  element anchored to an edge keeps its 4:3 distance to that edge
                  (scaled like the band) instead
 ----------------------------------------------------------------------------------------
 */
MTH_tdxReal GLI_fn_xToSafeArea(MTH_tdxReal xX, MTH_tdxReal xFull)
{
    switch ( GLI_g_eSafeAreaAnchor )
    {
    case GLI_C_eSafeAreaLeft:
        return MTH_M_xMul( xX, GLI_g_xSafeAreaRatio );
    case GLI_C_eSafeAreaRight:
        return MTH_M_xSub( xFull, MTH_M_xMul( MTH_M_xSub( xFull, xX ), GLI_g_xSafeAreaRatio ) );
    default:
        return MTH_M_xAdd( MTH_M_xMul( xX, GLI_g_xSafeAreaRatio ),
                           MTH_M_xMul( MTH_M_xMul( xFull, MTH_M_xSub( MTH_C_ONE, GLI_g_xSafeAreaRatio ) ), MTH_C_Inv2 ) );
    }
}

/*
 ----------------------------------------------------------------------------------------
  Edge anchors decided per group of 2D elements, not per element: an icon and its
  counter (or a gauge and its head) touch, and a group moves to a screen edge as one,
  so it keeps its 4:3 inner layout. Boxes are in the caller's 4:3 layout units.
 ----------------------------------------------------------------------------------------
 */
/* Two boxes closer than this (layout units, the HUD tables' percent) belong together */
#define GLI_C_xSafeAreaGroupGap MTH_M_xConstFloatToReal(1.0f)

typedef struct GLI_tdstSafeAreaBox_
{
    MTH_tdxReal           xLeft, xTop, xRight, xBottom;
    long                  lGroup;     /* group label (the key of one of its boxes), -1 when unused */
    GLI_tdeSafeAreaAnchor eAnchor;
} GLI_tdstSafeAreaBox;

static GLI_tdstSafeAreaBox gs_a_stSafeAreaBox[GLI_C_lMaxSafeAreaBoxes];

void GLI_fn_vResetSafeAreaBoxes(void)
{
    long lKey;

    for ( lKey = 0 ; lKey < GLI_C_lMaxSafeAreaBoxes ; lKey++ )
    {
        gs_a_stSafeAreaBox[lKey].lGroup  = -1;
        gs_a_stSafeAreaBox[lKey].eAnchor = GLI_C_eSafeAreaCentre;
    }
}

void GLI_fn_vAddSafeAreaBox(long lKey, MTH_tdxReal xLeft, MTH_tdxReal xTop, MTH_tdxReal xRight, MTH_tdxReal xBottom)
{
    GLI_tdstSafeAreaBox *p_stBox;

    if ( lKey < 0 || lKey >= GLI_C_lMaxSafeAreaBoxes )
        return;
    p_stBox = &gs_a_stSafeAreaBox[lKey];
    p_stBox->xLeft   = xLeft;
    p_stBox->xTop    = xTop;
    p_stBox->xRight  = xRight;
    p_stBox->xBottom = xBottom;
    p_stBox->lGroup  = lKey;
}

static unsigned char GLI_fn_bSafeAreaBoxesTouch(GLI_tdstSafeAreaBox *p_stA, GLI_tdstSafeAreaBox *p_stB)
{
    return (unsigned char)(    p_stA->xLeft   <= MTH_M_xAdd( p_stB->xRight,  GLI_C_xSafeAreaGroupGap )
                            && p_stB->xLeft   <= MTH_M_xAdd( p_stA->xRight,  GLI_C_xSafeAreaGroupGap )
                            && p_stA->xTop    <= MTH_M_xAdd( p_stB->xBottom, GLI_C_xSafeAreaGroupGap )
                            && p_stB->xTop    <= MTH_M_xAdd( p_stA->xBottom, GLI_C_xSafeAreaGroupGap ) );
}

void GLI_fn_vComputeSafeAreaAnchors(MTH_tdxReal xFull)
{
    MTH_tdxReal           xThird = MTH_M_xDiv( xFull, MTH_M_xLongToReal(3) );
    MTH_tdxReal           xHalf  = MTH_M_xMul( xFull, MTH_C_Inv2 );
    MTH_tdxReal           xLeft, xRight;
    GLI_tdeSafeAreaAnchor eAnchor;
    long                  i, j, k, lOld, lNew;

    /* groups: merge the groups of every two touching boxes */
    for ( i = 0 ; i < GLI_C_lMaxSafeAreaBoxes ; i++ )
    {
        if ( gs_a_stSafeAreaBox[i].lGroup < 0 )
            continue;
        for ( j = i + 1 ; j < GLI_C_lMaxSafeAreaBoxes ; j++ )
        {
            if ( gs_a_stSafeAreaBox[j].lGroup < 0
              || gs_a_stSafeAreaBox[j].lGroup == gs_a_stSafeAreaBox[i].lGroup
              || !GLI_fn_bSafeAreaBoxesTouch( &gs_a_stSafeAreaBox[i], &gs_a_stSafeAreaBox[j] ) )
                continue;
            lOld = gs_a_stSafeAreaBox[j].lGroup;
            lNew = gs_a_stSafeAreaBox[i].lGroup;
            for ( k = 0 ; k < GLI_C_lMaxSafeAreaBoxes ; k++ )
                if ( gs_a_stSafeAreaBox[k].lGroup == lOld )
                    gs_a_stSafeAreaBox[k].lGroup = lNew;
        }
    }

    /* anchor of each group, from its extent; a group is labelled with the key of one of its boxes */
    for ( i = 0 ; i < GLI_C_lMaxSafeAreaBoxes ; i++ )
    {
        if ( gs_a_stSafeAreaBox[i].lGroup != i )
            continue;
        xLeft  = gs_a_stSafeAreaBox[i].xLeft;
        xRight = gs_a_stSafeAreaBox[i].xRight;
        for ( k = 0 ; k < GLI_C_lMaxSafeAreaBoxes ; k++ )
            if ( gs_a_stSafeAreaBox[k].lGroup == i )
            {
                if ( gs_a_stSafeAreaBox[k].xLeft < xLeft )
                    xLeft = gs_a_stSafeAreaBox[k].xLeft;
                if ( gs_a_stSafeAreaBox[k].xRight > xRight )
                    xRight = gs_a_stSafeAreaBox[k].xRight;
            }

        if ( xLeft <= xThird && xRight <= xHalf )
            eAnchor = GLI_C_eSafeAreaLeft;
        else if ( xRight >= MTH_M_xSub( xFull, xThird ) && xLeft >= xHalf )
            eAnchor = GLI_C_eSafeAreaRight;
        else
            eAnchor = GLI_C_eSafeAreaCentre;

        for ( k = 0 ; k < GLI_C_lMaxSafeAreaBoxes ; k++ )
            if ( gs_a_stSafeAreaBox[k].lGroup == i )
                gs_a_stSafeAreaBox[k].eAnchor = eAnchor;
    }
}

void GLI_fn_vSetSafeAreaAnchorOfBox(long lKey)
{
    if ( lKey < 0 || lKey >= GLI_C_lMaxSafeAreaBoxes )
        GLI_g_eSafeAreaAnchor = GLI_C_eSafeAreaCentre;
    else
        GLI_g_eSafeAreaAnchor = gs_a_stSafeAreaBox[lKey].eAnchor;
}

/*
 ----------------------------------------------------------------------------------------
  Description   : 16 bit bitmap (lWidth x lHeight, 4:3) squeezed into the 4:3 band of a
                  bitmap of the same size, black on the sides: the driver stretches it to
                  the whole screen as before. Columns are picked, not filtered. The buffer
                  is kept between calls (the loading screen redraws at each progress step).
 ----------------------------------------------------------------------------------------
 */
static unsigned short *gs_p_uwBandBuffer  = NULL;
static long            gs_lBandBufferSize = 0;

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
    if ( lSize > gs_lBandBufferSize )
    {
        p_uwDest = (unsigned short *)realloc( gs_p_uwBandBuffer, (size_t)lSize );
        if ( p_uwDest == NULL )
            return p_vBuffer;
        gs_p_uwBandBuffer  = p_uwDest;
        gs_lBandBufferSize = lSize;
    }

    memset( gs_p_uwBandBuffer, 0, (size_t)lSize );
    p_uwSource = (unsigned short *)p_vBuffer;
    p_uwDest   = gs_p_uwBandBuffer + lBandLeft;
    for ( lLine = 0 ; lLine < lHeight ; lLine++ )
    {
        for ( lColumn = 0 ; lColumn < lBandWidth ; lColumn++ )
            p_uwDest[lColumn] = p_uwSource[(lColumn * lWidth) / lBandWidth];
        p_uwSource += lWidth;
        p_uwDest   += lWidth;
    }

    return gs_p_uwBandBuffer;
}

#endif /* RAYMAN2_WIDESCREEN */
