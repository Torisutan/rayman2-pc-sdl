#include "GliDX6vf/GliDX6vf.h"

/* 0x1000d3f8 */
static GLI_tdstDllInfo GLI_g_stDefaultDllInfo = { "DirectX6", "DirectX 6 Driver", "1.0.0", 3 };

GLI_tdstDllInfo GLI_g_stDllInfo;    /* 0x1000f8a0 */

/* 0x10008910 */
void __cdecl GLI_vInitDllInfo(void)
{
    GLI_g_stDllInfo = GLI_g_stDefaultDllInfo;
}
