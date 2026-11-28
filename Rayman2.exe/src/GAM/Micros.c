#include <string.h>
#include "GAM/GAM_types.h"
#include "SND/SND.h"

#include "GAM/GAM_globals.h"
#include "POS/POS_types.h"

#include "DNM/DNM.h"

#include "GAM/Always.h"
#include "GAM/MSSound.h"
#include "SCT/Sector.h"
#include "GAM/Actions/SectInfo.h"

/* 0x00411CC0 */
void *__cdecl fn_p_stGetFirstEngineObjectInMicroList(void);
void *__cdecl fn_hReturnHandleMicroFromIndex(long lIndex);

#define GAM_C_NBLOCKEVENTSLOTNUMBER    10

void *GAM_ga_pstBlockEventSlotMemory[GAM_C_NBLOCKEVENTSLOTNUMBER];
#define GAM_pfn_RetObjetSonoreXAnim         ((void *)&RetObjetSonoreXAnim)
#define GAM_pfn_RetObjetSonoreYAnim         ((void *)&RetObjetSonoreYAnim)
#define GAM_pfn_RetObjetSonoreZAnim         ((void *)&RetObjetSonoreZAnim)
#define GAM_pfn_RetObjetSonoreVXAnim        ((void *)&RetObjetSonoreVXAnim)
#define GAM_pfn_RetObjetSonoreVYAnim        ((void *)&RetObjetSonoreVYAnim)
#define GAM_pfn_RetObjetSonoreVZAnim        ((void *)&RetObjetSonoreVZAnim)
/* 0x00455060 */
extern unsigned char __cdecl fn_ucSetFieldDefaultFunction();
#define GAM_pfn_RetObjetSonoreREVERB        ((void *)&fn_ucSetFieldDefaultFunction)
#define GAM_pfn_RetObjetSonoreExtraCoefAnim ((void *)&RetObjetSonoreExtraCoefAnim)
/* 0x0040A180 */
long __cdecl RetObjetSonoreSwitchAnim(long indice, long toto);
#define GAM_pfn_RetObjetSonoreSwitchAnim    ((void *)&RetObjetSonoreSwitchAnim)

#define GAM_g_hMainCharacterSO_ML   GAM_pc_g_hMainCharacterSO

#define C_MSMicro_MinVolume   0

typedef struct SECT_tdstListOfSOSectorsInSoundInteraction_
{
	HIE_tdstSuperObject *hPointerOfSectorSO;
  long lVolume;
	LST2_M_StaticElementDeclaration(SECT_tdxHandleOfElementLstSoundInteraction)
} SECT_tdstListOfSOSectorsInSoundInteraction;

static __inline int SECT_fn_iGetNumberOfNodesInSoundInteractionList(HIE_tdstSuperObject *hSector)
{
	return LST2_M_StaticGetNumberOfElements(&((SECT_tdxHandleOfSectorObject)HIE_fn_hGetSuperObjectObject(hSector))->stListOfSectorsInSoundInteraction);
}

static __inline SECT_tdxHandleOfElementLstSoundInteraction SECT_fn_hGetFirstSoundNodeInSoundList(HIE_tdstSuperObject *_hSector)
{
  return(LST2_M_StaticGetFirstElement( &((SECT_tdxHandleOfSectorObject)HIE_fn_hGetSuperObjectObject(_hSector))->stListOfSectorsInSoundInteraction));
}

static __inline SECT_tdxHandleOfElementLstSoundInteraction SECT_fn_hGetNextSoundNodeInSoundList(SECT_tdxHandleOfElementLstSoundInteraction _hSoundNode)
{
  return(LST2_M_StaticGetNextElement( _hSoundNode));
}

#define SECT_M_ForEachSoundNodeInSoundInteractionList(_hSector,_hSoundList,_i) \
for \
( \
	_i=SECT_fn_iGetNumberOfNodesInSoundInteractionList(_hSector), \
	(_hSoundList)=SECT_fn_hGetFirstSoundNodeInSoundList(_hSector); \
	_i>0;  \
	_i--,(_hSoundList)=SECT_fn_hGetNextSoundNodeInSoundList(_hSoundList) \
)

