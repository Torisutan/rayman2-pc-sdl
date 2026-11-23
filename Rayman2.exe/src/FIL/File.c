#include "FIL/File.h"
#include "ERM/CpaError.h"
#include "GLI/Memory.h"

#include <direct.h>
#include <io.h>
#include <stdio.h>
#include <string.h>
#include <sys/stat.h>

#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
#include <windows.h>

typedef struct {
    unsigned int   attrib;
    unsigned int   time_create;
    unsigned int   time_access;
    unsigned int   time_write;
    unsigned int   size;
    char           name[260];
} FIL_tdstFindData32;

#define s_fseek       fseek
#define s_fread       fread
#define s_ftell       ftell
#define s_fclose      fclose
#define s_fopen       fopen
#define s_findfirst(p, d)  _findfirst((p), (struct _finddata_t *)(d))
#define s_findnext(h, d)   _findnext((h), (struct _finddata_t *)(d))
#define s_findclose   _findclose

/* 0x004fb640 */
typedef struct {
    char     aucFourCc[4];     /* 0x004fb640 */
    uint32_t ulPayloadSize;    /* 0x004fb644 */
    char     aucType[4];       /* 0x004fb648 */
} FIL_tdstRiffHeaderScratch;
typedef struct {
    char     aucFourCc[4];     /* 0x004fb650 */
    uint32_t ulChunkSize;      /* 0x004fb654 */
} FIL_tdstRiffChunkScratch;
typedef char FIL_RiffScratch_size_check[
    (sizeof(FIL_tdstRiffHeaderScratch) == 0x0C &&
     sizeof(FIL_tdstRiffChunkScratch)  == 0x08) ? 1 : -1];

unsigned char FIL_pc_g_ucInitMarker;
static FIL_tdstRiffHeaderScratch FIL_g_stRiffHeaderScratch;
static FIL_tdstRiffChunkScratch  FIL_g_stRiffChunkScratch;
#define FIL_g_aucRiffHeaderScratch ((char *)&FIL_g_stRiffHeaderScratch)
#define FIL_g_ulRiffPayloadSize    (FIL_g_stRiffHeaderScratch.ulPayloadSize)
#define FIL_g_aucRiffTypeScratch   ((char *)FIL_g_stRiffHeaderScratch.aucType)
#define FIL_g_aucRiffChunkScratch  ((char *)&FIL_g_stRiffChunkScratch)
#define FIL_g_ulRiffChunkSize      (FIL_g_stRiffChunkScratch.ulChunkSize)
/* 0x4a6914 */
static const char FIL_g_szAviFourCc_storage[]  = "AVI ";
static const char FIL_g_szRiffFourCc_storage[] = "RIFF";
#define FIL_g_szAviFourCc  (FIL_g_szAviFourCc_storage)
#define FIL_g_szRiffFourCc (FIL_g_szRiffFourCc_storage)

/* 0x4a6924 */
static const char FIL_g_szCntExt_storage[] = ".CNT";
static const char FIL_g_szModeRb_storage[] = "rb";
#define FIL_g_szCntExt  (FIL_g_szCntExt_storage)
#define FIL_g_szModeRb  (FIL_g_szModeRb_storage)

typedef struct FIL_tdstConcatFileElement {
    char *p_szFileName;
    long lPathNumber;
    long lOffset;
    unsigned long ulFileLength;
    unsigned long ulXorCode;
    unsigned long ulCheckSum;
} FIL_tdstConcatFileElement;

typedef struct FIL_tdstConcatFile {
    FILE *p_stFile;
    unsigned long ulNumberOfPath;
    unsigned long ulNumberOfFile;
    unsigned char bUseXorCode;
    unsigned char bUseCheckSum;
    unsigned char ucPathXorCode;
    unsigned char ucPathCheckSum;
    char **d_szPathList;
    FIL_tdstConcatFileElement *d_stConcatFileElement;
} FIL_tdstConcatFile;

typedef struct FIL_tdstFileInConcatFile_ {
    unsigned char  bUseConcatFile;
    FILE          *p_stFile;
    unsigned long  ulFileActualSeek;
    unsigned long  ulFileEndSeek;
    unsigned long  ulFileXorCode;
    unsigned long  ulFileCheckSum;
    unsigned long  ulCacheSeek;
    unsigned long  ulCacheSize;
    unsigned long  ulCacheCheckSum;
    unsigned char *p_ucCache;
} FIL_tdstFileInConcatFile;

FIL_tdstFileInConcatFile *FIL_g_hGlobalFileInConcatFile;   /* 0x004b9c48 */

typedef struct FIL_tdstFileNameListElement_ FIL_tdstFileNameListElement;
typedef struct FIL_tdstFileNameListAnchor_s  FIL_tdstFileNameListAnchor;

struct FIL_tdstFileNameListElement_ {
    FIL_tdstFileNameListElement *p_stNext;
    FIL_tdstFileNameListElement *p_stPrev;
    FIL_tdstFileNameListAnchor  *p_stAnchor;
    char *szFileName;
};

struct FIL_tdstFileNameListAnchor_s {
    FIL_tdstFileNameListElement *p_stHead;
    FIL_tdstFileNameListElement *p_stTail;
    int iCount;
};

/* 0x004a68ec */
unsigned char FIL_g_ucFILModuleId = 0xFF;

unsigned long __cdecl FIL_fn_ulGetFileSize(char *szFileName)
{
    int iHandle;
    long lLength = 0;

    iHandle = _open(szFileName, 0x8000, 0x100);
    if (iHandle != -1) {
        lLength = _filelength(iHandle);
        _close(iHandle);
    }

    return (unsigned long)lLength;
}

