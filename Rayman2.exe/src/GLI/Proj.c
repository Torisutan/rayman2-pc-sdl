#include "GLI/Camera.h"
#include "MTH/MTH_fopt.h"

typedef struct GLI_tdstAligned3DVector_
{
    MTH_tdxReal xX;
    MTH_tdxReal xY;
    MTH_tdxReal xZ;
    MTH_tdxReal ALIGN;
}
GLI_tdstAligned3DVector;

typedef struct GLI_tdstAligned2DVector_
{
    MTH_tdxReal   xX;
    MTH_tdxReal   xY;
    MTH_tdxReal   xOoZ;
    unsigned long ulPackedColor;
}
GLI_tdstAligned2DVector;

void __cdecl GLI_xSerialProjection ( struct GLI_tdstCamera_ *p_stCam ,
                        long lNbOfVertex ,
                        struct GLI_tdstAligned3DVector_  *p_stSource ,
                        struct GLI_tdstAligned2DVector_  *p_stDest )
{
    GLI_tdstAligned2DVector *p_stLast;
    MTH_tdxReal              xClip;
    MTH_tdxReal              xInter;
    MTH_tdxReal              xInv;
    long                     lIndex;

    p_stLast = p_stDest + lNbOfVertex;
    xClip = 8000.0f;
    switch (p_stCam->lCameraMode)
    {
    case GLI_C_lPersCamWithDistorsion:
    case GLI_C_lPersCamWithoutDistorsion:
        for (; p_stDest < p_stLast; p_stDest++, p_stSource++)
        {
            if (*(long *)&p_stSource->xZ < *(long *)&p_stCam->xNear)
            {
                *(unsigned long *)&p_stDest->xOoZ |= 0x80000000;
                *(unsigned long *)&p_stDest->xX = (~*(unsigned long *)&p_stSource->xX & 0x80000000) | *(unsigned long *)&xClip;
                *(unsigned long *)&p_stDest->xY = (~*(unsigned long *)&p_stSource->xY & 0x80000000) | *(unsigned long *)&xClip;
            }
            else
            {
                xInv = MTH_fn_fInverseOpt(p_stSource->xZ);
                p_stDest->xOoZ = xInv;
                xInter = p_stCam->xScreen * xInv;
                p_stDest->xOoZ = xInv * 0.25f;
                p_stDest->xX = p_stCam->stScale.xX * xInter * p_stSource->xX + p_stCam->stTrans.xX;
                p_stDest->xY = p_stCam->stScale.xY * p_stSource->xY * xInter + p_stCam->stTrans.xY;
            }
        }
        break;
    case GLI_C_lIsoCamWithDistorsion:
    case GLI_C_lIsoCamWithoutDistorsion:
        for (lIndex = 0; lIndex < lNbOfVertex; lIndex++)
        {
            if (p_stSource[lIndex].xZ < p_stCam->xNear)
                continue;
            p_stDest[lIndex].xOoZ = 1.0f;
            p_stDest[lIndex].xX = p_stCam->stScale.xX * p_stSource[lIndex].xX + p_stCam->stTrans.xX;
            p_stDest[lIndex].xY = p_stCam->stScale.xY * p_stSource[lIndex].xY + p_stCam->stTrans.xY;
        }
        break;
    }
}

void __cdecl GLI_xSerialProjectionNoTrans  (	struct GLI_tdstCamera_ *p_stCam ,
								long lNbOfVertex ,
								struct GLI_tdstAligned3DVector_ *p_stSource ,
								struct GLI_tdstAligned2DVector_ *p_stDest )
{
    GLI_tdstAligned2DVector *p_stLast;
    MTH_tdxReal              xClip;
    MTH_tdxReal              xInter;
    MTH_tdxReal              xInv;

    p_stLast = p_stDest + lNbOfVertex;
    xClip = 8000.0f;
    for (; p_stDest < p_stLast; p_stDest++, p_stSource++)
    {
        if (*(long *)&p_stSource->xZ < *(long *)&p_stCam->xNear)
        {
            *(unsigned long *)&p_stDest->xOoZ |= 0x80000000;
            *(unsigned long *)&p_stDest->xX = (~*(unsigned long *)&p_stSource->xX & 0x80000000) | *(unsigned long *)&xClip;
            *(unsigned long *)&p_stDest->xY = (~*(unsigned long *)&p_stSource->xY & 0x80000000) | *(unsigned long *)&xClip;
        }
        else
        {
            xInv = MTH_fn_fInverseOpt(p_stSource->xZ);
            p_stDest->xOoZ = xInv;
            xInter = p_stCam->xScreen * xInv;
            p_stDest->xOoZ = xInv * 0.25f;
            p_stDest->xX = p_stCam->stScale.xX * xInter * p_stSource->xX;
            p_stDest->xY = p_stSource->xY * p_stCam->stScale.xY * xInter;
        }
    }
}

