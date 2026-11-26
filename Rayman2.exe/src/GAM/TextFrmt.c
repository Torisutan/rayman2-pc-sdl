#include "GAM/GAM_types.h"
#include "GAM/TextFrmt.h"
#include <string.h>
#include <stdlib.h>
#include "GAM/GAM_globals.h"
#include "POS/POS_types.h"
#include "RND/RND.h"
#include "GAM/ObjsTbls.h"
#include "GAM/Family.h"
#include "GAM/PlayAnim.h"
#include "SPO/HieDef.h"
#include "MTH/MTH_xNeg.h"

#define TFMT_C_MAX_TEXTAFFICHE          30
#define TFMT_C_sizeofInfoTextAffiche    0x14

HIE_tdstSuperObject *TFMT_gp_SuperObjPersoGenerated_storage[TFMT_C_MAX_TEXTAFFICHE];
unsigned long        TFMT_g_bFreeTextAfficheBlocs_storage[TFMT_C_MAX_TEXTAFFICHE];
unsigned char        TFMT_g_stInfoTextAffiche_storage[TFMT_C_MAX_TEXTAFFICHE * TFMT_C_sizeofInfoTextAffiche];
#define TFMT_gp_SuperObjPersoGenerated  (TFMT_gp_SuperObjPersoGenerated_storage)
#define TFMT_g_bFreeTextAfficheBlocs    (TFMT_g_bFreeTextAfficheBlocs_storage)
#define TFMT_g_stInfoTextAffiche        (TFMT_g_stInfoTextAffiche_storage)

/* 0x00409A20 */
unsigned char TFMT_ucNumberOfCurrentTextAffiche( HIE_tdstSuperObject *p_SuperObjPersoTreated )
{
    register unsigned char i;
    for (i = 0; i < TFMT_C_MAX_TEXTAFFICHE; i++)
    {
        if (p_SuperObjPersoTreated == TFMT_gp_SuperObjPersoGenerated[i])
        {
            return i;
        }
    }

    return i ;
}

/* 0x00409A50 */
unsigned char __cdecl TFMT_ucSetNumber4CurrentTextAffiche(void)
{
    register unsigned char i;
    for (i = 0; i < TFMT_C_MAX_TEXTAFFICHE; i++)
    {
        if (TFMT_g_bFreeTextAfficheBlocs[i])
        {
            TFMT_g_bFreeTextAfficheBlocs[i] = FALSE;
            return i;
        }
    }

    return i;
}

/* 0x00409A90 */
void __cdecl TFMT_ucClearNumberOfCurrentTextAffiche(HIE_tdstSuperObject *p_SuperObjPersoTreated)
{
    unsigned char i;

    for (i = 0; i < TFMT_C_MAX_TEXTAFFICHE; i++)
    {
        if (p_SuperObjPersoTreated == TFMT_gp_SuperObjPersoGenerated[i])
        {
            TFMT_g_bFreeTextAfficheBlocs[i] = 1  ;
            TFMT_gp_SuperObjPersoGenerated[i] = NULL;
            memset(TFMT_g_stInfoTextAffiche + i * TFMT_C_sizeofInfoTextAffiche, 0x00,
                   TFMT_C_sizeofInfoTextAffiche);
            *(unsigned long *)(TFMT_g_stInfoTextAffiche + i * TFMT_C_sizeofInfoTextAffiche) =
                1  ;
            break;
        }
    }
}

static unsigned long TFMT_gs_hRandomHandleForTFMT_storage = 0xFFFFFFFFu;
static long  TFMT_g_lOffsetObjectTable_storage = 0;
static float TFMT_g_xDefaultCharWidth_storage  = 0.0f;
static float TFMT_g_xDefaultLineHeight_storage = 2.0f;
static float TFMT_g_xDefaultCharZoom_storage   = 0.0f;
static const unsigned long TFMT_C_xMmToUnit_bits = 0x3A83126Fu;
static const unsigned long TFMT_C_xEpsilon_bits  = 0x38D1B717u;
#define TFMT_gs_hRandomHandleForTFMT  (TFMT_gs_hRandomHandleForTFMT_storage)
#define TFMT_g_lOffsetObjectTable     (TFMT_g_lOffsetObjectTable_storage)
#define TFMT_g_xDefaultCharWidth      (TFMT_g_xDefaultCharWidth_storage)
#define TFMT_g_xDefaultLineHeight     (TFMT_g_xDefaultLineHeight_storage)
#define TFMT_g_xDefaultCharZoom       (TFMT_g_xDefaultCharZoom_storage)
#define TFMT_C_xMmToUnit              (*(const float *)&TFMT_C_xMmToUnit_bits)
#define TFMT_C_xZero                  (0.0f)
#define TFMT_C_xOne                   (1.0f)
#define TFMT_C_xHalf                  (0.5f)
#define TFMT_C_xEpsilon               (*(const float *)&TFMT_C_xEpsilon_bits)

float TFMT_a_xLetterWidth_storage[256];
#define TFMT_a_xLetterWidth           (TFMT_a_xLetterWidth_storage)

#define TFMT_RT_lReserveRandomHandle    RND_fn_hReserveANewHandleIndex
/* 0x00449BE0 */
extern void RDN_fn_vSetRandomSeed(unsigned long _lIndexNumber, long _lNewSeed);
/* 0x00419460 */
extern unsigned short __cdecl fn_uwGetObjectTableNumberOfelements(tdxHandleToObjectsTablesList _hTargetTable);
#define TFMT_RT_uwGetObjTableCount(h) ((short)fn_uwGetObjectTableNumberOfelements((void *)(h)))

#define TFMT_RT_vCopyObjectBetweenTables(hTgt, hSrc, uwTgt, uwSrc) \
    fn_vCopyObjectFromObjectTabletoAnother((tdxHandleToObjectsTablesList)(hTgt), \
        (tdxHandleToObjectsTablesList)(hSrc), (unsigned short)(uwTgt), (unsigned short)(uwSrc))

