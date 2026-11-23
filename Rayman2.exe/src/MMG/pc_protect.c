#include <string.h>
#include "MMG/MMG_globals.h"
#include "MMG/SNA_reloc.h"
#include "AI/AI_types.h"
#include "AI/AI_Struc.h"
#include "AI/DsgMem.h"
#include "GAM/GAM_types.h"
#include "GAM/GAM_globals.h"

unsigned char __cdecl fn_ucGetDsgVarInteger(long *p_lMemory, unsigned long ulIndex,
                                            AI_tdstGetSetParam *p_stValue);
unsigned char __cdecl fn_ucSetDsgVarInteger(long *pMyVarAddr, unsigned long ulIndex,
                                            AI_tdstGetSetParam *p_stValue);
unsigned char __cdecl fn_ucSetDsgVarArray(long *pMyVarAddr, unsigned long ulIndex,
                                          AI_tdstGetSetParam *p_stValue);

#ifndef RAYMAN2_NO_PROTECTION   /* private import table */
/* 0x004A13D0 */
MMG_pc_tdstImport MMG_pc_g_a_stImport[MMG_pc_C_NbImport + 1] = {

    { 0x522249B7UL, 16, "\xc5\xd7\x94\x8f\x3d\xe8\xc3\x90\x83\x4c\x27\xce\x94\x90\x33\x0b", 0, 0, { 0 } },

    { 0x4A1C1056UL, 21, "\xac\xa8\x5f\x47\x16\xad\xa0\x7f\x4a\x2a\xef\xe7\xa2\x7d\x51\x02\x09\xcd\x75\x50\x43", 0, 0, { 0 } },

    { 0x23FA8301UL, 17, "\xc5\xbe\xb2\x9e\x63\x23\x04\xf4\xc6\x95\x76\x35\x01\xdc\xb5\x8a\x61", 0, 0, { 4, 6 } },

    { 0x5D2545ECUL, 25, "\x00\xe8\xd0\x89\x40\xeb\xe0\x91\x5f\x1e\xf3\xac\x68\x5d\x2a\xf4\x8f\x61\x30\x08\xcc\x78\x47\x02\xe8", 0, 0, { 0 } },

    { 0x2E673CE2UL, 10, "\x55\x63\xad\xbf\x2f\x39\x81\xc3\xf2\x1c", 0, 0, { 4 } },

    { 0x6B5FF7FBUL, 12, "\xaf\x6a\x61\x71\x4e\x41\x71\x26\x1d\x26\x02\xe1", 0, 0, { 6 } },

    { 0x716B14DFUL, 10, "\x25\x2a\x3a\x22\x37\x02\x2d\x1a\x29\x1b", 0, 0, { 6 } },

    { 0x7E23565BUL, 8, "\x60\x2f\xc7\x59\x1f\xb3\x51\x0c", 0, 0, { 6, 4 } },

    { 0x5A151926UL, 11, "\x80\x3c\xf8\xab\x6f\x4d\x17\x9f\x53\x09\xd4", 0, 0, { 4, 6 } },

    { 0x3124E9C7UL, 9, "\x85\x53\x31\x3b\x1f\x13\x05\xef\xfb", 0, 0, { 0 } },

    { 0x5862A308UL, 8, "\xf9\x08\xfe\xfd\xd5\x24\x33\x24", 0, 0, { 4, 3 } },

    { 0x36840F8BUL, 11, "\xd7\x38\xa7\xd9\x1d\x74\xd5\x0e\x86\xc8\x01", 0, 0, { 4, 6 } },

    { 0x16A0AA98UL, 14, "\x91\xdd\x7a\x22\x83\x2c\xa3\x0c\xcd\x51\xc0\x60\xef\x72", 0, 0, { 6 } },

    { 0x136B44DDUL, 15, "\xdf\x83\xd2\x2c\x81\xd9\x3e\x6f\xee\x4d\x82\xe9\x2f\xaa\xf3", 0, 0, { 0 } },

    { 0x03D350CAUL, 23, "\xcb\xc3\xa3\x6e\x3b\xf8\xc3\xa6\x70\x39\x24\xf7\xc3\x9a\x6d\x3f\xbd\xb9\x93\x5a\x3e\x03\xcc", 0, 0, { 0 } },

    { 0x20176A95UL, 13, "\xc2\x9b\xa1\x68\x89\x6b\x7b\x65\x8b\x57\x55\x41\x54", 0, 0, { 0 } },

    { 0x6786BF52UL, 21, "\x4a\x1b\x57\x86\x9e\x9e\xd2\xf9\x14\x1f\x39\x5e\x96\xa8\xb2\xd5\x0d\x37\x50\x4e\x54", 0, 0, { 0 } },

    { 0x6F8A89A6UL, 23, "\x74\xcd\xcd\xde\xf8\x2b\x2c\x5d\x56\x9c\xa5\xd0\xf2\x02\x30\x15\x53\x6a\x98\x8c\xc0\xcf\xd4", 0, 0, { 0 } },

    { 0x3C064F6FUL, 11, "\x77\x63\x2e\xc6\xdd\xa4\x65\x39\xe5\xb6\x6d", 0, 0, { 4, 3, 6 } },

    { 0x7D36345FUL, 24, "\xd6\x58\x16\xcb\x93\x2d\x19\xc9\x82\x3a\xf9\xa7\x63\x1a\x8c\x3e\x16\xe4\xca\x6c\x22\xea\xa0\x54", 0, 0, { 0 } },

    { 0x47026508UL, 14, "\x3d\xe8\xa8\x67\xf4\xcd\x90\x7b\xc7\xbc\x5e\x1e\xea\x96", 0, 0, { 0 } },

    { 0x43510DB5UL, 13, "\x12\x28\x53\x5d\x75\x79\x87\xbd\x54\xb4\xad\xc6\xc6", 0, 0, { 0 } },

    { 0x23099E7DUL, 13, "\x50\x34\x0f\x0a\xe7\xd2\xf6\x5d\xb5\x7e\x65\x4c\x1f", 0, 0, { 0 } },

    { 0x15CA36B0UL, 12, "\x2b\x9e\x48\x0e\xbe\x9c\xf3\x31\x90\x60\x04\xc7", 0, 0, { 0 } },

    { 0x2B07E712UL, 13, "\xb4\x95\x68\x2c\xf1\x01\xc2\xba\xa4\x6a\x3e\x04\xfe", 0, 2, { 2, 2, 2, 2, 2 } },

    { 0x5BF861FAUL, 18, "\x1e\xc4\x4c\x01\x77\x2c\xb6\x7f\x16\x99\x46\xa9\x64\xf6\xb8\x5c\xee\x91", 0, 2, { 2, 2, 2, 2, 2 } },

    { 0x00BE2656UL, 12, "\xc0\x5d\x19\xd2\xb8\x5b\x0e\xdc\x8d\x58\x21\xe7", 0, 0, { 1, 6, 3, 4 } },

    { 0x7770088EUL, 11, "\xdc\x0b\x01\xe6\xf6\xf0\xde\xd9\xaf\xc3\xc5", 0, 0, { 4, 3, 1, 6, 5 } },

    { 0x0FF5A945UL, 17, "\x33\x17\xe2\xb6\xa5\x95\x6b\x4c\x40\x37\x31\xe9\xee\xc3\xc3\x8f\x6d", 0, 0, { 0 } },

    { 0x7453F4BAUL, 18, "\x61\x1a\x08\xc9\xb3\xad\x75\x60\x32\x21\x06\xe4\xa4\x9e\x7c\x3c\x31\x26", 0, 0, { 0 } },

    { 0x1DEB1B12UL, 18, "\x6e\x5a\x09\x07\xa4\x69\x40\x26\xe1\xa9\x8f\x5e\x2b\xe2\xb1\x82\x5a\xf6", 0, 0, { 6 } },

    { 0x0E6AA5E6UL, 26, "\xc8\x26\x6f\xad\x42\x7f\xfe\x1a\xb1\xe5\x68\xc1\xfe\x82\xcf\x3a\x70\xed\x3d\x9a\x23\x42\xb9\xef\x6f\xa6", 0, 0, { 0 } },

    { 0x5E8794BDUL, 12, "\x84\xdb\xef\x3d\x57\x78\x8e\xc2\xbe\x09\x27\x85", 0, 0, { 6 } },

    { 0x287F1AA2UL, 11, "\xeb\x48\xbe\xf3\x71\xab\x0b\x81\xcd\x08\x67", 0, 0, { 0 } },

    { 0x210E9F7BUL, 11, "\x53\x2e\x2c\xe9\xfd\xe9\xcf\xaf\xb9\x9e\x93", 0, 0, { 0 } },

    { 0x4CD2296EUL, 16, "\xdc\x50\xc7\x25\xec\x4a\xf2\x8a\xe6\x89\x07\x93\x16\x8b\x1e\xaa", 0, 0, { 0 } },

    { 0 }
};

