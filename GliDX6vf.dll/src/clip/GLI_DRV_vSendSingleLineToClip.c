#include "GliDX6vf/GliDX6vf.h"
#include "GliDX6vf/GliDX6vf_State.h"
#include "GliDX6vf/clip_staging.h"
#include "GliDX6vf/GLI_DiagLog.h"

void __cdecl GLI_DRV_vSendSingleLineToClip(
    void *param_1,
    void *param_2,
    void *param_3,
    void *param_4,
    void *param_5,
    void *param_6,
    long  param_7,
    void *param_8)
{
    unsigned int  *v1  = (unsigned int *)param_3;
    unsigned int  *v2  = (unsigned int *)param_5;

    GLI_M_DiagCountSend();
    if (GLI_g_ActivateGliFlag && GLI_DiagVerbose()) {
        GLI_DiagLog("send vSendSingleLineToClip #%lu v1=%p v2=%p",
                    GLI_g_DiagSendCount, param_3, param_5);
    }

    v1[3] = 0xffffffff;
    v2[3] = 0xffffffff;

    *(unsigned int *)(GLI_g_pStagingCursor + 0x00) = v1[0];
    *(unsigned int *)(GLI_g_pStagingCursor + 0x04) = v1[1];

    *(unsigned int *)(GLI_g_pStagingCursor + 0x20) = v2[0];
    *(unsigned int *)(GLI_g_pStagingCursor + 0x24) = v2[1];

    *(unsigned int *)(GLI_g_pStagingCursor + 0x0c) = *(unsigned int *)((unsigned char *)param_2 + 8);
    *(unsigned int *)(GLI_g_pStagingCursor + 0x08) = *(unsigned int *)(GLI_g_pStagingCursor + 0x0c);

    *(unsigned int *)(GLI_g_pStagingCursor + 0x2c) = *(unsigned int *)((unsigned char *)param_4 + 8);
    *(unsigned int *)(GLI_g_pStagingCursor + 0x28) = *(unsigned int *)(GLI_g_pStagingCursor + 0x2c);

    *(unsigned int *)(GLI_g_pStagingCursor + 0x10) = v1[3];
    *(unsigned int *)(GLI_g_pStagingCursor + 0x30) = v2[3];

    GLI_DrawStagedLine();
}
