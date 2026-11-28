#include "AI/AI_types.h"
#include "AI/Brain.h"
#include "GAM/GAM_types.h"
#include "AI/AI_globals.h"
#include "AI/MetaAction.h"
#include "AI/StrIntel.h"
#include "AI/AI_Struc.h"

unsigned char g_ucAIModuleId = 0xFF;

/* 0x00482760 */
void __cdecl fn_vUseCurrentActionTableEntry(AI_tdstIntelligence *p_stIntelligence, unsigned char ucNRule,
                                            AI_tdstNodeInterpret *p_stNode);

#define AI_M_OP(f)  ((AI_tdp_fn_p_stOperatorFunctionPtr)(f))
static AI_tdp_fn_p_stKeyWordFunctionPtr a_pfnKeyWordTable[20] =
{
    fn_p_stIfKeyWord, fn_p_stIfKeyWord, fn_p_stIfKeyWord,
    fn_p_stIfKeyWord, fn_p_stIfKeyWord, fn_p_stIfKeyWord,
    fn_p_stIfDebug, fn_p_stIfNotU64,
    fn_p_stThenKeyWord, fn_p_stElseKeyWord, fn_p_stEngineGotoKeyWord,
    fn_p_stMeKeyWord, fn_p_stMainActorKeyWord, fn_p_stNobodyKeyWord,
    fn_p_stNoSuperObjectKeyWord, fn_p_stNowhereKeyWord, fn_p_stEmptyTextKeyWord,
    fn_p_stCapsNullKeyWord, fn_p_stNoGraphKeyWord, fn_p_stNoActionKeyWord
};

/* 0x004A6398 */
static AI_tdp_fn_p_stOperatorFunctionPtr a_pfnOperatorTable[28] =
{
    AI_M_OP(fn_p_stScalarOperator), AI_M_OP(fn_p_stScalarOperator), AI_M_OP(fn_p_stScalarOperator),
    AI_M_OP(fn_p_stScalarOperator), AI_M_OP(fn_p_stScalarOperator),
    AI_M_OP(fn_p_stAffectOperator), AI_M_OP(fn_p_stAffectOperator), AI_M_OP(fn_p_stAffectOperator),
    AI_M_OP(fn_p_stAffectOperator), AI_M_OP(fn_p_stAffectOperator), AI_M_OP(fn_p_stAffectOperator),
    AI_M_OP(fn_p_stAffectOperator),
    AI_M_OP(fn_p_stDotOperator),
    AI_M_OP(fn_p_stVectorDotOperator), AI_M_OP(fn_p_stVectorDotOperator), AI_M_OP(fn_p_stVectorDotOperator),
    AI_M_OP(fn_p_stVectorOperator), AI_M_OP(fn_p_stVectorOperator), AI_M_OP(fn_p_stVectorOperator),
    AI_M_OP(fn_p_stVectorOperator), AI_M_OP(fn_p_stVectorOperator),
    AI_M_OP(fn_p_stAffectOperator), AI_M_OP(fn_p_stAffectOperator), AI_M_OP(fn_p_stAffectOperator),
    AI_M_OP(fn_p_stUltraOperator), AI_M_OP(fn_p_stModelCastOperator), AI_M_OP(fn_p_stArrayOperator),
    AI_M_OP(fn_p_stAffectOperator)
};

