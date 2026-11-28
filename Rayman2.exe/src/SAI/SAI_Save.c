#include <stdio.h>
#include <windows.h>

#include "SAI/SAI.h"
#include "ERM/CpaError.h"
#include "MMG/MMG_pubaccess.h"
#include "FIL/File.h"
#include "SAI/SAI_globals.h"
#include "SAI/SAI_Codec.h"

/* 0x004B72E4 */
static unsigned char SAI_g_ucBitstreamByte;
static unsigned char SAI_g_ucBitstreamCount;

extern char *__cdecl FUN_004928c0(void);

#define SAI_M_FatalInvalidSaveMode() \
    { \
        ERM_fn_vClearLastError(0); \
        ERM_fn_vUpdateLastError(8, g_ucSAIModuleId, 0, 7, -1, 0xff, 0xff, (char *)0); \
    }

unsigned char __cdecl SAI_fn_ucSaveAllGameTableValues( SAI_tdstInternalStructure *p_stInternalStructure, char *_p_cFileName, SAI_tdeFlags _eFlags);

/* 0x00449ec0 */
unsigned char __cdecl SAI_fn_ucLevelSaveValues(char *_szFileName)
{
    return SAI_fn_ucSaveAllGameTableValues(SAI_g_p_stInternalStructure, _szFileName,
                                           SAI_eLevelSaveTableValue | SAI_eLevelSaveCurrentValue);
}

unsigned char __cdecl SAI_fn_ucLoadAllGameTableValues( SAI_tdstInternalStructure *p_stInternalStructure, char *_p_cFileName, SAI_tdeFlags _eFlags);

/* 0x00449ee0 */
unsigned char __cdecl SAI_fn_ucLoadPlayerValues(char *_szFileName)
{
    return SAI_fn_ucLoadAllGameTableValues(SAI_g_p_stInternalStructure, _szFileName,
                                           SAI_ePlayerSaveTableValue | SAI_ePlayerSaveCurrentValue);
}

static void fn_vResetBitstream(void);
static void __cdecl fn_vFlushBitstream(FILE *p_File);
static unsigned char __cdecl fn_ucSaveGameTableValue(SAI_tdxHandleToListEntry hListEntry, FILE *p_File);

/* 0x0044a650 */
unsigned char __cdecl SAI_fn_ucSaveAllGameTableValues(
    SAI_tdstInternalStructure *p_stInternalStructure, char *_p_cFileName, SAI_tdeFlags _eFlags)
{
    SAI_tdxHandleToListEntry hListEntry = p_stInternalStructure->p_stFirstEntry;
    unsigned char            bReturn    = SAI_TRUE;
    FILE                    *p_File;
    char                     szTmpPath[260];

    sprintf(szTmpPath, "%s/TT.tmp", FUN_004928c0());
    DeleteFileA(szTmpPath);
    FIL_fn_bValidatePath(".", _p_cFileName);

    p_File = fopen(szTmpPath, "wb");
    if (p_File != NULL)
    {
        fn_vResetBitstream();
        for (; hListEntry != NULL; hListEntry = hListEntry->p_stNextEntry)
        {
            if ((hListEntry->eFlags & _eFlags) != 0)
                fn_ucSaveGameTableValue(hListEntry, p_File);
        }
        fn_vFlushBitstream(p_File);
        fclose(p_File);
        FUN_0048d2c0(szTmpPath, _p_cFileName, 2);
    }
    else
        bReturn = SAI_FALSE;
    DeleteFileA(szTmpPath);
    return bReturn;
}

static void __cdecl fn_vWriteBit(unsigned char ucValue, unsigned char ucBitPos, FILE *p_File);

