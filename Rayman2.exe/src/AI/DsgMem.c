#include "AI/AI_types.h"
#include "GAM/GAM_types.h"
#include "AI/Brain.h"
#include "AI/DsgMem.h"
#include "AI/DsgVarTable.h"

/* 0x00481D70 */
extern unsigned char __cdecl fn_ucGetFieldValue(unsigned long ulFieldId, void *p_stContext,
                                                AI_tdstGetSetParam *p_stValue);
extern void __cdecl fn_vOnAIErm_M_UpdateLastError(unsigned short FatalErrorNum, char *szMsg);

/* 0x004824F0 */
extern unsigned char __cdecl fn_ucSetFieldValue(unsigned long ulFieldId, void *p_stContext,
                                                AI_tdstGetSetParam *p_stValue);

unsigned char fn_ucGetDsgVarValue(void *pMyVarAddr,unsigned char ucIndex,tdeDsgVarTypeId eDsgVarTypeId,AI_tdstGetSetParam *p_stGetSetParam);

/* 0x004815F0 */
unsigned char __cdecl fn_ucGetDsgVar(unsigned char ucId, unsigned char ucIndex,
                                     AI_tdstMind *p_stMind, AI_tdstGetSetParam *p_stGetSetParam)
{
    return (fn_ucGetDsgVarValue((void *)M_GetDsgVarAddr(p_stMind, ucId), ucIndex,
                                M_GetDsgVarType(p_stMind, ucId), p_stGetSetParam));
}

/* 0x00481630 */
unsigned char fn_ucGetDsgVarValue(void *pMyVarAddr,unsigned char ucIndex,tdeDsgVarTypeId eDsgVarTypeId,AI_tdstGetSetParam *p_stGetSetParam)
{
	tdp_fn_ucGetDsgVarFunctionPtr p_fn_ucGetDsgVar=fn_p_fn_pGetDsgVarGetFunctionPtr(eDsgVarTypeId);
	return(p_fn_ucGetDsgVar(pMyVarAddr,ucIndex,p_stGetSetParam));
}

tdp_fn_ucSetDsgVarFunctionPtr fn_p_fn_pGetDsgVarSetFunctionPtr(enum tdeDsgVarTypeId_ eDsgVarTypeId);

/* 0x00481800 */
unsigned char __cdecl fn_ucSetDsgVar(unsigned char ucId, unsigned char ucIndex,
                                     AI_tdstMind *p_stPerso, AI_tdstGetSetParam *p_stGetSetParam)
{
	tdeDsgVarTypeId eDsgVarTypeId=M_GetDsgVarType(p_stPerso,ucId);
	tdp_fn_ucSetDsgVarFunctionPtr p_fn_ucSetDsgVar=fn_p_fn_pGetDsgVarSetFunctionPtr(eDsgVarTypeId);
    void *pMyVarAddr=(void*) M_GetDsgVarAddr(p_stPerso,ucId);

	return(p_fn_ucSetDsgVar(pMyVarAddr,ucIndex,p_stGetSetParam));
}

/* 0x00481CB0 */
void __cdecl fn_vOperatorGetDsgVar(unsigned char ucVarId, unsigned char ucIndex,
                                   HIE_tdstSuperObject *hSuperObjPerso, AI_tdstGetSetParam *p_stValue)
{
   if (fn_ucGetDsgVar(ucVarId, ucIndex, AI_M_stGetMindOfSuperObj(hSuperObjPerso), p_stValue)==C_INVALID_GET)
   {
   }
}

void __cdecl fn_vOperatorGetField(unsigned long ulFieldId, void *p_stContext,
                                  void *p_stGetSetParam)
{
    if (fn_ucGetFieldValue(ulFieldId, p_stContext, (AI_tdstGetSetParam *)p_stGetSetParam) == 0)
        fn_vOnAIErm_M_UpdateLastError(0x22, (char *)0);
}

/* 0x00482420 */
void __cdecl fn_vOperatorSetDsgVar(unsigned char ucVarId, unsigned char ucIndex,
                                   HIE_tdstSuperObject *hSuperObjPerso, AI_tdstGetSetParam *p_stValue)
{
   if (fn_ucSetDsgVar(ucVarId, ucIndex, AI_M_stGetMindOfSuperObj(hSuperObjPerso), p_stValue)==C_INVALID_SET)
   {

   }
}

/* 0x00482450 */
void __cdecl fn_vGetFieldOrDsgVarForAffectOperator(void *p_stContext, unsigned long ulType,
                                                   unsigned long ulDsgVarId, unsigned long ulDsgVarIndex,
                                                   long lFieldId, void *p_stGetSetParam)
{
    if (ulType == 0x0B)
        fn_vOperatorGetDsgVar(ulDsgVarId, ulDsgVarIndex, p_stContext, p_stGetSetParam);
    else if (ulType == 9)
        fn_vOperatorGetField((unsigned long)lFieldId, p_stContext, p_stGetSetParam);
}

void __cdecl fn_vOperatorSetField(unsigned long ulFieldId, void *p_stContext, void *p_stGetSetParam);

/* 0x004824A0 */
void __cdecl fn_vSetFieldOrDsgVarForAffectOperator(void *p_stContext, unsigned long ulType,
                                                   unsigned long ulDsgVarId, unsigned long ulDsgVarIndex,
                                                   long lFieldId, void *p_stGetSetParam)
{
    if (ulType == 0x0B)
        fn_vOperatorSetDsgVar(ulDsgVarId, ulDsgVarIndex, p_stContext, p_stGetSetParam);
    else if (ulType == 9)
        fn_vOperatorSetField((unsigned long)lFieldId, p_stContext, p_stGetSetParam);
}

void __cdecl fn_vOperatorSetField(unsigned long ulFieldId, void *p_stContext,
                                  void *p_stGetSetParam)
{
    if (fn_ucSetFieldValue(ulFieldId, p_stContext, (AI_tdstGetSetParam *)p_stGetSetParam) == 0)
        fn_vOnAIErm_M_UpdateLastError(0x22, (char *)0);
}

/* 0x00482520 */
void __cdecl fn_vSetVectorComponentForAffectOperator(unsigned char ucXYZflag, MTH3D_tdstVector *p_stVector,
                                                     MTH_tdxReal xReal)
{
	switch(ucXYZflag)
	{
	  case 1:
		MTH3D_M_vSetXofVector(p_stVector,xReal);
		break;
	  case 2:
		MTH3D_M_vSetYofVector(p_stVector,xReal);
		break;
	  case 3:
  	    MTH3D_M_vSetZofVector(p_stVector,xReal);
	    break;
	}
}

/* 0x00482560 */
void __cdecl fn_vBuildVectorForAffectOperator(unsigned char ucXYZflag, AI_tdstGetSetParam *p_stParam,
                                              MTH_tdxReal xReal)
{
	MTH3D_tdstVector *p_stVector=&M_GetSetParam_stVertexValue(p_stParam);
	MTH3D_M_vNullVector(p_stVector);
	fn_vSetVectorComponentForAffectOperator(ucXYZflag,p_stVector,xReal);
	M_GetSetParam_Type(p_stParam)=E_vt_Vector;
}
