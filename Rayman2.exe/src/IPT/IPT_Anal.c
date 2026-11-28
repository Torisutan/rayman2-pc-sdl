#define IPT_ANAL_DEFINES_PREDICATES
#include "IPT/IPT.h"
#include "GAM/GAM_types.h"
#include "ERM/CpaError.h"

IPT_tdstEntryElement   *IPT_g_hEngineMonitoredEntry;  /* 0x005002C0 */
IPT_tdstEntryElement   *IPT_g_h_ActualEntryInput;     /* 0x00509E34 */
IPT_tdstKeyWordElement *IPT_g_hNextParameter;         /* 0x00509E38 */
int                     IPT_pc_g_bMouseEnabled;       /* 0x004B9C68 */

unsigned char g_ucIPTModuleId = 0xFF;

/* 0x00495080 */
extern short __cdecl INO_fn_wReadAllDevices(unsigned short uwType, short wMode);

#include "INO/INO.h"

#define IPT_pc_wPollDevice(hDevice) INO_M_wReadDevice((INO_tdstDevice *)(hDevice))

/* 0x00402810 */
extern void __cdecl ERM_fn_vUpdateLastError(unsigned short uwStartOfWarningsId,
                                            unsigned char  ucModuleId,
                                            unsigned long  ulChannelId,
                                            unsigned short uwErrNum,
                                            long           lDebugData,
                                            unsigned char  ucOpenInfoWindow,
                                            unsigned char  ucStopForDebug,
                                            char          *szPersonalMsg);

#include <stdio.h>
#include <stdlib.h>
#include "GLI/GLD.h"
#include "FIL/File.h"
extern void __cdecl FIL_fn_vSaveBMPInFile(char *p_szFileName, int iPixelData, int iWidth, int iHeight,
                                            unsigned char ucBytesPerPixel,
                                            unsigned long ulPixelStride); /* 0x00492C30 */

#include "GLD/GLD_Pub.h"

long __cdecl IPT_fn_bIsJustValidated(IPT_tdstEntryElement *_hEntryElement);
void __cdecl GAM_pc_fn_vTakeSnapShot(void);

/* 0x00413860 */
void __cdecl fn_vAnalyse3DOSSpecialInputActions(void)
{

    unsigned char bSnapShot = ((unsigned char)IPT_fn_bIsJustValidated(IPT_g_hEngineMonitoredEntry) == IPT_C_TRUE);

    if (bSnapShot)
    {
        fn_vSaveEngineClock();
        GAM_pc_fn_vTakeSnapShot();
        fn_vLoadEngineClock();
    }
}
#define IPT_pc_ulEngineViewportHeight \
    (((struct GLD_tdstViewportAttributes_ *)GAM_g_stEngineStructure.stViewportAttr)->dwHeight)
#define IPT_pc_ulEngineViewportWidth \
    (((struct GLD_tdstViewportAttributes_ *)GAM_g_stEngineStructure.stViewportAttr)->dwWidth)

/* 0x004A71A0 */
static long GAM_pc_g_lSnapShotCounter;

#include "GAM/STRUCTUR/Engmode.h"

void __cdecl IPT_fn_vReadInput(void);

/* 0x0041B110 */
void __cdecl GAM_pc_fn_vTakeSnapShot(void)
{
    unsigned long  ulWidth;
    unsigned long  ulHeight;
    unsigned char *p_ucBuffer;
    char           szFileName[260];

    ulWidth    = IPT_pc_ulEngineViewportWidth;
    ulHeight   = IPT_pc_ulEngineViewportHeight;
    p_ucBuffer = (unsigned char *)malloc(ulHeight * ulWidth * 3);
    if (p_ucBuffer != NULL)
    {
        GLD_vGetFrontBufferIn24BitsAA(GAM_g_stEngineStructure.hGLDDevice,
                                      GAM_g_stEngineStructure.hGLDViewport,
                                      ulWidth, ulHeight, p_ucBuffer);
        do
        {
            sprintf(szFileName, "SnapShot\\Snap%04d.bmp", (int)GAM_pc_g_lSnapShotCounter++);
        } while (FIL_fn_bIsFileExist(szFileName));
        FIL_fn_vSaveBMPInFile(szFileName, (int)p_ucBuffer, ulWidth, ulHeight, 3, 3);
        free(p_ucBuffer);
    }
}

long __cdecl IPT_fn_bIsValidated(IPT_tdstEntryElement *_hEntryElement);
long __cdecl IPT_fn_bIsInvalidated(IPT_tdstEntryElement *_hEntryElement);