/* 0x004A5EE0 */
typedef struct AI_tdstConditionEntry_
{
    AI_tdp_fn_p_stConditionFunctionPtr p_fn_pCondition;
    unsigned char                      ucUltraOperator;
} AI_tdstConditionEntry;
#define AI_M_CD(f, u)  { (AI_tdp_fn_p_stConditionFunctionPtr)(f), (u) }
static AI_tdstConditionEntry a_stConditionTable[149] =
{
      AI_M_CD(fn_p_stBooleanCondition, 0), AI_M_CD(fn_p_stBooleanCondition, 0), AI_M_CD(fn_p_stBooleanCondition, 0), AI_M_CD(fn_p_stBooleanCondition, 0),
      AI_M_CD(fn_p_stComparisonCondition, 0), AI_M_CD(fn_p_stComparisonCondition, 0), AI_M_CD(fn_p_stComparisonCondition, 0), AI_M_CD(fn_p_stComparisonCondition, 0), AI_M_CD(fn_p_stComparisonCondition, 0), AI_M_CD(fn_p_stComparisonCondition, 0),
      AI_M_CD(fn_p_stZDDCondition, 0), AI_M_CD(fn_p_stZDDCondition, 0), AI_M_CD(fn_p_stZDDCondition, 0), AI_M_CD(fn_p_stZDDCondition, 0), AI_M_CD(fn_p_stZDDCondition, 0),
      AI_M_CD(fn_p_stZDECondition, 0), AI_M_CD(fn_p_stZDECondition, 0), AI_M_CD(fn_p_stZDECondition, 0), AI_M_CD(fn_p_stZDECondition, 0), AI_M_CD(fn_p_stZDECondition, 0), AI_M_CD(fn_p_stZDECondition, 0),
      AI_M_CD(fn_p_stZDECondition, 0), AI_M_CD(fn_p_stZDECondition, 0), AI_M_CD(fn_p_stZDECondition, 0), AI_M_CD(fn_p_stZDECondition, 0), AI_M_CD(fn_p_stZDECondition, 0), AI_M_CD(fn_p_stZDECondition, 0),
      AI_M_CD(fn_p_stReportOnSurfaceCondition, 1), AI_M_CD(fn_p_stReportOnSurfaceCondition, 1), AI_M_CD(fn_p_stReportOnSurfaceCondition, 1), AI_M_CD(fn_p_stReportOnSurfaceCondition, 1), AI_M_CD(fn_p_stReportOnSurfaceCondition, 1), AI_M_CD(fn_p_stReportOnSurfaceCondition, 1),
      AI_M_CD(fn_p_stReportThisPerso, 1),
      AI_M_CD(fn_p_stReportOnZDMSurfaceCondition, 1), AI_M_CD(fn_p_stReportOnZDMSurfaceCondition, 1), AI_M_CD(fn_p_stReportOnZDMSurfaceCondition, 1), AI_M_CD(fn_p_stReportOnZDMSurfaceCondition, 1),
      AI_M_CD(fn_p_stListCondition, 0), AI_M_CD(fn_p_stListCondition, 0), AI_M_CD(fn_p_stListCondition, 0), AI_M_CD(fn_p_stListCondition, 0),
      AI_M_CD(fn_p_st_UserEvent_IsSet, 0), AI_M_CD(fn_p_st_UserEvent_IsSet, 0),
      AI_M_CD(fn_p_stButtonCondition, 0), AI_M_CD(fn_p_stButtonCondition, 0), AI_M_CD(fn_p_stButtonCondition, 0), AI_M_CD(fn_p_stButtonCondition, 0),
      AI_M_CD(fn_p_stTimeCondition, 0),
      AI_M_CD(fn_p_stValidityCondition, 0), AI_M_CD(fn_p_stValidityCondition, 0), AI_M_CD(fn_p_stValidityCondition, 0), AI_M_CD(fn_p_stValidityCondition, 0), AI_M_CD(fn_p_stValidityCondition, 0),
      AI_M_CD(fn_p_stSectorCondition, 1), AI_M_CD(fn_p_st_ActivationCondition, 1), AI_M_CD(fn_p_st_HandledCondition, 0), AI_M_CD(fn_p_stAlw_IsMine, 1),
      AI_M_CD(fn_p_stLightCondition, 1), AI_M_CD(fn_p_stLightCondition, 1), AI_M_CD(fn_p_stLightCondition, 1),
      AI_M_CD(AI_fn_p_st_pc_ReportSurfaceMaterialCondition, 0), AI_M_CD(AI_fn_p_st_pc_ReportSurfaceMaterialCondition, 0), AI_M_CD(AI_fn_p_st_pc_ReportSurfaceMaterialCondition, 0), AI_M_CD(AI_fn_p_st_pc_ReportSurfaceMaterialCondition, 0),
      AI_M_CD(fn_p_stMiscCondition, 0), AI_M_CD(fn_p_stMiscCondition, 0), AI_M_CD(fn_p_stMiscCondition, 0), AI_M_CD(fn_p_stMiscCondition, 0), AI_M_CD(fn_p_stMiscCondition, 0),
      AI_M_CD(fn_p_stCheckAnimEnd, 1),
      AI_M_CD(fn_p_stMiscCondition, 0), AI_M_CD(fn_p_stMiscCondition, 0), AI_M_CD(fn_p_stMiscCondition, 0),
      AI_M_CD(fn_p_stCollisionWP, 0), AI_M_CD(fn_p_stCustomBitsCond, 1), AI_M_CD(fn_p_stIsPersoActive, 0), AI_M_CD(fn_p_stCheckAnimEnd, 1), AI_M_CD(fn_p_stCustomBitsCond, 1),
      AI_M_CD(fn_p_stMechanicOption, 1), AI_M_CD(fn_p_stMechanicOption, 1), AI_M_CD(fn_p_stMechanicOption, 1), AI_M_CD(fn_p_stMechanicOption, 1), AI_M_CD(fn_p_stMechanicOption, 1), AI_M_CD(fn_p_stMechanicOption, 1),
      AI_M_CD(fn_p_stMechanicOption, 1), AI_M_CD(fn_p_stMechanicOption, 1), AI_M_CD(fn_p_stMechanicOption, 1), AI_M_CD(fn_p_stMechanicOption, 1), AI_M_CD(fn_p_stMechanicOption, 1), AI_M_CD(fn_p_stMechanicOption, 1),
      AI_M_CD(fn_p_stMechanicOption, 1), AI_M_CD(fn_p_stMechanicOption, 1), AI_M_CD(fn_p_stMechanicOption, 1), AI_M_CD(fn_p_stMechanicOption, 1), AI_M_CD(fn_p_stMechanicOption, 1), AI_M_CD(fn_p_stMechanicOption, 1),
      AI_M_CD(fn_p_stMechanicOption, 1), AI_M_CD(fn_p_stMechanicOption, 1), AI_M_CD(fn_p_stMechanicOption, 1), AI_M_CD(fn_p_stMechanicOption, 1), AI_M_CD(fn_p_stMechanicOption, 1), AI_M_CD(fn_p_stMechanicOption, 1),
      AI_M_CD(fn_p_stNullVector, 0), AI_M_CD(fn_p_stHierarchySon, 1), AI_M_CD(fn_p_stIsMorphing, 1), AI_M_CD(fn_p_stCheckAnimEnd, 1),
      AI_M_CD(fn_p_stCheckCapabilities, 1), AI_M_CD(fn_p_stCheckCapabilities, 1), AI_M_CD(fn_p_stCheckCapabilities, 1), AI_M_CD(fn_p_stCheckCapabilities, 1), AI_M_CD(fn_p_stCheckCapabilities, 1), AI_M_CD(fn_p_stCheckCapabilities, 1),
      AI_M_CD(fn_p_stMagnetActivated, 1),
      AI_M_CD(AI_fn_p_stIsNotInCollisionWith, 1), AI_M_CD(AI_fn_p_stIsNotInCollisionWith, 1), AI_M_CD(AI_fn_p_stIsNotInCollisionWith, 1), AI_M_CD(AI_fn_p_stIsNotInCollisionWith, 1), AI_M_CD(AI_fn_p_stIsNotInCollisionWith, 1),
      AI_M_CD(AI_fn_p_stIsOfFamilyOrModel, 1), AI_M_CD(AI_fn_p_stIsOfFamilyOrModel, 1),
      AI_M_CD(AI_fn_p_stIsJoypadConnected, 1), AI_M_CD(AI_fn_p_stIsKeyJustPressed, 0), AI_M_CD(AI_fn_p_stIsButtonPadJustPressed, 0),
      AI_M_CD(AI_fn_p_stIsInDemoMode, 1), AI_M_CD(fn_p_stIsInStereoMode, 0), AI_M_CD(fn_p_stIsMusicPlaying, 0), AI_M_CD(AI_fn_p_stIsSharpnessMax, 0),
      AI_M_CD(AI_fn_p_stCameraCondition, 1), AI_M_CD(AI_fn_p_stCameraCondition, 1), AI_M_CD(AI_fn_p_stCameraCondition, 1), AI_M_CD(AI_fn_p_stCameraCondition, 1), AI_M_CD(AI_fn_p_stCameraCondition, 1), AI_M_CD(AI_fn_p_stCameraCondition, 1),
      AI_M_CD(AI_fn_p_stCameraCondition, 1), AI_M_CD(AI_fn_p_stCameraCondition, 1), AI_M_CD(AI_fn_p_stCameraCondition, 1), AI_M_CD(AI_fn_p_stCameraCondition, 1), AI_M_CD(AI_fn_p_stCameraCondition, 1), AI_M_CD(AI_fn_p_stCameraCondition, 1),
      AI_M_CD(AI_fn_p_stCameraCondition, 1), AI_M_CD(AI_fn_p_stCameraCondition, 1), AI_M_CD(AI_fn_p_stCameraCondition, 1), AI_M_CD(AI_fn_p_stCameraCondition, 1), AI_M_CD(AI_fn_p_stCameraCondition, 1), AI_M_CD(AI_fn_p_stCameraCondition, 1),
      AI_M_CD(AI_fn_p_stCameraCondition, 1), AI_M_CD(AI_fn_p_stCameraCondition, 1), AI_M_CD(AI_fn_p_stCameraCondition, 1)
};
extern AI_tdstFunctionEntry  a_stFunctionTable[];    /* 0x004A5460 */
extern AI_tdstProcedureEntry a_stProcedureTable[];   /* 0x004A43E0 */
extern AI_tdstTypeTable      a_stTypeTable[];        /* 0x004A4180 */

#define M_TestActionReturn(ActionReturn, Flag)  (((ActionReturn) & (Flag)) == (Flag))

#define E_ti_EndTree               8

void __cdecl fn_vSetProcedureActionReturn(unsigned char ucActionReturn);
#define AI_pfn_fn_vSetProcedureActionReturn  fn_vSetProcedureActionReturn

unsigned char __cdecl fn_ucGetProcedureActionReturn(void);
#define AI_pfn_fn_ucGetProcedureActionReturn fn_ucGetProcedureActionReturn

/* 0x00487CF0 */
extern unsigned char __cdecl fn_ucGetFieldValue(unsigned long ulFieldId, void *p_stContext,
                                                AI_tdstGetSetParam *p_stValue);
extern void __cdecl fn_vOnAIErm_M_UpdateLastError(unsigned short FatalErrorNum, char *szMsg);

/* 0x004815F0 */
extern unsigned char __cdecl fn_ucGetDsgVar(unsigned char, unsigned char, void *, void *);

AI_tdstNodeInterpret *g_p_stTreeMacro;

