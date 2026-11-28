#ifndef MTH_FOPT_H
#define MTH_FOPT_H

#if defined(__cplusplus)
    extern "C"
    {
#endif

#ifndef CPA_EXPORT
#define CPA_EXPORT
#endif
#ifndef INLINE
#define INLINE __inline
#endif
#ifndef MTH_CALL
#define MTH_CALL
#endif
#ifndef MTH_M_vCHK
#define MTH_M_vCHK(X) {}
#endif

#define MTH_M_xQuickDiv MTH_M_xDivLow

#ifdef MTH_LOW
#define MTH_M_xDivOpt MTH_M_xDivLow
#endif
#ifdef MTH_MEDIUM
#define MTH_M_xDivOpt MTH_M_xDivMedium
#endif
#ifdef MTH_HIGH
#define MTH_M_xDivOpt MTH_M_xDivHigh
#endif

#define MTH_M_xQuickSqrt MTH_M_xSqrtLow

#ifdef MTH_LOW
#define MTH_M_xSqrtOpt MTH_M_xSqrtLow
#endif
#ifdef MTH_MEDIUM
#define MTH_M_xSqrtOpt MTH_M_xSqrtMedium
#endif
#ifdef MTH_HIGH
#define MTH_M_xSqrtOpt MTH_M_xSqrtHigh
#endif

#define MTH_M_xQuickInvSqrt MTH_M_xInvSqrtLow

#ifdef MTH_LOW
#define MTH_M_xInvSqrtOpt MTH_M_xInvSqrtLow
#endif
#ifdef MTH_MEDIUM
#define MTH_M_xInvSqrtOpt MTH_M_xInvSqrtMedium
#endif
#ifdef MTH_HIGH
#define MTH_M_xInvSqrtOpt MTH_M_xInvSqrtHigh
#endif

#define MTH_M_xQuickInv MTH_M_xInvLow

#ifdef MTH_LOW
#define MTH_M_xInvOpt MTH_M_xInvLow
#endif
#ifdef MTH_MEDIUM
#define MTH_M_xInvOpt MTH_M_xInvMedium
#endif
#ifdef MTH_HIGH
#define MTH_M_xInvOpt MTH_M_xInvHigh
#endif

#ifdef MTH_LOW
#define MTH_M_xRealToLongRoundOpt MTH_M_xRealToLongRoundLow
#endif
#ifdef MTH_MEDIUM
#define MTH_M_xRealToLongRoundOpt MTH_M_xRealToLongRoundMedium
#endif
#ifdef MTH_HIGH
#define MTH_M_xRealToLongRoundOpt MTH_M_xRealToLongRoundHigh
#endif

#ifdef MTH_LOW
#define MTH_M_xRealToLongOpt MTH_M_xRealToLongLow
#endif
#ifdef MTH_MEDIUM
#define MTH_M_xRealToLongOpt MTH_M_xRealToLongMedium
#endif
#ifdef MTH_HIGH
#define MTH_M_xRealToLongOpt MTH_M_xRealToLongHigh
#endif

#define MTH_M_xRealToLongRoundLow       MTH_fn_fFloat2LongRoundOpt

#define MTH_M_xRealToLongRoundMedium(A) (long) ( (long)(A*2.0F)-(long)(A) )

#define MTH_M_xRealToLongRoundHigh(A)   (long) ( (long)(A*2.0F)-(long)(A) )

#define MTH_M_xRealToLongLow            MTH_fn_fFloat2LongOpt

#define MTH_M_xRealToLongMedium(X)      (long) ( X )

#define MTH_M_xRealToLongHigh(X)        (long) ( X )

#if defined(U64) || defined(NITRO)

#define MTH_M_xSqrtLow( A )             \
 ( sqrtf( A ) )

#define MTH_M_xSqrtMedium( A )          \
 ( sqrtf( A ) )

#define MTH_M_xSqrtHigh( A )            \
 ( sqrtf( A ) )

#else

#define MTH_M_xSqrtLow( A )             \
 ( MTH_fn_fSquareRootOpt( A ) )

#ifdef MTH_CHECK
#define MTH_M_xSqrtMedium( A )  MTH_fn_xSqrtCHK( A )
#else
#define MTH_M_xSqrtMedium( A )          \
                ( (float) sqrt((double) (A)) )
#endif

#ifdef MTH_CHECK
#define MTH_M_xSqrtHigh( A )    MTH_fn_xSqrtCHK( A )
#else
#define MTH_M_xSqrtHigh( A )            \
                ( (float) sqrt((double) (A)) )
#endif

#endif

#if defined(U64) || defined(NITRO)

#define MTH_M_xInvLow( A )              \
                (  MTH_C_ONE / (A) )

#define MTH_M_xInvMedium( A )           \
                (  MTH_C_ONE / (A) )

#define MTH_M_xInvHigh( A )             \
                (  MTH_C_ONE / (A) )

#else

#define MTH_M_xInvLow( A )              \
 ( MTH_fn_fInverseOpt( A ) )

#ifdef MTH_CHECK
#define MTH_M_xInvMedium( A )    MTH_fn_xInvCHK( A )
#else
#define MTH_M_xInvMedium( A )           \
                (  MTH_C_ONE / (A) )
#endif

#ifdef MTH_CHECK
#define MTH_M_xInvHigh( A )     MTH_fn_xInvCHK( A )
#else
#define MTH_M_xInvHigh( A )             \
                (  MTH_C_ONE / (A) )
#endif

#endif

#if defined(U64) || defined(NITRO)

#define MTH_M_xDivLow( A, B)                    \
                ((A) / (B))

#define MTH_M_xDivMedium( A, B)                 \
                ((A) / (B))

#define MTH_M_xDivHigh( A, B)                   \
                ((A) / (B))

#else

#define MTH_M_xDivLow( A, B )           \
 ( MTH_M_xMul( (A), ( MTH_fn_fInverseOpt( B ) ) ) )

#ifdef MTH_CHECK
#define MTH_M_xDivMedium( A, B)         MTH_fn_xDivCHK( A, B)
#else
#define MTH_M_xDivMedium( A, B)                 \
                ((A) / (B))
#endif

#ifdef MTH_CHECK
#define MTH_M_xDivHigh( A, B)            MTH_fn_xDivCHK( A, B)
#else
#define MTH_M_xDivHigh( A, B)                   \
                ((A) / (B))
#endif

#endif

#if defined(U64) || defined(NITRO)

#define MTH_M_xInvSqrtLow( A )          \
 (  MTH_C_ONE / sqrtf( A ) )

#define MTH_M_xInvSqrtMedium( A )               \
 (  MTH_C_ONE / sqrtf( A ) )

#define MTH_M_xInvSqrtHigh( A )         \
 (  MTH_C_ONE / sqrtf( A ) )

#else

#define MTH_M_xInvSqrtLow( A )          \
 ( MTH_fn_fInvSquareRootOpt( A ) )

#ifdef MTH_CHECK
#define MTH_M_xInvSqrtMedium( A )        MTH_fn_xInvSqrtCHK( A )
#else
#define MTH_M_xInvSqrtMedium( A )               \
                ( MTH_C_ONE / (float) sqrt((double) (A)) )
#endif

#ifdef MTH_CHECK
#define MTH_M_xInvSqrtHigh( A )         MTH_fn_xInvSqrtCHK( A )
#else
#define MTH_M_xInvSqrtHigh( A )         \
                ( MTH_C_ONE / (float) sqrt((double) (A)) )
#endif

#endif

#if defined(U64) || defined(NITRO)
#define MTH_M_vInit()                   \
{}
#else

#ifdef MTH_LOW
#define MTH_M_vInit()                   \
 {   MTH_fn_vInit();                    \
  }
#endif

#ifdef MTH_MEDIUM
#define MTH_M_vInit()                   \
 {   MTH_fn_vInit();                    \
  }

#endif

#ifdef MTH_HIGH
#define MTH_M_vInit()                   \
 {   MTH_fn_vInit();                    \
  }

#endif

#endif

#if !defined(U64) && !defined(NITRO)

/* The x87 inline assembly is the PC branch; MTH_C_DEV targets take the C forms (docs/PLAN.md) */
#ifndef MTH_C_DEV
INLINE void MTH_CALL  MTH_fn_vSet24bitFPU(void)
{
  long memvar;

  _asm{
#ifdef MTH_PARANOID
      push eax
#endif

      finit
      fwait
      fstcw [memvar]
      fwait
      mov eax, [memvar]
      and eax, 0fffffcffh
      mov [memvar], eax
      fldcw [memvar]
      fwait

#ifdef MTH_PARANOID
      pop eax
#endif
      }
}

INLINE void MTH_CALL MTH_fn_vSet53bitFPU(void)
{
  long memvar;

  _asm{
#ifdef MTH_PARANOID
      push eax
#endif
      finit
      fwait
      fstcw [memvar]
      fwait
      mov eax, [memvar]
      and eax, 0fffffeffh
      mov [memvar], eax
      fldcw [memvar]
      fwait

#ifdef MTH_PARANOID
      pop eax
#endif
      }
}

INLINE void MTH_CALL MTH_fn_vSet64bitFPU(void)
{
  long memvar;

  _asm{
#ifdef MTH_PARANOID
      push eax
#endif

      finit
      fwait
      fstcw [memvar]
      fwait
      mov eax, [memvar]
      and eax, 0ffffffffh
      mov [memvar], eax
      fldcw [memvar]
      fwait
#ifdef MTH_PARANOID
      pop eax
#endif
      }
}
#endif /* MTH_C_DEV */

extern CPA_EXPORT double MTH_gs_dDecal;

#ifndef MTH_C_DEV
INLINE long  MTH_fn_fFloat2LongRoundOptASM  (float f)
{
  long res;

  _asm{
    fld dword ptr f
    fadd qword ptr MTH_gs_dDecal
    fstp qword ptr res
  }

  return (res);
}
#endif /* MTH_C_DEV */

INLINE long MTH_fn_fFloat2LongRoundOpt (double f)
{
  long res;

  MTH_M_vCHK(f);

  f+= MTH_gs_dDecal;
  res= *(long *)&f;

  return (res);
}

#ifndef MTH_C_DEV
INLINE long  MTH_fn_fFloat2LongOptASM  (float f)
{
  long res;
  double m=0.49999999;

  _asm{
    mov ebx, f
    test ebx, 0x80000000
    jne F2L_negativ
    fld dword ptr f
    fsub qword ptr m
    fadd qword ptr MTH_gs_dDecal
    fstp qword ptr res
    jmp F2L_fin

    F2L_negativ:
    and ebx, 0x7FFFFFFF
    mov f, ebx
    fld dword ptr f
    fsub qword ptr m
    fadd qword ptr MTH_gs_dDecal
    xor ebx, ebx
    fstp qword ptr res
    sub ebx, res
    mov res, ebx
    F2L_fin :
  }

  return (res);
}
#endif /* MTH_C_DEV */

INLINE long  MTH_fn_fFloat2LongOpt  (double f)
{
  long res;
  static  double m=0.499999;

  MTH_M_vCHK(f);

  if( f>0)
    {
      f-= m;
      f+= MTH_gs_dDecal;
      res= *(long *)&f;
      return (res);
    }
  else
    {
      f= -f;
      f-= m;
      f+= MTH_gs_dDecal;
      res= *(long *)&f;
      return (-res);
    }
}

extern CPA_EXPORT unsigned long MTH_g_a2048_fSquareRootTable[1024*2];

INLINE float MTH_CALL MTH_fn_fSquareRootOpt(float f)
{
  float res_sqrt;

  MTH_M_vCHK(f);

#ifdef MTH_C_DEV
  {
    unsigned long ulF = *(unsigned long *)&f;

    *(unsigned long *)&res_sqrt = ((((ulF & 0x7F800000) + 0x3F800000) >> 1) & 0x7F800000)
                                  + MTH_g_a2048_fSquareRootTable[(ulF & 0x00FFE000) >> 13];
  }
#else /* MTH_C_DEV */
  _asm{
#ifdef MTH_PARANOID
        push ebx
        push eax
#endif

        mov  ebx,f
        mov  eax,f
        and  ebx,0x7F800000

        and  eax,0x00FFE000

        add  ebx,0x3F800000
        shr  ebx,1

        shr  eax,11

        and  ebx,0x7F800000
        add  ebx,dword ptr[MTH_g_a2048_fSquareRootTable+eax]
        mov  dword ptr[res_sqrt],ebx

#ifdef MTH_PARANOID
        pop eax
        pop ebx
#endif
      }
#endif /* MTH_C_DEV */

  MTH_M_vCHK(res_sqrt);

  return (res_sqrt);
}

extern CPA_EXPORT unsigned long MTH_g_a1024_fInverse[1024];

INLINE float MTH_CALL MTH_fn_fInverseOpt(float f)
{
  float res_inv;

  MTH_M_vCHK(f);

#ifdef MTH_C_DEV
  {
    unsigned long ulF = *(unsigned long *)&f;

    *(unsigned long *)&res_inv = (0x7E800000 - (ulF & 0xFF800000))
                                 + MTH_g_a1024_fInverse[(ulF & 0x007FE000) >> 13];
  }
#else /* MTH_C_DEV */
  _asm{
#ifdef MTH_PARANOID
      push ecx
      push ebx
      push eax
#endif

      mov  ebx,f
      mov  ecx,0x7E800000
      mov  eax,ebx
      and  eax,0x007FE000
      and  ebx,0xFF800000
      shr  eax,11
      sub  ecx,ebx
      add  ecx,dword ptr[MTH_g_a1024_fInverse + eax]
      mov  dword ptr[res_inv],ecx

#ifdef MTH_PARANOID
      pop eax
      pop ebx
      pop ecx
#endif
      }
#endif /* MTH_C_DEV */

  MTH_M_vCHK(res_inv);

  return (res_inv);
}

extern CPA_EXPORT unsigned long MTH_g_a2048_fInvSquareRootTable[1024*2];

INLINE float MTH_CALL MTH_fn_fInvSquareRootOpt(float f)
{
  float res_invsqrt;

  MTH_M_vCHK(f);

#ifdef MTH_C_DEV
  {
    unsigned long ulF = *(unsigned long *)&f;

    *(unsigned long *)&res_invsqrt = (((0xBD800000 - (ulF & 0x7F800000)) >> 1) & 0x7F800000)
                                     + MTH_g_a2048_fInvSquareRootTable[(ulF & 0x00FFE000) >> 13];
  }
#else /* MTH_C_DEV */
  _asm{
#ifdef MTH_PARANOID
      push ecx
      push ebx
      push eax
#endif

      mov  ecx,f
      mov  eax,f
      and  ecx,0x7F800000

      mov  ebx,0xBD800000

      and  eax,0x00FFE000

      sub  ebx,ecx
      shr  ebx,1

      shr  eax,11
      and  ebx,0x7F800000
      add  ebx,dword ptr[MTH_g_a2048_fInvSquareRootTable + eax]
      mov  dword ptr[res_invsqrt],ebx
#ifdef MTH_PARANOID
      pop eax
      pop ebx
      pop ecx
#endif
  }
#endif /* MTH_C_DEV */

  MTH_M_vCHK(res_invsqrt);

  return (res_invsqrt);
}

extern CPA_EXPORT void MTH_fn_vInit( void );

#ifndef MTH_C_DEV
#pragma warning( disable : 4035 )
static __inline unsigned char __fastcall MTH_M_bEqualZeroAsm( float A)
	{
		__asm
		{
			mov ecx,A
			mov al,0
			and ecx,0x7FFFFFFF
			cmp ecx,1
			adc al,0

		}
	}

static __inline unsigned char __fastcall MTH_M_bDifferentZeroAsm( float A)
	{
		__asm
		{
			mov ecx,A
			mov al,1
			and ecx,0x7FFFFFFF
			cmp ecx,1
			sbb al,0

		}
	}

static __inline unsigned char __fastcall MTH_M_bGreaterZeroAsm( float A)
	{
		__asm
		{
			mov ecx,A
			mov al,0
			sub ecx,1
			cmp ecx,0x7fffffff
			adc al,0
		}
	}

static __inline unsigned char __fastcall MTH_M_bLessEqualZeroAsm( float A)
	{
		__asm
		{
			mov ecx,A
			mov al,1
			sub ecx,1
			cmp ecx,0x7fffffff
			sbb al,0
		}
	}

static __inline unsigned char __fastcall MTH_M_bLessZeroAsm( float A)
	{
		__asm
		{
			mov al,1
			cmp A,0x80000001
			sbb al,0
		}
	}

static __inline unsigned char __fastcall MTH_M_bGreaterEqualZeroAsm( float A)
	{
		__asm
		{
			mov al,0
			cmp A,0x80000001
			adc al,0
		}
	}
#endif /* MTH_C_DEV */

#endif

#if defined(__cplusplus)
    }
#endif

#define MTH_M_p_a2048_fInvSquareRootTable  MTH_g_a2048_fInvSquareRootTable

#endif
