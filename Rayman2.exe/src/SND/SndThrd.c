#include <windows.h>
#include <stdio.h>
#include <string.h>

#include "SND/SND.h"
#include "SND/sndres.h"
#include "SND/sndparam_pc.h"
#include "ERM/CpaError.h"

#include "SND/snderr.h"

typedef struct SND_pc_tdstMouchard_ {
    char          szFile[128];
    long          lLine;
    unsigned long ulThreadId;
} SND_pc_tdstMouchard;

static SND_pc_tdstCriticalSection SND_pc_g_stErrDispCs;   /* 0x004B8508 */
static SND_pc_tdhLock      SND_pc_g_hSoundMutex;          /* 0x004B8520 */
static unsigned long       SND_pc_g_ulMutexLocked;        /* 0x004B8524 */
static unsigned long       SND_pc_g_ulMutexWaitFlag;      /* 0x004B8528 */
static SND_pc_tdstMouchard SND_pc_g_stMutexMouchard;      /* 0x004B8530 */
static unsigned long       SND_pc_g_ulThreadInitDone;     /* 0x004B85B8 */
static SND_pc_tdhEvent     SND_pc_g_hErrDispBusyEvent;    /* 0x004B85BC */
static SND_pc_tdstMouchard SND_pc_g_stErrDispMouchard;    /* 0x004B85C0 */

static const char SND_C_szLastCallFmt[] =
    "Last call by thread %x (line %d of %s)";
static const char SND_C_szDeadLockMsg[] =
    "DeadLock occured when waiting for Sound Mutex !\n"
    "Program will continue although";
static const char SND_C_szSndThrdSrcFile[] =
    "..\\..\\..\\src\\Win95\\sndthrd.c";
static const char SND_C_szEmptyThrd[] = "";

void __stdcall SND_fn_vMouchardThreadsnd(char *szFile, long lLine)
{
    strncpy(SND_pc_g_stMutexMouchard.szFile, szFile, 0x80);
    SND_pc_g_stMutexMouchard.lLine      = lLine;
    SND_pc_g_stMutexMouchard.ulThreadId = SND_pc_M_ulGetCurrentThreadId();
}

void __stdcall SND_pc_fn_vMouchardErrorDisplay(char *szFile, long lLine)
{
    strncpy(SND_pc_g_stErrDispMouchard.szFile, szFile, 0x80);
    SND_pc_g_stErrDispMouchard.lLine      = lLine;
    SND_pc_g_stErrDispMouchard.ulThreadId = SND_pc_M_ulGetCurrentThreadId();
}

void __stdcall SND_fn_vEnterCriticalSectionThreadSnd_(void)
{
    char szBuffer[256];

    if (SND_pc_g_ulThreadInitDone == 0) {
        SND_fn_vDisplayError(E_uwSndInitNotDoneError, (char *)SND_C_szEmptyThrd);
    }
    if (SND_pc_g_hSoundMutex != NULL) {
        unsigned long ulResult;

        ulResult = SND_pc_M_ulWaitLock(SND_pc_g_hSoundMutex, SND_pc_C_ulWaitForever);
        SND_pc_g_ulMutexWaitFlag = 0;
        if (ulResult == 0) {
            SND_pc_g_ulMutexLocked = 1;
        } else {
            SND_pc_M_ulGetLastError();
            sprintf(szBuffer, SND_C_szLastCallFmt,
                    (unsigned int)SND_pc_g_stMutexMouchard.ulThreadId,
                    (int)SND_pc_g_stMutexMouchard.lLine,
                    SND_pc_g_stMutexMouchard.szFile);
            SND_fn_vDisplayError(E_uwSndThreadDeadLockError, szBuffer);
        }
    } else {
        SND_fn_vDisplayError(E_uwSndThreadMultiThreadDetectedError, szBuffer);
    }
}

