#include "GAM/Always.h"
#include "ALW/ALW_globals.h"

/* 0x0040BC60 */
unsigned char __cdecl fn_ucIsAnAlwaysObject(struct tdstEngineObject_ *p_stEngineObject)
{

	return (unsigned char)(p_stEngineObject->h_StandardGame->ucMiscFlags & GAM_C_Always ? TRUE : FALSE);
}

#define M_ObjectGetPersonalType(p_stObject) ((p_stObject)->h_StandardGame->otObjectPersonalType)

unsigned char __cdecl fn_ucIsAnAlwaysModel(struct tdstEngineObject_ *p_stEngineObject)
{

	unsigned long				i;
	ALW_tdxHandleToModelList	h_AlwaysModelChild;

	if (M_ObjectGetPersonalType(p_stEngineObject) >= C_AlwaysObjectType)
		return TRUE;

	LST2_M_DynamicForEachElementOf(&(g_stAlways.ALW_h_LstAlwaysModel),h_AlwaysModelChild,i)
	{
		if (M_ObjectGetPersonalType(p_stEngineObject)==M_ObjectGetPersonalType(h_AlwaysModelChild->p_stAlwaysObject))
			return TRUE;
	}

	return( FALSE );
}

#include "ALW/ALW.h"
#include "MMG/MMG_globals.h"
#include "GAM/PlayAnim.h"
#include "GAM/GAM_pc_session.h"
#include "GLI/GLI_types.h"
#include "ERM/CpaError.h"
#include <stddef.h>

#define HIE_M_hNextChild(_hBrother)  ((_hBrother)->p_stNextBrother)

#define ALW_M_h3dDataGetFamily(h3d)  (((MS_tdxHandleTo3dData)(h3d))->h_Family)

#define ALW_ulCurrentTimerCount GAM_g_ulAccumulatedTime  /* 0x5003EC */

#include "AI/AI_types.h"

extern tdstStructureHandleFunction g_a_stStructureHandleFunction[];   /* 0x0049F4C8 */

/* 0x00412850 */
extern HIE_tdstSuperObject *__cdecl GAM_fn_hGetCurrentSector(HIE_tdstSuperObject *_hCharacter);
#define ALW_M_pfn_GAM_fn_hGetCurrentSector GAM_fn_hGetCurrentSector
/* 0x0044BDC0 */
extern int __cdecl SECT_fn_bIsInBorderBox(MTH3D_tdstVector *_p_stPosition, void *_hSector);
#define ALW_M_pfn_SECT_fn_bIsInBorderBox SECT_fn_bIsInBorderBox
/* 0x0044BE50 */
extern HIE_tdstSuperObject *__cdecl SECT_fn_hResearchInWhatSectorIAm(HIE_tdstSuperObject *SECT_hFatherSector, MTH3D_tdstVector *p_stAbsolutePosition);
#define ALW_M_pfn_SECT_fn_hResearchInWhatSectorIAm SECT_fn_hResearchInWhatSectorIAm
/* 0x0042DB20 */
extern void __cdecl fn_vSectInfoSetCurrentSector(void *h_SectInfo, void *hCurrentSector);
/* 0x00407AA0 */
extern void *__cdecl fn_p_stMSLightGetLight(void *h_MSLight);
#define ALW_M_pfn_fn_vGetBoundingVolumeOfFamily fn_p_stMSLightGetLight
/* 0x0042DE30 */
extern tdstAnim3d *__cdecl fn_p_stGetAnimInState(void *hState);
/* 0x00418A20 */
extern void __cdecl fn_vAllocArrayOfElts3d(void *h_3dData, unsigned short _wNumberOfElts3d);

#ifndef RAYMAN2_NO_PROTECTION   /* LCG of the relocation-table copy selector */
static unsigned int alw_lcg_step(unsigned int seed)
{
    unsigned int s    = seed ^ 0x75bd924u;
    unsigned int next = s * 0x41a7u + (unsigned int)((int)s / 0x1f31d * -0x7fffffff);
    if ((int)next < 0)
        next += 0x7fffffffu;
    return next;
}
#endif /* RAYMAN2_NO_PROTECTION */

