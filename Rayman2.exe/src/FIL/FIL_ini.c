/*
 =======================================================================================
  Name        : FIL_ini.c
  Description : Ini files in C-ANSI file mode (FILE_MODE_C_WIN95 not set)

  GetPrivateProfileStringA and WritePrivateProfileStringA on the C runtime, behind
  FIL_pc_M_ulGetProfileString and FIL_pc_M_bWriteProfileString (FIL/FIL_pc.h). The file is
  read whole for every call, as the engine reads a few keys at start and writes one now
  and then. Lines are "[section]" or "key=value";
  names compare without case, blanks around names and values do not count, and a value
  enclosed in quotes loses them. Only the first section of a name counts. Other lines
  are kept as they are.

  Checked against the Win32 functions on ubi.ini, the sound ini and edge cases (lists,
  cut buffers, defaults, duplicate names, LF and CRLF files, writes of every kind): same
  results and same file bytes, except one case the engine never makes: deleting a key
  written with leading blanks, where Win32 leaves the blanks behind.
 =======================================================================================
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "SND/snddef.h"

#ifndef FILE_MODE_C_WIN95

#include "FIL/FIL_pc.h"

/* What a line of an ini file is */
#define FIL_pc_C_lIniOther    0
#define FIL_pc_C_lIniSection  1
#define FIL_pc_C_lIniKey      2

/* Text being built for WritePrivateProfileStringA */
typedef struct FIL_pc_tdstIniText_
{
    char *d_cText;
    long  lLength;
    long  lCapacity;
} FIL_pc_tdstIniText;

/*
 ----------------------------------------------------------------------------------------
  Description   : The whole file, ended by '\0' (*p_lSize without it); NULL when it
                  cannot be read
 ----------------------------------------------------------------------------------------
 */
static char *FIL_pc_fn_p_cLoadIni(const char *szFileName, long *p_lSize)
{
    FILE *p_stFile = fopen(szFileName, "rb");
    char *d_cText;
    long  lSize;

    if ( p_stFile == NULL )
        return NULL;
    fseek(p_stFile, 0, SEEK_END);
    lSize = ftell(p_stFile);
    fseek(p_stFile, 0, SEEK_SET);
    if ( lSize < 0 || (d_cText = (char *)malloc((size_t)lSize + 1)) == NULL )
    {
        fclose(p_stFile);
        return NULL;
    }
    lSize = (long)fread(d_cText, 1, (size_t)lSize, p_stFile);
    fclose(p_stFile);
    d_cText[lSize] = '\0';
    *p_lSize = lSize;
    return d_cText;
}

/*
 ----------------------------------------------------------------------------------------
  Description   : Length of the line at p_cLine without its end; *p_p_cNext = next line
 ----------------------------------------------------------------------------------------
 */
static long FIL_pc_fn_lLineLength(const char *p_cLine, const char *p_cEnd, const char **p_p_cNext)
{
    const char *p_cScan = p_cLine;

    while ( p_cScan < p_cEnd && *p_cScan != '\n' )
        p_cScan++;
    *p_p_cNext = (p_cScan < p_cEnd) ? p_cScan + 1 : p_cEnd;
    if ( p_cScan > p_cLine && p_cScan[-1] == '\r' )
        p_cScan--;
    return (long)(p_cScan - p_cLine);
}

static void FIL_pc_fn_vTrim(const char **p_p_cText, long *p_lLength)
{
    while ( *p_lLength > 0 && (**p_p_cText == ' ' || **p_p_cText == '\t') )
    {
        (*p_p_cText)++;
        (*p_lLength)--;
    }
    while ( *p_lLength > 0 && ((*p_p_cText)[*p_lLength - 1] == ' ' || (*p_p_cText)[*p_lLength - 1] == '\t') )
        (*p_lLength)--;
}

/*
 ----------------------------------------------------------------------------------------
  Description   : What the line is; its section name, or its key name and value
 ----------------------------------------------------------------------------------------
 */
