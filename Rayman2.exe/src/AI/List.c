#include <string.h>
#include "AI/List.h"
#include "AI/AI_types.h"
#include "ERM/CpaError.h"
#include "ACP/ACP_types.h"
#include "MTH/MTH_flt.h"

#define LIST_C_ucEndOfList   255
#define LIST_C_ucSizeOfMaximalList 30
#define LIST_M_ucNbElt(p)    (((tdstList *)(p))->ucNbElt)
#define LIST_M_d_TabElt(p)   ((void **)((tdstList *)(p))->d_TabElt)
#define LIST_M_Min(a,b)      (((a)>(b)) ? (b) : (a))

#include "GAM/GAM_types.h"
#include "GAM/PlayAnim.h"
#include "AI/Brain.h"

#define LIST_M_ucMaxSize(p)  (((tdstList *)(p))->ucMaxSize)

extern void __cdecl fn_vOnAIErm_M_UpdateLastError(unsigned short FatalErrorNum, char *szMsg);

#define E_uwAIFatalPersoListFull  0x0e   /* 0x00483144 */

/* 0x0040DB00 */
extern void fn_vGetAllObjectsInZdd(void *_hSuperObj, unsigned char _ucModuleOrCharacter,
                                   unsigned char _ucChannelNumber, void **_p_hSuperObjTable,
                                   unsigned short *_p_uwNumber);

void *g_a_hPersoInZone[30];

extern void fn_vGetAllCharactersInContactWithThisZde(void *_hSuperObj, unsigned char _ucModuleOrCharacter,
                                                     unsigned char _ucChannelNumber, void **_hSuperObjTable,
                                                     unsigned short *_p_uwNumber, unsigned short _wMaterialMask);
extern void fn_vThisTypeVsThisType(void *_hCharacter, unsigned short _wCharacterMask,
                                   unsigned short _wOthersMask, void **_p_hObjectTable,
                                   unsigned short *_p_uwNumber);

/* 0x00483120 */
void __cdecl fn_vPutInList(void *p_List, void *p_Perso)
{
    if (LIST_M_ucNbElt(p_List) < LIST_M_ucMaxSize(p_List))
    {
        LIST_M_d_TabElt(p_List)[LIST_M_ucNbElt(p_List)++] = p_Perso;
    }
    else
    {
        M_AIFatalError(E_uwAIFatalPersoListFull);
    }
}

/* 0x00483150 */
void __cdecl fn_vInsertPersoAtPositionInList(void *p_List, void *p_Perso, unsigned char ucPosition)
{
    unsigned char ucI;

    ucPosition = (unsigned char)M_Min(LIST_M_ucNbElt(p_List) + 1, ucPosition);

    if (LIST_M_ucNbElt(p_List) < LIST_M_ucMaxSize(p_List))
    {
        for (ucI = LIST_M_ucNbElt(p_List); ucI > ucPosition; ucI--)
            LIST_M_d_TabElt(p_List)[ucI] = LIST_M_d_TabElt(p_List)[ucI - 1];
        LIST_M_d_TabElt(p_List)[ucPosition] = p_Perso;
        LIST_M_ucNbElt(p_List)++;
    }
    else
    {
        M_AIFatalError(E_uwAIFatalPersoListFull);
    }
}

/* 0x004831D0 */
unsigned char __cdecl fn_bFindPersoInList(void *p_List, void *p_Perso)
{
    unsigned char ucPosition;

    if (LIST_M_ucNbElt(p_List) == 0)
        return 0;

    for (ucPosition = 0;
         (ucPosition < LIST_M_ucNbElt(p_List)) && (LIST_M_d_TabElt(p_List)[ucPosition] != p_Perso);
         ucPosition++)
    {
    }

    return (unsigned char)(ucPosition < LIST_M_ucNbElt(p_List));
}