HIE_tdxHandleToSuperObject __cdecl fn_p_stAllocateAlways(
    tdObjectType                otObjectModelType,
    HIE_tdxHandleToSuperObject  p_stFatherSuperObject,
    HIE_tdxHandleToSuperObject  _hGenerator,
    unsigned short              uwAction,
    POS_tdstCompletePosition   *p_stMatrix)
{
    HIE_tdxHandleToSuperObject  h_stSuperObject = NULL;
    HIE_tdxHandleToSuperObject  h_SOSector;
    unsigned long               i, j;
    ALW_tdxHandleToModelList    h_AlwaysModelChild;
    struct tdstEngineObject_   *p_stEngineObject = NULL;
    MTH3D_tdstVector           *p_stTempVector = NULL;

    (void)uwAction;

    LST2_M_DynamicForEachElementOf(&(g_stAlways.ALW_h_LstAlwaysModel), h_AlwaysModelChild, i)
    {

        if (h_AlwaysModelChild->otObjectModelType == otObjectModelType)
        {

            h_stSuperObject = g_stAlways.d_stAlwaysSuperObject;

            for ( i = 0;
                  ( i < g_stAlways.ulMaxNumberOfAlways )
                  && ( g_stAlways.d_hAlwaysGenerator[i] != NULL
                       || HIE_M_hNextChild(h_stSuperObject) != NULL );
                  i++, h_stSuperObject++ )
            {
            }

            if (i >= g_stAlways.ulMaxNumberOfAlways)
            {
                return NULL;
            }

            g_stAlways.d_hAlwaysGenerator[i] = _hGenerator;

            p_stEngineObject = (struct tdstEngineObject_ *)HIE_fn_hGetSuperObjectObject(h_stSuperObject);

            HIE_fn_SO_vSetFlags(h_stSuperObject, 0);
            HIE_fn_vSetSuperObjectDrawMask(h_stSuperObject, GLI_C_lAllIsEnable);

            M_ForAllMiniStructureExceptNULL(h_AlwaysModelChild->p_stAlwaysObject, j)
            {

                if (M_VoidMiniStructurePointer(&g_stAlways.d_stAlwaysEngineObjectInit[i], j) == NULL)
                    g_a_stStructureHandleFunction[j].p_fn_vAllocFunction(&g_stAlways.d_stAlwaysEngineObjectInit[i]);

                M_CopyMSPointer(p_stEngineObject, &(g_stAlways.d_stAlwaysEngineObjectInit[i]), j);
                g_a_stStructureHandleFunction[j].p_fn_vCopyCloneMSFunction(p_stEngineObject, h_AlwaysModelChild->p_stAlwaysObject);
            }

            M_ObjectSetPersonalType(p_stEngineObject, C_AlwaysObjectType + i);
            M_SetSuperObject(p_stEngineObject, h_stSuperObject);

            if (p_stEngineObject->h_3dData)
            {
                fn_v3dDataSetMatrix(p_stEngineObject->h_3dData, p_stMatrix);
            }

            POS_fn_vMulMatrixMatrix(HIE_fn_hGetSuperObjectGlobalMatrix(h_stSuperObject),
                                    HIE_fn_hGetSuperObjectGlobalMatrix(p_stFatherSuperObject),
                                    HIE_fn_hGetSuperObjectMatrix(h_stSuperObject));
            HIE_fn_vValidateGlobalMatrix(h_stSuperObject);

            p_stTempVector = POS_fn_p_stGetTranslationVector(HIE_fn_hGetSuperObjectGlobalMatrix(h_stSuperObject));
            h_SOSector = ALW_M_pfn_GAM_fn_hGetCurrentSector(_hGenerator);
            if ( !ALW_M_pfn_SECT_fn_bIsInBorderBox(p_stTempVector, HIE_fn_hGetSuperObjectObject(h_SOSector)) )
            {
                h_SOSector = ALW_M_pfn_SECT_fn_hResearchInWhatSectorIAm(SECT_hFatherSector, p_stTempVector);
            }
            if ( !h_SOSector )
            {
                g_stAlways.d_hAlwaysGenerator[i] = NULL;
                return NULL;
            }

            fn_vSectInfoSetCurrentSector(p_stEngineObject->h_SectInfo, h_SOSector);

            M_ForAllMiniStructureExceptNULL(p_stEngineObject, j)
            {
                g_a_stStructureHandleFunction[j].p_fn_vInitFunction(p_stEngineObject, OTI_AlwaysCreated);
            }

            if ( fn_ucStandardGameGetPlatFormType(M_GetMSHandle(h_stSuperObject, StandardGame)) )
            {
                HIE_fn_vSuperObjectAddHead(p_stFatherSuperObject, h_stSuperObject);
            }
            else
            {
                HIE_fn_vSuperObjectAddTail(p_stFatherSuperObject, h_stSuperObject);
            }

            fn_vStandardGameSetIsActivable(p_stEngineObject->h_StandardGame, 1);

            h_stSuperObject->hBoundingVolume =
                ALW_M_pfn_fn_vGetBoundingVolumeOfFamily(ALW_M_h3dDataGetFamily(p_stEngineObject->h_3dData));

            M_ActivateObject(p_stEngineObject);
            {
                fn_v3dDataSetStartTime(p_stEngineObject->h_3dData, ALW_ulCurrentTimerCount);
                fn_v3dDataSetTimePreviousFrame(p_stEngineObject->h_3dData, ALW_ulCurrentTimerCount);
            }

            if (fn_p_st3dDataGetCurrentFrame(p_stEngineObject->h_3dData)->p_stArrayOfElts3d == NULL)
            {
                tdstAnim3d        *p_stCurrentAnim;
                tdxHandleToState   hState;

                if (fn_h3dDataGetCurrentState(p_stEngineObject->h_3dData))
                {
                    hState = fn_h3dDataGetCurrentState(p_stEngineObject->h_3dData);
                    p_stCurrentAnim = fn_p_stGetAnimInState(hState);
                    if (p_stCurrentAnim)
                    {
                        fn_vAllocArrayOfElts3d(p_stEngineObject->h_3dData, p_stCurrentAnim->ucMaxNumberOfElements);
                        fn_p_st3dDataGetCurrentFrame(p_stEngineObject->h_3dData)->p_stAnim = NULL;
                    }
                }

                fn_p_st3dDataGetCurrentFrame(p_stEngineObject->h_3dData)->p_stAnim = NULL;
            }

#ifndef RAYMAN2_NO_PROTECTION   /* steps the random relocation-table copy selector */
            MMG_g_ulLCGState = alw_lcg_step(MMG_g_ulLCGState);
            GAM_pc_g_ucAlwaysSpawnCounter =
                (unsigned char)(GAM_pc_g_ucAlwaysSpawnCounter + (int)(MMG_g_ulLCGState) % 5 + 1);
#endif /* RAYMAN2_NO_PROTECTION */

            return h_stSuperObject;
        }
    }

    ERM_fn_vUpdateLastError(0x30, g_ucGameModuleId, 0, 0x36, -1, 0xff, 0xff, NULL);
    ERM_fn_vClearLastError(0);
    return NULL;
}

