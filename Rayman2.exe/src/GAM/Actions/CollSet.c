#include "GAM/GAM_types.h"
#include "MTH/MTH_types.h"
#include "GAM/Actions/CollSet.h"
#include "ERM/CpaError.h"
#include "SAI/SAI.h"
#include "GAM/ZdxStuff.h"
#include "GAM/Family.h"
#include "GAM/Actions/3dData.h"

#define M_GAM_CollSetFlag(hCollSet) \
    (((MS_tdxHandleToCollSet)(hCollSet))->ucCollisionFlag)

#define C_ucCollSet_NO_COLLISION_WITH_MAP                  0x01
#define C_ucCollSet_NO_COLLISION_WITH_PROJECTILE           0x02
#define C_ucCollSet_NO_COLLISION_WITH_SECONDARY_CHARACTER  0x04
#define C_ucCollSet_NO_COLLISION_WITH_MAIN_CHARACTER       0x08
#define C_ucCollSet_NO_COLLISION_WITH_OTHER_SECTORS        0x20
#define C_ucCollSet_NO_COLLISION_ZDE_WITH_PROJECTILE       0x40

#define M_GAM_CollSetFlagW(hCollSet) \
    (((MS_tdxHandleToCollSet)(hCollSet))->ucCollisionFlag)

#define C_ucCollSet_FORCE_COLLISION_WHEN_NOT_MOVING 0x10

#define M_GAM_SOGetCollSet(hSO) M_GetMSHandle(hSO, CollSet)

#define CS_M_SAIInitFlags            0x13u

/* 0x004077e0 */
typedef void *(__cdecl *tGameMallocInHLM)(unsigned int ulSize);
#include "GAM/GAM_globals.h"
#define M_p_GameMallocInHLM fnp_vGameMallocInHLM

/* 0x004078C0 */
extern void __cdecl fn_vGameFreeInHLM(void *p_vObject);
#define M_GameFreeInHLM fn_vGameFreeInHLM

#define GAM_g_ucErmModuleId      g_ucGameModuleId   /* 0x0049F630 */
#define GAM_C_uwGameWarningsBase 0x30u
#define E_uwGameMiniStructObjectNull 0x0eu
#define E_uwGameMiniStructNull       0x0fu

#define M_GAM_EngObjCollSet(p_stObject) (((struct tdstEngineObject_ *)(p_stObject))->h_CollSet)

/* 0x0040CB00 */
extern void fn_vCorrectActivationList(void *hActivationList, long lNumberOfState);

/* 0x00417450 */
int __cdecl fn_ulCollSetSizeOf(void)
{
    return 0x68;
}

/* 0x00417460 */
void __cdecl fn_vCollSetAlloc(HIE_tdstEngineObject *engineObject)
{
    struct tdstEngineObject_ *pEngObj = (struct tdstEngineObject_ *)engineObject;
    void *hCollSet;

    if (engineObject == NULL) {

        ERM_fn_vClearLastError((unsigned long)engineObject);
        ERM_fn_vUpdateLastError(0x30u, g_ucGameModuleId,
            (unsigned long)engineObject, 0x0eu, -1,
            0xff, 0xff, (char *)engineObject);
    }

    if (pEngObj->h_CollSet != NULL) {

        ERM_fn_vClearLastError(0);
        ERM_fn_vUpdateLastError(0x30u, g_ucGameModuleId,
            0, 0x0du, -1, 0xff, 0xff, (char *)0);
    }

    hCollSet = fn_h_CollSetRealAlloc();
    pEngObj->h_CollSet = hCollSet;
    fn_vCollSetInitValueSAI(hCollSet);
}

