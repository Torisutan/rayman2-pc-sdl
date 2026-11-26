#ifndef MTH3D_H
#define MTH3D_H

#include <math.h>
#include "MTH/MTH_types.h"
#include "MTH/MTH_flt.h"

#ifndef INLINE
#define INLINE __inline
#endif

#if defined(OPTIMIZED_FOR_PC_FLOATS)
	#if defined(OPTIMIZED_FOR_PC_FLOATS_WITH_ASM)
		#define MTH3D_M_vAddVector( VectDest, VectA, VectB)		MTH3D_M_vAddVectorASM( VectDest, VectA, VectB)
	#else
		#define MTH3D_M_vAddVector( VectDest, VectA, VectB)		MTH3D_M_vAddVectorC( VectDest, VectA, VectB)
	#endif
#else
	#define MTH3D_M_vAddVector( VectDest, VectA, VectB)			MTH3D_M_vAddVectorORG(VectDest, VectA, VectB)
#endif

#if defined(OPTIMIZED_FOR_PC_FLOATS)
	#if defined(OPTIMIZED_FOR_PC_FLOATS_WITH_ASM)
		#define MTH3D_M_vSubVector( VectDest, VectA, VectB)		MTH3D_M_vSubVectorASM( VectDest, VectA, VectB)
	#else
		#define MTH3D_M_vSubVector( VectDest, VectA, VectB)		MTH3D_M_vSubVectorC( VectDest, VectA, VectB)
	#endif
#else
	#define MTH3D_M_vSubVector( VectDest, VectA, VectB)			MTH3D_M_vSubVectorORG(VectDest, VectA, VectB)
#endif

#if defined(OPTIMIZED_FOR_PC_FLOATS)
	#if defined(OPTIMIZED_FOR_PC_FLOATS_WITH_ASM)
		#define MTH3D_M_vNegVector( VectDest, VectA)		MTH3D_M_vNegVectorASM( VectDest, VectA)
	#else
		#define MTH3D_M_vNegVector( VectDest, VectA)		MTH3D_M_vNegVectorC( VectDest, VectA)
	#endif
#else
	#define MTH3D_M_vNegVector( VectDest, VectA)			MTH3D_M_vNegVectorORG(VectDest, VectA)
#endif

#define MTH3D_M_bEqualVector( VectA, VectB)                                     \
                (                                                               \
                (MTH_M_bEqual((VectA)->xX, (VectB)->xX)) &&                     \
				(MTH_M_bEqual((VectA)->xY, (VectB)->xY)) &&                     \
				(MTH_M_bEqual((VectA)->xZ, (VectB)->xZ))                        \
        )

#define MTH3D_M_vAddScalarVector( VectDest, a, VectA)   \
  {                                                     \
    (VectDest)->xX = MTH_M_xAdd((VectA)->xX, (a));      \
    (VectDest)->xY = MTH_M_xAdd((VectA)->xY, (a));      \
    (VectDest)->xZ = MTH_M_xAdd((VectA)->xZ, (a));      \
  }

#define MTH3D_M_vSubScalarVector( VectDest, a, VectA)   \
  {                                                     \
    (VectDest)->xX = MTH_M_xSub((VectA)->xX, (a));      \
    (VectDest)->xY = MTH_M_xSub((VectA)->xY, (a));      \
    (VectDest)->xZ = MTH_M_xSub((VectA)->xZ, (a));      \
  }

#define MTH3D_M_vAdd3( VectDest, VectA, x, y, z)

#if defined(OPTIMIZED_FOR_PC_FLOATS)
	#if defined(OPTIMIZED_FOR_PC_FLOATS_WITH_ASM)
		#define MTH3D_M_vAdd3ScalarVector( VectDest, VectA, x, y, z) 		MTH3D_M_vAdd3ScalarVectorASM( VectDest, VectA, x, y, z)
	#else
		#define MTH3D_M_vAdd3ScalarVector( VectDest, VectA, x, y, z) 		MTH3D_M_vAdd3ScalarVectorC( VectDest, VectA, x, y, z)
	#endif
#else
	#define MTH3D_M_vAdd3ScalarVector( VectDest, VectA, x, y, z) 			MTH3D_M_vAdd3ScalarVectorORG( VectDest, VectA, x, y, z)
#endif

#if defined(OPTIMIZED_FOR_PC_FLOATS)
	#if defined(OPTIMIZED_FOR_PC_FLOATS_WITH_ASM)

		#define MTH3D_M_vMulScalarVector( VectDest, a,VectA)		MTH3D_M_vMulScalarVectorC( VectDest, a, VectA)
	#else
		#define MTH3D_M_vMulScalarVector( VectDest, a,VectA)		MTH3D_M_vMulScalarVectorC( VectDest, a, VectA)
	#endif
#else
	#define MTH3D_M_vMulScalarVector( VectDest, a, VectA)			MTH3D_M_vMulScalarVectorORG(VectDest, a,VectA)
#endif

#if defined(OPTIMIZED_FOR_PC_FLOATS)
	#if defined(OPTIMIZED_FOR_PC_FLOATS_WITH_ASM)
		#define MTH3D_M_vDivScalarVector( VectDest, VectA ,a)		MTH3D_M_vDivScalarVectorASM( VectDest, VectA, a)
	#else
		#define MTH3D_M_vDivScalarVector( VectDest, VectA, a)		MTH3D_M_vDivScalarVectorC( VectDest, VectA, a)
	#endif
#else
	#define MTH3D_M_vDivScalarVector( VectDest, VectA, a)			MTH3D_M_vDivScalarVectorORG(VectDest, VectA, a)
#endif

#if defined(OPTIMIZED_FOR_PC_FLOATS)
	#if defined(OPTIMIZED_FOR_PC_FLOATS_WITH_ASM)

		#define MTH3D_M_xDotProductVector( VectA, VectB)         MTH3D_M_xDotProductVectorC( VectA, VectB)
	#else
		#define MTH3D_M_xDotProductVector( VectA, VectB)         MTH3D_M_xDotProductVectorC( VectA, VectB)
	#endif
#elif defined(NITRO)
	#define MTH3D_M_xDotProductVector( VectA, VectB)             MTH3D_M_xDotProductVectorInl( VectA, VectB)
#else
	#define MTH3D_M_xDotProductVector( VectA, VectB)             MTH3D_M_xDotProductVectorORG( VectA, VectB)
#endif

extern MTH_tdxReal MTH3D_M_xAngleBetweenUnitVectors(MTH3D_tdstVector *pVec1, MTH3D_tdstVector *pVec2);

#if defined(OPTIMIZED_FOR_PC_FLOATS)
	#if defined(OPTIMIZED_FOR_PC_FLOATS_WITH_ASM)
		#define MTH3D_M_vCrossProductVectorWithoutBuffer(VectDest, VectA, VectB)                MTH3D_M_vCrossProductVectorWithoutBufferASM(VectDest, VectA, VectB)
	#else
		#define MTH3D_M_vCrossProductVectorWithoutBuffer(VectDest, VectA, VectB)                MTH3D_M_vCrossProductVectorWithoutBufferC(VectDest, VectA, VectB)
	#endif
#else
	#define MTH3D_M_vCrossProductVectorWithoutBuffer(VectDest, VectA, VectB)                    MTH3D_M_vCrossProductVectorWithoutBufferORG(VectDest, VectA, VectB)
#endif

#if defined(OPTIMIZED_FOR_PC_FLOATS)
	#if defined(OPTIMIZED_FOR_PC_FLOATS_WITH_ASM)
		#define MTH3D_M_vCrossProductVector(VectDest, VectA, VectB)                MTH3D_M_vCrossProductVectorASM(VectDest, VectA, VectB)
	#else
		#define MTH3D_M_vCrossProductVector(VectDest, VectA, VectB)                MTH3D_M_vCrossProductVectorC(VectDest, VectA, VectB)
	#endif
#else
	#define MTH3D_M_vCrossProductVector(VectDest, VectA, VectB)                    MTH3D_M_vCrossProductVectorORG(VectDest, VectA, VectB)
#endif

#if defined(NITRO)
extern MTH_tdxReal MTH3D_M_xSqrVector(MTH3D_tdstVector *pVect);
#else

#define MTH3D_M_xSqrVector( VectA)                                              \
                MTH_M_xAdd(                                                     \
                 MTH_M_xAdd(                                                    \
                        MTH_M_xSqr((VectA)->xX),                                \
            MTH_M_xSqr((VectA)->xY)                                             \
                           ),                                                   \
                MTH_M_xSqr((VectA)->xZ) )

#endif

