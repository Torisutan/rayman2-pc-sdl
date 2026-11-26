#ifndef MTH_FLT_H
#define MTH_FLT_H

#include <math.h>
#include "MTH/MTH_types.h"

#pragma intrinsic(sqrt, cos, sin, tan, atan)

#ifndef INLINE
#define INLINE static __inline
#endif

#include "MTH/MTH_def.h"

#define MTH_M_vCHK(X) {}
#include "MTH/MTH_fopt.h"

#define MTH_C_InfinitPlus       (float)  3.402822E38
#define MTH_C_InfinitMinus      (float) -3.402822E38
#define MTH_C_EpsilonPlus       (float)  1.175495E-38
#define MTH_C_EpsilonMinus      (float) -1.175495E-38

#define MTH_C_Pi                (float)  3.141592653589793
#define MTH_C_2Pi               (float)  6.283185307179586
#define MTH_C_PiBy2             (float)  1.570796326794896
#define MTH_C_PiBy4             (float)  7.853981633974482E-1
#define MTH_C_PiBy8             (float)  3.926990816987241E-1
#define MTH_C_PiBy180           (float)  1.745329251994329E-2
#define MTH_C_180ByPi           (float)  5.729577951308233E1
#define MTH_C_e                 (float)  2.718281828459045
#define MTH_C_ONE               (float)  1.0
#define MTH_C_ZERO              (float)  0.0

#define MTH_C_MinusONE          (float) -1.0

#define MTH_C_2                 (float)  2.0
#define MTH_C_3                 (float)  3.0
#define MTH_C_4                 (float)  4.0
#define MTH_C_5                 (float)  5.0
#define MTH_C_8                 (float)  8.0

#define MTH_C_Minus2            (float) -2.0
#define MTH_C_Minus3            (float) -3.0
#define MTH_C_Minus4            (float) -4.0
#define MTH_C_Minus5            (float) -5.0

#define MTH_C_Inv2              (float)  0.5
#define MTH_C_Inv3              (float)  3.333333333333333E-1
#define MTH_C_Inv4              (float)  0.25
#define MTH_C_Inv5              (float)  0.2

#define MTH_C_MinusInv2         (float) -0.5
#define MTH_C_MinusInv3         (float) -3.333333333333333E-1
#define MTH_C_MinusInv4         (float) -0.25
#define MTH_C_MinusInv5         (float) -0.2

#define MTH_C_Sqrt2             (float)  1.414213562373095
#define MTH_C_Sqrt3             (float)  1.732050807568877
#define MTH_C_Sqrt4             (float)  2.0
#define MTH_C_Sqrt5             (float)  2.236067977499790

#define MTH_C_MinusSqrt2        (float) -1.414213562373095
#define MTH_C_MinusSqrt3        (float) -1.732050807568877
#define MTH_C_MinusSqrt4        (float) -2.0
#define MTH_C_MinusSqrt5        (float) -2.236067977499790

#define MTH_C_InvSqrt2          (float)  7.071067811865475E-1
#define MTH_C_InvSqrt3          (float)  5.773502691896259E-1
#define MTH_C_InvSqrt4          (float)  0.5
#define MTH_C_InvSqrt5          (float)  4.472135954999579E-1

#define MTH_C_MinusInvSqrt2     (float) -7.071067811865475E-1
#define MTH_C_MinusInvSqrt3     (float) -5.773502691896259E-1
#define MTH_C_MinusInvSqrt4     (float) -0.5
#define MTH_C_MinusInvSqrt5     (float) -4.472135954999579E-1

#define MTH_C_MAX_UNSIGNED_CHAR (float) 256.0
#define MTH_C_MAX_LONG			(long) 2147483647

#define MTH_M_xInterpretLongAsFloat(a) (*((float*)&(a)))

#ifdef MTH_OPTIMIZE
#define MTH_M_xRealToLongRound MTH_M_xRealToLongRoundOpt
#else
#ifdef MTH_CHECK
#define MTH_M_xRealToLongRound(A) MTH_fn_xRealToLongRoundCHK(A)
#else
#define MTH_M_xRealToLongRound(A)               \
 (long) ( (long)(A*2.0F)-(long)(A) )
#endif
#endif

