#include <stdio.h>
#include "AI/AI_types.h"
#include "ERM/CpaError.h"
#include "AI/AI_globals.h"
#include "GAM/GAM_types.h"
#include "AI/ActConst.h"
#include "AI/StrIntel.h"
#include "AI/EnumKey.h"
#include "AI/WP_Handl.h"
#include "FON/FON_types.h"

#define E_ti_KeyWord   0
#define E_ti_EndTree   8

#define M_IsTheEndOfTree(p_Tree)           (M_GetTypeInterpret(p_Tree) == E_ti_EndTree)

#define AI_M_pc_GetEngineObject(p_stSuperObject) \
    ((HIE_M_xGetSuperObjectMember(p_stSuperObject, ulTypeOfLinkedObject) != HIE_C_ulActor) ? NULL \
     : ((struct tdstEngineObject_ *)HIE_M_xGetSuperObjectMember(p_stSuperObject, hLinkedObject.p_Void)))

/* 0x00488FF0 */
AI_tdstNodeInterpret *__cdecl fn_p_stNoActionKeyWord(
    HIE_tdstSuperObject *p_SuperObjPerso, AI_tdstNodeInterpret *p_stTree, AI_tdstGetSetParam *p_stValue)
{
  M_Full_GetSetParam_Action(p_stValue, NULL);
  return(p_stTree);
}

/* 0x00489010 */
AI_tdstNodeInterpret *__cdecl fn_p_stMeKeyWord(
    HIE_tdstSuperObject *p_SuperObjPerso, AI_tdstNodeInterpret *p_stTree, AI_tdstGetSetParam *p_stValue)
{
  M_GetSetParam_p_stEngineObjValue(p_stValue) = AI_M_pc_GetEngineObject(p_SuperObjPerso);
  M_GetSetParam_Type(p_stValue) = E_vt_Perso;
  return(p_stTree);
}

/* 0x00489050 */
AI_tdstNodeInterpret *__cdecl fn_p_stMainActorKeyWord(
    HIE_tdstSuperObject *p_SuperObjPerso, AI_tdstNodeInterpret *p_stTree, AI_tdstGetSetParam *p_stValue)
{
  M_GetSetParam_p_stEngineObjValue(p_stValue) = AI_M_pc_GetEngineObject(GAM_g_stEngineStructure.g_hMainActor);
  M_GetSetParam_Type(p_stValue) = E_vt_Perso;
  return(p_stTree);
}

/* 0x00489090 */
AI_tdstNodeInterpret *__cdecl fn_p_stNoSuperObjectKeyWord(
    HIE_tdstSuperObject *p_SuperObjPerso, AI_tdstNodeInterpret *p_stTree, AI_tdstGetSetParam *p_stValue)
{
  M_Full_GetSetParam_SuperObject(p_stValue, NULL);
  return(p_stTree);
}

/* 0x004890B0 */
AI_tdstNodeInterpret *__cdecl fn_p_stNobodyKeyWord(
    HIE_tdstSuperObject *p_SuperObjPerso, AI_tdstNodeInterpret *p_stTree, AI_tdstGetSetParam *p_stValue)
{
  M_GetSetParam_p_stEngineObjValue(p_stValue) = NULL;
  M_GetSetParam_Type(p_stValue) = E_vt_Perso;
  return(p_stTree);
}

/* 0x004890D0 */
AI_tdstNodeInterpret *__cdecl fn_p_stNowhereKeyWord(
    HIE_tdstSuperObject *p_SuperObjPerso, AI_tdstNodeInterpret *p_stTree, AI_tdstGetSetParam *p_stValue)
{
  M_Full_GetSetParam_WayPoint(p_stValue,WP_C_hWayPointInvalid);
  return(p_stTree);
}

/* 0x004890F0 */
AI_tdstNodeInterpret *__cdecl fn_p_stEmptyTextKeyWord(
    HIE_tdstSuperObject *p_SuperObjPerso, AI_tdstNodeInterpret *p_stTree, AI_tdstGetSetParam *p_stValue)
{
  M_Full_GetSetParam_hText(p_stValue, FON_C_xInvalidTextHandle);
  return(p_stTree);
}

/* 0x00489110 */
AI_tdstNodeInterpret *__cdecl fn_p_stCapsNullKeyWord(
    HIE_tdstSuperObject *p_SuperObjPerso, AI_tdstNodeInterpret *p_stTree, AI_tdstGetSetParam *p_stValue)
{
  M_Full_GetSetParam_Caps(p_stValue, 0);
  return(p_stTree);
}

