#include "AI/AI_types.h"
#include "AI/AI_globals.h"
#include "GAM/GAM_types.h"
#include "GAM/cineinfo.h"
#include "GAM/Caps.h"
#include "GMT/GMT_types.h"
#include "AI/List.h"
#include "GAM/GameEng.h"
#include "GAM/Always.h"
#include "GAM/STRUCTUR/State.h"
#include "MOR/MOR.h"
#include "IPT/IPT.h"
#include "GAM/MSMag.h"
#include "MEC/MECTools.h"

struct IPT_tdstEntryElement_;
unsigned char __cdecl IPT_fn_bIsValidated(struct IPT_tdstEntryElement_ *_hEntryElement);
unsigned char __cdecl IPT_fn_bIsJustValidated(struct IPT_tdstEntryElement_ *_hEntryElement);
unsigned char __cdecl IPT_fn_bIsInvalidated(struct IPT_tdstEntryElement_ *_hEntryElement);
unsigned char __cdecl IPT_fn_bIsJustInvalidated(struct IPT_tdstEntryElement_ *_hEntryElement);

#define eCond_Et              0
#define eCond_Ou              1
#define eCond_Not             2
#define eCond_XOr             3
#define eCond_Equal           4
#define eCond_Different       5
#define eCond_Lesser          6
#define eCond_Greater         7
#define eCond_LesserOrEqual   8
#define eCond_GreaterOrEqual  9
#define eCond_PressedBut       0x2c
#define eCond_JustPressedBut   0x2d
#define eCond_ReleasedBut      0x2e
#define eCond_JustReleasedBut  0x2f

#define eCond_IsValidObject    0x31
#define eCond_IsValidWayPoint  0x32
#define eCond_IsValidGMT       0x33
#define eCond_IsValidAction    0x34
#define eCond_IsValidText      0x35

/* 0x00480950 */
extern AI_tdstNodeInterpret *__cdecl fn_p_stSwapNextParameter(AI_tdstNodeInterpret *p_stTree);

/* 0x00489830 */
AI_tdstNodeInterpret *__cdecl fn_p_stComparisonCondition(
    HIE_tdstSuperObject *p_SuperObjPerso, AI_tdstNodeInterpret *p_stTree, long *p_lValue)
{
    AI_tdstGetSetParam stParam1, stParam2;
    unsigned long      eCondId = AI_M_eCondIdInterpret(p_stTree - 1);
    long               lIsFloat;
    MTH_tdxReal        xParam1, xParam2;

    M_EvalNextTwoParameter();

    lIsFloat = (long)M_GetSetParam_Type(&stParam1);
    if (lIsFloat == (long)M_GetSetParam_Type(&stParam2))
    {
        if (lIsFloat == E_vt_Float)
        {

            xParam1 = M_GetSetParam_xValue(&stParam1);
            xParam2 = M_GetSetParam_xValue(&stParam2);
        }
        else if (lIsFloat == E_vt_Vector)
        {
            switch (eCondId)
            {
            case eCond_Equal :
                *p_lValue = MTH3D_M_bEqualVector(&M_GetSetParam_stVectorValue(&stParam1) , &M_GetSetParam_stVectorValue(&stParam2));
                break;

            case eCond_Different :
                *p_lValue = !MTH3D_M_bEqualVector(&M_GetSetParam_stVectorValue(&stParam1) , &M_GetSetParam_stVectorValue(&stParam2));
                break;

            default:
                *p_lValue=(long) FALSE;
                break;
            }
            return(p_stTree);
        }
        else
        {

            switch (eCondId)
            {
            case eCond_Equal :
                *p_lValue = (M_GetSetParam_lValue(&stParam1) == M_GetSetParam_lValue(&stParam2));
                break;

            case eCond_Different :
                *p_lValue = (M_GetSetParam_lValue(&stParam1) != M_GetSetParam_lValue(&stParam2));
                break;

            case eCond_Lesser :
                *p_lValue = (M_GetSetParam_lValue(&stParam1) < M_GetSetParam_lValue(&stParam2));
                break;

            case eCond_Greater :
                *p_lValue = (M_GetSetParam_lValue(&stParam1) > M_GetSetParam_lValue(&stParam2));
                break;

            case eCond_LesserOrEqual :
                *p_lValue = (M_GetSetParam_lValue(&stParam1) <= M_GetSetParam_lValue(&stParam2));
                break;

            case eCond_GreaterOrEqual:
                *p_lValue = (M_GetSetParam_lValue(&stParam1) >= M_GetSetParam_lValue(&stParam2));
                break;

            default:
                *p_lValue=(long) FALSE;
                break;
            }
            return(p_stTree);
        }
    }
    else
    {

        if (lIsFloat == E_vt_Float)
        {
            xParam1 = M_ReturnParam_xValue(&stParam1);
            xParam2 = MTH_M_xLongToReal(M_GetSetParam_lValue(&stParam2));
        }
        else
        {
            xParam1 = MTH_M_xLongToReal(M_GetSetParam_lValue(&stParam1));
            xParam2 = M_ReturnParam_xValue(&stParam2);
        }
    }

    switch (eCondId)
    {
    case eCond_Equal :
        *p_lValue = MTH_M_bEqual(xParam1, xParam2);
        break;

    case eCond_Different :
        *p_lValue = MTH_M_bDifferent(xParam1, xParam2);
        break;

    case eCond_Lesser :
        *p_lValue = MTH_M_bLess(xParam1, xParam2);
        break;

    case eCond_Greater :
        *p_lValue = MTH_M_bGreater(xParam1, xParam2);
        break;

    case eCond_LesserOrEqual :
        *p_lValue = MTH_M_bLessEqual(xParam1, xParam2);
        break;

    case eCond_GreaterOrEqual:
        *p_lValue = MTH_M_bGreaterEqual(xParam1, xParam2);
        break;

    default:
        *p_lValue=(long) FALSE;
        break;
    }
    return(p_stTree);
}

/* 0x00489B40 */
AI_tdstNodeInterpret *__cdecl fn_p_stBooleanCondition(
    HIE_tdstSuperObject *p_SuperObjPerso, AI_tdstNodeInterpret *p_stTree, long *p_lValue)
{
    AI_tdstGetSetParam stValue;
    unsigned long      eCondId = AI_M_eCondIdInterpret(p_stTree - 1);

    p_stTree  = fn_p_stEvalTree(p_SuperObjPerso, p_stTree, &stValue);
    *p_lValue = M_GetSetParam_lValue(&stValue);

    switch (eCondId)
    {
    case eCond_Et:
        if (*p_lValue == FALSE)
        {

            if (AI_M_NodeToSkip(p_stTree))
            {
                p_stTree += AI_M_NodeToSkip(p_stTree);
            }
            else
            {
                AI_tdstNodeInterpret *p_stTreeInit = p_stTree;
                p_stTree = fn_p_stSwapNextParameter(p_stTree);
                AI_M_NodeToSkip(p_stTreeInit) = (unsigned short)(p_stTree - p_stTreeInit);
            }
        }
        else
        {

            p_stTree  = fn_p_stEvalTree(p_SuperObjPerso, p_stTree, &stValue);
            *p_lValue = M_GetSetParam_lValue(&stValue);
        }
        return p_stTree;

    case eCond_Ou:
        if (*p_lValue != FALSE)
        {
            if (AI_M_NodeToSkip(p_stTree))
            {
                p_stTree += AI_M_NodeToSkip(p_stTree);
            }
            else
            {
                AI_tdstNodeInterpret *p_stTreeInit = p_stTree;
                p_stTree = fn_p_stSwapNextParameter(p_stTree);
                AI_M_NodeToSkip(p_stTreeInit) = (unsigned short)(p_stTree - p_stTreeInit);
            }
        }
        else
        {
            p_stTree  = fn_p_stEvalTree(p_SuperObjPerso, p_stTree, &stValue);
            *p_lValue = M_GetSetParam_lValue(&stValue);
        }
        return p_stTree;

    case eCond_Not:
        *p_lValue = (long)((*p_lValue == FALSE) ? TRUE : FALSE);
        return p_stTree;

    case eCond_XOr:
        {
            long l1 = *p_lValue;
            p_stTree  = fn_p_stEvalTree(p_SuperObjPerso, p_stTree, &stValue);

            *p_lValue = (l1 ^ *p_lValue) ? TRUE : FALSE;
            return p_stTree;
        }

    default:
        break;
    }

    *p_lValue = (long)FALSE;
    return p_stTree;
}

/* 0x00489C80 */
AI_tdstNodeInterpret *__cdecl fn_p_stButtonCondition(
    HIE_tdstSuperObject *p_SuperObjPerso, AI_tdstNodeInterpret *p_stTree, long *p_lValue)
{
    AI_tdstGetSetParam            stValue;
    unsigned long                 eCondId = AI_M_eCondIdInterpret(p_stTree - 1);
    struct IPT_tdstEntryElement_ *hHandle;

    p_stTree = fn_p_stEvalTree(p_SuperObjPerso, p_stTree, &stValue);
    hHandle  = (struct IPT_tdstEntryElement_ *)M_GetSetParam_lValue(&stValue);

    switch (eCondId)
    {
    case eCond_PressedBut:      *p_lValue = (long)IPT_fn_bIsValidated(hHandle);       return p_stTree;
    case eCond_JustPressedBut:  *p_lValue = (long)IPT_fn_bIsJustValidated(hHandle);   return p_stTree;
    case eCond_ReleasedBut:     *p_lValue = (long)IPT_fn_bIsInvalidated(hHandle);     return p_stTree;
    case eCond_JustReleasedBut: *p_lValue = (long)IPT_fn_bIsJustInvalidated(hHandle); return p_stTree;
    default:                    break;
    }
    *p_lValue = (long)FALSE;
    return p_stTree;
}

#define AI_g_ulCurrentTimerCount GAM_g_ulAccumulatedTime  /* 0x5003EC */

unsigned char __cdecl fn_ucGetConditionUltraOperator(unsigned long eCondId);
#define AI_M_pfn_fn_ucGetConditionUltraOperator  fn_ucGetConditionUltraOperator

#define AI_M_GetEngineObject(p_stSO) \
    (((p_stSO)->ulTypeOfLinkedObject != HIE_C_ulActor) \
        ? (struct tdstEngineObject_ *)0 \
        : (struct tdstEngineObject_ *)(p_stSO)->hLinkedObject.p_Void)

#define AI_M_pfn_fn_pstGetDNMReport fn_pstGetDNMReport

ACP_tdxBool __cdecl fn_bTestTypeOfGameMaterial(GMT_tdxHandleToGameMaterial hGameMat, GMT_tdxMask xTestMask);
#define AI_M_pfn_fn_bTestTypeOfGameMaterial fn_bTestTypeOfGameMaterial

#define C_WOT_ulNoObstacle         0x00000000
#define C_WOT_ulGround             0x00000001
#define C_WOT_ulWall               0x00000004
#define C_WOT_ulCeiling            0x00000010
#define C_WOT_ulWater              0x00000040
#define C_WOT_ulMobile             0x00010000

#define eCond_ZDMCollideWithGround   0x22
#define eCond_ZDMCollideWithWall     0x23
#define eCond_ZDMCollideWithNothing  0x24
#define eCond_ZDMCollideWithCeiling  0x25

#define eCond_CollideWithGround    0x1b
#define eCond_CollideWithWall      0x1c
#define eCond_CollideWithNothing   0x1d
#define eCond_CollideWithCeiling   0x1e
#define eCond_CollideWithPerso     0x1f
#define eCond_CollideWithWater     0x20

#define AI_ZDX_CHARACTER  0
#define AI_ZDX_MODULE     1

#define eCond_CollidePersoZDDNoWithPerso    0x0a
#define eCond_CollideModuleZDDNoWithPerso   0x0b
#define eCond_CollidePersoAllZDDWithPerso   0x0c
#define eCond_CollidePersoZDDWithAnyPerso   0x0d
#define eCond_CollideModuleZDDWithAnyPerso  0x0e

#define AI_M_pfn_fn_h_StandardGameGetSuperObject fn_h_StandardGameGetSuperObject

#define AI_M_GetSetParam_p_stSupObjValue(p_stParam) \
    AI_M_pfn_fn_h_StandardGameGetSuperObject( \
        *(void **)((char *)(p_stParam)->uParam.p_stEngObjValue + 4))

/* 0x0040D7A0 */
ACP_tdxBool __cdecl fn_bIsCharacterTWODetectedByThisZddOfCharacterOne(
    HIE_tdstSuperObject *, unsigned char, unsigned char, HIE_tdstSuperObject *);
