#include <stddef.h>

#include "ACP/ACP_types.h"
#include "GAM/GAM_types.h"
#include "MEC/MEC.h"
#include "MEC/MECTools.h"
#include "MEC/MECOpt.h"
#include "MEC/MECBhv.h"
#include "MEC/MECPfm.h"
#include "DNM/DNM.h"
#include "DNM/DNM_types.h"
#include "HIE/HIE_types.h"
#include "MTH/MTH_flt.h"
#include "MTH/MTH3D.h"
#include "POS/POS_types.h"

#ifndef MTH_M_xDoubleToReal
#define MTH_M_xDoubleToReal(_x)     ((MTH_tdxReal)(_x))
#endif
#ifndef MTH_M_xLongToReal
#define MTH_M_xLongToReal(_l)       ((MTH_tdxReal)(_l))
#endif

#define MTH_M_MoveSetNull(_pMove)                              \
    do {                                                       \
        MTH3D_M_vNullVector(&(_pMove)->m_stLinear);            \
        (_pMove)->m_stAngular.m_xAngle = MTH_C_ZERO;           \
        MTH3D_M_vNullVector(&(_pMove)->m_stAngular.m_stAxis);  \
    } while (0)
#define MTH_M_MoveSetLinear(_pMove, _pVec) \
    MTH3D_M_vCopyVector(&(_pMove)->m_stLinear, _pVec)
#define MTH_M_MoveSetAngularToNull(_pMove)                     \
    do {                                                       \
        (_pMove)->m_stAngular.m_xAngle = MTH_C_ZERO;           \
        MTH3D_M_vNullVector(&(_pMove)->m_stAngular.m_stAxis);  \
    } while (0)
#define MTH_M_MoveSetAngularPosition(_pMove, _xAngle, _pAxisPtr) \
    do {                                                          \
        (_pMove)->m_stAngular.m_xAngle = (_xAngle);               \
        MTH3D_M_vCopyVector(&(_pMove)->m_stAngular.m_stAxis,      \
                            (_pAxisPtr));                         \
    } while (0)

#define C_ulDNM_RAYMAN_CUSTOM_BIT    (0x80000000UL)
#define C_ulDNM_RAYMAN_ZDM_RADIUS    MTH_M_xDoubleToReal(0.9)
#define C_ulDNM_COMPUTE_CHANNEL      (0x2UL)
#define C_xDNM_BASE_COEF             MTH_C_ZERO
#define C_xDNM_WALK_COEF             MTH_C_ZERO

#define C_WOT_ulNoObstacle           0x00000000UL
#define C_WOT_ulGround               0x00000001UL
#define C_WOT_ulMobile               0x00010000UL
#define C_WOT_ulForceMobile          0x00000080UL

/* 0x0050A9A0 */
DNM_tdstMecReport g_stReport;

MTH_tdxReal MEC_g_xBaseCoeff;   /* 0x004B72C4 */
MTH_tdxReal MEC_g_xWalkCoeff;   /* 0x004B72C8 */

POS_tdstCompletePosition *POS_g_p_stIdentityMatrix;   /* 0x00510BA0 */

/* 0x004A10D4 */
MTH_tdxReal MEC_g_xClimbSpeedLimit = 0.7f;

typedef ACP_tdxBool       (__cdecl *t_fn_bGetCharForceCollNotMoving)(HIE_tdstSuperObject *);
typedef unsigned char     (__cdecl *t_fn_ucCollSetGetFreq)(HIE_tdstSuperObject *);
typedef ACP_tdxBool       (__cdecl *t_fn_bStandardGameIsAPlatform)(HIE_tdstSuperObject *);
typedef unsigned long     (__cdecl *t_fn_ulStandardGameGetCustomBits)(HIE_tdstSuperObject *);

/* 0x417A80 */
extern ACP_tdxBool   __cdecl fn_bGetCharacterForceCollisionWhenNotMoving(void *hSuperObject);
extern unsigned char __cdecl fn_ucCollSetGetCollComputeFrequencySO(void *hSuperObject);
#define fn_RT_bGetCharacterForceCollisionWhenNotMoving \
    ((t_fn_bGetCharForceCollNotMoving)&fn_bGetCharacterForceCollisionWhenNotMoving)
#define fn_RT_ucCollSetGetCollComputeFrequencySO \
    ((t_fn_ucCollSetGetFreq)&fn_ucCollSetGetCollComputeFrequencySO)

extern char __cdecl fn_bStandardGameIsAPlatform(void *hSupObj);
extern unsigned long __cdecl fn_ulStandardGameGetCustomBitsSO(void *hSupObj);
#define fn_RT_bStandardGameIsAPlatform \
    ((t_fn_bStandardGameIsAPlatform)fn_bStandardGameIsAPlatform)
#define fn_RT_ulStandardGameGetCustomBitsSO \
    ((t_fn_ulStandardGameGetCustomBits)fn_ulStandardGameGetCustomBitsSO)

static __inline void POS_fn_vForcedRotation(POS_tdxHandleToPosition _hPos,
                                            MTH3D_tdstMatrix *_pRotation)
{
    _hPos->stRotationMatrix = *_pRotation;
}

