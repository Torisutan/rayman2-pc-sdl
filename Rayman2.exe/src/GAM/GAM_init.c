#include <windows.h>
#include <stdio.h>
#include <string.h>

#include "GAM/GAM_types.h"
#include "GAM/GAM_globals.h"
#include "GAM/GAM_pc_session.h"
#include "AI/AI_types.h"
#include "SAI/SAI_types.h"
#include "SAI/SAI_globals.h"
#include "SAI/SAI_Codec.h"
#include "ERM/CpaError.h"
#include "MMG/MMG_alloc.h"
#include "MMG/MMG_globals.h"
#include "GEO/GEO_globals.h"
#include "MMG/SNA_reloc.h"
#include "MMG/SNA.h"
#include "MMG/SNA_file.h"   /* C-ANSI file mode: file calls to the C runtime */
#include "FIL/FIL_ini.h"   /* C-ANSI file mode: ini calls to the C runtime */
#include "FIL/File.h"
#include "IPT/IPT.h"
#include "RND/RND_globals.h"
#include "GLI/ErrGLI.h"
#include "ALW/ALW_globals.h"
#include "GAM/A3d.h"

static const char GAM_INIT_szRbMode[]      = "rb";
static const char GAM_INIT_szPathJoinFmt[] = "%s\\%s";
static const char GAM_INIT_szMallocAlloc[] = "Malloc allocation";
/* 0x0049F8C0 */
extern char *GAM_gs_pszHiddenSlot;

/* 0x004A7064 */
extern long GAM_gs_lNbSaveSlots;
#define GAM_INIT_ulSlotCount  (*(unsigned int *)&GAM_gs_lNbSaveSlots)

extern unsigned char g_ucErmModuleId;   /* 0x0049F14C */
static unsigned char GAM_pc_gs_ucErmChannelIdB = 0xFF;
#define GAM_pc_g_ucErmChannelIdA g_ucErmModuleId
#define GAM_pc_g_ucErmChannelIdB GAM_pc_gs_ucErmChannelIdB

extern void __cdecl Mmg_fn_vFirstInitMmgModule(unsigned char ucMaxNbChannel);
extern void __cdecl GMT_fn_vInitFor3DOS(void);
/* 0x00403290 */
extern void __cdecl SND_PrintUsedStaticMemory(void);
/* 0x00403290 */
extern void __cdecl Mmg_fn_cCheckAlignmentValidity(void);

extern void __cdecl GLI_fnv_SetDllFileName(const char *szBaseName);
extern void __cdecl GLI_fnv_SetDllName(const char *name);
extern void __cdecl GLI_fnv_SetDriverName(const char *name);
extern void __cdecl GLI_fnv_SetDeviceName(const char *name);
extern void __cdecl GLI_fnv_SetModeByString(char *value);
extern void __cdecl GLI_pc_fn_vParseTexturesMemIni(char *value);
extern void __cdecl FUN_00424fe0(unsigned int arg);

extern void __cdecl GEO_xInitGEOError(void);         /* 0x0041BBE0 */
extern void __cdecl IPT_fn_vFirstInitInput(void);    /* 0x0042DE50 */
extern void __cdecl RND_fn_vFirstInitRandom(void);   /* 0x00449A50 */
extern void __cdecl fn_vFirstInitGameMemory(void);   /* 0x004077A0 */
extern void __cdecl fn_vFirstInitAIMemory(void);     /* 0x00466770 */

/* 0x00449f00 */
extern void __cdecl SAI_fn_vFirstInit(void);
extern void __cdecl SAI_fn_vSAIUseFixMemory(void);   /* 0x00449fe0 */

/* 0x0045dbb0 */
static void FUN_0045dbb0(void)
{
    if (g_ucGliModuleId == 0xff)
        g_ucGliModuleId = Erm_fn_ucInitErrMsg();
    Mmg_fn_v_InitMmg(g_ucGliModuleId, 2, 0x186a0);
    Mmg_fn_cCheckAlignmentValidity();
}

/* 0x004928d0 */
extern unsigned char FIL_pc_g_ucInitMarker;
static void FUN_004928d0(void)
{
    if (FIL_g_ucFILModuleId == 0xff)
        FIL_g_ucFILModuleId = Erm_fn_ucInitErrMsg();
    FIL_pc_g_ucInitMarker = 0xff;
}

/* 0x0044d410 */
static void FUN_0044d410(void)
{
    SNA_g_ucLoadListMode = 1;
}

/* 0x0044d420 */
void __cdecl FUN_0044d420(void)
{
    SNA_g_ucLoadListMode = 0;
}

/* 0x0044f4f0 */
unsigned char __cdecl FUN_0044f4f0(void)
{
    return GAM_g_b4b7464;   /* 0x004b7464 */
}

/* 0x0044f510 */
void __cdecl GAM_pc_fn_vCloseExternalCtrlFile(void)
{
    if (GAM_pc_g_stExternalCtrlFile.hFile != INVALID_HANDLE_VALUE)
        SNA_pc_bCloseFile(&GAM_pc_g_stExternalCtrlFile);
}

/* 0x0044f4d0 */
void __cdecl FUN_0044f4d0(unsigned char ucValue)
{
    GAM_pc_g_ucGameEndCondition = ucValue;
}

/* 0x00466840 */
void __cdecl AI_fn_vAIUseFixMemory(void)
{
    AI_g_ucMemoryBlockUsed = 0;
}

/* 0x0041bc60 */
extern void __cdecl FUN_00421860(unsigned int arg);
void __cdecl GEO_xSelectMemoryChannel(unsigned int uiMode)
{
    if (5 < (unsigned char)uiMode && (unsigned char)uiMode != 0xff) {
        ERM_fn_vClearLastError(0);
        ERM_fn_vUpdateLastError(4, g_ucGEOModuleId, 0, 2, -1, 0xff, 0xff, (char *)GAM_INIT_szMallocAlloc);
    }
    g_ucGEOMMemMallocMode = (unsigned char)uiMode;
    FUN_00421860(uiMode);
}

/* 0x004077c0 */
void __cdecl GAM_fn_vSetGameMemoryInFix(void)
{
    GAM_g_ucHLMMemoryBlock = 0;
}

/* 0x004077E0 */
void *__cdecl fnp_vGameMallocInHLM(unsigned int ulSize)
{
    void *pMem;

    if (ulSize == 0) {
        ERM_fn_vClearLastError(0);
        ERM_fn_vUpdateLastError(0x30, g_ucGameModuleId, 0, 6, -1, 0xff, 0xff, NULL);
    }
    if (GAM_g_ucHLMMemoryBlock > 1) {
        ERM_fn_vClearLastError(0);
        ERM_fn_vUpdateLastError(0x30, g_ucGameModuleId, 0, 5, -1, 0xff, 0xff, NULL);
    }
    Mmg_fn_v_SetModeAlloc(g_ucGameModuleId, GAM_g_ucHLMMemoryBlock, 0);
    pMem = fn_p_vGenAlloc(ulSize, 0);
    if (pMem == NULL) {
        ERM_fn_vClearLastError(0);
        ERM_fn_vUpdateLastError(0x30, g_ucGameModuleId, 0, 7, -1, 0xff, 0xff, NULL);
    }
    memset(pMem, 0, ulSize);
    return pMem;
}

/* 0x00405810 */
void FUN_00405810(void)
{
    GEO_xSelectMemoryChannel(1);
    GAM_fn_vSetGameMemoryInFix();
    AI_fn_vAIUseFixMemory();
    SAI_fn_vSAIUseFixMemory();
}

