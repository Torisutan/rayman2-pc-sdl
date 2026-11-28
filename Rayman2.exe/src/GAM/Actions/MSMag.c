#define D_MSMagnet_StructureDefine
#include "GAM/MSMag.h"
#include "SAI/SAI.h"
#include "GAM/GAM_types.h"
#include "GAM/GAM_globals.h"
#include "ERM/CpaError.h"

#define C_Magnet_SAIInitFlags (SAI_eInitWhenPlayerDead|SAI_eInitWhenReinitTheMap|SAI_eInitWhenMapLoaded)

#define E_uwGameMiniStructObjectNull        ((unsigned short)0xE)
#define E_uwGameMiniStructNull              ((unsigned short)0xF)
extern void __cdecl fn_vGameFreeInHLM(void *p_vObject);

unsigned long __cdecl fn_ulMSMagnetSizeOf(void)
{
    return (unsigned long)0x78;
}

void __cdecl fn_vMSMagnetInitValueSAI(MS_tdxHandleToMSMagnet _h_MSMagnet);

#define E_uwGameMiniStructAllocAlreadyAlloc ((unsigned short)0xD)

void __cdecl fn_vMSMagnetSavedValueSAI(void *_h_MSMagnet)
{
    (void)_h_MSMagnet;
}

#define OTI_MapLoaded       1
#define OTI_ReinitTheMap    2
#define OTI_LoadSavedGame   3
#define OTI_PlayerDead      4
#define MAGNETOFF           0

void * __cdecl fn_h_MSMagnetRealAlloc(void);

void __cdecl fn_vMSMagnetAlloc(struct tdstEngineObject_ *p_stObject)
{
    if (p_stObject == NULL)
    {
        ERM_fn_vClearLastError(0);
        ERM_fn_vUpdateLastError(0x30, g_ucGameModuleId, 0,
                                E_uwGameMiniStructObjectNull, -1, 0xff, 0xff, NULL);
    }

    if (p_stObject->h_MSMagnet != NULL)
    {
        ERM_fn_vClearLastError(0);
        ERM_fn_vUpdateLastError(0x30, g_ucGameModuleId, 0,
                                E_uwGameMiniStructAllocAlreadyAlloc, -1, 0xff, 0xff, NULL);
    }

    p_stObject->h_MSMagnet = fn_h_MSMagnetRealAlloc();
    ((MS_tdxHandleToMSMagnet)p_stObject->h_MSMagnet)->ulDuration = 0;
    ((MS_tdxHandleToMSMagnet)p_stObject->h_MSMagnet)->status = MAGNETOFF;
    ((MS_tdxHandleToMSMagnet)p_stObject->h_MSMagnet)->xMaxIndexOfMagnetModification = 0;

    ((MS_tdxHandleToMSMagnet)p_stObject->h_MSMagnet)->ucModifiedFlags = 0;
    fn_vMSMagnetInitValueSAI((MS_tdxHandleToMSMagnet)p_stObject->h_MSMagnet);
}

void * __cdecl fn_h_MSMagnetRealAlloc(void)
{
    void *h_MSMagnet;
    h_MSMagnet = (void *)fnp_vGameMallocInHLM((unsigned int)fn_ulMSMagnetSizeOf());
    return(h_MSMagnet);
}

void __cdecl fn_vMSMagnetFree(struct tdstEngineObject_ *p_stObject)
{
    if (p_stObject == NULL)
    {
        ERM_fn_vClearLastError(0);
        ERM_fn_vUpdateLastError(0x30, g_ucGameModuleId, 0,
                                E_uwGameMiniStructObjectNull, -1, 0xff, 0xff, NULL);
    }

    if (p_stObject->h_MSMagnet == NULL)
    {
        ERM_fn_vClearLastError(0);
        ERM_fn_vUpdateLastError(0x30, g_ucGameModuleId, 0,
                                E_uwGameMiniStructNull, -1, 0xff, 0xff, NULL);
    }

    fn_vMSMagnetFreeInitValueSAI(p_stObject->h_MSMagnet);
    fn_vGameFreeInHLM(p_stObject->h_MSMagnet);
    p_stObject->h_MSMagnet = NULL;
}

