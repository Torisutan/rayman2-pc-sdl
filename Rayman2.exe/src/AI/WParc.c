
























#include "AI/AI_types.h"
#include "AI/AI_Proto.h"
#include "AI/StrIntel.h"
#include "AI/WParc.h"








WP_tdHandleOfListOfArc __cdecl WPARCLST_fn_hInit(void)
{
    WP_tdHandleOfListOfArc hList;


    hList = (WP_tdHandleOfListOfArc)AI_fn_p_vTrueAlloc(sizeof(WP_tdstListOfArc));
#line 61 ".\\AIBase\\WParc.c"
    AI_fn_vMemoryLogFile(hList, 0, __FILE__, __LINE__);
    fn_vErrorOnAIAlloc();

    if (hList) {
        LST2_M_DynamicInitAnchor(&hList->m_hArc);
    }
    return (hList);
}


#define AI_C_ActionFree 2







void __cdecl WPARCLST_fn_vDestroy(WP_tdHandleOfListOfArc hList)
{
    WP_tdHandleOfArc hElement, hNextElement;
    int i;

    if (hList) {
        LST2_M_DynamicForEachMovingElementOf(&hList->m_hArc, hElement, hNextElement, i)
        {
            LST2_M_DynamicIsolate(hElement);
            WPARC_fn_vDestroyArc(hElement);
        }



#line 93 ".\\AIBase\\WParc.c"
        AI_fn_vMemoryLogFile(hList, AI_C_ActionFree, __FILE__, __LINE__);
        AI_fn_vTrueFree(hList);
        fn_vErrorOnAIFree();
        hList = NULL;
    }
}







void __cdecl WPARCLST_fn_vAdd(WP_tdHandleOfListOfArc hListOfArc, WP_tdhGraphNode hNode,
                              long lWeight, unsigned long ulCapability)
{
    WP_tdHandleOfArc hNewArc;
    char bOK = 0;
    int i;
    WP_tdHandleOfArc hElement;

    if (hListOfArc) {
        LST2_M_DynamicForEachElementOf(&hListOfArc->m_hArc, hElement, i)
        {
            if (hElement->m_hNode == hNode) {
                hElement->m_ulCapabilityInit = ulCapability;
                hElement->m_ulCapability = ulCapability;
                hElement->m_lWeightInit = lWeight;
                hElement->m_lWeight = lWeight;
                bOK = 1;
                break;
            }
        }
        if (bOK == 0) {
            hNewArc = WPARC_CreateArc(hNode, lWeight, ulCapability);
            if (hNewArc) {
                LST2_M_DynamicAddTail(&hListOfArc->m_hArc, hNewArc);
            }
        }
    }
}








long __cdecl WPARCLST_fn_lChangeCapabilities(WP_tdHandleOfListOfArc hListOfArc,
                                             WP_tdhGraphNode hNode,
                                             unsigned long ulCapability, long lValue)
{
    WP_tdHandleOfArc hElement;
    int i;

    if (hListOfArc && hNode) {
        LST2_M_DynamicForEachElementOf(&hListOfArc->m_hArc, hElement, i) {
            if (hElement->m_hNode == hNode) {
                switch (lValue) {
                case 0:
                case 2:
                    hElement->m_ulCapability |= ulCapability;
                    break;
                case 1:
                case 3:
                    hElement->m_ulCapability = ulCapability;
                    break;
                case 4:
                    hElement->m_ulCapability &= ~ulCapability;;
                    break;
                case 5:
                    hElement->m_ulCapability = 0;
                    break;
                }
                return (0);
            }
        }
    }
    return (-1);
}

/* 0x00487B10 */

long WPARCLST_fn_lGetCapabilities (WP_tdHandleOfListOfArc hListOfArc,
															     WP_tdhGraphNode hNode, unsigned long* p_ulCapability) {
	WP_tdHandleOfArc hElement ;
	int i ;

	if ( hListOfArc && hNode ) {
		LST2_M_DynamicForEachElementOf(&hListOfArc->m_hArc, hElement, i) {
			if ( hElement->m_hNode == hNode ) {
				*p_ulCapability = hElement->m_ulCapability ;
				return (0) ;
			}
		}
	}
	return (-1) ;
}



/* 0x00487B50 */

long WPARCLST_fn_lChangeWeight (WP_tdHandleOfListOfArc hListOfArc,
                                WP_tdhGraphNode hNode, long lWeight) {
	WP_tdHandleOfArc hElement ;
	int i ;

	if ( hListOfArc && hNode ) {
		LST2_M_DynamicForEachElementOf(&hListOfArc->m_hArc, hElement, i) {
			if ( hElement->m_hNode == hNode ) {
				hElement->m_lWeight = lWeight ;
				return (0) ;
			}
		}
	}
	return (-1) ;
}



/* 0x00487B90 */
long WPARCLST_fn_lGetWeight (WP_tdHandleOfListOfArc hListOfArc,
                             WP_tdhGraphNode hNode, long *p_lWeight) {
	WP_tdHandleOfArc hElement ;
	int i ;

	if ( hListOfArc && hNode ) {
		LST2_M_DynamicForEachElementOf(&hListOfArc->m_hArc, hElement, i) {
			if ( hElement->m_hNode == hNode ) {
				*p_lWeight = hElement->m_lWeight ;
				return (0) ;
			}
		}
	}
	return (-1) ;
}







WP_tdHandleOfArc __cdecl WPARC_CreateArc(WP_tdhGraphNode hNode, long lWeight,
                                         unsigned long ulCapability)
{
    WP_tdHandleOfArc hNewArc;


    hNewArc = (WP_tdHandleOfArc)AI_fn_p_vTrueAlloc(sizeof(WP_tdstWayArc));
#line 284 ".\\AIBase\\WParc.c"
    AI_fn_vMemoryLogFile(hNewArc, 0, __FILE__, __LINE__);
    fn_vErrorOnAIAlloc();

    if (hNewArc) {
        LST2_M_DynamicInitElement(hNewArc);
        hNewArc->m_hNode = hNode;
        hNewArc->m_ulCapabilityInit = ulCapability;
        hNewArc->m_ulCapability = ulCapability;
        hNewArc->m_lWeightInit = lWeight;
        hNewArc->m_lWeight = lWeight;
    }
    return (hNewArc);
}






void __cdecl WPARC_fn_vDestroyArc(WP_tdHandleOfArc hArc)
{
    if (hArc) {


#line 308 ".\\AIBase\\WParc.c"
        AI_fn_vMemoryLogFile(hArc, AI_C_ActionFree, __FILE__, __LINE__);
        AI_fn_vTrueFree(hArc);
        fn_vErrorOnAIFree();
        hArc = NULL;
    }
}
