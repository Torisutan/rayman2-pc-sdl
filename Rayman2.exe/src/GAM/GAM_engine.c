#include <stdio.h>
#include <string.h>
#include "AI/AI_globals.h"
#include "MMG/SNA_reloc.h"
#include "MMG/MMG_globals.h"
#include "AI/AI_types.h"
#include "GAM/GAM_globals.h"
#include "GAM/GAM_pc_session.h"
#include "MGT/MGT_globals.h"
#include "GAM/GAM_types.h"

#include "GAM/TextFrmt.h"
#include "GMT/GMT_types.h"
#include "ERM/CpaError.h"

#include "GAM/GAM_pc.h"

#include "HIE/HIE_types.h"

#include "POS/POS_types.h"
#include "GAM/PlayAnim.h"
#include "SAI/SAI.h"
#include "GLI/Init_Gli.h"
#include "GLI/RasterDefaults.h"
#include "GLI/light_fog.h"
#include "GLI/Camera.h"
#include "FON/FON_globals.h"
#include "GAM/A3d.h"

extern void __cdecl fn_vDestructAllFatherSonLinks(void);  /* 0x0040b0e0 */
extern void __cdecl fn_vKillAllAlways(void);              /* 0x0040c200 */
extern void __cdecl AGO_vInit(void);                     /* 0x0045ffa0 */
extern char *__cdecl FUN_00492700(void);                 /* 0x00492700 */
extern void __cdecl fn_vKillAllAlwaysModel(void);
#include "GAM/Always.h"
#include "SND/sndtype.h"
#include "SND/sndfile.h"
extern void __cdecl MOR_pc_fn_vCleanAllMorphTasks(void); /* 0x00443EF0 */
extern void __cdecl SND_PrintUsedStaticMemory(void);   /* 0x00403290 */
extern void __cdecl fn_vReinitEngineClock(void);
extern void __cdecl fn_vInitAllObjectsWhenNewGameStart(void);
extern void __cdecl fn_vTestUnusedComport(void);

static const char GAM_ENG_szGameName[]       = "Rayman2";
static const char GAM_ENG_szFixPtxPath[]     = "\\world\\levels\\fix.ptx";
static const char GAM_ENG_szGeneralSaveFmt[] = "%s\\%s\\General.sav";
static const char GAM_ENG_szSlotSaveFmt[]    = "%s\\%s\\%s_%d.sav";
/* 0x0049F8C0 */
static const char GAM_ENG_szHiddenSlot[]     = "HiddenSlot";
char *GAM_gs_pszHiddenSlot = (char *)GAM_ENG_szHiddenSlot;  /* 0x0049F8C0 */
#define GAM_ENG_pszHiddenSlot     GAM_gs_pszHiddenSlot

unsigned int GAM_g_ul500248_storage;
unsigned int GAM_g_ul500250_storage;
unsigned int GAM_g_ul500254_storage;
#define GAM_g_ul500248  GAM_g_ul500248_storage
#define GAM_g_ul500250  GAM_g_ul500250_storage
#define GAM_g_ul500254  GAM_g_ul500254_storage

/* 0x0049F8D4 */
unsigned char GAM_pc_g_ucMode6SaveFlag_storage = 1;
#define GAM_pc_g_ucMode6SaveFlag  GAM_pc_g_ucMode6SaveFlag_storage

static unsigned int gam_lcg_step(unsigned int seed)
{
    unsigned int s    = seed ^ 0x75bd924u;
    unsigned int next = s * 0x41a7u + (unsigned int)((int)s / 0x1f31d * -0x7fffffff);
    if ((int)next < 0)
        next += 0x7fffffffu;
    return next;
}

/* 0x00422720 */
extern void GLI_xAdjustCameraToViewport(short, short, struct GLI_tdstCamera_ *);

/* 0x00408520 */
extern void __cdecl SND_fn_vSetVolumeLineSound(void *pLine, long lNewVolume);

/* 0x004099d0 */
extern void __cdecl GAM_fn_vSetEngineInLevelMode(void);

extern void __cdecl TFMT_vInitTextAfficheStructure(void);

void         GAM_pc_fn_vCheckMemoryCanary(void);
void __cdecl FUN_004668e0(int iIgnored);
void         FUN_00407ad0(void);
void         GAM_pc_fn_vPruneDscLessDirectories(void);
void         fn_v_UnclearPreviousLoadedAnims(void);
unsigned char __cdecl GAM_pc_fn_bDeleteSaveSlot(unsigned int uSlot);

/* 0x004A71A4 */
GMT_tdxHandleToGameMaterial GAM_pc_g_hEngineGameMaterial;

#ifndef RAYMAN2_NO_PROTECTION   /* decoy read of another relocation table, random load key */
/* 0x00500588 */
static void gam_fix_rekey_reloc_table(void)
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

#include "GAM/a3x_mem.h"
#include "IPT/IPT_Str.h"

extern void __cdecl SNA_pc_vVerifyCdLevelsFile(void);   /* 0x44DE60 */
extern char __cdecl FUN_00418960(void);
extern char *__cdecl FUN_00492770(void);
extern void __cdecl FUN_00421820(void);
extern void __cdecl FUN_00423610(const char *name);

#define GAM_g_stMainCharacterPosition (GAM_g_stEngineStructure.stMainCharacterPosition)
#define GAM_g_stMainCameraPosition    (GAM_g_stEngineStructure.stMainCameraPosition)

#define GAM_g_hCameraList_hLastElement       (GAM_g_stEngineStructure.hCameraList_hLastElementDyn)
#define GAM_g_hCameraList_lNumberOfElements  (GAM_g_stEngineStructure.hCameraList_lNumberOfElementsDyn)
#define GAM_g_hAlwaysActive_hLastElement       (GAM_g_stEngineStructure.hAlwaysActiveCharactersList_hLastElementDyn)
#define GAM_g_hAlwaysActive_lNumberOfElements  (GAM_g_stEngineStructure.hAlwaysActiveCharactersList_lNumberOfElementsDyn)

/* 0x4543c0 */
extern unsigned char fn_bLoadCurrentGameOptions(void);

