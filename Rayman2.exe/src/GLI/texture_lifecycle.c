#include "GLI/texture_lifecycle.h"
#include "GLI/cluster3_texture_table.h"
#include "GLI/GLI_types.h"

void __cdecl GLI_pc_fn_vUnLoadTexturesViaSlot(void);   /* 0x004269a0 */
void __cdecl GLI_pc_fn_vSetLodOfAllTextures(void);     /* 0x00426940 */
void __cdecl GLI_pc_fn_vDownLoadAllTextures(int bReload); /* 0x00426a00 */
long __cdecl GLI_pc_fn_lReduceTexturesToVramBudget(void); /* 0x0042d270 */

extern long GLI_pc_g_lTexturesMemMode;

/* 0x00426940 */
extern unsigned long GLI_gs_aDEFTableOfTextureMemoryChannels[1024];
extern struct GLI_tdstTexture *GLI_gs_aDEFTableOfTextureAlreadyRead[1024];

unsigned long GLI_g_ul503d58;

void __cdecl GLI_pc_fn_vFree(void **ppUser);
void __cdecl FIL_fn_vFreeGlobalHandleToFileInConcatFile(void);
extern void *GLI_gs_p_ConvertBufferMipMapping;   /* 0x00503d50 */

extern void *GLI_DRV_g_vUnLoadTextures;

unsigned long __cdecl GLI_pc_fn_vTrackMaxTextureBytes(void);
void *__cdecl GLI_pc_fn_p_vAlloc(unsigned long ulSize);
extern long (__cdecl *GLI_g_pfn_SetCommonData)(char const *, void *);

/* 0x00426A00 */
extern void *GLI_DRV_g_vDownLoadTextures;
extern unsigned long GLI_g_ulNumberOfLoadedTexture;
void __cdecl FIL_fn_vInitGlobalHandleToFileInConcatFile(void);
long __cdecl FUN_00424fd0(void);

void __cdecl GLI_pc_fn_vReleaseTextureSlotsOfKey(unsigned long ulKey);

#include "GLI/devinfo.h"
unsigned char __cdecl fn_bMicroGetActivationFlag(void *hTexture);
int __cdecl GLI_pc_fn_lHalveTextureWidth(void *hTexture);
int __cdecl GLI_pc_fn_lHalveTextureHeight(void *hTexture);
int __cdecl GLI_pc_fn_lHalveTextureMipBoth(void *hTexture);

void __cdecl GLI_pc_fn_vUnloadTexturesOfKey(unsigned long ulKey);

void __cdecl GLI_pc_fn_vUnloadTexturesOfKeyWrapper(unsigned long ulKey)
{
    GLI_pc_fn_vUnloadTexturesOfKey(ulKey);
}

void __cdecl GLI_pc_fn_vUnloadTexturesOfKey(unsigned long ulKey)
{
    GLI_pc_fn_vUnLoadTexturesViaSlot();
    GLI_pc_fn_vReleaseTextureSlotsOfKey(ulKey);
}

long __cdecl FUN_00425010(long lSize)
{
    if (lSize > 0x2000)
        return 13;
    if (lSize > 0x1000)
        return 12;
    if (lSize > 0x800)
        return 11;
    if (lSize > 0x400)
        return 10;
    if (lSize > 0x200)
        return 9;
    if (lSize > 0x100)
        return 8;
    if (lSize > 0x80)
        return 7;
    if (lSize > 0x40)
        return 6;
    if (lSize > 0x20)
        return 5;
    if (lSize > 0x10)
        return 4;
    if (lSize > 0x8)
        return 3;
    if (lSize > 0x4)
        return 2;
    return (lSize > 2);
}

void __cdecl GLI_pc_fn_vSetTextureLodFromMinSide(GLI_tdstTexture *p_stTexture)
{

    if (p_stTexture->uwHeight > 1 && p_stTexture->uwWidth > 1) {
        p_stTexture->lNumberOfLod =
            ((FUN_00425010(p_stTexture->uwHeight) < FUN_00425010(p_stTexture->uwWidth))
                 ? FUN_00425010(p_stTexture->uwHeight)
                 : FUN_00425010(p_stTexture->uwWidth)) + 1;
    }
}

void __cdecl GLI_pc_fn_vSetLodOfAllTextures(void)
{
    long lIdx;

    lIdx = 0;
    do {
        if (GLI_gs_aDEFTableOfTextureMemoryChannels[lIdx] != 0xc0de0005 &&
            GLI_gs_aDEFTableOfTextureAlreadyRead[lIdx]->lNumberOfLod != 0) {
            GLI_pc_fn_vSetTextureLodFromMinSide(GLI_gs_aDEFTableOfTextureAlreadyRead[lIdx]);
        }
        lIdx++;
    } while (lIdx < 1024);
}

