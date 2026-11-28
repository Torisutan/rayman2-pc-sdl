#include <stddef.h>
#include <stdint.h>
#include <math.h>
#include <stdio.h>
#include <string.h>
#include <windows.h>

#include "MMG/SNA_reloc.h"
#include "MMG/SNA.h"
#include "MMG/SNA_pc.h"
#include "MMG/MMG_pubaccess.h"
#include "MMG/MMG_globals.h"
#include "GAM/GAM_types.h"

extern unsigned int __cdecl GLI_AddToProgressBar(int lIncrement);
extern char        *__cdecl FUN_00492770(void);
extern void         __cdecl GLI_pc_fn_vFree(void **ppUser);   /* 0x45DCE0 */

extern int          __cdecl Erm_fn_iMessageBox(char *_szText, char *_szTitle, unsigned int _uiStyle);  /* 0x004028F0 */
extern void         __cdecl GLI_FatalExitOrSleep(void);                                                 /* 0x004011C0 */

extern unsigned char g_ucGameModuleId;   /* 0x0049F630 */
extern unsigned char g_ucGEOModuleId;    /* 0x0049FBBC */
extern unsigned char g_ucAIModuleId;     /* 0x004A417C */
extern unsigned char g_ucIPTModuleId;    /* 0x004A0CFC */
extern unsigned char g_ucGliModuleId;    /* 0x004A3FD4 */
extern unsigned char g_ucSndModuleId;    /* 0x004A22DC */
unsigned char g_ucFONModuleId = 0xFF;   /* 0x0049F6AC */
static SNA_tdstLoadListEntry SNA_gs_aLoadList[10] = {
    { &g_ucGameModuleId,          1 },  /* 0x4A2028 */
    { &g_ucGEOModuleId,           2 },
    { &g_ucAIModuleId,            1 },
    { &g_ucGameModuleId,          0 },  /* 0x4A2040 */
    { &g_ucGEOModuleId,           1 },
    { &g_ucIPTModuleId,           0 },
    { &g_ucAIModuleId,            0 },
    { &g_ucFONModuleId,           0 },
    { &g_ucGliModuleId,           0 },  /* 0x4A2068 */
    { &g_ucSndModuleId,           0 },
};
#define SNA_C_pLoadList0        (&SNA_gs_aLoadList[0])
#define SNA_C_pLoadList1        (&SNA_gs_aLoadList[3])
#define SNA_C_pLoadList2        (&SNA_gs_aLoadList[8])

#define SNA_p_vFreeAndNull(p)   GLI_pc_fn_vFree((void **)(p))

#define SNA_g_ucErrorMode       SNA_g_lProtectionGate

/* 0x0045DC60 */
extern void *__cdecl GLI_pc_fn_p_vAlloc(uint32_t ulSize);
#define SNA_p_pAlloc              GLI_pc_fn_p_vAlloc

#include "GAM/GAM_globals.h"
#define SNA_g_cCdDriveLetter    g_cCDROM

static const char SNA_C_szFmtFix_storage[]   = "%s\\Fix";
static const char SNA_C_szFmtLevel_storage[] = "%s\\%s\\%s";
static const char SNA_C_szFmtCd_storage[]    = "%c\\\\%s";
static const char SNA_C_szInsertCd_storage[] = "Please insert the CD";
static const char SNA_C_szExtRtb_storage[]   = ".rtb";
static const char SNA_C_szExtRtp_storage[]   = ".rtp";
static const char SNA_C_szExtRts_storage[]   = ".rts";
static const char SNA_C_szExtRtt_storage[]   = ".rtt";
static const char SNA_C_szExtRtl_storage[]   = ".rtl";
#define SNA_C_szFmtFix          (SNA_C_szFmtFix_storage)
#define SNA_C_szFmtLevel        (SNA_C_szFmtLevel_storage)
#define SNA_C_szFmtCd           (SNA_C_szFmtCd_storage)
#define SNA_C_szInsertCd        (SNA_C_szInsertCd_storage)
#define SNA_C_szExtRtb          (SNA_C_szExtRtb_storage)
#define SNA_C_szExtRtp          (SNA_C_szExtRtp_storage)
#define SNA_C_szExtRts          (SNA_C_szExtRts_storage)
#define SNA_C_szExtRtt          (SNA_C_szExtRtt_storage)
#define SNA_C_szExtRtl          (SNA_C_szExtRtl_storage)

#define SNA_p_pcGetBaseDir      FUN_00492770
/* 0x00418960 */
extern char __cdecl FUN_00418960(void);
#define SNA_p_bEmbeddedGate     FUN_00418960
unsigned char __cdecl SNA_pc_bOpenEmbeddedSna(void *_p_stFile);   /* 0x0044E8A0 */
#define SNA_p_iAskInsertCd(t, c, s) Erm_fn_iMessageBox((char *)(t), (char *)(c), (unsigned int)(s))
#define SNA_p_vOnCdPromptAbort      GLI_FatalExitOrSleep

#define SNA_C_lRelocTableSize   2560

SNA_pc_tdstGptTable *SNA_g_pGptContext;   /* 0x004B7358 */
/* 0x5121D6 */
unsigned char  SNA_g_ucLoadListMode;
unsigned char  SNA_pc_g_a_ucLoadKey[4];   /* 0x005116F0 */
unsigned long  SNA_g_ulTableIndex;         /* 0x005117C4 */
unsigned long  SNA_pc_g_a_ulTableState[SNA_C_lMaxLoadedTables];   /* 0x00511700 */
unsigned long  SNA_pc_g_a_ulTableKey[SNA_C_lMaxLoadedTables];     /* 0x00511760 */
unsigned char SNA_C_pRelocFlag_storage[SNA_C_lRelocTableSize];
int           SNA_C_pRelocDelta_storage[SNA_C_lRelocTableSize];
#define SNA_C_pRelocFlag        (SNA_C_pRelocFlag_storage)
#define SNA_C_pRelocDelta       (SNA_C_pRelocDelta_storage)

#include "GLI/ErrGLI.h"
#define SNA_g_ucRelocNull       g_ucGliModuleId

#define SNA_p_vGiveInformationBlock  Mmg_fn_vGiveInformationBlock
#define SNA_p_vSaveInformationBlock  Mmg_fn_vSaveInformationBlock
#define SNA_p_vProgress              GLI_AddToProgressBar

void __cdecl SNA_pc_vSortLoadList(SNA_tdstLoadListEntry *_p_stList, int _iCount);

void __cdecl SNA_pc_vInitLoadLists(void);

unsigned char __cdecl SNA_pc_bOpenSnaFamilyFile(void *_p_stFile);

SNA_pc_tdstGptTable *__cdecl SNA_pc_pReadGptTable(void);

void __cdecl SNA_pc_vFreeGptTable(SNA_pc_tdstGptTable *_p_stGpt);

/* 0x0044CF10 */
void __cdecl SNA_pc_vReloadGptTable(void)
{
    if (SNA_g_pGptContext != NULL) {
        SNA_pc_vFreeGptTable(SNA_g_pGptContext);
    }
    SNA_g_pGptContext = NULL;
    SNA_g_pGptContext = SNA_pc_pReadGptTable();

    SNA_g_pRelocCursor    = SNA_g_pGptContext->d_stRecord[0].d_stSlot;
    SNA_g_pRelocCursorEnd = SNA_g_pGptContext->d_stRecord[0].d_stSlot
                          + SNA_g_pGptContext->d_stRecord[0].ulNbSlots;
}

/* 0x0044CF60 */
void __cdecl SNA_pc_vReleaseGptTable(void)
{
    if (SNA_g_pGptContext != NULL) {
        SNA_pc_vFreeGptTable(SNA_g_pGptContext);
    }
    SNA_g_pGptContext = NULL;
    SNA_g_pRelocCursorEnd = NULL;
    SNA_g_pRelocCursor    = NULL;
}

unsigned long __cdecl SNA_pc_bIsBlockInLoadList(unsigned char _ucModuleId, char _cBlockId);

