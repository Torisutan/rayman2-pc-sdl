#include <string.h>
#include <math.h>
#include "AI/AI_types.h"
#include "GAM/GAM_types.h"
#include "GMT/GMT_types.h"
#include "AI/AI_globals.h"
#include "MMG/MMG_globals.h"
#include "GAM/GAM_globals.h"

/* 0x00449C10 */
extern long __cdecl RND_fn_lGetLongRandomValue(long _lIndexNumber, long _lMin, long _lMax);
#define AI_M_pfn_fn_lGetLongRandomValue  ((long (__cdecl *)(long, long, long))&RND_fn_lGetLongRandomValue)

#define AI_M_pfn_fn_ucGetFunctionUltraOperator  fn_ucGetFunctionUltraOperator

static void AI_fn_vStorePersoPosition(HIE_tdstSuperObject *p_SuperObj, AI_tdstGetSetParam *p_stValue)
{
    const MTH3D_tdstVector *p_stVector = &HIE_fn_hGetSuperObjectGlobalMatrix(p_SuperObj)->stTranslationVector;
    p_stValue->uParam.a_xVectorValue[0] = p_stVector->xX;
    p_stValue->uParam.a_xVectorValue[1] = p_stVector->xY;
    p_stValue->uParam.a_xVectorValue[2] = p_stVector->xZ;
    p_stValue->eType = 0x15;
}

/* 0x0047B7D0 */
AI_tdstNodeInterpret *__cdecl fn_p_stGetPersoAbsolutePosition(
    HIE_tdstSuperObject *p_SuperObjPerso, AI_tdstNodeInterpret *p_stTree, AI_tdstGetSetParam *p_stValue)
{
    unsigned long        eFuncId = AI_M_eFuncIdInterpret(p_stTree-1);
    HIE_tdstSuperObject *hNewSuperObjPerso;

    fn_vGetUltraOperatorPerso(
        AI_M_pfn_fn_ucGetFunctionUltraOperator(eFuncId), p_SuperObjPerso, &hNewSuperObjPerso);

    M_Full_GetSetParam_p_stVertex(p_stValue, POS_fn_vGetTranslationVectorReadOnly(hNewSuperObjPerso->hGlobalMatrix));
    return(p_stTree);
}

/* 0x0047B830 */
AI_tdstNodeInterpret *__cdecl fn_p_stGetMyAbsolutePosition(
    HIE_tdstSuperObject *p_SuperObjPerso, AI_tdstNodeInterpret *p_stTree, AI_tdstGetSetParam *p_stValue)
{

    M_Full_GetSetParam_p_stVertex(p_stValue, POS_fn_vGetTranslationVectorReadOnly(p_SuperObjPerso->hGlobalMatrix));
    return(p_stTree);
}

static const unsigned long AI_FUNC_xDegToRad_bits = 0x3C8EFA35u;
#define AI_FUNC_xDegToRad   (*(const float *)&AI_FUNC_xDegToRad_bits)
#define AI_FUNC_xRadToDeg   AI_K_xRadToDeg   /* 0x0049CCE4 */

/* 0x00449C70 */
extern float __cdecl RND_fn_xGetRealRandomValue(long _lIndexNumber, float _xMin, float _xMax);
#define AI_M_pfn_fn_xGetRealRandomValue  ((float (__cdecl *)(long, float, float))&RND_fn_xGetRealRandomValue)

#define AI_M_pfn_fn_p_stSkipArgFunc      ((AI_tdstNodeInterpret *(__cdecl *)(AI_tdstNodeInterpret *))&fn_p_stSkipThisArgument)

#define AI_FUNC_xReadReal(v)  ((v)->eType == 8 ? (v)->uParam.xValue : (float)(v)->uParam.lValue)

#define AI_FUNC_ulUsefulDeltaTimeMs   GAM_g_ulUsefulDeltaTime  /* 0x500434 */
#define AI_FUNC_xFrameLength          GAM_g_xFrameLength  /* 0x50043C */

/* 0x0047C810 */
AI_tdstNodeInterpret *__cdecl fn_p_stRealFunction(
    HIE_tdstSuperObject *p_SuperObjPerso, AI_tdstNodeInterpret *p_stTree, AI_tdstGetSetParam *p_stValue)
{
    AI_tdstNodeInterpret *p_stNext;
    float xValue1, xCoeff, xValue2, xResult, xTmp;
    long  iNbFrames;

    p_stNext = fn_p_stEvalTree(p_SuperObjPerso, p_stTree, p_stValue);
    xValue1  = AI_FUNC_xReadReal(p_stValue);
    p_stNext = fn_p_stEvalTree(p_SuperObjPerso, p_stNext, p_stValue);
    xCoeff   = AI_FUNC_xReadReal(p_stValue);
    p_stNext = fn_p_stEvalTree(p_SuperObjPerso, p_stNext, p_stValue);
    xValue2  = AI_FUNC_xReadReal(p_stValue);

    iNbFrames = (long)((float)AI_FUNC_ulUsefulDeltaTimeMs
                       / (AI_FUNC_xFrameLength * 1000.0f) + 0.5f);

    switch (iNbFrames)
    {
    case 1:  xResult = xCoeff;                          break;
    case 2:  xResult = xCoeff * xCoeff;                 break;
    case 3:  xResult = xCoeff * xCoeff * xCoeff;        break;
    case 4:  xTmp = xCoeff * xCoeff; xResult = xTmp * xTmp;          break;
    case 5:  xTmp = xCoeff * xCoeff; xResult = xTmp * xTmp * xCoeff; break;
    default: xTmp = xCoeff * xCoeff * xCoeff; xResult = xTmp * xTmp; break;
    }

    xResult = (xValue1 - xValue2) * xResult + xValue2;

    p_stValue->uParam.xValue = xResult;
    p_stValue->eType         = 8;
    return p_stNext;
}

#define AI_M_pfn_fn_h_StandardGameGetSuperObject  fn_h_StandardGameGetSuperObject
#define AI_FUNC_ulInfinitPlus   (0x7F7FFFF8u)

static unsigned long AI_fn_ulQuickSqrtBits(float xSq)
{
    unsigned long ulBits = *(unsigned long *)&xSq;
    return (unsigned long)
        ((int)((((ulBits & 0x7f800000) + 0x3f800000) >> 1) & 0x7f800000)
       + *(const int *)((const char *)MTH_g_a2048_fSquareRootTable + ((ulBits & 0xffe000) >> 0xb)));
}

#define AI_M_pfn_POS_fn_vMulMatrixVector(d, m, s) \
    POS_fn_vMulMatrixVector((MTH3D_tdstVector *)(d), (POS_tdstCompletePosition *)(m), (MTH3D_tdstVector *)(s))
#define AI_M_pfn_POS_fn_vGetTranslationVector(m, v) \
    POS_fn_vGetTranslationVector((POS_tdstCompletePosition *)(m), (MTH3D_tdstVector *)(v))
/* 0x00482D20 */
extern MTH_tdxReal __cdecl fn_xComputeDirectionAngle(MTH3D_tdstVector *p_stVector);
#define AI_M_pfn_fn_xComputeDirectionAngle(v)         fn_xComputeDirectionAngle((MTH3D_tdstVector *)(v))
/* 0x00482E50 */
float __cdecl fn_xRealModuloAround(float xFrom, float xModulus, float xValue);
#define AI_M_pfn_fn_xRealModuloAround                 fn_xRealModuloAround
float __cdecl fn_xComputeBestEcartDirectionAngle(float xAngleFollowed, float xAngleToFollow);
#define AI_M_pfn_fn_xComputeBestEcartDirectionAngle   fn_xComputeBestEcartDirectionAngle
/* 0x00482C50 */
extern float __cdecl fn_xExtendedDistanceBetweenPerso(void *, void *, unsigned char);
extern float __cdecl fn_xDistanceBetweenPersoAndWP(void *, void *);
#define AI_M_pfn_fn_xExtendedDistanceBetweenPerso     ((float (__cdecl *)(void *, void *, unsigned char))fn_xExtendedDistanceBetweenPerso)
#define AI_M_pfn_fn_xDistanceBetweenPersoAndWP        ((float (__cdecl *)(void *, void *))fn_xDistanceBetweenPersoAndWP)

/* 0x0047B8D0 */
AI_tdstNodeInterpret *__cdecl fn_p_stDistanceFunction(
    HIE_tdstSuperObject *p_SuperObjPerso, AI_tdstNodeInterpret *p_stTree, AI_tdstGetSetParam *p_stValue)
{
    long                  eFuncId = AI_M_eFuncIdInterpret(p_stTree-1);
    unsigned char         ucFlags;
    MTH_tdxReal           xReal = 0.0f;
    HIE_tdstSuperObject  *hNewSuperObjPerso;

    fn_vGetUltraOperatorPerso(
        AI_M_pfn_fn_ucGetFunctionUltraOperator(eFuncId), p_SuperObjPerso, &hNewSuperObjPerso);

    switch (eFuncId)
    {
    case 0:
        {
            MTH3D_tdstVector stVector;

            POS_fn_vGetTranslationVector(HIE_fn_hGetSuperObjectGlobalMatrix(hNewSuperObjPerso), &stVector);
            M_Full_GetSetParam_p_stVertex(p_stValue,&stVector);
            return (p_stTree);
            break;
        }

    case 2:
        {
            MTH3D_tdstVector     stVector1, stVector2;
            MTH_tdxReal          xAngleEnd, xAngleStart, xDeltaAngle;
            HIE_tdstSuperObject *hAimedPerso;

            p_stTree    = fn_p_stEvalTree(p_SuperObjPerso, p_stTree, p_stValue);
            hAimedPerso = M_GetSetParam_p_stSupObjValue(p_stValue);

            MTH3D_M_vSetVectorElements(&stVector1, 0.0f, -1.0f, 0.0f);

            AI_M_pfn_POS_fn_vMulMatrixVector(&stVector1, HIE_fn_hGetSuperObjectGlobalMatrix(hNewSuperObjPerso), &stVector1);
            xAngleStart = AI_M_pfn_fn_xComputeDirectionAngle(&stVector1);
            xAngleStart = AI_M_pfn_fn_xRealModuloAround(MTH_C_ZERO, MTH_C_2Pi, xAngleStart);

            POS_fn_vGetTranslationVector(HIE_fn_hGetSuperObjectGlobalMatrix(hNewSuperObjPerso), &stVector1);
            POS_fn_vGetTranslationVector(HIE_fn_hGetSuperObjectGlobalMatrix(hAimedPerso), &stVector2);
            MTH3D_M_vSubVector(&stVector2, &stVector2, &stVector1);
            xAngleEnd = AI_M_pfn_fn_xComputeDirectionAngle(&stVector2);
            xAngleEnd = AI_M_pfn_fn_xRealModuloAround(MTH_C_ZERO, MTH_C_2Pi, xAngleEnd);

            xDeltaAngle = AI_M_pfn_fn_xComputeBestEcartDirectionAngle(xAngleStart, xAngleEnd) * AI_FUNC_xRadToDeg;

            M_Full_GetSetParam_Float(p_stValue, xDeltaAngle);
            return (p_stTree);
            break;
        }

    case 0x11:
        {
            p_stTree = fn_p_stEvalTree(p_SuperObjPerso, p_stTree, p_stValue);
            xReal = AI_M_pfn_fn_xDistanceBetweenPersoAndWP(hNewSuperObjPerso, p_stValue->uParam.p_stEngObjValue);
            M_Full_GetSetParam_Float(p_stValue, xReal);
            return (p_stTree);
            break;
        }

    default:
        {
            HIE_tdstSuperObject *p_SuperObjTarget;

            p_stTree = fn_p_stEvalTree(p_SuperObjPerso, p_stTree, p_stValue);
            p_SuperObjTarget = M_GetSetParam_p_stSupObjValue(p_stValue);
            switch (eFuncId)
            {
            case 4:    ucFlags = 0x01; break;
            case 5:    ucFlags = 0x02; break;
            case 6:    ucFlags = 0x04; break;
            case 7:    ucFlags = 0x03; break;
            case 8:    ucFlags = 0x05; break;
            case 9:    ucFlags = 0x06; break;
            case 0xb:  ucFlags = 0x81; break;
            case 0xc:  ucFlags = 0x82; break;
            case 0xd:  ucFlags = 0x84; break;
            case 0xe:  ucFlags = 0x83; break;
            case 0xf:  ucFlags = 0x85; break;
            case 0x10: ucFlags = 0x86; break;
            default:   ucFlags = 0; break;
            }
            xReal = AI_M_pfn_fn_xExtendedDistanceBetweenPerso(hNewSuperObjPerso, p_SuperObjTarget, ucFlags);
            M_Full_GetSetParam_Float(p_stValue, xReal);
            return (p_stTree);
            break;
        }
    }
}

