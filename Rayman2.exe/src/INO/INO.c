#include "INO/INO.h"
#include "GLI/Memory.h"
#include "INO/INO_globals.h"
#include "INO/dinput_dx6.h"
/* With USE_SDL the DirectInput parts (the blocks fenced #ifndef USE_SDL) are in
   INO/sdl_ino.c; the device layer stays shared */
#ifdef USE_SDL
#include "INO/sdl_ino.h"
#endif /* USE_SDL */

/* 0x00509DE0 */
INO_tdhDevice INO_pc_a_hDevices_storage[INO_C_wNbMaxDevice];
#define INO_pc_a_hDevices  (INO_pc_a_hDevices_storage)

#define INO_pc_FUN_DInputInit  INO_pc_bInitDirectInput
extern long (__cdecl *_imp__time)(long *pTimer);
extern void (__cdecl *_imp__srand)(unsigned int uiSeed);
extern int  (__cdecl *_imp__rand)(void);

#define INO_pc_pfn_Alloc        GLI_pc_fn_p_vAlloc
#define INO_pc_pfn_FreeClear(p) GLI_pc_fn_vFree((void **)(p))
#define INO_pc_pfn_SlotAssign   INO_pc_wAssignDeviceSlot
#define INO_pc_pfn_Activate     INO_fn_wActivateDevice

struct _DIDATAFORMAT;
extern long __stdcall DirectInputCreateA(HINSTANCE hinst, unsigned long dwVersion, void *lplpDirectInput,
                                         void *punkOuter);
extern const struct _DIDATAFORMAT c_dfDIMouse;
extern const struct _DIDATAFORMAT c_dfDIKeyboard;
extern const struct _DIDATAFORMAT c_dfDIJoystick;
/* 0x0049CEB0 */
extern const struct _GUID IID_IDirectInputDevice2A;
#define INO_pc_pfn_DirectInputCreateA DirectInputCreateA

HWND      INO_pc_g_hWnd;
HINSTANCE INO_pc_g_hInstance;

typedef struct INO_pc_tdstDeviceSlot_ {
    DIDEVICEINSTANCEA   stInstance;
    void               *p_vDevice;
    void               *p_vDevice2;
    short               wUseDevice2;
    INO_tdstDevice     *p_stDevice;
} INO_pc_tdstDeviceSlot;

static INO_pc_tdstDeviceSlot INO_pc_g_aSlotTable[INO_C_wNbMaxDevice];   /* 0x00506F40 */
#define INO_C_DIERR_OLDVERSION    ((long)0x8007047E)

#define INO_C_DIEDFL_ALLDEVICES   0x00000000

#define INO_pc_SLOT_SUBTYPE(i)    GET_DIDEVICE_TYPE(INO_pc_g_aSlotTable[i].stInstance.dwDevType)
#define INO_pc_SLOT_DIDEV_A(i)    (INO_pc_g_aSlotTable[i].p_vDevice)
#define INO_pc_SLOT_DIDEV_B(i)    (INO_pc_g_aSlotTable[i].p_vDevice2)
#define INO_pc_SLOT_SELECTOR(i)   (INO_pc_g_aSlotTable[i].wUseDevice2)
#define INO_pc_SLOT_RECORD(i)     (INO_pc_g_aSlotTable[i].p_stDevice)

short __cdecl INO_pc_wRefreshDeviceCaps(INO_tdstDevice *pDevice);   /* 0x004957C0 */
short __cdecl INO_pc_ReadMouse(INO_tdstDevice *pDevice);
short __cdecl INO_pc_ReadKeyboard(INO_tdstDevice *pDevice);
short __cdecl INO_pc_ReadJoystick(INO_tdstDevice *pDevice);
short __cdecl INO_pc_wReadMouseDemo(INO_tdstDevice *pDevice);
short __cdecl INO_pc_wReadJoystickDemo(INO_tdstDevice *pDevice);
short __cdecl INO_pc_wReadJoystickDI(INO_tdstDevice *pDevice);

#define INO_pc_C_wMaxActiveKey    0x80
/* 0x00506C80 */
unsigned short INO_pc_g_a_wActiveKey_storage[INO_pc_C_wMaxActiveKey];
#define INO_pc_g_a_wActiveKey     (INO_pc_g_a_wActiveKey_storage)

#include "INO/Joystick.h"

/* 0x004960A0 */
float __cdecl INO_pc_xGetMouseAxis(INO_tdstDevice *pDevice)
{
    return ((INO_pc_tdstMouseState *)pDevice->p_stState)->fAxis;
}
#define INO_pc_pfn_GetMouseAxis   INO_pc_xGetMouseAxis
#define INO_pc_MOUSE_STATE_BYTES  0x38

/* 0x00420400 */
BOOL __cdecl GLD_bIsDeviceHandleValid(short hDev);
long __cdecl GLD_bGetDeviceAttributes(short hDev, void *pstDevAttrib);

/* 0x00495890 */
short __cdecl INO_pc_wInitMouseState(INO_tdstDevice *pDevice, short hDev)
{
    INO_pc_tdstMouseState *pstMouse;
    long           lDevAttrib[0x18];

    pstMouse = (INO_pc_tdstMouseState *)pDevice->p_stState;
    if (GLD_bIsDeviceHandleValid(hDev) != 0) {
        if (GLD_bGetDeviceAttributes(hDev, lDevAttrib) != 0) {
            pDevice->p_stState->ucStatus |= INO_C_ucGLDLink;
            pstMouse->fPosX = 0.5f;
            pstMouse->fPosY = 0.5f;
            pstMouse->hGLDDevice = hDev;
            pstMouse->fScale = 1.0f;
            pstMouse->fAxis = 1.0f;
            return 1;
        }
    }
    return 0;
}

/* 0x00495F70 */
void __cdecl INO_pc_fn_vAccumulateMouseAxes(INO_tdstDevice *pDevice)
{
    INO_pc_tdstMouseState *pstMouse;
    long           lDevAttrib[0x18];
    float          fValue;

    pstMouse = (INO_pc_tdstMouseState *)pDevice->p_stState;
    if ((pstMouse->ucStatus & INO_C_ucGLDLink) != 0) {
        if (GLD_bGetDeviceAttributes(pstMouse->hGLDDevice, lDevAttrib) != 0) {
            fValue = ((float)pstMouse->lRawX * pstMouse->fScale)
                     / (float)(unsigned long)(lDevAttrib[1] - 1) + pstMouse->fPosX;
            pstMouse->fPosX = fValue;
            if (fValue < 0.0f) {
                pstMouse->fPosX = 0.0f;
            }
            if (pstMouse->fPosX > 1.0f) {
                pstMouse->fPosX = 1.0f;
            }
            fValue = ((float)pstMouse->lRawY * pstMouse->fScale)
                     / (float)(unsigned long)(lDevAttrib[0] - 1) + pstMouse->fPosY;
            pstMouse->fPosY = fValue;
            if (fValue < 0.0f) {
                pstMouse->fPosY = 0.0f;
            }
            if (pstMouse->fPosY > 1.0f) {
                pstMouse->fPosY = 1.0f;
            }
        }
    }
}

/* 0x00494CE0 */
short __cdecl INO_pc_bIsInited(void)
{
    return (short)(INO_pc_g_bInited != 0);
}

/* 0x00495030 */
short __cdecl INO_fn_wIsHandleValid(INO_tdhDevice hDev)
{
    int i;

    if (INO_pc_bIsInited() && hDev != (INO_tdhDevice)0) {
        for (i = 0; i < INO_C_wNbMaxDevice; i++) {
            if (INO_pc_a_hDevices[i] == hDev) {
                return 1;
            }
        }
    }
    return 0;
}

/* 0x004950F0 */
short __cdecl INO_fn_wActivateDevice(INO_tdhDevice hDev)
{
    if (INO_pc_bIsInited() && INO_fn_wIsHandleValid(hDev)) {
        hDev->p_stState->ucStatus |= INO_C_ucActivated;
        return 1;
    }
    return 0;
}

/* 0x00495130 */
short __cdecl INO_fn_wDeactivateDevice(INO_tdhDevice hDev)
{
    if (INO_pc_bIsInited() && INO_fn_wIsHandleValid(hDev)) {
        hDev->p_stState->ucStatus &= (unsigned char)~INO_C_ucActivated;
        return 1;
    }
    return 0;
}

/* 0x00495080 */
short __cdecl INO_fn_wReadAllDevices(unsigned short uwType, short wMode)
{
    short          sResult;
    int            i;
    INO_tdstDevice *pDevice;

    sResult = 1;
    if (!INO_pc_bIsInited()) {
        return 0;
    }

    for (i = 0; i < INO_C_wNbMaxDevice; i++) {
        pDevice = INO_pc_a_hDevices[i];
        if (pDevice != (INO_tdstDevice *)0 &&
            ((unsigned char)pDevice->uwType & (unsigned char)uwType) != 0 &&
            (pDevice->p_stState->ucStatus & INO_C_ucActivated) != 0) {
            switch (wMode) {
            case 1:
                INO_pc_wRefreshDeviceCaps(pDevice);
                break;
            }
            pDevice = INO_pc_a_hDevices[i];
            if ((pDevice->p_stState->ucStatus & INO_C_ucConnected) != 0) {
                sResult = (short)(sResult & pDevice->pfnRead(pDevice));
            }
        }
    }
    return sResult;
}

