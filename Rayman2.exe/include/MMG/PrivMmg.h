#ifndef __MMG_PrivMmg_h__
#define __MMG_PrivMmg_h__

#define C_ucFreeHeaderSize (2)

#define C_ucMallocHeaderSize (1)
#define C_ucMallocFooterSize (0)

#define C_uaCleanKeyForBlockWithoutFree (0XAA)

#define C_FirstFreeValueForBlockWithoutFree ((char *)0XFFFFFFFF)

#define C_TypeOfOverflowCheckKeyForBlockWithoutFree unsigned long

typedef struct tdstInformationHeaderOfBolckWithoutFree_{
        unsigned char ucBoundededHeaderSize;
        unsigned char ucAlignment;
        unsigned char ucMode;
}tdstInformationHeaderOfBolckWithoutFree;

typedef struct tdstBlockInfoPriv_{
        char * p_cBeginBlock;
        char * p_cEndBlock;
        char * p_cFirstFree;
        char * p_cMaxMem;
}tdstBlockInfoPriv;

typedef struct tdstCurrentModeInfo_{
        unsigned char ucModuleId;
        unsigned char ucMode;
        void *p_vTdstBlockInfoOrTdstDynInfo;
}tdstCurrentModeInfo;

#include "SND/snddef.h"
#ifndef FILE_MODE_C_WIN95
/* System memory of the MMG blocks from the C runtime (docs/PLAN.md Phase 5, step 3) */
#include <stdlib.h>
#define CPA_MALLOC(size)	malloc(size)
#define CPA_FREE(ptr)		free(ptr)
#else /* original Win32 */
#define CPA_MALLOC(size)	GlobalAlloc(GMEM_FIXED,size)
#define CPA_FREE(ptr)		GlobalFree(ptr)
#endif /* FILE_MODE_C_WIN95 */

#endif
