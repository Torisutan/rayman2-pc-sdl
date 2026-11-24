#ifndef SND_SNDDEF_H
#define SND_SNDDEF_H

/* Platform selection as in the original engine's snddef.h (docs/PLAN.md Phase 5, platform macros) */
#if defined(U64) || defined(NITRO) || defined(PSX) || defined(DREAMCAST) || defined(PSP) || defined(WIIU)
#define GAMECONSOLE
#endif /* U64 || NITRO || PSX || DREAMCAST || PSP || WIIU */

/* Win32 file API (CreateFile) unless a target or FILE_MODE_C_ANSI asks for the C runtime */
#if !defined(GAMECONSOLE) && !defined(USE_SDL) && !defined(FILE_MODE_C_ANSI)
#define FILE_MODE_C_WIN95
#endif /* !GAMECONSOLE && !USE_SDL && !FILE_MODE_C_ANSI */

#define NB_MICRO_MAX 2

#define SND_CALL WINAPI

typedef void (__stdcall *SND_td_pfn_vSoundCallback)(long par);

#define SndBool_field_decl(x) SndBool x

#endif
