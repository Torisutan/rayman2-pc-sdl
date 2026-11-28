#include <stddef.h>
#include <string.h>

#include "DNM/DNM.h"
#include "HIE/HIE_types.h"
#include "MTH/MTH_flt.h"
#include "MTH/MTH_xNeg.h"
#include "MTH/MTH3D.h"
#include "POS/POS_types.h"
#include "ERM/CpaError.h"

extern unsigned char g_ucGameModuleId;

typedef unsigned char (__cdecl *t_fn_ucIsAnAlwaysObject)(HIE_tdstEngineObject *p_stObject);

extern void *__cdecl fn_h_DynamRealAlloc(void);  /* 0x00412C70 */
extern void __cdecl fn_v_DynamicAlloc(void *h_Dynam, int eSize,
                                      unsigned char bCollision, unsigned char bAlways);  /* 0x00412CA0 */
/* 0x00412DF0 */
void __cdecl fn_v_DynamicFree(MS_tdxHandleToDynam h_Dynam);
#define DNM_fn_v_DynamicFree         fn_v_DynamicFree
extern unsigned char __cdecl fn_ucIsAnAlwaysObject(void *p_stObject);
#define DNM_fn_ucIsAnAlwaysObject    ((t_fn_ucIsAnAlwaysObject)fn_ucIsAnAlwaysObject)

#define DNM_C_xCloneComplexBytes   0x338
#define DNM_C_xCloneAdvancedBytes  0x1DC
#define DNM_C_xCloneBaseBytes      0x154
#define DNM_C_xCloneReportBytes    sizeof(DNM_tdstReport)

#define DNM_fn_p_stDynamGetDynamics    fn_p_stDynamGetDynamics

/* 0x00412DF0 */
extern void __cdecl GLI_pc_fn_vFree(void **ppUser);
extern void __cdecl fn_vDynamSetDynamics(MS_tdxHandleToDynam h_Dynam, void *p_stDynam);  /* 0x0042DB20 */
extern void __cdecl fn_vGameFreeInHLM(void *p_vObject);

/* 0x00412BE0 */
void __cdecl fn_vDynamAlloc(HIE_tdstEngineObject *p_stObject)
{

    if (p_stObject == NULL) {

        ERM_fn_vClearLastError((unsigned long)p_stObject);
        ERM_fn_vUpdateLastError(0x30u, g_ucGameModuleId,
            (unsigned long)p_stObject, 0x0eu, -1,
            0xff, 0xff, (char *)p_stObject);
    }

    if (p_stObject->hDynam != NULL) {

        ERM_fn_vClearLastError(0);
        ERM_fn_vUpdateLastError(0x30u, g_ucGameModuleId,
            0, 0x0du, -1, 0xff, 0xff, (char *)0);
    }

    p_stObject->hDynam = fn_h_DynamRealAlloc();

    fn_v_DynamicAlloc(p_stObject->hDynam,
                      eDynamicSizeBase,
                      0,
                      DNM_fn_ucIsAnAlwaysObject(p_stObject));
}

#define DNM_M_bDynamicsIsAlways_CPA(p)          ((DNM_M_ulEndFlags(p) & DNM_C_Always) ? 1 : 0)
#define DNM_M_bDynamicsIsCollisionReport_CPA(p) ((DNM_M_ulEndFlags(p) & DNM_C_CollisionReport) ? 1 : 0)

#define DNM_M_p_stDynamicsGetReportWithoutCheckAccess(p) DNM_M_p_stDynamicsGetReport(p)
#define M_GameFreeInHLM(p_vPointer) { fn_vGameFreeInHLM((void *)(p_vPointer)); p_vPointer = NULL; }

void __cdecl fn_v_DynamicFree(MS_tdxHandleToDynam h_Dynam)
{
    DNM_tdstDynamics *p_stDynamic;

    if( (p_stDynamic = fn_p_stDynamGetDynamics(h_Dynam)) == NULL )
        return;

    if (DNM_M_bDynamicsIsAlways_CPA(p_stDynamic))
    {
        if (DNM_M_bDynamicsIsCollisionReport_CPA(p_stDynamic))
            GLI_pc_fn_vFree(&DNM_M_p_stDynamicsGetReportWithoutCheckAccess(p_stDynamic));
        GLI_pc_fn_vFree(&p_stDynamic);
    }
    else
    {
        if (DNM_M_bDynamicsIsCollisionReport_CPA(p_stDynamic))
            M_GameFreeInHLM (DNM_M_p_stDynamicsGetReportWithoutCheckAccess(p_stDynamic));
        M_GameFreeInHLM (p_stDynamic);
    }

    fn_vDynamSetDynamics( h_Dynam, NULL );
}

