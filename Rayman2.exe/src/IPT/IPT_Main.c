#include "IPT/IPT.h"
#include "INO/INO.h"
#include "IPT/IPT_KDef.h"
#include "ERM/CpaError.h"
#include "MMG/MMG_alloc.h"
#include "MMG/MMG_globals.h"

extern void __cdecl Mmg_fn_cCheckAlignmentValidity(void);
extern short __cdecl INO_fn_wDeactivateDevice(INO_tdhDevice hDev);

struct IPT_tdstConvertValue_ IPT_g_ConvertAZERTYtoQWERTY[] =   /* 0x004A0D00 */
{
    16, 30,
    17, 44,
    30, 16,
    39, 51,
    44, 17,
    50, 39,
    51, 50,
    -1, -1
};

struct IPT_tdstScriptValue_ IPT_g_hScriptKeyValue[] =          /* 0x004A0D20 */
{
    41, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 16, 17, 18, 19, 20, 21, 22,
    23, 24, 25, 26, 27, 30, 31, 32, 33, 34, 35, 36, 37, 38, 39, 40, 43, 86, 44, 45,
    46, 47, 48, 49, 50, 51, 52, 53, 59, 60, 61, 62, 63, 64, 65, 66, 67, 68, 87, 88,
    183, 70, 105, 57, 1, 15, 58, 42, 54, 29, 157, 56, 184, 28, 14, 210, 211, 199, 207, 201,
    209, 200, 208, 203, 205, 197, 181, 55, 74, 78, 83, 82, 79, 80, 81, 75, 76, 77, 71, 72,
    73, 156, -1
};

struct IPT_tdstScriptValue_ IPT_g_hScriptPadValue[] =          /* 0x004A0DF0 */
{
    IPT_E_PadUp, IPT_E_PadDown, IPT_E_PadLeft, IPT_E_PadRight,
    IPT_E_JoyXValue, IPT_E_JoyYValue, IPT_E_JoyZValue, IPT_E_JoyRValue, IPT_E_JoyUValue, IPT_E_JoyVValue,
    IPT_E_PadButton0, IPT_E_PadButton1, IPT_E_PadButton2, IPT_E_PadButton3,
    IPT_E_PadButton4, IPT_E_PadButton5, IPT_E_PadButton6, IPT_E_PadButton7,
    IPT_E_PadButton8, IPT_E_PadButton9, IPT_E_PadButton10, IPT_E_PadButton11,
    IPT_E_PadButton12, IPT_E_PadButton13, IPT_E_PadButton14, IPT_E_PadButton15,
    IPT_E_PadButton16, IPT_E_PadButton17, IPT_E_PadButton18, IPT_E_PadButton19,
    IPT_E_PadButton20, IPT_E_PadButton21, IPT_E_PadButton22, IPT_E_PadButton23,
    IPT_E_PadButton24, IPT_E_PadButton25, IPT_E_PadButton26, IPT_E_PadButton27,
    IPT_E_PadButton28, IPT_E_PadButton29, IPT_E_PadButton30, IPT_E_PadButton31,
    IPT_E_JoyButton0, IPT_E_JoyButton1, IPT_E_JoyButton2, IPT_E_JoyButton3,
    IPT_E_JoyButton4, IPT_E_JoyButton5, IPT_E_JoyButton6, IPT_E_JoyButton7,
    IPT_E_JoyButton8, IPT_E_JoyButton9, IPT_E_JoyButton10, IPT_E_JoyButton11,
    IPT_E_JoyButton12, IPT_E_JoyButton13, IPT_E_JoyButton14, IPT_E_JoyButton15,
    IPT_E_JoyButton16, IPT_E_JoyButton17, IPT_E_JoyButton18, IPT_E_JoyButton19,
    IPT_E_JoyButton20, IPT_E_JoyButton21, IPT_E_JoyButton22, IPT_E_JoyButton23,
    IPT_E_JoyButton24, IPT_E_JoyButton25, IPT_E_JoyButton26, IPT_E_JoyButton27,
    IPT_E_JoyButton28, IPT_E_JoyButton29, IPT_E_JoyButton30, IPT_E_JoyButton31,
    0, 1, 2, 3, 4, 5, 6, 7, 8,
    -1
};

