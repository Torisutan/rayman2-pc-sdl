#pragma once

#include "GliDX6vf/GliDX6vf_State.h"

#ifdef __cplusplus
extern "C" {
#endif

/* 0x100012f0 */
void __cdecl GLI_ResetStagingCursor(void);

/* 0x10001300 */
void __cdecl GLI_FlushStagedPrimitives(void);

/* 0x100013f0 */
void __cdecl GLI_DrawStagedLine(void);

/* 0x10001410 */
void __cdecl GLI_CopyStagedBlock60(const void *src);

/* 0x10001440 */
void __cdecl GLI_CopyStagedBlockC0(const void *src);

/* 0x10001340 */
void __cdecl GLI_FinalizeSpriteStaging(void);

/* 0x100014a0 */
void __cdecl GLI_AppendTrianglePipelineTerminal(void);

/* 0x10002760 */
void __cdecl GLI_ResetClipPipeline(void);

/* 0x10001560 */
void __cdecl GLI_SnapshotClipWindow(void);

void __cdecl GLI_PipelineTerminal(void);

/* 0x10006290 */
void __cdecl GLI_SendSpriteTrianglesViaClip(unsigned int *param_1, GLI_tdstInternalGlobalValuesFor3dEngine *p_stGlobals);

/* 0x10001050 */
unsigned int __cdecl GLI_InterpolatePackedColor(unsigned int param_1,
                                                 unsigned int param_2,
                                                 float param_3);

/* 0x10002a90 */ void __cdecl GLI_PL_BackfaceCull(void);
/* 0x10002c50 */ void __cdecl GLI_PL_XYGate(void);
/* 0x10002de0 */ void __cdecl GLI_PL_CopyFog(void);
/* 0x10002e50 */ void __cdecl GLI_PL_ZNearClip(void);
/* 0x10003c40 */ void __cdecl GLI_PL_XMinClip(void);
/* 0x10004940 */ void __cdecl GLI_PL_XMaxClip(void);
/* 0x100042c0 */ void __cdecl GLI_PL_YMaxClip(void);
/* 0x10004fb0 */ void __cdecl GLI_PL_YMinClip(void);
/* 0x100028c0 */ void __cdecl GLI_PL_CopyUV(void);

/* 0x10002780 */
void __cdecl GLI_PL_BackfaceCullA(void);
/* 0x100027d0 */
void __cdecl GLI_PL_BackfaceCullB(void);
/* 0x10002820 */
void __cdecl GLI_PL_CopyUVOffset(void);
/* 0x10002940 */
void __cdecl GLI_PL_ScaleU(void);
/* 0x10002990 */
void __cdecl GLI_PL_ScaleV(void);
/* 0x100029f0 */
void __cdecl GLI_PL_LoadColorsFromCtx(void);
/* 0x10002a40 */
void __cdecl GLI_PL_LoadColorsFromElement(void);
/* 0x10002ac0 */
void __cdecl GLI_PL_CulledVertexCheck(void);
/* 0x10002b10 */
void __cdecl GLI_PL_XSignAndGate(void);
/* 0x10002b50 */
void __cdecl GLI_PL_XGateReject(void);
/* 0x10002b80 */
void __cdecl GLI_PL_XSignCheck(void);
/* 0x10002bb0 */
void __cdecl GLI_PL_YSignAndGate(void);
/* 0x10002bf0 */
void __cdecl GLI_PL_YGateReject(void);
/* 0x10002c20 */
void __cdecl GLI_PL_YGateReject2(void);
/* 0x10002cc0 */
void __cdecl GLI_PL_LoadVertex(void);
/* 0x10003410 */
void __cdecl GLI_PL_FlagClip(void);
/* 0x10005630 */
void __cdecl GLI_PL_FlagClipComposite(void);

#ifdef __cplusplus
}
#endif
