#include <stdio.h>
#include <string.h>
#include <windows.h>

#include "SND/SND.h"
#include "SND/sndfile.h"

static char SND_pc_szSoftDirectory[0x100] = ".";   /* 0x004A2538 */

static char SND_pc_szDataDirectory[0x10][0x100];   /* 0x004B7500 */
static int  SND_pc_iDataDirectoryCount;            /* 0x004B8500 */

typedef struct SND_pc_tdstDataDirSlot_
{
    char _szPath[0x100];
}
SND_pc_tdstDataDirSlot;

extern char g_cCDROM;

void SND_CALL SND_fn_vSetSoftDirectory(char *czDir)
{
    strcpy(SND_pc_szSoftDirectory, czDir);
}

char * SND_CALL SND_fn_szGetSoftDirectory(void)
{
    return SND_pc_szSoftDirectory;
}

void SND_CALL SND_fn_vAddDataDirectory(const char *szDir)
{
    if (!SND_fn_bIsDataDirectory(szDir))
    {
        if (SND_pc_iDataDirectoryCount < 0x10)
        {
            strcpy(SND_pc_szDataDirectory[SND_pc_iDataDirectoryCount], szDir);
            SND_pc_iDataDirectoryCount = SND_pc_iDataDirectoryCount + 1;
            return;
        }
        SND_fn_vDisplayError(8, "Too many SoundData directories");
    }
    return;
}

void SND_CALL SND_fn_vRemoveDataDirectory(const char *cdDir)
{
    int            iIndex;
    char          *pszSlot;

    iIndex = 0;
    if (0 < SND_pc_iDataDirectoryCount)
    {
        pszSlot = SND_pc_szDataDirectory[0];
        while ((iIndex < SND_pc_iDataDirectoryCount) && (_stricmp(pszSlot, cdDir) != 0))
        {
            iIndex = iIndex + 1;
            pszSlot = pszSlot + 0x100;
        }
    }
    if ((iIndex != SND_pc_iDataDirectoryCount) && (SND_pc_iDataDirectoryCount != 0))
    {
        SND_pc_iDataDirectoryCount = SND_pc_iDataDirectoryCount - 1;
        *(SND_pc_tdstDataDirSlot *)SND_pc_szDataDirectory[iIndex] =
            *(SND_pc_tdstDataDirSlot *)SND_pc_szDataDirectory[SND_pc_iDataDirectoryCount];
    }
    return;
}

void SND_CALL SND_fn_vPurgeDataDirectory(void)
{
    SND_pc_iDataDirectoryCount = 0;
}

SndBool SND_CALL SND_fn_bGetOneDataDirectory(int id, char *czDir, int iMaxChar)
{
    if (id < SND_pc_iDataDirectoryCount)
    {
        strncpy(czDir, SND_pc_szDataDirectory[id], iMaxChar);
        return 1;
    }
    return 0;
}

int SND_CALL SND_fn_iGetNumberOfDataDirectory(void)
{
    return SND_pc_iDataDirectoryCount;
}

int SND_CALL SND_fn_iGetIdDataDirectory(const char *szDir)
{
    int   iId;

    for (iId = 0; iId < SND_pc_iDataDirectoryCount; iId++)
    {
        if (_stricmp(SND_pc_szDataDirectory[iId], szDir) == 0)
            return iId;
    }
    return -1;
}

SndBool SND_CALL SND_fn_bIsDataDirectory(const char *szDir)
{
    int iId;

    iId = SND_fn_iGetIdDataDirectory(szDir);
    return iId != -1;
}

void SND_CALL SND_fn_vResolveFileName(char *czRelativeName, char *czAbsoluteName)
{
    int    iDir;
    tdhSndFile hFile;
    char   szPath[0x100];
    char   szMsg[0x100];

    for (iDir = 0; iDir < SND_pc_iDataDirectoryCount; iDir = iDir + 1)
    {
        strcpy(szPath, SND_pc_szDataDirectory[iDir]);
        strcat(szPath, czRelativeName);
        hFile = SND_fn_hOpenFileReadSnd(szPath);
        if (hFile != OPENFILESND_FAILED)
        {
            SND_fn_vCloseFileSnd(hFile);
            strncpy(czAbsoluteName, szPath, 0x100);
            return;
        }
    }
    for (iDir = 0; iDir < SND_pc_iDataDirectoryCount; iDir = iDir + 1)
    {
        sprintf(szPath, "%c\\\\", (int)g_cCDROM);
        strcat(szPath, SND_pc_szDataDirectory[iDir]);
        strcat(szPath, czRelativeName);
        hFile = SND_fn_hOpenFileReadSnd(szPath);
        if (hFile != OPENFILESND_FAILED)
        {
            SND_fn_vCloseFileSnd(hFile);
            strncpy(czAbsoluteName, szPath, 0x100);
            return;
        }
    }
    sprintf(szMsg, "File %s cannot be found in any DataDirectory", czRelativeName);
    SND_fn_vDisplayError(10, szMsg);
    strncpy(czAbsoluteName, czRelativeName, 0x100);
}

