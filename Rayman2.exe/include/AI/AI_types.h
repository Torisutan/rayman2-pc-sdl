#pragma once

#include <stddef.h>
#include "ACP/ACP_types.h"
#include "MTH/MTH_types.h"
#include "HIE/HIE_types.h"
#include "AI/AI_globals.h"

typedef struct AI_tdstActionTableEntry_
{
    unsigned char  a_ucActionParamBlob[0x60];
    void          *p_stNode;
    unsigned char  bUsed;
    unsigned char  ucNumRule;
    unsigned char  bUseDefaultActionReturn;
    unsigned char  ucNewActionReturn;
} AI_tdstActionTableEntry;

typedef struct AI_tdstActionTable_
{
    AI_tdstActionTableEntry *p_stEntry;
    unsigned char            ucNbEntries;
    unsigned char            ucNbEntriesUsed;
    unsigned char            ucCurrentEntry;
    unsigned char            _ucPad;
} AI_tdstActionTable;

extern unsigned char g_ucUseDefaultActionReturn;
extern unsigned char g_ucNewActionReturn;

AI_tdstActionTableEntry *__cdecl fn_p_stSimulateAllocActionTableEntry(
    char **p_ptr, unsigned char ucActionTableNbEntry);

AI_tdstActionTable *__cdecl fn_p_stSimulateAllocActionTable(
    char **p_ptr, unsigned char ucActionTableNbEntry);

struct AI_tdstAIModel_;
struct AI_tdstIntelligence_;
struct AI_tdstDsgMem_;

typedef struct AI_tdstMind_
{
    struct AI_tdstAIModel_      *p_stAIModel;
    struct AI_tdstIntelligence_ *p_stIntelligence;
    struct AI_tdstIntelligence_ *p_stReflex;
    struct AI_tdstDsgMem_       *p_stDsgMem;
    ACP_tdxBool                  bDoingIntel;
} AI_tdstMind;
typedef AI_tdstMind             *AI_tdxHandleToMind;

#define AI_M_SetDoingIntel(p_stMind, DoingIntel)  ((p_stMind)->bDoingIntel = (DoingIntel))
#define AI_M_p_stGetIntelligence(p_stMind)        ((p_stMind)->p_stIntelligence)
#define AI_M_p_stGetReflex(p_stMind)              ((p_stMind)->p_stReflex)

struct AI_tdstDsgVar_;
typedef struct AI_tdstDsgVar_    AI_tdstDsgVar;

typedef struct AI_tdstScriptAI_
{
    void          *p_stComport;
    unsigned long  ulNbComport;
    unsigned long  ulNoComportInitDefault;
    unsigned char  ucActionTableNbEntry;
} AI_tdstScriptAI;

typedef struct AI_tdstDsgMem_
{
    AI_tdstDsgVar **p_p_stDsgVar;
    char           *p_cDsgMemBufferInit;
    char           *p_cDsgMemBuffer;
} AI_tdstDsgMem;

typedef struct AI_tdstIntelligence_
{
    AI_tdstScriptAI **p_p_stScriptAI;
    void             *p_stCurrentSchedule;
    void             *p_stCurrentComport;
    void             *p_stPrevComport;
    AI_tdstActionTable *p_stActionTable;
    unsigned long     ulNoComportInit;
} AI_tdstIntelligence;

extern unsigned char AI_g_ucMemoryBlockUsed;
/* 0x004A417C */
extern unsigned char g_ucAIModuleId;

#define C_ucMmgDefaultChannel ((unsigned char)0)

#define AI_M_pfn_Mmg_fn_v_SetModeAlloc  Mmg_fn_v_SetModeAlloc   /* 0x00443360 */
#define AI_M_pfn_Mmg_fn_p_vAlloc4Ch     fn_p_vGenAlloc          /* 0x00443120 */

#define AI_M_ChooseMemoryBlock() \
    AI_M_pfn_Mmg_fn_v_SetModeAlloc(g_ucAIModuleId, AI_g_ucMemoryBlockUsed, C_ucMmgDefaultChannel)

void  *__cdecl AI_fn_p_vTrueAlloc(unsigned long ulSize);
void   __cdecl AI_fn_vTrueFree(void *p_vPointer);
void   __cdecl fn_vReinitAI(AI_tdstMind *p_stMind);
char  *__cdecl fn_p_cSimulateAllocDsgMemBuffer(char **p_ptr,
                                                  unsigned long ulSize);
AI_tdstDsgMem *__cdecl fn_p_stSimulateAllocDsgMem(
    char **p_ptr, AI_tdstDsgVar **p_p_stDsgVar, unsigned long ulSize);
AI_tdstIntelligence *__cdecl fn_p_stSimulateAllocIntelligence(
    char **p_ptr, AI_tdstScriptAI **p_p_stScriptAI,
    unsigned char ucActionTableNbEntry);

#include "AI/GSParam.h"

typedef struct AI_tdstNodeInterpret_
{

    union
    {
        unsigned long                uId;
        AI_tduGetSetParamForNode     uParam;
    };
    unsigned short uwNodeToSkip;
    unsigned char  ucDepth;
    unsigned char  eType;
} AI_tdstNodeInterpret;

/* 0x004809B0 */
AI_tdstNodeInterpret *__cdecl fn_p_stSkipThisArgument(AI_tdstNodeInterpret *p_stTree);

/* 0x004829D0 */
char *__cdecl fn_szGetStringFromTextOrStringParam(AI_tdstGetSetParam *p_stValue);

#define AI_M_eKeyWordIdInterpret(p_Tree)   ((p_Tree)->uId)
#define AI_M_eCondIdInterpret(p_Tree)      ((p_Tree)->uId)
#define AI_M_GetTypeInterpret(p_Tree)      ((p_Tree)->eType)

typedef AI_tdstNodeInterpret *(__cdecl *AI_tdp_fn_p_stEvalTypePtr)(
    HIE_tdstSuperObject *, AI_tdstNodeInterpret *, AI_tdstGetSetParam *);

typedef struct AI_tdstTypeTable_
{
    AI_tdp_fn_p_stEvalTypePtr p_fn_p_stEvalTypePtr;
} AI_tdstTypeTable;

/* 0x004A4180 */
typedef AI_tdstNodeInterpret *(__cdecl *AI_tdp_fn_p_stKeyWordFunctionPtr)(
    HIE_tdstSuperObject *, AI_tdstNodeInterpret *, AI_tdstGetSetParam *);

typedef AI_tdstNodeInterpret *(__cdecl *AI_tdp_fn_p_stConditionFunctionPtr)(
    HIE_tdstSuperObject *, AI_tdstNodeInterpret *, long *);

AI_tdp_fn_p_stKeyWordFunctionPtr   __cdecl fn_p_fn_pGetKeyWordFunctionPtr(unsigned long eKeyWordId);
AI_tdp_fn_p_stConditionFunctionPtr __cdecl fn_p_fn_pGetConditionFunctionPtr(unsigned long eCondId);
#define AI_M_pfn_fn_p_fn_pGetKeyWordFunctionPtr    fn_p_fn_pGetKeyWordFunctionPtr
#define AI_M_pfn_fn_p_fn_pGetConditionFunctionPtr  fn_p_fn_pGetConditionFunctionPtr

typedef AI_tdstNodeInterpret *(__cdecl *AI_tdp_fn_p_stFunctionFunctionPtr)(
    HIE_tdstSuperObject *, AI_tdstNodeInterpret *, AI_tdstGetSetParam *);
typedef AI_tdstNodeInterpret *(__cdecl *AI_tdp_fn_p_stOperatorFunctionPtr)(
    HIE_tdstSuperObject *, AI_tdstNodeInterpret *, AI_tdstGetSetParam *);

AI_tdp_fn_p_stFunctionFunctionPtr  __cdecl fn_p_fn_pGetFunctionFunctionPtr(unsigned long eFuncId);
AI_tdp_fn_p_stOperatorFunctionPtr  __cdecl fn_p_fn_pGetOperatorFunctionPtr(unsigned long eOperatorId);
#define AI_M_pfn_fn_p_fn_pGetFunctionFunctionPtr   fn_p_fn_pGetFunctionFunctionPtr
#define AI_M_pfn_fn_p_fn_pGetOperatorFunctionPtr   fn_p_fn_pGetOperatorFunctionPtr
#define AI_M_pfn_fn_p_fn_pGetProcedureFunctionPtr  fn_p_fn_pGetProcedureFunctionPtr

#define AI_M_eFuncIdInterpret(p_Tree)       ((p_Tree)->uId)
#define AI_M_eOperatorIdInterpret(p_Tree)   ((p_Tree)->uId)
#define AI_M_eProcedureIdInterpret(p_Tree)  ((p_Tree)->uId)

AI_tdstNodeInterpret *__cdecl fn_p_stEvalKeyWord(
    HIE_tdstSuperObject *p_SuperObjPerso,
    AI_tdstNodeInterpret *p_stTree,
    AI_tdstGetSetParam *p_stValue);

AI_tdstNodeInterpret *__cdecl fn_p_stEvalCondition(
    HIE_tdstSuperObject *p_SuperObjPerso,
    AI_tdstNodeInterpret *p_stTree,
    AI_tdstGetSetParam *p_stValue);

AI_tdstNodeInterpret *__cdecl fn_p_stEvalTree(
    HIE_tdstSuperObject *p_SuperObjPerso,
    AI_tdstNodeInterpret *p_stTree,
    AI_tdstGetSetParam *p_stValue);

AI_tdstNodeInterpret *__cdecl fn_p_stEvalFunction(
    HIE_tdstSuperObject *p_SuperObjPerso,
    AI_tdstNodeInterpret *p_stTree,
    AI_tdstGetSetParam *p_stValue);

AI_tdstNodeInterpret *__cdecl fn_p_stEvalOperator(
    HIE_tdstSuperObject *p_SuperObjPerso,
    AI_tdstNodeInterpret *p_stTree,
    AI_tdstGetSetParam *p_stValue);

AI_tdstNodeInterpret *__cdecl fn_p_stEvalProcedure(
    HIE_tdstSuperObject *p_SuperObjPerso,
    AI_tdstNodeInterpret *p_stTree,
    AI_tdstGetSetParam *p_stValue);

AI_tdstNodeInterpret *__cdecl fn_p_stEvalEndOfTree(
    HIE_tdstSuperObject *p_SuperObjPerso,
    AI_tdstNodeInterpret *p_stTree,
    AI_tdstGetSetParam *p_stValue);

AI_tdstNodeInterpret *__cdecl fn_p_stEvalBeginMacro(
    HIE_tdstSuperObject *p_SuperObjPerso,
    AI_tdstNodeInterpret *p_stTree,
    AI_tdstGetSetParam *p_stValue);

AI_tdstNodeInterpret *__cdecl fn_p_stEvalConstant(
    HIE_tdstSuperObject *p_SuperObjPerso, AI_tdstNodeInterpret *p_stTree, AI_tdstGetSetParam *p_stValue);
AI_tdstNodeInterpret *__cdecl fn_p_stEvalReal(
    HIE_tdstSuperObject *p_SuperObjPerso, AI_tdstNodeInterpret *p_stTree, AI_tdstGetSetParam *p_stValue);
AI_tdstNodeInterpret *__cdecl fn_p_stEvalButton(
    HIE_tdstSuperObject *p_SuperObjPerso, AI_tdstNodeInterpret *p_stTree, AI_tdstGetSetParam *p_stValue);
AI_tdstNodeInterpret *__cdecl fn_p_stEvalColor(
    HIE_tdstSuperObject *p_SuperObjPerso, AI_tdstNodeInterpret *p_stTree, AI_tdstGetSetParam *p_stValue);
AI_tdstNodeInterpret *__cdecl fn_p_stEvalPersoRef(
    HIE_tdstSuperObject *p_SuperObjPerso, AI_tdstNodeInterpret *p_stTree, AI_tdstGetSetParam *p_stValue);
AI_tdstNodeInterpret *__cdecl fn_p_stEvalActionRef(
    HIE_tdstSuperObject *p_SuperObjPerso, AI_tdstNodeInterpret *p_stTree, AI_tdstGetSetParam *p_stValue);
AI_tdstNodeInterpret *__cdecl fn_p_stEvalSuperObjectRef(
    HIE_tdstSuperObject *p_SuperObjPerso, AI_tdstNodeInterpret *p_stTree, AI_tdstGetSetParam *p_stValue);
AI_tdstNodeInterpret *__cdecl fn_p_stEvalObjectTableRef(
    HIE_tdstSuperObject *p_SuperObjPerso, AI_tdstNodeInterpret *p_stTree, AI_tdstGetSetParam *p_stValue);
AI_tdstNodeInterpret *__cdecl fn_p_stEvalModelRef(
    HIE_tdstSuperObject *p_SuperObjPerso, AI_tdstNodeInterpret *p_stTree, AI_tdstGetSetParam *p_stValue);
AI_tdstNodeInterpret *__cdecl fn_p_stEvalWayPointRef(
    HIE_tdstSuperObject *p_SuperObjPerso, AI_tdstNodeInterpret *p_stTree, AI_tdstGetSetParam *p_stValue);
AI_tdstNodeInterpret *__cdecl fn_p_stEvalModuleRef(
    HIE_tdstSuperObject *p_SuperObjPerso, AI_tdstNodeInterpret *p_stTree, AI_tdstGetSetParam *p_stValue);
AI_tdstNodeInterpret *__cdecl fn_p_stEvalString(
    HIE_tdstSuperObject *p_SuperObjPerso, AI_tdstNodeInterpret *p_stTree, AI_tdstGetSetParam *p_stValue);
AI_tdstNodeInterpret *__cdecl fn_p_stEvalFamilyRef(
    HIE_tdstSuperObject *p_SuperObjPerso, AI_tdstNodeInterpret *p_stTree, AI_tdstGetSetParam *p_stValue);
AI_tdstNodeInterpret *__cdecl fn_p_stEvalComportRef(
    HIE_tdstSuperObject *p_SuperObjPerso, AI_tdstNodeInterpret *p_stTree, AI_tdstGetSetParam *p_stValue);
AI_tdstNodeInterpret *__cdecl fn_p_stEvalTextRef(
    HIE_tdstSuperObject *p_SuperObjPerso, AI_tdstNodeInterpret *p_stTree, AI_tdstGetSetParam *p_stValue);
AI_tdstNodeInterpret *__cdecl fn_p_stEvalGameMaterialRef(
    HIE_tdstSuperObject *p_SuperObjPerso, AI_tdstNodeInterpret *p_stTree, AI_tdstGetSetParam *p_stValue);
AI_tdstNodeInterpret *__cdecl fn_p_stEvalDsgVarId(
    HIE_tdstSuperObject *p_SuperObjPerso, AI_tdstNodeInterpret *p_stTree, AI_tdstGetSetParam *p_stValue);
AI_tdstNodeInterpret *__cdecl fn_p_stEvalLipsSynchroRef(
    HIE_tdstSuperObject *p_SuperObjPerso, AI_tdstNodeInterpret *p_stTree, AI_tdstGetSetParam *p_stValue);