unsigned long IPT_g_ulPadAndJoyButtonFlag[] =                   /* 0x004A0E98 */
{
    0x00000001, 0x00000002, 0x00000040, 0x00000080, 0x00000010, 0x00000020, 0x00000004, 0x00000008,
    0x00000100, 0x00000200, 0x00000400, 0x00000800, 0x00001000, 0x00002000, 0x00004000, 0x00008000,
    0x00010000, 0x00020000, 0x00040000, 0x00080000, 0x00100000, 0x00200000, 0x00400000, 0x00800000,
    0x01000000, 0x02000000, 0x04000000, 0x08000000, 0x10000000, 0x20000000, 0x40000000, 0x80000000
};

IPT_tdfn_bIsKeyWordValid *IPT_p_fn_bIsKeyWordValid[38] =
{
    /* 0x004A0F18 */
    NULL,

    IPT_fn_bIsKeyWordValid_And,                       /* 0x0042EB50 */
    IPT_fn_bIsKeyWordValid_Or,                        /* 0x0042EBA0 */
    IPT_fn_bIsKeyWordValid_Not,                       /* 0x0042EBF0 */

    IPT_fn_bIsKeyWordValid_KeyJustPressed,            /* 0x0042EC10 */
    IPT_fn_bIsKeyWordValid_KeyJustReleased,           /* 0x0042EC80 */
    IPT_fn_bIsKeyWordValid_KeyPressed,                /* 0x0042ECE0 */
    IPT_fn_bIsKeyWordValid_KeyReleased,               /* 0x0042ED50 */

    IPT_fn_bIsKeyWordValid_ActionJustValidated,       /* 0x00430130 */
    IPT_fn_bIsKeyWordValid_ActionJustInvalidated,     /* 0x00430190 */
    IPT_fn_bIsKeyWordValid_ActionValidated,           /* 0x00430160 */
    IPT_fn_bIsKeyWordValid_ActionInvalidated,         /* 0x004301C0 */

    IPT_fn_bIsKeyWordValid_PadJustPressed,            /* 0x0042EDB0 */
    IPT_fn_bIsKeyWordValid_PadJustReleased,           /* 0x0042EEB0 */
    IPT_fn_bIsKeyWordValid_PadPressed,                /* 0x0042EFE0 */
    IPT_fn_bIsKeyWordValid_PadReleased,               /* 0x0042F120 */

    IPT_fn_bIsKeyWordValid_JoystickAxeValue,          /* 0x0042F270 */
    IPT_fn_bIsKeyWordValid_JoystickAngularValue,      /* 0x0042F390 */
    IPT_fn_bIsKeyWordValid_JoystickTrueNormValue,     /* 0x0042F590 */
    IPT_fn_bIsKeyWordValid_JoystickCorrectedNormValue,/* 0x0042F750 */
    IPT_fn_bIsKeyWordValid_JoystickJustPressed,       /* 0x0042F9A0 */
    IPT_fn_bIsKeyWordValid_JoystickJustReleased,      /* 0x0042FA60 */
    IPT_fn_bIsKeyWordValid_JoystickPressed,           /* 0x0042FB00 */
    IPT_fn_bIsKeyWordValid_JoystickReleased,          /* 0x0042FBB0 */

    IPT_fn_bIsKeyWordValid_JoystickOrPadJustPressed,  /* 0x0042FC40 */
    IPT_fn_bIsKeyWordValid_JoystickOrPadJustReleased, /* 0x0042FD50 */
    IPT_fn_bIsKeyWordValid_JoystickOrPadPressed,      /* 0x0042FE80 */
    IPT_fn_bIsKeyWordValid_JoystickOrPadReleased,     /* 0x0042FFD0 */

    IPT_fn_bIsKeyWordValid_MouseAxeValue,             /* 0x00430460 */
    IPT_fn_bIsKeyWordValid_MouseAxePosition,          /* 0x00430540 */
    IPT_fn_bIsKeyWordValid_MouseJustPressed,          /* 0x004305E0 */
    IPT_fn_bIsKeyWordValid_MouseJustReleased,         /* 0x00430600 */
    IPT_fn_bIsKeyWordValid_MousePressed,              /* 0x00430790 */

    IPT_fn_bIsKeyWordValid_Sequence,                  /* 0x004301F0 */
    IPT_fn_bIsKeyWordValid_SequenceKey,               /* 0x00430240 */
    IPT_fn_bIsKeyWordValid_SequenceKeyEnd,            /* 0x004302C0 */
    IPT_fn_bIsKeyWordValid_SequencePad,               /* 0x00430340 */
    IPT_fn_bIsKeyWordValid_SequencePadEnd             /* 0x004303D0 */
};

