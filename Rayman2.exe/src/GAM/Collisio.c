#include <stddef.h>
#include <string.h>
#include <math.h>

#include "COL/COL.h"
#include "COL/CollGO.h"
#include "GAM/GAM_types.h"
#include "GEO/GeoObj.h"
#include "ERM/CpaError.h"
#include "HIE/HieSpObj.h"
#include "GAM/PlayAnim.h"
#include "GAM/Actions/CollSet.h"

extern void *__cdecl fn_h_CollSetRealAlloc(void);
extern void  __cdecl fn_vCollSetInitValueSAI(void *hCollSet);

/* 0x494811 */
typedef int           (__cdecl *tMakeOneObjFromManyZdx)(unsigned int ucType, void *hChar);
typedef void          (__cdecl *tStaticCollTree)(void *hChar, void *hSector, int hZdm,
                                                  void *pStart, void *pEnd);
typedef char          (__cdecl *tIsSectorVirtual)(void *hSector);

/* 0x0040DB90 */
extern void *__cdecl fn_hMakeOneObjectFromManyZdx(unsigned char ucZoneType, void *hCharacter);

#define fn_bCollSetGetCharNoCollWithProjectile(h)         fn_bCollSetGetCharacterNoCollisionWithProjectile((void *)(h))
#define fn_bCollSetGetCharNoCollWithSecondaryCharacter(h) fn_bCollSetGetCharacterNoCollisionWithSecondaryCharacter((void *)(h))
#define fn_bCollSetGetCharNoCollWithMainCharacter(h)      fn_bCollSetGetCharacterNoCollisionWithMainCharacter((void *)(h))
#define fn_bCollSetGetCharNoCollWithOtherSectors(h)       fn_bCollSetGetCharacterNoCollisionWithOtherSectors((void *)(h))
#define fn_bCollSetGetCharNoCollWithMap(h)                fn_bCollSetGetCharacterNoCollisionWithMap((void *)(h))

/* 0x00494420 */
static void fn_vNewTestStaticCollideFor2Characters(HIE_tdstSuperObject *hCharacter,
                                                   ACP_tdxHandleOfObject _hDynamicZdm,
                                                   HIE_tdstSuperObject *hStaticCharacter,
                                                   POS_tdstCompletePosition *p_stStartMatrix,
                                                   POS_tdstCompletePosition *p_stEndMatrix);

#define COL_PC_SO_Flag_NotPickable     0x00000001u
#define COL_PC_SO_Flag_NoTransform     0x00000004u
#define COL_PC_SO_Flag_ZoomScale       0x00000008u
#define COL_PC_SO_Flag_BoxBV           0x00000010u
#define COL_PC_SO_Flag_CheckChildren   0x00000200u
#define COL_PC_SO_Type_IPO_Any         0x00000060u
#define COL_PC_C_wStaticObject         0
#define COL_PC_C_wMobileObject         1

/* 0x0042DE30 */
void *PO_fn_hGetCollideSet(void *_hPhysicalObject);

#include "PCS/PCSStruc.h"

#include "PO/IPOProto.h"

#include "PO/POStruc.h"

static __inline ACP_tdxBool HIE_fn_SO_bHasABoxBoundingVolume( struct HIE_tdstSuperObject *_hSO )
{
    return (ACP_tdxBool)(_hSO->ulFlags & HIE_C_Flag_ulTypeOfBoundingVolume);
}

static __inline ACP_tdxBool HIE_fn_SO_bHasNoTransformationMatrix( struct HIE_tdstSuperObject *_hSO )
{
    return (ACP_tdxBool) (_hSO->ulFlags & HIE_C_Flag_ulNoTransformationMatrix);
}

typedef struct COL_tdstSectorCharElement_ COL_tdstSectorCharElement;
struct COL_tdstSectorCharElement_
{
    HIE_tdstSuperObject       *hPointerOfCharacter;
    COL_tdstSectorCharElement *hNextBrotherDyn;
};

typedef struct COL_tdstSectorCollisionNode_ COL_tdstSectorCollisionNode;
struct COL_tdstSectorCollisionNode_
{
    HIE_tdstSuperObject         *hPointerOfSectorSO;
    COL_tdstSectorCollisionNode *hNextBrotherSLk;
};