#ifndef USE_SDL
/* 0x004955B0 */
short __cdecl INO_fn_wGetNbDevices(unsigned short uwType)
{
    const INO_pc_tdstDeviceSlot *p_stSlot;
    int                  iCount;
    int                  i;

    if (INO_pc_g_pDirectInput == (void *)0) {
        return 0;
    }

    iCount   = 0;
    p_stSlot = INO_pc_g_aSlotTable;
    for (i = 0; i < INO_C_wNbMaxDevice; i++, p_stSlot++) {
        if ((1 << GET_DIDEVICE_TYPE(p_stSlot->stInstance.dwDevType)) == (uwType & 0xffff)) {
            iCount++;
        }
    }
    return (short)iCount;
}
#endif /* USE_SDL */

/* 0x00494CF0 */
short __cdecl INO_fn_wInit(HINSTANCE hInstance, HWND hWindow)
{
    int i;

    if (INO_pc_g_bInited == 0) {
        for (i = 0; i < INO_C_wNbMaxDevice; i++) {
            INO_pc_a_hDevices[i] = (INO_tdhDevice)0;
        }
        if (INO_pc_FUN_DInputInit(hInstance, hWindow) == 0) {
            return 0;
        }
        INO_pc_g_bInited = 1;
        (*_imp__srand)((unsigned int)(*_imp__time)((long *)0));
        INO_pc_g_bInputProduced = 0;
    }
    return 1;
}

/* 0x00494D90 */
INO_tdhDevice __cdecl INO_fn_hCreateDevice(unsigned short uwType, short wHistoricSize)
{
    INO_tdstDevice *pDevice;
    unsigned int    uCapsSize, uStateSize;
    unsigned int    uiType;
    int             iHistStride, iHistSize;
    unsigned int    n;
    unsigned int   *pZero;
    short           wSlot;

    if (!INO_pc_bIsInited()) {
        return (INO_tdhDevice)0;
    }
    if (INO_pc_g_wDeviceCount == INO_C_wNbMaxDevice) {
        return (INO_tdhDevice)0;
    }

    pDevice = (INO_tdstDevice *)INO_pc_pfn_Alloc(0x20);
    if (pDevice == (INO_tdstDevice *)0) {
        return (INO_tdhDevice)0;
    }

    pDevice->uwType = uwType;
    uiType = (unsigned int)uwType;
    if (uiType == INO_C_uwMouse) {
        uCapsSize = 0x54; uStateSize = 0x38; iHistStride = 0x08;
    } else if (uiType == INO_C_uwKeyboard) {

        uCapsSize = 0x0c; uStateSize = 0x22c; iHistStride = 0x18;
    } else if (uiType == INO_C_uwJoystick) {
        uCapsSize = 0x54; uStateSize = 0x24; iHistStride = 0x0c;
    } else {
        return (INO_tdhDevice)0;
    }

    pDevice->p_stCaps = (INO_tdstDevCaps *)INO_pc_pfn_Alloc(uCapsSize);
    if (pDevice->p_stCaps == (INO_tdstDevCaps *)0) {
        INO_pc_pfn_FreeClear(&pDevice);
        return (INO_tdhDevice)0;
    }
    pZero = (unsigned int *)pDevice->p_stCaps;
    for (n = uCapsSize >> 2; n != 0; n--) *pZero++ = 0;

    pDevice->p_stState = (INO_tdstDevState *)INO_pc_pfn_Alloc(uStateSize);
    if (pDevice->p_stState != (INO_tdstDevState *)0) {
        pZero = (unsigned int *)pDevice->p_stState;
        for (n = uStateSize >> 2; n != 0; n--) *pZero++ = 0;

        iHistSize = iHistStride * (short)wHistoricSize;
        pDevice->pvHistoric = INO_pc_pfn_Alloc((unsigned int)iHistSize);
        if (pDevice->pvHistoric == (void *)0) {
            INO_pc_pfn_FreeClear(&pDevice->p_stState);
            INO_pc_pfn_FreeClear(&pDevice->p_stCaps);
            INO_pc_pfn_FreeClear(&pDevice);
            return (INO_tdhDevice)0;
        }
        pDevice->wHistoricSize = wHistoricSize;
        pDevice->wHistoricHead = 0;
        pDevice->wRecordNumber = 0;
        pZero = (unsigned int *)pDevice->pvHistoric;
        for (n = (unsigned int)iHistSize >> 2; n != 0; n--) *pZero++ = 0;
        pDevice->ulLastTimeCount = 0;

        wSlot = INO_pc_pfn_SlotAssign(pDevice);
        if (wSlot != -1) {
            INO_pc_g_wDeviceCount++;
            INO_pc_a_hDevices[wSlot] = pDevice;
            if ((uwType & 0x0c) != 0) {
                INO_pc_pfn_Activate(pDevice);
            }
            return pDevice;
        }
        INO_pc_pfn_FreeClear(&pDevice->p_stState);
    }
    INO_pc_pfn_FreeClear(&pDevice->p_stCaps);
    INO_pc_pfn_FreeClear(&pDevice);
    return (INO_tdhDevice)0;
}

/* 0x00494F90 */
short __cdecl INO_pc_wReleaseDeviceSlot(INO_tdstDevice *pDevice);

short __cdecl INO_fn_wReleaseDevice(INO_tdhDevice hDevice)
{
    int i;
    INO_tdhDevice *pSlot;

    if (!INO_pc_bIsInited()) {
        return 0;
    }

    for (i = 0, pSlot = INO_pc_a_hDevices;
         (long)pSlot < (long)(INO_pc_a_hDevices + INO_C_wNbMaxDevice);
         i++, pSlot++) {
        if (*pSlot == hDevice) {
            INO_pc_a_hDevices[i] = (INO_tdhDevice)0;
            break;
        }
    }

    if (i == INO_C_wNbMaxDevice) {
        return 0;
    }
    INO_pc_wReleaseDeviceSlot(hDevice);
    INO_pc_a_hDevices[i] = (INO_tdhDevice)0;
    INO_pc_pfn_FreeClear(&hDevice->p_stState);
    INO_pc_pfn_FreeClear(&hDevice->p_stCaps);
    INO_pc_pfn_FreeClear(&hDevice->pvHistoric);
    INO_pc_pfn_FreeClear(&hDevice);
    INO_pc_g_wDeviceCount--;
    return 1;
}

#define INO_pc_COM_RELEASE(p) \
    ((unsigned long (__stdcall *)(void *))((*(void ***)(p))[2]))(p)

/* 0x00495070 */
short __cdecl INO_pc_wCheckInitReturnZero(void *_pvIgnored)
{
    INO_pc_bIsInited();
    return 0;
}

#ifndef USE_SDL
/* 0x00495540 */
void __cdecl INO_pc_fn_vReleaseDirectInput(void)
{
    int i;

    if (INO_pc_g_pDirectInput != (void *)0) {
        for (i = 0; i < INO_C_wNbMaxDevice; i++) {
            if (INO_pc_SLOT_RECORD(i) != 0) {
                if (INO_pc_SLOT_SELECTOR(i) != 0) {
                    INO_pc_COM_RELEASE(INO_pc_SLOT_DIDEV_B(i));
                } else {
                    INO_pc_COM_RELEASE(INO_pc_SLOT_DIDEV_A(i));
                }
                INO_pc_wReleaseDeviceSlot(INO_pc_SLOT_RECORD(i));
            }
        }
        INO_pc_COM_RELEASE(INO_pc_g_pDirectInput);
        INO_pc_g_pDirectInput = (void *)0;
    }
}
#endif /* USE_SDL */

/* 0x00494D50 */
void __cdecl INO_pc_fn_vShutdown(void)
{
    int i;

    if (INO_pc_g_bInited != 0) {
        for (i = 0; i < INO_C_wNbMaxDevice; i++) {
            if (INO_pc_a_hDevices[i] != (INO_tdhDevice)0) {
                INO_fn_wReleaseDevice(INO_pc_a_hDevices[i]);
            }
        }
        INO_pc_fn_vReleaseDirectInput();
    }
    INO_pc_g_bInited = 0;
}

#ifndef USE_SDL
/* 0x00495170 */
int __stdcall INO_pc_bEnumDeviceCallback(void *lpddi, void *pvRef)
{
    void           *pDev;
    void           *pDev2;
    LPDIDEVICEINSTANCEA pInst;
    void          **pVtbl;
    INO_pc_tdstDIDeviceVtbl *pDevVt;
    const void     *pFmt;
    unsigned char   ucType;
    short           sIdx;

    pInst = (LPDIDEVICEINSTANCEA)lpddi;
    pVtbl = *(void ***)INO_pc_g_pDirectInput;

    if (((long (__stdcall *)(void *, void *, void **, void *))pVtbl[3])(INO_pc_g_pDirectInput, &pInst->guidInstance, (void **)&lpddi, (void *)0) != 0) {
        return 0;
    }
    pDev = lpddi;
    ucType = (unsigned char)pInst->dwDevType;
    if (ucType == 2) {
        pFmt = &c_dfDIMouse;
    } else if (ucType == 3) {
        pFmt = &c_dfDIKeyboard;
    } else if (ucType == 4) {
        pFmt = &c_dfDIJoystick;
    } else {
        return 0;
    }
    pDevVt = *(INO_pc_tdstDIDeviceVtbl **)pDev;
    pDevVt->Unacquire(pDev);
    if (pDevVt->SetDataFormat(pDev, pFmt) != 0) {
        INO_pc_COM_RELEASE(pDev);
        return 0;
    }
    if (pDevVt->SetCooperativeLevel(pDev, (void *)INO_pc_g_hWnd, 6) != 0) {
        INO_pc_COM_RELEASE(pDev);
        return 0;
    }
    sIdx = *(short *)&INO_pc_g_lEnumDeviceCount;
    INO_pc_g_aSlotTable[sIdx].stInstance = *pInst;
    INO_pc_SLOT_DIDEV_A(sIdx) = pDev;
    if (ucType == 4) {
        if (pDevVt->QueryInterface(pDev, &IID_IDirectInputDevice2A, &pDev2) == 0) {
            (*(short *)&INO_pc_g_lEnumDeviceCount)++;
            INO_pc_SLOT_DIDEV_B(sIdx) = pDev2;
            INO_pc_SLOT_SELECTOR(sIdx) = 1;
            return 1;
        }
    }
    (*(short *)&INO_pc_g_lEnumDeviceCount)++;
    INO_pc_SLOT_DIDEV_B(sIdx) = (void *)0;
    INO_pc_SLOT_SELECTOR(sIdx) = 0;
    return 1;
}