int __stdcall SND_fn_bTryToEnterCriticalSectionThreadSnd(void)
{
    char szBuffer[256];

    if (SND_pc_g_ulThreadInitDone == 0) {
        SND_fn_vDisplayError(E_uwSndInitNotDoneError, (char *)SND_C_szEmptyThrd);
    }
    if (SND_pc_g_hSoundMutex != NULL) {
        unsigned long ulResult;

        sprintf(szBuffer, SND_C_szLastCallFmt,
                (unsigned int)SND_pc_g_stMutexMouchard.ulThreadId,
                (int)SND_pc_g_stMutexMouchard.lLine,
                SND_pc_g_stMutexMouchard.szFile);
        ulResult = SND_pc_M_ulWaitLock(SND_pc_g_hSoundMutex, 0x64);
        SND_pc_g_ulMutexWaitFlag = 0;
        if (ulResult == 0) {
            SND_pc_g_ulMutexLocked = 1;
            return 1;
        }
        SND_pc_M_ulGetLastError();
        SND_fn_vDisplayError(E_uwSndSystemError, (char *)SND_C_szDeadLockMsg);
        return 0;
    }
    SND_fn_vDisplayError(E_uwSndThreadMultiThreadDetectedError, szBuffer);
    return 0;
}

void __stdcall SND_fn_vQuitCriticalSectionThreadSnd(void)
{
    SND_pc_g_ulMutexLocked = 0;
    SND_pc_M_bReleaseLock(SND_pc_g_hSoundMutex);
}

void __stdcall SND_fn_vEnterCriticalSectionForErrorDisplay(void)
{
    SND_pc_M_vEnterCriticalSection(&SND_pc_g_stErrDispCs);
    SND_pc_M_bSetEvent(SND_pc_g_hErrDispBusyEvent);
    SND_pc_fn_vMouchardErrorDisplay((char *)SND_C_szSndThrdSrcFile, 0x13A);
}

void __stdcall SND_fn_vQuitCriticalSectionForErrorDisplay(void)
{
    SND_pc_M_bResetEvent(SND_pc_g_hErrDispBusyEvent);
    SND_pc_M_vLeaveCriticalSection(&SND_pc_g_stErrDispCs);
}

int __stdcall SND_fn_bEnterCriticalSectionForDriverThreadSnd(void)
{
    SND_pc_tdhWaitable aHandles[2];
    char   szBuffer[256];

    if (SND_pc_g_ulThreadInitDone == 0) {
        SND_fn_vDisplayError(E_uwSndInitNotDoneError, (char *)SND_C_szEmptyThrd);
    }
    if (SND_pc_g_hSoundMutex != NULL && SND_pc_g_hErrDispBusyEvent != NULL) {
        sprintf(szBuffer, SND_C_szLastCallFmt,
                (unsigned int)SND_pc_g_stMutexMouchard.ulThreadId,
                (int)SND_pc_g_stMutexMouchard.lLine,
                SND_pc_g_stMutexMouchard.szFile);
        aHandles[0] = SND_pc_g_hSoundMutex;
        aHandles[1] = SND_pc_g_hErrDispBusyEvent;
        if (SND_pc_M_ulWaitAny(2, aHandles, SND_pc_C_ulWaitForever) == 0) {
            if (SND_pc_M_ulWaitEvent(SND_pc_g_hErrDispBusyEvent, 0) != 0) {
                SND_pc_g_ulMutexWaitFlag = 0;
                SND_pc_g_ulMutexLocked   = 1;
                return 1;
            }
            SND_pc_M_bReleaseLock(SND_pc_g_hSoundMutex);
        }
    }
    return 0;
}

typedef const char * (*tdpfnResNameCallback)(unsigned long);

long __cdecl SND_fn_bGetAskedTheme(unsigned long *pulOut1, unsigned long *pulOut2);
long __cdecl SND_fn_bGetThemePosition(unsigned long *pulOut1, unsigned long *pulOut2, unsigned long *pulOut3);

/* 0x004B8648 */
static tdpfnResNameCallback SND_pc_g_pfnResNameCallback;

extern void *__cdecl SND_pc_fn_pstFindBankItemById(long lKey);                 /* 0x00454380 */
extern int   __cdecl SND_pc_fn_iSetResourceVolumeByType(tdstBlockResourceMem *pRes, long lVolume); /* 0x004585B0 */

/* 0x00454EE0 */
extern long __stdcall SND_fn_rGetDopplerFactor(void);