void * __cdecl fn_h_CollSetRealAlloc(void)
{
    tdstCollSet *h_CollSet;

    h_CollSet = (tdstCollSet*)M_p_GameMallocInHLM(sizeof(tdstCollSet));

    h_CollSet->ucCharacterPriority = 0;
    h_CollSet->hZddList = NULL;
    h_CollSet->hZdeList = NULL;
    h_CollSet->hZdmList = NULL;
    h_CollSet->hZdrList = NULL;
    h_CollSet->hZddActivationList = NULL;
    h_CollSet->hZdeActivationList = NULL;
    h_CollSet->hZdmActivationList = NULL;
    h_CollSet->hZdrActivationList = NULL;

    h_CollSet->stColliderInfo.ucColliderPriority = 0;

    h_CollSet->ulFBZddPriviligedZones   = 0;
    h_CollSet->ulFBZdePriviligedZones   = 0;
    h_CollSet->ulFBZdmPriviligedZones   = 0;
    h_CollSet->ulFBZdrPriviligedZones   = 0;
    h_CollSet->ucCollComputeFrequency   = 1;

    return(h_CollSet);
}

void __cdecl fn_vCollSetFree(void *p_stObject)
{
    if (p_stObject == NULL) {

        ERM_fn_vClearLastError((unsigned long)p_stObject);
        ERM_fn_vUpdateLastError(GAM_C_uwGameWarningsBase, GAM_g_ucErmModuleId,
            (unsigned long)p_stObject, E_uwGameMiniStructObjectNull, -1,
            0xff, 0xff, (char *)p_stObject);
    }

    if (M_GAM_EngObjCollSet(p_stObject) == NULL) {

        ERM_fn_vClearLastError(0);
        ERM_fn_vUpdateLastError(GAM_C_uwGameWarningsBase, GAM_g_ucErmModuleId,
            0, E_uwGameMiniStructNull, -1, 0xff, 0xff, (char *)0);
    }

    fn_vCollSetFreeInitValueSAI(M_GAM_EngObjCollSet(p_stObject));  /* 0x00417de0 */
    M_GameFreeInHLM(M_GAM_EngObjCollSet(p_stObject));              /* 0x004078c0 */
    M_GAM_EngObjCollSet(p_stObject) = NULL;       /* 0x0041759e */
}
#define GAM_fn_vCorrectActivationList fn_vCorrectActivationList

/* 0x417db0 */
void __cdecl fn_vCollSetSavedValueSAI(void *hCollSet)
{
}

#define OTI_WhenGameStart  0
#define OTI_MapLoaded      1
#define OTI_ReinitTheMap   2
#define OTI_LoadSavedGame  3
#define OTI_PlayerDead     4
#define OTI_AlwaysCreated  5

/* 0x004175B0 */
void __cdecl fn_vCollSetInit(void *p_stObject, int eObjectInit)
{
    if (p_stObject == NULL) {

        ERM_fn_vClearLastError(0);
        ERM_fn_vUpdateLastError(GAM_C_uwGameWarningsBase, GAM_g_ucErmModuleId,
            0, E_uwGameMiniStructObjectNull, -1, 0xff, 0xff, (char *)0);
    }

    if (((struct tdstEngineObject_ *)p_stObject)->h_CollSet == NULL) {

        ERM_fn_vClearLastError(0);
        ERM_fn_vUpdateLastError(GAM_C_uwGameWarningsBase, GAM_g_ucErmModuleId,
            0, E_uwGameMiniStructNull, -1, 0xff, 0xff, (char *)0);
    }

    switch (eObjectInit) {
    case OTI_AlwaysCreated:
        break;
    case OTI_MapLoaded:
        fn_vCollSetSavedValueSAI(((struct tdstEngineObject_ *)p_stObject)->h_CollSet);   /* 0x00403290 */

    case OTI_LoadSavedGame:
    case OTI_WhenGameStart:
        fn_vCollSetInitValueSAI(((struct tdstEngineObject_ *)p_stObject)->h_CollSet);
        ((MS_tdxHandleToCollSet)((struct tdstEngineObject_ *)p_stObject)->h_CollSet)->stColliderInfo.ucColliderPriority = 0;
        ((MS_tdxHandleToCollSet)((struct tdstEngineObject_ *)p_stObject)->h_CollSet)->ulFBZddPriviligedZones = 0;
        ((MS_tdxHandleToCollSet)((struct tdstEngineObject_ *)p_stObject)->h_CollSet)->ulFBZdePriviligedZones = 0;
        ((MS_tdxHandleToCollSet)((struct tdstEngineObject_ *)p_stObject)->h_CollSet)->ulFBZdmPriviligedZones = 0;
        ((MS_tdxHandleToCollSet)((struct tdstEngineObject_ *)p_stObject)->h_CollSet)->ulFBZdrPriviligedZones = 0;

        fn_vCorrectAllActivationLists(p_stObject);
        break;
    case OTI_PlayerDead:
    case OTI_ReinitTheMap:
        ((MS_tdxHandleToCollSet)((struct tdstEngineObject_ *)p_stObject)->h_CollSet)->stColliderInfo.ucColliderPriority = 0;
        ((MS_tdxHandleToCollSet)((struct tdstEngineObject_ *)p_stObject)->h_CollSet)->ulFBZddPriviligedZones = 0;
        ((MS_tdxHandleToCollSet)((struct tdstEngineObject_ *)p_stObject)->h_CollSet)->ulFBZdePriviligedZones = 0;
        ((MS_tdxHandleToCollSet)((struct tdstEngineObject_ *)p_stObject)->h_CollSet)->ulFBZdmPriviligedZones = 0;
        ((MS_tdxHandleToCollSet)((struct tdstEngineObject_ *)p_stObject)->h_CollSet)->ulFBZdrPriviligedZones = 0;
        break;
    }
}

