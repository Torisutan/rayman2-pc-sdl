#include "GAM/GAM_types.h"
#include "SCT/Sector.h"
#include "MTH/MTH3D.h"
#include "POS/POS_types.h"
#include "GAM/Always.h"
#include "GAM/InitEnum.h"
#include "ERM/CpaError.h"
#include "GAM/MSlight.h"

typedef struct tdstSectInfo_
{

   HIE_tdstSuperObject *hCurrentSector;
   SECT_tdxHandleOfElementLstCharacter hNodeInSector;

   MTH3D_tdstVector stPreviousPosition;
} tdstSectInfo;
typedef struct tdstSectInfo_ * MS_tdxHandleToSectInfo;

/* 0x004FF764 */
HIE_tdstSuperObject *g_hCharacterLauchingSoundEvents;

extern void *__cdecl fn_hMSLightGetLightNodeInSector(void *_hMSLight);

void __cdecl fn_vSectInfoSetCurrentSector(void *h_SectInfo, void *hCurrentSector)
{
    *(void **)h_SectInfo = hCurrentSector;
}

extern unsigned char __cdecl fn_bMSLightGetLocalLight(void *h_MSLight);       /* 0x416220 */
extern unsigned char __cdecl fn_bMSLightGetOnlyLocalLight(void *h_MSLight);   /* 0x416240 */

extern HIE_tdstSuperObject *SECT_hFatherSector;
HIE_tdstSuperObject *SECT_fn_hResearchInWhatSectorIAm(HIE_tdstSuperObject *_hFather, MTH3D_tdstVector *_p_stAbsolutePosition);
HIE_tdstSuperObject *HIE_fn_hGetChildNumber(HIE_tdstSuperObject *_hParent, long _lNumber);
void *fn_p_stMSLightGetLight(void *h_MSLight);
ACP_tdxBool __cdecl fn_bIsInFix(struct tdstEngineObject_ *p_stObject);
unsigned char FUN_0044f4e0(void);                                                  /* 0x5116EC */

void __cdecl SECT_fn_vSetCharacterPointerInCharacterList(void *_hCharacterElement, void *_hCharacter)
{
    *(void **)_hCharacterElement = _hCharacter;
}

void __cdecl SECT_fn_vSetHandleOfLightsInDNMLightsList(void *_hDNMLightsElement, void *_hDNMLights)
{
    *(void **)_hDNMLightsElement = _hDNMLights;
}

extern HIE_tdstSuperObject *__cdecl fn_h_StandardGameGetSuperObject(MS_tdxHandleToStandardGame h_StandardGame);

void __cdecl fn_vSectInfoSavedValueSAI(void *hSectInfo)
{
    (void)hSectInfo;
}

void __cdecl fn_vSectInfoCopyClone(struct tdstEngineObject_ *_p_stObjectDest, struct tdstEngineObject_ *_p_stObjectSrc)
{
    (void)_p_stObjectDest;
    (void)_p_stObjectSrc;
}

#define C_xCorrectionVector MTH_C_Inv5

void __cdecl GAM_pc_fn_vStopMatchingSounds(long lKey, void *p_pObject);
void __cdecl fn_vInitObjectsWhenPlayerChangeSector(long lUnused, void *p_pFirst, void *p_pSecond);
/* 0x419BA0 */
unsigned int GAM_pc_fn_ulStdGameAlwaysActiveViaTwoHops(void *p_stObjectChain);
void __cdecl SCT_fn_vTestChangeSectorWithHistory(void *_p_stBeforeAbsolutePosition,
                                                 void *_p_stAfterAbsolutePosition,
                                                 void **_p_hSectorSOWhereIWas);
void HIE_fn_vIsolate(HIE_tdstSuperObject *_hSprObj);
#include "SCT/SectExt.h"

#include "GAM/PlayAnim.h"
#include "GAM/Macros.h"

static __inline POS_tdstCompletePosition *fn_p_st3dDataGetMatrix(MS_tdxHandleTo3dData h_3dData)
    { return &(h_3dData->stGLIObjectMatrix); }

#define SIF_M_hSectInfo(p) ((MS_tdxHandleToSectInfo)(p)->h_SectInfo)
#define SIF_M_h3dData(p)   ((MS_tdxHandleTo3dData)(p)->h_3dData)

