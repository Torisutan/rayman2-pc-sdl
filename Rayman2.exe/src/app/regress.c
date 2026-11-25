/* Regression harness (fork-only; compiled with RAYMAN2_REGRESS, active when R2_REGRESS_OUT is set).
 *
 * Same input -> same engine state per frame, in any build. While a run is active:
 *   - time is virtual: every timer channel reads frame * frequency / 60, so each frame lasts
 *     exactly 1/60 s whatever the machine does (REGRESS_fn_bGetCounter);
 *   - the LCG pool and the random table use fixed seeds (REGRESS_C_ulLcgSeed / ulRandSeed);
 *   - input comes from a script (R2_REGRESS_INPUT) through the keyboard device's read method;
 *     mouse and joystick read nothing, so the devices plugged in do not matter. With
 *     R2_REGRESS_RECORD the real keyboard is read instead and written in the script format;
 *   - once the menu has played REGRESS_C_lSettleFrames frames, the level R2_REGRESS_LEVEL is
 *     requested the way GAM_fn_vAskToChangeLevel requests one; R2_REGRESS_FRAMES frames after
 *     it plays, the process exits.
 *
 * Output (R2_REGRESS_OUT): one line per frame, "frame hash field=value ...", the state the previous
 * frame left: engine structure fields (no pointers, handles or real-time counts), main actor
 * global matrix, random indices and table (not the MMG LCG: protection state only).
 * tools/regress.py compares two files.
 *
 * Input script: lines "FIRST LAST KEY[,KEY...]", frames counted from the level start (frame 0 =
 * first frame the level plays), KEY a DirectInput scan code (0x1E) or a name from s_a_stKeyName.
 * '#' starts a comment.
 */
#ifdef RAYMAN2_REGRESS

#include <windows.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "app/regress.h"
#include "GAM/GAM_types.h"
#include "HIE/HIE_types.h"
#include "POS/POS_types.h"
#include "MMG/MMG_globals.h"
#include "RND/RND_types.h"
#include "RND/RND_globals.h"
#include "TMR/TMR_globals.h"
#include "INO/INO.h"

extern INO_tdhDevice INO_pc_a_hDevices_storage[INO_C_wNbMaxDevice];   /* INO/INO.c */
extern unsigned char GAM_pc_g_ucMode6SaveFlag_storage;                /* GAM/GAM_engine.c */

#define REGRESS_C_ulFps            60
#define REGRESS_C_lSettleFrames    60
#define REGRESS_C_ulMaxScriptLines 512

typedef struct REGRESS_tdstScriptLine_
{
    long          lFirst;
    long          lLast;
    unsigned char a_ucKey[8];
    int           iNbKeys;
} REGRESS_tdstScriptLine;

static int            gs_iActive = -1;            /* -1 = environment not read yet */
static FILE          *gs_p_stOut;
static FILE          *gs_p_stRecord;
static char           gs_szLevel[GAM_C_MAX_NAME_LEVEL];
static long           gs_lFrames = 1800;
static unsigned long  gs_ulFrame;                 /* frames started so far (virtual clock) */
static long           gs_lPlayingFrames;          /* menu frames played before the request */
static int            gs_bRequested;
static long           gs_lLevelStart = -1;        /* gs_ulFrame of the level's first frame */
static REGRESS_tdstScriptLine gs_a_stScript[REGRESS_C_ulMaxScriptLines];
static int            gs_iNbScript;
static INO_tdfn_wMethod gs_a_pfnRealRead[INO_C_wNbMaxDevice];

static const struct { const char *szName; unsigned char ucCode; } s_a_stKeyName[] = {
    { "ESC", 0x01 }, { "ENTER", 0x1C }, { "SPACE", 0x39 }, { "A", 0x1E }, { "Q", 0x10 },
    { "LCTRL", 0x1D }, { "LSHIFT", 0x2A }, { "UP", 0xC8 }, { "DOWN", 0xD0 }, { "LEFT", 0xCB },
    { "RIGHT", 0xCD }, { "NUM0", 0x52 }, { "J", 0x24 }, { "F1", 0x3B }
};