AI_tdstNodeInterpret *__cdecl fn_p_stEvalSoundEventRef(
    HIE_tdstSuperObject *p_SuperObjPerso, AI_tdstNodeInterpret *p_stTree, AI_tdstGetSetParam *p_stValue);
AI_tdstNodeInterpret *__cdecl fn_p_stEvalParticleGenerator(
    HIE_tdstSuperObject *p_SuperObjPerso, AI_tdstNodeInterpret *p_stTree, AI_tdstGetSetParam *p_stValue);
AI_tdstNodeInterpret *__cdecl fn_p_stEvalLight(
    HIE_tdstSuperObject *p_SuperObjPerso, AI_tdstNodeInterpret *p_stTree, AI_tdstGetSetParam *p_stValue);
AI_tdstNodeInterpret *__cdecl fn_p_stEvalCaps(
    HIE_tdstSuperObject *p_SuperObjPerso, AI_tdstNodeInterpret *p_stTree, AI_tdstGetSetParam *p_stValue);
AI_tdstNodeInterpret *__cdecl fn_p_stEvalGraph(
    HIE_tdstSuperObject *p_SuperObjPerso, AI_tdstNodeInterpret *p_stTree, AI_tdstGetSetParam *p_stValue);
AI_tdstNodeInterpret *__cdecl fn_p_stEvalMask(
    HIE_tdstSuperObject *p_SuperObjPerso, AI_tdstNodeInterpret *p_stTree, AI_tdstGetSetParam *p_stValue);
AI_tdstNodeInterpret *__cdecl fn_p_stEvalConstantVector(
    HIE_tdstSuperObject *p_SuperObjPerso, AI_tdstNodeInterpret *p_stTree, AI_tdstGetSetParam *p_stValue);

#define AI_M_eFieldIdInterpret(p_Tree)  ((p_Tree)->uId)
#define AI_C_INVALID_GET                ((unsigned char)0)
#define AI_E_uwAIFatalNotValidField     ((unsigned short)10)

#define AI_M_pfn_fn_ucGetFieldValue  fn_ucGetFieldValue   /* 0x00487CF0 */

#define AI_M_pfn_fn_vAIFatalError(uwError) \
    fn_vOnAIErm_M_UpdateLastError((unsigned short)(uwError), (char *)0)

AI_tdstNodeInterpret *__cdecl fn_p_stEvalField(
    HIE_tdstSuperObject *p_SuperObjPerso, AI_tdstNodeInterpret *p_stTree, AI_tdstGetSetParam *p_stValue);

#define AI_M_ucVarIdInterpret(p_Tree)  ((p_Tree)->uParam.lValue)

#define AI_M_pfn_fn_ucGetDsgVar fn_ucGetDsgVar

AI_tdstNodeInterpret *__cdecl fn_p_stEvalDsgVarRef(
    HIE_tdstSuperObject *p_SuperObjPerso, AI_tdstNodeInterpret *p_stTree, AI_tdstGetSetParam *p_stValue);

/* 0x00480510 */
AI_tdstNodeInterpret *__cdecl fn_p_stEvalVector(
    HIE_tdstSuperObject *p_SuperObjPerso, AI_tdstNodeInterpret *p_stTree, AI_tdstGetSetParam *p_stValue);

#define AI_M_eMetaActionIdInterpret(p_Tree)  ((p_Tree)->uId)
#define AI_M_NodeToSkip(p_Tree)              ((p_Tree)->uwNodeToSkip)
#define AI_E_uwAIActionTableTooSmall         ((unsigned short)24)

#define AI_M_pfn_fn_ucIsRuleAndNodeInActionTable fn_ucIsRuleAndNodeInActionTable  /* 0x004826E0 */
#define AI_M_pfn_fn_ucFindPlaceInActionTable     fn_ucFindPlaceInActionTable      /* 0x00482730 */
#define AI_M_pfn_fn_vUseCurrentActionTableEntry fn_vUseCurrentActionTableEntry   /* 0x00482760 */
#define AI_M_pfn_fn_p_stSwapNextParameter2       fn_p_stSwapNextParameter2        /* 0x00480980 */

AI_tdstNodeInterpret *__cdecl fn_p_stSwapNextParameter(AI_tdstNodeInterpret *p_stTree);   /* 0x00480950 */
#define AI_M_pfn_fn_p_stSwapNextParameter        fn_p_stSwapNextParameter         /* 0x00480950 */
#define AI_M_pfn_fn_ucComputeActionNextPerso     fn_ucComputeActionNextPerso      /* 0x00480A30 */
#define AI_M_pfn_fn_p_stComputeActionStartPerso  fn_p_stComputeActionStartPerso   /* 0x004809E0 */

AI_tdstNodeInterpret *__cdecl fn_p_stEvalMetaAction(
    HIE_tdstSuperObject *p_SuperObjPerso, AI_tdstNodeInterpret *p_stTree, AI_tdstGetSetParam *p_stValue);

#define AI_M_GetMacroInitTree(p_stMacro)    ((p_stMacro)->p_stInitTree)
#define AI_M_GetMacroCurrentTree(p_stMacro) ((p_stMacro)->p_stCurrentTree)
#define AI_E_ti_MacroRef                    ((unsigned char)39)

AI_tdstNodeInterpret *__cdecl fn_p_stEvalMacro(
    HIE_tdstSuperObject *p_SuperObjPerso, AI_tdstNodeInterpret *p_stTree, AI_tdstGetSetParam *p_stValue);

AI_tdstNodeInterpret *__cdecl fn_p_stBooleanCondition(
    HIE_tdstSuperObject *p_SuperObjPerso, AI_tdstNodeInterpret *p_stTree, long *p_lValue);
AI_tdstNodeInterpret *__cdecl fn_p_stComparisonCondition(
    HIE_tdstSuperObject *p_SuperObjPerso, AI_tdstNodeInterpret *p_stTree, long *p_lValue);
AI_tdstNodeInterpret *__cdecl fn_p_stButtonCondition(
    HIE_tdstSuperObject *p_SuperObjPerso, AI_tdstNodeInterpret *p_stTree, long *p_lValue);
AI_tdstNodeInterpret *__cdecl fn_p_stTimeCondition(
    HIE_tdstSuperObject *p_SuperObjPerso, AI_tdstNodeInterpret *p_stTree, long *p_lValue);
AI_tdstNodeInterpret *__cdecl fn_p_stValidityCondition(
    HIE_tdstSuperObject *p_SuperObjPerso, AI_tdstNodeInterpret *p_stTree, long *p_lValue);
AI_tdstNodeInterpret *__cdecl fn_p_st_HandledCondition(
    HIE_tdstSuperObject *p_SuperObjPerso, AI_tdstNodeInterpret *p_stTree, long *p_lValue);
AI_tdstNodeInterpret *__cdecl fn_p_st_ActivationCondition(
    HIE_tdstSuperObject *p_SuperObjPerso, AI_tdstNodeInterpret *p_stTree, long *p_lValue);
AI_tdstNodeInterpret *__cdecl AI_fn_p_st_pc_ReportSurfaceMaterialCondition(
    HIE_tdstSuperObject *p_SuperObjPerso, AI_tdstNodeInterpret *p_stTree, long *p_lValue);
AI_tdstNodeInterpret *__cdecl fn_p_stReportOnZDMSurfaceCondition(
    HIE_tdstSuperObject *p_SuperObjPerso, AI_tdstNodeInterpret *p_stTree, long *p_lValue);
AI_tdstNodeInterpret *__cdecl fn_p_stReportOnSurfaceCondition(
    HIE_tdstSuperObject *p_SuperObjPerso, AI_tdstNodeInterpret *p_stTree, long *p_lValue);
AI_tdstNodeInterpret *__cdecl fn_p_stZDDCondition(
    HIE_tdstSuperObject *p_SuperObjPerso, AI_tdstNodeInterpret *p_stTree, long *p_lValue);
AI_tdstNodeInterpret *__cdecl fn_p_stZDECondition(
    HIE_tdstSuperObject *p_SuperObjPerso, AI_tdstNodeInterpret *p_stTree, long *p_lValue);
AI_tdstNodeInterpret *__cdecl fn_p_stMiscCondition(
    HIE_tdstSuperObject *p_SuperObjPerso, AI_tdstNodeInterpret *p_stTree, long *p_lValue);
AI_tdstNodeInterpret *__cdecl fn_p_stIsPersoActive(
    HIE_tdstSuperObject *p_SuperObjPerso, AI_tdstNodeInterpret *p_stTree, long *p_lValue);
AI_tdstNodeInterpret *__cdecl fn_p_stCheckAnimEnd(
    HIE_tdstSuperObject *p_SuperObjPerso, AI_tdstNodeInterpret *p_stTree, long *p_lValue);
AI_tdstNodeInterpret *__cdecl fn_p_stListCondition(
    HIE_tdstSuperObject *p_SuperObjPerso, AI_tdstNodeInterpret *p_stTree, long *p_lValue);
AI_tdstNodeInterpret *__cdecl fn_p_stLightCondition(
    HIE_tdstSuperObject *p_SuperObjPerso, AI_tdstNodeInterpret *p_stTree, long *p_lValue);
AI_tdstNodeInterpret *__cdecl fn_p_stSectorCondition(
    HIE_tdstSuperObject *p_SuperObjPerso, AI_tdstNodeInterpret *p_stTree, long *p_lValue);
AI_tdstNodeInterpret *__cdecl fn_p_stAlw_IsMine(
    HIE_tdstSuperObject *p_SuperObjPerso, AI_tdstNodeInterpret *p_stTree, long *p_lValue);
AI_tdstNodeInterpret *__cdecl fn_p_stHierarchySon(
    HIE_tdstSuperObject *p_SuperObjPerso, AI_tdstNodeInterpret *p_stTree, long *p_lValue);
AI_tdstNodeInterpret *__cdecl fn_p_stIsMorphing(
    HIE_tdstSuperObject *p_SuperObjPerso, AI_tdstNodeInterpret *p_stTree, long *p_lValue);
AI_tdstNodeInterpret *__cdecl fn_p_stNullVector(
    HIE_tdstSuperObject *p_SuperObjPerso, AI_tdstNodeInterpret *p_stTree, long *p_lValue);
AI_tdstNodeInterpret *__cdecl fn_p_stMechanicOption(
    HIE_tdstSuperObject *p_SuperObjPerso, AI_tdstNodeInterpret *p_stTree, long *p_lValue);
AI_tdstNodeInterpret *__cdecl fn_p_stCheckCapabilities(
    HIE_tdstSuperObject *p_SuperObjPerso, AI_tdstNodeInterpret *p_stTree, long *p_lValue);
AI_tdstNodeInterpret *__cdecl fn_p_stCustomBitsCond(
    HIE_tdstSuperObject *p_SuperObjPerso, AI_tdstNodeInterpret *p_stTree, long *p_lValue);
AI_tdstNodeInterpret *__cdecl fn_p_stCollisionWP(
    HIE_tdstSuperObject *p_SuperObjPerso, AI_tdstNodeInterpret *p_stTree, long *p_lValue);
AI_tdstNodeInterpret *__cdecl fn_p_stReportThisPerso(
    HIE_tdstSuperObject *p_SuperObjPerso, AI_tdstNodeInterpret *p_stTree, long *p_lValue);
AI_tdstNodeInterpret *__cdecl fn_p_st_UserEvent_IsSet(
    HIE_tdstSuperObject *p_SuperObjPerso, AI_tdstNodeInterpret *p_stTree, long *p_lValue);
AI_tdstNodeInterpret *__cdecl fn_p_stMagnetActivated(
    HIE_tdstSuperObject *p_SuperObjPerso, AI_tdstNodeInterpret *p_stTree, long *p_lValue);

/* 0x0048B980 */
AI_tdstNodeInterpret *__cdecl AI_fn_p_stIsNotInCollisionWith(
    HIE_tdstSuperObject *p_SuperObjPerso, AI_tdstNodeInterpret *p_stTree, long *p_lValue);

/* 0x0048BA50 */
AI_tdstNodeInterpret *__cdecl AI_fn_p_stIsOfFamilyOrModel(
    HIE_tdstSuperObject *p_SuperObjPerso, AI_tdstNodeInterpret *p_stTree, long *p_lValue);

/* 0x48bb10 */
AI_tdstNodeInterpret *__cdecl AI_fn_p_stIsJoypadConnected(
    HIE_tdstSuperObject *p_SuperObjPerso, AI_tdstNodeInterpret *p_stTree, long *p_lValue);
AI_tdstNodeInterpret *__cdecl AI_fn_p_stIsKeyJustPressed(
    HIE_tdstSuperObject *p_SuperObjPerso, AI_tdstNodeInterpret *p_stTree, long *p_lValue);
AI_tdstNodeInterpret *__cdecl AI_fn_p_stIsButtonPadJustPressed(
    HIE_tdstSuperObject *p_SuperObjPerso, AI_tdstNodeInterpret *p_stTree, long *p_lValue);
AI_tdstNodeInterpret *__cdecl AI_fn_p_stIsInDemoMode(
    HIE_tdstSuperObject *p_SuperObjPerso, AI_tdstNodeInterpret *p_stTree, long *p_lValue);
AI_tdstNodeInterpret *__cdecl fn_p_stIsInStereoMode(
    HIE_tdstSuperObject *p_SuperObjPerso, AI_tdstNodeInterpret *p_stTree, long *p_lValue);
AI_tdstNodeInterpret *__cdecl fn_p_stIsMusicPlaying(
    HIE_tdstSuperObject *p_SuperObjPerso, AI_tdstNodeInterpret *p_stTree, long *p_lValue);
AI_tdstNodeInterpret *__cdecl AI_fn_p_stIsSharpnessMax(
    HIE_tdstSuperObject *p_SuperObjPerso, AI_tdstNodeInterpret *p_stTree, long *p_lValue);

/* 0x004894D0 */
AI_tdstNodeInterpret *__cdecl AI_fn_p_stCameraCondition(
    HIE_tdstSuperObject *p_SuperObjPerso, AI_tdstNodeInterpret *p_stTree, long *p_lValue);

AI_tdstNodeInterpret *__cdecl fn_p_stNoActionKeyWord(
    HIE_tdstSuperObject *p_SuperObjPerso, AI_tdstNodeInterpret *p_stTree, AI_tdstGetSetParam *p_stValue);
AI_tdstNodeInterpret *__cdecl fn_p_stMeKeyWord(
    HIE_tdstSuperObject *p_SuperObjPerso, AI_tdstNodeInterpret *p_stTree, AI_tdstGetSetParam *p_stValue);
AI_tdstNodeInterpret *__cdecl fn_p_stMainActorKeyWord(
    HIE_tdstSuperObject *p_SuperObjPerso, AI_tdstNodeInterpret *p_stTree, AI_tdstGetSetParam *p_stValue);