static __inline HIE_tdstSuperObject *SECT_GetSectorInSoundList(SECT_tdxHandleOfElementLstSoundInteraction hSoundList)
    { return(hSoundList->hPointerOfSectorSO); }

static __inline long SCT_fn_lGetVolumeInSoundList(SECT_tdxHandleOfElementLstSoundInteraction hSoundList)
    { return(hSoundList->lVolume); }
#define GAM_pfn_RetObjetSonoreMicroLinkAnim ((void *)&RetObjetSonoreLinkAnim)

extern long __stdcall SND_pc_fn_bNop0(void);   /* 0x00454F30 */
#define GAM_pfn_RetObjetSonoreDefault       ((void *)&SND_pc_fn_bNop0)
#define GAM_pfn_RetObjetSonoreExtraCoefMusic ((void *)&RetObjetSonoreExtraCoefMusic)
extern int __stdcall SND_fn_bCanSetOptions(void);
#define GAM_pfn_SND_bCanSetOptions          ((void *)SND_fn_bCanSetOptions)
#define GAM_pfn_RetObjetInfoAnim            ((void *)&GAM_pc_RetObjetInfoAnim)
#define GAM_pfn_RetObjetInfoMusic           ((void *)&GAM_pc_RetObjetInfoMusic)
#define GAM_pfn_RetObjetInfoMenu            ((void *)&GAM_pc_RetObjetInfoMenu)

#define GAM_pfn_RetRollOffFactor            ((void *)&fn_bRetRollOffFactor)

#define GAM_C_NB_RET_OBJECT_SOUND_SLOTS 13

long RetObjetSonoreXMusic(long indice)
{
	return 0;
}

long RetObjetSonoreYMusic(long indice)
{
	return 0;
}

long RetObjetSonoreZMusic(long indice)
{
	return 0;
}

long RetObjetSonoreVXMusic(long indice)
{
	return 0;
}

long RetObjetSonoreVYMusic(long indice)
{
	return 0;
}

long RetObjetSonoreVZMusic(long indice)
{
	return 0;
}

long RetObjetSonoreSwitchMusic(long indice,long toto)
{
	return 0;
}

#define SIF_C_MATERIAL_INFO   1
#define SIF_C_NB_INFO_TYPES   2

#define GAM_C_WOT_ulGround    0x00000001UL
#define GAM_C_WOT_ulWall      0x00000004UL
#define GAM_C_WOT_ulCeiling   0x00000010UL

/* 0x004194B0 */
long __cdecl GMT_fn_hGetSoundMaterial(GMT_tdxHandleToGameMaterial _hGameMat);

