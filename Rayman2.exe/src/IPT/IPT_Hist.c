#include <string.h>
#include "IPT/IPT.h"
#include "INO/INO.h"

#include "IPT/IPT_KDef.h"
#include "INO/Joystick.h"

/* 0x00430DE0 */
static short IPT_pc_fn_wConvertKey(short swKey)
{
    short  sCurKey;
    struct IPT_tdstConvertValue_ *hPair;
    short  sKey = swKey;
    int    iIndex = 0;

    sCurKey = IPT_g_ConvertAZERTYtoQWERTY[0].swAZERTYKeyValue;
    if (sCurKey != -1) {
        hPair = IPT_g_ConvertAZERTYtoQWERTY;
        do {
            if (sCurKey == sKey) {
                break;
            }
            sCurKey = hPair[1].swAZERTYKeyValue;
            hPair++;
            iIndex++;
        } while (sCurKey != -1);
    }
    if (IPT_g_ConvertAZERTYtoQWERTY[iIndex].swAZERTYKeyValue != -1) {
        sKey = IPT_g_ConvertAZERTYtoQWERTY[iIndex].swQWERTYKeyValue;
    }
    return sKey;
}

#define IPT_pc_pfnJoyReadDirs(pDev, n) \
    INO_pc_uwGetHistoricRecordSecondWord((INO_tdstDevice *)(pDev), (short)(n))

#define IPT_pc_pfnJoyReadButtons(pDev, n) \
    INO_pc_ulGetHistoricRecordFirstDword((INO_tdstDevice *)(pDev), (short)(n))

static unsigned char IPT_pc_ucSatInc(unsigned char ucValue)
{
    unsigned int u = (unsigned int)ucValue + 1;
    return (u > 0xFF) ? (unsigned char)0xFF : (unsigned char)u;
}

