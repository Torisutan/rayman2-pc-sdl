#include "GAM/GAM_types.h"
#include "HIE/HIE_types.h"
#include "GAM/GAM_globals.h"
#include "GAM/GAM_pc.h"
#include "ERM/CpaError.h"
#include "TMR/TMR.h"
#include "GLI/Camera.h"

/* 0x004228D0 */
struct GLI_tdstCamera_;
extern void GLI_xAdjustCameraToViewport(short hDevice, short hViewport, struct GLI_tdstCamera_ *p_stCam);

#define BAS_M_GLI_xGetViewportCamera(hDev, hVpt, pp_stCam) \
    (((void (__cdecl *)(short, short, void **))GLI_xGetViewportCamera)((hDev), (hVpt), (pp_stCam)))

#include <stdlib.h>
#include <string.h>
#include "GAM/GAM_pc_DrawLock.h"
#include "AI/CAM_Tool.h"

extern unsigned long __cdecl fn_ulTimerTickPerSecond(short sChannel);
extern long __cdecl TMR_fn_wInitLibrary(void);

void fn_vReinitEngineClock(void)
{
}

/* 0x0049F9EC */
void (*GAM_pc_g_pfn_ActualizeClockHook)(void) = GAM_pc_fn_vRewindEngineClock;

extern short __stdcall TMR_fn_wStopTimer(short wTimerHandle);

typedef struct GAM_tdstBasicSaveSlot_
{
    long                             lSlotId;
    struct GAM_tdstBasicSaveSlot_   *pNext;
    char                             szNameScore[0x18];
} GAM_tdstBasicSaveSlot;

#include "GAM/PlayAnim.h"

extern unsigned long AI_SAV_g_ulFlag4b9c10;
extern void __cdecl AI_pc_fn_vReinitAIInDynamicWorlds(void);
extern unsigned int __cdecl GAM_pc_fn_ulFindMapLinkPos(unsigned int ulSubMap, unsigned int ulEntry);
extern unsigned char GAM_pc_g_ucMode6SaveFlag_storage;
extern HIE_tdstSuperObject *g_hCharacterLauchingSoundEvents;
#define GAM_M_h_StdCamCaracter (GAM_g_stEngineStructure.h_StdCamCaracter)
extern void __cdecl fn_vInitOneObject(struct tdstEngineObject_ *p_stObject, int eObjIniType);
extern void __cdecl MEC_fn_vImposeActorMatrix(HIE_tdstSuperObject *hSupObj);
extern void __cdecl CAM_fn_vUpdateTargetPosition(void *hCineinfo);
extern void __cdecl fn_vInitAveragePositionDatasIfNecessary(void *hCineinfo);
extern void __cdecl CAM_fn_vForceCameraPos(HIE_tdstSuperObject *hCamera, void *p_stPosition);
extern void __cdecl CAM_fn_vForceCameraTgt(HIE_tdstSuperObject *hCamera, void *p_stPosition, void *p_stRefAxisZ);
extern void *__cdecl SAI_fn_p_vGetTableValuePointerOf(void *p_vValue, int eFlags);
extern void __cdecl fn_vInitObjectsWhenPlayerChangeSector(long lUnused, void *p_pFirst, void *p_pSecond);
extern void __cdecl GAM_pc_fn_vStopMatchingSounds(long lKey, void *p_pObject);

void fn_vChangeEngineMode(GAM_tdeEngineMode ucMode)
{

    if (GAM_g_stEngineStructure.eEngineMode == E_EM_ModeInvalid)
    {

        ERM_fn_vUpdateLastError(0x30, g_ucGameModuleId, 0, 0x31, -1, 0xff, 0xff, NULL);
    }
    if (ucMode == E_EM_ModeInvalid || ucMode >= E_EM_NbTotalEngineMode)
    {

        ERM_fn_vClearLastError(0);
        ERM_fn_vUpdateLastError(0x30, g_ucGameModuleId, 0, 0x13, -1, 0xff, 0xff, NULL);
    }
    GAM_g_stEngineStructure.eEngineMode = ucMode;
}

GAM_tdeEngineMode GAM_fn_ucGetEngineMode(void)
{
    return GAM_g_stEngineStructure.eEngineMode;
}

