#ifndef RAYMAN2_GAM_MSSOUND_H
#define RAYMAN2_GAM_MSSOUND_H

typedef struct tdstMSSound_ * MS_tdxHandleToMSSound;

void __cdecl fn_vMSSoundSetSaturationDistance(MS_tdxHandleToMSSound h_MSSound, long xSaturationDistance);  /* 0x0042DB20 */
void __cdecl fn_vMSSoundSetBackGroundDistance(MS_tdxHandleToMSSound h_MSSound, long xBackGroundDistance);  /* 0x0042DE40 */
/* 0x00415930 */
struct _RollOffParam;
int __cdecl fn_bRetRollOffFactor(long _lObj, struct _RollOffParam *_p_stRolloff);

#endif