typedef struct COL_tdstSectorCollisionPart_
{
    COL_tdstSectorCharElement   *hFirstCharDyn;
    void                        *hLastCharDyn;
    long                         lNumberOfCharDyn;
    unsigned char                aucAnchorsBetween[0x30 - 0x0C];
    COL_tdstSectorCollisionNode *hFirstElementSLk;
    void                        *hLastElementSLk;
    long                         lNumberOfElementsSLk;
} COL_tdstSectorCollisionPart;

#define SECT_fn_hGetFirstCharListInSector(_hSector) \
    (((COL_tdstSectorCollisionPart *)HIE_fn_hGetSuperObjectObject(_hSector))->hFirstCharDyn)
#define SECT_fn_hGetNextCharListInSector(_hCharList) \
    ((_hCharList)->hNextBrotherDyn)
#define SECT_M_ForEachCharListInSector(_hSector,_hCharList,_i) \
for                                                        \
(                                                          \
    _i=0,(_hCharList)=SECT_fn_hGetFirstCharListInSector(_hSector);      \
    _hCharList;                                            \
    _i++,_hCharList=SECT_fn_hGetNextCharListInSector(_hCharList)        \
)
#define SECT_GetCharacterInList(_hCharacterList) \
    ((_hCharacterList)->hPointerOfCharacter)
#define SECT_fn_iGetNumberOfNodesInCollisionList(_hSector) \
    (((COL_tdstSectorCollisionPart *)HIE_fn_hGetSuperObjectObject(_hSector))->lNumberOfElementsSLk)
#define SECT_fn_hGetFirstCollisionNodeInCollisionList(_hSector) \
    (((COL_tdstSectorCollisionPart *)HIE_fn_hGetSuperObjectObject(_hSector))->hFirstElementSLk)
#define SECT_fn_hGetNextCollisionNodeInCollisionList(_hCollNode) \
    ((_hCollNode)->hNextBrotherSLk)
#define SECT_GetSectorInCollisionList(_hCollisionList) \
    ((_hCollisionList)->hPointerOfSectorSO)
#define SECT_M_ForEachCollisionNodeInCollisionInteractionList(_hSector,_hCollList,_i) \
for                                                        \
(                                                          \
    _i=SECT_fn_iGetNumberOfNodesInCollisionList(_hSector), \
    (_hCollList)=SECT_fn_hGetFirstCollisionNodeInCollisionList(_hSector);  \
    _i>0;                                                  \
    _i--,(_hCollList)=SECT_fn_hGetNextCollisionNodeInCollisionList(_hCollList)  \
)
extern char __cdecl SECT_fn_bIsThisSectorVirtual(void *_hSprObjSector);

#define C_ucTypeZdd  0u
#define C_ucTypeZdm  1u
#define C_ucTypeZde  2u
#define C_ucTypeZdr  3u

extern unsigned long  __cdecl fn_ulStandardGameGetCustomBitsSO(void *hSupObj);  /* 0x413700 */
extern unsigned char  __cdecl fn_ucIsAnAlwaysObject(void *p_stEngineObject);     /* 0x40BC60 */

/* 0x0042DE30 */
void *PO_fn_hGetCollideSet(void *_hPhysicalObject)
{
    return ((PO_tdstPhysicalObject *)_hPhysicalObject)->_hCollideSet;
}

ACP_tdxHandleOfObject PCS_fn_hGetGeoObjOfPhysicalCollSet(unsigned char ucZoneType,
                                                         tdstPhysicalCollSet *_hPhyCollSet)
{
    switch (ucZoneType)
    {
    case C_ucTypeZdd:  return (ACP_tdxHandleOfObject)((_hPhyCollSet)->hZdd);
    case C_ucTypeZdm:  return (ACP_tdxHandleOfObject)((_hPhyCollSet)->hZdm);
    case C_ucTypeZdr:  return (ACP_tdxHandleOfObject)((_hPhyCollSet)->hZdr);
    case C_ucTypeZde:  return (ACP_tdxHandleOfObject)((_hPhyCollSet)->hZde);
    default:           return NULL;
    }
}

