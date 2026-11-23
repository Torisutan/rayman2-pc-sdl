#pragma once

#include "ACP/ACP_types.h"

struct HIE_tdstSuperObject;

void __cdecl MGT_fn_vTreatMagnets(void);

/* 0x00440C00 */
void __cdecl MGT_fn_vDeleteObjectFromMagnetList(struct HIE_tdstSuperObject *p_stObject);

void __cdecl MGT_fn_vReleaseAllEmptyModifiedObjects(void);
void __cdecl MGT_fn_vClearTargetModificationsInfluencedBits(int p_target, ACP_tdxBool);
void __cdecl MGT_fn_vSetTargetModificationsUsed(int p_target, ACP_tdxBool bUsed);
