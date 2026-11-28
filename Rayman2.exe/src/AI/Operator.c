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

#include <math.h>
#include "AI/AI_types.h"

static const unsigned long AI_OPER_xDivEpsilon_bits = 0x00800005u;
static const unsigned long AI_OPER_xOne_bits        = 0x3F800000u;
#define AI_OPER_xDivEpsilon  (*(const float *)&AI_OPER_xDivEpsilon_bits)
#define AI_OPER_xOne         (*(const float *)&AI_OPER_xOne_bits)

/* 0x00482F10 */
unsigned char __cdecl fn_ucAreThereFloatIfYesConvertThem(AI_tdstGetSetParam *p_stParam1,
                                                         AI_tdstGetSetParam *p_stParam2);
#define AI_M_pfn_fn_ucAreThereFloatIfYesConvertThem  fn_ucAreThereFloatIfYesConvertThem

#define AI_M_pfn_fn_p_stSkipThisArgument    fn_p_stSkipThisArgument

#define AI_M_pfn_fn_p_stOperatorGetPerso  fn_p_stOperatorGetPerso
/* 0x00481CB0 */
extern void __cdecl fn_vOperatorGetDsgVar(unsigned char ucId, unsigned char ucIndex,
                                          void *p_context, void *p_stGetSetParam);
#define AI_M_pfn_fn_vOperatorGetDsgVar  ((void (__cdecl *)(unsigned long, unsigned long, HIE_tdstSuperObject *, void *))&fn_vOperatorGetDsgVar)
/* 0x00481D70 */
extern void __cdecl fn_vOperatorGetField(unsigned long ulFieldId, void *p_stContext, void *p_stGetSetParam);
#define AI_M_pfn_fn_vOperatorGetField   ((void (__cdecl *)(unsigned long, HIE_tdstSuperObject *, void *))&fn_vOperatorGetField)

#define AI_M_pfn_fn_vSetUltraOperatorPerso   fn_vSetUltraOperatorPerso

/* 0x004810C0 */
extern unsigned char __cdecl fn_ucGetTreeNbParam(AI_tdstNodeInterpret *p_stTree);
#define AI_M_pfn_fn_ucGetTreeNbParam   ((char (__cdecl *)(AI_tdstNodeInterpret *))&fn_ucGetTreeNbParam)
/* 0x00482450 */
extern void __cdecl fn_vGetFieldOrDsgVarForAffectOperator(void *p_stContext, unsigned long ulType,
        unsigned long ulDsgVarId, unsigned long ulDsgVarIndex, long lFieldId, void *p_stGetSetParam);
#define AI_M_pfn_fn_vGetFieldOrDsgVarForAffectOperator           ((void (__cdecl *)(HIE_tdstSuperObject *, unsigned long, unsigned long, unsigned long, long, void *))&fn_vGetFieldOrDsgVarForAffectOperator)
/* 0x004824A0 */
extern void __cdecl fn_vSetFieldOrDsgVarForAffectOperator(void *p_stContext, unsigned long ulType,
        unsigned long ulDsgVarId, unsigned long ulDsgVarIndex, long lFieldId, void *p_stGetSetParam);
#define AI_M_pfn_fn_vSetFieldOrDsgVarForAffectOperator          ((void (__cdecl *)(HIE_tdstSuperObject *, unsigned long, unsigned long, unsigned long, long, void *))&fn_vSetFieldOrDsgVarForAffectOperator)
/* 0x00482420 */
extern void __cdecl fn_vOperatorSetDsgVar(unsigned long ulId, unsigned long ulIndex,
                                          void *p_context, void *p_stGetSetParam);
#define AI_M_pfn_fn_vOperatorSetDsgVar     ((void (__cdecl *)(unsigned long, unsigned long, HIE_tdstSuperObject *, void *))&fn_vOperatorSetDsgVar)
/* 0x00482520 */
extern void __cdecl fn_vSetVectorComponentForAffectOperator(unsigned char ucXYZflag, void *p_stVector, float xReal);
extern void __cdecl fn_vBuildVectorForAffectOperator(char cComponent, void *p_stValue, float xValue);
#define AI_M_pfn_fn_vSetVectorComponentForAffectOperator   ((void (__cdecl *)(char, void *, float))&fn_vSetVectorComponentForAffectOperator)
#define AI_M_pfn_fn_vBuildVectorForAffectOperator ((void (__cdecl *)(char, void *, float))&fn_vBuildVectorForAffectOperator)

