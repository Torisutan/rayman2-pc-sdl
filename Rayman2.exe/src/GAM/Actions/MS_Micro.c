#include <string.h>
#include "GAM/GAM_types.h"
#include "SND/SND.h"
#include "GAM/GAM_globals.h"
#include "POS/POS_types.h"
#include "MTH/MTH_flt.h"
#include "MTH/MTH3D.h"
#include "DNM/DNM_types.h"
#include "DNM/DNM.h"

void *__cdecl fn_hReturnHandleMicroFromIndex(long lIndex);

#ifndef MTH_M_xRealToFixed16_16
#define MTH_M_xRealToFixed16_16(_x) ((long)((_x) * 65536.0F))
#endif
#ifndef M_Fixed1616ToRealSnd
#define M_Fixed1616ToRealSnd(_r) (_r)
#endif
#ifndef DNM_M_lDynamicsGetObjectType
#define DNM_M_lDynamicsGetObjectType(_p) (*(long *)(_p))
#endif
#ifndef DNM_M_p_stReportGetLastMove
#define DNM_M_p_stReportGetLastMove(_p) MTH_M_p_stMoveGetLinear(DNM_M_p_stReportGetAbsolutePrevSpeed(_p))
#endif

struct MS_stMicroLocal { void *hMicroMatrix; };
#define MS_M_hMicroMatrixOf(_hSuperObj) \
    (((struct MS_stMicroLocal *)((struct tdstEngineObject_ *)HIE_fn_hGetSuperObjectObject(_hSuperObj))->h_Micro)->hMicroMatrix)

long RetMicroSonoreFILTRE(long indice)
{
   (void)indice;
   return 0;
}

typedef struct MS_pc_tdstMicroElem MS_pc_tdstMicroElem;
typedef struct MS_pc_tdstMicroAnchor MS_pc_tdstMicroAnchor;
struct MS_pc_tdstMicroElem
{
    MS_pc_tdstMicroElem *hNext;
    MS_pc_tdstMicroElem *hPrev;
    void *hAnchor;
    long lIndex;
    void *hMicroObj;
};
struct MS_pc_tdstMicroAnchor
{
    MS_pc_tdstMicroElem *hFirst;
    MS_pc_tdstMicroElem *hLast;
    long lCount;
};

#define MS_pc_M_pAnchorOf(_p) ((MS_pc_tdstMicroAnchor *)((_p)->hAnchor))

void __cdecl SND_fn_vDestroyMicroSound(long lMicro);

void ERM_fn_vClearLastError(unsigned long ulChannelId);
void ERM_fn_vUpdateLastError(unsigned short uwStartOfWarningsId, unsigned char ucModuleId,
                             unsigned long ulChannelId, unsigned short uwErrNum,
                             long lDebugData, unsigned char ucOpenInfoWindow,
                             unsigned char ucStopForDebug, char *szPersonalMessage);

unsigned char __cdecl fn_bMicroGetActivationFlag(void *h_MS_Micro);

long __cdecl SND_fn_lCreateMicroSound(void);

struct MS_pc_stMicroActive
{
    void *hMicroMatrix;
    unsigned char bIsActive;
};

#include "GAM/Macros.h"

/* 0x004111A0 */
typedef struct MS_stMicroLocal *MS_tdxHandleToMicro;

/* 0x00411190 */
unsigned long __cdecl GAM_pc_fn_ulGetHolderSize8(void);
#define fn_ulMicroSizeOf GAM_pc_fn_ulGetHolderSize8
MS_tdxHandleToMicro fn_h_MicroRealAlloc(void);

