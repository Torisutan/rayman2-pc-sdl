#include "GAM/GAM_types.h"
#include "GAM/cineinfo.h"
#include "HIE/HIE_types.h"
#include "POS/POS_types.h"
#include "MTH/MTH_flt.h"
#include "GAM/GAM_globals.h"
#include "ERM/CpaError.h"

#define E_uwGameMiniStructObjectNull ((unsigned short)0xE)
#define E_uwGameMiniStructAllocAlreadyAlloc ((unsigned short)0xD)
#define E_uwGameMiniStructNull       ((unsigned short)0xF)

/* 0x004078C0 */
extern void __cdecl fn_vGameFreeInHLM(void *p_vObject);

#define CAM_M_pCineinfo(_p) ((MS_tdxHandleToCineinfo)(_p)->h_Cineinfo)

CAM_tdstCameraConstants CAM_g_stCameraConstants;       /* 0x004FF660 */
CAM_tdstCameraConstants CAM_g_stCopyCameraConstants;   /* 0x004FF6E0 */

unsigned long CAM_fn_ulCineinfoSizeOf( void )
{
	return(sizeof(struct tdstCineinfo_));
}

unsigned long CAM_fn_ulCineinfoInternalSizeOf( void )
{
	return(sizeof(struct tdstInternalStructurCineinfo_));
}

MS_tdxHandleToCineinfo CAM_fn_hCineinfoRealAlloc( void );
MS_tdxHandleToInternalCineinfo CAM_fn_hCineinfoInternalRealAlloc();

void CAM_fn_vCineinfoAlloc(struct tdstEngineObject_ *_p_stObject)
{
	if (_p_stObject == NULL)
	{
		ERM_fn_vClearLastError(0);
		ERM_fn_vUpdateLastError(0x30, g_ucGameModuleId, 0, E_uwGameMiniStructObjectNull, -1, 0xff, 0xff, NULL);
	}

	if (CAM_M_pCineinfo(_p_stObject) != NULL)
	{
		ERM_fn_vClearLastError(0);
		ERM_fn_vUpdateLastError(0x30, g_ucGameModuleId, 0, E_uwGameMiniStructAllocAlreadyAlloc, -1, 0xff, 0xff, NULL);
	}

	_p_stObject->h_Cineinfo = CAM_fn_hCineinfoRealAlloc();
	CAM_M_pCineinfo(_p_stObject)->hInit = CAM_fn_hCineinfoInternalRealAlloc();
	CAM_M_pCineinfo(_p_stObject)->hCurrent = CAM_fn_hCineinfoInternalRealAlloc();
	CAM_M_pCineinfo(_p_stObject)->hVisibility = CAM_fn_hCineinfoInternalRealAlloc();
	CAM_M_pCineinfo(_p_stObject)->hWork = CAM_fn_hCineinfoInternalRealAlloc();

	MTH3D_M_vNullVector(&CAM_M_pCineinfo(_p_stObject)->hInit->stShiftTarget);
	MTH3D_M_vNullVector(&CAM_M_pCineinfo(_p_stObject)->hInit->stShiftPos);

	CAM_M_pCineinfo(_p_stObject)->hInit->xDistMin = (MTH_tdxReal)6.0f;
	CAM_M_pCineinfo(_p_stObject)->hInit->xDistMax = (MTH_tdxReal)6.0f;
	CAM_M_pCineinfo(_p_stObject)->hInit->xBoundDistMin = (MTH_tdxReal)2.0f;
	CAM_M_pCineinfo(_p_stObject)->hInit->xBoundDistMax = (MTH_tdxReal)10.0f;

	CAM_M_pCineinfo(_p_stObject)->hInit->xAngleAlpha = MTH_C_ZERO;
	CAM_M_pCineinfo(_p_stObject)->hInit->xAngleShiftAlpha = MTH_C_ZERO;
	CAM_M_pCineinfo(_p_stObject)->hInit->xAngleTheta = MTH_C_ZERO;
	CAM_M_pCineinfo(_p_stObject)->hInit->xAngleShiftTheta = MTH_C_ZERO;

	CAM_M_pCineinfo(_p_stObject)->hInit->xLinearSpeed = (MTH_tdxReal)200.0f;
	CAM_M_pCineinfo(_p_stObject)->hInit->xLinearIncreaseSpeed = (MTH_tdxReal)1.0f;
	CAM_M_pCineinfo(_p_stObject)->hInit->xLinearDecreaseSpeed = MTH_C_ZERO;

	CAM_M_pCineinfo(_p_stObject)->hInit->xAngularSpeed = (MTH_tdxReal)3.14f;
	CAM_M_pCineinfo(_p_stObject)->hInit->xAngularIncreaseSpeed = (MTH_tdxReal)1.0f;
	CAM_M_pCineinfo(_p_stObject)->hInit->xAngularDecreaseSpeed = MTH_C_ZERO;

	CAM_M_pCineinfo(_p_stObject)->hInit->xTargetSpeed = (MTH_tdxReal)200.0f;
	CAM_M_pCineinfo(_p_stObject)->hInit->xTargetIncreaseSpeed = (MTH_tdxReal)1.0f;
	CAM_M_pCineinfo(_p_stObject)->hInit->xTargetDecreaseSpeed = MTH_C_ZERO;

	CAM_M_pCineinfo(_p_stObject)->hInit->xFocal = (MTH_tdxReal)1.5f;

	CAM_M_pCineinfo(_p_stObject)->hInit->xZMin = MTH_C_InfinitMinus;
	CAM_M_pCineinfo(_p_stObject)->hInit->xZMax = MTH_C_InfinitPlus;

	CAM_M_pCineinfo(_p_stObject)->hInit->hSuperObjectTargeted = NULL;
	CAM_M_pCineinfo(_p_stObject)->hInit->hSecondSuperObjectTargeted = NULL;

	CAM_M_pCineinfo(_p_stObject)->hInit->uwDNMFlags = 0;
	CAM_M_pCineinfo(_p_stObject)->hInit->uwIAFlags = 0;

	CAM_M_pCineinfo(_p_stObject)->hInit->eTypeOfViewport = CAM_e_NoViewport;
	CAM_M_pCineinfo(_p_stObject)->hInit->cChannel = -1;
	CAM_M_pCineinfo(_p_stObject)->hInit->bIsActive = 0;
}