/* 0x0049CD0C */
static const unsigned long AI_OPER_xZero_bits = 0x00000000u;
#define AI_OPER_xZero        (*(const float *)&AI_OPER_xZero_bits)

#define AI_MTH_M_lAdd(A,B) ( (A)+(B) )
#define AI_MTH_M_lSub(A,B) ( (A)-(B) )
#define AI_MTH_M_lMul(A,B) ( (A)*(B) )
#define AI_MTH_M_lDiv(A,B) ( (A)/(B) )
#define AI_MTH_M_xAdd(A,B) MTH_M_xAdd(A,B)
#define AI_MTH_M_xSub(A,B) MTH_M_xSub(A,B)
#define AI_MTH_M_xMul(A,B) MTH_M_xMul(A,B)
#define AI_MTH_M_xDiv(A,B) MTH_M_xDiv(A,B)
#define AI_M_Operator_Arith(ucIsThereFloat,Func,p_stParam1,p_stParam2,p_stValue) \
    if ((ucIsThereFloat)) \
    { \
        M_Full_GetSetParam_Float(p_stValue, \
          AI_MTH_M_x##Func( (p_stParam1)->uParam.xValue, (p_stParam2)->uParam.xValue ) ); \
    } \
    else \
    { \
        M_Full_GetSetParam_Integer(p_stValue, \
          AI_MTH_M_l##Func( (p_stParam1)->uParam.lValue, (p_stParam2)->uParam.lValue ) ); \
    }

#include <stddef.h>

#include "AI/AI_types.h"
#include "GAM/GAM_types.h"

HIE_tdstSuperObject *g_hCurrentSuperObjPerso;

#include <stddef.h>
#include "MTH/MTH_flt.h"
#include "MTH/MTH3D.h"

typedef int tdeTypeInterpret;
typedef unsigned long tdeOperatorId;
typedef long tdeFieldId;
#define M_TypeUnionInterpret(p_Tree)    (&((p_Tree)->uId))
#define M_eOperatorIdInterpret(p_Tree)  (*M_TypeUnionInterpret(p_Tree))
#define M_eFieldIdInterpret(p_Tree)     (*M_TypeUnionInterpret(p_Tree))

#define E_ti_Field                 9
#define E_ti_DsgVarRef             11
#define eNbField                   0x12
#define eOperator_PlusAffect       5
#define eOperator_MinusAffect      6
#define eOperator_MulAffect        7
#define eOperator_DivAffect        8
#define eOperator_PlusPlusAffect   9
#define eOperator_MinusMinusAffect 10
#define eOperator_Affect           11
#define eOperator_Dot              12
#define eOperator_GetVectorX       13
#define eOperator_GetVectorY       14
#define eOperator_GetVectorZ       15
#define eOperator_SetVectorX       21
#define eOperator_SetVectorY       22
#define eOperator_SetVectorZ       23
#define eOperator_AffectArray      27

#include "AI/ActConst.h"

#define AI_MTH_M_lAdd(A,B) ( (A)+(B) )
#define AI_MTH_M_lSub(A,B) ( (A)-(B) )
#define AI_MTH_M_lMul(A,B) ( (A)*(B) )
#define AI_MTH_M_lDiv(A,B) ( (A)/(B) )
#define AI_MTH_M_xAdd(A,B) MTH_M_xAdd(A,B)
#define AI_MTH_M_xSub(A,B) MTH_M_xSub(A,B)
#define AI_MTH_M_xMul(A,B) MTH_M_xMul(A,B)
#define AI_MTH_M_xDiv(A,B) MTH_M_xDiv(A,B)
#define M_Operator_Arith(ucIsThereFloat,Func,p_stParam1,p_stParam2,p_stValue) \
if ((ucIsThereFloat)) \
{ \
    M_Full_GetSetParam_Float(p_stValue, \
      AI_MTH_M_x##Func( \
      M_GetSetParam_xValue(p_stParam1), \
      M_GetSetParam_xValue(p_stParam2)  \
      ) \
    ); \
} \
else \
{ \
    M_Full_GetSetParam_Integer(p_stValue, \
      AI_MTH_M_l##Func(  \
      M_GetSetParam_lValue(p_stParam1), \
      M_GetSetParam_lValue(p_stParam2)  \
      ) \
    ); \
}