void __cdecl IPT_fn_vActualizeHistoric(void)
{
    INO_tdstKeyboardState  *pKb;
    short                   wCurrentTab;
    IPT_tdxHandleToScriptValue hKeyList;
    short                   swKey;
    int                     ucFound = IPT_C_FALSE;
    IPT_tdstHistoricElement stElement;
    long                    i;

    pKb         = (INO_tdstKeyboardState *)IPT_g_hInputStructure.a_hINODevice[IPT_E_KeyboardDeviceEvent]->p_stState;
    wCurrentTab = (short)pKb->wCurrentTab;

    hKeyList = IPT_g_hScriptKeyValue;
    swKey    = hKeyList->swKeyValue;
    while (swKey != -1)
    {
        if (pKb->a_ucTabKey[wCurrentTab][swKey] != 0)
        {
            if (pKb->a_ucTabKey[1 - wCurrentTab][swKey] == 0)
            {
                ucFound = IPT_C_TRUE;
                stElement.bf2State      = IPT_E_InProgress;
                stElement.bf6DeviceType = IPT_E_KeyboardDeviceEvent;
                stElement.ucCounter     = 0;

                stElement.swDeviceValue = (IPT_g_hInputStructure.ucKeyboardType == 1)
                                          ? swKey : IPT_pc_fn_wConvertKey(swKey);
                IPT_g_hInputStructure.p_ucKeyboardCounter[swKey] = 0;
            }

        }
        else
        {
            if (pKb->a_ucTabKey[1 - wCurrentTab][swKey] != 0)
                IPT_g_hInputStructure.p_ucKeyboardCounter[swKey] = 0;

        }
        IPT_g_hInputStructure.p_ucKeyboardCounter[swKey] = IPT_pc_ucSatInc(IPT_g_hInputStructure.p_ucKeyboardCounter[swKey]);

        hKeyList++;
        swKey = hKeyList->swKeyValue;
    }

    if (!ucFound && IPT_g_hInputStructure.ucOnePadActivate == IPT_C_ActivateDevice)
    {
        int            iType   = IPT_E_Pad00DeviceEvent;
        struct INO_tdstDevice_ **ppDev = &IPT_g_hInputStructure.a_hINODevice[IPT_E_Pad00DeviceEvent];

        do {
            /* 0x00509EC0 */
            if (ppDev >= &IPT_g_hInputStructure.a_hINODevice[IPT_E_NumberOfDeviceEvent]) break;
            if (IPT_g_hInputStructure.p_ucValideAndActiveDevice[iType] == IPT_C_ActivateJoystickDevice ||
                IPT_g_hInputStructure.p_ucValideAndActiveDevice[iType] == IPT_C_ActivatePadDevice)
            {
                INO_tdstDevice      *hDev     = *ppDev;
                INO_pc_tdstJoyState *pState   = (INO_pc_tdstJoyState *)hDev->p_stState;
                unsigned short       uwStored = pState->uwDirection;
                unsigned short       uwPress  = (unsigned short)(IPT_pc_pfnJoyReadDirs(hDev, 1) & (unsigned short)~uwStored);
                unsigned short       uwRel    = (unsigned short)(~(unsigned short)IPT_pc_pfnJoyReadDirs(hDev, 1) & uwStored);
                int                  iBit;

                static const unsigned short auwDirBit[4] = { C_ulJoyUp, C_ulJoyDown, C_ulJoyLeft, C_ulJoyRight };

                for (iBit = IPT_E_PadUp; iBit <= IPT_E_PadRight; iBit++)
                {
                    if (uwRel & auwDirBit[iBit])
                    {
                        ucFound = IPT_C_TRUE;
                        stElement.bf2State      = IPT_E_InProgress;
                        stElement.bf6DeviceType = (unsigned char)iType;
                        stElement.swDeviceValue = (short)iBit;
                        stElement.ucCounter     = 0;
                        IPT_g_hInputStructure.p_ucPadAndJoyCounter[iType - IPT_E_Pad00DeviceEvent][iBit] = 0;
                    }
                    else if (uwPress & auwDirBit[iBit])
                        IPT_g_hInputStructure.p_ucPadAndJoyCounter[iType - IPT_E_Pad00DeviceEvent][iBit] = 0;
                    IPT_g_hInputStructure.p_ucPadAndJoyCounter[iType - IPT_E_Pad00DeviceEvent][iBit] =
                        IPT_pc_ucSatInc(IPT_g_hInputStructure.p_ucPadAndJoyCounter[iType - IPT_E_Pad00DeviceEvent][iBit]);
                }

                if (!ucFound)
                {
                    long          j;
                    unsigned long ulButtons = pState->ulButtons;

                    for (j = IPT_E_PadFirstButton; !ucFound && j < IPT_E_JoyLastButton; j++)
                    {
                        unsigned long ulFlag = IPT_g_ulPadAndJoyButtonFlag[j - IPT_E_PadFirstButton];
                        if ((ulFlag & ulButtons) == ulFlag &&
                            (IPT_pc_pfnJoyReadButtons(hDev, 1) & ulFlag) != ulFlag)
                        {
                            ucFound = IPT_C_TRUE;
                            stElement.bf2State      = IPT_E_InProgress;
                            stElement.bf6DeviceType = (unsigned char)iType;
                            stElement.swDeviceValue = IPT_g_hScriptPadValue[j].swKeyValue;
                            stElement.ucCounter     = 0;
                            IPT_g_hInputStructure.p_ucPadAndJoyCounter[iType - IPT_E_Pad00DeviceEvent][IPT_g_hScriptPadValue[j].swKeyValue] = 0;
                        }
                        else if ((ulFlag & ulButtons) != ulFlag &&
                                 (IPT_pc_pfnJoyReadButtons(hDev, 1) & ulFlag) == ulFlag)
                            IPT_g_hInputStructure.p_ucPadAndJoyCounter[iType - IPT_E_Pad00DeviceEvent][IPT_g_hScriptPadValue[j].swKeyValue] = 0;
                        IPT_g_hInputStructure.p_ucPadAndJoyCounter[iType - IPT_E_Pad00DeviceEvent][IPT_g_hScriptPadValue[j].swKeyValue] =
                            IPT_pc_ucSatInc(IPT_g_hInputStructure.p_ucPadAndJoyCounter[iType - IPT_E_Pad00DeviceEvent][IPT_g_hScriptPadValue[j].swKeyValue]);
                    }
                }
            }
            iType++;
            ppDev++;
        } while (!ucFound);
    }

    if (ucFound)
    {
        memmove(&IPT_g_hInputStructure.hEventHistoric[1], &IPT_g_hInputStructure.hEventHistoric[0],
                (IPT_g_hInputStructure.sEventHistoricSize - 1) * sizeof(IPT_tdstHistoricElement));
        IPT_g_hInputStructure.hEventHistoric[0] = stElement;
    }

    for (i = 0; i < IPT_g_hInputStructure.sEventHistoricSize; i++)
    {
        IPT_tdstHistoricElement *pEl   = &IPT_g_hInputStructure.hEventHistoric[i];
        unsigned char            ucType = pEl->bf6DeviceType;
        int                      bActive = 0;

        if (ucType == IPT_E_KeyboardDeviceEvent && pEl->bf2State == IPT_E_InProgress)
        {
            INO_tdstKeyboardState *pKbState = (INO_tdstKeyboardState *)IPT_g_hInputStructure.a_hINODevice[IPT_E_KeyboardDeviceEvent]->p_stState;
            short                  wTab      = (short)pKbState->wCurrentTab;
            if (pKbState->a_ucTabKey[wTab][pEl->swDeviceValue] != 0)
                bActive = 1;
        }
        else if (ucType >= IPT_E_Pad00DeviceEvent && ucType <= IPT_E_Pad15DeviceEvent &&
                 pEl->bf2State == IPT_E_InProgress)
        {
            INO_pc_tdstJoyState *pDevSt = (INO_pc_tdstJoyState *)IPT_g_hInputStructure.a_hINODevice[ucType]->p_stState;
            short                swAct  = pEl->swDeviceValue;
            if      (swAct == IPT_E_PadUp    && (pDevSt->uwDirection & C_ulJoyUp))    bActive = 1;
            else if (swAct == IPT_E_PadDown  && (pDevSt->uwDirection & C_ulJoyDown))  bActive = 1;
            else if (swAct == IPT_E_PadLeft  && (pDevSt->uwDirection & C_ulJoyLeft))  bActive = 1;
            else if (swAct == IPT_E_PadRight && (pDevSt->uwDirection & C_ulJoyRight)) bActive = 1;
            else if (swAct >= IPT_E_PadFirstButton && swAct <= IPT_E_PadLastButton)
            {
                unsigned long ulFlag = IPT_g_ulPadAndJoyButtonFlag[swAct - IPT_E_PadFirstButton];
                if ((pDevSt->ulButtons & ulFlag) == ulFlag) bActive = 1;
            }
        }

        if (bActive)
            pEl->ucCounter = IPT_pc_ucSatInc(pEl->ucCounter);
        else if (pEl->bf2State == IPT_E_InProgress)
            pEl->bf2State = IPT_E_Stopping;
        else if (pEl->bf2State == IPT_E_Stopping)
            pEl->bf2State = IPT_E_Stopped;
    }
}
