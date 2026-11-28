#include "GAM/GAM_types.h"
#include "GAM/PlayAnim.h"
#include "GAM/STRUCTUR/State.h"
#include "AI/AI_types.h"
#include "AI/AI_globals.h"
#include "ACP/ACP_types.h"
#include "DNM/DNM.h"
#include "MTH/MTH3D.h"
#include "MEC/MEC.h"
#include "GMT/GmtFunct.h"
#include "GLI/VisuSet.h"
#include "PO/POProto.h"
#include "PO/IPOProto.h"
#include "POS/POS_types.h"
#include "GLI/GLI_types.h"
#include "GAM/GAM_globals.h"
#include "GAM/STRUCTUR/Engmode.h"
#include "MEC/DNMRepor.h"

#define C_WOT_ulMobile 0x00010000

#define DNM_M_vDynamicsSetSpeedAnim(_p_stDynamics,pstSpeedAnim) \
    MTH3D_M_vCopyVector(DNM_M_p_stDynamicsGetSpeedAnim(_p_stDynamics), pstSpeedAnim)

/* 0x0040FF90 */
MTH3D_tdstVector *__cdecl PLA_fn_stGetLinearSpeedVector(HIE_tdstSuperObject *p_stSuperObject, short sLastFrame);
MTH3D_tdstMatrix *__cdecl PLA_fn_stGetAngularSpeedMatrix(HIE_tdstSuperObject *p_stSuperObject, short sLastFrame);

typedef void (*tdpfnDynamicFunction)(DNM_tdstDynamics *_p_stDynamics, HIE_tdstSuperObject *_hSupObj,
                                     void *_p_stParsingDatas, void *_h_MecIdCard, MTH_tdxReal _xDT);
#define g_a_pfnDynamicFunction ((tdpfnDynamicFunction *)GAM_g_a_pfnDynamicFunction)

/* 0x0042DE30 */
extern void *__cdecl fn_p_stDynamGetParsingDatas(void *h_Dynam);
extern void __cdecl fn_vDynamSetUsedMechanics(void *h_Dynam, long eUsedMechanics);
struct DNM_stMecBaseIdCard;
void __cdecl DNM_fn_vChangeIdCard(DNM_tdstDynamics *p_stDynamics, struct DNM_stMecBaseIdCard *hNewIdCard);

extern void __cdecl GLI_xGetMaterialTextureScrollingCoef(ACP_tdxHandleOfMaterial hMaterial, long lNumberOfTexture,
                                                         long *p_bSwitch, MTH_tdxReal *p_fUSpeed,
                                                         MTH_tdxReal *p_fVSpeed);
extern ACP_tdxBool INT_fn_bIntersectSegmentWithFaceOfGeometricObject(MTH3D_tdstVector *p_stVertexA,
                                                                     MTH3D_tdstVector *p_stVertexB,
                                                                     MTH3D_tdstVector *p_stVectAB,
                                                                     ACP_tdxHandleOfObject p_stGeomObj,
                                                                     ACP_tdxIndex xNbMaxElements,
                                                                     ACP_tdxIndex *p_xNbElements,
                                                                     struct GLI_tdstDataOfElement_ *d_stDataOfElement);
extern void GEO_vGetFaceSurfaceSpeed(ACP_tdxHandleOfObject hObject, ACP_tdxIndex xElement,
                                     ACP_tdxIndex xFace, MTH3D_tdstVector *p_vSpeed);

