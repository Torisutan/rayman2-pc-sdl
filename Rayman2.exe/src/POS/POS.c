#include "POS/POS_types.h"
#include "MTH/MTH_flt.h"
#include "MTH/MTH_xNeg.h"

extern POS_tdstCompletePosition *POS_g_p_stIdentityMatrix;

#define CAM_C_xAngleEpsilon  ((MTH_tdxReal)1.0e-4f)

static void POS_vSetBasisIVector(MTH3D_tdstVector *p_stVector)
{
    p_stVector->xX = 1.0f;
    p_stVector->xY = 0.0f;
    p_stVector->xZ = 0.0f;
}

static void POS_vSetBasisJVector(MTH3D_tdstVector *p_stVector)
{
    p_stVector->xX = 0.0f;
    p_stVector->xY = 1.0f;
    p_stVector->xZ = 0.0f;
}

static void POS_vSetBasisKVector(MTH3D_tdstVector *p_stVector)
{
    p_stVector->xX = 0.0f;
    p_stVector->xY = 0.0f;
    p_stVector->xZ = 1.0f;
}

static MTH_tdxReal POS_xDotProduct(const MTH3D_tdstVector *p_stA, const MTH3D_tdstVector *p_stB)
{
    return (p_stA->xX * p_stB->xX) + (p_stA->xY * p_stB->xY) + (p_stA->xZ * p_stB->xZ);
}

static void POS_vBuildScaleMatrix(MTH3D_tdstMatrix *p_stDest,
                                  const MTH3D_tdstMatrix *p_stRotation,
                                  const MTH3D_tdstMatrix *p_stTransform)
{
    p_stDest->stCol_0.xX = POS_xDotProduct(&p_stRotation->stCol_0, &p_stTransform->stCol_0);
    p_stDest->stCol_0.xY = POS_xDotProduct(&p_stRotation->stCol_1, &p_stTransform->stCol_0);
    p_stDest->stCol_0.xZ = POS_xDotProduct(&p_stRotation->stCol_2, &p_stTransform->stCol_0);
    p_stDest->stCol_1.xX = POS_xDotProduct(&p_stRotation->stCol_0, &p_stTransform->stCol_1);
    p_stDest->stCol_1.xY = POS_xDotProduct(&p_stRotation->stCol_1, &p_stTransform->stCol_1);
    p_stDest->stCol_1.xZ = POS_xDotProduct(&p_stRotation->stCol_2, &p_stTransform->stCol_1);
    p_stDest->stCol_2.xX = POS_xDotProduct(&p_stRotation->stCol_0, &p_stTransform->stCol_2);
    p_stDest->stCol_2.xY = POS_xDotProduct(&p_stRotation->stCol_1, &p_stTransform->stCol_2);
    p_stDest->stCol_2.xZ = POS_xDotProduct(&p_stRotation->stCol_2, &p_stTransform->stCol_2);
}

void POS_fn_vSetIdentityMatrix(POS_tdstCompletePosition *p_stPos)
{

    *p_stPos = *POS_g_p_stIdentityMatrix;
}

void POS_fn_vMulMatrixVertex(MTH3D_tdstVector *_p_stDest,
                             POS_tdstCompletePosition *_hMatrix,
                             MTH3D_tdstVector *_p_stSource)
{
    switch (_hMatrix->ulType) {
    case POS_C_ulIdentityMatrixFlag:
        if (_p_stDest != _p_stSource) {
            *_p_stDest = *_p_stSource;
        }
        break;

    case POS_C_ulTranslationMatrixFlag:
        MTH3D_M_vAddVector(_p_stDest, &_hMatrix->stTranslationVector, _p_stSource);
        break;

    case POS_C_ulRotationMatrixFlag:

        MTH3D_M_vMulMatrixVector(_p_stDest, &_hMatrix->stRotationMatrix, _p_stSource);
        MTH3D_M_vAddVector(_p_stDest, &_hMatrix->stTranslationVector, _p_stDest);
        break;

    case POS_C_ulCompleteMatrixFlag:
        MTH3D_M_vMulMatrixVector(_p_stDest, &_hMatrix->stTransformMatrix, _p_stSource);
        MTH3D_M_vAddVector(_p_stDest, &_hMatrix->stTranslationVector, _p_stDest);
        break;

    default:
        *(volatile long *)0 = 5;
        break;
    }
}

