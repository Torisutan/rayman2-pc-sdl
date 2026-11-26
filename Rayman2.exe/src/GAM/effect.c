#include "GAM/GAM_types.h"
#include "HIE/HIE_types.h"
#include "POS/POS_types.h"
#include "MTH/MTH3D.h"
#include "GAM/effect.h"

typedef void *MS_tdxHandleToMSPrtSrc;
typedef void *MS_tdxHandleToSectInfo;

/* 0x0042DB30 */
extern void *__cdecl fn_hMSPrtSrcGetFootPath(void *h_MSPrtSrc);
/* 0x0042DE30 */
extern void *__cdecl fn_hMSPrtSrcGetWaterStreakGenerator(void *h_MSPrtSrc);

extern ACP_tdxBool __cdecl fn_bIsCharacterCrossingASurfaceOfWater(HIE_tdstSuperObject *_hCharacter, MTH3D_tdstVector *_p_stVector);
/* 0x0040C3F0 */
void __cdecl fn_vGenerateFoamInStreak(tdstWaterStreakEffect *_hWaterStreak, long _lElapsedTime, MTH3D_tdstVector *_p_stPosition);

void __cdecl fn_v_FootPath_Compute(HIE_tdstSuperObject *_hCharacter, ACP_tdxIndex _xType);

static float            gs_fDeltaZ = 0.01f; /* 0x0049F84C */
static float            gs_fSPZ = 0.3f; /* 0x0049F850 */
static MTH3D_tdstVector gs_stPicking = { 0, 0, -0.6f }; /* 0x0049F858 */
static ACP_tdxIndex     gs_a2a6_xUV[2][6] = { {0,1,3,1,2,3}, {1,0,2,0,3,2} }; /* 0x0049F868 */
static ACP_tdxIndex     gs_a4a2_xChangeSign[4][2] = { { 0, 0}, { 1, 0}, { 0, 1}, { 1,0} }; /* 0x0049F880 */

#include "GEO/GEO_types.h"

/* 0x0040C4B0 */
void __cdecl fn_v_FootPath_ReInit(tdstFootPath *_hFootPath)
{
    ACP_tdxIndex xLevel;
    int xI;

    if (_hFootPath->a8_hGeomObj[0] != 0)
    {
        xLevel = 0;
        if (0 < _hFootPath->xNumberOfAlphaLevels)
        {
            do
            {
                xI = (int)xLevel;
                xLevel = xLevel + 1;
                _hFootPath->a8_xNextFaces[xI] = 0;
                _hFootPath->a8_xNumberOfFaces[xI] = 0;
                ((GEO_tdstElementIndexedTriangles *)(((struct GEO_tdstGeometricObject_ *)_hFootPath->a8_hGeomObj[xI])->d_stListOfElements[0]))->xNbFaces = 0;
            } while (xLevel < _hFootPath->xNumberOfAlphaLevels);
        }
    }
}

/* 0x0040C4F0 */
void __cdecl fn_v_FootPath_Add(void *_hCharacter, ACP_tdxIndex _xFootPrintType)
{
    void *hMSPrtSrc = M_GetMSHandle((HIE_tdstSuperObject *)_hCharacter, MSPrtSrc);

    if (hMSPrtSrc != 0)
    {
        tdstFootPath *hFootPath = (tdstFootPath *)fn_hMSPrtSrcGetFootPath(hMSPrtSrc);
        if (hFootPath->a8_hGeomObj[0] != 0)
            fn_v_FootPath_Compute((HIE_tdstSuperObject *)_hCharacter, _xFootPrintType);
    }
}