/* 0x00411CF0 */
void __cdecl fn_vGetScrollSpeedMatCollided(HIE_tdstSuperObject *p_stSuperObj, MTH3D_tdstVector *p_stVector,
                                           unsigned long ulObstacleType)
{
    DNM_tdstReport      *p_stDynamReport;
    MS_tdxHandleToDynam  h_Dynam;
    DNM_tdstDynamics    *p_stDynamics;
    DNM_tdstObstacle    *p_stObstacle;
    long                 bScrollMode;
    MTH_tdxReal          xUSpeed, xVSpeed;
    GMT_tdxHandleToGameMaterial hGmt;
    ACP_tdxHandleOfMaterial     hVisualMaterial;

    MTH3D_M_vNullVector(p_stVector);

    if ((h_Dynam = M_GetMSHandle(p_stSuperObj, Dynam)) == NULL)
        return;

    if ((p_stDynamics = fn_p_stDynamGetDynamics(h_Dynam)) == NULL)
        return;

    p_stDynamReport = DNM_M_p_stDynamicsGetReport(p_stDynamics);
    if (!p_stDynamReport)
        return;

    switch (ulObstacleType)
    {
    default:
        p_stObstacle = DNM_M_p_stReportGetObstacle(p_stDynamReport);
        break;

    case 1:
        p_stObstacle = DNM_M_p_stReportGetGround(p_stDynamReport);
        break;

    case 2:
        p_stObstacle = DNM_M_p_stReportGetWall(p_stDynamReport);
        break;

    case 3:
        p_stObstacle = DNM_M_p_stReportGetCharacter(p_stDynamReport);
        break;

    case 4:
        p_stObstacle = DNM_M_p_stReportGetWater(p_stDynamReport);
        break;

    case 5:
        p_stObstacle = DNM_M_p_stReportGetCeil(p_stDynamReport);
        break;
    }

    hGmt = DNM_M_hObstacleGetCollidedMaterial(p_stObstacle);
    if (!GMT_fn_b_ulIsValid(hGmt))
        return;

    hVisualMaterial = GMT_fn_hGetVisualMaterial(hGmt);
    if (hVisualMaterial == NULL)
        return;

    GLI_xGetMaterialTextureScrollingCoef(hVisualMaterial, 0, &bScrollMode, &xUSpeed, &xVSpeed);

    if (bScrollMode)
    {

        HIE_tdstSuperObject *p_stObjectCollided = DNM_M_p_stObstacleGetObject(p_stObstacle);

        void *p_stInstanciatedPhysicalObject;
        void *p_stPhysicalObject;
        void *hVisualSet;
        MTH_tdxReal xThreshold;
        ACP_tdxHandleOfObject hObject;

        p_stInstanciatedPhysicalObject = HIE_fn_hGetSuperObjectObject(p_stObjectCollided);

        p_stPhysicalObject = IPO_fn_hGetPhysicalObject(p_stInstanciatedPhysicalObject);

        hVisualSet = PO_fn_hGetVisualSet(p_stPhysicalObject);

        GLI_vGetVisualSetLOD(hVisualSet, 0, &xThreshold, (void **)&hObject);

        {
            MTH3D_tdstVector vXYZSpeed, vGlobalSpeed;
            POS_tdstCompletePosition mGlobalToPerso;
            struct GLI_tdstDataOfElement_ stElement;
            ACP_tdxIndex xNbFaceCollided;
            MTH3D_tdstVector vContact, vNorm, vPt1, vPt2, vV12;
            POS_tdstCompletePosition mGlobalToObst;

            POS_fn_vInvertMatrix(&mGlobalToObst, HIE_fn_hGetSuperObjectGlobalMatrix(p_stObjectCollided));

            POS_fn_vMulMatrixVertex(&vContact, &mGlobalToObst, DNM_M_p_stObstacleGetContact(p_stObstacle));
            POS_fn_vMulMatrixVector(&vNorm, &mGlobalToObst, DNM_M_p_stObstacleGetNorm(p_stObstacle));

            MTH3D_M_vCopyVector(&vPt1, &vContact);
            MTH3D_M_vCopyVector(&vPt2, &vContact);

            MTH3D_M_vAddVector(&vPt1, &vPt1, &vNorm);
            MTH3D_M_vSubVector(&vPt2, &vPt2, &vNorm);
            MTH3D_M_vSubVector(&vV12, &vPt2, &vPt1);

            INT_fn_bIntersectSegmentWithFaceOfGeometricObject(
                &vPt1, &vPt2, &vV12, hObject, 1, &xNbFaceCollided, &stElement);

            if (!xNbFaceCollided)
                return;

            GEO_vGetFaceSurfaceSpeed(hObject, stElement.xElements, stElement.xIndexOfFace, &vXYZSpeed);

            POS_fn_vMulMatrixVector(&vGlobalSpeed, HIE_fn_hGetSuperObjectGlobalMatrix(p_stObjectCollided), &vXYZSpeed);

            POS_fn_vInvertIsoMatrix(&mGlobalToPerso, HIE_fn_hGetSuperObjectGlobalMatrix(p_stSuperObj));

            POS_fn_vMulMatrixVector(p_stVector, &mGlobalToPerso, &vGlobalSpeed);
        }
    }
}

