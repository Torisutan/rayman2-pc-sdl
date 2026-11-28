#include <string.h>
#include "MOR/MOR.h"
#include "HIE/HIE_types.h"
#include "GEO/GEO_types.h"
#include "GEO/GeoObj.h"
#include "MTH/MTH_types.h"
#include "TMP/TmpMem.h"
#include "GAM/GAM_types.h"
#include "GAM/ChanList.h"

MOR_tdstMorphStruct MorphList[MOR_C_NbMorphTasks];   /* 0x0050b200 */

int MOR_g_MaxMorphTask = -1;                                /* 0x004a11d4 */
#define MaxMorphTask  MOR_g_MaxMorphTask

#include "PO/POStruc.h"
#include "GLI/VisuS_st.h"

/* 0x0042af60 */
extern int __cdecl GLI_lGetVisualSetNumbertOfLOD(void *p);
/* 0x0042ae70 */
struct GEO_tdstVisualSet_ *GLI_p_stCreateTMPVisualSet(long lNbLOD);

static __inline void MOR_pc_vSetSuperObjectObjectAndType(HIE_tdstSuperObject *_hSprObj, void *_hObj, unsigned long _ulType)
{
    unsigned long ulType;

    ulType = (_hObj != NULL) ? _ulType : 1;
    _hSprObj->hLinkedObject.p_Void = _hObj;
    _hSprObj->ulTypeOfLinkedObject = ulType;
}

#include "GAM/PlayAnim.h"

typedef struct { GEO_tdstElementCones stCones; long lPcTail; } MOR_pc_tdstElementCones12;

#define MOR_M_PO_VISUALSET(p) (((PO_tdstPhysicalObject *)(p))->_hVisualSet)

/* 0x0042af20 */
extern GEO_tdstGeometricObject *GLI_hGetVisualSetGeometricForADistance(GEO_tdstVisualSet *p_stVSE, MTH_tdxReal xThreshold);

