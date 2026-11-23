#include <windows.h>
#include <stdlib.h>
#include <string.h>
#include <tlhelp32.h>

#include "MMG/MMG_globals.h"

#include "GAM/GAM_globals.h"
#include "port/regress.h"

#define g_ulLCGState   MMG_g_ulLCGState

MMG_pc_tduLcgPool MMG_pc_g_uLcgPool;   /* 0x00510EA0 */

#define g_ucMMGChannelMax  SNA_g_lProtectionGate

/* 0x00510BC0 */
char MMG_pc_g_a_cModuleName[0x30];

void *MMG_pc_g_pvFindIatResume;          /* 0x00510DF4 */
void *MMG_pc_g_pvFindIatExit;            /* 0x00510C34 */
void *MMG_pc_g_pvFindExportResume;       /* 0x00510DF8 */
void *MMG_pc_g_pvFindExportExit;         /* 0x00510C38 */
void *MMG_pc_g_pvLoadModuleResume;       /* 0x00510DFC */
void *MMG_pc_g_pvLoadModuleExit;         /* 0x00510C3C */
void *MMG_pc_g_pvResolveImportResume;    /* 0x00510E00 */
void *MMG_pc_g_pvResolveImportExit;      /* 0x00510C40 */
void *MMG_pc_g_pvIsKernel32Resume;       /* 0x00510E04 */
void *MMG_pc_g_pvIsKernel32Exit;         /* 0x00510C44 */

/* 0x004474B0 */
MMG_pc_tdstImport *__cdecl MMG_pc_fn_p_stGetImport(unsigned long ulIndex)
{
    return &MMG_pc_g_a_stImport[ulIndex];
}

/* 0x004474D0 */
char *__cdecl MMG_pc_fn_szDecryptModuleName(unsigned long ulIndex)
{
    unsigned long ulKey;
    unsigned char ucLen;
    unsigned char *p_ucKey;
    unsigned char ucSub;
    unsigned char ucXor;
    char *p_cSrc;
    char *p_cDst;
    unsigned char ucI;

    ulKey = MMG_pc_M_p_stModule(ulIndex)->ulKey;
    p_cSrc = MMG_pc_M_p_stModule(ulIndex)->a_cName;
    ucLen = MMG_pc_M_p_stModule(ulIndex)->ucNameLen;
    p_cDst = MMG_pc_g_a_cModuleName;
    ucSub = ((unsigned char *)&ulKey)[0];
    ucXor = ((unsigned char *)&ulKey)[1];
    for (ucI = 0; ucI < ucLen; ucI++) {
        *p_cDst++ = (char)((unsigned char)(*p_cSrc++ - ucSub) ^ ucXor);
        ucSub += ((unsigned char *)&ulKey)[2];
        ucXor += ((unsigned char *)&ulKey)[3];
    }
    *p_cDst = '\0';
    return MMG_pc_g_a_cModuleName;
}

static __inline long MMG_pc_fn_lCompareImportName(const unsigned char *p_ucName, unsigned char ucLen,
                                                  const unsigned char *p_ucEnc, unsigned long ulKey)
{
    unsigned char *p_ucKey;
    unsigned char ucI;
    unsigned char ucPlain;
    long lDiff;

    lDiff = 0;
    ucI = 0;
    p_ucKey = (unsigned char *)&ulKey;
    for (; ucI < ucLen; ucI++) {
        ucPlain = (unsigned char)(*p_ucEnc++ - p_ucKey[0]) ^ p_ucKey[1];
        p_ucKey[0] += p_ucKey[2];
        p_ucKey[1] += p_ucKey[3];
        lDiff = *p_ucName++ - ucPlain;
        if (lDiff != 0)
            break;
    }
    if (lDiff < 0)
        lDiff = -1;
    else if (lDiff > 0)
        lDiff = 1;
    return lDiff;
}

static __inline unsigned long *MMG_pc_fn_p_ulFindIatSlot(unsigned long hModule,
                                                        MMG_pc_tdstImport *p_stImport)
{
    unsigned long ulRva;
    IMAGE_IMPORT_DESCRIPTOR *p_stDesc;
    unsigned long *p_ulThunk;
    unsigned long *p_ulIat;

    ulRva = ((IMAGE_NT_HEADERS *)(hModule + ((IMAGE_DOS_HEADER *)hModule)->e_lfanew))
                ->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_IMPORT].VirtualAddress;
    if (ulRva == 0)
        return NULL;
    p_stDesc = (IMAGE_IMPORT_DESCRIPTOR *)(hModule + ulRva);
    if (hModule == 0 || p_stDesc == NULL)
        return NULL;

    for (; p_stDesc->Characteristics != 0; p_stDesc++) {
        p_ulThunk = (unsigned long *)(hModule + p_stDesc->Characteristics);
        p_ulIat = (unsigned long *)(hModule + (unsigned long)p_stDesc->FirstThunk);
        for (; *p_ulThunk != 0; p_ulThunk++, p_ulIat++) {
            if ((*p_ulThunk & IMAGE_ORDINAL_FLAG) == 0
                && MMG_pc_fn_lCompareImportName((unsigned char *)(hModule + *p_ulThunk + 2),
                                                p_stImport->ucNameLen,
                                                (unsigned char *)p_stImport->a_cName, p_stImport->ulKey) == 0)
                return p_ulIat;
        }
    }
    return NULL;
}

#define MMG_pc_M_ulExportRva(hModule)                                                      \
    (((IMAGE_NT_HEADERS *)((hModule) + ((IMAGE_DOS_HEADER *)(hModule))->e_lfanew))         \
         ->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_EXPORT].VirtualAddress)

static __inline unsigned long MMG_pc_fn_ulFindExportRva(unsigned long hModule,
                                                       IMAGE_EXPORT_DIRECTORY *p_stExport,
                                                       MMG_pc_tdstImport *p_stImport)
{
    unsigned long *p_ulNames;
    unsigned long *p_ulFunctions;
    unsigned short *p_uwOrdinals;
    long lI;
    long lNbNames;
    unsigned long ulFunction;

    if (hModule == 0 || p_stExport == NULL)
        return 0;

    p_ulNames = (unsigned long *)(hModule + (unsigned long)p_stExport->AddressOfNames);
    p_ulFunctions = (unsigned long *)(hModule + (unsigned long)p_stExport->AddressOfFunctions);
    p_uwOrdinals = (unsigned short *)(hModule + (unsigned long)p_stExport->AddressOfNameOrdinals);
    lNbNames = p_stExport->NumberOfNames;
    for (lI = 0; lI < lNbNames; lI++, p_ulNames++, p_uwOrdinals++) {
        ulFunction = p_ulFunctions[*p_uwOrdinals];
        if (MMG_pc_fn_lCompareImportName((unsigned char *)(hModule + *p_ulNames), p_stImport->ucNameLen,
                                         (unsigned char *)p_stImport->a_cName, p_stImport->ulKey) == 0)
            return ulFunction;
    }
    return 0;
}