/* 0x00492860 */
static void FUN_00492860(void)
{
    char  *szTemp = FIL_PATH_BUFFER(10);   /* 0x004fb508 */
    size_t ulLen;

    GetTempPathA(0x104, szTemp);

#ifdef RAYMAN2_FIX_BUGS
    /* FIX(BUGS.md#5) */
    /* an empty path (GetTempPathA failed) indexed the byte before the buffer */
    if (szTemp[0] == '\0')
        return;
#endif /* RAYMAN2_FIX_BUGS */
    if (szTemp[strlen(szTemp) - 1] == '\\' || szTemp[strlen(szTemp) - 1] == '/')
        szTemp[strlen(szTemp) - 1] = '\0';
}

/* 0x0044f8d0 */
unsigned int __cdecl GAM_pc_fn_ulReadExternalCtrlDword(void)
{
    unsigned int uC;
    SNA_fn_ulFRead(&uC, 4, 1, &GAM_pc_g_stExternalCtrlFile);
    return uC;
}

/* 0x0044f8b0 */
void __cdecl GAM_pc_fn_vReadExternalCtrlDwordInto(unsigned int *_p_uDst)
{
    SNA_fn_ulFRead(_p_uDst, 4, 1, &GAM_pc_g_stExternalCtrlFile);
}

/* 0x0044f930 */
void __cdecl GAM_pc_fn_vReadExternalCtrlBytes(void *_p_vBuffer, size_t _ulBytes)
{
    SNA_fn_ulFRead(_p_vBuffer, 1, _ulBytes, &GAM_pc_g_stExternalCtrlFile);
}

/* 0x0044f8f0 */
void __cdecl GAM_pc_fn_vReadExternalCtrlLengthPrefixedBytes(void *_p_vBuffer)
{
    unsigned int uwLength;

    SNA_fn_ulFRead(&uwLength, 2, 1, &GAM_pc_g_stExternalCtrlFile);
    SNA_fn_ulFRead(_p_vBuffer, 1, uwLength & 0xffffu, &GAM_pc_g_stExternalCtrlFile);
}

/* 0x0044f9a0 */
void __cdecl GAM_pc_fn_vReadExternalCtrlLevelNames(void)
{
    unsigned char ucFirstWord[4];
    unsigned int  ulTag;
    char          szName[52];
    unsigned int  ulIndex;

    GAM_pc_fn_vReadExternalCtrlDwordInto((unsigned int *)ucFirstWord);
    GAM_g_stEngineStructure.ucNumberOfLevels = 0;
    GAM_g_stEngineStructure.ucCurrentLevel = ucFirstWord[0];
    GAM_pc_fn_vReadExternalCtrlDwordInto(&ulTag);
    while (ulTag != 0xffff)
    {
        if (ulTag == 0x1f)
        {
            GAM_pc_fn_vReadExternalCtrlLengthPrefixedBytes(szName);
            if (g_cIsLevelOk == 0 &&
                (GAM_g_stEngineStructure.ucNumberOfLevels == GAM_g_stEngineStructure.ucCurrentLevel ||
                 GAM_g_stEngineStructure.ucNumberOfLevels == 0))
            {
                fn_vSetFirstLevelName(szName);
            }
            ulIndex = GAM_g_stEngineStructure.ucNumberOfLevels;
            GAM_g_stEngineStructure.ucNumberOfLevels = (unsigned char)(GAM_g_stEngineStructure.ucNumberOfLevels + 1);
            strcpy(GAM_g_stEngineStructure.a_szLevelName[ulIndex], szName);
        }
        GAM_pc_fn_vReadExternalCtrlDwordInto(&ulTag);
    }
}

/* 0x0044fa80 */
extern void __cdecl RND_fn_vComputeRandomTable(void);
extern void __cdecl RND_fn_vRemapRandomTable(void);
void __cdecl GAM_pc_fn_vReadExternalCtrlRandomTable(void)
{
    unsigned int ulTag;
    unsigned int ulSize;

    GAM_pc_fn_vReadExternalCtrlDwordInto(&ulSize);
    RND_g_stRandomStructure.ulSizeOfTable = ulSize;
    Mmg_fn_vInitSpecificBlock(0, (unsigned char)ulSize,
                              g_a_p_stMmgModuleBlocksInfo[g_ucRNDModuleId],
                              ulSize * 4 + 4, 100000, 1, 0);
    Mmg_fn_v_SetModeAlloc(g_ucRNDModuleId, 0, 0);
    RND_g_stRandomStructure.p_ulTable = (unsigned long *)fn_p_vGenAlloc(RND_g_stRandomStructure.ulSizeOfTable * 4, 0);

    GAM_pc_fn_vReadExternalCtrlDwordInto(&ulTag);
    while (ulTag != 0xffff)
    {
        if (ulTag == 0x21)
        {
            RND_fn_vComputeRandomTable();
            RND_fn_vRemapRandomTable();
        }
        else if (ulTag == 0x22)
        {
            GAM_pc_fn_vReadExternalCtrlBytes(RND_g_stRandomStructure.p_ulTable, RND_g_stRandomStructure.ulSizeOfTable * 4);
            RND_fn_vRemapRandomTable();
        }
        GAM_pc_fn_vReadExternalCtrlDwordInto(&ulTag);
    }
}

/* 0x00501420 */
static char GAM_pc_szDirTail[16];
#include "SND/sndtype.h"
#include "SND/sndfile.h"

/* 0x0044fb60 */
void __cdecl GAM_pc_fn_vReadExternalCtrlDirectories(void)
{
    unsigned int ulTag;
    char         szName[52];
    char         szPath0[256];
    char         szPath1[256];
    char         szPath2[256];

    GAM_pc_fn_vReadExternalCtrlDwordInto(&ulTag);
    while (ulTag != 0xffff)
    {
        switch (ulTag)
        {
        case 0x29:
            GAM_pc_fn_vReadExternalCtrlLengthPrefixedBytes(szName);
            FIL_pc_fn_vSetSoundDir(szName);
            break;
        case 0x2a:
            GAM_pc_fn_vReadExternalCtrlLengthPrefixedBytes(szName);
            break;
        case 0x2b:
            GAM_pc_fn_vReadExternalCtrlLengthPrefixedBytes(szName);
            strcpy(szPath0, FIL_pc_fn_szGetDataDir());
            strcat(szPath0, szName);
            FUN_00492710(szPath0);
            break;
        case 0x2c:
            GAM_pc_fn_vReadExternalCtrlLengthPrefixedBytes(szName);
            strcpy(szPath0, FIL_pc_fn_szGetDataDir());
            strcat(szPath0, szName);
            FUN_00492740(szPath0);
            break;
        case 0x2d:
            GAM_pc_fn_vReadExternalCtrlLengthPrefixedBytes(szName);
            strcpy(szPath0, FIL_pc_fn_szGetDataDir());
            strcat(szPath0, szName);
            FUN_004927b0(szPath0);
            strcpy(szPath1, FIL_pc_fn_szGetDataDir());
            strcat(szPath1, szName);
            FUN_00492780(szPath1);
            strcat(szPath1, "\\");
            SND_fn_vAddDataDirectory(szPath1);
            strcpy(szPath2, szPath1);
            strcat(szPath2, GAM_pc_szDirTail);
            strcat(szPath2, "\\");
            SND_fn_vAddDataDirectory(szPath2);
            break;
        case 0x2e:
            GAM_pc_fn_vReadExternalCtrlLengthPrefixedBytes(szName);
            strcpy(szPath0, FIL_pc_fn_szGetDataDir());
            strcat(szPath0, szName);
            FUN_004926d0(szPath0);
            break;
        case 0x30:
            GAM_pc_fn_vReadExternalCtrlLengthPrefixedBytes(szName);
            strcpy(szPath0, FIL_pc_fn_szGetDataDir());
            strcat(szPath0, szName);
            FUN_004927e0(szPath0);
            break;
        case 0x31:
            GAM_pc_fn_vReadExternalCtrlLengthPrefixedBytes(szName);
            strcpy(szPath0, FIL_pc_fn_szGetDataDir());
            strcat(szPath0, szName);
            FUN_00492820(szPath0);
            break;
        default:
            break;
        }
        GAM_pc_fn_vReadExternalCtrlDwordInto(&ulTag);
    }
}

