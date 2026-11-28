#include <string.h>
#pragma intrinsic(memcpy)
#include "AI/AI_types.h"
#include "AI/DsgVarTable.h"
#include "GMT/GMT_types.h"
#include "GAM/GAM_types.h"
#include "HIE/HieSpObj.h"
#include "AI/DsgMem.h"
#include "AI/List.h"
#include "FON/FON_types.h"

#include "AI/AITools.h"

#define M_TEST_TRUNCATED(p_stGetSetParam,expression,a,b) (void) fn_ucIsThereFloatIfYesConvertItToInteger(p_stGetSetParam)

unsigned char fn_ucGetDsgVarArray(void *pMyVarAddr,unsigned char ucIndex,AI_tdstGetSetParam *p_stGetSetParam);
unsigned char fn_ucSetDsgVarArray(void *pMyVarAddr,unsigned char ucIndex,AI_tdstGetSetParam *p_stGetSetParam);

unsigned char fn_ucGetDsgVarBoolean(void *pMyVarAddr,unsigned char ucIndex,AI_tdstGetSetParam *p_stGetSetParam);

#define fn_ucGetDsgVar_128To127         fn_ucGetDsgVarBoolean

unsigned char fn_ucGetDsgVarInteger(void *pMyVarAddr,unsigned char ucIndex,AI_tdstGetSetParam *p_stGetSetParam);
#define fn_ucGetDsgVarPositiveInteger   fn_ucGetDsgVarInteger

unsigned char fn_ucSetDsgVarBoolean(void *pMyVarAddr,unsigned char ucIndex,AI_tdstGetSetParam *p_stGetSetParam);
#define fn_ucSetDsgVar_128To127         fn_ucSetDsgVarBoolean
#define fn_ucSetDsgVar0To255            fn_ucSetDsgVarBoolean

unsigned char fn_ucSetDsgVar_32768To32767(void *pMyVarAddr,unsigned char ucIndex,AI_tdstGetSetParam *p_stGetSetParam);
#define fn_ucSetDsgVar0To65535          fn_ucSetDsgVar_32768To32767

unsigned char fn_ucSetDsgVarInteger(void *pMyVarAddr,unsigned char ucIndex,AI_tdstGetSetParam *p_stGetSetParam);
#define fn_ucSetDsgVarPositiveInteger   fn_ucSetDsgVarInteger

unsigned char fn_ucSetDsgVarWayPoint(void *pMyVarAddr,unsigned char ucIndex,AI_tdstGetSetParam *p_stGetSetParam);
#define fn_ucSetDsgVarAction            fn_ucSetDsgVarWayPoint
#define fn_ucSetDsgVarText              fn_ucSetDsgVarWayPoint
#define fn_ucSetDsgVarCaps              fn_ucSetDsgVarWayPoint
#define fn_ucSetDsgVarGraph             fn_ucSetDsgVarWayPoint

unsigned char fn_ucSetDsgVarComport(void *pMyVarAddr,unsigned char ucIndex,AI_tdstGetSetParam *p_stGetSetParam);
#define fn_ucSetDsgVarGameMaterial      fn_ucSetDsgVarComport

unsigned char fn_ucSetDsgVarPerso(void *pMyVarAddr,unsigned char ucIndex,AI_tdstGetSetParam *p_stGetSetParam);
#define fn_ucSetDsgVarSuperObject       fn_ucSetDsgVarPerso