/* 0x0047BB70 */
AI_tdstNodeInterpret *__cdecl fn_p_stMathTernarReal(
    HIE_tdstSuperObject *p_SuperObjPerso, AI_tdstNodeInterpret *p_stTree, AI_tdstGetSetParam *p_stValue)
{
    unsigned long         eFuncId  = *(unsigned long *)((char *)p_stTree - 8);
    AI_tdstNodeInterpret *p_stNode = fn_p_stEvalTree(p_SuperObjPerso, p_stTree, p_stValue);
    AI_tdstNodeInterpret *p_stRet;
    char                  bTestResult;
    float                 xRet;

    if (eFuncId == 0x29)
    {
        bTestResult = p_stValue->uParam.cValue;
    }
    else
    {
        float x1 = AI_FUNC_xReadReal(p_stValue);
        float x2;
        p_stNode = fn_p_stEvalTree(p_SuperObjPerso, p_stNode, p_stValue);
        x2 = AI_FUNC_xReadReal(p_stValue);
        switch (eFuncId)
        {
        case 0x24: bTestResult = (char)(x1 <  x2); break;
        case 0x25: bTestResult = (char)(x1 >  x2); break;
        case 0x27: bTestResult = (char)(x1 <= x2); break;
        case 0x28: bTestResult = (char)(x1 >= x2); break;
        case 0x26: bTestResult = (char)(x1 == x2); break;
        default:   bTestResult = 0;                break;
        }
    }

    if (bTestResult)
    {
        AI_tdstNodeInterpret *p_stAfterTrue = fn_p_stEvalTree(p_SuperObjPerso, p_stNode, p_stValue);
        xRet    = AI_FUNC_xReadReal(p_stValue);
        p_stRet = AI_M_pfn_fn_p_stSkipArgFunc(p_stAfterTrue);
    }
    else
    {
        AI_tdstNodeInterpret *p_stAfterTrue = AI_M_pfn_fn_p_stSkipArgFunc(p_stNode);
        p_stRet = fn_p_stEvalTree(p_SuperObjPerso, p_stAfterTrue, p_stValue);
        xRet    = AI_FUNC_xReadReal(p_stValue);
    }

    p_stValue->uParam.xValue = xRet;
    p_stValue->eType         = 8;
    return p_stRet;
}

/* 0x0047BCE0 */
AI_tdstNodeInterpret *__cdecl fn_p_stMathFunctionInt(
    HIE_tdstSuperObject *p_SuperObjPerso, AI_tdstNodeInterpret *p_stTree, AI_tdstGetSetParam *p_stValue)
{
    unsigned long eFuncId = *(unsigned long *)((char *)p_stTree - 8);
    long          lFirstValue = 0;

    p_stTree    = fn_p_stEvalTree(p_SuperObjPerso, p_stTree, p_stValue);
    lFirstValue = (p_stValue->eType == 8) ? (long)p_stValue->uParam.xValue : p_stValue->uParam.lValue;

    switch (eFuncId)
    {
    case 0x13:
        break;

    case 0x14:
        {
            long lMax;

            p_stTree = fn_p_stEvalTree(p_SuperObjPerso, p_stTree, p_stValue);
            lMax = (p_stValue->eType == 8) ? (long)p_stValue->uParam.xValue : p_stValue->uParam.lValue;

            lFirstValue = AI_M_pfn_fn_lGetLongRandomValue(0, lFirstValue, lMax);
        }
        break;
    default:
        p_stValue->uParam.lValue = lFirstValue;
        p_stValue->eType         = 6;
        return p_stTree;
    }
    p_stValue->uParam.lValue = lFirstValue;
    p_stValue->eType         = 6;
    return p_stTree;
}

/* 0x0047BD70 */
AI_tdstNodeInterpret *__cdecl fn_p_stMathAbsoluteValue(
    HIE_tdstSuperObject *p_SuperObjPerso, AI_tdstNodeInterpret *p_stTree, AI_tdstGetSetParam *p_stValue)
{
    MTH_tdxReal xAbs;

    p_stTree = fn_p_stEvalTree(p_SuperObjPerso, p_stTree, p_stValue);
    xAbs = AI_FUNC_xReadReal(p_stValue);
    M_Full_GetSetParam_Float(p_stValue, MTH_M_xAbs(xAbs));
    return p_stTree;
}

/* 0x0047BDC0 */
AI_tdstNodeInterpret *__cdecl fn_p_stMathFunctionReal(
    HIE_tdstSuperObject *p_SuperObjPerso, AI_tdstNodeInterpret *p_stTree, AI_tdstGetSetParam *p_stValue)
{
    long        eFuncId = *(long *)((char *)p_stTree - 8);
    MTH_tdxReal xFirstReal;

    p_stTree   = fn_p_stEvalTree(p_SuperObjPerso, p_stTree, p_stValue);
    xFirstReal = M_ReturnParam_xValue(p_stValue);

    switch (eFuncId)
    {
    case 0x15:
        break;

    case 0x16:
        xFirstReal = MTH_M_xSin(MTH_M_xDegToRad(xFirstReal));
        break;

    case 0x17:
        xFirstReal = MTH_M_xCos(MTH_M_xDegToRad(xFirstReal));
        break;

    case 0x18:
        xFirstReal = MTH_M_xSqr(xFirstReal);
        break;

    case 0x19:
        xFirstReal = MTH_fn_fSquareRootOpt(xFirstReal);
        break;

    case 0x1a:
        {
            MTH_tdxReal xMax;

            p_stTree = fn_p_stEvalTree(p_SuperObjPerso, p_stTree, p_stValue);
            xMax = M_ReturnParam_xValue(p_stValue);

            xFirstReal = AI_M_pfn_fn_xGetRealRandomValue(0, xFirstReal, xMax);
        }
        break;

    case 0x21:
        xFirstReal = MTH_M_bLessZero(xFirstReal)
            ? MTH_C_MinusONE
            : (MTH_M_bGreaterZero(xFirstReal) ? MTH_C_ONE : MTH_C_ZERO);
        break;

    case 0x22:
        xFirstReal = MTH_M_xMul(MTH_M_xSqr(xFirstReal), xFirstReal);
        break;

    case 0x23:
        {
            MTH_tdxReal xMin, xMax, xModulo;

            p_stTree = fn_p_stEvalTree(p_SuperObjPerso, p_stTree, p_stValue);
            xMin = M_ReturnParam_xValue(p_stValue);
            p_stTree = fn_p_stEvalTree(p_SuperObjPerso, p_stTree, p_stValue);
            xMax = M_ReturnParam_xValue(p_stValue);

            xModulo = MTH_M_xSub(xMax, xMin);

            while (MTH_M_bGreaterEqual(xFirstReal, xMax))
                xFirstReal = MTH_M_xSub(xFirstReal, xModulo);
            while (MTH_M_bLess(xFirstReal, xMin))
                xFirstReal = MTH_M_xAdd(xFirstReal, xModulo);
        }
        break;

    case 0x1b:
        {
            MTH_tdxReal xMax;

            p_stTree = fn_p_stEvalTree(p_SuperObjPerso, p_stTree, p_stValue);
            xMax = M_ReturnParam_xValue(p_stValue);

            xFirstReal = MTH_M_xMin(xFirstReal, xMax);
        }
        break;

    case 0x1c:
        {
            MTH_tdxReal xMax;

            p_stTree = fn_p_stEvalTree(p_SuperObjPerso, p_stTree, p_stValue);
            xMax = M_ReturnParam_xValue(p_stValue);
            xFirstReal = MTH_M_xMax(xFirstReal, xMax);
        }
        break;

    case 0x1d:
        xFirstReal = MTH_M_xDegToRad(xFirstReal);
        break;

    case 0x1e:
        xFirstReal = MTH_M_xRadToDeg(xFirstReal);
        break;

    case 0x20:
        {
            MTH_tdxReal xMin, xMax;

            p_stTree = fn_p_stEvalTree(p_SuperObjPerso, p_stTree, p_stValue);
            xMin = M_ReturnParam_xValue(p_stValue);
            if (MTH_M_bLess(xFirstReal, xMin))
            {
                xFirstReal = xMin;

                p_stTree = fn_p_stSkipThisArgument(p_stTree);
            }
            else
            {
                p_stTree = fn_p_stEvalTree(p_SuperObjPerso, p_stTree, p_stValue);
                xMax = M_ReturnParam_xValue(p_stValue);

                if (MTH_M_bGreater(xFirstReal, xMax))
                    xFirstReal = xMax;
            }
        }
        break;

    default:
        M_Full_GetSetParam_Float(p_stValue, xFirstReal);
        return (p_stTree);
        break;
    }
    M_Full_GetSetParam_Float(p_stValue, xFirstReal);
    return (p_stTree);
}