/* 0x0044D4F0 */
int __cdecl SNA_pc_vLoadAndRelocate(char *_szFile)
{
    unsigned char  a_ucStream[0x40];
    unsigned char  a_ucBlockInfo[0x40];
    char           a_cTemp[256];
    unsigned char  ucOk;
    unsigned long  ulMainHeader = 0;
    unsigned long  ulRead = 0;
    int            iReadOk;
    unsigned long  ulModule;
    unsigned long  ulBlock;
    unsigned long  ulThirdByte;
    unsigned long  ulOldBase;
    unsigned long  ulUnk2;
    unsigned long  ulUnk3;
    unsigned long  ulMaxPos;
    unsigned long  ulSize;
    int            iIdx;

    SNA_pc_vInitLoadLists();

    ucOk = SNA_pc_bOpenFile(_szFile, 2, 3, a_ucStream);
    if (ucOk == 0) {
        strcpy(a_cTemp, _szFile);
        sprintf(_szFile, SNA_C_szFmtCd, (int)SNA_g_cCdDriveLetter, a_cTemp);
        ucOk = SNA_pc_bOpenFile(_szFile, 2, 3, a_ucStream);
        while (ucOk == 0) {
            int iResponse = SNA_p_iAskInsertCd(SNA_C_szInsertCd,
                                               SNA_C_szInsertCd, 0x40035);
            if (iResponse == 2) {
                SNA_p_vOnCdPromptAbort();
            }
            ucOk = SNA_pc_bOpenFile(_szFile, 2, 3, a_ucStream);
        }
        strcpy(_szFile, a_cTemp);
        if (ucOk == 0) {
            return 0;
        }
    }

    iReadOk = SNA_pc_M_bReadFile(((SNA_pc_tdstStream *)a_ucStream)->hFile, &ulMainHeader, 4, &ulRead);

    if (SNA_g_pGptContext != NULL) {
        SNA_pc_vFreeGptTable(SNA_g_pGptContext);
    }
    SNA_g_pGptContext = NULL;
    SNA_g_pGptContext = SNA_pc_pReadGptTable();
    SNA_p_vProgress(1);

    while (iReadOk != 0) {
        if (SNA_fn_ulFRead(&ulModule, 1, 1, a_ucStream) == 0) {
            break;
        }
        SNA_fn_ulFRead(&ulBlock, 1, 1, a_ucStream);
        SNA_fn_ulFRead(&ulThirdByte, 1, 1, a_ucStream);
        SNA_p_vGiveInformationBlock(ulModule, ulBlock, (tdstBlockInfo *)a_ucBlockInfo);
        SNA_C_pRelocFlag[(ulModule & 0xFF) * 10 + (ulBlock & 0xFF)] = 0;
        SNA_fn_ulFRead(&ulOldBase, 4, 1, a_ucStream);

        iIdx = (ulBlock & 0xFF) + (ulModule & 0xFF) * 10;
        if ((unsigned long)((tdstBlockInfo *)a_ucBlockInfo)->p_cBeginBlock == ulOldBase) {
            SNA_C_pRelocDelta[iIdx] = 0;
        } else {
            SNA_C_pRelocFlag[iIdx] = 1;
            SNA_C_pRelocDelta[iIdx] =
                (int)((tdstBlockInfo *)a_ucBlockInfo)->p_cBeginBlock - (int)ulOldBase;
        }

        if (ulOldBase != 0xFFFFFFFF) {

            SNA_fn_ulFRead(&ulUnk2, 4, 1, a_ucStream);
            SNA_fn_ulFRead(&ulUnk3, 4, 1, a_ucStream);
            SNA_fn_ulFRead(&ulMaxPos, 4, 1, a_ucStream);
            SNA_fn_ulFRead(&ulSize, 4, 1, a_ucStream);

            if (SNA_pc_bIsBlockInLoadList((unsigned char)ulModule,
                                          (char)ulBlock) == 0) {
                if (ulSize != 0) {
                    SNA_pc_ulSeek(a_ucStream, (long)ulSize, 1);
                }
            } else if (ulSize != 0) {

                ((tdstBlockInfo *)a_ucBlockInfo)->p_cMaxMem =
                    (char *)ulMaxPos + SNA_C_pRelocDelta[iIdx];
                if (ulUnk3 != 0xFFFFFFFF) {
                    ((tdstBlockInfo *)a_ucBlockInfo)->p_cFirstFree =
                        (char *)ulUnk3 + SNA_C_pRelocDelta[iIdx];
                }

                SNA_fn_ulFRead(((tdstBlockInfo *)a_ucBlockInfo)->p_cBeginBlock, 1, ulSize, a_ucStream);
                SNA_p_vSaveInformationBlock(ulModule, ulBlock, (tdstBlockInfo *)a_ucBlockInfo);
                SNA_p_vProgress(1);

                {
                    unsigned int   uCount = SNA_g_pGptContext->ucNbRecords;
                    unsigned int   uRec = 0;

                    if (uCount != 0) {
                        SNA_pc_tdstGptRecord *p_stScan = SNA_g_pGptContext->d_stRecord;
                        do {
                            if (p_stScan->ucModule == (char)ulModule &&
                                p_stScan->ucBlock == (char)ulBlock) {
                                break;
                            }
                            uRec++;
                            p_stScan++;
                        } while (uRec < uCount);
                    }

                    if (uRec < uCount) {
                        if (SNA_g_pGptContext->d_stRecord[uRec].ulNbSlots != 0) {
                            unsigned int uSlot = 0;
                            do {
                                SNA_pc_tdstRelocSlot *p_stSlot =
                                    &SNA_g_pGptContext->d_stRecord[uRec].d_stSlot[uSlot];
                                unsigned char  ucTargetBlock  = p_stSlot->ucBlock;
                                unsigned char  ucTargetModule = p_stSlot->ucModule;
                                int           *p_iPtr;

                                if (SNA_C_pRelocFlag[iIdx] == 0) {
                                    p_iPtr = (int *)p_stSlot->ulValue;
                                } else {
                                    p_iPtr = (int *)(SNA_C_pRelocDelta[iIdx] +
                                                     (int)p_stSlot->ulValue);
                                }

                                if (ucTargetModule == SNA_g_ucRelocNull) {
                                    *p_iPtr = 0;
                                } else if (ucTargetModule != 0xFF) {
                                    *p_iPtr += SNA_C_pRelocDelta[
                                        (unsigned int)ucTargetBlock +
                                        (unsigned int)ucTargetModule * 10];
                                }

                                uSlot++;
                            } while (uSlot < SNA_g_pGptContext->d_stRecord[uRec].ulNbSlots);
                        }
                        SNA_p_vProgress(1);
                    }
                }
            }
        }
    }

    if (SNA_g_pGptContext != NULL) {
        SNA_pc_vFreeGptTable(SNA_g_pGptContext);
    }
    SNA_g_pGptContext = NULL;

#ifndef RAYMAN2_NO_PROTECTION   /* checksum mismatch: spoil the recorded table state the protection actors check */
    if (((SNA_pc_tdstStream *)a_ucStream)->ulChecksum != SNA_g_ulGptHeaderScratch && SNA_p_bEmbeddedGate())
        SNA_pc_g_a_ulTableState[SNA_g_ulTableIndex - 1] = ~SNA_pc_g_a_ulTableState[SNA_g_ulTableIndex - 1];
#endif /* RAYMAN2_NO_PROTECTION */

    SNA_pc_bCloseFile(a_ucStream);
    return 1;
}

/* 0x0044D990 */
unsigned char __cdecl SNA_pc_bOpenSnaFamilyFile(void *_p_stFile)
{
    char        a_cPath[256];
    char        a_cTemp[256];
    char       *p_cName;
    char       *p_cDollar;
    const char *p_cExt;
    unsigned char ucOk;

#ifndef RAYMAN2_NO_PROTECTION   /* protection gate */
    if (SNA_g_ucErrorMode != 0) {
        __asm { mov dword ptr [SNA_g_pOpenErrorFrame1], offset L_body }
        __asm { mov dword ptr [SNA_g_pOpenErrorFrame2], offset L_epi }
        return 1;
    }
L_body:
#endif /* RAYMAN2_NO_PROTECTION */
    if (SNA_g_ucNextRelocationTableToLoad == 0xFFu) {
        sprintf(a_cPath, SNA_C_szFmtFix, SNA_p_pcGetBaseDir());
    } else {
        if (SNA_p_bEmbeddedGate() && SNA_pc_bOpenEmbeddedSna(_p_stFile)) {
            return 1;
        }
        p_cName   = GAM_g_stEngineStructure.a_szLevelName[SNA_g_ucNextRelocationTableToLoad];
        p_cDollar = strchr(p_cName, '$');
        if (p_cDollar != (char *)0) {
            *p_cDollar = '\0';
        }
        sprintf(a_cPath, SNA_C_szFmtLevel, SNA_p_pcGetBaseDir(), p_cName, p_cName);
        if (p_cDollar != (char *)0) {
            *p_cDollar = '$';
        }
    }

    switch (SNA_pc_g_ucNextRelocationTableKind) {
    case 0:  p_cExt = SNA_C_szExtRtb; break;
    case 1:  p_cExt = SNA_C_szExtRtp; break;
    case 2:  p_cExt = SNA_C_szExtRts; break;
    case 3:  p_cExt = SNA_C_szExtRtt; break;
    case 4:  p_cExt = SNA_C_szExtRtl; break;
    default: p_cExt = (const char *)0; break;
    }
    if (p_cExt != (const char *)0) {
        strcat(a_cPath, p_cExt);
    }

    ucOk = SNA_pc_bOpenFile(a_cPath, 2, 0, _p_stFile);
    if (ucOk == 0) {
        strcpy(a_cTemp, a_cPath);
        sprintf(a_cPath, SNA_C_szFmtCd, (int)SNA_g_cCdDriveLetter, a_cTemp);
        ucOk = SNA_pc_bOpenFile(a_cPath, 2, 0, _p_stFile);
        while (ucOk == 0) {
            int iResponse = SNA_p_iAskInsertCd(SNA_C_szInsertCd,
                                               SNA_C_szInsertCd, 0x40035);
            if (iResponse == 2) {
                SNA_p_vOnCdPromptAbort();
            }
            ucOk = SNA_pc_bOpenFile(a_cPath, 2, 0, _p_stFile);
        }
        strcpy(a_cPath, a_cTemp);
    }

    return ucOk;
#ifndef RAYMAN2_NO_PROTECTION
L_epi:;
#endif /* RAYMAN2_NO_PROTECTION */
}

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

