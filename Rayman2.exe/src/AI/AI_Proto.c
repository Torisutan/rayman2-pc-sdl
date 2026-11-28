



















#include <string.h>

#include "AI/AI_types.h"
#include "AI/Brain.h"
#include "AI/AI_Struc.h"
#include "AI/DsgMem.h"
#include "AI/AI_Proto.h"
#include "AI/StrIntel.h"
#include "AI/DsgVarTable.h"
#include "SAI/SAI.h"
#include "MMG/MMG_alloc.h"
#include "GAM/GAM_types.h"
#include "ERM/CpaError.h"
#include "MMG/MMG_module.h"

/* 0x004E9728 */

unsigned char AI_g_ucMemoryBlockUsed;













#define AI_M_p_stGetDsgVar(p_stMind) (((AI_tdstAIModel *)(p_stMind)->p_stAIModel)->p_stDsgVar)





void fn_vTestUnusedComport(void)
{
}




void __cdecl AI_fn_vMemoryLogFile(void *p_vPointer, unsigned char ucAction, char *szFile, unsigned long ulLine)
{
    (void)p_vPointer;
    (void)ucAction;
    (void)szFile;
    (void)ulLine;
}




void __cdecl fn_vSecondPassOfMindLoading(AI_tdstMind *p_stMind, unsigned char _bPersoInFix,
                                         unsigned long _eObjectInit)
{
    (void)p_stMind;
    (void)_bPersoInFix;
    (void)_eObjectInit;
}


















#define AI_C_ActionMalloc 0



#define eDsgVarType_WayPoint        0x08
#define eDsgVarType_Perso           0x09
#define eDsgVarType_List            0x0A
#define eDsgVarType_Vector          0x0B
#define eDsgVarType_Comport         0x0C
#define eDsgVarType_Action          0x0D
#define eDsgVarType_GameMaterial    0x0F
#define eDsgVarType_PersoArray      0x12
#define eDsgVarType_VectorArray     0x13
#define eDsgVarType_FloatArray      0x14
#define eDsgVarType_IntegerArray    0x15
#define eDsgVarType_WayPointArray   0x16
#define eDsgVarType_TextArray       0x17


extern struct tdstBlockInfoPriv *g_a_p_stMmgModuleBlocksInfo[255];



extern struct tdstBlockInfoPriv *g_a_p_stMmgModuleBlocksInfo[255];

AI_tdstMind *__cdecl fn_pstAllocMind(void)
{
    AI_tdstMind *p_stMind = NULL;

    p_stMind = (AI_tdstMind *)AI_fn_p_vTrueAlloc(sizeof(AI_tdstMind));
#line 198 ".\\AIBase\\AI_Proto.c"
    AI_fn_vMemoryLogFile(p_stMind, AI_C_ActionMalloc, __FILE__, __LINE__);
    fn_vErrorOnAIAlloc();

    p_stMind->p_stAIModel = NULL;
    p_stMind->p_stIntelligence = NULL;
    p_stMind->p_stReflex = NULL;
    p_stMind->p_stDsgMem = NULL;
    AI_M_SetDoingIntel(p_stMind, 0);

    return p_stMind;
}

/* 0x00466770 */

void __cdecl fn_vFirstInitAIMemory(void)
{
    if (g_ucAIModuleId == 0xff)
        g_ucAIModuleId = Erm_fn_ucInitErrMsg();
    Mmg_fn_v_InitMmg(g_ucAIModuleId, 2, 0x186a0);
}


#define E_ucAIBlockFix              0
#define E_ucAIBlockLevel            1
#define C_ulDefaultMaxNbStaticMalloc 0x186a0

#define AI_C_BlockModeFixOrLevel    0x0c





void __cdecl fn_vInitAIFixMemory(unsigned long ulSize)
{
    Mmg_fn_vInitSpecificBlock(E_ucAIBlockFix, g_ucAIModuleId,
                              g_a_p_stMmgModuleBlocksInfo[g_ucAIModuleId],
                              ulSize, C_ulDefaultMaxNbStaticMalloc, 4, AI_C_BlockModeFixOrLevel);
}




void __cdecl fn_vInitAILevelMemory(unsigned long ulSize)
{
    Mmg_fn_vInitSpecificBlock(E_ucAIBlockLevel, g_ucAIModuleId,
                              g_a_p_stMmgModuleBlocksInfo[g_ucAIModuleId],
                              ulSize, C_ulDefaultMaxNbStaticMalloc, 4, AI_C_BlockModeFixOrLevel);
}

