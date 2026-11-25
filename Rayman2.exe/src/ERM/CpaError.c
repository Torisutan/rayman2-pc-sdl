#include "ERM/CpaError.h"
#include "GLI/Init_Gli.h"

/* 0x004fb620 */
ERM_tdstLastErrorInfo g_stErmTheLastErrorInfo[C_ucErmNbOfErrorChannel];
/* 0x004a6ab4 */
unsigned char g_ucErmNbOfModule;
/* 0x004a6ac0 */
HWND g_hWndHdl4Request_storage;
/* 0x004a6ab8 */
ERM_fn_ExitCallback_t p_fn_ExitAppliCallBack_storage;

unsigned char g_ucErmModuleId = 0xFF;

void Erm_fn_vSetCallBackForExit(void (*_p_fn_ExitAppliCallBack)(void))
{
    p_fn_ExitAppliCallBack = _p_fn_ExitAppliCallBack;
}

unsigned char Erm_fn_ucInitErrMsg(void)
{
    unsigned char ucI;

    if (g_ucErmNbOfModule == 0)
    {

        for (ucI = 0; ucI < C_ucErmNbOfErrorChannel; ucI++)
        {
            g_stErmTheLastErrorInfo[ucI].uwLastErr   = C_uwErmNoErr;
            g_stErmTheLastErrorInfo[ucI].ulChannelId = C_ucErmDefaultChannel;
        }
    }

    return (g_ucErmNbOfModule++);
}

unsigned short Erm_fn_uwCheckError(unsigned char ucModuleId, unsigned long ulChannelId)
{
    unsigned char ucI;
    for (ucI = 0;
         (  ucI < C_ucErmNbOfErrorChannel) &&
            (g_stErmTheLastErrorInfo[ucI].uwLastErr != C_uwErmNoErr) &&
            (g_stErmTheLastErrorInfo[ucI].ulChannelId != ulChannelId);
         ucI++) {};

    if (ucI >= C_ucErmNbOfErrorChannel)
        return C_uwErmNoErr;
    else
    {
        if ((g_stErmTheLastErrorInfo[ucI].ulChannelId == ulChannelId) &&
            (g_stErmTheLastErrorInfo[ucI].uwLastErr != C_uwErmNoErr) &&
            (g_stErmTheLastErrorInfo[ucI].ucLastFailedModuleId != ucModuleId))
        {
            char szTxt[C_ucSizeOfErrTxt];

            Erm_M_CheckLenOfTxt(szTxt);
            Erm_fn_v_PrintErrMsg(szTxt, "");
            Erm_M_PrintInfoWindow(szTxt, "");
        }
        return g_stErmTheLastErrorInfo[ucI].uwLastErr;
    }
}

ERM_tdstLastErrorInfo *Erm_fn_p_stFindstMyLastErrorInfo(unsigned long ulChannelId)
{
    unsigned char ucI;
    for (ucI = 0;
         (  ucI < C_ucErmNbOfErrorChannel) &&
            (g_stErmTheLastErrorInfo[ucI].uwLastErr != C_uwErmNoErr) &&
            (g_stErmTheLastErrorInfo[ucI].ulChannelId != ulChannelId);
         ucI++) {};

    if (ucI >= C_ucErmNbOfErrorChannel)
    {

        return NULL;
    }
    else
    {

        return g_stErmTheLastErrorInfo + ucI;
    }
}

void ERM_fn_vClearLastError(unsigned long ulChannelId)
{
    unsigned char ucJ;
    unsigned char ucI;

    for (ucI = 0;
         (  ucI < C_ucErmNbOfErrorChannel) &&
            (g_stErmTheLastErrorInfo[ucI].uwLastErr != C_uwErmNoErr) &&
            (g_stErmTheLastErrorInfo[ucI].ulChannelId != ulChannelId);
         ucI++) {};

    if (ucI < C_ucErmNbOfErrorChannel && (g_stErmTheLastErrorInfo[ucI].uwLastErr != C_uwErmNoErr))
    {
        (g_stErmTheLastErrorInfo + ucI)->uwLastErr = C_uwErmNoErr;
        Erm_M_AffectUlChannelId((g_stErmTheLastErrorInfo + ucI)->ulChannelId, C_ucErmDefaultChannel);
        Erm_fn_v_PrintErrMsg("Explicit request for clear of the last error", "");
        Erm_M_PrintTraceClearWindow();

        ucJ = (unsigned char)(ucI + 1);
        if ((ucJ < C_ucErmNbOfErrorChannel) && (g_stErmTheLastErrorInfo[ucJ].uwLastErr != C_uwErmNoErr))
        {
            for (;
                 (  ucJ < C_ucErmNbOfErrorChannel) &&
                    (g_stErmTheLastErrorInfo[ucJ].uwLastErr != C_uwErmNoErr);
                 ucJ++)
            {
                g_stErmTheLastErrorInfo[ucJ - 1] = g_stErmTheLastErrorInfo[ucJ];
                g_stErmTheLastErrorInfo[ucJ].uwLastErr = C_uwErmNoErr;
                Erm_M_AffectUlChannelId(g_stErmTheLastErrorInfo[ucJ].ulChannelId, C_ucErmDefaultChannel);
            }
        }
    }
    else
    {
        Erm_M_PrintTraceUnUsefullClearWindow();
        Erm_fn_v_PrintErrMsg("Explicit request for clear of the last error.\n But there is no error !", "");

    }
}

void Erm_fn_v_PrintModuleList(void)
{

}