typedef ACP_tdxBool   (__cdecl *t_MEC_fn_bNoMove)(DNM_tdstDynamics *);
typedef void          (__cdecl *t_MEC_fn_vSetInitialFrameTrans)(DNM_tdstDynamics *, MTH3D_tdstVector *);
typedef void          (__cdecl *t_MEC_fn_vImposeRotation)(DNM_tdstDynamics *);
typedef void          (__cdecl *t_MEC_fn_vBaseManagerExt)(DNM_tdstDynamics *, HIE_tdstSuperObject *, MTH_tdxReal);
typedef void          (__cdecl *t_MEC_fn_vComputeSpeed)(DNM_tdstDynamics *, MTH_tdxReal);
typedef void          (__cdecl *t_MEC_fn_vCorrectMatrix)(DNM_tdstDynamics *, POS_tdxHandleToPosition);
typedef ACP_tdxBool   (__cdecl *t_MEC_fn_bIsEqualMatrix)(POS_tdxHandleToPosition, POS_tdxHandleToPosition);
typedef void          (__cdecl *t_MEC_fn_vUpdatePlatform)(DNM_tdstDynamics *, HIE_tdstSuperObject *, POS_tdxHandleToPosition, POS_tdxHandleToPosition);
typedef void          (__cdecl *t_MEC_fn_vInitMatrix)(DNM_tdstDynamics *);
typedef void          (__cdecl *t_MEC_fn_vFillCollisionReport)(DNM_tdstDynamics *);
typedef void          (__cdecl *t_MEC_fn_vAdjustScaleData)(DNM_tdstDynamics *, POS_tdxHandleToPosition, POS_tdxHandleToPosition);
typedef void          (__cdecl *t_MEC_fn_vTranslateMatrixOn)(DNM_tdstDynamics *, MTH3D_tdstVector *);
typedef void          (__cdecl *t_MEC_fn_vTranslateMatrixOff)(DNM_tdstDynamics *, MTH3D_tdstVector *);
typedef void          (__cdecl *t_MEC_fn_vProposeSpeed)(DNM_tdstDynamics *, MTH_tdxReal, POS_tdxHandleToPosition, POS_tdxHandleToPosition);
typedef void          (__cdecl *t_MEC_fn_vAddStream)(DNM_tdstDynamics *, MTH_tdxReal, ACP_tdxBool);
typedef void          (__cdecl *t_MEC_fn_vInertia)(DNM_tdstDynamics *, MTH_tdxReal, POS_tdxHandleToPosition, POS_tdxHandleToPosition);
typedef void          (__cdecl *t_MEC_fn_vGravitySpeed)(DNM_tdstDynamics *, MTH_tdxReal);
typedef void          (__cdecl *t_MEC_fn_vAddSpeed)(DNM_tdstDynamics *, MTH_tdxReal, POS_tdxHandleToPosition, POS_tdxHandleToPosition);
typedef void          (__cdecl *t_MEC_fn_vAdditionnalTranslation)(DNM_tdstDynamics *);
typedef void          (__cdecl *t_MEC_fn_vSpeedLimit)(DNM_tdstDynamics *, MTH_tdxReal, POS_tdxHandleToPosition, POS_tdxHandleToPosition);
typedef void          (__cdecl *t_MEC_fn_vComputeInertiaTrans)(DNM_tdstDynamics *, MTH_tdxReal);
typedef void          (__cdecl *t_MEC_fn_vAdjustSpeed)(DNM_tdstDynamics *, MTH_tdxReal);
typedef ACP_tdxBool   (__cdecl *t_MEC_fn_bDynamicsNeedCollide)(DNM_tdstDynamics *);
typedef void          (__cdecl *t_MEC_fn_vObstacleMgmt)(DNM_tdstDynamics *, HIE_tdstSuperObject *, MTH_tdxReal);
typedef void          (__cdecl *t_MEC_fn_vGroundGuess)(DNM_tdstDynamics *, MTH_tdxReal);
typedef void          (__cdecl *t_MEC_fn_vPositionLimit)(DNM_tdstDynamics *, MTH3D_tdstVector *);
typedef ACP_tdxBool   (__cdecl *t_MEC_fn_vBasePlatformMgmt)(DNM_tdstDynamics *, HIE_tdstSuperObject *);
typedef MTH_tdxReal   (__cdecl *t_MEC_fn_xGetSlide)(DNM_tdstDynamics *, DNM_tdstMecObstacle *);
typedef void          (__cdecl *t_MEC_fn_vAddTiltSpeed)(DNM_tdstDynamics *, MTH_tdxReal);

typedef void          (__cdecl *t_MEC_fn_vSlowMode)(DNM_tdstDynamics *, long);
typedef void          (__cdecl *t_MEC_fn_vRepeatMove)(DNM_tdstDynamics *);
typedef void          (__cdecl *t_MEC_fn_vNormalMode)(DNM_tdstDynamics *, long);
typedef void          (__cdecl *t_MEC_fn_vObstacleMgmtBase)(DNM_tdstDynamics *, HIE_tdstSuperObject *, MTH_tdxReal);
typedef void          (__cdecl *t_MEC_fn_vObstacleMgmtGround)(DNM_tdstDynamics *, HIE_tdstSuperObject *, MTH_tdxReal);

#define MEC_RT_fn_bNoMove                       ((t_MEC_fn_bNoMove)MEC_fn_bNoMove)

#define MEC_RT_fn_vSetInitialFrameTranslation   ((t_MEC_fn_vSetInitialFrameTrans)MEC_fn_vSetInitialFrameTranslation)

#define MEC_RT_fn_vImposeRotation               ((t_MEC_fn_vImposeRotation)MEC_fn_vImposeRotation)

#define MEC_RT_fn_vComputeSpeed                 ((t_MEC_fn_vComputeSpeed)MEC_fn_vComputeSpeed)

#define MEC_RT_fn_vCorrectMatrix                ((t_MEC_fn_vCorrectMatrix)MEC_fn_vCorrectMatrix)

#define MEC_RT_fn_bIsEqualMatrix                ((t_MEC_fn_bIsEqualMatrix)MEC_fn_bIsEqualMatrix)

#define MEC_RT_fn_vUpdatePlatform               ((t_MEC_fn_vUpdatePlatform)MEC_fn_vUpdatePlatform)

#define MEC_RT_fn_vInitMatrix                   ((t_MEC_fn_vInitMatrix)MEC_fn_vInitMatrix)

#define MEC_RT_fn_vFillCollisionReport          ((t_MEC_fn_vFillCollisionReport)MEC_fn_vFillCollisionReport)

#define MEC_RT_fn_vAdjustScaleData              ((t_MEC_fn_vAdjustScaleData)MEC_fn_vAdjustScaleData)

#define MEC_RT_fn_vTranslateMatrixOn            ((t_MEC_fn_vTranslateMatrixOn)MEC_fn_vTranslateMatrixOn)

#define MEC_RT_fn_vProposeSpeed                 ((t_MEC_fn_vProposeSpeed)MEC_fn_vProposeSpeed)

#define MEC_RT_fn_vAddStream                    ((t_MEC_fn_vAddStream)MEC_fn_vAddStream)

#define MEC_RT_fn_vInertia                      ((t_MEC_fn_vInertia)MEC_fn_vInertia)

#define MEC_RT_fn_vGravitySpeed                 ((t_MEC_fn_vGravitySpeed)MEC_fn_vGravitySpeed)

#define MEC_RT_fn_vAddSpeed                     ((t_MEC_fn_vAddSpeed)MEC_fn_vAddSpeed)

#define MEC_RT_fn_vAdditionnalTranslation       ((t_MEC_fn_vAdditionnalTranslation)MEC_fn_vAdditionnalTranslation)

#define MEC_RT_fn_vSpeedLimit                   ((t_MEC_fn_vSpeedLimit)MEC_fn_vSpeedLimit)

#define MEC_RT_fn_vComputeInertiaTranslation    ((t_MEC_fn_vComputeInertiaTrans)MEC_fn_vComputeInertiaTranslation)

#define MEC_RT_fn_vAddTiltSpeed                 ((t_MEC_fn_vAddTiltSpeed)MEC_fn_vAddTiltSpeed)

#define MEC_RT_fn_bDynamicsNeedCollide          ((t_MEC_fn_bDynamicsNeedCollide)MEC_fn_bDynamicsNeedCollide)

#define MEC_RT_fn_vGiObstacleManagement         ((t_MEC_fn_vObstacleMgmt)MEC_fn_vGiObstacleManagement)

#define MEC_RT_fn_vClimbObstacleManagement      ((t_MEC_fn_vObstacleMgmt)MEC_fn_vClimbObstacleManagement)

#define MEC_RT_fn_vSpiderObstacleManagement     ((t_MEC_fn_vObstacleMgmt)MEC_fn_vSpiderObstacleManagement)

