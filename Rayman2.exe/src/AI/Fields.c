#include <string.h>
#include "MTH/MTH_flt.h"
#include "AI/AI_types.h"
#include "GAM/PlayAnim.h"
#include "AI/FieldTable.h"
#include "POS/POS_types.h"

#include "AI/AITools.h"

void __cdecl fn_vSetFieldTypeVector(AI_tdstGetSetParam *p_stValue)
{
    (void)p_stValue;
}

void __cdecl fn_vGetFieldTypeComport(AI_tdstGetSetParam *p_stValue)
{
    (void)p_stValue;
}

void __cdecl fn_vSetFieldTypeComport(AI_tdstGetSetParam *p_stValue)
{
    (void)p_stValue;
}

#include "DNM/DNM.h"

#include "MTH/MTH3D.h"

extern unsigned char __cdecl IPT_pc_fn_ucGetFieldPadGlobalVector(void *, AI_tdstGetSetParam *);    /* 0x0048D110 */
extern unsigned char __cdecl IPT_pc_fn_ucSetFieldPadGlobalVector(void *, AI_tdstGetSetParam *);    /* 0x0048D140 */
extern unsigned char __cdecl IPT_pc_fn_ucGetFieldPadHorizontalAxis(void *, AI_tdstGetSetParam *);  /* 0x0048D160 */
extern unsigned char __cdecl IPT_pc_fn_ucSetFieldPadHorizontalAxis(void *, AI_tdstGetSetParam *);  /* 0x0048D180 */
extern unsigned char __cdecl IPT_pc_fn_ucGetFieldPadVerticalAxis(void *, AI_tdstGetSetParam *);    /* 0x0048D1A0 */
extern unsigned char __cdecl IPT_pc_fn_ucSetFieldPadVerticalAxis(void *, AI_tdstGetSetParam *);    /* 0x0048D1C0 */
extern unsigned char __cdecl IPT_pc_fn_ucGetFieldPadAnalogForce(void *, AI_tdstGetSetParam *);     /* 0x0048D1E0 */
extern unsigned char __cdecl IPT_pc_fn_ucSetFieldPadAnalogForce(void *, AI_tdstGetSetParam *);     /* 0x0048D200 */
extern unsigned char __cdecl IPT_pc_fn_ucGetFieldPadTrueAnalogForce(void *, AI_tdstGetSetParam *); /* 0x0048D210 */
extern unsigned char __cdecl IPT_pc_fn_ucSetFieldPadTrueAnalogForce(void *, AI_tdstGetSetParam *); /* 0x0048D230 */
extern unsigned char __cdecl IPT_pc_fn_ucGetFieldPadRotationAngle(void *, AI_tdstGetSetParam *);   /* 0x0048D240 */
extern unsigned char __cdecl IPT_pc_fn_ucSetFieldPadRotationAngle(void *, AI_tdstGetSetParam *);   /* 0x0048D260 */
extern unsigned char __cdecl IPT_pc_fn_ucGetFieldPadSector(void *, AI_tdstGetSetParam *);          /* 0x0048D270 */
extern unsigned char __cdecl IPT_pc_fn_ucSetFieldPadSector(void *, AI_tdstGetSetParam *);          /* 0x0048D290 */