/* 0x00483210 */
unsigned char __cdecl fn_bFindModelInList(void *p_List, void *_p_stModel)
{
    unsigned char ucPosition;

    if (LIST_M_ucNbElt(p_List) == 0)
        return 0;

    for (ucPosition = 0;
         (ucPosition < LIST_M_ucNbElt(p_List)) &&
             (AI_M_p_stGetAIModel(M_pstGetMindOfBrain(M_GetMSHandle((HIE_tdstSuperObject *)LIST_M_d_TabElt(p_List)[ucPosition],Brain))) != _p_stModel);
         ucPosition++)
    {
    }

    return (unsigned char)(ucPosition < LIST_M_ucNbElt(p_List));
}

/* 0x00483260 */
unsigned char __cdecl fn_bFindFamilyInList(void *p_List, void *hFamily)
{
    unsigned char ucPosition;

    if (LIST_M_ucNbElt(p_List) == 0)
        return 0;

    for (ucPosition = 0;
         (ucPosition < LIST_M_ucNbElt(p_List)) &&

             (M_GetMSHandle((HIE_tdstSuperObject *)LIST_M_d_TabElt(p_List)[ucPosition],3dData)->h_Family != hFamily);
         ucPosition++)
    {
    }

    return (unsigned char)(ucPosition < LIST_M_ucNbElt(p_List));
}

/* 0x004832B0 */
char __cdecl fn_bFindPersoAndDeleteInList(void *p_List, void *p_Perso)
{
    unsigned char ucI;
    unsigned char ucPosition;

    for (ucPosition = 0;
         (ucPosition < LIST_M_ucNbElt(p_List)) && (LIST_M_d_TabElt(p_List)[ucPosition] != p_Perso);
         ucPosition++) {}

    if (LIST_M_d_TabElt(p_List)[ucPosition] == p_Perso)
    {
        for (ucI = ucPosition; ucI <= LIST_M_ucNbElt(p_List) - 2; ucI++)
            LIST_M_d_TabElt(p_List)[ucI] = LIST_M_d_TabElt(p_List)[ucI + 1];
        LIST_M_ucNbElt(p_List)--;
        return 1;
    }
    else
        return 0;
}

/* 0x00483340 */
void __cdecl fn_vDeletePersoAtPositionInList(void *p_List, unsigned char ucPosition)
{
    unsigned char ucI;

    if (ucPosition == LIST_C_ucEndOfList)
        ucPosition = (unsigned char)(LIST_M_ucNbElt(p_List) - 1);

    if (ucPosition <= LIST_M_ucNbElt(p_List) - 1)
    {
        for (ucI = ucPosition; ucI <= LIST_M_ucNbElt(p_List) - 2; ucI++)
            LIST_M_d_TabElt(p_List)[ucI] = LIST_M_d_TabElt(p_List)[ucI + 1];
        LIST_M_ucNbElt(p_List)--;
    }
    else
    {
        ERM_fn_vUpdateLastError(0x42, g_ucAIModuleId, 0, 0x10, -1, 0xff, 0xff, (char *)0);
    }
}

/* 0x004833D0 */
void __cdecl fn_vSortListByFamily(void *p_List, void *hFamily)
{
    unsigned char ucI, ucJ = 0;
    void *a_hSupObjElt[30];
    void *hSupObjElt;
    void *hFamilyElt;

    for (ucI = 0; ucI < LIST_M_ucNbElt(p_List); ucI++)
    {
        hSupObjElt = LIST_M_d_TabElt(p_List)[ucI];

        hFamilyElt = M_GetMSHandle((HIE_tdstSuperObject *)hSupObjElt,3dData)->h_Family;
        if (hFamilyElt == hFamily)
            a_hSupObjElt[ucJ++] = hSupObjElt;
    }

    LIST_M_ucNbElt(p_List) = 0;
    for (ucI = 0; ucI < ucJ; ucI++)
        fn_vPutInList(p_List, a_hSupObjElt[ucI]);
}