void __cdecl fn_vInsertObjectInSectorList(HIE_tdstSuperObject *hSector, HIE_tdstSuperObject *hCharacter);
void __cdecl fn_vSectInfoInitForSingleCharacter(HIE_tdstSuperObject *hCharacter, MTH3D_tdstVector *hPosition);

unsigned long __cdecl fn_ulSectInfoSizeOf(void)
{
    return 0x14;
}
void *__cdecl GAM_pc_fn_p_stAllocLinkedHolder(void);

void __cdecl fn_vSectInfoAlloc(void *p_pOwner)
{
    if (p_pOwner == (void *)0)
    {
        ERM_fn_vClearLastError(0);
        ERM_fn_vUpdateLastError(0x30, g_ucGameModuleId, 0, 0xe, -1, 0xff, 0xff, (char *)0);
    }
    if (((struct tdstEngineObject_ *)p_pOwner)->h_SectInfo != (void *)0)
    {
        ERM_fn_vClearLastError(0);
        ERM_fn_vUpdateLastError(0x30, g_ucGameModuleId, 0, 0xd, -1, 0xff, 0xff, (char *)0);
    }
    ((struct tdstEngineObject_ *)p_pOwner)->h_SectInfo = GAM_pc_fn_p_stAllocLinkedHolder();
}
void __cdecl fn_vGameFreeInHLM(void *p_vObject);

void __cdecl fn_vSectInfoFree(void *p_pOwner)
{
    if (p_pOwner == (void *)0)
    {
        ERM_fn_vClearLastError(0);
        ERM_fn_vUpdateLastError(0x30, g_ucGameModuleId, 0, 0xe, -1, 0xff, 0xff, (char *)0);
    }
    if (((struct tdstEngineObject_ *)p_pOwner)->h_SectInfo == (void *)0)
    {
        ERM_fn_vClearLastError(0);
        ERM_fn_vUpdateLastError(0x30, g_ucGameModuleId, 0, 0xf, -1, 0xff, 0xff, (char *)0);
    }
    fn_vGameFreeInHLM(((struct tdstEngineObject_ *)p_pOwner)->h_SectInfo);
    ((struct tdstEngineObject_ *)p_pOwner)->h_SectInfo = (void *)0;
}

void __cdecl fn_vSectInfoInit(struct tdstEngineObject_ *p_stObject, enum tdeObjectTreeInit_ eObjectInit)
{
	MTH3D_tdstVector *p_stV;
	HIE_tdstSuperObject *h_SO;
	MS_tdxHandleToMSLight hLight;

	if (p_stObject==NULL)
		M_GameFatalError(E_uwGameMiniStructObjectNull);

	if (p_stObject->h_SectInfo==NULL)
		M_GameFatalError(E_uwGameMiniStructNull);

	/* 0x00412473 */
	if ((fn_bIsInFix(p_stObject) == 0 && FUN_0044f4e0() == 2) ||
	    (fn_bIsInFix(p_stObject) != 0 && FUN_0044f4e0() == 4) ||
	    (FUN_0044f4e0() == 1 || FUN_0044f4e0() == 0))
	{

		switch(eObjectInit)
		{
		case OTI_MapLoaded:
			SIF_M_hSectInfo(p_stObject)->hCurrentSector = NULL;
			fn_vSectInfoSavedValueSAI(SIF_M_hSectInfo(p_stObject));
		case OTI_LoadSavedGame:
		case OTI_ReinitTheMap:
		case OTI_PlayerDead:
			if (M_GetSuperObject(p_stObject)!=NULL)
			{
				p_stV = POS_fn_p_stGetTranslationVector(fn_p_st3dDataGetMatrix(SIF_M_h3dData(p_stObject)));
				fn_vSectInfoInitForSingleCharacter(M_GetSuperObject(p_stObject),p_stV);
				MTH3D_M_vCopyVector(&SIF_M_hSectInfo(p_stObject)->stPreviousPosition,p_stV);
			}
			break;

		case OTI_AlwaysCreated:
			if (M_GetSuperObject(p_stObject)!=NULL)
			{
				h_SO  = M_GetSuperObject(p_stObject);
				p_stV = POS_fn_p_stGetTranslationVector (HIE_fn_hGetSuperObjectGlobalMatrix(h_SO));
				fn_vInsertObjectInSectorList(SIF_M_hSectInfo(p_stObject)->hCurrentSector,h_SO);
				SECT_fn_vSetCharacterPointerInCharacterList(SIF_M_hSectInfo(p_stObject)->hNodeInSector, h_SO);
				MTH3D_M_vCopyVector(&SIF_M_hSectInfo(p_stObject)->stPreviousPosition,p_stV);
				if( (hLight = M_GetMSHandle(h_SO,MSLight)) != 0 )
				{
					SECT_fn_vSetHandleOfLightsInDNMLightsList(
						fn_hMSLightGetLightNodeInSector(hLight),
						fn_p_stMSLightGetLight(hLight)
					);
				}
			}
			break;

		default:
			break;
		}
	}
}

