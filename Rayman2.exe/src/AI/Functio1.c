#include <stddef.h>
#include <string.h>
#include <malloc.h>
#include <math.h>
#include <stdlib.h>

#include "ACP/ACP_types.h"
#include "GAM/GAM_types.h"
#include "DNM/DNM_types.h"
#include "GAM/PlayAnim.h"
#include "GAM/Actions/CollSet.h"
#include "GAM/MSSound.h"
#include "MTH/MTH_types.h"
#include "HIE/HIE_types.h"
#include "HIE/HieSpObj.h"
#include "AI/AI_types.h"
#include "AI/Brain.h"
#include "FON/fontext.h"
#include "GLI/Brightness.h"
#include "AI/AI_globals.h"
#include "MTH/MTH_xNeg.h"
#include "MEC/DNMSurSt.h"

extern unsigned long g_xMask;   /* 0x004B9C44 */
#define AI_g_xMask  g_xMask

MTH3D_tdstVector     AI_g_stCollideParameterPoint;   /* 0x004E93D0 */
MTH3D_tdstVector     AI_g_stCollideParameterVector;  /* 0x004E93B0 */
MTH3D_tdstVector     AI_g_stCollideResultPoint;      /* 0x004E9400 */
MTH3D_tdstVector     AI_g_stCollideResultVector;     /* 0x004E93A0 */
HIE_tdstSuperObject *AI_g_hSuperObjectHit;           /* 0x004E93F8 */

char                 AI_pc_szSlotScoreBuf[16];        /* 0x004E93C0 */
char                 AI_pc_szSlotDateBuf[16];         /* 0x004E93E0 */
char                 AI_pc_szSlotNameBuf[8];          /* 0x004E93F0 */
/* 0x004B9A94 */
unsigned long        AI_g_ulSpoMask;

typedef struct AI_tdstTfmtLineResults_ {
    short             wStartChar;
    short             wEndChar;
    MTH3D_tdstVector  stLineOrigin;
} AI_tdstTfmtLineResults;

typedef struct AI_tdstTfmtStatus_ {
    MTH_tdxReal xScreenWidth;
    MTH_tdxReal xBaseCharX, xBaseCharZ;
    MTH_tdxReal xBaseToAddX;
    MTH_tdxReal xRandomPosRadius;
    MTH_tdxReal xCharWidth, xLineHeight, xCharZoom;
    MTH_tdxReal xCurCharZoom;
    MTH_tdxReal xCurCharWidth;
    long        hSourceObjectTable;     /* 0x408A00 */
    char        cJustificationMode;
} AI_tdstTfmtStatus;

extern ACP_tdxBool HIE_bIntersectSegmentWithFirstSuperObject(          /* 0x0045D740 */
    MTH3D_tdstVector *, MTH3D_tdstVector *, HIE_tdstSuperObject *,
    MTH3D_tdstVector *, MTH3D_tdstVector *, HIE_tdstSuperObject **);
extern unsigned long __cdecl fn_ulStandardGameGetCustomBitsSO(void *hSupObj); /* 0x00413700 */

static void
AI_fn_v_RayTestPersoAndStoreActorHit(HIE_tdstSuperObject *hCurrentPerso)
{
    HIE_tdstSuperObject *hSuperObjectHit = NULL;

    if (HIE_bIntersectSegmentWithFirstSuperObject(
            &AI_g_stCollideParameterPoint,
            &AI_g_stCollideParameterVector,
            hCurrentPerso,
            &AI_g_stCollideResultPoint,
            &AI_g_stCollideResultVector,
            &hSuperObjectHit))
    {

        while (hSuperObjectHit != NULL
               && hSuperObjectHit->ulTypeOfLinkedObject != AI_C_HIE_C_ulActor)
        {
            hSuperObjectHit = hSuperObjectHit->p_stFather;
        }
        if (hSuperObjectHit != NULL)
            AI_g_hSuperObjectHit = hSuperObjectHit;
    }
}

static ACP_tdxBool
AI_fn_b_PersoIsRayTarget(HIE_tdstSuperObject *hCurrentPerso,
                        HIE_tdstSuperObject *hSender)
{
    HIE_tdstEngineObject *p_stEngObj;
    unsigned char        *p_ucStandardGame;

    if (hCurrentPerso == hSender)
        return 0;

    if ((fn_ulStandardGameGetCustomBitsSO(hCurrentPerso)
         & AI_C_GAM_C_CustBitRayHit) == 0)
        return 0;

    p_stEngObj      = (HIE_tdstEngineObject *)hCurrentPerso->hLinkedObject.p_Void;
    p_ucStandardGame = (unsigned char *)p_stEngObj->hStandardGame;
    if ((p_ucStandardGame[0x29] & 0x4) == 0)
        return 0;

    return 1;
}

typedef struct AI_tdstCharListNode_ {
    HIE_tdstSuperObject              *hPointerOfCharacter;
    struct AI_tdstCharListNode_      *pNext;
} AI_tdstCharListNode;

typedef struct AI_tdstActivityNode_ {
    HIE_tdstSuperObject              *hPointerOfSectorSO;
    struct AI_tdstActivityNode_      *pNext;
} AI_tdstActivityNode;

typedef struct AI_tdstGraphicNode_ {
    unsigned char                     _pad_0[2];
    char                              cDisplayMode;
    unsigned char                     _pad_1;
    HIE_tdstSuperObject              *hPointerOfSectorSO;
    struct AI_tdstGraphicNode_       *pNext;
} AI_tdstGraphicNode;

typedef struct AI_tdstSectorObject_ {
    AI_tdstCharListNode              *a_pCharListHead;
    unsigned char                     _pad_04[0x24 - 0x04];
    AI_tdstGraphicNode               *a_pGraphicHead;
    unsigned char                     _pad_28[0x2C - 0x28];
    long                              lNbGraphicNodes;
    unsigned char                     _pad_30[0x3C - 0x30];
    AI_tdstActivityNode              *a_pActivityHead;
    unsigned char                     _pad_40[0x44 - 0x40];
    long                              lNbActivityNodes;
} AI_tdstSectorObject;

extern HIE_tdstSuperObject *__cdecl SECT_fn_hResearchInWhatSectorIAm(HIE_tdstSuperObject *SECT_hFatherSector, MTH3D_tdstVector *p_stAbsolutePosition);
extern char __cdecl SECT_fn_bIsThisSectorVirtual(void *_hSprObjSector);
extern void __cdecl SECT_fn_vGetMinPointInBorder(void *_hSector, MTH3D_tdstVector *_p_stMinPoint);
extern void __cdecl SECT_fn_vGetMaxPointInBorder(void *_hSector, MTH3D_tdstVector *_p_stMaxPoint);
extern unsigned long __cdecl fn_ulStandardGameGetCustomBitsSO(void *hSupObj);
extern ACP_tdxBool INT_fn_bDetectIntersectSegmentWithBox(MTH3D_tdstVector *p_stVertex1, MTH3D_tdstVector *p_stVect12, MTH3D_tdstVector *p_stMinPoint, MTH3D_tdstVector *p_stMaxPoint, MTH_tdxReal *_p_xT);

void __cdecl fn_vComputeCollideResult(HIE_tdstSuperObject *hSender)
{
    HIE_tdstSuperObject *hSuperObjectHit;
    HIE_tdstSuperObject *hSaveSO;
    HIE_tdstSuperObject *hCurrentPerso;
    AI_tdstCharListNode *hCharacterList;
    HIE_tdstSuperObject *hSector1;
    HIE_tdstSuperObject *hSector;
    AI_tdstGraphicNode  *hList;
    AI_tdstActivityNode *hInterList;
    AI_tdstSectorObject *p_stSectorObject;
    long                 i, j;
    ACP_tdxBool          bDynamCollide = 0;

    AI_g_stCollideResultPoint.xX  = 0;
    AI_g_stCollideResultPoint.xY  = 0;
    AI_g_stCollideResultPoint.xZ  = 0;
    AI_g_stCollideResultVector.xX = 0;
    AI_g_stCollideResultVector.xY = 0;
    AI_g_stCollideResultVector.xZ = 0;
    AI_g_hSuperObjectHit = NULL;
    hSaveSO = NULL;
    hSuperObjectHit = NULL;

    hSector1 = SECT_fn_hResearchInWhatSectorIAm(SECT_hFatherSector, &AI_g_stCollideParameterPoint);

    if (AI_g_ulMode != 1)
    {

        AI_g_ulSpoMask = AI_g_ulOverRideRayTraceSpoMask;
        if (hSector1 == NULL)
        {

            HIE_bIntersectSegmentWithFirstSuperObject(&AI_g_stCollideParameterPoint, &AI_g_stCollideParameterVector, SECT_hFatherSector, &AI_g_stCollideResultPoint, &AI_g_stCollideResultVector, &AI_g_hSuperObjectHit);
        }
        else
        {

            HIE_bIntersectSegmentWithFirstSuperObject(&AI_g_stCollideParameterPoint, &AI_g_stCollideParameterVector, hSector1, &AI_g_stCollideResultPoint, &AI_g_stCollideResultVector, &AI_g_hSuperObjectHit);

            p_stSectorObject = (AI_tdstSectorObject *)hSector1->hLinkedObject.p_Void;
            for (i = p_stSectorObject->lNbGraphicNodes, hList = p_stSectorObject->a_pGraphicHead; i > 0; i--, hList = hList->pNext)
            {

                if (hList->cDisplayMode == AI_C_SECT_ModeLookOnly)
                    break;
                hSector = hList->hPointerOfSectorSO;

                if (!SECT_fn_bIsThisSectorVirtual(hSector))
                {
                    HIE_bIntersectSegmentWithFirstSuperObject(&AI_g_stCollideParameterPoint, &AI_g_stCollideParameterVector, hSector, &AI_g_stCollideResultPoint, &AI_g_stCollideResultVector, &AI_g_hSuperObjectHit);
                }
            }
        }

        if (AI_g_ulMode == 0)
        {
            return;
        }
    }

    AI_g_ulSpoMask = 0;

    p_stSectorObject = (AI_tdstSectorObject *)hSector1->hLinkedObject.p_Void;
    for (i = 0, hCharacterList = p_stSectorObject->a_pCharListHead; hCharacterList != NULL; i++, hCharacterList = hCharacterList->pNext)
    {
        hCurrentPerso = hCharacterList->hPointerOfCharacter;

        if (hCurrentPerso != hSender)
        {
            if ((fn_ulStandardGameGetCustomBitsSO(hCurrentPerso) & AI_C_GAM_C_CustBitRayHit) != 0 &&
                (((unsigned char *)((HIE_tdstEngineObject *)hCurrentPerso->hLinkedObject.p_Void)->hStandardGame)[0x29] & 0x4) != 0)
            {
                if (HIE_bIntersectSegmentWithFirstSuperObject(&AI_g_stCollideParameterPoint, &AI_g_stCollideParameterVector, hCurrentPerso, &AI_g_stCollideResultPoint, &AI_g_stCollideResultVector, &hSuperObjectHit))
                {
                    bDynamCollide = 1;

                    while (hSuperObjectHit != NULL && hSuperObjectHit->ulTypeOfLinkedObject != AI_C_HIE_C_ulActor)
                    {

                        hSuperObjectHit = hSuperObjectHit->p_stFather;
                    }

                    if (hSuperObjectHit != NULL)
                        AI_g_hSuperObjectHit = hSuperObjectHit;
                }
            }
        }
    }

    p_stSectorObject = (AI_tdstSectorObject *)hSector1->hLinkedObject.p_Void;
    for (i = p_stSectorObject->lNbActivityNodes, hInterList = p_stSectorObject->a_pActivityHead; i > 0; i--, hInterList = hInterList->pNext)
    {
        MTH3D_tdstVector     stMin, stMax;
        AI_tdstSectorObject *p_stNbrSector;

        hSector = hInterList->hPointerOfSectorSO;
        p_stNbrSector = (AI_tdstSectorObject *)hSector->hLinkedObject.p_Void;

        SECT_fn_vGetMinPointInBorder(p_stNbrSector, &stMin);
        SECT_fn_vGetMaxPointInBorder(p_stNbrSector, &stMax);
        if (INT_fn_bDetectIntersectSegmentWithBox(&AI_g_stCollideParameterPoint, &AI_g_stCollideParameterVector, &stMin, &stMax, NULL))
        {

            for (j = 0, hCharacterList = ((AI_tdstSectorObject *)hSector->hLinkedObject.p_Void)->a_pCharListHead; hCharacterList != NULL; j++, hCharacterList = hCharacterList->pNext)
            {
                hCurrentPerso = hCharacterList->hPointerOfCharacter;
                if (hCurrentPerso != hSender)
                {
                    if ((fn_ulStandardGameGetCustomBitsSO(hCurrentPerso) & AI_C_GAM_C_CustBitRayHit) != 0 &&
                        (((unsigned char *)((HIE_tdstEngineObject *)hCurrentPerso->hLinkedObject.p_Void)->hStandardGame)[0x29] & 0x4) != 0)
                    {
                        if (HIE_bIntersectSegmentWithFirstSuperObject(&AI_g_stCollideParameterPoint, &AI_g_stCollideParameterVector, hCurrentPerso, &AI_g_stCollideResultPoint, &AI_g_stCollideResultVector, &hSuperObjectHit))
                        {

                            while (hSuperObjectHit != NULL && hSuperObjectHit->ulTypeOfLinkedObject != AI_C_HIE_C_ulActor)
                            {

                                hSuperObjectHit = hSuperObjectHit->p_stFather;
                            }

                            if (hSuperObjectHit != NULL)
                                AI_g_hSuperObjectHit = hSuperObjectHit;
                        }
                    }
                }
            }
        }
    }
}