void POS_fn_vMulMatrixVector(MTH3D_tdstVector *_p_stDest,
                             POS_tdstCompletePosition *_hMatrix,
                             MTH3D_tdstVector *_p_stSource)
{
    switch (_hMatrix->ulType) {
    case POS_C_ulIdentityMatrixFlag:
    case POS_C_ulTranslationMatrixFlag:
        if (_p_stDest != _p_stSource) {
            *_p_stDest = *_p_stSource;
        }
        break;

    case POS_C_ulRotationMatrixFlag:
        MTH3D_M_vMulMatrixVector(_p_stDest, &_hMatrix->stRotationMatrix, _p_stSource);
        break;

    case POS_C_ulCompleteMatrixFlag:
        MTH3D_M_vMulMatrixVector(_p_stDest, &_hMatrix->stTransformMatrix, _p_stSource);
        break;

    default:
        *(volatile long *)0 = 5;
        break;
    }
}

void POS_fn_vRotateVector(MTH3D_tdstVector *_p_stDest,
                          POS_tdstCompletePosition *_hMatrix,
                          MTH3D_tdstVector *_p_stSource)
{
    switch (_hMatrix->ulType) {
    case POS_C_ulIdentityMatrixFlag:
    case POS_C_ulTranslationMatrixFlag:
        if (_p_stDest != _p_stSource) {
            *_p_stDest = *_p_stSource;
        }
        break;

    case POS_C_ulRotationMatrixFlag:
    case POS_C_ulCompleteMatrixFlag:
        MTH3D_M_vMulMatrixVector(_p_stDest, &_hMatrix->stRotationMatrix, _p_stSource);
        break;

    default:
        *(volatile long *)0 = 5;
        break;
    }
}

void POS_fn_vSetRotationMatrix(POS_tdstCompletePosition *_hMatrix, MTH3D_tdstVector *_p_stI, MTH3D_tdstVector *_p_stJ, MTH3D_tdstVector *_p_stK);

void fn_vTurnMatrixX(POS_tdstCompletePosition *_p_stMatrix, MTH_tdxReal _xAngle)
{
    POS_tdstCompletePosition TmpMatrix;
    MTH3D_tdstVector I, J, K;
    MTH_tdxReal xCos, xSin;

    POS_fn_vSetIdentityMatrix(&TmpMatrix);
    xCos = MTH_M_xCos(_xAngle);
    xSin = MTH_M_xSin(_xAngle);
    if (MTH_M_bIsNullWithEpsilon(xCos, CAM_C_xAngleEpsilon))
        xCos = MTH_C_ZERO;
    if (MTH_M_bIsNullWithEpsilon(xSin, CAM_C_xAngleEpsilon))
        xSin = MTH_C_ZERO;
    MTH3D_M_vSetBaseIVector(&I);
    MTH3D_M_vSetVectorElements(&J, MTH_C_ZERO, xCos, xSin);
    MTH3D_M_vSetVectorElements(&K, MTH_C_ZERO, MTH_M_xNeg(xSin), xCos);
    POS_fn_vSetRotationMatrix(&TmpMatrix, &I, &J, &K);
    POS_fn_vMulMatrixMatrix(_p_stMatrix, &TmpMatrix, _p_stMatrix);
}

void fn_vTurnMatrixY(POS_tdstCompletePosition *_p_stMatrix, MTH_tdxReal _xAngle)
{
    POS_tdstCompletePosition TmpMatrix;
    MTH3D_tdstVector I, J, K;
    MTH_tdxReal xCos, xSin;

    POS_fn_vSetIdentityMatrix(&TmpMatrix);
    xCos = MTH_M_xCos(_xAngle);
    xSin = MTH_M_xSin(_xAngle);
    if (MTH_M_bIsNullWithEpsilon(xCos, CAM_C_xAngleEpsilon))
        xCos = MTH_C_ZERO;
    if (MTH_M_bIsNullWithEpsilon(xSin, CAM_C_xAngleEpsilon))
        xSin = MTH_C_ZERO;
    MTH3D_M_vSetVectorElements(&I, xCos, MTH_C_ZERO, MTH_M_xNeg(xSin));
    MTH3D_M_vSetBaseJVector(&J);
    MTH3D_M_vSetVectorElements(&K, xSin, MTH_C_ZERO, xCos);
    POS_fn_vSetRotationMatrix(&TmpMatrix, &I, &J, &K);
    POS_fn_vMulMatrixMatrix(_p_stMatrix, &TmpMatrix, _p_stMatrix);
}