/* 0x00411F80 */
void __cdecl fn_vSuperObjectSetLastCollidedCharacter(HIE_tdstSuperObject *hCharacter,
                                                     HIE_tdstSuperObject *_h_LastCollidedCharacter)
{
    DNM_tdstReport   *p_stDynamReport;
    MS_tdxHandleToDynam h_Dynam;
    DNM_tdstDynamics *p_stDynamics;

    if ((h_Dynam = M_GetMSHandle(hCharacter,Dynam)) == NULL )
        return;

    if ((p_stDynamics = fn_p_stDynamGetDynamics(h_Dynam)) == NULL)
        return;

    p_stDynamReport = DNM_M_p_stDynamicsGetReport(p_stDynamics);
    if (!p_stDynamReport)
        return;

    DNM_M_p_stObstacleSetObject(DNM_M_p_stReportGetCharacter(p_stDynamReport),_h_LastCollidedCharacter);

    if (!_h_LastCollidedCharacter)
        DNM_M_ulReportSetSurfaceState (p_stDynamReport, (DNM_M_ulReportGetSurfaceState(p_stDynamReport) & ~C_WOT_ulMobile));
}

/* 0x00411FC0 */
HIE_tdstSuperObject *fn_hSuperObjectGetLastCollidedCharacter(HIE_tdstSuperObject *hCharacter) {
	DNM_tdstReport * p_stDynamReport;
	MS_tdxHandleToDynam h_Dynam;
	DNM_tdstDynamics *p_stDynamics;
	HIE_tdstSuperObject *p_stObjectCollided;

	if((h_Dynam = M_GetMSHandle(hCharacter,Dynam)) == NULL )
		return NULL;

	if((p_stDynamics = fn_p_stDynamGetDynamics(h_Dynam)) == NULL)
		return NULL;

	p_stDynamReport = DNM_M_p_stDynamicsGetReport(p_stDynamics);
	if (!p_stDynamReport)
		return NULL;

	p_stObjectCollided=DNM_M_p_stObstacleGetObject(DNM_M_p_stReportGetCharacter(p_stDynamReport));

	while (p_stObjectCollided && (p_stObjectCollided->ulTypeOfLinkedObject != HIE_C_ulActor)) {

		p_stObjectCollided = HIE_fn_hGetSuperObjectFather(p_stObjectCollided);
	}

	if (p_stObjectCollided && !M_GetMSHandle(p_stObjectCollided,StandardGame)) {
		p_stObjectCollided = NULL;
		DNM_M_p_stObstacleSetObject(DNM_M_p_stReportGetCharacter(p_stDynamReport),NULL);
	}

	return p_stObjectCollided;
}

void __cdecl InitArrayOfDynamicFunction(void **p_pfnRecord)
{
    p_pfnRecord[0] = (void *)DNM_p_stDynamicsCameraMechanics;
    p_pfnRecord[1] = (void *)MEC_p_stDynamicsBaseMechanics;
}

/* 0x00412040 */
void *__cdecl fn_h_GetCurrentDNMIdCard(HIE_tdstSuperObject *p_SuperObjPerso)
{
    tdxHandleToState hCurrentState =
        fn_h3dDataGetCurrentState(M_GetMSHandle(p_SuperObjPerso, 3dData));

    if (hCurrentState == NULL)
        return NULL;
    return fn_hGetStateMechanicsIdCard(hCurrentState);
}

