#include "GAM/MSPrtSrc.h"
#include "SAI/SAI.h"
#include "GAM/GAM_types.h"
#include "ERM/CpaError.h"
#include "GAM/effect.h"
#include "POS/POS_types.h"
#include "HIE/HIE_types.h"

#include "PRT/PRT_types.h"
#include "GAM/Actions/MSPrtSrc.h"
#include "GAM/Actions/SectInfo.h"

extern unsigned char __cdecl fn_bIsInFix(void *p_stObject);
extern void *__cdecl fn_h_SectInfoGetCurrentSector(void *h_SectInfo);
void *__cdecl fn_hMSPrtSrcGetFootPath(MS_tdxHandleToMSPrtSrc h_MSPrtSrc);

#define E_uwGameMiniStructObjectNull        ((unsigned short)0xE)
#define E_uwGameMiniStructAllocAlreadyAlloc ((unsigned short)0xD)
#define E_uwGameMiniStructNull              ((unsigned short)0xF)

#define C_PrtSrc_SAIInitFlags (SAI_eInitWhenPlayerDead|SAI_eInitWhenReinitTheMap|SAI_eInitWhenMapLoaded)

#define PRT_M_vSetGenerationModeParticlesSource( _p_stParticlesSource, _ucGenerationMode )\
  ( (_p_stParticlesSource)->ucGenerationMode = ( _ucGenerationMode ) )

#define PRT_M_vSetGenerationProbabilityParticlesSource( _p_stParticlesSource, _xGenerationProbability )\
  ( (_p_stParticlesSource)->xGenerationProbability = ( _xGenerationProbability ) )

#define PRT_M_vSetNbFramesCrenelParticlesSource( _p_stParticlesSource, _xNbFramesNoGeneration, _xNbFramesGeneration )\
{\
  (_p_stParticlesSource)->xNbFramesNoGeneration = (ACP_tdxIndex) ( _xNbFramesNoGeneration );\
  (_p_stParticlesSource)->xNbFramesTotal = (ACP_tdxIndex) (( _xNbFramesNoGeneration ) + ( _xNbFramesGeneration ));\
  (_p_stParticlesSource)->xFramesCounter = 0;\
}

#define PRT_M_vSetNbParticlesToGenerateModeParticlesSource( _p_stParticlesSource, _ucNbParticlesToGenerateMode )\
  ( (_p_stParticlesSource)->ucNbParticlesToGenerateMode = ( _ucNbParticlesToGenerateMode ) )

#define PRT_M_vSetNbParticlesToGenerateParticlesSource( _p_stParticlesSource, _xNbParticlesToGenerate )\
  ( (_p_stParticlesSource)->xNbParticlesToGenerate = ( _xNbParticlesToGenerate ) )

#define PRT_M_vSetNbMinMaxParticlesSource( _p_stParticlesSource, _xNbMin, _xNbMax )\
{\
  (_p_stParticlesSource)->xNbMin = ( _xNbMin );\
  (_p_stParticlesSource)->xNbMax = ( _xNbMax );\
}

#define PRT_M_vSetLifeModeParticlesSource( _p_stParticlesSource, _ucLifeMode )\
  ( (_p_stParticlesSource)->ucLifeMode = ( _ucLifeMode ) )

#define PRT_M_vSetLifeTimeParticlesSource( _p_stParticlesSource, _xLifeTime )\
  ( (_p_stParticlesSource)->xMinTime = ( _xLifeTime ) )

#define PRT_M_vSetMinMaxLifeTimeParticlesSource( _p_stParticlesSource, _xMinTime, _xMaxTime )\
{\
  (_p_stParticlesSource)->xMinTime = ( _xMinTime );\
  (_p_stParticlesSource)->xMaxTime = ( _xMaxTime );\
}

