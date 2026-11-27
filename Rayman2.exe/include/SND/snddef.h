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

#endif
