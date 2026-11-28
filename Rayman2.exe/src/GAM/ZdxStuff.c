#include "GAM/GAM_types.h"
#include "GEO/GeoObj.h"
#include "GEO/Element.h"
#include "LST/LstDef2.h"
#include "GAM/PlayAnim.h"
#include "GAM/Actions/CollSet.h"
#include "GAM/Actions/SectInfo.h"
#include "SCT/SectExt.h"
#include "ERM/CpaError.h"
#include "INT/IntersGO.h"
#include "GAM/ObjsTbls.h"

#define C_ucTypeZdd 0u
#define C_ucTypeZde 2u

#define C_ucNbPointsInGeometricAccelerator   30
#define C_ucNbElementsInGeometricAccelerator 4
#define C_ucNumberBidulesInElements          10

ACP_tdxHandleOfObject g_hCollisionGeoObj;        /* 0x0050036c */
ACP_tdxHandleOfObject g_hStaticCollisionGeoObj;  /* 0x00501404 */

typedef struct tdstZdxIndexList_ *ZDX_tdxHandleToZdxIndexList;
typedef struct tdstGeometricZdxList_ *ZDX_tdxHandleToGeoZdxList;
typedef struct tdstZdxList_ *ZDX_tdxHandleToZdxList;
typedef struct tdstZoneSetList_ *ZDX_tdxHandleToZoneSetList;

LST2_M_StaticUseListOf(ZDX_tdxHandleToZdxIndexList);
LST2_M_StaticUseListOf(ZDX_tdxHandleToGeoZdxList);
LST2_M_StaticUseListOf(ZDX_tdxHandleToZoneSetList);

typedef struct tdstZdxIndexList_
{
   LST2_M_StaticElementDeclaration(ZDX_tdxHandleToZdxIndexList)
   unsigned short uwZdxIndex;
} tdstZdxIndexList;

typedef struct tdstZdxList_
{
   LST2_M_StaticAnchorDeclaration(ZDX_tdxHandleToGeoZdxList) hGeoZdxList;
   unsigned short uwNumberOfZdx;
} tdstZdxList;

typedef struct tdstGeometricZdxList_
{
   LST2_M_StaticElementDeclaration(ZDX_tdxHandleToGeoZdxList)
   ACP_tdxHandleOfObject hGeoObj;
} tdstGeometricZdxList;

typedef struct tdstZoneSetList_
{
   LST2_M_StaticAnchorDeclaration(ZDX_tdxHandleToZdxIndexList) hZdxIndexList;
} tdstZoneSetList;

/* 0x0040CC60 */
unsigned short fn_uwGetIndexFromZdxIndexList(void *p_stZdxIndexList)
{
    return( ((tdstZdxIndexList *)p_stZdxIndexList)->uwZdxIndex );
}

/* 0x0040CC70 */
ACP_tdxHandleOfObject fn_hGetGeometricZdxOfTypeAtIndex(unsigned char ucZoneType,
                                                       unsigned short uwIndex,
                                                       void *hSuperObj)
{
    void **p_stElement = *(void ***)CS_fn_hGetZdxList(ucZoneType, hSuperObj);
    unsigned short uwI;

    for (uwI = 0; uwI < uwIndex; uwI++)
        p_stElement = (void **)*p_stElement;

    return (ACP_tdxHandleOfObject)p_stElement[1];
}

unsigned char fn_eGetPrivilegedActivationZoneOfCharacter(unsigned char ucZoneType,
                                                         void *hCharacter, short xIndex)
{
   register MS_tdxHandleToCollSet hCollSet = M_GetMSHandle(hCharacter,CollSet);

   if( hCollSet )
      return( CS_fn_eGetPrivilegedActivationZone(ucZoneType,hCollSet,xIndex) );

   return 3;
}

ACP_tdxBool fn_bIsAllDefaultPrivilegedActivationZoneOfCharacter(unsigned char ucZoneType,
                                                                void *hCharacter)
{
   register MS_tdxHandleToCollSet hCollSet = M_GetMSHandle(hCharacter,CollSet);

   if( hCollSet )
      return((ACP_tdxBool)( CS_fn_ulGetPrivilegedActivationZoneInCollSet(ucZoneType,hCollSet) == 0 ));

   return TRUE;
}

/* 0x0040DED0 */
long fn_lGetZdxListNumberOfElements(unsigned char ucZoneType, void *hSuperObj)
{
   ZDX_tdxHandleToZdxList hZdxList;

   hZdxList = CS_fn_hGetZdxList(ucZoneType, hSuperObj);

   if (hZdxList)
       return LST2_M_StaticGetNumberOfElements( &hZdxList->hGeoZdxList );

   return 0;
}

/* 0x0040C9D0 */
void *fn_p_stGetCurrentZoneSetOfType2(unsigned char ucZoneType, void *hSuperObj)
{
   if( M_GetMSHandle(hSuperObj,CollSet) )
      return( fn_hGetCurrentZoneSetOfType(ucZoneType,M_GetMSHandle(hSuperObj,CollSet)) );
   return NULL;
}

/* 0x0040C700 */
ACP_tdxHandleOfObject fn_hIsThisZoneActive(HIE_tdstSuperObject *_hCharacter,
                                           unsigned char _ucZoneType,
                                           unsigned char _ucZoneNumber)
{
    tdstGeometricZdxList *p_stGeoZdxList;
    unsigned char  ePrivilege;
    tdstZoneSetList *p_stZoneSet;
    tdstZdxList    *p_stZdxList = NULL;
    tdstZdxIndexList *p_stZdxIndex;
    int            iIndex, iMax;
    ACP_tdxIndex   xI;

    ePrivilege = fn_eGetPrivilegedActivationZoneOfCharacter(_ucZoneType, _hCharacter, _ucZoneNumber);
    if (ePrivilege == 1)
    {
        p_stZdxList    = (tdstZdxList *)CS_fn_hGetZdxList(_ucZoneType, _hCharacter);
        p_stGeoZdxList = LST2_M_StaticGetFirstElement(&p_stZdxList->hGeoZdxList);
        for (xI = 0; xI < _ucZoneNumber; xI++)
            p_stGeoZdxList = LST2_M_StaticGetNextElement(p_stGeoZdxList);
        return (ACP_tdxHandleOfObject)p_stGeoZdxList->hGeoObj;
    }
    else if (ePrivilege == 0)
    {

        p_stZoneSet = (tdstZoneSetList *)fn_p_stGetCurrentZoneSetOfType2(_ucZoneType, _hCharacter);
        if (p_stZoneSet)
        {
            p_stZdxIndex = LST2_M_StaticGetFirstElement(&p_stZoneSet->hZdxIndexList);
            iMax = (int)LST2_M_StaticGetNumberOfElements(&p_stZoneSet->hZdxIndexList);
            iIndex = 0;
            while ((iIndex < iMax) && (_ucZoneNumber > fn_uwGetIndexFromZdxIndexList(p_stZdxIndex)))
            {
                p_stZdxIndex = LST2_M_StaticGetNextElement(p_stZdxIndex);
                iIndex++;
            }
            if ((iIndex < iMax) && (_ucZoneNumber == fn_uwGetIndexFromZdxIndexList(p_stZdxIndex)))
            {
                p_stZdxList    = (tdstZdxList *)CS_fn_hGetZdxList(_ucZoneType, _hCharacter);
                p_stGeoZdxList = LST2_M_StaticGetFirstElement(&p_stZdxList->hGeoZdxList);
                for (xI = 0; xI < _ucZoneNumber; xI++)
                    p_stGeoZdxList = LST2_M_StaticGetNextElement(p_stGeoZdxList);
                return (ACP_tdxHandleOfObject)p_stGeoZdxList->hGeoObj;
            }
        }
    }
    return NULL;
}

extern ACP_tdxIndex GEO_xGetElementType(ACP_tdxHandleOfObject hObject, ACP_tdxIndex xElement);
extern ACP_tdxIndex GEO_xGetGeometricObjectNumberOfElementsMax(ACP_tdxHandleOfObject hObject);
extern ACP_tdxIndex GEO_lGetAlignedBoxNbBoxes(ACP_tdxHandleOfObject hObject, ACP_tdxHandleOfElement hElement);
extern ACP_tdxIndex GEO_lGetConeNbCones(ACP_tdxHandleOfObject hObject, ACP_tdxHandleOfElement hElement);