/* 0x00494270 */
static ACP_tdxBool fn_bIsGeometricObjectValidForCollision(ACP_tdxHandleOfObject _hGeoObj)
{
    return (ACP_tdxBool)(GEO_xGetGeometricObjectNumberOfPoints(_hGeoObj) >= 1);
}

/* 0x004943C0 */
static void fn_vCollideStaticCharacterWithStaticPhysicals(
    HIE_tdstSuperObject      *_hDynamicCharacter,
    ACP_tdxHandleOfObject     _hDynamicGeoCol,
    HIE_tdstSuperObject      *_hModule,
    POS_tdstCompletePosition *p_stStartMatrix,
    POS_tdstCompletePosition *p_stEndMatrix)
{
    void *hStaticCollSet = PO_fn_hGetCollideSet(HIE_fn_hGetSuperObjectObject(_hModule));

    if (hStaticCollSet)
    {
        ACP_tdxHandleOfObject p_stStaticGeoColl =
            (ACP_tdxHandleOfObject)PCS_fn_hGetGeoObjOfPhysicalCollSet(C_ucTypeZdr, hStaticCollSet);

        if (p_stStaticGeoColl)
        {
            COL_fn_vCollideStaticGeomObj1WithStaticGeomObj2(
                _hDynamicGeoCol, p_stStartMatrix, p_stEndMatrix, p_stStaticGeoColl,
                HIE_fn_hGetSuperObjectGlobalMatrix(_hModule),
                (void *)_hModule, COL_PC_C_wMobileObject,
                (ACP_tdxBool)HIE_fn_SO_bHasNoTransformationMatrix(_hModule),
                (ACP_tdxBool)HIE_fn_SO_bHasZoomInsteadOfScale(_hDynamicCharacter));
        }
    }
}

#define COL_PC_C_ulCustomBit_Projectile  0x00000008u