/* 0x004176db */
void __cdecl fn_vCollSetDesinit(void *p_stObject, int eObjectInit)
{
    if (p_stObject == NULL) {

        ERM_fn_vClearLastError((unsigned long)p_stObject);
        ERM_fn_vUpdateLastError(GAM_C_uwGameWarningsBase, GAM_g_ucErmModuleId,
            (unsigned long)p_stObject, E_uwGameMiniStructObjectNull, -1,
            0xff, 0xff, (char *)p_stObject);
    }

    if (M_GAM_EngObjCollSet(p_stObject) == NULL) {

        ERM_fn_vClearLastError(0);
        ERM_fn_vUpdateLastError(GAM_C_uwGameWarningsBase, GAM_g_ucErmModuleId,
            0, E_uwGameMiniStructNull, -1, 0xff, 0xff, (char *)0);
    }

    (void)eObjectInit;
}
int __cdecl fn_ulCollSetSizeOf(void);

void __cdecl fn_vCollSetCopyClone(HIE_tdstEngineObject *pDest,
                                      HIE_tdstEngineObject *pSrc)
{

    memcpy(((struct tdstEngineObject_ *)pDest)->h_CollSet,
           ((struct tdstEngineObject_ *)pSrc)->h_CollSet,
           (long)fn_ulCollSetSizeOf() - sizeof(struct stColliderInfo_));
}

unsigned char __cdecl fn_ucCollSetGetColliderPriority(void *hCollSet)
{
    return ((MS_tdxHandleToCollSet)hCollSet)->stColliderInfo.ucColliderPriority;
}

/* 0x00417750 */
unsigned char __cdecl fn_ucCollSetGetColliderType(void *hCollSet)
{
    return ((MS_tdxHandleToCollSet)hCollSet)->stColliderInfo.ucColliderType;
}

/* 0x00417760 */
MTH3D_tdstVector __cdecl fn_stCollSetGetColliderVector(void *hCollSet, short wIndex)
{

    MTH3D_tdstVector stNullVect = { 0.0f, 0.0f, 0.0f };

    return ( wIndex >= 0 && wIndex < C_wCollsetNbCollisionVectors )
    ? ((MS_tdxHandleToCollSet)hCollSet)->stColliderInfo.a_stColliderVectors[wIndex]
    : stNullVect;
}

float __cdecl fn_xCollSetGetColliderReal(void *hCollSet, short wIndex)
{
    return (wIndex >= 0 && wIndex < C_wCollsetNbCollisionReals)
        ? ((MS_tdxHandleToCollSet)hCollSet)->stColliderInfo.a_xColliderReals[wIndex]
        : 0.0f;
}

void __cdecl fn_vCollSetSetColliderPriority(void *hCollSet, unsigned char ucPriority)
{
    ((MS_tdxHandleToCollSet)hCollSet)->stColliderInfo.ucColliderPriority = ucPriority;
}

void __cdecl fn_vCollSetSetColliderType(void *hCollSet, unsigned char ucType)
{
    ((MS_tdxHandleToCollSet)hCollSet)->stColliderInfo.ucColliderType = ucType;
}

