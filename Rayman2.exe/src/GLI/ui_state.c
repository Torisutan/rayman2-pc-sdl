#include "GLI/ui_state.h"
#include "GLI/GLI_globals.h"
#include <windows.h>
#include "GAM/GAM_pc_DrawLock.h"
#include "TMP/TmpMem.h"
#include "ERM/CpaError.h"
#include "FIL/File.h"
/* 0x00420a80 */
extern long __cdecl GLD_bGetViewportAttributes(short hNotUsed, short hVP, void *pstViewAttrib);
#include <string.h>
#include "GLI/ui_reset_redraw.h"
extern long __cdecl GLD_bFlipDeviceWithSynchro(short hDev_unused, int eSynchro_unused); /* 0x00420f50 */
extern void __cdecl GLI_pc_fn_vWrite16bBitmapToBackBuffer(void *p_vBuffer, long lPitch, long lHeight,
                                                          long lArg4, long lArg5, long lArg6,
                                                          long lArg7); /* 0x00421fd0 */

#define GLI_pc_g_lOverlayBufferSize  (GLI_pc_g_stProgressUi.lBufferSize)
#define GLI_pc_g_p_vOverlayBuffer    (GLI_pc_g_stProgressUi.p_vBuffer)
#define GLI_pc_g_lOverlayBufferDimC  (GLI_pc_g_stProgressUi.lBufferDimC)
#define GLI_pc_g_lOverlayBufferDim8  (GLI_pc_g_stProgressUi.lBufferDim8)

void __cdecl GLI_pc_fn_vShowProgressUiImage(char *p_stUiObject);

void __cdecl FUN_0045ec20(void)
{
    GLI_pc_g_ucRandomPickEnabled = 0;

    if (GLI_pc_g_ucErrorChannelId == 0xff) {
        GLI_pc_g_ucErrorChannelId = Erm_fn_ucInitErrMsg();
    }
}

void __cdecl FUN_0045ec40(void)
{

    unsigned long a_ulRect[28];
    unsigned long ulSize;

    GAM_pc_M_ulWaitDrawSemaphore((HANDLE)GLI_pc_g_hOverlaySemaphore);

    FIL_fn_vInitGlobalHandleToFileInConcatFile();

    GLD_bGetViewportAttributes(GLI_pc_g_uwOverlayDevice,
                               GLI_pc_g_uwOverlayViewport,
                               a_ulRect);

    ulSize = a_ulRect[3] * a_ulRect[2] * 2;
    GLI_pc_g_lOverlayBufferSize = (long)ulSize;
    GLI_pc_g_p_vOverlayBuffer   = TMP_fnp_vMalloc(ulSize);
    GLI_pc_g_lOverlayBufferDimC = (long)a_ulRect[3];
    GLI_pc_g_lOverlayBufferDim8 = (long)a_ulRect[2];

    GAM_pc_M_bReleaseDrawSemaphore((HANDLE)GLI_pc_g_hOverlaySemaphore);
}

void __cdecl FUN_0045edc0(unsigned short p_uArg1, unsigned short p_uArg2, void *h_pGdiHandle)
{
    GLI_pc_g_uwOverlayDevice   = p_uArg1;
    GLI_pc_g_uwOverlayViewport = p_uArg2;
    GLI_pc_g_hOverlaySemaphore = h_pGdiHandle;
}

void __cdecl FUN_0045edf0(void)
{
    if (GLI_pc_g_p_vOverlayBuffer != (void *)0) {
        GLI_pc_fn_vShowProgressUiImage((char *)&GLI_pc_g_stProgressUi);
    }
}

/* 0x0045ee10 */
void __cdecl GLI_pc_fn_vShowProgressUiImage(char *p_stUiObject)
{
    unsigned long a_ulAttrib[28];

    if (_stricmp(p_stUiObject, GLI_pc_g_stProgressUi.szName) != 0) {
        FUN_0045fbf0(p_stUiObject, p_stUiObject, ((GLI_pc_tdstProgressUi *)p_stUiObject)->lFormatKind);
    }
    GAM_pc_M_ulWaitDrawSemaphore((HANDLE)GLI_pc_g_hOverlaySemaphore);
    GLD_bGetViewportAttributes(GLI_pc_g_uwOverlayDevice, GLI_pc_g_uwOverlayViewport, a_ulAttrib);
    GLI_pc_fn_vWrite16bBitmapToBackBuffer(((GLI_pc_tdstProgressUi *)p_stUiObject)->p_vBuffer,
                                          ((GLI_pc_tdstProgressUi *)p_stUiObject)->lBufferDimC,
                                          ((GLI_pc_tdstProgressUi *)p_stUiObject)->lBufferDim8,
                                          a_ulAttrib[10], a_ulAttrib[8],
                                          a_ulAttrib[11], a_ulAttrib[9]);
    GLD_bFlipDeviceWithSynchro(GLI_pc_g_uwOverlayDevice, 0xff);
    GAM_pc_M_bReleaseDrawSemaphore((HANDLE)GLI_pc_g_hOverlaySemaphore);
}
