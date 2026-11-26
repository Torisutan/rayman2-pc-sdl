#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <stdio.h>
#include <string.h>
#include <io.h>
#include <direct.h>
#include <time.h>
#include <sys/stat.h>
#include <stdint.h>

#include "GAM/GAM_pc.h"
#include "GAM/GAM_globals.h"
#include "SAI/SAI_Codec.h"
#include "GLI/Memory.h"
#include "GLI/Brightness.h"
#include "IPT/IPT_Save.h"
#include "FIL/File.h"

typedef struct GAM_tdstSaveSlot_ {
    long                       lSlotId;
    struct GAM_tdstSaveSlot_  *pNext;
    char                       szNameScore[0xB];
    char                       szScoreTail[0xD];
} GAM_tdstSaveSlot;

long  GAM_gs_lNbSaveSlots;        /* 0x4A7064 */
void *GAM_gs_pSaveSlotHead;      /* 0x4A706C */
#define GAM_g_p_stSaveSlotHead  (*(GAM_tdstSaveSlot **)&GAM_gs_pSaveSlotHead)
#define GAM_g_lNbSaveSlots      GAM_gs_lNbSaveSlots

unsigned long GAM_gs_ulSaveSlotCount2;   /* 0x4A7068 */
#define GAM_g_ulSaveSlotCount2  GAM_gs_ulSaveSlotCount2
/* 0x49F77C */
static const char GAM_g_szDefaultSlotName_storage[] = "007";
char              GAM_g_szSlotNameWork_storage[4] = "\\";
#define GAM_g_szDefaultSlotName  (GAM_g_szDefaultSlotName_storage)
#define GAM_g_szSlotNameWork     (GAM_g_szSlotNameWork_storage)

extern char *__cdecl FUN_00492700(void);

extern unsigned char __cdecl GAM_pc_fn_ucSortSaveSlotsByDate(void);   /* 0x410A80 */

#define GAM_RT_pvAllocZeroed(n) GLI_pc_fn_p_vAlloc(n)

/* 0x004081D0 */
long __cdecl GAM_pc_fn_lGetSaveSlotId(long lSlotId)
{
    GAM_tdstSaveSlot *p_stSlot = GAM_g_p_stSaveSlotHead;
    GAM_tdstSaveSlot *p_stNext;
    long lIndex;

    for (lIndex = 1; lIndex != lSlotId; lIndex++) {
        p_stNext = p_stSlot->pNext;
        if (p_stNext == NULL)
            break;
        p_stSlot = p_stNext;
    }
    return p_stSlot->lSlotId;
}

/* 0x004083B0 */
char *__cdecl GAM_pc_fn_p_szGetSaveSlotNameScore(long lSlotId)
{
    GAM_tdstSaveSlot *p_stSlot = GAM_g_p_stSaveSlotHead;

    if (lSlotId != 1) {
        long lCount = lSlotId - 1;
        do {
            p_stSlot = p_stSlot->pNext;
            lCount--;
        } while (lCount != 0);
    }
    return p_stSlot->szNameScore;
}

/* 0x00410980 */
unsigned char __cdecl GAM_pc_fn_bBuildSaveSlotPath(char *pszOut, long lSlotId)
{
    char szPath[128];

    sprintf(szPath, "%s\\%s%i", FUN_00492700(), "Slot", (int)lSlotId);
    if (strlen(szPath) > 0x80)
        return 0;
    strcpy(pszOut, szPath);
    return 1;
}

static unsigned long GAM_g_ulAllFilesSuffix = 0x2A2E2A5CUL; /* 0x0049F928 */
static char GAM_g_cAllFilesSuffixNul = 0; /* 0x0049F92C */

/* 0x004106A0 */
int __cdecl GAM_pc_fn_bDeleteDirectoryTree(char *pszDir)
{
    char             szPattern[256];
    WIN32_FIND_DATAA stFind;
    HANDLE           hFind;
    size_t           ulLen;
    char            *pSuffix;

    strcpy(szPattern, pszDir);
    ulLen = strlen(szPattern);
    pSuffix = &szPattern[ulLen];

    if (pSuffix[-1] == '\\')
        pSuffix[-1] = '\0';
    *(unsigned long *)pSuffix = GAM_g_ulAllFilesSuffix;
    pSuffix[4] = GAM_g_cAllFilesSuffixNul;

    hFind = FindFirstFileA(szPattern, &stFind);
    if (hFind != INVALID_HANDLE_VALUE) {
        do {
            if ((stFind.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) == 0) {

                strcpy(pSuffix + 1, stFind.cFileName);
                if (!DeleteFileA(szPattern))
                    return 0;
            } else if (strcmp(stFind.cFileName, ".") != 0 &&
                       strcmp(stFind.cFileName, "..") != 0) {

                strcpy(pSuffix + 1, stFind.cFileName);
                if (!GAM_pc_fn_bDeleteDirectoryTree(szPattern))
                    return 0;
                RemoveDirectoryA(szPattern);
            }
        } while (FindNextFileA(hFind, &stFind));
        FindClose(hFind);
    }
    return 1;
}

