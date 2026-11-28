












#include "AI/AI_types.h"
#include "AI/AI_Proto.h"
#include "AI/StrIntel.h"
#include "AI/WParc.h"
#include "AI/WPgraphe.h"
#include "TMP/TmpMem.h"

#include <string.h>

/* 0x0047F590 */









WP_tdHandleOfGraph WPG_fn_hCreate (char* _szName, char* _szReference)
{
	WP_tdHandleOfGraph hNewGraph ;




    hNewGraph = (WP_tdHandleOfGraph)AI_fn_p_vTrueAlloc(sizeof(WP_tdstGraph));
#line 185 ".\\AIBase\\WPgraphe.c"
    AI_fn_vMemoryLogFile(hNewGraph, 0, __FILE__, __LINE__);
    fn_vErrorOnAIAlloc();

	if (hNewGraph)
	{
		LST2_M_DynamicInitAnchor(&hNewGraph->m_hListOfNode) ;
	}

	return (hNewGraph) ;
}


#define AI_C_ActionFree 2







void __cdecl WPG_fn_vDestroy(WP_tdHandleOfGraph hGraph)
{
    WP_tdhGraphNode hElement, hNextElement;
    int i;

    LST2_M_DynamicForEachMovingElementOf(&hGraph->m_hListOfNode, hElement, hNextElement, i)
    {
        LST2_M_DynamicIsolate(hElement);
        WPG_fn_vDestroyNode(hElement);
    }


#line 272 ".\\AIBase\\WPgraphe.c"
    AI_fn_vMemoryLogFile(hGraph, AI_C_ActionFree, __FILE__, __LINE__);
    AI_fn_vTrueFree(hGraph);
    fn_vErrorOnAIFree();
    hGraph = NULL;
}






long __cdecl WPG_fn_lAddWayPoint(WP_tdHandleOfGraph _hGraph, WP_tdhWayPoint _hWayPoint, long _lTypeOfWP)
{
    WP_tdhGraphNode hNewNode;

    if (_hGraph && _hWayPoint) {
        hNewNode = WPG_fn_hCreateNode(_hWayPoint, _lTypeOfWP);
        LST2_M_DynamicAddTail(&_hGraph->m_hListOfNode, hNewNode);
        return (0);
    }
    return (-1);
}






long __cdecl WPG_fn_lAddArcToWayPoint(WP_tdHandleOfGraph hGraph, WP_tdhWayPoint hSrcWayPoint,
                                      WP_tdhWayPoint hDstWayPoint, long lWeight,
                                      unsigned long ulCapability)
{
    int i;
    WP_tdhGraphNode hSrcNode, hDstNode;

    if (hGraph && hSrcWayPoint && hDstWayPoint) {

        LST2_M_DynamicForEachElementOf(&hGraph->m_hListOfNode, hDstNode, i) {
            if (hDstNode->m_hWayPoint == hDstWayPoint) {
                break;
            }
        }
        if (hDstNode) {
            LST2_M_DynamicForEachElementOf(&hGraph->m_hListOfNode, hSrcNode, i) {
                if (hSrcNode->m_hWayPoint == hSrcWayPoint) {
                    WPG_fn_vAddArc(hSrcNode, hDstNode, ulCapability, lWeight);
                    return (0);
                }
            }
        }
    }
    return (-1);
}

long WPG_fn_lChangeArcCapabilities (WP_tdhGraphNode hSrcNode, WP_tdhGraphNode hDstNode, unsigned long ulCapability, long lValue);