/* 0x00483460 */
void __cdecl fn_vSortListByModel(void *p_stList, void *_p_stModel)
{
    void         *hPerso;
    unsigned char ucPos = 0;
    void         *a_hSupObjElt[30];
    unsigned char ucI, ucJ = 0;
    MS_tdxHandleToBrain hBrain;

    hPerso = fn_p_stGetPersoInList(p_stList, ucPos);
    while (hPerso)
    {
        hBrain = M_GetMSHandle((HIE_tdstSuperObject *)hPerso,Brain);
        if (hBrain && (AI_M_p_stGetAIModel(M_pstGetMindOfBrain(hBrain)) == _p_stModel))
            a_hSupObjElt[ucJ++] = hPerso;

        ucPos++;
        hPerso = fn_p_stGetPersoInList(p_stList, ucPos);
    }

    LIST_M_ucNbElt(p_stList) = 0;
    for (ucI = 0; ucI < ucJ; ucI++)
        fn_vPutInList(p_stList, a_hSupObjElt[ucI]);
}

/* 0x00483510 */
void __cdecl fn_vUnionList(void *p_ListA, void *p_ListB, void *p_ListC)
{
    unsigned char ucI, ucJ;
    void *p_Perso;

    for (ucI = 0; ucI < LIST_M_ucNbElt(p_ListB); ucI++)
        fn_vPutInList(p_ListA, LIST_M_d_TabElt(p_ListB)[ucI]);

    for (ucI = 0; ucI < LIST_M_ucNbElt(p_ListC); ucI++)
    {
        p_Perso = LIST_M_d_TabElt(p_ListC)[ucI];
        for (ucJ = 0;
             (ucJ < LIST_M_ucNbElt(p_ListB)) && (LIST_M_d_TabElt(p_ListB)[ucJ] != p_Perso);
             ucJ++) {}
        if (LIST_M_d_TabElt(p_ListB)[ucJ] != p_Perso)
            fn_vPutInList(p_ListA, p_Perso);
    }
}

/* 0x004835D0 */
void __cdecl fn_vInterList(void *p_ListA, void *p_ListB, void *p_ListC)
{
    unsigned char ucB, ucC;
    void *p_Perso;

    for (ucB = 0; ucB < LIST_M_ucNbElt(p_ListB); ucB++)
    {
        p_Perso = LIST_M_d_TabElt(p_ListB)[ucB];
        for (ucC = 0; ucC < LIST_M_ucNbElt(p_ListC); ucC++)
        {
            if (LIST_M_d_TabElt(p_ListC)[ucC] == p_Perso)
            {
                fn_vPutInList(p_ListA, p_Perso);
                break;
            }
        }
    }
}

/* 0x00483640 */
void __cdecl fn_vDiffList(void *p_ListA, void *p_ListB, void *p_ListC)
{
    unsigned char ucB, ucC;
    void *p_Perso;

    for (ucB = 0; ucB < LIST_M_ucNbElt(p_ListB); ucB++)
    {
        p_Perso = LIST_M_d_TabElt(p_ListB)[ucB];
        for (ucC = 0;
             (ucC < LIST_M_ucNbElt(p_ListC)) && (LIST_M_d_TabElt(p_ListC)[ucC] != p_Perso);
             ucC++) {}
        if (LIST_M_d_TabElt(p_ListC)[ucC] != p_Perso)
            fn_vPutInList(p_ListA, p_Perso);
    }
}

/* 0x004836C0 */
void __cdecl fn_vAddList(void *p_ListA, void *p_ListB)
{
    unsigned char ucA, ucB, ucSizeA;
    void *hPerso;

    ucSizeA = LIST_M_ucNbElt(p_ListA);
    for (ucB = 0; ucB < LIST_M_ucNbElt(p_ListB); ucB++)
    {
        hPerso = LIST_M_d_TabElt(p_ListB)[ucB];
        for (ucA = 0; (ucA < ucSizeA) && LIST_M_d_TabElt(p_ListA)[ucA] != hPerso; ucA++) {}
        if (LIST_M_d_TabElt(p_ListA)[ucA] != hPerso)
            fn_vPutInList(p_ListA, hPerso);
    }
}