/* 0x00404C70 */
long __cdecl fn_lAToI(const char *_szString)
{
    if (!(*_szString >= '0' || *_szString <= '9' ||
          *_szString == '+' || *_szString == '-'))
    {
        ERM_fn_vClearLastError(0);
        ERM_fn_vUpdateLastError(0x30, g_ucGameModuleId, 0, 0x18, -1, 0xff, 0xff, NULL);
    }
    return atoi(_szString);
}

void fn_vSetFirstLevelName(char *_szName)
{
    strcpy(GAM_g_stEngineStructure.szFirstLevelName, _szName);
}

char *fn_p_szGetFirstLevelName(void)
{
    return GAM_g_stEngineStructure.szFirstLevelName;
}

void fn_vSetLevelName(char *_szName)
{
    char *p_cDelimiter;

    strcpy(GAM_g_stEngineStructure.szLevelName, _szName);
    p_cDelimiter = strchr(GAM_g_stEngineStructure.szLevelName, '$');
    if (p_cDelimiter == NULL)
    {
        GAM_g_stEngineStructure.lSubMapNumber = 0;
    }
    else
    {
        long lSubMap, lEntry;

        *p_cDelimiter++ = 0;
        /* 0x00404D00 */
        if (GAM_g_lSubMapNumber == 0)
            lSubMap = 0;
        else
            lSubMap = fn_lAToI(p_cDelimiter);

        p_cDelimiter = strchr(p_cDelimiter, '$');
        if (p_cDelimiter == NULL)
            lEntry = 0;
        else
            lEntry = fn_lAToI(++p_cDelimiter);

        GAM_g_stEngineStructure.lSubMapNumber = GAM_M_MakeSubMapNumber(lSubMap, (unsigned short)lEntry);
    }
}

char *fn_p_szGetLevelName(void)
{
    return GAM_g_stEngineStructure.szLevelName;
}

long fn_lGetSubMapNumber(void)
{
    return GAM_g_lSubMapNumber;
}

void fn_vSetNextLevelName(char *_szName)
{
    strcpy(GAM_g_stEngineStructure.szNextLevelName, _szName);
}

char *fn_p_szGetNextLevelName(void)
{
    return GAM_g_stEngineStructure.szNextLevelName;
}

void fn_vFreeNextLevelName(void)
{
    GAM_g_stEngineStructure.szNextLevelName[0] = 0;
}

unsigned char fn_bIsNextLevelNameValid(void)
{
    if (GAM_g_stEngineStructure.szNextLevelName[0] == 0)
        return 0;
    else
        return 1;
}

unsigned char GAM_fn_bIsSlotValid(short _wSlotdId)
{
    unsigned char          ucReturn = TRUE;
    GAM_tdstBasicSaveSlot *p_stSlot;
    long                   lSlotId = _wSlotdId;
    long                   lIdx;

    p_stSlot = (GAM_tdstBasicSaveSlot *)GAM_gs_pSaveSlotHead;
    if (fn_lGetNbAvailableSlots() < lSlotId)
        return FALSE;

    if (lSlotId != 1)
    {
        lIdx = lSlotId - 1;
        do
        {
            p_stSlot = p_stSlot->pNext;
        } while (--lIdx != 0);
    }

    if (  strlen(p_stSlot->szNameScore) == 0
        || p_stSlot->szNameScore[0] == ' ')
        ucReturn = FALSE;

    return(ucReturn);
}

