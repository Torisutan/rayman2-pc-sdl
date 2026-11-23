#include <stddef.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <windows.h>
#include <intrin.h>

#include "MMG/SNA.h"
#include "MMG/MMG_globals.h"

#define SNA_g_ucErrorMode       SNA_g_lProtectionGate

size_t __cdecl SNA_fn_ulFRead(void   *_p_vBuffer,
                              size_t  _ulSize,
                              size_t  _ulCount,
                              void   *_p_stFile)
{
    SNA_pc_tdstStream *p_stStream;
    unsigned char *p_ucIn;
    size_t         ulBytesToRead;

#ifndef RAYMAN2_NO_PROTECTION   /* protection gate */
    if (SNA_g_ucErrorMode != 0) {
        __asm { mov dword ptr [SNA_g_pErrorFrame1], offset L_body }
        __asm { mov dword ptr [SNA_g_pErrorFrame2], offset L_epi }
        return 0;
    }
L_body:
#endif /* RAYMAN2_NO_PROTECTION */
    ulBytesToRead = _ulSize * _ulCount;
    if (ulBytesToRead == 0) {
        return 0;
    }

    p_stStream = (SNA_pc_tdstStream *)_p_stFile;
    p_ucIn     = (unsigned char *)_p_vBuffer;
    if (p_stStream->pfn_bRawRead(p_ucIn, ulBytesToRead, p_stStream) == 0) {
        return 0;
    }

    /* 0x450EB0 */
    if (SNA_pc_bGetFlagBit0(p_stStream) != 0) {
        unsigned char ucFill;
        unsigned char ucTrailingBytes;
        unsigned long ulRoom;
        unsigned long ulXorWords;
        size_t        ulRemain = ulBytesToRead;

        if (p_stStream->ucCarryUsed != 0) {
            ulRoom = 4u - p_stStream->ucCarryUsed;
            ucFill = (unsigned char)ulRemain;
            if (ulRemain >= ulRoom)
                ucFill = (unsigned char)ulRoom;
            memcpy(&p_stStream->uCarry.a_ucByte[p_stStream->ucCarryUsed], _p_vBuffer, ucFill);
            p_stStream->ucCarryUsed += ucFill;
            p_ucIn = (unsigned char *)_p_vBuffer + ucFill;
            ulRemain -= ucFill;
            if (p_stStream->ucCarryUsed == 4) {
                p_stStream->ucCarryUsed = 0;
                p_stStream->ulChecksum ^= p_stStream->uCarry.ulWord;
            }
        }

        ucTrailingBytes = (unsigned char)(ulRemain & 3u);
        for (ulXorWords = ulRemain >> 2; ulXorWords != 0; ulXorWords--) {
            p_stStream->ulChecksum ^= *(unsigned long *)p_ucIn;
            p_ucIn += 4;
        }

        if (ucTrailingBytes != 0) {
            memcpy(&p_stStream->uCarry.a_ucByte[p_stStream->ucCarryUsed], p_ucIn, ucTrailingBytes);
            p_stStream->ucCarryUsed += ucTrailingBytes;
        }
    }

    return _ulCount;
#ifndef RAYMAN2_NO_PROTECTION
L_epi:;
#endif /* RAYMAN2_NO_PROTECTION */
}

/* 0x00450EB0 */
unsigned char __cdecl SNA_pc_bGetFlagBit0(void *_p_stFile)
{
    return ((SNA_pc_tdstStream *)_p_stFile)->ucFlags & 1u;
}

/* 0x00450EC0 */
unsigned char __cdecl SNA_pc_bGetFlagBit1(void *_p_stFile)
{
    return (unsigned char)((((SNA_pc_tdstStream *)_p_stFile)->ucFlags >> 1) & 1u);
}

/* 0x00450ED0 */
unsigned char __cdecl SNA_pc_bGetFlagBit2(void *_p_stFile)
{
    return (unsigned char)((((SNA_pc_tdstStream *)_p_stFile)->ucFlags & 4u) >> 2);
}