extern char *__cdecl FIL_pc_fn_szGetDataDir(void);
extern char *__cdecl FIL_pc_fn_szGetSoundDir(void);
extern void __cdecl MTH_fn_vInit(void);
extern void __cdecl GLI_xInitGli(void);
extern void __cdecl FUN_00409c80(void);
extern void __cdecl GAM_fn_vCreateGameDeviceAndViewPorts(void);
extern void __cdecl FUN_0045edc0(unsigned short, unsigned short, void *);
extern void __cdecl FUN_0045ec40(void);
extern void __cdecl FUN_00407760(void);
extern unsigned char __cdecl FUN_0044f4f0(void);

extern void __cdecl GAM_pc_fn_vProcessExternalCtrlCmds();
extern int __cdecl SND_fn_iInitSound(void *pInitStruct);
extern void __cdecl SND_fn_vSetDopplerFactorSound(long factor);

extern int __cdecl SND_fn_bInitDataLoadSnd(const char *_szFileTitle);
extern void __cdecl GAM_pc_fn_vCloseExternalCtrlFile(void);
extern void __cdecl FUN_0044d420(void);

#define GLI_C_lHasNoMirror 0x10000000

#include "GAM/STRUCTUR/Engmode.h"

#define GAM_g_h_StdCamCaracter (GAM_g_stEngineStructure.h_StdCamCaracter)

/* 0x40b950 */
extern void __cdecl fn_vStartEngineClock(void);
extern void __cdecl fn_vInitAllObjectsWhenPlayerDead(void);
extern void __cdecl IPT_fn_vActivateAllEntryElements(void);
extern void *__cdecl GAM_p_stGetBlockEventSlotMemory(unsigned long);
extern long __cdecl SND_fn_lSendRequestSound(long, long, void *, long, void *);

/* 0x40b950 */
extern long __cdecl GLD_bSetBackgroundColorForDevice(short, unsigned long);
extern void __cdecl SND_fn_vKillAllObjectTypeSound(long);
extern void __cdecl fn_vForceReinitAllObjects(void);
extern void __cdecl GAM_fn_lSaveBlockEventSlotMemory(void *, unsigned long);
extern void __cdecl MOR_fn_vRemoveAllMorphedObjects(void);
extern void __cdecl MGT_fn_vLastDesInitMagnet(void);
extern long __cdecl GLD_bClearViewportWithChoice(short, short, short);

/* 0x004052A0 */
long __cdecl GLD_bDeleteViewport(short hDev_unused, short hVpPacked);

/* 0x4077D0 */
extern void __cdecl GEO_xSelectMemoryChannel(unsigned int uiMode);
extern void __cdecl GAM_fn_vSetGameMemoryInLevel(void);
extern void __cdecl AI_fn_vAIUseLevelMemory(void);

/* 0x00509E74 */
extern struct IPT_tdstInput_ IPT_g_hInputStructure;
#define GAM_pc_a_hINODevice   (IPT_g_hInputStructure.a_hINODevice)
#define GAM_pc_C_lNbINODevice IPT_E_NumberOfDeviceEvent

extern short __cdecl INO_pc_wCheckInitReturnZero(void *_pvIgnored);
extern void  __cdecl INO_pc_fn_vShutdown(void);

/* 0x403290 */
extern void FUN_00405810(void);
extern void __cdecl GAM_pc_fn_vCloseCdFallbackFile(void);
extern void __cdecl SND_fn_vDesInitSound(void);
extern void __cdecl fn_vLastDesinitFamilyStructure(void);
extern void FON_fn_vDesInitFonts(void);

unsigned char FUN_0044f4e0(void);
void GAM_pc_fn_vDeleteEngineFixViewport(void);
void __cdecl GAM_pc_fn_vShutdownInputDevices(void);
void FUN_00403060(void)
{

    /* 0x00421800 */
    FUN_00405810();
    fn_bSaveCurrentGameOptions();
    GAM_pc_fn_vDeleteEngineFixViewport();
    GLI_vCloseGli();
    if (FUN_0044f4e0() == 1)
        GAM_pc_fn_vShutdownInputDevices();
    else
        GAM_pc_fn_vShutdownInputDevices();
    if (FUN_0044f4e0() == 2)
        FUN_00405810();
    GAM_pc_fn_vCloseCdFallbackFile();
    SND_fn_vDesInitSound();
    SND_PrintUsedStaticMemory();
    if (FUN_0044f4e0() != 1)
        fn_vLastDesinitFamilyStructure();
    SND_PrintUsedStaticMemory();
    FON_fn_vDesInitFonts();
}

/* 0x00492700 */
extern char *__cdecl FUN_00492700(void);
unsigned char __cdecl fn_bPlayerSaveGameGeneral(char *szPath);   /* 0x004103f0 */
unsigned char __cdecl fn_bLevelSaveGameItSelf(char *szPath);     /* 0x00410400 */

extern void (__cdecl *p_fn_vEngine)(void);       /* 0x0049F37C */
extern void (__cdecl *p_fn_vDisplayAll)(void);   /* 0x0049F380 */
void __cdecl GAM_fn_vEngine(void);
void __cdecl fn_vDisplayAll(void);

/* 0x004044A0 */
extern void __cdecl GLI_xSetViewportCamera(short hDevice, short hViewport, long lCam);

/* 0x500554 */
extern void __cdecl SND_fn_vKillAllObjectSound(void);

extern void __cdecl fn_vDesinitFamilyStructureForPriority(unsigned char ucPriority);
extern void __cdecl fn_vDestroyAllHierarchy(HIE_tdstSuperObject *p_stSuperObject);
extern void __cdecl fn_vRemoveAllMicros(void);
extern void __cdecl GAM_pc_fn_vFreeLevelMemoryUseFix(void);