MS_tdxHandleToCineinfo CAM_fn_hCineinfoRealAlloc( void )
{
	MS_tdxHandleToCineinfo hCineinfo;
	hCineinfo = (MS_tdxHandleToCineinfo) fnp_vGameMallocInHLM(CAM_fn_ulCineinfoSizeOf());
	return(hCineinfo);
}

MS_tdxHandleToInternalCineinfo CAM_fn_hCineinfoInternalRealAlloc()
{
	MS_tdxHandleToInternalCineinfo hCineinfoInternal;
	hCineinfoInternal = (MS_tdxHandleToInternalCineinfo) fnp_vGameMallocInHLM(CAM_fn_ulCineinfoInternalSizeOf());
	return(hCineinfoInternal);
}

void CAM_fn_vCineinfoFree(struct tdstEngineObject_ *_p_stObject)
{
	if (_p_stObject == NULL)
	{
		ERM_fn_vClearLastError(0);
		ERM_fn_vUpdateLastError(0x30, g_ucGameModuleId, 0, E_uwGameMiniStructObjectNull, -1, 0xff, 0xff, NULL);
	}

	if (_p_stObject->h_Cineinfo == NULL)
	{
		ERM_fn_vClearLastError(0);
		ERM_fn_vUpdateLastError(0x30, g_ucGameModuleId, 0, E_uwGameMiniStructNull, -1, 0xff, 0xff, NULL);
	}

	if (CAM_M_pCineinfo(_p_stObject)->hInit == NULL)
	{
		ERM_fn_vClearLastError(0);
		ERM_fn_vUpdateLastError(0x30, g_ucGameModuleId, 0, E_uwGameMiniStructNull, -1, 0xff, 0xff, NULL);
	}
	fn_vGameFreeInHLM(CAM_M_pCineinfo(_p_stObject)->hInit);
	CAM_M_pCineinfo(_p_stObject)->hInit = NULL;

	if (CAM_M_pCineinfo(_p_stObject)->hCurrent == NULL)
	{
		ERM_fn_vClearLastError(0);
		ERM_fn_vUpdateLastError(0x30, g_ucGameModuleId, 0, E_uwGameMiniStructNull, -1, 0xff, 0xff, NULL);
	}
	fn_vGameFreeInHLM(CAM_M_pCineinfo(_p_stObject)->hCurrent);
	CAM_M_pCineinfo(_p_stObject)->hCurrent = NULL;

	if (CAM_M_pCineinfo(_p_stObject)->hVisibility == NULL)
	{
		ERM_fn_vClearLastError(0);
		ERM_fn_vUpdateLastError(0x30, g_ucGameModuleId, 0, E_uwGameMiniStructNull, -1, 0xff, 0xff, NULL);
	}
	fn_vGameFreeInHLM(CAM_M_pCineinfo(_p_stObject)->hVisibility);
	CAM_M_pCineinfo(_p_stObject)->hVisibility = NULL;

	if (CAM_M_pCineinfo(_p_stObject)->hWork == NULL)
	{
		ERM_fn_vClearLastError(0);
		ERM_fn_vUpdateLastError(0x30, g_ucGameModuleId, 0, E_uwGameMiniStructNull, -1, 0xff, 0xff, NULL);
	}
	fn_vGameFreeInHLM(CAM_M_pCineinfo(_p_stObject)->hWork);
	CAM_M_pCineinfo(_p_stObject)->hWork = NULL;

	fn_vGameFreeInHLM(_p_stObject->h_Cineinfo);
	_p_stObject->h_Cineinfo = NULL;
}