#define TFMT_RT_vSetTranslationVector POS_fn_vSetTranslationVector

static __inline unsigned char fn_ucToTableElement ( int _n )
{
	if ( _n < ' ' )
		_n = 0 ;
	else if ( _n >= 'a' && _n <= 'z' )
		_n -= ' ' + ' ' ;
	else if ( _n >= 'A' && _n <= 'Z' )
		_n -= ' ' ;
	else if ( _n >= 160 )
		_n -= ' ' + ' ' ;
	else
		_n -= ' ' ;

	return ( unsigned char ) _n ;
}

static __inline tdxHandleToFamilyList fn_h3dDataGetFamily(MS_tdxHandleTo3dData h_3dData)
{
	return(h_3dData->h_Family);
}

static __inline HIE_tdstSuperObject *fn_hGetSuperObjectInChannel(CHN_tdxHandleToChannelArray hChannelArray, unsigned short uwChannelNumber)
{
	return (hChannelArray) ? hChannelArray[uwChannelNumber].hSupObject : NULL;
}

/* 0x00409910 */
void TFMT_vChangeLetter
(
	HIE_tdstSuperObject *hSuperObjPerso ,
	unsigned char ucPosLetter ,
	unsigned char ucLetter
)
{
	MS_tdxHandleTo3dData              h_Current3dData          = NULL ;
	CHN_tdxHandleToChannelArray       hChannelArray            = NULL ;
	tdstAChannel                      *p_stChannel             = NULL ;
	struct tdstObjectsTableElement_   * d_stCurrentObjectsTable = NULL ;

	short wSourceObject = 0 , wNbObjectsInSourceTable = 0 ;

	h_Current3dData         = M_GetMSHandle(hSuperObjPerso,3dData);
	d_stCurrentObjectsTable = fn_h3dDataGetCurrentObjectsTable(h_Current3dData)->d_stObjectsTable;
	hChannelArray           = h_Current3dData->hArrayOfChannels;

	wNbObjectsInSourceTable = fn_uwGetObjectTableNumberOfelements( fn_hGetObjectsTableOfFamilyByIndex( fn_h3dDataGetFamily(h_Current3dData), 0 ) );

	if ( ucLetter < ' ' )
	{
		wSourceObject = -1 ;
	}
	else
	{
		unsigned char ucTemp = fn_ucToTableElement ( ucLetter ) ;
		wSourceObject = (short) ( ucTemp + TFMT_g_lOffsetObjectTable ) ;
	}

	if ( ( wSourceObject < 0 ) || ( wSourceObject >= wNbObjectsInSourceTable ) )
	{
		wSourceObject = 0 ;
	}
	else
	{

		p_stChannel = &hChannelArray[ucPosLetter];

		HIE_M_vSetSuperObjectMember(p_stChannel->hSupObject, ulTypeOfLinkedObject, (d_stCurrentObjectsTable+wSourceObject)->h_Target ? HIE_C_ulPO : HIE_C_ulSuperObject);
		HIE_M_vSetSuperObjectMember(p_stChannel->hSupObject, hLinkedObject.p_Void, (d_stCurrentObjectsTable+wSourceObject)->h_Target);
	}
}

#define TFMT_C_MAX_WIDTHTABLEELEMENT  256

/* 0x004099D0 */
void __cdecl TFMT_vInitTextAfficheStructure(void)
{
    register int iCpt = 0;
    register int iSizeOfStructure = TFMT_C_sizeofInfoTextAffiche;

    for (; iCpt < TFMT_C_MAX_TEXTAFFICHE; iCpt++)
    {
        memset(TFMT_g_stInfoTextAffiche + iCpt * TFMT_C_sizeofInfoTextAffiche, 0x00, iSizeOfStructure);
        *(unsigned long *)(TFMT_g_stInfoTextAffiche + iCpt * TFMT_C_sizeofInfoTextAffiche) = 1;
        TFMT_g_bFreeTextAfficheBlocs[iCpt] = 1;
        TFMT_gp_SuperObjPersoGenerated[iCpt] = NULL;
    }
}

/* 0x00409B00 */
void __cdecl TFMT_vInitScapingTable(MTH_tdxReal _xValue)
{
    long i;

    for (i = 0; i < TFMT_C_MAX_WIDTHTABLEELEMENT; i++)
        TFMT_a_xLetterWidth[i] = _xValue;
}

MTH_tdxReal TFMT_ga_fLetterScale[TFMT_C_MAX_WIDTHTABLEELEMENT] ;
unsigned long TFMT_ga_ulNbScale = 0 ;

/* 0x00409B20 */
void TFMT_vUpdateSpacingTable ( char *szText ) {
  char *szCur ;
  int i ;

  szCur = strtok (szText, ":") ;
  TFMT_a_xLetterWidth[0] = MTH_M_xDiv(MTH_M_xLongToReal(atol ( szCur )), MTH_M_xLongToReal(1000)) ;

  i=1 ;
  while ( szCur != NULL && i<TFMT_C_MAX_WIDTHTABLEELEMENT ) {
    szCur = strtok (NULL, ":") ;
    if ( szCur != NULL ) {

      TFMT_a_xLetterWidth[i] = MTH_M_xDiv(MTH_M_xLongToReal(atol ( szCur )), MTH_M_xLongToReal(1000)) ;
      i++;

      *(szCur-1) = ':' ;
    }
  }
}

/* 0x00409BB0 */
void TFMT_vUpdateScaleTable ( char *szText ) {
  char *szCur ;
  int i ;

  szCur = strtok (szText, ":") ;
  TFMT_ga_fLetterScale[0] = MTH_M_xDiv(MTH_M_xLongToReal(atol ( szCur )), MTH_M_xLongToReal(1000)) ;
  i=1 ;
  while ( szCur != NULL && i<TFMT_C_MAX_WIDTHTABLEELEMENT ) {
    szCur = strtok (NULL, ":") ;
    if ( szCur != NULL ) {
      TFMT_ga_fLetterScale[i] = MTH_M_xDiv(MTH_M_xLongToReal(atol ( szCur )), MTH_M_xLongToReal(1000)) ;
    i++;
      *(szCur-1) = ':' ;
    }
  }
  TFMT_ga_ulNbScale = i ;
}