extern void __cdecl GLI_pc_fn_vUnloadTexturesOfKey(unsigned long ulKey);
extern void __cdecl GEO_xDeleteMemoryChannel(unsigned long ulChannel);
extern void __cdecl Mmg_fn_vFreeBlock(unsigned char ucBlockId, unsigned char ucModuleId, void *p_stMyBlocksInfo);
extern void __cdecl Mmg_fn_vDeleteBlock(unsigned char ucBlockId, unsigned char ucModuleId, void *p_stMyBlocksInfo);
extern void __cdecl fn_vFreeAILevelMemory(void);
extern void __cdecl fn_vDeleteAILevelMemory(void);
extern void __cdecl GEO_xSelectMemoryChannel(unsigned int uiMode);
extern void __cdecl GAM_fn_vSetGameMemoryInFix(void);

extern void __cdecl AI_fn_vAIUseFixMemory(void);   /* 0x00466840 */

/* 0x00440AE0 */
extern void __cdecl MGT_pc_fn_vDestroyModifiedObject(short sModuleIdx);
extern void __cdecl MGT_pc_fn_vDestroyMagnetModification(MGT_tdstMagnetModification *p_stMod);
extern void __cdecl MGT_pc_fn_vReleaseActiveMagnetsClearMods(void);

/* 0x0045EDA0 */
extern void __cdecl FIL_fn_vCloseConcatFile(void **hConcatFile);
extern void __cdecl FIL_fn_vFreeGlobalHandleToFileInConcatFile(void);
void __cdecl GAM_pc_fn_vLoadFixStreams(void)
{
    extern unsigned int *GAM_pc_g_p_ul4b72f0;               /* 0x4B72F0 */
    extern POS_tdstCompletePosition *POS_g_p_stIdentityMatrix; /* 0x510BA0 */
    extern long HIE_g_lNbMatrixInStack;                     /* 0x4B98D4 */
    extern unsigned long g_hCollisionGeoObj;                /* 0x50036C */
    extern unsigned long g_hStaticCollisionGeoObj;          /* 0x501404 */
    extern void *GAM_g_pAnimStack500288;
    extern void *GAM_g_pAnimStack500298;
    extern int *p_stFramesKF;                               /* 0x500274 */
    extern short *p_stAngularSpeedQuats;                    /* 0x5002A0 */
    extern struct IPT_tdstInput_ IPT_g_hInputStructure;     /* 0x509E60 */
    extern struct IPT_tdstKeyAndPadDefine_ *IPT_gd_hKeyAndPadDefine; /* 0x50A980 */
    extern void *IPT_g_hEngineMonitoredEntry;               /* 0x5002C0 */

    char szFixSnaPath[0x218];
    char szFixGptPath[0x114];
    void *hSavedLevelNameList;               /* 0x500588 */
    unsigned long ulSavedEventHistoric;      /* 0x50A578 */
    unsigned long aulSavedTableA[19];        /* 0x509E74 */
    unsigned char aucSavedFlagsB[19];        /* 0x509E61 */
    unsigned char ucSavedOnePad;             /* 0x509E60 */
    unsigned char ucSavedAzerty;             /* 0x50A53E */
    unsigned char aucFixBlock5002C0[0xAC];   /* 0x5002C0 */
    unsigned long *pulCur;
    unsigned long *pulEnd;
    unsigned long *pulTpl;
    unsigned int   uiIdx;

    hSavedLevelNameList = GAM_g_stEngineStructure.hLevelNameList;
    SNA_pc_vVerifyCdLevelsFile();
#ifndef RAYMAN2_NO_PROTECTION
    if (FUN_00418960() != 0)
        gam_fix_rekey_reloc_table();
#endif /* RAYMAN2_NO_PROTECTION */

    sprintf(szFixSnaPath, "%s\\Fix.sna", FUN_00492770());
    SNA_g_ucNextRelocationTableToLoad = 0xff;
    SNA_pc_g_ucNextRelocationTableKind = 0;
    SNA_pc_vLoadAndRelocate(szFixSnaPath);
#ifndef RAYMAN2_NO_PROTECTION
    if (FUN_00418960() != 0)
        gam_fix_rekey_reloc_table();
#endif /* RAYMAN2_NO_PROTECTION */

    SNA_g_ucNextRelocationTableToLoad = 0xff;
    SNA_pc_g_ucNextRelocationTableKind = 1;
    sprintf(szFixGptPath, "%s\\Fix.gpt", FUN_00492770());
    SNA_pc_vOpenGptFile(szFixGptPath);
    POS_g_p_stIdentityMatrix = (POS_tdstCompletePosition *)SNA_pc_ulReadAndRelocateLong();
    pulTpl = (unsigned long *)POS_g_p_stIdentityMatrix;
    pulTpl[0]  = 1UL;
    pulTpl[1]  = 0UL;
    pulTpl[2]  = 0UL;
    pulTpl[3]  = 0UL;
    pulTpl[4]  = 0x3f800000UL;
    pulTpl[5]  = 0UL;
    pulTpl[6]  = 0UL;
    pulTpl[7]  = 0UL;
    pulTpl[8]  = 0x3f800000UL;
    pulTpl[9]  = 0UL;
    pulTpl[10] = 0UL;
    pulTpl[11] = 0UL;
    pulTpl[12] = 0x3f800000UL;
    pulTpl[13] = 0x3f800000UL;
    pulTpl[14] = 0UL;
    pulTpl[15] = 0UL;
    pulTpl[16] = 0UL;
    pulTpl[17] = 0x3f800000UL;
    pulTpl[18] = 0UL;
    pulTpl[19] = 0UL;
    pulTpl[20] = 0UL;
    pulTpl[21] = 0x3f800000UL;

    pulCur = (unsigned long *)&HIE_g_a_hMatrixStack_backing[1];
    pulEnd = pulCur + 50;
    do {
        *pulCur = SNA_pc_ulReadAndRelocateLong();
        pulCur++;
    } while (pulCur != pulEnd);

    SNA_pc_vReadAndRelocateBlock((int *)&HIE_g_lNbMatrixInStack, 4);
    g_hCollisionGeoObj = SNA_pc_ulReadAndRelocateLong();
    g_hStaticCollisionGeoObj = SNA_pc_ulReadAndRelocateLong();
    SNA_pc_vReadAndRelocateBlock((int *)aucFixBlock5002C0, 0xAC);
    IPT_g_hEngineMonitoredEntry = *(void **)aucFixBlock5002C0;

    ulSavedEventHistoric = (unsigned long)IPT_g_hInputStructure.hEventHistoric;
    ucSavedAzerty = IPT_g_hInputStructure.ucKeyboardType;
    ucSavedOnePad = IPT_g_hInputStructure.ucOnePadActivate;
    memcpy(aucSavedFlagsB, IPT_g_hInputStructure.p_ucValideAndActiveDevice, sizeof(IPT_g_hInputStructure.p_ucValideAndActiveDevice));
    memcpy(aulSavedTableA, IPT_g_hInputStructure.a_hINODevice, sizeof(IPT_g_hInputStructure.a_hINODevice));

    IPT_gd_hKeyAndPadDefine = (struct IPT_tdstKeyAndPadDefine_ *)SNA_pc_ulReadAndRelocateLong();
    SNA_pc_vReadAndRelocateBlock((int *)&IPT_g_hInputStructure, sizeof(IPT_g_hInputStructure));

    IPT_g_hInputStructure.d_stEntryElementArray = (struct IPT_tdstEntryElement_ *)SNA_pc_ulReadAndRelocateLong();
    IPT_g_hInputStructure.hEventHistoric = (struct IPT_tdstHistoricElement_ *)ulSavedEventHistoric;
    memcpy(IPT_g_hInputStructure.a_hINODevice, aulSavedTableA, sizeof(IPT_g_hInputStructure.a_hINODevice));
    memcpy(IPT_g_hInputStructure.p_ucValideAndActiveDevice, aucSavedFlagsB, sizeof(IPT_g_hInputStructure.p_ucValideAndActiveDevice));
    IPT_g_hInputStructure.ucOnePadActivate = ucSavedOnePad;
    IPT_g_hInputStructure.ucKeyboardType = ucSavedAzerty;
    SNA_pc_vReadAndRelocateBlock((int *)FON_g_stGeneral, 0x14);

    SNA_pc_vReadRawArray(tdstStacks, eStackNbMax, sizeof(tdstStackInfos));

    p_stA3dGeneral = (tdstA3dGENERAL *)SNA_pc_ulReadAndRelocateLong();      /* 0x500278 */
    p_stSpeedVectors = (MTH3D_tdstVector *)SNA_pc_ulReadAndRelocateLong(); /* 0x500294 */
    p_stAngularSpeedQuats = (short *)SNA_pc_ulReadAndRelocateLong();        /* 0x5002A0 */
    p_stHierarchies = (tdstCouple *)SNA_pc_ulReadAndRelocateLong();         /* 0x500284 */
    p_stNTTO = (tdstNTTO *)SNA_pc_ulReadAndRelocateLong();                  /* 0x50029C */
    p_stOnlyFrames = (tdstOnlyFrame *)SNA_pc_ulReadAndRelocateLong();       /* 0x500290 */
    p_stChannels = (tdstA3dChannel *)SNA_pc_ulReadAndRelocateLong();        /* 0x50027C */
    p_stFrames = (tdstA3dFrame *)SNA_pc_ulReadAndRelocateLong();            /* 0x500280 */
    p_stFramesKF = (int *)SNA_pc_ulReadAndRelocateLong();                   /* 0x500274 */
    p_stKeyFrames = (tdstKeyFrame *)SNA_pc_ulReadAndRelocateLong();         /* 0x50028C */
    GAM_g_pAnimStack500298 = (void *)SNA_pc_ulReadAndRelocateLong();  /* 0x500298 */
    GAM_g_pAnimStack500288 = (void *)SNA_pc_ulReadAndRelocateLong();  /* 0x500288 */
    GAM_pc_g_p_ul4b72f0 = (unsigned int *)SNA_pc_ulReadAndRelocateLong(); /* 0x4B72F0 */

    SNA_pc_vCloseGptFile();
#ifndef RAYMAN2_NO_PROTECTION
    if (FUN_00418960() != 0)
        gam_fix_rekey_reloc_table();
#endif /* RAYMAN2_NO_PROTECTION */

    sprintf(szFixSnaPath, "%s\\Fix.ptx", FUN_00492770());
    FUN_00423610(szFixSnaPath);
    SNA_g_ucNextRelocationTableToLoad = 0xff;
    SNA_pc_g_ucNextRelocationTableKind = 3;
    FUN_00421820();
    GAM_g_stEngineStructure.hLevelNameList = hSavedLevelNameList;
    SNA_g_ucNextRelocationTableToLoad = 0xff;
    SNA_pc_g_ucNextRelocationTableKind = 2;

    sprintf(szFixGptPath, "%s\\Fix.snd", FUN_00492770());
    SNA_pc_vOpenSndFile(szFixGptPath);
    SNA_pc_vReadAndRelocateSnd(szFixGptPath);
#ifndef RAYMAN2_NO_PROTECTION
    if (FUN_00418960() != 0)
        gam_fix_rekey_reloc_table();
#endif /* RAYMAN2_NO_PROTECTION */
}
void fn_vInitGameLoop(void)
{

    /* 0x00449e80 */
    GAM_tdeEngineMode eCallingMode = GAM_g_stEngineStructure.eEngineMode;
    char *p_szName;

    GAM_fn_vSetEngineInLevelMode();
    fn_bSaveCurrentGameOptions();
    fn_vReinitEngineClock();
    HIE_fn_vInitGlobalMatrixFrameCounter(0);

    if (fn_bIsNextLevelNameValid())
    {
        p_szName = fn_p_szGetNextLevelName();
        fn_vSetLevelName(p_szName);
        fn_vFreeNextLevelName();
    }
    else
    {
        p_szName = fn_p_szGetFirstLevelName();
        fn_vSetLevelName(p_szName);
    }

    fn_vInitAllObjectsWhenNewGameStart();
    SAI_fn_bInitAllWhen(SAI_eInitWhenNewGameStart);   /* 0x449e80 */
    fn_vChangeEngineMode(E_EM_ModeEnterLevel);
    fn_vInitLevelLoop();                         /* 0x403730 */
    fn_vChangeEngineMode(eCallingMode);
    fn_vTestUnusedComport();
    TFMT_vInitTextAfficheStructure();
}