void __cdecl CAM_fn_vUpdateTargetPosition(struct tdstCineinfo_ *hCineinfo);

/* 0x00413DB0 */
void __cdecl CAM_fn_vInitCompleteCineinfo(struct tdstCineinfo_ *hCineinfo)
{
    unsigned long *pulZero = (unsigned long *)&hCineinfo->stLastFailedOrigin;

    pulZero[0]=0;  pulZero[1]=0;  pulZero[2]=0;  pulZero[3]=0;  pulZero[4]=0;
    pulZero[5]=0;  pulZero[6]=0;  pulZero[7]=0;  pulZero[8]=0;  pulZero[9]=0;
    pulZero[10]=0; pulZero[11]=0; pulZero[12]=0; pulZero[13]=0; pulZero[14]=0;
    pulZero[15]=0; pulZero[16]=0; pulZero[17]=0; pulZero[18]=0; pulZero[19]=0;
    pulZero[20]=0; pulZero[21]=0; pulZero[22]=0; pulZero[23]=0; pulZero[24]=0;

    hCineinfo->ucVolIAFlags     = 0;
    hCineinfo->ucPerIAFlags     = 0;
    hCineinfo->cLastCutAngleSens = 0;
    hCineinfo->bCanDoBestPos    = 1;

    CAM_fn_vUpdateTargetPosition(hCineinfo);
    CAM_g_hOldTargetedPerso = NULL;
    CAM_fn_vSetCineinfoCurrentFromInit(hCineinfo);
}

/* 0x00413E30 */
void __cdecl CAM_fn_vUpdateTargetPosition(struct tdstCineinfo_ *hCineinfo)
{
    if (hCineinfo->hWork->hSuperObjectTargeted != NULL) {
        MTH3D_tdstVector stAxis;

        memset(hCineinfo->AveragePosition.a_CheckedSpeed, 0, 24 * 4);
        memset(hCineinfo->AveragePosition.a_CheckedPos, 0, 24 * 4);
        memset(hCineinfo->AveragePosition.a_CheckedTime, 0, 8 * 4);

        stAxis.xX = 0.0f;  stAxis.xY = -1.0f; stAxis.xZ = 0.0f;
        fn_vCineinfoSetSightAxis(hCineinfo, &stAxis);
        stAxis.xX = -1.0f; stAxis.xY = 0.0f;  stAxis.xZ = 0.0f;
        fn_vCineinfoSetFirstComplementaryAxis(hCineinfo, &stAxis);
        stAxis.xX = 0.0f;  stAxis.xY = 0.0f;  stAxis.xZ = -1.0f;
        fn_vCineinfoSetSecondComplementaryAxis(hCineinfo, &stAxis);
    }
}

