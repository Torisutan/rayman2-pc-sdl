#include "GAM/STRUCTUR/State.h"

tdxHandleToState __cdecl fn_hGetStateToGoInTransition(tdxHandleToTransition h_Transition)
{
    return (h_Transition->h_StateToGo);
}

tdxHandleToState __cdecl fn_hGetNextStateInState(tdxHandleToState h_State)
{
    return (h_State->h_NextState);
}

signed char __cdecl fn_scGetSpeedInState(tdxHandleToState h_State)
{
    return (h_State->scSpeedAnim);
}

/* 0x00419990 */
void *__cdecl fn_hGetStateMechanicsIdCard(tdxHandleToState h_State)
{
    return h_State->h_LinkedMechanicsIdCard;
}

/* 0x004199A0 */
unsigned char __cdecl fn_ucGetStateCustomBits(tdxHandleToState h_State)
{
    return (h_State->ucCustomBits);
}

tdxHandleToState __cdecl fn_hGetTargetStateInTransition(tdxHandleToTransition h_Transition);

/* 0x004199B0 */
tdxHandleToTransition __cdecl fn_hIfExistTransitionalWithTargetState(
    tdxHandleToState h_State, tdxHandleToState h_TargetState)
{
    tdxHandleToTransition p_stTransition =
        (tdxHandleToTransition)h_State->hForTransitionArray.hFirst;

    while (p_stTransition != NULL)
    {
        if (fn_hGetTargetStateInTransition(p_stTransition) == h_TargetState)
            return p_stTransition;
        p_stTransition = p_stTransition->pNextElement;
    }
    return NULL;
}

long __cdecl fn_hIfExistProhibitedState(tdxHandleToState h_State, tdxHandleToState h_TargetState)
{
    tdstProhibit *p_stProhibit = (tdstProhibit *)h_State->hForProhibitArray.hFirst;
    long max = h_State->hForProhibitArray.lCount;
    long i = 0;

    while ((p_stProhibit != NULL) && (i < max))
    {
        if (p_stProhibit->h_ProhibitedState == h_TargetState)
            return (1);
        p_stProhibit = p_stProhibit->pNextElement;
        ++i;
    }
    return (0);
}

unsigned char __cdecl fn_ucGetLinkingTypeInTransition(tdxHandleToTransition h_Transition)
{
    return (h_Transition->ucLinkingType);
}

void __cdecl fn_vSetLinkingTypeInTransition(tdxHandleToTransition h_Transition,
                                            unsigned char ucLinkingType)
{
    h_Transition->ucLinkingType = ucLinkingType;
}

/* 0x00419970 */
struct tdstAnim3d_ * __cdecl fn_p_stGetAnimInState(tdxHandleToState h_State)
{
    return (h_State->p_stAnim);
}

tdxHandleToState __cdecl fn_hGetTargetStateInTransition(tdxHandleToTransition h_Transition)
{
    return (h_Transition->h_TargetState);
}
