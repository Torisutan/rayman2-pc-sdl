/****************************************
 * u_types.h
 * Windows compatibility of the targets without the Win32 API: the Windows
 * names the engine keeps on every target, one branch per target. Reached
 * through cpa_std.h.
 ****************************************/

#ifndef __U_TYPES_H__
#define __U_TYPES_H__

#if defined(LINUX)

/*----------------------------------------------------------------------------
 * The widths and layouts of 32-bit Windows (ILP32: the level data and the
 * driver interface see the same sizes). API structures and handles of
 * Win32-only calls are not declared here: their users get a module branch.
 *--------------------------------------------------------------------------*/
#define FALSE 0
#define TRUE 1

/* a macro, not a typedef: the shared code also writes "unsigned __int64" */
#ifndef __int64
#define __int64 long long
#endif /*__int64*/

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

#endif /*__U_TYPES_H__*/