/* 0x00408590 */
void __cdecl TFMT_vSetDefaultValuesCharacter(
    MTH_tdxReal _xCharWidth, MTH_tdxReal _xLineHeight, MTH_tdxReal _xCharZoom)
{
    TFMT_g_xDefaultCharWidth = TFMT_C_xZero;
    if (_xLineHeight >= TFMT_C_xZero)
        TFMT_g_xDefaultLineHeight = _xLineHeight;
    if (_xCharZoom >= TFMT_C_xZero)
        TFMT_g_xDefaultCharZoom = _xCharZoom;
    TFMT_vInitScapingTable(_xCharWidth);
}

/* 0x004085E0 */
void __cdecl TFMT_vInitFormatterStatus(TFMT_tdstStatus *_p_stStatus,
                                       HIE_tdstSuperObject *_hSuperObjPerso,
                                       MTH_tdxReal _xScreenWidth)
{
    TFMT_g_lOffsetObjectTable = 0;
    if (TFMT_gs_hRandomHandleForTFMT == 0xFFFFFFFF)
        TFMT_gs_hRandomHandleForTFMT = (unsigned long)TFMT_RT_lReserveRandomHandle();

    _p_stStatus->xScreenWidth = _xScreenWidth;
    _p_stStatus->xBaseCharX = _p_stStatus->xBaseCharZ = TFMT_C_xZero;
    _p_stStatus->xBaseToAddX = TFMT_C_xZero;
    _p_stStatus->xCharWidth = TFMT_g_xDefaultCharWidth;
    _p_stStatus->xLineHeight = TFMT_g_xDefaultLineHeight;
    _p_stStatus->xCharZoom = TFMT_g_xDefaultCharZoom;
    _p_stStatus->xCurCharZoom = TFMT_C_xZero;
    _p_stStatus->cJustificationMode = 'l';
    _p_stStatus->hSourceObjectTable = fn_hGetObjectsTableOfFamilyByIndex(
        fn_h3dDataGetFamily(M_GetMSHandle(_hSuperObjPerso,3dData)), 0);
    _p_stStatus->xRandomPosRadius = TFMT_C_xZero;
}

/* 0x00408660 */
static __inline MTH_tdxReal GetLetterSpacing(unsigned char ucChar);
static char *__cdecl TFMT_pszReadNextChar(TFMT_tdstStatus *_p_stStatus,
                                          HIE_tdstSuperObject *_hSuperObjPerso,
                                          char *_pszCurrentChar,
                                          unsigned char *_p_ucReturnedChar,
                                          char *_p_bLineBreak)
{
    unsigned char ucChar ;

    * _p_bLineBreak = FALSE ;
    if ( _pszCurrentChar && ( ( ucChar = * ( _pszCurrentChar ++ ) ) != 0 ) )
    {

        while ( ucChar == '/' )
        {
            switch ( *_pszCurrentChar )
            {
            case 'L':
            case 'l':
                *_p_bLineBreak = TRUE;
                _p_stStatus->xBaseCharX = _p_stStatus->xBaseToAddX = MTH_C_ZERO;
                _p_stStatus->xBaseCharZ -= _p_stStatus->xLineHeight;
                break;

            case 'Z':
            case 'z':
                _pszCurrentChar ++;
                _p_stStatus->xCharZoom = MTH_M_xDiv(MTH_M_xLongToReal(atoi(_pszCurrentChar)), MTH_M_xLongToReal(1000));
                break;

            case 'W':
            case 'w':
                _pszCurrentChar ++;
                _p_stStatus->xCharWidth = MTH_M_xMul(_p_stStatus->xCharZoom, MTH_M_xDiv(MTH_M_xLongToReal(atoi(_pszCurrentChar)), MTH_M_xLongToReal(1000)));
                break;

            case 'H':
            case 'h':
                _pszCurrentChar ++;
                _p_stStatus->xLineHeight = MTH_M_xMul(_p_stStatus->xCharZoom, MTH_M_xDiv(MTH_M_xLongToReal(atoi(_pszCurrentChar)), MTH_M_xLongToReal(1000)));
                break;

            case 'C':
            case 'c':
                _p_stStatus->hSourceObjectTable = fn_hGetObjectsTableOfFamilyByIndex(
                    fn_h3dDataGetFamily(M_GetMSHandle(_hSuperObjPerso,3dData)),
                    (ACP_tdxIndex) atoi(++ _pszCurrentChar)
                );
                break;

            case 'o':
            case 'O':
                _pszCurrentChar ++;
                TFMT_g_lOffsetObjectTable = atoi(_pszCurrentChar);
                break;

            case 'J':
            case 'j':
                _p_stStatus->cJustificationMode = *(++ _pszCurrentChar);
                break;

            case 'R':
            case 'r':
                {
                    long lValue = atoi(++ _pszCurrentChar);
                    _p_stStatus->xRandomPosRadius = MTH_M_xDiv(MTH_M_xLongToReal(lValue), MTH_M_xLongToReal(1000));
                    /* 0x00449BE0 */
                    RDN_fn_vSetRandomSeed(TFMT_gs_hRandomHandleForTFMT, (long)lValue);
                }
                break;

            case '/':
                ucChar = '/';
                _pszCurrentChar ++;
                break;

            case ':':
                ucChar = ':';
                _pszCurrentChar ++;
                break;
            }

            while ( *(++ _pszCurrentChar) != ':' ) {}
            ucChar = *(++ _pszCurrentChar);

            if ( ! *_p_bLineBreak )
                _pszCurrentChar ++;
        }

        if ( (_p_stStatus->xBaseCharX == MTH_C_ZERO) && (ucChar == ' ') )
            while ( (ucChar = *(_pszCurrentChar ++)) == ' ' ) {}

        _p_stStatus->xBaseCharX += _p_stStatus->xBaseToAddX;

        if ( ! *_p_bLineBreak )
        {
            if ( _p_stStatus->xCharWidth )
            {
                _p_stStatus->xBaseToAddX = _p_stStatus->xCharWidth;
            }
            else
            {
                MTH_tdxReal xZoom ;
                unsigned char ucTemp ;

                xZoom = _p_stStatus->xCharZoom ;
                if ( MTH_M_bEqualZero (xZoom) ) {
                    if ( TFMT_ga_ulNbScale )
                        xZoom = TFMT_ga_fLetterScale[0] ;
                    else
                        xZoom = MTH_C_ONE ;
                }
                ucTemp = fn_ucToTableElement ( ucChar ) ;
                _p_stStatus->xBaseToAddX = MTH_M_xMul ( xZoom , GetLetterSpacing(ucTemp) ) ;
            }
        }
    }
    else
    {

        *_p_bLineBreak = TRUE;
        *_p_ucReturnedChar = '\0' ;
        return NULL;
    }
    *_p_ucReturnedChar = ucChar;
    return _pszCurrentChar;
}