/* 0x00483740 */
void __cdecl fn_vAffectZddInList(void *p_List, void *p_stPerso, unsigned char ucModulOrCharact,
                                 ACP_tdxIndex xZoneId)
{
    unsigned short uwI, uwNbDetectedPerso;

    LIST_M_ucNbElt(p_List) = 0;

    uwNbDetectedPerso = LIST_M_ucMaxSize(p_List);
    fn_vGetAllObjectsInZdd(p_stPerso, ucModulOrCharact, (unsigned char)xZoneId, g_a_hPersoInZone,
                           &uwNbDetectedPerso);

    for (uwI = 0; uwI < uwNbDetectedPerso; uwI++)
        fn_vPutInList(p_List, g_a_hPersoInZone[uwI]);
}

/* 0x004837A0 */
void __cdecl fn_vAffectZdeInList(void *p_List, void *p_stPerso, unsigned char ucModulOrCharact,
                                 ACP_tdxIndex xZoneId, unsigned short xMask)
{
    unsigned short uwI, uwNbDetectedPerso;

    LIST_M_ucNbElt(p_List) = 0;

    uwNbDetectedPerso = LIST_M_ucMaxSize(p_List);
    fn_vGetAllCharactersInContactWithThisZde(p_stPerso, ucModulOrCharact, (unsigned char)xZoneId,
                                             g_a_hPersoInZone, &uwNbDetectedPerso, (unsigned short)xMask);

    for (uwI = 0; uwI < uwNbDetectedPerso; uwI++)
        fn_vPutInList(p_List, g_a_hPersoInZone[uwI]);
}

/* 0x00483810 */
void __cdecl fn_vAffectTypeZdeWithTypeZdeInList(void *p_List, void *p_stPerso, unsigned short xPersoMask,
                                                unsigned short xOthersMask)
{
    unsigned short uwI, uwNbDetectedPerso;

    LIST_M_ucNbElt(p_List) = 0;

    uwNbDetectedPerso = LIST_M_ucMaxSize(p_List);
    fn_vThisTypeVsThisType(p_stPerso, xPersoMask, xOthersMask, g_a_hPersoInZone, &uwNbDetectedPerso);

    for (uwI = 0; uwI < uwNbDetectedPerso; uwI++)
        fn_vPutInList(p_List, g_a_hPersoInZone[uwI]);
}

/* 0x00483870 */
unsigned char __cdecl fn_ucAtLeastOnePersoInZdd(void *p_stPerso, unsigned char ucModulOrCharact,
                                                ACP_tdxIndex xZoneId)
{
    unsigned short uwNbDetectedPerso = 1;

    fn_vGetAllObjectsInZdd(p_stPerso, ucModulOrCharact, (unsigned char)xZoneId, g_a_hPersoInZone,
                           &uwNbDetectedPerso);
    return (unsigned char)(uwNbDetectedPerso != 0);
}

/* 0x004838B0 */
unsigned char __cdecl fn_ucAtLeastOnePersoInZde(void *p_stPerso, unsigned char ucModulOrCharact,
                                                ACP_tdxIndex xZoneId, unsigned short xMask)
{
    unsigned short uwNbDetectedPerso = 1;

    fn_vGetAllCharactersInContactWithThisZde(p_stPerso, ucModulOrCharact, (unsigned char)xZoneId,
                                             g_a_hPersoInZone, &uwNbDetectedPerso, (unsigned short)xMask);
    return (unsigned char)(uwNbDetectedPerso != 0);
}

/* 0x004838F0 */
void *__cdecl fn_p_stGetPersoInList(void *p_List, unsigned char ucPosition)
{
    if (LIST_M_ucNbElt(p_List) == 0)
        return (void *)0;

    if (ucPosition >= LIST_M_ucNbElt(p_List) && ucPosition != LIST_C_ucEndOfList)
        return (void *)0;

    if (ucPosition == LIST_C_ucEndOfList)
        return LIST_M_d_TabElt(p_List)[LIST_M_ucNbElt(p_List) - 1];
    else
        return LIST_M_d_TabElt(p_List)[ucPosition];
}