unsigned char __cdecl fn_ucIsRuleAndNodeInActionTable(AI_tdstIntelligence *p_stIntelligence,
                                                      unsigned char ucNumRule,
                                                      AI_tdstNodeInterpret *p_stTree);
unsigned char __cdecl fn_ucFindPlaceInActionTable(AI_tdstIntelligence *p_stIntelligence);

/* 0x00480980 */
AI_tdstNodeInterpret *__cdecl fn_p_stSwapNextParameter2(AI_tdstNodeInterpret *p_stTree);

AI_tdstNodeInterpret *__cdecl fn_p_stIntelligenceEvalTreeEngine( HIE_tdstSuperObject *p_SuperObjPerso, AI_tdstNodeInterpret *p_stTree, AI_tdstGetSetParam *p_stValue);

/* 0x00480220 */
AI_tdstNodeInterpret *__cdecl fn_p_stEvalMacro(
    HIE_tdstSuperObject *p_SuperObjPerso, AI_tdstNodeInterpret *p_stTree, AI_tdstGetSetParam *p_stValue)
{
    unsigned char         ucActionReturn;
    AI_tdstNodeInterpret *p_stTreeMacro, *p_stTreeSave;
    AI_tdstMacro         *p_stMacro = M_p_stMacroInterpret(p_stTree);
    unsigned char         ucIndTabAction;

    p_stTreeSave    = g_p_stTreeMacro;
    g_p_stTreeMacro = p_stTree;
    g_ucIsInMacro++;

    ucIndTabAction = fn_ucIsRuleInActionTable(g_p_stIntelligence, g_ucNRule);
    if (ucIndTabAction >= AI_M_ActionTableNbEntries(g_p_stIntelligence))
    {

        AI_M_GetNodeInterpret(AI_M_GetMacroCurrentTree(p_stMacro)) =
            AI_M_GetNodeInterpret(AI_M_GetMacroInitTree(p_stMacro));
    }
    else
    {
        AI_tdstNodeInterpret *p_stTreeTmp = fn_p_stGetTableAction(g_p_stIntelligence, ucIndTabAction);
        if (p_stTreeTmp != g_p_stTreeMacro)

            AI_M_GetNodeInterpret(AI_M_GetMacroCurrentTree(p_stMacro)) =
                AI_M_GetNodeInterpret(AI_M_GetMacroInitTree(p_stMacro));
    }

    p_stTreeMacro = AI_M_GetNodeInterpret(AI_M_GetMacroCurrentTree(p_stMacro));
    fn_p_stIntelligenceEvalTreeEngine(p_SuperObjPerso, p_stTreeMacro, p_stValue);

    ucActionReturn = (unsigned char)M_GetSetParam_ActionReturnValue(p_stValue);

    g_ucIsInMacro--;
    if (M_TestActionReturn(ucActionReturn, C_TREE_FINISHED))
    {

        AI_M_GetNodeInterpret(AI_M_GetMacroCurrentTree(p_stMacro)) =
            AI_M_GetNodeInterpret(AI_M_GetMacroInitTree(p_stMacro));
        M_Full_GetSetParam_ActionReturn(p_stValue, C_ACTION_ENGINE_CONTINUE);
        g_p_stTreeMacro = p_stTreeSave;
        return ++p_stTree;
    }
    else if (M_TestActionReturn(ucActionReturn, C_ACTION_CHANGE_COMPORT))
    {
        AI_M_GetNodeInterpret(AI_M_GetMacroCurrentTree(p_stMacro)) =
            AI_M_GetNodeInterpret(AI_M_GetMacroInitTree(p_stMacro));
        g_p_stTreeMacro = p_stTreeSave;
        return ++p_stTree;
    }
    else
    {
        ucIndTabAction = fn_ucIsRuleInActionTable(g_p_stIntelligence, g_ucNRule);
        if (ucIndTabAction < AI_M_ActionTableNbEntries(g_p_stIntelligence))
        {
            AI_tdstNodeInterpret *p_stTreeTmp = fn_p_stGetTableAction(g_p_stIntelligence, ucIndTabAction);
            if (M_GetTypeInterpret(p_stTreeTmp) != AI_E_ti_MacroRef)
            {

                AI_M_GetNodeInterpret(AI_M_GetMacroCurrentTree(p_stMacro)) = p_stTreeTmp;
                fn_vUnuseCurrentActionTableEntry(g_p_stIntelligence);
                AI_M_pfn_fn_vUseCurrentActionTableEntry(g_p_stIntelligence, g_ucNRule, g_p_stTreeMacro);
            }
            g_p_stTreeMacro = p_stTreeSave;
        }
    }
    return p_stTree;
}

/* 0x00480390 */
AI_tdstNodeInterpret *__cdecl fn_p_stEvalColor(
    HIE_tdstSuperObject *p_SuperObjPerso, AI_tdstNodeInterpret *p_stTree, AI_tdstGetSetParam *p_stValue)
{
  p_SuperObjPerso=p_SuperObjPerso;
  M_Full_GetSetParam_ColorValue(p_stValue,M_ColorValueInterpret(p_stTree));
  return(++p_stTree);
}

/* 0x004803B0 */
AI_tdstNodeInterpret *__cdecl fn_p_stEvalModelRef(
    HIE_tdstSuperObject *p_SuperObjPerso, AI_tdstNodeInterpret *p_stTree, AI_tdstGetSetParam *p_stValue)
{
  p_SuperObjPerso=p_SuperObjPerso;
  M_Full_GetSetParam_p_stModel(p_stValue,M_p_stModelInterpret(p_stTree));
  return(++p_stTree);
}

/* 0x004803D0 */
AI_tdstNodeInterpret *__cdecl fn_p_stEvalField(
    HIE_tdstSuperObject *p_SuperObjPerso, AI_tdstNodeInterpret *p_stTree, AI_tdstGetSetParam *p_stValue)
{

    if (fn_ucGetFieldValue(AI_M_eFieldIdInterpret(p_stTree), p_SuperObjPerso, p_stValue)
        == AI_C_INVALID_GET)
    {
        fn_vOnAIErm_M_UpdateLastError(AI_E_uwAIFatalNotValidField, (char *)0);
    }
    return ++p_stTree;
}

AI_tdstNodeInterpret *__cdecl fn_p_stEvalDsgVarRef(
    HIE_tdstSuperObject *p_SuperObjPerso, AI_tdstNodeInterpret *p_stTree, AI_tdstGetSetParam *p_stValue)
{
    unsigned char ucId = (unsigned char)M_ucVarIdInterpret(p_stTree);

    (void)fn_ucGetDsgVar(ucId, 0, AI_M_stGetMindOfSuperObj(p_SuperObjPerso), p_stValue);

    return ++p_stTree;
}

/* 0x00480430 */
AI_tdstNodeInterpret *__cdecl fn_p_stEvalConstant(
    HIE_tdstSuperObject *p_SuperObjPerso, AI_tdstNodeInterpret *p_stTree, AI_tdstGetSetParam *p_stValue)
{
  p_SuperObjPerso=p_SuperObjPerso;
  M_Full_GetSetParam_Integer(p_stValue,M_lValueInterpret(p_stTree));
  return(++p_stTree);
}

/* 0x00480450 */
AI_tdstNodeInterpret *__cdecl fn_p_stEvalButton(
    HIE_tdstSuperObject *p_SuperObjPerso, AI_tdstNodeInterpret *p_stTree, AI_tdstGetSetParam *p_stValue)
{
  p_SuperObjPerso=p_SuperObjPerso;
  M_Full_GetSetParam_Button(p_stValue,M_hInputHandleIdInterpret(p_stTree));
  return(++p_stTree);
}

