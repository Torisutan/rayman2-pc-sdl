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

#include "DNM/DNM_types.h"
#include "GMT/GMT_types.h"
#include "TMP/TmpMem.h"

extern void *__cdecl fnp_vGameMallocInHLM(unsigned int ulSize);  /* 0x004077E0 */

#define DNM_M_bDynamicsIsAlways_CPA(p)          ((DNM_M_ulEndFlags(p) & DNM_C_Always) ? 1 : 0)
#define DNM_M_bDynamicsIsCollisionReport_CPA(p) ((DNM_M_ulEndFlags(p) & DNM_C_CollisionReport) ? 1 : 0)

#define DNM_M_p_stDynamicsGetReportWithoutCheckAccess(p) DNM_M_p_stDynamicsGetReport(p)
#define M_GameFreeInHLM(p_vPointer) { fn_vGameFreeInHLM((void *)(p_vPointer)); p_vPointer = NULL; }

void __cdecl DNM_fn_vDynamicsCopyClone ( DNM_tdstDynamics *_p_stDst, DNM_tdstDynamics *_p_stSrc );

/* 0x00412BD0 */
unsigned long __cdecl fn_ulDynamSizeOf(void)
{
    return sizeof(struct tdstDynam_);
}

/* 0x00419B90 */
void *__cdecl fn_p_stDynamGetDynamics(MS_tdxHandleToDynam h_Dynam)
{
    return(h_Dynam->p_stDynamics);
}

void __cdecl fn_vDynamSetDynamics(MS_tdxHandleToDynam h_Dynam, void *p_stDynam)
{
    h_Dynam->p_stDynamics = (DNM_tdstDynamics *)p_stDynam;
}

void *__cdecl fn_p_stDynamGetParsingDatas(MS_tdxHandleToDynam h_Dynam)
{
    return(h_Dynam->p_stParsingDatas);
}

void __cdecl fn_vDynamSetParsingDatas(MS_tdxHandleToDynam h_Dynam, void *p_stParsingDatas)
{
    h_Dynam->p_stParsingDatas = (DNM_tdstParsingDatas *)p_stParsingDatas;
}

#define FN_DYN_SIZE_BASE       0x154
#define FN_DYN_SIZE_ADVANCED   0x1DC
#define FN_DYN_SIZE_COMPLEX    0x338
#define FN_DYN_SIZE_REPORT     0x16C

#define FN_DYN_SET_REPORT(p, r) (DNM_M_p_stDynamicsGetReport(p) = (r))

#define FN_DYN_SET_COLLISION_REPORT(p, b) \
    (DNM_M_ulEndFlags(p) = ((b) ? (DNM_M_ulEndFlags(p) |= DNM_C_CollisionReport) \
                                : (DNM_M_ulEndFlags(p) &= ~DNM_C_CollisionReport)))

#define FN_DYN_SET_ALWAYS(p, b) \
    (DNM_M_ulEndFlags(p) = ((b) ? (DNM_M_ulEndFlags(p) |= DNM_C_Always) \
                                : (DNM_M_ulEndFlags(p) &= ~DNM_C_Always)))

#define FN_DYN_SET_BASE_SIZE(p) \
    { DNM_M_ulEndFlags(p) &= ~(DNM_C_BaseSize|DNM_C_AdvancedSize|DNM_C_ComplexSize); \
      DNM_M_ulEndFlags(p) |= DNM_C_BaseSize; }
#define FN_DYN_SET_ADVANCED_SIZE(p) \
    { DNM_M_ulEndFlags(p) &= ~(DNM_C_BaseSize|DNM_C_AdvancedSize|DNM_C_ComplexSize); \
      DNM_M_ulEndFlags(p) |= (DNM_C_AdvancedSize|DNM_C_BaseSize); }
#define FN_DYN_SET_COMPLEX_SIZE(p) \
    { DNM_M_ulEndFlags(p) |= (DNM_C_ComplexSize|DNM_C_AdvancedSize|DNM_C_BaseSize); }
#include "ERM/CpaError.h"
#include "GAM/GAM_types.h"
void __cdecl fn_vInitOneObject(struct tdstEngineObject_ *p_stObject, int eObjIniType);
extern void __cdecl fn_v_DynamicFree(void *param_1);
void __cdecl fn_vGameFreeInHLM(void *p_vObject);
extern void __cdecl DNM_fn_vInitBaseMechanic(void *_p_stDynamic);

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

/* 0x00412C70 */
void *__cdecl fn_h_DynamRealAlloc(void)
{
    void *h_Dynam;

    h_Dynam = fnp_vGameMallocInHLM((unsigned int)fn_ulDynamSizeOf());
    fn_vDynamSetParsingDatas((MS_tdxHandleToDynam)h_Dynam,
                             fnp_vGameMallocInHLM((unsigned int)sizeof(DNM_tdstParsingDatas)));
    return h_Dynam;
}