static const MTH3D_tdstVector *AI_fn_p_xComputePersoGlobalCenter(HIE_tdstSuperObject *p_SuperObj, MTH3D_tdstVector *p_stLocal)
{
    POS_tdstCompletePosition *hGlobalMatrix;
    GEO_tdxHandleToBoundingSphere hBoundingSphere;

    hGlobalMatrix   = HIE_fn_hGetSuperObjectGlobalMatrix(p_SuperObj);
    hBoundingSphere = (GEO_tdxHandleToBoundingSphere)p_SuperObj->hBoundingVolume;

    if (hBoundingSphere == (GEO_tdxHandleToBoundingSphere)0)
        return &hGlobalMatrix->stTranslationVector;

    p_stLocal->xX = hBoundingSphere->stCenterPoint.xZ * hGlobalMatrix->stRotationMatrix.stCol_2.xX
                  + hBoundingSphere->stCenterPoint.xY * hGlobalMatrix->stRotationMatrix.stCol_1.xX
                  + hBoundingSphere->stCenterPoint.xX * hGlobalMatrix->stRotationMatrix.stCol_0.xX
                  + hGlobalMatrix->stTranslationVector.xX;
    p_stLocal->xY = hBoundingSphere->stCenterPoint.xZ * hGlobalMatrix->stRotationMatrix.stCol_2.xY
                  + hBoundingSphere->stCenterPoint.xY * hGlobalMatrix->stRotationMatrix.stCol_1.xY
                  + hBoundingSphere->stCenterPoint.xX * hGlobalMatrix->stRotationMatrix.stCol_0.xY
                  + hGlobalMatrix->stTranslationVector.xY;
    p_stLocal->xZ = hBoundingSphere->stCenterPoint.xZ * hGlobalMatrix->stRotationMatrix.stCol_2.xZ
                  + hBoundingSphere->stCenterPoint.xY * hGlobalMatrix->stRotationMatrix.stCol_1.xZ
                  + hBoundingSphere->stCenterPoint.xX * hGlobalMatrix->stRotationMatrix.stCol_0.xZ
                  + hGlobalMatrix->stTranslationVector.xZ;
    return p_stLocal;
}

/* 0x0047B480 */
AI_tdstNodeInterpret *__cdecl fn_p_stDistanceToPersoCenter(
    HIE_tdstSuperObject *p_SuperObjPerso, AI_tdstNodeInterpret *p_stTree, AI_tdstGetSetParam *p_stValue)
{
	long							eFuncId=AI_M_eFuncIdInterpret(p_stTree-1);
	GEO_tdxHandleToBoundingSphere	hBoundingSphere1,hBoundingSphere2;
	MTH3D_tdstVector				stCenter1, *p_stCenter1;
	MTH3D_tdstVector				stCenter2, *p_stCenter2;
	MTH3D_tdstVector				stVect;
	HIE_tdstSuperObject				*p_SuperObjTarget;
	MTH3D_tdstVector				stGlobalCenter1,stGlobalCenter2;

    HIE_tdstSuperObject *hNewSuperObjPerso;
	fn_vGetUltraOperatorPerso(fn_ucGetFunctionUltraOperator(eFuncId),p_SuperObjPerso,&hNewSuperObjPerso);

	M_EvalNextParameter(p_stValue);
	p_SuperObjTarget=M_GetSetParam_p_stSupObjValue(p_stValue);

	hBoundingSphere1 = (GEO_tdxHandleToBoundingSphere)p_SuperObjTarget->hBoundingVolume;
	hBoundingSphere2 = (GEO_tdxHandleToBoundingSphere)hNewSuperObjPerso->hBoundingVolume;

	if (hBoundingSphere1)
	{

		p_stCenter1 = &stCenter1;
		MTH3D_M_vMulMatrixVector(&stGlobalCenter1, &HIE_fn_hGetSuperObjectGlobalMatrix(p_SuperObjTarget)->stRotationMatrix, &(hBoundingSphere1->stCenterPoint));
		MTH3D_M_vAddVector (p_stCenter1,&stGlobalCenter1,POS_fn_vGetTranslationVectorReadOnly(p_SuperObjTarget->hGlobalMatrix));
	}
	else
		p_stCenter1 = POS_fn_vGetTranslationVectorReadOnly(p_SuperObjTarget->hGlobalMatrix);

	if (hBoundingSphere2)
	{

		p_stCenter2 = &stCenter2;
		MTH3D_M_vMulMatrixVector(&stGlobalCenter2, &HIE_fn_hGetSuperObjectGlobalMatrix(hNewSuperObjPerso)->stRotationMatrix, &(hBoundingSphere2->stCenterPoint));
		MTH3D_M_vAddVector (p_stCenter2,&stGlobalCenter2,POS_fn_vGetTranslationVectorReadOnly(hNewSuperObjPerso->hGlobalMatrix));
	}
	else
		p_stCenter2 = POS_fn_vGetTranslationVectorReadOnly(hNewSuperObjPerso->hGlobalMatrix);

	MTH3D_M_vSubVector(&stVect,p_stCenter1,p_stCenter2);

	M_Full_GetSetParam_Float(p_stValue, MTH_fn_fSquareRootOpt(MTH3D_M_xSqrVector(&stVect)));
	return(p_stTree);
}

/* 0x0047B6C0 */
AI_tdstNodeInterpret *__cdecl fn_p_stDistanceToPerso(
    HIE_tdstSuperObject *p_SuperObjPerso, AI_tdstNodeInterpret *p_stTree, AI_tdstGetSetParam *p_stValue)
{
    long                  eFuncId=AI_M_eFuncIdInterpret(p_stTree-1);
    MTH3D_tdstVector      stVect;
    HIE_tdstSuperObject  *p_SuperObjTarget;

    HIE_tdstSuperObject  *hNewSuperObjPerso;
    fn_vGetUltraOperatorPerso(fn_ucGetFunctionUltraOperator(eFuncId),p_SuperObjPerso,&hNewSuperObjPerso);

    M_EvalNextParameter(p_stValue);

    if (M_GetSetParam_xValue(p_stValue)==0)
    {

        M_Full_GetSetParam_Float(p_stValue, MTH_C_InfinitPlus);
        return(p_stTree);
    }

    p_SuperObjTarget=M_GetSetParam_p_stSupObjValue(p_stValue);

    MTH3D_M_vSubVector(&stVect,
        POS_fn_vGetTranslationVectorReadOnly(p_SuperObjTarget->hGlobalMatrix),
        POS_fn_vGetTranslationVectorReadOnly(hNewSuperObjPerso->hGlobalMatrix));

    M_Full_GetSetParam_Float(p_stValue, MTH_fn_fSquareRootOpt(MTH3D_M_xSqrVector(&stVect)));
    return(p_stTree);
}

/* 0x0047C1A0 */
AI_tdstNodeInterpret *__cdecl fn_p_stLocalToGlobal(
    HIE_tdstSuperObject *p_SuperObjPerso, AI_tdstNodeInterpret *p_stTree, AI_tdstGetSetParam *p_stValue)
{

    long                  eFuncId = AI_M_eFuncIdInterpret(p_stTree-1);
    MTH3D_tdstVector     *p_stVector = (MTH3D_tdstVector *)p_stValue->uParam.a_xVectorValue;
    HIE_tdstSuperObject  *hNewSuperObjPerso;

    fn_vGetUltraOperatorPerso(
        AI_M_pfn_fn_ucGetFunctionUltraOperator(eFuncId), p_SuperObjPerso, &hNewSuperObjPerso);

    p_stTree = fn_p_stEvalTree(p_SuperObjPerso, p_stTree, p_stValue);
    MTH3D_M_vMulMatrixVector(p_stVector,
        &HIE_fn_hGetSuperObjectGlobalMatrix(hNewSuperObjPerso)->stRotationMatrix,
        p_stVector);
    return (p_stTree);
}

/* 0x0047C250 */
AI_tdstNodeInterpret *__cdecl fn_p_stGlobalToLocal(
    HIE_tdstSuperObject *p_SuperObjPerso, AI_tdstNodeInterpret *p_stTree, AI_tdstGetSetParam *p_stValue)
{
    long                  eFuncId = AI_M_eFuncIdInterpret(p_stTree-1);
    MTH3D_tdstVector     *p_stVector = (MTH3D_tdstVector *)p_stValue->uParam.a_xVectorValue;
    HIE_tdstSuperObject  *hNewSuperObjPerso;
    MTH3D_tdstMatrix      stTransposedMatrix;

    fn_vGetUltraOperatorPerso(
        AI_M_pfn_fn_ucGetFunctionUltraOperator(eFuncId), p_SuperObjPerso, &hNewSuperObjPerso);

    p_stTree = fn_p_stEvalTree(p_SuperObjPerso, p_stTree, p_stValue);

    MTH3D_M_vTranspMatrixWithoutBuffer(&stTransposedMatrix,
        &HIE_fn_hGetSuperObjectGlobalMatrix(hNewSuperObjPerso)->stRotationMatrix);
    MTH3D_M_vMulMatrixVector(p_stVector, &stTransposedMatrix, p_stVector);
    return (p_stTree);
}

#define AI_FUNC_ucReadBit(v)  ((unsigned char)((v)->eType == 8 ? (long)(v)->uParam.xValue : (v)->uParam.lValue))

/* 0x004838F0 */
extern void *__cdecl fn_p_stGetPersoInList(void *p_List, unsigned char ucPosition);
#define AI_M_pfn_fn_p_stGetPersoInList  ((HIE_tdstSuperObject *(__cdecl *)(void *, unsigned char))fn_p_stGetPersoInList)

/* 0x00487860 */
extern void __cdecl WP_fnv_WayPoint_ComputeLocation(void *_hWayPoint, float *_p_stVertex);
#define AI_M_pfn_WP_fnv_WayPoint_ComputeLocation \
    ((void (__cdecl *)(const void *, float *))WP_fnv_WayPoint_ComputeLocation)

/* 0x0047B860 */
AI_tdstNodeInterpret *__cdecl fn_p_stGetWpAbsolutePosition(
    HIE_tdstSuperObject *p_SuperObjPerso, AI_tdstNodeInterpret *p_stTree, AI_tdstGetSetParam *p_stValue)
{
    const float *p_hWP;

    p_stTree = fn_p_stEvalTree(p_SuperObjPerso, p_stTree, p_stValue);
    p_hWP    = (const float *)p_stValue->uParam.p_stEngObjValue;

    if (p_hWP != (const float *)0)
    {
        if (*(const void *const *)(p_hWP + 4) != (const void *)0)
        {
            AI_M_pfn_WP_fnv_WayPoint_ComputeLocation(p_hWP, p_stValue->uParam.a_xVectorValue);
            p_stValue->eType = 0x15;
        }
        else
        {
            p_stValue->uParam.a_xVectorValue[0] = p_hWP[0];
            p_stValue->uParam.a_xVectorValue[1] = p_hWP[1];
            p_stValue->uParam.a_xVectorValue[2] = p_hWP[2];
            p_stValue->eType = 0x15;
        }
    }
    else
    {
        p_stValue->uParam.a_xVectorValue[0] = 0.0f;
        p_stValue->uParam.a_xVectorValue[1] = 0.0f;
        p_stValue->uParam.a_xVectorValue[2] = 0.0f;
        p_stValue->eType = 0x15;
    }
    return p_stTree;
}

