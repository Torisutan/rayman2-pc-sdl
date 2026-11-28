#include "GLI/Camera.h"
#include "GLI/util.h"
#include "MTH/MTH_flt.h"
#include "MMG/MMG_alloc.h"
#include "GEO/GEO_globals.h"
#include "ERM/CpaError.h"
#include "GLI/Init_Gli_helpers.h"
#include <windows.h>
#include "GLI/GLD.h"

void __cdecl GLI_xSetCameraNearFar(void *p_stCam, float xNear_unused, float xFar)
{
    (void)xNear_unused;

    ((GLI_tdstCamera *)p_stCam)->xNear = 0.25f;

    if (xFar == 0.0f) {
        ((GLI_tdstCamera *)p_stCam)->xFar = 100000.0f;
    } else {
        ((GLI_tdstCamera *)p_stCam)->xFar = xFar;
    }
}

void __cdecl GLI_xGetCameraMatrix(void *p_stCam, long *p_stMatrix)
{

    *(POS_tdstCompletePosition *)p_stMatrix = ((GLI_tdstCamera *)p_stCam)->stMatrix;
}

void __cdecl GLI_xSetCameraMatrix(void *p_stCam, long *p_stMatrix)
{

    ((GLI_tdstCamera *)p_stCam)->stMatrix = *(POS_tdstCompletePosition *)p_stMatrix;
}

void __cdecl GLI_xSetCameraAspectAndRatio(void *p_stCam, float xAlphaX, float xRatio)
{
    ((GLI_tdstCamera *)p_stCam)->xAlphaX = xAlphaX;
    ((GLI_tdstCamera *)p_stCam)->xRatio  = xRatio;
    ((GLI_tdstCamera *)p_stCam)->xAlphaY = MTH_M_xMul(xAlphaX, xRatio);
}

void __cdecl GLI_xGetCameraAspectAndRatio(void *p_stCam, float *p_xAlphaX, float *p_xRatio)
{
    *p_xAlphaX = ((GLI_tdstCamera *)p_stCam)->xAlphaX;
    *p_xRatio  = ((GLI_tdstCamera *)p_stCam)->xRatio;
}

void __cdecl GLI_xSetCameraRatio(void *p_stCam, float xRatio)
{
    ((GLI_tdstCamera *)p_stCam)->xRatio = xRatio;
    ((GLI_tdstCamera *)p_stCam)->xAlphaY =
        MTH_M_xMul(((GLI_tdstCamera *)p_stCam)->xAlphaX, xRatio);
}

#include "GLD/GLD_Pub.h"
#include "GLI/Vpt3D.h"
void __cdecl GLI_xSetViewportCamera(short hDevice, short hViewport, long lCam)
{
    struct GLD_tdstViewportAttributes_ stViewAttrib;
    GLI_tdstSpecificAttributesFor3D *p_stSpecAttrib3D;

    if (GLD_bGetViewportAttributes(hDevice, hViewport, &stViewAttrib) != 0) {
        p_stSpecAttrib3D = (GLI_tdstSpecificAttributesFor3D *)stViewAttrib.p_vSpecificToXD;
        if ((GLI_tdstCamera *)lCam != p_stSpecAttrib3D->p_stCam) {
            p_stSpecAttrib3D->p_stCam = (GLI_tdstCamera *)lCam;
            GLI_xAdjustCameraToViewport(hDevice, hViewport, (struct GLI_tdstCamera_ *)lCam);
        }
    }
}

void __cdecl GLI_xGetViewportCamera(unsigned long hDevice, unsigned short hViewport, void **p_stCam)
{
    struct GLD_tdstViewportAttributes_ stViewAttrib;
    GLI_tdstSpecificAttributesFor3D *p_stSpecAttrib3D;

    if ( !GLD_bGetViewportAttributes( (GLD_tdhDevice)hDevice, hViewport, &stViewAttrib ) )
      return ;

    p_stSpecAttrib3D = (GLI_tdstSpecificAttributesFor3D *)stViewAttrib.p_vSpecificToXD;
    (*p_stCam) = p_stSpecAttrib3D->p_stCam ;
}

void __cdecl GLI_xCreateCamera(GLI_tdstCamera **h_stCam, long lCameraMode)
{
    long *p_lCam;
    long  i;

    /* 0x501524 */
    if (GEO_g_bDynamicAllocation != 2) {
        Mmg_fn_v_SetModeAlloc(g_ucGEOModuleId, g_ucGEOMMemMallocMode, 0);
        *h_stCam = fn_p_vGenAlloc(0xdc, 0);
    }
    if (GEO_g_bDynamicAllocation == 2) {
fallback_path:
        ERM_fn_vClearLastError(0);
        if (GEO_g_bDynamicAllocation == 1) {
            ERM_fn_vUpdateLastError(4, g_ucGEOModuleId, 0, 8, -1, 0xff, 0xff, NULL);
            ERM_fn_vClearLastError(0);
            GEO_g_bDynamicAllocation = 2;
        }
        Mmg_fn_v_SetModeAlloc(g_ucGEOModuleId, 0xff, 0);
        *h_stCam = fn_p_vGenAlloc(0xdc, 0);
    } else if (GEO_g_bDynamicAllocation == 1) {
        if (*h_stCam != NULL) goto fill_block;
        goto fallback_path;
    }
    if (*h_stCam == NULL) {
        ERM_fn_vClearLastError(0);
        ERM_fn_vUpdateLastError(4, g_ucGEOModuleId, 0, 1, -1, 0xff, 0xff, NULL);
    }

fill_block:

    p_lCam = (long *)*h_stCam;
    for (i = 0x37; i != 0; i--) {
        *p_lCam++ = 0;
    }

    if (*h_stCam != NULL) {

        (*h_stCam)->lCameraMode = lCameraMode;
        switch ((*h_stCam)->lCameraMode) {
        case GLI_C_lIsoCamWithDistorsion :
        case GLI_C_lIsoCamWithoutDistorsion :
            (*h_stCam)->xAlphaX = GLI_M_LongToValue( 1 ) ;
            (*h_stCam)->xAlphaY = GLI_M_LongToValue( 1 ) ;
            break ;

        case GLI_C_lPersCamWithDistorsion :
        case GLI_C_lPersCamWithoutDistorsion :
            (*h_stCam)->xAlphaX = GLI_M_PIO4 ;
            (*h_stCam)->xAlphaY = GLI_M_PIO4 ;
            break;
        default:
            break;
        }

        (*h_stCam)->xNear = 0.25f;

        (*h_stCam)->xFar = 10000.0f;
        (*h_stCam)->xRatio = GLI_M_Div((*h_stCam)->xAlphaY , (*h_stCam)->xAlphaX );
    }
}
