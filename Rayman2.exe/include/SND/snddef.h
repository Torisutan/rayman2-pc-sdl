#ifndef SND_SNDDEF_H
#define SND_SNDDEF_H

/* Platform selection as in the original engine's snddef.h (docs/PLAN.md Phase 5, platform macros) */
#if defined(U64) || defined(NITRO) || defined(PSX) || defined(DREAMCAST) || defined(PSP) || defined(WIIU)
#define GAMECONSOLE
#endif /* U64 || NITRO || PSX || DREAMCAST || PSP || WIIU */

/* Win32 API for files, ini files and memory (CreateFile, profile strings, GlobalAlloc), unless
   a target or RAYMAN2_C_RUNTIME asks for the C runtime */
#if !defined(GAMECONSOLE) && !defined(USE_SDL) && !defined(RAYMAN2_C_RUNTIME)
#define FILE_MODE_C_WIN95
#endif /* !GAMECONSOLE && !USE_SDL && !RAYMAN2_C_RUNTIME */

#define NB_MICRO_MAX 2

#define SND_CALL WINAPI

typedef void (__stdcall *SND_td_pfn_vSoundCallback)(long par);

#define SndBool_field_decl(x) SndBool x

#if defined(USE_SDL)
/* SDL backend (SND/sdl_snd.c): locks, events, the critical section, threads, the assert box and
 * the driver tests. The init struct's instance and the drivers' PCM format stay the types the
 * sample driver's SDL branch shares with it (its SND/snddef.h). */
#include "SND/sdl_snd.h"
#define SND_pc_tdhLock					void *
#define SND_pc_tdhEvent					void *
#define SND_pc_tdhWaitable				void *
#define SND_pc_tdstCriticalSection		void *
#define SND_pc_tdhWindow				HWND
#define SND_pc_tdhInstance				HINSTANCE
#define SND_pc_tdstThreadSecurity		int		/* SDL threads have no security attributes */
#define SND_pc_tdpfnThreadProc			SND_tdpfnThreadProcSnd
#define SND_pc_tdstOsVersion			SND_tdstOsVersionSnd
#define SND_pc_tdstWaveFormat			WAVEFORMATEX
#define SND_pc_tdstDeviceCaps			SND_tdstDeviceCapsSnd

#define SND_pc_C_ulWaitForever			SND_C_ulWaitForeverSnd

#define SND_pc_M_hCreateLock(bOwned)			SND_fn_hCreateLockSnd(bOwned)
#define SND_pc_M_ulWaitLock(hLock,ulMs)			SND_fn_ulWaitOneSnd((hLock),(ulMs))
#define SND_pc_M_bReleaseLock(hLock)			SND_fn_bReleaseLockSnd(hLock)
#define SND_pc_M_bDeleteLock(hLock)				SND_fn_bDeleteLockSnd(hLock)

#define SND_pc_M_vInitCriticalSection(p_stCs)	SND_fn_vInitCriticalSectionSnd(p_stCs)
#define SND_pc_M_vDeleteCriticalSection(p_stCs)	SND_fn_vDeleteCriticalSectionSnd(p_stCs)
#define SND_pc_M_vEnterCriticalSection(p_stCs)	SND_fn_vEnterCriticalSectionSnd(p_stCs)
#define SND_pc_M_vLeaveCriticalSection(p_stCs)	SND_fn_vLeaveCriticalSectionSnd(p_stCs)

#define SND_pc_M_hCreateManualEvent()			SND_fn_hCreateManualEventSnd()
#define SND_pc_M_bSetEvent(hEvent)				SND_fn_bSetEventSnd(hEvent)
#define SND_pc_M_bResetEvent(hEvent)			SND_fn_bResetEventSnd(hEvent)
#define SND_pc_M_ulWaitEvent(hEvent,ulMs)		SND_fn_ulWaitOneSnd((hEvent),(ulMs))
#define SND_pc_M_bDeleteEvent(hEvent)			SND_fn_bDeleteEventSnd(hEvent)

#define SND_pc_M_ulWaitAny(ulNb,a_hObjects,ulMs)	SND_fn_ulWaitAnySnd((ulNb),(a_hObjects),(ulMs))

#define SND_pc_M_vSleep(ulMs)					SND_fn_vSleepSnd(ulMs)
#define SND_pc_M_ulGetCurrentThreadId()			SND_fn_ulGetCurrentThreadIdSnd()
#define SND_pc_M_ulGetLastError()				SND_fn_ulGetLastErrorSnd()

