#include "GAM/GAM_types.h"
#include "ALW/ALW_globals.h"
#include "HIE/HIE_types.h"
#include "POS/POS_types.h"
#include "MTH/MTH3D.h"
#include "MTH/MTH_flt.h"
#include "ERM/CpaError.h"
#include "SND/SND.h"
#include "MGT/MGT.h"
#include "GAM/PlayAnim.h"
#include "GAM/ChanList.h"
#include "GLI/Material.h"
#include "GLI/Texture.h"
#include "GLI/misc_trivial.h"
#include "GLI/RasterDefaults.h"
#include "GLI/GLD.h"
#include "GLI/light_fog.h"
#include "AGO/AGO.h"
#include "MOR/MOR.h"
#include "GAM/GAM_globals.h"
#include "SCT/SectExt.h"
#include "GAM/AlwAct.h"

extern void *__cdecl fn_p_stDynamGetDynamics(void *h_Dynam);
/* 0x0042DE30 */
extern void *__cdecl fn_p_stDynamGetParsingDatas(void *h_Dynam);
/* 0x0042DE30 */
extern void *__cdecl fn_p_stGetAnimInState(void *hState);
/* 0x00403290 */
extern void __cdecl CAM_vDisplayInfo(void);
/* 0x00419B90 */
extern unsigned long __cdecl DNM_fn_eIdentityCardGetType(void *);

/* 0x0040b060 */
extern HIE_tdstSuperObject *__cdecl HIE_fn_hForceGetSuperObjectNextBrother(HIE_tdstSuperObject *);
extern void __cdecl HIE_fn_vChangeFather(HIE_tdstSuperObject *, HIE_tdstSuperObject *, unsigned char);

#define GAM_M_pc_hMainCharacterSO   GAM_pc_g_hMainCharacterSO

static void fn_vRecurseDestructChildLink(HIE_tdstSuperObject *_hSuperObjRoot)
{
    HIE_tdstSuperObject *hChild, *hNextChild, *hPlatformChild, *hPlatformNextChild;

    for (hChild = _hSuperObjRoot->p_stFirstChild,
         hNextChild = HIE_fn_hForceGetSuperObjectNextBrother(hChild);
         hChild != (HIE_tdstSuperObject *)0;
         hChild = hNextChild,
         hNextChild = HIE_fn_hForceGetSuperObjectNextBrother(hChild))
    {
        if (hChild->ulTypeOfLinkedObject == 2  )
        {
            if (fn_ucStandardGameGetPlatFormType(M_GetMSHandle(hChild, StandardGame)) == 0)
                break;

            for (hPlatformChild = hChild->p_stFirstChild,
                 hPlatformNextChild = HIE_fn_hForceGetSuperObjectNextBrother(hPlatformChild);
                 hPlatformChild != (HIE_tdstSuperObject *)0;
                 hPlatformChild = hPlatformNextChild,
                 hPlatformNextChild = HIE_fn_hForceGetSuperObjectNextBrother(hPlatformChild))
            {
                if (hPlatformChild->ulTypeOfLinkedObject == 2  )
                    HIE_fn_vChangeFather(hPlatformChild, _hSuperObjRoot, 0  );
            }
        }
    }
}

/* 0x0040b0e0 */
void fn_vDestructAllFatherSonLinks(void)
{
    HIE_tdstSuperObject *hChild, *hNextChild, *hMainCharacter;

    hMainCharacter = GAM_M_pc_hMainCharacterSO;

    for (hChild = hMainCharacter->p_stFirstChild,
         hNextChild = HIE_fn_hForceGetSuperObjectNextBrother(hChild);
         hChild != (HIE_tdstSuperObject *)0;
         hChild = hNextChild,
         hNextChild = HIE_fn_hForceGetSuperObjectNextBrother(hChild))
    {
        if (hChild->ulTypeOfLinkedObject == 2  )
            HIE_fn_vChangeFather(hChild, gp_stDynamicWorld,
                                 fn_ucStandardGameGetPlatFormType(M_GetMSHandle(hChild, StandardGame)));
    }

    fn_vRecurseDestructChildLink(gp_stDynamicWorld);
    fn_vRecurseDestructChildLink(gp_stInactiveDynamicWorld);
}

/* 0x005010E8 */
char GAM_pc_gs_aszUiSurfaceName[0x2E0];
#define GAM_pc_g_szUiSurfaceName (GAM_pc_gs_aszUiSurfaceName)

void fn_vInsertActorInDynamicHierarchy(HIE_tdstSuperObject *_hCharacter, ACP_tdxBool _bNewInsert)
{
    struct tdstEngineObject_ *p_stObject;
    p_stObject = (struct tdstEngineObject_ *)HIE_fn_hGetSuperObjectObject(_hCharacter);

    if (M_ObjectIsActivable(p_stObject)
        &&
        (
          (_bNewInsert
            &&
            (
              !fn_bf1StandardGameGetIsDesactivateAtAll(p_stObject->h_StandardGame)
              ||
              (fn_eStandardGameGetInitFlagWhenDeadOrTaken(p_stObject->h_StandardGame) < OI_WhenPlayerIsDead)
            )
          )
          ||
          (
            !fn_bf1StandardGameGetIsDesactivateAtAll(p_stObject->h_StandardGame)
          )
        )
      )
    {
        if (HIE_fn_hGetSuperObjectFather(_hCharacter) == gp_stInactiveDynamicWorld)
        {
            if (fn_ucStandardGameGetPlatFormType(M_GetMSHandle(_hCharacter, StandardGame)) != 0)
            {
                HIE_fn_vSuperObjectAddHead(gp_stDynamicWorld, _hCharacter);
            }
            else
            {
                HIE_fn_vSuperObjectAddTail(gp_stDynamicWorld, _hCharacter);
            }
            M_ActivateObject(p_stObject);
            if (_bNewInsert)
            {
                MS_tdxHandleToStandardGame hStdGame = M_GetMSHandle(_hCharacter, StandardGame);
                fn_vStandardGameSetCustomBits(hStdGame, fn_ulStandardGameGetCustomBits(hStdGame) | GAM_C_CustBitOutOfVisibility);
            }
        }
    }
}