ACP_tdxBool __cdecl fn_bIsCharacterTWODetectedByCharacterOne(
    HIE_tdstSuperObject *, HIE_tdstSuperObject *);
unsigned char __cdecl fn_ucAtLeastOnePersoInZdd(void *, unsigned char, ACP_tdxIndex);
#define AI_M_pfn_fn_bIsCharacterTWODetectedByThisZddOfCharacterOne fn_bIsCharacterTWODetectedByThisZddOfCharacterOne
#define AI_M_pfn_fn_bIsCharacterTWODetectedByCharacterOne fn_bIsCharacterTWODetectedByCharacterOne
#define AI_M_pfn_fn_ucAtLeastOnePersoInZdd \
    ((unsigned char (__cdecl *)(HIE_tdstSuperObject *, unsigned char, unsigned char))fn_ucAtLeastOnePersoInZdd)

/* 0x00489D60 */
AI_tdstNodeInterpret *__cdecl fn_p_stZDDCondition(
    HIE_tdstSuperObject *p_SuperObjPerso, AI_tdstNodeInterpret *p_stTree, long *p_lValue)
{
    AI_tdstGetSetParam   stParam;
    unsigned long        eCondId = AI_M_eCondIdInterpret(p_stTree - 1);
    HIE_tdstSuperObject *hCharacter1;
    HIE_tdstSuperObject *hCharacter2;

    p_stTree    = fn_p_stEvalTree(p_SuperObjPerso, p_stTree, &stParam);
    hCharacter1 = AI_M_GetSetParam_p_stSupObjValue(&stParam);

    p_stTree    = fn_p_stEvalTree(p_SuperObjPerso, p_stTree, &stParam);

    switch (eCondId)
    {
    case eCond_CollidePersoZDDNoWithPerso:
    case eCond_CollideModuleZDDNoWithPerso:
        {
            unsigned char ucModuleOrCharacter =
                (eCondId != eCond_CollidePersoZDDNoWithPerso) ? AI_ZDX_MODULE : AI_ZDX_CHARACTER;
            unsigned char ucChannelNumber = (unsigned char)M_GetSetParam_lValue(&stParam);

            p_stTree    = fn_p_stEvalTree(p_SuperObjPerso, p_stTree, &stParam);
            hCharacter2 = AI_M_GetSetParam_p_stSupObjValue(&stParam);

            if (hCharacter1)
                *p_lValue = (long)(AI_M_pfn_fn_bIsCharacterTWODetectedByThisZddOfCharacterOne(
                                       hCharacter1, ucModuleOrCharacter, ucChannelNumber, hCharacter2) & 0xff);
            else
                *p_lValue = (long)FALSE;
        }
        return p_stTree;

    case eCond_CollidePersoAllZDDWithPerso:
        hCharacter2 = AI_M_GetSetParam_p_stSupObjValue(&stParam);
        if (hCharacter1 && hCharacter2)
            *p_lValue = (long)(AI_M_pfn_fn_bIsCharacterTWODetectedByCharacterOne(
                                   hCharacter1, hCharacter2) & 0xff);
        else
            *p_lValue = (long)FALSE;
        return p_stTree;

    case eCond_CollidePersoZDDWithAnyPerso:
    case eCond_CollideModuleZDDWithAnyPerso:
        {
            unsigned char ucModuleOrCharacter =
                (eCondId != eCond_CollidePersoZDDWithAnyPerso) ? AI_ZDX_MODULE : AI_ZDX_CHARACTER;
            unsigned char ucChannelNumber = (unsigned char)M_GetSetParam_lValue(&stParam);

            if (hCharacter1)
                *p_lValue = (long)(fn_ucAtLeastOnePersoInZdd(
                                       hCharacter1, ucModuleOrCharacter, ucChannelNumber) & 0xff);
            else
                *p_lValue = (long)FALSE;
        }
        return p_stTree;

    default:
        break;
    }
    *p_lValue = (long)FALSE;
    return p_stTree;
}

/* 0x0042DE20 */
typedef unsigned short AI_GMT_tdxMask;

#define AI_M_pfn_GMT_fn_vResetEntireMask GMT_fn_vResetEntireMask

/* 0x0040D390 */
ACP_tdxBool __cdecl fn_bThisZdeFromOneVsThisZdeFromTwo(
    HIE_tdstSuperObject *, unsigned char, unsigned char, HIE_tdstSuperObject *, unsigned char, unsigned char);
#define AI_M_pfn_fn_bThisZdeFromOneVsThisZdeFromTwo fn_bThisZdeFromOneVsThisZdeFromTwo
/* 0x0040D350 */
extern unsigned char __cdecl fn_bThisZdeFromOneVsAllZdeFromTwo();
extern unsigned char __cdecl fn_bThisTypeFromOneVsThisTypeFromTwo();
extern unsigned char __cdecl fn_ucAtLeastOnePersoInZde();
#define AI_M_pfn_fn_bThisZdeFromOneVsAllZdeFromTwo \
    ((unsigned char (__cdecl *)(HIE_tdstSuperObject *, unsigned char, unsigned char, \
                                HIE_tdstSuperObject *, AI_GMT_tdxMask))fn_bThisZdeFromOneVsAllZdeFromTwo)
#define AI_M_pfn_fn_bThisTypeFromOneVsThisTypeFromTwo \
    ((unsigned char (__cdecl *)(HIE_tdstSuperObject *, AI_GMT_tdxMask, \
                                HIE_tdstSuperObject *, AI_GMT_tdxMask))fn_bThisTypeFromOneVsThisTypeFromTwo)
#define AI_M_pfn_fn_ucAtLeastOnePersoInZde \
    ((unsigned char (__cdecl *)(HIE_tdstSuperObject *, unsigned char, unsigned char, \
                                AI_GMT_tdxMask))fn_ucAtLeastOnePersoInZde)

#define AI_M_GetSetParam_xMaskValue(p) ((AI_GMT_tdxMask)(p)->uParam.ulValue)

#define eCond_CollidePersoZDENoWithPersoZDENo      0x0f
#define eCond_CollideModuleZDENoWithPersoZDENo     0x10
#define eCond_CollidePersoZDENoWithModuleZDENo     0x11
#define eCond_CollideModuleZDENoWithModuleZDENo    0x12
#define eCond_CollidePersoZDENoWithPersoTypeZDE    0x13
#define eCond_CollideModuleZDENoWithPersoTypeZDE   0x14
#define eCond_CollidePersoTypeZDEWithPersoTypeZDE  0x15
#define eCond_CollidePersoAllZDEWithPersoAllZDE    0x16
#define eCond_CollidePersoTypeZDEWithPersoAllZDE   0x17
#define eCond_CollidePersoAllZDEWithPersoTypeZDE   0x18
#define eCond_CollidePersoZDENoWithTypeZDE         0x19
#define eCond_CollideModuleZDENoWithTypeZDE        0x1a

/* 0x00489EE0 */
AI_tdstNodeInterpret *__cdecl fn_p_stZDECondition(
    HIE_tdstSuperObject *p_SuperObjPerso, AI_tdstNodeInterpret *p_stTree, long *p_lValue)
{
    AI_tdstGetSetParam   stParam;
    HIE_tdstSuperObject *hCharacter1 = NULL;
    HIE_tdstSuperObject *hCharacter2 = NULL;
    long                 eCondId = AI_M_eCondIdInterpret(p_stTree - 1);

    switch (eCondId)
    {

    case eCond_CollidePersoZDENoWithPersoZDENo:
    case eCond_CollideModuleZDENoWithPersoZDENo:
    case eCond_CollidePersoZDENoWithModuleZDENo:
    case eCond_CollideModuleZDENoWithModuleZDENo:
        {
            unsigned char ucChannelNumber1 = 0, ucChannelNumber2 = 0;
            unsigned char ucModuleOrCharacter1 = 0, ucModuleOrCharacter2 = 0;

            p_stTree        = fn_p_stEvalTree(p_SuperObjPerso, p_stTree, &stParam);
            hCharacter1     = AI_M_GetSetParam_p_stSupObjValue(&stParam);
            p_stTree        = fn_p_stEvalTree(p_SuperObjPerso, p_stTree, &stParam);
            ucChannelNumber1 = (unsigned char)M_GetSetParam_lValue(&stParam);
            p_stTree        = fn_p_stEvalTree(p_SuperObjPerso, p_stTree, &stParam);
            hCharacter2     = AI_M_GetSetParam_p_stSupObjValue(&stParam);
            p_stTree        = fn_p_stEvalTree(p_SuperObjPerso, p_stTree, &stParam);
            ucChannelNumber2 = (unsigned char)M_GetSetParam_lValue(&stParam);

            if (eCondId == eCond_CollidePersoZDENoWithPersoZDENo)
            {
                ucModuleOrCharacter1 = AI_ZDX_CHARACTER;
                ucModuleOrCharacter2 = AI_ZDX_CHARACTER;
            }
            else if (eCondId == eCond_CollideModuleZDENoWithPersoZDENo)
            {
                ucModuleOrCharacter1 = AI_ZDX_MODULE;
                ucModuleOrCharacter2 = AI_ZDX_CHARACTER;
            }
            else if (eCondId == eCond_CollidePersoZDENoWithModuleZDENo)
            {
                ucModuleOrCharacter1 = AI_ZDX_CHARACTER;
                ucModuleOrCharacter2 = AI_ZDX_MODULE;
            }
            else if (eCondId == eCond_CollideModuleZDENoWithModuleZDENo)
            {
                ucModuleOrCharacter1 = AI_ZDX_MODULE;
                ucModuleOrCharacter2 = AI_ZDX_MODULE;
            }

            if (hCharacter1 && hCharacter2)
                *p_lValue = (long)(AI_M_pfn_fn_bThisZdeFromOneVsThisZdeFromTwo(
                                       hCharacter1, ucModuleOrCharacter1, ucChannelNumber1,
                                       hCharacter2, ucModuleOrCharacter2, ucChannelNumber2) & 0xff);
            else
                *p_lValue = (long)FALSE;
        }
        return p_stTree;

    case eCond_CollidePersoZDENoWithPersoTypeZDE:
    case eCond_CollideModuleZDENoWithPersoTypeZDE:
        {
            unsigned char  ucChannelNumber;
            unsigned char  ucModuleOrCharacter = 0;
            AI_GMT_tdxMask xMask;

            p_stTree        = fn_p_stEvalTree(p_SuperObjPerso, p_stTree, &stParam);
            hCharacter1     = AI_M_GetSetParam_p_stSupObjValue(&stParam);
            p_stTree        = fn_p_stEvalTree(p_SuperObjPerso, p_stTree, &stParam);
            ucChannelNumber = (unsigned char)M_GetSetParam_lValue(&stParam);
            p_stTree        = fn_p_stEvalTree(p_SuperObjPerso, p_stTree, &stParam);
            hCharacter2     = AI_M_GetSetParam_p_stSupObjValue(&stParam);
            p_stTree        = fn_p_stEvalTree(p_SuperObjPerso, p_stTree, &stParam);
            xMask           = AI_M_GetSetParam_xMaskValue(&stParam);

            if (eCondId == eCond_CollidePersoZDENoWithPersoTypeZDE)
            {
                ucModuleOrCharacter = AI_ZDX_CHARACTER;
            }
            else if (eCondId == eCond_CollideModuleZDENoWithPersoTypeZDE)
            {
                ucModuleOrCharacter = AI_ZDX_MODULE;
            }
            if (hCharacter1 && hCharacter2)
                *p_lValue = (long)(AI_M_pfn_fn_bThisZdeFromOneVsAllZdeFromTwo(
                                       hCharacter1, ucModuleOrCharacter, ucChannelNumber,
                                       hCharacter2, xMask) & 0xff);
            else
                *p_lValue = (long)FALSE;
        }
        return p_stTree;

    case eCond_CollidePersoTypeZDEWithPersoTypeZDE:
    case eCond_CollidePersoTypeZDEWithPersoAllZDE:
    case eCond_CollidePersoAllZDEWithPersoAllZDE:
    case eCond_CollidePersoAllZDEWithPersoTypeZDE:
        {
            AI_GMT_tdxMask xMask1;
            AI_GMT_tdxMask xMask2;

            AI_M_pfn_GMT_fn_vResetEntireMask(&xMask1);
            AI_M_pfn_GMT_fn_vResetEntireMask(&xMask2);

            p_stTree    = fn_p_stEvalTree(p_SuperObjPerso, p_stTree, &stParam);
            hCharacter1 = AI_M_GetSetParam_p_stSupObjValue(&stParam);

            p_stTree    = fn_p_stEvalTree(p_SuperObjPerso, p_stTree, &stParam);
            if (eCondId == eCond_CollidePersoAllZDEWithPersoAllZDE)
            {
                hCharacter2 = AI_M_GetSetParam_p_stSupObjValue(&stParam);
            }
            else if (eCondId == eCond_CollidePersoAllZDEWithPersoTypeZDE)
            {
                hCharacter2 = AI_M_GetSetParam_p_stSupObjValue(&stParam);
                p_stTree    = fn_p_stEvalTree(p_SuperObjPerso, p_stTree, &stParam);
                xMask2      = AI_M_GetSetParam_xMaskValue(&stParam);
            }
            else if (eCondId == eCond_CollidePersoTypeZDEWithPersoAllZDE)
            {
                xMask1      = AI_M_GetSetParam_xMaskValue(&stParam);
                p_stTree    = fn_p_stEvalTree(p_SuperObjPerso, p_stTree, &stParam);
                hCharacter2 = AI_M_GetSetParam_p_stSupObjValue(&stParam);
            }
            else if (eCondId == eCond_CollidePersoTypeZDEWithPersoTypeZDE)
            {
                xMask1      = AI_M_GetSetParam_xMaskValue(&stParam);
                p_stTree    = fn_p_stEvalTree(p_SuperObjPerso, p_stTree, &stParam);
                hCharacter2 = AI_M_GetSetParam_p_stSupObjValue(&stParam);
                p_stTree    = fn_p_stEvalTree(p_SuperObjPerso, p_stTree, &stParam);
                xMask2      = AI_M_GetSetParam_xMaskValue(&stParam);
            }

            if (hCharacter1 && hCharacter2)
                *p_lValue = (long)(AI_M_pfn_fn_bThisTypeFromOneVsThisTypeFromTwo(
                                       hCharacter1, xMask1, hCharacter2, xMask2) & 0xff);
            else
                *p_lValue = (long)FALSE;
        }
        return p_stTree;

    case eCond_CollidePersoZDENoWithTypeZDE:
    case eCond_CollideModuleZDENoWithTypeZDE:
        {
            unsigned char  ucModuleOrCharacter =
                (eCondId == eCond_CollidePersoZDENoWithTypeZDE) ? AI_ZDX_CHARACTER : AI_ZDX_MODULE;
            unsigned char  ucChannelNumber = 0;
            AI_GMT_tdxMask xMask;

            p_stTree        = fn_p_stEvalTree(p_SuperObjPerso, p_stTree, &stParam);
            hCharacter1     = AI_M_GetSetParam_p_stSupObjValue(&stParam);
            p_stTree        = fn_p_stEvalTree(p_SuperObjPerso, p_stTree, &stParam);
            ucChannelNumber = (unsigned char)M_GetSetParam_lValue(&stParam);
            p_stTree        = fn_p_stEvalTree(p_SuperObjPerso, p_stTree, &stParam);
            xMask           = AI_M_GetSetParam_xMaskValue(&stParam);

            if (hCharacter1)
                *p_lValue = (long)(AI_M_pfn_fn_ucAtLeastOnePersoInZde(
                                       hCharacter1, ucModuleOrCharacter, ucChannelNumber, xMask) & 0xff);
            else
                *p_lValue = (long)FALSE;
        }
        return p_stTree;

    default:
        break;
    }
    *p_lValue = (long)FALSE;
    return p_stTree;
}