AI_tdstNodeInterpret *__cdecl fn_p_stNoSuperObjectKeyWord(
    HIE_tdstSuperObject *p_SuperObjPerso, AI_tdstNodeInterpret *p_stTree, AI_tdstGetSetParam *p_stValue);
AI_tdstNodeInterpret *__cdecl fn_p_stNobodyKeyWord(
    HIE_tdstSuperObject *p_SuperObjPerso, AI_tdstNodeInterpret *p_stTree, AI_tdstGetSetParam *p_stValue);
AI_tdstNodeInterpret *__cdecl fn_p_stNowhereKeyWord(
    HIE_tdstSuperObject *p_SuperObjPerso, AI_tdstNodeInterpret *p_stTree, AI_tdstGetSetParam *p_stValue);
AI_tdstNodeInterpret *__cdecl fn_p_stEmptyTextKeyWord(
    HIE_tdstSuperObject *p_SuperObjPerso, AI_tdstNodeInterpret *p_stTree, AI_tdstGetSetParam *p_stValue);
AI_tdstNodeInterpret *__cdecl fn_p_stCapsNullKeyWord(
    HIE_tdstSuperObject *p_SuperObjPerso, AI_tdstNodeInterpret *p_stTree, AI_tdstGetSetParam *p_stValue);
AI_tdstNodeInterpret *__cdecl fn_p_stNoGraphKeyWord(
    HIE_tdstSuperObject *p_SuperObjPerso, AI_tdstNodeInterpret *p_stTree, AI_tdstGetSetParam *p_stValue);
AI_tdstNodeInterpret *__cdecl fn_p_stSkipThenElseParameter(
    AI_tdstNodeInterpret *p_stTree, AI_tdstNodeInterpret *p_stTreeInit);
AI_tdstNodeInterpret *__cdecl fn_p_stThenKeyWord(
    HIE_tdstSuperObject *p_SuperObjPerso, AI_tdstNodeInterpret *p_stTree, AI_tdstGetSetParam *p_stValue);
AI_tdstNodeInterpret *__cdecl fn_p_stElseKeyWord(
    HIE_tdstSuperObject *p_SuperObjPerso, AI_tdstNodeInterpret *p_stTree, AI_tdstGetSetParam *p_stValue);
AI_tdstNodeInterpret *__cdecl fn_p_stIfDebug(
    HIE_tdstSuperObject *p_SuperObjPerso, AI_tdstNodeInterpret *p_stTree, AI_tdstGetSetParam *p_stValue);
AI_tdstNodeInterpret *__cdecl fn_p_stIfNotU64(
    HIE_tdstSuperObject *p_SuperObjPerso, AI_tdstNodeInterpret *p_stTree, AI_tdstGetSetParam *p_stValue);
AI_tdstNodeInterpret *__cdecl fn_p_stIfKeyWord(
    HIE_tdstSuperObject *p_SuperObjPerso, AI_tdstNodeInterpret *p_stTree, AI_tdstGetSetParam *p_stValue);
AI_tdstNodeInterpret *__cdecl fn_p_stEngineGotoKeyWord(
    HIE_tdstSuperObject *p_SuperObjPerso, AI_tdstNodeInterpret *p_stTree, AI_tdstGetSetParam *p_stValue);

/* 0x00481DA0 */
AI_tdstNodeInterpret *__cdecl fn_p_stVectorDotOperator(
    HIE_tdstSuperObject *p_SuperObjPerso, AI_tdstNodeInterpret *p_stTree, long *p_lValue);
/* 0x00481A60 */
AI_tdstNodeInterpret *__cdecl fn_p_stVectorOperator(
    HIE_tdstSuperObject *p_SuperObjPerso, AI_tdstNodeInterpret *p_stTree, AI_tdstGetSetParam *p_stValue);
/* 0x00481880 */
AI_tdstNodeInterpret *__cdecl fn_p_stScalarOperator(
    HIE_tdstSuperObject *p_SuperObjPerso, AI_tdstNodeInterpret *p_stTree, AI_tdstGetSetParam *p_stValue);
/* 0x00481CE0 */
AI_tdstNodeInterpret *__cdecl fn_p_stDotOperator(
    HIE_tdstSuperObject *p_SuperObjPerso, AI_tdstNodeInterpret *p_stTree, long *p_lValue);
/* 0x004825E0 */
AI_tdstNodeInterpret *__cdecl fn_p_stUltraOperator(
    HIE_tdstSuperObject *p_SuperObjPerso, AI_tdstNodeInterpret *p_stTree, long *p_lValue);
/* 0x00482620 */
AI_tdstNodeInterpret *__cdecl fn_p_stModelCastOperator(
    HIE_tdstSuperObject *p_SuperObjPerso, AI_tdstNodeInterpret *p_stTree, long *p_lValue);
/* 0x00481C50 */
AI_tdstNodeInterpret *__cdecl fn_p_stArrayOperator(
    HIE_tdstSuperObject *p_SuperObjPerso, AI_tdstNodeInterpret *p_stTree, long *p_lValue);
/* 0x00481DF0 */
AI_tdstNodeInterpret *__cdecl fn_p_stAffectOperator(
    HIE_tdstSuperObject *p_SuperObjPerso, AI_tdstNodeInterpret *p_stTree, AI_tdstGetSetParam *p_stValue);

/* 0x0047B7D0 */
AI_tdstNodeInterpret *__cdecl fn_p_stGetPersoAbsolutePosition(
    HIE_tdstSuperObject *p_SuperObjPerso, AI_tdstNodeInterpret *p_stTree, AI_tdstGetSetParam *p_stValue);
/* 0x0047B830 */
AI_tdstNodeInterpret *__cdecl fn_p_stGetMyAbsolutePosition(
    HIE_tdstSuperObject *p_SuperObjPerso, AI_tdstNodeInterpret *p_stTree, AI_tdstGetSetParam *p_stValue);
/* 0x0047B6C0 */
AI_tdstNodeInterpret *__cdecl fn_p_stDistanceToPerso(
    HIE_tdstSuperObject *p_SuperObjPerso, AI_tdstNodeInterpret *p_stTree, AI_tdstGetSetParam *p_stValue);
/* 0x0047B8D0 */
AI_tdstNodeInterpret *__cdecl fn_p_stDistanceFunction(
    HIE_tdstSuperObject *p_SuperObjPerso, AI_tdstNodeInterpret *p_stTree, AI_tdstGetSetParam *p_stValue);
/* 0x0047BB70 */
AI_tdstNodeInterpret *__cdecl fn_p_stMathTernarReal(
    HIE_tdstSuperObject *p_SuperObjPerso, AI_tdstNodeInterpret *p_stTree, AI_tdstGetSetParam *p_stValue);
/* 0x0047B480 */
AI_tdstNodeInterpret *__cdecl fn_p_stDistanceToPersoCenter(
    HIE_tdstSuperObject *p_SuperObjPerso, AI_tdstNodeInterpret *p_stTree, AI_tdstGetSetParam *p_stValue);
/* 0x0047C1A0 */
AI_tdstNodeInterpret *__cdecl fn_p_stLocalToGlobal(
    HIE_tdstSuperObject *p_SuperObjPerso, AI_tdstNodeInterpret *p_stTree, AI_tdstGetSetParam *p_stValue);
AI_tdstNodeInterpret *__cdecl fn_p_stGlobalToLocal(
    HIE_tdstSuperObject *p_SuperObjPerso, AI_tdstNodeInterpret *p_stTree, AI_tdstGetSetParam *p_stValue);
/* 0x0047C940 */
AI_tdstNodeInterpret *__cdecl fn_p_stHitPointsAndHitPointsMaxFunction(
    HIE_tdstSuperObject *p_SuperObjPerso, AI_tdstNodeInterpret *p_stTree, AI_tdstGetSetParam *p_stValue);
/* 0x0047CB40 */
AI_tdstNodeInterpret *__cdecl fn_p_stListFunction(
    HIE_tdstSuperObject *p_SuperObjPerso, AI_tdstNodeInterpret *p_stTree, AI_tdstGetSetParam *p_stValue);
/* 0x0047CAF0 */
AI_tdstNodeInterpret *__cdecl fn_p_stTimeFunction(
    HIE_tdstSuperObject *p_SuperObjPerso, AI_tdstNodeInterpret *p_stTree, AI_tdstGetSetParam *p_stValue);
/* 0x0047CBF0 */
AI_tdstNodeInterpret *__cdecl fn_p_stInputFunction(
    HIE_tdstSuperObject *p_SuperObjPerso, AI_tdstNodeInterpret *p_stTree, AI_tdstGetSetParam *p_stValue);
/* 0x0047B860 */
AI_tdstNodeInterpret *__cdecl fn_p_stGetWpAbsolutePosition(
    HIE_tdstSuperObject *p_SuperObjPerso, AI_tdstNodeInterpret *p_stTree, AI_tdstGetSetParam *p_stValue);
/* 0x0047BD70 */
AI_tdstNodeInterpret *__cdecl fn_p_stMathAbsoluteValue(
    HIE_tdstSuperObject *p_SuperObjPerso, AI_tdstNodeInterpret *p_stTree, AI_tdstGetSetParam *p_stValue);
/* 0x0047C810 */
AI_tdstNodeInterpret *__cdecl fn_p_stRealFunction(
    HIE_tdstSuperObject *p_SuperObjPerso, AI_tdstNodeInterpret *p_stTree, AI_tdstGetSetParam *p_stValue);
/* 0x004883B0 */
AI_tdstNodeInterpret *__cdecl MGT_fn_p_stGetInfo(
    HIE_tdstSuperObject *p_SuperObjPerso, AI_tdstNodeInterpret *p_stTree, AI_tdstGetSetParam *p_stValue);
/* 0x0047D710 */
AI_tdstNodeInterpret *__cdecl fn_p_stGetFrameLength(
    HIE_tdstSuperObject *p_SuperObjPerso, AI_tdstNodeInterpret *p_stTree, AI_tdstGetSetParam *p_stValue);
/* 0x00478580 */
AI_tdstNodeInterpret *__cdecl fn_p_stGetDTFunction(
    HIE_tdstSuperObject *p_SuperObjPerso, AI_tdstNodeInterpret *p_stTree, AI_tdstGetSetParam *p_stValue);
/* 0x00474A00 */
AI_tdstNodeInterpret *__cdecl fn_p_stCode4VitessePadAnalogique(
    HIE_tdstSuperObject *p_SuperObjPerso, AI_tdstNodeInterpret *p_stTree, AI_tdstGetSetParam *p_stValue);
/* 0x0047CC30 */
AI_tdstNodeInterpret *__cdecl fn_p_stMiscFunction(
    HIE_tdstSuperObject *p_SuperObjPerso, AI_tdstNodeInterpret *p_stTree, AI_tdstGetSetParam *p_stValue);
/* 0x0047D830 */
AI_tdstNodeInterpret *__cdecl fn_p_stSaveGameFunction(
    HIE_tdstSuperObject *p_SuperObjPerso, AI_tdstNodeInterpret *p_stTree, AI_tdstGetSetParam *p_stValue);
/* 0x0047D730 */
AI_tdstNodeInterpret *__cdecl fn_p_stIsAValidSlotName(
    HIE_tdstSuperObject *p_SuperObjPerso, AI_tdstNodeInterpret *p_stTree, AI_tdstGetSetParam *p_stValue);
/* 0x00476130 */
AI_tdstNodeInterpret *__cdecl fn_p_stCode4PointsDeMagie(
    HIE_tdstSuperObject *p_SuperObjPerso, AI_tdstNodeInterpret *p_stTree, AI_tdstGetSetParam *p_stValue);
/* 0x00475F90 */
AI_tdstNodeInterpret *__cdecl fn_p_stCode4PointsDair(
    HIE_tdstSuperObject *p_SuperObjPerso, AI_tdstNodeInterpret *p_stTree, AI_tdstGetSetParam *p_stValue);
/* 0x00478450 */
AI_tdstNodeInterpret *__cdecl fn_p_stGetNearestActorInCurrentSector(
    HIE_tdstSuperObject *p_SuperObjPerso, AI_tdstNodeInterpret *p_stTree, AI_tdstGetSetParam *p_stValue);
/* 0x00478220 */
AI_tdstNodeInterpret *__cdecl fn_p_stGetNbActivePerso(
    HIE_tdstSuperObject *p_SuperObjPerso, AI_tdstNodeInterpret *p_stTree, AI_tdstGetSetParam *p_stValue);
/* 0x00477D20 */
AI_tdstNodeInterpret *__cdecl fn_p_stGetSituationOnRail(
    HIE_tdstSuperObject *p_SuperObjPerso, AI_tdstNodeInterpret *p_stTree, AI_tdstGetSetParam *p_stValue);
/* 0x00477E90 */
AI_tdstNodeInterpret *__cdecl fn_p_stPersoCapabilityFunction(
    HIE_tdstSuperObject *p_SuperObjPerso, AI_tdstNodeInterpret *p_stTree, AI_tdstGetSetParam *p_stValue);
/* 0x00477EE0 */
AI_tdstNodeInterpret *__cdecl fn_p_stCapabilityFunction(
    HIE_tdstSuperObject *p_SuperObjPerso, AI_tdstNodeInterpret *p_stTree, AI_tdstGetSetParam *p_stValue);
/* 0x00477F90 */
AI_tdstNodeInterpret *__cdecl fn_p_stGetNbFrame(
    HIE_tdstSuperObject *p_SuperObjPerso, AI_tdstNodeInterpret *p_stTree, AI_tdstGetSetParam *p_stValue);
/* 0x004784F0 */
AI_tdstNodeInterpret *__cdecl fn_p_stDotProduct(
    HIE_tdstSuperObject *p_SuperObjPerso, AI_tdstNodeInterpret *p_stTree, AI_tdstGetSetParam *p_stValue);
/* 0x00478000 */
AI_tdstNodeInterpret *__cdecl fn_p_stVectorOperations(
    HIE_tdstSuperObject *p_SuperObjPerso, AI_tdstNodeInterpret *p_stTree, AI_tdstGetSetParam *p_stValue);
/* 0x00478180 */
AI_tdstNodeInterpret *__cdecl fn_p_stGetSPOCoordinates(
    HIE_tdstSuperObject *p_SuperObjPerso, AI_tdstNodeInterpret *p_stTree, AI_tdstGetSetParam *p_stValue);
/* 0x00478260 */
AI_tdstNodeInterpret *__cdecl fn_p_stTractionFactorFunction(
    HIE_tdstSuperObject *p_SuperObjPerso, AI_tdstNodeInterpret *p_stTree, AI_tdstGetSetParam *p_stValue);
/* 0x00478560 */
AI_tdstNodeInterpret *__cdecl fn_p_stGetCPUCounter(
    HIE_tdstSuperObject *p_SuperObjPerso, AI_tdstNodeInterpret *p_stTree, AI_tdstGetSetParam *p_stValue);
/* 0x00476F20 */
AI_tdstNodeInterpret *__cdecl fn_p_stGraphFunction(
    HIE_tdstSuperObject *p_SuperObjPerso, AI_tdstNodeInterpret *p_stTree, AI_tdstGetSetParam *p_stValue);