void *__cdecl FIL_fn_hCreateConcatFileHandle(unsigned long ulNumberOfPath,
                                             unsigned long ulNumberOfFiles)
{
    FIL_tdstConcatFile *hConcatFile;

    hConcatFile = (FIL_tdstConcatFile *)GLI_pc_fn_p_vAlloc(0x18);
    hConcatFile->ulNumberOfPath = ulNumberOfPath;
    hConcatFile->ulNumberOfFile = ulNumberOfFiles;

    if (ulNumberOfPath != 0) {
        hConcatFile->d_szPathList =
            (char **)GLI_pc_fn_p_vAlloc(ulNumberOfPath * 4u);
    } else {
        hConcatFile->d_szPathList = NULL;
    }

    hConcatFile->d_stConcatFileElement =
        (FIL_tdstConcatFileElement *)GLI_pc_fn_p_vAlloc(
            ulNumberOfFiles * sizeof(FIL_tdstConcatFileElement));

    return hConcatFile;
}

void __cdecl fn_vDestroyConcatFileHandle(void *hConcatFile)
{
    unsigned long i;

    if (((FIL_tdstConcatFile *)hConcatFile)->d_szPathList != NULL) {
        for (i = 0; i < ((FIL_tdstConcatFile *)hConcatFile)->ulNumberOfPath; ++i) {
            GLI_pc_fn_vFree((void **)&((FIL_tdstConcatFile *)hConcatFile)->d_szPathList[i]);
        }
        GLI_pc_fn_vFree((void **)&((FIL_tdstConcatFile *)hConcatFile)->d_szPathList);
    }

    if (((FIL_tdstConcatFile *)hConcatFile)->d_stConcatFileElement != NULL) {
        for (i = 0; i < ((FIL_tdstConcatFile *)hConcatFile)->ulNumberOfFile; ++i) {
            GLI_pc_fn_vFree(
                (void **)&((FIL_tdstConcatFile *)hConcatFile)->d_stConcatFileElement[i].p_szFileName);
        }
        GLI_pc_fn_vFree((void **)&((FIL_tdstConcatFile *)hConcatFile)->d_stConcatFileElement);
    }

    GLI_pc_fn_vFree(&hConcatFile);
}

long __cdecl FIL_FUN_00492fc0(FILE *pFile,
                              const char *szSectionName,
                              const char *szExtension)
{
    uint32_t ulNameLength;
    long lScanEnd;
    long lPos;
    char szRecordName[260];

    if (s_fseek(pFile, 0, SEEK_SET) != 0) {
        return -1;
    }

    if (s_fread(FIL_g_aucRiffHeaderScratch, 1, 0x0c, pFile) != 0x0c) {
        return -1;
    }

    if (strncmp(FIL_g_aucRiffHeaderScratch, FIL_g_szRiffFourCc, 4) != 0) {
        return -1;
    }

    if (strncmp(FIL_g_aucRiffTypeScratch, FIL_g_szAviFourCc, 4) != 0) {
        return -1;
    }

    lScanEnd = (long)(FIL_g_ulRiffPayloadSize + 8u);
    lPos = s_ftell(pFile);
    if (lPos == -1) {
        return -1;
    }

    while (lPos < lScanEnd) {
        if (s_fread(FIL_g_aucRiffChunkScratch, 1, 8, pFile) != 8) {
            return -1;
        }

        if (strncmp(FIL_g_aucRiffChunkScratch, szExtension, 4) == 0) {
            ulNameLength = 0;
            if (s_fread(&ulNameLength, 1, 1, pFile) == 1 &&
                s_fread(szRecordName, 1, ulNameLength & 0xffu, pFile) ==
                    (ulNameLength & 0xffu)) {
                if (szSectionName[0] == '\0' ||
                    _stricmp(szRecordName, szSectionName) == 0) {
                    return s_ftell(pFile);
                }
            }
        }

        if (s_fseek(pFile,
                    (long)((FIL_g_ulRiffChunkSize & 1u) +
                           FIL_g_ulRiffChunkSize),
                    SEEK_CUR) != 0) {
            return -1;
        }

        lPos = s_ftell(pFile);
        if (lPos == -1) {
            return -1;
        }
    }

    return -1;
}

/* 0x004929c0 */
unsigned char __cdecl FIL_fn_bIsFileExist(char *_szFileName)
{
    FILE *p_stFile;
    unsigned char bExists;

    bExists = 0;
    p_stFile = fopen(_szFileName, FIL_g_szModeRb);
    if (p_stFile != NULL) {
        fclose(p_stFile);
        bExists = 1;
    }
    return bExists;
}

/* 0x00492f60 */
void __cdecl FIL_fn_vBuildConcatCompleteFileName(char *szCompleteFilename,
                                                  void *hConcatFile,
                                                  unsigned long ulIndex)
{
    FIL_tdstConcatFile *pConcatFile = (FIL_tdstConcatFile *)hConcatFile;
    FIL_tdstConcatFileElement *pEntry = &pConcatFile->d_stConcatFileElement[ulIndex];

    if (pEntry->lPathNumber == -1L) {
        strcpy(szCompleteFilename, pEntry->p_szFileName);
    } else {
        sprintf(szCompleteFilename, "%s\\%s",
                pConcatFile->d_szPathList[pEntry->lPathNumber],
                pEntry->p_szFileName);
    }
}