void __cdecl GLI_vComputeTextures(void)
{
    GLI_pc_fn_vUnLoadTexturesViaSlot();       /* 0x004269a0 */
    FUN_0042c780();
    GLI_pc_fn_lReduceTexturesToVramBudget(); /* 0x0042d270 */
    GLI_pc_fn_vSetLodOfAllTextures();        /* 0x00426940 */
    GLI_pc_fn_vDownLoadAllTextures(0);       /* 0x00426a00 */
}

void __cdecl GLI_pc_fn_vUnLoadTexturesViaSlot(void)
{
    ((void (__cdecl *)(void))GLI_DRV_g_vUnLoadTextures)();
}

void __cdecl GLI_pc_fn_vAllocConvertBufferMipMapping(void)
{
    GLI_g_ul503d58 = GLI_pc_fn_vTrackMaxTextureBytes();
    if (GLI_g_ul503d58 != 0) {
        GLI_gs_p_ConvertBufferMipMapping = GLI_pc_fn_p_vAlloc(GLI_g_ul503d58);
        (*GLI_g_pfn_SetCommonData)("ConvertBufferMipMapping", GLI_gs_p_ConvertBufferMipMapping);
    }
}

void __cdecl FUN_004269e0(void)
{
    if (GLI_g_ul503d58 != 0)
        GLI_pc_fn_vFree(&GLI_gs_p_ConvertBufferMipMapping);
    FIL_fn_vFreeGlobalHandleToFileInConcatFile();
}

void __cdecl GLI_pc_fn_vDownLoadAllTextures(int bReload)
{
    GLI_pc_fn_vAllocConvertBufferMipMapping();
    FIL_fn_vInitGlobalHandleToFileInConcatFile();
    GLI_g_ulNumberOfLoadedTexture = 0;
    ((void (__cdecl *)(int, long, int))GLI_DRV_g_vDownLoadTextures)(0, FUN_00424fd0(), bReload);
    FUN_004269e0();
    FIL_fn_vFreeGlobalHandleToFileInConcatFile();
}

void __cdecl GLI_vReloadTextures(void)
{
    GLI_pc_fn_vDownLoadAllTextures(1);        /* 0x00426a00 */
}

