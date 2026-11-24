#ifndef __SNDLST2_H__
#define __SNDLST2_H__

#define SNDLST2_C_DynamicInvalid									        SNDLST2_C_DoubleLinkInvalid
#define SNDLST2_M_DynamicAnchorTo(TypeElement)						SNDLST2_M_AnchorTo(Dyn, TypeElement)
#define SNDLST2_M_DynamicUseListOf(type)							    SNDLST2_M_DoubleLinkUseListOf(Dyn, type)
#define SNDLST2_M_DynamicGetMember(handle, member)				SNDLST2_M_DoubleLinkGetMember(Dyn, handle, member)
#define SNDLST2_M_DynamicSetMember(handle, member, value)			SNDLST2_M_DoubleLinkSetMember(Dyn, handle, member, value)
#define SNDLST2_M_DynamicAnchorDeclaration(TypeElement)	SNDLST2_M_DoubleLinkAnchorDeclaration(Dyn, TypeElement)
#define SNDLST2_M_DynamicParentDeclaration(TypeElement)	SNDLST2_M_DoubleLinkParentDeclaration(Dyn, TypeElement)
#define SNDLST2_M_DynamicGetFirstElement(hAnchor)				SNDLST2_M_DoubleLinkGetFirstElement(Dyn, hAnchor)
#define SNDLST2_M_DynamicGetLastElement(hAnchor)					SNDLST2_M_DoubleLinkGetLastElement(Dyn, hAnchor)
#define SNDLST2_M_DynamicGetNumberOfElements(hAnchor)		SNDLST2_M_DoubleLinkGetNumberOfElements(Dyn, hAnchor)
#define SNDLST2_M_DynamicSetFirstElement(hAnchor, value)	SNDLST2_M_DoubleLinkSetFirstElement(Dyn, hAnchor, value)
#define SNDLST2_M_DynamicSetLastElement(hAnchor, value)	SNDLST2_M_DoubleLinkSetLastElement(Dyn, hAnchor, value)
#define SNDLST2_M_DynamicInitAnchor(hAnchor)						  SNDLST2_M_DoubleLinkInitAnchor(Dyn, hAnchor)
#define SNDLST2_M_DynamicElementDeclaration(TypeElement)	SNDLST2_M_DoubleLinkElementDeclaration(Dyn, TypeElement)
#define SNDLST2_M_DynamicChildDeclaration(TypeElement, TypeParent)	SNDLST2_M_DoubleLinkChildDeclaration(Dyn, TypeElement, TypeParent)
#define SNDLST2_M_DynamicGetNextElement(hElement)				SNDLST2_M_DoubleLinkGetNextElement(Dyn, hElement)
#define SNDLST2_M_DynamicGetPrevElement(hElement)				SNDLST2_M_DoubleLinkGetPrevElement(Dyn, hElement)
#define SNDLST2_M_DynamicGetFather(hElement)						  SNDLST2_M_DoubleLinkGetFather(Dyn, hElement)
#define SNDLST2_M_DynamicSetNextElement(hElement, value)	SNDLST2_M_DoubleLinkSetNextElement(Dyn, hElement, value)
#define SNDLST2_M_DynamicSetPrevElement(hElement, value)	SNDLST2_M_DoubleLinkSetPrevElement(Dyn, hElement, value)
#define SNDLST2_M_DynamicSetFather(hElement, value)			SNDLST2_M_DoubleLinkSetFather(Dyn, hElement, value)
#define SNDLST2_M_DynamicInitElement(hElement)						SNDLST2_M_DoubleLinkInitElement(Dyn, hElement)
#define SNDLST2_M_DynamicAddTail(hAnchor, hElement)			SNDLST2_M_DoubleLinkAddTail(Dyn, hAnchor, hElement)
#define SNDLST2_M_DynamicAddHead(hAnchor, hElement)			SNDLST2_M_DoubleLinkAddHead(Dyn, hAnchor, hElement)
#define SNDLST2_M_DynamicIsolate(hElement)							  SNDLST2_M_DoubleLinkIsolate(Dyn, hElement)
#define SNDLST2_M_DynamicInsertBetween(hElement,hPrevElement,hNextElement)	SNDLST2_M_DoubleLinkInsertBetween(Dyn, hElement,hPrevElement,hNextElement)
#define SNDLST2_M_DynamicWhatElementNumber(hElement0,hElement, i)	    SNDLST2_M_DoubleLinkWhatElementNumber(Dyn, hElement0,hElement, i)
#define SNDLST3_M_DynamicWhatElementNumber(hElement0,hAnchor, hElement, i)	    LST3_M_DoubleLinkWhatElementNumber(Dyn, hElement0,hAnchor,hElement, i)
#define SNDLST2_M_DynamicForEachElementOf(hAnchor, hElement, i)		    SNDLST2_M_DoubleLinkForEachElementOf(Dyn, hAnchor, hElement, i)
#define SNDLST2_M_DynamicForEachMovingElementOf(hAnchor, hElement, hNextElement, i)	SNDLST2_M_DoubleLinkForEachMovingElementOf(Dyn, hAnchor, hElement, hNextElement, i)
#define SNDLST2_M_DynamicGetElementNumber(hAnchor, hElement, k, i)     SNDLST2_M_DoubleLinkGetElementNumber(Dyn, hAnchor, hElement, k, i)
#define SNDLST2_M_DynamicGetNthBrother(hFirstElement, hElement, k, i)  SNDLST2_M_DoubleLinkGetNthBrother(Dyn, hFirstElement, hElement, k, i)

