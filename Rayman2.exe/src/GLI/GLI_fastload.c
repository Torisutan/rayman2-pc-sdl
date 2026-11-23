/*
 =======================================================================================
  Name        : GLI_fastload.c
  Description : Loading screen: progress bar refreshed at a bounded rate

  Every progress step that moves the bar redraws the loading screen and waits for a
  synchronised flip (GLI_RefreshUiSurfaceIfVisible). Those waits, not the loading itself,
  take most of a level load. The bar is still drawn into its buffer at every step
  (GLI_FillProgressBarSpan); only the copy to the screen and the flip are skipped while
  the last one is more recent than GLI_pc_C_ulProgressRefreshPeriod. The end of the load
  shows the full image again (FUN_0045edf0), so the last steps are never lost.
 =======================================================================================
 */
#ifdef RAYMAN2_FAST_LOADING

#include <windows.h>
#include "GLI/Texture.h"
#include "GLI/GLI_fastload.h"

static unsigned long GLI_pc_g_ulLastProgressRefresh;
static long          GLI_pc_g_bProgressRefreshed = 0;

/*
 ----------------------------------------------------------------------------------------
  Description   : Refresh the loading screen unless it was refreshed less than
                  GLI_pc_C_ulProgressRefreshPeriod ago
 ----------------------------------------------------------------------------------------
 */
void __cdecl GLI_pc_fn_vRefreshProgressUiIfDue(char *p_stUiObject)
{
    unsigned long ulNow = GetTickCount();

    if ( GLI_pc_g_bProgressRefreshed
      && ulNow - GLI_pc_g_ulLastProgressRefresh < GLI_pc_C_ulProgressRefreshPeriod )
        return;

    GLI_RefreshUiSurfaceIfVisible(p_stUiObject);
    GLI_pc_g_ulLastProgressRefresh = ulNow;
    GLI_pc_g_bProgressRefreshed    = 1;
}

#endif /* RAYMAN2_FAST_LOADING */