/* 0x00409F50 */
long __cdecl RetObjetSonoreXAnim(void *hObj)
{
    MTH3D_tdstVector stPos;
    POS_fn_vGetTranslationVector(HIE_fn_hGetSuperObjectGlobalMatrix((HIE_tdstSuperObject *)hObj), &stPos);
    return (long)(stPos.xX * 65536.0f);
}
long __cdecl RetObjetSonoreYAnim(void *hObj)
{
    MTH3D_tdstVector stPos;
    POS_fn_vGetTranslationVector(HIE_fn_hGetSuperObjectGlobalMatrix((HIE_tdstSuperObject *)hObj), &stPos);
    return (long)(stPos.xY * 65536.0f);
}
long __cdecl RetObjetSonoreZAnim(void *hObj)
{
    MTH3D_tdstVector stPos;
    POS_fn_vGetTranslationVector(HIE_fn_hGetSuperObjectGlobalMatrix((HIE_tdstSuperObject *)hObj), &stPos);
    return (long)(stPos.xZ * 65536.0f);
}
/* 0x00409FE0 */
long __cdecl RetObjetSonoreVXAnim(void *indice)
{
    MTH3D_tdstVector stVertex;
    DNM_tdstDynamics *p_stDynamics;
    void *h_Dynam = M_GetMSHandle((HIE_tdstSuperObject *)indice, Dynam);

    if (h_Dynam == NULL)
        return 0;
    p_stDynamics = (DNM_tdstDynamics *)fn_p_stDynamGetDynamics((MS_tdxHandleToDynam)h_Dynam);
    if (p_stDynamics == NULL)
        return 0;
    *(unsigned long *)&stVertex.xX = *(unsigned long *)&DNM_M_pDynamicsGetPreviousSpeed(p_stDynamics)->xX;
    return (long)(stVertex.xX * 65536.0f);
}
long __cdecl RetObjetSonoreVYAnim(void *indice)
{
    MTH3D_tdstVector stVertex;
    DNM_tdstDynamics *p_stDynamics;
    void *h_Dynam = M_GetMSHandle((HIE_tdstSuperObject *)indice, Dynam);

    if (h_Dynam == NULL)
        return 0;
    p_stDynamics = (DNM_tdstDynamics *)fn_p_stDynamGetDynamics((MS_tdxHandleToDynam)h_Dynam);
    if (p_stDynamics == NULL)
        return 0;
    *(unsigned long *)&stVertex.xY = *(unsigned long *)&DNM_M_pDynamicsGetPreviousSpeed(p_stDynamics)->xY;
    return (long)(stVertex.xY * 65536.0f);
}
long __cdecl RetObjetSonoreVZAnim(void *indice)
{
    MTH3D_tdstVector stVertex;
    DNM_tdstDynamics *p_stDynamics;
    void *h_Dynam = M_GetMSHandle((HIE_tdstSuperObject *)indice, Dynam);

    if (h_Dynam == NULL)
        return 0;
    p_stDynamics = (DNM_tdstDynamics *)fn_p_stDynamGetDynamics((MS_tdxHandleToDynam)h_Dynam);
    if (p_stDynamics == NULL)
        return 0;
    *(unsigned long *)&stVertex.xZ = *(unsigned long *)&DNM_M_pDynamicsGetPreviousSpeed(p_stDynamics)->xZ;
    return (long)(stVertex.xZ * 65536.0f);
}
/* 0x0040A0A0 */
void __cdecl RetObjetSonoreExtraCoefAnim(long indice, SND_tduRefEvt evt, long *pitch, long *pan, long *vol)
{
    DNM_tdstDynamics *p_stDynamics;
    MTH3D_tdstVector stVertex;
    long rTmpPitch, rTmpVol, rTmpPan;

    rTmpPitch = 0;
    for (; pitch && rTmpPitch == 0;)
    {
        if (!M_GetMSHandle((HIE_tdstSuperObject *)indice, Dynam))
        {
            *pitch = 0;
            break;
        }
        if ((p_stDynamics = fn_p_stDynamGetDynamics(M_GetMSHandle((HIE_tdstSuperObject *)indice, Dynam))) == NULL)
        {
            *pitch = 0;
            break;
        }

        MTH3D_M_vCopyVector(&stVertex, DNM_M_pDynamicsGetPreviousSpeed(p_stDynamics));

        rTmpPitch = (long)((MTH3D_M_xNormVector(&stVertex)) * 65536.0F);

        if (rTmpPitch > ((long)((200.0)*0x10000)))
            rTmpPitch = ((long)((200.0)*0x10000));
        else
            if (rTmpPitch < ((long)((0.1)*0x10000)))
                rTmpPitch = ((long)((0.1)*0x10000));
        *pitch = rTmpPitch >> 7;
    }

    rTmpVol = 0;
    for (; vol && rTmpVol ==  0;)
    {

        rTmpVol = 0x10000;

        if (rTmpVol > ((long)((127.0)*0x10000)))
            rTmpVol = ((long)((127.0)*0x10000));
        else
            if (rTmpVol < 0)
                break;
        *vol = rTmpVol;
    }

    rTmpPan = 0;
    for (; pan && rTmpPan ==  0;)
    {

        rTmpPan = 0x10000;

        if (rTmpPan > ((long)((127.0)*0x10000)))
            rTmpPan = ((long)((127.0)*0x10000));
        else
            if (rTmpPan < 0)
                break;
        *pan = rTmpPan;
    }
}

