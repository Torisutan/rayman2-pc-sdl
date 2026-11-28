#include "AI/CAM_Tool.h"
#include "GAM/cineinfo.h"
#include "GAM/ChanList.h"
#include "GAM/GAM_types.h"
#include "HIE/HIE_types.h"
#include "POS/POS_types.h"
#include "DNM/DNM_types.h"
#include "MTH/MTH3D.h"
#include "MTH/MTH_flt.h"
#include "MTH/MTH_xNeg.h"
#include "AI/CAM_Base.h"
#include <string.h>
#include "AI/AI_globals.h"

MTH_tdxReal CAM_fn_xSetAngleInInterval02Pi(MTH_tdxReal _xAngle);                /* 0x00473B70 */
MTH_tdxReal CAM_fn_xComputeHorizontalAngle(MTH3D_tdstVector *_p_stOrgVector, MTH3D_tdstVector *_p_stDestVector, MTH3D_tdstVector *_p_stAxisZ); /* 0x00433390 */
MTH_tdxReal CAM_fn_xComputeVerticalAngle(MTH3D_tdstVector *_p_stOrgVector, MTH3D_tdstVector *_p_stDestVector, MTH3D_tdstVector *_p_stAxisZ);   /* 0x004335C0 */
DNM_tdstReport *fn_pstGetDNMReport(HIE_tdstSuperObject *_p_SuperObjPerso);
void *fn_p_stDynamGetParsingDatas(void *_h_Dynam);
void *fn_p_stDynamGetDynamics(void *_h_Dynam);
void CAM_fn_vComputeCameraSystemAxis(DNM_tdstDynamics *_p_stDynamics, HIE_tdstSuperObject *_h_SupObj,
	MTH3D_tdstVector *_p_stNewPosition, MTH3D_tdstVector *_p_stNewTarget,
	MTH3D_tdstVector *_p_stAxisX, MTH3D_tdstVector *_p_stAxisY, MTH3D_tdstVector *_p_stAxisZ);
void fn_vGetPersoSightAxisSystem(MS_tdxHandleToCineinfo _hCineinfo, MTH3D_tdstVector *_p_stSightAxis,
	MTH3D_tdstVector *_p_stFirstComplementaryAxis, MTH3D_tdstVector *_p_stSecondComplementaryAxis);

#include "MEC/DNMRepor.h"

void __cdecl fn_vSectInfoInitForSingleCharacter(HIE_tdstSuperObject *hCharacter, MTH3D_tdstVector *hPosition);

#define DNM_M_p_stCPDGetTarget(_p_stParsingDatas)			(&(_p_stParsingDatas)->stVector)

typedef int CAM_tdxCtxKnob0, CAM_tdxCtxKnob1, CAM_tdxCtxKnob2;

void __cdecl fn_vSectInfoSetCurrentSector(void *h_SectInfo, void *hCurrentSector);
void *__cdecl fn_h_SectInfoGetCurrentSector(void *h_SectInfo);

typedef void *MS_tdxHandleToSectInfo;
#define M_GetCineinfo(_p_stSuperObject_, _Name_)\
(\
	(MS_tdxHandleToInternalCineinfo)\
		(\
			(\
				M_GetMSHandle(_p_stSuperObject_,Cineinfo)\
			)\
			->h##_Name_\
		)\
)

#define CAM_C_xAverageForComputeLinearSpeed	(CAM_g_stCameraConstants.xAverageForComputeLinearSpeed)
#define CAM_C_xMinLinearSpeedTgtPerso		(CAM_g_stCameraConstants.xMinLinearSpeedTgtPerso)
#define CAM_C_xCutAngleFactorPos			(CAM_g_stCameraConstants.xCutAngleFactorPos)
#define CAM_C_xCutAngleFactorTgt			(CAM_g_stCameraConstants.xCutAngleFactorTgt)
#define CAM_C_xMinAngularSpeedTgtPerso		(CAM_g_stCameraConstants.xMinAngularSpeedTgtPerso)

#ifndef C_IAFlags_NoDynChangeTheta
#define C_IAFlags_NoDynChangeTheta				0x0020
#endif
#ifndef C_IAFlags_PositionIsAlreadyComputed
#define C_IAFlags_PositionIsAlreadyComputed		0x2000
#endif
#ifndef C_IAFlags_RefAxisIsAlreadyComputed
#define C_IAFlags_RefAxisIsAlreadyComputed		0x4000
#endif
#ifndef C_VolIAFlags_InitJustCopiedInCurrent
#define C_VolIAFlags_InitJustCopiedInCurrent	0x02
#endif

#include "MEC/macdpid.h"

#include "MEC/dnmdynam.h"

/* 0x00414420 */
void CAM_fn_vAverageSpeed(MS_tdxHandleToCineinfo _hCineinfo, MTH_tdxReal xTimeWindow, MTH3D_tdstVector *p_result);

#ifndef C_WOT_ulGround
#define C_WOT_ulGround			0x00000001
#endif

#include "GAM/PlayAnim.h"

#ifndef C_IAFlags_RefAxisIsAlreadyComputed
#define C_IAFlags_RefAxisIsAlreadyComputed      0x4000
#endif