/* 0x004435E0 */
ACP_tdxIndex __cdecl MOR_fn_xPrepareMorph(HIE_tdstSuperObject *_hActorSuperObject,
                                          ACP_tdxIndex _xStartIndexInObjectTable,
                                          ACP_tdxIndex _xEndIndexInObjectTable,
                                          int _iNbMillisecond, int _iStart, int _iEnd,
                                          unsigned char _ucMorphType,
                                          unsigned short _xChannelIndex)
{
    MS_tdxHandleTo3dData    h_3dData    = M_GetMSHandle(_hActorSuperObject, 3dData);
    void                   *hMorphList  = fn_h3dDataGetMorphList(h_3dData);
    tdstObjectsTablesList  *h_ObjTbl;
    ACP_tdxIndex            xMorphListIndex;
    ACP_tdxIndex            xFreePosition = -1;
    MOR_tdstMorphStruct    *mp;
    PO_tdstPhysicalObject  *pStartPhysicalObject;
    PO_tdstPhysicalObject  *pEndPhysicalObject;
    GEO_tdstVisualSet      *pStartGeo;
    GEO_tdstVisualSet      *pEndGeo;
    GEO_tdstVisualSet      *pNewGeo;
    PO_tdstPhysicalObject  *pMorphPO;
    short                   j;
    long                    i;
    long                    lNbElements;

    _xStartIndexInObjectTable--;
    _xEndIndexInObjectTable--;

    xMorphListIndex = fn_hMorphListIndex(hMorphList, _xChannelIndex);
    if (xMorphListIndex != -1 && MorphList[xMorphListIndex].bInUse == TRUE)
        return -1;

    for (j = 0; j < MOR_C_NbMorphTasks; j++)
    {
        if (MorphList[j].bInUse == FALSE)
        {
            xFreePosition = (ACP_tdxIndex)j;
            break;
        }
    }
    if (xFreePosition == -1)
        return -1;

    h_ObjTbl             = fn_h3dDataGetCurrentObjectsTable(h_3dData);
    pStartPhysicalObject = (PO_tdstPhysicalObject *)h_ObjTbl->d_stObjectsTable[_xStartIndexInObjectTable].h_Target;
    pEndPhysicalObject   = (PO_tdstPhysicalObject *)h_ObjTbl->d_stObjectsTable[_xEndIndexInObjectTable].h_Target;

    pStartGeo = pStartPhysicalObject->_hVisualSet;
    pEndGeo   = pEndPhysicalObject->_hVisualSet;

    if (GLI_lGetVisualSetNumbertOfLOD(pStartGeo) != GLI_lGetVisualSetNumbertOfLOD(pEndGeo))
        return -1;
    for (i = 0; i < GLI_lGetVisualSetNumbertOfLOD(pStartGeo); i++)
    {
        GEO_tdstGeometricObject *pStartElement = GLI_hGetVisualSetGeometricForADistance(pStartGeo, (float)i);
        GEO_tdstGeometricObject *pEndElement   = GLI_hGetVisualSetGeometricForADistance(pEndGeo, (float)i);

        if (pStartElement->xNbPoints != pEndElement->xNbPoints)
            return -1;
    }

    mp = MorphList + xFreePosition;

    mp->bInUse                   = TRUE;
    mp->xChannelIndex            = _xChannelIndex;
    mp->hActorSuperObject        = _hActorSuperObject;
    mp->xStartIndexInObjectTable = _xStartIndexInObjectTable;
    mp->xEndIndexInObjectTable   = _xEndIndexInObjectTable;
    mp->iNbMillisecond           = _iNbMillisecond * 10;
    mp->iStart                   = _iStart * 10;
    mp->iEnd                     = _iEnd * 10;
    mp->iCurrent                 = _iStart * 10;
    mp->ucMorphType              = _ucMorphType;
    mp->iStep                    = mp->iStart <= mp->iEnd ? 10 : -10;
    mp->hOriginalPhysicalObject  = pStartPhysicalObject;

    pMorphPO = (PO_tdstPhysicalObject *)TMP_M_p_Malloc(sizeof(PO_tdstPhysicalObject));
    mp->p_stMorphPO = pMorphPO;

    lNbElements = GLI_lGetVisualSetNumbertOfLOD(pStartGeo);
    pNewGeo = GLI_p_stCreateTMPVisualSet(lNbElements);
    pMorphPO->_hVisualSet = pNewGeo;
    pNewGeo->xLastDistance = pStartGeo->xLastDistance;
    pNewGeo->xNbLodDefinitions = (ACP_tdxIndex)GLI_lGetVisualSetNumbertOfLOD(pStartGeo);   /* 0x42AF60 */
    pMorphPO->_hCollideSet      = pStartPhysicalObject->_hCollideSet;
    pMorphPO->_hBoundingVolume  = pStartPhysicalObject->_hBoundingVolume;

    for (i = 0; i < GLI_lGetVisualSetNumbertOfLOD(pNewGeo); i++)
    {
        pNewGeo->d_xThresholdsTable[i] = pStartGeo->d_xThresholdsTable[i];
        pNewGeo->d_p_stLodDefinitions[i] = MOR_fn_xCopyGeometricObjectForMorph(pStartGeo->d_p_stLodDefinitions[i]);
    }

    fn_vPutMorphElementInChannel(hMorphList, _xChannelIndex, xFreePosition);

    if (xFreePosition > MaxMorphTask)
        MaxMorphTask = xFreePosition;

    return xFreePosition;
}