#include "GLD/GLD_Pub.h"
#include "GLI/Vpt3D.h"
#ifdef RAYMAN2_WIDESCREEN
#include "GLI/GLI_ratio.h"
#endif /* RAYMAN2_WIDESCREEN */

void __cdecl GLI_vGet3DVertexFromScreenPos(
			struct GLD_tdstViewportAttributes_	*p_stVpt,
			MTH3D_tdstVector			*p_stPos3D,
			MTH_tdxReal					xRatioX,
			MTH_tdxReal					xRatioY,
			MTH_tdxReal					xSize)
{

	GLI_tdst2DVertex					stPos2D, stTrans ;
	GLI_tdstSpecificAttributesFor3D		*p_stSpecAttrib3D;
	GLI_tdstCamera						*p_stCam;
	MTH_tdxReal							xScreen;
	MTH3D_tdstVector					stTemp ;
	POS_tdstCompletePosition			stMatrix, stInvMatrix ;

#ifdef RAYMAN2_WIDESCREEN
	/* screen positions of superimposed objects are in the 4:3 band */
	if ( GLI_g_xSafeAreaRatio < MTH_C_ONE )
		xRatioX = GLI_fn_xToSafeArea(xRatioX, MTH_C_ONE);
#endif /* RAYMAN2_WIDESCREEN */
	stPos2D.xX = MTH_M_xMul(MTH_M_xLongToReal(p_stVpt->dwWidth), xRatioX);
	stPos2D.xY = MTH_M_xMul(MTH_M_xLongToReal(p_stVpt->dwHeight), xRatioY);

	p_stSpecAttrib3D = (GLI_tdstSpecificAttributesFor3D *)p_stVpt->p_vSpecificToXD;
	p_stCam = p_stSpecAttrib3D->p_stCam ;

	switch(p_stCam->lCameraMode)
	{
		case GLI_C_lIsoCamWithDistorsion :
		case GLI_C_lIsoCamWithoutDistorsion :
			xScreen = MTH_M_xConstFloatToReal(0.1f);
			break;

		case GLI_C_lPersCamWithDistorsion :
		case GLI_C_lPersCamWithoutDistorsion :
			xScreen = p_stCam->xScreen;
			break ;

		default:
			p_stPos3D->xX = p_stPos3D->xY = p_stPos3D->xZ = MTH_C_ZERO;
			return;
	}
	stTrans.xX = p_stCam->stTrans.xX;
	stTrans.xY = p_stCam->stTrans.xY;

	if ( xSize != 0.0 )
	{
		stTemp.xX = MTH_M_xDiv(MTH_M_xDiv(MTH_M_xSub(stPos2D.xX, stTrans.xX), p_stCam->stScale.xX), xSize);
		stTemp.xY = MTH_M_xDiv(MTH_M_xDiv(MTH_M_xSub(stPos2D.xY, stTrans.xY), p_stCam->stScale.xY), xSize);
		stTemp.xZ = MTH_M_xMul(MTH_M_xDiv(MTH_M_xAdd(xSize, MTH_C_ONE), xSize), xScreen);
	}
	else
	{
		stTemp.xX = MTH_M_xDiv(MTH_M_xSub(stPos2D.xX, stTrans.xX), p_stCam->stScale.xX);
		stTemp.xY = MTH_M_xDiv(MTH_M_xSub(stPos2D.xY, stTrans.xY), p_stCam->stScale.xY);
		stTemp.xZ = MTH_C_ZERO;
	}

	GLI_xGetCameraMatrix ( p_stCam , (long *)&stMatrix );
	POS_fn_vInvertMatrix ( &stInvMatrix , &stMatrix );
	POS_fn_vMulMatrixVertex ( p_stPos3D , &stInvMatrix , &stTemp );
}