unsigned char fn_ucGetDsgVar0To255(void *pMyVarAddr,unsigned char ucIndex,AI_tdstGetSetParam *p_stGetSetParam);
unsigned char fn_ucGetDsgVar_32768To32767(void *pMyVarAddr,unsigned char ucIndex,AI_tdstGetSetParam *p_stGetSetParam);
unsigned char fn_ucGetDsgVar0To65535(void *pMyVarAddr,unsigned char ucIndex,AI_tdstGetSetParam *p_stGetSetParam);
unsigned char fn_ucGetDsgVarFloat(void *pMyVarAddr,unsigned char ucIndex,AI_tdstGetSetParam *p_stGetSetParam);
unsigned char fn_ucGetDsgVarWayPoint(void *pMyVarAddr,unsigned char ucIndex,AI_tdstGetSetParam *p_stGetSetParam);
unsigned char fn_ucGetDsgVarPerso(void *pMyVarAddr,unsigned char ucIndex,AI_tdstGetSetParam *p_stGetSetParam);
unsigned char fn_ucGetDsgVarList(void *pMyVarAddr,unsigned char ucIndex,AI_tdstGetSetParam *p_stGetSetParam);
unsigned char fn_ucGetDsgVarVector(void *pMyVarAddr,unsigned char ucIndex,AI_tdstGetSetParam *p_stGetSetParam);
unsigned char fn_ucGetDsgVarComport(void *pMyVarAddr,unsigned char ucIndex,AI_tdstGetSetParam *p_stGetSetParam);
unsigned char fn_ucGetDsgVarAction(void *pMyVarAddr,unsigned char ucIndex,AI_tdstGetSetParam *p_stGetSetParam);
unsigned char fn_ucGetDsgVarText(void *pMyVarAddr,unsigned char ucIndex,AI_tdstGetSetParam *p_stGetSetParam);
unsigned char fn_ucGetDsgVarGameMaterial(void *pMyVarAddr,unsigned char ucIndex,AI_tdstGetSetParam *p_stGetSetParam);
unsigned char fn_ucGetDsgVarCaps(void *pMyVarAddr,unsigned char ucIndex,AI_tdstGetSetParam *p_stGetSetParam);
unsigned char fn_ucGetDsgVarGraph(void *pMyVarAddr,unsigned char ucIndex,AI_tdstGetSetParam *p_stGetSetParam);
unsigned char fn_ucGetDsgVarSuperObject(void *pMyVarAddr,unsigned char ucIndex,AI_tdstGetSetParam *p_stGetSetParam);
unsigned char fn_ucSetDsgVarFloat(void *pMyVarAddr,unsigned char ucIndex,AI_tdstGetSetParam *p_stGetSetParam);
unsigned char fn_ucSetDsgVarVector(void *pMyVarAddr,unsigned char ucIndex,AI_tdstGetSetParam *p_stGetSetParam);
unsigned char fn_ucSetDsgVarList(void *pMyVarAddr,unsigned char ucIndex,AI_tdstGetSetParam *p_stGetSetParam);

/* 0x004A4278 */
tdstDsgVarTypeTableEntry a_stDsgVarTypeTable[eNbDsgVarType]=
{
			#define M_DEFINE_DSGVAR_TYPE(a,b,c,d,e,f,g,h,i,j) {d,g,h},
	#include "AI/DefVar.h"
	#undef M_DEFINE_DSGVAR_TYPE
};

/* 0x00481260 */
unsigned long fn_ulSizeOfDsgVar(tdeDsgVarTypeId eDsgVarTypeId, unsigned char ucNbElt)
{
    switch (eDsgVarTypeId)
	{

    case eDsgVarType_Caps:
        return ( C_SizeOfCaps);
    case eDsgVarType_Graph:
        return ( C_SizeOfGraph);

    case eDsgVarType_Boolean:
        return ( C_SizeOfBoolean);
    case eDsgVarType__128To127:
        return (C_SizeOf_128To127);
    case eDsgVarType_0To255:
        return (C_SizeOf0To255);
    case eDsgVarType__32768To32767:
        return (C_SizeOf_32768To32767);
    case eDsgVarType_0To65535:
        return (C_SizeOf0To65535);
    case eDsgVarType_Integer:
        return (C_SizeOfInteger);
    case eDsgVarType_PositiveInteger:
        return (C_SizeOfPositiveInteger);
    case eDsgVarType_Float:
        return (C_SizeOfFloat);
    case eDsgVarType_WayPoint:
        return (C_SizeOfWayPoint);
    case eDsgVarType_Perso:
        return (C_SizeOfPerso);
    case eDsgVarType_Text:
        return(C_SizeOfText);
    case eDsgVarType_List:
        return (C_SizeOfList1+ (ucNbElt-1)*C_SizeOfPersoInList);
    case eDsgVarType_Vector:
	  return (C_SizeOfVector);
	case eDsgVarType_Action :
	   return (C_SizeOfAction);
	case eDsgVarType_Comport :
	   return(C_SizeOfComport);
 	case eDsgVarType_GameMaterial:
		return(C_SizeOfGameMaterial);
	case eDsgVarType_SuperObject:
		return(C_SizeOfSuperObject);

	case eDsgVarType_VectorArray:
  		return(C_SizeOfArray1 - sizeof(C_TypeOfArrayElement)+ (ARRAY_FACTOR*ucNbElt*C_SizeOfArrayElement));
	case eDsgVarType_PersoArray:
	case eDsgVarType_FloatArray:
	case eDsgVarType_IntegerArray:
	case eDsgVarType_WayPointArray:
	case eDsgVarType_TextArray:
		return(C_SizeOfArray1+ (ucNbElt-1)*C_SizeOfArrayElement);

   default:
        return 0;
		break;

    }
}

