#ifndef __FIL_FIL_pc_h__
#define __FIL_FIL_pc_h__

#define FIL_pc_M_ulGetProfileString(szSection, szKey, szDefault, szOut, ulSize, szIniPath) \
    GetPrivateProfileStringA((szSection), (szKey), (szDefault), (szOut), (ulSize), (szIniPath))
#define FIL_pc_M_bWriteProfileString(szSection, szKey, szValue, szIniPath) \
    WritePrivateProfileStringA((szSection), (szKey), (szValue), (szIniPath))

#endif