extern char cRefAxisIsAlreadyComputed;
extern char cNoDynChangeTheta;
void CAM_fn_vComputeRealWantedPos(CAM_tdstUpdateCamera *_p_stStruct);
void CAM_fn_vComputeMovePosWithDynTheta(CAM_tdstUpdateCamera *_p_stStruct);
void CAM_fn_vForceFailureVisibility(CAM_tdstUpdateCamera *_p_stStruct);
void CAM_fn_vComputeFailureVisibility(CAM_tdstUpdateCamera *_p_stStruct);

void CAM_memcpy(void *dest, void *src, long size)
{
	register long *psrc = (long*)src;
	register long *pdest = (long*)dest;
	register long lsize = size;
	while(lsize >= 4)
	{
		*pdest++ = *psrc++;
		lsize -= 4;
	}
	while(lsize)
	{
		*((char *)pdest++) = *((char *)psrc++);
		lsize--;
	}
}

void CAM_fn_vSetResetInternalFlag(
	MS_tdxHandleToCineinfo	_hCineInfo,
	unsigned short			*_p_uwCurrentValue,
	unsigned short			*_p_uwVisibilityValue,
	unsigned short			*_p_uwWorkValue,
	long					_lTypeOfChange,
	long					_lChangingMode,
	unsigned short			_uwFlag)
{
	if (_lTypeOfChange == 1)
	{
		CAM_fn_vSetCineinfoWorkFromCurrent(_hCineInfo);
		if (_lChangingMode)
			*_p_uwWorkValue |= _uwFlag;
		else
			*_p_uwWorkValue &= ~_uwFlag;
	}
	else
	{
		if (_lChangingMode)
		{
			*_p_uwCurrentValue |= _uwFlag;
			*_p_uwVisibilityValue |= _uwFlag;
			*_p_uwWorkValue |= _uwFlag;
		}
		else
		{
			*_p_uwCurrentValue &= ~_uwFlag;
			*_p_uwVisibilityValue &= ~_uwFlag;
			*_p_uwWorkValue &= ~_uwFlag;
		}
	}
}

void CAM_fn_vForceCameraPos(
	HIE_tdstSuperObject			*_hSuperObjCamera,
	MTH3D_tdstVector			*_p_stPosition)
{
	DNM_tdstReport			*p_stReport;

	p_stReport = fn_pstGetDNMReport(_hSuperObjCamera);

	POS_fn_vSetTranslationVector(HIE_fn_hGetSuperObjectGlobalMatrix(_hSuperObjCamera), _p_stPosition);
	POS_fn_vSetTranslationVector(HIE_fn_hGetSuperObjectMatrix(_hSuperObjCamera), _p_stPosition);

	MTH3D_M_vNullVector(DNM_M_p_stReportGetLastMove(p_stReport));

	fn_vSectInfoInitForSingleCharacter(_hSuperObjCamera, _p_stPosition);
}

void CAM_fn_vForceCameraTgt(
	HIE_tdstSuperObject			*_hSuperObjCamera,
	MTH3D_tdstVector			*_p_stPosition,
	MTH3D_tdstVector			*_p_stRefAxisZ)
{

	MS_tdxHandleToDynam		h_Dynam;
    MS_tdxHandleToCineinfo	hCineInfo;
	DNM_tdstParsingDatas	*p_stParsingDatas;
	DNM_tdstReport			*p_stReport;
	MTH3D_tdstVector		stX, stY, stZ;
	MTH3D_tdstVector		stCameraPosition;

	hCineInfo = M_GetMSHandle(_hSuperObjCamera, Cineinfo);
	h_Dynam = M_GetMSHandle(_hSuperObjCamera, Dynam);
	p_stReport = fn_pstGetDNMReport(_hSuperObjCamera);
	p_stParsingDatas = fn_p_stDynamGetParsingDatas(h_Dynam);

	POS_fn_vGetTranslationVector(HIE_fn_hGetSuperObjectGlobalMatrix(_hSuperObjCamera), &stCameraPosition);

	MTH3D_M_vCopyVector(&stZ, _p_stRefAxisZ);
	CAM_fn_vComputeCameraSystemAxis(
		fn_p_stDynamGetDynamics(h_Dynam),
		_hSuperObjCamera,
		&stCameraPosition,
		_p_stPosition,
		&stX,
		&stY,
		&stZ);

	POS_fn_vSetRotationMatrix(HIE_fn_hGetSuperObjectMatrix(_hSuperObjCamera), &stX, &stY, &stZ);
	POS_fn_vSetRotationMatrix(HIE_fn_hGetSuperObjectGlobalMatrix(_hSuperObjCamera), &stX, &stY, &stZ);

	MTH3D_M_vCopyVector(DNM_M_p_stCPDGetTarget(p_stParsingDatas), _p_stPosition);

	MTH3D_M_vNullVector(DNM_M_p_stReportGetLastTgtMove(p_stReport));

}

ACP_tdxBool CAM_fn_bPersoIsMoving(CAM_tdstUpdateCamera *_p_stStruct, MTH3D_tdstVector *_p_stMove)
{

	MTH3D_tdstVector	stMove;

	CAM_fn_vAverageSpeed(_p_stStruct->hCineinfo, CAM_C_xAverageForComputeLinearSpeed, &stMove);
	if(MTH_M_bGreater(MTH3D_M_xNormVector(&stMove), CAM_C_xMinLinearSpeedTgtPerso))
	{
		if(_p_stMove)
		{
			MTH3D_M_vCopyVector(_p_stMove, &stMove);
		}
		return TRUE;
	}

	if(_p_stMove)
		MTH3D_M_vNullVector(_p_stMove);

	return FALSE;
}