#pragma optimize("y", off)

/* 0x00447540 */
unsigned long __cdecl MMG_pc_fn_ulFindImportInIat(unsigned long ulIndex)
{
    long lLast;
    long lRand;
    unsigned long hModule;
    unsigned long *p_ulSlot;
    unsigned long ulResult;

    if (SNA_g_lProtectionGate != 0) {
        __asm { mov dword ptr [MMG_pc_g_pvFindIatResume], offset L_body }
        __asm { mov dword ptr [MMG_pc_g_pvFindIatExit], offset L_epi }
        return 0;
    }
L_body:
    for (lLast = 4; MMG_pc_g_a_stImport[ulIndex].a_ulImporter[lLast] == MMG_pc_g_a_stImport[ulIndex].ulModule;
         lLast--)
        ;
    if (lLast + 1 != 0) {
        MMG_pc_M_vStepLcg();
        lRand = (int)MMG_g_ulLCGState % (lLast + 1);
    } else {
        lRand = 0;
    }
    hModule = g_p_ulMMGTable[MMG_pc_g_a_stImport[ulIndex].a_ulImporter[lRand]];
    if (hModule == 0)
        p_ulSlot = NULL;
    else
        p_ulSlot = MMG_pc_fn_p_ulFindIatSlot(hModule, MMG_pc_fn_p_stGetImport(ulIndex));
    ulResult = 0;
    if (p_ulSlot != NULL)
        ulResult = *p_ulSlot;
L_epi:
    return ulResult;
}

/* 0x00447730 */
unsigned long __cdecl MMG_pc_fn_ulFindImportInExports(unsigned long ulIndex)
{
    unsigned long hModule;
    unsigned long ulResult;
    unsigned long ulRva;
    MMG_pc_tdstImport *p_stImport;

    if (SNA_g_lProtectionGate != 0) {
        __asm { mov dword ptr [MMG_pc_g_pvFindExportResume], offset L_body }
        __asm { mov dword ptr [MMG_pc_g_pvFindExportExit], offset L_epi }
        return 0;
    }
L_body:
    ulResult = 0;
    hModule = g_p_ulMMGTable[MMG_pc_g_a_stImport[ulIndex].ulModule];
    if (hModule != 0) {
        p_stImport = MMG_pc_fn_p_stGetImport(ulIndex);
        ulRva = MMG_pc_M_ulExportRva(hModule);
        if (ulRva != 0)
            ulResult = MMG_pc_fn_ulFindExportRva(hModule, (IMAGE_EXPORT_DIRECTORY *)(hModule + ulRva),
                                                 p_stImport);
    }
L_epi:
    return ulResult;
}

/* 0x00447A90 */
void __cdecl MMG_pc_fn_vLoadModuleHandle(unsigned long ulModule, long bInit)
{
    unsigned long hModule;
    char *p_szName;
    unsigned long (__stdcall *pfn_hGetModuleHandle)(const char *);

    if (SNA_g_lProtectionGate != 0) {
        __asm { mov dword ptr [MMG_pc_g_pvLoadModuleResume], offset L_body }
        __asm { mov dword ptr [MMG_pc_g_pvLoadModuleExit], offset L_epi }
        return;
    }
L_body:
    p_szName = MMG_pc_fn_szDecryptModuleName(ulModule);
    pfn_hGetModuleHandle = (unsigned long (__stdcall *)(const char *))(
        MMG_pc_g_a_stImport[MMG_pc_C_ImpGetModuleHandleA].ulKey
        ^ MMG_pc_g_a_stImport[MMG_pc_C_ImpGetModuleHandleA].ulPtrXorKey);
    hModule = pfn_hGetModuleHandle(p_szName);
    if (!bInit && hModule != g_p_ulMMGTable[ulModule])
        hModule = 0;
    g_p_ulMMGTable[ulModule] = hModule;
L_epi:;
}

/* 0x00447AF0 */
void __cdecl MMG_pc_fn_vResolveImport(unsigned long ulIndex, long bInit)
{
    unsigned long ulKey;
    unsigned long hModule;
    unsigned long ulAddress;

    if (SNA_g_lProtectionGate != 0) {
        __asm { mov dword ptr [MMG_pc_g_pvResolveImportResume], offset L_body }
        __asm { mov dword ptr [MMG_pc_g_pvResolveImportExit], offset L_epi }
        return;
    }
L_body:
    hModule = g_p_ulMMGTable[MMG_pc_g_a_stImport[ulIndex].ulModule];
    ulKey = MMG_pc_g_a_stImport[ulIndex].ulKey;
    if (hModule == 0) {
        MMG_pc_g_a_stImport[ulIndex].ulPtrXorKey = hModule;
        return;
    }
    if (bInit) {
        ulAddress = MMG_pc_fn_ulFindImportInExports(ulIndex);
        ulAddress += ulAddress ? hModule : 0;
    } else {
        if (MMG_pc_g_a_stImport[ulIndex].ulModule != MMG_pc_g_a_stImport[ulIndex].a_ulImporter[0]
            && g_p_ulMMGTable[MMG_pc_g_a_stImport[ulIndex].a_ulImporter[0]] != 0) {
            ulAddress = MMG_pc_fn_ulFindImportInIat(ulIndex);
        } else {
            ulAddress = MMG_pc_fn_ulFindImportInExports(ulIndex);
            ulAddress += ulAddress ? hModule : 0;
        }
        if (MMG_pc_g_ulPsapiLoaded && ulAddress == 0)
            ulAddress = MMG_pc_g_a_stImport[ulIndex].ulPtrXorKey ^ ulKey;
        if (ulAddress != (MMG_pc_g_a_stImport[ulIndex].ulPtrXorKey ^ ulKey))
            ulAddress = 0;
    }
    if (ulAddress != 0)
        ulKey ^= ulAddress;
    else
        ulKey = 0;
    MMG_pc_g_a_stImport[ulIndex].ulPtrXorKey = ulKey;
L_epi:;
}