void __cdecl fn_vRemoveObjectInSectorList(HIE_tdstSuperObject *hCharacter);
void __cdecl fn_vSectInfoDesinit(struct tdstEngineObject_ *p_stObject, long eObjectInit)
{
    if (p_stObject == NULL)
    {
        ERM_fn_vClearLastError((unsigned long)p_stObject);
        ERM_fn_vUpdateLastError(0x30, g_ucGameModuleId, (unsigned long)p_stObject, 0x0e, -1,
                                0xff, 0xff, (char *)p_stObject);
    }

    if (p_stObject->h_SectInfo == NULL)
    {
        ERM_fn_vClearLastError(0);
        ERM_fn_vUpdateLastError(0x30, g_ucGameModuleId, 0, 0x0f, -1, 0xff, 0xff, (char *)0);
    }

    switch (eObjectInit)
    {
    case OTI_MapLoaded:
    case OTI_ReinitTheMap:
    case OTI_LoadSavedGame:
    case OTI_AlwaysCreated:
    case OTI_PlayerDead:
        if (fn_h_StandardGameGetSuperObject(p_stObject->h_StandardGame))
        {
            fn_vRemoveObjectInSectorList(fn_h_StandardGameGetSuperObject(p_stObject->h_StandardGame));
        }
        break;
    default:
        break;
    }
}

void __cdecl fn_vSectInfoInitForSingleCharacter(HIE_tdstSuperObject *hCharacter, MTH3D_tdstVector *hPosition)
{
    HIE_tdstSuperObject *testSO;
    MS_tdxHandleToSectInfo hSectInfo;
    MS_tdxHandleToMSLight hLight;

    /* 0x00412670 */
    if ((fn_bIsInFix((struct tdstEngineObject_ *)HIE_fn_hGetSuperObjectObject(hCharacter)) != 0
            || FUN_0044f4e0() != 2)
        && (fn_bIsInFix((struct tdstEngineObject_ *)HIE_fn_hGetSuperObjectObject(hCharacter)) == 0
            || FUN_0044f4e0() != 4)
        && (FUN_0044f4e0() != 1 && FUN_0044f4e0() != 0))
        return;

    hSectInfo = M_GetMSHandle(hCharacter,SectInfo);

    testSO = SECT_fn_hResearchInWhatSectorIAm(SECT_hFatherSector, hPosition);
    hSectInfo->hCurrentSector = testSO ? testSO
        : HIE_fn_hGetChildNumber(SECT_hFatherSector, SECT_hFatherSector->lNumberOfChildren - 1);

    SECT_fn_vSetCharacterPointerInCharacterList(hSectInfo->hNodeInSector, hCharacter);

    if( (hLight = M_GetMSHandle(hCharacter,MSLight)) != 0 )
    {
        SECT_fn_vSetHandleOfLightsInDNMLightsList(fn_hMSLightGetLightNodeInSector(hLight),
            fn_p_stMSLightGetLight(hLight));
    }

    fn_vInsertObjectInSectorList(hSectInfo->hCurrentSector, hCharacter);
}

void __cdecl SECT_fn_vIsolateCharListNode(void *_hNode);
void __cdecl fn_vInsertLightInSectorList(HIE_tdstSuperObject *hSector, HIE_tdstSuperObject *hCharacter);

