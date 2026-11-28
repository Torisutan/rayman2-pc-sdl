#include "GLI/Material.h"
#include "GLI/GLI_types.h"
#include "GLI/mater_st.h"
#include "MTH/MTH_flt.h"
#include "MMG/MMG_alloc.h"
#include "GEO/GEO_globals.h"
#include "ERM/CpaError.h"

#define WIN32_LEAN_AND_MEAN
#include <windows.h>

#define GLI_g_ucMaterialAllocState  GEO_g_bDynamicAllocation

#define GLI_g_ucMemoryModule        g_ucGEOModuleId        /* 0x0049fbbc */
#define GLI_g_ucMemoryMode          g_ucGEOMMemMallocMode  /* 0x0049fbc0 */

float gs_fCoefOfMultiplicationForAnimatedTexture;
#define GLI_g_ulDefaultHandle70     (*(unsigned long *)&gs_fCoefOfMultiplicationForAnimatedTexture)

static unsigned long GLI_M_gs_lActualRefreshNumber_storage;  /* 0x4a726c */
#define GLI_g_ulDefaultHandle60     (GLI_M_gs_lActualRefreshNumber_storage)

/* 0x443360 */
typedef void (__cdecl *pfn_FinalizeMaterial_t)(int);

#define GLI_fn_vFinalizeMaterial(h)    GLI_vUnLockAnimatedTexture((void *)(h))

/* 0x00426F50 */
void __cdecl GLI_xSetLightState(void *p_stLight, long b_lOnOff);
void __cdecl GLI_xGetLightState(void *p_stLight, long *pb_lOnOff);

/* 0x00503D5C */
static float gs_fCoefOfMultiplicationForScolledTextures;

void __cdecl GLI_xSetMaterialType(void *hMaterial, unsigned long xMType)
{
    *(unsigned long *)hMaterial = xMType;
}

void __cdecl GLI_xGetMaterialType(void *hMaterial, unsigned long *xMType)
{
    *xMType = *(unsigned long *)hMaterial;
}
#define GLI_M_gs_fCoefMulScrolledTex (gs_fCoefOfMultiplicationForScolledTextures)

#define GLI_M_gs_fCoefMulAnimatedTex   (gs_fCoefOfMultiplicationForAnimatedTexture)

unsigned char GLI_M_g_bAnimFinishedFlag_storage;     /* 0x4a7270 */
unsigned char GLI_M_g_bTakeCareOfAnimCoord_storage;  /* 0x4a7274 */
#define GLI_M_gs_lActualRefreshNumber  (GLI_M_gs_lActualRefreshNumber_storage)
#define GLI_M_g_bAnimFinishedFlag      (GLI_M_g_bAnimFinishedFlag_storage)
#define GLI_M_g_bTakeCareOfAnimCoord   (GLI_M_g_bTakeCareOfAnimCoord_storage)

#include "GLI/devinfo.h"
unsigned char GLI_M_g_bOneShotAnim_storage;     /* 0x4a7278 */
float         GLI_M_g_xBaseTimeForAnim_storage; /* 0x503d60 */
#define GLI_M_g_bOneShotAnim           (GLI_M_g_bOneShotAnim_storage)
#define GLI_M_g_xBaseTimeForAnim       (GLI_M_g_xBaseTimeForAnim_storage)

#define GLI_M_xUVScrollThreshold  (0.0f)

#include "GAM/PlayAnim.h"
#include "GEO/GEO_types.h"
#include "PO/POStruc.h"
#include "GLI/VisuS_st.h"

extern tdstEngineObject *g_AlphabetCharacterPointer;

