#include <string.h>
#include "AI/WPgraphe.h"
#define D_MSWay_StructureDefine
#include "GAM/MSWay.h"
#include "GAM/GAM_globals.h"
#include "ERM/CpaError.h"
#include "GAM/GAM_types.h"

/* 0x00412BD0 */
unsigned long __cdecl fn_ulMSWaySizeOf(void)
{
    return 0x0cu;
}

long WPG_fn_lGetLinkCapabilities (WP_tdHandleOfGraph hGraph, WP_tdhWayPoint hSrcWayPoint,
                                  WP_tdhWayPoint hDstWayPoint, unsigned long* p_ulCapability);
long WPG_fn_lGetLinkWeight (WP_tdHandleOfGraph hGraph, WP_tdhWayPoint hSrcWayPoint,
                            WP_tdhWayPoint hDstWayPoint, long *p_lWeight);
long WPG_fn_lChangeLinkWeight (WP_tdHandleOfGraph hGraph, WP_tdhWayPoint hSrcWayPoint,
                               WP_tdhWayPoint hDstWayPoint, long lWeight);
long WPG_fn_lChangeLinkCapabilities (WP_tdHandleOfGraph hGraph, WP_tdhWayPoint hSrcWayPoint,
                                     WP_tdhWayPoint hDstWayPoint, unsigned long ulCapability, long lValue);

#define E_uwGameMiniStructObjectNull_W      ((unsigned short)0xE)
#define E_uwGameMiniStructAllocAlreadyAlloc_W ((unsigned short)0xD)
#define E_uwGameMiniStructNull_W            ((unsigned short)0xF)

extern void *__cdecl fn_h_MSWayRealAlloc(void);            /* 0x00416E60 */
extern void __cdecl fn_vGameFreeInHLM(void *p_vObject);
extern unsigned long __cdecl fn_ulMSWaySizeOf(void);        /* 0x00412BD0 */

/* 0x00416DF0 */
void __cdecl fn_vMSWayAlloc(struct tdstEngineObject_ *p_stObject)
{
    if (p_stObject == NULL)
    {
        ERM_fn_vClearLastError(0);
        ERM_fn_vUpdateLastError(0x30, g_ucGameModuleId, 0, E_uwGameMiniStructObjectNull_W, -1, 0xff, 0xff, NULL);
    }
    if (p_stObject->h_MSWay != NULL)
    {
        ERM_fn_vClearLastError(0);
        ERM_fn_vUpdateLastError(0x30, g_ucGameModuleId, 0, E_uwGameMiniStructAllocAlreadyAlloc_W, -1, 0xff, 0xff, NULL);
    }
    p_stObject->h_MSWay = fn_h_MSWayRealAlloc();
}

void MSWay_fn_vInitPath (MS_tdxHandleToMSWay hWay);

/* 0x00416E60 */
void *__cdecl fn_h_MSWayRealAlloc(void)
{
    void *h_MSWay;

    h_MSWay = fnp_vGameMallocInHLM((unsigned int)fn_ulMSWaySizeOf());
    MSWay_fn_vInitPath(h_MSWay);
    return h_MSWay;
}

void MSWay_fn_vDesinitPath(MS_tdxHandleToMSWay hWay);

/* 0x00416E80 */
void __cdecl fn_vMSWayFree(struct tdstEngineObject_ *p_stObject)
{
    void *hMSWay;

    if (p_stObject == NULL)
    {
        ERM_fn_vClearLastError(0);
        ERM_fn_vUpdateLastError(0x30, g_ucGameModuleId, 0, E_uwGameMiniStructObjectNull_W, -1, 0xff, 0xff, NULL);
    }
    hMSWay = p_stObject->h_MSWay;
    if (hMSWay == NULL)
    {
        ERM_fn_vClearLastError(0);
        ERM_fn_vUpdateLastError(0x30, g_ucGameModuleId, 0, E_uwGameMiniStructNull_W, -1, 0xff, 0xff, NULL);
    }
    MSWay_fn_vDesinitPath(hMSWay);
    fn_vGameFreeInHLM(p_stObject->h_MSWay);
    p_stObject->h_MSWay = NULL;
}