/* 0x00405010 */
void fn_vChangeViewportRatio(unsigned long ulMode, MTH_tdxReal xRatio)
{
    struct GLI_tdstCamera_ *p_stCam;

    /* 0x5004CC */
    GAM_pc_M_ulWaitDrawSemaphore((HANDLE)GAM_g_stEngineStructure.hDrawSem);

    /* 0x00405010 */
    if (ulMode == 1)
    {
        BAS_M_GLI_xGetViewportCamera(GAM_g_stEngineStructure.hGLDDevice,
                                     GAM_g_stEngineStructure.hGLDViewport, (void **)&p_stCam);
        if (p_stCam != NULL)
        {
            GLI_xSetCameraRatio(p_stCam, xRatio);
            GLI_xAdjustCameraToViewport(GAM_g_stEngineStructure.hGLDDevice, GAM_g_stEngineStructure.hGLDViewport, p_stCam);
        }
    }
    else
    {
        BAS_M_GLI_xGetViewportCamera(GAM_g_stEngineStructure.hGLDDevice,
                                     GAM_g_stEngineStructure.hGLDFixViewport, (void **)&p_stCam);
        if (p_stCam != NULL)
        {
            GLI_xSetCameraRatio(p_stCam, xRatio);
            GLI_xAdjustCameraToViewport(GAM_g_stEngineStructure.hGLDDevice, GAM_g_stEngineStructure.hGLDFixViewport, p_stCam);
        }
    }

    GAM_pc_M_bReleaseDrawSemaphore((HANDLE)GAM_g_stEngineStructure.hDrawSem);
}
void __cdecl fn_vCreateFixViewPort(short _hDevice);
void fn_vCreateGameViewPortCamera(void);
void GAM_fn_vActualizeAllGameViewportSize(void)
{

    struct GLI_tdstCamera_ *p_stCam;

    ((void (__cdecl *)(short, short, void **))GLI_xGetViewportCamera)(
        GAM_g_stEngineStructure.hGLDDevice, GAM_g_stEngineStructure.hGLDViewport, (void **)&p_stCam);
    if (p_stCam)
        GLI_xAdjustCameraToViewport(GAM_g_stEngineStructure.hGLDDevice, GAM_g_stEngineStructure.hGLDViewport, p_stCam);

    ((void (__cdecl *)(short, short, void **))GLI_xGetViewportCamera)(
        GAM_g_stEngineStructure.hGLDDevice, GAM_g_stEngineStructure.hGLDFixViewport, (void **)&p_stCam);
    if (p_stCam)
        GLI_xAdjustCameraToViewport(GAM_g_stEngineStructure.hGLDDevice, GAM_g_stEngineStructure.hGLDFixViewport, p_stCam);
}

void __cdecl GAM_fn_vCreateGameDeviceAndViewPorts(void)
{
    GAM_g_stEngineStructure.p_stFixCamera=NULL;
    fn_vCreateFixViewPort(GAM_g_stEngineStructure.hGLDDevice);
    fn_vCreateGameViewPortCamera();
}

void fn_vReputGameViewportCamera(void)
{
	GLI_xSetViewportCamera(GAM_g_stEngineStructure.hGLDDevice,GAM_g_stEngineStructure.hGLDViewport,(long)GAM_g_stEngineStructure.p_stGameViewportCamera);
}

/* 0x00405480 */
void fn_vCreateGameViewPortCamera(void)
{
	GLI_xCreateCamera ((GLI_tdstCamera **)&GAM_g_stEngineStructure.p_stGameViewportCamera,GLI_C_lPersCamWithoutDistorsion);
	GLI_xSetCameraAspectAndRatio (GAM_g_stEngineStructure.p_stGameViewportCamera, MTH_M_xConstFloatToReal(1.5f), MTH_M_xConstFloatToReal(0.75f));
	GLI_xSetViewportCamera( GAM_g_stEngineStructure.hGLDDevice,GAM_g_stEngineStructure.hGLDViewport,(long)GAM_g_stEngineStructure.p_stGameViewportCamera);
}