/* ---- environment and files ---------------------------------------------------------------- */

static int REGRESS_fn_bEnv(const char *szName, char *szOut, DWORD ulSize)
{
    DWORD ulLen = GetEnvironmentVariableA(szName, szOut, ulSize);
    return ulLen != 0 && ulLen < ulSize;
}

static int REGRESS_fn_iParseKey(const char *szTok, unsigned char *p_ucOut)
{
    int i;
    if (szTok[0] == '0' && (szTok[1] == 'x' || szTok[1] == 'X')) {
        *p_ucOut = (unsigned char)strtoul(szTok, NULL, 16);
        return 1;
    }
    for (i = 0; i < (int)(sizeof(s_a_stKeyName) / sizeof(s_a_stKeyName[0])); i++) {
        if (_stricmp(szTok, s_a_stKeyName[i].szName) == 0) {
            *p_ucOut = s_a_stKeyName[i].ucCode;
            return 1;
        }
    }
    return 0;
}

static void REGRESS_fn_vLoadScript(const char *szPath)
{
    FILE *p_stFile = fopen(szPath, "rt");
    char  szLine[256];
    if (p_stFile == NULL) {
        return;
    }
    while (fgets(szLine, sizeof(szLine), p_stFile) != NULL && gs_iNbScript < REGRESS_C_ulMaxScriptLines) {
        REGRESS_tdstScriptLine *p_stLine = &gs_a_stScript[gs_iNbScript];
        char *p_cHash = strchr(szLine, '#');
        char  szKeys[200];
        char *szTok;
        if (p_cHash != NULL) {
            *p_cHash = 0;
        }
        if (sscanf(szLine, "%ld %ld %199s", &p_stLine->lFirst, &p_stLine->lLast, szKeys) != 3) {
            continue;
        }
        p_stLine->iNbKeys = 0;
        for (szTok = strtok(szKeys, ","); szTok != NULL && p_stLine->iNbKeys < 8; szTok = strtok(NULL, ",")) {
            if (REGRESS_fn_iParseKey(szTok, &p_stLine->a_ucKey[p_stLine->iNbKeys])) {
                p_stLine->iNbKeys++;
            }
        }
        gs_iNbScript++;
    }
    fclose(p_stFile);
}

int __cdecl REGRESS_fn_bActive(void)
{
    char szBuf[MAX_PATH];
    if (gs_iActive >= 0) {
        return gs_iActive;
    }
    gs_iActive = 0;
    if (!REGRESS_fn_bEnv("R2_REGRESS_OUT", szBuf, sizeof(szBuf))) {
        return 0;
    }
    gs_p_stOut = fopen(szBuf, "wt");
    if (gs_p_stOut == NULL) {
        return 0;
    }
    if (REGRESS_fn_bEnv("R2_REGRESS_LEVEL", szBuf, sizeof(szBuf))) {
        strncpy(gs_szLevel, szBuf, sizeof(gs_szLevel) - 1);
    }
    if (REGRESS_fn_bEnv("R2_REGRESS_FRAMES", szBuf, sizeof(szBuf))) {
        gs_lFrames = atol(szBuf);
    }
    if (REGRESS_fn_bEnv("R2_REGRESS_RECORD", szBuf, sizeof(szBuf))) {
        gs_p_stRecord = fopen(szBuf, "wt");
    } else if (REGRESS_fn_bEnv("R2_REGRESS_INPUT", szBuf, sizeof(szBuf))) {
        REGRESS_fn_vLoadScript(szBuf);
    }
    gs_iActive = 1;
    return 1;
}

/* ---- virtual clock ------------------------------------------------------------------------ */