/* 0x0044DC40 */
SNA_pc_tdstGptTable *__cdecl SNA_pc_pReadGptTable(void)
{
    unsigned char  a_ucStream[0x40];
    SNA_pc_tdstGptTable  *p_stHead;
    SNA_pc_tdstGptRecord *d_stRecords;
    SNA_pc_tdstGptRecord *p_stRec;
    SNA_pc_tdstRelocSlot *d_stSlots;
    unsigned char  ucCount;
    unsigned char  ucModule     = 0xF0;
    unsigned char  ucBlock      = 0xF0;
    unsigned char  ucModulePrev;
    unsigned char  ucBlockPrev;
    unsigned char  ucCounter;
    unsigned char  ucIndex;
    unsigned long  ulPtrCount;

#ifndef RAYMAN2_NO_PROTECTION   /* protection gate */
    if (SNA_g_ucErrorMode != 0) {
        __asm { mov dword ptr [SNA_g_pGptErrorFrame1], offset L_body }
        __asm { mov dword ptr [SNA_g_pGptErrorFrame2], offset L_epi }
        return NULL;
    }
L_body:
#endif /* RAYMAN2_NO_PROTECTION */
    SNA_pc_vInitLoadLists();
    if (SNA_pc_bOpenSnaFamilyFile(a_ucStream) == 0) {
        return NULL;
    }

    SNA_fn_ulFRead(&ucCount, 1, 1, a_ucStream);

    p_stHead              = (SNA_pc_tdstGptTable *)SNA_p_pAlloc(sizeof(SNA_pc_tdstGptTable));
    p_stHead->ucNbRecords = ucCount;
    d_stRecords = (SNA_pc_tdstGptRecord *)SNA_p_pAlloc((ucCount & 0xFFu) * sizeof(SNA_pc_tdstGptRecord));
    p_stHead->d_stRecord  = d_stRecords;

    SNA_fn_ulFRead(&SNA_g_ulGptHeaderScratch, 4, 1, a_ucStream);

    ucCounter = 0;
    ucIndex   = 0;
    if (ucCount != 0) {
        do {
            ucModulePrev = ucModule;
            ucBlockPrev  = ucBlock;
            SNA_fn_ulFRead(&ucModule, 1, 1, a_ucStream);
            SNA_fn_ulFRead(&ucBlock, 1, 1, a_ucStream);
            if (ucModule == ucModulePrev && ucBlock == ucBlockPrev) {
                p_stHead->ucNbRecords -= 1;
                break;
            }
            SNA_fn_ulFRead(&ulPtrCount, 4, 1, a_ucStream);
            if (ulPtrCount == 0) {
                d_stSlots = NULL;
            } else {
                d_stSlots = (SNA_pc_tdstRelocSlot *)SNA_p_pAlloc(ulPtrCount * sizeof(SNA_pc_tdstRelocSlot));
                SNA_fn_ulFRead(d_stSlots, ulPtrCount * sizeof(SNA_pc_tdstRelocSlot), 1, a_ucStream);
            }
            p_stRec   = &d_stRecords[ucIndex];
            ucCounter = (unsigned char)(ucCounter + 1);
            ucIndex   = ucCounter;
            p_stRec->ucModule  = ucModule;
            p_stRec->ucBlock   = ucBlock;
            p_stRec->ulNbSlots = ulPtrCount;
            p_stRec->d_stSlot  = d_stSlots;
        } while (ucCounter < ucCount);
    }

    SNA_pc_bCloseFile(a_ucStream);
    return p_stHead;
#ifndef RAYMAN2_NO_PROTECTION
L_epi:;
#endif /* RAYMAN2_NO_PROTECTION */
}

/* 0x0044DDE0 */
void __cdecl SNA_pc_vFreeGptTable(SNA_pc_tdstGptTable *_p_stGpt)
{
    SNA_pc_tdstGptRecord *d_stRecArray;
    unsigned char ucI;

    if (_p_stGpt != NULL) {
        d_stRecArray = _p_stGpt->d_stRecord;
        ucI      = 0;
        if (_p_stGpt->ucNbRecords > 0) {
            do {

                SNA_pc_tdstRelocSlot **p_d_stSlots = &d_stRecArray[ucI].d_stSlot;
                if (*p_d_stSlots != NULL) {
                    SNA_p_vFreeAndNull(p_d_stSlots);
                }
                ucI++;
            } while (ucI < _p_stGpt->ucNbRecords);
        }
        SNA_p_vFreeAndNull(&d_stRecArray);
        SNA_p_vFreeAndNull(&_p_stGpt);
    }
}

SNA_pc_tdhFile SNA_g_hGptFile;   /* 0x005149BC */
FILE  *SNA_g_pSndFile;   /* 0x005149B8 */

static const char SNA_C_szModeRb[] = "rb";   /* 0x0049F75C */

extern void *__stdcall SND_fn_pGetBinEvent(unsigned long ulBinId);
#define SNA_p_pGetBinEvent(ulId) ((int)SND_fn_pGetBinEvent((unsigned long)(ulId)))

/* 0x0044CFA0 */
void __cdecl SNA_pc_vRawReadGptFile(void *_p_vBuffer, unsigned long _dwBytes)
{
    SNA_pc_M_bReadFile(SNA_g_hGptFile, _p_vBuffer, _dwBytes, &_dwBytes);
}

/* 0x0044CFC0 */
unsigned long __cdecl SNA_pc_ulReadAndRelocateLong(void)
{
    unsigned long ulValue;
    unsigned long ulResult;
    unsigned long ulModule;
    unsigned long ulId;
    int           iIdx;

    SNA_pc_vRawReadGptFile(&ulValue, 4);
    ulResult = ulValue;
    if (SNA_g_pRelocCursor < SNA_g_pRelocCursorEnd
            && ulValue == SNA_g_pRelocCursor->ulValue) {
        ulModule = (unsigned long)SNA_g_pRelocCursor->ucModule;
        ulId     = (unsigned long)SNA_g_pRelocCursor->ucBlock;
        iIdx     = (int)ulId + (int)ulModule * 10;
        if (SNA_C_pRelocFlag[iIdx] != 0) {
            ulResult = ulResult + (unsigned long)SNA_C_pRelocDelta[iIdx];
        }
        SNA_g_pRelocCursor = SNA_g_pRelocCursor + 1;
    }
    return ulResult;
}

/* 0x0044D040 */
void __cdecl SNA_pc_vRelocateGptSlot(unsigned long *_p_ulSlot)
{
    unsigned char ucModule;
    unsigned char ucBlock;
    int           iIdx;

    if (SNA_g_pRelocCursor < SNA_g_pRelocCursorEnd
            && *_p_ulSlot == SNA_g_pRelocCursor->ulValue) {
        ucModule = SNA_g_pRelocCursor->ucModule;
        ucBlock  = SNA_g_pRelocCursor->ucBlock;
        iIdx     = (int)((unsigned long)ucBlock + (unsigned long)ucModule * 10);
        if (SNA_C_pRelocFlag[iIdx] != 0) {
            *_p_ulSlot = *_p_ulSlot + (unsigned long)SNA_C_pRelocDelta[iIdx];
        }
        SNA_g_pRelocCursor = SNA_g_pRelocCursor + 1;
    }
}

/* 0x0044D0B0 */
void __cdecl SNA_pc_vReadAndRelocateBlock(int *_p_iBuffer, unsigned int _uiBytes)
{
    int          *piEnd;
    int           iIdx;
    SNA_pc_tdstRelocSlot *p_stCursor;
    unsigned int  uiModule;
    int          *piId;
    SNA_pc_tdstRelocSlot *p_stScan;
    int          *piWalk;
    unsigned int  uiRem;

    SNA_pc_vRawReadGptFile(_p_iBuffer, _uiBytes);
    uiRem = _uiBytes & 3;
    if (uiRem != 0) {
        SNA_pc_vRawReadGptFile(&_uiBytes, 4 - uiRem);
    }
    piEnd    = (int *)((int)_p_iBuffer + (_uiBytes & 0xfffffffc));
    p_stScan   = SNA_g_pRelocCursor;
    p_stCursor = SNA_g_pRelocCursor;
    for (piWalk = _p_iBuffer; piWalk < piEnd; piWalk = piWalk + 1) {
        SNA_g_pRelocCursor = p_stCursor;
        if (p_stScan < SNA_g_pRelocCursorEnd && (unsigned long)*piWalk == p_stScan->ulValue) {
            uiModule = (unsigned int)p_stScan->ucModule;
            piId     = (int *)(unsigned int)p_stScan->ucBlock;
            iIdx     = (int)piId + uiModule * 10;
            SNA_g_pRelocCursor = p_stScan;
            if (SNA_C_pRelocFlag[iIdx] != 0) {
                SNA_g_pRelocCursor = p_stCursor;
                *piWalk = SNA_C_pRelocDelta[iIdx] + *piWalk;
            }
            p_stScan = SNA_g_pRelocCursor + 1;
            SNA_g_pRelocCursor = p_stScan;
        }
        p_stCursor = SNA_g_pRelocCursor;
    }
    SNA_g_pRelocCursor = p_stCursor;
}