void fn_vOperatorSetDsgVar(unsigned char ucVarId,unsigned char ucIndex,HIE_tdstSuperObject *hSuperObjPerso,AI_tdstGetSetParam *p_stValue);
void fn_vGetFieldOrDsgVarForAffectOperator(HIE_tdstSuperObject *hSuperObjPerso,tdeTypeInterpret eTypeInterpret,unsigned char ucVarId,unsigned char ucIndex,tdeFieldId eFieldId,AI_tdstGetSetParam *p_stValue);
void fn_vSetFieldOrDsgVarForAffectOperator(HIE_tdstSuperObject *hSuperObjPerso,tdeTypeInterpret eTypeInterpret,unsigned char ucVarId,unsigned char ucIndex,tdeFieldId eFieldId,AI_tdstGetSetParam *p_stValue);
void fn_vSetVectorComponentForAffectOperator(unsigned char ucXYZflag,MTH3D_tdstVector *p_stVector,MTH_tdxReal xReal);
void fn_vBuildVectorForAffectOperator(unsigned char ucXYZflag,AI_tdstGetSetParam *p_stParam,MTH_tdxReal xReal);
unsigned char fn_ucGetTreeNbParam(AI_tdstNodeInterpret *p_stTree);
unsigned char fn_ucAreThereFloatIfYesConvertThem(AI_tdstGetSetParam *p_stParam1,AI_tdstGetSetParam *p_stParam2);

unsigned char __cdecl fn_ucGetDsgVar(unsigned char ucId, unsigned char ucIndex, AI_tdstMind *p_stMind, AI_tdstGetSetParam *p_stGetSetParam);

tdp_fn_ucSetDsgVarFunctionPtr fn_p_fn_pGetDsgVarSetFunctionPtr(enum tdeDsgVarTypeId_ eDsgVarTypeId);

unsigned char __cdecl fn_ucSetDsgVar(unsigned char ucId, unsigned char ucIndex, AI_tdstMind *p_stPerso, AI_tdstGetSetParam *p_stGetSetParam);

void __cdecl fn_vOperatorSetField(unsigned long ulFieldId, void *p_stContext, void *p_stGetSetParam);

AI_tdstNodeInterpret *__cdecl fn_p_stScalarOperator(
    HIE_tdstSuperObject *p_SuperObjPerso, AI_tdstNodeInterpret *p_stTree, AI_tdstGetSetParam *p_stValue)
{
    AI_tdstGetSetParam    stParam2;
    long                  eOperatorId = *(long *)((char *)p_stTree - 8);
    unsigned char         ucIsThereFloat;

    if (eOperatorId == 4)
    {
        M_Full_GetSetParam_Integer(p_stValue, 0);
    }
    else
    {

        p_stTree = fn_p_stEvalTree(p_SuperObjPerso, p_stTree, p_stValue);

        if ((eOperatorId == 2) || (eOperatorId == 3))
        {

            if (MTH_M_bIsNull(M_ReturnParam_xValue(p_stValue)))
            {

                return fn_p_stSkipThisArgument(p_stTree);
            }
        }
    }

    p_stTree = fn_p_stEvalTree(p_SuperObjPerso, p_stTree, &stParam2);

    ucIsThereFloat = fn_ucAreThereFloatIfYesConvertThem(p_stValue, &stParam2);

    switch (eOperatorId)
    {
    case 0:
        AI_M_Operator_Arith(ucIsThereFloat, Add, p_stValue, &stParam2, p_stValue);
        return p_stTree;

    case 4:
    case 1:
        AI_M_Operator_Arith(ucIsThereFloat, Sub, p_stValue, &stParam2, p_stValue);
        return p_stTree;

    case 2:
        AI_M_Operator_Arith(ucIsThereFloat, Mul, p_stValue, &stParam2, p_stValue);
        return p_stTree;

    case 3:
        if (((ucIsThereFloat) && (MTH_M_bDifferentWithEpsilon(stParam2.uParam.xValue, MTH_C_ZERO, MTH_C_EpsilonPlus)))
            || ((!ucIsThereFloat) && (stParam2.uParam.lValue != 0)))
        {
            AI_M_Operator_Arith(ucIsThereFloat, Div, p_stValue, &stParam2, p_stValue);
        }
        else
        {
            M_Full_GetSetParam_Integer(p_stValue, 0);
        }
        return p_stTree;

    default:
        break;
    }
    M_Full_GetSetParam_Integer(p_stValue, 0);
    return p_stTree;
}

