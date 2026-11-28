#include <stddef.h>

#include "GMT/GMT_types.h"
#include "GMT/GmtFunct.h"
#include "ACP/ACP_types.h"
#include "ERM/CpaError.h"

typedef GMT_tdxHandleToGameMaterial (*GMT_tdfn_hVirtualCreate)(void);
typedef void (*GMT_tdfn_vVirtualDestroy)(GMT_tdxHandleToGameMaterial);
typedef void (*GMT_tdfn_vVirtualCopy)(GMT_tdxHandleToGameMaterial, GMT_tdxHandleToGameMaterial);

GMT_tdfn_hVirtualCreate GMT_p_fn_hVirtualCreate; /* 0x004B72B8 */
GMT_tdfn_vVirtualDestroy GMT_p_fn_vVirtualDestroy; /* 0x004B72BC */
GMT_tdfn_vVirtualCopy GMT_p_fn_vVirtualCopy; /* 0x004B72C0 */

/* 0x0042DAF0 */
GMT_tdxHandleToGameMaterial __cdecl GMT_fn_hCreateGameMaterial(void)
{
    if (GMT_p_fn_hVirtualCreate)
        return GMT_p_fn_hVirtualCreate();
    else
        return NULL;
}

/* 0x0042DB00 */
unsigned long __cdecl GMT_fn_b_ulIsValid(GMT_tdxHandleToGameMaterial _hMaterial)
{
    return (_hMaterial != (GMT_tdxHandleToGameMaterial)GMT_C_ulInvalidHandle) &&
           (_hMaterial != NULL);
}

/* 0x0042DB20 */
void __cdecl GMT_fn_vSetVisualMaterial(GMT_tdxHandleToGameMaterial _hGameMat,
                                       ACP_tdxHandleOfMaterial _hVisualMat)
{
    GMT_M_vSetVisualMaterial(_hGameMat, _hVisualMat);
}

/* 0x0042DB30 */
GMT_tdxHandleToCollideMaterial __cdecl GMT_fn_hGetCollideMaterial(GMT_tdxHandleToGameMaterial _hGameMat)
{
    return GMT_M_hGetCollideMaterial(_hGameMat);
}

/* 0x0042DB40 */
ACP_tdxBool __cdecl GMT_fn_xTestCollideIdentifierTypeOfGameMaterial(GMT_tdxHandleToGameMaterial _hGameMat,
                                                                    unsigned short _uwType)
{
    GMT_tdxHandleToCollideMaterial hCollideMat;

    hCollideMat = GMT_fn_hGetCollideMaterial(_hGameMat);
    if (hCollideMat == (GMT_tdxHandleToCollideMaterial)GMT_C_InvalidCollideMaterial)
        return FALSE;
    return GMT_fn_xTestCollideMaterialTypeOfIdentifier(hCollideMat, _uwType);
}

/* 0x0042DB70 */
ACP_tdxBool __cdecl GMT_fn_xMultipleTestCollideIdentifierFromGameMaterial(GMT_tdxHandleToGameMaterial _hGameMat,
                                                                          GMT_tdxMask _xType)
{
    GMT_tdxHandleToCollideMaterial hCollideMat;

    if (_xType == 0)
        return TRUE;

    hCollideMat = GMT_fn_hGetCollideMaterial(_hGameMat);
    if (!hCollideMat || hCollideMat == GMT_C_InvalidCollideMaterial)
        return FALSE;
    return GMT_fn_xMultipleTestCollideIdentifier(hCollideMat, _xType);
}

GMT_tdxMask __cdecl GMT_fn_hGetCollideMaterialIdentifier(GMT_tdxHandleToCollideMaterial _hCollideMat);

/* 0x0042DE00 */
ACP_tdxBool __cdecl GMT_fn_xMultipleTestCollideIdentifier(GMT_tdxHandleToCollideMaterial _hCollideMat,
                                                          GMT_tdxMask _xType)
{
    GMT_tdxMask xIdentifier;

    xIdentifier = GMT_fn_hGetCollideMaterialIdentifier(_hCollideMat);
    xIdentifier = (GMT_tdxMask)(xIdentifier & _xType);

    if (xIdentifier == _xType)
        return TRUE;
    else
        return FALSE;
}

/* 0x00424770 */
extern void __cdecl GLI_xDuplicateMaterial(void **p_hDest, void *hSource);