void CAM_fn_vCineinfoDesinit(struct tdstEngineObject_ *_p_stObject, int _eObjectInit)
{
	if (_p_stObject == NULL)
	{
		ERM_fn_vClearLastError(0);
		ERM_fn_vUpdateLastError(0x30, g_ucGameModuleId, 0, E_uwGameMiniStructObjectNull, -1, 0xff, 0xff, NULL);
	}
	if (_p_stObject->h_Cineinfo == NULL)
	{
		ERM_fn_vClearLastError(0);
		ERM_fn_vUpdateLastError(0x30, g_ucGameModuleId, 0, E_uwGameMiniStructNull, -1, 0xff, 0xff, NULL);
	}
}

/* 0x00414040 */
void __cdecl CAM_fn_vCineinfoCopyClone(struct tdstEngineObject_ *_p_stObjectDest, struct tdstEngineObject_ *_p_stObjectSrc)
{
    memcpy(((struct tdstCineinfo_ *)_p_stObjectDest->h_Cineinfo)->hInit,
           ((struct tdstCineinfo_ *)_p_stObjectSrc->h_Cineinfo)->hInit,
           sizeof(struct tdstInternalStructurCineinfo_));
}

/* 0x00414060 */
struct HIE_tdstSuperObject *__cdecl CAM_fn_hCineInfoGetSuperObjectTargeted(
    struct tdstInternalStructurCineinfo_ *_hIntCineinfo)
{
    return _hIntCineinfo->hSuperObjectTargeted;
}

/* 0x00414070 */
unsigned char __cdecl CAM_fn_cCineInfoGetChannel(MS_tdxHandleToInternalCineinfo _hIntCineinfo)
{
	return (_hIntCineinfo -> cChannel);
}

void fn_vInitAveragePositionDatasIfNecessary(MS_tdxHandleToCineinfo _hCineinfo);

void CAM_fn_vAverageSpeed(MS_tdxHandleToCineinfo _hCineinfo,MTH_tdxReal xTimeWindow,MTH3D_tdstVector *p_result)
{
	MTH_tdxReal				xCurrentTime,tmpTime;
	unsigned long			ulIndex;
	MTH3D_tdstVector		*p_stTmp;

	fn_vInitAveragePositionDatasIfNecessary(_hCineinfo);

	xCurrentTime = 0;
	MTH3D_M_vNullVector(p_result);

	for ( ulIndex = 0; ulIndex < C_ucNbOfCheckedSpeeds; ulIndex ++ )
	{
		tmpTime = _hCineinfo ->AveragePosition.a_CheckedTime[ulIndex];

		p_stTmp = &(_hCineinfo ->AveragePosition.a_CheckedSpeed[ulIndex]);

		if(tmpTime && !MTH3D_M_bIsNullVector(p_stTmp))
		{
			xCurrentTime += tmpTime;
			MTH3D_M_vMulAddVector( p_result, tmpTime , p_stTmp , p_result );

			if(xCurrentTime >= xTimeWindow)
				break;
		}
	}
	if ( xCurrentTime )
		MTH3D_M_vDivScalarVector( p_result , p_result , xCurrentTime);
}

void CAM_fn_vAveragePosition(MS_tdxHandleToCineinfo _hCineinfo,MTH_tdxReal xTimeWindow,MTH3D_tdstVector *p_result)
{
	MTH_tdxReal				xCurrentTime,tmpTime;
	unsigned long			ulIndex;
	MTH3D_tdstVector		*p_stTmp;

	fn_vInitAveragePositionDatasIfNecessary(_hCineinfo);

	xCurrentTime = 0;
    MTH3D_M_vNullVector(p_result);

	for ( ulIndex = 0; ulIndex < C_ucNbOfCheckedSpeeds; ulIndex ++ )
	{
		tmpTime = _hCineinfo ->AveragePosition.a_CheckedTime[ulIndex];
			p_stTmp = &(_hCineinfo ->AveragePosition.a_CheckedPos[ulIndex]);

			if(tmpTime && !MTH3D_M_bIsNullVector(p_stTmp))
			{
				xCurrentTime += tmpTime;

				MTH3D_M_vMulAddVector( p_result, tmpTime , p_stTmp , p_result );

				if(xCurrentTime >= xTimeWindow)
					break;
			}
	}
	if(xCurrentTime)
        MTH3D_M_vDivScalarVector( p_result, p_result, xCurrentTime);
}

