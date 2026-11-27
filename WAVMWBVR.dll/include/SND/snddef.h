#ifndef _SNDDEF_H_
#define _SNDDEF_H_

#if  defined(U64) || defined(NITRO) || defined(PSX) || defined(DREAMCAST)
#define GAMECONSOLE
#endif

#if (!defined(GAMECONSOLE))  || defined(WIN32) || defined(WIN95)
#include <windows.h>
#endif

#if !defined(GAMECONSOLE)
#include <stdio.h>
#include <string.h>
#include <memory.h>
#else

#if defined(U64) || defined(NITRO)
#include "Ultra.h"
#endif

#if (defined(DREAMCAST))
#include <stdio.h>
#include <string.h>
#include "dreaminc.h"
#endif

#if (defined(PSX))
#include <stdio.h>
#include <string.h>
#include <memory.h>
#endif

#endif

#if !defined(GAMECONSOLE) && !defined(GENERIC)

#if defined(_DLL_COMPILATION_MODE)
#define SUPERVISATER
#endif

#define SND_DISPLAY_ERROR_MESSAGE
#define NB_MICRO_MAX 2

#endif

#if defined(U64) || defined(NITRO)

#define SND_BINARY_VERSION

#define DISABLE_ADPCM
#define DISABLE_MPEG

#define DISABLE_CD

#define DISABLE_THEME_WAVE
#define DISABLE_BUFFER_CLIENT

#define DISABLE_SNDSTRM
#define DISABLE_MULTIWAVFORMAT
#ifndef RETAIL
#define SND_PROFILE
#endif
#define RUN_TIME_LOADING
#define DISABLE_RECORD

#define NB_MICRO_MAX 2

#endif

#ifdef PSX

#define SND_BINARY_VERSION
#define DISABLE_MULTIWAVFORMAT

#define DISABLE_ADPCM
#define DISABLE_MPEG
#define DISABLE_MIDI

#define DISABLE_SEQUENCE

#define DISABLE_THEME_WAVE

#define DISABLE_WAVE
#define DISABLE_THEME_MIDI

#define SUPERVISATER
#define DISABLE_RECORD

#define NB_MICRO_MAX 2

#endif

#ifdef DREAMCAST

#define DISABLE_MPEG
#define DISABLE_MIDI

#define DISABLE_THEME_WAVE
#define DISABLE_BUFFER_CLIENT
#define DISABLE_THEME_MIDI
#define DISABLE_SEQUENCE
#define DISABLE_ADPCM
#define DISABLE_SPLIT
#define DISABLE_MULTIWAVFORMAT

#define DISABLE_RECORD
#define SND_DISPLAY_ERROR_MESSAGE
#define NB_MICRO_MAX 2

#endif

#ifdef GENERIC

#define SND_BINARY_VERSION
#define DISABLE_ADPCM
#define DISABLE_MPEG
#define DISABLE_SEQUENCE
#define DISABLE_CD
#define DISABLE_SNDSTRM
#define SND_DISPLAY_ERROR_MESSAGE

#endif

#if !defined(SND_SCRIPT_VERSION) && !defined(SND_HYBRID_VERSION) && !defined(SND_BINARY_VERSION)
#define SND_HYBRID_VERSION
#endif

#ifdef SND_SCRIPT_VERSION
#define NO_ACP_LDBIN
#if defined(NO_ACP_SCRIPT) || defined(SND_HYBRID_VERSION) || defined (SND_BINARY_VERSION)
#error Macros not coherent
#endif
#endif

#ifdef SND_HYBRID_VERSION
#if defined(NO_ACP_LDBIN) || defined(NO_ACP_SCRIPT) || defined(SND_SCRIPT_VERSION) || defined(SND_BINARY_VERSION)
#error Macros not coherent
#endif
#endif

#ifdef SND_BINARY_VERSION
#define NO_ACP_SCRIPT
#if defined(NO_ACP_LDBIN) || defined(SND_HYBRID_VERSION) || defined(SND_SCRIPT_VERSION)
#error Macros not coherent
#endif
#endif

#if !defined(GAMECONSOLE)  || defined(GENERIC)
#define FILE_MODE_C_WIN95
#if (defined(_MSC_VER) && !defined(VISUAL))     || (defined(__WACTOMC__) && !defined(WATCOM))
#pragma message("Macros definitions are not homogenious")
#endif
#endif