#if defined(NITRO)
extern MTH_tdxReal MTH3D_M_xNormVector(MTH3D_tdstVector *VectA);
#else
#define MTH3D_M_xNormVector( VectA )                                            \
                        MTH_M_xSqrt( MTH3D_M_xSqrVector( VectA ) )
#endif

#define MTH3D_M_xInvNormVector( VectA )                                         \
                        MTH_M_xInvSqrt( MTH3D_M_xSqrVector( VectA ) )

#define MTH3D_M_xQuickNormVector( VectA )                                            \
						MTH3D_M_xNormVector(VectA)

#define MTH3D_M_xQuickInvNormVector( VectA )                                         \
		MTH3D_M_xInvNormVector(VectA)

#define MTH3D_M_xVectorGapSqr(VectA, VectB)                                     \
                        (MTH_M_xSqrAddSqrAddSqr(                                \
             MTH_M_xSub( (VectA)->xX, (VectB)->xX),                             \
             MTH_M_xSub( (VectA)->xY, (VectB)->xY),                             \
             MTH_M_xSub( (VectA)->xZ, (VectB)->xZ) ) )

#define MTH3D_M_xVectorGap(VectA, VectB)                                                \
                        (MTH_M_xSqrt( MTH3D_M_xVectorGapSqr(VectA, VectB) ))

#if defined(OPTIMIZED_FOR_PC_FLOATS)

#define MTH3D_M_vFillVector( VectDest, a)                                               \
        {   (VectDest)->xZ=(VectDest)->xY=(VectDest)->xX = (a);                         \
        }
#else
#define MTH3D_M_vFillVector( VectDest, a)                                               \
        {   (VectDest)->xX = (a);														\
			(VectDest)->xY = (a);														\
			(VectDest)->xZ = (a);														\
        }
#endif

#if defined(NITRO)
extern void MTH3D_M_vNormalizeVector(MTH3D_tdstVector *VectDest, MTH3D_tdstVector *VectA);
#elif !defined(U64)
#define MTH3D_M_vNormalizeVector(VectDest, VectA)                                  \
         {																		   \
			register MTH_tdxReal inv_norm= MTH3D_M_xInvNormVector( VectA);         \
			MTH3D_M_vMulScalarVector(VectDest, (inv_norm), VectA );                \
         }
#else
#define MTH3D_M_vNormalizeVector(VectDest, VectA)								\
	{																															\
		register MTH_tdxReal xNorm= MTH3D_M_xNormVector( VectA);		\
		if( MTH_M_bDifferentZero(xNorm) )														\
		{																														\
			xNorm = MTH_M_xInv(xNorm);																\
			MTH3D_M_vMulScalarVector(VectDest, xNorm, VectA );				\
		}																														\
	}
#endif

#if !defined(U64) && !defined(NITRO)
#define MTH3D_M_vQuickNormalizeVector(VectDest, VectA)                                  \
         {																		   \
			register MTH_tdxReal inv_norm= MTH3D_M_xQuickInvNormVector( VectA);         \
			MTH3D_M_vMulScalarVector(VectDest, (inv_norm), VectA );                \
         }
#else
#define MTH3D_M_vQuickNormalizeVector(VectDest, VectA)	MTH3D_M_vNormalizeVector(VectDest,VectA)
#endif

#define MTH3D_M_vGetVectorElements( Vx, Vy, Vz, VectA)                  \
        {   *Vx = (VectA)->xX;                                          \
            *Vy = (VectA)->xY;                                          \
            *Vz = (VectA)->xZ; }

#define MTH3D_M_vSetVectorElements( VectDest, Vx, Vy, Vz)               \
        {   (VectDest)->xX = (Vx);                                      \
            (VectDest)->xY = (Vy);                                      \
            (VectDest)->xZ = (Vz); }

#define MTH3D_M_xGetXofVector( VectA )                                  (VectA)->xX

#define MTH3D_M_vSetXofVector( VectDest, Vx)                            { (VectDest)->xX = (Vx); }

#define MTH3D_M_xGetYofVector( VectA )                                  (VectA)->xY

#define MTH3D_M_vSetYofVector( VectDest, Vy)                            { (VectDest)->xY = (Vy); }

#define MTH3D_M_xGetZofVector( VectA )                                  (VectA)->xZ

#define MTH3D_M_vSetZofVector( VectDest, Vz)                            { (VectDest)->xZ = (Vz); }

#if (defined(OPTIMIZED_FOR_PC_FLOATS) || defined(MTH_RealIsFloat))

#define MTH3D_M_vCopyVector( VectDest, VectA)                                           \
	{	((unsigned long *) (VectDest))[0]=	((unsigned long *) (VectA))[0];				\
		((unsigned long *) (VectDest))[1]=	((unsigned long *) (VectA))[1];				\
		((unsigned long *) (VectDest))[2]=	((unsigned long *) (VectA))[2];				\
	}

#else
	#define MTH3D_M_vCopyVector( VectDest, VectA)                                   \
     {  memcpy(VectDest, VectA, sizeof(MTH3D_tdstVector)); }
#endif

#if defined(OPTIMIZED_FOR_PC_FLOATS)
#define MTH3D_M_vNullVector( VectDest)    { (VectDest)->xZ=(VectDest)->xY=(VectDest)->xX=MTH_C_ZERO; }
#else
#define MTH3D_M_vNullVector( VectDest)                                                  \
        {         (VectDest)->xX = MTH_C_ZERO;                                          \
                  (VectDest)->xY = MTH_C_ZERO;                                          \
                  (VectDest)->xZ = MTH_C_ZERO; }
#endif

#if defined(OPTIMIZED_FOR_PC_FLOATS)
	#if defined(OPTIMIZED_FOR_PC_FLOATS_WITH_ASM)
		#define MTH3D_M_vScaleVector( VectDest, VectA, VectB )		MTH3D_M_vScaleVectorASM( VectDest, VectA, VectB)
	#else
		#define MTH3D_M_vScaleVector( VectDest, VectA, VectB)		MTH3D_M_vScaleVectorC( VectDest, VectA, VectB)
	#endif
#else
	#define MTH3D_M_vScaleVector( VectDest, VectA, VectB)			MTH3D_M_vScaleVectorORG(VectDest, VectA, VectB)
#endif

#define MTH3D_M_vMiddleVector( VectDest, VectA, VectB )                 \
        {       MTH3D_M_vAddVector( VectDest, VectA, VectB );           \
                MTH3D_M_vMulScalarVector( VectDest, MTH_C_Inv2 , VectDest); }

#if defined(OPTIMIZED_FOR_PC_FLOATS)
	#if defined(OPTIMIZED_FOR_PC_FLOATS_WITH_ASM)
		#define MTH3D_M_vLinearInterpolVector( VectDest, VectA, VectB, t)		MTH3D_M_vLinearInterpolVectorASM( VectDest, VectA, VectB, t)
	#else
		#define MTH3D_M_vLinearInterpolVector( VectDest, VectA, VectB, t)		MTH3D_M_vLinearInterpolVectorC( VectDest, VectA, VectB, t)
	#endif
#else
	#define MTH3D_M_vLinearInterpolVector( VectDest, VectA, VectB,t)			MTH3D_M_vLinearInterpolVectorORG(VectDest, VectA, VectB, t)
#endif

#if defined(OPTIMIZED_FOR_PC_FLOATS)
	#if defined(OPTIMIZED_FOR_PC_FLOATS_WITH_ASM)
		#define MTH3D_M_vLinearScaleVector( VectDest, VectA, VectB, VectC)		MTH3D_M_vLinearScaleVectorASM( VectDest, VectA, VectB, VectC)
	#else
		#define MTH3D_M_vLinearScaleVector( VectDest, VectA, VectB, VectC)		MTH3D_M_vLinearScaleVectorC( VectDest, VectA, VectB, VectC)
	#endif
#else
	#define MTH3D_M_vLinearScaleVector( VectDest, VectA, VectB, VectC)			MTH3D_M_vLinearScaleVectorORG(VectDest, VectA, VectB, VectC)
#endif

#define MTH3D_M_vMaxVector( VectDest, VectA, VectB)                     \
        {   (VectDest)->xX= MTH_M_xMax( (VectA)->xX, (VectB)->xX );     \
            (VectDest)->xY= MTH_M_xMax( (VectA)->xY, (VectB)->xY );     \
            (VectDest)->xZ= MTH_M_xMax( (VectA)->xZ, (VectB)->xZ ); }