void __cdecl fn_vMSMagnetSetStatus(MS_tdxHandleToMSMagnet h_MSMagnet, unsigned char xValue);

void __cdecl fn_vMSMagnetInit(struct tdstEngineObject_ *p_stObject, int eObjectInit)
{
    if (p_stObject == NULL)
    {
        ERM_fn_vClearLastError(0);
        ERM_fn_vUpdateLastError(0x30, g_ucGameModuleId, 0,
                                E_uwGameMiniStructObjectNull, -1, 0xff, 0xff, NULL);
    }

    if (p_stObject->h_MSMagnet == NULL)
    {
        ERM_fn_vClearLastError(0);
        ERM_fn_vUpdateLastError(0x30, g_ucGameModuleId, 0,
                                E_uwGameMiniStructNull, -1, 0xff, 0xff, NULL);
    }

    switch (eObjectInit)
    {
    case OTI_ReinitTheMap:
        fn_vMSMagnetSetStatus(p_stObject->h_MSMagnet, MAGNETOFF);
        break;
    case OTI_LoadSavedGame:
        fn_vMSMagnetSetStatus(p_stObject->h_MSMagnet, MAGNETOFF);
        break;
    case OTI_MapLoaded:
        fn_vMSMagnetSetStatus(p_stObject->h_MSMagnet, MAGNETOFF);
        fn_vMSMagnetInitValueSAI(p_stObject->h_MSMagnet);
        fn_vMSMagnetSavedValueSAI(p_stObject->h_MSMagnet);
        break;
    case OTI_PlayerDead:
        fn_vMSMagnetSetStatus(p_stObject->h_MSMagnet, MAGNETOFF);
        break;
    default:
        break;
    }
}

void __cdecl fn_vMSMagnetDesinit(struct tdstEngineObject_ *p_stObject, int eObjectInit)
{
    if (p_stObject == NULL)
    {
        ERM_fn_vClearLastError(0);
        ERM_fn_vUpdateLastError(0x30, g_ucGameModuleId, 0,
                                E_uwGameMiniStructObjectNull, -1, 0xff, 0xff, NULL);
    }

    if (p_stObject->h_MSMagnet == NULL)
    {
        ERM_fn_vClearLastError(0);
        ERM_fn_vUpdateLastError(0x30, g_ucGameModuleId, 0,
                                E_uwGameMiniStructNull, -1, 0xff, 0xff, NULL);
    }
}

void __cdecl fn_vMSMagnetCopyClone(struct tdstEngineObject_ *_p_stObjectDest, struct tdstEngineObject_ *_p_stObjectSrc)
{
    memcpy(_p_stObjectDest->h_MSMagnet, _p_stObjectSrc->h_MSMagnet, fn_ulMSMagnetSizeOf());
}

/* 0x00415160 */
float __cdecl fn_xMSMagnetGetFar(MS_tdxHandleToMSMagnet h_MSMagnet)
{
    return((h_MSMagnet->xFar > h_MSMagnet->xNear)?h_MSMagnet->xFar:h_MSMagnet->xNear);
}

void __cdecl fn_vMSMagnetSetNearFarModification(MS_tdxHandleToMSMagnet h_MSMagnet, unsigned char _bModified);

/* 0x00415180 */
void __cdecl fn_vMSMagnetSetFar(MS_tdxHandleToMSMagnet h_MSMagnet, float xValue)
{
    h_MSMagnet->xFar = xValue;
    fn_vMSMagnetSetNearFarModification(h_MSMagnet,TRUE);
}

/* 0x004151A0 */
float __cdecl fn_xMSMagnetGetNear(MS_tdxHandleToMSMagnet h_MSMagnet)
{
    return((h_MSMagnet->xFar < h_MSMagnet->xNear)?h_MSMagnet->xFar:h_MSMagnet->xNear);
}