/* 0x00481330 */
unsigned char fn_ucGetDsgVarFloat(void *pMyVarAddr,unsigned char ucIndex,AI_tdstGetSetParam *p_stGetSetParam)
{
	MTH_tdxReal xValue=(C_TypeOfFloat) (*((C_TypeOfFloat *)pMyVarAddr));
    ucIndex=ucIndex;
	M_Full_GetSetParam_Float(p_stGetSetParam,xValue);
	return(C_VALID_GET);
}

/* 0x00481350 */
unsigned char fn_ucGetDsgVarPerso(void *pMyVarAddr,unsigned char ucIndex,AI_tdstGetSetParam *p_stGetSetParam)
{
    C_TypeOfPerso hSuperObjectPerso =(C_TypeOfPerso) (*((C_TypeOfPerso *)pMyVarAddr));
    ucIndex=ucIndex;
	M_Full_GetSetParam_Perso(p_stGetSetParam,hSuperObjectPerso);
	if (hSuperObjectPerso==NULL)
		return(C_INVALID_GET);
	return(C_VALID_GET);
}

/* 0x004813A0 */
unsigned char fn_ucGetDsgVarSuperObject(void *pMyVarAddr,unsigned char ucIndex,AI_tdstGetSetParam *p_stGetSetParam)
{
    C_TypeOfSuperObject hSuperObject =(C_TypeOfSuperObject) (*((C_TypeOfSuperObject *)pMyVarAddr));
    ucIndex=ucIndex;
    M_Full_GetSetParam_SuperObject(p_stGetSetParam,hSuperObject);
    if (hSuperObject==NULL)
       return(C_INVALID_GET);
    return(C_VALID_GET);
}

/* 0x004813C0 */
unsigned char fn_ucGetDsgVarVector(void *pMyVarAddr,unsigned char ucIndex,AI_tdstGetSetParam *p_stGetSetParam)
{
	MTH3D_tdstVector *p_stVertex=(C_TypeOfVector *)pMyVarAddr;
    ucIndex=ucIndex;
	M_Full_GetSetParam_p_stVertex(p_stGetSetParam,p_stVertex);
	return(C_VALID_GET);
}

/* 0x004813F0 */
unsigned char fn_ucGetDsgVarCaps(void *pMyVarAddr,unsigned char ucIndex,AI_tdstGetSetParam *p_stGetSetParam)
{
    C_TypeOfCaps ulCaps=(*((C_TypeOfCaps *)pMyVarAddr));
    ucIndex=ucIndex;
    M_Full_GetSetParam_Caps(p_stGetSetParam, ulCaps);

    return(C_VALID_GET);
}

/* 0x00481410 */
unsigned char fn_ucGetDsgVarGraph(void *pMyVarAddr,unsigned char ucIndex,AI_tdstGetSetParam *p_stGetSetParam)
{
    C_TypeOfGraph hGraph=(*((C_TypeOfGraph *)pMyVarAddr));
    ucIndex=ucIndex;
    M_Full_GetSetParam_Graph(p_stGetSetParam, hGraph);

    return(C_VALID_GET);
}

/* 0x00481430 */
unsigned char fn_ucGetDsgVarWayPoint(void *pMyVarAddr,unsigned char ucIndex,AI_tdstGetSetParam *p_stGetSetParam)
{
    C_TypeOfWayPoint hWayPoint=(*((C_TypeOfWayPoint *)pMyVarAddr));
    ucIndex=ucIndex;
    M_Full_GetSetParam_WayPoint(p_stGetSetParam,hWayPoint);

    return(C_VALID_GET);
}