void fn_vMicroAlloc(struct tdstEngineObject_ *p_stObject)
{
	MTH3D_tdstVector stI, stJ, stK;

	if (p_stObject==NULL)
		M_GameFatalError(E_uwGameMiniStructObjectNull);

	if (p_stObject->h_Micro!=NULL)
		M_GameFatalError(E_uwGameMiniStructAllocAlreadyAlloc);

	p_stObject->h_Micro = fn_h_MicroRealAlloc();

	MTH3D_M_vSetVectorElements(&stJ, MTH_C_MinusONE, MTH_C_ZERO, MTH_C_ZERO);
	MTH3D_M_vSetBaseKVector(&stK);
	if (p_stObject -> h_Cineinfo != NULL)
	{

		MTH3D_M_vSetVectorElements(&stI, MTH_C_ZERO, MTH_C_MinusONE, MTH_C_ZERO);
	}
	else
	{

		MTH3D_M_vSetVectorElements(&stI, MTH_C_ZERO, MTH_C_ONE, MTH_C_ZERO);
	}
	POS_fn_vSetRotationMatrix(((MS_tdxHandleToMicro)p_stObject->h_Micro)->hMicroMatrix, &stI, &stJ, &stK);

}

/* 0x00411280 */
MS_tdxHandleToMicro fn_h_MicroRealAlloc()
{
   MS_tdxHandleToMicro h_MS_Micro;

   h_MS_Micro = (MS_tdxHandleToMicro)fnp_vGameMallocInHLM(fn_ulMicroSizeOf());
   h_MS_Micro->hMicroMatrix = GEO_fn_hCreateMatrix();
   return(h_MS_Micro);
}
void __cdecl fn_vGameFreeInHLM(void *p_vObject);

/* 0x004111A0 */
void __cdecl fn_vMicroFree(void *p_pOwner)
{
    if (p_pOwner == (void *)0)
    {
        ERM_fn_vClearLastError(0);
        ERM_fn_vUpdateLastError(0x30, g_ucGameModuleId, 0, 0xe, -1, 0xff, 0xff, (char *)0);
    }
    if (((struct tdstEngineObject_ *)p_pOwner)->h_Micro == (void *)0)
    {
        ERM_fn_vClearLastError(0);
        ERM_fn_vUpdateLastError(0x30, g_ucGameModuleId, 0, 0xf, -1, 0xff, 0xff, (char *)0);
    }
    fn_vGameFreeInHLM(((struct tdstEngineObject_ *)p_pOwner)->h_Micro);
    ((struct tdstEngineObject_ *)p_pOwner)->h_Micro = (void *)0;
}

void __cdecl fn_vAddMicro(struct tdstEngineObject_ *hEngObj);

/* 0x00411320 */
void __cdecl fn_vActivateMicroForObject(struct tdstEngineObject_ *hEngObj, long lActivate)
{
    if (hEngObj == NULL)
    {
        ERM_fn_vClearLastError(0);
        ERM_fn_vUpdateLastError(0x30u, g_ucGameModuleId, 0, 0x0eu, -1, 0xff, 0xff, (char *)0);
    }
    if (hEngObj->h_Micro == NULL)
    {
        ERM_fn_vClearLastError(0);
        ERM_fn_vUpdateLastError(0x30u, g_ucGameModuleId, 0, 0x0fu, -1, 0xff, 0xff, (char *)0);
    }
    switch (lActivate)
    {
    case 0:
        break;
    case 1:
        if (fn_bMicroGetActivationFlag(hEngObj->h_Micro) != 0)
        {
            fn_vAddMicro(hEngObj);
        }
        break;
    default:
        break;
    }
}

/* 0x00411450 */
void *__cdecl fn_hReturnHandleMicroFromIndex(long lIndex)
{
    MS_pc_tdstMicroElem *hMicro = (MS_pc_tdstMicroElem *)GAM_g_stIndexMicrosList_hFirst;

    while (hMicro != NULL)
    {
        if (hMicro->lIndex == lIndex)
            return hMicro->hMicroObj;
        hMicro = hMicro->hNext;
    }
    return NULL;
}

