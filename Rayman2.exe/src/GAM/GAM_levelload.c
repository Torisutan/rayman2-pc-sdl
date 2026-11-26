#include <stdio.h>
#include <string.h>
#include "GAM/GAM_types.h"

#include "GAM/GAM_pc.h"
#include "GAM/A3d.h"
#include "COL/COL_types.h"
#include "AI/AI_globals.h"
#include "GAM/GAM_globals.h"
#include "GAM/GAM_pc_session.h"
#include "ALW/ALW_globals.h"
#include "MMG/MMG_globals.h"
#include "GAM/pc_text_render.h"
#include "AI/AI_types.h"
#include "GAM/PlayAnim.h"
#include "SAI/SAI.h"
#include "MMG/SNA_reloc.h"
#include "GLI/Camera.h"
#include "GLI/Texture.h"
#include "GLI/RasterDefaults.h"
#include "GLI/ui_reset_redraw.h"
#include "GLI/GLI_globals.h"
#include "GLI/light_fog.h"
#include "SND/SND.h"
#include "IPT/IPT.h"

/* 0x004A6AD4 */
static unsigned char GAM_pc_g_ucFirstLevelLoadDone_storage = 0;
#define GAM_pc_g_ucFirstLevelLoadDone  GAM_pc_g_ucFirstLevelLoadDone_storage

#define GAM_ALWAYS_POOL_BASE  ((char *)&g_stAlways)

unsigned int GAM_g_u4a724c_storage;   /* 0x004A724C */
int          GAM_g_l4a7250_storage;   /* 0x004A7250 */
/* 0x004A727C */
void *g_AlphabetCharacterPointer;
#define GAM_ADDR_4a724c  ((void *)&GAM_g_u4a724c_storage)
#define GAM_ADDR_4a7250  ((void *)&GAM_g_l4a7250_storage)
#define GAM_ADDR_4a727c  ((void *)&g_AlphabetCharacterPointer)

static const char GAM_LL_szPtxPathFmt[] = "%s\\%s\\%s.ptx";   /* 0x0049F354 */
static const char GAM_LL_szSnaPathFmt[] = "%s\\%s\\%s.sna";   /* 0x0049F344 */
static const char GAM_LL_szGptPathFmt[] = "%s\\%s\\%s.gpt";   /* 0x0049F334 */
static const char GAM_LL_szSndPathFmt[] = "%s\\%s\\%s.snd";   /* 0x0049F324 */

static const char GAM_LL_szRegSection[]  = "RAYMAN2";    /* 0x0049F310 */
static const char GAM_LL_szRegLanguage[] = "Language";   /* 0x0049F318 */

static unsigned int gam_lcg_step(unsigned int seed)
{
    unsigned int s    = seed ^ 0x75bd924u;
    unsigned int next = s * 0x41a7u + (unsigned int)((int)s / 0x1f31d * -0x7fffffff);
    if ((int)next < 0)
        next += 0x7fffffffu;
    return next;
}

#ifndef RAYMAN2_NO_PROTECTION   /* level-load tamper pass over the protection actors */
extern void __cdecl MMG_pc_fn_vInstallTableDsgVars(AI_tdstMind *p_stMind);
extern void __cdecl MMG_pc_fn_vRunTamperCodeBlock(AI_tdstMind *volatile p_stMind);

void __cdecl GAM_pc_fn_vProtectFlaggedObjects(int _bSkipTamper)
{
    ALW_tdxHandleToModelList hModel;
    HIE_tdstSuperObject *p_stChild;
    struct tdstEngineObject_ *p_stObj;
    MS_tdxHandleToStandardGame hStdGame;
    MS_tdxHandleToBrain hBrain;
    AI_tdstMind *p_stMind;

    hModel = LST2_M_DynamicGetFirstElement(&g_stAlways.ALW_h_LstAlwaysModel);
    while (hModel != NULL)
    {
        p_stObj = hModel->p_stAlwaysObject;
        hStdGame = p_stObj->h_StandardGame;
        if (hStdGame != NULL && (hStdGame->ulCustomBits & GAM_C_CustBitProtection) != 0)
        {
            hBrain = (MS_tdxHandleToBrain)p_stObj->h_Brain;
            if (hBrain != NULL)
            {
                p_stMind = hBrain->p_stMind;
                if (p_stMind != NULL && _bSkipTamper == 0)
                {
                    MMG_pc_fn_vInstallTableDsgVars(p_stMind);
                    MMG_pc_fn_vRunTamperCodeBlock(p_stMind);
                }
            }
        }
        hModel = LST2_M_DynamicGetNextElement(hModel);
    }

    p_stChild = gp_stDynamicWorld->p_stFirstChild;
    while (p_stChild != NULL)
    {
        p_stObj = (struct tdstEngineObject_ *)p_stChild->hLinkedObject.p_Void;
        hStdGame = p_stObj->h_StandardGame;
        if (hStdGame != NULL && (hStdGame->ulCustomBits & GAM_C_CustBitProtection) != 0)
        {
            hBrain = (MS_tdxHandleToBrain)p_stObj->h_Brain;
            if (hBrain != NULL)
            {
                p_stMind = hBrain->p_stMind;
                if (p_stMind != NULL && _bSkipTamper == 0)
                {
                    MMG_pc_fn_vInstallTableDsgVars(p_stMind);
                    MMG_pc_fn_vRunTamperCodeBlock(p_stMind);
                }
            }
        }
        p_stChild = p_stChild->p_stNextBrother;
    }
}
#endif /* RAYMAN2_NO_PROTECTION */