/* 0x410510 */
int __cdecl GAM_pc_fn_bCopyDirectoryTree(char *pszSrc, char *pszDst)
{
    char             szSrc[256];
    char             szDst[256];
    WIN32_FIND_DATAA stFind;
    HANDLE           hFind;
    size_t           ulSrcLen;
    size_t           ulDstLen;
    unsigned long    ulSuffix;

    strcpy(szSrc, pszSrc);
    ulSuffix = GAM_g_ulAllFilesSuffix;
    strcpy(szDst, pszDst);
    ulSrcLen = strlen(szSrc);
    ulDstLen = strlen(szDst);
    *(unsigned long *)&szSrc[ulSrcLen] = ulSuffix;
    szSrc[ulSrcLen + 4] = GAM_g_cAllFilesSuffixNul;

    hFind = FindFirstFileA(szSrc, &stFind);
    if (hFind == INVALID_HANDLE_VALUE)
        return 0;
    do {
        szSrc[ulSrcLen] = '\\';
        szDst[ulDstLen] = '\\';
        if ((stFind.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) == 0) {
            strcpy(&szSrc[ulSrcLen + 1], stFind.cFileName);
            strcpy(&szDst[ulDstLen + 1], stFind.cFileName);
            if (!CopyFileA(szSrc, szDst, FALSE))
                return 0;
        }
        szDst[ulDstLen] = '\0';
        szSrc[ulSrcLen] = '\0';
    } while (FindNextFileA(hFind, &stFind));
    FindClose(hFind);
    return 1;
}

/* 0x004082E0 */
int __cdecl GAM_pc_fn_lEnsureDefaultSaveSlot(void)
{
    GAM_tdstSaveSlot *p_stOldHead;
    GAM_tdstSaveSlot *p_stHead;
    char              szPath[260];
    int               bResult = 0;

    *(unsigned long *)GAM_g_szSlotNameWork = *(unsigned long *)GAM_g_szDefaultSlotName;

    p_stOldHead = GAM_g_p_stSaveSlotHead;
    if (GAM_g_lNbSaveSlots < 1) {
        GAM_g_p_stSaveSlotHead = (GAM_tdstSaveSlot *)GAM_RT_pvAllocZeroed(0x20);
        GAM_g_p_stSaveSlotHead->pNext   = p_stOldHead;
        GAM_g_lNbSaveSlots              = GAM_g_lNbSaveSlots + 1;
        GAM_g_p_stSaveSlotHead->lSlotId = GAM_g_lNbSaveSlots;
    }
    p_stHead = GAM_g_p_stSaveSlotHead;

    GAM_g_lSaveSlotDirty = 1;

    strcpy(p_stHead->szScoreTail, p_stHead->szNameScore);

    sprintf(p_stHead->szNameScore, "%3s 0%2.1f", GAM_g_szSlotNameWork, 0.0);

    if (GAM_pc_fn_bBuildSaveSlotPath(szPath, p_stHead->lSlotId))
        bResult = GAM_pc_fn_bDeleteDirectoryTree(szPath);

    return bResult;
}

/* 0x0049F780 */
static const char GAM_g_szSlotScoreTail_storage[] = "\\";

long __cdecl GAM_pc_fn_lCountSaveSlotDirs(char *pszPath)
{
    struct _finddata_t stFind;
    long               lCount = 0;
    intptr_t           hFind;

    if (_chdir(pszPath) != 0)
        return 0;

    hFind = _findfirst("*.*", &stFind);
    if (hFind != -1) {
        do {
            if (stFind.attrib & _A_SUBDIR) {
                if (stFind.name[0] != '.' && strncmp(stFind.name, "Slot", 4) == 0)
                    lCount++;
            }
        } while (_findnext(hFind, &stFind) == 0);
        _findclose(hFind);
    }
    return lCount;
}

/* 0x00408290 */
long __cdecl fn_lGetNbAvailableSlots(void)
{
    char szCwd[0x100];
    long lNbSlots;

    GetCurrentDirectoryA(0x100, szCwd);
    lNbSlots = GAM_pc_fn_lCountSaveSlotDirs(FUN_00492700());
    SetCurrentDirectoryA(szCwd);
    GAM_pc_fn_ucGrowSaveSlotList(lNbSlots);
    return lNbSlots;
}