/* 0x00481A60 */
AI_tdstNodeInterpret *__cdecl fn_p_stVectorOperator(
    HIE_tdstSuperObject *p_SuperObjPerso, AI_tdstNodeInterpret *p_stTree, AI_tdstGetSetParam *p_stValue)
{
    AI_tdstGetSetParam    stParam1, stParam2;
    long                  eOperatorId = *(long *)((char *)p_stTree - 8);
    MTH3D_tdstVector     *p_stVector, *p_stVector1, *p_stVector2 = (MTH3D_tdstVector *)0;
    MTH_tdxReal           xFactor = MTH_C_ONE;

    p_stTree = fn_p_stEvalTree(p_SuperObjPerso, p_stTree, &stParam1);
    p_stVector1 = (MTH3D_tdstVector *)stParam1.uParam.a_xVectorValue;

    if (eOperatorId != 0x14)
    {
        p_stTree = fn_p_stEvalTree(p_SuperObjPerso, p_stTree, &stParam2);
        if ((eOperatorId == 0x10) || (eOperatorId == 0x11))
        {
            p_stVector2 = (MTH3D_tdstVector *)stParam2.uParam.a_xVectorValue;
        }
        else
        {
            xFactor = M_ReturnParam_xValue(&stParam2);
        }
    }

    p_stVector = (MTH3D_tdstVector *)p_stValue->uParam.a_xVectorValue;
    p_stValue->eType = E_vt_Vector;

    switch (eOperatorId)
    {
    case 0x10:
        MTH3D_M_vAddVector(p_stVector, p_stVector1, p_stVector2);
        return p_stTree;
    case 0x11:
        MTH3D_M_vSubVector(p_stVector, p_stVector1, p_stVector2);
        return p_stTree;
    case 0x12:
        MTH3D_M_vMulScalarVector(p_stVector, xFactor, p_stVector1);
        return p_stTree;
    case 0x13:
        if (MTH_M_bDifferentWithEpsilon(xFactor, MTH_C_ZERO, MTH_C_EpsilonPlus))
        {
            MTH3D_M_vDivScalarVector(p_stVector, p_stVector1, xFactor);
        }
        else
        {
            MTH3D_M_vNullVector(p_stVector);
        }
        return p_stTree;
    case 0x14:
        MTH3D_M_vNegVector(p_stVector, p_stVector1);
        return p_stTree;
    default:
        break;
    }
    MTH3D_M_vNullVector(p_stVector);
    return p_stTree;
}

/* 0x00481C10 */
AI_tdstNodeInterpret *__cdecl fn_p_stOperatorGetPerso(
    HIE_tdstSuperObject *p_SuperObjPerso, AI_tdstNodeInterpret *p_stTree,
    HIE_tdstSuperObject **hSuperObjPersoFound)
{
  HIE_tdstSuperObject *hSuperObjPerso;
  AI_tdstGetSetParam stValue;

	M_EvalNextParameter(&stValue);

  hSuperObjPerso = (HIE_tdstSuperObject *)M_GetSetParam_p_stSupObjValue(&stValue);
	*hSuperObjPersoFound=hSuperObjPerso;
	return p_stTree;
}

/* 0x00481C50 */
AI_tdstNodeInterpret *__cdecl fn_p_stArrayOperator(
    HIE_tdstSuperObject *p_SuperObjPerso, AI_tdstNodeInterpret *p_stTree, long *p_lValue)
{
    HIE_tdstSuperObject  *p_stObject = p_SuperObjPerso;
    AI_tdstNodeInterpret *p_stArray  = p_stTree;
    AI_tdstNodeInterpret *p_stNext;
    unsigned char         ucId;

    if (p_stTree->eType == 0x02)
        p_stArray = AI_M_pfn_fn_p_stOperatorGetPerso(
            p_SuperObjPerso, p_stTree + 1, &p_stObject);

    ucId    = (unsigned char)M_ucVarIdInterpret(p_stArray);
    p_stNext = fn_p_stEvalTree(
        p_SuperObjPerso, p_stArray + 1, (AI_tdstGetSetParam *)p_lValue);

    fn_vOperatorGetDsgVar(ucId, (unsigned char)p_lValue[0], p_stObject, p_lValue);
    return p_stNext;
}