#ifndef RAYMAN2_NO_PROTECTION   /* tamper pass, then re-keys the recorded relocation-table keys/states with the LCG */
void __cdecl GAM_pc_fn_vReseedLoadTables(void)
{
    unsigned long ulCount;
    unsigned long ulIdx;
    unsigned int  uState;
    unsigned int  uKey;
    unsigned int  uS;

    GAM_pc_fn_vProtectFlaggedObjects(0);

    ulCount = SNA_g_ulTableIndex;
    ulIdx   = 0;
    if (ulCount <= 0)
        return;
    uState  = MMG_g_ulLCGState;
    do
    {

        uS   = uState ^ 0x75bd924u;
        uKey = uS * 0x41a7u + (unsigned int)((int)uS / 0x1f31d * -0x7fffffff);
        if ((int)uKey < 0)
            uKey += 0x7fffffffu;
        SNA_pc_g_a_ulTableKey[ulIdx] = uKey;

        uS     = uKey ^ 0x75bd924u;
        uState = uS * 0x41a7u + (unsigned int)((int)uS / 0x1f31d * -0x7fffffff);
        if ((int)uState < 0)
            uState += 0x7fffffffu;
        SNA_pc_g_a_ulTableState[ulIdx] = uState;
    } while (++ulIdx < ulCount);
    MMG_g_ulLCGState = uState;
}
#endif /* RAYMAN2_NO_PROTECTION */

/* 0x0044DBE0 */
unsigned char __cdecl GAM_pc_fn_ucFindLevelNameIndex(char *szName)
{
    unsigned int uLen;
    unsigned int uIdx;

    uIdx = GAM_g_stEngineStructure.ucNumberOfLevels;
    uLen = strlen(szName);
    while (uIdx--)
    {
        if (_strnicmp(szName, GAM_g_stEngineStructure.a_szLevelName[uIdx], uLen) == 0)
            return (unsigned char)uIdx;
    }
    return 0xff;
}

/* 0x00412020 */
extern HIE_tdstSuperObject *g_hCharacterLauchingSoundEvents;
extern void __cdecl DNM_p_stDynamicsCameraMechanics(void);
extern void __cdecl MEC_p_stDynamicsBaseMechanics(void);

void __cdecl InitArrayOfDynamicFunction(void **p_pfnRecord)
{
    p_pfnRecord[0] = (void *)DNM_p_stDynamicsCameraMechanics;
    p_pfnRecord[1] = (void *)MEC_p_stDynamicsBaseMechanics;
}

/* 0x004B7300 */
extern unsigned long GAM_pc_g_ulNbFixObject_storage;
extern unsigned long GAM_pc_g_a_ulFixObject_storage[21];

/* 0x0049F2B4 */
static unsigned long GAM_pc_g_ul49f2b4 = 1;

/* 0x004B72F0 */
unsigned int *GAM_pc_g_p_ul4b72f0;
int           GAM_pc_g_l4b72f4;
int           GAM_pc_g_l4b72f8;
int           GAM_pc_g_l4b72fc;

#ifndef RAYMAN2_NO_PROTECTION   /* decoy read of another relocation table, random load key */
static void gam_drm_rekey_reloc_table(void)
{
    SNA_g_ucNextRelocationTableToLoad = (unsigned char)(GAM_g_stEngineStructure.ucNumberOfLevels
        - SNA_g_ucNextRelocationTableToLoad - SNA_pc_g_ucNextRelocationTableKind);
    SNA_pc_g_ucNextRelocationTableKind = (unsigned char)(4 - SNA_pc_g_ucNextRelocationTableKind);
    MMG_g_ulLCGState = gam_lcg_step(MMG_g_ulLCGState);
    GAM_pc_g_ucAlwaysSpawnCounter = (unsigned char)(GAM_pc_g_ucAlwaysSpawnCounter + (int)(MMG_g_ulLCGState) % 5 + 1);

    if (SNA_g_pGptContext != NULL)
        SNA_pc_vFreeGptTable(SNA_g_pGptContext);
    SNA_g_pGptContext = NULL;
    SNA_g_pGptContext = SNA_pc_pReadGptTable();
}
#endif /* RAYMAN2_NO_PROTECTION */