#define MTH3D_M_vMinVector( VectDest, VectA, VectB)                     \
        {   (VectDest)->xX= MTH_M_xMin( (VectA)->xX, (VectB)->xX );     \
            (VectDest)->xY= MTH_M_xMin( (VectA)->xY, (VectB)->xY );     \
            (VectDest)->xZ= MTH_M_xMin( (VectA)->xZ, (VectB)->xZ ); }

#define MTH3D_M_xDetXYVector( VectA, VectB )    MTH_M_xMulSubMul( (VectA)->xX, (VectB)->xY, (VectA)->xY, (VectB)->xX)

#define MTH3D_M_xDetYZVector( VectA, VectB )    MTH_M_xMulSubMul( (VectA)->xY, (VectB)->xZ, (VectA)->xZ, (VectB)->xY)

#define MTH3D_M_xDetZXVector( VectA, VectB )    MTH_M_xMulSubMul( (VectA)->xZ, (VectB)->xX, (VectA)->xX, (VectB)->xZ)

#define MTH3D_M_bIsNullVector( VectA )          ( MTH_M_bEqualZero((VectA)->xX) && MTH_M_bEqualZero((VectA)->xY) && MTH_M_bEqualZero((VectA)->xZ) )

#if defined(OPTIMIZED_FOR_PC_FLOATS)
	#if defined(OPTIMIZED_FOR_PC_FLOATS_WITH_ASM)
		#define MTH3D_M_vMulAddVector( VectDest, x,VectA, VectB )		MTH3D_M_vMulAddVectorASM( VectDest, x,VectA, VectB)
	#else
		#define MTH3D_M_vMulAddVector( VectDest, x,VectA, VectB)		MTH3D_M_vMulAddVectorC( VectDest, x,VectA, VectB)
	#endif
#else
	#define MTH3D_M_vMulAddVector( VectDest, x,VectA, VectB)			MTH3D_M_vMulAddVectorORG(VectDest, x,VectA, VectB)
#endif

#define MTH3D_M_vMullAddVector MTH3D_M_vMulAddVector

#if defined(OPTIMIZED_FOR_PC_FLOATS)
	#if defined(OPTIMIZED_FOR_PC_FLOATS_WITH_ASM)
		#define MTH3D_M_vMul3AddVector( VectDest, x,VectA, y,VectB, z,VectC)		MTH3D_M_vMul3AddVectorASM( VectDest, x,VectA, y,VectB, z,VectC)
	#else
		#define MTH3D_M_vMul3AddVector( VectDest, x,VectA, y,VectB, z,VectC)		MTH3D_M_vMul3AddVectorC( VectDest, x,VectA, y,VectB, z,VectC)
	#endif
#else
	#define MTH3D_M_vMul3AddVector( VectDest, x,VectA, y,VectB, z,VectC)			MTH3D_M_vMul3AddVectorORG(VectDest, x,VectA, y,VectB, z,VectC)
#endif

#if defined(OPTIMIZED_FOR_PC_FLOATS)
	#if defined(OPTIMIZED_FOR_PC_FLOATS_WITH_ASM)
		#define MTH3D_M_vMul4AddVector( VectDest, x,VectA, y,VectB, z,VectC, w,VectD)		MTH3D_M_vMul4AddVectorASM( VectDest, x,VectA, y,VectB, z,VectC, w,VectD)
	#else
		#define MTH3D_M_vMul4AddVector( VectDest, x,VectA, y,VectB, z,VectC, w,VectD)		MTH3D_M_vMul4AddVectorC( VectDest, x,VectA, y,VectB, z,VectC, w,VectD)
	#endif
#else
	#define MTH3D_M_vMul4AddVector( VectDest, x,VectA, y,VectB, z,VectC, w,VectD)			MTH3D_M_vMul4AddVectorORG(VectDest, x,VectA, y,VectB, z,VectC, w,VectD)
#endif

#define MTH3D_M_vSetBaseIVector( VectDest )     { (VectDest)->xX = MTH_C_ONE; (VectDest)->xZ = (VectDest)->xY = MTH_C_ZERO; }

#define MTH3D_M_vSetBaseJVector( VectDest )     { (VectDest)->xX = MTH_C_ZERO; (VectDest)->xY = MTH_C_ONE; (VectDest)->xZ = MTH_C_ZERO; }

#define MTH3D_M_vSetBaseKVector( VectDest )     { (VectDest)->xY = (VectDest)->xX = MTH_C_ZERO; (VectDest)->xZ = MTH_C_ONE; }

#define MTH3D_M_xNorm1Vector( VectA )     MTH_M_xAdd3( MTH_M_xAbs((VectA)->xX) , MTH_M_xAbs((VectA)->xY) , MTH_M_xAbs((VectA)->xZ) )

#if defined(OPTIMIZED_FOR_PC_FLOATS)

#define MTH3D_M_vCopyMatrix( MatDest, MatA)                             \
     {  MTH3D_M_vCopyVector(&((MatDest)->stCol_0),&((MatA)->stCol_0) ); \
		MTH3D_M_vCopyVector(&((MatDest)->stCol_1),&((MatA)->stCol_1) ); \
		MTH3D_M_vCopyVector(&((MatDest)->stCol_2),&((MatA)->stCol_2) ); \
	 }
#else
#define MTH3D_M_vCopyMatrix( MatDest, MatA)                             \
     {  memcpy(MatDest, MatA, sizeof(MTH3D_tdstMatrix)); }
#endif

#define MTH3D_M_vAddMatrix( MatDest, MatA, MatB)                                \
        {  MTH3D_M_vAddVector( &((MatDest)->stCol_0),                           \
                                &((MatA)->stCol_0), &((MatB)->stCol_0));        \
           MTH3D_M_vAddVector( &((MatDest)->stCol_1),                           \
                                &((MatA)->stCol_1), &((MatB)->stCol_1));        \
           MTH3D_M_vAddVector( &((MatDest)->stCol_2),                           \
                                       &((MatA)->stCol_2), &((MatB)->stCol_2)); }

#define MTH3D_M_vSubMatrix( MatDest, MatA, MatB)                                \
        {  MTH3D_M_vSubVector( &((MatDest)->stCol_0),                           \
                                &((MatA)->stCol_0), &((MatB)->stCol_0));        \
           MTH3D_M_vSubVector( &((MatDest)->stCol_1),                           \
                                &((MatA)->stCol_1), &((MatB)->stCol_1));        \
           MTH3D_M_vSubVector( &((MatDest)->stCol_2),                           \
                               &((MatA)->stCol_2), &((MatB)->stCol_2)); }

#define MTH3D_M_vNegMatrix( MatDest, MatA)                      \
        {  MTH3D_M_vNegVector( &((MatDest)->stCol_0),           \
                                &((MatA)->stCol_0));            \
           MTH3D_M_vNegVector( &((MatDest)->stCol_1),           \
                                &((MatA)->stCol_1));            \
           MTH3D_M_vNegVector( &((MatDest)->stCol_2),           \
                               &((MatA)->stCol_2)); }

#if defined(OPTIMIZED_FOR_PC_FLOATS)
	#if defined(OPTIMIZED_FOR_PC_FLOATS_WITH_ASM)
		#define MTH3D_M_vMulMatrixMatrixWithoutBuffer(MatDest, MatA, MatB)    MTH3D_M_vMulMatrixMatrixWithoutBufferASM(MatDest, MatA, MatB)
	#elif defined(OPTIMIZED_FOR_U64_ASM)
		#define MTH3D_M_vMulMatrixMatrixWithoutBuffer(MatDest, MatA, MatB)    MTH3D_M_vMulMatrixMatrixWithoutBufferU64ASM(MatDest, MatA, MatB)
	#else
		#define MTH3D_M_vMulMatrixMatrixWithoutBuffer(MatDest, MatA, MatB)    MTH3D_M_vMulMatrixMatrixWithoutBufferC(MatDest, MatA, MatB)
	#endif
#else
		#define MTH3D_M_vMulMatrixMatrixWithoutBuffer(MatDest, MatA, MatB)    MTH3D_M_vMulMatrixMatrixWithoutBufferORG(MatDest, MatA, MatB)
#endif

#if defined(OPTIMIZED_FOR_PC_FLOATS)
	#if defined(OPTIMIZED_FOR_PC_FLOATS_WITH_ASM)
		#define MTH3D_M_vMulMatrixMatrix(MatDest, MatA, MatB)    MTH3D_M_vMulMatrixMatrixASM(MatDest, MatA, MatB)
	#else
		#define MTH3D_M_vMulMatrixMatrix(MatDest, MatA, MatB)    MTH3D_M_vMulMatrixMatrixC(MatDest, MatA, MatB)
	#endif
