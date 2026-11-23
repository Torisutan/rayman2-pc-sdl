#pragma once

#define WIN32_LEAN_AND_MEAN
#include <windows.h>

/* 0x0049f080 */
extern HANDLE g_hWnd_storage;
#define g_hWnd (&g_hWnd_storage)

#undef EXTERN
#if !defined(D_GAM_VariableDefine)
#define EXTERN extern
#else
#define EXTERN
#endif

EXTERN long GAM_g_stIndexMicrosList_lCount;
EXTERN unsigned char GAM_g_ucRefreshUi;
EXTERN long GAM_g_lSaveSlotDirty;
EXTERN long GAM_g_lSoundObjectTypeMenu;
EXTERN unsigned long PLA_g_ulMaxOccupationOfSupObjHeap;
EXTERN unsigned long g_Randomhandle;
EXTERN unsigned long PLA_g_ulMaxOccupationOfChannels;
EXTERN float TFMT_g_xFallbackScale;

#undef EXTERN
#if !defined(D_GAM_VariableDefine)
#define EXTERN extern
#else
#define EXTERN
#endif

EXTERN void * GAM_g_stIndexMicrosList_hFirst;
EXTERN void * GAM_g_stIndexMicrosList_hLast;
EXTERN void * GAM_g_lSoundVolumeLineSound;
EXTERN void * GAM_g_lSoundVolumeLineMusic;
EXTERN void * CAM_g_hOldTargetedPerso;

struct tdstNodeCamera_;

#undef EXTERN
#if !defined(D_GAM_VariableDefine)
#define EXTERN extern
#else
#define EXTERN
#endif

EXTERN unsigned char GAM_g_ucIsEdInGhostMode;

#undef EXTERN
#if !defined(D_GAM_VariableDefine)
#define EXTERN extern
#else
#define EXTERN
#endif

EXTERN void *gp_stLight;

#define CAM_g_hCameraListHead      (GAM_g_stEngineStructure.hCameraList_hFirstElementDyn)

#undef EXTERN
#if !defined(D_GAM_VariableDefine)
#define EXTERN extern
#else
#define EXTERN
#endif

EXTERN void *GAM_g_a_pfnDynamicFunction[2];

#undef EXTERN
#if !defined(D_GAM_VariableDefine)
#define EXTERN extern
#else
#define EXTERN
#endif

EXTERN long GAM_g_lSoundObjectTypeMusic;
EXTERN long GAM_g_lSoundObjectTypeAnim;

#undef EXTERN
#if !defined(D_GAM_VariableDefine)
#define EXTERN extern
#else
#define EXTERN
#endif
EXTERN unsigned char GAM_g_bInGameMenu;

#undef EXTERN
#if !defined(D_GAM_VariableDefine)
#define EXTERN extern
#else
#define EXTERN
#endif
EXTERN char GAM_g_aszCmdLine[260];
#define GAM_g_szCmdLine (GAM_g_aszCmdLine)

#undef EXTERN
#if !defined(D_GAM_VariableDefine)
#define EXTERN extern
#else
#define EXTERN
#endif
EXTERN char g_cCDROM;

#undef EXTERN
#if !defined(D_GAM_VariableDefine)
#define EXTERN extern
#else
#define EXTERN
#endif
EXTERN float GAM_pc_g_xParticuleRate;

#undef EXTERN
#if !defined(D_GAM_VariableDefine)
#define EXTERN extern
#else
#define EXTERN
#endif
EXTERN char g_cIsLevelOk;   /* 0x00500371 */
EXTERN unsigned char GAM_g_b4b7464;

#undef EXTERN
#if !defined(D_GAM_VariableDefine)
#define EXTERN extern
#else
#define EXTERN
#endif
EXTERN unsigned long GAM_g_ul4b7468;

#undef EXTERN
#if !defined(D_GAM_VariableDefine)
#define EXTERN extern
#else
#define EXTERN
#endif
EXTERN unsigned long GAM_g_ul5013c8;

#undef EXTERN
#if !defined(D_GAM_VariableDefine)
#define EXTERN extern
#else
#define EXTERN
#endif
EXTERN unsigned long GAM_g_ul4a6ac8;

#undef EXTERN
#if !defined(D_GAM_VariableDefine)
#define EXTERN extern
#else
#define EXTERN
#endif
EXTERN unsigned char GAM_g_aucObjectTypes[0x24];
#define GAM_g_stObjectTypes ((char *)GAM_g_aucObjectTypes)

#undef EXTERN
#if !defined(D_GAM_VariableDefine)
#define EXTERN extern
#else
#define EXTERN
#endif

#undef EXTERN
#if !defined(D_GAM_VariableDefine)
#define EXTERN extern
#else
#define EXTERN
#endif
EXTERN unsigned int GAM_pc_g_ulMapLinkPosListHead;

#undef EXTERN
#if !defined(D_GAM_VariableDefine)
#define EXTERN extern
#else
#define EXTERN
#endif
EXTERN unsigned char GAM_g_ucHLMMemoryBlock;

/* 0x004077E0 */
void *__cdecl fnp_vGameMallocInHLM(unsigned int ulSize);

#undef EXTERN
#if !defined(D_GAM_VariableDefine)
#define EXTERN extern
#else
#define EXTERN
#endif
EXTERN int GAM_pc_g_hMapMusicSound;

#undef EXTERN
#if !defined(D_GAM_VariableDefine)
#define EXTERN extern
#else
#define EXTERN
#endif
EXTERN unsigned char GAM_pc_g_ucMapMusicIsOn;

EXTERN struct SNA_pc_tdstStream_ GAM_pc_g_stExternalCtrlFile;   /* 0x005116C0 */