/* 0x00480470 */
AI_tdstNodeInterpret *__cdecl fn_p_stEvalSuperObjectRef(
    HIE_tdstSuperObject *p_SuperObjPerso, AI_tdstNodeInterpret *p_stTree, AI_tdstGetSetParam *p_stValue)
{
  p_SuperObjPerso=p_SuperObjPerso;
  M_GetSetParam_SuperObjectValue(p_stValue) = M_SuperObjectInterpret(p_stTree);
  M_GetSetParam_Type(p_stValue) = E_vt_SuperObject;
  return(++p_stTree);
}

/* 0x00480490 */
AI_tdstNodeInterpret *__cdecl fn_p_stEvalObjectTableRef(
    HIE_tdstSuperObject *p_SuperObjPerso, AI_tdstNodeInterpret *p_stTree, AI_tdstGetSetParam *p_stValue)
{
  p_SuperObjPerso=p_SuperObjPerso;
  M_GetSetParam_hObjectTable(p_stValue) = M_ObjectTableInterpret(p_stTree);
  M_GetSetParam_Type(p_stValue) = E_vt_ObjectTable;
  return(++p_stTree);
}

/* 0x004804B0 */
AI_tdstNodeInterpret *__cdecl fn_p_stEvalPersoRef(
    HIE_tdstSuperObject *p_SuperObjPerso, AI_tdstNodeInterpret *p_stTree, AI_tdstGetSetParam *p_stValue)
{
  p_SuperObjPerso=p_SuperObjPerso;
  M_GetSetParam_p_stEngineObjValue(p_stValue) = M_p_stPersoRefInterpret(p_stTree);
  M_GetSetParam_Type(p_stValue) = E_vt_Perso;
  return(++p_stTree);
}

/* 0x004804D0 */
AI_tdstNodeInterpret *__cdecl fn_p_stEvalActionRef(
    HIE_tdstSuperObject *p_SuperObjPerso, AI_tdstNodeInterpret *p_stTree, AI_tdstGetSetParam *p_stValue)
{
  p_SuperObjPerso=p_SuperObjPerso;
  M_GetSetParam_ActionValue(p_stValue) = M_ActionInterpret(p_stTree);
  M_GetSetParam_Type(p_stValue) = E_vt_Action;
  return(++p_stTree);
}

/* 0x004804F0 */
AI_tdstNodeInterpret *__cdecl fn_p_stEvalReal(
    HIE_tdstSuperObject *p_SuperObjPerso, AI_tdstNodeInterpret *p_stTree, AI_tdstGetSetParam *p_stValue)
{
  p_SuperObjPerso=p_SuperObjPerso;
  M_Full_GetSetParam_Float(p_stValue,M_xValueInterpret(p_stTree));
  return(++p_stTree);
}

/* 0x00480510 */
AI_tdstNodeInterpret *__cdecl fn_p_stEvalVector(
    HIE_tdstSuperObject *p_SuperObjPerso, AI_tdstNodeInterpret *p_stTree, AI_tdstGetSetParam *p_stValue)
{
  MTH_tdxReal xX,xY,xZ;

  p_stTree++;

  M_EvalNextParameter(p_stValue);
  xX=M_ReturnParam_xValue(p_stValue);

  M_EvalNextParameter(p_stValue);
  xY=M_ReturnParam_xValue(p_stValue);

  M_EvalNextParameter(p_stValue);
  xZ=M_ReturnParam_xValue(p_stValue);

  MTH3D_M_vSetVectorElements (&M_GetSetParam_stVertexValue(p_stValue), xX, xY, xZ);
  M_GetSetParam_Type(p_stValue)=E_vt_Vector;

  return(p_stTree);
}

/* 0x004805A0 */
AI_tdstNodeInterpret *__cdecl fn_p_stEvalConstantVector(
    HIE_tdstSuperObject *p_SuperObjPerso, AI_tdstNodeInterpret *p_stTree, AI_tdstGetSetParam *p_stValue)
{
  p_SuperObjPerso=p_SuperObjPerso;
  M_Full_GetSetParam_p_stVertex(p_stValue,M_p_stVectorInterpret(p_stTree));
  return(++p_stTree);
}

/* 0x004805D0 */
AI_tdstNodeInterpret *__cdecl fn_p_stEvalWayPointRef(
    HIE_tdstSuperObject *p_SuperObjPerso, AI_tdstNodeInterpret *p_stTree, AI_tdstGetSetParam *p_stValue)
{
  p_SuperObjPerso=p_SuperObjPerso;
  M_Full_GetSetParam_WayPoint(p_stValue,M_lWayPointInterpret(p_stTree));
  return(++p_stTree);
}

/* 0x004805F0 */
AI_tdstNodeInterpret *__cdecl fn_p_stEvalModuleRef(
    HIE_tdstSuperObject *p_SuperObjPerso, AI_tdstNodeInterpret *p_stTree, AI_tdstGetSetParam *p_stValue)
{
  p_SuperObjPerso=p_SuperObjPerso;
  M_Full_GetSetParam_Module(p_stValue,M_lModuleInterpret(p_stTree));
  return(++p_stTree);
}

/* 0x00480610 */
AI_tdstNodeInterpret *__cdecl fn_p_stEvalDsgVarId(
    HIE_tdstSuperObject *p_SuperObjPerso, AI_tdstNodeInterpret *p_stTree, AI_tdstGetSetParam *p_stValue)
{
  p_SuperObjPerso=p_SuperObjPerso;
  M_Full_GetSetParam_DsgVarId(p_stValue,M_lDsgVarIdInterpret(p_stTree));
  return(++p_stTree);
}

/* 0x00480630 */
AI_tdstNodeInterpret *__cdecl fn_p_stEvalMask(
    HIE_tdstSuperObject *p_SuperObjPerso, AI_tdstNodeInterpret *p_stTree, AI_tdstGetSetParam *p_stValue)
{
  p_SuperObjPerso=p_SuperObjPerso;
  M_Full_GetSetParam_Mask(p_stValue,M_xMaskInterpret(p_stTree));
  return(++p_stTree);
}

/* 0x00480650 */
AI_tdstNodeInterpret *__cdecl fn_p_stEvalString(
    HIE_tdstSuperObject *p_SuperObjPerso, AI_tdstNodeInterpret *p_stTree, AI_tdstGetSetParam *p_stValue)
{
  p_SuperObjPerso=p_SuperObjPerso;
  M_Full_GetSetParam_String(p_stValue, M_szStringInterpret(p_stTree));
  return(++p_stTree);
}

/* 0x00480670 */
AI_tdstNodeInterpret *__cdecl fn_p_stEvalLipsSynchroRef(
    HIE_tdstSuperObject *p_SuperObjPerso, AI_tdstNodeInterpret *p_stTree, AI_tdstGetSetParam *p_stValue)
{
  p_SuperObjPerso=p_SuperObjPerso;
  M_Full_GetSetParam_hLipsSynchro(p_stValue, M_hLipSynchroHandleIdInterpret(p_stTree));
  return(++p_stTree);
}

/* 0x00480690 */
AI_tdstNodeInterpret *__cdecl fn_p_stEvalSoundEventRef(
    HIE_tdstSuperObject *p_SuperObjPerso, AI_tdstNodeInterpret *p_stTree, AI_tdstGetSetParam *p_stValue)
{
  p_SuperObjPerso=p_SuperObjPerso;
  M_Full_GetSetParam_SoundEvent(p_stValue, M_SoundEventInterpret(p_stTree));
  return(++p_stTree);
}

