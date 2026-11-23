#ifndef RAYMAN2_GAM_TEXTFRMT_H
#define RAYMAN2_GAM_TEXTFRMT_H

#include "GAM/GAM_types.h"

typedef struct TFMT_tdstStatus_ {
    MTH_tdxReal xScreenWidth;
    MTH_tdxReal xBaseCharX;
    MTH_tdxReal xBaseCharZ;
    MTH_tdxReal xBaseToAddX;
    MTH_tdxReal xRandomPosRadius;
    MTH_tdxReal xCharWidth;
    MTH_tdxReal xLineHeight;
    MTH_tdxReal xCharZoom;
    MTH_tdxReal xCurCharZoom;
    MTH_tdxReal xCurCharWidth;
    long        hSourceObjectTable;
    char        cJustificationMode;
} TFMT_tdstStatus;

typedef struct TFMT_tdstLineResults_ {
    short wStartChar;
    short wEndChar;
    float a_xLineOrigin[3];
} TFMT_tdstLineResults;

void  __cdecl TFMT_vInitFormatterStatus(TFMT_tdstStatus *_p_stStatus,
                                        HIE_tdstSuperObject *_hSuperObjPerso,
                                        MTH_tdxReal _xScreenWidth);

#ifndef AI_RT_TFMT_wPositionModulesAccordingToText
short __cdecl TFMT_wPositionModulesAccordingToText(
    TFMT_tdstStatus *_p_stStatus, HIE_tdstSuperObject *_hSuperObjPerso,
    char *_pszText, long _lStart, long _lEnd, MTH_tdxReal _xScreenWidth,
    TFMT_tdstLineResults *_d_stLineResults, short _wNbLines);
#endif

/* 0x00408590 */
void __cdecl TFMT_vSetDefaultValuesCharacter(
    MTH_tdxReal _xCharWidth, MTH_tdxReal _xLineHeight, MTH_tdxReal _xCharZoom);

/* 0x00409B00 */
void __cdecl TFMT_vInitScapingTable(MTH_tdxReal _xValue);

/* 0x00409910 */
void TFMT_vChangeLetter(HIE_tdstSuperObject *hSuperObjPerso, unsigned char ucPosLetter,
                        unsigned char ucLetter);

/* 0x00409A20 */
unsigned char TFMT_ucNumberOfCurrentTextAffiche(HIE_tdstSuperObject *p_SuperObjPersoTreated);

/* 0x00409B20 */
void TFMT_vUpdateSpacingTable(char *szText);

/* 0x00409BB0 */
void TFMT_vUpdateScaleTable(char *szText);

#endif