/* 0x00478E10 */
AI_tdstNodeInterpret *__cdecl fn_p_stNamesFunc(
    HIE_tdstSuperObject *p_SuperObjPerso,
    AI_tdstNodeInterpret *p_stTree,
    AI_tdstGetSetParam *p_stValue)
{
    long eFuncId = AI_M_eFuncIdInterpret(p_stTree - 1);

    switch ( eFuncId )
    {
    case AI_eFunc_GetInputEntryName:
        {
            struct IPT_tdstEntryElement_ *IPT_xHandle;

            M_EvalNextParameter(p_stValue);
            IPT_xHandle = (struct IPT_tdstEntryElement_ *) M_GetSetParam_lValue(p_stValue);
            M_Full_GetSetParam_String(p_stValue, AI_RT_IPT_fn_szGetEntryActionName(IPT_xHandle));
        }
        return p_stTree;
        break;

    case AI_eFunc_GetFormattedTextInfo:
        {
            unsigned char ucArrayStartCharId, ucArrayEndCharId, ucArrayPosVectorId, ucMaxNbLines, ucCurrentLine;
            AI_tdstTfmtStatus stStatus;
            AI_tdstTfmtLineResults *d_stLineResults;
            MTH_tdxReal xScreenWidth;
            char *pszString;

            M_EvalNextParameter(p_stValue);
            pszString = (char *)fn_szGetStringFromTextOrStringParam(p_stValue);
            M_EvalNextParameter(p_stValue);
            xScreenWidth = M_ReturnParam_xValue(p_stValue);
            ucArrayStartCharId = (unsigned char) M_ucVarIdInterpret(p_stTree);
            p_stTree ++;
            ucArrayEndCharId = (unsigned char) M_ucVarIdInterpret(p_stTree);
            p_stTree ++;
            ucArrayPosVectorId = (unsigned char) M_ucVarIdInterpret(p_stTree);
            p_stTree ++;
            M_EvalNextParameter(p_stValue);
            ucMaxNbLines = M_ReturnParam_lValue(p_stValue);

            d_stLineResults = (AI_tdstTfmtLineResults *) _alloca(sizeof(AI_tdstTfmtLineResults) * ucMaxNbLines);

            ucMaxNbLines = ucCurrentLine = AI_RT_TFMT_wPositionModulesAccordingToText(&stStatus, p_SuperObjPerso, pszString, 0, -1, xScreenWidth, d_stLineResults, ucMaxNbLines);

            while ( ucCurrentLine -- )
            {
                M_Full_GetSetParam_Integer(p_stValue, d_stLineResults[ucCurrentLine].wStartChar);
                if ( AI_RT_fn_ucSetDsgVar(ucArrayStartCharId, ucCurrentLine, AI_M_stGetMindOfSuperObj(p_SuperObjPerso), p_stValue) == C_INVALID_SET )
                    break;
                M_Full_GetSetParam_Integer(p_stValue, d_stLineResults[ucCurrentLine].wEndChar);
                if ( AI_RT_fn_ucSetDsgVar(ucArrayEndCharId, ucCurrentLine, AI_M_stGetMindOfSuperObj(p_SuperObjPerso), p_stValue) == C_INVALID_SET )
                    break;
                M_Full_GetSetParam_p_stVertex(p_stValue, &d_stLineResults[ucCurrentLine].stLineOrigin);
                if ( AI_RT_fn_ucSetDsgVar(ucArrayPosVectorId, ucCurrentLine, AI_M_stGetMindOfSuperObj(p_SuperObjPerso), p_stValue) == C_INVALID_SET )
                    break;
            }

            M_Full_GetSetParam_Integer(p_stValue, ucMaxNbLines);
        }
        return p_stTree;
        break;

    case AI_eFunc_GetSlotName:
        {
            long  lSlotId;
            char *pszSlot;

            M_EvalNextParameter(p_stValue);
            lSlotId = M_ReturnParam_lValue(p_stValue);

            if (lSlotId == 0) { AI_RT_pc_vEnsureDefaultSlot(); lSlotId = 1; }

            pszSlot = AI_RT_pc_szGetSlotString(lSlotId);
            strncpy(AI_pc_szSlotNameBuf, pszSlot, 3);
            AI_pc_szSlotNameBuf[3] = '\0';
            M_Full_GetSetParam_String(p_stValue, AI_pc_szSlotNameBuf);
        }
        return p_stTree;
        break;

    case AI_eFunc_GetSlotScore:
        {
            long  lSlotId;
            char *pszSlot;

            M_EvalNextParameter(p_stValue);
            lSlotId = M_ReturnParam_lValue(p_stValue);

            if (lSlotId == 0) { AI_RT_pc_vEnsureDefaultSlot(); lSlotId = 1; }

            pszSlot = AI_RT_pc_szGetSlotString(lSlotId);
            strcpy(AI_pc_szSlotScoreBuf, pszSlot + 3);
            AI_pc_szSlotScoreBuf[5] = '\0';
            M_Full_GetSetParam_String(p_stValue, AI_pc_szSlotScoreBuf);
        }
        return p_stTree;
        break;

    case AI_eFunc_GetSlotDate:
        {
            long  lSlotId;
            char *pszDate;

            M_EvalNextParameter(p_stValue);
            lSlotId = M_ReturnParam_lValue(p_stValue);

            if (lSlotId == 0) { AI_RT_pc_vEnsureDefaultSlot(); lSlotId = 1; }

            pszDate = AI_RT_pc_szFormatSlotDate(AI_RT_pc_lGetSlotId(lSlotId));
            strncpy(AI_pc_szSlotDateBuf, pszDate, 0x0B);
            AI_pc_szSlotDateBuf[0x0B] = '\0';
            M_Full_GetSetParam_String(p_stValue, AI_pc_szSlotDateBuf);
        }
        return p_stTree;
        break;

    case AI_eFunc_GetStringCharAt:
        {
            register short wCpt = 0;
            short wIndexInString = 0;
            char *pszString = NULL;

            M_EvalNextParameter(p_stValue);
            pszString = (char *)fn_szGetStringFromTextOrStringParam(p_stValue);

            M_EvalNextParameter(p_stValue);
            wIndexInString = M_GetSetParam_lValue(p_stValue);

            while( pszString[wCpt++] == '/')
            {
                while( pszString[wCpt++] != ':' )
                {}
            }
            wIndexInString += wCpt-1;

            if ( (wIndexInString>= 0) && (wIndexInString < (short) strlen(pszString)) )
            {
                M_Full_GetSetParam_Integer(p_stValue, pszString[wIndexInString]);
            }
            else
            {
                M_Full_GetSetParam_Integer(p_stValue, 0);
            }
        }
        return p_stTree;
        break;

    default:
        return p_stTree;
        break;
    }
}

#define AI_DT_ulUsefulDeltaTime   GAM_g_ulUsefulDeltaTime  /* 0x500434 */

/* 0x00478580 */
AI_tdstNodeInterpret *__cdecl fn_p_stGetDTFunction(
    HIE_tdstSuperObject *p_SuperObjPerso, AI_tdstNodeInterpret *p_stTree, AI_tdstGetSetParam *p_stValue)
{
    p_stValue->uParam.xValue = (float)AI_DT_ulUsefulDeltaTime;
    p_stValue->eType         = 8;
    return p_stTree;
}

#define AI_VPA_xReadReal(v)  ((v)->eType == 8 ? (v)->uParam.xValue : (float)(v)->uParam.lValue)

/* 0x00474A00 */
AI_tdstNodeInterpret *__cdecl fn_p_stCode4VitessePadAnalogique(
    HIE_tdstSuperObject *p_SuperObjPerso, AI_tdstNodeInterpret *p_stTree, AI_tdstGetSetParam *p_stValue)
{

    long eFuncId = AI_M_eFuncIdInterpret(p_stTree - 1);

    switch (eFuncId)
    {
    case 0x41:
        {
            long                  lSpeedFromIA;
            MTH_tdxReal           xXvalue, xYvalue, xSpeedX, xSpeedY;
            MTH3D_tdstVector      stSpeedVector;

            M_EvalNextParameter(p_stValue);
            xYvalue = MTH_M_xNeg( M_ReturnParam_xValue(p_stValue) );

            M_EvalNextParameter(p_stValue);
            xXvalue = MTH_M_xNeg( M_ReturnParam_xValue(p_stValue) );

            M_EvalNextParameter(p_stValue);
            lSpeedFromIA = M_ReturnParam_lValue(p_stValue);

            xSpeedX = MTH_M_xMul( lSpeedFromIA, MTH_M_xDiv( xXvalue, 100 ) );

            xSpeedY = MTH_M_xMul( lSpeedFromIA, MTH_M_xDiv( xYvalue, 100 ) );

            MTH3D_M_vNullVector( &stSpeedVector );
            MTH3D_M_vSetXofVector( &stSpeedVector, xSpeedX );
            MTH3D_M_vSetZofVector( &stSpeedVector, xSpeedY );
            MTH3D_M_vNormalizeVector( &stSpeedVector, &stSpeedVector );
            MTH3D_M_vMulScalarVector( &stSpeedVector, lSpeedFromIA, &stSpeedVector );

            M_Full_GetSetParam_p_stVertex( p_stValue, &stSpeedVector );
        }
        return p_stTree;

    default:
        break;
    }
    return(p_stTree);
}

#define AI_GCM_ulCurrentTimerCount GAM_g_ulAccumulatedTime  /* 0x5003EC */
#define AI_C_HIE_Flag_ulNotHitByRayTrace  ((unsigned long)0x40)

/* 0x00479910 */
AI_tdstNodeInterpret *__cdecl fn_p_stGetCollisionWithCollideMaterial(
    HIE_tdstSuperObject *p_SuperObjPerso, AI_tdstNodeInterpret *p_stTree, AI_tdstGetSetParam *p_stValue)
{

	MTH3D_tdstVector *p_stPoint;
	MTH3D_tdstVector *p_stVector,stVectorSave;
	unsigned long ulMode;
	unsigned long xTestedMask;
	int	bCompute = FALSE;

	M_EvalNextParameter(p_stValue);
	p_stPoint=&M_GetSetParam_stVectorValue(p_stValue);
	if (!MTH3D_M_bEqualVector (&AI_g_stCollideParameterPoint,p_stPoint))
	{
		MTH3D_M_vCopyVector (&AI_g_stCollideParameterPoint,p_stPoint);
		bCompute = TRUE;
	}

	M_EvalNextParameter(p_stValue);
	p_stVector=&M_GetSetParam_stVectorValue(p_stValue);
	if (!MTH3D_M_bEqualVector (&AI_g_stCollideParameterVector,p_stVector))
	{
		MTH3D_M_vCopyVector (&AI_g_stCollideParameterVector,p_stVector);
		bCompute = TRUE;
	}

	M_EvalNextParameter(p_stValue);
	ulMode=M_GetSetParam_ulValue (p_stValue);

    M_EvalNextParameter(p_stValue);
	 xTestedMask = (unsigned long) M_GetSetParam_xMaskValue(p_stValue);

	if (bCompute ||
		AI_g_ulCollideTimeCounter != AI_GCM_ulCurrentTimerCount ||
		AI_g_ulMode != ulMode
	)
	{
		MTH3D_M_vCopyVector (&stVectorSave,&AI_g_stCollideParameterVector);
		AI_g_xMask = xTestedMask;
		AI_g_ulCollideTimeCounter = AI_GCM_ulCurrentTimerCount;
		AI_g_hSuperObjectHit = NULL;
		AI_g_ulMode = ulMode;
		fn_vComputeCollideResult (p_SuperObjPerso);
		MTH3D_M_vCopyVector (&AI_g_stCollideParameterVector,&stVectorSave);
		AI_g_xMask = 0;
	}

	AI_g_ulOverRideRayTraceSpoMask = AI_C_HIE_Flag_ulNotHitByRayTrace;

	M_Full_GetSetParam_p_stVertex(p_stValue,&AI_g_stCollideResultPoint);
	return(p_stTree);
}

#define AI_M_pfn_GAM_hGetSuperObjectFromStdGame           ((void *(__cdecl *)(void *))&fn_h_StandardGameGetSuperObject)
/* 0x00411FC0 */
extern void *__cdecl fn_hSuperObjectGetLastCollidedCharacter(void *hCharacter);
#define AI_M_pfn_fn_hSuperObjectGetLastCollidedCharacter  fn_hSuperObjectGetLastCollidedCharacter

extern void __cdecl fn_vOnAIErm_M_UpdateLastError(unsigned short FatalErrorNum, char *szMsg);
#define AI_M_pfn_M_AIFatalError_fn1(uwError) \
    fn_vOnAIErm_M_UpdateLastError((unsigned short)(uwError), (char *)0)

#define AI_M_pfn_fn_ucGetFunctionUltraOperator_fn1        fn_ucGetFunctionUltraOperator

/* 0x00479680 */
AI_tdstNodeInterpret *__cdecl fn_p_stGetLastCollisionActor(
    HIE_tdstSuperObject *p_SuperObjPerso, AI_tdstNodeInterpret *p_stTree, AI_tdstGetSetParam *p_stValue)
{
    long                 eFuncId = AI_M_eFuncIdInterpret(p_stTree - 1);
    HIE_tdstSuperObject *p_SuperObjPersoTreated = NULL;
    HIE_tdstSuperObject *p_SuperObjCollidedActor = NULL;
    AI_tdstGetSetParam   stParam;

    if (eFuncId == 0xd0)
    {

        M_EvalNextParameter(&stParam);
        p_SuperObjPersoTreated = M_GetSetParam_p_stSupObjValue(&stParam);

        p_SuperObjCollidedActor = fn_hSuperObjectGetLastCollidedCharacter(p_SuperObjPersoTreated);

        M_Full_GetSetParam_Perso(p_stValue,p_SuperObjCollidedActor);

        return (p_stTree);
    }
    M_AIFatalError(E_uwAIFatalNotValidFunction);
    return(p_stTree);
}

/* 0x00479AC0 */
AI_tdstNodeInterpret *__cdecl fn_p_stComputeRebondVector(
    HIE_tdstSuperObject *p_SuperObjPerso, AI_tdstNodeInterpret *p_stTree, AI_tdstGetSetParam *p_stValue)
{
    MTH3D_tdstVector stNormalVector;
    MTH3D_tdstVector stInitVector;
    MTH_tdxReal      xCoefAbsorption;
    MTH_tdxReal      xReactionNorm;
    MTH3D_tdstVector stReactionVector;

    p_stTree = fn_p_stEvalTree(p_SuperObjPerso, p_stTree, p_stValue);
    stInitVector = *(MTH3D_tdstVector *)p_stValue->uParam.a_xVectorValue;

    p_stTree = fn_p_stEvalTree(p_SuperObjPerso, p_stTree, p_stValue);
    stNormalVector = *(MTH3D_tdstVector *)p_stValue->uParam.a_xVectorValue;

    p_stTree = fn_p_stEvalTree(p_SuperObjPerso, p_stTree, p_stValue);
    xCoefAbsorption = M_ReturnParam_xValue(p_stValue);

    xReactionNorm = MTH_M_xMul(-2.0f, MTH3D_M_xDotProductVector(&stInitVector, &stNormalVector));

    MTH3D_M_vMulScalarVector(&stReactionVector, xReactionNorm, &stNormalVector);

    MTH3D_M_vAddVector(&stReactionVector, &stReactionVector, &stInitVector);

    MTH3D_M_vMulScalarVector(&stReactionVector, xCoefAbsorption, &stReactionVector);

    p_stValue->eType = 0x15;
    *(MTH3D_tdstVector *)p_stValue->uParam.a_xVectorValue = stReactionVector;
    return p_stTree;
}

static const unsigned long AI_VAA_xEpsilon_bits = 0x3727C5ACu;
#define AI_VAA_xEpsilon              (*(const float *)&AI_VAA_xEpsilon_bits)
#define AI_VAA_xRadToDeg             AI_K_xRadToDeg   /* 0x0049CCE4 */