/* 0x0044D160 */
void __cdecl SNA_pc_vReadAndRelocateArray(int *_p_iBuffer, int _iCount,
                                          unsigned int _uiElemSize)
{
    if (_uiElemSize != 0) {
        for (; _iCount != 0; _iCount = _iCount - 1) {
            SNA_pc_vReadAndRelocateBlock(_p_iBuffer, _uiElemSize);
            _p_iBuffer = (int *)((int)_p_iBuffer + _uiElemSize);
        }
    }
}

/* 0x0044D190 */
void __cdecl SNA_pc_vReadRawArray(void *_p_vBuffer, int _iCount,
                                  unsigned int _uiElemSize)
{
    unsigned int  uiTotal = _uiElemSize * (unsigned int)_iCount;
    if (uiTotal != 0) {
        unsigned long ulPadScratch;
        DWORD         dwPad        = 0;
        ((unsigned short *)&ulPadScratch)[0] = 0;
        ((unsigned char *)&ulPadScratch)[2]  = 0;
        if ((uiTotal & 3) != 0) {
            dwPad = 4 - (uiTotal & 3);
        }
        SNA_pc_vRawReadGptFile(_p_vBuffer, uiTotal);
        SNA_pc_vRawReadGptFile(&ulPadScratch, dwPad);
    }
}

/* 0x0044CD60 */
void __cdecl SNA_pc_vOpenGptFile(char *_szPath)
{
    char a_cTemp[256];

    SNA_pc_vInitLoadLists();
    SNA_g_hGptFile = SNA_pc_M_hOpenFile(_szPath, SNA_pc_C_ulAccessRead, SNA_pc_C_ulOpenExisting,
                                        SNA_pc_C_ulAttributeNormal);
    if (SNA_g_hGptFile == SNA_pc_C_hInvalidFile) {
        strcpy(a_cTemp, _szPath);
        sprintf(_szPath, SNA_C_szFmtCd, (int)SNA_g_cCdDriveLetter, a_cTemp);
        SNA_g_hGptFile = SNA_pc_M_hOpenFile(_szPath, SNA_pc_C_ulAccessRead, SNA_pc_C_ulOpenExisting,
                                            SNA_pc_C_ulAttributeNormal);
        while (SNA_g_hGptFile == SNA_pc_C_hInvalidFile) {
            int iResponse = SNA_p_iAskInsertCd(SNA_C_szInsertCd,
                                               SNA_C_szInsertCd, 0x40035);
            if (iResponse == 2) {
                SNA_p_vOnCdPromptAbort();
            }
            SNA_g_hGptFile = SNA_pc_M_hOpenFile(_szPath, SNA_pc_C_ulAccessRead, SNA_pc_C_ulOpenExisting,
                                                SNA_pc_C_ulAttributeNormal);
        }
        strcpy(_szPath, a_cTemp);
    }

    if (SNA_g_pGptContext != NULL) {
        SNA_pc_vFreeGptTable(SNA_g_pGptContext);
    }
    SNA_g_pGptContext = NULL;
    SNA_g_pGptContext = SNA_pc_pReadGptTable();

    SNA_g_pRelocCursor    = SNA_g_pGptContext->d_stRecord[0].d_stSlot;
    SNA_g_pRelocCursorEnd = SNA_g_pGptContext->d_stRecord[0].d_stSlot
                          + SNA_g_pGptContext->d_stRecord[0].ulNbSlots;

    SNA_p_vProgress(1);
}

/* 0x0044CEC0 */
void __cdecl SNA_pc_vCloseGptFile(void)
{
    if (SNA_g_pGptContext != NULL) {
        SNA_pc_vFreeGptTable(SNA_g_pGptContext);
    }
    SNA_g_pGptContext = NULL;
    SNA_pc_M_bCloseFile(SNA_g_hGptFile);
    SNA_p_vProgress(1);
    SNA_g_pRelocCursorEnd = NULL;
    SNA_g_pRelocCursor    = NULL;
}

/* 0x0044D1E0 */
void __cdecl SNA_pc_vOpenSndFile(char *_szPath)
{
    char a_cTemp[256];

    SNA_pc_vInitLoadLists();
    SNA_g_pSndFile = fopen(_szPath, SNA_C_szModeRb);
    if (SNA_g_pSndFile == (FILE *)0) {
        strcpy(a_cTemp, _szPath);
        sprintf(_szPath, SNA_C_szFmtCd, (int)SNA_g_cCdDriveLetter, a_cTemp);
        SNA_g_pSndFile = fopen(_szPath, SNA_C_szModeRb);
        while (SNA_g_pSndFile == (FILE *)0) {
            int iResponse = SNA_p_iAskInsertCd(SNA_C_szInsertCd,
                                               SNA_C_szInsertCd, 0x40035);
            if (iResponse == 2) {
                SNA_p_vOnCdPromptAbort();
            }
            SNA_g_pSndFile = fopen(_szPath, SNA_C_szModeRb);
        }
        strcpy(_szPath, a_cTemp);
    }

    if (SNA_g_pGptContext != NULL) {
        SNA_pc_vFreeGptTable(SNA_g_pGptContext);
    }
    SNA_g_pGptContext = NULL;
    SNA_g_pGptContext = SNA_pc_pReadGptTable();
}

/* 0x0044D2F0 */
void __cdecl SNA_pc_vReadAndRelocateSnd(char *_szPath)
{
    SNA_pc_tdstRelocSlot *p_stSlot;
    int            iEvent;
    int           *p_iPtr;
    unsigned long  ulId;

    p_stSlot = SNA_g_pGptContext->d_stRecord[0].d_stSlot;

    while (!feof(SNA_g_pSndFile)) {
        if (fread(&ulId, 4, 1, SNA_g_pSndFile) == 0
            || fread(&p_iPtr, 4, 1, SNA_g_pSndFile) == 0)
            break;
        iEvent = SNA_p_pGetBinEvent(ulId);
        p_iPtr = (int *)((int)p_iPtr
                 + SNA_C_pRelocDelta[(unsigned int)p_stSlot->ucBlock
                                     + (unsigned int)p_stSlot->ucModule * 10]);
        p_stSlot = p_stSlot + 1;
        *p_iPtr = iEvent;
    }
    fclose(SNA_g_pSndFile);
}

/* 0x0044D3A0 */
void __cdecl SNA_pc_vSortLoadList(SNA_tdstLoadListEntry *_p_stList, int _iCount)
{
    SNA_tdstLoadListEntry *p_stLast = _p_stList + _iCount - 1;
    SNA_tdstLoadListEntry *p_stI;
    SNA_tdstLoadListEntry *p_stJ;

    for (p_stI = _p_stList; p_stI < p_stLast; p_stI++) {
        for (p_stJ = p_stI + 1; p_stJ < p_stLast + 1; p_stJ++) {
            if (*p_stI->p_ucModuleId > *p_stJ->p_ucModuleId ||
                (*p_stI->p_ucModuleId == *p_stJ->p_ucModuleId &&
                 (unsigned char)p_stI->lBlockId > (unsigned char)p_stJ->lBlockId)) {
                SNA_tdstLoadListEntry stTmp = *p_stI;
                *p_stI = *p_stJ;
                *p_stJ = stTmp;
            }
        }
    }
}

/* 0x0044D430 */
unsigned long __cdecl SNA_pc_bIsBlockInLoadList(unsigned char _ucModuleId,
                                                char _cBlockId)
{
    SNA_tdstLoadListEntry *p_stBegin;
    SNA_tdstLoadListEntry *p_stEnd;

    if (SNA_g_ucLoadListMode != 0) {
        p_stBegin = SNA_C_pLoadList1;
        p_stEnd   = SNA_C_pLoadList2;
    } else {
        p_stBegin = SNA_C_pLoadList0;
        p_stEnd   = SNA_C_pLoadList1;
    }

    for (; p_stBegin < p_stEnd && _ucModuleId > *p_stBegin->p_ucModuleId; p_stBegin++) {
    }
    if (p_stBegin < p_stEnd) {
        do {
            if (_ucModuleId != *p_stBegin->p_ucModuleId) {
                return 0;
            }
            if (_cBlockId == (char)p_stBegin->lBlockId) {
                return 1;
            }
            p_stBegin++;
        } while (p_stBegin < p_stEnd);
    }
    return 0;
}