void __cdecl GLI_xCreateMaterial(void **p_hMaterial)
{
    struct GLI_tdstMaterial_ *p_stMat;

    if (GLI_g_ucMaterialAllocState != 2) {
        Mmg_fn_v_SetModeAlloc(GLI_g_ucMemoryModule, GLI_g_ucMemoryMode, 0);
        *p_hMaterial = fn_p_vGenAlloc(0x78, 0);
    }
    if (GLI_g_ucMaterialAllocState == 2) {

fallback_path:
        ERM_fn_vClearLastError(0);
        if (GLI_g_ucMaterialAllocState == 1) {
            ERM_fn_vUpdateLastError(4, GLI_g_ucMemoryModule, 0, 8, -1, 0xff, 0xff, NULL);
            ERM_fn_vClearLastError(0);
            GLI_g_ucMaterialAllocState = 2;
        }
        Mmg_fn_v_SetModeAlloc(GLI_g_ucMemoryModule, 0xff, 0);
        *p_hMaterial = fn_p_vGenAlloc(0x78, 0);
    } else if (GLI_g_ucMaterialAllocState == 1) {
        if (*p_hMaterial != NULL) goto fill_block;
        goto fallback_path;
    }
    if (*p_hMaterial == NULL) {
        ERM_fn_vClearLastError(0);
        ERM_fn_vUpdateLastError(4, GLI_g_ucMemoryModule, 0, 1, -1, 0xff, 0xff, NULL);
    }

fill_block:

    memset(*p_hMaterial, 0, 0x78);

    p_stMat = (struct GLI_tdstMaterial_ *)*p_hMaterial;

    p_stMat->ulMaterialType = 0xFFFFFFFF;

    p_stMat->stAmbient.xR = MTH_C_ZERO;
    p_stMat->stAmbient.xG = MTH_M_xConstFloatToReal(0.1f);
    p_stMat->stAmbient.xB = MTH_M_xConstFloatToReal(0.1f);

    p_stMat->stDiffuse.xR = MTH_M_xConstFloatToReal(0.8f);
    p_stMat->stDiffuse.xG = MTH_M_xConstFloatToReal(0.8f);
    p_stMat->stDiffuse.xB = MTH_M_xConstFloatToReal(0.8f);

    p_stMat->stColor.xR = MTH_C_ONE;
    p_stMat->stColor.xG = MTH_C_ZERO;
    p_stMat->stColor.xB = MTH_C_ONE;

    p_stMat->lIncrementIsEnable = 0;
    p_stMat->p_stTexture = NULL;

    p_stMat->p_stFistAnimatedTExtureNode = NULL;
    p_stMat->p_stActualAnimatedTExtureNode = NULL;
    p_stMat->lNumberOfDisplayNode = 0;
    *(unsigned long *)&p_stMat->xActualDisplayTimeSinceStartOfLastTexture = GLI_g_ulDefaultHandle70;
    p_stMat->dwActualRefreshNumber = GLI_g_ulDefaultHandle60;

    GLI_fn_vFinalizeMaterial((int)p_stMat);
}

/* 0x00424770 */
void __cdecl GLI_xDuplicateMaterial(void **p_hDest, void *hSource)
{
    typedef struct { unsigned long ul[0x1e]; } GLI_tdst30Dword;
    unsigned long  aColor[4];
    long           lSpecExp;
    void          *pTexture;
    long           lType;

    GLI_xCreateMaterial(p_hDest);

    *(GLI_tdst30Dword *)*p_hDest = *(GLI_tdst30Dword *)hSource;

    GLI_xGetMaterialAmbientCoef(hSource, aColor);   GLI_xSetMaterialAmbientCoef(*p_hDest, aColor);
    GLI_xGetMaterialDiffuseCoef(hSource, aColor);   GLI_xSetMaterialDiffuseCoef(*p_hDest, aColor);
    GLI_xGetMaterialColor(hSource, aColor);         GLI_xSetMaterialColor(*p_hDest, aColor);
    GLI_xGetMaterialTexture(hSource, &pTexture);    GLI_xSetMaterialTexture(*p_hDest, pTexture);
    GLI_xGetLightState(hSource, &lType);   /* 0x426f50 */
    GLI_xSetLightState(*p_hDest, lType);
    GLI_xGetMaterialSpecularCoef(hSource, &lSpecExp, aColor);
    GLI_xSetMaterialSpecularCoef(*p_hDest, lSpecExp, aColor);
}