int __cdecl REGRESS_fn_bGetCounter(short channel, LARGE_INTEGER *p_stOut)
{
    unsigned __int64 ullTicks;
    if (!REGRESS_fn_bActive() || channel < 0 || channel > 2) {
        return 0;
    }
    ullTicks = (unsigned __int64)gs_ulFrame * TMR_a3_ulTimerFrequency[channel] / REGRESS_C_ulFps;
    p_stOut->QuadPart = (__int64)ullTicks;
    return 1;
}

/* ---- input -------------------------------------------------------------------------------- */

static short REGRESS_fn_wReadNothing(INO_tdstDevice *hDevice)
{
    (void)hDevice;
    return 0;
}

static int REGRESS_fn_iDeviceIndex(INO_tdstDevice *hDevice)
{
    int i;
    for (i = 0; i < INO_C_wNbMaxDevice; i++) {
        if (INO_pc_a_hDevices_storage[i] == hDevice) {
            return i;
        }
    }
    return -1;
}

static long REGRESS_fn_lScriptFrame(void)
{
    return gs_lLevelStart < 0 ? -1 : (long)(gs_ulFrame - 1) - gs_lLevelStart;
}

/* What a successful INO_fn_wReadKeyboard does, with the key buffer from the script. */
static short REGRESS_fn_wReadKeyboardScript(INO_tdstDevice *hDevice)
{
    INO_tdstKeyboardState *p_stState = (INO_tdstKeyboardState *)hDevice->p_stState;
    unsigned char *p_ucBuf;
    long lFrame = REGRESS_fn_lScriptFrame();
    int  i, k;

    if ((p_stState->ucStatus & INO_C_ucConnected) == 0) {
        return 0;
    }
    hDevice->ulLastTimeCount += 1;
    p_stState->wCurrentTab = (unsigned short)(1 - (short)p_stState->wCurrentTab);
    p_stState->ucStatus |= INO_C_ucAcquired;
    p_stState->ucStatus &= (unsigned char)~INO_C_ucAccessError;
    p_ucBuf = p_stState->a_ucTabKey[p_stState->wCurrentTab];
    memset(p_ucBuf, 0, 256);
    for (i = 0; i < gs_iNbScript; i++) {
        if (lFrame >= gs_a_stScript[i].lFirst && lFrame <= gs_a_stScript[i].lLast) {
            for (k = 0; k < gs_a_stScript[i].iNbKeys; k++) {
                p_ucBuf[gs_a_stScript[i].a_ucKey[k]] = INO_C_ucKeyDown;
            }
        }
    }
    return 1;
}

/* The real read, its key buffer written as script lines. */
static short REGRESS_fn_wReadKeyboardRecord(INO_tdstDevice *hDevice)
{
    INO_tdstKeyboardState *p_stState = (INO_tdstKeyboardState *)hDevice->p_stState;
    int   iIndex = REGRESS_fn_iDeviceIndex(hDevice);
    short wRet = (iIndex >= 0 && gs_a_pfnRealRead[iIndex] != NULL) ? gs_a_pfnRealRead[iIndex](hDevice) : 0;
    long  lFrame = REGRESS_fn_lScriptFrame();
    unsigned char *p_ucBuf = p_stState->a_ucTabKey[p_stState->wCurrentTab];
    int   i, bFirst = 1;

    if (lFrame < 0 || gs_p_stRecord == NULL) {
        return wRet;
    }
    for (i = 0; i < 256; i++) {
        if (p_ucBuf[i] & INO_C_ucKeyDown) {
            fprintf(gs_p_stRecord, bFirst ? "%ld %ld 0x%02X" : ",0x%02X", lFrame, lFrame, i);
            if (bFirst) {
                bFirst = 0;
            }
        }
    }
    if (!bFirst) {
        fputc('\n', gs_p_stRecord);
    }
    return wRet;
}