extern unsigned long MTH_g_a2048_fInvSquareRootTable[2048];   /* 0x0050BB60 */
#define AI_VAA_a_lQuickInvNormTable_storage  MTH_g_a2048_fInvSquareRootTable
#define AI_VAA_p_lQuickInvNormTable  ((const int *)AI_VAA_a_lQuickInvNormTable_storage)

#define AI_M_pfn_fn_p_stSkipThisArgument  fn_p_stSkipThisArgument

static void AI_VAA_vQuickNormalize(float *px, float *py, float *pz)
{
    float         xSq    = *pz * *pz + *py * *py + *px * *px;
    unsigned long ulBits = *(unsigned long *)&xSq;
    int           lInv   = (int)(((0xBD800000u - (ulBits & 0x7F800000u)) >> 1) & 0x7F800000u)
                         + *(const int *)((const char *)AI_VAA_p_lQuickInvNormTable
                                          + ((ulBits & 0xFFE000) >> 0xB));
    float         xInv   = *(float *)&lInv;
    *px *= xInv; *py *= xInv; *pz *= xInv;
}

static float AI_VAA_xQuickNorm(float x, float y, float z)
{
    float         xSq    = z * z + y * y + x * x;
    unsigned long ulBits = *(unsigned long *)&xSq;
    int           lRes   = (int)((((ulBits & 0x7F800000u) + 0x3F800000u) >> 1) & 0x7F800000u)
                         + *(const int *)((const char *)AI_VAA_p_lQuickNormTable
                                          + ((ulBits & 0xFFE000) >> 0xB));
    return *(float *)&lRes;
}

/* 0x00479BE0 */
AI_tdstNodeInterpret *__cdecl fn_p_stVectorAndAngle(
    HIE_tdstSuperObject *p_SuperObjPerso, AI_tdstNodeInterpret *p_stTree, AI_tdstGetSetParam *p_stValue)
{
    unsigned long  eFuncId = AI_M_eFuncIdInterpret(p_stTree - 1);
    ACP_tdxBool    bASourceVectorIsNull = 0;
    float          xEps = AI_VAA_xEpsilon;
    unsigned char  ucPlan;
    float          Ux, Uy, Uz;
    float          Vx, Vy, Vz;
    float          xCos, xSin, xAngle, xResult;
    float          xCrossX, xCrossY, xCrossZ;

    p_stTree = fn_p_stEvalTree(p_SuperObjPerso, p_stTree, p_stValue);
    Ux = p_stValue->uParam.a_xVectorValue[0];
    Uy = p_stValue->uParam.a_xVectorValue[1];
    Uz = p_stValue->uParam.a_xVectorValue[2];

    p_stTree = fn_p_stEvalTree(p_SuperObjPerso, p_stTree, p_stValue);
    Vx = p_stValue->uParam.a_xVectorValue[0];
    Vy = p_stValue->uParam.a_xVectorValue[1];
    Vz = p_stValue->uParam.a_xVectorValue[2];

    if (xEps <= MTH_M_xAbs(Ux) || xEps <= MTH_M_xAbs(Uy) || xEps <= MTH_M_xAbs(Uz))
        AI_VAA_vQuickNormalize(&Ux, &Uy, &Uz);
    else
        bASourceVectorIsNull = 1;

    if (xEps <= MTH_M_xAbs(Vx) || xEps <= MTH_M_xAbs(Vy) || xEps <= MTH_M_xAbs(Vz))
        AI_VAA_vQuickNormalize(&Vx, &Vy, &Vz);
    else
        bASourceVectorIsNull = 1;

    switch (eFuncId)
    {
    case 0xBA:
        if (bASourceVectorIsNull)
        {
            p_stTree = (AI_tdstNodeInterpret *)AI_M_pfn_fn_p_stSkipThisArgument(p_stTree);
            xResult  = 0.0f;
        }
        else
        {
            p_stTree = fn_p_stEvalTree(p_SuperObjPerso, p_stTree, p_stValue);
            ucPlan   = p_stValue->uParam.ucValue;

            xCos = Ux * Vx + Uy * Vy + Uz * Vz;
            if (xCos > 1.0f)       xCos = 1.0f;
            else if (xCos < -1.0f) xCos = -1.0f;
            xAngle = (float)acos((double)xCos);

            if (ucPlan == 0)      { if (Ux * Vy - Uy * Vx < 0.0f) xAngle = -xAngle; }
            else if (ucPlan == 1) { if (Uy * Vz - Uz * Vy < 0.0f) xAngle = -xAngle; }
            else if (ucPlan == 2) { if (Uz * Vx - Ux * Vz < 0.0f) xAngle = -xAngle; }

            xResult = xAngle * AI_VAA_xRadToDeg;
        }
        break;

    case 0xBB:
        if (bASourceVectorIsNull)
        {
            xResult = 1.0f;
        }
        else
        {
            xCos = Ux * Vx + Uy * Vy + Uz * Vz;
            if (xCos > 1.0f)       xCos = 1.0f;
            else if (xCos < -1.0f) xCos = -1.0f;
            xResult = xCos;
        }
        break;

    case 0xBC:
        if (bASourceVectorIsNull)
        {
            p_stTree = (AI_tdstNodeInterpret *)AI_M_pfn_fn_p_stSkipThisArgument(p_stTree);
            xResult  = 0.0f;
        }
        else
        {
            p_stTree = fn_p_stEvalTree(p_SuperObjPerso, p_stTree, p_stValue);
            ucPlan   = p_stValue->uParam.ucValue;

            xCrossX = Uy * Vz - Uz * Vy;
            xCrossY = Uz * Vx - Ux * Vz;
            xCrossZ = Ux * Vy - Uy * Vx;
            xSin    = AI_VAA_xQuickNorm(xCrossX, xCrossY, xCrossZ);

            if (ucPlan == 0)      { if (xCrossZ < 0.0f) xSin = -xSin; }
            else if (ucPlan == 1) { if (xCrossX < 0.0f) xSin = -xSin; }
            else if (ucPlan == 2) { if (xCrossY < 0.0f) xSin = -xSin; }

            if (xSin > 1.0f)       xSin = 1.0f;
            else if (xSin < -1.0f) xSin = -1.0f;
            xResult = xSin;
        }
        break;

    default:
        return p_stTree;
    }

    p_stValue->uParam.xValue = xResult;
    p_stValue->eType         = 8;
    return p_stTree;
}

extern void __cdecl GEO_vGetPointOfObject(void *hObject, MTH3D_tdstVector *p_stPoint, short xIndex);
extern void __cdecl GEO_vGetMinMaxPointOfIndexedAlignedBox(void *hObject, short hElement,
                                                           short xBoxIdx, short *p_xMin, short *p_xMax);

#define AI_M_pfn_fn_hGetGeometricZdxOfTypeAtIndex \
    ((void *(__cdecl *)(unsigned char, unsigned short, void *))&fn_hGetGeometricZdxOfTypeAtIndex)

extern float __cdecl GEO_xGetRadiusOfIndexedSphere(void *hObject, short hElement, short xSphereIdx);

#define C_ucTypeZdd (unsigned char)0
#define C_ucTypeZdm (unsigned char)1
#define C_ucTypeZde (unsigned char)2
#define C_ucTypeZdr (unsigned char)3

/* 0x00474B60 */
AI_tdstNodeInterpret *__cdecl fn_p_stCode4LitCentreZDx(
    HIE_tdstSuperObject *p_SuperObjPerso, AI_tdstNodeInterpret *p_stTree, AI_tdstGetSetParam *p_stValue)
{
    AI_tdstGetSetParam    stParam;
    HIE_tdstSuperObject  *hPerso;
    ACP_tdxHandleOfObject hGeoObj;
    long                  lZoneNumber;
    MTH3D_tdstVector      stVectorMin,
                          stVectorMax;
    ACP_tdxIndex          xMaxPoint,
                          xMinPoint;

    long                  eFuncId = AI_M_eFuncIdInterpret(p_stTree - 1);

    fn_vGetUltraOperatorPerso(fn_ucGetFunctionUltraOperator(eFuncId), p_SuperObjPerso, &hPerso);

    M_EvalNextParameter( &stParam );
    lZoneNumber = (long) M_GetSetParam_lZDMIdValue( &stParam );

    switch( eFuncId )
    {
    case 0xA9:

          hGeoObj = fn_hGetGeometricZdxOfTypeAtIndex( C_ucTypeZdm, (unsigned short)(lZoneNumber-1), hPerso );
          break;

    case 0xAA:

          hGeoObj = fn_hGetGeometricZdxOfTypeAtIndex( C_ucTypeZde, (unsigned short)(lZoneNumber-1), hPerso );
          break;

    case 0xAB:

          hGeoObj = fn_hGetGeometricZdxOfTypeAtIndex( C_ucTypeZdd, (unsigned short)(lZoneNumber-1), hPerso );
          break;

    default:

          M_AIFatalError(E_uwAIFatalNotValidFunction);
          return(p_stTree);
          break;
    }

    if( hGeoObj )
    {
        switch( *hGeoObj->d_xListOfElementsTypes )
        {
          case GEO_C_xElementPoints:
          case GEO_C_xElementSpheres:
              {

                M_Full_GetSetParam_p_stVertex( p_stValue, &hGeoObj->d_stListOfPoints[0] );
              }
              break;

          case GEO_C_xElementAlignedBoxes:
              {

                GEO_vGetMinMaxPointOfIndexedAlignedBox( hGeoObj, 0, 0, &xMinPoint, &xMaxPoint );

                GEO_vGetPointOfObject( hGeoObj, &stVectorMin, xMinPoint );
                GEO_vGetPointOfObject( hGeoObj, &stVectorMax, xMaxPoint );

                MTH3D_M_vMiddleVector( &stVectorMin, &stVectorMin, &stVectorMax );

                M_Full_GetSetParam_p_stVertex( p_stValue, &stVectorMin );
              }
              break;

          default:
              {

                MTH3D_M_vNullVector( &stVectorMin );
                M_Full_GetSetParam_p_stVertex( p_stValue, &stVectorMin );
              }
              break;
        }
    }
    else
    {

        MTH3D_M_vNullVector( &stVectorMin );
        M_Full_GetSetParam_p_stVertex( p_stValue, &stVectorMin );
    }

    return(p_stTree);

}

/* 0x004754D0 */
AI_tdstNodeInterpret *__cdecl fn_p_stCode4LitPositionZDx(
    HIE_tdstSuperObject *p_SuperObjPerso, AI_tdstNodeInterpret *p_stTree, AI_tdstGetSetParam *p_stValue)
{
    AI_tdstGetSetParam    stParam;
    HIE_tdstSuperObject  *hPerso;
    ACP_tdxHandleOfObject hGeoObj;
    long                  lZoneNumber;
    MTH3D_tdstVector      stVectorMin;
    ACP_tdxIndex          xMaxPoint,
                          xMinPoint;

    long                  eFuncId = AI_M_eFuncIdInterpret(p_stTree - 1);

    fn_vGetUltraOperatorPerso(fn_ucGetFunctionUltraOperator(eFuncId), p_SuperObjPerso, &hPerso);

    M_EvalNextParameter( &stParam );
    lZoneNumber = (long) M_GetSetParam_lZDMIdValue( &stParam );

    switch( eFuncId )
    {
    case 0xA6:

          hGeoObj = fn_hGetGeometricZdxOfTypeAtIndex( C_ucTypeZdm, (unsigned short)(lZoneNumber-1), hPerso );
          break;

    case 0xA7:

          hGeoObj = fn_hGetGeometricZdxOfTypeAtIndex( C_ucTypeZde, (unsigned short)(lZoneNumber-1), hPerso );
          break;

    case 0xA8:

          hGeoObj = fn_hGetGeometricZdxOfTypeAtIndex( C_ucTypeZdd, (unsigned short)(lZoneNumber-1), hPerso );
          break;

    default:
        {

          M_AIFatalError(E_uwAIFatalNotValidFunction);
          return(p_stTree);
          break;
        }
    }

    if( hGeoObj )
    {
        switch( *hGeoObj->d_xListOfElementsTypes )
        {
        case GEO_C_xElementPoints:
        case GEO_C_xElementSpheres:
            M_Full_GetSetParam_p_stVertex( p_stValue, &hGeoObj->d_stListOfPoints[0] );
            return(p_stTree);
            break;

        case GEO_C_xElementAlignedBoxes:
        case GEO_C_xElementCones:

            GEO_vGetMinMaxPointOfIndexedAlignedBox ( hGeoObj, 0, 0, &xMinPoint, &xMaxPoint );

            GEO_vGetPointOfObject( hGeoObj, &stVectorMin, xMinPoint );

            M_Full_GetSetParam_p_stVertex( p_stValue, &stVectorMin );
            return(p_stTree);
            break;
        }
    }

    return(p_stTree);

}

/* 0x00474D40 */
AI_tdstNodeInterpret *__cdecl fn_p_stCode4LitDimensionZDx(
    HIE_tdstSuperObject *p_SuperObjPerso, AI_tdstNodeInterpret *p_stTree, AI_tdstGetSetParam *p_stValue)
{
    AI_tdstGetSetParam    stParam;
    HIE_tdstSuperObject  *hPerso;
    ACP_tdxHandleOfObject hGeoObj;
    long                  lZoneNumber;
    MTH3D_tdstVector      stVectorMin,
                          stVectorMax;
    ACP_tdxIndex          xMaxPoint,
                          xMinPoint;

    long                  eFuncId = AI_M_eFuncIdInterpret(p_stTree - 1);

    fn_vGetUltraOperatorPerso(fn_ucGetFunctionUltraOperator(eFuncId), p_SuperObjPerso, &hPerso);

    M_EvalNextParameter( &stParam );
    lZoneNumber = (long) M_GetSetParam_lZDMIdValue( &stParam );

    switch( eFuncId )
    {
    case 0xAF:

          hGeoObj = fn_hGetGeometricZdxOfTypeAtIndex( C_ucTypeZdm, (unsigned short)(lZoneNumber-1), hPerso );
          break;

    case 0xB0:

          hGeoObj = fn_hGetGeometricZdxOfTypeAtIndex( C_ucTypeZde, (unsigned short)(lZoneNumber-1), hPerso );
          break;

    case 0xB1:

          hGeoObj = fn_hGetGeometricZdxOfTypeAtIndex( C_ucTypeZdd, (unsigned short)(lZoneNumber-1), hPerso );
          break;

    default:
        {

          M_AIFatalError(E_uwAIFatalNotValidFunction);
          return(p_stTree);
          break;
        }
    }

    if( hGeoObj )
    {
        switch( *hGeoObj->d_xListOfElementsTypes )
        {
          case GEO_C_xElementSpheres:
              {
                MTH_tdxReal xRadius;
                xRadius = GEO_xGetRadiusOfIndexedSphere(hGeoObj, 0, 0);
                MTH3D_M_vSetVectorElements(&stVectorMin, xRadius, xRadius, xRadius);
              }
              break;

          case GEO_C_xElementAlignedBoxes:
              {

                GEO_vGetMinMaxPointOfIndexedAlignedBox( hGeoObj, 0, 0, &xMinPoint, &xMaxPoint );

                GEO_vGetPointOfObject( hGeoObj, &stVectorMin, xMinPoint );
                GEO_vGetPointOfObject( hGeoObj, &stVectorMax, xMaxPoint );

                MTH3D_M_vSetVectorElements(&stVectorMin,
                                            MTH_M_xAbs(MTH_M_xSub(stVectorMin.xX,stVectorMax.xX)),
                                            MTH_M_xAbs(MTH_M_xSub(stVectorMin.xY,stVectorMax.xY)),
                                            MTH_M_xAbs(MTH_M_xSub(stVectorMin.xZ,stVectorMax.xZ)));
              }
              break;

          default:
              {

                MTH3D_M_vNullVector( &stVectorMin );
              }
              break;
        }
    }
    else
    {
        MTH3D_M_vNullVector( &stVectorMin );
    }

    M_Full_GetSetParam_p_stVertex( p_stValue, &stVectorMin );
    return(p_stTree);

}