#else
		#define MTH3D_M_vMulMatrixMatrix(MatDest, MatA, MatB)    MTH3D_M_vMulMatrixMatrixORG(MatDest, MatA, MatB)
#endif

#define MTH3D_M_vMulTranspMatrixMatrixWithoutBuffer(MatDest, MatA, MatB)                \
    {                                                                                   \
            (MatDest)->stCol_0.xX =  MTH_M_xMulAddMulAddMul(                            \
                (MatA)->stCol_0.xX, (MatB)->stCol_0.xX,                                 \
                (MatA)->stCol_0.xY, (MatB)->stCol_0.xY,                                 \
                (MatA)->stCol_0.xZ, (MatB)->stCol_0.xZ );                               \
            (MatDest)->stCol_0.xY =  MTH_M_xMulAddMulAddMul(                            \
                (MatA)->stCol_1.xX, (MatB)->stCol_0.xX,                                 \
                (MatA)->stCol_1.xY, (MatB)->stCol_0.xY,                                 \
                (MatA)->stCol_1.xZ, (MatB)->stCol_0.xZ );                               \
            (MatDest)->stCol_0.xZ =  MTH_M_xMulAddMulAddMul(                            \
                (MatA)->stCol_2.xX, (MatB)->stCol_0.xX,                                 \
                (MatA)->stCol_2.xY, (MatB)->stCol_0.xY,                                 \
                (MatA)->stCol_2.xZ, (MatB)->stCol_0.xZ );                               \
                                                                                        \
            (MatDest)->stCol_1.xX =  MTH_M_xMulAddMulAddMul(                            \
                (MatA)->stCol_0.xX, (MatB)->stCol_1.xX,                                 \
                (MatA)->stCol_0.xY, (MatB)->stCol_1.xY,                                 \
                (MatA)->stCol_0.xZ, (MatB)->stCol_1.xZ );                               \
            (MatDest)->stCol_1.xY =  MTH_M_xMulAddMulAddMul(                            \
                (MatA)->stCol_1.xX, (MatB)->stCol_1.xX,                                 \
                (MatA)->stCol_1.xY, (MatB)->stCol_1.xY,                                 \
                (MatA)->stCol_1.xZ, (MatB)->stCol_1.xZ );                               \
            (MatDest)->stCol_1.xZ =  MTH_M_xMulAddMulAddMul(                            \
                (MatA)->stCol_2.xX, (MatB)->stCol_1.xX,                                 \
                (MatA)->stCol_2.xY, (MatB)->stCol_1.xY,                                 \
                (MatA)->stCol_2.xZ, (MatB)->stCol_1.xZ );                               \
                                                                                        \
            (MatDest)->stCol_2.xX =  MTH_M_xMulAddMulAddMul(                            \
                (MatA)->stCol_0.xX, (MatB)->stCol_2.xX,                                 \
                (MatA)->stCol_0.xY, (MatB)->stCol_2.xY,                                 \
                (MatA)->stCol_0.xZ, (MatB)->stCol_2.xZ );                               \
            (MatDest)->stCol_2.xY =  MTH_M_xMulAddMulAddMul(                            \
                (MatA)->stCol_1.xX, (MatB)->stCol_2.xX,                                 \
                (MatA)->stCol_1.xY, (MatB)->stCol_2.xY,                                 \
                (MatA)->stCol_1.xZ, (MatB)->stCol_2.xZ );                               \
            (MatDest)->stCol_2.xZ =  MTH_M_xMulAddMulAddMul(                            \
                (MatA)->stCol_2.xX, (MatB)->stCol_2.xX,                                 \
                (MatA)->stCol_2.xY, (MatB)->stCol_2.xY,                                 \
                (MatA)->stCol_2.xZ, (MatB)->stCol_2.xZ );                               \
      }

#define MTH3D_M_vMulTranspMatrixMatrix(Mat_Dest, Mat_A, Mat_B)          \
            if( (Mat_Dest==Mat_A) || (Mat_Dest==Mat_B) )                \
            {                                                           \
              MTH3D_tdstMatrix Mtemp;                                   \
                                                                        \
              MTH3D_M_vMulTranspMatrixMatrixWithoutBuffer(              \
                                        &Mtemp, Mat_A, Mat_B);          \
              MTH3D_M_vCopyMatrix( Mat_Dest, &Mtemp);                   \
            }                                                           \
            else                                                        \
            {                                                           \
             MTH3D_M_vMulTranspMatrixMatrixWithoutBuffer(Mat_Dest,      \
                                              Mat_A,  Mat_B);           \
             }

#if defined(OPTIMIZED_FOR_PC_FLOATS)

#define MTH3D_M_vMulScalarMatrix(MatDest, a, MatA)											   \
   {	    register MTH_tdxReal xTempMTH3D_M_vMulScalarMatrix=(a);													   \
		    MTH3D_M_vMulScalarVector( &((MatDest)->stCol_0), xTempMTH3D_M_vMulScalarMatrix, &((MatA)->stCol_0));       \
            MTH3D_M_vMulScalarVector( &((MatDest)->stCol_1), xTempMTH3D_M_vMulScalarMatrix, &((MatA)->stCol_1));       \
            MTH3D_M_vMulScalarVector( &((MatDest)->stCol_2), xTempMTH3D_M_vMulScalarMatrix, &((MatA)->stCol_2)); }
#else
#define MTH3D_M_vMulScalarMatrix(MatDest, a, MatA)											   \
   {	    MTH3D_M_vMulScalarVector( &((MatDest)->stCol_0), (a), &((MatA)->stCol_0));         \
            MTH3D_M_vMulScalarVector( &((MatDest)->stCol_1), (a), &((MatA)->stCol_1));         \
            MTH3D_M_vMulScalarVector( &((MatDest)->stCol_2), (a), &((MatA)->stCol_2)); }
#endif

#if defined(OPTIMIZED_FOR_PC_FLOATS)

#define MTH3D_M_vDivScalarMatrix(MatDest, MatA, a)											   \
			MTH3D_M_vMulScalarMatrix(MatDest, MTH_M_xDiv(MTH_C_ONE,(a)), MatA);
#else
#define MTH3D_M_vDivScalarMatrix(MatDest, MatA, a)                              \
   {                    MTH3D_M_vDivScalarVector( &((MatDest)->stCol_0),        \
                                        &((MatA)->stCol_0), (a));               \
                        MTH3D_M_vDivScalarVector( &((MatDest)->stCol_1),        \
                                        &((MatA)->stCol_1), (a));               \
                        MTH3D_M_vDivScalarVector( &((MatDest)->stCol_2),        \
                                                  &((MatA)->stCol_2), (a)); }
#endif

#define MTH3D_M_vSqrMatrix(MatDest, MatA)       { MTH3D_M_vMulMatrixMatrix( MatDest, MatA, MatA); }

#define MTH3D_M_vTranspMatrixWithoutBuffer(MatDest, MatA)               \
        {   (MatDest)->stCol_0.xX = (MatA)->stCol_0.xX;                 \
            (MatDest)->stCol_0.xY = (MatA)->stCol_1.xX;                 \
            (MatDest)->stCol_0.xZ = (MatA)->stCol_2.xX;                 \
            (MatDest)->stCol_1.xX = (MatA)->stCol_0.xY;                 \
            (MatDest)->stCol_1.xY = (MatA)->stCol_1.xY;                 \
            (MatDest)->stCol_1.xZ = (MatA)->stCol_2.xY;                 \
            (MatDest)->stCol_2.xX = (MatA)->stCol_0.xZ;                 \
            (MatDest)->stCol_2.xY = (MatA)->stCol_1.xZ;                 \
            (MatDest)->stCol_2.xZ = (MatA)->stCol_2.xZ; }

#define MTH3D_M_vTranpMatrixWithoutBuffer MTH3D_M_vTranspMatrixWithoutBuffer

#if defined(OPTIMIZED_FOR_PC_FLOATS)

#define MTH3D_M_vTranspMatrix(Mat_Dest, Mat_A)                    \
            if( Mat_Dest==Mat_A )                                 \
            {                                                     \
			  long lTemp0=*((long*) &((Mat_Dest)->stCol_0.xY));	  \
			  long lTemp1=*((long*) &((Mat_Dest)->stCol_0.xZ));	  \
			  long lTemp2=*((long*) &((Mat_Dest)->stCol_1.xZ));   \
			  (Mat_Dest)->stCol_0.xY=(Mat_Dest)->stCol_1.xX;	  \
			  (Mat_Dest)->stCol_0.xZ=(Mat_Dest)->stCol_2.xX;	  \
			  (Mat_Dest)->stCol_1.xZ=(Mat_Dest)->stCol_2.xY;	  \
			  *((long*) &((Mat_Dest)->stCol_1.xX))=lTemp0;		  \
			  *((long*) &((Mat_Dest)->stCol_2.xX))=lTemp1;		  \
			  *((long*) &((Mat_Dest)->stCol_2.xY))=lTemp2;		  \
            }                                                     \
            else                                                  \
            {                                                     \
	             MTH3D_M_vTranspMatrixWithoutBuffer(Mat_Dest, Mat_A); \
            }
