#ifndef __AI_globals_h__
#define __AI_globals_h__

#undef EXTERN
#if !defined(D_AI_VariableDefine)
#define EXTERN extern
#else
#define EXTERN
#endif

EXTERN unsigned char AI_g_ucProcedureActionReturn;

EXTERN char gcGlobAleat;   /* 0x004A7070 */

/* 0x004A544C */
EXTERN char AI_CAM_gs_cCanTestStatic;
#define AI_CAM_gp_cCanTestStatic (&AI_CAM_gs_cCanTestStatic)

/* 0x0049CCE4 */
extern const float AI_K_xRadToDeg;

EXTERN char gcSimulateNotU64;   /* 0x004B9C34 */
EXTERN char AI_CAM_c_RefAxisIsAlreadyComputed;
EXTERN char AI_CAM_c_NoDynChangeTheta;
EXTERN long AI_MAGIC_iPoints;
EXTERN long AI_MAGIC_iPointsMax;
EXTERN float AI_AIR_xPoints;
EXTERN float AI_AIR_xPointsMax;
EXTERN unsigned long AI_g_xMask;
EXTERN unsigned long AI_g_ulCollideTimeCounter;
EXTERN unsigned long AI_SAV_g_ulFlag4b9c10;   /* 0x004B9C10 */
EXTERN unsigned char bSomethingDone;
EXTERN unsigned char ucIsInSchedule;
EXTERN unsigned char g_ucIsInMacro;

EXTERN float AI_GLI_xFogNear;   /* 0x00503D74 */
EXTERN float AI_GLI_xFogFar;   /* 0x00503D70 */
EXTERN float AI_GLI_xFogInf;   /* 0x00503DA4 */
EXTERN float AI_GLI_xFogBlendNear;   /* 0x00503D6C */
EXTERN float AI_GLI_xFogBlendFar;   /* 0x00503D64 */
EXTERN float AI_GLI_xSavedFogNear;   /* 0x00503D80 */
EXTERN float AI_GLI_xSavedFogFar;   /* 0x00503D84 */
EXTERN float AI_GLI_xSavedFogInf;   /* 0x00503D88 */
EXTERN float AI_GLI_xSavedFogBlendNear;   /* 0x00503D8C */
EXTERN float AI_GLI_xSavedFogBlendFar;   /* 0x00503D90 */
EXTERN char AI_GLI_cSavedForceFogIsOn;   /* 0x004B9B50 */

EXTERN unsigned char AI_PAD_bResetInertia;   /* 0x004B9B9C */
extern float AI_PAD_xPadVectorX;
extern float AI_PAD_xPadVectorY;
extern float AI_PAD_xPadVectorZ;
extern short AI_PAD_wAxisValueX;
extern short AI_PAD_wAxisValueY;
extern float AI_PAD_xAnalogForceG;
extern float AI_PAD_xTrueAnalogForceG;
extern float AI_PAD_xRotationAngleG;
extern long AI_PAD_lDirectionSector;
/* 0x4B9B88 */
EXTERN unsigned long AI_PAD_hKeyLeft;
EXTERN unsigned long AI_PAD_hKeyRight;
EXTERN unsigned long AI_PAD_hKeyDown;
EXTERN unsigned long AI_PAD_hKeyUp;
EXTERN unsigned long AI_PAD_hKeySlow;
EXTERN float AI_PAD_xPrevY;
EXTERN float AI_PAD_xPrevX;
EXTERN long AI_PAD_lCurXValue;
EXTERN long AI_PAD_lCurYValue;

#endif

#undef EXTERN
#if !defined(D_AI_VariableDefine)
#define EXTERN extern
#else
#define EXTERN
#endif

EXTERN unsigned char AI_g_ucDynamicAlloc;

#undef EXTERN
#if !defined(D_AI_VariableDefine)
#define EXTERN extern
#else
#define EXTERN
#endif

EXTERN long AI_g_iNumLoop;

#undef EXTERN
#if !defined(D_AI_VariableDefine)
#define EXTERN extern
#else
#define EXTERN
#endif

EXTERN char AI_GLI_cFogIsSaved;   /* 0x00503D68 */

#define AI_g_p_stMainActor             (GAM_g_stEngineStructure.g_hNextMainActor)

#define AI_g_ulTrameNumber             (*(unsigned long *)(GAM_ENGINE_BASE + 0x64))
#define AI_GES_ucPreviousLevel         (*(unsigned char *)(GAM_ENGINE_BASE + 0xC24))
#define AI_GES_ucExitIdToQuitPrevLevel (*(unsigned char *)(GAM_ENGINE_BASE + 0xC25))
#define AI_GES_xDemoMode               (*(unsigned char *)(GAM_ENGINE_BASE + 0xC27))

#undef EXTERN
#if !defined(D_AI_VariableDefine)
#define EXTERN extern
#else
#define EXTERN
#endif

struct AI_tdstIntelligence_;
EXTERN struct AI_tdstIntelligence_ *g_p_stIntelligence;

#undef EXTERN
#if !defined(D_AI_VariableDefine)
#define EXTERN extern
#else
#define EXTERN
#endif

EXTERN unsigned char g_ucNRule;

extern long  AI_TEXT_gai_IntParameters[10];
extern float AI_TEXT_gax_RealParameters[10];

#define AI_TA_p_stFixViewportAttr ((void *)(GAM_ENGINE_BASE + 0x154))

#ifdef RAYMAN2_WIDESCREEN
/* width of the 4:3 band: superimposed objects are scaled to it (width / 640) */
#include "GLI/GLI_ratio.h"
#define AI_TA_dwScreenWidth \
    ((unsigned long)MTH_M_xRealToLong(MTH_M_xMul(GLI_g_xSafeAreaRatio, MTH_M_xLongToReal( \
        ((struct GLD_tdstViewportAttributes_ *)AI_TA_p_stFixViewportAttr)->dwWidth))))
#else
#define AI_TA_dwScreenWidth \
    (((struct GLD_tdstViewportAttributes_ *)AI_TA_p_stFixViewportAttr)->dwWidth)
#endif /* RAYMAN2_WIDESCREEN */
#define AI_TA_dwScreenHeight \
    (((struct GLD_tdstViewportAttributes_ *)AI_TA_p_stFixViewportAttr)->dwHeight)

#define AI_OPT_p_stLanguageTable  ((char *)GAM_g_stEngineStructure.p_stLanguageTable)