/* 0x0042DB20 */
void __cdecl fn_p_stMSPrtSrcSetGenerator(MS_tdxHandleToMSPrtSrc h_MSPrtSrc, void *p_stPrtGenerator)
{
    h_MSPrtSrc->p_stParticleGenerator = (tdstParticleGenerator *)p_stPrtGenerator;
}

/* 0x0042DB30 */
void *__cdecl fn_hMSPrtSrcGetFootPath(MS_tdxHandleToMSPrtSrc h_MSPrtSrc)
{
    return h_MSPrtSrc->hFootPath;
}

/* 0x0042DE30 */
void *__cdecl fn_hMSPrtSrcGetWaterStreakGenerator(MS_tdxHandleToMSPrtSrc h_MSPrtSrc)
{
    return h_MSPrtSrc->hWaterStreakGenerator;
}

extern void *__cdecl fnp_vGameMallocInHLM(unsigned int ulSize);

extern void __cdecl fn_vGameFreeInHLM(void *p_vObject);

#define OTI_MapLoaded       1
#define OTI_ReinitTheMap    2
#define OTI_LoadSavedGame   3
#define OTI_PlayerDead      4
#define OTI_AlwaysCreated   5

unsigned char __cdecl fn_bIsGeneratorPointInCorrectEnvironment(void *hActiveSector,
                                                               MTH3D_tdstVector *p_stAbsolutePoint,
                                                               long lEnvironmentMask,
                                                               float *p_xDepthCorrection)
{
    (void)hActiveSector;
    (void)p_stAbsolutePoint;
    (void)lEnvironmentMask;
    (void)p_xDepthCorrection;
    return 0;
}

/* 0x004467E0 */
extern void __cdecl PRT_fn_vSourceGenerateParticle(void *_p_stParticlesSystem, void *_p_stParticlesSource,
                                                   POS_tdstCompletePosition *_p_stSourceMatrix, MTH_tdxReal _xTimeT);
void __cdecl fn_vMSPrtSrcUpdateLinkedEnvironment(HIE_tdstSuperObject *hCharacter, MS_tdxHandleToMSPrtSrc h_MSPrtSrc);

/* 0x00414680 */
unsigned long __cdecl fn_ulMSPrtSrcSizeOf(void)
{
    return sizeof(struct tdstMSPrtSrc_);
}

void *__cdecl fn_h_MSPrtSrcRealAlloc(void);
void __cdecl fn_vMSPrtSrcInitValueSAI(MS_tdxHandleToMSPrtSrc h_MSPrtSrc);

/* 0x00414690 */
void __cdecl fn_vMSPrtSrcAlloc(struct tdstEngineObject_ *p_stObject)
{
    if (p_stObject == NULL)
    {
        ERM_fn_vClearLastError(0);
        ERM_fn_vUpdateLastError(0x30, g_ucGameModuleId, 0,
                                E_uwGameMiniStructObjectNull,
                                -1, 0xff, 0xff, NULL);
    }

    if (p_stObject->h_MSPrtSrc != NULL)
    {
        ERM_fn_vClearLastError(0);
        ERM_fn_vUpdateLastError(0x30, g_ucGameModuleId, 0,
                                E_uwGameMiniStructAllocAlreadyAlloc,
                                -1, 0xff, 0xff, NULL);
    }

    p_stObject->h_MSPrtSrc = fn_h_MSPrtSrcRealAlloc();
    fn_vMSPrtSrcInitValueSAI(p_stObject->h_MSPrtSrc);
}

/* 0x00414710 */
void *__cdecl fn_h_MSPrtSrcRealAlloc(void)
{
    return fnp_vGameMallocInHLM((unsigned int)fn_ulMSPrtSrcSizeOf());
}

void __cdecl fn_vMSPrtSrcFreeInitValueSAI(MS_tdxHandleToMSPrtSrc h_MSPrtSrc);