/* 0x0047F7A0 */
long WPG_fn_lChangeLinkCapabilities (WP_tdHandleOfGraph hGraph,
																     WP_tdhWayPoint hSrcWayPoint,
																     WP_tdhWayPoint hDstWayPoint,
																     unsigned long ulCapability, long lValue ) {
	int i;
	WP_tdhGraphNode hSrcNode, hDstNode;

	if ( hGraph && hSrcWayPoint && hDstWayPoint ) {



		LST2_M_DynamicForEachElementOf(&hGraph->m_hListOfNode, hDstNode, i ) {
			if ( hDstNode->m_hWayPoint == hDstWayPoint ) {
				break ;
			}
		}
		if ( hDstNode ) {
			LST2_M_DynamicForEachElementOf(&hGraph->m_hListOfNode, hSrcNode, i ) {
				if ( hSrcNode->m_hWayPoint == hSrcWayPoint ) {
					return (WPG_fn_lChangeArcCapabilities ( hSrcNode, hDstNode, ulCapability, lValue) ) ;
				}
			}
		}

	}
	return (-1) ;
}

/* 0x0047F800 */

long WPG_fn_lGetLinkCapabilities (WP_tdHandleOfGraph hGraph,
															WP_tdhWayPoint hSrcWayPoint,
															WP_tdhWayPoint hDstWayPoint,
															unsigned long* p_ulCapability) {
	int i;
	WP_tdhGraphNode hSrcNode, hDstNode;

	if ( hGraph && hSrcWayPoint && hDstWayPoint ) {



		LST2_M_DynamicForEachElementOf(&hGraph->m_hListOfNode, hDstNode, i ) {
			if ( hDstNode->m_hWayPoint == hDstWayPoint ) {
				break ;
			}
		}
		if ( hDstNode ) {
			LST2_M_DynamicForEachElementOf(&hGraph->m_hListOfNode, hSrcNode, i ) {
				if ( hSrcNode->m_hWayPoint == hSrcWayPoint ) {
					return (WPG_fn_lGetArcCapabilities ( hSrcNode, hDstNode, p_ulCapability) ) ;
				}
			}
		}

	}
	return (-1) ;

}

long WPG_fn_lChangeArcWeight (WP_tdhGraphNode hSrcNode, WP_tdhGraphNode hDstNode, long lWeight);

/* 0x0047F860 */
long WPG_fn_lChangeLinkWeight (WP_tdHandleOfGraph hGraph,
																 WP_tdhWayPoint hSrcWayPoint,
																 WP_tdhWayPoint hDstWayPoint,
																 long lWeight) {
	int i;
	WP_tdhGraphNode hSrcNode, hDstNode;

	if ( hGraph && hSrcWayPoint && hDstWayPoint ) {



		LST2_M_DynamicForEachElementOf(&hGraph->m_hListOfNode, hDstNode, i ) {
			if ( hDstNode->m_hWayPoint == hDstWayPoint ) {
				break ;
			}
		}
		if ( hDstNode ) {
			LST2_M_DynamicForEachElementOf(&hGraph->m_hListOfNode, hSrcNode, i ) {
				if ( hSrcNode->m_hWayPoint == hSrcWayPoint ) {
					return (WPG_fn_lChangeArcWeight ( hSrcNode, hDstNode, lWeight) ) ;
				}
			}
		}

	}
	return (-1) ;

}

long WPG_fn_lGetArcWeight (WP_tdhGraphNode hSrcNode, WP_tdhGraphNode hDstNode, long *p_lWeight);

/* 0x0047F8C0 */
long WPG_fn_lGetLinkWeight (WP_tdHandleOfGraph hGraph,
															WP_tdhWayPoint hSrcWayPoint,
															WP_tdhWayPoint hDstWayPoint,
															long *p_lWeight) {
	int i;
	WP_tdhGraphNode hSrcNode, hDstNode;

	if ( hGraph && hSrcWayPoint && hDstWayPoint ) {



		LST2_M_DynamicForEachElementOf(&hGraph->m_hListOfNode, hDstNode, i ) {
			if ( hDstNode->m_hWayPoint == hDstWayPoint ) {
				break ;
			}
		}
		if ( hDstNode ) {
			LST2_M_DynamicForEachElementOf(&hGraph->m_hListOfNode, hSrcNode, i ) {
				if ( hSrcNode->m_hWayPoint == hSrcWayPoint ) {
					return (WPG_fn_lGetArcWeight ( hSrcNode, hDstNode, p_lWeight) ) ;
				}
			}
		}

	}
	return (-1) ;

}