static void REGRESS_fn_vHookDevices(void)
{
    int i;
    for (i = 0; i < INO_C_wNbMaxDevice; i++) {
        INO_tdstDevice *hDevice = INO_pc_a_hDevices_storage[i];
        INO_tdfn_wMethod pfnOurs;
        if (hDevice == NULL) {
            continue;
        }
        if (INO_M_uwType(hDevice) == INO_C_uwKeyboard) {
            pfnOurs = gs_p_stRecord ? REGRESS_fn_wReadKeyboardRecord : REGRESS_fn_wReadKeyboardScript;
        } else {
            pfnOurs = REGRESS_fn_wReadNothing;
        }
        if (hDevice->pfnRead != pfnOurs) {
            gs_a_pfnRealRead[i] = hDevice->pfnRead;
            hDevice->pfnRead = pfnOurs;
        }
    }
}

/* ---- state hash --------------------------------------------------------------------------- */

static unsigned __int64 gs_ullHash;
static char             gs_szLine[2048];
static int              gs_iLineLen;

static unsigned long REGRESS_fn_ulFnv32(const void *p_vData, unsigned long ulSize)
{
    const unsigned char *p_ucData = (const unsigned char *)p_vData;
    unsigned long ulHash = 0x811C9DC5u;
    while (ulSize--) {
        ulHash = (ulHash ^ *p_ucData++) * 0x01000193u;
    }
    return ulHash;
}

static void REGRESS_fn_vAddText(const char *szName, const char *szValue)
{
    gs_iLineLen += _snprintf(gs_szLine + gs_iLineLen, sizeof(gs_szLine) - gs_iLineLen, " %s=%s", szName, szValue);
}

/* A field: into the frame hash, and onto the line as its own 32-bit digest. */
static void REGRESS_fn_vAddField(const char *szName, const void *p_vData, unsigned long ulSize)
{
    const unsigned char *p_ucData = (const unsigned char *)p_vData;
    unsigned long ulDigest = REGRESS_fn_ulFnv32(p_vData, ulSize);
    char szValue[16];
    unsigned long i;
    for (i = 0; i < ulSize; i++) {
        gs_ullHash = (gs_ullHash ^ p_ucData[i]) * 0x00000100000001B3ui64;
    }
    sprintf(szValue, "%08lx", ulDigest);
    REGRESS_fn_vAddText(szName, szValue);
}

#define REGRESS_M_Field(name, lvalue) REGRESS_fn_vAddField(name, &(lvalue), sizeof(lvalue))

