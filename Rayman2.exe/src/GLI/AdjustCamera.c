#include "GLI/AdjustCamera.h"
#include "GLD/GLD_Pub.h"
#include "POS/POS_types.h"
#include "MTH/MTH_flt.h"

#include "GLI/Camera.h"
#ifdef RAYMAN2_WIDESCREEN
#include "GLI/GLI_ratio.h"
#endif /* RAYMAN2_WIDESCREEN */

struct GLD_tdstDeviceAttributes_
{
    unsigned long dwHeight;
    unsigned long dwWidth;
    MTH_tdxReal   xInvPixelDimensionX;
    MTH_tdxReal   xInvPixelDimensionY;
};

void __cdecl GLI_xAdjustCameraToViewport2(void *_p_stDev, void *_p_stVpt, struct GLI_tdstCamera_ *p_stCam)
{
    struct GLD_tdstDeviceAttributes_   *p_stDev;
    struct GLD_tdstViewportAttributes_ *p_stVpt;
    MTH_tdxReal xWidth, xHeight, xWidthScaled, xHeightScaled;
    MTH_tdxReal xScaleX, xScaleY, xTanX, xTanY, xHalf, xAlpha;
    MTH_tdxReal xD, xE, xE2, xI, xInv, xSin, xCos;
    unsigned long ulHalf;
#ifdef RAYMAN2_WIDESCREEN
    MTH_tdxReal xSafeAreaRatio;
#endif /* RAYMAN2_WIDESCREEN */

    p_stDev = (struct GLD_tdstDeviceAttributes_ *)_p_stDev;
    p_stVpt = (struct GLD_tdstViewportAttributes_ *)_p_stVpt;

    xWidth        = MTH_M_xLongToReal(p_stVpt->dwWidth);
    xWidthScaled  = p_stDev->xInvPixelDimensionX * xWidth;
    xHeight       = MTH_M_xLongToReal(p_stVpt->dwHeight);
    xHeightScaled = p_stDev->xInvPixelDimensionY * xHeight;

#ifdef RAYMAN2_WIDESCREEN
    /* screen ratio for the 2D safe area */
    GLI_g_xSafeAreaRatio = GLI_fn_xComputeSafeAreaRatio(
        MTH_M_xMul( p_stDev->xInvPixelDimensionX, MTH_M_xLongToReal(p_stDev->dwWidth) ),
        MTH_M_xMul( p_stDev->xInvPixelDimensionY, MTH_M_xLongToReal(p_stDev->dwHeight) ) );
#endif /* RAYMAN2_WIDESCREEN */

    xHalf = xWidth * 0.5f;
    p_stCam->stTrans.xX = xHalf + p_stVpt->iOffsetPosX;
    xHalf = xHeight * 0.5f;
    p_stCam->stTrans.xY = xHalf + p_stVpt->iOffsetPosY;

    switch (p_stCam->lCameraMode)
    {
    case GLI_C_lIsoCamWithoutDistorsion:
        p_stCam->xAlphaY = p_stCam->xAlphaX;
        p_stCam->xScreen = 0.0f;
        xScaleX = MTH_M_xNeg(MTH_C_ONE / (p_stDev->xInvPixelDimensionX * p_stCam->xAlphaX));
        p_stCam->stScale.xX = xScaleX;
        p_stCam->stNormPlaneUp.xZ    = 0.0f;
        p_stCam->stNormPlaneDown.xZ  = 0.0f;
        p_stCam->stNormPlaneLeft.xZ  = 0.0f;
        p_stCam->stNormPlaneRight.xZ = 0.0f;
        p_stCam->stNormPlaneLeft.xY  = 0.0f;
        p_stCam->stNormPlaneRight.xY = 0.0f;
        p_stCam->stNormPlaneDown.xX  = 0.0f;
        p_stCam->stNormPlaneUp.xX    = 0.0f;
        p_stCam->stNormPlaneLeft.xX  = -1.0f;
        p_stCam->stNormPlaneUp.xY    = -1.0f;
        xScaleY = MTH_M_xNeg(MTH_C_ONE / (p_stDev->xInvPixelDimensionY * p_stCam->xAlphaX));
        p_stCam->stScale.xY = xScaleY;
        p_stCam->stNormPlaneRight.xX = 1.0f;
        p_stCam->stNormPlaneDown.xY  = 1.0f;
        xD = -(xWidth / xScaleX);
        p_stCam->xDistPlaneLeft = p_stCam->xDistPlaneRight = xD * 0.5f;
        xD = -(xHeight / xScaleY);
        p_stCam->xDistPlaneDown = p_stCam->xDistPlaneUp = xD * 0.5f;
        return;

    case GLI_C_lIsoCamWithDistorsion:
        p_stCam->xScreen = 0.0f;
        xScaleX = MTH_M_xNeg(xWidth / (p_stCam->xAlphaX * MTH_M_xLongToReal(p_stVpt->dwWidth)
                                       * p_stDev->xInvPixelDimensionX));
        p_stCam->stScale.xX = xScaleX;
        p_stCam->stNormPlaneUp.xZ    = 0.0f;
        p_stCam->stNormPlaneDown.xZ  = 0.0f;
        p_stCam->stNormPlaneLeft.xZ  = 0.0f;
        p_stCam->stNormPlaneRight.xZ = 0.0f;
        p_stCam->stNormPlaneLeft.xY  = 0.0f;
        p_stCam->stNormPlaneRight.xY = 0.0f;
        p_stCam->stNormPlaneDown.xX  = 0.0f;
        p_stCam->stNormPlaneUp.xX    = 0.0f;
        p_stCam->stNormPlaneLeft.xX  = -1.0f;
        p_stCam->stNormPlaneUp.xY    = -1.0f;
        xScaleY = MTH_M_xNeg(xHeight / (p_stCam->xAlphaY * MTH_M_xLongToReal(p_stVpt->dwHeight)
                                        * p_stDev->xInvPixelDimensionY));
        p_stCam->stScale.xY = xScaleY;
        p_stCam->stNormPlaneRight.xX = 1.0f;
        p_stCam->stNormPlaneDown.xY  = 1.0f;
        xD = -(xWidth / xScaleX);
        p_stCam->xDistPlaneLeft = p_stCam->xDistPlaneRight = xD * 0.5f;
        xD = -(xHeight / xScaleY);
        p_stCam->xDistPlaneDown = p_stCam->xDistPlaneUp = xD * 0.5f;
        return;

    case GLI_C_lPersCamWithoutDistorsion:
        xTanX = MTH_M_xTan(p_stCam->xAlphaX * 0.5f);
#ifdef RAYMAN2_WIDESCREEN
        /* Hor+ : the camera keeps its 4:3 angles, a wider viewport sees more on the sides */
        xSafeAreaRatio = GLI_fn_xComputeSafeAreaRatio(xWidthScaled, xHeightScaled);
        if ( xSafeAreaRatio < MTH_C_ONE )
            xTanX = MTH_M_xDiv(xTanX, xSafeAreaRatio);
#endif /* RAYMAN2_WIDESCREEN */
        p_stCam->xScreen = (MTH_tdxReal)(1.0 / xTanX)
                         * (p_stDev->xInvPixelDimensionX * MTH_M_xLongToReal(p_stVpt->dwWidth) * 0.5f);
        p_stCam->stScale.xX = (xWidth / MTH_M_xLongToReal(p_stVpt->dwWidth))
                            * MTH_M_xNeg(MTH_C_ONE / p_stDev->xInvPixelDimensionX);
        xTanY = MTH_M_xTan(p_stCam->xAlphaY * 0.5f);
        p_stCam->stScale.xY = MTH_M_xNeg(xHeight / ((p_stCam->xScreen + p_stCam->xScreen) * xTanY));

        ulHalf = p_stVpt->dwWidth >> 1;
        xI = MTH_M_xLongToReal((long)(p_stVpt->dwClipRightInPix - p_stVpt->iOffsetPosX - ulHalf));
        p_stCam->stNormPlaneLeft.xY = 0.0f;
        xD = MTH_M_xLongToReal(ulHalf) / xTanX;
        xInv = MTH_C_ONE / MTH_M_xSqrt(xI * xI + xD * xD);
        p_stCam->stNormPlaneLeft.xX = -(xInv * xD);
        p_stCam->stNormPlaneLeft.xZ = -(xI * xInv);
        xI = MTH_M_xLongToReal((long)((p_stVpt->dwWidth >> 1) - p_stVpt->dwClipLeftInPix + p_stVpt->iOffsetPosY));
        p_stCam->stNormPlaneRight.xY = 0.0f;
        xInv = MTH_C_ONE / MTH_M_xSqrt(xI * xI + xD * xD);
        p_stCam->stNormPlaneRight.xX = xInv * xD;
        p_stCam->stNormPlaneRight.xZ = -(xI * xInv);

        ulHalf = p_stVpt->dwHeight >> 1;
        xI = MTH_M_xLongToReal((long)(p_stVpt->dwClipBottomInPix - p_stVpt->iOffsetPosY - ulHalf));
        p_stCam->stNormPlaneDown.xX = 0.0f;
        xE = MTH_M_xLongToReal(ulHalf) / xTanY;
        xE2 = xE * xE;
        xInv = MTH_C_ONE / MTH_M_xSqrt(xI * xI + xE2);
        p_stCam->stNormPlaneDown.xY = -(xInv * xE);
        p_stCam->stNormPlaneDown.xZ = -(xI * xInv);
        xI = MTH_M_xLongToReal((long)((p_stVpt->dwHeight >> 1) - p_stVpt->dwClipTopInPix + p_stVpt->iOffsetPosY));
        p_stCam->stNormPlaneUp.xX = 0.0f;
        xInv = MTH_C_ONE / MTH_M_xSqrt(xI * xI + xE2);
        p_stCam->stNormPlaneUp.xY = xInv * xE;
        p_stCam->stNormPlaneUp.xZ = -(xI * xInv);
        break;

    case GLI_C_lPersCamWithDistorsion:
        xHalf = p_stCam->xAlphaX * 0.5f;
        p_stCam->xScreen = (MTH_tdxReal)(1.0 / MTH_M_xTan(xHalf)) * (xWidthScaled * 0.5f);
        xAlpha = MTH_M_xATan(xHeightScaled / (p_stCam->xScreen + p_stCam->xScreen));
        xAlpha = xAlpha + xAlpha;
        p_stCam->xAlphaY = xAlpha;
        p_stCam->stScale.xX = MTH_M_xNeg(MTH_C_ONE / p_stDev->xInvPixelDimensionX);
        p_stCam->stNormPlaneLeft.xY  = 0.0f;
        p_stCam->stNormPlaneRight.xY = 0.0f;
        p_stCam->stNormPlaneUp.xX    = 0.0f;
        p_stCam->stNormPlaneDown.xX  = 0.0f;
        p_stCam->stScale.xY = MTH_M_xNeg(MTH_C_ONE / p_stDev->xInvPixelDimensionY);
        p_stCam->stNormPlaneRight.xZ = p_stCam->stNormPlaneLeft.xZ = -MTH_M_xSin(xHalf);
        xCos = -MTH_M_xCos(xHalf);
        p_stCam->stNormPlaneLeft.xX  = xCos;
        p_stCam->stNormPlaneRight.xX = -xCos;
        xHalf = xAlpha * 0.65f;
        xSin = -MTH_M_xSin(xHalf);
        p_stCam->stNormPlaneUp.xZ = xSin;
        xCos = MTH_M_xCos(xHalf);
        p_stCam->stNormPlaneUp.xY = xCos;
        xCos = -xCos;
        xInv = MTH_C_ONE / MTH_M_xSqrt(xCos * xCos + xSin * xSin);
        p_stCam->stNormPlaneDown.xY = xInv * xCos;
        p_stCam->stNormPlaneDown.xZ = xInv * xSin;
        break;

    default:
        return;
    }

    p_stCam->xDistPlaneUp    = 0.0f;
    p_stCam->xDistPlaneDown  = 0.0f;
    p_stCam->xDistPlaneRight = 0.0f;
    p_stCam->xDistPlaneLeft  = 0.0f;
}