/* 0x00414720 */
void __cdecl fn_vMSPrtSrcFree(struct tdstEngineObject_ *p_stObject)
{
    if (p_stObject == NULL)
    {
        ERM_fn_vClearLastError(0);
        ERM_fn_vUpdateLastError(0x30, g_ucGameModuleId, 0,
                                E_uwGameMiniStructObjectNull,
                                -1, 0xff, 0xff, NULL);
    }

    if (p_stObject->h_MSPrtSrc == NULL)
    {
        ERM_fn_vClearLastError(0);
        ERM_fn_vUpdateLastError(0x30, g_ucGameModuleId, 0,
                                E_uwGameMiniStructNull,
                                -1, 0xff, 0xff, NULL);
    }

    fn_vMSPrtSrcFreeInitValueSAI(p_stObject->h_MSPrtSrc);
    fn_vGameFreeInHLM(p_stObject->h_MSPrtSrc);
    p_stObject->h_MSPrtSrc = NULL;
}

void __cdecl fn_vMSPrtSrcInitMinMaxPoints(MS_tdxHandleToMSPrtSrc h_MSPrtSrc);
void __cdecl fn_vMSPrtSrcSavedValueSAI(void *h_MSPrtSrc);

/* 0x004147B0 */
void __cdecl fn_vMSPrtSrcInit(struct tdstEngineObject_ *p_stObject, int eObjectInit)
{
    if (p_stObject == NULL)
    {
        ERM_fn_vClearLastError(0);
        ERM_fn_vUpdateLastError(0x30, g_ucGameModuleId, 0,
                                E_uwGameMiniStructObjectNull,
                                -1, 0xff, 0xff, NULL);
    }

    if (p_stObject->h_MSPrtSrc == NULL)
    {
        ERM_fn_vClearLastError(0);
        ERM_fn_vUpdateLastError(0x30, g_ucGameModuleId, 0,
                                E_uwGameMiniStructNull,
                                -1, 0xff, 0xff, NULL);
    }

    switch (eObjectInit)
    {
    case OTI_ReinitTheMap:
        {
            void *hFootPath = fn_hMSPrtSrcGetFootPath(p_stObject->h_MSPrtSrc);
            if (hFootPath != NULL)
                fn_v_FootPath_ReInit((tdstFootPath *)hFootPath);
        }
        break;
    case OTI_MapLoaded:
        if (fn_bIsInFix(p_stObject))
        {

            ((MS_tdxHandleToMSPrtSrc)p_stObject->h_MSPrtSrc)->p_stParticleGenerator = NULL;
            ((MS_tdxHandleToMSPrtSrc)p_stObject->h_MSPrtSrc)->hWaterStreakGenerator = NULL;
            ((MS_tdxHandleToMSPrtSrc)p_stObject->h_MSPrtSrc)->ucGeneratorActivation = 0;
            ((MS_tdxHandleToMSPrtSrc)p_stObject->h_MSPrtSrc)->ucCanGenerate = 0;
        }
        else
        {
            fn_vMSPrtSrcInitMinMaxPoints(p_stObject->h_MSPrtSrc);
            fn_vMSPrtSrcInitValueSAI(p_stObject->h_MSPrtSrc);
            fn_vMSPrtSrcSavedValueSAI(p_stObject->h_MSPrtSrc);
        }
        break;
    default:
        break;
    }
}

/* 0x004148A0 */
void __cdecl fn_vMSPrtSrcDesinit(struct tdstEngineObject_ *p_stObject, int eObjectInit)
{
    if (p_stObject == NULL)
    {
        ERM_fn_vClearLastError(0);
        ERM_fn_vUpdateLastError(0x30, g_ucGameModuleId, 0,
                                E_uwGameMiniStructObjectNull,
                                -1, 0xff, 0xff, NULL);
    }

    if (p_stObject->h_MSPrtSrc == NULL)
    {
        ERM_fn_vClearLastError(0);
        ERM_fn_vUpdateLastError(0x30, g_ucGameModuleId, 0,
                                E_uwGameMiniStructNull,
                                -1, 0xff, 0xff, NULL);
    }

    switch (eObjectInit)
    {
    case OTI_ReinitTheMap:
        break;
    case OTI_MapLoaded:
        break;
    case OTI_PlayerDead:
        break;
    default:
        break;
    }
}

