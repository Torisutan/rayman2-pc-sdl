#include <stddef.h>
#include <stdlib.h>
#include <string.h>

#include "AI/CAM_Base.h"
#include "GAM/GAM_types.h"
#include "HIE/HIE_types.h"
#include "MTH/MTH3D.h"
#include "MTH/MTH_flt.h"
#include "AI/AI_globals.h"

extern MTH3D_tdstVector *MTH_p_stRotationMulVector(MTH_tdstRotation *_p_stRotation,
                                                   MTH3D_tdstVector *_p_stSource,
                                                   MTH3D_tdstVector *_p_stResult);

#define AI_CAM_c_CanTestStatic            (*(char *)AI_CAM_gp_cCanTestStatic)
#define cCanTestStatic                    AI_CAM_c_CanTestStatic

typedef HIE_tdstSuperObject *HIE_tdxHandleToSuperObject;

#define MTH_M_RotationSetAxis(_p_stRotation,_p_stVector) MTH3D_M_vCopyVector(&(_p_stRotation)->m_stAxis,_p_stVector)

#define C_IAFlags_NoVisibilityWithDynHie    0x0010
#define GAM_C_CustBitCameraHit              0x02000000
#define HIE_M_lGetNumberOfChildren(_hSprObj)    ((_hSprObj)->lNumberOfChildren)

#define CAM_C_xRayCameraSphereToGo1         (CAM_g_stCameraConstants.xRayCameraSphereToGo1)
#define CAM_C_xRayCameraSphereToGo2         (CAM_g_stCameraConstants.xRayCameraSphereToGo2)
#define CAM_C_xRayCameraSphereToSee1        (CAM_g_stCameraConstants.xRayCameraSphereToSee1)
#define CAM_C_xRayCameraSphereToSee2        (CAM_g_stCameraConstants.xRayCameraSphereToSee2)
#define CAM_C_xNumRayCameraCanGo            (CAM_g_stCameraConstants.xNumRayCameraCanGo)
#define CAM_C_xNumRayCameraCanSee           (CAM_g_stCameraConstants.xNumRayCameraCanSee)

typedef struct CAM_tdstSOSectorsInCollisionInteraction_
{
    HIE_tdstSuperObject *hPointerOfSectorSO;
    struct CAM_tdstSOSectorsInCollisionInteraction_ *hNextBrotherSLk;
} CAM_tdstSOSectorsInCollisionInteraction;

typedef CAM_tdstSOSectorsInCollisionInteraction *CAM_tdxHandleOfElementLstCollisionInteraction;

typedef struct CAM_tdstSOCharacters_ *CAM_tdxHandleOfElementLstCharacter;
struct CAM_tdstSOCharacters_
{
    HIE_tdxHandleToSuperObject  hPointerOfCharacter;
    CAM_tdxHandleOfElementLstCharacter hNextBrotherDyn;
    CAM_tdxHandleOfElementLstCharacter hPrevBrotherDyn;
    CAM_tdxHandleOfElementLstCharacter hFatherDyn;
};

typedef struct CAM_tdstSectorCollisionPart_
{
    CAM_tdxHandleOfElementLstCharacter              hFirstCharDyn;
    CAM_tdxHandleOfElementLstCharacter              hLastCharDyn;
    long                                            lNumberOfCharDyn;
    unsigned char                                   aucAnchorsBetween[0x30 - 0x0C];
    CAM_tdxHandleOfElementLstCollisionInteraction   hFirstElementSLk;
    CAM_tdxHandleOfElementLstCollisionInteraction   hLastElementSLk;
    long                                            lNumberOfElementsSLk;
} CAM_tdstSectorCollisionPart;

#define SECT_fn_hGetFirstCharListInSector(_hSector) \
    (((CAM_tdstSectorCollisionPart *)HIE_fn_hGetSuperObjectObject(_hSector))->hFirstCharDyn)
#define SECT_fn_hGetNextCharListInSector(_hCharList) \
    ((_hCharList)->hNextBrotherDyn)
#define SECT_M_ForEachCharListInSector(_hSector,_hCharList,_i) \
for														\
(														\
	_i=0,(_hCharList)=SECT_fn_hGetFirstCharListInSector(_hSector);		\
	_hCharList;									\
	_i++,_hCharList=SECT_fn_hGetNextCharListInSector(_hCharList)			\
)
#define SECT_GetCharacterInList(_hCharacterList) \
    ((_hCharacterList)->hPointerOfCharacter)

#define SECT_fn_iGetNumberOfNodesInCollisionList(_hSector) \
    (((CAM_tdstSectorCollisionPart *)HIE_fn_hGetSuperObjectObject(_hSector))->lNumberOfElementsSLk)

#define SECT_fn_hGetFirstCollisionNodeInCollisionList(_hSector) \
    (((CAM_tdstSectorCollisionPart *)HIE_fn_hGetSuperObjectObject(_hSector))->hFirstElementSLk)

#define SECT_fn_hGetNextCollisionNodeInCollisionList(_hCollNode) \
    ((_hCollNode)->hNextBrotherSLk)

#define SECT_GetSectorInCollisionList(_hCollisionList) \
    ((_hCollisionList)->hPointerOfSectorSO)

#define SECT_M_ForEachCollisionNodeInCollisionInteractionList(_hSector,_hCollList,_i) \
for														\
(														\
	_i=SECT_fn_iGetNumberOfNodesInCollisionList(_hSector), \
	(_hCollList)=SECT_fn_hGetFirstCollisionNodeInCollisionList(_hSector);		\
	_i>0; 									\
	_i--,(_hCollList)=SECT_fn_hGetNextCollisionNodeInCollisionList(_hCollList)			\
)