#define IPT_E_KeyWord_ActionValidated  ((short)0xf00a)

#define IPT_E_KeyWord_KeyPressed       ((short)0xf006)

#define IPT_pc_E_KeyWord_JoystickOrPadPressed ((short)0xf01a)

#define IPT_C_Duplicate   0
#define IPT_C_Exchange    1
#define IPT_C_NoDuplicate 2

#define IPT_pc_M_pKbState()           ((INO_tdstKeyboardState *)((INO_tdstDevice *)IPT_g_hInputStructure.a_hINODevice[IPT_E_KeyboardDeviceEvent])->p_stState)
#define IPT_pc_M_bKeyJustPressed(swKey)                                                     \
    (IPT_pc_M_pKbState()->a_ucTabKey[IPT_pc_M_pKbState()->wCurrentTab][swKey]                \
     && !IPT_pc_M_pKbState()->a_ucTabKey[1 - IPT_pc_M_pKbState()->wCurrentTab][swKey])

#define IPT_pc_M_bJoystickButtonJustPressed(hDevice, ulFlag)                                       \
    ((((INO_pc_tdstJoyState *)(hDevice)->p_stState)->ulButtons & (ulFlag)) == (ulFlag)             \
     && (INO_pc_ulGetHistoricRecordFirstDword((hDevice), 1) & (ulFlag)) != (ulFlag))

#define IPT_pc_ulMemSizeStash (IPT_g_hInputStructure.hEntryElement_lNumberOfElementsDyn)

/* 0x004B9B68 */
typedef struct IPT_pc_tdstVector_
{
    float xX;
    float xY;
    float xZ;
} IPT_pc_tdstVector;

IPT_pc_tdstVector IPT_g_FieldPadGlobalVector;
short IPT_g_FieldPadHorizontalAxis;
short IPT_g_FieldPadVerticalAxis;
float IPT_g_FieldPadAnalogForce;
float IPT_g_FieldPadTrueAnalogForce;
float IPT_g_FieldPadRotationAngle;
int   IPT_g_FieldPadSector;

typedef union IPT_pc_tduGetSetParam_
{
    long           lValue;
    short          wValue;
    float          xValue;
    IPT_pc_tdstVector stVectorValue;
    long              alVectorValue[3];
} IPT_pc_tduGetSetParam;

/* 0x0042DE50 */
void __cdecl IPT_fn_vFirstInitInput(void)
{
    if (g_ucIPTModuleId == 0xff)
        g_ucIPTModuleId = Erm_fn_ucInitErrMsg();
    Mmg_fn_v_InitMmg(g_ucIPTModuleId, 1, 0x186a0);
    Mmg_fn_cCheckAlignmentValidity();
}

/* 0x0042DE80 */
void __cdecl IPT_fn_vInitMemory(unsigned long _ulMemSize)
{
    IPT_pc_ulMemSizeStash = _ulMemSize;

    Mmg_fn_vInitSpecificBlock(0, (unsigned char)*(unsigned long *)&g_ucIPTModuleId,
                              g_a_p_stMmgModuleBlocksInfo[(unsigned char)*(unsigned long *)&g_ucIPTModuleId],
                              _ulMemSize, 0x186a0, 4, 0xd);
}

void __cdecl IPT_fn_vResetInputEntry(void)
{
    unsigned long i;

    for (i = 0; i < IPT_g_hInputStructure.ulNumberOfEntryElement; i++)
    {
        IPT_g_hInputStructure.d_stEntryElementArray[i].lState = -2;
    }
}

/* 0x0042E110 */
char *__cdecl IPT_fn_szGetEntryActionName(IPT_tdstEntryElement *_hEntryElement)
{
    if (_hEntryElement != (IPT_tdstEntryElement *)0)
        return _hEntryElement->p_szEntryName;
    return (char *)0;
}