/* 0x004770F0 */
AI_tdstNodeInterpret *__cdecl fn_p_stGraphToWayFunction(
    HIE_tdstSuperObject *p_SuperObjPerso, AI_tdstNodeInterpret *p_stTree, AI_tdstGetSetParam *p_stValue);
/* 0x00477300 */
AI_tdstNodeInterpret *__cdecl fn_p_stWayFunction(
    HIE_tdstSuperObject *p_SuperObjPerso, AI_tdstNodeInterpret *p_stTree, AI_tdstGetSetParam *p_stValue);
/* 0x00477630 */
AI_tdstNodeInterpret *__cdecl fn_p_stCloserWPInGraph(
    HIE_tdstSuperObject *p_SuperObjPerso, AI_tdstNodeInterpret *p_stTree, AI_tdstGetSetParam *p_stValue);
/* 0x00477A80 */
AI_tdstNodeInterpret *__cdecl fn_p_stTravelOnAGraph(
    HIE_tdstSuperObject *p_SuperObjPerso, AI_tdstNodeInterpret *p_stTree, AI_tdstGetSetParam *p_stValue);
/* 0x00477CC0 */
AI_tdstNodeInterpret *__cdecl fn_p_stMiscFuncOnTypeOfWP(
    HIE_tdstSuperObject *p_SuperObjPerso, AI_tdstNodeInterpret *p_stTree, AI_tdstGetSetParam *p_stValue);
/* 0x00477F10 */
AI_tdstNodeInterpret *__cdecl fn_p_stGetScrollSpeed(
    HIE_tdstSuperObject *p_SuperObjPerso, AI_tdstNodeInterpret *p_stTree, AI_tdstGetSetParam *p_stValue);
/* 0x004782C0 */
AI_tdstNodeInterpret *__cdecl fn_p_stGetZDxCenter(
    HIE_tdstSuperObject *p_SuperObjPerso, AI_tdstNodeInterpret *p_stTree, AI_tdstGetSetParam *p_stValue);
/* 0x00476960 */
AI_tdstNodeInterpret *__cdecl fn_p_stCode4PersoLePlusProche(
    HIE_tdstSuperObject *p_SuperObjPerso, AI_tdstNodeInterpret *p_stTree, AI_tdstGetSetParam *p_stValue);
/* 0x00475680 */
AI_tdstNodeInterpret *__cdecl fn_p_stCode4TextAffiche(
    HIE_tdstSuperObject *p_SuperObjPerso, AI_tdstNodeInterpret *p_stTree, AI_tdstGetSetParam *p_stValue);
/* 0x00479910 */
AI_tdstNodeInterpret *__cdecl fn_p_stGetCollisionWithCollideMaterial(
    HIE_tdstSuperObject *p_SuperObjPerso, AI_tdstNodeInterpret *p_stTree, AI_tdstGetSetParam *p_stValue);
/* 0x004785B0 */
AI_tdstNodeInterpret *__cdecl fn_p_stGetNormalCollideVector(
    HIE_tdstSuperObject *p_SuperObjPerso, AI_tdstNodeInterpret *p_stTree, AI_tdstGetSetParam *p_stValue);
/* 0x0047A150 */
AI_tdstNodeInterpret *__cdecl fn_p_GetMechanicParameter(
    HIE_tdstSuperObject *p_SuperObjPerso, AI_tdstNodeInterpret *p_stTree, AI_tdstGetSetParam *p_stValue);
/* 0x00479260 */
AI_tdstNodeInterpret *__cdecl fn_p_stGetCollidePoint(
    HIE_tdstSuperObject *p_SuperObjPerso, AI_tdstNodeInterpret *p_stTree, AI_tdstGetSetParam *p_stValue);
/* 0x00475610 */
AI_tdstNodeInterpret *__cdecl fn_p_stCode4VitesseHorizontaleDuPerso(
    HIE_tdstSuperObject *p_SuperObjPerso, AI_tdstNodeInterpret *p_stTree, AI_tdstGetSetParam *p_stValue);
/* 0x0047A3E0 */
AI_tdstNodeInterpret *__cdecl fn_p_GetHierarchyLink(
    HIE_tdstSuperObject *p_SuperObjPerso, AI_tdstNodeInterpret *p_stTree, AI_tdstGetSetParam *p_stValue);
/* 0x0047A660 */
AI_tdstNodeInterpret *__cdecl fn_p_ComputeJumpImpulsion(
    HIE_tdstSuperObject *p_SuperObjPerso, AI_tdstNodeInterpret *p_stTree, AI_tdstGetSetParam *p_stValue);
/* 0x0047A8E0 */
AI_tdstNodeInterpret *__cdecl fn_p_GetBooleanInArray(
    HIE_tdstSuperObject *p_SuperObjPerso, AI_tdstNodeInterpret *p_stTree, AI_tdstGetSetParam *p_stValue);
/* 0x00478A20 */
AI_tdstNodeInterpret *__cdecl fn_p_stGetPersoSighting(
    HIE_tdstSuperObject *p_SuperObjPerso, AI_tdstNodeInterpret *p_stTree, AI_tdstGetSetParam *p_stValue);
/* 0x00478AA0 */
AI_tdstNodeInterpret *__cdecl fn_p_st3DFunc(
    HIE_tdstSuperObject *p_SuperObjPerso, AI_tdstNodeInterpret *p_stTree, AI_tdstGetSetParam *p_stValue);
/* 0x0047AA70 */
AI_tdstNodeInterpret *__cdecl fn_p_stGetButtonName(
    HIE_tdstSuperObject *p_SuperObjPerso, AI_tdstNodeInterpret *p_stTree, AI_tdstGetSetParam *p_stValue);
/* 0x00488590 */
AI_tdstNodeInterpret *__cdecl CAM_fn_p_stGetVectorParameter(
    HIE_tdstSuperObject *p_SuperObjPerso, AI_tdstNodeInterpret *p_stTree, AI_tdstGetSetParam *p_stValue);
/* 0x004886A0 */
AI_tdstNodeInterpret *__cdecl CAM_fn_p_stGetRealParameter(
    HIE_tdstSuperObject *p_SuperObjPerso, AI_tdstNodeInterpret *p_stTree, AI_tdstGetSetParam *p_stValue);
/* 0x004889C0 */
AI_tdstNodeInterpret *__cdecl CAM_fn_p_stGetTargetedSuperObject(
    HIE_tdstSuperObject *p_SuperObjPerso, AI_tdstNodeInterpret *p_stTree, AI_tdstGetSetParam *p_stValue);
/* 0x00488A80 */
AI_tdstNodeInterpret *__cdecl CAM_fn_p_stGetTypeOfViewport(
    HIE_tdstSuperObject *p_SuperObjPerso, AI_tdstNodeInterpret *p_stTree, AI_tdstGetSetParam *p_stValue);
/* 0x00488BB0 */
AI_tdstNodeInterpret *__cdecl CAM_fn_p_stGetCameraOfViewport(
    HIE_tdstSuperObject *p_SuperObjPerso, AI_tdstNodeInterpret *p_stTree, AI_tdstGetSetParam *p_stValue);
/* 0x00488B50 */
AI_tdstNodeInterpret *__cdecl CAM_fn_p_stGetSectorCameraType(
    HIE_tdstSuperObject *p_SuperObjPerso, AI_tdstNodeInterpret *p_stTree, AI_tdstGetSetParam *p_stValue);
/* 0x00488DC0 */
AI_tdstNodeInterpret *__cdecl CAM_fn_p_stComputeCurrentTarget(
    HIE_tdstSuperObject *p_SuperObjPerso, AI_tdstNodeInterpret *p_stTree, AI_tdstGetSetParam *p_stValue);
/* 0x00488CB0 */
AI_tdstNodeInterpret *__cdecl CAM_fn_p_stComputeTargetWithTgtPerso(
    HIE_tdstSuperObject *p_SuperObjPerso, AI_tdstNodeInterpret *p_stTree, AI_tdstGetSetParam *p_stValue);
/* 0x00488E20 */
AI_tdstNodeInterpret *__cdecl CAM_fn_p_stGetBestPos(
    HIE_tdstSuperObject *p_SuperObjPerso, AI_tdstNodeInterpret *p_stTree, AI_tdstGetSetParam *p_stValue);
/* 0x0047AC30 */
AI_tdstNodeInterpret *__cdecl fn_p_stGetNbAvailableResolution(
    HIE_tdstSuperObject *p_SuperObjPerso, AI_tdstNodeInterpret *p_stTree, AI_tdstGetSetParam *p_stValue);
/* 0x0047AC50 */
AI_tdstNodeInterpret *__cdecl fn_p_stGetCurrentResolution(
    HIE_tdstSuperObject *p_SuperObjPerso, AI_tdstNodeInterpret *p_stTree, AI_tdstGetSetParam *p_stValue);
/* 0x0047ACD0 */
AI_tdstNodeInterpret *__cdecl fn_p_stGetNameResolution(
    HIE_tdstSuperObject *p_SuperObjPerso, AI_tdstNodeInterpret *p_stTree, AI_tdstGetSetParam *p_stValue);
/* 0x0047AD30 */
AI_tdstNodeInterpret *__cdecl fn_p_stGetNbSlotsAvailable(
    HIE_tdstSuperObject *p_SuperObjPerso, AI_tdstNodeInterpret *p_stTree, AI_tdstGetSetParam *p_stValue);
/* 0x0047AD60 */
AI_tdstNodeInterpret *__cdecl fn_p_stGetVideoOptions(
    HIE_tdstSuperObject *p_SuperObjPerso, AI_tdstNodeInterpret *p_stTree, AI_tdstGetSetParam *p_stValue);
/* 0x0047AD10 */
AI_tdstNodeInterpret *__cdecl fn_p_stGetBrightness(
    HIE_tdstSuperObject *p_SuperObjPerso, AI_tdstNodeInterpret *p_stTree, AI_tdstGetSetParam *p_stValue);
/* 0x0047AC70 */
AI_tdstNodeInterpret *__cdecl AI_pc_fn_p_stSaveVideoMode(
    HIE_tdstSuperObject *p_SuperObjPerso, AI_tdstNodeInterpret *p_stTree, AI_tdstGetSetParam *p_stValue);
/* 0x0047AC90 */
AI_tdstNodeInterpret *__cdecl AI_pc_fn_p_stIsResolutionAvailable(
    HIE_tdstSuperObject *p_SuperObjPerso, AI_tdstNodeInterpret *p_stTree, AI_tdstGetSetParam *p_stValue);
/* 0x00479680 */
AI_tdstNodeInterpret *__cdecl fn_p_stGetLastCollisionActor(
    HIE_tdstSuperObject *p_SuperObjPerso, AI_tdstNodeInterpret *p_stTree, AI_tdstGetSetParam *p_stValue);
/* 0x00479BE0 */
AI_tdstNodeInterpret *__cdecl fn_p_stVectorAndAngle(
    HIE_tdstSuperObject *p_SuperObjPerso, AI_tdstNodeInterpret *p_stTree, AI_tdstGetSetParam *p_stValue);
/* 0x00474B60 */
AI_tdstNodeInterpret *__cdecl fn_p_stCode4LitCentreZDx(
    HIE_tdstSuperObject *p_SuperObjPerso, AI_tdstNodeInterpret *p_stTree, AI_tdstGetSetParam *p_stValue);
/* 0x004754D0 */
AI_tdstNodeInterpret *__cdecl fn_p_stCode4LitPositionZDx(
    HIE_tdstSuperObject *p_SuperObjPerso, AI_tdstNodeInterpret *p_stTree, AI_tdstGetSetParam *p_stValue);
/* 0x00474D40 */
AI_tdstNodeInterpret *__cdecl fn_p_stCode4LitDimensionZDx(
    HIE_tdstSuperObject *p_SuperObjPerso, AI_tdstNodeInterpret *p_stTree, AI_tdstGetSetParam *p_stValue);
/* 0x00474EF0 */
AI_tdstNodeInterpret *__cdecl fn_p_stCode4LitAxeZDx(
    HIE_tdstSuperObject *p_SuperObjPerso, AI_tdstNodeInterpret *p_stTree, AI_tdstGetSetParam *p_stValue);
/* 0x00479AC0 */
AI_tdstNodeInterpret *__cdecl fn_p_stComputeRebondVector(
    HIE_tdstSuperObject *p_SuperObjPerso, AI_tdstNodeInterpret *p_stTree, AI_tdstGetSetParam *p_stValue);
/* 0x004750F0 */
AI_tdstNodeInterpret *__cdecl fn_p_stCode4VecteurPointAxe(
    HIE_tdstSuperObject *p_SuperObjPerso, AI_tdstNodeInterpret *p_stTree, AI_tdstGetSetParam *p_stValue);
/* 0x00479720 */
AI_tdstNodeInterpret *__cdecl fn_p_stGetCollision(
    HIE_tdstSuperObject *p_SuperObjPerso, AI_tdstNodeInterpret *p_stTree, AI_tdstGetSetParam *p_stValue);
/* 0x004788E0 */
AI_tdstNodeInterpret *__cdecl fn_p_stCollideCommunicationFunc(
    HIE_tdstSuperObject *p_SuperObjPerso, AI_tdstNodeInterpret *p_stTree, AI_tdstGetSetParam *p_stValue);
/* 0x0047A490 */
AI_tdstNodeInterpret *__cdecl fn_p_GetPriviligedActivationZdx(
    HIE_tdstSuperObject *p_SuperObjPerso, AI_tdstNodeInterpret *p_stTree, AI_tdstGetSetParam *p_stValue);
/* 0x0047A590 */
AI_tdstNodeInterpret *__cdecl fn_p_GetComputationFrequency(
    HIE_tdstSuperObject *p_SuperObjPerso, AI_tdstNodeInterpret *p_stTree, AI_tdstGetSetParam *p_stValue);
/* 0x0047AAB0 */
AI_tdstNodeInterpret *__cdecl fn_p_stGetDriversAvailable(
    HIE_tdstSuperObject *p_SuperObjPerso, AI_tdstNodeInterpret *p_stTree, AI_tdstGetSetParam *p_stValue);
AI_tdstNodeInterpret *__cdecl fn_p_stGetCurrentLanguageId(
    HIE_tdstSuperObject *p_SuperObjPerso, AI_tdstNodeInterpret *p_stTree, AI_tdstGetSetParam *p_stValue);
AI_tdstNodeInterpret *__cdecl fn_p_stGetNbLanguages(
    HIE_tdstSuperObject *p_SuperObjPerso, AI_tdstNodeInterpret *p_stTree, AI_tdstGetSetParam *p_stValue);
AI_tdstNodeInterpret *__cdecl fn_p_stGetLanguageText(
    HIE_tdstSuperObject *p_SuperObjPerso, AI_tdstNodeInterpret *p_stTree, AI_tdstGetSetParam *p_stValue);
AI_tdstNodeInterpret *__cdecl fn_p_stTextToInt(
    HIE_tdstSuperObject *p_SuperObjPerso, AI_tdstNodeInterpret *p_stTree, AI_tdstGetSetParam *p_stValue);
