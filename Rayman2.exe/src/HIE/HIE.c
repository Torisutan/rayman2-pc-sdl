#include "HIE/HIE_types.h"
#include "POS/POS_types.h"

#include <stdlib.h>
#include <string.h>
#include "HIE/HIE_globals.h"

#include "MMG/MMG_alloc.h"
#include "GEO/GeoMem.h"
#include "GLI/GLI_globals.h"

/* 0x00514EE0 */
POS_tdstCompletePosition HIE_g_a_stCameraMatrixStack[HIE_C_lMaxMatrixInStack + 1];

HIE_tdstSuperObject *HIE_gd_hFixSPO[HIE_C_MaxNumberOfSPOInTheFix];
HIE_tdstSuperObject *HIE_gd_hFixSPOSave[HIE_C_MaxNumberOfSPOInTheFix];
unsigned char HIE_gd_ucContextOfSPOSave[HIE_C_MaxNumberOfSPOInTheFix];
/* 0x00514EA0 */
long HIE_gs_lCurrentFrame;

#define HIE_g_hCurrentMatrixMirror GLI_g_p_stCurrentMatrix

void *HIE_g_a_hMatrixStack_backing[1 + HIE_C_lMaxMatrixInStack];
#define HIE_g_a_hMatrixStackCurrent (HIE_g_a_hMatrixStack_backing)
#define HIE_g_a_hMatrixStack        (HIE_g_a_hMatrixStack_backing + 1)

/* 0x00413790 */
unsigned char __cdecl fn_ucStdGameCheckValidity(struct HIE_tdstSuperObject *_h_SprObj);

extern POS_tdstCompletePosition *g_p_stCurrentInCamCoordsMatrix;   /* 0x00514D64 */

void HIE_fn_vInitMatrixStack(void)
{

    POS_tdstCompletePosition *h_BottomMatrix = (POS_tdstCompletePosition *)HIE_g_a_hMatrixStack[0];
    HIE_g_lNbMatrixInStack = 0;
    POS_fn_vSetIdentityMatrix(h_BottomMatrix);
    HIE_g_lNbMatrixInStack++;
    HIE_g_hCurrentMatrix = HIE_g_a_hMatrixStackCurrent[HIE_g_lNbMatrixInStack];
    HIE_g_hCurrentMatrixMirror = HIE_g_hCurrentMatrix;
}

ACP_tdxBool HIE_fn_bLoadIdentity(void)
{
    if (HIE_g_lNbMatrixInStack == HIE_C_lMaxMatrixInStack)
        return 0;
    HIE_g_a_hMatrixStack[HIE_g_lNbMatrixInStack] = HIE_g_a_hMatrixStack[0];
    HIE_g_lNbMatrixInStack++;
    HIE_g_hCurrentMatrix = HIE_g_a_hMatrixStackCurrent[HIE_g_lNbMatrixInStack];
    HIE_g_hCurrentMatrixMirror = HIE_g_hCurrentMatrix;
    return 1;
}

ACP_tdxBool HIE_fn_bPopMatrix(void)
{
    if (HIE_g_lNbMatrixInStack == 0)
    {
        return 0;
    }

    HIE_g_lNbMatrixInStack--;
    HIE_g_hCurrentMatrix = HIE_g_a_hMatrixStackCurrent[HIE_g_lNbMatrixInStack];
    HIE_g_hCurrentMatrixMirror = HIE_g_hCurrentMatrix;

    return 1;
}

ACP_tdxBool HIE_fn_bStoreMatrix(void *hMatrix)
{
    long lNb = HIE_g_lNbMatrixInStack;
    if (lNb == HIE_C_lMaxMatrixInStack)
        return 0;
    HIE_g_a_hMatrixStack[lNb] = hMatrix;
    lNb++;
    HIE_g_lNbMatrixInStack = lNb;
    HIE_g_hCurrentMatrix = HIE_g_a_hMatrixStackCurrent[lNb];
    HIE_g_hCurrentMatrixMirror = HIE_g_hCurrentMatrix;
    return 1;
}
#include "GLD/GLD_Pub.h"
#include "GLI/Vpt3D.h"

