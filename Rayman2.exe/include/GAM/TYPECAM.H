#if !defined(__TYPECAM_H__)
#define __TYPECAM_H__

#if _MSC_VER >= 1000
#pragma once
#endif

typedef enum e_ucListViewport_
{
	CAM_e_NoViewport = 0,
	CAM_e_MainViewport,
	CAM_e_SecondaryViewport
} e_ucListViewport;

typedef enum e_ucCameraStates_
{
	CAM_e_State_GoToOptimal = 0,
	CAM_e_State_GoToComputedOptimal,
	CAM_e_State_FailureVisibility,
	CAM_e_State_FailureVisibilityWithoutCut,
	CAM_e_TempState_GhostMode,
	CAM_e_TempState_ApexMode
} e_ucCameraStates;

#endif