#ifndef RAYMAN2_NO_PROTECTION
unsigned char __cdecl SNA_pc_bRawReadDecodeStubbed(void *_p_vBuffer, size_t _ulBytes,
                                                   void *_p_stFile);
unsigned char __cdecl SNA_pc_bRawReadPlainStubbed(void *_p_vBuffer, size_t _ulBytes,
                                                  void *_p_stFile);
#endif /* RAYMAN2_NO_PROTECTION */

/* 0x00450A00 */
void __cdecl SNA_pc_vInitStream(unsigned char _ucFlags, void *_p_stFile)
{
    SNA_pc_tdstStream *p_stStream = (SNA_pc_tdstStream *)_p_stFile;

    p_stStream->ulChecksum        = 0;
    p_stStream->ucCarryUsed       = 0;
    p_stStream->ucFlags           = _ucFlags;
    p_stStream->ul_pc_20          = 0;
    p_stStream->pfn_bRawReadPlain = SNA_pc_bRawReadPlain;

    if (SNA_pc_bGetFlagBit1(_p_stFile) != 0) {
        p_stStream->uKey.ulState  = SNA_pc_C_ulStreamSeed;
        p_stStream->ul_pc_1C      = 0;
#ifndef RAYMAN2_NO_PROTECTION
        p_stStream->pfn_bRawRead  = SNA_pc_bGetFlagBit2(_p_stFile)
                                    ? SNA_pc_bRawReadDecodeStubbed
                                    : SNA_pc_bRawReadDecode;
#else
        /* Copy protection removed: flag bit 2 (read through the code-pool stubs) reads directly. */
        p_stStream->pfn_bRawRead  = SNA_pc_bRawReadDecode;
#endif /* RAYMAN2_NO_PROTECTION */
    } else {
        p_stStream->uKey.ulState  = 0;
        p_stStream->ul_pc_1C      = 0;
#ifndef RAYMAN2_NO_PROTECTION
        p_stStream->pfn_bRawRead  = SNA_pc_bGetFlagBit2(_p_stFile)
                                    ? SNA_pc_bRawReadPlainStubbed
                                    : SNA_pc_bRawReadPlain;
#else
        p_stStream->pfn_bRawRead  = SNA_pc_bRawReadPlain;
#endif /* RAYMAN2_NO_PROTECTION */
    }
}

/* 0x00450A80 */
unsigned char __cdecl SNA_pc_bOpenFile(const char *_szPath, char _cMode,
                                       unsigned char _ucFlags, void *_p_stFile)
{
    SNA_pc_tdstStream *p_stStream;
    DWORD  dwDesiredAccess;
    DWORD  dwCreationDisposition;
    DWORD  dwAttrs;
    HANDLE hFile;
    unsigned char *p_ucStub;
    unsigned char *p_ucEnd;
    void  *hProcess;

    p_stStream = (SNA_pc_tdstStream *)_p_stFile;
    SNA_pc_vInitStream(_ucFlags, _p_stFile);

    switch ((unsigned char)_cMode) {
    case 1:
        dwCreationDisposition = 2;
        dwDesiredAccess       = 0x40000000;
        dwAttrs = GetFileAttributesA(_szPath);
        if ((dwAttrs & 1) != 0) {
            SetFileAttributesA(_szPath, dwAttrs & 0xFFFFFFFEu);
        }
        break;
    case 2:
        dwCreationDisposition = 3;
        dwDesiredAccess       = 0x80000000;
        break;
    default:
        __asm int 3
        break;
    }

#ifndef RAYMAN2_NO_PROTECTION   /* flag bit 2: CreateFileA through a code-pool stub */
    if (SNA_pc_bGetFlagBit2(_p_stFile) != 0) {

        MMG_pc_M_vStepLcg();
        p_ucStub = &MMG_pc_g_uLcgPool.a_ucCode[(int)MMG_g_ulLCGState % MMG_pc_C_PoolSlots];
        MMG_pc_M_vWriteStub(p_ucStub, MMG_pc_C_ImpCreateFileA, 0xFF, 0xE0);
        MMG_pc_M_vFlushPool(hProcess);
        hFile = ((HANDLE (__stdcall *)(LPCSTR, DWORD, DWORD, LPSECURITY_ATTRIBUTES, DWORD, DWORD, HANDLE))
                 p_ucStub)(_szPath, dwDesiredAccess, 0, NULL, dwCreationDisposition,
                           0x80  , NULL);
        for (p_ucEnd = p_ucStub + MMG_pc_C_StubSize; p_ucStub < p_ucEnd; p_ucStub++) {
            MMG_pc_M_vStepLcg();
            *p_ucStub = (unsigned char)((int)MMG_g_ulLCGState % 0xff);
        }
        p_stStream->hFile = hFile;
    } else
#endif /* RAYMAN2_NO_PROTECTION */
    {
        p_stStream->hFile = CreateFileA(_szPath, dwDesiredAccess, 0, NULL,
                                        dwCreationDisposition, 0x80  ,
                                        NULL);
    }
    p_stStream->ulValid = (p_stStream->hFile != INVALID_HANDLE_VALUE);
    return (unsigned char)p_stStream->ulValid;
}