static void REGRESS_fn_vHashFrame(void)
{
    GAM_tdstEngineStructure *p_stEng = &GAM_g_stEngineStructure;
    GAM_tdstEngineTimerStructure *p_stTimer = &p_stEng->stEngineTimer;
    char szValue[64];

    gs_ullHash = 0xCBF29CE484222325ui64;
    gs_iLineLen = 0;

    sprintf(szValue, "%u", (unsigned)p_stEng->eEngineMode);
    REGRESS_fn_vAddText("mode", szValue);
    REGRESS_fn_vAddText("lvl", p_stEng->szLevelName[0] ? p_stEng->szLevelName : "-");
    REGRESS_M_Field("eng.mode", p_stEng->eEngineMode);
    REGRESS_M_Field("eng.level", p_stEng->szLevelName);
    REGRESS_M_Field("eng.next", p_stEng->szNextLevelName);
    REGRESS_M_Field("eng.input", p_stEng->eInputMode);
    REGRESS_M_Field("eng.fixmode", p_stEng->eDisplayFixMode);
    REGRESS_M_Field("eng.dispmode", p_stEng->ulDisplayMode);
    REGRESS_M_Field("tmr.trame", p_stTimer->ulTrameNumber);
    REGRESS_M_Field("tmr.delta", p_stTimer->ulDeltaTimerCount);
    REGRESS_M_Field("tmr.counter", p_stTimer->a_ulCounter);
    REGRESS_M_Field("tmr.useful", p_stTimer->ulUsefulDeltaTime);
    REGRESS_M_Field("tmr.framelen", p_stTimer->xFrameLength);
    REGRESS_M_Field("eng.mainpos", p_stEng->stMainCharacterPosition);
    REGRESS_M_Field("eng.campos", p_stEng->stMainCameraPosition);
    REGRESS_M_Field("eng.submap", p_stEng->lSubMapNumber);
    REGRESS_M_Field("eng.paused", p_stEng->bEngineIsInPaused);
    REGRESS_M_Field("eng.haspaused", p_stEng->bEngineHasInPaused);
    REGRESS_M_Field("eng.curlevel", p_stEng->ucCurrentLevel);
    REGRESS_M_Field("eng.prevlevel", p_stEng->ucPreviousLevel);
    REGRESS_M_Field("eng.exitid", p_stEng->ucExitIdToQuitPrevLevel);
    REGRESS_M_Field("eng.lvlcounter", p_stEng->ucLevelGlobalCounter);
    REGRESS_M_Field("eng.demo", p_stEng->xDemoMode);
    REGRESS_M_Field("eng.frozen", p_stEng->bEngineFrozen);
    REGRESS_M_Field("eng.resurect", p_stEng->bResurection);
    REGRESS_M_Field("eng.cammode", p_stEng->cCameraMode);
    if (p_stEng->g_hMainActor != NULL && p_stEng->g_hMainActor->hGlobalMatrix != NULL) {
        REGRESS_fn_vAddField("actor.matrix", p_stEng->g_hMainActor->hGlobalMatrix, sizeof(POS_tdstCompletePosition));
    } else {
        REGRESS_fn_vAddText("actor.matrix", "-");
    }
    REGRESS_M_Field("rnd.index", RND_g_stRandomStructure.a_ulIndex);
    REGRESS_M_Field("rnd.max", RND_g_stRandomStructure.ulMaxValueInTable);
    if (RND_g_stRandomStructure.p_ulTable != NULL) {
        REGRESS_fn_vAddField("rnd.table", RND_g_stRandomStructure.p_ulTable,
                             RND_g_stRandomStructure.ulSizeOfTable * sizeof(unsigned long));
    }
}

/* ---- per frame ---------------------------------------------------------------------------- */

static void REGRESS_fn_vScenario(void)
{
    GAM_tdstEngineStructure *p_stEng = &GAM_g_stEngineStructure;
    if (gs_szLevel[0] == 0) {
        if (gs_lLevelStart < 0 && p_stEng->eEngineMode == E_EM_ModePlaying) {
            gs_lLevelStart = (long)gs_ulFrame;           /* no level asked: script from the menu */
        }
        return;
    }
    if (!gs_bRequested) {
        if (p_stEng->eEngineMode != E_EM_ModePlaying) {
            gs_lPlayingFrames = 0;
        } else if (++gs_lPlayingFrames >= REGRESS_C_lSettleFrames) {
            strcpy(p_stEng->szNextLevelName, gs_szLevel);
            GAM_pc_g_ucMode6SaveFlag_storage = 0;
            p_stEng->eEngineMode = E_EM_ModeChangeLevel;
            gs_bRequested = 1;
        }
    } else if (gs_lLevelStart < 0 && p_stEng->eEngineMode == E_EM_ModePlaying
               && _stricmp(p_stEng->szLevelName, gs_szLevel) == 0) {
        gs_lLevelStart = (long)gs_ulFrame;
    }
}

void __cdecl REGRESS_fn_vBeforeFrame(void)
{
    if (!REGRESS_fn_bActive()) {
        return;
    }
    if (gs_ulFrame > 0) {
        REGRESS_fn_vHashFrame();
        fprintf(gs_p_stOut, "%lu %016I64x%s\n", gs_ulFrame - 1, gs_ullHash, gs_szLine);
    }
    if (gs_lLevelStart >= 0 && (long)gs_ulFrame - gs_lLevelStart >= gs_lFrames) {
        fclose(gs_p_stOut);
        if (gs_p_stRecord != NULL) {
            fclose(gs_p_stRecord);
        }
        ExitProcess(0);
    }
    REGRESS_fn_vScenario();
    REGRESS_fn_vHookDevices();
    gs_ulFrame++;
}

#endif /* RAYMAN2_REGRESS */