#ifdef SNDLST2_StaticIsOptimised

#define SNDLST2_C_StaticInvalid				                  SNDLST2_C_StaticInvalid
#define SNDLST2_M_StaticGetMember(handle, member)		    SNDLST2_M_ArrayGetMember(Opt, handle, member)
#define SNDLST2_M_StaticSetMember(handle, member, value)	SNDLST2_M_ArraySetMember(Opt, handle, member, value)
#define SNDLST2_M_StaticAnchorTo(TypeElement)						SNDLST2_M_ArrayAnchorTo(Opt, TypeElement)
#define SNDLST2_M_StaticUseListOf(TypeElement)				    SNDLST2_M_ArrayUseListOf(Opt, TypeElement)
#define SNDLST2_M_StaticParentDeclaration(TypeElement)		SNDLST2_M_ArrayParentDeclaration(Opt, TypeElement)
#define SNDLST2_M_StaticAnchorDeclaration(TypeElement)		SNDLST2_M_ArrayAnchorDeclaration(Opt, TypeElement)
#define SNDLST2_M_StaticGetFirstElement(hAnchor)		      SNDLST2_M_ArrayGetFirstElement(Opt, hAnchor)
#define SNDLST2_M_StaticSetFirstElement(hAnchor, value)	SNDLST2_M_ArraySetFirstElement(Opt, hAnchor, value)
#define SNDLST2_M_StaticGetNumberOfElements(hAnchor)	    SNDLST2_M_ArrayGetNumberOfElements(Opt, hAnchor)
#define SNDLST2_M_StaticSetNumberOfElements(hAnchor, value)	          SNDLST2_M_ArraySetNumberOfElements(Opt, hAnchor, value)
#define SNDLST2_M_StaticElementDeclaration(TypeElement)  SNDLST2_M_ArrayElementDeclaration(Opt, TypeElement)
#define SNDLST2_M_StaticChildDeclaration(TypeElement,TypeParent)       SNDLST2_M_ArrayChildDeclaration(Opt, TypeElement,TypeParent)
#define SNDLST2_M_StaticGetNextElement(_hElement)		    SNDLST2_M_ArrayGetNextElement(Opt, _hElement)
#define SNDLST2_M_StaticGetNthBrother(hFirstElement, hElement, k, i)		SNDLST2_M_ArrayGetNthBrother(Opt, hFirstElement, hElement, k, i)
#define SNDLST2_M_StaticGetPrevElement(_hElement)		    SNDLST2_M_ArrayGetPrevElement(Opt, _hElement)
#define SNDLST2_M_StaticForEachElementOf(_hAnchor, _hElement, _i)		  SNDLST2_M_ArrayForEachElementOf(Opt, _hAnchor, _hElement, _i)
#define SNDLST2_M_StaticGetElementNumber(_hAnchor, _hElement, _k, _i)	SNDLST2_M_ArrayGetElementNumber(Opt, _hAnchor, _hElement, _k, _i)
#define SNDLST2_M_StaticWhatElementNumber(_hElement0,_hAnchor, _i)	    SNDLST2_M_ArrayWhatElementNumber(Opt, _hElement0,_hAnchor, _i)
#define SNDLST3_M_StaticWhatElementNumber(_hElement0,_hAnchor, _hIterator, _i)	    SNDLST3_M_ArrayWhatElementNumber(Opt, _hElement0,_hAnchor, _hIterator, _i)

#define SNDLST2_M_StaticInitAnchor(hAnchor)
#define SNDLST2_M_StaticInitElement(hElement)

#endif

#ifdef SNDLST2_StaticIsSemiOptimised