#define M_Trunc(Vector, Type)\
{\
	int _iTemp_;\
	_iTemp_ = MTH_M_xRealToLong(MTH_M_xMul(MTH3D_M_xGet##Type##ofVector(&Vector), MTH_M_xLongToReal(1000)));\
	MTH3D_M_vSet##Type##ofVector(&Vector, MTH_M_xDiv(MTH_M_xLongToReal(_iTemp_), MTH_M_xLongToReal(1000)));\
}

typedef void *MS_tdxHandleToSectInfo;
typedef void *MS_tdxHandleToCollSet;

extern void *__cdecl fn_h_SectInfoGetCurrentSector(void *h_SectInfo);

char CAM_fn_cIsSectorInCollisionList(HIE_tdxHandleToSuperObject _hFirstSector, HIE_tdxHandleToSuperObject _hTestSector);
char CAM_fn_cSendSegmentInDynamicWorld(MTH3D_tdstVector *_p_stSource, MTH3D_tdstVector *_p_stDest, HIE_tdxHandleToSuperObject _hSector, HIE_tdxHandleToSuperObject _hCamera);
char CAM_fn_cSendSegment(CAM_tdstUpdateCamera *_p_stStruct, MTH3D_tdstVector *_p_stSource, MTH3D_tdstVector *_p_stDest);
char CAM_fn_cSendMultiSegment(CAM_tdstUpdateCamera *_p_stStruct, MTH3D_tdstVector *_p_stSourcePos, MTH3D_tdstVector *_p_stTargetPos, MTH_tdxReal _xRadiusCamera, MTH_tdxReal _xRadiusTarget, long _lNbRay);
char CAM_fn_cCameraCantGo(CAM_tdstUpdateCamera *_p_stStruct, MTH3D_tdstVector *_p_stSourcePos, MTH3D_tdstVector *_p_stTargetPos);
char CAM_fn_cCameraCantSee(CAM_tdstUpdateCamera *_p_stStruct, MTH3D_tdstVector *_p_stSourcePos, MTH3D_tdstVector *_p_stTargetPos);

char CAM_fn_cIsSectorInCollisionList(HIE_tdxHandleToSuperObject	_hFirstSector, HIE_tdxHandleToSuperObject _hTestSector)
{

	int												i;
	CAM_tdxHandleOfElementLstCollisionInteraction	hList;

	SECT_M_ForEachCollisionNodeInCollisionInteractionList(_hFirstSector, hList, i)
	{
		if(SECT_GetSectorInCollisionList(hList) == _hTestSector)
			return 1;
	}
	return 0;
}

char CAM_fn_cSendSegmentInDynamicWorld
(
	MTH3D_tdstVector			*_p_stSource,
	MTH3D_tdstVector			*_p_stDest,
	HIE_tdxHandleToSuperObject  _hSector,
	HIE_tdxHandleToSuperObject  _hCamera
)
{

	long								j;
	CAM_tdxHandleOfElementLstCharacter	hCharacterList;
	HIE_tdxHandleToSuperObject			hTreatCharacter;
	unsigned char						ucCameraPriority;
	MTH3D_tdstVector					stTempVector;
	MS_tdxHandleToCollSet				hCollset;
	MS_tdxHandleToStandardGame			hStdGame;

	ucCameraPriority = CS_fn_ucGetCharacterPriority(M_GetMSHandle(_hCamera, CollSet));
	MTH3D_M_vSubVector(&stTempVector, _p_stDest, _p_stSource);
	SECT_M_ForEachCharListInSector(_hSector, hCharacterList, j)
	{
		hTreatCharacter = SECT_GetCharacterInList(hCharacterList);
		hCollset = M_GetMSHandle(hTreatCharacter, CollSet);
		hStdGame = M_GetMSHandle(hTreatCharacter, StandardGame);

		if ( (hCollset && (CS_fn_ucGetCharacterPriority(hCollset) == ucCameraPriority)) ||
			 (hStdGame && (fn_ulStandardGameGetCustomBits(hStdGame) & GAM_C_CustBitCameraHit)) )

		{

			{

				if(HIE_M_lGetNumberOfChildren(hTreatCharacter))
				{
					if
					(
						HIE_bDetectIntersectSegmentWithSuperObject
						(
							_p_stSource,

							&stTempVector,
							hTreatCharacter
						)
					)
						return 1;
				}
			}
		}
	}
	return 0;
}

char CAM_fn_cSendSegment
(
	CAM_tdstUpdateCamera	*_p_stStruct,
	MTH3D_tdstVector		*_p_stSource,
	MTH3D_tdstVector		*_p_stDest
)
{

	MTH3D_tdstVector								stTempVector;
	int												i;
	MS_tdxHandleToSectInfo							hPersoCineinfo;
	MS_tdxHandleToSectInfo							hCamCineinfo;
	CAM_tdxHandleOfElementLstCollisionInteraction	hList;
	MTH3D_tdstVector								stSource, stDest;
	HIE_tdxHandleToSuperObject						hPersoTargeted;
	HIE_tdxHandleToSuperObject						hCamera;
	HIE_tdxHandleToSuperObject						hCurrentSectorPerso;
	HIE_tdxHandleToSuperObject						hCurrentSectorCamera;
	HIE_tdxHandleToSuperObject						hSectorInCollision;
	char											cVisibilityWithDynWorld;

	if(MTH3D_M_bEqualVector(_p_stSource, _p_stDest))
		return 0;

	cVisibilityWithDynWorld = _p_stStruct->hCineinfoWork->uwIAFlags & C_IAFlags_NoVisibilityWithDynHie ? 0 : 1;

	stSource = *_p_stSource;
	stDest = *_p_stDest;
	M_Trunc(stSource, X);
	M_Trunc(stSource, Y);
	M_Trunc(stSource, Z);
	M_Trunc(stDest, X);
	M_Trunc(stDest, Y);
	M_Trunc(stDest, Z);

	hCamera = _p_stStruct->hSuperObjCamera;
	hCamCineinfo = M_GetMSHandle(hCamera, SectInfo);
	hCurrentSectorPerso = hCurrentSectorCamera = fn_h_SectInfoGetCurrentSector(hCamCineinfo);

	hPersoTargeted = _p_stStruct->hCineinfoWork->hSuperObjectTargeted;
	if(hPersoTargeted)
	{
		hPersoCineinfo = M_GetMSHandle(hPersoTargeted, SectInfo);
		hCurrentSectorPerso = fn_h_SectInfoGetCurrentSector(hPersoCineinfo);
	}

	MTH3D_M_vSubVector(&stTempVector, &stDest, &stSource);

	if
	(
		cCanTestStatic
	&&	HIE_bDetectIntersectSegmentWithSuperObject
		(
			&stSource,

			&stTempVector,
			hCurrentSectorPerso
		)
	)
		return 1;

	if
	(
		(cVisibilityWithDynWorld)
	&&	CAM_fn_cSendSegmentInDynamicWorld
		(
			&stSource,
			&stDest,
			hCurrentSectorPerso,
			hCamera
		)
	)
		return 1;

	if(hCurrentSectorPerso != hCurrentSectorCamera)
	{

		if
		(
			cCanTestStatic
		&&	HIE_bDetectIntersectSegmentWithSuperObject
			(
				&stSource,

				&stTempVector,
				hCurrentSectorCamera
			)
		)
			return 1;

		if
		(
			(cVisibilityWithDynWorld)
		&&	CAM_fn_cSendSegmentInDynamicWorld
			(
				&stSource,
				&stDest,
				hCurrentSectorCamera,
				hCamera
			)
		)
			return 1;
	}

	SECT_M_ForEachCollisionNodeInCollisionInteractionList
	(
		hCurrentSectorPerso,
		hList,
		i
	)
	{
		hSectorInCollision = SECT_GetSectorInCollisionList(hList);

		if(hSectorInCollision != hCurrentSectorCamera)
		{

			if
			(
				cCanTestStatic
			&&	HIE_bDetectIntersectSegmentWithSuperObject
				(
					&stSource,

					&stTempVector,
					hSectorInCollision
				)
			)
				return 1;

			if
			(
				(cVisibilityWithDynWorld)
			&&	CAM_fn_cSendSegmentInDynamicWorld
				(
					&stSource,
					&stDest,
					hSectorInCollision,
					hCamera
				)
			)
				return 1;
		}
	}

	if(hCurrentSectorPerso != hCurrentSectorCamera)
	{
		SECT_M_ForEachCollisionNodeInCollisionInteractionList
		(
			hCurrentSectorCamera,
			hList,
			i
		)
		{
			hSectorInCollision = SECT_GetSectorInCollisionList(hList);
			if
			(

				!CAM_fn_cIsSectorInCollisionList
				(
					hCurrentSectorPerso,
					hSectorInCollision
				)

			&&	(hSectorInCollision != hCurrentSectorPerso)
			)
			{

				if
				(
					cCanTestStatic
				&&	HIE_bDetectIntersectSegmentWithSuperObject
					(
						&stSource,

						&stTempVector,
						hSectorInCollision
					)
				)
					return 1;

				if
				(
					(cVisibilityWithDynWorld)
				&&	CAM_fn_cSendSegmentInDynamicWorld
					(
						&stSource,
						&stDest,
						hSectorInCollision,
						hCamera
					)
				)
					return 1;
			}
		}
	}

	return 0;
}

char CAM_fn_cSendMultiSegment
(
	CAM_tdstUpdateCamera	*_p_stStruct,
	MTH3D_tdstVector		*_p_stSourcePos,
	MTH3D_tdstVector		*_p_stTargetPos,
	MTH_tdxReal				_xRadiusCamera,
	MTH_tdxReal				_xRadiusTarget,
	long					_lNbRay
)
{

	MTH3D_tdstVector		stDirection;
	MTH3D_tdstVector		stPositionSource, stPositionSource1;
	MTH3D_tdstVector		stPositionTarget, stPositionTarget1;
	MTH_tdstRotation		stRotation;
	MTH3D_tdstVector		stAxisZ, stVector;
	long					i;

	if(_lNbRay == 1)
	{
		return CAM_fn_cSendSegment
		(
			_p_stStruct,
			_p_stSourcePos,
			_p_stTargetPos
		);
	}

	MTH3D_M_vSubVector(&stDirection, _p_stTargetPos, _p_stSourcePos);
	MTH3D_M_vNormalizeVector(&stDirection, &stDirection);

	MTH3D_M_vSetBaseKVector(&stAxisZ);
	MTH3D_M_vCrossProductVectorWithoutBuffer(&stVector, &stDirection, &stAxisZ);
	if(MTH3D_M_bIsNullVector(&stVector))
	{
		MTH3D_M_vSetBaseIVector(&stAxisZ);
		MTH3D_M_vCrossProductVectorWithoutBuffer(&stVector, &stDirection, &stAxisZ);
	}
	MTH3D_M_vNormalizeVector(&stVector, &stVector);
	MTH3D_M_vMulScalarVector(&stVector, _xRadiusCamera, &stVector);

	MTH_M_RotationSetAxis(&stRotation, &stDirection);

	MTH_M_xRotationSetAngle(&stRotation, MTH_M_xDiv(MTH_C_2Pi, _lNbRay));

	for(i = 0; i < _lNbRay; i++)
	{

		MTH3D_M_vMulScalarVector(&stPositionSource, _xRadiusCamera, &stVector);
		MTH3D_M_vAddVector(&stPositionSource1, &stPositionSource, _p_stSourcePos);

		MTH3D_M_vMulScalarVector(&stPositionTarget, _xRadiusTarget, &stVector);
		MTH3D_M_vAddVector(&stPositionTarget1, &stPositionTarget, _p_stTargetPos);

		if
		(
			CAM_fn_cSendSegment
			(
				_p_stStruct,
				&stPositionSource1,
				&stPositionTarget1
			)
		)
			return 1;

		MTH_p_stRotationMulVector(&stRotation, &stVector, &stVector);
	}
	return 0;
}

char CAM_fn_cCameraCantGo
(
	CAM_tdstUpdateCamera	*_p_stStruct,
	MTH3D_tdstVector		*_p_stSourcePos,
	MTH3D_tdstVector		*_p_stTargetPos
)
{
	return CAM_fn_cSendMultiSegment
	(
		_p_stStruct,
		_p_stSourcePos,
		_p_stTargetPos,
		CAM_C_xRayCameraSphereToGo1,
		CAM_C_xRayCameraSphereToGo2,
		CAM_C_xNumRayCameraCanGo
	);
}

char CAM_fn_cCameraCantSee
(
	CAM_tdstUpdateCamera	*_p_stStruct,
	MTH3D_tdstVector		*_p_stSourcePos,
	MTH3D_tdstVector		*_p_stTargetPos
)
{
	return CAM_fn_cSendMultiSegment
	(
		_p_stStruct,
		_p_stSourcePos,
		_p_stTargetPos,
		CAM_C_xRayCameraSphereToSee1,
		CAM_C_xRayCameraSphereToSee2,
		CAM_C_xNumRayCameraCanSee
	);
}

static char CAM_fn_cCameraCantGoFailure
(
	CAM_tdstUpdateCamera	*_p_stStruct,
	MTH3D_tdstVector		*_p_stSourcePos,
	MTH3D_tdstVector		*_p_stTargetPos
)
{

	MTH3D_tdstVector	stTempVector;
	MTH_tdxReal			xNorm;

	MTH3D_M_vSubVector(&stTempVector, _p_stTargetPos, _p_stSourcePos);
	xNorm = MTH3D_M_xNormVector(&stTempVector);
	xNorm = MTH_M_xAdd(xNorm, MTH_M_xConstFloatToReal(0.35f));
	MTH3D_M_vNormalizeVector(&stTempVector, &stTempVector);
	MTH3D_M_vMulScalarVector(&stTempVector, xNorm, &stTempVector);
	MTH3D_M_vAddVector(&stTempVector, &stTempVector, _p_stSourcePos);

	return CAM_fn_cSendMultiSegment
	(
		_p_stStruct,
		_p_stSourcePos,
		&stTempVector,
		CAM_C_xRayCameraSphereToGo1,
		CAM_C_xRayCameraSphereToGo2,
		CAM_C_xNumRayCameraCanGo
	);
}

#define C_mode_MUSTSEE					1
#define C_mode_MUSTGO					2
#define C_mode_FAILURE					4
#define C_mode_MORE_CORRECT				8

#define CAM_C_xMinDistPerso							(_p_stStruct->hCineinfoWork->xBoundDistMin)
#define CAM_C_xMinDistPersoToComputePos				(MTH_M_xAdd(CAM_C_xMinDistPerso, CAM_g_stCameraConstants.xOffsetMinDistPerso))

char CAM_fn_cIsWantedPosIncorrect
(
	CAM_tdstUpdateCamera	*_p_stStruct,
	MTH3D_tdstVector		*_p_stWantedPos,
	char					_cMode
)
{

	MTH3D_tdstVector	stTempVector;
	char				cCant;

	MTH3D_M_vSubVector(&stTempVector, _p_stWantedPos, &_p_stStruct->stTargetedPersoRealPos);
	if(MTH_M_bLess(MTH3D_M_xNormVector(&stTempVector), CAM_C_xMinDistPersoToComputePos))
		return 1;

	if(_cMode & C_mode_MUSTSEE)
	{

		cCant = ((_cMode & C_mode_FAILURE) ? CAM_fn_cCameraCantGoFailure : CAM_fn_cCameraCantSee) (_p_stStruct, &_p_stStruct->stTargetedPersoRealPos, _p_stWantedPos);

		if(cCant)
			return 1;
	}

	if(_cMode & C_mode_MUSTGO)
	{
		cCant = CAM_fn_cCameraCantGo(_p_stStruct, &_p_stStruct->stCurrentCameraPos, _p_stWantedPos);

		if(cCant)
			return 1;
	}

	return 0;
}

char CAM_fn_cFindCorrectPosWithDir
(
	CAM_tdstUpdateCamera	*_p_stStruct,
	MTH3D_tdstVector		*_p_stDir,
	MTH3D_tdstVector		*_p_stCompute,
	char					_cMode
)
{

	MTH3D_tdstVector	stTestedPos;

	MTH3D_M_vAddVector(&stTestedPos, _p_stDir, &_p_stStruct->stTargetedPersoPos);

	if(CAM_fn_cIsWantedPosIncorrect(_p_stStruct, &stTestedPos, _cMode))
		return 0;

	else
	{
		MTH3D_M_vCopyVector(_p_stCompute, &stTestedPos);
		return 1;
	}
}

#define CAM_e_State_GoToOptimal                 0
#define C_PerIAFlags_ConstraintMoveVisFailure   0x08

#define CAM_C_xTickTimeFindBetterPosIfNoMove \
    (MTH_M_xMul(MTH_M_xMul(MTH_M_xLongToReal(CAM_g_stCameraConstants.xTickFindBetterPosIfNoMove),MTH_M_xConstFloatToReal(1000)),GAM_g_xFrameLength))

#pragma optimize("y", off)
char CAM_fn_cCanResetConstraintMoveVisFailure(CAM_tdstUpdateCamera *_p_stStruct, MTH3D_tdstVector *_p_stWanted)
{

	MTH3D_tdstVector	stTempVector, stTempVector1;
	MTH_tdxReal			xNorm, xNorm1;

	if(_p_stStruct->hCineinfo->eState == CAM_e_State_GoToOptimal)
		return 1;

	if(MTH_M_bGreaterEqual(_p_stStruct->hCineinfo->xCounter4,CAM_C_xTickTimeFindBetterPosIfNoMove))
		return 1;

	if(!(_p_stStruct->hCineinfo->ucPerIAFlags & C_PerIAFlags_ConstraintMoveVisFailure))
		return 1;

	MTH3D_M_vSubVector
	(
		&stTempVector,
		_p_stWanted,
		&_p_stStruct->hCineinfo->stLastFailedOrigin
	);
	xNorm = MTH3D_M_xNormVector(&stTempVector);

	MTH3D_M_vSubVector
	(
		&stTempVector1,
		&_p_stStruct->hCineinfo->stLastFailedWanted,
		&_p_stStruct->hCineinfo->stLastFailedOrigin
	);
	xNorm1 = MTH3D_M_xNormVector(&stTempVector1);

	if(MTH_M_bLessEqual(xNorm, MTH_M_xAdd(xNorm1, (double)MTH_C_Inv2)))
		return 0;

	return 1;
}
#pragma optimize("", on)

#define C_PerIAFlags_NoShift                    0x04

void CAM_fn_vComputeMovePosWithDynTheta(CAM_tdstUpdateCamera *_p_stStruct);

void CAM_fn_vRestoreNormalState(CAM_tdstUpdateCamera *_p_stStruct)
{

	_p_stStruct->hCineinfo->eState = CAM_e_State_GoToOptimal;

	CAM_fn_vSetCineinfoCurrentFromVisibility(_p_stStruct->hCineinfo);
	CAM_fn_vSetCineinfoWorkFromVisibility(_p_stStruct->hCineinfo);

	_p_stStruct->hCineinfo->ucPerIAFlags |= C_PerIAFlags_NoShift;
	_p_stStruct->hCineinfo->ucPerIAFlags &= ~C_PerIAFlags_ConstraintMoveVisFailure;

	CAM_fn_vComputeMovePosWithDynTheta(_p_stStruct);
}

#define CAM_C_xEpsilonLastMove  ((MTH_tdxReal)1.0e-4f)

#define CAM_C_xEpsilonForEqualVectors       (CAM_g_stCameraConstants.xEpsilonForEqualVectors)

ACP_tdxBool CAM_fn_bEqualVectorWithEpsilon(MTH3D_tdstVector *_p_stVector1, MTH3D_tdstVector *_p_stVector2, MTH_tdxReal _xEpsilon);

char CAM_fnc_IsDirectionCorrect
(
	CAM_tdstUpdateCamera	*_p_stStruct,
	MTH3D_tdstVector		*_pst_Dir,
	MTH_tdxReal				_x_Pound,
	MTH_tdxReal				_x_PoundRef,
	char					_c_Mode
)
{

	MTH3D_tdstVector	stTempVector;

	MTH3D_M_vAddVector(&stTempVector, _pst_Dir, &_p_stStruct->stTargetedPersoRealPos);

	if(_c_Mode & C_mode_MORE_CORRECT)
	{

		if(MTH_M_bGreaterEqual(_x_Pound, _x_PoundRef))
			return 0;

		_x_PoundRef = MTH_M_xSub(_x_PoundRef, _x_Pound);
		if(MTH_M_bLess(_x_PoundRef, MTH_M_xConstFloatToReal(0.05f)))
			return 0;

		if(!CAM_fn_cCanResetConstraintMoveVisFailure(_p_stStruct, &stTempVector))
			return 0;
	}

	if(MTH_M_bLess(MTH3D_M_xNormVector(_pst_Dir), CAM_C_xMinDistPersoToComputePos))
		return 0;

	if
	(
		(!(_c_Mode & C_mode_FAILURE))
	&&	CAM_fn_bEqualVectorWithEpsilon
		(
			&stTempVector,
			&_p_stStruct->stWantedCameraPos,
			CAM_C_xEpsilonForEqualVectors
		)
	)
		return 0;

	if
	(
		((!(_c_Mode & C_mode_FAILURE)) || (_c_Mode & C_mode_MORE_CORRECT))
	&&	CAM_fn_bEqualVectorWithEpsilon
		(
			&stTempVector,
			&_p_stStruct->stCurrentCameraPos,
			CAM_C_xEpsilonForEqualVectors
		)
	)
		return 0;

	return 1;
}

void CAM_fn_vComputeAnglesBetweenTwoVectors(MS_tdxHandleToInternalCineinfo _hIntCineInfo, MTH3D_tdstVector *_p_stSourceVector, MTH3D_tdstVector *_p_stTargetVector, MTH_tdxReal *_p_xHorizontalAngle, MTH_tdxReal *_p_xVerticalAngle, MTH3D_tdstVector *_p_stRefAxisZ);
void CAM_fn_vComputeRefAxis(CAM_tdstUpdateCamera *_p_stStruct, MTH3D_tdstVector *_p_stRefAxis);

void CAM_fn_vComputeCineinfoWorkForAPosition(CAM_tdstUpdateCamera *_p_stStruct, MTH3D_tdstVector *_p_stPosition)
{

	MTH3D_tdstVector				stDir;
	MTH3D_tdstVector				stCamTargetVector;
	MTH_tdxReal						xAlphaAngle, xThetaAngle;
	MTH_tdxReal						xNorm;

	CAM_fn_vComputeRefAxis(_p_stStruct, &stDir);

	MTH3D_M_vSubVector(&stCamTargetVector, _p_stPosition, &_p_stStruct->stTargetedPersoPos);
	xNorm = MTH3D_M_xNormVector(&stCamTargetVector);

	CAM_fn_vComputeAnglesBetweenTwoVectors
	(
		_p_stStruct->hCineinfoWork,
		&stDir,
		&stCamTargetVector,
		&xAlphaAngle,
		&xThetaAngle,
		&_p_stStruct->stCptPos.stRefAxisZ
	);

	_p_stStruct->hCineinfoWork->xDistMin = xNorm;
	_p_stStruct->hCineinfoWork->xDistMax = xNorm;
	_p_stStruct->hCineinfoWork->xAngleAlpha = xAlphaAngle;
	_p_stStruct->hCineinfoWork->xAngleTheta = xThetaAngle;
}

/* 0x004874F0 */
void CAM_fn_vComputeCineinfoCurrentForAPosition(CAM_tdstUpdateCamera *_p_stStruct, MTH3D_tdstVector *_p_stPosition)
{

	struct tdstInternalStructurCineinfo_	*p_stMemo;

	p_stMemo = _p_stStruct->hCineinfoWork;
	_p_stStruct->hCineinfoWork = _p_stStruct->hCineinfoCurrent;
	CAM_fn_vComputeCineinfoWorkForAPosition(_p_stStruct, _p_stPosition);
	_p_stStruct->hCineinfoWork = p_stMemo;
}

/* 0x004E9280 */
struct tdstInternalStructurCineinfo_		gst_Ideal;

#undef MTH_M_xRealToLongRound
#define MTH_M_xRealToLongRound(A)               \
 (long) ( (long)(A*2.0F)-(long)(A) )

MTH_tdxReal	CAM_fnx_ComputePosPound(CAM_tdstUpdateCamera *_pst_Struct, MTH3D_tdstVector *_pst_Dir)
{

	MTH_tdxReal								x_Pound, x_Angle, x_Angle1;
	MTH_tdxReal								x_P1, x_P2, x_P3;
	MTH3D_tdstVector						st_TempVector;
	struct tdstInternalStructurCineinfo_	*pst_Memo;
	struct tdstInternalStructurCineinfo_	st_Temp;

	MTH3D_M_vAddVector(&st_TempVector, &_pst_Struct->stTargetedPersoPos, _pst_Dir);

	pst_Memo = _pst_Struct->hCineinfoWork;

	st_Temp.hSuperObjectTargeted = _pst_Struct->hCineinfoWork->hSuperObjectTargeted;
	st_Temp.hSecondSuperObjectTargeted = _pst_Struct->hCineinfoWork->hSecondSuperObjectTargeted;
	st_Temp.cChannel = _pst_Struct->hCineinfoWork->cChannel;

	_pst_Struct->hCineinfoWork = &st_Temp;
	CAM_fn_vComputeCineinfoWorkForAPosition(_pst_Struct, &st_TempVector);
	_pst_Struct->hCineinfoWork = pst_Memo;

	x_Angle = MTH_M_bGreater(gst_Ideal.xAngleAlpha, MTH_C_Pi) ? MTH_M_xSub(MTH_C_2Pi, gst_Ideal.xAngleAlpha) : gst_Ideal.xAngleAlpha;
	x_Angle1 = MTH_M_bGreater(st_Temp.xAngleAlpha, MTH_C_Pi) ? MTH_M_xSub(MTH_C_2Pi, st_Temp.xAngleAlpha) : st_Temp.xAngleAlpha;
	x_P1 = MTH_M_xAbs(MTH_M_xSub(x_Angle, x_Angle1));

	x_P1 = MTH_M_xLongToReal(MTH_M_xRealToLongRound(MTH_M_xMul(x_P1, MTH_M_xLongToReal(100))));
	x_P1 = MTH_M_xMul(x_P1, MTH_M_xConstFloatToReal(0.01f));

	x_Angle = MTH_M_bGreater(gst_Ideal.xAngleTheta, MTH_C_Pi) ? MTH_M_xSub(MTH_C_2Pi, gst_Ideal.xAngleTheta) : gst_Ideal.xAngleTheta;
	x_Angle1 = MTH_M_bGreater(st_Temp.xAngleTheta, MTH_C_Pi) ? MTH_M_xSub(st_Temp.xAngleTheta, MTH_C_2Pi) : st_Temp.xAngleTheta;
	x_P2 = MTH_M_xAbs(MTH_M_xSub(x_Angle, x_Angle1));

	x_P2 = MTH_M_xLongToReal(MTH_M_xRealToLongRound(MTH_M_xMul(x_P2, MTH_M_xLongToReal(100))));

	x_P2 = MTH_M_xMul(x_P2, MTH_M_xConstFloatToReal(0.10f));

	x_P3 = MTH_M_xAbs(MTH_M_xSub(gst_Ideal.xDistMin, st_Temp.xDistMin));

	x_P3 = MTH_M_xLongToReal(MTH_M_xRealToLongRound(MTH_M_xMul(x_P3, MTH_M_xLongToReal(100))));
	x_P3 = MTH_M_xMul(x_P3, MTH_M_xConstFloatToReal(0.01f));

	x_Pound = MTH_M_xAdd(x_P1, x_P2);
	x_Pound = MTH_M_xAdd(x_Pound, MTH_M_xMul(x_P3, MTH_M_xConstFloatToReal(0.1f)));

	return x_Pound;
}

#define CAM_e_State_GoToComputedOptimal         1
void CAM_fn_vSetCineinfoVisibilityFromCurrent(MS_tdxHandleToCineinfo _hCineinfo);
void CAM_fn_vComputeMovePos(CAM_tdstUpdateCamera *_p_stStruct);

void CAM_fn_vMakeAsNewIdealPosition(CAM_tdstUpdateCamera *_p_stStruct, MTH3D_tdstVector *_p_stPosition)
{

	_p_stStruct->hCineinfo->cLastCutAngleSens = 0;

	CAM_fn_vSetCineinfoVisibilityFromCurrent(_p_stStruct->hCineinfo);

	CAM_fn_vComputeCineinfoCurrentForAPosition(_p_stStruct, _p_stPosition);

	_p_stStruct->hCineinfo->eState = CAM_e_State_GoToComputedOptimal;

	_p_stStruct->hCineinfo->ucPerIAFlags |= C_PerIAFlags_NoShift;

	_p_stStruct->hCineinfoWork->xAngleAlpha = _p_stStruct->hCineinfoCurrent->xAngleAlpha;
	_p_stStruct->hCineinfoWork->xAngleTheta = _p_stStruct->hCineinfoCurrent->xAngleTheta;
	_p_stStruct->hCineinfoWork->xDistMin = _p_stStruct->hCineinfoCurrent->xDistMin;
	_p_stStruct->hCineinfoWork->xDistMax = _p_stStruct->hCineinfoCurrent->xDistMax;

	CAM_fn_vComputeMovePos(_p_stStruct);

	MTH3D_M_vCopyVector(&_p_stStruct->hCineinfo->stLastFailedOrigin, &_p_stStruct->stCurrentCameraPos);
	MTH3D_M_vCopyVector(&_p_stStruct->hCineinfo->stLastFailedWanted, _p_stPosition);
}

unsigned short		guw_NbVectorArrayCpt = 0;
typedef struct
{
	MTH3D_tdstVector	stDir;
	MTH_tdxReal			xPound;
} tdst_PosPound;
tdst_PosPound gast_VectorArrayCpt[256];

#define C_IAFlags_NoVisibility                  0x0008
#define C_IAFlags_TargetIsAlreadyComputed       0x1000
#define C_IAFlags_NoShiftUntilPosReached        0x0040
#ifndef C_IAFlags_PositionIsAlreadyComputed
#define C_IAFlags_PositionIsAlreadyComputed     0x2000
#endif

#define CONSTANT_FORCEBESTPOS                   (CAM_g_stCameraConstants.wFlags & 0x0004)
#define CONSTANT_NOBETTERPOS                    (CAM_g_stCameraConstants.wFlags & 0x0020)
#define CONSTANT_NODISTVIS                      (CAM_g_stCameraConstants.wFlags & 0x0040)

#define CAM_e_State_FailureVisibility           2

#define CAM_C_xBaseAngleComputePosNormal        (CAM_g_stCameraConstants.xBaseAngleComputePosNormal)
#define CAM_C_xBaseAngleComputePosFailure       (CAM_g_stCameraConstants.xBaseAngleComputePosFailure)

#define CAM_C_xTickTimeTestVisibility \
    (MTH_M_xMul(MTH_M_xMul(MTH_M_xLongToReal(CAM_g_stCameraConstants.xTickTestVisibility),MTH_C_1000F),GAM_g_xFrameLength))
#define CAM_C_xTickTimeFindBetterPos \
    (MTH_M_xMul(MTH_M_xMul(MTH_M_xLongToReal(CAM_g_stCameraConstants.xTickFindBetterPos),MTH_C_1000F),GAM_g_xFrameLength))
#define CAM_C_xTickTimeFailureCantSee \
    (MTH_M_xMul(MTH_M_xMul(MTH_M_xLongToReal(CAM_g_stCameraConstants.xTickFailureCantSee),MTH_C_1000F),GAM_g_xFrameLength))
#define MTH_C_1000F                             (MTH_M_xConstFloatToReal(1000))

#include "MEC/DNMRepor.h"

extern char cJustBetterPos;
void CAM_fn_vInitCameraStructure(HIE_tdstSuperObject *_hSuperObjPerso, CAM_tdstUpdateCamera *_p_stStruct);
ACP_tdxBool CAM_fn_bPersoOnFace(HIE_tdstSuperObject *_h_SuperObjPerso, MTH3D_tdstVector *_p_stNormal);
MTH_tdxReal CAM_fn_xSetAngleInInterval02Pi(MTH_tdxReal _xAngle);
char CAM_fn_cFindNewGoodIdealPos(CAM_tdstUpdateCamera *_p_stStruct, MTH3D_tdstVector *_p_stComputeVector, char _cMode);

static int CAM_fni_ComparePositions(const void *_pv_Elem1, const void *_pv_Elem2)
{

	MTH_tdxReal		xPound1, xPound2;

	xPound1 = ((tdst_PosPound *) _pv_Elem1)->xPound;
	xPound2 = ((tdst_PosPound *) _pv_Elem2)->xPound;
	return (MTH_M_bGreater(xPound2, xPound1)) ? -1 : (MTH_M_bEqual(xPound2, xPound1)) ? 0 : 1;
}

void CAM_fn_vFillVectorArray(CAM_tdstUpdateCamera *_p_stStruct, char _cMode)
{

	MTH3D_tdstVector						stAxis, stTempVector, *p_stDir;
	MTH_tdxReal								xAngle, xBaseAngle, xNormIdeal, xSaveTheta;
	unsigned long							ulCounter, ulMax, ulAngle;
	struct tdstInternalStructurCineinfo_	stTemp;
	CAM_tdstUpdateCamera					stStruct;
	int										eState;

	CAM_fn_vInitCameraStructure(_p_stStruct->hSuperObjCamera, &stStruct);
	stStruct.hCineinfoWork = &stTemp;
	memcpy(&stTemp, &gst_Ideal, sizeof(struct tdstInternalStructurCineinfo_));
	CAM_fn_vComputeReferencePoint(&stStruct);

	MTH3D_M_vSubVector(&stTempVector, &_p_stStruct->stRealWantedCameraPos, &_p_stStruct->stTargetedPersoPos);
	xNormIdeal = MTH3D_M_xNormVector(&stTempVector);

	if(!CAM_fn_bPersoOnFace(_p_stStruct->hCineinfoWork->hSuperObjectTargeted, &stAxis))
	{
		MTH3D_M_vSetVectorElements(&stAxis, MTH_C_ZERO, MTH_C_ZERO, MTH_C_ONE);
	}

	xBaseAngle = (_cMode & C_mode_FAILURE) ? CAM_C_xBaseAngleComputePosFailure : CAM_C_xBaseAngleComputePosNormal;

	ulCounter = 0;
	ulMax = MTH_M_xRealToLong(MTH_M_xDiv(MTH_C_2Pi, xBaseAngle));
	xAngle = stTemp.xAngleAlpha;

	eState = stStruct.hCineinfo->eState;
	stStruct.hCineinfo->eState = CAM_e_State_GoToOptimal;

	xSaveTheta = stTemp.xAngleTheta;
	for ( ulAngle = 0; ulAngle < ulMax; ulAngle ++ )
	{

		stTemp.xAngleAlpha = xAngle;
		stTemp.xAngleAlpha = CAM_fn_xSetAngleInInterval02Pi(stTemp.xAngleAlpha);
		stTemp.xAngleTheta = xSaveTheta;
		stStruct.hCineinfoWork->uwIAFlags |= C_IAFlags_NoShiftUntilPosReached;
		CAM_fn_vComputeMovePosWithDynTheta(&stStruct);

		p_stDir = &gast_VectorArrayCpt[ulCounter].stDir;
		MTH3D_M_vSubVector(p_stDir, &stStruct.stWantedCameraPos, &stStruct.stTargetedPersoPos);
		gast_VectorArrayCpt[ulCounter].xPound = CAM_fnx_ComputePosPound(_p_stStruct, p_stDir);
		ulCounter++;

		if(!CONSTANT_NODISTVIS)
		{
			if(_cMode & C_mode_FAILURE)
			{
				p_stDir = &gast_VectorArrayCpt[ulCounter].stDir;
				MTH3D_M_vSubVector(p_stDir, &stStruct.stWantedCameraPos, &stStruct.stTargetedPersoPos);
				MTH3D_M_vNormalizeVector(p_stDir, p_stDir);
				MTH3D_M_vMulScalarVector(p_stDir, MTH_M_xMul(xNormIdeal, MTH_C_Inv2), p_stDir);
				gast_VectorArrayCpt[ulCounter].xPound = CAM_fnx_ComputePosPound(_p_stStruct, p_stDir);
				ulCounter++;
				if(_cMode & C_mode_MORE_CORRECT)
				{
					p_stDir = &gast_VectorArrayCpt[ulCounter].stDir;
					MTH3D_M_vSubVector(p_stDir, &stStruct.stWantedCameraPos, &stStruct.stTargetedPersoPos);
					MTH3D_M_vNormalizeVector(p_stDir, p_stDir);
					MTH3D_M_vMulScalarVector(p_stDir, MTH_M_xMul(xNormIdeal, MTH_M_xConstFloatToReal(0.33f)), p_stDir);
					gast_VectorArrayCpt[ulCounter].xPound = CAM_fnx_ComputePosPound(_p_stStruct, p_stDir);
					ulCounter++;
				}
			}
		}

		stTemp.xAngleTheta = MTH_C_ZERO;
		stStruct.hCineinfoWork->uwIAFlags |= C_IAFlags_NoShiftUntilPosReached;
		CAM_fn_vComputeMovePosWithDynTheta(&stStruct);

		p_stDir = &gast_VectorArrayCpt[ulCounter].stDir;
		MTH3D_M_vSubVector(p_stDir, &stStruct.stWantedCameraPos, &stStruct.stTargetedPersoPos);
		gast_VectorArrayCpt[ulCounter].xPound = CAM_fnx_ComputePosPound(_p_stStruct, p_stDir);
		ulCounter++;

		stTemp.xAngleTheta = MTH_C_PiBy4;
		stStruct.hCineinfoWork->uwIAFlags |= C_IAFlags_NoShiftUntilPosReached;
		CAM_fn_vComputeMovePosWithDynTheta(&stStruct);

		p_stDir = &gast_VectorArrayCpt[ulCounter].stDir;
		MTH3D_M_vSubVector(p_stDir, &stStruct.stWantedCameraPos, &stStruct.stTargetedPersoPos);
		gast_VectorArrayCpt[ulCounter].xPound = CAM_fnx_ComputePosPound(_p_stStruct, p_stDir);
		ulCounter++;

		stTemp.xAngleTheta = -MTH_C_PiBy4;
		stStruct.hCineinfoWork->uwIAFlags |= C_IAFlags_NoShiftUntilPosReached;
		CAM_fn_vComputeMovePosWithDynTheta(&stStruct);

		p_stDir = &gast_VectorArrayCpt[ulCounter].stDir;
		MTH3D_M_vSubVector(p_stDir, &stStruct.stWantedCameraPos, &stStruct.stTargetedPersoPos);
		gast_VectorArrayCpt[ulCounter].xPound = CAM_fnx_ComputePosPound(_p_stStruct, p_stDir);
		ulCounter++;

		xAngle += xBaseAngle;
	}
	guw_NbVectorArrayCpt = ulCounter;

	stStruct.hCineinfo->eState = eState;
}

char CAM_fn_cFindNewGoodIdealPos
(
	CAM_tdstUpdateCamera	*_p_stStruct,
	MTH3D_tdstVector		*_p_stComputeVector,
	char					_cMode
)
{

	unsigned long		ulCounter;
	MTH3D_tdstVector	stTempVector, *p_stDir;
	MTH_tdxReal			xPoundRef;

	MTH3D_M_vNullVector(_p_stComputeVector);

	xPoundRef = MTH_C_ZERO;
	if(_cMode & C_mode_MORE_CORRECT)
	{
		if(_cMode & C_mode_FAILURE)
		{
			MTH3D_M_vSubVector(&stTempVector, &_p_stStruct->stCurrentCameraPos, &_p_stStruct->stTargetedPersoPos);
		}
		else
		{
			MTH3D_M_vSubVector(&stTempVector, &_p_stStruct->stWantedCameraPos, &_p_stStruct->stTargetedPersoPos);
		}
		xPoundRef = CAM_fnx_ComputePosPound(_p_stStruct, &stTempVector);
	}

	CAM_fn_vFillVectorArray(_p_stStruct, _cMode);

	qsort
	(
		gast_VectorArrayCpt,
		guw_NbVectorArrayCpt,
		sizeof(tdst_PosPound),
		CAM_fni_ComparePositions
	);

	for ( ulCounter = 0; ulCounter < guw_NbVectorArrayCpt; ulCounter ++ )
	{
		p_stDir = &gast_VectorArrayCpt[ulCounter].stDir;
		if
		(
			CAM_fnc_IsDirectionCorrect
			(
				_p_stStruct,
				p_stDir,
				gast_VectorArrayCpt[ulCounter].xPound,
				xPoundRef,
				_cMode
			)
			&&
			CAM_fn_cFindCorrectPosWithDir
			(
				_p_stStruct,
				p_stDir,
				_p_stComputeVector,
				_cMode
			)
		)
		{
			if(ulCounter == 0)
				return 1;
			return 2;
		}
	}

	return 0;
}

void CAM_fn_vForceFailureVisibility(CAM_tdstUpdateCamera *_p_stStruct)
{

	MTH3D_tdstVector	stComputeVector, stTempVector;
	char				cMode;

	cMode = C_mode_MUSTSEE|C_mode_FAILURE;
	if(CONSTANT_FORCEBESTPOS)
		cMode |= C_mode_MORE_CORRECT;

	if(!CAM_fn_cFindNewGoodIdealPos(_p_stStruct, &stComputeVector, cMode))
		return;

	MTH3D_M_vSubVector(&stTempVector, &stComputeVector, &_p_stStruct->stCurrentCameraPos);
	_p_stStruct->hCineinfo->eState = CAM_e_State_FailureVisibility;

	MTH3D_M_vCopyVector(&_p_stStruct->hCineinfo->stLastFailedWanted, &stComputeVector);
}

void CAM_fn_vForceCameraPos(HIE_tdstSuperObject *_hSuperObjCamera, MTH3D_tdstVector *_p_stPosition);
void CAM_fn_vForceCameraTgt(HIE_tdstSuperObject *_hSuperObjCamera, MTH3D_tdstVector *_p_stPosition, MTH3D_tdstVector *_p_stRefAxisZ);

/* 0x00486F40 */
void CAM_fn_vComputeFailureVisibility(CAM_tdstUpdateCamera *_p_stStruct)
{
	if(_p_stStruct->hCineinfo->eState == CAM_e_State_FailureVisibility)
	{

		CAM_fn_vForceCameraPos(_p_stStruct->hSuperObjCamera, &_p_stStruct->hCineinfo->stLastFailedWanted);
		CAM_fn_vForceCameraTgt(_p_stStruct->hSuperObjCamera, &_p_stStruct->stCptPos.stTarget, &_p_stStruct->stCptPos.stRefAxisZ);
		MTH3D_M_vCopyVector(&_p_stStruct->stCurrentCameraPos, &_p_stStruct->hCineinfo->stLastFailedWanted);

		CAM_fn_vMakeAsNewIdealPosition(_p_stStruct, &_p_stStruct->hCineinfo->stLastFailedWanted);
	}
}

void CAM_fn_vTestBadDistance(CAM_tdstUpdateCamera *_p_stStruct)
{

	MTH3D_tdstVector	stTempVector, stComputeVector;
	char				cMode;

	if
	(
		(_p_stStruct->hCineinfo->eState != CAM_e_State_GoToOptimal)
	&&	(_p_stStruct->hCineinfo->eState != CAM_e_State_GoToComputedOptimal)
	)
		return;

	if
	(
		(_p_stStruct->hCineinfoWork->uwIAFlags & C_IAFlags_TargetIsAlreadyComputed)
	||	(_p_stStruct->hCineinfoWork->uwIAFlags & C_IAFlags_PositionIsAlreadyComputed)
	||	(_p_stStruct->hCineinfo->ucVolIAFlags & C_VolIAFlags_InitJustCopiedInCurrent)
	)
		return;

	MTH3D_M_vSubVector(&stTempVector, &_p_stStruct->stCurrentCameraPos, &_p_stStruct->stTargetedPersoRealPos);

	if
	(
		(MTH_M_bGreater(MTH3D_M_xNormVector(&stTempVector), CAM_C_xMinDistPerso))
	||	(
			(!(DNM_M_bReportCollision(_p_stStruct->p_stReport)))
		&&	(!(_p_stStruct->hCineinfo->ucVolIAFlags & C_VolIAFlags_CameraNotMove))
		)
	)
		return;

	if(!CAM_fn_cIsWantedPosIncorrect(_p_stStruct, &_p_stStruct->stWantedCameraPos, C_mode_MUSTGO|C_mode_MUSTSEE))
		return;

	_p_stStruct->hCineinfo->xCounter1 = MTH_C_ZERO;

	cMode = C_mode_MUSTGO|C_mode_MUSTSEE;
	if(_p_stStruct->hCineinfoWork->uwIAFlags & C_IAFlags_NoVisibility)
		cMode &= ~C_mode_MUSTSEE;

	if(!CAM_fn_cFindNewGoodIdealPos(_p_stStruct, &stComputeVector, cMode))
		CAM_fn_vForceFailureVisibility(_p_stStruct);
	else
	{
		CAM_fn_vMakeAsNewIdealPosition(_p_stStruct, &stComputeVector);
		_p_stStruct->hCineinfo->ucPerIAFlags |= C_PerIAFlags_ConstraintMoveVisFailure;
	}
}

void CAM_fn_vTestBadVisibility(CAM_tdstUpdateCamera	*_p_stStruct)
{

	MTH3D_tdstVector	stComputeVector, stTempVector;

	if
	(
		(_p_stStruct->hCineinfo->eState != CAM_e_State_GoToOptimal)
	&&	(_p_stStruct->hCineinfo->eState != CAM_e_State_GoToComputedOptimal)
	)
		return;

	if
	(
		(_p_stStruct->hCineinfoWork->uwIAFlags & C_IAFlags_NoVisibility)

	||	(_p_stStruct->hCineinfoWork->uwIAFlags & C_IAFlags_PositionIsAlreadyComputed)
	||	(_p_stStruct->hCineinfo->ucVolIAFlags & C_VolIAFlags_InitJustCopiedInCurrent)
	)
		return;

	_p_stStruct->hCineinfo->xCounter2 = MTH_M_xAdd(_p_stStruct->hCineinfo->xCounter2,MTH_M_xLongToReal(GAM_g_ulUsefulDeltaTime));

	if(MTH_M_bLess(_p_stStruct->hCineinfo->xCounter2 , CAM_C_xTickTimeTestVisibility))
		return;

	_p_stStruct->hCineinfo->xCounter2 = MTH_C_ZERO;

	if(MTH_M_bIsNull(_p_stStruct->hCineinfo->xCounter3))
	{

		if(MTH_M_bGreater(_p_stStruct->hCineinfo->xCounter4 , MTH_M_xMul(MTH_M_xLongToReal(10000),GAM_g_xFrameLength)))

				cCanTestStatic = 0;
			else
			{
				MTH3D_tdstVector stVectorNull;

 				MTH3D_M_vNullVector(&stVectorNull);

				MTH3D_M_vCopyVector(&stComputeVector, DNM_M_p_stReportGetLastMove(_p_stStruct->p_stReport));
				MTH3D_M_vSubVector(&stTempVector, &_p_stStruct->stTargetedPersoRealPos, &_p_stStruct->stCurrentCameraPos);

				if
				(
					(!CAM_fn_bEqualVectorWithEpsilon(&stComputeVector, &stVectorNull, CAM_C_xEpsilonLastMove))
				&&	(!CAM_fn_bEqualVectorWithEpsilon(&stTempVector, &stVectorNull, CAM_C_xEpsilonLastMove))
				)
				{
					MTH3D_M_vNormalizeVector(&stComputeVector, &stComputeVector);
					MTH3D_M_vNormalizeVector(&stTempVector, &stTempVector);
					if(MTH_M_bGreater(MTH_M_xAbs(MTH3D_M_xDotProductVector(&stTempVector, &stComputeVector)), MTH_M_xConstFloatToReal(0.9f)))
						cCanTestStatic = 0;
				}
			}

	}

	if(!CAM_fn_cCameraCantSee(_p_stStruct, &_p_stStruct->stTargetedPersoRealPos, &_p_stStruct->stCurrentCameraPos))
	{

		_p_stStruct->hCineinfo->xCounter3 = MTH_C_ZERO;
		return;
	}

	_p_stStruct->hCineinfo->xCounter3 = MTH_M_xAdd(_p_stStruct->hCineinfo->xCounter3,MTH_M_xLongToReal(GAM_g_ulUsefulDeltaTime));

	if(MTH_M_bGreater(_p_stStruct->hCineinfo->xCounter3 , CAM_C_xTickTimeFailureCantSee))
	{

		_p_stStruct->hCineinfo->xCounter3 = MTH_C_ZERO;
		CAM_fn_vForceFailureVisibility(_p_stStruct);
		return;
	}

	if(!CAM_fn_cIsWantedPosIncorrect(_p_stStruct, &_p_stStruct->stWantedCameraPos, C_mode_MUSTGO|C_mode_MUSTSEE))
		return;

	_p_stStruct->hCineinfo->xCounter1 = MTH_C_ZERO;
	_p_stStruct->hCineinfo->xCounter3 = MTH_C_ZERO;

	if(!CAM_fn_cFindNewGoodIdealPos(_p_stStruct, &stComputeVector, C_mode_MUSTGO|C_mode_MUSTSEE))
		CAM_fn_vForceFailureVisibility(_p_stStruct);
	else
	{
		CAM_fn_vMakeAsNewIdealPosition(_p_stStruct, &stComputeVector);
		_p_stStruct->hCineinfo->ucPerIAFlags |= C_PerIAFlags_ConstraintMoveVisFailure;
	}
}

void CAM_fn_vTestOppositeCollision(CAM_tdstUpdateCamera *_p_stStruct)
{

	MTH3D_tdstVector	stCameraMove;
	char				cMode;

	if
	(
		(_p_stStruct->hCineinfo->eState != CAM_e_State_GoToOptimal)
	&&	(_p_stStruct->hCineinfo->eState != CAM_e_State_GoToComputedOptimal)
	)
		return;

	if
	(
		(_p_stStruct->hCineinfoWork->uwIAFlags & C_IAFlags_TargetIsAlreadyComputed)
	||	(_p_stStruct->hCineinfoWork->uwIAFlags & C_IAFlags_PositionIsAlreadyComputed)
	||	(_p_stStruct->hCineinfo->ucVolIAFlags & C_VolIAFlags_InitJustCopiedInCurrent)
	)
		return;

	if (!(DNM_M_bReportCollision(_p_stStruct->p_stReport)))
		return;

	if(!(DNM_M_bReportIsWall(_p_stStruct->p_stReport)))
		return;

	_p_stStruct->hCineinfo->xCounter1 = MTH_C_ZERO;

	cMode = C_mode_MUSTGO|C_mode_MUSTSEE;
	if(_p_stStruct->hCineinfoWork->uwIAFlags & C_IAFlags_NoVisibility)
		cMode &= ~C_mode_MUSTSEE;

	if(!CAM_fn_cFindNewGoodIdealPos(_p_stStruct, &stCameraMove, cMode))
		CAM_fn_vForceFailureVisibility(_p_stStruct);
	else
		CAM_fn_vMakeAsNewIdealPosition(_p_stStruct, &stCameraMove);
}

void CAM_fn_vCheckForABetterPos(CAM_tdstUpdateCamera *_p_stStruct)
{

	MTH3D_tdstVector	stComputeVector;
	char				cRes;

	_p_stStruct->hCineinfo->xCounter1 = MTH_M_xAdd(_p_stStruct->hCineinfo->xCounter1,MTH_M_xLongToReal(GAM_g_ulUsefulDeltaTime));

	if(MTH_M_bGreaterEqual(_p_stStruct->hCineinfo->xCounter1 , CAM_C_xTickTimeFindBetterPos) && (!(CONSTANT_NOBETTERPOS)) && _p_stStruct->hCineinfo->bCanDoBestPos)
	{

		_p_stStruct->hCineinfo->xCounter1 = MTH_C_ZERO;

		cRes = CAM_fn_cFindNewGoodIdealPos(_p_stStruct, &stComputeVector, C_mode_MUSTGO|C_mode_MUSTSEE|C_mode_MORE_CORRECT);
		if(cRes)
		{
			if(cRes == 1)
			{
				CAM_fn_vRestoreNormalState(_p_stStruct);
			}
			else
			{
				CAM_fn_vMakeAsNewIdealPosition(_p_stStruct, &stComputeVector);
				if(_p_stStruct->hCineinfo->ucPerIAFlags & C_PerIAFlags_ConstraintMoveVisFailure)
					cJustBetterPos = 1;
				else
					cJustBetterPos = 0;
				_p_stStruct->hCineinfo->ucPerIAFlags &= ~C_PerIAFlags_ConstraintMoveVisFailure;
			}
			_p_stStruct->hCineinfo->bCanDoBestPos = FALSE;
		}
	}
}