#else
#define MTH3D_M_vTranspMatrix(Mat_Dest, Mat_A)                    \
            if( Mat_Dest==Mat_A )                                 \
            {                                                     \
              MTH3D_tdstMatrix Mtemp;                             \
                                                                  \
              MTH3D_M_vTranspMatrixWithoutBuffer(&Mtemp, Mat_A);  \
              MTH3D_M_vCopyMatrix( Mat_Dest, &Mtemp);             \
            }                                                     \
            else                                                  \
            {                                                     \
	             MTH3D_M_vTranspMatrixWithoutBuffer(Mat_Dest, Mat_A); \
            }
#endif

#define MTH3D_M_vTranpMatrix MTH3D_M_vTranspMatrix

#define MTH3D_M_xTraceMatrix(MatA)    MTH_M_xAdd3((MatA)->stCol_0.xX, (MatA)->stCol_1.xY, (MatA)->stCol_2.xZ)

#define MTH3D_M_vSetIdentityMatrix(MatDest)                                     \
        {   (MatDest)->stCol_0.xX = MTH_C_ONE;                                  \
            (MatDest)->stCol_0.xY = MTH_C_ZERO;                                 \
            (MatDest)->stCol_0.xZ = MTH_C_ZERO;                                 \
            (MatDest)->stCol_1.xX = MTH_C_ZERO;                                 \
            (MatDest)->stCol_1.xY = MTH_C_ONE;                                  \
            (MatDest)->stCol_1.xZ = MTH_C_ZERO;                                 \
            (MatDest)->stCol_2.xX = MTH_C_ZERO;                                 \
            (MatDest)->stCol_2.xY = MTH_C_ZERO;                                 \
            (MatDest)->stCol_2.xZ = MTH_C_ONE; }

#define MTH3D_M_bEqualMatrix(MatA, MatB)                                \
                        (                                               \
            MTH_M_bEqual( (MatA)->stCol_0.xX, (MatB)->stCol_0.xX) &&    \
            MTH_M_bEqual( (MatA)->stCol_0.xY, (MatB)->stCol_0.xY) &&    \
            MTH_M_bEqual( (MatA)->stCol_0.xZ, (MatB)->stCol_0.xZ) &&    \
            MTH_M_bEqual( (MatA)->stCol_1.xX, (MatB)->stCol_1.xX) &&    \
            MTH_M_bEqual( (MatA)->stCol_1.xY, (MatB)->stCol_1.xY) &&    \
            MTH_M_bEqual( (MatA)->stCol_1.xZ, (MatB)->stCol_1.xZ) &&    \
            MTH_M_bEqual( (MatA)->stCol_2.xX, (MatB)->stCol_2.xX) &&    \
            MTH_M_bEqual( (MatA)->stCol_2.xY, (MatB)->stCol_2.xY) &&    \
            MTH_M_bEqual( (MatA)->stCol_2.xZ, (MatB)->stCol_2.xZ)       \
            )

#define MTH3D_M_bMatchMatrix(MatA, MatB, eps)                                           \
                        (                                                               \
            MTH_M_bEqualWithEpsilon( (MatA)->stCol_0.xX, (MatB)->stCol_0.xX, eps) &&    \
            MTH_M_bEqualWithEpsilon( (MatA)->stCol_0.xY, (MatB)->stCol_0.xY, eps) &&    \
            MTH_M_bEqualWithEpsilon( (MatA)->stCol_0.xZ, (MatB)->stCol_0.xZ, eps) &&    \
            MTH_M_bEqualWithEpsilon( (MatA)->stCol_1.xX, (MatB)->stCol_1.xX, eps) &&    \
            MTH_M_bEqualWithEpsilon( (MatA)->stCol_1.xY, (MatB)->stCol_1.xY, eps) &&    \
            MTH_M_bEqualWithEpsilon( (MatA)->stCol_1.xZ, (MatB)->stCol_1.xZ, eps) &&    \
            MTH_M_bEqualWithEpsilon( (MatA)->stCol_2.xX, (MatB)->stCol_2.xX, eps) &&    \
            MTH_M_bEqualWithEpsilon( (MatA)->stCol_2.xY, (MatB)->stCol_2.xY, eps) &&    \
            MTH_M_bEqualWithEpsilon( (MatA)->stCol_2.xZ, (MatB)->stCol_2.xZ, eps)       \
            )

#if defined(OPTIMIZED_FOR_PC_FLOATS)

#define MTH3D_M_vFillMatrix(MatDest, b)																 \
    {		register MTH_tdxReal a=(b);															 \
            (MatDest)->stCol_0.xX = (a); (MatDest)->stCol_0.xY = (a); (MatDest)->stCol_0.xZ = (a);   \
            (MatDest)->stCol_1.xX = (a); (MatDest)->stCol_1.xY = (a); (MatDest)->stCol_1.xZ = (a);   \
            (MatDest)->stCol_2.xX = (a); (MatDest)->stCol_2.xY = (a); (MatDest)->stCol_2.xZ = (a);   \
    }
#else
#define MTH3D_M_vFillMatrix(MatDest, a)																 \
    {       (MatDest)->stCol_0.xX = (a); (MatDest)->stCol_0.xY = (a); (MatDest)->stCol_0.xZ = (a);   \
            (MatDest)->stCol_1.xX = (a); (MatDest)->stCol_1.xY = (a); (MatDest)->stCol_1.xZ = (a);   \
			(MatDest)->stCol_2.xX = (a); (MatDest)->stCol_2.xY = (a); (MatDest)->stCol_2.xZ = (a);   }
#endif

#if defined(OPTIMIZED_FOR_PC_FLOATS)
	#if defined(OPTIMIZED_FOR_PC_FLOATS_WITH_ASM)
		#define MTH3D_M_xDetMatrix(MatA)				MTH3D_M_xDetMatrixASM(MatA)
	#else
		#define MTH3D_M_xDetMatrix(MatA)				MTH3D_M_xDetMatrixC(MatA)
	#endif
#else
	#define MTH3D_M_xDetMatrix(MatA)					MTH3D_M_xDetMatrixORG(MatA)
#endif

#if defined(OPTIMIZED_FOR_PC_FLOATS)

#define MTH3D_M_vComMatrixWithoutBuffer(MatDest, MatA)                                                                                         \
    {       (MatDest)->stCol_0.xX= MTH_M_xMulSubMul((MatA)->stCol_1.xY,(MatA)->stCol_2.xZ,(MatA)->stCol_1.xZ,(MatA)->stCol_2.xY);              \
			(MatDest)->stCol_0.xY= MTH_M_xMulSubMul((MatA)->stCol_1.xZ,(MatA)->stCol_2.xX,(MatA)->stCol_1.xX,(MatA)->stCol_2.xZ);			   \
			(MatDest)->stCol_0.xZ= MTH_M_xMulSubMul((MatA)->stCol_1.xX,(MatA)->stCol_2.xY,(MatA)->stCol_1.xY,(MatA)->stCol_2.xX);              \
			(MatDest)->stCol_1.xX= MTH_M_xMulSubMul((MatA)->stCol_0.xZ,(MatA)->stCol_2.xY,(MatA)->stCol_0.xY,(MatA)->stCol_2.xZ);			   \
			(MatDest)->stCol_1.xY= MTH_M_xMulSubMul((MatA)->stCol_0.xX,(MatA)->stCol_2.xZ,(MatA)->stCol_0.xZ,(MatA)->stCol_2.xX);              \
		    (MatDest)->stCol_1.xZ= MTH_M_xMulSubMul((MatA)->stCol_0.xY,(MatA)->stCol_2.xX,(MatA)->stCol_0.xX,(MatA)->stCol_2.xY);			   \
			(MatDest)->stCol_2.xX= MTH_M_xMulSubMul((MatA)->stCol_0.xY,(MatA)->stCol_1.xZ,(MatA)->stCol_0.xZ,(MatA)->stCol_1.xY);              \
            (MatDest)->stCol_2.xY= MTH_M_xMulSubMul((MatA)->stCol_0.xZ,(MatA)->stCol_1.xX,(MatA)->stCol_0.xX,(MatA)->stCol_1.xZ);			   \
			(MatDest)->stCol_2.xZ= MTH_M_xMulSubMul((MatA)->stCol_0.xX,(MatA)->stCol_1.xY,(MatA)->stCol_0.xY,(MatA)->stCol_1.xX); }