/* 0x0047ABA0 */
AI_tdstNodeInterpret *__cdecl fn_p_stOptionRecupererFunc(
    HIE_tdstSuperObject *p_SuperObjPerso, AI_tdstNodeInterpret *p_stTree, AI_tdstGetSetParam *p_stValue);
/* 0x0047ABF0 */
AI_tdstNodeInterpret *__cdecl fn_p_stOptionSlotIsValid(
    HIE_tdstSuperObject *p_SuperObjPerso, AI_tdstNodeInterpret *p_stTree, AI_tdstGetSetParam *p_stValue);
/* 0x0047AD80 */
AI_tdstNodeInterpret *__cdecl fn_p_stGetMSSoundValues(
    HIE_tdstSuperObject *p_SuperObjPerso, AI_tdstNodeInterpret *p_stTree, AI_tdstGetSetParam *p_stValue);
/* 0x0047AE10 */
AI_tdstNodeInterpret *__cdecl fn_p_stGetStdGameLimit(
    HIE_tdstSuperObject *p_SuperObjPerso, AI_tdstNodeInterpret *p_stTree, AI_tdstGetSetParam *p_stValue);
/* 0x0047C350 */
AI_tdstNodeInterpret *__cdecl fn_p_stVectorFunction(
    HIE_tdstSuperObject *p_SuperObjPerso, AI_tdstNodeInterpret *p_stTree, AI_tdstGetSetParam *p_stValue);
/* 0x0047B0E0 */
AI_tdstNodeInterpret *__cdecl fn_p_stComputeProtectKey(
    HIE_tdstSuperObject *p_SuperObjPerso, AI_tdstNodeInterpret *p_stTree, AI_tdstGetSetParam *p_stValue);
/* 0x0047B170 */
AI_tdstNodeInterpret *__cdecl fn_p_stComputeXor(
    HIE_tdstSuperObject *p_SuperObjPerso, AI_tdstNodeInterpret *p_stTree, AI_tdstGetSetParam *p_stValue);
/* 0x0047B340 */
AI_tdstNodeInterpret *__cdecl AI_pc_fn_p_stComputeNotU(
    HIE_tdstSuperObject *p_SuperObjPerso, AI_tdstNodeInterpret *p_stTree, AI_tdstGetSetParam *p_stValue);
/* 0x0047CE80 */
AI_tdstNodeInterpret *__cdecl fn_p_stGameMaterialFunction(
    HIE_tdstSuperObject *p_SuperObjPerso, AI_tdstNodeInterpret *p_stTree, AI_tdstGetSetParam *p_stValue);
/* 0x0047D190 */
AI_tdstNodeInterpret *__cdecl fn_p_stColorFunction(
    HIE_tdstSuperObject *p_SuperObjPerso, AI_tdstNodeInterpret *p_stTree, AI_tdstGetSetParam *p_stValue);
/* 0x0047CF90 */
AI_tdstNodeInterpret *__cdecl fn_p_stVisualGameMaterialFunction(
    HIE_tdstSuperObject *p_SuperObjPerso, AI_tdstNodeInterpret *p_stTree, AI_tdstGetSetParam *p_stValue);
/* 0x0047D6A0 */
AI_tdstNodeInterpret *__cdecl fn_p_st_SPO_GetDrawFlag(
    HIE_tdstSuperObject *p_SuperObjPerso, AI_tdstNodeInterpret *p_stTree, AI_tdstGetSetParam *p_stValue);

AI_tdstNodeInterpret *__cdecl fn_p_stHitPointsProcedure(
    HIE_tdstSuperObject *p_SuperObjPerso, AI_tdstNodeInterpret *p_stTree);
/* 0x00467FA0 */
AI_tdstNodeInterpret *__cdecl fn_p_stHitPointsMaxProcedure(
    HIE_tdstSuperObject *p_SuperObjPerso, AI_tdstNodeInterpret *p_stTree);
/* 0x00472D60 */
AI_tdstNodeInterpret *__cdecl fn_p_st_ACT_SetDrawFlag(
    HIE_tdstSuperObject *p_SuperObjPerso, AI_tdstNodeInterpret *p_stTree);
/* 0x00468180 */
AI_tdstNodeInterpret *__cdecl fn_p_stListZDDZDEProcedure(
    HIE_tdstSuperObject *p_SuperObjPerso, AI_tdstNodeInterpret *p_stTree);
/* 0x004682B0 */
AI_tdstNodeInterpret *__cdecl fn_p_stListProcedure(
    HIE_tdstSuperObject *p_SuperObjPerso, AI_tdstNodeInterpret *p_stTree);
/* 0x00468660 */
AI_tdstNodeInterpret *__cdecl fn_p_stListEnsembleProcedure(
    HIE_tdstSuperObject *p_SuperObjPerso, AI_tdstNodeInterpret *p_stTree);
/* 0x00468B90 */
AI_tdstNodeInterpret *__cdecl fn_p_stFogProcedure(
    HIE_tdstSuperObject *p_SuperObjPerso, AI_tdstNodeInterpret *p_stTree);
/* 0x00488220 */
AI_tdstNodeInterpret *__cdecl MGT_fn_p_stActiveMagnet(
    HIE_tdstSuperObject *p_SuperObjPerso, AI_tdstNodeInterpret *p_stTree);
/* 0x00488260 */
AI_tdstNodeInterpret *__cdecl MGT_fn_p_stDeactiveMagnet(
    HIE_tdstSuperObject *p_SuperObjPerso, AI_tdstNodeInterpret *p_stTree);
/* 0x004882C0 */
AI_tdstNodeInterpret *__cdecl MGT_fn_p_stSetParam(
    HIE_tdstSuperObject *p_SuperObjPerso, AI_tdstNodeInterpret *p_stTree);
/* 0x00472990 */
AI_tdstNodeInterpret *__cdecl fn_p_st_FootPath_AddFootPrint(
    HIE_tdstSuperObject *p_SuperObjPerso, AI_tdstNodeInterpret *p_stTree);
/* 0x004729D0 */
AI_tdstNodeInterpret *__cdecl fn_p_st_FootPath_Clear(
    HIE_tdstSuperObject *p_SuperObjPerso, AI_tdstNodeInterpret *p_stTree);
/* 0x00472A00 */
AI_tdstNodeInterpret *__cdecl fn_p_st_SinusEffect_SetFreq(
    HIE_tdstSuperObject *p_SuperObjPerso, AI_tdstNodeInterpret *p_stTree);
/* 0x00472A80 */
AI_tdstNodeInterpret *__cdecl fn_p_st_SinusEffect_SetAmplitude(
    HIE_tdstSuperObject *p_SuperObjPerso, AI_tdstNodeInterpret *p_stTree);
/* 0x00472BA0 */
AI_tdstNodeInterpret *__cdecl fn_p_st_SinusEffect_SetState(
    HIE_tdstSuperObject *p_SuperObjPerso, AI_tdstNodeInterpret *p_stTree);
/* 0x00472B00 */
AI_tdstNodeInterpret *__cdecl fn_p_st_SinusEffect_SetRLIParams(
    HIE_tdstSuperObject *p_SuperObjPerso, AI_tdstNodeInterpret *p_stTree);
/* 0x00472C40 */
AI_tdstNodeInterpret *__cdecl fn_p_st_SPO_SetDrawFlag(
    HIE_tdstSuperObject *p_SuperObjPerso, AI_tdstNodeInterpret *p_stTree);
/* 0x00472CD0 */
AI_tdstNodeInterpret *__cdecl fn_p_st_SPO_SetEngineDisplayModeFlag(
    HIE_tdstSuperObject *p_SuperObjPerso, AI_tdstNodeInterpret *p_stTree);
/* 0x00469520 */
AI_tdstNodeInterpret *__cdecl fn_p_stKeyboardProcedure(
    HIE_tdstSuperObject *p_SuperObjPerso, AI_tdstNodeInterpret *p_stTree);
/* 0x00472F90 */
AI_tdstNodeInterpret *__cdecl fn_p_stNoneProcedure(
    HIE_tdstSuperObject *p_SuperObjPerso, AI_tdstNodeInterpret *p_stTree);
/* 0x00469580 */
AI_tdstNodeInterpret *__cdecl fn_p_stChangeComportIntelligenceProcedure(
    HIE_tdstSuperObject *p_SuperObjPerso, AI_tdstNodeInterpret *p_stTree);
/* 0x00469640 */
AI_tdstNodeInterpret *__cdecl fn_p_stChangeComportReflexProcedure(
    HIE_tdstSuperObject *p_SuperObjPerso, AI_tdstNodeInterpret *p_stTree);
/* 0x004696C0 */
AI_tdstNodeInterpret *__cdecl fn_p_stChangeMyComportIntelligenceProcedure(
    HIE_tdstSuperObject *p_SuperObjPerso, AI_tdstNodeInterpret *p_stTree);
/* 0x00469710 */
AI_tdstNodeInterpret *__cdecl fn_p_stChangeMyComportReflexProcedure(
    HIE_tdstSuperObject *p_SuperObjPerso, AI_tdstNodeInterpret *p_stTree);
/* 0x00469760 */
AI_tdstNodeInterpret *__cdecl fn_p_stChangeMyComportIntelligenceAndReflexProcedure(
    HIE_tdstSuperObject *p_SuperObjPerso, AI_tdstNodeInterpret *p_stTree);
/* 0x00472290 */
AI_tdstNodeInterpret *__cdecl fn_p_stChangeActionProcedure(
    HIE_tdstSuperObject *p_SuperObjPerso, AI_tdstNodeInterpret *p_stTree);
/* 0x00472370 */
AI_tdstNodeInterpret *__cdecl fn_p_stChangeActionRandomProcedure(
    HIE_tdstSuperObject *p_SuperObjPerso, AI_tdstNodeInterpret *p_stTree);
/* 0x00472E90 */
AI_tdstNodeInterpret *__cdecl fn_p_stLoopKeyWord(
    HIE_tdstSuperObject *p_SuperObjPerso, AI_tdstNodeInterpret *p_stTree);
/* 0x00472F00 */
AI_tdstNodeInterpret *__cdecl fn_p_stEndLoopKeyWord(
    HIE_tdstSuperObject *p_SuperObjPerso, AI_tdstNodeInterpret *p_stTree);
/* 0x00472F30 */
AI_tdstNodeInterpret *__cdecl fn_p_stBreakKeyword(
    HIE_tdstSuperObject *p_SuperObjPerso, AI_tdstNodeInterpret *p_stTree);
/* 0x00467770 */
AI_tdstNodeInterpret *__cdecl fn_p_stMiscNoProcedure(
    HIE_tdstSuperObject *p_SuperObjPerso, AI_tdstNodeInterpret *p_stTree);
/* 0x00467970 */
AI_tdstNodeInterpret *__cdecl fn_p_stMiscProcedure(
    HIE_tdstSuperObject *p_SuperObjPerso, AI_tdstNodeInterpret *p_stTree);
/* 0x004675A0 */
AI_tdstNodeInterpret *__cdecl fn_p_stMiscUltraProcedure(
    HIE_tdstSuperObject *p_SuperObjPerso, AI_tdstNodeInterpret *p_stTree);
/* 0x00467C60 */
AI_tdstNodeInterpret *__cdecl fn_p_stMiscMoreProcedure(
    HIE_tdstSuperObject *p_SuperObjPerso, AI_tdstNodeInterpret *p_stTree);
/* 0x00468730 */
AI_tdstNodeInterpret *__cdecl fn_p_stLinkTableProcedure(
    HIE_tdstSuperObject *p_SuperObjPerso, AI_tdstNodeInterpret *p_stTree);
/* 0x004687B0 */
AI_tdstNodeInterpret *__cdecl fn_p_stBuildObjectTableFromTableAndStringProcedure(
    HIE_tdstSuperObject *p_SuperObjPerso, AI_tdstNodeInterpret *p_stTree);
/* 0x00468970 */
AI_tdstNodeInterpret *__cdecl fn_p_stModuleControlProcedure(
    HIE_tdstSuperObject *p_SuperObjPerso, AI_tdstNodeInterpret *p_stTree);
/* 0x00468AB0 */
AI_tdstNodeInterpret *__cdecl fn_p_stSaveGameProcedure(
    HIE_tdstSuperObject *p_SuperObjPerso, AI_tdstNodeInterpret *p_stTree);
/* 0x00468B10 */
AI_tdstNodeInterpret *__cdecl fn_p_stChangeActivationChannelProcedure(
    HIE_tdstSuperObject *p_SuperObjPerso, AI_tdstNodeInterpret *p_stTree);
/* 0x00468E90 */
AI_tdstNodeInterpret *__cdecl fn_p_stPersoLightProcedure(
    HIE_tdstSuperObject *p_SuperObjPerso, AI_tdstNodeInterpret *p_stTree);
/* 0x004691E0 */
AI_tdstNodeInterpret *__cdecl fn_p_stSoundProcedure(
    HIE_tdstSuperObject *p_SuperObjPerso, AI_tdstNodeInterpret *p_stTree);
/* 0x004692D0 */
AI_tdstNodeInterpret *__cdecl fn_p_stParticleGeneratorProcedure(
    HIE_tdstSuperObject *p_SuperObjPerso, AI_tdstNodeInterpret *p_stTree);
/* 0x00472450 */
AI_tdstNodeInterpret *__cdecl fn_p_stTransparenceProcedure(
    HIE_tdstSuperObject *p_SuperObjPerso, AI_tdstNodeInterpret *p_stTree);
/* 0x00472400 */
AI_tdstNodeInterpret *__cdecl fn_p_stDisplayProcedure(
    HIE_tdstSuperObject *p_SuperObjPerso, AI_tdstNodeInterpret *p_stTree);
/* 0x00472930 */
AI_tdstNodeInterpret *__cdecl fn_p_stMenuAndVignetteProcedure(
    HIE_tdstSuperObject *p_SuperObjPerso, AI_tdstNodeInterpret *p_stTree);
/* 0x004697D0 */
AI_tdstNodeInterpret *__cdecl fn_p_stTurnProcedure(
    HIE_tdstSuperObject *p_SuperObjPerso, AI_tdstNodeInterpret *p_stTree);
/* 0x00472100 */
AI_tdstNodeInterpret *__cdecl fn_p_stTurnPersoProcedure(
    HIE_tdstSuperObject *p_SuperObjPerso, AI_tdstNodeInterpret *p_stTree);
/* 0x00467850 */
AI_tdstNodeInterpret *__cdecl fn_p_stKillPersoAndClearVariableProcedure(
    HIE_tdstSuperObject *p_SuperObjPerso, AI_tdstNodeInterpret *p_stTree);
/* 0x00472600 */
AI_tdstNodeInterpret *__cdecl fn_p_stVisualGameMaterialProcedure(
    HIE_tdstSuperObject *p_SuperObjPerso, AI_tdstNodeInterpret *p_stTree);