/* 0x00412740 */
void __cdecl fn_vInsertObjectInSectorList(HIE_tdstSuperObject *hSector, HIE_tdstSuperObject *hCharacter)
{

    tdstSectInfo * p_stSectInfo;

    p_stSectInfo = M_GetMSHandle(hCharacter,SectInfo);
    SECT_fn_vIsolateCharListNode(p_stSectInfo->hNodeInSector);

    if( fn_ucStandardGameGetPlatFormType(M_GetMSHandle(hCharacter,StandardGame))!=0 )
        SECT_fn_vAddHeadCharListNode(hSector,p_stSectInfo->hNodeInSector);
    else
        SECT_fn_vAddTailCharListNode(hSector,p_stSectInfo->hNodeInSector);

    if( M_GetMSHandle(hCharacter,MSLight) )
        fn_vInsertLightInSectorList(hSector,hCharacter);
}

/* 0x004127a0 */
void __cdecl fn_vInsertLightInSectorList(HIE_tdstSuperObject *hSector, HIE_tdstSuperObject *hCharacter)
{
    MS_tdxHandleToMSLight hMSLight;

    hMSLight = M_GetMSHandle(hCharacter,MSLight);

    if (fn_bMSLightGetLocalLight(hMSLight))
    {

        if (fn_bMSLightGetOnlyLocalLight(hMSLight))
            hCharacter->ulFlags |= HIE_C_Flag_ExcluLight;
        else
            hCharacter->ulFlags &= ~HIE_C_Flag_ExcluLight;
        return;
    }
    SECT_fn_vIsolateCharListNode(fn_hMSLightGetLightNodeInSector(hMSLight));
    SECT_fn_vAddTailDNMLightsListNode(hSector,
        (SECT_tdxHandleOfElementLstDNMLights)fn_hMSLightGetLightNodeInSector(hMSLight));
}

void __cdecl fn_vRemoveObjectInSectorList(HIE_tdstSuperObject *hCharacter)
{
    SECT_fn_vIsolateCharListNode(M_GetMSHandle(hCharacter,SectInfo)->hNodeInSector);
    if( M_GetMSHandle(hCharacter,MSLight) )
    {
        SECT_fn_vIsolateCharListNode(fn_hMSLightGetLightNodeInSector(M_GetMSHandle(hCharacter,MSLight)));
    }
}

void *__cdecl fn_h_SectInfoGetCurrentSector(void *h_SectInfo);

/* 0x00412850 */
HIE_tdstSuperObject *__cdecl GAM_fn_hGetCurrentSector(HIE_tdstSuperObject *_hCharacter)
{
    if (_hCharacter->ulTypeOfLinkedObject != HIE_C_ulActor)
        return NULL;
    if (!((struct tdstEngineObject_ *)HIE_fn_hGetSuperObjectObject(_hCharacter))->h_SectInfo)
        return NULL;
    return fn_h_SectInfoGetCurrentSector(((struct tdstEngineObject_ *)HIE_fn_hGetSuperObjectObject(_hCharacter))->h_SectInfo);
}
#include "SND/SND.h"
#include "GAM/GAM_globals.h"

void __cdecl GAM_pc_fn_vStopMatchingSounds(long lKey, void *p_pObject)
{
    SECT_tdxHandleOfSectorObject pStruct;
    SCT_tdxHandleOfLstSoundEvent plNode;
    unsigned long ulCount;

    pStruct = (SECT_tdxHandleOfSectorObject)((HIE_tdstSuperObject *)p_pObject)->hLinkedObject.p_Void;
    ulCount = LST2_M_StaticGetNumberOfElements(&pStruct->stListOfSoundEvent);
    plNode = LST2_M_StaticGetFirstElement(&pStruct->stListOfSoundEvent);
    for (; ulCount > 0; ulCount--)
    {
        if ((long)plNode->hPointerOfSectorSO == lKey || plNode->hPointerOfSectorSO == NULL)
            SND_fn_lSendRequestSound(-1, GAM_g_lSoundObjectTypeMusic, plNode->uSoundEvent, 0, (void *)0);
        plNode = LST2_M_StaticGetNextElement(plNode);
    }
}