/* 0x00411470 */
long __cdecl RetMicroSonoreX(long indice)
{
    MTH3D_tdstVector stVertex;
    struct tdstEngineObject_ *p_Object = (struct tdstEngineObject_ *)fn_hReturnHandleMicroFromIndex(indice);
    struct HIE_tdstSuperObject *hSuperObj = M_GetSuperObject(p_Object);
    POS_fn_vGetTranslationVector(HIE_fn_hGetSuperObjectGlobalMatrix(hSuperObj), &stVertex);
    return (long)(stVertex.xX * 65536.0f);
}
long __cdecl RetMicroSonoreY(long indice)
{
    MTH3D_tdstVector stVertex;
    struct tdstEngineObject_ *p_Object = (struct tdstEngineObject_ *)fn_hReturnHandleMicroFromIndex(indice);
    struct HIE_tdstSuperObject *hSuperObj = M_GetSuperObject(p_Object);
    POS_fn_vGetTranslationVector(HIE_fn_hGetSuperObjectGlobalMatrix(hSuperObj), &stVertex);
    return (long)(stVertex.xY * 65536.0f);
}
long __cdecl RetMicroSonoreZ(long indice)
{
    MTH3D_tdstVector stVertex;
    struct tdstEngineObject_ *p_Object = (struct tdstEngineObject_ *)fn_hReturnHandleMicroFromIndex(indice);
    struct HIE_tdstSuperObject *hSuperObj = M_GetSuperObject(p_Object);
    POS_fn_vGetTranslationVector(HIE_fn_hGetSuperObjectGlobalMatrix(hSuperObj), &stVertex);
    return (long)(stVertex.xZ * 65536.0f);
}

/* 0x00411530 */
long __cdecl RetMicroSonoreVX(long indice)
{
	struct tdstEngineObject_	*p_Object;
	struct HIE_tdstSuperObject	*hSuperObj;
	DNM_tdstDynamics			*p_stDynamics;
	MTH_tdxReal					xResult = MTH_C_ZERO;

	p_Object  = fn_hReturnHandleMicroFromIndex(indice);
	if (p_Object)
	{
		hSuperObj = M_GetSuperObject( p_Object );
		if( !M_GetMSHandle(hSuperObj,Dynam) )
			return 0;
		if( (p_stDynamics = fn_p_stDynamGetDynamics(M_GetMSHandle(hSuperObj,Dynam))) == NULL )
			return 0;

		if (DNM_M_lDynamicsGetObjectType(p_stDynamics)==DNM_eCamera)
		{
			MTH_tdxReal xInvDTInSecond;

			xInvDTInSecond = MTH_M_xDiv(MTH_M_xConstFloatToReal(1000.f),MTH_M_xLongToReal(GAM_g_ulUsefulDeltaTime));

			xResult = MTH_M_xMul(MTH3D_M_xGetXofVector(DNM_M_p_stReportGetLastMove(DNM_M_p_stDynamicsGetReport(p_stDynamics))),xInvDTInSecond);
		}
		else
		{
			xResult = MTH3D_M_xGetXofVector(DNM_M_pDynamicsGetPreviousSpeed(p_stDynamics));
		}
	}

	if(xResult>MTH_M_xConstFloatToReal(32000.0f))
		xResult=MTH_M_xConstFloatToReal(32000.0f);
	if(xResult<MTH_M_xConstFloatToReal(-32000.0f))
		xResult=MTH_M_xConstFloatToReal(-32000.0f);

	return (long)M_Fixed1616ToRealSnd(MTH_M_xRealToFixed16_16(xResult));
}

long __cdecl RetMicroSonoreVY(long indice)
{
	struct tdstEngineObject_	*p_Object;
	struct HIE_tdstSuperObject	*hSuperObj;
	DNM_tdstDynamics			*p_stDynamics;
	MTH_tdxReal					xResult = MTH_C_ZERO;

	p_Object  = fn_hReturnHandleMicroFromIndex(indice);
	if (p_Object)
	{
		hSuperObj = M_GetSuperObject( p_Object );
		if( !M_GetMSHandle(hSuperObj,Dynam) )
			return 0;
		if( (p_stDynamics = fn_p_stDynamGetDynamics(M_GetMSHandle(hSuperObj,Dynam))) == NULL )
			return 0;

		if (DNM_M_lDynamicsGetObjectType(p_stDynamics)==DNM_eCamera)
		{
			MTH_tdxReal xInvDTInSecond;

			xInvDTInSecond = MTH_M_xDiv(MTH_M_xConstFloatToReal(1000.f),MTH_M_xLongToReal(GAM_g_ulUsefulDeltaTime));

			xResult = MTH_M_xMul(MTH3D_M_xGetYofVector(DNM_M_p_stReportGetLastMove(DNM_M_p_stDynamicsGetReport(p_stDynamics))),xInvDTInSecond);
		}
		else
		{
			xResult = MTH3D_M_xGetYofVector(DNM_M_pDynamicsGetPreviousSpeed(p_stDynamics));
		}
	}

	if(xResult>MTH_M_xConstFloatToReal(32000.0f))
		xResult=MTH_M_xConstFloatToReal(32000.0f);
	if(xResult<MTH_M_xConstFloatToReal(-32000.0f))
		xResult=MTH_M_xConstFloatToReal(-32000.0f);

	return (long)M_Fixed1616ToRealSnd(MTH_M_xRealToFixed16_16(xResult));
}