/* 0x004938d0 */
void __cdecl FIL_fn_vCloseFileInConcatFile(FIL_tdstFileInConcatFile **hHandleToFile)
{

    if (((*hHandleToFile)->ulFileEndSeek == (*hHandleToFile)->ulFileActualSeek) &&
        ((*hHandleToFile)->ulFileCheckSum != 0) &&
        ((*hHandleToFile)->ulCacheCheckSum != (*hHandleToFile)->ulFileCheckSum)) {
        ERM_fn_vClearLastError(0);
        ERM_fn_vUpdateLastError(6, FIL_g_ucFILModuleId, 0, 5,
                                (long)-1, 0xFF, 0xFF, NULL);
    }

    if ((*hHandleToFile)->bUseConcatFile == 0) {
        fclose((*hHandleToFile)->p_stFile);
    }

    if (hHandleToFile != NULL) {
        if ((*hHandleToFile)->p_ucCache != NULL) {
            GLI_pc_fn_vFree((void **)&(*hHandleToFile)->p_ucCache);
        }
        GLI_pc_fn_vFree((void **)hHandleToFile);
        *hHandleToFile = NULL;
    }
}

/* 0x00493c20 */
void __cdecl FIL_fn_vInitGlobalHandleToFileInConcatFile(void)
{
    if (FIL_g_hGlobalFileInConcatFile == NULL) {
        FIL_g_hGlobalFileInConcatFile =
            (FIL_tdstFileInConcatFile *)GLI_pc_fn_p_vAlloc(0x28);
        FIL_g_hGlobalFileInConcatFile->bUseConcatFile = 1;
        FIL_g_hGlobalFileInConcatFile->ulCacheCheckSum = 0;
        FIL_g_hGlobalFileInConcatFile->p_ucCache =
            (unsigned char *)GLI_pc_fn_p_vAlloc(0x40000);
        FIL_g_hGlobalFileInConcatFile->ulCacheSize = 0x40000;
        FIL_g_hGlobalFileInConcatFile->ulCacheSeek =
            FIL_g_hGlobalFileInConcatFile->ulCacheSize;
    }
}

/* 0x00493c80 */
void __cdecl FIL_fn_vFreeGlobalHandleToFileInConcatFile(void)
{
    if (FIL_g_hGlobalFileInConcatFile != NULL) {

        FIL_g_hGlobalFileInConcatFile->ulCacheCheckSum =
            FIL_g_hGlobalFileInConcatFile->ulFileCheckSum;

        FIL_g_hGlobalFileInConcatFile->ulCacheSize  = 0x40000;
        FIL_g_hGlobalFileInConcatFile->ulCacheSeek  =
            FIL_g_hGlobalFileInConcatFile->ulCacheSize;
        FIL_fn_vCloseFileInConcatFile(&FIL_g_hGlobalFileInConcatFile);
    }
}

/* 0x004929f0 */
unsigned char __cdecl FIL_fn_bIsValidPath(char *_szFileName)
{
    struct _stat local_stat;
    return (unsigned char)(_stat(_szFileName, &local_stat) == 0);
}

/* 0x00493820 */
void __cdecl FIL_vFillCacheFromFile(FIL_tdstFileInConcatFile *hHandleToFile)
{
    size_t ulBytesToRead;
    size_t ulBytesRead;
    size_t ulIdx;
    size_t ulDwords;
    unsigned long *pul;

    fseek(hHandleToFile->p_stFile, (long)hHandleToFile->ulFileActualSeek, SEEK_SET);

    ulBytesToRead = hHandleToFile->ulCacheSize;
    if (hHandleToFile->ulFileEndSeek <
            (unsigned long)(ulBytesToRead + hHandleToFile->ulFileActualSeek)) {
        ulBytesToRead = hHandleToFile->ulFileEndSeek - hHandleToFile->ulFileActualSeek;
    }

    ulBytesRead = fread(hHandleToFile->p_ucCache, 1, ulBytesToRead, hHandleToFile->p_stFile);

    hHandleToFile->ulFileActualSeek += (unsigned long)ulBytesRead;
    hHandleToFile->ulCacheSeek       = 0;
    hHandleToFile->ulCacheSize       = (unsigned long)ulBytesRead;

    if ((hHandleToFile->ulFileXorCode != 0) || (hHandleToFile->ulFileCheckSum != 0)) {
        pul = (unsigned long *)hHandleToFile->p_ucCache;
        ulIdx = 0;
        ulDwords = ulBytesRead >> 2;
        if (ulDwords != 0) {
            do {
                if (hHandleToFile->ulFileXorCode != 0) {
                    pul[ulIdx] ^= hHandleToFile->ulFileXorCode;
                }
                if (hHandleToFile->ulFileCheckSum != 0) {
                    hHandleToFile->ulCacheCheckSum += pul[ulIdx];
                }
                ulIdx++;
            } while (ulIdx < ulDwords);
        }
    }
}

/* 0x00492930 */
void __cdecl FIL_fn_vDestroyOnElementInFileNameList(FIL_tdstFileNameListElement *hFileNameListElement)
{

    if (hFileNameListElement->p_stNext != NULL) {
        hFileNameListElement->p_stNext->p_stPrev = hFileNameListElement->p_stPrev;
    }
    if (hFileNameListElement->p_stPrev != NULL) {
        hFileNameListElement->p_stPrev->p_stNext = hFileNameListElement->p_stNext;
    }

    if (hFileNameListElement->p_stAnchor != NULL) {
        hFileNameListElement->p_stAnchor->iCount--;
        if (hFileNameListElement->p_stAnchor->p_stHead == hFileNameListElement) {
            hFileNameListElement->p_stAnchor->p_stHead = hFileNameListElement->p_stNext;
        }
        if (hFileNameListElement->p_stAnchor->p_stTail == hFileNameListElement) {
            hFileNameListElement->p_stAnchor->p_stTail = hFileNameListElement->p_stPrev;
        }
    }

    hFileNameListElement->p_stNext   = NULL;
    hFileNameListElement->p_stPrev   = NULL;
    hFileNameListElement->p_stAnchor = NULL;

    GLI_pc_fn_vFree((void **)&hFileNameListElement->szFileName);
    GLI_pc_fn_vFree((void **)&hFileNameListElement);
}