/* 0x0042DE30 */
extern void *PO_fn_hGetCollideSet(void *_hPhysicalObject);
extern ACP_tdxHandleOfObject PCS_fn_hGetGeoObjOfPhysicalCollSet(unsigned char ucZoneType,
                                                                void *_hPhyCollSet);

extern void *PCS_fn_hGetZddGeoObjOfPhysicalCollSet(void *_hPhyCollSet);

#define C_wMaskForZoom ((unsigned short)0x8000)

#define ZDX_M_hGetSuperObjectInChannel(hList, uwChannel)                                  \
    ((hList) ? ((CHN_tdxHandleToChannelArray)(hList))[(unsigned int)(uwChannel)].hSupObject \
             : (HIE_tdstSuperObject *)NULL)

/* 0x0040C7C0 */
GMT_tdxHandleToGameMaterial fn_hIsTypeExistsInGeoObj(ACP_tdxHandleOfObject _hGeoObj,GMT_tdxMask _wMaterialMask)
{
    ACP_tdxIndex xIndex, xIndexMax;
    GMT_tdxHandleToGameMaterial hGMT;
    ACP_tdxIndex xNbElements, xNbElementsMax;

    xNbElementsMax = GEO_xGetGeometricObjectNumberOfElementsMax( _hGeoObj );

    for( xNbElements=0; xNbElements<xNbElementsMax; xNbElements++ )

    {
        switch( GEO_xGetElementType(_hGeoObj,xNbElements) )
        {
        case GEO_C_xElementIndexedTriangles:

            GEO_vGetGameMaterialOfIndexedTriangles(_hGeoObj,xNbElements,&hGMT);
            if( GMT_fn_xMultipleTestCollideIdentifierFromGameMaterial(hGMT,_wMaterialMask) )
                return hGMT;
            break;
        case GEO_C_xElementSpheres:

            xIndexMax = GEO_lGetIndexedSphereNbSpheres( _hGeoObj, xNbElements );

            for( xIndex=0; xIndex<xIndexMax; xIndex++ )

            {
                hGMT = GEO_hGetGameMaterialOfIndexedSphere(_hGeoObj,xNbElements,xIndex);
                if( GMT_fn_xMultipleTestCollideIdentifierFromGameMaterial(hGMT,_wMaterialMask) )
                    return hGMT;
            }
            break;
        case GEO_C_xElementAlignedBoxes:

            xIndexMax = GEO_lGetAlignedBoxNbBoxes( _hGeoObj, xNbElements );

            for( xIndex=0; xIndex<xIndexMax; xIndex++ )

            {
                hGMT = GEO_vGetGameMaterialOfIndexedAlignedBox(_hGeoObj,xNbElements,xIndex);
                if( GMT_fn_xMultipleTestCollideIdentifierFromGameMaterial(hGMT,_wMaterialMask) )
                    return hGMT;
            }
            break;
        case GEO_C_xElementCones:

            xIndexMax = GEO_lGetConeNbCones( _hGeoObj, xNbElements );

            for( xIndex=0; xIndex<xIndexMax; xIndex++ )

            {
                hGMT = GEO_vGetGameMaterialOfIndexedCone(_hGeoObj,xNbElements,xIndex);
                if( GMT_fn_xMultipleTestCollideIdentifierFromGameMaterial(hGMT,_wMaterialMask) )
                    return hGMT;
            }
            break;
        case GEO_C_xElementPoints:
            hGMT = GEO_xGetGameMaterialOfElementPoints(_hGeoObj,xNbElements);
            if( GMT_fn_xMultipleTestCollideIdentifierFromGameMaterial(hGMT,_wMaterialMask) )
                return hGMT;
            break;

        case GEO_C_xElementNULL:
        case GEO_C_xElementSprites:
        case GEO_C_xElementLines:
            break;
        default:
            break;
        }
    }
    return NULL;
}

ACP_tdxBool fn_bIsCharacterInZdd(ACP_tdxHandleOfObject _hGeoObj,POS_tdstCompletePosition * _p_stMatrix,HIE_tdstSuperObject *_hCharacter)
{
   POS_tdstCompletePosition stMatrix;
   MTH3D_tdstVector stPosition;

	if(!M_ObjectIsActive ((tdstEngineObject*)HIE_fn_hGetSuperObjectObject(_hCharacter)) )
	   return FALSE;

   POS_fn_vInvertMatrix(&stMatrix,_p_stMatrix);
   POS_fn_vMulMatrixVertex(&stPosition,&stMatrix,POS_fn_p_stGetTranslationVector(HIE_fn_hGetSuperObjectGlobalMatrix(_hCharacter)));

   return( INT_fn_bGetInclusionPointInGeometricObject(&stPosition,_hGeoObj) );
}

/* 0x0040D290 */
ACP_tdxHandleOfObject ZDX_fn_hGetGeoObjOfTypeAndMatrixFromZdxInfos(
            HIE_tdstSuperObject *_hCharacter,
            unsigned char _ucZoneType,
            unsigned char _ucModuleOrCharacter,
            unsigned char _ucChannelNumber,
            POS_tdstCompletePosition **_p_hMatrix)
{
    ACP_tdxHandleOfObject hGeoObj;
    HIE_tdstSuperObject  *hModule, *hModuleIfZoom;
    long                  lType;
    void                 *hPhysical;
    void                 *hPhyCollSet;

    if (_ucModuleOrCharacter)
    {
        hModuleIfZoom = hModule = ZDX_M_hGetSuperObjectInChannel(
            fn_h3dDataGetChannelSOList(M_GetMSHandle(_hCharacter, 3dData)), _ucChannelNumber);
        if (!hModule)
            return NULL;
        lType = (long)hModule->ulTypeOfLinkedObject;
        if ((lType & (HIE_C_ulPO | HIE_C_ulPO_Mirror)) == 0)
        {
            hModuleIfZoom = ZDX_M_hGetSuperObjectInChannel(
                fn_h3dDataGetChannelSOList(M_GetMSHandle(_hCharacter, 3dData)),
                (unsigned short)(_ucChannelNumber + C_wMaskForZoom));
            if (!hModuleIfZoom)
                return NULL;
        }
        hPhysical   = HIE_fn_hGetSuperObjectObject(hModuleIfZoom);
        hPhyCollSet = PO_fn_hGetCollideSet(hPhysical);
        if (!hPhyCollSet)
            return NULL;
        hGeoObj = PCS_fn_hGetGeoObjOfPhysicalCollSet(_ucZoneType, hPhyCollSet);
        if (!hGeoObj)
            return NULL;

        *_p_hMatrix = HIE_fn_hGetSuperObjectGlobalMatrix(hModule);
        return hGeoObj;
    }
    else
    {

        hGeoObj = fn_hIsThisZoneActive(_hCharacter, _ucZoneType, (unsigned char)(_ucChannelNumber - 1));
        if (hGeoObj)
        {
            *_p_hMatrix = HIE_fn_hGetSuperObjectGlobalMatrix(_hCharacter);
            return hGeoObj;
        }
    }
    return NULL;
}

