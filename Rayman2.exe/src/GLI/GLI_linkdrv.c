/*
 =======================================================================================
  Name        : GLI_linkdrv.c
  Description : Renderer drivers linked into the exe (USE_GLI_DX6)

  GLI_fnl_InitDll loads the driver DLL that ubi.ini names (GLI_DllFile) and binds its
  GLI_DRV entry points by name. A driver linked into the exe answers the same names from
  a table, so the binding code and the driver interface stay as they are; a driver that
  is not linked in is still loaded from the driver folder. Linked in: the Direct3D 6
  driver (GliDX6vf.dll sources).

  This file sees the drivers' own declarations, not the engine's GLI headers: there the
  GLI_DRV_* names stand for the engine's function pointers.
 =======================================================================================
 */
#ifdef USE_GLI_DX6

#include "GliDX6vf/GliDX6vf.h"
#include "GLI/GLI_linkdrv.h"
#include <string.h>

/* One entry point of a linked-in driver */
typedef struct GLI_tdstLinkedProc_
{
    const char *szName;
    FARPROC     pfnProc;
} GLI_tdstLinkedProc;

/* A linked-in driver: the file name it replaces, what its DllMain does, its entry points */
typedef struct GLI_tdstLinkedDriver_
{
    const char                  *szFileName;
    void                        (__cdecl *p_fn_vAttach)(void);
    const GLI_tdstLinkedProc *d_stProcs;
} GLI_tdstLinkedDriver;

#define GLI_M_LinkedProc(Name)  { #Name, (FARPROC)Name }

static const GLI_tdstLinkedProc gs_a_stDX6Procs[] =
{
    GLI_M_LinkedProc(GLI_DRV_bBeginScene),
    GLI_M_LinkedProc(GLI_DRV_bEndScene),
    GLI_M_LinkedProc(GLI_DRV_bLockDevice),
    GLI_M_LinkedProc(GLI_DRV_bPrepareForGliWindowed),
    GLI_M_LinkedProc(GLI_DRV_bUnlockDevice),
    GLI_M_LinkedProc(GLI_DRV_bWindowedModeIsOptimized),
    GLI_M_LinkedProc(GLI_DRV_fn_lGetAllDisplayConfig),
    GLI_M_LinkedProc(GLI_DRV_fnl_EnumModes),
    GLI_M_LinkedProc(GLI_DRV_hChangeMode),
    GLI_M_LinkedProc(GLI_DRV_lGetDllInfo),
    GLI_M_LinkedProc(GLI_DRV_lGetSizeOfTexture),
    GLI_M_LinkedProc(GLI_DRV_lSetCommonData),
    GLI_M_LinkedProc(GLI_DRV_lSetCommonFct),
    GLI_M_LinkedProc(GLI_DRV_vActivateGli),
    GLI_M_LinkedProc(GLI_DRV_vAddBlackPolygon),
    GLI_M_LinkedProc(GLI_DRV_vClearDevice),
    GLI_M_LinkedProc(GLI_DRV_vClearZBufferRegion),
    GLI_M_LinkedProc(GLI_DRV_vCloseDriver),
    GLI_M_LinkedProc(GLI_DRV_vComputeFogEffect),
    GLI_M_LinkedProc(GLI_DRV_vDoOpaqueTextureSelection),
    GLI_M_LinkedProc(GLI_DRV_vDownLoadTextures),
    GLI_M_LinkedProc(GLI_DRV_vFlipDevice),
    GLI_M_LinkedProc(GLI_DRV_vFlipDeviceWithSyncro),
    GLI_M_LinkedProc(GLI_DRV_vGetStats),
    GLI_M_LinkedProc(GLI_DRV_vNoBlackPolygon),
    GLI_M_LinkedProc(GLI_DRV_vNonOptimizedWindowedMode),
    GLI_M_LinkedProc(GLI_DRV_vOptimizedWindowedMode),
    GLI_M_LinkedProc(GLI_DRV_vPrepareForGliFullScreen),
    GLI_M_LinkedProc(GLI_DRV_vReadaptDisplay),
    GLI_M_LinkedProc(GLI_DRV_vSendSingleLineToClip),
    GLI_M_LinkedProc(GLI_DRV_vSendSpriteToClip),
    GLI_M_LinkedProc(GLI_DRV_vSendSpriteToClipWithColors),
    GLI_M_LinkedProc(GLI_DRV_vSendSpriteToClipWithUV),
    GLI_M_LinkedProc(GLI_DRV_vSetClipWindow),
    GLI_M_LinkedProc(GLI_DRV_vSetZClip),
    GLI_M_LinkedProc(GLI_DRV_vUnLoadTextures),
    GLI_M_LinkedProc(GLI_DRV_vWrite16bBitmapToBackBuffer),
    GLI_M_LinkedProc(GLI_DRV_xClearViewingList),
    GLI_M_LinkedProc(GLI_DRV_xInitDriver),
    GLI_M_LinkedProc(GLI_DRV_xSendElementTIToClip_TRIANGLES),
    GLI_M_LinkedProc(GLI_DRV_xSendListToViewport),
    GLI_M_LinkedProc(GLI_DRV_xSendSingleTriangleToClip_TRIANGLES),
    { NULL, NULL }
};

#undef GLI_M_LinkedProc

/* File names as GLI_fnv_SetDllFileName builds them ("dll/%s%s.dll") */
static const GLI_tdstLinkedDriver gs_a_stLinkedDrivers[] =
{
    { "dll/GliDX6vf.dll", GLI_vInitDllInfo, gs_a_stDX6Procs }
};

#define C_lNbLinkedDrivers  ((long)(sizeof(gs_a_stLinkedDrivers) / sizeof(gs_a_stLinkedDrivers[0])))

/* The driver GLI_fn_hLoadDriver returned last when it is linked in, else NULL */
static const GLI_tdstLinkedDriver *gs_p_stLinkedDriver = NULL;

/*
 ----------------------------------------------------------------------------------------
  Description   : LoadLibraryA for the renderer driver. A linked-in driver gets the exe's
                  own module handle (it lives there) and its DllMain work done once.
 ----------------------------------------------------------------------------------------
 */
HMODULE __stdcall GLI_fn_hLoadDriver(const char *szFileName)
{
    long lDriver;

    gs_p_stLinkedDriver = NULL;
    for ( lDriver = 0 ; lDriver < C_lNbLinkedDrivers ; lDriver++ )
    {
        if ( _stricmp(szFileName, gs_a_stLinkedDrivers[lDriver].szFileName) == 0 )
        {
            gs_p_stLinkedDriver = &gs_a_stLinkedDrivers[lDriver];
            gs_p_stLinkedDriver->p_fn_vAttach();
            return GetModuleHandleA(NULL);
        }
    }
    return LoadLibraryA(szFileName);
}

/*
 ----------------------------------------------------------------------------------------
  Description   : GetProcAddress for the driver GLI_fn_hLoadDriver returned last.
                  A name the linked-in driver does not have gives NULL, as for a DLL.
 ----------------------------------------------------------------------------------------
 */
FARPROC __stdcall GLI_fn_pfnGetDriverProc(HMODULE hDriver, const char *szName)
{
    const GLI_tdstLinkedProc *p_stProc;

    if ( gs_p_stLinkedDriver == NULL )
        return GetProcAddress(hDriver, szName);

    for ( p_stProc = gs_p_stLinkedDriver->d_stProcs ; p_stProc->szName != NULL ; p_stProc++ )
    {
        if ( strcmp(szName, p_stProc->szName) == 0 )
            return p_stProc->pfnProc;
    }
    return NULL;
}

#endif /* USE_GLI_DX6 */