/* 0x0048A330 */
AI_tdstNodeInterpret *__cdecl fn_p_stReportOnSurfaceCondition(
    HIE_tdstSuperObject *p_SuperObjPerso, AI_tdstNodeInterpret *p_stTree, long *p_lValue)
{
    unsigned long        eCondId = AI_M_eCondIdInterpret(p_stTree - 1);
    HIE_tdstSuperObject *hNewSuperObjPerso;
    DNM_tdstReport      *p_stDynamReport;
    unsigned long        ulSurfState;

    fn_vGetUltraOperatorPerso(
        AI_M_pfn_fn_ucGetConditionUltraOperator(eCondId), p_SuperObjPerso, &hNewSuperObjPerso);

    p_stDynamReport = AI_M_pfn_fn_pstGetDNMReport(hNewSuperObjPerso);
    if (p_stDynamReport == NULL) { *p_lValue = (long)FALSE; return p_stTree; }

    ulSurfState = DNM_M_ulReportGetSurfaceState(p_stDynamReport);
    switch (eCondId)
    {
    case eCond_CollideWithGround:  *p_lValue = (long)((ulSurfState & C_WOT_ulGround)  == C_WOT_ulGround);  return p_stTree;
    case eCond_CollideWithWall:    *p_lValue = (long)((ulSurfState & C_WOT_ulWall)    == C_WOT_ulWall);    return p_stTree;
    case eCond_CollideWithNothing: *p_lValue = (long)(ulSurfState == C_WOT_ulNoObstacle);                  return p_stTree;
    case eCond_CollideWithCeiling: *p_lValue = (long)((ulSurfState & C_WOT_ulCeiling) == C_WOT_ulCeiling); return p_stTree;
    case eCond_CollideWithPerso:   *p_lValue = (long)((ulSurfState & C_WOT_ulMobile)  == C_WOT_ulMobile);  return p_stTree;
    case eCond_CollideWithWater:   *p_lValue = (long)((ulSurfState & C_WOT_ulWater)   == C_WOT_ulWater);   return p_stTree;
    default:                       break;
    }
    *p_lValue = (long)FALSE;
    return p_stTree;
}

#define eCond_CmtIdentifierContainsMask  0x41
#define eCond_HitByCollider              0x42
#define eCond_IsTypeOfGMTCollide         0x43
#define eCond_IsInComport                0x44
#define eCond_IsInReflexComport          0x45
#define eCond_ChangeActionEnable         0x47
#define eCond_EngineReinitRequested      0x48
#define eCond_IsThereMechEvent           0x49

#define AI_g_eEngineMode      (*(unsigned char *)(GAM_ENGINE_BASE + 0x0))
#define AI_EM_ModePlaying     9

ACP_tdxBool __cdecl fn_bIsTypeOfGMT(HIE_tdstSuperObject *p_SuperObjPerso, GMT_tdxMask xTestMask);
#define AI_M_pfn_fn_bIsTypeOfGMT fn_bIsTypeOfGMT

/* 0x0040FF10 */
extern long __cdecl PLA_fn_bTestNewState(HIE_tdstSuperObject *p_stSuperObject,
                                         void *h_WantedState);
#define AI_M_pfn_PLA_fn_bTestNewState  PLA_fn_bTestNewState
extern unsigned short __cdecl fn_uwGetNbEngineFrameSinceLastMechEvent(HIE_tdstSuperObject *p_stSuperObject);
#define AI_M_pfn_fn_uwGetNbEngineFrameSinceLastMechEvent fn_uwGetNbEngineFrameSinceLastMechEvent

#define AI_LME_INVALID  0xffff

#include "AI/Brain.h"

#define AI_M_pfn_GAM_fn_bIsCharacterActive GAM_fn_bIsCharacterActive

#define eCond_IsPersoActive  0x4c

/* 0x410310 */
extern long __cdecl PLA_fn_bTestIfEndOfAnim(HIE_tdstSuperObject *p_stSuperObject);
extern long __cdecl PLA_fn_bTestCurrentAction(HIE_tdstSuperObject *p_stSuperObject,
                                              void *h_FirstStateOfAction);
#define AI_M_pfn_PLA_fn_bTestIfEndOfAnim    PLA_fn_bTestIfEndOfAnim
#define AI_M_pfn_PLA_fn_bTestCurrentAction  PLA_fn_bTestCurrentAction

extern void __cdecl fn_vOnAIErm_M_UpdateLastError(unsigned short FatalErrorNum, char *szMsg);
#define AI_M_pfn_M_AIFatalError(uwError) \
    fn_vOnAIErm_M_UpdateLastError((unsigned short)(uwError), (char *)0)
#define AI_E_uwAIFatalNotValidCondition  0x2b

#define eCond_IsInAction      0x46
#define eCond_CheckActionEnd  0x4d
#define eCond_CheckAnimEnd    0x6a

#define AI_M_pfn_fn_bFindPersoInList  fn_bFindPersoInList
#define AI_M_pfn_fn_bFindModelInList  fn_bFindModelInList
#define AI_M_pfn_fn_bFindFamilyInList fn_bFindFamilyInList

#define AI_M_GetSetParam_Dsg_List(p)  ((void *)(p)->uParam.lValue)

#define AI_M_bListEmptyTest(p_vList)  (*(unsigned char *)(p_vList) == 0)

#define eCond_IsPersoInList   0x26
#define eCond_IsModelInList   0x27
#define eCond_IsFamilyInList  0x28
#define eCond_ListEmptyTest   0x29

/* 0x0048A6F0 */
AI_tdstNodeInterpret *__cdecl fn_p_stListCondition(
    HIE_tdstSuperObject *p_SuperObjPerso, AI_tdstNodeInterpret *p_stTree, long *p_lValue)
{
    AI_tdstGetSetParam stParam1, stParam2;
    unsigned long      eCondId = AI_M_eCondIdInterpret(p_stTree - 1);

    switch (eCondId)
    {
    case eCond_IsPersoInList:
        p_stTree = fn_p_stEvalTree(p_SuperObjPerso, p_stTree, &stParam1);
        p_stTree = fn_p_stEvalTree(p_SuperObjPerso, p_stTree, &stParam2);
        *p_lValue = (long)(signed char)AI_M_pfn_fn_bFindPersoInList(
                        AI_M_GetSetParam_Dsg_List(&stParam1),
                        AI_M_GetSetParam_p_stSupObjValue(&stParam2));
        return p_stTree;

    case eCond_IsModelInList:
        p_stTree = fn_p_stEvalTree(p_SuperObjPerso, p_stTree, &stParam1);
        p_stTree = fn_p_stEvalTree(p_SuperObjPerso, p_stTree, &stParam2);
        *p_lValue = (long)(signed char)AI_M_pfn_fn_bFindModelInList(
                        AI_M_GetSetParam_Dsg_List(&stParam1),
                        (void *)M_GetSetParam_lValue(&stParam2));
        return p_stTree;

    case eCond_IsFamilyInList:
        p_stTree = fn_p_stEvalTree(p_SuperObjPerso, p_stTree, &stParam1);
        p_stTree = fn_p_stEvalTree(p_SuperObjPerso, p_stTree, &stParam2);
        *p_lValue = (long)(signed char)AI_M_pfn_fn_bFindFamilyInList(
                        AI_M_GetSetParam_Dsg_List(&stParam1),
                        (void *)M_GetSetParam_lValue(&stParam2));
        return p_stTree;

    case eCond_ListEmptyTest:
        p_stTree  = fn_p_stEvalTree(p_SuperObjPerso, p_stTree, &stParam1);
        *p_lValue = (long)AI_M_bListEmptyTest(AI_M_GetSetParam_Dsg_List(&stParam1));
        return p_stTree;

    default:
        break;
    }
    *p_lValue = (long)FALSE;
    return p_stTree;
}

/* 0x0048A840 */
AI_tdstNodeInterpret *__cdecl fn_p_stTimeCondition(
    HIE_tdstSuperObject *p_SuperObjPerso, AI_tdstNodeInterpret *p_stTree, long *p_lValue)
{
    AI_tdstGetSetParam stParam;
    unsigned long      ulSavedTime, ulDeltaTime, ulLimitDeltaTime;
    unsigned long      ulCurrentTime = AI_g_ulCurrentTimerCount;

    p_stTree    = fn_p_stEvalTree(p_SuperObjPerso, p_stTree, &stParam);
    ulSavedTime = stParam.uParam.ulValue;

    p_stTree         = fn_p_stEvalTree(p_SuperObjPerso, p_stTree, &stParam);
    ulLimitDeltaTime = stParam.uParam.ulValue;

    if (ulCurrentTime >= ulSavedTime)
        ulDeltaTime = ulCurrentTime - ulSavedTime;
    else
        ulDeltaTime = ulSavedTime - ulCurrentTime;

    *p_lValue = (ulDeltaTime >= ulLimitDeltaTime);
    return p_stTree;
}