/* 0x0042E120 */
void __cdecl IPT_fn_vActivateAllEntryElements(void)
{
    IPT_tdstEntryElement *hEntryElement;
    unsigned long         ulI;

    for (ulI = 0, hEntryElement = IPT_g_hInputStructure.d_stEntryElementArray;
         ulI < IPT_g_hInputStructure.ulNumberOfEntryElement;
         ulI++, hEntryElement++)
    {
        IPT_fn_vActivateEntryElement(hEntryElement);
    }
}

/* 0x0042E150 */
void __cdecl FUN_0042e150(void)
{
    IPT_tdstEntryElement *hEntryElement;
    unsigned long         ulI;

    for (ulI = 0, hEntryElement = IPT_g_hInputStructure.d_stEntryElementArray;
         ulI < IPT_g_hInputStructure.ulNumberOfEntryElement;
         ulI++, hEntryElement++)
    {
        IPT_fn_vDesactivateEntryElement(hEntryElement);
    }
}

/* 0x0042E180 */
void __cdecl IPT_fn_vActivateEntryElement(IPT_tdstEntryElement *_hEntryElement)
{
    if (_hEntryElement != (IPT_tdstEntryElement *)0)
    {
        _hEntryElement->bIsActivate = IPT_C_TRUE;
        _hEntryElement->lState      = -2;
    }
}

/* 0x0042E1A0 */
void __cdecl IPT_fn_vDesactivateEntryElement(IPT_tdstEntryElement *_hEntryElement)
{
    if (_hEntryElement != (IPT_tdstEntryElement *)0)
    {
        _hEntryElement->bIsActivate = IPT_C_FALSE;
        _hEntryElement->lState      = -2;
    }
}

typedef struct IPT_pc_tdstGetSetParam_
{
    IPT_pc_tduGetSetParam uParam;
    int                   eType;
} IPT_pc_tdstGetSetParam;

/* 0x0042E1C0 */
unsigned char __cdecl IPT_fn_bAJoypadConnected(void)
{
    const unsigned char *a_ucValideAndActiveDevice = IPT_g_hInputStructure.p_ucValideAndActiveDevice;
    unsigned char        ucConnected = 0;
    unsigned int         uId         = 2;

    do {
        if (a_ucValideAndActiveDevice[uId] == IPT_C_JoyKeywordDeviceActive)
            ucConnected = 1;
        uId++;
    } while (uId <= 0x11);

    return ucConnected;
}

/* 0x0042E1E0 */
void IPT_fn_vDesactiveJoystickAndPadControl(void)
{
	unsigned long i;
	for (i=IPT_E_Pad00DeviceEvent;i<=IPT_E_Pad15DeviceEvent;i++)
	{
		if (  IPT_g_hInputStructure.p_ucValideAndActiveDevice[i]==IPT_C_ActivatePadDevice
			||IPT_g_hInputStructure.p_ucValideAndActiveDevice[i]==IPT_C_ActivateJoystickDevice)
		{
			IPT_g_hInputStructure.p_ucValideAndActiveDevice[i]=IPT_C_ValideDevice;
			INO_fn_wDeactivateDevice((INO_tdhDevice)IPT_g_hInputStructure.a_hINODevice[i]);
		}
	}
}

/* 0x0042E220 */
void IPT_fn_vActiveJoystickControl(void)
{
	unsigned long i;
	for (i=IPT_E_Pad00DeviceEvent;i<=IPT_E_Pad15DeviceEvent;i++)
	{
		if (IPT_g_hInputStructure.p_ucValideAndActiveDevice[i]==IPT_C_ValideDevice)
		{
			IPT_g_hInputStructure.p_ucValideAndActiveDevice[i]=IPT_C_ActivateJoystickDevice;
			INO_fn_wActivateDevice((INO_tdhDevice)IPT_g_hInputStructure.a_hINODevice[i]);
		}
	}
}