/* 0x00447BE0 */
long __cdecl MMG_pc_fn_bIsKernel32Image(unsigned char *p_ucImage)
{
    IMAGE_NT_HEADERS *p_stNt;
    IMAGE_OPTIONAL_HEADER *p_stOptional;
    IMAGE_EXPORT_DIRECTORY *p_stExport;
    unsigned long *p_ulName;
    long bResult;

    if (SNA_g_lProtectionGate != 0) {
        __asm mov dword ptr [MMG_pc_g_pvIsKernel32Resume], offset L_body
        __asm mov dword ptr [MMG_pc_g_pvIsKernel32Exit], offset L_epi
        return 1;
    }
L_body:
    bResult = 0;
    if (*(unsigned short *)p_ucImage == IMAGE_DOS_SIGNATURE) {
        p_stNt = (IMAGE_NT_HEADERS *)(p_ucImage + ((IMAGE_DOS_HEADER *)p_ucImage)->e_lfanew);
        if (p_stNt->Signature == IMAGE_NT_SIGNATURE) {
            p_stOptional = &p_stNt->OptionalHeader;
            if (p_stOptional != NULL && p_stOptional->DataDirectory[IMAGE_DIRECTORY_ENTRY_EXPORT].VirtualAddress != 0) {
                p_stExport = (IMAGE_EXPORT_DIRECTORY *)(p_ucImage
                             + p_stOptional->DataDirectory[IMAGE_DIRECTORY_ENTRY_EXPORT].VirtualAddress);
                if (p_stExport != NULL) {
                    p_ulName = (unsigned long *)(p_ucImage + p_stExport->Name);
                    if ((p_ulName[0] & 0xdfdfdfdf) == 0x4e52454b
                        && (p_ulName[1] | 0x2020) == 0x32336c65
                        && (p_ulName[2] & 0xdfdfdfff) == 0x4c4c442e
                        && *(unsigned char *)&p_ulName[3] == 0)
                        bResult = 1;
                }
            }
        }
    }
L_epi:
    return bResult;
}

/* 0x00447C80 */
void *__cdecl MMG_pc_fn_pScanForKernel32(unsigned char *p_ucStart, long lCount, long lStep)
{
    void *pResult;

    __asm {
        call    L_install

        mov     eax, [esp + 0xc]
        push    ebx
        mov     ebx, [eax + 0xa8]
        sub     [eax + 0xb0], ebx
        dec     dword ptr [eax + 0xac]
        jne     L_handled
        mov     ebx, [eax + 0xc4]
        mov     [eax + 0xb0], ebx
L_handled:
        pop     ebx
        mov     eax, 0
        ret
L_install:
        push    dword ptr fs:[0]
        mov     dword ptr fs:[0], esp
        mov     eax, p_ucStart
        mov     ecx, lCount
        mov     edx, lStep
L_probe:
        mov     ebx, [eax]
        or      ecx, ecx
        je      L_none
        push    eax
        push    edx
        push    ecx
        push    eax
        call    MMG_pc_fn_bIsKernel32Image
        or      eax, eax
        pop     eax
        pop     ecx
        pop     edx
        pop     eax
        jne     L_unlink
        sub     eax, edx
        dec     ecx
        je      L_none
        jmp     L_probe
L_none:
        xor     eax, eax
L_unlink:
        mov     ebx, fs:[0]
        mov     ebx, [ebx]
        mov     fs:[0], ebx
        mov     pResult, eax
    }
    return pResult;
}

/* 0x004B72E0 */
void *MMG_pc_g_pvKernel32Base;

void *MMG_pc_g_pvFindKernel32Resume;     /* 0x00510DDC */
void *MMG_pc_g_pvFindKernel32Exit;       /* 0x00510C1C */

/* 0x00447D10 */
void *__cdecl MMG_pc_fn_pFindKernel32(void)
{
    unsigned long ulStart;

    if (SNA_g_lProtectionGate != 0) {
        __asm { mov dword ptr [MMG_pc_g_pvFindKernel32Resume], offset L_body }
        __asm { mov dword ptr [MMG_pc_g_pvFindKernel32Exit], offset L_epi }
        return NULL;
    }
L_body:
    if (MMG_pc_g_pvKernel32Base == NULL) {
        ulStart = 0;
        __asm {
            mov     eax, fs:[0x30]
            and     eax, 0x80000000
            shr     eax, 1
            add     eax, 0x80000000
            mov     ulStart, eax
        }
        MMG_pc_g_pvKernel32Base = MMG_pc_fn_pScanForKernel32((unsigned char *)ulStart, 0x1000, 0x10000);
        if (MMG_pc_g_pvKernel32Base == NULL) {
            MMG_pc_g_pvKernel32Base = MMG_pc_fn_pScanForKernel32((unsigned char *)0xc0000000, 0x1000, 0x10000);
            if (MMG_pc_g_pvKernel32Base == NULL) {
                MMG_pc_g_pvKernel32Base = MMG_pc_fn_pScanForKernel32((unsigned char *)0x80000000, 0x1000, 0x10000);
                if (MMG_pc_g_pvKernel32Base == NULL)
                    MMG_pc_g_pvKernel32Base = MMG_pc_fn_pScanForKernel32(NULL, 0xf000, 0xffff0000);
            }
        }
    }
L_epi:
    return MMG_pc_g_pvKernel32Base;
}

#pragma optimize("", on)

/* 0x00448DB0 */
void __cdecl MMG_pc_fn_vLoadAllModuleHandles(long bInit)
{
    unsigned long ulModule;

    for (ulModule = 0; ulModule < g_ucMMGTableSize; ulModule++)
        MMG_pc_fn_vLoadModuleHandle(ulModule, bInit);
}

/* 0x00448D80 */
void __cdecl MMG_pc_fn_vResolveAllImports(long bInit)
{
    unsigned long ulIndex;

    for (ulIndex = 1; ulIndex < g_ul113f0; ulIndex++)
        MMG_pc_fn_vResolveImport(ulIndex, bInit);
}

#pragma optimize("y", off)

void *MMG_pc_g_pvResolveModuleApiResume;    /* 0x00510DE0 */
void *MMG_pc_g_pvResolveModuleApiExit;      /* 0x00510C20 */

