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
