#include "AI/ActParam.h"
#include "GAM/GAM_types.h"
#include "AI/Brain.h"

#define M_GetIntellOrReflex(p_stMind) \
    ((p_stMind)->bDoingIntel ? (p_stMind)->p_stIntelligence : (p_stMind)->p_stReflex)

#define M_ActionParamN(p_stInt, ucN) \
    (((AI_tduGetSetParam *)AI_M_ActionTableParam((p_stInt), \
        AI_M_ActionTableCurrentEntry(p_stInt)))[(ucN)])

/* 0x00487DC0 */
void __cdecl fn_vSetActionParamxValue(unsigned char ucN,
                                      HIE_tdstSuperObject *p_SuperObjPerso, float xValue)
{
    AI_tdstIntelligence *p_stIntellOrReflex =
        M_GetIntellOrReflex(AI_M_stGetMindOfSuperObj(p_SuperObjPerso));

    M_ActionParamN(p_stIntellOrReflex, ucN).xValue = xValue;
}

/* 0x00487E10 */
MTH_tdxReal __cdecl fn_xGetActionParamxValue(unsigned char ucN,
                                             HIE_tdstSuperObject *p_SuperObjPerso)
{
    AI_tdstIntelligence *p_stIntellOrReflex =
        M_GetIntellOrReflex(AI_M_stGetMindOfSuperObj(p_SuperObjPerso));

    return M_tduGetSetParam_xValue(&M_ActionParamN(p_stIntellOrReflex, ucN));
}

/* 0x00487E60 */
long __cdecl fn_lGetActionParamlValue(unsigned char ucN,
                                      HIE_tdstSuperObject *p_SuperObjPerso)
{
    AI_tdstIntelligence *p_stIntellOrReflex =
        M_GetIntellOrReflex(AI_M_stGetMindOfSuperObj(p_SuperObjPerso));

    return M_ActionParamN(p_stIntellOrReflex, ucN).lValue;
}

/* 0x00487EA0 */
void __cdecl fn_vSetActionParamlValue(unsigned char ucN,
                                      HIE_tdstSuperObject *p_SuperObjPerso, long lValue)
{
    AI_tdstIntelligence *p_stIntellOrReflex =
        M_GetIntellOrReflex(AI_M_stGetMindOfSuperObj(p_SuperObjPerso));

    M_ActionParamN(p_stIntellOrReflex, ucN).lValue = lValue;
}

/* 0x00487EF0 */
void __cdecl fn_vSetActionParamp_stVertex(unsigned char ucN,
                                          HIE_tdstSuperObject *p_SuperObjPerso,
                                          MTH3D_tdstVector *p_stVertex)
{
    AI_tdstIntelligence *p_stIntellOrReflex =
        M_GetIntellOrReflex(AI_M_stGetMindOfSuperObj(p_SuperObjPerso));

    ((MTH3D_tdstVector *)&M_ActionParamN(p_stIntellOrReflex, ucN))->xX = p_stVertex->xX;
    ((MTH3D_tdstVector *)&M_ActionParamN(p_stIntellOrReflex, ucN))->xY = p_stVertex->xY;
    ((MTH3D_tdstVector *)&M_ActionParamN(p_stIntellOrReflex, ucN))->xZ = p_stVertex->xZ;
}

/* 0x00487F80 */
MTH3D_tdstVector *__cdecl fn_p_stGetActionParamp_stVertex(unsigned char ucN,
                                                          HIE_tdstSuperObject *p_SuperObjPerso)
{
    AI_tdstIntelligence *p_stIntellOrReflex =
        M_GetIntellOrReflex(AI_M_stGetMindOfSuperObj(p_SuperObjPerso));

    return (MTH3D_tdstVector *)&M_ActionParamN(p_stIntellOrReflex, ucN);
}

void __cdecl fn_vSetActionParamStateValue(unsigned char ucN,
                                          HIE_tdstSuperObject *p_SuperObjPerso,
                                          struct tdstState_ *hState)
{
    AI_tdstIntelligence *p_stIntellOrReflex =
        M_GetIntellOrReflex(AI_M_stGetMindOfSuperObj(p_SuperObjPerso));

    M_tduGetSetParam_HandleToState(&M_ActionParamN(p_stIntellOrReflex, ucN)) = hState;
}

struct tdstState_ *__cdecl fn_xGetActionParamStateValue(unsigned char ucN,
                                                       HIE_tdstSuperObject *p_SuperObjPerso)
{
    AI_tdstIntelligence *p_stIntellOrReflex =
        M_GetIntellOrReflex(AI_M_stGetMindOfSuperObj(p_SuperObjPerso));

    return (struct tdstState_ *)M_ActionParamN(p_stIntellOrReflex, ucN).p_stEngObjValue;
}