/* 0x00450C40 */
unsigned char __cdecl SNA_pc_bCloseFile(void *_p_stFile)
{
    SNA_pc_tdstStream *p_stStream = (SNA_pc_tdstStream *)_p_stFile;
    unsigned char *p_ucStub;
    unsigned char *p_ucEnd;
    void *hProcess;
    HANDLE hFile;
    BOOL bOk;

    p_stStream->ucCarryUsed = 0;
#ifndef RAYMAN2_NO_PROTECTION   /* flag bit 2: CloseHandle through a code-pool stub */
    if (SNA_pc_bGetFlagBit2(_p_stFile) != 0) {

        hFile = p_stStream->hFile;
        MMG_pc_M_vStepLcg();
        p_ucStub = &MMG_pc_g_uLcgPool.a_ucCode[(int)MMG_g_ulLCGState % MMG_pc_C_PoolSlots];
        p_ucStub[0] = 0x68;
        *(unsigned long *)(p_ucStub + 1) = MMG_pc_g_a_stImport[MMG_pc_C_ImpCloseHandle].ulPtrXorKey
                                           ^ MMG_pc_g_a_stImport[MMG_pc_C_ImpCloseHandle].ulKey;
        p_ucStub[5] = 0xC3;
        MMG_pc_M_vFlushPool(hProcess);
        bOk = ((BOOL (__stdcall *)(HANDLE))p_ucStub)(hFile);
        for (p_ucEnd = p_ucStub + MMG_pc_C_StubSize; p_ucStub < p_ucEnd; p_ucStub++) {
            MMG_pc_M_vStepLcg();
            *p_ucStub = (unsigned char)((int)MMG_g_ulLCGState % 0xff);
        }
        p_stStream->hFile = INVALID_HANDLE_VALUE;
        return (unsigned char)(bOk == 0);
    }
#endif /* RAYMAN2_NO_PROTECTION */
    bOk = CloseHandle(p_stStream->hFile);
    p_stStream->hFile = INVALID_HANDLE_VALUE;
    return (unsigned char)(bOk == 0);
}

/* 0x004512C0 */
unsigned char __cdecl SNA_pc_bRawReadPlain(void *_p_vBuffer, size_t _ulBytes,
                                           void *_p_stFile)
{
    SNA_pc_tdstStream *p_stStream = (SNA_pc_tdstStream *)_p_stFile;
    DWORD     dwBytesRead;
    BOOL      bOk;

    bOk = ReadFile(p_stStream->hFile, _p_vBuffer, (DWORD)_ulBytes,
                   &dwBytesRead, NULL);
    p_stStream->ulValid = (unsigned long)bOk;
    return (unsigned char)(dwBytesRead == (DWORD)_ulBytes);
}