int HIE_fn_bInitCameraMatrixStack(void *_hVpt)
{
    HIE_g_lNbMatrixInCameraStack = 0;

    memcpy(&HIE_g_a_stCameraMatrixStack[0], &((( GLI_tdstSpecificAttributesFor3D *)((struct GLD_tdstViewportAttributes_ *)_hVpt)->p_vSpecificToXD)->p_stCam)->stMatrix, sizeof(POS_tdstCompletePosition));
    HIE_g_lNbMatrixInCameraStack++;
    HIE_g_p_stCurrentInCamCoordsMatrix = &HIE_g_a_stCameraMatrixStack[HIE_g_lNbMatrixInCameraStack - 1];
    return 1;
}

ACP_tdxBool HIE_fn_bPushOnCameraMatrixStack(void *_hMatrix)
{
    if (HIE_g_lNbMatrixInCameraStack == 0 || HIE_g_lNbMatrixInCameraStack == HIE_C_lMaxMatrixInStack)
        return 0;
    POS_fn_vMulMatrixMatrix(
        &HIE_g_a_stCameraMatrixStack[HIE_g_lNbMatrixInCameraStack],
        &HIE_g_a_stCameraMatrixStack[HIE_g_lNbMatrixInCameraStack - 1],
        (POS_tdstCompletePosition *)_hMatrix);
    HIE_g_lNbMatrixInCameraStack++;
    HIE_g_p_stCurrentInCamCoordsMatrix = &HIE_g_a_stCameraMatrixStack[HIE_g_lNbMatrixInCameraStack - 1];
    return 1;
}

ACP_tdxBool HIE_fn_bPopOnCameraMatrixStack(void)
{
    if (HIE_g_lNbMatrixInCameraStack == 0)
        return 0;
    HIE_g_lNbMatrixInCameraStack--;
    HIE_g_p_stCurrentInCamCoordsMatrix =
        &HIE_g_a_stCameraMatrixStack[HIE_g_lNbMatrixInCameraStack - 1];
    return 1;
}

void HIE_fn_vSetSuperObjectObjectAndType(HIE_tdstSuperObject *_hSprObj,
                                         void *_hObj,
                                         unsigned long _ulType)
{
    unsigned long ulType = 1;

    if (_hObj != NULL)
    {
        ulType = _ulType;
    }

    _hSprObj->ulTypeOfLinkedObject = ulType;
    _hSprObj->hLinkedObject.p_Void = _hObj;
}

HIE_tdstSuperObject * HIE_fn_hGetElementFormFixViewport(short xIndex)
{
    if (xIndex < HIE_g_xFixIndex)
    {
        return HIE_gd_hFixSPO[xIndex];
    }

    return NULL;
}

void HIE_fn_SO_vSetSuperimposedFlag(HIE_tdstSuperObject *_hSO)
{
    if ((_hSO->ulFlags & HIE_C_Flag_Superimposed) == HIE_C_Flag_Superimposed)
    {
        return;
    }

    _hSO->ulFlags |= (HIE_C_Flag_Superimposed | HIE_C_Flag_NotPickable);

    if (HIE_g_xFixIndex < HIE_C_MaxNumberOfSPOInTheFix)
    {
        HIE_gd_hFixSPO[HIE_g_xFixIndex] = _hSO;
        HIE_g_xFixIndex++;
    }
}