#define FIL_g_szSearchPathPrefix ("")

/* 0x00494040 */
static void __cdecl FIL_vAddMatchToFileNameList(FIL_tdstFileNameListAnchor *p_stAnchor,
                                                char *szBase,
                                                FIL_tdstFindData32 *p_stFindData,
                                                unsigned long ulAttribMask)
{
    FIL_tdstFileNameListElement *p_stElem;

    if ((p_stFindData->attrib & ulAttribMask) == 0)
        return;

    if (strcmp(p_stFindData->name, ".") == 0 ||
        strcmp(p_stFindData->name, "..") == 0)
        return;

    p_stElem = (FIL_tdstFileNameListElement *)GLI_pc_fn_p_vAlloc(sizeof(FIL_tdstFileNameListElement));

    if (*szBase == '\0') {
        p_stElem->szFileName =
            (char *)GLI_pc_fn_p_vAlloc(strlen(p_stFindData->name) + 1);
        sprintf(p_stElem->szFileName, "%s", p_stFindData->name);
    } else {
        p_stElem->szFileName =
            (char *)GLI_pc_fn_p_vAlloc((strlen(szBase) + 1) +
                                     (strlen(p_stFindData->name) + 1));
        sprintf(p_stElem->szFileName, "%s\\%s", szBase, p_stFindData->name);
    }

    p_stElem->p_stAnchor = p_stAnchor;
    p_stElem->p_stPrev   = p_stAnchor->p_stTail;
    if (p_stAnchor->p_stHead == NULL) {
        p_stAnchor->p_stHead = p_stElem;
    } else {
        p_stAnchor->p_stTail->p_stNext = p_stElem;
    }
    p_stAnchor->p_stTail = p_stElem;
    p_stAnchor->iCount++;
}

/* 0x004941d0 */
static void __cdecl FIL_vSearchOneDirectory(FIL_tdstFileNameListAnchor *p_stAnchor,
                                            char *szPattern,
                                            unsigned long ulAttrib)
{
    long               _FindHandle;
    FIL_tdstFindData32 finddata;

    _FindHandle = s_findfirst(szPattern, &finddata);
    if (_FindHandle != -1) {
        FIL_vAddMatchToFileNameList(p_stAnchor, FIL_g_szSearchPathPrefix,
                                    &finddata, ulAttrib);
        while (s_findnext(_FindHandle, &finddata) != -1) {
            FIL_vAddMatchToFileNameList(p_stAnchor, FIL_g_szSearchPathPrefix,
                                        &finddata, ulAttrib);
        }
        s_findclose(_FindHandle);
    }
}

/* 0x00492900 */
void *__cdecl FIL_fn_hSearchFile(char *_szFileName, unsigned long ulAttrib)
{
    FIL_tdstFileNameListAnchor *p_stAnchor;

    p_stAnchor = (FIL_tdstFileNameListAnchor *)GLI_pc_fn_p_vAlloc(0xc);
    p_stAnchor->p_stHead = NULL;
    p_stAnchor->p_stTail = NULL;
    p_stAnchor->iCount   = 0;

    FIL_vSearchOneDirectory(p_stAnchor, _szFileName, ulAttrib);
    return p_stAnchor;
}

/* 0x00493960 */
long __cdecl FIL_fn_lReadFileInConcatFile(void *p_vBuffer, long lSize, long lNumber,
                                          FIL_tdstFileInConcatFile *hHandleToFile)
{
    unsigned long ulBytesNeeded;
    unsigned long ulAvailable;
    long          lItemsRead;
    unsigned long ulCacheSeek;
    unsigned long ulCacheSize;
    unsigned long ulBytesInCache;
    unsigned char *pOut;
    unsigned char *pCache;

    lItemsRead = 0;
    if (hHandleToFile == NULL) {
        return 0;
    }
    ulBytesNeeded = (unsigned long)(lSize * lNumber);
    if ((int)ulBytesNeeded <= 0) {
        return 0;
    }

    ulAvailable = (hHandleToFile->ulFileEndSeek   - hHandleToFile->ulFileActualSeek
                 + hHandleToFile->ulCacheSize)     - hHandleToFile->ulCacheSeek;

    if (ulAvailable < ulBytesNeeded) {
        ulBytesNeeded = ulAvailable;
    }
    lItemsRead = (long)ulBytesNeeded / lSize;

    pOut = (unsigned char *)p_vBuffer;

    while (ulBytesNeeded != 0) {
        ulCacheSeek = hHandleToFile->ulCacheSeek;
        ulCacheSize = hHandleToFile->ulCacheSize;

        if (ulCacheSize < ulBytesNeeded + ulCacheSeek) {

            if (ulCacheSeek < ulCacheSize) {

                ulBytesInCache = ulCacheSize - ulCacheSeek;
                pCache = hHandleToFile->p_ucCache + ulCacheSeek;

                memcpy(pOut, pCache, ulBytesInCache);
                pOut += ulBytesInCache;

                hHandleToFile->ulCacheSeek = ulCacheSize;
                ulBytesNeeded -= ulBytesInCache;
            } else {

                FIL_vFillCacheFromFile(hHandleToFile);
            }
        } else {

            pCache = hHandleToFile->p_ucCache + ulCacheSeek;

            memcpy(pOut, pCache, ulBytesNeeded);

            hHandleToFile->ulCacheSeek += ulBytesNeeded;
            ulBytesNeeded = 0;
        }
    }

    return lItemsRead;
}