/* 0x004478A0 */
void __cdecl MMG_pc_fn_vResolveModuleApi(long bInit)
{
    unsigned long ulKey;
    unsigned long ulAddress;
    unsigned long hModule;

    if (SNA_g_lProtectionGate != 0) {
        __asm { mov dword ptr [MMG_pc_g_pvResolveModuleApiResume], offset L_body }
        __asm { mov dword ptr [MMG_pc_g_pvResolveModuleApiExit], offset L_epi }
        return;
    }
L_body:
    if (bInit) {
        g_p_ulMMGTable[0] = (unsigned int)MMG_pc_fn_pFindKernel32();
        MMG_pc_g_a_stImport[MMG_pc_C_ImpGetModuleHandleA].ulPtrXorKey =
            (MMG_pc_fn_ulFindImportInExports(MMG_pc_C_ImpGetModuleHandleA) + g_p_ulMMGTable[0])
            ^ MMG_pc_g_a_stImport[MMG_pc_C_ImpGetModuleHandleA].ulKey;
        return;
    }
    ulKey = MMG_pc_g_a_stImport[MMG_pc_C_ImpGetModuleHandleA].ulKey;
    if (MMG_pc_g_a_stImport[MMG_pc_C_ImpGetModuleHandleA].ulModule
            != MMG_pc_g_a_stImport[MMG_pc_C_ImpGetModuleHandleA].a_ulImporter[0]
        && g_p_ulMMGTable[MMG_pc_g_a_stImport[MMG_pc_C_ImpGetModuleHandleA].a_ulImporter[0]] != 0)
        ulAddress = MMG_pc_fn_ulFindImportInIat(MMG_pc_C_ImpGetModuleHandleA);
    else {
        hModule = (unsigned long)GetModuleHandleA(MMG_pc_fn_szDecryptModuleName(1));
        ulAddress = *MMG_pc_fn_p_ulFindIatSlot(hModule, MMG_pc_fn_p_stGetImport(MMG_pc_C_ImpGetModuleHandleA));
    }
    ulAddress ^= ulKey;
    if (ulAddress != MMG_pc_g_a_stImport[MMG_pc_C_ImpGetModuleHandleA].ulPtrXorKey)
        ulAddress = 0;
    MMG_pc_g_a_stImport[MMG_pc_C_ImpGetModuleHandleA].ulPtrXorKey = ulAddress;
L_epi:;
}

#pragma optimize("", on)

void *MMG_pc_g_pvCheckToolhelpResume;    /* 0x00510DD8 */
void *MMG_pc_g_pvCheckToolhelpExit;      /* 0x00510C18 */

/* 0x00447DE0 */
void __cdecl MMG_pc_fn_vInitProcessEnumeration(void)
{
    unsigned char *p_ucStub;
    unsigned char *p_ucEnd;
    void *hProcess;
    void *hSnapshot;
    char *p_szName;

    if (SNA_g_lProtectionGate != 0) {
        __asm { mov dword ptr [MMG_pc_g_pvCheckToolhelpResume], offset L_body }
        __asm { mov dword ptr [MMG_pc_g_pvCheckToolhelpExit], offset L_epi }
        return;
    }
L_body:
    if (MMG_pc_g_a_stImport[19].ulPtrXorKey != 0 && MMG_pc_g_a_stImport[20].ulPtrXorKey != 0
        && MMG_pc_g_a_stImport[21].ulPtrXorKey != 0 && MMG_pc_g_a_stImport[22].ulPtrXorKey != 0
        && MMG_pc_g_a_stImport[23].ulPtrXorKey != 0) {
        MMG_pc_M_vStepLcg();
        p_ucStub = &MMG_pc_g_uLcgPool.a_ucCode[(int)MMG_g_ulLCGState % MMG_pc_C_PoolSlots];
        MMG_pc_M_vWriteStub(p_ucStub, 19, 0xFF, 0xE0);
        MMG_pc_M_vFlushPool(hProcess);
        hSnapshot = ((void *(__stdcall *)(unsigned long, unsigned long))p_ucStub)(2, 0);
        for (p_ucEnd = p_ucStub + MMG_pc_C_StubSize; p_ucStub < p_ucEnd; p_ucStub++) {
            MMG_pc_M_vStepLcg();
            *p_ucStub = (unsigned char)((int)MMG_g_ulLCGState % 0xff);
        }
        if (hSnapshot == NULL) {
            MMG_pc_g_ulPsapiLoaded = 1;
            p_szName = MMG_pc_fn_szDecryptModuleName(2);
            MMG_pc_M_vStepLcg();
            p_ucStub = &MMG_pc_g_uLcgPool.a_ucCode[(int)MMG_g_ulLCGState % MMG_pc_C_PoolSlots];
            MMG_pc_M_vWriteStub(p_ucStub, 26, 0xFF, 0xE0);
            MMG_pc_M_vFlushPool(hProcess);
            ((void *(__stdcall *)(const char *))p_ucStub)(p_szName);
            for (p_ucEnd = p_ucStub + MMG_pc_C_StubSize; p_ucStub < p_ucEnd; p_ucStub++) {
                MMG_pc_M_vStepLcg();
                *p_ucStub = (unsigned char)((int)MMG_g_ulLCGState % 0xff);
            }
        } else {
            MMG_pc_g_ulPsapiLoaded = 0;
            return;
        }
    } else {
        MMG_pc_g_ulPsapiLoaded = 1;
        p_szName = MMG_pc_fn_szDecryptModuleName(2);
        MMG_pc_M_vStepLcg();
        p_ucStub = &MMG_pc_g_uLcgPool.a_ucCode[(int)MMG_g_ulLCGState % MMG_pc_C_PoolSlots];
        MMG_pc_M_vWriteStub(p_ucStub, 26, 0xFF, 0xE0);
        MMG_pc_M_vFlushPool(hProcess);
        ((void *(__stdcall *)(const char *))p_ucStub)(p_szName);
        for (p_ucEnd = p_ucStub + MMG_pc_C_StubSize; p_ucStub < p_ucEnd; p_ucStub++) {
            MMG_pc_M_vStepLcg();
            *p_ucStub = (unsigned char)((int)MMG_g_ulLCGState % 0xff);
        }
    }
    MMG_pc_fn_vLoadModuleHandle(2, 1);
    MMG_pc_fn_vResolveImport(24, 1);
    MMG_pc_fn_vResolveImport(25, 1);
L_epi:;
}

void __cdecl GLI_FatalExitOrSleep(void);   /* 0x004011C0 */