/* 0x004806B0 */
AI_tdstNodeInterpret *__cdecl fn_p_stEvalFamilyRef(
    HIE_tdstSuperObject *p_SuperObjPerso, AI_tdstNodeInterpret *p_stTree, AI_tdstGetSetParam *p_stValue)
{
  p_SuperObjPerso=p_SuperObjPerso;
  M_Full_GetSetParam_hFamily(p_stValue, M_hFamilyHandleIdInterpret(p_stTree));
  return(++p_stTree);
}

/* 0x004806D0 */
AI_tdstNodeInterpret *__cdecl fn_p_stEvalEndOfTree(
    HIE_tdstSuperObject  *p_SuperObjPerso,
    AI_tdstNodeInterpret *p_stTree,
    AI_tdstGetSetParam   *p_stValue)
{
    (void)p_SuperObjPerso;
    M_Full_GetSetParam_ActionReturn(p_stValue, C_TREE_FINISHED);
    return p_stTree;
}

#define AI_M_TY(f)  { (AI_tdp_fn_p_stEvalTypePtr)(f) }

AI_tdstNodeInterpret *__cdecl fn_p_stEvalFunction( HIE_tdstSuperObject *p_SuperObjPerso, AI_tdstNodeInterpret *p_stTree, AI_tdstGetSetParam *p_stValue);
AI_tdstNodeInterpret *__cdecl fn_p_stEvalOperator( HIE_tdstSuperObject *p_SuperObjPerso, AI_tdstNodeInterpret *p_stTree, AI_tdstGetSetParam *p_stValue);
AI_tdstNodeInterpret *__cdecl fn_p_stEvalProcedure( HIE_tdstSuperObject *p_SuperObjPerso, AI_tdstNodeInterpret *p_stTree, AI_tdstGetSetParam *p_stValue);
AI_tdstNodeInterpret *__cdecl fn_p_stEvalBeginMacro( HIE_tdstSuperObject *p_SuperObjPerso, AI_tdstNodeInterpret *p_stTree, AI_tdstGetSetParam *p_stValue);
AI_tdstNodeInterpret *__cdecl fn_p_stEvalComportRef( HIE_tdstSuperObject *p_SuperObjPerso, AI_tdstNodeInterpret *p_stTree, AI_tdstGetSetParam *p_stValue);
AI_tdstNodeInterpret *__cdecl fn_p_stEvalTextRef( HIE_tdstSuperObject *p_SuperObjPerso, AI_tdstNodeInterpret *p_stTree, AI_tdstGetSetParam *p_stValue);
AI_tdstNodeInterpret *__cdecl fn_p_stEvalGameMaterialRef( HIE_tdstSuperObject *p_SuperObjPerso, AI_tdstNodeInterpret *p_stTree, AI_tdstGetSetParam *p_stValue);
AI_tdstNodeInterpret *__cdecl fn_p_stEvalParticleGenerator( HIE_tdstSuperObject *p_SuperObjPerso, AI_tdstNodeInterpret *p_stTree, AI_tdstGetSetParam *p_stValue);
AI_tdstNodeInterpret *__cdecl fn_p_stEvalLight( HIE_tdstSuperObject *p_SuperObjPerso, AI_tdstNodeInterpret *p_stTree, AI_tdstGetSetParam *p_stValue);
AI_tdstNodeInterpret *__cdecl fn_p_stEvalCaps( HIE_tdstSuperObject *p_SuperObjPerso, AI_tdstNodeInterpret *p_stTree, AI_tdstGetSetParam *p_stValue);
AI_tdstNodeInterpret *__cdecl fn_p_stEvalGraph( HIE_tdstSuperObject *p_SuperObjPerso, AI_tdstNodeInterpret *p_stTree, AI_tdstGetSetParam *p_stValue);
AI_tdstNodeInterpret *__cdecl fn_p_stEvalNULL( HIE_tdstSuperObject *p_SuperObjPerso, AI_tdstNodeInterpret *p_stTree, AI_tdstGetSetParam *p_stValue);
AI_tdstNodeInterpret *__cdecl fn_p_stEvalMetaAction( HIE_tdstSuperObject *p_SuperObjPerso, AI_tdstNodeInterpret *p_stTree, AI_tdstGetSetParam *p_stValue);
AI_tdstTypeTable a_stTypeTable[41] =
{
      AI_M_TY(fn_p_stEvalKeyWord), AI_M_TY(fn_p_stEvalCondition), AI_M_TY(fn_p_stEvalOperator), AI_M_TY(fn_p_stEvalFunction),
      AI_M_TY(fn_p_stEvalProcedure), AI_M_TY(fn_p_stEvalMetaAction), AI_M_TY(fn_p_stEvalBeginMacro), AI_M_TY(fn_p_stEvalBeginMacro),
      AI_M_TY(fn_p_stEvalEndOfTree), AI_M_TY(fn_p_stEvalField), AI_M_TY(fn_p_stEvalDsgVarRef), AI_M_TY(fn_p_stEvalDsgVarRef),
      AI_M_TY(fn_p_stEvalConstant), AI_M_TY(fn_p_stEvalReal), AI_M_TY(fn_p_stEvalButton), AI_M_TY(fn_p_stEvalConstantVector),
      AI_M_TY(fn_p_stEvalVector), AI_M_TY(fn_p_stEvalMask), AI_M_TY(fn_p_stEvalModuleRef), AI_M_TY(fn_p_stEvalDsgVarId),
      AI_M_TY(fn_p_stEvalString), AI_M_TY(fn_p_stEvalLipsSynchroRef), AI_M_TY(fn_p_stEvalFamilyRef), AI_M_TY(fn_p_stEvalPersoRef),
      AI_M_TY(fn_p_stEvalActionRef), AI_M_TY(fn_p_stEvalSuperObjectRef), AI_M_TY(fn_p_stEvalWayPointRef), AI_M_TY(fn_p_stEvalTextRef),
      AI_M_TY(fn_p_stEvalComportRef), AI_M_TY(fn_p_stEvalModuleRef), AI_M_TY(fn_p_stEvalSoundEventRef), AI_M_TY(fn_p_stEvalObjectTableRef),
      AI_M_TY(fn_p_stEvalGameMaterialRef), AI_M_TY(fn_p_stEvalParticleGenerator), AI_M_TY(fn_p_stEvalColor), AI_M_TY(fn_p_stEvalModelRef),
      AI_M_TY(fn_p_stEvalLight), AI_M_TY(fn_p_stEvalCaps), AI_M_TY(fn_p_stEvalGraph), AI_M_TY(fn_p_stEvalMacro),
      AI_M_TY(fn_p_stEvalNULL)
};

/* 0x004806F0 */
AI_tdstNodeInterpret *__cdecl fn_p_stEvalBeginMacro(
    HIE_tdstSuperObject  *p_SuperObjPerso,
    AI_tdstNodeInterpret *p_stTree,
    AI_tdstGetSetParam   *p_stValue)
{
    (void)p_SuperObjPerso;
    M_Full_GetSetParam_ActionReturn(p_stValue, C_ACTION_ENGINE_CONTINUE);
    return ++p_stTree;
}

/* 0x00480710 */
AI_tdstNodeInterpret *__cdecl fn_p_stEvalComportRef(
    HIE_tdstSuperObject *p_SuperObjPerso, AI_tdstNodeInterpret *p_stTree, AI_tdstGetSetParam *p_stValue)
{
  p_SuperObjPerso=p_SuperObjPerso;
  M_Full_GetSetParam_pComport(p_stValue, M_ComportInterpret(p_stTree));
  return(++p_stTree);
}