/* 0x408460 */
unsigned char __cdecl GAM_pc_fn_ucGrowSaveSlotList(long lCount)
{
    long lNbSlots = GAM_g_lNbSaveSlots;
    unsigned char ucRet = 0;

    if (lCount != lNbSlots) {
    if (lCount > lNbSlots) {
        do {
            GAM_tdstSaveSlot *p_stOldHead = GAM_g_p_stSaveSlotHead;

            GAM_g_p_stSaveSlotHead = (GAM_tdstSaveSlot *)GAM_RT_pvAllocZeroed(0x20);
            GAM_g_p_stSaveSlotHead->pNext = p_stOldHead;
            GAM_g_lNbSaveSlots = GAM_g_lNbSaveSlots + 1;
            GAM_g_p_stSaveSlotHead->lSlotId = GAM_g_lNbSaveSlots;
            strncpy(GAM_g_p_stSaveSlotHead->szNameScore, GAM_g_szSlotNameWork, 3);
            *(unsigned short *)GAM_g_p_stSaveSlotHead->szScoreTail =
                *(unsigned short *)GAM_g_szSlotScoreTail_storage;
        } while (lCount > GAM_g_lNbSaveSlots);
    }

    ucRet = GAM_pc_fn_ucSortSaveSlotsByDate();
    }

    return ucRet;
}

/* 0x410C50 */
long __cdecl GAM_pc_fn_lCompareSaveSlotDates(char *pszDirA, char *pszDirB)
{
    struct _finddata_t stFindA;
    struct _finddata_t stFindB;
    char               szFileA[260];
    char               szFileB[260];
    intptr_t           hFindA;
    intptr_t           hFindB;
    long               lResult = 1;

    sprintf(szFileA, "%s\\General.sav", pszDirA);
    sprintf(szFileB, "%s\\General.sav", pszDirB);

    if (_access(szFileA, 0) == 0) {
        hFindA = _findfirst(szFileA, &stFindA);
    } else {
        hFindA = _findfirst(pszDirA, &stFindA);
        if (hFindA == -1)
            return -1;
        if ((stFindA.attrib & _A_RDONLY) == _A_RDONLY)
            _chmod(pszDirA, _S_IREAD | _S_IWRITE);
    }

    if (_access(szFileB, 0) == 0) {
        hFindB = _findfirst(szFileB, &stFindB);
    } else {
        hFindB = _findfirst(pszDirB, &stFindB);
        if (hFindB == -1) {
            _findclose(hFindA);
            return hFindB;
        }
    }

    if (!(difftime(stFindA.time_write, stFindB.time_write) > 0.0))
        lResult = 0;
    _findclose(hFindA);
    _findclose(hFindB);
    return lResult;
}

/* 0x4A7064 */
unsigned char __cdecl GAM_pc_fn_ucSortSaveSlotsByDate(void)
{
    char              szPathA[128];
    char              szPathB[128];
    char              szTmpName[12];
    char              szTmpScore[12];
    long              lPass = 0;
    long              lIdx;
    long              lSlotId;
    GAM_tdstSaveSlot *p_stNodeB = NULL;
    GAM_tdstSaveSlot *p_stNodeA;
    int               bSwapped = 1;

    if (GAM_g_lNbSaveSlots > 0) {
        do {
            if (!bSwapped)
                break;
            bSwapped = 0;
            p_stNodeA = p_stNodeB;
            lIdx = GAM_g_lNbSaveSlots;
            while (p_stNodeB = p_stNodeA, --lIdx, lPass < lIdx) {
                if (lIdx == GAM_g_lNbSaveSlots - 1) {
                    p_stNodeA = p_stNodeB = GAM_g_p_stSaveSlotHead;
                    p_stNodeA = p_stNodeA->pNext;
                } else {
                    p_stNodeA = p_stNodeB->pNext;
                }
                sprintf(szPathA, "%s\\%s%i", FUN_00492700(), "Slot", (int)p_stNodeA->lSlotId);
                sprintf(szPathB, "%s\\%s%i", FUN_00492700(), "Slot", (int)p_stNodeB->lSlotId);
                if (GAM_pc_fn_lCompareSaveSlotDates(szPathA, szPathB) != 0) {
                    lSlotId = p_stNodeA->lSlotId;
                    strcpy(szTmpName, p_stNodeA->szNameScore);
                    strcpy(szTmpScore, p_stNodeA->szScoreTail);
                    p_stNodeA->lSlotId = p_stNodeB->lSlotId;
                    strcpy(p_stNodeA->szNameScore, p_stNodeB->szNameScore);
                    p_stNodeB->lSlotId = lSlotId;
                    strcpy(p_stNodeB->szNameScore, szTmpName);
                    strcpy(p_stNodeB->szScoreTail, szTmpScore);
                    bSwapped = 1;
                }
            }
            lPass++;
        } while (lPass < GAM_g_lNbSaveSlots);
    }
    return 1;
}