/* 0x004A1268 */
char MMG_pc_g_a_szModule[MMG_pc_C_NbModule + 1][sizeof(MMG_pc_tdstModule)] = {
    "KERNEL32.DLL", "USER32.DLL", "PSAPI.DLL", "ADVAPI32.DLL", "GDI32.DLL", "VERSION.DLL", "WINMM.DLL", ""
};
#endif /* RAYMAN2_NO_PROTECTION */

#ifndef RAYMAN2_NO_PROTECTION   /* level-load tamper block: DsgVar table install, code block execution */
void *MMG_pc_g_pvTamperResumeA;   /* 0x00510E14 */
void *MMG_pc_g_pvTamperExitA;     /* 0x00510C54 */
void *MMG_pc_g_pvTamperResumeB;   /* 0x00510E10 */
void *MMG_pc_g_pvTamperExitB;     /* 0x00510C50 */
void *MMG_pc_g_pvTamperResumeC;   /* 0x00510E0C */
void *MMG_pc_g_pvTamperExitC;     /* 0x00510C4C */

#pragma optimize("y", off)

void __cdecl MMG_pc_fn_vInstallTableDsgVars(AI_tdstMind *p_stMind)
{
    AI_tdstGetSetParam stVal;
    unsigned long ulSlot;
    unsigned long ulIdx;
    long *p_lDst1;
    long *p_lDst2;

    if (SNA_g_lProtectionGate != 0) {
        __asm { mov dword ptr [MMG_pc_g_pvTamperResumeA], offset L_pc_body }
        __asm { mov dword ptr [MMG_pc_g_pvTamperExitA], offset L_pc_epi }
        return;
    }
L_pc_body:
    if (p_stMind != NULL) {
        if (fn_ucGetDsgVarInteger((long *)M_GetDsgVarAddrDefault(p_stMind, 0), 0, &stVal) == 1
            && stVal.uParam.lValue == 0) {
            *(unsigned char *)&ulSlot = 0;
            p_lDst1 = (long *)M_GetDsgVarAddrDefault(p_stMind, 1);
            p_lDst2 = (long *)M_GetDsgVarAddrDefault(p_stMind, 3);
            if (SNA_g_ulTableIndex > 0) {
                ulIdx = 0;
                do {
                    stVal.uParam.lValue = (long)SNA_pc_g_a_ulTableKey[ulIdx];
                    stVal.eType = 6;
                    fn_ucSetDsgVarArray(p_lDst1, ulSlot, &stVal);
                    stVal.uParam.lValue = (long)SNA_pc_g_a_ulTableState[ulIdx];
                    stVal.eType = 6;
                    fn_ucSetDsgVarArray(p_lDst2, ulSlot, &stVal);
                    *(unsigned char *)&ulSlot += 1;
                    ulIdx = ulSlot & 0xffu;
                } while (ulIdx < SNA_g_ulTableIndex);
            }
            stVal.uParam.lValue = (long)SNA_g_ulTableIndex;
            stVal.eType = 6;
            fn_ucSetDsgVarInteger((long *)M_GetDsgVarAddrDefault(p_stMind, 2), 0, &stVal);
        }
    }
L_pc_epi:;
}