ACP_tdxBool CAM_fn_bPersoIsTurning(HIE_tdstSuperObject *_h_SuperObjPerso, MTH_tdxReal *_p_xAngularSpeed)
{

	MS_tdxHandleToDynam		h_Dynam;
	DNM_tdstDynamics		*p_stDynamics;
	DNM_tdstReport			*p_stReport;

	MTH_tdxReal				xAngle;

	if(_p_xAngularSpeed)
		*_p_xAngularSpeed = MTH_C_ZERO;

    h_Dynam = M_GetMSHandle(_h_SuperObjPerso, Dynam);
	if(h_Dynam == NULL)
		return FALSE;
	p_stDynamics = fn_p_stDynamGetDynamics(h_Dynam);
    if(p_stDynamics == NULL)
		return FALSE;
	p_stReport = DNM_M_p_stDynamicsGetReport(p_stDynamics);
	if(p_stReport == NULL)
		return FALSE;

	xAngle = MTH_M_xRotationGetAngle(MTH_M_p_stMoveGetAngular(DNM_M_p_stReportGetAbsoluteCurrSpeed(p_stReport)));

	if(MTH_M_bGreater(xAngle, CAM_C_xMinAngularSpeedTgtPerso))
	{
		if(_p_xAngularSpeed)
			*_p_xAngularSpeed = xAngle;
		return TRUE;
	}

	return FALSE;
}

ACP_tdxBool CAM_fn_bPersoOnFace(HIE_tdstSuperObject *_h_SuperObjPerso, MTH3D_tdstVector *_p_stNormal)
{

	MS_tdxHandleToDynam		h_Dynam;
	DNM_tdstDynamics		*p_stDynamics;
	DNM_tdstReport			*p_stReport;

    h_Dynam = M_GetMSHandle(_h_SuperObjPerso, Dynam);
	if(h_Dynam == NULL)
		return FALSE;
	p_stDynamics = fn_p_stDynamGetDynamics(h_Dynam);
    if(p_stDynamics == NULL)
		return FALSE;
	p_stReport = DNM_M_p_stDynamicsGetReport(p_stDynamics);
	if(p_stReport == NULL)
		return FALSE;

	if(!(p_stReport->m_ulCurrSurfaceState & C_WOT_ulGround))
		return FALSE;

	MTH3D_M_vCopyVector(_p_stNormal, &p_stReport->m_stGround.m_stNorm);
	return TRUE;
}

HIE_tdstSuperObject *CAM_fn_hGetFather(HIE_tdstSuperObject *_h_SuperObjPerso)
{
	MS_tdxHandleToDynam			h_Dynam;
	DNM_tdstDynamics			*p_stDynamics;

	if(_h_SuperObjPerso == NULL)
		return NULL;
	h_Dynam = M_GetMSHandle(_h_SuperObjPerso, Dynam);
	if(h_Dynam == NULL)
		return NULL;
	p_stDynamics = fn_p_stDynamGetDynamics(h_Dynam);
	if(p_stDynamics == NULL)
		return NULL;

	if (!DNM_M_bDynamicsIsComplexSize(p_stDynamics))
		return NULL;

	return DNM_M_hDynamicsGetPlatformSO(p_stDynamics);
}
void CAM_fn_vComputeProjection(
	MTH3D_tdstVector	*_p_sDestVector,
	MTH3D_tdstVector	*_p_sSrcVector,
	MTH3D_tdstVector	*_p_sNormVector)
{

	MTH_tdxReal			xScalar;
	MTH3D_tdstVector	stNormPart;

	xScalar = MTH3D_M_xDotProductVector(_p_sSrcVector, _p_sNormVector);

	if (!MTH_M_bEqualZero(xScalar))
	{
		MTH3D_M_vMulScalarVector(&stNormPart, xScalar, _p_sNormVector);

		MTH3D_M_vSubVector(_p_sDestVector, _p_sSrcVector, &stNormPart);
	}
	else
	{
		MTH3D_M_vCopyVector(_p_sDestVector, _p_sSrcVector);
	}
}