/* 0x00410DB0 */
char *__cdecl GAM_pc_fn_szFormatSaveSlotDate(long lSlotId)
{

    static char        szResult[128];
    char               szDir[128];
    char               szFile[128];
    struct _finddata_t stFind;
    intptr_t           hFind;
    struct tm         *p_stTime;

    if (!(unsigned char)GAM_pc_fn_bBuildSaveSlotPath(szDir, lSlotId))
        return NULL;

    sprintf(szFile, "%s\\%s", szDir, "general.sav");
    hFind = _findfirst(szFile, &stFind);
    if (hFind == -1) {
        hFind = _findfirst(szDir, &stFind);
        if (hFind == -1)
            return NULL;
    }
    if ((stFind.attrib & _A_RDONLY) == _A_RDONLY)
        _chmod(szDir, _S_IREAD | _S_IWRITE);

    p_stTime = localtime(&stFind.time_write);
    sprintf(szResult, "%02i;%02i %i=%02i",
            p_stTime->tm_mday, p_stTime->tm_mon + 1,
            p_stTime->tm_hour, p_stTime->tm_min);
    _findclose(hFind);
    return szResult;
}

/* 0x00410A00 */
unsigned char __cdecl GAM_pc_fn_ucUpdateSaveSlotList(void *unused, unsigned long lSlotId)
{
    GAM_tdstSaveSlot *p_stOldHead;

    if (lSlotId >= 1 && lSlotId <= (unsigned long)GAM_g_lNbSaveSlots)
    {
        GAM_pc_fn_ucSortSaveSlotsByDate();
    }
    else
    {
        p_stOldHead = GAM_g_p_stSaveSlotHead;
        if (p_stOldHead != NULL)
        {
            GAM_g_p_stSaveSlotHead          = (GAM_tdstSaveSlot *)GLI_pc_fn_p_vAlloc(0x20);
            GAM_g_p_stSaveSlotHead->pNext   = p_stOldHead;
            GAM_g_lNbSaveSlots              = GAM_g_lNbSaveSlots + 1;
            GAM_g_p_stSaveSlotHead->lSlotId = GAM_g_lNbSaveSlots;
        }
    }

    strncpy(GAM_g_p_stSaveSlotHead->szNameScore, GAM_g_szSlotNameWork, 3);
    *(short *)GAM_g_p_stSaveSlotHead->szScoreTail = *(short *)"\\";
    return 1;
}

extern char *GAM_gs_pszHiddenSlot;                  /* 0x49F8C0 */
#define GAM_SS_pszHiddenSlot   GAM_gs_pszHiddenSlot

#define GAM_SS_pszEmptyPrefix  ("")

extern unsigned int GAM_g_ul500248_storage;   /* 0x500248 */
extern unsigned int GAM_g_ul500250_storage;   /* 0x500250 */
extern unsigned int GAM_g_ul500254_storage;   /* 0x500254 */
unsigned int GAM_g_ul50024c;                  /* 0x50024C */
unsigned int GAM_g_ul500258;                  /* 0x500258 */
char         GAM_g_szCurrentGameOptionsFileName[0x104];   /* 0x500124 */
extern char  AI_PAD_cAnalogMinX_storage;      /* 0x4A40A4 */
extern char  AI_PAD_cAnalogMaxX_storage;      /* 0x4A40A8 */
extern char  AI_PAD_cAnalogMinY_storage;      /* 0x4A40AC */
extern char  AI_PAD_cAnalogMaxY_storage;      /* 0x4A40B0 */
extern char  AI_PAD_cAnalogValueX_storage;    /* 0x4B9B5C */
extern char  AI_PAD_cAnalogValueY_storage;    /* 0x4B9B60 */
extern char *__cdecl FUN_00492850(void);
extern char *__cdecl FUN_004928c0(void);

/* 0x49F734 */
static char GAM_SS_szOptionsVersion[] = "V1";

/* 0x500020 */
static char GAM_pc_szLoadFileName[0x104];

void __cdecl GAM_pc_fn_vSetLoadFileName(char *pszSrc)
{
    strcpy(GAM_pc_szLoadFileName, pszSrc);
}

void __cdecl fn_vSetCurrentGameOptionsFileName(char *pszSrc)
{
    strcpy(GAM_g_szCurrentGameOptionsFileName, pszSrc);
}