/* 0x00481CB0 */
void __cdecl fn_vOperatorGetDsgVar(unsigned char ucVarId, unsigned char ucIndex,
                                   HIE_tdstSuperObject *hSuperObjPerso, AI_tdstGetSetParam *p_stValue)
{
   if (fn_ucGetDsgVar(ucVarId, ucIndex, AI_M_stGetMindOfSuperObj(hSuperObjPerso), p_stValue)==C_INVALID_GET)
   {
   }
}

/* 0x00481CE0 */
AI_tdstNodeInterpret *__cdecl fn_p_stDotOperator(
    HIE_tdstSuperObject *p_SuperObjPerso, AI_tdstNodeInterpret *p_stTree, long *p_lValue)
{

    HIE_tdstSuperObject *hSuperObjPerso;
    long                 eTypeInterpret;
    unsigned char        ucIndex = 0;

    p_stTree = AI_M_pfn_fn_p_stOperatorGetPerso(p_SuperObjPerso, p_stTree, &hSuperObjPerso);

    eTypeInterpret = p_stTree->eType;

    if (eTypeInterpret == 0x0b)
    {
        unsigned char ucVarId = (unsigned char)M_ucVarIdInterpret(p_stTree);
        p_stTree++;
        fn_vOperatorGetDsgVar(ucVarId, ucIndex, hSuperObjPerso, p_lValue);
    }
    else if (eTypeInterpret == 0x09)
    {
        long eFieldId = p_stTree->uParam.lValue;
        p_stTree++;
        fn_vOperatorGetField(eFieldId, hSuperObjPerso, p_lValue);
    }
    else if ((eTypeInterpret == 0x02) && (p_stTree->uParam.lValue == 0x19))
    {
        p_stTree = fn_p_stEvalTree(p_SuperObjPerso, p_stTree, (AI_tdstGetSetParam *)p_lValue);
    }
    return p_stTree;
}

void __cdecl fn_vOperatorGetField(unsigned long ulFieldId, void *p_stContext,
                                  void *p_stGetSetParam)
{
    if (fn_ucGetFieldValue(ulFieldId, p_stContext, (AI_tdstGetSetParam *)p_stGetSetParam) == 0)
        fn_vOnAIErm_M_UpdateLastError(0x22, (char *)0);
}

/* 0x00481DA0 */
AI_tdstNodeInterpret *__cdecl fn_p_stVectorDotOperator(
    HIE_tdstSuperObject *p_SuperObjPerso, AI_tdstNodeInterpret *p_stTree, long *p_lValue)
{
    unsigned long         opId     = *(unsigned long *)((char *)p_stTree - 8);
    AI_tdstNodeInterpret *p_stNext = fn_p_stEvalTree(
        p_SuperObjPerso, p_stTree, (AI_tdstGetSetParam *)p_lValue);

    if (p_lValue[3] == 0x15)
    {

        switch (opId)
        {
        case 0xd:
            p_lValue[3] = 8;
            break;
        case 0xe:
        {
            float xY = *(float *)&p_lValue[1];
            *(float *)&p_lValue[0] = xY;
            p_lValue[3] = 8;
            break;
        }
        case 0xf:
        {
            float xZ = *(float *)&p_lValue[2];
            *(float *)&p_lValue[0] = xZ;
            p_lValue[3] = 8;
            break;
        }
        default:
            break;
        }
    }
    return p_stNext;
}