/* 0x00474EF0 */
AI_tdstNodeInterpret *__cdecl fn_p_stCode4LitAxeZDx(
    HIE_tdstSuperObject *p_SuperObjPerso, AI_tdstNodeInterpret *p_stTree, AI_tdstGetSetParam *p_stValue)
{
    AI_tdstGetSetParam    stParam;
    HIE_tdstSuperObject  *hPerso;
    ACP_tdxHandleOfObject hGeoObj;
    long                  lZoneNumber,
                          lLocalGlobal;
    MTH_tdxReal           fMinX,
                          fMinY,
                          fMinZ,
                          fMaxX,
                          fMaxY,
                          fMaxZ;
    MTH3D_tdstVector      stVectorMin,
                          stVectorMax,
                          stAxisVector;
    ACP_tdxIndex          xMaxPoint,
                          xMinPoint;

    long                  eFuncId = AI_M_eFuncIdInterpret(p_stTree - 1);

    fn_vGetUltraOperatorPerso(fn_ucGetFunctionUltraOperator(eFuncId), p_SuperObjPerso, &hPerso);

    M_EvalNextParameter( &stParam );
    lZoneNumber = (long) M_GetSetParam_lZDMIdValue( &stParam );

    M_EvalNextParameter(p_stValue);
    lLocalGlobal = M_GetSetParam_lValue(p_stValue);

    switch( eFuncId )
    {
    case 0xAC:

          hGeoObj = fn_hGetGeometricZdxOfTypeAtIndex( C_ucTypeZdm, (unsigned short)(lZoneNumber-1), hPerso );
          break;

    case 0xAD:

          hGeoObj = fn_hGetGeometricZdxOfTypeAtIndex( C_ucTypeZde, (unsigned short)(lZoneNumber-1), hPerso );
          break;

    case 0xAE:

          hGeoObj = fn_hGetGeometricZdxOfTypeAtIndex( C_ucTypeZdd, (unsigned short)(lZoneNumber-1), hPerso );
          break;

    default:
        {

          M_AIFatalError(E_uwAIFatalNotValidFunction);
          return(p_stTree);
          break;
        }
    }

    MTH3D_M_vNullVector( &stAxisVector );

    if( hGeoObj )
    {
        switch( *hGeoObj->d_xListOfElementsTypes )
        {
          case GEO_C_xElementAlignedBoxes:
              {

                GEO_vGetMinMaxPointOfIndexedAlignedBox( hGeoObj, 0, 0, &xMinPoint, &xMaxPoint );

                GEO_vGetPointOfObject( hGeoObj, &stVectorMin, xMinPoint );
                GEO_vGetPointOfObject( hGeoObj, &stVectorMax, xMaxPoint );

                MTH3D_M_vGetVectorElements(&fMinX,&fMinY,&fMinZ,&stVectorMin);
                MTH3D_M_vGetVectorElements(&fMaxX,&fMaxY,&fMaxZ,&stVectorMax);

                fMaxX -= fMinX;
                fMaxY -= fMinY;
                fMaxZ -= fMinZ;

                if ( fMaxX>fMaxY && fMaxX>fMaxZ )
                {
                    MTH3D_M_vSetXofVector( &stAxisVector, MTH_C_ONE );
                }

                if ( fMaxY>fMaxX && fMaxY>fMaxZ )
                {
                    MTH3D_M_vSetYofVector( &stAxisVector, MTH_C_ONE );
                }

                if ( fMaxZ>fMaxX && fMaxZ>fMaxY )
                {
                    MTH3D_M_vSetZofVector( &stAxisVector, MTH_C_ONE );
                }
              }
              break;

          default:
              {

                MTH3D_M_vNullVector( &stVectorMin );
              }
              break;
        }
    }
    else
    {

      MTH3D_M_vNullVector( &stVectorMin );
    }

    if ( lLocalGlobal == TRUE )
    {
      GEO_tdxHandleToMatrix hMatrix;

      hMatrix = HIE_fn_hGetSuperObjectGlobalMatrix(p_SuperObjPerso);

      POS_fn_vRotateVector(&stAxisVector,hMatrix, &stAxisVector);
    }

    M_Full_GetSetParam_p_stVertex( p_stValue, &stAxisVector );

    return(p_stTree);
}

#define AI_VPAX_xEpsilon  (1.0e-4f)

/* 0x004750F0 */
AI_tdstNodeInterpret *__cdecl fn_p_stCode4VecteurPointAxe(
    HIE_tdstSuperObject *p_SuperObjPerso, AI_tdstNodeInterpret *p_stTree, AI_tdstGetSetParam *p_stValue)
{

    AI_tdstGetSetParam    stParam1, stParam2, stParam3;
    MTH3D_tdstVector      stvVector, stdVector, stOMVector, *p_stMVector, *p_stOVector, *p_stuVector;
    long                  eFuncId = AI_M_eFuncIdInterpret(p_stTree-1);

    M_EvalNextParameter( &stParam1 );
    p_stMVector = &M_GetSetParam_stVectorValue( &stParam1 );

    M_EvalNextParameter( &stParam2 );
    p_stOVector = &M_GetSetParam_stVectorValue( &stParam2 );

    M_EvalNextParameter( &stParam3 );
    p_stuVector = &M_GetSetParam_stVectorValue( &stParam3 );

    switch( eFuncId )
    {
    case 0xb3:
        {
            MTH_tdxReal xNormOP, xNormV;
            MTH3D_tdstVector stPVector = *p_stuVector;

            MTH3D_M_vSubVector( &stOMVector, p_stMVector, p_stOVector );

            MTH3D_M_vSubVector(p_stuVector, p_stuVector, p_stOVector);
            if
            (
                /* 0x0049cca0 */
                MTH_M_bIsNullWithEpsilon(MTH3D_M_xGetXofVector(p_stuVector), AI_VPAX_xEpsilon)
                && MTH_M_bIsNullWithEpsilon(MTH3D_M_xGetYofVector(p_stuVector), AI_VPAX_xEpsilon)
                && MTH_M_bIsNullWithEpsilon(MTH3D_M_xGetZofVector(p_stuVector), AI_VPAX_xEpsilon)
            )
            {

                MTH3D_M_vNegVector(&stdVector, &stOMVector);
            }
            else
            {

                xNormOP = MTH3D_M_xNormVector(p_stuVector);
                MTH3D_M_vNormalizeVector(p_stuVector, p_stuVector);

                xNormV = MTH3D_M_xDotProductVector(&stOMVector, p_stuVector);

                if ( MTH_M_bLessZero(xNormV) )
                {

                    MTH3D_M_vNegVector(&stdVector, &stOMVector);
                }
                else
                {

                    MTH3D_M_vMulScalarVector(&stvVector, xNormV, p_stuVector);

                    if ( MTH_M_bGreater(xNormV, xNormOP))
                    {

                        MTH3D_M_vSubVector( &stdVector, &stPVector, p_stMVector);
                    }
                    else
                    {

                        MTH3D_M_vSubVector( &stdVector, &stvVector, &stOMVector);
                    }
                }
            }
        }
        break;

    case 0xb2:
        {

            MTH3D_M_vSubVector( &stOMVector, p_stMVector, p_stOVector );

            if
            (
                /* 0x0049cca0 */
                MTH_M_bIsNullWithEpsilon(MTH3D_M_xGetXofVector(p_stuVector), AI_VPAX_xEpsilon)
                && MTH_M_bIsNullWithEpsilon(MTH3D_M_xGetYofVector(p_stuVector), AI_VPAX_xEpsilon)
                && MTH_M_bIsNullWithEpsilon(MTH3D_M_xGetZofVector(p_stuVector), AI_VPAX_xEpsilon)
            )
            {

                MTH3D_M_vNullVector(&stdVector);
            }
            else
            {
                MTH_tdxReal xTmp;
                MTH3D_M_vNormalizeVector(p_stuVector, p_stuVector);
                xTmp = MTH3D_M_xDotProductVector(&stOMVector, p_stuVector);
                MTH3D_M_vMulScalarVector(&stvVector, xTmp, p_stuVector);

                MTH3D_M_vSubVector( &stdVector, &stvVector, &stOMVector);
            }
        }
        break;

    default:
        {

            M_AIFatalError(E_uwAIFatalNotValidFunction);
            return(p_stTree);
            break;
        }
    }

    M_Full_GetSetParam_p_stVertex( p_stValue, &stdVector );

    return(p_stTree);
}

/* 0x00479720 */
AI_tdstNodeInterpret *__cdecl fn_p_stGetCollision(
    HIE_tdstSuperObject *p_SuperObjPerso, AI_tdstNodeInterpret *p_stTree, AI_tdstGetSetParam *p_stValue)
{
	unsigned long eFuncId=AI_M_eFuncIdInterpret(p_stTree-1);
	MTH3D_tdstVector *p_stPoint;
	MTH3D_tdstVector *p_stVector,stVectorSave;
	unsigned long ulMode;
	int	bCompute = FALSE;

	M_EvalNextParameter(p_stValue);
	p_stPoint=&M_GetSetParam_stVectorValue(p_stValue);
	if (!MTH3D_M_bEqualVector (&AI_g_stCollideParameterPoint,p_stPoint))
	{
		MTH3D_M_vCopyVector (&AI_g_stCollideParameterPoint,p_stPoint);
		bCompute = TRUE;
	}

	M_EvalNextParameter(p_stValue);
	p_stVector=&M_GetSetParam_stVectorValue(p_stValue);
	if (!MTH3D_M_bEqualVector (&AI_g_stCollideParameterVector,p_stVector))
	{
		MTH3D_M_vCopyVector (&AI_g_stCollideParameterVector,p_stVector);
		bCompute = TRUE;
	}

	M_EvalNextParameter(p_stValue);
	ulMode=M_GetSetParam_ulValue (p_stValue);

	if (bCompute ||
		AI_g_ulCollideTimeCounter != AI_GCM_ulCurrentTimerCount ||
		AI_g_ulMode != ulMode
	)
	{
		MTH3D_M_vCopyVector (&stVectorSave,&AI_g_stCollideParameterVector);
		AI_g_ulCollideTimeCounter = AI_GCM_ulCurrentTimerCount;
		AI_g_hSuperObjectHit = NULL;
		AI_g_ulMode = ulMode;
		AI_g_xMask = 0;
		fn_vComputeCollideResult (p_SuperObjPerso);
		MTH3D_M_vCopyVector (&AI_g_stCollideParameterVector,&stVectorSave);
	}

	switch (eFuncId)
	{
		case 0xc6:
		M_Full_GetSetParam_p_stVertex(p_stValue,&AI_g_stCollideResultPoint);
		break;

		case 0xc7:
		M_Full_GetSetParam_p_stVertex(p_stValue,&AI_g_stCollideResultVector);
		break;

		case 0xc8:
		M_Full_GetSetParam_Perso(p_stValue,AI_g_hSuperObjectHit);
		break;

		default:
break;
	}

	AI_g_ulOverRideRayTraceSpoMask = AI_C_HIE_Flag_ulNotHitByRayTrace;

	return(p_stTree);
}

extern unsigned char __cdecl fn_ucCollSetGetColliderType(void *hCollSet);

extern float            __cdecl fn_xCollSetGetColliderReal(void *hCollSet, short wIndex);
extern MTH3D_tdstVector __cdecl fn_stCollSetGetColliderVector(void *hCollSet, short wIndex);
#include "AI/GSParam.h"

/* 0x004788E0 */
AI_tdstNodeInterpret *__cdecl fn_p_stCollideCommunicationFunc(
    HIE_tdstSuperObject *p_SuperObjPerso, AI_tdstNodeInterpret *p_stTree, AI_tdstGetSetParam *p_stValue)
{
  unsigned long eFuncId = AI_M_eFuncIdInterpret(p_stTree-1);
	short wIndex = 0;
	MS_tdxHandleToCollSet _hCollSet;

	HIE_tdstSuperObject *hNewSuperObjPerso;
	fn_vGetUltraOperatorPerso(AI_M_pfn_fn_ucGetFunctionUltraOperator_fn1(eFuncId),p_SuperObjPerso,&hNewSuperObjPerso);

	_hCollSet = M_GetMSHandle(hNewSuperObjPerso,CollSet);

	switch ( eFuncId )
	{
	case 0xcd:
		if ( _hCollSet )
			wIndex = fn_ucCollSetGetColliderType(_hCollSet);
		M_Full_GetSetParam_Integer(p_stValue, wIndex);
		break;

	case 0xce:
		{

			MTH3D_tdstVector *p_stDstVect = &M_GetSetParam_stVertexValue(p_stValue);

			M_EvalNextParameter(p_stValue);
			wIndex = M_GetSetParam_wValue(p_stValue);

			if ( _hCollSet )
				*p_stDstVect = fn_stCollSetGetColliderVector(_hCollSet, wIndex);
			else
				MTH3D_M_vNullVector(p_stDstVect);
			M_GetSetParam_Type(p_stValue) = E_vt_Vector;
		}
		break;

	case 0xcf:
		{
			M_EvalNextParameter(p_stValue);
			wIndex = M_GetSetParam_wValue(p_stValue);
			M_Full_GetSetParam_Float(p_stValue, _hCollSet ? fn_xCollSetGetColliderReal(_hCollSet, wIndex) : MTH_C_ZERO );
		}
		break;
	default:
break;
	}

	return p_stTree;
}

extern unsigned char __cdecl CS_fn_eGetPrivilegedActivationZone(unsigned char ucZoneType, void *hCollSet, short wIndex);