/* 0x00412CA0 */
void __cdecl fn_v_DynamicAlloc(void *h_Dynam, eDynamicSize eSize,
                               ACP_tdxBool bCollision, ACP_tdxBool bAlways)
{
    DNM_tdstDynamics *p_stDynamic = NULL;

    if (bAlways)
    {
        switch (eSize)
        {
        case eDynamicSizeBase :
            p_stDynamic = (DNM_tdstDynamics *)TMP_M_p_Malloc(FN_DYN_SIZE_BASE);
            FN_DYN_SET_BASE_SIZE(p_stDynamic);
            break;
        case eDynamicSizeAdvanced :
            p_stDynamic = (DNM_tdstDynamics *)TMP_M_p_Malloc(FN_DYN_SIZE_ADVANCED);
            FN_DYN_SET_ADVANCED_SIZE(p_stDynamic);
            break;
        case eDynamicSizeComplex :
            p_stDynamic = (DNM_tdstDynamics *)TMP_M_p_Malloc(FN_DYN_SIZE_COMPLEX);
            FN_DYN_SET_COMPLEX_SIZE(p_stDynamic);
            break;
        }
    }
    else
    {
        switch (eSize)
        {
        case eDynamicSizeBase :
            p_stDynamic = (DNM_tdstDynamics *)fnp_vGameMallocInHLM(FN_DYN_SIZE_BASE);
            FN_DYN_SET_BASE_SIZE(p_stDynamic);
            break;
        case eDynamicSizeAdvanced :
            p_stDynamic = (DNM_tdstDynamics *)fnp_vGameMallocInHLM(FN_DYN_SIZE_ADVANCED);
            FN_DYN_SET_ADVANCED_SIZE(p_stDynamic);
            break;
        case eDynamicSizeComplex :
            p_stDynamic = (DNM_tdstDynamics *)fnp_vGameMallocInHLM(FN_DYN_SIZE_COMPLEX);
            FN_DYN_SET_COMPLEX_SIZE(p_stDynamic);
            break;
        }
    }

    fn_vDynamSetDynamics((MS_tdxHandleToDynam)h_Dynam, p_stDynamic);
    FN_DYN_SET_ALWAYS(p_stDynamic, bAlways);

    if (bCollision)
    {
        if (bAlways)
            FN_DYN_SET_REPORT(p_stDynamic,
                              (DNM_tdstReport *)TMP_M_p_Malloc(FN_DYN_SIZE_REPORT));
        else
            FN_DYN_SET_REPORT(p_stDynamic,
                              (DNM_tdstReport *)fnp_vGameMallocInHLM(FN_DYN_SIZE_REPORT));
    }
    else
        FN_DYN_SET_REPORT(p_stDynamic, NULL);

    FN_DYN_SET_COLLISION_REPORT(p_stDynamic, bCollision);
}

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

void __cdecl fn_vDynamFree(void *p_pOwner)
{
    if (p_pOwner == (void *)0)
    {
        ERM_fn_vClearLastError(0);
        ERM_fn_vUpdateLastError(0x30, g_ucGameModuleId, 0, 0xe, -1, 0xff, 0xff, (char *)0);
    }
    if (((struct tdstEngineObject_ *)p_pOwner)->h_Dynam == (void *)0)
    {
        ERM_fn_vClearLastError(0);
        ERM_fn_vUpdateLastError(0x30, g_ucGameModuleId, 0, 0xf, -1, 0xff, 0xff, (char *)0);
    }
    fn_v_DynamicFree(((struct tdstEngineObject_ *)p_pOwner)->h_Dynam);
    fn_vGameFreeInHLM(((struct tdstEngineObject_ *)p_pOwner)->h_Dynam);
    ((struct tdstEngineObject_ *)p_pOwner)->h_Dynam = (void *)0;
}

void __cdecl fn_vDynamInit(struct tdstEngineObject_ *p_stObject, int eObjIniType)
{

    if (eObjIniType >= 0 && eObjIniType <= 4)
    {
        DNM_fn_vInitBaseMechanic(((MS_tdxHandleToDynam)p_stObject->h_Dynam)->p_stDynamics);
        memset(((MS_tdxHandleToDynam)p_stObject->h_Dynam)->p_stParsingDatas, 0, 7 * 4);
    }
}

void __cdecl fn_vDynamDesinit(void *p_pOwner, long lMode)
{
    void *pHolder;
    DNM_tdstDynamics *p_stDynamics;

    if (p_pOwner == (void *)0)
    {
        ERM_fn_vClearLastError(0);
        ERM_fn_vUpdateLastError(0x30, g_ucGameModuleId, 0, 0xe, -1, 0xff, 0xff, (char *)0);
    }
    if (((struct tdstEngineObject_ *)p_pOwner)->h_Dynam == (void *)0)
    {
        ERM_fn_vClearLastError(0);
        ERM_fn_vUpdateLastError(0x30, g_ucGameModuleId, 0, 0xf, -1, 0xff, 0xff, (char *)0);
    }
    switch (lMode)
    {
    case 1:
        pHolder = ((struct tdstEngineObject_ *)p_pOwner)->h_Dynam;
        p_stDynamics = ((MS_tdxHandleToDynam)pHolder)->p_stDynamics;
        if ((DNM_M_ulEndFlags(p_stDynamics) & DNM_C_Always) != 0)
            fn_v_DynamicFree(pHolder);
        break;
    }
}

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

/* 0x00413100 */
void __cdecl fn_vDynamSetUsedMechanics(MS_tdxHandleToDynam h_Dynam, DNM_tdeMechanicsID eUsedMechanics)
{
    h_Dynam->eUsedMechanics = eUsedMechanics;
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