/* 0x00489130 */
AI_tdstNodeInterpret *__cdecl fn_p_stNoGraphKeyWord(
    HIE_tdstSuperObject *p_SuperObjPerso, AI_tdstNodeInterpret *p_stTree, AI_tdstGetSetParam *p_stValue)
{
  M_Full_GetSetParam_Graph(p_stValue, NULL);
  return(p_stTree);
}

/* 0x00489150 */
AI_tdstNodeInterpret *__cdecl fn_p_stThenKeyWord(
    HIE_tdstSuperObject *p_SuperObjPerso, AI_tdstNodeInterpret *p_stTree, AI_tdstGetSetParam *p_stValue)
{
  return fn_p_stIntelligenceEvalTreeEngine(p_SuperObjPerso, p_stTree, p_stValue);
}

/* 0x00489170 */
AI_tdstNodeInterpret *__cdecl fn_p_stElseKeyWord(
    HIE_tdstSuperObject *p_SuperObjPerso, AI_tdstNodeInterpret *p_stTree, AI_tdstGetSetParam *p_stValue)
{
	p_stTree--;
	return fn_p_stSkipThenElseParameter(p_stTree, p_stTree);
}

/* 0x00489190 */
AI_tdstNodeInterpret *__cdecl fn_p_stSkipThenElseParameter(AI_tdstNodeInterpret *p_stTree, AI_tdstNodeInterpret *p_stTreeInit)
{
	if(M_NodeToSkip(p_stTreeInit))
	{
		return (p_stTreeInit+M_NodeToSkip(p_stTreeInit));
	}
	else
	{
		p_stTree = fn_p_stSwapNextParameter(p_stTree + 1);
		M_NodeToSkip(p_stTreeInit)=(unsigned short) (p_stTree-p_stTreeInit);
	}
	return p_stTree;
}

char gcSimulateRelease = 1;   /* 0x004A66BC */

/* 0x004891D0 */
AI_tdstNodeInterpret *__cdecl fn_p_stIfDebug(
    HIE_tdstSuperObject *p_SuperObjPerso, AI_tdstNodeInterpret *p_stTree, AI_tdstGetSetParam *p_stValue)
{
	enum tdeKeyWordId_	eKeyId=M_eKeyWordIdInterpret(p_stTree-1);
	p_SuperObjPerso=p_SuperObjPerso;
	p_stValue=p_stValue;
	if((eKeyId == eKeyWord_IfDebug) && (gcSimulateRelease))
	{
		p_stTree = fn_p_stSkipThenElseParameter(p_stTree-1, p_stTree-1);
	}
	M_Full_GetSetParam_ActionReturn(p_stValue, C_ACTION_ENGINE_CONTINUE);
    return(p_stTree);
}

/* 0x00489210 */
AI_tdstNodeInterpret *__cdecl fn_p_stIfNotU64(
    HIE_tdstSuperObject *p_SuperObjPerso, AI_tdstNodeInterpret *p_stTree, AI_tdstGetSetParam *p_stValue)
{
	enum tdeKeyWordId_	eKeyId=M_eKeyWordIdInterpret(p_stTree-1);
	p_SuperObjPerso=p_SuperObjPerso;
	p_stValue=p_stValue;
	if((eKeyId == eKeyWord_IfNotU64) && (gcSimulateNotU64))
	{
		p_stTree = fn_p_stSkipThenElseParameter(p_stTree-1, p_stTree-1);
	}
	M_Full_GetSetParam_ActionReturn(p_stValue, C_ACTION_ENGINE_CONTINUE);
    return(p_stTree);
}