unsigned char __cdecl fn_ucGetFieldPosition(void *p_stContext, AI_tdstGetSetParam *p_stValue);
unsigned char __cdecl fn_ucGetFieldAbsoluteAxisX(void *p_stContext, AI_tdstGetSetParam *p_stValue);
#ifdef RAYMAN2_FIX_BUGS
/* FIX(BUGS.md#4) */
unsigned char __cdecl fn_ucGetFieldAbsoluteAxisY(void *p_stContext, AI_tdstGetSetParam *p_stValue);
unsigned char __cdecl fn_ucGetFieldAbsoluteAxisZ(void *p_stContext, AI_tdstGetSetParam *p_stValue);
#endif /* RAYMAN2_FIX_BUGS */
unsigned char __cdecl fn_ucSetFieldDefaultFunction(void *p_stContext, AI_tdstGetSetParam *p_stValue);
unsigned char __cdecl fn_ucSetFieldShadowScaleX(void *p_stContext, AI_tdstGetSetParam *p_stValue);
unsigned char __cdecl fn_ucSetFieldShadowScaleY(void *p_stContext, AI_tdstGetSetParam *p_stValue);
unsigned char __cdecl fn_ucGetFieldSpeed(void *p_stContext, AI_tdstGetSetParam *p_stValue);
unsigned char __cdecl fn_ucGetFieldNormSpeed(void *p_stContext, AI_tdstGetSetParam *p_stValue);
unsigned char __cdecl fn_ucGetFieldOrientation(HIE_tdstSuperObject *hSuperObjPerso, AI_tdstGetSetParam *p_stGetSetParam);
unsigned char __cdecl fn_ucGetFieldPrevComportIntell(void *p_stContext, AI_tdstGetSetParam *p_stValue);
unsigned char __cdecl fn_ucGetFieldPrevComportReflex(void *p_stContext, AI_tdstGetSetParam *p_stValue);
unsigned char __cdecl fn_ucGetFieldShadowScaleX(void *p_stContext, AI_tdstGetSetParam *p_stValue);
unsigned char __cdecl fn_ucGetFieldShadowScaleY(void *p_stContext, AI_tdstGetSetParam *p_stValue);

/* 0x004A6598 */
tdstFieldTableEntryA a_stFieldTableA[18] =
{
    { 2, fn_ucGetFieldPosition,          fn_ucSetFieldDefaultFunction },
    { 2, fn_ucGetFieldOrientation,       fn_ucSetFieldDefaultFunction },
    { 2, fn_ucGetFieldSpeed,             fn_ucSetFieldDefaultFunction },
    { 1, fn_ucGetFieldNormSpeed,         fn_ucSetFieldDefaultFunction },
    { 2, fn_ucGetFieldAbsoluteAxisX,     fn_ucSetFieldDefaultFunction },
#ifdef RAYMAN2_FIX_BUGS
    /* FIX(BUGS.md#4) */
    { 2, fn_ucGetFieldAbsoluteAxisY,     fn_ucSetFieldDefaultFunction },
    { 2, fn_ucGetFieldAbsoluteAxisZ,     fn_ucSetFieldDefaultFunction },
#else
    { 2, fn_ucGetFieldAbsoluteAxisX,     fn_ucSetFieldDefaultFunction },
    { 2, fn_ucGetFieldAbsoluteAxisX,     fn_ucSetFieldDefaultFunction },
#endif /* RAYMAN2_FIX_BUGS */
    { 3, fn_ucGetFieldPrevComportIntell, fn_ucSetFieldDefaultFunction },
    { 3, fn_ucGetFieldPrevComportReflex, fn_ucSetFieldDefaultFunction },
    { 1, fn_ucGetFieldShadowScaleX,      fn_ucSetFieldShadowScaleX },
    { 1, fn_ucGetFieldShadowScaleY,      fn_ucSetFieldShadowScaleY },
    { 2, IPT_pc_fn_ucGetFieldPadGlobalVector,    IPT_pc_fn_ucSetFieldPadGlobalVector },
    { 0, IPT_pc_fn_ucGetFieldPadHorizontalAxis,  IPT_pc_fn_ucSetFieldPadHorizontalAxis },
    { 0, IPT_pc_fn_ucGetFieldPadVerticalAxis,    IPT_pc_fn_ucSetFieldPadVerticalAxis },
    { 1, IPT_pc_fn_ucGetFieldPadAnalogForce,     IPT_pc_fn_ucSetFieldPadAnalogForce },
    { 1, IPT_pc_fn_ucGetFieldPadTrueAnalogForce, IPT_pc_fn_ucSetFieldPadTrueAnalogForce },
    { 1, IPT_pc_fn_ucGetFieldPadRotationAngle,   IPT_pc_fn_ucSetFieldPadRotationAngle },
    { 0, IPT_pc_fn_ucGetFieldPadSector,          IPT_pc_fn_ucSetFieldPadSector },
};