/* 0x0047A490 */
AI_tdstNodeInterpret *__cdecl fn_p_GetPriviligedActivationZdx(
    HIE_tdstSuperObject *p_SuperObjPerso, AI_tdstNodeInterpret *p_stTree, AI_tdstGetSetParam *p_stValue)
{
    long                  eFuncId = AI_M_eFuncIdInterpret(p_stTree - 1);
    HIE_tdstSuperObject  *hNewSuperObjPerso;
    AI_tdstGetSetParam    stParam;
    long                  lZoneNumber;
    unsigned char         ePrivilege;
    MS_tdxHandleToCollSet hCollSet;

    fn_vGetUltraOperatorPerso(
        AI_M_pfn_fn_ucGetFunctionUltraOperator_fn1(eFuncId), p_SuperObjPerso, &hNewSuperObjPerso);

    hCollSet = M_GetMSHandle(hNewSuperObjPerso, CollSet);

    M_EvalNextParameter(&stParam);
    lZoneNumber = (long)M_GetSetParam_lZDMIdValue(&stParam);
    lZoneNumber--;

    switch (eFuncId)
    {
    case 0xf1:
        ePrivilege = CS_fn_eGetPrivilegedActivationZone(C_ucTypeZdd, hCollSet, (unsigned short)lZoneNumber);
        M_Full_GetSetParam_Integer(p_stValue, (unsigned short)ePrivilege);
        break;

    case 0xf2:
        ePrivilege = CS_fn_eGetPrivilegedActivationZone(C_ucTypeZdm, hCollSet, (unsigned short)lZoneNumber);
        M_Full_GetSetParam_Integer(p_stValue, (unsigned short)ePrivilege);
        break;

    case 0xf3:
        ePrivilege = CS_fn_eGetPrivilegedActivationZone(C_ucTypeZde, hCollSet, (unsigned short)lZoneNumber);
        M_Full_GetSetParam_Integer(p_stValue, (unsigned short)ePrivilege);
        break;

    case 0xf4:
        ePrivilege = CS_fn_eGetPrivilegedActivationZone(C_ucTypeZdr, hCollSet, (unsigned short)lZoneNumber);
        M_Full_GetSetParam_Integer(p_stValue, (unsigned short)ePrivilege);
        break;

    default:
        AI_M_pfn_M_AIFatalError_fn1(0x27);
        break;
    }

    return p_stTree;
}

extern unsigned char __cdecl fn_ucCollSetGetCollComputeFrequency(void *hCollSet);

/* 0x0047A590 */
static __inline unsigned char AI_F1_uc3dDataGetBrainComputationFrequency(MS_tdxHandleTo3dData h_3dData)
{
    return (unsigned char)(h_3dData ? h_3dData->ucBrainComputationFrequency : 1);
}
static __inline unsigned char AI_F1_uc3dDataGetLightComputationFrequency(MS_tdxHandleTo3dData h_3dData)
{
    return (unsigned char)(h_3dData ? h_3dData->ucLightComputationFrequency : 1);
}

AI_tdstNodeInterpret *__cdecl fn_p_GetComputationFrequency(
    HIE_tdstSuperObject *p_SuperObjPerso, AI_tdstNodeInterpret *p_stTree, AI_tdstGetSetParam *p_stValue)
{
    HIE_tdstSuperObject *hPerso;
    unsigned char        ucFrequency;
    long                 eFuncId = AI_M_eFuncIdInterpret(p_stTree - 1);

    fn_vGetUltraOperatorPerso(fn_ucGetFunctionUltraOperator(eFuncId), p_SuperObjPerso, &hPerso);

    switch( eFuncId )
    {
    case 0xf6:
        ucFrequency = AI_F1_uc3dDataGetBrainComputationFrequency( M_GetMSHandle(hPerso,3dData) );
        M_GetSetParam_ucValue(p_stValue) = ucFrequency;
        M_GetSetParam_Type	 (p_stValue) = E_vt_0To255;
        break;

    case 0xf5:
        ucFrequency = fn_ucCollSetGetCollComputeFrequency( M_GetMSHandle(hPerso,CollSet) );
        M_GetSetParam_ucValue(p_stValue) = ucFrequency;
        M_GetSetParam_Type	 (p_stValue) = E_vt_0To255;
        break;

    case 0xf7:
        ucFrequency = AI_F1_uc3dDataGetLightComputationFrequency( M_GetMSHandle(hPerso,3dData) );
        M_GetSetParam_ucValue(p_stValue) = ucFrequency;
        M_GetSetParam_Type	 (p_stValue) = E_vt_0To255;
        break;

    default:

        M_AIFatalError(E_uwAIFatalNotValidFunction);
        return(p_stTree);
        break;
    }

    return(p_stTree);
}

#define AI_OPT_ucNbLanguages       (*(unsigned char *)(GAM_ENGINE_BASE + 0xC29))

/* 0x0047AAB0 */
AI_tdstNodeInterpret *__cdecl fn_p_stGetDriversAvailable(
    HIE_tdstSuperObject *p_SuperObjPerso, AI_tdstNodeInterpret *p_stTree, AI_tdstGetSetParam *p_stValue)
{
    (void)p_SuperObjPerso;
    p_stValue->uParam.lValue = 0;
    p_stValue->eType         = 6;
    return p_stTree;
}

/* 0x0047AAD0 */
AI_tdstNodeInterpret *__cdecl fn_p_stGetCurrentLanguageId(
    HIE_tdstSuperObject *p_SuperObjPerso, AI_tdstNodeInterpret *p_stTree, AI_tdstGetSetParam *p_stValue)
{
    (void)p_SuperObjPerso;
    p_stValue->uParam.lValue = (long)GAM_g_ucCurrentLanguage;
    p_stValue->eType         = 6;
    return p_stTree;
}

/* 0x0047AAF0 */
AI_tdstNodeInterpret *__cdecl fn_p_stGetNbLanguages(
    HIE_tdstSuperObject *p_SuperObjPerso, AI_tdstNodeInterpret *p_stTree, AI_tdstGetSetParam *p_stValue)
{
    (void)p_SuperObjPerso;
    p_stValue->uParam.lValue = (long)AI_OPT_ucNbLanguages;
    p_stValue->eType         = 6;
    return p_stTree;
}

#include "GAM/STRUCTUR/Engmode.h"

/* 0x0047AB10 */
AI_tdstNodeInterpret *__cdecl fn_p_stGetLanguageText(
    HIE_tdstSuperObject *p_SuperObjPerso, AI_tdstNodeInterpret *p_stTree, AI_tdstGetSetParam *p_stValue)
{
    AI_tdstNodeInterpret *p_stNext;
    AI_tdstGetSetParam    stParam;
    long                  lId;

    p_stNext = fn_p_stEvalTree(p_SuperObjPerso, p_stTree, &stParam);
    lId      = stParam.uParam.lValue;
    p_stValue->uParam.szString = ((tdstLanguageStructure *)GAM_g_stEngineStructure.p_stLanguageTable)[lId].szLanguageText;
    p_stValue->eType           = 9;
    return p_stNext;
}

/* 0x0047AB50 */
AI_tdstNodeInterpret *__cdecl fn_p_stTextToInt(
    HIE_tdstSuperObject *p_SuperObjPerso, AI_tdstNodeInterpret *p_stTree, AI_tdstGetSetParam *p_stValue)
{
    AI_tdstNodeInterpret *p_stNext;
    AI_tdstGetSetParam    stParam;

    p_stNext = fn_p_stEvalTree(p_SuperObjPerso, p_stTree, &stParam);
    p_stValue->uParam.lValue = atoi(fn_szGetStringFromTextOrStringParam(&stParam));
    p_stValue->eType         = 6;
    return p_stNext;
}

extern long __cdecl SND_fn_lGetVolumeLineSound(void *pLine);
/* 0x4FF7CC */
extern void *GAM_g_lSoundVolumeLineMusic;
extern void *GAM_g_lSoundVolumeLineSound;
#define AI_OPT_p_lSoundVolumeLineMusic  (GAM_g_lSoundVolumeLineMusic)
#define AI_OPT_p_lSoundVolumeLineSound  (GAM_g_lSoundVolumeLineSound)
/* 0x00404E20 */
extern unsigned char __cdecl GAM_fn_bIsSlotValid(short _wSlotdId);
#define AI_M_pfn_GAM_fn_bIsSlotValid    GAM_fn_bIsSlotValid

/* 0x0047ABA0 */
AI_tdstNodeInterpret *__cdecl fn_p_stOptionRecupererFunc(
    HIE_tdstSuperObject *p_SuperObjPerso, AI_tdstNodeInterpret *p_stTree, AI_tdstGetSetParam *p_stValue)
{
    unsigned long eFuncId = *(unsigned long *)((char *)p_stTree - 8);
    long          lValue;
    (void)p_SuperObjPerso;

    switch (eFuncId)
    {
    case 0x100:
        lValue = SND_fn_lGetVolumeLineSound(AI_OPT_p_lSoundVolumeLineMusic);
        break;

    case 0x101:
        lValue = SND_fn_lGetVolumeLineSound(AI_OPT_p_lSoundVolumeLineSound);
        break;

    default:
        AI_M_pfn_M_AIFatalError_fn1(0x27);
        return p_stTree;
    }
    p_stValue->uParam.lValue = lValue;
    p_stValue->eType         = 6;
    return p_stTree;
}

/* 0x0047ABF0 */
AI_tdstNodeInterpret *__cdecl fn_p_stOptionSlotIsValid(
    HIE_tdstSuperObject *p_SuperObjPerso, AI_tdstNodeInterpret *p_stTree, AI_tdstGetSetParam *p_stValue)
{
    long lValue;

    p_stTree = fn_p_stEvalTree(p_SuperObjPerso, p_stTree, p_stValue);
    lValue = p_stValue->uParam.lValue;

    p_stValue->uParam.lValue = (long)AI_M_pfn_GAM_fn_bIsSlotValid((short)lValue);
    p_stValue->eType = 6;
    return p_stTree;
}

/* 0x0047AD80 */
long __cdecl fn_xMSSoundGetSaturationDistance(void *h_MSSound);
long __cdecl fn_xMSSoundGetBackGroundDistance(void *h_MSSound);
AI_tdstNodeInterpret *__cdecl fn_p_stGetMSSoundValues(
    HIE_tdstSuperObject *p_SuperObjPerso, AI_tdstNodeInterpret *p_stTree, AI_tdstGetSetParam *p_stValue)
{
    HIE_tdstSuperObject  *hPerso;
    long                  eFuncId = AI_M_eFuncIdInterpret(p_stTree - 1);
    MS_tdxHandleToMSSound hMSSound;
    unsigned long         ulValue = 0;
    long                  xSndValue;

    fn_vGetUltraOperatorPerso(fn_ucGetFunctionUltraOperator(eFuncId), p_SuperObjPerso, &hPerso);

    hMSSound = M_GetMSHandle(hPerso, MSSound);

    if (hMSSound)
    {
        switch (eFuncId)
        {
        case 0x10c:
            xSndValue = fn_xMSSoundGetSaturationDistance(hMSSound);
            ulValue = M_RealToIntSnd(xSndValue);
            break;
        case 0x10d:
            xSndValue = fn_xMSSoundGetBackGroundDistance(hMSSound);
            ulValue = M_RealToIntSnd(xSndValue);
            break;
        default:
            M_AIFatalError(E_uwAIFatalNotValidFunction);
            break;
        }
    }

    M_Full_GetSetParam_Integer(p_stValue, ulValue);

    return(p_stTree);
}

/* 0x0047AE10 */
static __inline unsigned char fn_ucStandardGameGetTransparencyZoneMin(MS_tdxHandleToStandardGame _hStdGame)
    { return _hStdGame->ucTransparencyZoneMin; }

static __inline unsigned char fn_ucStandardGameGetTransparencyZoneMax(MS_tdxHandleToStandardGame _hStdGame)
    { return _hStdGame->ucTransparencyZoneMax; }

AI_tdstNodeInterpret *__cdecl fn_p_stGetStdGameLimit(
    HIE_tdstSuperObject *p_SuperObjPerso, AI_tdstNodeInterpret *p_stTree, AI_tdstGetSetParam *p_stValue)
{
    unsigned long        eFuncId = AI_M_eFuncIdInterpret(p_stTree - 1);
    HIE_tdstSuperObject *hNewSuperObjPerso;
    MS_tdxHandleToStandardGame hStdGame;
    unsigned long        ulValue = 0;

    fn_vGetUltraOperatorPerso(
        AI_M_pfn_fn_ucGetFunctionUltraOperator_fn1(eFuncId), p_SuperObjPerso, &hNewSuperObjPerso);

    hStdGame = M_GetMSHandle(hNewSuperObjPerso, StandardGame);
    if (hStdGame)
    {
        switch (eFuncId)
        {
        case 0x10e:
            ulValue = fn_ucStandardGameGetTooFarLimit(hStdGame);
            break;
        case 0x10f:
            ulValue = fn_ucStandardGameGetTransparencyZoneMin(hStdGame);
            break;
        case 0x110:
            ulValue = fn_ucStandardGameGetTransparencyZoneMax(hStdGame);
            break;
        default:
            AI_M_pfn_M_AIFatalError_fn1(0x27);
            break;
        }
    }
    M_Full_GetSetParam_Integer(p_stValue, ulValue);
    return p_stTree;
}

#define AI_M_pfn_fn_pstGetDNMReport_fn1   fn_pstGetDNMReport

#define AI_C_WOT_ulNoObstacle  0x00000000UL
#define AI_C_WOT_ulGround      0x00000001UL
#define AI_C_WOT_ulWall        0x00000004UL
#define AI_C_WOT_ulCeiling     0x00000010UL
#define AI_C_WOT_ulWater       0x00000040UL
#define AI_C_WOT_ulMobile      0x00010000UL

#define AI_M_AddObstacleNorm(_acc, _p_stObstacle)                                        \
    do {                                                                                 \
        const MTH3D_tdstVector *_p_stN = DNM_M_p_stObstacleGetNorm(_p_stObstacle);       \
        (_acc).xX += _p_stN->xX;                                                         \
        (_acc).xY += _p_stN->xY;                                                         \
        (_acc).xZ += _p_stN->xZ;                                                         \
    } while (0)