#include "GAM/HitPts.h"

/* 0x0047C940 */
AI_tdstNodeInterpret *__cdecl fn_p_stHitPointsAndHitPointsMaxFunction(
    HIE_tdstSuperObject *p_SuperObjPerso, AI_tdstNodeInterpret *p_stTree, AI_tdstGetSetParam *p_stValue)
{
    long eFuncId = AI_M_eFuncIdInterpret(p_stTree - 1);
    void *p_stObject;
    long lValue = 0;

    M_EvalNextParameter(p_stValue);
    p_stObject = M_GetSetParam_p_stEngineObjValue(p_stValue);

    switch (eFuncId)
    {
    case 0x2b:
        lValue = (long)fn_ucGetHitPoints(p_stObject);
        break;

    case 0x2c:
        M_EvalNextParameter(p_stValue);
        fn_vAddHitPoints(p_stObject, (unsigned char)M_ReturnParam_lValue(p_stValue));
        lValue = (long)fn_ucGetHitPoints(p_stObject);
        break;

    case 0x2d:
        M_EvalNextParameter(p_stValue);
        lValue = (long)fn_ucSubHitPoints(p_stObject, (unsigned char)M_ReturnParam_lValue(p_stValue));
        break;

    case 0x2e:
        lValue = (long)fn_ucGetHitPointsMax(p_stObject);
        break;

    case 0x2f:
        M_EvalNextParameter(p_stValue);
        fn_vAddHitPointsMax(p_stObject, (unsigned char)M_ReturnParam_lValue(p_stValue));
        lValue = (long)fn_ucGetHitPointsMax(p_stObject);
        break;

    case 0x30:
        M_EvalNextParameter(p_stValue);
        lValue = (long)fn_ucSubHitPointsMax(p_stObject, (unsigned char)M_ReturnParam_lValue(p_stValue));
        break;

    default:
        lValue = 0;
        break;
    }
    M_Full_GetSetParam_Integer(p_stValue, lValue);
    return (p_stTree);
}

#define AI_FUNC_ulCurrentTimerCount   GAM_g_ulAccumulatedTime  /* 0x5003EC */

/* 0x0047CAF0 */
AI_tdstNodeInterpret *__cdecl fn_p_stTimeFunction(
    HIE_tdstSuperObject *p_SuperObjPerso, AI_tdstNodeInterpret *p_stTree, AI_tdstGetSetParam *p_stValue)
{
    unsigned long eFuncId = *(unsigned long *)((char *)p_stTree - 8);
    unsigned long ulCurrentTime = AI_FUNC_ulCurrentTimerCount;

    switch (eFuncId)
    {
    case 0x3c:
        break;

    case 0x3d:
        {
            unsigned long ulSavedTime;

            p_stTree = fn_p_stEvalTree(p_SuperObjPerso, p_stTree, p_stValue);
            ulSavedTime = p_stValue->uParam.ulValue;
            if (ulCurrentTime >= ulSavedTime)
            {
                ulCurrentTime -= ulSavedTime;
            }
            else
            {
                ulCurrentTime = (unsigned long)(-(long)(ulCurrentTime - ulSavedTime));
            }
        }
        break;
    default:
        break;
    }
    p_stValue->uParam.ulValue = (unsigned long)ulCurrentTime;
    p_stValue->eType          = 7;
    return p_stTree;
}

/* 0x0047CB40 */
AI_tdstNodeInterpret *__cdecl fn_p_stListFunction(
    HIE_tdstSuperObject *p_SuperObjPerso, AI_tdstNodeInterpret *p_stTree, AI_tdstGetSetParam *p_stValue)
{
    unsigned long eFuncId = *(unsigned long *)((char *)p_stTree - 8);
    void         *p_stList;

    p_stTree = fn_p_stEvalTree(p_SuperObjPerso, p_stTree, p_stValue);
    p_stList = p_stValue->uParam.p_stEngObjValue;

    switch (eFuncId)
    {
    case 0x32:

        p_stTree = fn_p_stEvalTree(p_SuperObjPerso, p_stTree, p_stValue);
#define M_GetEngineObject_L(p_stSO) ((*(long *)(p_stSO) != 2  ) ? NULL : HIE_fn_hGetSuperObjectObject(p_stSO))
        p_stValue->uParam.lValue = (long)((AI_M_pfn_fn_p_stGetPersoInList(p_stList, (unsigned char)p_stValue->uParam.lValue) == NULL)
            ? NULL
            : M_GetEngineObject_L(AI_M_pfn_fn_p_stGetPersoInList(p_stList, (unsigned char)p_stValue->uParam.lValue)));
#undef M_GetEngineObject_L
        p_stValue->eType = 0x0b;
        return p_stTree;

    case 0x31:
        p_stValue->uParam.lValue = *(unsigned char *)p_stList;
        p_stValue->eType         = 6;
        return p_stTree;

    default:
        break;
    }
    return p_stTree;
}

/* 0x0047CBF0 */
AI_tdstNodeInterpret *__cdecl fn_p_stInputFunction(
    HIE_tdstSuperObject *p_SuperObjPerso, AI_tdstNodeInterpret *p_stTree, AI_tdstGetSetParam *p_stValue)
{
    AI_tdstNodeInterpret *p_stNext = fn_p_stEvalTree(p_SuperObjPerso, p_stTree, p_stValue);

    p_stValue->uParam.lValue = (long)AI_RT_IPT_fn_lGetAnalogicValue(p_stValue->uParam.lValue);
    p_stValue->eType         = 6;
    return p_stNext;
}

long __cdecl fn_lAddColor(long color1, long color2);

#define AI_M_pfn_fn_lAddColor    ((unsigned long (__cdecl *)(unsigned long, unsigned long))&fn_lAddColor)

AI_tdstNodeInterpret *__cdecl fn_p_stEvalRedOrGreenOrBlueOrAlpha( HIE_tdstSuperObject *p_SuperObjPerso, AI_tdstNodeInterpret *p_stTree, long *lValue);

/* 0x0047D480 */
AI_tdstNodeInterpret *__cdecl fn_p_stEvalRed(
    HIE_tdstSuperObject *p_SuperObjPerso, AI_tdstNodeInterpret *p_stTree, AI_tdstGetSetParam *p_stValue)
{
    long lValue;

    p_stTree = fn_p_stEvalRedOrGreenOrBlueOrAlpha(p_SuperObjPerso, p_stTree, &lValue);
    M_GetSetParam_RedValue(p_stValue)=lValue;
    return p_stTree;
}

/* 0x0047D4B0 */
AI_tdstNodeInterpret *__cdecl fn_p_stEvalRedOrGreenOrBlueOrAlpha(
    HIE_tdstSuperObject *p_SuperObjPerso, AI_tdstNodeInterpret *p_stTree, long *lValue)
{
    AI_tdstGetSetParam stParam;

    p_stTree = fn_p_stEvalTree(p_SuperObjPerso, p_stTree, &stParam);
    *lValue  = M_ReturnParam_lValue(&stParam);
    if (*lValue > 100)
        *lValue = 100;
    else if (*lValue < 0)
        *lValue = 0;
    return p_stTree;
}

/* 0x0047D510 */
AI_tdstNodeInterpret *__cdecl fn_p_stEvalGreen(
    HIE_tdstSuperObject *p_SuperObjPerso, AI_tdstNodeInterpret *p_stTree, AI_tdstGetSetParam *p_stValue)
{
    long lValue;

    p_stTree = fn_p_stEvalRedOrGreenOrBlueOrAlpha(p_SuperObjPerso, p_stTree, &lValue);
    M_GetSetParam_GreenValue(p_stValue)=lValue;
    return p_stTree;
}

/* 0x0047D540 */
AI_tdstNodeInterpret *__cdecl fn_p_stEvalBlue(
    HIE_tdstSuperObject *p_SuperObjPerso, AI_tdstNodeInterpret *p_stTree, AI_tdstGetSetParam *p_stValue)
{
    long lValue;

    p_stTree = fn_p_stEvalRedOrGreenOrBlueOrAlpha(p_SuperObjPerso, p_stTree, &lValue);
    M_GetSetParam_BlueValue(p_stValue)=lValue;
    return p_stTree;
}

AI_tdstNodeInterpret *__cdecl fn_p_stEvalRGB( HIE_tdstSuperObject *p_SuperObjPerso, AI_tdstNodeInterpret *p_stTree, AI_tdstGetSetParam *p_stValue);

AI_tdstNodeInterpret *__cdecl fn_p_stEvalAlpha( HIE_tdstSuperObject *p_SuperObjPerso, AI_tdstNodeInterpret *p_stTree, AI_tdstGetSetParam *p_stValue);

/* 0x0047D570 */
AI_tdstNodeInterpret *__cdecl fn_p_stEvalRGBA(
    HIE_tdstSuperObject *p_SuperObjPerso, AI_tdstNodeInterpret *p_stTree, AI_tdstGetSetParam *p_stValue)
{
    p_stTree = fn_p_stEvalRGB(p_SuperObjPerso, p_stTree, p_stValue);
    p_stTree = fn_p_stEvalAlpha(p_SuperObjPerso, p_stTree, p_stValue);
    return p_stTree;
}

/* 0x0047D5A0 */
AI_tdstNodeInterpret *__cdecl fn_p_stEvalAlpha(
    HIE_tdstSuperObject *p_SuperObjPerso, AI_tdstNodeInterpret *p_stTree, AI_tdstGetSetParam *p_stValue)
{
    long lValue;

    p_stTree = fn_p_stEvalRedOrGreenOrBlueOrAlpha(p_SuperObjPerso, p_stTree, &lValue);
    M_GetSetParam_AlphaValue(p_stValue)=lValue;
    return p_stTree;
}

/* 0x0047D5D0 */
long __cdecl fn_lAddColor(long color1, long color2)
{
    RGBAStruct c1;
    RGBAStruct c2;
    RGBAStruct c3;
    long r, g, b, a;

    RGBA_Color(c1) = color1;
    RGBA_Color(c2) = color2;

    r = RGBA_Red(c1)   + RGBA_Red(c2);
    g = RGBA_Green(c1) + RGBA_Blue(c2);
    b = RGBA_Blue(c1)  + RGBA_Green(c2);
    a = RGBA_Alpha(c1) + RGBA_Alpha(c2);

    if (r < 0) r = 0; else if (r > 100) r = 100;
    if (g < 0) g = 0; else if (g > 100) g = 100;
    if (b < 0) b = 0; else if (b > 100) b = 100;
    if (a < 0) a = 0; else if (a > 100) a = 100;

    RGBA_Red(c3)   = r;
    RGBA_Green(c3) = g;
    RGBA_Blue(c3)  = b;
    RGBA_Alpha(c3) = a;

    return (RGBA_Color(c3));
}