/* 0x00416F10 */
void __cdecl fn_vMSWayInit(struct tdstEngineObject_ *p_stObject, int eObjectInit)
{
    void *hMSWay;

    if (p_stObject == NULL)
    {
        ERM_fn_vClearLastError(0);
        ERM_fn_vUpdateLastError(0x30, g_ucGameModuleId, 0, E_uwGameMiniStructObjectNull_W, -1, 0xff, 0xff, NULL);
    }
    hMSWay = p_stObject->h_MSWay;
    if (hMSWay == NULL)
    {
        ERM_fn_vClearLastError(0);
        ERM_fn_vUpdateLastError(0x30, g_ucGameModuleId, 0, E_uwGameMiniStructNull_W, -1, 0xff, 0xff, NULL);
    }
    MSWay_fn_vInitPath(hMSWay);
}

/* 0x00416F80 */
void __cdecl fn_vMSWayDesinit(struct tdstEngineObject_ *p_stObject, int eObjectInit)
{
    void *hMSWay;

    if (p_stObject == NULL)
    {
        ERM_fn_vClearLastError(0);
        ERM_fn_vUpdateLastError(0x30, g_ucGameModuleId, 0, E_uwGameMiniStructObjectNull_W, -1, 0xff, 0xff, NULL);
    }
    hMSWay = p_stObject->h_MSWay;
    if (hMSWay == NULL)
    {
        ERM_fn_vClearLastError(0);
        ERM_fn_vUpdateLastError(0x30, g_ucGameModuleId, 0, E_uwGameMiniStructNull_W, -1, 0xff, 0xff, NULL);
    }
    MSWay_fn_vDesinitPath(hMSWay);
}

/* 0x00416FF0 */
void __cdecl fn_vMSWayCopyClone(struct tdstEngineObject_ *_p_stObjectDest, struct tdstEngineObject_ *_p_stObjectSrc)
{
    MSWay_fn_vDesinitPath(_p_stObjectDest->h_MSWay);

    memcpy(_p_stObjectDest->h_MSWay, _p_stObjectSrc->h_MSWay, fn_ulMSWaySizeOf());
    MSWay_fn_vInitPath(_p_stObjectDest->h_MSWay);
}

/* 0x00417030 */
void MSWay_fn_vInitPath (MS_tdxHandleToMSWay hWay)
{

  MSWay_fn_vDesinitPath(hWay);
}

/* 0x00417040 */
void MSWay_fn_vDesinitPath(MS_tdxHandleToMSWay hWay)
{
  if ( hWay )
  {
    if ( hWay->m_hPath )
    {
        WPG_fn_vDestroy (hWay->m_hPath) ;
    }
    hWay->m_hPath = NULL ;
    hWay->m_lIndex = -1 ;
    hWay->m_bIsCircular = 0;
  }
}

/* 0x00417070 */
void MSWay_fn_vSetPath(MS_tdxHandleToMSWay hWay, WP_tdHandleOfGraph hPath, char _bIsCircular)
{
  if ( hWay && hPath )
  {
    if ( hWay->m_hPath != NULL )
    {
        WPG_fn_vDestroy (hWay->m_hPath) ;
    }
    hWay->m_hPath = hPath ;

    hWay->m_lIndex = 0 ;
    hWay->m_bIsCircular = _bIsCircular;
  }
}

/* 0x004170B0 */
long MSWay_fn_lGetNumberWayPointInPath (MS_tdxHandleToMSWay hWay)
{
  if ( hWay && hWay->m_hPath )
    return (LST2_M_DynamicGetNumberOfElements(&hWay->m_hPath->m_hListOfNode)) ;
  return (-1) ;
}

/* 0x004170D0 */
long MSWay_fn_lGetCurrentIndexInPath (MS_tdxHandleToMSWay hWay)
{
  if (hWay && hWay->m_hPath)
    return (hWay->m_lIndex) ;
  return (-1) ;
}

/* 0x004170F0 */
long MSWay_fn_lGetLastIndexInPath (MS_tdxHandleToMSWay hWay)
{
  if ( hWay && hWay->m_hPath )
    return (LST2_M_DynamicGetNumberOfElements(&hWay->m_hPath->m_hListOfNode)-1) ;

  return (-1) ;
}

/* 0x00417110 */
long MSWay_fn_lGetFirstIndexInPath (MS_tdxHandleToMSWay hWay)
{
  if ( hWay && hWay->m_hPath )
    return (0) ;
  return (-1) ;
}