/* 0x00466800 */



void __cdecl fn_vFreeAILevelMemory(void)
{
    Mmg_fn_vFreeBlock(1, g_ucAIModuleId, g_a_p_stMmgModuleBlocksInfo[g_ucAIModuleId]);
}

/* 0x00466820 */



void __cdecl fn_vDeleteAILevelMemory(void)
{
    Mmg_fn_vDeleteBlock(1, g_ucAIModuleId, g_a_p_stMmgModuleBlocksInfo[g_ucAIModuleId]);
}

/* 0x00466840 */



void __cdecl AI_fn_vAIUseFixMemory(void)
{
    AI_g_ucMemoryBlockUsed = 0;
}










void __cdecl AI_fn_vAIUseLevelMemory(void)
{
    AI_g_ucMemoryBlockUsed = 1;
}












void *__cdecl AI_fn_p_vTrueAlloc(unsigned long ulSize)
{
    void *p_vPointer = NULL;



    Mmg_fn_v_SetModeAlloc(g_ucAIModuleId, AI_g_ucMemoryBlockUsed, C_ucMmgDefaultChannel);

    if (AI_g_ucDynamicAlloc ||
        (p_vPointer = fn_p_vGenAlloc(ulSize, C_ucMmgDefaultChannel)) == NULL)
    {

    }
    memset(p_vPointer, 0, ulSize);

    return p_vPointer;
}







void __cdecl AI_fn_vTrueFree(void *p_vPointer)
{


    Mmg_fn_v_SetModeAlloc(g_ucAIModuleId, AI_g_ucMemoryBlockUsed, C_ucMmgDefaultChannel);
    fn_vGenFree(p_vPointer, C_ucMmgDefaultChannel);
}















/* 0x403583 */





void __cdecl FUN_004668e0(int iIgnored)
{
    (void)iIgnored;
    g_ucNewActionReturn = 0x20;
    g_ucUseDefaultActionReturn = 1;
}






ACP_tdxBool __cdecl fn_bDoIntelligencePerso(HIE_tdstSuperObject *p_SuperObjPerso)
{
    AI_tdstMind *p_stPerso = AI_M_stGetMindOfSuperObj(p_SuperObjPerso);

    AI_M_SetDoingIntel(p_stPerso, 1);


    if (AI_M_p_stGetIntelligence(p_stPerso) != NULL)
        return fn_bIntelligenceEngine(p_SuperObjPerso, AI_M_p_stGetIntelligence(p_stPerso));
    else
        return FALSE;
}






ACP_tdxBool __cdecl fn_bDoReflexPerso(HIE_tdstSuperObject *p_SuperObjPerso)
{
    AI_tdstMind *p_stPerso = AI_M_stGetMindOfSuperObj(p_SuperObjPerso);

    AI_M_SetDoingIntel(p_stPerso, 0);


    if (AI_M_p_stGetReflex(p_stPerso) != NULL)
        return fn_bIntelligenceEngine(p_SuperObjPerso, AI_M_p_stGetReflex(p_stPerso));
    else
        return FALSE;
}








void __cdecl fn_vReinitDesignerVariablesWithFlags(AI_tdstMind *p_stMind, tdeObjectTreeInit eObjIniType)
{
    if (p_stMind != NULL)
    {

        if (p_stMind->p_stDsgMem != NULL)
        {
            char         *ptrSrc = NULL;
            char         *ptrDst = p_stMind->p_stDsgMem->p_cDsgMemBuffer;
            unsigned char ucVarId;


            if (p_stMind->p_stDsgMem->p_cDsgMemBufferInit == NULL)
            {

                ptrSrc = AI_M_GetDsgVarModel(p_stMind)->p_cDsgMemDefaultInit;
            }
            else
            {

                ptrSrc = p_stMind->p_stDsgMem->p_cDsgMemBufferInit;
            }


            for (ucVarId = 0; ucVarId < AI_M_GetDsgVarModel(p_stMind)->ucNbDsgVar; ucVarId++)
            {
                unsigned long eDsgVarType = M_GetDsgVarType(p_stMind, ucVarId);


                if (eDsgVarType == eDsgVarType_List)
                {

                    char *p_cDstList = ptrDst + M_GetDsgVarOffset(p_stMind, ucVarId);
                    char *p_cSrcList = ptrSrc + M_GetDsgVarOffset(p_stMind, ucVarId);

                    p_cDstList[0] = 0;
                    p_cDstList[1] = p_cSrcList[1];
                }

                else
                {
                    tdeObjectTreeInit eFlags = M_GetDsgVarInitType(p_stMind, ucVarId);


                    if (eObjIniType <= eFlags)
                    {

                        unsigned long ulMyVarOffset = M_GetDsgVarOffset(p_stMind, ucVarId);
                        unsigned long ulMyVarSize;

                        if (fn_GetDsgVarType(eDsgVarType) == E_vt_Array)
                        {
                            tdstArray *p_stArray = (tdstArray *)M_GetDsgVarAddrDefault(p_stMind, ucVarId);
                            ulMyVarSize = fn_ulSizeOfDsgVar(eDsgVarType, p_stArray->ucMaxSize);
                        }
                        else
                            ulMyVarSize = fn_ulSizeOfDsgVar(eDsgVarType, 0);
                        memcpy(ptrDst + ulMyVarOffset, ptrSrc + ulMyVarOffset, ulMyVarSize);
                    }
                }
            }
        }
    }
}