/* 0x0047F920 */
void WPG_fn_vReinitGraph (WP_tdHandleOfGraph hGraph) {
	int i, j;
	WP_tdhGraphNode hNode;
	WP_tdHandleOfArc hElement ;

	if ( hGraph) {
		LST2_M_DynamicForEachElementOf(&hGraph->m_hListOfNode, hNode, i ) {
      hNode->m_lTypeOfWP = hNode->m_lTypeOfWPInit;
  		LST2_M_DynamicForEachElementOf(&hNode->m_hListOfArc->m_hArc, hElement, j) {
        hElement->m_ulCapability = hElement->m_ulCapabilityInit;
        hElement->m_lWeight      = hElement->m_lWeightInit;
      }
    }
  }
}








WP_tdHandleOfGraph __cdecl WPG_fn_lShortestPath(WP_tdHandleOfGraph hGraph,
                                                WP_tdhWayPoint hSrcWayPoint,
                                                WP_tdhWayPoint hDstWayPoint,
                                                unsigned long ulCapability)
{
    WP_tdhGraphNode hCurrentNode;
    WP_tdhGraphNode hNeighbor;
    WP_tdHandleOfArc hArc;
    WP_tdhGraphNode *d_hExplored;
    WP_tdhGraphNode *d_hReached;
    int *d_iWayOfExplored, *d_iWayOfReached;
    int *d_iValuationOfExplored, *d_iValuationOfReached;
    int iCurrentWayPointInExplored, iNbElementInExplored;
    int iCurrentWayPointInReached, iNbElementInReached;
    int iNbElementInTheGraph;
    int iDone, iIndex, iValuation, i;
    WP_tdHandleOfGraph tdhShortestPath;

    long lWeight;


    iNbElementInTheGraph = LST2_M_DynamicGetNumberOfElements(&hGraph->m_hListOfNode);
    d_hExplored = (WP_tdhGraphNode *)TMP_M_p_Malloc(sizeof(WP_tdhGraphNode) * iNbElementInTheGraph);
    d_hReached = (WP_tdhGraphNode *)TMP_M_p_Malloc(sizeof(WP_tdhGraphNode) * iNbElementInTheGraph);
    d_iWayOfExplored = (int *)TMP_M_p_Malloc(sizeof(int) * iNbElementInTheGraph);
    d_iValuationOfExplored = (int *)TMP_M_p_Malloc(sizeof(int) * iNbElementInTheGraph);
    d_iWayOfReached = (int *)TMP_M_p_Malloc(sizeof(int) * iNbElementInTheGraph);
    d_iValuationOfReached = (int *)TMP_M_p_Malloc(sizeof(int) * iNbElementInTheGraph);



    iCurrentWayPointInExplored = 0;
    iCurrentWayPointInReached = 0;
    iNbElementInExplored = 0;
    iNbElementInReached = 1;
    LST2_M_DynamicForEachElementOf(&hGraph->m_hListOfNode, hCurrentNode, i) {
        if (hCurrentNode->m_hWayPoint == hSrcWayPoint) {
            d_hReached[0] = hCurrentNode;
            d_iWayOfReached[0] = -1;
            break;
        }
    }

    if (d_hReached[0] == NULL) {
        tdhShortestPath = NULL;



    } else {
        iDone = 0;


        while (iNbElementInReached
               && !iDone) {

            iValuation = 0x7FFFFFFF;
            for (iIndex = 0; iIndex < iNbElementInReached; iIndex++) {
                if (iValuation > d_iValuationOfReached[iIndex]) {
                    iValuation = d_iValuationOfReached[iIndex];
                    iCurrentWayPointInReached = iIndex;
                }
            }


            d_hExplored[iNbElementInExplored] = d_hReached[iCurrentWayPointInReached];
            d_iValuationOfExplored[iNbElementInExplored] = d_iValuationOfReached[iCurrentWayPointInReached];
            d_iWayOfExplored[iNbElementInExplored] = d_iWayOfReached[iCurrentWayPointInReached];
            iCurrentWayPointInExplored = iNbElementInExplored;
            iNbElementInExplored++;

            memmove(d_iValuationOfReached + iCurrentWayPointInReached,
                    d_iValuationOfReached + iCurrentWayPointInReached + 1,
                    sizeof(int) * (iNbElementInReached - iCurrentWayPointInReached));
            memmove(d_iWayOfReached + iCurrentWayPointInReached,
                    d_iWayOfReached + iCurrentWayPointInReached + 1,
                    sizeof(int) * (iNbElementInReached - iCurrentWayPointInReached));
            memmove(d_hReached + iCurrentWayPointInReached,
                    d_hReached + iCurrentWayPointInReached + 1,
                    sizeof(WP_tdhWayPoint *) * (iNbElementInReached - iCurrentWayPointInReached));
            iNbElementInReached--;


            hCurrentNode = d_hExplored[iCurrentWayPointInExplored];
            if (hCurrentNode->m_hWayPoint == hDstWayPoint) {
                iDone = 1;
            }


            LST2_M_DynamicForEachElementOf(&hCurrentNode->m_hListOfArc->m_hArc, hArc, iIndex) {
                hNeighbor = hArc->m_hNode;


                if (!(hArc->m_ulCapability & ulCapability)) {
                    continue;
                }


                for (i = 0; i < iNbElementInExplored; i++) {
                    if (d_hExplored[i] == hNeighbor) {
                        break;
                    }
                }
                if (i == iNbElementInExplored) {

                    for (i = 0; i < iNbElementInReached; i++) {
                        if (d_hReached[i] == hNeighbor) {
                            break;
                        }
                    }
                    if (i == iNbElementInReached) {

                        d_hReached[iNbElementInReached] = hNeighbor;
                        d_iValuationOfReached[iNbElementInReached] =
                            d_iValuationOfExplored[iCurrentWayPointInExplored] + hArc->m_lWeight;
                        d_iWayOfReached[iNbElementInReached] = iCurrentWayPointInExplored;
                        iNbElementInReached++;
                    } else {

                        if (d_iValuationOfReached[i] >
                            (d_iValuationOfExplored[iCurrentWayPointInExplored] + hArc->m_lWeight)) {
                            d_iValuationOfReached[i] =
                                d_iValuationOfExplored[iCurrentWayPointInExplored] + hArc->m_lWeight;
                            d_iWayOfReached[i] = iCurrentWayPointInExplored;
                        }
                    }
                }
            }
        }



        tdhShortestPath = NULL;
        if (iDone) {
            tdhShortestPath = WPG_fn_hCreate("Generated by ShortestPath", "@");
            i = iCurrentWayPointInExplored;


            while (i != -1) {
                WPG_fn_lAddWayPoint(tdhShortestPath,
                                    d_hExplored[i]->m_hWayPoint, d_hExplored[i]->m_lTypeOfWP);
                i = d_iWayOfExplored[i];
            }


            i = iCurrentWayPointInExplored;
            while (i != 0) {
                WPG_fn_lGetLinkCapabilities ( hGraph, d_hExplored[d_iWayOfExplored[i]]->m_hWayPoint, d_hExplored[i]->m_hWayPoint, &ulCapability ) ;
                WPG_fn_lGetLinkWeight ( hGraph, d_hExplored[d_iWayOfExplored[i]]->m_hWayPoint, d_hExplored[i]->m_hWayPoint, &lWeight ) ;
                WPG_fn_lAddArcToWayPoint(tdhShortestPath,
                                         d_hExplored[d_iWayOfExplored[i]]->m_hWayPoint,
                                         d_hExplored[i]->m_hWayPoint,
                                         lWeight, ulCapability);
                i = d_iWayOfExplored[i];
            }
        }
    }

    TMP_M_Free(d_hExplored);
    TMP_M_Free(d_hReached);
    TMP_M_Free(d_iWayOfExplored);
    TMP_M_Free(d_iValuationOfExplored);
    TMP_M_Free(d_iWayOfReached);
    TMP_M_Free(d_iValuationOfReached);

    return (tdhShortestPath);
}