#include "GMT/GmtFunct.h"
#include "GLI/Material.h"
void __cdecl GLI_xGetMaterialTextureScrollingCoef(void *hMaterial, long lNumberOfTexture,
                                                  long *p_bSwitch, float *p_fUSpeed, float *p_fVSpeed);   /* 0x00424AF0 */
#include "AI/AITools.h"
#include "PRT/PRT_types.h"

/* 0x0047CF90 */
AI_tdstNodeInterpret *__cdecl fn_p_stVisualGameMaterialFunction(
    HIE_tdstSuperObject *p_SuperObjPerso, AI_tdstNodeInterpret *p_stTree, AI_tdstGetSetParam *p_stValue)
{
    long                    eFuncId = AI_M_eFuncIdInterpret(p_stTree - 1);
    void                   *hGameMaterial;
    void                   *hVisualMaterial = NULL;
    float                   xValue = 0.0f;
    GEO_tdstColor           stColor;
    long                    lValue;
    float                   fU, fV;

    M_EvalNextParameter(p_stValue);
    hGameMaterial = M_GetSetParam_p_stEngineObjValue(p_stValue);
    if (GMT_fn_b_ulIsValid(hGameMaterial))
    {
        hVisualMaterial = GMT_fn_hGetVisualMaterial(hGameMaterial);
        if (hVisualMaterial != NULL)
        {
            switch (eFuncId)
            {
            case 0x51:
                GLI_xGetMaterialColor(hVisualMaterial, &stColor);
                fn_vConvertColorToParam(&stColor, p_stValue);
                return (p_stTree);
                break;

            case 0x52:
                GLI_xGetMaterialSpecularCoef(hVisualMaterial, &lValue, &stColor);
                fn_vConvertColorToParam(&stColor, p_stValue);
                return (p_stTree);
                break;

            case 0x53:
                GLI_xGetMaterialSpecularCoef(hVisualMaterial, &lValue, &stColor);
                M_Full_GetSetParam_Integer(p_stValue, lValue);
                return (p_stTree);
                break;

            case 0x54:
                GLI_xGetMaterialDiffuseCoef(hVisualMaterial, &stColor);
                fn_vConvertColorToParam(&stColor, p_stValue);
                return (p_stTree);
                break;

            case 0x55:
                GLI_xGetMaterialAmbientCoef(hVisualMaterial, &stColor);
                fn_vConvertColorToParam(&stColor, p_stValue);
                return (p_stTree);
                break;

            case 0x56:
                GLI_xGetMaterialTextureScrollingCoef(hVisualMaterial,   0, &lValue, &fU, &fV);
                xValue = fU;
                M_Full_GetSetParam_Float(p_stValue, xValue);
                return (p_stTree);
                break;

            case 0x57:
                GLI_xGetMaterialTextureScrollingCoef(hVisualMaterial,   0, &lValue, &fU, &fV);
                xValue = fV;
                M_Full_GetSetParam_Float(p_stValue, xValue);
                return (p_stTree);
                break;

            case 0x58:
                M_Full_GetSetParam_Integer(p_stValue, GLI_lGetCurrentAnimatedTextureNumber(hVisualMaterial));
                return (p_stTree);
                break;

            case 0x59:
                M_Full_GetSetParam_Integer(p_stValue, GLI_lGetMaterialNumberOfAnimatedTexture(hVisualMaterial));
                return (p_stTree);
                break;

            default:
                break;
            }
        }
    }
    M_Full_GetSetParam_Integer(p_stValue, 0);
    return (p_stTree);
}

/* 0x0047D190 */
AI_tdstNodeInterpret *__cdecl fn_p_stColorFunction(
    HIE_tdstSuperObject *p_SuperObjPerso, AI_tdstNodeInterpret *p_stTree, AI_tdstGetSetParam *p_stValue)
{
	long eFuncId=AI_M_eFuncIdInterpret(p_stTree-1);
	long lValue=0;
	long color1=0,color2=0;
	RGBAStruct rgba;

	RGBA_Color(rgba)=0;

	switch(eFuncId)
	{
	case 0x4b:
		M_GetSetParam_lValue(p_stValue)=0;
		return(fn_p_stEvalRGBA(p_SuperObjPerso,p_stTree,p_stValue));
		break;

	case 0x4c:
		M_GetSetParam_lValue(p_stValue)=0;
		return(fn_p_stEvalRGB(p_SuperObjPerso,p_stTree,p_stValue));
		break;

	case 0x46:
		M_EvalNextParameter(p_stValue);	color1=M_GetSetParam_ColorValue(p_stValue);
		M_EvalNextParameter(p_stValue);	color2=M_GetSetParam_ColorValue(p_stValue);
		M_Full_GetSetParam_ColorValue(p_stValue,fn_lAddColor(color1,color2));
		return(p_stTree);
		break;

	case 0x47:
		M_EvalNextParameter(p_stValue);	color1=M_GetSetParam_ColorValue(p_stValue);
		M_EvalNextParameter(p_stValue);
		RGBA_Red(rgba)=M_ReturnParam_lValue(p_stValue);	color2=RGBA_Color(rgba);
		M_Full_GetSetParam_ColorValue(p_stValue,fn_lAddColor(color1,color2));
		return(p_stTree);
		break;

	case 0x48:
		M_EvalNextParameter(p_stValue); color1=M_GetSetParam_ColorValue(p_stValue);
		M_EvalNextParameter(p_stValue);
		RGBA_Green(rgba)=M_ReturnParam_lValue(p_stValue); color2=RGBA_Color(rgba);
		M_Full_GetSetParam_ColorValue(p_stValue,fn_lAddColor(color1,color2));
		return(p_stTree);
		break;

	case 0x49:
		M_EvalNextParameter(p_stValue);	color1=M_GetSetParam_ColorValue(p_stValue);
		M_EvalNextParameter(p_stValue);
		RGBA_Blue(rgba)=M_ReturnParam_lValue(p_stValue); color2=RGBA_Color(rgba);
		M_Full_GetSetParam_ColorValue(p_stValue,fn_lAddColor(color1,color2));
		return(p_stTree);
		break;

	case 0x4a:
		M_EvalNextParameter(p_stValue);	color1=M_GetSetParam_ColorValue(p_stValue);
		M_EvalNextParameter(p_stValue);
		RGBA_Alpha(rgba)=M_ReturnParam_lValue(p_stValue); color2=RGBA_Color(rgba);
		M_Full_GetSetParam_ColorValue(p_stValue,fn_lAddColor(color1,color2));
		return(p_stTree);
		break;

	case 0x4d:
		M_EvalNextParameter(p_stValue);
		lValue=M_GetSetParam_RedValue(p_stValue);
		break;

	case 0x4e:
		M_EvalNextParameter(p_stValue);
		lValue=M_GetSetParam_GreenValue(p_stValue);
		break;

	case 0x4f:
		M_EvalNextParameter(p_stValue);
		lValue=M_GetSetParam_BlueValue(p_stValue);
		break;

	case 0x50:
		M_EvalNextParameter(p_stValue);
		lValue=M_GetSetParam_AlphaValue(p_stValue);
		break;

	default:
		break;
	}

	if (lValue<0) lValue=0; else if (lValue>100) lValue=100;
	M_Full_GetSetParam_Integer(p_stValue,lValue);
	return(p_stTree);
}

/* 0x0047D440 */
AI_tdstNodeInterpret *__cdecl fn_p_stEvalRGB(
    HIE_tdstSuperObject *p_SuperObjPerso, AI_tdstNodeInterpret *p_stTree, AI_tdstGetSetParam *p_stValue)
{
    M_GetSetParam_AlphaValue(p_stValue)=0;

    p_stTree = fn_p_stEvalRed(p_SuperObjPerso, p_stTree, p_stValue);
    p_stTree = fn_p_stEvalGreen(p_SuperObjPerso, p_stTree, p_stValue);
    p_stTree = fn_p_stEvalBlue(p_SuperObjPerso, p_stTree, p_stValue);

    M_GetSetParam_Type(p_stValue)=E_vt_Color;
    return p_stTree;
}

/* 0x0047D6A0 */
AI_tdstNodeInterpret *__cdecl fn_p_st_SPO_GetDrawFlag(
    HIE_tdstSuperObject *p_SuperObjPerso, AI_tdstNodeInterpret *p_stTree, AI_tdstGetSetParam *p_stValue)
{
    AI_tdstGetSetParam    stParam;
    HIE_tdstSuperObject  *hSpo;
    long                  lBit;
    long                  lReturn = -1;

    p_stTree = fn_p_stEvalTree(p_SuperObjPerso, p_stTree, &stParam);
    hSpo     = (HIE_tdstSuperObject *)stParam.uParam.p_stEngObjValue;
    p_stTree = fn_p_stEvalTree(p_SuperObjPerso, p_stTree, &stParam);
    lBit     = stParam.uParam.lValue;

    if ( (hSpo != NULL) && (lBit >= 0) && (lBit <= 31 ) )
    {
        lReturn = (HIE_fn_lGetSuperObjectDrawMask( hSpo ) & (1 << lBit) ) ? 1 : 0;
    }

    p_stValue->uParam.lValue = lReturn;
    p_stValue->eType         = 6;
    return p_stTree;
}

/* 0x0047D710 */
AI_tdstNodeInterpret *__cdecl fn_p_stGetFrameLength(
    HIE_tdstSuperObject *p_SuperObjPerso, AI_tdstNodeInterpret *p_stTree, AI_tdstGetSetParam *p_stValue)
{
    p_stValue->uParam.xValue = AI_FUNC_xFrameLength;
    p_stValue->eType         = 8;
    return p_stTree;
}

/* 0x0040c2a0 */
extern unsigned long __cdecl fn_ulGetNumberOfAlwaysBelongingToMe(void *_hGenerator);
#define AI_M_pfn_fn_ulGetNumberOfAlwaysBelongingToMe fn_ulGetNumberOfAlwaysBelongingToMe
#define AI_M_pfn_fn_lGetSubMapNumber                 ((unsigned long (__cdecl *)(void))fn_lGetSubMapNumber)
/* 0x0040bc80 */
extern unsigned char __cdecl fn_ucIsAnAlwaysModel(void *p_stEngineObject);

extern void *__cdecl fn_p_stAllocateAlways(long otObjectModelType,
    void *p_stFatherSuperObject, void *_hGenerator, unsigned short uwAction,
    void *p_stMatrix);

#include "HIE/HieSpObj.h"