void __cdecl fn_vGetFieldTypeFloat(AI_tdstGetSetParam *p_stValue);
void __cdecl fn_vGetFieldTypeInteger(AI_tdstGetSetParam *p_stValue);
void __cdecl fn_vSetFieldTypeInteger(AI_tdstGetSetParam *p_stValue);
void __cdecl fn_vGetFieldTypeVector(AI_tdstGetSetParam *p_stValue);

/* 0x004A6670 */
tdstFieldTableEntryB a_stFieldTypeTable[5] =
{
    { 6,    fn_vGetFieldTypeInteger, fn_vSetFieldTypeInteger },
    { 8,    fn_vGetFieldTypeFloat,   fn_vGetFieldTypeFloat   },
    { 0x15, fn_vGetFieldTypeVector,  fn_vSetFieldTypeVector  },
    { 0x22, fn_vGetFieldTypeComport, fn_vSetFieldTypeComport },
    { 3,    fn_vGetFieldTypeInteger, fn_vSetFieldTypeInteger },
};

void __cdecl fn_vGetFieldTypeVector(AI_tdstGetSetParam *p_stValue)
{
    (void)p_stValue;
}

/* 0x00455060 */
unsigned char __cdecl fn_ucSetFieldDefaultFunction(void *p_stContext,
                                                   AI_tdstGetSetParam *p_stValue)
{
    return 0;
}

/* 0x00487C60 */
void __cdecl fn_vSetFieldTypeInteger(AI_tdstGetSetParam *p_stValue)
{
    (void)fn_ucIsThereFloatIfYesConvertItToInteger(p_stValue);
}

unsigned long __cdecl fn_eGetFieldTypeIndex(unsigned long ulFieldId);
tdp_fn_ucGetSetField __cdecl fn_p_fn_ucGetFieldSetFunctionPtr(unsigned long ulFieldId);
tdp_fn_vFieldPostProc __cdecl fn_p_fn_vSetFieldPostProcFunctionPtr(unsigned long ulTypeIndex);

/* 0x00487C70 */
unsigned char __cdecl fn_ucSetFieldValue(unsigned long ulFieldId, void *p_stContext,
                                         AI_tdstGetSetParam *p_stValue)
{
    unsigned long         ulTypeIndex   = fn_eGetFieldTypeIndex(ulFieldId);
    tdp_fn_ucGetSetField  p_fn_writer   = fn_p_fn_ucGetFieldSetFunctionPtr(ulFieldId);
    tdp_fn_vFieldPostProc p_fn_postproc = fn_p_fn_vSetFieldPostProcFunctionPtr(ulTypeIndex);

    p_fn_postproc(p_stValue);
    return p_fn_writer(p_stContext, p_stValue);
}

/* 0x00487CB0 */
void __cdecl fn_vGetFieldTypeFloat(AI_tdstGetSetParam *p_stValue)
{
    if (p_stValue->eType != 8)
    {
        long lValue = p_stValue->uParam.lValue;
        p_stValue->eType = 8;
        p_stValue->uParam.xValue = (float)lValue;
    }
}

/* 0x00487CD0 */
void __cdecl fn_vGetFieldTypeInteger(AI_tdstGetSetParam *p_stValue)
{
    if (p_stValue->eType == 8)
    {
        p_stValue->uParam.lValue = (long)p_stValue->uParam.xValue;
        p_stValue->eType = 6;
    }
}

tdp_fn_ucGetSetField __cdecl fn_p_fn_ucGetFieldGetFunctionPtr(unsigned long ulFieldId);
tdp_fn_vFieldPostProc __cdecl fn_p_fn_vGetFieldPostProcFunctionPtr(unsigned long ulTypeIndex);

/* 0x00487CF0 */
unsigned char __cdecl fn_ucGetFieldValue(unsigned long ulFieldId, void *p_stContext,
                                         AI_tdstGetSetParam *p_stValue)
{
    unsigned long         ulTypeIndex  = fn_eGetFieldTypeIndex(ulFieldId);
    tdp_fn_ucGetSetField  p_fn_reader  = fn_p_fn_ucGetFieldGetFunctionPtr(ulFieldId);
    tdp_fn_vFieldPostProc p_fn_postproc = fn_p_fn_vGetFieldPostProcFunctionPtr(ulTypeIndex);
    unsigned char         ucRet;

    memset(&p_stValue->uParam, 0, sizeof(p_stValue->uParam));
    p_stValue->eType = 0;
    ucRet = p_fn_reader(p_stContext, p_stValue);
    if (ucRet == 1)
        p_fn_postproc(p_stValue);
    return ucRet;
}