void fn_vTurnMatrixZ(POS_tdstCompletePosition *_p_stMatrix, MTH_tdxReal _xAngle)
{
    POS_tdstCompletePosition TmpMatrix;
    MTH3D_tdstVector I, J, K;
    MTH_tdxReal xCos, xSin;

    POS_fn_vSetIdentityMatrix(&TmpMatrix);
    xCos = MTH_M_xCos(_xAngle);
    xSin = MTH_M_xSin(_xAngle);
    if (MTH_M_bIsNullWithEpsilon(xCos, CAM_C_xAngleEpsilon))
        xCos = MTH_C_ZERO;
    if (MTH_M_bIsNullWithEpsilon(xSin, CAM_C_xAngleEpsilon))
        xSin = MTH_C_ZERO;
    MTH3D_M_vSetVectorElements(&I, xCos, xSin, MTH_C_ZERO);
    MTH3D_M_vSetVectorElements(&J, MTH_M_xNeg(xSin), xCos, MTH_C_ZERO);
    MTH3D_M_vSetBaseKVector(&K);
    POS_fn_vSetRotationMatrix(&TmpMatrix, &I, &J, &K);
    POS_fn_vMulMatrixMatrix(_p_stMatrix, &TmpMatrix, _p_stMatrix);
}

/* 0x00445C70 */
void POS_fn_vRotateZ(POS_tdstCompletePosition *hMatrix, MTH_tdxReal xZAngle)
{
    MTH3D_tdstVector stI;
    MTH3D_tdstVector stJ;
    MTH3D_tdstVector stK;
    MTH_tdxReal xSin = MTH_M_xSin(xZAngle);
    MTH_tdxReal xCos = MTH_M_xCos(xZAngle);

    MTH3D_M_vSetVectorElements(&stI, xCos, xSin, MTH_C_ZERO);
    MTH3D_M_vSetVectorElements(&stJ, MTH_M_xNeg(xSin), xCos, MTH_C_ZERO);
    MTH3D_M_vSetVectorElements(&stK, MTH_C_ZERO, MTH_C_ZERO, MTH_C_ONE);

    POS_fn_vSetRotationMatrix(hMatrix, &stI, &stJ, &stK);
}

void POS_fn_vSetRotationMatrix(POS_tdstCompletePosition *_hMatrix,
                               MTH3D_tdstVector *_p_stI,
                               MTH3D_tdstVector *_p_stJ,
                               MTH3D_tdstVector *_p_stK)
{

	switch(_hMatrix->ulType)
	{
	case POS_C_ulIdentityMatrixFlag:
	case POS_C_ulTranslationMatrixFlag:
	case POS_C_ulRotationMatrixFlag:
		MTH3D_M_vSetVectorsInMatrix( &_hMatrix->stRotationMatrix, _p_stI, _p_stJ, _p_stK );
		MTH3D_M_vCopyMatrix(&_hMatrix->stTransformMatrix, &_hMatrix->stRotationMatrix);
		_hMatrix->ulType = POS_C_ulRotationMatrixFlag;
		break;

	default:
	    {
			MTH3D_tdstMatrix stScaleMatrix;

			POS_fn_vGetScaleMatrix(_hMatrix, &stScaleMatrix.stCol_0, &stScaleMatrix.stCol_1, &stScaleMatrix.stCol_2);

			MTH3D_M_vSetVectorsInMatrix( &_hMatrix->stRotationMatrix, _p_stI, _p_stJ, _p_stK );

			MTH3D_M_vMulMatrixMatrixWithoutBuffer(&_hMatrix->stTransformMatrix, &_hMatrix->stRotationMatrix, &stScaleMatrix);
	    }
		break;
	}
}

void POS_fn_vGetRotationMatrixCol2(POS_tdstCompletePosition *_hMatrix, MTH3D_tdstVector *_p_stCol2)
{
    MTH3D_M_vGetColumnInMatrix(_p_stCol2, &_hMatrix->stRotationMatrix, 2);
}

/* 0x00445F20 */
void POS_fn_vGetRotationMatrix(POS_tdstCompletePosition *_hMatrix,
                               MTH3D_tdstVector *_p_stI,
                               MTH3D_tdstVector *_p_stJ,
                               MTH3D_tdstVector *_p_stK)
{
    /* 0x00445F20 */
    _p_stI->xX = _hMatrix->stRotationMatrix.stCol_0.xX;
    _p_stI->xY = _hMatrix->stRotationMatrix.stCol_0.xY;
    _p_stI->xZ = _hMatrix->stRotationMatrix.stCol_0.xZ;
    _p_stJ->xX = _hMatrix->stRotationMatrix.stCol_1.xX;
    _p_stJ->xY = _hMatrix->stRotationMatrix.stCol_1.xY;
    _p_stJ->xZ = _hMatrix->stRotationMatrix.stCol_1.xZ;
    _p_stK->xX = _hMatrix->stRotationMatrix.stCol_2.xX;
    _p_stK->xY = _hMatrix->stRotationMatrix.stCol_2.xY;
    _p_stK->xZ = _hMatrix->stRotationMatrix.stCol_2.xZ;
}