/* 0x00489250 */
AI_tdstNodeInterpret *__cdecl fn_p_stIfKeyWord(
    HIE_tdstSuperObject *p_SuperObjPerso, AI_tdstNodeInterpret *p_stTree, AI_tdstGetSetParam *p_stValue)
{
	AI_tdstNodeInterpret	*p_stTreeInit=p_stTree-1;
	char				cMax = 0;

	if(M_eKeyWordIdInterpret(p_stTreeInit) == eKeyWord_If)
	{
		M_EvalNextParameter(p_stValue);
	}
	else
	{
		switch(M_eKeyWordIdInterpret(p_stTreeInit))
		{
		case eKeyWord_If2:
			cMax = gcGlobAleat & 0x01;
			goto ComIf;
		case eKeyWord_If4:
			cMax = gcGlobAleat & 0x03;
			goto ComIf;
		case eKeyWord_If8:
			cMax = gcGlobAleat & 0x07;
			goto ComIf;
		case eKeyWord_If16:
			cMax = gcGlobAleat & 0x0F;
			goto ComIf;

ComIf:
			if(cMax)
			{
				M_GetSetParam_lValue(p_stValue) = 0;
				p_stTree = fn_p_stSkipThenElseParameter(p_stTreeInit, p_stTree);
			}
			else
			{
				M_EvalNextParameter(p_stValue);
			}
			break;

		case eKeyWord_IfNot:
			M_EvalNextParameter(p_stValue);
			M_GetSetParam_lValue(p_stValue) = M_GetSetParam_lValue(p_stValue) != 0 ? 0 : 1;
			break;

		default:
			break;

		}
	}

	if (M_GetSetParam_lValue(p_stValue) != 0)
	{
		p_stTree++;
	}
	else
	{
		if (!M_IsTheEndOfTree(p_stTree))
			p_stTree = fn_p_stSkipThenElseParameter(p_stTree, p_stTreeInit);

		if (M_DepthInterpret(p_stTree) == M_DepthInterpret(p_stTreeInit))
		{
			if (   (M_GetTypeInterpret(p_stTree) == E_ti_KeyWord)
				&& (M_eKeyWordIdInterpret(p_stTree) == eKeyWord_Else))
			{
				p_stTree++;
			}
		}
	}
	M_Full_GetSetParam_ActionReturn(p_stValue, C_ACTION_ENGINE_CONTINUE);
	return(p_stTree);
}

/* 0x00407B00 */
char *__cdecl fn_szFindPersonalTypeNameOfPersonalType(long otType);

#define M_ObjectGetPersonalType(p_stObject) ((p_stObject)->h_StandardGame->otObjectPersonalType)
#define AI_M_szGetSuperObjectName(p_stSupObj) (fn_szFindPersonalTypeNameOfPersonalType(M_ObjectGetPersonalType(AI_M_pc_GetEngineObject(p_stSupObj))))

AI_tdstNodeInterpret *__cdecl fn_p_stEngineGotoKeyWord(
    HIE_tdstSuperObject *p_SuperObjPerso, AI_tdstNodeInterpret *p_stTree, AI_tdstGetSetParam *p_stValue)
{
	char szForErrorString[200];
	AI_tdstNodeInterpret *p_stTempTree;
	long lValue;

	M_EvalNextParameter(p_stValue);
	lValue = M_ReturnParam_lValue(p_stValue);

	M_Full_GetSetParam_ActionReturn(p_stValue, C_ACTION_ENGINE_CONTINUE);

    if (lValue == 0)
    {
        sprintf(szForErrorString, "parameter zero in %s", AI_M_szGetSuperObjectName(p_SuperObjPerso));
        ERM_fn_vUpdateLastError(0x42, g_ucAIModuleId, 0, 0x4d, -1, 0xff, 0, szForErrorString);
        ERM_fn_vClearLastError(0);
        return p_stTree;
    }
    else if (lValue < 0)
    {
        unsigned long ulBound;
        ulBound = (unsigned long)AI_M_GetNodeInterpret(AI_M_GetRuleN(g_p_stIntelligence, g_ucNRule));
        p_stTempTree = p_stTree + lValue;
        if ((unsigned long)p_stTempTree >= ulBound)
            return p_stTempTree;
        {
            sprintf(szForErrorString, "out of interpret tree (negative) in %s", AI_M_szGetSuperObjectName(p_SuperObjPerso));
            ERM_fn_vUpdateLastError(0x42, g_ucAIModuleId, 0, 0x4d, -1, 0xff, 0, szForErrorString);
            ERM_fn_vClearLastError(0);
            return p_stTree;
        }
    }
    else

    {
        long i;
        p_stTempTree = p_stTree;
        for (i = 0; i < lValue; i++)
        {
            if (M_IsTheEndOfTree(p_stTempTree))
            {
                sprintf(szForErrorString, "out of interpret tree (positive) in %s", AI_M_szGetSuperObjectName(p_SuperObjPerso));
                ERM_fn_vUpdateLastError(0x42, g_ucAIModuleId, 0, 0x4d, -1, 0xff, 0, szForErrorString);
                ERM_fn_vClearLastError(0);
                return p_stTree;
            }
            else
                p_stTempTree++;
        }
        return p_stTempTree;
    }
    return(p_stTree);
}