/* 0x00481450 */
unsigned char fn_ucGetDsgVarAction(void *pMyVarAddr,unsigned char ucIndex,AI_tdstGetSetParam *p_stGetSetParam)
{
    C_TypeOfAction hAction =(C_TypeOfAction) (*((C_TypeOfAction *)pMyVarAddr));
    M_Full_GetSetParam_Action(p_stGetSetParam,hAction);
    ucIndex=ucIndex;
    if (hAction==NULL)
       return(C_INVALID_GET);
    return(C_VALID_GET);
}

/* 0x00481470 */
unsigned char fn_ucGetDsgVarComport(void *pMyVarAddr,unsigned char ucIndex,AI_tdstGetSetParam *p_stGetSetParam)
{
    C_TypeOfComport p_stComport =(C_TypeOfComport) (*((C_TypeOfComport *)pMyVarAddr));
    ucIndex=ucIndex;
	M_Full_GetSetParam_pComport(p_stGetSetParam,p_stComport);
   	if (p_stComport==NULL)
       return(C_INVALID_GET);
    return(C_VALID_GET);
}

/* 0x00481490 */
unsigned char fn_ucGetDsgVarText(void *pMyVarAddr,unsigned char ucIndex,AI_tdstGetSetParam *p_stGetSetParam)
{
    C_TypeOfText hText=(C_TypeOfText) (*((C_TypeOfText *)pMyVarAddr));
    ucIndex=ucIndex;
    M_Full_GetSetParam_hText(p_stGetSetParam,hText);
  if ( hText == FON_C_xInvalidTextHandle )
       return(C_INVALID_GET);
    return(C_VALID_GET);
}

/* 0x004814B0 */
unsigned char fn_ucGetDsgVarList(void *pMyVarAddr,unsigned char ucIndex,AI_tdstGetSetParam *p_stGetSetParam)
{
    ucIndex=ucIndex;
    M_Full_GetSetParam_List(p_stGetSetParam,((C_TypeOfList1 *)pMyVarAddr) );
    return(C_VALID_GET);
}

/* 0x004814D0 */
unsigned char fn_ucGetDsgVarBoolean(void *pMyVarAddr,unsigned char ucIndex,AI_tdstGetSetParam *p_stGetSetParam)
{
    long lValue=( (long) (* ((C_TypeOfBoolean *)pMyVarAddr) ) );
    ucIndex=ucIndex;
    M_Full_GetSetParam_Integer(p_stGetSetParam,lValue);
    return(C_VALID_GET);
}

/* 0x004814F0 */
unsigned char fn_ucGetDsgVar0To255(void *pMyVarAddr,unsigned char ucIndex,AI_tdstGetSetParam *p_stGetSetParam)
{
	long lValue=( (long) (* ((C_TypeOf0To255 *)pMyVarAddr) ) );
	ucIndex=ucIndex;
	M_Full_GetSetParam_Integer(p_stGetSetParam,lValue);
	return(C_VALID_GET);
}

/* 0x00481510 */
unsigned char fn_ucGetDsgVar_32768To32767(void *pMyVarAddr,unsigned char ucIndex,AI_tdstGetSetParam *p_stGetSetParam)
{
	long lValue=( (long) (* ((C_TypeOf_32768To32767 *)pMyVarAddr) ) );
	ucIndex=ucIndex;
	M_Full_GetSetParam_Integer(p_stGetSetParam,lValue);
	return(C_VALID_GET);
}

/* 0x00481530 */
unsigned char fn_ucGetDsgVar0To65535(void *pMyVarAddr,unsigned char ucIndex,AI_tdstGetSetParam *p_stGetSetParam)
{
	long lValue=( (long) (* ((C_TypeOf0To65535 *)pMyVarAddr) ) );
	ucIndex=ucIndex;
	M_Full_GetSetParam_Integer(p_stGetSetParam,lValue);
	return(C_VALID_GET);
}

/* 0x00481550 */
unsigned char fn_ucGetDsgVarInteger(void *pMyVarAddr,unsigned char ucIndex,AI_tdstGetSetParam *p_stGetSetParam)
{
	long lValue=( (long) (* ((C_TypeOfInteger *)pMyVarAddr) ) );
	ucIndex=ucIndex;
	M_Full_GetSetParam_Integer(p_stGetSetParam,lValue);
	return(C_VALID_GET);
}