/* 0x00492a10 */
unsigned char __cdecl FIL_fn_bValidatePath(char *_p_szStartPath, char *_p_szFileName)
{
    char *pcVar;
    int   iVar;
    char *pcComp;
    size_t uLen;
    volatile unsigned char local_20a;
    unsigned char ucSavedDrv2;
    unsigned char ucSavedDrv3;
    char local_208[260];
    char local_104[260];

    local_20a = 1;

    while (strchr(_p_szStartPath, '/') != NULL) {
        *strchr(_p_szStartPath, '/') = '\\';
    }
    while (strchr(_p_szFileName, '/') != NULL) {
        *strchr(_p_szFileName, '/') = '\\';
    }

    uLen = strlen(_p_szStartPath);

    if ((_p_szStartPath[uLen - 1] == '\\') || (_p_szStartPath[0] == '\0')) {
        sprintf(local_208, "%s%s", _p_szStartPath, _p_szFileName);
    } else {
        sprintf(local_208, "%s\\%s", _p_szStartPath, _p_szFileName);
    }

    pcVar = strrchr(local_208, '\\');
    *pcVar = '\0';

    if (_access(local_208, 0) != -1) {
        return local_20a;
    }

    *pcVar = '\\';
    pcComp = local_208;
    _getcwd(local_104, (int)sizeof(local_104));

    if ((local_208[0] != '\0') && (local_208[1] == ':')) {
        ucSavedDrv2 = (unsigned char)local_208[2];
        ucSavedDrv3 = (unsigned char)local_208[3];
        local_208[2] = '\\';
        local_208[3] = '\0';
        iVar = _chdir(local_208);
        local_20a = (unsigned char)(iVar != -1);
        local_208[2] = (char)ucSavedDrv2;
        local_208[3] = (char)ucSavedDrv3;
    }

    while (local_20a && (pcVar = strchr(pcComp, '\\')) != NULL) {
        *pcVar = '\0';
        if (*pcComp == '\0') {

            _chdir("\\");

        } else {
            iVar = _chdir(pcComp);
            if (iVar == -1) {
                _mkdir(pcComp);
                iVar = _chdir(pcComp);
                if (iVar == -1) {
                    local_20a = 0;
                }
            }
            *pcVar  = '\\';
            pcComp  = pcVar + 1;

        }
    }

    iVar = _chdir(local_104);
    if (iVar == -1) {
        local_20a = 0;
    }

    return local_20a;
}

#pragma pack(push, 1)
typedef struct
{
    unsigned short usType;
    unsigned long  ulSize;
    unsigned long  ulReserved;
    unsigned long  ulOffBits;
} FIL_tdstPcBmpFileHeader;
#pragma pack(pop)

typedef struct
{
    unsigned long  ulSize;
    long           lWidth;
    long           lHeight;
    unsigned short usPlanes;
    unsigned short usBitCount;
    unsigned long  ulCompression;
    unsigned long  ulSizeImage;
    unsigned long  ulXPelsPerMeter;
    unsigned long  ulYPelsPerMeter;
    unsigned long  ulClrUsed;
    unsigned long  ulClrImportant;
} FIL_tdstPcBmpInfoHeader;

void __cdecl FIL_fn_vSaveBMPInFile(char *p_szFileName,
                                     int iPixelData,
                                     int iWidth,
                                     int iHeight,
                                     unsigned char ucBytesPerPixel,
                                     unsigned long ulPixelStride)
{
    FILE                    *pFile;
    int                      iRowOffset;
    int                      iPixelOffset;
    int                      iCount;
    unsigned long            ulImageSize;
    FIL_tdstPcBmpFileHeader  stFileHeader;
    FIL_tdstPcBmpInfoHeader  stInfoHeader;

    ulImageSize = (unsigned long)iHeight * (unsigned long)iWidth
                * (unsigned long)ucBytesPerPixel;

    stFileHeader.usType     = 0x4d42;
    stFileHeader.ulSize     = ulImageSize + 0x36;
    stFileHeader.ulReserved = 0;
    stFileHeader.ulOffBits  = 0x36;

    stInfoHeader.ulSize          = 0x28;
    stInfoHeader.lWidth          = iWidth;
    stInfoHeader.lHeight         = iHeight;
    stInfoHeader.usPlanes        = 1;
    stInfoHeader.usBitCount      = (unsigned short)(ucBytesPerPixel << 3);
    stInfoHeader.ulCompression   = 0;
    stInfoHeader.ulSizeImage     = ulImageSize;
    stInfoHeader.ulXPelsPerMeter = 0;
    stInfoHeader.ulYPelsPerMeter = 0;
    stInfoHeader.ulClrUsed       = 0;
    stInfoHeader.ulClrImportant  = 0;

    FIL_fn_bValidatePath(FIL_g_szSearchPathPrefix, p_szFileName);

    pFile = fopen(p_szFileName, "wb");
    if (pFile != NULL) {
        fwrite(&stFileHeader, 0xe, 1, pFile);
        fwrite(&stInfoHeader, 0x28, 1, pFile);
        if (iHeight >= 1) {
            iRowOffset = (iHeight - 1) * iWidth;
            do {
                if (iWidth > 0) {
                    iPixelOffset = iRowOffset * (int)(ulPixelStride & 0xff);
                    iCount = iWidth;
                    do {
                        fwrite((void *)(iPixelData + iPixelOffset), 1,
                               (unsigned int)ucBytesPerPixel, pFile);
                        iPixelOffset += (int)(ulPixelStride & 0xff);
                        iCount--;
                    } while (iCount != 0);
                }
                iRowOffset -= iWidth;
                iHeight--;
            } while (iHeight != 0);
        }
        fclose(pFile);
    }
}