/* 0x004A22A4 */
unsigned char GAM_pc_g_ucLevelMemInitPasses = 2;

void __cdecl GAM_fn_vSetGameMemoryInLevel(void);
void __cdecl GEO_xCreateMemoryChannel(unsigned char ucChannel, unsigned long ulSize);
void __cdecl fn_vInitAIFixMemory(unsigned long ulSize);
void __cdecl fn_vInitAILevelMemory(unsigned long ulSize);
void __cdecl AI_fn_vAIUseLevelMemory(void);
void __cdecl GLI_pc_fn_vInitGliBlock0(uint32_t ulSize);
void __cdecl GLI_pc_fn_vInitGliBlock1(uint32_t ulSize);
void __cdecl IPT_fn_vInitMemory(unsigned long _ulMemSize);
void __cdecl SAI_fn_vInitFixMemory(unsigned long _ulMemSize);
void __cdecl SAI_fn_vInitLevelMemory(unsigned long _ulMemSize);
void __cdecl SAI_fn_vSAIUseLevelMemory(void);
void FON_fn_vFirstInitFONMemory(unsigned long _ulFontTextMemorySize);

/* 0x00450000 */
void __cdecl GAM_pc_fn_vReadExternalCtrlCommands(void)
{
    unsigned int uCmd;

    GAM_pc_fn_vReadExternalCtrlDwordInto(&uCmd);
    while (uCmd != 0xffff)
    {
        switch (uCmd)
        {
        case 1:
            Mmg_fn_vInitSpecificBlock(0, g_ucGameModuleId, g_a_p_stMmgModuleBlocksInfo[g_ucGameModuleId],
                                      GAM_pc_fn_ulReadExternalCtrlDword(), 100000, 1, 0);
            GAM_fn_vSetGameMemoryInFix();
            break;
        case 2:
            GEO_xCreateMemoryChannel(1, GAM_pc_fn_ulReadExternalCtrlDword());
            GEO_xSelectMemoryChannel(1);
            break;
        case 3:
            GEO_xCreateMemoryChannel(0, GAM_pc_fn_ulReadExternalCtrlDword());
            break;
        case 4:
            fn_vInitAIFixMemory(GAM_pc_fn_ulReadExternalCtrlDword());
            AI_fn_vAIUseFixMemory();
            break;
        case 5:
            GLI_pc_fn_vInitGliBlock0(GAM_pc_fn_ulReadExternalCtrlDword());
            break;
        case 15:
            GLI_pc_fn_vInitGliBlock1(GAM_pc_fn_ulReadExternalCtrlDword());
            break;
        case 7:
            SAI_fn_vInitFixMemory(GAM_pc_fn_ulReadExternalCtrlDword());
            SAI_fn_vSAIUseFixMemory();
            break;
        case 6:
            IPT_fn_vInitMemory(GAM_pc_fn_ulReadExternalCtrlDword());
            break;
        case 8:
            FON_fn_vFirstInitFONMemory(GAM_pc_fn_ulReadExternalCtrlDword());
            break;
        case 16:
            GAM_pc_fn_ulReadExternalCtrlDword();
            GAM_pc_fn_ulReadExternalCtrlDword();
            break;
        case 11:
            {
                unsigned int uSize = GAM_pc_fn_ulReadExternalCtrlDword();
                if (GAM_pc_g_ucLevelMemInitPasses)
                    Mmg_fn_vInitSpecificBlock(1, g_ucGameModuleId,
                                              g_a_p_stMmgModuleBlocksInfo[g_ucGameModuleId],
                                              uSize, 100000, 1, 0);
            }
            GAM_fn_vSetGameMemoryInLevel();
            break;
        case 12:
            {
                unsigned int uSize = GAM_pc_fn_ulReadExternalCtrlDword();
                if (GAM_pc_g_ucLevelMemInitPasses)
                    fn_vInitAILevelMemory(uSize);
            }
            AI_fn_vAIUseLevelMemory();
            break;
        case 13:
            {
                unsigned int uSize = GAM_pc_fn_ulReadExternalCtrlDword();
                if (GAM_pc_g_ucLevelMemInitPasses)
                    GEO_xCreateMemoryChannel(2, uSize);
            }
            GEO_xSelectMemoryChannel(2);
            break;
        case 14:
            {
                unsigned int uSize = GAM_pc_fn_ulReadExternalCtrlDword();
                if (GAM_pc_g_ucLevelMemInitPasses)
                    SAI_fn_vInitLevelMemory(uSize);
            }
            SAI_fn_vSAIUseLevelMemory();
            break;
        }
        GAM_pc_fn_vReadExternalCtrlDwordInto(&uCmd);
    }
    if (GAM_pc_g_ucLevelMemInitPasses)
        GAM_pc_g_ucLevelMemInitPasses--;
}

/* 0x0044f530 */
static const char GAM_pc_szPleaseInsertCD[] = "Please insert the CD";

void __cdecl GAM_pc_fn_vOpenDsbFile(int lMode, const char *szOpenMode)
{
    /* 0x00492770 */
    extern char *__cdecl FUN_00492770(void);

    /* 0x004011c0 */
    extern void __cdecl GLI_FatalExitOrSleep(void);

    char szPath[256];
    char szOrig[256];
    char cMode;

    if (GAM_pc_g_stExternalCtrlFile.hFile != INVALID_HANDLE_VALUE)
    {
        GAM_pc_fn_vCloseExternalCtrlFile();
    }
    switch (lMode)
    {
    case 0:
        break;
    case 1:
        strcpy(szPath, FIL_pc_fn_szGetDataDir());
        strcat(szPath, "\\");
        strcat(szPath, "Game.dsb");
        break;
    case 2:
        strcpy(szPath, FUN_00492770());
        strcat(szPath, "\\");
        strcat(szPath, fn_p_szGetLevelName());
        strcat(szPath, "\\");
        strcat(szPath, fn_p_szGetLevelName());
        strcat(szPath, ".");
        strcat(szPath, "d");
        strcat(szPath, "s");
        strcat(szPath, "b");
        break;
    default:
        return;
    }

    cMode = (szOpenMode[0] == 'w') ? 1 : 2;
    if (SNA_pc_bOpenFile(szPath, cMode, 2, &GAM_pc_g_stExternalCtrlFile) == 0)
    {
        strcpy(szOrig, szPath);
        sprintf(szPath, "%c\\\\%s", g_cCDROM, szOrig);   /* 0x004A06D8 */
        while (SNA_pc_bOpenFile(szPath, cMode, 2, &GAM_pc_g_stExternalCtrlFile) == 0)
        {
            if (Erm_fn_iMessageBox((char *)GAM_pc_szPleaseInsertCD,
                                   (char *)GAM_pc_szPleaseInsertCD, 0x40035) == 2)
            {
                GLI_FatalExitOrSleep();
            }
        }
        strcpy(szPath, szOrig);
    }
}

/* 0x0044f870 */
void FUN_0044f870(unsigned long ulArg)
{
    /* 0x0044f530 */
    GAM_pc_fn_vOpenDsbFile((int)ulArg, GAM_INIT_szRbMode);
    GAM_pc_g_stExternalCtrlFile.ulValid =
        (unsigned long)ReadFile(GAM_pc_g_stExternalCtrlFile.hFile,
                                &GAM_pc_g_stExternalCtrlFile.uKey.ulState,
                                4, &ulArg, NULL);
}