MTH_tdxReal CAM_fn_xComputeRotation(
	MTH3D_tdstVector	*_p_stDestVector,
	MTH3D_tdstVector	*_p_stSrcVector,
	MTH3D_tdstVector	*_p_stAxisVector)
{
#if defined(USE_20_BIT_FRACTIONS)

    MTH_tdxReal			xCos;

	if( MTH3D_M_bIsNullVector(_p_stSrcVector) || MTH3D_M_bIsNullVector(_p_stDestVector) )
		return MTH_C_ZERO;

	if(_p_stAxisVector)
	{
		MTH3D_M_vCrossProductVector(_p_stAxisVector, _p_stSrcVector, _p_stDestVector);
		if (!MTH3D_M_bIsNullVector(_p_stAxisVector))
		{
			MTH3D_M_vNormalizeVector(_p_stAxisVector, _p_stAxisVector);
		}
	}

	xCos = (_p_stSrcVector->xX * _p_stDestVector->xX + _p_stSrcVector->xY * _p_stDestVector->xY +
			_p_stSrcVector->xZ * _p_stDestVector->xZ + 8) >> 4;
	xCos = (MTH_M_xACos20(xCos) + 128) >> 8;

	return xCos;
#else

    MTH_tdxReal			xCos;
	MTH_tdxReal			xDenom, xNumer;

	if( MTH3D_M_bIsNullVector(_p_stSrcVector) || MTH3D_M_bIsNullVector(_p_stDestVector) )
		return MTH_C_ZERO;

	xDenom = MTH_M_xMul( MTH3D_M_xSqrVector( _p_stSrcVector ), MTH3D_M_xSqrVector( _p_stDestVector ) );
	xNumer = MTH3D_M_xDotProductVector( _p_stSrcVector, _p_stDestVector );
	xCos   = MTH_M_xDiv( xNumer, MTH_M_xSqrt( xDenom ) );

	if(_p_stAxisVector)
	{
		MTH3D_M_vCrossProductVector(_p_stAxisVector, _p_stSrcVector, _p_stDestVector);
		if(!MTH3D_M_bIsNullVector(_p_stAxisVector))
		{
			MTH3D_M_vNormalizeVector(_p_stAxisVector, _p_stAxisVector);
		}
	}

	if (MTH_M_bLess(xCos,MTH_C_MinusONE))
        xCos = MTH_C_MinusONE;
    else if (MTH_M_bGreater(xCos,MTH_C_ONE))
        xCos =MTH_C_ONE;

	return (MTH_M_xACos(xCos));
#endif
}

/* 0x004739D0 */
ACP_tdxBool __cdecl CAM_fn_bEqualVectorWithEpsilon(MTH3D_tdstVector *_p_stVector1,
                                                   MTH3D_tdstVector *_p_stVector2,
                                                   MTH_tdxReal _xEpsilon)
{
    MTH3D_tdstVector stDiffVector;
    MTH_tdxReal      xDist;

    MTH3D_M_vSubVector(&stDiffVector, _p_stVector1, _p_stVector2);
    xDist = MTH3D_M_xSqrVector(&stDiffVector);

    if (MTH_M_bGreater(xDist, MTH_M_xSqr(_xEpsilon)))
        return 0;
    return 1;
}

void CAM_fn_vCheckValidityOfComputedAngles(
	MS_tdxHandleToInternalCineinfo	_hIntCineInfo,
	MTH_tdxReal						*_p_xHorizontalAngle,
	MTH_tdxReal						*_p_xVerticalAngle)
{

	MTH_tdxReal		xAbsHorizontalAngle, xAbsVerticalAngle;

	xAbsHorizontalAngle =  MTH_M_xAbs(*_p_xHorizontalAngle);

	xAbsVerticalAngle = MTH_M_xAbs(*_p_xVerticalAngle);

	if(MTH_M_bGreater(xAbsVerticalAngle, MTH_C_PiBy2))
	{
		if (MTH_M_bGreater(*_p_xVerticalAngle, MTH_C_ZERO))
			*_p_xVerticalAngle = MTH_M_xSub(MTH_C_Pi, *_p_xVerticalAngle);
		else
			*_p_xVerticalAngle = MTH_M_xSub(MTH_M_xNeg(MTH_C_Pi), *_p_xVerticalAngle);
	}

	*_p_xHorizontalAngle = CAM_fn_xSetAngleInInterval02Pi(*_p_xHorizontalAngle);
	*_p_xVerticalAngle = CAM_fn_xSetAngleInInterval02Pi(*_p_xVerticalAngle);
}

void CAM_fn_vComputeAnglesBetweenTwoVectors(
	MS_tdxHandleToInternalCineinfo	_hIntCineInfo,
	MTH3D_tdstVector				*_p_stSourceVector,
	MTH3D_tdstVector				*_p_stTargetVector,
	MTH_tdxReal						*_p_xHorizontalAngle,
	MTH_tdxReal						*_p_xVerticalAngle,
	MTH3D_tdstVector				*_p_stRefAxisZ)
{
	*_p_xHorizontalAngle = CAM_fn_xComputeHorizontalAngle(_p_stSourceVector, _p_stTargetVector, _p_stRefAxisZ);
	*_p_xVerticalAngle = CAM_fn_xComputeVerticalAngle(_p_stSourceVector, _p_stTargetVector, _p_stRefAxisZ);

	CAM_fn_vCheckValidityOfComputedAngles(_hIntCineInfo, _p_xHorizontalAngle, _p_xVerticalAngle);
}

MTH_tdxReal CAM_fn_xSetAngleInIntervalPiPi(MTH_tdxReal _xAngle)
{
	if (MTH_M_bGreater(_xAngle, MTH_C_Pi))
		return (MTH_M_xSub(_xAngle, MTH_C_2Pi));
	else if(MTH_M_bLessEqual(_xAngle, MTH_M_xNeg(MTH_C_Pi)))
		return (MTH_M_xAdd(_xAngle, MTH_C_2Pi));
	else
		return (_xAngle);
}