void __cdecl fn_vCollSetSetColliderVector(void *hCollSet, short wIndex,
                                              MTH3D_tdstVector *p_stVector)
{
    if (wIndex >= 0 && wIndex < C_wCollsetNbCollisionVectors)
        ((MS_tdxHandleToCollSet)hCollSet)->stColliderInfo.a_stColliderVectors[wIndex] = *p_stVector;
}

void __cdecl fn_vCollSetSetColliderReal(void *hCollSet, short wIndex, float xReal)
{
    if (wIndex >= 0 && wIndex < C_wCollsetNbCollisionReals)
        ((MS_tdxHandleToCollSet)hCollSet)->stColliderInfo.a_xColliderReals[wIndex] = xReal;
}

/* 0x417870 */
void * __cdecl CS_fn_hGetZdxList(unsigned char ucZoneType, void *hSuperObject)
{
   if( !M_GetMSHandle(hSuperObject,CollSet) )
      return NULL;
   switch(ucZoneType)
   {
   case C_ucTypeZdd:
      return( CS_M_GetZdxList(Zdd,M_GetMSHandle(hSuperObject,CollSet)) );
      break;
   case C_ucTypeZde:
      return( CS_M_GetZdxList(Zde,M_GetMSHandle(hSuperObject,CollSet)) );
      break;
   case C_ucTypeZdm:
      return( CS_M_GetZdxList(Zdm,M_GetMSHandle(hSuperObject,CollSet)) );
      break;
   case C_ucTypeZdr:
      return( CS_M_GetZdxList(Zdr,M_GetMSHandle(hSuperObject,CollSet)) );
      break;
   default:
      return NULL;
      break;
   }
}

void * __cdecl CS_fn_hGetActivationList(unsigned char ucZoneType, void *hSuperObject)
{
    void *hCollSet = hSuperObject ? M_GAM_SOGetCollSet(hSuperObject) : NULL;
    return hCollSet ? CS_fn_hGetActivationListInCollSet(ucZoneType, hCollSet) : NULL;
}

void * __cdecl CS_fn_hGetActivationListInCollSet(unsigned char ucZoneType, void *hCollSet)
{
   switch(ucZoneType)
   {
   case C_ucTypeZdd:
      return( CS_M_GetActivationList(Zdd,((MS_tdxHandleToCollSet)hCollSet)) );
      break;
   case C_ucTypeZde:
      return( CS_M_GetActivationList(Zde,((MS_tdxHandleToCollSet)hCollSet)) );
      break;
   case C_ucTypeZdm:
      return( CS_M_GetActivationList(Zdm,((MS_tdxHandleToCollSet)hCollSet)) );
      break;
   case C_ucTypeZdr:
      return( CS_M_GetActivationList(Zdr,((MS_tdxHandleToCollSet)hCollSet)) );
      break;
   default:
      return NULL;
      break;
   }
}

unsigned char __cdecl CS_fn_ucGetCharacterPriority(void *hCollSet)
{
    return ((MS_tdxHandleToCollSet)hCollSet)->ucCharacterPriority;
}

void __cdecl CS_fn_vSetCharacterPriority(void *hCollSet, unsigned char ucPriority)
{
    ((MS_tdxHandleToCollSet)hCollSet)->ucCharacterPriority = ucPriority;
}

ACP_tdxBool __cdecl fn_bCollSetGetCharacterNoCollisionWithMap(void *hCollSet)
{
    return (ACP_tdxBool)(M_GAM_CollSetFlag(hCollSet) & C_ucCollSet_NO_COLLISION_WITH_MAP);
}

ACP_tdxBool __cdecl fn_bCollSetGetCharacterNoCollisionWithProjectile(void *hCollSet)
{
    return (ACP_tdxBool)(M_GAM_CollSetFlag(hCollSet) & C_ucCollSet_NO_COLLISION_WITH_PROJECTILE);
}

ACP_tdxBool __cdecl fn_bCollSetGetCharacterNoCollisionWithSecondaryCharacter(void *hCollSet)
{
    return (ACP_tdxBool)(M_GAM_CollSetFlag(hCollSet) & C_ucCollSet_NO_COLLISION_WITH_SECONDARY_CHARACTER);
}