unsigned char __cdecl fn_bSaveCurrentGameOptions(void)
{
    unsigned char     bResult = 1;
    char              szTmpPath[0x104];
    char              szOptPath[0x104];
    FILE             *p_stFile;
    long              lTemp;
    long              lNb;
    GAM_tdstSaveSlot *p_stSlot;

    sprintf(szTmpPath, "%s/TT.tmp", FUN_004928c0());
    sprintf(szOptPath, "%s/%s", FUN_00492850(), GAM_g_szCurrentGameOptionsFileName);
    FIL_fn_bValidatePath(".", szOptPath);
    remove(szTmpPath);
    p_stFile = fopen(szTmpPath, "wb");
    if (p_stFile != NULL)
    {
        lTemp = strlen(GAM_SS_szOptionsVersion);
        fwrite(&lTemp, 1, 4, p_stFile);
        fwrite(GAM_SS_szOptionsVersion, 1, lTemp + 1, p_stFile);
        lNb = GAM_g_lNbSaveSlots;
        fwrite(&lNb, 1, 4, p_stFile);
        p_stSlot = GAM_g_p_stSaveSlotHead;
        lTemp = 0;
        if (0 < lNb)
        {
            do {
                fwrite(&p_stSlot->szNameScore[0], 1, 0xb, p_stFile);
                fwrite(p_stSlot, 1, 4, p_stFile);
                p_stSlot = p_stSlot->pNext;
                lTemp++;
            } while (lTemp < lNb);
        }
        fwrite(&GAM_g_ul500248_storage, 1, 4, p_stFile);
        fwrite(&GAM_g_ul50024c, 1, 4, p_stFile);
        lTemp = GLI_fn_lGetBrightness();
        fwrite(&lTemp, 1, 4, p_stFile);
        fwrite(&GAM_g_ul500250_storage, 1, 4, p_stFile);
        fwrite(&GAM_g_ul500254_storage, 1, 4, p_stFile);
        fwrite(&GAM_g_ul500258, 1, 4, p_stFile);
        fwrite(&AI_PAD_cAnalogMinX_storage, 1, 1, p_stFile);
        fwrite(&AI_PAD_cAnalogMaxX_storage, 1, 1, p_stFile);
        fwrite(&AI_PAD_cAnalogMinY_storage, 1, 1, p_stFile);
        fwrite(&AI_PAD_cAnalogMaxY_storage, 1, 1, p_stFile);
        fwrite(&AI_PAD_cAnalogValueX_storage, 1, 1, p_stFile);
        fwrite(&AI_PAD_cAnalogValueY_storage, 1, 1, p_stFile);
        IPT_fn_bSaveOptionsInFile(p_stFile);
        fclose(p_stFile);
        FUN_0048d2c0(szTmpPath, szOptPath, 2);
    }
    else
    {
        bResult = 0;
    }
    remove(szTmpPath);
    return bResult;
}

extern void fn_vValidateAllOption(void);
extern unsigned char __cdecl IPT_fn_bLoadOptionsFromFile(FILE *p_stFile);
extern void *_alloca(unsigned int);                        /* 0x0045FE80 */