#ifdef MTH_CHECK
#define MTH_M_xAdd( A, B)        MTH_fn_xAddCHK( A, B)
#else
#define MTH_M_xAdd( A, B)                       \
                ((A) + (B))
#endif

#ifdef MTH_CHECK
#define MTH_M_xSub( A, B)        MTH_fn_xSubCHK( A, B)
#else
#define MTH_M_xSub( A, B)                       \
                ((A) - (B))
#endif

#ifdef MTH_CHECK
#define MTH_M_xMul( A, B)       MTH_fn_xMulCHK( A, B)
#else
#define MTH_M_xMul( A, B)                       \
                ((A) * (B))
#endif

#ifdef MTH_OPTIMIZE
#define MTH_M_xDiv MTH_M_xDivOpt
#else
#ifdef MTH_CHECK
#define MTH_M_xDiv( A, B)        MTH_fn_xDivCHK( A, B)
#else
#define MTH_M_xDiv( A, B)                       \
                ((A) / (B))
#endif
#endif

#ifdef MTH_CHECK
	#define MTH_M_xNeg( A )         MTH_fn_xNegCHK( A )

#else

INLINE
float MTH_M_xNegOpt( MTH_tdxReal xA )
	{

		unsigned long	xNeg;
		unsigned long*	pA=(unsigned long*)&xA;

		xNeg=(*pA)+0x80000000;

		return MTH_M_xInterpretLongAsFloat(xNeg);
	}

	#define MTH_M_xNeg(a) (MTH_M_xNegOpt(a))
#endif

#ifdef MTH_CHECK
#define MTH_M_xSqr( A )         MTH_fn_xSqrCHK( A )
#else
INLINE
float MTH_M_xSqrOpt( MTH_tdxReal xA )
	{

		return (xA*xA);
	}
	#define MTH_M_xSqr(a) (MTH_M_xSqrOpt(a))

#endif

#ifdef MTH_OPTIMIZE
	#define MTH_M_xSqrt MTH_M_xSqrtOpt
#else
	#ifdef MTH_U64_DEV
		#define MTH_M_xSqrt(A) ( sqrtf( A ) )
		#define MTH_M_xQuickSqrt(A) ( sqrtf( A ) )
	#else
		#ifdef MTH_CHECK
			#define MTH_M_xSqrt(A)          MTH_fn_xSqrtCHK(A)
		#else
			#define MTH_M_xSqrt(A) ( (float) sqrt((double) (A)) )
		#endif
	#endif
#endif

#ifdef MTH_OPTIMIZE
#define MTH_M_xInvSqrt MTH_M_xInvSqrtOpt
#else
#ifdef MTH_CHECK
#define MTH_M_xInvSqrt( A )     MTH_fn_xInvSqrtCHK( A )
#else
#define MTH_M_xInvSqrt( A ) (  MTH_C_ONE / MTH_M_xSqrt( A ) )
#endif
#endif

#ifdef MTH_OPTIMIZE
#define MTH_M_xInv MTH_M_xInvOpt
#else
#ifdef MTH_CHECK
#define MTH_M_xInv(A)            MTH_fn_xInvCHK(A)
#else
#define MTH_M_xInv(A) (  MTH_C_ONE / (A) )
#endif
#endif

INLINE
MTH_tdxReal MTH_M_xSign( MTH_tdxReal x)
{

	register long*	px=(long*)&x;
	long			xSign;

	if (((*px)&0x7FFFFFFF)==0) return MTH_C_ZERO;
	else
		{
			xSign=*px;
			xSign&=0x80000000;
			xSign|=0x3f800000;
			return  MTH_M_xInterpretLongAsFloat(xSign) ;
		}

}

INLINE
MTH_tdxReal MTH_M_xAbs( MTH_tdxReal x)
{
#if !defined(U64) && !defined(NITRO)

	register long*	px=(long*)&x;
	long			xAbs;

	xAbs=(*px)&0x7FFFFFFF;

	return MTH_M_xInterpretLongAsFloat(xAbs);
#else

	*((long *)&x) &= 0x7FFFFFFF;
	return x;
#endif
}

INLINE
MTH_tdxReal MTH_M_xNegAbs( MTH_tdxReal x)
{

	register long*	px=(long*)&x;
	long			xNegAbs;

	xNegAbs=(*px)|0x80000000;

	return MTH_M_xInterpretLongAsFloat(xNegAbs);
}