SndBool SND_CALL SND_fn_bSetCurrentDirectory(char *czDir)
{
    return SND_pc_M_bSetCurrentDirectory(czDir);
}

SndBool SND_CALL SND_fn_bGetCurrentDirectory(char *czDir, int iMaxChar)
{
    return SND_pc_M_ulGetCurrentDirectory(czDir, iMaxChar);
}

SndBool SND_CALL SND_fn_bLoadDataInMemAndSwap(void *adr, unsigned long ulNbBytes, char *czFileName,
                                               unsigned long ulOffset, SndBool bSwap)
{
    tdhSndFile hFile;
    unsigned long dwRead;
    int            iPairs;
    unsigned char *p;
    unsigned char  ucTmp;

    hFile = SND_fn_hOpenFileReadSnd(czFileName);
    if (hFile != OPENFILESND_FAILED)
    {
        SND_fn_dwSeekFileSnd(hFile, ulOffset, SEEKFILESND_BEGIN);
        dwRead = SND_fn_dwReadFileSnd(hFile, ulNbBytes, adr);
        SND_fn_vCloseFileSnd(hFile);
        if (bSwap != 0)
        {
            p = (unsigned char *)adr;
            for (iPairs = (int)(ulNbBytes >> 1); iPairs > 0; iPairs = iPairs - 1)
            {
                ucTmp = *p;
                *p = p[1];
                p[1] = ucTmp;
                p = p + 2;
            }
        }
        return dwRead == ulNbBytes;
    }
    return FALSE;
}

SndBool SND_CALL SND_fn_bLoadDataInMem(void *adr, unsigned long ulNbBytes, char *czFileName,
                                        unsigned long ulOffset)
{
    return SND_fn_bLoadDataInMemAndSwap(adr, ulNbBytes, czFileName, ulOffset, 0);
}

unsigned long SND_CALL SND_fn_dwFileSndLength(tdhSndFile hSndFile)
{
    return SND_pc_M_ulGetFileSize(hSndFile);
}

tdhSndFile SND_CALL SND_fn_hOpenFileReadSnd(char *czFileName)
{
    tdhSndFile hFile;
    unsigned long dwError;
    char   szCwd[0x50];
    char   *pszMsg;
    char   szMsg[0x100];

    hFile = SND_pc_M_hOpenFile(czFileName, OPENFILESND_READ);

    if (hFile == OPENFILESND_FAILED)
    {
        SND_pc_M_ulGetCurrentDirectory(szCwd, 0x50);
        dwError = SND_pc_M_ulGetLastError();
        if ((dwError != 2) && (dwError != 3))
        {
            SND_pc_M_ulFormatErrorText(dwError, &pszMsg);
            strcpy(szMsg, czFileName);
            strcat(szMsg, " -> ");
            strncat(szMsg, pszMsg, 0xff - strlen(szMsg));
            SND_pc_M_vFreeErrorText(pszMsg);
        }
    }
    return hFile;
}

unsigned long SND_CALL SND_fn_dwReadFileSnd(tdhSndFile hSndFile, unsigned long dwNbBytes, void *ptr)
{
    int   bOk;
    unsigned long dwRead;

    dwRead = 0;
    bOk = SND_pc_M_bReadFile(hSndFile, ptr, dwNbBytes, &dwRead);
    if (bOk == 0)
        SND_pc_M_ulGetLastError();
    return dwRead;
}

unsigned long SND_CALL SND_fn_dwWriteFileSnd(tdhSndFile hSndFile, unsigned long dwNbBytes, void *ptr)
{
    unsigned long dwWritten;

    dwWritten = 0;
    SND_pc_M_bWriteFile(hSndFile, ptr, dwNbBytes, &dwWritten);
    return dwWritten;
}

unsigned long SND_CALL SND_fn_dwSeekFileSnd(tdhSndFile hSndFile, unsigned long dwOffset, unsigned long dwOrigin)
{
    return SND_pc_M_ulSeekFile(hSndFile, dwOffset, dwOrigin);
}

void SND_CALL SND_fn_vCloseFileSnd(tdhSndFile hSndFile)
{
    SND_pc_M_bCloseFile(hSndFile);
}