/* 0x004952D0 */
short __cdecl INO_pc_bInitDirectInput(HINSTANCE hInstance, HWND hWindow)
{
    long             hr;
    void           **pVtbl;
    long (__stdcall *pfnEnumDevices)(void *, unsigned long, void *, void *, unsigned long);

    if (INO_pc_g_pDirectInput != (void *)0) {
        return 1;
    }
    *(short *)&INO_pc_g_lEnumDeviceCount = 0;

    hr = INO_pc_pfn_DirectInputCreateA(hInstance, 0x0500, &INO_pc_g_pDirectInput, (void *)0);
    if (hr == INO_C_DIERR_OLDVERSION) {
        hr = INO_pc_pfn_DirectInputCreateA(hInstance, 0x0300, &INO_pc_g_pDirectInput, (void *)0);
        if (hr == 0) {
            INO_pc_g_lDInputVersion = 0x0300;
        }
    } else {
        INO_pc_g_lDInputVersion = 0x0500;
    }
    if (hr != 0) {
        INO_pc_g_pDirectInput   = (void *)0;
        INO_pc_g_lDInputVersion = 0;
        return 0;
    }

    INO_pc_g_hWnd = hWindow;
    memset(INO_pc_g_aSlotTable, 0, sizeof(INO_pc_g_aSlotTable));
    INO_pc_g_hInstance = hInstance;

    pVtbl = *(void ***)INO_pc_g_pDirectInput;
    pfnEnumDevices = (long (__stdcall *)(void *, unsigned long, void *, void *, unsigned long))
                     pVtbl[4];
    hr = pfnEnumDevices(INO_pc_g_pDirectInput, 0, (void *)INO_pc_bEnumDeviceCallback,
                        (void *)0, INO_C_DIEDFL_ALLDEVICES);
    return (short)(hr == 0);
}

/* 0x495600 */
static DIDEVCAPS     INO_pc_stDevCapsDX5;
static DIDEVCAPS_DX3 INO_pc_stDevCapsDX3;
static unsigned char INO_pc_aucScanToVK[0x100];
static unsigned char INO_pc_aucToAsciiKeyState[0x100];
static unsigned char INO_pc_aucKeyboardLayoutName[0x20];
static HKL           INO_pc_g_hKeyboardLayout;
static unsigned char INO_pc_ucShiftDown;
static unsigned char INO_pc_ucCtrlDown;
static unsigned char INO_pc_ucAltDown;
static unsigned char INO_pc_ucCapsOn;

typedef struct INO_pc_tdstPropRange_ {
    unsigned long dwSize;
    unsigned long dwHeaderSize;
    unsigned long dwObj;
    unsigned long dwHow;
    long          lMin;
    long          lMax;
} INO_pc_tdstPropRange;

/* 0x00495600 */
short __cdecl INO_pc_wAcquireDeviceAndGetCaps(INO_tdstDevice *pDevice)
{
    short           wSlot;
    INO_tdstDevState *pState;
    INO_pc_tdstAxisDeviceCaps *pCaps;
    unsigned char   ucType;
    long            hr;

    for (wSlot = 0; wSlot < INO_C_wNbMaxDevice; wSlot++) {
        if (INO_pc_SLOT_RECORD(wSlot) == pDevice) {
            break;
        }
    }
    if (wSlot == INO_C_wNbMaxDevice) {
        return 0;
    }

#define INO_pc_M_Acquire(pDev)          ((*(INO_pc_tdstDIDeviceVtbl **)(pDev))->Acquire(pDev))
#define INO_pc_M_GetCaps(pDev, pCapsBuf) \
    ((*(INO_pc_tdstDIDeviceVtbl **)(pDev))->GetCapabilities((pDev), (pCapsBuf)))
    if (INO_pc_SLOT_SELECTOR(wSlot) == 0) {
        INO_pc_M_Acquire(INO_pc_SLOT_DIDEV_A(wSlot));
    } else {
        INO_pc_M_Acquire(INO_pc_SLOT_DIDEV_B(wSlot));
    }

    if (INO_pc_g_lDInputVersion == 0x0300) {
        memset(&INO_pc_stDevCapsDX3, 0, sizeof(INO_pc_stDevCapsDX3));
        INO_pc_stDevCapsDX3.dwSize = sizeof(DIDEVCAPS_DX3);
        if (INO_pc_SLOT_SELECTOR(wSlot) == 0) {
            hr = INO_pc_M_GetCaps(INO_pc_SLOT_DIDEV_A(wSlot), &INO_pc_stDevCapsDX3);
        } else {
            hr = INO_pc_M_GetCaps(INO_pc_SLOT_DIDEV_B(wSlot), &INO_pc_stDevCapsDX3);
        }
    } else {
        memset(&INO_pc_stDevCapsDX5, 0, sizeof(INO_pc_stDevCapsDX5));
        INO_pc_stDevCapsDX5.dwSize = sizeof(DIDEVCAPS);
        if (INO_pc_SLOT_SELECTOR(wSlot) == 0) {
            hr = INO_pc_M_GetCaps(INO_pc_SLOT_DIDEV_A(wSlot), &INO_pc_stDevCapsDX5);
        } else {
            hr = INO_pc_M_GetCaps(INO_pc_SLOT_DIDEV_B(wSlot), &INO_pc_stDevCapsDX5);
        }
    }
#undef INO_pc_M_Acquire
#undef INO_pc_M_GetCaps

    pState = pDevice->p_stState;
    if (hr != 0) {
        pState->ucStatus &= (unsigned char)~INO_C_ucConnected;
        pState->ucStatus &= (unsigned char)~INO_C_ucAcquired;
        return 0;
    }

    pCaps = (INO_pc_tdstAxisDeviceCaps *)pDevice->p_stCaps;
    ucType = (unsigned char)(pDevice->uwType & 0xFF);
    if (INO_pc_g_lDInputVersion == 0x0300) {
        if (INO_pc_stDevCapsDX3.dwFlags & DIDC_ATTACHED) {
            pState->ucStatus |= INO_C_ucConnected;
        }
        pDevice->p_stCaps->ucNbButtons = (unsigned char)INO_pc_stDevCapsDX3.dwButtons;
        if (ucType == INO_C_uwMouse) {
            pCaps->ucNbAxes = (unsigned char)INO_pc_stDevCapsDX3.dwAxes;
        } else if (ucType == INO_C_uwJoystick) {
            pCaps->ucNbPOVs = (unsigned char)INO_pc_stDevCapsDX3.dwPOVs;
            pCaps->ucNbAxes = (unsigned char)INO_pc_stDevCapsDX3.dwAxes;
        }
        return (short)GET_DIDEVICE_SUBTYPE(INO_pc_stDevCapsDX3.dwDevType);
    }
    if (INO_pc_stDevCapsDX5.dwFlags & DIDC_ATTACHED) {
        pState->ucStatus |= INO_C_ucConnected;
    }
    pDevice->p_stCaps->ucNbButtons = (unsigned char)INO_pc_stDevCapsDX5.dwButtons;
    if (ucType == INO_C_uwJoystick) {
        pCaps->ucNbAxes = (unsigned char)INO_pc_stDevCapsDX5.dwAxes;
        pCaps->ucNbPOVs = (unsigned char)INO_pc_stDevCapsDX5.dwPOVs;
    }
    return (short)GET_DIDEVICE_SUBTYPE(INO_pc_stDevCapsDX5.dwDevType);
}

/* 0x00496110 */
unsigned int __cdecl INO_pc_ulTranslateScanCodeToAscii(unsigned int uiScanCode, INO_tdstDevice *pDevice)
{
    short           sScan;
    unsigned int    uVirtKey;
    INO_tdstKeyboardState *pState;
    unsigned char  *pKeys;
    WORD            wChar;
    int             iResult;

    sScan = (short)uiScanCode;
    if ((((sScan < 0x47) || (0x53 < sScan)) && (sScan != 0xB5)) ||
        ((GetKeyState(0x90) & 1) == 0)) {
        uVirtKey = MapVirtualKeyExA((UINT)sScan, 1, INO_pc_g_hKeyboardLayout);
    } else {
        uVirtKey = INO_pc_aucScanToVK[sScan];
    }

    if (uVirtKey != 0 && uVirtKey != 0x14 && uVirtKey != 0x91 && uVirtKey != 0x90 &&
        uVirtKey != 0x10 && uVirtKey != 0x11 && uVirtKey != 0x12 &&
        (uVirtKey < 0x70 || uVirtKey > 0x87)) {
        pState = (INO_tdstKeyboardState *)pDevice->p_stState;
        pKeys = pState->a_ucTabKey[pState->wCurrentTab];

        INO_pc_ucShiftDown = (unsigned char)((pKeys[0x2A] | pKeys[0x36]) & 0x80);
        INO_pc_ucCtrlDown  = (unsigned char)((pKeys[0x9D] | pKeys[0x1D]) & 0x80);
        INO_pc_ucAltDown   = (unsigned char)((pKeys[0xB8] | pKeys[0x38]) & 0x80);
        INO_pc_ucCapsOn    = (unsigned char)(GetKeyState(0x14) & 1);

        iResult = ToAsciiEx(uVirtKey, (UINT)sScan, INO_pc_aucToAsciiKeyState,
                            &wChar, 0, INO_pc_g_hKeyboardLayout);
        if (iResult != 0) {
            switch (iResult) {
            case 1:
                return (unsigned int)(wChar & 0xFF);
            case 2:
                return 0;
            default:
                return (unsigned int)iResult;
            }
        }
    }
    return 0;
}