void fn_vFillDynamicHierarchy(HIE_tdstSuperObject *_hSOCurrentSector, ACP_tdxBool _bInit)
{
    long                                            i,j;
    SECT_tdxHandleOfElementLstCharacter             hCharacterList;
    SECT_tdxHandleOfElementLstActivityInteraction   hActivityList;
    HIE_tdxHandleToSuperObject                      hCharacter;
    HIE_tdxHandleToSuperObject                      hChildSector;
    tdxHandleToAlwaysActiveCharacterNode            hCharAlwaysActiveNode;
    HIE_tdxHandleToSuperObject                      *p_hGenerator;
    HIE_tdxHandleToSuperObject                      *p_hLastGenerator;
    ACP_tdxIndex                                    xIndex , xIndexMax;
    char                                            cCounter;
    ACP_tdxBool                                     bNewSector;

    cCounter = (char)GAM_g_ulEngineTrameNumber;

    bNewSector = (ACP_tdxBool) (_bInit || ( SECT_cGetCounterInSector(_hSOCurrentSector) != cCounter ));

    SECT_M_ForEachCharListInSector(_hSOCurrentSector,hCharacterList,i)
    {
        hCharacter = SECT_GetCharacterInList(hCharacterList);
        fn_vInsertActorInDynamicHierarchy(hCharacter , bNewSector);
    }

    SECT_M_ForEachActivityNodeInActivityInteractionList(_hSOCurrentSector,hActivityList,i)
    {
        hChildSector = SECT_GetSectorInActivityList(hActivityList);
        bNewSector = (ACP_tdxBool) (_bInit || ( SECT_cGetCounterInSector(hChildSector) != cCounter ));
        SECT_M_ForEachCharListInSector(hChildSector,hCharacterList,j)
        {
            hCharacter = SECT_GetCharacterInList(hCharacterList);
            fn_vInsertActorInDynamicHierarchy(hCharacter , bNewSector);
        }
    }

    M_ForEachAlwaysActiveCharacterNode(hCharAlwaysActiveNode,i)
    {
        hCharacter = fn_hGetCharacterInAlwaysActiveCharacterNode(hCharAlwaysActiveNode);
        fn_vInsertActorInDynamicHierarchy(hCharacter , _bInit);
    }

    hCharacter = g_stAlways.d_stAlwaysSuperObject;
    p_hGenerator = g_stAlways.d_hAlwaysGenerator;
    p_hLastGenerator = p_hGenerator + g_stAlways.ulMaxNumberOfAlways;
    for( ; p_hGenerator < p_hLastGenerator; hCharacter++, p_hGenerator++ )
    {
        if( M_GetMSHandle(hCharacter, StandardGame) )
        {

            bNewSector = (ACP_tdxBool) (_bInit ||
                        (   ((struct tdstEngineObject_ *)HIE_fn_hGetSuperObjectObject(hCharacter))->h_SectInfo &&
                            fn_h_SectInfoGetCurrentSector(((struct tdstEngineObject_ *)HIE_fn_hGetSuperObjectObject(hCharacter))->h_SectInfo) &&
                            (SECT_cGetCounterInSector(fn_h_SectInfoGetCurrentSector(((struct tdstEngineObject_ *)HIE_fn_hGetSuperObjectObject(hCharacter))->h_SectInfo)) != cCounter) ));

            fn_vInsertActorInDynamicHierarchy(hCharacter , bNewSector);
        }
    }

    xIndexMax = HIE_g_xFixIndex;
    for ( xIndex=0 ; xIndex<xIndexMax ; xIndex++)
    {
        HIE_tdxHandleToSuperObject hFixSPO ;

        hFixSPO = HIE_fn_hGetElementFormFixViewport (xIndex) ;
        fn_vInsertActorInDynamicHierarchy(hFixSPO , _bInit);
    }
}

void fn_vDesinitInactivePerso()
{
	HIE_tdxHandleToSuperObject hCharacter,hChild;

	for (hCharacter = gp_stInactiveDynamicWorld->p_stFirstChild; hCharacter != NULL; hCharacter = hCharacter->p_stNextBrother)
	{
		if (M_ObjectIsActive((struct tdstEngineObject_*)HIE_fn_hGetSuperObjectObject(hCharacter)))
		{
			if( fn_ucStandardGameGetPlatFormType(M_GetMSHandle(hCharacter,StandardGame))!=0 )
			{
				for (hChild = hCharacter->p_stFirstChild; hChild != NULL; hChild = hChild->p_stNextBrother)
				{
					if (hChild->ulTypeOfLinkedObject == HIE_C_ulActor)
					{
						PLA_fn_vDesInitAllChildOfCharacter(hChild,(struct tdstEngineObject_*)HIE_fn_hGetSuperObjectObject(hChild));
						M_UnactivateObject((struct tdstEngineObject_*)HIE_fn_hGetSuperObjectObject(hChild));
						SND_fn_vKillObjectSound2((long)hChild,GAM_g_lSoundObjectTypeAnim);
					}
				}
			}
			PLA_fn_vDesInitAllChildOfCharacter(hCharacter,(struct tdstEngineObject_*)HIE_fn_hGetSuperObjectObject(hCharacter));
			M_UnactivateObject((struct tdstEngineObject_*)HIE_fn_hGetSuperObjectObject(hCharacter));
			SND_fn_vKillObjectSound2((long)hCharacter,GAM_g_lSoundObjectTypeAnim);
		}
	}
}

