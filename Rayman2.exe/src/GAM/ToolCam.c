#include "GAM/GAM_types.h"
#include <string.h>
#include "GAM/GAM_pc.h"
#include "GAM/cineinfo.h"

#include "HIE/HIE_types.h"
#include "POS/POS_types.h"

#include "MTH/MTH_flt.h"
#include "GLI/Camera.h"
#include "GLI/Init_Gli_helpers.h"
#include "GAM/GAM_globals.h"
#include "GAM/GAM_pc_DrawLock.h"

#include "GAM/ToolCam.h"

#define CAM_p_pGameMallocInHLM(sz)   fnp_vGameMallocInHLM((unsigned int)(sz))
#define GLI_p_xCreateCamera(h, t)    GLI_xCreateCamera((h), (long)(t))
#define GLI_p_xSetCameraAspectAndRatio GLI_xSetCameraAspectAndRatio

void CAM_fn_vLoadCameraConstants(void);
#define CAM_p_vLoadCameraConstants   CAM_fn_vLoadCameraConstants

void *__cdecl CAM_fn_p_stGetViewPortManagement(long _lViewportNumber)
{
	if
	(
		(_lViewportNumber > CAM_Nb_Viewport)
	||	(_lViewportNumber == (long) CAM_e_NoViewport)
	)
		return NULL;
	else
		return(&((tdxHandleToViewportManagement)GAM_g_stEngineStructure.a_hViewportArray)[_lViewportNumber - 1]);
}

HIE_tdstSuperObject *__cdecl CAM_fn_hGetActiveCamera(long _lViewportNumber)
{
	tdxHandleToViewportManagement	hViewport;

	hViewport = CAM_fn_p_stGetViewPortManagement(_lViewportNumber);
	return (hViewport->hCamera);
}

/* 0x0040B840 */
void __cdecl CAM_fn_vAssignCameraToAViewport(long _lViewportNumber, void *_hCamera)
{
	tdxHandleToViewportManagement	hViewport;

	hViewport = CAM_fn_p_stGetViewPortManagement(_lViewportNumber);
	hViewport->hCamera = _hCamera;
	hViewport->bValid = TRUE;
}

void __cdecl CAM_fn_vReInitCameras(void)
{
    long                            i;
    tdstNodeCamera                 *p_stTempNodeCamera;
    tdstNodeCamera                 *p_stNextNodeCamera;
    HIE_tdstSuperObject            *hSuperObject;
    tdxHandleToViewportManagement   hViewport;
    MS_tdxHandleToCineinfo          hCineinfo;
    MS_tdxHandleToInternalCineinfo  hIntCineinfo;

    for (i = 1; i <= CAM_Nb_Viewport; i++)
    {
        hViewport = CAM_fn_p_stGetViewPortManagement(i);
        hViewport->hTempCamera = NULL;
        hViewport->bValid = FALSE;
    }

    for (i = 0,
         p_stTempNodeCamera = CAM_g_hCameraListHead,
         p_stNextNodeCamera = (p_stTempNodeCamera != NULL) ? p_stTempNodeCamera->hNext : NULL;
         p_stTempNodeCamera != NULL;
         i++,
         p_stTempNodeCamera = p_stNextNodeCamera,
         p_stNextNodeCamera = (p_stTempNodeCamera != NULL) ? p_stTempNodeCamera->hNext : NULL)
    {
        if (p_stTempNodeCamera != NULL)
        {

            hSuperObject = p_stTempNodeCamera->p_stSuperObjectCamera;
            hCineinfo = M_GetMSHandle(hSuperObject,Cineinfo);
            hIntCineinfo = hCineinfo->hInit;

            if (hIntCineinfo->bIsActive &&
                hIntCineinfo->eTypeOfViewport != CAM_e_NoViewport)
            {
                CAM_fn_vAssignCameraToAViewport(hIntCineinfo->eTypeOfViewport, hSuperObject);
            }
        }
    }
}