#include "SND/SND.h"
#include "MGT/MGT.h"
#include "GAM/PlayAnim.h"
#include "GAM/ChanList.h"
#include "ERM/CpaError.h"
#include "GAM/GAM_globals.h"
extern void __cdecl fn_v_DynamicFree(void *param_1);   /* 0x412df0 */

void __cdecl fn_vKillAlwaysByPersonalType(tdObjectType otPersonalType)
{
    HIE_tdstSuperObject        *h_SuperObject;
    HIE_tdstSuperObject        *h_Child, *h_NextChild;
    struct tdstEngineObject_   *p_stEngineObject;
    unsigned long               ulRank;
    unsigned long               i;

    ulRank        = otPersonalType - C_AlwaysObjectType;
    h_SuperObject = HIE_fn_hGetElementFromSuperObjectTab(g_stAlways.d_stAlwaysSuperObject, ulRank);

    TFMT_ucClearNumberOfCurrentTextAffiche(h_SuperObject);

    SND_fn_vKillObjectSound2((long)h_SuperObject, GAM_g_lSoundObjectTypeAnim);

    p_stEngineObject = (struct tdstEngineObject_ *)HIE_fn_hGetSuperObjectObject(h_SuperObject);

    if (p_stEngineObject->h_MSMagnet)
        MGT_fn_vDeleteObjectFromMagnetList(h_SuperObject);

    if (p_stEngineObject->h_StandardGame->otObjectModelType == C_InvalidObjectType) {
        ERM_fn_vUpdateLastError(0x30, g_ucGameModuleId, 0, 0x48, -1, 0, 0, NULL);
        ERM_fn_vClearLastError(0);
    }

    g_stAlways.d_hAlwaysGenerator[ulRank] = NULL;

    fn_vRemoveObjectInSectorList(h_SuperObject);

    p_stEngineObject->h_StandardGame->otObjectFamilyType   = C_InvalidObjectType;
    p_stEngineObject->h_StandardGame->otObjectModelType    = C_InvalidObjectType;
    p_stEngineObject->h_StandardGame->otObjectPersonalType = C_AlwaysObjectType;

    PLA_fn_vDesInitAllChildOfCharacter(
        h_SuperObject,
        (struct tdstEngineObject_ *)HIE_fn_hGetSuperObjectObject(h_SuperObject));
    fn_vFreeMorphList(
        (CHN_tdxHandleToMorphList *)&(((MS_tdxHandleTo3dData)p_stEngineObject->h_3dData)->hMorphChannelList));

    {
        void *h_Dynam = p_stEngineObject->h_Dynam;
        if (h_Dynam != NULL)
            fn_v_DynamicFree(h_Dynam);
    }

    HIE_fn_vPseudoIsolate(h_SuperObject);

    if (fn_ucStandardGameGetPlatFormType(M_GetMSHandle(h_SuperObject, StandardGame))) {
        h_Child     = h_SuperObject->p_stFirstChild;
        h_NextChild = HIE_fn_hForceGetSuperObjectNextBrother(h_Child);
        for (; h_Child != NULL;
               h_Child = h_NextChild,
               h_NextChild = HIE_fn_hForceGetSuperObjectNextBrother(h_NextChild)) {
            if (h_Child->ulTypeOfLinkedObject == HIE_C_ulActor)
                fn_vReputCharacterSuperObjectAtTheWorld(h_Child);
        }
    }

    if (h_SuperObject->ulFlags & HIE_C_Flag_Superimposed)
    {
        HIE_fn_SO_vClearSuperimposedFlag(h_SuperObject);
    }

    memset(p_stEngineObject, 0, sizeof(struct tdstEngineObject_));
}