/* 0x00494420 */
static void fn_vNewTestStaticCollideFor2Characters(HIE_tdstSuperObject *hCharacter,
                                                   ACP_tdxHandleOfObject _hDynamicZdm,
                                                   HIE_tdstSuperObject *hStaticCharacter,
                                                   POS_tdstCompletePosition *p_stStartMatrix,
                                                   POS_tdstCompletePosition *p_stEndMatrix)
{
    GEO_tdxHandleToBoundingSphere hBoundingVolumeCharacter = NULL;
    GEO_tdxHandleToBoundingSphere hBoundingVolumeStaticCharacter = NULL;
    CHN_tdxHandleToChannel hChannel;
    MS_tdxHandleToCollSet hStaticCollSet;
    MS_tdxHandleToCollSet hDynamicCollSet;
    HIE_tdstSuperObject *hMainChar;

    if (! M_ObjectIsActive ((struct tdstEngineObject_ *) HIE_fn_hGetSuperObjectObject (hStaticCharacter)))
    {
        return;
    }

    if ( HIE_fn_SO_bIsNotPickable ( hStaticCharacter ) )
    {
        return;
    }

    hDynamicCollSet = M_GetMSHandle (hCharacter, CollSet);
    hStaticCollSet  = M_GetMSHandle (hStaticCharacter, CollSet);

    if ( CS_fn_ucGetCharacterPriority ( hDynamicCollSet ) < ( hStaticCollSet ? CS_fn_ucGetCharacterPriority( hStaticCollSet ) : 0 ) )
    {
        return;
    }

    /* 0x00500578 */
    hMainChar = GAM_pc_g_hMainCharacterSO;

    if ( hDynamicCollSet )
    {

        if
        (
            fn_bCollSetGetCharacterNoCollisionWithProjectile (hDynamicCollSet) &&
            (fn_ulStandardGameGetCustomBitsSO (hStaticCharacter) & COL_PC_C_ulCustomBit_Projectile)
        )
        {
            return;
        }

        if ( hStaticCharacter == hMainChar )
        {

            if ( fn_bCollSetGetCharacterNoCollisionWithMainCharacter ( hDynamicCollSet ) )
                return ;
        }
        else
        {

            if
            (
                fn_bCollSetGetCharacterNoCollisionWithSecondaryCharacter ( hDynamicCollSet )
                && ! fn_ucIsAnAlwaysObject ( HIE_fn_hGetSuperObjectObject ( hStaticCharacter ) )
            )
                return ;
        }
    }

    if ( hStaticCollSet )
    {

        if
        (
            fn_bCollSetGetCharacterNoCollisionWithProjectile (hStaticCollSet) &&
            (fn_ulStandardGameGetCustomBitsSO (hCharacter) & COL_PC_C_ulCustomBit_Projectile)
        )
        {
            return;
        }

        if ( hMainChar == hCharacter )
        {

            if ( fn_bCollSetGetCharacterNoCollisionWithMainCharacter ( hStaticCollSet ) )
                return ;
        }
        else
        {

            if
            (
                fn_bCollSetGetCharacterNoCollisionWithSecondaryCharacter ( hStaticCollSet )
                && ! fn_ucIsAnAlwaysObject ( HIE_fn_hGetSuperObjectObject ( hCharacter ) )
            )
                return ;
        }
    }

    hBoundingVolumeCharacter = ( GEO_tdxHandleToBoundingSphere ) HIE_fn_hGetSuperObjectBoundingVolume( hCharacter ) ;
    hBoundingVolumeStaticCharacter = ( GEO_tdxHandleToBoundingSphere ) HIE_fn_hGetSuperObjectBoundingVolume ( hStaticCharacter ) ;

    if ( hBoundingVolumeCharacter && hBoundingVolumeStaticCharacter )
    if
    (
        ! COL_fn_bDetectCollisionStaticGeomObjWithStaticGeomObj (
            hBoundingVolumeCharacter, p_stEndMatrix,
            (void *) hBoundingVolumeStaticCharacter ,
            HIE_fn_hGetSuperObjectGlobalMatrix ( hStaticCharacter ) ,
            ( ACP_tdxBool ) HIE_fn_SO_bHasNoTransformationMatrix ( hStaticCharacter ) ,
            ( ACP_tdxBool ) HIE_fn_SO_bHasZoomInsteadOfScale ( hCharacter ) ,
            FALSE
        )
    )
    {
        return;
    }

    if ( hStaticCollSet )
    {
        GEO_tdstGeometricObject * hStaticZdr = (GEO_tdstGeometricObject *) fn_hStaticMakeOneObjectFromManyZdx ( C_ucTypeZdr , hStaticCharacter ) ;

        if ( fn_bIsGeometricObjectValidForCollision (hStaticZdr) )
        {
            COL_fn_vCollideStaticGeomObj1WithStaticGeomObj2(
                _hDynamicZdm, p_stStartMatrix, p_stEndMatrix,
                hStaticZdr, HIE_fn_hGetSuperObjectGlobalMatrix (hStaticCharacter),
                (void *) hStaticCharacter, COL_PC_C_wMobileObject,
                (ACP_tdxBool) HIE_fn_SO_bHasNoTransformationMatrix(hStaticCharacter),
                (ACP_tdxBool) HIE_fn_SO_bHasZoomInsteadOfScale(hCharacter)
            );
        }
    }

    hChannel = fn_h3dDataGetFirstActiveChannel (M_GetMSHandle (hStaticCharacter , 3dData));
    while ( hChannel )
    {
        HIE_tdstSuperObject *hModule = hChannel->hZoomSupObject ? hChannel->hZoomSupObject : hChannel->hSupObject ;

        if
        (
            ( HIE_fn_ulGetSuperObjectType (hModule) == HIE_C_ulPO )
            && ( ! ( HIE_M_xGetSuperObjectMember(hModule, ulFlags) & HIE_C_Flag_ulHidden ) )
        )
        {
            fn_vCollideStaticCharacterWithStaticPhysicals (
                hCharacter ,
                _hDynamicZdm ,
                hModule ,
                p_stStartMatrix , p_stEndMatrix
            ) ;
        }
        hChannel = hChannel -> p_stNextActiveChannel ;
    }
}