#define SNDLST2_C_StaticInvalid				                  SNDLST2_C_StaticInvalid
#define SNDLST2_M_StaticGetMember(handle, member)		    SNDLST2_M_ArrayGetMember(SemiOpt, handle, member)
#define SNDLST2_M_StaticSetMember(handle, member, value)	SNDLST2_M_ArraySetMember(SemiOpt, handle, member, value)
#define SNDLST2_M_StaticAnchorTo(TypeElement)						SNDLST2_M_ArrayAnchorTo(SemiOpt, TypeElement)
#define SNDLST2_M_StaticUseListOf(TypeElement)				    SNDLST2_M_ArrayUseListOf(SemiOpt, TypeElement)
#define SNDLST2_M_StaticParentDeclaration(TypeElement)		SNDLST2_M_DoubleLinkParentDeclaration(SemiOpt, TypeElement)
#define SNDLST2_M_StaticAnchorDeclaration(TypeElement)		SNDLST2_M_DoubleLinkAnchorDeclaration(SemiOpt, TypeElement)
#define SNDLST2_M_StaticGetFirstElement(hAnchor)		      SNDLST2_M_ArrayGetFirstElement(SemiOpt, hAnchor)
#define SNDLST2_M_StaticGetNumberOfElements(hAnchor)	    SNDLST2_M_ArrayGetNumberOfElements(SemiOpt, hAnchor)
#define SNDLST2_M_StaticSetFirstElement(hAnchor, value)	SNDLST2_M_ArraySetFirstElement(SemiOpt, hAnchor, value)
#define SNDLST2_M_StaticSetNumberOfElements(hAnchor, value)	          SNDLST2_M_ArraySetNumberOfElements(SemiOpt, hAnchor, value)
#define SNDLST2_M_StaticElementDeclaration(TypeElement)  SNDLST2_M_DoubleLinkElementDeclaration(SemiOpt, TypeElement)
#define SNDLST2_M_StaticChildDeclaration(TypeElement,TypeParent)       SNDLST2_M_DoubleLinkChildDeclaration(SemiOpt, TypeElement,TypeParent)
#define SNDLST2_M_StaticGetNextElement(_hElement)		    SNDLST2_M_ArrayGetNextElement(SemiOpt, _hElement)
#define SNDLST2_M_StaticGetPrevElement(_hElement)		    SNDLST2_M_ArrayGetPrevElement(SemiOpt, _hElement)
#define SNDLST2_M_StaticGetNthBrother(hFirstElement, hElement, k, i)		SNDLST2_M_ArrayGetNthBrother(SemiOpt, hFirstElement, hElement, k, i)
#define SNDLST2_M_StaticForEachElementOf(_hAnchor, _hElement, _i)		  SNDLST2_M_ArrayForEachElementOf(SemiOpt, _hAnchor, _hElement, _i)
#define SNDLST2_M_StaticGetElementNumber(_hAnchor, _hElement, _k, _i)	SNDLST2_M_ArrayGetElementNumber(SemiOpt, _hAnchor, _hElement, _k, _i)
#define SNDLST2_M_StaticWhatElementNumber(_hElement0,_hAnchor, _i)	    SNDLST2_M_ArrayWhatElementNumber(SemiOpt, _hElement0,_hAnchor, _i)
#define SNDLST3_M_StaticWhatElementNumber(_hElement0,_hAnchor, _hIterator, _i)	    SNDLST3_M_ArrayWhatElementNumber(SemiOpt, _hElement0,_hAnchor, _hIterator, _i)

#define SNDLST2_M_StaticInitElement(hElement)						  SNDLST2_M_DoubleLinkInitElement(SemiOpt, hElement)
#define SNDLST2_M_StaticAddTail(hAnchor, hElement)				  SNDLST2_M_DoubleLinkAddTail(SemiOpt, hAnchor, hElement)
#define SNDLST2_M_StaticAddHead(hAnchor, hElement)				  SNDLST2_M_DoubleLinkAddHead(SemiOpt, hAnchor, hElement)
#define SNDLST2_M_StaticIsolate(hElement)							    SNDLST2_M_DoubleLinkIsolate(SemiOpt, hElement)
#define SNDLST2_M_StaticInsertBetween(hElement,hPrevElement,hNextElement)	SNDLST2_M_DoubleLinkInsertBetween(SemiOpt, hElement,hPrevElement,hNextElement)

#define SNDLST2_M_StaticConvert(_hAnchor, _dTab, _ulSize, _hDest, _hSrc, _i)   \
{                                                                           \
  _hDest=_dTab;                                                             \
  SNDLST2_M_DoubleLinkForEachElementOf(SemiOpt, _hAnchor, _hSrc, _i)           \
  {                                                                         \
    memcpy(_hDest, _hSrc, _ulSize);                                         \
    _hDest++;                                                               \
  }                                                                         \
  SNDLST2_M_StaticSetFirstElement(_hAnchor, _dTab);                            \
}