void HIE_fn_SO_vClearSuperimposedFlag(HIE_tdstSuperObject *_hSO)
{
    short xIndex;

    if ((_hSO->ulFlags & HIE_C_Flag_Superimposed) != HIE_C_Flag_Superimposed)
    {
        return;
    }

    _hSO->ulFlags &= ~(HIE_C_Flag_Superimposed | HIE_C_Flag_NotPickable);

    if (HIE_g_xFixIndex == 0)
    {
        return;
    }

    for (xIndex = 0; xIndex < HIE_C_MaxNumberOfSPOInTheFix; xIndex++)
    {
        if (HIE_gd_hFixSPO[xIndex] == _hSO)
        {
            HIE_g_xFixIndex--;
            if (HIE_g_xFixIndex != 0)
            {
                HIE_gd_hFixSPO[xIndex] = HIE_gd_hFixSPO[HIE_g_xFixIndex];
            }
            HIE_gd_hFixSPO[HIE_g_xFixIndex] = NULL;
            break;
        }
    }
}

void HIE_fn_SO_SwitchSuperImposedTab(long lParam)
{
    int i;

    if (lParam != 0)
    {
        if (HIE_g_xFixIndex != 0)
        {
            for (i = 0; i < HIE_g_xFixIndex; i++)
            {
                HIE_gd_hFixSPOSave[HIE_g_xFixIndexSave++] = HIE_gd_hFixSPO[i];
                HIE_gd_ucContextOfSPOSave[i] = HIE_g_ucContext;
            }
            HIE_g_xFixIndex = 0;
        }
        HIE_g_ucContext++;
        return;
    }

    HIE_g_ucContext--;
    if (HIE_g_xFixIndexSave != 0)
    {
        HIE_g_xFixIndex = 0;
        for (; (HIE_g_xFixIndexSave > 0) &&
               (HIE_gd_ucContextOfSPOSave[HIE_g_xFixIndexSave - 1] == HIE_g_ucContext);
             HIE_g_xFixIndexSave--)
        {
            if (fn_ucStdGameCheckValidity((struct HIE_tdstSuperObject *)HIE_gd_hFixSPOSave[HIE_g_xFixIndexSave - 1]) != 0)
            {
                HIE_gd_hFixSPO[HIE_g_xFixIndex++] = HIE_gd_hFixSPOSave[HIE_g_xFixIndexSave - 1];
            }
        }
    }
}

void HIE_fn_vSetSuperObjectExcluLight(HIE_tdstSuperObject *_hSprObj,
                                      unsigned char _bExcluLight)
{
    if (_bExcluLight != 0)
    {
        _hSprObj->ulFlags |= HIE_C_Flag_ExcluLight;
    }
    else
    {
        _hSprObj->ulFlags &= ~HIE_C_Flag_ExcluLight;
    }
}

void HIE_fn_vIsolate(HIE_tdstSuperObject *_hSprObj)
{

    if (_hSprObj->p_stNextBrother != NULL)
    {
        _hSprObj->p_stNextBrother->p_stPrevBrother = _hSprObj->p_stPrevBrother;
    }

    if (_hSprObj->p_stPrevBrother != NULL)
    {
        _hSprObj->p_stPrevBrother->p_stNextBrother = _hSprObj->p_stNextBrother;
    }

    if (_hSprObj->p_stFather != NULL)
    {
        _hSprObj->p_stFather->lNumberOfChildren--;

        if (_hSprObj->p_stFather->p_stFirstChild == _hSprObj)
        {
            _hSprObj->p_stFather->p_stFirstChild = _hSprObj->p_stNextBrother;
        }

        if (_hSprObj->p_stFather->p_stLastChild == _hSprObj)
        {
            _hSprObj->p_stFather->p_stLastChild = _hSprObj->p_stPrevBrother;
        }
    }

    _hSprObj->p_stNextBrother = NULL;
    _hSprObj->p_stPrevBrother = NULL;
    _hSprObj->p_stFather = NULL;
}