#define MTH_M_xMax( A, B)                       \
                (( (A)>(B) ) ? (A) : (B))

#define MTH_M_xMin( A, B)                       \
                (( (A)<(B) ) ? (A) : (B))

#define MTH_M_bGreater( A, B)           \
                ( (A)>(B) )

#define MTH_M_bLess( A, B)                      \
                ( (A)<(B) )

#define MTH_M_bGreaterEqual( A, B)      \
                ( (A)>=(B) )

#define MTH_M_bLessEqual( A, B)         \
                ( (A)<=(B) )

#define MTH_M_bEqual( A, B)                     \
                ( (A)==(B) )

#define MTH_M_bDifferent( A, B )        \
                ( (A)!=(B) )

#define MTH_M_bIsNull MTH_M_bEqualZero

#define MTH_M_xMulAddMul(a, b, c, d)    \
                ((a)*(b) + (c)*(d))

#define MTH_M_xSqrAddSqr(a, b)                  \
                ((a)*(a) + (b)*(b))

#define MTH_M_xMulSubMul(a, b, c, d)    \
        ((a)*(b) - (c)*(d))

#define MTH_M_xMulAddMulAddMul(a, b, c, d, e, f)        \
                ((a)*(b) + (c)*(d) + (e)*(f))

#define MTH_M_xSqrAddSqrAddSqr(a, b, c)                         \
                (MTH_M_xSqr(a) + MTH_M_xSqr(b) + MTH_M_xSqr(c))

#define MTH_M_xMul3(a, b, c)    \
                ((a)*(b)*(c))

#define MTH_M_xMul4(a, b, c, d)    \
                ((a)*(b)*(c)*(d))

#define MTH_M_xAdd3(a, b, c)    \
                ((a)+(b)+(c))

#if defined(OPTIMIZED_FOR_PC_FLOATS_WITH_ASM)
	#define MTH_M_bEqualZero(a) (MTH_M_bEqualZeroAsm(a))

#elif !defined(U64) && !defined(NITRO)

INLINE
unsigned char MTH_M_bEqualZeroOpt( MTH_tdxReal xA )
	{

		register long* pA=(long*)&xA;

		return (((*pA)&0x7FFFFFFF)==0);
	}
	#define MTH_M_bEqualZero(a) (MTH_M_bEqualZeroOpt(a))

#else

	#define MTH_M_bEqualZero( x )   \
									 MTH_M_bEqual((x), MTH_C_ZERO )

#endif

#if defined(OPTIMIZED_FOR_PC_FLOATS_WITH_ASM)
	#define MTH_M_bDifferentZero(a) (MTH_M_bDifferentZeroAsm(a))

#elif !defined(U64) && !defined(NITRO)

INLINE
long MTH_M_bDifferentZeroOpt( MTH_tdxReal xA )
	{

		register long* pA=(long*)&xA;

		return ((*pA)&0x7FFFFFFF);
	}
	#define MTH_M_bDifferentZero(a) (MTH_M_bDifferentZeroOpt(a))

#else

	#define MTH_M_bDifferentZero( x )       \
                 MTH_M_bDifferent((x), MTH_C_ZERO )

#endif

#if defined(OPTIMIZED_FOR_PC_FLOATS_WITH_ASM)
	#define MTH_M_bGreaterZero(x) (MTH_M_bGreaterZeroAsm(x))
#else

	#define	MTH_M_bGreaterZero(x)   \
                 MTH_M_bGreater((x), MTH_C_ZERO)
#endif

#if defined(OPTIMIZED_FOR_PC_FLOATS_WITH_ASM)
	#define MTH_M_bLessZero(x) (MTH_M_bLessZeroAsm(x))
#else

	#define MTH_M_bLessZero(x)              \
                 MTH_M_bLess((x), MTH_C_ZERO)
#endif

#if defined(OPTIMIZED_FOR_PC_FLOATS_WITH_ASM)
	#define MTH_M_bGreaterEqualZero(x) (MTH_M_bGreaterEqualZeroAsm(x))
#else

	#define MTH_M_bGreaterEqualZero(x)              \
                 MTH_M_bGreaterEqual((x), MTH_C_ZERO)
#endif