ACP_tdxBool __cdecl fn_bCollSetGetCharacterNoCollisionWithMainCharacter(void *hCollSet)
{
    return (ACP_tdxBool)(M_GAM_CollSetFlag(hCollSet) & C_ucCollSet_NO_COLLISION_WITH_MAIN_CHARACTER);
}

ACP_tdxBool __cdecl fn_bCollSetGetCharacterNoCollisionWithOtherSectors(void *hCollSet)
{
    return (ACP_tdxBool)(M_GAM_CollSetFlag(hCollSet) & C_ucCollSet_NO_COLLISION_WITH_OTHER_SECTORS);
}

ACP_tdxBool __cdecl fn_bCollSetGetCharacterNoCollisionZdeWithProjectile(void *hCollSet)
{
    return (ACP_tdxBool)(M_GAM_CollSetFlag(hCollSet) & C_ucCollSet_NO_COLLISION_ZDE_WITH_PROJECTILE);
}

void __cdecl fn_vCollSetSetCharacterNoCollisionWithMap(void *hCollSet, ACP_tdxBool bValue)
{
    if (bValue) M_GAM_CollSetFlagW(hCollSet) |=  C_ucCollSet_NO_COLLISION_WITH_MAP;
    else        M_GAM_CollSetFlagW(hCollSet) &= (unsigned char)~C_ucCollSet_NO_COLLISION_WITH_MAP;
}

void __cdecl fn_vCollSetSetCharacterNoCollisionWithProjectile(void *hCollSet, ACP_tdxBool bValue)
{
    if (bValue) M_GAM_CollSetFlagW(hCollSet) |=  C_ucCollSet_NO_COLLISION_WITH_PROJECTILE;
    else        M_GAM_CollSetFlagW(hCollSet) &= (unsigned char)~C_ucCollSet_NO_COLLISION_WITH_PROJECTILE;
}

void __cdecl fn_vCollSetSetCharacterNoCollisionWithSecondaryCharacter(void *hCollSet, ACP_tdxBool bValue)
{
    if (bValue) M_GAM_CollSetFlagW(hCollSet) |=  C_ucCollSet_NO_COLLISION_WITH_SECONDARY_CHARACTER;
    else        M_GAM_CollSetFlagW(hCollSet) &= (unsigned char)~C_ucCollSet_NO_COLLISION_WITH_SECONDARY_CHARACTER;
}

void __cdecl fn_vCollSetSetCharacterNoCollisionWithMainCharacter(void *hCollSet, ACP_tdxBool bValue)
{
    if (bValue) M_GAM_CollSetFlagW(hCollSet) |=  C_ucCollSet_NO_COLLISION_WITH_MAIN_CHARACTER;
    else        M_GAM_CollSetFlagW(hCollSet) &= (unsigned char)~C_ucCollSet_NO_COLLISION_WITH_MAIN_CHARACTER;
}

void __cdecl fn_vCollSetSetCharacterNoCollisionWithOtherSectors(void *hCollSet, ACP_tdxBool bValue)
{
    if (bValue) M_GAM_CollSetFlagW(hCollSet) |=  C_ucCollSet_NO_COLLISION_WITH_OTHER_SECTORS;
    else        M_GAM_CollSetFlagW(hCollSet) &= (unsigned char)~C_ucCollSet_NO_COLLISION_WITH_OTHER_SECTORS;
}

void __cdecl fn_vCollSetSetCharacterNoCollisionZdeWithProjectile(void *hCollSet, ACP_tdxBool bValue)
{
    if (bValue) M_GAM_CollSetFlagW(hCollSet) |=  C_ucCollSet_NO_COLLISION_ZDE_WITH_PROJECTILE;
    else        M_GAM_CollSetFlagW(hCollSet) &= (unsigned char)~C_ucCollSet_NO_COLLISION_ZDE_WITH_PROJECTILE;
}