/* 0x00408960 */
static unsigned short __cdecl TFMT_uwGetNbCharsFillingALine(TFMT_tdstStatus *_p_stStatus,
                                                   HIE_tdstSuperObject *_hSuperObjPerso,
                                                   char *_pszText, float *_p_xActualWidth)
{
    TFMT_tdstStatus stTfmtStatusSave = *_p_stStatus;
    unsigned char ucCurrentChar;
    short wCharIndex = 0;
    char bLineBreak;
    MTH_tdxReal xCurrentWidth = MTH_C_ZERO, xScreenWidth = _p_stStatus->xScreenWidth;

    if ( MTH_M_bLessZero(xScreenWidth) )
        xScreenWidth = MTH_M_xLongToReal(1000);

    do
    {

        _pszText = TFMT_pszReadNextChar(_p_stStatus, _hSuperObjPerso, _pszText, & ucCurrentChar, &bLineBreak);
        if ( !bLineBreak )
        {
            MTH_tdxReal xNewWidth = xCurrentWidth + _p_stStatus->xCharWidth;
            if ( xNewWidth < xScreenWidth )
            {
                xCurrentWidth = xNewWidth;
                wCharIndex ++;
            }
            else
                bLineBreak = 1;
        }
    } while ( !bLineBreak );

    *_p_stStatus = stTfmtStatusSave;

    *_p_xActualWidth = xCurrentWidth;

    return wCharIndex;
}