#define M_ObjectGetModelType(p_stObject) ((p_stObject)->h_StandardGame->otObjectModelType)
AI_tdstNodeInterpret *__cdecl fn_p_stMiscFunction(
    HIE_tdstSuperObject *p_SuperObjPerso, AI_tdstNodeInterpret *p_stTree, AI_tdstGetSetParam *p_stValue)
{
	long eFuncId=AI_M_eFuncIdInterpret(p_stTree-1);
	switch(eFuncId)
	{
	case 0x45:
		M_Full_GetSetParam_Integer(p_stValue, GAM_M_GetSubMap(AI_M_pfn_fn_lGetSubMapNumber()));
		return p_stTree;
		break;

  case 0x44:
		M_Full_GetSetParam_Integer(p_stValue, GAM_g_ucLevelGlobalCounter);
    return p_stTree;
    break;

  case 0x43:
    {
      long lNbAlways;

			M_EvalNextParameter(p_stValue);

      lNbAlways = AI_M_pfn_fn_ulGetNumberOfAlwaysBelongingToMe(M_GetSetParam_p_stSupObjValue(p_stValue));

			M_Full_GetSetParam_Integer(p_stValue, lNbAlways);
    }
		return p_stTree;
		break;

	case 0xd6:

		M_EvalNextParameter(p_stValue);

		M_GetSetParam_Type(p_stValue) = E_vt_Module;
		return p_stTree;
		break;

	case 0x42:
		{
			struct tdstEngineObject_ *p_stEngineObject=NULL;
			HIE_tdstSuperObject *p_SuperObjPersoGenerated=NULL;
			MTH3D_tdstVector *p_stVertex;

			M_EvalNextParameter(p_stValue);
			p_stEngineObject=M_GetSetParam_p_stEngineObjValue(p_stValue);

			M_EvalNextParameter(p_stValue);
			p_stVertex = &M_GetSetParam_stVertexValue(p_stValue);

			if (p_stEngineObject!= NULL)
			{
				if (fn_ucIsAnAlwaysModel(p_stEngineObject))
				{
					POS_tdstCompletePosition stMatrix;
					tdObjectType otObjectModelType = M_ObjectGetModelType(p_stEngineObject);

					POS_fn_vSetIdentityMatrix(&stMatrix);
					POS_fn_vSetTranslationVector(&stMatrix,p_stVertex);

					p_SuperObjPersoGenerated = (HIE_tdstSuperObject *)fn_p_stAllocateAlways(otObjectModelType, gp_stDynamicWorld, p_SuperObjPerso, 0, &stMatrix);
				}
			}
      M_Full_GetSetParam_Perso(p_stValue, p_SuperObjPersoGenerated);
		}
		return p_stTree;
		break;

	default:
		return(p_stTree);
		break;
	}
}

#define AI_M_pfn_sav_vEnsureDefaultSlot   GAM_pc_fn_lEnsureDefaultSaveSlot

#define AI_M_pfn_sav_bLevelSaveAll        fn_bLevelSaveGameAll
#define AI_M_pfn_sav_bPlayerSaveAll       fn_bPlayerSaveGameAll
/* 0x00410870 */
extern unsigned char __cdecl fn_bSaveGameInSlot(unsigned long ulSlotId);
#define AI_M_pfn_sav_ucSaveInSlot         fn_bSaveGameInSlot
/* 0x00410f20 */
extern unsigned char __cdecl fn_bLoadGameFromSlot(unsigned long ulSlotId);
#define AI_M_pfn_sav_ucLoadSlot           fn_bLoadGameFromSlot

#define AI_M_pfn_sav_cPostLoadTest        fn_bLoadPlayerSaveGameAfterLoadLevel
/* 0x00449e80 */
extern unsigned char __cdecl SAI_fn_bInitAllWhen(unsigned short eUserFlags);
#define AI_M_pfn_sav_vAfterLoad           SAI_fn_bInitAllWhen
/* 0x00410ff0 */
extern unsigned char __cdecl GAM_pc_fn_ucErasePrepSaveSlot(unsigned long ulSlotId);
#define AI_M_pfn_sav_vErasePrep           GAM_pc_fn_ucErasePrepSaveSlot
/* 0x004083D0 */
extern unsigned long __cdecl GAM_pc_fn_ulRemoveSaveSlotNode(long lSlotIndex);
#define AI_M_pfn_sav_ucEraseSlot          GAM_pc_fn_ulRemoveSaveSlotNode
/* 0x004110E0 */
extern unsigned long __cdecl GAM_pc_fn_ulEraseSaveSlotDir(long lSlotIndex);
#define AI_M_pfn_sav_ucOp5d               GAM_pc_fn_ulEraseSaveSlotDir
/* 0x00407B80 */
extern unsigned char __cdecl fn_bSaveCurrentGameOptions(void);
#define AI_M_pfn_sav_vOp5d2               fn_bSaveCurrentGameOptions

extern long GAM_g_lSaveSlotDirty;        /* 0x500244 */
#define AI_SAV_g_ulCurrentSlot   GAM_g_lSaveSlotDirty

extern unsigned long GAM_gs_ulSaveSlotCount2;   /* 0x4A7068 */
#define AI_SAV_g_ulSlotCount     GAM_gs_ulSaveSlotCount2

/* 0x4A7064 */
extern long  GAM_gs_lNbSaveSlots;
extern void *GAM_gs_pSaveSlotHead;
#define AI_VSN_ulSlotCount   (*(unsigned long *)&GAM_gs_lNbSaveSlots)
#define AI_VSN_p_SlotHead    (*(char **)&GAM_gs_pSaveSlotHead)

extern char GAM_g_szSlotNameWork_storage[4];   /* 0x0049F8D0 */
#define AI_VSN_szNameScratch (GAM_g_szSlotNameWork_storage)

/* 0x0047D730 */
AI_tdstNodeInterpret * __cdecl fn_p_stIsAValidSlotName(HIE_tdstSuperObject *p_SuperObjPerso, AI_tdstNodeInterpret *p_stTree, AI_tdstGetSetParam *p_stValue)
{
    AI_tdstGetSetParam stParam;
    char              *pszString = NULL;
    char               a3_cName[4];
    char               a3_cDummyName[4];

    long               lIndex, lNumberOfSlots;
    long               i;
    long               lResult = 0;
    char              *p_stSlot;                 /* 0x4a706c */

    M_EvalNextParameter(&stParam);
    pszString = (char *)fn_szGetStringFromTextOrStringParam(&stParam);

    M_EvalNextParameter(&stParam);
    lIndex = M_GetSetParam_lValue(&stParam) - 1;

    M_EvalNextParameter(&stParam);

    while( *pszString++ == '/')
    {
         while( *pszString++ != ':' )
         {}
    }

    memcpy(&a3_cName[0],pszString-1,3);
    pszString = &a3_cName[0];

    pszString[3] = '\0';
    a3_cDummyName[3] = '\0';
    lNumberOfSlots = (long)AI_VSN_ulSlotCount;
    strncpy(AI_VSN_szNameScratch, a3_cName, 3);

    p_stSlot = AI_VSN_p_SlotHead;
    for (i=0; i < lNumberOfSlots; i++)
    {
        if ((i != lIndex) &&
            (strncmp(p_stSlot + 8, pszString, 3) == 0))
        {
            lResult = i+1;
        }
        p_stSlot = *(char **)(p_stSlot + 4);
    }

    M_Full_GetSetParam_Integer( p_stValue, lResult );

    return(p_stTree);
}

/* 0x0047D830 */
AI_tdstNodeInterpret *__cdecl fn_p_stSaveGameFunction(
    HIE_tdstSuperObject *p_SuperObjPerso, AI_tdstNodeInterpret *p_stTree, AI_tdstGetSetParam *p_stValue)
{
    long          eFuncId=AI_M_eFuncIdInterpret(p_stTree-1);
    ACP_tdxBool   bReturn = FALSE;

    /* 0x47d830 */
    switch (eFuncId)
    {
    case 0x5a:
    case 0x5b:
    case 0x5c:
    case 0x5d:
        {
            long               lSlotId;

            char              *pszSlotName = NULL;
            long               lNbOfLums = 0;

            AI_tdstGetSetParam stParam;
            M_EvalNextParameter(&stParam);

            lSlotId = M_GetSetParam_lValue(&stParam);

            switch (eFuncId)
            {
            case 0x5a:
                if (lSlotId == 0) { AI_M_pfn_sav_vEnsureDefaultSlot(); lSlotId = 1; }
                if ((bReturn = AI_M_pfn_sav_bLevelSaveAll()) == 0)
                    break;
                if ((bReturn = AI_M_pfn_sav_bPlayerSaveAll()) == 0)
                    break;
                bReturn = AI_M_pfn_sav_ucSaveInSlot(lSlotId);
                AI_SAV_g_ulCurrentSlot = 1;
                break;

            case 0x5b:
                if (lSlotId != 0)
                {
                    bReturn = AI_M_pfn_sav_ucLoadSlot(lSlotId);
                    AI_SAV_g_ulCurrentSlot = lSlotId;
                    if (AI_M_pfn_sav_cPostLoadTest() && bReturn)
                        AI_M_pfn_sav_vAfterLoad(4);
                }
                break;

            case 0x5c:
                if (lSlotId == 0) { AI_M_pfn_sav_vEnsureDefaultSlot(); lSlotId = 1; }
                AI_M_pfn_sav_vErasePrep(lSlotId);
                bReturn = (unsigned char)AI_M_pfn_sav_ucEraseSlot(lSlotId);
                if (lSlotId == AI_SAV_g_ulCurrentSlot)
                    AI_SAV_g_ulCurrentSlot = 0;
                break;

            case 0x5d:
                bReturn = (unsigned char)AI_M_pfn_sav_ucOp5d(lSlotId);
                AI_SAV_g_ulCurrentSlot = lSlotId;
                if (AI_SAV_g_ulSlotCount == 0)
                    AI_SAV_g_ulCurrentSlot = 1;
                AI_M_pfn_sav_vOp5d2();
                AI_SAV_g_ulFlag4b9c10 = 1;
                break;

            default:
                break;
            }
        }
        break;

    case 0x5e:
        bReturn = (unsigned char)AI_SAV_g_ulCurrentSlot;
        break;

    default:
        break;
    }

    M_Full_GetSetParam_Integer( p_stValue, (long)bReturn );
    return (p_stTree);
}

#define AI_M_pfn_POS_fn_vQuatToMat(m, x, y, z, w) \
    POS_fn_vQuatToMat((MTH3D_tdstMatrix *)(m), (x), (y), (z), (w))

#define AI_M_pfn_GMT_fn_b_ulIsValid(h) \
    GMT_fn_b_ulIsValid((GMT_tdxHandleToGameMaterial)(h))
#define AI_M_pfn_GMT_fn_hGetCollideMaterial(h) \
    ((unsigned long)GMT_fn_hGetCollideMaterial((GMT_tdxHandleToGameMaterial)(h)))
#define AI_M_pfn_GMT_fn_hGetCollideMaterialIdentifier(h) \
    ((unsigned short)GMT_fn_hGetCollideMaterialIdentifier((GMT_tdxHandleToCollideMaterial)(h)))