void __cdecl COL_fn_vStaticCollisionWithPartOfTreeForSector(HIE_tdstSuperObject *_p_stEngineObj, HIE_tdstSuperObject *_p_stSuperObj, ACP_tdxHandleOfObject _p_stDynamicGeoColl, POS_tdstCompletePosition *_p_stStartMatrix, POS_tdstCompletePosition *_p_stEndMatrix);

void __cdecl COL_fn_vNewStaticCollisionForCharacter(HIE_tdstSuperObject *_hCharacter,
                                                   HIE_tdstSuperObject *_hSector,
                                                   POS_tdstCompletePosition *_p_stStartMatrix,
                                                   POS_tdstCompletePosition *_p_stEndMatrix)
{
    MS_tdxHandleToCollSet hCollSet;
    COL_tdstSectorCharElement *hCharacterList;
    COL_tdstSectorCollisionNode *hCollisionList;
    ACP_tdxHandleOfObject hDynamicZDM;
    HIE_tdstSuperObject *hStaticCharacter;
    GEO_tdxHandleToParallelBox hParallelBoxStatic = NULL;
    GEO_tdxHandleToBoundingSphere hBoundingSphereStatic = NULL;
    MTH3D_tdstVector stMove;
    GEO_tdstParallelBox stBackupParallelBox;
    MTH_tdxReal xBackupSphereRadius;

    long i, j;

    hDynamicZDM = fn_hMakeOneObjectFromManyZdx(C_ucTypeZdm, _hCharacter);

    hCollSet = M_GetMSHandle(_hCharacter, CollSet);

    MTH3D_M_vSubVector(&stMove, POS_fn_p_stGetTranslationVector(_p_stEndMatrix), POS_fn_p_stGetTranslationVector(_p_stStartMatrix));

    if (HIE_fn_SO_bHasABoxBoundingVolume(_hCharacter))
    {
        hParallelBoxStatic = (GEO_tdxHandleToParallelBox)HIE_fn_hGetSuperObjectBoundingVolume(_hCharacter);
        memcpy(&stBackupParallelBox, hParallelBoxStatic, sizeof(GEO_tdstParallelBox));
        if (MTH_M_bGreaterZero(MTH3D_M_xGetXofVector(&stMove)))
            MTH3D_M_vSetXofVector(&(hParallelBoxStatic->stMaxPoint), MTH_M_xAdd(MTH3D_M_xGetXofVector(&(hParallelBoxStatic->stMaxPoint)), MTH3D_M_xGetXofVector(&stMove)))
        else
            MTH3D_M_vSetXofVector(&(hParallelBoxStatic->stMinPoint), MTH_M_xAdd(MTH3D_M_xGetXofVector(&(hParallelBoxStatic->stMinPoint)), MTH3D_M_xGetXofVector(&stMove)))

        if (MTH_M_bGreaterZero(MTH3D_M_xGetYofVector(&stMove)))
            MTH3D_M_vSetYofVector(&(hParallelBoxStatic->stMaxPoint), MTH_M_xAdd(MTH3D_M_xGetYofVector(&(hParallelBoxStatic->stMaxPoint)), MTH3D_M_xGetYofVector(&stMove)))
        else
            MTH3D_M_vSetYofVector(&(hParallelBoxStatic->stMinPoint), MTH_M_xAdd(MTH3D_M_xGetYofVector(&(hParallelBoxStatic->stMinPoint)), MTH3D_M_xGetYofVector(&stMove)))

        if (MTH_M_bGreaterZero(MTH3D_M_xGetZofVector(&stMove)))
            MTH3D_M_vSetZofVector(&(hParallelBoxStatic->stMaxPoint), MTH_M_xAdd(MTH3D_M_xGetZofVector(&(hParallelBoxStatic->stMaxPoint)), MTH3D_M_xGetZofVector(&stMove)))
        else
            MTH3D_M_vSetZofVector(&(hParallelBoxStatic->stMinPoint), MTH_M_xAdd(MTH3D_M_xGetZofVector(&(hParallelBoxStatic->stMinPoint)), MTH3D_M_xGetZofVector(&stMove)))
    }
    else
    {
        hBoundingSphereStatic = (GEO_tdxHandleToBoundingSphere)HIE_fn_hGetSuperObjectBoundingVolume(_hCharacter);
        xBackupSphereRadius = hBoundingSphereStatic->xRadius;
        hBoundingSphereStatic->xRadius = MTH_M_xAdd(hBoundingSphereStatic->xRadius, MTH3D_M_xNormVector(&stMove));
    }

    if (! fn_bCollSetGetCharacterNoCollisionWithProjectile         (hCollSet) ||
        ! fn_bCollSetGetCharacterNoCollisionWithSecondaryCharacter (hCollSet) ||
        ! fn_bCollSetGetCharacterNoCollisionWithMainCharacter      (hCollSet))
    {

        SECT_M_ForEachCharListInSector ( _hSector, hCharacterList, i )
        {
            hStaticCharacter = SECT_GetCharacterInList ( hCharacterList ) ;
            if ( _hCharacter != hStaticCharacter )
            {
                fn_vNewTestStaticCollideFor2Characters (
                    _hCharacter , hDynamicZDM , hStaticCharacter ,
                    _p_stStartMatrix , _p_stEndMatrix
                ) ;
            }
        }

        if (! fn_bCollSetGetCharacterNoCollisionWithOtherSectors (hCollSet))
        {
            SECT_M_ForEachCollisionNodeInCollisionInteractionList ( _hSector , hCollisionList , i )
            {
                SECT_M_ForEachCharListInSector ( SECT_GetSectorInCollisionList ( hCollisionList ) , hCharacterList, j )
                {
                    hStaticCharacter = SECT_GetCharacterInList ( hCharacterList ) ;

                    if ( _hCharacter != hStaticCharacter )
                    {
                        fn_vNewTestStaticCollideFor2Characters (
                            _hCharacter , hDynamicZDM , hStaticCharacter ,
                            _p_stStartMatrix , _p_stEndMatrix
                        ) ;
                    }
                }
            }
        }
    }

    if (! fn_bCollSetGetCharacterNoCollisionWithMap (hCollSet) && ! SECT_fn_bIsThisSectorVirtual (_hSector))
    {

        COL_fn_vStaticCollisionWithPartOfTreeForSector (_hCharacter, _hSector, hDynamicZDM, _p_stStartMatrix, _p_stEndMatrix);

        if (! fn_bCollSetGetCharacterNoCollisionWithOtherSectors (hCollSet))
        {
            SECT_M_ForEachCollisionNodeInCollisionInteractionList ( _hSector, hCollisionList, i )
            {
                HIE_tdstSuperObject *hSectorToCollide = SECT_GetSectorInCollisionList (hCollisionList);
                if (! SECT_fn_bIsThisSectorVirtual (hSectorToCollide))
                {
                    COL_fn_vStaticCollisionWithPartOfTreeForSector (_hCharacter, hSectorToCollide, hDynamicZDM,
                        _p_stStartMatrix, _p_stEndMatrix);
                }
            }
        }
    }

    if (HIE_fn_SO_bHasABoxBoundingVolume(_hCharacter))
    {
        hParallelBoxStatic = (GEO_tdxHandleToParallelBox)HIE_fn_hGetSuperObjectBoundingVolume(_hCharacter);
        memcpy(hParallelBoxStatic, &stBackupParallelBox, sizeof(GEO_tdstParallelBox));
    }
    else
    {
        hBoundingSphereStatic = (GEO_tdxHandleToBoundingSphere)HIE_fn_hGetSuperObjectBoundingVolume(_hCharacter);
        hBoundingSphereStatic->xRadius = xBackupSphereRadius;
    }

    return;

}