/* 0x004957C0 */
short __cdecl INO_pc_wRefreshDeviceCaps(INO_tdstDevice *pDevice)
{
    short           wSlot;
    INO_tdstDevState *pState;
    void           *pDIDev;
    INO_pc_tdstDIDeviceVtbl *pVtbl;
    long            hr;
    unsigned char   ucState;

    for (wSlot = 0; wSlot < INO_C_wNbMaxDevice; wSlot++) {
        if (INO_pc_SLOT_RECORD(wSlot) == pDevice) {
            break;
        }
    }
    if (wSlot == INO_C_wNbMaxDevice) {
        return 0;
    }

    memset(&INO_pc_stDevCapsDX3, 0, sizeof(INO_pc_stDevCapsDX3));
    INO_pc_stDevCapsDX3.dwSize = sizeof(DIDEVCAPS_DX3);
    if (INO_pc_SLOT_SELECTOR(wSlot) != 0) {
        pDIDev = INO_pc_SLOT_DIDEV_B(wSlot);
        pVtbl = *(INO_pc_tdstDIDeviceVtbl **)pDIDev;
        hr = pVtbl->GetCapabilities(pDIDev, &INO_pc_stDevCapsDX3);
    } else {
        pDIDev = INO_pc_SLOT_DIDEV_A(wSlot);
        pVtbl = *(INO_pc_tdstDIDeviceVtbl **)pDIDev;
        hr = pVtbl->GetCapabilities(pDIDev, &INO_pc_stDevCapsDX3);
    }

    if (hr == 0) {
        if (INO_pc_stDevCapsDX3.dwFlags & DIDC_ATTACHED) {
            pState = pDevice->p_stState;
            ucState = pState->ucStatus;
            ucState |= INO_C_ucConnected;
            pState->ucStatus = ucState;
            return 1;
        }
        pState = pDevice->p_stState;
        ucState = pState->ucStatus;
        ucState &= (unsigned char)~INO_C_ucConnected;
        pState->ucStatus = ucState;
    }
    return 0;
}

/* 0x00495900 */
short __cdecl INO_pc_wSetupMouseDI(INO_tdstDevice *pDevice)
{
    short                    wSlot;
    short                    wType;
    short                    wAxis;
    unsigned long            aulObj[3];
    INO_pc_tdstAxisDeviceCaps *pCaps;
    INO_tdstDevState        *pState;
    void                    *pDIDev;
    INO_pc_tdstDIDeviceVtbl *pVtbl;
    INO_pc_tdstPropRange     stRange;
    long                     hr;
    long                     lCenter;

    aulObj[0] = 0x00;
    aulObj[1] = 0x04;
    aulObj[2] = 0x08;

    for (wSlot = 0; wSlot < INO_C_wNbMaxDevice; wSlot++) {
        if (INO_pc_SLOT_RECORD(wSlot) == pDevice) {
            break;
        }
    }
    if (wSlot == INO_C_wNbMaxDevice) {
        return 0;
    }

    wType = (short)INO_pc_wAcquireDeviceAndGetCaps(pDevice);
    if (wType == 0) {
        return 0;
    }

    switch ((int)wType) {
    case 2:
        pDevice->uwType |= 0x0400;
        break;
    case 4:
        pDevice->uwType |= 0x0700;
        break;
    case 5:
        pDevice->uwType |= 0x0500;
        break;
    case 3:
        pDevice->uwType |= 0x0600;
        break;
    default:
        break;
    }

    pCaps = (INO_pc_tdstAxisDeviceCaps *)pDevice->p_stCaps;
    pState = pDevice->p_stState;
    pDIDev = (INO_pc_SLOT_SELECTOR(wSlot) != 0) ? INO_pc_SLOT_DIDEV_B(wSlot)
                                                : INO_pc_SLOT_DIDEV_A(wSlot);
    pVtbl = *(INO_pc_tdstDIDeviceVtbl **)pDIDev;
    for (wAxis = 0; wAxis < 3; wAxis++) {
        stRange.dwSize       = 0x18;
        stRange.dwHeaderSize = 0x10;
        stRange.dwHow        = 1;
        stRange.dwObj        = aulObj[wAxis];
        stRange.lMin         = 0;
        stRange.lMax         = 0;
        hr = pVtbl->GetProperty(pDIDev, (void *)4, &stRange);
        if ((unsigned long)hr != 0x80070002UL) {
            if (hr != 0) {
                return 0;
            }
            lCenter = (stRange.lMin + stRange.lMax) >> 1;
            pCaps->a_stAxis[wAxis].m_lCenter = lCenter;
            pCaps->a_stAxis[wAxis].m_lRange  = stRange.lMax - lCenter;
            pCaps->stDev.ulCapsFlags |= 1UL << wAxis;
        }
    }
    pState->ucStatus &= (unsigned char)~INO_C_ucGLDLink;
    return 1;
}

/* 0x004962D0 */
short __cdecl INO_pc_wSetupKeyboardDI(INO_tdstDevice *pDevice)
{
    short           wSlot;
    unsigned int    uiIndex;
    short           wKey;
    unsigned char   ucProbe;
    unsigned short *pwAzerty;

    for (wSlot = 0; wSlot < INO_C_wNbMaxDevice; wSlot++) {
        if (INO_pc_SLOT_RECORD(wSlot) == pDevice) {
            break;
        }
    }
    if (wSlot == INO_C_wNbMaxDevice) {
        return 0;
    }
    if (INO_pc_wAcquireDeviceAndGetCaps(pDevice) == 0) {
        return 0;
    }

    for (uiIndex = 0; uiIndex < INO_pc_C_wMaxActiveKey; uiIndex++) {
        INO_pc_g_a_wActiveKey[uiIndex] = 0;
    }
    INO_pc_g_uwActiveKeyCount = 0;
    GetKeyboardLayoutNameA((LPSTR)INO_pc_aucKeyboardLayoutName);
    INO_pc_g_hKeyboardLayout = LoadKeyboardLayoutA((LPCSTR)INO_pc_aucKeyboardLayoutName, 1);

    for (wKey = 1; wKey < 0x58; wKey++) {
        INO_fn_wAddActiveKey(wKey);
    }
    INO_fn_wAddActiveKey(0x69);
    INO_fn_wAddActiveKey(0x9C);
    INO_fn_wAddActiveKey(0x9D);
    INO_fn_wAddActiveKey(0xB5);
    INO_fn_wAddActiveKey(0xB7);
    INO_fn_wAddActiveKey(0xB8);
    INO_fn_wAddActiveKey(0xC7);
    INO_fn_wAddActiveKey(0xC5);
    INO_fn_wAddActiveKey(0xC8);
    INO_fn_wAddActiveKey(0xC9);
    INO_fn_wAddActiveKey(0xCB);
    INO_fn_wAddActiveKey(0xCD);
    INO_fn_wAddActiveKey(0xCF);
    INO_fn_wAddActiveKey(0xD0);
    INO_fn_wAddActiveKey(0xD1);
    INO_fn_wAddActiveKey(0xD2);
    INO_fn_wAddActiveKey(0xD3);
    INO_fn_wAddActiveKey(0xDB);
    INO_fn_wAddActiveKey(0xDC);
    INO_fn_wAddActiveKey(0xDD);

    INO_pc_aucScanToVK[0x52] = 0x60;
    INO_pc_aucScanToVK[0x4F] = 0x61;
    INO_pc_aucScanToVK[0x50] = 0x62;
    INO_pc_aucScanToVK[0x51] = 0x63;
    INO_pc_aucScanToVK[0x4B] = 0x64;
    INO_pc_aucScanToVK[0x4C] = 0x65;
    INO_pc_aucScanToVK[0x4D] = 0x66;
    INO_pc_aucScanToVK[0x47] = 0x67;
    INO_pc_aucScanToVK[0x48] = 0x68;
    INO_pc_aucScanToVK[0x49] = 0x69;
    INO_pc_aucScanToVK[0xB5] = 0x6F;
    INO_pc_aucScanToVK[0x37] = 0x6A;
    INO_pc_aucScanToVK[0x4A] = 0x6D;
    INO_pc_aucScanToVK[0x4E] = 0x6B;
    INO_pc_aucScanToVK[0x53] = 0x6E;

    ucProbe = (unsigned char)INO_pc_ulTranslateScanCodeToAscii(0x10, pDevice);
    pwAzerty = &((INO_tdstKeyboardCaps *)pDevice->p_stCaps)->wIsAzerty;
    if (ucProbe != 0x61 && ucProbe != 0x41) {
        *pwAzerty = 0;
        return 1;
    }
    *pwAzerty = 1;
    return 1;
}