/* 0x004A1FB4 */
static long GAM_pc_g_lLevelLoadFlag = 1;

/* 0x0044ADB0 */
void __cdecl GAM_pc_fn_vSetLevelLoadFlag(void)
{
    GAM_pc_g_lLevelLoadFlag = 1;
}

/* 0x00412BA0 */
unsigned int __cdecl GAM_pc_fn_ulFindMapLinkPos(unsigned int ulSubMap, unsigned int ulEntry)
{
    unsigned int ulKey;
    unsigned int *pulNode;

    ulKey = (ulEntry & 0x0000FFFF) + (ulSubMap << 16);
    pulNode = (unsigned int *)GAM_pc_g_ulMapLinkPosListHead;
    while (pulNode != 0)
    {
        if (pulNode[0] == ulKey)
            return pulNode[1];
        pulNode = (unsigned int *)pulNode[2];
    }
    return 0;
}

#include "GAM/a3x_mem.h"
#include "GLD/GLD_Pub.h"
#include "GLI/Vpt3D.h"
#include "GAM/Family.h"
#include "GAM/ObjType.h"
#include "GAM/STRUCTUR/Engmode.h"
#define GAM_M_stFixViewportAttr (*(struct GLD_tdstViewportAttributes_ *)GAM_g_stEngineStructure.stFixViewportAttr)
#define GAM_M_stFixAttributes3D (*(GLI_tdstSpecificAttributesFor3D *)GAM_g_stEngineStructure.stFixAttributes3D)

extern long __cdecl GLD_bSetBackgroundColorForDevice(short, unsigned long);
extern unsigned char __cdecl FUN_0044f4f0(void);
extern void __cdecl FUN_0044f870(unsigned long ulArg);