/* 0x00472FA0 */
AI_tdstNodeInterpret *__cdecl fn_p_stAGOProcedure(
    HIE_tdstSuperObject *p_SuperObjPerso, AI_tdstNodeInterpret *p_stTree);
/* 0x0046BC60 */
AI_tdstNodeInterpret *__cdecl fn_p_stOptionChangerProc(
    HIE_tdstSuperObject *p_SuperObjPerso, AI_tdstNodeInterpret *p_stTree);
/* 0x00467CF0 */
AI_tdstNodeInterpret *__cdecl fn_p_stDynamProcedure(
    HIE_tdstSuperObject *p_SuperObjPerso, AI_tdstNodeInterpret *p_stTree);
/* 0x004698A0 */
AI_tdstNodeInterpret *__cdecl fn_p_stCustomBitsProc(
    HIE_tdstSuperObject *p_SuperObjPerso, AI_tdstNodeInterpret *p_stTree);
/* 0x0046AB20 */
AI_tdstNodeInterpret *__cdecl fn_p_st3DProc(
    HIE_tdstSuperObject *p_SuperObjPerso, AI_tdstNodeInterpret *p_stTree);
/* 0x0046BF20 */
AI_tdstNodeInterpret *__cdecl fn_p_stFixePositionPerso(
    HIE_tdstSuperObject *p_SuperObjPerso, AI_tdstNodeInterpret *p_stTree);
/* 0x004708B0 */
AI_tdstNodeInterpret *__cdecl fn_p_stCode4MagicPointsProcedures(
    HIE_tdstSuperObject *p_SuperObjPerso, AI_tdstNodeInterpret *p_stTree);
/* 0x00470770 */
AI_tdstNodeInterpret *__cdecl fn_p_stCode4AirPointsProcedures(
    HIE_tdstSuperObject *p_SuperObjPerso, AI_tdstNodeInterpret *p_stTree);
/* 0x004709D0 */
AI_tdstNodeInterpret *__cdecl fn_p_stSetLocalLight(
    HIE_tdstSuperObject *p_SuperObjPerso, AI_tdstNodeInterpret *p_stTree);
/* 0x00470B20 */
AI_tdstNodeInterpret *__cdecl fn_p_stSetLight(
    HIE_tdstSuperObject *p_SuperObjPerso, AI_tdstNodeInterpret *p_stTree);
/* 0x00470F20 */
AI_tdstNodeInterpret *__cdecl fn_p_stRLIProcedure(
    HIE_tdstSuperObject *p_SuperObjPerso, AI_tdstNodeInterpret *p_stTree);
/* 0x00470D60 */
AI_tdstNodeInterpret *__cdecl fn_p_stReinitGraphProcedure(
    HIE_tdstSuperObject *p_SuperObjPerso, AI_tdstNodeInterpret *p_stTree);
/* 0x00470DA0 */
AI_tdstNodeInterpret *__cdecl fn_p_stSetScrollSpeed(
    HIE_tdstSuperObject *p_SuperObjPerso, AI_tdstNodeInterpret *p_stTree);
/* 0x00471020 */
AI_tdstNodeInterpret *__cdecl fn_p_stManageFrame(
    HIE_tdstSuperObject *p_SuperObjPerso, AI_tdstNodeInterpret *p_stTree);
/* 0x00471780 */
AI_tdstNodeInterpret *__cdecl fn_p_stFreezeAnim(
    HIE_tdstSuperObject *p_SuperObjPerso, AI_tdstNodeInterpret *p_stTree);
/* 0x00471370 */
AI_tdstNodeInterpret *__cdecl fn_p_stSetSPOCoordinates(
    HIE_tdstSuperObject *p_SuperObjPerso, AI_tdstNodeInterpret *p_stTree);
/* 0x004710C0 */
AI_tdstNodeInterpret *__cdecl fn_p_stSPOSuperimpoed(
    HIE_tdstSuperObject *p_SuperObjPerso, AI_tdstNodeInterpret *p_stTree);
/* 0x004717F0 */
AI_tdstNodeInterpret *__cdecl fn_p_stFixePositionFade(
    HIE_tdstSuperObject *p_SuperObjPerso, AI_tdstNodeInterpret *p_stTree);
/* 0x00471A90 */
AI_tdstNodeInterpret *__cdecl fn_p_stSetZDxSize(
    HIE_tdstSuperObject *p_SuperObjPerso, AI_tdstNodeInterpret *p_stTree);
/* 0x00471990 */
AI_tdstNodeInterpret *__cdecl fn_p_stSortArray(
    HIE_tdstSuperObject *p_SuperObjPerso, AI_tdstNodeInterpret *p_stTree);
/* 0x0047EF80 */
AI_tdstNodeInterpret *__cdecl CAM_fn_p_stForceMovingOnRail(
    HIE_tdstSuperObject *p_SuperObjPerso, AI_tdstNodeInterpret *p_stTree);
/* 0x00471A50 */
AI_tdstNodeInterpret *__cdecl fn_p_stActiveJoystickAnal(
    HIE_tdstSuperObject *p_SuperObjPerso, AI_tdstNodeInterpret *p_stTree);
/* 0x00471E40 */
AI_tdstNodeInterpret *__cdecl fn_p_stPauseSound(
    HIE_tdstSuperObject *p_SuperObjPerso, AI_tdstNodeInterpret *p_stTree);
/* 0x00471D10 */
AI_tdstNodeInterpret *__cdecl fn_p_stSetSoundVolume(
    HIE_tdstSuperObject *p_SuperObjPerso, AI_tdstNodeInterpret *p_stTree);
/* 0x0046FAA0 */
AI_tdstNodeInterpret *__cdecl fn_p_stInitKeyBoardDirections(
    HIE_tdstSuperObject *p_SuperObjPerso, AI_tdstNodeInterpret *p_stTree);
/* 0x004731B0 */
AI_tdstNodeInterpret *__cdecl fn_p_stJFFTXTProcedure(
    HIE_tdstSuperObject *p_SuperObjPerso, AI_tdstNodeInterpret *p_stTree);
/* 0x004730A0 */
AI_tdstNodeInterpret *__cdecl fn_p_stInGameMenuProcedure(
    HIE_tdstSuperObject *p_SuperObjPerso, AI_tdstNodeInterpret *p_stTree);
/* 0x004730E0 */
AI_tdstNodeInterpret *__cdecl AI_fn_p_stSetPcEngineFlags(
    HIE_tdstSuperObject *p_SuperObjPerso, AI_tdstNodeInterpret *p_stTree);
/* 0x00471500 */
AI_tdstNodeInterpret *__cdecl fn_p_stTractionFactorProcedure(
    HIE_tdstSuperObject *p_SuperObjPerso, AI_tdstNodeInterpret *p_stTree);
/* 0x00470C80 */
AI_tdstNodeInterpret *__cdecl fn_p_stCapabilityProcedure(
    HIE_tdstSuperObject *p_SuperObjPerso, AI_tdstNodeInterpret *p_stTree);
/* 0x00471FB0 */
AI_tdstNodeInterpret *__cdecl fn_p_stSetTooFarLimit(
    HIE_tdstSuperObject *p_SuperObjPerso, AI_tdstNodeInterpret *p_stTree);
/* 0x00472020 */
AI_tdstNodeInterpret *__cdecl fn_p_stSetTransparencyZone(
    HIE_tdstSuperObject *p_SuperObjPerso, AI_tdstNodeInterpret *p_stTree);
/* 0x004720A0 */
AI_tdstNodeInterpret *__cdecl fn_p_stBaseFrequenceForMenu(
    HIE_tdstSuperObject *p_SuperObjPerso, AI_tdstNodeInterpret *p_stTree);
/* 0x004720E0 */
AI_tdstNodeInterpret *__cdecl fn_p_stSaveSinusEffectContext(
    HIE_tdstSuperObject *p_SuperObjPerso, AI_tdstNodeInterpret *p_stTree);
/* 0x00471E00 */
AI_tdstNodeInterpret *__cdecl fn_p_stSetDopplerEffect(
    HIE_tdstSuperObject *p_SuperObjPerso, AI_tdstNodeInterpret *p_stTree);
/* 0x00469CB0 */
AI_tdstNodeInterpret *__cdecl fn_p_st3DProc_ForReorientation(
    HIE_tdstSuperObject *p_SuperObjPerso, AI_tdstNodeInterpret *p_stTree);
/* 0x0046A890 */
AI_tdstNodeInterpret *__cdecl fn_p_stChangePersoHorizSighting(
    HIE_tdstSuperObject *p_SuperObjPerso, AI_tdstNodeInterpret *p_stTree);
/* 0x0046CBD0 */
AI_tdstNodeInterpret *__cdecl fn_p_SetMechanicSpeedVector(
    HIE_tdstSuperObject *p_SuperObjPerso, AI_tdstNodeInterpret *p_stTree);
/* 0x0046CE00 */
AI_tdstNodeInterpret *__cdecl fn_p_SetMechanicSpeedReal(
    HIE_tdstSuperObject *p_SuperObjPerso, AI_tdstNodeInterpret *p_stTree);
/* 0x00469B30 */
AI_tdstNodeInterpret *__cdecl fn_p_stCollideCommunicationProc(
    HIE_tdstSuperObject *p_SuperObjPerso, AI_tdstNodeInterpret *p_stTree);
/* 0x0046DCE0 */
AI_tdstNodeInterpret *__cdecl fn_p_stResetLastCollisionActor(
    HIE_tdstSuperObject *p_SuperObjPerso, AI_tdstNodeInterpret *p_stTree);
/* 0x00469960 */
AI_tdstNodeInterpret *__cdecl fn_p_stStringProc(
    HIE_tdstSuperObject *p_SuperObjPerso, AI_tdstNodeInterpret *p_stTree);
/* 0x00469AC0 */
AI_tdstNodeInterpret *__cdecl fn_p_stIntToText(
    HIE_tdstSuperObject *p_SuperObjPerso, AI_tdstNodeInterpret *p_stTree);
/* 0x0046C1D0 */
AI_tdstNodeInterpret *__cdecl fn_p_SetMechanicOption(
    HIE_tdstSuperObject *p_SuperObjPerso, AI_tdstNodeInterpret *p_stTree);
/* 0x0046C710 */
AI_tdstNodeInterpret *__cdecl fn_p_SetMechanicRealParameter(
    HIE_tdstSuperObject *p_SuperObjPerso, AI_tdstNodeInterpret *p_stTree);
/* 0x0046C980 */
AI_tdstNodeInterpret *__cdecl fn_p_SetMechanic3RealParameter(
    HIE_tdstSuperObject *p_SuperObjPerso, AI_tdstNodeInterpret *p_stTree);
/* 0x0046CA40 */
AI_tdstNodeInterpret *__cdecl fn_p_SetMechanicVectorParameter(
    HIE_tdstSuperObject *p_SuperObjPerso, AI_tdstNodeInterpret *p_stTree);
/* 0x0046DC00 */
AI_tdstNodeInterpret *__cdecl fn_p_SetMechanicListParameter(
    HIE_tdstSuperObject *p_SuperObjPerso, AI_tdstNodeInterpret *p_stTree);
/* 0x0046D9A0 */
AI_tdstNodeInterpret *__cdecl fn_p_PlatformLink(
    HIE_tdstSuperObject *p_SuperObjPerso, AI_tdstNodeInterpret *p_stTree);
/* 0x0046DA50 */
AI_tdstNodeInterpret *__cdecl fn_p_HierFatherLink(
    HIE_tdstSuperObject *p_SuperObjPerso, AI_tdstNodeInterpret *p_stTree);
/* 0x0046E3C0 */
AI_tdstNodeInterpret *__cdecl fn_p_LinkControl(
    HIE_tdstSuperObject *p_SuperObjPerso, AI_tdstNodeInterpret *p_stTree);
/* 0x0046BFE0 */
AI_tdstNodeInterpret *__cdecl fn_p_stFixePositionZDx(
    HIE_tdstSuperObject *p_SuperObjPerso, AI_tdstNodeInterpret *p_stTree);
/* 0x0046C640 */
AI_tdstNodeInterpret *__cdecl fn_p_stChangeLigthIntensity(
    HIE_tdstSuperObject *p_SuperObjPerso, AI_tdstNodeInterpret *p_stTree);
/* 0x0046D8A0 */
AI_tdstNodeInterpret *__cdecl fn_SPO_p_stChangeFlag(
    HIE_tdstSuperObject *p_SuperObjPerso, AI_tdstNodeInterpret *p_stTree);
/* 0x0046D920 */
AI_tdstNodeInterpret *__cdecl fn_p_stChangeActorSpoFlag(
    HIE_tdstSuperObject *p_SuperObjPerso, AI_tdstNodeInterpret *p_stTree);
/* 0x0046BD40 */
AI_tdstNodeInterpret *__cdecl fn_p_stChangeScreen(
    HIE_tdstSuperObject *p_SuperObjPerso, AI_tdstNodeInterpret *p_stTree);
/* 0x0046DEA0 */
AI_tdstNodeInterpret *__cdecl fn_p_stShadow(
    HIE_tdstSuperObject *p_SuperObjPerso, AI_tdstNodeInterpret *p_stTree);
/* 0x0046DFB0 */
AI_tdstNodeInterpret *__cdecl fn_p_stSetPrivilegeActivationZDx(
    HIE_tdstSuperObject *p_SuperObjPerso, AI_tdstNodeInterpret *p_stTree);
/* 0x0046E360 */
AI_tdstNodeInterpret *__cdecl fn_p_stActivationZDR(
    HIE_tdstSuperObject *p_SuperObjPerso, AI_tdstNodeInterpret *p_stTree);
/* 0x0046E180 */
AI_tdstNodeInterpret *__cdecl fn_p_stSetCollSetCollisionFlag(
    HIE_tdstSuperObject *p_SuperObjPerso, AI_tdstNodeInterpret *p_stTree);
/* 0x0046E2A0 */
AI_tdstNodeInterpret *__cdecl fn_p_stSetComputationFrequency(
    HIE_tdstSuperObject *p_SuperObjPerso, AI_tdstNodeInterpret *p_stTree);
/* 0x0046BB10 */
AI_tdstNodeInterpret *__cdecl fn_p_stWorldMapProc(
    HIE_tdstSuperObject *p_SuperObjPerso, AI_tdstNodeInterpret *p_stTree);
/* 0x0047E670 */
AI_tdstNodeInterpret *__cdecl CAM_fn_p_stSetIAFlags(
    HIE_tdstSuperObject *p_SuperObjPerso, AI_tdstNodeInterpret *p_stTree);
/* 0x0047E830 */
AI_tdstNodeInterpret *__cdecl CAM_fn_p_stSetDNMFlags(
    HIE_tdstSuperObject *p_SuperObjPerso, AI_tdstNodeInterpret *p_stTree);
/* 0x0047E4C0 */
AI_tdstNodeInterpret *__cdecl CAM_fn_p_stActivate(
    HIE_tdstSuperObject *p_SuperObjPerso, AI_tdstNodeInterpret *p_stTree);