#define MEC_RT_fn_vGroundGuess                  ((t_MEC_fn_vGroundGuess)MEC_fn_vGroundGuess)

#define MEC_RT_fn_vPositionLimit                ((t_MEC_fn_vPositionLimit)MEC_fn_vPositionLimit)

#define MEC_RT_fn_vBasePlatformManagement       ((t_MEC_fn_vBasePlatformMgmt)MEC_fn_vBasePlatformManagement)

#define MEC_RT_fn_xGetSlide                     ((t_MEC_fn_xGetSlide)MEC_fn_xGetSlide)

#define MEC_RT_fn_vSlowMode                     ((t_MEC_fn_vSlowMode)MEC_fn_vSlowMode)

#define MEC_RT_fn_vRepeatMove                   ((t_MEC_fn_vRepeatMove)MEC_fn_vRepeatMove)

#define MEC_RT_fn_vNormalMode                   ((t_MEC_fn_vNormalMode)MEC_fn_vNormalMode)

#define MEC_RT_fn_vBaseObstacleManagement       ((t_MEC_fn_vObstacleMgmtBase)MEC_fn_vBaseObstacleManagement)

#define MEC_RT_fn_vGroundObstacleManagement     ((t_MEC_fn_vObstacleMgmtGround)MEC_fn_vGroundObstacleManagement)

static const MTH_tdxReal MEC_gs_xScaleEpsilon = 0.001f;   /* 0x0049C6F0 */
#define MEC_C_xScaleEpsilon          (MEC_gs_xScaleEpsilon)

static __inline ACP_tdxBool MEC_fn_bIsScale(DNM_tdstDynamics *_p_stDynamic)
{
    MTH_tdxReal xScaleX = DNM_M_p_stDynamicsGetScaleX(_p_stDynamic);
    MTH_tdxReal xScaleY = DNM_M_p_stDynamicsGetScaleY(_p_stDynamic);
    MTH_tdxReal xScaleZ = DNM_M_p_stDynamicsGetScaleZ(_p_stDynamic);
    MTH_tdxReal xEps    = MEC_C_xScaleEpsilon;

    MTH_tdxReal xAbsX = (xScaleX < MTH_C_ONE) ? (MTH_C_ONE - xScaleX) : (xScaleX - MTH_C_ONE);
    MTH_tdxReal xAbsY = (xScaleY < MTH_C_ONE) ? (MTH_C_ONE - xScaleY) : (xScaleY - MTH_C_ONE);
    MTH_tdxReal xAbsZ = (xScaleZ < MTH_C_ONE) ? (MTH_C_ONE - xScaleZ) : (xScaleZ - MTH_C_ONE);

    return (ACP_tdxBool)((xAbsX >= xEps) || (xAbsY >= xEps) || (xAbsZ >= xEps));
}

static __inline void MEC_fn_vRegisterCollisionTranslation(DNM_tdstDynamics *_p_stDynamic,
                                                          POS_tdxHandleToPosition _hMatrix)
{
    MTH3D_tdstVector *p_stTrans = POS_fn_p_stGetTranslationVector(_hMatrix);
    MTH3D_tdstVector *p_stColTrans = DNM_M_pDynamicsGetCollisionTranslation(_p_stDynamic);
    MTH3D_M_vCopyVector(p_stColTrans, p_stTrans);
}

static long MEC_fn_lComputeCall(DNM_tdstDynamics *p_stDynamic,
                                HIE_tdstSuperObject *hSupObj,
                                MTH_tdxReal xDT, MTH_tdxReal xCoef)
{
    long lNbFrame, lCall;
    POS_tdxHandleToPosition hPreviousMatrix, hCurrentMatrix;
    MTH3D_tdstVector *p_stPreviousPosition, *p_stCurrentPosition;
    MTH3D_tdstVector stVector;

    hCurrentMatrix       = DNM_M_p_stDynamicsGetCurrentMatrix(p_stDynamic);
    hPreviousMatrix      = DNM_M_p_stDynamicsGetPreviousMatrix(p_stDynamic);
    p_stPreviousPosition = POS_fn_p_stGetTranslationVector(hPreviousMatrix);
    p_stCurrentPosition  = POS_fn_p_stGetTranslationVector(hCurrentMatrix);

    MTH3D_M_vSubVector(&stVector, p_stCurrentPosition, p_stPreviousPosition);

    if ((fn_RT_ulStandardGameGetCustomBitsSO(hSupObj) & C_ulDNM_RAYMAN_CUSTOM_BIT)
            == C_ulDNM_RAYMAN_CUSTOM_BIT)
    {
        lCall = MTH_M_xRealToLong(MTH_M_xAdd(
                    MTH_M_xMul(MTH_M_xDiv(MTH3D_M_xNormVector(&stVector),
                                          C_ulDNM_RAYMAN_ZDM_RADIUS),
                               xCoef),
                    MTH_C_ONE));
    }
    else
    {

        lNbFrame = MTH_M_xRealToLong(MTH_M_xAdd(
                    MTH_M_xDiv(xDT, g_xCachedFrameLength),
                    MTH_C_Inv2));
        lCall = 1;
        if (lNbFrame > 4 && MTH_M_bGreater(MTH3D_M_xNormVector(&stVector), MTH_C_Inv2))
            lCall = 2;
    }

    return lCall;
}

static void MEC_fn_vBaseManager(DNM_tdstDynamics *p_stDynamic,
                                HIE_tdstSuperObject *hSupObj,
                                MTH_tdxReal xDT)
{
    long lCall, lCounter;

    if (DNM_M_bDynamicsIsDontUseNewMechanic(p_stDynamic) ||
        MTH_M_bEqualZero(MEC_g_xBaseCoeff))
    {
        MEC_RT_fn_vBaseObstacleManagement(p_stDynamic, hSupObj, xDT);
    }
    else
    {
        lCall = MEC_fn_lComputeCall(p_stDynamic, hSupObj, xDT, MEC_g_xBaseCoeff);
        MEC_RT_fn_vSlowMode(p_stDynamic, lCall);
        MEC_RT_fn_vBaseObstacleManagement(p_stDynamic, hSupObj, xDT);
        for (lCounter = 1; lCounter < lCall; lCounter++)
        {
            MEC_RT_fn_vRepeatMove(p_stDynamic);
            MEC_RT_fn_vBaseObstacleManagement(p_stDynamic, hSupObj, xDT);
        }
        MEC_RT_fn_vNormalMode(p_stDynamic, lCall);
    }
}