#if defined(GAMECONSOLE)
#undef M_SET_ALIGNMENT
#else
#define M_SET_ALIGNMENT 4
#endif

#ifdef NO_ACP
#undef NO_ACP_SCRIPT
#define NO_ACP_SCRIPT
#undef NO_ACP_MEMORY
#define NO_ACP_MEMORY
#undef NO_ACP_ERROR
#define NO_ACP_ERROR
#endif

#ifdef NO_ACP_SOUND
#define NO_ACP_SCRIPT
#define NO_ACP_MEMORY
#define NO_ACP_ERROR
#define NO_ACP_LDBIN
#define DISABLE_MPEG
#define DISABLE_MIDI
#define DISABLE_CD
#define DISABLE_WAVE
#endif

#ifdef NO_ACP_ERROR
#undef NO_ACP_MEMORY
#define NO_ACP_MEMORY
#endif

#define SND_C_RES_FANTOME_PTR NULL
#define SND_C_RES_FANTOME_ID -1

#if !defined(GAMECONSOLE) && !defined(GENERIC)

#if defined(USE_SDL)
/* SDL backend (sdl_sndwave.c): locks, events and the output device are its objects; the PCM format
 * and the buffer header are its structs with the fields the shared code fills; the instance is only
 * passed on, and no message is posted (SND_pc_M_bPostMessage below), so a parameter is a pointer. */
#include "sdl_sndwave.h"
#define SND_pc_tdhLock					void *
#define SND_pc_tdhEvent					void *
#define SND_pc_tdhThread				void *
#define SND_pc_tdhWaitable				void *
#define SND_pc_tdstCriticalSection		void *
#define SND_pc_tdstPerfCounter			SND_tdstPerfCounter
#define SND_pc_tdhDevice				void *
#define SND_pc_tdhWindow				HWND
#define SND_pc_tdhInstance				void *
#define SND_pc_tdstWaveFormat			SND_tdstWaveFormat
#define SND_pc_tdstBufferHeader			SND_tdstBufferHeader
#define SND_pc_tdulMessageParam			void *
#define SND_pc_tdlMessageParam			void *
#else
#define SND_pc_tdhLock					HANDLE
#define SND_pc_tdhEvent					HANDLE
#define SND_pc_tdhThread				HANDLE
#define SND_pc_tdhWaitable				HANDLE
#define SND_pc_tdstCriticalSection		CRITICAL_SECTION
#define SND_pc_tdstPerfCounter			LARGE_INTEGER
#define SND_pc_tdhDevice				HWAVEOUT
#define SND_pc_tdhWindow				HWND
#define SND_pc_tdhInstance				HINSTANCE
#define SND_pc_tdstWaveFormat			WAVEFORMATEX
#define SND_pc_tdstBufferHeader			WAVEHDR
#define SND_pc_tdulMessageParam			WPARAM
#define SND_pc_tdlMessageParam			LPARAM
#endif /* USE_SDL */
#endif

typedef struct _SND_tdstInitStruct
{
#if !defined(GAMECONSOLE) && !defined(GENERIC)
        SND_pc_tdhInstance hProcessInstance;
        SND_pc_tdhWindow hMainWindow;
        SND_pc_tdhLock hMutexDX;
#else
        int bidon;
#endif

} SND_tdstInitStruct;

#if !defined(GAMECONSOLE) || defined(WIN95) || defined(WIN32)

#define SND_CALL WINAPI
#else
#define SND_CALL
#endif

#if !defined(GAMECONSOLE) && !defined(GENERIC)

#if defined(USE_SDL)
/* SDL backend (sdl_sndwave.c): output device, locks, events, the voice-end callback thread, timing and
 * the error box. The SDL output has no message thread: every caller counts as the driver's thread (one
 * thread id), so taking and releasing the device always goes the direct way, under the sample lock, and
 * nothing is posted (the waveOut thread's messages have no receiver here).
 * APM's critical section is an SDL mutex. */
#define SND_pc_C_ulWaitForever			SND_C_ulWaitForever
#define SND_pc_C_ulWaitTimedOut			SND_C_ulWaitTimedOut

#define SND_pc_M_hCreateLock(bOwned)			SND_fn_hCreateLock(bOwned)
#define SND_pc_M_ulWaitLock(hLock,ulMs)			SND_fn_ulWaitLock((hLock),(ulMs))
#define SND_pc_M_bReleaseLock(hLock)			SND_fn_bReleaseLock(hLock)
#define SND_pc_M_bDeleteLock(hLock)				SND_fn_bDeleteLock(hLock)