static long FIL_pc_fn_lParseIniLine(const char *p_cLine, long lLength,
                                    const char **p_p_cName, long *p_lName,
                                    const char **p_p_cValue, long *p_lValue)
{
    long lChar;

    FIL_pc_fn_vTrim(&p_cLine, &lLength);
    if ( lLength > 0 && p_cLine[0] == '[' )
    {
        for ( lChar = 1 ; lChar < lLength && p_cLine[lChar] != ']' ; lChar++ )
            ;
        *p_p_cName = p_cLine + 1;
        *p_lName   = lChar - 1;
        FIL_pc_fn_vTrim(p_p_cName, p_lName);
        return FIL_pc_C_lIniSection;
    }
    for ( lChar = 0 ; lChar < lLength && p_cLine[lChar] != '=' ; lChar++ )
        ;
    if ( lChar == lLength )
        return FIL_pc_C_lIniOther;

    *p_p_cName  = p_cLine;
    *p_lName    = lChar;
    FIL_pc_fn_vTrim(p_p_cName, p_lName);
    *p_p_cValue = p_cLine + lChar + 1;
    *p_lValue   = lLength - lChar - 1;
    FIL_pc_fn_vTrim(p_p_cValue, p_lValue);
    if ( *p_lValue >= 2 && ((*p_p_cValue)[0] == '"' || (*p_p_cValue)[0] == '\'')
      && (*p_p_cValue)[*p_lValue - 1] == (*p_p_cValue)[0] )
    {
        (*p_p_cValue)++;
        *p_lValue -= 2;
    }
    return FIL_pc_C_lIniKey;
}

/* Name of lLength characters equal to szName, without case */
static int FIL_pc_fn_bSameName(const char *p_cName, long lLength, const char *szName)
{
    long lChar;

    for ( lChar = 0 ; lChar < lLength ; lChar++ )
    {
        if ( szName[lChar] == '\0' )
            return 0;
        if ( tolower((unsigned char)p_cName[lChar]) != tolower((unsigned char)szName[lChar]) )
            return 0;
    }
    return szName[lLength] == '\0';
}

/* One string into szReturn, cut to fit: the characters copied */
static unsigned long FIL_pc_fn_ulCopyString(char *szReturn, unsigned long ulSize,
                                            const char *p_cText, long lLength)
{
    unsigned long ulCopy = (unsigned long)lLength < ulSize - 1 ? (unsigned long)lLength : ulSize - 1;

    memmove(szReturn, p_cText, ulCopy);
    szReturn[ulCopy] = '\0';
    return ulCopy;
}

/*
 ----------------------------------------------------------------------------------------
  Description   : One name more in a '\0'-separated list. Returns 0 once the list is
                  full (as Win32, also when the name just fits): the last name is cut and
                  the list ended, *p_ulPos = ulSize - 2.
 ----------------------------------------------------------------------------------------
 */
static int FIL_pc_fn_bAddToList(char *szReturn, unsigned long ulSize, unsigned long *p_ulPos,
                                const char *p_cName, long lLength)
{
    if ( *p_ulPos + (unsigned long)lLength + 2 >= ulSize )
    {
        unsigned long ulCopy = (*p_ulPos < ulSize - 2) ? ulSize - 2 - *p_ulPos : 0;

        memcpy(szReturn + *p_ulPos, p_cName, ulCopy);
        szReturn[ulSize - 2] = '\0';
        szReturn[ulSize - 1] = '\0';
        *p_ulPos = ulSize - 2;
        return 0;
    }
    memcpy(szReturn + *p_ulPos, p_cName, (size_t)lLength);
    *p_ulPos += (unsigned long)lLength;
    szReturn[(*p_ulPos)++] = '\0';
    return 1;
}

/*
 ----------------------------------------------------------------------------------------
  Description   : GetPrivateProfileStringA
 ----------------------------------------------------------------------------------------
 */