void fn_vReinitTheStaticHierarchie(HIE_tdstSuperObject *hSuperObject);
void fn_vInitDeadLoop(void)
{

    /* 0x00423ab0 */
    void *pvDst;

    AI_g_ulTrameNumber = 0;
    GAM_g_bEngineFrozen = 0;
    GAM_g_stEngineStructure.cCameraMode = C_CamModeNormal;
    fn_vStartEngineClock();
    fn_vReinitTheStaticHierarchie((HIE_tdstSuperObject *)SECT_hFatherSector);
    CAM_fn_vActualizeAViewport(1);   /* 0x0040b950 */

    if (GAM_g_stEngineStructure.eEngineMode != E_EM_ModeEnterLevel && GAM_g_stEngineStructure.eEngineMode != E_EM_ModeEnterGame)
    {
        HIE_tdstSuperObject *hMaincharacter = GAM_pc_g_hMainCharacterSO;

        if (M_GetMSHandle(hMaincharacter, StandardGame)->ucHitPoints > 0)
        {
            SAI_fn_bInitAllWhen(SAI_eInitWhenPlayerDead);   /* 0x449e80 */
            fn_vInitAllObjectsWhenPlayerDead();
            IPT_fn_vActivateAllEntryElements();
        }
        else
        {
            fn_vReinitTheMap();
        }
        if (GAM_g_stEngineStructure.bResurection != 0)
        {
            if (hMaincharacter != NULL)
            {

                pvDst = SAI_fn_p_vGetTableValuePointerOf((void *)&M_GetMSHandle(hMaincharacter, 3dData)->stGLIObjectMatrix, 0x32);
                memcpy(pvDst, &GAM_g_stMainCharacterPosition, 0x16 * 4);
            }
            if (GAM_g_h_StdCamCaracter != NULL)
            {
                pvDst = SAI_fn_p_vGetTableValuePointerOf((void *)&M_GetMSHandle(GAM_g_h_StdCamCaracter, 3dData)->stGLIObjectMatrix, 0x32);
                memcpy(pvDst, &GAM_g_stMainCameraPosition, 0x16 * 4);
            }
        }
        GAM_g_stEngineStructure.bResurection = 0;
        g_hSOCurrentMainSector = NULL;
        *(unsigned char *)GLI_g_cGlobalFogIsOn_ADDR = 0;   /* 0x004A7248 */
        FUN_00423ab0();
    }

    /* 0x40b040 */
    fn_vReputCharacterSuperObjectAtTheWorld(GAM_pc_g_hMainCharacterSO);

    if (GAM_g_stEngineStructure.eEngineMode == E_EM_ModeDeadLoop && GAM_pc_g_ucMapMusicIsOn != 0)
    {
        int iEvt = (int)GAM_p_stGetBlockEventSlotMemory(0);
        SND_fn_lSendRequestSound(-1, GAM_g_lSoundObjectTypeMusic, (void *)iEvt, 0, (void *)0);
    }
}