#pragma optimize("", on)

#define MMG_pc_C_lTamperIntelligenceShift  0x60
#define MMG_pc_C_lTamperDsgMemShift        0x78

void __cdecl MMG_pc_fn_vTamperRandomEntry(void)
{
    HIE_tdstSuperObject *p_stRoot;
    HIE_tdstSuperObject *p_stHead;
    HIE_tdstSuperObject *p_stNode;
    struct tdstEngineObject_ *p_stObj;
    MS_tdxHandleToStandardGame hStdGame;
    MS_tdxHandleToBrain hBrain;
    AI_tdstMind *p_stMind;
    int iCount;
    int iRand;
    int iLeft;
    unsigned int ulS;
    unsigned int ulNext;

    if (SNA_g_lProtectionGate != 0) {
        __asm { mov dword ptr [MMG_pc_g_pvTamperResumeB], offset L_pc_body2 }
        __asm { mov dword ptr [MMG_pc_g_pvTamperExitB], offset L_pc_ret2 }
        return;
    }
L_pc_body2:
    p_stRoot = gp_stDynamicWorld;
    iCount = p_stRoot->lNumberOfChildren;
    if (iCount != 0) {
        ulS = MMG_g_ulLCGState ^ 0x75bd924u;
        ulNext = ulS * 0x41a7u + (unsigned int)(((int)ulS / 0x1f31d) * -0x7fffffff);
        MMG_g_ulLCGState = ulNext;
        if ((int)MMG_g_ulLCGState < 0) {
            MMG_g_ulLCGState = MMG_g_ulLCGState + 0x7fffffffu;
        }
        iRand = (int)MMG_g_ulLCGState % iCount;
    } else {
        iRand = 0;
    }
    p_stHead = p_stRoot->p_stFirstChild;
    iLeft = iRand;
    for (;;) {
        p_stNode = p_stHead;
        while (p_stNode != 0) {
            p_stObj = (struct tdstEngineObject_ *)p_stNode->hLinkedObject.p_Void;
            hStdGame = p_stObj->h_StandardGame;
            if (hStdGame != 0 && (hStdGame->ulCustomBits & GAM_C_CustBitNoAI) == 0) {
                hBrain = (MS_tdxHandleToBrain)p_stObj->h_Brain;
                if (hBrain != 0) {
                    p_stMind = hBrain->p_stMind;
                    if (p_stMind != 0) {
                        if (iLeft == 0) {
                            p_stMind->p_stIntelligence = (struct AI_tdstIntelligence_ *)
                                ((char *)p_stMind->p_stIntelligence + MMG_pc_C_lTamperIntelligenceShift);
                            p_stMind->p_stDsgMem = (struct AI_tdstDsgMem_ *)
                                ((char *)p_stMind->p_stDsgMem + MMG_pc_C_lTamperDsgMemShift);
L_pc_ret2:
                            return;
                        }
                        iLeft = iLeft - 1;
                    }
                }
            }
            p_stNode = p_stNode->p_stNextBrother;
        }
    }
}