/* 0x00451090 */
unsigned char __cdecl SNA_pc_bRawReadDecode(void *_p_vBuffer, size_t _ulBytes,
                                            void *_p_stFile)
{
    unsigned char *p_ucBuffer = (unsigned char *)_p_vBuffer;
    unsigned char *p_ucEnd    = p_ucBuffer + _ulBytes;
    SNA_pc_tdstStream *p_stStream = (SNA_pc_tdstStream *)_p_stFile;
    DWORD          dwBytesRead;
    BOOL           bOk;

    bOk = ReadFile(p_stStream->hFile, _p_vBuffer, (DWORD)_ulBytes,
                   &dwBytesRead, NULL);
    p_stStream->ulValid = (unsigned long)bOk;
    if (dwBytesRead != (DWORD)_ulBytes) {
        return 0;
    }

    for (; p_ucBuffer < p_ucEnd; p_ucBuffer++) {
        uint32_t s;
        uint32_t q;
        *p_ucBuffer ^= p_stStream->uKey.a_ucByte[1];
        s = p_stStream->uKey.ulState ^ 0x075BD924u;
        q = s / 127773u;
        p_stStream->uKey.ulState = s * 16807u - q * 0x7FFFFFFFu;
    }
    return 1;
}

#ifndef RAYMAN2_NO_PROTECTION   /* reads through a code-pool stub (flag bit 2) */
/* 0x00451110 */
unsigned char __cdecl SNA_pc_bRawReadDecodeStubbed(void *_p_vBuffer, size_t _ulBytes,
                                                   void *_p_stFile)
{
    unsigned char *p_ucBuffer = (unsigned char *)_p_vBuffer;
    unsigned char *p_ucEnd    = p_ucBuffer + _ulBytes;
    SNA_pc_tdstStream *p_stStream = (SNA_pc_tdstStream *)_p_stFile;
    HANDLE         hFile      = p_stStream->hFile;
    unsigned char *p_ucStub;
    unsigned char *p_ucStubEnd;
    void          *hProcess;
    DWORD          dwBytesRead;
    BOOL           bOk;

    MMG_pc_M_vStepLcg();
    p_ucStub = &MMG_pc_g_uLcgPool.a_ucCode[(int)MMG_g_ulLCGState % MMG_pc_C_PoolSlots];
    MMG_pc_M_vWriteStub(p_ucStub, MMG_pc_C_ImpReadFile, 0x50, 0xC3);
    MMG_pc_M_vFlushPool(hProcess);
    bOk = ((BOOL (__stdcall *)(HANDLE, LPVOID, DWORD, LPDWORD, LPOVERLAPPED))p_ucStub)(
        hFile, _p_vBuffer, (DWORD)_ulBytes, &dwBytesRead, NULL);
    for (p_ucStubEnd = p_ucStub + MMG_pc_C_StubSize; p_ucStub < p_ucStubEnd; p_ucStub++) {
        MMG_pc_M_vStepLcg();
        *p_ucStub = (unsigned char)((int)MMG_g_ulLCGState % 0xff);
    }
    p_stStream->ulValid = (unsigned long)bOk;
    if (dwBytesRead != (DWORD)_ulBytes) {
        return 0;
    }

    for (; p_ucBuffer < p_ucEnd; p_ucBuffer++) {
        uint32_t s;
        uint32_t q;
        *p_ucBuffer ^= p_stStream->uKey.a_ucByte[1];
        s = p_stStream->uKey.ulState ^ 0x075BD924u;
        q = s / 127773u;
        p_stStream->uKey.ulState = s * 16807u - q * 0x7FFFFFFFu;
    }
    return 1;
}