/* 0x00480730 */
AI_tdstNodeInterpret *__cdecl fn_p_stEvalTextRef(
    HIE_tdstSuperObject *p_SuperObjPerso, AI_tdstNodeInterpret *p_stTree, AI_tdstGetSetParam *p_stValue)
{
  p_SuperObjPerso=p_SuperObjPerso;
  M_Full_GetSetParam_hText(p_stValue, M_TextInterpret(p_stTree));
  return(++p_stTree);
}

/* 0x00480750 */
AI_tdstNodeInterpret *__cdecl fn_p_stEvalParticleGenerator(
    HIE_tdstSuperObject *p_SuperObjPerso, AI_tdstNodeInterpret *p_stTree, AI_tdstGetSetParam *p_stValue)
{
  p_SuperObjPerso=p_SuperObjPerso;
  M_Full_GetSetParam_hParticleGenerator(p_stValue, M_ParticleGeneratorInterpret(p_stTree));
  return(++p_stTree);
}

/* 0x00480770 */
AI_tdstNodeInterpret *__cdecl fn_p_stEvalGameMaterialRef(
    HIE_tdstSuperObject *p_SuperObjPerso, AI_tdstNodeInterpret *p_stTree, AI_tdstGetSetParam *p_stValue)
{
  p_SuperObjPerso=p_SuperObjPerso;
  M_Full_GetSetParam_hGameMaterial(p_stValue, M_GameMaterialInterpret(p_stTree));
  return(++p_stTree);
}

/* 0x00480790 */
AI_tdstNodeInterpret *__cdecl fn_p_stEvalLight(
    HIE_tdstSuperObject *p_SuperObjPerso, AI_tdstNodeInterpret *p_stTree, AI_tdstGetSetParam *p_stValue)
{
  p_SuperObjPerso=p_SuperObjPerso;
  M_Full_GetSetParam_Light(p_stValue,M_p_stLightInterpret(p_stTree));
  return(++p_stTree);
}

/* 0x004807B0 */
AI_tdstNodeInterpret *__cdecl fn_p_stEvalCaps(
    HIE_tdstSuperObject *p_SuperObjPerso, AI_tdstNodeInterpret *p_stTree, AI_tdstGetSetParam *p_stValue)
{
  p_SuperObjPerso=p_SuperObjPerso;
  M_Full_GetSetParam_Caps(p_stValue, M_ulCapsInterpret(p_stTree));
  return(++p_stTree);
}

/* 0x004807D0 */
AI_tdstNodeInterpret *__cdecl fn_p_stEvalGraph(
    HIE_tdstSuperObject *p_SuperObjPerso, AI_tdstNodeInterpret *p_stTree, AI_tdstGetSetParam *p_stValue)
{
  p_SuperObjPerso=p_SuperObjPerso;
  M_Full_GetSetParam_Graph(p_stValue,M_p_stGraphInterpret(p_stTree));
  return(++p_stTree);
}

/* 0x004807F0 */
AI_tdstNodeInterpret *__cdecl fn_p_stEvalNULL(
    HIE_tdstSuperObject *p_SuperObjPerso, AI_tdstNodeInterpret *p_stTree, AI_tdstGetSetParam *p_stValue)
{
    (void)p_SuperObjPerso;
    (void)p_stValue;
    return ++p_stTree;
}

AI_tdstNodeInterpret *__cdecl fn_p_stEvalKeyWord(
    HIE_tdstSuperObject  *p_SuperObjPerso,
    AI_tdstNodeInterpret *p_stTree,
    AI_tdstGetSetParam   *p_stValue)
{

    unsigned long eKeyWordId = M_eKeyWordIdInterpret(p_stTree++);
    AI_tdp_fn_p_stKeyWordFunctionPtr p_fn_p_stKeyWordFunctionPtr =
        AI_M_pfn_fn_p_fn_pGetKeyWordFunctionPtr(eKeyWordId);

    return p_fn_p_stKeyWordFunctionPtr(p_SuperObjPerso, p_stTree, p_stValue);
}

AI_tdstNodeInterpret *__cdecl fn_p_stEvalCondition(
    HIE_tdstSuperObject  *p_SuperObjPerso,
    AI_tdstNodeInterpret *p_stTree,
    AI_tdstGetSetParam   *p_stValue)
{
  long lValue=(long) FALSE;
  unsigned long eCondId=M_eCondIdInterpret(p_stTree++);
   AI_tdp_fn_p_stConditionFunctionPtr p_fn_p_stConditionFunctionPtr=AI_M_pfn_fn_p_fn_pGetConditionFunctionPtr(eCondId);

  p_stTree = p_fn_p_stConditionFunctionPtr(p_SuperObjPerso,p_stTree,&lValue);

  M_Full_GetSetParam_Integer(p_stValue, ((lValue !=FALSE)?TRUE:FALSE) );

  return(p_stTree);
}

/* 0x00480880 */
AI_tdstNodeInterpret *__cdecl fn_p_stEvalFunction(
    HIE_tdstSuperObject  *p_SuperObjPerso,
    AI_tdstNodeInterpret *p_stTree,
    AI_tdstGetSetParam   *p_stValue)
{

    unsigned long eFuncId = AI_M_eFuncIdInterpret(p_stTree++);
    AI_tdp_fn_p_stFunctionFunctionPtr p_fn_p_stFunctionFunctionPtr =
        AI_M_pfn_fn_p_fn_pGetFunctionFunctionPtr(eFuncId);

    return p_fn_p_stFunctionFunctionPtr(p_SuperObjPerso, p_stTree, p_stValue);
}

/* 0x004808B0 */
AI_tdstNodeInterpret *__cdecl fn_p_stEvalOperator(
    HIE_tdstSuperObject  *p_SuperObjPerso,
    AI_tdstNodeInterpret *p_stTree,
    AI_tdstGetSetParam   *p_stValue)
{

    unsigned long eOperatorId = AI_M_eOperatorIdInterpret(p_stTree++);
    AI_tdp_fn_p_stOperatorFunctionPtr p_fn_p_stOperatorFunctionPtr =
        AI_M_pfn_fn_p_fn_pGetOperatorFunctionPtr(eOperatorId);

    return p_fn_p_stOperatorFunctionPtr(p_SuperObjPerso, p_stTree, p_stValue);
}

/* 0x004808E0 */
void __cdecl fn_vSetProcedureActionReturn(unsigned char ucActionReturn)
{
    AI_g_ucProcedureActionReturn = ucActionReturn;
}
unsigned char __cdecl fn_ucGetProcedureActionReturn(void)
{
    return AI_g_ucProcedureActionReturn;
}

/* 0x00480900 */
AI_tdstNodeInterpret *__cdecl fn_p_stEvalProcedure(
    HIE_tdstSuperObject  *p_SuperObjPerso,
    AI_tdstNodeInterpret *p_stTree,
    AI_tdstGetSetParam   *p_stValue)
{
    unsigned long eProcedureId = AI_M_eProcedureIdInterpret(p_stTree);
    AI_tdp_fn_p_stProcedureFunctionPtr p_fn_p_stProcedureFunctionPtr;

    p_fn_p_stProcedureFunctionPtr = AI_M_pfn_fn_p_fn_pGetProcedureFunctionPtr(eProcedureId);

    AI_pfn_fn_vSetProcedureActionReturn(C_ACTION_ENGINE_CONTINUE);

    p_stTree = p_fn_p_stProcedureFunctionPtr(p_SuperObjPerso, ++p_stTree);

    M_Full_GetSetParam_ActionReturn(p_stValue,
        AI_pfn_fn_ucGetProcedureActionReturn() & ~C_ACTION_FINISHED);

    return p_stTree;
}