long __cdecl RetMicroSonoreVZ(long indice)
{
	struct tdstEngineObject_	*p_Object;
	struct HIE_tdstSuperObject	*hSuperObj;
	DNM_tdstDynamics			*p_stDynamics;
	MTH_tdxReal					xResult = MTH_C_ZERO;

	p_Object  = fn_hReturnHandleMicroFromIndex(indice);
	if (p_Object)
	{
		hSuperObj = M_GetSuperObject( p_Object );
		if( !M_GetMSHandle(hSuperObj,Dynam) )
			return 0;
		if( (p_stDynamics = fn_p_stDynamGetDynamics(M_GetMSHandle(hSuperObj,Dynam))) == NULL )
			return 0;

		if (DNM_M_lDynamicsGetObjectType(p_stDynamics)==DNM_eCamera)
		{
			MTH_tdxReal xInvDTInSecond;

			xInvDTInSecond = MTH_M_xDiv(MTH_M_xConstFloatToReal(1000.f),MTH_M_xLongToReal(GAM_g_ulUsefulDeltaTime));

			xResult = MTH_M_xMul(MTH3D_M_xGetZofVector(DNM_M_p_stReportGetLastMove(DNM_M_p_stDynamicsGetReport(p_stDynamics))),xInvDTInSecond);
		}
		else
		{
			xResult = MTH3D_M_xGetZofVector(DNM_M_pDynamicsGetPreviousSpeed(p_stDynamics));
		}
	}

	if(xResult>MTH_M_xConstFloatToReal(32000.0f))
		xResult=MTH_M_xConstFloatToReal(32000.0f);
	if(xResult<MTH_M_xConstFloatToReal(-32000.0f))
		xResult=MTH_M_xConstFloatToReal(-32000.0f);

	return (long)M_Fixed1616ToRealSnd(MTH_M_xRealToFixed16_16(xResult));
}

/* 0x00411770 */
long __cdecl RetMicroSonoreDNX(long indice)
{
	MTH3D_tdstVector stV2,stV1,stV3,stW1,stW2,stW3;
	struct tdstEngineObject_ *p_Object;
	struct HIE_tdstSuperObject *hSuperObj;
	POS_tdstCompletePosition *pMatrix1,*pMatrix2;
	MTH_tdxReal xN;

	p_Object = fn_hReturnHandleMicroFromIndex(indice);
	hSuperObj = M_GetSuperObject( p_Object );
	pMatrix1=HIE_fn_hGetSuperObjectGlobalMatrix(hSuperObj);
	pMatrix2=MS_M_hMicroMatrixOf(hSuperObj);
	POS_fn_vGetRotationMatrix(pMatrix1,&stV1,&stV2,&stV3);
	POS_fn_vGetRotationMatrix(pMatrix2,&stW1,&stW2,&stW3);
	xN=MTH_M_xAdd(
		MTH_M_xAdd(
			MTH_M_xMul(stV1.xX,stW1.xX),MTH_M_xMul(stV2.xX,stW1.xY)),
		MTH_M_xMul(stV3.xX,stW1.xZ));
	return (long)M_Fixed1616ToRealSnd(MTH_M_xRealToFixed16_16(xN));
}