void ERM_fn_vUpdateLastError(unsigned short uwStartOfWarningsId,
                             unsigned char  ucModuleId,
                             unsigned long  ulChannelId,
                             unsigned short uwErrNum,
                             long           lDebugData,
                             unsigned char  ucOpenInfoWindow,
                             unsigned char  ucStopForDebug,
                             char          *szPersonalMsg)
{
    tdstLastErrorInfo *p_stMyLastErrorInfo;

    Erm_M_PrintSeparationBegin();
    p_stMyLastErrorInfo = Erm_M_p_stFindstMyLastErrorInfo(ulChannelId);
    if (p_stMyLastErrorInfo == NULL)
    {
        Erm_fn_v_PrintErrMsg("Error in the Erm:", "There is no more Channel error to raise the new error.\nPerhaps the constant C_ucErmNbOfErrorChannel is too low.");
        Erm_M_PrintInfoWindow("Error in the Erm:", "There is no more Channel error to raise the new error.\nPerhaps the constant C_ucErmNbOfErrorChannel is too low.");

        if (p_fn_ExitAppliCallBack)
            p_fn_ExitAppliCallBack();
        GLI_FatalExitOrSleep();     /* 0x4011c0 */

    }
    else
    {
        if ((*p_stMyLastErrorInfo).uwLastErr != C_uwErmNoErr)
        {
            Erm_fn_v_PrintErrMsg("Strange!\nThe last error was never treated", "");
            Erm_M_PrintInfoWindow("Strange!\tThe last error was never treated\n\nThe last error was:", Erm_M_szGetLastErrorString(*p_stMyLastErrorInfo));
        }

        Erm_M_AffectLastErrorInfo((*p_stMyLastErrorInfo), ModuleName, uwErrNum, ulChannelId, lDebugData, ucModuleId, p_cFileName, uwLineNumber);
        if ((*p_stMyLastErrorInfo).uwLastErr < uwStartOfWarningsId)
        {

            if (szPersonalMsg == NULL)
            {

                Erm_fn_v_PrintErrMsg("Fatale error: ", Erm_M_szGetLastErrorString((*p_stMyLastErrorInfo)));

                Erm_M_PrintInfoWindow("Fatale error: ", Erm_M_szGetLastErrorString((*p_stMyLastErrorInfo)));
            }
            else
            {
                Erm_fn_v_PrintErrMsgWithPersonalMsg("\nFatale error: ", Erm_M_szGetLastErrorString((*p_stMyLastErrorInfo)), szPersonalMsg);

                Erm_M_PrintInfoWindowWithPersonalMsg("Fatale error: ", Erm_M_szGetLastErrorString((*p_stMyLastErrorInfo)), szPersonalMsg);

            }

            if (p_fn_ExitAppliCallBack)
                p_fn_ExitAppliCallBack();
            GLI_FatalExitOrSleep();     /* 0x4011c0 */

        }
        else
        {

            if (szPersonalMsg == NULL)
            {

                Erm_fn_v_PrintErrMsg("Warning error: ", Erm_M_szGetLastErrorString((*p_stMyLastErrorInfo)));

                if (ucOpenInfoWindow)

                {
                    Erm_M_PrintInfoWindow("Warning error: ", Erm_M_szGetLastErrorString((*p_stMyLastErrorInfo)));
                }
            }
            else
            {
                Erm_fn_v_PrintErrMsgWithPersonalMsg("Warning error: ", Erm_M_szGetLastErrorString((*p_stMyLastErrorInfo)), szPersonalMsg);

                if (ucOpenInfoWindow)

                {

                    Erm_M_PrintInfoWindowWithPersonalMsg("Warning error: ", Erm_M_szGetLastErrorString((*p_stMyLastErrorInfo)), szPersonalMsg);

                }
            }

            DebugIf((ucStopForDebug != C_ucNeverStopForDebug), Erm_M_AnErrorWasRaised());

        }
    }
    Erm_M_PrintSeparationEnd();
}

#ifdef USE_SDL
/* Erm_fn_iMessageBox: ERM/sdl_erm.c */
#else
int Erm_fn_iMessageBox(char *_szText, char *_szTitle, unsigned int _uiStyle)
{
    HWND hWnd = g_hWndHdl4Request;

    if (hWnd != C_hNoWindowTreatment)
    {
        tdstMsgBox stMsgBox;

        stMsgBox.szText  = _szText;
        stMsgBox.szTitle = _szTitle;
        stMsgBox.uiStyle = _uiStyle;

        SendMessage(hWnd, WM_MESSAGEBOX, C_uiWinCm_RequestMessageBox, (LPARAM)&stMsgBox);

        return stMsgBox.iResult;
    }

    else
        return MessageBox(NULL, _szText, _szTitle, _uiStyle);
}
#endif /* USE_SDL */

void Erm_fn_vSetWndHdl4Request(HWND hWin)
{
    g_hWndHdl4Request = hWin;
}

void Erm_fn_vAnErrorWasRaised(void)
{

}

void Erm_fn_v_ClearLogFile(void)
{

}

void Erm_fn_v_Printsz(char *szTxt)
{

}

void Erm_fn_v_PrintErrMsg(char *p_cTxt1, char *p_cTxt2)
{

}

void Erm_fn_v_PrintErrMsgWithPersonalMsg(char *p_cTxt1, char *p_cTxt2, char *p_cTxt3)
{

}

void Erm_fn_vPrintNameOfAllModules(char *szFileName)
{

}