#define AI_M_pfn_fn_pstGetDNMReport fn_pstGetDNMReport
#define AI_GMT_C_ulInvalidHandle        ((unsigned long)0xCDCDCDCD)
#define AI_GMT_C_InvalidCollideMaterial ((unsigned long)0xFFFFFFFF)
#include "AI/GSParam.h"

#include "DNM/DNM_types.h"
AI_tdstNodeInterpret *__cdecl fn_p_stGameMaterialFunction(
    HIE_tdstSuperObject *p_SuperObjPerso, AI_tdstNodeInterpret *p_stTree, AI_tdstGetSetParam *p_stValue)
{
    long                  eFuncId = AI_M_eFuncIdInterpret(p_stTree-1);
    GMT_tdxHandleToGameMaterial hGameMaterial = (GMT_tdxHandleToGameMaterial)AI_GMT_C_ulInvalidHandle;
    void                 *p_stDynamReport;

    HIE_tdstSuperObject  *hNewSuperObjPerso;
    fn_vGetUltraOperatorPerso(
        AI_M_pfn_fn_ucGetFunctionUltraOperator(eFuncId), p_SuperObjPerso, &hNewSuperObjPerso);

    switch (eFuncId)
    {
    case 0xca:
    case 0xcb:
        {
            MS_tdxHandleToBrain hBrain;
            if ((hBrain = M_GetMSHandle(hNewSuperObjPerso,Brain)) != 0)
            {

                hGameMaterial = (GMT_tdxHandleToGameMaterial)hBrain->hLastCollidedGoThroughMaterial;
                if (GMT_fn_b_ulIsValid(hGameMaterial))
                {

                    if (eFuncId == 0xca)
                    {
                        GMT_tdxHandleToCollideMaterial hMaterialCollide = GMT_fn_hGetCollideMaterial(hGameMaterial);
                        GMT_tdxMask xMask = (GMT_tdxMask)0;
                        if (hMaterialCollide != (GMT_tdxHandleToCollideMaterial)AI_GMT_C_InvalidCollideMaterial)
                            xMask = GMT_fn_hGetCollideMaterialIdentifier(hMaterialCollide);

                        M_Full_GetSetParam_Mask(p_stValue,xMask);
                    }
                }
                else
                    M_Full_GetSetParam_Mask(p_stValue,0);
            }
        }
        return p_stTree;
        break;

    case 0xcc:
        if ((p_stDynamReport = AI_M_pfn_fn_pstGetDNMReport(hNewSuperObjPerso)) != NULL)
            hGameMaterial = DNM_M_hObstacleGetCollidedMaterial(DNM_M_p_stReportGetGround((DNM_tdstReport *)p_stDynamReport));
        break;
    default:
        break;
    }

    p_stValue->uParam.ulValue = (unsigned long)hGameMaterial;
    p_stValue->eType          = 0x26;
    return (p_stTree);
}

#include "GAM/STRUCTUR/Engmode.h"

/* 0x0047C350 */
AI_tdstNodeInterpret *__cdecl fn_p_stVectorFunction(HIE_tdstSuperObject *p_SuperObjPerso, AI_tdstNodeInterpret *p_stTree, AI_tdstGetSetParam *p_stValue)
{
    long                  eFuncId = AI_M_eFuncIdInterpret(p_stTree-1);
    MTH3D_tdstVector     *p_stVector = &M_GetSetParam_stVertexValue(p_stValue);

    M_EvalNextParameter(p_stValue);

    switch ( eFuncId )
    {
    case 0xb9:
        {

            MTH3D_tdstVector stRotated = *p_stVector;
            MTH3D_tdstMatrix stMatrix;
            MTH_tdxReal xAngle;

            M_EvalNextParameter(p_stValue);
            xAngle = M_ReturnParam_xValue(p_stValue);
            if ( !MTH_M_bIsNull(xAngle) )
            {

                M_EvalNextParameter(p_stValue);

                xAngle = MTH_M_xMul(MTH_M_xDegToRad(xAngle),MTH_M_xNeg(MTH_C_Inv2));
                MTH3D_M_vMulScalarVector(p_stVector,MTH_M_xSin(xAngle),p_stVector);
                POS_fn_vQuatToMat(&stMatrix,p_stVector->xX,p_stVector->xY,p_stVector->xZ,MTH_M_xCos(xAngle));

                MTH3D_M_vMulMatrixVector(p_stVector, &stMatrix, &stRotated);
            }
            else
            {

                p_stTree = fn_p_stSkipThisArgument(p_stTree);

                M_Full_GetSetParam_p_stVertex(p_stValue, &stRotated);
            }
        }
        return p_stTree;
        break;

    case 0xb6:
    case 0xb5:
        {
            MTH_tdxReal xScalar, xTmp;
            MTH3D_tdstVector stVCurrent = *p_stVector;
            MTH3D_tdstVector stVSecond;

            M_EvalNextParameter(p_stValue);
            xTmp = M_ReturnParam_xValue(p_stValue);

            M_EvalNextParameter(p_stValue);
            stVSecond = *p_stVector;

            if ((xTmp < MTH_C_ZERO) || (xTmp > MTH_C_ONE))
            {

                if (xTmp < MTH_C_ZERO) xTmp = MTH_C_ZERO;
                if (xTmp > MTH_C_ONE) xTmp = MTH_C_ONE;
            }

            if ( (eFuncId == 0xb5  ) || MTH_M_bEqualZero(xTmp) )
                xScalar = xTmp;
            else
            {
                int iNbFrames = MTH_M_xRealToLong(
                    MTH_M_xAdd(
                        MTH_M_xDiv(
                            MTH_M_xLongToReal(M_ulGetUsefulDeltaTimeInMilliSeconds(GAM_g_stEngineStructure.stEngineTimer)),
                            MTH_M_xMul(M_xGetFrameLength(GAM_g_stEngineStructure.stEngineTimer), MTH_M_xLongToReal(1000))
                        ),
                        MTH_C_Inv2
                    )
                );

#if 0
                xScalar = xTmp;
                while (iNbFrames > 1)
                {
                    MTH3D_M_vLinearInterpolVector(p_stVector, p_stVector, &stV1, xScalar);
                    iNbFrames--;
                }
#else
                switch ( iNbFrames )
                {
                case 1:
                    xScalar = xTmp;
                    break;

                case 2:
                    xScalar = MTH_M_xMul(xTmp, xTmp);
                    break;

                case 3:
                    xScalar = MTH_M_xMul(MTH_M_xMul(xTmp, xTmp), xTmp);
                    break;

                case 4:
                    xTmp = MTH_M_xMul(xTmp, xTmp);
                    xScalar = MTH_M_xMul(xTmp, xTmp);
                    break;

                case 5:
                    xScalar = MTH_M_xMul(xTmp, xTmp);
                    xScalar = MTH_M_xMul(MTH_M_xMul(xScalar, xScalar), xTmp);
                    break;

                case 6:
                    xScalar = MTH_M_xMul(MTH_M_xMul(xTmp, xTmp), xTmp);
                    xScalar = MTH_M_xMul(xScalar, xScalar);
                    break;

                default:
                    /* 0x0047C350 */
                    xScalar = MTH_M_xMul(MTH_M_xMul(xTmp, xTmp), xTmp);
                    xScalar = MTH_M_xMul(xScalar, xScalar);
                    break;
                }
#endif
            }

            MTH3D_M_vLinearInterpolVector(p_stVector, &stVSecond, &stVCurrent, xScalar);
        }
        return p_stTree;
        break;

    case 0xb4:
        {
            MTH_tdxReal xScalar;

            MTH3D_tdstVector stProjectedVector = *p_stVector;

            M_EvalNextParameter(p_stValue);

            if ( !MTH3D_M_bIsNullVector(p_stVector) )
            {
                MTH3D_M_vNormalizeVector(p_stVector, p_stVector);

                xScalar = MTH3D_M_xDotProductVector(p_stVector, &stProjectedVector);

                MTH3D_M_vMulScalarVector(p_stVector, xScalar, p_stVector);

            }
            else
                *p_stVector = stProjectedVector;
        }
        return p_stTree;
        break;

    case 0xb7:
        {
            MTH3D_tdstVector stVertex;
            MTH_tdxReal xScalar;

            stVertex = *p_stVector;
            M_EvalNextParameter(p_stValue);
            xScalar = M_ReturnParam_xValue(p_stValue);
            MTH3D_M_vMulScalarVector(&stVertex, xScalar, &stVertex);
            M_Full_GetSetParam_p_stVertex(p_stValue, &stVertex);
        }
        return(p_stTree);
        break;

    case 0xb8:
        M_Full_GetSetParam_Float(p_stValue,MTH3D_M_xNormVector(p_stVector) );
        return(p_stTree);
        break;

#if 0

    case   0x0:
        {

            MTH3D_tdstVector			stVertex, stSource, stDest;
            MTH_tdxReal 				xTrans, yTrans;
            GLD_tdstViewportAttributes	*p_stVpt;
            GLI_tdstCamera				*p_stCam;
            GLI_tdxValue 				xInterm ;

            stVertex.xX = -1;
            stVertex.xY = -1;
            stVertex.xZ = -1;

            p_stVpt = &g_stEngineStructure.stViewportAttr;

            p_stCam=(((GLI_tdstSpecificAttributesFor3D *)p_stVpt->p_vSpecificToXD)->p_stCam);
            POS_fn_vMulMatrixVertex( &stSource, &(p_stCam->stMatrix), p_stVector);

            if ( MTH_M_bGreaterEqual(stSource.xZ,p_stCam->xNear) )
            {
                stDest.xZ = MTH_M_xInv(stSource.xZ) ;
                xInterm = MTH_M_xMul(p_stCam -> xScreen , stDest.xZ );

                xTrans = GLI_M_Mul( GLI_M_LongToValue( g_stEngineStructure.stViewportAttr.dwWidth ), MTH_C_Inv2 );
                yTrans = GLI_M_Mul( GLI_M_LongToValue( g_stEngineStructure.stViewportAttr.dwHeight), MTH_C_Inv2 );

                stDest.xX = MTH_M_xAdd(xTrans, MTH_M_xMul(MTH_M_xMul(stSource.xX,p_stCam -> stScale . xX), xInterm)) ;
                stDest.xY = MTH_M_xAdd(yTrans, MTH_M_xMul(MTH_M_xMul(stSource.xY,p_stCam -> stScale . xY), xInterm)) ;

                stVertex.xX = MTH_M_xMul(stDest.xX, MTH_M_xFloatToReal(1000.0f / (float)g_stEngineStructure.stViewportAttr.dwWidth));
                stVertex.xY = MTH_M_xMul(stDest.xY, MTH_M_xFloatToReal(1000.0f / (float)g_stEngineStructure.stViewportAttr.dwHeight));
                stVertex.xZ = MTH_C_ZERO;

                if (MTH_M_bGreater(stVertex.xX,MTH_M_xFloatToReal(1000)))
                    stVertex.xX = MTH_M_xFloatToReal(-1);

                if (MTH_M_bGreater(stVertex.xY,MTH_M_xFloatToReal(1000)))
                    stVertex.xY = MTH_M_xFloatToReal(-1);
            }

            M_Full_GetSetParam_p_stVertex(p_stValue, &stVertex);
        }

        return(p_stTree);
        break;
#endif
    default:
        break;
    }
    return(p_stTree);
}