/* 0x0048A8A0 */
AI_tdstNodeInterpret *__cdecl fn_p_stValidityCondition(
    HIE_tdstSuperObject *p_SuperObjPerso, AI_tdstNodeInterpret *p_stTree, long *p_lValue)
{
    AI_tdstGetSetParam stParam;
    unsigned long      eCondId = AI_M_eCondIdInterpret(p_stTree - 1);

    p_stTree = fn_p_stEvalTree(p_SuperObjPerso, p_stTree, &stParam);

    switch (eCondId)
    {
    case eCond_IsValidText:
        *p_lValue = (long)(stParam.uParam.lValue != -1);
        return p_stTree;

    case eCond_IsValidObject:
        {
            struct tdstEngineObject_ *hObject = (struct tdstEngineObject_ *)stParam.uParam.p_stEngObjValue;
            unsigned char            *p_ucStdGame;
            *p_lValue = (long)(    (hObject != NULL)
                                && ((p_ucStdGame = (unsigned char *)hObject->h_StandardGame) != NULL)
                                && ((p_ucStdGame[0x29] & 1) == 0) );
        }
        return p_stTree;

    case eCond_IsValidWayPoint:
    case eCond_IsValidGMT:
    case eCond_IsValidAction:
        *p_lValue = (long)(stParam.uParam.lValue != 0);
        return p_stTree;

    default:
        break;
    }
    *p_lValue = (long)FALSE;
    return p_stTree;
}

/* 0x0048A950 */
AI_tdstNodeInterpret *__cdecl fn_p_stMiscCondition(
    HIE_tdstSuperObject *p_SuperObjPerso, AI_tdstNodeInterpret *p_stTree, long *p_lValue)
{
    AI_tdstGetSetParam stParam;
    unsigned long      eCondId = AI_M_eCondIdInterpret(p_stTree - 1);

    switch (eCondId)
    {
    case eCond_EngineReinitRequested:

        *p_lValue = (long)(AI_g_eEngineMode != AI_EM_ModePlaying);
        return p_stTree;

    case eCond_CmtIdentifierContainsMask:
        {
            AI_GMT_tdxMask xTestedMask, xBits;
            p_stTree    = fn_p_stEvalTree(p_SuperObjPerso, p_stTree, &stParam);
            xTestedMask = AI_M_GetSetParam_xMaskValue(&stParam);
            p_stTree    = fn_p_stEvalTree(p_SuperObjPerso, p_stTree, &stParam);
            xBits       = AI_M_GetSetParam_xMaskValue(&stParam);
            *p_lValue   = (long)((xTestedMask & xBits) != 0);
        }
        return p_stTree;

    case eCond_HitByCollider:
        {

            void *hCollSet = ((struct tdstEngineObject_ *)(HIE_fn_hGetSuperObjectObject(p_SuperObjPerso)))->h_CollSet;
            *p_lValue = (long)((hCollSet && fn_ucCollSetGetColliderPriority(hCollSet)) ? TRUE : FALSE);
            if (*p_lValue)

                fn_vCollSetSetColliderPriority(hCollSet, 0);
        }
        return p_stTree;

    case eCond_IsTypeOfGMTCollide:
        {
            DNM_tdstReport *p_stDynamReport;
            unsigned long   ulSurfState;

            p_stTree  = fn_p_stEvalTree(p_SuperObjPerso, p_stTree, &stParam);
            p_stDynamReport = (DNM_tdstReport *)AI_M_pfn_fn_pstGetDNMReport(p_SuperObjPerso);
            if (p_stDynamReport == (DNM_tdstReport *)0) { *p_lValue = (long)FALSE; return p_stTree; }

            ulSurfState = DNM_M_ulReportGetSurfaceState(p_stDynamReport);
            if (ulSurfState != C_WOT_ulNoObstacle)
                *p_lValue = (long)(AI_M_pfn_fn_bIsTypeOfGMT(
                                       p_SuperObjPerso, AI_M_GetSetParam_xMaskValue(&stParam)) & 0xff);
            else
                *p_lValue = (long)FALSE;
        }
        return p_stTree;

    case eCond_IsInComport:
    case eCond_IsInReflexComport:
        {
            HIE_tdstSuperObject *hTempPerso;
            void                *p_stIntelligence;

            p_stTree   = fn_p_stEvalTree(p_SuperObjPerso, p_stTree, &stParam);
            hTempPerso = AI_M_GetSetParam_p_stSupObjValue(&stParam);

            p_stTree   = fn_p_stEvalTree(p_SuperObjPerso, p_stTree, &stParam);

            p_stIntelligence = (eCondId == eCond_IsInComport)
                                 ? AI_M_p_stGetIntelligence(AI_M_stGetMindOfSuperObj(hTempPerso))
                                 : AI_M_p_stGetReflex(AI_M_stGetMindOfSuperObj(hTempPerso));

            if (p_stIntelligence != (void *)0)

                *p_lValue = (long)((void *)AI_M_GetCurrentComport((AI_tdstIntelligence *)p_stIntelligence)
                                   == (void *)M_GetSetParam_lValue(&stParam));
            else
                *p_lValue = (long)FALSE;
        }
        return p_stTree;

    case eCond_ChangeActionEnable:
        p_stTree  = fn_p_stEvalTree(p_SuperObjPerso, p_stTree, &stParam);
        *p_lValue = (unsigned long)AI_M_pfn_PLA_fn_bTestNewState(
                               p_SuperObjPerso, (void *)M_GetSetParam_lValue(&stParam));
        return p_stTree;

    case eCond_IsThereMechEvent:
        {
            unsigned char  ucVarId = AI_M_uVarIdInterpret(p_stTree);
            unsigned short uwNbFrame;

            p_stTree++;
            uwNbFrame = AI_M_pfn_fn_uwGetNbEngineFrameSinceLastMechEvent(p_SuperObjPerso);
            if (uwNbFrame != AI_LME_INVALID)
            {

				M_Full_GetSetParam_Integer(&stParam, uwNbFrame);
                if (fn_ucSetDsgVar(ucVarId, 0, AI_M_stGetMindOfSuperObj(p_SuperObjPerso), &stParam) == C_INVALID_SET)
                {

                    *p_lValue = (long)FALSE;
                }
                else
                    *p_lValue = (long)TRUE;
            }
            else
                *p_lValue = (long)FALSE;
        }
        return p_stTree;

    default:
        break;
    }
    *p_lValue = (long)FALSE;
    return p_stTree;
}

extern unsigned char __cdecl fn_ucMSLightGetOnOff(void *h_MSLight);
extern unsigned char __cdecl fn_bMSLightGetPulse(void *h_MSLight);
extern unsigned char __cdecl fn_bMSLightGetGiroPhare(void *h_MSLight);
#define AI_M_pfn_fn_ucMSLightGetOnOff     fn_ucMSLightGetOnOff
#define AI_M_pfn_fn_bMSLightGetPulse      fn_bMSLightGetPulse
#define AI_M_pfn_fn_bMSLightGetGiroPhare  fn_bMSLightGetGiroPhare

#define eCond_IsPersoLightOn           0x3a
#define eCond_IsPersoLightPulseOn      0x3b
#define eCond_IsPersoLightGyroPhareOn  0x3c

/* 0x00482A00 */
void __cdecl fn_vGetCenterOfBoundingVolume(HIE_tdstSuperObject *hPerso, MTH3D_tdstVector *p_stVertex);

extern HIE_tdstSuperObject *__cdecl GAM_fn_hGetCurrentSector(HIE_tdstSuperObject *_hCharacter);

typedef struct AICond_tdstGraphicNode_
{
    unsigned char                      _pad_0[4];
    HIE_tdstSuperObject               *hPointerOfSectorSO;
    struct AICond_tdstGraphicNode_    *pNext;
} AICond_tdstGraphicNode;

typedef struct AICond_tdstSectorObj_
{
    unsigned char                      _pad_0[0x24];
    AICond_tdstGraphicNode            *a_pGraphicHead;
    unsigned char                      _pad_1[0x2C - 0x28];
    long                               lNbGraphicNodes;
} AICond_tdstSectorObj;

unsigned char fn_bPerso1Point1IntersectPerso2Point2(
    HIE_tdstSuperObject *hPerso1, MTH3D_tdstVector *p_stVertex1,
    HIE_tdstSuperObject *hPerso2, MTH3D_tdstVector *p_stVertex2)
{
    MTH3D_tdstVector           stTempVector;
    HIE_tdstSuperObject       *hSector1;
    HIE_tdstSuperObject       *hSector2;
    AICond_tdstGraphicNode    *hList;
    long                       i;

    hSector1 = GAM_fn_hGetCurrentSector(hPerso1);
    if (hSector1 == NULL) return FALSE;

    hSector2 = GAM_fn_hGetCurrentSector(hPerso2);
    if (hSector2 == NULL) return FALSE;

    MTH3D_M_vSubVector(&stTempVector, p_stVertex2, p_stVertex1);

    if (HIE_bDetectIntersectSegmentWithSuperObject(p_stVertex1, &stTempVector, hSector1))
        return TRUE;

    if (hSector1 != hSector2)
    {
        if (HIE_bDetectIntersectSegmentWithSuperObject(p_stVertex1, &stTempVector, hSector2))
            return TRUE;
    }

    i     = ((AICond_tdstSectorObj *)hSector1->hLinkedObject.p_Void)->lNbGraphicNodes;
    hList = ((AICond_tdstSectorObj *)hSector1->hLinkedObject.p_Void)->a_pGraphicHead;
    for (; i > 0; i--, hList = hList->pNext)
    {
        if ((hList->hPointerOfSectorSO != hSector2) &&
            HIE_bDetectIntersectSegmentWithSuperObject(p_stVertex1, &stTempVector, hList->hPointerOfSectorSO))
        {
            return TRUE;
        }
    }
    return FALSE;
}

#define eCond_SeePerso  0x36

/* 0x0048ACE0 */
AI_tdstNodeInterpret *__cdecl fn_p_stSectorCondition(
    HIE_tdstSuperObject *p_SuperObjPerso, AI_tdstNodeInterpret *p_stTree, long *p_lValue)
{
    unsigned long        eCondId = AI_M_eCondIdInterpret(p_stTree - 1);
    HIE_tdstSuperObject *hNewSuperObjPerso;
    AI_tdstGetSetParam   stParam;

    fn_vGetUltraOperatorPerso(
        AI_M_pfn_fn_ucGetConditionUltraOperator(eCondId), p_SuperObjPerso, &hNewSuperObjPerso);

    if (eCondId != eCond_SeePerso)
    {
        *p_lValue = (long)FALSE;
        return p_stTree;
    }
    {
        HIE_tdstSuperObject *pPerso1 = hNewSuperObjPerso;
        HIE_tdstSuperObject *pPerso2;
        MTH3D_tdstVector     stVertex1, stVertex2;

        p_stTree = fn_p_stEvalTree(p_SuperObjPerso, p_stTree, &stParam);
        pPerso2  = AI_M_GetSetParam_p_stSupObjValue(&stParam);

        fn_vGetCenterOfBoundingVolume(pPerso1, &stVertex1);
        fn_vGetCenterOfBoundingVolume(pPerso2, &stVertex2);

        *p_lValue = (long)((unsigned char)fn_bPerso1Point1IntersectPerso2Point2(
                               pPerso1, &stVertex1, pPerso2, &stVertex2) == 0);
        return p_stTree;
    }
}

/* 0x0048AD90 */
AI_tdstNodeInterpret *__cdecl fn_p_stLightCondition(
    HIE_tdstSuperObject *p_SuperObjPerso, AI_tdstNodeInterpret *p_stTree, long *p_lValue)
{
    unsigned long        eCondId = AI_M_eCondIdInterpret(p_stTree - 1);
    HIE_tdstSuperObject *hNewSuperObjPerso;
    void                *h_MSLight;
    long                 lValue = (long)FALSE;

    fn_vGetUltraOperatorPerso(
        AI_M_pfn_fn_ucGetConditionUltraOperator(eCondId), p_SuperObjPerso, &hNewSuperObjPerso);

    h_MSLight = AI_M_GetEngineObject(hNewSuperObjPerso)->h_MSLight;
    if (h_MSLight != (void *)0)
    {
        switch (eCondId)
        {
        case eCond_IsPersoLightOn:
            lValue = (long)(AI_M_pfn_fn_ucMSLightGetOnOff(h_MSLight) & 0xff);
            break;
        case eCond_IsPersoLightPulseOn:
            lValue = (long)(AI_M_pfn_fn_bMSLightGetPulse(h_MSLight) & 0xff);
            break;
        case eCond_IsPersoLightGyroPhareOn:
            lValue = (long)(AI_M_pfn_fn_bMSLightGetGiroPhare(h_MSLight) & 0xff);
            break;
        default:
            break;
        }
    }

    *p_lValue = lValue;
    return p_stTree;
}