#pragma optimize("y", off)
/* 0x004785B0 */
AI_tdstNodeInterpret *__cdecl fn_p_stGetNormalCollideVector(
    HIE_tdstSuperObject *p_SuperObjPerso, AI_tdstNodeInterpret *p_stTree, AI_tdstGetSetParam *p_stValue)
{
    unsigned long        eFuncId = AI_M_eFuncIdInterpret(p_stTree - 1);
    MTH3D_tdstVector     stNormalVector;
    DNM_tdstReport      *p_stDynamReport;
    DNM_tdstObstacle    *p_stObstacle = NULL;
    HIE_tdstSuperObject *hNewSuperObjPerso;

    fn_vGetUltraOperatorPerso(
        AI_M_pfn_fn_ucGetFunctionUltraOperator_fn1(eFuncId), p_SuperObjPerso, &hNewSuperObjPerso);
    MTH3D_M_vNullVector(&stNormalVector);

    p_stDynamReport = AI_M_pfn_fn_pstGetDNMReport_fn1(hNewSuperObjPerso);

    switch ( eFuncId )
    {
    case 0xbe:
        {
            long lWhichObstacle;
            M_EvalNextParameter(p_stValue);
            lWhichObstacle = M_GetSetParam_lValue(p_stValue);
            switch ( lWhichObstacle )
            {
            default:
                M_Full_GetSetParam_p_stVertex(p_stValue, &stNormalVector);
                return p_stTree;
                break;

            case 0:
                p_stObstacle = DNM_M_p_stReportGetObstacle(p_stDynamReport);
                break;

            case 1:
                p_stObstacle = DNM_M_p_stReportGetGround(p_stDynamReport);
                break;

            case 2:
                p_stObstacle = DNM_M_p_stReportGetWall(p_stDynamReport);
                break;

            case 3:
                p_stObstacle = DNM_M_p_stReportGetCharacter(p_stDynamReport);
                break;

            case 4:
                p_stObstacle = DNM_M_p_stReportGetWater(p_stDynamReport);
                break;

            case 5:
                p_stObstacle = DNM_M_p_stReportGetCeil(p_stDynamReport);
                break;
            }
            M_Full_GetSetParam_p_stVertex(p_stValue, DNM_M_p_stObstacleGetNorm(p_stObstacle));
        }
        return p_stTree;
        break;

    case 0xbd:
        {
            unsigned long ulSurfState = DNM_M_ulReportGetSurfaceState(p_stDynamReport);
            if ( (ulSurfState & AI_C_WOT_ulMobile) == AI_C_WOT_ulMobile )
            {
                p_stObstacle = DNM_M_p_stReportGetCharacter(p_stDynamReport);
                MTH3D_M_vAddVector(&stNormalVector, &stNormalVector, DNM_M_p_stObstacleGetNorm(p_stObstacle));
            }
            if ( (ulSurfState & AI_C_WOT_ulGround) == AI_C_WOT_ulGround )
            {
                p_stObstacle = DNM_M_p_stReportGetGround(p_stDynamReport);
                MTH3D_M_vAddVector(&stNormalVector, &stNormalVector, DNM_M_p_stObstacleGetNorm(p_stObstacle));
            }
            if ( (ulSurfState & AI_C_WOT_ulWall) == AI_C_WOT_ulWall )
            {
                p_stObstacle = DNM_M_p_stReportGetWall(p_stDynamReport);
                MTH3D_M_vAddVector(&stNormalVector, &stNormalVector, DNM_M_p_stObstacleGetNorm(p_stObstacle));
            }
            if ( (ulSurfState & AI_C_WOT_ulWater) == AI_C_WOT_ulWater )
            {
                p_stObstacle = DNM_M_p_stReportGetWater(p_stDynamReport);
                MTH3D_M_vAddVector(&stNormalVector, &stNormalVector, DNM_M_p_stObstacleGetNorm(p_stObstacle));
            }
            if ( (ulSurfState & AI_C_WOT_ulCeiling) == AI_C_WOT_ulCeiling )
            {
                p_stObstacle = DNM_M_p_stReportGetCeil(p_stDynamReport);
                MTH3D_M_vAddVector(&stNormalVector, &stNormalVector, DNM_M_p_stObstacleGetNorm(p_stObstacle));
            }
            if ( ulSurfState == AI_C_WOT_ulNoObstacle )
            {
                p_stObstacle = DNM_M_p_stReportGetObstacle(p_stDynamReport);
                MTH3D_M_vAddVector(&stNormalVector, &stNormalVector, DNM_M_p_stObstacleGetNorm(p_stObstacle));
            }
            if ( !MTH3D_M_bIsNullVector(&stNormalVector) )
            {
                MTH3D_M_vNormalizeVector(&stNormalVector, &stNormalVector);
            }
            M_Full_GetSetParam_p_stVertex(p_stValue, &stNormalVector);
        }
        return p_stTree;
        break;

    default:
        break;
    }
    AI_M_pfn_M_AIFatalError_fn1(0x27);
    return(p_stTree);
}
#pragma optimize("", on)

/* 0x00419B90 */
extern void *__cdecl fn_p_stDynamGetDynamics(void *h_Dynam);
#define AI_M_pfn_fn_p_stDynamGetDynamics_fn1  ((char *(__cdecl *)(void *))&fn_p_stDynamGetDynamics)
/* 0x00479220 */
extern unsigned short __cdecl fn_xMaskOfObstacle(void *_p_stObstacle);
#define AI_M_pfn_fn_xMaskOfObstacle_fn1       fn_xMaskOfObstacle
/* 0x0042DB40 */
extern unsigned char __cdecl GMT_fn_xTestCollideIdentifierTypeOfGameMaterial(GMT_tdxHandleToGameMaterial _hGameMat, unsigned short _uwType);
#define AI_M_pfn_GMT_xTestCollideIdType_fn1   ((unsigned char (__cdecl *)(void *, long))&GMT_fn_xTestCollideIdentifierTypeOfGameMaterial)

#define AI_C_xDegToRad_f1   0.017453292f

#define AI_DNM_OBS_xRate            0x00
#define AI_DNM_OBS_stContact        0x10
#define AI_DNM_OBS_hCollidedMat     0x20

#define AI_M_WriteVector3(_v, _p_xSrc)                                   \
    do {                                                                 \
        const float *_p_xS = (const float *)(_p_xSrc);                   \
        (_v)->uParam.a_xVectorValue[0] = _p_xS[0];                       \
        (_v)->uParam.a_xVectorValue[1] = _p_xS[1];                       \
        (_v)->uParam.a_xVectorValue[2] = _p_xS[2];                       \
        (_v)->eType = E_vt_Vector;                                    \
    } while (0)

/* 0x0047A150 */
AI_tdstNodeInterpret *__cdecl fn_p_GetMechanicParameter(
    HIE_tdstSuperObject *p_SuperObjPerso, AI_tdstNodeInterpret *p_stTree, AI_tdstGetSetParam *p_stValue)
{
    DNM_tdstDynamics     *pstDynamic;
    MS_tdxHandleToDynam   h_Dynam;
    HIE_tdstSuperObject  *hNewSuperObjPerso;
    long                  eFuncId = AI_M_eFuncIdInterpret(p_stTree - 1);
    MTH_tdxReal           xNulValue = MTH_C_ZERO;

    fn_vGetUltraOperatorPerso(fn_ucGetFunctionUltraOperator(eFuncId), p_SuperObjPerso, &hNewSuperObjPerso);

    h_Dynam = M_GetMSHandle(hNewSuperObjPerso,Dynam);
    if (!h_Dynam)
    {
        M_Full_GetSetParam_Float ( p_stValue, xNulValue );
        return (p_stTree);
    }

    pstDynamic = fn_p_stDynamGetDynamics(h_Dynam);

    switch (eFuncId)
    {
    case 0xdd :
        {
            MTH_tdxReal xGravity = DNM_M_xDynamicsGetGravityFactor (pstDynamic);
            M_Full_GetSetParam_Float ( p_stValue, xGravity );
        }
        return (p_stTree);
        break;

    case 0xde :
        {
            MTH_tdxReal xSlide = DNM_M_xDynamicsGetSlide (pstDynamic);
            M_Full_GetSetParam_Float ( p_stValue, xSlide );
        }
        return (p_stTree);
        break;

    case 0xdf :
        {
            MTH_tdxReal xRebound = DNM_M_xDynamicsGetRebound (pstDynamic);
            M_Full_GetSetParam_Float ( p_stValue, xRebound );
        }
        return (p_stTree);
        break;

    case 0xe0 :
        {
            MTH_tdxReal xSlopeLimit = MTH_M_xTan (MTH_M_xDegToRad (DNM_M_xDynamicsGetSlopeLimit (pstDynamic)));
            M_Full_GetSetParam_Float ( p_stValue, xSlopeLimit );
        }
        return (p_stTree);
        break;

    case 0xe1 :
        {
            MTH_tdxReal xInertiaX = DNM_M_xDynamicsGetInertiaFactorX (pstDynamic);
            M_Full_GetSetParam_Float ( p_stValue, xInertiaX );
        }
        return (p_stTree);
        break;

    case 0xe2 :
        {
            MTH_tdxReal xInertiaY = DNM_M_xDynamicsGetInertiaFactorY (pstDynamic);
            M_Full_GetSetParam_Float ( p_stValue, xInertiaY );
        }
        return (p_stTree);
        break;

    case 0xe3 :
        {
            MTH_tdxReal xInertiaZ = DNM_M_xDynamicsGetInertiaFactorZ (pstDynamic);
            M_Full_GetSetParam_Float ( p_stValue, xInertiaZ );
        }
        return (p_stTree);
        break;

    case 0xe4 :
        {
            MTH_tdxReal xTiltIntensity = DNM_M_xDynamicsGetTiltIntensity (pstDynamic);
            M_Full_GetSetParam_Float ( p_stValue, xTiltIntensity );
        }
        return (p_stTree);
        break;

    case 0xe5 :
        {
            MTH_tdxReal xTiltInertia = DNM_M_xDynamicsGetTiltInertia (pstDynamic);
            M_Full_GetSetParam_Float ( p_stValue, xTiltInertia );
        }
        return (p_stTree);
        break;

    case 0xe6 :
        {
            MTH_tdxReal xTiltOrigin = DNM_M_xDynamicsGetTiltOrigin (pstDynamic);
            M_Full_GetSetParam_Float ( p_stValue, xTiltOrigin );
        }
        return (p_stTree);
        break;

    case 0xe7 :
        {
            MTH3D_tdstVector stSpeedLimit;
            MTH3D_M_vCopyVector ( &stSpeedLimit, DNM_M_pDynamicsGetMaxSpeed (pstDynamic) );
            M_Full_GetSetParam_p_stVertex (p_stValue,&stSpeedLimit);
        }
        return (p_stTree);
        break;

    case 0xe8 :
        {
            MTH_tdxReal xStreamPriority = DNM_M_xDynamicsGetStreamPriority (pstDynamic);
            M_Full_GetSetParam_Float ( p_stValue, xStreamPriority );
        }
        return (p_stTree);
        break;

    case 0xe9 :
        {
            MTH3D_tdstVector stStreamSpeed;
            MTH3D_M_vCopyVector ( &stStreamSpeed, DNM_M_pDynamicsGetStreamSpeed (pstDynamic) );
            M_Full_GetSetParam_p_stVertex (p_stValue,&stStreamSpeed);
        }
        return (p_stTree);
        break;

    case 0xea :
        {
            MTH_tdxReal xStreamFactor = DNM_M_xDynamicsGetStreamFactor (pstDynamic);
            M_Full_GetSetParam_Float ( p_stValue, xStreamFactor );
        }
        return (p_stTree);
        break;

    case 0xeb :
        {
            MTH_tdxReal xSlideFactorX = DNM_M_xDynamicsGetSlideFactorX (pstDynamic);
            M_Full_GetSetParam_Float ( p_stValue, xSlideFactorX );
        }
        return (p_stTree);
        break;

    case 0xec :
        {
            MTH_tdxReal xSlideFactorY = DNM_M_xDynamicsGetSlideFactorY (pstDynamic);
            M_Full_GetSetParam_Float ( p_stValue, xSlideFactorY );
        }
        return (p_stTree);
        break;

    case 0xed :
        {
            MTH_tdxReal xSlideFactorZ = DNM_M_xDynamicsGetSlideFactorZ (pstDynamic);
            M_Full_GetSetParam_Float ( p_stValue, xSlideFactorZ );
        }
        return (p_stTree);
        break;

    case 0xef :
        M_Full_GetSetParam_p_stVertex (p_stValue,DNM_M_p_stDynamicsGetSpeedAnim (pstDynamic));
        return (p_stTree);
        break;

    default:
        break;
    }

    M_AIFatalError(E_uwAIFatalNotValidFunction);
    return(p_stTree);
}