unsigned long __cdecl FIL_pc_fn_ulGetProfileStringC(const char *szSection, const char *szKey,
                                                    const char *szDefault, char *szReturn,
                                                    unsigned long ulSize, const char *szFileName)
{
    char       *d_cText, *d_cDefault;
    const char *p_cLine, *p_cNext, *p_cEnd, *p_cName, *p_cValue;
    long        lSize = 0, lLength, lName, lValue, lType;
    int         bInSection = 0, bSectionSeen = 0, bFull = 0;
    unsigned long ulPos = 0, ulResult;

    if ( ulSize == 0 )
        return 0;

    /* the default first, without its trailing blanks: callers may pass the return buffer
       as the default */
    lLength = (szDefault != NULL) ? (long)strlen(szDefault) : 0;
    d_cDefault = (char *)malloc((size_t)lLength + 1);
    if ( d_cDefault == NULL )
        return 0;
    memcpy(d_cDefault, szDefault != NULL ? szDefault : "", (size_t)lLength);
    while ( lLength > 0 && d_cDefault[lLength - 1] == ' ' )
        lLength--;
    d_cDefault[lLength] = '\0';

    d_cText = FIL_pc_fn_p_cLoadIni(szFileName, &lSize);
    p_cLine = (d_cText != NULL) ? d_cText : "";
    p_cEnd  = p_cLine + lSize;

    if ( szSection == NULL || szKey == NULL )
    {
        /* list of the section names, or of the key names of a section */
        if ( ulSize < 2 )
        {
            szReturn[0] = '\0';
            ulResult = 0;
        }
        else
        {
            for ( ; p_cLine < p_cEnd && !bFull ; p_cLine = p_cNext )
            {
                lLength = FIL_pc_fn_lLineLength(p_cLine, p_cEnd, &p_cNext);
                lType = FIL_pc_fn_lParseIniLine(p_cLine, lLength, &p_cName, &lName, &p_cValue, &lValue);
                if ( lType == FIL_pc_C_lIniSection )
                {
                    if ( szSection == NULL )
                        bFull = !FIL_pc_fn_bAddToList(szReturn, ulSize, &ulPos, p_cName, lName);
                    else if ( bInSection )
                        break;                  /* only the first section of that name */
                    else
                    {
                        bInSection = FIL_pc_fn_bSameName(p_cName, lName, szSection);
                        bSectionSeen |= bInSection;
                    }
                }
                else if ( lType == FIL_pc_C_lIniKey && szSection != NULL && bInSection )
                    bFull = !FIL_pc_fn_bAddToList(szReturn, ulSize, &ulPos, p_cName, lName);
            }
            if ( !bFull )
                szReturn[ulPos] = '\0';
            ulResult = ulPos;
            /* no such file, or no such section: the default, as for a value */
            if ( szSection == NULL ? d_cText == NULL : !bSectionSeen )
                ulResult = FIL_pc_fn_ulCopyString(szReturn, ulSize, d_cDefault, (long)strlen(d_cDefault));
        }
    }
    else
    {
        /* one value, else the default without its trailing blanks */
        ulResult = (unsigned long)-1;
        for ( ; p_cLine < p_cEnd ; p_cLine = p_cNext )
        {
            lLength = FIL_pc_fn_lLineLength(p_cLine, p_cEnd, &p_cNext);
            lType = FIL_pc_fn_lParseIniLine(p_cLine, lLength, &p_cName, &lName, &p_cValue, &lValue);
            if ( lType == FIL_pc_C_lIniSection )
            {
                if ( bInSection )
                    break;                      /* only the first section of that name */
                bInSection = FIL_pc_fn_bSameName(p_cName, lName, szSection);
            }
            else if ( lType == FIL_pc_C_lIniKey && bInSection && FIL_pc_fn_bSameName(p_cName, lName, szKey) )
            {
                ulResult = FIL_pc_fn_ulCopyString(szReturn, ulSize, p_cValue, lValue);
                break;
            }
        }
        if ( ulResult == (unsigned long)-1 )
            ulResult = FIL_pc_fn_ulCopyString(szReturn, ulSize, d_cDefault, (long)strlen(d_cDefault));
    }

    free(d_cText);
    free(d_cDefault);
    return ulResult;
}