void __cdecl ReinitDesignerVariables(AI_tdstMind *p_stMind)
{
    if (p_stMind != NULL)
    {

        if (p_stMind->p_stDsgMem != NULL)
        {
            char         *ptrSrc = NULL;
            char         *ptrDst = p_stMind->p_stDsgMem->p_cDsgMemBuffer;
            unsigned char ucVarId;


            if (p_stMind->p_stDsgMem->p_cDsgMemBufferInit == NULL)
            {

                ptrSrc = AI_M_GetDsgVarModel(p_stMind)->p_cDsgMemDefaultInit;
            }
            else
            {

                ptrSrc = p_stMind->p_stDsgMem->p_cDsgMemBufferInit;
            }


            for (ucVarId = 0; ucVarId < AI_M_GetDsgVarModel(p_stMind)->ucNbDsgVar; ucVarId++)
            {
                unsigned long eDsgVarType = M_GetDsgVarType(p_stMind, ucVarId);
                tdeObjectTreeInit eFlags  = M_GetDsgVarInitType(p_stMind, ucVarId);

                if (eFlags != OTI_WhenGameStart)
                {

                    unsigned long ulMyVarOffset = M_GetDsgVarOffset(p_stMind, ucVarId);
                    unsigned long ulMyVarSize;

                    if (fn_GetDsgVarType(eDsgVarType) == E_vt_Array)
                    {
                        tdstArray *p_stArray = (tdstArray *)M_GetDsgVarAddrDefault(p_stMind, ucVarId);
                        ulMyVarSize = fn_ulSizeOfDsgVar(eDsgVarType, p_stArray->ucMaxSize);
                    }
                    else
                        ulMyVarSize = fn_ulSizeOfDsgVar(eDsgVarType, 0);
                    memcpy(ptrDst + ulMyVarOffset, ptrSrc + ulMyVarOffset, ulMyVarSize);
                }
            }
        }
    }
}







void __cdecl fn_vInsertDsgvarOneDsgVarInSAI(char *p_MyVarAddr,
                                            unsigned long eDsgVarType,
                                            SAI_tdeFlags eFlags)
{
    switch (eDsgVarType)
    {

        case eDsgVarType_WayPoint:
        case eDsgVarType_Comport:
        case eDsgVarType_Action:
        case eDsgVarType_Perso:
        case eDsgVarType_GameMaterial:
            SAI_fn_bInsertPointerInTable((void *)p_MyVarAddr, eFlags);
            break;
        case eDsgVarType_List:

            break;
        case eDsgVarType_PersoArray:
        case eDsgVarType_VectorArray:
        case eDsgVarType_FloatArray:
        case eDsgVarType_IntegerArray:
        case eDsgVarType_WayPointArray:
        case eDsgVarType_TextArray:

            break;
        default:
            SAI_fn_bInsertArrayInTable((void *)p_MyVarAddr, fn_ulSizeOfDsgVar(eDsgVarType, 0), eFlags);
            break;
    }
}