static void MEC_fn_vWalkManager(DNM_tdstDynamics *p_stDynamic,
                                HIE_tdstSuperObject *hSupObj,
                                MTH_tdxReal xDT)
{
    long lCall, lCounter;

    if (DNM_M_bDynamicsIsDontUseNewMechanic(p_stDynamic) ||
        MTH_M_bEqualZero(MEC_g_xWalkCoeff))
    {
        MEC_RT_fn_vGroundObstacleManagement(p_stDynamic, hSupObj, xDT);
    }
    else
    {
        lCall = MEC_fn_lComputeCall(p_stDynamic, hSupObj, xDT, MEC_g_xWalkCoeff);
        MEC_RT_fn_vSlowMode(p_stDynamic, lCall);
        MEC_RT_fn_vGroundObstacleManagement(p_stDynamic, hSupObj, xDT);
        for (lCounter = 1; lCounter < lCall; lCounter++)
        {
            g_lCallCounter = 0;
            MEC_RT_fn_vRepeatMove(p_stDynamic);
            MEC_RT_fn_vGroundObstacleManagement(p_stDynamic, hSupObj, xDT);
        }
        MEC_RT_fn_vNormalMode(p_stDynamic, lCall);
    }
}

DNM_tdstDynamics * __cdecl MEC_p_stDynamicsBaseMechanics(
    DNM_tdstDynamics    *p_stDynamic,
    HIE_tdstSuperObject *hSupObj,
    DNM_tdstParsingDatas *p_stParsingDatas,
    void                *h_MecIdCard,
    MTH_tdxReal          xDT)
{

    POS_tdxHandleToPosition hGlobalMatrix;
    POS_tdxHandleToPosition hCurrentMatrix;
    POS_tdxHandleToPosition hPreviousMatrix;
    POS_tdxHandleToPosition hPrevPreviousMatrix       = NULL;
    POS_tdxHandleToPosition hAbsolutePreviousMatrix   = NULL;
    POS_tdxHandleToPosition hPreviousReferenceMatrix;
    POS_tdxHandleToPosition hInvPreviousReferenceMatrix;
    POS_tdxHandleToPosition hPrevPreviousReferenceMatrix;
    POS_tdxHandleToPosition hRefMatrix;
    POS_tdxHandleToPosition hInvRefMatrix;

    POS_tdstCompletePosition stPreviousReferenceMatrix;
    POS_tdstCompletePosition stInvPreviousReferenceMatrix;
    POS_tdstCompletePosition stPrevPreviousReferenceMatrix;
    POS_tdstCompletePosition stInvMatrix;
    POS_tdstCompletePosition stRefMatrix;
    MTH3D_tdstVector        *p_stCurrentTranslation;
    MTH3D_tdstVector        *p_stAbsolutePreviousTranslation;
    MTH3D_tdstVector         stSpeed;
    MTH3D_tdstVector         stStablePreviousPosition;
    MTH3D_tdstVector        *p_stPreviousX, *p_stPreviousY, *p_stPreviousZ;
    DNM_tdstReport          *p_stReport = NULL;
    ACP_tdxBool              bNeedToCollide;
    ACP_tdxBool              bPlatform       = 0;
    ACP_tdxBool              bPlatformChange = 0;
    ACP_tdxBool              bImposeRotation;
    MTH3D_tdstVector         stI, stJ, stK;
    long                     lNbFrame, lFrame;
    MTH_tdxReal              fFrameLength;
    MTH_tdstMove             stMove;
    MTH3D_tdstVector         stVector;
    MTH3D_tdstVector         stMatrixTranslation;
    DNM_tdstMecObstacle     *p_stObstacle;

    (void)p_stParsingDatas;

    if (DNM_M_bDynamicsIsComplexSize(p_stDynamic) &&
        DNM_M_bDynamicsIsCollisionReport(p_stDynamic))
        MTH_M_MoveSetNull(&stMove);

    if (DNM_M_bDynamicsIsCollisionReport(p_stDynamic))
    {
        p_stReport = DNM_M_p_stDynamicsGetReport(p_stDynamic);

        if (DNM_M_ulReportGetSurfaceState(p_stReport) & C_WOT_ulForceMobile)
            DNM_M_ulReportSetSurfaceState(p_stReport, C_WOT_ulMobile);
        else
            DNM_M_ulReportSetSurfaceState(p_stReport, C_WOT_ulNoObstacle);
    }

    DNM_M_vDynamicsSetExec(p_stDynamic, 1  );
    DNM_M_vDynamicsSetActorMove(p_stDynamic, 0  );

    if (!fn_RT_bGetCharacterForceCollisionWhenNotMoving(hSupObj) &&
        MEC_RT_fn_bNoMove(p_stDynamic) &&
        !DNM_M_bDynamicsIsInit(p_stDynamic) &&
        !DNM_M_bDynamicsIsChangeScale(p_stDynamic))
    {
        return p_stDynamic;
    }

    (void)h_MecIdCard;
    hCurrentMatrix  = DNM_M_p_stDynamicsGetCurrentMatrix(p_stDynamic);
    hPreviousMatrix = DNM_M_p_stDynamicsGetPreviousMatrix(p_stDynamic);

    if (DNM_M_bDynamicsIsComplexSize(p_stDynamic))
    {
        hPrevPreviousMatrix     = DNM_M_p_stDynamicsGetPrevPreviousMatrix(p_stDynamic);
        hAbsolutePreviousMatrix = DNM_M_p_stDynamicsGetAbsolutePreviousMatrix(p_stDynamic);
    }

    POS_fn_vCopyMatrix(&stPreviousReferenceMatrix, POS_g_p_stIdentityMatrix);
    hPreviousReferenceMatrix = &stPreviousReferenceMatrix;

    POS_fn_vCopyMatrix(&stPrevPreviousReferenceMatrix, POS_g_p_stIdentityMatrix);
    hPrevPreviousReferenceMatrix = &stPrevPreviousReferenceMatrix;

    hGlobalMatrix = HIE_fn_hGetSuperObjectGlobalMatrix(hSupObj);
    POS_fn_vCopyMatrix(&stRefMatrix, POS_g_p_stIdentityMatrix);

    POS_fn_vCopyMatrix(&stInvMatrix, POS_g_p_stIdentityMatrix);
    hRefMatrix    = &stRefMatrix;
    hInvRefMatrix = &stInvMatrix;
    DNM_M_vDynamicsSetComputeInvertMatrix(p_stDynamic, 1  );

    fFrameLength = g_xCachedFrameLength;
    lNbFrame = MTH_M_xRealToLong(MTH_M_xAdd(MTH_M_xDiv(xDT, fFrameLength), MTH_C_Inv2));
    xDT      = MTH_M_xMul(fFrameLength, MTH_M_xLongToReal(lNbFrame));

    DNM_M_ulReportSetSurfaceState(&g_stReport, 0);
    g_lCallCounter = 0;
    g_bHighSpeed   = 0;

    if (DNM_M_bDynamicsIsInit(p_stDynamic))
    {
        POS_fn_vCopyMatrix(hCurrentMatrix,  hGlobalMatrix);
        POS_fn_vCopyMatrix(hPreviousMatrix, hGlobalMatrix);

        if (DNM_M_bDynamicsIsComplexSize(p_stDynamic))
        {
            POS_fn_vCopyMatrix(hPrevPreviousMatrix,     hGlobalMatrix);
            POS_fn_vCopyMatrix(hAbsolutePreviousMatrix, hGlobalMatrix);
        }

        if (DNM_M_bDynamicsIsAdvancedSize(p_stDynamic))
            MEC_fn_vRegisterCollisionTranslation(p_stDynamic, hCurrentMatrix);

        if (!DNM_M_bDynamicsIsChangeScale(p_stDynamic))
        {
            POS_fn_vGetScaleMatrix(hGlobalMatrix, &stI, &stJ, &stK);
            DNM_M_vDynamicsSetScale(p_stDynamic,
                                    MTH3D_M_xGetXofVector(&stI),
                                    MTH3D_M_xGetYofVector(&stJ),
                                    MTH3D_M_xGetZofVector(&stK));
        }
        MTH3D_M_vNullVector(DNM_M_pDynamicsGetPreviousSpeed(p_stDynamic));

        DNM_M_vDynamicsSetInit(p_stDynamic, 0  );
    }

    if (DNM_M_bDynamicsIsMechanicChange(p_stDynamic) &&
        DNM_M_bDynamicsIsComplexSize(p_stDynamic))
        MEC_fn_vCancelTilt(p_stDynamic);

    POS_fn_vCopyMatrix(hRefMatrix, hPreviousMatrix);
    if (DNM_M_bDynamicsIsImposeRotation(p_stDynamic))
        POS_fn_vForcedRotation(hRefMatrix,
                               DNM_M_p_stDynamicsGetImposeRotationMatrix(p_stDynamic));

    if (DNM_M_bDynamicsIsAdvancedSize(p_stDynamic))
    {
        if (DNM_M_cDynamicsGetCollideCounter(p_stDynamic) <= 0)
            DNM_M_vDynamicsSetCollideCounter(p_stDynamic,
                fn_RT_ucCollSetGetCollComputeFrequencySO(hSupObj));
    }

    if (DNM_M_bDynamicsIsShoot(p_stDynamic))
    {

        bImposeRotation = (ACP_tdxBool)DNM_M_bDynamicsIsImposeRotation(p_stDynamic);

        MTH3D_M_vMulScalarVector(&stVector, xDT,
                                 DNM_M_pDynamicsGetPreviousSpeed(p_stDynamic));
        MEC_RT_fn_vSetInitialFrameTranslation(p_stDynamic, &stVector);

        POS_fn_vCopyMatrix(hPreviousReferenceMatrix, hPreviousMatrix);

        if (DNM_M_bDynamicsIsImposeSpeed(p_stDynamic))
        {
            MEC_fn_vImposeSpeed(p_stDynamic, xDT, hRefMatrix, hInvRefMatrix);
            DNM_M_vDynamicsSetImposeSpeedX(p_stDynamic, 0);
            DNM_M_vDynamicsSetImposeSpeedY(p_stDynamic, 0);
            DNM_M_vDynamicsSetImposeSpeedZ(p_stDynamic, 0);
        }

        if (bImposeRotation)
        {
            MEC_RT_fn_vImposeRotation(p_stDynamic);
            DNM_M_vDynamicsSetImposeRotation(p_stDynamic, 0);
        }

        if (DNM_M_bDynamicsIsCollide(p_stDynamic))
        {
            if (DNM_M_bDynamicsIsAdvancedSize(p_stDynamic))
            {
                DNM_M_vDynamicsDecCollideCounter(p_stDynamic);

                if (DNM_M_cDynamicsGetCollideCounter(p_stDynamic) == 0)
                {

                    MEC_fn_vBaseManager(p_stDynamic, hSupObj, xDT);
                    if (fn_RT_ucCollSetGetCollComputeFrequencySO(hSupObj) > 1)
                        MEC_fn_vRegisterCollisionTranslation(p_stDynamic, hCurrentMatrix);
                }
            }
            else
            {

                MEC_fn_vBaseManager(p_stDynamic, hSupObj, xDT);
            }
        }

        MEC_RT_fn_vComputeSpeed(p_stDynamic, xDT);

        MEC_RT_fn_vCorrectMatrix(p_stDynamic, hCurrentMatrix);

        if (!MEC_RT_fn_bIsEqualMatrix(hGlobalMatrix, hCurrentMatrix))
        {
            POS_fn_vCopyMatrix(hGlobalMatrix, hCurrentMatrix);
            HIE_fn_vComputeNewRelativeMatrix(hSupObj);
        }

        if (fn_RT_bStandardGameIsAPlatform(hSupObj))
        {
            if (!MEC_RT_fn_bIsEqualMatrix(hPreviousMatrix, hCurrentMatrix))
            {
                HIE_fn_vRefreshHierarchy(hSupObj);

                if (bImposeRotation)
                {
                    hInvPreviousReferenceMatrix = &stInvPreviousReferenceMatrix;
                    POS_fn_vInvertMatrix(hInvPreviousReferenceMatrix,
                                         hPreviousReferenceMatrix);
                }
                else
                {

                    POS_fn_vInvertMatrix(hInvRefMatrix, hRefMatrix);
                    hInvPreviousReferenceMatrix = hInvRefMatrix;
                }

                MEC_RT_fn_vUpdatePlatform(p_stDynamic, hSupObj,
                                          hPreviousReferenceMatrix,
                                          hInvPreviousReferenceMatrix);
            }
        }
        else if ((fn_RT_ulStandardGameGetCustomBitsSO(hSupObj) & C_ulDNM_COMPUTE_CHANNEL) &&
                 !MEC_RT_fn_bIsEqualMatrix(hPreviousMatrix, hCurrentMatrix))
        {
            HIE_fn_vRefreshHierarchy(hSupObj);
        }

        MEC_RT_fn_vInitMatrix(p_stDynamic);
        MEC_RT_fn_vFillCollisionReport(p_stDynamic);
        return p_stDynamic;
    }

    POS_fn_vGetTranslationVector(hPreviousMatrix, &stStablePreviousPosition);

    POS_fn_vCopyMatrix(hPreviousReferenceMatrix, hPreviousMatrix);
    if (DNM_M_bDynamicsIsComplexSize(p_stDynamic))
        POS_fn_vCopyMatrix(hPrevPreviousReferenceMatrix, hPrevPreviousMatrix);

    DNM_M_bDynamicsSetPlatformCrash(p_stDynamic, 0);
    DNM_M_bDynamicsSetCanFall(p_stDynamic, 0);

    if (DNM_M_bDynamicsIsScale(p_stDynamic) && MEC_fn_bIsScale(p_stDynamic))
        MEC_RT_fn_vAdjustScaleData(p_stDynamic, hRefMatrix, hInvRefMatrix);

    MTH3D_M_vMulScalarVector(DNM_M_p_stDynamicsGetSpeedAnim(p_stDynamic),
                             fFrameLength,
                             DNM_M_p_stDynamicsGetSpeedAnim(p_stDynamic));
    POS_fn_vRotateVector(DNM_M_p_stDynamicsGetSpeedAnim(p_stDynamic),
                         hCurrentMatrix,
                         DNM_M_p_stDynamicsGetSpeedAnim(p_stDynamic));

    MTH3D_M_vDivScalarVector(DNM_M_pDynamicsGetAddTranslation(p_stDynamic),
                             DNM_M_pDynamicsGetAddTranslation(p_stDynamic),
                             MTH_M_xLongToReal(lNbFrame));

    MEC_RT_fn_vTranslateMatrixOn(p_stDynamic, &stMatrixTranslation);

    for (lFrame = 0; lFrame < lNbFrame; lFrame++)
    {
        if (DNM_M_bDynamicsIsAnimation(p_stDynamic))
            MEC_RT_fn_vSetInitialFrameTranslation(p_stDynamic,
                DNM_M_p_stDynamicsGetSpeedAnim(p_stDynamic));

        if (DNM_M_bDynamicsIsProposeSpeed(p_stDynamic))
            MEC_RT_fn_vProposeSpeed(p_stDynamic, fFrameLength, hRefMatrix, hInvRefMatrix);

        if (DNM_M_bDynamicsIsStream(p_stDynamic))
        {
            if (DNM_M_xDynamicsGetStreamPriority(p_stDynamic) == MTH_C_ZERO)
                MEC_RT_fn_vAddStream(p_stDynamic, fFrameLength, 1  );
        }

        MEC_RT_fn_vInertia(p_stDynamic, fFrameLength, hRefMatrix, hInvRefMatrix);

        if (DNM_M_bDynamicsIsGravity(p_stDynamic))
            MEC_RT_fn_vGravitySpeed(p_stDynamic, fFrameLength);

        if (DNM_M_bDynamicsIsImposeSpeed(p_stDynamic))
            MEC_fn_vImposeSpeed(p_stDynamic, fFrameLength, hRefMatrix, hInvRefMatrix);

        if (DNM_M_bDynamicsIsAddSpeed(p_stDynamic))
            MEC_RT_fn_vAddSpeed(p_stDynamic, fFrameLength, hRefMatrix, hInvRefMatrix);

        if (DNM_M_bDynamicsIsImposeRotation(p_stDynamic))
        {
            MEC_RT_fn_vImposeRotation(p_stDynamic);
            MEC_RT_fn_vAdditionnalTranslation(p_stDynamic);
        }
        else if (DNM_M_bDynamicsIsImposeTranslation(p_stDynamic))
            MEC_RT_fn_vAdditionnalTranslation(p_stDynamic);

        if (DNM_M_bDynamicsIsStream(p_stDynamic))
        {
            if (DNM_M_xDynamicsGetStreamPriority(p_stDynamic) == MTH_C_ONE)
                MEC_RT_fn_vAddStream(p_stDynamic, fFrameLength, 0  );
        }

        if (DNM_M_bDynamicsIsSpeedLimit(p_stDynamic))
            MEC_RT_fn_vSpeedLimit(p_stDynamic, fFrameLength, hRefMatrix, hInvRefMatrix);

        MEC_RT_fn_vComputeSpeed(p_stDynamic, fFrameLength);

        if (DNM_M_bDynamicsIsAdvancedSize(p_stDynamic))
            MEC_RT_fn_vComputeInertiaTranslation(p_stDynamic, fFrameLength);

        if (DNM_M_bDynamicsIsComplexSize(p_stDynamic))
            POS_fn_vCopyMatrix(hPrevPreviousMatrix, hPreviousMatrix);
        POS_fn_vCopyMatrix(hPreviousMatrix, hCurrentMatrix);

        if (MTH3D_M_bIsNullVector(DNM_M_pDynamicsGetPreviousSpeed(p_stDynamic)))
            lFrame = lNbFrame;
    }

    if (DNM_M_bDynamicsIsComplexSize(p_stDynamic))
        POS_fn_vCopyMatrix(hPrevPreviousMatrix, hPrevPreviousReferenceMatrix);
    POS_fn_vCopyMatrix(hPreviousMatrix, hPreviousReferenceMatrix);

    /* 0x00437A40 */
    MEC_fn_vTranslateMatrixOff(p_stDynamic, &stMatrixTranslation);

    /* 0x00436EE0 */
    MEC_fn_vAdjustSpeed(p_stDynamic, xDT);
    if (DNM_M_bDynamicsIsTilt(p_stDynamic))
        MEC_RT_fn_vAddTiltSpeed(p_stDynamic, xDT);

    bNeedToCollide = (ACP_tdxBool)MEC_RT_fn_bDynamicsNeedCollide(p_stDynamic);

    if (DNM_M_bDynamicsIsComplexSize(p_stDynamic))
        bPlatform = MEC_fn_bIsPlatform(p_stDynamic);

    DNM_M_vDynamicsSetNoGravity(p_stDynamic, 0);

    if (DNM_M_bDynamicsIsAdvancedSize(p_stDynamic))
    {

        DNM_M_vDynamicsDecCollideCounter(p_stDynamic);

        if (DNM_M_cDynamicsGetCollideCounter(p_stDynamic) == 0)
        {
            if (bNeedToCollide || bPlatform ||
                fn_RT_bGetCharacterForceCollisionWhenNotMoving(hSupObj))
            {
                if (DNM_M_bDynamicsIsGi(p_stDynamic))
                    MEC_RT_fn_vGiObstacleManagement(p_stDynamic, hSupObj, xDT);
                else if (DNM_M_bDynamicsIsClimb(p_stDynamic))
                {
                    MEC_RT_fn_vClimbObstacleManagement(p_stDynamic, hSupObj, xDT);

                    if (MTH_M_bLess(MTH3D_M_xNormVector(
                                        DNM_M_pDynamicsGetPreviousSpeed(p_stDynamic)),
                                    MEC_g_xClimbSpeedLimit))
                    {
                        MTH3D_M_vCopyVector(
                            POS_fn_p_stGetTranslationVector(hCurrentMatrix),
                            POS_fn_p_stGetTranslationVector(hPreviousMatrix));
                        MTH3D_M_vNullVector(DNM_M_pDynamicsGetPreviousSpeed(p_stDynamic));
                    }
                }
                else if (DNM_M_bDynamicsIsOnGround(p_stDynamic))
                    MEC_fn_vWalkManager(p_stDynamic, hSupObj, xDT);
                else if (DNM_M_bDynamicsIsSpider(p_stDynamic))
                    MEC_RT_fn_vSpiderObstacleManagement(p_stDynamic, hSupObj, xDT);
                else if (DNM_M_bDynamicsIsCollide(p_stDynamic))
                    MEC_fn_vBaseManager(p_stDynamic, hSupObj, xDT);
            }

            if (fn_RT_ucCollSetGetCollComputeFrequencySO(hSupObj) > 1)
                MEC_fn_vRegisterCollisionTranslation(p_stDynamic, hCurrentMatrix);
        }
        else
        {
            if (DNM_M_bDynamicsIsOnGround(p_stDynamic))
                MEC_RT_fn_vGroundGuess(p_stDynamic, xDT);

            if (DNM_M_bDynamicsIsCollisionReport(p_stDynamic))

                DNM_M_ulReportSetSurfaceState(p_stReport,
                    DNM_M_ulReportGetPrevSurfaceState(p_stReport));
        }
    }
    else
    {
        if (bNeedToCollide ||
            fn_RT_bGetCharacterForceCollisionWhenNotMoving(hSupObj))
        {
            if (DNM_M_bDynamicsIsGi(p_stDynamic))
                MEC_RT_fn_vGiObstacleManagement(p_stDynamic, hSupObj, xDT);
            else if (DNM_M_bDynamicsIsClimb(p_stDynamic))
            {
                MEC_RT_fn_vClimbObstacleManagement(p_stDynamic, hSupObj, xDT);
                if (MTH_M_bLess(MTH3D_M_xNormVector(
                                    DNM_M_pDynamicsGetPreviousSpeed(p_stDynamic)),
                                MEC_g_xClimbSpeedLimit))
                {
                    MTH3D_M_vCopyVector(
                        POS_fn_p_stGetTranslationVector(hCurrentMatrix),
                        POS_fn_p_stGetTranslationVector(hPreviousMatrix));
                    MTH3D_M_vNullVector(DNM_M_pDynamicsGetPreviousSpeed(p_stDynamic));
                }
            }
            else if (DNM_M_bDynamicsIsOnGround(p_stDynamic))
                MEC_fn_vWalkManager(p_stDynamic, hSupObj, xDT);
            else if (DNM_M_bDynamicsIsSpider(p_stDynamic))
                MEC_RT_fn_vSpiderObstacleManagement(p_stDynamic, hSupObj, xDT);
            else if (DNM_M_bDynamicsIsCollide(p_stDynamic))
                MEC_fn_vBaseManager(p_stDynamic, hSupObj, xDT);
        }
    }

    MEC_RT_fn_vPositionLimit(p_stDynamic, &stStablePreviousPosition);

    if (DNM_M_bDynamicsIsComplexSize(p_stDynamic))
        bPlatformChange = MEC_RT_fn_vBasePlatformManagement(p_stDynamic, hSupObj);

    MEC_RT_fn_vCorrectMatrix(p_stDynamic, hCurrentMatrix);

    p_stCurrentTranslation = POS_fn_p_stGetTranslationVector(hCurrentMatrix);

    if (DNM_M_bDynamicsIsComplexSize(p_stDynamic) &&
        DNM_M_bDynamicsIsCollisionReport(p_stDynamic))
    {
        p_stAbsolutePreviousTranslation =
            POS_fn_p_stGetTranslationVector(hAbsolutePreviousMatrix);

        MTH3D_M_vSubVector(&stSpeed,
                           p_stCurrentTranslation,
                           p_stAbsolutePreviousTranslation);
        MTH3D_M_vDivScalarVector(&stSpeed, &stSpeed, xDT);

        POS_fn_vGetRotationVector(hAbsolutePreviousMatrix,
                                  &p_stPreviousX, &p_stPreviousY, &p_stPreviousZ);

        MTH_M_MoveSetAngularPosition(&stMove, MTH_C_ONE, p_stPreviousY);
        MTH_M_MoveSetLinear(&stMove, &stSpeed);
        DNM_M_ReportSetAbsoluteCurrSpeed(
            DNM_M_p_stDynamicsGetReport(p_stDynamic), &stMove);
    }

    if (!MEC_RT_fn_bIsEqualMatrix(hGlobalMatrix, hCurrentMatrix) || bPlatformChange)
    {
        POS_fn_vCopyMatrix(hGlobalMatrix, hCurrentMatrix);
        HIE_fn_vComputeNewRelativeMatrix(hSupObj);
        DNM_M_vDynamicsSetActorMove(p_stDynamic, 1  );
    }

    if (fn_RT_bStandardGameIsAPlatform(hSupObj))
    {
        if (!MEC_RT_fn_bIsEqualMatrix(hPreviousMatrix, hCurrentMatrix))
        {
            HIE_fn_vRefreshHierarchy(hSupObj);

            if (DNM_M_bDynamicsIsImposeRotation(p_stDynamic))
            {
                hInvPreviousReferenceMatrix = &stInvPreviousReferenceMatrix;
                POS_fn_vInvertMatrix(hInvPreviousReferenceMatrix,
                                     hPreviousReferenceMatrix);
            }
            else
            {
                POS_fn_vInvertMatrix(hInvRefMatrix, hRefMatrix);
                hInvPreviousReferenceMatrix = hInvRefMatrix;
            }

            MEC_RT_fn_vUpdatePlatform(p_stDynamic, hSupObj,
                                      hPreviousReferenceMatrix,
                                      hInvPreviousReferenceMatrix);
        }
    }
    else if ((fn_RT_ulStandardGameGetCustomBitsSO(hSupObj) & C_ulDNM_COMPUTE_CHANNEL) &&
             !MEC_RT_fn_bIsEqualMatrix(hPreviousMatrix, hCurrentMatrix))
    {
        HIE_fn_vRefreshHierarchy(hSupObj);
    }

    MEC_RT_fn_vInitMatrix(p_stDynamic);

    if (DNM_M_bDynamicsIsComplexSize(p_stDynamic) &&
        DNM_M_bDynamicsIsCollisionReport(p_stDynamic))
    {
        MTH3D_M_vCopyVector(&stVector, p_stCurrentTranslation);
        MTH_M_MoveSetAngularToNull(&stMove);
        MTH_M_MoveSetLinear(&stMove, &stVector);
        DNM_M_ReportSetAbsoluteCurrPosition(
            DNM_M_p_stDynamicsGetReport(p_stDynamic), &stMove);
    }

    MEC_RT_fn_vFillCollisionReport(p_stDynamic);

    if (DNM_M_bDynamicsIsCollisionReport(p_stDynamic))
        DNM_M_ulReportSetPrevSurfaceState(p_stReport,
            DNM_M_ulReportGetSurfaceState(p_stReport));

    DNM_M_vDynamicsSetNbFrame(p_stDynamic, (unsigned char)lNbFrame);

    MTH3D_M_vNullVector(DNM_M_pDynamicsGetImposeSpeed(p_stDynamic));
    MTH3D_M_vNullVector(DNM_M_pDynamicsGetProposeSpeed(p_stDynamic));
    MTH3D_M_vNullVector(DNM_M_p_stDynamicsGetSpeedAnim(p_stDynamic));
    MTH3D_M_vNullVector(DNM_M_pDynamicsGetAddTranslation(p_stDynamic));
    DNM_M_bDynamicsSetAbsoluteImposeSpeed(p_stDynamic, 0);
    DNM_M_bDynamicsSetAbsoluteProposeSpeed(p_stDynamic, 0);
    DNM_M_bDynamicsSetAbsoluteAddSpeed(p_stDynamic, 0);
    DNM_M_vDynamicsSetImposeSpeedX(p_stDynamic, 0);
    DNM_M_vDynamicsSetImposeSpeedY(p_stDynamic, 0);
    DNM_M_vDynamicsSetImposeSpeedZ(p_stDynamic, 0);
    DNM_M_vDynamicsSetProposeSpeedX(p_stDynamic, 0);
    DNM_M_vDynamicsSetProposeSpeedY(p_stDynamic, 0);
    DNM_M_vDynamicsSetProposeSpeedZ(p_stDynamic, 0);
    DNM_M_vDynamicsSetAddSpeedX(p_stDynamic, 0);
    DNM_M_vDynamicsSetAddSpeedY(p_stDynamic, 0);
    DNM_M_vDynamicsSetAddSpeedZ(p_stDynamic, 0);
    DNM_M_vDynamicsSetImposeRotation(p_stDynamic, 0);
    DNM_M_vDynamicsSetImposeTranslation(p_stDynamic, 0);
    DNM_M_bDynamicsSetMechanicChange(p_stDynamic, 0);
    DNM_M_vDynamicsSetChangeScale(p_stDynamic, 0);

    MEC_g_xBaseCoeff = C_xDNM_BASE_COEF;
    MEC_g_xWalkCoeff = C_xDNM_WALK_COEF;

    if (DNM_M_bDynamicsIsAdvancedSize(p_stDynamic))
    {
        p_stObstacle = DNM_M_p_stReportGetObstacle(&g_stReport);
        if (p_stObstacle &&
            ((DNM_M_ulReportGetSurfaceState(&g_stReport)) != C_WOT_ulNoObstacle))
            DNM_M_vDynamicsSetPreviousSlide(p_stDynamic,
                MEC_RT_fn_xGetSlide(p_stDynamic, p_stObstacle));
        else
            DNM_M_vDynamicsSetPreviousSlide(p_stDynamic, MTH_C_ZERO);
    }

    p_stObstacle = DNM_M_p_stReportGetGround(&g_stReport);
    if (p_stObstacle &&
        ((DNM_M_ulReportGetSurfaceState(&g_stReport) & C_WOT_ulGround) == C_WOT_ulGround) &&
        (MEC_RT_fn_xGetSlide(p_stDynamic, p_stObstacle) != MTH_C_ZERO))
        DNM_M_vDynamicsSetSlidingGround(p_stDynamic, 1  );
    else
        DNM_M_vDynamicsSetSlidingGround(p_stDynamic, 0  );

    if (DNM_M_bDynamicsIsAdvancedSize(p_stDynamic))
    {
        MTH3D_M_vNullVector(DNM_M_pDynamicsGetAddSpeed(p_stDynamic));
        MTH3D_M_vNullVector(DNM_M_pDynamicsGetStreamSpeed(p_stDynamic));
    }

    return p_stDynamic;
}