/* 0x00481570 */
unsigned char fn_ucGetDsgVarGameMaterial(void *pMyVarAddr,unsigned char ucIndex,AI_tdstGetSetParam *p_stGetSetParam)
{
	C_TypeOfGameMaterial hGameMaterial=( (C_TypeOfGameMaterial) ( *((C_TypeOfGameMaterial *)pMyVarAddr) ) );
	ucIndex=ucIndex;
	M_Full_GetSetParam_hGameMaterial(p_stGetSetParam,hGameMaterial);
	if (GMT_fn_b_ulIsValid(hGameMaterial))
		return(C_INVALID_GET);
	return(C_VALID_GET);
}

tdp_fn_ucGetDsgVarFunctionPtr fn_p_fn_pGetDsgVarGetFunctionPtr(enum tdeDsgVarTypeId_ eDsgVarTypeId);

/* 0x004815A0 */
unsigned char fn_ucGetDsgVarArray(void *pMyVarAddr,unsigned char ucIndex,AI_tdstGetSetParam *p_stGetSetParam)
{
	unsigned char ret=C_INVALID_GET;
	tdstArray *p_stArray=(tdstArray*) pMyVarAddr;
	tdeDsgVarTypeId eDsgVarTypeId=M_ARRAY_TYPE(p_stArray);
	{
		tdp_fn_ucGetDsgVarFunctionPtr p_fn_ucGetDsgVar=fn_p_fn_pGetDsgVarGetFunctionPtr(eDsgVarTypeId);
		ret=p_fn_ucGetDsgVar((void *) M_ARRAY_ELEMENT(p_stArray,ucIndex),0,p_stGetSetParam);
	}
	return(ret);
}

/* 0x00481660 */
unsigned char fn_ucSetDsgVarFloat(void *pMyVarAddr,unsigned char ucIndex,AI_tdstGetSetParam *p_stGetSetParam)
{
  ucIndex=ucIndex;
  *((C_TypeOfFloat *)pMyVarAddr) = (C_TypeOfFloat)  M_ReturnParam_xValue(p_stGetSetParam);
  return(C_VALID_SET);
}

/* 0x00481680 */
unsigned char fn_ucSetDsgVarPerso(void *pMyVarAddr,unsigned char ucIndex,AI_tdstGetSetParam *p_stGetSetParam)
{
   struct tdstEngineObject_ *p_stObject=M_GetSetParam_p_stEngineObjValue(p_stGetSetParam);
   ucIndex=ucIndex;
   if (p_stObject!=NULL)
   {
	  *((C_TypeOfPerso *)pMyVarAddr) = (C_TypeOfPerso) M_GetSuperObject(p_stObject);

   }
   else
   {
	  *((C_TypeOfPerso *)pMyVarAddr) = NULL;

   }
      return(C_VALID_SET);
}

/* 0x004816B0 */
unsigned char fn_ucSetDsgVarVector(void *pMyVarAddr,unsigned char ucIndex,AI_tdstGetSetParam *p_stGetSetParam)
{
   MTH3D_tdstVector *p_stVertex = &M_GetSetParam_stVertexValue(p_stGetSetParam);
   ucIndex=ucIndex;
   MTH3D_M_vCopyVector(((C_TypeOfVector *)pMyVarAddr),p_stVertex);
   return(C_VALID_SET);
}

/* 0x004816D0 */
unsigned char fn_ucSetDsgVarWayPoint(void *pMyVarAddr,unsigned char ucIndex,AI_tdstGetSetParam *p_stGetSetParam)
{
	C_TypeOfWayPoint hWayPoint = M_GetSetParam_Dsg_WayPoint(p_stGetSetParam);
	ucIndex=ucIndex;
	*((C_TypeOfWayPoint *)pMyVarAddr) = (C_TypeOfWayPoint) hWayPoint;

    return(C_VALID_SET);
}

/* 0x004816E0 */
unsigned char fn_ucSetDsgVarList(void *pMyVarAddr,unsigned char ucIndex,AI_tdstGetSetParam *p_stGetSetParam)
{
	tdstList *p_SrcList = (tdstList *) M_GetSetParam_Dsg_List(p_stGetSetParam);
	tdstList *p_TgtList = (tdstList *)pMyVarAddr;

	ucIndex=ucIndex;
	p_TgtList->ucNbElt = (unsigned char) M_Min(p_SrcList->ucNbElt, p_TgtList->ucMaxSize);
	memcpy((void*) p_TgtList->d_TabElt,(void*) p_SrcList->d_TabElt,p_TgtList->ucNbElt*C_SizeOfPersoInList);

  return(C_VALID_SET);
}