/* 0x0040C530 */
void __cdecl fn_v_FootPath_Compute(HIE_tdstSuperObject *_hCharacter, ACP_tdxIndex _xType)
{
    tdstFootPath *hFootPath;
    POS_tdstCompletePosition *hMatrix;
    MTH3D_tdstVector stPos, stI, stJ, stK, stT1, stT2;

    hFootPath = (tdstFootPath *)fn_hMSPrtSrcGetFootPath(M_GetMSHandle(_hCharacter, MSPrtSrc));
    fn_h_SectInfoGetCurrentSector(M_GetMSHandle(_hCharacter, SectInfo));
    hMatrix = HIE_fn_hGetSuperObjectGlobalMatrix(_hCharacter);

    POS_fn_vGetTranslationVector(hMatrix, &stPos);
    POS_fn_vGetRotationMatrix(hMatrix, &stI, &stJ, &stK);
    POS_fn_vRotateVector(&stK, hMatrix, &hFootPath->a2_stOffset[_xType]);
    MTH3D_M_vAddVector(&stPos, &stPos, &stK);
    stPos.xZ = stPos.xZ + gs_fSPZ;

    MTH3D_M_vMulScalarVectorC(&stI, hFootPath->a2_xScale[0], &stI);
    MTH3D_M_vMulScalarVectorC(&stJ, hFootPath->a2_xScale[1], &stJ);
    if (gs_a4a2_xChangeSign[0][0])
        MTH3D_M_vNegVectorC(&stI, &stI);
    if (gs_a4a2_xChangeSign[0][1])
        MTH3D_M_vNegVectorC(&stJ, &stJ);

    MTH3D_M_vAddVector(&stT1, &stI, &stJ);
    MTH3D_M_vAddVector(&stT1, &stT1, &stPos);
    MTH3D_M_vAddVector(&stT2, &stT1, &gs_stPicking);
}

/* 0x0040C3B0 */
void __cdecl fn_vTreatWaterStreak(tdstWaterStreakEffect *_hWaterStreak, long _lElapsedTime)
{
    long lIndex;
    tdstWaterFoam *p_stFoam;

    p_stFoam = _hWaterStreak->a_stFoams;
    for (lIndex = 0; lIndex < _hWaterStreak->lNbFoams; lIndex++, p_stFoam++)
    {
        if (p_stFoam->bFoamIsActive != 0)
        {
            p_stFoam->lLifeTime = p_stFoam->lLifeTime - _lElapsedTime;
            if (p_stFoam->lLifeTime <= 0)
                p_stFoam->bFoamIsActive = 0;
        }
    }
}

/* 0x0040C3F0 */
void __cdecl fn_vGenerateFoamInStreak(tdstWaterStreakEffect *_hWaterStreak, long _lElapsedTime, MTH3D_tdstVector *_p_stPosition)
{
    long lIndex;
    unsigned long ulNow;
    tdstWaterFoam *p_stFoam;

    ulNow = GAM_g_ulAccumulatedTime;
    if ((_hWaterStreak->ulLastTimeOfShift - ulNow) <= (unsigned long)_hWaterStreak->lTimeBetweenTwoShifts)
        return;
    _hWaterStreak->ulLastTimeOfShift = ulNow;

    p_stFoam = _hWaterStreak->a_stFoams;
    for (lIndex = 0; lIndex < _hWaterStreak->lNbFoams; lIndex++, p_stFoam++)
    {
        if (p_stFoam->bFoamIsActive == 0)
        {
            p_stFoam->lLifeTime = _hWaterStreak->lLifeTime;
            p_stFoam->stPosition.xX = _p_stPosition->xX;
            p_stFoam->stPosition.xY = _p_stPosition->xY;
            p_stFoam->stPosition.xZ = _p_stPosition->xZ;
            p_stFoam->bFoamIsActive = 1;
            return;
        }
    }
}

/* 0x00455060 */
ACP_tdxBool __cdecl fn_bIsCharacterCrossingASurfaceOfWater(HIE_tdstSuperObject *_hCharacter, MTH3D_tdstVector *_p_stVector)
{
    return 0;
}

/* 0x0040C450 */
void __cdecl fn_vWaterStreakEffect(void *_p_stViewportAttr, void *_hCharacter, long _lDrawMask)
{
    void *hMSPrtSrc;
    tdstWaterStreakEffect *hWaterStreak;
    long lElapsedTime;
    MTH3D_tdstVector stV;

    hMSPrtSrc = M_GetMSHandle((HIE_tdstSuperObject *)_hCharacter, MSPrtSrc);
    if (hMSPrtSrc != 0)
    {
        hWaterStreak = (tdstWaterStreakEffect *)fn_hMSPrtSrcGetWaterStreakGenerator(hMSPrtSrc);
        if (hWaterStreak == 0)
            return;
        lElapsedTime = GAM_g_ulUsefulDeltaTime;
        if (fn_bIsCharacterCrossingASurfaceOfWater((HIE_tdstSuperObject *)_hCharacter, &stV))
            fn_vGenerateFoamInStreak(hWaterStreak, lElapsedTime, &stV);
        fn_vTreatWaterStreak(hWaterStreak, lElapsedTime);
    }
}