/* 0x00417130 */
long MSWay_fn_lSetCurrentIndexInPath (MS_tdxHandleToMSWay hWay, long _lIndex)
{
  long lNbOfElts;
  if ( hWay != NULL && hWay->m_hPath )
  {
    lNbOfElts = LST2_M_DynamicGetNumberOfElements(&hWay->m_hPath->m_hListOfNode);
    if (hWay->m_bIsCircular)
    {
      _lIndex %= lNbOfElts;
      if (_lIndex<0) _lIndex+=lNbOfElts;
    }
    if (_lIndex>=lNbOfElts)
      _lIndex = lNbOfElts-1;
    if (_lIndex<0)
      _lIndex = 0;
    hWay->m_lIndex = _lIndex ;
    return (hWay->m_lIndex) ;
  }
  return (-1) ;
}

/* 0x00417170 */
long MSWay_fn_lLookForWPInMSWay(MS_tdxHandleToMSWay _hWay, void *_hWayPoint)
{
  long lNbOfElts;
  int iIndex;
  WP_tdhGraphNode hElement;

  if ( _hWay && _hWay->m_hPath )
  {
    lNbOfElts = LST2_M_DynamicGetNumberOfElements(&_hWay->m_hPath->m_hListOfNode);

    LST2_M_DynamicForEachElementOf(&_hWay->m_hPath->m_hListOfNode, hElement, iIndex)
    {
      if (_hWayPoint == hElement->m_hWayPoint)
        return ((long)(lNbOfElts - iIndex - 1) );
    }
  }
  return(-1);
}

/* 0x004171B0 */
long MSWay_fn_lIncrementCurrentIndexInPath (MS_tdxHandleToMSWay hWay, int iStep)
{
  long lNbOfElts;

  if (hWay == NULL)
    return (-1);

  if ( hWay->m_hPath )
  {
    lNbOfElts = LST2_M_DynamicGetNumberOfElements(&hWay->m_hPath->m_hListOfNode);
    if (hWay->m_bIsCircular)
    {
      hWay->m_lIndex += iStep;
      hWay->m_lIndex %= lNbOfElts;
      if (hWay->m_lIndex<0) hWay->m_lIndex+=lNbOfElts;
      return (hWay->m_lIndex);
    }
    else
    {
      if ( (hWay->m_lIndex + iStep >= 0) &&
        (hWay->m_lIndex + iStep < lNbOfElts ) )
      {
        hWay->m_lIndex += iStep  ;
        return (hWay->m_lIndex) ;
      }
    }
  }
  return (-1) ;
}

/* 0x00417210 */
void *MSWay_fn_hGetWayPointOfPath (MS_tdxHandleToMSWay hWay, int iIndex)
{
  WP_tdhGraphNode hElement ;
  int i, k ;
  long lNbOfElts;

  if ( hWay && hWay->m_hPath)
  {
    lNbOfElts = LST2_M_DynamicGetNumberOfElements(&hWay->m_hPath->m_hListOfNode);
    if (hWay->m_bIsCircular)
    {
      iIndex %= lNbOfElts;
      if (iIndex<0) iIndex+=lNbOfElts;
    }
    else
    {
			if (iIndex < 0 || iIndex >= lNbOfElts )
        return(NULL);
    }

    k = lNbOfElts - iIndex - 1;
    LST2_M_DynamicGetElementNumber(&hWay->m_hPath->m_hListOfNode, hElement, k, i)
    return (hElement->m_hWayPoint);
  }
  return (NULL) ;
}

/* 0x00417260 */
unsigned long MSWay_fn_ulGetCapsForNextWayPointOfPath (MS_tdxHandleToMSWay hWay, int iIndex)
{
  WP_tdhWayPoint hWayPoint1, hWayPoint2 ;
  unsigned long ulNewCaps ;
  long lNbOfElts;

  if ( hWay && hWay->m_hPath != NULL)
  {
    lNbOfElts = LST2_M_DynamicGetNumberOfElements(&hWay->m_hPath->m_hListOfNode);
    if (hWay->m_bIsCircular)
    {
      iIndex %= lNbOfElts;
      if (iIndex<0) iIndex+=lNbOfElts;
    }
    if ( iIndex >= 0 && iIndex < lNbOfElts )
    {
      hWayPoint1 = MSWay_fn_hGetWayPointOfPath(hWay, iIndex ) ;
      hWayPoint2 = MSWay_fn_hGetWayPointOfPath(hWay, iIndex+1 ) ;
      if ( hWayPoint2 )
      {
        if (WPG_fn_lGetLinkCapabilities (hWay->m_hPath, hWayPoint1, hWayPoint2, &ulNewCaps)== -1 )
          return (0);
        return ( ulNewCaps ) ;
      }
    }
  }
  return (0) ;
}