extern long __cdecl GLI_fnl_InitDll(void);

/* 0x00492690 */
extern void __cdecl FIL_pc_fn_vSetDataDir(char *param_1);

/* 0x0044f500 */
void __cdecl FUN_0044f500(unsigned char ucValue)
{
    GAM_g_b4b7464 = ucValue;
}

/* 0x00404ba0 */
void FUN_00404ba0(void)
{
    char szData[8];
    /* 0x404BA3 */
    szData[0] = 'D';
    szData[1] = 'a';
    szData[3] = 'a';
    szData[4] = '\0';
    szData[2] = 't';
    FIL_pc_fn_vSetDataDir(szData);
    FUN_0044f500(1);
    FUN_0044f4d0(1);
}

#  define GLILOAD_ERR_MSG    "Graphics Dll not found, run install"  /* 0x0049f3c8 */
#  define GLILOAD_ERR_TITLE  "Setup problem"                        /* 0x0049f3ec */

#ifndef RAYMAN2_NO_PROTECTION   /* protection gate of FUN_00405850 */
void *GAM_pc_g_pvGliLoadResume;   /* 0x00511414 */
void *GAM_pc_g_pvGliLoadExit;     /* 0x00511334 */
#endif /* RAYMAN2_NO_PROTECTION */

static const char GAM_pc_c_szGliLoadErrMsg[]   = GLILOAD_ERR_MSG;     /* 0x0049F3C8 */
static const char GAM_pc_c_szGliLoadErrTitle[] = GLILOAD_ERR_TITLE;   /* 0x0049F3EC */
extern void __cdecl GLI_FatalExitOrSleep(void);   /* 0x004011C0 */

#define GAM_pc_M_vNop8 \
    __asm _emit 0x90 __asm _emit 0x90 __asm _emit 0x90 __asm _emit 0x90 \
    __asm _emit 0x90 __asm _emit 0x90 __asm _emit 0x90 __asm _emit 0x90

#ifdef RAYMAN2_NO_PROTECTION
/* Only the protection gate needs the naked body below (label addresses, room for a decrypted
   body); without it the function is plain C. */
void __cdecl GAM_pc_fn_vLoadGliDll(void)
{
    g_cIsLevelOk = 0;
    FUN_0044f500(0);
    if ( !GLI_fnl_InitDll() )
    {
        Erm_fn_iMessageBox((char *)GAM_pc_c_szGliLoadErrMsg, (char *)GAM_pc_c_szGliLoadErrTitle, 0x40010);
        GLI_FatalExitOrSleep();
    }
    FUN_00404ba0();
}
#else
__declspec(naked) void __cdecl GAM_pc_fn_vLoadGliDll(void)
{
    __asm {
        push    ebp
        mov     ebp, esp
        sub     esp, 0x28
#ifndef RAYMAN2_NO_PROTECTION   /* protection gate */
        mov     eax, dword ptr [SNA_g_lProtectionGate]
        test    eax, eax
        je      L_gate
        mov     dword ptr [GAM_pc_g_pvGliLoadResume], offset L_body
        mov     dword ptr [GAM_pc_g_pvGliLoadExit], offset L_epi
        mov     esp, ebp
        pop     ebp
        ret
L_gate:
#endif /* RAYMAN2_NO_PROTECTION */
        push    edi
        mov     byte ptr [g_cIsLevelOk], 0
#ifndef RAYMAN2_NO_PROTECTION   /* room for a decrypted body */
L_body:
        GAM_pc_M_vNop8 GAM_pc_M_vNop8 GAM_pc_M_vNop8 GAM_pc_M_vNop8 GAM_pc_M_vNop8
        GAM_pc_M_vNop8 GAM_pc_M_vNop8 GAM_pc_M_vNop8 GAM_pc_M_vNop8 GAM_pc_M_vNop8
        GAM_pc_M_vNop8 GAM_pc_M_vNop8 GAM_pc_M_vNop8
        __asm _emit 0x90 __asm _emit 0x90
L_epi:
#endif /* RAYMAN2_NO_PROTECTION */
        push    0
        call    FUN_0044f500
        add     esp, 4
        call    GLI_fnl_InitDll
        test    eax, eax
        jne     L_ok
        push    0x40010
        push    offset GAM_pc_c_szGliLoadErrTitle
        push    offset GAM_pc_c_szGliLoadErrMsg
        call    Erm_fn_iMessageBox
        add     esp, 0xC
        call    GLI_FatalExitOrSleep
L_ok:
        call    FUN_00404ba0
        pop     edi
        mov     esp, ebp
        pop     ebp
        ret
    }
}
#endif /* RAYMAN2_NO_PROTECTION */

/* 0x004468d0 */
void __cdecl GAM_pc_fn_vSetParticuleRate(float xRate)
{
    GAM_pc_g_xParticuleRate = xRate;
}

/* 0x00414e70 */
void __cdecl FUN_00414e70(float xRate)
{
    GAM_pc_fn_vSetParticuleRate(xRate);
}

#  define UBIINI_FILE        "\\Ubi.ini"        /* 0x0049f40c */
#  define UBIINI_SEC_GLI     "Rayman2"          /* 0x0049f2f0 */
#  define UBIINI_SEC_GAME    "RAYMAN2"          /* 0x0049f310 */
#  define UBIINI_EMPTY       ""                 /* 0x004a6abc */
#  define UBIINI_K_DLLFILE   "GLI_DllFile"      /* 0x0049f4ac */
#  define UBIINI_K_DLL       "GLI_Dll"          /* 0x0049f4a4 */
#  define UBIINI_K_DRIVER    "GLI_Driver"       /* 0x0049f498 */
#  define UBIINI_K_DEVICE    "GLI_Device"       /* 0x0049f48c */
#  define UBIINI_K_MODE      "GLI_Mode"         /* 0x0049f480 */
#  define UBIINI_K_TEXMEM    "TexturesMem"      /* 0x0049f470 */
#  define UBIINI_D_AGP       "Agp"              /* 0x0049f47c */
#  define UBIINI_K_PARTRATE  "ParticuleRate"    /* 0x0049f458 */
#  define UBIINI_D_MEDIUM    "Medium"           /* 0x0049f468 */
#  define UBIINI_V_NULL      "Null"             /* 0x0049f450 */
#  define UBIINI_V_LOW       "Low"              /* 0x0049f44c */
#  define UBIINI_V_HIGH      "High"             /* 0x0049f444 */
#  define UBIINI_K_SOUNDHD   "SoundOnHD"        /* 0x0049f434 */
#  define UBIINI_D_ZERO      "0"                /* 0x0049f440 */
#  define UBIINI_K_COMPLETE  "Complete"         /* 0x0049f420 */
#  define UBIINI_D_ONE       "1"                /* 0x0049f42c */

long GAM_pc_g_lSoundOnHD;    /* 0x004A6ADC */
long GAM_pc_g_lComplete;     /* 0x004A6AE0 */