WP_tdHandleOfGraph __cdecl WPG_fn_lBuildOrderedPath(WP_tdHandleOfGraph hGraph, WP_tdhWayPoint hStartWP)
{
    WP_tdHandleOfGraph tdhNewPath;
    WP_tdhGraphNode hCurrentNode;

    WP_tdhGraphNode *d_hListOfWPNode;
    WP_tdhGraphNode hFirstNode;
    int iNbElementInTheGraph;
    int iCount, iStartingIndex, iCurrentIndex;
    unsigned long ulCapability;
    long lWeight;
    long lBestWeight;
    unsigned long ulCapsOfBestNode = 0;
    int iIndexOfBestNode;

    WP_tdhWayPoint *d_hListOfDestWP;
    long *d_hListOfTypeOfWP;
    unsigned long *d_hListOfDestCaps;
    long *d_hListOfDestWeight;
    int iIndex;

    iNbElementInTheGraph = LST2_M_DynamicGetNumberOfElements(&hGraph->m_hListOfNode);
    if (iNbElementInTheGraph == 0) {
        return NULL;
    }


    d_hListOfWPNode = (WP_tdhGraphNode *)TMP_M_p_Malloc(sizeof(WP_tdhGraphNode) * iNbElementInTheGraph);
    d_hListOfDestWP = (WP_tdhWayPoint *)TMP_M_p_Malloc(sizeof(WP_tdhWayPoint) * iNbElementInTheGraph);
    d_hListOfTypeOfWP = (long *)TMP_M_p_Malloc(sizeof(long) * iNbElementInTheGraph);
    d_hListOfDestCaps = (unsigned long *)TMP_M_p_Malloc(sizeof(unsigned long) * iNbElementInTheGraph);
    d_hListOfDestWeight = (long *)TMP_M_p_Malloc(sizeof(long) * iNbElementInTheGraph);

    iStartingIndex = -1;

    LST2_M_DynamicForEachElementOf(&hGraph->m_hListOfNode, hCurrentNode, iCount)
    {
        if (hCurrentNode->m_hWayPoint == hStartWP) {
            d_hListOfWPNode[0] = hCurrentNode;
            iStartingIndex = iCount;
            break;
        }
    }

    tdhNewPath = NULL;
    if (iStartingIndex != -1)
    {

        tdhNewPath = WPG_fn_hCreate("Generated by BuildOrderedPath", "@");
        iCount = iStartingIndex + 1;

        while (iCount != -1) {
            int i;
            LST2_M_DynamicGetElementNumber(&hGraph->m_hListOfNode, hCurrentNode,
                                           (iCount % iNbElementInTheGraph), i);
            d_hListOfWPNode[(iCount - iStartingIndex) % iNbElementInTheGraph] = hCurrentNode;
            if (++iCount % iNbElementInTheGraph == iStartingIndex) {
                iCount = -1;
            }
        }


        hFirstNode = d_hListOfWPNode[0];
        iIndex = 0;
        d_hListOfDestWP[0] = d_hListOfWPNode[0]->m_hWayPoint;
        d_hListOfTypeOfWP[0] = d_hListOfWPNode[0]->m_lTypeOfWP;

        iCurrentIndex = 0;

        do {
            iIndexOfBestNode = -1;
            lBestWeight = (long)(((unsigned long)(1 << 31)) - 1);
            for (iCount = 1; iCount < iNbElementInTheGraph; iCount++) {
                if (iCount != iCurrentIndex) {
                    if ((WPG_fn_lGetArcCapabilities(d_hListOfWPNode[iCurrentIndex], d_hListOfWPNode[iCount], &ulCapability) != -1)
                        && (WPG_fn_lGetArcWeight      (d_hListOfWPNode[iCurrentIndex], d_hListOfWPNode[iCount], &lWeight ) != -1)) {
                        if (lWeight < lBestWeight) {
                            lBestWeight = lWeight;
                            iIndexOfBestNode = iCount;
                            ulCapsOfBestNode = ulCapability;
                        }
                    }
                }
            }
            if (iIndexOfBestNode != -1) {
                iIndex++;
                d_hListOfDestWP[iIndex] = d_hListOfWPNode[iIndexOfBestNode]->m_hWayPoint;
                d_hListOfTypeOfWP[iIndex] = d_hListOfWPNode[iIndexOfBestNode]->m_lTypeOfWP;
                d_hListOfDestCaps[iIndex] = ulCapsOfBestNode;
                d_hListOfDestWeight[iIndex] = lBestWeight;

                d_hListOfWPNode[iCurrentIndex] = NULL;
                iCurrentIndex = iIndexOfBestNode;
            }
        } while (iIndexOfBestNode != -1);

        d_hListOfDestCaps[0] = (unsigned long)0;
        d_hListOfDestWeight[0] = (long)0;


        if ((WPG_fn_lGetArcCapabilities(d_hListOfWPNode[iCurrentIndex], hFirstNode, &ulCapability) != -1)
            && (WPG_fn_lGetArcWeight(d_hListOfWPNode[iCurrentIndex], hFirstNode, &lWeight) != -1)) {

            d_hListOfDestCaps[0] = ulCapability;
            d_hListOfDestWeight[0] = lWeight;
        }


        WPG_fn_lAddWayPoint(tdhNewPath, d_hListOfDestWP[iIndex], d_hListOfTypeOfWP[iIndex]);
        iIndex--;
        while (iIndex >= 0) {

            WPG_fn_lAddWayPoint(tdhNewPath, d_hListOfDestWP[iIndex], d_hListOfTypeOfWP[iIndex]);

            WPG_fn_lAddArcToWayPoint(tdhNewPath,
                                     d_hListOfDestWP[iIndex],
                                     d_hListOfDestWP[iIndex + 1],
                                     d_hListOfDestWeight[iIndex + 1], d_hListOfDestCaps[iIndex + 1]);
            iIndex--;
        }
        WPG_fn_lAddArcToWayPoint(tdhNewPath,
                                 d_hListOfWPNode[iCurrentIndex]->m_hWayPoint,
                                 d_hListOfDestWP[0],
                                 d_hListOfDestWeight[0], d_hListOfDestCaps[0]);

    }


    TMP_M_Free(d_hListOfWPNode);
    TMP_M_Free(d_hListOfDestWP);
    TMP_M_Free(d_hListOfTypeOfWP);
    TMP_M_Free(d_hListOfDestCaps);
    TMP_M_Free(d_hListOfDestWeight);

    return (tdhNewPath);
}