#include "MMG/MMG_alloc.h"
#include "MMG/MMG_pubaccess.h"
#include "GEO/GEO_globals.h"
#include "GEO/GeoMem.h"

#define ERM_fn_ucInitErrMsg_call     Erm_fn_ucInitErrMsg
#define ERM_fn_uwCheckError_call     Erm_fn_uwCheckError
#define ERM_fn_vClearLastError_call  ERM_fn_vClearLastError
#define ERM_fn_vUpdateLastError_call ERM_fn_vUpdateLastError

#define gmt_pal_ch        g_ucGEOModuleId        /* 0x0049FBBC */
#define gmt_pal_mode      g_ucGEOMMemMallocMode  /* 0x0049FBC0 */
#define gmt_pal_dbg_flag  GEO_g_bDynamicAllocation  /* 0x00501524 */

#define gmt_pal_bounds_lo (MMG_g_p_GmtPalBlockInfo->p_cBeginBlock)
#define gmt_pal_bounds_hi (MMG_g_p_GmtPalBlockInfo->p_cEndBlock)
#define gmt_erm_diag_flag MMG_g_ucErmChannelId   /* 0x004A11C4 */

/* 0x004A0CBC */
unsigned char g_ucGmtModuleId = 0xFF;

/* 0x0042DBB0 */
extern void __cdecl DNM_fn_vInvalidateMatCharacteristics(DNM_tdxHandleToMecMatCharacteristics *_p_hMatCharacteristics);

GMT_tdxHandleToGameMaterial __cdecl GMT_fn_hCreateFor3DOS(void)
{
    GMT_tdxHandleToGameMaterial hNewGameMaterial;

    GEO_M_CPAMalloc(hNewGameMaterial, GMT_tdxHandleToGameMaterial, sizeof(struct GMT_tdstGameMaterial_), E_uwGEONotEnoughtMemory);

    if (hNewGameMaterial != NULL)
    {

        DNM_fn_vInvalidateMatCharacteristics(&(hNewGameMaterial->hMechanicsMaterial));
        hNewGameMaterial->hVisualMaterial = (ACP_tdxHandleOfMaterial)NULL;
        hNewGameMaterial->hSoundMaterial = (SND_tdxHandleToSoundMaterial)GMT_C_InvalidSoundMaterial;
        hNewGameMaterial->hCollideMaterial = (GMT_tdxHandleToCollideMaterial)GMT_C_InvalidCollideMaterial;
        return hNewGameMaterial;
    }
    return NULL;
}

/* 0x0042DAA0 */
void __cdecl GMT_fn_vInitFor3DOS(void)
{
    GMT_p_fn_hVirtualCreate = &GMT_fn_hCreateFor3DOS;
    GMT_p_fn_vVirtualCopy   = &GMT_fn_vCopyFor3DOS;

    GMT_p_fn_vVirtualDestroy = &GMT_fn_vDestroyFor3DOS;
    GMT_fn_vInitGameMaterialErrorAndMemory();
}

/* 0x0042DAD0 */
void __cdecl GMT_fn_vInitGameMaterialErrorAndMemory(void)
{
    if (g_ucGmtModuleId == C_ucModuleNotInitialized)
        g_ucGmtModuleId = ERM_fn_ucInitErrMsg_call();
}

/* 0x00419B90 */
ACP_tdxHandleOfMaterial __cdecl GMT_fn_hGetVisualMaterial(GMT_tdxHandleToGameMaterial _hGameMat)
{
    return GMT_M_hGetVisualMaterial(_hGameMat);
}

/* 0x0042DE30 */
DNM_tdxHandleToMecMatCharacteristics __cdecl GMT_fn_hGetMechanicsMaterial(GMT_tdxHandleToGameMaterial _hGameMat)
{
    return GMT_M_hGetMechanicsMaterial(_hGameMat);
}

/* 0x004194B0 */
SND_tdxHandleToSoundMaterial __cdecl GMT_fn_hGetSoundMaterial(GMT_tdxHandleToGameMaterial _hGameMat)
{
    return GMT_M_hGetSoundMaterial(_hGameMat);
}