/* 0x0040DBD0 */
ACP_tdxHandleOfObject fn_hMakeOneObjectFromManyOthers(unsigned char _ucZoneType,
                                                      void *_hCharacter,
                                                      ACP_tdxHandleOfObject _hTargetObject)
{
    ACP_tdxIndex          xIndex;
    ACP_tdxHandleOfObject a10_hTable[10];
    ACP_tdxHandleOfObject hGeoObj;
    short                 xI;
    void                 *p_stZdxIndex;
    void                 *p_stZoneSet;
    unsigned char         ePrivilege;
    unsigned short        uwCurrentIndexInActivation;

    xIndex      = 0;
    hGeoObj     = NULL;
    p_stZoneSet = fn_p_stGetCurrentZoneSetOfType2(_ucZoneType, _hCharacter);

    if (fn_bIsAllDefaultPrivilegedActivationZoneOfCharacter(_ucZoneType, _hCharacter))
    {

        if (p_stZoneSet)
        {
            for (p_stZdxIndex = *(void **)p_stZoneSet; p_stZdxIndex != NULL;
                 p_stZdxIndex = *(void **)p_stZdxIndex)
            {
                hGeoObj = fn_hGetGeometricZdxOfTypeAtIndex(
                    _ucZoneType, fn_uwGetIndexFromZdxIndexList(p_stZdxIndex), _hCharacter);
                if (hGeoObj)
                    a10_hTable[xIndex++] = hGeoObj;
            }
        }
    }
    else
    {
        xI           = 0;
        p_stZdxIndex = p_stZoneSet ? *(void **)p_stZoneSet : NULL;
        uwCurrentIndexInActivation =
            (unsigned short)((p_stZdxIndex == NULL) ? -1
                                                    : fn_uwGetIndexFromZdxIndexList(p_stZdxIndex));

        while (xI < fn_lGetZdxListNumberOfElements(_ucZoneType, _hCharacter))
        {
            ePrivilege = fn_eGetPrivilegedActivationZoneOfCharacter(_ucZoneType, _hCharacter, xI);
            if ((ePrivilege == 1) ||
                ((ePrivilege == 0) && (uwCurrentIndexInActivation == xI)))
            {
                hGeoObj = fn_hGetGeometricZdxOfTypeAtIndex(_ucZoneType, xI, _hCharacter);
                if (hGeoObj)
                    a10_hTable[xIndex++] = hGeoObj;
            }

            xI++;
            if ((p_stZdxIndex != NULL) && (uwCurrentIndexInActivation < xI))
            {
                p_stZdxIndex = *(void **)p_stZdxIndex;
                uwCurrentIndexInActivation =
                    (unsigned short)((p_stZdxIndex == NULL) ? -1
                                                            : fn_uwGetIndexFromZdxIndexList(p_stZdxIndex));
            }
        }
    }

    GEO_fn_hTransformTableOfObjectInOneObjectForGAM(_hTargetObject, a10_hTable, xIndex);
    return _hTargetObject;
}

/* 0x0040DB90 */
ACP_tdxHandleOfObject fn_hMakeOneObjectFromManyZdx(unsigned char ucZoneType, void *hCharacter)
{
    return fn_hMakeOneObjectFromManyOthers(ucZoneType, hCharacter, g_hCollisionGeoObj);
}

/* 0x0040DBB0 */
ACP_tdxHandleOfObject fn_hStaticMakeOneObjectFromManyZdx(unsigned char ucZoneType, void *hCharacter)
{
    return fn_hMakeOneObjectFromManyOthers(ucZoneType, hCharacter, g_hStaticCollisionGeoObj);
}

void fn_vFirstTimeInitCollisionGeometricObject(void)
{
    ACP_tdxHandleOfElement hElement;

    GEO_vCreateGeometricObject(&g_hCollisionGeoObj,
        C_ucNbPointsInGeometricAccelerator, C_ucNbElementsInGeometricAccelerator);

    GEO_vCreateElementSpheres(g_hCollisionGeoObj, &hElement, C_ucNumberBidulesInElements);

    GEO_vCreateElementAlignedBoxes(g_hCollisionGeoObj, &hElement, C_ucNumberBidulesInElements);

    GEO_vCreateElementCones(g_hCollisionGeoObj, &hElement, C_ucNumberBidulesInElements);

    GEO_xCreateElementPoints(g_hCollisionGeoObj, &hElement, C_ucNumberBidulesInElements);
    GEO_vEndModifyObject(g_hCollisionGeoObj);
    GEO_vCreateObjectListOfPointsMaterial(g_hCollisionGeoObj);
    GEO_vComputeObjectListOfPointsMaterial(g_hCollisionGeoObj);

    GEO_vCreateGeometricObject(&g_hStaticCollisionGeoObj,
        C_ucNbPointsInGeometricAccelerator, C_ucNbElementsInGeometricAccelerator);

    GEO_vCreateElementSpheres(g_hStaticCollisionGeoObj, &hElement, C_ucNumberBidulesInElements);

    GEO_vCreateElementAlignedBoxes(g_hStaticCollisionGeoObj, &hElement, C_ucNumberBidulesInElements);

    GEO_vCreateElementCones(g_hStaticCollisionGeoObj, &hElement, C_ucNumberBidulesInElements);

    GEO_xCreateElementPoints(g_hStaticCollisionGeoObj, &hElement, C_ucNumberBidulesInElements);
    GEO_vEndModifyObject(g_hStaticCollisionGeoObj);
    GEO_vCreateObjectListOfPointsMaterial(g_hStaticCollisionGeoObj);
    GEO_vComputeObjectListOfPointsMaterial(g_hStaticCollisionGeoObj);
}

typedef struct tdstZoneSetEncapsulation_ *ZDX_tdxHandleToZoneSetEncapsulation;
LST2_M_StaticUseListOf(ZDX_tdxHandleToZoneSetEncapsulation);
typedef struct tdstZoneSetEncapsulation_
{
    LST2_M_StaticElementDeclaration(ZDX_tdxHandleToZoneSetEncapsulation)
    void *p_stZoneSetList;
    unsigned short uwIndex;
} tdstZoneSetEncapsulation;

extern void *__cdecl fnp_vGameMallocInHLM(unsigned int ulSize);  /* 0x004077E0 */

ZDX_tdxHandleToZoneSetEncapsulation fn_p_stCreateZoneSetEncapsulation(void)
{
    ZDX_tdxHandleToZoneSetEncapsulation p_stZSE;

    p_stZSE = (ZDX_tdxHandleToZoneSetEncapsulation)
        fnp_vGameMallocInHLM((unsigned int)sizeof(tdstZoneSetEncapsulation));
    p_stZSE->hNextBrotherSLk = NULL;
    p_stZSE->p_stZoneSetList = NULL;
    p_stZSE->uwIndex = 0;
    return p_stZSE;
}

typedef struct tdstCsaList_
{
   LST2_M_StaticAnchorDeclaration(ZDX_tdxHandleToZoneSetEncapsulation) hZoneSetEncapsulationList;
} tdstCsaList;

/* 0x0040CB00 */
void fn_vCorrectActivationList(tdstCsaList *_p_stCsaList, long _lNumberOfState)
{
   ZDX_tdxHandleToZoneSetEncapsulation p_stZSENew;
   ZDX_tdxHandleToZoneSetEncapsulation p_stZSECurrent;
   ZDX_tdxHandleToZoneSetEncapsulation p_stZSENext;
   unsigned short uwIndex, uwNextIndex;

   if (LST2_M_StaticGetNumberOfElements(&_p_stCsaList->hZoneSetEncapsulationList) == 0)
   {

      for (uwIndex = 0; uwIndex < _lNumberOfState; uwIndex++)
      {
         p_stZSENew = fn_p_stCreateZoneSetEncapsulation();
         p_stZSENew->uwIndex = uwIndex;
         LST2_M_StaticAddTail(&_p_stCsaList->hZoneSetEncapsulationList, p_stZSENew);
      }
   }
   else
   {

      p_stZSECurrent = LST2_M_StaticGetFirstElement(&_p_stCsaList->hZoneSetEncapsulationList);
      uwIndex = p_stZSECurrent->uwIndex;
      while (uwIndex > 0)
      {
         uwIndex--;
         p_stZSENew = fn_p_stCreateZoneSetEncapsulation();
         p_stZSENew->uwIndex = uwIndex;
         LST2_M_StaticAddHead(&_p_stCsaList->hZoneSetEncapsulationList, p_stZSENew);
      }

      p_stZSENext = LST2_M_StaticGetNextElement(p_stZSECurrent);
      while (p_stZSENext != LST2_C_StaticInvalid)
      {
         uwIndex = (unsigned short)(p_stZSECurrent->uwIndex + 1);
         uwNextIndex = p_stZSENext->uwIndex;
         while (uwIndex < uwNextIndex)
         {
            p_stZSENew = fn_p_stCreateZoneSetEncapsulation();
            p_stZSENew->uwIndex = uwIndex;
            LST3_M_StaticInsertBetween(&_p_stCsaList->hZoneSetEncapsulationList, p_stZSENew, p_stZSECurrent, p_stZSENext);
            p_stZSECurrent = p_stZSENew;
            uwIndex++;
         }
         p_stZSECurrent = p_stZSENext;
         p_stZSENext = LST2_M_StaticGetNextElement(p_stZSENext);
      }

      p_stZSECurrent = LST2_M_StaticGetLastElement(&_p_stCsaList->hZoneSetEncapsulationList);
      uwIndex = (unsigned short)(p_stZSECurrent->uwIndex + 1);
      while (uwIndex < _lNumberOfState)
      {
         p_stZSENew = fn_p_stCreateZoneSetEncapsulation();
         p_stZSENew->uwIndex = uwIndex;
         LST2_M_StaticAddTail(&_p_stCsaList->hZoneSetEncapsulationList, p_stZSENew);
         uwIndex++;
      }
   }
   if (LST2_M_StaticGetNumberOfElements(&_p_stCsaList->hZoneSetEncapsulationList) > _lNumberOfState)
   {

      ERM_fn_vUpdateLastError(0x30, g_ucGameModuleId, 0, 0x40, -1, 0xff, 0xff, NULL);
      Erm_M_ClearLastError(C_ucErmDefaultChannel);
   }
}

