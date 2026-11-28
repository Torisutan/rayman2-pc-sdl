/****************************************
 * TIMER
 * u_define.h
 * Calling conventions of the targets without MSVC: one convention per
 * target, the keywords spell nothing. Reached through cpa_std.h.
 ****************************************/

#ifndef __U_DEFINE_H__
#define __U_DEFINE_H__

#if defined(LINUX)

#ifndef __stdcall
#define __stdcall
#endif /*__stdcall*/

#ifndef __cdecl
#define __cdecl
#endif /*__cdecl*/

#ifndef __fastcall
#define __fastcall
#endif /*__fastcall*/

/* exports of the driver modules: the shared objects export every external name */
#ifndef __declspec
#define __declspec(x)
#endif /*__declspec*/

#endif /* LINUX */

#endif /*__U_DEFINE_H__*/