MTH_tdxReal CAM_fn_xSetAngleInInterval02Pi(MTH_tdxReal _xAngle)
{
	if (MTH_M_bLess(_xAngle, MTH_C_ZERO))
		return(MTH_M_xAdd(_xAngle, MTH_C_2Pi));
	else if (MTH_M_bGreaterEqual(_xAngle, MTH_C_2Pi))
		return(MTH_M_xSub(_xAngle, MTH_C_2Pi));
	else
		return (_xAngle);
}
void CAM_fn_vGetPersoSightAxis(CAM_tdstUpdateCamera *_p_stStruct, MTH3D_tdstVector *_p_stAxis,  MTH3D_tdstVector *_p_stAxisZ)
{
	MTH3D_tdstVector			stFirstComplementaryAxis;
	MTH3D_tdstVector			stSecondComplementaryAxis;
	POS_tdstCompletePosition	*p_stPersoGlobalMatrix;
	MTH3D_tdstVector			stTmpVector1, stTmpVector2;
	unsigned char						ucChannel;
	HIE_tdstSuperObject			*hPerso;
	MS_tdxHandleTo3dData		h3dData;
	CHN_tdxHandleToChannelArray	hChannelList;

	hPerso = _p_stStruct->hCineinfoWork->hSuperObjectTargeted;

	fn_vGetPersoSightAxisSystem
	(
		_p_stStruct->hCineinfo,
		_p_stAxis,
		&stFirstComplementaryAxis,
		&stSecondComplementaryAxis
	);

	if ((ucChannel = _p_stStruct->hCineinfoWork->cChannel) != C_ucUnknownChannel)
	{

		h3dData = M_GetMSHandle(hPerso, 3dData);
		hChannelList = h3dData->hArrayOfChannels;
		hPerso = hChannelList ? hChannelList[(unsigned int)(short)ucChannel].hSupObject : NULL;
	}
	p_stPersoGlobalMatrix = HIE_fn_hGetSuperObjectGlobalMatrix(hPerso);

	POS_fn_vGetRotationMatrix(p_stPersoGlobalMatrix, &stTmpVector1, _p_stAxis, &stTmpVector2);
	if (_p_stAxisZ)
		MTH3D_M_vCopyVector(_p_stAxisZ, &stTmpVector2);
	MTH3D_M_vNegVector(_p_stAxis, _p_stAxis);
}

/* 0x00473C60 */
MTH_tdxReal __cdecl Cam_fn_xComputeProjectionRail(MTH3D_tdstVector *_p_stTargetedPersoPos,
                                                  MTH3D_tdstVector *_p_stWpPosition,
                                                  MTH3D_tdstVector *_p_stDestWpPosition)
{
    MTH3D_tdstVector stAxe;
    MTH3D_tdstVector stTargetWp;
    MTH3D_tdstVector stProjectedVector;
    MTH3D_tdstVector stProjectedPoint;
    MTH_tdxReal      xFactor;
    MTH_tdxReal      xDotProduct;
    MTH_tdxReal      xSqrNorm;
    MTH_tdxReal      xDist1, xDist2;

    MTH3D_M_vSubVector(&stTargetWp, _p_stTargetedPersoPos, _p_stWpPosition);
    MTH3D_M_vSubVector(&stAxe, _p_stDestWpPosition, _p_stWpPosition);

    xSqrNorm = MTH3D_M_xSqrVector(&stAxe);
    if (MTH_M_bIsNull(xSqrNorm))
    {
        return (MTH_C_InfinitPlus);
    }
    xDotProduct = MTH3D_M_xDotProductVector(&stAxe, &stTargetWp);
    xFactor = MTH_M_xDiv(xDotProduct, xSqrNorm);

    if (MTH_M_bLessZero(xFactor))
    {
        xDist1 = MTH3D_M_xVectorGap(_p_stWpPosition, _p_stTargetedPersoPos);
        xDist2 = MTH3D_M_xVectorGap(_p_stDestWpPosition, _p_stTargetedPersoPos);

        if (MTH_M_bGreater(xDist1, xDist2))
            return (xDist2);
        else
            return (xDist1);
    }

    MTH3D_M_vMulScalarVector(&stProjectedVector, xFactor, &stAxe);

    MTH3D_M_vAddVector(&stProjectedPoint, &stProjectedVector, _p_stWpPosition);

    return (MTH3D_M_xVectorGap(&stProjectedPoint, _p_stTargetedPersoPos));
}