/* 0x0040B8D0 */
void __cdecl CAM_fn_vInitCameras(void)
{
	long							lSize;
    long							i;
	tdxHandleToViewportManagement	hViewport;

	lSize = sizeof(struct tdstViewportManagement_) * CAM_Nb_Viewport;
	GAM_g_stEngineStructure.a_hViewportArray = (tdxHandleToViewportManagement)CAM_p_pGameMallocInHLM(lSize);

	CAM_fn_vReInitCameras();

	for (i = 1; i <= CAM_Nb_Viewport; i++)
	{
		hViewport = CAM_fn_p_stGetViewPortManagement(i);
		GLI_p_xCreateCamera(&hViewport->p_stCamera,GLI_C_lPersCamWithoutDistorsion);
		GLI_p_xSetCameraAspectAndRatio(hViewport->p_stCamera, MTH_M_xConstFloatToReal(1.5f), MTH_M_xConstFloatToReal(0.75f));
	}

    CAM_p_vLoadCameraConstants();
}

/* 0x500554 */
void __cdecl CAM_fn_vCameraManagement(void)
{
    tdstNodeCamera                 *p_stNodeCamera;
    struct HIE_tdstSuperObject     *hCamera;
    MS_tdxHandleToCineinfo          hCineinfo;
    MS_tdxHandleToInternalCineinfo  hIntCineinfo;
    tdxHandleToViewportManagement   hViewport;
    long                            i;

    for (p_stNodeCamera = CAM_g_hCameraListHead; p_stNodeCamera != NULL;
         p_stNodeCamera = p_stNodeCamera->hNext)
    {
        if (p_stNodeCamera != NULL)
        {
            hCamera = p_stNodeCamera->p_stSuperObjectCamera;

            hCineinfo = M_GetMSHandle(hCamera, Cineinfo);

            if (hCineinfo->ucVolIAFlags & C_VolIAFlags_CurrentAlreadyCopiedInWork)
                hIntCineinfo = hCineinfo->hWork;
            else
                hIntCineinfo = hCineinfo->hCurrent;

            if (hIntCineinfo->bIsActive ||
                hIntCineinfo->eTypeOfViewport != CAM_e_NoViewport)
            {
                if (hIntCineinfo->eTypeOfViewport != CAM_e_NoViewport)
                    fn_vTreatCameraHierarchy(hCamera, TRUE);
                else
                    fn_vTreatCameraHierarchy(hCamera, FALSE);
            }
        }
    }

    for (i = CAM_Nb_Viewport; i > 0; i--)
    {
        hViewport = CAM_fn_p_stGetViewPortManagement(i);
        if (hViewport->bValid)
            CAM_fn_vActualizeAViewport(i);
    }
}

#define GLI_p_xSetCameraMatrix(c, m)    GLI_xSetCameraMatrix((c), (long *)(m))
#define GLI_p_xGetCameraAspectAndRatio  GLI_xGetCameraAspectAndRatio
#define GLI_p_xSetViewportCamera(d, v, c)  GLI_xSetViewportCamera((unsigned long)(d), (unsigned short)(v), (long)(c))

#define GLI_p_xAdjustCameraToViewport(d, v, c) \
    GLI_xAdjustCameraToViewport((unsigned long)(d), (unsigned short)(v), (struct GLI_tdstCamera_ *)(c))

#define CAM_g_uwDeviceWord    (GAM_g_stEngineStructure.hGLDDevice)    /* 0x500454 */
#define CAM_g_uwDisplayState  (GAM_g_stEngineStructure.hGLDViewport)  /* 0x500456 */

#define CAM_g_hDrawSemaphore     (GAM_g_stEngineStructure.hDrawSem)   /* 0x5004CC */

