#ifndef MTH_DEF_H
#define MTH_DEF_H

#define MTH_OPTIMIZE

#ifdef MTH_NO_OPTIMIZE
#pragma message ("*** MTH : Warning MTH_OPTMIZE has been removed!")
#undef MTH_OPTIMIZE
#endif

#ifdef MTH_LOW

#ifdef MTH_MEDIUM
#pragma message ("*** MTH : Precision conflict ! MTH_MEDIUM Disable")
#undef MTH_MEDIUM
#endif

#ifdef MTH_HIGH
#pragma message ("*** MTH : Precision conflict ! MTH_HIGH Disable")
#undef MTH_HIGH
#endif

#pragma message ("*** MTH : Precision set to MTH_LOW")
#define MTH_PrecisionFound

#endif

#ifdef MTH_MEDIUM

#ifdef MTH_LOW
#pragma message ("*** MTH : Precision conflict ! MTH_LOW Disable")
#undef MTH_LOW
#endif

#ifdef MTH_HIGH
#pragma message ("*** MTH : Precision conflict ! MTH_HIGH Disable")
#undef MTH_HIGH
#endif

#pragma message ("*** MTH : Precision set to MTH_MEDIUM")
#define MTH_PrecisionFound

#endif

#ifdef MTH_HIGH

#ifdef MTH_LOW
#pragma message ("*** MTH : Precision conflict ! MTH_LOW Disable")
#undef MTH_LOW
#endif

#ifdef MTH_MEDIUM
#pragma message ("*** MTH : Precision conflict ! MTH_MEDIUM Disable")
#undef MTH_MEDIUM
#endif

#pragma message ("*** MTH : Precision set to MTH_HIGH")
#define MTH_PrecisionFound

#endif

#ifndef  MTH_PrecisionFound

#define MTH_MEDIUM

#endif

#define MTH_UNKOWN_DEV

#ifdef _MSC_VER
#ifndef VISUAL
#define VISUAL
#endif
#endif

#ifdef VISUAL
#undef MTH_UNKOWN_DEV
#define MTH_PC_DEV
#endif
#ifdef WATCOM
#undef MTH_UNKOWN_DEV
#define MTH_PC_DEV
#endif

#if defined(U64)
#undef MTH_UNKOWN_DEV
#undef  MTH_PC_DEV
#define MTH_U64_DEV
#endif

#if defined(NITRO)
#undef MTH_UNKOWN_DEV
#undef MTH_PC_DEV
#define MTH_NITRO_DEV
#endif

#ifdef MTH_UNKOWN_DEV
#pragma message ("*** MTH : !! ALERT !! Alien Target Machine Found (Unkown Target) !")
#pragma message ("***  VISUAL NOR WATCOM NOR U64 is defined ! Check your makefile please !")
#define MTH_RealIsFloat
#define NO_ASSEMBLY_IN_GLI
#endif

#ifdef MTH_NITRO_DEV
#undef MTH_RealIsFloat
#define MTH_RealIsFixed19_12
#define NO_ASSEMBLY_IN_GLI
#endif

#ifdef MTH_U64_DEV
#define MTH_RealIsFloat
#define NO_ASSEMBLY_IN_GLI
#if defined(MTH_RealIsFloat) && !defined(ACTIVE_EDITOR)
	#define OPTIMIZED_FOR_PC_FLOATS
#ifndef CODEWARRIOR_GCC
	#define OPTIMIZED_FOR_U64_ASM
#endif
#endif

#endif

#ifdef MTH_PC_DEV

#define MTH_RealIsFloat

	#define OPTIMIZED_FOR_PC_FLOATS

	#define OPTIMIZED_FOR_PC_FLOATS_WITH_ASM

#define NO_ASSEMBLY_IN_GLI
#endif

#ifdef MTH_PC_FINAL
#define MTH_RealIsFloat
#endif

#define MTH_CALL

#endif