/* 0x004935c0 */
void * __cdecl FIL_fn_hOpenFileInConcatFile(
    void *hConcatFile,
    char *p_szPath,
    char *p_szFilename)
{
    int bFound;
    char *pcBasename;
    char *pcSlash;
    int iCmp;
    FIL_tdstFileInConcatFile *pHandle;
    unsigned long uLen;
    FILE *pFile;
    FIL_tdstConcatFile *pConcat;
    FIL_tdstConcatFileElement *pElem;
    unsigned long ulIdx;
    char local_104[260];

    pHandle = NULL;
    bFound  = 0;

    pcSlash = strchr(p_szFilename, '/');
    while (pcSlash != NULL) {
        *pcSlash = '\\';
        pcSlash  = strchr(p_szFilename, '/');
    }

    pcBasename = p_szFilename;
    pcSlash    = strrchr(p_szFilename, '\\');
    if (pcSlash != NULL) {
        pcBasename = pcSlash + 1;
    }

    pConcat = (FIL_tdstConcatFile *)hConcatFile;
    if (pConcat != NULL && pConcat->ulNumberOfFile != 0) {
        ulIdx = 0;
        do {
            if (bFound) break;
            pElem = pConcat->d_stConcatFileElement + ulIdx;
            iCmp  = _stricmp(pcBasename, pElem->p_szFileName);
            if (iCmp == 0) {

                FIL_fn_vBuildConcatCompleteFileName(local_104, hConcatFile, ulIdx);
                iCmp = _stricmp(p_szFilename, local_104);
                if (iCmp == 0) {
                    bFound  = 1;
                    pHandle = (FIL_tdstFileInConcatFile *)GLI_pc_fn_p_vAlloc(0x28);
                    pHandle->bUseConcatFile   = 1;
                    pHandle->p_stFile         = pConcat->p_stFile;
                    pHandle->ulFileActualSeek = (unsigned long)pElem->lOffset;
                    pHandle->ulFileEndSeek    = (unsigned long)(pElem->lOffset +
                                                               (long)pElem->ulFileLength);

                    if (pConcat->bUseXorCode == 0 && pConcat->bUseCheckSum == 0) {
                        pHandle->ulFileXorCode  = 0;
                        pHandle->ulFileCheckSum = 0;
                    } else {
                        pHandle->ulFileXorCode  = pElem->ulXorCode;
                        pHandle->ulFileCheckSum = pElem->ulCheckSum;
                    }
                    pHandle->ulCacheCheckSum = 0;

                    uLen = pElem->ulFileLength;
                    if (uLen < 0x40001) {
                        pHandle->p_ucCache  = (unsigned char *)GLI_pc_fn_p_vAlloc(uLen);
                        pHandle->ulCacheSize = uLen;
                    } else {
                        pHandle->p_ucCache  = (unsigned char *)GLI_pc_fn_p_vAlloc(0x40000);
                        pHandle->ulCacheSize = 0x40000;
                    }
                    pHandle->ulCacheSeek = pHandle->ulCacheSize;
                }
            }
            ulIdx++;
        } while (ulIdx < pConcat->ulNumberOfFile);
    }

    if (!bFound) {
        sprintf(local_104, "%s\\%s", p_szPath, p_szFilename);
        uLen  = FIL_fn_ulGetFileSize(local_104);
        pFile = s_fopen(local_104, "rb");
        if (pFile != NULL) {
            pHandle = (FIL_tdstFileInConcatFile *)GLI_pc_fn_p_vAlloc(0x28);
            pHandle->bUseConcatFile   = 0;
            pHandle->p_stFile         = pFile;
            pHandle->ulFileActualSeek = 0;
            pHandle->ulFileEndSeek    = uLen;
            pHandle->ulFileXorCode    = 0;
            pHandle->ulFileCheckSum   = 0;
            pHandle->ulCacheCheckSum  = 0;

            if (0x40000 < uLen) {
                pHandle->p_ucCache  = (unsigned char *)GLI_pc_fn_p_vAlloc(0x40000);
                pHandle->ulCacheSize = 0x40000;
                pHandle->ulCacheSeek = pHandle->ulCacheSize;
                return pHandle;
            }
            pHandle->p_ucCache  = (unsigned char *)GLI_pc_fn_p_vAlloc(uLen);
            pHandle->ulCacheSize = uLen;
            pHandle->ulCacheSeek = uLen;
        }
    }

    return pHandle;
}

/* 0x00492e80 */
unsigned char __cdecl FIL_fn_bSetConcatPathElement(void *hConcatFile,
                                                    unsigned long ulPathNumber,
                                                    char *p_szPathName)
{
    FIL_tdstConcatFile *pConcat;
    unsigned long ulOffset;
    char *pDst;
    unsigned char bResult;

    pConcat = (FIL_tdstConcatFile *)hConcatFile;
    bResult = 0;
    if (ulPathNumber < pConcat->ulNumberOfPath) {
        ulOffset = ulPathNumber * sizeof(char *);
        pDst = (char *)GLI_pc_fn_p_vAlloc(strlen(p_szPathName) + 1);
        *(char **)((char *)pConcat->d_szPathList + ulOffset) = pDst;
        strcpy(*(char **)((char *)pConcat->d_szPathList + ulOffset), p_szPathName);
        bResult = 1;
    }
    return bResult;
}