/* 0x00451300 */
unsigned char __cdecl SNA_pc_bRawReadPlainStubbed(void *_p_vBuffer, size_t _ulBytes,
                                                  void *_p_stFile)
{
    SNA_pc_tdstStream *p_stStream = (SNA_pc_tdstStream *)_p_stFile;
    HANDLE         hFile      = p_stStream->hFile;
    unsigned char *p_ucStub;
    unsigned char *p_ucStubEnd;
    void          *hProcess;
    DWORD          dwBytesRead;
    BOOL           bOk;

    MMG_pc_M_vStepLcg();
    p_ucStub = &MMG_pc_g_uLcgPool.a_ucCode[(int)MMG_g_ulLCGState % MMG_pc_C_PoolSlots];
    MMG_pc_M_vWriteStub(p_ucStub, MMG_pc_C_ImpReadFile, 0x50, 0xC3);
    MMG_pc_M_vFlushPool(hProcess);
    bOk = ((BOOL (__stdcall *)(HANDLE, LPVOID, DWORD, LPDWORD, LPOVERLAPPED))p_ucStub)(
        hFile, _p_vBuffer, (DWORD)_ulBytes, &dwBytesRead, NULL);
    for (p_ucStubEnd = p_ucStub + MMG_pc_C_StubSize; p_ucStub < p_ucStubEnd; p_ucStub++) {
        MMG_pc_M_vStepLcg();
        *p_ucStub = (unsigned char)((int)MMG_g_ulLCGState % 0xff);
    }
    p_stStream->ulValid = (unsigned long)bOk;
    return (unsigned char)(dwBytesRead == (DWORD)_ulBytes);
}

#endif /* RAYMAN2_NO_PROTECTION */

/* 0x00450EE0 */
unsigned long __cdecl SNA_pc_ulSeek(void *_p_stFile, long _lDistance, int _iOrigin)
{
    SNA_pc_tdstStream *p_stStream = (SNA_pc_tdstStream *)_p_stFile;
    DWORD          dwMoveMethod;
    long           lDistance = _lDistance;
    unsigned char *p_ucStub;
    unsigned char *p_ucEnd;
    void          *hProcess;
    HANDLE         hFile;
    DWORD          dwResult;

    if (_iOrigin == 1 && SNA_pc_bGetFlagBit1(_p_stFile) != 0) {
        long lCount = abs(lDistance);

        if (lDistance > 0) {
            while (lCount--) {
                p_stStream->uKey.a_ucByte[1] += p_stStream->uKey.a_ucByte[3];
                p_stStream->uKey.a_ucByte[0] += p_stStream->uKey.a_ucByte[2];
            }
        } else {
            while (lCount--) {
                p_stStream->uKey.a_ucByte[0] -= p_stStream->uKey.a_ucByte[2];
                p_stStream->uKey.a_ucByte[1] -= p_stStream->uKey.a_ucByte[3];
            }
        }
    }

    if (_iOrigin == 0) {
        dwMoveMethod = 0;
    } else {
        dwMoveMethod = (DWORD)(_iOrigin == 2) + 1;
    }
#ifndef RAYMAN2_NO_PROTECTION   /* flag bit 2: SetFilePointer through a code-pool stub */
    if (SNA_pc_bGetFlagBit2(_p_stFile) != 0) {

        hFile = p_stStream->hFile;
        MMG_pc_M_vStepLcg();
        p_ucStub = &MMG_pc_g_uLcgPool.a_ucCode[(int)MMG_g_ulLCGState % MMG_pc_C_PoolSlots];
        MMG_pc_M_vWriteStub(p_ucStub, MMG_pc_C_ImpSetFilePointer, 0x50, 0xC3);
        MMG_pc_M_vFlushPool(hProcess);
        dwResult = ((DWORD (__stdcall *)(HANDLE, LONG, PLONG, DWORD))p_ucStub)(
            hFile, _lDistance, NULL, dwMoveMethod);
        for (p_ucEnd = p_ucStub + MMG_pc_C_StubSize; p_ucStub < p_ucEnd; p_ucStub++) {
            MMG_pc_M_vStepLcg();
            *p_ucStub = (unsigned char)((int)MMG_g_ulLCGState % 0xff);
        }
        return dwResult;
    }
#endif /* RAYMAN2_NO_PROTECTION */
    return SetFilePointer(p_stStream->hFile, _lDistance, NULL, dwMoveMethod);
}