/* 0x0042E260 */
IPT_tdstEntryElement * __cdecl IPT_fn_hSearchElementInOptionWithKeyWord(signed short _swEntryType, signed short swPadNumber, signed short _swKeyValue)
{
    unsigned long i;
    IPT_tdstEntryElement *hEntryElement;
    IPT_tdstEntryElement *hAction = (IPT_tdstEntryElement *)0;
    IPT_tdstKeyWordElement *hNextElement;

    for (i = 0; i < IPT_g_hInputStructure.ulNumberOfEntryElement; i++)
    {
        hEntryElement = &IPT_g_hInputStructure.d_stEntryElementArray[i];
        if (hEntryElement->p_szEntryName != (char *)0)
        {
            if (_swEntryType == IPT_E_KeyWord_KeyPressed)
            {
                hNextElement = &(hEntryElement->d_stKeyWordElementArray)[0];
                if (hNextElement->u_ElementUnion.swValue == IPT_E_KeyWord_KeyPressed)
                {
                    hNextElement = &(hEntryElement->d_stKeyWordElementArray)[1];
                    if (hNextElement->u_ElementUnion.swValue == _swKeyValue)
                        hAction = hEntryElement;
                }
            }
            else if (_swEntryType == IPT_pc_E_KeyWord_JoystickOrPadPressed)
            {
                hNextElement = &(hEntryElement->d_stKeyWordElementArray)[0];
                if (hNextElement->u_ElementUnion.swValue == IPT_pc_E_KeyWord_JoystickOrPadPressed)
                {
                    hNextElement = &(hEntryElement->d_stKeyWordElementArray)[1];
                    if (hNextElement->u_ElementUnion.swValue == swPadNumber)
                    {
                        hNextElement = &(hEntryElement->d_stKeyWordElementArray)[2];
                        if (hNextElement->u_ElementUnion.swValue == _swKeyValue)
                            hAction = hEntryElement;
                    }
                }
            }
        }
    }
    return hAction;
}

/* 0x0042E2D0 */
unsigned char __cdecl IPT_fn_bModifyIPTAction(IPT_tdstEntryElement *_hElementToChange,
                                              IPT_tdstEntryElement *_hReplacingElement)
{
    long lI;

    for (lI = 0; lI < (long)_hElementToChange->ulNumberOfKeyWordElement; lI++)
        if (_hElementToChange->d_stKeyWordElementArray[lI].u_ElementUnion.swValue == IPT_E_KeyWord_ActionValidated)
        {
            _hElementToChange->d_stKeyWordElementArray[lI + 1].u_ElementUnion.hEntryElement = _hReplacingElement;
            return IPT_C_TRUE;
        }
    return IPT_C_FALSE;
}