/* 0x0040A180 */
long __cdecl RetObjetSonoreSwitchAnim(long indice, long toto)
{
    long eTypeId = toto;
    HIE_tdstSuperObject *hCharacter;

    hCharacter = (HIE_tdstSuperObject *)indice;
    switch (eTypeId)
    {

    case SIF_C_MATERIAL_INFO:
        {
            DNM_tdstReport   *p_stReport;
            DNM_tdstDynamics *p_stDynamics;
            unsigned long     ulSurfState;
            DNM_tdstObstacle *p_stObstacle;
            GMT_tdxHandleToGameMaterial hGMT;

            if (M_GetMSHandle(hCharacter, Dynam))
            {
                p_stDynamics = (DNM_tdstDynamics *)fn_p_stDynamGetDynamics(
                    M_GetMSHandle(hCharacter, Dynam));
                if (!p_stDynamics)
                    return 0;
                p_stReport = DNM_M_p_stDynamicsGetReport(p_stDynamics);
                ulSurfState = DNM_M_ulReportGetSurfaceState(p_stReport);

                if (DNM_M_bDynamicsIsGi(p_stDynamics) && (ulSurfState & GAM_C_WOT_ulCeiling))
                {
                    p_stObstacle = DNM_M_p_stReportGetCeil(p_stReport);
                    hGMT = DNM_M_hObstacleGetCollidedMaterial(p_stObstacle);
                    if (hGMT)
                        return (long)GMT_fn_hGetSoundMaterial(hGMT);
                    else
                        return 0;
                }

                else if (DNM_M_bDynamicsIsClimb(p_stDynamics) && (ulSurfState & GAM_C_WOT_ulWall))
                {
                    p_stObstacle = DNM_M_p_stReportGetWall(p_stReport);
                    hGMT = DNM_M_hObstacleGetCollidedMaterial(p_stObstacle);
                    if (hGMT)
                        return (long)GMT_fn_hGetSoundMaterial(hGMT);
                    else
                        return 0;
                }

                else if (ulSurfState & GAM_C_WOT_ulGround)
                {
                    p_stObstacle = DNM_M_p_stReportGetGround(p_stReport);
                    hGMT = DNM_M_hObstacleGetCollidedMaterial(p_stObstacle);
                    if (hGMT)
                        return (long)GMT_fn_hGetSoundMaterial(hGMT);
                    else
                        return 0;
                }
                else
                    return 0;
            }
            else
            {
                return 0;
            }
        }
        break;
    case SIF_C_NB_INFO_TYPES:
        return 0;
        break;
    default:
        return 0;
        break;
    }
}
/* 0x0040A1F0 */
long __cdecl RetObjetSonoreLinkAnim(long objet, long micro)
{
    HIE_tdstSuperObject *hObjectSector;
    HIE_tdstSuperObject *hSuperObj;
    HIE_tdstSuperObject *hMicroSector;
    SECT_tdxHandleOfElementLstSoundInteraction hSoundList;
    unsigned long ulI;

    hObjectSector = fn_h_SectInfoGetCurrentSector(
        M_GetMSHandle((HIE_tdstSuperObject *)objet, SectInfo));
    hSuperObj = M_GetSuperObject((struct tdstEngineObject_ *)fn_hReturnHandleMicroFromIndex(micro));

    if ((void *)objet == GAM_g_hMainCharacterSO_ML)
        return 1;

    hMicroSector = fn_h_SectInfoGetCurrentSector(
        M_GetMSHandle(hSuperObj, SectInfo));
    if (hObjectSector == hMicroSector)
        return 1;

    SECT_M_ForEachSoundNodeInSoundInteractionList(hMicroSector, hSoundList, ulI)
    {
        if (SECT_GetSectorInSoundList(hSoundList) == hObjectSector)
            return (SCT_fn_lGetVolumeInSoundList(hSoundList) != C_MSMicro_MinVolume);
    }
    return 1;
}
/* 0x0040A280 */
void __cdecl GAM_pc_RetObjetInfoAnim(void *hObj, char *pszBuffer, int iSize)
{
    struct tdstEngineObject_ *p_stEngine;

    if (hObj == (void *)-1)
    {
        strncpy(pszBuffer, "Ghost Object", iSize);
        return;
    }
    p_stEngine = (struct tdstEngineObject_ *)HIE_fn_hGetSuperObjectObject((HIE_tdstSuperObject *)hObj);
    strncpy(pszBuffer, "Perso:", iSize);
    if (((HIE_tdstSuperObject *)hObj)->ulTypeOfLinkedObject == HIE_C_ulActor)
    {
        if (!fn_ucIsAnAlwaysObject(p_stEngine))
        {
            if (p_stEngine->h_Brain != NULL)
                strncat(pszBuffer, "", iSize - strlen(pszBuffer));
            else
                strncat(pszBuffer, "no intelligence", iSize - strlen(pszBuffer));
        }
        else
        {
            strncat(pszBuffer, "Always", iSize - strlen(pszBuffer));
        }
    }
}
/* 0x0040A350 */
void __cdecl RetObjetSonoreExtraCoefMusic(long indice, SND_tduRefEvt evt,
                                          long *pitch, long *pan, long *vol)
{
    struct tdstEngineObject_ *p_stEngineMicro;
    MTH3D_tdstVector stMicroPosition;

    (void)indice; (void)evt; (void)pitch; (void)pan;
    if (vol == NULL)
        return;
    p_stEngineMicro = (struct tdstEngineObject_ *)fn_p_stGetFirstEngineObjectInMicroList();
    if (p_stEngineMicro != NULL && p_stEngineMicro->h_3dData != NULL)
    {
        struct HIE_tdstSuperObject *hSO = M_GetSuperObject(p_stEngineMicro);
        POS_fn_vGetTranslationVector(HIE_fn_hGetSuperObjectGlobalMatrix(hSO), &stMicroPosition);
    }
    *vol = 0x10000;
}