/* 0x0048AE10 */
AI_tdstNodeInterpret *__cdecl fn_p_st_ActivationCondition(
    HIE_tdstSuperObject *p_SuperObjPerso, AI_tdstNodeInterpret *p_stTree, long *p_lValue)
{
    unsigned long        eCondId = AI_M_eCondIdInterpret(p_stTree - 1);
    HIE_tdstSuperObject *hNewSuperObjPerso;

    fn_vGetUltraOperatorPerso(
        AI_M_pfn_fn_ucGetConditionUltraOperator(eCondId), p_SuperObjPerso, &hNewSuperObjPerso);
    *p_lValue = (long)M_ObjectIsActivable(AI_M_GetEngineObject(hNewSuperObjPerso));
    return p_stTree;
}

/* 0x0048AE60 */
AI_tdstNodeInterpret *__cdecl fn_p_st_HandledCondition(
    HIE_tdstSuperObject *p_SuperObjPerso, AI_tdstNodeInterpret *p_stTree, long *p_lValue)
{
    AI_tdstGetSetParam stParam;

    p_stTree  = fn_p_stEvalTree(p_SuperObjPerso, p_stTree, &stParam);
    *p_lValue = (long)M_ObjectIsAlreadyTreated(
                    (struct tdstEngineObject_ *)stParam.uParam.p_stEngObjValue);
    return p_stTree;
}

#define AI_M_pfn_fn_ucIsAlwaysGeneratedByMe fn_ucIsAlwaysGeneratedByMe

#define eCond_Alw_IsMine  0x39

/* 0x0048AF80 */
AI_tdstNodeInterpret *__cdecl fn_p_stAlw_IsMine(
    HIE_tdstSuperObject *p_SuperObjPerso, AI_tdstNodeInterpret *p_stTree, long *p_lValue)
{
    HIE_tdstSuperObject *hNewSuperObjPerso;
    AI_tdstGetSetParam   stParam;
    char                *hAlwaysObject;

    fn_vGetUltraOperatorPerso(
        AI_M_pfn_fn_ucGetConditionUltraOperator(eCond_Alw_IsMine), p_SuperObjPerso, &hNewSuperObjPerso);

    p_stTree      = fn_p_stEvalTree(p_SuperObjPerso, p_stTree, &stParam);
    hAlwaysObject = (char *)stParam.uParam.p_stEngObjValue;

    *p_lValue = (hAlwaysObject != (char *)0 && *(void **)(hAlwaysObject + 4) != (void *)0)
        ? (long)(AI_M_pfn_fn_ucIsAlwaysGeneratedByMe(
                     AI_M_GetSetParam_p_stSupObjValue(&stParam), hNewSuperObjPerso) & 0xff)
        : 0;

    return p_stTree;
}

/* 0x0048B000 */
AI_tdstNodeInterpret *__cdecl AI_fn_p_st_pc_ReportSurfaceMaterialCondition(
    HIE_tdstSuperObject *p_SuperObjPerso, AI_tdstNodeInterpret *p_stTree, long *p_lValue)
{
    AI_tdstGetSetParam stParam;
    long               eCondId = AI_M_eCondIdInterpret(p_stTree - 1);
    DNM_tdstReport    *p_stDynamReport;
    unsigned long      ulSurfState;

    p_stTree = fn_p_stEvalTree(p_SuperObjPerso, p_stTree, &stParam);

    p_stDynamReport = AI_M_pfn_fn_pstGetDNMReport(p_SuperObjPerso);
    if (p_stDynamReport == NULL) { *p_lValue = (long)FALSE; return p_stTree; }

    ulSurfState = DNM_M_ulReportGetSurfaceState(p_stDynamReport);
    *p_lValue = (long)FALSE;
    switch (eCondId)
    {
    case 0x3d: if (ulSurfState == 0)           return p_stTree; *p_lValue = (long)(AI_M_pfn_fn_bTestTypeOfGameMaterial(DNM_M_hObstacleGetCollidedMaterial(DNM_M_p_stReportGetObstacle(p_stDynamReport)), stParam.uParam.lValue) & 0xff); return p_stTree;
    case 0x3e: if ((ulSurfState & 0x04) == 0)  return p_stTree; *p_lValue = (long)(AI_M_pfn_fn_bTestTypeOfGameMaterial(DNM_M_hObstacleGetCollidedMaterial(DNM_M_p_stReportGetWall(p_stDynamReport)), stParam.uParam.lValue) & 0xff); return p_stTree;
    case 0x3f: if ((ulSurfState & 0x01) == 0)  return p_stTree; *p_lValue = (long)(AI_M_pfn_fn_bTestTypeOfGameMaterial(DNM_M_hObstacleGetCollidedMaterial(DNM_M_p_stReportGetGround(p_stDynamReport)), stParam.uParam.lValue) & 0xff); return p_stTree;
    case 0x40: if ((ulSurfState & 0x10) == 0)  return p_stTree; *p_lValue = (long)(AI_M_pfn_fn_bTestTypeOfGameMaterial(DNM_M_hObstacleGetCollidedMaterial(DNM_M_p_stReportGetCeil(p_stDynamReport)), stParam.uParam.lValue) & 0xff); return p_stTree;
    default:   return p_stTree;
    }
}

/* 0x00419B90 */
extern void *__cdecl fn_p_stDynamGetDynamics(void *h_Dynam);
#define AI_M_pfn_fn_p_stDynamGetDynamics  fn_p_stDynamGetDynamics

#define eCond_HierIsSonOfActor  0x68

#define eCond_IsMorphing  0x69

#define AI_MTH_C_EpsilonPlus  1.0e-5f

#define AI_M_bIsNullWithEpsilon(x, eps)  MTH_M_bIsNullWithEpsilon(x, eps)

#define eCond_IsNullVector  0x67

/* 0x0048B2D0 */
AI_tdstNodeInterpret *__cdecl fn_p_stNullVector(
    HIE_tdstSuperObject *p_SuperObjPerso, AI_tdstNodeInterpret *p_stTree, long *p_lValue)
{
    AI_tdstGetSetParam stParam;
    float             *p_stVector;

    p_stTree = fn_p_stEvalTree(p_SuperObjPerso, p_stTree, &stParam);
    p_stVector = &stParam.uParam.a_xVectorValue[0];

    *p_lValue = (long)((   AI_M_bIsNullWithEpsilon(p_stVector[0], AI_MTH_C_EpsilonPlus)
                        && AI_M_bIsNullWithEpsilon(p_stVector[1], AI_MTH_C_EpsilonPlus)
                        && AI_M_bIsNullWithEpsilon(p_stVector[2], AI_MTH_C_EpsilonPlus)) ? TRUE : FALSE);
    return p_stTree;
}

#define AI_M_pfn_fn_p_stSkipThisArgument fn_p_stSkipThisArgument

#define AI_M_pfn_fn_p_stEvalNextVarIdPerso(a, b, c) \
    fn_p_stOperatorGetPerso((a), (b), (HIE_tdstSuperObject **)(c))

#define eCond_IsGiBlock                 0x4f
#define eCond_IsMechanicBlock           0x50
#define eCond_IsMechanicAnimation       0x51
#define eCond_IsMechanicCollide         0x52
#define eCond_IsMechanicGravity         0x53
#define eCond_IsMechanicTilt            0x54
#define eCond_IsMechanicGi              0x55
#define eCond_IsMechanicClimb           0x56
#define eCond_IsMechanicOnGround        0x57
#define eCond_IsMechanicSpider          0x58
#define eCond_IsMechanicShoot           0x59
#define eCond_IsMechanicSwim            0x5a
#define eCond_IsMechanicNeverFall       0x5b
#define eCond_IsMechanicCollisionControl 0x5c
#define eCond_IsMechanicKeepSpeedZ      0x5d
#define eCond_IsMechanicSpeedLimit      0x5e
#define eCond_IsMechanicInertia         0x5f
#define eCond_IsMechanicStream          0x60
#define eCond_IsMechanicStickOnPlatform 0x61
#define eCond_IsMechanicPatformCrash    0x62
#define eCond_IsMechanicScale           0x63
#define eCond_IsMechanicExec            0x64
#define eCond_CanFall                   0x65
#define eCond_IsMechanicCrash           0x66

/* 0x0048B380 */
AI_tdstNodeInterpret *__cdecl fn_p_stMechanicOption(
    HIE_tdstSuperObject *p_SuperObjPerso, AI_tdstNodeInterpret *p_stTree, long *p_lValue)
{
    unsigned long        eCondId = AI_M_eCondIdInterpret(p_stTree - 1);
    HIE_tdstSuperObject *hNewSuperObjPerso;
    void                *h_Dynam;
    DNM_tdstDynamics    *pstDynamic;
    unsigned long        bOption;

    fn_vGetUltraOperatorPerso(
        AI_M_pfn_fn_ucGetConditionUltraOperator(eCondId), p_SuperObjPerso, &hNewSuperObjPerso);

    h_Dynam = ((struct tdstEngineObject_ *)(HIE_fn_hGetSuperObjectObject(hNewSuperObjPerso)))->h_Dynam;
    if (h_Dynam == (void *)0) { *p_lValue = (long)FALSE; return p_stTree; }

    pstDynamic = (DNM_tdstDynamics *)AI_M_pfn_fn_p_stDynamGetDynamics(h_Dynam);

    switch (eCondId)
    {
    case eCond_IsMechanicAnimation:          bOption = (DNM_M_ulFlags(pstDynamic)      ) & 1; break;
    case eCond_IsMechanicCollide:            bOption = (DNM_M_ulFlags(pstDynamic) >>  1) & 1; break;
    case eCond_IsMechanicGravity:            bOption = (DNM_M_ulFlags(pstDynamic) >>  2) & 1; break;
    case eCond_IsMechanicTilt:               bOption = (DNM_M_ulFlags(pstDynamic) >>  3) & 1; break;
    case eCond_IsMechanicGi:                 bOption = (DNM_M_ulFlags(pstDynamic) >>  4) & 1; break;
    case eCond_IsMechanicClimb:              bOption = (DNM_M_ulFlags(pstDynamic) >>  6) & 1; break;
    case eCond_IsMechanicOnGround:           bOption = (DNM_M_ulFlags(pstDynamic) >>  5) & 1; break;
    case eCond_IsMechanicSpider:             bOption = (DNM_M_ulEndFlags(pstDynamic) >>  8) & 1; break;
    case eCond_IsMechanicShoot:              bOption = (DNM_M_ulEndFlags(pstDynamic) >>  9) & 1; break;
    case eCond_IsMechanicSwim:               bOption = (DNM_M_ulEndFlags(pstDynamic) >>  9) & 1; break;
    case eCond_IsMechanicNeverFall:          bOption = (DNM_M_ulEndFlags(pstDynamic) >> 0x15) & 1; break;
    case eCond_IsMechanicCollisionControl:   bOption = (DNM_M_ulFlags(pstDynamic) >>  7) & 1; break;
    case eCond_IsMechanicKeepSpeedZ:         bOption = (DNM_M_ulFlags(pstDynamic) >>  8) & 1; break;
    case eCond_IsMechanicSpeedLimit:         bOption = (DNM_M_ulFlags(pstDynamic) >>  9) & 1; break;
    case eCond_IsMechanicInertia:            bOption = (DNM_M_ulFlags(pstDynamic) >> 10) & 1; break;
    case eCond_IsMechanicStream:             bOption = (DNM_M_ulFlags(pstDynamic) >> 11) & 1; break;
    case eCond_IsMechanicStickOnPlatform:    bOption = (DNM_M_ulFlags(pstDynamic) >> 12) & 1; break;
    case eCond_IsMechanicPatformCrash:       bOption = (DNM_M_ulEndFlags(pstDynamic) >>  5) & 1; break;

    case eCond_CanFall:
        if ((bOption = (DNM_M_ulEndFlags(pstDynamic) >> 6) & 1) != 0)
        {
            AI_tdstGetSetParam    stParam;
            void                 *p_stMind;
            unsigned char         ucVectorId;

            {
                HIE_tdstSuperObject *p_stResultOwnerPerso;
                if (M_GetTypeInterpret(p_stTree) == E_ti_Operator)
                {
                    p_stTree++;
                    p_stTree = AI_M_pfn_fn_p_stEvalNextVarIdPerso(p_SuperObjPerso, p_stTree, &p_stResultOwnerPerso);
                }
                else
                    p_stResultOwnerPerso = p_SuperObjPerso;
                p_stMind = AI_M_stGetMindOfSuperObj(p_stResultOwnerPerso);
                ucVectorId = *(unsigned char *)p_stTree;
                p_stTree++;
            }

			M_GetSetParam_stVertexValue(&stParam) = *DNM_M_pDynamicsGetFallTranslation(pstDynamic);
			M_GetSetParam_Type(&stParam) = E_vt_Vector;
            ucVectorId = fn_ucSetDsgVar(ucVectorId, 0, p_stMind, &stParam);
        }
        else
            p_stTree = AI_M_pfn_fn_p_stSkipThisArgument(p_stTree);
        break;

    case eCond_IsGiBlock:
    case eCond_IsMechanicBlock:
        bOption = (DNM_M_ulEndFlags(pstDynamic) >> 0x10) & 1;
        DNM_M_ulEndFlags(pstDynamic) &= 0xfffeffffUL;
        break;

    case eCond_IsMechanicScale:              bOption = (DNM_M_ulFlags(pstDynamic) >> 13) & 1; break;

    case eCond_IsMechanicExec:
        bOption = (DNM_M_ulEndFlags(pstDynamic) >> 0x0d) & 1;
        DNM_M_ulEndFlags(pstDynamic) &= 0xffffdfffUL;
        break;

    case eCond_IsMechanicCrash:              bOption = (DNM_M_ulEndFlags(pstDynamic) >> 0x13) & 1; break;

    default:
        bOption = FALSE;
        break;
    }

    *p_lValue = bOption ? TRUE : FALSE;
    return p_stTree;
}