/* 0x00496920 */
short __cdecl INO_pc_wSetupJoystickDI(INO_tdstDevice *pDevice)
{
    short                    wSlot;
    short                    wType;
    short                    wAxis;
    unsigned long            aulObj[6];
    INO_pc_tdstAxisDeviceCaps *pCaps;
    void                    *pDIDev;
    INO_pc_tdstDIDeviceVtbl *pVtbl;
    INO_pc_tdstPropRange     stRange;
    long                     hr;
    long                     lCenter;

    aulObj[0] = 0x00;
    aulObj[1] = 0x04;
    aulObj[2] = 0x08;
    aulObj[3] = 0x0C;
    aulObj[4] = 0x10;
    aulObj[5] = 0x14;

    for (wSlot = 0; wSlot < INO_C_wNbMaxDevice; wSlot++) {
        if (INO_pc_SLOT_RECORD(wSlot) == pDevice) {
            break;
        }
    }
    if (wSlot == INO_C_wNbMaxDevice) {
        return 0;
    }

    wType = (short)INO_pc_wAcquireDeviceAndGetCaps(pDevice);
    if (wType == 0) {
        return 0;
    }

    switch ((int)wType) {
    case 2:
    case 4:
        pDevice->uwType |= 0x0200;
        break;
    case 3:
    case 5:
        pDevice->uwType |= 0x0100;
        break;
    case 6:
        pDevice->uwType |= 0x0300;
        break;
    default:
        break;
    }

    pCaps = (INO_pc_tdstAxisDeviceCaps *)pDevice->p_stCaps;
    pDIDev = (INO_pc_SLOT_SELECTOR(wSlot) != 0) ? INO_pc_SLOT_DIDEV_B(wSlot)
                                                : INO_pc_SLOT_DIDEV_A(wSlot);
    pVtbl = *(INO_pc_tdstDIDeviceVtbl **)pDIDev;
    for (wAxis = 0; wAxis < 6; wAxis++) {
        stRange.dwSize       = 0x18;
        stRange.dwHeaderSize = 0x10;
        stRange.dwHow        = 1;
        stRange.dwObj        = aulObj[wAxis];
        stRange.lMin         = 0;
        stRange.lMax         = 0;
        hr = pVtbl->GetProperty(pDIDev, (void *)4, &stRange);
        if ((unsigned long)hr != 0x80070002UL) {
            if (hr != 0) {
                return 0;
            }
            lCenter = (stRange.lMin + stRange.lMax) >> 1;
            pCaps->a_stAxis[wAxis].m_lCenter    = lCenter;
            pCaps->a_stAxis[wAxis].m_lRange     = (stRange.lMax - stRange.lMin) >> 1;
            pCaps->a_stAxis[wAxis].m_lThreshold = (stRange.lMax - stRange.lMin) >> 2;
            pCaps->stDev.ulCapsFlags |= 1UL << wAxis;
        }
    }
    return 1;
}

/* 0x004953A0 */
short __cdecl INO_pc_wAssignDeviceSlot(INO_tdstDevice *pDevice)
{
    short  sSlot;
    short  sSetup;
    int    hr;
    void  *pDIDev;
    INO_pc_tdstDIDeviceVtbl *pVtbl;

    for (sSlot = 0; sSlot < 0x14; sSlot++) {
        if (INO_pc_SLOT_RECORD(sSlot) == (INO_tdstDevice *)0 &&
            (1 << INO_pc_SLOT_SUBTYPE(sSlot)) == (pDevice->uwType & 0xff)) {
            INO_pc_SLOT_RECORD(sSlot) = pDevice;

            switch (pDevice->uwType & 0xff) {
            case INO_C_uwMouse:
                pDevice->pfnRead = INO_pc_ReadMouse;
                sSetup = (short)INO_pc_wSetupMouseDI(pDevice);
                break;
            case INO_C_uwKeyboard:
                pDevice->pfnRead = INO_pc_ReadKeyboard;
                sSetup = (short)INO_pc_wSetupKeyboardDI(pDevice);
                break;
            case INO_C_uwJoystick:
                pDevice->pfnRead = INO_pc_ReadJoystick;
                sSetup = (short)INO_pc_wSetupJoystickDI(pDevice);
                break;
            }

            if (sSetup == 0) {
                return -1;
            }

            if (INO_pc_SLOT_SELECTOR(sSlot) != 0) {
                pDIDev = INO_pc_SLOT_DIDEV_B(sSlot);
                pVtbl = *(INO_pc_tdstDIDeviceVtbl **)pDIDev;
                hr = (int)pVtbl->Acquire(pDIDev);
            } else {
                pDIDev = INO_pc_SLOT_DIDEV_A(sSlot);
                pVtbl = *(INO_pc_tdstDIDeviceVtbl **)pDIDev;
                hr = (int)pVtbl->Acquire(pDIDev);
            }

            switch (hr) {
            case -0x7ff8ffe2:
                pDevice->p_stState->ucStatus &= (unsigned char)~INO_C_ucAcquired;
                return sSlot;
            case -0x7ff8ffa9:
                pDevice->p_stState->ucStatus &= (unsigned char)~INO_C_ucAcquired;
                return -1;
            case 0:
            case 1:
                pDevice->p_stState->ucStatus |= INO_C_ucAcquired;
                break;
            }
            return sSlot;
        }
    }
    return -1;
}

/* 0x004954D0 */
short __cdecl INO_pc_wReleaseDeviceSlot(INO_tdstDevice *pDevice)
{
    int    iSlot;
    void  *pDIDev;

    for (iSlot = 0; iSlot < 0x14; iSlot++) {
        if (INO_pc_SLOT_RECORD(iSlot) == pDevice) {
            if (INO_pc_SLOT_SELECTOR(iSlot) != 0) {
                pDIDev = INO_pc_SLOT_DIDEV_B(iSlot);
                (*(INO_pc_tdstDIDeviceVtbl **)pDIDev)->Unacquire(pDIDev);
            } else {
                pDIDev = INO_pc_SLOT_DIDEV_A(iSlot);
                (*(INO_pc_tdstDIDeviceVtbl **)pDIDev)->Unacquire(pDIDev);
            }
            memset(&INO_pc_g_aSlotTable[iSlot], 0, sizeof(INO_pc_tdstDeviceSlot));
            return 1;
        }
    }
    return 0;
}

/* 0x00496590 */
short __cdecl INO_fn_wReadKeyboard(INO_tdstDevice *hDevice)
{
    INO_tdstKeyboardState *pState;
    short                  sSlot;
    short                  sTab;
    int                    hrAcq;
    unsigned int           hrRead;
    void                  *pDIDev;
    INO_pc_tdstDIDeviceVtbl *pVtbl;
    unsigned char         *pBuf;

    pState = (INO_tdstKeyboardState *)hDevice->p_stState;
    if ((pState->ucStatus & INO_C_ucConnected) == 0) {
        return 0;
    }

    sSlot = 0;
    while (INO_pc_SLOT_RECORD(sSlot) != hDevice) {
        sSlot++;
        if (sSlot >= 0x14) {
            break;
        }
    }
    if (sSlot == 0x14) {
        return 0;
    }

    hDevice->ulLastTimeCount += 1;
    sTab = (short)pState->wCurrentTab;
    pState->wCurrentTab = (unsigned short)(1 - sTab);

    if (((pState->ucStatus & INO_C_ucAcquired) == 0) ||
        ((pState->ucStatus & INO_C_ucAccessError) != 0)) {
        if (INO_pc_SLOT_SELECTOR(sSlot) == 0) {
            pDIDev = INO_pc_SLOT_DIDEV_A(sSlot);
            pVtbl = *(INO_pc_tdstDIDeviceVtbl **)pDIDev;
            hrAcq = (int)pVtbl->Acquire(pDIDev);
        } else {
            pDIDev = INO_pc_SLOT_DIDEV_B(sSlot);
            pVtbl = *(INO_pc_tdstDIDeviceVtbl **)pDIDev;
            hrAcq = (int)pVtbl->Acquire(pDIDev);
        }

        if (hrAcq <= -0x7ff8fff4) {
            if ((unsigned int)hrAcq == 0x8007000c || (unsigned int)hrAcq == 0x80070005) {
                return 0;
            }
        } else {
            if ((unsigned int)hrAcq == 0x8007001e) {
                return 0;
            }
            if (-1 < hrAcq && hrAcq < 2) {
                pState->ucStatus |= INO_C_ucAcquired;
                pState->ucStatus &= (unsigned char)~INO_C_ucAccessError;
                goto do_read;
            }
        }
        pState->ucStatus &= (unsigned char)~INO_C_ucAcquired;
        pState->ucStatus |= INO_C_ucAccessError;
        return 0;
    }

do_read:
    pBuf = pState->a_ucTabKey[(short)(1 - sTab)];
    if (INO_pc_SLOT_SELECTOR(sSlot) == 0) {
        pDIDev = INO_pc_SLOT_DIDEV_A(sSlot);
        pVtbl = *(INO_pc_tdstDIDeviceVtbl **)pDIDev;
        hrRead = (unsigned int)pVtbl->GetDeviceState(pDIDev, 0x100, pBuf);
    } else {
        pDIDev = INO_pc_SLOT_DIDEV_B(sSlot);
        pVtbl = *(INO_pc_tdstDIDeviceVtbl **)pDIDev;
        hrRead = (unsigned int)pVtbl->GetDeviceState(pDIDev, 0x100, pBuf);
    }

    if ((int)hrRead <= -0x7ff8fff4) {
        if (hrRead == 0x8007000c) {
            pState->ucStatus &= (unsigned char)~INO_C_ucAcquired;
            return 0;
        }
        if (hrRead == 0x8000000a) {
            return 1;
        }
    } else {
        if (hrRead == 0x8007001e) {
            pState->ucStatus &= (unsigned char)~INO_C_ucAcquired;
            return 0;
        }
        if (hrRead == 0) {
            return 1;
        }
    }
    pState->ucStatus |= INO_C_ucAccessError;
    pState->ucStatus &= (unsigned char)~INO_C_ucAcquired;
    return 0;
}
#endif /* USE_SDL */