void fn_vClearDynamicHierarchy(HIE_tdstSuperObject *_hSOCurrentSector)
{
    HIE_tdstSuperObject *hCharacter;
    HIE_tdstSuperObject *hNextCharacter;
    HIE_tdstSuperObject *hChildSector;
    SECT_tdxHandleOfElementLstActivityInteraction hActivityList;
    long                 i;
    char                 cCounter;

    for (hCharacter = gp_stDynamicWorld->p_stFirstChild,
         hNextCharacter = HIE_fn_hForceGetSuperObjectNextBrother(hCharacter);
         hCharacter != (HIE_tdstSuperObject *)0;
         hCharacter = hNextCharacter,
         hNextCharacter = HIE_fn_hForceGetSuperObjectNextBrother(hCharacter))
    {
        if ((hCharacter->ulTypeOfLinkedObject == HIE_C_ulActor) &&
            !CAM_fn_bSuperObjectIsACamera(hCharacter))
        {
            HIE_fn_vIsolate(hCharacter);
            if (fn_ucStandardGameGetPlatFormType(M_GetMSHandle(hCharacter, StandardGame)) != 0)
            {
                HIE_fn_vSuperObjectAddHead(gp_stInactiveDynamicWorld, hCharacter);
            }
            else
            {
                HIE_fn_vSuperObjectAddTail(gp_stInactiveDynamicWorld, hCharacter);
            }
        }
    }

    if (_hSOCurrentSector)
    {
        cCounter = (char)GAM_g_ulEngineTrameNumber;

        ((SECT_tdxHandleOfSectorObject)HIE_fn_hGetSuperObjectObject(_hSOCurrentSector))->cCounter = cCounter;

        for (i = LST2_M_StaticGetNumberOfElements(&((SECT_tdxHandleOfSectorObject)HIE_fn_hGetSuperObjectObject(_hSOCurrentSector))->stListOfSectorsInActivityInteraction),
             hActivityList = LST2_M_StaticGetFirstElement(&((SECT_tdxHandleOfSectorObject)HIE_fn_hGetSuperObjectObject(_hSOCurrentSector))->stListOfSectorsInActivityInteraction);
             i > 0;
             i--, hActivityList = LST2_M_StaticGetNextElement(hActivityList))
        {
            hChildSector = hActivityList->hPointerOfSectorSO;
            ((SECT_tdxHandleOfSectorObject)hChildSector->hLinkedObject.p_Void)->cCounter = cCounter;
        }
    }
}

void *__cdecl fn_h_SectInfoGetCurrentSector(void *h_SectInfo);   /* 0x00419B90 */

void fn_vUpdateDynamicHierarchy(void)
{
    HIE_tdstSuperObject *hActualSector;
    HIE_tdstSuperObject *hOldSector;
    ACP_tdxBool          bInit;

    /* 0x00419B90 */
    hActualSector = (HIE_tdstSuperObject *)fn_h_SectInfoGetCurrentSector(
        ((struct tdstEngineObject_ *)HIE_fn_hGetSuperObjectObject(GAM_pc_g_hMainCharacterSO))->h_SectInfo);

    if (hActualSector != g_hSOCurrentMainSector)
    {
        bInit = (ACP_tdxBool)(g_hSOCurrentMainSector == NULL);

        hOldSector              = g_hSOCurrentMainSector;
        g_hSOPreviousMainSector = hOldSector;
        g_hSOCurrentMainSector  = hActualSector;

        fn_vClearDynamicHierarchy(hOldSector);

        fn_vFillDynamicHierarchy(g_hSOCurrentMainSector, bInit);

        fn_vDesinitInactivePerso();
    }
}

#define GAM_g_hMainCharacterSO   (GAM_g_stEngineStructure.g_hMainActor)

#define GAM_g_cCameraMode        (GAM_g_stEngineStructure.cCameraMode)             /* 0x500FAC */
#define C_CamModeLook            1
#define GAM_g_ulDisplayMode      (GAM_g_stEngineStructure.ulDisplayMode)           /* 0x5003E0 */
#define GAM_g_p_stViewportAttr   ((void *)GAM_g_stEngineStructure.stViewportAttr)

#define DNM_CAM_C_NoObstacle     0x0040

/* 0x0040E800 */
ACP_tdxBool __cdecl PLA_fn_bSkipCurrentFrame(HIE_tdstSuperObject *hSuperObj);

/* 0x0040C450 */
extern void __cdecl fn_vWaterStreakEffect(void *_p_stViewportAttr, void *_hCharacter, long _lDrawMask);
/* 0x004149A0 */
extern void __cdecl fn_vMSPrtSrcSourceGenerateParticles(HIE_tdstSuperObject *_hCharacter);

/* 0x0042DB20 */
extern void __cdecl fn_vSectInfoSetCurrentSector(void *h_SectInfo, void *hCurrentSector);
/* 0x00412740 */
extern void __cdecl fn_vInsertObjectInSectorList(HIE_tdstSuperObject *hSector, HIE_tdstSuperObject *hCharacter);
/* 0x004128D0 */
void GAM_fn_vSectInfoTestChangeSectorForCharacter(HIE_tdstSuperObject *_hCharacter);

#define GAM_p_h3dData(h)    ((MS_tdxHandleTo3dData)(((struct tdstEngineObject_ *)HIE_fn_hGetSuperObjectObject(h))->h_3dData))
#define GAM_h_SectInfo(h)   (((struct tdstEngineObject_ *)HIE_fn_hGetSuperObjectObject(h))->h_SectInfo)

#include "DNM/DNM_types.h"
#include "GAM/cineinfo.h"