/* 0x0042E310 */
unsigned char __cdecl IPT_fn_bChangeIPTValue(IPT_tdstEntryElement *_hElement, unsigned char ucWhichTest)
{
    signed short swPadNumber;
    unsigned char ucReturn = IPT_C_FALSE;
    unsigned long i;
    unsigned char bFlag = FALSE;
    INO_tdstDevice *hDevice;

    IPT_tdstEntryElement *hExchangeEntry;
    IPT_tdstKeyWordElement *hExchangeNextElement;
    IPT_tdstKeyWordElement *hNextElement;

    hNextElement = &(_hElement->d_stKeyWordElementArray)[0];
    if (hNextElement->u_ElementUnion.swValue == IPT_E_KeyWord_KeyPressed)
    {
        for (i = 0; IPT_g_hScriptKeyValue[i].swKeyValue != -1 && !IPT_pc_M_bKeyJustPressed(IPT_g_hScriptKeyValue[i].swKeyValue); i++);
        if (IPT_g_hScriptKeyValue[i].swKeyValue != -1
            && IPT_fn_p_szGetNameOfKeyName(IPT_g_hScriptKeyValue[i].swKeyValue, 0) != NULL)
        {
            hExchangeEntry = IPT_fn_hSearchElementInOptionWithKeyWord(IPT_E_KeyWord_KeyPressed, 0, IPT_g_hScriptKeyValue[i].swKeyValue);
            if (ucWhichTest == IPT_C_Duplicate)
                bFlag = TRUE;
            else if (ucWhichTest == IPT_C_NoDuplicate)
            {
                if (hExchangeEntry == NULL || hExchangeEntry == _hElement)
                    bFlag = TRUE;
            }
            else if (ucWhichTest == IPT_C_Exchange)
            {
                bFlag = TRUE;
                if (hExchangeEntry != NULL && hExchangeEntry != _hElement)
                {
                    hExchangeNextElement = &(hExchangeEntry->d_stKeyWordElementArray)[1];
                    hExchangeNextElement->u_ElementUnion.swValue = hNextElement[1].u_ElementUnion.swValue;
                }
            }
            if (bFlag)
            {
                hNextElement[1].u_ElementUnion.swValue = IPT_g_hScriptKeyValue[i].swKeyValue;
                return IPT_C_TRUE;
            }
        }
    }
    else if (hNextElement->u_ElementUnion.swValue == IPT_pc_E_KeyWord_JoystickOrPadPressed)
    {
        hNextElement++;
        swPadNumber = hNextElement->u_ElementUnion.swValue;
        hNextElement++;
        hDevice = IPT_g_hInputStructure.a_hINODevice[IPT_E_Pad00DeviceEvent+swPadNumber];
        for (i = IPT_E_PadFirstButton; i <= IPT_E_PadLastButton && !IPT_pc_M_bJoystickButtonJustPressed(hDevice, IPT_g_ulPadAndJoyButtonFlag[i - IPT_E_PadFirstButton]); i++);
        if (i <= IPT_E_PadLastButton)
        {
            hExchangeEntry = IPT_fn_hSearchElementInOptionWithKeyWord(IPT_pc_E_KeyWord_JoystickOrPadPressed, swPadNumber, IPT_g_hScriptPadValue[i].swKeyValue);
            if (ucWhichTest == IPT_C_Duplicate)
                bFlag = TRUE;
            else if (ucWhichTest == IPT_C_NoDuplicate)
            {
                if (hExchangeEntry == NULL || hExchangeEntry == _hElement)
                    bFlag = TRUE;
            }
            else if (ucWhichTest == IPT_C_Exchange)
            {
                bFlag = TRUE;
                if (hExchangeEntry != NULL && hExchangeEntry != _hElement)
                {
                    hExchangeNextElement = &(hExchangeEntry->d_stKeyWordElementArray)[2];
                    hExchangeNextElement->u_ElementUnion.swValue = hNextElement->u_ElementUnion.swValue;
                }
            }
            if (bFlag)
            {
                hNextElement->u_ElementUnion.swValue = IPT_g_hScriptPadValue[i].swKeyValue;
                ucReturn = IPT_C_TRUE;
            }
        }
    }

    return (ucReturn);
}

/* 0x0042E540 */
char *__cdecl FUN_0042E540(long lHandle)
{
    IPT_tdstKeyWordElement *p_stKeyWord;
    unsigned short          uwType;
    unsigned char           ucSelector;

    p_stKeyWord = ((IPT_tdstEntryElement *)lHandle)->d_stKeyWordElementArray;
    uwType = (unsigned short)p_stKeyWord->u_ElementUnion.swValue;
    if (uwType == 0xf006)
    {
        ucSelector = IPT_g_hInputStructure.ucKeyboardType;
        p_stKeyWord++;
        if (ucSelector == 1)
            return IPT_fn_p_szGetNameOfKeyName((unsigned short)p_stKeyWord->u_ElementUnion.swValue, 0);
        return IPT_fn_p_szGetNameOfKeyName((unsigned short)p_stKeyWord->u_ElementUnion.swValue, 1);
    }
    if (uwType == 0xf01a)
        return IPT_fn_p_szGetNameOfKeyName((unsigned short)(p_stKeyWord[2].u_ElementUnion.swValue | 0x8000), 0);
    return (char *)p_stKeyWord;
}

/* 0x0042E5A0 */
char *__cdecl IPT_fn_p_szGetNameOfKeyName(unsigned short uwKeyCode, unsigned char ucFrenchOrAmerican)
{
    IPT_tdstKeyAndPadDefine *hDefine;
    unsigned long            ulI;

    hDefine = IPT_gd_hKeyAndPadDefine;
    ulI = 0;
    if (hDefine[0].uwBasedKey != 0xffff)
    {
        do {
            if (hDefine[ulI].uwBasedKey == uwKeyCode)
                break;
            ulI++;
        } while (hDefine[ulI].uwBasedKey != 0xffff);
    }
    if (hDefine[ulI].uwBasedKey == 0xffff)
        return (char *)0;
    if (ucFrenchOrAmerican == 0)
        return hDefine[ulI].p_szFrenchKey;
    if (ucFrenchOrAmerican == 1)
        return hDefine[ulI].p_szAmericanKey;
    return (char *)0;
}