/* 0x00414910 */
void __cdecl fn_vMSPrtSrcCopyClone(struct tdstEngineObject_ * _p_stObjectDest, struct tdstEngineObject_ * _p_stObjectSrc)
{
    memcpy(_p_stObjectDest->h_MSPrtSrc, _p_stObjectSrc->h_MSPrtSrc, fn_ulMSPrtSrcSizeOf());
}

/* 0x00414940 */
void __cdecl fn_vMSPrtSrcInitValueSAI(MS_tdxHandleToMSPrtSrc h_MSPrtSrc)
{

    if( h_MSPrtSrc->p_stParticleGenerator!=NULL )
        SAI_fn_bInsertPointerInTable((void*)&h_MSPrtSrc->p_stParticleGenerator, C_PrtSrc_SAIInitFlags);
    SAI_fn_bInsertCharInTable((void *)&h_MSPrtSrc->ucGeneratorActivation, C_PrtSrc_SAIInitFlags);
}

/* 0x00414970 */
void __cdecl fn_vMSPrtSrcFreeInitValueSAI(MS_tdxHandleToMSPrtSrc h_MSPrtSrc)
{

    SAI_fn_bRemoveValueFromTable((void*)&h_MSPrtSrc->p_stParticleGenerator, C_PrtSrc_SAIInitFlags);
    SAI_fn_bRemoveValueFromTable((void *)&h_MSPrtSrc->ucGeneratorActivation, C_PrtSrc_SAIInitFlags);
}

/* 0x00414990 */
void __cdecl fn_vMSPrtSrcSetGeneratorActivation(MS_tdxHandleToMSPrtSrc h_MSPrtSrc, unsigned char ucActivation)
{
    h_MSPrtSrc->ucGeneratorActivation = ucActivation;
}

/* 0x004149A0 */
void __cdecl fn_vMSPrtSrcSourceGenerateParticles( HIE_tdstSuperObject *_hCharacter)
{
	MS_tdxHandleToMSPrtSrc hMSPrtSrc;

   	MTH_tdxReal xTime;

	hMSPrtSrc = M_GetMSHandle(_hCharacter,MSPrtSrc);
	if( !hMSPrtSrc )
		return;
	if( !hMSPrtSrc->p_stParticleGenerator )
		return;
	fn_vMSPrtSrcUpdateLinkedEnvironment(_hCharacter,hMSPrtSrc);

	if( hMSPrtSrc->ucGeneratorActivation!=0 )
	{
		if( hMSPrtSrc->ucCanGenerate )
		{

			xTime = MTH_M_xMul(MTH_M_xLongToReal(GAM_g_stEngineStructure.stEngineTimer.ulCurrentTimerCount), 0.001f);

			PRT_fn_vSourceGenerateParticle
				(
					hMSPrtSrc->p_stParticleGenerator->p_stPrtSys,
					hMSPrtSrc->p_stParticleGenerator->p_stPrtSrc,
					HIE_fn_hGetSuperObjectGlobalMatrix(_hCharacter),
					xTime
				);
		}
	}
}