/* 0x00407E60 */
unsigned char __cdecl fn_bLoadGameOptions(char *pszFileName)
{
    unsigned char     bResult = 0;
    char              szTmpPath[0x104];
    char              szOptPath[0x104];
    char              szDiscard[0xb];
    FILE             *p_stFile;
    long              lHeaderLen;
    unsigned long    ulCount;
    char             *pszVersionBuf;
    long              lFileNb;
    long              lAvail;
    long              lSlotId;
    long              lChanged;
    long              lWalk;
    long              lScanId;
    long              lBrightness;
    GAM_tdstSaveSlot *p_stNode;
    GAM_tdstSaveSlot *p_stScan;
    GAM_tdstSaveSlot *p_stNext;
    unsigned char     bDecoded;

    lChanged = 0;
    sprintf(szTmpPath, "%s/TT.tmp", FUN_004928c0());
    sprintf(szOptPath, "%s/%s", FUN_00492850(), pszFileName);
    remove(szTmpPath);
    bDecoded = FUN_0048d370(szOptPath, szTmpPath, 2);
    if (bDecoded && (p_stFile = fopen(szTmpPath, "rb")) != NULL)
    {
        fread(&lHeaderLen, 1, 4, p_stFile);
        ulCount = (unsigned long)lHeaderLen + 1;
        pszVersionBuf = (char *)_alloca((unsigned int)((ulCount + 3) & 0xfc));
        fread(pszVersionBuf, 1, ulCount, p_stFile);
        if (strcmp(GAM_SS_szOptionsVersion, pszVersionBuf) == 0)
        {
            lAvail = GAM_g_lNbSaveSlots;
            if (lAvail == 0)
            {
                lAvail = fn_lGetNbAvailableSlots();
            }
            fread(&lFileNb, 1, 4, p_stFile);
            lWalk = 0;
            p_stNode = GAM_g_p_stSaveSlotHead;
            if (0 < lFileNb)
            {
                do {
                    if (lWalk < lAvail)
                    {
                        fread(p_stNode->szNameScore, 1, 0xb, p_stFile);
                        fread(&lSlotId, 1, 4, p_stFile);
                        if ((char)GAM_pc_fn_bBuildSaveSlotPath(szTmpPath, lSlotId) != 0 &&
                            FIL_fn_bIsValidPath(szTmpPath))
                        {
                            lScanId = GAM_g_p_stSaveSlotHead->lSlotId;
                            p_stScan = GAM_g_p_stSaveSlotHead;
                            while (lScanId != lSlotId && (p_stNext = p_stScan->pNext) != NULL)
                            {
                                lScanId = p_stNext->lSlotId;
                                p_stScan = p_stNext;
                            }
                            if (p_stScan != p_stNode)
                            {
                                lChanged = 1;
                                p_stScan->lSlotId = p_stNode->lSlotId;
                                p_stNode->lSlotId = lSlotId;
                            }
                        }
                        p_stNode = p_stNode->pNext;
                    }
                    else
                    {
                        fread(szDiscard, 1, 0xb, p_stFile);
                        fread(&lSlotId, 1, 4, p_stFile);
                    }
                    lWalk = lWalk + 1;
                } while (lWalk < lFileNb);
            }
            if (lWalk < lAvail)
            {
                do {

                    unsigned long ulLen;
                    unsigned long ulDw;
                    char         *pSrc;
                    char         *pDst;
                    char          cChar;

                    ulLen = 0xffffffff;
                    pSrc = "007 100";
                    do
                    {
                        if (ulLen == 0)
                            break;
                        ulLen = ulLen - 1;
                        cChar = *pSrc;
                        pSrc = pSrc + 1;
                    } while (cChar != 0);
                    ulLen = ~ulLen;
                    pSrc = pSrc - ulLen;
                    pDst = p_stNode->szNameScore;
                    for (ulDw = ulLen >> 2; ulDw != 0; ulDw--)
                    {
                        *(unsigned long *)pDst = *(unsigned long *)pSrc;
                        pSrc = pSrc + 4;
                        pDst = pDst + 4;
                    }
                    for (ulLen = ulLen & 3; ulLen != 0; ulLen--)
                    {
                        *pDst = *pSrc;
                        pSrc = pSrc + 1;
                        pDst = pDst + 1;
                    }
                    p_stNode = p_stNode->pNext;
                    lWalk = lWalk + 1;
                } while (lWalk < lAvail);
            }
            if (lChanged != 0)
            {
                GAM_pc_fn_ucSortSaveSlotsByDate();
            }
            GAM_g_lSaveSlotDirty = 0;
            fread(&GAM_g_ul500248_storage, 1, 4, p_stFile);
            fread(&GAM_g_ul50024c, 1, 4, p_stFile);
            fread(&lBrightness, 1, 4, p_stFile);
            GLI_fn_vSetBrightness(lBrightness);
            fread(&GAM_g_ul500250_storage, 1, 4, p_stFile);
            fread(&GAM_g_ul500254_storage, 1, 4, p_stFile);
            fread(&GAM_g_ul500258, 1, 4, p_stFile);
            fread(&AI_PAD_cAnalogMinX_storage, 1, 1, p_stFile);
            fread(&AI_PAD_cAnalogMaxX_storage, 1, 1, p_stFile);
            fread(&AI_PAD_cAnalogMinY_storage, 1, 1, p_stFile);
            fread(&AI_PAD_cAnalogMaxY_storage, 1, 1, p_stFile);
            fread(&AI_PAD_cAnalogValueX_storage, 1, 1, p_stFile);
            fread(&AI_PAD_cAnalogValueY_storage, 1, 1, p_stFile);
            IPT_fn_bLoadOptionsFromFile(p_stFile);
            fclose(p_stFile);
            fn_vValidateAllOption();
            bResult = 1;
        }
    }
    remove(szTmpPath);
    return bResult;
}

char GAM_g_a_szOptionsSlotNames[3][9];

unsigned char __cdecl GAM_pc_fn_bLoadSaveStateFromLoadFile(void);