#endif

#if !defined(SNDLST2_StaticIsOptimised)&&!defined(SNDLST2_StaticIsSemiOptimised)

#define SNDLST2_C_StaticInvalid									        SNDLST2_C_DoubleLinkInvalid
#define SNDLST2_M_StaticGetMember(handle, member)				SNDLST2_M_DoubleLinkGetMember(Sta, handle, member)
#define SNDLST2_M_StaticSetMember(handle, member, value)	SNDLST2_M_DoubleLinkSetMember(Sta, handle, member, value)
#define SNDLST2_M_StaticAnchorTo(TypeElement)						SNDLST2_M_AnchorTo(Sta, TypeElement)
#define SNDLST2_M_StaticUseListOf(type)							    SNDLST2_M_DoubleLinkUseListOf(Sta, type)
#define SNDLST2_M_StaticParentDeclaration(TypeElement)		SNDLST2_M_DoubleLinkParentDeclaration(Sta, TypeElement)
#define SNDLST2_M_StaticAnchorDeclaration(TypeElement)		SNDLST2_M_DoubleLinkAnchorDeclaration(Sta, TypeElement)
#define SNDLST2_M_StaticGetFirstElement(hAnchor)					SNDLST2_M_DoubleLinkGetFirstElement(Sta, hAnchor)
#define SNDLST2_M_StaticGetLastElement(hAnchor)					SNDLST2_M_DoubleLinkGetLastElement(Sta, hAnchor)
#define SNDLST2_M_StaticGetNumberOfElements(hAnchor)			SNDLST2_M_DoubleLinkGetNumberOfElements(Sta, hAnchor)
#define SNDLST2_M_StaticSetFirstElement(hAnchor, value)	SNDLST2_M_DoubleLinkSetFirstElement(Sta, hAnchor, value)
#define SNDLST2_M_StaticSetLastElement(hAnchor, value)		SNDLST2_M_DoubleLinkSetLastElement(Sta, hAnchor, value)
#define SNDLST2_M_StaticInitAnchor(hAnchor)						  SNDLST2_M_DoubleLinkInitAnchor(Sta, hAnchor)
#define SNDLST2_M_StaticElementDeclaration(TypeElement)	SNDLST2_M_DoubleLinkElementDeclaration(Sta, TypeElement)
#define SNDLST2_M_StaticChildDeclaration(TypeElement, TypeParent)	        SNDLST2_M_DoubleLinkChildDeclaration(Sta, TypeElement, TypeParent)
#define SNDLST2_M_StaticGetNextElement(hElement)					SNDLST2_M_DoubleLinkGetNextElement(Sta, hElement)
#define SNDLST2_M_StaticGetPrevElement(hElement)					SNDLST2_M_DoubleLinkGetPrevElement(Sta, hElement)
#define SNDLST2_M_StaticGetFather(hElement)						  SNDLST2_M_DoubleLinkGetFather(Sta, hElement)
#define SNDLST2_M_StaticSetNextElement(hElement, value)	SNDLST2_M_DoubleLinkSetNextElement(Sta, hElement, value)
#define SNDLST2_M_StaticSetPrevElement(hElement, value)	SNDLST2_M_DoubleLinkSetPrevElement(Sta, hElement, value)
#define SNDLST2_M_StaticSetFather(hElement, value)				SNDLST2_M_DoubleLinkSetFather(Sta, hElement, value)
#define SNDLST2_M_StaticInitElement(hElement)						SNDLST2_M_DoubleLinkInitElement(Sta, hElement)
#define SNDLST2_M_StaticAddTail(hAnchor, hElement)				SNDLST2_M_DoubleLinkAddTail(Sta, hAnchor, hElement)
#define SNDLST2_M_StaticAddHead(hAnchor, hElement)				SNDLST2_M_DoubleLinkAddHead(Sta, hAnchor, hElement)
#define SNDLST2_M_StaticIsolate(hElement)							  SNDLST2_M_DoubleLinkIsolate(Sta, hElement)
#define SNDLST2_M_StaticInsertBetween(hElement,hPrevElement,hNextElement)	SNDLST2_M_DoubleLinkInsertBetween(Sta, hElement,hPrevElement,hNextElement)
#define SNDLST2_M_StaticWhatElementNumber(hElement0,hElement, i)			      SNDLST2_M_DoubleLinkWhatElementNumber(Sta, hElement0,hElement, i)
#define SNDLST3_M_StaticWhatElementNumber(hElement0, hAnchor, hElement, i)			      SNDLST3_M_DoubleLinkWhatElementNumber(Sta, hElement0, hAnchor, hElement, i)
#define SNDLST2_M_StaticForEachElementOf(hAnchor, hElement, i)		          SNDLST2_M_DoubleLinkForEachElementOf(Sta, hAnchor, hElement, i)