/* 0x0042E600 */
unsigned char __cdecl IPT_fn_bAKeyJustPressed(void)
{
    INO_tdstKeyboardState   *pState;
    unsigned char           *pTabDown;
    unsigned char           *pTabUp;
    IPT_tdxHandleToScriptValue hKeyList;
    short                    swTab;
    IPT_tdstKeyAndPadDefine *hDefine;

    hKeyList = IPT_g_hScriptKeyValue;
    if (hKeyList->swKeyValue == -1)
        return IPT_C_FALSE;
    pState   = (INO_tdstKeyboardState *)((INO_tdstDevice *)IPT_g_hInputStructure.a_hINODevice[IPT_E_KeyboardDeviceEvent])->p_stState;
    swTab    = (short)pState->wCurrentTab;
    pTabDown = pState->a_ucTabKey[swTab];
    pTabUp   = pState->a_ucTabKey[1 - swTab];
    do
    {
        if (pTabDown[hKeyList->swKeyValue] != 0 && pTabUp[hKeyList->swKeyValue] == 0)
        {
            for (hDefine = IPT_gd_hKeyAndPadDefine; hDefine->uwBasedKey != 0xffff; hDefine++)
            {
                if (hDefine->uwBasedKey == (unsigned short)hKeyList->swKeyValue)
                    return IPT_C_TRUE;
            }
        }
        hKeyList++;
    } while (hKeyList->swKeyValue != -1);
    return IPT_C_FALSE;
}

/* 0x0042E690 */
unsigned char __cdecl IPT_fn_bAButtonPadJustPressed(void)
{
    unsigned long   ulId;
    long            lButton;
    unsigned long   ulFlag;
    INO_tdstDevice **ppDevice;
    INO_tdstDevice  *pDevice;
    INO_pc_tdstJoyState *pState;

    ulId     = IPT_E_Pad00DeviceEvent;
    if (IPT_g_hInputStructure.ucOnePadActivate != IPT_C_ActivateDevice)
        return IPT_C_FALSE;

    ppDevice = &IPT_g_hInputStructure.a_hINODevice[IPT_E_Pad00DeviceEvent];
    do
    {
        pDevice  = *ppDevice;

        lButton = IPT_E_PadFirstButton;
        do
        {
            pState = (INO_pc_tdstJoyState *)pDevice->p_stState;
            if ((pState->ulButtons & IPT_g_ulPadAndJoyButtonFlag[ulId - IPT_E_PadFirstButton]) == IPT_g_ulPadAndJoyButtonFlag[ulId - IPT_E_PadFirstButton] &&
                (INO_pc_ulGetHistoricRecordFirstDword(pDevice, 1) & IPT_g_ulPadAndJoyButtonFlag[ulId - IPT_E_PadFirstButton]) != IPT_g_ulPadAndJoyButtonFlag[ulId - IPT_E_PadFirstButton])
                return IPT_C_TRUE;
        } while (++lButton <= IPT_E_PadLastButton);
        ulId++;
        ppDevice++;
    } while ((long)ppDevice < (long)&IPT_g_hInputStructure.a_hINODevice[IPT_E_NumberOfDeviceEvent]);
    return IPT_C_FALSE;
}

/* 0x0048D110 */
unsigned char IPT_pc_fn_ucGetFieldPadGlobalVector(void *hSuperObject, IPT_pc_tdstGetSetParam *p_stValue)
{
    p_stValue->uParam.alVectorValue[0] = *(long *)&IPT_g_FieldPadGlobalVector.xX;
    p_stValue->uParam.alVectorValue[1] = *(long *)&IPT_g_FieldPadGlobalVector.xY;
    p_stValue->uParam.alVectorValue[2] = *(long *)&IPT_g_FieldPadGlobalVector.xZ;
    p_stValue->eType = 0x15;
    return 1;
}