/* 0x0044D4A0 */
void __cdecl SNA_pc_vInitLoadLists(void)
{
    if (SNA_g_bLoadListsSorted == 0) {
        SNA_pc_vSortLoadList(SNA_C_pLoadList1, 5);
        SNA_pc_vSortLoadList(SNA_C_pLoadList0, 3);
        SNA_pc_vSortLoadList(SNA_C_pLoadList2, 2);
        SNA_g_bLoadListsSorted = 1;
    }
}

extern unsigned int __cdecl MMG_pc_fn_ulStepLcgState(void);
extern long __stdcall MMG_pc_fn_bFlushInstructionCache(void *hProcess, const void *p_vBase, unsigned long ulSize);
extern long __stdcall MMG_pc_fn_bCloseHandle(void *hObject);

#define SNA_pc_M_vFlushA() \
    ((int (__stdcall *)(void *, const void *, unsigned long))MMG_pc_M_ulImportAddress(MMG_pc_C_ImpFlushInstructionCache))( \
        ((void *(__stdcall *)(void))MMG_pc_M_ulImportAddress(MMG_pc_C_ImpGetCurrentProcess))(), \
        &MMG_pc_g_uLcgPool, MMG_pc_C_PoolSize)

#define SNA_pc_M_pStubSlot() \
    (&MMG_pc_g_uLcgPool.a_ucCode[(int)MMG_pc_fn_ulStepLcgState() % MMG_pc_C_PoolSlots])

#define SNA_pc_M_vFlushViaForwarder() \
    MMG_pc_fn_bFlushInstructionCache( \
        ((void *(__stdcall *)(void))MMG_pc_M_ulImportAddress(MMG_pc_C_ImpGetCurrentProcess))(), \
        &MMG_pc_g_uLcgPool, MMG_pc_C_PoolSize)

#define SNA_pc_M_vScrubStub(p_ucStub) \
    { \
        for (p_ucEnd = (p_ucStub) + MMG_pc_C_StubSize; (p_ucStub) < p_ucEnd; (p_ucStub)++) { \
            MMG_pc_M_vStepLcg(); \
            *(p_ucStub) = (unsigned char)((int)MMG_g_ulLCGState % 0xff); \
        } \
    }

#define SNA_pc_M_vStubReadA(lpBuf, cbBuf) \
    { \
        p_ucStub = SNA_pc_M_pStubSlot(); \
        MMG_pc_M_vWriteStub(p_ucStub, MMG_pc_C_ImpReadFile, 0x50, 0xC3); \
        SNA_pc_M_vFlushA(); \
        ((BOOL (__stdcall *)(HANDLE, LPVOID, DWORD, LPDWORD, LPOVERLAPPED))p_ucStub)( \
            hFile, (lpBuf), (cbBuf), &dwRead, NULL); \
        SNA_pc_M_vScrubStub(p_ucStub); \
    }

#define SNA_pc_M_vStubReadB(lpBuf, cbBuf) \
    { \
        p_ucStub = SNA_pc_M_pStubSlot(); \
        MMG_pc_M_vWriteStub(p_ucStub, MMG_pc_C_ImpReadFile, 0x50, 0xC3); \
        SNA_pc_M_vFlushViaForwarder(); \
        ((BOOL (__stdcall *)(HANDLE, LPVOID, DWORD, LPDWORD, LPOVERLAPPED))p_ucStub)( \
            hFile, (lpBuf), (cbBuf), &dwRead, NULL); \
        SNA_pc_M_vScrubStub(p_ucStub); \
    }

static const char SNA_pc_szFmtCdLevels[] = "%c\\\\%s\\%s";

extern char *SNA_pc_a_pszLevelFile[24];   /* 0x004A2090 */
char *SNA_pc_a_pszLevelFile[24] = {
    "LEVELS0.DAT", "MAP0.DAT", "TEXTURES0.DAT", "ANIMS0.DAT", "SOUNDS0.DAT", "MUSIC0.DAT",
    "GRAPHICS0.DAT", "MENU0.DAT", "LEVELS1.DAT", "MAP1.DAT", "TEXTURES1.DAT", "ANIMS1.DAT",
    "SOUNDS1.DAT", "MUSIC1.DAT", "GRAPHICS1.DAT", "MENU1.DAT", "LEVELS2.DAT", "MAP2.DAT",
    "TEXTURES2.DAT", "ANIMS2.DAT", "SOUNDS2.DAT", "MUSIC2.DAT", "GRAPHICS2.DAT", "MENU2.DAT"
};

void *SNA_pc_g_pCdCheckErrorFrame1;     /* 0x00510DEC */
void *SNA_pc_g_pCdCheckErrorFrame2;     /* 0x00510C2C */
void *SNA_pc_g_pOpenFileErrorFrame1;    /* 0x00510DF0 */
void *SNA_pc_g_pOpenFileErrorFrame2;    /* 0x00510C30 */

unsigned long  SNA_pc_g_ulSelectorModulus;    /* 0x00511750 */
unsigned long  SNA_pc_g_ulCountModulus;       /* 0x005117B4 */
unsigned long  SNA_pc_g_ulUnitNumerator;      /* 0x005117B0 */
unsigned long  SNA_pc_g_ulUnitDenominator;    /* 0x005117C0 */
unsigned long  SNA_pc_g_ulOffsetMul;          /* 0x005117BC */
long           SNA_pc_g_lOffsetBase;          /* 0x00511754 */

BOOL __stdcall SNA_pc_fn_bReadFile(HANDLE hFile, LPVOID lpBuffer, DWORD dwToRead, LPDWORD lpRead, LPOVERLAPPED lpOverlapped);

#ifndef RAYMAN2_NO_PROTECTION
void __cdecl SNA_pc_vVerifyCdLevelsFile(void)
{
    char           a_cPath[0x108];
    unsigned char *p_ucStub;
    unsigned char *p_ucEnd;
    HANDLE         hFile;
    DWORD          dwRead;
    unsigned long  ulWord;
    unsigned long  a_ulHdr[4];

    if (SNA_g_ucErrorMode != 0) {
        __asm { mov dword ptr [SNA_pc_g_pCdCheckErrorFrame1], offset L_body }
        __asm { mov dword ptr [SNA_pc_g_pCdCheckErrorFrame2], offset L_epi }
        goto L_epi;
    }
L_body:
    if (SNA_p_bEmbeddedGate() == 0) {
        goto L_epi;
    }
    sprintf(a_cPath, SNA_pc_szFmtCdLevels, SNA_g_cCdDriveLetter, SNA_p_pcGetBaseDir(),
            SNA_pc_a_pszLevelFile[0]);

    p_ucStub = SNA_pc_M_pStubSlot();
    MMG_pc_M_vWriteStub(p_ucStub, MMG_pc_C_ImpCreateFileA, 0xFF, 0xE0);
    SNA_pc_M_vFlushA();
    hFile = ((HANDLE (__stdcall *)(const char *, DWORD, DWORD, LPSECURITY_ATTRIBUTES, DWORD, DWORD, HANDLE))p_ucStub)(
        a_cPath, 0x80000000, 0, NULL, 3, 0x80, NULL);
    SNA_pc_M_vScrubStub(p_ucStub);

    if (hFile == INVALID_HANDLE_VALUE) {
        do {
            char a_cText[0x2B] = "Please, insert Rayman II CD in CDROM Drive";
            char a_cTitle[0x16] = "Rayman 2 CD not found";

            if (MessageBoxA(NULL, a_cText, a_cTitle, MB_RETRYCANCEL) == IDCANCEL) {
                GLI_FatalExitOrSleep();
            }
            p_ucStub = SNA_pc_M_pStubSlot();
            MMG_pc_M_vWriteStub(p_ucStub, MMG_pc_C_ImpCreateFileA, 0xFF, 0xE0);
            SNA_pc_M_vFlushA();
            hFile = ((HANDLE (__stdcall *)(const char *, DWORD, DWORD, LPSECURITY_ATTRIBUTES, DWORD, DWORD, HANDLE))p_ucStub)(
                a_cPath, 0x80000000, 0, NULL, 3, 0x80, NULL);
            SNA_pc_M_vScrubStub(p_ucStub);
        } while (hFile == INVALID_HANDLE_VALUE);
    }
    if (hFile == INVALID_HANDLE_VALUE) {
        goto L_epi;
    }

    SNA_pc_M_vStubReadA(a_ulHdr, 0x10);
    SNA_pc_M_vStubReadA(&ulWord, 4);
    a_ulHdr[0] += a_ulHdr[2];
    a_ulHdr[1] += a_ulHdr[3];
    SNA_pc_M_vStubReadA(&ulWord, 4);
    SNA_pc_g_ulUnitNumerator = (ulWord - a_ulHdr[0]) ^ a_ulHdr[1];
    a_ulHdr[0] += a_ulHdr[2];
    a_ulHdr[1] += a_ulHdr[3];
    SNA_pc_M_vStubReadA(&ulWord, 4);
    SNA_pc_g_ulSelectorModulus = ((ulWord - a_ulHdr[0]) ^ a_ulHdr[1]) >> 2;
    a_ulHdr[0] += a_ulHdr[2];
    a_ulHdr[1] += a_ulHdr[3];
    SNA_pc_M_vStubReadB(&ulWord, 4);
    a_ulHdr[0] += a_ulHdr[2];
    a_ulHdr[1] += a_ulHdr[3];
    SNA_pc_M_vStubReadB(&ulWord, 4);
    SNA_pc_g_ulCountModulus = (ulWord - a_ulHdr[0]) ^ a_ulHdr[1];
    a_ulHdr[0] += a_ulHdr[2];
    a_ulHdr[1] += a_ulHdr[3];
    SNA_pc_fn_bReadFile(hFile, &ulWord, 4, &dwRead, NULL);
    SNA_pc_g_ulUnitDenominator = (ulWord - a_ulHdr[0]) ^ a_ulHdr[1];
    a_ulHdr[0] += a_ulHdr[2];
    a_ulHdr[1] += a_ulHdr[3];
    SNA_pc_fn_bReadFile(hFile, &ulWord, 4, &dwRead, NULL);
    SNA_pc_g_lOffsetBase = (long)((ulWord - a_ulHdr[0]) ^ a_ulHdr[1]);
    a_ulHdr[0] += a_ulHdr[2];
    a_ulHdr[1] += a_ulHdr[3];
    SNA_pc_fn_bReadFile(hFile, &ulWord, 4, &dwRead, NULL);
    SNA_pc_g_ulOffsetMul = (ulWord - a_ulHdr[0]) ^ a_ulHdr[1];
    a_ulHdr[0] += a_ulHdr[2];
    a_ulHdr[1] += a_ulHdr[3];
    MMG_pc_fn_bCloseHandle(hFile);

L_epi:
    ;
}
#else
/* Copy protection removed: LEVELS0.DAT is read from the data directory (no CD drive letter, no
   code-pool stubs). Its header carries the selector math of the embedded relocation tables. */