ACP_tdxIndex __cdecl MOR_fn_xStopMorph(HIE_tdstSuperObject *_hActorSuperObject,
                                       unsigned short _xChannelIndex)
{
    ACP_tdxIndex          xMorphListIndex;
    short                 j;
    MOR_tdstMorphStruct * mp = NULL;

    xMorphListIndex = fn_hMorphListIndex(fn_h3dDataGetMorphList(M_GetMSHandle(_hActorSuperObject, 3dData)), _xChannelIndex);
    if (xMorphListIndex == -1 || MorphList[xMorphListIndex].bInUse == FALSE)
        return -1;

    for (j = 0; j <= MaxMorphTask; j++)
    {
        mp = MorphList + j;

        if ((mp->bInUse != FALSE) && mp->hActorSuperObject == _hActorSuperObject && mp->xChannelIndex == _xChannelIndex)
            break;
        else
            mp = NULL;
    }

    if (mp == NULL)
        return -1;

    MOR_fn_bRemoveMorphedObjectFromHierarchy(mp->hActorSuperObject,
        mp->p_stMorphPO,
        fn_h3dDataGetCurrentObjectsTable(M_GetMSHandle(mp->hActorSuperObject, 3dData))->d_stObjectsTable[mp->xEndIndexInObjectTable].h_Target);

    MOR_fn_vDeletePhysicalObject(mp->p_stMorphPO);
    mp->p_stMorphPO = NULL;
    MOR_fn_vCleanMorphTask(mp);
    if (j == MaxMorphTask)
        MaxMorphTask--;

    fn_vRemoveChannelFromMorphList(fn_h3dDataGetMorphList(M_GetMSHandle(_hActorSuperObject, 3dData)), _xChannelIndex);

    return 0;
}

ACP_tdxIndex __cdecl MOR_fn_xMorphing(HIE_tdstSuperObject *_hActorSuperObject,
                                      short _xStartIndexInObjectTable,
                                      short _xEndIndexInObjectTable,
                                      int _iRatio,
                                      unsigned short _xChannelIndex)
{
    MS_tdxHandleTo3dData   h_3dData   = M_GetMSHandle(_hActorSuperObject, 3dData);
    tdstObjectsTablesList *h_ObjTbl   = fn_h3dDataGetCurrentObjectsTable(h_3dData);
    void                  *hMorphList = fn_h3dDataGetMorphList(h_3dData);
    PO_tdstPhysicalObject *pStart = (PO_tdstPhysicalObject *)h_ObjTbl->d_stObjectsTable[_xStartIndexInObjectTable].h_Target;
    PO_tdstPhysicalObject *pEnd   = (PO_tdstPhysicalObject *)h_ObjTbl->d_stObjectsTable[_xEndIndexInObjectTable].h_Target;
    GEO_tdstVisualSet     *geoStart = pStart->_hVisualSet;
    GEO_tdstVisualSet     *geoEnd   = pEnd->_hVisualSet;
    ACP_tdxIndex xMorphListIndex;
    ACP_tdxBool  bCreateObject;
    MOR_tdstMorphStruct *mp;
    long         j;

    if (GLI_lGetVisualSetNumbertOfLOD(geoStart) != GLI_lGetVisualSetNumbertOfLOD(geoEnd))
        return -1;

    for (j = 0; j < GLI_lGetVisualSetNumbertOfLOD(geoStart); j++)
    {
        if (geoStart->d_p_stLodDefinitions[j]->xNbPoints !=
            geoEnd->d_p_stLodDefinitions[j]->xNbPoints)
            return -1;
    }

    xMorphListIndex = fn_hMorphListIndex(hMorphList, _xChannelIndex);
    if (xMorphListIndex == -1 || MorphList[xMorphListIndex].bInUse == FALSE)
    {
        short k;
        xMorphListIndex = -1;
        for (k = 0; k < MOR_C_NbMorphTasks; k++)
        {
            if (MorphList[k].bInUse == FALSE)
            {
                xMorphListIndex = k;
                break;
            }
        }
        if (xMorphListIndex == -1)
            return -1;
        bCreateObject = TRUE;
    }
    else
    {
        bCreateObject = FALSE;
    }

    mp = &MorphList[xMorphListIndex];
    mp->bInUse                   = TRUE;
    mp->xChannelIndex            = _xChannelIndex;
    mp->hActorSuperObject        = _hActorSuperObject;
    mp->xStartIndexInObjectTable = _xStartIndexInObjectTable;
    mp->xEndIndexInObjectTable   = _xEndIndexInObjectTable;
    mp->iNbMillisecond           = 100;
    mp->iEnd                     = _iRatio;
    mp->iStart                   = mp->iEnd - 1;
    mp->iCurrent                 = mp->iStart;
    mp->ucMorphType              = MOR_C_MORPH_WAITATEND_ANIM;
    mp->iStep                    = 10;
    mp->hOriginalPhysicalObject  = pStart;

    if (bCreateObject)
    {
        PO_tdstPhysicalObject *pMorphPO = (PO_tdstPhysicalObject *)TMP_M_p_Malloc(sizeof(PO_tdstPhysicalObject));
        GEO_tdstVisualSet     *geoMorph;

        mp->p_stMorphPO = pMorphPO;
        geoMorph = GLI_p_stCreateTMPVisualSet(GLI_lGetVisualSetNumbertOfLOD(geoStart));
        pMorphPO->_hVisualSet = geoMorph;
        geoMorph->xLastDistance     = geoStart->xLastDistance;
        geoMorph->xNbLodDefinitions = (ACP_tdxIndex)GLI_lGetVisualSetNumbertOfLOD(geoStart);
        pMorphPO->_hCollideSet = pStart->_hCollideSet;
        pMorphPO->_hBoundingVolume = pStart->_hBoundingVolume;

        for (j = 0; j < GLI_lGetVisualSetNumbertOfLOD(geoMorph); j++)
        {
            PO_tdstPhysicalObject *pOrig   = (PO_tdstPhysicalObject *)mp->hOriginalPhysicalObject;
            GEO_tdstVisualSet     *geoOrig = pOrig->_hVisualSet;
            geoMorph->d_xThresholdsTable[j]   = geoOrig->d_xThresholdsTable[j];
            geoMorph->d_p_stLodDefinitions[j] = MOR_fn_xCopyGeometricObjectForMorph(geoOrig->d_p_stLodDefinitions[j]);
        }

        fn_vPutMorphElementInChannel(hMorphList, _xChannelIndex, xMorphListIndex);
    }

    if (xMorphListIndex > MaxMorphTask)
        MaxMorphTask = xMorphListIndex;

    return xMorphListIndex;
}