/* 0x00480050 */
WP_tdhWayPoint WPG_fn_hGetWaypointOfGraph (WP_tdHandleOfGraph hGraph, int iIndex, long* _plTypeOfWP)
{
	WP_tdhGraphNode hElement ;
	int i ;

	LST2_M_DynamicGetElementNumber(&hGraph->m_hListOfNode, hElement,
																 iIndex, i) ;
  *_plTypeOfWP = hElement->m_lTypeOfWP;
	return (hElement->m_hWayPoint) ;
}






WP_tdhGraphNode __cdecl WPG_fn_hCreateNode(WP_tdhWayPoint _hWayPoint, long _lTypeOfWP)
{
    WP_tdhGraphNode hNewNode = NULL;

    if (_hWayPoint) {

        hNewNode = (WP_tdhGraphNode)AI_fn_p_vTrueAlloc(sizeof(WP_tdstGraphNode));
#line 1135 ".\\AIBase\\WPgraphe.c"
        AI_fn_vMemoryLogFile(hNewNode, 0, __FILE__, __LINE__);
        fn_vErrorOnAIAlloc();

        if (hNewNode) {
            LST2_M_DynamicInitElement(hNewNode);
            hNewNode->m_hWayPoint = _hWayPoint;
            hNewNode->m_lTypeOfWP = _lTypeOfWP;
            hNewNode->m_lTypeOfWPInit = _lTypeOfWP;
            hNewNode->m_hListOfArc = WPARCLST_fn_hInit();
        }
    }
    return (hNewNode);
}