/* 0x0040D9D0 */
void fn_vDetectAllObjectsInThisModule(
   HIE_tdstSuperObject *_hCharacter,
   ACP_tdxHandleOfObject _hModule,
   POS_tdstCompletePosition * _p_stMatrix,
   HIE_tdstSuperObject * * _p_hSuperObjTable,
   long _lMaxNumberOfObjects,
   long * _lCurrentNumberOfObjects)
{
	SECT_tdxHandleOfElementLstCharacter hCharacterList;
	SECT_tdxHandleOfElementLstActivityInteraction hActivityList;
	long i,j;
	HIE_tdstSuperObject *hCharInList;

	SECT_M_ForEachCharListInSector(fn_h_SectInfoGetCurrentSector(M_GetMSHandle(_hCharacter,SectInfo)),hCharacterList,i)
	{
		hCharInList = SECT_GetCharacterInList(hCharacterList);
		if( _hCharacter != hCharInList )
		{
			if( M_ObjectIsActive ((tdstEngineObject*)HIE_fn_hGetSuperObjectObject(hCharInList)) )
			{
				if( fn_bIsCharacterInZdd(_hModule,_p_stMatrix,hCharInList) )
				{
					if( (*_lCurrentNumberOfObjects)< _lMaxNumberOfObjects )
					{
						_p_hSuperObjTable[(*_lCurrentNumberOfObjects)++] = hCharInList;
					}
					else
						break;
				}
			}
		}
	}

	if( *_lCurrentNumberOfObjects < _lMaxNumberOfObjects )
	{

		SECT_M_ForEachActivityNodeInActivityInteractionList(fn_h_SectInfoGetCurrentSector(M_GetMSHandle(_hCharacter,SectInfo)),hActivityList,i)
		{
			if( *_lCurrentNumberOfObjects < _lMaxNumberOfObjects )
			{
				SECT_M_ForEachCharListInSector(SECT_GetSectorInActivityList(hActivityList),hCharacterList,j)
				{
					hCharInList = SECT_GetCharacterInList(hCharacterList);
					if( M_ObjectIsActive ((tdstEngineObject*)HIE_fn_hGetSuperObjectObject(hCharInList)) )
					{
						if( fn_bIsCharacterInZdd(_hModule,_p_stMatrix,hCharInList) )
						{
							if( *_lCurrentNumberOfObjects < _lMaxNumberOfObjects )
							{
								_p_hSuperObjTable[(*_lCurrentNumberOfObjects)++] = hCharInList;
							}
							else
								break;
						}
					}
				}
			}
			else
				break;
		}
	}
}

/* 0x0040D940 */
static void fn_vGetAllObjectsInModuleZdd(
   HIE_tdstSuperObject *hSuperObj,
   unsigned char _ucChannelNumber,
   HIE_tdstSuperObject * * p_hSuperObjTable,
   long _lNumberMaxOfObjects,
   long * _p_lCurrentNumberOfObjects)
{
	HIE_tdstSuperObject *hModule,*hModuleIfZoom;
	POS_tdstCompletePosition * p_stMatrix;
	ACP_tdxHandleOfObject hGeoObj;
	void *hPhysical;

	hModuleIfZoom = hModule = ZDX_M_hGetSuperObjectInChannel( fn_h3dDataGetChannelSOList(M_GetMSHandle(hSuperObj,3dData)),_ucChannelNumber);
	if( !hModule ) return;
	if( hModule->ulTypeOfLinkedObject != HIE_C_ulPO )
	{
		hModuleIfZoom = ZDX_M_hGetSuperObjectInChannel( fn_h3dDataGetChannelSOList(M_GetMSHandle(hSuperObj,3dData)),(unsigned short)(_ucChannelNumber+C_wMaskForZoom));
		if( !hModuleIfZoom ) return;
	}

	hPhysical = HIE_fn_hGetSuperObjectObject(hModuleIfZoom);
	/* 0x0042DE30 */
	hGeoObj = PCS_fn_hGetZddGeoObjOfPhysicalCollSet( PO_fn_hGetCollideSet(hPhysical) );
	if( !hGeoObj )
		return;

	p_stMatrix = HIE_fn_hGetSuperObjectGlobalMatrix(hModule);
	fn_vDetectAllObjectsInThisModule(hSuperObj,hGeoObj,p_stMatrix,p_hSuperObjTable,_lNumberMaxOfObjects,_p_lCurrentNumberOfObjects);
}

/* 0x0040DB00 */
void fn_vGetAllObjectsInZdd(HIE_tdstSuperObject *_hSuperObj,
							unsigned char _ucModuleOrCharacter,
							unsigned char _ucChannelNumber,
							HIE_tdstSuperObject * * _p_hSuperObjTable,
							unsigned short * _p_uwNumber)
{
   ACP_tdxHandleOfObject hGeoObj;
   long lMaxNumberOfObjects;
   long lCurrentNumberOfObjects;

   lMaxNumberOfObjects = (long)(*_p_uwNumber);
   lCurrentNumberOfObjects = 0;

   if( _ucModuleOrCharacter )
   {
      fn_vGetAllObjectsInModuleZdd(_hSuperObj,_ucChannelNumber,_p_hSuperObjTable,lMaxNumberOfObjects,&lCurrentNumberOfObjects);
   }
   else
   {

      hGeoObj = fn_hIsThisZoneActive(_hSuperObj,C_ucTypeZdd,(unsigned char)(_ucChannelNumber-1));
      if( hGeoObj )
      {
         fn_vDetectAllObjectsInThisModule(_hSuperObj,hGeoObj,HIE_fn_hGetSuperObjectGlobalMatrix(_hSuperObj),_p_hSuperObjTable,lMaxNumberOfObjects,&lCurrentNumberOfObjects);
      }
   }
   *_p_uwNumber = (unsigned short)lCurrentNumberOfObjects;
}

/* 0x0040D390 */
ACP_tdxBool fn_bThisZdeFromOneVsThisZdeFromTwo(
            HIE_tdstSuperObject *_hCharacterONE,
            unsigned char _ucModuleOrCharacterForONE,
            unsigned char _ucChannelNumberForONE,
            HIE_tdstSuperObject *_hCharacterTWO,
            unsigned char _ucModuleOrCharacterForTWO,
            unsigned char _ucChannelNumberForTWO)
{
   POS_tdstCompletePosition *p_stMatrix1,*p_stMatrix2;
   ACP_tdxHandleOfObject hGeoObj1,hGeoObj2;
   GMT_tdxHandleToGameMaterial hMat1,hMat2;

   hGeoObj1 = ZDX_fn_hGetGeoObjOfTypeAndMatrixFromZdxInfos(_hCharacterONE,C_ucTypeZde,_ucModuleOrCharacterForONE,_ucChannelNumberForONE,&p_stMatrix1);
   hGeoObj2 = ZDX_fn_hGetGeoObjOfTypeAndMatrixFromZdxInfos(_hCharacterTWO,C_ucTypeZde,_ucModuleOrCharacterForTWO,_ucChannelNumberForTWO,&p_stMatrix2);
   if( !hGeoObj1 || !hGeoObj2 )
      return FALSE;
   if( INT_fn_bDetectIntersectGeomObjWithGeomObj(hGeoObj1,p_stMatrix1,hGeoObj2,p_stMatrix2,&hMat1,&hMat2) )
      return TRUE;
   else
      return FALSE;
}