long __cdecl RetMicroSonoreDNY(long indice)
{
	MTH3D_tdstVector stV1,stV2,stV3,stW1,stW2,stW3;
	struct tdstEngineObject_ *p_Object;
	struct HIE_tdstSuperObject *hSuperObj;
	POS_tdstCompletePosition *pMatrix1,*pMatrix2;
	MTH_tdxReal xN;

	p_Object = fn_hReturnHandleMicroFromIndex(indice);
	hSuperObj = M_GetSuperObject( p_Object );
	pMatrix1=HIE_fn_hGetSuperObjectGlobalMatrix(hSuperObj);
	pMatrix2=MS_M_hMicroMatrixOf(hSuperObj);
	POS_fn_vGetRotationMatrix(pMatrix1,&stV1,&stV2,&stV3);
	POS_fn_vGetRotationMatrix(pMatrix2,&stW1,&stW2,&stW3);
	xN=MTH_M_xAdd(
		MTH_M_xAdd(
			MTH_M_xMul(stV1.xY,stW1.xX),MTH_M_xMul(stV2.xY,stW1.xY)),
		MTH_M_xMul(stV3.xY,stW1.xZ));
	return (long)M_Fixed1616ToRealSnd(MTH_M_xRealToFixed16_16(xN));
}

long __cdecl RetMicroSonoreDNZ(long indice)
{
	MTH3D_tdstVector stV2,stV1,stV3,stW1,stW2,stW3;
	struct tdstEngineObject_ *p_Object;
	struct HIE_tdstSuperObject *hSuperObj;
	POS_tdstCompletePosition *pMatrix1,*pMatrix2;
	MTH_tdxReal xN;

	p_Object = fn_hReturnHandleMicroFromIndex(indice);
	hSuperObj = M_GetSuperObject( p_Object );
	pMatrix1=HIE_fn_hGetSuperObjectGlobalMatrix(hSuperObj);
	pMatrix2=MS_M_hMicroMatrixOf(hSuperObj);
	POS_fn_vGetRotationMatrix(pMatrix1,&stV1,&stV2,&stV3);
	POS_fn_vGetRotationMatrix(pMatrix2,&stW1,&stW2,&stW3);
	xN=MTH_M_xAdd(
		MTH_M_xAdd(
			MTH_M_xMul(stV1.xZ,stW1.xX),MTH_M_xMul(stV2.xZ,stW1.xY)),
		MTH_M_xMul(stV3.xZ,stW1.xZ));
	return (long)M_Fixed1616ToRealSnd(MTH_M_xRealToFixed16_16(xN));
}

long __cdecl RetMicroSonoreDTX(long indice)
{
	MTH3D_tdstVector stV1,stV2,stV3,stW1,stW2,stW3;
	struct tdstEngineObject_ *p_Object;
	struct HIE_tdstSuperObject *hSuperObj;
	POS_tdstCompletePosition *pMatrix1,*pMatrix2;
	MTH_tdxReal xN;

	p_Object = fn_hReturnHandleMicroFromIndex(indice);
	hSuperObj = M_GetSuperObject( p_Object );
	pMatrix1=HIE_fn_hGetSuperObjectGlobalMatrix(hSuperObj);
	pMatrix2=MS_M_hMicroMatrixOf(hSuperObj);
	POS_fn_vGetRotationMatrix(pMatrix1,&stV1,&stV2,&stV3);
	POS_fn_vGetRotationMatrix(pMatrix2,&stW1,&stW2,&stW3);
	xN=MTH_M_xAdd(
		MTH_M_xAdd(
			MTH_M_xMul(stV1.xX,stW2.xX),MTH_M_xMul(stV2.xX,stW2.xY)),
		MTH_M_xMul(stV3.xX,stW2.xZ));
	return (long)M_Fixed1616ToRealSnd(MTH_M_xRealToFixed16_16(xN));
}

long __cdecl RetMicroSonoreDTY(long indice)
{
	MTH3D_tdstVector stV1,stV2,stV3,stW1,stW2,stW3;
	struct tdstEngineObject_ *p_Object;
	struct HIE_tdstSuperObject *hSuperObj;
	POS_tdstCompletePosition *pMatrix1,*pMatrix2;
	MTH_tdxReal xN;

	p_Object = fn_hReturnHandleMicroFromIndex(indice);
	hSuperObj = M_GetSuperObject( p_Object );
	pMatrix1=HIE_fn_hGetSuperObjectGlobalMatrix(hSuperObj);
	pMatrix2=MS_M_hMicroMatrixOf(hSuperObj);
	POS_fn_vGetRotationMatrix(pMatrix1,&stV1,&stV2,&stV3);
	POS_fn_vGetRotationMatrix(pMatrix2,&stW1,&stW2,&stW3);
	xN=MTH_M_xAdd(
		MTH_M_xAdd(
			MTH_M_xMul(stV1.xY,stW2.xX),MTH_M_xMul(stV2.xY,stW2.xY)),
		MTH_M_xMul(stV3.xY,stW2.xZ));
	return (long)M_Fixed1616ToRealSnd(MTH_M_xRealToFixed16_16(xN));
}