/* 0x0048B620 */
AI_tdstNodeInterpret *__cdecl fn_p_stCheckAnimEnd(
    HIE_tdstSuperObject *p_SuperObjPerso, AI_tdstNodeInterpret *p_stTree, long *p_lValue)
{
    unsigned long        eCondId = AI_M_eCondIdInterpret(p_stTree - 1);
    HIE_tdstSuperObject *hNewSuperObjPerso;
    AI_tdstGetSetParam   stParam;

    fn_vGetUltraOperatorPerso(
        AI_M_pfn_fn_ucGetConditionUltraOperator(eCondId), p_SuperObjPerso, &hNewSuperObjPerso);

    switch (eCondId)
    {
    case eCond_IsInAction:

        p_stTree  = fn_p_stEvalTree(p_SuperObjPerso, p_stTree, &stParam);
        *p_lValue = (long)AI_M_pfn_PLA_fn_bTestCurrentAction(
                               hNewSuperObjPerso, (void *)M_GetSetParam_lValue(&stParam));
        return p_stTree;
    case eCond_CheckActionEnd:
        *p_lValue = (long)PLA_fn_bTestIfEndOfAction(hNewSuperObjPerso);
        return p_stTree;
    case eCond_CheckAnimEnd:
        *p_lValue = (long)AI_M_pfn_PLA_fn_bTestIfEndOfAnim(hNewSuperObjPerso);
        return p_stTree;
    default:

        AI_M_pfn_M_AIFatalError(AI_E_uwAIFatalNotValidCondition);
        return p_stTree;
    }
}

/* 0x0048B6E0 */
AI_tdstNodeInterpret *__cdecl fn_p_stHierarchySon(
    HIE_tdstSuperObject *p_SuperObjPerso, AI_tdstNodeInterpret *p_stTree, long *p_lValue)
{
    unsigned long        eCondId = AI_M_eCondIdInterpret(p_stTree - 1);
    HIE_tdstSuperObject *hNewSuperObjPerso;
    AI_tdstGetSetParam   stParam;
    HIE_tdstSuperObject *hFather;
    HIE_tdstSuperObject *hChild;
    int                  i;
    DNM_tdstDynamics    *pstDynamic;
    void                *h_Dynam;

    fn_vGetUltraOperatorPerso(
        AI_M_pfn_fn_ucGetConditionUltraOperator(eCondId), p_SuperObjPerso, &hNewSuperObjPerso);
    *p_lValue = (long)FALSE;

    p_stTree = fn_p_stEvalTree(p_SuperObjPerso, p_stTree, &stParam);
    hFather  = (HIE_tdstSuperObject *)AI_M_GetSetParam_p_stSupObjValue(&stParam);

    h_Dynam = ((struct tdstEngineObject_ *)(HIE_fn_hGetSuperObjectObject(hNewSuperObjPerso)))->h_Dynam;
    if (h_Dynam != (void *)0)
    {
        pstDynamic = (DNM_tdstDynamics *)AI_M_pfn_fn_p_stDynamGetDynamics(h_Dynam);
        if (DNM_M_bDynamicsIsComplexSize(pstDynamic))
        {
            if (DNM_M_hDynamicsGetPlatformSO(pstDynamic) == hFather)
                *p_lValue = (long)TRUE;
            return p_stTree;
        }
    }

    HIE_M_ForEachChildOf(hFather, hChild, i)
    {
        if (hChild == hNewSuperObjPerso)
        {
            *p_lValue = (long)TRUE;
            return p_stTree;
        }
    }

    return p_stTree;
}

#define AI_M_pfn_fn_ulGetCapabilities \
    ((unsigned long (__cdecl *)(void *))fn_ulGetCapabilities)

#define eCond_HasTheCapability             0x6b
#define eCond_HasOneOfTheCapabilities      0x6c
#define eCond_HasTheCapabilityNumber       0x6d
#define eCond_PersoHasTheCapability        0x6e
#define eCond_PersoHasOneOfTheCapabilities 0x6f
#define eCond_PersoHasTheCapabilityNumber  0x70

/* 0x0048B7A0 */
AI_tdstNodeInterpret *__cdecl fn_p_stCheckCapabilities(
    HIE_tdstSuperObject *p_SuperObjPerso, AI_tdstNodeInterpret *p_stTree, long *p_lValue)
{
    unsigned long        eCondId = AI_M_eCondIdInterpret(p_stTree - 1);
    HIE_tdstSuperObject *hNewSuperObjPerso;
    AI_tdstGetSetParam   stParam;
    unsigned long        ubf32CapabilityToTest = 0;
    unsigned long        ubf32BitOfCapability  = 0;
    unsigned char        ucBitNumber;

    fn_vGetUltraOperatorPerso(
        AI_M_pfn_fn_ucGetConditionUltraOperator(eCondId), p_SuperObjPerso, &hNewSuperObjPerso);
    *p_lValue = (long)FALSE;

    switch (eCondId)
    {
    case eCond_HasTheCapability:
    case eCond_HasOneOfTheCapabilities:
        p_stTree = fn_p_stEvalTree(p_SuperObjPerso, p_stTree, &stParam);
        ubf32CapabilityToTest = stParam.uParam.ulValue;
        p_stTree = fn_p_stEvalTree(p_SuperObjPerso, p_stTree, &stParam);
        ubf32BitOfCapability = stParam.uParam.ulValue;
        break;

    case eCond_HasTheCapabilityNumber:
        p_stTree = fn_p_stEvalTree(p_SuperObjPerso, p_stTree, &stParam);
        ubf32CapabilityToTest = stParam.uParam.ulValue;
        p_stTree = fn_p_stEvalTree(p_SuperObjPerso, p_stTree, &stParam);
        ucBitNumber = stParam.uParam.ucValue;
        if (ucBitNumber >= 32) return p_stTree;
        ubf32BitOfCapability = 1 << ucBitNumber;
        break;

    case eCond_PersoHasTheCapability:
    case eCond_PersoHasOneOfTheCapabilities:
        ubf32CapabilityToTest = AI_M_pfn_fn_ulGetCapabilities(AI_M_GetEngineObject(hNewSuperObjPerso));
        p_stTree = fn_p_stEvalTree(p_SuperObjPerso, p_stTree, &stParam);
        ubf32BitOfCapability = stParam.uParam.ulValue;
        break;

    case eCond_PersoHasTheCapabilityNumber:
        ubf32CapabilityToTest = AI_M_pfn_fn_ulGetCapabilities(AI_M_GetEngineObject(hNewSuperObjPerso));
        p_stTree = fn_p_stEvalTree(p_SuperObjPerso, p_stTree, &stParam);
        ucBitNumber = stParam.uParam.ucValue;
        if (ucBitNumber >= 32) return p_stTree;
        ubf32BitOfCapability = 1 << ucBitNumber;
        break;

    default:
        break;
    }

    if (eCondId == eCond_HasOneOfTheCapabilities || eCondId == eCond_PersoHasOneOfTheCapabilities)
        *p_lValue = (long)((ubf32CapabilityToTest & ubf32BitOfCapability) != 0);
    else
        *p_lValue = (long)((ubf32CapabilityToTest & ubf32BitOfCapability) == ubf32BitOfCapability);

    return p_stTree;
}

/* 0x0048B920 */
AI_tdstNodeInterpret *__cdecl fn_p_stIsPersoActive(
    HIE_tdstSuperObject *p_SuperObjPerso, AI_tdstNodeInterpret *p_stTree, long *p_lValue)
{
    AI_tdstGetSetParam stParam;
    void              *p_stEngineObj;

    p_stTree      = fn_p_stEvalTree(p_SuperObjPerso, p_stTree, &stParam);
    p_stEngineObj = stParam.uParam.p_stEngObjValue;

    if (((struct tdstEngineObject_ *)p_stEngineObj)->h_StandardGame != (void *)0)
    {
        HIE_tdstSuperObject *hPerso = AI_M_GetSetParam_p_stSupObjValue(&stParam);
        *p_lValue = (long)(AI_M_pfn_GAM_fn_bIsCharacterActive(hPerso) & 0xff);
    }
    else
        *p_lValue = (long)0;

    return p_stTree;
}

#include "GAM/PlayAnim.h"

#define AI_M_pfn_fn_ucGetStateCustomBits fn_ucGetStateCustomBits

#define eCond_IsCustomBitSet              0x4b
#define eCond_IsCurrentStateCustomBitSet  0x4e

/* 0x0048B0C0 */
AI_tdstNodeInterpret *__cdecl fn_p_stCustomBitsCond(
    HIE_tdstSuperObject *p_SuperObjPerso, AI_tdstNodeInterpret *p_stTree, long *p_lValue)
{
    unsigned long        eCondId = AI_M_eCondIdInterpret(p_stTree - 1);
    HIE_tdstSuperObject *hNewSuperObjPerso;
    AI_tdstGetSetParam   stParam;

    fn_vGetUltraOperatorPerso(
        AI_M_pfn_fn_ucGetConditionUltraOperator(eCondId), p_SuperObjPerso, &hNewSuperObjPerso);

    p_stTree     = fn_p_stEvalTree(p_SuperObjPerso, p_stTree, &stParam);

    switch (eCondId)
    {
    case eCond_IsCustomBitSet:
        {
            unsigned long  ulMask       = (1 << (M_GetSetParam_lValue(&stParam) - 1));
            unsigned long  ulCustomBits = fn_ulStandardGameGetCustomBits(M_GetMSHandle(hNewSuperObjPerso,StandardGame));
            *p_lValue = (long)((ulMask & ulCustomBits) != 0);
        }
        return p_stTree;
    case eCond_IsCurrentStateCustomBitSet:
        {
            unsigned long  ulMask   = (unsigned long)stParam.uParam.ulValue;
            tdxHandleToState h_State;
            h_State = fn_h3dDataGetCurrentState(M_GetMSHandle(hNewSuperObjPerso,3dData));
            *p_lValue = ((h_State != (tdxHandleToState)0 && (AI_M_pfn_fn_ucGetStateCustomBits(h_State) & ulMask)) ? TRUE : FALSE);
        }
        return p_stTree;
    default:
        *p_lValue = (long)FALSE;
        return p_stTree;
    }
}

#define eCond_CollisionWP  0x4a

/* 0x00487860 */
extern void __cdecl WP_fnv_WayPoint_ComputeLocation(void *_hWayPoint, MTH3D_tdstVector *_p_stVertex);

#define _WP_D_WAYPOINT_FRIEND_
#include "AI/WPWayPt.h"
#undef _WP_D_WAYPOINT_FRIEND_
AI_tdstNodeInterpret *__cdecl fn_p_stCollisionWP(
    HIE_tdstSuperObject *p_SuperObjPerso, AI_tdstNodeInterpret *p_stTree, long *p_lValue)
{
	AI_tdstGetSetParam			stParam;
	HIE_tdstSuperObject			*hPerso;
	WP_tdhWayPoint				hWP;
	MTH3D_tdstVector			stPosWP;

	M_EvalNextParameter( &stParam );
	hPerso = (HIE_tdstSuperObject *) M_GetSetParam_p_stSupObjValue( &stParam );

	M_EvalNextParameter( &stParam );
	hWP = (WP_tdhWayPoint) M_GetSetParam_WayPointValue( &stParam );

	if ((hPerso==0) || (hWP==0))
	{
		*p_lValue=FALSE;
		return(p_stTree);
	}

	if (hWP->m_hSuperObject)
	{
		WP_fnv_WayPoint_ComputeLocation(hWP,&stPosWP);

		MTH3D_M_vSubVector(&stPosWP,&stPosWP,POS_fn_vGetTranslationVectorReadOnly(hPerso->hGlobalMatrix));
	}
	else

		MTH3D_M_vSubVector(&stPosWP,&hWP->m_stVertex, POS_fn_vGetTranslationVectorReadOnly(hPerso->hGlobalMatrix));

	if MTH_M_bGreaterZero(MTH_M_xSub(MTH3D_M_xSqrVector(&stPosWP),MTH_M_xSqr(hWP->m_xRadius)))
		*p_lValue=FALSE;
	else
		*p_lValue=TRUE;

    return(p_stTree);
}

