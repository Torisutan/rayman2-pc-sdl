#include "GLI/cpu_timer.h"
#include "TMR/TMR_globals.h"
#include "GLI/timer_sub2.h"
#include "GLI/int64_arithmetic.h"

#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <string.h>
#include "ERM/CpaError.h"
#include "port/regress.h"

#ifndef USE_SDL   /* the SDL counterpart of the PC timer functions: TMR/sdl_timer.c */
__declspec(dllimport) DWORD __stdcall timeGetTime(void);

unsigned long __cdecl fn_ulTimerEventTickPerSecond(short channel)
{
    unsigned long ulFreq;
    switch (channel) {
    case 0:
        TMR_pc_fn_vSetupTimerPeriod();
        return 1000ul;
    case 1:
        ulFreq = 0;
        break;
    case 2:
        ulFreq = 0;
        break;
    default:
        return 0ul;
    }

    return ulFreq;
}

void __cdecl fn_vTimerGetCounter(short channel, LARGE_INTEGER *p_stOut)
{
#ifdef RAYMAN2_REGRESS
    if (REGRESS_fn_bGetCounter(channel, p_stOut))
        return;
#endif /* RAYMAN2_REGRESS */

    switch (channel) {
    case 0:
        p_stOut->u.LowPart  = (DWORD)timeGetTime();
        p_stOut->u.HighPart = 0;
        break;
    case 1:
        QueryPerformanceCounter(p_stOut);
        break;
    case 2:
        TMR_pc_fn_vReadTSC((unsigned __int64 *)p_stOut);
        break;
    }

}

unsigned long __cdecl TMR_pc_fn_bHasCPUID(void)
{
    unsigned long ulResult;

    __asm {
        pushfd
        pop  eax
        test eax, 0x200000
        jz   try_set

        and  eax, 0xffdfffff
        push eax
        popfd
        pushfd
        pop  eax
        test eax, 0x200000
        jz   supported
        jmp  not_supported
    try_set:
        or   eax, 0x200000
        push eax
        popfd
        pushfd
        pop  eax
        test eax, 0x200000
        jnz  supported
        jmp  not_supported
    supported:
        mov  eax, 1
        jmp  done
    not_supported:
        mov  eax, 0
    done:
        mov  ulResult, eax
    }

    return ulResult;
}

unsigned long __cdecl TMR_pc_fn_ulGetCPUSignature(void)
{
    unsigned long ulSig;

    __asm {
        mov   eax, 1
        _emit 0x0f
        _emit 0xa2
        mov   ulSig, eax
    }
    return ulSig;
}

unsigned long __cdecl TMR_pc_fn_ulReadCpuVendorString(void)
{

    __asm {
        mov   eax, 0
        _emit 0x0f
        _emit 0xa2
        mov   word ptr [TMR_pc_g_aucCpuVendor + 0], bx
        shr   ebx, 16
        mov   word ptr [TMR_pc_g_aucCpuVendor + 2], bx
        mov   word ptr [TMR_pc_g_aucCpuVendor + 4], dx
        shr   edx, 16
        mov   word ptr [TMR_pc_g_aucCpuVendor + 6], dx
        mov   word ptr [TMR_pc_g_aucCpuVendor + 8], cx
        shr   ecx, 16
        mov   word ptr [TMR_pc_g_aucCpuVendor + 10], cx
    }
}

int __cdecl TMR_pc_fn_bIsTscUsableForVendor(void)
{
    unsigned long ulFamily;

    if (TMR_pc_fn_bHasCPUID() == 0) {
        return 0;
    }
    TMR_pc_g_ulCpuSignature = TMR_pc_fn_ulGetCPUSignature();
    TMR_pc_fn_ulReadCpuVendorString();

    if (strcmp((const char *)TMR_pc_g_aucCpuVendor, "GenuineIntel") == 0) {
        ulFamily = (TMR_pc_g_ulCpuSignature >> 8) & 0xf;
        switch (ulFamily) {
        case 4:
            return 0;
        case 5:
        case 6:
            return 1;
        default:
            return 0;
        }
    }

    if (strcmp((const char *)TMR_pc_g_aucCpuVendor, "AuthenticAMD") == 0) {
        switch (TMR_pc_g_ulCpuSignature & 0xff0) {
        case 0x500: case 0x510:
            return 0;
        case 0x520: case 0x530: case 0x560:
        case 0x570: case 0x580: case 0x590:
            return 1;
        default:
            return 0;
        }
    }

    if (strcmp((const char *)TMR_pc_g_aucCpuVendor, "CyrixInstead") != 0) {
        return 0;
    }
    ulFamily = (TMR_pc_g_ulCpuSignature >> 8) & 0xf;
    switch (ulFamily) {
    case 4:
    case 5:
        return 0;
    case 6:
        return 1;
    default:
        return 0;
    }
}

unsigned long __cdecl fn_ulTimerTickPerSecond(short sChannel)
{
    int iCh = (int)sChannel;
    LARGE_INTEGER liFreq;
    BOOL bOk;

    switch (iCh) {
    case 2:
        if (TMR_pc_fn_bIsTscUsableForVendor() != 0) {
            return TMR_pc_fn_ulCalibrateTscFreq();
        }
        return 0ul;
    case 1:
        bOk = QueryPerformanceFrequency(&liFreq);
        return (bOk == 1) ? (unsigned long)liFreq.LowPart : 0ul;
    case 0:
        TMR_pc_fn_vSetupTimerPeriod();
        return 1000ul;
    default:
        return 0ul;
    }
}

#endif /* USE_SDL */

short __cdecl TMR_fn_wInitLibrary(void)
{

    if (*(unsigned short *)&g_bTMRInited != 0) {
        return 0;
    }

    memset(TMR_a_stTimerTab, 0, 576);

    *TMR_g_p_wTimerCount = 0;

    TMR_a3_ulTimerFrequency[0] = fn_ulTimerTickPerSecond(0);   /* 0x0045E8A0 */
    TMR_a3_ulTimerFrequency[1] = fn_ulTimerTickPerSecond(1);
    TMR_a3_ulTimerFrequency[2] = fn_ulTimerTickPerSecond(2);

    a3_ulEventFrequency[0] = fn_ulTimerEventTickPerSecond(0);
    a3_ulEventFrequency[1] = fn_ulTimerEventTickPerSecond(1);
    a3_ulEventFrequency[2] = fn_ulTimerEventTickPerSecond(2);

    if (TMR_pc_g_bCpuFeaturesInited == 0) {
        if (TMR_pc_g_ucExternalSvcId == 0xff) {
            TMR_pc_g_ucExternalSvcId = (unsigned char)Erm_fn_ucInitErrMsg();   /* 0x004025D0 */
        }
        TMR_pc_g_bCpuFeaturesInited = 1;
    }

    ++*(unsigned short *)&g_bTMRInited;
    return 0;
}