void __cdecl fn_vTreatCharactersInSector(HIE_tdstSuperObject *hCharacter)
{
    MTH3D_tdstVector stOldV;
    MS_tdxHandleToDynam h_Dynam;
    DNM_tdstDynamics *p_stDynamic;
    ACP_tdxBool      bSectorChange = TRUE;
    unsigned long    ulTrameNow = GAM_g_ulEngineTrameNumber;

    M_GetMSHandle(hCharacter, StandardGame)->ulLastTrame = ulTrameNow;
    POS_fn_vGetTranslationVector(HIE_fn_hGetSuperObjectGlobalMatrix(hCharacter), &stOldV);

    if ((hCharacter != GAM_g_hMainCharacterSO) || !GAM_g_ucIsEdInGhostMode)
    {
        MS_tdxHandleTo3dData h_Current3dData = GAM_p_h3dData(hCharacter);

        if (h_Current3dData)
        {
            ACP_tdxBool    bSkip, bNoDisplay;
            unsigned long  ulCurrentBits;
            void          *p_stCurrentState;
            void          *p_stCurrentAnim;

            ulCurrentBits    = fn_ulStandardGameGetCustomBits(M_GetMSHandle(hCharacter, StandardGame));
            p_stCurrentState = fn_h3dDataGetCurrentState(h_Current3dData);
            p_stCurrentAnim  = p_stCurrentState ? fn_p_stGetAnimInState(p_stCurrentState) : NULL;

            bNoDisplay = (ACP_tdxBool)(
                (p_stCurrentAnim == NULL)
                || ((ulCurrentBits & GAM_C_CustBitNoAnimPlayer) && (hCharacter->lNumberOfChildren > 0))
                || ((ulCurrentBits & GAM_C_CustBitOutOfVisibility)
                    && (ulCurrentBits & (GAM_C_CustBitUnseenFrozen | GAM_C_CustBitUnseenFrozenAnimPlayer)))
                || ((ulCurrentBits & GAM_C_CustBitNoAnimPlayerWhenTooFar)
                    && fn_bStandardGameIsTooFar(M_GetMSHandle(hCharacter, StandardGame)))
            );

            if (!bNoDisplay || (ulCurrentBits & GAM_C_CustBitSoundWhenInvisible))
            {
                bSkip = PLA_fn_bSkipCurrentFrame(hCharacter);

                bSkip &= !fn_b3dDataHasAnimMatrixChanged(h_Current3dData);

                bSkip |= fn_uc3dDataGetFlagEndState(h_Current3dData);

                if (!bSkip)
                    PLA_fn_bDoAnimPlayerForCharacter(hCharacter, bNoDisplay, (ACP_tdxBool)FALSE);
            }
            else
            {
                bSkip = TRUE;
            }

            fn_vMakeCharacterReact(hCharacter);

            if (!M_GetMSHandle(hCharacter, StandardGame))
                return;
        }

        fn_vWaterStreakEffect(GAM_g_p_stViewportAttr, hCharacter, GAM_g_ulDisplayMode);

        fn_vMSPrtSrcSourceGenerateParticles(hCharacter);

        fn_vUpdateLight(hCharacter);

        if (GAM_g_ucIsEdInGhostMode && CAM_fn_bSuperObjectIsACamera(hCharacter))
        {
            fn_vSectInfoSetCurrentSector(
                GAM_h_SectInfo(hCharacter),
                fn_h_SectInfoGetCurrentSector(GAM_h_SectInfo(GAM_g_hMainCharacterSO)));
            fn_vInsertObjectInSectorList(
                fn_h_SectInfoGetCurrentSector(GAM_h_SectInfo(hCharacter)), hCharacter);
        }
        else
        {

            if (!(
                    (hCharacter->ulFlags & HIE_C_Flag_Superimposed)
                    ||
                    (
                        CAM_fn_bSuperObjectIsACamera(hCharacter)
                        && (GAM_g_cCameraMode == C_CamModeLook)
                        && (M_GetMSHandle(hCharacter, Cineinfo)->hWork->uwDNMFlags & DNM_CAM_C_NoObstacle)
                    )
                ))
            {
                h_Dynam = M_GetMSHandle(hCharacter, Dynam);
                if (h_Dynam)
                {
                    p_stDynamic = fn_p_stDynamGetDynamics(h_Dynam);
                    if (p_stDynamic)
                    {
                        if (DNM_M_bDynamicsIsAdvancedSize(p_stDynamic)
                            && DNM_M_bDynamicsIsCollide(p_stDynamic))
                            bSectorChange = (ACP_tdxBool)(DNM_M_cDynamicsGetCollideCounter(p_stDynamic) == 0);
                        else
                            bSectorChange = TRUE;
                    }
                }

                if (bSectorChange)
                    GAM_fn_vSectInfoTestChangeSectorForCharacter(hCharacter);
            }
        }
    }
}

/* 0x00412DF0 */
extern void __cdecl fn_v_DynamicFree(void *param_1);

void __cdecl fn_vReputCharacterSuperObjectAtTheWorld(struct HIE_tdstSuperObject *h_SupObject)
{
    HIE_fn_vChangeFather(
        h_SupObject,
        gp_stDynamicWorld,
        fn_ucStandardGameGetPlatFormType(M_GetMSHandle(h_SupObject, StandardGame)));
}

void __cdecl fn_vTreatCameraHierarchy(HIE_tdstSuperObject *hSuperObj,
                                      ACP_tdxBool bForcedActivity)
{
    HIE_tdstSuperObject      *hChild;
    struct tdstEngineObject_ *p_stObj;
    ACP_tdxBool               bTmp = FALSE;

    HIE_fn_vPushMatrix(hSuperObj);

    if ((hSuperObj->ulTypeOfLinkedObject == HIE_C_ulActor)
        &&
        (
          !GAM_g_bEngineFrozen
          || (fn_ulStandardGameGetCustomBits(M_GetMSHandle(hSuperObj, StandardGame))
              & GAM_C_CustBitUnfreezable)
        ))
    {
        p_stObj = (struct tdstEngineObject_ *)HIE_fn_hGetSuperObjectObject(hSuperObj);

        if (bForcedActivity)
            bTmp = TRUE;

        if (M_ObjectIsActivable(p_stObj) && (!M_ObjectIsAlreadyTreated(p_stObj)))
        {
            MTH3D_tdstVector            stDist;
            MTH3D_tdstVector           *p_stMainCharTranslation;
            MS_tdxHandleToStandardGame  hStdGame;
            unsigned long               ulCustomBits;

            p_stMainCharTranslation = POS_fn_p_stGetTranslationVector(
                HIE_fn_hGetSuperObjectGlobalMatrix(GAM_g_hMainCharacterSO));

            hStdGame = M_GetMSHandle(hSuperObj, StandardGame);
            if (hStdGame)
            {
                ulCustomBits = fn_ulStandardGameGetCustomBits(hStdGame);
                if (ulCustomBits & (GAM_C_CustBitNoAnimPlayerWhenTooFar
                                    | GAM_C_CustBitNoAIWhenTooFar
                                    | GAM_C_CustBitNoMecaWhenTooFar))
                {
                    MTH3D_M_vSubVector(&stDist,
                        POS_fn_p_stGetTranslationVector(
                            HIE_fn_hGetSuperObjectGlobalMatrix(hSuperObj)),
                        p_stMainCharTranslation);

                    if (MTH_M_bLess(MTH3D_M_xNorm1Vector(&stDist),
                                    MTH_M_xLongToReal(fn_ucStandardGameGetTooFarLimit(hStdGame))))
                        fn_vStandardGameSetTooFar(hStdGame, FALSE);
                    else
                        fn_vStandardGameSetTooFar(hStdGame, TRUE);
                }
            }
            fn_vTreatCharactersInSector(hSuperObj);
            bTmp = TRUE;
        }

        HIE_fn_bPopMatrix();
        HIE_fn_vPushMatrix(hSuperObj);
    }

    for (hChild = hSuperObj->p_stFirstChild; hChild != NULL;
         hChild = hChild->p_stNextBrother)
    {
        fn_vTreatCameraHierarchy(hChild, bTmp);
    }

    HIE_fn_bPopMatrix();
}