#define eCond_CollideWithThisPerso  0x21

/* 0x0048A430 */
AI_tdstNodeInterpret *__cdecl fn_p_stReportThisPerso(
    HIE_tdstSuperObject *p_SuperObjPerso, AI_tdstNodeInterpret *p_stTree, long *p_lValue)
{
    unsigned long        eCondId = AI_M_eCondIdInterpret(p_stTree - 1);
    HIE_tdstSuperObject *hNewSuperObjPerso;
    HIE_tdstSuperObject *hPersoSupObj;
    AI_tdstGetSetParam   stParam;
    DNM_tdstReport      *p_stDynamReport;
    void                *hObstacleObj;

    fn_vGetUltraOperatorPerso(
        AI_M_pfn_fn_ucGetConditionUltraOperator(eCondId), p_SuperObjPerso, &hNewSuperObjPerso);

    p_stTree     = fn_p_stEvalTree(p_SuperObjPerso, p_stTree, &stParam);
    hPersoSupObj = AI_M_GetSetParam_p_stSupObjValue(&stParam);

    p_stDynamReport = AI_M_pfn_fn_pstGetDNMReport(hNewSuperObjPerso);
    if (p_stDynamReport == NULL) { *p_lValue = (long)FALSE; return p_stTree; }

    if (   ((DNM_M_ulReportGetSurfaceState(p_stDynamReport) & C_WOT_ulMobile) == C_WOT_ulMobile)
        && ((hObstacleObj = DNM_M_p_stObstacleGetObject(DNM_M_p_stReportGetCharacter(p_stDynamReport))) != (void *)0) )
    {
        {
            /* 0x00437A20 */
            HIE_tdstSuperObject *hFather = MEC_fn_hGetFatherActor((HIE_tdstSuperObject *)hObstacleObj);
            *p_lValue = (long)(hFather == hPersoSupObj);
        }
        return p_stTree;
    }

    *p_lValue = (long)FALSE;
    return p_stTree;
}

/* 0x0048A4F0 */
AI_tdstNodeInterpret *__cdecl fn_p_stReportOnZDMSurfaceCondition(
    HIE_tdstSuperObject *p_SuperObjPerso, AI_tdstNodeInterpret *p_stTree, long *p_lValue)
{
    AI_tdstGetSetParam   stParam;
    DNM_tdstReport      *p_stDynamReport;
    unsigned long        ulSurfState;
    GMT_tdxMask          xTestMask;
    long                 eCondId = AI_M_eCondIdInterpret(p_stTree - 1);

    HIE_tdstSuperObject *hNewSuperObjPerso;
    fn_vGetUltraOperatorPerso(fn_ucGetConditionUltraOperator(eCondId), p_SuperObjPerso, &hNewSuperObjPerso);

    if ((p_stDynamReport = fn_pstGetDNMReport(hNewSuperObjPerso)) == NULL)
    {
        *p_lValue = (long)FALSE;
        return p_stTree;
    }

    ulSurfState = DNM_M_ulReportGetSurfaceState(p_stDynamReport);

    M_EvalNextParameter(&stParam);
    xTestMask = (GMT_tdxMask) M_GetSetParam_xMaskValue(&stParam);

    switch (eCondId)
    {
    case eCond_ZDMCollideWithGround:
        if ((ulSurfState & C_WOT_ulGround) == C_WOT_ulGround)
            *p_lValue = (long)fn_bTestTypeOfGameMaterial(DNM_M_hObstacleGetMyMaterial(DNM_M_p_stReportGetGround(p_stDynamReport)), xTestMask);
        else
            *p_lValue = (long)FALSE;
        return p_stTree;
        break;

#if !defined(_AI_EXCLUDE_NEVER_USED_)
    case eCond_ZDMCollideWithWall:
        if ((ulSurfState & C_WOT_ulWall) == C_WOT_ulWall)
            *p_lValue = (long)fn_bTestTypeOfGameMaterial(DNM_M_hObstacleGetMyMaterial(DNM_M_p_stReportGetWall(p_stDynamReport)), xTestMask);
        else
            *p_lValue = (long)FALSE;
        return p_stTree;
        break;

    case eCond_ZDMCollideWithNothing:
        if (ulSurfState != C_WOT_ulNoObstacle)
        {
            if (   ((ulSurfState & C_WOT_ulGround) == C_WOT_ulGround)
                  )
                *p_lValue = (long)!fn_bTestTypeOfGameMaterial(DNM_M_hObstacleGetMyMaterial(DNM_M_p_stReportGetGround(p_stDynamReport)), xTestMask);
            else if (   ((ulSurfState & C_WOT_ulWall) == C_WOT_ulWall)   )
                *p_lValue = (long)!fn_bTestTypeOfGameMaterial(DNM_M_hObstacleGetMyMaterial(DNM_M_p_stReportGetWall(p_stDynamReport)), xTestMask);
      else if ((ulSurfState & C_WOT_ulCeiling) == C_WOT_ulCeiling)
                *p_lValue = (long)!fn_bTestTypeOfGameMaterial(DNM_M_hObstacleGetMyMaterial(DNM_M_p_stReportGetCeil(p_stDynamReport)), xTestMask);
            else
                *p_lValue = (long)TRUE;
        }
        else
            *p_lValue = (long)TRUE;
        return p_stTree;
        break;

    case eCond_ZDMCollideWithCeiling:
        if ((ulSurfState & C_WOT_ulCeiling) == C_WOT_ulCeiling)
            *p_lValue = (long)fn_bTestTypeOfGameMaterial(DNM_M_hObstacleGetMyMaterial(DNM_M_p_stReportGetCeil(p_stDynamReport)), xTestMask);
        else
            *p_lValue = (long)FALSE;
        return p_stTree;
        break;
#endif

    default:
        break;
    }
    *p_lValue = (long) FALSE;
    return p_stTree;
}

#define eCond_UserEvent_IsSet   0x2a
#define eCond_UserEvent_IsSet2  0x2b

/* 0x00418970 */
extern long __cdecl fn_b3dDataGetUserEventFlag(char *h_3dData, unsigned char ucFlagIndex,
                                               MTH3D_tdstVector *p_stPosition);

/* 0x0048AEA0 */
AI_tdstNodeInterpret *__cdecl fn_p_st_UserEvent_IsSet(
    HIE_tdstSuperObject *p_SuperObjPerso, AI_tdstNodeInterpret *p_stTree, long *p_lValue)
{
    AI_tdstGetSetParam  stParam;
    unsigned char       ucEventIndex;
    unsigned char       ucVarId = 0;
    long                eCondId = AI_M_eCondIdInterpret(p_stTree - 1);

    switch (eCondId)
    {
    default:
        return p_stTree;

    case eCond_UserEvent_IsSet:
    case eCond_UserEvent_IsSet2:
        p_stTree     = fn_p_stEvalTree(p_SuperObjPerso, p_stTree, &stParam);
        ucEventIndex = (unsigned char)M_GetSetParam_lValue(&stParam);

        if (eCondId == eCond_UserEvent_IsSet2)
        {
            ucVarId  = AI_M_uVarIdInterpret(p_stTree);
            p_stTree = p_stTree + 1;
        }
        break;
    }

    if (ucEventIndex <= 7)
    {
        MTH3D_tdstVector stPosition;
        char            *h_3dData = (char *)((struct tdstEngineObject_ *)(HIE_fn_hGetSuperObjectObject(p_SuperObjPerso)))->h_3dData;

        *p_lValue = fn_b3dDataGetUserEventFlag(h_3dData, ucEventIndex, &stPosition);

        if (*p_lValue != 0 && eCondId == eCond_UserEvent_IsSet2)
        {

            M_Full_GetSetParam_p_stVertex(&stParam, &stPosition);
            if (fn_ucSetDsgVar(ucVarId, 0, AI_M_stGetMindOfSuperObj(p_SuperObjPerso), &stParam) == C_INVALID_SET)
                *p_lValue = (long)FALSE;
        }
    }
    else
        *p_lValue = (long)FALSE;

    return p_stTree;
}

#define eCond_MagnetIsActivated  0x71

/* 0x00488100 */
AI_tdstNodeInterpret *__cdecl fn_p_stIsMorphing(
    HIE_tdstSuperObject *p_SuperObjPerso, AI_tdstNodeInterpret *p_stTree, long *p_lValue)
{
    unsigned long        eCondId = AI_M_eCondIdInterpret(p_stTree - 1);
    HIE_tdstSuperObject *hNewSuperObjPerso;
    AI_tdstGetSetParam   stValue;

    fn_vGetUltraOperatorPerso(
        AI_M_pfn_fn_ucGetConditionUltraOperator(eCondId), p_SuperObjPerso, &hNewSuperObjPerso);

    p_stTree  = fn_p_stEvalTree(p_SuperObjPerso, p_stTree, &stValue);
    *p_lValue = (long)(MOR_fn_bIsChannelMorphed(
                           p_SuperObjPerso, M_GetSetParam_lValue(&stValue)) & 0xff);

    return p_stTree;
}

/* 0x00488480 */
AI_tdstNodeInterpret *__cdecl fn_p_stMagnetActivated(
    HIE_tdstSuperObject *p_SuperObjPerso, AI_tdstNodeInterpret *p_stTree, long *p_lValue)
{
    unsigned long        eCondId = AI_M_eCondIdInterpret(p_stTree - 1);
    HIE_tdstSuperObject *hNewSuperObjPerso;
    MS_tdxHandleToMSMagnet hMagnet;

    fn_vGetUltraOperatorPerso(
        AI_M_pfn_fn_ucGetConditionUltraOperator(eCondId), p_SuperObjPerso, &hNewSuperObjPerso);

    hMagnet = (MS_tdxHandleToMSMagnet)((struct tdstEngineObject_ *)(HIE_fn_hGetSuperObjectObject(hNewSuperObjPerso)))->h_MSMagnet;
    if (hMagnet != NULL)
    {
        switch (fn_lMSMagnetGetStatus(hMagnet))   /* 0x00415210 */
        {
        case 0:
        case 3:
            *p_lValue = (long)FALSE;
            break;
        case 1:
        case 2:
            *p_lValue = (long)TRUE;
            break;
        default:
            break;
        }
    }
    return p_stTree;
}

#define eCond_IsNotInCollWithMap              0x72
#define eCond_IsNotInCollWithProjectile       0x73
#define eCond_IsNotInCollWithSecondCharacter  0x74
#define eCond_IsNotInCollWithMainCharacter    0x75
#define eCond_IsNotInCollWithOtherSectors     0x76

/* 0x0048B980 */
AI_tdstNodeInterpret *__cdecl AI_fn_p_stIsNotInCollisionWith(
    HIE_tdstSuperObject *p_SuperObjPerso, AI_tdstNodeInterpret *p_stTree, long *p_lValue)
{
    unsigned long        eCondId = AI_M_eCondIdInterpret(p_stTree - 1);
    HIE_tdstSuperObject *hNewSuperObjPerso;
    void                *hCollSet;

    fn_vGetUltraOperatorPerso(
        AI_M_pfn_fn_ucGetConditionUltraOperator(eCondId), p_SuperObjPerso, &hNewSuperObjPerso);

    hCollSet = ((struct tdstEngineObject_ *)(HIE_fn_hGetSuperObjectObject(hNewSuperObjPerso)))->h_CollSet;

    switch (eCondId)
    {
    case eCond_IsNotInCollWithMap:
        *p_lValue = (long)(unsigned char)fn_bCollSetGetCharacterNoCollisionWithMap(hCollSet);
        break;
    case eCond_IsNotInCollWithProjectile:
        *p_lValue = (long)(unsigned char)fn_bCollSetGetCharacterNoCollisionWithProjectile(hCollSet);
        break;
    case eCond_IsNotInCollWithSecondCharacter:
        *p_lValue = (long)(unsigned char)fn_bCollSetGetCharacterNoCollisionWithSecondaryCharacter(hCollSet);
        break;
    case eCond_IsNotInCollWithMainCharacter:
        *p_lValue = (long)(unsigned char)fn_bCollSetGetCharacterNoCollisionWithMainCharacter(hCollSet);
        break;
    case eCond_IsNotInCollWithOtherSectors:
        *p_lValue = (long)(unsigned char)fn_bCollSetGetCharacterNoCollisionWithOtherSectors(hCollSet);
        break;
    default:
        break;
    }
    return p_stTree;
}