/* 0x00405a30 */
static void FUN_00405a30(void)
{

    char szValue[256];
    char szIniPath[260];
    float xRate;
    int   bHasRate;

#ifndef RAYMAN2_LOCAL_UBIINI
    GetWindowsDirectoryA(szIniPath, 0x104);
#else
    strcpy(szIniPath, ".");   /* ubi.ini in the working directory (the game folder) */
#endif /* RAYMAN2_LOCAL_UBIINI */
    strcat(szIniPath, UBIINI_FILE);                              /* 0x0049f40c */

    /* 0x4a6abc */
    GetPrivateProfileStringA(UBIINI_SEC_GLI, UBIINI_K_DLLFILE, UBIINI_EMPTY, szValue, 0xff, szIniPath);
    GLI_fnv_SetDllFileName(szValue);   /* 0x00421e60 */
    GetPrivateProfileStringA(UBIINI_SEC_GLI, UBIINI_K_DLL, UBIINI_EMPTY, szValue, 0xff, szIniPath);
    GLI_fnv_SetDllName(szValue);   /* 0x00421e80 */
    GetPrivateProfileStringA(UBIINI_SEC_GLI, UBIINI_K_DRIVER, UBIINI_EMPTY, szValue, 0xff, szIniPath);
    GLI_fnv_SetDriverName(szValue);     /* 0x00421eb0 */
    GetPrivateProfileStringA(UBIINI_SEC_GLI, UBIINI_K_DEVICE, UBIINI_EMPTY, szValue, 0xff, szIniPath);
    GLI_fnv_SetDeviceName(szValue);     /* 0x00421ee0 */
    GetPrivateProfileStringA(UBIINI_SEC_GLI, UBIINI_K_MODE, UBIINI_EMPTY, szValue, 0xff, szIniPath);
    GLI_fnv_SetModeByString(szValue);           /* 0x00421f10 */
    GetPrivateProfileStringA(UBIINI_SEC_GLI, UBIINI_K_TEXMEM, UBIINI_D_AGP, szValue, 0xff, szIniPath);
    GLI_pc_fn_vParseTexturesMemIni(szValue);    /* 0x00424ef0 */

    GetPrivateProfileStringA(UBIINI_SEC_GAME, UBIINI_K_PARTRATE, UBIINI_D_MEDIUM, szValue, 0x104, szIniPath);
    if (_stricmp(szValue, UBIINI_V_NULL) == 0)                   /* 0x0049f450 */
        FUN_00414e70(0.0f);                  /* 0x00414e70 */
    bHasRate = 0;
    if (_stricmp(szValue, UBIINI_V_LOW) == 0)                    /* 0x0049f44c */
    {
        xRate = 1.0f; bHasRate = 1;
    }
    else if (_stricmp(szValue, UBIINI_D_MEDIUM) == 0)            /* 0x0049f468 */
    {
        xRate = 2.0f; bHasRate = 1;
    }
    else if (_stricmp(szValue, UBIINI_V_HIGH) == 0)              /* 0x0049f444 */
    {
        xRate = 3.0f; bHasRate = 1;
    }
    if (bHasRate)
        FUN_00414e70(xRate);                 /* 0x00414e70 */

    /* 0x0049f430 */
    GetPrivateProfileStringA(UBIINI_SEC_GAME, UBIINI_K_SOUNDHD, UBIINI_D_ZERO, szValue, 0x104, szIniPath);
    sscanf(szValue, "%d", &GAM_pc_g_lSoundOnHD);
    GetPrivateProfileStringA(UBIINI_SEC_GAME, UBIINI_K_COMPLETE, UBIINI_D_ONE, szValue, 0x104, szIniPath);
    sscanf(szValue, "%d", &GAM_pc_g_lComplete);
}

/* 0x0044f950 */
unsigned long __cdecl GAM_pc_fn_ulReadExternalCtrlCmd(void)
{
    unsigned long ulWord;
    if (GAM_pc_g_stExternalCtrlFile.hFile == INVALID_HANDLE_VALUE)
        return 0xffffffff;
    if (GAM_pc_g_stExternalCtrlFile.ulValid == 0)
        return 0xffff;
    if (SNA_fn_ulFRead(&ulWord, 4, 1, &GAM_pc_g_stExternalCtrlFile) < 1)
        return 0xffff;
    return ulWord;
}

void __cdecl GAM_pc_fn_vLoadExternalCtrlSoundBanks(void);

void __cdecl GAM_pc_fn_vSetLoadFileName(char *pszSrc);                 /* 0x00407B20 */
void __cdecl fn_vSetCurrentGameOptionsFileName(char *pszSrc);   /* 0x00407B50 */
void __cdecl GLD_vSetFrameSynchro(const char *szKey, const char *szValue, const char *szPercent); /* 0x00420D20 */
void __cdecl GLI_pc_fn_vOpenFileWithCdFallback(char *path);            /* 0x0045ECC0 */
void __cdecl GLI_fn_vCloseBigFileTextures(void);                         /* 0x004251B0 */
void __cdecl GLI_fn_vOpenBigFileTextures(char *p_szArchivePath);       /* 0x004250D0 */
void __cdecl IPT_pc_fn_vCreateMouseDevice(long _lParam);               /* 0x0042E040 */

#include "INO/INO.h"

#include "GLI/GLI_globals.h"

extern void __cdecl FUN_0045ec40(void);                                          /* 0x0045ec40 */
extern void __cdecl GLI_pc_fn_vShowProgressUiImage(char *p_stUiObject);        /* 0x0045ee10 */
extern void __cdecl GLI_DrawProgressBarFrameAndFill(int ui_object);            /* 0x0045f420 */
extern void __cdecl FUN_0045fc70(void *p_stUiObject);                          /* 0x0045fc70 */
extern void __cdecl FUN_0045fbf0(void *p_stUiObject, char *p_szName, int iFmt); /* 0x0045fbf0 */
extern long __cdecl SND_pc_fn_lGetTwicePlus25(void);                           /* 0x00451440 */

void __cdecl GAM_pc_fn_vReadExternalCtrlProgressUi(void)
{
    unsigned int ulTag;
    char         szName[152];

    GAM_pc_fn_vReadExternalCtrlDwordInto(&ulTag);
    while (ulTag != 0xffff)
    {
        switch (ulTag)
        {
        case 0x48:
            GAM_pc_fn_vReadExternalCtrlLengthPrefixedBytes(szName);
            strcpy(GLI_pc_g_stProgressUi.szName + 0x104, szName);
            FUN_0045fbf0(&GLI_pc_g_stProgressUi, szName, 2);
            break;
        case 0x47:
            GAM_pc_fn_vReadExternalCtrlLengthPrefixedBytes(szName);
            FUN_0045fc70(&GLI_pc_g_stProgressUi);
            FUN_0045fbf0(&GLI_pc_g_stProgressUi, szName, 2);
            break;
        case 0x49:
            FUN_0045ec40();
            break;
        case 0x4b:
            GLI_pc_fn_vShowProgressUiImage((char *)&GLI_pc_g_stProgressUi);
            GLI_pc_fn_vShowProgressUiImage((char *)&GLI_pc_g_stProgressUi);
            break;
        case 0x4c:
            GAM_pc_fn_vReadExternalCtrlBytes(GLI_pc_g_stProgressUi.a_xBorderBlend, sizeof(GLI_pc_g_stProgressUi.a_xBorderBlend));
            break;
        case 0x4d:
            GAM_pc_fn_vReadExternalCtrlBytes(GLI_pc_g_stProgressUi.a_xBackgroundBlend, sizeof(GLI_pc_g_stProgressUi.a_xBackgroundBlend));
            break;
        case 0x4e:
            GAM_pc_fn_vReadExternalCtrlBytes(GLI_pc_g_stProgressUi.a_xCornerBlend[0], sizeof(GLI_pc_g_stProgressUi.a_xCornerBlend[0]));
            GAM_pc_fn_vReadExternalCtrlBytes(GLI_pc_g_stProgressUi.a_xCornerBlend[1], sizeof(GLI_pc_g_stProgressUi.a_xCornerBlend[1]));
            GAM_pc_fn_vReadExternalCtrlBytes(GLI_pc_g_stProgressUi.a_xCornerBlend[2], sizeof(GLI_pc_g_stProgressUi.a_xCornerBlend[2]));
            GAM_pc_fn_vReadExternalCtrlBytes(GLI_pc_g_stProgressUi.a_xCornerBlend[3], sizeof(GLI_pc_g_stProgressUi.a_xCornerBlend[3]));
            break;
        case 0x4f:
            GLI_pc_g_lProgressBarLastPos = 0;
            GLI_pc_g_lProgressBarCurPos = 0;

            GLI_pc_g_lProgressBarStart =
                (long)(GAM_pc_fn_ulReadExternalCtrlDword() * (unsigned long)GLI_pc_g_stProgressUi.lBufferDimC / 0x280);
            GLI_pc_g_stProgressUi.lBarYStart =
                (long)(GAM_pc_fn_ulReadExternalCtrlDword() * (unsigned long)GLI_pc_g_stProgressUi.lBufferDim8 / 0x1e0);
            GLI_pc_g_lProgressBarEnd =
                (long)(GAM_pc_fn_ulReadExternalCtrlDword() * (unsigned long)GLI_pc_g_stProgressUi.lBufferDimC / 0x280);
            GLI_pc_g_stProgressUi.lBarYEnd =
                (long)(GAM_pc_fn_ulReadExternalCtrlDword() * (unsigned long)GLI_pc_g_stProgressUi.lBufferDim8 / 0x1e0);
            break;
        case 0x50:
            GLI_DrawProgressBarFrameAndFill((int)&GLI_pc_g_stProgressUi);
            GLI_pc_g_bProgressBarEnabled = 1;
            break;
        case 0x51:
            GAM_pc_fn_ulReadExternalCtrlDword();
            GLI_pc_g_lProgressBarDivisor = SND_pc_fn_lGetTwicePlus25();
            GLI_pc_g_lProgressBarAccum = 0;
            break;
        default:
            break;
        }
        GAM_pc_fn_vReadExternalCtrlDwordInto(&ulTag);
    }
}