ACP_tdxBool fn_bIsCharacterTWODetectedByThisZddOfCharacterOne(
            HIE_tdstSuperObject *_hCharacterONE,
            unsigned char _ucModuleOrCharacter,
            unsigned char _ucChannelNumber,
            HIE_tdstSuperObject *_hCharacterTWO)
{
   POS_tdstCompletePosition * p_stMatrix;
   ACP_tdxHandleOfObject hGeoObj;

   hGeoObj = ZDX_fn_hGetGeoObjOfTypeAndMatrixFromZdxInfos(_hCharacterONE,C_ucTypeZdd,_ucModuleOrCharacter,_ucChannelNumber,&p_stMatrix);
   if( !hGeoObj )
      return FALSE;
   return( fn_bIsCharacterInZdd(hGeoObj,p_stMatrix,_hCharacterTWO) );
}

/* 0x0040D7E0 */
ACP_tdxBool fn_bIsCharacterTWODetectedByCharacterOne(
            HIE_tdstSuperObject *_hCharacterONE,
            HIE_tdstSuperObject *_hCharacterTWO)
{
	ACP_tdxIndex xI;
	void *p_stZdxIndex;
	void *p_stZoneSet;
	ACP_tdxHandleOfObject hGeoObj1;
	void *hPhysical;
	void *hPhyCollSet;
	CHN_tdxHandleToChannel hChannel;
	HIE_tdstSuperObject *hModule;
	long lType;

	unsigned char ePrivilege;
	unsigned short uwCurrentIndexInActivation;

	hGeoObj1					= NULL;
	hPhysical					= NULL;
	hPhyCollSet					= NULL;
	hChannel					= NULL;
	hModule						= NULL;
	lType						= 0;
	xI							= 0;
	p_stZoneSet					= fn_p_stGetCurrentZoneSetOfType2(C_ucTypeZdd,_hCharacterONE);
	p_stZdxIndex				= (p_stZoneSet ? *(void **)p_stZoneSet : NULL);
	uwCurrentIndexInActivation	= (unsigned short)((p_stZdxIndex == NULL) ? -1 : fn_uwGetIndexFromZdxIndexList(p_stZdxIndex));
	while( xI < fn_lGetZdxListNumberOfElements(C_ucTypeZdd,_hCharacterONE) )
	{
		ePrivilege = fn_eGetPrivilegedActivationZoneOfCharacter(C_ucTypeZdd,_hCharacterONE,xI);
		if( (ePrivilege == 1) ||
			((ePrivilege == 0) && (uwCurrentIndexInActivation == xI)) )
		{
			hGeoObj1 = fn_hGetGeometricZdxOfTypeAtIndex(C_ucTypeZdd,xI,_hCharacterONE);
			if( hGeoObj1 )
			{
				if( fn_bIsCharacterInZdd(hGeoObj1,HIE_fn_hGetSuperObjectGlobalMatrix(_hCharacterONE),_hCharacterTWO) )
					return TRUE;
			}
		}

		xI++;
		if( (p_stZdxIndex != NULL) && (uwCurrentIndexInActivation < xI) )
		{
			p_stZdxIndex = *(void **)p_stZdxIndex;
			uwCurrentIndexInActivation	= (unsigned short)((p_stZdxIndex == NULL) ? -1 : fn_uwGetIndexFromZdxIndexList(p_stZdxIndex));
		}
	}

	hChannel = fn_h3dDataGetFirstActiveChannel(M_GetMSHandle(_hCharacterONE,3dData));
	while (hChannel)
	{
		if ( hModule && hChannel->hZoomSupObject)
		{
			hModule = hChannel->hZoomSupObject;
		}
		else
		{
			hModule = hChannel->hSupObject;
		}
		lType = (long)hModule->ulTypeOfLinkedObject;
		if( lType & (HIE_C_ulPO | HIE_C_ulPO_Mirror ))
		{
			hPhysical = HIE_fn_hGetSuperObjectObject(hModule);
			hPhyCollSet = PO_fn_hGetCollideSet(hPhysical);
			if( hPhyCollSet )
			{
				hGeoObj1 = PCS_fn_hGetZddGeoObjOfPhysicalCollSet( hPhyCollSet );
				if( hGeoObj1  )
				{
					if( fn_bIsCharacterInZdd(hGeoObj1,HIE_fn_hGetSuperObjectGlobalMatrix(hModule),_hCharacterTWO) )
						return TRUE;
				}
			}
		}
	   hChannel = hChannel->p_stNextActiveChannel;
	}
	return FALSE;
}

/* 0x0040CFA0 */
extern void *PCS_fn_hGetZdeGeoObjOfPhysicalCollSet(void *_hPhyCollSet);
/* 0x004194C0 */
extern ACP_tdxBool __cdecl fn_bObjectTableHasZde(tdxHandleToObjectsTablesList _hObjectTableList);