#define SND_pc_M_vInitCriticalSection(p_stCs)	SND_fn_vInitCriticalSection(p_stCs)
#define SND_pc_M_vDeleteCriticalSection(p_stCs)	SND_fn_vDeleteCriticalSection(p_stCs)
#define SND_pc_M_vEnterCriticalSection(p_stCs)	SND_fn_vEnterCriticalSection(p_stCs)
#define SND_pc_M_vLeaveCriticalSection(p_stCs)	SND_fn_vLeaveCriticalSection(p_stCs)

#define SND_pc_M_ulLockOrBreak(hLock)			SND_pc_fn_ulWaitObjectOrBreak((hLock),SND_C_ulWaitForever)
#define SND_pc_M_ulWaitEventOrBreak(hEvent)		SND_fn_ulWaitEventOrBreak(hEvent)

#define SND_pc_M_hCreateEvent()					SND_fn_hCreateEvent()
#define SND_pc_M_bSetEvent(hEvent)				SND_fn_bSetEvent(hEvent)
#define SND_pc_M_bDeleteEvent(hEvent)			SND_fn_bDeleteEvent(hEvent)

#define SND_pc_M_ulGetCurrentThreadId()			SND_C_ulDriverThreadId
#define SND_pc_M_bPostMessage(ulThreadId,uiMsg,wParam,lParam)	FALSE

#define SND_pc_M_ulOpenDevice(p_hDevice,p_stFormat)	SND_fn_ulOpenDevice((p_hDevice),(p_stFormat)->nSamplesPerSec,(p_stFormat)->nChannels,(p_stFormat)->wBitsPerSample)
#define SND_pc_M_ulPrepareBuffer(hDevice,p_stHdr)	0
#define SND_pc_M_ulWriteBuffer(hDevice,p_stHdr)		SND_fn_ulWriteDevice((hDevice),(p_stHdr)->lpData,(p_stHdr)->dwBufferLength)
#define SND_pc_M_ulResetDevice(hDevice)				SND_fn_ulResetDevice(hDevice)
#define SND_pc_M_ulCloseDevice(hDevice)				SND_fn_ulCloseDevice(hDevice)
#define SND_pc_M_ulGetDeviceErrorText(ulErr,szText,ulSize)	SND_fn_ulGetDeviceErrorText((ulErr),(szText),(ulSize))

#define SND_pc_M_bGetPerfCounter(p_stCounter)	SND_fn_bGetPerfCounter(p_stCounter)
#define SND_pc_M_bGetPerfFrequency(p_stFreq)	SND_fn_bGetPerfFrequency(p_stFreq)

#define SND_pc_M_pvAllocCallback(ulSize)		malloc(ulSize)
#define SND_pc_M_vFreeCallback(pvBlock)			free(pvBlock)
#define SND_pc_M_vStartCallbackThread(pfnProc,pvParam)	SND_fn_vStartCallbackThread((pfnProc),(pvParam))

#define SND_pc_M_vErrorBox(szText,szCaption)	SND_fn_vErrorBox((szText),(szCaption))
#define SND_pc_M_vBreak()						SND_fn_vBreak()
#else
#define SND_pc_C_ulWaitForever			INFINITE
#define SND_pc_C_ulWaitTimedOut			WAIT_TIMEOUT

#define SND_pc_M_hCreateLock(bOwned)			CreateMutexA(NULL,(bOwned),NULL)
#define SND_pc_M_ulWaitLock(hLock,ulMs)			WaitForSingleObject((hLock),(ulMs))
#define SND_pc_M_bReleaseLock(hLock)			ReleaseMutex(hLock)
#define SND_pc_M_bDeleteLock(hLock)				CloseHandle(hLock)

#define SND_pc_M_vInitCriticalSection(p_stCs)	InitializeCriticalSection(p_stCs)
#define SND_pc_M_vDeleteCriticalSection(p_stCs)	DeleteCriticalSection(p_stCs)
#define SND_pc_M_vEnterCriticalSection(p_stCs)	EnterCriticalSection(p_stCs)
#define SND_pc_M_vLeaveCriticalSection(p_stCs)	LeaveCriticalSection(p_stCs)