/* 0x0048D140 */
unsigned char IPT_pc_fn_ucSetFieldPadGlobalVector(void *hSuperObject, IPT_pc_tdstGetSetParam *p_stValue)
{
    IPT_g_FieldPadGlobalVector = p_stValue->uParam.stVectorValue;
    return 1;
}

/* 0x0048D160 */
unsigned char IPT_pc_fn_ucGetFieldPadHorizontalAxis(void *hSuperObject, IPT_pc_tdstGetSetParam *p_stValue)
{
    p_stValue->uParam.lValue = IPT_g_FieldPadHorizontalAxis;
    p_stValue->eType = 6;
    return 1;
}

/* 0x0048D180 */
unsigned char IPT_pc_fn_ucSetFieldPadHorizontalAxis(void *hSuperObject, IPT_pc_tdstGetSetParam *p_stValue)
{
    IPT_g_FieldPadHorizontalAxis = p_stValue->uParam.wValue;
    return 1;
}

/* 0x0048D1A0 */
unsigned char IPT_pc_fn_ucGetFieldPadVerticalAxis(void *hSuperObject, IPT_pc_tdstGetSetParam *p_stValue)
{
    p_stValue->uParam.lValue = IPT_g_FieldPadVerticalAxis;
    p_stValue->eType = 6;
    return 1;
}

/* 0x0048D1C0 */
unsigned char IPT_pc_fn_ucSetFieldPadVerticalAxis(void *hSuperObject, IPT_pc_tdstGetSetParam *p_stValue)
{
    IPT_g_FieldPadVerticalAxis = p_stValue->uParam.wValue;
    return 1;
}

/* 0x0048D1E0 */
unsigned char IPT_pc_fn_ucGetFieldPadAnalogForce(void *hSuperObject, IPT_pc_tdstGetSetParam *p_stValue)
{
    p_stValue->uParam.xValue = IPT_g_FieldPadAnalogForce;
    p_stValue->eType = 8;
    return 1;
}

/* 0x0048D200 */
unsigned char IPT_pc_fn_ucSetFieldPadAnalogForce(void *hSuperObject, IPT_pc_tdstGetSetParam *p_stValue)
{
    IPT_g_FieldPadAnalogForce = p_stValue->uParam.xValue;
    return 1;
}

/* 0x0048D210 */
unsigned char IPT_pc_fn_ucGetFieldPadTrueAnalogForce(void *hSuperObject, IPT_pc_tdstGetSetParam *p_stValue)
{
    p_stValue->uParam.xValue = IPT_g_FieldPadTrueAnalogForce;
    p_stValue->eType = 8;
    return 1;
}

/* 0x0048D230 */
unsigned char IPT_pc_fn_ucSetFieldPadTrueAnalogForce(void *hSuperObject, IPT_pc_tdstGetSetParam *p_stValue)
{
    IPT_g_FieldPadTrueAnalogForce = p_stValue->uParam.xValue;
    return 1;
}

/* 0x0048D240 */
unsigned char IPT_pc_fn_ucGetFieldPadRotationAngle(void *hSuperObject, IPT_pc_tdstGetSetParam *p_stValue)
{
    p_stValue->uParam.xValue = IPT_g_FieldPadRotationAngle;
    p_stValue->eType = 8;
    return 1;
}

/* 0x0048D260 */
unsigned char IPT_pc_fn_ucSetFieldPadRotationAngle(void *hSuperObject, IPT_pc_tdstGetSetParam *p_stValue)
{
    IPT_g_FieldPadRotationAngle = p_stValue->uParam.xValue;
    return 1;
}

/* 0x0048D270 */
unsigned char IPT_pc_fn_ucGetFieldPadSector(void *hSuperObject, IPT_pc_tdstGetSetParam *p_stValue)
{
    p_stValue->uParam.lValue = IPT_g_FieldPadSector;
    p_stValue->eType = 6;
    return 1;
}

/* 0x0048D290 */
unsigned char IPT_pc_fn_ucSetFieldPadSector(void *hSuperObject, IPT_pc_tdstGetSetParam *p_stValue)
{
    IPT_g_FieldPadSector = p_stValue->uParam.lValue;
    return 1;
}