#define SND_pc_M_vInitThreadSecurity(stSecurity)	((stSecurity) = 0)
#define SND_pc_M_hCreateThread(p_stSecurity,pfnProc,pvParam,p_ulId)	SND_fn_hCreateThreadSnd((pfnProc),(pvParam),(p_ulId))

#define SND_pc_M_vAssertBox(hWnd,szText,szCaption)	SND_fn_vAssertBoxSnd((szText),(szCaption))
#define SND_pc_M_vBreak()						SND_fn_vBreakSnd()

#define SND_pc_M_bGetOsVersion(p_stVersion)		SND_fn_bGetOsVersionSnd(p_stVersion)
#define SND_pc_M_ulGetDeviceCaps(p_stCaps)		SND_fn_ulGetDeviceCapsSnd(p_stCaps)
#else
#define SND_pc_tdhLock					HANDLE
#define SND_pc_tdhEvent					HANDLE
#define SND_pc_tdhWaitable				HANDLE
#define SND_pc_tdstCriticalSection		CRITICAL_SECTION
#define SND_pc_tdhWindow				HWND
#define SND_pc_tdhInstance				HINSTANCE
#define SND_pc_tdstThreadSecurity		SECURITY_ATTRIBUTES
#define SND_pc_tdpfnThreadProc			LPTHREAD_START_ROUTINE
#define SND_pc_tdstOsVersion			OSVERSIONINFOA
#define SND_pc_tdstWaveFormat			WAVEFORMATEX
#define SND_pc_tdstDeviceCaps			WAVEOUTCAPSA

#define SND_pc_C_ulWaitForever			INFINITE

#define SND_pc_M_hCreateLock(bOwned)			CreateMutexA(NULL,(bOwned),NULL)
#define SND_pc_M_ulWaitLock(hLock,ulMs)			WaitForSingleObject((hLock),(ulMs))
#define SND_pc_M_bReleaseLock(hLock)			ReleaseMutex(hLock)
#define SND_pc_M_bDeleteLock(hLock)				CloseHandle(hLock)

#define SND_pc_M_vInitCriticalSection(p_stCs)	InitializeCriticalSection(p_stCs)
#define SND_pc_M_vDeleteCriticalSection(p_stCs)	DeleteCriticalSection(p_stCs)
#define SND_pc_M_vEnterCriticalSection(p_stCs)	EnterCriticalSection(p_stCs)
#define SND_pc_M_vLeaveCriticalSection(p_stCs)	LeaveCriticalSection(p_stCs)

#define SND_pc_M_hCreateManualEvent()			CreateEventA(NULL,1,0,NULL)
#define SND_pc_M_bSetEvent(hEvent)				SetEvent(hEvent)
#define SND_pc_M_bResetEvent(hEvent)			ResetEvent(hEvent)
#define SND_pc_M_ulWaitEvent(hEvent,ulMs)		WaitForSingleObject((hEvent),(ulMs))
#define SND_pc_M_bDeleteEvent(hEvent)			CloseHandle(hEvent)

#define SND_pc_M_ulWaitAny(ulNb,a_hObjects,ulMs)	WaitForMultipleObjects((ulNb),(a_hObjects),0,(ulMs))

#define SND_pc_M_vSleep(ulMs)					Sleep(ulMs)
#define SND_pc_M_ulGetCurrentThreadId()			GetCurrentThreadId()
#define SND_pc_M_ulGetLastError()				GetLastError()

#define SND_pc_M_vInitThreadSecurity(stSecurity)	((stSecurity).nLength = sizeof(SECURITY_ATTRIBUTES), \
	(stSecurity).bInheritHandle = TRUE, (stSecurity).lpSecurityDescriptor = NULL)
#define SND_pc_M_hCreateThread(p_stSecurity,pfnProc,pvParam,p_ulId)	CreateThread((p_stSecurity),0,(pfnProc),(pvParam),0,(p_ulId))

#define SND_pc_M_vAssertBox(hWnd,szText,szCaption)	MessageBoxA((hWnd),(szText),(szCaption),0x40000)
#define SND_pc_M_vBreak()						DebugBreak()

#define SND_pc_M_bGetOsVersion(p_stVersion)		GetVersionExA(p_stVersion)
#define SND_pc_M_ulGetDeviceCaps(p_stCaps)		waveOutGetDevCapsA(WAVE_MAPPER,(p_stCaps),sizeof(*(p_stCaps)))
#endif /* USE_SDL */

#endif
