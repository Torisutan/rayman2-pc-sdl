#include <stdlib.h>
#include "SND/snddef.h"

typedef long SndReal;

extern SndReal SND_CALL SND_fn_rDivRealRealSnd(SndReal r1, SndReal r2);
extern SndReal SND_CALL SND_fn_rMulRealRealQuickSnd(SndReal r1, SndReal r2);

#undef RAND_MAX
#define RAND_MAX 32767

/* 0x00458830 */
SndReal SND_fn_rGetRandomSnd(SndReal max)
{
	return SND_fn_rMulRealRealQuickSnd(SND_fn_rDivRealRealSnd(rand(), RAND_MAX) ,max);
}

#if defined(_DLL_COMPILATION_MODE)
extern void *SND_pc_hModuleSxd;
extern void *SND_pc_hModuleCd;
extern void *SND_pc_hModuleMpeg;
extern void *SND_pc_hModuleDbg;
extern void *SND_pc_hModuleAdpcm;
extern void *SND_pc_hModuleRecord;

/* 0x00458850 */
void *__stdcall SND_fn_vGetHModuleDbg(void)    { return SND_pc_hModuleDbg; }
/* 0x00458860 */
void *__stdcall SND_fn_vGetHModuleMpeg(void)   { return SND_pc_hModuleMpeg; }
/* 0x00458870 */
void *__stdcall SND_fn_vGetHModuleWav(void)    { return SND_pc_hModuleSxd; }
/* 0x00458880 */
void *__stdcall SND_fn_vGetHModuleCd(void)     { return SND_pc_hModuleCd; }
/* 0x00458890 */
void *__stdcall SND_fn_vGetHModuleAdpcm(void)  { return SND_pc_hModuleAdpcm; }
/* 0x004588A0 */
void *__stdcall SND_fn_vGetHModuleRecord(void) { return SND_pc_hModuleRecord; }
#endif