/* 0x510BA0 */
void __cdecl MEC_vTurnAroundZAxis(DNM_tdstDynamics *p_stDynamic,
                                  MTH_tdxReal xAngle)
{
    POS_tdxHandleToPosition hCurrentMatrix;
    MTH3D_tdstVector stVectorZ;
    MTH3D_tdstVector stMove;

    if (DNM_M_bDynamicsIsInit(p_stDynamic)) return;
    if (MTH_M_bIsNull(xAngle)) return;

    hCurrentMatrix = DNM_M_p_stDynamicsGetCurrentMatrix(p_stDynamic);

    if (DNM_M_bDynamicsIsTilt(p_stDynamic))
    {
        MTH3D_tdstVector stI, stJ, stK;
        MTH_tdxReal xCos, xSin;
        POS_tdstCompletePosition stMatrix;
        POS_tdxHandleToPosition hMatrix = &stMatrix;

        xCos = MTH_M_xCos(xAngle);
        xSin = MTH_M_xSin(xAngle);

        MTH3D_M_vCopyVector(&stMove, POS_fn_p_stGetTranslationVector(hCurrentMatrix));

        POS_fn_vCopyMatrix(hMatrix, POS_g_p_stIdentityMatrix);
        MTH3D_M_vSetVectorElements(&stI, xCos, xSin, MTH_C_ZERO);
        MTH3D_M_vSetVectorElements(&stJ, MTH_M_xNeg(xSin), xCos, MTH_C_ZERO);
        MTH3D_M_vSetBaseKVector(&stK);

        POS_fn_vSetRotationMatrix(hMatrix, &stI, &stJ, &stK);
        POS_fn_vMulMatrixMatrix(hCurrentMatrix, hMatrix, hCurrentMatrix);

        MTH3D_M_vCopyVector(POS_fn_p_stGetTranslationVector(hCurrentMatrix), &stMove);
        return;
    }

    MTH3D_M_vCopyVector(&stMove, POS_fn_p_stGetTranslationVector(hCurrentMatrix));
    MTH3D_M_vSetVectorElements(&stVectorZ, MTH_C_ZERO, MTH_C_ZERO, MTH_C_MinusONE);
    POS_fn_vRotatePositionAroundAxis(hCurrentMatrix, &stVectorZ, xAngle);
    MTH3D_M_vCopyVector(POS_fn_p_stGetTranslationVector(hCurrentMatrix), &stMove);
}