void __cdecl MOR_fn_vMorphTo(MOR_tdstMorphStruct *mp)
{
    float                  xRatio;
    long                   j, k;
    MS_tdxHandleTo3dData   h_3dData;
    tdstObjectsTablesList *h_ObjTbl;
    PO_tdstPhysicalObject *phy1, *phy2, *phy3;
    GEO_tdstVisualSet     *geo1, *geo2, *geo3;

    xRatio = (float)mp->iCurrent;
    xRatio = xRatio / (float)mp->iNbMillisecond;
    if (xRatio <= 1.0f)
    {
        if (xRatio < 0.0f)
            xRatio = 0.0f;
    }
    else
    {
        xRatio = 1.0f;
    }

    h_3dData = M_GetMSHandle(mp->hActorSuperObject, 3dData);
    h_ObjTbl = fn_h3dDataGetCurrentObjectsTable(h_3dData);
    phy1 = (PO_tdstPhysicalObject *)h_ObjTbl->d_stObjectsTable[mp->xStartIndexInObjectTable].h_Target;
    phy3 = (PO_tdstPhysicalObject *)mp->p_stMorphPO;
    phy2 = (PO_tdstPhysicalObject *)h_ObjTbl->d_stObjectsTable[mp->xEndIndexInObjectTable].h_Target;
    geo1 = phy1->_hVisualSet;
    geo2 = phy2->_hVisualSet;
    geo3 = phy3->_hVisualSet;

    for (j = 0; j < GLI_lGetVisualSetNumbertOfLOD(geo1); j++)
    {
        GEO_tdstGeometricObject *e1 = geo1->d_p_stLodDefinitions[j];
        GEO_tdstGeometricObject *e2 = geo2->d_p_stLodDefinitions[j];
        GEO_tdstGeometricObject *e3 = geo3->d_p_stLodDefinitions[j];
        short nbPoints = (short)e3->xNbPoints;

        for (k = 0; k < nbPoints; k++)
        {
            MTH3D_tdstVector *p1 = &e1->d_stListOfPoints[k];
            MTH3D_tdstVector *p2 = &e2->d_stListOfPoints[k];
            MTH3D_tdstVector *p3 = &e3->d_stListOfPoints[k];
            p3->xX = (p2->xX - p1->xX) * xRatio + p1->xX;
            p3->xY = (p2->xY - p1->xY) * xRatio + p1->xY;
            p3->xZ = p1->xZ + (p2->xZ - p1->xZ) * xRatio;
        }

        GEO_vEndModifyObject2(e3);
    }
}