void Cam_fn_vComputeMovingOnRail
(
	CAM_tdstUpdateCamera    *_p_stStruct,
	MTH3D_tdstVector		*_p_stWantedPosition,
	MTH3D_tdstVector		*_p_stWpPosition,
	MTH3D_tdstVector		*_p_stPreviousWpPosition,
	MTH_tdxReal				_xDistWanted
)
{

	MTH3D_tdstVector			stRailAxis;
	MTH3D_tdstVector			stTargetCamera;
	MTH3D_tdstVector			stVector;
	MTH3D_tdstVector			stVector1;
	MTH3D_tdstVector			stProjectedVector;
	MTH3D_tdstVector			stCameraNextWp;
	MTH3D_tdstVector			stProjectedPoint;
	MTH3D_tdstVector			stWpDestination;
	MTH_tdxReal					xFactor;
	MTH_tdxReal					xDotProduct;
	MTH_tdxReal					xNorm;
	MTH_tdxReal					xActualNorm;
	MTH_tdxReal					xSqrNorm;
	MTH_tdxReal					xDistWanted;
	char						cCameraIsOnWp = 0;

	CAM_fn_vComputeReferencePoint(_p_stStruct);
	MTH3D_M_vSubVector(&stCameraNextWp, _p_stWpPosition, &_p_stStruct->stCurrentCameraPos);
	MTH3D_M_vCopyVector(&stWpDestination, _p_stWpPosition);
	MTH3D_M_vSubVector(&stTargetCamera, &_p_stStruct->stTargetedPersoPos, &_p_stStruct->stCurrentCameraPos);

	xSqrNorm = MTH3D_M_xSqrVector(&stCameraNextWp);
	if (MTH_M_bIsNull(xSqrNorm))
	{
		MTH3D_M_vSubVector(&stCameraNextWp, &_p_stStruct->stCurrentCameraPos, _p_stPreviousWpPosition);
		MTH3D_M_vCopyVector(&stWpDestination, _p_stPreviousWpPosition);
		xSqrNorm = MTH3D_M_xSqrVector(&stCameraNextWp);
		if (MTH_M_bIsNull(xSqrNorm))
		{
			MTH3D_M_vCopyVector(_p_stWantedPosition, &_p_stStruct->stCurrentCameraPos);
			return;
		}
		cCameraIsOnWp = 1;
	}
	xDotProduct = MTH3D_M_xDotProductVector(&stCameraNextWp,  &stTargetCamera);
	xFactor = MTH_M_xDiv(xDotProduct, xSqrNorm);

	/* 0x0049CC88 */
	if (!MTH_M_bIsNullWithEpsilon(xFactor, (MTH_tdxReal)1.0e-4f))
	{

		if (cCameraIsOnWp)
			{MTH3D_M_vSubVector(&stCameraNextWp, &_p_stStruct->stCurrentCameraPos, &stWpDestination);}
		else
			{MTH3D_M_vSubVector(&stCameraNextWp, &stWpDestination, &_p_stStruct->stCurrentCameraPos);}

		MTH3D_M_vMulScalarVector(&stProjectedVector, xFactor, &stCameraNextWp);
		MTH3D_M_vSubVector(&stVector1, &stTargetCamera, &stProjectedVector);
		MTH3D_M_vAddVector(&stProjectedPoint, &_p_stStruct->stCurrentCameraPos, &stProjectedVector);
		xNorm = MTH3D_M_xSqrVector(&stVector1);
		xDistWanted = MTH_M_xSqr(_xDistWanted);
		xActualNorm = MTH3D_M_xVectorGap(&stProjectedPoint, &_p_stStruct->stCurrentCameraPos);

		if (!MTH_M_bGreater(xNorm, xDistWanted))
		{
			xNorm = MTH_M_xSqrt(MTH_M_xSub(xDistWanted, xNorm));
			if (MTH_M_bGreater(xNorm, xActualNorm))
			{
				xNorm = MTH_M_xSub(xNorm, xActualNorm);
				if (MTH_M_bGreaterZero(xFactor))
					{MTH3D_M_vCopyVector(&stWpDestination, _p_stPreviousWpPosition);}
				else
				{if (cCameraIsOnWp) {return;}}
			}
			else
			{
				xNorm = MTH_M_xSub(xActualNorm, xNorm);
				if (MTH_M_bLessZero(xFactor))
					{MTH3D_M_vCopyVector(&stWpDestination, _p_stPreviousWpPosition);}
				else
				{if (cCameraIsOnWp) {return;}}
			}
		}
		else
		{

			if (MTH_M_bLessZero(xFactor))
			{MTH3D_M_vCopyVector(&stWpDestination, _p_stPreviousWpPosition);}
			else
			{if (cCameraIsOnWp) return;}
			xNorm = xActualNorm;
		}

		MTH3D_M_vSubVector(&stRailAxis, &stWpDestination, &_p_stStruct->stCurrentCameraPos);
		if (MTH_M_bGreater(xNorm,MTH3D_M_xNormVector(&stRailAxis)))
		{
			MTH3D_M_vCopyVector(_p_stWantedPosition, &stWpDestination);
		}
		else
		{
			MTH3D_M_vNormalizeVector(&stRailAxis,&stRailAxis);
			MTH3D_M_vMulScalarVector(&stVector, xNorm, &stRailAxis);
			MTH3D_M_vAddVector(_p_stWantedPosition, &_p_stStruct->stCurrentCameraPos, &stVector);
		}
	}
}