static const char SNA_pc_szFmtLevels[] = "%s\\%s";

void __cdecl SNA_pc_vVerifyCdLevelsFile(void)
{
    char           a_cPath[0x108];
    HANDLE         hFile;
    DWORD          dwRead;
    unsigned long  ulWord;
    unsigned long  a_ulHdr[4];
    unsigned long  a_ulValue[8];
    long           i;

    if (SNA_p_bEmbeddedGate() == 0) {
        return;
    }
    sprintf(a_cPath, SNA_pc_szFmtLevels, SNA_p_pcGetBaseDir(), SNA_pc_a_pszLevelFile[0]);
    hFile = SNA_pc_M_hOpenFile(a_cPath, SNA_pc_C_ulAccessRead, SNA_pc_C_ulOpenExisting,
                               SNA_pc_C_ulAttributeNormal);
    if (hFile == SNA_pc_C_hInvalidFile) {
        return;
    }

    /* eight header words, each (word - a) ^ b with a += c, b += d after every word */
    SNA_pc_M_bReadFile(hFile, a_ulHdr, 0x10, &dwRead);
    for (i = 0; i < 8; i++) {
        SNA_pc_M_bReadFile(hFile, &ulWord, 4, &dwRead);
        a_ulValue[i] = (ulWord - a_ulHdr[0]) ^ a_ulHdr[1];
        a_ulHdr[0] += a_ulHdr[2];
        a_ulHdr[1] += a_ulHdr[3];
    }
    SNA_pc_g_ulUnitNumerator   = a_ulValue[1];
    SNA_pc_g_ulSelectorModulus = a_ulValue[2] >> 2;
    SNA_pc_g_ulCountModulus    = a_ulValue[4];
    SNA_pc_g_ulUnitDenominator = a_ulValue[5];
    SNA_pc_g_lOffsetBase       = (long)a_ulValue[6];
    SNA_pc_g_ulOffsetMul       = a_ulValue[7];
    SNA_pc_M_bCloseFile(hFile);
}
#endif /* RAYMAN2_NO_PROTECTION */

/* 0x0044E750 */
BOOL __stdcall SNA_pc_fn_bReadFile(HANDLE hFile, LPVOID lpBuffer, DWORD dwToRead,
                                   LPDWORD lpRead, LPOVERLAPPED lpOverlapped)
{
#ifndef RAYMAN2_NO_PROTECTION   /* Win32 call through a code-pool stub */
    unsigned char *p_ucStub;
    unsigned char *p_ucEnd;
    BOOL bResult;
    void *hProcess;

    MMG_pc_M_vStepLcg();
    p_ucStub = &MMG_pc_g_uLcgPool.a_ucCode[(int)MMG_g_ulLCGState % MMG_pc_C_PoolSlots];
    MMG_pc_M_vWriteStub(p_ucStub, MMG_pc_C_ImpReadFile, 0x50, 0xC3);
    MMG_pc_M_vFlushPool(hProcess);
    bResult = ((BOOL (__stdcall *)(HANDLE, LPVOID, DWORD, LPDWORD, LPOVERLAPPED))p_ucStub)(
        hFile, lpBuffer, dwToRead, lpRead, lpOverlapped);
    for (p_ucEnd = p_ucStub + MMG_pc_C_StubSize; p_ucStub < p_ucEnd; p_ucStub++) {
        MMG_pc_M_vStepLcg();
        *p_ucStub = (unsigned char)((int)MMG_g_ulLCGState % 0xff);
    }
    return bResult;
#else
    return SNA_pc_M_bReadFile(hFile, lpBuffer, dwToRead, lpRead);   /* lpOverlapped: NULL */
#endif /* RAYMAN2_NO_PROTECTION */
}

HANDLE __stdcall SNA_pc_fn_hCreateFileA(LPCSTR lpFileName, DWORD dwDesiredAccess, DWORD dwShareMode,
                                        LPSECURITY_ATTRIBUTES lpSecurityAttributes,
                                        DWORD dwCreationDisposition, DWORD dwFlagsAndAttributes,
                                        HANDLE hTemplateFile)
{
#ifndef RAYMAN2_NO_PROTECTION   /* Win32 call through a code-pool stub */
    unsigned char *p_ucStub;
    unsigned char *p_ucEnd;
    HANDLE hResult;
    void *hProcess;

    MMG_pc_M_vStepLcg();
    p_ucStub = &MMG_pc_g_uLcgPool.a_ucCode[(int)MMG_g_ulLCGState % MMG_pc_C_PoolSlots];
    MMG_pc_M_vWriteStub(p_ucStub, MMG_pc_C_ImpCreateFileA, 0xFF, 0xE0);
    MMG_pc_M_vFlushPool(hProcess);
    hResult =((HANDLE (__stdcall *)(LPCSTR, DWORD, DWORD, LPSECURITY_ATTRIBUTES, DWORD, DWORD, HANDLE))p_ucStub)(
        lpFileName, dwDesiredAccess, dwShareMode, lpSecurityAttributes, dwCreationDisposition,
        dwFlagsAndAttributes, hTemplateFile);
    for (p_ucEnd = p_ucStub + MMG_pc_C_StubSize; p_ucStub < p_ucEnd; p_ucStub++) {
        MMG_pc_M_vStepLcg();
        *p_ucStub = (unsigned char)((int)MMG_g_ulLCGState % 0xff);
    }
    return hResult;
#else
    /* the callers pass share 0, no security, no template */
    return SNA_pc_M_hOpenFile(lpFileName, dwDesiredAccess, dwCreationDisposition,
                              dwFlagsAndAttributes);
#endif /* RAYMAN2_NO_PROTECTION */
}

DWORD __stdcall SNA_pc_fn_dwSetFilePointer(HANDLE hFile, LONG lDistanceToMove,
                                           PLONG lpDistanceToMoveHigh, DWORD dwMoveMethod)
{
#ifndef RAYMAN2_NO_PROTECTION   /* Win32 call through a code-pool stub */
    unsigned char *p_ucStub;
    unsigned char *p_ucEnd;
    DWORD dwResult;
    void *hProcess;

    MMG_pc_M_vStepLcg();
    p_ucStub = &MMG_pc_g_uLcgPool.a_ucCode[(int)MMG_g_ulLCGState % MMG_pc_C_PoolSlots];
    MMG_pc_M_vWriteStub(p_ucStub, MMG_pc_C_ImpSetFilePointer, 0x50, 0xC3);
    MMG_pc_M_vFlushPool(hProcess);
    dwResult = ((DWORD (__stdcall *)(HANDLE, LONG, PLONG, DWORD))p_ucStub)(
        hFile, lDistanceToMove, lpDistanceToMoveHigh, dwMoveMethod);
    for (p_ucEnd = p_ucStub + MMG_pc_C_StubSize; p_ucStub < p_ucEnd; p_ucStub++) {
        MMG_pc_M_vStepLcg();
        *p_ucStub = (unsigned char)((int)MMG_g_ulLCGState % 0xff);
    }
    return dwResult;
#else
    return SNA_pc_M_ulSeekFile(hFile, lDistanceToMove, dwMoveMethod);   /* no high part: NULL */
#endif /* RAYMAN2_NO_PROTECTION */
}