unsigned long __cdecl MMG_pc_fn_ulCountExporters(MMG_pc_tdstImport *p_stImport);
unsigned long __cdecl MMG_pc_fn_ulGetExeIatDelta(MMG_pc_tdstImport *p_stImport);

#define M_ulApi(i)  (MMG_pc_g_a_stImport[i].ulPtrXorKey ^ MMG_pc_g_a_stImport[i].ulKey)

void __cdecl MMG_pc_fn_vFillTamperApiTable(unsigned long *_p_ulTable)
{
    _p_ulTable[6]  = (unsigned long)strchr;
    _p_ulTable[13] = M_ulApi(32);
    _p_ulTable[2]  = M_ulApi(8);
    _p_ulTable[7]  = M_ulApi(31);
    _p_ulTable[0]  = M_ulApi(11);
    _p_ulTable[4]  = M_ulApi(15);
    _p_ulTable[10] = M_ulApi(16);
    _p_ulTable[3]  = M_ulApi(28);
    _p_ulTable[14] = M_ulApi(29);
    _p_ulTable[5]  = M_ulApi(30);
    _p_ulTable[15] = M_ulApi(33);
    _p_ulTable[9]  = M_ulApi(34);
    _p_ulTable[8]  = M_ulApi(18);
    _p_ulTable[11] = (unsigned long)MMG_pc_fn_ulCountExporters;
    _p_ulTable[12] = (unsigned long)MMG_pc_fn_ulGetExeIatDelta;
    _p_ulTable[1]  = (unsigned long)MMG_pc_fn_vTamperRandomEntry;
}