void fn_vTreatDynamicHierarchy(HIE_tdstSuperObject *hSuperObj,
                                   HIE_tdstSuperObject *hMainChar,
                                   MTH3D_tdstVector *p_stMainCharTranslation)
{
    HIE_tdstSuperObject      *hChild, *hNextChild;
    struct tdstEngineObject_ *p_stObj;
    unsigned long             ulCustomBits;
    MTH3D_tdstVector          stDist;

    if (hSuperObj->ulTypeOfLinkedObject == HIE_C_ulActor)
    {
        MS_tdxHandleToStandardGame hStdGame = M_GetMSHandle(hSuperObj, StandardGame);
        p_stObj = (struct tdstEngineObject_ *)HIE_fn_hGetSuperObjectObject(hSuperObj);

        if (!hStdGame
            || (fn_ucStandardGameGetPlatFormType(hStdGame) == 0
                && (!M_ObjectIsActive(p_stObj) || M_ObjectIsAlreadyTreated(p_stObj))))
        {
            return;
        }

        HIE_fn_vPushMatrix(hSuperObj);
        if (fn_ucStandardGameGetPlatFormType(hStdGame) == 0)
        {
            if (!GAM_g_bEngineFrozen
                || (fn_ulStandardGameGetCustomBits(hStdGame) & GAM_C_CustBitUnfreezable))
            {

                if (hSuperObj == hMainChar)
                {
                    fn_vStandardGameSetTooFar(hStdGame, FALSE);
                }
                else
                {
                    ulCustomBits = fn_ulStandardGameGetCustomBits(hStdGame);
                    if (ulCustomBits & (GAM_C_CustBitNoAnimPlayerWhenTooFar
                                        | GAM_C_CustBitNoAIWhenTooFar
                                        | GAM_C_CustBitNoMecaWhenTooFar))
                    {
                        MTH3D_M_vSubVector(&stDist,
                            POS_fn_p_stGetTranslationVector(
                                HIE_fn_hGetSuperObjectGlobalMatrix(hSuperObj)),
                            p_stMainCharTranslation);

                        if (MTH_M_bLess(MTH3D_M_xNorm1Vector(&stDist),
                                        MTH_M_xLongToReal(fn_ucStandardGameGetTooFarLimit(hStdGame))))
                            fn_vStandardGameSetTooFar(hStdGame, FALSE);
                        else
                            fn_vStandardGameSetTooFar(hStdGame, TRUE);
                    }
                }
                fn_vTreatCharactersInSector(hSuperObj);
            }
            HIE_fn_bPopMatrix();
            HIE_fn_vPushMatrix(hSuperObj);

            if (hSuperObj == GAM_g_hMainCharacterSO)
            {
                hChild     = hSuperObj->p_stFirstChild;
                hNextChild = HIE_fn_hForceGetSuperObjectNextBrother(hChild);
                while (hChild != NULL)
                {
                    if ((hChild->ulTypeOfLinkedObject == HIE_C_ulActor)
                        && !CAM_fn_bSuperObjectIsACamera(hChild))
                        fn_vTreatDynamicHierarchy(hChild, hMainChar, p_stMainCharTranslation);
                    hChild     = hNextChild;
                    hNextChild = HIE_fn_hForceGetSuperObjectNextBrother(hChild);
                }
            }
        }
        else
        {

            hChild     = hSuperObj->p_stLastChild;
            hNextChild = HIE_fn_hForceGetSuperObjectPrevBrother(hChild);
            while (hChild != NULL)
            {
                if ((hChild->ulTypeOfLinkedObject == HIE_C_ulActor)
                    && !CAM_fn_bSuperObjectIsACamera(hSuperObj))
                    fn_vTreatDynamicHierarchy(hChild, hMainChar, p_stMainCharTranslation);
                hChild     = hNextChild;
                hNextChild = HIE_fn_hForceGetSuperObjectPrevBrother(hChild);
            }
        }
    }
    else
    {
        HIE_fn_vPushMatrix(hSuperObj);
        hChild     = hSuperObj->p_stFirstChild;
        hNextChild = HIE_fn_hForceGetSuperObjectNextBrother(hChild);
        while (hChild != NULL)
        {
            if ((hChild->ulTypeOfLinkedObject == HIE_C_ulActor)
                && !CAM_fn_bSuperObjectIsACamera(hChild))
                fn_vTreatDynamicHierarchy(hChild, hMainChar, p_stMainCharTranslation);
            hChild     = hNextChild;
            hNextChild = HIE_fn_hForceGetSuperObjectNextBrother(hChild);
        }
    }
    HIE_fn_bPopMatrix();
}

void fn_vTreatDynamicHierarchyOfPlatForms(HIE_tdstSuperObject *hSuperObj)
{
    HIE_tdstSuperObject        *hChild, *hNextChild;
    HIE_tdstSuperObject        *hMainChar;
    MS_tdxHandleToStandardGame  hStdGame;
    struct tdstEngineObject_   *p_stObj;
    unsigned long               ulCustomBits;
    MTH3D_tdstVector            stDist, *p_stMainCharTranslation;

    hMainChar = GAM_g_hMainCharacterSO;
    p_stMainCharTranslation = POS_fn_p_stGetTranslationVector(
        HIE_fn_hGetSuperObjectGlobalMatrix(hMainChar));

    HIE_fn_vPushMatrix(hSuperObj);
    hChild     = hSuperObj->p_stFirstChild;
    hNextChild = HIE_fn_hForceGetSuperObjectNextBrother(hChild);
    while (hChild != NULL)
    {
        if (hChild->ulTypeOfLinkedObject == HIE_C_ulActor)
        {
            hStdGame = M_GetMSHandle(hChild, StandardGame);
            p_stObj  = (struct tdstEngineObject_ *)HIE_fn_hGetSuperObjectObject(hChild);

            if (!hStdGame
                || !M_ObjectIsActive(p_stObj)
                || (GAM_g_bEngineFrozen
                    && (fn_ulStandardGameGetCustomBits(hStdGame) ^ GAM_C_CustBitUnfreezable)))
            {

            }
            else if (fn_ucStandardGameGetPlatFormType(hStdGame) != 0)
            {
                if (!M_ObjectIsAlreadyTreated(p_stObj))
                {

                    if (hChild == hMainChar)
                    {
                        fn_vStandardGameSetTooFar(hStdGame, FALSE);
                    }
                    else
                    {
                        ulCustomBits = fn_ulStandardGameGetCustomBits(hStdGame);
                        if (ulCustomBits & (GAM_C_CustBitNoAnimPlayerWhenTooFar
                                            | GAM_C_CustBitNoAIWhenTooFar
                                            | GAM_C_CustBitNoMecaWhenTooFar))
                        {
                            MTH3D_M_vSubVector(&stDist,
                                POS_fn_p_stGetTranslationVector(
                                    HIE_fn_hGetSuperObjectGlobalMatrix(hChild)),
                                p_stMainCharTranslation);

                            if (MTH_M_bLess(MTH3D_M_xNorm1Vector(&stDist),
                                            MTH_M_xLongToReal(fn_ucStandardGameGetTooFarLimit(hStdGame))))
                                fn_vStandardGameSetTooFar(hStdGame, FALSE);
                            else
                                fn_vStandardGameSetTooFar(hStdGame, TRUE);
                        }
                    }
                    HIE_fn_vPushMatrix(hChild);
                    fn_vTreatCharactersInSector(hChild);
                    HIE_fn_bPopMatrix();
                }
            }
            else
            {
                break;
            }
        }
        hChild     = hNextChild;
        hNextChild = HIE_fn_hForceGetSuperObjectNextBrother(hChild);
    }
    HIE_fn_bPopMatrix();
}

