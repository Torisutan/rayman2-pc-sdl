#pragma once

#include <windows.h>
#include <stdlib.h>

#if defined(U64) || defined(NITRO)
#include "cpa_std.h"
#endif

#define C_szErmVersion  "ERM V5.1.0"
#define C_szErmFullName "Error Manager"
#define C_szErmDate     "SEP 06 1997"

#define C_uwErmNoErr                (0)

#define C_ucNbOfMaxModule           (255)

#define C_ucSizeOfErrTxt            (512)

#define C_ucSizeOfModuleInformation (200)

#define C_ucErmNbOfErrorChannel     (3)

#define C_ucErmOpenInfoWindow       (0xFF)

#define C_ucErmNoOpenInfoWindow     (0)

#define C_lErmNoDebugData           (0xFFFFFFFF)

#define C_ucNeverStopForDebug       (0)
#define C_ucAllowStopForDebug       (0xFF)

#define C_ucErmDefaultChannel       (0)

#define C_ucModuleNotInitialized    (0xff)

#define C_szErrorLogFileName        "Error.log"

#define C_ucErmVersionAccessId      0
#define C_ucErmFullNameAccessId     1
#define C_ucErmDateAccessId         2

#define WM_MESSAGEBOX               WM_USER
#define C_uiWinCm_RequestMessageBox 0x0024
#define C_hNoWindowTreatment        0

typedef struct tdstMsgBox_ {
    char        *szText;
    char        *szTitle;
    unsigned int uiStyle;
    int          iResult;
} tdstMsgBox;

typedef struct ERM_tdstLastErrorInfo_ {
    unsigned short uwLastErr;
    unsigned char  ucLastFailedModuleId;
    unsigned char  _pad;
    unsigned long  ulChannelId;

} ERM_tdstLastErrorInfo;

typedef ERM_tdstLastErrorInfo tdstLastErrorInfo;

/* 0x4a6ab4 */
extern unsigned char g_ucErmNbOfModule;

/* 0x4fb620 */
extern ERM_tdstLastErrorInfo g_stErmTheLastErrorInfo[C_ucErmNbOfErrorChannel];

/* 0x4a6ab8 */
typedef void (*ERM_fn_ExitCallback_t)(void);
extern ERM_fn_ExitCallback_t p_fn_ExitAppliCallBack_storage;   /* 0x4a6ab8 */
#define p_fn_ExitAppliCallBack  p_fn_ExitAppliCallBack_storage

extern HWND g_hWndHdl4Request_storage;   /* 0x004a6ac0 */
#define g_hWndHdl4Request       g_hWndHdl4Request_storage

#define Erm_M_AffectUlChannelId(A, B)       ((A) = (B))

#define Erm_M_p_stFindstMyLastErrorInfo(ulChannelId) \
    Erm_fn_p_stFindstMyLastErrorInfo(ulChannelId)

#define Erm_M_DebugAffect(A, B)
#define DebugIf(cond, action)

#define Erm_M_PrintSeparationBegin()
#define Erm_M_PrintSeparationEnd()
#define Erm_M_szGetLastErrorString(A)       ""
#define Erm_M_GetModuleInformation(szModuleFullName, ucModuleId) \
    *((char *)(szModuleFullName)) = '\0'
#define Erm_M_CheckLenOfTxt(szTxt)

#define Erm_M_PrintInfoWindow(szTxt1, szTxt2)
#define Erm_M_PrintInfoWindowWithPersonalMsg(s1, s2, s3)
#define Erm_M_CheckLenOfTxtWithoutUpdate(szTxt)

#define Erm_M_PrintTraceClearWindow()
#define Erm_M_PrintTraceUnUsefullClearWindow()
#define Erm_M_StopForDebugForTraceClear(szTxt, fn, ln)

#define Erm_M_AnErrorWasRaised()

#define Erm_M_AffectLastErrorInfo(stMyLastErrorInfo, ModuleName, uwErrNum, \
                                  ulMyChannelId, lMyDebugData, ucModuleId, \
                                  p_cFileName, uwLineNumber) \
{ \
    (stMyLastErrorInfo).uwLastErr = (uwErrNum); \
    (stMyLastErrorInfo).ucLastFailedModuleId = (ucModuleId); \
    Erm_M_DebugAffect((stMyLastErrorInfo).lDebugData, (long)(lMyDebugData)); \
    Erm_M_AffectUlChannelId((stMyLastErrorInfo).ulChannelId, (ulMyChannelId)); \
}

#define Erm_M_Error_Exit() \
{ \
    Erm_M_AnErrorWasRaised(); \
    if (p_fn_ExitAppliCallBack) \
        p_fn_ExitAppliCallBack(); \
    exit(-1); \
}

#define Erm_M_ClearLastError(ulMyChannelId) \
    ERM_fn_vClearLastError(ulMyChannelId)

#define Erm_M_UpdateLastError(ModuleName, ulChannelId, uwErrNum, lDebugData, \
                              ucOpenInfoWindow, ucStopForDebug, szPersonalMsg) \
    ERM_fn_vUpdateLastError((unsigned short)E_uw##ModuleName##StartOfWarning, \
                            (unsigned char)g_uc##ModuleName##ModuleId, \
                            (ulChannelId), (uwErrNum), (lDebugData), \
                            (ucOpenInfoWindow), (ucStopForDebug), (szPersonalMsg))

#define Erm_M_InitErrMsg(ModuleAbreviation) \
{ \
    if (g_uc##ModuleAbreviation##ModuleId == C_ucModuleNotInitialized) \
        g_uc##ModuleAbreviation##ModuleId = Erm_fn_ucInitErrMsg(); \
}

unsigned char Erm_fn_ucInitErrMsg(void);

unsigned short Erm_fn_uwCheckError(unsigned char ucModuleId,
                                   unsigned long ulChannelId);

ERM_tdstLastErrorInfo *Erm_fn_p_stFindstMyLastErrorInfo(unsigned long ulChannelId);

void ERM_fn_vClearLastError(unsigned long ulChannelId);

void ERM_fn_vUpdateLastError(unsigned short uwStartOfWarningsId,
                             unsigned char  ucModuleId,
                             unsigned long  ulChannelId,
                             unsigned short uwErrNum,
                             long           lDebugData,
                             unsigned char  ucOpenInfoWindow,
                             unsigned char  ucStopForDebug,
                             char          *szPersonalMsg);

void Erm_fn_vSetCallBackForExit(void (*_p_fn_ExitAppliCallBack)(void));

void Erm_fn_v_PrintModuleList(void);

int Erm_fn_iMessageBox(char *_szText, char *_szTitle, unsigned int _uiStyle);

void Erm_fn_vSetWndHdl4Request(HWND hWin);

void Erm_fn_v_PrintErrMsg(char *p_cTxt1, char *p_cTxt2);
void Erm_fn_v_PrintErrMsgWithPersonalMsg(char *p_cTxt1, char *p_cTxt2, char *p_cTxt3);
void Erm_fn_v_Printsz(char *szTxt);
void Erm_fn_v_ClearLogFile(void);
void Erm_fn_vPrintNameOfAllModules(char *szFileName);

void Erm_fn_vAnErrorWasRaised(void);
