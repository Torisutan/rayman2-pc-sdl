#ifndef RAYMAN2_AI_CAM_TOOL_H
#define RAYMAN2_AI_CAM_TOOL_H

#include "ACP/ACP_types.h"
#include "MTH/MTH3D.h"

struct tdstCineinfo_;
struct CAM_tdstUpdateCamera_;
struct HIE_tdstSuperObject;

void CAM_memcpy(void *dest, void *src, long size);   /* 0x00473360 */
void CAM_fn_vSetResetInternalFlag(struct tdstCineinfo_ *_hCineInfo, unsigned short *_p_uwCurrentValue, unsigned short *_p_uwVisibilityValue, unsigned short *_p_uwWorkValue, long _lTypeOfChange, long _lChangingMode, unsigned short _uwFlag); /* 0x004733A0 */
/* 0x004739D0 */
extern ACP_tdxBool	CAM_fn_bEqualVectorWithEpsilon(MTH3D_tdstVector	*, MTH3D_tdstVector *, MTH_tdxReal);
/* 0x00474270 */
extern void			CAM_fn_vInitCameraStructure(struct HIE_tdstSuperObject *, struct CAM_tdstUpdateCamera_ *);
/* 0x00473E20 */
extern void			Cam_fn_vComputeMovingOnRail(struct CAM_tdstUpdateCamera_ *, MTH3D_tdstVector *, MTH3D_tdstVector *, MTH3D_tdstVector *, MTH_tdxReal);
/* 0x00473C60 */
extern MTH_tdxReal  Cam_fn_xComputeProjectionRail(MTH3D_tdstVector *,MTH3D_tdstVector *,MTH3D_tdstVector *);
/* 0x004746C0 */
extern void CAM_fn_vForceBestPosition(struct HIE_tdstSuperObject *_hCamera);

#endif