ACP_tdxBool fn_bIsThisZdeInClashWithCharacterZdeOfTypes(ACP_tdxHandleOfObject _hGeoObj,
                                                        POS_tdstCompletePosition *_hMatrix,
                                                        HIE_tdstSuperObject *_hCharacter,
                                                        GMT_tdxMask _wMaterialMask)
{
    unsigned char ePrivilege;
    unsigned short uwCurrentIndexInActivation;
    ACP_tdxIndex xI;
    void *p_stZdxIndex;
    void *p_stZoneSet;
    ACP_tdxHandleOfObject hStaticGeoObj;
    void *hPhysical;
    void *hPhyCollSet;
    GMT_tdxHandleToGameMaterial hMat1, hMat2;
    CHN_tdxHandleToChannel hChannel;
    HIE_tdstSuperObject *hModule;
    long lType;

    if (!M_ObjectIsActive((struct tdstEngineObject_ *)HIE_fn_hGetSuperObjectObject(_hCharacter)))
        return FALSE;

    hStaticGeoObj = NULL;
    hPhysical = NULL;
    hPhyCollSet = NULL;
    hMat1 = NULL;
    hMat2 = NULL;
    hChannel = NULL;
    hModule = NULL;
    lType = 0;
    xI = 0;
    p_stZoneSet = fn_p_stGetCurrentZoneSetOfType2(C_ucTypeZde, _hCharacter);

    if (fn_bIsAllDefaultPrivilegedActivationZoneOfCharacter(C_ucTypeZde, _hCharacter))
    {

        if (p_stZoneSet)
        {
            for (xI = 0, p_stZdxIndex = *(void **)p_stZoneSet; p_stZdxIndex;
                 xI++, p_stZdxIndex = *(void **)p_stZdxIndex)
            {
                hStaticGeoObj = fn_hGetGeometricZdxOfTypeAtIndex(C_ucTypeZde, fn_uwGetIndexFromZdxIndexList(p_stZdxIndex), _hCharacter);
                if (hStaticGeoObj && ((_wMaterialMask == 0) || fn_hIsTypeExistsInGeoObj(hStaticGeoObj, _wMaterialMask)))
                {

                    if (INT_fn_bDetectIntersectGeomObjWithGeomObj(_hGeoObj, _hMatrix, hStaticGeoObj, HIE_fn_hGetSuperObjectGlobalMatrix(_hCharacter), &hMat1, &hMat2))
                    {

                        if ((_wMaterialMask == 0) || GMT_fn_xMultipleTestCollideIdentifierFromGameMaterial(hMat2, _wMaterialMask))
                        {
                            return TRUE;
                        }
                    }
                }
            }
        }
    }
    else
    {
        p_stZdxIndex = (p_stZoneSet ? *(void **)p_stZoneSet : NULL);
        uwCurrentIndexInActivation = (unsigned short)((p_stZdxIndex == NULL) ? -1 : fn_uwGetIndexFromZdxIndexList(p_stZdxIndex));
        while (xI < fn_lGetZdxListNumberOfElements(C_ucTypeZde, _hCharacter))
        {
            ePrivilege = fn_eGetPrivilegedActivationZoneOfCharacter(C_ucTypeZde, _hCharacter, xI);
            if ((ePrivilege == 1) ||
                ((ePrivilege == 0) && (uwCurrentIndexInActivation == xI)))
            {
                hStaticGeoObj = fn_hGetGeometricZdxOfTypeAtIndex(C_ucTypeZde, xI, _hCharacter);

                if (hStaticGeoObj && ((_wMaterialMask == 0) || fn_hIsTypeExistsInGeoObj(hStaticGeoObj, _wMaterialMask)))
                {

                    if (INT_fn_bDetectIntersectGeomObjWithGeomObj(_hGeoObj, _hMatrix, hStaticGeoObj, HIE_fn_hGetSuperObjectGlobalMatrix(_hCharacter), &hMat1, &hMat2))
                    {

                        if ((_wMaterialMask == 0) || GMT_fn_xMultipleTestCollideIdentifierFromGameMaterial(hMat2, _wMaterialMask))
                        {
                            return TRUE;
                        }
                    }
                }
            }

            xI++;
            if ((p_stZdxIndex != NULL) && (uwCurrentIndexInActivation < xI))
            {
                p_stZdxIndex = *(void **)p_stZdxIndex;
                uwCurrentIndexInActivation = (unsigned short)((p_stZdxIndex == NULL) ? -1 : fn_uwGetIndexFromZdxIndexList(p_stZdxIndex));
            }
        }
    }

    {
        tdxHandleToObjectsTablesList hObjectTable;
        if ((hObjectTable = fn_h3dDataGetCurrentObjectsTable(M_GetMSHandle(_hCharacter, 3dData))) == NULL ||
            fn_bObjectTableHasZde(hObjectTable) == FALSE)
        {
            return FALSE;
        }
    }

    hChannel = fn_h3dDataGetFirstActiveChannel(M_GetMSHandle(_hCharacter, 3dData));
    while (hChannel)
    {
        if (hChannel->hZoomSupObject)
            hModule = hChannel->hZoomSupObject;
        else
            hModule = hChannel->hSupObject;

        lType = (long)hModule->ulTypeOfLinkedObject;
        if (lType & (HIE_C_ulPO | HIE_C_ulPO_Mirror))
        {
            hPhysical = HIE_fn_hGetSuperObjectObject(hModule);
            hPhyCollSet = PO_fn_hGetCollideSet(hPhysical);
            if (hPhyCollSet)
            {
                hStaticGeoObj = PCS_fn_hGetZdeGeoObjOfPhysicalCollSet(hPhyCollSet);

                if (hStaticGeoObj && ((_wMaterialMask == 0) || fn_hIsTypeExistsInGeoObj(hStaticGeoObj, _wMaterialMask)))
                {

                    if (INT_fn_bDetectIntersectGeomObjWithGeomObj(_hGeoObj, _hMatrix, hStaticGeoObj, HIE_fn_hGetSuperObjectGlobalMatrix(hModule), &hMat1, &hMat2))
                    {

                        if ((_wMaterialMask == 0) || GMT_fn_xMultipleTestCollideIdentifierFromGameMaterial(hMat2, _wMaterialMask))
                            return TRUE;
                    }
                }
            }
        }
        hChannel = hChannel->p_stNextActiveChannel;
    }
    return FALSE;
}

#define GAM_C_CustBitProjectile 0x00000008
#pragma warning (disable : 4706)
void fn_vGetAllCharacterInContactWithThisGeoObj(
   HIE_tdstSuperObject *_hSuperObj,
   ACP_tdxHandleOfObject _hGeoObj,
   POS_tdstCompletePosition *_hMatrix,
   HIE_tdstSuperObject **_hSuperObjTable,
   GMT_tdxMask _wMaterialMask,
   long _lMaxNumberOfObjects,
   long *_p_lCurrentNumberOfObjects)
{
    SECT_tdxHandleOfElementLstCharacter hCharacterList;
    SECT_tdxHandleOfElementLstCollisionInteraction hCollisionList;
    unsigned long ulI, ulJ;
    HIE_tdstSuperObject *hCharInList;

    int bThisActorIsAProjectile    = fn_ulStandardGameGetCustomBits(M_GetMSHandle(_hSuperObj,StandardGame)) & GAM_C_CustBitProjectile;
    int bNoCollisionWithProjectile = fn_bCollSetGetCharacterNoCollisionZdeWithProjectile(M_GetMSHandle(_hSuperObj,CollSet));

    SECT_M_ForEachCharListInSector(fn_h_SectInfoGetCurrentSector(M_GetMSHandle(_hSuperObj,SectInfo)),hCharacterList,ulI)
    {
        if( *_p_lCurrentNumberOfObjects < _lMaxNumberOfObjects )
        {
            hCharInList = SECT_GetCharacterInList(hCharacterList);
            if( _hSuperObj != hCharInList )
            {
                if( M_ObjectIsActive ((struct tdstEngineObject_ *)HIE_fn_hGetSuperObjectObject(hCharInList)) )
                {
                    {

                        MS_tdxHandleToCollSet hCollSet;
                        if (bThisActorIsAProjectile && ((hCollSet = M_GetMSHandle(hCharInList,CollSet)) != 0) && fn_bCollSetGetCharacterNoCollisionZdeWithProjectile(hCollSet))
                            continue;

                        if (bNoCollisionWithProjectile && (fn_ulStandardGameGetCustomBits(M_GetMSHandle(hCharInList,StandardGame)) & GAM_C_CustBitProjectile))
                            continue;
                    }

                    if( fn_bIsThisZdeInClashWithCharacterZdeOfTypes(_hGeoObj,_hMatrix,hCharInList,_wMaterialMask) )
                    {

                        _hSuperObjTable[(*_p_lCurrentNumberOfObjects)++] = hCharInList;
                    }
                }
            }
        }
        else
        {
            return;
        }
    }

    if( (*_p_lCurrentNumberOfObjects < _lMaxNumberOfObjects) )
    {

        SECT_M_ForEachCollisionNodeInCollisionInteractionList(fn_h_SectInfoGetCurrentSector(M_GetMSHandle(_hSuperObj,SectInfo)),hCollisionList,ulI)
        {
            SECT_M_ForEachCharListInSector(SECT_GetSectorInCollisionList(hCollisionList),hCharacterList,ulJ)
            {
                if( (*_p_lCurrentNumberOfObjects < _lMaxNumberOfObjects) )
                {
                    hCharInList = SECT_GetCharacterInList(hCharacterList);
                    if( M_ObjectIsActive ((struct tdstEngineObject_ *)HIE_fn_hGetSuperObjectObject(hCharInList)) )
                    {
                        {

                            MS_tdxHandleToCollSet hCollSet;
                            if (bThisActorIsAProjectile && ((hCollSet = M_GetMSHandle(hCharInList,CollSet)) != 0) && fn_bCollSetGetCharacterNoCollisionZdeWithProjectile(hCollSet))
                                continue;

                            if (bNoCollisionWithProjectile && (fn_ulStandardGameGetCustomBits(M_GetMSHandle(hCharInList,StandardGame)) & GAM_C_CustBitProjectile))
                                continue;
                        }

                        if( fn_bIsThisZdeInClashWithCharacterZdeOfTypes(_hGeoObj,_hMatrix,hCharInList,_wMaterialMask) )
                        {

                            _hSuperObjTable[(*_p_lCurrentNumberOfObjects)++] = hCharInList;
                        }
                    }
                }
                else
                {
                    return;
                }
            }
        }
    }
}
#pragma warning (default : 4706)

#define ZDX_M_hGetZoomSuperObjectInChannel(hList, uwChannel)                              \
    ((hList) ? ((CHN_tdxHandleToChannelArray)(hList))[(unsigned int)(uwChannel)].hZoomSupObject \
             : (HIE_tdstSuperObject *)NULL)