/* lLength characters inserted into the text at lPos */
static int FIL_pc_fn_bInsertText(FIL_pc_tdstIniText *p_stText, long lPos, const char *p_cText, long lLength)
{
    if ( p_stText->lLength + lLength + 1 > p_stText->lCapacity )
    {
        long  lCapacity = (p_stText->lLength + lLength + 1) * 2;
        char *d_cNew    = (char *)realloc(p_stText->d_cText, (size_t)lCapacity);

        if ( d_cNew == NULL )
            return 0;
        p_stText->d_cText   = d_cNew;
        p_stText->lCapacity = lCapacity;
    }
    memmove(p_stText->d_cText + lPos + lLength, p_stText->d_cText + lPos, (size_t)(p_stText->lLength - lPos));
    memcpy(p_stText->d_cText + lPos, p_cText, (size_t)lLength);
    p_stText->lLength += lLength;
    return 1;
}

#define M_FIL_bAppendText(p_stTo, p_cFrom, lFrom) \
    FIL_pc_fn_bInsertText((p_stTo), (p_stTo)->lLength, (p_cFrom), (lFrom))

/* Line end of the lines WritePrivateProfileStringA writes, whatever the file uses */
#define FIL_pc_C_szIniEol  "\r\n"
#define FIL_pc_C_lIniEol   2

/* "key=value" and a line end inserted at lPos, after a line end of its own if the text there has none */
static int FIL_pc_fn_bInsertKey(FIL_pc_tdstIniText *p_stText, long lPos, const char *p_cKey, long lKey,
                                const char *szValue)
{
    long lValue = (long)strlen(szValue);
    int  bOk    = 1;

    if ( lPos > 0 && p_stText->d_cText[lPos - 1] != '\n' )
    {
        bOk  = FIL_pc_fn_bInsertText(p_stText, lPos, FIL_pc_C_szIniEol, FIL_pc_C_lIniEol);
        lPos += FIL_pc_C_lIniEol;
    }
    bOk = bOk && FIL_pc_fn_bInsertText(p_stText, lPos, p_cKey, lKey);
    lPos += lKey;
    bOk = bOk && FIL_pc_fn_bInsertText(p_stText, lPos++, "=", 1);
    bOk = bOk && FIL_pc_fn_bInsertText(p_stText, lPos, szValue, lValue);
    lPos += lValue;
    return bOk && FIL_pc_fn_bInsertText(p_stText, lPos, FIL_pc_C_szIniEol, FIL_pc_C_lIniEol);
}

/*
 ----------------------------------------------------------------------------------------
  Description   : WritePrivateProfileStringA. The file is rebuilt line by line: the key
                  of the section is replaced (keeping its spelling in the file) or
                  dropped, or added after the section's last line that is not blank; a
                  missing section is added at the end. Only the first section of the
                  name is touched. Written lines end in "\r\n", as Win32 writes them.
 ----------------------------------------------------------------------------------------
 */
