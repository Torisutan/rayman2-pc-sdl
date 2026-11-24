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

#define SUPERVISATER

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

typedef struct _SND_tdstInitStruct
{
#if !defined(GAMECONSOLE) && !defined(GENERIC)
        HINSTANCE hProcessInstance;
        HWND hMainWindow;
        HANDLE hMutexDX;
#else
        int bidon;
#endif

} SND_tdstInitStruct;

#if !defined(GAMECONSOLE) || defined(WIN95) || defined(WIN32)

#define SND_CALL WINAPI
#else
#define SND_CALL
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