/* 0x00492ef0 */
unsigned char __cdecl FIL_fn_bSetConcatFileElement(void *hConcatFile,
                                                    unsigned long ulFileNumber,
                                                    char *p_szFileName)
{
    FIL_tdstConcatFile *pConcat;
    unsigned long ulOffset;
    char *pDst;
    unsigned char bResult;

    pConcat = (FIL_tdstConcatFile *)hConcatFile;
    bResult = 0;
    if (ulFileNumber < pConcat->ulNumberOfFile) {
        ulOffset = ulFileNumber * sizeof(FIL_tdstConcatFileElement);
        pDst = (char *)GLI_pc_fn_p_vAlloc(strlen(p_szFileName) + 1);
        ((FIL_tdstConcatFileElement *)((char *)pConcat->d_stConcatFileElement + ulOffset))->p_szFileName = pDst;
        strcpy(((FIL_tdstConcatFileElement *)((char *)pConcat->d_stConcatFileElement + ulOffset))->p_szFileName,
               p_szFileName);
        bResult = 1;
    }
    return bResult;
}

/* 0x00493590 */
void __cdecl FIL_fn_vCloseConcatFile(void **hConcatFile)
{
    if (*hConcatFile != NULL) {
        fclose(((FIL_tdstConcatFile *)*hConcatFile)->p_stFile);
        fn_vDestroyConcatFileHandle(*hConcatFile);
        *hConcatFile = NULL;
    }
}

/* 0x00493170 */
static FILE * __cdecl FIL_OpenConcatFileAndLocateSection(char *param_1, long *param_2)
{
    char *pcAt;
    char *pcDot;
    FILE *pFile;
    long  lOffset;

    char szSection[260];
    char szFilePath[260];
    char szExtension[8];

    *param_2 = 0;

    pcAt = strrchr(param_1, '@');
    if (pcAt == NULL) {

        strcpy(szFilePath, param_1);
        szSection[0] = '\0';

        memcpy(szExtension, FIL_g_szCntExt, 5);
    } else {

        strcpy(szFilePath, pcAt + 1);
        memcpy(szSection, param_1, (size_t)(pcAt - param_1));
        szSection[pcAt - param_1] = '\0';

        pcDot = strrchr(szSection, '.');
        if (pcDot == NULL) {

            memcpy(szExtension, FIL_g_szCntExt, 5);
        } else {

            memset(szExtension, 0, 5);
            strncpy(szExtension, pcDot, 4);
            *pcDot = '\0';
        }
    }

    _strupr(szSection);
    _strupr(szExtension);

    pFile = s_fopen(szFilePath, FIL_g_szModeRb);
    if (pFile != NULL) {
        lOffset = FIL_FUN_00492fc0(pFile, szSection, szExtension);
        *param_2 = lOffset;
        if (lOffset == -1) {

            *param_2 = 0;
            rewind(pFile);
        }
    }
    return pFile;
}

/* 0x00493300 */
void *__cdecl FIL_fn_vOpenConcatFile(char *p_szFilename)
{
    FILE               *pFile;
    long                lSectionBase;
    unsigned long       ulNumberOfPath;
    unsigned long       ulNumberOfFile;
    void               *hConcatFile;
    FIL_tdstConcatFile *pConcat;
    unsigned char       ucComputedCheckSum;
    unsigned long       i;
    long                lNameLen;
    char                szNameBuf[260];
    long                lPathNumber;
    unsigned long       ulXorCode;
    unsigned long       ulCheckSum;
    long                lOffset;
    unsigned long       ulFileLength;

    pFile = FIL_OpenConcatFileAndLocateSection(p_szFilename, &lSectionBase);
    if (pFile == NULL)
        return NULL;

    s_fread(&ulNumberOfPath, 4, 1, pFile);
    s_fread(&ulNumberOfFile, 4, 1, pFile);

    hConcatFile = FIL_fn_hCreateConcatFileHandle(ulNumberOfPath, ulNumberOfFile);
    pConcat     = (FIL_tdstConcatFile *)hConcatFile;

    s_fread(&pConcat->bUseXorCode,   1, 1, pFile);
    s_fread(&pConcat->bUseCheckSum,  1, 1, pFile);
    s_fread(&pConcat->ucPathXorCode, 1, 1, pFile);

    ucComputedCheckSum = 0;
    for (i = 0; i < ulNumberOfPath; i++) {
        long j;
        s_fread(&lNameLen, 4, 1, pFile);
        s_fread(szNameBuf, 1, (size_t)lNameLen, pFile);
        szNameBuf[lNameLen] = '\0';
        if (pConcat->bUseXorCode) {
            for (j = 0; j < lNameLen; j++)
                ((unsigned char *)szNameBuf)[j] ^= pConcat->ucPathXorCode;
        }
        if (pConcat->bUseCheckSum) {
            for (j = 0; j < lNameLen; j++)
                ucComputedCheckSum += ((unsigned char *)szNameBuf)[j];
        }
        FIL_fn_bSetConcatPathElement(hConcatFile, i, szNameBuf);
    }

    s_fread(&pConcat->ucPathCheckSum, 1, 1, pFile);

    if (pConcat->ucPathCheckSum != ucComputedCheckSum) {
        ERM_fn_vClearLastError(0);
        ERM_fn_vUpdateLastError(6, FIL_g_ucFILModuleId, 0, 4,
                                (long)-1, 0xFF, 0xFF, p_szFilename);
    }

    for (i = 0; i < ulNumberOfFile; i++) {
        FIL_tdstConcatFileElement *pElem = &pConcat->d_stConcatFileElement[i];

        s_fread(&lPathNumber, 4, 1, pFile);

        s_fread(&lNameLen, 4, 1, pFile);
        s_fread(szNameBuf, 1, (size_t)lNameLen, pFile);
        szNameBuf[lNameLen] = '\0';
        if (pConcat->bUseXorCode) {
            long j;
            for (j = 0; j < lNameLen; j++)
                ((unsigned char *)szNameBuf)[j] ^= pConcat->ucPathXorCode;
        }
        FIL_fn_bSetConcatFileElement(hConcatFile, i, szNameBuf);

        s_fread(&ulXorCode,    4, 1, pFile);
        s_fread(&ulCheckSum,   4, 1, pFile);
        s_fread(&lOffset,      4, 1, pFile);
        s_fread(&ulFileLength, 4, 1, pFile);

        pElem->lPathNumber  = lPathNumber;
        pElem->ulXorCode    = ulXorCode;
        pElem->ulCheckSum   = ulCheckSum;
        pElem->lOffset      = lOffset + lSectionBase;
        pElem->ulFileLength = ulFileLength;
    }

    pConcat->p_stFile = pFile;

    return hConcatFile;
}

