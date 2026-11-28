#include "GAM/GAM_types.h"
#include "HIE/HIE_types.h"

tdxHandleToAlwaysActiveCharacterNode fn_hGetFirstAlwaysActiveCharNode(void)
{
    return GAM_g_hFirstAlwaysActiveCharNode;
}

HIE_tdstSuperObject * fn_hGetCharacterInAlwaysActiveCharacterNode(tdxHandleToAlwaysActiveCharacterNode hCharNode)
{
    return hCharNode->hAlwActSuperObject;
}

tdxHandleToAlwaysActiveCharacterNode fn_hGetNextAlwaysActiveCharNode(tdxHandleToAlwaysActiveCharacterNode hCharNode)
{
    return hCharNode->p_stNext;
}