#define SNDLST2_M_StaticForEachMovingElementOf(hAnchor, hElement, hNextElement, i)	SNDLST2_M_DoubleLinkForEachMovingElementOf(Sta, hAnchor, hElement, hNextElement, i)
#define SNDLST2_M_StaticGetElementNumber(hAnchor, hElement, k, i)	        SNDLST2_M_DoubleLinkGetElementNumber(Sta, hAnchor, hElement, k, i)
#define SNDLST2_M_StaticGetNthBrother(hFirstElement, hElement, k, i)       SNDLST2_M_DoubleLinkGetNthBrother(Sta, hFirstElement, hElement, k, i)

#endif

#define SNDLST2_C_ArrayInvalid				-1

#define SNDLST2_M_ArrayGetMember(n, handle, member)		\
((handle)->member##n)

#define SNDLST2_M_ArraySetMember(n, handle, member, value)	\
{(handle)->member##n=(value);}

#define SNDLST2_M_ArrayAnchorTo(n, TypeElement)										\
SNDLST2_M_AnchorTo(n, TypeElement)

#define SNDLST2_M_ArrayUseListOf(n, TypeElement)				\
SNDLST2_M_ArrayAnchorTo(n, TypeElement)										\
{																\
TypeElement hFirstElement##n;									\
long lNumberOfElements##n;										\
}

#define SNDLST2_M_ArrayParentDeclaration(n, TypeElement)		\
TypeElement hFirstElement##n;								\
long lNumberOfElements##n;

#define SNDLST2_M_ArrayAnchorDeclaration(n, TypeElement)		\
SNDLST2_M_ArrayAnchorTo(n, TypeElement)

#define SNDLST2_M_ArrayGetFirstElement(n, hAnchor)		\
SNDLST2_M_ArrayGetMember(n, (hAnchor), hFirstElement)

#define SNDLST2_M_ArraySetFirstElement(n, hAnchor, value)		\
SNDLST2_M_ArraySetMember(n, (hAnchor), hFirstElement, (value))

#define SNDLST2_M_ArrayGetNumberOfElements(n, hAnchor)	\
SNDLST2_M_ArrayGetMember(n, (hAnchor), lNumberOfElements)

#define SNDLST2_M_ArraySetNumberOfElements(n, hAnchor, value)	\
SNDLST2_M_ArraySetMember(n, (hAnchor), lNumberOfElements, (value))

#define SNDLST2_M_ArrayElementDeclaration(n, TypeElement)
#define SNDLST2_M_ArrayChildDeclaration(n, TypeElement,TypeParent)

#define SNDLST2_M_ArrayGetNextElement(n, _hElement)		\
((_hElement)+1)

#define SNDLST2_M_ArrayGetNthBrother(n, hFirstElement, hElement, k, i)		\
{(hElement)=(hFirstElement)+(k); i=i;}

#define SNDLST2_M_ArrayGetPrevElement(n, _hElement)		\
((_hElement)-1)

#define SNDLST2_M_ArrayForEachElementOf(n, _hAnchor, _hElement, _i)		\
	for((_hElement)=SNDLST2_M_ArrayGetFirstElement(n, (_hAnchor)),(_i)=0;	\
	(_i)<SNDLST2_M_ArrayGetNumberOfElements(n, (_hAnchor));			\
	(_i)++,(_hElement)=SNDLST2_M_ArrayGetNextElement(n, _hElement))

#define SNDLST2_M_ArrayGetElementNumber(n, _hAnchor, _hElement, _k, _i)		\
  {(_hElement)=SNDLST2_M_ArrayGetFirstElement(n, (_hAnchor))+(_k);(_i)=0;}

#define SNDLST2_M_ArrayWhatElementNumber(n, _hElement0,_hAnchor, _i)	\
{_i=(_hElement0) - SNDLST2_M_ArrayGetFirstElement(n, (_hAnchor));}

#define SNDLST3_M_ArrayWhatElementNumber(n, _hElement0,_hAnchor, _hIterator, _i)	\
{	_i=(_hElement0) - SNDLST2_M_ArrayGetFirstElement(n, (_hAnchor));_hIterator=0;}

#define SNDLST2_C_DoubleLinkInvalid 0

#define SNDLST2_M_DoubleLinkGetMember(n, handle, member)			\
((handle)->member##n)

#define SNDLST2_M_DoubleLinkSetMember(n, handle, member, value)	\
{(handle)->member##n=(value);}

#define SNDLST2_M_AnchorTo(n, TypeElement) struct LST2_tdstAnchorTo##TypeElement##n

#define SNDLST2_M_DoubleLinkUseListOf(n, TypeElement)							\
SNDLST2_M_AnchorTo(n, TypeElement)										\
{																\
	TypeElement hFirstElement##n;								\
	TypeElement hLastElement##n;								\
	long lNumberOfElements##n;									\
}

#define SNDLST2_M_DoubleLinkParentDeclaration(n, TypeElement)							\
	TypeElement hFirstElement##n;								\
	TypeElement hLastElement##n;								\
	long lNumberOfElements##n;

#define SNDLST2_M_DoubleLinkAnchorDeclaration(n, TypeElement)		\
SNDLST2_M_AnchorTo(n, TypeElement)

#define SNDLST2_M_DoubleLinkGetFirstElement(n, hAnchor)				\
SNDLST2_M_DoubleLinkGetMember(n, hAnchor, hFirstElement)

#define SNDLST2_M_DoubleLinkGetLastElement(n, hAnchor)				\
SNDLST2_M_DoubleLinkGetMember(n, hAnchor, hLastElement)

#define SNDLST2_M_DoubleLinkSetFirstElement(n, hAnchor, value)		\
SNDLST2_M_DoubleLinkGetMember(n, hAnchor, hFirstElement) = (value)

#define SNDLST2_M_DoubleLinkGetNumberOfElements(n, hAnchor)		\
SNDLST2_M_DoubleLinkGetMember(n, hAnchor, lNumberOfElements)

#define SNDLST2_M_DoubleLinkSetLastElement(n, hAnchor, value)		\
SNDLST2_M_DoubleLinkGetMember(n, hAnchor, hLastElement) = (value)

#define SNDLST2_M_DoubleLinkInitAnchor(n, hAnchor)				\
{													\
	SNDLST2_M_DoubleLinkGetFirstElement(n, (hAnchor))= SNDLST2_C_DoubleLinkInvalid;		\
	SNDLST2_M_DoubleLinkGetLastElement(n, (hAnchor)) = SNDLST2_C_DoubleLinkInvalid;		\
	SNDLST2_M_DoubleLinkGetNumberOfElements(n, (hAnchor))= 0;								\
}

#define SNDLST2_M_DoubleLinkElementDeclaration(n, TypeElement)	\
TypeElement hNextBrother##n;								\
TypeElement hPrevBrother##n;								\
SNDLST2_M_AnchorTo(n, TypeElement)* hFather##n;

#define SNDLST2_M_DoubleLinkChildDeclaration(n, TypeElement, TypeFather)	\
TypeElement hNextBrother##n;								\
TypeElement hPrevBrother##n;								\
TypeFather hFather##n;

#define SNDLST2_M_DoubleLinkGetNextElement(n, hElement)	\
SNDLST2_M_DoubleLinkGetMember(n, (hElement), hNextBrother)

#define SNDLST2_M_DoubleLinkGetPrevElement(n, hElement)	\
SNDLST2_M_DoubleLinkGetMember(n, (hElement), hPrevBrother)

#define SNDLST2_M_DoubleLinkGetFather(n, hSO) \
SNDLST2_M_DoubleLinkGetMember(n, (hSO), hFather)

#define SNDLST2_M_DoubleLinkSetNextElement(n, hElement, value)	\
SNDLST2_M_DoubleLinkGetMember(n, (hElement), hNextBrother) = (value)

#define SNDLST2_M_DoubleLinkSetPrevElement(n, hElement, value)	\
SNDLST2_M_DoubleLinkGetMember(n, (hElement), hPrevBrother) = (value)

#define SNDLST2_M_DoubleLinkSetFather(n, hSO, value) \
SNDLST2_M_DoubleLinkGetMember(n, (hSO), hFather) = (value)

#define SNDLST2_M_DoubleLinkInitElement(n, hElement)				\
{												\
	SNDLST2_M_DoubleLinkGetNextElement(n, (hElement))= SNDLST2_C_DoubleLinkInvalid;	\
	SNDLST2_M_DoubleLinkGetPrevElement(n, (hElement))= SNDLST2_C_DoubleLinkInvalid;	\
	SNDLST2_M_DoubleLinkGetFather(n, (hElement))   = SNDLST2_C_DoubleLinkInvalid;	\
}

#define SNDLST2_M_DoubleLinkIsolate(n, hElement)											\
{																			\
	if(SNDLST2_M_DoubleLinkGetNextElement(n, (hElement))!=SNDLST2_C_DoubleLinkInvalid)							\
		SNDLST2_M_DoubleLinkGetPrevElement(n, SNDLST2_M_DoubleLinkGetNextElement(n, (hElement)))  = SNDLST2_M_DoubleLinkGetPrevElement(n, (hElement));	\
	if(SNDLST2_M_DoubleLinkGetPrevElement(n, (hElement))!=SNDLST2_C_DoubleLinkInvalid)							\
		SNDLST2_M_DoubleLinkGetNextElement(n, SNDLST2_M_DoubleLinkGetPrevElement(n, (hElement))) = SNDLST2_M_DoubleLinkGetNextElement(n, (hElement));	\
	if(SNDLST2_M_DoubleLinkGetFather(n, (hElement))!=SNDLST2_C_DoubleLinkInvalid)								\
	{																							\
		SNDLST2_M_DoubleLinkGetNumberOfElements(n, SNDLST2_M_DoubleLinkGetFather(n, (hElement)))--;		\
		if(SNDLST2_M_DoubleLinkGetFirstElement(n, SNDLST2_M_DoubleLinkGetFather(n, (hElement)))==(hElement))	\
			SNDLST2_M_DoubleLinkGetFirstElement(n, SNDLST2_M_DoubleLinkGetFather(n, (hElement))) = SNDLST2_M_DoubleLinkGetNextElement(n, (hElement));	\
		if(SNDLST2_M_DoubleLinkGetLastElement(n, SNDLST2_M_DoubleLinkGetFather(n, (hElement)))==(hElement))	\
			SNDLST2_M_DoubleLinkGetLastElement(n, SNDLST2_M_DoubleLinkGetFather(n, (hElement))) = SNDLST2_M_DoubleLinkGetPrevElement(n, (hElement));	\
	}																		\
	SNDLST2_M_DoubleLinkInitElement(n, (hElement));											\
}

#define SNDLST2_M_DoubleLinkAddTail(n, hAnchor, hElement)							\
{																			\
	SNDLST2_M_DoubleLinkIsolate(n, (hElement));										\
	SNDLST2_M_DoubleLinkGetNextElement(n, (hElement)) = SNDLST2_C_DoubleLinkInvalid;			\
	SNDLST2_M_DoubleLinkGetPrevElement(n, (hElement)) = SNDLST2_M_DoubleLinkGetLastElement(n, (hAnchor));		\
	SNDLST2_M_DoubleLinkGetFather(n, (hElement)) = (hAnchor);						\
	if (SNDLST2_M_DoubleLinkGetFirstElement(n, (hAnchor))==SNDLST2_C_DoubleLinkInvalid)		\
		SNDLST2_M_DoubleLinkGetFirstElement(n, (hAnchor)) = (hElement);				\
	else																	\
		SNDLST2_M_DoubleLinkGetNextElement(n, SNDLST2_M_DoubleLinkGetLastElement(n, (hAnchor))) = (hElement);	\
	SNDLST2_M_DoubleLinkGetLastElement(n, (hAnchor)) = (hElement);						\
	SNDLST2_M_DoubleLinkGetNumberOfElements(n, (hAnchor))++;							\
}

#define SNDLST2_M_DoubleLinkAddHead(n, hAnchor,hElement)									\
{																			\
	SNDLST2_M_DoubleLinkIsolate(n, (hElement));										\
	SNDLST2_M_DoubleLinkGetPrevElement(n, (hElement)) = SNDLST2_C_DoubleLinkInvalid;								\
	SNDLST2_M_DoubleLinkGetNextElement(n, (hElement)) = SNDLST2_M_DoubleLinkGetFirstElement(n, (hAnchor));	\
	SNDLST2_M_DoubleLinkGetFather(n, (hElement)) = (hAnchor);								\
	if (SNDLST2_M_DoubleLinkGetLastElement(n, (hAnchor))==SNDLST2_C_DoubleLinkInvalid)							\
		SNDLST2_M_DoubleLinkGetLastElement(n, (hAnchor)) = (hElement);						\
	else																	\
		SNDLST2_M_DoubleLinkGetPrevElement(n, SNDLST2_M_DoubleLinkGetFirstElement(n, (hAnchor))) = (hElement);	\
	SNDLST2_M_DoubleLinkGetFirstElement(n, (hAnchor)) = (hElement);							\
	SNDLST2_M_DoubleLinkGetNumberOfElements(n, (hAnchor))++;							\
}

#define SNDLST2_M_DoubleLinkInsertBetween(n, hElement,hPrevElement,hNextElement)							\
{																										\
	SNDLST2_M_DoubleLinkIsolate(n, (hElement));																\
	SNDLST2_M_DoubleLinkGetNextElement(n, (hElement)) = (hNextElement);										\
	SNDLST2_M_DoubleLinkGetPrevElement(n, (hElement)) = (hPrevElement);										\
	if((hPrevElement)!=SNDLST2_C_DoubleLinkInvalid)															\
	{																									\
		SNDLST2_M_DoubleLinkGetNextElement(n, (hPrevElement)) = (hElement);									\
		SNDLST2_M_DoubleLinkGetFather(n, (hElement)) = SNDLST2_M_DoubleLinkGetFather(n, (hPrevElement));		\
	}																									\
	else																								\
	{																									\
		SNDLST2_M_DoubleLinkGetFirstElement(n, SNDLST2_M_DoubleLinkGetFather(n, (hNextElement))) = (hElement);	\
	}																									\
	if((hNextElement)!=SNDLST2_C_DoubleLinkInvalid)															\
	{																									\
		SNDLST2_M_DoubleLinkGetPrevElement(n, (hNextElement)) = (hElement);									\
		SNDLST2_M_DoubleLinkGetFather(n, (hElement)) = SNDLST2_M_DoubleLinkGetFather(n, (hNextElement));		\
	}																									\
	else																								\
	{																									\
		SNDLST2_M_DoubleLinkGetLastElement(n, SNDLST2_M_DoubleLinkGetFather(n, (hPrevElement))) = (hElement);	\
	}																									\
	SNDLST2_M_DoubleLinkGetNumberOfElements(n, SNDLST2_M_DoubleLinkGetFather(n, (hElement)))++;					\
}

#define SNDLST2_M_DoubleLinkWhatElementNumber(n, hElement0,hElement, i)	\
{for													\
(													\
	i=-1,(hElement)=(hElement0);						\
	(hElement)!=SNDLST2_C_DoubleLinkInvalid;						\
	i++,(hElement)=SNDLST2_M_DoubleLinkGetPrevElement(n,(hElement))				\
);}

#define SNDLST3_M_DoubleLinkWhatElementNumber(n, hElement0,hAnchor, hElement, i)	\
{for													\
(													\
	hAnchor=hAnchor,i=-1,(hElement)=(hElement0);						\
	(hElement)!=SNDLST2_C_DoubleLinkInvalid;						\
	i++,(hElement)=SNDLST2_M_DoubleLinkGetPrevElement(n,(hElement))				\
);}

#define SNDLST2_M_DoubleLinkForEachElementOf(n, hAnchor, hElement, i)		\
for														\
(														\
	(i)=0,(hElement)=SNDLST2_M_DoubleLinkGetFirstElement(n, (hAnchor));	\
	(hElement)!=SNDLST2_C_DoubleLinkInvalid;							\
	(i)++,hElement=SNDLST2_M_DoubleLinkGetNextElement(n, (hElement))		\
)

#define SNDLST2_M_DoubleLinkForEachMovingElementOf(n, hAnchor, hElement, hNextElement, i)	\
for																		\
(																		\
	(i)=0,(hElement)=SNDLST2_M_DoubleLinkGetFirstElement(n, (hAnchor)),(hNextElement)=(hElement)?SNDLST2_M_DoubleLinkGetNextElement(n,(hElement)):SNDLST2_C_DoubleLinkInvalid;	\
	(hElement)!=SNDLST2_C_DoubleLinkInvalid;													\
	(i)++,(hElement)=(hNextElement),(hNextElement)=(hElement)?SNDLST2_M_DoubleLinkGetNextElement(n, (hElement)):SNDLST2_C_DoubleLinkInvalid			\
)

#define SNDLST2_M_DoubleLinkGetElementNumber(n, hAnchor, hElement, k, i)		\
{														\
	for													\
	(													\
		(i)=0,(hElement)=SNDLST2_M_DoubleLinkGetFirstElement(n, (hAnchor));	\
		(i)<(k);											\
		(i)++,(hElement)=SNDLST2_M_DoubleLinkGetNextElement(n, (hElement))		\
	);														\
}

#define SNDLST2_M_DoubleLinkGetNthBrother(n, hFirstElement, hElement, k, i)		\
{														\
	for													\
	(													\
		(i)=0,(hElement)=hFirstElement;	\
		(i)<(k);											\
		(i)++,(hElement)=SNDLST2_M_DoubleLinkGetNextElement(n, (hElement))		\
	);														\
}

#endif