/* 0x00450480 */
void __cdecl GAM_pc_fn_vReadExternalCtrlOpenDataFiles(void)
{
    char szPath[256];
    unsigned int uaName[38];
    unsigned int uCmd;

    GAM_pc_fn_vReadExternalCtrlDwordInto(&uCmd);
    while (uCmd != 0xffff)
    {
        switch (uCmd)
        {
        case 0x41:
            GAM_pc_fn_vReadExternalCtrlLengthPrefixedBytes(uaName);
            { extern void __cdecl GAM_pc_fn_vCloseCdFallbackFile(void);  /* 0x0045EDA0 */
              GAM_pc_fn_vCloseCdFallbackFile(); }
            strcpy(szPath, FIL_pc_fn_szGetDataDir());
            strcat(szPath, (char *)uaName);
            GLI_pc_fn_vOpenFileWithCdFallback(szPath);
            break;
        case 0x42:
            GAM_pc_fn_vReadExternalCtrlLengthPrefixedBytes(uaName);
            GLI_fn_vCloseBigFileTextures();
            strcpy(szPath, FIL_pc_fn_szGetDataDir());
            strcat(szPath, (char *)uaName);
            GLI_fn_vOpenBigFileTextures(szPath);
            break;
        default:
            break;
        }
        GAM_pc_fn_vReadExternalCtrlDwordInto(&uCmd);
    }
}

/* 0x004505e0 */
void __cdecl GAM_pc_fn_vReadExternalCtrlFileAndGldOptions(void)
{
    unsigned int uaKey[38];
    unsigned int uaValue[5];
    unsigned int uaPercent[5];
    unsigned int uCmd;

    GAM_pc_fn_vReadExternalCtrlDwordInto(&uCmd);
    while (uCmd != 0xffff)
    {
        switch (uCmd)
        {
        case 0x65:
            GAM_pc_fn_vReadExternalCtrlLengthPrefixedBytes(uaKey);
            GAM_pc_fn_vSetLoadFileName((char *)uaKey);
            break;
        case 0x66:
            GAM_pc_fn_vReadExternalCtrlLengthPrefixedBytes(uaKey);
            fn_vSetCurrentGameOptionsFileName((char *)uaKey);
            break;
        case 0x67:
            GAM_pc_fn_vReadExternalCtrlLengthPrefixedBytes(uaKey);
            GAM_pc_fn_vReadExternalCtrlLengthPrefixedBytes(uaValue);
            GAM_pc_fn_vReadExternalCtrlLengthPrefixedBytes(uaPercent);
            GLD_vSetFrameSynchro((char *)uaKey, (char *)uaValue, (char *)uaPercent);
            break;
        default:
            break;
        }
        GAM_pc_fn_vReadExternalCtrlDwordInto(&uCmd);
    }
}

/* 0x004506b0 */
void __cdecl GAM_pc_fn_vReadExternalCtrlInputDevices(void)
{
    unsigned int uCmd;
    unsigned int uDevice;

    GAM_pc_fn_vReadExternalCtrlDwordInto(&uCmd);
    while (uCmd != 0xffff)
    {
        switch (uCmd)
        {
        case 0x79:
            uDevice = GAM_pc_fn_ulReadExternalCtrlDword();
            if (IPT_g_hInputStructure.p_ucValideAndActiveDevice[IPT_E_Pad00DeviceEvent + uDevice] == IPT_C_ValideDevice)
            {
                INO_fn_wActivateDevice(IPT_g_hInputStructure.a_hINODevice[IPT_E_Pad00DeviceEvent + uDevice]);
                IPT_g_hInputStructure.p_ucValideAndActiveDevice[IPT_E_Pad00DeviceEvent + uDevice] = IPT_C_ActivatePadDevice;
                IPT_g_hInputStructure.ucOnePadActivate = IPT_C_ActivateDevice;
            }
            break;
        case 0x7a:
            uDevice = GAM_pc_fn_ulReadExternalCtrlDword();
            if (IPT_g_hInputStructure.p_ucValideAndActiveDevice[IPT_E_Pad00DeviceEvent + uDevice] == IPT_C_ValideDevice)
            {
                INO_fn_wActivateDevice(IPT_g_hInputStructure.a_hINODevice[IPT_E_Pad00DeviceEvent + uDevice]);
                IPT_g_hInputStructure.p_ucValideAndActiveDevice[IPT_E_Pad00DeviceEvent + uDevice] = IPT_C_ActivateJoystickDevice;
                IPT_g_hInputStructure.ucOnePadActivate = IPT_C_ActivateDevice;
            }
            break;
        case 0x7b:
            if (IPT_g_hInputStructure.p_ucValideAndActiveDevice[IPT_E_KeyboardDeviceEvent] == IPT_C_ValideDevice)
            {
                IPT_g_hInputStructure.p_ucValideAndActiveDevice[IPT_E_KeyboardDeviceEvent] = IPT_C_ActivateDevice;
            }
            break;
        case 0x7c:
            IPT_pc_g_bMouseEnabled = 1;
            IPT_pc_fn_vCreateMouseDevice(0);
            if (IPT_g_hInputStructure.p_ucValideAndActiveDevice[IPT_E_MouseDeviceEvent] == IPT_C_ValideDevice)
            {
                IPT_g_hInputStructure.p_ucValideAndActiveDevice[IPT_E_MouseDeviceEvent] = IPT_C_ActivateMouseDevice;
            }
            break;
        }
        GAM_pc_fn_vReadExternalCtrlDwordInto(&uCmd);
    }
}

/* 0x00450850 */
void __cdecl GAM_pc_fn_vReadExternalCtrlAlwaysMaxAndBanks(void)
{
    unsigned int uCmd;

    GAM_pc_fn_vReadExternalCtrlDwordInto(&uCmd);
    while (uCmd != 0xffff)
    {
        switch (uCmd)
        {
        case 0x5b:
            GAM_g_ulMaxNumberOfAlways = GAM_pc_fn_ulReadExternalCtrlDword();
            break;
        case 0x5c:
            GAM_pc_fn_vLoadExternalCtrlSoundBanks();
            break;
        default:
            break;
        }
        GAM_pc_fn_vReadExternalCtrlDwordInto(&uCmd);
    }
}