void __cdecl WPG_fn_vDestroyNode(WP_tdhGraphNode hNode)
{
    if (hNode) {
        WPARCLST_fn_vDestroy(hNode->m_hListOfArc);


#line 1166 ".\\AIBase\\WPgraphe.c"
        AI_fn_vMemoryLogFile(hNode, AI_C_ActionFree, __FILE__, __LINE__);
        AI_fn_vTrueFree(hNode);
        fn_vErrorOnAIFree();
        hNode = NULL;
    }
}





void __cdecl WPG_fn_vAddArc(WP_tdhGraphNode hSrcNode, WP_tdhGraphNode hDstNode,
                            unsigned long ulCapability, long lWeight)
{
    if (hSrcNode && hDstNode) {
        WPARCLST_fn_vAdd(hSrcNode->m_hListOfArc, hDstNode, lWeight, ulCapability);
    }
}

/* 0x00480150 */
WP_tdHandleOfListOfArc WPG_fn_hGetArcList (WP_tdhGraphNode hSrcNode) {
	if ( hSrcNode )
		return (hSrcNode->m_hListOfArc) ;
	return (NULL) ;
}



/* 0x00480160 */

long WPG_fn_lChangeArcCapabilities (WP_tdhGraphNode hSrcNode, WP_tdhGraphNode hDstNode, unsigned long ulCapability, long lValue)
{
	if ( hSrcNode && hDstNode )
		return (WPARCLST_fn_lChangeCapabilities ( hSrcNode->m_hListOfArc, hDstNode, ulCapability, lValue ) ) ;
	return (-1) ;
}