#undef M_ulApi

typedef unsigned long (__cdecl *MMG_pc_tdpfnTamperBlock)(unsigned char *p_ucDst, unsigned long *p_ulTable);

unsigned int __cdecl MMG_pc_fn_ulStepLcgState(void);

#pragma optimize("", off)

void __cdecl MMG_pc_fn_vRunTamperCodeBlock(AI_tdstMind *volatile p_stMind)
{

    AI_tdstGetSetParam a;
    volatile unsigned long b;
    volatile long c;
    volatile unsigned long d;
    volatile unsigned long e;
    volatile unsigned long f;
    volatile unsigned long g;
    unsigned long h[16];
    volatile unsigned long i;
    volatile unsigned long j;
    volatile long k;
    volatile unsigned long l;
    volatile unsigned long m;
    volatile unsigned long n;
    volatile unsigned long o;

    if (SNA_g_lProtectionGate != 0) {
        __asm { mov dword ptr [MMG_pc_g_pvTamperResumeC], offset L_pc_body3 }
        __asm { mov dword ptr [MMG_pc_g_pvTamperExitC], offset L_pc_epi3 }
        goto L_pc_epi3;
    }
L_pc_body3:
    if (p_stMind != NULL) {
        unsigned long x, y;
        /* 0x004673A0 */
        b = (unsigned long)M_GetDsgVarAddrInit(p_stMind, 0);
        if (b != 0
            && fn_ucGetDsgVarInteger((long *)b, 0, &a) == 1
            && (c = a.uParam.lValue, c == 2)) {
            m = (unsigned long)((tdstArray1 *)M_GetDsgVarAddrInit(p_stMind, 1))->d_ArrayElement;
            l = (unsigned long)((tdstArray1 *)M_GetDsgVarAddrInit(p_stMind, 3))->d_ArrayElement;
            g = (unsigned long)M_GetDsgVarAddrInit(p_stMind, 2);
            f = (unsigned long)M_GetDsgVarAddrInit(p_stMind, 4);
            j = (unsigned long)M_GetDsgVarAddrInit(p_stMind, 5);

            e = m;
            i = 0x00ec8b55UL;
            e = m;
            d = *(unsigned long *)e ^ (*(unsigned long *)e | i);
            while (e < g) {
                n = *(unsigned long *)e;
                *(unsigned long *)e = *(unsigned long *)e ^ d;
                d = n;
                e += 4;
            }

            MMG_pc_fn_vFillTamperApiTable(h);

            e = l;
            while (e < f) {
                o = *(unsigned long *)e;
                *(unsigned long *)e = *(unsigned long *)e ^ d;
                d = o;
                e += 4;
            }

            fn_ucGetDsgVarInteger((long *)j, 0, &a);
            if (a.uParam.lValue != 0) {
                *(unsigned char *)l = (unsigned char)g_cCDROM;
            }

            k = (long)((MMG_pc_tdpfnTamperBlock)m)((unsigned char *)l, h);

            while (m < g) {
                *(unsigned char *)m = (unsigned char)MMG_pc_fn_ulStepLcgState();
                m += 1;
            }

            a.uParam.lValue = k;
            a.eType = 6;

            while (l < f) {
                *(unsigned char *)l = (unsigned char)MMG_pc_fn_ulStepLcgState();
                l += 1;
            }

            fn_ucSetDsgVarInteger((long *)f, 0, &a);
        }
    }
L_pc_epi3:;
}

#pragma optimize("", on)
#endif /* RAYMAN2_NO_PROTECTION */