/* 0x00408A00 */
short __cdecl TFMT_wPositionModulesAccordingToText(
    TFMT_tdstStatus *_p_stStatus, HIE_tdstSuperObject *_hSuperObjPerso,
    char *_pszText, long _lStart, long _lEnd, MTH_tdxReal _xScreenWidth,
    TFMT_tdstLineResults *_d_stLineResults, short _wNbLines)
{
    MTH_tdxReal xJustificationShift;
    short wNbCharsInCurrentLine, wCurrentChar = 0, wCurrentLine = 0;
    tdxHandleToObjectsTablesList hTargetObjectTable = fn_h3dDataGetCurrentObjectsTable(M_GetMSHandle(_hSuperObjPerso,3dData));
    short wNbObjectsInTargetTable = TFMT_RT_uwGetObjTableCount(hTargetObjectTable);

    char *pszInitText = _pszText, *pszEndText = _pszText + (_lEnd >= 0 ? _lEnd : strlen(_pszText));

    _pszText += _lStart;

    TFMT_vInitFormatterStatus(_p_stStatus, _hSuperObjPerso, _xScreenWidth);

    while ( (_pszText < pszEndText) && *_pszText )
    {

        wNbCharsInCurrentLine = TFMT_uwGetNbCharsFillingALine(_p_stStatus, _hSuperObjPerso, _pszText, &xJustificationShift);

        switch ( _p_stStatus->cJustificationMode )
        {
        case 'r':
            xJustificationShift = MTH_M_xSub(_p_stStatus->xScreenWidth, xJustificationShift);
            break;

        case 'c':
            xJustificationShift = MTH_M_xDiv(MTH_M_xSub(_p_stStatus->xScreenWidth, xJustificationShift), MTH_C_2);
            break;

        default:
            xJustificationShift = MTH_C_ZERO;
        }

        if ( _d_stLineResults && (_wNbLines >= 0) )
            _d_stLineResults[wCurrentLine].wStartChar = (short) (_pszText - pszInitText);

        while ( (wNbCharsInCurrentLine --) && (_pszText < pszEndText) )
        {
            unsigned char ucChar;
            char bLineBreak;
            short wSourceObject, wNbObjectsInSourceTable;

            _pszText = TFMT_pszReadNextChar ( _p_stStatus , _hSuperObjPerso , _pszText , & ucChar , & bLineBreak ) ;

            if ( ! _d_stLineResults )
            {
                if ( ucChar < ' ' )
                {
                    wSourceObject = -1 ;
                }
                else
                {
                    unsigned char ucTemp = fn_ucToTableElement ( ucChar ) ;
                    wSourceObject = (short) ( ucTemp + TFMT_g_lOffsetObjectTable ) ;
                }

                wNbObjectsInSourceTable = TFMT_RT_uwGetObjTableCount(_p_stStatus->hSourceObjectTable);
                if ( wSourceObject < 0 || wSourceObject >= wNbObjectsInSourceTable )
                {
                    wSourceObject = 0;
                }
                else
                {
                    POS_tdstCompletePosition *hChannelLocalMatrix;
                    HIE_tdstSuperObject *hChannelSuperObject;

                    TFMT_RT_vCopyObjectBetweenTables(hTargetObjectTable, _p_stStatus->hSourceObjectTable, wCurrentChar, wSourceObject);

                    hChannelSuperObject = fn_hGetSuperObjectInChannel(fn_h3dDataGetChannelSOList(M_GetMSHandle(_hSuperObjPerso,3dData)), wCurrentChar);
                    if ( hChannelSuperObject )
                    {
                        MTH3D_tdstVector stV1, stV2, stV3;

                        hChannelLocalMatrix = HIE_fn_hGetSuperObjectMatrix(hChannelSuperObject);

                        MTH3D_M_vSetVectorElements(
                            &stV1,
                            MTH_M_xAdd(xJustificationShift, MTH_M_xAdd(_p_stStatus->xBaseCharX, RND_fn_xGetRealRandomValue(TFMT_gs_hRandomHandleForTFMT, MTH_M_xNeg(_p_stStatus->xRandomPosRadius), _p_stStatus->xRandomPosRadius))),
                            MTH_C_ZERO,
                            MTH_M_xAdd(_p_stStatus->xBaseCharZ, RND_fn_xGetRealRandomValue(TFMT_gs_hRandomHandleForTFMT, MTH_M_xNeg(_p_stStatus->xRandomPosRadius), _p_stStatus->xRandomPosRadius))
                        );
                        POS_fn_vSetTranslationVector(hChannelLocalMatrix, &stV1);

                        if ( MTH_M_bDifferentWithEpsilon(_p_stStatus->xCharZoom, MTH_C_ONE, TFMT_C_xEpsilon) )
                        {
                            MTH3D_M_vSetVectorElements(&stV1,	_p_stStatus->xCharZoom, MTH_C_ZERO, MTH_C_ZERO);
                            MTH3D_M_vSetVectorElements(&stV2,	MTH_C_ZERO, _p_stStatus->xCharZoom, MTH_C_ZERO);
                            MTH3D_M_vSetVectorElements(&stV3, MTH_C_ZERO, MTH_C_ZERO, _p_stStatus->xCharZoom);
                            POS_fn_vSetScaleMatrix(hChannelLocalMatrix, &stV1, &stV2, &stV3);
                        }
                    }
                }
            }
            wCurrentChar ++;
        }

        if ( _d_stLineResults && (_wNbLines >= 0) )
        {
            _d_stLineResults[wCurrentLine].wEndChar = (short) (_pszText - pszInitText);
            _d_stLineResults[wCurrentLine].a_xLineOrigin[0] = xJustificationShift;
            _d_stLineResults[wCurrentLine].a_xLineOrigin[1] = MTH_C_ZERO;
            _d_stLineResults[wCurrentLine].a_xLineOrigin[2] = (_p_stStatus->xBaseCharZ)  ;
        }

        if ( _pszText < pszEndText )
        {
            if ( *_pszText == '/' )
            {
                char bLineBreak;
                do
                {
                    unsigned char ucChar;

                    _pszText = TFMT_pszReadNextChar(_p_stStatus, _hSuperObjPerso, _pszText, & ucChar, &bLineBreak);
                } while ( !bLineBreak );
            }
            else
            {
                _p_stStatus->xBaseCharZ -= _p_stStatus->xLineHeight;
                _p_stStatus->xBaseCharX = _p_stStatus->xBaseToAddX = MTH_C_ZERO;
            }
        }

        _wNbLines --;
        wCurrentLine ++;
    }

    while ( wCurrentChar < wNbObjectsInTargetTable )
    {
        TFMT_RT_vCopyObjectBetweenTables(hTargetObjectTable, _p_stStatus->hSourceObjectTable, wCurrentChar, 0);
        wCurrentChar ++;
    }

    return wCurrentLine;
}

/* 0x00408DD0 */
static long TFMT_lProcessFormater(TFMT_tdstStatus *p_stStatus, char *szString,
                                  ACP_tdxBool *p_bLineBreak, ACP_tdxBool *p_bStartLine)
{
    long i = 1;

    switch (*(szString++))
    {
    case 'C':
    case 'c':
        p_stStatus->cJustificationMode = 'c';
        break;

    case 'H':
    case 'h':

        p_stStatus->xLineHeight = MTH_M_xDiv(MTH_M_xLongToReal(atoi(szString)), MTH_M_xLongToReal(1000));
        break;

    case 'L':
    case 'l':
        *p_bLineBreak = TRUE;
        *p_bStartLine = TRUE;
        p_stStatus->xBaseCharX = p_stStatus->xBaseToAddX = MTH_C_ZERO;
        p_stStatus->xBaseCharZ -= p_stStatus->xLineHeight;
        break;

    case 'W':
    case 'w':

        p_stStatus->xCharWidth = MTH_M_xDiv(MTH_M_xLongToReal(atoi(szString)), MTH_M_xLongToReal(1000));
        break;

    case 'Z':
    case 'z':

        p_stStatus->xCharZoom = MTH_M_xDiv(MTH_M_xLongToReal(atoi(szString)), MTH_M_xLongToReal(1000));
        break;

    case 'o':
    case 'O':
        TFMT_g_lOffsetObjectTable = atoi(szString);
        break;
    }
    while (*szString != ':')
    {
        szString++;
        i++;
    }
    return (++i);
}

static __inline MTH_tdxReal GetLetterSpacing(unsigned char ucChar)
{

    return TFMT_a_xLetterWidth[ucChar];
}