ACP_tdxBool __cdecl fn_bGetCharacterForceCollisionWhenNotMoving(void *hSuperObject)
{
    MS_tdxHandleToCollSet h_CollSet = hSuperObject ? M_GAM_SOGetCollSet(hSuperObject) : NULL;

    return (ACP_tdxBool) (h_CollSet ? h_CollSet -> ucCollisionFlag & C_ucCollSet_FORCE_COLLISION_WHEN_NOT_MOVING : FALSE);
}

void __cdecl fn_vCollSetSetCollComputeFrequency(void *hCollSet, unsigned char ucFrequency)
{
    ((MS_tdxHandleToCollSet)hCollSet)->ucCollComputeFrequency = ucFrequency;
}

unsigned char __cdecl fn_ucCollSetGetCollComputeFrequency(void *hCollSet)
{
    return (unsigned char)(hCollSet ? ((MS_tdxHandleToCollSet)hCollSet)->ucCollComputeFrequency : 1);
}

unsigned char __cdecl fn_ucCollSetGetCollComputeFrequencySO(void *hSuperObject)
{
    void *hCollSet = hSuperObject ? M_GAM_SOGetCollSet(hSuperObject) : NULL;
    return (unsigned char)(hCollSet ? fn_ucCollSetGetCollComputeFrequency(hCollSet) : 1);
}

void __cdecl fn_vCorrectAllActivationLists(void *p_stObject)
{
   if( ((struct tdstEngineObject_ *)p_stObject)->h_CollSet )
   {
      long lNumberOfState;
      MS_tdxHandleToCollSet hCollSet;
      tdstFamilyList *  hFamily;

      hCollSet = ((struct tdstEngineObject_ *)p_stObject)->h_CollSet;
      hFamily = (tdstFamilyList *)((MS_tdxHandleTo3dData)((struct tdstEngineObject_ *)p_stObject)->h_3dData)->h_Family;
      lNumberOfState = hFamily->hForStateArray_count;
      if( hCollSet->hZddActivationList )
         GAM_fn_vCorrectActivationList(hCollSet->hZddActivationList,lNumberOfState);
      if( hCollSet->hZdeActivationList )
         GAM_fn_vCorrectActivationList(hCollSet->hZdeActivationList,lNumberOfState);
      if( hCollSet->hZdmActivationList )
         GAM_fn_vCorrectActivationList(hCollSet->hZdmActivationList,lNumberOfState);
      if( hCollSet->hZdrActivationList )
         GAM_fn_vCorrectActivationList(hCollSet->hZdrActivationList,lNumberOfState);
   }
}

void __cdecl fn_vSetCurrentZoneSetOfType(unsigned char ucZoneType, void *hCollSet,
                                             void *hCurrentZoneSet)
{
   switch( ucZoneType )
   {
   case C_ucTypeZdd:
      ((MS_tdxHandleToCollSet)hCollSet)->hZddCurrentActivation = hCurrentZoneSet;
      break;
   case C_ucTypeZdr:
      ((MS_tdxHandleToCollSet)hCollSet)->hZdrCurrentActivation = hCurrentZoneSet;
      break;
   case C_ucTypeZde:
      ((MS_tdxHandleToCollSet)hCollSet)->hZdeCurrentActivation = hCurrentZoneSet;
      break;
   case C_ucTypeZdm:
      ((MS_tdxHandleToCollSet)hCollSet)->hZdmCurrentActivation = hCurrentZoneSet;
      break;
   }
}

void * __cdecl fn_hGetCurrentZoneSetOfType(unsigned char ucZoneType, void *hCollSet)
{
   switch( ucZoneType )
   {
   case C_ucTypeZdd:
      return( ((MS_tdxHandleToCollSet)hCollSet)->hZddCurrentActivation );
      break;
   case C_ucTypeZdr:
      return( ((MS_tdxHandleToCollSet)hCollSet)->hZdrCurrentActivation );
      break;
   case C_ucTypeZde:
      return( ((MS_tdxHandleToCollSet)hCollSet)->hZdeCurrentActivation );
      break;
   case C_ucTypeZdm:
      return( ((MS_tdxHandleToCollSet)hCollSet)->hZdmCurrentActivation );
      break;
   }
   return (void *)(unsigned int)ucZoneType;
}

