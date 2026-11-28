#ifndef _MEC_MECOPT_H_
#define _MEC_MECOPT_H_

#include "ACP/ACP_types.h"
#include "DNM/DNM_types.h"
#include "POS/POS_types.h"
#include "MTH/MTH3D.h"

void __cdecl MEC_fn_vGravitySpeed (DNM_tdstDynamics *p_stDynamic, MTH_tdxReal xDT);                  /* 0x0043DF40 */
void __cdecl MEC_fn_vInertia (DNM_tdstDynamics *p_stDynamic, MTH_tdxReal xDT, POS_tdxHandleToPosition hMatrix, POS_tdxHandleToPosition hInvMatrix); /* 0x0043DF70 */
void __cdecl MEC_fn_vAddTiltSpeed (DNM_tdstDynamics *p_stDynamic, MTH_tdxReal xDt);                                   /* 0x0043E0A0 */
void __cdecl MEC_fn_vPositionLimit (DNM_tdstDynamics *p_stDynamic, MTH3D_tdstVector *p_stSafePreviousPosition); /* 0x0043E940 */
void __cdecl MEC_fn_vSpeedLimit (DNM_tdstDynamics *p_stDynamic, MTH_tdxReal xDT, POS_tdxHandleToPosition hMatrix, POS_tdxHandleToPosition hInvMatrix); /* 0x0043EAC0 */
void __cdecl MEC_fn_vImposeSpeed (DNM_tdstDynamics *p_stDynamic, MTH_tdxReal xDT, POS_tdxHandleToPosition hMatrix, POS_tdxHandleToPosition hInvMatrix);  /* 0x0043E350 */
void __cdecl MEC_fn_vAddSpeed (DNM_tdstDynamics *p_stDynamic, MTH_tdxReal xDT, POS_tdxHandleToPosition hMatrix, POS_tdxHandleToPosition hInvMatrix);     /* 0x0043E4E0 */
void __cdecl MEC_fn_vProposeSpeed (DNM_tdstDynamics *p_stDynamic, MTH_tdxReal xDT, POS_tdxHandleToPosition hMatrix, POS_tdxHandleToPosition hInvMatrix); /* 0x0043E680 */
void __cdecl MEC_fn_vAdjustScaleData (DNM_tdstDynamics *p_stDynamic, POS_tdxHandleToPosition hMatrix, POS_tdxHandleToPosition hInvMatrix); /* 0x0043ECA0 */
void __cdecl MEC_fn_vAddStream (DNM_tdstDynamics *p_stDynamic, MTH_tdxReal xDT, ACP_tdxBool bSlide); /* 0x0043E810 */
void __cdecl MEC_fn_vImposeRotation (DNM_tdstDynamics *p_stDynamic);                                 /* 0x0043E870 */
void __cdecl MEC_fn_vAdditionnalTranslation (DNM_tdstDynamics *p_stDynamic);                         /* 0x0043E900 */

#endif