/* 0x0044a730 */
static unsigned char __cdecl fn_ucSaveGameTableValue(SAI_tdxHandleToListEntry hListEntry, FILE *p_File)
{
    unsigned char bReturn = SAI_TRUE;

    switch( hListEntry->eFlags & SAI_C_TypesFlags )
    {
     case SAI_eType8:
        if( (hListEntry->eFlags & SAI_ePlayerSaveTableValue) || (hListEntry->eFlags & SAI_eLevelSaveTableValue) )
            fwrite(&hListEntry->uData.ucCharData, 1, 1, p_File);
        else if( (hListEntry->eFlags & SAI_ePlayerSaveCurrentValue) || (hListEntry->eFlags & SAI_eLevelSaveCurrentValue) )
            fwrite(hListEntry->p_vDataPointer, 1, 1, p_File);
        else
            SAI_M_FatalInvalidSaveMode();
        break;
     case SAI_eType16:
        if( (hListEntry->eFlags & SAI_ePlayerSaveTableValue) || (hListEntry->eFlags & SAI_eLevelSaveTableValue) )
            fwrite(&hListEntry->uData.uwShortData, 1, 2, p_File);
        else if( (hListEntry->eFlags & SAI_ePlayerSaveCurrentValue) || (hListEntry->eFlags & SAI_eLevelSaveCurrentValue) )
            fwrite(hListEntry->p_vDataPointer, 1, 2, p_File);
        else
            SAI_M_FatalInvalidSaveMode();
        break;
     case SAI_eType32:
        if( (hListEntry->eFlags & SAI_ePlayerSaveTableValue) || (hListEntry->eFlags & SAI_eLevelSaveTableValue) )
            fwrite(&hListEntry->uData.ulLongData, 1, 4, p_File);
        else if( (hListEntry->eFlags & SAI_ePlayerSaveCurrentValue) || (hListEntry->eFlags & SAI_eLevelSaveCurrentValue) )
            fwrite(hListEntry->p_vDataPointer, 1, 4, p_File);
        else
            SAI_M_FatalInvalidSaveMode();
        break;
     case SAI_eType64:
        if( (hListEntry->eFlags & SAI_ePlayerSaveTableValue) || (hListEntry->eFlags & SAI_eLevelSaveTableValue) )
            fwrite(&hListEntry->uData.i64Long64Data, 1, 8, p_File);
        else if( (hListEntry->eFlags & SAI_ePlayerSaveCurrentValue) || (hListEntry->eFlags & SAI_eLevelSaveCurrentValue) )
            fwrite(hListEntry->p_vDataPointer, 1, 8, p_File);
        else
            SAI_M_FatalInvalidSaveMode();
        break;
     case SAI_eTypeXX:
        if( (hListEntry->eFlags & SAI_ePlayerSaveTableValue) || (hListEntry->eFlags & SAI_eLevelSaveTableValue) )
            fwrite(hListEntry->uData.stArrayData.p_ucPointer, hListEntry->uData.stArrayData.ulArraySize, 1, p_File);
        else if( (hListEntry->eFlags & SAI_ePlayerSaveCurrentValue) || (hListEntry->eFlags & SAI_eLevelSaveCurrentValue) )
            fwrite(hListEntry->p_vDataPointer, hListEntry->uData.stArrayData.ulArraySize, 1, p_File);
        else
            SAI_M_FatalInvalidSaveMode();
        break;
     case SAI_eTypePointer:
        if( (hListEntry->eFlags & SAI_ePlayerSaveTableValue) || (hListEntry->eFlags & SAI_eLevelSaveTableValue) )
        {
            fwrite(&hListEntry->uData.stPointerData.uwBlocAndModuleID, 1, 2, p_File);
            fwrite(&hListEntry->uData.stPointerData.p_vPointer,        1, 4, p_File);
        }
        else if( (hListEntry->eFlags & SAI_ePlayerSaveCurrentValue) || (hListEntry->eFlags & SAI_eLevelSaveCurrentValue) )
        {
            unsigned short uwBlocId;
            unsigned long  ulValue = *(unsigned long *)hListEntry->p_vDataPointer;
            if( ulValue != 0 )
            {
                void *p_vBeginBloc;
                Mmg_fn_vWhereIs( (void *)ulValue, &uwBlocId, &p_vBeginBloc );
                if( uwBlocId != 0xffff )
                    ulValue = ulValue - (unsigned long)p_vBeginBloc;
            }
            else
                uwBlocId = 0xffff;
            fwrite(&uwBlocId, 1, 2, p_File);
            fwrite(&ulValue,  1, 4, p_File);
        }
        else
            SAI_M_FatalInvalidSaveMode();
        break;
     case SAI_eType1:
        if( (hListEntry->eFlags & SAI_ePlayerSaveTableValue) || (hListEntry->eFlags & SAI_eLevelSaveTableValue) )
            fn_vWriteBit(hListEntry->uData.stBooleanData.ucCharData,
                         hListEntry->uData.stBooleanData.ucBitPosition, p_File);
        else if( (hListEntry->eFlags & SAI_ePlayerSaveCurrentValue) || (hListEntry->eFlags & SAI_eLevelSaveCurrentValue) )
            fn_vWriteBit(*(unsigned char *)hListEntry->p_vDataPointer,
                         hListEntry->uData.stBooleanData.ucBitPosition, p_File);
        else
            SAI_M_FatalInvalidSaveMode();
        break;
     default:
        bReturn = SAI_FALSE;
        break;
    }
    return bReturn;
}