void fn_vKillAllAlwaysInNonActiveSectors(void)
{
    struct tdstEngineObject_ *p_stAlwaysEngObj;
    HIE_tdstSuperObject     **p_hGenerator, **p_hLastGenerator;
    HIE_tdstSuperObject      *hAlwaysSuperObject;

    hAlwaysSuperObject = GAM_g_p_stAlwaysSuperObject;
    p_hGenerator       = (HIE_tdstSuperObject **)GAM_g_p_hAlwaysGenerator;
    p_hLastGenerator   = p_hGenerator + GAM_g_ulMaxNumberOfAlways;

    for (; p_hGenerator < p_hLastGenerator; hAlwaysSuperObject++, p_hGenerator++)
    {
        p_stAlwaysEngObj =
            (struct tdstEngineObject_ *)HIE_fn_hGetSuperObjectObject(hAlwaysSuperObject);

        if (p_stAlwaysEngObj->h_StandardGame
            &&
            (
              (fn_eStandardGameGetInitFlagWhenOutOfZone(p_stAlwaysEngObj->h_StandardGame)
                   == OI_WhenGeneratorIsDesactivated
               && !M_ObjectIsActive(
                       (struct tdstEngineObject_ *)HIE_fn_hGetSuperObjectObject(*p_hGenerator)))
              ||
              ((fn_ulStandardGameGetCustomBits(p_stAlwaysEngObj->h_StandardGame)
                    & GAM_C_CustBitDestroyWhenAnimEnded)
               && PLA_fn_bTestIfEndOfAction(hAlwaysSuperObject))
            ))
        {
            fn_vKillEngineObjectOrAlwaysByPointer(p_stAlwaysEngObj);
        }
        if (*p_hGenerator == NULL)
            hAlwaysSuperObject->p_stNextBrother = NULL;
    }
}

void fn_vKillEngineObjectOrAlwaysByPointer(struct tdstEngineObject_ *p_stEngObj)
{
    MS_tdxHandleToStandardGame hStdGame = p_stEngObj->h_StandardGame;

    if (hStdGame->otObjectFamilyType != C_InvalidObjectType)
    {
        if (hStdGame->otObjectPersonalType >= C_AlwaysObjectType)
            fn_vKillAlwaysByPersonalType(hStdGame->otObjectPersonalType);
        else
            fn_vKillEngineObjectNotAlways(p_stEngObj);   /* 0x406220 */
    }
    else
    {
        ERM_fn_vUpdateLastError(0x30, g_ucGameModuleId, 0, 0x38, -1, 0xff, 0xff, NULL);
    }
}

#define GAM_g_hNewMainCharacterSO   (*(HIE_tdstSuperObject **)(GAM_ENGINE_BASE + 0x1FC)) /* 0x50057C */

#define GAM_g_hDevice3D             (GAM_g_stEngineStructure.hGLDDevice)             /* 0x500454 */
#define GAM_g_hViewport3D           (GAM_g_stEngineStructure.hGLDViewport)           /* 0x500456 */
#define GAM_g_hGLDFixViewport       (GAM_g_stEngineStructure.hGLDFixViewport)        /* 0x5004D0 */
#define GAM_g_hDrawSemaphore        (GAM_g_stEngineStructure.hDrawSem)               /* 0x5004CC */
/* 0x0049F81C */
static const unsigned char GAM_g_ucEraseBackground_storage = 1;
#define GAM_g_ucEraseBackground     (GAM_g_ucEraseBackground_storage)
#define GAM_g_ucMirrorDisplayMode   GAM_g_bInGameMenu   /* 0x004B9BC8 */

__declspec(dllimport) unsigned long __stdcall WaitForSingleObject(void *hHandle, unsigned long dwMilliseconds);
#define GAM_pc_WaitForSingleObject  WaitForSingleObject

/* 0x0044C440 */
extern void __cdecl FUN_0044c440(void);

/* 0x0040AD00 */
extern unsigned short __cdecl FUN_00401340(int);
static void __cdecl fn_vSendBackGroundColor(void *hSprObjSector)
{
    int          *p_stElement;
    unsigned long a_ulColor[4];
    SECT_tdxHandleOfSectorObject hSectorObject;

    hSectorObject = (SECT_tdxHandleOfSectorObject)HIE_fn_hGetSuperObjectObject(hSprObjSector);
    for (p_stElement = (int *)LST2_M_StaticGetFirstElement(&hSectorObject->stListOfStaticLights);
         p_stElement != NULL;
         p_stElement = (int *)p_stElement[1])
    {
        if (GLI_lGetLightType((void *)*p_stElement) == GLI_C_lFogLight)
        {

            unsigned long ulR, ulG, ulB;
            GLI_xGetLightBackgroundColor((void *)*p_stElement, a_ulColor);
            ulR = (unsigned char)(long)MTH_M_xMul(*(float *)&a_ulColor[0], MTH_M_xLongToReal(255));
            ulG = (unsigned char)(long)MTH_M_xMul(*(float *)&a_ulColor[1], MTH_M_xLongToReal(255));
            ulB = (unsigned char)(long)MTH_M_xMul(*(float *)&a_ulColor[2], MTH_M_xLongToReal(255));
            GLD_bSetBackgroundColorForDevice(FUN_00401340(0), ulR | (ulG << 8) | (ulB << 16));
            break;
        }
    }
}