void __cdecl AI_fn_vInsertDsgVarInSAI(AI_tdstMind *p_stMind)
{
  if (p_stMind!=NULL)
  {

    if ( (AI_M_p_stGetDsgMem(p_stMind) != NULL) && (AI_M_p_stGetDsgVar(p_stMind)!=NULL) )
    {
	  unsigned char ucVarId;

      for (ucVarId=0;ucVarId<M_GetNbDsgVar(p_stMind);ucVarId++)
	  {
        SAI_tdeFlags eFlags=(SAI_tdeFlags)M_GetDsgVarSaveType(p_stMind,ucVarId);

		if (eFlags!=(SAI_tdeFlags) 0)
		{

          char *p_MyVarAddr=(char*) M_GetDsgVarAddr(p_stMind,ucVarId);
		  unsigned long eDsgVarType=M_GetDsgVarType(p_stMind,ucVarId);


		  if (fn_GetDsgVarType(eDsgVarType)==E_vt_Array)
		  {
			unsigned char ucIndex=0;
			tdstArray *p_stArray=(tdstArray*) p_MyVarAddr;
			unsigned long eDsgVarTypeId=M_ARRAY_TYPE(p_stArray);

			for (ucIndex=0;ucIndex<M_ARRAY_SIZE(p_stArray);ucIndex++)
			{
				p_MyVarAddr=(char*) M_ARRAY_ELEMENT(p_stArray,ucIndex);
				fn_vInsertDsgvarOneDsgVarInSAI(p_MyVarAddr,eDsgVarTypeId,eFlags);
			}
		  }
		  else
		  {
			fn_vInsertDsgvarOneDsgVarInSAI(p_MyVarAddr,eDsgVarType,eFlags);
		  }
		}
	  }
	}
  }
}







void __cdecl fn_vReinitAIComport(AI_tdstMind *p_stMind)
{

    fn_vSetUltraOperatorPerso(NULL, NULL);

    if (p_stMind != NULL)
    {
        AI_tdstIntelligence *p_stReflexOrIntelligence;


        if ((p_stReflexOrIntelligence = AI_M_p_stGetIntelligence(p_stMind)) != NULL)
        {
            fn_vReinitComport(p_stReflexOrIntelligence);
        }


        if ((p_stReflexOrIntelligence = AI_M_p_stGetReflex(p_stMind)) != NULL)
        {
            fn_vReinitComport(p_stReflexOrIntelligence);
        }
    }
}







void __cdecl fn_vReinitAI(AI_tdstMind *p_stMind)
{
    if (p_stMind != NULL)
    {
        fn_vReinitAIComport(p_stMind);
        ReinitDesignerVariables(p_stMind);
    }
}






void __cdecl fn_vReinitAIComportWithFlags(AI_tdstMind *p_stMind, tdeObjectTreeInit eObjIniType)
{
    eObjIniType = eObjIniType;
    fn_vReinitAIComport(p_stMind);
}





void __cdecl fn_vReinitAIWithFlags(AI_tdstMind *p_stMind, tdeObjectTreeInit eObjIniType)
{
    if (p_stMind != NULL)
    {

        fn_vReinitAIComportWithFlags(p_stMind, eObjIniType);


        fn_vReinitDesignerVariablesWithFlags(p_stMind, eObjIniType);
    }
}







char *__cdecl fn_p_cSimulateAllocDsgMemBuffer(char **p_ptr,
                                                 unsigned long ulSize)
{
    char *p_cDsgMemBuffer = NULL;
    p_cDsgMemBuffer = (char *)(*p_ptr);
    (*p_ptr) += ulSize * sizeof(char);

    memset(p_cDsgMemBuffer, 0, ulSize);
    return p_cDsgMemBuffer;
}







AI_tdstDsgMem *__cdecl fn_p_stSimulateAllocDsgMem(
    char **p_ptr, AI_tdstDsgVar **p_p_stDsgVar, unsigned long ulSize)
{
    AI_tdstDsgMem *p_stDsgMem = NULL;
    p_stDsgMem = (AI_tdstDsgMem *)(*p_ptr);
    memset(*p_ptr, 0, sizeof(AI_tdstDsgMem));
    (*p_ptr) += sizeof(AI_tdstDsgMem);

    p_stDsgMem->p_p_stDsgVar = p_p_stDsgVar;
    p_stDsgMem->p_cDsgMemBufferInit = NULL;

    p_stDsgMem->p_cDsgMemBuffer =
        fn_p_cSimulateAllocDsgMemBuffer(p_ptr, ulSize);
    return p_stDsgMem;
}






