#include "GAM/ChanList.h"
#include "GAM/PlayAnim.h"
#include "TMP/TmpMem.h"
#include "MOR/MOR.h"

/* 0x00419510 */
void __cdecl fn_vFreeCSOList(CHN_tdxHandleToChannelArray *p_hChannelArray)
{
    CHN_tdxHandleToChannelArray hArray = *p_hChannelArray;

    if (hArray == NULL)
        return;

    TMP_M_Free(hArray);
}

void __cdecl fn_vTakeControlOfChannel(struct tdstEngineObject_ *_hActor, unsigned short _uwChannel, ACP_tdxBool _bControlled)
{

    unsigned long ulNbChannels ;
    MS_tdxHandleTo3dData hCurrent3dData = _hActor->h_3dData;
    CHN_tdxHandleToChannelArray hChannelArray = fn_h3dDataGetChannelSOList(hCurrent3dData);

    if (hChannelArray == NULL)
    {
        return;
    }

    ulNbChannels = fn_ul3dDataGetNumberOfChannels(hCurrent3dData) ;

    if (_uwChannel != 0xffff)
    {

        hChannelArray[_uwChannel].bControlledChannel = _bControlled;
    }
    else
    {

        tdstAChannel *p_stChannel ;
        unsigned long ulI ;
        for ( ulI = 0 , p_stChannel = hChannelArray ; ulI < ulNbChannels ; ulI ++ , p_stChannel ++ )
        {
            p_stChannel->bControlledChannel = _bControlled;
        }
    }
}

/* 0x00419580 */
void __cdecl fn_vUpdateSPOFlagsOfChannelForActivation(HIE_tdstSuperObject *_hSOChannel, ACP_tdxBool _bActivate)
{
    unsigned long ulSpoFlags;
    if (_hSOChannel)
    {
        ulSpoFlags = HIE_fn_SO_ulGetFlags(_hSOChannel);
        if (_bActivate)
        {

            HIE_fn_SO_vSetFlags(_hSOChannel,
                ulSpoFlags & ~(HIE_C_Flag_NotPickable | HIE_C_Flag_ulHidden | HIE_C_Flag_ulNoShadowOnMe));
        }
        else
        {

            HIE_fn_SO_vSetFlags(_hSOChannel,
                ulSpoFlags | HIE_C_Flag_NotPickable | HIE_C_Flag_ulHidden | HIE_C_Flag_ulNoShadowOnMe);
        }
    }
}

void __cdecl fn_vActivateChannel(HIE_tdstSuperObject *_hPerso, unsigned short _uwChannel, ACP_tdxBool _bActivate)
{
    MS_tdxHandleTo3dData h_3dData = M_GetMSHandle(_hPerso, 3dData);
    tdstAChannel *p_stChannel;
    HIE_tdstSuperObject *hModule;

    unsigned long ulNbChannels;
    CHN_tdxHandleToChannelArray hChannelArray = fn_h3dDataGetChannelSOList(h_3dData);

    if (hChannelArray == NULL)
    {
        return;
    }

    ulNbChannels = fn_ul3dDataGetNumberOfChannels(h_3dData);

    if (_uwChannel != 0xffff)
    {

        p_stChannel = &hChannelArray[_uwChannel];
        p_stChannel->bActiveChannel = _bActivate;
        hModule = p_stChannel->hZoomSupObject ? p_stChannel->hZoomSupObject : p_stChannel->hSupObject;
        fn_vUpdateSPOFlagsOfChannelForActivation(hModule, _bActivate);
    }
    else
    {

        unsigned long ulI;
        for (ulI = 0, p_stChannel = hChannelArray; ulI < ulNbChannels; ulI++, p_stChannel++)
        {
            p_stChannel = &hChannelArray[ulI];
            p_stChannel->bActiveChannel = _bActivate;
            hModule = p_stChannel->hZoomSupObject ? p_stChannel->hZoomSupObject : p_stChannel->hSupObject;
            fn_vUpdateSPOFlagsOfChannelForActivation(hModule, _bActivate);
        }
    }
}

static tdstMorphChannelList *fn_p_stCreateMorphListElement(void)
{
    tdstMorphChannelList *p_stMorphList;

    p_stMorphList = (tdstMorphChannelList *)TMP_M_p_Malloc(sizeof(tdstMorphChannelList));
    LST2_M_DynamicInitElement(p_stMorphList);

    return p_stMorphList;
}

/* 0x00419650 */
void __cdecl GLI_pc_v_vTmpFree(void *pUser)
{
    GLI_pc_fn_vFree(&pUser);
}