/* 0x0047E400 */
AI_tdstNodeInterpret *__cdecl CAM_fn_p_stChangeChannel(
    HIE_tdstSuperObject *p_SuperObjPerso, AI_tdstNodeInterpret *p_stTree);
/* 0x0047E200 */
AI_tdstNodeInterpret *__cdecl CAM_fn_p_stChangeTgtPerso(
    HIE_tdstSuperObject *p_SuperObjPerso, AI_tdstNodeInterpret *p_stTree);
/* 0x0047E2E0 */
AI_tdstNodeInterpret *__cdecl CAM_fn_p_stChangeSecondTgtPerso(
    HIE_tdstSuperObject *p_SuperObjPerso, AI_tdstNodeInterpret *p_stTree);
/* 0x0047E540 */
AI_tdstNodeInterpret *__cdecl CAM_fn_p_stAssociateViewport(
    HIE_tdstSuperObject *p_SuperObjPerso, AI_tdstNodeInterpret *p_stTree);
/* 0x0047DEB0 */
AI_tdstNodeInterpret *__cdecl CAM_fn_p_stChangeRealParameter(
    HIE_tdstSuperObject *p_SuperObjPerso, AI_tdstNodeInterpret *p_stTree);
/* 0x0047DDE0 */
AI_tdstNodeInterpret *__cdecl CAM_fn_p_stChangeVectorParameter(
    HIE_tdstSuperObject *p_SuperObjPerso, AI_tdstNodeInterpret *p_stTree);
/* 0x0047EA70 */
AI_tdstNodeInterpret *__cdecl CAM_fn_p_stChangeConstants(
    HIE_tdstSuperObject *p_SuperObjPerso, AI_tdstNodeInterpret *p_stTree);
/* 0x0047ED90 */
AI_tdstNodeInterpret *__cdecl CAM_fn_p_stReset(
    HIE_tdstSuperObject *p_SuperObjPerso, AI_tdstNodeInterpret *p_stTree);
/* 0x0047EC20 */
AI_tdstNodeInterpret *__cdecl CAM_fn_p_stSetForce(
    HIE_tdstSuperObject *p_SuperObjPerso, AI_tdstNodeInterpret *p_stTree);
/* 0x0047EBD0 */
AI_tdstNodeInterpret *__cdecl CAM_fn_p_stConstants(
    HIE_tdstSuperObject *p_SuperObjPerso, AI_tdstNodeInterpret *p_stTree);
/* 0x0047EDD0 */
AI_tdstNodeInterpret *__cdecl CAM_fn_p_stForceBestPos(
    HIE_tdstSuperObject *p_SuperObjPerso, AI_tdstNodeInterpret *p_stTree);
/* 0x0047EF20 */
AI_tdstNodeInterpret *__cdecl CAM_fn_p_stForceNormalState(
    HIE_tdstSuperObject *p_SuperObjPerso, AI_tdstNodeInterpret *p_stTree);
/* 0x0047F560 */
AI_tdstNodeInterpret *__cdecl CAM_fn_p_stCameraMode(
    HIE_tdstSuperObject *p_SuperObjPerso, AI_tdstNodeInterpret *p_stTree);
/* 0x0046FB30 */
AI_tdstNodeInterpret *__cdecl fn_p_stSetCenterPosition(
    HIE_tdstSuperObject *p_SuperObjPerso, AI_tdstNodeInterpret *p_stTree);
/* 0x0046FBC0 */
AI_tdstNodeInterpret *__cdecl fn_p_stSetMaximalValues(
    HIE_tdstSuperObject *p_SuperObjPerso, AI_tdstNodeInterpret *p_stTree);
/* 0x00470270 */
AI_tdstNodeInterpret *__cdecl fn_p_stTextSuperImposed(
    HIE_tdstSuperObject *p_SuperObjPerso, AI_tdstNodeInterpret *p_stTree);
/* 0x00470660 */
AI_tdstNodeInterpret *__cdecl fn_p_stCode4ChangeCaractere(
    HIE_tdstSuperObject *p_SuperObjPerso, AI_tdstNodeInterpret *p_stTree);
/* 0x00470560 */
AI_tdstNodeInterpret *__cdecl fn_p_stCode4FormateTexte(
    HIE_tdstSuperObject *p_SuperObjPerso, AI_tdstNodeInterpret *p_stTree);
/* 0x004713E0 */
AI_tdstNodeInterpret *__cdecl fn_p_stMiscProcOnTypeOfWP(
    HIE_tdstSuperObject *p_SuperObjPerso, AI_tdstNodeInterpret *p_stTree);
/* 0x00471570 */
AI_tdstNodeInterpret *__cdecl fn_p_stTurnToPosition(
    HIE_tdstSuperObject *p_SuperObjPerso, AI_tdstNodeInterpret *p_stTree);
/* 0x0047DA40 */
AI_tdstNodeInterpret *__cdecl CAM_fn_p_stUpdatePosition(
    HIE_tdstSuperObject *p_SuperObjPerso, AI_tdstNodeInterpret *p_stTree);
/* 0x0046F830 */
AI_tdstNodeInterpret *__cdecl fn_p_stSetAnalogJoystickAxes(
    HIE_tdstSuperObject *p_SuperObjPerso, AI_tdstNodeInterpret *p_stTree);
/* 0x0046E450 */
AI_tdstNodeInterpret *__cdecl fn_p_stReadAnalogJoystickMario(
    HIE_tdstSuperObject *p_SuperObjPerso, AI_tdstNodeInterpret *p_stTree);
/* 0x0046FC60 */
AI_tdstNodeInterpret *__cdecl fn_p_stSetBooleanInArray(
    HIE_tdstSuperObject *p_SuperObjPerso, AI_tdstNodeInterpret *p_stTree);
/* 0x0046FD70 */
AI_tdstNodeInterpret *__cdecl fn_p_stSetDefaultFormatCharacter(
    HIE_tdstSuperObject *p_SuperObjPerso, AI_tdstNodeInterpret *p_stTree);
/* 0x0046FDE0 */
AI_tdstNodeInterpret *__cdecl fn_p_stSelectDriver(
    HIE_tdstSuperObject *p_SuperObjPerso, AI_tdstNodeInterpret *p_stTree);
/* 0x0046FE00 */
AI_tdstNodeInterpret *__cdecl fn_p_stSelectResolution(
    HIE_tdstSuperObject *p_SuperObjPerso, AI_tdstNodeInterpret *p_stTree);
/* 0x0046FE40 */
AI_tdstNodeInterpret *__cdecl fn_p_stChangeBrightness(
    HIE_tdstSuperObject *p_SuperObjPerso, AI_tdstNodeInterpret *p_stTree);
/* 0x004704C0 */
AI_tdstNodeInterpret *__cdecl fn_p_stTextIntegerParameter(
    HIE_tdstSuperObject *p_SuperObjPerso, AI_tdstNodeInterpret *p_stTree);
/* 0x00470510 */
AI_tdstNodeInterpret *__cdecl fn_p_stTextRealParameter(
    HIE_tdstSuperObject *p_SuperObjPerso, AI_tdstNodeInterpret *p_stTree);
/* 0x0046FE80 */
AI_tdstNodeInterpret *__cdecl fn_p_stSetInStereoMode(
    HIE_tdstSuperObject *p_SuperObjPerso, AI_tdstNodeInterpret *p_stTree);
/* 0x0046FF00 */
AI_tdstNodeInterpret *__cdecl fn_p_stSetaturationAndBackgroundDistance(
    HIE_tdstSuperObject *p_SuperObjPerso, AI_tdstNodeInterpret *p_stTree);
/* 0x0046FF90 */
AI_tdstNodeInterpret *__cdecl fn_p_stSaveCurrentSoundRequest(
    HIE_tdstSuperObject *p_SuperObjPerso, AI_tdstNodeInterpret *p_stTree);
/* 0x0046FEC0 */
AI_tdstNodeInterpret *__cdecl AI_fn_p_stPcSetSoundGlobal(
    HIE_tdstSuperObject *p_SuperObjPerso, AI_tdstNodeInterpret *p_stTree);
/* 0x0046DD50 */
AI_tdstNodeInterpret *__cdecl fn_p_SetMechanic3Real(
    HIE_tdstSuperObject *p_SuperObjPerso, AI_tdstNodeInterpret *p_stTree);
/* 0x0046DE50 */
AI_tdstNodeInterpret *__cdecl fn_p_SetMechanicClimbSpeedLimit(
    HIE_tdstSuperObject *p_SuperObjPerso, AI_tdstNodeInterpret *p_stTree);
/* 0x00487FD0 */
AI_tdstNodeInterpret *__cdecl fn_p_stPrepareMorph(
    HIE_tdstSuperObject *p_SuperObjPerso, AI_tdstNodeInterpret *p_stTree);
AI_tdstNodeInterpret *__cdecl fn_p_stStopMorph(
    HIE_tdstSuperObject *p_SuperObjPerso, AI_tdstNodeInterpret *p_stTree);
AI_tdstNodeInterpret *__cdecl fn_p_stMorphing(
    HIE_tdstSuperObject *p_SuperObjPerso, AI_tdstNodeInterpret *p_stTree);
/* 0x0047BCE0 */
AI_tdstNodeInterpret *__cdecl fn_p_stMathFunctionInt(
    HIE_tdstSuperObject *p_SuperObjPerso, AI_tdstNodeInterpret *p_stTree, AI_tdstGetSetParam *p_stValue);
/* 0x0047BDC0 */
AI_tdstNodeInterpret *__cdecl fn_p_stMathFunctionReal(
    HIE_tdstSuperObject *p_SuperObjPerso, AI_tdstNodeInterpret *p_stTree, AI_tdstGetSetParam *p_stValue);

typedef struct AI_tdstTreeInterpret_
{
    AI_tdstNodeInterpret *p_stNodeInterpret;

} AI_tdstTreeInterpret;

typedef struct AI_tdstMacro_
{
    AI_tdstTreeInterpret *p_stInitTree;
    AI_tdstTreeInterpret *p_stCurrentTree;
} AI_tdstMacro;

typedef struct AI_tdstComport_
{
    AI_tdstTreeInterpret *p_stRules;
    AI_tdstTreeInterpret *p_stSchedule;
    unsigned char         ucNbRules;        /* 0x480E00 */
} AI_tdstComport;

#define AI_M_GetComportSchedule(p_stComp)     ((p_stComp)->p_stSchedule)
#define AI_M_GetTreeComportSchedule(p_stComp) (AI_M_GetComportSchedule(p_stComp)->p_stNodeInterpret)
#define AI_M_GetCurrentComport(p_stInt)       ((p_stInt)->p_stCurrentComport)
#define AI_M_GetPrevComport(p_stInt)          ((p_stInt)->p_stPrevComport)
#define AI_M_GetCurrentSchedule(p_stInt)      ((p_stInt)->p_stCurrentSchedule)

#define AI_M_GetComportRules(p_stComp)        (((AI_tdstComport *)(p_stComp))->p_stRules)
#define AI_M_GetComportNbRules(p_stComp)      (((AI_tdstComport *)(p_stComp))->ucNbRules)
#define AI_M_GetRules(p_stInt)                (AI_M_GetComportRules(AI_M_GetCurrentComport(p_stInt)))
#define AI_M_GetRuleN(p_stInt, NRule)         (&(AI_M_GetRules(p_stInt)[(NRule)]))
#define AI_M_GetNbRules(p_stInt)              (AI_M_GetComportNbRules(AI_M_GetCurrentComport(p_stInt)))
#define AI_M_GetNodeInterpret(p_Tree)         ((p_Tree)->p_stNodeInterpret)
#define AI_M_GetNodeInterpretN(p_Tree, NNode) (&(AI_M_GetNodeInterpret(p_Tree)[NNode]))
#define AI_M_ActionTableNbEntries(p_stInt)    ((p_stInt)->p_stActionTable->ucNbEntries)

#define AI_M_ActionTableNbEntriesUsed(p_stInt)   ((p_stInt)->p_stActionTable->ucNbEntriesUsed)
#define AI_M_IsActionTableUsed(p_stInt)          (AI_M_ActionTableNbEntriesUsed(p_stInt) != 0)
#define AI_M_ActionTableEntryN(p_stInt, N)       ((p_stInt)->p_stActionTable->p_stEntry[(N)])
#define AI_M_ActionTableNumNode(p_stInt, N)      (AI_M_ActionTableEntryN(p_stInt, N).p_stNode)
#define AI_M_ActionTableUseActionReturn(p_stInt, N) (AI_M_ActionTableEntryN(p_stInt, N).bUseDefaultActionReturn)
#define AI_M_ActionTableNewActionReturn(p_stInt, N) (AI_M_ActionTableEntryN(p_stInt, N).ucNewActionReturn)
#define AI_M_ActionTableUseFlag(p_stInt, N)      (AI_M_ActionTableEntryN(p_stInt, N).bUsed)
#define AI_M_ActionTableNumRule(p_stInt, N)      (AI_M_ActionTableEntryN(p_stInt, N).ucNumRule)
#define AI_M_ActionTableParam(p_stInt, N)        (AI_M_ActionTableEntryN(p_stInt, N).a_ucActionParamBlob)
#define AI_M_ActionTableCurrentEntry(p_stInt)    ((p_stInt)->p_stActionTable->ucCurrentEntry)

void __cdecl fn_vInitActionTableEntry(AI_tdstIntelligence *p_stIntelligence, unsigned char ucNEntry,
                                      unsigned char ucNumRule, AI_tdstNodeInterpret *p_stNode, unsigned char bfUsed);
void __cdecl fn_vUnuseCurrentActionTableEntry(AI_tdstIntelligence *p_stIntelligence);
void __cdecl fn_vUnuseAllRulesWhichHaveGreaterNumRule(AI_tdstIntelligence *p_stIntelligence, unsigned char ucNumRule);
void __cdecl fn_vUnuseAllRulesFromSchedule(AI_tdstIntelligence *p_stIntelligence);

unsigned char __cdecl fn_ucIsRuleInActionTable(AI_tdstIntelligence *p_stIntelligence, unsigned char ucNumRule);
AI_tdstNodeInterpret *__cdecl fn_p_stGetTableAction(AI_tdstIntelligence *p_stIntelligence, unsigned char ucNEntry);

unsigned char __cdecl fn_ucIntelligenceRulesEngine(HIE_tdstSuperObject *p_SuperObjPerso, AI_tdstIntelligence *p_stIntelligence);
unsigned char __cdecl fn_ucIntelligenceScheduleEngine(HIE_tdstSuperObject *p_SuperObjPerso, AI_tdstIntelligence *p_stIntelligence);
void          __cdecl fn_vComputeScheduleInterrupt(AI_tdstIntelligence *p_stIntelligence);

#define AI_M_pfn_fn_p_stIntelligenceEvalTreeEngine fn_p_stIntelligenceEvalTreeEngine /* 0x00480BD0 */
#define AI_M_pfn_fn_ucIsRuleInActionTable       fn_ucIsRuleInActionTable       /* 0x004826A0 */
#define AI_M_pfn_fn_p_stGetTableAction          fn_p_stGetTableAction          /* 0x00482990 */
#define AI_M_pfn_fn_vUnuseAllRulesFromSchedule  fn_vUnuseAllRulesFromSchedule  /* 0x004828E0 */