void __cdecl MOR_fn_vCleanMorphTask(MOR_tdstMorphStruct *mp)
{
    int   i;
    int  *p;

    if (mp->bInUse != FALSE)
    {
        p = (int *)mp;
        for (i = 0xc; i != 0; i--)
            *p++ = 0;
        mp->bInUse = FALSE;
    }
}

void __cdecl GEO_fn_vMorphAddInMainLoop(void)
{
    short i;
    MOR_tdstMorphStruct *mp;
    int iIncrement;

    for (i = 0; i <= MaxMorphTask; i++)
    {
        mp = MorphList + i;

        if (mp->bInUse == FALSE)
            continue;

        iIncrement   = (long)GAM_g_ulUsefulDeltaTime * mp->iStep;
        mp->iCurrent += iIncrement;

        switch (mp->ucMorphType)
        {
        case 1:
            if (mp->iCurrent > mp->iEnd)
            {
                MS_tdxHandleTo3dData   h_3dData   = M_GetMSHandle(mp->hActorSuperObject, 3dData);
                tdstObjectsTablesList *h_ObjTbl   = fn_h3dDataGetCurrentObjectsTable(h_3dData);
                void                  *pEndTarget = h_ObjTbl->d_stObjectsTable[mp->xEndIndexInObjectTable].h_Target;
                void                  *hMorphList = fn_h3dDataGetMorphList(h_3dData);

                MOR_fn_bRemoveMorphedObjectFromHierarchy(mp->hActorSuperObject, mp->p_stMorphPO, pEndTarget);
                MOR_fn_vDeletePhysicalObject(mp->p_stMorphPO);
                fn_vRemoveChannelFromMorphList(hMorphList, mp->xChannelIndex);
                MOR_fn_vCleanMorphTask(mp);
                if (i == MaxMorphTask)
                    MaxMorphTask--;
                break;
            }
            MOR_fn_vMorphTo(mp);
            break;

        case 2:
            if (mp->iCurrent > mp->iEnd)
            {
                if (mp->iCurrent - iIncrement < mp->iEnd)
                    mp->iCurrent = mp->iEnd;
                else
                    break;
            }
            MOR_fn_vMorphTo(mp);
            break;

        case 3:
            if (mp->iCurrent > mp->iEnd)
            {
                if (mp->iCurrent < mp->iEnd + (mp->iEnd - mp->iStart))
                    mp->iCurrent = mp->iEnd - (mp->iCurrent - mp->iEnd);
                else
                    mp->iCurrent = mp->iEnd;
                if (mp->iStep >= 0)
                    mp->iStep = -mp->iStep;
            }
            else if (mp->iCurrent < mp->iStart)
            {
                if (mp->iCurrent > mp->iStart - (mp->iEnd - mp->iStart))
                    mp->iCurrent = mp->iStart + (mp->iStart - mp->iCurrent);
                else
                    mp->iCurrent = mp->iStart;
                if (mp->iStep < 0)
                    mp->iStep = -mp->iStep;
            }
            MOR_fn_vMorphTo(mp);
            break;

        default:
            break;
        }
    }
}

void __cdecl MOR_pc_fn_vCleanAllMorphTasks(void)
{
    short i;

    for (i = 0; (int)i <= MaxMorphTask; i++)
        if (*((unsigned char *)MorphList + (int)i * 0x30) != 0)
            MOR_fn_vCleanMorphTask((MOR_tdstMorphStruct *)((char *)MorphList + (int)i * 0x30));
}

