#pragma once
/* Regression harness (fork-only, compiled with RAYMAN2_REGRESS). See src/port/regress.c. */

#ifdef RAYMAN2_REGRESS
#include <windows.h>

/* Nonzero when a regression run is active (R2_REGRESS_OUT is set). */
int  __cdecl REGRESS_fn_bActive(void);
/* Called by the engine thread before every fn_vOneTrame: hashes the state the previous frame
   left, advances the virtual clock, drives the scenario and the input. */
void __cdecl REGRESS_fn_vBeforeFrame(void);
/* Timer counter source while a run is active: nonzero = *p_stOut was set (virtual clock). */
int  __cdecl REGRESS_fn_bGetCounter(short channel, LARGE_INTEGER *p_stOut);

/* Fixed seeds while a run is active. */
#define REGRESS_C_ulLcgSeed   0x12345678u
#define REGRESS_C_ulRandSeed  0xDEADBEEFu
#endif