void HIE_fn_vPseudoIsolate(HIE_tdstSuperObject *_hSprObj)
{
    HIE_tdstSuperObject *p_stNextBrother;

    /* 0x0045bc00 */
    p_stNextBrother = _hSprObj->p_stNextBrother;

    if (p_stNextBrother != NULL)
    {
        p_stNextBrother->p_stPrevBrother = _hSprObj->p_stPrevBrother;
    }

    if (_hSprObj->p_stPrevBrother != NULL)
    {
        _hSprObj->p_stPrevBrother->p_stNextBrother = _hSprObj->p_stNextBrother;
    }

    if (_hSprObj->p_stFather != NULL)
    {
        _hSprObj->p_stFather->lNumberOfChildren--;

        if (_hSprObj->p_stFather->p_stFirstChild == _hSprObj)
        {
            _hSprObj->p_stFather->p_stFirstChild = _hSprObj->p_stNextBrother;
        }

        if (_hSprObj->p_stFather->p_stLastChild == _hSprObj)
        {
            _hSprObj->p_stFather->p_stLastChild = _hSprObj->p_stPrevBrother;
        }
    }

    _hSprObj->p_stPrevBrother = NULL;
    _hSprObj->p_stFather = NULL;
    _hSprObj->p_stNextBrother = p_stNextBrother;
}

HIE_tdstSuperObject *HIE_fn_p_stCreateSuperObjectTab(long _lSize)
{

    HIE_tdstSuperObject *p_stNewSuperObjectTab;
    HIE_tdstSuperObject *p_stNewSuperObject;
    long lIndex;

    GEO_M_CPAMalloc(p_stNewSuperObjectTab, HIE_tdstSuperObject *,
                    sizeof(HIE_tdstSuperObject) * _lSize, E_uwGEONotEnoughtMemory);

    for (p_stNewSuperObject = p_stNewSuperObjectTab, lIndex = 0;
         lIndex < _lSize;
         p_stNewSuperObject++, lIndex++)
    {

        p_stNewSuperObject->p_stFirstChild = NULL;
        p_stNewSuperObject->p_stLastChild = NULL;
        p_stNewSuperObject->lNumberOfChildren = 0;
        p_stNewSuperObject->p_stNextBrother = NULL;
        p_stNewSuperObject->p_stPrevBrother = NULL;
        p_stNewSuperObject->p_stFather = NULL;
        HIE_fn_vSetSuperObjectObjectAndType(p_stNewSuperObject, NULL, 0);
        p_stNewSuperObject->hBoundingVolume = NULL;
        p_stNewSuperObject->lDrawModeMask = GLI_C_lAllIsEnable;
        p_stNewSuperObject->hGlobalMatrix = GEO_fn_hCreateMatrix();
        POS_fn_vSetIdentityMatrix((POS_tdstCompletePosition *)p_stNewSuperObject->hGlobalMatrix);
        HIE_fn_vInvalidateOneGlobalMatrix(p_stNewSuperObject);
        p_stNewSuperObject->fTransparenceLevel = 255.0f;
        HIE_fn_vSetSuperObjectExcluLight(p_stNewSuperObject, 0);
    }

    return p_stNewSuperObjectTab;
}

unsigned char __cdecl HIE_fn_bIsSuperObjectValid(HIE_tdstSuperObject *_hSprObj);

HIE_tdstSuperObject * HIE_fn_hForceGetSuperObjectNextBrother(HIE_tdstSuperObject *_hSprObj)
{
    if (HIE_fn_bIsSuperObjectValid(_hSprObj))
        return _hSprObj->p_stNextBrother;
    else
        return _hSprObj;
}

HIE_tdstSuperObject * HIE_fn_hForceGetSuperObjectPrevBrother(HIE_tdstSuperObject *_hSprObj)
{
    if (HIE_fn_bIsSuperObjectValid(_hSprObj))
        return _hSprObj->p_stPrevBrother;
    else
        return _hSprObj;
}

void HIE_fn_vInvalidateAllGlobalMatrices(void)
{
    HIE_gs_lCurrentFrame++;

    if (HIE_gs_lCurrentFrame == -1)
    {
        HIE_gs_lCurrentFrame = 0;
    }
}

void HIE_fn_vInvalidateOneGlobalMatrix(HIE_tdstSuperObject *_hSprObj)
{
    if (_hSprObj->lLastComputeFrame == HIE_C_lCounterNotValid)
    {
        return;
    }

    if (HIE_gs_lCurrentFrame != 0)
    {
        _hSprObj->lLastComputeFrame = HIE_gs_lCurrentFrame - 1;
    }
    else
    {
        _hSprObj->lLastComputeFrame = -2;
    }
}