void __cdecl CAM_fn_vActualizeAViewport(long _lViewport)
{
    tdxHandleToViewportManagement hViewport;
    void                     *p_Camera;
    POS_tdstCompletePosition  stMatrix;
    MTH_tdxReal               xAngle,xRatio, xFocal;
    MTH3D_tdstVector          I,J,K;
    POS_tdstCompletePosition *p_stCamera;
    HIE_tdstSuperObject      *hCamera;

    hViewport = CAM_fn_p_stGetViewPortManagement(_lViewport);
    hCamera   = hViewport->hCamera;

    p_stCamera = HIE_fn_hGetSuperObjectGlobalMatrix(hCamera);

    POS_fn_vGetRotationMatrix( p_stCamera, &I, &J, &K );

    POS_fn_vCopyMatrix( &stMatrix, p_stCamera );

    MTH3D_M_vNegVector( &J, &J );
    POS_fn_vSetRotationMatrix( &stMatrix, &I, &K, &J );
    POS_fn_vInvertIsoMatrix( &stMatrix, &stMatrix );

    GAM_pc_M_ulWaitDrawSemaphore(CAM_g_hDrawSemaphore);

    p_Camera = hViewport->p_stCamera;
    GLI_p_xSetCameraMatrix( p_Camera, &stMatrix );
    GLI_p_xGetCameraAspectAndRatio( p_Camera, &xAngle, &xRatio );
    xFocal = M_GetMSHandle(hCamera, Cineinfo)->hWork->xFocal;

    if (MTH_M_bIsNull(xFocal))
        xFocal = M_GetMSHandle(hCamera, Cineinfo)->hCurrent->xFocal;

    if (!MTH_M_bEqual(xFocal, xAngle))
    {
        GLI_p_xSetCameraAspectAndRatio( p_Camera, xFocal, xRatio );
        GLI_p_xSetViewportCamera(CAM_g_uwDeviceWord, CAM_g_uwDisplayState, p_Camera);
        GLI_p_xAdjustCameraToViewport(CAM_g_uwDeviceWord, CAM_g_uwDisplayState, p_Camera);
    }
    else
        GLI_p_xSetViewportCamera(CAM_g_uwDeviceWord, CAM_g_uwDisplayState, p_Camera);

    GAM_pc_M_bReleaseDrawSemaphore(CAM_g_hDrawSemaphore);
}