static unsigned char __cdecl fn_uReadBit(FILE *p_File);

/* 0x0044aaf0 */
static unsigned char __cdecl fn_ucLoadGameTableValue(SAI_tdxHandleToListEntry hListEntry, FILE *p_File)
{
    unsigned char bReturn = SAI_TRUE;

    if (!feof(p_File))
    switch (hListEntry->eFlags & SAI_C_TypesFlags)
    {
     case SAI_eType8:
        fread(&hListEntry->uData.ucCharData, 1, 1, p_File);
        break;
     case SAI_eType16:
        fread(&hListEntry->uData.uwShortData, 1, 2, p_File);
        break;
     case SAI_eType32:
        fread(&hListEntry->uData.ulLongData, 1, 4, p_File);
        break;
     case SAI_eType64:
        fread(&hListEntry->uData.i64Long64Data, 1, 8, p_File);
        break;
     case SAI_eTypeXX:
        fread(hListEntry->uData.stArrayData.p_ucPointer,
              hListEntry->uData.stArrayData.ulArraySize, 1, p_File);
        break;
     case SAI_eTypePointer:
        fread(&hListEntry->uData.stPointerData.uwBlocAndModuleID, 1, 2, p_File);
        fread(&hListEntry->uData.stPointerData.p_vPointer,        1, 4, p_File);
        break;
     case SAI_eType1:
        hListEntry->uData.stBooleanData.ucCharData =
            (unsigned char)(fn_uReadBit(p_File)
                            << hListEntry->uData.stBooleanData.ucBitPosition);
        break;
     default:
        bReturn = SAI_FALSE;
        break;
    }
    return bReturn;
}

/* 0x0044ac10 */
unsigned char __cdecl SAI_fn_ucLoadAllGameTableValues(
    SAI_tdstInternalStructure *p_stInternalStructure, char *_p_cFileName, SAI_tdeFlags _eFlags)
{
    SAI_tdxHandleToListEntry hListEntry = p_stInternalStructure->p_stFirstEntry;
    unsigned char            bReturn    = SAI_FALSE;
    FILE                    *p_File;
    char                     szTmpPath[260];

    sprintf(szTmpPath, "%s/TT.tmp", FUN_004928c0());
    DeleteFileA(szTmpPath);

    if (FUN_0048d370(_p_cFileName, szTmpPath, 2) &&
        (p_File = fopen(szTmpPath, "rb")) != NULL)
    {
        fn_vResetBitstream();
        for (; hListEntry != NULL; hListEntry = hListEntry->p_stNextEntry)
        {
            if ((hListEntry->eFlags & _eFlags) != 0)
                fn_ucLoadGameTableValue(hListEntry, p_File);
        }
        fclose(p_File);
        bReturn = SAI_TRUE;
    }
    DeleteFileA(szTmpPath);
    return bReturn;
}

/* 0x0044acd0 */
static void __cdecl fn_vWriteBit(unsigned char ucValue, unsigned char ucBitPos, FILE *p_File)
{
    SAI_g_ucBitstreamByte |= (unsigned char)(((ucValue >> ucBitPos) & 1) << SAI_g_ucBitstreamCount);
    SAI_g_ucBitstreamCount++;
    if (SAI_g_ucBitstreamCount > 7)
        fn_vFlushBitstream(p_File);
}

/* 0x0044ad20 */
static unsigned char __cdecl fn_uReadBit(FILE *p_File)
{
    unsigned long ulBit;

    if (SAI_g_ucBitstreamCount == 0 || SAI_g_ucBitstreamCount > 7)
    {
        if (fread(&SAI_g_ucBitstreamByte, 1, 1, p_File) == 0)
            return 0xff;
        SAI_g_ucBitstreamCount = 0;
    }
    ulBit = ((unsigned long)SAI_g_ucBitstreamByte >> SAI_g_ucBitstreamCount) & 1;
    SAI_g_ucBitstreamCount++;
    return (unsigned char)ulBit;
}

/* 0x0044ad70 */
static void fn_vResetBitstream(void)
{
    SAI_g_ucBitstreamByte  = 0;
    SAI_g_ucBitstreamCount = 0;
}

/* 0x0044ad80 */
static void __cdecl fn_vFlushBitstream(FILE *p_File)
{
    if (SAI_g_ucBitstreamCount != 0)
    {
        fwrite(&SAI_g_ucBitstreamByte, 1, 1, p_File);
        SAI_g_ucBitstreamByte  = 0;
        SAI_g_ucBitstreamCount = 0;
    }
}