void HIE_fn_vInitGlobalMatrixFrameCounter(long lValue)
{
    if (lValue != HIE_C_lCounterNotValid)
        HIE_gs_lCurrentFrame = lValue;
    else
        HIE_gs_lCurrentFrame = lValue + 1;
}

static __inline ACP_tdxBool HIE_fn_bIsGlobalMatrixValid(HIE_tdstSuperObject *_hSprObj);

void HIE_fn_vPushMatrix(HIE_tdstSuperObject *hSprObj)
{
    if (HIE_fn_bIsGlobalMatrixValid(hSprObj))
    {
        HIE_fn_bStoreMatrix(hSprObj->hGlobalMatrix);
    }
    else
    {
        POS_fn_vMulMatrixMatrix(
            (POS_tdstCompletePosition *)hSprObj->hGlobalMatrix,
            (POS_tdstCompletePosition *)HIE_g_a_hMatrixStack[HIE_g_lNbMatrixInStack - 1],
            (POS_tdstCompletePosition *)hSprObj->hLocalMatrix);
        HIE_fn_bStoreMatrix(hSprObj->hGlobalMatrix);
        HIE_fn_vValidateGlobalMatrix(hSprObj);
    }
}

/* 0x0045BFF0 */
void HIE_fn_vPopMatrix(void)
{
    HIE_fn_bPopMatrix();
}

void HIE_fn_vPushOnCameraMatrixStack(HIE_tdstSuperObject *_hSprObj)
{
    HIE_fn_bPushOnCameraMatrixStack(_hSprObj->hLocalMatrix);
}

/* 0x0045C020 */
void HIE_fn_vPopOnCameraMatrixStack(void)
{
    HIE_fn_bPopOnCameraMatrixStack();
}

void HIE_fn_vComputeNewRelativeMatrix(HIE_tdstSuperObject *_hSprObj)
{
    POS_tdstCompletePosition stTemp;

    POS_fn_vInvertMatrix(&stTemp, (POS_tdstCompletePosition *)_hSprObj->p_stFather->hGlobalMatrix);
    POS_fn_vMulMatrixMatrix((POS_tdstCompletePosition *)_hSprObj->hLocalMatrix,
                            &stTemp,
                            (POS_tdstCompletePosition *)_hSprObj->hGlobalMatrix);
}

void HIE_fn_vComputeNewRelativeScaleMatrix(HIE_tdstSuperObject *_hSprObj)
{
    HIE_fn_vComputeNewRelativeMatrix(_hSprObj);
}