static __inline void *fn_h3dDataGetFamily(MS_tdxHandleTo3dData h_3dData)
{
    return(h_3dData->h_Family);
}

#define eCond_IsOfFamily  0x77
#define eCond_IsOfModel   0x78

/* 0x0048BA50 */
AI_tdstNodeInterpret *__cdecl AI_fn_p_stIsOfFamilyOrModel(
    HIE_tdstSuperObject *p_SuperObjPerso, AI_tdstNodeInterpret *p_stTree, long *p_lValue)
{
    unsigned long        eCondId = AI_M_eCondIdInterpret(p_stTree - 1);
    HIE_tdstSuperObject *hNewSuperObjPerso;
    AI_tdstGetSetParam   stParam;
    char                *engineObj;
    long                 lRef;

    fn_vGetUltraOperatorPerso(
        AI_M_pfn_fn_ucGetConditionUltraOperator(eCondId), p_SuperObjPerso, &hNewSuperObjPerso);

    p_stTree = fn_p_stEvalTree(p_SuperObjPerso, p_stTree, &stParam);
    lRef     = stParam.uParam.lValue;

    switch (eCondId)
    {
    case eCond_IsOfFamily:
    {

        *p_lValue = (long)((long)fn_h3dDataGetFamily(M_GetMSHandle(hNewSuperObjPerso,3dData)) == lRef);
        return p_stTree;
    }
    case eCond_IsOfModel:
    {
        MS_tdxHandleToBrain h_Brain;
        h_Brain = ((struct tdstEngineObject_ *)(HIE_fn_hGetSuperObjectObject(hNewSuperObjPerso)))->h_Brain;
        *p_lValue = (long)((h_Brain != (MS_tdxHandleToBrain)0) && ((long)fn_p_stBrainGetMind(h_Brain)->p_stAIModel == lRef));
        return p_stTree;
    }
    default:
        AI_M_pfn_M_AIFatalError(0x2b);
        break;
    }
    return p_stTree;
}

/* 0x0042E1C0 */
extern unsigned char __cdecl IPT_fn_bAJoypadConnected(void);
#define AI_M_pfn_IPT_AJoypadIsConnected     IPT_fn_bAJoypadConnected

extern int __cdecl SND_fn_bGetStereoSound(void);
#define AI_M_pfn_SND_IsInStereoMode         SND_fn_bGetStereoSound
extern int __cdecl SND_fn_bIsMusicPlayingSound(void);
#define AI_M_pfn_SND_IsMusicPlaying         SND_fn_bIsMusicPlayingSound

/* 0x0048BB10 */
AI_tdstNodeInterpret *__cdecl AI_fn_p_stIsJoypadConnected(
    HIE_tdstSuperObject *p_SuperObjPerso, AI_tdstNodeInterpret *p_stTree, long *p_lValue)
{
    (void)p_SuperObjPerso;
    *p_lValue = (long)(AI_M_pfn_IPT_AJoypadIsConnected() & 0xff);
    return p_stTree;
}

/* 0x0048BB30 */
AI_tdstNodeInterpret *__cdecl AI_fn_p_stIsKeyJustPressed(
    HIE_tdstSuperObject *p_SuperObjPerso, AI_tdstNodeInterpret *p_stTree, long *p_lValue)
{
    (void)p_SuperObjPerso;
    *p_lValue = (long)(IPT_fn_bAKeyJustPressed() & 0xff);
    return p_stTree;
}

/* 0x0048BB50 */
AI_tdstNodeInterpret *__cdecl AI_fn_p_stIsButtonPadJustPressed(
    HIE_tdstSuperObject *p_SuperObjPerso, AI_tdstNodeInterpret *p_stTree, long *p_lValue)
{
    (void)p_SuperObjPerso;
    *p_lValue = (long)(IPT_fn_bAButtonPadJustPressed() & 0xff);
    return p_stTree;
}

/* 0x0048BB70 */
AI_tdstNodeInterpret *__cdecl AI_fn_p_stIsInDemoMode(
    HIE_tdstSuperObject *p_SuperObjPerso, AI_tdstNodeInterpret *p_stTree, long *p_lValue)
{
    (void)p_SuperObjPerso;
    *p_lValue = (long)AI_GES_xDemoMode;
    return p_stTree;
}

/* 0x0048BB90 */
AI_tdstNodeInterpret *__cdecl fn_p_stIsInStereoMode(
    HIE_tdstSuperObject *p_SuperObjPerso, AI_tdstNodeInterpret *p_stTree, long *p_lValue)
{
    (void)p_SuperObjPerso;
    *p_lValue = (long)AI_M_pfn_SND_IsInStereoMode();
    return p_stTree;
}

/* 0x0048BBA0 */
AI_tdstNodeInterpret *__cdecl fn_p_stIsMusicPlaying(
    HIE_tdstSuperObject *p_SuperObjPerso, AI_tdstNodeInterpret *p_stTree, long *p_lValue)
{
    (void)p_SuperObjPerso;
    *p_lValue = (long)AI_M_pfn_SND_IsMusicPlaying();
    return p_stTree;
}

/* 0x0048BBB0 */
AI_tdstNodeInterpret *__cdecl AI_fn_p_stIsSharpnessMax(
    HIE_tdstSuperObject *p_SuperObjPerso, AI_tdstNodeInterpret *p_stTree, long *p_lValue)
{
    (void)p_SuperObjPerso;
    *p_lValue = 1;
    return p_stTree;
}

AI_tdstNodeInterpret *__cdecl AI_fn_p_stCameraCondition(
    HIE_tdstSuperObject *p_SuperObjPerso, AI_tdstNodeInterpret *p_stTree, long *p_lValue)
{

    HIE_tdstSuperObject         *hCamera;
    MS_tdxHandleToCineinfo       hCineInfo;
    MS_tdxHandleToInternalCineinfo hIntCineInfo;
    AI_tdstGetSetParam           stValue;
    long                         lCurrentValue;
    long                         lViewport;
    unsigned long                eCondId = AI_M_eCondIdInterpret(p_stTree - 1);

    fn_vGetUltraOperatorPerso(
        AI_M_pfn_fn_ucGetConditionUltraOperator(eCondId), p_SuperObjPerso, &hCamera);

    hCineInfo = ((struct tdstEngineObject_ *)(HIE_fn_hGetSuperObjectObject(hCamera)))->h_Cineinfo;

    p_stTree = fn_p_stEvalTree(p_SuperObjPerso, p_stTree, &stValue);
    lCurrentValue = stValue.uParam.lValue;

    if (lCurrentValue == 1)
    {
        if (hCineInfo->ucVolIAFlags & C_VolIAFlags_CurrentAlreadyCopiedInWork)
        {
            hIntCineInfo = hCineInfo->hWork;
        }
        else
        {
            hIntCineInfo = hCineInfo->hCurrent;
        }
    }
    else if (lCurrentValue == -1)
    {
        hIntCineInfo = hCineInfo->hInit;
    }
    else
    {
        if (hCineInfo->ucPerIAFlags & C_PerIAFlags_VisibilityCopied)
        {
            hIntCineInfo = hCineInfo->hVisibility;
        }
        else
        {
            hIntCineInfo = hCineInfo->hCurrent;
        }
    }

    switch (eCondId)
    {

    case 0x80:
        *p_lValue = (long)hIntCineInfo->bIsActive;
        return p_stTree;

    case 0x81:
        p_stTree = fn_p_stEvalTree(p_SuperObjPerso, p_stTree, &stValue);
        lViewport = stValue.uParam.lValue;
        *p_lValue = (long)(lViewport == (long)hIntCineInfo->eTypeOfViewport);
        return p_stTree;

    case 0x82:
        *p_lValue = (long)(hIntCineInfo->uwIAFlags & 0x0001);
        return p_stTree;
    case 0x83:
        *p_lValue = (long)(hIntCineInfo->uwIAFlags & 0x0002);
        return p_stTree;
    case 0x84:
        *p_lValue = (long)(hIntCineInfo->uwIAFlags & 0x0004);
        return p_stTree;
    case 0x85:
        *p_lValue = (long)(hIntCineInfo->uwIAFlags & 0x0008);
        return p_stTree;
    case 0x86:
        *p_lValue = (long)(hIntCineInfo->uwIAFlags & 0x0010);
        return p_stTree;
    case 0x87:
        *p_lValue = (long)(hIntCineInfo->uwIAFlags & 0x0020);
        return p_stTree;
    case 0x88:
        *p_lValue = (long)(hIntCineInfo->uwIAFlags & 0x0040);
        return p_stTree;
    case 0x89:
        *p_lValue = (long)(hIntCineInfo->uwIAFlags & 0x0080);
        return p_stTree;

    case 0x8a:
        *p_lValue = (long)(hIntCineInfo->uwDNMFlags & 0x0001);
        return p_stTree;
    case 0x8b:
        *p_lValue = (long)(hIntCineInfo->uwDNMFlags & 0x0002);
        return p_stTree;
    case 0x8c:
        *p_lValue = (long)(hIntCineInfo->uwDNMFlags & 0x0004);
        return p_stTree;
    case 0x8d:
        *p_lValue = (long)(hIntCineInfo->uwDNMFlags & 0x0008);
        return p_stTree;
    case 0x8e:
        *p_lValue = (long)(hIntCineInfo->uwDNMFlags & 0x0010);
        return p_stTree;
    case 0x8f:
        *p_lValue = (long)(hIntCineInfo->uwDNMFlags & 0x0020);
        return p_stTree;
    case 0x90:
        *p_lValue = (long)(hIntCineInfo->uwDNMFlags & 0x0040);
        return p_stTree;
    case 0x91:
        *p_lValue = (long)(hIntCineInfo->uwDNMFlags & 0x0080);
        return p_stTree;

    case 0x93:
        *p_lValue = (long)(hIntCineInfo->uwIAFlags & 0x1000);
        return p_stTree;
    case 0x94:
        *p_lValue = (long)(hIntCineInfo->uwIAFlags & 0x4000);
        return p_stTree;
    case 0x92:
        *p_lValue = (long)(hIntCineInfo->uwIAFlags & 0x2000);
        return p_stTree;
    default:
        break;
    }
    AI_M_pfn_M_AIFatalError(0x2b);
    return p_stTree;
}

/* 0x004897B0 */
ACP_tdxBool __cdecl fn_bTestTypeOfGameMaterial(GMT_tdxHandleToGameMaterial hGameMat,
                                               GMT_tdxMask xTestMask)
{
    GMT_tdxHandleToCollideMaterial hCollMat;
    GMT_tdxMask                    xCollMask;

    if (!GMT_fn_b_ulIsValid(hGameMat))
        return FALSE;
    hCollMat = GMT_fn_hGetCollideMaterial(hGameMat);
    if (hCollMat == (GMT_tdxHandleToCollideMaterial)GMT_C_InvalidCollideMaterial)
        return FALSE;
    xCollMask = GMT_fn_hGetCollideMaterialIdentifier(hCollMat);
    return ((xCollMask & xTestMask) == xTestMask);
}

/* 0x00489800 */
ACP_tdxBool __cdecl fn_bIsTypeOfGMT(HIE_tdstSuperObject *p_SuperObjPerso, GMT_tdxMask xTestMask)
{
    DNM_tdstReport *p_stDynamReport;

    p_stDynamReport = fn_pstGetDNMReport(p_SuperObjPerso);
    if (p_stDynamReport == NULL)
        return FALSE;
    return fn_bTestTypeOfGameMaterial(DNM_M_hObstacleGetCollidedMaterial(DNM_M_p_stReportGetGround(p_stDynamReport)), xTestMask);
}

/* 0x0048BBC0 */
extern long __cdecl fn_lGetActionParamlValue(unsigned char ucN, HIE_tdstSuperObject *p_SuperObjPerso);
extern void CAM_fn_vComputeProjection(MTH3D_tdstVector *_p_stDest, MTH3D_tdstVector *_p_stSrc, MTH3D_tdstVector *_p_stAxis);
extern MTH_tdxReal CAM_fn_xComputeRotation(MTH3D_tdstVector *_p_stDest, MTH3D_tdstVector *_p_stSrc, MTH3D_tdstVector *_p_stAxis);
extern MTH3D_tdstVector *MTH_p_stRotationMulVector(MTH_tdstRotation *_p_stRotation, MTH3D_tdstVector *_p_stSource, MTH3D_tdstVector *_p_stResult);
