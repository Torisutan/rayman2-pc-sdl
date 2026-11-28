#include "HIE/HIE_types.h"
#include "POS/POS_types.h"
#include <string.h>

#include "MMG/MMG_alloc.h"

#include "GEO/GeoMem.h"
#include "HIE/HIE_globals.h"
#include "GLI/GLI_globals.h"
#include "GLD/GLD_Pub.h"
#include "GLI/Vpt3D.h"

#define HIE_g_a_hMatrixStack (HIE_g_a_hMatrixStack_backing + 1)

void *GEO_fn_hCreateMatrix(void)
{
    void *hMatrix;

    GEO_M_CPAMalloc(hMatrix, void *, sizeof(POS_tdstCompletePosition), E_uwGEONotEnoughtMemory);
    POS_fn_vSetIdentityMatrix((POS_tdstCompletePosition *)hMatrix);
    return hMatrix;
}

void HIE_fn_vCreateMatrixStack(void)
{
    HIE_g_a_hMatrixStack[0] = GEO_fn_hCreateMatrix();
    HIE_g_lNbMatrixInStack  = 1;
}

#define HIE_M_vUpdateStack()\
{\
	GLI_g_p_stCurrentMatrix=HIE_g_hCurrentMatrix=HIE_g_a_hMatrixStack[HIE_g_lNbMatrixInStack-1];\
}

void HIE_fn_vInitMatrixStack(void)
{
	HIE_g_lNbMatrixInStack=0;
	POS_fn_vSetIdentityMatrix(HIE_g_a_hMatrixStack[HIE_g_lNbMatrixInStack]);
	HIE_g_lNbMatrixInStack++;
	HIE_M_vUpdateStack();
}

ACP_tdxBool HIE_fn_bLoadIdentity(void)
{
	if( HIE_g_lNbMatrixInStack == HIE_C_lMaxMatrixInStack )
		return 0;

	HIE_g_a_hMatrixStack[HIE_g_lNbMatrixInStack] = HIE_g_a_hMatrixStack[0];
	HIE_g_lNbMatrixInStack++;
	HIE_M_vUpdateStack();
	return 1;
}

ACP_tdxBool HIE_fn_bPopMatrix(void)
{
	if( HIE_g_lNbMatrixInStack == 0 )
		return 0;

	HIE_g_lNbMatrixInStack--;
	HIE_M_vUpdateStack();
	return 1;
}

ACP_tdxBool HIE_fn_bStoreMatrix( void *_hMatrix )
{
	if( HIE_g_lNbMatrixInStack == HIE_C_lMaxMatrixInStack )
		return 0;

	HIE_g_a_hMatrixStack[HIE_g_lNbMatrixInStack] = _hMatrix;

	HIE_g_lNbMatrixInStack++;
	HIE_M_vUpdateStack();
	return 1;
}

extern POS_tdstCompletePosition  *g_p_stCurrentInCamCoordsMatrix;   /* 0x00514D64 */

#define HIE_M_vUpdateCameraMatrixStack()	\
{\
	g_p_stCurrentInCamCoordsMatrix = &HIE_g_a_stCameraMatrixStack[HIE_g_lNbMatrixInCameraStack-1];	\
}

int HIE_fn_bInitCameraMatrixStack(void *_hVpt)
{
	HIE_g_lNbMatrixInCameraStack=0;
	memcpy(&HIE_g_a_stCameraMatrixStack[HIE_g_lNbMatrixInCameraStack] , &((( GLI_tdstSpecificAttributesFor3D *)((struct GLD_tdstViewportAttributes_ *)_hVpt)->p_vSpecificToXD)->p_stCam)->stMatrix, sizeof(POS_tdstCompletePosition));
	HIE_g_lNbMatrixInCameraStack++;
	HIE_M_vUpdateCameraMatrixStack();
	return 1;
}

ACP_tdxBool HIE_fn_bPushOnCameraMatrixStack( void *_hMatrix )
{
	if( !HIE_g_lNbMatrixInCameraStack || HIE_g_lNbMatrixInCameraStack == HIE_C_lMaxMatrixInStack )
		return 0;

	POS_fn_vMulMatrixMatrix( &HIE_g_a_stCameraMatrixStack[HIE_g_lNbMatrixInCameraStack] , &HIE_g_a_stCameraMatrixStack[HIE_g_lNbMatrixInCameraStack-1]
																					    ,  (POS_tdstCompletePosition *)_hMatrix );
	HIE_g_lNbMatrixInCameraStack++;
	HIE_M_vUpdateCameraMatrixStack();

	return 1;
}

ACP_tdxBool HIE_fn_bPopOnCameraMatrixStack(void)
{
	if( HIE_g_lNbMatrixInCameraStack == 0 )
		return 0;

	HIE_g_lNbMatrixInCameraStack--;
	HIE_M_vUpdateCameraMatrixStack();

	return 1;
}