/* 0x00493cc0 */
static unsigned char __cdecl FIL_fn_bDestroyFileOrDirectory_impl(char *param_1)
{
    long              _FindHandle;
    int               iVar4;
    FIL_tdstFindData32 finddata;
    char             *pcStar;
    char             *pcSlash;
    unsigned char     bResult;
    char              local_21c[260];

    bResult = 1;
    _FindHandle = s_findfirst(param_1, &finddata);
    if (_FindHandle != -1) {

    if ((finddata.attrib & 0x10) != 0) {

        if (strcmp(finddata.name, ".") != 0 &&
            strcmp(finddata.name, "..") != 0) {

            strcpy(local_21c, param_1);
            pcStar = strrchr(local_21c, '*');
            if (pcStar != NULL) {
                pcSlash = strrchr(local_21c, '/');

                if (pcSlash != NULL)
                    *pcSlash = '\0';
            }

            strcat(local_21c, "/*");
            FIL_fn_bDestroyFileOrDirectory_impl(local_21c);
            _rmdir(param_1);
        }
    } else {

        _unlink(param_1);
    }

    iVar4 = s_findnext(_FindHandle, &finddata);
    while (iVar4 != -1) {

        if ((finddata.attrib & 0x10) == 0) {

            strcpy(local_21c, param_1);
            pcStar = strrchr(local_21c, '*');
            if (pcStar != NULL) {
                pcSlash = strrchr(local_21c, '/');

                if (pcSlash != NULL)
                    *pcSlash = '\0';
            }

            strcat(local_21c, "/");
            strcat(local_21c, finddata.name);
            _unlink(local_21c);
        } else {

            if (strcmp(finddata.name, ".") != 0 &&
                strcmp(finddata.name, "..") != 0) {
                strcpy(local_21c, param_1);
                pcStar = strrchr(local_21c, '*');
                if (pcStar != NULL) {
                    pcSlash = strrchr(local_21c, '/');
                    if (pcSlash != NULL)
                        *pcSlash = '\0';
                }
                strcat(local_21c, "/");
                strcat(local_21c, finddata.name);
                FIL_fn_bDestroyFileOrDirectory_impl(local_21c);
            }
        }

        iVar4 = s_findnext(_FindHandle, &finddata);
    }
    s_findclose(_FindHandle);
    }

    return bResult;
}

/* 0x004928f0 */
unsigned char __cdecl FIL_fn_bDestroyFileOrDirectory(char *_szFileName)
{
    return FIL_fn_bDestroyFileOrDirectory_impl(_szFileName);
}

char FIL_g_aaszPathRegistry[FIL_PATH_BUFFER_COUNT][FIL_PATH_BUFFER_SIZE];

/* 0x00492650 */
void __cdecl FIL_pc_fn_vSetSoundDir(char *param_1)
{
    strcpy(FIL_PATH_BUFFER(1), param_1);
}

/* 0x00492680 */
char *__cdecl FIL_pc_fn_szGetSoundDir(void)
{
    return FIL_PATH_BUFFER(1);
}

/* 0x00492690 */
void __cdecl FIL_pc_fn_vSetDataDir(char *param_1)
{
    strcpy(FIL_PATH_BUFFER(0), param_1);
}

/* 0x004926c0 */
char *__cdecl FIL_pc_fn_szGetDataDir(void)
{
    return FIL_PATH_BUFFER(0);
}

/* 0x004926d0 */
void  __cdecl FUN_004926d0(char *param_1) { strcpy(FIL_PATH_BUFFER(2), param_1); }
char *__cdecl FUN_00492700(void)          { return FIL_PATH_BUFFER(2); }

/* 0x00492710 */
void  __cdecl FUN_00492710(char *param_1) { strcpy(FIL_PATH_BUFFER(3), param_1); }

/* 0x00492740 */
void  __cdecl FUN_00492740(char *param_1) { strcpy(FIL_PATH_BUFFER(4), param_1); }
char *__cdecl FUN_00492770(void)          { return FIL_PATH_BUFFER(4); }

/* 0x00492780 */
void  __cdecl FUN_00492780(char *param_1) { strcpy(FIL_PATH_BUFFER(5), param_1); }

/* 0x004927b0 */
void  __cdecl FUN_004927b0(char *param_1) { strcpy(FIL_PATH_BUFFER(6), param_1); }

/* 0x004927e0 */
void  __cdecl FUN_004927e0(char *param_1) { strcpy(FIL_PATH_BUFFER(8), param_1); }
char *__cdecl FUN_00492810(void)          { return FIL_PATH_BUFFER(8); }

/* 0x00492820 */
void  __cdecl FUN_00492820(char *param_1) { strcpy(FIL_PATH_BUFFER(9), param_1); }
char *__cdecl FUN_00492850(void)          { return FIL_PATH_BUFFER(9); }

/* 0x004928c0 */
char *__cdecl FUN_004928c0(void)          { return FIL_PATH_BUFFER(10); }