#if defined(OPTIMIZED_FOR_PC_FLOATS_WITH_ASM)
	#define MTH_M_bLessEqualZero(x) (MTH_M_bLessEqualZeroAsm(x))
#else

	#define MTH_M_bLessEqualZero(x)              \
                 MTH_M_bLessEqual((x), MTH_C_ZERO)
#endif

#define MTH_M_bInRangeEqual( x, a, b)   \
                ( MTH_M_bGreaterEqual((x),(a)) && MTH_M_bLessEqual((x),(b)) )

#define MTH_M_bInRange( x, a, b)        \
                ( MTH_M_bGreater((x),(a)) && MTH_M_bLess((x),(b)) )

#define MTH_M_bInUnitEqual( x ) \
                ( MTH_M_bGreaterEqual((x),MTH_C_ZERO) && MTH_M_bLessEqual((x),MTH_C_ONE))

#define MTH_M_bInUnit( x )              \
                ( MTH_M_bGreater((x),MTH_C_ZERO) && MTH_M_bLess((x),MTH_C_ONE) )

#define MTH_M_bEqualWithEpsilon( A, B, EPS)                     \
                (MTH_M_bLess( MTH_M_xAbs((A)-(B)), (EPS) ))

#define MTH_M_bDifferentWithEpsilon( A, B, EPS )        \
                (MTH_M_bGreater( MTH_M_xAbs((A)-(B)), (EPS) ))

#define MTH_M_bIsNullWithEpsilon(A, EPS)                        \
                (MTH_M_bLess( MTH_M_xAbs(A), (EPS) ))

#define MTH_M_xLinearInterpol(a, b, t)  \
                 MTH_M_xAdd( (a), MTH_M_xMul((t), MTH_M_xSub( (b), (a) ) ) )

#define MTH_M_xTrinomeDelta( a, b, c)                                   \
     ( MTH_M_xSub ( MTH_M_xSqr ( (b) ),                                 \
                    MTH_M_xMul ( MTH_C_4, MTH_M_xMul ( (a), (c) ) ) )   \
     )

#ifdef MTH_U64_DEV
 #define MTH_M_xSin( X )                                \
     sinf( X )
#else
 #define MTH_M_xSin( X )                                \
                (float) sin((double) (X) )
#endif

#ifdef MTH_U64_DEV
 #define MTH_M_xCos( X )                                \
     cosf( X )
#else
 #define MTH_M_xCos( X )                                \
                (float) cos((double) (X) )
#endif

#ifdef MTH_U64_DEV
 #define MTH_M_xTan( X )                                \
     ( (sinf(X)) / (cosf(X)) )
#else
 #define MTH_M_xTan( X )                                \
                (float) tan((double) (X) )
#endif

#ifdef MTH_U64_DEV
 #define MTH_M_xATan( X )                       \
                 atan( (X) )
#else
 #define MTH_M_xATan( X )                       \
				 (float)atan(X)
#endif

#ifdef MTH_U64_DEV
 #define MTH_M_xACos( X )                       \
                acos( (X) )
#else
 #define MTH_M_xACos( X )                       \
                (float) acos((double) (X) )
#endif

#ifdef MTH_U64_DEV
 #define MTH_M_xASin( X )                       \
                asin( (X) )
#else
 #define MTH_M_xASin( X )                       \
                (float) asin((double) (X) )
#endif

#ifdef MTH_U64_DEV
 #define MTH_M_xCoTan( X )                      \
     (float)( MTH_C_ONE / MTH_M_xTan( (X) ))
#else
 #define MTH_M_xCoTan( X )                      \
                (float) (MTH_C_ONE / tan((double) (X) ))
#endif

#define MTH_M_xDegToRad( X )            \
                (float) ( ( X ) *( MTH_C_PiBy180 ) )

#define MTH_M_xRadToDeg( X )            \
                (float) ( ( X ) *( MTH_C_180ByPi ) )

#define MTH_M_xLongToFloat( X )             (float) (X)
#define MTH_M_xLongToReal               MTH_M_xLongToFloat

#define MTH_M_xFloatToLong MTH_M_xRealToLongOpt
#define MTH_M_xRealToLong               MTH_M_xFloatToLong

#define MTH_M_xConstFloatToReal(_x) ((MTH_tdxReal)(_x))

#endif