/* 0x00479260 */
AI_tdstNodeInterpret *__cdecl fn_p_stGetCollidePoint(
    HIE_tdstSuperObject *p_SuperObjPerso, AI_tdstNodeInterpret *p_stTree, AI_tdstGetSetParam *p_stValue)
{
    long                  eFuncId = AI_M_eFuncIdInterpret(p_stTree - 1);
    DNM_tdstReport       *p_stDynamReport = NULL;
    DNM_tdstObstacle     *p_stObstacle = NULL;
    GMT_tdxMask           xMask = (GMT_tdxMask) 0;

    HIE_tdstSuperObject  *hNewSuperObjPerso;
    fn_vGetUltraOperatorPerso(fn_ucGetFunctionUltraOperator(eFuncId), p_SuperObjPerso, &hNewSuperObjPerso);

    p_stDynamReport = fn_pstGetDNMReport(hNewSuperObjPerso);

    switch ( eFuncId )
    {
    case 0xc4:
        {
            unsigned long ulSurfState = DNM_M_ulReportGetSurfaceState(p_stDynamReport);
            if ( (ulSurfState & C_WOT_ulMobile) == C_WOT_ulMobile )
            {
                p_stObstacle = DNM_M_p_stReportGetCharacter(p_stDynamReport);
                xMask |= fn_xMaskOfObstacle(p_stObstacle);
            }
            if ( (ulSurfState & C_WOT_ulGround) == C_WOT_ulGround )
            {
                p_stObstacle = DNM_M_p_stReportGetGround(p_stDynamReport);
                xMask |= fn_xMaskOfObstacle(p_stObstacle);
#ifndef FUDGEFUDGE

#endif
            }
            if ( (ulSurfState & C_WOT_ulWall) == C_WOT_ulWall )
            {
                p_stObstacle = DNM_M_p_stReportGetWall(p_stDynamReport);
                xMask |= fn_xMaskOfObstacle(p_stObstacle);
            }
            if ( (ulSurfState & C_WOT_ulWater) == C_WOT_ulWater )
            {
                p_stObstacle = DNM_M_p_stReportGetWater(p_stDynamReport);
                xMask |= fn_xMaskOfObstacle(p_stObstacle);
            }
            if ( (ulSurfState & C_WOT_ulCeiling) == C_WOT_ulCeiling )
            {
                p_stObstacle = DNM_M_p_stReportGetCeil(p_stDynamReport);
                xMask |= fn_xMaskOfObstacle(p_stObstacle);
            }
            if ( ulSurfState == C_WOT_ulNoObstacle )
            {
                p_stObstacle = DNM_M_p_stReportGetObstacle(p_stDynamReport);
                xMask |= fn_xMaskOfObstacle(p_stObstacle);
            }
        }
        M_Full_GetSetParam_Mask(p_stValue, xMask);
        return p_stTree;
        break;

    case 0xc5:
        {
            long lWhichObstacle;
            M_EvalNextParameter(p_stValue);
            lWhichObstacle = M_GetSetParam_lValue(p_stValue);
            switch ( lWhichObstacle )
            {
            default:
                M_Full_GetSetParam_Mask(p_stValue, (GMT_tdxMask) 0);
                return p_stTree;
                break;

            case 0:
                p_stObstacle = DNM_M_p_stReportGetObstacle(p_stDynamReport);
                break;

            case 1:
                p_stObstacle = DNM_M_p_stReportGetGround(p_stDynamReport);
                break;

            case 2:
                p_stObstacle = DNM_M_p_stReportGetWall(p_stDynamReport);
                break;

            case 3:
                p_stObstacle = DNM_M_p_stReportGetCharacter(p_stDynamReport);
                break;

            case 4:
                p_stObstacle = DNM_M_p_stReportGetWater(p_stDynamReport);
                break;

            case 5:
                p_stObstacle = DNM_M_p_stReportGetCeil(p_stDynamReport);
                break;
            }
            M_Full_GetSetParam_Mask(p_stValue, fn_xMaskOfObstacle(p_stObstacle));
        }
        return p_stTree;
        break;

    case 0xc0:
        {
            MTH3D_tdstVector stNullVector = { MTH_C_ZERO, MTH_C_ZERO, MTH_C_ZERO };
            long lWhichObstacle;
            M_EvalNextParameter(p_stValue);
            lWhichObstacle = M_GetSetParam_lValue(p_stValue);
            switch ( lWhichObstacle )
            {
            default:
                M_Full_GetSetParam_p_stVertex(p_stValue, &stNullVector);
                return p_stTree;
                break;

            case 0:
                p_stObstacle = DNM_M_p_stReportGetObstacle(p_stDynamReport);
                break;

            case 1:
                p_stObstacle = DNM_M_p_stReportGetGround(p_stDynamReport);
                break;

            case 2:
                p_stObstacle = DNM_M_p_stReportGetWall(p_stDynamReport);
                break;

            case 3:
                p_stObstacle = DNM_M_p_stReportGetCharacter(p_stDynamReport);
                break;

            case 4:
                p_stObstacle = DNM_M_p_stReportGetWater(p_stDynamReport);
                break;

            case 5:
                p_stObstacle = DNM_M_p_stReportGetCeil(p_stDynamReport);
                break;
            }
            M_Full_GetSetParam_p_stVertex(p_stValue, DNM_M_p_stObstacleGetContact(p_stObstacle));
        }
        return p_stTree;
        break;

    case 0xc1:
        {
            MTH3D_tdstVector *p_stCollidePoint = &M_GetSetParam_stVertexValue(p_stValue);
            DNM_tdstObstacle *p_stObstacle = DNM_M_p_stReportGetGround(p_stDynamReport);

            M_GetSetParam_Type(p_stValue) = E_vt_Vector;

            if ( GMT_fn_xTestCollideIdentifierTypeOfGameMaterial(DNM_M_hObstacleGetCollidedMaterial(p_stObstacle),0x0002) )
            {
                *p_stCollidePoint = *DNM_M_p_stObstacleGetContact(p_stObstacle);
            }
            else
            {
                MTH3D_M_vNullVector(p_stCollidePoint);
            }
        }
        return (p_stTree);
        break;

    case 0xbf:
        {
            DNM_tdstObstacle *p_stObstacle = DNM_M_p_stReportGetObstacle(p_stDynamReport);
            M_Full_GetSetParam_p_stVertex(p_stValue, DNM_M_p_stObstacleGetContact(p_stObstacle));
        }
        return (p_stTree);
        break;

    case 0xc3:
        {
            long lWhichObstacle;
            M_EvalNextParameter(p_stValue);
            lWhichObstacle = M_GetSetParam_lValue(p_stValue);
            switch ( lWhichObstacle )
            {
            default:
                M_Full_GetSetParam_Float(p_stValue, MTH_C_ZERO);
                return p_stTree;
                break;

            case 0:
                p_stObstacle = DNM_M_p_stReportGetObstacle(p_stDynamReport);
                break;

            case 1:
                p_stObstacle = DNM_M_p_stReportGetGround(p_stDynamReport);
                break;

            case 2:
                p_stObstacle = DNM_M_p_stReportGetWall(p_stDynamReport);
                break;

            case 3:
                p_stObstacle = DNM_M_p_stReportGetCharacter(p_stDynamReport);
                break;

            case 4:
                p_stObstacle = DNM_M_p_stReportGetWater(p_stDynamReport);
                break;

            case 5:
                p_stObstacle = DNM_M_p_stReportGetCeil(p_stDynamReport);
                break;
            }
            M_Full_GetSetParam_Float(p_stValue,DNM_M_xObstacleGetRate(p_stObstacle));
        }
        return p_stTree;
        break;

#ifndef _AI_EXCLUDE_NEVER_USED_
    case 0xc2:
        {
            DNM_tdstObstacle *p_stObstacle = DNM_M_p_stReportGetObstacle(p_stDynamReport);
            M_Full_GetSetParam_Float(p_stValue, MTH_C_ONE);
            M_Full_GetSetParam_Float(p_stValue,DNM_M_xObstacleGetRate(p_stObstacle));
        }
        return (p_stTree);
        break;
#endif

    default:
        break;
    }

    M_AIFatalError(E_uwAIFatalNotValidFunction);
    return(p_stTree);
}

/* 0x00475610 */
AI_tdstNodeInterpret *__cdecl fn_p_stCode4VitesseHorizontaleDuPerso(
    HIE_tdstSuperObject *p_SuperObjPerso, AI_tdstNodeInterpret *p_stTree, AI_tdstGetSetParam *p_stValue)
{
    unsigned long        eFuncId = AI_M_eFuncIdInterpret(p_stTree - 1);
    HIE_tdstSuperObject *hNewSuperObjPerso;
    char                *p_cReport;
    MTH_tdxReal          xValue = 0;

    fn_vGetUltraOperatorPerso(
        AI_M_pfn_fn_ucGetFunctionUltraOperator_fn1(eFuncId), p_SuperObjPerso, &hNewSuperObjPerso);

    p_cReport = AI_M_pfn_fn_pstGetDNMReport_fn1(hNewSuperObjPerso);
    if (p_cReport == (char *)0)
        return p_stTree;

    switch (eFuncId)
    {
    case 0xa0:
        M_Full_GetSetParam_Float(p_stValue, xValue);
        return p_stTree;

    case 0xa1:
        M_Full_GetSetParam_Float(p_stValue, xValue);
        return p_stTree;

    default:
        AI_M_pfn_M_AIFatalError_fn1(0x27);
        return p_stTree;
    }
}

#define HIE_C_InvalidSuperObject ((HIE_tdstSuperObject *)0)
#define HIE_fn_bIsSuperObjectValid(hSuperObject) ((hSuperObject)!=HIE_C_InvalidSuperObject)

/* 0x0047A3E0 */
AI_tdstNodeInterpret *__cdecl fn_p_GetHierarchyLink(
    HIE_tdstSuperObject *p_SuperObjPerso, AI_tdstNodeInterpret *p_stTree, AI_tdstGetSetParam *p_stValue)
{
    DNM_tdstDynamics     *pstDynamic;
    MS_tdxHandleToDynam   h_Dynam;
    HIE_tdstSuperObject  *hNewSuperObjPerso;
    HIE_tdstSuperObject  *hFather;
    long                  eFuncId = AI_M_eFuncIdInterpret(p_stTree - 1);

    fn_vGetUltraOperatorPerso(fn_ucGetFunctionUltraOperator(eFuncId), p_SuperObjPerso, &hNewSuperObjPerso);

    h_Dynam = M_GetMSHandle(hNewSuperObjPerso,Dynam);

    if (!h_Dynam)
    {
        return (p_stTree);
    }

    pstDynamic = fn_p_stDynamGetDynamics(h_Dynam);

    switch (eFuncId)
    {
    case 0xf0 :

        hFather = hNewSuperObjPerso;
        do
        {
            hFather = HIE_fn_hGetSuperObjectFather (hFather);
        }
        while (HIE_fn_bIsSuperObjectValid (hFather) && HIE_fn_ulGetSuperObjectType(hFather)!=HIE_C_ulActor);

        M_Full_GetSetParam_Perso(p_stValue,hFather);
        return (p_stTree);
        break;

    default:
        break;
    }

    M_AIFatalError(E_uwAIFatalNotValidFunction);
    return(p_stTree);
}

#define AI_CJI_M_Full_GetSetParam_p_stVertex(p_stValue, p_stVect)                        \
    do { MTH3D_M_vCopyVector((MTH3D_tdstVector *)(p_stValue)->uParam.a_xVectorValue, (p_stVect)); \
         (p_stValue)->eType = E_vt_Vector; } while (0)
/* 0x0047A660 */
AI_tdstNodeInterpret *__cdecl fn_p_ComputeJumpImpulsion(
    HIE_tdstSuperObject *p_SuperObjPerso, AI_tdstNodeInterpret *p_stTree, AI_tdstGetSetParam *p_stValue)
{
    long                  eFuncId = *(long *)((char *)p_stTree - 8);
    HIE_tdstSuperObject  *hPerso;
    AI_tdstGetSetParam    stParam;
    MTH3D_tdstVector      stStartPosition;
    MTH3D_tdstVector      stEndPosition;
    MTH3D_tdstVector      stImpulsion;
    MTH3D_tdstVector      stNullVector;
    MTH_tdxReal           xDeltaZ;
    MTH_tdxReal           xZmax;
    MTH_tdxReal           xG;
    MTH_tdxReal           xDelta;
    MTH_tdxReal           xt;
    MTH_tdxReal           xa, xb, xc;
    DNM_tdstDynamics     *p_stDynamic;
    void                 *h_Dynam;

    fn_vGetUltraOperatorPerso(
        AI_M_pfn_fn_ucGetFunctionUltraOperator_fn1(eFuncId), p_SuperObjPerso, &hPerso);

    MTH3D_M_vNullVector(&stNullVector);

    p_stTree = fn_p_stEvalTree(p_SuperObjPerso, p_stTree, &stParam);
    stStartPosition = *(MTH3D_tdstVector *)stParam.uParam.a_xVectorValue;

    p_stTree = fn_p_stEvalTree(p_SuperObjPerso, p_stTree, &stParam);
    stEndPosition = *(MTH3D_tdstVector *)stParam.uParam.a_xVectorValue;

    p_stTree = fn_p_stEvalTree(p_SuperObjPerso, p_stTree, &stParam);
    xDeltaZ = M_ReturnParam_xValue(&stParam);

    if (MTH_M_bLess(xDeltaZ, MTH_C_ZERO))
    {
        AI_CJI_M_Full_GetSetParam_p_stVertex(p_stValue, &stNullVector);
        return (p_stTree);
    }

    xZmax = MTH_M_xAdd(xDeltaZ, MTH3D_M_xGetZofVector(&stEndPosition));
    if (MTH_M_bLess(xZmax, MTH3D_M_xGetZofVector(&stStartPosition)))
        xZmax = MTH_M_xAdd(xDeltaZ, MTH3D_M_xGetZofVector(&stStartPosition));

    MTH3D_M_vNullVector(&stImpulsion);
    MTH3D_M_vNullVector(&stNullVector);
    AI_CJI_M_Full_GetSetParam_p_stVertex(p_stValue, &stImpulsion);

    h_Dynam = ((struct tdstEngineObject_ *)(HIE_fn_hGetSuperObjectObject(hPerso)))->h_Dynam;
    if (!h_Dynam)
    {
        AI_CJI_M_Full_GetSetParam_p_stVertex(p_stValue, &stNullVector);
        return (p_stTree);
    }
    p_stDynamic = (DNM_tdstDynamics *)AI_M_pfn_fn_p_stDynamGetDynamics_fn1(h_Dynam);
    if (!p_stDynamic)
    {
        AI_CJI_M_Full_GetSetParam_p_stVertex(p_stValue, &stNullVector);
        return (p_stTree);
    }

    xG = DNM_M_xDynamicsGetGravityFactor(p_stDynamic);
    if (MTH_M_bIsNull(xG))
    {
        AI_CJI_M_Full_GetSetParam_p_stVertex(p_stValue, &stNullVector);
        return (p_stTree);
    }

    MTH3D_M_vSetZofVector(&stImpulsion, MTH_M_xSqrt(MTH_M_xMul(MTH_M_xMul(MTH_M_xSub(xZmax, MTH3D_M_xGetZofVector(&stStartPosition)), MTH_C_2), xG)));

    xa = MTH_M_xMul(xG, MTH_C_MinusInv2);
    xb = MTH3D_M_xGetZofVector(&stImpulsion);
    xc = MTH_M_xSub(MTH3D_M_xGetZofVector(&stStartPosition), MTH3D_M_xGetZofVector(&stEndPosition));
    xDelta = MTH_M_xTrinomeDelta(xa, xb, xc);
    if (MTH_M_bLess(xDelta, MTH_C_ZERO))
    {
        AI_CJI_M_Full_GetSetParam_p_stVertex(p_stValue, &stNullVector);
        return (p_stTree);
    }

    xt = MTH_M_xDiv(MTH_M_xSub(MTH_M_xNeg(xb), MTH_M_xSqrt(xDelta)), MTH_M_xMul(MTH_C_2, xa));

    MTH3D_M_vSetXofVector(&stImpulsion, MTH_M_xDiv(MTH_M_xSub(MTH3D_M_xGetXofVector(&stEndPosition), MTH3D_M_xGetXofVector(&stStartPosition)), xt));

    MTH3D_M_vSetYofVector(&stImpulsion, MTH_M_xDiv(MTH_M_xSub(MTH3D_M_xGetYofVector(&stEndPosition), MTH3D_M_xGetYofVector(&stStartPosition)), xt));

    AI_CJI_M_Full_GetSetParam_p_stVertex(p_stValue, &stImpulsion);
    return (p_stTree);
}

unsigned char __cdecl fn_ucGetDsgVar(unsigned char ucId, unsigned char ucIndex, void *p_stMind, void *p_stGetSetParam);
#define AI_GBA_C_VALID_GET  1