/* 0x0042DCD0 */
void __cdecl GMT_fn_vCopyFor3DOS(GMT_tdxHandleToGameMaterial _hDst,
                                  GMT_tdxHandleToGameMaterial _hSrc)
{
    ACP_tdxHandleOfMaterial hVisual;
    ACP_tdxHandleOfMaterial hCopyOfVisual;

    hVisual = GMT_fn_hGetVisualMaterial(_hSrc);
    GLI_xDuplicateMaterial((void **)&hCopyOfVisual, (void *)hVisual);
    GMT_M_vSetVisualMaterial(_hDst, hCopyOfVisual);
    GMT_M_vSetSoundMaterial(_hDst, GMT_M_hGetSoundMaterial(_hSrc));
    GMT_M_vSetCollideMaterial(_hDst, GMT_M_hGetCollideMaterial(_hSrc));
    GMT_M_vSetMechanicsMaterial(_hDst, GMT_M_hGetMechanicsMaterial(_hSrc));
}

/* 0x0042DD10 */
void __cdecl GMT_fn_vDestroyFor3DOS(GMT_tdxHandleToGameMaterial _hOld)
{

    Mmg_fn_v_SetModeAlloc(gmt_pal_ch, gmt_pal_mode, 0);

    if (gmt_pal_dbg_flag != 0)
    {
        Mmg_fn_vGiveInformationBlock(gmt_pal_ch, gmt_pal_mode, (void *)MMG_g_p_GmtPalBlockInfo);

        if ((unsigned long)_hOld <  (unsigned long)gmt_pal_bounds_lo ||
            (unsigned long)_hOld >  (unsigned long)gmt_pal_bounds_hi)
        {
            Mmg_fn_v_SetModeAlloc(gmt_pal_ch, 0xff, 0);
        }
    }

    fn_vGenFree((void *)_hOld, 0);

    if (ERM_fn_uwCheckError_call(gmt_erm_diag_flag, 0) != 0)
    {
        ERM_fn_vClearLastError_call(0);
        ERM_fn_vUpdateLastError_call(  4,
                                       gmt_pal_ch,
                                       0,
                                       5,
                                       (long)C_lErmNoDebugData,
                                       0xff,
                                       0xff,
                                       (char *)0);
    }
}

/* 0x0042DDD0 */
GMT_tdxMask __cdecl GMT_fn_hGetCollideMaterialIdentifier(GMT_tdxHandleToCollideMaterial _hCollideMat)
{
    return GMT_M_wGetCollideMaterialIdentifier(_hCollideMat);
}

/* 0x0042DDE0 */
ACP_tdxBool __cdecl GMT_fn_xTestCollideMaterialTypeOfIdentifier(GMT_tdxHandleToCollideMaterial _hCollideMat,
                                                                unsigned short _uwType)
{
    unsigned short uwMask;
    unsigned short uwIdentifier;

    uwMask = 0x0001;
    uwMask = (unsigned short)(uwMask << _uwType);

    uwIdentifier = (unsigned short)GMT_fn_hGetCollideMaterialIdentifier(_hCollideMat);
    uwMask = (unsigned short)(uwMask & uwIdentifier);

    if (uwMask == 0)
        return FALSE;
    else
        return TRUE;
}

/* 0x0042DE20 */
void __cdecl GMT_fn_vResetEntireMask(GMT_tdxMask *_p_xMask)
{
    *_p_xMask = 0x0000;
}

/* 0x0042DE40 */
void __cdecl GMT_fn_vSetMechanicsMaterial(GMT_tdxHandleToGameMaterial _hGameMat,
                                          DNM_tdxHandleToMecMatCharacteristics _hMechMat)
{
    GMT_M_vSetMechanicsMaterial(_hGameMat, _hMechMat);
}

#include "DNM/DNM_types.h"
unsigned short __cdecl fn_xMaskOfObstacle(DNM_tdstObstacle *_p_stObstacle)
{
    if (_p_stObstacle != (void *)0)
    {
        GMT_tdxHandleToGameMaterial hGameMaterial = DNM_M_hObstacleGetCollidedMaterial(_p_stObstacle);
        if (GMT_fn_b_ulIsValid(hGameMaterial))
        {
            GMT_tdxHandleToCollideMaterial hMaterialCollide = GMT_fn_hGetCollideMaterial(hGameMaterial);
            if (hMaterialCollide != 0 && hMaterialCollide != (GMT_tdxHandleToCollideMaterial)-1)
                return GMT_fn_hGetCollideMaterialIdentifier(hMaterialCollide);
        }
    }
    return 0;
}