/* 0x00403280 */
void fn_vDesInitDeadLoop(void)
{
    fn_vDestructAllFatherSonLinks();  /* 0x0040b0e0 */
    fn_vKillAllAlways();              /* 0x0040c200 */
    AGO_vInit();                      /* 0x0045ffa0 */
}
void fn_vReinitTheMap(void)
{

    GLD_bSetBackgroundColorForDevice((unsigned long)FUN_00401340(0), 0);
    SND_fn_vKillAllObjectTypeSound(GAM_g_lSoundObjectTypeAnim);
    POS_fn_vSetIdentityMatrix(&GAM_g_stMainCharacterPosition);
    POS_fn_vSetIdentityMatrix(&GAM_g_stMainCameraPosition);
    GAM_g_stEngineStructure.bResurection = 0;
    fn_vKillAllAlways();
    fn_vDestructAllFatherSonLinks();  /* 0x0040b0e0 */
    SAI_fn_bInitAllWhen(SAI_eInitWhenReinitTheMap);   /* 0x449e80 */
    fn_vForceReinitAllObjects();
    fn_vReinitTheStaticHierarchie((HIE_tdstSuperObject *)SECT_hFatherSector);
    CAM_fn_vActualizeAViewport(1);
    FUN_00423ab0();
    if (GAM_pc_g_ucMapMusicIsOn != 0)
        GAM_fn_lSaveBlockEventSlotMemory((void *)GAM_pc_g_hMapMusicSound, 0);
    MOR_fn_vRemoveAllMorphedObjects();
    MGT_fn_vLastDesInitMagnet();
    *(unsigned char *)GLI_g_cGlobalFogIsOn_ADDR = 0;   /* 0x004A7248 */
    SPO_ReinitSuperimposedArray();
    *(int *)&g_hSOCurrentMainSector = 0;
    *(int *)&AI_g_iNumLoop = 0;
    IPT_fn_vActivateAllEntryElements();
    TFMT_vInitTextAfficheStructure();
    AGO_vInit();   /* 0x0045ffa0 */
    GLD_bClearViewportWithChoice(GAM_g_stEngineStructure.hGLDDevice, GAM_g_stEngineStructure.hGLDViewport, 1);
    fn_vStartEngineClock();
}
void fn_vReinitTheStaticHierarchie(HIE_tdstSuperObject *hSuperObject)
{
    HIE_tdstSuperObject *hChild;

    HIE_fn_vSetSuperObjectDrawMask(hSuperObject, HIE_fn_lGetSuperObjectDrawMask(hSuperObject) | ~GLI_C_lHasNoMirror);
    for (hChild = hSuperObject->p_stFirstChild; hChild != NULL; hChild = hChild->p_stNextBrother)
    {
        fn_vReinitTheStaticHierarchie(hChild);
    }
}