/* 0x00496270 */
short __cdecl INO_fn_wAddActiveKey(short wKey)
{
    int uCount;
    short i;

    if (INO_pc_g_uwActiveKeyCount <= (INO_pc_C_wMaxActiveKey - 1)) {
        uCount = (int)INO_pc_g_uwActiveKeyCount;
        for (i = 0; i < uCount; i++) {
            if ((unsigned int)INO_pc_g_a_wActiveKey[i] == (unsigned int)(int)wKey) {
                return 0;
            }
        }
        INO_pc_g_uwActiveKeyCount = (unsigned short)(INO_pc_g_uwActiveKeyCount + 1);
        INO_pc_g_a_wActiveKey[uCount] = (unsigned short)wKey;
    }
    return 0;
}

/* 0x00496710 */
int __cdecl INO_pc_fn_iReadKeyboardFrame(INO_tdstDevice *pDevice)
{
    INO_tdstKeyboardState *pState;
    INO_tdstKeyboardRecord *pRec;
    unsigned char         *pbCur;
    unsigned char         *pbOther;
    unsigned short         auPressed[8];
    unsigned short         uwKey;
    unsigned short         uwIndex;
    unsigned int           uiAscii;
    unsigned int           uiI;
    short                  sTab;
    short                  sChanged;
    short                  sCount;
    short                  sRecord;
    int                    bPressed;

    pState = (INO_tdstKeyboardState *)pDevice->p_stState;
    pRec = (INO_tdstKeyboardRecord *)pDevice->pvHistoric;
    sTab = (short)pState->wCurrentTab;
    pbCur = pState->a_ucTabKey[sTab];
    pbOther = pState->a_ucTabKey[1 - sTab];

    sChanged = 0;
    sCount = 0;
    bPressed = 0;
    uwIndex = 0;
    if (INO_pc_g_uwActiveKeyCount != 0) {
        do {
            uwKey = INO_pc_g_a_wActiveKey[uwIndex];
            if (pbCur[uwKey] != pbOther[uwKey]) {
                sChanged = 1;
                if (pbCur[uwKey] != 0) {
                    bPressed = 1;
                    pState->wLastKey = uwKey;
                }
            }
            if (sCount < 8 && pbCur[uwKey] != 0) {
                auPressed[sCount] = uwKey;
                sCount++;
            }
            uwIndex++;
        } while (uwIndex < INO_pc_g_uwActiveKeyCount);
    }

    if (bPressed != 0) {
        uiAscii = INO_pc_ulTranslateScanCodeToAscii((unsigned int)pState->wLastKey, pDevice);
        pState->wLastASCII = (unsigned short)uiAscii;
        if ((short)uiAscii != 0) {
            if (pState->wCurrentStringLength == 0x20) {
                for (uiI = 0; uiI < 0x1f; uiI++) {
                    pState->szCurrentString[uiI] = pState->szCurrentString[uiI + 1];
                }
                pState->szCurrentString[0x1f] = (char)pState->wLastASCII;
            } else {
                pState->szCurrentString[pState->wCurrentStringLength] = (char)pState->wLastASCII;
                pState->wCurrentStringLength = (unsigned short)(pState->wCurrentStringLength + 1);
            }
        }
    } else {
        pState->wLastASCII = 0;
    }

    sRecord = pDevice->wRecordNumber;
    if (sRecord == 0) {
        pRec[0].wNbKeyDown = 0;
        pRec[0].ulDate = pDevice->ulLastTimeCount;
        pDevice->wRecordNumber = 1;
        sChanged = 0;
    } else if (sChanged != 0) {
        pDevice->wHistoricHead = (short)(pDevice->wHistoricHead + 1);
        if (pDevice->wHistoricHead == pDevice->wHistoricSize) {
            pDevice->wHistoricHead = 0;
        }
        if (sRecord != pDevice->wHistoricSize) {
            pDevice->wRecordNumber = (short)(sRecord + 1);
        }
        pRec[pDevice->wHistoricHead].ulDate = pDevice->ulLastTimeCount;
        pRec[pDevice->wHistoricHead].wNbKeyDown = 0;
        for (uiI = 0; uiI < (unsigned int)sCount; uiI++) {
            pRec[pDevice->wHistoricHead].a_wKeyDown[uiI] = auPressed[uiI];
        }
        pRec[pDevice->wHistoricHead].wNbKeyDown = (unsigned short)sCount;
    }
    pState->wStateHasChanged = (unsigned short)sChanged;
    return 1;
}

#ifndef USE_SDL
static long ino_pc_ClampMouseAxis(INO_tdstDevice *pDevice, long lAxis)
{
    long v = (long)(INO_pc_pfn_GetMouseAxis(pDevice) * lAxis);
    long a = (v < 0x100) ? v : 0xff;
    if (a < -0xff) {
        return -0xff;
    }
    return (v < 0x100) ? v : 0xff;
}

/* 0x00495C60 */
short __cdecl INO_pc_wReadMouseDI(INO_tdstDevice *hDevice)
{
    INO_pc_tdstMouseState *pstMouse;
    short          sSlot;
    int            hr;
    void          *pDIDev;
    INO_pc_tdstDIDeviceVtbl *pVtbl;
    DIMOUSESTATE   stState;
    int            i;

    pstMouse = (INO_pc_tdstMouseState *)hDevice->p_stState;
    if ((pstMouse->ucStatus & INO_C_ucConnected) == 0) {
        return 0;
    }

    sSlot = 0;
    while (INO_pc_SLOT_RECORD(sSlot) != hDevice) {
        sSlot++;
        if (sSlot >= 0x14) {
            return 0;
        }
    }

    if (((pstMouse->ucStatus & INO_C_ucAcquired) == 0) || ((pstMouse->ucStatus & INO_C_ucAccessError) != 0)) {
        if (INO_pc_SLOT_SELECTOR(sSlot) != 0) {
            pDIDev = INO_pc_SLOT_DIDEV_B(sSlot);
            pVtbl = *(INO_pc_tdstDIDeviceVtbl **)pDIDev;
            hr = (int)pVtbl->Acquire(pDIDev);
        } else {
            pDIDev = INO_pc_SLOT_DIDEV_A(sSlot);
            pVtbl = *(INO_pc_tdstDIDeviceVtbl **)pDIDev;
            hr = (int)pVtbl->Acquire(pDIDev);
        }
        if (hr < -0x7ff8fff3) {
            if ((unsigned int)hr != 0x8007000c && (unsigned int)hr != 0x80070005) {
                pstMouse->ucStatus = (unsigned char)((pstMouse->ucStatus & 0xfd) | 4);
                return 0;
            }
            return 0;
        }
        if ((unsigned int)hr == 0x8007001e) {
            return 0;
        }
        if (hr < 0 || 1 < hr) {
            pstMouse->ucStatus = (unsigned char)((pstMouse->ucStatus & 0xfd) | 4);
            return 0;
        }
        pstMouse->ucStatus = (unsigned char)((pstMouse->ucStatus & 0xfb) | 2);
    }

    pDIDev = (INO_pc_SLOT_SELECTOR(sSlot) == 0) ? INO_pc_SLOT_DIDEV_A(sSlot)
                                                : INO_pc_SLOT_DIDEV_B(sSlot);
    pVtbl = *(INO_pc_tdstDIDeviceVtbl **)pDIDev;
    hr = (int)pVtbl->GetDeviceState(pDIDev, 0x10, &stState);

    if (hr < -0x7ff8fff3) {
        if (hr == -0x7ff8fff4) {
            pstMouse->ucStatus &= 0xfd;
            return 0;
        }
        if (hr != -0x7ffffff6) {
            pstMouse->ucStatus = (unsigned char)((pstMouse->ucStatus & 0xfd) | 4);
            return 0;
        }
    } else {
        if (hr == -0x7ff8ffe2) {
            pstMouse->ucStatus &= 0xfd;
            return 0;
        }
        if (hr != 0) {
            pstMouse->ucStatus = (unsigned char)((pstMouse->ucStatus & 0xfd) | 4);
            return 0;
        }
    }

    pstMouse->lRawX = ino_pc_ClampMouseAxis(hDevice, stState.lX);
    pstMouse->lRawY = ino_pc_ClampMouseAxis(hDevice, stState.lY);
    pstMouse->lRawZ = ino_pc_ClampMouseAxis(hDevice, stState.lZ);

    pstMouse->ulButtons = 0;
    i = 0;
    if (hDevice->p_stCaps->ucNbButtons != 0) {
        do {
            if ((stState.rgbButtons[(short)i] & 0x80) != 0) {
                pstMouse->ulButtons |= 1u << (unsigned char)i;
            }
            i++;
        } while ((short)i < (short)(unsigned short)hDevice->p_stCaps->ucNbButtons);
    }
    return 1;
}
#endif /* USE_SDL */