#define MTH_C_PiBy16 ((float) 1.9634954084936207E-1)

void CAM_fn_vLoadCameraConstants(void)
{
	CAM_g_stCameraConstants.xRayCameraSphereToGo1 = MTH_M_xConstFloatToReal(0.35f);
	CAM_g_stCameraConstants.xRayCameraSphereToGo2 = MTH_M_xConstFloatToReal(0.35f);
	CAM_g_stCameraConstants.xRayCameraSphereToSee1 = MTH_M_xConstFloatToReal(0.10f);
	CAM_g_stCameraConstants.xRayCameraSphereToSee2 = MTH_M_xConstFloatToReal(0.10f);

	CAM_g_stCameraConstants.xBaseAngleComputePosNormal = MTH_C_PiBy4;
	CAM_g_stCameraConstants.xBaseAngleComputePosFailure = MTH_C_PiBy4;
	CAM_g_stCameraConstants.xBaseCutAngle = MTH_C_PiBy8;

	CAM_g_stCameraConstants.xAverageForComputePos = MTH_M_xConstFloatToReal(0.40f);
	CAM_g_stCameraConstants.xAverageForComputeLinearSpeed = MTH_M_xConstFloatToReal(0.40f);

	CAM_g_stCameraConstants.xMinLinearSpeedCamera = MTH_M_xConstFloatToReal(0.005f);
	CAM_g_stCameraConstants.xMinLinearSpeedTgtPerso = MTH_M_xConstFloatToReal(0.01f);
	CAM_g_stCameraConstants.xMinAngularSpeedTgtPerso = MTH_M_xConstFloatToReal(0.01f);

	CAM_g_stCameraConstants.xEpsilonDynTgtAngle = MTH_C_PiBy4;
	CAM_g_stCameraConstants.xEpsilonDynTgtFocalAngle = MTH_C_PiBy16;
	CAM_g_stCameraConstants.xEpsilonForEqualVectors = MTH_M_xConstFloatToReal(0.5f);

	CAM_g_stCameraConstants.xOffsetMinDistPerso = MTH_M_xConstFloatToReal(0.05f);
	CAM_g_stCameraConstants.xDistanceForNoCut = MTH_C_5;

	CAM_g_stCameraConstants.xNumRayCameraCanGo = 3;
	CAM_g_stCameraConstants.xNumRayCameraCanSee = 1;

	CAM_g_stCameraConstants.xTickTestVisibility = 4;
	CAM_g_stCameraConstants.xTickFindBetterPos = 20;
	CAM_g_stCameraConstants.xTickFindBetterPosIfNoMove = 60;
	CAM_g_stCameraConstants.xTickFailureCantSee = 60;

	CAM_g_stCameraConstants.xDynSpeed_LinearFactNorm = MTH_C_3;
	CAM_g_stCameraConstants.xDynSpeed_LinearMax = MTH_M_xLongToReal(10);
	CAM_g_stCameraConstants.xDynSpeed_LinearMulPerso = MTH_M_xConstFloatToReal(1.5f);
	CAM_g_stCameraConstants.xDynSpeed_AngularFactNorm = MTH_C_3;
	CAM_g_stCameraConstants.xDynSpeed_AngularMax = MTH_C_3;
	CAM_g_stCameraConstants.xDynSpeed_AngularMulPerso = MTH_C_ZERO;
	CAM_g_stCameraConstants.xDynSpeed_TargetFactNorm = MTH_C_4;
	CAM_g_stCameraConstants.xDynSpeed_TargetMax = MTH_M_xLongToReal(10);
	CAM_g_stCameraConstants.xDynSpeed_TargetMulPerso = MTH_C_ONE;

	CAM_g_stCameraConstants.xCutAngleFactorPos = MTH_C_ONE;
	CAM_g_stCameraConstants.xCutAngleFactorTgt = MTH_C_ONE;

	CAM_g_stCameraConstants.wFlags = 0;
}