void __cdecl GAM_pc_fn_vCreateEngineGameMaterial(void);
void fn_vFirstInitEngine(void)
{

    /* 0x00408590 */
    char szPath[256];

    unsigned long aAppContext[3];

    /* 0x423610 */
    strcpy(szPath, FIL_pc_fn_szGetDataDir());
    strcat(szPath, (char *)GAM_ENG_szFixPtxPath);
    FUN_00423610(szPath);

    GAM_g_stEngineStructure.hGLDDevice = FUN_00401340(0);
    GAM_g_stEngineStructure.hGLDViewport = FUN_00401350(0);
    GAM_g_stEngineStructure.hDrawSem = fn_hGetDrawSemaphore();
    GAM_g_stEngineStructure.eInputMode = 0;
    GAM_g_stEngineStructure.eDisplayFixMode = 0xff;
    *(unsigned int *)&CAM_g_hCameraListHead = 0;
    GAM_g_hCameraList_hLastElement = 0;
    GAM_g_hCameraList_lNumberOfElements = 0;
    GAM_pc_g_hMainCharacterSO = 0;
    *(unsigned int *)&AI_g_p_stMainActor = 0;
    *(unsigned int *)&GAM_g_hFirstAlwaysActiveCharNode = 0;
    GAM_g_hAlwaysActive_hLastElement = 0;
    GAM_g_hAlwaysActive_lNumberOfElements = 0;
    AI_GES_ucPreviousLevel = 0;
    AI_GES_ucExitIdToQuitPrevLevel = 0;
    GAM_g_ucLevelGlobalCounter = 0;
    MTH_fn_vInit();
    GAM_g_ucIsEdInGhostMode = 0;
    fn_vFirstInitEngineClock();
    GAM_pc_fn_vCheckMemoryCanary();
    GLI_xInitGli();
    FUN_00409c80();
    GAM_fn_vCreateGameDeviceAndViewPorts();
    FUN_0045edc0(GAM_g_stEngineStructure.hGLDDevice, GAM_g_stEngineStructure.hGLDFixViewport,
                 GAM_g_stEngineStructure.hDrawSem);
    FUN_0045ec40();
    FUN_00407760();
    if (FUN_0044f4f0() == 1)
        GAM_pc_fn_vProcessExternalCtrlCmds(1);
    FUN_00406820();   /* 0x00406820 */
    if (FUN_0044f4e0() != 1)
        FUN_004668e0(0);
    SND_fn_vSetSoftDirectory(FIL_pc_fn_szGetSoundDir());
    GAM_pc_fn_vGetAppContext(aAppContext);
    SND_fn_iInitSound(aAppContext);
    SND_fn_vSetDopplerFactorSound(0);   /* 0x452a90 */
    SND_fn_bInitDataLoadSnd((char *)GAM_ENG_szGameName);
    if (FUN_0044f4f0() == 1)
    {
        GAM_pc_fn_vProcessExternalCtrlCmds(0xffff);
        GAM_pc_fn_vCloseExternalCtrlFile();
        GAM_g_ul4b7468 = 0;
    }
    fn_lInitSoundForAnim();          /* 0x0040a3e0 */
    fn_vInitMicrosFunction();        /* 0x00411c20 */
    GAM_pc_fn_bDeleteSaveSlot(0);
    SND_PrintUsedStaticMemory();
    GAM_pc_fn_vPruneDscLessDirectories();
    FUN_00407ad0();
    fn_v_UnclearPreviousLoadedAnims();
    GAM_pc_fn_vLoadFixStreams();   /* 0x00402960 */
    fn_bLoadCurrentGameOptions();
    PLA_fn_vCreateAndInitHeaps();   /* 0x0040f110 */
    fn_vFirstTimeInitCollisionGeometricObject();   /* 0x0040dd40 */
    POS_fn_vSetIdentityMatrix(&GAM_g_stMainCharacterPosition);  /* 0x004447e0 */
    POS_fn_vSetIdentityMatrix(&GAM_g_stMainCameraPosition);  /* 0x004447e0 */
    GAM_g_stEngineStructure.bResurection = 0;
    SND_PrintUsedStaticMemory();
    FUN_0044c310(GAM_g_stEngineStructure.hGLDDevice);   /* 0x0044c310 */
    HIE_fn_vCreateMatrixStack();   /* 0x0045b740 */
    GAM_pc_fn_vCreateEngineGameMaterial();
    FUN_0044d420();
    TFMT_vSetDefaultValuesCharacter(1.0f, 2.0f, 0.0f);   /* 0x00408590 */
}

void fn_vReputGameViewportCamera(void);
void fn_vDesInitLevelLoop(void)
{

    /* 0x0040bb50 */
    SND_fn_vKillAllObjectSound();
    SND_fn_vSetDopplerFactorSound(0);    /* 0x452a90 */
    fn_vDesInitDeadLoop();           /* 0x00403280 */
    fn_vStopEngineClock();           /* 0x00409dc0 */
    SPO_ReinitSuperimposedArray();
    fn_vKillAllAlwaysModel();
    fn_vLastDesinitAlwaysStructure();
    MGT_fn_vLastDesInitMagnet();
    fn_vDesinitFamilyStructureForPriority(1);
    fn_vDestroyAllHierarchy((HIE_tdstSuperObject *)GAM_g_ul5013c8);
    MOR_pc_fn_vCleanAllMorphTasks();
    GAM_g_ul4a6ac8 = 0;
    fn_vRemoveAllMicros();
    GAM_pc_fn_vFreeLevelMemoryUseFix();
    *(unsigned int *)&CAM_g_hCameraListHead = 0;
    GAM_g_hCameraList_hLastElement = 0;
    GAM_g_hCameraList_lNumberOfElements = 0;
    GAM_pc_g_hMainCharacterSO = 0;
    *(unsigned int *)&AI_g_p_stMainActor = 0;
    *(unsigned int *)&GAM_g_hFirstAlwaysActiveCharNode = 0;
    GAM_g_hAlwaysActive_hLastElement = 0;
    GAM_g_hAlwaysActive_lNumberOfElements = 0;
    fn_vReputGameViewportCamera();
}

