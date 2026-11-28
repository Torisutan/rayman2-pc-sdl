#ifndef RAYMAN2_GAM_MSMAG_H
#define RAYMAN2_GAM_MSMAG_H

#define MAGNETOFF                   0
#define MAGNETON                    1
#define MAGNETGROWING               2
#define MAGNETRECOVERING            3

#define MAGNET_STRENGTH_MODIFIED    1
#define MAGNET_NEARFAR_MODIFIED     2
#define MAGNET_POSITION_MODIFIED    4

#if defined(D_MSMagnet_StructureDefine)
#include "MTH/MTH_types.h"
#include "ACP/ACP_types.h"
#include "MGT/MGTStruc.h"

struct tdstMSMagnet_
{
    MTH_tdxReal                         xFar;
    MTH_tdxReal                         xNear;
    MTH_tdxReal                         xStrength;
    unsigned char                       status;

    MTH3D_tdstVector                    stPosition;
    unsigned long                       ulDuration;

    ACP_tdxIndex                        xMaxIndexOfMagnetModification;
    MGT_tdxHandleToMagnetModification   d_hMagnetModification[MGT_MAX_NUMBEROF_MODIFIEDOBJECTS];

    unsigned char                       ucModifiedFlags;
};
#endif

typedef struct tdstMSMagnet_ * MS_tdxHandleToMSMagnet;

float         __cdecl fn_xMSMagnetGetFar(MS_tdxHandleToMSMagnet h_MSMagnet);                    /* 0x00415160 */
void          __cdecl fn_vMSMagnetSetFar(MS_tdxHandleToMSMagnet h_MSMagnet, float xValue);      /* 0x00415180 */
float         __cdecl fn_xMSMagnetGetNear(MS_tdxHandleToMSMagnet h_MSMagnet);                   /* 0x004151A0 */
void          __cdecl fn_vMSMagnetSetNear(MS_tdxHandleToMSMagnet h_MSMagnet, float xValue);     /* 0x004151C0 */
float         __cdecl fn_xMSMagnetGetStrength(MS_tdxHandleToMSMagnet h_MSMagnet);               /* 0x004151E0 */
void          __cdecl fn_vMSMagnetSetStrength(MS_tdxHandleToMSMagnet h_MSMagnet, float xValue); /* 0x004151F0 */
unsigned char __cdecl fn_lMSMagnetGetStatus(MS_tdxHandleToMSMagnet h_MSMagnet);                 /* 0x00415210 */
void          __cdecl fn_vMSMagnetSetStatus(MS_tdxHandleToMSMagnet h_MSMagnet, unsigned char xValue);   /* 0x00415220 */
unsigned long __cdecl fn_ulMSMagnetGetDuration(MS_tdxHandleToMSMagnet h_MSMagnet);             /* 0x00415300 */
void          __cdecl fn_vMSMagnetSetDuration(MS_tdxHandleToMSMagnet h_MSMagnet, unsigned long ulDuration); /* 0x00415310 */

void          __cdecl fn_vMSMagnetAddMagnetModification(MS_tdxHandleToMSMagnet h_MSMagnet, void *h_MagnetModif);       /* 0x00415230 */
void          __cdecl fn_vMSMagnetDeleteMagnetModification(MS_tdxHandleToMSMagnet h_MSMagnet, void *h_MagnetModif);    /* 0x00415270 */
void          __cdecl fn_vMSMagnetFreeInitValueSAI(MS_tdxHandleToMSMagnet _h_MSMagnet);                                 /* 0x004153F0 */
void          __cdecl fn_vMSMagnetSetNearFarModification(MS_tdxHandleToMSMagnet h_MSMagnet, unsigned char _bModified);  /* 0x00415320 */
void          __cdecl fn_vMSMagnetSetStrengthModification(MS_tdxHandleToMSMagnet h_MSMagnet, unsigned char _bModified); /* 0x00415350 */

#endif