AI_tdstActionTableEntry *__cdecl fn_p_stSimulateAllocActionTableEntry(
    char **p_ptr,
    unsigned char ucActionTableNbEntry)
{
    AI_tdstActionTableEntry *p_stActionTableEntry = NULL;
    p_stActionTableEntry = (AI_tdstActionTableEntry *)(*p_ptr);
    memset(*p_ptr, 0,
           ucActionTableNbEntry * sizeof(AI_tdstActionTableEntry));
    (*p_ptr) += (ucActionTableNbEntry * sizeof(AI_tdstActionTableEntry));
    return p_stActionTableEntry;
}







AI_tdstActionTable *__cdecl fn_p_stSimulateAllocActionTable(
    char **p_ptr,
    unsigned char ucActionTableNbEntry)
{
    AI_tdstActionTable      *p_stActionTable = NULL;
    AI_tdstActionTableEntry *p_stEntry       = NULL;
    unsigned char            ucNEntry;

    if (ucActionTableNbEntry != 0)
    {
        p_stActionTable = (AI_tdstActionTable *)(*p_ptr);
        memset(*p_ptr, 0, sizeof(AI_tdstActionTable));
        (*p_ptr) += sizeof(AI_tdstActionTable);
    }
    else
    {
        return NULL;
    }

    p_stActionTable->ucNbEntries = ucActionTableNbEntry;
    p_stActionTable->p_stEntry =
        fn_p_stSimulateAllocActionTableEntry(p_ptr, ucActionTableNbEntry);

    p_stActionTable->ucNbEntriesUsed = 0;
    p_stActionTable->ucCurrentEntry  = 0;

    for (ucNEntry = 0, p_stEntry = p_stActionTable->p_stEntry;
         ucNEntry < ucActionTableNbEntry;
         ucNEntry++, p_stEntry++)
    {
        p_stEntry->bUseDefaultActionReturn = g_ucUseDefaultActionReturn;
        p_stEntry->ucNewActionReturn       = g_ucNewActionReturn;
    }

    return p_stActionTable;
}









AI_tdstIntelligence *__cdecl fn_p_stSimulateAllocIntelligence(
    char **p_ptr, AI_tdstScriptAI **p_p_stScriptAI,
    unsigned char ucActionTableNbEntry)
{
    AI_tdstIntelligence *p_stIntelligence = NULL;

    if ((*p_p_stScriptAI)->ulNbComport != 0)
    {
        p_stIntelligence = (AI_tdstIntelligence *)(*p_ptr);
        memset(*p_ptr, 0, sizeof(AI_tdstIntelligence));
        (*p_ptr) += sizeof(AI_tdstIntelligence);

        p_stIntelligence->p_p_stScriptAI = p_p_stScriptAI;

        p_stIntelligence->ulNoComportInit =
            (*p_p_stScriptAI)->ulNoComportInitDefault;

        p_stIntelligence->p_stActionTable =
            fn_p_stSimulateAllocActionTable(p_ptr, ucActionTableNbEntry);
    }

    return p_stIntelligence;
}