void __cdecl GAM_pc_RetObjetInfoMusic(void *hObj, char *pszBuffer, int iSize)
{
    strncpy(pszBuffer, "music ???", iSize);
}
void __cdecl GAM_pc_RetObjetInfoMenu(void *hObj, char *pszBuffer, int iSize)
{
    strncpy(pszBuffer, "menu ???", iSize);
}

/* 0x0040a3e0 */
void __cdecl fn_lInitSoundForAnim(void)
{
    tdstAllRetObjectSound st_pfnRetObjectSound;

    memset(&st_pfnRetObjectSound,0,sizeof(st_pfnRetObjectSound));
    st_pfnRetObjectSound.X = RetObjetSonoreXAnim;
    st_pfnRetObjectSound.Y = RetObjetSonoreYAnim;
    st_pfnRetObjectSound.Z = RetObjetSonoreZAnim;
    st_pfnRetObjectSound.VX = RetObjetSonoreVXAnim;
    st_pfnRetObjectSound.VY = RetObjetSonoreVYAnim;
    st_pfnRetObjectSound.VZ = RetObjetSonoreVZAnim;
    st_pfnRetObjectSound.REVERB = (td_pfn_ucRetSoundMidi)GAM_pfn_RetObjetSonoreREVERB;
    st_pfnRetObjectSound.ExtraCoef = RetObjetSonoreExtraCoefAnim;
    st_pfnRetObjectSound.Switch = RetObjetSonoreSwitchAnim;
    st_pfnRetObjectSound.MicroLink = RetObjetSonoreLinkAnim;

    GAM_g_lSoundVolumeLineSound=SND_fn_lAddVolumeLineSound();
    GAM_g_lSoundObjectTypeAnim = SND_fn_lAddObjectTypeSound2(&st_pfnRetObjectSound,GAM_g_lSoundVolumeLineSound);
    SND_fn_vSetInfoForTypeSound(GAM_g_lSoundObjectTypeAnim,GAM_pfn_RetObjetInfoAnim);

    memset(&st_pfnRetObjectSound,0,sizeof(st_pfnRetObjectSound));

    st_pfnRetObjectSound.X = RetObjetSonoreXMusic;
    st_pfnRetObjectSound.Y = RetObjetSonoreYMusic;
    st_pfnRetObjectSound.Z = RetObjetSonoreZMusic;
    st_pfnRetObjectSound.VX = RetObjetSonoreVXMusic;
    st_pfnRetObjectSound.VY = RetObjetSonoreVYMusic;
    st_pfnRetObjectSound.VZ = RetObjetSonoreVZMusic;
    st_pfnRetObjectSound.REVERB = (td_pfn_ucRetSoundMidi)GAM_pfn_RetObjetSonoreREVERB;
    st_pfnRetObjectSound.ExtraCoef = RetObjetSonoreExtraCoefMusic;
    st_pfnRetObjectSound.Switch = RetObjetSonoreSwitchMusic;
    st_pfnRetObjectSound.MicroLink = (td_pfn_bRetSoundLink)GAM_pfn_SND_bCanSetOptions;

    GAM_g_lSoundVolumeLineMusic=SND_fn_lAddVolumeLineSound();
    GAM_g_lSoundObjectTypeMusic = SND_fn_lAddObjectTypeSound2(&st_pfnRetObjectSound,GAM_g_lSoundVolumeLineMusic);
    SND_fn_vSetInfoForTypeSound(GAM_g_lSoundObjectTypeMusic,GAM_pfn_RetObjetInfoMusic);

    memset(&st_pfnRetObjectSound,0,sizeof(st_pfnRetObjectSound));

    st_pfnRetObjectSound.X = RetObjetSonoreXMusic;
    st_pfnRetObjectSound.Y = RetObjetSonoreYMusic;
    st_pfnRetObjectSound.Z = RetObjetSonoreZMusic;
    st_pfnRetObjectSound.VX = RetObjetSonoreVXMusic;
    st_pfnRetObjectSound.VY = RetObjetSonoreVYMusic;
    st_pfnRetObjectSound.VZ = RetObjetSonoreVZMusic;
    st_pfnRetObjectSound.REVERB = (td_pfn_ucRetSoundMidi)GAM_pfn_RetObjetSonoreREVERB;
    st_pfnRetObjectSound.ExtraCoef = RetObjetSonoreExtraCoefMusic;
    st_pfnRetObjectSound.Switch = RetObjetSonoreSwitchMusic;
    st_pfnRetObjectSound.MicroLink = (td_pfn_bRetSoundLink)GAM_pfn_SND_bCanSetOptions;

    GAM_g_lSoundObjectTypeMenu = SND_fn_lAddObjectTypeSound2(&st_pfnRetObjectSound,GAM_g_lSoundVolumeLineSound);
    SND_fn_vSetInfoForTypeSound(GAM_g_lSoundObjectTypeMenu,GAM_pfn_RetObjetInfoMenu);

    SND_fn_vSetRollOffFunctionForTypeSound(GAM_g_lSoundObjectTypeAnim,GAM_pfn_RetRollOffFactor);

    memset (GAM_ga_pstBlockEventSlotMemory , 0 ,GAM_C_NBLOCKEVENTSLOTNUMBER * sizeof(void *));
}

/* 0x0040A5A0 */
void __cdecl GAM_fn_lSaveBlockEventSlotMemory(void *_p_stBlockEvent, unsigned long _ulIndex)
{
    GAM_ga_pstBlockEventSlotMemory[_ulIndex] = _p_stBlockEvent;
}

/* 0x0040A5B0 */
void *__cdecl GAM_p_stGetBlockEventSlotMemory(unsigned long _ulIndex)
{
    return GAM_ga_pstBlockEventSlotMemory[_ulIndex];
}
