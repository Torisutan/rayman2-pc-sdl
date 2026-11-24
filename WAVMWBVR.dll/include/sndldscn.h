#ifndef _SND_LOAD_SCRIPT_H_
#define _SND_LOAD_SCRIPT_H_

#include "SCR.h"

#ifdef __cplusplus
extern "C" {
#endif

#undef CPA_EXPORT
#if defined(CPA_WANTS_IMPORT)
#define CPA_EXPORT __declspec(dllimport)
#elif defined(CPA_WANTS_EXPORT)
#define CPA_EXPORT __declspec(dllexport)
#else
#define CPA_EXPORT
#endif

char *SND_fn_pszGetResourceSectionNameFromPointer(tdstBlockResourceMem *_pResMem);
extern SCR_tdst_Link_Table SND_gs_ResTable;
extern CPA_EXPORT SCR_tdst_Link_Table* SND_pstGetEventGroupLinkTable(void);

extern CPA_EXPORT SCR_tdst_Link_Table* SND_pstGetEventPreEditorLoadTable(void);

extern CPA_EXPORT void SND_fn_vConvertBRDiskToString(tdstBlockResourceDisk *_pDisk,char *_szString);
extern CPA_EXPORT void SND_fn_vConvertStringToBRDisk(char *_szString,tdstBlockResourceDisk *_pDisk);
extern unsigned long SND_fn_ulGetEventGroupId(const char *_szFileName);

void SND_fn_vInitScriptSnd(void);
void SND_fn_vDesInitScriptSnd(void);
CPA_EXPORT const char * SND_fn_czGetProjectTitle(void);

typedef struct _tdstResTypeList tdstResTypeList;
typedef struct _tdstResTypeFunctions tdstResTypeFunctions;

struct _tdstResTypeList
{
	SNDLST2_M_DynamicParentDeclaration( tdstResTypeFunctions *)
};

typedef void (*td_pfn_vResScriptEntryAnalysis)(char *szAction, char *szParams[],tdstBlockResourceDisk *pResDisk);
typedef void (*td_pfn_vBRMemInit)(tdstBlockResourceMem *pResMem);
typedef void (*td_pfn_vBRDiskInit)(tdstBlockResourceDisk *pResDisk);

struct _tdstResTypeFunctions
{
	int m_iResTypeId;
	td_pfn_vResScriptEntryAnalysis		m_pAnalyseEntry;
	td_pfn_vBRMemInit m_pBRMemInit;
	td_pfn_vBRDiskInit m_pBRDiskInit;
	SNDLST2_M_DynamicChildDeclaration(tdstResTypeFunctions*,tdstResTypeList*)
};

void SND_fn_vInitResTypeList(void);
void SND_fn_vDesInitResTypeList(void);
void SND_fn_vAddResType(tdstResTypeFunctions *_pResFunctions);
void SND_fn_vAnalyseScriptEntryForResType(int _iResTypeId,char *szAction, char *szParams[],tdstBlockResourceDisk *pResDisk);
void SND_fn_vInitBRMemForResType(int _iResTypeId,tdstBlockResourceMem *pResMem);
void SND_fn_vInitBRDiskForResType(int _iResTypeId,tdstBlockResourceDisk *pResDisk);

#ifdef __cplusplus
}
#endif

#endif
