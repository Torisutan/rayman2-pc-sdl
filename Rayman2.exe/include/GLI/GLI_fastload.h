#pragma once
/*
 =======================================================================================
  Name        : GLI_fastload.h
  Description : Loading screen: progress bar refreshed at a bounded rate
 =======================================================================================
 */
#ifdef RAYMAN2_FAST_LOADING

/* Minimum time between two refreshes of the progress bar on screen, in milliseconds */
#define GLI_C_ulProgressRefreshPeriod  100

/**********************************************************************************************/
/* Name: GLI_fn_vRefreshProgressUiIfDue */
/* Goal: GLI_RefreshUiSurfaceIfVisible, at most once per GLI_C_ulProgressRefreshPeriod */
/**********************************************************************************************/
void __cdecl GLI_fn_vRefreshProgressUiIfDue(char *p_stUiObject);

#endif /* RAYMAN2_FAST_LOADING */