/* 0x004054D0 */
void __cdecl GAM_fn_vAskToChangeLevel(char *szNewLevelName, ACP_tdxBool _bAskForAutosave)
{
    char szLevelPart[80];
    char *p_szSep;
    char *p_szSepEntry;
    unsigned int ulSubMap;
    unsigned int ulEntry;
    void *p_pvFound;
    HIE_tdstSuperObject *p_stSO;
    struct tdstEngineObject_ *p_stEngine;
    HIE_tdstSuperObject *p_stCamera;
    struct tdstEngineObject_ *p_stCameraEngine;
    HIE_tdstSuperObject *p_stWorld;
    void *h_OldSector;
    void *h_NewSector;
    void *p_pvNew;
    float aVector[6];

    if (AI_SAV_g_ulFlag4b9c10 != 0)
    {
        AI_pc_fn_vReinitAIInDynamicWorlds();
        AI_SAV_g_ulFlag4b9c10 = 0;
    }

    strcpy(szLevelPart, szNewLevelName);
    p_szSep = strchr(szLevelPart, '$');
    if (p_szSep == NULL)
    {

        GAM_pc_g_ucMode6SaveFlag_storage = _bAskForAutosave;
        goto LAB_SetNextLevel;
    }

    *p_szSep++ = '\0';
    if (_stricmp(szLevelPart, fn_p_szGetLevelName()) != 0)
    {
        GAM_pc_g_ucMode6SaveFlag_storage = _bAskForAutosave;
        goto LAB_SetNextLevel;
    }
    {
        {

            ulSubMap = (unsigned int)fn_lAToI(p_szSep);
            if (ulSubMap != ((unsigned long)fn_lGetSubMapNumber() >> 0x10))
            {
                p_szSepEntry = strchr(p_szSep + 1, '$');
                ulEntry = 0;
                if (p_szSepEntry != NULL)
                    ulEntry = (unsigned int)fn_lAToI(p_szSepEntry + 1);
                p_pvFound = (void *)GAM_pc_fn_ulFindMapLinkPos(ulSubMap, ulEntry);
                if (p_pvFound != NULL)
                {
                    p_stSO = GAM_pc_g_hMainCharacterSO;
                    p_stEngine = (struct tdstEngineObject_ *)p_stSO->hLinkedObject.p_Void;
                    h_OldSector = *(void **)p_stEngine->h_SectInfo;
                    *(POS_tdstCompletePosition *)p_stSO->hGlobalMatrix = *(POS_tdstCompletePosition *)p_pvFound;

                    p_stWorld = gp_stDynamicWorld;
                    if (p_stSO->p_stFather != p_stWorld)
                    {

                        if (p_stEngine->h_StandardGame->ucPlatFormType == 0)
                            HIE_fn_vSuperObjectAddTail(p_stWorld, p_stSO);
                        else
                            HIE_fn_vSuperObjectAddHead(p_stWorld, p_stSO);
                    }

                    p_pvNew = SAI_fn_p_vGetTableValuePointerOf((void *)&((MS_tdxHandleTo3dData)p_stEngine->h_3dData)->stGLIObjectMatrix, 0x32);
                    *(POS_tdstCompletePosition *)p_pvNew = *(POS_tdstCompletePosition *)p_stSO->hGlobalMatrix;

                    fn_vInitOneObject(p_stEngine, 3);
                    HIE_fn_vComputeNewRelativeMatrix(p_stSO);
                    MEC_fn_vImposeActorMatrix(p_stSO);

                    p_stCamera = GAM_M_h_StdCamCaracter;
                    p_stCameraEngine = (struct tdstEngineObject_ *)p_stCamera->hLinkedObject.p_Void;
                    CAM_fn_vUpdateTargetPosition(p_stCameraEngine->h_Cineinfo);
                    fn_vInitAveragePositionDatasIfNecessary(p_stCameraEngine->h_Cineinfo);

                    aVector[0] = 0.0f;
                    aVector[1] = 2.0f;
                    aVector[2] = 2.0f;
                    POS_fn_vMulMatrixVertex((MTH3D_tdstVector *)aVector, (POS_tdstCompletePosition *)p_stSO->hGlobalMatrix, (MTH3D_tdstVector *)aVector);
                    CAM_fn_vForceCameraPos(p_stCamera, aVector);
                    aVector[3] = 0.0f;
                    aVector[4] = 0.0f;
                    aVector[5] = 1.0f;
                    CAM_fn_vForceCameraTgt(p_stCamera, (void *)&((POS_tdstCompletePosition *)p_stSO->hGlobalMatrix)->stTranslationVector, &aVector[3]);
                    CAM_fn_vForceBestPosition(p_stCamera);

                    h_NewSector = *(void **)p_stEngine->h_SectInfo;
                    if (h_NewSector != h_OldSector)
                    {
                        fn_vInitObjectsWhenPlayerChangeSector((long)p_stSO, h_NewSector, h_OldSector);
                        if (p_stSO == g_hCharacterLauchingSoundEvents)
                            GAM_pc_fn_vStopMatchingSounds((long)h_OldSector, h_NewSector);
                    }
                    fn_vSetLevelName(szNewLevelName);
                }
            }
            return;
        }
    }

LAB_SetNextLevel:
    fn_vSetNextLevelName(szNewLevelName);
    fn_vChangeEngineMode(E_EM_ModeChangeLevel);
}
void __cdecl GEO_xSelectMemoryChannel(unsigned int uiMode);
void __cdecl GAM_fn_vSetGameMemoryInFix(void);
void __cdecl AI_fn_vAIUseFixMemory(void);
extern void __cdecl SAI_fn_vSAIUseFixMemory(void);   /* 0x00449fe0 */

/* 0x00405810 */
void FUN_00405810(void)
{
    GEO_xSelectMemoryChannel(1);
    GAM_fn_vSetGameMemoryInFix();
    AI_fn_vAIUseFixMemory();
    SAI_fn_vSAIUseFixMemory();
}

