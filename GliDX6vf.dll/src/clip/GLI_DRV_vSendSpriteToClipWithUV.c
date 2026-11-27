#include "GliDX6vf/GliDX6vf.h"
#include "GliDX6vf/GliDX6vf_State.h"
#include "GliDX6vf/clip_staging.h"
#include "GliDX6vf/GLI_DiagLog.h"

#define TEX_XADDU_OFFSET            0x20
#define TEX_XADDV_OFFSET            0x24
#define TEX_INCREMENT_IS_ENABLE     0x28

__declspec(dllexport) void __cdecl
GLI_DRV_vSendSpriteToClipWithUV(float *param_1, float *param_2,
                                  float param_3, GLI_tdstInternalGlobalValuesFor3dEngine *p_stGlobals)
{
    unsigned int uVar4;
    int tex;
    float u_off, v_off;

    GLI_M_DiagCountSend();
    if (GLI_g_ActivateGliFlag && GLI_DiagVerbose()) {
        GLI_DiagLog("send vSendSpriteToClipWithUV #%lu p1=%p uv=%p z=0x%08x ctx=0x%x",
                    GLI_g_DiagSendCount, (void *)param_1, (void *)param_2, *(unsigned int *)&param_3, (int)p_stGlobals);
    }
    GLI_ResetStagingCursor();

    *(float *) GLI_g_pStagingCursor          = param_1[0];
    *(float *)(GLI_g_pStagingCursor + 0x04)  = param_1[1];
    *(float *)(GLI_g_pStagingCursor + 0x20)  = param_1[4];
    *(float *)(GLI_g_pStagingCursor + 0x24)  = param_1[5];
    *(float *)(GLI_g_pStagingCursor + 0x40)  = param_1[8];
    *(float *)(GLI_g_pStagingCursor + 0x44)  = param_1[9];
    *(float *)(GLI_g_pStagingCursor + 0x60)  = param_1[0xc];
    *(float *)(GLI_g_pStagingCursor + 0x64)  = param_1[0xd];

    *(float *)(GLI_g_pStagingCursor + 0x0c) = param_3;
    *(float *)(GLI_g_pStagingCursor + 0x08) = param_3;
    *(float *)(GLI_g_pStagingCursor + 0x2c) = param_3;
    *(float *)(GLI_g_pStagingCursor + 0x28) = param_3;
    *(float *)(GLI_g_pStagingCursor + 0x4c) = param_3;
    *(float *)(GLI_g_pStagingCursor + 0x48) = param_3;
    *(float *)(GLI_g_pStagingCursor + 0x6c) = param_3;
    *(float *)(GLI_g_pStagingCursor + 0x68) = param_3;

    *(unsigned int *)(GLI_g_pStagingCursor + 0x10) = p_stGlobals->ulColorInitForSprite;
    *(unsigned int *)(GLI_g_pStagingCursor + 0x30) = p_stGlobals->ulColorInitForSprite;
    *(unsigned int *)(GLI_g_pStagingCursor + 0x50) = p_stGlobals->ulColorInitForSprite;
    *(unsigned int *)(GLI_g_pStagingCursor + 0x70) = p_stGlobals->ulColorInitForSprite;

    {
        if (param_1[1]    < 0.0f) return;
        if (param_1[5]    < 0.0f) return;
        if (param_1[9]    < 0.0f) return;
        if (param_1[0xd]  < 0.0f) return;
        if (param_1[1]    > GLI_g_GliBigGlobals->fYMaxClipping) return;
        if (param_1[5]    > GLI_g_GliBigGlobals->fYMaxClipping) return;
        if (param_1[9]    > GLI_g_GliBigGlobals->fYMaxClipping) return;
        if (param_1[0xd]  > GLI_g_GliBigGlobals->fYMaxClipping) return;
    }

    if (p_stGlobals->xTextureDontAcceptFog != 0) {
        uVar4 = 0xff000000u;
    } else {
        uVar4 = p_stGlobals->ulSpecularColorForSprite;
    }
    *(unsigned int *)(GLI_g_pStagingCursor + 0x14) = uVar4;
    *(unsigned int *)(GLI_g_pStagingCursor + 0x34) = uVar4;
    *(unsigned int *)(GLI_g_pStagingCursor + 0x54) = uVar4;
    *(unsigned int *)(GLI_g_pStagingCursor + 0x74) = uVar4;

    tex = (int)p_stGlobals->p_stCurrentTexture;

#ifdef GLI_DX6_FIXES
    if (tex != 0 && *(int *)((unsigned char *)tex + TEX_INCREMENT_IS_ENABLE) != 0) {
#else
    if (*(int *)((unsigned char *)tex + TEX_INCREMENT_IS_ENABLE) != 0) {
#endif
        u_off = *(float *)((unsigned char *)tex + TEX_XADDU_OFFSET);
        v_off = *(float *)((unsigned char *)tex + TEX_XADDV_OFFSET);
        *(float *)(GLI_g_pStagingCursor + 0x58) = param_2[4] + u_off;
        *(float *)(GLI_g_pStagingCursor + 0x5c) = param_2[5] + v_off;
        *(float *)(GLI_g_pStagingCursor + 0x78) = param_2[6] + u_off;
        *(float *)(GLI_g_pStagingCursor + 0x7c) = param_2[7] + v_off;
        *(float *)(GLI_g_pStagingCursor + 0x18) = u_off + param_2[0];
        *(float *)(GLI_g_pStagingCursor + 0x1c) = param_2[1] + v_off;
        *(float *)(GLI_g_pStagingCursor + 0x38) = param_2[2] + u_off;
        *(float *)(GLI_g_pStagingCursor + 0x3c) = param_2[3] + v_off;
    } else {
        *(float *)(GLI_g_pStagingCursor + 0x58) = param_2[4];
        *(float *)(GLI_g_pStagingCursor + 0x5c) = param_2[5];
        *(float *)(GLI_g_pStagingCursor + 0x78) = param_2[6];
        *(float *)(GLI_g_pStagingCursor + 0x7c) = param_2[7];
        *(float *)(GLI_g_pStagingCursor + 0x18) = param_2[0];
        *(float *)(GLI_g_pStagingCursor + 0x1c) = param_2[1];
        *(float *)(GLI_g_pStagingCursor + 0x38) = param_2[2];
        *(float *)(GLI_g_pStagingCursor + 0x3c) = param_2[3];
    }

    GLI_FinalizeSpriteStaging();
    GLI_FlushStagedPrimitives();
}