/* 0x004172E0 */
long MSWay_fn_lChangeCapsForNextWayPointOfPath (MS_tdxHandleToMSWay hWay, int iIndex, unsigned long ulNewCaps, long lValue)
{
  WP_tdhWayPoint hWayPoint1, hWayPoint2 ;
  long lNbOfElts;

  if ( hWay && hWay->m_hPath != NULL)
  {
    lNbOfElts = LST2_M_DynamicGetNumberOfElements(&hWay->m_hPath->m_hListOfNode);
    if (hWay->m_bIsCircular)
    {
      iIndex %= lNbOfElts;
      if (iIndex<0) iIndex+=lNbOfElts;
    }
    if ( iIndex >= 0 && iIndex < lNbOfElts )
    {
      hWayPoint1 = MSWay_fn_hGetWayPointOfPath(hWay, iIndex ) ;
      hWayPoint2 = MSWay_fn_hGetWayPointOfPath(hWay, iIndex+1 ) ;
      if ( hWayPoint2 )
      {
        return (WPG_fn_lChangeLinkCapabilities (hWay->m_hPath, hWayPoint1, hWayPoint2, ulNewCaps, lValue));
      }
    }
  }
  return (-1) ;
}

/* 0x00417360 */
long MSWay_fn_lGetWeightForNextWayPointOfPath (MS_tdxHandleToMSWay hWay, int iIndex)
{
  WP_tdhWayPoint hWayPoint1, hWayPoint2 ;
  long lValue ;
  long lNbOfElts;

  if ( hWay && hWay->m_hPath != NULL)
  {
    lNbOfElts = LST2_M_DynamicGetNumberOfElements(&hWay->m_hPath->m_hListOfNode);
    if (hWay->m_bIsCircular)
    {
      iIndex %= lNbOfElts;
      if (iIndex<0) iIndex+=lNbOfElts;
    }
    if ( iIndex >= 0 && iIndex < lNbOfElts )
    {
      hWayPoint1 = MSWay_fn_hGetWayPointOfPath(hWay, iIndex ) ;
      hWayPoint2 = MSWay_fn_hGetWayPointOfPath(hWay, iIndex+1 ) ;
      if ( hWayPoint2 )
      {
        if (WPG_fn_lGetLinkWeight (hWay->m_hPath, hWayPoint1, hWayPoint2, &lValue)== -1 )
          return (0);
        return ( lValue ) ;
      }
    }
  }
  return (0) ;
}

/* 0x004173E0 */
long MSWay_fn_lChangeWeightForNextWayPointOfPath (MS_tdxHandleToMSWay hWay, int iIndex, long lWeight)
{
  WP_tdhWayPoint hWayPoint1, hWayPoint2 ;
  long lNbOfElts;

  if ( hWay && hWay->m_hPath != NULL)
  {
    lNbOfElts = LST2_M_DynamicGetNumberOfElements(&hWay->m_hPath->m_hListOfNode);
    if (hWay->m_bIsCircular)
    {
      iIndex %= lNbOfElts;
      if (iIndex<0) iIndex+=lNbOfElts;
    }
    if ( iIndex >= 0 && iIndex < lNbOfElts )
    {
      hWayPoint1 = MSWay_fn_hGetWayPointOfPath(hWay, iIndex ) ;
      hWayPoint2 = MSWay_fn_hGetWayPointOfPath(hWay, iIndex+1 ) ;
      if ( hWayPoint2 )
      {
        return (WPG_fn_lChangeLinkWeight (hWay->m_hPath, hWayPoint1, hWayPoint2, lWeight));
      }
    }
  }
  return (-1) ;
}