#ifndef RAYMAN2_NO_PROTECTION   /* CD check: boot-time search for the "RAYMAN2" CD volume */
#pragma optimize("y", off)
static void FUN_00448e30(void)
{
    long lDrive;
    char a_cVolumeName[0x84];
    unsigned long ulDrives;
    unsigned long ulDriveType;
    long bFound;
    char szText[52];
    char szCaption[28];
    char szRoot[8];
    unsigned char *p_ucStub;
    unsigned char *p_ucEnd;
    void *hProcess;

    if (g_ucMMGChannelMax != 0) {
        __asm { mov dword ptr [g_pfnMMG_510e08], offset L_cd_body }
        __asm { mov dword ptr [g_pfnMMG_510c48], offset L_cd_epi }
        return;
    }
L_cd_body:
    szRoot[3] = '\0';

    MMG_pc_M_vStepLcg();
    p_ucStub = &MMG_pc_g_uLcgPool.a_ucCode[(int)MMG_g_ulLCGState % MMG_pc_C_PoolSlots];
    MMG_pc_M_vWriteStub(p_ucStub, MMG_pc_C_ImpGetLogicalDrives, 0xFF, 0xE0);
    MMG_pc_M_vFlushPool(hProcess);
    ulDrives = ((unsigned long (__stdcall *)(void))p_ucStub)();
    for (p_ucEnd = p_ucStub + MMG_pc_C_StubSize; p_ucStub < p_ucEnd; p_ucStub++) {
        MMG_pc_M_vStepLcg();
        *p_ucStub = (unsigned char)((int)MMG_g_ulLCGState % 0xff);
    }

    szRoot[1] = ':';
    bFound = 0;
    szRoot[2] = '\\';
    do {
        for (lDrive = 0; lDrive < 32; lDrive++) {
            if ((ulDrives & (1 << lDrive)) == 0)
                continue;
            szRoot[0] = (char)lDrive + 'A';

            MMG_pc_M_vStepLcg();
            p_ucStub = &MMG_pc_g_uLcgPool.a_ucCode[(int)MMG_g_ulLCGState % MMG_pc_C_PoolSlots];
            MMG_pc_M_vWriteStub(p_ucStub, MMG_pc_C_ImpGetDriveTypeA, 0xFF, 0xE0);
            MMG_pc_M_vFlushPool(hProcess);
            ulDriveType = ((unsigned long (__stdcall *)(const char *))p_ucStub)(szRoot);
            for (p_ucEnd = p_ucStub + MMG_pc_C_StubSize; p_ucStub < p_ucEnd; p_ucStub++) {
                MMG_pc_M_vStepLcg();
                *p_ucStub = (unsigned char)((int)MMG_g_ulLCGState % 0xff);
            }
            if (ulDriveType != 5)
                continue;

            MMG_pc_M_vStepLcg();
            p_ucStub = &MMG_pc_g_uLcgPool.a_ucCode[(int)MMG_g_ulLCGState % MMG_pc_C_PoolSlots];
            MMG_pc_M_vWriteStub(p_ucStub, MMG_pc_C_ImpGetVolumeInformationA, 0x50, 0xC3);
            MMG_pc_M_vFlushPool(hProcess);
            ((int (__stdcall *)(const char *, char *, unsigned long, unsigned long *, unsigned long *,
                                unsigned long *, char *, unsigned long))p_ucStub)(
                szRoot, a_cVolumeName, 0x80, 0, 0, 0, 0, 0);
            for (p_ucEnd = p_ucStub + MMG_pc_C_StubSize; p_ucStub < p_ucEnd; p_ucStub++) {
                MMG_pc_M_vStepLcg();
                *p_ucStub = (unsigned char)((int)MMG_g_ulLCGState % 0xff);
            }
            if (a_cVolumeName[1] == 'A' && a_cVolumeName[2] == 'Y' && a_cVolumeName[5] == 'N'
                && a_cVolumeName[3] == 'M' && a_cVolumeName[4] == 'A' && a_cVolumeName[6] == '2'
                && a_cVolumeName[0] == 'R' && strlen(a_cVolumeName) == 7) {
                bFound = 1;
                g_cCDROM = (char)lDrive + 'A';
                break;
            }
        }

        if (!bFound) {
        szText[0x29] = 'e'; szText[0x27] = 'i'; szText[0x25] = 'D'; szText[0x24] = ' ';
        szText[0x23] = 'M'; szText[0x20] = 'D'; szText[0x1e] = ' '; szText[0x1c] = 'i';
        szText[0x1b] = ' '; szText[0x18] = ' '; szText[0x16] = 'I'; szText[0x14] = 'n';
        szText[0x11] = 'y'; szText[0x10] = 'a'; szText[0x0f] = 'R'; szText[0x0e] = ' ';
        szText[0x09] = 'n'; szText[0x07] = ' '; szText[0x06] = ','; szText[0x04] = 's';
        szText[0x03] = 'a'; szText[0x00] = 'P'; szText[0x01] = 'l'; szText[0x02] = 'e';
        szText[0x05] = 'e'; szText[0x08] = 'i'; szText[0x0a] = 's'; szText[0x0b] = 'e';
        szText[0x0c] = 'r'; szText[0x0d] = 't'; szText[0x12] = 'm'; szText[0x13] = 'a';
        szText[0x15] = ' '; szText[0x17] = 'I'; szText[0x19] = 'C'; szText[0x1a] = 'D';
        szText[0x1d] = 'n'; szText[0x1f] = 'C'; szText[0x21] = 'R'; szText[0x22] = 'O';
        szText[0x26] = 'r'; szText[0x28] = 'v'; szText[0x2a] = '\0';
        szCaption[0x12] = 'u'; szCaption[0x11] = 'o'; szCaption[0x0f] = ' '; szCaption[0x0e] = 't';
        szCaption[0x0d] = 'o'; szCaption[0x0b] = ' '; szCaption[0x0a] = 'D'; szCaption[0x09] = 'C';
        szCaption[0x08] = ' '; szCaption[0x07] = '2'; szCaption[0x05] = 'n'; szCaption[0x03] = 'm';
        szCaption[0x02] = 'y'; szCaption[0x01] = 'a'; szCaption[0x00] = 'R'; szCaption[0x04] = 'a';
        szCaption[0x06] = ' '; szCaption[0x0c] = 'n'; szCaption[0x10] = 'f'; szCaption[0x13] = 'n';
        szCaption[0x14] = 'd'; szCaption[0x15] = '\0';
        if (MessageBoxA(NULL, szText, szCaption, MB_RETRYCANCEL) == IDCANCEL)
            GLI_FatalExitOrSleep();
        }
    } while (!bFound);
L_cd_epi:;
}
#pragma optimize("", on)
#endif /* RAYMAN2_NO_PROTECTION */