void POS_fn_vGetRotationVector(POS_tdstCompletePosition *_hMatrix,
                               MTH3D_tdstVector **_p_p_stI,
                               MTH3D_tdstVector **_p_p_stJ,
                               MTH3D_tdstVector **_p_p_stK)
{
    *_p_p_stI = &_hMatrix->stRotationMatrix.stCol_0;
    *_p_p_stJ = &_hMatrix->stRotationMatrix.stCol_1;
    *_p_p_stK = &_hMatrix->stRotationMatrix.stCol_2;
}

void POS_fn_vSetScaleMatrix(POS_tdstCompletePosition *_hMatrix,
                            MTH3D_tdstVector *_p_stI,
                            MTH3D_tdstVector *_p_stJ,
                            MTH3D_tdstVector *_p_stK)
{

	if (MTH_M_bEqualWithEpsilon(_p_stI->xX, MTH_C_ONE, 0.01) &&
		MTH_M_bEqualWithEpsilon(_p_stJ->xY, MTH_C_ONE, 0.01) &&
		MTH_M_bEqualWithEpsilon(_p_stK->xZ, MTH_C_ONE, 0.01))
	{

		MTH3D_M_vCopyMatrix(&_hMatrix->stTransformMatrix,&_hMatrix->stRotationMatrix);

    	_hMatrix->ulType = POS_C_ulRotationMatrixFlag;
		return;
	}

	switch ( _hMatrix->ulType )
	{
	case POS_C_ulIdentityMatrixFlag:
	case POS_C_ulTranslationMatrixFlag:
		MTH3D_M_vSetVectorsInMatrix(&_hMatrix->stTransformMatrix, _p_stI, _p_stJ, _p_stK);
		_hMatrix->ulType = POS_C_ulCompleteMatrixFlag;
		break;

	default:
	    {
	    	MTH3D_tdstMatrix stScaleMatrix;
			MTH3D_M_vSetVectorsInMatrix( &stScaleMatrix, _p_stI, _p_stJ, _p_stK );
			MTH3D_M_vMulMatrixMatrixWithoutBuffer( &_hMatrix->stTransformMatrix, &_hMatrix->stRotationMatrix, &stScaleMatrix);
			_hMatrix->ulType = POS_C_ulCompleteMatrixFlag;
	    }
		break;
	}
}

void POS_fn_vGetScaleMatrix(POS_tdstCompletePosition *_hMatrix,
                            MTH3D_tdstVector *_p_stI,
                            MTH3D_tdstVector *_p_stJ,
                            MTH3D_tdstVector *_p_stK)
{
	MTH3D_M_vSetBaseIVector(_p_stI);
	MTH3D_M_vSetBaseJVector(_p_stJ);
	MTH3D_M_vSetBaseKVector(_p_stK);

	if ( _hMatrix->ulType > POS_C_ulRotationMatrixFlag )
	{
	  	MTH3D_tdstMatrix stScaleMatrix;
		MTH3D_M_vMulTranspMatrixMatrixWithoutBuffer(&stScaleMatrix,
													&_hMatrix->stRotationMatrix,
													&_hMatrix->stTransformMatrix);
		MTH3D_M_vGetVectorsInMatrix(_p_stI,_p_stJ,_p_stK,&stScaleMatrix);
	}
}

void POS_fn_vSetZoomMatrix(POS_tdstCompletePosition *_hMatrix, MTH_tdxReal _xZoom)
{
    MTH_tdxReal xA = _hMatrix->stRotationMatrix.stCol_0.xX * _xZoom;
    MTH_tdxReal xB = _hMatrix->stRotationMatrix.stCol_0.xY * _xZoom;
    MTH_tdxReal xC = _hMatrix->stRotationMatrix.stCol_0.xZ * _xZoom;
    MTH_tdxReal xD = _hMatrix->stRotationMatrix.stCol_1.xX * _xZoom;
    MTH_tdxReal xE = _hMatrix->stRotationMatrix.stCol_1.xY * _xZoom;
    MTH_tdxReal xF = _hMatrix->stRotationMatrix.stCol_1.xZ * _xZoom;
    MTH_tdxReal xG = _hMatrix->stRotationMatrix.stCol_2.xX * _xZoom;
    MTH_tdxReal xH = _hMatrix->stRotationMatrix.stCol_2.xY * _xZoom;
    MTH_tdxReal xI = _hMatrix->stRotationMatrix.stCol_2.xZ * _xZoom;
    _hMatrix->stTransformMatrix.stCol_0.xX = xA;
    _hMatrix->stTransformMatrix.stCol_0.xY = xB;
    _hMatrix->stTransformMatrix.stCol_0.xZ = xC;
    _hMatrix->stTransformMatrix.stCol_1.xX = xD;
    _hMatrix->stTransformMatrix.stCol_1.xY = xE;
    _hMatrix->stTransformMatrix.stCol_1.xZ = xF;
    _hMatrix->stTransformMatrix.stCol_2.xX = xG;
    _hMatrix->stTransformMatrix.stCol_2.xY = xH;
    _hMatrix->stTransformMatrix.stCol_2.xZ = xI;
    _hMatrix->ulType = POS_C_ulCompleteMatrixFlag;
}