/* 0x0045dc60 */
ACP_tdxBool __cdecl MOR_fn_bIsChannelMorphed(HIE_tdstSuperObject *_hActorSuperObject,
                                             unsigned short _xChannelIndex)
{
    short                 i;
    MOR_tdstMorphStruct  *mp;

    for (i = 0; i <= MaxMorphTask; i++)
    {
        mp = MorphList + i;

        if (mp->bInUse
            && mp->hActorSuperObject == _hActorSuperObject
            && mp->xChannelIndex == _xChannelIndex)
        {

            if ((mp->ucMorphType == 2 || mp->ucMorphType == 1) && mp->iCurrent >= mp->iEnd)
                return FALSE;
            else
                return TRUE;
        }
    }

    return FALSE;
}
void __cdecl MOR_fn_vRemoveAllMorphedObjects(void)
{
    short i;
    MOR_tdstMorphStruct *mp;

    for (i = 0; (int)i <= MaxMorphTask; i++) {
        mp = MorphList + i;
        if (mp->bInUse != 0) {
            MOR_fn_bRemoveMorphedObjectFromHierarchy(mp->hActorSuperObject,
                                                     mp->p_stMorphPO,
                                                     mp->hOriginalPhysicalObject);

            if (((struct tdstEngineObject_ *)mp->hActorSuperObject->hLinkedObject.p_Void)->h_StandardGame) {
                fn_vRemoveChannelFromMorphList(
                    ((MS_tdxHandleTo3dData)((struct tdstEngineObject_ *)mp->hActorSuperObject->hLinkedObject.p_Void)->h_3dData)->hMorphChannelList,
                    (unsigned short)mp->xChannelIndex);
            }
            MOR_fn_vDeletePhysicalObject(mp->p_stMorphPO);
            MOR_fn_vCleanMorphTask(mp);
        }
    }
    MaxMorphTask = -1;
}

unsigned char __cdecl MOR_fn_bRemoveMorphedObjectFromHierarchy(
    HIE_tdstSuperObject *hSuperObject, void *p_stPhysicalObjectToRemove,
    void *p_stPhysicalObjectToPut)
{
    HIE_tdstSuperObject *hChildSuperObject;

    if (HIE_fn_hGetSuperObjectObject(hSuperObject) == p_stPhysicalObjectToRemove)
    {
        MOR_pc_vSetSuperObjectObjectAndType(hSuperObject, p_stPhysicalObjectToPut, HIE_C_ulPO);
        return TRUE;
    }

    for (hChildSuperObject = hSuperObject->p_stFirstChild;
         hChildSuperObject != NULL;
         hChildSuperObject = hChildSuperObject->p_stNextBrother)
    {
        if (MOR_fn_bRemoveMorphedObjectFromHierarchy(hChildSuperObject,
                p_stPhysicalObjectToRemove, p_stPhysicalObjectToPut))
            return TRUE;
    }

    return FALSE;
}

void *__cdecl MOR_fn_hGetMorphedObject(short _xIndex)
{
    return MorphList[_xIndex].p_stMorphPO;
}

void __cdecl MOR_fn_vDoMorph(short _xIndex)
{
    MOR_fn_vMorphTo(&MorphList[_xIndex]);
}