/* 0x00481730 */
unsigned char fn_ucSetDsgVarBoolean(void *pMyVarAddr,unsigned char ucIndex,AI_tdstGetSetParam *p_stGetSetParam)
{
	M_TEST_TRUNCATED(p_stGetSetParam, M_INF_SUP, 0, 1);
	ucIndex=ucIndex;
	*((C_TypeOfBoolean *)pMyVarAddr) = (C_TypeOfBoolean) M_GetSetParam_Dsg_Boolean(p_stGetSetParam);
	return(C_VALID_SET);
}

/* 0x00481750 */
unsigned char fn_ucSetDsgVar_32768To32767(void *pMyVarAddr,unsigned char ucIndex,AI_tdstGetSetParam *p_stGetSetParam)
{
	M_TEST_TRUNCATED(p_stGetSetParam, M_INF_SUP, -32768, 32767);
	ucIndex=ucIndex;
	*((C_TypeOf_32768To32767 *)pMyVarAddr) = (C_TypeOf_32768To32767) M_GetSetParam_Dsg__32768To32767(p_stGetSetParam);
	return(C_VALID_SET);
}

/* 0x00481770 */
unsigned char fn_ucSetDsgVarInteger(void *pMyVarAddr,unsigned char ucIndex,AI_tdstGetSetParam *p_stGetSetParam)
{
	M_TEST_TRUNCATED(p_stGetSetParam, M_NONE, 0, 0);
	ucIndex=ucIndex;
	*((C_TypeOfInteger *)(pMyVarAddr)) = (C_TypeOfInteger) M_GetSetParam_Dsg_Integer(p_stGetSetParam);
	return(C_VALID_SET);
}

/* 0x00481790 */
unsigned char fn_ucSetDsgVarComport(void *pMyVarAddr,unsigned char ucIndex,AI_tdstGetSetParam *p_stGetSetParam)
{
   C_TypeOfComport p_stComport = M_GetSetParam_Dsg_Comport(p_stGetSetParam);
   ucIndex=ucIndex;
   *((C_TypeOfComport *)pMyVarAddr) = (C_TypeOfComport) p_stComport;
   if (p_stComport==NULL)
 	 return(C_INVALID_SET);
   return(C_VALID_SET);
}

/* 0x004817B0 */
unsigned char fn_ucSetDsgVarArray(void *pMyVarAddr,unsigned char ucIndex,AI_tdstGetSetParam *p_stGetSetParam)
{
	unsigned char ret=C_INVALID_SET;
	tdstArray *p_stArray=(tdstArray*) pMyVarAddr;
	tdeDsgVarTypeId eDsgVarTypeId=M_ARRAY_TYPE(p_stArray);

		{
			tdp_fn_ucSetDsgVarFunctionPtr p_fn_ucSetDsgVar=fn_p_fn_pGetDsgVarSetFunctionPtr(eDsgVarTypeId);
			ret=p_fn_ucSetDsgVar((void *) M_ARRAY_ELEMENT(p_stArray,ucIndex),0,p_stGetSetParam);
		}

	return(ret);
}

/* 0x00481850 */
tdp_fn_ucGetDsgVarFunctionPtr fn_p_fn_pGetDsgVarGetFunctionPtr(enum tdeDsgVarTypeId_ eDsgVarTypeId)
{
	return(a_stDsgVarTypeTable[eDsgVarTypeId].p_fn_ucGetDsgVarFunction);
}

/* 0x00481860 */
tdp_fn_ucSetDsgVarFunctionPtr fn_p_fn_pGetDsgVarSetFunctionPtr(enum tdeDsgVarTypeId_ eDsgVarTypeId)
{
	return(a_stDsgVarTypeTable[eDsgVarTypeId].p_fn_ucSetDsgVarFunction);
}

/* 0x00481870 */
enum tdeVariableType_ fn_GetDsgVarType(enum tdeDsgVarTypeId_ eDsgVarTypeId)
{
   return(a_stDsgVarTypeTable[eDsgVarTypeId].eVariableType);
}