long __cdecl RetMicroSonoreDTZ(long indice)
{
	MTH3D_tdstVector stV1,stV2,stV3,stW1,stW2,stW3;
	struct tdstEngineObject_ *p_Object;
	struct HIE_tdstSuperObject *hSuperObj;
	POS_tdstCompletePosition *pMatrix1,*pMatrix2;
	MTH_tdxReal xN;

	p_Object = fn_hReturnHandleMicroFromIndex(indice);
	hSuperObj = M_GetSuperObject( p_Object );
	pMatrix1=HIE_fn_hGetSuperObjectGlobalMatrix(hSuperObj);
	pMatrix2=MS_M_hMicroMatrixOf(hSuperObj);
	POS_fn_vGetRotationMatrix(pMatrix1,&stV1,&stV2,&stV3);
	POS_fn_vGetRotationMatrix(pMatrix2,&stW1,&stW2,&stW3);
	xN=MTH_M_xAdd(
		MTH_M_xAdd(
			MTH_M_xMul(stV1.xZ,stW2.xX),MTH_M_xMul(stV2.xZ,stW2.xY)),
		MTH_M_xMul(stV3.xZ,stW2.xZ));
	return (long)M_Fixed1616ToRealSnd(MTH_M_xRealToFixed16_16(xN));
}

/* 0x00411AD0 */
void __cdecl fn_vAddMicro(struct tdstEngineObject_ *hEngObj)
{
    MS_pc_tdstMicroElem *hMicro;

    hMicro = (MS_pc_tdstMicroElem *)fnp_vGameMallocInHLM(sizeof(MS_pc_tdstMicroElem));

    hMicro->hNext = NULL;
    hMicro->hPrev = NULL;
    hMicro->hAnchor = NULL;

    hMicro->lIndex = SND_fn_lCreateMicroSound();
    hMicro->hMicroObj = hEngObj;

    if (hMicro->hNext != NULL)
    {
        hMicro->hNext->hPrev = hMicro->hPrev;
    }
    if (hMicro->hPrev != NULL)
    {
        hMicro->hPrev->hNext = hMicro->hNext;
    }
    if (hMicro->hAnchor != NULL)
    {
        MS_pc_M_pAnchorOf(hMicro)->lCount--;
        if (MS_pc_M_pAnchorOf(hMicro)->hFirst == hMicro)
        {
            MS_pc_M_pAnchorOf(hMicro)->hFirst = hMicro->hNext;
        }
        if (MS_pc_M_pAnchorOf(hMicro)->hLast == hMicro)
        {
            MS_pc_M_pAnchorOf(hMicro)->hLast = hMicro->hPrev;
        }
    }
    hMicro->hNext = NULL;
    hMicro->hPrev = NULL;
    hMicro->hAnchor = NULL;

    hMicro->hNext = NULL;
    hMicro->hPrev = GAM_g_stIndexMicrosList_hLast;
    hMicro->hAnchor = (void *)&GAM_g_stIndexMicrosList_hFirst;
    if (GAM_g_stIndexMicrosList_hFirst == NULL)
    {
        GAM_g_stIndexMicrosList_hFirst = hMicro;
    }
    else
    {
        ((MS_pc_tdstMicroElem *)GAM_g_stIndexMicrosList_hLast)->hNext = hMicro;
    }
    GAM_g_stIndexMicrosList_lCount = GAM_g_stIndexMicrosList_lCount + 1;
    GAM_g_stIndexMicrosList_hLast = hMicro;

    ((struct MS_pc_stMicroActive *)((struct tdstEngineObject_ *)hEngObj)->h_Micro)->bIsActive = 1;
}