ACP_tdxBool __cdecl CAM_fn_bSuperObjectIsACamera(HIE_tdstSuperObject *hSuperObj)
{
    if (hSuperObj->ulTypeOfLinkedObject != HIE_C_ulActor)
        return FALSE;

    if ((struct tdstCineinfo_ *)M_GetMSHandle(hSuperObj,Cineinfo) != NULL)
        return TRUE;
    return FALSE;
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

/* 0x004140A0 */
void __cdecl CAM_fn_vSetCineinfoWorkFromCurrent(struct tdstCineinfo_ *hCineinfo)
{
    if ((hCineinfo->ucVolIAFlags & C_VolIAFlags_CurrentAlreadyCopiedInWork) == 0) {
        unsigned long *pulSrc = (unsigned long *)hCineinfo->hCurrent;
        unsigned long *pulDst = (unsigned long *)hCineinfo->hWork;
        hCineinfo->ucVolIAFlags |= C_VolIAFlags_CurrentAlreadyCopiedInWork;
        memcpy(pulDst, pulSrc, 0x1f * 4);
    }
}

/* 0x00414370 */
void __cdecl CAM_fn_vRemindSpeed(struct tdstCineinfo_ *hCineinfo, MTH_tdstMove *p_Speed)
{
    unsigned long *pulDst = (unsigned long *)hCineinfo->AveragePosition.a_CheckedSpeed;
    unsigned long *pulSrc = (unsigned long *)&hCineinfo->AveragePosition.a_CheckedSpeed[1];
    memcpy(pulDst, pulSrc, 0x15 * 4);

    *(unsigned long *)&hCineinfo->AveragePosition.a_CheckedSpeed[7].xX = *(unsigned long *)&p_Speed->m_stLinear.xX;
    *(unsigned long *)&hCineinfo->AveragePosition.a_CheckedSpeed[7].xY = *(unsigned long *)&p_Speed->m_stLinear.xY;
    *(unsigned long *)&hCineinfo->AveragePosition.a_CheckedSpeed[7].xZ = *(unsigned long *)&p_Speed->m_stLinear.xZ;
}

/* 0x004143B0 */
void __cdecl CAM_fn_vRemindPos(struct tdstCineinfo_ *hCineinfo, MTH_tdstMove *p_Pos)
{
    unsigned long *pulDst = (unsigned long *)hCineinfo->AveragePosition.a_CheckedPos;
    unsigned long *pulSrc = (unsigned long *)&hCineinfo->AveragePosition.a_CheckedPos[1];
    memcpy(pulDst, pulSrc, 0x15 * 4);

    *(unsigned long *)&hCineinfo->AveragePosition.a_CheckedPos[7].xX = *(unsigned long *)&p_Pos->m_stLinear.xX;
    *(unsigned long *)&hCineinfo->AveragePosition.a_CheckedPos[7].xY = *(unsigned long *)&p_Pos->m_stLinear.xY;
    *(unsigned long *)&hCineinfo->AveragePosition.a_CheckedPos[7].xZ = *(unsigned long *)&p_Pos->m_stLinear.xZ;
}

/* 0x004143F0 */
void __cdecl CAM_fn_vRemindTime(struct tdstCineinfo_ *hCineinfo, MTH_tdxReal _dt)
{
    unsigned long *pulDst = (unsigned long *)hCineinfo->AveragePosition.a_CheckedTime;
    unsigned long *pulSrc = (unsigned long *)&hCineinfo->AveragePosition.a_CheckedTime[1];
    memcpy(pulDst, pulSrc, 7 * 4);
    hCineinfo->AveragePosition.a_CheckedTime[7] = _dt;
}

/* 0x00414080 */
void __cdecl CAM_fn_vSetCineinfoCurrentFromInit(struct tdstCineinfo_ *hCineinfo)
{
    unsigned long *pulSrc = (unsigned long *)hCineinfo->hInit;
    unsigned long *pulDst = (unsigned long *)hCineinfo->hCurrent;
    memcpy(pulDst, pulSrc, 0x1f * 4);
    hCineinfo->ucVolIAFlags |= C_VolIAFlags_InitJustCopiedInCurrent;
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

/* 0x004141B0 */
void __cdecl fn_vCineinfoSetSightAxis(struct tdstCineinfo_ *hCineinfo, MTH3D_tdstVector *p_stVector)
{
    hCineinfo->stSightAxisSystem.SightAxis.xX = p_stVector->xX;
    hCineinfo->stSightAxisSystem.SightAxis.xY = p_stVector->xY;
    hCineinfo->stSightAxisSystem.SightAxis.xZ = p_stVector->xZ;
}

/* 0x004141F0 */
void __cdecl fn_vCineinfoSetFirstComplementaryAxis(struct tdstCineinfo_ *hCineinfo, MTH3D_tdstVector *p_stVector)
{
    hCineinfo->stSightAxisSystem.FirstComplementaryAxis.xX = p_stVector->xX;
    hCineinfo->stSightAxisSystem.FirstComplementaryAxis.xY = p_stVector->xY;
    hCineinfo->stSightAxisSystem.FirstComplementaryAxis.xZ = p_stVector->xZ;
}

/* 0x00414230 */
void __cdecl fn_vCineinfoSetSecondComplementaryAxis(struct tdstCineinfo_ *hCineinfo, MTH3D_tdstVector *p_stVector)
{
    hCineinfo->stSightAxisSystem.SecondComplementaryAxis.xX = p_stVector->xX;
    hCineinfo->stSightAxisSystem.SecondComplementaryAxis.xY = p_stVector->xY;
    hCineinfo->stSightAxisSystem.SecondComplementaryAxis.xZ = p_stVector->xZ;
}

/* 0x00414060 */
struct HIE_tdstSuperObject *__cdecl CAM_fn_hCineInfoGetSuperObjectTargeted(
    struct tdstInternalStructurCineinfo_ *_hIntCineinfo)
{
    return _hIntCineinfo->hSuperObjectTargeted;
}

void __cdecl CAM_vDisplayInfo(void)
{
}