void __cdecl fn_vInitMorphList(CHN_tdxHandleToMorphList *p_hMorphList)
{
    if (*p_hMorphList != NULL)
    {
        return;
    }

    *p_hMorphList = (CHN_tdxHandleToMorphList)TMP_M_p_Malloc(sizeof(LST2_M_DynamicAnchorDeclaration(CHN_tdxHandleToMorphElement)));
    LST2_M_DynamicInitAnchor(*p_hMorphList);
}

static void fn_vFreeMorphListElement(tdstMorphChannelList *p_stMorphList);

void __cdecl fn_vFreeMorphList(CHN_tdxHandleToMorphList *p_hMorphList)
{
    if (*p_hMorphList == NULL) return;

    while (LST2_M_DynamicGetNumberOfElements(*p_hMorphList) > 0)
    {

        tdstMorphChannelList *p_hMorphElt = LST2_M_DynamicGetFirstElement(*p_hMorphList);

        LST2_M_DynamicIsolate(p_hMorphElt);

        fn_vFreeMorphListElement(p_hMorphElt);
    }
    TMP_M_Free(*p_hMorphList);
    *p_hMorphList = NULL;
}

void __cdecl fn_vPutMorphElementInChannel(CHN_tdxHandleToMorphList hMorphList, unsigned short uwChannel, ACP_tdxIndex IndexOfMorphList)
{
    tdstMorphChannelList *p_stMorph, *p_stMorph2;
    unsigned short uwIndex, uwNumOfElts;

    if (!hMorphList) return;

    uwNumOfElts = (unsigned short)LST2_M_DynamicGetNumberOfElements(hMorphList);
    LST2_M_DynamicForEachElementOf(hMorphList, p_stMorph2, uwIndex)
    {
        if (p_stMorph2->uwChannelNumber == uwChannel)

        {	p_stMorph2->MorphListIndex = IndexOfMorphList;
        break;
        }
        if (p_stMorph2->uwChannelNumber > uwChannel)

        {	break;
        }
    }
    if (!p_stMorph2)

    {
        p_stMorph = fn_p_stCreateMorphListElement();
        p_stMorph->uwChannelNumber = uwChannel;
        p_stMorph->MorphListIndex = IndexOfMorphList;
        LST2_M_DynamicAddTail(hMorphList, p_stMorph);
    }
    else
    {
        if (p_stMorph2->uwChannelNumber != uwChannel)

        {
            p_stMorph = fn_p_stCreateMorphListElement();
            p_stMorph->uwChannelNumber = uwChannel;
            p_stMorph->MorphListIndex = IndexOfMorphList;
            LST2_M_DynamicInsertBetween(p_stMorph, LST2_M_DynamicGetPrevElement(p_stMorph2), p_stMorph2);
        }
    }

    (void)uwNumOfElts;
}

void __cdecl fn_vRemoveChannelFromMorphList(CHN_tdxHandleToMorphList hMorphList, unsigned short uwChannel)
{
    tdstMorphChannelList *p_stMorph, *p_stMorph_Next;
    unsigned short uwIndex;

    if (!hMorphList) return;

    LST2_M_DynamicForEachMovingElementOf(hMorphList, p_stMorph, p_stMorph_Next, uwIndex)
    {
        if (p_stMorph->uwChannelNumber == uwChannel)
        {
            if (!LST2_M_DynamicGetNumberOfElements(hMorphList))  break;
            LST2_M_DynamicIsolate(p_stMorph);
            fn_vFreeMorphListElement(p_stMorph);
            break;
        }
    }
}

ACP_tdxIndex __cdecl fn_hMorphListIndex(CHN_tdxHandleToMorphList hMorphList, unsigned short uwChannel)
{
    tdstMorphChannelList *p_stMorph;
    unsigned short uwIndex;

    if (!hMorphList) return -1;

    LST2_M_DynamicForEachElementOf(hMorphList, p_stMorph, uwIndex)
    {
        if (p_stMorph->uwChannelNumber == uwChannel)

        {	return p_stMorph->MorphListIndex;
        }
        if (p_stMorph->uwChannelNumber > uwChannel)

        {	return -1;
        }
    }

    return -1;
}

void *__cdecl fn_hGetMorphedObjectInChannel(CHN_tdxHandleToMorphList hMorphList, unsigned short uwChannel)
{
    ACP_tdxIndex xIndex = fn_hMorphListIndex(hMorphList, uwChannel);
    return xIndex != -1 ? MOR_fn_hGetMorphedObject(xIndex) : NULL;
}

static void fn_vFreeMorphListElement(tdstMorphChannelList *p_stMorphList)
{
    TMP_M_Free(p_stMorphList);
}