/* 0x00408F20 */
void __cdecl TEXT_vComputeLengthOgString(char *p_szString, TFMT_tdstStatus *p_stStatus)
{
    register long lNbChar = 0;
    MTH_tdxReal xZoom, xMaxScreenWidth = MTH_C_ZERO;
    ACP_tdxBool bLineBreak = FALSE, bStartLine = FALSE;

    while (*p_szString)
    {
        if (*p_szString == '/')
        {

            ++p_szString;
            p_szString += TFMT_lProcessFormater(p_stStatus, p_szString, &bLineBreak, &bStartLine);
            if (bLineBreak)
            {
                xMaxScreenWidth = MTH_M_xMax(xMaxScreenWidth, p_stStatus->xScreenWidth);
                p_stStatus->xScreenWidth = MTH_C_ZERO;
                bLineBreak = FALSE;
            }
        }
        else
        {
            xZoom = p_stStatus->xCharZoom;
            if (MTH_M_bEqualZero(xZoom))
            {
                if (TFMT_ga_ulNbScale)
                    xZoom = TFMT_ga_fLetterScale[(lNbChar % TFMT_ga_ulNbScale)];
                else
                    xZoom = MTH_C_ONE;
            }
            lNbChar++;
            if (p_stStatus->xCharWidth)
            {
                p_stStatus->xScreenWidth += MTH_M_xMul(xZoom, p_stStatus->xCharWidth);
            }
            else
            {
                unsigned char ucTemp = fn_ucToTableElement(*p_szString);
                p_stStatus->xScreenWidth += MTH_M_xMul(xZoom, GetLetterSpacing(ucTemp));
            }
            p_szString++;
        }
    }
    p_stStatus->xScreenWidth = MTH_M_xMax(p_stStatus->xScreenWidth, xMaxScreenWidth);
}

/* 0x00409060 */
static char *TFMT_pszReadNextChar2(
                             TFMT_tdstStatus            *_p_stStatus,
                             HIE_tdstSuperObject        *_hSuperObjPerso,
                             char                       *_pszCurrentChar,
                             unsigned char              *_p_ucReturnedChar,
                             ACP_tdxBool                *_p_bLineBreak,
                             BOOL                       bDisplayFullString,
                             short                      *wCurrentChar,
                             unsigned long              *ulEvent,
                             unsigned long              **ulDelay,
                             short                      *wCurrentChar4Channels,
                             unsigned short             *p_uwIndexOfChar
                          )
{
    register unsigned char ucChar;
    register unsigned short uwLenString;
    long lNbChar = 0;
    MTH_tdxReal xPrevZoom, xPrevWidth = 0, xWidth = 0;
    MTH_tdxReal xZoom = 0;
    ACP_tdxBool bStartLine;

    *_p_bLineBreak = FALSE;
    xPrevZoom = MTH_C_ZERO;

    if (!bDisplayFullString)
    {
        register long i = 0;

        bStartLine = TRUE;
        uwLenString = (unsigned short) strlen(_pszCurrentChar);
        ucChar = (unsigned char) *(_pszCurrentChar + *wCurrentChar);

        if (*ulEvent)
        {
            if (ucChar == '/')
            {
                ucChar = (unsigned char) (*(_pszCurrentChar + *wCurrentChar) + 1);
                if ((ucChar == 'E') || (ucChar == 'e'))
                {
                    (*wCurrentChar)++;

                    *ulEvent = (unsigned long) atoi(_pszCurrentChar + (++(*wCurrentChar)));
                    while (*(_pszCurrentChar + (*wCurrentChar)++) != ':');
                }
                else
                {
                    *ulEvent = 0;
                }
            }
            else
            {
                *ulEvent = 0;
            }

            *_p_ucReturnedChar = 0;
            _pszCurrentChar += uwLenString;
            return _pszCurrentChar;
        }

        while (ucChar == '/')
        {
            ucChar = (unsigned char) _pszCurrentChar[++(*wCurrentChar)];
            if ((ucChar == 'E') || (ucChar == 'e'))
            {

                *ulEvent = (unsigned long) atoi(_pszCurrentChar + (++(*wCurrentChar)));
                while (*(_pszCurrentChar + (*wCurrentChar)++) != ':');

                *_p_ucReturnedChar = 0;
                _pszCurrentChar += uwLenString;
                return _pszCurrentChar;
            }
            else if ((ucChar == 'D') || (ucChar == 'd') || (ucChar == 'P') || (ucChar == 'p'))
            {

                **ulDelay = (unsigned long) atoi(_pszCurrentChar + (++(*wCurrentChar)));
                if ((ucChar == 'P') || (ucChar == 'p'))
                {
                    **ulDelay = -((int) **ulDelay);
                }

                while (*(_pszCurrentChar + (*wCurrentChar)++) != ':');

                *_p_ucReturnedChar = 0;
                _pszCurrentChar += uwLenString;
                return _pszCurrentChar;
            }
            else
            {
                while (*(_pszCurrentChar + (*wCurrentChar)++) != ':');
                ucChar = *(_pszCurrentChar + *wCurrentChar);
            }
        }

        *wCurrentChar4Channels = -1;

        for (; i <= *wCurrentChar; i++)
        {
            ucChar = (unsigned char) *(_pszCurrentChar + i);

            if (ucChar == '/')
            {
                i += TFMT_lProcessFormater(_p_stStatus, _pszCurrentChar + i + 1, _p_bLineBreak, &bStartLine);
            }
            else
            {
                xZoom = _p_stStatus->xCharZoom;

                if (MTH_M_bEqualZero(xZoom))
                {
                    if (TFMT_ga_ulNbScale)
                        xZoom = TFMT_ga_fLetterScale[(lNbChar % TFMT_ga_ulNbScale)];
                    else
                        xZoom = MTH_C_ONE;
                }
                _p_stStatus->xCurCharZoom = xZoom;

                if (_p_stStatus->xCharWidth)
                {
                    xWidth = _p_stStatus->xCharWidth;
                }
                else
                {
                    unsigned char ucTemp = fn_ucToTableElement(ucChar);
                    xWidth = GetLetterSpacing(ucTemp);
                }

                (*wCurrentChar4Channels)++;
                if (!bStartLine)
                {
                    _p_stStatus->xBaseToAddX += MTH_M_xMul(xPrevZoom, xPrevWidth);
                }
                else
                {
                    bStartLine = FALSE;
                }
                lNbChar++;
                xPrevZoom = xZoom;
                xPrevWidth = xWidth;
            }
        }
        _p_stStatus->xCurCharWidth = MTH_M_xMul(xZoom, xWidth);
        _p_stStatus->xBaseCharX += _p_stStatus->xBaseToAddX;
    }

    else
    {
        bStartLine = FALSE;
        uwLenString = (unsigned short) strlen(_pszCurrentChar);

        lNbChar = *p_uwIndexOfChar;
        while (*_pszCurrentChar++ == '/')
        {
            _pszCurrentChar += TFMT_lProcessFormater(_p_stStatus, _pszCurrentChar, _p_bLineBreak, &bStartLine);
        }

        ucChar = (unsigned char) *(_pszCurrentChar - 1);

        xZoom = _p_stStatus->xCharZoom;
        if (MTH_M_bEqualZero(xZoom))
        {
            if (TFMT_ga_ulNbScale)
                xZoom = TFMT_ga_fLetterScale[(lNbChar % TFMT_ga_ulNbScale)];
            else
                xZoom = MTH_C_ONE;
        }
        _p_stStatus->xCurCharZoom = xZoom;

        if (!bStartLine)
        {
            _p_stStatus->xBaseCharX += _p_stStatus->xBaseToAddX;
        }
        else
        {
            bStartLine = FALSE;
        }

        if (_p_stStatus->xCharWidth)
        {
            _p_stStatus->xBaseToAddX = MTH_M_xMul(xZoom, _p_stStatus->xCharWidth);
        }
        else
        {
            unsigned char ucTemp = fn_ucToTableElement(ucChar);
            _p_stStatus->xBaseToAddX = MTH_M_xMul(xZoom, GetLetterSpacing(ucTemp));
        }
        _p_stStatus->xCurCharWidth = _p_stStatus->xBaseToAddX;
        lNbChar++;
    }

    *_p_ucReturnedChar = ucChar;
    if (!bDisplayFullString)
    {
        _pszCurrentChar += uwLenString;
    }

    *p_uwIndexOfChar = (unsigned short) lNbChar;
    return _pszCurrentChar;
}

