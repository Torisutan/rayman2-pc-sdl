#include "AI/AI_types.h"
#include "GAM/MSMag.h"
#include "GAM/GAM_types.h"

#define AI_M_pfn_fn_ucGetFunctionUltraOperator  fn_ucGetFunctionUltraOperator
#define AI_M_pfn_fn_xMSMagnetGetStrength   ((float (__cdecl *)(const void *))fn_xMSMagnetGetStrength)
#define AI_M_pfn_fn_xMSMagnetGetFar        ((float (__cdecl *)(const void *))fn_xMSMagnetGetFar)
#define AI_M_pfn_fn_xMSMagnetGetNear       ((float (__cdecl *)(const void *))fn_xMSMagnetGetNear)
#define AI_M_pfn_fn_ulMSMagnetGetDuration  ((unsigned long (__cdecl *)(const void *))fn_ulMSMagnetGetDuration)

extern void __cdecl fn_vOnAIErm_M_UpdateLastError(unsigned short FatalErrorNum, char *szMsg);
#define AI_M_pfn_M_AIFatalError(uwError) \
    fn_vOnAIErm_M_UpdateLastError((unsigned short)(uwError), (char *)0)
extern void __cdecl MGT_fn_vAddObjectToMagnetList(HIE_tdstSuperObject *p_stObject);

/* 0x00488220 */
AI_tdstNodeInterpret *__cdecl MGT_fn_p_stActiveMagnet(
    HIE_tdstSuperObject *p_SuperObjPerso, AI_tdstNodeInterpret *p_stTree)
{
    unsigned long        procId  = (unsigned long)M_eProcedureIdInterpret(p_stTree-1);

    fn_vGetUltraOperatorPerso(fn_ucGetProcedureUltraOperator(procId), p_SuperObjPerso,
                                 &p_SuperObjPerso);

    MGT_fn_vAddObjectToMagnetList(p_SuperObjPerso);
    return p_stTree;
}

/* 0x004883B0 */
AI_tdstNodeInterpret *__cdecl MGT_fn_p_stGetInfo(
    HIE_tdstSuperObject *p_SuperObjPerso, AI_tdstNodeInterpret *p_stTree, AI_tdstGetSetParam *p_stValue)
{
    unsigned long         eFuncId = *(unsigned long *)((char *)p_stTree - 8);
    HIE_tdstSuperObject  *hNewSuperObjPerso;
    const void           *hMagnet;
    AI_tdstGetSetParam    stValue;
    AI_tdstNodeInterpret *p_stNext;

    fn_vGetUltraOperatorPerso(
        AI_M_pfn_fn_ucGetFunctionUltraOperator(eFuncId), p_SuperObjPerso, &hNewSuperObjPerso);

    hMagnet = M_GetMSHandle(hNewSuperObjPerso,MSMagnet);
    if (hMagnet == (const void *)0)
        return p_stTree;

    p_stNext = fn_p_stEvalTree(p_SuperObjPerso, p_stTree, &stValue);
    switch (eFuncId)
    {
    case 0x37:
        p_stValue->uParam.xValue = AI_M_pfn_fn_xMSMagnetGetStrength(hMagnet);
        p_stValue->eType         = 8;
        break;
    case 0x38:
        p_stValue->uParam.xValue = AI_M_pfn_fn_xMSMagnetGetFar(hMagnet);
        p_stValue->eType         = 8;
        break;
    case 0x39:
        p_stValue->uParam.xValue = AI_M_pfn_fn_xMSMagnetGetNear(hMagnet);
        p_stValue->eType         = 8;
        break;
    case 0x3a:
        p_stValue->uParam.ulValue = AI_M_pfn_fn_ulMSMagnetGetDuration(hMagnet);
        p_stValue->eType          = 7;

    default:
        AI_M_pfn_M_AIFatalError(0x27);
        break;
    }
    return p_stNext;
}