/* 0x00483930 */
AI_tdstNodeInterpret *__cdecl fn_p_stListSelect(void *p_List, char b_cInverse, AI_tdstNodeInterpret *p_stTree)
{
    void                **p_p_stPerso;
    unsigned char         stWorkList[124];
    AI_tdstNodeInterpret *p_stTreeSv = p_stTree;
    AI_tdstGetSetParam    stParam;

    p_p_stPerso = LIST_M_d_TabElt(p_List);
    LIST_M_ucNbElt(stWorkList) = 0;
    while (p_p_stPerso < LIST_M_d_TabElt(p_List) + LIST_M_ucNbElt(p_List))
    {
        p_stTree = fn_p_stEvalTree((HIE_tdstSuperObject *)*p_p_stPerso, p_stTreeSv, &stParam);
        if (stParam.uParam.cValue ^ b_cInverse)
            fn_vPutInList(stWorkList, *p_p_stPerso);
        p_p_stPerso++;
    }
    memcpy(p_List, stWorkList, 8 + (LIST_M_ucNbElt(stWorkList) - 1) * 4);
    return p_stTree;
}

/* 0x004839D0 */
AI_tdstNodeInterpret *__cdecl fn_p_stListSort(void *p_List, unsigned char ucWantedNbElt, char b_cIncrease, AI_tdstNodeInterpret *p_stTree)
{
    void                **p_p_stPerso;
    void                 *p_stPerso;
    unsigned char         ucI, ucJ;
    MTH_tdxReal           a_xCriteraResult[LIST_C_ucSizeOfMaximalList];
    MTH_tdxReal          *p_xCriteraResult;
    MTH_tdxReal           xValue;
    unsigned char         ucId = 0;
    AI_tdstNodeInterpret *p_stTreeSv = p_stTree;
    AI_tdstGetSetParam    stParam;

    p_p_stPerso = LIST_M_d_TabElt(p_List);
    p_xCriteraResult = a_xCriteraResult;

    while (p_p_stPerso < LIST_M_d_TabElt(p_List) + LIST_M_ucNbElt(p_List))
    {
        p_stTree = fn_p_stEvalTree((HIE_tdstSuperObject *)*p_p_stPerso, p_stTreeSv, &stParam);
        *(p_xCriteraResult++) = M_ReturnParam_xValue(&stParam);
        p_p_stPerso++;
    }

    if (b_cIncrease == 0)
    {
        xValue = MTH_C_InfinitMinus;
        for (ucI = 0; (ucI < LIST_M_ucNbElt(p_List) - 1) && (ucI < ucWantedNbElt); ucI++)
        {
            for (ucJ = ucI; ucJ < LIST_M_ucNbElt(p_List); ucJ++)
            {
                if (a_xCriteraResult[ucJ] > xValue)
                {
                    xValue = a_xCriteraResult[ucJ];
                    ucId = ucJ;
                }
            }

            a_xCriteraResult[ucId] = a_xCriteraResult[ucI];
            p_stPerso = LIST_M_d_TabElt(p_List)[ucId];
            LIST_M_d_TabElt(p_List)[ucId] = LIST_M_d_TabElt(p_List)[ucI];
            LIST_M_d_TabElt(p_List)[ucI] = p_stPerso;
        }
    }
    else
    {
        xValue = MTH_C_InfinitPlus;
        for (ucI = 0; (ucI < LIST_M_ucNbElt(p_List) - 1) && (ucI < ucWantedNbElt); ucI++)
        {
            for (ucJ = ucI; ucJ < LIST_M_ucNbElt(p_List); ucJ++)
            {
                if (a_xCriteraResult[ucJ] < xValue)
                {
                    xValue = a_xCriteraResult[ucJ];
                    ucId = ucJ;
                }
            }

            a_xCriteraResult[ucId] = a_xCriteraResult[ucI];
            p_stPerso = LIST_M_d_TabElt(p_List)[ucId];
            LIST_M_d_TabElt(p_List)[ucId] = LIST_M_d_TabElt(p_List)[ucI];
            LIST_M_d_TabElt(p_List)[ucI] = p_stPerso;
        }
    }

    LIST_M_ucNbElt(p_List) = (unsigned char)LIST_M_Min(LIST_M_ucNbElt(p_List), ucWantedNbElt);
    return p_stTree;
}