static __inline unsigned int lcg_next(unsigned int seed)
{
    unsigned int s = seed ^ 0x75bd924u;
    unsigned int next = s * 0x41a7u + (unsigned int)((int)s / 0x1f31d * -0x7fffffff);

    if ((int)next < 0) {
        next += 0x7fffffffu;
    }

    return next;
}

/* 0x00448480 */
unsigned int __cdecl MMG_pc_fn_ulStepLcgState(void)
{
    MMG_pc_M_vStepLcg();
    return MMG_g_ulLCGState;
}

/* 0x004484D0 */
long __stdcall MMG_pc_fn_bFlushInstructionCache(void *hProcess, const void *p_vBase, unsigned long ulSize)
{
    long (__stdcall *pfn_bFlush)(void *, const void *, unsigned long);

    pfn_bFlush = (long (__stdcall *)(void *, const void *, unsigned long))
        MMG_pc_M_ulImportAddress(MMG_pc_C_ImpFlushInstructionCache);
    return pfn_bFlush(hProcess, p_vBase, ulSize);
}

/* 0x00448500 */
long __stdcall MMG_pc_fn_bCloseHandle(void *hObject)
{
    unsigned char *p_ucStub;
    unsigned char *p_ucEnd;
    void *hProcess;
    long bResult;

    MMG_pc_M_vStepLcg();
    p_ucStub = &MMG_pc_g_uLcgPool.a_ucCode[(int)MMG_g_ulLCGState % MMG_pc_C_PoolSlots];
    p_ucStub[0] = 0x68;
    *(unsigned long *)(p_ucStub + 1) = MMG_pc_g_a_stImport[11].ulPtrXorKey ^ MMG_pc_g_a_stImport[11].ulKey;
    p_ucStub[5] = 0xC3;
    MMG_pc_M_vFlushPool(hProcess);
    bResult = ((long (__stdcall *)(void *))p_ucStub)(hObject);
    for (p_ucEnd = p_ucStub + MMG_pc_C_StubSize; p_ucStub < p_ucEnd; p_ucStub++) {
        MMG_pc_M_vStepLcg();
        *p_ucStub = (unsigned char)((int)MMG_g_ulLCGState % 0xff);
    }
    return bResult;
}

/* 0x00448970 */
unsigned long __cdecl MMG_pc_fn_ulFindExportRvaInDirectory(unsigned long hModule,
                                                          IMAGE_EXPORT_DIRECTORY *p_stExport,
                                                          MMG_pc_tdstImport *p_stImport)
{
    unsigned long *p_ulNames;
    unsigned long *p_ulFunctions;
    unsigned short *p_uwOrdinals;
    long lI;
    long lNbNames;
    unsigned long ulFunction;

    if (hModule == 0 || p_stExport == NULL)
        return 0;

    p_ulNames = (unsigned long *)(hModule + (unsigned long)p_stExport->AddressOfNames);
    p_ulFunctions = (unsigned long *)(hModule + (unsigned long)p_stExport->AddressOfFunctions);
    p_uwOrdinals = (unsigned short *)(hModule + (unsigned long)p_stExport->AddressOfNameOrdinals);
    lNbNames = p_stExport->NumberOfNames;
    for (lI = 0; lI < lNbNames; lI++, p_ulNames++, p_uwOrdinals++) {
        ulFunction = p_ulFunctions[*p_uwOrdinals];
        if (MMG_pc_fn_lCompareImportName((unsigned char *)(hModule + *p_ulNames), p_stImport->ucNameLen,
                                         (unsigned char *)p_stImport->a_cName, p_stImport->ulKey) == 0)
            return ulFunction;
    }
    return 0;
}

void *MMG_pc_g_pvCountToolhelpResume;    /* 0x00510DD4 */
void *MMG_pc_g_pvCountToolhelpExit;      /* 0x00510C14 */

#pragma optimize("y", off)

/* 0x00448610 */
unsigned long __cdecl MMG_pc_fn_ulCountExportersToolhelp(MMG_pc_tdstImport *p_stImport)
{
    MODULEENTRY32 stEntry;
    unsigned long ulCount;
    void *hSnapshot;
    long bMore;
    unsigned char *p_ucStub;
    unsigned char *p_ucEnd;
    void *hProcess;
    unsigned long hModule;
    unsigned long ulRva;
    long lOffset;

    ulCount = 0;
    if (SNA_g_lProtectionGate != 0) {
        __asm { mov dword ptr [MMG_pc_g_pvCountToolhelpResume], offset L_body }
        __asm { mov dword ptr [MMG_pc_g_pvCountToolhelpExit], offset L_epi }
        return 0;
    }
L_body:
    lOffset = (int)MMG_pc_fn_ulStepLcgState() % MMG_pc_C_PoolSlots;
    MMG_pc_M_vWriteStub(&MMG_pc_g_uLcgPool.a_ucCode[lOffset], 19, 0xFF, 0xE0);
    p_ucStub = &MMG_pc_g_uLcgPool.a_ucCode[lOffset];
    MMG_pc_M_vFlushPool(hProcess);
    hSnapshot = ((void *(__stdcall *)(unsigned long, unsigned long))p_ucStub)(TH32CS_SNAPMODULE, 0);
    for (p_ucEnd = p_ucStub + MMG_pc_C_StubSize; p_ucStub < p_ucEnd; p_ucStub++) {
        MMG_pc_M_vStepLcg();
        *p_ucStub = (unsigned char)((int)MMG_g_ulLCGState % 0xff);
    }
    stEntry.dwSize = sizeof(MODULEENTRY32);

    lOffset = (int)MMG_pc_fn_ulStepLcgState() % MMG_pc_C_PoolSlots;
    MMG_pc_g_uLcgPool.a_ucCode[lOffset] = 0xE9;
    p_ucStub = &MMG_pc_g_uLcgPool.a_ucCode[lOffset];
    *(unsigned long *)(p_ucStub + 1) = (MMG_pc_g_a_stImport[22].ulPtrXorKey ^ MMG_pc_g_a_stImport[22].ulKey)
                                       - (unsigned long)p_ucStub - 5;
    MMG_pc_M_vFlushPool(hProcess);
    bMore = ((long (__stdcall *)(void *, MODULEENTRY32 *))p_ucStub)(hSnapshot, &stEntry);
    for (p_ucEnd = p_ucStub + MMG_pc_C_StubSize; p_ucStub < p_ucEnd; p_ucStub++) {
        MMG_pc_M_vStepLcg();
        *p_ucStub = (unsigned char)((int)MMG_g_ulLCGState % 0xff);
    }
    if (bMore) {
        do {
            hModule = (unsigned long)stEntry.hModule;
            ulRva = MMG_pc_M_ulExportRva(hModule);
            if ((ulRva != 0 ? MMG_pc_fn_ulFindExportRvaInDirectory(
                                  hModule, (IMAGE_EXPORT_DIRECTORY *)(hModule + ulRva), p_stImport)
                            : 0)
                != 0)
                ulCount++;

            lOffset = (int)MMG_pc_fn_ulStepLcgState() % MMG_pc_C_PoolSlots;
            MMG_pc_g_uLcgPool.a_ucCode[lOffset] = 0xE9;
            p_ucStub = &MMG_pc_g_uLcgPool.a_ucCode[lOffset];
            *(unsigned long *)(p_ucStub + 1) =
                (MMG_pc_g_a_stImport[23].ulPtrXorKey ^ MMG_pc_g_a_stImport[23].ulKey) - (unsigned long)p_ucStub - 5;
            MMG_pc_M_vFlushPool(hProcess);
            bMore = ((long (__stdcall *)(void *, MODULEENTRY32 *))p_ucStub)(hSnapshot, &stEntry);
            for (p_ucEnd = p_ucStub + MMG_pc_C_StubSize; p_ucStub < p_ucEnd; p_ucStub++) {
                MMG_pc_M_vStepLcg();
                *p_ucStub = (unsigned char)((int)MMG_g_ulLCGState % 0xff);
            }
        } while (bMore);
    }

    lOffset = (int)MMG_pc_fn_ulStepLcgState() % MMG_pc_C_PoolSlots;
    MMG_pc_g_uLcgPool.a_ucCode[lOffset] = 0x68;
    p_ucStub = &MMG_pc_g_uLcgPool.a_ucCode[lOffset];
    *(unsigned long *)(p_ucStub + 1) = MMG_pc_g_a_stImport[11].ulPtrXorKey ^ MMG_pc_g_a_stImport[11].ulKey;
    p_ucStub[5] = 0xC3;
    MMG_pc_M_vFlushPool(hProcess);
    ((long (__stdcall *)(void *))p_ucStub)(hSnapshot);
    for (p_ucEnd = p_ucStub + MMG_pc_C_StubSize; p_ucStub < p_ucEnd; p_ucStub++)
        *p_ucStub = (unsigned char)((int)MMG_pc_fn_ulStepLcgState() % 0xff);
L_epi:
    return ulCount;
}

