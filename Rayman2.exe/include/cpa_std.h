/****************************************
 * cpa_std.h
 * Target header of the non-Windows targets: the MSVC keywords and the Windows
 * names the shared code spells, declared per target, as the original engine
 * structure declares them for its console targets through this header.
 *
 * The decomp's headers have no common root that every file includes, so the
 * build includes this header first in every file of a non-Windows target
 * (gcc/clang -include). Windows builds never include it.
 ****************************************/

#ifndef __CPA_STD_H__
#define __CPA_STD_H__

#if defined(LINUX)

/*----------------------------------------------------------------------------
 * Calling conventions: one convention per target, the keywords spell nothing
 * (as the original engine's console targets).
 *--------------------------------------------------------------------------*/
#ifndef __cdecl
#define __cdecl
#endif /*__cdecl*/

#ifndef __stdcall
#define __stdcall
#endif /*__stdcall*/

#ifndef __fastcall
#define __fastcall
#endif /*__fastcall*/

/* exports of the driver modules: the shared objects export every external name */
#ifndef __declspec
#define __declspec(x)
#endif /*__declspec*/

/* a macro, not the original's typedef: the shared code also writes "unsigned __int64" */
#ifndef __int64
#define __int64 long long
#endif /*__int64*/

/*----------------------------------------------------------------------------
 * Windows compatibility: the Windows names the original engine keeps on every
 * target, with the widths and layouts they have on 32-bit Windows (ILP32: the
 * level data and the driver interface see the same sizes). API structures
 * and handles of Win32-only calls are not declared here: their users get a
 * module branch instead.
 *--------------------------------------------------------------------------*/
#define FALSE 0
#define TRUE 1

typedef unsigned char   BYTE;
typedef char            CHAR;
typedef unsigned short  WORD;
typedef unsigned int    UINT;
typedef int             BOOL;
typedef long            LONG;
typedef unsigned long   DWORD;

typedef char *          LPSTR;
typedef const char *    LPCSTR;
typedef void *          LPVOID;

typedef unsigned int    WPARAM;
typedef long            LPARAM;
typedef long            LRESULT;

/* handles: pointer-sized, as on Windows */
typedef void *          HANDLE;
typedef void *          HWND;
typedef void *          HINSTANCE;

typedef struct tagRECT
{
    LONG left;
    LONG top;
    LONG right;
    LONG bottom;
} RECT;

typedef struct tagPOINT
{
    LONG x;
    LONG y;
} POINT;

#endif /* LINUX */

#endif /*__CPA_STD_H__*/