long __cdecl GLI_pc_fn_lReduceTexturesToVramBudget(void)
{
    GLI_tdstTexture *p_stTex;
    long lBudget;
    long lLimit;
    long lDelta;
    long lDelta2;
    long lIdx;
    long lCredit;
    long lRound;
    unsigned long ulThreshold;
    unsigned short usDim;

    lBudget = FUN_0042c6e0();
    ulThreshold = GLI_pc_g_stGliCaps.ulTextureMaxSize;
    lLimit = (long)GLI_pc_g_stGliCaps.ulTotalTextureMemory;
    if ((GLI_pc_g_stGliCaps.ulDriverCaps & GLI_pc_C_ulDrvCapsReserveTextureMemory) != 0)
        lLimit -= 0x8000;

    if ((GLI_pc_g_stGliCaps.ulTextureCaps & E_TEX_SupportNonSquare) == 0) {
        lIdx = 0;
        do {
            if (GLI_gs_aDEFTableOfTextureMemoryChannels[lIdx] != 0xc0de0005) {
                p_stTex = GLI_gs_aDEFTableOfTextureAlreadyRead[lIdx];
                if (p_stTex->uwHeight < p_stTex->uwWidth) {
                    do {
                        lDelta = GLI_pc_fn_lHalveTextureWidth(p_stTex);
                        lBudget -= lDelta;
                        p_stTex = GLI_gs_aDEFTableOfTextureAlreadyRead[lIdx];
                    } while (p_stTex->uwHeight < p_stTex->uwWidth);
                }
                p_stTex = GLI_gs_aDEFTableOfTextureAlreadyRead[lIdx];
                if (p_stTex->uwWidth < p_stTex->uwHeight) {
                    do {
                        lDelta = GLI_pc_fn_lHalveTextureHeight(p_stTex);
                        lBudget -= lDelta;
                        p_stTex = GLI_gs_aDEFTableOfTextureAlreadyRead[lIdx];
                    } while (p_stTex->uwWidth < p_stTex->uwHeight);
                }
                p_stTex = GLI_gs_aDEFTableOfTextureAlreadyRead[lIdx];
                usDim = p_stTex->uwWidth;
                while (ulThreshold < usDim) {
                    lDelta = GLI_pc_fn_lHalveTextureWidth(p_stTex);
                    lDelta2 = GLI_pc_fn_lHalveTextureHeight(GLI_gs_aDEFTableOfTextureAlreadyRead[lIdx]);
                    lBudget = (lBudget - lDelta) - lDelta2;
                    p_stTex = GLI_gs_aDEFTableOfTextureAlreadyRead[lIdx];
                    usDim = p_stTex->uwWidth;
                }
            }
            lIdx++;
        } while (lIdx < 1024);
    } else {
        lIdx = 0;
        do {
            if (GLI_gs_aDEFTableOfTextureMemoryChannels[lIdx] != 0xc0de0005) {
                p_stTex = GLI_gs_aDEFTableOfTextureAlreadyRead[lIdx];
                usDim = p_stTex->uwWidth;
                while (ulThreshold < usDim) {
                    lDelta = GLI_pc_fn_lHalveTextureWidth(p_stTex);
                    lBudget -= lDelta;
                    p_stTex = GLI_gs_aDEFTableOfTextureAlreadyRead[lIdx];
                    usDim = p_stTex->uwWidth;
                }
            }
            lIdx++;
        } while (lIdx < 1024);
        lIdx = 0;
        do {
            if (GLI_gs_aDEFTableOfTextureMemoryChannels[lIdx] != 0xc0de0005) {
                p_stTex = GLI_gs_aDEFTableOfTextureAlreadyRead[lIdx];
                usDim = p_stTex->uwHeight;
                while (ulThreshold < usDim) {
                    lDelta = GLI_pc_fn_lHalveTextureHeight(p_stTex);
                    lBudget -= lDelta;
                    p_stTex = GLI_gs_aDEFTableOfTextureAlreadyRead[lIdx];
                    usDim = p_stTex->uwHeight;
                }
            }
            lIdx++;
        } while (lIdx < 1024);
    }

    if (lLimit != -1) {
        lCredit = 1;
        lRound = 10;
        if (lLimit < lBudget) {
            while (0 < lRound) {
                if (lCredit != 0) {
                    lCredit = lCredit - 1;
                    lIdx = 1023;
                    do {
                        if (lBudget <= lLimit)
                            break;
                        if (GLI_gs_aDEFTableOfTextureMemoryChannels[lIdx] != 0xc0de0005 &&
                            fn_bMicroGetActivationFlag(GLI_gs_aDEFTableOfTextureAlreadyRead[lIdx]) == '\n') {
                            lCredit = lCredit + 1;
                            lDelta = GLI_pc_fn_lHalveTextureMipBoth(GLI_gs_aDEFTableOfTextureAlreadyRead[lIdx]);
                            lBudget -= lDelta;
                            if (lLimit <= lBudget) {
                                lDelta = GLI_pc_fn_lHalveTextureMipBoth(GLI_gs_aDEFTableOfTextureAlreadyRead[lIdx]);
                                lBudget -= lDelta;
                            }
                        }
                        lIdx--;
                    } while (lIdx >= 0);
                }
                lIdx = 0;
                do {
                    if (lBudget <= lLimit)
                        break;
                    if (GLI_gs_aDEFTableOfTextureMemoryChannels[lIdx] != 0xc0de0005 &&
                        fn_bMicroGetActivationFlag(GLI_gs_aDEFTableOfTextureAlreadyRead[lIdx]) != '\n' &&
                        fn_bMicroGetActivationFlag(GLI_gs_aDEFTableOfTextureAlreadyRead[lIdx]) != '\x1e') {
                        lDelta = GLI_pc_fn_lHalveTextureMipBoth(GLI_gs_aDEFTableOfTextureAlreadyRead[lIdx]);
                        lBudget -= lDelta;
                    }
                    lIdx++;
                } while (lIdx < 1024);
                lIdx = 0;
                do {
                    if (lBudget <= lLimit)
                        return 0;
                    if (GLI_gs_aDEFTableOfTextureMemoryChannels[lIdx] != 0xc0de0005 &&
                        fn_bMicroGetActivationFlag(GLI_gs_aDEFTableOfTextureAlreadyRead[lIdx]) == '\x1e') {
                        lDelta = GLI_pc_fn_lHalveTextureMipBoth(GLI_gs_aDEFTableOfTextureAlreadyRead[lIdx]);
                        lBudget -= lDelta;
                    }
                    lIdx++;
                } while (lIdx < 1024);
                lRound = lRound - 1;
                if (lBudget <= lLimit)
                    return 0;
            }
        }
    }
    return 0;
}