/* 0x004094C0 */
static void __cdecl TFMT_pc_vInitChannelArray(MS_tdxHandleTo3dData h_Current3dData, unsigned short uwNbChannels)
{
    /* 0x004094D5 */
    CHN_tdxHandleToChannelArray hChannelArray;

    h_Current3dData->ulNumberOfChannels = uwNbChannels;
    fn_vInitCSOList(&hChannelArray, uwNbChannels);
    fn_v3dDataSetChannelSOList(h_Current3dData, hChannelArray);
    fn_v3dDataSetFirstActiveChannel(h_Current3dData, hChannelArray);
}

/* 0x00409500 */
static void __cdecl TFMT_pc_vAddChannelChild(HIE_tdstSuperObject *p_stSO, short wChannelIndex,
                                             short wSourceObject, MTH_tdxReal xBaseCharX,
                                             MTH_tdxReal xBaseCharZ, MTH_tdxReal xCurCharWidth,
                                             MTH_tdxReal xRandomPosRadius, MTH_tdxReal xCurCharZoom)
{
    MS_tdxHandleTo3dData          h_Current3dData;
    CHN_tdxHandleToChannelArray   hChannelArray;
    tdstAChannel                  *p_stChannel;
    HIE_tdstSuperObject           *p_stSO_Child;
    POS_tdstCompletePosition      *hChannelLocalMatrix;
    struct tdstObjectsTableElement_ *d_stCurrentObjectsTable;

    MTH_tdxReal                   axVec1[6];
    MTH_tdxReal                   axVec2[3];
    MTH_tdxReal                   xHalfCharWidth;

    h_Current3dData         = M_GetMSHandle(p_stSO, 3dData);
    hChannelArray           = fn_h3dDataGetChannelSOList(h_Current3dData);
    d_stCurrentObjectsTable = fn_h3dDataGetCurrentObjectsTable(h_Current3dData)->d_stObjectsTable;

    p_stSO_Child = PLA_fn_hFindNextFreeSupObj();

    if (p_stSO_Child->p_stNextBrother != NULL)
        p_stSO_Child->p_stNextBrother->p_stPrevBrother = p_stSO_Child->p_stPrevBrother;
    if (p_stSO_Child->p_stPrevBrother != NULL)
        p_stSO_Child->p_stPrevBrother->p_stNextBrother = p_stSO_Child->p_stNextBrother;
    if (p_stSO_Child->p_stFather != NULL)
    {
        p_stSO_Child->p_stFather->lNumberOfChildren--;
        if (p_stSO_Child->p_stFather->p_stFirstChild == p_stSO_Child)
            p_stSO_Child->p_stFather->p_stFirstChild = p_stSO_Child->p_stNextBrother;
        if (p_stSO_Child->p_stFather->p_stLastChild == p_stSO_Child)
            p_stSO_Child->p_stFather->p_stLastChild = p_stSO_Child->p_stPrevBrother;
    }
    p_stSO_Child->p_stNextBrother = NULL;
    p_stSO_Child->p_stPrevBrother = NULL;
    p_stSO_Child->p_stFather      = NULL;
    p_stSO_Child->p_stNextBrother = NULL;
    p_stSO_Child->p_stPrevBrother = p_stSO->p_stLastChild;
    p_stSO_Child->p_stFather      = p_stSO;
    if (p_stSO->p_stFirstChild == NULL)
        p_stSO->p_stFirstChild = p_stSO_Child;
    else
        p_stSO->p_stLastChild->p_stNextBrother = p_stSO_Child;
    p_stSO->p_stLastChild = p_stSO_Child;
    p_stSO->lNumberOfChildren++;

    p_stChannel = &hChannelArray[wChannelIndex];
    p_stChannel->bControlledChannel = TRUE;
    POS_fn_vSetIdentityMatrix(p_stSO_Child->hLocalMatrix);
    p_stChannel->hSupObject = p_stSO_Child;

    HIE_M_vSetSuperObjectMember(p_stSO_Child, ulTypeOfLinkedObject, (d_stCurrentObjectsTable + wSourceObject)->h_Target ? HIE_C_ulPO : HIE_C_ulSuperObject);
    HIE_M_vSetSuperObjectMember(p_stSO_Child, hLinkedObject.p_Void, (d_stCurrentObjectsTable + wSourceObject)->h_Target);
    HIE_fn_vSetSuperObjectTransparenceLevel(p_stSO_Child, MTH_M_xLongToReal(255));

    if (wChannelIndex)
    {
        (p_stChannel - 1)->p_stNextActiveChannel = p_stChannel;
    }

    if (p_stSO_Child != NULL)
    {

        hChannelLocalMatrix = HIE_fn_hGetSuperObjectMatrix(p_stSO_Child);

        xHalfCharWidth = xCurCharWidth * 0.5f;
        axVec1[0] = (xHalfCharWidth + RND_fn_xGetRealRandomValue(TFMT_gs_hRandomHandleForTFMT, MTH_M_xNeg(xRandomPosRadius), xRandomPosRadius)) + xBaseCharX;
        axVec1[1] = MTH_C_ZERO;
        axVec1[2] = RND_fn_xGetRealRandomValue(TFMT_gs_hRandomHandleForTFMT, MTH_M_xNeg(xRandomPosRadius), xRandomPosRadius) + xBaseCharZ;
        POS_fn_vSetTranslationVector(hChannelLocalMatrix, (MTH3D_tdstVector *)axVec1);

        if (MTH_M_xAbs(xCurCharZoom - MTH_C_ONE) > 0.0001f)
        {
            axVec1[0] = xCurCharZoom;
            axVec1[1] = MTH_C_ZERO;
            axVec1[2] = MTH_C_ZERO;
            axVec2[0] = MTH_C_ZERO;
            axVec2[1] = xCurCharZoom;
            axVec2[2] = MTH_C_ZERO;
            axVec1[3] = MTH_C_ZERO;
            axVec1[4] = MTH_C_ZERO;
            axVec1[5] = xCurCharZoom;
            POS_fn_vSetScaleMatrix(hChannelLocalMatrix, (MTH3D_tdstVector *)axVec1,
                                   (MTH3D_tdstVector *)axVec2, (MTH3D_tdstVector *)&axVec1[3]);
        }
    }
}

