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