void *__cdecl MOR_fn_xCopyGeometricObjectForMorph(GEO_tdstGeometricObject *_pGeometricObjectToCopy)
{
    /* 0x444470 */
    static const unsigned long aulElementSize[10] =
        { 0, 0x20, 0x10, 0xc, 0x1c, 0x10, 0x14, 8, 8, 0xc };
    GEO_tdstGeometricObject *pNew;
    short xNbPoints   = (short)_pGeometricObjectToCopy->xNbPoints;
    short xNbElements = (short)_pGeometricObjectToCopy->xNbElements;
    short i;

    GEO_vCreateTMPGeometricObject((ACP_tdxHandleOfObject *)&pNew, xNbPoints, xNbElements);
    if (pNew == NULL)
        return NULL;

    memcpy(pNew->d_stListOfPoints, _pGeometricObjectToCopy->d_stListOfPoints,
           (size_t)xNbPoints * sizeof(MTH3D_tdstVector));
    memcpy(pNew->d_stListOfPointsNormals, _pGeometricObjectToCopy->d_stListOfPointsNormals,
           (size_t)xNbPoints * sizeof(MTH3D_tdstVector));
    memcpy(pNew->d_xListOfElementsTypes, _pGeometricObjectToCopy->d_xListOfElementsTypes,
           (size_t)xNbElements * sizeof(ACP_tdxIndex));

#define MOR_M_CopyElement(_type) \
        pNew->d_stListOfElements[i] = GLI_pc_fn_p_vAlloc(sizeof(_type)); \
        if (pNew->d_stListOfElements[i] == NULL) \
            return NULL; \
        pNew->d_xListOfElementsTypes[i] = _pGeometricObjectToCopy->d_xListOfElementsTypes[i]; \
        *(_type *)pNew->d_stListOfElements[i] = *(_type *)_pGeometricObjectToCopy->d_stListOfElements[i]
    for (i = 0; i < xNbElements; i++)
    {
        /* 0x444470 */
        switch (_pGeometricObjectToCopy->d_xListOfElementsTypes[i])
        {
        case GEO_C_xElementPoints:
            MOR_M_CopyElement(GEO_tdstElementPoints);
            break;
        case GEO_C_xElementLines:
            MOR_M_CopyElement(GEO_tdstElementLines);
            break;
        case GEO_C_xElementFaceMapDescriptors:
            MOR_M_CopyElement(GEO_tdstElementFaceMapDescriptors);
            break;
        case GEO_C_xElementSprites:
            MOR_M_CopyElement(GEO_tdstElementSprite);
            break;
        case GEO_C_xElementSpheres:
        case GEO_C_xElementAlignedBoxes:
            MOR_M_CopyElement(GEO_tdstElementSpheres);
            break;
        case GEO_C_xElementCones:

            MOR_M_CopyElement(MOR_pc_tdstElementCones12);
            break;
        case GEO_C_xElementTMeshes:
            MOR_M_CopyElement(GEO_tdstElementTMeshes);
            break;
        case GEO_C_xElementIndexedTriangles:
            MOR_M_CopyElement(GEO_tdstElementIndexedTriangles);
            break;
        case GEO_C_xElementNULL:
            break;
        }
    }
#undef MOR_M_CopyElement

    return pNew;
}

void __cdecl MOR_fn_vDeleteGeometricObject(void *_ppGeometricObject)
{
    GEO_tdstGeometricObject **ppGeo = (GEO_tdstGeometricObject **)_ppGeometricObject;
    GEO_tdstGeometricObject  *pGeo  = *ppGeo;
    short i;

    /* 0x444544 */
    for (i = 0; i < pGeo->xNbElements; i++)
    {
        short xType = (short)pGeo->d_xListOfElementsTypes[i];

        if (xType >= 1 && xType <= 9)
            TMP_fn_vFree(&pGeo->d_stListOfElements[i]);
    }

    TMP_fn_vFree(&pGeo->d_stListOfPoints);
    TMP_fn_vFree(&pGeo->d_stListOfPointsNormals);
    TMP_fn_vFree(&pGeo->d_xListOfElementsTypes);
    TMP_fn_vFree(&pGeo->d_stListOfElements);
    TMP_fn_vFree(ppGeo);
}

void __cdecl MOR_fn_vDeletePhysicalObject(void *_pPhysicalObject)
{
    short i;

    for (i = 0; i < MOR_M_PO_VISUALSET(_pPhysicalObject)->xNbLodDefinitions; i++)
        MOR_fn_vDeleteGeometricObject(&MOR_M_PO_VISUALSET(_pPhysicalObject)->d_p_stLodDefinitions[i]);

    TMP_fn_vFree(&MOR_M_PO_VISUALSET(_pPhysicalObject)->d_xThresholdsTable);
    TMP_fn_vFree(&MOR_M_PO_VISUALSET(_pPhysicalObject)->d_p_stLodDefinitions);
    TMP_fn_vFree(&((PO_tdstPhysicalObject *)_pPhysicalObject)->_hVisualSet);
    TMP_fn_vFree(&_pPhysicalObject);
}