void __cdecl DNM_fn_vDynamicsCopyClone ( DNM_tdstDynamics *_p_stDst, DNM_tdstDynamics *_p_stSrc );

/* 0x00412FF0 */
void __cdecl fn_vDynamCopyClone(HIE_tdstEngineObject *_p_stObjectDest,
                                    HIE_tdstEngineObject *_p_stObjectSrc)
{
    MS_tdxHandleToDynam hDest, hSrc;
    DNM_tdstDynamics *p_stSrcDynamic, *p_stDestDynamic;
    eDynamicSize eSize;
    unsigned char bAlloc = 1;
    unsigned char bAlways;

    hDest = _p_stObjectDest->hDynam;
    hSrc  = _p_stObjectSrc->hDynam;
    p_stSrcDynamic  = hSrc ->p_stDynamics;
    p_stDestDynamic = hDest->p_stDynamics;
    if (!p_stSrcDynamic) return;

    bAlways = DNM_fn_ucIsAnAlwaysObject(_p_stObjectDest);

    if (DNM_M_bDynamicsIsComplexSize(p_stSrcDynamic))
        eSize = eDynamicSizeComplex;
    else if (DNM_M_bDynamicsIsAdvancedSize(p_stSrcDynamic))
        eSize = eDynamicSizeAdvanced;
    else
        eSize = eDynamicSizeBase;

    if (p_stDestDynamic) {

        switch (eSize) {
        case eDynamicSizeComplex:
            bAlloc = (unsigned char)!DNM_M_bDynamicsIsComplexSize(p_stDestDynamic);
            break;
        case eDynamicSizeAdvanced:
            bAlloc = (unsigned char)!DNM_M_bDynamicsIsAdvancedSize(p_stDestDynamic);
            break;
        case eDynamicSizeBase:
            bAlloc = (unsigned char)!DNM_M_bDynamicsIsBaseSize(p_stDestDynamic);
            break;
        }

        if (DNM_M_bDynamicsIsCollisionReport(p_stSrcDynamic) !=
            DNM_M_bDynamicsIsCollisionReport(p_stDestDynamic))
            bAlloc = 1;

        if (bAlloc) DNM_fn_v_DynamicFree(hDest);
    }

    if (bAlloc)
        fn_v_DynamicAlloc(hDest, (int)eSize,
            (unsigned char)DNM_M_bDynamicsIsCollisionReport(p_stSrcDynamic),
            bAlways);

    DNM_fn_vDynamicsCopyClone(hDest->p_stDynamics, hSrc->p_stDynamics);
    memcpy(hDest->p_stParsingDatas, hSrc->p_stParsingDatas,
           sizeof(DNM_tdstParsingDatas));
    hDest->eUsedMechanics = hSrc->eUsedMechanics;

    DNM_M_vDynamicsSetAlways(hDest->p_stDynamics, bAlways);
}

DNM_tdstDynamics * __cdecl fn_p_stDynamGetDynamicsSO(HIE_tdstSuperObject *hSupObj)
{
    void *h_Object;
    MS_tdxHandleToDynam h_Dynam;
    DNM_tdstDynamics *p_stDynamics = NULL;

    h_Object = HIE_fn_hGetSuperObjectObject(hSupObj);
    if (h_Object)
    {

        h_Dynam = ((HIE_tdstEngineObject *)h_Object)->hDynam;
        if (h_Dynam)
            p_stDynamics = (DNM_tdstDynamics *)DNM_fn_p_stDynamGetDynamics(h_Dynam);
    }

    return p_stDynamics;
}

void __cdecl DNM_fn_vDynamicsCopyClone ( DNM_tdstDynamics *_p_stDst, DNM_tdstDynamics *_p_stSrc )
{
	DNM_tdstReport* p_stReport=NULL;

	if (DNM_M_bDynamicsIsCollisionReport(_p_stDst))
	{

		p_stReport = DNM_M_p_stDynamicsGetReport (_p_stDst);
	}

	if (DNM_M_bDynamicsIsComplexSize(_p_stSrc)) memcpy ( _p_stDst, _p_stSrc, DNM_C_xCloneComplexBytes );
	else if (DNM_M_bDynamicsIsAdvancedSize(_p_stSrc)) memcpy ( _p_stDst, _p_stSrc, DNM_C_xCloneAdvancedBytes );
	else memcpy ( _p_stDst, _p_stSrc, DNM_C_xCloneBaseBytes );

	if (p_stReport)
	{
		DNM_M_p_stDynamicsGetReport(_p_stDst) = p_stReport;
		if (DNM_M_bDynamicsIsCollisionReport(_p_stSrc))
			memcpy ( p_stReport, DNM_M_p_stDynamicsGetReport(_p_stSrc), DNM_C_xCloneReportBytes );
	}
}