void GAM_fn_vSectInfoTestChangeSectorForCharacter(HIE_tdstSuperObject *_hCharacter)
{
	tdstSectInfo * p_stSectInfo;
	MTH3D_tdstVector stCurrentPosition;
	HIE_tdstSuperObject *hSector,*hPreviousSector;

	MTH3D_tdstVector stCurrentRaisedPosition,stPreviousRaisedPosition;
	MTH3D_tdstVector stAdding;

	MTH3D_tdstVector a2_stColons[2];

	POS_fn_vGetRotationMatrix(HIE_fn_hGetSuperObjectGlobalMatrix(_hCharacter),a2_stColons,a2_stColons+1,&stAdding);
	MTH3D_M_vMulScalarVector(&stAdding,C_xCorrectionVector,&stAdding);

	p_stSectInfo = (tdstSectInfo *)M_GetMSHandle(_hCharacter,SectInfo);
	POS_fn_vGetTranslationVector(HIE_fn_hGetSuperObjectGlobalMatrix(_hCharacter),&stCurrentPosition);
	hSector = p_stSectInfo->hCurrentSector;
	hPreviousSector = p_stSectInfo->hCurrentSector;
	MTH3D_M_vCopyVector(&stCurrentRaisedPosition,&stCurrentPosition);
	MTH3D_M_vCopyVector(&stPreviousRaisedPosition,&p_stSectInfo->stPreviousPosition);
	MTH3D_M_vAddVector(&stCurrentRaisedPosition,&stCurrentRaisedPosition,&stAdding);
	MTH3D_M_vAddVector(&stPreviousRaisedPosition,&stPreviousRaisedPosition,&stAdding);
	SCT_fn_vTestChangeSectorWithHistory(
		&stPreviousRaisedPosition,
		&stCurrentRaisedPosition,
		(void **)&hSector);
	if( hSector!=p_stSectInfo->hCurrentSector )
	{
		ACP_tdxBool bOnInactivedSector;

		if( _hCharacter == g_hCharacterLauchingSoundEvents)
			GAM_pc_fn_vStopMatchingSounds((long)p_stSectInfo->hCurrentSector,hSector);
		p_stSectInfo->hCurrentSector = hSector;

		if( !fn_ucIsAnAlwaysObject((struct tdstEngineObject_ *)HIE_fn_hGetSuperObjectObject(_hCharacter)) &&
		    !(unsigned char)GAM_pc_fn_ulStdGameAlwaysActiveViaTwoHops(_hCharacter) )
		{
			bOnInactivedSector = (ACP_tdxBool) (p_stSectInfo->hCurrentSector != g_hSOCurrentMainSector);
			if( bOnInactivedSector )
			{
				SECT_tdxHandleOfElementLstActivityInteraction hActivityList;
				HIE_tdstSuperObject *hChildSector;
				long i;

				SECT_M_ForEachActivityNodeInActivityInteractionList(g_hSOCurrentMainSector,hActivityList,i)
				{
					hChildSector = SECT_GetSectorInActivityList(hActivityList);
					if( hChildSector == p_stSectInfo->hCurrentSector )
					{
						bOnInactivedSector = FALSE;
						break;
					}
				}
				if( bOnInactivedSector )
				{
					HIE_fn_vIsolate(_hCharacter);
					if( fn_ucStandardGameGetPlatFormType(M_GetMSHandle(_hCharacter,StandardGame))!=0 )
					{
						HIE_fn_vSuperObjectAddHead(gp_stInactiveDynamicWorld,_hCharacter);
					}
					else
					{
						HIE_fn_vSuperObjectAddTail(gp_stInactiveDynamicWorld,_hCharacter);
					}
          M_UnactivateObject((struct tdstEngineObject_*)HIE_fn_hGetSuperObjectObject(_hCharacter));
				}
			}
		}

		fn_vInsertObjectInSectorList(
			p_stSectInfo->hCurrentSector,_hCharacter);
		if( _hCharacter==GAM_pc_g_hMainCharacterSO )
			fn_vInitObjectsWhenPlayerChangeSector((long)_hCharacter,p_stSectInfo->hCurrentSector,hPreviousSector);
	}
	MTH3D_M_vCopyVector(&p_stSectInfo->stPreviousPosition,&stCurrentPosition);
}

/* 0x00419B90 */
void *__cdecl fn_h_SectInfoGetCurrentSector(void *h_SectInfo)
{
    return *(void **)h_SectInfo;
}