/* 0x004507b0 */
extern int __stdcall SND_fn_bStartLoadBanks(void);
extern int __stdcall SND_fn_bInitBank(int _iBankNum);
extern int __stdcall SND_fn_bEndLoadBanks(void);

void __cdecl GAM_pc_fn_vLoadExternalCtrlSoundBanks(void)
{
    unsigned int ulCmd;
    unsigned int ulCount;
    unsigned int ulBanks[500];
    unsigned int *p_ulBank;

    SND_fn_bStartLoadBanks();
    GAM_pc_fn_vReadExternalCtrlDwordInto(&ulCmd);
    while (ulCmd != 0xffff)
    {
        switch (ulCmd)
        {
        case 0x5d:
            ulCount = GAM_pc_fn_ulReadExternalCtrlDword();
            GAM_pc_fn_vReadExternalCtrlBytes(ulBanks, ulCount * 4);
            if ((int)ulCount > 0)
            {
                p_ulBank = ulBanks;
                do
                {
                    SND_fn_bInitBank((int)*p_ulBank);
                    p_ulBank = p_ulBank + 1;
                    ulCount = ulCount - 1;
                } while (ulCount != 0);
            }
            break;
        case 0x5e:
            ulCount = GAM_pc_fn_ulReadExternalCtrlDword();
            SND_fn_bInitBank((int)ulCount);
            break;
        }
        GAM_pc_fn_vReadExternalCtrlDwordInto(&ulCmd);
    }
    SND_fn_bEndLoadBanks();
}

extern void *__cdecl GLI_pc_fn_p_vAlloc(unsigned long ulSize);                         /* 0x0045DC60 */

/* 0x0042DEC0 */
void __cdecl IPT_pc_fn_vInitInputDevices(short sHistoricSize, HINSTANCE hInstance, HWND hWindow, long lParam)
{
    void *pBuffer;
    INO_tdstDevice *pDevice;
    struct INO_tdstDevice_ **pPadSlot;
    short swNbPads;
    int iPad;
    unsigned long ulBufferSize;

    IPT_g_hInputStructure.sEventHistoricSize = sHistoricSize;
    ulBufferSize = (unsigned long)((int)sHistoricSize * 6);
    pBuffer = GLI_pc_fn_p_vAlloc(ulBufferSize);
    IPT_g_hInputStructure.hEventHistoric = (IPT_tdstHistoricElement *)pBuffer;
    memset(pBuffer, 0, ulBufferSize);

    memset(IPT_g_hInputStructure.p_ucKeyboardCounter, 0, sizeof(IPT_g_hInputStructure.p_ucKeyboardCounter));
    memset(IPT_g_hInputStructure.p_ucPadAndJoyCounter, 0, sizeof(IPT_g_hInputStructure.p_ucPadAndJoyCounter));

    memset(IPT_g_hInputStructure.p_ucValideAndActiveDevice, 0, IPT_E_MouseDeviceEvent);
    memset(IPT_g_hInputStructure.a_ucMouseButtonsCounter, 0, sizeof(IPT_g_hInputStructure.a_ucMouseButtonsCounter));
    IPT_g_hInputStructure.ucOnePadActivate = 0;
    IPT_g_hInputStructure.p_ucValideAndActiveDevice[IPT_E_MouseDeviceEvent] = IPT_C_InvalideDevice;

    if ((short)INO_fn_wInit(hInstance, hWindow) != 0)
    {
        pDevice = INO_fn_hCreateDevice(INO_C_uwKeyboard, 2);
        if (pDevice != NULL)
        {
            IPT_g_hInputStructure.p_ucValideAndActiveDevice[IPT_E_KeyboardDeviceEvent] = IPT_C_ValideDevice;
            IPT_g_hInputStructure.a_hINODevice[IPT_E_KeyboardDeviceEvent] = pDevice;
            IPT_g_hInputStructure.ucKeyboardType = (unsigned char)(2 - (((INO_tdstKeyboardCaps *)pDevice->p_stCaps)->wIsAzerty != 0));
        }
        IPT_g_hInputStructure.ucOnePadActivate = IPT_C_ValideDevice;

        iPad = 0;
        swNbPads = INO_fn_wGetNbDevices(INO_C_uwJoystick);
        if (0 < swNbPads)
        {
            pPadSlot = &IPT_g_hInputStructure.a_hINODevice[IPT_E_Pad00DeviceEvent];
            do
            {
                pDevice = INO_fn_hCreateDevice(INO_C_uwJoystick, 2);
                if (pDevice != NULL)
                {
                    if ((pDevice->p_stState->ucStatus & INO_C_ucConnected) == 0)
                    {
                        INO_fn_wReleaseDevice(pDevice);
                    }
                    else
                    {
                        *pPadSlot = pDevice;
                        IPT_g_hInputStructure.p_ucValideAndActiveDevice[IPT_E_Pad00DeviceEvent + iPad] = IPT_C_ValideDevice;
                    }
                }
                iPad = iPad + 1;
                pPadSlot = pPadSlot + 1;
                swNbPads = INO_fn_wGetNbDevices(INO_C_uwJoystick);
            } while (iPad < (int)swNbPads);
        }
        IPT_pc_fn_vCreateMouseDevice(lParam);
    }
}

/* 0x004508a0 */
void __cdecl GAM_pc_fn_vProcessExternalCtrlCmds(short nCount)
{
    while (nCount != 0)
    {
        nCount = (short)(nCount - 1);
        switch (GAM_pc_fn_ulReadExternalCtrlCmd())
        {
        case 0x00: GAM_pc_fn_vReadExternalCtrlCommands(); break;
        case 0x1e: GAM_pc_fn_vReadExternalCtrlLevelNames(); break;
        case 0x20: GAM_pc_fn_vReadExternalCtrlRandomTable(); break;
        case 0x28: GAM_pc_fn_vReadExternalCtrlDirectories(); break;
        case 0x40: GAM_pc_fn_vReadExternalCtrlOpenDataFiles(); break;       /* 0x00450480 */
        case 0x46: GAM_pc_fn_vReadExternalCtrlProgressUi(); break;          /* 0x00450230 */
        case 0x5a: GAM_pc_fn_vReadExternalCtrlAlwaysMaxAndBanks(); break;   /* 0x00450850 */
        case 0x5c: GAM_pc_fn_vLoadExternalCtrlSoundBanks(); break;          /* 0x004507b0 */
        case 0x64: GAM_pc_fn_vReadExternalCtrlFileAndGldOptions(); break;   /* 0x004505e0 */
        case 0x6e: {

            /* 0x450913 */
            extern void *__cdecl GAM_pc_fn_hGetAppInstance(void);
            IPT_pc_fn_vInitInputDevices((short)GAM_pc_fn_ulReadExternalCtrlDword(),
                                        (HINSTANCE)GAM_pc_fn_hGetAppInstance(),
                                        (HWND)GAM_fn_hGetWindowHandle(), 0);
            break;
        }
        case 0x78: GAM_pc_fn_vReadExternalCtrlInputDevices(); break;        /* 0x004506b0 */
        default: return;
        }
    }
}

