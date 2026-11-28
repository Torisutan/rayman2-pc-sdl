#include "AI/AI_types.h"
#include "AI/Brain.h"
#include "GMT/GMT_types.h"
#include "GAM/Actions/CollSet.h"
#include "ERM/CpaError.h"
#include <ctype.h>
#include <math.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "MMG/MMG_globals.h"
#include "AI/AI_globals.h"
#include "AI/GSParam.h"
#include "GAM/GAM_types.h"
#include "GAM/PlayAnim.h"
#include "GAM/cineinfo.h"
#include "GAM/ToolCam.h"
#include "AI/CAM_Tool.h"
#include "GAM/TYPECAM.H"
#include "HIE/HIE_types.h"
#include "GAM/MSMag.h"
#include "GAM/MSWay.h"
#include "GAM/MSPrtSrc.h"
#include "GAM/effect.h"
#include "GAM/MSSound.h"
#include "GAM/HitPts.h"
#include "GAM/Caps.h"
#include "GLI/Proj.h"
#include "GLI/light_fog.h"
#include "GLI/GLI_globals.h"
#include "GAM/GAM_globals.h"
#include "MEC/MECPfm.h"
#include "AI/WPgraphe.h"
#include "GAM/MSlight.h"
#include "AI/CAM_Tool.h"
#include "GEO/GeoObj.h"
#include "MEC/MEC.h"
#include "MEC/MECTools.h"
#include "GMT/GmtFunct.h"
#include "SCT/Sector.h"
#include "MTH/MTH_xNeg.h"

extern void __cdecl fn_vInitActionTable(AI_tdstIntelligence *p_stIntelligence);

extern void __cdecl MEC_vTurnAroundZAxis(DNM_tdstDynamics *p_stDynamic, MTH_tdxReal xAngle);
extern void __cdecl MEC_vTurnAbsolute(DNM_tdstDynamics *p_stDynamic, MTH3D_tdstVector *p_stVector);
/* 0x00480980 */
extern AI_tdstNodeInterpret *__cdecl fn_p_stSwapNextParameter2(AI_tdstNodeInterpret *p_stTree);

extern void __cdecl fn_vSaveEngineClock(void);
extern void __cdecl fn_vLoadEngineClock(void);

#include "GAM/cineinfo.h"

extern void          __cdecl IPT_fn_vActivateEntryElement(void *hEntryElement);
extern void          __cdecl IPT_fn_vDesactivateEntryElement(void *hEntryElement);
extern unsigned char __cdecl IPT_fn_bModifyIPTAction(void *hElementToChange, void *hReplacingElement);
extern void __cdecl fn_vSetProcedureActionReturn(unsigned char ucActionReturn);
extern void *__cdecl fn_hGetStateMechanicsIdCard(tdxHandleToState h_State);
extern void __cdecl fn_vSetHitPointsToInitValue(void *p_stObject);
extern void __cdecl fn_vSetHitPointsMaxToInitValue(void *p_stObject);
extern unsigned long __cdecl fn_ulStandardGameGetCustomBitsSO(void *hSupObj);
extern void __cdecl ANI_fn_vFreeze(HIE_tdstSuperObject *hSuperObjPerso);
extern void __cdecl ANI_fn_vUnFreeze(HIE_tdstSuperObject *hSuperObjPerso);
extern char __cdecl fn_bStandardGameIsAPlatform(void *hSupObj);
extern void *__cdecl fn_p_stGetPersoInList(void *p_List, unsigned char ucPosition);

AI_tdstNodeInterpret *__cdecl fn_p_stSetHitPoints( HIE_tdstSuperObject *p_SuperObjPerso, AI_tdstNodeInterpret *p_stTree, void *p_stObject);

#define AI_M_pfn_fn_p_stSetHitPoints           fn_p_stSetHitPoints

AI_tdstNodeInterpret *__cdecl fn_p_stAddHitPoints( HIE_tdstSuperObject *p_SuperObjPerso, AI_tdstNodeInterpret *p_stTree, void *p_stObject);
#define AI_M_pfn_fn_p_stAddHitPoints           fn_p_stAddHitPoints

AI_tdstNodeInterpret *__cdecl fn_p_stSubHitPoints( HIE_tdstSuperObject *p_SuperObjPerso, AI_tdstNodeInterpret *p_stTree, void *p_stObject);
#define AI_M_pfn_fn_p_stSubHitPoints           fn_p_stSubHitPoints
#define AI_M_pfn_fn_vSetHitPointsInit          ((void (__cdecl *)(void *, unsigned char))fn_vSetHitPointsInit)
#define AI_M_pfn_fn_vSetHitPointsToInitValue   ((void (__cdecl *)(void *))fn_vSetHitPointsToInitValue)
#define AI_M_pfn_fn_vSetHitPointsToMaxValue    ((void (__cdecl *)(void *))fn_vSetHitPointsToMaxValue)

AI_tdstNodeInterpret *__cdecl fn_p_stSetHitPointsMax( HIE_tdstSuperObject *p_SuperObjPerso, AI_tdstNodeInterpret *p_stTree, void *p_stObject);

#define AI_M_pfn_fn_p_stSetHitPointsMax        fn_p_stSetHitPointsMax

AI_tdstNodeInterpret *__cdecl fn_p_stAddHitPointsMax( HIE_tdstSuperObject *p_SuperObjPerso, AI_tdstNodeInterpret *p_stTree, void *p_stObject);
#define AI_M_pfn_fn_p_stAddHitPointsMax        fn_p_stAddHitPointsMax

AI_tdstNodeInterpret *__cdecl fn_p_stSubHitPointsMax( HIE_tdstSuperObject *p_SuperObjPerso, AI_tdstNodeInterpret *p_stTree, void *p_stObject);
#define AI_M_pfn_fn_p_stSubHitPointsMax        fn_p_stSubHitPointsMax
#define AI_M_pfn_fn_vSetHitPointsMaxToInit     ((void (__cdecl *)(void *))fn_vSetHitPointsMaxToInitValue)
#define AI_M_pfn_fn_vSetHitPointsMaxToMax      ((void (__cdecl *)(void *))fn_vSetHitPointsMaxToMaxValue)

#define AI_M_pfn_fn_h_StandardGameGetSuperObject  ((HIE_tdstSuperObject *(__cdecl *)(void *))&fn_h_StandardGameGetSuperObject)

/* 0x00470210 */
extern void __cdecl fn_vOnAIErm_M_UpdateLastError(unsigned short FatalErrorNum, char *szMsg);
#if defined(LINUX) || defined(OSX) || defined(PSP) || defined(WIIU)
/* ShellExecute: Windows only (the procedure table takes fn_p_stNoneProcedure, AI/ConvProc.c) */
#else

__declspec(dllimport) void *__stdcall ShellExecuteA(void *hWnd, const char *lpOperation, const char *lpFile,
                                                    const char *lpParameters, const char *lpDirectory, int nShowCmd);
#endif /* LINUX || OSX || PSP || WIIU */

/* 0x00483740 */
extern void __cdecl fn_vAffectZddInList(void *p_List, void *p_stPerso, unsigned char ucModulOrCharact,
                                        ACP_tdxIndex xZoneId);
#define AI_M_pfn_fn_vAffectZddInList                 fn_vAffectZddInList
/* 0x004837A0 */
extern void __cdecl fn_vAffectZdeInList();
extern void __cdecl fn_vAffectTypeZdeWithTypeZdeInList();
#define AI_M_pfn_fn_vAffectZdeInList                 ((void (__cdecl *)(void *, HIE_tdstSuperObject *, int, long, unsigned short))fn_vAffectZdeInList)
#define AI_M_pfn_fn_vAffectTypeZdeWithTypeZdeInList  ((void (__cdecl *)(void *, HIE_tdstSuperObject *, unsigned short, unsigned short))fn_vAffectTypeZdeWithTypeZdeInList)
/* 0x0042DE20 */
extern void __cdecl GMT_fn_vResetEntireMask(unsigned short *_p_xMask);
#define AI_M_pfn_GMT_fn_vResetEntireMask             ((void (__cdecl *)(unsigned short *))&GMT_fn_vResetEntireMask)

/* 0x00483120 */
extern void __cdecl fn_vPutInList(void *p_List, void *p_Perso);
#define AI_M_pfn_fn_vPutInList                   ((void (__cdecl *)(void *, void *))&fn_vPutInList)
/* 0x483150 */
extern void __cdecl fn_vInsertPersoAtPositionInList(void *p_List, void *p_Perso, unsigned char ucPosition);
extern char __cdecl fn_bFindPersoAndDeleteInList(void *p_List, void *p_Perso);
#define AI_M_pfn_fn_vInsertPersoAtPositionInList ((void (__cdecl *)(void *, void *, long))&fn_vInsertPersoAtPositionInList)

extern void __cdecl fn_vDeletePersoAtPositionInList(void *p_List, unsigned char ucPosition);
#define AI_M_pfn_fn_vDeletePersoAtPositionInList fn_vDeletePersoAtPositionInList
#define AI_M_pfn_fn_bFindPersoAndDeleteInList    ((int (__cdecl *)(void *, void *))&fn_bFindPersoAndDeleteInList)

extern AI_tdstNodeInterpret *__cdecl fn_p_stListSelect(void *p_List, char b_cInverse, AI_tdstNodeInterpret *p_stTree);
#define AI_M_pfn_fn_p_stListSelect               fn_p_stListSelect
/* 0x004839D0 */
extern AI_tdstNodeInterpret *__cdecl fn_p_stListSort(void *p_List, unsigned char ucWantedNbElt, char b_cIncrease, AI_tdstNodeInterpret *p_stTree);
#define AI_M_pfn_fn_p_stListSort                 fn_p_stListSort
/* 0x4833D0 */
extern void __cdecl fn_vSortListByFamily(void *p_List, void *hFamily);
#define AI_M_pfn_fn_vSortListByFamily            ((void (__cdecl *)(void *, void *))&fn_vSortListByFamily)
/* 0x483460 */
extern void __cdecl fn_vSortListByModel(void *p_stList, void *_p_stModel);
#define AI_M_pfn_fn_vSortListByModel             ((void (__cdecl *)(void *, void *))&fn_vSortListByModel)
#define AI_M_pfn_fn_p_stGetPersoInList2          ((HIE_tdstSuperObject *(__cdecl *)(void *, unsigned char))fn_p_stGetPersoInList)

#include "AI/AI_Struc.h"
#include "AI/List.h"
#include "HIE/HieSpObj.h"

static __inline struct HIE_tdstSuperObject *HIE_fn_hGetSuperObjectNextBrother(struct HIE_tdstSuperObject *_hSprObj)
{
    return HIE_M_hNextChild(_hSprObj);
}

static __inline struct HIE_tdstSuperObject *HIE_fn_hGetSuperObjectFirstChild(struct HIE_tdstSuperObject *_hSprObj)
{
    return HIE_M_hFirstChild(_hSprObj);
}

#define HIE_C_InvalidSuperObject ((HIE_tdstSuperObject *)0)
#define HIE_fn_bIsSuperObjectValid(hSuperObject) ((hSuperObject)!=HIE_C_InvalidSuperObject)

static __inline void *fn_h3dDataGetFamily(MS_tdxHandleTo3dData h_3dData)
{
    return(h_3dData->h_Family);
}

/* 0x483510 */
extern void __cdecl fn_vUnionList(void *p_ListA, void *p_ListB, void *p_ListC);
extern void __cdecl fn_vInterList(void *p_ListA, void *p_ListB, void *p_ListC);
extern void __cdecl fn_vDiffList(void *p_ListA, void *p_ListB, void *p_ListC);
extern void __cdecl fn_vAddList(void *p_ListA, void *p_ListB);
#define AI_M_pfn_fn_vUnionList ((void (__cdecl *)(void *, void *, void *))&fn_vUnionList)
#define AI_M_pfn_fn_vInterList ((void (__cdecl *)(void *, void *, void *))&fn_vInterList)
#define AI_M_pfn_fn_vDiffList  ((void (__cdecl *)(void *, void *, void *))&fn_vDiffList)
#define AI_M_pfn_fn_vAddList   ((void (__cdecl *)(void *, void *))&fn_vAddList)

#include "AI/AITools.h"
#include "PRT/PRT_types.h"

#define AI_GLI_cGlobalForceFogIsOn  (*(char  *)GLI_g_cGlobalFogIsOn_ADDR)

float AI_GLI_a_xFogColor[4];   /* 0x00504000 */

float AI_GLI_a_xSavedFogColor[4];   /* 0x00503D94 */

long  AI_TEXT_gai_IntParameters[10];   /* 0x004E9620 */
float AI_TEXT_gax_RealParameters[10];  /* 0x004E9500 */

#define AI_PROC_xReadReal(v)  ((v)->eType == 8 ? (v)->uParam.xValue : (float)(v)->uParam.lValue)

extern AI_tdstProcedureEntry a_stProcedureTable[];   /* 0x004A43E0 */
#define AI_M_ucGetProcedureUltra(procId)   (a_stProcedureTable[procId].bUltraProcedureFlag)

#define AI_g_p_stUltraOperatorPerso  g_hCurrentSuperObjPerso

#define AI_MGT_hMagnet(perso)  M_GetMSHandle((perso), MSMagnet)

#define AI_MAGNET_ON           1
#define AI_MAGNET_GROWING      2
#define AI_MAGNET_RECOVERING   3

/* 0x00440B50 */
extern void __cdecl MGT_fn_vAddObjectToMagnetList(HIE_tdstSuperObject *p_stObject);
#define AI_M_pfn_MSMagnetGetStatus         ((unsigned char (__cdecl *)(void *))fn_lMSMagnetGetStatus)
#define AI_M_pfn_MSMagnetSetStatus         ((void (__cdecl *)(void *, unsigned char))fn_vMSMagnetSetStatus)
#define AI_M_pfn_MSMagnetSetStrength       ((void (__cdecl *)(void *, float))fn_vMSMagnetSetStrength)
#define AI_M_pfn_MSMagnetSetFar            ((void (__cdecl *)(void *, float))fn_vMSMagnetSetFar)
#define AI_M_pfn_MSMagnetSetNear           ((void (__cdecl *)(void *, float))fn_vMSMagnetSetNear)
#define AI_M_pfn_MSMagnetSetDuration       ((void (__cdecl *)(void *, unsigned long))fn_vMSMagnetSetDuration)

extern void __cdecl fn_vOnAIErm_M_UpdateLastError(unsigned short FatalErrorNum, char *szMsg);
#define AI_M_pfn_M_AIFatalError(uwError) \
    fn_vOnAIErm_M_UpdateLastError((unsigned short)(uwError), (char *)0)

/* 0x004825B0 */
static HIE_tdstSuperObject *AI_p_stResolveUltraPerso(
    unsigned char ucUltra, HIE_tdstSuperObject *p_SuperObjPerso)
{
    HIE_tdstSuperObject *p_stResult = p_SuperObjPerso;
    if (AI_g_p_stUltraOperatorPerso != NULL && ucUltra != 0)
    {
        p_stResult = AI_g_p_stUltraOperatorPerso;
        AI_g_p_stUltraOperatorPerso = NULL;
    }
    return p_stResult;
}

#define AI_PRT_hMSPrtSrc(perso)  M_GetMSHandle((perso), MSPrtSrc)
/* 0x0042DB30 */
extern void *__cdecl fn_hMSPrtSrcGetFootPath(void *h_MSPrtSrc);
#define AI_M_pfn_MSPrtSrcGetFootPath   ((void *(__cdecl *)(void *))&fn_hMSPrtSrcGetFootPath)

static const unsigned long AI_GLI_x0_1_bits = 0x3DCCCCCDu;
#define AI_GLI_x0_1  (*(const float *)&AI_GLI_x0_1_bits)

extern void __cdecl GLI_fn_SinusEffect_vSetState(unsigned char v);   /* 0x00423C00 */
/* 0x00423C50 */
extern void __cdecl GLI_fn_SinusEffect_vSetFreq(long _lXFreq, long _lYFreq, long _lZFreq);
extern void __cdecl GLI_fn_SinusEffect_vSetAmplitude(void *_p_stAmpVector);
#define AI_M_pfn_SinusEffect_SetFreq      ((void (__cdecl *)(long, long, long))&GLI_fn_SinusEffect_vSetFreq)
#define AI_M_pfn_SinusEffect_SetAmplitude ((void (__cdecl *)(float *))&GLI_fn_SinusEffect_vSetAmplitude)
/* 0x00423C30 */
extern char __cdecl GLI_fn_SinusEffect_vGetFlag(void);
extern void __cdecl GLI_fn_SinusEffect_vSetFlag(char _cFlag);
#define AI_M_pfn_SinusEffect_GetFlag      ((unsigned char (__cdecl *)(void))&GLI_fn_SinusEffect_vGetFlag)
#define AI_M_pfn_SinusEffect_SetFlag      ((void (__cdecl *)(unsigned char))&GLI_fn_SinusEffect_vSetFlag)
#define AI_M_pfn_SinusEffect_SetState(v)  GLI_fn_SinusEffect_vSetState((unsigned char)(v))
/* 0x00423E80 */
extern void __cdecl GLI_vSetSinusEffectOnRLIBase(long xRed, long xGreen, long xBlue);
extern void __cdecl GLI_vSetSinusEffectOnRLIAmp(long xRed, long xGreen, long xBlue);
#define AI_M_pfn_SinusEffect_SetRLIBase   ((void (__cdecl *)(float, float, float))&GLI_vSetSinusEffectOnRLIBase)
#define AI_M_pfn_SinusEffect_SetRLIAmp    ((void (__cdecl *)(float, float, float))&GLI_vSetSinusEffectOnRLIAmp)
#define AI_C_SinusEffect_cAffectSecondViewPort  1
#define AI_C_SinusEffect_cUseZFactor            2

#define AI_g_ulEngineDisplayMode  (*(unsigned long *)(GAM_ENGINE_BASE + 0x60))

#define AI_M_pfn_IPT_DesactivateEntryElement ((void (__cdecl *)(long))IPT_fn_vDesactivateEntryElement)
#define AI_M_pfn_IPT_ActivateEntryElement    ((void (__cdecl *)(long))IPT_fn_vActivateEntryElement)

#define AI_M_p_stMindRaw(obj)  AI_M_stGetMindOfSuperObj(obj)

#define AI_M_pfn_fn_vSetProcedureActionReturn ((void  (__cdecl *)(long))fn_vSetProcedureActionReturn)

#define AI_C_ACTION_CHANGE_COMPORT            8

extern ACP_tdxBool __cdecl PLA_fn_bSetNewState(HIE_tdstSuperObject *p_stSuperObject,
                                               tdxHandleToState h_WantedState, ACP_tdxBool _bForce,
                                               ACP_tdxBool _bHandleSkippedEventsIfRelevant,
                                               ACP_tdxBool _bPcSkipTransition);  /* 0x0040FAA0 */
extern ACP_tdxBool __cdecl PLA_fn_bSetNewStateWithRandom(HIE_tdstSuperObject *p_stSuperObject, void *h_WantedState);
#define AI_M_pfn_PLA_bSetNewStateWithRandom PLA_fn_bSetNewStateWithRandom
/* 0x00419B90 */
extern void *__cdecl fn_p_stDynamGetDynamics(void *h_Dynam);
#define AI_M_pfn_fn_p_stDynamGetDynamics    ((void *(__cdecl *)(void *))&fn_p_stDynamGetDynamics)
#define AI_M_pfn_fn_hGetStateMechanicsIdCard ((void *(__cdecl *)(void *))fn_hGetStateMechanicsIdCard)
#define AI_C_ACTION_ENGINE_STOP_RULE        2

AI_tdstNodeInterpret *AI_g_a_p_stLoopInit[50];
long                  AI_g_a_lLoopCpt[50];
AI_tdstNodeInterpret *AI_g_a_p_stLoopTree[50];

extern void __cdecl fn_vSnifThePlayerIsDead(void);
#define AI_M_pfn_fn_vSnifThePlayerIsDead   fn_vSnifThePlayerIsDead
/* 0x412040 */
extern void *__cdecl fn_h_GetCurrentDNMIdCard(void *p_SuperObjPerso);
#define AI_M_pfn_fn_h_GetCurrentDNMIdCard  fn_h_GetCurrentDNMIdCard

/* 0x00405FA0 */
extern void __cdecl fn_vForceActiveAnObject(struct tdstEngineObject_ *p_stObject);
/* 0x004061B0 */
extern void __cdecl fn_vDesactiveAnObject(struct tdstEngineObject_ *p_stObject);
#define AI_M_pfn_fn_vDesactiveAnObject     fn_vDesactiveAnObject

#define AI_M_pfn_fn_szGetStringFromParam   fn_szGetStringFromTextOrStringParam

extern void __cdecl GAM_fn_vAskToChangeLevel(char *szNewLevelName, ACP_tdxBool _bAskForAutosave);
#define AI_M_pfn_GAM_vAskToChangeLevel     ((void  (__cdecl *)(char *, char))&GAM_fn_vAskToChangeLevel)
#define AI_M_pfn_HIE_vInvalidateHierarchy  HIE_fn_vInvalidateHierarchy
#define AI_M_pfn_MEC_vFreePlatformLink     MEC_fn_vFreePlatformLink

#define AI_M_pfn_fn_vReputCharacterSOAtWorld ((void (__cdecl *)(void *))&fn_vReputCharacterSuperObjectAtTheWorld)
#define AI_M_pfn_HIE_vChangeFather         HIE_fn_vChangeFather
#define AI_M_pfn_fn_vSaveEngineClock       fn_vSaveEngineClock
#define AI_M_pfn_fn_vLoadEngineClock       fn_vLoadEngineClock

/* 0x4187E0 */
extern void __cdecl fn_v3dDataFactorFrameRate(void *_h3dData, unsigned long _ulFactor100);
#define AI_M_pfn_fn_v3dDataFactorFrameRate  fn_v3dDataFactorFrameRate
/* 0x417950 */
extern void __cdecl CS_fn_vSetCharacterPriority(void *hCollSet, unsigned char ucPriority);
#define AI_M_pfn_CS_vSetCharacterPriority  CS_fn_vSetCharacterPriority

extern void __cdecl fn_vSnifThePlayerIsDeadWithResurrectionAt(HIE_tdstSuperObject *_hCharacterPositionOne, HIE_tdstSuperObject *_hCharacterPositionTwo);
#define AI_M_pfn_fn_vSnifPlayerDeadWithResurrection fn_vSnifThePlayerIsDeadWithResurrectionAt

extern void __cdecl fn_vExchangeTwoObjectsInCurrentObjectsTable(HIE_tdstSuperObject *p_stSuperObject, unsigned short w_ObjectA, unsigned short w_ObjectB);

extern unsigned short __cdecl fn_uwGetObjectTableNumberOfelements(tdxHandleToObjectsTablesList _hTargetTable);
#define AI_M_pfn_fn_uwGetObjectTableNb ((short (__cdecl *)(void *))fn_uwGetObjectTableNumberOfelements)
extern void __cdecl fn_vCopyObjectFromObjectTabletoAnother(tdxHandleToObjectsTablesList _hTargetTable, tdxHandleToObjectsTablesList _hSourceTable,unsigned short _uwTargetObject, unsigned short _uwSourceObject);
#define AI_M_pfn_fn_vCopyObjectInTable fn_vCopyObjectFromObjectTabletoAnother

extern void __cdecl fn_vTakeControlOfChannel(struct tdstEngineObject_ *_hActor, unsigned short _uwChannel, ACP_tdxBool _bControlled);
#define AI_M_pfn_fn_vTakeControlOfChannel fn_vTakeControlOfChannel

extern void __cdecl fn_vChangeEngineMode(unsigned char ucMode);
#define AI_C_EM_ModeStoppingProgram  2

extern void __cdecl fn_vActivateChannel(HIE_tdstSuperObject *_hPerso, unsigned short _uwChannel, ACP_tdxBool _bActivate);
#define AI_M_pfn_fn_vActivateChannel fn_vActivateChannel

#define AI_g_lSoundObjectTypeMenu   GAM_g_lSoundObjectTypeMenu
#define AI_SND_C_OBJET_FANTOME      (-1)

/* 0x00472450 */
static __inline HIE_tdstSuperObject *fn_hGetSuperObjectInChannel(CHN_tdxHandleToChannelArray hChannelArray, unsigned short uwChannelNumber)
{
	return (hChannelArray) ? hChannelArray[uwChannelNumber].hSupObject : NULL;
}

static __inline void fn_v3dDataSetDrawMask(MS_tdxHandleTo3dData _h3dData,long _lDrawMask)
{
	_h3dData->lDrawMask = _lDrawMask;
}
/* 0x0040F260 */
extern void __cdecl PLA_fn_vUpdateTransparencyForModules(HIE_tdstSuperObject *p_stSuperObject);

extern void fn_vAskToDisplayVignette(char *szName);

static const unsigned long AI_xDegToRad_bits = 0x3C8EFA35u;
static const unsigned long AI_x0_001_bits    = 0x3A83126Fu;
#define AI_xDegToRad            (*(const float *)&AI_xDegToRad_bits)
#define AI_x0_001               (*(const float *)&AI_x0_001_bits)

#define AI_g_ulUsefulDeltaTime  (*(unsigned long *)(GAM_ENGINE_BASE + 0xB4))

#define AI_M_pfn_fn_vKillEngineObject(o)  fn_vKillEngineObjectOrAlwaysByPointer((struct tdstEngineObject_ *)(o))

#define AI_M_pfn_fn_p_stEvalNextVarIdPerso(a, b, c) \
    fn_p_stOperatorGetPerso((a), (b), (HIE_tdstSuperObject **)(c))
#define AI_C_VALID_GET  1
unsigned char __cdecl fn_ucGetDsgVar(unsigned char ucId, unsigned char ucIndex, void *p_stMind, void *p_stGetSetParam);

#define AI_M_pfn_GMT_fn_hGetVisualMaterial  GMT_fn_hGetVisualMaterial
/* 0x004249E0 */
extern void __cdecl GLI_xSetMaterialTextureScrollingCoef(void *hMaterial, long lNumberOfTexture, long bSwitch, float fUSpeed, float fVSpeed);
#define AI_M_pfn_GLI_xSetMaterialTextureScrollingCoef GLI_xSetMaterialTextureScrollingCoef

extern void __cdecl GLI_xSetMaterialColor(void *hMaterial, void *p_stColor);
extern void __cdecl GLI_xSetMaterialDiffuseCoef(void *hMaterial, void *p_stColor);
extern void __cdecl GLI_xSetMaterialAmbientCoef(void *hMaterial, void *p_stColor);
extern void __cdecl GLI_xSetMaterialSpecularCoef(void *hMaterial, long lExp, void *p_stColor);
extern void __cdecl GLI_xGetMaterialSpecularCoef(void *hMaterial, long *p_lExp, void *p_stColor);
extern void __cdecl GLI_vSetCurrentAnimatedTextureNumber(void *hMaterial, long lTextureNumber);
extern void __cdecl GLI_vLockAnimatedTexture(void *hMaterial);
extern void __cdecl GLI_vUnLockAnimatedTexture(void *hMaterial);
extern void __cdecl GLI_vSetMaterialAsChromed(void *hMaterial, long bChromed);  /* 0x4249e0 */

/* 0x00472FA0 */
extern void AGO_vAddParticle(unsigned long PartType, MTH3D_tdstVector *pPos,
                             MTH3D_tdstVector *pDir, void *hVisualMaterial, float parameter);
#define AI_M_pfn_AGO_vAddParticle AGO_vAddParticle

extern void __cdecl fn_vChangeDetailsValue(unsigned long ulValue);
extern void __cdecl fn_vChangeMusicVolumeValue(unsigned long ulValue);
extern void __cdecl fn_vChangeFxVolumeValue(unsigned long ulValue);
#define AI_M_pfn_fn_vChangeDetailsValue     fn_vChangeDetailsValue
#define AI_M_pfn_fn_vChangeMusicVolumeValue fn_vChangeMusicVolumeValue
#define AI_M_pfn_fn_vChangeFxVolumeValue    fn_vChangeFxVolumeValue
extern unsigned char __cdecl IPT_fn_bChangeIPTValue(struct IPT_tdstEntryElement_ *_hElement, unsigned char ucWhichTest);
#define AI_M_pfn_IPT_fn_bChangeIPTValue     ((char (__cdecl *)(void *, long))IPT_fn_bChangeIPTValue)
#define AI_M_pfn_IPT_fn_bModifyIPTAction    ((char (__cdecl *)(void *, void *))IPT_fn_bModifyIPTAction)
#define AI_IPT_C_Exchange                   1L

#define AI_M_pfn_CAM_fn_hCineInfoGetSuperObjectTargeted ((void *(__cdecl *)(void *))CAM_fn_hCineInfoGetSuperObjectTargeted)

#define AI_M_g_h_StdCamCaracter    (GAM_g_stEngineStructure.h_StdCamCaracter)
#define AI_C_ucNbOfCheckedSpeeds   8

/* 0x432D30 */
extern void __cdecl MEC_vImposeTranslation(DNM_tdstDynamics *p_stDynamic, MTH3D_tdstVector *p_stVector);
extern void __cdecl MEC_vImposeRotationMatrix(DNM_tdstDynamics *p_stDynamic, POS_tdxHandleToPosition hRotationMatrix);
#define AI_M_pfn_MEC_vImposeRotationMatrix   ((void (__cdecl *)(void *, POS_tdstCompletePosition *))&MEC_vImposeRotationMatrix)
#define AI_M_pfn_HIE_vComputeNewRelativeMatrix HIE_fn_vComputeNewRelativeMatrix
#define AI_M_pfn_HIE_vRefreshHierarchy       HIE_fn_vRefreshHierarchy
#define AI_M_3D_pfn_DynamGetDynamics         ((void *(__cdecl *)(void *))&fn_p_stDynamGetDynamics)

#define AI_3D_xDegToRad  AI_xDegToRad   /* 0x0049CBF0 */
#define AI_3D_xEpsilon   AI_x0_001      /* 0x0049CBF4 */
static const unsigned long AI_3D_xOne_bits       = 0x3F800000u;
#define AI_3D_xOne       (*(const float *)&AI_3D_xOne_bits)

#define AI_3D_M_pGlobalMatrix(so)  HIE_fn_hGetSuperObjectGlobalMatrix((so))
#define AI_3D_M_pEngineObj(so)     HIE_fn_hGetSuperObjectObject((so))
#define AI_3D_M_hDynam(so)         (((struct tdstEngineObject_ *)AI_3D_M_pEngineObj(so))->h_Dynam)

static __inline ACP_tdxBool HIE_fn_SO_bHasNoTransformationMatrix( struct HIE_tdstSuperObject *_hSO )
{
    return (ACP_tdxBool) (_hSO->ulFlags & HIE_C_Flag_ulNoTransformationMatrix);
}

static __inline void fn_v3dDataSetAnimMatrixHasChanged(MS_tdxHandleTo3dData h_3dData)
{
	h_3dData->ucAnimMatrixHasChanged = TRUE;
}

#include "AI/GSParam.h"

#define AI_M_pfn_MEC_vImposeRotationVector MEC_vImposeRotationVector

/* 0x432da0 */
extern void __cdecl MEC_vInitTranslation(DNM_tdstDynamics *p_stDynamic, HIE_tdstSuperObject *hSupObj, MTH3D_tdstVector *p_stVector);
#define AI_M_pfn_MEC_vInitTranslation \
    ((void (__cdecl *)(void *, void *, float *))&MEC_vInitTranslation)

extern void *__cdecl fn_hMSLightGetLightNodeInSector(void *_hMSLight);   /* 0x00415FE0 */
extern void  __cdecl SECT_fn_vIsolateCharListNode(void *_hNode);         /* 0x0044C070 */
extern HIE_tdstSuperObject *__cdecl GAM_fn_hGetCurrentSector(HIE_tdstSuperObject *_hCharacter);
#define AI_M_pfn_GAM_fn_hGetCurrentSector GAM_fn_hGetCurrentSector

#define AI_M_pfn_SECT_fn_vAddTailDNMLightsListNode SECT_fn_vAddTailDNMLightsListNode

#define AI_M_pfn_fn_vMSLightSetLocalLight fn_vMSLightSetLocalLight
extern void __cdecl fn_vMSLightSetOnlyLocalLight(void *h, unsigned char bOnlyLocalLight);
#define AI_M_pfn_fn_vMSLightSetOnlyLocalLight fn_vMSLightSetOnlyLocalLight

/* 0x004731B0 */
extern void __cdecl JFFTXT_vAddText(int StringNumber, MTH3D_tdstVector *p_stPosition, unsigned char *pszString, MTH_tdxReal xParameter);
#define AI_M_pfn_JFFTXT_vAddText(n, p, s, x) JFFTXT_vAddText((int)(n), (MTH3D_tdstVector *)(p), (unsigned char *)(s), (MTH_tdxReal)(x))
extern void __cdecl JFFTXT_vExtendText(int StringNumber, int ID, MTH_tdxReal xParameter);
#define AI_M_pfn_JFFTXT_vExtendText(n, id, x) JFFTXT_vExtendText((int)(n), (int)(id), (MTH_tdxReal)(x))
extern void __cdecl JFFTXT_vModifyCaracter(int StringNumber, int Position, unsigned char newcar);
#define AI_M_pfn_JFFTXT_vModifyCaracter JFFTXT_vModifyCaracter

static long AI_PAD_lCheat1Enable_storage;   /* 0x4A708C */
static long AI_PAD_lCheat2Enable_storage;   /* 0x4A7088 */

#define AI_M_pfn_fn_vSetCapabilities    ((void (__cdecl *)(void *, unsigned long))fn_vSetCapabilities)
#define AI_M_pfn_fn_vAddCapabilities    ((void (__cdecl *)(void *, unsigned long))fn_vAddCapabilities)
#define AI_M_pfn_fn_vSubCapabilities    ((void (__cdecl *)(void *, unsigned long))fn_vSubCapabilities)
#define AI_M_pfn_fn_vChangeCapabilities ((void (__cdecl *)(void *, unsigned long, long))fn_vChangeCapabilities)

/* 0x004720A0 */
extern void __cdecl GLI_vSetSinEffetBase(long xBase);
#define AI_M_pfn_GLI_vSetSinEffetBase  ((void (__cdecl *)(float))&GLI_vSetSinEffetBase)

/* 0x004720E0 */
extern void __cdecl GLI_vSaveSinusContext(void);
extern void __cdecl GLI_vRestoreSinusContext(void);
#define AI_M_pfn_GLI_vSaveSinusContext     GLI_vSaveSinusContext
#define AI_M_pfn_GLI_vRestoreSinusContext  GLI_vRestoreSinusContext

/* 0x00471D10 */
extern void __cdecl SND_fn_vChangeVolumeSound(long indice, long type, long lNewVolume);
#define AI_M_pfn_SND_fn_vChangeVolumeSound SND_fn_vChangeVolumeSound

extern void __cdecl SND_fn_vSetDopplerFactorSound(long factor);   /* 0x00452A90 */

extern void __cdecl SND_fn_vPauseSound(void);    /* 0x004536D0 */
extern void __cdecl SND_fn_vResumeSound(void);   /* 0x00453700 */

extern void __cdecl SND_fn_vKillAllObjectTypeSound(long type);   /* 0x00453080 */
#define AI_M_pfn_SND_fn_vKillAllObjectTypeSound SND_fn_vKillAllObjectTypeSound

extern long  fn_lGetSubMapNumber(void);    /* 0x00404DB0 */
extern char *fn_p_szGetLevelName(void);    /* 0x00404DA0 */

/* 0x0042E1E0 */
extern void __cdecl IPT_fn_vDesactiveJoystickAndPadControl(void);
#define AI_M_pfn_IPT_fn_vDesactiveJoystickAndPadControl IPT_fn_vDesactiveJoystickAndPadControl
/* 0x0042E220 */
extern void __cdecl IPT_fn_vActiveJoystickControl(void);
#define AI_M_pfn_IPT_fn_vActiveJoystickControl   IPT_fn_vActiveJoystickControl

#define AI_M_pfn_HIE_SO_vSetSuperimposedFlag   ((void (__cdecl *)(void *))HIE_fn_SO_vSetSuperimposedFlag)
#define AI_M_pfn_HIE_SO_vClearSuperimposedFlag ((void (__cdecl *)(void *))HIE_fn_SO_vClearSuperimposedFlag)
/* 0x412810 */
extern void __cdecl fn_vRemoveObjectInSectorList(HIE_tdstSuperObject *hCharacter);
#define AI_M_pfn_fn_vRemoveObjectInSectorList  ((void (__cdecl *)(void *))&fn_vRemoveObjectInSectorList)
/* 0x00412740 */
extern void __cdecl fn_vInsertObjectInSectorList(HIE_tdstSuperObject *hSector, HIE_tdstSuperObject *hCharacter);
#define AI_M_pfn_fn_vInsertObjectInSectorList  fn_vInsertObjectInSectorList
/* 0x0042DB20 */
extern void __cdecl fn_vSectInfoSetCurrentSector(void *h_SectInfo, void *hCurrentSector);
#define AI_M_pfn_fn_vSectInfoSetCurrentSector  ((void (__cdecl *)(void *, void *))&fn_vSectInfoSetCurrentSector)

#define AI_M_pfn_fn_h_SectInfoGetCurrentSector ((void *(__cdecl *)(void *))&fn_h_SectInfoGetCurrentSector)
#define AI_M_pfn_GLI_vGet3DVertexFromScreenPos GLI_vGet3DVertexFromScreenPos

#define AI_M_pfn_HIE_SO_SwitchSuperImposedTab  ((void (__cdecl *)(long))HIE_fn_SO_SwitchSuperImposedTab)

#define AI_SPO_g_p_stMainActor                 GAM_pc_g_hMainCharacterSO
#define AI_SPO_pFixViewportAttr                AI_TA_p_stFixViewportAttr  /* 0x5004D4 */
static const unsigned long AI_SPO_x0_001_bits = 0x3A83126Fu;
static const unsigned long AI_SPO_x0_01_bits  = 0x3C23D70Au;
#define AI_SPO_x0_001  (*(const float *)&AI_SPO_x0_001_bits)
#define AI_SPO_x0_01   (*(const float *)&AI_SPO_x0_01_bits)
/* 0x0049CC58 */
static const unsigned long AI_SPO_x0_001565_bits = 0x3ACCCCCDu;
#define AI_SPO_x0_001565                       (*(const float *)&AI_SPO_x0_001565_bits)
#include "GAM/Actions/SectInfo.h"
#include "GLD/GLD_Pub.h"

#define AI_SPO_dwScreenWidth                   AI_TA_dwScreenWidth

/* 0x40CC70 */
extern void __cdecl GEO_vSetRadiusOfIndexedSphere(void *, short, short, float);
extern void __cdecl GEO_vSetRadiusOfIndexedCone(void *, short, short, float);
#define AI_M_pfn_fn_hGetGeometricZdxOfTypeAtIndex \
    ((void *(__cdecl *)(unsigned char, unsigned short, void *))&fn_hGetGeometricZdxOfTypeAtIndex)
#define AI_M_pfn_GEO_vSetRadiusOfIndexedSphere \
    ((void (__cdecl *)(void *, short, short, float))&GEO_vSetRadiusOfIndexedSphere)
/* 0x0041ED20 */
extern void GEO_vGetPointsOfIndexedCone(ACP_tdxHandleOfObject hObject, ACP_tdxHandleOfElement hElement,
    ACP_tdxIndex xNumberOfCone, ACP_tdxIndex *p_xTopPoint, ACP_tdxIndex *p_xBasePoint);
#define AI_M_pfn_GEO_vGetPointsOfIndexedCone GEO_vGetPointsOfIndexedCone

#define AI_M_pfn_GEO_vGetPointOfObject  GEO_vGetPointOfObject
#define AI_M_pfn_GEO_vSetPointOfObject  GEO_vSetPointOfObject
#define AI_M_pfn_GEO_vSetRadiusOfIndexedCone \
    ((void (__cdecl *)(void *, short, short, float))&GEO_vSetRadiusOfIndexedCone)
#define AI_ZDX_xDegToRad   AI_xDegToRad   /* 0x0049CBF0 */

AI_tdstNodeInterpret *__cdecl fn_p_stCustomBitsProc( HIE_tdstSuperObject *p_SuperObjPerso, AI_tdstNodeInterpret *p_stTree);

#include "AI/AI_Struc.h"
#include "AI/DsgMem.h"
#include "AI/List.h"
#include "GAM/PlayAnim.h"
#include "AI/CAM_Base.h"
#include "HIE/HieSpObj.h"

#define PLA_C_wRandomFrame  ((short)-32768)
/* 0x0040fd30 */
void __cdecl PLA_fn_vSetCurrFrame(HIE_tdstSuperObject *p_stSuperObject, short _wFrame);
/* 0x0040fd30 */
void __cdecl PLA_fn_vSetCurrFrame(HIE_tdstSuperObject *p_stSuperObject, short _wFrame);
#define AI_M_pfn_PLA_fn_vSetCurrFrame PLA_fn_vSetCurrFrame

AI_tdstNodeInterpret *__cdecl fn_p_stVisualGameMaterialProcedure( HIE_tdstSuperObject *p_SuperObjPerso, AI_tdstNodeInterpret *p_stTree);

/* 0x00470DA0 */
extern void __cdecl GLI_xSetMaterialTextureDecalage(void *hMaterial, long lNumberOfTexture, long bSwitch, float fUSpeed, float fVSpeed);
#define AI_M_pfn_GLI_xSetMaterialTextureDecalage GLI_xSetMaterialTextureDecalage
/* 0x00424B10 */
extern void __cdecl GLI_xSetMaterialTextureScrollingOnOff(void *hMaterial, long bSwitch);
#define AI_M_pfn_GLI_xSetMaterialTextureScrollingOnOff GLI_xSetMaterialTextureScrollingOnOff

#define AI_M_pfn_WPG_fn_vReinitGraph WPG_fn_vReinitGraph

#define AI_M_pfn_fn_vMSLightSetRLIParameter fn_vMSLightSetRLIParameter

#define AI_M_pfn_GLI_xSetLightState GLI_xSetLightState

#define AI_M_pfn_GLI_vSetLightNearFar GLI_vSetLightNearFar

#define AI_M_pfn_GLI_xSetLightColor  GLI_xSetLightColor
/* 0x00497260 */
extern int  __cdecl GLI_lGetVisualSetNumbertOfLOD(void *p);
extern void __cdecl GLI_vGetVisualSetLOD(void *hVisualSet, long lNumberOfLodToGet,
                                         float *p_xThreshold, void **h_stObject);
extern void __cdecl GLI_vClearListOfLightInViewport(void *p_stVpt);
extern void __cdecl GLI_vAddLightToViewport(void *p_stVpt, void *p_stLight);
extern void __cdecl GLI_vResetTableOfActiveLightsForChar(char _cValue);
extern void __cdecl fn_vGetFieldTypeVector(void);          /* 0x00403290 */
extern short *__cdecl HIE_pc_fn_p_stCreateFrameTable(short sCount);
extern unsigned long *__cdecl HIE_pc_fn_pulSetFrameTableEntry(void *p_stTable, short sIndex, long lCount);
extern unsigned long __cdecl HIE_pc_fn_ulGetFrameTableEntry(void *p_stTable, short sIndex);
extern short __cdecl HIE_pc_fn_sGetFrameTableEntryCount(void *p_stTable, short sIndex);
extern void __cdecl HIE_pc_fn_vStoreScaledShortRecords(short *psDst, const float *pfSrc, long lCount);

/* 0x004B9C78 */
static float AI_pc_xLightSourceRecord[4];

void __cdecl AI_pc_fn_vLightWalkSector(int *param_1);
void __cdecl fn_vCalculeLumiereEffet(HIE_tdstSuperObject *p_stSuperObj);
void HIE_fn_vPopMatrix(void);

/* 0x00409910 */
extern void __cdecl TFMT_vChangeLetter(void *hSuperObjPerso, unsigned char ucPosLetter, unsigned char ucLetter);

extern short __cdecl MOR_fn_xStopMorph(HIE_tdstSuperObject *, unsigned short);             /* 0x443890 */
extern short __cdecl MOR_fn_xMorphing(HIE_tdstSuperObject *, short, short, int, unsigned short); /* 0x443990 */
extern short __cdecl MOR_fn_xPrepareMorph(HIE_tdstSuperObject *, short, short, int, int, int, unsigned char, unsigned short); /* 0x4435E0 */

extern unsigned char __cdecl fn_ucCollSetGetColliderPriority(void *hCollSet);
extern void          __cdecl fn_vCollSetSetColliderPriority(void *hCollSet, unsigned char ucPriority);
extern void          __cdecl fn_vCollSetSetColliderType(void *hCollSet, unsigned char ucType);
extern void          __cdecl fn_vCollSetSetColliderReal(void *hCollSet, short wIndex, float xReal);
extern void          __cdecl fn_vCollSetSetColliderVector(void *hCollSet, short wIndex, MTH3D_tdstVector *p_stVector);

extern void __cdecl fn_vSuperObjectSetLastCollidedCharacter(HIE_tdstSuperObject *hCharacter, HIE_tdstSuperObject *_h_LastCollidedCharacter);
#define AI_M_pfn_fn_vSuperObjectSetLastCollidedCharacter fn_vSuperObjectSetLastCollidedCharacter

#define AI_M_pfn_fn_pstGetDNMReport fn_pstGetDNMReport

#include "GLI/Init_Gli_helpers.h"
#define AI_M_pfn_fn_vWriteProfileString GLI_pc_fn_vWriteIniString

#include "GAM/STRUCTUR/Engmode.h"

extern void __cdecl DNM_fn_vSetHangingLimit(DNM_tdstDynamics *p_stDynamic, MTH_tdxReal xValue);

extern float MEC_g_xBaseCoeff;   /* 0x004B72C4 */
extern float MEC_g_xWalkCoeff;   /* 0x004B72C8 */
#define AI_DAT_xGlobalMecReal0  MEC_g_xBaseCoeff
#define AI_DAT_xGlobalMecReal1  MEC_g_xWalkCoeff

#ifndef DNM_M_vDynamicsSetStreamPriority
#define DNM_M_vDynamicsSetStreamPriority(_p_stDynamics,xStreamPriority) (DNM_M_xDynamicsGetStreamPriority(_p_stDynamics)=xStreamPriority)
#endif

#pragma optimize("y", off)
#pragma optimize("", on)

#define AI_M_pfn_MEC_vForcePlatformLink MEC_fn_vForcePlatformLink

AI_tdstNodeInterpret *__cdecl fn_p_PlatformLink( HIE_tdstSuperObject *p_SuperObjPerso, AI_tdstNodeInterpret *p_stTree);

#define AI_M_pfn_fn_bStandardGameIsAPlatform ((char (__cdecl *)(void *))fn_bStandardGameIsAPlatform)

extern float MEC_g_xClimbSpeedLimit;   /* 0x004A10D4 */
#define AI_MEC_xClimbSpeedLimit  MEC_g_xClimbSpeedLimit

extern void __cdecl GEO_vEndModifyObject2(ACP_tdxHandleOfObject hObject);
extern void __cdecl GEO_vGetMinMaxPointOfIndexedAlignedBox(void *hObject, short hElement,
                                                           short xBoxIdx, short *p_xMin, short *p_xMax);
#define AI_M_pfn_fn_hGetGeometricZdxOfTypeAtIndex \
    ((void *(__cdecl *)(unsigned char, unsigned short, void *))&fn_hGetGeometricZdxOfTypeAtIndex)

extern void __cdecl fn_vChangeViewPortPercent(
    unsigned long m, unsigned long w, unsigned long ww, unsigned long h,
    unsigned long ct, unsigned long cl, unsigned long cb, unsigned long cr, int x, int y);
extern void fn_vChangeViewportRatio(unsigned long ulMode, MTH_tdxReal xRatio);
#define AI_M_pfn_fn_vChangeViewportRatio  fn_vChangeViewportRatio

extern void __cdecl CS_fn_vSetPrivilegedActivationZone(
    unsigned char ucZoneType, void *hCollSet, short wIndex, unsigned char ePrivilege);

extern void __cdecl fn_vCollSetSetCharacterNoCollisionWithMap(void *h, ACP_tdxBool b);
extern void __cdecl fn_vCollSetSetCharacterNoCollisionWithProjectile(void *h, ACP_tdxBool b);
extern void __cdecl fn_vCollSetSetCharacterNoCollisionWithSecondaryCharacter(void *h, ACP_tdxBool b);
extern void __cdecl fn_vCollSetSetCharacterNoCollisionWithMainCharacter(void *h, ACP_tdxBool b);
extern void __cdecl fn_vCollSetSetCharacterNoCollisionWithOtherSectors(void *h, ACP_tdxBool b);
extern void __cdecl fn_vCollSetSetCharacterNoCollisionZdeWithProjectile(void *h, ACP_tdxBool b);

extern void __cdecl fn_vCollSetSetCollComputeFrequency(void *hCollSet, unsigned char ucFrequency);
/* 0x418850 */
extern void __cdecl fn_v3dDataSetBrainComputationFrequency(MS_tdxHandleTo3dData h_3dData, unsigned char _ucFrequency);
extern void __cdecl fn_v3dDataSetLightComputationFrequency(MS_tdxHandleTo3dData h_3dData, unsigned char _ucFrequency);
#define AI_M_pfn_fn_v3dDataSetBrainComputationFrequency ((void (__cdecl *)(void *, unsigned char))&fn_v3dDataSetBrainComputationFrequency)
#define AI_M_pfn_fn_v3dDataSetLightComputationFrequency ((void (__cdecl *)(void *, unsigned char))&fn_v3dDataSetLightComputationFrequency)

#define AI_M_pfn_SND_fn_vSetStereoSound  SND_fn_vSetStereoSound

/* 0x0046FE80 */
extern void __cdecl SND_fn_vSetStereoSound(int active);

#define AI_M_pfn_MSSoundSetSaturationDistance  ((void  (__cdecl *)(void *, long))fn_vMSSoundSetSaturationDistance)
#define AI_M_pfn_MSSoundSetBackGroundDistance  ((void  (__cdecl *)(void *, long))fn_vMSSoundSetBackGroundDistance)
/* 0x451790 */
extern void *__cdecl SND_fn_pstGetLastEventForObjectTypeSound(long type);
#define AI_M_pfn_SND_pstGetLastEventForType    SND_fn_pstGetLastEventForObjectTypeSound
/* 0x40A5A0 */
extern void __cdecl GAM_fn_lSaveBlockEventSlotMemory(void *_p_stBlockEvent, unsigned long _ulIndex);
#define AI_M_pfn_GAM_lSaveBlockEventSlotMemory ((long  (__cdecl *)(void *, long))&GAM_fn_lSaveBlockEventSlotMemory)

extern void __cdecl SND_pc_fn_vSetSoundFixedGlobal(double dValue);
#define AI_M_pfn_PcSetSoundGlobal453850        SND_pc_fn_vSetSoundFixedGlobal

/* 0x0046FE00 */
extern long __cdecl GLI_fn_lRequestNewDisplayMode(long);

extern void __cdecl GLI_fn_vSetBrightness(long lValue);

extern void __cdecl TFMT_vSetDefaultValuesCharacter(float xCharWidth, float xLineHeight, float xCharZoom);

char AI_PAD_cAnalogMinX_storage = -50;  /* 0x4A40A4 */
char AI_PAD_cAnalogMaxX_storage =  50;  /* 0x4A40A8 */
char AI_PAD_cAnalogMinY_storage = -50;  /* 0x4A40AC */
char AI_PAD_cAnalogMaxY_storage =  50;  /* 0x4A40B0 */
#define AI_PAD_cXmin          (AI_PAD_cAnalogMinX_storage)
#define AI_PAD_cXmax          (AI_PAD_cAnalogMaxX_storage)
#define AI_PAD_cYmin          (AI_PAD_cAnalogMinY_storage)
#define AI_PAD_cYmax          (AI_PAD_cAnalogMaxY_storage)

static void *AI_PAD_hAnalogAxisX_storage;   /* 0x4B9B54 */
static void *AI_PAD_hAnalogAxisY_storage;   /* 0x4B9B58 */
char  AI_PAD_cAnalogValueX_storage;         /* 0x4B9B5C */
char  AI_PAD_cAnalogValueY_storage;         /* 0x4B9B60 */
#define AI_PAD_xJoystickAxeX  (*(unsigned long *)&AI_PAD_hAnalogAxisX_storage)
#define AI_PAD_xJoystickAxeY  (*(unsigned long *)&AI_PAD_hAnalogAxisY_storage)
#define AI_PAD_cCenterX       (*(signed char *)&AI_PAD_cAnalogValueX_storage)
#define AI_PAD_cCenterY       (*(signed char *)&AI_PAD_cAnalogValueY_storage)

#define C_Pad_DefaultXMinCalibration  (-50)
#define C_Pad_DefaultXMaxCalibration  50
#define C_Pad_DefaultYMinCalibration  (-50)
#define C_Pad_DefaultYMaxCalibration  50

AI_tdstNodeInterpret *__cdecl AI_fn_p_stSetPcEngineFlags( HIE_tdstSuperObject *p_SuperObjPerso, AI_tdstNodeInterpret *p_stTree);

#define AI_PAD_lCheat1Enable     (AI_PAD_lCheat1Enable_storage)
#define AI_PAD_lCheat2Enable     (AI_PAD_lCheat2Enable_storage)
extern void (__cdecl *AI_pc_g_a2_pfnPadCheat[2])(void);
#define AI_PAD_pfnCheat          (AI_pc_g_a2_pfnPadCheat[1])

#define AI_PAD_ulUsefulDeltaTime GAM_g_ulUsefulDeltaTime  /* 0x500434 */
#define AI_PAD_xFrameLength      GAM_g_xFrameLength  /* 0x50043C */

static const unsigned long AI_PAD_x1000_bits      = 0x447A0000u;
static const unsigned long AI_PAD_xInv2Neg_bits   = 0xBF000000u;
static const unsigned long AI_PAD_xKeyStepDown_bits = 0xC2C80000u;
static const unsigned long AI_PAD_xKeyStepLeft_bits = 0x42C80000u;
static const unsigned long AI_PAD_xSlowInertia_bits = 0x3F733333u;
static const unsigned long AI_PAD_xSlowMul_bits     = 0x3F000000u;
static const unsigned long AI_PAD_xKbdInertia_bits  = 0x3F19999Au;
static const unsigned long AI_PAD_xDiagScale_bits   = 0x3F3504F3u;
static const unsigned long AI_PAD_xCircleMax_bits   = 0x45FD2000u;
static const unsigned __int64 AI_PAD_dPowExp_bits = 0x3FD3333333333333ui64;
static const unsigned long AI_PAD_xPowWeight_bits   = 0x3FC00000u;
static const unsigned long AI_PAD_xVertThresh_bits  = 0x3F4CCCCDu;
static const unsigned long AI_PAD_xMinusOne_bits    = 0xBF800000u;
static const unsigned long AI_PAD_xRotSpeedK_bits   = 0x38D1B717u;
static const unsigned long AI_PAD_xRadToDeg_bits    = 0x42652EE1u;
static const unsigned long AI_PAD_xSector45_bits    = 0x42340000u;
static const unsigned long AI_PAD_xSector135_bits   = 0x43070000u;
#define AI_PAD_x1000      (*(const float *)&AI_PAD_x1000_bits)
#define AI_PAD_xInv2Neg   (*(const float *)&AI_PAD_xInv2Neg_bits)
#define AI_PAD_xKeyStepDown (*(const float *)&AI_PAD_xKeyStepDown_bits)
#define AI_PAD_xKeyStepLeft (*(const float *)&AI_PAD_xKeyStepLeft_bits)
#define AI_PAD_xSlowInertia (*(const float *)&AI_PAD_xSlowInertia_bits)
#define AI_PAD_xSlowMul     (*(const float *)&AI_PAD_xSlowMul_bits)
#define AI_PAD_xKbdInertia  (*(const float *)&AI_PAD_xKbdInertia_bits)
#define AI_PAD_xDiagScale   (*(const float *)&AI_PAD_xDiagScale_bits)
#define AI_PAD_xCircleMax   (*(const float *)&AI_PAD_xCircleMax_bits)
#define AI_PAD_dPowExp      (*(const double *)&AI_PAD_dPowExp_bits)
#define AI_PAD_xPowWeight   (*(const float *)&AI_PAD_xPowWeight_bits)
#define AI_PAD_xVertThresh  (*(const float *)&AI_PAD_xVertThresh_bits)
#define AI_PAD_xMinusOne    (*(const float *)&AI_PAD_xMinusOne_bits)
#define AI_PAD_xRotSpeedK   (*(const float *)&AI_PAD_xRotSpeedK_bits)
#define AI_PAD_xRadToDeg    (*(const float *)&AI_PAD_xRadToDeg_bits)
#define AI_PAD_xSector45    (*(const float *)&AI_PAD_xSector45_bits)
#define AI_PAD_xSector135   (*(const float *)&AI_PAD_xSector135_bits)

#define AI_PAD_pfn_bReadButton AI_RT_IPT_fn_bIsValidated
#define AI_PAD_pfn_xReadAxis   AI_RT_IPT_fn_lGetAnalogicValue

static int AI_PAD_bPositiveNonZero(float x) { return (unsigned long)(*(long *)&x - 1L) < 0x7FFFFFFFUL; }
static int AI_PAD_bNegativeNonZero(float x) { return *(unsigned long *)&x > 0x80000000UL; }

static float AI_PAD_xSignToUnit(float x)
{
    unsigned long u = (*(unsigned long *)&x & 0xBF800000UL) | 0x3F800000UL;
    return *(float *)&u;
}

extern MTH3D_tdstVector IPT_g_FieldPadGlobalVector;     /* 0x004B9B68 */
extern short IPT_g_FieldPadHorizontalAxis;              /* 0x004B9B74 */
extern short IPT_g_FieldPadVerticalAxis;                /* 0x004B9B76 */
extern float IPT_g_FieldPadAnalogForce;                 /* 0x004B9B78 */
extern float IPT_g_FieldPadTrueAnalogForce;             /* 0x004B9B7C */
extern float IPT_g_FieldPadRotationAngle;               /* 0x004B9B80 */
extern int   IPT_g_FieldPadSector;                      /* 0x004B9B84 */

#ifndef MTH_M_xFloatToFloat
#define MTH_M_xFloatToFloat( X )            (X)
#endif
#ifndef MTH_M_xFloatToDouble
#define MTH_M_xFloatToDouble( X )           (double) (X)
#endif
#ifndef MTH_M_xDoubleToFloat
#define MTH_M_xDoubleToFloat( X )           (float) (X)
#endif
#ifndef MTH_M_xRealToDouble
#define MTH_M_xRealToDouble             MTH_M_xFloatToDouble
#endif
#ifndef MTH_M_xFloatToReal
#define MTH_M_xFloatToReal              MTH_M_xFloatToFloat
#endif
#ifndef MTH_M_xDoubleToReal
#define MTH_M_xDoubleToReal             MTH_M_xDoubleToFloat
#endif

#define AI_CAM_hCineInfo(camera)   M_GetMSHandle((camera), Cineinfo)
#define AI_CAM_hInit(hCine)        (((struct tdstCineinfo_ *)(hCine))->hInit)
#define AI_CAM_hCurrent(hCine)     (((struct tdstCineinfo_ *)(hCine))->hCurrent)
#define AI_CAM_hVisibility(hCine)  (((struct tdstCineinfo_ *)(hCine))->hVisibility)
#define AI_CAM_hWork(hCine)        (((struct tdstCineinfo_ *)(hCine))->hWork)
#define AI_CAM_uwIAFlags(internal)  (((struct tdstInternalStructurCineinfo_ *)(internal))->uwIAFlags)
#define AI_CAM_uwDNMFlags(internal) (((struct tdstInternalStructurCineinfo_ *)(internal))->uwDNMFlags)
#define AI_CAM_cChannel(internal)   (((struct tdstInternalStructurCineinfo_ *)(internal))->cChannel)
#define AI_CAM_hSuperObjectTargeted(internal) (((struct tdstInternalStructurCineinfo_ *)(internal))->hSuperObjectTargeted)
#define AI_CAM_hSecondSuperObjectTargeted(internal) (((struct tdstInternalStructurCineinfo_ *)(internal))->hSecondSuperObjectTargeted)
/* 0x004B9C14 */
extern void *CAM_g_hOldTargetedPerso;
#define AI_CAM_g_hOldTargetedPerso  (CAM_g_hOldTargetedPerso)
/* 0x0049CCF8 */
static const unsigned long AI_CAM_x0_001_bits = 0x3A83126Fu;
#define AI_CAM_x0_001  (*(const float *)&AI_CAM_x0_001_bits)

#define AI_CAM_ucVolIAFlags(hCine)  (((struct tdstCineinfo_ *)(hCine))->ucVolIAFlags)
#define AI_CAM_ucPerIAFlags(hCine)  (((struct tdstCineinfo_ *)(hCine))->ucPerIAFlags)
#define AI_CAM_xApexRatio(hCine)    (((struct tdstCineinfo_ *)(hCine))->xApexRatio)
#define AI_CAM_C_VolIAFlags_CurrentAlreadyCopiedInWork  1
#define AI_CAM_C_PerIAFlags_VisibilityCopied            2

#define AI_CAM_eTypeOfViewport(internal) (((struct tdstInternalStructurCineinfo_ *)(internal))->eTypeOfViewport)

#define AI_CAM_g_hStdCamCaracter    AI_M_g_h_StdCamCaracter   /* 0x500580 */

#define AI_CAM_bCanDoBestPos(hCine) (((struct tdstCineinfo_ *)(hCine))->bCanDoBestPos)

#define AI_M_pfn_CAM_fn_vSetResetInternalFlag CAM_fn_vSetResetInternalFlag

/* 0x0047DC70 */
extern void __cdecl CAM_fn_vActivateCamera(void *_hCineInfo, long _lTypeOfChange, ACP_tdxBool _bActive);
#define AI_M_pfn_CAM_fn_vActivateCamera CAM_fn_vActivateCamera

#define AI_M_pfn_CAM_fn_vSetCineinfoWorkFromCurrent \
    ((void (__cdecl *)(void *))CAM_fn_vSetCineinfoWorkFromCurrent)

#define AI_M_pfn_CAM_fn_vUpdateTargetPosition ((void (__cdecl *)(void *))CAM_fn_vUpdateTargetPosition)

/* 0x00414100 */
extern void __cdecl CAM_fn_vSetCineinfoCurrentFromVisibility(struct tdstCineinfo_ *hCineinfo);
extern void __cdecl CAM_fn_vSetCineinfoWorkFromVisibility(struct tdstCineinfo_ *hCineinfo);
#define AI_M_pfn_CAM_fn_vSetCineinfoCurrentFromVisibility CAM_fn_vSetCineinfoCurrentFromVisibility
#define AI_M_pfn_CAM_fn_vSetCineinfoWorkFromVisibility    CAM_fn_vSetCineinfoWorkFromVisibility

#define AI_M_pfn_CAM_fn_hGetActiveCamera         ((HIE_tdstSuperObject *(__cdecl *)(long))&CAM_fn_hGetActiveCamera)
/* 0x40B840 */
extern void __cdecl CAM_fn_vAssignCameraToAViewport(long _lViewportNumber, void *_hCamera);
#define AI_M_pfn_CAM_fn_vAssignCameraToAViewport CAM_fn_vAssignCameraToAViewport

extern void __cdecl GAM_fn_vChangeCharacterForMicro(void *_hCharacter);
#define AI_M_pfn_GAM_fn_vChangeCharacterForMicro GAM_fn_vChangeCharacterForMicro

/* 0x0047DBE0 */
extern void __cdecl CAM_fn_vChangeARealParameter(void *_hCineInfo, float *_p_xInitialValue, float *_p_xCurrentValue, float *_p_xVisibilityValue, float *_p_xWorkValue, long _lTypeOfChange, long _lChangingMode, float _xValue);
#define AI_M_pfn_CAM_fn_vChangeARealParameter CAM_fn_vChangeARealParameter

/* 0x0047DCE0 */
extern void __cdecl CAM_fn_vChangeAVectorParameter(void *_hCineInfo, MTH3D_tdstVector *_p_stInitialValue, MTH3D_tdstVector *_p_stCurrentValue, MTH3D_tdstVector *_p_stVisibilityValue, MTH3D_tdstVector *_p_stWorkValue, long _lTypeOfChange, long _lChangingMode, MTH3D_tdstVector *_p_stValue);

#define AI_M_pfn_CAM_fn_vInitCompleteCineinfo ((void (__cdecl *)(void *))CAM_fn_vInitCompleteCineinfo)

/* 0x00474270 */
extern void __cdecl CAM_fn_vInitCameraStructure(struct HIE_tdstSuperObject *_hSuperObjPerso, struct CAM_tdstUpdateCamera_ *_p_stStruct);
#define AI_M_pfn_CAM_fn_vInitCameraStructure        ((void (__cdecl *)(void *, void *))CAM_fn_vInitCameraStructure)
#define AI_M_pfn_CAM_fn_vComputeReferencePoint(p_stStruct) CAM_fn_vComputeReferencePoint((struct CAM_tdstUpdateCamera_ *)(p_stStruct))
#define AI_M_pfn_CAM_fn_vComputeTarget(p_stStruct)         CAM_fn_vComputeTarget((struct CAM_tdstUpdateCamera_ *)(p_stStruct))
/* 0x00484540 */
void CAM_fn_vComputeRealWantedPos(void *_p_stStruct);
#define AI_M_pfn_CAM_fn_vComputeRealWantedPos(p_stStruct) CAM_fn_vComputeRealWantedPos((void *)(p_stStruct))
extern void __cdecl CAM_fn_vComputeMovePosWithDynTheta(struct CAM_tdstUpdateCamera_ *_p_stStruct);
#define AI_M_pfn_CAM_fn_vComputeMovePosWithDynTheta ((void (__cdecl *)(void *))CAM_fn_vComputeMovePosWithDynTheta)
extern void __cdecl CAM_fn_vForceFailureVisibility(struct CAM_tdstUpdateCamera_ *_p_stStruct);
extern void __cdecl CAM_fn_vComputeFailureVisibility(struct CAM_tdstUpdateCamera_ *_p_stStruct);
#define AI_M_pfn_CAM_fn_vForceFailureVisibility     ((void (__cdecl *)(void *))CAM_fn_vForceFailureVisibility)   /* 0x00486EA0 */
#define AI_M_pfn_CAM_fn_vComputeFailureVisibility   ((void (__cdecl *)(void *))CAM_fn_vComputeFailureVisibility)   /* 0x00486F40 */

extern char cRefAxisIsAlreadyComputed;   /* 0x4E92FD */
extern char cNoDynChangeTheta;           /* 0x4E92FC */
#define AI_CAM_cRefAxisIsAlreadyComputed (cRefAxisIsAlreadyComputed)
#define AI_CAM_cNoDynChangeTheta         (cNoDynChangeTheta)
#define AI_CAM_cCanTestStatic            (*(unsigned char *)AI_CAM_gp_cCanTestStatic)

#define AI_CAM_struct_hWork(s)    (((CAM_tdstUpdateCamera *)(s))->hCineinfoWork)
#define AI_CAM_struct_hCurrent(s) (((CAM_tdstUpdateCamera *)(s))->hCineinfoCurrent)

#define AI_CAM_eState(cineinfo)  (((struct tdstCineinfo_ *)(cineinfo))->eState)
#define AI_CAM_C_PerIAFlags_ConstraintMoveVisFailure  0x08

#define AI_CAM_C_PerIAFlags_ShowInfo  0x01

#define AI_PAD_hAnalogAxisX  (AI_PAD_hAnalogAxisX_storage)
#define AI_PAD_hAnalogAxisY  (AI_PAD_hAnalogAxisY_storage)
#define AI_PAD_cAnalogValueX (AI_PAD_cAnalogValueX_storage)
#define AI_PAD_cAnalogValueY (AI_PAD_cAnalogValueY_storage)

#define AI_PAD_cAnalogMinX   (AI_PAD_cAnalogMinX_storage)
#define AI_PAD_cAnalogMaxX   (AI_PAD_cAnalogMaxX_storage)
#define AI_PAD_cAnalogMinY   (AI_PAD_cAnalogMinY_storage)
#define AI_PAD_cAnalogMaxY   (AI_PAD_cAnalogMaxY_storage)
#define AI_M_pfn_IPT_fn_xGetAnalogAxisValue ((float (__cdecl *)(void *))IPT_fn_lGetAnalogicValue)

/* 0x004A40B4 */
char	TEXT_gc_ViewportFixed = 1;

/* 0x004702A0 */
extern void *GAM_gs_pSaveSlotHead;
extern long GAM_gs_lNbSaveSlots;     /* 0x4A7064 */
extern unsigned long GAM_gs_ulSaveSlotCount2;   /* 0x4A7068 */
extern char GAM_g_szSlotNameWork_storage[4];    /* 0x49F8D0 */
extern char GAM_g_a_szOptionsSlotNames[3][9];   /* 0x500228 */
extern void *__cdecl GLI_pc_fn_p_vAlloc(unsigned long ulSize);   /* 0x45DC60 */

/* 0x00470450 */
extern void *GAM_gs_pSaveSlotHead;

#define AI_M_pfn_POS_fn_vInvertMatrix          ((void (__cdecl *)(void *, void *))POS_fn_vInvertMatrix)
#define AI_M_pfn_POS_fn_vMulMatrixVertex       ((void (__cdecl *)(float *, void *, float *))POS_fn_vMulMatrixVertex)
/* 0x00482F50 */
extern unsigned char __cdecl fn_ucInternalTurn(HIE_tdstSuperObject *p_SuperObjPerso, MTH_tdxReal xAngle);
#define AI_M_pfn_fn_ucInternalTurn             fn_ucInternalTurn
#define AI_M_pfn_fn_vSetProcedureActionReturn  ((void (__cdecl *)(long))fn_vSetProcedureActionReturn)

#define AI_C_xDegToRad   AI_xDegToRad   /* 0x0049CBF0 */
#define AI_C_xMilli      AI_x0_001      /* 0x0049CBF4 */

#define AI_g_ulUsefulDeltaTime  (*(unsigned long *)(GAM_ENGINE_BASE + 0xB4))

/* 0x00433210 */
extern void __cdecl MEC_fn_vUpdateSpeed(HIE_tdstSuperObject *hSupObj);
#define AI_M_pfn_MEC_fn_vUpdateSpeed             ((void (__cdecl *)(void *))&MEC_fn_vUpdateSpeed)
#define AI_M_pfn_fn_p_stDynamGetDynamics         ((void *(__cdecl *)(void *))&fn_p_stDynamGetDynamics)
#define AI_M_pfn_CAM_fn_vRemindSpeed             ((void (__cdecl *)(void *, void *))CAM_fn_vRemindSpeed)
#define AI_M_pfn_CAM_fn_vRemindPos               ((void (__cdecl *)(void *, void *))CAM_fn_vRemindPos)
#define AI_M_pfn_CAM_fn_vRemindTime              ((void (__cdecl *)(void *, float))CAM_fn_vRemindTime)
extern void __cdecl CAM_fn_vUpdateGeneralCamera(struct CAM_tdstUpdateCamera_ *_p_stStruct);
#define AI_M_pfn_CAM_fn_vUpdateGeneralCamera     ((void (__cdecl *)(void *))CAM_fn_vUpdateGeneralCamera)
/* 0x00474420 */
extern void __cdecl CAM_fn_vSendParametersToParsing(void *_p_stStruct);

/* 0x004675A0 */
AI_tdstNodeInterpret *__cdecl fn_p_stMiscUltraProcedure(
    HIE_tdstSuperObject *p_SuperObjPerso, AI_tdstNodeInterpret *p_stTree)
{

    long                  eProcedureId=M_eProcedureIdInterpret(p_stTree-1);
    AI_tdstGetSetParam    stParam;

    HIE_tdstSuperObject  *hNewSuperObjPerso;
    fn_vGetUltraOperatorPerso(fn_ucGetProcedureUltraOperator(eProcedureId),p_SuperObjPerso,&hNewSuperObjPerso);

    switch(eProcedureId)
    {
    case 0x5a :
        {
            M_EvalNextParameter(&stParam);
            CS_fn_vSetCharacterPriority(M_GetMSHandle(hNewSuperObjPerso, CollSet), (unsigned char) M_ReturnParam_lValue(&stParam));
        }
        return(p_stTree);
        break;

    case 0xee:
        M_vSetLastCollidedGoThroughMaterialOfBrain(M_GetMSHandle(hNewSuperObjPerso,Brain), GMT_C_ulInvalidHandle);
        return p_stTree;
        break;

    case 0x59:
        M_EvalNextParameter(&stParam);
        fn_v3dDataFactorFrameRate(((struct tdstEngineObject_ *)(HIE_fn_hGetSuperObjectObject(hNewSuperObjPerso)))->h_3dData,M_ReturnParam_lValue(&stParam));
        return(p_stTree);
        break;

    case 0xed:
        {
            ACP_tdxBool bFlag;
            M_EvalNextParameter(&stParam);
            bFlag = M_GetSetParam_lValue(&stParam);
            M_vSetWarnMechanicsFlagOfBrain(M_GetMSHandle(hNewSuperObjPerso,Brain), bFlag);
        }
        return(p_stTree);
        break;

    default:
        break;
    }
    return(p_stTree);
}

/* 0x00467770 */
AI_tdstNodeInterpret *__cdecl fn_p_stMiscNoProcedure(
    HIE_tdstSuperObject *p_SuperObjPerso, AI_tdstNodeInterpret *p_stTree)
{
    unsigned long procId = (unsigned long)M_eProcedureIdInterpret(p_stTree-1);

    switch (procId)
    {
    case 0x52:
        AI_g_ulOverRideRayTraceSpoMask = 0;
        break;

    case 0x51:
    {
        AI_tdstMind *p_stMind  = AI_M_p_stMindRaw(p_SuperObjPerso);
        void        *p_stIntel = (p_stMind->bDoingIntel != 0)
                                     ? (void *)AI_M_p_stGetIntelligence(p_stMind)
                                     : (void *)AI_M_p_stGetReflex(p_stMind);
        void        *p_stComport = AI_M_GetCurrentComport((AI_tdstIntelligence *)p_stIntel);
        void        *p_stSchedule;

        fn_vInitActionTable((AI_tdstIntelligence *)p_stIntel);
        p_stSchedule = AI_M_GetComportSchedule((AI_tdstComport *)p_stComport);
        if (p_stSchedule == NULL)
            AI_M_GetCurrentSchedule((AI_tdstIntelligence *)p_stIntel) = NULL;
        else
            AI_M_GetCurrentSchedule((AI_tdstIntelligence *)p_stIntel) = AI_M_GetTreeComportSchedule((AI_tdstComport *)p_stComport);
        AI_g_iNumLoop = 0;

        AI_M_pfn_fn_vSetProcedureActionReturn(0xb);
        break;
    }

    case 0x50:
    {
        MS_tdxHandleToDynam h_Dynam = M_GetMSHandle(p_SuperObjPerso,Dynam);
        if (h_Dynam != NULL)
        {
            DNM_tdstDynamics *p_stDynamics = AI_M_pfn_fn_p_stDynamGetDynamics(h_Dynam);
            if (p_stDynamics != NULL)
            {
                void *hMecIdCard = AI_M_pfn_fn_h_GetCurrentDNMIdCard(p_SuperObjPerso);
                if (hMecIdCard != NULL)
                    DNM_fn_vChangeIdCard(p_stDynamics, hMecIdCard);
            }
        }
        break;
    }

    case 0x4f:
        AI_M_pfn_fn_vSnifThePlayerIsDead();
        break;
    }
    return p_stTree;
}

/* 0x00467850 */
AI_tdstNodeInterpret *__cdecl fn_p_stKillPersoAndClearVariableProcedure(
    HIE_tdstSuperObject *p_SuperObjPerso, AI_tdstNodeInterpret *p_stTree)
{
    long                eProcedureId = M_eProcedureIdInterpret(p_stTree-1);
    void               *hKilledObject;
    AI_tdstGetSetParam  stParam;
    AI_tdstMind        *p_stMind;
    unsigned char       ucVarId;

    switch (eProcedureId)
    {
    case 0x8c:

        {
            HIE_tdstSuperObject *p_stResultOwnerPerso;
            if (M_GetTypeInterpret(p_stTree) == E_ti_Operator)
            {
                p_stTree++;
                p_stTree = AI_M_pfn_fn_p_stEvalNextVarIdPerso(p_SuperObjPerso, p_stTree, (void **)&p_stResultOwnerPerso);
            }
            else
                p_stResultOwnerPerso = p_SuperObjPerso;
            p_stMind = AI_M_p_stMindRaw(p_stResultOwnerPerso);
            ucVarId = (unsigned char)M_ucVarIdInterpret(p_stTree);
            p_stTree++;
        }
        if (AI_M_pfn_fn_ucGetDsgVar(ucVarId, 0, p_stMind, &stParam) == AI_C_VALID_GET)
        {
            hKilledObject = stParam.uParam.p_stEngObjValue;
            AI_M_pfn_fn_vKillEngineObject(hKilledObject);

            if (hKilledObject == M_GetEngineObject(p_SuperObjPerso))
                AI_M_pfn_fn_vSetProcedureActionReturn(3);

            stParam.uParam.p_stEngObjValue = NULL;
            ucVarId = fn_ucSetDsgVar(ucVarId, 0, p_stMind, &stParam);
        }
        return p_stTree;

    case 0x8b:
        {
            p_stTree = fn_p_stEvalTree(p_SuperObjPerso, p_stTree, &stParam);
            hKilledObject = stParam.uParam.p_stEngObjValue;
            AI_M_pfn_fn_vKillEngineObject(hKilledObject);

            if (hKilledObject == M_GetEngineObject(p_SuperObjPerso))
                AI_M_pfn_fn_vSetProcedureActionReturn(3);
        }
        return p_stTree;

    default:
        break;
    }
    return p_stTree;
}

/* 0x00467970 */
AI_tdstNodeInterpret *__cdecl fn_p_stMiscProcedure(HIE_tdstSuperObject *p_SuperObjPerso, AI_tdstNodeInterpret *p_stTree)
{
    AI_tdstGetSetParam stValue;
    long eProcedureId=M_eProcedureIdInterpret(p_stTree-1);

    M_EvalNextParameter(&stValue);

    switch ( eProcedureId )
    {
    case 0x5b:
        {
            struct tdstEngineObject_ *p_stObj = M_GetSetParam_p_stEngineObjValue(&stValue);

            if ( M_ObjectIsAlreadyTreated(p_stObj) )
            {

                ((MS_tdxHandleToStandardGame)p_stObj->h_StandardGame)->ulLastTrame = GAM_g_stEngineStructure.stEngineTimer.ulTrameNumber - 1;

                HIE_fn_vInvalidateHierarchy(M_GetSuperObject(p_stObj));
            }
        }
        return p_stTree;
        break;

    case 0x132:
        {
            ACP_tdxBool bFreeze = M_GetSetParam_cValue(&stValue);

            if ( bFreeze )
            {
                fn_vSaveEngineClock();
            }
            else if ( GAM_g_stEngineStructure.bEngineFrozen )   /* 0x467A0D */
            {
                fn_vLoadEngineClock();
            }

            GAM_g_stEngineStructure.bEngineFrozen = bFreeze;
        }
        return p_stTree;
        break;

    case 0x130:
        {
            MS_tdxHandleToStandardGame hStdGame = M_GetMSHandle(p_SuperObjPerso,StandardGame);
            ACP_tdxBool bPlatform = (ACP_tdxBool) M_GetSetParam_cValue(&stValue), bPrevPlatform = fn_ucStandardGameGetPlatFormType(hStdGame);

            if ( bPrevPlatform != bPlatform )
            {

                if ( bPrevPlatform )
                {
                    int i;
                    HIE_tdstSuperObject *hChild;

                    HIE_M_ForEachChildOf(p_SuperObjPerso,hChild,i)
                    {
                        if ( HIE_fn_ulGetSuperObjectType(hChild) == HIE_C_ulActor )
                        {
                            MS_tdxHandleToDynam hDynam = M_GetMSHandle(hChild, Dynam);
                            if ( hDynam )
                            {
                                DNM_tdstDynamics *p_stDynamic = fn_p_stDynamGetDynamics(hDynam);
                                if ( p_stDynamic && MEC_fn_bIsBaseMechanic(p_stDynamic) )
                                {

                                    MEC_fn_vFreePlatformLink(p_stDynamic, hChild);
                                }
                            }

                            fn_vReputCharacterSuperObjectAtTheWorld(hChild);
                        }
                    }
                }

                hStdGame->ucPlatFormType = (unsigned char) bPlatform;

                HIE_fn_vChangeFather(p_SuperObjPerso, HIE_M_hGetSuperObjectFather(p_SuperObjPerso), bPlatform);
            }
        }
        return p_stTree;
        break;

    case 0x53:
        AI_g_p_stMainActor = M_GetSetParam_p_stSupObjValue(&stValue);
        return p_stTree;
        break;

    case 0x54:
        {
            struct tdstEngineObject_ *p_stActivateObject;
            p_stActivateObject = M_GetSetParam_p_stEngineObjValue(&stValue);
            fn_vForceActiveAnObject(p_stActivateObject);
        }
        return p_stTree;
        break;

    case 0x55:
        fn_vDesactiveAnObject(M_GetSetParam_p_stEngineObjValue(&stValue));
        return p_stTree;
        break;

    case 0x57:
    case 0x56:
        GAM_fn_vAskToChangeLevel(fn_szGetStringFromTextOrStringParam(&stValue), ( ACP_tdxBool ) ( eProcedureId == 0x56 ) );
        return p_stTree;
        break;

    case 0x58:
        g_ucUseDefaultActionReturn = TRUE;/* 0x467B45 */
        g_ucNewActionReturn = (unsigned char) M_GetSetParam_cValue(&stValue);
        return p_stTree;
        break;

    default:
        break;
    }
    return(p_stTree);
}

/* 0x00467C60 */
AI_tdstNodeInterpret *__cdecl fn_p_stMiscMoreProcedure(
    HIE_tdstSuperObject *p_SuperObjPerso, AI_tdstNodeInterpret *p_stTree)
{
    unsigned long         procId = (unsigned long)M_eProcedureIdInterpret(p_stTree-1);
    AI_tdstGetSetParam    stParam1, stParam2;
    AI_tdstNodeInterpret *p_stNext;

    p_stNext = fn_p_stEvalTree(p_SuperObjPerso, p_stTree, &stParam1);
    p_stNext = fn_p_stEvalTree(p_SuperObjPerso, p_stNext, &stParam2);

    if (procId == 0x5c)
    {
        AI_M_pfn_fn_vSnifPlayerDeadWithResurrection(
            stParam1.uParam.p_stEngObjValue
                ? AI_M_pfn_fn_h_StandardGameGetSuperObject(stParam1.uParam.p_stEngObjValue->h_StandardGame)
                : NULL,
            stParam2.uParam.p_stEngObjValue
                ? AI_M_pfn_fn_h_StandardGameGetSuperObject(stParam2.uParam.p_stEngObjValue->h_StandardGame)
                : NULL);
    }
    return p_stNext;
}

/* 0x00467CF0 */
AI_tdstNodeInterpret *__cdecl fn_p_stDynamProcedure(
    HIE_tdstSuperObject *p_SuperObjPerso, AI_tdstNodeInterpret *p_stTree)
{
    unsigned long         procId = (unsigned long)M_eProcedureIdInterpret(p_stTree-1);
    HIE_tdstSuperObject  *hNewSuperObjPerso;

    fn_vGetUltraOperatorPerso(fn_ucGetProcedureUltraOperator(procId), p_SuperObjPerso, &hNewSuperObjPerso);

    switch (procId)
    {
    case 0x9f:
        {
            if (AI_M_g_h_StdCamCaracter != NULL)
            {

                char *p_stCineinfo = *(char **)((char *)HIE_fn_hGetSuperObjectObject((HIE_tdstSuperObject *)AI_M_g_h_StdCamCaracter) + 0x10);

                if (CAM_fn_hCineInfoGetSuperObjectTargeted(*(void **)(p_stCineinfo + 4)) == (void *)hNewSuperObjPerso)
                {
                    unsigned long     ulIndex;
                    float             xBaseTime;
                    MTH3D_tdstVector *p_stVectorPos;

                    p_stVectorPos = POS_fn_p_stGetTranslationVector(HIE_fn_hGetSuperObjectGlobalMatrix(hNewSuperObjPerso));

                    xBaseTime = 0.03f;
                    for (ulIndex = 0; ulIndex < AI_C_ucNbOfCheckedSpeeds; ulIndex++)
                    {
                        *(float *)(p_stCineinfo + 0x138 + ulIndex * 4) = xBaseTime;
                        MTH3D_M_vNullVector((MTH3D_tdstVector *)(p_stCineinfo + 0x78 + ulIndex * 0xc));
                        MTH3D_M_vCopyVector((MTH3D_tdstVector *)(p_stCineinfo + 0xd8 + ulIndex * 0xc), p_stVectorPos);
                    }
                }
            }
        }
        return p_stTree;

    default:
        break;
    }
    return p_stTree;
}

/* 0x00467D90 */
AI_tdstNodeInterpret *__cdecl fn_p_stHitPointsProcedure(
    HIE_tdstSuperObject *p_SuperObjPerso, AI_tdstNodeInterpret *p_stTree)
{
    unsigned long         procId = (unsigned long)M_eProcedureIdInterpret(p_stTree-1);
    AI_tdstGetSetParam    stParam;

    p_stTree = fn_p_stEvalTree(p_SuperObjPerso, p_stTree, &stParam);

    switch (procId)
    {
    case 0:
        return AI_M_pfn_fn_p_stSetHitPoints(p_SuperObjPerso, p_stTree, stParam.uParam.p_stEngObjValue);
    case 1:
        {
            void *hPerso = stParam.uParam.p_stEngObjValue;
            p_stTree = fn_p_stEvalTree(p_SuperObjPerso, p_stTree, &stParam);
            AI_M_pfn_fn_vSetHitPointsInit(hPerso, stParam.uParam.ucValue);
        }
        return p_stTree;
    case 2:
        AI_M_pfn_fn_vSetHitPointsToInitValue(stParam.uParam.p_stEngObjValue);
        return p_stTree;
    case 3:
        AI_M_pfn_fn_vSetHitPointsToMaxValue(stParam.uParam.p_stEngObjValue);
        return p_stTree;
    case 4:
        return AI_M_pfn_fn_p_stAddHitPoints(p_SuperObjPerso, p_stTree, stParam.uParam.p_stEngObjValue);
    case 5:
        return AI_M_pfn_fn_p_stSubHitPoints(p_SuperObjPerso, p_stTree, stParam.uParam.p_stEngObjValue);
    default:
        return p_stTree;
    }
}

/* 0x00467E80 */
AI_tdstNodeInterpret *__cdecl fn_p_stSetHitPoints(
    HIE_tdstSuperObject *p_SuperObjPerso, AI_tdstNodeInterpret *p_stTree, void *p_stObject)
{
    AI_tdstGetSetParam    stParam;
    AI_tdstNodeInterpret *p_stNext = fn_p_stEvalTree(p_SuperObjPerso, p_stTree, &stParam);
    unsigned char ucValue = (unsigned char)((stParam.eType == 8) ? (long)stParam.uParam.xValue
                                                                  : stParam.uParam.lValue);
    fn_vSetHitPoints(p_stObject, ucValue);
    return p_stNext;
}

/* 0x00467EE0 */
AI_tdstNodeInterpret *__cdecl fn_p_stAddHitPoints(
    HIE_tdstSuperObject *p_SuperObjPerso, AI_tdstNodeInterpret *p_stTree, void *p_stObject)
{
    AI_tdstGetSetParam    stParam;
    AI_tdstNodeInterpret *p_stNext = fn_p_stEvalTree(p_SuperObjPerso, p_stTree, &stParam);
    unsigned char ucValue = (unsigned char)((stParam.eType == 8) ? (long)stParam.uParam.xValue
                                                                  : stParam.uParam.lValue);
    fn_vAddHitPoints(p_stObject, ucValue);
    return p_stNext;
}

/* 0x00467F40 */
AI_tdstNodeInterpret *__cdecl fn_p_stSubHitPoints(
    HIE_tdstSuperObject *p_SuperObjPerso, AI_tdstNodeInterpret *p_stTree, void *p_stObject)
{
    AI_tdstGetSetParam    stParam;
    AI_tdstNodeInterpret *p_stNext = fn_p_stEvalTree(p_SuperObjPerso, p_stTree, &stParam);
    unsigned char ucValue = (unsigned char)((stParam.eType == 8) ? (long)stParam.uParam.xValue
                                                                  : stParam.uParam.lValue);
    (void) fn_ucSubHitPoints(p_stObject, ucValue);
    return p_stNext;
}

/* 0x00467FA0 */
AI_tdstNodeInterpret *__cdecl fn_p_stHitPointsMaxProcedure(
    HIE_tdstSuperObject *p_SuperObjPerso, AI_tdstNodeInterpret *p_stTree)
{
    unsigned long         procId   = (unsigned long)M_eProcedureIdInterpret(p_stTree-1);
    AI_tdstGetSetParam    stParam;
    AI_tdstNodeInterpret *p_stNext = fn_p_stEvalTree(p_SuperObjPerso, p_stTree, &stParam);
    void                 *p_vEngineObject = stParam.uParam.p_stEngObjValue;

    switch (procId)
    {
    case 6:
        return AI_M_pfn_fn_p_stSetHitPointsMax(p_SuperObjPerso, p_stNext, p_vEngineObject);
    case 7:
        AI_M_pfn_fn_vSetHitPointsMaxToInit(p_vEngineObject);
        return p_stNext;
    case 8:
        AI_M_pfn_fn_vSetHitPointsMaxToMax(p_vEngineObject);
        return p_stNext;
    case 9:
        return AI_M_pfn_fn_p_stAddHitPointsMax(p_SuperObjPerso, p_stNext, p_vEngineObject);
    case 10:
        return AI_M_pfn_fn_p_stSubHitPointsMax(p_SuperObjPerso, p_stNext, p_vEngineObject);
    default:
        return p_stNext;
    }
}

/* 0x00468060 */
AI_tdstNodeInterpret *__cdecl fn_p_stSetHitPointsMax(
    HIE_tdstSuperObject *p_SuperObjPerso, AI_tdstNodeInterpret *p_stTree, void *p_stObject)
{
    AI_tdstGetSetParam    stParam;
    AI_tdstNodeInterpret *p_stNext = fn_p_stEvalTree(p_SuperObjPerso, p_stTree, &stParam);
    unsigned char ucValue = (unsigned char)((stParam.eType == 8) ? (long)stParam.uParam.xValue
                                                                  : stParam.uParam.lValue);
    fn_vSetHitPointsMax(p_stObject, ucValue);
    return p_stNext;
}

/* 0x004680C0 */
AI_tdstNodeInterpret *__cdecl fn_p_stAddHitPointsMax(
    HIE_tdstSuperObject *p_SuperObjPerso, AI_tdstNodeInterpret *p_stTree, void *p_stObject)
{
    AI_tdstGetSetParam    stParam;
    AI_tdstNodeInterpret *p_stNext = fn_p_stEvalTree(p_SuperObjPerso, p_stTree, &stParam);
    unsigned char ucValue = (unsigned char)((stParam.eType == 8) ? (long)stParam.uParam.xValue
                                                                  : stParam.uParam.lValue);
    fn_vAddHitPointsMax(p_stObject, ucValue);
    return p_stNext;
}

/* 0x00468120 */
AI_tdstNodeInterpret *__cdecl fn_p_stSubHitPointsMax(
    HIE_tdstSuperObject *p_SuperObjPerso, AI_tdstNodeInterpret *p_stTree, void *p_stObject)
{
    AI_tdstGetSetParam    stParam;
    AI_tdstNodeInterpret *p_stNext = fn_p_stEvalTree(p_SuperObjPerso, p_stTree, &stParam);
    unsigned char ucValue = (unsigned char)((stParam.eType == 8) ? (long)stParam.uParam.xValue
                                                                  : stParam.uParam.lValue);
    (void) fn_ucSubHitPointsMax(p_stObject, ucValue);
    return p_stNext;
}

/* 0x00468180 */
AI_tdstNodeInterpret *__cdecl fn_p_stListZDDZDEProcedure(
    HIE_tdstSuperObject *p_SuperObjPerso, AI_tdstNodeInterpret *p_stTree)
{
    AI_tdstGetSetParam stValue;
    long eProcedureId=M_eProcedureIdInterpret(p_stTree-1);
    void *p_stList=NULL;

    M_EvalNextParameter(&stValue);
    p_stList=(void *) M_GetSetParam_Dsg_List(&stValue);

    if (eProcedureId==0x18)
    {
        unsigned short xPersoMask,xOthersMask;

        M_EvalNextParameter(&stValue);
        xPersoMask=M_GetSetParam_xMaskValue(&stValue);

        M_EvalNextParameter(&stValue);
        xOthersMask=M_GetSetParam_xMaskValue(&stValue);

        fn_vAffectTypeZdeWithTypeZdeInList(p_stList,p_SuperObjPerso,xPersoMask,xOthersMask);
    }
    else
    {
        ACP_tdxIndex xIndex;

        M_EvalNextParameter(&stValue);
        xIndex=(ACP_tdxIndex) M_GetSetParam_lValue(&stValue);

        if (eProcedureId==0x12)
        {
            fn_vAffectZddInList(p_stList,p_SuperObjPerso, 0  ,xIndex);
        }
        else
        if (eProcedureId==0x13)
        {
            fn_vAffectZddInList(p_stList,p_SuperObjPerso, 1  , xIndex);
        }
        else
        {
            unsigned short xMask;
            int            iZdx;

            if ( (eProcedureId==0x14)
                 ||
                 (eProcedureId==0x15)
               )
            {
                GMT_fn_vResetEntireMask(&xMask);
            }
            else
            {
                M_EvalNextParameter(&stValue);
                xMask=M_GetSetParam_xMaskValue(&stValue);
            }

            /* 0x468243 */
            if ( (eProcedureId==0x14)
                 ||
                 (eProcedureId==0x16)
               )
            {
                iZdx = 0;
            }
            else
            {
                iZdx = 1;
            }
            fn_vAffectZdeInList(p_stList, p_SuperObjPerso, iZdx, xIndex, xMask);
        }
    }
    return(p_stTree);
}

/* 0x004682B0 */
AI_tdstNodeInterpret *__cdecl fn_p_stListProcedure(HIE_tdstSuperObject *p_SuperObjPerso, AI_tdstNodeInterpret *p_stTree)
{
    AI_tdstGetSetParam stParam1,stParam2;
    long eProcedureId=M_eProcedureIdInterpret(p_stTree-1);
    tdstList *p_stList;

    M_EvalNextParameter(&stParam1);
    p_stList=(tdstList *) M_GetSetParam_Dsg_List(&stParam1);

    switch(eProcedureId)
    {
#if !defined(_AI_EXCLUDE_NEVER_USED_)
    case 0x19:
        M_EvalNextParameter(&stParam1);
        fn_vPutInList(p_stList, M_GetSetParam_p_stSupObjValue(&stParam1));
        return(p_stTree);
        break;

    case 0x1a:
        M_EvalNextTwoParameter();
        fn_vInsertPersoAtPositionInList(p_stList,M_GetSetParam_p_stSupObjValue(&stParam1),(unsigned char) M_GetSetParam_lValue(&stParam2));
        return(p_stTree);
        break;
#endif
    case 0x1b:
        M_EvalNextParameter(&stParam1);
        fn_vDeletePersoAtPositionInList(p_stList,(unsigned char)M_GetSetParam_lValue(&stParam1));
        return(p_stTree);
        break;

#if !defined(_AI_EXCLUDE_NEVER_USED_)
    case 0x1c:
        M_EvalNextParameter(&stParam1);
        (void)fn_bFindPersoAndDeleteInList(p_stList,M_GetSetParam_p_stSupObjValue(&stParam1));
        return(p_stTree);
        break;

    case 0x1d:
        return(fn_p_stListSelect(p_stList,0, p_stTree));
        break;

    case 0x1e:
        return(fn_p_stListSelect(p_stList,1, p_stTree));
        break;
#endif

    case 0x1f:
        M_EvalNextTwoParameter();
        return( fn_p_stListSort(p_stList,(unsigned char)M_GetSetParam_lValue(&stParam1),(char)M_GetSetParam_lValue(&stParam2),p_stTree) );
        break;

    case 0x20:
        M_EvalNextParameter(&stParam1);
        fn_vSortListByFamily(p_stList, M_GetSetParam_hFamily(&stParam1));
        return p_stTree;
        break;

    case 0x21:
        M_EvalNextParameter(&stParam1);
        fn_vSortListByModel(p_stList, M_GetSetParam_p_stModel(&stParam1));
        return p_stTree;
        break;

#if !defined(_AI_EXCLUDE_NEVER_USED_)
    case 0x22:
        {
            HIE_tdstSuperObject *hChild, *hBrother;
            void                *hFamily;

            M_ClearList(p_stList);

            M_EvalNextParameter(&stParam2);
            hFamily=M_GetSetParam_hFamily(&stParam2);

            for ( hChild = HIE_fn_hGetSuperObjectFirstChild(gp_stDynamicWorld); HIE_fn_bIsSuperObjectValid(hChild); hChild = HIE_fn_hGetSuperObjectFirstChild(hChild) )
            {

                for ( hBrother = hChild; HIE_fn_bIsSuperObjectValid(hBrother); hBrother = HIE_fn_hGetSuperObjectNextBrother(hBrother) )
                {
                    if
                    (
                        (HIE_fn_ulGetSuperObjectType(hBrother) == HIE_C_ulActor)
                        && (fn_h3dDataGetFamily(M_GetMSHandle(hBrother,3dData)) == hFamily)
                    )
                    {

                        if (M_ucNbEltInList(p_stList)<M_ucGetMaxSizeOfList(p_stList))
                            fn_vPutInList(p_stList, hBrother);
                        else
                            break;
                    }
                }
            }
        }
        return(p_stTree);
        break;

    case 0x23:
        {
            HIE_tdstSuperObject *hChild, *hBrother;
            AI_tdstAIModel      *p_stModel;

            M_ClearList(p_stList);

            M_EvalNextParameter(&stParam2);
            p_stModel=M_GetSetParam_p_stModel(&stParam2);

            for ( hChild = HIE_fn_hGetSuperObjectFirstChild(gp_stDynamicWorld); HIE_fn_bIsSuperObjectValid(hChild); hChild = HIE_fn_hGetSuperObjectFirstChild(hChild) )
            {

                for ( hBrother = hChild; HIE_fn_bIsSuperObjectValid(hBrother); hBrother = HIE_fn_hGetSuperObjectNextBrother(hBrother) )
                {
                    MS_tdxHandleToBrain hBrain;
                    if
                    (
                        (HIE_fn_ulGetSuperObjectType(hBrother) == HIE_C_ulActor)
                        &&	((hBrain = M_GetMSHandle(hBrother,Brain)) != 0)
                        &&	(AI_M_p_stGetAIModel(M_pstGetMindOfBrain(hBrain)) == p_stModel)
                    )
                    {

                        if (M_ucNbEltInList(p_stList)<M_ucGetMaxSizeOfList(p_stList))
                            fn_vPutInList(p_stList, hBrother);
                        else
                            break;
                    }
                }
            }
        }
        return(p_stTree);
        break;
#endif

    case 0x24:
        {
            HIE_tdstSuperObject *hPerso;
            void                *hFamily;
            unsigned char        ucPos = 0;

            M_EvalNextParameter(&stParam2);
            hFamily=M_GetSetParam_hFamily(&stParam2);

            hPerso = fn_p_stGetPersoInList(p_stList, ucPos);
            while(hPerso)
            {
                if(fn_h3dDataGetFamily(M_GetMSHandle(hPerso,3dData)) == hFamily)
                    fn_vDeletePersoAtPositionInList(p_stList, ucPos);
                else
                    ucPos++;
                hPerso = fn_p_stGetPersoInList(p_stList, ucPos);
            }
        }
        return(p_stTree);
        break;

    case 0x25:
        {
            HIE_tdstSuperObject *hPerso;
            AI_tdstAIModel      *p_stModel;
            unsigned char        ucPos = 0;

            M_EvalNextParameter(&stParam2);
            p_stModel=M_GetSetParam_p_stModel(&stParam2);

            hPerso = fn_p_stGetPersoInList(p_stList, ucPos);
            while(hPerso)
            {
                MS_tdxHandleToBrain hBrain;
                if
                (
                    ((hBrain = M_GetMSHandle(hPerso,Brain)) != 0)
                &&	(AI_M_p_stGetAIModel(M_pstGetMindOfBrain(hBrain)) == p_stModel)
                )
                    fn_vDeletePersoAtPositionInList(p_stList, ucPos);
                else
                    ucPos++;
                hPerso = fn_p_stGetPersoInList(p_stList, ucPos);
            }
        }
        return(p_stTree);
        break;

    default:
        break;
    }
    return(p_stTree);
}

/* 0x00468660 */
AI_tdstNodeInterpret *__cdecl fn_p_stListEnsembleProcedure(
    HIE_tdstSuperObject *p_SuperObjPerso, AI_tdstNodeInterpret *p_stTree)
{
    unsigned long         procId   = (unsigned long)M_eProcedureIdInterpret(p_stTree-1);
    AI_tdstGetSetParam    stValue;
    AI_tdstNodeInterpret *p_stNext = fn_p_stEvalTree(p_SuperObjPerso, p_stTree, &stValue);
    void                 *p_stListA = stValue.uParam.p_stEngObjValue;
    void                 *p_stListB, *p_stListC;

    p_stNext  = fn_p_stEvalTree(p_SuperObjPerso, p_stNext, &stValue);
    p_stListB = stValue.uParam.p_stEngObjValue;

    if (procId == 0x29)
    {
        AI_M_pfn_fn_vAddList(p_stListA, p_stListB);
        return p_stNext;
    }

    p_stNext  = fn_p_stEvalTree(p_SuperObjPerso, p_stNext, &stValue);
    p_stListC = stValue.uParam.p_stEngObjValue;
    switch (procId)
    {
    case 0x26: AI_M_pfn_fn_vUnionList(p_stListA, p_stListB, p_stListC); break;
    case 0x27: AI_M_pfn_fn_vInterList(p_stListA, p_stListB, p_stListC); break;
    case 0x28: AI_M_pfn_fn_vDiffList (p_stListA, p_stListB, p_stListC); break;
    default:   break;
    }
    return p_stNext;
}

/* 0x00468730 */
AI_tdstNodeInterpret *__cdecl fn_p_stLinkTableProcedure(
    HIE_tdstSuperObject *p_SuperObjPerso, AI_tdstNodeInterpret *p_stTree)
{
    AI_tdstGetSetParam    stParam;
    unsigned char         ucObj1;
    unsigned long         procId;

    procId = (unsigned long)M_eProcedureIdInterpret(p_stTree-1);
    p_stTree = fn_p_stEvalTree(p_SuperObjPerso, p_stTree, &stParam);
    switch (procId)
    {
    case 0x5d:
        ucObj1 = stParam.uParam.ucValue;
        p_stTree = fn_p_stEvalTree(p_SuperObjPerso, p_stTree, &stParam);
        fn_vExchangeTwoObjectsInCurrentObjectsTable(p_SuperObjPerso, ucObj1,
                                                    stParam.uParam.ucValue);
        return p_stTree;
    case 0x5e:

        M_GetMSHandle(p_SuperObjPerso,3dData)->h_CurrentObjectsTable =
            (tdxHandleToObjectsTablesList)stParam.uParam.ulValue;
        return p_stTree;
    default:
        break;
    }
    return p_stTree;
}

/* 0x004687B0 */
AI_tdstNodeInterpret *__cdecl fn_p_stBuildObjectTableFromTableAndStringProcedure(
    HIE_tdstSuperObject *p_SuperObjPerso, AI_tdstNodeInterpret *p_stTree)
{
    AI_tdstGetSetParam    stParam;
    unsigned long         eProcedureId = (unsigned long)M_eProcedureIdInterpret(p_stTree-1);
    char                 *pszChar;

    p_stTree = fn_p_stEvalTree(p_SuperObjPerso, p_stTree, &stParam);
    switch (eProcedureId)
    {
    case 0x60:
    {
        float         xScreenWidth;
        long          lStart, lEnd;
        unsigned char stStatus[0x30];

        pszChar = AI_M_pfn_fn_szGetStringFromParam(&stParam);
        p_stTree = fn_p_stEvalTree(p_SuperObjPerso, p_stTree, &stParam);
        xScreenWidth = M_ReturnParam_xValue(&stParam);
        p_stTree = fn_p_stEvalTree(p_SuperObjPerso, p_stTree, &stParam);
        lStart = M_ReturnParam_lValue(&stParam);
        p_stTree = fn_p_stEvalTree(p_SuperObjPerso, p_stTree, &stParam);
        lEnd = M_ReturnParam_lValue(&stParam);

        if (lStart < 0)
            lStart = 0;

        TFMT_wPositionModulesAccordingToText(stStatus, p_SuperObjPerso, pszChar,
                                             lStart, lEnd, xScreenWidth, NULL, -1);
    }
        return p_stTree;

    case 0x5f:
    {
        short  wNbObjectsInTargetTable = 0, wNbObjectsInSourceTable = 0;
        short  wTargetObject = 0;

        void  *hSourceObjectTable = stParam.uParam.p_stEngObjValue;

        void  *hTargetObjectTable = M_GetMSHandle(p_SuperObjPerso,3dData)->h_CurrentObjectsTable;
        wNbObjectsInTargetTable = AI_M_pfn_fn_uwGetObjectTableNb(hTargetObjectTable);
        wNbObjectsInSourceTable = AI_M_pfn_fn_uwGetObjectTableNb(hSourceObjectTable);

        p_stTree = fn_p_stEvalTree(p_SuperObjPerso, p_stTree, &stParam);
        pszChar = AI_M_pfn_fn_szGetStringFromParam(&stParam);

        while ((*pszChar) && (wTargetObject < wNbObjectsInTargetTable))
        {

            short wSourceObject = toupper(*pszChar) - ' ';
            if (wSourceObject < 0 || wSourceObject >= wNbObjectsInSourceTable)
                wSourceObject = 0;
            AI_M_pfn_fn_vCopyObjectInTable(hTargetObjectTable, hSourceObjectTable, wTargetObject, wSourceObject);
            pszChar++;
            wTargetObject++;
        }

        while (wTargetObject < wNbObjectsInTargetTable)
        {
            AI_M_pfn_fn_vCopyObjectInTable(hTargetObjectTable, hSourceObjectTable, wTargetObject, 0);
            wTargetObject++;
        }
    }
        return p_stTree;

    default:
        break;
    }
    return p_stTree;
}

/* 0x00468970 */
AI_tdstNodeInterpret *__cdecl fn_p_stModuleControlProcedure(
    HIE_tdstSuperObject *p_SuperObjPerso, AI_tdstNodeInterpret *p_stTree)
{
	AI_tdstGetSetParam stParam1;
	long eProcedureId = M_eProcedureIdInterpret(p_stTree-1);
	long lModuleId;

	if ( eProcedureId != 0xd8   )
		M_EvalNextParameter(&stParam1);

	switch(eProcedureId)
	{
	case 0x62:
		lModuleId = M_GetSetParam_lValue(&stParam1);
		for ( lModuleId --; lModuleId >= 0; lModuleId -- )
		{

			fn_vTakeControlOfChannel((tdstEngineObject *)HIE_fn_hGetSuperObjectObject(p_SuperObjPerso), (unsigned short) lModuleId, TRUE);
		}
		return(p_stTree);
		break;

	case 0x61:
		lModuleId = M_GetSetParam_lModuleValue(&stParam1);
		fn_vTakeControlOfChannel((tdstEngineObject *)HIE_fn_hGetSuperObjectObject(p_SuperObjPerso), (unsigned short)lModuleId, TRUE);
		return(p_stTree);
		break;

	case 0x63:
		lModuleId = M_GetSetParam_lModuleValue(&stParam1);
		fn_vTakeControlOfChannel((tdstEngineObject *)HIE_fn_hGetSuperObjectObject(p_SuperObjPerso), (unsigned short)lModuleId, FALSE);
		return(p_stTree);
		break;

	case 0xd8:
		fn_vTakeControlOfChannel((tdstEngineObject *)HIE_fn_hGetSuperObjectObject(p_SuperObjPerso), (unsigned short) -1, FALSE);
		return(p_stTree);
		break;

	default:
		break;
	}
    return(p_stTree);
}

/* 0x00468AB0 */
AI_tdstNodeInterpret *__cdecl fn_p_stSaveGameProcedure(
    HIE_tdstSuperObject *p_SuperObjPerso, AI_tdstNodeInterpret *p_stTree)
{
    unsigned long         procId = (unsigned long)M_eProcedureIdInterpret(p_stTree-1);
    AI_tdstGetSetParam    stParam;
    switch (procId)
    {
    case 0x65:
        fn_vChangeEngineMode(AI_C_EM_ModeStoppingProgram);
        AI_M_pfn_fn_vSetProcedureActionReturn(3);
        break;
    case 0x64:
        p_stTree = fn_p_stEvalTree(p_SuperObjPerso, p_stTree, &stParam);
        p_stTree = fn_p_stEvalTree(p_SuperObjPerso, p_stTree, &stParam);
        break;
    default:
        break;
    }
    return p_stTree;
}

/* 0x00468B10 */
AI_tdstNodeInterpret *__cdecl fn_p_stChangeActivationChannelProcedure(
    HIE_tdstSuperObject *p_SuperObjPerso, AI_tdstNodeInterpret *p_stTree)
{
    AI_tdstGetSetParam    stParam;
    HIE_tdstSuperObject  *hNewSuperObjPerso;
    unsigned short        uwChannel;
    unsigned long         procId;

    procId = (unsigned long)M_eProcedureIdInterpret(p_stTree-1);

    fn_vGetUltraOperatorPerso(fn_ucGetProcedureUltraOperator(procId), p_SuperObjPerso, &hNewSuperObjPerso);

    p_stTree  = fn_p_stEvalTree(p_SuperObjPerso, p_stTree, &stParam);
    uwChannel = stParam.uParam.uwValue;

    switch (procId)
    {
    case 0x66:
        fn_vActivateChannel(hNewSuperObjPerso, uwChannel, 1);
        return p_stTree;
    case 0x67:
        fn_vActivateChannel(hNewSuperObjPerso, uwChannel, 0);
        return p_stTree;
    default:
        break;
    }
    return p_stTree;
}

/* 0x00468B90 */
AI_tdstNodeInterpret *__cdecl fn_p_stFogProcedure(
    HIE_tdstSuperObject *p_SuperObjPerso, AI_tdstNodeInterpret *p_stTree)
{
    long                  eProcedureId = M_eProcedureIdInterpret(p_stTree-1);
    AI_tdstGetSetParam    stParam;
    AI_tdstNodeInterpret *p_stNext;

    switch (eProcedureId)
    {
    case 0x2a:
        p_stNext = fn_p_stEvalTree(p_SuperObjPerso, p_stTree, &stParam);
        AI_GLI_cGlobalForceFogIsOn = (char)((stParam.eType == 8
            ? (long)stParam.uParam.xValue : (long)stParam.uParam.cValue) != 0);
        return p_stNext;
    case 0x2b:
    {
        GEO_tdstColor stColor;
        p_stNext = fn_p_stEvalTree(p_SuperObjPerso, p_stTree, &stParam);
        fn_vConvertParamToColor(&stParam,&stColor);
        AI_GLI_a_xFogColor[0] = stColor.xR; AI_GLI_a_xFogColor[1] = stColor.xG;
        AI_GLI_a_xFogColor[2] = stColor.xB; AI_GLI_a_xFogColor[3] = stColor.xA;
        return p_stNext;
    }
    case 0x2c:
        p_stNext = fn_p_stEvalTree(p_SuperObjPerso, p_stTree, &stParam);
        AI_GLI_xFogNear = AI_PROC_xReadReal(&stParam);
        p_stNext = fn_p_stEvalTree(p_SuperObjPerso, p_stNext, &stParam);
        AI_GLI_xFogFar  = AI_PROC_xReadReal(&stParam);
        p_stNext = fn_p_stEvalTree(p_SuperObjPerso, p_stNext, &stParam);
        AI_GLI_xFogInf  = AI_PROC_xReadReal(&stParam);
        return p_stNext;
    case 0x2d:
        p_stNext = fn_p_stEvalTree(p_SuperObjPerso, p_stTree, &stParam);
        AI_GLI_xFogBlendNear = AI_PROC_xReadReal(&stParam);
        p_stNext = fn_p_stEvalTree(p_SuperObjPerso, p_stNext, &stParam);
        AI_GLI_xFogBlendFar  = AI_PROC_xReadReal(&stParam);
        return p_stNext;
    case 0x2e:
        if (AI_GLI_cFogIsSaved != 0)
        {
            AI_GLI_a_xFogColor[0] = AI_GLI_a_xSavedFogColor[0];
            AI_GLI_a_xFogColor[1] = AI_GLI_a_xSavedFogColor[1];
            AI_GLI_a_xFogColor[2] = AI_GLI_a_xSavedFogColor[2];
            AI_GLI_a_xFogColor[3] = AI_GLI_a_xSavedFogColor[3];
            AI_GLI_xFogNear      = AI_GLI_xSavedFogNear;
            AI_GLI_xFogFar       = AI_GLI_xSavedFogFar;
            AI_GLI_xFogInf       = AI_GLI_xSavedFogInf;
            AI_GLI_xFogBlendNear = AI_GLI_xSavedFogBlendNear;
            AI_GLI_xFogBlendFar  = AI_GLI_xSavedFogBlendFar;
            AI_GLI_cFogIsSaved   = 0;
            AI_GLI_cGlobalForceFogIsOn = AI_GLI_cSavedForceFogIsOn;
        }
        return p_stTree;
    case 0x2f:
        if (AI_GLI_cFogIsSaved == 0)
        {
            AI_GLI_a_xSavedFogColor[0] = AI_GLI_a_xFogColor[0];
            AI_GLI_a_xSavedFogColor[1] = AI_GLI_a_xFogColor[1];
            AI_GLI_a_xSavedFogColor[2] = AI_GLI_a_xFogColor[2];
            AI_GLI_a_xSavedFogColor[3] = AI_GLI_a_xFogColor[3];
            AI_GLI_xSavedFogNear      = AI_GLI_xFogNear;
            AI_GLI_xSavedFogFar       = AI_GLI_xFogFar;
            AI_GLI_xSavedFogInf       = AI_GLI_xFogInf;
            AI_GLI_xSavedFogBlendNear = AI_GLI_xFogBlendNear;
            AI_GLI_xSavedFogBlendFar  = AI_GLI_xFogBlendFar;
            AI_GLI_cFogIsSaved        = 1;
            AI_GLI_cSavedForceFogIsOn = AI_GLI_cGlobalForceFogIsOn;
        }
        return p_stTree;
    default:
        return p_stTree;
    }
}

/* 0x00468E90 */
AI_tdstNodeInterpret *__cdecl fn_p_stPersoLightProcedure(
    HIE_tdstSuperObject *p_SuperObjPerso, AI_tdstNodeInterpret *p_stTree)
{
    long                  eProcedureId = M_eProcedureIdInterpret(p_stTree-1);
    MS_tdxHandleToMSLight h_MSLight = NULL;

    HIE_tdstSuperObject  *hNewSuperObjPerso;
    fn_vGetUltraOperatorPerso(fn_ucGetProcedureUltraOperator(eProcedureId), p_SuperObjPerso, &hNewSuperObjPerso);

    h_MSLight = M_GetEngineObject(hNewSuperObjPerso)->h_MSLight;

    if (h_MSLight == NULL)
    {
        return(fn_p_stSwapNextParameter2(p_stTree));
    }
    else
    {
        AI_tdstGetSetParam stParam;

        switch(eProcedureId)
        {
        case 0x68:
            fn_vMSLightSetOnOff(h_MSLight,1);
            return(p_stTree);
            break;

        case 0x69:
            fn_vMSLightSetOnOff(h_MSLight,0);
            return(p_stTree);
            break;

        case 0x6b:
            {
                MTH_tdxReal xNear,xFar;

                M_EvalNextParameter(&stParam);
                xNear=M_ReturnParam_xValue(&stParam);

                M_EvalNextParameter(&stParam);
                xFar=M_ReturnParam_xValue(&stParam);

                fn_vMSLightSetNearFar(h_MSLight,xNear,xFar);
            }
            return(p_stTree);
            break;

        case 0x6a:
            {
                GEO_tdstColor stColor;
                M_EvalNextParameter(&stParam);
                fn_vConvertParamToColor(&stParam,&stColor);
                fn_vMSLightSetColorRGBA(h_MSLight,stColor.xR,stColor.xG,stColor.xB,stColor.xA);
            }
            return(p_stTree);
            break;

#if !defined(_AI_EXCLUDE_NEVER_USED_)
        case 0x6c:
            {
                MTH_tdxReal xAlpha;

                M_EvalNextParameter(&stParam);
                xAlpha=M_ReturnParam_xValue(&stParam);
                fn_vMSLightSetLittleAlpha(h_MSLight,xAlpha);

                M_EvalNextParameter(&stParam);
                xAlpha=M_ReturnParam_xValue(&stParam);
                fn_vMSLightSetBigAlpha(h_MSLight,xAlpha);
            }
            return(p_stTree);
            break;

        case 0x6d:
            {
                MTH_tdxReal xStep;
                ACP_tdxBool bOnOff;

                M_EvalNextParameter(&stParam);
                bOnOff=(ACP_tdxBool) M_GetSetParam_lValue(&stParam)?TRUE:FALSE;

                M_EvalNextParameter(&stParam);
                xStep=M_ReturnParam_xValue(&stParam);

                fn_vMSLightSetGiroPhare(h_MSLight,(unsigned char) bOnOff,xStep);
            }
            return(p_stTree);
            break;

        case 0x6e:
            {
                MTH_tdxReal xStep,xMaxRange;
                ACP_tdxBool bOnOff;

                M_EvalNextParameter(&stParam);
                bOnOff=(ACP_tdxBool) M_GetSetParam_lValue(&stParam)?TRUE:FALSE;

                M_EvalNextParameter(&stParam);
                xStep=M_ReturnParam_xValue(&stParam);

                M_EvalNextParameter(&stParam);
                xMaxRange=M_ReturnParam_xValue(&stParam);

                fn_vMSLightSetPulse(h_MSLight,(unsigned char) bOnOff,xStep,xMaxRange);
            }
            return(p_stTree);
            break;

        case 0x6f:
            fn_vMSLightSetTypeOfLight(h_MSLight,1);
            return(p_stTree);
            break;
#endif

        case 0x70:
            fn_vMSLightSetTypeOfLight(h_MSLight,2);
            return(p_stTree);
            break;

#if !defined(_AI_EXCLUDE_NEVER_USED_)
        case 0x71:
            fn_vMSLightSetTypeOfLight(h_MSLight,3);
            return(p_stTree);
            break;

        case 0x72:
            fn_vMSLightSetTypeOfLight(h_MSLight,4);
            return(p_stTree);
            break;
#endif

        default:
            break;
        }
        return(p_stTree);
    }
}

/* 0x004691E0 */
AI_tdstNodeInterpret *__cdecl fn_p_stSoundProcedure(
    HIE_tdstSuperObject *p_SuperObjPerso, AI_tdstNodeInterpret *p_stTree)
{
    unsigned long         procId = (unsigned long)M_eProcedureIdInterpret(p_stTree-1);
    HIE_tdstSuperObject  *hNewSuperObjPerso;
    AI_tdstGetSetParam    stParam;
    AI_tdstNodeInterpret *p_stNext;

    fn_vGetUltraOperatorPerso(fn_ucGetProcedureUltraOperator(procId), p_SuperObjPerso, &hNewSuperObjPerso);
    p_stNext = fn_p_stEvalTree(p_SuperObjPerso, p_stTree, &stParam);

    switch (procId)
    {
    case 0x73:
        SND_fn_lSendRequestSound((long)(void *)hNewSuperObjPerso, GAM_g_lSoundObjectTypeAnim, stParam.uParam.p_stEngObjValue, 0, NULL);
        break;
    case 0x75:
        SND_fn_lSendRequestSound(AI_SND_C_OBJET_FANTOME, GAM_g_lSoundObjectTypeMusic, stParam.uParam.p_stEngObjValue, 0, NULL);
        break;
    case 0x76:
        SND_fn_lSendRequestSound(AI_SND_C_OBJET_FANTOME, GAM_g_lSoundObjectTypeAnim, stParam.uParam.p_stEngObjValue, 0, NULL);
        break;
    case 0x77:
        SND_fn_lSendRequestSound(AI_SND_C_OBJET_FANTOME, AI_g_lSoundObjectTypeMenu, stParam.uParam.p_stEngObjValue, 0, NULL);
        break;
    }
    return p_stNext;
}

/* 0x004692D0 */
AI_tdstNodeInterpret *__cdecl fn_p_stParticleGeneratorProcedure(
    HIE_tdstSuperObject *p_SuperObjPerso, AI_tdstNodeInterpret *p_stTree)
{

	long							eProcedureId = M_eProcedureIdInterpret(p_stTree-1);
	AI_tdstGetSetParam				stParam;
	struct tdstParticleGenerator_	*hPrtGen;
	MS_tdxHandleToMSPrtSrc			hMSPrtSrc;
	MTH_tdxReal						xReal;
	ACP_tdxIndex					xIndex;

	HIE_tdstSuperObject *hNewSuperObjPerso;
	fn_vGetUltraOperatorPerso(fn_ucGetProcedureUltraOperator(eProcedureId),p_SuperObjPerso,&hNewSuperObjPerso);

	if ( (hMSPrtSrc = M_GetMSHandle(hNewSuperObjPerso,MSPrtSrc)) == NULL )
		return (p_stTree);

	switch(eProcedureId)
	{

	case 0x7b:
		fn_vMSPrtSrcSetGeneratorActivation(hMSPrtSrc,1);
		return(p_stTree);
		break;

	case 0x7c:
		fn_vMSPrtSrcSetGeneratorActivation(hMSPrtSrc,0);
		return(p_stTree);
		break;

	case 0x7d:
		M_EvalNextParameter(&stParam);
		hPrtGen = M_GetSetParam_hParticleGenerator(&stParam);
		fn_p_stMSPrtSrcSetGenerator(hMSPrtSrc,hPrtGen);
		return(p_stTree);
		break;

	case 0x7e:
		fn_vMSPrtSrcSetGenerationModeNone(hMSPrtSrc);
		return(p_stTree);
		break;

	case 0x7f:
		fn_vMSPrtSrcSetGenerationModeContinuous(hMSPrtSrc);
		return(p_stTree);
		break;

	case 0x80:
		M_EvalNextParameter(&stParam);
		xIndex = (ACP_tdxIndex)M_GetSetParam_lValue(&stParam);
		M_EvalNextParameter(&stParam);
		fn_vMSPrtSrcSetGenerationModeCrenel(hMSPrtSrc, xIndex, (ACP_tdxIndex)M_GetSetParam_lValue(&stParam) );
		return(p_stTree);
		break;

	case 0x81:
		M_EvalNextParameter(&stParam);
		fn_vMSPrtSrcSetGenerationModeProbability(hMSPrtSrc, M_GetSetParam_xValue(&stParam));
		return(p_stTree);
		break;

	case 0x82:
		M_EvalNextParameter(&stParam);
		fn_vMSPrtSrcSetGenerationNumberConstant(hMSPrtSrc,(ACP_tdxIndex) M_GetSetParam_lValue(&stParam));
		return(p_stTree);
		break;

	case 0x83:
		M_EvalNextParameter(&stParam);
		xIndex = (ACP_tdxIndex)M_GetSetParam_lValue(&stParam);
		M_EvalNextParameter(&stParam);
		fn_vMSPrtSrcSetGenerationNumberProbabilist(hMSPrtSrc,xIndex, (ACP_tdxIndex)M_GetSetParam_lValue(&stParam));
		return(p_stTree);
		break;

	case 0x84:
		fn_vMSPrtSrcSetParticlesInfiniteLifeTime(hMSPrtSrc);
		return(p_stTree);
		break;

	case 0x85:
		M_EvalNextParameter(&stParam);
		fn_vMSPrtSrcSetParticlesConstantLifeTime(hMSPrtSrc,M_GetSetParam_xValue(&stParam));
		return(p_stTree);
		break;

	case 0x86:
		M_EvalNextParameter(&stParam);
		xReal = M_GetSetParam_xValue(&stParam);
		M_EvalNextParameter(&stParam);
		fn_vMSPrtSrcSetParticlesProbabilistLifeTime(hMSPrtSrc,xReal,M_GetSetParam_xValue(&stParam));
		return(p_stTree);
		break;

	default:
		break;
	}

	return(p_stTree);
}

/* 0x00469520 */
AI_tdstNodeInterpret *__cdecl fn_p_stKeyboardProcedure(
    HIE_tdstSuperObject *p_SuperObjPerso, AI_tdstNodeInterpret *p_stTree)
{
    unsigned long         procId = (unsigned long)M_eProcedureIdInterpret(p_stTree-1);
    AI_tdstGetSetParam    stValue;
    AI_tdstNodeInterpret *p_stNext = fn_p_stEvalTree(p_SuperObjPerso, p_stTree, &stValue);
    long                  lHandle  = stValue.uParam.lValue;

    switch (procId)
    {
    case 0x40:
        AI_M_pfn_IPT_DesactivateEntryElement(lHandle);
        break;
    case 0x41:
        AI_M_pfn_IPT_ActivateEntryElement(lHandle);
        break;
    default:
        break;
    }
    return p_stNext;
}

static void AI_v_InternalChangeComportProcedure( AI_tdstIntelligence *p_stIntelOrReflex, void *p_stComport, char bContinueExecution);

/* 0x00469580 */
AI_tdstNodeInterpret *__cdecl fn_p_stChangeComportIntelligenceProcedure(
    HIE_tdstSuperObject *p_SuperObjPerso, AI_tdstNodeInterpret *p_stTree)
{
    AI_tdstGetSetParam    stParam;
    void                 *p_stAimed, *p_stComport;
    AI_tdstMind          *p_stMind;
    char                  bTest;
    AI_tdstNodeInterpret *p_stNext;

    p_stNext  = fn_p_stEvalTree(p_SuperObjPerso, p_stTree, &stParam);
    p_stAimed = AI_M_pfn_fn_h_StandardGameGetSuperObject(stParam.uParam.p_stEngObjValue->h_StandardGame);
    p_stMind  = AI_M_p_stMindRaw(p_stAimed);
    p_stNext  = fn_p_stEvalTree(p_SuperObjPerso, p_stNext, &stParam);
    p_stComport = stParam.uParam.p_stEngObjValue;
    bTest = (char)((p_stAimed != (void *)p_SuperObjPerso) || (p_stMind->bDoingIntel == 0));
    AI_v_InternalChangeComportProcedure(AI_M_p_stGetIntelligence(p_stMind), p_stComport, bTest);
    return p_stNext;
}

/* 0x00469600 */
static void AI_v_InternalChangeComportProcedure(
    AI_tdstIntelligence *p_stIntelOrReflex, void *p_stComport, char bContinueExecution)
{
    if (p_stIntelOrReflex->p_stCurrentComport != p_stComport)
    {
        if (fn_ucChangeComportIntell(p_stIntelOrReflex,
                                        (AI_tdstComport *)p_stComport) != 0
            && bContinueExecution == 0)
        {
            AI_g_iNumLoop = 0;
            AI_M_pfn_fn_vSetProcedureActionReturn(AI_C_ACTION_CHANGE_COMPORT);
        }
    }
}

/* 0x00469640 */
AI_tdstNodeInterpret *__cdecl fn_p_stChangeComportReflexProcedure(
    HIE_tdstSuperObject *p_SuperObjPerso, AI_tdstNodeInterpret *p_stTree)
{
    AI_tdstGetSetParam    stParam;
    void                 *p_stAimed, *p_stComport;
    AI_tdstMind          *p_stMind;
    char                  bTest;
    AI_tdstNodeInterpret *p_stNext;

    p_stNext  = fn_p_stEvalTree(p_SuperObjPerso, p_stTree, &stParam);
    p_stAimed = AI_M_pfn_fn_h_StandardGameGetSuperObject(stParam.uParam.p_stEngObjValue->h_StandardGame);
    p_stMind  = AI_M_p_stMindRaw(p_stAimed);
    p_stNext  = fn_p_stEvalTree(p_SuperObjPerso, p_stNext, &stParam);
    p_stComport = stParam.uParam.p_stEngObjValue;
    bTest = (char)((p_stAimed != (void *)p_SuperObjPerso) || (p_stMind->bDoingIntel != 0));
    AI_v_InternalChangeComportProcedure(AI_M_p_stGetReflex(p_stMind), p_stComport, bTest);
    return p_stNext;
}

/* 0x004696C0 */
AI_tdstNodeInterpret *__cdecl fn_p_stChangeMyComportIntelligenceProcedure(
    HIE_tdstSuperObject *p_SuperObjPerso, AI_tdstNodeInterpret *p_stTree)
{
    AI_tdstGetSetParam    stParam;
    AI_tdstMind          *p_stMind = AI_M_p_stMindRaw(p_SuperObjPerso);
    AI_tdstNodeInterpret *p_stNext = fn_p_stEvalTree(p_SuperObjPerso, p_stTree, &stParam);

    AI_v_InternalChangeComportProcedure(
        AI_M_p_stGetIntelligence(p_stMind), stParam.uParam.p_stEngObjValue,
        (char)(p_stMind->bDoingIntel == 0));
    return p_stNext;
}

/* 0x00469710 */
AI_tdstNodeInterpret *__cdecl fn_p_stChangeMyComportReflexProcedure(
    HIE_tdstSuperObject *p_SuperObjPerso, AI_tdstNodeInterpret *p_stTree)
{
    AI_tdstGetSetParam    stParam;
    AI_tdstMind          *p_stMind = AI_M_p_stMindRaw(p_SuperObjPerso);
    AI_tdstNodeInterpret *p_stNext = fn_p_stEvalTree(p_SuperObjPerso, p_stTree, &stParam);

    AI_v_InternalChangeComportProcedure(
        AI_M_p_stGetReflex(p_stMind), stParam.uParam.p_stEngObjValue,
        p_stMind->bDoingIntel);
    return p_stNext;
}

/* 0x00469760 */
AI_tdstNodeInterpret *__cdecl fn_p_stChangeMyComportIntelligenceAndReflexProcedure(
    HIE_tdstSuperObject *p_SuperObjPerso, AI_tdstNodeInterpret *p_stTree)
{
    AI_tdstGetSetParam    stParam;
    AI_tdstMind          *p_stMind = AI_M_p_stMindRaw(p_SuperObjPerso);
    void                 *p_stIntellComport, *p_stReflexComport;
    AI_tdstNodeInterpret *p_stNext;

    p_stNext = fn_p_stEvalTree(p_SuperObjPerso, p_stTree, &stParam);
    p_stIntellComport = stParam.uParam.p_stEngObjValue;
    p_stNext = fn_p_stEvalTree(p_SuperObjPerso, p_stNext, &stParam);
    p_stReflexComport = stParam.uParam.p_stEngObjValue;

    AI_v_InternalChangeComportProcedure(AI_M_p_stGetIntelligence(p_stMind), p_stIntellComport, 0);
    AI_v_InternalChangeComportProcedure(AI_M_p_stGetReflex(p_stMind),       p_stReflexComport, 0);
    return p_stNext;
}

/* 0x004697D0 */
AI_tdstNodeInterpret *__cdecl fn_p_stTurnProcedure(
    HIE_tdstSuperObject *p_SuperObjPerso, AI_tdstNodeInterpret *p_stTree)
{
	AI_tdstGetSetParam stParam;
	MTH_tdxReal xAngle;
	long eProcedureId=M_eProcedureIdInterpret(p_stTree-1);
	DNM_tdstDynamics *p_stDynamics;
	MS_tdxHandleToDynam hDynam;

	HIE_tdstSuperObject *hNewSuperObjPerso;
	fn_vGetUltraOperatorPerso(fn_ucGetProcedureUltraOperator(eProcedureId),p_SuperObjPerso,&hNewSuperObjPerso);

	M_EvalNextParameter(&stParam);
	xAngle=M_ReturnParam_xValue(&stParam);

	hDynam = M_GetMSHandle(hNewSuperObjPerso,Dynam);
	if (hDynam)
	{
		p_stDynamics = fn_p_stDynamGetDynamics(hDynam);
		if (p_stDynamics)
		{

			switch(eProcedureId)
			{
				case 0x87:
					xAngle=MTH_M_xDegToRad(xAngle);
					xAngle =MTH_M_xMul(xAngle,MTH_M_xMul(MTH_M_xLongToReal(GAM_g_stEngineStructure.stEngineTimer.ulUsefulDeltaTime),MTH_M_xConstFloatToReal(0.001f)));
					MEC_vTurnAroundZAxis ( p_stDynamics,xAngle );
				break;
				case 0x88:
					xAngle=MTH_M_xDegToRad(xAngle);
					MEC_vTurnAroundZAxis ( p_stDynamics,xAngle );
				break;
			default:
				break;
			}
		}
	}

	return(p_stTree);
}

/* 0x004698A0 */
AI_tdstNodeInterpret *__cdecl fn_p_stCustomBitsProc(
    HIE_tdstSuperObject *p_SuperObjPerso, AI_tdstNodeInterpret *p_stTree)
{
	AI_tdstGetSetParam stParam;
	unsigned long eProcedureId = M_eProcedureIdInterpret(p_stTree - 1);
  HIE_tdstSuperObject *hNewSuperObjPerso;
  unsigned long ulCurrentBits;
	MS_tdxHandleToStandardGame hStdGame;

	fn_vGetUltraOperatorPerso(fn_ucGetProcedureUltraOperator(eProcedureId),p_SuperObjPerso,&hNewSuperObjPerso);

	hStdGame = M_GetMSHandle(hNewSuperObjPerso,StandardGame);

  ulCurrentBits = fn_ulStandardGameGetCustomBits(hStdGame);

  M_EvalNextParameter(&stParam);
  switch ( eProcedureId )
  {
  case 0xae:
    {
      unsigned long ulMask = (1 << (M_GetSetParam_lValue(&stParam) - 1));

      M_EvalNextParameter(&stParam);
      if ( M_GetSetParam_lValue(&stParam) )
        ulCurrentBits |= ulMask;
      else
        ulCurrentBits &= ~ulMask;

      fn_vStandardGameSetCustomBits(hStdGame, ulCurrentBits);
    }
    break;

  case 0xaf:
    {
      unsigned long ulBitMask, ulNewBits;

      ulBitMask = M_GetSetParam_lValue(&stParam);

      M_EvalNextParameter(&stParam);
      ulNewBits = M_GetSetParam_lValue(&stParam);

      ulCurrentBits &= ~ulBitMask;

      ulNewBits &= ulBitMask;

      ulCurrentBits |= ulNewBits;

      fn_vStandardGameSetCustomBits(hStdGame, ulCurrentBits);
    }
    break;

	default:
		break;
  }
  return p_stTree;
}

/* 0x00469960 */
AI_tdstNodeInterpret *__cdecl fn_p_stStringProc(
    HIE_tdstSuperObject *p_SuperObjPerso, AI_tdstNodeInterpret *p_stTree)
{
    long                  eProcedureId = M_eProcedureIdInterpret(p_stTree-1);
    AI_tdstGetSetParam    stParam;
    char                 *pszString     = NULL;
    short                 wStringLength  = 0;
    short                 wAffectedPos;

    p_stTree = fn_p_stEvalTree(p_SuperObjPerso, p_stTree, &stParam);

    if (eProcedureId != 0xf2)
    {
        pszString = AI_M_pfn_fn_szGetStringFromParam(&stParam);
        if (pszString != NULL)
            wStringLength = (short)strlen(pszString);

        p_stTree = fn_p_stEvalTree(p_SuperObjPerso, p_stTree, &stParam);
    }
    wAffectedPos = stParam.uParam.wValue;

    switch (eProcedureId)
    {
    case 0xf2:
        GAM_g_ucCurrentLanguage = stParam.uParam.ucValue;
        AI_M_pfn_fn_vWriteProfileString(
            "RAYMAN2", "Language",
            ((tdstLanguageStructure *)GAM_g_stEngineStructure.p_stLanguageTable)[stParam.uParam.ucValue].szLanguageCode);
        break;

    case 0xef:
    case 0xf0:
        if (wAffectedPos >= 0 && wAffectedPos < wStringLength)
        {
            short wNewCharacter;

            p_stTree = fn_p_stEvalTree(p_SuperObjPerso, p_stTree, &stParam);
            wNewCharacter = stParam.uParam.wValue;
            pszString += wAffectedPos;
            if (eProcedureId == 0xef)
                memmove(pszString + 1, pszString,
                        (size_t)(wStringLength - wAffectedPos) + 1);
            *pszString = (char)wNewCharacter;
        }
        break;

    case 0xf1:
        if (wAffectedPos >= 0 && wAffectedPos < wStringLength)
        {
            pszString += wAffectedPos;

            memmove(pszString, pszString + 1, (size_t)(wStringLength - wAffectedPos));
        }
        break;
    }
    return p_stTree;
}

/* 0x00469AC0 */
AI_tdstNodeInterpret *__cdecl fn_p_stIntToText(
    HIE_tdstSuperObject *p_SuperObjPerso, AI_tdstNodeInterpret *p_stTree)
{
    AI_tdstGetSetParam    stParam;
    char                 *pszString;
    long                  iValue;
    AI_tdstNodeInterpret *p_stNext;

    p_stNext  = fn_p_stEvalTree(p_SuperObjPerso, p_stTree, &stParam);
    pszString = AI_M_pfn_fn_szGetStringFromParam(&stParam);

    p_stNext = fn_p_stEvalTree(p_SuperObjPerso, p_stNext, &stParam);
    iValue   = M_ReturnParam_lValue(&stParam);

    _itoa((int)iValue, pszString, 10);
    return p_stNext;
}

/* 0x00469B30 */
AI_tdstNodeInterpret *__cdecl fn_p_stCollideCommunicationProc(
    HIE_tdstSuperObject *p_SuperObjPerso, AI_tdstNodeInterpret *p_stTree)
{
    AI_tdstGetSetParam    stParam;
    HIE_tdstSuperObject  *hNewSuperObjPerso = p_SuperObjPerso;
    void                 *hCollSet;
    long                  eProcedureId = M_eProcedureIdInterpret(p_stTree-1);
    unsigned char         ucPriority;
    short                 wIndex = -1;

    fn_vGetUltraOperatorPerso(fn_ucGetProcedureUltraOperator(eProcedureId), p_SuperObjPerso, &hNewSuperObjPerso);

    hCollSet = M_GetMSHandle(hNewSuperObjPerso,CollSet);

    p_stTree = fn_p_stEvalTree(p_SuperObjPerso, p_stTree, &stParam);
    ucPriority = M_GetSetParam_lValue(&stParam);

    p_stTree = fn_p_stEvalTree(p_SuperObjPerso, p_stTree, &stParam);
    switch (eProcedureId)
    {
    case 0xe8:
        break;

    case 0xe9:
    case 0xea:

        wIndex = stParam.uParam.wValue;
        p_stTree = fn_p_stEvalTree(p_SuperObjPerso, p_stTree, &stParam);
        break;

    default:
        return p_stTree;
    }

    if (hCollSet && ucPriority >= fn_ucCollSetGetColliderPriority(hCollSet))
    {

        fn_vCollSetSetColliderPriority(hCollSet, ucPriority);
        switch (eProcedureId)
        {
        case 0xe8:
            fn_vCollSetSetColliderType(hCollSet, (unsigned char)stParam.uParam.cValue);
            break;

        case 0xe9:
            fn_vCollSetSetColliderVector(hCollSet, wIndex,
                                         (MTH3D_tdstVector *)stParam.uParam.a_xVectorValue);
            break;

        case 0xea:
            fn_vCollSetSetColliderReal(hCollSet, wIndex, M_ReturnParam_xValue(&stParam));
            break;
        }
    }
    return p_stTree;
}

/* 0x00469C70 */
void __cdecl fn_vUpdateSuperObjectAfter3DChanges(
    HIE_tdstSuperObject *_p_SuperObj, POS_tdstCompletePosition *_hGLobalMovedMatrix,
    MTH3D_tdstVector *_p_stNewPosition, ACP_tdxBool _bIsAnActor)
{

    HIE_fn_vComputeNewRelativeMatrix(_p_SuperObj);

    if (_bIsAnActor)
    {

        if ( _hGLobalMovedMatrix && M_GetMSHandle(_p_SuperObj, Dynam) )
        {
            MEC_vImposeRotationMatrix(
                fn_p_stDynamGetDynamics(M_GetMSHandle(_p_SuperObj, Dynam)),
                _hGLobalMovedMatrix
            );
        }
    }
}
/* 0x00469CB0 */
AI_tdstNodeInterpret *__cdecl fn_p_st3DProc_ForReorientation(
    HIE_tdstSuperObject *p_SuperObjPerso, AI_tdstNodeInterpret *p_stTree)
{
	AI_tdstGetSetParam stParam;
	long eProcedureId = M_eProcedureIdInterpret(p_stTree - 1);
	HIE_tdstSuperObject *hNewSuperObjPerso = p_SuperObjPerso, *hChannelSuperObject = NULL, *hCharSO = p_SuperObjPerso;
	POS_tdstCompletePosition *hGLobalMovedMatrix;
	unsigned char uwChannel = C_ucUnknownChannel, uwSecondaryMode = 0;
	MTH3D_tdstVector stImposedAxis, stProposedAxis, stGlobalTranslation, stInducedTranslation;
	ACP_tdxBool bTranslationChanged = 0, bGlobalVectors = 1;

	fn_vGetUltraOperatorPerso(fn_ucGetProcedureUltraOperator(eProcedureId),p_SuperObjPerso,&hNewSuperObjPerso);
	hCharSO = hNewSuperObjPerso;

	switch ( eProcedureId )
	{
	case 0xe5:
	case 0xe6:

		M_EvalNextParameter(&stParam);
		uwChannel = (unsigned char)M_GetSetParam_lModuleValue(&stParam);
		if ( uwChannel == C_ucUnknownChannel )
		{

			return fn_p_stSwapNextParameter(p_stTree);
			break;
		}
		hChannelSuperObject = fn_hGetSuperObjectInChannel(fn_h3dDataGetChannelSOList(M_GetMSHandle(hNewSuperObjPerso,3dData)), uwChannel);

		M_EvalNextParameter(&stParam);
		bGlobalVectors = (ACP_tdxBool)M_GetSetParam_lValue(&stParam);

		bTranslationChanged |= (eProcedureId == 0xe6);

	case 0xab:
	case 0xac:

		M_EvalNextParameter(&stParam);
		stImposedAxis = M_GetSetParam_stVectorValue(&stParam);
		M_EvalNextParameter(&stParam);
		stProposedAxis = M_GetSetParam_stVectorValue(&stParam);

		M_EvalNextParameter(&stParam);
		uwSecondaryMode = (unsigned char)M_GetSetParam_uwValue(&stParam);

		bTranslationChanged |= (eProcedureId == 0xac);
		if ( bTranslationChanged )
		{
			M_EvalNextParameter(&stParam);
			stGlobalTranslation = M_GetSetParam_stVectorValue(&stParam);
		}
		break;

	default:
		return p_stTree;
		break;
	}

	if ( uwChannel != C_ucUnknownChannel )
	{
		if ( !bGlobalVectors )
		{

			POS_tdstCompletePosition *hPersoGlobalMatrix = HIE_fn_hGetSuperObjectGlobalMatrix(hNewSuperObjPerso);

			POS_fn_vMulMatrixVector(&stImposedAxis, hPersoGlobalMatrix, &stImposedAxis);
			POS_fn_vMulMatrixVector(&stProposedAxis, hPersoGlobalMatrix, &stProposedAxis);
		}

		if ( hChannelSuperObject )
		{

			hNewSuperObjPerso = hChannelSuperObject;
		}

	}

	hGLobalMovedMatrix = HIE_fn_hGetSuperObjectGlobalMatrix(hNewSuperObjPerso);

	if ( bTranslationChanged )
	{

		POS_fn_vMulMatrixVector(&stInducedTranslation, hGLobalMovedMatrix, &stGlobalTranslation);
	}

	{
		MTH3D_tdstVector stI, stJ, stK, stTmp;

		POS_fn_vGetRotationMatrix(hGLobalMovedMatrix, &stI, &stJ, &stK);
		if ( MTH3D_M_bIsNullVector(&stImposedAxis) )
		{
			switch ( uwSecondaryMode )
			{
			default:
				break;

			case 0:
			case 1:
				stImposedAxis = stJ;
				break;

			case 2:
			case 3:
				stImposedAxis = stK;
				break;

			case 4:
			case 5:
				stImposedAxis = stI;
				break;
			}
		}
		else
		{
			MTH3D_M_vNormalizeVector(&stImposedAxis, &stImposedAxis);
			if  ( (uwSecondaryMode == 0) || (uwSecondaryMode == 1) )
			{

				MTH3D_M_vNegVector(&stImposedAxis, &stImposedAxis);
			}
		}

		if ( MTH3D_M_bIsNullVector(&stProposedAxis) )
		{
			switch ( uwSecondaryMode )
			{
			default:
				break;

			case 0:
			case 4:
				stProposedAxis = stK;
				break;

			case 1:
			case 3:
				stProposedAxis = stI;
				break;

			case 2:
			case 5:
				stProposedAxis = stJ;
				break;
			}
		}
		else
		{
			MTH3D_M_vNormalizeVector(&stProposedAxis, &stProposedAxis);
			if  ( (uwSecondaryMode == 2) || (uwSecondaryMode == 5) )
			{

				MTH3D_M_vNegVector(&stProposedAxis, &stProposedAxis);
			}
		}

		switch ( uwSecondaryMode )
		{
		default:
			break;

		case 5:

			MTH3D_M_vCrossProductVectorWithoutBuffer(&stTmp, &stImposedAxis, &stProposedAxis);
			if ( !MTH3D_M_bIsNullVector(&stTmp) )
			{
				MTH3D_M_vNormalizeVector(&stK, &stTmp);
			}

			MTH3D_M_vCrossProductVectorWithoutBuffer(&stJ, &stK, &stImposedAxis);
			break;

		case 4:

			MTH3D_M_vCrossProductVectorWithoutBuffer(&stTmp, &stProposedAxis, &stImposedAxis);
			if ( !MTH3D_M_bIsNullVector(&stTmp) )
			{
				MTH3D_M_vNormalizeVector(&stJ, &stTmp);
			}

			MTH3D_M_vCrossProductVectorWithoutBuffer(&stK, &stImposedAxis, &stJ);
			break;

		case 3:

			MTH3D_M_vCrossProductVectorWithoutBuffer(&stTmp, &stImposedAxis, &stProposedAxis);
			if ( !MTH3D_M_bIsNullVector(&stTmp) )
			{
				MTH3D_M_vNormalizeVector(&stJ, &stTmp);
			}

			MTH3D_M_vCrossProductVectorWithoutBuffer(&stI, &stJ, &stImposedAxis);
			break;

		case 2:

			MTH3D_M_vCrossProductVectorWithoutBuffer(&stTmp, &stProposedAxis, &stImposedAxis);
			if ( !MTH3D_M_bIsNullVector(&stTmp) )
			{
				MTH3D_M_vNormalizeVector(&stI, &stTmp);
			}

			MTH3D_M_vCrossProductVectorWithoutBuffer(&stJ, &stImposedAxis, &stI);
			break;

		case 1:

			MTH3D_M_vCrossProductVectorWithoutBuffer(&stTmp, &stProposedAxis, &stImposedAxis);
			if ( !MTH3D_M_bIsNullVector(&stTmp) )
			{
				MTH3D_M_vNormalizeVector(&stK, &stTmp);
			}

			MTH3D_M_vCrossProductVectorWithoutBuffer(&stI, &stImposedAxis, &stK);
			break;

		case 0:

			MTH3D_M_vCrossProductVectorWithoutBuffer(&stTmp, &stImposedAxis, &stProposedAxis);
			if ( !MTH3D_M_bIsNullVector(&stTmp) )
			{
				MTH3D_M_vNormalizeVector(&stI, &stTmp);
			}

			MTH3D_M_vCrossProductVectorWithoutBuffer(&stK, &stI, &stImposedAxis);
			break;
		}

		if ( uwSecondaryMode < 2 )
		{
			POS_fn_vSetRotationMatrix(hGLobalMovedMatrix, &stI, &stImposedAxis, &stK);
		}
		else if ( uwSecondaryMode < 4 )
		{
			POS_fn_vSetRotationMatrix(hGLobalMovedMatrix, &stI, &stJ, &stImposedAxis);
		}
		else
		{
			POS_fn_vSetRotationMatrix(hGLobalMovedMatrix, &stImposedAxis, &stJ, &stK);
		}
		if ( bTranslationChanged )
		{

			POS_fn_vMulMatrixVector(&stTmp, hGLobalMovedMatrix, &stGlobalTranslation);

			MTH3D_M_vSubVector(&stInducedTranslation, &stInducedTranslation, &stTmp);

			POS_fn_vGetTranslationVector(hGLobalMovedMatrix, &stGlobalTranslation);
			MTH3D_M_vAddVector(&stGlobalTranslation, &stGlobalTranslation, &stInducedTranslation);
			POS_fn_vSetTranslationVector(hGLobalMovedMatrix, &stGlobalTranslation);
		}
	}
	fn_vUpdateSuperObjectAfter3DChanges(
		hNewSuperObjPerso,
		hGLobalMovedMatrix,
		bTranslationChanged ? &stGlobalTranslation : NULL,
		(ACP_tdxBool) (uwChannel == C_ucUnknownChannel)
		);

	if (uwChannel != C_ucUnknownChannel)
	{

		fn_v3dDataSetAnimMatrixHasChanged(M_GetMSHandle(hCharSO,3dData));
	}

	return p_stTree;
}

AI_tdstNodeInterpret *__cdecl fn_p_stChangePersoHorizSighting(
    HIE_tdstSuperObject *p_SuperObjPerso, AI_tdstNodeInterpret *p_stTree)
{
    long                      eProcedureId = M_eProcedureIdInterpret(p_stTree-1);
    AI_tdstGetSetParam        stParam;
    MTH3D_tdstVector          stSightVector;
    MTH3D_tdstVector          stXvect, stZvect;
    MTH3D_tdstVector         *p_stTmpV0, *p_stTmpV1, *p_stTmpV2;
    POS_tdstCompletePosition *hGlobalMatrix;
    POS_tdstCompletePosition *hLocalMatrix;
    void                     *hDynam;
    unsigned long             ulCustomBits;
    HIE_tdstSuperObject      *hNewSuperObjPerso;

    fn_vGetUltraOperatorPerso(fn_ucGetProcedureUltraOperator(eProcedureId), p_SuperObjPerso, &hNewSuperObjPerso);

    M_EvalNextParameter(&stParam);
    stSightVector = M_GetSetParam_stVectorValue(&stParam);

    hGlobalMatrix = HIE_fn_hGetSuperObjectGlobalMatrix(hNewSuperObjPerso);
    hLocalMatrix  = HIE_fn_hGetSuperObjectMatrix(hNewSuperObjPerso);
    hDynam        = AI_3D_M_hDynam(hNewSuperObjPerso);

    stSightVector.xZ = 0.0f;

    if (MTH3D_M_bIsNullVector(&stSightVector))
    {
        POS_fn_vGetRotationVector(HIE_fn_hGetSuperObjectGlobalMatrix(hNewSuperObjPerso), &p_stTmpV0, &p_stTmpV1, &p_stTmpV2);
        MTH3D_M_vNegVector(&stSightVector, p_stTmpV1);
        stSightVector.xZ = 0.0f;
        if (MTH3D_M_bIsNullVector(&stSightVector))
            stSightVector = *p_stTmpV2;
    }

    {
        MTH3D_M_vMulScalarVector(&stSightVector, MTH_fn_fInvSquareRootOpt(MTH3D_M_xSqrVector(&stSightVector)), &stSightVector);
    }

    stXvect.xZ = 0.0f;
    MTH3D_M_vSetBaseKVector(&stZvect);

    stXvect.xY = stSightVector.xX;
    stSightVector.xX = MTH_M_xNeg(stSightVector.xX);
    stSightVector.xY = MTH_M_xNeg(stSightVector.xY);
    stXvect.xX = stSightVector.xY;

    POS_fn_vSetRotationMatrix(hGlobalMatrix, &stXvect, &stSightVector, &stZvect);

    HIE_fn_vValidateGlobalMatrix(hNewSuperObjPerso);

    if (HIE_M_xGetSuperObjectMember(HIE_M_hGetSuperObjectFather(hNewSuperObjPerso), ulTypeOfLinkedObject) & HIE_C_ulActor)
        AI_M_pfn_HIE_vComputeNewRelativeMatrix(hNewSuperObjPerso);
    else
        POS_fn_vSetRotationMatrix(hLocalMatrix, &stXvect, &stSightVector, &stZvect);

    if (((ulCustomBits = fn_ulStandardGameGetCustomBitsSO(hNewSuperObjPerso)) & GAM_C_CustBitNeedModuleMatrices)
        && (!hDynam || (ulCustomBits & GAM_C_CustBitNoMechanic)))
        AI_M_pfn_HIE_vRefreshHierarchy(hNewSuperObjPerso);

    if (hDynam != NULL)
        AI_M_pfn_MEC_vImposeRotationVector(AI_M_3D_pfn_DynamGetDynamics(hDynam),
                                           &stXvect, &stSightVector, &stZvect);
    return p_stTree;
}

/* 0x0046AB20 */
AI_tdstNodeInterpret *__cdecl fn_p_st3DProc(
    HIE_tdstSuperObject *p_SuperObjPerso, AI_tdstNodeInterpret *p_stTree)
{
	AI_tdstGetSetParam stParam;
	long eProcedureId = M_eProcedureIdInterpret(p_stTree - 1);
	HIE_tdstSuperObject *hNewSuperObjPerso = p_SuperObjPerso, *hChannelSuperObject = NULL, *hCharSO = p_SuperObjPerso;

	POS_tdstCompletePosition stPersoToWorldMatrix, *hGLobalMovedMatrix, *hPersoGlobalMatrix = NULL;
	MTH3D_tdstVector stPositionToReach, stTempVector, stGlobalTranslation;
	ACP_tdxBool bGlobalAxis = 0, bTranslationChanged = 0, bRotationChanged = 0, bScaleChanged = 0;
	unsigned char ucChannel = C_ucUnknownChannel;
	MTH_tdxReal xAngle = MTH_C_ZERO;
	MS_tdxHandleToDynam hDynam;
	unsigned long ulCustomBits;

	fn_vGetUltraOperatorPerso(fn_ucGetProcedureUltraOperator(eProcedureId),p_SuperObjPerso,&hNewSuperObjPerso);

	hCharSO = hNewSuperObjPerso;

	hPersoGlobalMatrix = HIE_fn_hGetSuperObjectGlobalMatrix(hNewSuperObjPerso);
	hDynam = M_GetMSHandle(hNewSuperObjPerso,Dynam);

	M_EvalNextParameter(&stParam);

	switch ( eProcedureId )
	{
	case 0xe1:
		ucChannel = (unsigned char)M_GetSetParam_lModuleValue(&stParam);
		if ( ucChannel == C_ucUnknownChannel )
		{
			return fn_p_stSwapNextParameter(p_stTree);
			break;
		}
		hChannelSuperObject = fn_hGetSuperObjectInChannel(fn_h3dDataGetChannelSOList(M_GetMSHandle(hNewSuperObjPerso,3dData)), ucChannel);

		M_EvalNextParameter(&stParam);

	case 0xa7:

		stPositionToReach = M_GetSetParam_stVectorValue(&stParam);

		M_EvalNextParameter(&stParam);
		xAngle = M_ReturnParam_xValue(&stParam);
		xAngle = MTH_M_xNeg(MTH_M_xDegToRad(xAngle));

		M_EvalNextParameter(&stParam);
		stGlobalTranslation = M_GetSetParam_stVectorValue(&stParam);

		bRotationChanged = 1;
		bGlobalAxis = 1;
		bTranslationChanged = 1;
		break;

	case 0xe0:
		ucChannel = (unsigned char)M_GetSetParam_lModuleValue(&stParam);
		if ( ucChannel == C_ucUnknownChannel )
		{
			return fn_p_stSwapNextParameter(p_stTree);
			break;
		}
		hChannelSuperObject = fn_hGetSuperObjectInChannel(fn_h3dDataGetChannelSOList(M_GetMSHandle(hNewSuperObjPerso,3dData)), ucChannel);

		M_EvalNextParameter(&stParam);

	case 0xa6:

		stPositionToReach = M_GetSetParam_stVectorValue(&stParam);

		M_EvalNextParameter(&stParam);
		xAngle = M_ReturnParam_xValue(&stParam);
		xAngle = -MTH_M_xDegToRad(xAngle);

		bRotationChanged = 1;
		bGlobalAxis = 1;
		break;

	case 0x9e:
		{

			p_SuperObjPerso	= M_GetSetParam_p_stSupObjValue(&stParam);

			if ( p_SuperObjPerso == hNewSuperObjPerso )
			{
				return fn_p_stSwapNextParameter(p_stTree);
				break;
			}

			M_EvalNextParameter(&stParam);
			ucChannel = (unsigned char)M_GetSetParam_lModuleValue(&stParam);
			if ( ucChannel == C_ucUnknownChannel )
			{
				return fn_p_stSwapNextParameter(p_stTree);
				break;
			}

			hChannelSuperObject = fn_hGetSuperObjectInChannel(fn_h3dDataGetChannelSOList(M_GetMSHandle(p_SuperObjPerso,3dData)), ucChannel);

			if ( !hChannelSuperObject )
			{
				return fn_p_stSwapNextParameter(p_stTree);
				break;
			}

			ucChannel = C_ucUnknownChannel;

			bTranslationChanged = 1;
			bRotationChanged = 1;
			bGlobalAxis = 1;
		}
		break;

	case 0xe7:

		ucChannel = (unsigned char)M_GetSetParam_lModuleValue(&stParam);
		if ( ucChannel == C_ucUnknownChannel )
		{
			return fn_p_stSwapNextParameter(p_stTree);
			break;
		}

		hChannelSuperObject = fn_hGetSuperObjectInChannel(fn_h3dDataGetChannelSOList(M_GetMSHandle(hNewSuperObjPerso,3dData)), ucChannel);

		M_EvalNextParameter(&stParam);

	case 0xad:

		stPositionToReach = M_GetSetParam_stVectorValue(&stParam);

		bGlobalAxis = 1;
		break;

	case 0xe2:
	case 0xe3:
	case 0xe4:

		ucChannel = (unsigned char)M_GetSetParam_lModuleValue(&stParam);
		if ( ucChannel == C_ucUnknownChannel )
		{
			return fn_p_stSwapNextParameter(p_stTree);
			break;
		}

		hChannelSuperObject = fn_hGetSuperObjectInChannel(fn_h3dDataGetChannelSOList(M_GetMSHandle(hNewSuperObjPerso,3dData)), ucChannel);

		M_EvalNextParameter(&stParam);

	case 0xa8:
	case 0xa9:
	case 0xaa:

		xAngle = M_ReturnParam_xValue(&stParam);
		xAngle = MTH_M_xDegToRad(xAngle);

		M_EvalNextParameter(&stParam);
		stPositionToReach = M_GetSetParam_stVectorValue(&stParam);

		bTranslationChanged = 1;
		bRotationChanged = 1;

		bGlobalAxis = 1;
		break;

	case 0xdd:
	case 0xde:
	case 0xdf:

		ucChannel = (unsigned char)M_GetSetParam_lModuleValue(&stParam);
		if ( ucChannel == C_ucUnknownChannel )
		{
			return fn_p_stSwapNextParameter(p_stTree);
			break;
		}

		hChannelSuperObject = fn_hGetSuperObjectInChannel(fn_h3dDataGetChannelSOList(M_GetMSHandle(hNewSuperObjPerso,3dData)), ucChannel);

		M_EvalNextParameter(&stParam);

	case 0xa3:
	case 0xa4:
	case 0xa5:

		xAngle = M_ReturnParam_xValue(&stParam);
		xAngle = MTH_M_xDegToRad(xAngle);

		bRotationChanged = 1;

		bGlobalAxis = 1;
		break;

	case 0x9d:
		stPositionToReach = M_GetSetParam_stVectorValue(&stParam);
		bTranslationChanged = 1;
		bGlobalAxis = 1;
		break;

	case 0xa2:
	case 0xa1:
		stPositionToReach = M_GetSetParam_stVectorValue(&stParam);
		if ( eProcedureId == 0xa2 )
		{
			M_EvalNextParameter(&stParam);
			stGlobalTranslation = M_GetSetParam_stVectorValue(&stParam);
			bTranslationChanged = 1;
		}
		bGlobalAxis = 1;
		bRotationChanged = 1;
		break;

	case 0xa0:
		stPositionToReach = M_GetSetParam_stVectorValue(&stParam);

		M_EvalNextParameter(&stParam);
		bGlobalAxis = (ACP_tdxBool)M_GetSetParam_lValue(&stParam);
		bTranslationChanged = 1;
		break;

	case 0xdc:
	case 0xd9:
	case 0xdb:
	case 0xda:
		{

			stPositionToReach = M_GetSetParam_stVectorValue(&stParam);

			M_EvalNextParameter(&stParam);
			ucChannel = (unsigned char)M_GetSetParam_lModuleValue(&stParam);
			if ( ucChannel == C_ucUnknownChannel )
			{
				return fn_p_stSwapNextParameter(p_stTree);
				break;
			}

			hChannelSuperObject = fn_hGetSuperObjectInChannel(fn_h3dDataGetChannelSOList(M_GetMSHandle(hNewSuperObjPerso,3dData)), ucChannel);

			M_EvalNextParameter(&stParam);
			bGlobalAxis = (ACP_tdxBool)M_GetSetParam_lValue(&stParam);

			if ( eProcedureId == 0xdc )
			{
				M_EvalNextParameter(&stParam);
				stGlobalTranslation = M_GetSetParam_stVectorValue(&stParam);
				bTranslationChanged = 1;
			}
		}
		break;

	default:
		return p_stTree;
		break;
	}

	if ( ucChannel != C_ucUnknownChannel )
	{

		if ( !hChannelSuperObject )
			return p_stTree;
		hNewSuperObjPerso = hChannelSuperObject;
	}

	hGLobalMovedMatrix = HIE_fn_hGetSuperObjectGlobalMatrix(hNewSuperObjPerso);

	if ( !bGlobalAxis )
	{

		POS_fn_vCopyMatrix(&stPersoToWorldMatrix, hPersoGlobalMatrix);
	}

	switch ( eProcedureId )
	{
	default:
		break;

		case 0xe1:
		case 0xa7:

			POS_fn_vMulMatrixVector(&stTempVector, hGLobalMovedMatrix, &stGlobalTranslation);

		case 0xe0:
		case 0xa6:
			MTH3D_M_vNormalizeVector(&stPositionToReach, &stPositionToReach);
			POS_fn_vRotatePositionAroundAxisNoTranslation(hGLobalMovedMatrix, &stPositionToReach, xAngle);

			if ( bTranslationChanged )
			{

				POS_fn_vMulMatrixVector(&stPositionToReach, hGLobalMovedMatrix, &stGlobalTranslation);

				MTH3D_M_vSubVector(&stPositionToReach, &stTempVector, &stPositionToReach);

				goto label_RelativeMovePerso;
			}

			goto label_UpdateRelativeMatrix;

		case 0x9e:

			POS_fn_vCopyMatrix(hGLobalMovedMatrix, HIE_fn_hGetSuperObjectGlobalMatrix(hChannelSuperObject));

			goto label_UpdateRelativeMatrix;

		case 0xad:

			bScaleChanged = 1;

		case 0xe7:

			if ( HIE_fn_SO_bHasNoTransformationMatrix(hNewSuperObjPerso) )
			{
				return p_stTree;
				break;
			}

			if ( HIE_fn_SO_bHasZoomInsteadOfScale(hNewSuperObjPerso) )
			{

				MTH3D_M_vSetYofVector(&stPositionToReach, MTH3D_M_xGetXofVector(&stPositionToReach));
				MTH3D_M_vSetZofVector(&stPositionToReach, MTH3D_M_xGetXofVector(&stPositionToReach));
			}

			if ( (ucChannel != C_ucUnknownChannel) || !hDynam )
			{
				MTH3D_tdstVector stV1, stV2, stV3;

				MTH3D_M_vSetVectorElements(&stV1, MTH3D_M_xGetXofVector(&stPositionToReach), MTH_C_ZERO, MTH_C_ZERO);
				MTH3D_M_vSetVectorElements(&stV2, MTH_C_ZERO, MTH3D_M_xGetYofVector(&stPositionToReach), MTH_C_ZERO);
				MTH3D_M_vSetVectorElements(&stV3, MTH_C_ZERO, MTH_C_ZERO, MTH3D_M_xGetZofVector(&stPositionToReach));

				POS_fn_vSetScaleMatrix(hGLobalMovedMatrix, &stV1, &stV2, &stV3);
			}

			goto label_UpdateRelativeMatrix;

		case 0xe2:
		case 0xe3:
		case 0xe4:
		case 0xa8:
		case 0xa9:
		case 0xaa:

			POS_fn_vMulMatrixVector(&stTempVector, hGLobalMovedMatrix, &stPositionToReach);

		case 0xdd:
		case 0xde:
		case 0xdf:
		case 0xa3:
		case 0xa4:
		case 0xa5:
			{

				MTH3D_tdstVector stV1, stV2, stV3;
				MTH_tdxReal xCos = MTH_M_xCos(xAngle);
				MTH_tdxReal xSin = MTH_M_xSin(xAngle);

				switch ( eProcedureId )
				{
				case 0xe2:
				case 0xa8:
				case 0xdd:
				case 0xa3:
					MTH3D_M_vSetVectorElements(&stV1, MTH_C_ONE, MTH_C_ZERO, MTH_C_ZERO);
					MTH3D_M_vSetVectorElements(&stV2, MTH_C_ZERO, xCos, xSin);
					MTH3D_M_vSetVectorElements(&stV3, MTH_C_ZERO, -xSin, xCos);
					break;

				case 0xde:
				case 0xa4:
				case 0xe3:
				case 0xa9:
					MTH3D_M_vSetVectorElements(&stV1, xCos, MTH_C_ZERO, xSin);
					MTH3D_M_vSetVectorElements(&stV2, MTH_C_ZERO, MTH_C_ONE, MTH_C_ZERO);
					MTH3D_M_vSetVectorElements(&stV3, -xSin, MTH_C_ZERO, xCos);
					break;

				case 0xdf:
				case 0xa5:
				case 0xe4:
				case 0xaa:
					MTH3D_M_vSetVectorElements(&stV1, xCos, xSin, MTH_C_ZERO);
					MTH3D_M_vSetVectorElements(&stV2, -xSin, xCos, MTH_C_ZERO);
					MTH3D_M_vSetVectorElements(&stV3, MTH_C_ZERO, MTH_C_ZERO, MTH_C_ONE);
					break;

				default:
					break;
				}

				POS_fn_vRotateVector(&stV1, hGLobalMovedMatrix, &stV1);
				POS_fn_vRotateVector(&stV2, hGLobalMovedMatrix, &stV2);
				POS_fn_vRotateVector(&stV3, hGLobalMovedMatrix, &stV3);

				POS_fn_vSetRotationMatrix(hGLobalMovedMatrix, &stV1, &stV2, &stV3);

				if ( bTranslationChanged )
				{

					POS_fn_vMulMatrixVector(&stPositionToReach, hGLobalMovedMatrix, &stPositionToReach);

					MTH3D_M_vSubVector(&stPositionToReach, &stTempVector, &stPositionToReach);

					goto label_RelativeMovePerso;
				}
			}

			goto label_UpdateRelativeMatrix;

		case 0xa2:
		case 0xdc:

			POS_fn_vMulMatrixVector(&M_GetSetParam_stVertexValue(&stParam), hGLobalMovedMatrix, &stGlobalTranslation);

		case 0xa1:
		case 0xd9:
			{
				MTH3D_tdstVector stI, stJ, stK;
				unsigned char b_ucKisUp, b_ucIisLeft;
				POS_fn_vGetRotationMatrix(hGLobalMovedMatrix, &stI, &stTempVector, &stK);

				if ( MTH3D_M_bIsNullVector(&stPositionToReach) )
					stJ = stTempVector;
				else
				{

					MTH3D_M_vNegVector(&stPositionToReach, &stPositionToReach);

					if ( !bGlobalAxis )
						POS_fn_vMulMatrixVector(&stPositionToReach, &stPersoToWorldMatrix, &stPositionToReach);

					MTH3D_M_vNormalizeVector(&stJ, &stPositionToReach);
				}

				if (MTH_M_bIsNullWithEpsilon(MTH3D_M_xGetXofVector(&stJ), AI_3D_xEpsilon) &&
					MTH_M_bIsNullWithEpsilon(MTH3D_M_xGetYofVector(&stJ), AI_3D_xEpsilon))
				{

					if (MTH_M_bIsNullWithEpsilon(MTH3D_M_xGetXofVector(&stTempVector), AI_3D_xEpsilon) &&
						MTH_M_bIsNullWithEpsilon(MTH3D_M_xGetYofVector(&stTempVector), AI_3D_xEpsilon))

						stTempVector = stK;

				}
				else
				{

					MTH3D_M_vSetVectorElements(&stTempVector, MTH_M_xNeg(MTH3D_M_xGetXofVector(&stJ)), MTH_M_xNeg(MTH3D_M_xGetYofVector(&stJ)), MTH_M_xSub(MTH3D_M_xGetZofVector(&stJ), MTH_M_xLongToReal(-2)));
				}

				b_ucKisUp = MTH_M_bGreater(MTH_M_xSign(MTH3D_M_xGetZofVector(&stK)), MTH_C_ZERO);
				b_ucIisLeft = MTH_M_bGreater(MTH_M_xSign(MTH3D_M_xGetXofVector(&stI)), MTH_C_ZERO);

				MTH3D_M_vCrossProductVectorWithoutBuffer(&stI, &stTempVector, &stJ);
				MTH3D_M_vSetZofVector(&stI, MTH_C_ZERO)

				MTH3D_M_vCrossProductVectorWithoutBuffer(&stK, &stI, &stJ);

				if ( MTH_M_bGreater(MTH_M_xSign(MTH3D_M_xGetZofVector(&stK)), MTH_C_ZERO) != b_ucKisUp )
					MTH3D_M_vNegVector(&stK, &stK);

				/* 0x0046B6C0 */
				MTH3D_M_vCrossProductVectorWithoutBuffer(&stI, &stJ, &stK);
				MTH3D_M_vNormalizeVector(&stI, &stI);
				MTH3D_M_vNormalizeVector(&stK, &stK);

				POS_fn_vSetRotationMatrix(hGLobalMovedMatrix, &stI, &stJ, &stK);

				if ( bTranslationChanged )
				{

					POS_fn_vMulMatrixVector(&stGlobalTranslation, hGLobalMovedMatrix, &stGlobalTranslation);

					MTH3D_M_vSubVector(&stPositionToReach, &M_GetSetParam_stVertexValue(&stParam), &stGlobalTranslation);

					goto label_RelativeMovePerso;
				}
			}

			goto label_UpdateRelativeMatrix;

		case 0xdb:
		case 0xa0:
label_RelativeMovePerso:

			if ( !bGlobalAxis )
				POS_fn_vMulMatrixVector(&stPositionToReach, &stPersoToWorldMatrix, &stPositionToReach);

			POS_fn_vGetTranslationVector(hGLobalMovedMatrix, &stTempVector);
			MTH3D_M_vAddVector(&stPositionToReach, &stPositionToReach, &stTempVector);

			goto label_SetPersoAbsolutePosition;

		case 0xda:

			if (!bGlobalAxis)

				POS_fn_vMulMatrixVertex(&stPositionToReach, &stPersoToWorldMatrix, &stPositionToReach);

		case 0x9d:
label_SetPersoAbsolutePosition:

			POS_fn_vGetTranslationVector(hGLobalMovedMatrix, &stGlobalTranslation);

			POS_fn_vSetTranslationVector(hGLobalMovedMatrix, &stPositionToReach);

			if
				(
				( (eProcedureId == 0x9d) || (eProcedureId == 0xa0) )
				&& hDynam
				)
				MEC_vImposeTranslation(fn_p_stDynamGetDynamics(hDynam),&stPositionToReach);

label_UpdateRelativeMatrix:

		HIE_fn_vValidateGlobalMatrix(hNewSuperObjPerso);

		HIE_fn_vComputeNewRelativeMatrix(hNewSuperObjPerso);

		if ((HIE_fn_ulGetSuperObjectType(hNewSuperObjPerso) != HIE_C_ulActor) ||
			(
				((ulCustomBits = fn_ulStandardGameGetCustomBitsSO(hNewSuperObjPerso)) & GAM_C_CustBitNeedModuleMatrices)
				&&
				(!hDynam || (ulCustomBits & GAM_C_CustBitNoMechanic))
			)
			)
		{
			HIE_fn_vRefreshHierarchy ( hNewSuperObjPerso ) ;
		}

		if ( ( ucChannel == C_ucUnknownChannel ) && hDynam )
		{
			if ( bScaleChanged )
			{
				DNM_tdstDynamics * pstDynamic = fn_p_stDynamGetDynamics ( hDynam ) ;
				if ( pstDynamic )
				{
						DNM_M_vDynamicsSetScale (
							pstDynamic ,
							MTH3D_M_xGetXofVector ( & stPositionToReach ) ,
							MTH3D_M_xGetYofVector ( & stPositionToReach ) ,
							MTH3D_M_xGetZofVector ( & stPositionToReach)
						);
					DNM_M_vDynamicsSetChangeScale ( pstDynamic , TRUE ) ;
				}
			}
			MEC_vImposeRotationMatrix(fn_p_stDynamGetDynamics(hDynam), hGLobalMovedMatrix);
		}
	}

	if (ucChannel != C_ucUnknownChannel)
	{

		fn_v3dDataSetAnimMatrixHasChanged(M_GetMSHandle(hCharSO,3dData));
	}

	return p_stTree;
}

/* 0x0046BB10 */
AI_tdstNodeInterpret *__cdecl fn_p_stWorldMapProc(
    HIE_tdstSuperObject *p_SuperObjPerso, AI_tdstNodeInterpret *p_stTree)
{
    long                  eProcedureId = M_eProcedureIdInterpret(p_stTree-1);
    AI_tdstGetSetParam    stParam;

    switch (eProcedureId)
    {
    case 0x159:
        p_stTree = fn_p_stEvalTree(p_SuperObjPerso, p_stTree, &stParam);
        GAM_g_ucLevelGlobalCounter = stParam.uParam.ucValue;
        break;

    case 0x158:
    {
        unsigned char ucVarId;

        ucVarId  = (unsigned char)M_ucVarIdInterpret(p_stTree);
        p_stTree ++;

        if (AI_GES_xDemoMode != 0)
        {
            AI_GES_ucPreviousLevel = 0;
            M_Full_GetSetParam_Integer(&stParam, AI_GES_ucPreviousLevel);
        }
        else
            M_Full_GetSetParam_Integer(&stParam, AI_GES_ucPreviousLevel);

        ucVarId = fn_ucSetDsgVar(ucVarId, 0, AI_M_p_stMindRaw(p_SuperObjPerso), &stParam);

        ucVarId  = (unsigned char)M_ucVarIdInterpret(p_stTree);
        p_stTree ++;
        if (AI_GES_xDemoMode != 0)
        {
            AI_GES_ucExitIdToQuitPrevLevel = 0;
            M_Full_GetSetParam_Integer(&stParam, AI_GES_ucExitIdToQuitPrevLevel);
        }
        else
            M_Full_GetSetParam_Integer(&stParam, AI_GES_ucExitIdToQuitPrevLevel);

        ucVarId = fn_ucSetDsgVar(ucVarId, 0, AI_M_p_stMindRaw(p_SuperObjPerso), &stParam);
        break;
    }

    case 0x157:
        p_stTree = fn_p_stEvalTree(p_SuperObjPerso, p_stTree, &stParam);
        if (AI_GES_xDemoMode == 0)
            AI_GES_ucPreviousLevel = stParam.uParam.ucValue;

        p_stTree = fn_p_stEvalTree(p_SuperObjPerso, p_stTree, &stParam);
        if (AI_GES_xDemoMode == 0)
            AI_GES_ucExitIdToQuitPrevLevel = stParam.uParam.ucValue;
        break;

    default:
        break;
    }
    return p_stTree;
}

/* 0x0046BC60 */
AI_tdstNodeInterpret *__cdecl fn_p_stOptionChangerProc(
    HIE_tdstSuperObject *p_SuperObjPerso, AI_tdstNodeInterpret *p_stTree)
{
    AI_tdstGetSetParam    stParam;
    long                  eProcedureId = M_eProcedureIdInterpret(p_stTree-1);

    M_EvalNextParameter(&stParam);

    switch (eProcedureId)
    {
    case 0x9c:
        {
            void *IPT_xHandleToChange, *IPT_xHandle;

            IPT_xHandleToChange = (void *) M_GetSetParam_lValue(&stParam);
            M_EvalNextParameter(&stParam);
            IPT_xHandle = (void *) M_GetSetParam_lValue(&stParam);
            AI_M_pfn_IPT_fn_bModifyIPTAction(IPT_xHandleToChange, IPT_xHandle);
        }
        break;

    case 0x9b:
        {

            void *IPT_xHandle = (void *) M_GetSetParam_lValue(&stParam);
            AI_M_pfn_IPT_fn_bChangeIPTValue(IPT_xHandle, AI_IPT_C_Exchange);
        }
        break;

    case 0x97:
        AI_M_pfn_fn_vChangeDetailsValue(M_GetSetParam_lValue(&stParam));
        break;

    case 0x98:
        AI_M_pfn_fn_vChangeMusicVolumeValue(M_GetSetParam_lValue(&stParam));
        break;

    case 0x99:
        AI_M_pfn_fn_vChangeFxVolumeValue(M_GetSetParam_lValue(&stParam));
        break;

    case 0x9a:
        break;

    default:
        break;
    }
    return p_stTree;
}

/* 0x0046BD40 */
AI_tdstNodeInterpret *__cdecl fn_p_stChangeScreen(
    HIE_tdstSuperObject *p_SuperObjPerso, AI_tdstNodeInterpret *p_stTree)
{
	AI_tdstGetSetParam			stParam;
	unsigned long				ulMode;
	long						lVal1, lVal2, lVal3, lVal4;
	long						eProcedureId = M_eProcedureIdInterpret(p_stTree - 1);
	unsigned long				ulChangeScreenPositionInPercent = 0;

	M_EvalNextParameter(&stParam);
	ulMode = M_GetSetParam_lValue(&stParam);

	switch ( eProcedureId )
	{
	case 0x13a:
		M_EvalNextParameter(&stParam);
		lVal1 = M_GetSetParam_lValue(&stParam);
		M_EvalNextParameter(&stParam);
		lVal2 = M_GetSetParam_lValue(&stParam);
		M_EvalNextParameter(&stParam);
		lVal3 = M_GetSetParam_lValue(&stParam);
		M_EvalNextParameter(&stParam);
		lVal4 = M_GetSetParam_lValue(&stParam);
		fn_vChangeViewPortPercent(ulMode, 2, 0, 0, lVal1, lVal2, lVal3, lVal4, 0, 0);
		break;

	case 0x139:
		M_EvalNextParameter(&stParam);
		lVal1 = M_GetSetParam_lValue(&stParam);
		M_EvalNextParameter(&stParam);
		lVal2 = M_GetSetParam_lValue(&stParam);

		if(lVal1 < 0) lVal1 = 0;
		if(lVal1 > 100) lVal1 = 100;
		if(lVal2 < 0) lVal2 = 0;
		if(lVal2 > 100) lVal2 = 100;
		fn_vChangeViewPortPercent(ulMode, 1, lVal1, lVal2, 0, 0, 0, 0, 0, 0);
		break;

	case 0x13c:
		ulChangeScreenPositionInPercent = 1;

	case 0x13b:
		M_EvalNextParameter(&stParam);
		lVal1 = (int) M_GetSetParam_lValue(&stParam);
		M_EvalNextParameter(&stParam);
		lVal2 = (int) M_GetSetParam_lValue(&stParam);
		fn_vChangeViewPortPercent(ulMode, 3 + ulChangeScreenPositionInPercent, 0, 0, 0, 0, 0, 0, (int) lVal1, (int) lVal2);
		break;

	case 0x13d:
		M_EvalNextParameter(&stParam);
		AI_M_pfn_fn_vChangeViewportRatio(ulMode, M_ReturnParam_xValue(&stParam));
		break;

	default:
		break;
	}
	return p_stTree;
}

AI_tdstNodeInterpret *__cdecl fn_p_stFixePositionPerso(
    HIE_tdstSuperObject *p_SuperObjPerso, AI_tdstNodeInterpret *p_stTree)
{
  AI_tdstGetSetParam        stParam;
  HIE_tdstSuperObject      *hPerso;
  MTH3D_tdstVector         *p_stVector;
  MS_tdxHandleToDynam       h_Dynam;
  void                     *p_stDynamics;
  POS_tdstCompletePosition *hGlobalMatrix;

  long eProcedureId = M_eProcedureIdInterpret(p_stTree-1);

  M_EvalNextParameter( &stParam );
  hPerso = (HIE_tdstSuperObject *) M_GetSetParam_p_stSupObjValue( &stParam );

  M_EvalNextParameter( &stParam );
  p_stVector = &M_GetSetParam_stVertexValue( &stParam );

  switch(eProcedureId)
  {
  case 0xc8:
    {
      h_Dynam = M_GetMSHandle(hPerso,Dynam);

      if ( h_Dynam )
      {
        p_stDynamics=fn_p_stDynamGetDynamics(h_Dynam);
        MEC_vInitTranslation( p_stDynamics, hPerso, p_stVector ) ;
      }
      else
      {
        hGlobalMatrix = HIE_fn_hGetSuperObjectGlobalMatrix (hPerso);
        POS_fn_vSetTranslationVector ( hGlobalMatrix, p_stVector );
        HIE_fn_vComputeNewRelativeMatrix (hPerso);
      }

      return p_stTree;
    }

  default:
    {

      M_AIFatalError(0x28);
      return(p_stTree);
    }
  }
}

/* 0x0046BFE0 */
AI_tdstNodeInterpret *__cdecl fn_p_stFixePositionZDx(
    HIE_tdstSuperObject *p_SuperObjPerso, AI_tdstNodeInterpret *p_stTree)
{
    AI_tdstGetSetParam    stParam;
    void                 *hPerso;
    MTH3D_tdstVector     *p_stVector;
    MTH3D_tdstVector      stVectorMin, stVectorMax, stTmpVector;
    short                 xMaxPoint, xMinPoint;
    void                 *hGeoObj;
    long                  lZoneNumber;
    long                  eProcedureId = M_eProcedureIdInterpret(p_stTree-1);

    p_stTree = fn_p_stEvalTree(p_SuperObjPerso, p_stTree, &stParam);
    hPerso   = AI_M_pfn_fn_h_StandardGameGetSuperObject(
                   stParam.uParam.p_stEngObjValue->h_StandardGame);

    p_stTree = fn_p_stEvalTree(p_SuperObjPerso, p_stTree, &stParam);
    lZoneNumber = stParam.uParam.lValue;

    p_stTree = fn_p_stEvalTree(p_SuperObjPerso, p_stTree, &stParam);
    p_stVector = (MTH3D_tdstVector *)stParam.uParam.a_xVectorValue;

    switch (eProcedureId)
    {
    case 0x133:
        hGeoObj = AI_M_pfn_fn_hGetGeometricZdxOfTypeAtIndex(1, (unsigned short)(lZoneNumber - 1), hPerso);
        break;

    case 0x134:
        hGeoObj = AI_M_pfn_fn_hGetGeometricZdxOfTypeAtIndex(0, (unsigned short)(lZoneNumber - 1), hPerso);
        break;

    case 0x135:
        hGeoObj = AI_M_pfn_fn_hGetGeometricZdxOfTypeAtIndex(2, (unsigned short)(lZoneNumber - 1), hPerso);
        break;

    default:

        AI_M_pfn_M_AIFatalError(0x28);
        return p_stTree;
    }

    if (hGeoObj)
    {
        switch (*((GEO_tdstGeometricObject *)hGeoObj)->d_xListOfElementsTypes)
        {
        case 5:
        case 7:

            GEO_vSetPointOfObject(hGeoObj, p_stVector, 0);
            GEO_vEndModifyObject2(hGeoObj);
            break;

        case 8:
        case 9:

            GEO_vGetMinMaxPointOfIndexedAlignedBox(hGeoObj, 0, 0, &xMinPoint, &xMaxPoint);

            GEO_vGetPointOfObject(hGeoObj, &stVectorMin, xMinPoint);
            GEO_vGetPointOfObject(hGeoObj, &stVectorMax, xMaxPoint);

            MTH3D_M_vSubVector(&stTmpVector, p_stVector, &stVectorMin);

            MTH3D_M_vAddVector(&stVectorMin, &stVectorMin, &stTmpVector);
            MTH3D_M_vAddVector(&stVectorMax, &stVectorMax, &stTmpVector);

            GEO_vSetPointOfObject(hGeoObj, &stVectorMin, xMinPoint);
            GEO_vSetPointOfObject(hGeoObj, &stVectorMax, xMaxPoint);

            GEO_vEndModifyObject2(hGeoObj);
            break;
        }
    }
    return p_stTree;
}

/* 0x0046C1D0 */
AI_tdstNodeInterpret *__cdecl fn_p_SetMechanicOption(
    HIE_tdstSuperObject *p_SuperObjPerso, AI_tdstNodeInterpret *p_stTree)
{
	AI_tdstGetSetParam stValue;
	DNM_tdstDynamics* pstDynamic;
	MS_tdxHandleToDynam h_Dynam;
	HIE_tdstSuperObject *hNewSuperObjPerso;
	long eProcedureId=M_eProcedureIdInterpret(p_stTree-1);

	fn_vGetUltraOperatorPerso(fn_ucGetProcedureUltraOperator(eProcedureId),p_SuperObjPerso,&hNewSuperObjPerso);

	M_EvalNextParameter(&stValue);

	h_Dynam = M_GetMSHandle(hNewSuperObjPerso,Dynam);

	if (!h_Dynam)
	{
		return(p_stTree);
	}

	pstDynamic = fn_p_stDynamGetDynamics(h_Dynam);

	switch (eProcedureId)
	{

	case 0xf4:
		DNM_M_vDynamicsSetAnimation (pstDynamic,(BOOL) M_GetSetParam_lValue(&stValue)?TRUE:FALSE);
		break;

	case 0xf5:
		DNM_M_vDynamicsSetCollide (pstDynamic,(BOOL) M_GetSetParam_lValue(&stValue)?TRUE:FALSE);

		if (M_GetSetParam_lValue(&stValue))
			DNM_M_vDynamicsSetCollisionTranslation ( pstDynamic, POS_fn_p_stGetTranslationVector(DNM_M_p_stDynamicsGetCurrentMatrix(pstDynamic)) );
		break;

	case 0xf6:
		DNM_M_vDynamicsSetGravity (pstDynamic,(BOOL) M_GetSetParam_lValue(&stValue)?TRUE:FALSE);
		break;

	case 0xf7:
		DNM_M_vDynamicsSetTilt (pstDynamic,(BOOL) M_GetSetParam_lValue(&stValue)?TRUE:FALSE);
		break;

	case 0xf8:
		DNM_M_vDynamicsSetGi (pstDynamic,(BOOL) M_GetSetParam_lValue(&stValue)?TRUE:FALSE);
		break;

	case 0xf9:
		DNM_M_vDynamicsSetClimb (pstDynamic,(BOOL) M_GetSetParam_lValue(&stValue)?TRUE:FALSE);
		break;

	case 0xfa:
		DNM_M_vDynamicsSetOnGround (pstDynamic,(BOOL) M_GetSetParam_lValue(&stValue)?TRUE:FALSE);
		break;

	case 0xfb:
		DNM_M_vDynamicsSetSpider (pstDynamic,(BOOL) M_GetSetParam_lValue(&stValue)?TRUE:FALSE);
		break;

	case 0xfc:
		DNM_M_vDynamicsSetShoot (pstDynamic,(BOOL) M_GetSetParam_lValue(&stValue)?TRUE:FALSE);
		break;

	case 0xfd:
		DNM_M_vDynamicsSetSwim (pstDynamic,(BOOL) M_GetSetParam_lValue(&stValue)?TRUE:FALSE);
		break;

	case 0xfe:
		DNM_M_vDynamicsSetNeverFall (pstDynamic,(BOOL) M_GetSetParam_lValue(&stValue)?TRUE:FALSE);
		break;

	case 0x100:
		DNM_M_vDynamicsSetCollisionControl (pstDynamic,(BOOL) M_GetSetParam_lValue(&stValue)?TRUE:FALSE);
		break;

	case 0x101:
		DNM_M_vDynamicsSetKeepWallSpeedZ (pstDynamic,(BOOL) M_GetSetParam_lValue(&stValue)?TRUE:FALSE);
		break;

	case 0x102:
		DNM_M_vDynamicsSetSpeedLimit (pstDynamic,(BOOL) M_GetSetParam_lValue(&stValue)?TRUE:FALSE);
		break;

	case 0x103:
		DNM_M_vDynamicsSetInertia (pstDynamic,(BOOL) M_GetSetParam_lValue(&stValue)?TRUE:FALSE);
		break;

	case 0x104:
		DNM_M_vDynamicsSetStream (pstDynamic,(BOOL) M_GetSetParam_lValue(&stValue)?TRUE:FALSE);
		break;

	case 0x105:
		DNM_M_vDynamicsSetStickOnPlatform (pstDynamic,(BOOL) M_GetSetParam_lValue(&stValue)?TRUE:FALSE);
		break;

	case 0x106:
		DNM_M_vDynamicsSetScaleOption (pstDynamic,(BOOL) M_GetSetParam_lValue(&stValue)?TRUE:FALSE);
		break;

	case 0xff:
		DNM_M_vDynamicsSetDontUseNewMechanic (pstDynamic,(BOOL) M_GetSetParam_lValue(&stValue)?TRUE:FALSE);
		break;

	default:
		break;
	}

	return p_stTree;
}

/* 0x0046C640 */
AI_tdstNodeInterpret *__cdecl fn_p_stChangeLigthIntensity(
    HIE_tdstSuperObject *p_SuperObjPerso, AI_tdstNodeInterpret *p_stTree)
{
    unsigned long         procId = (unsigned long)M_eProcedureIdInterpret(p_stTree-1);
    AI_tdstGetSetParam    stParam;
    void                 *h_MSLight;
    AI_tdstNodeInterpret *p_stNext;

    p_stNext  = fn_p_stEvalTree(p_SuperObjPerso, p_stTree, &stParam);
    h_MSLight = stParam.uParam.p_stEngObjValue->h_MSLight;
    if (h_MSLight != NULL && procId == 0x136)
    {
        float xR, xG, xB, xStep;
        p_stNext = fn_p_stEvalTree(p_SuperObjPerso, p_stNext, &stParam); xR    = stParam.uParam.xValue;
        p_stNext = fn_p_stEvalTree(p_SuperObjPerso, p_stNext, &stParam); xG    = stParam.uParam.xValue;
        p_stNext = fn_p_stEvalTree(p_SuperObjPerso, p_stNext, &stParam); xB    = stParam.uParam.xValue;
        p_stNext = fn_p_stEvalTree(p_SuperObjPerso, p_stNext, &stParam); xStep = stParam.uParam.xValue;
        fn_vMSLightSetColorRGBA(h_MSLight, xR * xStep, xG * xStep, xB * xStep, 0.0f);
    }
    return p_stNext;
}

/* 0x0046C710 */
AI_tdstNodeInterpret *__cdecl fn_p_SetMechanicRealParameter(
    HIE_tdstSuperObject *p_SuperObjPerso, AI_tdstNodeInterpret *p_stTree)
{
    AI_tdstGetSetParam    stValue;
    DNM_tdstDynamics     *pstDynamic;
    void                 *h_Dynam;
    HIE_tdstSuperObject  *hNewSuperObjPerso;
    MTH_tdxReal           xValue;
    long                  eProcedureId = M_eProcedureIdInterpret(p_stTree-1);

    fn_vGetUltraOperatorPerso(fn_ucGetProcedureUltraOperator(eProcedureId),p_SuperObjPerso,&hNewSuperObjPerso);

    M_EvalNextParameter(&stValue);
    xValue = M_GetSetParam_xValue (&stValue);

    h_Dynam = M_GetMSHandle(hNewSuperObjPerso,Dynam);

    if (!h_Dynam)
    {

        return(p_stTree);
    }

    pstDynamic = AI_M_pfn_fn_p_stDynamGetDynamics(h_Dynam);

    switch (eProcedureId)
    {

    case 0x107 :
        DNM_M_vDynamicsSetGravityFactor (pstDynamic,xValue);
        break;

    case 0x108 :
        DNM_M_vDynamicsSetSlide (pstDynamic,xValue);
        break;

    case 0x109 :
        DNM_M_vDynamicsSetRebound (pstDynamic,xValue);
        break;

    case 0x10a :
        DNM_M_vDynamicsSetSlopeLimit ( pstDynamic, MTH_M_xTan (MTH_M_xDegToRad (xValue)) );
        break;

    case 0x10b :
        DNM_M_vDynamicsSetInertiaFactorX (pstDynamic,xValue);
        break;

    case 0x10c :
        DNM_M_vDynamicsSetInertiaFactorY (pstDynamic,xValue);
        break;

    case 0x10d :
        DNM_M_vDynamicsSetInertiaFactorZ (pstDynamic,xValue);
        break;

    case 0x10f :
        DNM_M_vDynamicsSetTiltIntensity (pstDynamic,xValue);
        break;

    case 0x110 :
        DNM_M_vDynamicsSetTiltInertia (pstDynamic,xValue);
        break;

    case 0x111 :
        DNM_M_vDynamicsSetTiltOrigin (pstDynamic,xValue);
        break;

    case 0x113 :
        DNM_M_vDynamicsSetStreamPriority (pstDynamic,xValue);
        break;

    case 0x115 :
        DNM_M_vDynamicsSetStreamFactor (pstDynamic,xValue);
        break;

    case 0x125 :
        DNM_M_vDynamicsSetSlideFactorX (pstDynamic,xValue);
        break;

    case 0x126 :
        DNM_M_vDynamicsSetSlideFactorY (pstDynamic,xValue);
        break;

    case 0x127 :
        DNM_M_vDynamicsSetSlideFactorZ (pstDynamic,xValue);
        break;

    case 0x129 :
        DNM_M_vDynamicsSetBitInEndFlags_(pstDynamic, DNM_C_Hanging, TRUE);
        DNM_M_vDynamicsSetCollide (pstDynamic,TRUE);
        DNM_M_vDynamicsSetKeepWallSpeedZ (pstDynamic,TRUE);
        DNM_M_vDynamicsSetCollisionControl (pstDynamic,TRUE);
        DNM_M_vDynamicsSetSlide (pstDynamic,MTH_C_ONE);
        DNM_M_vDynamicsSetRebound (pstDynamic,MTH_C_ZERO);

        DNM_fn_vSetHangingLimit (pstDynamic,xValue);
        break;

    case 0x12b : AI_DAT_xGlobalMecReal0 = xValue; break;
    case 0x12c : AI_DAT_xGlobalMecReal1 = xValue; break;

    default:
        break;
    }
    return p_stTree;
}

/* 0x0046C980 */
AI_tdstNodeInterpret *__cdecl fn_p_SetMechanic3RealParameter(
    HIE_tdstSuperObject *p_SuperObjPerso, AI_tdstNodeInterpret *p_stTree)
{
    AI_tdstGetSetParam    stValue;
    MTH_tdxReal           xValue1, xValue2, xValue3;
    DNM_tdstDynamics     *pstDynamic;
    void                 *h_Dynam;
    HIE_tdstSuperObject  *hNewSuperObjPerso;
    long                  eProcedureId = M_eProcedureIdInterpret(p_stTree-1);

    fn_vGetUltraOperatorPerso(fn_ucGetProcedureUltraOperator(eProcedureId),p_SuperObjPerso,&hNewSuperObjPerso);

    M_EvalNextParameter(&stValue);
    xValue1 = M_GetSetParam_xValue (&stValue);
    M_EvalNextParameter(&stValue);
    xValue2 = M_GetSetParam_xValue (&stValue);
    M_EvalNextParameter(&stValue);
    xValue3 = M_GetSetParam_xValue (&stValue);

    h_Dynam = M_GetMSHandle(hNewSuperObjPerso,Dynam);

    if (!h_Dynam)
    {
        return(p_stTree);
    }

    pstDynamic = AI_M_pfn_fn_p_stDynamGetDynamics(h_Dynam);

    switch (eProcedureId)
    {
    case 0x10e :
        DNM_M_vDynamicsSetInertiaFactorX (pstDynamic,xValue1);
        DNM_M_vDynamicsSetInertiaFactorY (pstDynamic,xValue2);
        DNM_M_vDynamicsSetInertiaFactorZ (pstDynamic,xValue3);
        break;

    default:
        break;
    }

    return p_stTree;
}

/* 0x0046CA40 */
AI_tdstNodeInterpret *__cdecl fn_p_SetMechanicVectorParameter(
    HIE_tdstSuperObject *p_SuperObjPerso, AI_tdstNodeInterpret *p_stTree)
{
	AI_tdstGetSetParam stValue;
	MTH3D_tdstVector *p_stVector, stVector;
	DNM_tdstDynamics* pstDynamic;
	MS_tdxHandleToDynam h_Dynam;
	HIE_tdstSuperObject *hNewSuperObjPerso;
	long eProcedureId=M_eProcedureIdInterpret(p_stTree-1);

	fn_vGetUltraOperatorPerso(fn_ucGetProcedureUltraOperator(eProcedureId),p_SuperObjPerso,&hNewSuperObjPerso);

	M_EvalNextParameter(&stValue);
	p_stVector = &M_GetSetParam_stVertexValue(&stValue);

	h_Dynam = M_GetMSHandle(hNewSuperObjPerso,Dynam);

	if (!h_Dynam)
	{
		return(p_stTree);
	}

	pstDynamic = fn_p_stDynamGetDynamics(h_Dynam);

	switch (eProcedureId)
	{
	case 0x112:
		{
			MTH_tdxReal xTmp;

			stVector = *DNM_M_pDynamicsGetMaxSpeed(pstDynamic);

			xTmp = MTH3D_M_xGetXofVector(p_stVector);
			if ( MTH_M_bGreaterZero(xTmp) )
				MTH3D_M_vSetXofVector(&stVector, xTmp);
			xTmp = MTH3D_M_xGetYofVector(p_stVector);
			if ( MTH_M_bGreaterZero(xTmp) )
				MTH3D_M_vSetYofVector(&stVector, xTmp);
			xTmp = MTH3D_M_xGetZofVector(p_stVector);
			if ( MTH_M_bGreaterZero(xTmp) )
				MTH3D_M_vSetZofVector(&stVector, xTmp);

			DNM_M_vDynamicsSetMaxSpeed (pstDynamic,&stVector);
		}
		break;

	case 0x114:
		DNM_M_vDynamicsSetStreamSpeed (pstDynamic,p_stVector);
		break;

	case 0x116:
		MTH3D_M_vAddVector ( &stVector, DNM_M_pDynamicsGetStreamSpeed (pstDynamic), p_stVector );
		DNM_M_vDynamicsSetStreamSpeed (pstDynamic,&stVector);
		break;

	default:
		break;
	}

	return p_stTree;
}

/* 0x0046CBD0 */
AI_tdstNodeInterpret *__cdecl fn_p_SetMechanicSpeedVector(
    HIE_tdstSuperObject *p_SuperObjPerso, AI_tdstNodeInterpret *p_stTree)
{
    AI_tdstGetSetParam    stValue;
    MTH3D_tdstVector     *p_stVector;
    DNM_tdstDynamics     *pstDynamic;
    void                 *h_Dynam;
    HIE_tdstSuperObject  *hNewSuperObjPerso;
    long                  eProcedureId = M_eProcedureIdInterpret(p_stTree-1);

    fn_vGetUltraOperatorPerso(fn_ucGetProcedureUltraOperator(eProcedureId),p_SuperObjPerso,&hNewSuperObjPerso);

    M_EvalNextParameter(&stValue);
    p_stVector = &M_GetSetParam_stVertexValue(&stValue);

    h_Dynam = M_GetMSHandle(hNewSuperObjPerso,Dynam);

    if (!h_Dynam)
    {
        return(p_stTree);
    }

    pstDynamic = AI_M_pfn_fn_p_stDynamGetDynamics(h_Dynam);

    switch (eProcedureId)
    {

    case 0xbc :
        DNM_M_bDynamicsSetAbsoluteProposeSpeed (pstDynamic, FALSE );
        DNM_M_vDynamicsSetProposeSpeedX (pstDynamic,TRUE);
        DNM_M_vDynamicsSetProposeSpeedY (pstDynamic,TRUE);
        DNM_M_vDynamicsSetProposeSpeedZ (pstDynamic,TRUE);
        DNM_M_vDynamicsSetProposeSpeed (pstDynamic,p_stVector);
        break;

    case 0xb0 :
        DNM_M_bDynamicsSetAbsoluteImposeSpeed (pstDynamic, FALSE );
        DNM_M_vDynamicsSetImposeSpeedX (pstDynamic,TRUE);
        DNM_M_vDynamicsSetImposeSpeedY (pstDynamic,TRUE);
        DNM_M_vDynamicsSetImposeSpeedZ (pstDynamic,TRUE);
        DNM_M_vDynamicsSetImposeSpeed (pstDynamic,p_stVector);
        break;

    case 0xc9 :
        DNM_M_bDynamicsSetAbsoluteAddSpeed (pstDynamic, FALSE );
        DNM_M_vDynamicsSetAddSpeedX (pstDynamic,TRUE);
        DNM_M_vDynamicsSetAddSpeedY (pstDynamic,TRUE);
        DNM_M_vDynamicsSetAddSpeedZ (pstDynamic,TRUE);
        DNM_M_vDynamicsSetAddSpeed (pstDynamic,p_stVector);
        break;

    case 0xc2 :
        DNM_M_bDynamicsSetAbsoluteProposeSpeed (pstDynamic, TRUE );
        DNM_M_vDynamicsSetProposeSpeedX (pstDynamic,TRUE);
        DNM_M_vDynamicsSetProposeSpeedY (pstDynamic,TRUE);
        DNM_M_vDynamicsSetProposeSpeedZ (pstDynamic,TRUE);
        DNM_M_vDynamicsSetProposeSpeed (pstDynamic,p_stVector);
        break;

    case 0xb6 :
        DNM_M_bDynamicsSetAbsoluteImposeSpeed (pstDynamic, TRUE );
        DNM_M_vDynamicsSetImposeSpeedX (pstDynamic,TRUE);
        DNM_M_vDynamicsSetImposeSpeedY (pstDynamic,TRUE);
        DNM_M_vDynamicsSetImposeSpeedZ (pstDynamic,TRUE);
        DNM_M_vDynamicsSetImposeSpeed (pstDynamic,p_stVector);
        break;

    case 0xcf :
        DNM_M_bDynamicsSetAbsoluteAddSpeed (pstDynamic, TRUE );
        DNM_M_vDynamicsSetAddSpeedX (pstDynamic,TRUE);
        DNM_M_vDynamicsSetAddSpeedY (pstDynamic,TRUE);
        DNM_M_vDynamicsSetAddSpeedZ (pstDynamic,TRUE);
        DNM_M_vDynamicsSetAddSpeed (pstDynamic,p_stVector);
        break;

    case 0x118 :

        DNM_M_vDynamicsSetBitInFlags_(pstDynamic, DNM_C_LimitX, TRUE);
        DNM_M_vDynamicsSetBitInFlags_(pstDynamic, DNM_C_LimitY, TRUE);
        DNM_M_vDynamicsSetBitInFlags_(pstDynamic, DNM_C_LimitZ, TRUE);
        MTH3D_M_vCopyVector(DNM_M_pDynamicsGetLimit(pstDynamic), p_stVector);
        break;

    default:
        break;
    }

    return p_stTree;
}

/* 0x0046CE00 */
AI_tdstNodeInterpret *__cdecl fn_p_SetMechanicSpeedReal(
    HIE_tdstSuperObject *p_SuperObjPerso, AI_tdstNodeInterpret *p_stTree)
{
    AI_tdstGetSetParam    stValue;
    DNM_tdstDynamics     *pstDynamic;
    void                 *h_Dynam;
    HIE_tdstSuperObject  *hNewSuperObjPerso;
    MTH_tdxReal           xValueX, xValueY, xValueZ;
    long                  eProcedureId = M_eProcedureIdInterpret(p_stTree-1);

    fn_vGetUltraOperatorPerso(fn_ucGetProcedureUltraOperator(eProcedureId),p_SuperObjPerso,&hNewSuperObjPerso);

    h_Dynam = M_GetMSHandle(hNewSuperObjPerso,Dynam);

    if (!h_Dynam)
    {
        return(p_stTree);
    }

    pstDynamic = AI_M_pfn_fn_p_stDynamGetDynamics(h_Dynam);

    switch (eProcedureId)
    {

    case 0xb1 :
        M_EvalNextParameter(&stValue);
        xValueX = M_GetSetParam_xValue (&stValue);
        DNM_M_bDynamicsSetAbsoluteImposeSpeed (pstDynamic, FALSE );
        DNM_M_vDynamicsSetImposeSpeedX (pstDynamic,TRUE);
        MTH3D_M_vSetXofVector (DNM_M_pDynamicsGetImposeSpeed (pstDynamic),xValueX);
        break;

    case 0xb2 :
        M_EvalNextParameter(&stValue);
        xValueY = M_GetSetParam_xValue (&stValue);
        DNM_M_bDynamicsSetAbsoluteImposeSpeed (pstDynamic, FALSE );
        DNM_M_vDynamicsSetImposeSpeedY (pstDynamic,TRUE);
        MTH3D_M_vSetYofVector (DNM_M_pDynamicsGetImposeSpeed (pstDynamic),xValueY);
        break;

    case 0xb3 :
        M_EvalNextParameter(&stValue);
        xValueZ = M_GetSetParam_xValue (&stValue);
        DNM_M_bDynamicsSetAbsoluteImposeSpeed (pstDynamic, FALSE );
        DNM_M_vDynamicsSetImposeSpeedZ (pstDynamic,TRUE);
        MTH3D_M_vSetZofVector (DNM_M_pDynamicsGetImposeSpeed (pstDynamic),xValueZ);
        break;

    case 0xb4 :
        M_EvalNextParameter(&stValue);
        xValueX = M_GetSetParam_xValue (&stValue);
        M_EvalNextParameter(&stValue);
        xValueY = M_GetSetParam_xValue (&stValue);
        DNM_M_bDynamicsSetAbsoluteImposeSpeed (pstDynamic, FALSE );
        DNM_M_vDynamicsSetImposeSpeedX (pstDynamic,TRUE);
        DNM_M_vDynamicsSetImposeSpeedY (pstDynamic,TRUE);
        MTH3D_M_vSetXofVector (DNM_M_pDynamicsGetImposeSpeed (pstDynamic),xValueX);
        MTH3D_M_vSetYofVector (DNM_M_pDynamicsGetImposeSpeed (pstDynamic),xValueY);
        break;

    case 0xb5 :
        M_EvalNextParameter(&stValue);
        xValueX = M_GetSetParam_xValue (&stValue);
        M_EvalNextParameter(&stValue);
        xValueY = M_GetSetParam_xValue (&stValue);
        M_EvalNextParameter(&stValue);
        xValueZ = M_GetSetParam_xValue (&stValue);
        DNM_M_bDynamicsSetAbsoluteImposeSpeed (pstDynamic, FALSE );
        DNM_M_vDynamicsSetImposeSpeedX (pstDynamic,TRUE);
        DNM_M_vDynamicsSetImposeSpeedY (pstDynamic,TRUE);
        DNM_M_vDynamicsSetImposeSpeedZ (pstDynamic,TRUE);
        MTH3D_M_vSetXofVector (DNM_M_pDynamicsGetImposeSpeed (pstDynamic),xValueX);
        MTH3D_M_vSetYofVector (DNM_M_pDynamicsGetImposeSpeed (pstDynamic),xValueY);
        MTH3D_M_vSetZofVector (DNM_M_pDynamicsGetImposeSpeed (pstDynamic),xValueZ);
        break;

    case 0xbd :
        M_EvalNextParameter(&stValue);
        xValueX = M_GetSetParam_xValue (&stValue);
        DNM_M_bDynamicsSetAbsoluteProposeSpeed (pstDynamic, FALSE );
        DNM_M_vDynamicsSetProposeSpeedX (pstDynamic,TRUE);
        MTH3D_M_vSetXofVector (DNM_M_pDynamicsGetProposeSpeed (pstDynamic),xValueX);
        break;

    case 0xbe :
        M_EvalNextParameter(&stValue);
        xValueY = M_GetSetParam_xValue (&stValue);
        DNM_M_bDynamicsSetAbsoluteProposeSpeed (pstDynamic, FALSE );
        DNM_M_vDynamicsSetProposeSpeedY (pstDynamic,TRUE);
        MTH3D_M_vSetYofVector (DNM_M_pDynamicsGetProposeSpeed (pstDynamic),xValueY);
        break;

    case 0xbf :
        M_EvalNextParameter(&stValue);
        xValueZ = M_GetSetParam_xValue (&stValue);
        DNM_M_bDynamicsSetAbsoluteProposeSpeed (pstDynamic, FALSE );
        DNM_M_vDynamicsSetProposeSpeedZ (pstDynamic,TRUE);
        MTH3D_M_vSetZofVector (DNM_M_pDynamicsGetProposeSpeed (pstDynamic),xValueZ);
        break;

    case 0xc0 :
        M_EvalNextParameter(&stValue);
        xValueX = M_GetSetParam_xValue (&stValue);
        M_EvalNextParameter(&stValue);
        xValueY = M_GetSetParam_xValue (&stValue);
        DNM_M_bDynamicsSetAbsoluteProposeSpeed (pstDynamic, FALSE );
        DNM_M_vDynamicsSetProposeSpeedX (pstDynamic,TRUE);
        DNM_M_vDynamicsSetProposeSpeedY (pstDynamic,TRUE);
        MTH3D_M_vSetXofVector (DNM_M_pDynamicsGetProposeSpeed (pstDynamic),xValueX);
        MTH3D_M_vSetYofVector (DNM_M_pDynamicsGetProposeSpeed (pstDynamic),xValueY);
        break;

    case 0xc1 :
        M_EvalNextParameter(&stValue);
        xValueX = M_GetSetParam_xValue (&stValue);
        M_EvalNextParameter(&stValue);
        xValueY = M_GetSetParam_xValue (&stValue);
        M_EvalNextParameter(&stValue);
        xValueZ = M_GetSetParam_xValue (&stValue);
        DNM_M_bDynamicsSetAbsoluteProposeSpeed (pstDynamic, FALSE );
        DNM_M_vDynamicsSetProposeSpeedX (pstDynamic,TRUE);
        DNM_M_vDynamicsSetProposeSpeedY (pstDynamic,TRUE);
        DNM_M_vDynamicsSetProposeSpeedZ (pstDynamic,TRUE);
        MTH3D_M_vSetXofVector (DNM_M_pDynamicsGetProposeSpeed (pstDynamic),xValueX);
        MTH3D_M_vSetYofVector (DNM_M_pDynamicsGetProposeSpeed (pstDynamic),xValueY);
        MTH3D_M_vSetZofVector (DNM_M_pDynamicsGetProposeSpeed (pstDynamic),xValueZ);
        break;

    case 0xb7 :
        M_EvalNextParameter(&stValue);
        xValueX = M_GetSetParam_xValue (&stValue);
        DNM_M_bDynamicsSetAbsoluteImposeSpeed (pstDynamic, TRUE );
        DNM_M_vDynamicsSetImposeSpeedX (pstDynamic,TRUE);
        MTH3D_M_vSetXofVector (DNM_M_pDynamicsGetImposeSpeed (pstDynamic),xValueX);
        break;

    case 0xb8 :
        M_EvalNextParameter(&stValue);
        xValueY = M_GetSetParam_xValue (&stValue);
        DNM_M_bDynamicsSetAbsoluteImposeSpeed (pstDynamic, TRUE );
        DNM_M_vDynamicsSetImposeSpeedY (pstDynamic,TRUE);
        MTH3D_M_vSetYofVector (DNM_M_pDynamicsGetImposeSpeed (pstDynamic),xValueY);
        break;

    case 0xb9 :
        M_EvalNextParameter(&stValue);
        xValueZ = M_GetSetParam_xValue (&stValue);
        DNM_M_bDynamicsSetAbsoluteImposeSpeed (pstDynamic, TRUE );
        DNM_M_vDynamicsSetImposeSpeedZ (pstDynamic,TRUE);
        MTH3D_M_vSetZofVector (DNM_M_pDynamicsGetImposeSpeed (pstDynamic),xValueZ);
        break;

    case 0xba :
        M_EvalNextParameter(&stValue);
        xValueX = M_GetSetParam_xValue (&stValue);
        M_EvalNextParameter(&stValue);
        xValueY = M_GetSetParam_xValue (&stValue);
        DNM_M_bDynamicsSetAbsoluteImposeSpeed (pstDynamic, TRUE );
        DNM_M_vDynamicsSetImposeSpeedX (pstDynamic,TRUE);
        DNM_M_vDynamicsSetImposeSpeedY (pstDynamic,TRUE);
        MTH3D_M_vSetXofVector (DNM_M_pDynamicsGetImposeSpeed (pstDynamic),xValueX);
        MTH3D_M_vSetYofVector (DNM_M_pDynamicsGetImposeSpeed (pstDynamic),xValueY);
        break;

    case 0xbb :
        M_EvalNextParameter(&stValue);
        xValueX = M_GetSetParam_xValue (&stValue);
        M_EvalNextParameter(&stValue);
        xValueY = M_GetSetParam_xValue (&stValue);
        M_EvalNextParameter(&stValue);
        xValueZ = M_GetSetParam_xValue (&stValue);
        DNM_M_bDynamicsSetAbsoluteImposeSpeed (pstDynamic, TRUE );
        DNM_M_vDynamicsSetImposeSpeedX (pstDynamic,TRUE);
        DNM_M_vDynamicsSetImposeSpeedY (pstDynamic,TRUE);
        DNM_M_vDynamicsSetImposeSpeedZ (pstDynamic,TRUE);
        MTH3D_M_vSetXofVector (DNM_M_pDynamicsGetImposeSpeed (pstDynamic),xValueX);
        MTH3D_M_vSetYofVector (DNM_M_pDynamicsGetImposeSpeed (pstDynamic),xValueY);
        MTH3D_M_vSetZofVector (DNM_M_pDynamicsGetImposeSpeed (pstDynamic),xValueZ);
        break;

    case 0xc3 :
        M_EvalNextParameter(&stValue);
        xValueX = M_GetSetParam_xValue (&stValue);
        DNM_M_bDynamicsSetAbsoluteProposeSpeed (pstDynamic, TRUE );
        DNM_M_vDynamicsSetProposeSpeedX (pstDynamic,TRUE);
        MTH3D_M_vSetXofVector (DNM_M_pDynamicsGetProposeSpeed (pstDynamic),xValueX);
        break;

    case 0xc4 :
        M_EvalNextParameter(&stValue);
        xValueY = M_GetSetParam_xValue (&stValue);
        DNM_M_bDynamicsSetAbsoluteProposeSpeed (pstDynamic, TRUE );
        DNM_M_vDynamicsSetProposeSpeedY (pstDynamic,TRUE);
        MTH3D_M_vSetYofVector (DNM_M_pDynamicsGetProposeSpeed (pstDynamic),xValueY);
        break;

    case 0xc5 :
        M_EvalNextParameter(&stValue);
        xValueZ = M_GetSetParam_xValue (&stValue);
        DNM_M_bDynamicsSetAbsoluteProposeSpeed (pstDynamic, TRUE );
        DNM_M_vDynamicsSetProposeSpeedZ (pstDynamic,TRUE);
        MTH3D_M_vSetZofVector (DNM_M_pDynamicsGetProposeSpeed (pstDynamic),xValueZ);
        break;

    case 0xc6 :
        M_EvalNextParameter(&stValue);
        xValueX = M_GetSetParam_xValue (&stValue);
        M_EvalNextParameter(&stValue);
        xValueY = M_GetSetParam_xValue (&stValue);
        DNM_M_bDynamicsSetAbsoluteProposeSpeed (pstDynamic, TRUE );
        DNM_M_vDynamicsSetProposeSpeedX (pstDynamic,TRUE);
        DNM_M_vDynamicsSetProposeSpeedY (pstDynamic,TRUE);
        MTH3D_M_vSetXofVector (DNM_M_pDynamicsGetProposeSpeed (pstDynamic),xValueX);
        MTH3D_M_vSetYofVector (DNM_M_pDynamicsGetProposeSpeed (pstDynamic),xValueY);
        break;

    case 0xc7 :
        M_EvalNextParameter(&stValue);
        xValueX = M_GetSetParam_xValue (&stValue);
        M_EvalNextParameter(&stValue);
        xValueY = M_GetSetParam_xValue (&stValue);
        M_EvalNextParameter(&stValue);
        xValueZ = M_GetSetParam_xValue (&stValue);
        DNM_M_bDynamicsSetAbsoluteProposeSpeed (pstDynamic, TRUE );
        DNM_M_vDynamicsSetProposeSpeedX (pstDynamic,TRUE);
        DNM_M_vDynamicsSetProposeSpeedY (pstDynamic,TRUE);
        DNM_M_vDynamicsSetProposeSpeedZ (pstDynamic,TRUE);
        MTH3D_M_vSetXofVector (DNM_M_pDynamicsGetProposeSpeed (pstDynamic),xValueX);
        MTH3D_M_vSetYofVector (DNM_M_pDynamicsGetProposeSpeed (pstDynamic),xValueY);
        MTH3D_M_vSetZofVector (DNM_M_pDynamicsGetProposeSpeed (pstDynamic),xValueZ);
        break;

    case 0xca :
        M_EvalNextParameter(&stValue);
        xValueX = M_GetSetParam_xValue (&stValue);
        DNM_M_bDynamicsSetAbsoluteAddSpeed (pstDynamic, FALSE );
        DNM_M_vDynamicsSetAddSpeedX (pstDynamic,TRUE);
        MTH3D_M_vSetXofVector (DNM_M_pDynamicsGetAddSpeed (pstDynamic),xValueX);
        break;

    case 0xcb :
        M_EvalNextParameter(&stValue);
        xValueY = M_GetSetParam_xValue (&stValue);
        DNM_M_bDynamicsSetAbsoluteAddSpeed (pstDynamic, FALSE );
        DNM_M_vDynamicsSetAddSpeedY (pstDynamic,TRUE);
        MTH3D_M_vSetYofVector (DNM_M_pDynamicsGetAddSpeed (pstDynamic),xValueY);
        break;

    case 0xcc :
        M_EvalNextParameter(&stValue);
        xValueZ = M_GetSetParam_xValue (&stValue);
        DNM_M_bDynamicsSetAbsoluteAddSpeed (pstDynamic, FALSE );
        DNM_M_vDynamicsSetAddSpeedZ (pstDynamic,TRUE);
        MTH3D_M_vSetZofVector (DNM_M_pDynamicsGetAddSpeed (pstDynamic),xValueZ);
        break;

    case 0xcd :
        M_EvalNextParameter(&stValue);
        xValueX = M_GetSetParam_xValue (&stValue);
        M_EvalNextParameter(&stValue);
        xValueY = M_GetSetParam_xValue (&stValue);
        DNM_M_bDynamicsSetAbsoluteAddSpeed (pstDynamic, FALSE );
        DNM_M_vDynamicsSetAddSpeedX (pstDynamic,TRUE);
        DNM_M_vDynamicsSetAddSpeedY (pstDynamic,TRUE);
        MTH3D_M_vSetXofVector (DNM_M_pDynamicsGetAddSpeed (pstDynamic),xValueX);
        MTH3D_M_vSetYofVector (DNM_M_pDynamicsGetAddSpeed (pstDynamic),xValueY);
        break;

    case 0xce :
        M_EvalNextParameter(&stValue);
        xValueX = M_GetSetParam_xValue (&stValue);
        M_EvalNextParameter(&stValue);
        xValueY = M_GetSetParam_xValue (&stValue);
        M_EvalNextParameter(&stValue);
        xValueZ = M_GetSetParam_xValue (&stValue);
        DNM_M_bDynamicsSetAbsoluteAddSpeed (pstDynamic, FALSE );
        DNM_M_vDynamicsSetAddSpeedX (pstDynamic,TRUE);
        DNM_M_vDynamicsSetAddSpeedY (pstDynamic,TRUE);
        DNM_M_vDynamicsSetAddSpeedZ (pstDynamic,TRUE);
        MTH3D_M_vSetXofVector (DNM_M_pDynamicsGetAddSpeed (pstDynamic),xValueX);
        MTH3D_M_vSetYofVector (DNM_M_pDynamicsGetAddSpeed (pstDynamic),xValueY);
        MTH3D_M_vSetZofVector (DNM_M_pDynamicsGetAddSpeed (pstDynamic),xValueZ);
        break;

    case 0xd0 :
        M_EvalNextParameter(&stValue);
        xValueX = M_GetSetParam_xValue (&stValue);
        DNM_M_bDynamicsSetAbsoluteAddSpeed (pstDynamic, TRUE );
        DNM_M_vDynamicsSetAddSpeedX (pstDynamic,TRUE);
        MTH3D_M_vSetXofVector (DNM_M_pDynamicsGetImposeSpeed (pstDynamic),xValueX);
        break;

    case 0xd1 :
        M_EvalNextParameter(&stValue);
        xValueY = M_GetSetParam_xValue (&stValue);
        DNM_M_bDynamicsSetAbsoluteAddSpeed (pstDynamic, TRUE );
        DNM_M_vDynamicsSetAddSpeedY (pstDynamic,TRUE);
        MTH3D_M_vSetYofVector (DNM_M_pDynamicsGetImposeSpeed (pstDynamic),xValueY);
        break;

    case 0xd2 :
        M_EvalNextParameter(&stValue);
        xValueZ = M_GetSetParam_xValue (&stValue);
        DNM_M_bDynamicsSetAbsoluteAddSpeed (pstDynamic, TRUE );
        DNM_M_vDynamicsSetAddSpeedZ (pstDynamic,TRUE);
        MTH3D_M_vSetZofVector (DNM_M_pDynamicsGetImposeSpeed (pstDynamic),xValueZ);
        break;

    case 0xd3 :
        M_EvalNextParameter(&stValue);
        xValueX = M_GetSetParam_xValue (&stValue);
        M_EvalNextParameter(&stValue);
        xValueY = M_GetSetParam_xValue (&stValue);
        DNM_M_bDynamicsSetAbsoluteAddSpeed (pstDynamic, TRUE );
        DNM_M_vDynamicsSetAddSpeedX (pstDynamic,TRUE);
        DNM_M_vDynamicsSetAddSpeedY (pstDynamic,TRUE);
        MTH3D_M_vSetXofVector (DNM_M_pDynamicsGetImposeSpeed (pstDynamic),xValueX);
        MTH3D_M_vSetYofVector (DNM_M_pDynamicsGetImposeSpeed (pstDynamic),xValueY);
        break;

    case 0xd4 :
        M_EvalNextParameter(&stValue);
        xValueX = M_GetSetParam_xValue (&stValue);
        M_EvalNextParameter(&stValue);
        xValueY = M_GetSetParam_xValue (&stValue);
        M_EvalNextParameter(&stValue);
        xValueZ = M_GetSetParam_xValue (&stValue);
        DNM_M_bDynamicsSetAbsoluteAddSpeed (pstDynamic, TRUE );
        DNM_M_vDynamicsSetAddSpeedX (pstDynamic,TRUE);
        DNM_M_vDynamicsSetAddSpeedY (pstDynamic,TRUE);
        DNM_M_vDynamicsSetAddSpeedZ (pstDynamic,TRUE);
        MTH3D_M_vSetXofVector (DNM_M_pDynamicsGetImposeSpeed (pstDynamic),xValueX);
        MTH3D_M_vSetYofVector (DNM_M_pDynamicsGetImposeSpeed (pstDynamic),xValueY);
        MTH3D_M_vSetZofVector (DNM_M_pDynamicsGetImposeSpeed (pstDynamic),xValueZ);
        break;

    case 0x119 :
        M_EvalNextParameter(&stValue);
        xValueX = M_GetSetParam_xValue (&stValue);
        DNM_M_vDynamicsSetBitInFlags_(pstDynamic, DNM_C_LimitX, TRUE);
        MTH3D_M_vSetXofVector (DNM_M_pDynamicsGetLimit (pstDynamic),xValueX);
        break;

    case 0x11a :
        M_EvalNextParameter(&stValue);
        xValueY = M_GetSetParam_xValue (&stValue);
        DNM_M_vDynamicsSetBitInFlags_(pstDynamic, DNM_C_LimitY, TRUE);
        MTH3D_M_vSetYofVector (DNM_M_pDynamicsGetLimit (pstDynamic),xValueY);
        break;

    case 0x11c :
        M_EvalNextParameter(&stValue);
        xValueX = M_GetSetParam_xValue (&stValue);
        M_EvalNextParameter(&stValue);
        xValueY = M_GetSetParam_xValue (&stValue);
        M_EvalNextParameter(&stValue);
        xValueZ = M_GetSetParam_xValue (&stValue);
        DNM_M_vDynamicsSetBitInFlags_(pstDynamic, DNM_C_LimitX, TRUE);
        DNM_M_vDynamicsSetBitInFlags_(pstDynamic, DNM_C_LimitY, TRUE);
        DNM_M_vDynamicsSetBitInFlags_(pstDynamic, DNM_C_LimitZ, TRUE);
        MTH3D_M_vSetXofVector (DNM_M_pDynamicsGetLimit (pstDynamic),xValueX);
        MTH3D_M_vSetYofVector (DNM_M_pDynamicsGetLimit (pstDynamic),xValueY);
        MTH3D_M_vSetZofVector (DNM_M_pDynamicsGetLimit (pstDynamic),xValueZ);
        break;

    case 0x11b :
        M_EvalNextParameter(&stValue);
        xValueZ = M_GetSetParam_xValue (&stValue);
        DNM_M_vDynamicsSetBitInFlags_(pstDynamic, DNM_C_LimitZ, TRUE);
        MTH3D_M_vSetZofVector (DNM_M_pDynamicsGetLimit (pstDynamic),xValueZ);
        break;

    case 0x11d :
        DNM_M_vDynamicsSetBitInFlags_(pstDynamic, DNM_C_LimitX, FALSE);
        break;

    case 0x11e :
        DNM_M_vDynamicsSetBitInFlags_(pstDynamic, DNM_C_LimitY, FALSE);
        break;

    case 0x11f :
        DNM_M_vDynamicsSetBitInFlags_(pstDynamic, DNM_C_LimitZ, FALSE);
        break;

    case 0x120 :
        DNM_M_vDynamicsSetBitInFlags_(pstDynamic, DNM_C_LimitX, FALSE);
        DNM_M_vDynamicsSetBitInFlags_(pstDynamic, DNM_C_LimitY, FALSE);
        DNM_M_vDynamicsSetBitInFlags_(pstDynamic, DNM_C_LimitZ, FALSE);
        break;

    case 0x12a :
        DNM_M_vDynamicsSetBitInEndFlags_(pstDynamic, DNM_C_Hanging, FALSE);
        break;

    default:
        break;
    }

    return(p_stTree);
}

/* 0x0046D8A0 */
AI_tdstNodeInterpret *__cdecl fn_SPO_p_stChangeFlag(
    HIE_tdstSuperObject *p_SuperObjPerso, AI_tdstNodeInterpret *p_stTree)
{
    AI_tdstGetSetParam    stValue;
    void                 *hSPO;
    long                  lFlag;
    unsigned long         ulMask;
    unsigned long         ulFlags;
    AI_tdstNodeInterpret *p_stNext;

    p_stNext = fn_p_stEvalTree(p_SuperObjPerso, p_stTree, &stValue);
    hSPO     = stValue.uParam.p_stEngObjValue;
    p_stNext = fn_p_stEvalTree(p_SuperObjPerso, p_stNext, &stValue);
    lFlag    = stValue.uParam.lValue;
    p_stNext = fn_p_stEvalTree(p_SuperObjPerso, p_stNext, &stValue);

    ulFlags = HIE_fn_SO_ulGetFlags((HIE_tdstSuperObject *)hSPO);
    ulMask  = 1u << lFlag;
    if (stValue.uParam.lValue != 0)
        ulFlags |= ulMask;
    else
        ulFlags &= ~ulMask;
    HIE_fn_SO_vSetFlags((HIE_tdstSuperObject *)hSPO, ulFlags);
    return p_stNext;
}

/* 0x0046D920 */
AI_tdstNodeInterpret *__cdecl fn_p_stChangeActorSpoFlag(
    HIE_tdstSuperObject *p_SuperObjPerso, AI_tdstNodeInterpret *p_stTree)
{
    AI_tdstGetSetParam    stValue;
    HIE_tdstSuperObject  *hNewSuperObjPerso;
    long                  lFlag;
    long                  lSet;
    unsigned long         ulFlags;
    unsigned long         ulMask;
    unsigned long         procId;

    procId = (unsigned long)M_eProcedureIdInterpret(p_stTree-1);

    fn_vGetUltraOperatorPerso(fn_ucGetProcedureUltraOperator(procId), p_SuperObjPerso, &hNewSuperObjPerso);

    p_stTree = fn_p_stEvalTree(p_SuperObjPerso, p_stTree, &stValue);
    lFlag    = stValue.uParam.lValue;
    p_stTree = fn_p_stEvalTree(p_SuperObjPerso, p_stTree, &stValue);
    lSet     = stValue.uParam.lValue;

    ulFlags = HIE_fn_SO_ulGetFlags(hNewSuperObjPerso);
    ulMask  = 1 << lFlag;
    if (lSet)
        ulFlags |= ulMask;
    else
        ulFlags &= ~ulMask;
    HIE_fn_SO_vSetFlags(hNewSuperObjPerso, ulFlags);
    return p_stTree;
}

/* 0x0046D9A0 */
AI_tdstNodeInterpret *__cdecl fn_p_PlatformLink(
    HIE_tdstSuperObject *p_SuperObjPerso, AI_tdstNodeInterpret *p_stTree)
{
    AI_tdstGetSetParam    stValue;
    HIE_tdstSuperObject  *hNewSuperObjPerso;
    void                 *h_Dynam;
    void                 *pstDynamic;
    void                 *hPlatform;
    unsigned long         procId;

    procId = (unsigned long)M_eProcedureIdInterpret(p_stTree-1);

    fn_vGetUltraOperatorPerso(fn_ucGetProcedureUltraOperator(procId), p_SuperObjPerso, &hNewSuperObjPerso);

    h_Dynam = M_GetMSHandle(hNewSuperObjPerso,Dynam);
    if (!h_Dynam)
        return p_stTree;

    pstDynamic = fn_p_stDynamGetDynamics(h_Dynam);
    switch (procId)
    {
    case 0x121:
        p_stTree = fn_p_stEvalTree(p_SuperObjPerso, p_stTree, &stValue);

        hPlatform = fn_h_StandardGameGetSuperObject(stValue.uParam.p_stEngObjValue->h_StandardGame);
        MEC_fn_vForcePlatformLink(pstDynamic, hNewSuperObjPerso, hPlatform);
        break;
    case 0x122:
        MEC_fn_vFreePlatformLink(pstDynamic, hNewSuperObjPerso);
        break;
    default:
        break;
    }
    return p_stTree;
}

/* 0x0046DA50 */
AI_tdstNodeInterpret *__cdecl fn_p_HierFatherLink(
    HIE_tdstSuperObject *p_SuperObjPerso, AI_tdstNodeInterpret *p_stTree)
{
	AI_tdstGetSetParam stValue;
	DNM_tdstDynamics* pstDynamic;
	MS_tdxHandleToDynam h_Dynam;
	HIE_tdstSuperObject *hNewSuperObjPerso;
	HIE_tdstSuperObject *hChildSuperObject;
	HIE_tdstSuperObject *hPlatform;
	tdstList* p_stList;
	int i;
	long eProcedureId=M_eProcedureIdInterpret(p_stTree-1);

	fn_vGetUltraOperatorPerso(fn_ucGetProcedureUltraOperator(eProcedureId),p_SuperObjPerso,&hNewSuperObjPerso);

	h_Dynam = M_GetMSHandle(hNewSuperObjPerso,Dynam);
	if ( h_Dynam )
		pstDynamic = fn_p_stDynamGetDynamics(h_Dynam);
	else
		pstDynamic = NULL;

	switch (eProcedureId)
	{
	case 0x12d:
		M_EvalNextParameter( &stValue );

		if ( !M_GetSetParam_p_stEngineObjValue(&stValue) )
		{
			HIE_fn_vChangeFather(hNewSuperObjPerso, HIE_M_hGetSuperObjectFather(hNewSuperObjPerso), fn_bStandardGameIsAPlatform(hNewSuperObjPerso));
		}
		else
		{
			hPlatform = (HIE_tdstSuperObject *) M_GetSetParam_p_stSupObjValue( &stValue );

			if ( pstDynamic && MEC_fn_bIsBaseMechanic(pstDynamic) )
			{
				MEC_fn_vForcePlatformLink(pstDynamic,hNewSuperObjPerso,hPlatform);
			}
			else
			{

				HIE_fn_vChangeFather(hNewSuperObjPerso, hPlatform, FALSE);
			}
		}
		break;

	case 0x12e:
		if ( pstDynamic && MEC_fn_bIsBaseMechanic(pstDynamic) )
			MEC_fn_vFreePlatformLink(pstDynamic, hNewSuperObjPerso);
		else
			fn_vReputCharacterSuperObjectAtTheWorld(hNewSuperObjPerso);
		break;

	case 0x12f:
		M_EvalNextParameter(&stValue);
		p_stList=(tdstList *) M_GetSetParam_Dsg_List(&stValue);
		M_ClearList(p_stList);

		if ( M_ucGetMaxSizeOfList(p_stList) > 0 )
		{

			HIE_M_ForEachChildOf (hNewSuperObjPerso,hChildSuperObject,i)
			{
				if (HIE_fn_ulGetSuperObjectType(hChildSuperObject) == HIE_C_ulActor)
				{
					fn_vPutInList(p_stList, hChildSuperObject);

					if ( M_ucNbEltInList(p_stList)==M_ucGetMaxSizeOfList(p_stList) )
					{
						return p_stTree;
						break;
					}
				}
			}
		}
		break;

	default:
		break;
	}
	return p_stTree;
}
/* 0x0046DC00 */
AI_tdstNodeInterpret *__cdecl fn_p_SetMechanicListParameter(
    HIE_tdstSuperObject *p_SuperObjPerso, AI_tdstNodeInterpret *p_stTree)
{
    unsigned long         procId = (unsigned long)M_eProcedureIdInterpret(p_stTree-1);
    AI_tdstGetSetParam    stValue;
    void                 *p_stList;
    MTH3D_tdstVector     *p_stVector;
    AI_tdstNodeInterpret *p_stNext;
    unsigned long         ulCounter, ulMaxList;

    p_stNext = fn_p_stEvalTree(p_SuperObjPerso, p_stTree, &stValue);
    p_stList = stValue.uParam.p_stEngObjValue;
    p_stNext = fn_p_stEvalTree(p_SuperObjPerso, p_stNext, &stValue);
    p_stVector = (MTH3D_tdstVector *)stValue.uParam.a_xVectorValue;

    if (procId == 0x117)
    {
        ulMaxList = M_ucNbEltInList((tdstList *)p_stList);
        for (ulCounter = 0; ulCounter < ulMaxList; ulCounter++)
        {
            HIE_tdstSuperObject *hSuperObject;
            void                *h_Dynam;
            DNM_tdstDynamics    *pstDynamic;
            MTH3D_tdstVector     stVector;

            hSuperObject = AI_M_pfn_fn_p_stGetPersoInList2(p_stList, (unsigned char)ulCounter);
            h_Dynam = M_GetMSHandle(hSuperObject,Dynam);
            if (h_Dynam == NULL)
                return p_stNext;
            pstDynamic = (DNM_tdstDynamics *)AI_M_pfn_fn_p_stDynamGetDynamics(h_Dynam);

            MTH3D_M_vAddVector(&stVector, DNM_M_pDynamicsGetStreamSpeed(pstDynamic), p_stVector);
            MTH3D_M_vCopyVector(DNM_M_pDynamicsGetStreamSpeed(pstDynamic), &stVector);
        }
    }
    return p_stNext;
}

/* 0x0046DCE0 */
AI_tdstNodeInterpret *__cdecl fn_p_stResetLastCollisionActor(
    HIE_tdstSuperObject *p_SuperObjPerso, AI_tdstNodeInterpret *p_stTree)
{
    unsigned long         procId = (unsigned long)M_eProcedureIdInterpret(p_stTree-1);
    HIE_tdstSuperObject  *hNewSuperObjPerso;
    char                 *p_stReport;

    fn_vGetUltraOperatorPerso(fn_ucGetProcedureUltraOperator(procId), p_SuperObjPerso, &hNewSuperObjPerso);

    switch (procId)
    {
    case 0xeb:
        if (M_GetMSHandle(hNewSuperObjPerso,StandardGame) != NULL)
            AI_M_pfn_fn_vSuperObjectSetLastCollidedCharacter(hNewSuperObjPerso, NULL);
        break;
    case 0xec:
        p_stReport = AI_M_pfn_fn_pstGetDNMReport(hNewSuperObjPerso);
        if (p_stReport != NULL)
            ((DNM_tdstReport *)p_stReport)->m_ulCurrSurfaceState = 0;
        break;
    default:
        break;
    }
    return p_stTree;
}

/* 0x0046DD50 */
AI_tdstNodeInterpret *__cdecl fn_p_SetMechanic3Real(
    HIE_tdstSuperObject *p_SuperObjPerso, AI_tdstNodeInterpret *p_stTree)
{
	AI_tdstGetSetParam stValue;
	DNM_tdstDynamics* pstDynamic;
	MS_tdxHandleToDynam h_Dynam;
	HIE_tdstSuperObject *hNewSuperObjPerso;
	MTH_tdxReal xValue1, xValue2, xValue3;
	long eProcedureId=M_eProcedureIdInterpret(p_stTree-1);

	fn_vGetUltraOperatorPerso(fn_ucGetProcedureUltraOperator(eProcedureId),p_SuperObjPerso,&hNewSuperObjPerso);

	h_Dynam = M_GetMSHandle(hNewSuperObjPerso,Dynam);
	if (!h_Dynam)
	{
		return(p_stTree);
	}

	pstDynamic = fn_p_stDynamGetDynamics(h_Dynam);
	if (!pstDynamic) return p_stTree;

	M_EvalNextParameter(&stValue);
	xValue1 = M_GetSetParam_xValue (&stValue);
	M_EvalNextParameter(&stValue);
	xValue2 = M_GetSetParam_xValue (&stValue);
	M_EvalNextParameter(&stValue);
	xValue3 = M_GetSetParam_xValue (&stValue);

	switch (eProcedureId)
	{

	case 0x123:
		DNM_M_vDynamicsSetScale ( pstDynamic, xValue1, xValue2, xValue3 );
		DNM_M_vDynamicsSetChangeScale ( pstDynamic, TRUE );
		break;

	case 0x124:
		DNM_M_vDynamicsSetSlideFactorX ( pstDynamic, xValue1 );
		DNM_M_vDynamicsSetSlideFactorY ( pstDynamic, xValue2 );
		DNM_M_vDynamicsSetSlideFactorZ ( pstDynamic, xValue3 );
		break;

	default:
		break;
	}

	return p_stTree;
}

/* 0x0046DE50 */
AI_tdstNodeInterpret *__cdecl fn_p_SetMechanicClimbSpeedLimit(
    HIE_tdstSuperObject *p_SuperObjPerso, AI_tdstNodeInterpret *p_stTree)
{
    unsigned long         procId = (unsigned long)M_eProcedureIdInterpret(p_stTree-1);
    AI_tdstGetSetParam    stValue;
    AI_tdstNodeInterpret *p_stNext;
    HIE_tdstSuperObject  *p_stResolved;

    fn_vGetUltraOperatorPerso(fn_ucGetProcedureUltraOperator(procId), p_SuperObjPerso, &p_stResolved);

    p_stNext = fn_p_stEvalTree(p_SuperObjPerso, p_stTree, &stValue);
    AI_MEC_xClimbSpeedLimit = stValue.uParam.xValue;
    return p_stNext;
}

/* 0x0046DEA0 */
AI_tdstNodeInterpret *__cdecl fn_p_stShadow(
    HIE_tdstSuperObject *p_SuperObjPerso, AI_tdstNodeInterpret *p_stTree)
{
	AI_tdstGetSetParam stValue;
	HIE_tdstSuperObject *hNewSuperObjPerso;
	long  lValue;
	unsigned long lCustomBits;
	MS_tdxHandleToStandardGame hStdGame;
	long eProcedureId=M_eProcedureIdInterpret(p_stTree-1);

	fn_vGetUltraOperatorPerso(fn_ucGetProcedureUltraOperator(eProcedureId),p_SuperObjPerso,&hNewSuperObjPerso);

	switch (eProcedureId)
	{
	case 0x13e:
		{
			hStdGame = M_GetMSHandle(hNewSuperObjPerso,StandardGame);
			lCustomBits = fn_ulStandardGameGetCustomBits(hStdGame);
			M_EvalNextParameter(&stValue);
			lValue = M_GetSetParam_lValue (&stValue);
			if (lValue)
			{

				lCustomBits |= GAM_C_CustBitActorHasShadow;
			}
			else
			{

				lCustomBits &= ~GAM_C_CustBitActorHasShadow;
			}
			fn_vStandardGameSetCustomBits(hStdGame,lCustomBits);
			break;
		}

	case 0x13f:
		M_EvalNextParameter(&stValue);
		*(MTH_tdxReal *)((char *)M_GetMSHandle(hNewSuperObjPerso,3dData) + 0x114) = M_GetSetParam_xValue(&stValue);
		break;
	case 0x140:
		M_EvalNextParameter(&stValue);
		MTH3D_M_vCopyVector((MTH3D_tdstVector *)((char *)M_GetMSHandle(hNewSuperObjPerso,3dData) + 0x108), &M_GetSetParam_stVectorValue(&stValue));
		break;

	default :
		{
			return p_stTree;
		}
		break;
	}

	return p_stTree;
}

/* 0x0046DFB0 */
AI_tdstNodeInterpret *__cdecl fn_p_stSetPrivilegeActivationZDx(
    HIE_tdstSuperObject *p_SuperObjPerso, AI_tdstNodeInterpret *p_stTree)
{
    AI_tdstGetSetParam    stParam;
    HIE_tdstSuperObject  *hPerso;
    void                 *hCollSet;
    long                  lZoneNumber;
    unsigned long         procId;

    procId = (unsigned long)M_eProcedureIdInterpret(p_stTree-1);

    fn_vGetUltraOperatorPerso(fn_ucGetProcedureUltraOperator(procId), p_SuperObjPerso, &hPerso);

    hCollSet = M_GetMSHandle(hPerso,CollSet);
    p_stTree = fn_p_stEvalTree(p_SuperObjPerso, p_stTree, &stParam);
    lZoneNumber = stParam.uParam.lValue;
    lZoneNumber--;

    switch (procId)
    {

    case 0x141:
        CS_fn_vSetPrivilegedActivationZone(0, hCollSet, (short)lZoneNumber, 1);
        break;
    case 0x142:
        CS_fn_vSetPrivilegedActivationZone(1, hCollSet, (short)lZoneNumber, 1);
        break;
    case 0x144:
        CS_fn_vSetPrivilegedActivationZone(3, hCollSet, (short)lZoneNumber, 1);
        break;
    case 0x143:
        CS_fn_vSetPrivilegedActivationZone(2, hCollSet, (short)lZoneNumber, 1);
        break;

    case 0x147:
        CS_fn_vSetPrivilegedActivationZone(2, hCollSet, (short)lZoneNumber, 2);
        break;
    case 0x145:
        CS_fn_vSetPrivilegedActivationZone(0, hCollSet, (short)lZoneNumber, 2);
        break;
    case 0x146:
        CS_fn_vSetPrivilegedActivationZone(1, hCollSet, (short)lZoneNumber, 2);
        break;
    case 0x148:
        CS_fn_vSetPrivilegedActivationZone(3, hCollSet, (short)lZoneNumber, 2);
        break;

    case 0x149:
        CS_fn_vSetPrivilegedActivationZone(0, hCollSet, (short)lZoneNumber, 0);
        break;
    case 0x14C:
        CS_fn_vSetPrivilegedActivationZone(3, hCollSet, (short)lZoneNumber, 0);
        break;
    case 0x14A:
        CS_fn_vSetPrivilegedActivationZone(1, hCollSet, (short)lZoneNumber, 0);
        break;
    case 0x14B:
        CS_fn_vSetPrivilegedActivationZone(2, hCollSet, (short)lZoneNumber, 0);
        break;
    default:
        AI_M_pfn_M_AIFatalError(0x28);
        return p_stTree;
    }
    return p_stTree;
}

/* 0x0046E180 */
AI_tdstNodeInterpret *__cdecl fn_p_stSetCollSetCollisionFlag(
    HIE_tdstSuperObject *p_SuperObjPerso, AI_tdstNodeInterpret *p_stTree)
{
    long                  eProcedureId = M_eProcedureIdInterpret(p_stTree-1);
    HIE_tdstSuperObject  *hPerso;
    void                 *hCollSet;
    AI_tdstGetSetParam    stParam;
    long                  bValue;

    fn_vGetUltraOperatorPerso(fn_ucGetProcedureUltraOperator(eProcedureId), p_SuperObjPerso, &hPerso);
    hCollSet = M_GetMSHandle(hPerso,CollSet);

    p_stTree = fn_p_stEvalTree(p_SuperObjPerso, p_stTree, &stParam);
    bValue   = stParam.uParam.lValue;

    switch (eProcedureId)
    {
    case 0x14e: fn_vCollSetSetCharacterNoCollisionWithMap(hCollSet, bValue);                 break;
    case 0x150: fn_vCollSetSetCharacterNoCollisionWithSecondaryCharacter(hCollSet, bValue);  break;
    case 0x151: fn_vCollSetSetCharacterNoCollisionWithMainCharacter(hCollSet, bValue);       break;
    case 0x152: fn_vCollSetSetCharacterNoCollisionWithOtherSectors(hCollSet, bValue);        break;
    case 0x14f: fn_vCollSetSetCharacterNoCollisionWithProjectile(hCollSet, bValue);          break;
    case 0x153: fn_vCollSetSetCharacterNoCollisionZdeWithProjectile(hCollSet, bValue);       break;
    default:    AI_M_pfn_M_AIFatalError(0x28);                                                   break;
    }
    return p_stTree;
}

/* 0x0046E2A0 */
AI_tdstNodeInterpret *__cdecl fn_p_stSetComputationFrequency(
    HIE_tdstSuperObject *p_SuperObjPerso, AI_tdstNodeInterpret *p_stTree)
{
    unsigned long         procId = (unsigned long)M_eProcedureIdInterpret(p_stTree-1);
    HIE_tdstSuperObject  *hPerso;
    AI_tdstGetSetParam    stParam;
    unsigned char         ucFrequency;

    fn_vGetUltraOperatorPerso(fn_ucGetProcedureUltraOperator(procId), p_SuperObjPerso, &hPerso);

    p_stTree = fn_p_stEvalTree(p_SuperObjPerso, p_stTree, &stParam);
    ucFrequency = stParam.uParam.ucValue;

    switch (procId)
    {
    case 0x154:
        fn_vCollSetSetCollComputeFrequency(M_GetMSHandle(hPerso,CollSet), ucFrequency);
        break;
    case 0x155:
        AI_M_pfn_fn_v3dDataSetBrainComputationFrequency(M_GetMSHandle(hPerso,3dData), ucFrequency);
        break;
    case 0x156:
        AI_M_pfn_fn_v3dDataSetLightComputationFrequency(M_GetMSHandle(hPerso,3dData), ucFrequency);
        break;
    default:
        AI_M_pfn_M_AIFatalError(0x28);
        break;
    }
    return p_stTree;
}

/* 0x0046E360 */
AI_tdstNodeInterpret *__cdecl fn_p_stActivationZDR(
    HIE_tdstSuperObject *p_SuperObjPerso, AI_tdstNodeInterpret *p_stTree)
{
    unsigned long         procId = (unsigned long)M_eProcedureIdInterpret(p_stTree-1);
    HIE_tdstSuperObject  *p_stNew;
    AI_tdstGetSetParam    stValue;
    AI_tdstNodeInterpret *p_stNext;

    fn_vGetUltraOperatorPerso(fn_ucGetProcedureUltraOperator(procId), p_SuperObjPerso, &p_stNew);
    p_stNext = fn_p_stEvalTree(p_SuperObjPerso, p_stTree, &stValue);

    if (stValue.uParam.lValue != 0)
        HIE_fn_SO_vSetFlags(p_stNew, HIE_fn_SO_ulGetFlags(p_stNew) & 0xfffffffe);
    else
        HIE_fn_SO_vSetFlags(p_stNew, HIE_fn_SO_ulGetFlags(p_stNew) | 1);
    return p_stNext;
}

/* 0x0046E3C0 */
AI_tdstNodeInterpret *__cdecl fn_p_LinkControl(
    HIE_tdstSuperObject *p_SuperObjPerso, AI_tdstNodeInterpret *p_stTree)
{
    AI_tdstGetSetParam    stParam;
    HIE_tdstSuperObject  *hPerso;
    void                 *h_Dynam;
    DNM_tdstDynamics     *pstDynamic;
    ACP_tdxBool           bValue;
    unsigned long         procId;

    procId = (unsigned long)M_eProcedureIdInterpret(p_stTree-1);

    fn_vGetUltraOperatorPerso(fn_ucGetProcedureUltraOperator(procId), p_SuperObjPerso, &hPerso);

    p_stTree = fn_p_stEvalTree(p_SuperObjPerso, p_stTree, &stParam);
    bValue   = (ACP_tdxBool)stParam.uParam.lValue;

    h_Dynam = M_GetMSHandle(hPerso,Dynam);
    if (!h_Dynam)
        return p_stTree;

    pstDynamic = fn_p_stDynamGetDynamics(h_Dynam);
    if (!pstDynamic)
        return p_stTree;

    DNM_M_ulFlags(pstDynamic) = ((bValue) ? (DNM_M_ulFlags(pstDynamic) |= DNM_C_LockPlatform) : (DNM_M_ulFlags(pstDynamic) &= ~DNM_C_LockPlatform));
    return p_stTree;
}

/* 0x0046E450 */
AI_tdstNodeInterpret *__cdecl fn_p_stReadAnalogJoystickMario(
    HIE_tdstSuperObject *p_SuperObjPerso, AI_tdstNodeInterpret *p_stTree)
{
    unsigned long         procId  = (unsigned long)M_eProcedureIdInterpret(p_stTree-1);
    HIE_tdstSuperObject  *hCamera;
    AI_tdstGetSetParam    stParam;
    AI_tdstNodeInterpret *p_stNode;
    AI_tdstNodeInterpret *p_stRet;

    float xCenterValue, xMinimumAngle, xRotationSpeed, xJoystickInertia;
    long  iMovementPlane, iRotationAsked, iComputeStrafeSector;
    long  iDt;
    int   iDeltaFrame;
    int   iXValue = 0, iYValue = 0;
    char  bKeyboardUsed = 0;
    float xKbdX = 0.0f, xKbdY = 0.0f;
    float xAnalogForce = 0.0f, xTrueAnalogForce = 0.0f;
    float xAngleRotation = 0.0f;
    float xPow, xCoeff;

    MTH3D_tdstVector stJoyVector = {0.0f, 0.0f, 0.0f};
    MTH3D_tdstVector stSightVector;
    MTH3D_tdstVector stI, stJ, stK;
    MTH3D_tdstVector *pPersoV0 = 0, *pPersoV1 = 0, *pPersoV2 = 0;
    MTH3D_tdstVector *pCamV0 = 0,   *pCamV1 = 0,   *pCamV2 = 0;
    POS_tdstCompletePosition *pPersoGlobal;
    void *hDynam;

    fn_vGetUltraOperatorPerso(fn_ucGetProcedureUltraOperator(procId), p_SuperObjPerso, &hCamera);
    p_stNode = fn_p_stEvalTree(p_SuperObjPerso, p_stTree, &stParam);
    xCenterValue = M_ReturnParam_xValue(&stParam);
    p_stNode = fn_p_stEvalTree(p_SuperObjPerso, p_stNode, &stParam);
    xMinimumAngle = M_ReturnParam_xValue(&stParam) * AI_3D_xDegToRad;
    p_stNode = fn_p_stEvalTree(p_SuperObjPerso, p_stNode, &stParam);
    xRotationSpeed = M_ReturnParam_xValue(&stParam);
    p_stNode = fn_p_stEvalTree(p_SuperObjPerso, p_stNode, &stParam);
    xJoystickInertia = M_ReturnParam_xValue(&stParam);
    p_stNode = fn_p_stEvalTree(p_SuperObjPerso, p_stNode, &stParam);
    iMovementPlane = M_ReturnParam_lValue(&stParam);
    p_stNode = fn_p_stEvalTree(p_SuperObjPerso, p_stNode, &stParam);
    iRotationAsked = M_GetSetParam_lValue(&stParam);
    p_stRet = fn_p_stEvalTree(p_SuperObjPerso, p_stNode, &stParam);
    iComputeStrafeSector = M_GetSetParam_lValue(&stParam);

    if (iMovementPlane == 0)
    {
        iXValue = 0; iYValue = 0;
        stJoyVector.xX = stJoyVector.xY = stJoyVector.xZ = 0.0f;
        xAnalogForce = 0.0f; xTrueAnalogForce = 0.0f;
        xAngleRotation = 0.0f; iComputeStrafeSector = 0;
        goto fin;
    }

    iDt = (long)AI_PAD_ulUsefulDeltaTime;
    iDeltaFrame = (int)((float)iDt / (AI_PAD_xFrameLength * AI_PAD_x1000) - AI_PAD_xInv2Neg);

    if (AI_PAD_hKeyUp    && AI_PAD_pfn_bReadButton(AI_PAD_hKeyUp))    { xKbdY = -100.0f;              bKeyboardUsed = 1; }
    if (AI_PAD_hKeyDown  && AI_PAD_pfn_bReadButton(AI_PAD_hKeyDown))  { xKbdY -= AI_PAD_xKeyStepDown; bKeyboardUsed = 1; }
    if (AI_PAD_hKeyRight && AI_PAD_pfn_bReadButton(AI_PAD_hKeyRight)) { xKbdX = 100.0f;               bKeyboardUsed = 1; }
    if (AI_PAD_hKeyLeft  && AI_PAD_pfn_bReadButton(AI_PAD_hKeyLeft))  { xKbdX -= AI_PAD_xKeyStepLeft; bKeyboardUsed = 1; }

    if (!bKeyboardUsed)
    {

        iXValue = AI_PAD_xJoystickAxeX ? (int)AI_PAD_pfn_xReadAxis(AI_PAD_xJoystickAxeX) : 0;
        iYValue = AI_PAD_xJoystickAxeY ? (int)AI_PAD_pfn_xReadAxis(AI_PAD_xJoystickAxeY) : 0;
    }
    else
    {
        xCoeff = AI_PAD_xKbdInertia;
        if (AI_PAD_hKeySlow && AI_PAD_pfn_bReadButton(AI_PAD_hKeySlow))
        {
            xKbdY *= AI_PAD_xSlowMul;
            xKbdX *= AI_PAD_xSlowMul;
            xCoeff = AI_PAD_xSlowInertia;
        }
        if (xKbdY != 0.0f && xKbdX != 0.0f)
        {
            xKbdY *= AI_PAD_xDiagScale;
            xKbdX *= AI_PAD_xDiagScale;
        }
        if (AI_PAD_bResetInertia == 0)
        {
            xPow = xCoeff;
            if (iDeltaFrame != 1)
            {
                if (iDeltaFrame == 2)      xPow = xCoeff * xCoeff;
                else if (iDeltaFrame == 3) xPow = xCoeff * xCoeff * xCoeff;
                else { float t = xCoeff * xCoeff; xPow = t * t; }
            }
            xKbdY = (AI_PAD_xPrevY - xKbdY) * xPow + xKbdY;
            xKbdX = (AI_PAD_xPrevX - xKbdX) * xPow + xKbdX;
        }
        else
        {
            AI_PAD_bResetInertia = 0;
        }
        iXValue = (int)xKbdX;
        iYValue = (int)xKbdY;
    }
    AI_PAD_xPrevX = xKbdX;
    AI_PAD_xPrevY = xKbdY;

    if (iMovementPlane == 2)
    {
        xRotationSpeed = 0.0f;
        iRotationAsked = 0;
    }

    if (!bKeyboardUsed)
    {
        int iCentreX = (int)AI_PAD_cCenterX;
        int iCentreY = (int)AI_PAD_cCenterY;
        int iCalX, iCalY, iRange;

        if ((short)iYValue > (short)AI_PAD_cYmax)      AI_PAD_cYmax = (signed char)iYValue;
        else if ((short)iYValue < (short)AI_PAD_cYmin) AI_PAD_cYmin = (signed char)iYValue;
        if ((short)iXValue > (short)AI_PAD_cXmax)      AI_PAD_cXmax = (signed char)iXValue;
        else if ((short)iXValue < (short)AI_PAD_cXmin) AI_PAD_cXmin = (signed char)iXValue;

        iCalX = iXValue - iCentreX;
        if (iCalX > 0)      iCalX = (iCalX * 100)  / ((int)AI_PAD_cXmax - iCentreX);
        else if (iCalX < 0) iCalX = (iCalX * -100) / ((int)AI_PAD_cXmin - iCentreX);

        iCalY = iYValue - iCentreY;
        if (iCalY > 0)      { iRange = (int)AI_PAD_cYmax - iCentreY; iCalY = ((iCalY * 25) << 2) / iRange; }
        else if (iCalY < 0) { iRange = (int)AI_PAD_cYmin - iCentreY; iCalY = ((iCalY * -25) << 2) / iRange; }

        AI_PAD_lCurXValue = iCalX;
        AI_PAD_lCurYValue = iCalY;
        iXValue = iCalX;
        iYValue = iCalY;

        if (AI_PAD_lCheat1Enable != 0) AI_PAD_pfnCheat();
        if (AI_PAD_lCheat2Enable != 0) AI_PAD_pfnCheat();
        if (AI_PAD_lCurXValue != iXValue) iXValue = AI_PAD_lCurXValue;
        if (AI_PAD_lCurYValue != iYValue) iYValue = AI_PAD_lCurYValue;

        if (AI_PAD_xCircleMax < (float)(iXValue * iXValue + iYValue * iYValue))
        {
            float xLen = MTH_M_xSqrt((float)(iXValue * iXValue + iYValue * iYValue));
            iXValue = (int)((float)(iXValue * 100) / xLen);
            iYValue = (int)((float)(iYValue * 100) / xLen);
        }
    }

    stJoyVector.xX = (float)(-iXValue);
    if (iMovementPlane == 2)
    {
        stJoyVector.xY = 0.0f;
        stJoyVector.xZ = (float)(-iYValue);
    }
    else
    {
        stJoyVector.xY = (float)iYValue;
        stJoyVector.xZ = 0.0f;
    }

    xTrueAnalogForce = MTH_M_xSqrt(stJoyVector.xX * stJoyVector.xX +
                             stJoyVector.xY * stJoyVector.xY +
                             stJoyVector.xZ * stJoyVector.xZ);
    if (xTrueAnalogForce <= xCenterValue)
    {
        xTrueAnalogForce = 0.0f;
        iRotationAsked = 0;
        stJoyVector.xX = stJoyVector.xY = stJoyVector.xZ = 0.0f;
    }

    if (AI_PAD_bPositiveNonZero(xJoystickInertia))
    {
        xPow = xJoystickInertia;
        if (iDeltaFrame != 1)
        {
            if (iDeltaFrame == 2) xPow = xJoystickInertia * xJoystickInertia;
            else
            {
                float t = xJoystickInertia * xJoystickInertia;
                xPow = (iDeltaFrame == 3) ? t * xJoystickInertia : t * t;
            }
        }
        xAnalogForce = (IPT_g_FieldPadAnalogForce - xTrueAnalogForce) * xPow + xTrueAnalogForce;

        if (xCenterValue < xAnalogForce - xTrueAnalogForce)
        {
            iRotationAsked = 0;
            stJoyVector.xX = stJoyVector.xY = stJoyVector.xZ = 0.0f;
        }
    }
    else
    {
        xAnalogForce = xTrueAnalogForce;
    }

    if (xAnalogForce < xCenterValue)
    {
        iXValue = 0; iYValue = 0;
        stJoyVector.xX = stJoyVector.xY = stJoyVector.xZ = 0.0f;
        xAngleRotation = 0.0f;
        iComputeStrafeSector = 0;
        goto fin;
    }

    pPersoGlobal = AI_3D_M_pGlobalMatrix(p_SuperObjPerso);

    if (iMovementPlane == 2)
    {

        float xDotX, xDotZ, xJX, xJZ, xTermX, xTermZ;

        if (MTH_M_xAbs(stJoyVector.xX) != 0.0f || MTH_M_xAbs(stJoyVector.xY) != 0.0f || MTH_M_xAbs(stJoyVector.xZ) != 0.0f)
        {
            float xInv = 1.0f / MTH_M_xSqrt(stJoyVector.xX * stJoyVector.xX +
                                      stJoyVector.xY * stJoyVector.xY +
                                      stJoyVector.xZ * stJoyVector.xZ);
            stJoyVector.xX = xInv * stJoyVector.xX * xAnalogForce;
            stJoyVector.xY = xInv * stJoyVector.xY * xAnalogForce;
            stJoyVector.xZ = xInv * stJoyVector.xZ * xAnalogForce;
        }

        POS_fn_vGetRotationVector(pPersoGlobal,                         &pPersoV0, &pPersoV1, &pPersoV2);
        POS_fn_vGetRotationVector(AI_3D_M_pGlobalMatrix(hCamera),       &pCamV0,   &pCamV1,   &pCamV2);

        xDotX = pCamV0->xZ * pPersoV0->xZ + pCamV0->xX * pPersoV0->xX + pCamV0->xY * pPersoV0->xY;
        xDotX = MTH_M_xMul(
            MTH_M_xSign(xDotX),
            MTH_M_xMin(
                MTH_C_ONE,
                MTH_M_xMul(
                    MTH_M_xFloatToReal(1.5f),
                    MTH_M_xDoubleToReal(pow(
                        MTH_M_xRealToDouble(MTH_M_xAbs(xDotX)),
                        AI_PAD_dPowExp
                    )
                )
            )
        ));
        xDotZ = pCamV2->xZ * pPersoV2->xZ + pCamV2->xX * pPersoV2->xX + pCamV2->xY * pPersoV2->xY;
        xDotZ = MTH_M_xMul(
            MTH_M_xSign(xDotZ),
            MTH_M_xMin(
                MTH_C_ONE,
                MTH_M_xMul(
                    MTH_M_xFloatToReal(1.5f),
                    MTH_M_xDoubleToReal(pow(
                        MTH_M_xRealToDouble(MTH_M_xAbs(xDotZ)),
                        AI_PAD_dPowExp
                    )
                )
            )
        ));

        xJX = stJoyVector.xX;
        xJZ = stJoyVector.xZ;
        xTermX = xDotX * xJX;
        xTermZ = xDotZ * xJZ;
        stJoyVector.xX = pPersoV0->xX * xTermX + pPersoV2->xX * xTermZ;
        stJoyVector.xY = pPersoV0->xY * xTermX + pPersoV2->xY * xTermZ;
        stJoyVector.xZ = pPersoV0->xZ * xTermX + pPersoV2->xZ * xTermZ;
        xAngleRotation = 0.0f;
    }
    else
    {

        int   bVectorToTargetIsNull;
        float xInv, xDotProd, xDet, xDynamicRotation;

        stK.xX = 0.0f; stK.xY = 0.0f; stK.xZ = 1.0f;

        POS_fn_vGetTranslationVector(AI_3D_M_pGlobalMatrix(hCamera),  &stI);
        POS_fn_vGetTranslationVector(pPersoGlobal,                   &stJ);
        stJ.xX = stI.xX - stJ.xX;
        stJ.xY = stI.xY - stJ.xY;
        stJ.xZ = stI.xZ - stJ.xZ;
        if (MTH_M_xAbs(stJ.xX) == 0.0f && MTH_M_xAbs(stJ.xY) == 0.0f && MTH_M_xAbs(stJ.xZ) == 0.0f)
        {
            bVectorToTargetIsNull = 1;
        }
        else
        {
            bVectorToTargetIsNull = 0;
            xInv = 1.0f / MTH_M_xSqrt(stJ.xZ * stJ.xZ + stJ.xY * stJ.xY + stJ.xX * stJ.xX);
            stJ.xX *= xInv; stJ.xY *= xInv; stJ.xZ *= xInv;
        }

        if (bVectorToTargetIsNull || iYValue < 0 || AI_PAD_xVertThresh < MTH_M_xAbs(stJ.xZ))
        {

            float xZc;
            POS_fn_vGetRotationVector(AI_3D_M_pGlobalMatrix(hCamera), &pCamV0, &pCamV1, &pCamV2);
            stJ.xX = pCamV1->xX;
            stJ.xY = pCamV1->xY;
            if (MTH_M_xAbs(stJ.xX) == 0.0f && MTH_M_xAbs(stJ.xY) == 0.0f)
            {
                stJ.xX = pCamV2->xX;
                stJ.xY = pCamV2->xY;
            }
            xZc = 0.0f;
            xInv = 1.0f / MTH_M_xSqrt(xZc * xZc + stJ.xY * stJ.xY + stJ.xX * stJ.xX);
            stJ.xX *= xInv; stJ.xY *= xInv; stJ.xZ = xInv * xZc;
        }
        else
        {
            stJ.xZ = 0.0f;
            if (MTH_M_xAbs(stJ.xX) == 0.0f && MTH_M_xAbs(stJ.xY) == 0.0f)
            {
                POS_fn_vGetRotationVector(AI_3D_M_pGlobalMatrix(hCamera), &pCamV0, &pCamV1, &pCamV2);
                stJ.xX = pCamV2->xX;
                stJ.xY = pCamV2->xY;
                if (MTH_M_xAbs(stJ.xX) == 0.0f && MTH_M_xAbs(stJ.xY) == 0.0f) goto fin;
            }
            xInv = 1.0f / MTH_M_xSqrt(stJ.xY * stJ.xY + stJ.xX * stJ.xX + 0.0f);
            stJ.xX *= xInv; stJ.xY *= xInv; stJ.xZ = xInv * 0.0f;
        }

        stI.xX = stJ.xY * stK.xZ - stJ.xZ * stK.xY;
        stI.xY = stJ.xZ * stK.xX - stJ.xX * stK.xZ;
        stI.xZ = stJ.xX * stK.xY - stJ.xY * stK.xX;
        {
            float xJoyX = stJoyVector.xX, xJoyY = stJoyVector.xY, xJoyZ = stJoyVector.xZ;
            stJoyVector.xX = xJoyZ * stK.xX + xJoyY * stJ.xX + xJoyX * stI.xX;
            stJoyVector.xY = xJoyZ * stK.xY + xJoyY * stJ.xY + xJoyX * stI.xY;
            stJoyVector.xZ = xJoyZ * stK.xZ + xJoyY * stJ.xZ + xJoyX * stI.xZ;
        }

        if (MTH_M_xAbs(stJoyVector.xX) == 0.0f && MTH_M_xAbs(stJoyVector.xY) == 0.0f && MTH_M_xAbs(stJoyVector.xZ) == 0.0f)
            goto fin;
        xInv = 1.0f / MTH_M_xSqrt(stJoyVector.xZ * stJoyVector.xZ +
                            stJoyVector.xY * stJoyVector.xY +
                            stJoyVector.xX * stJoyVector.xX);
        stJoyVector.xX *= xInv; stJoyVector.xY *= xInv; stJoyVector.xZ *= xInv;

        POS_fn_vGetRotationVector(pPersoGlobal, &pPersoV0, &pPersoV1, &pPersoV2);
        stSightVector.xX = -pPersoV1->xX;
        stSightVector.xY = -pPersoV1->xY;
        stSightVector.xZ = 0.0f;
        if (MTH_M_xAbs(stSightVector.xX) == 0.0f && MTH_M_xAbs(stSightVector.xY) == 0.0f) goto fin;
        xInv = 1.0f / MTH_M_xSqrt(stSightVector.xY * stSightVector.xY + stSightVector.xX * stSightVector.xX);
        stSightVector.xX *= xInv; stSightVector.xY *= xInv;

        xDotProd = stSightVector.xX * stJoyVector.xX + stSightVector.xY * stJoyVector.xY;
        if (xDotProd > AI_3D_xOne)         xDotProd = AI_3D_xOne;
        else if (xDotProd < AI_PAD_xMinusOne) xDotProd = AI_PAD_xMinusOne;
        xAngleRotation = (float)acos((double)xDotProd);
        xDet = stSightVector.xX * stJoyVector.xY - stSightVector.xY * stJoyVector.xX;
        if (AI_PAD_bNegativeNonZero(xDet)) xAngleRotation = -xAngleRotation;

        stJoyVector.xX *= xAnalogForce;
        stJoyVector.xY *= xAnalogForce;
        stJoyVector.xZ *= xAnalogForce;

        if (iRotationAsked != 0 && AI_PAD_bPositiveNonZero(xRotationSpeed) != 0)
        {
            xDynamicRotation = (float)iDt * AI_PAD_xRotSpeedK * xRotationSpeed * xAngleRotation * xAnalogForce;
            if (MTH_M_xAbs(xDynamicRotation) > MTH_M_xAbs(xAngleRotation))
                xDynamicRotation = xAngleRotation;
            if (MTH_M_xAbs(xDynamicRotation) > xMinimumAngle)
            {
                if (AI_PAD_bPositiveNonZero(xTrueAnalogForce) && iRotationAsked == 1)
                {
                    pPersoGlobal = AI_3D_M_pGlobalMatrix(p_SuperObjPerso);
                    hDynam = AI_3D_M_hDynam(p_SuperObjPerso);
                    if (pPersoGlobal != NULL)
                    {
                        float xCos = (float)cos((double)xDynamicRotation);
                        float xSin = (float)sin((double)xDynamicRotation);
                        MTH3D_tdstMatrix  stRot;
                        MTH3D_tdstMatrix *pM = POS_fn_vGetRotationMatrixReadOnly(pPersoGlobal);

                        stRot.stCol_0.xX = pM->stCol_0.xZ * 0.0f + pM->stCol_0.xY * (-xSin) + pM->stCol_0.xX * xCos;
                        stRot.stCol_0.xY = pM->stCol_0.xZ * 0.0f + pM->stCol_0.xY * xCos    + pM->stCol_0.xX * xSin;
                        stRot.stCol_0.xZ = pM->stCol_0.xZ * 1.0f + pM->stCol_0.xY * 0.0f    + pM->stCol_0.xX * 0.0f;
                        stRot.stCol_1.xX = pM->stCol_1.xZ * 0.0f + pM->stCol_1.xY * (-xSin) + pM->stCol_1.xX * xCos;
                        stRot.stCol_1.xY = pM->stCol_1.xZ * 0.0f + pM->stCol_1.xY * xCos    + pM->stCol_1.xX * xSin;
                        stRot.stCol_1.xZ = pM->stCol_1.xZ * 1.0f + pM->stCol_1.xY * 0.0f    + pM->stCol_1.xX * 0.0f;
                        stRot.stCol_2.xX = pM->stCol_2.xZ * 0.0f + pM->stCol_2.xY * (-xSin) + pM->stCol_2.xX * xCos;
                        stRot.stCol_2.xY = pM->stCol_2.xZ * 0.0f + pM->stCol_2.xY * xCos    + pM->stCol_2.xX * xSin;
                        stRot.stCol_2.xZ = pM->stCol_2.xZ * 1.0f + pM->stCol_2.xY * 0.0f    + pM->stCol_2.xX * 0.0f;

                        POS_fn_vSetRotationMatrix(pPersoGlobal, &stRot.stCol_0, &stRot.stCol_1, &stRot.stCol_2);
                        AI_M_pfn_HIE_vComputeNewRelativeMatrix(p_SuperObjPerso);
                        if (hDynam != NULL)
                            AI_M_pfn_MEC_vImposeRotationMatrix(AI_M_3D_pfn_DynamGetDynamics(hDynam), pPersoGlobal);
                    }
                }
            }
        }
        xAngleRotation = xAngleRotation * AI_PAD_xRadToDeg;
    }

    if (iComputeStrafeSector != 0)
    {
        if (iMovementPlane == 2)
        {
            if (iXValue > iYValue)
                iComputeStrafeSector = (iXValue > -iYValue) ? 2 : 1;
            else
                iComputeStrafeSector = (iXValue > -iYValue) ? 3 : 4;
        }
        else
        {
            float xAbsAngle = MTH_M_xAbs(xAngleRotation);
            if (AI_PAD_xSector45 < xAbsAngle)
            {
                if (xAbsAngle < AI_PAD_xSector135)
                    iComputeStrafeSector = AI_PAD_bPositiveNonZero(xAngleRotation) ? 4 : 2;
                else
                    iComputeStrafeSector = 3;
            }
            else
            {
                iComputeStrafeSector = 1;
            }
        }
    }

fin:

    IPT_g_FieldPadHorizontalAxis   = (short)iXValue;
    IPT_g_FieldPadRotationAngle    = xAngleRotation;
    IPT_g_FieldPadVerticalAxis     = (short)iYValue;
    IPT_g_FieldPadGlobalVector.xX  = stJoyVector.xX;
    IPT_g_FieldPadGlobalVector.xY  = stJoyVector.xY;
    IPT_g_FieldPadGlobalVector.xZ  = stJoyVector.xZ;
    IPT_g_FieldPadSector           = iComputeStrafeSector;
    IPT_g_FieldPadAnalogForce      = xAnalogForce;
    IPT_g_FieldPadTrueAnalogForce  = xTrueAnalogForce;
    return p_stRet;
}

/* 0x0046F830 */
AI_tdstNodeInterpret *__cdecl fn_p_stSetAnalogJoystickAxes(
    HIE_tdstSuperObject *p_SuperObjPerso, AI_tdstNodeInterpret *p_stTree)
{
	long                        eProcedureId = M_eProcedureIdInterpret(p_stTree-1);
	AI_tdstGetSetParam			stParam;

	switch ( eProcedureId )
	{
	case 0x15c:
		AI_PAD_bResetInertia = 1;
		break;

	case 0x15d:
		{
			AI_tdstMind *               p_stMind;
			unsigned char               ucVarId;
			unsigned short uwCenter;
			unsigned long ulBounds;

			M_vEvalNextVarId(p_stMind, ucVarId);

			uwCenter = ((unsigned short)AI_PAD_cCenterY << 8) + (unsigned short)AI_PAD_cCenterX;
			M_Full_GetSetParam_Integer(&stParam, uwCenter);
			ucVarId = fn_ucSetDsgVar(ucVarId, 0, p_stMind, &stParam);

			M_vEvalNextVarId(p_stMind, ucVarId);
			ulBounds = AI_PAD_cXmin + (AI_PAD_cXmax << 8) + (AI_PAD_cYmin << 16) + (AI_PAD_cYmax << 24);
			M_Full_GetSetParam_Integer(&stParam, ulBounds);
			ucVarId = fn_ucSetDsgVar(ucVarId, 0, p_stMind, &stParam);
		}
		break;

	case 0x15e:
		{
			unsigned long ulBounds;
			unsigned short uwCenter;

			M_EvalNextParameter(&stParam);
			/* 0x4B9B5C */
			uwCenter = (unsigned short)M_GetSetParam_ulValue(&stParam);
			AI_PAD_cCenterX = uwCenter & 0xFF;
			AI_PAD_cCenterY = uwCenter >> 8;

			M_EvalNextParameter( &stParam );
			ulBounds = M_GetSetParam_ulValue(&stParam);
			AI_PAD_cXmin = ulBounds & 0xFF;
			if ( AI_PAD_cXmin == 0 )
				AI_PAD_cXmin = C_Pad_DefaultXMinCalibration;
			AI_PAD_cXmax = (ulBounds >> 8) & 0xFF;
			if ( AI_PAD_cXmax == 0 )
				AI_PAD_cXmax = C_Pad_DefaultXMaxCalibration;
			AI_PAD_cYmin = (ulBounds >> 16) & 0xFF;
			if ( AI_PAD_cYmin == 0 )
				AI_PAD_cYmin = C_Pad_DefaultYMinCalibration;
			AI_PAD_cYmax = (ulBounds >> 24) & 0xFF;
			if ( AI_PAD_cYmax == 0 )
				AI_PAD_cYmax = C_Pad_DefaultYMaxCalibration;

			if ( AI_PAD_cCenterX <= AI_PAD_cXmin )
				AI_PAD_cCenterX = AI_PAD_cXmin + 1;
			if ( AI_PAD_cCenterX >= AI_PAD_cXmax )
				AI_PAD_cCenterX = AI_PAD_cXmax - 1;
			if ( AI_PAD_cCenterY <= AI_PAD_cYmin )
				AI_PAD_cCenterY = AI_PAD_cYmin + 1;
			if ( AI_PAD_cCenterY >= AI_PAD_cYmax )
				AI_PAD_cCenterY = AI_PAD_cYmax - 1;
		}
		break;

	case 0x15b:

		M_EvalNextParameter( &stParam );
		AI_PAD_xJoystickAxeX = M_GetSetParam_lValue(&stParam );

		M_EvalNextParameter( &stParam );
		AI_PAD_xJoystickAxeY = M_GetSetParam_lValue(&stParam );
		break;

	default:
		break;
	}

	return p_stTree;
}

/* 0x0046FAA0 */
AI_tdstNodeInterpret *__cdecl fn_p_stInitKeyBoardDirections(
    HIE_tdstSuperObject *p_SuperObjPerso, AI_tdstNodeInterpret *p_stTree)
{
    AI_tdstGetSetParam    stParam;
    AI_tdstNodeInterpret *p_stNext;

    p_stNext = fn_p_stEvalTree(p_SuperObjPerso, p_stTree, &stParam);
    AI_PAD_hKeyUp    = stParam.uParam.ulValue;
    p_stNext = fn_p_stEvalTree(p_SuperObjPerso, p_stNext, &stParam);
    AI_PAD_hKeyDown  = stParam.uParam.ulValue;
    p_stNext = fn_p_stEvalTree(p_SuperObjPerso, p_stNext, &stParam);
    AI_PAD_hKeyLeft  = stParam.uParam.ulValue;
    p_stNext = fn_p_stEvalTree(p_SuperObjPerso, p_stNext, &stParam);
    AI_PAD_hKeyRight = stParam.uParam.ulValue;
    p_stNext = fn_p_stEvalTree(p_SuperObjPerso, p_stNext, &stParam);
    AI_PAD_hKeySlow  = stParam.uParam.ulValue;
    return p_stNext;
}

/* 0x0046FB30 */
AI_tdstNodeInterpret *__cdecl fn_p_stSetCenterPosition(
    HIE_tdstSuperObject *p_SuperObjPerso, AI_tdstNodeInterpret *p_stTree)
{
    long lX = (AI_PAD_hAnalogAxisX != NULL)
              ? (long)AI_M_pfn_IPT_fn_xGetAnalogAxisValue(AI_PAD_hAnalogAxisX) : 0;
    long lY = (AI_PAD_hAnalogAxisY != NULL)
              ? (long)AI_M_pfn_IPT_fn_xGetAnalogAxisValue(AI_PAD_hAnalogAxisY) : 0;
    char cX = (char)lX;
    char cY = (char)lY;
    (void)p_SuperObjPerso;

    AI_PAD_cAnalogValueX = cX;
    AI_PAD_cAnalogValueY = cY;
    if (cX >= AI_PAD_cAnalogMaxX) AI_PAD_cAnalogMaxX = (char)(cX + 1);
    if (cX <= AI_PAD_cAnalogMinX) AI_PAD_cAnalogMinX = (char)(cX - 1);
    if (cY >= AI_PAD_cAnalogMaxY) AI_PAD_cAnalogMaxY = (char)(cY + 1);
    if (cY <= AI_PAD_cAnalogMinY) AI_PAD_cAnalogMinY = (char)(cY - 1);
    return p_stTree;
}

/* 0x0046FBC0 */
AI_tdstNodeInterpret *__cdecl fn_p_stSetMaximalValues(
    HIE_tdstSuperObject *p_SuperObjPerso, AI_tdstNodeInterpret *p_stTree)
{
    short sX = (AI_PAD_hAnalogAxisX != NULL)
               ? (short)(long)AI_M_pfn_IPT_fn_xGetAnalogAxisValue(AI_PAD_hAnalogAxisX) : 0;
    short sY = (AI_PAD_hAnalogAxisY != NULL)
               ? (short)(long)AI_M_pfn_IPT_fn_xGetAnalogAxisValue(AI_PAD_hAnalogAxisY) : 0;
    (void)p_SuperObjPerso;

    if (sY > AI_PAD_cAnalogMaxY)
    {
        AI_PAD_cAnalogMaxY = (char)sY;
    }
    else if (sY < AI_PAD_cAnalogMinY)
    {
        AI_PAD_cAnalogMinY = (char)sY;
    }

    if (sX > AI_PAD_cAnalogMaxX)
    {
        AI_PAD_cAnalogMaxX = (char)sX;
    }
    else if (sX < AI_PAD_cAnalogMinX)
    {
        AI_PAD_cAnalogMinX = (char)sX;
    }
    return p_stTree;
}

/* 0x0046FC60 */
AI_tdstNodeInterpret *__cdecl fn_p_stSetBooleanInArray(
    HIE_tdstSuperObject *p_SuperObjPerso, AI_tdstNodeInterpret *p_stTree)
{
	AI_tdstGetSetParam			stParam;
	long						eProcedureId = M_eProcedureIdInterpret(p_stTree-1);
	unsigned char				ucArrayId;
	unsigned long				ulIndex;
	ACP_tdxBool					bState;
	AI_tdstMind *				p_stMind;

  M_vEvalNextVarId(p_stMind, ucArrayId);

	M_EvalNextParameter(&stParam);
	ulIndex = M_GetSetParam_lValue(&stParam);
	ulIndex--;

	M_EvalNextParameter(&stParam);
	bState = (M_GetSetParam_lValue(&stParam) ? TRUE : FALSE);

	switch(eProcedureId)
	{
	case 0x15f:
		{
			unsigned long	ulPos		= (ulIndex >> 5);
			unsigned long	ulBit		= (ulIndex & 31);

			if( fn_ucGetDsgVar(ucArrayId , (unsigned char)ulPos , p_stMind , &stParam) == C_VALID_GET )
			{
				unsigned ulValue = M_GetSetParam_lValue(&stParam);
				if(bState)
				{
					ulValue |= (1 << ulBit);
				}
				else
				{
					ulValue &= ~(1 << ulBit);
				}

				M_Full_GetSetParam_Integer(&stParam , ulValue);

				ucArrayId = fn_ucSetDsgVar(ucArrayId , (unsigned char)ulPos , p_stMind , &stParam);
			}
			break;
		}

	default:

		M_AIFatalError(0x28  );
		break;
	}
	return p_stTree;
}

/* 0x0046FD70 */
AI_tdstNodeInterpret *__cdecl fn_p_stSetDefaultFormatCharacter(
    HIE_tdstSuperObject *p_SuperObjPerso, AI_tdstNodeInterpret *p_stTree)
{
    AI_tdstGetSetParam    stParam;
    float                 xCharWidth, xLineHeight, xCharZoom;
    AI_tdstNodeInterpret *p_stNext;

    p_stNext    = fn_p_stEvalTree(p_SuperObjPerso, p_stTree, &stParam);
    xCharWidth  = stParam.uParam.xValue;
    p_stNext    = fn_p_stEvalTree(p_SuperObjPerso, p_stNext, &stParam);
    xLineHeight = stParam.uParam.xValue;
    p_stNext    = fn_p_stEvalTree(p_SuperObjPerso, p_stNext, &stParam);
    xCharZoom   = stParam.uParam.xValue;

    TFMT_vSetDefaultValuesCharacter(xCharWidth, xLineHeight, xCharZoom);
    return p_stNext;
}

/* 0x0046FDE0 */
AI_tdstNodeInterpret *__cdecl fn_p_stSelectDriver(
    HIE_tdstSuperObject *p_SuperObjPerso, AI_tdstNodeInterpret *p_stTree)
{
    AI_tdstGetSetParam stParam;

    return fn_p_stEvalTree(p_SuperObjPerso, p_stTree, &stParam);
}
AI_tdstNodeInterpret *__cdecl fn_p_stSelectResolution(
    HIE_tdstSuperObject *p_SuperObjPerso, AI_tdstNodeInterpret *p_stTree)
{
    AI_tdstGetSetParam    stParam;
    AI_tdstNodeInterpret *p_stNext = fn_p_stEvalTree(p_SuperObjPerso, p_stTree, &stParam);
    GLI_fn_lRequestNewDisplayMode(stParam.uParam.lValue);
    return p_stNext;
}

/* 0x0046FE40 */
AI_tdstNodeInterpret *__cdecl fn_p_stChangeBrightness(
    HIE_tdstSuperObject *p_SuperObjPerso, AI_tdstNodeInterpret *p_stTree)
{
    AI_tdstGetSetParam    stParam;
    AI_tdstNodeInterpret *p_stNext = fn_p_stEvalTree(p_SuperObjPerso, p_stTree, &stParam);
    GLI_fn_vSetBrightness(stParam.uParam.lValue);
    return p_stNext;
}
AI_tdstNodeInterpret *__cdecl fn_p_stSetInStereoMode(
    HIE_tdstSuperObject *p_SuperObjPerso, AI_tdstNodeInterpret *p_stTree)
{
    AI_tdstGetSetParam    stParam;
    AI_tdstNodeInterpret *p_stNext = fn_p_stEvalTree(p_SuperObjPerso, p_stTree, &stParam);
    SND_fn_vSetStereoSound((ACP_tdxBool)stParam.uParam.lValue);
    return p_stNext;
}

/* 0x0046FEC0 */
AI_tdstNodeInterpret *__cdecl AI_fn_p_stPcSetSoundGlobal(
    HIE_tdstSuperObject *p_SuperObjPerso, AI_tdstNodeInterpret *p_stTree)
{
    AI_tdstGetSetParam    stParam;
    AI_tdstNodeInterpret *p_stNext = fn_p_stEvalTree(p_SuperObjPerso, p_stTree, &stParam);
    AI_M_pfn_PcSetSoundGlobal453850((double)stParam.uParam.xValue);
    return p_stNext;
}

AI_tdstNodeInterpret *__cdecl MGT_fn_p_stActiveMagnet( HIE_tdstSuperObject *p_SuperObjPerso, AI_tdstNodeInterpret *p_stTree);

/* 0x0046FF00 */
AI_tdstNodeInterpret *__cdecl fn_p_stSetaturationAndBackgroundDistance(
    HIE_tdstSuperObject *p_SuperObjPerso, AI_tdstNodeInterpret *p_stTree)
{
    unsigned long         procId  = (unsigned long)M_eProcedureIdInterpret(p_stTree-1);
    AI_tdstNodeInterpret *p_stNext = p_stTree;
    HIE_tdstSuperObject  *p_SuperObjEval = p_SuperObjPerso;
    void                 *hMSSound;

    fn_vGetUltraOperatorPerso(fn_ucGetProcedureUltraOperator(procId), p_SuperObjPerso,
                              &p_SuperObjPerso);
    hMSSound = ((struct tdstEngineObject_ *)AI_3D_M_pEngineObj(p_SuperObjPerso))->h_MSSound;

    if (hMSSound != NULL)
    {
        AI_tdstGetSetParam stParam;
        p_stNext = fn_p_stEvalTree(p_SuperObjEval, p_stNext, &stParam);
        AI_M_pfn_MSSoundSetSaturationDistance(hMSSound, M_GetSetParam_lValue(&stParam) << 16);
        p_stNext = fn_p_stEvalTree(p_SuperObjEval, p_stNext, &stParam);
        AI_M_pfn_MSSoundSetBackGroundDistance(hMSSound, M_GetSetParam_lValue(&stParam) << 16);
    }
    return p_stNext;
}

/* 0x0046FF90 */
AI_tdstNodeInterpret *__cdecl fn_p_stSaveCurrentSoundRequest(
    HIE_tdstSuperObject *p_SuperObjPerso, AI_tdstNodeInterpret *p_stTree)
{
    unsigned long         procId = (unsigned long)M_eProcedureIdInterpret(p_stTree-1);
    AI_tdstGetSetParam    stParam;
    AI_tdstNodeInterpret *p_stNext = fn_p_stEvalTree(p_SuperObjPerso, p_stTree, &stParam);

    switch (procId)
    {
    case 0x16d:
    {
        void *p_stBlockEvent = AI_M_pfn_SND_pstGetLastEventForType(GAM_g_lSoundObjectTypeMusic);
        AI_M_pfn_GAM_lSaveBlockEventSlotMemory(p_stBlockEvent, 0);
        break;
    }
    case 0x16e:
        break;
    default:
        AI_M_pfn_M_AIFatalError(0x28);
        break;
    }
    return p_stNext;
}
#define AI_M_pfn_CAM_fn_vSendParametersToParsing ((void (__cdecl *)(void *))CAM_fn_vSendParametersToParsing)

#include "GAM/STRUCTUR/Engmode.h"

#include "AI/AI_Erm.h"
#ifdef RAYMAN2_NO_PROTECTION
void __cdecl AI_pc_fn_vSetProtectionActor(HIE_tdstSuperObject *p_SuperObjPerso);   /* Function.c */
#endif /* RAYMAN2_NO_PROTECTION */

/* 0x00470000 */
AI_tdstNodeInterpret *__cdecl fn_p_stMiscProtectProcedure(
    HIE_tdstSuperObject *p_SuperObjPerso, AI_tdstNodeInterpret *p_stTree)
{
    unsigned long         ulProcId = (unsigned long)M_eProcedureIdInterpret(p_stTree-1);
    unsigned long         ulModule;
    unsigned char        *p_ucStub;
    unsigned char        *p_ucScrub;
    void                 *hProcess;
    AI_tdstMind          *p_stMind;
    unsigned char         ucVarId;
    AI_tdstGetSetParam    stParam;

#ifndef RAYMAN2_NO_PROTECTION
    MMG_pc_M_vStepLcg();
    p_ucStub = &MMG_pc_g_uLcgPool.a_ucCode[(int)MMG_g_ulLCGState % MMG_pc_C_PoolSlots];
    MMG_pc_M_vWriteStub(p_ucStub, MMG_pc_C_ImpGetModuleHandleA, 0xFF, 0xE0);
    MMG_pc_M_vFlushPool(hProcess);
    ulModule = (unsigned long)((void *(__stdcall *)(unsigned long))(void *)p_ucStub)(0);

    for (p_ucScrub = p_ucStub; p_ucScrub < p_ucStub + MMG_pc_C_StubSize; p_ucScrub++)
    {
        MMG_pc_M_vStepLcg();
        *p_ucScrub = (unsigned char)((int)MMG_g_ulLCGState % 0xff);
    }
#else
    AI_pc_fn_vSetProtectionActor(p_SuperObjPerso);   /* see Function.c */
#endif /* RAYMAN2_NO_PROTECTION */

    if (ulProcId != 0x170)
    {
        M_AIFatalError(E_uwAIFatalNotValidProcedure);
        return p_stTree;
    }

    M_vEvalNextVarId(p_stMind, ucVarId);

    if (fn_ucGetDsgVar(ucVarId, 0, p_stMind, &stParam) == 1)
    {
        unsigned __int64 ullSum = *(unsigned __int64 *)M_GetSetParam_Param(&stParam);
        unsigned long    ulKeep = ((unsigned long *)M_GetSetParam_Param(&stParam))[2];
        unsigned long    ulValue;

        M_EvalNextParameter(&stParam);
#ifndef RAYMAN2_NO_PROTECTION   /* ACT_UpdateChecksum: sum of squared dwords of the running code image */
        ulValue = *(unsigned long *)(ulModule + M_GetSetParam_lValue(&stParam));
        ullSum += (unsigned __int64)ulValue * (unsigned __int64)ulValue;

        *(unsigned __int64 *)M_GetSetParam_Param(&stParam)  = ullSum;
        ((unsigned long *)M_GetSetParam_Param(&stParam))[2] = ulKeep;
        M_GetSetParam_Type(&stParam) = E_vt_Vector;
        fn_ucSetDsgVar(ucVarId, 0, p_stMind, &stParam);
#endif /* RAYMAN2_NO_PROTECTION */
    }
    return p_stTree;
}

#if !(defined(LINUX) || defined(OSX) || defined(PSP) || defined(WIIU))
AI_tdstNodeInterpret *__cdecl fn_p_stShellExecuteProcedure(
    HIE_tdstSuperObject *p_SuperObjPerso, AI_tdstNodeInterpret *p_stTree)
{
    AI_tdstGetSetParam    stParam;
    AI_tdstNodeInterpret *p_stNext;
    char                 *szFile;

    if ((unsigned long)M_eProcedureIdInterpret(p_stTree-1) != 0x171)
    {
        fn_vOnAIErm_M_UpdateLastError(0x28, (char *)0);
        return p_stTree;
    }
    p_stNext = fn_p_stEvalTree(p_SuperObjPerso, p_stTree, &stParam);
    szFile   = fn_szGetStringFromTextOrStringParam(&stParam);
    ShellExecuteA((void *)0, (const char *)0, szFile, (const char *)0, (const char *)0, 0);
    return p_stNext;
}
#endif /* LINUX || OSX || PSP || WIIU */

/* 0x00470270 */
AI_tdstNodeInterpret *__cdecl fn_p_stTextSuperImposed(
    HIE_tdstSuperObject *p_SuperObjPerso, AI_tdstNodeInterpret *p_stTree)
{
  AI_tdstGetSetParam stParam;
  M_EvalNextParameter(&stParam);
  TEXT_gc_ViewportFixed = (char) M_GetSetParam_lValue(&stParam);
  return p_stTree;
}
AI_tdstNodeInterpret *__cdecl fn_p_stCode4ValideNomDuSlot(
    HIE_tdstSuperObject *p_SuperObjPerso, AI_tdstNodeInterpret *p_stTree)
{
    long                  procId = M_eProcedureIdInterpret(p_stTree-1);
    AI_tdstGetSetParam    stParam;
    AI_tdstNodeInterpret *p_stNext;
    long                  lSlot;
    long                  lScore;
    long                  lWalk;
    char                 *pName;
    char                 *pNode;
    char                 *pOldHead;

    p_stNext = fn_p_stEvalTree(p_SuperObjPerso, p_stTree, &stParam);
    lSlot = stParam.uParam.lValue;
    p_stNext = fn_p_stEvalTree(p_SuperObjPerso, p_stNext, &stParam);
    pName = fn_szGetStringFromTextOrStringParam(&stParam);
    p_stNext = fn_p_stEvalTree(p_SuperObjPerso, p_stNext, &stParam);
    lScore = stParam.uParam.lValue;
    if (lScore < 0)
    {
        lScore = 0;
    }
    if (lScore > 1000)
    {
        lScore = 1000;
    }

    if (pName == NULL)
    {
        GAM_g_a_szOptionsSlotNames[lSlot - 1][8] = 0;
        return p_stNext;
    }

    if (procId != 0x1af)
    {
        return p_stNext;
    }

    if (lSlot == 0)
    {
        GAM_pc_fn_lEnsureDefaultSaveSlot();
        return p_stNext;
    }

    while (*pName == '/')
    {
        pName++;
        while (*pName != ':')
        {
            pName++;
        }
        pName++;
    }
    pName[3] = '\0';

    pNode = (char *)GAM_gs_pSaveSlotHead;
    if (GAM_gs_lNbSaveSlots < lSlot)
    {
        pOldHead = (char *)GAM_gs_pSaveSlotHead;
        pNode = (char *)GLI_pc_fn_p_vAlloc(0x20);
        GAM_gs_pSaveSlotHead = pNode;
        *(void **)(pNode + 4) = pOldHead;
        GAM_gs_lNbSaveSlots = GAM_gs_lNbSaveSlots + 1;
        *(long *)pNode = GAM_gs_lNbSaveSlots;
        GAM_gs_ulSaveSlotCount2 = 0;
    }
    else
    {
        GAM_gs_ulSaveSlotCount2 = 1;
        if (lSlot > 1)
        {
            lWalk = lSlot - 1;
            do {
                pNode = *(char **)(pNode + 4);
                lWalk--;
            } while (lWalk != 0);
        }
    }

    strcpy(pNode + 0x13, pNode + 8);

    sprintf(pNode + 8, (lScore < 100) ? "%3s 0%2.1f" : "%3s %2.1f", GAM_g_szSlotNameWork_storage, (double)((float)lScore * 0.1f));

    return p_stNext;
}
AI_tdstNodeInterpret *__cdecl fn_p_stCode4RestoreNomDuSlot(
    HIE_tdstSuperObject *p_SuperObjPerso, AI_tdstNodeInterpret *p_stTree)
{
    long                  procId = M_eProcedureIdInterpret(p_stTree-1);
    AI_tdstGetSetParam    stParam;
    AI_tdstNodeInterpret *p_stNext;
    char                 *pHead;
    char                 *pSrc;
    char                 *pDst;

    p_stNext = fn_p_stEvalTree(p_SuperObjPerso, p_stTree, &stParam);

    if (procId == 0x1b0)
    {
        if (stParam.uParam.lValue == 0)
        {
            GAM_pc_fn_lEnsureDefaultSaveSlot();
        }
        pHead = *(char **)&GAM_gs_pSaveSlotHead;
        pSrc = pHead + 0x13;
        pDst = pHead + 8;
        strcpy(pDst, pSrc);
    }
    return p_stNext;
}

/* 0x004704C0 */
AI_tdstNodeInterpret *__cdecl fn_p_stTextIntegerParameter(
    HIE_tdstSuperObject *p_SuperObjPerso, AI_tdstNodeInterpret *p_stTree)
{
    AI_tdstGetSetParam    stParam;
    long                  l_NumInt, l_ValInt;
    AI_tdstNodeInterpret *p_stNext;

    p_stNext = fn_p_stEvalTree(p_SuperObjPerso, p_stTree, &stParam);
    l_NumInt = stParam.uParam.lValue;
    p_stNext = fn_p_stEvalTree(p_SuperObjPerso, p_stNext, &stParam);
    l_ValInt = stParam.uParam.lValue;

    if (l_NumInt >= 10) l_NumInt = 9;
    AI_TEXT_gai_IntParameters[l_NumInt] = l_ValInt;
    return p_stNext;
}

/* 0x00470510 */
AI_tdstNodeInterpret *__cdecl fn_p_stTextRealParameter(
    HIE_tdstSuperObject *p_SuperObjPerso, AI_tdstNodeInterpret *p_stTree)
{
    AI_tdstGetSetParam    stParam;
    long                  l_NumInt;
    float                 x_ValReal;
    AI_tdstNodeInterpret *p_stNext;

    p_stNext  = fn_p_stEvalTree(p_SuperObjPerso, p_stTree, &stParam);
    l_NumInt  = stParam.uParam.lValue;
    p_stNext  = fn_p_stEvalTree(p_SuperObjPerso, p_stNext, &stParam);
    x_ValReal = stParam.uParam.xValue;

    if (l_NumInt >= 10) l_NumInt = 9;
    AI_TEXT_gax_RealParameters[l_NumInt] = x_ValReal;
    return p_stNext;
}

/* 0x00470560 */
AI_tdstNodeInterpret *__cdecl fn_p_stCode4FormateTexte(
    HIE_tdstSuperObject *p_SuperObjPerso, AI_tdstNodeInterpret *p_stTree)
{
    unsigned long         procId = (unsigned long)M_eProcedureIdInterpret(p_stTree-1);
    AI_tdstGetSetParam    stParam;
    AI_tdstNodeInterpret *p_stNext;
    char                 *pszStringFormat;
    char                 *pszStringSource;
    char                 *pszStringDestination;

    p_stNext             = fn_p_stEvalTree(p_SuperObjPerso, p_stTree, &stParam);
    pszStringFormat      = fn_szGetStringFromTextOrStringParam(&stParam);
    p_stNext             = fn_p_stEvalTree(p_SuperObjPerso, p_stNext, &stParam);
    pszStringSource      = fn_szGetStringFromTextOrStringParam(&stParam);
    p_stNext             = fn_p_stEvalTree(p_SuperObjPerso, p_stNext, &stParam);
    pszStringDestination = fn_szGetStringFromTextOrStringParam(&stParam);

    if (procId == 0x1ae)
    {
        char szTmpString[256];
        strcpy(szTmpString, pszStringFormat);
        strcat(szTmpString, pszStringSource);
        strcpy(pszStringDestination, szTmpString);
    }
    return p_stNext;
}

/* 0x00470660 */
AI_tdstNodeInterpret *__cdecl fn_p_stCode4ChangeCaractere(
    HIE_tdstSuperObject *p_SuperObjPerso, AI_tdstNodeInterpret *p_stTree)
{
  AI_tdstGetSetParam stParam;
  tdstEngineObject *p_stEngineObject = NULL;

  unsigned long eProcedureId = M_eProcedureIdInterpret(p_stTree - 1);

  unsigned char ucPosLetter = 0 , ucLetter = 0 ;
  char          *pszString = NULL ;

  M_EvalNextParameter(&stParam);
  p_stEngineObject = M_GetSetParam_p_stEngineObjValue(&stParam);

  M_EvalNextParameter(&stParam);
  pszString = (char *)fn_szGetStringFromTextOrStringParam(&stParam);

  M_EvalNextParameter(&stParam);
  ucPosLetter = M_GetSetParam_ucValue(&stParam);

  M_EvalNextParameter(&stParam);
  ucLetter = M_GetSetParam_ucValue(&stParam);

  switch ( eProcedureId )
  {
    case 0x1ad:
        {
					if  ( pszString )
					{
            register int iCpt = 0;
						int iStrLen = strlen(pszString), iTrueLetterPos;

            while( pszString[iCpt++] == '/')
            {
                while( pszString[iCpt++] != ':' )
				{}
            }
						iTrueLetterPos = ucPosLetter + iCpt - 1;

            if ( iTrueLetterPos > iStrLen )
            {
                pszString[iStrLen - 1] = ucLetter;
            }
            else
            {
                pszString[iTrueLetterPos] = ucLetter;
            }

            TFMT_vChangeLetter( M_GetSuperObject(p_stEngineObject), ucPosLetter, ucLetter );
        }
        }
        break;

	default:
		break;
  }

  return p_stTree;
}

/* 0x00470770 */
AI_tdstNodeInterpret *__cdecl fn_p_stCode4AirPointsProcedures(
    HIE_tdstSuperObject *p_SuperObjPerso, AI_tdstNodeInterpret *p_stTree)
{
    AI_tdstGetSetParam    stParam;
    long                  eProcedureId = M_eProcedureIdInterpret(p_stTree-1);
    MTH_tdxReal           xAirPoints;
    MTH_tdxReal           xSign = MTH_C_ONE;

    M_EvalNextParameter(&stParam);
    xAirPoints = M_GetSetParam_xValue(&stParam);

    switch(eProcedureId)
    {
        case 0x17c:
        case 0x17a:
            /* 0x4E9308 */
            AI_AIR_xPoints = xAirPoints;
            return(p_stTree);
            break;

        case 0x17d:
        case 0x17b:
            /* 0x4E930C */
            AI_AIR_xPointsMax = xAirPoints;
            return(p_stTree);
            break;

        case 0x180:
            xSign = MTH_C_MinusONE;

        case 0x17e:
            AI_AIR_xPoints = MTH_M_xAdd(AI_AIR_xPoints, MTH_M_xMul(xSign, xAirPoints));

            if ( AI_AIR_xPoints > AI_AIR_xPointsMax )
                AI_AIR_xPoints = AI_AIR_xPointsMax;
            else if ( AI_AIR_xPoints < 0 )
                AI_AIR_xPoints = 0;
            return(p_stTree);
            break;

        case 0x181:
            xSign = MTH_C_MinusONE;

        case 0x17f:

            AI_AIR_xPointsMax = MTH_M_xAdd(AI_AIR_xPointsMax, MTH_M_xMul(xSign, xAirPoints));

            if ( AI_AIR_xPointsMax < 0 )
                AI_AIR_xPointsMax = 0;
            if ( AI_AIR_xPoints > AI_AIR_xPointsMax )
                AI_AIR_xPoints = AI_AIR_xPointsMax;
            return(p_stTree);
            break;

        default:
            break;
    }

    return(p_stTree);
}

/* 0x004708B0 */
AI_tdstNodeInterpret *__cdecl fn_p_stCode4MagicPointsProcedures(
    HIE_tdstSuperObject *p_SuperObjPerso, AI_tdstNodeInterpret *p_stTree)
{
    unsigned long       procId = (unsigned long)M_eProcedureIdInterpret(p_stTree-1);
    AI_tdstGetSetParam  stParam;
    unsigned char       iMagicPoints;

    p_stTree     = fn_p_stEvalTree(p_SuperObjPerso, p_stTree, &stParam);
    iMagicPoints = stParam.uParam.ucValue;

    switch (procId)
    {
    case 0x172:
    case 0x174:
        AI_MAGIC_iPoints = iMagicPoints;
        return p_stTree;
    case 0x173:
    case 0x175:
        AI_MAGIC_iPointsMax = iMagicPoints;
        return p_stTree;
    case 0x176:
        AI_MAGIC_iPoints += iMagicPoints;
        if (AI_MAGIC_iPoints > AI_MAGIC_iPointsMax)
            AI_MAGIC_iPoints = AI_MAGIC_iPointsMax;
        return p_stTree;
    case 0x178:
        AI_MAGIC_iPoints -= iMagicPoints;
        if (AI_MAGIC_iPoints < 0)
            AI_MAGIC_iPoints = 0;
        return p_stTree;
    case 0x177:
        AI_MAGIC_iPointsMax += iMagicPoints;
        return p_stTree;
    case 0x179:
        AI_MAGIC_iPointsMax -= iMagicPoints;
        if (AI_MAGIC_iPointsMax < 0)
            AI_MAGIC_iPointsMax = 0;
        return p_stTree;
    default:
        break;
    }
    return p_stTree;
}

AI_tdstNodeInterpret *__cdecl fn_p_stSetLocalLight(
    HIE_tdstSuperObject *p_SuperObjPerso, AI_tdstNodeInterpret *p_stTree)
{
	AI_tdstGetSetParam stParam;
	unsigned long eProcedureId = M_eProcedureIdInterpret(p_stTree-1);
	MS_tdxHandleToMSLight hMSLight;

	if(eProcedureId == 0x183  ) {
		HIE_tdstSuperObject *p_SuperObjPersoTreated;
		unsigned char bLocalLight;
		unsigned char bOnlyLocalLight;

		M_EvalNextParameter(&stParam);
		p_SuperObjPersoTreated=M_GetSetParam_p_stSupObjValue(&stParam);

		M_EvalNextParameter(&stParam);
		bLocalLight = M_GetSetParam_ucValue(&stParam);

		M_EvalNextParameter(&stParam);
		bOnlyLocalLight = M_GetSetParam_ucValue(&stParam);

		hMSLight = M_GetMSHandle(p_SuperObjPersoTreated,MSLight);

		if (!hMSLight)
			return(p_stTree);

		if (bLocalLight) {

			SECT_fn_vIsolateCharListNode(fn_hMSLightGetLightNodeInSector(hMSLight));

			if (bOnlyLocalLight) { p_SuperObjPersoTreated->ulFlags |= HIE_C_Flag_ExcluLight; }
			else { p_SuperObjPersoTreated->ulFlags &= ~HIE_C_Flag_ExcluLight; }

			fn_vMSLightSetLocalLight(hMSLight,TRUE);
			fn_vMSLightSetOnlyLocalLight(hMSLight,bOnlyLocalLight);
		}
		else {
			HIE_tdstSuperObject *hSector = GAM_fn_hGetCurrentSector(p_SuperObjPersoTreated);

			if (bOnlyLocalLight) { p_SuperObjPersoTreated->ulFlags |= HIE_C_Flag_ExcluLight; }
			else { p_SuperObjPersoTreated->ulFlags &= ~HIE_C_Flag_ExcluLight; }

			if (hSector) {
				SECT_fn_vIsolateCharListNode(fn_hMSLightGetLightNodeInSector(hMSLight));
				SECT_fn_vAddTailDNMLightsListNode(hSector,fn_hMSLightGetLightNodeInSector(hMSLight));
			}

			fn_vMSLightSetLocalLight(hMSLight,FALSE);
			fn_vMSLightSetOnlyLocalLight(hMSLight, bOnlyLocalLight);
		}

		return(p_stTree);
	}

	return(p_stTree);
}

AI_tdstNodeInterpret *__cdecl fn_p_stSetLight(
    HIE_tdstSuperObject *p_SuperObjPerso, AI_tdstNodeInterpret *p_stTree)
{
    unsigned long         procId  = (unsigned long)M_eProcedureIdInterpret(p_stTree-1);
    AI_tdstGetSetParam    stParam;
    AI_tdstNodeInterpret *p_stNext = p_stTree;
    void                 *hLight   = (void *)0;

    if (procId != 0x187)
    {
        p_stNext = fn_p_stEvalTree(p_SuperObjPerso, p_stTree, &stParam);
        hLight   = (void *)stParam.uParam.lValue;
    }

    switch (procId)
    {
    case 0x184:
        p_stNext = fn_p_stEvalTree(p_SuperObjPerso, p_stNext, &stParam);
        AI_M_pfn_GLI_xSetLightState(hLight, stParam.uParam.ucValue);
        return p_stNext;
    case 0x185:
    {
        float xNear, xFar;
        p_stNext = fn_p_stEvalTree(p_SuperObjPerso, p_stNext, &stParam);
        xNear    = stParam.uParam.xValue;
        p_stNext = fn_p_stEvalTree(p_SuperObjPerso, p_stNext, &stParam);
        xFar     = stParam.uParam.xValue;
        AI_M_pfn_GLI_vSetLightNearFar(hLight, xNear, xFar);
        return p_stNext;
    }
    case 0x186:
    {
        GEO_tdstColor stColor;
        p_stNext = fn_p_stEvalTree(p_SuperObjPerso, p_stNext, &stParam);
        stColor.xR = stParam.uParam.xValue;
        p_stNext = fn_p_stEvalTree(p_SuperObjPerso, p_stNext, &stParam);
        stColor.xG = stParam.uParam.xValue;
        p_stNext = fn_p_stEvalTree(p_SuperObjPerso, p_stNext, &stParam);
        stColor.xB = stParam.uParam.xValue;
        p_stNext = fn_p_stEvalTree(p_SuperObjPerso, p_stNext, &stParam);
        stColor.xA = stParam.uParam.xValue;
        AI_M_pfn_GLI_xSetLightColor(hLight, &stColor);
        return p_stNext;
    }
    case 0x187:
        fn_vCalculeLumiereEffet(SECT_hFatherSector);
        return p_stNext;
    default:
        break;
    }
    return p_stNext;
}

AI_tdstNodeInterpret *__cdecl fn_p_stCapabilityProcedure(
    HIE_tdstSuperObject *p_SuperObjPerso, AI_tdstNodeInterpret *p_stTree)
{
    unsigned long         procId = (unsigned long)M_eProcedureIdInterpret(p_stTree-1);
    HIE_tdstSuperObject  *hNewSuperObjPerso;
    void                 *hEngineObj;
    AI_tdstGetSetParam    stParam;
    AI_tdstNodeInterpret *p_stNext;
    unsigned long         ubf32Capabilities;

    fn_vGetUltraOperatorPerso(fn_ucGetProcedureUltraOperator(procId), p_SuperObjPerso, &hNewSuperObjPerso);

    hEngineObj = M_GetEngineObject(hNewSuperObjPerso);

    p_stNext = fn_p_stEvalTree(p_SuperObjPerso, p_stTree, &stParam);
    ubf32Capabilities = stParam.uParam.ulValue;

    switch (procId)
    {
    case 0x18c:
        AI_M_pfn_fn_vSetCapabilities(hEngineObj, ubf32Capabilities);
        return p_stNext;
    case 0x18d:
        AI_M_pfn_fn_vAddCapabilities(hEngineObj, ubf32Capabilities);
        return p_stNext;
    case 0x18e:
        AI_M_pfn_fn_vSubCapabilities(hEngineObj, ubf32Capabilities);
        return p_stNext;
    case 0x18f:
        p_stNext = fn_p_stEvalTree(p_SuperObjPerso, p_stNext, &stParam);
        AI_M_pfn_fn_vChangeCapabilities(hEngineObj, ubf32Capabilities, stParam.uParam.lValue);
        return p_stNext;
    default:
        break;
    }
    return p_stNext;
}

AI_tdstNodeInterpret *__cdecl fn_p_stReinitGraphProcedure(
    HIE_tdstSuperObject *p_SuperObjPerso, AI_tdstNodeInterpret *p_stTree)
{
    AI_tdstGetSetParam    stParam;
    AI_tdstNodeInterpret *p_stNext;

    p_stNext = fn_p_stEvalTree(p_SuperObjPerso, p_stTree, &stParam);
    AI_M_pfn_WPG_fn_vReinitGraph((void *)stParam.uParam.lValue);
    return p_stNext;
}

AI_tdstNodeInterpret *__cdecl fn_p_stSetScrollSpeed(
    HIE_tdstSuperObject *p_SuperObjPerso, AI_tdstNodeInterpret *p_stTree)
{
    unsigned long         procId = (unsigned long)M_eProcedureIdInterpret(p_stTree-1);
    AI_tdstGetSetParam    stParam;
    AI_tdstNodeInterpret *p_stNext;
    void                 *hMaterial;

    p_stNext  = fn_p_stEvalTree(p_SuperObjPerso, p_stTree, &stParam);
    hMaterial = stParam.uParam.p_stEngObjValue;

    if (procId == 0x191)
    {
        float xU, xV;
        p_stNext = fn_p_stEvalTree(p_SuperObjPerso, p_stNext, &stParam);
        xU = stParam.uParam.xValue * 0.01f;
        p_stNext = fn_p_stEvalTree(p_SuperObjPerso, p_stNext, &stParam);
        xV = stParam.uParam.xValue * 0.01f;
        AI_M_pfn_GLI_xSetMaterialTextureScrollingCoef(
            AI_M_pfn_GMT_fn_hGetVisualMaterial(hMaterial), 0, 1, xU, xV);
        return p_stNext;
    }
    if (procId == 0x192)
    {
        p_stNext = fn_p_stEvalTree(p_SuperObjPerso, p_stNext, &stParam);
        AI_M_pfn_GLI_xSetMaterialTextureScrollingOnOff(
            AI_M_pfn_GMT_fn_hGetVisualMaterial(hMaterial), stParam.uParam.ucValue);
        return p_stNext;
    }
    if (procId == 0x193)
    {
        float xU, xV;
        p_stNext = fn_p_stEvalTree(p_SuperObjPerso, p_stNext, &stParam);
        xU = stParam.uParam.xValue * 0.01f;
        p_stNext = fn_p_stEvalTree(p_SuperObjPerso, p_stNext, &stParam);
        xV = stParam.uParam.xValue * 0.01f;
        AI_M_pfn_GLI_xSetMaterialTextureDecalage(
            AI_M_pfn_GMT_fn_hGetVisualMaterial(hMaterial), 0, 2, xU, xV);
        return p_stNext;
    }
    if (procId == 0x194)
    {
        p_stNext = fn_p_stEvalTree(p_SuperObjPerso, p_stNext, &stParam);
        AI_M_pfn_GLI_xSetMaterialTextureScrollingOnOff(
            AI_M_pfn_GMT_fn_hGetVisualMaterial(hMaterial),
            stParam.uParam.ucValue ? 3 : 1);
        return p_stNext;
    }
    AI_M_pfn_M_AIFatalError(0x27);
    return p_stNext;
}

AI_tdstNodeInterpret *__cdecl fn_p_stRLIProcedure(
    HIE_tdstSuperObject *p_SuperObjPerso, AI_tdstNodeInterpret *p_stTree)
{
	long eProcedureId=M_eProcedureIdInterpret(p_stTree-1);
	MS_tdxHandleToMSLight hMSLight ;
	AI_tdstGetSetParam stParam;
	long lIndex1 = -1;
	long lIndex2 = -1;
	long lNbRLI = -1;
	long lPercent = -1;

  HIE_tdstSuperObject *hNewSuperObjPerso;
	fn_vGetUltraOperatorPerso(fn_ucGetProcedureUltraOperator(eProcedureId),p_SuperObjPerso,&hNewSuperObjPerso);

	hMSLight = M_GetMSHandle(hNewSuperObjPerso,MSLight);

	switch(eProcedureId) {
		case 0x188:
			lNbRLI   = 0 ;
			lIndex1  = 0 ;
			lIndex2  = 0 ;
			lPercent = 0 ;
			break ;
		case 0x189:
			lNbRLI   = 1 ;
			M_EvalNextParameter(&stParam);
			lIndex1  = M_GetSetParam_lValue(&stParam);
			lIndex2  = 0 ;
			lPercent = 0 ;
			break ;
		case 0x18a:
			lNbRLI   = 2 ;
			M_EvalNextParameter(&stParam);
			lIndex1  = M_GetSetParam_lValue(&stParam);
			M_EvalNextParameter(&stParam);
			lIndex2  = M_GetSetParam_lValue(&stParam) ;
			M_EvalNextParameter(&stParam);
			lPercent = M_GetSetParam_lValue(&stParam) ;
			break ;
	default:
break;
	}
	fn_vMSLightSetRLIParameter ( hMSLight, lNbRLI, lIndex1, lIndex2, lPercent) ;
	return p_stTree;
}

AI_tdstNodeInterpret *__cdecl fn_p_stManageFrame(
    HIE_tdstSuperObject *p_SuperObjPerso, AI_tdstNodeInterpret *p_stTree)
{
    unsigned long         procId = (unsigned long)M_eProcedureIdInterpret(p_stTree-1);
    HIE_tdstSuperObject  *hNewSuperObjPerso;
    AI_tdstGetSetParam    stParam;
    AI_tdstNodeInterpret *p_stNext;
    short                 wFrame;

    fn_vGetUltraOperatorPerso(fn_ucGetProcedureUltraOperator(procId), p_SuperObjPerso, &hNewSuperObjPerso);

    switch (procId)
    {
    case 0x195:
        p_stNext = fn_p_stEvalTree(p_SuperObjPerso, p_stTree, &stParam);
        wFrame   = stParam.uParam.wValue;
        AI_M_pfn_PLA_fn_vSetCurrFrame(
            hNewSuperObjPerso, (short)((wFrame == PLA_C_wRandomFrame) ? -1 : wFrame));
        return p_stNext;

    case 0x196:
        AI_M_pfn_PLA_fn_vSetCurrFrame(hNewSuperObjPerso, PLA_C_wRandomFrame);
        return p_stTree;

    default:
        break;
    }
    AI_M_pfn_M_AIFatalError(0x27);
    return p_stTree;
}

AI_tdstNodeInterpret *__cdecl fn_p_stSPOSuperimpoed(
    HIE_tdstSuperObject *p_SuperObjPerso, AI_tdstNodeInterpret *p_stTree)
{
	long eProcedureId = M_eProcedureIdInterpret(p_stTree-1);
	AI_tdstGetSetParam stParam;

	HIE_tdstSuperObject *hNewSuperObjPerso;
	fn_vGetUltraOperatorPerso(fn_ucGetProcedureUltraOperator(eProcedureId),p_SuperObjPerso,&hNewSuperObjPerso);

	switch ( eProcedureId )
	{
	case 0x19b:
		AI_M_pfn_HIE_SO_vClearSuperimposedFlag(hNewSuperObjPerso);

		AI_M_pfn_fn_vSectInfoSetCurrentSector(
			M_GetMSHandle(hNewSuperObjPerso,SectInfo),
			AI_M_pfn_fn_h_SectInfoGetCurrentSector(M_GetMSHandle(AI_SPO_g_p_stMainActor,SectInfo))
		);
		AI_M_pfn_fn_vInsertObjectInSectorList(AI_M_pfn_fn_h_SectInfoGetCurrentSector(M_GetMSHandle(hNewSuperObjPerso,SectInfo)),hNewSuperObjPerso);
		return p_stTree;
		break;

	case 0x199:
		{
			POS_tdstCompletePosition *hPersoGlobalMatrix ;
			long lX, lY, lZ ;
			MTH_tdxReal xX, xY, xZ ;
			MTH3D_tdstVector stVertex;
			MS_tdxHandleToDynam h_Dynam = M_GetMSHandle(hNewSuperObjPerso,Dynam);
			AI_M_pfn_HIE_SO_vSetSuperimposedFlag(hNewSuperObjPerso);

			AI_M_pfn_fn_vRemoveObjectInSectorList(hNewSuperObjPerso);

			hPersoGlobalMatrix = HIE_fn_hGetSuperObjectGlobalMatrix(hNewSuperObjPerso);

			M_EvalNextParameter(&stParam);
			lX = M_GetSetParam_lValue(&stParam);
			M_EvalNextParameter(&stParam);
			lY = M_GetSetParam_lValue(&stParam);
			M_EvalNextParameter(&stParam);
			lZ = M_GetSetParam_lValue(&stParam);

			xX = MTH_M_xMul( MTH_M_xLongToReal(lX), AI_SPO_x0_001 ) ;
			xY = MTH_M_xMul( MTH_M_xLongToReal(lY), AI_SPO_x0_001 ) ;
			xZ = MTH_M_xMul( MTH_M_xLongToReal(lZ), AI_SPO_x0_001 ) ;

			AI_M_pfn_GLI_vGet3DVertexFromScreenPos(AI_SPO_pFixViewportAttr, &stVertex, xX, xY, xZ);
			POS_fn_vSetTranslationVector(hPersoGlobalMatrix,&stVertex);

			hPersoGlobalMatrix->ulType = 4;
			hPersoGlobalMatrix->stTransformMatrix.stCol_2.xZ = hPersoGlobalMatrix->stTransformMatrix.stCol_0.xX =
				MTH_M_xMul(MTH_M_xLongToReal(AI_SPO_dwScreenWidth), AI_SPO_x0_001565);

			if ( h_Dynam )
			{
				DNM_tdstDynamics *p_stDynamics = AI_M_3D_pfn_DynamGetDynamics(h_Dynam);
				AI_M_pfn_MEC_vInitTranslation( p_stDynamics, hNewSuperObjPerso, (float *)&stVertex ) ;
			}
			AI_M_pfn_HIE_vComputeNewRelativeMatrix(hNewSuperObjPerso);
		}
		return p_stTree;
		break;

	case 0x19a:
		{
			POS_tdstCompletePosition *hPersoGlobalMatrix ;
			MTH3D_tdstVector stVertex;
			MS_tdxHandleToDynam h_Dynam = M_GetMSHandle(hNewSuperObjPerso,Dynam);
			AI_M_pfn_HIE_SO_vSetSuperimposedFlag(hNewSuperObjPerso);

			AI_M_pfn_fn_vRemoveObjectInSectorList(hNewSuperObjPerso);

			hPersoGlobalMatrix = HIE_fn_hGetSuperObjectGlobalMatrix(hNewSuperObjPerso);

			M_EvalNextParameter(&stParam);
			stVertex.xX = M_GetSetParam_xValue(&stParam);
			M_EvalNextParameter(&stParam);
			stVertex.xY = M_GetSetParam_xValue(&stParam);
			M_EvalNextParameter(&stParam);
			stVertex.xZ = M_GetSetParam_xValue(&stParam);

			POS_fn_vSetTranslationVector(hPersoGlobalMatrix,&stVertex);

			if ( h_Dynam )
			{
				DNM_tdstDynamics *p_stDynamics = AI_M_3D_pfn_DynamGetDynamics(h_Dynam);
				AI_M_pfn_MEC_vInitTranslation( p_stDynamics, hNewSuperObjPerso, (float *)&stVertex ) ;
			}
			AI_M_pfn_HIE_vComputeNewRelativeMatrix(hNewSuperObjPerso);
		}
		return p_stTree ;
		break;

	case 0x19d:
		{
		  long lParam ;

		  M_EvalNextParameter(&stParam);
		  lParam = M_GetSetParam_lValue(&stParam);
		  AI_M_pfn_HIE_SO_SwitchSuperImposedTab(lParam) ;
		}
		return (p_stTree) ;
		break;

	default:
		break;
	}
	AI_M_pfn_M_AIFatalError(0x27);
	return(p_stTree);
}

/* 0x00471370 */
AI_tdstNodeInterpret *__cdecl fn_p_stSetSPOCoordinates(
    HIE_tdstSuperObject *p_SuperObjPerso, AI_tdstNodeInterpret *p_stTree)
{
    unsigned long         procId = (unsigned long)M_eProcedureIdInterpret(p_stTree-1);
    AI_tdstGetSetParam    stParam;
    AI_tdstNodeInterpret *p_stNext;
    HIE_tdstSuperObject  *hSpo;

    p_stNext = fn_p_stEvalTree(p_SuperObjPerso, p_stTree, &stParam);
    hSpo     = (HIE_tdstSuperObject *)stParam.uParam.p_stEngObjValue;
    p_stNext = fn_p_stEvalTree(p_SuperObjPerso, p_stNext, &stParam);

    if (procId == 0x19c)
    {
        if (hSpo != NULL)
            POS_fn_vSetTranslationVector(
                HIE_fn_hGetSuperObjectMatrix(hSpo),
                (MTH3D_tdstVector *)&stParam.uParam.a_xVectorValue[0]);
    }
    else
    {
        AI_M_pfn_M_AIFatalError(0x27);
    }
    return p_stNext;
}

/* 0x004713E0 */
AI_tdstNodeInterpret *__cdecl fn_p_stMiscProcOnTypeOfWP(
    HIE_tdstSuperObject *p_SuperObjPerso, AI_tdstNodeInterpret *p_stTree)
{
	AI_tdstGetSetParam stParam;
	long                       lTypeOfWP, lIndex;
	WP_tdHandleOfGraph         hGraph;
	WP_tdhWayPoint		         hWayPoint;
	WP_tdhGraphNode            hCurrentNode, hTheNode;
	long lValue;

	M_EvalNextParameter(&stParam);
	hGraph = M_GetSetParam_GraphValue(&stParam) ;

	M_EvalNextParameter( &stParam );
	hWayPoint = M_GetSetParam_WayPointValue(&stParam) ;

	lTypeOfWP = 0L;

	M_EvalNextParameter( &stParam );
	lTypeOfWP = (long)M_GetSetParam_CapsValue(&stParam);

	M_EvalNextParameter(&stParam);
	lValue = (unsigned char) M_GetSetParam_lValue(&stParam);

	lIndex = -1;
	hTheNode = (WP_tdhGraphNode)NULL;

	LST2_M_DynamicForEachElementOf(&hGraph->m_hListOfNode, hCurrentNode, lIndex)
	{
		if ( hCurrentNode->m_hWayPoint == hWayPoint )
		{
			hTheNode = hCurrentNode;
			break ;
		}
	}

	if (lValue == 2)
	{
		LST2_M_DynamicForEachElementOf(&hGraph->m_hListOfNode, hCurrentNode, lIndex)
		{
			hCurrentNode->m_lTypeOfWP &= ~lTypeOfWP;
		}
	}
	if (lValue == 3)
	{
		LST2_M_DynamicForEachElementOf(&hGraph->m_hListOfNode, hCurrentNode, lIndex)
		{
			hCurrentNode->m_lTypeOfWP = 0;
		}
	}

	if ( !hTheNode )
		return(p_stTree);

	switch (lValue)
	{
	case 0:
	case 2:
		hTheNode->m_lTypeOfWP |= lTypeOfWP;
		break;

	case 1:
	case 3:
		hTheNode->m_lTypeOfWP = lTypeOfWP;
		break;

	case 4:
		hTheNode->m_lTypeOfWP &= ~lTypeOfWP;;

	case 5:
		hTheNode->m_lTypeOfWP = 0;
		break;
	}
	return(p_stTree);
}

/* 0x00471500 */
AI_tdstNodeInterpret *__cdecl fn_p_stTractionFactorProcedure(
    HIE_tdstSuperObject *p_SuperObjPerso, AI_tdstNodeInterpret *p_stTree)
{
    unsigned long         procId = (unsigned long)M_eProcedureIdInterpret(p_stTree-1);
    HIE_tdstSuperObject  *p_stNew;
    struct tdstEngineObject_ *hEngineObj;
    AI_tdstGetSetParam    stParam;
    AI_tdstNodeInterpret *p_stNext;
    unsigned char         ucTractionFactor;

    fn_vGetUltraOperatorPerso(fn_ucGetProcedureUltraOperator(procId), p_SuperObjPerso, &p_stNew);
    hEngineObj = M_GetEngineObject(p_stNew);

    p_stNext = fn_p_stEvalTree(p_SuperObjPerso, p_stTree, &stParam);
    ucTractionFactor = stParam.uParam.ucValue;
    switch (procId)
    {
    case 0x19e:
        hEngineObj->h_StandardGame->ucTractionFactor = ucTractionFactor;
        break;
    default:
        break;
    }
    return p_stNext;
}

/* 0x00471570 */
AI_tdstNodeInterpret *__cdecl fn_p_stTurnToPosition(
    HIE_tdstSuperObject *p_SuperObjPerso, AI_tdstNodeInterpret *p_stTree)
{
    long                  eProcId = M_eProcedureIdInterpret(p_stTree-1);
    AI_tdstGetSetParam    stParam;
    MTH3D_tdstVector      stPosition, stLocalPosition, stPersoDirection, stCrossProd;
    MTH_tdxReal           xMaxAngle, xAngle;
    POS_tdstCompletePosition mGlobalToPerso;
    HIE_tdstSuperObject  *hNewSuperObjPerso;

    fn_vGetUltraOperatorPerso(fn_ucGetProcedureUltraOperator(eProcId), p_SuperObjPerso, &hNewSuperObjPerso);

    p_stTree = fn_p_stEvalTree(p_SuperObjPerso, p_stTree, &stParam);
    stPosition = *(MTH3D_tdstVector *)stParam.uParam.a_xVectorValue;

    p_stTree = fn_p_stEvalTree(p_SuperObjPerso, p_stTree, &stParam);
    xMaxAngle = M_ReturnParam_xValue(&stParam);
    xMaxAngle = MTH_M_xDegToRad(MTH_M_xAbs(xMaxAngle));

    POS_fn_vInvertMatrix(&mGlobalToPerso, HIE_fn_hGetSuperObjectGlobalMatrix(hNewSuperObjPerso));

    POS_fn_vMulMatrixVertex(&stLocalPosition, &mGlobalToPerso, &stPosition);

    if (MTH3D_M_bIsNullVector(&stLocalPosition))
        return p_stTree;
    MTH3D_M_vSetVectorElements(&stPersoDirection, MTH_C_ZERO, MTH_C_MinusONE, MTH_C_ZERO);
    MTH3D_M_vSetZofVector(&stLocalPosition, MTH_C_ZERO);

    xAngle = MTH3D_M_xDotProductVector(&stPersoDirection, &stLocalPosition);
    xAngle = MTH_M_xDiv(xAngle, MTH3D_M_xNormVector(&stLocalPosition));
    xAngle = MTH_M_xACos(xAngle);

    if (eProcId == 0x1a0)
        xMaxAngle = MTH_M_xMul(xMaxAngle, MTH_M_xMul(MTH_M_xLongToReal(AI_g_ulUsefulDeltaTime), AI_C_xMilli));

    xAngle = MTH_M_xMin(xAngle, xMaxAngle);

    MTH3D_M_vCrossProductVectorWithoutBuffer(&stCrossProd, &stPersoDirection, &stLocalPosition);
    if (MTH_M_bLess(MTH3D_M_xGetZofVector(&stCrossProd), MTH_C_ZERO))
        xAngle = MTH_M_xNeg(xAngle);

    if (!AI_M_pfn_fn_ucInternalTurn(hNewSuperObjPerso, xAngle))
        AI_M_pfn_fn_vSetProcedureActionReturn(1);

    return p_stTree;
}

/* 0x00471780 */
AI_tdstNodeInterpret *__cdecl fn_p_stFreezeAnim(
    HIE_tdstSuperObject *p_SuperObjPerso, AI_tdstNodeInterpret *p_stTree)
{
    HIE_tdstSuperObject  *hNewSuperObjPerso;
    unsigned long         procId;

    procId = (unsigned long)M_eProcedureIdInterpret(p_stTree-1);

    fn_vGetUltraOperatorPerso(fn_ucGetProcedureUltraOperator(procId), p_SuperObjPerso, &hNewSuperObjPerso);

    switch (procId)
    {

    case 0x197:
        ANI_fn_vFreeze(hNewSuperObjPerso);
        return p_stTree;

    case 0x198:
        ANI_fn_vUnFreeze(hNewSuperObjPerso);
        return p_stTree;
    default:
        break;
    }
    AI_M_pfn_M_AIFatalError(0x27);
    return p_stTree;
}

/* 0x004717F0 */
AI_tdstNodeInterpret *__cdecl fn_p_stFixePositionFade(
    HIE_tdstSuperObject *p_SuperObjPerso, AI_tdstNodeInterpret *p_stTree)
{
    AI_tdstGetSetParam        stParam;
    AI_tdstNodeInterpret     *p_stNext;
    HIE_tdstSuperObject      *hSpo;
    POS_tdstCompletePosition *p_stLocal;
    MTH3D_tdstVector          stVertex;
    long                      lX, lY, lZ;
    float                     fScale;

    p_stNext  = fn_p_stEvalTree(p_SuperObjPerso, p_stTree, &stParam);
    hSpo      = (HIE_tdstSuperObject *)stParam.uParam.p_stEngObjValue;
    AI_M_pfn_HIE_SO_vSetSuperimposedFlag(hSpo);
    AI_M_pfn_fn_vRemoveObjectInSectorList(hSpo);
    p_stLocal = (POS_tdstCompletePosition *)hSpo->hLocalMatrix;
    p_stNext  = fn_p_stEvalTree(p_SuperObjPerso, p_stNext, &stParam); lX = stParam.uParam.lValue;
    p_stNext  = fn_p_stEvalTree(p_SuperObjPerso, p_stNext, &stParam); lY = stParam.uParam.lValue;
    p_stNext  = fn_p_stEvalTree(p_SuperObjPerso, p_stNext, &stParam); lZ = stParam.uParam.lValue;
    AI_M_pfn_GLI_vGet3DVertexFromScreenPos(AI_SPO_pFixViewportAttr, &stVertex,
        (float)lX * AI_SPO_x0_01, (float)lY * AI_SPO_x0_01, (float)lZ * AI_SPO_x0_001);
    POS_fn_vSetTranslationVector(p_stLocal, &stVertex);
    p_stLocal->ulType = 4;
    fScale = (float)AI_SPO_dwScreenWidth * AI_SPO_x0_001565;
    p_stLocal->stTransformMatrix.stCol_0.xX = fScale;
    p_stLocal->stTransformMatrix.stCol_2.xZ = fScale;
    return p_stNext;
}

int fni_CompareIntegerLess(const void *_pv_Elem1, const void *_pv_Elem2)
{
    long *p_lElement1 = (long *)_pv_Elem1;
    long *p_lElement2 = (long *)_pv_Elem2;

    return (*p_lElement2 < *p_lElement1) ? -1 : (*p_lElement2 == *p_lElement1) ? 0 : 1;
}

int fni_CompareIntegerGreater(const void *_pv_Elem1, const void *_pv_Elem2)
{
    long *p_lElement1 = (long *)_pv_Elem1;
    long *p_lElement2 = (long *)_pv_Elem2;

    return (*p_lElement2 > *p_lElement1) ? -1 : (*p_lElement2 == *p_lElement1) ? 0 : 1;
}

int fni_CompareFloatGreater(const void *_pv_Elem1, const void *_pv_Elem2)
{
    MTH_tdxReal *p_xElement1 = (MTH_tdxReal *)_pv_Elem1;
    MTH_tdxReal *p_xElement2 = (MTH_tdxReal *)_pv_Elem2;

    return (MTH_M_bGreater(*p_xElement2, *p_xElement1)) ? -1 : (MTH_M_bEqual(*p_xElement2, *p_xElement1)) ? 0 : 1;
}

int fni_CompareFloatLess(const void *_pv_Elem1, const void *_pv_Elem2)
{
    MTH_tdxReal *p_xElement1 = (MTH_tdxReal *)_pv_Elem1;
    MTH_tdxReal *p_xElement2 = (MTH_tdxReal *)_pv_Elem2;

    return (MTH_M_bLess(*p_xElement2, *p_xElement1)) ? -1 : (MTH_M_bEqual(*p_xElement2, *p_xElement1)) ? 0 : 1;
}

AI_tdstNodeInterpret *__cdecl fn_p_stSortArray(
    HIE_tdstSuperObject *p_SuperObjPerso, AI_tdstNodeInterpret *p_stTree)
{
  AI_tdstGetSetParam			stParam;
  tdstArray					*p_stArray;
  unsigned char				ucVarId;
  long						lNbElements;
  long						lType;
  int (*p_fnCompare)(const void *, const void *);

  ucVarId = (unsigned char) AI_M_uVarIdInterpret(p_stTree);
  p_stTree ++;
  p_stArray = (tdstArray*) M_GetDsgVarAddr(AI_M_stGetMindOfSuperObj(p_SuperObjPerso),ucVarId);

  M_EvalNextParameter(&stParam);
  lNbElements = M_GetSetParam_lValue(&stParam);

  M_EvalNextParameter(&stParam);
  lType = M_GetSetParam_lValue(&stParam);

  if (lNbElements > (long)p_stArray->ucMaxSize)
	lNbElements = (long)p_stArray->ucMaxSize;

  switch(p_stArray->eType)
  {
  case 5:
	  {

		  if (lType)
			p_fnCompare = fni_CompareIntegerGreater;
		  else
			p_fnCompare = fni_CompareIntegerLess;
		  break;
	  }
  case 7:
	  {

		  if (lType)
			p_fnCompare = fni_CompareFloatGreater;
		  else
			p_fnCompare = fni_CompareFloatLess;
		  break;
	  }
  default :
	  {
		  return p_stTree;
		  break;
	  }
  }

  qsort((void *)p_stArray->d_ArrayElement, (int)lNbElements, C_SizeOfArrayElement, p_fnCompare);
  return p_stTree;
}

AI_tdstNodeInterpret *__cdecl fn_p_stActiveJoystickAnal(
    HIE_tdstSuperObject *p_SuperObjPerso, AI_tdstNodeInterpret *p_stTree)
{
    unsigned long procId = (unsigned long)M_eProcedureIdInterpret(p_stTree-1);
    (void)p_SuperObjPerso;

    switch (procId)
    {

    case 0x1a2:
        AI_M_pfn_IPT_fn_vDesactiveJoystickAndPadControl();
        AI_M_pfn_IPT_fn_vActiveJoystickControl();
        return p_stTree;

    case 0x1a3:
        AI_M_pfn_IPT_fn_vDesactiveJoystickAndPadControl();
        return p_stTree;
    default:
        break;
    }

    AI_M_pfn_M_AIFatalError(0x27);
    return p_stTree;
}

AI_tdstNodeInterpret *__cdecl fn_p_stSetZDxSize(
    HIE_tdstSuperObject *p_SuperObjPerso, AI_tdstNodeInterpret *p_stTree)
{
	AI_tdstGetSetParam			stParam;
	long						lZoneNumber;
	ACP_tdxHandleOfObject		hGeoObj;
	long						eProcedureId=M_eProcedureIdInterpret(p_stTree-1);

	HIE_tdstSuperObject *hNewSuperObjPerso;
	fn_vGetUltraOperatorPerso(fn_ucGetProcedureUltraOperator(eProcedureId),p_SuperObjPerso,&hNewSuperObjPerso);

	M_EvalNextParameter( &stParam );
	lZoneNumber = (long) M_GetSetParam_lZDMIdValue(&stParam);

    switch (eProcedureId)
    {

    case 0x1a4:
        hGeoObj = AI_M_pfn_fn_hGetGeometricZdxOfTypeAtIndex(1, (unsigned short)(lZoneNumber - 1), hNewSuperObjPerso);
        break;

    case 0x1a5:
        hGeoObj = AI_M_pfn_fn_hGetGeometricZdxOfTypeAtIndex(0, (unsigned short)(lZoneNumber - 1), hNewSuperObjPerso);
        break;

    default:

        AI_M_pfn_M_AIFatalError(0x28);
        return p_stTree;
    }

    if (hGeoObj)
    {
        switch (eProcedureId)
        {

        case 0x1a4:
            if (*hGeoObj->d_xListOfElementsTypes == GEO_C_xElementSpheres)
            {
                MTH_tdxReal xRadius;

                M_EvalNextParameter( &stParam );
                xRadius=M_GetSetParam_xValue(&stParam);

                AI_M_pfn_GEO_vSetRadiusOfIndexedSphere(hGeoObj, 0, 0, xRadius);
            }
            return p_stTree;

        case 0x1a5:
            if (*hGeoObj->d_xListOfElementsTypes == GEO_C_xElementCones)
            {
                MTH_tdxReal xAngle,xHeight,xRadius;
                MTH3D_tdstVector stBasePoint,stTopPoint,stVector;
                ACP_tdxIndex xBasePoint,xTopPoint;

                M_EvalNextParameter( &stParam );
                xAngle=M_GetSetParam_xValue(&stParam);

                M_EvalNextParameter( &stParam );
                xHeight=M_GetSetParam_xValue(&stParam);

                AI_M_pfn_GEO_vGetPointsOfIndexedCone(hGeoObj, 0, 0, &xBasePoint, &xTopPoint);
                AI_M_pfn_GEO_vGetPointOfObject(hGeoObj, &stBasePoint, xBasePoint);
                AI_M_pfn_GEO_vGetPointOfObject(hGeoObj, &stTopPoint, xTopPoint);

                MTH3D_M_vSubVector(&stVector, &stBasePoint, &stTopPoint);
                MTH3D_M_vNormalizeVector(&stVector, &stVector);
                MTH3D_M_vMulScalarVector(&stVector, xHeight, &stVector);

                if (MTH_M_bGreater(xHeight, MTH_C_ZERO))
                {
                    MTH3D_M_vAddVector(&stBasePoint, &stTopPoint, &stVector);
                    AI_M_pfn_GEO_vSetPointOfObject(hGeoObj, &stBasePoint, xBasePoint);
                }

                else
                {
                    MTH3D_M_vAddVector(&stTopPoint, &stBasePoint, &stVector);
                    AI_M_pfn_GEO_vSetPointOfObject(hGeoObj, &stTopPoint, xTopPoint);
                }

                xRadius = MTH_M_xMul(MTH_M_xAbs(xHeight), MTH_M_xTan(MTH_M_xDegToRad(xAngle)));
                AI_M_pfn_GEO_vSetRadiusOfIndexedCone(hGeoObj, 0, 0, xRadius);
            }
            return p_stTree;

        default:
            break;
        }
    }

    return p_stTree;
}

AI_tdstNodeInterpret *__cdecl fn_p_stSetSoundVolume(
    HIE_tdstSuperObject *p_SuperObjPerso, AI_tdstNodeInterpret *p_stTree)
{
    unsigned long         procId = (unsigned long)M_eProcedureIdInterpret(p_stTree-1);
    AI_tdstGetSetParam    stParam;
    AI_tdstNodeInterpret *p_stNext;
    long                  lVolume;

    switch (procId)
    {
    case 0x1a6:
    {
        HIE_tdstSuperObject *hNewSuperObjPerso;

        fn_vGetUltraOperatorPerso(fn_ucGetProcedureUltraOperator(procId), p_SuperObjPerso, &hNewSuperObjPerso);
        p_stNext = fn_p_stEvalTree(p_SuperObjPerso, p_stTree, &stParam);
        lVolume  = stParam.uParam.lValue;
        AI_M_pfn_SND_fn_vChangeVolumeSound((long)hNewSuperObjPerso, GAM_g_lSoundObjectTypeAnim, lVolume);
        return p_stNext;
    }
    case 0x1a7:
        p_stNext = fn_p_stEvalTree(p_SuperObjPerso, p_stTree, &stParam);
        lVolume  = stParam.uParam.lValue;
        AI_M_pfn_SND_fn_vChangeVolumeSound(AI_SND_C_OBJET_FANTOME, GAM_g_lSoundObjectTypeMusic, lVolume);
        return p_stNext;
    case 0x1a8:
        p_stNext = fn_p_stEvalTree(p_SuperObjPerso, p_stTree, &stParam);
        lVolume  = stParam.uParam.lValue;
        AI_M_pfn_SND_fn_vChangeVolumeSound(AI_SND_C_OBJET_FANTOME, GAM_g_lSoundObjectTypeAnim, lVolume);
        return p_stNext;
    default:
        break;
    }
    AI_M_pfn_M_AIFatalError(0x27);
    return p_stTree;
}

AI_tdstNodeInterpret *__cdecl fn_p_stSetDopplerEffect(
    HIE_tdstSuperObject *p_SuperObjPerso, AI_tdstNodeInterpret *p_stTree)
{
    AI_tdstGetSetParam    stParam;
    AI_tdstNodeInterpret *p_stNext;

    p_stNext = fn_p_stEvalTree(p_SuperObjPerso, p_stTree, &stParam);
    SND_fn_vSetDopplerFactorSound((long)(stParam.uParam.xValue * 0x10000));
    return p_stNext;
}

AI_tdstNodeInterpret *__cdecl fn_p_stPauseSound(
    HIE_tdstSuperObject *p_SuperObjPerso, AI_tdstNodeInterpret *p_stTree)
{
    unsigned long procId = (unsigned long)M_eProcedureIdInterpret(p_stTree-1);
    (void)p_SuperObjPerso;

    switch (procId)
    {
    case 0x1aa:
        SND_fn_vPauseSound();
        break;
    case 0x1ab:
        SND_fn_vResumeSound();
        break;
    case 0x1ac:
        AI_M_pfn_SND_fn_vKillAllObjectTypeSound(AI_g_lSoundObjectTypeMenu);
        break;
    default:
        break;
    }
    return p_stTree;
}
AI_tdstNodeInterpret *__cdecl fn_p_stPlayDemo(
    HIE_tdstSuperObject *p_SuperObjPerso, AI_tdstNodeInterpret *p_stTree)
{
    AI_tdstGetSetParam stParam;
    char               szMapName[40];
    char              *p_cMapName;
    char              *p_cDollar;
    char               cNext;

    p_stTree = fn_p_stEvalTree(p_SuperObjPerso, p_stTree, &stParam);
    p_cMapName = fn_szGetStringFromTextOrStringParam(&stParam);
    fn_lGetSubMapNumber();
    if (*p_cMapName == '*')
    {
        strcpy(szMapName, fn_p_szGetLevelName());
        p_cDollar = strchr(p_cMapName, '$');
        if (p_cDollar == NULL)
            goto l_NextParams;
        cNext = p_cDollar[1];
    }
    else
    {
        strcpy(szMapName, p_cMapName);
        p_cDollar = strchr(szMapName, '$');
        if (p_cDollar == NULL)
            goto l_NextParams;
        cNext = p_cDollar[1];
        *p_cDollar = '\0';
    }
    if (cNext != '*')
        atol(p_cDollar + 1);

l_NextParams:

    p_stTree = fn_p_stEvalTree(p_SuperObjPerso, p_stTree, &stParam);
    fn_szGetStringFromTextOrStringParam(&stParam);

    p_stTree = fn_p_stEvalTree(p_SuperObjPerso, p_stTree, &stParam);

    p_stTree = fn_p_stEvalTree(p_SuperObjPerso, p_stTree, &stParam);
    fn_szGetStringFromTextOrStringParam(&stParam);
    return p_stTree;
}

/* 0x00471FB0 */
AI_tdstNodeInterpret *__cdecl fn_p_stSetTooFarLimit(
    HIE_tdstSuperObject *p_SuperObjPerso, AI_tdstNodeInterpret *p_stTree)
{
    AI_tdstGetSetParam    stParam;
    HIE_tdstSuperObject  *hNewSuperObjPerso;
    struct tdstEngineObject_ *hEngineObj;
    unsigned long         procId;

    procId = (unsigned long)M_eProcedureIdInterpret(p_stTree-1);

    fn_vGetUltraOperatorPerso(fn_ucGetProcedureUltraOperator(procId), p_SuperObjPerso, &hNewSuperObjPerso);

    hEngineObj = M_GetEngineObject(hNewSuperObjPerso);

    p_stTree = fn_p_stEvalTree(p_SuperObjPerso, p_stTree, &stParam);
    if (hEngineObj != NULL)
    {
        MS_tdxHandleToStandardGame hStdGame = hEngineObj->h_StandardGame;
        if (hStdGame != NULL)
            hStdGame->ucTooFarLimit =
                stParam.uParam.ucValue;
    }
    return p_stTree;
}

/* 0x00472020 */
AI_tdstNodeInterpret *__cdecl fn_p_stSetTransparencyZone(
    HIE_tdstSuperObject *p_SuperObjPerso, AI_tdstNodeInterpret *p_stTree)
{
    AI_tdstGetSetParam        stParam;
    HIE_tdstSuperObject      *hNewSuperObjPerso;
    struct tdstEngineObject_ *hEngineObj;
    long                      eProcedureId = M_eProcedureIdInterpret(p_stTree-1);
    long                      lValueMin, lValueMax;

    fn_vGetUltraOperatorPerso(fn_ucGetProcedureUltraOperator(eProcedureId), p_SuperObjPerso, &hNewSuperObjPerso);
    hEngineObj = M_GetEngineObject(hNewSuperObjPerso);

    M_EvalNextParameter(&stParam);
    lValueMin = M_GetSetParam_lValue(&stParam);
    M_EvalNextParameter(&stParam);
    lValueMax = M_GetSetParam_lValue(&stParam);

    if (hEngineObj && hEngineObj->h_StandardGame)
    {
        hEngineObj->h_StandardGame->ucTransparencyZoneMin = (unsigned char)lValueMin;
        hEngineObj->h_StandardGame->ucTransparencyZoneMax = (unsigned char)lValueMax;
    }
    return p_stTree;
}

AI_tdstNodeInterpret *__cdecl fn_p_stBaseFrequenceForMenu(
    HIE_tdstSuperObject *p_SuperObjPerso, AI_tdstNodeInterpret *p_stTree)
{
    AI_tdstGetSetParam    stParam;
    AI_tdstNodeInterpret *p_stNext;

    p_stNext = fn_p_stEvalTree(p_SuperObjPerso, p_stTree, &stParam);
    AI_M_pfn_GLI_vSetSinEffetBase(stParam.uParam.xValue);
    return p_stNext;
}

AI_tdstNodeInterpret *__cdecl fn_p_stSaveSinusEffectContext(
    HIE_tdstSuperObject *p_SuperObjPerso, AI_tdstNodeInterpret *p_stTree)
{
    unsigned long procId = (unsigned long)M_eProcedureIdInterpret(p_stTree-1);
    (void)p_SuperObjPerso;

    if (procId == 0x1b9)
        AI_M_pfn_GLI_vSaveSinusContext();
    else
        AI_M_pfn_GLI_vRestoreSinusContext();
    return p_stTree;
}

/* 0x00472100 */
AI_tdstNodeInterpret *__cdecl fn_p_stTurnPersoProcedure(
    HIE_tdstSuperObject *p_SuperObjPerso, AI_tdstNodeInterpret *p_stTree)
{
    AI_tdstGetSetParam    stParam;
    void                 *h_Dynam;
    void                 *p_stDynamic;
    HIE_tdstSuperObject  *p_stAimedObject;
    MTH3D_tdstVector      stVertexDirection, stVertex1;
    long                  eProcedureId = M_eProcedureIdInterpret(p_stTree-1);
    HIE_tdstSuperObject  *hNewSuperObjPerso;

    fn_vGetUltraOperatorPerso(fn_ucGetProcedureUltraOperator(eProcedureId), p_SuperObjPerso, &hNewSuperObjPerso);

    p_stTree = fn_p_stEvalTree(p_SuperObjPerso, p_stTree, &stParam);

    p_stAimedObject = AI_M_pfn_fn_h_StandardGameGetSuperObject(
                          stParam.uParam.p_stEngObjValue->h_StandardGame);

    h_Dynam = M_GetMSHandle(hNewSuperObjPerso,Dynam);
    if (h_Dynam)
    {
        p_stDynamic = AI_M_pfn_fn_p_stDynamGetDynamics(h_Dynam);
        if (p_stDynamic)
        {

            POS_fn_vGetTranslationVector(HIE_fn_hGetSuperObjectGlobalMatrix(hNewSuperObjPerso), &stVertex1);

            switch (eProcedureId)
            {

            case 0x89:
            {
                MTH_tdxReal xDX, xDY, xDZ;

                p_stTree = fn_p_stEvalTree(p_SuperObjPerso, p_stTree, &stParam);
                xDX = M_ReturnParam_xValue(&stParam);

                p_stTree = fn_p_stEvalTree(p_SuperObjPerso, p_stTree, &stParam);
                xDY = M_ReturnParam_xValue(&stParam);

                p_stTree = fn_p_stEvalTree(p_SuperObjPerso, p_stTree, &stParam);
                xDZ = M_ReturnParam_xValue(&stParam);

                MTH3D_M_vSetVectorElements(&stVertexDirection, xDX, xDY, xDZ);
                POS_fn_vMulMatrixVertex(&stVertexDirection,
                    HIE_fn_hGetSuperObjectGlobalMatrix(p_stAimedObject), &stVertexDirection);
            }
            break;

            case 0x8a:
                POS_fn_vGetTranslationVector(HIE_fn_hGetSuperObjectGlobalMatrix(p_stAimedObject), &stVertexDirection);
                break;

            default:
                return p_stTree;
            }

            MTH3D_M_vSubVector(&stVertexDirection, &stVertexDirection, &stVertex1);
            MEC_vTurnAbsolute(p_stDynamic, &stVertexDirection);
        }
    }

    return p_stTree;
}

/* 0x00472290 */
AI_tdstNodeInterpret *__cdecl fn_p_stChangeActionProcedure(
    HIE_tdstSuperObject *p_SuperObjPerso, AI_tdstNodeInterpret *p_stTree)
{
    unsigned long         procId = (unsigned long)M_eProcedureIdInterpret(p_stTree-1);
    HIE_tdstSuperObject  *hNewSuperObjPerso;
    AI_tdstGetSetParam    stParam;
    void                 *h_WantedState;
    ACP_tdxBool           bHandleSkippedEventsIfRelevant;

    fn_vGetUltraOperatorPerso(fn_ucGetProcedureUltraOperator(procId), p_SuperObjPerso, &hNewSuperObjPerso);

    p_stTree = fn_p_stEvalTree(p_SuperObjPerso, p_stTree, &stParam);
    h_WantedState = stParam.uParam.p_stEngObjValue;
    if (procId == 0x4b)
    {
        p_stTree = fn_p_stEvalTree(p_SuperObjPerso, p_stTree, &stParam);
        bHandleSkippedEventsIfRelevant = (ACP_tdxBool)stParam.uParam.lValue;
    }
    else
        bHandleSkippedEventsIfRelevant = FALSE;

    if (PLA_fn_bSetNewState(hNewSuperObjPerso, h_WantedState, (ACP_tdxBool)(procId == 0x49),
                            bHandleSkippedEventsIfRelevant, FALSE))
    {

        void *h_Dynam = M_GetMSHandle(hNewSuperObjPerso,Dynam);
        if (h_Dynam != NULL)
        {
            DNM_tdstDynamics *p_stDynamics = AI_M_pfn_fn_p_stDynamGetDynamics(h_Dynam);
            if (p_stDynamics != NULL)
            {
                void *hIdCard = AI_M_pfn_fn_hGetStateMechanicsIdCard(h_WantedState);
                if (DNM_M_pDynamicsGetCurrentIdCard(p_stDynamics) != hIdCard)
                    DNM_fn_vChangeIdCard(p_stDynamics, hIdCard);
            }
        }
    }
    else
        AI_M_pfn_fn_vSetProcedureActionReturn(AI_C_ACTION_ENGINE_STOP_RULE);
    return p_stTree;
}

/* 0x00472370 */
AI_tdstNodeInterpret *__cdecl fn_p_stChangeActionRandomProcedure(
    HIE_tdstSuperObject *p_SuperObjPerso, AI_tdstNodeInterpret *p_stTree)
{
    AI_tdstGetSetParam    stParam;
    void                 *h_WantedState;
    void                 *h_Dynam;
    DNM_tdstDynamics     *p_stDynamics;
    void                 *hIdCard;
    AI_tdstNodeInterpret *p_stNext = fn_p_stEvalTree(p_SuperObjPerso, p_stTree, &stParam);

    h_WantedState = stParam.uParam.p_stEngObjValue;
    if (AI_M_pfn_PLA_bSetNewStateWithRandom(p_SuperObjPerso, h_WantedState))
    {
        h_Dynam = M_GetMSHandle(p_SuperObjPerso,Dynam);
        if (h_Dynam != NULL)
        {
            p_stDynamics = AI_M_pfn_fn_p_stDynamGetDynamics(h_Dynam);
            if (p_stDynamics != NULL)
            {
                hIdCard = AI_M_pfn_fn_hGetStateMechanicsIdCard(h_WantedState);
                if (DNM_M_pDynamicsGetCurrentIdCard(p_stDynamics) != hIdCard)
                    DNM_fn_vChangeIdCard(p_stDynamics, hIdCard);
            }
        }
    }
    else
        AI_M_pfn_fn_vSetProcedureActionReturn(AI_C_ACTION_ENGINE_STOP_RULE);
    return p_stNext;
}

/* 0x00472400 */
AI_tdstNodeInterpret *__cdecl fn_p_stDisplayProcedure(
    HIE_tdstSuperObject *p_SuperObjPerso, AI_tdstNodeInterpret *p_stTree)
{
    long eProcedureId = M_eProcedureIdInterpret(p_stTree-1);
    long lValue;

    HIE_tdstSuperObject *hNewSuperObjPerso;
    fn_vGetUltraOperatorPerso(fn_ucGetProcedureUltraOperator(eProcedureId),p_SuperObjPerso,&hNewSuperObjPerso);

    if (eProcedureId == 0x78)
    {
        lValue = HIE_fn_lGetSuperObjectDrawMask(hNewSuperObjPerso);
        lValue = (lValue | GLI_C_lIsNotGrided);
        fn_v3dDataSetDrawMask(M_GetMSHandle(hNewSuperObjPerso,3dData), lValue);
        HIE_fn_vSetSuperObjectDrawMask(hNewSuperObjPerso, lValue);
    }
    return(p_stTree);
}

AI_tdstNodeInterpret *__cdecl fn_p_stTransparenceProcedure(
    HIE_tdstSuperObject *p_SuperObjPerso, AI_tdstNodeInterpret *p_stTree)
{

    long                  eProcedureId=M_eProcedureIdInterpret(p_stTree-1);
    AI_tdstGetSetParam    stParam;
    MTH_tdxReal           xValue;
    long                  lValue;
    long                  lMode;
    ACP_tdxBool           bModule = FALSE;

    HIE_tdstSuperObject  *hNewSuperObjPerso;
    HIE_tdstSuperObject  *hModule = NULL;
    fn_vGetUltraOperatorPerso(fn_ucGetProcedureUltraOperator(eProcedureId),p_SuperObjPerso,&hNewSuperObjPerso);

    M_EvalNextParameter(&stParam);

    switch(eProcedureId)
    {
    case 0x10 :
    case 0x11 :
        {
            CHN_tdxHandleToChannelArray hChannelArray;
            unsigned short              uwChannelNumber;

            if ( eProcedureId == 0x10 )
            {
                uwChannelNumber = (unsigned short) M_GetSetParam_lModuleValue(&stParam);
            }
            else
            {
                uwChannelNumber = (unsigned short) M_GetSetParam_lValue(&stParam);
            }
            M_EvalNextParameter(&stParam);

            hChannelArray = fn_h3dDataGetChannelSOList(M_GetMSHandle(hNewSuperObjPerso, 3dData));
            if (hChannelArray == NULL)
                break;
            else
            {
                hNewSuperObjPerso = fn_hGetSuperObjectInChannel(hChannelArray,uwChannelNumber);
                if (hNewSuperObjPerso==NULL)
                    break;
            }
            HIE_fn_SO_vSetFlags(hNewSuperObjPerso, HIE_fn_SO_ulGetFlags(hNewSuperObjPerso) | HIE_C_Flag_ModuleTransparency);
        }
    case 0x0c :
        {
            xValue = M_ReturnParam_xValue(&stParam);
            if (MTH_M_bLessZero(xValue))
                xValue = MTH_C_ZERO;
            else
                if (MTH_M_bGreater(xValue, MTH_M_xLongToReal(255)))
                    xValue = MTH_M_xLongToReal(255);
            HIE_fn_vSetSuperObjectTransparenceLevel(hNewSuperObjPerso, xValue);

            if (eProcedureId== 0x0c)
            {

                PLA_fn_vUpdateTransparencyForModules(hNewSuperObjPerso);
            }
        }
        break;

    case 0x0e :
    case 0x0f :
        {
            CHN_tdxHandleToChannelArray hChannelArray;
            unsigned short              uwChannelNumber;

            if ( eProcedureId == 0x0e )
            {
                uwChannelNumber = (unsigned short) M_GetSetParam_lModuleValue(&stParam);
            }
            else
            {
                uwChannelNumber = (unsigned short) M_GetSetParam_lValue(&stParam);
            }
            M_EvalNextParameter(&stParam);

            hChannelArray = fn_h3dDataGetChannelSOList(M_GetMSHandle(hNewSuperObjPerso, 3dData));
            if (hChannelArray == NULL)
                break;
            else
            {
                hModule = fn_hGetSuperObjectInChannel(hChannelArray,uwChannelNumber);
                if (hModule==NULL)
                    break;
            }
            bModule = TRUE;
        }
    case 0x0b :
        {
            lValue = HIE_fn_lGetSuperObjectDrawMask(hNewSuperObjPerso);
            lMode = M_GetSetParam_lValue(&stParam);
            if (lMode)
            {
                lValue = (lValue & ~GLI_C_lIsNotGrided);
            }
            else
            {
                lValue = (lValue | GLI_C_lIsNotGrided);
                if (bModule)
                    HIE_fn_SO_vSetFlags(hModule, HIE_fn_SO_ulGetFlags(hModule) & ~HIE_C_Flag_ModuleTransparency);
            }
            if (!bModule)
            {
                fn_v3dDataSetDrawMask(M_GetMSHandle(hNewSuperObjPerso,3dData), lValue);
                HIE_fn_vSetSuperObjectDrawMask(hNewSuperObjPerso, lValue);

            }
            else
            {
                HIE_fn_vSetSuperObjectDrawMask(hModule, lValue);
            }
        }
        break;

    default:
        break;
    }
    return(p_stTree);
}

/* 0x00472600 */
AI_tdstNodeInterpret *__cdecl fn_p_stVisualGameMaterialProcedure(
    HIE_tdstSuperObject *p_SuperObjPerso, AI_tdstNodeInterpret *p_stTree)
{
    long                  eProcedureId = M_eProcedureIdInterpret(p_stTree-1);
    AI_tdstGetSetParam    stParam;
    AI_tdstNodeInterpret *p_stTreeOrg = p_stTree;
    void                 *hGameMaterial;
    void                 *hVisualMaterial = NULL;
    GEO_tdstColor         stColor;
    long                  lValue = 0;

    M_EvalNextParameter(&stParam);
    hGameMaterial = stParam.uParam.p_stEngObjValue;
    if (hGameMaterial != NULL)
        hVisualMaterial = AI_M_pfn_GMT_fn_hGetVisualMaterial(hGameMaterial);

    if (hVisualMaterial == NULL)
    {
        return AI_M_pfn_fn_p_stSwapNextParameter2(p_stTreeOrg);
    }

    switch(eProcedureId)
    {
    case 0x8d:
        M_EvalNextParameter(&stParam);
        fn_vConvertParamToColor(&stParam,&stColor);
        GLI_xSetMaterialColor(hVisualMaterial,&stColor);
        return p_stTree;

    case 0x8e:
        M_EvalNextParameter(&stParam);
        GLI_xGetMaterialSpecularCoef(hVisualMaterial,&lValue,&stColor);
        lValue = M_ReturnParam_lValue(&stParam);
        GLI_xSetMaterialSpecularCoef(hVisualMaterial,lValue,&stColor);
        return p_stTree;

    case 0x8f:
        M_EvalNextParameter(&stParam);
        GLI_xGetMaterialSpecularCoef(hVisualMaterial,&lValue,&stColor);
        fn_vConvertParamToColor(&stParam,&stColor);
        GLI_xSetMaterialSpecularCoef(hVisualMaterial,lValue,&stColor);
        return p_stTree;

    case 0x90:
        M_EvalNextParameter(&stParam);
        fn_vConvertParamToColor(&stParam,&stColor);
        GLI_xSetMaterialDiffuseCoef(hVisualMaterial,&stColor);
        return p_stTree;

    case 0x91:
        M_EvalNextParameter(&stParam);
        fn_vConvertParamToColor(&stParam,&stColor);
        GLI_xSetMaterialAmbientCoef(hVisualMaterial,&stColor);
        return p_stTree;

    case 0x92:
        M_EvalNextParameter(&stParam);
        GLI_vSetMaterialAsChromed(hVisualMaterial, M_GetSetParam_lValue(&stParam));
        return p_stTree;

    case 0x93:
        {
            float fUSpeed, fVSpeed;
            M_EvalNextParameter(&stParam);
            fUSpeed = M_ReturnParam_xValue(&stParam);
            M_EvalNextParameter(&stParam);
            fVSpeed = M_ReturnParam_xValue(&stParam);
            AI_M_pfn_GLI_xSetMaterialTextureScrollingCoef(hVisualMaterial, 0  , 1, fUSpeed, fVSpeed);
        }
        return p_stTree;

    case 0x94:
        M_EvalNextParameter(&stParam);
        GLI_vLockAnimatedTexture(hVisualMaterial);
        return p_stTree;

    case 0x95:
        GLI_vUnLockAnimatedTexture(hVisualMaterial);
        return p_stTree;

    case 0x96:
        M_EvalNextParameter(&stParam);
        GLI_vSetCurrentAnimatedTextureNumber(hVisualMaterial, M_ReturnParam_lValue(&stParam));
        return p_stTree;

    default:
        break;
    }
    return p_stTree;
}

/* 0x00472930 */
AI_tdstNodeInterpret *__cdecl fn_p_stMenuAndVignetteProcedure(
    HIE_tdstSuperObject *p_SuperObjPerso, AI_tdstNodeInterpret *p_stTree)
{
    long               eProcedureId = M_eProcedureIdInterpret(p_stTree-1);
    AI_tdstGetSetParam stParam;

    char *p_szString;

    switch (eProcedureId)
    {
    case 0x79:
        p_stTree = fn_p_stEvalTree(p_SuperObjPerso, p_stTree, &stParam);
        p_szString = AI_M_pfn_fn_szGetStringFromParam(&stParam);
        fn_vAskToDisplayVignette(p_szString);
        return p_stTree;

    case 0x7a:
        fn_vAskToDisplayVignette(NULL);
        return p_stTree;

    default:
        break;
    }
    return p_stTree;
}

/* 0x00472990 */
AI_tdstNodeInterpret *__cdecl fn_p_st_FootPath_AddFootPrint(
    HIE_tdstSuperObject *p_SuperObjPerso, AI_tdstNodeInterpret *p_stTree)
{
    AI_tdstGetSetParam    stParam;
    AI_tdstNodeInterpret *p_stNext = fn_p_stEvalTree(p_SuperObjPerso, p_stTree, &stParam);

    fn_v_FootPath_Add(p_SuperObjPerso, stParam.uParam.wValue);   /* 0x40c4f0 */
    return p_stNext;
}

/* 0x004729D0 */
AI_tdstNodeInterpret *__cdecl fn_p_st_FootPath_Clear(
    HIE_tdstSuperObject *p_SuperObjPerso, AI_tdstNodeInterpret *p_stTree)
{
    void *hMSPrtSrc = AI_PRT_hMSPrtSrc(p_SuperObjPerso);

    if (hMSPrtSrc != NULL)
    {
        void *hFootPath = AI_M_pfn_MSPrtSrcGetFootPath(hMSPrtSrc);
        if (hFootPath != NULL)
            fn_v_FootPath_ReInit((tdstFootPath *)hFootPath);   /* 0x40c4b0 */
    }
    return p_stTree;
}

/* 0x00472A00 */
AI_tdstNodeInterpret *__cdecl fn_p_st_SinusEffect_SetFreq(
    HIE_tdstSuperObject *p_SuperObjPerso, AI_tdstNodeInterpret *p_stTree)
{
    unsigned long         procId = (unsigned long)M_eProcedureIdInterpret(p_stTree-1);
    AI_tdstGetSetParam    stParam;
    long                  lX, lY, lZ;
    AI_tdstNodeInterpret *p_stNext;

    p_stNext = fn_p_stEvalTree(p_SuperObjPerso, p_stTree, &stParam);
    lX = (long)stParam.uParam.wValue;
    p_stNext = fn_p_stEvalTree(p_SuperObjPerso, p_stNext, &stParam);
    lY = (long)stParam.uParam.wValue;
    lZ = 0;
    if (procId == 0x3b)
    {
        p_stNext = fn_p_stEvalTree(p_SuperObjPerso, p_stNext, &stParam);
        lZ = (long)stParam.uParam.wValue;
    }
    AI_M_pfn_SinusEffect_SetFreq(lX, lY, lZ);
    return p_stNext;
}

/* 0x00472A80 */
AI_tdstNodeInterpret *__cdecl fn_p_st_SinusEffect_SetAmplitude(
    HIE_tdstSuperObject *p_SuperObjPerso, AI_tdstNodeInterpret *p_stTree)
{
    AI_tdstGetSetParam    stParam;
    MTH3D_tdstVector      stAmp;
    AI_tdstNodeInterpret *p_stNext = fn_p_stEvalTree(p_SuperObjPerso, p_stTree, &stParam);

    stAmp = *(MTH3D_tdstVector *)stParam.uParam.a_xVectorValue;
    stAmp.xX = MTH_M_xMul(stAmp.xX, AI_GLI_x0_1);
    stAmp.xY = MTH_M_xMul(stAmp.xY, AI_GLI_x0_1);
    stAmp.xZ = MTH_M_xMul(stAmp.xZ, AI_GLI_x0_1);
    AI_M_pfn_SinusEffect_SetAmplitude((float *)&stAmp);
    return p_stNext;
}

/* 0x00472B00 */
AI_tdstNodeInterpret *__cdecl fn_p_st_SinusEffect_SetRLIParams(
    HIE_tdstSuperObject *p_SuperObjPerso, AI_tdstNodeInterpret *p_stTree)
{
    unsigned long         procId = (unsigned long)M_eProcedureIdInterpret(p_stTree-1);
    AI_tdstGetSetParam    stParam;
    float                 xRed, xGreen, xBlue;
    AI_tdstNodeInterpret *p_stNext;

    p_stNext = fn_p_stEvalTree(p_SuperObjPerso, p_stTree, &stParam);
    xRed   = stParam.uParam.xValue;
    p_stNext = fn_p_stEvalTree(p_SuperObjPerso, p_stNext, &stParam);
    xGreen = stParam.uParam.xValue;
    p_stNext = fn_p_stEvalTree(p_SuperObjPerso, p_stNext, &stParam);
    xBlue  = stParam.uParam.xValue;

    if (procId == 0x3d)
        AI_M_pfn_SinusEffect_SetRLIBase(xRed, xGreen, xBlue);
    else if (procId == 0x3c)
        AI_M_pfn_SinusEffect_SetRLIAmp(xRed, xGreen, xBlue);
    return p_stNext;
}

/* 0x00472BA0 */
AI_tdstNodeInterpret *__cdecl fn_p_st_SinusEffect_SetState(
    HIE_tdstSuperObject *p_SuperObjPerso, AI_tdstNodeInterpret *p_stTree)
{
    AI_tdstGetSetParam    stParam;
    AI_tdstNodeInterpret *p_stNext = fn_p_stEvalTree(p_SuperObjPerso, p_stTree, &stParam);
    char                  cState   = stParam.uParam.cValue;

    if (cState < 64)
    {
        AI_M_pfn_SinusEffect_SetState(cState);
    }
    else
    {
        unsigned char cFlag = AI_M_pfn_SinusEffect_GetFlag();
        switch (cState)
        {
        case 64: cFlag &= (unsigned char)~AI_C_SinusEffect_cAffectSecondViewPort; break;
        case 65: cFlag |=  AI_C_SinusEffect_cAffectSecondViewPort;                break;
        case 66: cFlag &= (unsigned char)~AI_C_SinusEffect_cUseZFactor;           break;
        case 67: cFlag |=  AI_C_SinusEffect_cUseZFactor;                          break;
        }
        AI_M_pfn_SinusEffect_SetFlag(cFlag);
    }
    return p_stNext;
}

/* 0x00472C40 */
AI_tdstNodeInterpret *__cdecl fn_p_st_SPO_SetDrawFlag(
    HIE_tdstSuperObject *p_SuperObjPerso, AI_tdstNodeInterpret *p_stTree)
{
    AI_tdstGetSetParam    stParam;
    void                 *hSpo;
    long                  lBit, lType;
    AI_tdstNodeInterpret *p_stNext;

    p_stNext = fn_p_stEvalTree(p_SuperObjPerso, p_stTree, &stParam);
    hSpo = stParam.uParam.p_stEngObjValue;
    p_stNext = fn_p_stEvalTree(p_SuperObjPerso, p_stNext, &stParam);
    lBit = stParam.uParam.lValue;
    p_stNext = fn_p_stEvalTree(p_SuperObjPerso, p_stNext, &stParam);
    lType = stParam.uParam.lValue;

    if (hSpo != NULL && lBit >= 0 && lBit <= 31)
    {
        unsigned long ulMask     = 1UL << lBit;
        unsigned long ulDrawMask = (unsigned long)HIE_fn_lGetSuperObjectDrawMask((HIE_tdstSuperObject *)hSpo);
        if (lType == 2)
            lType = ((ulDrawMask & ulMask) == 0);
        switch (lType)
        {
        case 0:
            ulDrawMask &= ~ulMask;
            break;
        case 1:
            ulDrawMask |= ulMask;
            break;
        default:
            break;
        }
        HIE_fn_vSetSuperObjectDrawMask((HIE_tdstSuperObject *)hSpo, (long)ulDrawMask);
    }
    return p_stNext;
}

/* 0x00472CD0 */
AI_tdstNodeInterpret *__cdecl fn_p_st_SPO_SetEngineDisplayModeFlag(
    HIE_tdstSuperObject *p_SuperObjPerso, AI_tdstNodeInterpret *p_stTree)
{
    AI_tdstGetSetParam    stParam;
    long                  lBit, lType;
    AI_tdstNodeInterpret *p_stNext;

    p_stNext = fn_p_stEvalTree(p_SuperObjPerso, p_stTree, &stParam);
    lBit = stParam.uParam.lValue;
    p_stNext = fn_p_stEvalTree(p_SuperObjPerso, p_stNext, &stParam);
    lType = stParam.uParam.lValue;

    if (lBit >= 0 && lBit <= 31)
    {
        unsigned long ulMask = 1UL << lBit;
        if (lType == 2)
            lType = ((ulMask & AI_g_ulEngineDisplayMode) == 0);
        switch (lType)
        {
        case 0:
            AI_g_ulEngineDisplayMode &= ~ulMask;
            break;
        case 1:
            AI_g_ulEngineDisplayMode |= ulMask;
            break;
        default:
            break;
        }
    }
    return p_stNext;
}

/* 0x00472D60 */
AI_tdstNodeInterpret *__cdecl fn_p_st_ACT_SetDrawFlag(
    HIE_tdstSuperObject *p_SuperObjPerso, AI_tdstNodeInterpret *p_stTree)
{
    AI_tdstGetSetParam    stParam;
    AI_tdstNodeInterpret *p_stNext;
    HIE_tdstSuperObject  *hCharact;
    long                  lBit, lType;

    p_stNext = fn_p_stEvalTree(p_SuperObjPerso, p_stTree, &stParam);
    hCharact = AI_M_pfn_fn_h_StandardGameGetSuperObject(stParam.uParam.p_stEngObjValue->h_StandardGame);
    p_stNext = fn_p_stEvalTree(p_SuperObjPerso, p_stNext, &stParam);
    lBit     = stParam.uParam.lValue;
    p_stNext = fn_p_stEvalTree(p_SuperObjPerso, p_stNext, &stParam);
    lType    = stParam.uParam.lValue;

    if ( (hCharact != NULL) && (lBit >= 0) && (lBit <= 31 ) )
    {
        MS_tdxHandleTo3dData h3dData = M_GetMSHandle(hCharact,3dData);
        long  lMask     = 1 << lBit;
        long  lDrawMask = h3dData->lDrawMask;

        if (lType == 2)
            lType = (lDrawMask & lMask) ? 0 : 1;

        switch (lType )
        {
            case 0:
                lDrawMask &= ~lMask;
                break;
            case 1:
                lDrawMask |= lMask;
                break;
            default:
                break;
        }
        HIE_fn_vSetSuperObjectDrawMask( hCharact, lDrawMask );
        h3dData->lDrawMask = lDrawMask;
    }
    return p_stNext;
}

/* 0x00472E20 */
AI_tdstNodeInterpret *__cdecl fn_p_stSkipLoop(AI_tdstNodeInterpret *p_stTreeInit)
{
    int                   iLevel = 0;
    AI_tdstNodeInterpret *p_stTree = p_stTreeInit;

    if (AI_M_NodeToSkip(p_stTreeInit))
        return p_stTreeInit + AI_M_NodeToSkip(p_stTreeInit);

    while (AI_M_GetTypeInterpret(p_stTree) != 8)
    {
        if (AI_M_GetTypeInterpret(p_stTree) == 4)
        {
            if (AI_M_eProcedureIdInterpret(p_stTree) == 0x4c)
            {
                iLevel++;
            }
            else if (AI_M_eProcedureIdInterpret(p_stTree) == 0x4d)
            {
                iLevel--;
                if (iLevel <= 0)
                {
                    p_stTree++;
                    break;
                }
            }
        }
        p_stTree++;
    }
    AI_M_NodeToSkip(p_stTreeInit) = (unsigned short)(p_stTree - p_stTreeInit);
    return p_stTree;
}

/* 0x00472E90 */
AI_tdstNodeInterpret *__cdecl fn_p_stLoopKeyWord(
    HIE_tdstSuperObject *p_SuperObjPerso, AI_tdstNodeInterpret *p_stTree)
{
    AI_tdstNodeInterpret *p_stTreeInit = (AI_tdstNodeInterpret *)((char *)p_stTree - 8);
    AI_tdstGetSetParam    stValue;
    AI_tdstNodeInterpret *p_stNext;
    long                  lNum;
    long                  lValue;

    AI_g_a_p_stLoopInit[AI_g_iNumLoop] = p_stTreeInit;
    p_stNext = fn_p_stEvalTree(p_SuperObjPerso, p_stTree, &stValue);
    lNum = AI_g_iNumLoop;
    AI_g_a_lLoopCpt[AI_g_iNumLoop] = stValue.uParam.lValue;
    lValue = stValue.uParam.lValue;
    if (lValue == 0)
        return fn_p_stSkipLoop(p_stTreeInit);
    AI_g_a_p_stLoopTree[lNum] = p_stNext;
    AI_g_iNumLoop = lNum + 1;
    return p_stNext;
}

/* 0x00472F00 */
AI_tdstNodeInterpret *__cdecl fn_p_stEndLoopKeyWord(
    HIE_tdstSuperObject *p_SuperObjPerso, AI_tdstNodeInterpret *p_stTree)
{
    long lNum = AI_g_iNumLoop;
    (void)p_SuperObjPerso;

    if (lNum != 0)
    {
        if (--AI_g_a_lLoopCpt[lNum - 1] > 0)
            return AI_g_a_p_stLoopTree[lNum - 1];
        AI_g_iNumLoop = lNum - 1;
    }
    return p_stTree;
}

/* 0x00472F30 */
AI_tdstNodeInterpret *__cdecl fn_p_stBreakKeyword(
    HIE_tdstSuperObject *p_SuperObjPerso, AI_tdstNodeInterpret *p_stTree)
{
    AI_tdstGetSetParam    stValue;
    long                  lValue;

    fn_p_stEvalTree(p_SuperObjPerso, p_stTree, &stValue);
    lValue = stValue.uParam.lValue;
    if (lValue > AI_g_iNumLoop - 1)
        lValue = AI_g_iNumLoop - 1;
    p_stTree = fn_p_stSkipLoop(AI_g_a_p_stLoopInit[AI_g_iNumLoop - lValue - 1]);
    AI_g_iNumLoop -= (lValue + 1);
    return p_stTree;
}

/* 0x00472F90 */
AI_tdstNodeInterpret *__cdecl fn_p_stNoneProcedure(
    HIE_tdstSuperObject *p_SuperObjPerso, AI_tdstNodeInterpret *p_stTree)
{
    (void)p_SuperObjPerso;
    return p_stTree;
}

AI_tdstNodeInterpret *__cdecl fn_p_stAGOProcedure(
    HIE_tdstSuperObject *p_SuperObjPerso, AI_tdstNodeInterpret *p_stTree)
{
    unsigned long         procId = (unsigned long)M_eProcedureIdInterpret(p_stTree-1);
    AI_tdstGetSetParam    stParam;
    AI_tdstNodeInterpret *p_stNext = p_stTree;
    unsigned long         ulEffectType;
    MTH3D_tdstVector      stVec1, stVec2;
    void                 *hVisualMaterial;
    float                 xValue;

    if (procId != 0x1f7)
        return p_stTree;

    p_stNext     = fn_p_stEvalTree(p_SuperObjPerso, p_stNext, &stParam);
    ulEffectType = stParam.uParam.ulValue;

    p_stNext = fn_p_stEvalTree(p_SuperObjPerso, p_stNext, &stParam);
    stVec1   = *(MTH3D_tdstVector *)stParam.uParam.a_xVectorValue;
    p_stNext = fn_p_stEvalTree(p_SuperObjPerso, p_stNext, &stParam);
    stVec2   = *(MTH3D_tdstVector *)stParam.uParam.a_xVectorValue;

    p_stNext        = fn_p_stEvalTree(p_SuperObjPerso, p_stNext, &stParam);
    /* 0x472FA0 */
    hVisualMaterial = NULL;
    if (stParam.uParam.p_stEngObjValue != NULL)
        hVisualMaterial = (void *)AI_M_pfn_fn_p_stDynamGetDynamics(stParam.uParam.p_stEngObjValue);

    p_stNext = fn_p_stEvalTree(p_SuperObjPerso, p_stNext, &stParam);
    xValue   = M_ReturnParam_xValue(&stParam);

    AI_M_pfn_AGO_vAddParticle(ulEffectType, &stVec1, &stVec2, hVisualMaterial, xValue);
    return p_stNext;
}

/* 0x004730A0 */
AI_tdstNodeInterpret *__cdecl fn_p_stInGameMenuProcedure(
    HIE_tdstSuperObject *p_SuperObjPerso, AI_tdstNodeInterpret *p_stTree)
{
    unsigned long         procId = (unsigned long)M_eProcedureIdInterpret(p_stTree-1);
    AI_tdstGetSetParam    stParam;

    if (procId != 0x1f8)
        return p_stTree;
    p_stTree = fn_p_stEvalTree(p_SuperObjPerso, p_stTree, &stParam);
    if (stParam.uParam.ulValue) GAM_g_bInGameMenu = TRUE; else GAM_g_bInGameMenu = FALSE;
    return p_stTree;
}
AI_tdstNodeInterpret *__cdecl AI_fn_p_stSetPcEngineFlags(
    HIE_tdstSuperObject *p_SuperObjPerso, AI_tdstNodeInterpret *p_stTree)
{
    unsigned long         procId = (unsigned long)M_eProcedureIdInterpret(p_stTree-1);
    AI_tdstGetSetParam    stParam;
    AI_tdstNodeInterpret *p_stNext;

    switch (procId)
    {
    case 0x1fe:
        p_stNext = fn_p_stEvalTree(p_SuperObjPerso, p_stTree, &stParam);
        GAM_pc_g_ulActualizeClockHookGate = (unsigned long)(stParam.uParam.lValue != 0);
        return p_stNext;
    case 0x1ff:
        p_stNext = fn_p_stEvalTree(p_SuperObjPerso, p_stTree, &stParam);
        AI_PAD_lCheat2Enable_storage = (stParam.uParam.lValue != 0);
        return p_stNext;
    case 0x200:
        p_stNext = fn_p_stEvalTree(p_SuperObjPerso, p_stTree, &stParam);
        AI_PAD_lCheat1Enable_storage = (stParam.uParam.lValue != 0);
        return p_stNext;
    case 0x201:
        p_stNext = fn_p_stEvalTree(p_SuperObjPerso, p_stTree, &stParam);
        GLI_pc_g_ucRandomPickEnabled = stParam.uParam.ucValue;   /* 0x005166c0 */
        return p_stNext;
    default:
        break;
    }
    return p_stTree;
}

AI_tdstNodeInterpret *__cdecl fn_p_stJFFTXTProcedure(
    HIE_tdstSuperObject *p_SuperObjPerso, AI_tdstNodeInterpret *p_stTree)
{
    unsigned long         procId = (unsigned long)M_eProcedureIdInterpret(p_stTree-1);
    AI_tdstGetSetParam    stParam;
    AI_tdstNodeInterpret *p_stNext;

    switch (procId)
    {
    case 0x1f9:
    {
        unsigned long    ulStringNumber;
        MTH3D_tdstVector stPosition;
        char            *pszString;
        float            xParameter;

        p_stNext = fn_p_stEvalTree(p_SuperObjPerso, p_stTree, &stParam);
        ulStringNumber = stParam.uParam.ulValue;
        p_stNext = fn_p_stEvalTree(p_SuperObjPerso, p_stNext, &stParam);
        stPosition.xX = stParam.uParam.a_xVectorValue[0];
        stPosition.xY = stParam.uParam.a_xVectorValue[1];
        stPosition.xZ = stParam.uParam.a_xVectorValue[2];
        p_stNext = fn_p_stEvalTree(p_SuperObjPerso, p_stNext, &stParam);
        pszString = fn_szGetStringFromTextOrStringParam(&stParam);
        p_stNext = fn_p_stEvalTree(p_SuperObjPerso, p_stNext, &stParam);
        xParameter = M_ReturnParam_xValue(&stParam);
        AI_M_pfn_JFFTXT_vAddText(ulStringNumber, &stPosition, (unsigned char *)pszString, xParameter);
        return p_stNext;
    }
    case 0x1fa:
    {
        unsigned long ulStringNumber, ulID;
        float         xParameter;
        p_stNext = fn_p_stEvalTree(p_SuperObjPerso, p_stTree, &stParam);
        ulStringNumber = stParam.uParam.ulValue;
        p_stNext = fn_p_stEvalTree(p_SuperObjPerso, p_stNext, &stParam);
        ulID = stParam.uParam.ulValue;
        p_stNext = fn_p_stEvalTree(p_SuperObjPerso, p_stNext, &stParam);
        xParameter = M_ReturnParam_xValue(&stParam);
        AI_M_pfn_JFFTXT_vExtendText(ulStringNumber, ulID, xParameter);
        return p_stNext;
    }
    case 0x1fb:
    {
        unsigned long ulStringNumber;
        int           lPosition, lNewCar;
        p_stNext = fn_p_stEvalTree(p_SuperObjPerso, p_stTree, &stParam);
        ulStringNumber = stParam.uParam.ulValue;
        p_stNext = fn_p_stEvalTree(p_SuperObjPerso, p_stNext, &stParam);
        lPosition = stParam.uParam.lValue;
        p_stNext = fn_p_stEvalTree(p_SuperObjPerso, p_stNext, &stParam);
        lNewCar = stParam.uParam.lValue;
        AI_M_pfn_JFFTXT_vModifyCaracter(ulStringNumber, lPosition, lNewCar);
        return p_stNext;
    }
    default:
        break;
    }
    return p_stTree;
}

/* 0x00487FD0 */
AI_tdstNodeInterpret *__cdecl fn_p_stPrepareMorph(
    HIE_tdstSuperObject *p_SuperObjPerso, AI_tdstNodeInterpret *p_stTree)
{
    ACP_tdxIndex    xStartIndexInObjectTable;
    ACP_tdxIndex    xEndIndexInObjectTable;
    int             iNbMillisecond;
    unsigned char   ucMorphType;
    ACP_tdxIndex    xChannelIndex;
    AI_tdstGetSetParam	stValue [5];

    long eProcId=M_eProcedureIdInterpret(p_stTree-1);

    HIE_tdstSuperObject *hNewSuperObjPerso;
    fn_vGetUltraOperatorPerso (fn_ucGetProcedureUltraOperator(eProcId),p_SuperObjPerso,&hNewSuperObjPerso);

    M_EvalNextParameter(&stValue[0]);
    M_EvalNextParameter(&stValue[1]);
    M_EvalNextParameter(&stValue[2]);
    M_EvalNextParameter(&stValue[3]);
    M_EvalNextParameter(&stValue[4]);

    xStartIndexInObjectTable = M_GetSetParam_lValue(&stValue[0]);
    xEndIndexInObjectTable   = M_GetSetParam_lValue(&stValue[1]);
    iNbMillisecond           = M_GetSetParam_lValue(&stValue[2]);
    ucMorphType              = M_GetSetParam_lValue(&stValue[3]);
    xChannelIndex            = M_GetSetParam_lModuleValue(&stValue[4]);

    if (MOR_fn_xPrepareMorph (p_SuperObjPerso, xStartIndexInObjectTable, xEndIndexInObjectTable,
                              iNbMillisecond, 0, iNbMillisecond, ucMorphType, xChannelIndex) == -1)
    {

        ERM_fn_vUpdateLastError(0x42, g_ucAIModuleId, 0, 0x4c, -1, 0xff, 0,
                                (char *)"Morphing requires two objects with the same number of points");
        ERM_fn_vClearLastError(0);
    }

    return (p_stTree);
}

/* 0x004880B0 */
AI_tdstNodeInterpret *__cdecl fn_p_stStopMorph(
    HIE_tdstSuperObject *p_SuperObjPerso, AI_tdstNodeInterpret *p_stTree)
{
    unsigned long         procId = (unsigned long)M_eProcedureIdInterpret(p_stTree-1);
    AI_tdstGetSetParam    stParam;
    HIE_tdstSuperObject  *hNewSuperObjPerso;

    fn_vGetUltraOperatorPerso(fn_ucGetProcedureUltraOperator(procId), p_SuperObjPerso, &hNewSuperObjPerso);
    p_stTree = fn_p_stEvalTree(p_SuperObjPerso, p_stTree, &stParam);
    MOR_fn_xStopMorph(p_SuperObjPerso, stParam.uParam.uwValue);
    return p_stTree;
}

/* 0x00488160 */
AI_tdstNodeInterpret *__cdecl fn_p_stMorphing(
    HIE_tdstSuperObject *p_SuperObjPerso, AI_tdstNodeInterpret *p_stTree)
{
    unsigned long         procId = (unsigned long)M_eProcedureIdInterpret(p_stTree-1);
    AI_tdstGetSetParam    stValue[4];
    AI_tdstNodeInterpret *p_stNode;
    HIE_tdstSuperObject  *hNewSuperObjPerso;
    ACP_tdxIndex          xStartIndexInObjectTable, xEndIndexInObjectTable, xRatio, xChannelIndex;

    fn_vGetUltraOperatorPerso(fn_ucGetProcedureUltraOperator(procId), p_SuperObjPerso, &hNewSuperObjPerso);

    p_stNode = fn_p_stEvalTree(p_SuperObjPerso, p_stTree, &stValue[0]);
    p_stNode = fn_p_stEvalTree(p_SuperObjPerso, p_stNode, &stValue[1]);
    p_stNode = fn_p_stEvalTree(p_SuperObjPerso, p_stNode, &stValue[2]);
    p_stNode = fn_p_stEvalTree(p_SuperObjPerso, p_stNode, &stValue[3]);
    xStartIndexInObjectTable = stValue[0].uParam.lValue;
    xEndIndexInObjectTable   = stValue[1].uParam.lValue;
    xRatio                   = stValue[2].uParam.lValue;
    xChannelIndex            = stValue[3].uParam.lValue;

    if (MOR_fn_xMorphing(p_SuperObjPerso, (ACP_tdxIndex)(xStartIndexInObjectTable - 1),
                         (ACP_tdxIndex)(xEndIndexInObjectTable - 1), xRatio, xChannelIndex) == -1) {
        ERM_fn_vUpdateLastError(0x42, g_ucAIModuleId, 0, 0x4c, -1, 0xff, 0,
                                (char *)"Morphing requires two objects with the same number of points");
        ERM_fn_vClearLastError(0);
    }
    return p_stNode;
}

/* 0x00497260 */
void __cdecl AI_pc_fn_vLightWalkNode(unsigned char *p_ucNode)
{
    void          *hMaterial;
    void          *hVisualMat;
    long           lVisualSet;
    short          sCount;
    short          sIndex;
    long           lIndex;
    short         *psTable;
    float          xThreshold;
    void          *hObject;
    unsigned char *pucChild;
    unsigned long  ulEntry;

    HIE_fn_vPushMatrix((HIE_tdstSuperObject *)p_ucNode);
    if ((((HIE_tdstSuperObject *)p_ucNode)->ulTypeOfLinkedObject & (HIE_C_ulIPO | HIE_C_ulIPO_Mirror)) != 0)
    {
        hMaterial = ((HIE_tdstSuperObject *)p_ucNode)->hLinkedObject.p_Void;
        hVisualMat = GMT_fn_hGetVisualMaterial((GMT_tdxHandleToGameMaterial)hMaterial);
        lVisualSet = (long)fn_h_SectInfoGetCurrentSector(hVisualMat);
        sCount = (short)GLI_lGetVisualSetNumbertOfLOD((void *)lVisualSet);
        psTable = (short *)GMT_fn_hGetMechanicsMaterial((GMT_tdxHandleToGameMaterial)hMaterial);
        if (psTable == NULL)
        {
            psTable = HIE_pc_fn_p_stCreateFrameTable(sCount);
            if (0 < (int)sCount)
            {
                sIndex = 0;
                do
                {
                    GLI_vGetVisualSetLOD((void *)lVisualSet, (long)sIndex, &xThreshold, &hObject);
                    HIE_pc_fn_pulSetFrameTableEntry(psTable, sIndex, (long)((GEO_tdstGeometricObject *)hObject)->xNbPoints);
                    sIndex++;
                } while ((int)sIndex < (int)sCount);
            }
            GMT_fn_vSetMechanicsMaterial((GMT_tdxHandleToGameMaterial)hMaterial, (void *)psTable);
        }

        if (0 < (int)sCount)
        {
            lIndex = 0;
            do
            {
                GLI_vGetVisualSetLOD((void *)lVisualSet, lIndex, &xThreshold, &hObject);
                fn_vGetFieldTypeVector();
                sIndex = HIE_pc_fn_sGetFrameTableEntryCount(psTable, (short)lIndex);
                ulEntry = HIE_pc_fn_ulGetFrameTableEntry(psTable, (short)lIndex);
                HIE_pc_fn_vStoreScaledShortRecords((short *)ulEntry, AI_pc_xLightSourceRecord, (long)sIndex);
                lIndex++;
            } while (lIndex < (long)sCount);
        }
    }
    for (pucChild = (unsigned char *)((HIE_tdstSuperObject *)p_ucNode)->p_stFirstChild; pucChild != NULL;
         pucChild = (unsigned char *)((HIE_tdstSuperObject *)pucChild)->p_stNextBrother)
    {
        AI_pc_fn_vLightWalkNode(pucChild);
    }
    HIE_fn_vPopMatrix();
}

/* 0x004973A0 */
void __cdecl AI_pc_fn_vLightWalkSector(int *param_1)
{
    int           *piVar6;
    unsigned char *pucChild;
    short          sCount;
    int            iVar1;
    int            iVar4;
    unsigned char  ucLighted;

    HIE_fn_vPushMatrix((HIE_tdstSuperObject *)param_1);
    if (((HIE_tdstSuperObject *)param_1)->ulTypeOfLinkedObject == HIE_C_ulSector)
    {
        GLI_vClearListOfLightInViewport((void *)0);
        iVar4 = LST2_M_StaticGetNumberOfElements(&((SECT_tdxHandleOfSectorObject)((HIE_tdstSuperObject *)param_1)->hLinkedObject.p_Void)->stListOfStaticLights);
        piVar6 = (int *)LST2_M_StaticGetFirstElement(&((SECT_tdxHandleOfSectorObject)((HIE_tdstSuperObject *)param_1)->hLinkedObject.p_Void)->stListOfStaticLights);
        sCount = (short)iVar4;
        if (0 < (short)iVar4)
        {
            iVar4 = (int)sCount;
            do
            {
                iVar1 = (int)((SECT_tdxHandleOfElementLstStaticLights)piVar6)->hStaticLights;
                ucLighted = GLI_ucGetObjectLighted((void *)iVar1);
                if ((ucLighted & 1) != 0)
                {
                    GLI_vAddLightToViewport((void *)0, (void *)iVar1);
                }
                piVar6 = (int *)LST2_M_StaticGetNextElement((SECT_tdxHandleOfElementLstStaticLights)piVar6);
                iVar4--;
            } while (iVar4 != 0);
        }
        pucChild = (unsigned char *)((HIE_tdstSuperObject *)param_1)->p_stFirstChild;
        if (pucChild != NULL)
        {
            do
            {
                AI_pc_fn_vLightWalkNode(pucChild);
                pucChild = (unsigned char *)((HIE_tdstSuperObject *)pucChild)->p_stNextBrother;
            } while (pucChild != NULL);
            HIE_fn_vPopMatrix();
            return;
        }
    }
    else
    {
        for (iVar4 = (int)((HIE_tdstSuperObject *)param_1)->p_stFirstChild; iVar4 != 0;
             iVar4 = (int)((HIE_tdstSuperObject *)iVar4)->p_stNextBrother)
        {
            fn_vCalculeLumiereEffet((HIE_tdstSuperObject *)iVar4);
        }
    }
    HIE_fn_vPopMatrix();
}

/* 0x00497440 */
void __cdecl fn_vCalculeLumiereEffet(HIE_tdstSuperObject *p_stSuperObj)
{
    HIE_fn_bLoadIdentity();
    HIE_fn_vInvalidateAllGlobalMatrices();
    GLI_vResetTableOfActiveLightsForChar(1);
    AI_pc_fn_vLightWalkSector((int *)p_stSuperObj);
    HIE_fn_vPopMatrix();
}