/* 0x00407DD0 */
unsigned char fn_bLoadCurrentGameOptions(void)
{
    unsigned char ucReturn = 1;
    int uwFileNode;

    if (!fn_bLoadGameOptions(GAM_g_szCurrentGameOptionsFileName))
    {
        if (!GAM_pc_fn_bLoadSaveStateFromLoadFile())
            ucReturn = 0;
    }
    if (!ucReturn)
    {
        GAM_g_ul500248_storage = 0;
        GAM_g_ul500250_storage = 127;
        GAM_g_ul500258 = 127;
        GAM_g_ul500254_storage = 127;
        GAM_g_ul50024c = 0;
        for (uwFileNode = 0; uwFileNode < 3; uwFileNode++)
        {
            memset(GAM_g_a_szOptionsSlotNames[uwFileNode], ' ', 8);
            GAM_g_a_szOptionsSlotNames[uwFileNode][8] = '\0';
        }
    }
    return ucReturn;
}

/* 0x00407E50 */
unsigned char __cdecl GAM_pc_fn_bLoadSaveStateFromLoadFile(void)
{
    return fn_bLoadGameOptions(GAM_pc_szLoadFileName);
}

unsigned char __cdecl fn_bSaveGameInSlot(unsigned long ulSlotId)
{
    char              szSlotPath[0x104];
    char              szHiddenPath[0x104];
    GAM_tdstSaveSlot *p_stSlot;
    long              lSlotId;

    if (ulSlotId < 1 || ulSlotId > (unsigned long)GAM_g_lNbSaveSlots)
        return 0;

    sprintf(szHiddenPath, "%s\\%s", FUN_00492700(), GAM_SS_pszHiddenSlot);
    if (!FIL_fn_bIsValidPath(szHiddenPath))
        return 1;

    p_stSlot = GAM_g_p_stSaveSlotHead;
    if (ulSlotId != 1)
    {
        unsigned long ulCount2 = GAM_g_ulSaveSlotCount2;
        unsigned long i = 1;
        do {
            if (ulCount2 == 0)
                break;
            p_stSlot = p_stSlot->pNext;
            i++;
        } while (i != ulSlotId);
    }
    lSlotId = p_stSlot->lSlotId;

    if (GAM_pc_fn_bBuildSaveSlotPath(szSlotPath, lSlotId) &&
        GAM_pc_fn_bDeleteDirectoryTree(szSlotPath) &&
        FIL_fn_bValidatePath(szSlotPath, GAM_SS_pszEmptyPrefix) &&
        GAM_pc_fn_bCopyDirectoryTree(szHiddenPath, szSlotPath) &&
        GAM_pc_fn_ucUpdateSaveSlotList(szSlotPath, lSlotId) &&
        fn_bSaveCurrentGameOptions())
        return 1;

    return 0;
}

/* 0x0049F8D4 */
extern unsigned char GAM_pc_g_ucMode6SaveFlag_storage;
#define GAM_pc_g_ucMode6SaveFlag  GAM_pc_g_ucMode6SaveFlag_storage

/* 0x00410EC0 */
unsigned char __cdecl GAM_pc_fn_ucIsSaveSlotPathValid(unsigned long ulSlotId)
{
    char szSlotPath[260];
    long lSlotNum;

    if ((ulSlotId >= 1) && (ulSlotId <= (unsigned long)GAM_g_lNbSaveSlots)) {
        lSlotNum = GAM_pc_fn_lGetSaveSlotId((long)ulSlotId);
        sprintf(szSlotPath, "%s\\Slot%i", FUN_00492700(), (int)lSlotNum);
        return FIL_fn_bIsValidPath(szSlotPath);
    }
    return 0;
}

/* 0x00410F20 */
unsigned char __cdecl fn_bLoadGameFromSlot(unsigned long ulSlotId)
{
    char szHiddenPath[260];
    char szSlotPath[260];
    long lSlotNum;
    unsigned char ucResult;

    ucResult = 0;
    if (GAM_pc_fn_ucIsSaveSlotPathValid(ulSlotId)) {
        lSlotNum = GAM_pc_fn_lGetSaveSlotId((long)ulSlotId);
        sprintf(szSlotPath, "%s\\Slot%i", FUN_00492700(), (int)lSlotNum);
        strncpy(GAM_g_szSlotNameWork, GAM_pc_fn_p_szGetSaveSlotNameScore((long)ulSlotId), 3);
        sprintf(szHiddenPath, "%s\\%s", FUN_00492700(), GAM_SS_pszHiddenSlot);
        if (GAM_pc_fn_bDeleteDirectoryTree(szHiddenPath)) {
            FIL_fn_bValidatePath(szHiddenPath, GAM_SS_pszEmptyPrefix);
            ucResult = (unsigned char)GAM_pc_fn_bCopyDirectoryTree(szSlotPath, szHiddenPath);
        }
    }
    GAM_pc_g_ucMode6SaveFlag = 0;
    return ucResult;
}

