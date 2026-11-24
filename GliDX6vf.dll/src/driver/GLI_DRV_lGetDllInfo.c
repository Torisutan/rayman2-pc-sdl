#include "GliDX6vf/GliDX6vf.h"

#include <string.h>

/* 0x10006780 */
BOOL WINAPI DllMain(HINSTANCE hInstance, DWORD dwReason, LPVOID pReserved)
{
    GLI_vInitDllInfo();
    return TRUE;
}

/* 0x10006790 */
__declspec(dllexport) long __cdecl GLI_DRV_lGetDllInfo(char const *szName, void *pData)
{
    char *szSource;

    if (strcmp(szName, "Name") == 0)
        szSource = GLI_g_stDllInfo.szName;
    else if (strcmp(szName, "Description") == 0)
        szSource = GLI_g_stDllInfo.szDescription;
    else if (strcmp(szName, "Version") == 0)
        szSource = GLI_g_stDllInfo.szVersion;
    else if (strcmp(szName, "Code") == 0) {
        *(long *)pData = GLI_g_stDllInfo.lCode;
        return 1;
    } else
        return 0;
    strcpy((char *)pData, szSource);
    return 1;
}