void __cdecl fn_vSimulateMindAllocationFromAnotherMind(AI_tdstMind *p_stMindSrc,
                                                       AI_tdstMind *p_stMindDst)
{
    if ((p_stMindSrc != NULL) && (p_stMindDst != NULL))
    {
        AI_tdstAIModel      *p_stLoadedAIModel;
        AI_tdstMind         *p_stLoadedMind = p_stMindDst;
        AI_tdstScriptAI    **p_p_stScriptAI = NULL;
        AI_tdstIntelligence *p_stIntelligenceOrReflex = NULL;

        char *ptr = (char *)(p_stMindDst + 1);


        p_stLoadedAIModel = (AI_tdstAIModel *)p_stMindSrc->p_stAIModel;
        p_stLoadedMind->p_stAIModel = (struct AI_tdstAIModel_ *)p_stLoadedAIModel;


        p_p_stScriptAI = &p_stLoadedAIModel->p_stScriptAIIntell;
        if (*p_p_stScriptAI != NULL)
        {
            p_stLoadedMind->p_stIntelligence = (struct AI_tdstIntelligence_ *)
                fn_p_stSimulateAllocIntelligence(&ptr, p_p_stScriptAI,
                                                    (*p_p_stScriptAI)->ucActionTableNbEntry);
            p_stIntelligenceOrReflex = (AI_tdstIntelligence *)p_stMindSrc->p_stIntelligence;
            if (p_stIntelligenceOrReflex != NULL)
            {
                ((AI_tdstIntelligence *)p_stLoadedMind->p_stIntelligence)->ulNoComportInit =
                    p_stIntelligenceOrReflex->ulNoComportInit;
            }
        }
        else
        {
            p_stLoadedMind->p_stIntelligence = NULL;
        }

        p_stIntelligenceOrReflex = (AI_tdstIntelligence *)p_stMindSrc->p_stIntelligence;
        if (p_stIntelligenceOrReflex != NULL)
        {
            ((AI_tdstIntelligence *)p_stLoadedMind->p_stIntelligence)->ulNoComportInit =
                p_stIntelligenceOrReflex->ulNoComportInit;
        }


        p_p_stScriptAI = &p_stLoadedAIModel->p_stScriptAIReflex;
        if (*p_p_stScriptAI != NULL)
        {
            p_stLoadedMind->p_stReflex = (struct AI_tdstIntelligence_ *)
                fn_p_stSimulateAllocIntelligence(&ptr, p_p_stScriptAI,
                                                    (*p_p_stScriptAI)->ucActionTableNbEntry);

            p_stIntelligenceOrReflex = (AI_tdstIntelligence *)p_stMindSrc->p_stReflex;
            if (p_stIntelligenceOrReflex != NULL)
            {
                ((AI_tdstIntelligence *)p_stLoadedMind->p_stReflex)->ulNoComportInit =
                    p_stIntelligenceOrReflex->ulNoComportInit;
            }
        }
        else
        {
            p_stLoadedMind->p_stReflex = NULL;
        }


        p_stLoadedMind->p_stDsgMem = NULL;
        if (AI_M_p_stGetDsgVar(p_stLoadedMind) != NULL)
        {
            if (AI_M_p_stGetDsgVar(p_stLoadedMind)->ulSize != 0)
            {
                p_stLoadedMind->p_stDsgMem =
                    fn_p_stSimulateAllocDsgMem(&ptr, &AI_M_p_stGetDsgVar(p_stLoadedMind),
                                                  AI_M_p_stGetDsgVar(p_stLoadedMind)->ulSize);
                *(p_stLoadedMind->p_stDsgMem->p_p_stDsgVar) = *(p_stMindSrc->p_stDsgMem->p_p_stDsgVar);


                if (p_stMindSrc->p_stDsgMem->p_cDsgMemBufferInit != NULL)
                {
                    p_stLoadedMind->p_stDsgMem->p_cDsgMemBufferInit =
                        fn_p_cSimulateAllocDsgMemBuffer(&ptr, AI_M_p_stGetDsgVar(p_stLoadedMind)->ulSize);


                    memcpy(p_stLoadedMind->p_stDsgMem->p_cDsgMemBufferInit,
                           p_stMindSrc->p_stDsgMem->p_cDsgMemBufferInit,
                           AI_M_p_stGetDsgVar(p_stLoadedMind)->ulSize);
                }
            }
        }


        p_stIntelligenceOrReflex = (AI_tdstIntelligence *)p_stLoadedMind->p_stIntelligence;
        if (p_stIntelligenceOrReflex)
        {
            p_stIntelligenceOrReflex->p_stCurrentComport = (void *)p_stIntelligenceOrReflex->ulNoComportInit;
            p_stIntelligenceOrReflex->p_stPrevComport = p_stIntelligenceOrReflex->p_stCurrentComport;
        }
        p_stIntelligenceOrReflex = (AI_tdstIntelligence *)p_stLoadedMind->p_stReflex;
        if (p_stIntelligenceOrReflex)
        {
            p_stIntelligenceOrReflex->p_stCurrentComport = (void *)p_stIntelligenceOrReflex->ulNoComportInit;
            p_stIntelligenceOrReflex->p_stPrevComport = p_stIntelligenceOrReflex->p_stCurrentComport;
        }
        if (p_stLoadedMind->p_stDsgMem != NULL)
        {

            if (p_stLoadedMind->p_stDsgMem->p_cDsgMemBufferInit == NULL)
            {

                memcpy(p_stLoadedMind->p_stDsgMem->p_cDsgMemBuffer,
                       AI_M_p_stGetDsgVar(p_stLoadedMind)->p_cDsgMemDefaultInit,
                       AI_M_p_stGetDsgVar(p_stLoadedMind)->ulSize);
            }
            else
            {

                memcpy(p_stLoadedMind->p_stDsgMem->p_cDsgMemBuffer,
                       p_stLoadedMind->p_stDsgMem->p_cDsgMemBufferInit,
                       AI_M_p_stGetDsgVar(p_stLoadedMind)->ulSize);
            }
        }
    }
}