/* 0x00411B80 */
void __cdecl fn_vRemoveAllMicros(void)
{
    MS_pc_tdstMicroElem *hMicro;
    MS_pc_tdstMicroElem *hNext;

    hMicro = (MS_pc_tdstMicroElem *)GAM_g_stIndexMicrosList_hFirst;
    hNext = (hMicro != NULL) ? hMicro->hNext : NULL;
    while (hMicro != NULL)
    {
        SND_fn_vDestroyMicroSound(hMicro->lIndex);

        if (hMicro->hNext != NULL)
        {
            hMicro->hNext->hPrev = hMicro->hPrev;
        }
        if (hMicro->hPrev != NULL)
        {
            hMicro->hPrev->hNext = hMicro->hNext;
        }
        if (hMicro->hAnchor != NULL)
        {
            MS_pc_M_pAnchorOf(hMicro)->lCount--;
            if (MS_pc_M_pAnchorOf(hMicro)->hFirst == hMicro)
            {
                MS_pc_M_pAnchorOf(hMicro)->hFirst = hMicro->hNext;
            }
            if (MS_pc_M_pAnchorOf(hMicro)->hLast == hMicro)
            {
                MS_pc_M_pAnchorOf(hMicro)->hLast = hMicro->hPrev;
            }
        }
        hMicro->hNext = NULL;
        hMicro->hPrev = NULL;
        hMicro->hAnchor = NULL;

        hMicro = hNext;
        hNext = (hNext != NULL) ? hNext->hNext : NULL;
    }

    GAM_g_stIndexMicrosList_hFirst = NULL;
    GAM_g_stIndexMicrosList_hLast  = NULL;
    GAM_g_stIndexMicrosList_lCount = 0;
}

/* 0x00411c20 */
void __cdecl fn_vInitMicrosFunction(void)
{
   tdstAllRetMicroSound st_pfnRetMicroSound;

   memset(&st_pfnRetMicroSound,0,sizeof(st_pfnRetMicroSound));
   st_pfnRetMicroSound.X=RetMicroSonoreX;
   st_pfnRetMicroSound.Y=RetMicroSonoreY;
   st_pfnRetMicroSound.Z=RetMicroSonoreZ;
   st_pfnRetMicroSound.VX=RetMicroSonoreVX;
   st_pfnRetMicroSound.VY=RetMicroSonoreVY;
   st_pfnRetMicroSound.VZ=RetMicroSonoreVZ;
   st_pfnRetMicroSound.DNX=RetMicroSonoreDNX;
   st_pfnRetMicroSound.DNY=RetMicroSonoreDNY;
   st_pfnRetMicroSound.DNZ=RetMicroSonoreDNZ;
   st_pfnRetMicroSound.DTX=RetMicroSonoreDTX;
   st_pfnRetMicroSound.DTY=RetMicroSonoreDTY;
   st_pfnRetMicroSound.DTZ=RetMicroSonoreDTZ;
   st_pfnRetMicroSound.Filtre=RetMicroSonoreFILTRE;

   SND_fn_vSetRetMicroSound(&st_pfnRetMicroSound);

    GAM_g_stIndexMicrosList_hFirst = 0;
    GAM_g_stIndexMicrosList_hLast  = 0;
    GAM_g_stIndexMicrosList_lCount = 0;
}

void __cdecl GAM_fn_vChangeCharacterForMicro(void *_hCharacter);

/* 0x00411CC0 */
void *__cdecl fn_p_stGetFirstEngineObjectInMicroList(void)
{
    MS_pc_tdstMicroElem *hMicro = (MS_pc_tdstMicroElem *)GAM_g_stIndexMicrosList_hFirst;

    if (hMicro != NULL)
        return hMicro->hMicroObj;
    return NULL;
}

/* 0x00411CD0 */
void __cdecl GAM_fn_vChangeCharacterForMicro(void *_hCharacter)
{
    void *hEngineObject = HIE_fn_hGetSuperObjectObject((HIE_tdstSuperObject *)_hCharacter);
    ((MS_pc_tdstMicroElem *)GAM_g_stIndexMicrosList_hFirst)->hMicroObj = hEngineObject;
}