void FUN_00403690(void)
{
    /* 0x0045ec20 */
    extern void __cdecl FUN_0045ec20(void);

    unsigned char ucCond;

    if (g_ucGameModuleId == 0xff)
        g_ucGameModuleId = Erm_fn_ucInitErrMsg();

    FUN_00492860();
    FUN_00405a30();
    GAM_pc_fn_vLoadGliDll();

    GAM_g_stEngineStructure.eEngineMode = E_EM_ModeStartingProgram;

    GEO_xInitGEOError();
    IPT_fn_vFirstInitInput();
    RND_fn_vFirstInitRandom();
    FUN_0048d2a0();
    SAI_fn_vFirstInit();
    FUN_0045dbb0();
    FUN_004928d0();
    FUN_0045ec20();
    fn_vFirstInitAIMemory();
    fn_vFirstInitGameMemory();
    SND_PrintUsedStaticMemory();
    FUN_0044d410();

    ucCond = FUN_0044f4f0();
    if (ucCond == 1)
    {
        FUN_0044f870(1);
        /* 0x004b7468 */
        GAM_g_ul4b7468 = 1;
        GAM_pc_fn_vProcessExternalCtrlCmds(1);
    }
    else
    {
        FUN_0044f4d0(0);
    }

    FUN_00405810();
}

void fn_vInitEngineWhenInitApplication(void)
{

    if (GAM_pc_g_ucErmChannelIdA == 0xFF)
        GAM_pc_g_ucErmChannelIdA = Erm_fn_ucInitErrMsg();
    if (MMG_g_ucErmChannelId == 0xFF)
        MMG_g_ucErmChannelId = Erm_fn_ucInitErrMsg();

    /* 0x004433c0 */
    Mmg_fn_vFirstInitMmgModule(10);

    /* 0x0042daa0 */
    GMT_fn_vInitFor3DOS();

    if (GAM_pc_g_ucErmChannelIdB == 0xFF)
        GAM_pc_g_ucErmChannelIdB = Erm_fn_ucInitErrMsg();

    FUN_00403690();
}

/* 0x403583 */
void __cdecl FUN_004668e0(int iIgnored)
{
    (void)iIgnored;
    g_ucNewActionReturn = 0x20;
    g_ucUseDefaultActionReturn = 1;
}

#include "GAM/ObjType.h"

/* 0x00403604 */
void FUN_00407ad0(void)
{
    LST2_M_DynamicInitAnchor(&g_stObjectTypes.hFamilyType);
    LST2_M_DynamicInitAnchor(&g_stObjectTypes.hModelType);
    LST2_M_DynamicInitAnchor(&g_stObjectTypes.hPersonalType);
}

/* 0x0049F618 */
static const unsigned long GAM_g_aulMemoryCanary[6] = {1, 2, 3, 4, 5, 6};

void GAM_pc_fn_vCheckMemoryCanary(void)
{
    if (GAM_g_aulMemoryCanary[0] != 1 ||
        GAM_g_aulMemoryCanary[1] != 2 ||
        GAM_g_aulMemoryCanary[2] != 3 ||
        GAM_g_aulMemoryCanary[3] != 4 ||
        GAM_g_aulMemoryCanary[4] != 5 ||
        GAM_g_aulMemoryCanary[5] != 6)
    {
        ERM_fn_vClearLastError(0);
        ERM_fn_vUpdateLastError(0x30, g_ucGameModuleId, 0, 0x15,
                                -1, 0xff, 0xff, (char *)0);
    }
}

extern char         *__cdecl FUN_00492700(void);
extern char         *__cdecl FUN_00492770(void);
extern unsigned char __cdecl FIL_fn_bDestroyFileOrDirectory(char *_szFileName);
extern void         *__cdecl FIL_fn_hSearchFile(char *_szFileName, unsigned long ulAttrib);
extern unsigned char __cdecl FIL_fn_bIsFileExist(char *_szFileName);   /* 0x004929c0 */
struct FIL_tdstFileNameListElement_;
extern void          __cdecl FIL_fn_vDestroyOnElementInFileNameList(
                                 struct FIL_tdstFileNameListElement_ *hElem);  /* 0x00492930 */

/* 0x45fe70 */
static const char GAM_INIT_szGlobFmt[]      = "%s/*";
static const char GAM_INIT_szGlobFmtDrive[] = "%c\\\\%s/*";
static const char GAM_INIT_szPathFmt[]      = "%s/%s/%s.%s";
static const char GAM_INIT_szPathFmtDrive[] = "%c\\\\%s/%s/%s.%s";
static const char GAM_INIT_szDscExt[]       = "dsc";

void GAM_pc_fn_vPruneDscLessDirectories(void)
{
    char  szPath[260];
    int  *pList;
    int  *pCur;
    int  *pNext;
    char *szName;

    sprintf(szPath, GAM_INIT_szGlobFmt, FUN_00492770());
    pList = (int *)FIL_fn_hSearchFile(szPath, 0x10);
    GAM_g_stEngineStructure.hLevelNameList = pList;

    if (pList[2] != 0)
    {
        pCur  = (int *)pList[0];
        pNext = (pCur != (int *)0) ? (int *)pCur[0] : (int *)0;
        while (pCur != (int *)0)
        {
            szName = (char *)pCur[3];
            sprintf(szPath, GAM_INIT_szPathFmt,
                    FUN_00492770(), szName, szName, (char *)GAM_INIT_szDscExt);
            if (FIL_fn_bIsFileExist(szPath) == 0)
                FIL_fn_vDestroyOnElementInFileNameList(
                    (struct FIL_tdstFileNameListElement_ *)pCur);
            pCur  = pNext;
            pNext = (pCur != (int *)0) ? (int *)pCur[0] : (int *)0;
        }
    }
    else
    {

        sprintf(szPath, GAM_INIT_szGlobFmtDrive,
                g_cCDROM, FUN_00492770());
        pList = (int *)FIL_fn_hSearchFile(szPath, 0x10);
        GAM_g_stEngineStructure.hLevelNameList = pList;
        pCur  = (int *)pList[0];
        pNext = (pCur != (int *)0) ? (int *)pCur[0] : (int *)0;
        while (pCur != (int *)0)
        {
            szName = (char *)pCur[3];
            sprintf(szPath, GAM_INIT_szPathFmtDrive,
                    g_cCDROM, FUN_00492770(), szName, szName,
                    (char *)GAM_INIT_szDscExt);
            if (FIL_fn_bIsFileExist(szPath) == 0)
                FIL_fn_vDestroyOnElementInFileNameList(
                    (struct FIL_tdstFileNameListElement_ *)pCur);
            pCur  = pNext;
            pNext = (pCur != (int *)0) ? (int *)pCur[0] : (int *)0;
        }
    }

    GAM_g_stEngineStructure.lSubMapNumber = 0xff00ff;
}

extern unsigned char __cdecl GAM_pc_fn_bBuildSaveSlotPath(char *p_szOut, long lSlot);

/* 0x4928f0 */
unsigned char __cdecl GAM_pc_fn_bDeleteSaveSlot(unsigned int param_1)
{
    char szPath[260];
    if (param_1 == 0) {
        sprintf(szPath, GAM_INIT_szPathJoinFmt, FUN_00492700(),
                GAM_gs_pszHiddenSlot);
    } else if (param_1 < 1 || param_1 > GAM_INIT_ulSlotCount ||
               (char)GAM_pc_fn_bBuildSaveSlotPath(szPath, (long)param_1) == 0) {
        return 0;
    }
    return FIL_fn_bDestroyFileOrDirectory(szPath);
}

#include "GAM/a3x_mem.h"

void fn_v_UnclearPreviousLoadedAnims( void )
{
    unsigned short uwNumStack;

    for (uwNumStack = 0; uwNumStack < eStackNbMax; uwNumStack++)
        tdstStacks[uwNumStack].ulResetPos = tdstStacks[uwNumStack].ulStackPos;
}

/* 0x004067F0 */
void fn_v_ClearLoadedAnims(unsigned char ucClearAll)
{
    unsigned short uwNumStack;

    for (uwNumStack = 0; uwNumStack < eStackNbMax; uwNumStack++)
    {
        if (ucClearAll)
            tdstStacks[uwNumStack].ulResetPos = 0;
        M_v_ResetOneStack(uwNumStack);
    }
}