/* 0x0040CF00 */
void fn_vGetAllCharactersInContactWithThisModuleZde(
 HIE_tdstSuperObject *_hSuperObj,
 unsigned char _ucChannelNumber,
 HIE_tdstSuperObject **_hSuperObjTable,
 GMT_tdxMask _wMaterialMask,
 long _lMaxNumberOfObjects,
 long *_p_lCurrentNumberofObjects)
{
    HIE_tdstSuperObject *hModule;
    ACP_tdxHandleOfObject hGeoObj;
    void *hPhysical;
    void *hPhyCollSet;

    hModule = ZDX_M_hGetSuperObjectInChannel(fn_h3dDataGetChannelSOList(M_GetMSHandle(_hSuperObj,3dData)), _ucChannelNumber);
    if( ! hModule || (unsigned long)hModule->ulTypeOfLinkedObject != HIE_C_ulPO )
    {
        hModule = ZDX_M_hGetZoomSuperObjectInChannel(fn_h3dDataGetChannelSOList(M_GetMSHandle(_hSuperObj,3dData)), (unsigned short)(_ucChannelNumber));
        if ( !hModule || (unsigned long)hModule->ulTypeOfLinkedObject != HIE_C_ulPO )
            return;
    }
    hPhysical = HIE_fn_hGetSuperObjectObject(hModule);
    hPhyCollSet = PO_fn_hGetCollideSet(hPhysical);
    if( !hPhyCollSet )
        return;
    hGeoObj = PCS_fn_hGetZdeGeoObjOfPhysicalCollSet(hPhyCollSet);
    if( !hGeoObj )
        return;

    fn_vGetAllCharacterInContactWithThisGeoObj(_hSuperObj,hGeoObj,HIE_fn_hGetSuperObjectGlobalMatrix(hModule),_hSuperObjTable,_wMaterialMask,_lMaxNumberOfObjects,_p_lCurrentNumberofObjects);
}

/* 0x0040D350 */
ACP_tdxBool fn_bThisZdeFromOneVsAllZdeFromTwo(
            HIE_tdstSuperObject *_hCharacterONE,
            unsigned char _ucModuleOrCharacter,
            unsigned char _ucChannelNumber,
            HIE_tdstSuperObject *_hCharacterTWO,
            GMT_tdxMask _wMaterialMask)
{
   POS_tdstCompletePosition *p_stMatrix;
   ACP_tdxHandleOfObject hGeoObj;

   hGeoObj = ZDX_fn_hGetGeoObjOfTypeAndMatrixFromZdxInfos(_hCharacterONE,C_ucTypeZde,_ucModuleOrCharacter,_ucChannelNumber,&p_stMatrix);
   if( !hGeoObj )
      return FALSE;
   return( fn_bIsThisZdeInClashWithCharacterZdeOfTypes(hGeoObj,p_stMatrix,_hCharacterTWO,_wMaterialMask) );
}

ACP_tdxBool fn_bThisTypeFromOneVsThisTypeFromTwo( HIE_tdstSuperObject *_hCharacterONE, GMT_tdxMask _wMaterialMaskOfONE, HIE_tdstSuperObject *_hCharacterTWO, GMT_tdxMask _wMaterialMaskOfTWO);

/* 0x0040D410 */
void fn_vThisTypeVsThisType(
   HIE_tdstSuperObject *_hCharacter,
   GMT_tdxMask _wCharacterMask,
   GMT_tdxMask _wOthersMask,
   HIE_tdstSuperObject **_p_hObjectTable,
   unsigned short *_p_uwNumber)
{
   SECT_tdxHandleOfElementLstCharacter hCharacterList;
   HIE_tdstSuperObject *hSOTmp;
   SECT_tdxHandleOfElementLstCollisionInteraction hCollisionList;
   unsigned long ulI, ulJ;
   unsigned short uwCurrentNumber = 0;
   unsigned short uwMaxNumber = *_p_uwNumber;

   SECT_M_ForEachCharListInSector(fn_h_SectInfoGetCurrentSector(M_GetMSHandle(_hCharacter,SectInfo)),hCharacterList,ulI)
   {
      hSOTmp = SECT_GetCharacterInList(hCharacterList);
      if( _hCharacter != hSOTmp )
      {
         if( M_ObjectIsActive ((struct tdstEngineObject_ *)HIE_fn_hGetSuperObjectObject(hSOTmp)) )
         {
            if( uwCurrentNumber < uwMaxNumber )
            {
               if( fn_bThisTypeFromOneVsThisTypeFromTwo(_hCharacter, _wCharacterMask, hSOTmp, _wOthersMask) )
               {
                  _p_hObjectTable[uwCurrentNumber++] = hSOTmp;
               }
            }
            else
            {
               *_p_uwNumber = uwMaxNumber;
               return;
            }
         }
      }
   }

   SECT_M_ForEachCollisionNodeInCollisionInteractionList(fn_h_SectInfoGetCurrentSector(M_GetMSHandle(_hCharacter,SectInfo)),hCollisionList,ulI)
   {
      SECT_M_ForEachCharListInSector(SECT_GetSectorInCollisionList(hCollisionList),hCharacterList,ulJ)
      {
         if( uwCurrentNumber < uwMaxNumber )
         {
            hSOTmp = SECT_GetCharacterInList(hCharacterList);
            if( M_ObjectIsActive ((struct tdstEngineObject_ *)HIE_fn_hGetSuperObjectObject(hSOTmp)) )
            {
               if( fn_bThisTypeFromOneVsThisTypeFromTwo(_hCharacter, _wCharacterMask, hSOTmp, _wOthersMask) )
               {
                  _p_hObjectTable[uwCurrentNumber++] = hSOTmp;
               }
            }
         }
         else
         {
            *_p_uwNumber = uwMaxNumber;
            return;
         }
      }
   }
   *_p_uwNumber = uwCurrentNumber;
}