AI_tdstNodeInterpret *__cdecl fn_p_stIntelligenceEvalTreeEngine(
    HIE_tdstSuperObject *p_SuperObjPerso, AI_tdstNodeInterpret *p_stTree, AI_tdstGetSetParam *p_stValue);
#define AI_M_pfn_fn_ucGetActionReturn  fn_ucGetActionReturn   /* 0x00480D10 */

unsigned char __cdecl fn_ucGetActionReturn(AI_tdstGetSetParam *p_stValue, AI_tdstNodeInterpret *p_stTree, unsigned char ucDepth);
unsigned char __cdecl fn_ucIsThereAnotherCommand(AI_tdstNodeInterpret *p_stTree, unsigned char ucDepth);
#define AI_M_pfn_fn_vUnuseCurrentActionTableEntry        fn_vUnuseCurrentActionTableEntry        /* 0x00482820 */
#define AI_M_pfn_fn_vUnuseAllRulesWhichHaveGreaterNumRule fn_vUnuseAllRulesWhichHaveGreaterNumRule /* 0x00482870 */

typedef AI_tdstNodeInterpret *(__cdecl *AI_tdp_fn_p_stProcedureFunctionPtr)(
    HIE_tdstSuperObject *, AI_tdstNodeInterpret *);

AI_tdp_fn_p_stProcedureFunctionPtr __cdecl fn_p_fn_pGetProcedureFunctionPtr(unsigned long eProcId);

typedef struct AI_tdstProcedureEntry_
{
    AI_tdp_fn_p_stProcedureFunctionPtr p_fn_p_stProcedureFunction;
    unsigned char                       bUltraProcedureFlag;
    unsigned char                       _a_ucPad[3];
} AI_tdstProcedureEntry;

/* 0x004A43E0 */
typedef struct AI_tdstFunctionEntry_
{
    AI_tdp_fn_p_stFunctionFunctionPtr p_fn_p_stFunctionFunction;
    unsigned char                     bUltraFunctionFlag;
} AI_tdstFunctionEntry;

extern HIE_tdstSuperObject *g_hCurrentSuperObjPerso;

#define AI_M_pfn_fn_vInitActionTable  fn_vInitActionTable   /* 0x00482940 */

void __cdecl fn_vGetUltraOperatorPerso(
    unsigned char        bYouHaveRightToUseThisFunction,
    HIE_tdstSuperObject *hCurrentSuperObjPerso,
    HIE_tdstSuperObject **hNewSuperObjPerso);

unsigned char __cdecl fn_ucGetProcedureUltraOperator(unsigned long eProcId);

unsigned char __cdecl fn_ucChangeComportIntell(
    AI_tdstIntelligence *p_stIntelligence,
    AI_tdstComport      *p_stComport);

typedef struct AI_tdstBrain_
{
    AI_tdstMind *p_stMind;
    void        *hLastCollidedGoThroughMaterial;
    ACP_tdxBool  bWarnMechanicsFlag;
} AI_tdstBrain;

typedef AI_tdstBrain *MS_tdxHandleToBrain;

#define AI_M_pstGetMindOfBrain(h_Brain) ((h_Brain)->p_stMind)

#define AI_M_stGetMindOfSuperObj(hSupObj) ((AI_tdstMind *)fn_p_stBrainGetMind(M_GetMSHandle((hSupObj), Brain)))

#define AI_M_pfn_fn_bIntelligenceEngine  fn_bIntelligenceEngine   /* 0x00480D80 */
ACP_tdxBool __cdecl fn_bIntelligenceEngine(HIE_tdstSuperObject *p_SuperObjPerso,
                                           AI_tdstIntelligence *p_stIntelligence);

/* 0x00487810 */
unsigned char __cdecl fn_ucGetFunctionUltraOperator(unsigned long eFuncId);

/* 0x481C10 */
struct AI_tdstNodeInterpret_ *__cdecl fn_p_stOperatorGetPerso(
    HIE_tdstSuperObject *p_SuperObjPerso, struct AI_tdstNodeInterpret_ *p_stTree,
    HIE_tdstSuperObject **hSuperObjPersoFound);
void __cdecl fn_vSetUltraOperatorPerso(HIE_tdstSuperObject *hCurrentSuperObjPerso,
                                       HIE_tdstSuperObject *hNewCurrentSuperObjPerso);

/* 0x00482E20 */
void *__cdecl fn_pstGetDNMReport(HIE_tdstSuperObject *p_SuperObjPerso);

#define AI_M_pfn_fn_ucIntelligenceRulesEngine     fn_ucIntelligenceRulesEngine     /* 0x00480E00 */
#define AI_M_pfn_fn_ucIntelligenceScheduleEngine  fn_ucIntelligenceScheduleEngine  /* 0x00480ED0 */
#define AI_M_pfn_fn_vComputeScheduleInterrupt     fn_vComputeScheduleInterrupt     /* 0x00480F70 */

ACP_tdxBool __cdecl fn_bDoReflexPerso(HIE_tdstSuperObject *p_SuperObjPerso);
ACP_tdxBool __cdecl fn_bDoIntelligencePerso(HIE_tdstSuperObject *p_SuperObjPerso);

/* 0x00417E80 */
void __cdecl fn_vBrainAlloc(HIE_tdstEngineObject *p_stObject);
void __cdecl fn_vBrainCopyClone(
    HIE_tdstEngineObject *p_stObjectDest,
    HIE_tdstEngineObject *p_stObjectSrc);

extern HIE_tdstSuperObject *SECT_hFatherSector;

extern unsigned long AI_g_ulSpoMask;
extern unsigned long AI_g_ulOverRideRayTraceSpoMask;

extern MTH3D_tdstVector       AI_g_stCollideParameterPoint;
extern MTH3D_tdstVector       AI_g_stCollideParameterVector;
extern MTH3D_tdstVector       AI_g_stCollideResultPoint;
extern MTH3D_tdstVector       AI_g_stCollideResultVector;
extern HIE_tdstSuperObject   *AI_g_hSuperObjectHit;
extern unsigned long AI_g_ulMode;

#define AI_C_HIE_C_ulActor          ((unsigned long)2)
#define AI_C_GAM_C_CustBitRayHit    ((unsigned long)0x10)
#define AI_C_SECT_ModeLookOnly      ((char)0x02)

/* 0x00474780 */
typedef HIE_tdstSuperObject *(__cdecl *AI_tdp_SCT_fn_hResearchInWhatSectorIAm)(
    HIE_tdstSuperObject *, MTH3D_tdstVector *);
typedef unsigned char (__cdecl *AI_tdp_HIE_bIntersectSegmentWithFirstSuperObject)(
    MTH3D_tdstVector *, MTH3D_tdstVector *, void *,
    MTH3D_tdstVector *, MTH3D_tdstVector *, HIE_tdstSuperObject **);
typedef unsigned char (__cdecl *AI_tdp_SECT_fn_bIsThisSectorVirtual)(HIE_tdstSuperObject *);
typedef void (__cdecl *AI_tdp_SECT_fn_vGetMinPointInBorder)(void *, MTH3D_tdstVector *);
typedef void (__cdecl *AI_tdp_SECT_fn_vGetMaxPointInBorder)(void *, MTH3D_tdstVector *);
typedef unsigned char (__cdecl *AI_tdp_INT_fn_bDetectIntersectSegmentWithBox)(
    MTH3D_tdstVector *, MTH3D_tdstVector *,
    MTH3D_tdstVector *, MTH3D_tdstVector *, MTH3D_tdstVector *);
typedef unsigned long (__cdecl *AI_tdp_fn_ulStandardGameGetCustomBitsSO)(HIE_tdstSuperObject *);

void __cdecl fn_vComputeCollideResult(HIE_tdstSuperObject *hSender);

#define AI_eFunc_GetSlotDate          0xD7
#define AI_eFunc_GetSlotName          0xD8
#define AI_eFunc_GetSlotScore         0xD9
#define AI_eFunc_GetStringCharAt      0xDA
#define AI_eFunc_GetFormattedTextInfo 0xDB
#define AI_eFunc_GetInputEntryName    0xDC

#define M_EvalOneParameter(p_SuperObjPerso,p_stTreeOut,p_stTreeIn,p_stParam) \
    (p_stTreeOut) = fn_p_stEvalTree( (p_SuperObjPerso), (p_stTreeIn), (p_stParam) );
#define M_EvalNextParameter(p_stValue) \
    M_EvalOneParameter(p_SuperObjPerso,p_stTree,p_stTree,(p_stValue))

#define M_EvalNextTwoParameter() \
    M_EvalNextParameter(&stParam1); \
    M_EvalNextParameter(&stParam2);

#define E_ti_Operator 2
#define E_uwAIFatalNotValidFunction ((unsigned short)0x27)

#define M_RealToIntSnd(r) ((r)>>16)
#define M_GetTypeInterpret(p_Tree)              AI_M_GetTypeInterpret(p_Tree)
#define M_ucVarIdInterpret(p_Tree)              AI_M_uVarIdInterpret(p_Tree)
void __cdecl fn_vOnAIErm_M_UpdateLastError(unsigned short FatalErrorNum, char *szMsg);   /* 0x00480FB0 */
#define M_AIFatalErrorMsg(FatalErrorNum,szMsg) fn_vOnAIErm_M_UpdateLastError(FatalErrorNum,szMsg)
#define M_AIFatalError(FatalErrorNum) M_AIFatalErrorMsg(FatalErrorNum,NULL)
#define M_vEvalNextVarId(p_stResultMind, ucResultVarId) \
{ \
  HIE_tdstSuperObject *p_stResultOwnerPerso; \
  if ( M_GetTypeInterpret(p_stTree) == E_ti_Operator )   \
  { \
      \
    p_stTree ++; \
      \
    p_stTree = fn_p_stOperatorGetPerso(p_SuperObjPerso,p_stTree,&p_stResultOwnerPerso); \
  } \
  else \
      \
    p_stResultOwnerPerso = p_SuperObjPerso; \
\
    \
  p_stResultMind = AI_M_stGetMindOfSuperObj(p_stResultOwnerPerso); \
    \
  ucResultVarId = M_ucVarIdInterpret(p_stTree); \
  p_stTree ++; \
}

#define M_eProcedureIdInterpret(p_Tree)         ((p_Tree)->uParam.lValue)

#define AI_M_p_stGetAIModel(p_stMind)           ((p_stMind)->p_stAIModel)
#define M_pstGetMindOfBrain(h_Brain)            fn_p_stBrainGetMind(h_Brain)

#define AI_M_uVarIdInterpret(p_Tree)      ((p_Tree)->uParam.lValue)

long  __cdecl GAM_pc_fn_lGetSaveSlotId(long lSlotId);
char *__cdecl GAM_pc_fn_p_szGetSaveSlotNameScore(long lSlotId);
char *__cdecl GAM_pc_fn_szFormatSaveSlotDate(long lSlotId);
int   __cdecl GAM_pc_fn_lEnsureDefaultSaveSlot(void);

typedef unsigned short (__cdecl *AI_tdp_TFMT_wPositionModules)(
    void *p_stStatus, HIE_tdstSuperObject *p_stPerso, char *pszString,
    long lStart, long lEnd, MTH_tdxReal xScreenWidth,
    void *d_stLineResults, short wNbLines);                            /* 0x00408A00 */
short __cdecl TFMT_wPositionModulesAccordingToText(
    void *p_stStatus, HIE_tdstSuperObject *p_stPerso, char *pszString,
    long lStart, long lEnd, MTH_tdxReal xScreenWidth,
    void *d_stLineResults, short wNbLines);

void __cdecl fn_vMSLightSetOnOff(void *h_MSLight, unsigned char bOnOff);
void __cdecl fn_vMSLightSetTypeOfLight(void *h_MSLight, long lTypeOfLight);
void __cdecl fn_vMSLightSetColorRGBA(void *h_MSLight, float xR, float xG, float xB, float xA);
void __cdecl fn_vMSLightSetNearFar(void *h_MSLight, float xNear, float xFar);
void __cdecl fn_vMSLightSetLittleAlpha(void *h_MSLight, float xAlpha);
void __cdecl fn_vMSLightSetBigAlpha(void *h_MSLight, float xAlpha);
void __cdecl fn_vMSLightSetGiroPhare(void *h_MSLight, unsigned char bOnOff, float xStep);
void __cdecl fn_vMSLightSetPulse(void *h_MSLight, unsigned char bOnOff, float xStep, float xMaxRange);

long __cdecl SND_fn_lSendRequestSound(long indice, long type, void *evt, long prio, void *proc);

unsigned char __cdecl fn_ucSetDsgVar(unsigned char ucId, unsigned char ucIndex,
                                     AI_tdstMind *p_stMind, AI_tdstGetSetParam *p_stGetSetParam);
struct IPT_tdstEntryElement_;
char *__cdecl IPT_fn_szGetEntryActionName(struct IPT_tdstEntryElement_ *_hEntryElement);

unsigned char __cdecl IPT_fn_bIsValidated(struct IPT_tdstEntryElement_ *_hEntryElement);
float __cdecl IPT_fn_lGetAnalogicValue(struct IPT_tdstEntryElement_ *_hEntryElement);

#define AI_RT_pc_lGetSlotId            GAM_pc_fn_lGetSaveSlotId
#define AI_RT_pc_szGetSlotString       GAM_pc_fn_p_szGetSaveSlotNameScore
#define AI_RT_pc_szFormatSlotDate      GAM_pc_fn_szFormatSaveSlotDate
#define AI_RT_pc_vEnsureDefaultSlot    GAM_pc_fn_lEnsureDefaultSaveSlot
#define AI_RT_TFMT_wPositionModulesAccordingToText \
    ((AI_tdp_TFMT_wPositionModules)TFMT_wPositionModulesAccordingToText)
#define AI_RT_fn_ucSetDsgVar           fn_ucSetDsgVar
#define AI_RT_IPT_fn_szGetEntryActionName(h) \
    IPT_fn_szGetEntryActionName((struct IPT_tdstEntryElement_ *)(h))
#define AI_RT_IPT_fn_bIsValidated(h) \
    IPT_fn_bIsValidated((struct IPT_tdstEntryElement_ *)(h))
#define AI_RT_IPT_fn_lGetAnalogicValue(h) \
    IPT_fn_lGetAnalogicValue((struct IPT_tdstEntryElement_ *)(h))

extern char AI_pc_szSlotScoreBuf[16];
extern char AI_pc_szSlotDateBuf[16];
extern char AI_pc_szSlotNameBuf[8];

AI_tdstNodeInterpret *__cdecl fn_p_stNamesFunc(
    HIE_tdstSuperObject *p_SuperObjPerso,
    AI_tdstNodeInterpret *p_stTree,
    AI_tdstGetSetParam *p_stValue);