/* 0x0047A8E0 */
AI_tdstNodeInterpret *__cdecl fn_p_GetBooleanInArray(
    HIE_tdstSuperObject *p_SuperObjPerso, AI_tdstNodeInterpret *p_stTree, AI_tdstGetSetParam *p_stValue)
{
    long                        eFuncId = AI_M_eFuncIdInterpret(p_stTree-1);
    AI_tdstMind *               p_stMind;
    unsigned char               ucArrayId;
    unsigned long               ulFirstIndex;
    unsigned long               ulIndexFirstLong;
    unsigned long               ulIndexFirstBit;

    M_vEvalNextVarId(p_stMind, ucArrayId);

    M_EvalNextParameter(p_stValue);
    ulFirstIndex = M_GetSetParam_lValue(p_stValue);

    ulFirstIndex--;

    ulIndexFirstLong = (ulFirstIndex >> 5);
    ulIndexFirstBit	 = (ulFirstIndex & 31);

    M_Full_GetSetParam_Integer(p_stValue, 0);

    switch (eFuncId)
    {
    case 0xf8:
        {
            if (AI_M_pfn_fn_ucGetDsgVar(ucArrayId, (unsigned char)ulIndexFirstLong, p_stMind, p_stValue)
                == AI_GBA_C_VALID_GET)
            {
                unsigned long ulValue = p_stValue->uParam.lValue;

                M_Full_GetSetParam_Integer(p_stValue, (ulValue & (1 << ulIndexFirstBit)) ? 1 : 0);
            }
            break;
        }
    case 0xf9:
        {
            unsigned long ulLastIndex;
            unsigned long ulIndexLastLong;
            unsigned long ulIndexLastBit;
            unsigned long ulNumberOfBits;
            unsigned long ulIndexLong;

            p_stTree = fn_p_stEvalTree(p_SuperObjPerso, p_stTree, p_stValue);
            ulLastIndex = p_stValue->uParam.lValue;
            ulLastIndex--;

            ulIndexLastLong = (ulLastIndex >> 5);
            ulIndexLastBit  = (ulLastIndex & 31);

            ulNumberOfBits = 0;
            for (ulIndexLong = ulIndexFirstLong; ulIndexLong <= ulIndexLastLong; ulIndexLong++)
            {
                if (AI_M_pfn_fn_ucGetDsgVar(ucArrayId, (unsigned char)ulIndexLong, p_stMind, p_stValue)
                    == AI_GBA_C_VALID_GET)
                {
                    unsigned long ulValue     = p_stValue->uParam.lValue;
                    unsigned long ulFirstBit  = (ulIndexLong == ulIndexFirstLong ? ulIndexFirstBit : 0);
                    unsigned long ulLastBit   = (ulIndexLong == ulIndexLastLong  ? ulIndexLastBit  : 31);
                    unsigned long ulIndexBit;

                    for (ulIndexBit = ulFirstBit; ulIndexBit <= ulLastBit; ulIndexBit++)
                    {
                        if (ulValue & (1 << ulIndexBit))      ulNumberOfBits++;
                    }
                }
            }
            M_Full_GetSetParam_Integer(p_stValue, ulNumberOfBits);
            break;
        }

    default:

        AI_M_pfn_M_AIFatalError_fn1(0x27);
        return (p_stTree);
        break;
    }
    return (p_stTree);
}

/* 0x00478A20 */
AI_tdstNodeInterpret *__cdecl fn_p_stGetPersoSighting(
    HIE_tdstSuperObject *p_SuperObjPerso, AI_tdstNodeInterpret *p_stTree, AI_tdstGetSetParam *p_stValue)
{
    unsigned long        eFuncId = AI_M_eFuncIdInterpret(p_stTree - 1);
    HIE_tdstSuperObject *hNewSuperObjPerso;
    MTH3D_tdstVector    *p_stTmpV0, *p_stTmpV1, *p_stTmpV2;

    fn_vGetUltraOperatorPerso(
        AI_M_pfn_fn_ucGetFunctionUltraOperator_fn1(eFuncId), p_SuperObjPerso, &hNewSuperObjPerso);

    POS_fn_vGetRotationVector((POS_tdstCompletePosition *)hNewSuperObjPerso->hGlobalMatrix,
                              &p_stTmpV0, &p_stTmpV1, &p_stTmpV2);

    {   long register NegMask = 0x80000000;
        *((long *)&p_stValue->uParam.a_xVectorValue[0]) = *((long *)&p_stTmpV1->xX) ^ NegMask;
        *((long *)&p_stValue->uParam.a_xVectorValue[1]) = *((long *)&p_stTmpV1->xY) ^ NegMask;
        *((long *)&p_stValue->uParam.a_xVectorValue[2]) = *((long *)&p_stTmpV1->xZ) ^ NegMask;
    }
    p_stValue->eType = 0x15;
    return p_stTree;
}

/* 0x00480950 */
static __inline HIE_tdstSuperObject *fn_hGetSuperObjectInChannel(CHN_tdxHandleToChannelArray hChannelArray, unsigned short uwChannelNumber)
{
	return (hChannelArray) ? hChannelArray[uwChannelNumber].hSupObject : NULL;
}

/* 0x00478AA0 */
AI_tdstNodeInterpret *__cdecl fn_p_st3DFunc(
    HIE_tdstSuperObject *p_SuperObjPerso, AI_tdstNodeInterpret *p_stTree, AI_tdstGetSetParam *p_stValue)
{
  long eFuncId = AI_M_eFuncIdInterpret(p_stTree-1);
  unsigned char ucChannel;
  long lGlobalAxis = 1;
  MTH3D_tdstVector stDstVector;
  POS_tdstCompletePosition *hObjectMatrix;

  HIE_tdstSuperObject *hNewSuperObjPerso;
  fn_vGetUltraOperatorPerso(fn_ucGetFunctionUltraOperator(eFuncId),p_SuperObjPerso,&hNewSuperObjPerso);

  MTH3D_M_vNullVector(&stDstVector);

	switch ( eFuncId )
	{
	case 0xa2:

		lGlobalAxis = 0;
		break;

	case 0xd3:
	case 0xd4:

		lGlobalAxis = 0;

	case 0xd2:
	case 0xd5:
		{
		  HIE_tdstSuperObject *hChannelSuperObject;

			M_EvalNextParameter(p_stValue);
			ucChannel = M_GetSetParam_lValue(p_stValue);

			if ( ucChannel == C_ucUnknownChannel )
			{
				M_Full_GetSetParam_p_stVertex(p_stValue,&stDstVector);
				return fn_p_stSwapNextParameter(p_stTree);
				break;
			}

			hChannelSuperObject = fn_hGetSuperObjectInChannel(fn_h3dDataGetChannelSOList(M_GetMSHandle(hNewSuperObjPerso,3dData)), ucChannel);

			if ( hChannelSuperObject )
			{

				hNewSuperObjPerso = hChannelSuperObject;
			}

			if ( eFuncId == 0xd5 )
			{
				M_EvalNextParameter(p_stValue);
				lGlobalAxis = M_GetSetParam_lValue(p_stValue);
			}

		}

	case 0xa5:
	case 0xa4:

		break;

	default:
		M_AIFatalError(E_uwAIFatalNotValidFunction);
		return p_stTree;
		break;
	}

  hObjectMatrix = lGlobalAxis ? HIE_fn_hGetSuperObjectGlobalMatrix(hNewSuperObjPerso) : HIE_fn_hGetSuperObjectMatrix(hNewSuperObjPerso);

  switch ( eFuncId )
  {
	case 0xa5:
		{
			MTH3D_tdstVector stV1, stV2;

            DNM_tdstDynamics *      p_stDynamic;
            MS_tdxHandleToDynam     h_Dynam;

			h_Dynam = M_GetMSHandle (hNewSuperObjPerso,Dynam);
            if ( h_Dynam != 0 )
            {
            	p_stDynamic = fn_p_stDynamGetDynamics (h_Dynam);
                if (( p_stDynamic != 0 ) && DNM_M_bDynamicsIsTilt (p_stDynamic))
                {
                    MTH3D_M_vSetVectorElements (& stDstVector , MTH_C_ZERO , MTH_C_ZERO , MTH_C_ONE);
                    break;
                }
            }

			POS_fn_vGetRotationMatrix(hObjectMatrix, &stV1, &stV2, &stDstVector);
		}
		break;

	case 0xa4:
		{
			MTH3D_tdstVector stV2, stV3;

			POS_fn_vGetRotationMatrix(hObjectMatrix, &stDstVector, &stV2, &stV3);
		}
		break;

	case 0xd5:
		{
			MTH3D_tdstVector stV1, stV3;
			POS_fn_vGetRotationMatrix(hObjectMatrix, &stV1, &stDstVector, &stV3);

			MTH3D_M_vNegVector(&stDstVector, &stDstVector);
		}
		break;

	case 0xd4:
	case 0xa2:
		{
			MTH3D_tdstVector stV1, stV2, stV3;

			POS_fn_vGetScaleMatrix(hObjectMatrix, &stV1, &stV2, &stV3);

			MTH3D_M_vSetVectorElements(&stDstVector, MTH3D_M_xNormVector(&stV1), MTH3D_M_xNormVector(&stV2), MTH3D_M_xNormVector(&stV3));
		}
		break;

  case 0xd3:
  case 0xd2:

    POS_fn_vGetTranslationVector(hObjectMatrix, &stDstVector);
		break;
	default:
break;
  }

  M_Full_GetSetParam_p_stVertex(p_stValue, &stDstVector);
  return p_stTree;
}

extern char *__cdecl FUN_0042E540(long lHandle);
#define AI_M_pfn_pc_szResolveButtonName  FUN_0042E540

/* 0x0047AA70 */
AI_tdstNodeInterpret *__cdecl fn_p_stGetButtonName(
    HIE_tdstSuperObject *p_SuperObjPerso, AI_tdstNodeInterpret *p_stTree, AI_tdstGetSetParam *p_stValue)
{
    AI_tdstGetSetParam    stParam;
    AI_tdstNodeInterpret *p_stNext;

    p_stNext = fn_p_stEvalTree(p_SuperObjPerso, p_stTree, &stParam);
    M_Full_GetSetParam_String(p_stValue,
                                 AI_M_pfn_pc_szResolveButtonName(stParam.uParam.lValue));
    return p_stNext;
}

/* 0x004221F0 */
extern long __cdecl pc_GLI_lGetNbResolutions(void);
#define AI_M_pfn_pc_GLI_lGetNbResolutions    pc_GLI_lGetNbResolutions
extern long __cdecl pc_GLI_lGetCurrentResolution(void);
extern long __cdecl pc_GLI_bGetResolutionName(long lMode, char *szName);
#define AI_M_pfn_pc_GLI_lGetCurrentResolution pc_GLI_lGetCurrentResolution
#define AI_M_pfn_pc_GLI_vGetResolutionName   pc_GLI_bGetResolutionName
extern long __cdecl fn_lGetNbAvailableSlots(void);
#define AI_M_pfn_fn_lGetNbAvailableSlots     fn_lGetNbAvailableSlots

char AI_pc_szResolutionNameBuf[60];

/* 0x0047AC30 */
AI_tdstNodeInterpret *__cdecl fn_p_stGetNbAvailableResolution(
    HIE_tdstSuperObject *p_SuperObjPerso, AI_tdstNodeInterpret *p_stTree, AI_tdstGetSetParam *p_stValue)
{
    (void)p_SuperObjPerso;
    p_stValue->uParam.lValue = AI_M_pfn_pc_GLI_lGetNbResolutions();
    p_stValue->eType = 6;
    return p_stTree;
}

/* 0x0047AC50 */
AI_tdstNodeInterpret *__cdecl fn_p_stGetCurrentResolution(
    HIE_tdstSuperObject *p_SuperObjPerso, AI_tdstNodeInterpret *p_stTree, AI_tdstGetSetParam *p_stValue)
{
    (void)p_SuperObjPerso;
    p_stValue->uParam.lValue = AI_M_pfn_pc_GLI_lGetCurrentResolution();
    p_stValue->eType = 6;
    return p_stTree;
}

/* 0x0047ACD0 */
AI_tdstNodeInterpret *__cdecl fn_p_stGetNameResolution(
    HIE_tdstSuperObject *p_SuperObjPerso, AI_tdstNodeInterpret *p_stTree, AI_tdstGetSetParam *p_stValue)
{
    AI_tdstNodeInterpret *p_stNext = fn_p_stEvalTree(p_SuperObjPerso, p_stTree, p_stValue);
    AI_M_pfn_pc_GLI_vGetResolutionName(p_stValue->uParam.lValue, AI_pc_szResolutionNameBuf);
    M_Full_GetSetParam_String(p_stValue, AI_pc_szResolutionNameBuf);
    return p_stNext;
}

/* 0x0047AD30 */
AI_tdstNodeInterpret *__cdecl fn_p_stGetNbSlotsAvailable(
    HIE_tdstSuperObject *p_SuperObjPerso, AI_tdstNodeInterpret *p_stTree, AI_tdstGetSetParam *p_stValue)
{
    AI_tdstNodeInterpret *p_stNext = fn_p_stEvalTree(p_SuperObjPerso, p_stTree, p_stValue);
    p_stValue->uParam.lValue = AI_M_pfn_fn_lGetNbAvailableSlots();
    p_stValue->eType = 6;
    return p_stNext;
}

/* 0x0047AD60 */
AI_tdstNodeInterpret *__cdecl fn_p_stGetVideoOptions(
    HIE_tdstSuperObject *p_SuperObjPerso, AI_tdstNodeInterpret *p_stTree, AI_tdstGetSetParam *p_stValue)
{
    (void)p_SuperObjPerso;
    p_stValue->uParam.lValue = 1;
    p_stValue->eType = 6;
    return p_stTree;
}

/* 0x0047AD10 */
AI_tdstNodeInterpret *__cdecl fn_p_stGetBrightness(
    HIE_tdstSuperObject *p_SuperObjPerso, AI_tdstNodeInterpret *p_stTree, AI_tdstGetSetParam *p_stValue)
{
    (void)p_SuperObjPerso;
    p_stValue->uParam.lValue = GLI_fn_lGetBrightness();
    p_stValue->eType = 6;
    return p_stTree;
}

/* 0x0047AC70 */
extern long __cdecl pc_GLI_lSaveVideoModeToConfig(void);
#define AI_M_pfn_pc_GLI_lSaveVideoModeToConfig  pc_GLI_lSaveVideoModeToConfig
AI_tdstNodeInterpret *__cdecl AI_pc_fn_p_stSaveVideoMode(
    HIE_tdstSuperObject *p_SuperObjPerso, AI_tdstNodeInterpret *p_stTree, AI_tdstGetSetParam *p_stValue)
{
    (void)p_SuperObjPerso;
    p_stValue->uParam.lValue = AI_M_pfn_pc_GLI_lSaveVideoModeToConfig();
    p_stValue->eType = 6;
    return p_stTree;
}

/* 0x0047AC90 */
extern long __cdecl pc_GLI_lIsResolutionAvailable(long lSlot);
#define AI_M_pfn_pc_GLI_lIsResolutionAvailable  pc_GLI_lIsResolutionAvailable
AI_tdstNodeInterpret *__cdecl AI_pc_fn_p_stIsResolutionAvailable(
    HIE_tdstSuperObject *p_SuperObjPerso, AI_tdstNodeInterpret *p_stTree, AI_tdstGetSetParam *p_stValue)
{
    AI_tdstNodeInterpret *p_stNext = fn_p_stEvalTree(p_SuperObjPerso, p_stTree, p_stValue);
    p_stValue->uParam.lValue = AI_M_pfn_pc_GLI_lIsResolutionAvailable(p_stValue->uParam.lValue);
    p_stValue->eType = 6;
    return p_stNext;
}