void CAM_fn_vInitCameraStructure(HIE_tdstSuperObject *_hSuperObjPerso, CAM_tdstUpdateCamera *_p_stStruct)
{

	MS_tdxHandleToDynam			hDynam;
	MTH3D_tdstVector			stTempVector;
	DNM_tdstDynamics			*p_stDynamics;

	memset(_p_stStruct, 0, sizeof(CAM_tdstUpdateCamera));

	hDynam = M_GetMSHandle(_hSuperObjPerso, Dynam);

	_p_stStruct->p_stDynamics = fn_p_stDynamGetDynamics(hDynam);
	_p_stStruct->p_stReport = DNM_M_p_stDynamicsGetReport(_p_stStruct->p_stDynamics);

	_p_stStruct->hCineinfo = M_GetMSHandle(_hSuperObjPerso, Cineinfo);
	_p_stStruct->hCineinfoWork = M_GetCineinfo(_hSuperObjPerso, Work);
	_p_stStruct->hCineinfoCurrent = M_GetCineinfo(_hSuperObjPerso, Current);
	_p_stStruct->hCineinfoInit = M_GetCineinfo(_hSuperObjPerso, Init);
	_p_stStruct->hCineinfoVisibility = M_GetCineinfo(_hSuperObjPerso, Visibility);

	_p_stStruct->hSuperObjCamera = _hSuperObjPerso;

	POS_fn_vGetTranslationVector
	(
		HIE_fn_hGetSuperObjectGlobalMatrix(_hSuperObjPerso),
		&_p_stStruct->stCurrentCameraPos
	);

	if(_p_stStruct->hCineinfoWork->hSuperObjectTargeted != NULL)
	{

		_p_stStruct->cTgtPersoIsMovingAbsolute = CAM_fn_bPersoIsMoving
			(
				_p_stStruct,
				&_p_stStruct->stTgtPersoMoveAbsolute
			);

		if(CAM_fn_hGetFather(_p_stStruct->hCineinfoWork->hSuperObjectTargeted) == NULL)
		{
			_p_stStruct->cTgtPersoIsMovingRelative = _p_stStruct->cTgtPersoIsMovingAbsolute;
			MTH3D_M_vCopyVector(&_p_stStruct->stTgtPersoMoveRelative, &_p_stStruct->stTgtPersoMoveAbsolute);
		}
		else
		{
			hDynam = M_GetMSHandle(_p_stStruct->hCineinfoWork->hSuperObjectTargeted, Dynam);
			p_stDynamics = fn_p_stDynamGetDynamics(hDynam);
			POS_fn_vGetTranslationVector(DNM_M_p_stDynamicsGetPrevPreviousMatrix(p_stDynamics), &stTempVector);
			POS_fn_vGetTranslationVector(DNM_M_p_stDynamicsGetPreviousMatrix(p_stDynamics), &_p_stStruct->stTgtPersoMoveRelative);
			MTH3D_M_vSubVector(&_p_stStruct->stTgtPersoMoveRelative, &stTempVector, &_p_stStruct->stTgtPersoMoveRelative);
			if(MTH_M_bGreater(MTH3D_M_xNormVector(&_p_stStruct->stTgtPersoMoveRelative), CAM_C_xMinLinearSpeedTgtPerso))
				_p_stStruct->cTgtPersoIsMovingRelative = 1;
			else
				_p_stStruct->cTgtPersoIsMovingRelative = 0;
		}
	}
}