void POS_fn_vGetTransformMatrix(POS_tdstCompletePosition *_hMatrix,
                                MTH3D_tdstVector *_p_stI,
                                MTH3D_tdstVector *_p_stJ,
                                MTH3D_tdstVector *_p_stK)
{
    /* 0x004464F0 */
    _p_stI->xX = _hMatrix->stTransformMatrix.stCol_0.xX;
    _p_stI->xY = _hMatrix->stTransformMatrix.stCol_0.xY;
    _p_stI->xZ = _hMatrix->stTransformMatrix.stCol_0.xZ;
    _p_stJ->xX = _hMatrix->stTransformMatrix.stCol_1.xX;
    _p_stJ->xY = _hMatrix->stTransformMatrix.stCol_1.xY;
    _p_stJ->xZ = _hMatrix->stTransformMatrix.stCol_1.xZ;
    _p_stK->xX = _hMatrix->stTransformMatrix.stCol_2.xX;
    _p_stK->xY = _hMatrix->stTransformMatrix.stCol_2.xY;
    _p_stK->xZ = _hMatrix->stTransformMatrix.stCol_2.xZ;
}

static __inline void fn_vSetMatrixAsIdentity(MTH3D_tdstMatrix *_hMatrix)
{
	*_hMatrix = POS_g_p_stIdentityMatrix->stRotationMatrix;
}

void POS_fn_vInvertIsoMatrix(POS_tdstCompletePosition *_hDest, POS_tdstCompletePosition *_hSource)
{
	switch ( _hSource->ulType )
	{
	case POS_C_ulIdentityMatrixFlag:
		if ( _hDest != _hSource )
	    {
			POS_fn_vSetIdentityMatrix( _hDest );
    	}
		break;

	case POS_C_ulTranslationMatrixFlag:
		MTH3D_M_vNegVector(&_hDest->stTranslationVector, &_hSource->stTranslationVector);
		if ( _hDest != _hSource )
		{
			fn_vSetMatrixAsIdentity(&_hDest->stTransformMatrix);
			fn_vSetMatrixAsIdentity(&_hDest->stRotationMatrix);

			_hDest->ulType = POS_C_ulTranslationMatrixFlag;
		}
		break;

	default:
	    {
			MTH3D_tdstVector stVertex;

			if ( _hDest != _hSource )
			{
				MTH3D_M_vTranspMatrixWithoutBuffer(&_hDest->stRotationMatrix, &_hSource->stRotationMatrix);
			}
			else
			{
				MTH3D_M_vTranspMatrix(&_hDest->stRotationMatrix, &_hSource->stRotationMatrix);
			}

			MTH3D_M_vCopyMatrix(&_hDest->stTransformMatrix, &_hDest->stRotationMatrix);

			MTH3D_M_vNegVector(&stVertex, &_hSource->stTranslationVector);

  			MTH3D_M_vMulMatrixVector(&_hDest->stTranslationVector, &_hDest->stTransformMatrix, &stVertex);

	 		_hDest->ulType = POS_C_ulRotationMatrixFlag;
	    }
		break;
	}
}

void POS_fn_vInvertMatrix(POS_tdstCompletePosition *_hDest, POS_tdstCompletePosition *_hSource)
{
	if(_hSource->ulType <= POS_C_ulRotationMatrixFlag )
	{
		POS_fn_vInvertIsoMatrix(_hDest, _hSource);
	}
	else
	{
	   	MTH3D_tdstVector stVertex;

		if ( _hDest != _hSource )
		{
			MTH3D_M_vTranspMatrixWithoutBuffer(&_hDest->stRotationMatrix, &_hSource->stRotationMatrix);
		}
		else
		{
			MTH3D_M_vTranspMatrix(&_hDest->stRotationMatrix, &_hSource->stRotationMatrix);
		}

		MTH3D_M_vInverMatrix(&_hDest->stTransformMatrix, &_hSource->stTransformMatrix);

		MTH3D_M_vNegVector(&stVertex, &_hSource->stTranslationVector);
		MTH3D_M_vMulMatrixVector(&_hDest->stTranslationVector, &_hDest->stTransformMatrix, &stVertex);

		_hDest->ulType = POS_C_ulCompleteMatrixFlag;
	}
}