/* 0x00424860 */
void __cdecl GLI_xSetMaterialAmbientCoef(void *hMaterial, void *p_stColor)
{
    ACP_tdxHandleOfMaterial hMat;

    hMat = (ACP_tdxHandleOfMaterial)hMaterial;
    hMat->stAmbient = *(GEO_tdstColor *)p_stColor;
}

/* 0x00424890 */
void __cdecl GLI_xGetMaterialAmbientCoef(void *hMaterial, void *p_stColor)
{
    ACP_tdxHandleOfMaterial hMat;

    hMat = (ACP_tdxHandleOfMaterial)hMaterial;
    *(GEO_tdstColor *)p_stColor = hMat->stAmbient;
}

/* 0x004248c0 */
void __cdecl GLI_xSetMaterialColor(void *hMaterial, void *p_stColor)
{
    ACP_tdxHandleOfMaterial hMat;

    hMat = (ACP_tdxHandleOfMaterial)hMaterial;
    hMat->stColor = *(GEO_tdstColor *)p_stColor;
}

/* 0x004248f0 */
void __cdecl GLI_xGetMaterialColor(void *hMaterial, void *p_stColor)
{
    ACP_tdxHandleOfMaterial hMat;

    hMat = (ACP_tdxHandleOfMaterial)hMaterial;
    *(GEO_tdstColor *)p_stColor = hMat->stColor;
}

/* 0x00424920 */
void __cdecl GLI_xSetMaterialDiffuseCoef(void *hMaterial, void *p_stColor)
{
    ACP_tdxHandleOfMaterial hMat;

    hMat = (ACP_tdxHandleOfMaterial)hMaterial;
    hMat->stDiffuse = *(GEO_tdstColor *)p_stColor;
}

/* 0x00424950 */
void __cdecl GLI_xGetMaterialDiffuseCoef(void *hMaterial, void *p_stColor)
{
    ACP_tdxHandleOfMaterial hMat;

    hMat = (ACP_tdxHandleOfMaterial)hMaterial;
    *(GEO_tdstColor *)p_stColor = hMat->stDiffuse;
}

/* 0x00424980 */
void __cdecl GLI_xSetMaterialSpecularCoef(void *hMaterial, long lExp, void *p_stColor)
{
    ACP_tdxHandleOfMaterial hMat;

    hMat = (ACP_tdxHandleOfMaterial)hMaterial;
    hMat->stSpecular = *(GEO_tdstColor *)p_stColor;
    hMat->lSpecularExponent = lExp;
}

/* 0x004249b0 */
void __cdecl GLI_xGetMaterialSpecularCoef(void *hMaterial, long *p_lExp, void *p_stColor)
{
    ACP_tdxHandleOfMaterial hMat;

    hMat = (ACP_tdxHandleOfMaterial)hMaterial;
    *(GEO_tdstColor *)p_stColor = hMat->stSpecular;
    *p_lExp = hMat->lSpecularExponent;
}

/* 0x004249e0 */
void __cdecl GLI_vSetMaterialAsChromed(void *hMaterial, long bChromed)
{
    if (bChromed != 0) {
        *(unsigned long *)hMaterial &= (GLI_C_lAllIsEnable - GLI_C_lIsNotChromed);
    } else {
        *(unsigned long *)hMaterial |= GLI_C_lIsNotChromed;
    }
}

void __cdecl GLI_xSetMaterialTexture(void *hMaterial, void *p_stTexture)
{
    struct GLI_tdstMaterial_ *p_stMat = (struct GLI_tdstMaterial_ *)hMaterial;
    GLI_tdstTexture *p_stTex = (GLI_tdstTexture *)p_stTexture;

    p_stMat->p_stTexture = p_stTex;
    if (p_stTex != NULL) {

        if (*(unsigned char *)&p_stTex->ulTextureCaps & 0x08)
            p_stMat->ulMaterialType &= 0xfffffbffUL;
        if (*(unsigned char *)&p_stTex->ulTextureCaps & 0x40) {
            p_stMat->ulMaterialType &= 0xf7ffffffUL;
            return;
        }
    }
    p_stMat->ulMaterialType |= 0x08000000UL;
}