MTH3D_tdstVector *fn_p_CineinfoGetSightAxis(MS_tdxHandleToCineinfo _hCineinfo)
{
	return (&(_hCineinfo->stSightAxisSystem.SightAxis));
}

/* 0x004141B0 */
void __cdecl fn_vCineinfoSetSightAxis(struct tdstCineinfo_ *hCineinfo, MTH3D_tdstVector *p_stVector)
{
    hCineinfo->stSightAxisSystem.SightAxis.xX = p_stVector->xX;
    hCineinfo->stSightAxisSystem.SightAxis.xY = p_stVector->xY;
    hCineinfo->stSightAxisSystem.SightAxis.xZ = p_stVector->xZ;
}

MTH3D_tdstVector *fn_p_CineinfoGetFirstComplementaryAxis(MS_tdxHandleToCineinfo _hCineinfo)
{
	return (&(_hCineinfo->stSightAxisSystem.FirstComplementaryAxis));
}

/* 0x004141F0 */
void __cdecl fn_vCineinfoSetFirstComplementaryAxis(struct tdstCineinfo_ *hCineinfo, MTH3D_tdstVector *p_stVector)
{
    hCineinfo->stSightAxisSystem.FirstComplementaryAxis.xX = p_stVector->xX;
    hCineinfo->stSightAxisSystem.FirstComplementaryAxis.xY = p_stVector->xY;
    hCineinfo->stSightAxisSystem.FirstComplementaryAxis.xZ = p_stVector->xZ;
}

MTH3D_tdstVector *fn_p_CineinfoGetSecondComplementaryAxis(MS_tdxHandleToCineinfo _hCineinfo)
{
	return (&(_hCineinfo->stSightAxisSystem.SecondComplementaryAxis));
}

/* 0x00414230 */
void __cdecl fn_vCineinfoSetSecondComplementaryAxis(struct tdstCineinfo_ *hCineinfo, MTH3D_tdstVector *p_stVector)
{
    hCineinfo->stSightAxisSystem.SecondComplementaryAxis.xX = p_stVector->xX;
    hCineinfo->stSightAxisSystem.SecondComplementaryAxis.xY = p_stVector->xY;
    hCineinfo->stSightAxisSystem.SecondComplementaryAxis.xZ = p_stVector->xZ;
}

void fn_vGetPersoSightAxisSystem
(
	MS_tdxHandleToCineinfo		_hCineinfo,
	MTH3D_tdstVector			*_p_stSightAxis,
	MTH3D_tdstVector			*_p_stFirstComplementaryAxis,
	MTH3D_tdstVector			*_p_stSecondComplementaryAxis
)
{
	MTH3D_M_vCopyVector(_p_stSightAxis , fn_p_CineinfoGetSightAxis(_hCineinfo));
	MTH3D_M_vCopyVector(_p_stFirstComplementaryAxis , fn_p_CineinfoGetFirstComplementaryAxis(_hCineinfo));
	MTH3D_M_vCopyVector(_p_stSecondComplementaryAxis , fn_p_CineinfoGetSecondComplementaryAxis(_hCineinfo));
}

void fn_vInitAveragePositionDatasIfNecessary(MS_tdxHandleToCineinfo	_hCineinfo)
{
	unsigned long			ulIndex;
    MTH_tdxReal             xBaseTime;
    MTH3D_tdstVector        *p_stVectorPos;

	for ( ulIndex = 0; ulIndex < C_ucNbOfCheckedSpeeds; ulIndex ++ )
        if(_hCineinfo ->AveragePosition.a_CheckedTime[ulIndex])
            break;

    if(ulIndex < C_ucNbOfCheckedSpeeds)
		return;

    p_stVectorPos = POS_fn_p_stGetTranslationVector(HIE_fn_hGetSuperObjectGlobalMatrix(_hCineinfo->hWork->hSuperObjectTargeted));
    xBaseTime     = (MTH_tdxReal)0.03f;

	for ( ulIndex = 0; ulIndex < C_ucNbOfCheckedSpeeds; ulIndex ++ )
    {
		_hCineinfo->AveragePosition.a_CheckedTime[ulIndex] = xBaseTime;
        MTH3D_M_vNullVector(&(_hCineinfo ->AveragePosition.a_CheckedSpeed[ulIndex]));
        MTH3D_M_vCopyVector(&(_hCineinfo ->AveragePosition.a_CheckedPos[ulIndex]),p_stVectorPos);
    }
}

