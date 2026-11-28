#pragma once

#ifdef __cplusplus
extern "C" {
#endif

extern unsigned char  SNA_g_ucLoadListMode;
/* 0x005116F0 */
extern unsigned char  SNA_pc_g_a_ucLoadKey[4];
#define SNA_g_ucNextRelocationTableToLoad   (SNA_pc_g_a_ucLoadKey[0])
#define SNA_pc_g_ucNextRelocationTableKind  (SNA_pc_g_a_ucLoadKey[1])
/* 0x0044DC40 */
typedef struct SNA_pc_tdstRelocSlot_
{
    unsigned long ulValue;
    unsigned char ucModule;
    unsigned char ucBlock;
    unsigned char a2_uc_pc_Pad[2];
} SNA_pc_tdstRelocSlot;
typedef struct SNA_pc_tdstGptRecord_
{
    unsigned char         ucModule;
    unsigned char         ucBlock;
    unsigned long         ulNbSlots;
    SNA_pc_tdstRelocSlot *d_stSlot;
} SNA_pc_tdstGptRecord;
typedef struct SNA_pc_tdstGptTable_
{
    SNA_pc_tdstGptRecord *d_stRecord;
    unsigned char         ucNbRecords;
} SNA_pc_tdstGptTable;

/* 0x004B7358 */
extern SNA_pc_tdstGptTable *SNA_g_pGptContext;

#define SNA_C_lMaxLoadedTables  20
extern unsigned long SNA_g_ulTableIndex;
extern unsigned long SNA_pc_g_a_ulTableState[SNA_C_lMaxLoadedTables];
extern unsigned long SNA_pc_g_a_ulTableKey[SNA_C_lMaxLoadedTables];

/* 0x004A2028 */
typedef struct SNA_tdstLoadListEntry_
{
    unsigned char *p_ucModuleId;
    long           lBlockId;
} SNA_tdstLoadListEntry;

void          __cdecl SNA_pc_vSortLoadList(SNA_tdstLoadListEntry *_p_stList,
                                           int _iCount);          /* 0x0044D3A0 */
unsigned long __cdecl SNA_pc_bIsBlockInLoadList(unsigned char _ucModuleId,
                                                char _cBlockId);  /* 0x0044D430 */
void          __cdecl SNA_pc_vInitLoadLists(void);               /* 0x0044D4A0 */
void          __cdecl SNA_pc_vFreeGptTable(SNA_pc_tdstGptTable *_p_stGpt); /* 0x0044DDE0 */
SNA_pc_tdstGptTable *__cdecl SNA_pc_pReadGptTable(void);         /* 0x0044DC40 */
unsigned char __cdecl SNA_pc_bOpenSnaFamilyFile(void *_p_stFile);/* 0x0044D990 */
int           __cdecl SNA_pc_vLoadAndRelocate(char *_szFile);    /* 0x0044D4F0 */

void          __cdecl SNA_pc_vRawReadGptFile(void *_p_vBuffer,
                                             unsigned long _dwBytes);   /* 0x0044CFA0 */
void          __cdecl SNA_pc_vOpenGptFile(char *_szPath);               /* 0x0044CD60 */
unsigned long __cdecl SNA_pc_ulReadAndRelocateLong(void);              /* 0x0044CFC0 */
void          __cdecl SNA_pc_vReadAndRelocateBlock(int *_p_iBuffer,
                                                   unsigned int _uiBytes); /* 0x0044D0B0 */
void          __cdecl SNA_pc_vReadAndRelocateArray(int *_p_iBuffer, int _iCount,
                                                   unsigned int _uiElemSize); /* 0x0044D160 */
void          __cdecl SNA_pc_vReadRawArray(void *_p_vBuffer, int _iCount,
                                           unsigned int _uiElemSize);   /* 0x0044D190 */
void          __cdecl SNA_pc_vCloseGptFile(void);                      /* 0x0044CEC0 */
void          __cdecl SNA_pc_vOpenSndFile(char *_szPath);              /* 0x0044D1E0 */
void          __cdecl SNA_pc_vReadAndRelocateSnd(char *_szPath);      /* 0x0044D2F0 */

#ifdef __cplusplus
}
#endif