/* 0x4046C0 */
unsigned char GAM_fn_bIsStoppingProgram(void)
{
    unsigned char ucReturn = 0;

    if (GAM_g_stEngineStructure.eEngineMode == E_EM_ModeStoppingProgram)
        ucReturn = 1;

    return ucReturn;
}

/* 0x005116ec */
unsigned char GAM_fn_bIsDesInitMode(void)
{
    unsigned char ucReturn = 0;

    if (GAM_g_stEngineStructure.eEngineMode == E_EM_ModeStoppingProgram ||
        GAM_g_stEngineStructure.eEngineMode == E_EM_ModeChangeLevel ||
        GAM_g_stEngineStructure.eEngineMode == E_EM_ModePlayerDead ||
        FUN_0044f4e0() == 2)
        ucReturn = 1;

    return ucReturn;
}

void GAM_fn_vActualizeAllGameViewportSize(void);
void fn_vValidateAllOption(void);

void GAM_fn_vChooseTheGoodInit(void)
{

    /* 0x402710 */
    unsigned char *p_byte0   = &SNA_g_ucNextRelocationTableToLoad;
    unsigned char *p_byte1   = &SNA_pc_g_ucNextRelocationTableKind;
    unsigned char *p_byte2   = &GAM_pc_g_ucAlwaysSpawnCounter;
    unsigned int  *p_ulLCG   = &MMG_g_ulLCGState;  /* 0x00510bf0 */
    unsigned char  ucRNGCount = GAM_g_stEngineStructure.ucNumberOfLevels;
    unsigned int   uVar;

    switch (GAM_g_stEngineStructure.eEngineMode)
    {
    case E_EM_ModeStartingProgram:
        fn_vFirstInitEngine();          /* 0x00403430 */
        fn_vFreeNextLevelName();
        fn_vChangeEngineMode(E_EM_ModeEnterGame);

    case E_EM_ModeEnterGame:

#ifndef RAYMAN2_NO_PROTECTION   /* random relocation-table load key */
        if (ucRNGCount == 0)
        {
            *p_byte0 = 0;
        }
        else
        {
            *p_ulLCG = gam_lcg_step(*p_ulLCG);
            *p_byte0 = (unsigned char)(*p_ulLCG % ucRNGCount);
        }
        uVar     = gam_lcg_step(*p_ulLCG);
        *p_byte1 = (unsigned char)(uVar % 3);
        *p_ulLCG = gam_lcg_step(uVar);
        *(unsigned int *)&SNA_g_ulTableIndex = 0;
        *p_byte2 = (unsigned char)(*p_ulLCG % 0x14);
#else
        /* Copy protection removed: every relocation table is read from its first copy in
           LEVELS0.DAT (load key byte 2 = 0); bytes 0 and 1 are set before each load. */
        *p_byte0 = 0;
        *p_byte1 = 0;
        *(unsigned int *)&SNA_g_ulTableIndex = 0;
        *p_byte2 = 0;
#endif /* RAYMAN2_NO_PROTECTION */

        GAM_fn_vActualizeAllGameViewportSize();
        fn_vInitGameLoop();                      /* 0x004030c0 */
        p_fn_vEngine     = GAM_fn_vEngine;       /* 0x0049F37C */
        p_fn_vDisplayAll = fn_vDisplayAll;       /* 0x0049F380 */
        fn_vChangeEngineMode(E_EM_ModePlaying);
        fn_vValidateAllOption();
        fn_vFreeNextLevelName();
        if (FUN_0044f4e0() == 2)
            fn_vChangeEngineMode(E_EM_ModeStoppingProgram);
        fn_vStartEngineClock();
        return;

    case E_EM_ModeEnterLevel:
        GAM_fn_vActualizeAllGameViewportSize();
        p_fn_vEngine     = GAM_fn_vEngine;       /* 0x0049F37C */
        p_fn_vDisplayAll = fn_vDisplayAll;       /* 0x0049F380 */
        fn_vInitLevelLoop();                     /* 0x403730 */
        fn_vChangeEngineMode(E_EM_ModePlaying);
        fn_vValidateAllOption();
        return;

    case E_EM_ModeDeadLoop:
        fn_vInitDeadLoop();             /* 0x00403150 */
        fn_vValidateAllOption();
        fn_vChangeEngineMode(E_EM_ModePlaying);
        return;

    default:
        ERM_fn_vClearLastError(0);
        ERM_fn_vUpdateLastError(0x30, g_ucGameModuleId, 0, 0x11,
                                -1, 0xff, 0xff, (char *)0);
        return;
    }
}

unsigned char fn_bPlayerSaveGameAll(void);
unsigned char fn_bLevelSaveGameAll(void);

/* 0x00404940 */
void fn_vChooseTheGoodDesInit(void)
{
    /* 0x402710 */
    unsigned char *p_bSaveFlag = &GAM_pc_g_ucMode6SaveFlag;

    switch (GAM_g_stEngineStructure.eEngineMode)
    {
    case E_EM_ModeStoppingProgram:
        fn_vDesInitLevelLoop();         /* 0x004044a0 */
        FUN_00403060();                     /* 0x00403060 */
        return;

    case E_EM_ModeChangeLevel:

        if (*p_bSaveFlag != 0)
        {
            fn_bPlayerSaveGameAll();                 /* 0x00410360 */
            fn_bLevelSaveGameAll();                 /* 0x004103a0 */
        }
        else
        {
            *p_bSaveFlag = 1;
        }
        fn_vDesInitLevelLoop();         /* 0x004044a0 */
        fn_vSetLevelName(fn_p_szGetNextLevelName());
        fn_vChangeEngineMode(E_EM_ModeEnterLevel);
        return;

    case E_EM_ModePlayerDead:
        fn_vDesInitDeadLoop();          /* 0x00403280 */
        fn_vChangeEngineMode(E_EM_ModeDeadLoop);
        return;

    default:
        ERM_fn_vClearLastError(0);
        ERM_fn_vUpdateLastError(0x30, g_ucGameModuleId, 0, 0x12,
                                -1, 0xff, 0xff, (char *)0);
        return;
    }
}

void fn_vDestroyFixViewPort(short _hDevice);