void HIE_fn_vChangeFather(HIE_tdstSuperObject *_hSprObj,
                          HIE_tdstSuperObject *_hNewFather,
                          unsigned char _bFirstChild)
{
    HIE_tdstSuperObject *p_stFather;

    if (_hSprObj->p_stNextBrother != NULL)
        _hSprObj->p_stNextBrother->p_stPrevBrother = _hSprObj->p_stPrevBrother;
    if (_hSprObj->p_stPrevBrother != NULL)
        _hSprObj->p_stPrevBrother->p_stNextBrother = _hSprObj->p_stNextBrother;

    p_stFather = _hSprObj->p_stFather;
    if (p_stFather != NULL)
    {
        p_stFather->lNumberOfChildren--;
        if (_hSprObj->p_stFather->p_stFirstChild == _hSprObj)
            _hSprObj->p_stFather->p_stFirstChild = _hSprObj->p_stNextBrother;
        if (_hSprObj->p_stFather->p_stLastChild == _hSprObj)
            _hSprObj->p_stFather->p_stLastChild = _hSprObj->p_stPrevBrother;
    }

    _hSprObj->p_stNextBrother = NULL;
    _hSprObj->p_stPrevBrother = NULL;
    _hSprObj->p_stFather      = NULL;

    if (_bFirstChild != 0)
    {
        HIE_tdstSuperObject *p_stOldFirst;

        _hSprObj->p_stNextBrother = NULL;
        _hSprObj->p_stFather      = NULL;
        _hSprObj->p_stPrevBrother = NULL;
        p_stOldFirst = _hNewFather->p_stFirstChild;
        _hSprObj->p_stFather      = _hNewFather;
        _hSprObj->p_stNextBrother = p_stOldFirst;

        if (_hNewFather->p_stLastChild == NULL)
        {
            _hNewFather->p_stLastChild  = _hSprObj;
            _hNewFather->p_stFirstChild = _hSprObj;
            _hNewFather->lNumberOfChildren++;
            HIE_fn_vComputeNewRelativeScaleMatrix(_hSprObj);
            return;
        }

        _hNewFather->p_stFirstChild->p_stPrevBrother = _hSprObj;
        _hNewFather->p_stFirstChild = _hSprObj;
        _hNewFather->lNumberOfChildren++;
        HIE_fn_vComputeNewRelativeScaleMatrix(_hSprObj);
        return;
    }

    {
        HIE_tdstSuperObject *p_stOldLast;

        _hSprObj->p_stPrevBrother = NULL;
        _hSprObj->p_stFather      = NULL;
        _hSprObj->p_stNextBrother = NULL;
        p_stOldLast = _hNewFather->p_stLastChild;
        _hSprObj->p_stFather      = _hNewFather;
        _hSprObj->p_stPrevBrother = p_stOldLast;

        if (_hNewFather->p_stFirstChild == NULL)
            _hNewFather->p_stFirstChild = _hSprObj;
        else
            _hNewFather->p_stLastChild->p_stNextBrother = _hSprObj;

        _hNewFather->p_stLastChild = _hSprObj;
        _hNewFather->lNumberOfChildren++;
        HIE_fn_vComputeNewRelativeScaleMatrix(_hSprObj);
    }
}

void HIE_fn_vRefreshHierarchy(HIE_tdstSuperObject *_hSprObj)
{
    HIE_tdstSuperObject *hChild;

    HIE_fn_vPushMatrix(_hSprObj);
    for (hChild = _hSprObj->p_stFirstChild; hChild != NULL; hChild = hChild->p_stNextBrother)
        HIE_fn_vRefreshHierarchy(hChild);
    HIE_fn_vPopMatrix();
}

void HIE_fn_vInvalidateHierarchy(HIE_tdstSuperObject *_hSprObj)
{
    HIE_tdstSuperObject *hChild;

    HIE_fn_vInvalidateOneGlobalMatrix(_hSprObj);

    for (hChild = _hSprObj->p_stFirstChild;
         hChild != NULL;
         hChild = hChild->p_stNextBrother)
    {
        HIE_fn_vInvalidateHierarchy(hChild);
    }
}

void HIE_fn_vSetMagnetModification(HIE_tdstSuperObject *_hSprObj, unsigned char _xModified)
{
    if (_xModified != 0)
        _hSprObj->ulFlags |= HIE_C_Flag_MagnetModification;
    else
        _hSprObj->ulFlags &= ~HIE_C_Flag_MagnetModification;
}

unsigned char HIE_fn_bGetMagnetModification(HIE_tdstSuperObject *_hSprObj)
{
    return (_hSprObj->ulFlags & HIE_C_Flag_MagnetModification) != 0;
}

static __inline ACP_tdxBool HIE_fn_bIsGlobalMatrixValid(HIE_tdstSuperObject *_hSprObj)
{
    long lFrame;

    lFrame = _hSprObj->lLastComputeFrame;
    return (ACP_tdxBool)(lFrame == HIE_gs_lCurrentFrame || lFrame == HIE_C_lCounterNotValid);
}

unsigned char __cdecl HIE_fn_bIsSuperObjectValid(HIE_tdstSuperObject *_hSprObj)
{
    return (unsigned char)(_hSprObj != (HIE_tdstSuperObject *)0);
}