/* 0x00414A10 */
void __cdecl fn_vMSPrtSrcInitMinMaxPoints(MS_tdxHandleToMSPrtSrc h_MSPrtSrc)
{
    tdstParticleGenerator * p_stPrtGen;

    p_stPrtGen = h_MSPrtSrc->p_stParticleGenerator;
    if( !p_stPrtGen )
        return;
    if( !p_stPrtGen->ucLinkedToEnvironment )
        return;
    if( !p_stPrtGen->p_stPrtEnv )
        return;
    if( p_stPrtGen->p_stPrtEnv->ulEnvType == PRT_C_ulAlignedBoxEnv )
    {

        MTH3D_M_vCopyVector( &p_stPrtGen->stMinPoint, &p_stPrtGen->p_stPrtEnv->stMinPoint);
        MTH3D_M_vCopyVector( &p_stPrtGen->stMaxPoint, &p_stPrtGen->p_stPrtEnv->stMaxPoint);
    }
    else
    {

        MTH3D_M_vCopyVector( &p_stPrtGen->stMinPoint, &p_stPrtGen->p_stPrtEnv->stSphereCenter);
    }
}

unsigned char __cdecl fn_bIsGeneratorActiveInThisEnvironment(HIE_tdstSuperObject *hCharacterSource, tdstParticleGenerator *p_stGenerator);

/* 0x00414A70 */
void __cdecl fn_vMSPrtSrcUpdateLinkedEnvironment(HIE_tdstSuperObject *hCharacter, MS_tdxHandleToMSPrtSrc h_MSPrtSrc)
{
    MTH3D_tdstVector stMinPoint, stMaxPoint;
    tdstParticleGenerator *p_stPrtGen;
    MTH_tdxReal      xDepth;

    p_stPrtGen = h_MSPrtSrc->p_stParticleGenerator;
    h_MSPrtSrc->ucCanGenerate = 1;
    while (p_stPrtGen)
    {
        if (!p_stPrtGen->ucLinkedToEnvironment)
            return;
        if (!p_stPrtGen->p_stPrtEnv)
            return;
        if (!fn_bIsGeneratorActiveInThisEnvironment(hCharacter, p_stPrtGen))
        {
            h_MSPrtSrc->ucCanGenerate = 0;

        }
        else
        {
            if (p_stPrtGen->p_stPrtEnv->ulEnvType == PRT_C_ulAlignedBoxEnv)
            {

                POS_fn_vGetTranslationVector(HIE_fn_hGetSuperObjectGlobalMatrix(hCharacter), &stMinPoint);
                MTH3D_M_vAddVector(&stMaxPoint, &stMinPoint, &p_stPrtGen->stMaxPoint);
                MTH3D_M_vAddVector(&stMinPoint, &stMinPoint, &p_stPrtGen->stMinPoint);
                if (!fn_bIsGeneratorPointInCorrectEnvironment(
                        fn_h_SectInfoGetCurrentSector(M_GetMSHandle(hCharacter,SectInfo)),
                        &stMinPoint, p_stPrtGen->lEnvironmentMask, &xDepth))
                {
                    stMinPoint.xZ += xDepth;
                }
                if (!fn_bIsGeneratorPointInCorrectEnvironment(
                        fn_h_SectInfoGetCurrentSector(M_GetMSHandle(hCharacter,SectInfo)),
                        &stMaxPoint, p_stPrtGen->lEnvironmentMask, &xDepth))
                {
                    h_MSPrtSrc->ucCanGenerate = 0;

                }
                else
                {
                    if (stMinPoint.xZ >= stMaxPoint.xZ)
                    {
                        h_MSPrtSrc->ucCanGenerate = 0;

                    }
                    else
                    {

                        PRT_M_vSetMinMaxPointParticlesEnvironment(p_stPrtGen->p_stPrtEnv, &stMinPoint, &stMaxPoint);
                    }
                }
            }
            else
            {

                POS_fn_vMulMatrixVertex(&p_stPrtGen->p_stPrtEnv->stSphereCenter,
                                        HIE_fn_hGetSuperObjectGlobalMatrix(hCharacter),
                                        &p_stPrtGen->stMinPoint);
            }
        }
        p_stPrtGen = p_stPrtGen->p_stLinkedPrtGen;
    }
}