/* 0x00488FA0 */
tdp_fn_vFieldPostProc __cdecl fn_p_fn_vGetFieldPostProcFunctionPtr(unsigned long ulTypeIndex)
{
    return a_stFieldTypeTable[ulTypeIndex].p_fn_vGetPostProc;
}

/* 0x00488FB0 */
tdp_fn_vFieldPostProc __cdecl fn_p_fn_vSetFieldPostProcFunctionPtr(unsigned long ulTypeIndex)
{
    return a_stFieldTypeTable[ulTypeIndex].p_fn_vSetPostProc;
}

/* 0x00488FC0 */
unsigned long __cdecl fn_eGetFieldTypeIndex(unsigned long ulFieldId)
{
    return a_stFieldTableA[ulFieldId].eTypeIndex;
}

/* 0x00488FD0 */
tdp_fn_ucGetSetField __cdecl fn_p_fn_ucGetFieldGetFunctionPtr(unsigned long ulFieldId)
{
    return a_stFieldTableA[ulFieldId].p_fn_ucGetField;
}

/* 0x00488FE0 */
tdp_fn_ucGetSetField __cdecl fn_p_fn_ucGetFieldSetFunctionPtr(unsigned long ulFieldId)
{
    return a_stFieldTableA[ulFieldId].p_fn_ucSetField;
}

/* 0x0048CE20 */
unsigned char __cdecl fn_ucGetFieldPrevComportIntell(void *p_stContext,
                                                     AI_tdstGetSetParam *p_stValue)
{
    AI_tdstMind         *p_stMind  = AI_M_pstGetMindOfBrain(M_GetMSHandle((HIE_tdstSuperObject *)p_stContext, Brain));
    AI_tdstIntelligence *p_stIntel = AI_M_p_stGetIntelligence(p_stMind);
    p_stValue->uParam.lValue = (long)AI_M_GetPrevComport(p_stIntel);
    p_stValue->eType = 0x22;
    return 1;
}

/* 0x0048CE50 */
unsigned char __cdecl fn_ucGetFieldPrevComportReflex(void *p_stContext,
                                                     AI_tdstGetSetParam *p_stValue)
{
    AI_tdstMind         *p_stMind   = AI_M_pstGetMindOfBrain(M_GetMSHandle((HIE_tdstSuperObject *)p_stContext, Brain));
    AI_tdstIntelligence *p_stReflex = AI_M_p_stGetReflex(p_stMind);
    p_stValue->uParam.lValue = (long)AI_M_GetPrevComport(p_stReflex);
    p_stValue->eType = 0x22;
    return 1;
}

/* 0x0048CE80 */
unsigned char __cdecl fn_ucGetFieldShadowScaleX(void *p_stContext,
                                                AI_tdstGetSetParam *p_stValue)
{
    tdst3dData *p_st3dData = M_GetMSHandle((HIE_tdstSuperObject *)p_stContext, 3dData);
    float xVal = p_st3dData->xShadowScaleX;
    p_stValue->uParam.xValue = xVal;
    p_stValue->eType = 8;
    return 1;
}

/* 0x0048CEA0 */
unsigned char __cdecl fn_ucSetFieldShadowScaleX(void *p_stContext,
                                                AI_tdstGetSetParam *p_stValue)
{
    float xVal = p_stValue->uParam.xValue;
    tdst3dData *p_st3dData = M_GetMSHandle((HIE_tdstSuperObject *)p_stContext, 3dData);
    p_st3dData->xShadowScaleX = xVal;
    return 1;
}

/* 0x0048CEC0 */
unsigned char __cdecl fn_ucGetFieldShadowScaleY(void *p_stContext,
                                                AI_tdstGetSetParam *p_stValue)
{
    tdst3dData *p_st3dData = M_GetMSHandle((HIE_tdstSuperObject *)p_stContext, 3dData);
    float xVal = p_st3dData->xShadowScaleY;
    p_stValue->uParam.xValue = xVal;
    p_stValue->eType = 8;
    return 1;
}