AI_tdstNodeInterpret *fn_p_stAffectOperator(HIE_tdstSuperObject *p_SuperObjPerso, AI_tdstNodeInterpret *p_stTree, AI_tdstGetSetParam *p_stValue)
{
  AI_tdstGetSetParam stParam1, stParam2;
  tdeFieldId eFieldId=eNbField;
  unsigned char ucVarId=0xFF;
  unsigned char ucIndex=0;
  unsigned char ucXYZflag=0;
  unsigned char ucNbParam=0;

  HIE_tdstSuperObject *hSuperObjPerso = p_SuperObjPerso;
  tdeOperatorId eOperatorId = M_eOperatorIdInterpret(p_stTree-1);
  tdeTypeInterpret eTypeInterpret = M_GetTypeInterpret(p_stTree);

  if ( eOperatorId == eOperator_AffectArray )
  {
		ucNbParam = fn_ucGetTreeNbParam(p_stTree)-1;
  }

  if ( eTypeInterpret == E_ti_Operator )
  {
		{
			register tdeOperatorId eOpId = M_eOperatorIdInterpret(p_stTree);
			if ( eOpId == eOperator_GetVectorX )
				ucXYZflag = 1;
			else if ( eOpId == eOperator_GetVectorY )
				ucXYZflag = 2;
			else if ( eOpId == eOperator_GetVectorZ )
				ucXYZflag = 3;
		}
		if ( ucXYZflag )
		{
			p_stTree ++;
			eTypeInterpret = M_GetTypeInterpret(p_stTree);
		}
  }

  if ( eTypeInterpret == E_ti_Operator )
  {
		if ( M_eOperatorIdInterpret(p_stTree) == eOperator_Dot )
		{
			p_stTree=fn_p_stOperatorGetPerso(p_SuperObjPerso,p_stTree + 1,&hSuperObjPerso);
			eTypeInterpret = M_GetTypeInterpret(p_stTree);
		}
		else
		{
			p_stTree ++;
			eTypeInterpret = M_GetTypeInterpret(p_stTree);
			if ( eTypeInterpret == E_ti_Operator )
			{
				if ( M_eOperatorIdInterpret(p_stTree) == eOperator_Dot )
				{
					p_stTree = fn_p_stOperatorGetPerso(p_SuperObjPerso, p_stTree + 1, &hSuperObjPerso);
				}
			}
			ucVarId = (unsigned char) M_ucVarIdInterpret(p_stTree);
			p_stTree ++;
			eTypeInterpret = E_ti_Operator;
		}
  }

  if ( eTypeInterpret == E_ti_DsgVarRef )
  {
    ucVarId = (unsigned char) M_ucVarIdInterpret(p_stTree);
		p_stTree ++;
  }
  else if ( eTypeInterpret == E_ti_Field )
  {
    eFieldId = M_eFieldIdInterpret(p_stTree);
		p_stTree ++;
  }
  else if ( eTypeInterpret == E_ti_Operator )
  {
		M_EvalNextParameter(p_stValue);
		ucIndex = (unsigned char) M_ReturnParam_lValue(p_stValue);
		eTypeInterpret = E_ti_DsgVarRef;
  }

  if ( eOperatorId == eOperator_Affect )
  {
		if ( ucXYZflag )
		{

			fn_vGetFieldOrDsgVarForAffectOperator(hSuperObjPerso,eTypeInterpret,ucVarId,ucIndex,eFieldId,p_stValue);

			M_EvalNextParameter(&stParam1);

			fn_vSetVectorComponentForAffectOperator(ucXYZflag,&M_GetSetParam_stVertexValue(p_stValue),M_ReturnParam_xValue(&stParam1));
		}
		else
		{

			M_EvalNextParameter(p_stValue);
		}
  }
  else if ( eOperatorId == eOperator_AffectArray )
  {
		if ( ucNbParam > 0 )
		{
			do
			{

				M_EvalNextParameter(p_stValue);
				fn_vOperatorSetDsgVar(ucVarId,ucIndex,hSuperObjPerso,p_stValue);
				ucIndex ++;
				ucNbParam --;
			}
			while ( ucNbParam );
	 }
	 M_Full_GetSetParam_ActionReturn(p_stValue,C_ACTION_ENGINE_CONTINUE);
	 return(p_stTree);
  }
  else
  {
		unsigned char ucIsVector=0;

		fn_vGetFieldOrDsgVarForAffectOperator(hSuperObjPerso,eTypeInterpret,ucVarId,ucIndex,eFieldId,&stParam2);

		if ( (eOperatorId == eOperator_MinusMinusAffect) || (eOperatorId == eOperator_PlusPlusAffect) )
		{
			if (!ucXYZflag)
			{
				M_Full_GetSetParam_Integer(&stParam1,1);
			}
			else
			{

				fn_vBuildVectorForAffectOperator(ucXYZflag,&stParam1,MTH_C_ONE);
			}
		}
		else
		{
			M_EvalNextParameter(&stParam1);

			if (ucXYZflag)
			{

				fn_vBuildVectorForAffectOperator(ucXYZflag,&stParam1,M_ReturnParam_xValue(&stParam1));
			}
		}

		ucIsVector = (unsigned char) M_IsVectorType(&stParam2);
		if ( ucIsVector )
		{
			ucIsVector=2;
			if (!M_IsVectorType(&stParam1))
			{
				if
				(
					(eOperatorId==eOperator_SetVectorX)
					||(eOperatorId==eOperator_SetVectorY)
					||(eOperatorId==eOperator_SetVectorZ)
					||(eOperatorId==eOperator_MulAffect)
					||(eOperatorId==eOperator_DivAffect)
					||(ucXYZflag)
				)
				{
					ucIsVector=1;
				}
				else
				{
					MTH3D_M_vNullVector(&M_GetSetParam_stVertexValue(&stParam1));
				}
			}
		}
		else
		{
			if (M_IsVectorType(&stParam1))
			{
				M_Full_GetSetParam_Float(&stParam1,MTH_C_ZERO);
			}
		}

		if ( ucIsVector )
		{
			MTH3D_tdstVector *p_stVector1=NULL,*p_stVector2;
			MTH_tdxReal xReal = MTH_C_ZERO;
			MTH3D_tdstVector *p_stVector=&M_GetSetParam_stVertexValue(p_stValue);
			M_GetSetParam_Type(p_stValue)=E_vt_Vector;

			p_stVector2=&M_GetSetParam_stVertexValue(&stParam2);
			if ( ucIsVector == 1 )
			{
				xReal=M_ReturnParam_xValue(&stParam1);
				MTH3D_M_vCopyVector(p_stVector,p_stVector2);
			}
			else
			{
				p_stVector1 = &M_GetSetParam_stVertexValue(&stParam1);
			}

			switch(eOperatorId)
			{
			case eOperator_SetVectorX :
				MTH3D_M_vSetXofVector(p_stVector,xReal);
				break;

			case eOperator_SetVectorY :
				MTH3D_M_vSetYofVector(p_stVector,xReal);
				break;

			case eOperator_SetVectorZ :
				MTH3D_M_vSetZofVector(p_stVector,xReal);
				break;

			case eOperator_MulAffect:
				if (!ucXYZflag)
				{
					MTH3D_M_vMulScalarVector(p_stVector, xReal, p_stVector2);
				}
				else
				{
					MTH3D_M_vCopyVector(p_stVector,p_stVector2);
					switch ( ucXYZflag )
					{
					case 1:
						xReal = MTH3D_M_xGetXofVector(p_stVector1);
						MTH3D_M_vSetXofVector(p_stVector,MTH_M_xMul(MTH3D_M_xGetXofVector(p_stVector2),xReal));
						break;

					case 2:
						xReal = MTH3D_M_xGetYofVector(p_stVector1);
						MTH3D_M_vSetYofVector(p_stVector,MTH_M_xMul(MTH3D_M_xGetYofVector(p_stVector2),xReal));
						break;

					case 3:
						xReal = MTH3D_M_xGetZofVector(p_stVector1);
						MTH3D_M_vSetZofVector(p_stVector,MTH_M_xMul(MTH3D_M_xGetZofVector(p_stVector2),xReal));
						break;
					}
				}
				break;

			case eOperator_DivAffect:
				if ( !ucXYZflag )
				{
					MTH3D_M_vDivScalarVector(p_stVector,p_stVector2,xReal);
				}
				else
				{
					MTH3D_M_vCopyVector(p_stVector,p_stVector2);
					switch ( ucXYZflag )
					{
					case 1:
						xReal = MTH3D_M_xGetXofVector(p_stVector1);
						MTH3D_M_vSetXofVector(p_stVector,MTH_M_xDiv(MTH3D_M_xGetXofVector(p_stVector2),xReal));
						break;

					case 2:
						xReal = MTH3D_M_xGetYofVector(p_stVector1);
						MTH3D_M_vSetYofVector(p_stVector,MTH_M_xDiv(MTH3D_M_xGetYofVector(p_stVector2),xReal));
						break;

					case 3:
						xReal = MTH3D_M_xGetZofVector(p_stVector1);
						MTH3D_M_vSetZofVector(p_stVector,MTH_M_xDiv(MTH3D_M_xGetZofVector(p_stVector2),xReal));
						break;
					}
				}
				break;

			case eOperator_PlusAffect:
			case eOperator_PlusPlusAffect:
				MTH3D_M_vAddVector(p_stVector,p_stVector2,p_stVector1);
				break;

			case eOperator_MinusAffect:
			case eOperator_MinusMinusAffect:
				MTH3D_M_vSubVector(p_stVector,p_stVector2,p_stVector1);
				break;

			default:
				MTH3D_M_vNullVector(p_stVector);
				break;
			}
		}
		else
		{

			unsigned char ucIsThereFloat = fn_ucAreThereFloatIfYesConvertThem(&stParam1,&stParam2);

			M_Full_GetSetParam_Integer(p_stValue,0);

			switch(eOperatorId)
			{
			case eOperator_PlusAffect:
			case eOperator_PlusPlusAffect:
				M_Operator_Arith(ucIsThereFloat,Add,&stParam2,&stParam1,p_stValue);
				break;

			case eOperator_MinusAffect:
			case eOperator_MinusMinusAffect:
				M_Operator_Arith(ucIsThereFloat,Sub,&stParam2,&stParam1,p_stValue);
				break;

			case eOperator_MulAffect:
				M_Operator_Arith(ucIsThereFloat,Mul,&stParam2,&stParam1,p_stValue);
				break;
			case eOperator_DivAffect:
				if
				(
					((ucIsThereFloat) && (MTH_M_bDifferentWithEpsilon(M_GetSetParam_xValue(&stParam1),MTH_C_ZERO,MTH_C_EpsilonPlus)))
					||
					((!ucIsThereFloat) && ( M_GetSetParam_lValue(&stParam1) != 0))
				)
				{
				 	M_Operator_Arith(ucIsThereFloat,Div,&stParam2,&stParam1,p_stValue);
				}
				break;

			default:
				break;
			}
		}
  }

	fn_vSetFieldOrDsgVarForAffectOperator(hSuperObjPerso,eTypeInterpret,ucVarId,ucIndex,eFieldId,p_stValue);

  M_Full_GetSetParam_ActionReturn(p_stValue,C_ACTION_ENGINE_CONTINUE);
  return(p_stTree);
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

/* 0x00482590 */
void __cdecl fn_vSetUltraOperatorPerso(HIE_tdstSuperObject *hCurrentSuperObjPerso,
                                       HIE_tdstSuperObject *hNewCurrentSuperObjPerso)
{

    g_hCurrentSuperObjPerso = (hCurrentSuperObjPerso != hNewCurrentSuperObjPerso)
                                  ? hNewCurrentSuperObjPerso : NULL;
}

void __cdecl fn_vGetUltraOperatorPerso(
    unsigned char         bYouHaveRightToUseThisFunction,
    HIE_tdstSuperObject  *hCurrentSuperObjPerso,
    HIE_tdstSuperObject **hNewSuperObjPerso)
{
    if (g_hCurrentSuperObjPerso  )
    {
        if (bYouHaveRightToUseThisFunction)
        {
            *hNewSuperObjPerso = g_hCurrentSuperObjPerso;
            g_hCurrentSuperObjPerso = NULL;
        }

    }
    else
        *hNewSuperObjPerso = hCurrentSuperObjPerso;
}

/* 0x004825E0 */
AI_tdstNodeInterpret *__cdecl fn_p_stUltraOperator(
    HIE_tdstSuperObject *p_SuperObjPerso, AI_tdstNodeInterpret *p_stTree, long *p_lValue)
{
    HIE_tdstSuperObject  *p_stObject;
    AI_tdstNodeInterpret *p_stExpr =
        AI_M_pfn_fn_p_stOperatorGetPerso(p_SuperObjPerso, p_stTree, &p_stObject);

    AI_M_pfn_fn_vSetUltraOperatorPerso(p_SuperObjPerso, p_stObject);
    return fn_p_stEvalTree(p_SuperObjPerso, p_stExpr, (AI_tdstGetSetParam *)p_lValue);
}

/* 0x00482620 */
AI_tdstNodeInterpret *__cdecl fn_p_stModelCastOperator(
    HIE_tdstSuperObject *p_SuperObjPerso, AI_tdstNodeInterpret *p_stTree, long *p_lValue)
{
    HIE_tdstSuperObject  *p_stObject;
    AI_tdstNodeInterpret *p_stAfter  = fn_p_stEvalTree(
        p_SuperObjPerso, p_stTree, (AI_tdstGetSetParam *)p_lValue);
    AI_tdstNodeInterpret *p_stNext =
        AI_M_pfn_fn_p_stOperatorGetPerso(p_SuperObjPerso, p_stAfter, &p_stObject);

    p_lValue[0] = (p_stObject == (HIE_tdstSuperObject *)0) ? 0
        : ((*(long *)p_stObject != 2) ? 0 : *((long *)p_stObject + 1));
    p_lValue[3] = 0x0b;
    return p_stNext;
}