long WPARCLST_fn_lGetCapabilities (WP_tdHandleOfListOfArc hListOfArc, WP_tdhGraphNode hNode, unsigned long* p_ulCapability);

/* 0x00480190 */

long WPG_fn_lGetArcCapabilities (WP_tdhGraphNode hSrcNode, WP_tdhGraphNode hDstNode, unsigned long* p_ulCapability)
{
	if ( hSrcNode && hDstNode )
		return (WPARCLST_fn_lGetCapabilities ( hSrcNode->m_hListOfArc, hDstNode, p_ulCapability ) ) ;
	return (-1) ;
}

long WPARCLST_fn_lChangeWeight (WP_tdHandleOfListOfArc hListOfArc, WP_tdhGraphNode hNode, long lWeight);

/* 0x004801C0 */
long WPG_fn_lChangeArcWeight (WP_tdhGraphNode hSrcNode, WP_tdhGraphNode hDstNode, long lWeight)
{
	if ( hSrcNode && hDstNode )
		return (WPARCLST_fn_lChangeWeight ( hSrcNode->m_hListOfArc, hDstNode, lWeight ) ) ;
	return (-1) ;
}

long WPARCLST_fn_lGetWeight (WP_tdHandleOfListOfArc hListOfArc, WP_tdhGraphNode hNode, long *p_lWeight);

/* 0x004801F0 */
long WPG_fn_lGetArcWeight (WP_tdhGraphNode hSrcNode, WP_tdhGraphNode hDstNode, long *p_lWeight)
{
	if ( hSrcNode && hDstNode )
		return (WPARCLST_fn_lGetWeight ( hSrcNode->m_hListOfArc, hDstNode, p_lWeight ) ) ;
	return (-1) ;
}