int __cdecl FIL_pc_fn_bWriteProfileStringC(const char *szSection, const char *szKey,
                                           const char *szValue, const char *szFileName)
{
    FIL_pc_tdstIniText stText = { NULL, 0, 0 };
    char       *d_cText;
    const char *p_cLine, *p_cNext, *p_cEnd, *p_cName, *p_cValue;
    long        lSize = 0, lLength, lName, lValue, lType, lInsertPos = 0;
    int         bInSection = 0, bSectionSeen = 0, bDone = 0, bOk = 1, bAdd;
    FILE       *p_stFile;

    if ( szSection == NULL )
        return 0;
    bAdd = (szKey != NULL && szValue != NULL);

    d_cText = FIL_pc_fn_p_cLoadIni(szFileName, &lSize);
    p_cLine = (d_cText != NULL) ? d_cText : "";
    p_cEnd  = p_cLine + lSize;

    for ( ; p_cLine < p_cEnd && bOk ; p_cLine = p_cNext )
    {
        lLength = FIL_pc_fn_lLineLength(p_cLine, p_cEnd, &p_cNext);
        lType = FIL_pc_fn_lParseIniLine(p_cLine, lLength, &p_cName, &lName, &p_cValue, &lValue);
        if ( lType == FIL_pc_C_lIniSection )
        {
            if ( bInSection && bAdd && !bDone )
            {
                bOk  = FIL_pc_fn_bInsertKey(&stText, lInsertPos, szKey, (long)strlen(szKey), szValue);
                bDone = 1;
            }
            bInSection = !bSectionSeen && FIL_pc_fn_bSameName(p_cName, lName, szSection);
            bSectionSeen |= bInSection;
            if ( bInSection && szKey == NULL )
                continue;                       /* the whole section goes */
        }
        else if ( bInSection )
        {
            if ( szKey == NULL )
                continue;
            if ( lType == FIL_pc_C_lIniKey && !bDone && FIL_pc_fn_bSameName(p_cName, lName, szKey) )
            {
                bDone = 1;
                if ( szValue != NULL )
                {
                    /* only the value changes: the text up to '=', the blanks after the old
                       value and the line end stay (a last line without one gets it below) */
                    long lEqual = 0, lValueEnd = lLength;

                    while ( p_cLine[lEqual] != '=' )
                        lEqual++;
                    while ( lValueEnd > lEqual + 1 && (p_cLine[lValueEnd - 1] == ' ' || p_cLine[lValueEnd - 1] == '\t') )
                        lValueEnd--;
                    bOk = M_FIL_bAppendText(&stText, p_cLine, lEqual + 1)
                       && M_FIL_bAppendText(&stText, szValue, (long)strlen(szValue))
                       && M_FIL_bAppendText(&stText, p_cLine + lValueEnd, (long)(p_cNext - p_cLine - lValueEnd));
                    lInsertPos = stText.lLength;
                }
                continue;
            }
        }
        bOk = bOk && M_FIL_bAppendText(&stText, p_cLine, (long)(p_cNext - p_cLine));
        if ( bInSection && lLength > 0 )
            lInsertPos = stText.lLength;
    }
    if ( bOk && bInSection && bAdd && !bDone )
        bOk = FIL_pc_fn_bInsertKey(&stText, lInsertPos, szKey, (long)strlen(szKey), szValue);
    if ( bOk && !bSectionSeen && bAdd )
    {
        if ( stText.lLength > 0 && stText.d_cText[stText.lLength - 1] != '\n' )
            bOk = M_FIL_bAppendText(&stText, FIL_pc_C_szIniEol, FIL_pc_C_lIniEol);
        bOk = bOk && M_FIL_bAppendText(&stText, "[", 1)
                  && M_FIL_bAppendText(&stText, szSection, (long)strlen(szSection))
                  && M_FIL_bAppendText(&stText, "]", 1)
                  && M_FIL_bAppendText(&stText, FIL_pc_C_szIniEol, FIL_pc_C_lIniEol)
                  && FIL_pc_fn_bInsertKey(&stText, stText.lLength, szKey, (long)strlen(szKey), szValue);
    }
    /* Win32 ends the file it rewrites with a line end, even when nothing else changed */
    if ( bOk && stText.lLength > 0 && stText.d_cText[stText.lLength - 1] != '\n' )
        bOk = M_FIL_bAppendText(&stText, FIL_pc_C_szIniEol, FIL_pc_C_lIniEol);
    free(d_cText);

    if ( bOk )
    {
        p_stFile = fopen(szFileName, "wb");
        bOk = (p_stFile != NULL);
        if ( bOk )
        {
            bOk = fwrite(stText.d_cText, 1, (size_t)stText.lLength, p_stFile) == (size_t)stText.lLength;
            bOk = (fclose(p_stFile) == 0) && bOk;
        }
    }
    free(stText.d_cText);
    return bOk;
}

#endif /* FILE_MODE_C_WIN95 */