/* 0x00429F70 */
unsigned char __cdecl GLI_pc_fn_bSubmitAngleQuad(void *hCamSector, void *hCamera, void *p_stVpt);

void GAM_fn_vEngine(void)
{

    /* 0x0044c440 */
    extern void __cdecl FUN_0044c440(void);

    /* 0x4465C0 */
    extern void __cdecl SCT_fn_vSendSectorWhereIAmToViewportWithMirror(void *p_stAbsolutePositionOfCamera,
        unsigned short hDevice, unsigned short hViewport, void *p_stVpt, void *hSprObjSector,
        unsigned long lDrawMask);

    extern void __cdecl GLI_pc_fn_vDisplayMagicMirror(void *pvParam);   /* 0x00424440 */
    /* 0x00429F70 */
    unsigned short hDevice3D   = GAM_g_hDevice3D;
    unsigned short hViewport3D = GAM_g_hViewport3D;

    int bEnabledWrite;

    SND_fn_vSynchroSound();   /* 0x452B70 */
    FUN_0044c440();

    if (!fn_bIsEngineInPause())
    {
        HIE_tdstSuperObject *hMainChar;

        HIE_fn_vInvalidateAllGlobalMatrices();

        if (GAM_g_hNewMainCharacterSO != NULL)
        {
            GAM_g_hMainCharacterSO    = GAM_g_hNewMainCharacterSO;
            GAM_g_hNewMainCharacterSO = NULL;
        }

        fn_vUpdateDynamicHierarchy();
        fn_vKillAllAlwaysInNonActiveSectors();
        HIE_fn_vInitMatrixStack();

        if (bEnabledWrite)
            MGT_fn_vTreatMagnets();

        hMainChar = GAM_g_hMainCharacterSO;
        fn_vTreatDynamicHierarchyOfPlatForms(gp_stDynamicWorld);
        fn_vTreatDynamicHierarchy(
            gp_stDynamicWorld, hMainChar,
            POS_fn_p_stGetTranslationVector(HIE_fn_hGetSuperObjectGlobalMatrix(hMainChar)));
        CAM_fn_vCameraManagement();

        GLI_vRefreshAllCyclingTextures(
            (float)(unsigned long)GAM_g_ulUsefulDeltaTime / (GAM_g_xFrameLength * 1000.0f));
    }

    GAM_pc_WaitForSingleObject(GAM_g_hDrawSemaphore, 0xFFFFFFFF);

    if (GLD_bFlipDeviceWithSynchro(hDevice3D, 0xfe) == 0)
    {
        ERM_fn_vClearLastError(0);
        ERM_fn_vUpdateLastError(0x30, g_ucGameModuleId, 0, 0x2c, -1, 0xff, 0xff, NULL);
    }

    fn_vActualizeEngineClock();

    if (GAM_g_ucEraseBackground)
    {
        if (GLD_bClearViewportWithChoice(hDevice3D, GAM_g_hGLDFixViewport, 3) == 0)
        {
            ERM_fn_vClearLastError(0);
            ERM_fn_vUpdateLastError(0x30, g_ucGameModuleId, 0, 0x29, -1, 0xff, 0xff, NULL);
        }
    }
    else
    {
        if (GLD_bClearViewportWithChoice(hDevice3D, GAM_g_hGLDFixViewport, 2) == 0)
        {
            ERM_fn_vClearLastError(0);
            ERM_fn_vUpdateLastError(0x30, g_ucGameModuleId, 0, 0x29, -1, 0xff, 0xff, NULL);
        }
    }

    GLI_xClearViewingList();
    GLI_pc_fn_vBeginScene(hDevice3D);
    GLI_pc_fn_vUpdateCosPhase(GAM_g_ulUsefulDeltaTime);

    if (GLD_bRequestWriteToViewport(hDevice3D, hViewport3D,
                                    GAM_g_p_stViewportAttr, (unsigned long *)&bEnabledWrite) == 0)
    {
        ERM_fn_vClearLastError(0);
        ERM_fn_vUpdateLastError(0x30, g_ucGameModuleId, 0, 0x2a, -1, 0xff, 0xff, NULL);
    }

    if (bEnabledWrite)
    {
        MTH3D_tdstVector     stCameraPos;
        HIE_tdstSuperObject *hCamera;
        void                *hCamSector;

        GEO_fn_vMorphAddInMainLoop();

        hCamera    = CAM_fn_hGetActiveCamera(1);
        POS_fn_vGetTranslationVector(HIE_fn_hGetSuperObjectGlobalMatrix(hCamera), &stCameraPos);
        hCamSector = fn_h_SectInfoGetCurrentSector(GAM_h_SectInfo(hCamera));

        fn_vSendBackGroundColor(hCamSector);

        SCT_fn_vSendSectorWhereIAmToViewportWithMirror(
            &stCameraPos, hDevice3D, hViewport3D, GAM_g_p_stViewportAttr,
            hCamSector, GAM_g_ulDisplayMode);

        if (GAM_g_ucMirrorDisplayMode)
            GLI_pc_fn_vDisplayMagicMirror((void *)GAM_g_stEngineStructure.stFixViewportAttr);
        else
            GLI_pc_fn_bSubmitAngleQuad(hCamSector, hCamera, GAM_g_p_stViewportAttr);
    }
}

__declspec(dllimport) int __stdcall ReleaseSemaphore(void *hSemaphore, long lReleaseCount, long *lpPreviousCount);
#define GAM_pc_ReleaseSemaphore  ReleaseSemaphore

void fn_vDisplayAll(void)
{

    GLI_xSendListToViewport((unsigned long)GAM_g_p_stViewportAttr);

    if (GLD_bWriteToViewportFinished(GAM_g_hDevice3D, GAM_g_hViewport3D) == 0)
    {
        ERM_fn_vClearLastError(0);
        ERM_fn_vUpdateLastError(0x30, g_ucGameModuleId, 0, 0x2b, -1, 0xff, 0xff, NULL);
    }

    fn_vDisplayFix();

    /* 0x403290 */
    CAM_vDisplayInfo();

    GAM_pc_ReleaseSemaphore(GAM_g_hDrawSemaphore, 1, NULL);

    GLI_pc_fn_vEndScene(GAM_g_hDevice3D);
}