/* 0x00433020 */
void __cdecl MEC_vTurnAbsolute(DNM_tdstDynamics *p_stDynamic,
                               MTH3D_tdstVector *p_stVector)
{
    POS_tdxHandleToPosition hCurrentMatrix;
    MTH3D_tdstVector *p_stI, *p_stJ, *p_stK;

    if (DNM_M_bDynamicsIsInit(p_stDynamic)) return;
    if (MTH3D_M_bIsNullVector(p_stVector)) return;

    hCurrentMatrix = DNM_M_p_stDynamicsGetCurrentMatrix(p_stDynamic);

    POS_fn_vGetRotationVector(hCurrentMatrix, &p_stI, &p_stJ, &p_stK);

    MTH3D_M_vNormalizeVector(p_stJ, p_stVector);
    MTH3D_M_vNegVector(p_stJ, p_stJ);
    MTH3D_M_vCrossProductVector(p_stI, p_stJ, p_stK);
    MTH3D_M_vCrossProductVector(p_stJ, p_stK, p_stI);
    MEC_fn_vUpdateMatrix(p_stDynamic, hCurrentMatrix);

    if (DNM_M_bDynamicsIsComplexSize(p_stDynamic))
        MEC_fn_vCancelTilt(p_stDynamic);
}

/* 0x004331D0 */
void __cdecl DNM_fn_vSetHangingLimit(DNM_tdstDynamics *p_stDynamic,
                                     MTH_tdxReal xValue)
{
    if (MEC_fn_bIsPlatform(p_stDynamic))
    {
        HIE_tdstSuperObject *hPlatformSO = DNM_M_hDynamicsGetPlatformSO(p_stDynamic);
        POS_tdxHandleToPosition hGlobal = HIE_fn_hGetSuperObjectGlobalMatrix(hPlatformSO);
        MTH_tdxReal xPlatformZ = MTH3D_M_xGetZofVector(
            POS_fn_p_stGetTranslationVector(hGlobal));
        DNM_M_vDynamicsSetHangingLimit(p_stDynamic, MTH_M_xSub(xValue, xPlatformZ));
    }
    else
    {
        DNM_M_vDynamicsSetHangingLimit(p_stDynamic, xValue);
    }
}
