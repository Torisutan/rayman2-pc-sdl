#ifndef __FIL_FIL_pc_h__
#define __FIL_FIL_pc_h__

#include "SND/snddef.h"
#ifndef FILE_MODE_C_WIN95
/* Ini files on the C runtime (FIL/FIL_ini.c), same results as the Win32 profile string API */
unsigned long __cdecl FIL_fn_ulGetProfileString(const char *szSection, const char *szKey,
                                                    const char *szDefault, char *szReturn,
                                                    unsigned long ulSize, const char *szFileName);
int __cdecl FIL_fn_bWriteProfileString(const char *szSection, const char *szKey,
                                           const char *szValue, const char *szFileName);
#define FIL_pc_M_ulGetProfileString(szSection, szKey, szDefault, szOut, ulSize, szIniPath) \
    FIL_fn_ulGetProfileString((szSection), (szKey), (szDefault), (szOut), (ulSize), (szIniPath))
#define FIL_pc_M_bWriteProfileString(szSection, szKey, szValue, szIniPath) \
    FIL_fn_bWriteProfileString((szSection), (szKey), (szValue), (szIniPath))
#else /* original Win32 */
#define FIL_pc_M_ulGetProfileString(szSection, szKey, szDefault, szOut, ulSize, szIniPath) \
    GetPrivateProfileStringA((szSection), (szKey), (szDefault), (szOut), (ulSize), (szIniPath))
#define FIL_pc_M_bWriteProfileString(szSection, szKey, szValue, szIniPath) \
    WritePrivateProfileStringA((szSection), (szKey), (szValue), (szIniPath))
#endif /* FILE_MODE_C_WIN95 */

#ifndef FILE_MODE_C_WIN95
/* The temp folder on the C runtime (FIL/FIL_path.c) */
unsigned long __cdecl FIL_fn_ulGetTempPath(unsigned long ulSize, char *szOut);
#define FIL_pc_M_ulGetTempPath(ulSize, szOut) FIL_fn_ulGetTempPath((ulSize), (szOut))
#else /* original Win32 */
#define FIL_pc_M_ulGetTempPath(ulSize, szOut) GetTempPathA((ulSize), (szOut))
#endif /* FILE_MODE_C_WIN95 */

#endif