/* 0x00414C20 */
unsigned char __cdecl fn_bIsGeneratorActiveInThisEnvironment(HIE_tdstSuperObject *hCharacterSource, tdstParticleGenerator *p_stGenerator)
{
    POS_tdstCompletePosition *hSourceMatrix;
    void *hActiveSector;
    MTH3D_tdstVector stTmpPoint;
    float xDepth;

    if( !p_stGenerator->ucLinkedToEnvironment )
        return 1;
    hSourceMatrix = HIE_fn_hGetSuperObjectGlobalMatrix(hCharacterSource);
    hActiveSector = fn_h_SectInfoGetCurrentSector(M_GetMSHandle(hCharacterSource,SectInfo));
    POS_fn_vMulMatrixVertex(&stTmpPoint, hSourceMatrix, &p_stGenerator->p_stPrtSrc->stVertex1);
    if (!fn_bIsGeneratorPointInCorrectEnvironment(hActiveSector, &stTmpPoint, p_stGenerator->lEnvironmentMask, &xDepth))
        return 0;
    POS_fn_vMulMatrixVertex(&stTmpPoint, hSourceMatrix, &p_stGenerator->p_stPrtSrc->stVertex2);
    if (!fn_bIsGeneratorPointInCorrectEnvironment(hActiveSector, &stTmpPoint, p_stGenerator->lEnvironmentMask, &xDepth))
        return 0;
    POS_fn_vMulMatrixVertex(&stTmpPoint, hSourceMatrix, &p_stGenerator->p_stPrtSrc->stVertex3);
    if (!fn_bIsGeneratorPointInCorrectEnvironment(hActiveSector, &stTmpPoint, p_stGenerator->lEnvironmentMask, &xDepth))
        return 0;
    return 1;
}

/* 0x00414D00 */
void __cdecl fn_vMSPrtSrcSetGenerationModeNone(MS_tdxHandleToMSPrtSrc h_MSPrtSrc)
{
    if( !h_MSPrtSrc )
        return;
    if( !h_MSPrtSrc->p_stParticleGenerator )
        return;
    PRT_M_vSetGenerationModeParticlesSource
        (
            h_MSPrtSrc->p_stParticleGenerator->p_stPrtSrc,
            PRT_C_ucNoGeneration
        );
}

/* 0x00414D20 */
void __cdecl fn_vMSPrtSrcSetGenerationModeContinuous(MS_tdxHandleToMSPrtSrc h_MSPrtSrc)
{
    if( !h_MSPrtSrc )
        return;
    if( !h_MSPrtSrc->p_stParticleGenerator )
        return;
    PRT_M_vSetGenerationModeParticlesSource
        (
            h_MSPrtSrc->p_stParticleGenerator->p_stPrtSrc,
            PRT_C_ucContinousGeneration
        );
}

/* 0x00414D40 */
void __cdecl fn_vMSPrtSrcSetGenerationModeCrenel(MS_tdxHandleToMSPrtSrc h_MSPrtSrc, short xNb1, short xNb2)
{
    if( !h_MSPrtSrc )
        return;
    if( !h_MSPrtSrc->p_stParticleGenerator )
        return;
    PRT_M_vSetGenerationModeParticlesSource
        (
            h_MSPrtSrc->p_stParticleGenerator->p_stPrtSrc,
            PRT_C_ucCrenelGeneration
        );

    PRT_M_vSetNbFramesCrenelParticlesSource
        (
            h_MSPrtSrc->p_stParticleGenerator->p_stPrtSrc,
            xNb1,
            xNb2
        );
}

/* 0x00414D80 */
void __cdecl fn_vMSPrtSrcSetGenerationModeProbability(MS_tdxHandleToMSPrtSrc h_MSPrtSrc, float xProbability)
{
    if( !h_MSPrtSrc )
        return;
    if( !h_MSPrtSrc->p_stParticleGenerator )
        return;
    PRT_M_vSetGenerationModeParticlesSource
        (
            h_MSPrtSrc->p_stParticleGenerator->p_stPrtSrc,
            PRT_C_ucProbabilistGeneration
        );
    PRT_M_vSetGenerationProbabilityParticlesSource
        (
            h_MSPrtSrc->p_stParticleGenerator->p_stPrtSrc,
            xProbability
        );
}