extern void   __cdecl GAM_pc_fn_vProcessExternalCtrlCmds(short);
extern void __cdecl GAM_pc_fn_vCloseExternalCtrlFile(void);
extern char *__cdecl FUN_00492770(void);
extern void __cdecl FUN_00423610(const char *name);
extern void __cdecl FUN_0044d420(void);
extern char __cdecl FUN_00418960(void);
extern void __cdecl fn_vDynamInit(struct tdstEngineObject_ *p_stObject, int eObjIniType);
extern void *__stdcall SND_fn_pGetBinEvent(unsigned long ulBinId);
extern void __cdecl FUN_00421820(void);
extern void __cdecl FUN_0044f4d0(unsigned char);
extern unsigned char fn_bLoadLevelSaveGameAfterLoadLevel(void);
extern void __cdecl fn_vInitAllObjectsWhenSavedGameJustLoaded(void);
extern void __cdecl GAM_fn_lSaveBlockEventSlotMemory(void *_p_stBlockEvent, unsigned long _ulIndex);
extern void __cdecl FUN_0045edf0(void);
extern void __cdecl IPT_fn_vActivateAllEntryElements(void);
extern void __cdecl AGO_vInit(void);
extern void __cdecl GLI_pc_fn_vReadIniString(const char *, const char *, unsigned long, char *);
extern void __cdecl GLD_pc_fn_vRestartFrameTimer(void);
void fn_vInitLevelLoop(void)
{

    /* 0x0044d4f0 */
    void          (__cdecl *SNA_vOpenGptFile)(char *)                   = SNA_pc_vOpenGptFile;
    unsigned long (__cdecl *SNA_ulReadAndRelocateLong)(void)            = SNA_pc_ulReadAndRelocateLong;
    void          (__cdecl *SNA_vReadRawArray)(void *, int, unsigned int)= SNA_pc_vReadRawArray;
    void          (__cdecl *SNA_vCloseGptFile)(void)                    = SNA_pc_vCloseGptFile;
    void          (__cdecl *SNA_vOpenSndFile)(char *)                   = SNA_pc_vOpenSndFile;
    void          (__cdecl *SNA_vReadAndRelocateSnd)(void)             = SNA_pc_vReadAndRelocateSnd;

    char  szPath480[260];
    char  szPath208[260];
    char  szSaveFirstName[260];
    char  szSaveNextName[260];
    unsigned char abSaveTimer[0x70];  /* 0x5003e4 */
    int   iSaveBlock488;
    int   iSave4cc;
    int   iAttrib;
    char  cTableId;

    GAM_pc_fn_vSetLevelLoadFlag();
    fn_v_ClearLoadedAnims(FALSE);
    AI_GLI_cFogIsSaved = 0;
    GLD_bSetBackgroundColorForDevice((unsigned long)FUN_00401340(0), 0);
    g_AlphabetCharacterPointer = 0;
    GAM_g_bInGameMenu = 0;

    if (FUN_0044f4f0() == 1)
    {
        FUN_0044f870(2);
        GAM_g_ul4b7468 = 2;
        GAM_pc_fn_vProcessExternalCtrlCmds(1);
    }
    if (FUN_0044f4f0() == 1)
    {
        GAM_pc_fn_vProcessExternalCtrlCmds(1);
        GAM_pc_fn_vProcessExternalCtrlCmds(-1);
        GAM_pc_fn_vCloseExternalCtrlFile();
        GAM_g_ul4b7468 = 0;
    }

    /* 0x423610 */
    {
        char *p1 = fn_p_szGetLevelName();
        char *p2 = fn_p_szGetLevelName();
        char *pb = FUN_00492770();
        sprintf(szPath480, GAM_LL_szPtxPathFmt, pb, p2, p1);
    }
    FUN_00423610(szPath480);

    {
        char *p1 = fn_p_szGetLevelName();
        char *p2 = fn_p_szGetLevelName();
        char *pb = FUN_00492770();
        sprintf(szPath208, GAM_LL_szSnaPathFmt, pb, p2, p1);
    }

    if (FUN_0044f4e0() == 1)
    {
        unsigned int  uReadCount;
        unsigned int  uIdx;
        unsigned int *p_ulObjSlot;

        cTableId = (char)GAM_pc_fn_ucFindLevelNameIndex(fn_p_szGetLevelName());

        /* 0x500456 */
        GLI_xGetViewportCamera((unsigned long)GAM_g_stEngineStructure.hGLDDevice, (unsigned short)GAM_g_stEngineStructure.hGLDViewport, (void **)&iAttrib);
        if (iAttrib == 0)
        {
            GLI_xCreateCamera((void **)&iAttrib, 0x67);   /* 0x422750 */
            GLI_xSetCameraAspectAndRatio((void *)iAttrib, 1.5f, 0.75f);
            GLI_xSetViewportCamera((unsigned long)GAM_g_stEngineStructure.hGLDDevice,
                                   (unsigned short)GAM_g_stEngineStructure.hGLDViewport, iAttrib);   /* 0x4226d0 */
        }

        FUN_0044d420();
        *(unsigned int *)&SNA_g_ulTableIndex = 0;

#ifndef RAYMAN2_NO_PROTECTION
        if (FUN_00418960() != 0)
            gam_drm_rekey_reloc_table();
#endif /* RAYMAN2_NO_PROTECTION */
        SNA_g_ucNextRelocationTableToLoad = (unsigned char)cTableId;
        SNA_pc_g_ucNextRelocationTableKind = 0;
        SNA_pc_vLoadAndRelocate(szPath208);

#ifndef RAYMAN2_NO_PROTECTION
        if (FUN_00418960() != 0)
            gam_drm_rekey_reloc_table();
#endif /* RAYMAN2_NO_PROTECTION */
        SNA_g_ucNextRelocationTableToLoad = (unsigned char)cTableId;
        SNA_pc_g_ucNextRelocationTableKind = 1;
        {
            char *p1 = fn_p_szGetLevelName();
            char *p2 = fn_p_szGetLevelName();
            char *pb = FUN_00492770();
            sprintf(szPath480, GAM_LL_szGptPathFmt, pb, p2, p1);
        }
        SNA_vOpenGptFile(szPath480);

        uReadCount = SNA_ulReadAndRelocateLong();
        GAM_pc_g_ulNbFixObject_storage = uReadCount;
        uIdx = 0;
        if (uReadCount != 0)
        {
            p_ulObjSlot = (unsigned int *)GAM_pc_g_a_ulFixObject_storage;
            do
            {
                unsigned int uObj = SNA_ulReadAndRelocateLong();
                *p_ulObjSlot = uObj;
                if (fn_h_StandardGameGetSuperObject((MS_tdxHandleToStandardGame)*(int *)(uObj + 4)) != 0)
                {
                    int *piEntry = (int *)SNA_ulReadAndRelocateLong();
                    int *piSO    = (*piEntry == 2) ? (int *)piEntry[1] : (int *)0;

                    fn_vStandardGameSetSuperObject((MS_tdxHandleToStandardGame)piSO[1], (struct HIE_tdstSuperObject *)piEntry);
                    *p_ulObjSlot = (unsigned int)piSO;
                    SNA_pc_vReadAndRelocateBlock((void *)(*piSO + 0x18), 0x58);
                    piEntry[0xc] = (int)SNA_ulReadAndRelocateLong();
                    SNA_pc_vReadAndRelocateBlock(&iSaveBlock488, 4);
                    piEntry[5]   = (int)SNA_ulReadAndRelocateLong();
                    piEntry[6]   = (int)SNA_ulReadAndRelocateLong();
                    piEntry[7]   = (int)SNA_ulReadAndRelocateLong();
                    piEntry[0xe] = iSaveBlock488;
                    if (GAM_pc_g_ucFirstLevelLoadDone == 0)
                        ((MS_tdxHandleToStandardGame)piSO[1])->ucMiscFlags &= 0xf7;
                    else if (*(int *)(piEntry[1] + 8) != 0)
                        fn_vDynamInit((struct tdstEngineObject_ *)piSO, 0);
                }
                uIdx++;
                p_ulObjSlot++;
            } while (uIdx < GAM_pc_g_ulNbFixObject_storage);
        }
        GAM_pc_g_ucFirstLevelLoadDone = 1;

        GAM_g_ul5013c8 = SNA_ulReadAndRelocateLong();
        *(unsigned int *)&gp_stDynamicWorld = SNA_ulReadAndRelocateLong();
        *(unsigned int *)&gp_stInactiveDynamicWorld = SNA_ulReadAndRelocateLong();
        {
            unsigned int v = SNA_ulReadAndRelocateLong();
            GAM_g_ul4a6ac8 = v;
            *(unsigned int *)&SECT_hFatherSector = v;
        }
        GAM_pc_g_ulMapLinkPosListHead = SNA_ulReadAndRelocateLong();
        SNA_pc_vReadAndRelocateBlock((void *)GAM_ALWAYS_POOL_BASE, 0x1c);   /* 0x004a6b18 */
        *(unsigned int *)&g_stAlways.ALW_h_LstAlwaysModel = SNA_ulReadAndRelocateLong();  /* 0x004a6b1c */
        *(unsigned int *)&g_stAlways.ALW_h_LstAlwaysModel.hLastElementDyn = SNA_ulReadAndRelocateLong();  /* 0x004a6b20 */

        {
            unsigned int v = SNA_ulReadAndRelocateLong();
            if (GAM_pc_g_p_ul4b72f0 != (unsigned int *)0)
            {
                *GAM_pc_g_p_ul4b72f0 = v;
                if (v != 0)
                    *(unsigned int **)(v + 4) = GAM_pc_g_p_ul4b72f0;
            }
        }
        {
            ALW_tdxHandleToModelList prev = NULL;
            ALW_tdxHandleToModelList hModel;
            for (hModel = g_stAlways.ALW_h_LstAlwaysModel.hLastElementDyn; hModel != NULL;
                 hModel = hModel->hPrevBrotherDyn)
            {
                hModel->hNextBrotherDyn = prev;
                hModel->hFatherDyn = &g_stAlways.ALW_h_LstAlwaysModel;   /* 0x004a6b1c */
                prev = hModel;
            }
        }
        {
            unsigned int v = SNA_ulReadAndRelocateLong();
            if (GAM_pc_g_l4b72f4 != 0 && v != 0)
                *(int *)(v + 4) = GAM_pc_g_l4b72f4;
        }
        {
            unsigned int v = SNA_ulReadAndRelocateLong();
            if (GAM_pc_g_l4b72f8 != 0 && v != 0)
                *(int *)(v + 4) = GAM_pc_g_l4b72f8;
        }
        {
            unsigned int v = SNA_ulReadAndRelocateLong();
            if (GAM_pc_g_l4b72fc != 0 && v != 0)
                *(int *)(v + 4) = GAM_pc_g_l4b72fc;
        }

        SNA_pc_vReadAndRelocateBlock((void *)&g_stObjectTypes, sizeof(tdstObjectType));

        {
            tdhHandleToObjectTypeElement prev = NULL;
            tdhHandleToObjectTypeElement hElement;
            for (hElement = g_stObjectTypes.hFamilyType.hLastElementDyn; hElement != NULL;
                 hElement = hElement->hPrevBrotherDyn)
            {
                hElement->hNextBrotherDyn = prev;
                hElement->hFatherDyn = &g_stObjectTypes.hFamilyType;
                prev = hElement;
            }
        }
        {
            tdhHandleToObjectTypeElement prev = NULL;
            tdhHandleToObjectTypeElement hElement;
            for (hElement = g_stObjectTypes.hModelType.hLastElementDyn; hElement != NULL;
                 hElement = hElement->hPrevBrotherDyn)
            {
                hElement->hNextBrotherDyn = prev;
                hElement->hFatherDyn = &g_stObjectTypes.hModelType;
                prev = hElement;
            }
        }
        {
            tdhHandleToObjectTypeElement prev = NULL;
            tdhHandleToObjectTypeElement hElement;
            for (hElement = g_stObjectTypes.hPersonalType.hLastElementDyn; hElement != NULL;
                 hElement = hElement->hPrevBrotherDyn)
            {
                hElement->hNextBrotherDyn = prev;
                hElement->hFatherDyn = &g_stObjectTypes.hPersonalType;
                prev = hElement;
            }
        }

        {
            unsigned char ucSaveMode = GAM_g_stEngineStructure.eEngineMode;
            unsigned int  uSave588   = (unsigned int)GAM_g_stEngineStructure.hLevelNameList;
            unsigned char ucSaveFa4  = GAM_g_stEngineStructure.ucPreviousLevel;
            unsigned char ucSaveFa5  = GAM_g_stEngineStructure.ucExitIdToQuitPrevLevel;

            strcpy(szSaveFirstName, GAM_g_stEngineStructure.szFirstLevelName);
            strcpy(szSaveNextName,  GAM_g_stEngineStructure.szNextLevelName);

            iSaveBlock488 = *(int *)&((GLI_tdstSpecificAttributesFor3D *)GAM_M_stFixViewportAttr.p_vSpecificToXD)->xNear;
            memcpy(abSaveTimer, &GAM_g_stEngineStructure.stEngineTimer, 0x70);
            iSave4cc = (int)GAM_g_stEngineStructure.hDrawSem;

            SNA_pc_vReadAndRelocateBlock((void *)&GAM_g_stEngineStructure, sizeof(GAM_g_stEngineStructure));

            GAM_M_stFixAttributes3D.p_stCam = GAM_g_stEngineStructure.p_stFixCamera;
            *(int *)&GAM_M_stFixAttributes3D.xNear = iSaveBlock488;
            GAM_M_stFixViewportAttr.p_vSpecificToXD = &GAM_M_stFixAttributes3D;
            strcpy(GAM_g_stEngineStructure.szFirstLevelName, szSaveFirstName);
            strcpy(GAM_g_stEngineStructure.szNextLevelName, szSaveNextName);
            AI_GES_ucPreviousLevel = ucSaveFa4;
            AI_GES_ucExitIdToQuitPrevLevel = ucSaveFa5;
            GAM_g_stEngineStructure.hDrawSem = (void *)iSave4cc;
            memcpy(&GAM_g_stEngineStructure.stEngineTimer, abSaveTimer, 0x70);
            GAM_g_stEngineStructure.hLevelNameList = (void *)uSave588;
            GAM_g_stEngineStructure.eEngineMode = ucSaveMode;
        }

        {
            tdstFamilyList *prev = NULL;
            tdstFamilyList *p_stFamily;
            for (p_stFamily = GAM_g_stEngineStructure.hFamilyList_hLastElementDyn; p_stFamily != NULL;
                 p_stFamily = p_stFamily->hPrevBrother)
            {
                p_stFamily->hNextBrother = prev;
                p_stFamily->hFather = (tdstFamilyListAnchor *)&GAM_g_stEngineStructure.hFamilyList_hFirstElementDyn;
                prev = p_stFamily;
            }
        }

        *(unsigned int *)&gp_stLight = SNA_ulReadAndRelocateLong();
        GAM_pc_g_hMainCharacterSO = (struct HIE_tdstSuperObject *)SNA_ulReadAndRelocateLong();
        *(unsigned int *)&AI_g_p_stMainActor = 0;
        *(unsigned int *)&g_hCharacterLauchingSoundEvents = SNA_ulReadAndRelocateLong();
        *(unsigned int *)&g_hTextVisualMaterial = SNA_ulReadAndRelocateLong();
        *(unsigned int *)&g_hTextGameMaterialPub = SNA_ulReadAndRelocateLong();
        *(unsigned int *)&g_hTextGameMaterial = SNA_ulReadAndRelocateLong();
        *(unsigned int *)&g_hTextTexture = SNA_ulReadAndRelocateLong();
        *(unsigned int *)&COL_g_d_lTaggedFacesTable = SNA_ulReadAndRelocateLong();
        {
            unsigned int   *p_ulDst = (unsigned int *)g_a_hTextGeoObject;
            unsigned short *p_uwDst = (unsigned short *)g_a_xTextElement;
            do
            {
                *p_uwDst = (unsigned short)SNA_ulReadAndRelocateLong();
                *p_ulDst = SNA_ulReadAndRelocateLong();
                p_uwDst++;
                p_ulDst++;
            } while (p_uwDst < (unsigned short *)&g_a_xTextElement[10]);
        }
        SNA_ulReadAndRelocateLong();

        SNA_vReadRawArray(tdstStacks, eStackNbMax, sizeof(tdstStackInfos));
        SNA_vReadRawArray(&p_stA3dGeneral[tdstStacks[eStackA3dGENERAL].ulResetPos],
                          (int)(tdstStacks[eStackA3dGENERAL].ulStackPos - tdstStacks[eStackA3dGENERAL].ulResetPos), sizeof(tdstA3dGENERAL));
        SNA_vReadRawArray(&p_stSpeedVectors[tdstStacks[eStackVertexes].ulResetPos],
                          (int)(tdstStacks[eStackVertexes].ulStackPos - tdstStacks[eStackVertexes].ulResetPos), sizeof(MTH3D_tdstVector));
        SNA_vReadRawArray((char *)p_stAngularSpeedQuats + tdstStacks[eStackQuaternions].ulResetPos * 8,
                          (int)(tdstStacks[eStackQuaternions].ulStackPos - tdstStacks[eStackQuaternions].ulResetPos), 8);
        SNA_vReadRawArray(&p_stHierarchies[tdstStacks[eStackHierarchies].ulResetPos],
                          (int)(tdstStacks[eStackHierarchies].ulStackPos - tdstStacks[eStackHierarchies].ulResetPos), sizeof(tdstCouple));
        SNA_vReadRawArray(&p_stNTTO[tdstStacks[eStackNTTO].ulResetPos],
                          (int)(tdstStacks[eStackNTTO].ulStackPos - tdstStacks[eStackNTTO].ulResetPos), sizeof(tdstNTTO));
        SNA_vReadRawArray(&p_stOnlyFrames[tdstStacks[eStackOnlyFrames].ulResetPos],
                          (int)(tdstStacks[eStackOnlyFrames].ulStackPos - tdstStacks[eStackOnlyFrames].ulResetPos), sizeof(tdstOnlyFrame));
        SNA_vReadRawArray(&p_stChannels[tdstStacks[eStackChannels].ulResetPos],
                          (int)(tdstStacks[eStackChannels].ulStackPos - tdstStacks[eStackChannels].ulResetPos), sizeof(tdstA3dChannel));
        SNA_vReadRawArray(&p_stFrames[tdstStacks[eStackFrames].ulResetPos],
                          (int)(tdstStacks[eStackFrames].ulStackPos - tdstStacks[eStackFrames].ulResetPos), sizeof(tdstA3dFrame));
        SNA_vReadRawArray(&p_stFramesKF[tdstStacks[eStackFramesKF].ulResetPos],
                          (int)(tdstStacks[eStackFramesKF].ulStackPos - tdstStacks[eStackFramesKF].ulResetPos), sizeof(int));
        SNA_vReadRawArray(&p_stKeyFrames[tdstStacks[eStackKeyFrames].ulResetPos],
                          (int)(tdstStacks[eStackKeyFrames].ulStackPos - tdstStacks[eStackKeyFrames].ulResetPos), sizeof(tdstKeyFrame));
        SNA_pc_vReadAndRelocateArray((char *)GAM_g_pAnimStack500298 + tdstStacks[eStackEvents].ulResetPos * 0xc,
                          (int)(tdstStacks[eStackEvents].ulStackPos - tdstStacks[eStackEvents].ulResetPos), 0xc);
        SNA_vReadRawArray((char *)GAM_g_pAnimStack500288 + tdstStacks[eStackMorphData].ulResetPos * 8,
                          (int)(tdstStacks[eStackMorphData].ulStackPos - tdstStacks[eStackMorphData].ulResetPos), 8);

        g_AlphabetCharacterPointer = (void *)SNA_ulReadAndRelocateLong();
        *(unsigned int *)GAM_ADDR_4a724c = SNA_ulReadAndRelocateLong();
        *(int *)GAM_ADDR_4a7250 = **(int **)(*(int *)GAM_ADDR_4a724c + 0x14);
        GAM_pc_g_ucMapMusicIsOn = (unsigned char)SNA_ulReadAndRelocateLong();
        GAM_pc_g_hMapMusicSound = (int)SNA_ulReadAndRelocateLong();
        GAM_pc_g_hMapMusicSound = (int)SND_fn_pGetBinEvent((unsigned long)GAM_pc_g_hMapMusicSound);
        SNA_vCloseGptFile();
        *(unsigned int *)&CAM_g_hCameraListHead = 0;
        GAM_g_stEngineStructure.hCameraList_hLastElementDyn = NULL;
        GAM_g_stEngineStructure.hCameraList_lNumberOfElementsDyn = 0;

        SNA_g_ucNextRelocationTableToLoad = (unsigned char)cTableId;
        SNA_pc_g_ucNextRelocationTableKind = 3;
        GAM_fn_vActualizeAllGameViewportSize();
        FUN_00421820();
        GLI_AddToProgressBar(1);
#ifndef RAYMAN2_NO_PROTECTION
        if (FUN_00418960() != 0)
            gam_drm_rekey_reloc_table();
#endif /* RAYMAN2_NO_PROTECTION */

        SNA_g_ucNextRelocationTableToLoad = (unsigned char)cTableId;
        SNA_pc_g_ucNextRelocationTableKind = 2;
        {
            char *p1 = fn_p_szGetLevelName();
            char *p2 = fn_p_szGetLevelName();
            char *pb = FUN_00492770();
            sprintf(szPath480, GAM_LL_szSndPathFmt, pb, p2, p1);
        }
        SNA_vOpenSndFile(szPath480);
        SNA_vReadAndRelocateSnd();

#ifndef RAYMAN2_NO_PROTECTION
        if (FUN_00418960() != 0)
            gam_drm_rekey_reloc_table();
#endif /* RAYMAN2_NO_PROTECTION */

        FUN_0044f4d0(4);
#ifndef RAYMAN2_NO_PROTECTION
        GAM_pc_fn_vReseedLoadTables();
#endif /* RAYMAN2_NO_PROTECTION */
        fn_vInitAllObjectsWhenMapJustLoaded();   /* 0x4062f0 */
        GLI_AddToProgressBar(1);
        CAM_fn_vInitCameras();                        /* 0x40b8d0 */

#ifndef RAYMAN2_NO_PROTECTION
        MMG_g_ulLCGState = gam_lcg_step(MMG_g_ulLCGState);
        *(unsigned int *)&SNA_g_ulTableIndex = MMG_g_ulLCGState;
#endif /* RAYMAN2_NO_PROTECTION */
        fn_vInitDeadLoop();                       /* 0x403150 */

        if (GAM_g_stEngineStructure.eEngineMode == 5)
        {
            char cSave450 = fn_bLoadLevelSaveGameAfterLoadLevel();
            char cSave410;
            if (cSave450 != 0)
                SAI_fn_bInitAllWhen(SAI_eInitWhenLevelGameSavedLoaded);   /* 0x449e80 */
            cSave410 = (char)fn_bLoadPlayerSaveGameAfterLoadLevel();
            if (cSave410 != 0)
                SAI_fn_bInitAllWhen(SAI_eInitWhenPlayerGameSavedLoaded);  /* 0x449e80 */
            if (cSave410 != 0 || cSave450 != 0)
                fn_vInitAllObjectsWhenSavedGameJustLoaded();
        }
        FUN_0044f4d0(1);
    }

    if ((GAM_g_stEngineStructure.eEngineMode == 5 || GAM_g_stEngineStructure.eEngineMode == 3) &&
        GAM_pc_g_ucMapMusicIsOn != 0)
    {
        GAM_fn_lSaveBlockEventSlotMemory((void *)GAM_pc_g_hMapMusicSound, 0);
        SND_fn_lSendRequestSound(-1, GAM_g_lSoundObjectTypeMusic, (void *)GAM_pc_g_hMapMusicSound, 0, (void *)0);
    }

    InitArrayOfDynamicFunction((void **)GAM_g_a_pfnDynamicFunction);
    IPT_fn_vResetInputEntry();      /* 0x42e090 */
    PLA_fn_vInitHeaps();
    GAM_pc_g_ul49f2b4 = 0;
    GLI_AddToProgressBar(1);
    FUN_0045edf0();
    FUN_0045fc70(&GLI_pc_g_stProgressUi);
    GLI_g_cGlobalFogIsOn = 0;   /* 0x004A7248 */
    FUN_00423ab0();
    IPT_fn_vActivateAllEntryElements();
    IPT_fn_vReadInput();
    IPT_fn_vReadInput();
    *(unsigned int *)&g_hSOCurrentMainSector = 0;
    *(unsigned int *)&g_hSOPreviousMainSector = 0;
    AGO_vInit();

    szSaveFirstName[0] = '\0';
    GLI_pc_fn_vReadIniString((char *)GAM_LL_szRegSection, (char *)GAM_LL_szRegLanguage, 0x104, szSaveFirstName);
    GAM_g_ucCurrentLanguage = 0;
    if (GAM_g_stEngineStructure.ucNbLanguages != 0)
    {
        int iIndex  = 0;
        while (_stricmp(szSaveFirstName, ((tdstLanguageStructure *)GAM_g_stEngineStructure.p_stLanguageTable)[iIndex].szLanguageCode) != 0)
        {
            iIndex++;
            if ((int)(unsigned int)GAM_g_stEngineStructure.ucNbLanguages <= iIndex)
            {
                GLD_pc_fn_vRestartFrameTimer();
                return;
            }
        }
        GAM_g_ucCurrentLanguage = (unsigned char)iIndex;
    }
    GLD_pc_fn_vRestartFrameTimer();
}