/* 0x0040C2A0 */
unsigned long __cdecl fn_ulGetNumberOfAlwaysBelongingToMe(HIE_tdstSuperObject *_hGenerator)
{

	unsigned long i;
	unsigned long ulReturn = 0;
	tdstEngineObject *hAlwaysObject;

	for (i = 0 ; i < g_stAlways.ulMaxNumberOfAlways ; i ++)
	{
		hAlwaysObject = HIE_fn_hGetSuperObjectObject(HIE_fn_hGetElementFromSuperObjectTab(g_stAlways.d_stAlwaysSuperObject,i));
		if
			(
			(hAlwaysObject != NULL)
			&& (hAlwaysObject->h_3dData != NULL)
			&& (g_stAlways.d_hAlwaysGenerator[i] == _hGenerator)
			)
		{
			ulReturn ++;
		}
	}
	return ulReturn;
}

#include "GAM/GAM_types.h"
#include "HIE/HieSpObj.h"

typedef void *MS_tdxHandleToSectInfo;

extern void *__cdecl fn_h_SectInfoGetCurrentSector(void *h_SectInfo);              /* 0x419b90 */
extern void  __cdecl fn_vSectInfoSetCurrentSector(void *h_SectInfo, void *hSec);   /* 0x42db20 */
extern void  __cdecl fn_vInsertObjectInSectorList(HIE_tdstSuperObject *hSector,
                                                  HIE_tdstSuperObject *hCharacter);/* 0x412740 */

#define GAM_M_pc_hMainCharacterSO   GAM_pc_g_hMainCharacterSO

void fn_vKillAllAlways(void)
{

	unsigned long				i;
	struct tdstEngineObject_	*p_stEngineObject;
	HIE_tdstSuperObject			*hSO;

	for (i=0;i<g_stAlways.ulMaxNumberOfAlways;i++)
	{
		hSO = HIE_fn_hGetElementFromSuperObjectTab(g_stAlways.d_stAlwaysSuperObject,i);
		if ( HIE_fn_SO_bIsSuperimposed( hSO ) )
    {
			HIE_fn_SO_vClearSuperimposedFlag( hSO );

		  fn_vSectInfoSetCurrentSector(
			  M_GetMSHandle(hSO,SectInfo),
			  fn_h_SectInfoGetCurrentSector(M_GetMSHandle(GAM_M_pc_hMainCharacterSO,SectInfo))
      );
		  fn_vInsertObjectInSectorList(fn_h_SectInfoGetCurrentSector(M_GetMSHandle(hSO,SectInfo)),hSO);
    }

		p_stEngineObject=(struct tdstEngineObject_ *)HIE_fn_hGetSuperObjectObject( hSO );

		if (p_stEngineObject->h_StandardGame!=NULL)
			fn_vKillAlwaysByPersonalType(M_ObjectGetPersonalType(p_stEngineObject));
	}
}

/* 0x0040C370 */
unsigned char fn_ucIsAlwaysGeneratedByMe(HIE_tdstSuperObject *_hAlways, HIE_tdstSuperObject *_hGenerator)
{
    struct tdstEngineObject_ *p_stEngineObject;
    unsigned long             ulRank;

    if
    (
           (_hGenerator == NULL)
        || (_hAlways == NULL)
        || (M_GetMSHandle(_hAlways, StandardGame) == NULL)
        || ((p_stEngineObject = (struct tdstEngineObject_ *)HIE_fn_hGetSuperObjectObject(_hAlways)) == NULL)
    )
        return 0;

    ulRank = p_stEngineObject->h_StandardGame->otObjectPersonalType - C_AlwaysObjectType;

    return (unsigned char)(g_stAlways.d_hAlwaysGenerator[ulRank] == (void *)_hGenerator);
}

/* 0x0040BB50 */
void fn_vLastDesinitAlwaysStructure(void)
{
    fn_vKillAllAlways();

    if (g_stAlways.ulMaxNumberOfAlways)
    {
        g_stAlways.d_stAlwaysEngineObjectInit = NULL;
        g_stAlways.d_stAlwaysSuperObject      = NULL;
        g_stAlways.d_hAlwaysGenerator         = NULL;
    }
}