/* 0x0048CEE0 */
unsigned char __cdecl fn_ucSetFieldShadowScaleY(void *p_stContext,
                                                AI_tdstGetSetParam *p_stValue)
{
    float xVal = p_stValue->uParam.xValue;
    tdst3dData *p_st3dData = M_GetMSHandle((HIE_tdstSuperObject *)p_stContext, 3dData);
    p_st3dData->xShadowScaleY = xVal;
    return 1;
}

/* 0x0048CF00 */
unsigned char __cdecl fn_ucGetFieldPosition(void *p_stContext,
                                            AI_tdstGetSetParam *p_stValue)
{

    *(long *)&p_stValue->uParam.a_xVectorValue[0] = *(long *)&HIE_fn_hGetSuperObjectGlobalMatrix((HIE_tdstSuperObject *)p_stContext)->stTranslationVector.xX;
    *(long *)&p_stValue->uParam.a_xVectorValue[1] = *(long *)&HIE_fn_hGetSuperObjectGlobalMatrix((HIE_tdstSuperObject *)p_stContext)->stTranslationVector.xY;
    *(long *)&p_stValue->uParam.a_xVectorValue[2] = *(long *)&HIE_fn_hGetSuperObjectGlobalMatrix((HIE_tdstSuperObject *)p_stContext)->stTranslationVector.xZ;
    p_stValue->eType = 0x15;
    return 1;
}
unsigned char __cdecl fn_ucGetFieldOrientation(HIE_tdstSuperObject *hSuperObjPerso, AI_tdstGetSetParam *p_stGetSetParam)
{

	MTH3D_tdstVector stVertexDirection, stVertex;

	MTH3D_M_vSetVectorElements(&stVertexDirection, MTH_C_ZERO, MTH_C_MinusONE, MTH_C_ZERO);
	POS_fn_vMulMatrixVertex(&stVertexDirection,HIE_fn_hGetSuperObjectGlobalMatrix(hSuperObjPerso),&stVertexDirection);

	POS_fn_vGetTranslationVector(HIE_fn_hGetSuperObjectGlobalMatrix(hSuperObjPerso),&stVertex);

	MTH3D_M_vSubVector(&stVertexDirection, &stVertexDirection, &stVertex);
	MTH3D_M_vNormalizeVector(&stVertexDirection,&stVertexDirection);

	M_Full_GetSetParam_p_stVertex(p_stGetSetParam,&stVertexDirection);
    return(C_VALID_GET);
}

/* 0x0048D000 */
unsigned char __cdecl fn_ucGetFieldSpeed(void *p_stContext,
                                         AI_tdstGetSetParam *p_stValue)
{
    MS_tdxHandleToDynam p_hDynam = M_GetMSHandle((HIE_tdstSuperObject *)p_stContext, Dynam);

    p_stValue->eType = 0x15;
    if (p_hDynam != NULL)
    {
        DNM_tdstDynamics *p_stDynam = (DNM_tdstDynamics *)fn_p_stDynamGetDynamics((MS_tdxHandleToDynam)p_hDynam);   /* 0x419B90 */
        if (p_stDynam != NULL)
        {

            MTH3D_tdstVector *p_stDest = (MTH3D_tdstVector *)p_stValue;

            *p_stDest = *DNM_M_pDynamicsGetPreviousSpeed(p_stDynam);
            return 1;
        }
    }
    *(long *)&p_stValue->uParam.a_xVectorValue[0] = 0;
    *(long *)&p_stValue->uParam.a_xVectorValue[1] = 0;
    *(long *)&p_stValue->uParam.a_xVectorValue[2] = 0;
    return 1;
}