/* 0x4077D0 */
extern void __cdecl GEO_xSelectMemoryChannel(unsigned int uiMode);
extern void __cdecl GAM_fn_vSetGameMemoryInLevel(void);
extern void __cdecl AI_fn_vAIUseLevelMemory(void);
#include "SAI/SAI.h"

void __cdecl GAM_fn_vSetEngineInLevelMode(void)
{

    GEO_xSelectMemoryChannel(2);
    GAM_fn_vSetGameMemoryInLevel();
    AI_fn_vAIUseLevelMemory();
    SAI_fn_vSAIUseLevelMemory();
}

void fn_vSetEngineInPaused(void)
{
    /* 0x00405CC0 */
    GAM_g_bEngineIsInPaused = 1;
    GAM_g_lCurrentFrameSave = HIE_gs_lCurrentFrame;
    fn_vSaveEngineClock();
}

void fn_vResetEngineFromPaused(void)
{
    GAM_g_bEngineIsInPaused = 0;
    HIE_gs_lCurrentFrame = GAM_g_lCurrentFrameSave;

    fn_vLoadEngineClock();
}

unsigned char fn_bIsEngineInPause(void)
{
    return GAM_g_bEngineIsInPaused;
}
void fn_vFirstInitEngineClock(void)
{

    unsigned long ulTimerFreq;

    TMR_fn_wInitLibrary();                  /* 0x0045DDC0 */
    GAM_g_sTimerHandle = TMR_fn_wCreateTimer(1);     /* 0x0045DE70 */
    ulTimerFreq        = fn_ulTimerTickPerSecond(1); /* 0x0045E8A0 */
    GAM_g_ulTimerScale = ulTimerFreq / 1000;
}
#include "GLI/GLI_globals.h"

/* 0x00409c80 */
void __cdecl FUN_00409c80(void)
{
    long lTicks = GLI_pc_g_lTickBudget;   /* 0x004a7218 */
    __int64 i64Ticks;
    __int64 i64Tps;
    volatile float fTicksAsFloat;

    i64Ticks = (__int64)(unsigned __int64)(unsigned long)lTicks;
    fTicksAsFloat = (float)i64Ticks;
    i64Tps = (__int64)(unsigned __int64)(unsigned long)fn_ulTimerTickPerSecond(1);   /* 0x0045e8a0 */

    GAM_g_xFrameLength = (float)(fTicksAsFloat / (double)i64Tps);
}

unsigned long fn_ulDetermineUsefulDT(void)
{
    float xFM;
    long  lDT;
    long  lFrames;
    long  lMax;

    xFM = GAM_g_xFrameLength * 1000.0f;
    lDT = (long)GAM_g_ulDeltaTimerCount;
    if ((float)lDT < xFM)
    {
        lDT = (long)xFM;
    }
    else
    {
        lMax = (long)(xFM * 8.0f);
        if (lDT > lMax)
            lDT = lMax;
    }

    lFrames = (long)(((float)lDT * 0.001f / GAM_g_xFrameLength) - (-0.5f));
    return (unsigned long)(long)((float)lFrames * xFM);
}

void fn_vStartEngineClock(void)
{
    stTimerCount stCurrent;
    unsigned long i;

    TMR_fn_wResetTimer(GAM_g_sTimerHandle);
    TMR_fn_wStartTimer(GAM_g_sTimerHandle);
    TMR_fn_wReadTimer(GAM_g_sTimerHandle, &stCurrent);

    GAM_g_ulCurrentTimerCountLow  = stCurrent.m_ulLowPart;
    GAM_g_ulCurrentTimerCountHigh = stCurrent.m_ulHighPart;
    GAM_g_stEngineStructure.stEngineTimer.ulCurrentTimerCount = stCurrent.m_ulLowPart / GAM_g_ulTimerScale;
    GAM_g_ulUsefulDeltaTime = fn_ulDetermineUsefulDT();

    GAM_g_ulEngineTrameNumber = 1;
    GAM_g_a_ulCounter[0] = 1;
    for (i = 1; i < GAM_C_NumberOfCounter; i++)
        GAM_g_a_ulCounter[i] = 0;
}

void fn_vStopEngineClock(void)
{
    TMR_fn_wStopTimer(GAM_g_sTimerHandle);
}