void *SNA_pc_g_pOpenEmbeddedFrame1;   /* 0x00510DC4 */
void *SNA_pc_g_pOpenEmbeddedFrame2;   /* 0x00510C04 */

HANDLE __cdecl SNA_pc_fn_hOpenFileAtSinusOffset(DWORD *p_ulOffset);

#ifndef RAYMAN2_NO_PROTECTION
unsigned char __cdecl SNA_pc_bOpenEmbeddedSna(void *_p_stFile)
{
    unsigned char *p_ucStub;
    unsigned char *p_ucEnd;
    HANDLE         hFile;
    DWORD          ulOffset;
    unsigned long  ulWord;
    unsigned long  ulX;
    unsigned long  ulPos;
    unsigned char  ucOk;

    if (SNA_g_lProtectionGate != 0) {
        __asm { mov dword ptr [SNA_pc_g_pOpenEmbeddedFrame1], offset L_body }
        __asm { mov dword ptr [SNA_pc_g_pOpenEmbeddedFrame2], offset L_epi }
        return 1;
    }
L_body:
    hFile = SNA_pc_fn_hOpenFileAtSinusOffset(&ulOffset);
    if (hFile == INVALID_HANDLE_VALUE) {
        ucOk = 0;
        goto L_epi;
    }

    MMG_pc_M_vStepLcg();
    p_ucStub = &MMG_pc_g_uLcgPool.a_ucCode[(int)MMG_g_ulLCGState % MMG_pc_C_PoolSlots];
    MMG_pc_M_vWriteStub(p_ucStub, MMG_pc_C_ImpSetFilePointer, 0x50, 0xC3);
    SNA_pc_M_vFlushA();
    ((DWORD (__stdcall *)(HANDLE, LONG, PLONG, DWORD))p_ucStub)(hFile, (LONG)ulOffset, NULL, 0);
    for (p_ucEnd = p_ucStub + MMG_pc_C_StubSize; p_ucStub < p_ucEnd; p_ucStub++) {
        MMG_pc_M_vStepLcg();
        *p_ucStub = (unsigned char)((int)MMG_g_ulLCGState % 0xff);
    }

    SNA_pc_vInitStream(6, _p_stFile);
    SNA_pc_g_a_ucLoadKey[3] = (unsigned char)~SNA_pc_g_a_ucLoadKey[2];
    ulX   = *(unsigned long *)SNA_pc_g_a_ucLoadKey ^ 0x75bd924u;
    ulPos = ulX * 0x41a7u - 0x7fffffffu * (ulX / 0x1f31du);
    ((SNA_pc_tdstStream *)_p_stFile)->uKey.ulState = ulPos;
    SNA_pc_g_a_ulTableKey[SNA_g_ulTableIndex] = *(unsigned long *)SNA_pc_g_a_ucLoadKey;
    ((SNA_pc_tdstStream *)_p_stFile)->hFile = hFile;
    ((SNA_pc_tdstStream *)_p_stFile)->ulValid = 1;
    SNA_fn_ulFRead(&ulWord, 4, 1, _p_stFile);
    SNA_pc_g_a_ulTableState[SNA_g_ulTableIndex] = ulWord;
    SNA_g_ulTableIndex++;

    MMG_pc_M_vStepLcg();
    SNA_pc_g_a_ucLoadKey[2] = (unsigned char)(SNA_pc_g_a_ucLoadKey[2] + (int)MMG_g_ulLCGState % 5 + 1);
    ucOk = 1;
L_epi:
    return ucOk;
}
#else
unsigned char __cdecl SNA_pc_bOpenEmbeddedSna(void *_p_stFile)
{
    HANDLE         hFile;
    DWORD          ulOffset;
    unsigned long  ulWord;
    unsigned long  ulX;
    unsigned long  ulPos;

    hFile = SNA_pc_fn_hOpenFileAtSinusOffset(&ulOffset);
    if (hFile == SNA_pc_C_hInvalidFile) {
        return 0;
    }
    SNA_pc_M_ulSeekFile(hFile, (LONG)ulOffset, SNA_pc_C_ulSeekBegin);

    SNA_pc_vInitStream(6, _p_stFile);
    SNA_pc_g_a_ucLoadKey[3] = (unsigned char)~SNA_pc_g_a_ucLoadKey[2];
    ulX   = *(unsigned long *)SNA_pc_g_a_ucLoadKey ^ 0x75bd924u;
    ulPos = ulX * 0x41a7u - 0x7fffffffu * (ulX / 0x1f31du);
    ((SNA_pc_tdstStream *)_p_stFile)->uKey.ulState = ulPos;
    ((SNA_pc_tdstStream *)_p_stFile)->hFile = hFile;
    ((SNA_pc_tdstStream *)_p_stFile)->ulValid = 1;
    SNA_fn_ulFRead(&ulWord, 4, 1, _p_stFile);   /* table state word: only the protection actors read it */
    return 1;
}
#endif /* RAYMAN2_NO_PROTECTION */

HANDLE __cdecl SNA_pc_fn_hOpenFileAtSinusOffset(DWORD *p_ulOffset);

/* 0x004A2270 */
static const char SNA_pc_szCdromNotFound[] = "CDROM not found!";

#define SNA_pc_M_vScrubStubCall(p_ucStub) \
    { \
        for (p_ucEnd = (p_ucStub) + MMG_pc_C_StubSize; (p_ucStub) < p_ucEnd; (p_ucStub)++) { \
            *(p_ucStub) = (unsigned char)((int)MMG_pc_fn_ulStepLcgState() % 0xff); \
        } \
    }

#define SNA_pc_M_vOpenStubA(lpPath) \
    { \
        p_ucStub = SNA_pc_M_pStubSlot(); \
        MMG_pc_M_vWriteStub(p_ucStub, MMG_pc_C_ImpCreateFileA, 0xFF, 0xE0); \
        SNA_pc_M_vFlushA(); \
        hFile = ((HANDLE (__stdcall *)(const char *, DWORD, DWORD, LPSECURITY_ATTRIBUTES, DWORD, DWORD, HANDLE))p_ucStub)( \
            (lpPath), 0x80000000, 0, NULL, 3, 0x80, NULL); \
        SNA_pc_M_vScrubStubCall(p_ucStub); \
    }

#define SNA_pc_M_vStubReadD(lpBuf, cbBuf) \
    { \
        p_ucStub = SNA_pc_M_pStubSlot(); \
        MMG_pc_M_vWriteStub(p_ucStub, MMG_pc_C_ImpReadFile, 0x50, 0xC3); \
        SNA_pc_M_vFlushViaForwarder(); \
        ((BOOL (__stdcall *)(HANDLE, LPVOID, DWORD, LPDWORD, LPOVERLAPPED))p_ucStub)( \
            hFile, (lpBuf), (cbBuf), &dwRead, NULL); \
        SNA_pc_M_vScrubStubCall(p_ucStub); \
    }

#define SNA_pc_M_vStubSeekD(lDist, dwMethod) \
    { \
        p_ucStub = SNA_pc_M_pStubSlot(); \
        MMG_pc_M_vWriteStub(p_ucStub, MMG_pc_C_ImpSetFilePointer, 0x50, 0xC3); \
        SNA_pc_M_vFlushViaForwarder(); \
        ((DWORD (__stdcall *)(HANDLE, LONG, PLONG, DWORD))p_ucStub)( \
            hFile, (lDist), NULL, (dwMethod)); \
        SNA_pc_M_vScrubStubCall(p_ucStub); \
    }