/* 0x00414DA0 */
void __cdecl fn_vMSPrtSrcSetGenerationNumberConstant(MS_tdxHandleToMSPrtSrc h_MSPrtSrc, short xNumber)
{
    if( !h_MSPrtSrc )
        return;
    if( !h_MSPrtSrc->p_stParticleGenerator )
        return;
    PRT_M_vSetNbParticlesToGenerateModeParticlesSource
        (
            h_MSPrtSrc->p_stParticleGenerator->p_stPrtSrc,
            PRT_C_ucConstantNb
        );
    PRT_M_vSetNbParticlesToGenerateParticlesSource
        (
            h_MSPrtSrc->p_stParticleGenerator->p_stPrtSrc,
            (unsigned char)xNumber
        );
}

/* 0x00414DD0 */
void __cdecl fn_vMSPrtSrcSetGenerationNumberProbabilist(MS_tdxHandleToMSPrtSrc h_MSPrtSrc, short xMin, short xMax)
{
    if( !h_MSPrtSrc )
        return;
    if( !h_MSPrtSrc->p_stParticleGenerator )
        return;
    PRT_M_vSetNbParticlesToGenerateModeParticlesSource
        (
            h_MSPrtSrc->p_stParticleGenerator->p_stPrtSrc,
            PRT_C_ucProbabilistNb
        );
    PRT_M_vSetNbMinMaxParticlesSource
        (
            h_MSPrtSrc->p_stParticleGenerator->p_stPrtSrc,
            xMin, xMax
        );
}

/* 0x00414E00 */
void __cdecl fn_vMSPrtSrcSetParticlesInfiniteLifeTime(MS_tdxHandleToMSPrtSrc h_MSPrtSrc)
{
    if( !h_MSPrtSrc )
        return;
    if( !h_MSPrtSrc->p_stParticleGenerator )
        return;
    PRT_M_vSetLifeModeParticlesSource
        (
            h_MSPrtSrc->p_stParticleGenerator->p_stPrtSrc,
            PRT_C_ucNoLifeTime
        );
}

/* 0x00414E20 */
void __cdecl fn_vMSPrtSrcSetParticlesConstantLifeTime(MS_tdxHandleToMSPrtSrc h_MSPrtSrc, float xLifeTime)
{
    if( !h_MSPrtSrc )
        return;
    if( !h_MSPrtSrc->p_stParticleGenerator )
        return;
    PRT_M_vSetLifeModeParticlesSource
        (
            h_MSPrtSrc->p_stParticleGenerator->p_stPrtSrc,
            PRT_C_ucConstantLifeTime
        );
    PRT_M_vSetLifeTimeParticlesSource
        (
            h_MSPrtSrc->p_stParticleGenerator->p_stPrtSrc,
            xLifeTime
        );
}

/* 0x00414E40 */
void __cdecl fn_vMSPrtSrcSetParticlesProbabilistLifeTime(MS_tdxHandleToMSPrtSrc h_MSPrtSrc, float xMin, float xMax)
{
    if( !h_MSPrtSrc )
        return;
    if( !h_MSPrtSrc->p_stParticleGenerator )
        return;
    PRT_M_vSetLifeModeParticlesSource
        (
            h_MSPrtSrc->p_stParticleGenerator->p_stPrtSrc,
            PRT_C_ucProbabilistLifeTime
        );
    PRT_M_vSetMinMaxLifeTimeParticlesSource
        (
            h_MSPrtSrc->p_stParticleGenerator->p_stPrtSrc,
            xMin,
            xMax
        );
}

void __cdecl fn_vMSPrtSrcSavedValueSAI(void *h_MSPrtSrc)
{
    (void)h_MSPrtSrc;
}