#include "GLD/GLD_Pub.h"
#define GAM_g_p_stFixViewportAttr   ((void *)GAM_g_stEngineStructure.stFixViewportAttr)
#define GAM_g_r_stFixViewportAttr   (*(struct GLD_tdstViewportAttributes_ *)GAM_g_stEngineStructure.stFixViewportAttr)

void fn_vDisplayFix(void)
{

    /* 0x00420B20 */
    extern void __cdecl GEO_fn_vComputeCameraParametersForCulling(void *, float);
    /* 0x00460670 */
    int bEnabledWrite;

    if (GAM_g_ucRefreshUi)
        GLI_RefreshUiSurfaceUnsynchronized(GAM_pc_g_szUiSurfaceName);

    GLI_ClearZBufferRegion(GAM_g_r_stFixViewportAttr.dwLeftInPixForClip, GAM_g_r_stFixViewportAttr.dwRightInPixForClip,
                           GAM_g_r_stFixViewportAttr.dwTopInPixForClip, GAM_g_r_stFixViewportAttr.dwBottomInPixForClip);

    GLI_xClearViewingList();

    if (GLD_bRequestWriteToViewport(GAM_g_hDevice3D, GAM_g_hGLDFixViewport,
                                    GAM_g_p_stFixViewportAttr, (unsigned long *)&bEnabledWrite) == 0)
    {
        ERM_fn_vClearLastError(0);
        ERM_fn_vUpdateLastError(0x30, g_ucGameModuleId, 0, 0x2a, -1, 0xff, 0xff, NULL);
    }

    GEO_fn_vComputeCameraParametersForCulling(GAM_g_p_stFixViewportAttr, 0.0f);

    HIE_fn_bSendSPOToFixViewport(GAM_g_p_stFixViewportAttr, GAM_g_ulDisplayMode);
    AGO_vDisplayGAUGES(GAM_g_p_stFixViewportAttr);
    GLI_xSendListToViewport((unsigned long)GAM_g_p_stFixViewportAttr);

    if (GLD_bWriteToViewportFinished(GAM_g_hDevice3D, GAM_g_hGLDFixViewport) == 0)
    {
        ERM_fn_vClearLastError(0);
        ERM_fn_vUpdateLastError(0x30, g_ucGameModuleId, 0, 0x2b, -1, 0xff, 0xff, NULL);
    }
}

void __cdecl fn_vMakeCharacterReact(HIE_tdstSuperObject *p_stSuperObj)
{
    unsigned long              ulCurrentBits;
    MS_tdxHandleToStandardGame hStdGame = M_GetMSHandle(p_stSuperObj, StandardGame);

    ulCurrentBits = fn_ulStandardGameGetCustomBits(hStdGame);

    if ((ulCurrentBits & (GAM_C_CustBitUnseenFrozen | GAM_C_CustBitOutOfVisibility))
        != (GAM_C_CustBitUnseenFrozen | GAM_C_CustBitOutOfVisibility))
    {

        if (!((ulCurrentBits & GAM_C_CustBitNoAI)
#ifdef RAYMAN2_NO_PROTECTION
              /* Copy protection removed: the protection actors (their scripts check the
                 relocation-table keys and the code image) do not think. */
              || (ulCurrentBits & GAM_C_CustBitProtection)
#endif /* RAYMAN2_NO_PROTECTION */
              || ((ulCurrentBits & GAM_C_CustBitNoAIWhenTooFar)
                  && fn_bStandardGameIsTooFar(hStdGame))))
        {
            fn_vMakeCharacterThink(p_stSuperObj);

            if ((hStdGame = M_GetMSHandle(p_stSuperObj, StandardGame)) != NULL)
                ulCurrentBits = fn_ulStandardGameGetCustomBits(hStdGame);
        }

        if (hStdGame
            && !((ulCurrentBits & GAM_C_CustBitNoMechanic)
                 || ((ulCurrentBits & GAM_C_CustBitNoMecaWhenTooFar)
                     && fn_bStandardGameIsTooFar(hStdGame))))
        {
            fn_vMakeCharacterMechanicallyReact(p_stSuperObj);
        }
    }
}

#define GAM_M_GetEngineObjectOfActor(p_stSuperObject) \
    ((((p_stSuperObject)->ulTypeOfLinkedObject) != HIE_C_ulActor) ? NULL : \
     ((struct tdstEngineObject_ *)(HIE_fn_hGetSuperObjectObject(p_stSuperObject))))

unsigned char __cdecl GAM_fn_bIsCharacterActive(HIE_tdstSuperObject *hSuperObj)
{
    return (unsigned char)
           ( ((HIE_fn_hGetSuperObjectFather(hSuperObj)==gp_stDynamicWorld) ||
              (HIE_fn_hGetSuperObjectFather(HIE_fn_hGetSuperObjectFather(hSuperObj))==gp_stDynamicWorld))
             && (fn_bf1StandardGameGetIsActive(GAM_M_GetEngineObjectOfActor(hSuperObj)->h_StandardGame)==1)
           );
}

static long GAM_fn_lGetNbPersoActivableInSector(HIE_tdstSuperObject *hSector)
{
    long lNbPerso = 0;
    int *hCharacterList;
    int i;

    for (i = 0, hCharacterList = (int *)*(int *)hSector->hLinkedObject.p_Void;
         hCharacterList != NULL;
         i++, hCharacterList = (int *)hCharacterList[1])
    {
        if (M_ObjectIsActivable((struct tdstEngineObject_ *)HIE_fn_hGetSuperObjectObject((HIE_tdstSuperObject *)hCharacterList[0])))
            lNbPerso++;
    }

    return lNbPerso;
}

long __cdecl GAM_fn_lGetNbPersoActive(void)
{
    long                 lNbPerso;
    HIE_tdstSuperObject *hActiveSector;
    SECT_tdxHandleOfElementLstActivityInteraction hActivityList;
    int                  i;

    hActiveSector = (HIE_tdstSuperObject *)fn_h_SectInfoGetCurrentSector(
        ((struct tdstEngineObject_ *)HIE_fn_hGetSuperObjectObject(GAM_M_pc_hMainCharacterSO))->h_SectInfo);

    lNbPerso = GAM_fn_lGetNbPersoActivableInSector(hActiveSector);

    SECT_M_ForEachActivityNodeInActivityInteractionList(hActiveSector,hActivityList,i) {
        lNbPerso += GAM_fn_lGetNbPersoActivableInSector(SECT_GetSectorInActivityList(hActivityList));
    }

    return lNbPerso;
}