unsigned long __cdecl INO_pc_ulGetHistoricRecordFirstDword(INO_tdstDevice *pDevice, short wHistoryNum)
{
    return ((INO_pc_tdstJoyRecord *)pDevice->pvHistoric)
        [((pDevice->wHistoricHead - wHistoryNum) + pDevice->wHistoricSize) % pDevice->wHistoricSize].ulButtons;
}

unsigned short __cdecl INO_pc_uwGetHistoricRecordSecondWord(INO_tdstDevice *pDevice, short wHistoryNum)
{
    return ((INO_pc_tdstJoyRecord *)pDevice->pvHistoric)
        [((pDevice->wHistoricHead - wHistoryNum) + pDevice->wHistoricSize) % pDevice->wHistoricSize].uwDirection;
}

/* 0x00496030 */
void __cdecl INO_pc_fn_vPushSampleRecord(INO_tdstDevice *pDevice)
{
    short                   wState;
    INO_pc_tdstMouseRecord *p_stDest;
    INO_pc_tdstMouseState  *pstMouse;

    wState = pDevice->wRecordNumber;
    pstMouse = (INO_pc_tdstMouseState *)pDevice->p_stState;
    p_stDest = (INO_pc_tdstMouseRecord *)pDevice->pvHistoric;
    if (wState == 0)
    {
        p_stDest[0].ulTimeCount = pDevice->ulLastTimeCount;
        p_stDest[0].ulButtons = pstMouse->ulButtons;
        pDevice->wRecordNumber = 1;
        return;
    }
    pDevice->wHistoricHead = (short)(pDevice->wHistoricHead + 1);
    if (pDevice->wHistoricHead == pDevice->wHistoricSize)
        pDevice->wHistoricHead = 0;
    if (wState != pDevice->wHistoricSize)
        pDevice->wRecordNumber = (short)(wState + 1);
    p_stDest[pDevice->wHistoricHead].ulTimeCount = pDevice->ulLastTimeCount;
    p_stDest[pDevice->wHistoricHead].ulButtons = pstMouse->ulButtons;
}

/* 0x00496550 */
short __cdecl INO_pc_wFlipBufferIndex(INO_tdstDevice *pDevice)
{
    INO_tdstKeyboardState *pstKbd;

    pstKbd = (INO_tdstKeyboardState *)pDevice->p_stState;
    pstKbd->ucStatus = (unsigned char)(pstKbd->ucStatus | INO_C_ucAcquired);
    pDevice->p_stState->ucStatus = (unsigned char)(pDevice->p_stState->ucStatus & (unsigned char)~INO_C_ucAccessError);
    pDevice->ulLastTimeCount = pDevice->ulLastTimeCount + 1;
    pstKbd->wCurrentTab = (unsigned short)(1 - pstKbd->wCurrentTab);
    return 1;
}

int __cdecl INO_pc_lGetStoredValue(void);

/* 0x00496510 */
short __cdecl INO_pc_ReadKeyboard(INO_tdstDevice *pDevice)
{
    short sRead;

    if ((INO_pc_lGetStoredValue() & 0x9) == 0x9) {
        sRead = INO_pc_wFlipBufferIndex(pDevice);
    } else {
        sRead = INO_fn_wReadKeyboard(pDevice);
    }
    if (sRead != 0) {
        INO_pc_fn_iReadKeyboardFrame(pDevice);
    }
    return sRead;
}

/* 0x00497050 */
void __cdecl INO_pc_fn_vUpdateAxisFlags(INO_tdstDevice *pDevice)
{
    INO_tdstAxisCaps    *pstAxis;
    INO_pc_tdstJoyState *pstJoy;

    pstJoy = (INO_pc_tdstJoyState *)pDevice->p_stState;

    pstAxis = ((INO_pc_tdstAxisDeviceCaps *)pDevice->p_stCaps)->a_stAxis;
    pstJoy->uwDirection = 0;
    if (pstJoy->a_lAxis[0] < -pstAxis[0].m_lThreshold)
    {
        pstJoy->uwDirection = C_ulJoyLeft;
    }
    else if (pstJoy->a_lAxis[0] > pstAxis[0].m_lThreshold)
    {
        pstJoy->uwDirection = C_ulJoyRight;
    }

    if (pstJoy->a_lAxis[1] > pstAxis[1].m_lThreshold)
    {
        pstJoy->uwDirection |= C_ulJoyDown;
    }
    else if (pstJoy->a_lAxis[1] < -pstAxis[1].m_lThreshold)
    {
        pstJoy->uwDirection |= C_ulJoyUp;
    }
    if (pstJoy->a_lAxis[2] < -pstAxis[2].m_lThreshold)
    {
        pstJoy->uwDirection |= C_ulJoyZDown;
        return;
    }
    if (pstJoy->a_lAxis[2] > pstAxis[2].m_lThreshold)
    {
        pstJoy->uwDirection |= C_ulJoyZUp;
    }
}

extern int IPT_pc_g_bMouseEnabled;

/* 0x00495870 */
INO_tdhDevice __cdecl INO_pc_hCreateMouseDevice(unsigned short uwHistoricSize)
{
    if (IPT_pc_g_bMouseEnabled != 0)
        return INO_fn_hCreateDevice(INO_C_uwMouse, (short)uwHistoricSize);
    return (INO_tdhDevice)0;
}

/* 0x004970C0 */
void __cdecl INO_pc_fn_vPushTripleRecord(INO_tdstDevice *pDevice)
{
    short                 wState;
    INO_pc_tdstJoyRecord *p_stDest;
    INO_pc_tdstJoyState  *pstJoy;

    wState = pDevice->wRecordNumber;
    pstJoy = (INO_pc_tdstJoyState *)pDevice->p_stState;
    p_stDest = (INO_pc_tdstJoyRecord *)pDevice->pvHistoric;
    if (wState == 0)
    {
        p_stDest[0].ulTimeCount = pDevice->ulLastTimeCount;
        p_stDest[0].ulButtons = pstJoy->ulButtons;
        p_stDest[0].uwDirection = pstJoy->uwDirection;
        pDevice->wRecordNumber = 1;
        return;
    }
    pDevice->wHistoricHead = (short)(pDevice->wHistoricHead + 1);
    if (pDevice->wHistoricHead == pDevice->wHistoricSize)
        pDevice->wHistoricHead = 0;
    if (wState != pDevice->wHistoricSize)
        pDevice->wRecordNumber = (short)(wState + 1);
    p_stDest[pDevice->wHistoricHead].ulTimeCount = pDevice->ulLastTimeCount;
    p_stDest[pDevice->wHistoricHead].ulButtons = pstJoy->ulButtons;
    p_stDest[pDevice->wHistoricHead].uwDirection = pstJoy->uwDirection;
}

/* 0x004B9C58 */
int INO_pc_g_lStoredValue;

/* 0x00494CB0 */
int __cdecl INO_pc_lGetInputProduced(void)
{
    return INO_pc_g_bInputProduced;
}

/* 0x00494CC0 */
void __cdecl INO_pc_vSetStoredValue(int lValue)
{
    INO_pc_g_lStoredValue = lValue;
}

/* 0x00494CD0 */
int __cdecl INO_pc_lGetStoredValue(void)
{
    return INO_pc_g_lStoredValue;
}

/* 0x00495A80 */
short __cdecl INO_pc_ReadMouse(INO_tdstDevice *pDevice)
{
    short sRead;

    if (INO_pc_lGetStoredValue() != 0 && INO_pc_lGetStoredValue() != 1) {
        INO_pc_wReadMouseDI(pDevice);
        if (((INO_pc_tdstMouseState *)pDevice->p_stState)->ulButtons != 0UL) {
            INO_pc_vSetStoredValue(1);
        }
    }

    if ((INO_pc_lGetStoredValue() & 0x3) == 0x3) {
        sRead = INO_pc_wReadMouseDemo(pDevice);
    } else {
        sRead = INO_pc_wReadMouseDI(pDevice);
    }
    if (sRead != 0) {
        INO_pc_fn_vAccumulateMouseAxes(pDevice);
        INO_pc_fn_vPushSampleRecord(pDevice);
    }
    return sRead;
}

#define INO_pc_C_fDemoScale   3.0518509475997192e-5f
short __cdecl INO_pc_wReadMouseDemo(INO_tdstDevice *pDevice)
{
    INO_pc_tdstMouseState *pstMouse;
    unsigned char          ucValue;
    float                  xRand;
    int (__cdecl *pfnRand)(void);

    pstMouse = (INO_pc_tdstMouseState *)pDevice->p_stState;
    (void)INO_pc_lGetInputProduced();
    pfnRand = _imp__rand;
    pstMouse->ucStatus = (unsigned char)((pstMouse->ucStatus & 0xFB) | 0x02);

    xRand = (float)pfnRand() * INO_pc_C_fDemoScale;
    ucValue = (unsigned char)(long)(xRand * 130.0f);
    if (ucValue > 0x7D)
    {
        xRand = (float)pfnRand() * INO_pc_C_fDemoScale;
        pstMouse->lRawX = (long)(-63.0f - xRand * -126.0f);
    }
    else if (ucValue > 0x7A)
        pstMouse->lRawX = 0;

    xRand = (float)pfnRand() * INO_pc_C_fDemoScale;
    ucValue = (unsigned char)(long)(xRand * 130.0f);
    if (ucValue > 0x7D)
    {
        xRand = (float)pfnRand() * INO_pc_C_fDemoScale;
        pstMouse->lRawY = (long)(-63.0f - xRand * -126.0f);
    }
    else if (ucValue > 0x7A)
        pstMouse->lRawY = 0;

    xRand = (float)pfnRand() * INO_pc_C_fDemoScale;
    ucValue = (unsigned char)(long)(xRand * 130.0f);
    if (ucValue > 0x7D)
    {
        xRand = (float)pfnRand() * INO_pc_C_fDemoScale;
        pstMouse->lRawZ = (long)(-63.0f - xRand * -126.0f);
    }
    else if (ucValue > 0x7A)
        pstMouse->lRawZ = 0;

    xRand = (float)pfnRand() * INO_pc_C_fDemoScale;
    ucValue = (unsigned char)(long)(xRand * 130.0f);
    if (ucValue > 0x7D)
    {
        xRand = (float)pfnRand() * INO_pc_C_fDemoScale;
        pstMouse->ulButtons = (unsigned long)(long)(xRand * 255.0f);
    }
    else if (ucValue > 0x7A)
        pstMouse->ulButtons = 0;

    return 1;
}