/* 0x004151C0 */
void __cdecl fn_vMSMagnetSetNear(MS_tdxHandleToMSMagnet h_MSMagnet, float xValue)
{
    h_MSMagnet->xNear = xValue;
    fn_vMSMagnetSetNearFarModification(h_MSMagnet,TRUE);
}

/* 0x004151E0 */
float __cdecl fn_xMSMagnetGetStrength(MS_tdxHandleToMSMagnet h_MSMagnet)
{
    return(h_MSMagnet->xStrength);
}

void __cdecl fn_vMSMagnetSetStrengthModification(MS_tdxHandleToMSMagnet h_MSMagnet, unsigned char _bModified);

/* 0x004151F0 */
void __cdecl fn_vMSMagnetSetStrength(MS_tdxHandleToMSMagnet h_MSMagnet, float xValue)
{
    h_MSMagnet->xStrength = xValue;
    fn_vMSMagnetSetStrengthModification(h_MSMagnet,TRUE);
}

/* 0x00415210 */
unsigned char __cdecl fn_lMSMagnetGetStatus(MS_tdxHandleToMSMagnet h_MSMagnet)
{
    return(h_MSMagnet->status);
}

/* 0x00415220 */
void __cdecl fn_vMSMagnetSetStatus(MS_tdxHandleToMSMagnet h_MSMagnet, unsigned char xValue)
{
    h_MSMagnet->status = xValue;
}

/* 0x00415230 */
void __cdecl fn_vMSMagnetAddMagnetModification(MS_tdxHandleToMSMagnet h_MSMagnet, void *h_MagnetModif)
{
    ACP_tdxIndex xIndex;
    MGT_tdxHandleToMagnetModification *p_h_MagnetModif;

    for( xIndex = 0 , p_h_MagnetModif = h_MSMagnet -> d_hMagnetModification ;
         xIndex < h_MSMagnet -> xMaxIndexOfMagnetModification ;
         xIndex++, p_h_MagnetModif++ )
    {
        if( (*p_h_MagnetModif) == NULL )
        {
            break;
        }
    }
    if( xIndex < MGT_MAX_NUMBEROF_MODIFIEDOBJECTS )
    {
        (*p_h_MagnetModif) = h_MagnetModif;
        if( xIndex == h_MSMagnet -> xMaxIndexOfMagnetModification )
            h_MSMagnet -> xMaxIndexOfMagnetModification++;
    }
}

/* 0x00415270 */
void __cdecl fn_vMSMagnetDeleteMagnetModification(MS_tdxHandleToMSMagnet h_MSMagnet, void *h_MagnetModif)
{
    ACP_tdxIndex xIndex;

    for( xIndex = 0 ; xIndex < h_MSMagnet -> xMaxIndexOfMagnetModification ; xIndex++ )
    {
        if( h_MSMagnet -> d_hMagnetModification[ xIndex ] == h_MagnetModif )
        {
            h_MSMagnet -> d_hMagnetModification[ xIndex ] = NULL;
            if( xIndex == (h_MSMagnet -> xMaxIndexOfMagnetModification-1) )
                h_MSMagnet -> xMaxIndexOfMagnetModification--;
            break;
        }
    }
}
/* 0x004152C0 */
MGT_tdstMagnetModification *fn_hMSMagnetGetMagnetModification(MS_tdxHandleToMSMagnet h_MSMagnet, ACP_tdxIndex xIndex )
{
	return h_MSMagnet -> d_hMagnetModification[ xIndex ];
}
/* 0x004152D0 */
MTH3D_tdstVector * fn_hMSMagnetGetPosition(MS_tdxHandleToMSMagnet h_MSMagnet)
{
	return(&(h_MSMagnet->stPosition));
}
/* 0x004152E0 */
void fn_vMSMagnetSetPosition(MS_tdxHandleToMSMagnet h_MSMagnet,MTH3D_tdstVector *stPosition)
{
	MTH3D_M_vCopyVector(&(h_MSMagnet->stPosition), stPosition);
}