/* 0x00424a40 */
void __cdecl GLI_xGetMaterialTexture(void *hMaterial, void **p_hTexture)
{

    *p_hTexture = ((ACP_tdxHandleOfMaterial)hMaterial)->p_stTexture;
}

void __cdecl GLI_xSetMaterialTextureScrollingCoef(void *hMaterial, long lNumberOfTexture,
                                                  long bSwitch, MTH_tdxReal fUSpeed,
                                                  MTH_tdxReal fVSpeed)
{
    ACP_tdxHandleOfMaterial hMat;

    hMat = (ACP_tdxHandleOfMaterial)hMaterial;
    hMat->fAddU = hMat->fConstantAddU = fUSpeed ;
    hMat->fAddV = hMat->fConstantAddV = fVSpeed ;
    hMat->lIncrementIsEnable = bSwitch ;

    if ((MTH_M_bEqual(fUSpeed, MTH_C_ZERO)) && (MTH_M_bEqual(fVSpeed, MTH_C_ZERO)))
        hMat->lIncrementIsEnable = 0;
}

void __cdecl GLI_xSetMaterialTextureDecalage(void *hMaterial, long lNumberOfTexture,
                                             long bSwitch, MTH_tdxReal fUSpeed,
                                             MTH_tdxReal fVSpeed)
{
    ACP_tdxHandleOfMaterial hMat;

    hMat = (ACP_tdxHandleOfMaterial)hMaterial;
    hMat->fAddU = fUSpeed ;
    hMat->fAddV = fVSpeed ;
    hMat->lIncrementIsEnable = bSwitch ;
    if ((MTH_M_bEqual(hMat->fAddU, MTH_C_ZERO)) && (MTH_M_bEqual(hMat->fAddV, MTH_C_ZERO)))
        hMat->lIncrementIsEnable = 0;
}

void __cdecl GLI_xGetMaterialTextureScrollingCoef(void *hMaterial, long lNumberOfTexture,
                                                  long *p_bSwitch, MTH_tdxReal *p_fUSpeed,
                                                  MTH_tdxReal *p_fVSpeed)
{
    *p_fUSpeed = ((ACP_tdxHandleOfMaterial)hMaterial)->fConstantAddU ;
    *p_fVSpeed = ((ACP_tdxHandleOfMaterial)hMaterial)->fConstantAddV ;
    *p_bSwitch = ((ACP_tdxHandleOfMaterial)hMaterial)->lIncrementIsEnable;
}

/* 0x00424B10 */
void __cdecl GLI_xSetMaterialTextureScrollingOnOff(void *hMaterial, long bSwitch)
{
    ACP_tdxHandleOfMaterial hMat;

    hMat = (ACP_tdxHandleOfMaterial)hMaterial;
    if (bSwitch
        && MTH_M_bEqual(hMat->fConstantAddU, MTH_C_ZERO)
        && MTH_M_bEqual(hMat->fConstantAddV, MTH_C_ZERO))
        return;
    hMat->lIncrementIsEnable = bSwitch;
}

/* 0x00424b40 */
long __cdecl GLI_lGetMaterialNumberOfAnimatedTexture(void *hMaterial)
{
    return ((ACP_tdxHandleOfMaterial)hMaterial)->lNumberOfDisplayNode;
}

/* 0x00424b50 */
long __cdecl GLI_lGetCurrentAnimatedTextureNumber(void *hMaterial)
{
    GLI_tdstAnimatedTextureNode *p_Node;
    long lNumber = 1;
    ACP_tdxHandleOfMaterial hMat;

    hMat = (ACP_tdxHandleOfMaterial)hMaterial;
    p_Node = hMat->p_stFistAnimatedTExtureNode;
    while (p_Node != hMat->p_stActualAnimatedTExtureNode)
    {
        lNumber ++;
        p_Node = p_Node->p_stNextDisplayNode ;
    }
    if (p_Node == hMat->p_stActualAnimatedTExtureNode)
        return lNumber;
    else
        return 0;
}