/* 0x0048D060 */
unsigned char __cdecl fn_ucGetFieldNormSpeed(void *p_stContext,
                                             AI_tdstGetSetParam *p_stValue)
{
    MS_tdxHandleToDynam p_hDynam = M_GetMSHandle((HIE_tdstSuperObject *)p_stContext, Dynam);

    p_stValue->eType = 8;
    if (p_hDynam != NULL)
    {
        DNM_tdstDynamics *p_stDynam = (DNM_tdstDynamics *)fn_p_stDynamGetDynamics((MS_tdxHandleToDynam)p_hDynam);   /* 0x419B90 */
        if (p_stDynam != NULL)
        {
            MTH3D_tdstVector *p_speed = DNM_M_pDynamicsGetPreviousSpeed(p_stDynam);
            float x = MTH3D_M_xGetXofVector(p_speed), y = MTH3D_M_xGetYofVector(p_speed), z = MTH3D_M_xGetZofVector(p_speed);
            p_stValue->uParam.xValue = MTH_M_xSqrt(x * x + y * y + z * z);
            return 1;
        }
    }
    p_stValue->uParam.xValue = 0.0f;
    return 1;
}

/* 0x0048D0D0 */
unsigned char __cdecl fn_ucGetFieldAbsoluteAxisX(void *p_stContext,
                                                 AI_tdstGetSetParam *p_stValue)
{
    POS_tdstCompletePosition *p_stMatrix =
        HIE_fn_hGetSuperObjectGlobalMatrix((HIE_tdstSuperObject *)p_stContext);
#ifdef RAYMAN2_FIX_BUGS
    /* FIX(BUGS.md#4) */
    /* the axes were fetched through NULL (crash); AbsoluteAxisY/Z shared this getter */
    MTH3D_tdstVector stI, stJ, stK;

    POS_fn_vGetRotationMatrix(p_stMatrix, &stI, &stJ, &stK);
    p_stValue->uParam.a_xVectorValue[0] = stI.xX;
    p_stValue->uParam.a_xVectorValue[1] = stI.xY;
    p_stValue->uParam.a_xVectorValue[2] = stI.xZ;
#else
    MTH3D_tdstVector *p_stNull = (MTH3D_tdstVector *)0;

    POS_fn_vGetRotationMatrix(p_stMatrix, p_stNull, p_stNull, p_stNull);
    *(long *)&p_stValue->uParam.a_xVectorValue[0] = *(long *)&p_stNull->xX;
    *(long *)&p_stValue->uParam.a_xVectorValue[1] = *(long *)&p_stNull->xY;
    *(long *)&p_stValue->uParam.a_xVectorValue[2] = *(long *)&p_stNull->xZ;
#endif /* RAYMAN2_FIX_BUGS */
    p_stValue->eType = 0x15;
    return 1;
}
#ifdef RAYMAN2_FIX_BUGS
/* FIX(BUGS.md#4) */
unsigned char __cdecl fn_ucGetFieldAbsoluteAxisY(void *p_stContext,
                                                 AI_tdstGetSetParam *p_stValue)
{
    MTH3D_tdstVector stI, stJ, stK;

    POS_fn_vGetRotationMatrix(HIE_fn_hGetSuperObjectGlobalMatrix((HIE_tdstSuperObject *)p_stContext),
                              &stI, &stJ, &stK);
    p_stValue->uParam.a_xVectorValue[0] = stJ.xX;
    p_stValue->uParam.a_xVectorValue[1] = stJ.xY;
    p_stValue->uParam.a_xVectorValue[2] = stJ.xZ;
    p_stValue->eType = 0x15;
    return 1;
}

/* FIX(BUGS.md#4) */
unsigned char __cdecl fn_ucGetFieldAbsoluteAxisZ(void *p_stContext,
                                                 AI_tdstGetSetParam *p_stValue)
{
    MTH3D_tdstVector stI, stJ, stK;

    POS_fn_vGetRotationMatrix(HIE_fn_hGetSuperObjectGlobalMatrix((HIE_tdstSuperObject *)p_stContext),
                              &stI, &stJ, &stK);
    p_stValue->uParam.a_xVectorValue[0] = stK.xX;
    p_stValue->uParam.a_xVectorValue[1] = stK.xY;
    p_stValue->uParam.a_xVectorValue[2] = stK.xZ;
    p_stValue->eType = 0x15;
    return 1;
}
#endif /* RAYMAN2_FIX_BUGS */