void __stdcall SND_fn_vWaitForValueThreadSnd(unsigned long *p_ulValue,
                                             unsigned long ulExpected)
{
    long lTries = 0;

    while ((*p_ulValue != ulExpected) && (lTries < 10)) {
        lTries++;
        SND_pc_M_vSleep(0x64);
    }
    if (*p_ulValue != ulExpected) {
        SND_fn_vDisplayError(E_uwSndThreadDeadLockError,
                             (char *)SND_C_szEmptyThrd);
    }
}

void __cdecl SND_pc_fn_vInitErrorDisplayCs(void)
{
    SND_pc_M_vInitCriticalSection(&SND_pc_g_stErrDispCs);
    SND_pc_g_hErrDispBusyEvent = SND_pc_M_hCreateManualEvent();
}

void __cdecl SND_pc_fn_vDesInitErrorDisplayCs(void)
{
    SND_pc_tdhEvent hEvent = SND_pc_g_hErrDispBusyEvent;

    SND_pc_M_vLeaveCriticalSection(&SND_pc_g_stErrDispCs);
    SND_pc_M_vDeleteCriticalSection(&SND_pc_g_stErrDispCs);
    SND_pc_g_hErrDispBusyEvent = NULL;
    SND_pc_M_bResetEvent(hEvent);
    SND_pc_M_bDeleteEvent(hEvent);
}

/* 0x00454E00 */
SndBool __stdcall dbgSND_fn_bGetAskedTheme(tduRefRes *asked_theme, SndBool *transiting)
{
    return SND_fn_bGetAskedTheme((unsigned long *)asked_theme, (unsigned long *)transiting);
}

/* 0x00454E20 */
SndBool __stdcall dbgSND_fn_bGetThemePosition(tduRefRes *current_theme, long *id_part, long *id_loop)
{
    return SND_fn_bGetThemePosition((unsigned long *)current_theme, (unsigned long *)id_part, (unsigned long *)id_loop);
}

/* 0x00454E40 */
void __stdcall dbgSND_fn_vGetInfoForObjectSound(long id, long type, char *str, long size_ptr)
{
    SND_fn_vGetInfoForObjectSound(id, type, str, size_ptr);
}

/* 0x00454E60 */
long __stdcall dbgSND_fn_lGetSizeOfSoundParam(struct SoundParam_ *pObj)
{
    return SND_M_lGetSizeOfSoundParam(pObj);
}

/* 0x00454E80 */
void __stdcall SND_fn_vSetResNameCallback(tdpfnResNameCallback pfn)
{
    SND_pc_g_pfnResNameCallback = pfn;
}

/* 0x00454E90 */
const char * __stdcall SND_fn_czGetResNameFromEdId(unsigned long edId)
{
    if (SND_pc_g_pfnResNameCallback == NULL)
    {
        return "ResName unavailable";
    }
    return SND_pc_g_pfnResNameCallback(edId);
}
SndBool __stdcall dbgSND_fn_bSetResourceStaticVolume(long lResId, long lVolume)
{
    void *pItem = SND_pc_fn_pstFindBankItemById(lResId);

    if (pItem != NULL)
        return (SndBool)SND_pc_fn_iSetResourceVolumeByType((tdstBlockResourceMem *)pItem, lVolume);
    return 0;
}
long __stdcall dbgSND_fn_rGetDopplerFactor(void)
{
    return SND_fn_rGetDopplerFactor();
}

/* 0x00454EF0 */
void __stdcall dbgSND_fn_vSendRequestSound(void *pEvt)
{
    SND_fn_lSendRequestSound(-1, 0, pEvt, 0, NULL);
}

void __stdcall SND_fn_vInitThreadSnd(void)
{
    SND_pc_g_hSoundMutex      = SND_pc_M_hCreateLock(1);
    SND_pc_g_ulMutexLocked    = 0;
    SND_pc_g_ulThreadInitDone = 1;
    SND_pc_g_ulMutexWaitFlag  = 0;
    SND_pc_fn_vInitErrorDisplayCs();
}

void __stdcall SND_fn_vDesInitThreadSnd(void)
{
    SND_pc_tdhLock hMutex;

    SND_pc_fn_vDesInitErrorDisplayCs();
    hMutex = SND_pc_g_hSoundMutex;
    SND_pc_g_hSoundMutex = NULL;
    SND_pc_M_bReleaseLock(hMutex);
    SND_pc_M_bDeleteLock(hMutex);
}