#define SND_pc_M_ulLockOrBreak(hLock)			SND_pc_fn_ulWaitObjectOrBreak((hLock),INFINITE)
#define SND_pc_M_ulWaitEventOrBreak(hEvent)		SND_pc_fn_ulWaitObjectOrBreak((hEvent),INFINITE)
#define SND_pc_M_ulJoinThreadOrBreak(hThread)	SND_pc_fn_ulWaitObjectOrBreak((hThread),INFINITE)

#define SND_pc_M_hCreateEvent()					CreateEventA(NULL,FALSE,FALSE,NULL)
#define SND_pc_M_bSetEvent(hEvent)				SetEvent(hEvent)
#define SND_pc_M_bDeleteEvent(hEvent)			CloseHandle(hEvent)

#define SND_pc_M_hCreateThread(pfnProc,pvParam,p_ulId)	CreateThread(NULL,0,(pfnProc),(pvParam),0,(p_ulId))
#define SND_pc_M_bThreadEnded(hThread)			(WaitForSingleObject((hThread),0) == WAIT_OBJECT_0)
#define SND_pc_M_bDeleteThread(hThread)			CloseHandle(hThread)
#define SND_pc_M_ulGetCurrentThreadId()			GetCurrentThreadId()
#define SND_pc_M_vRaiseThreadPriority()			SetThreadPriority(GetCurrentThread(),THREAD_PRIORITY_ABOVE_NORMAL)
#define SND_pc_M_vExitThread()					ExitThread(0)
#define SND_pc_M_uiRegisterMessage(szName)		RegisterWindowMessageA(szName)
#define SND_pc_M_bPostMessage(ulThreadId,uiMsg,wParam,lParam)	PostThreadMessageA((ulThreadId),(uiMsg),(wParam),(lParam))
#define SND_pc_M_bGetMessage(p_stMsg)			GetMessageA((p_stMsg),NULL,0,0)
#define SND_pc_M_bPeekMessage(p_stMsg)			PeekMessageA((p_stMsg),NULL,0,0,PM_NOREMOVE)

#define SND_pc_M_ulOpenDevice(p_hDevice,p_stFormat)	waveOutOpen((p_hDevice),WAVE_MAPPER,(p_stFormat),GetCurrentThreadId(),0,CALLBACK_THREAD)
#define SND_pc_M_ulPrepareBuffer(hDevice,p_stHdr)	waveOutPrepareHeader((hDevice),(p_stHdr),sizeof(WAVEHDR))
#define SND_pc_M_ulWriteBuffer(hDevice,p_stHdr)		waveOutWrite((hDevice),(p_stHdr),sizeof(WAVEHDR))
#define SND_pc_M_ulResetDevice(hDevice)				waveOutReset(hDevice)
#define SND_pc_M_ulCloseDevice(hDevice)				waveOutClose(hDevice)
#define SND_pc_M_ulGetDeviceErrorText(ulErr,szText,ulSize)	waveOutGetErrorTextA((ulErr),(szText),(ulSize))

#define SND_pc_M_bGetPerfCounter(p_stCounter)	QueryPerformanceCounter(p_stCounter)
#define SND_pc_M_bGetPerfFrequency(p_stFreq)	QueryPerformanceFrequency(p_stFreq)

#define SND_pc_M_pvAllocCallback(ulSize)		GlobalAlloc(GMEM_FIXED,(ulSize))
#define SND_pc_M_vFreeCallback(pvBlock)			GlobalFree(pvBlock)
#define SND_pc_M_vStartCallbackThread(pfnProc,pvParam)	_beginthread((pfnProc),0,(pvParam))

#define SND_pc_M_vErrorBox(szText,szCaption)	MessageBoxA(NULL,(szText),(szCaption),0)
#define SND_pc_M_vBreak()						DebugBreak()
#endif /* USE_SDL */
#endif

typedef unsigned long tdxId;

typedef void (SND_CALL *SND_td_pfn_vSoundCallback)(long par);

#define UNINITIALIZED_VALUE 0x12345678

#ifdef PSX
#define SndBool_field_decl(x) SndBool x:1
#define TRUE  1
#define FALSE 0
#define PSX_EDITOR_TRUE 1
typedef int SndBool;
#else
#define SndBool_field_decl(x) SndBool x

#ifndef TRUE
#define TRUE 1
#define FALSE 0
#endif

typedef int SndBool;

#endif

#define SND_ENUM_ALIGNEMENT (long)0x13245678

#ifndef max
#define max(a,b)            (((a) > (b)) ? (a) : (b))
#endif

#ifndef min
#define min(a,b)            (((a) < (b)) ? (a) : (b))
#endif

#endif