/* 0x00405170 */
void GAM_pc_fn_vDeleteEngineFixViewport(void)
{
    fn_vDestroyFixViewPort((short)GAM_g_stEngineStructure.hGLDDevice);
}

void fn_vDestroyFixViewPort(short _hDevice)
{
    GLD_bDeleteViewport(_hDevice, *(short *)&GAM_g_stEngineStructure.hGLDFixViewport);
}

void __cdecl fn_vChangeDetailsValue(unsigned long ulValue);
void __cdecl fn_vChangeMusicVolumeValue(unsigned long ulValue);
void __cdecl fn_vChangeFxVolumeValue(unsigned long ulValue);

/* 0x4084e0 */
void fn_vValidateAllOption(void)
{
    fn_vChangeDetailsValue(GAM_g_ul500248);
    fn_vChangeMusicVolumeValue(GAM_g_ul500254);
    fn_vChangeFxVolumeValue(GAM_g_ul500250);
}

void __cdecl fn_vChangeDetailsValue(unsigned long ulValue)
{
    GAM_g_ul500248 = ulValue;
}

void __cdecl fn_vChangeMusicVolumeValue(unsigned long ulValue)
{
    if (ulValue > 127)
        ulValue = 127;
    GAM_g_ul500254 = ulValue;
    SND_fn_vSetVolumeLineSound(GAM_g_lSoundVolumeLineMusic, ulValue);
}

/* 0x00408550 */
void __cdecl fn_vChangeFxVolumeValue(unsigned long ulValue)
{
    if (ulValue > 127)
        ulValue = 127;
    GAM_g_ul500250 = ulValue;
    SND_fn_vSetVolumeLineSound(GAM_g_lSoundVolumeLineSound, ulValue);
}

unsigned char fn_bPlayerSaveGameAll(void)
{
    char  szPath[260];

    sprintf(szPath, (char *)GAM_ENG_szGeneralSaveFmt, FUN_00492700(), GAM_ENG_pszHiddenSlot);
    /* 0x004103f0 */
    return fn_bPlayerSaveGameGeneral(szPath);
}

unsigned char fn_bLevelSaveGameAll(void)
{
    /* 0x00404db0 */
    char  szPath[260];

    sprintf(szPath, (char *)GAM_ENG_szSlotSaveFmt, FUN_00492700(), GAM_ENG_pszHiddenSlot,
            fn_p_szGetLevelName(), (int)((unsigned long)fn_lGetSubMapNumber() >> 0x10));
    /* 0x00410400 */
    return fn_bLevelSaveGameItSelf(szPath);
}

/* 0x004103f0 */
unsigned char __cdecl fn_bPlayerSaveGameGeneral(char *szPath)
{
    return SAI_fn_ucPlayerSaveValues(szPath);
}

unsigned char __cdecl fn_bLevelSaveGameItSelf(char *szPath)
{
    return SAI_fn_ucLevelSaveValues(szPath);
}

/* 0x00410410 */
unsigned char fn_bLoadPlayerSaveGameAfterLoadLevel(void)
{
    char  szPath[260];

    sprintf(szPath, (char *)GAM_ENG_szGeneralSaveFmt, FUN_00492700(), GAM_ENG_pszHiddenSlot);
    /* 0x00410410 */
    return SAI_fn_ucLoadPlayerValues(szPath);
}

/* 0x00410450 */
unsigned char fn_bLoadLevelSaveGameAfterLoadLevel(void)
{
    char  szPath[260];

    sprintf(szPath, (char *)GAM_ENG_szSlotSaveFmt, FUN_00492700(), GAM_ENG_pszHiddenSlot,
            fn_p_szGetLevelName(), (int)((unsigned long)fn_lGetSubMapNumber() >> 0x10));
    return SAI_fn_ucLoadPlayerValues(szPath);
}

/* 0x00403430 */
void __cdecl GAM_pc_fn_vCreateEngineGameMaterial(void)
{
    GAM_pc_g_hEngineGameMaterial = GMT_fn_hCreateGameMaterial();
}

void __cdecl GAM_pc_fn_vShutdownInputDevices(void)
{
    struct INO_tdstDevice_ **ppDevice;

    ppDevice = GAM_pc_a_hINODevice;
    do {
        INO_pc_wCheckInitReturnZero((void *)ppDevice);
        ppDevice = ppDevice + 1;
    } while ((long)ppDevice < (long)&GAM_pc_a_hINODevice[GAM_pc_C_lNbINODevice]);
    INO_pc_fn_vShutdown();
}
void __cdecl MGT_fn_vLastDesInitMagnet(void)
{
    short sIdx;
    MGT_tdstModifiedObject **pp_stObj;
    int iSlot;
    int iRemain;

    sIdx = 0;
    pp_stObj = g_aModifiedObjects;
    if (0 < g_lNbPendingMagnets) {
        do {
            if (*pp_stObj != (MGT_tdstModifiedObject *)0) {
                iSlot = 0;
                iRemain = MGT_MAX_NUMBEROF_MAGNETS;
                do {
                    if ((*pp_stObj)->aDEF_hMagnet[iSlot] != (MGT_tdstMagnetModification *)0)
                        MGT_pc_fn_vDestroyMagnetModification((*pp_stObj)->aDEF_hMagnet[iSlot]);
                    iSlot = iSlot + 1;
                    iRemain = iRemain - 1;
                } while (iRemain != 0);
                MGT_pc_fn_vDestroyModifiedObject(sIdx);
            }
            sIdx = (short)(sIdx + 1);
            pp_stObj = pp_stObj + 1;
        } while (sIdx < g_lNbPendingMagnets);
    }
    g_lNbPendingMagnets = 0;
    MGT_pc_fn_vReleaseActiveMagnetsClearMods();
}
void __cdecl GAM_pc_fn_vCloseCdFallbackFile(void)
{
    extern void *GLI_pc_g_hCdFallbackFile;

    FIL_fn_vCloseConcatFile((void **)&GLI_pc_g_hCdFallbackFile);
    FIL_fn_vFreeGlobalHandleToFileInConcatFile();
}