void POS_fn_vNormalizeMatrix(POS_tdstCompletePosition *_hMatrix)
{
	if ( _hMatrix->ulType >= POS_C_ulRotationMatrixFlag )
	{
		MTH3D_tdstVector *p_stI , *p_stJ , *p_stK ;

		POS_fn_vGetRotationVector( _hMatrix , &p_stI , &p_stJ , &p_stK ) ;

		MTH3D_M_vCrossProductVectorWithoutBuffer( p_stJ , p_stK , p_stI ) ;
		MTH3D_M_vNormalizeVector( p_stI,p_stI ) ;
		MTH3D_M_vNormalizeVector( p_stJ,p_stJ ) ;
		MTH3D_M_vCrossProductVectorWithoutBuffer( p_stK , p_stI , p_stJ ) ;

		MTH3D_M_vCopyMatrix( &_hMatrix->stTransformMatrix, &_hMatrix->stRotationMatrix);
		_hMatrix->ulType = POS_C_ulRotationMatrixFlag;
	}
}

void POS_fn_vMulMatrixMatrix(POS_tdstCompletePosition *_hDest,
                             POS_tdstCompletePosition *_hA,
                             POS_tdstCompletePosition *_hB)
{

	POS_tdstCompletePosition stTempMatrix , *p_stResult;
	unsigned long ulResultType;

	if (_hA->ulType == POS_C_ulIdentityMatrixFlag )
	{
		if ( _hB != _hDest )
			POS_fn_vCopyMatrix(_hDest, _hB);
		return;
	}

	if ( _hB->ulType == POS_C_ulIdentityMatrixFlag )
	{
		if ( _hA != _hDest )
			POS_fn_vCopyMatrix(_hDest, _hA);
		return;
	}

	if ( (_hDest == _hA) || (_hDest == _hB) )
		p_stResult = &stTempMatrix;
	else
		p_stResult = _hDest;

	ulResultType = (_hA->ulType > _hB->ulType) ? _hA->ulType : _hB->ulType;

	MTH3D_M_vTransformVectorWithoutBuffer(&p_stResult->stTranslationVector, &_hA->stTransformMatrix, &_hB->stTranslationVector, &_hA->stTranslationVector);

	MTH3D_M_vMulMatrixMatrixWithoutBuffer(&p_stResult->stRotationMatrix, &_hA->stRotationMatrix, &_hB->stRotationMatrix);

	if ( ulResultType <= POS_C_ulRotationMatrixFlag )
	{
		MTH3D_M_vCopyMatrix( &p_stResult->stTransformMatrix, &p_stResult->stRotationMatrix );
	}
	else
	{
		MTH3D_M_vMulMatrixMatrixWithoutBuffer(&p_stResult->stTransformMatrix, &_hA->stTransformMatrix, &_hB->stTransformMatrix );
	}

	if( _hDest != p_stResult )
		POS_fn_vCopyMatrix(_hDest, p_stResult);

	_hDest->ulType = ulResultType;
}

void POS_fn_vRotatePositionAroundAxis(POS_tdxHandleToPosition _hPos,
                                      MTH3D_tdstVector *_p_stAxisVector,
                                      MTH_tdxReal _xAngle)
{
    MTH3D_tdstMatrix stMatrix;
    MTH3D_tdstVector stAxis;
    MTH_tdxReal xSinAngle;

    _xAngle   = MTH_M_xMul(_xAngle, MTH_C_Inv2);
    xSinAngle = MTH_M_xSin(_xAngle);
    _xAngle   = MTH_M_xCos(_xAngle);

    MTH3D_M_vMulScalarVector(&stAxis, xSinAngle, _p_stAxisVector);

    POS_fn_vQuatToMat(&stMatrix, stAxis.xX, stAxis.xY, stAxis.xZ, _xAngle);

    if (_hPos->ulType <= POS_C_ulTranslationMatrixFlag)
    {   MTH3D_M_vCopyMatrix(&_hPos->stRotationMatrix, &stMatrix);   }
    else
        MTH3D_M_vMulMatrixMatrix(&_hPos->stRotationMatrix, &_hPos->stRotationMatrix, &stMatrix);

    if (_hPos->ulType <= POS_C_ulRotationMatrixFlag)
    {   MTH3D_M_vCopyMatrix(&_hPos->stTransformMatrix, &_hPos->stRotationMatrix);   }
    else
        MTH3D_M_vMulMatrixMatrix(&_hPos->stTransformMatrix, &_hPos->stTransformMatrix, &stMatrix);

    MTH3D_M_vMulMatrixVector(&_hPos->stTranslationVector, &stMatrix, &_hPos->stTranslationVector);

    _hPos->ulType = (_hPos->ulType > POS_C_ulRotationMatrixFlag) ? _hPos->ulType : POS_C_ulRotationMatrixFlag;
}