void CAM_fn_vSendParametersToParsing(CAM_tdstUpdateCamera *_p_stStruct)
{

	tdstMACDPID				*p_stMACDPID;
	DNM_tdstDynamics		*p_stDynamics;
	MS_tdxHandleToDynam		h_Dynam;

	if((h_Dynam = M_GetMSHandle(_p_stStruct->hSuperObjCamera, Dynam)) == NULL )
	  return;
	if((p_stDynamics = fn_p_stDynamGetDynamics(h_Dynam)) == NULL)
	  return;
	p_stMACDPID = DNM_M_p_stDynamicsGetExternalDatas(p_stDynamics);

	M_xMACDPIDGetCutAngleFactorPos(p_stMACDPID) = CAM_C_xCutAngleFactorPos;
	M_xMACDPIDGetCutAngleFactorTgt(p_stMACDPID) = CAM_C_xCutAngleFactorTgt;

	M_xMACDPIDSetTheta(p_stMACDPID, _p_stStruct->hCineinfoWork->xAngleTheta);

	MTH3D_M_vCopyVector(M_p_stMACDPIDGetTarget(p_stMACDPID), &_p_stStruct->stCptPos.stTarget);
	MTH3D_M_vCopyVector(M_p_stMACDPIDGetMovePos(p_stMACDPID), &_p_stStruct->stCptPos.stMovePos);

	M_xMACDPIDGetLinearSpeed(p_stMACDPID) = _p_stStruct->stCptPos.xLinearSpeed;
	if(MTH_M_bEqual(_p_stStruct->hCineinfoWork->xLinearIncreaseSpeed, MTH_C_ZERO))
		_p_stStruct->hCineinfoWork->xLinearIncreaseSpeed = MTH_M_xConstFloatToReal(0.001f);
	M_xMACDPIDGetLinearIncreaseSpeed(p_stMACDPID) = _p_stStruct->hCineinfoWork->xLinearIncreaseSpeed;
	M_xMACDPIDGetLinearDecreaseSpeed(p_stMACDPID) = _p_stStruct->hCineinfoWork->xLinearDecreaseSpeed;

	M_xMACDPIDGetAngularSpeed(p_stMACDPID) = _p_stStruct->stCptPos.xAngularSpeed;
	if(MTH_M_bEqual(_p_stStruct->hCineinfoWork->xAngularIncreaseSpeed, MTH_C_ZERO))

		_p_stStruct->hCineinfoWork->xLinearIncreaseSpeed = MTH_M_xConstFloatToReal(0.001f);
	M_xMACDPIDGetAngularIncreaseSpeed(p_stMACDPID) = _p_stStruct->hCineinfoWork->xAngularIncreaseSpeed;
	M_xMACDPIDGetAngularDecreaseSpeed(p_stMACDPID) = _p_stStruct->hCineinfoWork->xAngularDecreaseSpeed;

	M_xMACDPIDGetTargetSpeed(p_stMACDPID) = _p_stStruct->stCptPos.xTargetSpeed;

	if(MTH_M_bEqual(_p_stStruct->hCineinfoWork->xAngularIncreaseSpeed, MTH_C_ZERO))
		_p_stStruct->hCineinfoWork->xTargetIncreaseSpeed = MTH_M_xConstFloatToReal(0.001f);
	M_xMACDPIDGetTargetIncreaseSpeed(p_stMACDPID) = _p_stStruct->hCineinfoWork->xTargetIncreaseSpeed;
	M_xMACDPIDGetTargetDecreaseSpeed(p_stMACDPID) = _p_stStruct->hCineinfoWork->xTargetDecreaseSpeed;

	MTH3D_M_vCopyVector(M_p_stMACDPIDGetPosReferencePoint(p_stMACDPID), &_p_stStruct->stTargetedPersoPos);

	MTH3D_M_vCopyVector(M_p_stMACDPIDGetRefAxisZ(p_stMACDPID), &_p_stStruct->stCptPos.stRefAxisZ);

	if(_p_stStruct->hCineinfo->ucVolIAFlags & C_VolIAFlags_InitJustCopiedInCurrent)
	{
		_p_stStruct->hCineinfoWork->uwDNMFlags |= DNM_CAM_C_NoLinearParsing;
		_p_stStruct->hCineinfoWork->uwDNMFlags |= DNM_CAM_C_NoAngularParsing;
		_p_stStruct->hCineinfoWork->uwDNMFlags |= DNM_CAM_C_NoTargetParsing;
	}

	if(_p_stStruct->hCineinfoWork->hSuperObjectTargeted == NULL)
	{
		MTH3D_M_vNullVector(M_p_stMACDPIDGetPosReferencePoint(p_stMACDPID));
	}

	if(_p_stStruct->hCineinfoWork->uwIAFlags & C_IAFlags_PositionIsAlreadyComputed)
	{
		_p_stStruct->hCineinfoWork->uwDNMFlags |= DNM_CAM_C_NoAngularParsing;
		MTH3D_M_vNullVector(M_p_stMACDPIDGetPosReferencePoint(p_stMACDPID));
	}

	if(_p_stStruct->hCineinfoWork->uwDNMFlags & DNM_CAM_C_FixedOrientation)
		_p_stStruct->hCineinfoWork->uwDNMFlags |= DNM_CAM_C_NoTargetParsing;

	if(_p_stStruct->hCineinfoWork->uwDNMFlags & DNM_CAM_C_NoLinearParsing)
	{
		_p_stStruct->hCineinfoWork->uwDNMFlags |= DNM_CAM_C_NoLinearInertia;
		M_xMACDPIDGetCutAngleFactorPos(p_stMACDPID) = MTH_C_ONE;
	}

	if(_p_stStruct->hCineinfoWork->uwDNMFlags & DNM_CAM_C_NoAngularParsing)
	{
		_p_stStruct->hCineinfoWork->uwDNMFlags |= DNM_CAM_C_NoAngularInertia;
		M_xMACDPIDGetCutAngleFactorPos(p_stMACDPID) = MTH_C_ONE;
	}

	if(_p_stStruct->hCineinfoWork->uwDNMFlags & DNM_CAM_C_NoTargetParsing)
	{
		_p_stStruct->hCineinfoWork->uwDNMFlags |= DNM_CAM_C_NoTargetInertia;
		M_xMACDPIDGetCutAngleFactorTgt(p_stMACDPID) = MTH_C_ONE;
	}

	M_uwMACDPIDGetFlags(p_stMACDPID) = _p_stStruct->hCineinfoWork->uwDNMFlags;

	if((_p_stStruct->hCineinfoWork->hSuperObjectTargeted) && (_p_stStruct->hCineinfoWork->uwDNMFlags & DNM_CAM_C_NoObstacle))
	{

		fn_vSectInfoSetCurrentSector(M_GetMSHandle(_p_stStruct->hSuperObjCamera,SectInfo), fn_h_SectInfoGetCurrentSector(M_GetMSHandle(_p_stStruct->hCineinfoWork->hSuperObjectTargeted,SectInfo)));

	}

	if(!(_p_stStruct->cTgtPersoIsMovingAbsolute))
	{
		M_xMACDPIDGetCutAngleFactorPos(p_stMACDPID) = M_xMACDPIDGetCutAngleFactorTgt(p_stMACDPID) = MTH_C_ONE;
	}
}

void CAM_fn_vForceBestPosition(HIE_tdstSuperObject *_hCamera)
{
    MS_tdxHandleToCineinfo hCineinfo;
    CAM_tdstUpdateCamera stStruct;

    hCineinfo = M_GetMSHandle(_hCamera, Cineinfo);
    CAM_fn_vSetCineinfoWorkFromCurrent(hCineinfo);
    CAM_fn_vInitCameraStructure(_hCamera, &stStruct);

    cRefAxisIsAlreadyComputed = stStruct.hCineinfoWork->uwIAFlags & C_IAFlags_RefAxisIsAlreadyComputed ? 1 : 0;
    cNoDynChangeTheta = stStruct.hCineinfoWork->uwIAFlags & C_IAFlags_NoDynChangeTheta ? 1 : 0;
    (*(char *)AI_CAM_gp_cCanTestStatic) = 1;

    CAM_fn_vComputeReferencePoint(&stStruct);
    CAM_fn_vComputeTarget(&stStruct);
    CAM_fn_vComputeRealWantedPos(&stStruct);
    CAM_fn_vComputeMovePosWithDynTheta(&stStruct);

    if (stStruct.hCineinfoCurrent->hSuperObjectTargeted == NULL)
        return;

    CAM_fn_vForceFailureVisibility(&stStruct);
    CAM_fn_vComputeFailureVisibility(&stStruct);
}