/* 0x00409720 */
unsigned long TFMT_wPositionModulesAccordingToText2(
                                                     TFMT_tdstStatus            *_p_stStatus,
                                                     HIE_tdstSuperObject        *_hSuperObjPerso,
                                                     char                       *_pszText,
                                                     long                       _lStart,
                                                     long                       _lEnd,
                                                     ACP_tdxBool                *bFirstTimeZ,
                                                     short                      *pwCurrentChar,
                                                     unsigned long              *ulDelay
                                                   )
{
    MS_tdxHandleTo3dData                  h_Current3dData      = NULL;
    short             wCurrentLine          = 0,
                      wCurrentChar          = 0,
                      wCurrentChar4Channels = 0;

    unsigned short    uwLenOfString = 0,
                      uwIndexOfChar;

    unsigned long ulEvent = 0;

    BOOL  bDisplayFullString = FALSE;

    char *pszEndText  = _pszText + (_lEnd >= 0 ? _lEnd : strlen(_pszText));

    if (*bFirstTimeZ == TRUE)
    {
        uwLenOfString = (unsigned short) strlen(_pszText);
    }

    wCurrentChar = *pwCurrentChar;

    if (wCurrentChar == -1)
    {
        bDisplayFullString = TRUE;
        wCurrentChar = 0;
    }

    _pszText += _lStart;
    uwIndexOfChar = (unsigned short)_lStart;

    TFMT_vInitFormatterStatus(_p_stStatus, _hSuperObjPerso, MTH_C_ZERO);

    h_Current3dData = M_GetMSHandle(_hSuperObjPerso,3dData);

    while ((_pszText < pszEndText) && *_pszText)
    {

        while (_pszText < pszEndText)
        {
            ACP_tdxBool   bLineBreak;
            unsigned char ucChar;
            short         wSourceObject,
                          wNbObjectsInSourceTable;

            _pszText = TFMT_pszReadNextChar2(_p_stStatus, _hSuperObjPerso, _pszText, &ucChar, &bLineBreak,
                                             bDisplayFullString, &wCurrentChar, &ulEvent, &ulDelay,
                                             &wCurrentChar4Channels, &uwIndexOfChar);

            wNbObjectsInSourceTable = fn_uwGetObjectTableNumberOfelements(_p_stStatus->hSourceObjectTable);

            if (ucChar < ' ')
            {
                wSourceObject = -1;
            }
            else
            {
                unsigned char ucTemp = fn_ucToTableElement(ucChar);
                wSourceObject = (short)(ucTemp + TFMT_g_lOffsetObjectTable);
            }

            if (wSourceObject < 0 || wSourceObject >= wNbObjectsInSourceTable)
            {
                *pwCurrentChar = wCurrentChar;
                return ulEvent;
            }
            else
            {

                if (*bFirstTimeZ == TRUE)
                {
                    TFMT_pc_vInitChannelArray(h_Current3dData, uwLenOfString);
                    *bFirstTimeZ = FALSE;
                }

                TFMT_pc_vAddChannelChild(_hSuperObjPerso, wCurrentChar4Channels, wSourceObject,
                                         _p_stStatus->xBaseCharX, _p_stStatus->xBaseCharZ,
                                         _p_stStatus->xCurCharWidth, _p_stStatus->xRandomPosRadius,
                                         _p_stStatus->xCurCharZoom);
            }
            wCurrentChar4Channels ++;
            *pwCurrentChar = ++wCurrentChar;
        }
        wCurrentLine ++;
    }

    return ulEvent;
}