void *MMG_pc_g_pvCountPsapiResume;       /* 0x00510DD0 */
void *MMG_pc_g_pvCountPsapiExit;         /* 0x00510C10 */

/* 0x004481B0 */
unsigned long __cdecl MMG_pc_fn_ulCountExportersPsapi(MMG_pc_tdstImport *p_stImport)
{
    unsigned long a_hModule[0x400];
    unsigned long ulCount;
    unsigned long ulNeeded;
    unsigned long ulNbModule;
    unsigned long ulI;
    void *hProcess;
    void *hFlushProcess;
    long bOk;
    unsigned char *p_ucStub;
    unsigned char *p_ucEnd;
    unsigned long hModule;
    unsigned long ulRva;

    ulCount = 0;
    if (SNA_g_lProtectionGate != 0) {
        __asm { mov dword ptr [MMG_pc_g_pvCountPsapiResume], offset L_body }
        __asm { mov dword ptr [MMG_pc_g_pvCountPsapiExit], offset L_epi }
        return 0;
    }
L_body:
    hProcess = ((void *(__stdcall *)(void))MMG_pc_M_ulImportAddress(MMG_pc_C_ImpGetCurrentProcess))();
    MMG_pc_M_vStepLcg();
    p_ucStub = &MMG_pc_g_uLcgPool.a_ucCode[(int)MMG_g_ulLCGState % MMG_pc_C_PoolSlots];
    MMG_pc_M_vWriteStub(p_ucStub, 25, 0x50, 0xC3);
    MMG_pc_M_vFlushPool(hFlushProcess);
    bOk = ((long (__stdcall *)(void *, unsigned long *, unsigned long, unsigned long *))p_ucStub)(
        hProcess, a_hModule, sizeof(a_hModule), &ulNeeded);
    for (p_ucEnd = p_ucStub + MMG_pc_C_StubSize; p_ucStub < p_ucEnd; p_ucStub++) {
        MMG_pc_M_vStepLcg();
        *p_ucStub = (unsigned char)((int)MMG_g_ulLCGState % 0xff);
    }
    if (!bOk)
        return 0;
    ulNbModule = ulNeeded / sizeof(unsigned long);
    if (ulNbModule == 1)
        return 0;
    for (ulI = 0; ulI < ulNbModule; ulI++) {
        hModule = a_hModule[ulI];
        ulRva = MMG_pc_M_ulExportRva(hModule);
        if ((ulRva != 0 ? MMG_pc_fn_ulFindExportRva(hModule, (IMAGE_EXPORT_DIRECTORY *)(hModule + ulRva),
                                                    p_stImport)
                        : 0)
            != 0)
            ulCount++;
    }
L_epi:
    return ulCount;
}

void *MMG_pc_g_pvIatDeltaResume;         /* 0x00510DE4 */
void *MMG_pc_g_pvIatDeltaExit;           /* 0x00510C24 */

/* 0x00448AB0 */
unsigned long __cdecl MMG_pc_fn_ulGetExeIatDelta(MMG_pc_tdstImport *p_stImport)
{
    MMG_pc_tdstImport *p_stRecord;
    unsigned long ulKey;
    unsigned char *p_ucStub;
    unsigned char *p_ucEnd;
    void *hProcess;
    unsigned long hModule;
    unsigned long *p_ulSlot;
    unsigned long ulAddress;

    p_stRecord = MMG_pc_g_a_stImport;
    if (SNA_g_lProtectionGate != 0) {
        __asm { mov dword ptr [MMG_pc_g_pvIatDeltaResume], offset L_body }
        __asm { mov dword ptr [MMG_pc_g_pvIatDeltaExit], offset L_epi }
        return 0;
    }
L_body:
    ulKey = p_stImport->ulKey;
    MMG_pc_M_vStepLcg();
    p_ucStub = &MMG_pc_g_uLcgPool.a_ucCode[(int)MMG_g_ulLCGState % MMG_pc_C_PoolSlots];
    MMG_pc_M_vWriteStub(p_ucStub, MMG_pc_C_ImpGetModuleHandleA, 0xFF, 0xE0);
    MMG_pc_M_vFlushPool(hProcess);
    hModule = ((unsigned long (__stdcall *)(const char *))p_ucStub)(NULL);
    for (p_ucEnd = p_ucStub + MMG_pc_C_StubSize; p_ucStub < p_ucEnd; p_ucStub++) {
        MMG_pc_M_vStepLcg();
        *p_ucStub = (unsigned char)((int)MMG_g_ulLCGState % 0xff);
    }
    p_ulSlot = MMG_pc_fn_p_ulFindIatSlot(hModule, p_stImport);
    ulAddress = p_ulSlot != NULL ? *p_ulSlot : 0;
    if (ulAddress != 0) {
        while (*(char *)p_stRecord != '\0' && strcmp((char *)p_stRecord, (char *)p_stImport) != 0)
            p_stRecord++;
        if (*(char *)p_stRecord != '\0')
            return ulAddress - (p_stRecord->ulPtrXorKey ^ ulKey);
    }
    return 0;
L_epi:;
}