/* 0x00496AB0 */
short __cdecl INO_pc_ReadJoystick(INO_tdstDevice *pDevice)
{
    short sRead;

    if ((INO_pc_lGetStoredValue() & 0x5) == 0x5) {
        sRead = INO_pc_wReadJoystickDemo(pDevice);
    } else {
        sRead = INO_pc_wReadJoystickDI(pDevice);
    }
    if (sRead != 0) {
        INO_pc_fn_vUpdateAxisFlags(pDevice);
        INO_pc_fn_vPushTripleRecord(pDevice);
    }
    return sRead;
}

/* 0x00496B00 */
short __cdecl INO_pc_wReadJoystickDemo(INO_tdstDevice *pDevice)
{
    INO_pc_tdstJoyState *pstJoy;
    INO_tdstAxisCaps *pstAxis;
    unsigned char  ucValue;
    long           lRange;
    float          xRand;
    int (__cdecl *pfnRand)(void);

    (void)INO_pc_lGetInputProduced();
    pstJoy = (INO_pc_tdstJoyState *)pDevice->p_stState;

    pstAxis = ((INO_pc_tdstAxisDeviceCaps *)pDevice->p_stCaps)->a_stAxis;
    pfnRand = _imp__rand;
    pstJoy->ucStatus = (unsigned char)((pstJoy->ucStatus & (unsigned char)~INO_C_ucAccessError) | INO_C_ucAcquired);

#define INO_pc_M_DemoAxis(n)                                                                \
    xRand = (float)pfnRand() * INO_pc_C_fDemoScale;                                         \
    ucValue = (unsigned char)(long)(xRand * 130.0f);                                        \
    if (ucValue > 0x7D)                                                                     \
    {                                                                                       \
        lRange = pstAxis->m_lRange;                                                         \
        pstJoy->a_lAxis[n] = (long)((float)pfnRand() * INO_pc_C_fDemoScale                  \
                                    * (float)(lRange * 2) - (float)lRange);                 \
    }                                                                                       \
    else if (ucValue > 0x7A)                                                                \
        pstJoy->a_lAxis[n] = 0;

    INO_pc_M_DemoAxis(0)
    INO_pc_M_DemoAxis(1)
    INO_pc_M_DemoAxis(2)
    INO_pc_M_DemoAxis(3)
    INO_pc_M_DemoAxis(4)
    INO_pc_M_DemoAxis(5)
#undef INO_pc_M_DemoAxis

    xRand = (float)pfnRand() * INO_pc_C_fDemoScale;
    ucValue = (unsigned char)(long)(xRand * 130.0f);
    if (ucValue > 0x7D)
    {
        xRand = (float)pfnRand() * INO_pc_C_fDemoScale;
        pstJoy->ulButtons = (long)(xRand * 255.0f);
    }
    else if (ucValue > 0x7A)
        pstJoy->ulButtons = 0;

    return 1;
}

#ifndef USE_SDL
/* 0x00496DE0 */
short __cdecl INO_pc_wReadJoystickDI(INO_tdstDevice *pDevice)
{
    INO_pc_tdstJoyState *pstJoy;
    INO_pc_tdstAxisDeviceCaps *pstCaps;
    short                sSlot;
    int                  hr;
    void                *pDIDev;
    INO_pc_tdstDIDeviceVtbl *pVtbl;
    DIJOYSTATE           stState;
    int                  i;

    pstJoy = (INO_pc_tdstJoyState *)pDevice->p_stState;
    if ((pstJoy->ucStatus & INO_C_ucConnected) == 0) {
        return 0;
    }

    sSlot = 0;
    while (INO_pc_SLOT_RECORD(sSlot) != pDevice) {
        sSlot++;
        if (sSlot >= 0x14) {
            return 0;
        }
    }

    if (((pstJoy->ucStatus & INO_C_ucAcquired) == 0) || ((pstJoy->ucStatus & INO_C_ucAccessError) != 0)) {
        if (INO_pc_SLOT_SELECTOR(sSlot) != 0) {
            pDIDev = INO_pc_SLOT_DIDEV_B(sSlot);
            pVtbl = *(INO_pc_tdstDIDeviceVtbl **)pDIDev;
            hr = (int)pVtbl->Acquire(pDIDev);
        } else {
            pDIDev = INO_pc_SLOT_DIDEV_A(sSlot);
            pVtbl = *(INO_pc_tdstDIDeviceVtbl **)pDIDev;
            hr = (int)pVtbl->Acquire(pDIDev);
        }
        if (hr < -0x7ff8fff3) {
            if ((unsigned int)hr != 0x8007000c && (unsigned int)hr != 0x80070005) {
                pstJoy->ucStatus = (unsigned char)((pstJoy->ucStatus & 0xfd) | 4);
            }
            return 0;
        }
        if ((unsigned int)hr == 0x8007001e) {
            return 0;
        }
        if (hr < 0 || 1 < hr) {
            pstJoy->ucStatus = (unsigned char)((pstJoy->ucStatus & 0xfd) | 4);
            return 0;
        }
        pstJoy->ucStatus = (unsigned char)((pstJoy->ucStatus & 0xfb) | 2);
    }

    if (INO_pc_SLOT_SELECTOR(sSlot) != 0) {
        pDIDev = INO_pc_SLOT_DIDEV_B(sSlot);
        pVtbl = *(INO_pc_tdstDIDeviceVtbl **)pDIDev;
        hr = (int)pVtbl->Poll(pDIDev);
        if (hr > -0x7ff8ffe2) {
            if (hr < 0 || 1 < hr) {
                pstJoy->ucStatus = (unsigned char)((pstJoy->ucStatus & 0xfd) | 4);
                return 0;
            }
        } else if (hr == -0x7ff8ffe2 || hr == -0x7ff8fff4) {
            pstJoy->ucStatus &= 0xfd;
            return 0;
        } else {
            pstJoy->ucStatus = (unsigned char)((pstJoy->ucStatus & 0xfd) | 4);
            return 0;
        }
    }

    if (INO_pc_SLOT_SELECTOR(sSlot) != 0) {
        pDIDev = INO_pc_SLOT_DIDEV_B(sSlot);
        pVtbl = *(INO_pc_tdstDIDeviceVtbl **)pDIDev;
        hr = (int)pVtbl->GetDeviceState(pDIDev, 0x50, &stState);
    } else {
        pDIDev = INO_pc_SLOT_DIDEV_A(sSlot);
        pVtbl = *(INO_pc_tdstDIDeviceVtbl **)pDIDev;
        hr = (int)pVtbl->GetDeviceState(pDIDev, 0x50, &stState);
    }

    if (hr < -0x7ff8fff3) {
        if (hr == -0x7ff8fff4) {
            pstJoy->ucStatus &= 0xfd;
            return 0;
        }
        if (hr != -0x7ffffff6) {
            pstJoy->ucStatus = (unsigned char)((pstJoy->ucStatus & 0xfd) | 4);
            return 0;
        }
    } else {
        if (hr == -0x7ff8ffe2) {
            pstJoy->ucStatus &= 0xfd;
            return 0;
        }
        if (hr != 0) {
            pstJoy->ucStatus = (unsigned char)((pstJoy->ucStatus & 0xfd) | 4);
            return 0;
        }
    }

    pstCaps = (INO_pc_tdstAxisDeviceCaps *)pDevice->p_stCaps;
    pstJoy->a_lAxis[0] = stState.lX - pstCaps->a_stAxis[0].m_lCenter;
    pstJoy->a_lAxis[1] = stState.lY - pstCaps->a_stAxis[1].m_lCenter;
    pstJoy->a_lAxis[2] = stState.lZ - pstCaps->a_stAxis[2].m_lCenter;
    pstJoy->a_lAxis[3] = stState.lRx - pstCaps->a_stAxis[3].m_lCenter;
    pstJoy->a_lAxis[4] = stState.lRy - pstCaps->a_stAxis[4].m_lCenter;
    pstJoy->a_lAxis[5] = stState.lRz - pstCaps->a_stAxis[5].m_lCenter;

    pstJoy->ulButtons = 0UL;
    i = 0;
    if (pDevice->p_stCaps->ucNbButtons != 0) {
        do {
            if ((stState.rgbButtons[(short)i] & 0x80) != 0) {
                pstJoy->ulButtons |= 1UL << i;
            }
            i++;
        } while ((short)i < (short)pDevice->p_stCaps->ucNbButtons);
    }
    if ((pstJoy->uwDirection & C_ulJoyZDown) == C_ulJoyZDown) {
        pstJoy->ulButtons |= 0x10UL;
    }
    return 1;
}
#endif /* USE_SDL */