void POS_fn_vRotatePositionAroundAxisNoTranslation(POS_tdxHandleToPosition _hPos,
                                                    MTH3D_tdstVector *_p_stAxisVector,
                                                    MTH_tdxReal _xAngle)
{
    MTH3D_tdstMatrix stMatrix;
    MTH3D_tdstVector stAxis;
    MTH_tdxReal xSinAngle;

    _xAngle   = MTH_M_xMul(_xAngle, MTH_C_Inv2);
    xSinAngle = MTH_M_xSin(_xAngle);
    _xAngle   = MTH_M_xCos(_xAngle);

    MTH3D_M_vMulScalarVector(&stAxis, xSinAngle, _p_stAxisVector);

    POS_fn_vQuatToMat(&stMatrix, stAxis.xX, stAxis.xY, stAxis.xZ, _xAngle);

    if (_hPos->ulType <= POS_C_ulTranslationMatrixFlag)
    {   MTH3D_M_vCopyMatrix(&_hPos->stRotationMatrix, &stMatrix);   }
    else
        MTH3D_M_vMulMatrixMatrix(&_hPos->stRotationMatrix, &_hPos->stRotationMatrix, &stMatrix);

    if (_hPos->ulType <= POS_C_ulRotationMatrixFlag)
    {   MTH3D_M_vCopyMatrix(&_hPos->stTransformMatrix, &_hPos->stRotationMatrix);   }
    else
        MTH3D_M_vMulMatrixMatrix(&_hPos->stTransformMatrix, &_hPos->stTransformMatrix, &stMatrix);

    _hPos->ulType = (_hPos->ulType > POS_C_ulRotationMatrixFlag) ? _hPos->ulType : POS_C_ulRotationMatrixFlag;
}

/* 0x00445BE0 */
void POS_fn_vRotateX(POS_tdstCompletePosition *hMatrix, MTH_tdxReal xXAngle)
{
    MTH3D_tdstVector stI;
    MTH3D_tdstVector stJ;
    MTH3D_tdstVector stK;
    MTH_tdxReal xSin = MTH_M_xSin(xXAngle);
    MTH_tdxReal xCos = MTH_M_xCos(xXAngle);

    MTH3D_M_vSetVectorElements(&stI, MTH_C_ONE, MTH_C_ZERO, MTH_C_ZERO);
    MTH3D_M_vSetVectorElements(&stJ, MTH_C_ZERO, xCos, xSin);
    MTH3D_M_vSetVectorElements(&stK, MTH_C_ZERO, MTH_M_xNeg(xSin), xCos);

    POS_fn_vSetRotationMatrix(hMatrix, &stI, &stJ, &stK);
}

void POS_fn_vSetTransformMatrix(POS_tdstCompletePosition *_hMatrix,
                                MTH3D_tdstVector *_p_stI,
                                MTH3D_tdstVector *_p_stJ,
                                MTH3D_tdstVector *_p_stK)
{

    _hMatrix->stTransformMatrix.stCol_0.xX = _p_stI->xX;
    _hMatrix->stTransformMatrix.stCol_0.xY = _p_stI->xY;
    _hMatrix->stTransformMatrix.stCol_0.xZ = _p_stI->xZ;
    _hMatrix->stTransformMatrix.stCol_1.xX = _p_stJ->xX;
    _hMatrix->stTransformMatrix.stCol_1.xY = _p_stJ->xY;
    _hMatrix->stTransformMatrix.stCol_1.xZ = _p_stJ->xZ;
    _hMatrix->stTransformMatrix.stCol_2.xX = _p_stK->xX;
    _hMatrix->stTransformMatrix.stCol_2.xY = _p_stK->xY;
    _hMatrix->stTransformMatrix.stCol_2.xZ = _p_stK->xZ;
    _hMatrix->ulType = POS_C_ulCompleteMatrixFlag;
}

void POS_fn_vSetTranslationVector(POS_tdstCompletePosition *_hMatrix, MTH3D_tdstVector *_p_stTrs)
{

    _hMatrix->stTranslationVector.xX = _p_stTrs->xX;
    _hMatrix->stTranslationVector.xY = _p_stTrs->xY;
    _hMatrix->stTranslationVector.xZ = _p_stTrs->xZ;
    if (_hMatrix->ulType == POS_C_ulIdentityMatrixFlag)
        _hMatrix->ulType = POS_C_ulTranslationMatrixFlag;
}