/* 0x00415300 */
unsigned long __cdecl fn_ulMSMagnetGetDuration(MS_tdxHandleToMSMagnet h_MSMagnet)
{
    return(h_MSMagnet->ulDuration);
}

/* 0x00415310 */
void __cdecl fn_vMSMagnetSetDuration(MS_tdxHandleToMSMagnet h_MSMagnet, unsigned long ulDuration)
{
    h_MSMagnet->ulDuration = ulDuration;
}

/* 0x00415320 */
void __cdecl fn_vMSMagnetSetNearFarModification(MS_tdxHandleToMSMagnet h_MSMagnet, unsigned char _bModified)
{

    if( _bModified )
        h_MSMagnet->ucModifiedFlags |= MAGNET_NEARFAR_MODIFIED;
    else
        h_MSMagnet->ucModifiedFlags &= ~MAGNET_NEARFAR_MODIFIED;
}

/* 0x00415340 */
ACP_tdxBool fn_bMSMagnetIsNearFarModified(MS_tdxHandleToMSMagnet h_MSMagnet)
{

	return (unsigned char)( h_MSMagnet->ucModifiedFlags & MAGNET_NEARFAR_MODIFIED ? TRUE : FALSE );
}

/* 0x00415350 */
void __cdecl fn_vMSMagnetSetStrengthModification(MS_tdxHandleToMSMagnet h_MSMagnet, unsigned char _bModified)
{

    if( _bModified )
        h_MSMagnet->ucModifiedFlags |= MAGNET_STRENGTH_MODIFIED;
    else
        h_MSMagnet->ucModifiedFlags &= ~MAGNET_STRENGTH_MODIFIED;
}

ACP_tdxBool	fn_bMSMagnetIsStrengthModified(MS_tdxHandleToMSMagnet h_MSMagnet)
{

	return (unsigned char)( h_MSMagnet->ucModifiedFlags & MAGNET_STRENGTH_MODIFIED ? TRUE : FALSE );
}
/* 0x00415380 */
void fn_vMSMagnetSetPositionModification(MS_tdxHandleToMSMagnet h_MSMagnet,ACP_tdxBool _bModified)
{
	if( _bModified )
		h_MSMagnet->ucModifiedFlags |= MAGNET_POSITION_MODIFIED;
	else
		h_MSMagnet->ucModifiedFlags &= ~MAGNET_POSITION_MODIFIED;
}

ACP_tdxBool fn_bMSMagnetIsPositionModified(MS_tdxHandleToMSMagnet h_MSMagnet)
{
    return (unsigned char)( h_MSMagnet->ucModifiedFlags & MAGNET_POSITION_MODIFIED ? TRUE : FALSE );
}

void __cdecl fn_vMSMagnetInitValueSAI(MS_tdxHandleToMSMagnet _h_MSMagnet)
{

	SAI_fn_bInsertFloatInTable((void*)&_h_MSMagnet->xFar, C_Magnet_SAIInitFlags);
	SAI_fn_bInsertFloatInTable((void*)&_h_MSMagnet->xNear, C_Magnet_SAIInitFlags);
	SAI_fn_bInsertFloatInTable((void*)&_h_MSMagnet->xStrength, C_Magnet_SAIInitFlags);
	SAI_fn_bInsertLongInTable((void*)&_h_MSMagnet->ulDuration, C_Magnet_SAIInitFlags);
}

void __cdecl fn_vMSMagnetFreeInitValueSAI(MS_tdxHandleToMSMagnet _h_MSMagnet)
{

	SAI_fn_bRemoveValueFromTable((void*)&_h_MSMagnet->xFar, C_Magnet_SAIInitFlags);
	SAI_fn_bRemoveValueFromTable((void*)&_h_MSMagnet->xNear, C_Magnet_SAIInitFlags);
	SAI_fn_bRemoveValueFromTable((void*)&_h_MSMagnet->xStrength, C_Magnet_SAIInitFlags);
	SAI_fn_bRemoveValueFromTable((void*)&_h_MSMagnet->ulDuration, C_Magnet_SAIInitFlags);
}