#else
#define MTH3D_M_vComMatrixWithoutBuffer(MatDest, MatA)                                                                                         \
    {       (MatDest)->stCol_0.xX= MTH_M_xMulSubMul((MatA)->stCol_1.xY,(MatA)->stCol_2.xZ,(MatA)->stCol_1.xZ,(MatA)->stCol_2.xY);              \
            (MatDest)->stCol_0.xY= MTH_M_xNeg( MTH_M_xMulSubMul((MatA)->stCol_1.xX,(MatA)->stCol_2.xZ,(MatA)->stCol_1.xZ,(MatA)->stCol_2.xX)); \
            (MatDest)->stCol_0.xZ= MTH_M_xMulSubMul((MatA)->stCol_1.xX,(MatA)->stCol_2.xY,(MatA)->stCol_1.xY,(MatA)->stCol_2.xX);              \
            (MatDest)->stCol_1.xX= MTH_M_xNeg(MTH_M_xMulSubMul((MatA)->stCol_0.xY,(MatA)->stCol_2.xZ,(MatA)->stCol_0.xZ,(MatA)->stCol_2.xY));  \
            (MatDest)->stCol_1.xY= MTH_M_xMulSubMul((MatA)->stCol_0.xX,(MatA)->stCol_2.xZ,(MatA)->stCol_0.xZ,(MatA)->stCol_2.xX);              \
            (MatDest)->stCol_1.xZ= MTH_M_xNeg(MTH_M_xMulSubMul((MatA)->stCol_0.xX,(MatA)->stCol_2.xY,(MatA)->stCol_0.xY,(MatA)->stCol_2.xX));  \
            (MatDest)->stCol_2.xX= MTH_M_xMulSubMul((MatA)->stCol_0.xY,(MatA)->stCol_1.xZ,(MatA)->stCol_0.xZ,(MatA)->stCol_1.xY);              \
            (MatDest)->stCol_2.xY= MTH_M_xNeg( MTH_M_xMulSubMul((MatA)->stCol_0.xX,(MatA)->stCol_1.xZ,(MatA)->stCol_0.xZ,(MatA)->stCol_1.xX)); \
            (MatDest)->stCol_2.xZ= MTH_M_xMulSubMul((MatA)->stCol_0.xX,(MatA)->stCol_1.xY,(MatA)->stCol_0.xY,(MatA)->stCol_1.xX); }
#endif

#if defined(OPTIMIZED_FOR_PC_FLOATS)

#define MTH3D_M_vTransComMatrixWithoutBuffer(MatDest, MatA)                                                                                    \
    {       (MatDest)->stCol_0.xX= MTH_M_xMulSubMul((MatA)->stCol_1.xY,(MatA)->stCol_2.xZ,(MatA)->stCol_1.xZ,(MatA)->stCol_2.xY);              \
			(MatDest)->stCol_0.xY= MTH_M_xMulSubMul((MatA)->stCol_0.xZ,(MatA)->stCol_2.xY,(MatA)->stCol_0.xY,(MatA)->stCol_2.xZ);			   \
			(MatDest)->stCol_0.xZ= MTH_M_xMulSubMul((MatA)->stCol_0.xY,(MatA)->stCol_1.xZ,(MatA)->stCol_0.xZ,(MatA)->stCol_1.xY);              \
			(MatDest)->stCol_1.xX= MTH_M_xMulSubMul((MatA)->stCol_1.xZ,(MatA)->stCol_2.xX,(MatA)->stCol_1.xX,(MatA)->stCol_2.xZ);			   \
			(MatDest)->stCol_1.xY= MTH_M_xMulSubMul((MatA)->stCol_0.xX,(MatA)->stCol_2.xZ,(MatA)->stCol_0.xZ,(MatA)->stCol_2.xX);              \
            (MatDest)->stCol_1.xZ= MTH_M_xMulSubMul((MatA)->stCol_0.xZ,(MatA)->stCol_1.xX,(MatA)->stCol_0.xX,(MatA)->stCol_1.xZ);			   \
			(MatDest)->stCol_2.xX= MTH_M_xMulSubMul((MatA)->stCol_1.xX,(MatA)->stCol_2.xY,(MatA)->stCol_1.xY,(MatA)->stCol_2.xX);              \
		    (MatDest)->stCol_2.xY= MTH_M_xMulSubMul((MatA)->stCol_0.xY,(MatA)->stCol_2.xX,(MatA)->stCol_0.xX,(MatA)->stCol_2.xY);			   \
			(MatDest)->stCol_2.xZ= MTH_M_xMulSubMul((MatA)->stCol_0.xX,(MatA)->stCol_1.xY,(MatA)->stCol_0.xY,(MatA)->stCol_1.xX); }
#else
#define MTH3D_M_vTransComMatrixWithoutBuffer(MatDest, MatA)                                                                                    \
    {       (MatDest)->stCol_0.xX= MTH_M_xMulSubMul((MatA)->stCol_1.xY,(MatA)->stCol_2.xZ,(MatA)->stCol_1.xZ,(MatA)->stCol_2.xY);              \
            (MatDest)->stCol_1.xX= MTH_M_xNeg( MTH_M_xMulSubMul((MatA)->stCol_1.xX,(MatA)->stCol_2.xZ,(MatA)->stCol_1.xZ,(MatA)->stCol_2.xX)); \
            (MatDest)->stCol_2.xX= MTH_M_xMulSubMul((MatA)->stCol_1.xX,(MatA)->stCol_2.xY,(MatA)->stCol_1.xY,(MatA)->stCol_2.xX);              \
            (MatDest)->stCol_0.xY= MTH_M_xNeg(MTH_M_xMulSubMul((MatA)->stCol_0.xY,(MatA)->stCol_2.xZ,(MatA)->stCol_0.xZ,(MatA)->stCol_2.xY));  \
            (MatDest)->stCol_1.xY= MTH_M_xMulSubMul((MatA)->stCol_0.xX,(MatA)->stCol_2.xZ,(MatA)->stCol_0.xZ,(MatA)->stCol_2.xX);              \
            (MatDest)->stCol_2.xY= MTH_M_xNeg(MTH_M_xMulSubMul((MatA)->stCol_0.xX,(MatA)->stCol_2.xY,(MatA)->stCol_0.xY,(MatA)->stCol_2.xX));  \
            (MatDest)->stCol_0.xZ= MTH_M_xMulSubMul((MatA)->stCol_0.xY,(MatA)->stCol_1.xZ,(MatA)->stCol_0.xZ,(MatA)->stCol_1.xY);              \
            (MatDest)->stCol_1.xZ= MTH_M_xNeg( MTH_M_xMulSubMul((MatA)->stCol_0.xX,(MatA)->stCol_1.xZ,(MatA)->stCol_0.xZ,(MatA)->stCol_1.xX)); \
            (MatDest)->stCol_2.xZ= MTH_M_xMulSubMul((MatA)->stCol_0.xX,(MatA)->stCol_1.xY,(MatA)->stCol_0.xY,(MatA)->stCol_1.xX); }
#endif

#define MTH3D_M_vTranspComMatrixWithoutBuffer(MatDest, MatA)   MTH3D_M_vTransComMatrixWithoutBuffer(MatDest, MatA)                                                                                      \

#if defined(OPTIMIZED_FOR_PC_FLOATS)
	#if defined(OPTIMIZED_FOR_PC_FLOATS_WITH_ASM)
		#define MTH3D_M_vInverMatrix(MatDest, MatA)		MTH3D_M_vInverMatrixASM(MatDest, MatA)
	#else
		#define MTH3D_M_vInverMatrix(MatDest, MatA)		MTH3D_M_vInverMatrixC(MatDest, MatA)
	#endif
#else
	#define MTH3D_M_vInverMatrix(MatDest, MatA)			MTH3D_M_vInverMatrixORG(MatDest, MatA)
#endif

#define MTH3D_M_vGetMatrixColumnElements( Vx, Vy, Vz, col, MatA)                            \
      if( (col)==0 )                                                                        \
        { *Vx= (MatA)->stCol_0.xX; *Vy= (MatA)->stCol_0.xY; *Vz= (MatA)->stCol_0.xZ; }      \
      else                                                                                  \
        if( (col)==1 )                                                                      \
          { *Vx= (MatA)->stCol_1.xX; *Vy= (MatA)->stCol_1.xY; *Vz= (MatA)->stCol_1.xZ;  }   \
        else                                                                                \
          { *Vx= (MatA)->stCol_2.xX; *Vy= (MatA)->stCol_2.xY; *Vz= (MatA)->stCol_2.xZ;  }

