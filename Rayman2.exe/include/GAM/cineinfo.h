#ifndef RAYMAN2_GAM_CINEINFO_H
#define RAYMAN2_GAM_CINEINFO_H

#include "MTH/MTH_types.h"
#include "ACP/ACP_types.h"

struct HIE_tdstSuperObject;

typedef struct CAM_tdstCameraConstants_ {
    float xRayCameraSphereToGo1, xRayCameraSphereToGo2, xRayCameraSphereToSee1, xRayCameraSphereToSee2;
    float xBaseAngleComputePosNormal, xBaseAngleComputePosFailure;
    float xAverageForComputePos, xAverageForComputeLinearSpeed;
    float xMinLinearSpeedCamera, xMinLinearSpeedTgtPerso, xMinAngularSpeedTgtPerso;
    float xEpsilonDynTgtAngle, xEpsilonDynTgtFocalAngle, xEpsilonForEqualVectors;
    float xOffsetMinDistPerso, xDistanceForNoCut;
    short xNumRayCameraCanGo, xNumRayCameraCanSee;
    short xTickTestVisibility, xTickFindBetterPos, xTickFindBetterPosIfNoMove, xTickFailureCantSee;
    float xDynSpeed_LinearFactNorm, xDynSpeed_LinearMax, xDynSpeed_LinearMulPerso;
    float xDynSpeed_AngularFactNorm, xDynSpeed_AngularMax, xDynSpeed_AngularMulPerso;
    float xDynSpeed_TargetFactNorm, xDynSpeed_TargetMax, xDynSpeed_TargetMulPerso;
    float xBaseCutAngle, xCutAngleFactorPos, xCutAngleFactorTgt;
    short wFlags, wAlign;
} CAM_tdstCameraConstants;
typedef char CAM_ct_assert_constants_size[(sizeof(CAM_tdstCameraConstants) == 0x80) ? 1 : -1];

extern CAM_tdstCameraConstants CAM_g_stCameraConstants;       /* 0x004FF660 */
extern CAM_tdstCameraConstants CAM_g_stCopyCameraConstants;   /* 0x004FF6E0 */
#define CAM_C_xAverageForComputePos (CAM_g_stCameraConstants.xAverageForComputePos)

#include "GAM/TYPECAM.H"

#define C_IAFlags_NoDynamicTarget                0x0001
#define C_IAFlags_NoAverageMoveTgtPerso          0x0002
#define C_IAFlags_NoParseCutAngle                0x0004
#define C_IAFlags_NoVisibility                   0x0008
#define C_IAFlags_NoVisibilityWithDynHie         0x0010
#define C_IAFlags_NoDynChangeTheta               0x0020
#define C_IAFlags_NoShiftUntilPosReached         0x0040
#define C_IAFlags_NoDynSpeed                     0x0080
#define C_IAFlags_User                           0x0FFF

#define C_IAFlags_TargetIsAlreadyComputed        0x1000
#define C_IAFlags_PositionIsAlreadyComputed      0x2000
#define C_IAFlags_RefAxisIsAlreadyComputed       0x4000
#define C_VolIAFlags_CurrentAlreadyCopiedInWork  0x01
#define C_VolIAFlags_InitJustCopiedInCurrent     0x02
#define C_VolIAFlags_CameraNotMove               0x04
#define C_PerIAFlags_VisibilityCopied            0x02
#define C_PerIAFlags_ShowInfo                    0x01
#define C_PerIAFlags_ConstraintMoveVisFailure    0x08

struct tdstInternalStructurCineinfo_
{
    MTH3D_tdstVector            stShiftTarget;
    MTH3D_tdstVector            stShiftPos;
    MTH_tdxReal                 xDistMin;
    MTH_tdxReal                 xDistMax;
    MTH_tdxReal                 xBoundDistMin;
    MTH_tdxReal                 xBoundDistMax;
    MTH_tdxReal                 xAngleAlpha;
    MTH_tdxReal                 xAngleShiftAlpha;
    MTH_tdxReal                 xAngleTheta;
    MTH_tdxReal                 xAngleShiftTheta;
    MTH_tdxReal                 xLinearSpeed;
    MTH_tdxReal                 xLinearIncreaseSpeed;
    MTH_tdxReal                 xLinearDecreaseSpeed;
    MTH_tdxReal                 xAngularSpeed;
    MTH_tdxReal                 xAngularIncreaseSpeed;
    MTH_tdxReal                 xAngularDecreaseSpeed;
    MTH_tdxReal                 xTargetSpeed;
    MTH_tdxReal                 xTargetIncreaseSpeed;
    MTH_tdxReal                 xTargetDecreaseSpeed;
    MTH_tdxReal                 xFocal;
    MTH_tdxReal                 xZMin;
    MTH_tdxReal                 xZMax;
    struct HIE_tdstSuperObject *hSuperObjectTargeted;
    struct HIE_tdstSuperObject *hSecondSuperObjectTargeted;
    unsigned short              uwDNMFlags;
    unsigned short              uwIAFlags;
    enum e_ucListViewport_      eTypeOfViewport;
    char                        cChannel;
    ACP_tdxBool                 bIsActive;
};