#pragma optimize("", on)

void *MMG_pc_g_pvCountExportersResume;   /* 0x00510DCC */
void *MMG_pc_g_pvCountExportersExit;     /* 0x00510C0C */

#pragma optimize("y", off)

/* 0x00448DE0 */
unsigned long __cdecl MMG_pc_fn_ulCountExporters(MMG_pc_tdstImport *p_stImport)
{
    if (SNA_g_lProtectionGate != 0) {
        __asm { mov dword ptr [MMG_pc_g_pvCountExportersResume], offset L_body }
        __asm { mov dword ptr [MMG_pc_g_pvCountExportersExit], offset L_epi }
        return 0;
    }
L_body:
    if (MMG_pc_g_ulPsapiLoaded)
        return MMG_pc_fn_ulCountExportersPsapi(p_stImport);
    return MMG_pc_fn_ulCountExportersToolhelp(p_stImport);
L_epi:;
}

#pragma optimize("", on)

#pragma optimize("y", off)
void __cdecl MMG_fn_vInit(int bInitMMG)
{
    LARGE_INTEGER qpc;
    BOOL qpcOk;
    unsigned int uSeed;
    unsigned int *pTable;
    unsigned int i;

    if (g_ucMMGChannelMax != 0) {
        __asm { mov dword ptr [g_pfnMMG_510dc8], offset L_body }
        __asm { mov dword ptr [g_pfnMMG_510c08], offset L_epi }
        return;
    }
L_body:
#ifndef RAYMAN2_NO_PROTECTION   /* LCG and code pool of the protection, module-handle table of the private imports */
    qpcOk = QueryPerformanceCounter(&qpc);
    uSeed = qpc.LowPart;
    if (!qpcOk) {
        uSeed = 0x12345678u;
    }
#ifdef RAYMAN2_REGRESS
    if (REGRESS_fn_bActive())
        uSeed = REGRESS_C_ulLcgSeed;
#endif
    pTable = MMG_pc_g_uLcgPool.a_ulWord;

    for (i = 0x119; i != 0; i--) {
        uSeed = lcg_next(uSeed);
        *pTable++ = uSeed;
    }

    g_ulLCGState = uSeed;

    if (bInitMMG) {
        g_ucMMGTableSize = 7;
        g_ul113f0 = 0x24;
        g_p_ulMMGTable = (unsigned int *)malloc(0x1c);

        pTable = g_p_ulMMGTable;
        for (i = g_ucMMGTableSize & 0x3fffffffu; i != 0; i--) {
            g_ulLCGState = lcg_next(g_ulLCGState);
            *pTable++ = g_ulLCGState;
        }
    }

    MMG_pc_fn_vResolveModuleApi(bInitMMG);
    MMG_pc_fn_vLoadAllModuleHandles(bInitMMG);
    MMG_pc_fn_vResolveAllImports(bInitMMG);

    if (bInitMMG) {
        MMG_pc_fn_vInitProcessEnumeration();
        FUN_00448e30();
    }
#else
    /* Copy protection removed: no private import table, no CD drive search. The CD fallback
       paths ("<drive>\\<path>") resolve against the working directory. */
    if (bInitMMG) {
        g_cCDROM = '.';
    }
#endif /* RAYMAN2_NO_PROTECTION */
L_epi:;
}
#pragma optimize("", on)

/* 0x00511404 */
void *MMG_pc_g_pvNopGateResume;
void *MMG_pc_g_pvNopGateExit;

#define MMG_pc_M_vNop16 \
    __asm _emit 0x90 __asm _emit 0x90 __asm _emit 0x90 __asm _emit 0x90 \
    __asm _emit 0x90 __asm _emit 0x90 __asm _emit 0x90 __asm _emit 0x90 \
    __asm _emit 0x90 __asm _emit 0x90 __asm _emit 0x90 __asm _emit 0x90 \
    __asm _emit 0x90 __asm _emit 0x90 __asm _emit 0x90 __asm _emit 0x90
#define MMG_pc_M_vNop64 \
    MMG_pc_M_vNop16 MMG_pc_M_vNop16 MMG_pc_M_vNop16 MMG_pc_M_vNop16

__declspec(naked) void __cdecl MMG_pc_fn_vNopGate(void)
{
    __asm {
        push    ebp
        mov     ebp, esp
        sub     esp, 0x110
        mov     eax, dword ptr [SNA_g_lProtectionGate]
        push    ebx
        push    esi
        push    edi
        test    eax, eax
        je      L_body
        mov     dword ptr [MMG_pc_g_pvNopGateResume], offset L_body
        mov     dword ptr [MMG_pc_g_pvNopGateExit], offset L_epi
        pop     edi
        pop     esi
        pop     ebx
        mov     esp, ebp
        pop     ebp
        ret
L_body:
        MMG_pc_M_vNop64 MMG_pc_M_vNop64 MMG_pc_M_vNop64 MMG_pc_M_vNop64
        MMG_pc_M_vNop64 MMG_pc_M_vNop64 MMG_pc_M_vNop64 MMG_pc_M_vNop64
        MMG_pc_M_vNop64 MMG_pc_M_vNop64 MMG_pc_M_vNop64 MMG_pc_M_vNop64
        MMG_pc_M_vNop64 MMG_pc_M_vNop16
        __asm _emit 0x90 __asm _emit 0x90 __asm _emit 0x90 __asm _emit 0x90 __asm _emit 0x90
        __asm _emit 0x90 __asm _emit 0x90 __asm _emit 0x90 __asm _emit 0x90
L_epi:
        pop     edi
        pop     esi
        pop     ebx
        mov     esp, ebp
        pop     ebp
        ret
    }
}
