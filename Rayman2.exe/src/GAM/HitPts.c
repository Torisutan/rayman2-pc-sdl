#include "GAM/HitPts.h"
#include "GAM/GAM_types.h"
#include "GAM/Actions/StdGame.h"

#define HP_M_pStdGame(o)   (((struct tdstEngineObject_ *)(o))->h_StandardGame)
#define HP_M_ucHitPoints(o)     (HP_M_pStdGame(o)->ucHitPoints)

/* 0x004949A0 */
unsigned char __cdecl fn_ucGetHitPoints(void *p_stObject)
{
    return HP_M_ucHitPoints(p_stObject);
}
#define HP_M_ucHitPointsMax(o)  (HP_M_pStdGame(o)->ucHitPointsMax)
#define HP_M_ucHitPointsMaxMax(o)(HP_M_pStdGame(o)->ucHitPointsMaxMax)
#define HP_M_ucSaveHitPoints(o) (HP_M_pStdGame(o)->ucSaveHitPoints)

/* 0x004949B0 */
void __cdecl fn_vSetHitPointsInit(void *p_stObject, unsigned char ucValue)
{
    HP_M_ucSaveHitPoints(p_stObject) = ucValue;
}

/* 0x004949C0 */
void __cdecl fn_vSetHitPoints(void *p_stObject, unsigned char ucValue)
{
    HP_M_ucHitPoints(p_stObject) = ucValue;
}

/* 0x004949D0 */
void __cdecl fn_vSetHitPointsToMaxValue(void *p_stObject)
{
    HP_M_ucHitPoints(p_stObject) = HP_M_ucHitPointsMax(p_stObject);
}

/* 0x004949E0 */
void __cdecl fn_vAddHitPoints(void *p_stObject, unsigned char ucValue)
{
    if (HP_M_ucHitPoints(p_stObject) + ucValue <= HP_M_ucHitPointsMax(p_stObject))
        HP_M_ucHitPoints(p_stObject) = (unsigned char)(HP_M_ucHitPoints(p_stObject) + ucValue);
    else
        HP_M_ucHitPoints(p_stObject) = HP_M_ucHitPointsMax(p_stObject);
}

/* 0x00494A30 */
unsigned char __cdecl fn_ucSubHitPoints(void *p_stObject, unsigned char ucValue)
{
    if ((long)HP_M_ucHitPoints(p_stObject) - (long)ucValue > 0)
        HP_M_ucHitPoints(p_stObject) = (unsigned char)(HP_M_ucHitPoints(p_stObject) - ucValue);
    else
        HP_M_ucHitPoints(p_stObject) = 0;
    return HP_M_ucHitPoints(p_stObject);
}

/* 0x00494A80 */
unsigned char __cdecl fn_ucGetHitPointsMax(void *p_stObject)
{
    return HP_M_ucHitPointsMax(p_stObject);
}

/* 0x00494A90 */
void __cdecl fn_vSetHitPointsMax(void *p_stObject, unsigned char ucValue)
{
    HP_M_ucHitPointsMax(p_stObject) = ucValue;
}

/* 0x00494AA0 */
void __cdecl fn_vSetHitPointsMaxToMaxValue(void *p_stObject)
{
    HP_M_ucHitPointsMax(p_stObject) = HP_M_ucHitPointsMaxMax(p_stObject);
}

/* 0x00494AB0 */
void __cdecl fn_vAddHitPointsMax(struct tdstEngineObject_ *_p_stObject,unsigned char _ucValue)
{
 if (fn_ucStandardGameGetHitPointsMax(_p_stObject->h_StandardGame)+_ucValue<=fn_ucStandardGameGetHitPointsMaxMax(_p_stObject->h_StandardGame))
  fn_vStandardGameSetHitPointsMax(_p_stObject->h_StandardGame,(unsigned char)(fn_ucStandardGameGetHitPointsMax(_p_stObject->h_StandardGame)+_ucValue));
 else
  fn_vStandardGameSetHitPointsMax(_p_stObject->h_StandardGame,fn_ucStandardGameGetHitPointsMaxMax(_p_stObject->h_StandardGame));
}

/* 0x00494B00 */
unsigned char __cdecl fn_ucSubHitPointsMax(void *p_stObject, unsigned char ucValue)
{
    if ((long)HP_M_ucHitPointsMax(p_stObject) - (long)ucValue > 0)
        HP_M_ucHitPointsMax(p_stObject) = (unsigned char)(HP_M_ucHitPointsMax(p_stObject) - ucValue);
    else
        HP_M_ucHitPointsMax(p_stObject) = 0;
    fn_vAddHitPoints(p_stObject, 0);
    return HP_M_ucHitPointsMax(p_stObject);
}