/* 0x00414370 */
void __cdecl CAM_fn_vRemindSpeed(struct tdstCineinfo_ *hCineinfo, MTH_tdstMove *p_Speed)
{
    unsigned long *pulDst = (unsigned long *)hCineinfo->AveragePosition.a_CheckedSpeed;
    unsigned long *pulSrc = (unsigned long *)&hCineinfo->AveragePosition.a_CheckedSpeed[1];
#ifdef RAYMAN2_FIX_BUGS
    /* FIX(BUGS.md#9) */
    /* shift down by one element: source and destination overlap */
    memmove(pulDst, pulSrc, 0x15 * 4);
#else
    memcpy(pulDst, pulSrc, 0x15 * 4);
#endif /* RAYMAN2_FIX_BUGS */

    *(unsigned long *)&hCineinfo->AveragePosition.a_CheckedSpeed[7].xX = *(unsigned long *)&p_Speed->m_stLinear.xX;
    *(unsigned long *)&hCineinfo->AveragePosition.a_CheckedSpeed[7].xY = *(unsigned long *)&p_Speed->m_stLinear.xY;
    *(unsigned long *)&hCineinfo->AveragePosition.a_CheckedSpeed[7].xZ = *(unsigned long *)&p_Speed->m_stLinear.xZ;
}

/* 0x004143B0 */
void __cdecl CAM_fn_vRemindPos(struct tdstCineinfo_ *hCineinfo, MTH_tdstMove *p_Pos)
{
    unsigned long *pulDst = (unsigned long *)hCineinfo->AveragePosition.a_CheckedPos;
    unsigned long *pulSrc = (unsigned long *)&hCineinfo->AveragePosition.a_CheckedPos[1];
#ifdef RAYMAN2_FIX_BUGS
    /* FIX(BUGS.md#9) */
    /* shift down by one element: source and destination overlap */
    memmove(pulDst, pulSrc, 0x15 * 4);
#else
    memcpy(pulDst, pulSrc, 0x15 * 4);
#endif /* RAYMAN2_FIX_BUGS */

    *(unsigned long *)&hCineinfo->AveragePosition.a_CheckedPos[7].xX = *(unsigned long *)&p_Pos->m_stLinear.xX;
    *(unsigned long *)&hCineinfo->AveragePosition.a_CheckedPos[7].xY = *(unsigned long *)&p_Pos->m_stLinear.xY;
    *(unsigned long *)&hCineinfo->AveragePosition.a_CheckedPos[7].xZ = *(unsigned long *)&p_Pos->m_stLinear.xZ;
}

/* 0x004143F0 */
void __cdecl CAM_fn_vRemindTime(struct tdstCineinfo_ *hCineinfo, MTH_tdxReal _dt)
{
    unsigned long *pulDst = (unsigned long *)hCineinfo->AveragePosition.a_CheckedTime;
    unsigned long *pulSrc = (unsigned long *)&hCineinfo->AveragePosition.a_CheckedTime[1];
#ifdef RAYMAN2_FIX_BUGS
    /* FIX(BUGS.md#9) */
    /* shift down by one element: source and destination overlap */
    memmove(pulDst, pulSrc, 7 * 4);
#else
    memcpy(pulDst, pulSrc, 7 * 4);
#endif /* RAYMAN2_FIX_BUGS */
    hCineinfo->AveragePosition.a_CheckedTime[7] = _dt;
}

void CAM_fn_vSetCineinfoVisibilityFromCurrent(MS_tdxHandleToCineinfo _hCineinfo)
{
	if(!(_hCineinfo->ucPerIAFlags & C_PerIAFlags_VisibilityCopied))
	{
		*_hCineinfo->hVisibility = *_hCineinfo->hCurrent;
		_hCineinfo->ucPerIAFlags |= C_PerIAFlags_VisibilityCopied;
	}
}