void fn_vActualizeEngineClock(void)
{
    stTimerCount stCurrentTimerCount;
    unsigned long i;

    GAM_g_stEngineStructure.stEngineTimer.ulTrameNumber++;

    if (GAM_pc_g_ulActualizeClockHookGate != 0)
    {
        (*GAM_pc_g_pfn_ActualizeClockHook)();
    }

    TMR_fn_wReadTimer(GAM_g_sTimerHandle, &stCurrentTimerCount);

    GAM_g_ulDeltaTimerCount =
        ((stCurrentTimerCount.m_ulHighPart == GAM_g_ulCurrentTimerCountHigh)
             ? (stCurrentTimerCount.m_ulLowPart - GAM_g_ulCurrentTimerCountLow)
             : ((stCurrentTimerCount.m_ulLowPart > GAM_g_ulCurrentTimerCountLow)
                    ? (stCurrentTimerCount.m_ulLowPart - GAM_g_ulCurrentTimerCountLow)
                    : (stCurrentTimerCount.m_ulLowPart - GAM_g_ulCurrentTimerCountLow)))
        / GAM_g_ulTimerScale;

    GAM_g_ulCurrentTimerCountLow  = stCurrentTimerCount.m_ulLowPart;
    GAM_g_ulCurrentTimerCountHigh = stCurrentTimerCount.m_ulHighPart;

    GAM_g_ulAccumulatedTime += GAM_g_ulDeltaTimerCount;

    GAM_g_ulUsefulDeltaTime = fn_ulDetermineUsefulDT();

    for (i = 2; i < GAM_C_NumberOfCounter; i++)
    {
        if (GAM_g_a_ulCounter[i] == i - 1)
            GAM_g_a_ulCounter[i] = 0;
        else
            GAM_g_a_ulCounter[i]++;
    }
}

void fn_vSaveEngineClock(void)
{
    stTimerCount stCurrent;
    unsigned long ulDelta;

    TMR_fn_wReadTimer(GAM_g_sTimerHandle, &stCurrent);

    ulDelta = (stCurrent.m_ulHighPart == GAM_g_ulCurrentTimerCountHigh)
              ? (stCurrent.m_ulLowPart - GAM_g_ulCurrentTimerCountLow)
              : ((stCurrent.m_ulLowPart > GAM_g_ulCurrentTimerCountLow)
                     ? (stCurrent.m_ulLowPart - GAM_g_ulCurrentTimerCountLow)
                     : (stCurrent.m_ulLowPart - GAM_g_ulCurrentTimerCountLow));
    GAM_g_ulPauseTimeLow  = stCurrent.m_ulLowPart;
    GAM_g_ulPauseTimeHigh = stCurrent.m_ulHighPart;
    GAM_g_ulPauseTimeAccumulated = ulDelta / GAM_g_ulTimerScale + GAM_g_ulAccumulatedTime;
}

void fn_vLoadEngineClock(void)
{
    GAM_g_ulCurrentTimerCountLow  = GAM_g_ulPauseTimeLow;
    GAM_g_ulCurrentTimerCountHigh = GAM_g_ulPauseTimeHigh;
    GAM_g_ulAccumulatedTime       = GAM_g_ulPauseTimeAccumulated;

    TMR_fn_wSetTimerValue(GAM_g_sTimerHandle,
                          (stTimerCount *)&GAM_g_ulCurrentTimerCountLow);
    TMR_fn_wStartTimer(GAM_g_sTimerHandle);
}

void GAM_pc_fn_vRewindEngineClock(void)
{
    unsigned long ulDelta;
    stTimerCount  stNow;

    TMR_fn_wReadTimer(GAM_g_sTimerHandle, &stNow);

    ulDelta = (stNow.m_ulHighPart == GAM_g_ulCurrentTimerCountHigh)
              ? (stNow.m_ulLowPart - GAM_g_ulCurrentTimerCountLow)
              : (stNow.m_ulLowPart - GAM_g_ulCurrentTimerCountLow);
    if (GAM_g_ulCurrentTimerCountLow >= ulDelta)
    {
        GAM_g_ulCurrentTimerCountLow = GAM_g_ulCurrentTimerCountLow - ulDelta;
    }
    else
    {
        GAM_g_ulCurrentTimerCountLow  = GAM_g_ulCurrentTimerCountLow - ulDelta;
        GAM_g_ulCurrentTimerCountHigh = GAM_g_ulCurrentTimerCountHigh - 1;
    }
}