/* 0x004809B0 */
AI_tdstNodeInterpret *__cdecl fn_p_stSkipThisArgument(AI_tdstNodeInterpret *p_stTree)
{
    unsigned char ucDepth = p_stTree->ucDepth;

    p_stTree++;

    while (p_stTree->ucDepth > ucDepth && p_stTree->eType != E_ti_EndTree)
        p_stTree++;

    return p_stTree;
}

static unsigned char fn_ucComputeActionNextPerso(HIE_tdstSuperObject *p_SuperObjPerso, enum tdeMetaActionId_ eMetaActionId);

/* 0x004809E0 */
AI_tdstNodeInterpret *fn_p_stComputeActionStartPerso(HIE_tdstSuperObject *p_SuperObjPerso,
                                                     AI_tdstNodeInterpret *p_stTree,
                                                     enum tdeMetaActionId_ eMetaActionId,
                                                     AI_tdstGetSetParam *p_stValue)
{
    tdp_fn_p_stMetaActionInitFunctionPtr p_fn_ucMetaActionInitFunctionPtr = fn_p_fn_pGetMetaActionInitFunctionPtr(eMetaActionId);
    p_stTree = p_fn_ucMetaActionInitFunctionPtr(p_SuperObjPerso, p_stTree);

    M_Full_GetSetParam_ActionReturn(p_stValue, (long) fn_ucComputeActionNextPerso(p_SuperObjPerso, eMetaActionId));
    return p_stTree;
}

static unsigned char fn_ucComputeActionNextPerso(HIE_tdstSuperObject *p_SuperObjPerso,
                                                 enum tdeMetaActionId_ eMetaActionId)
{
    tdp_fn_ucMetaActionFunctionPtr p_fn_ucMetaActionFunctionPtr = fn_p_fn_pGetMetaActionFunctionPtr(eMetaActionId);
    return(p_fn_ucMetaActionFunctionPtr(p_SuperObjPerso));
}

/* 0x00480A50 */
AI_tdstNodeInterpret *__cdecl fn_p_stEvalMetaAction(
    HIE_tdstSuperObject *p_SuperObjPerso, AI_tdstNodeInterpret *p_stTree, AI_tdstGetSetParam *p_stValue)
{
    AI_tdstNodeInterpret *p_stTreeForActionTable = (g_ucIsInMacro != 0) ? g_p_stTreeMacro : p_stTree;
    unsigned long         eMetaActionId;

    if ((AI_M_ActionTableCurrentEntry(g_p_stIntelligence) =
            fn_ucIsRuleAndNodeInActionTable(g_p_stIntelligence, g_ucNRule, p_stTreeForActionTable))
        < AI_M_ActionTableNbEntries(g_p_stIntelligence))
    {
        eMetaActionId = M_eMetaActionIdInterpret(p_stTree);

        if (AI_M_NodeToSkip(p_stTree))
        {
            p_stTree += AI_M_NodeToSkip(p_stTree);
        }
        else
        {
            AI_tdstNodeInterpret *p_stTreeInit = p_stTree;
            p_stTree = AI_M_pfn_fn_p_stSwapNextParameter2(p_stTree + 1);
            AI_M_NodeToSkip(p_stTreeInit) = (unsigned short)(p_stTree - p_stTreeInit);
        }

        M_Full_GetSetParam_ActionReturn(p_stValue,
            (long)fn_ucComputeActionNextPerso(p_SuperObjPerso, eMetaActionId));
        bSomethingDone = TRUE;
        return p_stTree;
    }
    else
    {
        fn_vUnuseAllRulesWhichHaveGreaterNumRule(g_p_stIntelligence, g_ucNRule);

        if ((AI_M_ActionTableCurrentEntry(g_p_stIntelligence) =
                fn_ucFindPlaceInActionTable(g_p_stIntelligence))
            < AI_M_ActionTableNbEntries(g_p_stIntelligence))
        {

            AI_M_pfn_fn_vUseCurrentActionTableEntry(g_p_stIntelligence, g_ucNRule, p_stTree);

            eMetaActionId = M_eMetaActionIdInterpret(p_stTree);
            p_stTree++;

            bSomethingDone = TRUE;
            return fn_p_stComputeActionStartPerso(p_SuperObjPerso, p_stTree, eMetaActionId, p_stValue);
        }
        else
        {

            AI_M_pfn_fn_vAIFatalError(AI_E_uwAIActionTableTooSmall);
            M_Full_GetSetParam_ActionReturn(p_stValue, 0);
            return 0;
        }
    }
}

AI_tdstNodeInterpret *__cdecl fn_p_stEvalTree(
    HIE_tdstSuperObject  *p_SuperObjPerso,
    AI_tdstNodeInterpret *p_stTree,
    AI_tdstGetSetParam   *p_stValue)
{
    return a_stTypeTable[M_GetTypeInterpret(p_stTree)].p_fn_p_stEvalTypePtr(
        p_SuperObjPerso, p_stTree, p_stValue);
}

/* 0x480BB0 */
unsigned char __cdecl fn_ucIsThereAnotherCommand(AI_tdstNodeInterpret *p_stTree, unsigned char ucDepth)
{
    return (unsigned char)(p_stTree->ucDepth == ucDepth);
}

AI_tdstNodeInterpret *__cdecl fn_p_stIntelligenceEvalTreeEngine(
    HIE_tdstSuperObject *p_SuperObjPerso, AI_tdstNodeInterpret *p_stTree,
    AI_tdstGetSetParam *p_stValue)
{
    unsigned char ucActionReturn;
    unsigned char ucDepthInit;

ComputeActionReturn:
    ucDepthInit = p_stTree->ucDepth;
    p_stTree = fn_p_stEvalTree(p_SuperObjPerso, p_stTree, p_stValue);
    ucActionReturn = fn_ucGetActionReturn(p_stValue, p_stTree, ucDepthInit);

    if (M_TestActionReturn(ucActionReturn, C_TREE_FINISHED)
        || M_TestActionReturn(ucActionReturn, C_ACTION_CHANGE_COMPORT))
    {
        M_Full_GetSetParam_ActionReturn(p_stValue, ucActionReturn);
        return p_stTree;
    }

    if (M_TestActionReturn(ucActionReturn, C_ACTION_FINISHED))
        fn_vUnuseCurrentActionTableEntry(g_p_stIntelligence);

    if (M_TestActionReturn(ucActionReturn, C_ACTION_ENGINE_STOP))
    {
        fn_vUnuseAllRulesWhichHaveGreaterNumRule(g_p_stIntelligence, g_ucNRule);
    }
    else
    {

        if (p_stTree->eType == E_ti_EndTree)
            ucActionReturn = C_TREE_FINISHED;
        else if (!M_TestActionReturn(ucActionReturn, C_ACTION_ENGINE_STOP_RULE))
        {
            if (ucIsInSchedule)
                AI_M_GetCurrentSchedule(g_p_stIntelligence) = p_stTree;
            goto ComputeActionReturn;
        }
    }

    M_Full_GetSetParam_ActionReturn(p_stValue, ucActionReturn);
    return p_stTree;
}