/* 0x0040D570 */
ACP_tdxBool fn_bThisTypeFromOneVsThisTypeFromTwo(
            HIE_tdstSuperObject *_hCharacterONE,
            GMT_tdxMask _wMaterialMaskOfONE,
            HIE_tdstSuperObject *_hCharacterTWO,
            GMT_tdxMask _wMaterialMaskOfTWO)
{
    ACP_tdxIndex xI;
    void *p_stZdxIndex;
    void *p_stZoneSet;
    ACP_tdxHandleOfObject hGeoObj1;
    void *hPhysical;
    void *hPhyCollSet;
    CHN_tdxHandleToChannel hChannel;
    long lType;
    HIE_tdstSuperObject *hModule;

    unsigned char ePrivilege;
    unsigned short uwCurrentIndexInActivation;

    hGeoObj1 = NULL;
    hPhysical = NULL;
    hPhyCollSet = NULL;
    hChannel = NULL;
    hModule = NULL;
    lType = 0;
    xI = 0;
    p_stZoneSet = fn_p_stGetCurrentZoneSetOfType2(C_ucTypeZde, _hCharacterONE);

    if (fn_bIsAllDefaultPrivilegedActivationZoneOfCharacter(C_ucTypeZde, _hCharacterONE))
    {
        if (p_stZoneSet)
        {
            for (xI = 0, p_stZdxIndex = *(void **)p_stZoneSet; p_stZdxIndex;
                 xI++, p_stZdxIndex = *(void **)p_stZdxIndex)
            {
                hGeoObj1 = fn_hGetGeometricZdxOfTypeAtIndex(C_ucTypeZde, fn_uwGetIndexFromZdxIndexList(p_stZdxIndex), _hCharacterONE);
                if (hGeoObj1 && fn_hIsTypeExistsInGeoObj(hGeoObj1, _wMaterialMaskOfONE))
                {
                    if (fn_bIsThisZdeInClashWithCharacterZdeOfTypes(hGeoObj1, HIE_fn_hGetSuperObjectGlobalMatrix(_hCharacterONE), _hCharacterTWO, _wMaterialMaskOfTWO))
                        return TRUE;
                }
            }
        }
    }
    else
    {
        p_stZdxIndex = (p_stZoneSet ? *(void **)p_stZoneSet : NULL);
        uwCurrentIndexInActivation = (unsigned short)((p_stZdxIndex == NULL) ? -1 : fn_uwGetIndexFromZdxIndexList(p_stZdxIndex));
        while (xI < fn_lGetZdxListNumberOfElements(C_ucTypeZde, _hCharacterONE))
        {
            ePrivilege = fn_eGetPrivilegedActivationZoneOfCharacter(C_ucTypeZde, _hCharacterONE, xI);
            if ((ePrivilege == 1) ||
                ((ePrivilege == 0) && (uwCurrentIndexInActivation == xI)))
            {
                hGeoObj1 = fn_hGetGeometricZdxOfTypeAtIndex(C_ucTypeZde, xI, _hCharacterONE);
                if (hGeoObj1 && fn_hIsTypeExistsInGeoObj(hGeoObj1, _wMaterialMaskOfONE))
                {
                    if (fn_bIsThisZdeInClashWithCharacterZdeOfTypes(hGeoObj1, HIE_fn_hGetSuperObjectGlobalMatrix(_hCharacterONE), _hCharacterTWO, _wMaterialMaskOfTWO))
                        return TRUE;
                }
            }

            xI++;
            if ((p_stZdxIndex != NULL) && (uwCurrentIndexInActivation < xI))
            {
                p_stZdxIndex = *(void **)p_stZdxIndex;
                uwCurrentIndexInActivation = (unsigned short)((p_stZdxIndex == NULL) ? -1 : fn_uwGetIndexFromZdxIndexList(p_stZdxIndex));
            }
        }
    }

    hChannel = fn_h3dDataGetFirstActiveChannel(M_GetMSHandle(_hCharacterONE, 3dData));
    while (hChannel)
    {
        if (hModule && hChannel->hZoomSupObject)
        {
            hModule = hChannel->hZoomSupObject;
        }
        else
        {
            hModule = hChannel->hSupObject;
        }
        lType = (long)hModule->ulTypeOfLinkedObject;
        if (lType & (HIE_C_ulPO | HIE_C_ulPO_Mirror))
        {
            hPhysical = HIE_fn_hGetSuperObjectObject(hModule);
            hPhyCollSet = PO_fn_hGetCollideSet(hPhysical);
            if (hPhyCollSet)
            {
                hGeoObj1 = PCS_fn_hGetZdeGeoObjOfPhysicalCollSet(hPhyCollSet);
                if (hGeoObj1 && fn_hIsTypeExistsInGeoObj(hGeoObj1, _wMaterialMaskOfONE))
                {
                    if (fn_bIsThisZdeInClashWithCharacterZdeOfTypes(hGeoObj1, HIE_fn_hGetSuperObjectGlobalMatrix(hModule), _hCharacterTWO, _wMaterialMaskOfTWO))
                        return TRUE;
                }
            }
        }
        hChannel = hChannel->p_stNextActiveChannel;
    }
    return FALSE;
}

#define C_ucTypeZdm 1u
#define C_ucTypeZdr 3u

#include "GAM/Family.h"

#define ZDX_M_pFamilyStateArrayAnchor(_h3d) \
    ((void *)&((tdxHandleToFamilyList)((MS_tdxHandleTo3dData)(_h3d))->h_Family)->hForStateArray_first)

void __cdecl fn_vUpdateCurrentActivationForCharacter(HIE_tdstSuperObject *_hCharacter)
{
   ACP_tdxIndex xIndex,xI;
   tdstZoneSetEncapsulation * p_stZSE;
   tdstCsaList *hCsaList = NULL;
   tdxHandleToState hStateTmp,hFirstState;
   MS_tdxHandleToCollSet hCollSet;
   void **hAnchor;

   hCollSet = M_GetMSHandle(_hCharacter,CollSet);
   if( !hCollSet )
      return;

   hFirstState = fn_h3dDataGetCurrentState(M_GetMSHandle(_hCharacter,3dData));

   hAnchor = (void **)ZDX_M_pFamilyStateArrayAnchor(M_GetMSHandle(_hCharacter,3dData));
   xIndex = 0;
   hStateTmp = (tdxHandleToState)hAnchor[0];
   while( hStateTmp != hFirstState && hStateTmp != NULL )
   {
      xIndex++;
      hStateTmp = (tdxHandleToState)*(void **)hStateTmp;
   }

   hCsaList=(tdstCsaList *)CS_fn_hGetActivationList(C_ucTypeZdd,_hCharacter);
   if( hCsaList )
   {
      LST2_M_StaticGetElementNumber(&hCsaList->hZoneSetEncapsulationList,p_stZSE,xIndex,xI);
      fn_vSetCurrentZoneSetOfType(C_ucTypeZdd,hCollSet,(void *)p_stZSE->p_stZoneSetList);
   }
   else
   {
      fn_vSetCurrentZoneSetOfType(C_ucTypeZdd,hCollSet,(void *)NULL);
   }

   hCsaList=(tdstCsaList *)CS_fn_hGetActivationList(C_ucTypeZde,_hCharacter);
   if( hCsaList )
   {
      LST2_M_StaticGetElementNumber(&hCsaList->hZoneSetEncapsulationList,p_stZSE,xIndex,xI);
      fn_vSetCurrentZoneSetOfType(C_ucTypeZde,hCollSet,(void *)p_stZSE->p_stZoneSetList);
   }
   else
   {
      fn_vSetCurrentZoneSetOfType(C_ucTypeZde,hCollSet,(void *)NULL);
   }

   hCsaList=(tdstCsaList *)CS_fn_hGetActivationList(C_ucTypeZdr,_hCharacter);
   if( hCsaList )
   {
      LST2_M_StaticGetElementNumber(&hCsaList->hZoneSetEncapsulationList,p_stZSE,xIndex,xI);
      fn_vSetCurrentZoneSetOfType(C_ucTypeZdr,hCollSet,(void *)p_stZSE->p_stZoneSetList);
   }
   else
   {
      fn_vSetCurrentZoneSetOfType(C_ucTypeZdr,hCollSet,(void *)NULL);
   }

   hCsaList=(tdstCsaList *)CS_fn_hGetActivationList(C_ucTypeZdm,_hCharacter);
   if( hCsaList )
   {
      LST2_M_StaticGetElementNumber(&hCsaList->hZoneSetEncapsulationList,p_stZSE,xIndex,xI);
      fn_vSetCurrentZoneSetOfType(C_ucTypeZdm,hCollSet,(void *)p_stZSE->p_stZoneSetList);
   }
   else
   {
      fn_vSetCurrentZoneSetOfType(C_ucTypeZdm,hCollSet,(void *)NULL);
   }
}

/* 0x0040CCA0 */
void fn_vGetAllCharactersInContactWithThisZde(HIE_tdstSuperObject *_hSuperObj,unsigned char _ucModuleOrCharacter,unsigned char _ucChannelNumber,HIE_tdstSuperObject **_hSuperObjTable,unsigned short *_p_uwNumber,GMT_tdxMask _wMaterialMask)
{
   ACP_tdxHandleOfObject hGeoObj;
   long lMaxNumberOfObjects;
   long lCurrentNumberOfObjects;

   lMaxNumberOfObjects = (long)(*_p_uwNumber);
   lCurrentNumberOfObjects = 0;
   if( _ucModuleOrCharacter )
   {
      fn_vGetAllCharactersInContactWithThisModuleZde(_hSuperObj,_ucChannelNumber,_hSuperObjTable,_wMaterialMask,lMaxNumberOfObjects,&lCurrentNumberOfObjects);
   }
   else
   {
      hGeoObj = fn_hIsThisZoneActive(_hSuperObj,C_ucTypeZde,(unsigned char)(_ucChannelNumber-1));
      if( hGeoObj )
         fn_vGetAllCharacterInContactWithThisGeoObj(_hSuperObj,hGeoObj,HIE_fn_hGetSuperObjectGlobalMatrix(_hSuperObj),_hSuperObjTable,_wMaterialMask,lMaxNumberOfObjects,&lCurrentNumberOfObjects);
   }
   *_p_uwNumber = (unsigned short)lCurrentNumberOfObjects;
}