typedef struct tdstInternalStructurCineinfo_ *MS_tdxHandleToInternalCineinfo;

#define C_ucNbOfCheckedSpeeds 8
typedef struct tdstAveragePosition_
{
    MTH3D_tdstVector a_CheckedSpeed[C_ucNbOfCheckedSpeeds];
    MTH3D_tdstVector a_CheckedPos[C_ucNbOfCheckedSpeeds];
    MTH_tdxReal      a_CheckedTime[C_ucNbOfCheckedSpeeds];
} tdstAveragePosition;

typedef struct tdstSightAxisSystem_
{
    MTH3D_tdstVector SightAxis;
    MTH3D_tdstVector FirstComplementaryAxis;
    MTH3D_tdstVector SecondComplementaryAxis;
} tdstSightAxisSystem;

struct tdstCineinfo_
{
    struct tdstInternalStructurCineinfo_ *hInit;
    struct tdstInternalStructurCineinfo_ *hCurrent;
    struct tdstInternalStructurCineinfo_ *hVisibility;
    struct tdstInternalStructurCineinfo_ *hWork;

    MTH3D_tdstVector            stLastFailedOrigin;
    MTH3D_tdstVector            stLastFailedWanted;
    MTH3D_tdstVector            stForceTarget;
    MTH3D_tdstVector            stForcePosition;
    MTH3D_tdstVector            stForceRefAxisY;
    MTH3D_tdstVector            stForceRefAxisZ;
    MTH_tdxReal                 xApexRatio;
    MTH_tdxReal                 xLastDeltaCut;
    enum e_ucCameraStates_      eState;
    MTH_tdxReal                 xCounter1;
    MTH_tdxReal                 xCounter2;
    MTH_tdxReal                 xCounter3;
    MTH_tdxReal                 xCounter4;
    unsigned char               ucVolIAFlags;
    unsigned char               ucPerIAFlags;
    char                        cLastCutAngleSens;
    tdstAveragePosition         AveragePosition;
    tdstSightAxisSystem         stSightAxisSystem;
    ACP_tdxBool                 bCanDoBestPos;
};

typedef struct tdstCineinfo_ *MS_tdxHandleToCineinfo;

typedef struct tdstNodeCamera_
{
    struct HIE_tdstSuperObject *p_stSuperObjectCamera;
    struct tdstNodeCamera_     *hNext;
    struct tdstNodeCamera_     *hPrev;
    void                       *hFather;
} tdstNodeCamera;

void __cdecl CAM_fn_vInitCompleteCineinfo(struct tdstCineinfo_ *hCineinfo);       /* 0x00413DB0 */
void __cdecl CAM_fn_vUpdateTargetPosition(struct tdstCineinfo_ *hCineinfo);       /* 0x00413E30 */
void __cdecl CAM_fn_vSetCineinfoWorkFromCurrent(struct tdstCineinfo_ *hCineinfo); /* 0x004140A0 */
void __cdecl CAM_fn_vSetCineinfoCurrentFromVisibility(struct tdstCineinfo_ *hCineinfo); /* 0x00414100 */
void __cdecl CAM_fn_vSetCineinfoWorkFromVisibility(struct tdstCineinfo_ *hCineinfo);    /* 0x00414120 */
void __cdecl CAM_fn_vRemindSpeed(struct tdstCineinfo_ *hCineinfo, MTH_tdstMove *p_Speed); /* 0x00414370 */
void __cdecl CAM_fn_vRemindPos  (struct tdstCineinfo_ *hCineinfo, MTH_tdstMove *p_Pos);   /* 0x004143B0 */
void __cdecl CAM_fn_vRemindTime (struct tdstCineinfo_ *hCineinfo, MTH_tdxReal _dt);       /* 0x004143F0 */

/* 0x00414550 */
void __cdecl CAM_fn_vAveragePosition(struct tdstCineinfo_ *_hCineinfo, MTH_tdxReal xTimeWindow,
                                     MTH3D_tdstVector *p_result);                             /* 0x00414550 */

void __cdecl CAM_fn_vSetCineinfoCurrentFromInit(struct tdstCineinfo_ *hCineinfo);            /* 0x00414080 */
void __cdecl fn_vCineinfoSetSightAxis(struct tdstCineinfo_ *hCineinfo, MTH3D_tdstVector *p_stVector);              /* 0x004141B0 */
void __cdecl fn_vCineinfoSetFirstComplementaryAxis(struct tdstCineinfo_ *hCineinfo, MTH3D_tdstVector *p_stVector); /* 0x004141F0 */
void __cdecl fn_vCineinfoSetSecondComplementaryAxis(struct tdstCineinfo_ *hCineinfo, MTH3D_tdstVector *p_stVector);/* 0x00414230 */

struct HIE_tdstSuperObject *__cdecl CAM_fn_hCineInfoGetSuperObjectTargeted(
    struct tdstInternalStructurCineinfo_ *_hIntCineinfo);                                         /* 0x00414060 */

#endif