/* 0x480D10 */
unsigned char __cdecl fn_ucGetActionReturn(AI_tdstGetSetParam *p_stValue,
                                           AI_tdstNodeInterpret *p_stTree, unsigned char ucDepth)
{
    unsigned char ucActionReturn = (unsigned char)M_GetSetParam_ActionReturnValue(p_stValue);

    if (!M_TestActionReturn(g_ucNewActionReturn, C_ACTION_ENGINE_NOTDEFINED))
    {
        if (!g_ucUseDefaultActionReturn)
        {

            if (!(M_TestActionReturn(ucActionReturn, C_ACTION_FINISHED)
                  && fn_ucIsThereAnotherCommand(p_stTree, ucDepth)))
            {
                ucActionReturn &= (C_ACTION_CHANGE_COMPORT | C_TREE_FINISHED | C_ACTION_FINISHED);
                ucActionReturn |= g_ucNewActionReturn;
            }
            g_ucNewActionReturn = C_ACTION_ENGINE_NOTDEFINED;
            g_ucUseDefaultActionReturn = TRUE;
        }
        else
            g_ucUseDefaultActionReturn = FALSE;
    }
    return ucActionReturn;
}

ACP_tdxBool __cdecl fn_bIntelligenceEngine(HIE_tdstSuperObject *p_SuperObjPerso,
                                           AI_tdstIntelligence *p_stIntelligence)
{
    ACP_tdxBool   ReturnValue = FALSE;
    unsigned char ucReturn;

    do
    {

        bSomethingDone = FALSE;
        ucReturn = fn_ucIntelligenceRulesEngine(p_SuperObjPerso, p_stIntelligence);
        if (!M_TestActionReturn(ucReturn, C_ACTION_CHANGE_COMPORT))
        {

            if (M_TestActionReturn(ucReturn, C_ACTION_ENGINE_STOP) || bSomethingDone)
                fn_vComputeScheduleInterrupt(p_stIntelligence);
            else

                ucReturn = fn_ucIntelligenceScheduleEngine(p_SuperObjPerso, p_stIntelligence);
        }
        ReturnValue |= bSomethingDone;
    }
    while (M_TestActionReturn(ucReturn, C_ACTION_CHANGE_COMPORT)
           && !M_TestActionReturn(ucReturn, C_ACTION_ENGINE_STOP));

    return ReturnValue;
}

/* 0x480E00 */
unsigned char __cdecl fn_ucIntelligenceRulesEngine(HIE_tdstSuperObject *p_SuperObjPerso,
                                                    AI_tdstIntelligence *p_stIntelligence)
{
    char                  cNRule;
    unsigned char         ucIndTabAction;
    AI_tdstGetSetParam    stValue;
    unsigned char         ucReturn;
    AI_tdstNodeInterpret *p_stTree;
    char                  cNIntelligenceRule;
    unsigned char         ucTableNbEntries;

    g_p_stIntelligence = p_stIntelligence;
    ucIsInSchedule = FALSE;

BeginComport:
    cNIntelligenceRule = (char)(AI_M_GetNbRules(p_stIntelligence) - 1);
    ucTableNbEntries   = AI_M_ActionTableNbEntries(p_stIntelligence);

    for (cNRule = 0; cNRule <= cNIntelligenceRule; cNRule++)
    {
        g_ucNRule = (unsigned char)(cNRule + 1);

        ucIndTabAction = fn_ucIsRuleInActionTable(p_stIntelligence, g_ucNRule);
        if (ucIndTabAction < ucTableNbEntries)
            p_stTree = fn_p_stGetTableAction(p_stIntelligence, ucIndTabAction);
        else
            p_stTree = AI_M_GetNodeInterpretN(AI_M_GetRuleN(p_stIntelligence, cNRule), 0);

        p_stTree = fn_p_stIntelligenceEvalTreeEngine(p_SuperObjPerso, p_stTree, &stValue);
        ucReturn = (unsigned char)M_GetSetParam_ActionReturnValue(&stValue);

        if (M_TestActionReturn(ucReturn, C_ACTION_ENGINE_STOP))
            return C_ACTION_ENGINE_STOP;

        if (M_TestActionReturn(ucReturn, C_ACTION_CHANGE_COMPORT))
        {
            bSomethingDone = FALSE;
            goto BeginComport;
        }
        if (!M_TestActionReturn(ucReturn, C_TREE_FINISHED))
            return C_ACTION_ENGINE_CONTINUE;

    }
    return C_ACTION_ENGINE_CONTINUE;
}

/* 0x480ED0 */
unsigned char __cdecl fn_ucIntelligenceScheduleEngine(HIE_tdstSuperObject *p_SuperObjPerso,
                                                       AI_tdstIntelligence *p_stIntelligence)
{
    AI_tdstNodeInterpret *p_stTree;
    unsigned char         ucIndTabAction;
    AI_tdstGetSetParam    stValue;
    unsigned char         ucReturn;

    g_p_stIntelligence = p_stIntelligence;
    ucIsInSchedule = TRUE;

    p_stTree = AI_M_GetCurrentSchedule(p_stIntelligence);
    if (p_stTree == NULL)
        return C_TREE_FINISHED;

    g_ucNRule = 0;
    ucIndTabAction = fn_ucIsRuleInActionTable(p_stIntelligence, g_ucNRule);
    if (ucIndTabAction < AI_M_ActionTableNbEntries(p_stIntelligence))
        p_stTree = fn_p_stGetTableAction(p_stIntelligence, ucIndTabAction);

    if (M_GetTypeInterpret(p_stTree) == E_ti_EndTree)
        p_stTree = AI_M_GetTreeComportSchedule((AI_tdstComport *)AI_M_GetCurrentComport(p_stIntelligence));

    p_stTree = fn_p_stIntelligenceEvalTreeEngine(p_SuperObjPerso, p_stTree, &stValue);
    ucReturn = (unsigned char)M_GetSetParam_ActionReturnValue(&stValue);

    if ((!M_TestActionReturn(ucReturn, C_ACTION_CHANGE_COMPORT))
        && (!M_TestActionReturn(ucReturn, C_ACTION_ENGINE_STOP_RULE)))
        AI_M_GetCurrentSchedule(p_stIntelligence) = p_stTree;

    return ucReturn;
}

/* 0x480F70 */
void __cdecl fn_vComputeScheduleInterrupt(AI_tdstIntelligence *p_stIntelligence)
{
    unsigned char ucIndTabAction = fn_ucIsRuleInActionTable(p_stIntelligence, 0);

    if (ucIndTabAction < AI_M_ActionTableNbEntries(p_stIntelligence))
    {

        AI_M_GetCurrentSchedule(p_stIntelligence) =
            fn_p_stGetTableAction(p_stIntelligence, ucIndTabAction);

        fn_vUnuseAllRulesFromSchedule(p_stIntelligence);
    }
}

AI_tdp_fn_p_stKeyWordFunctionPtr __cdecl fn_p_fn_pGetKeyWordFunctionPtr(unsigned long eKeyWordId)
{
    return a_pfnKeyWordTable[eKeyWordId];
}
/* 0x00487D60 */
unsigned char __cdecl fn_ucGetConditionUltraOperator(unsigned long eCondId)
{
    return a_stConditionTable[eCondId].ucUltraOperator;
}
AI_tdp_fn_p_stConditionFunctionPtr __cdecl fn_p_fn_pGetConditionFunctionPtr(unsigned long eCondId)
{
    return a_stConditionTable[eCondId].p_fn_pCondition;
}
AI_tdp_fn_p_stOperatorFunctionPtr __cdecl fn_p_fn_pGetOperatorFunctionPtr(unsigned long eOperatorId)
{
    return a_pfnOperatorTable[eOperatorId];
}