#define MTH3D_M_vSetMatrixColumnElements( MatDest, Vx, Vy, Vz, col)                                   \
      if( (col)==0 )                                                                                  \
        {(MatDest)->stCol_0.xX= (Vx); (MatDest)->stCol_0.xY= (Vy); (MatDest)->stCol_0.xZ= (Vz); }     \
      else                                                                                            \
        if( (col)==1 )                                                                                \
         {(MatDest)->stCol_1.xX= (Vx); (MatDest)->stCol_1.xY= (Vy); (MatDest)->stCol_1.xZ= (Vz); }    \
        else                                                                                          \
         {(MatDest)->stCol_2.xX= (Vx); (MatDest)->stCol_2.xY= (Vy); (MatDest)->stCol_2.xZ= (Vz); }

#define MTH3D_M_xGetMatrixElement( lin, col, MatA)                                                                        \
      ( (col)==0 ) ?                                                                                                      \
      ( ( (lin)==0 ) ? (MatA)->stCol_0.xX : ( ( (lin)==1 ) ? (MatA)->stCol_0.xY : (MatA)->stCol_0.xZ ) ) :    \
      (                                                                                                                   \
       ( (col)==1 ) ?                                                                                                     \
         ( ( (lin)==0 ) ? (MatA)->stCol_1.xX : ( ( (lin)==1 ) ? (MatA)->stCol_1.xY : (MatA)->stCol_1.xZ ) ) : \
         ( ( (lin)==0 ) ? (MatA)->stCol_2.xX : ( ( (lin)==1 ) ? (MatA)->stCol_2.xY : (MatA)->stCol_2.xZ ) )   \
      )

#define MTH3D_M_vSetMatrixElement( MatDest, a, lin, col)                                                                  \
 if( (col)==0 )                                                                                                           \
 { if((lin)==0){(MatDest)->stCol_0.xX=(a);}else if((lin)==1){(MatDest)->stCol_0.xY=(a);}else{(MatDest)->stCol_0.xZ=(a);}} \
 else                                                                                                                     \
  if( (col)==1)                                                                                                           \
 { if((lin)==0){(MatDest)->stCol_1.xX=(a);}else if((lin)==1){(MatDest)->stCol_1.xY=(a);}else{(MatDest)->stCol_1.xZ=(a);}} \
  else                                                                                                                    \
 { if((lin)==0){(MatDest)->stCol_2.xX=(a);}else if((lin)==1){(MatDest)->stCol_2.xY=(a);}else{(MatDest)->stCol_2.xZ=(a);}}

#define MTH3D_M_vSetMatrixCol0Elements( MatDest, Vx, Vy, Vz)                                    \
            {(MatDest)->stCol_0.xX= (Vx); (MatDest)->stCol_0.xY= (Vy); (MatDest)->stCol_0.xZ= (Vz); }

#define MTH3D_M_vSetMatrixCol1Elements( MatDest, Vx, Vy, Vz)                                    \
            {(MatDest)->stCol_1.xX= (Vx); (MatDest)->stCol_1.xY= (Vy); (MatDest)->stCol_1.xZ= (Vz); }

#define MTH3D_M_vSetMatrixCol2Elements( MatDest, Vx, Vy, Vz)                                    \
            {(MatDest)->stCol_2.xX= (Vx); (MatDest)->stCol_2.xY= (Vy); (MatDest)->stCol_2.xZ= (Vz); }

#define MTH3D_M_vSetMatrixLineXElements( MatDest, V0, V1, V2)                                   \
            {(MatDest)->stCol_0.xX= (V0); (MatDest)->stCol_1.xX= (V1); (MatDest)->stCol_2.xX= (V2); }

#define MTH3D_M_vSetMatrixLineYElements( MatDest, V0, V1, V2)                                   \
            {(MatDest)->stCol_0.xY= (V0); (MatDest)->stCol_1.xY= (V1); (MatDest)->stCol_2.xY= (V2); }

#define MTH3D_M_vSetMatrixLineZElements( MatDest, V0, V1, V2)                                   \
            {(MatDest)->stCol_0.xZ= (V0); (MatDest)->stCol_1.xZ= (V1); (MatDest)->stCol_2.xZ= (V2); }

#define MTH3D_M_vGetMatrixCol0Elements( Vx, Vy, Vz, MatA)                                       \
            {Vx= (MatA)->stCol_0.xX; Vy= (MatA)->stCol_0.xY; Vz= (MatA)->stCol_0.xZ; }

#define MTH3D_M_vGetMatrixCol1Elements( Vx, Vy, Vz, MatA)                                       \
            {Vx= (MatA)->stCol_1.xX; Vy= (MatA)->stCol_1.xY; Vz= (MatA)->stCol_1.xZ; }

#define MTH3D_M_vGetMatrixCol2Elements( Vx, Vy, Vz, MatA)                                       \
            {Vx= (MatA)->stCol_2.xX; Vy= (MatA)->stCol_2.xY; Vz= (MatA)->stCol_2.xZ; }

#define MTH3D_M_vGetMatrixLineXElements( V0, V1, V2, MatA)                                      \
            {V0= (MatA)->stCol_0.xX; V1= (MatA)->stCol_1.xX; V2= (MatA)->stCol_2.xX; }

#define MTH3D_M_vGetMatrixLineYElements( V0, V1, V2, MatA)                                      \
            {V0= (MatA)->stCol_0.xY; V1= (MatA)->stCol_1.xY; V2= (MatA)->stCol_2.xY; }

#define MTH3D_M_vGetMatrixLineZElements( V0, V1, V2, MatA)                                      \
            {V0= (MatA)->stCol_0.xZ; V1= (MatA)->stCol_1.xZ; V2= (MatA)->stCol_2.xZ; }

#if defined(OPTIMIZED_FOR_PC_FLOATS)
	#if defined(OPTIMIZED_FOR_PC_FLOATS_WITH_ASM)
		#define MTH3D_M_vMulMatrixVectorWithoutBuffer( VectDest, MatA, VectA)	MTH3D_M_vMulMatrixVectorWithoutBufferASM(VectDest, MatA, VectA)
	#elif defined(OPTIMIZED_FOR_U64_ASM)
		#define MTH3D_M_vMulMatrixVectorWithoutBuffer( VectDest, MatA, VectA)	MTH3D_M_vMulMatrixVectorWithoutBufferU64ASM(VectDest, MatA, VectA)
	#else
		#define MTH3D_M_vMulMatrixVectorWithoutBuffer( VectDest, MatA, VectA)	MTH3D_M_vMulMatrixVectorWithoutBufferC(VectDest, MatA, VectA)
	#endif
#else
	#define MTH3D_M_vMulMatrixVectorWithoutBuffer( VectDest, MatA, VectA)		MTH3D_M_vMulMatrixVectorWithoutBufferORG(VectDest, MatA, VectA)
#endif

#if defined(OPTIMIZED_FOR_PC_FLOATS)
	#if defined(OPTIMIZED_FOR_PC_FLOATS_WITH_ASM)
		#define MTH3D_M_vMulMatrixVector( VectDest, MatA, VectA)	MTH3D_M_vMulMatrixVectorASM(VectDest, MatA, VectA)
	#elif defined(OPTIMIZED_FOR_U64_ASM)
		#define MTH3D_M_vMulMatrixVector( VectDest, MatA, VectA)	MTH3D_M_vMulMatrixVectorU64ASM(VectDest, MatA, VectA)
	#else
		#define MTH3D_M_vMulMatrixVector( VectDest, MatA, VectA)	MTH3D_M_vMulMatrixVectorC(VectDest, MatA, VectA)
	#endif
#else
	#define MTH3D_M_vMulMatrixVector( VectDest, MatA, VectA)		MTH3D_M_vMulMatrixVectorORG(VectDest, MatA, VectA)
#endif

#define MTH3D_M_vGetColumn0InMatrix( VectDest, MatA)                       \
            { (VectDest)->xX=(MatA)->stCol_0.xX;                                \
              (VectDest)->xY=(MatA)->stCol_0.xY;                                \
              (VectDest)->xZ=(MatA)->stCol_0.xZ; }