#include "AI/DsgMem.h"
#include "AI/Brain.h"

#define AI_PRO_M_vCallModuleStub(ulModule)                                                     \
    {                                                                                          \
        MMG_pc_M_vStepLcg();                                                                   \
        p_ucStub = &MMG_pc_g_uLcgPool.a_ucCode[(int)MMG_g_ulLCGState % MMG_pc_C_PoolSlots];   \
        MMG_pc_M_vWriteStub(p_ucStub, MMG_pc_C_ImpGetModuleHandleA, 0xFF, 0xE0);              \
        MMG_pc_M_vFlushPool(hProcess);                                                         \
        (ulModule) = (unsigned long)((void *(__stdcall *)(const char *))(void *)p_ucStub)(NULL); \
        for (p_ucEnd = p_ucStub + MMG_pc_C_StubSize; p_ucStub < p_ucEnd; p_ucStub++)          \
        {                                                                                      \
            MMG_pc_M_vStepLcg();                                                               \
            *p_ucStub = (unsigned char)((int)MMG_g_ulLCGState % 0xff);                         \
        }                                                                                      \
    }

#ifdef RAYMAN2_NO_PROTECTION
/* Copy protection removed. The AI functions of the original PC protection block
   (PRO_ExecuteVariable, ACT_ComputeProtectKey, MATH_Xor/.../MemoryValue, ACT_UpdateChecksum; the
   console versions of the engine have none of them) mark their calling actor as a protection
   actor: it stops thinking (fn_vMakeCharacterReact), so its checks never run. */
void __cdecl AI_pc_fn_vSetProtectionActor(HIE_tdstSuperObject *p_SuperObjPerso)
{
    MS_tdxHandleToStandardGame hStdGame = M_GetMSHandle(p_SuperObjPerso, StandardGame);

    if (hStdGame != NULL)
    {
        fn_vStandardGameSetCustomBits(hStdGame,
            fn_ulStandardGameGetCustomBits(hStdGame) | GAM_C_CustBitProtection);
    }
}
#endif /* RAYMAN2_NO_PROTECTION */

#ifndef RAYMAN2_NO_PROTECTION   /* PRO_ExecuteVariable: runs code held in a DsgVar array */
/* 0x0047AEB0 */
typedef unsigned long (__cdecl *AI_tdpfnPcExecVar)(unsigned char *p_ucData, unsigned long *p_ulTable);

unsigned long AI_pc_g_pvExecVarResume;   /* 0x00510E18 */
unsigned long AI_pc_g_pvExecVarExit;     /* 0x00510C58 */

void __cdecl MMG_pc_fn_vFillTamperApiTable(unsigned long *_p_ulTable);

#pragma optimize("", off)

AI_tdstNodeInterpret *__cdecl fn_p_ExecuteVariable(
    HIE_tdstSuperObject *p_SuperObjPerso, AI_tdstNodeInterpret *p_stTree, AI_tdstGetSetParam *p_stValue)
{

    AI_tdstMind       *v1;
    long               v2;
    unsigned long      v3 = 0;
    unsigned long      v4;
    unsigned long      v5[16];
    AI_tdpfnPcExecVar  v6;
    unsigned long      v7;
    unsigned char     *v8;

    if (SNA_g_lProtectionGate != 0)
    {
        __asm { mov dword ptr [AI_pc_g_pvExecVarResume], offset L_body }
        __asm { mov dword ptr [AI_pc_g_pvExecVarExit], offset L_epi }
        return (AI_tdstNodeInterpret *)0;
    }
L_body:
    v2 = AI_M_eFuncIdInterpret(p_stTree-1);
    if (v2 == 0x111)
    {
        M_vEvalNextVarId(v1, v4);
        v6 = (AI_tdpfnPcExecVar)((tdstArray *)M_GetDsgVarAddr(v1, v4))->d_ArrayElement;
        if (*(unsigned char *)v6 != 0)
        {
            M_vEvalNextVarId(v1, v7);
            v8 = (unsigned char *)((tdstArray *)M_GetDsgVarAddr(v1, v7))->d_ArrayElement;
            MMG_pc_fn_vFillTamperApiTable(v5);
            M_EvalNextParameter(p_stValue);
            if (M_ReturnParam_lValue(p_stValue) != 0)
            {
                *v8 = (unsigned char)g_cCDROM;
            }
            v3 = v6(v8, v5);
        }
        else
        {
            p_stTree = fn_p_stSkipThisArgument(p_stTree);
            p_stTree = fn_p_stSkipThisArgument(p_stTree);
        }
    }
    M_Full_GetSetParam_Integer(p_stValue, v3);

    __asm { mov eax, p_stTree }
L_epi:
    ;
}

#pragma optimize("", on)
#else
/* Copy protection removed: the code a DsgVar array holds is not executed. The function yields 0,
   as it does when the array holds no code. */
AI_tdstNodeInterpret *__cdecl fn_p_ExecuteVariable(
    HIE_tdstSuperObject *p_SuperObjPerso, AI_tdstNodeInterpret *p_stTree, AI_tdstGetSetParam *p_stValue)
{
    AI_tdstMind       *v1;
    unsigned long      v4;

    AI_pc_fn_vSetProtectionActor(p_SuperObjPerso);
    if (AI_M_eFuncIdInterpret(p_stTree-1) == 0x111)
    {
        M_vEvalNextVarId(v1, v4);
        p_stTree = fn_p_stSkipThisArgument(p_stTree);
        p_stTree = fn_p_stSkipThisArgument(p_stTree);
    }
    M_Full_GetSetParam_Integer(p_stValue, 0);
    return p_stTree;
}
#endif /* RAYMAN2_NO_PROTECTION */

/* 0x0047B0E0 */
AI_tdstNodeInterpret *__cdecl fn_p_stComputeProtectKey(
    HIE_tdstSuperObject *p_SuperObjPerso, AI_tdstNodeInterpret *p_stTree, AI_tdstGetSetParam *p_stValue)
{
    long                  eFuncId = AI_M_eFuncIdInterpret(p_stTree-1);
    HIE_tdstSuperObject  *hNewSuperObjPerso;
    unsigned long         ulKey = 0;
    unsigned long         ulSeed;
    AI_tdstGetSetParam    stParam;

    fn_vGetUltraOperatorPerso(
        AI_M_pfn_fn_ucGetFunctionUltraOperator(eFuncId), p_SuperObjPerso, &hNewSuperObjPerso);
#ifdef RAYMAN2_NO_PROTECTION
    AI_pc_fn_vSetProtectionActor(p_SuperObjPerso);
#endif /* RAYMAN2_NO_PROTECTION */

    M_EvalNextParameter(&stParam);

    if (eFuncId == 0x112)
    {
        ulSeed = (unsigned long)M_GetSetParam_lValue(&stParam) ^ 0x075bd924u;
        ulKey  = ulSeed * 0xbc8fu + (ulSeed / 0xadc8u) * 0x80000001u;
    }

    M_Full_GetSetParam_Integer(p_stValue, ulKey);
    return p_stTree;
}

/* 0x0047B170 */
AI_tdstNodeInterpret *__cdecl fn_p_stComputeXor(
    HIE_tdstSuperObject *p_SuperObjPerso, AI_tdstNodeInterpret *p_stTree, AI_tdstGetSetParam *p_stValue)
{
    long                  eFuncId = AI_M_eFuncIdInterpret(p_stTree-1);
    unsigned long         ulResult = 0;
    unsigned long         ulModule;
    unsigned long         ulValue1;
    unsigned char        *p_ucStub;
    unsigned char        *p_ucEnd;
    void                 *hProcess;
    AI_tdstGetSetParam    stParam1;
    AI_tdstGetSetParam    stParam2;

#ifndef RAYMAN2_NO_PROTECTION
    AI_PRO_M_vCallModuleStub(ulModule);
#else
    AI_pc_fn_vSetProtectionActor(p_SuperObjPerso);
#endif /* RAYMAN2_NO_PROTECTION */

    M_EvalNextParameter(&stParam1);
    ulValue1 = (unsigned long)M_GetSetParam_lValue(&stParam1);
    if (eFuncId == 0x11b)
    {
#ifndef RAYMAN2_NO_PROTECTION   /* MATH_MemoryValue: a dword of the running code image */
        ulResult = *(unsigned long *)(ulModule + ulValue1);
#endif /* RAYMAN2_NO_PROTECTION */
    }
    else
    {
        M_EvalNextParameter(&stParam2);
        switch (eFuncId)
        {
        case 0x113: ulResult = ulValue1 ^ (unsigned long)M_GetSetParam_lValue(&stParam2); break;
        case 0x114: ulResult = ulValue1 & (unsigned long)M_GetSetParam_lValue(&stParam2); break;
        case 0x115: ulResult = ulValue1 | (unsigned long)M_GetSetParam_lValue(&stParam2); break;
        case 0x117: ulResult = ulValue1 / (unsigned long)M_GetSetParam_lValue(&stParam2); break;
        case 0x118: ulResult = ulValue1 * (unsigned long)M_GetSetParam_lValue(&stParam2); break;
        case 0x119: ulResult = ulValue1 + (unsigned long)M_GetSetParam_lValue(&stParam2); break;
        case 0x11a: ulResult = ulValue1 - (unsigned long)M_GetSetParam_lValue(&stParam2); break;
        }
    }

    M_GetSetParam_lValue(p_stValue) = (long)ulResult;
    M_GetSetParam_Type(p_stValue) = E_vt_PositiveInteger;
    return p_stTree;
}

/* 0x0047B340 */
AI_tdstNodeInterpret *__cdecl AI_pc_fn_p_stComputeNotU(
    HIE_tdstSuperObject *p_SuperObjPerso, AI_tdstNodeInterpret *p_stTree, AI_tdstGetSetParam *p_stValue)
{
    unsigned long         ulModule;
    unsigned char        *p_ucStub;
    unsigned char        *p_ucEnd;
    void                 *hProcess;
    AI_tdstGetSetParam    stParam;

#ifndef RAYMAN2_NO_PROTECTION
    AI_PRO_M_vCallModuleStub(ulModule);
#else
    AI_pc_fn_vSetProtectionActor(p_SuperObjPerso);
#endif /* RAYMAN2_NO_PROTECTION */

    M_EvalNextParameter(&stParam);
    M_GetSetParam_lValue(p_stValue) = (long)~(unsigned long)M_GetSetParam_lValue(&stParam);
    M_GetSetParam_Type(p_stValue) = E_vt_PositiveInteger;
    return p_stTree;
}