void POS_fn_vGetTranslationVector(POS_tdstCompletePosition *_hMatrix, MTH3D_tdstVector *_p_stTrs)
{

    _p_stTrs->xX = _hMatrix->stTranslationVector.xX;
    _p_stTrs->xY = _hMatrix->stTranslationVector.xY;
    _p_stTrs->xZ = _hMatrix->stTranslationVector.xZ;
}

MTH_tdxReal POS_fn_xGetXScale(POS_tdstCompletePosition *_hMatrix)
{

    MTH_tdxReal xResult = MTH_C_ONE;
    if (_hMatrix->ulType > POS_C_ulRotationMatrixFlag)
    {
        MTH_tdxReal xX = _hMatrix->stTransformMatrix.stCol_0.xX;
        MTH_tdxReal xY = _hMatrix->stTransformMatrix.stCol_1.xX;
        MTH_tdxReal xZ = _hMatrix->stTransformMatrix.stCol_2.xX;
        xResult = MTH_M_xSqrt(xX * xX + xY * xY + xZ * xZ);
    }
    return xResult;
}

MTH_tdxReal POS_fn_xGetYScale(POS_tdstCompletePosition *_hMatrix)
{

    MTH_tdxReal xResult = MTH_C_ONE;
    if (_hMatrix->ulType > POS_C_ulRotationMatrixFlag)
    {
        MTH_tdxReal xX = _hMatrix->stTransformMatrix.stCol_0.xY;
        MTH_tdxReal xY = _hMatrix->stTransformMatrix.stCol_1.xY;
        MTH_tdxReal xZ = _hMatrix->stTransformMatrix.stCol_2.xY;
        xResult = MTH_M_xSqrt(xX * xX + xY * xY + xZ * xZ);
    }
    return xResult;
}

MTH_tdxReal POS_fn_xGetMaxScale(POS_tdstCompletePosition *_hMatrix)
{

	MTH_tdxReal xRet;
	MTH_tdxReal xL1,xL2,xL3;

	if ( _hMatrix->ulType <= POS_C_ulRotationMatrixFlag )
		return MTH_C_ONE;

	xL1 = MTH3D_M_xSqrVector(&_hMatrix->stTransformMatrix.stCol_0);
	xL2 = MTH3D_M_xSqrVector(&_hMatrix->stTransformMatrix.stCol_1);
	xL3 = MTH3D_M_xSqrVector(&_hMatrix->stTransformMatrix.stCol_2);
	xRet = MTH_M_xMax(xL1, xL2);
	xRet = MTH_M_xMax(xRet, xL3);
	xRet = MTH_M_xSqrt(xRet);
	return xRet;
}

void POS_fn_vQuatToMat(MTH3D_tdstMatrix *_p_stMatrix,
                       MTH_tdxReal _xX, MTH_tdxReal _xY, MTH_tdxReal _xZ, MTH_tdxReal _xW)
{

	MTH_tdxReal xXs,xYs,xZs,xWx,xWy,xWz,xXx,xXy,xXz,xYy,xYz,xZz;

	xXs = MTH_M_xAdd(_xX,_xX);
	xYs = MTH_M_xAdd(_xY,_xY);
	xZs = MTH_M_xAdd(_xZ,_xZ);

	xWx = MTH_M_xMul(_xW,xXs);
	xWy = MTH_M_xMul(_xW,xYs);
	xWz = MTH_M_xMul(_xW,xZs);
	xXx = MTH_M_xMul(_xX,xXs);
	xXy = MTH_M_xMul(_xX,xYs);
	xXz = MTH_M_xMul(_xX,xZs);
	xYy = MTH_M_xMul(_xY,xYs);
	xYz = MTH_M_xMul(_xY,xZs);
	xZz = MTH_M_xMul(_xZ,xZs);

	(_p_stMatrix->stCol_0).xX =MTH_M_xSub(MTH_C_ONE,MTH_M_xAdd(xYy,xZz));
	(_p_stMatrix->stCol_1).xX =MTH_M_xAdd(xXy,xWz);
	(_p_stMatrix->stCol_2).xX =MTH_M_xSub(xXz,xWy);

	(_p_stMatrix->stCol_0).xY =MTH_M_xSub(xXy,xWz);
	(_p_stMatrix->stCol_1).xY =MTH_M_xSub(MTH_C_ONE,MTH_M_xAdd(xXx,xZz));
	(_p_stMatrix->stCol_2).xY =MTH_M_xAdd(xYz,xWx);

	(_p_stMatrix->stCol_0).xZ =MTH_M_xAdd(xXz,xWy);
	(_p_stMatrix->stCol_1).xZ =MTH_M_xSub(xYz,xWx);
	(_p_stMatrix->stCol_2).xZ =MTH_M_xSub(MTH_C_ONE,MTH_M_xAdd(xXx,xYy));
}