#define MTH3D_M_vGetColumn1InMatrix( VectDest, MatA)                       \
             { (VectDest)->xX=(MatA)->stCol_1.xX;                               \
               (VectDest)->xY=(MatA)->stCol_1.xY;                               \
               (VectDest)->xZ=(MatA)->stCol_1.xZ; }

#define MTH3D_M_vGetColumn2InMatrix( VectDest, MatA)                       \
             { (VectDest)->xX=(MatA)->stCol_2.xX;                               \
               (VectDest)->xY=(MatA)->stCol_2.xY;                               \
               (VectDest)->xZ=(MatA)->stCol_2.xZ; }

#define MTH3D_M_vGetColumnInMatrix( VectDest, MatA, col )                       \
            if( col==0 )                                                        \
            { (VectDest)->xX=(MatA)->stCol_0.xX;                                \
              (VectDest)->xY=(MatA)->stCol_0.xY;                                \
              (VectDest)->xZ=(MatA)->stCol_0.xZ; }                              \
            else                                                                \
             if( col==1 )                                                       \
             { (VectDest)->xX=(MatA)->stCol_1.xX;                               \
               (VectDest)->xY=(MatA)->stCol_1.xY;                               \
               (VectDest)->xZ=(MatA)->stCol_1.xZ; }                             \
             else                                                               \
             { (VectDest)->xX=(MatA)->stCol_2.xX;                               \
               (VectDest)->xY=(MatA)->stCol_2.xY;                               \
               (VectDest)->xZ=(MatA)->stCol_2.xZ; }

#define MTH3D_M_vSetColumnInMatrix( MatDest, VectA, col)                        \
            if( col==0 )                                                        \
            { (MatDest)->stCol_0.xX=(VectA)->xX;                                \
              (MatDest)->stCol_0.xY=(VectA)->xY;                                \
              (MatDest)->stCol_0.xZ=(VectA)->xZ; }                              \
            else                                                                \
             if( col==1 )                                                       \
             { (MatDest)->stCol_1.xX=(VectA)->xX;                               \
               (MatDest)->stCol_1.xY=(VectA)->xY;                               \
               (MatDest)->stCol_1.xZ=(VectA)->xZ; }                             \
             else                                                               \
             { (MatDest)->stCol_2.xX=(VectA)->xX;                               \
               (MatDest)->stCol_2.xY=(VectA)->xY;                               \
               (MatDest)->stCol_2.xZ=(VectA)->xZ; }

#define MTH3D_M_vGetLineInMatrix( VectDest, MatA, lin )                         \
            if( lin==0 )                                                        \
            { (VectDest)->xX=(MatA)->stCol_0.xX;                                \
              (VectDest)->xY=(MatA)->stCol_1.xX;                                \
              (VectDest)->xZ=(MatA)->stCol_2.xX; }                              \
            else                                                                \
             if( lin==1 )                                                       \
             { (VectDest)->xX=(MatA)->stCol_0.xY;                               \
               (VectDest)->xY=(MatA)->stCol_1.xY;                               \
               (VectDest)->xZ=(MatA)->stCol_2.xY; }                             \
             else                                                               \
             { (VectDest)->xX=(MatA)->stCol_0.xZ;                               \
               (VectDest)->xY=(MatA)->stCol_1.xZ;                               \
               (VectDest)->xZ=(MatA)->stCol_2.xZ; }

#define MTH3D_M_vSetLineInMatrix( MatDest, VectA, lin)                          \
            if( lin==0 )                                                        \
            { (MatDest)->stCol_0.xX=(VectA)->xX;                                \
              (MatDest)->stCol_1.xX=(VectA)->xY;                                \
              (MatDest)->stCol_2.xX=(VectA)->xZ; }                              \
            else                                                                \
             if( lin==1 )                                                       \
             { (MatDest)->stCol_0.xY=(VectA)->xX;                               \
               (MatDest)->stCol_1.xY=(VectA)->xY;                               \
               (MatDest)->stCol_2.xY=(VectA)->xZ; }                             \
             else                                                               \
             { (MatDest)->stCol_0.xZ=(VectA)->xX;                               \
               (MatDest)->stCol_1.xZ=(VectA)->xY;                               \
               (MatDest)->stCol_2.xZ=(VectA)->xZ; }

#define MTH3D_M_vGetDiagInMatrix( VectDest, MatA)                               \
        {               (VectDest)->xX= (MatA)->stCol_0.xX;                     \
                        (VectDest)->xY= (MatA)->stCol_1.xY;                     \
                        (VectDest)->xZ= (MatA)->stCol_2.xZ; }

#define MTH3D_M_vSetDiagInMatrix( MatDest, VectA)                               \
        {               (MatDest)->stCol_0.xX= (VectA)->xX;                     \
                        (MatDest)->stCol_1.xY= (VectA)->xY;                     \
                        (MatDest)->stCol_2.xZ= (VectA)->xZ; }

#define MTH3D_M_vGetVectorsInMatrix( VaDest, VbDest, VcDest, MatA)                      \
   {          (VaDest)->xX=(MatA)->stCol_0.xX;                                          \
              (VaDest)->xY=(MatA)->stCol_0.xY;                                          \
              (VaDest)->xZ=(MatA)->stCol_0.xZ;                                          \
              (VbDest)->xX=(MatA)->stCol_1.xX;                                          \
              (VbDest)->xY=(MatA)->stCol_1.xY;                                          \
              (VbDest)->xZ=(MatA)->stCol_1.xZ;                                          \
              (VcDest)->xX=(MatA)->stCol_2.xX;                                          \
              (VcDest)->xY=(MatA)->stCol_2.xY;                                          \
              (VcDest)->xZ=(MatA)->stCol_2.xZ; }

#define MTH3D_M_vSetVectorsInMatrix( MatDest, VectA, VectB, VectC)              \
  {          (MatDest)->stCol_0.xX=(VectA)->xX;                                 \
             (MatDest)->stCol_0.xY=(VectA)->xY;                                 \
             (MatDest)->stCol_0.xZ=(VectA)->xZ;                                 \
             (MatDest)->stCol_1.xX=(VectB)->xX;                                 \
             (MatDest)->stCol_1.xY=(VectB)->xY;                                 \
             (MatDest)->stCol_1.xZ=(VectB)->xZ;                                 \
             (MatDest)->stCol_2.xX=(VectC)->xX;                                 \
             (MatDest)->stCol_2.xY=(VectC)->xY;                                 \
             (MatDest)->stCol_2.xZ=(VectC)->xZ; }

#define MTH3D_M_vPrintfVector( Name, VectA)                                     \
        {   printf("%s = \n |%e", (Name), (VectA)->xX);                         \
            printf("\n |%e", (VectA)->xY);                                      \
            printf("\n |%e\n", (VectA)->xZ); }

#define MTH3D_M_vPrintfMatrix( Name, MatA)                                                                      \
        {   printf("%s = \n |%e %e %e|", (Name), (MatA)->stCol_0.xX, (MatA)->stCol_1.xX, (MatA)->stCol_2.xX);   \
            printf("\n |%e %e %e|", (MatA)->stCol_0.xY, (MatA)->stCol_1.xY, (MatA)->stCol_2.xY );               \
            printf("\n |%e %e %e|\n", (MatA)->stCol_0.xZ, (MatA)->stCol_1.xZ, (MatA)->stCol_2.xZ ); }

#define MTH3D_M_vPrintfReal( Name, RealA)                                     \
	{   printf("%s = \n %e\n", (Name), (RealA) ); }

#if defined(OPTIMIZED_FOR_PC_FLOATS)
	#if defined(OPTIMIZED_FOR_PC_FLOATS_WITH_ASM)
		#define MTH3D_M_vTransformVectorWithoutBuffer( VectDest, MatA, VectA, VectB)	MTH3D_M_vTransformVectorWithoutBufferASM(VectDest, MatA, VectA, VectB)
	#elif defined(OPTIMIZED_FOR_U64_ASM)
		#define MTH3D_M_vTransformVectorWithoutBuffer( VectDest, MatA, VectA, VectB)	MTH3D_M_vTransformVectorWithoutBufferU64ASM(VectDest, MatA, VectA, VectB)
	#else
		#define MTH3D_M_vTransformVectorWithoutBuffer( VectDest, MatA, VectA, VectB)	MTH3D_M_vTransformVectorWithoutBufferC(VectDest, MatA, VectA, VectB)
	#endif
#else
	#define MTH3D_M_vTransformVectorWithoutBuffer( VectDest, MatA, VectA, VectB)		MTH3D_M_vTransformVectorWithoutBufferORG(VectDest, MatA, VectA, VectB)
#endif

#include "MTH/MTH3dopt.h"

#endif