void __cdecl IPT_fn_vReadInput(void)
{
    unsigned long          i;
    IPT_tdstEntryElement  *hEntryElement;
    unsigned char          ucIsActionValid;
    unsigned char          ucAnyDeviceHasInput = IPT_C_FALSE;
    long                   lState;

    if (IPT_g_hInputStructure.ucOnePadActivate == IPT_C_ActivateDevice)
    {
        if ((short)INO_fn_wReadAllDevices(INO_C_uwJoystick, INO_C_wConnectedDevices) != 0)
            ucAnyDeviceHasInput = IPT_C_TRUE;
    }

    if (IPT_g_hInputStructure.p_ucValideAndActiveDevice[IPT_E_KeyboardDeviceEvent] == IPT_C_ActivateDevice)
    {
        if (IPT_pc_wPollDevice(IPT_g_hInputStructure.a_hINODevice[IPT_E_KeyboardDeviceEvent]) != 0)
            ucAnyDeviceHasInput = IPT_C_TRUE;
    }

    if (IPT_pc_g_bMouseEnabled != 0 &&
        IPT_g_hInputStructure.p_ucValideAndActiveDevice[IPT_E_MouseDeviceEvent] == IPT_C_ActivateMouseDevice)
    {
        if (IPT_pc_wPollDevice(IPT_g_hInputStructure.a_hINODevice[IPT_E_MouseDeviceEvent]) != 0)
            ucAnyDeviceHasInput = IPT_C_TRUE;
    }

    if (ucAnyDeviceHasInput == IPT_C_TRUE)
    {

        IPT_fn_vActualizeHistoric();
        IPT_g_ulAnyDeviceInputFlag = 0;

        for (i = 0; i < IPT_g_hInputStructure.ulNumberOfEntryElement; i++)
        {
            hEntryElement = &IPT_g_hInputStructure.d_stEntryElementArray[i];
            if (hEntryElement->bIsActivate == IPT_C_TRUE)
            {
                ucIsActionValid = IPT_fn_bIsEntryActionValid(hEntryElement);

                if (ucIsActionValid == IPT_C_TRUE && (unsigned char)IPT_fn_bIsValidated(hEntryElement))
                {
                    lState = hEntryElement->lState + 1;
                    if (lState > 0x7fffffffL)
                        lState = 0x7fffffffL;
                    hEntryElement->lState = lState;
                }
                else if (ucIsActionValid == IPT_C_FALSE && (unsigned char)IPT_fn_bIsInvalidated(hEntryElement))
                {
                    lState = hEntryElement->lState - 1;
                    if (lState < (long)0x80000000L)
                        lState = (long)0x80000000L;
                    hEntryElement->lState = lState;
                }
                else if (ucIsActionValid == IPT_C_FALSE && (unsigned char)IPT_fn_bIsValidated(hEntryElement))
                    hEntryElement->lState = -1;
                else if (ucIsActionValid == IPT_C_TRUE && (unsigned char)IPT_fn_bIsInvalidated(hEntryElement))
                    hEntryElement->lState = 1;
            }
        }
    }
}

long __cdecl IPT_fn_bIsJustValidated(IPT_tdstEntryElement *_hEntryElement)
{
    if (_hEntryElement != NULL && _hEntryElement->lState == 1)
        return IPT_C_TRUE;
    return IPT_C_FALSE;
}

long __cdecl IPT_fn_bIsValidated(IPT_tdstEntryElement *_hEntryElement)
{
    if (_hEntryElement != NULL && _hEntryElement->lState > 0)
        return IPT_C_TRUE;
    return IPT_C_FALSE;
}

long __cdecl IPT_fn_bIsJustInvalidated(IPT_tdstEntryElement *_hEntryElement)
{
    if (_hEntryElement != NULL && _hEntryElement->lState == -1)
        return IPT_C_TRUE;
    return IPT_C_FALSE;
}

long __cdecl IPT_fn_bIsInvalidated(IPT_tdstEntryElement *_hEntryElement)
{

    if (_hEntryElement != NULL && _hEntryElement->lState >= 0)
        return IPT_C_FALSE;
    return IPT_C_TRUE;
}

float __cdecl IPT_fn_lGetAnalogicValue(IPT_tdstEntryElement *_hEntryElement)
{

    if (_hEntryElement != NULL)
        return _hEntryElement->xAnalogicValue;
    return IPT_g_xDefaultAnalogicValue;
}

/* 0x0042EAC0 */
unsigned char __cdecl IPT_fn_bIsEntryActionValid(IPT_tdstEntryElement *_hEntryElement)
{
    unsigned char ucIsValid = IPT_C_FALSE;

    IPT_g_h_ActualEntryInput = _hEntryElement;
    _hEntryElement->xAnalogicValue = 0.0f;

    IPT_g_hNextParameter = _hEntryElement->d_stKeyWordElementArray;

    if (IPT_g_hNextParameter != NULL)
        ucIsValid = IPT_fn_bIsKeyWordValid(IPT_g_hNextParameter);
    return ucIsValid;
}

unsigned char __cdecl IPT_fn_bIsKeyWordValid(void *hKeyWordElement)
{

    unsigned char ucReturn = IPT_C_FALSE;
    IPT_tdfn_bIsKeyWordValid *p_fn_bFunction;

    if ((p_fn_bFunction = IPT_p_fn_bIsKeyWordValid[(*(short *)hKeyWordElement) & 0x00ff]) != NULL)
        ucReturn = p_fn_bFunction(hKeyWordElement);
    else
    {

        ERM_fn_vClearLastError(0);
        ERM_fn_vUpdateLastError(0x0E, (unsigned char)g_ucIPTModuleId,
                                0, 0x0C, -1, 0xFF, 0xFF, 0);
    }
    return ucReturn;
}