/* 0x00424b80 */
void __cdecl GLI_vSetCurrentAnimatedTextureNumber(void *hMaterial, long lTextureNumber)
{
    long  lNumber;
    GLI_tdstAnimatedTextureNode *p_Node;
    GLI_tdstAnimatedTextureNode *p_stFirst;
    ACP_tdxHandleOfMaterial hMat;

    hMat = (ACP_tdxHandleOfMaterial)hMaterial;
    p_stFirst = hMat->p_stFistAnimatedTExtureNode;
    p_Node = p_stFirst;
    lNumber = 1;

    while (p_Node != NULL && lNumber != lTextureNumber)
    {
        p_Node = p_Node->p_stNextDisplayNode;
        lNumber++;
    }
    if (p_Node == NULL)
        p_Node = p_stFirst;
    hMat->p_stActualAnimatedTExtureNode = p_Node;
}

void __cdecl GLI_vUnLockAnimatedTexture(void *hMaterial);

void __cdecl GLI_vRefreshAllCyclingTextures(float fCoef)
{
    GLI_M_gs_fCoefMulScrolledTex  = fCoef;
    GLI_M_gs_fCoefMulAnimatedTex  = GLI_M_gs_fCoefMulAnimatedTex + fCoef;
    GLI_M_gs_lActualRefreshNumber = GLI_M_gs_lActualRefreshNumber + 1;
}
void __cdecl GLI_pc_vRefreshOneCyclingTexture(void *hMaterial)
{
    struct GLI_tdstMaterial_ *p_stMat = (struct GLI_tdstMaterial_ *)hMaterial;
    struct GLI_tdstTexture   *p_stTexture;
    GLI_tdstAnimatedTextureNode *p_stNode;
    unsigned long ulSavedMaterialType;

    p_stTexture = p_stMat->p_stTexture;
    if (p_stTexture == 0)
        goto set_refresh_number;

    if (p_stMat->lIncrementIsEnable == 0) {
        p_stTexture->lIncrementIsEnable = 0;
    } else {
        p_stTexture->lIncrementIsEnable = 1;
        if (p_stMat->lIncrementIsEnable == 1 &&
            p_stMat->dwActualRefreshNumber != GLI_M_gs_lActualRefreshNumber) {
            float xAddU, xAddV;
            p_stMat->fAddU =
                p_stMat->fConstantAddU * GLI_M_gs_fCoefMulScrolledTex + p_stMat->fAddU;
            p_stMat->fAddV =
                p_stMat->fConstantAddV * GLI_M_gs_fCoefMulScrolledTex + p_stMat->fAddV;
            xAddU = p_stMat->fAddU;
            if (xAddU < GLI_M_xUVScrollThreshold)
                p_stMat->fAddU = -((-xAddU) - (float)(long)(-xAddU));
            else
                p_stMat->fAddU = xAddU - (float)(long)xAddU;
            xAddV = p_stMat->fAddV;
            if (xAddV < GLI_M_xUVScrollThreshold)
                p_stMat->fAddV = -((-xAddV) - (float)(long)(-xAddV));
            else
                p_stMat->fAddV = xAddV - (float)(long)xAddV;
        }

        *(long *)&p_stMat->p_stTexture->xAddU = *(long *)&p_stMat->fAddU;
        *(long *)&p_stMat->p_stTexture->xAddV = *(long *)&p_stMat->fAddV;
    }

    if (p_stMat->lNumberOfDisplayNode <= 1)
        goto set_refresh_number;
    if (p_stMat->xIsLocked != 0)
        goto set_refresh_number;
    if (p_stMat->dwActualRefreshNumber == GLI_M_gs_lActualRefreshNumber)
        goto set_refresh_number;

    GLI_M_g_bAnimFinishedFlag = 0;
    if (GLI_M_g_bTakeCareOfAnimCoord != 0) {

        p_stNode = p_stMat->p_stFistAnimatedTExtureNode;
        p_stMat->p_stActualAnimatedTExtureNode = p_stNode;
        while (GLI_M_gs_fCoefMulAnimatedTex - GLI_M_g_xBaseTimeForAnim
               > p_stNode->xDisplayTime * GLI_pc_g_stGliCaps.xRefreshRate) {
            GLI_M_g_xBaseTimeForAnim =
                p_stNode->xDisplayTime * GLI_pc_g_stGliCaps.xRefreshRate + GLI_M_g_xBaseTimeForAnim;
            p_stNode = p_stNode->p_stNextDisplayNode;
            if (p_stNode == 0) {
                if (GLI_M_g_bOneShotAnim != 0)
                    GLI_M_g_bAnimFinishedFlag = 1;
                else
                    p_stMat->p_stActualAnimatedTExtureNode = p_stMat->p_stFistAnimatedTExtureNode;
                goto swap_texture;
            }
            p_stMat->p_stActualAnimatedTExtureNode = p_stNode;
        }
    } else {

        p_stNode = p_stMat->p_stActualAnimatedTExtureNode;
        if ((unsigned long)(*(unsigned long *)&p_stNode->xDisplayTime - 1) >= 0x7fffffff)
            goto swap_texture;
        while (GLI_M_gs_fCoefMulAnimatedTex - p_stMat->xActualDisplayTimeSinceStartOfLastTexture
               > p_stNode->xDisplayTime * GLI_pc_g_stGliCaps.xRefreshRate) {
            p_stMat->xActualDisplayTimeSinceStartOfLastTexture =
                p_stNode->xDisplayTime * GLI_pc_g_stGliCaps.xRefreshRate +p_stMat->xActualDisplayTimeSinceStartOfLastTexture;
            p_stNode = p_stNode->p_stNextDisplayNode;
            if (p_stNode == 0)
                p_stNode = p_stMat->p_stFistAnimatedTExtureNode;
            p_stMat->p_stActualAnimatedTExtureNode = p_stNode;
        }
    }

swap_texture:
    {
        GLI_tdstTexture *p_stNodeTex = p_stMat->p_stActualAnimatedTExtureNode->p_stTexture;
        GLI_tdstTexture *p_stMatTex  = p_stMat->p_stTexture;
        if (p_stNodeTex != 0 && p_stMatTex != 0) {
            *(long *)&p_stNodeTex->xAddU = *(long *)&p_stMatTex->xAddU;
            *(long *)&p_stNodeTex->xAddV = *(long *)&p_stMatTex->xAddV;
            p_stNodeTex->lIncrementIsEnable = p_stMatTex->lIncrementIsEnable;
        }

        GLI_xGetMaterialType(p_stMat, &ulSavedMaterialType);
        GLI_xSetMaterialTexture(p_stMat, p_stMat->p_stActualAnimatedTExtureNode->p_stTexture);
        GLI_xSetMaterialType(p_stMat, ulSavedMaterialType);
    }

set_refresh_number:
    if (GLI_M_g_bTakeCareOfAnimCoord == 0)
        p_stMat->dwActualRefreshNumber = GLI_M_gs_lActualRefreshNumber;
}

/* 0x00424E90 */
void *__cdecl GLI_pGetVisualMaterialForLetter(int c)
{
    PO_tdstPhysicalObject *Po;
    tdxHandleToObjectsTablesList d_stObjectsTable;
    MS_tdxHandleTo3dData p3d;
    GEO_tdstGeometricObject *pObj;

    if (g_AlphabetCharacterPointer)
    {
        p3d = g_AlphabetCharacterPointer->h_3dData;
        d_stObjectsTable = p3d->h_CurrentObjectsTable;
        if (c <= d_stObjectsTable->wNumberOfElement)
        {
            Po = (PO_tdstPhysicalObject *)(d_stObjectsTable->d_stObjectsTable[c].h_Target);
            if (Po)
            {
                pObj = ((GEO_tdstVisualSet *)Po->_hVisualSet)->d_p_stLodDefinitions[0];
                if (pObj && pObj->xNbElements == 1
                    && pObj->d_xListOfElementsTypes[0] == GEO_C_xElementIndexedTriangles)
                    return ((GEO_tdstElementIndexedTriangles *)pObj->d_stListOfElements[0])->hMaterial->hVisualMaterial;
            }
        }
    }
    return NULL;
}