/* 0x00410FF0 */
unsigned char __cdecl GAM_pc_fn_ucErasePrepSaveSlot(unsigned long ulSlotId)
{
    char szSlotPath[260];
    char szLastPath[260];
    GAM_tdstSaveSlot *p_stPrev;
    unsigned char ucResult;

    ucResult = 1;
    if (GAM_pc_fn_ucIsSaveSlotPathValid(ulSlotId)) {
        sprintf(szLastPath, "%s\\Slot%i", FUN_00492700(), (int)GAM_g_lNbSaveSlots);
        if (GAM_g_lNbSaveSlots != GAM_pc_fn_lGetSaveSlotId((long)ulSlotId)) {
            sprintf(szSlotPath, "%s\\Slot%i", FUN_00492700(),
                    (int)GAM_pc_fn_lGetSaveSlotId((long)ulSlotId));
            if (FIL_fn_bValidatePath(szSlotPath, GAM_SS_pszEmptyPrefix)) {
                GAM_pc_fn_bCopyDirectoryTree(szLastPath, szSlotPath);
            }
            p_stPrev = GAM_g_p_stSaveSlotHead;
            while ((p_stPrev->pNext != NULL) && (p_stPrev->lSlotId != GAM_g_lNbSaveSlots)) {
                p_stPrev = p_stPrev->pNext;
            }
            p_stPrev->lSlotId = GAM_pc_fn_lGetSaveSlotId((long)ulSlotId);
        }
        ucResult = FIL_fn_bDestroyFileOrDirectory(szLastPath);
    }
    return ucResult;
}

/* 0x004083D0 */
unsigned long __cdecl GAM_pc_fn_ulRemoveSaveSlotNode(long lSlotIndex)
{
    GAM_tdstSaveSlot *p_stHead;
    GAM_tdstSaveSlot *p_stPrev;
    GAM_tdstSaveSlot *p_stCursor;
    long              lWalk;
    long              lLast;

    lWalk = 0;
    if (lSlotIndex > 1)
    {
        lLast = lSlotIndex - 1;
        p_stCursor = GAM_g_p_stSaveSlotHead;
        p_stPrev = p_stCursor;
        while (lWalk != lLast)
        {
            if (p_stCursor == NULL)
            {
                break;
            }
            p_stPrev = p_stCursor;
            p_stCursor = p_stPrev->pNext;
            lWalk = lWalk + 1;
        }

        p_stPrev->pNext = p_stCursor->pNext;
        GLI_pc_fn_vFree((void **)&p_stCursor);
        GAM_g_lNbSaveSlots = GAM_g_lNbSaveSlots - 1;
        return 1;
    }

    p_stHead = GAM_g_p_stSaveSlotHead;
    GLI_pc_fn_vFree((void **)&GAM_gs_pSaveSlotHead);
    GAM_g_lNbSaveSlots = GAM_g_lNbSaveSlots - 1;
    GAM_g_p_stSaveSlotHead = p_stHead->pNext;
    return 1;
}

/* 0x004104A0 */
unsigned char __cdecl GAM_pc_fn_bDeleteSaveSlot(unsigned int uSlot);

/* 0x004110E0 */
unsigned long __cdecl GAM_pc_fn_ulEraseSaveSlotDir(long lSlotIndex)
{
    long                 lWalk;
    GAM_tdstSaveSlot    *p_stNode;
    char                 szPath[260];
    unsigned long        ulResult;
    unsigned char        ucResult;

    lWalk = 1;
    p_stNode = GAM_g_p_stSaveSlotHead;
    if (lSlotIndex != 1)
    {
        do
        {
            if (GAM_g_ulSaveSlotCount2 == 0)
            {
                break;
            }
            p_stNode = p_stNode->pNext;
            lWalk = lWalk + 1;
        } while (lWalk != lSlotIndex);
    }
    if ((unsigned char)GAM_pc_fn_bBuildSaveSlotPath(szPath, p_stNode->lSlotId) == 0)
    {
        return 0;
    }
    if (GAM_pc_fn_bDeleteDirectoryTree(szPath) == 0)
    {
        return 0;
    }
    if (FIL_fn_bValidatePath(szPath, GAM_SS_pszEmptyPrefix) == 0)
    {
        return 0;
    }
    ucResult = GAM_pc_fn_bDeleteSaveSlot(0);
    ulResult = ucResult;
    return ulResult;
}