/* 0x00414100 */
void __cdecl CAM_fn_vSetCineinfoCurrentFromVisibility(struct tdstCineinfo_ *hCineinfo)
{
    unsigned long *pulSrc = (unsigned long *)hCineinfo->hVisibility;
    unsigned long *pulDst = (unsigned long *)hCineinfo->hCurrent;
    memcpy(pulDst, pulSrc, 0x1f * 4);
    hCineinfo->ucPerIAFlags &= ~C_PerIAFlags_VisibilityCopied;
}

/* 0x00414120 */
void __cdecl CAM_fn_vSetCineinfoWorkFromVisibility(struct tdstCineinfo_ *hCineinfo)
{
    unsigned long *pulSrc = (unsigned long *)hCineinfo->hVisibility;
    unsigned long *pulDst = (unsigned long *)hCineinfo->hWork;
    memcpy(pulDst, pulSrc, 0x1f * 4);
}

void CAM_fn_vSetCameraInList(struct HIE_tdstSuperObject *_hSupObjectCamera)
{
	tdstNodeCamera *p_stTempNodeCamera;

	p_stTempNodeCamera = (tdstNodeCamera *)fnp_vGameMallocInHLM(sizeof(tdstNodeCamera));
	p_stTempNodeCamera->p_stSuperObjectCamera = _hSupObjectCamera;

	p_stTempNodeCamera->hPrev = NULL;
	p_stTempNodeCamera->hFather = NULL;

	p_stTempNodeCamera->hNext = NULL;
	p_stTempNodeCamera->hPrev = GAM_g_stEngineStructure.hCameraList_hLastElementDyn;
	p_stTempNodeCamera->hFather = &GAM_g_stEngineStructure.hCameraList_hFirstElementDyn;

	if (GAM_g_stEngineStructure.hCameraList_hFirstElementDyn == NULL)
	{
		GAM_g_stEngineStructure.hCameraList_hFirstElementDyn = p_stTempNodeCamera;
		GAM_g_stEngineStructure.hCameraList_hLastElementDyn = p_stTempNodeCamera;
		GAM_g_stEngineStructure.hCameraList_lNumberOfElementsDyn++;
		return;
	}
	((tdstNodeCamera *)GAM_g_stEngineStructure.hCameraList_hLastElementDyn)->hNext = p_stTempNodeCamera;
	GAM_g_stEngineStructure.hCameraList_hLastElementDyn = p_stTempNodeCamera;
	GAM_g_stEngineStructure.hCameraList_lNumberOfElementsDyn++;
}

#define E_uwGameCineInfoSuperObjectNull ((unsigned short)0x10)

void CAM_fn_vCineinfoInit(struct tdstEngineObject_ *_p_stObject, int _eObjectInit)
{
	MS_tdxHandleToCineinfo	hCineinfo;
	struct HIE_tdstSuperObject *hSupObjectCamera;

	if (_p_stObject == NULL)
	{
		ERM_fn_vClearLastError(0);
		ERM_fn_vUpdateLastError(0x30, g_ucGameModuleId, 0, E_uwGameMiniStructObjectNull, -1, 0xff, 0xff, NULL);
	}

	hCineinfo = (MS_tdxHandleToCineinfo)_p_stObject->h_Cineinfo;
	if (_p_stObject->h_Cineinfo == NULL)
	{
		ERM_fn_vClearLastError(0);
		ERM_fn_vUpdateLastError(0x30, g_ucGameModuleId, 0, E_uwGameMiniStructNull, -1, 0xff, 0xff, NULL);
	}

	switch (_eObjectInit)
	{
	case 0:
		break;
	case 1:
		hSupObjectCamera = M_GetSuperObject(_p_stObject);
		if (hSupObjectCamera == NULL)
		{
			ERM_fn_vClearLastError(0);
			ERM_fn_vUpdateLastError(0x30, g_ucGameModuleId, 0, E_uwGameCineInfoSuperObjectNull, -1, 0xff, 0xff, NULL);
		}
		CAM_fn_vSetCameraInList(hSupObjectCamera);

	case 2:
	case 3:
	case 4:
	case 5:
		hCineinfo->hInit->hSuperObjectTargeted = GAM_pc_g_hMainCharacterSO;
		CAM_fn_vInitCompleteCineinfo(hCineinfo);
		break;
	default:
		break;
	}
}