unsigned long __cdecl CS_fn_ulGetPrivilegedActivationZoneInCollSet(unsigned char ucZoneType, void *hCollSet)
{
   switch( ucZoneType )
   {
   case C_ucTypeZdd:
      return CS_M_GetPrivilegedZone(Zdd,((MS_tdxHandleToCollSet)hCollSet));
      break;
   case C_ucTypeZdr:
      return CS_M_GetPrivilegedZone(Zdr,((MS_tdxHandleToCollSet)hCollSet));
      break;
   case C_ucTypeZde:
      return CS_M_GetPrivilegedZone(Zde,((MS_tdxHandleToCollSet)hCollSet));
      break;
   case C_ucTypeZdm:
      return CS_M_GetPrivilegedZone(Zdm,((MS_tdxHandleToCollSet)hCollSet));
      break;
   }
   return ucZoneType;
}

void __cdecl CS_fn_vSetPrivilegedActivationZoneInCollSet(unsigned char ucZoneType, void *hCollSet,
                                                         unsigned long ulFB)
{
   switch( ucZoneType )
   {
   case C_ucTypeZdd:
      CS_M_SetPrivilegedZone(Zdd,((MS_tdxHandleToCollSet)hCollSet),ulFB);
      break;
   case C_ucTypeZdr:
      CS_M_SetPrivilegedZone(Zdr,((MS_tdxHandleToCollSet)hCollSet),ulFB);
      break;
   case C_ucTypeZde:
      CS_M_SetPrivilegedZone(Zde,((MS_tdxHandleToCollSet)hCollSet),ulFB);
      break;
   case C_ucTypeZdm:
      CS_M_SetPrivilegedZone(Zdm,((MS_tdxHandleToCollSet)hCollSet),ulFB);
      break;
   }
}

void __cdecl CS_fn_vSetPrivilegedActivationZone(unsigned char ucZoneType, void *hCollSet,
                                                short wIndex, unsigned char ePrivilege)
{
    short         wDecal = (short)(wIndex << 1);
    unsigned long ulFB   = CS_fn_ulGetPrivilegedActivationZoneInCollSet(ucZoneType, hCollSet);
    switch (ePrivilege) {
    case 0:
        ulFB &= ~((unsigned long)(3 << wDecal));
        break;
    case 1:
        ulFB &= ~((unsigned long)(3 << wDecal));
        ulFB |= (unsigned long)(1 << wDecal);
        break;
    case 2:
        ulFB &= ~((unsigned long)(3 << wDecal));
        ulFB |= (unsigned long)(2 << wDecal);
        break;
    }
    CS_fn_vSetPrivilegedActivationZoneInCollSet(ucZoneType, hCollSet, ulFB);
}

unsigned char __cdecl CS_fn_eGetPrivilegedActivationZone(unsigned char ucZoneType, void *hCollSet,
                                                         short wIndex)
{
    unsigned long ulFB = CS_fn_ulGetPrivilegedActivationZoneInCollSet(ucZoneType, hCollSet);
    short         wDecal;
    unsigned char ucFlags;
    if (ulFB == 0)
        return 0;
    wDecal  = (short)(wIndex << 1);
    ucFlags = (unsigned char)((ulFB & (3 << wDecal)) >> wDecal);
    switch (ucFlags) {
    case 0: return 0;
    case 1: return 1;
    case 2: return 2;
    }
    return 3;
}

void __cdecl fn_vCollSetInitValueSAI(void *hCollSet)
{

    SAI_fn_bInsertCharInTable((void*)&((MS_tdxHandleToCollSet)hCollSet)->ucCollisionFlag,		CS_M_SAIInitFlags);
    SAI_fn_bInsertCharInTable((void*)&((MS_tdxHandleToCollSet)hCollSet)->ucCollComputeFrequency,CS_M_SAIInitFlags);
}

void __cdecl fn_vCollSetFreeInitValueSAI(void *hCollSet)
{

    SAI_fn_bRemoveValueFromTable((void*)&((MS_tdxHandleToCollSet)hCollSet)->ucCollisionFlag,		CS_M_SAIInitFlags);
    SAI_fn_bRemoveValueFromTable((void*)&((MS_tdxHandleToCollSet)hCollSet)->ucCollComputeFrequency,	CS_M_SAIInitFlags);
}