#ifndef RAYMAN2_NO_PROTECTION
HANDLE __cdecl SNA_pc_fn_hOpenFileAtSinusOffset(DWORD *p_ulOffset)
{
    char           a_cPath[0x104];
    unsigned char  a_ucBufA[0x200];
    unsigned char  a_ucBufB[0x200];
    unsigned char *p_ucStub;
    unsigned char *p_ucEnd;
    HANDLE         hFile;
    DWORD          dwRead;
    unsigned long  ulWord;
    unsigned long  a_ulHdr[4];
    unsigned long  ulCount;
    unsigned long  ulKind;
    unsigned long  ulN;
    unsigned long  ulUnit;
    __int64        i64Ival;             /* 0x5117B0 */
    unsigned long  ulQuot;
    unsigned long  i;
    long           lRem;
    long           lPos;
    double         dAcc;
    double         dSin;
    double         dIp;
    double         dFrac;
    double         dFloor1;
    double         dFloor2;

    if (SNA_g_lProtectionGate != 0) {
        __asm { mov dword ptr [SNA_pc_g_pOpenFileErrorFrame1], offset L_body }
        __asm { mov dword ptr [SNA_pc_g_pOpenFileErrorFrame2], offset L_epi }
        return (HANDLE)0;
    }
L_body:
    sprintf(a_cPath, SNA_pc_szFmtCdLevels, SNA_g_cCdDriveLetter, SNA_p_pcGetBaseDir(),
            SNA_pc_a_pszLevelFile[0]);
    SNA_pc_M_vOpenStubA(a_cPath);
    if (hFile == INVALID_HANDLE_VALUE) {
        do {
            if (SNA_p_iAskInsertCd(SNA_C_szInsertCd, SNA_pc_szCdromNotFound, 5) == 2) {
                SNA_p_vOnCdPromptAbort();
            }
            SNA_pc_M_vOpenStubA(a_cPath);
        } while (hFile == INVALID_HANDLE_VALUE);
        if (hFile == INVALID_HANDLE_VALUE) {
            goto L_epi;
        }
    }

    /* 0x0044F204 */
    ulN = (unsigned long)SNA_pc_g_a_ucLoadKey[0] % SNA_pc_g_ulSelectorModulus;
    ulKind = (unsigned long)SNA_pc_g_a_ucLoadKey[1] & 3;
    SNA_pc_g_a_ucLoadKey[0] = (unsigned char)ulN;
    SNA_pc_g_a_ucLoadKey[1] = (unsigned char)ulKind;
    ulCount = ulN * 4;
    switch (ulKind) {
    case 1:
        ulCount += 1;
        break;
    case 2:
        ulCount += 2;
        break;
    case 3:
        ulCount += 3;
        break;
    default:
        break;
    }

    ulN = (unsigned long)SNA_pc_g_a_ucLoadKey[2] % SNA_pc_g_ulCountModulus;
    SNA_pc_g_a_ucLoadKey[2] = (unsigned char)ulN;
    dAcc = 1.06913;
    if (ulN != 0) {
        i = 0;
        do {
            dSin = sin((double)i * (double)i * 1.69314);
            dAcc = (dAcc - fabs(dSin) * -0.69314) - -0.52658;
            i++;
        } while (i < ulN);
    }
    dFrac = modf(dAcc, &dIp);
    dFloor1 = floor(dFrac * 1000000.0);
    i64Ival = (__int64)SNA_pc_g_ulUnitNumerator;
    dFloor2 = floor((double)i64Ival * (dFloor1 * 1e-6));
    lPos = (long)dFloor2 * (long)SNA_pc_g_ulOffsetMul + SNA_pc_g_lOffsetBase;

    p_ucStub = SNA_pc_M_pStubSlot();
    MMG_pc_M_vWriteStub(p_ucStub, MMG_pc_C_ImpSetFilePointer, 0x50, 0xC3);
    SNA_pc_M_vFlushA();
    ((DWORD (__stdcall *)(HANDLE, LONG, PLONG, DWORD))p_ucStub)(hFile, lPos, NULL, 0);
    SNA_pc_M_vScrubStubCall(p_ucStub);

    SNA_pc_M_vStubReadD(a_ulHdr, 0x10);

    if (ulCount != 0) {
        a_ulHdr[0] += a_ulHdr[2] * ulCount;
        ulN = ulCount;
        do {
            a_ulHdr[1] += a_ulHdr[3];
            ulN--;
        } while (ulN != 0);
    }

    SNA_pc_M_vStubSeekD(ulCount * 4, 1);
    SNA_pc_M_vStubReadD(&ulWord, 4);

    *p_ulOffset = (ulWord - a_ulHdr[0]) ^ a_ulHdr[1];

    /* 0x00448480 */
    ulUnit = SNA_pc_g_ulUnitNumerator / SNA_pc_g_ulUnitDenominator;
    ulQuot = *p_ulOffset / (ulUnit << 11);
    lRem = 0;
    if (ulQuot != 0) {
        lRem = (int)MMG_pc_fn_ulStepLcgState() % (int)ulQuot;
    }

    SNA_pc_fn_dwSetFilePointer(hFile, *p_ulOffset, NULL, 0);
    SNA_pc_fn_bReadFile(hFile, a_ucBufA, 0x200, &dwRead, NULL);
    *p_ulOffset -= (unsigned long)lRem * (ulUnit << 11);
    MMG_pc_fn_bCloseHandle(hFile);

    sprintf(a_cPath, SNA_pc_szFmtCdLevels, SNA_g_cCdDriveLetter, SNA_p_pcGetBaseDir(),
            SNA_pc_a_pszLevelFile[lRem]);
    hFile = SNA_pc_fn_hCreateFileA(a_cPath, 0x80000000, 0, NULL, 3, 0x80, NULL);
    if (hFile == INVALID_HANDLE_VALUE) {
        do {
            if (SNA_p_iAskInsertCd(SNA_C_szInsertCd, SNA_pc_szCdromNotFound, 5) == 2) {
                SNA_p_vOnCdPromptAbort();
            }
            hFile = SNA_pc_fn_hCreateFileA(a_cPath, 0x80000000, 0, NULL, 3, 0x80, NULL);
        } while (hFile == INVALID_HANDLE_VALUE);
    }
    SNA_pc_fn_dwSetFilePointer(hFile, *p_ulOffset, NULL, 0);
    SNA_pc_fn_bReadFile(hFile, a_ucBufB, 0x200, &dwRead, NULL);

L_epi:
    return hFile;
}
#else
HANDLE __cdecl SNA_pc_fn_hOpenFileAtSinusOffset(DWORD *p_ulOffset)
{
    char           a_cPath[0x104];
    HANDLE         hFile;
    DWORD          dwRead;
    unsigned long  ulWord;
    unsigned long  a_ulHdr[4];
    unsigned long  ulCount;
    unsigned long  ulKind;
    unsigned long  ulN;
    __int64        i64Ival;
    unsigned long  i;
    long           lPos;
    double         dAcc;
    double         dSin;
    double         dIp;
    double         dFrac;
    double         dFloor1;
    double         dFloor2;

    sprintf(a_cPath, SNA_pc_szFmtLevels, SNA_p_pcGetBaseDir(), SNA_pc_a_pszLevelFile[0]);
    hFile = SNA_pc_M_hOpenFile(a_cPath, SNA_pc_C_ulAccessRead, SNA_pc_C_ulOpenExisting,
                               SNA_pc_C_ulAttributeNormal);
    if (hFile == SNA_pc_C_hInvalidFile) {
        return hFile;
    }

    /* load key: [0] level, [1] table kind, [2] copy selector (always 0: first copy) */
    ulN = (unsigned long)SNA_pc_g_a_ucLoadKey[0] % SNA_pc_g_ulSelectorModulus;
    ulKind = (unsigned long)SNA_pc_g_a_ucLoadKey[1] & 3;
    SNA_pc_g_a_ucLoadKey[0] = (unsigned char)ulN;
    SNA_pc_g_a_ucLoadKey[1] = (unsigned char)ulKind;
    ulCount = ulN * 4 + ulKind;

    ulN = (unsigned long)SNA_pc_g_a_ucLoadKey[2] % SNA_pc_g_ulCountModulus;
    SNA_pc_g_a_ucLoadKey[2] = (unsigned char)ulN;
    dAcc = 1.06913;
    for (i = 0; i < ulN; i++) {
        dSin = sin((double)i * (double)i * 1.69314);
        dAcc = (dAcc - fabs(dSin) * -0.69314) - -0.52658;
    }
    dFrac = modf(dAcc, &dIp);
    dFloor1 = floor(dFrac * 1000000.0);
    i64Ival = (__int64)SNA_pc_g_ulUnitNumerator;
    dFloor2 = floor((double)i64Ival * (dFloor1 * 1e-6));
    lPos = (long)dFloor2 * (long)SNA_pc_g_ulOffsetMul + SNA_pc_g_lOffsetBase;

    SNA_pc_M_ulSeekFile(hFile, lPos, SNA_pc_C_ulSeekBegin);
    SNA_pc_M_bReadFile(hFile, a_ulHdr, 0x10, &dwRead);
    a_ulHdr[0] += a_ulHdr[2] * ulCount;
    a_ulHdr[1] += a_ulHdr[3] * ulCount;
    SNA_pc_M_ulSeekFile(hFile, (LONG)(ulCount * 4), SNA_pc_C_ulSeekCurrent);
    SNA_pc_M_bReadFile(hFile, &ulWord, 4, &dwRead);
    *p_ulOffset = (ulWord - a_ulHdr[0]) ^ a_ulHdr[1];

    /* The CD split this stream into files of (ulUnitNumerator / ulUnitDenominator) << 11 bytes
       (SNA_pc_a_pszLevelFile) and read the table from a randomly chosen one of them. LEVELS0.DAT
       holds the whole stream, so the table is read there at its full offset. */
    return hFile;
}
#endif /* RAYMAN2_NO_PROTECTION */