/* 0x004948A0 */
void __cdecl COL_fn_vStaticCollisionWithPartOfTreeForSector(HIE_tdstSuperObject *_p_stEngineObj,
                                                            HIE_tdstSuperObject *_p_stSuperObj,
                                                            ACP_tdxHandleOfObject _p_stDynamicGeoColl,
                                                            POS_tdstCompletePosition *_p_stStartMatrix,
                                                            POS_tdstCompletePosition *_p_stEndMatrix)
{
  unsigned long p;
  HIE_tdstSuperObject *pChild;
  void *p_stStaticGeoColl;
  void *hPhysicalObject;
  void *hPhyCollSet;
  GEO_tdxHandleToBoundingSphere hBoundingVolumeCharacter = NULL;
  GEO_tdxHandleToParallelBox hParallelBoxStatic = NULL;
  GEO_tdxHandleToBoundingSphere hBoundingSphereStatic = NULL;
  void *hBoundingVolumeStatic;
  long lType;

  if (HIE_fn_SO_bIsNotPickable(_p_stSuperObj))
  {
      return;
  }

  hBoundingVolumeCharacter = HIE_fn_hGetSuperObjectBoundingVolume(_p_stEngineObj);
  if (HIE_fn_SO_bHasABoxBoundingVolume(_p_stSuperObj))
  {
      hParallelBoxStatic = (GEO_tdxHandleToParallelBox)HIE_fn_hGetSuperObjectBoundingVolume(_p_stSuperObj);
      hBoundingVolumeStatic = (void *)hParallelBoxStatic;
  }
  else
  {
      hBoundingSphereStatic = (GEO_tdxHandleToBoundingSphere)HIE_fn_hGetSuperObjectBoundingVolume(_p_stSuperObj);
      hBoundingVolumeStatic = (void *)hBoundingSphereStatic;
  }

  if (hBoundingVolumeCharacter && hBoundingVolumeStatic)
  {
      if (! COL_fn_bDetectCollisionStaticGeomObjWithStaticGeomObj(hBoundingVolumeCharacter, _p_stEndMatrix,
                 hBoundingVolumeStatic, HIE_fn_hGetSuperObjectGlobalMatrix(_p_stSuperObj),
                 (ACP_tdxBool) HIE_fn_SO_bHasNoTransformationMatrix(_p_stSuperObj),
                 (ACP_tdxBool) HIE_fn_SO_bHasZoomInsteadOfScale(_p_stEngineObj),
                 (ACP_tdxBool) HIE_fn_SO_bHasABoxBoundingVolume(_p_stSuperObj)))
      {
          if (HIE_fn_SO_bCheckChildren(_p_stSuperObj))
          {
              HIE_M_ForEachChildOf(_p_stSuperObj, pChild, p)
              {
                  COL_fn_vStaticCollisionWithPartOfTreeForSector(_p_stEngineObj, pChild, _p_stDynamicGeoColl, _p_stStartMatrix, _p_stEndMatrix);
              }
              return;
          }
          else
              return;
      }
  }

  lType = HIE_fn_ulGetSuperObjectType(_p_stSuperObj);

  if (lType & (HIE_C_ulIPO | HIE_C_ulIPO_Mirror))
  {
      hPhysicalObject = IPO_fn_hGetPhysicalObject(HIE_fn_hGetSuperObjectObject(_p_stSuperObj));
      hPhyCollSet = PO_fn_hGetCollideSet(hPhysicalObject);
      if (hPhyCollSet)
      {
          p_stStaticGeoColl = PCS_fn_hGetZdrGeoObjOfPhysicalCollSet(hPhyCollSet);
          COL_fn_vCollideStaticGeomObj1WithStaticGeomObj2(_p_stDynamicGeoColl, _p_stStartMatrix, _p_stEndMatrix,
              (ACP_tdxHandleOfObject)p_stStaticGeoColl, HIE_fn_hGetSuperObjectGlobalMatrix(_p_stSuperObj),
              (void *) _p_stSuperObj, COL_PC_C_wStaticObject,
              (ACP_tdxBool) HIE_fn_SO_bHasNoTransformationMatrix(_p_stSuperObj),
              (ACP_tdxBool) HIE_fn_SO_bHasZoomInsteadOfScale(_p_stEngineObj));
      }
  }

  HIE_M_ForEachChildOf(_p_stSuperObj, pChild, p)
  {
      COL_fn_vStaticCollisionWithPartOfTreeForSector(_p_stEngineObj, pChild, _p_stDynamicGeoColl, _p_stStartMatrix, _p_stEndMatrix);
  }
}
#define COL_PC_HIE_C_ulPO                0x00000008u
#define COL_PC_HIE_C_Flag_ulHidden       0x00000002u