/* 0x00412060 */
void __cdecl fn_vTransferSpeedInformationPLA_DNM(HIE_tdstSuperObject *p_SuperObjPerso, short sLastFrame)
{
    MS_tdxHandleToDynam h_Dynam;
    DNM_tdstDynamics *p_stDynamics;
    MTH3D_tdstVector *p_MthVector;
    MTH3D_tdstMatrix *p_MthRotationMatrix;

    if ((h_Dynam = M_GetMSHandle(p_SuperObjPerso, Dynam)) == NULL)
        return;
    if ((p_stDynamics = fn_p_stDynamGetDynamics(h_Dynam)) == NULL)
        return;

    p_MthVector = PLA_fn_stGetLinearSpeedVector(p_SuperObjPerso, sLastFrame);
    p_MthRotationMatrix = PLA_fn_stGetAngularSpeedMatrix(p_SuperObjPerso, sLastFrame);

    if (p_MthVector)
    {

          DNM_M_vDynamicsSetSpeedAnim(p_stDynamics, p_MthVector);
    }

    if (p_MthRotationMatrix)
        MEC_vImposeRelativeRotation(p_stDynamics, p_MthRotationMatrix);
}

void __cdecl fn_vMakeCharacterThink(HIE_tdstSuperObject *p_stSuperObj)
{
    AI_tdstMind         *p_stMind;
    MS_tdxHandleToBrain  h_Brain;

    fn_v3dDataUpdateBrainCounter(p_stSuperObj);

    if (!fn_b3dDataCanComputeBrain(p_stSuperObj))
        return;
    if ((h_Brain = M_GetMSHandle(p_stSuperObj, Brain)) == NULL)
        return;
    if ((p_stMind = AI_M_pstGetMindOfBrain(h_Brain)) == NULL)
        return;

    fn_v3dDataUpdateMainBrainCounter(M_GetMSHandle(p_stSuperObj, 3dData));
    gcGlobAleat = (char)fn_uw3dDataGetMainBrainCounter(M_GetMSHandle(p_stSuperObj, 3dData));

    if (!fn_bDoReflexPerso(p_stSuperObj))
    {

        if ((h_Brain = M_GetMSHandle(p_stSuperObj, Brain)) != NULL)
            if ((p_stMind = AI_M_pstGetMindOfBrain(h_Brain)) != NULL)
                fn_bDoIntelligencePerso(p_stSuperObj);
    }
}

/* 0x00412150 */
void __cdecl fn_vMakeCharacterMechanicallyReact(HIE_tdstSuperObject *p_stSuperObj)
{
    MS_tdxHandleToDynam h_Dynam;
    DNM_tdstDynamics *p_stDynamics;
    void *h_IdentityCard;

    if ((h_Dynam = M_GetMSHandle(p_stSuperObj, Dynam)) == NULL)
        return;
    if ((p_stDynamics = fn_p_stDynamGetDynamics(h_Dynam)) == NULL)
        return;

    h_IdentityCard = fn_h_GetCurrentDNMIdCard(p_stSuperObj);

    if (DNM_M_pDynamicsGetCurrentIdCard(p_stDynamics) != h_IdentityCard)
        DNM_fn_vChangeIdCard(p_stDynamics, h_IdentityCard);

    if (h_IdentityCard != NULL)
    {
        g_a_pfnDynamicFunction[DNM_fn_eIdentityCardGetType(h_IdentityCard)]
        (
            p_stDynamics,
            p_stSuperObj,
            fn_p_stDynamGetParsingDatas(M_GetMSHandle(p_stSuperObj, Dynam)),
            h_IdentityCard,
            M_xGetUsefulDeltaTimeInSeconds(GAM_g_stEngineStructure.stEngineTimer)
        );

        if (DNM_M_bDynamicsIsCollisionReport(p_stDynamics)) DNM_M_ReportStateTranslate(DNM_M_p_stDynamicsGetReport(p_stDynamics));
        fn_vDynamSetUsedMechanics(h_Dynam, DNM_fn_eIdentityCardGetType(h_IdentityCard));
    }
}
