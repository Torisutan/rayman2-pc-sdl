/*
//////////////////////////////////////////////////////////////
//	SNDADPCM_MMX_PC.C  (APMmxBVR.dll)
//////////////////////////////////////////////////////////////
//	C branch of the ADPCM driver's MMX asm (mmx1..mmx3*.asm):
//	the IMA ADPCM decoders, the voice mixer and the 16-bit
//	output, as declared in sndadpcm_pc.h. The asm is built
//	instead under OPTIMIZED_FOR_MMX_WITH_ASM.
//////////////////////////////////////////////////////////////
*/
/* Each function computes what the asm computes, byte for byte (checked against the asm on the same
 * input): its saturations (packssdw, paddsw, psubsw), its products (pmulhw, pmullw, pmaddwd), the 64-bit
 * register that holds the source bytes of a decoder (shifted as a qword or per dword), its loop shapes (an
 * MMX block of 4 frames even when fewer are left, a scalar tail, one frame past the end on odd lengths).
 * The six internal mixer parts of the asm share the frame of SND_pc_fn_vMixVoiceADPCM; here they are the
 * blocks of that function. */
#include "SNDinc.h"

#if !defined(OPTIMIZED_FOR_MMX_WITH_ASM)

#include "sndres.h"
#include "sndhard.h"
#include "sndadpcm_pc.h"
#include "sndadpcm_mmx_pc.h"
#include <stddef.h>

long	SND_pc_a_lIndexTableADPCM[16] =
{
	-1, -1, -1, -1, 2, 4, 6, 8,
	-1, -1, -1, -1, 2, 4, 6, 8
};

long	SND_pc_a_lStepTableADPCM[89] =
{
	0x7, 0x8, 0x9, 0xA, 0xB, 0xC, 0xD, 0xE,
	0x10, 0x11, 0x13, 0x15, 0x17, 0x19, 0x1C, 0x1F,
	0x22, 0x25, 0x29, 0x2D, 0x32, 0x37, 0x3C, 0x42,
	0x49, 0x50, 0x58, 0x61, 0x6B, 0x76, 0x82, 0x8F,
	0x9D, 0xAD, 0xBE, 0xD1, 0xE6, 0xFD, 0x117, 0x133,
	0x151, 0x173, 0x198, 0x1C1, 0x1EE, 0x220, 0x256, 0x292,
	0x2D4, 0x31C, 0x36C, 0x3C3, 0x424, 0x48E, 0x502, 0x583,
	0x610, 0x6AB, 0x756, 0x812, 0x8E0, 0x9C3, 0xABD, 0xBD0,
	0xCFF, 0xE4C, 0xFBA, 0x114C, 0x1307, 0x14EE, 0x1706, 0x1954,
	0x1BDC, 0x1EA5, 0x21B6, 0x2515, 0x28CA, 0x2CDF, 0x315B, 0x364B,
	0x3BB9, 0x41B2, 0x4844, 0x4F7E, 0x5771, 0x602F, 0x69CE, 0x7462,
	0x7FFF
};

/*---- register arithmetic */
#define SND_pc_M_ulLow32(ul)	((unsigned long)(ul) & 0xFFFFFFFFUL)
#define SND_pc_M_wLow16(ul)		((short)(unsigned short)((unsigned long)(ul) & 0xFFFF))

/* a 32-bit register value as a signed long */
static long SND_pc_fn_lSigned32ADPCM(unsigned long ulValue)
{
	ulValue = SND_pc_M_ulLow32(ulValue);
	return (ulValue & 0x80000000UL) ? -(long)(0xFFFFFFFFUL - ulValue) - 1 : (long)ulValue;
}

static short SND_pc_fn_wSat16ADPCM(long lValue)	/* packssdw / paddsw */
{
	return (short)(lValue > 32767 ? 32767 : (lValue < -32768 ? -32768 : lValue));
}

/* high dword of a 32 x 32 bit unsigned product (mul) */
static unsigned long SND_pc_fn_ulMulHigh32ADPCM(unsigned long ulA,unsigned long ulB)
{
	unsigned long ulALo = ulA & 0xFFFF, ulAHi = (ulA >> 16) & 0xFFFF;
	unsigned long ulBLo = ulB & 0xFFFF, ulBHi = (ulB >> 16) & 0xFFFF;
	unsigned long ulMid = (ulALo * ulBLo >> 16) + (ulAHi * ulBLo & 0xFFFF) + (ulALo * ulBHi & 0xFFFF);
	return SND_pc_M_ulLow32(ulAHi * ulBHi + (ulAHi * ulBLo >> 16) + (ulALo * ulBHi >> 16) + (ulMid >> 16));
}

/* a qword register as two dwords: psrlq (whole qword) and psrld (each dword) */
typedef struct SND_pc_tdstQwordADPCM_
{
	unsigned long	ulLo;
	unsigned long	ulHi;
} SND_pc_tdstQwordADPCM;

static SND_pc_tdstQwordADPCM SND_pc_fn_stShiftQwordADPCM(SND_pc_tdstQwordADPCM stQ,long lBits)
{
	stQ.ulLo = SND_pc_M_ulLow32((stQ.ulLo >> lBits) | (stQ.ulHi << (32 - lBits)));
	stQ.ulHi >>= lBits;
	return stQ;
}

static SND_pc_tdstQwordADPCM SND_pc_fn_stShiftDwordsADPCM(SND_pc_tdstQwordADPCM stQ,long lBits)
{
	stQ.ulLo >>= lBits;
	stQ.ulHi >>= lBits;
	return stQ;
}

/* the source dword at a 4-aligned pointer, little-endian as the data */
static unsigned long SND_pc_fn_ulReadDwordADPCM(const unsigned char *p_ucSrc)
{
	return (unsigned long)p_ucSrc[0] | ((unsigned long)p_ucSrc[1] << 8) | ((unsigned long)p_ucSrc[2] << 16)
		| ((unsigned long)p_ucSrc[3] << 24);
}

/*---- decoders */
/* new step index word: idx + delta, then clamped through the asm's biases (0..88 for the index word) */
static short SND_pc_fn_wClampIndexADPCM(short wIndex,short wDelta,short wMaxBias)
{
	short w = SND_pc_fn_wSat16ADPCM((long)wIndex + wDelta);
	w = SND_pc_fn_wSat16ADPCM((long)SND_pc_fn_wSat16ADPCM((long)w + wMaxBias) - wMaxBias);
	return SND_pc_fn_wSat16ADPCM((long)SND_pc_fn_wSat16ADPCM((long)w - 32768) + 32768);
}

static unsigned long SND_pc_fn_ulNextIndexADPCM(unsigned long ulIndex,long lTableValue)
{
	unsigned long ulDelta = (unsigned long)lTableValue;
	unsigned short uwLo = (unsigned short)SND_pc_fn_wClampIndexADPCM(SND_pc_M_wLow16(ulIndex),SND_pc_M_wLow16(ulDelta),0x7FA7);
	unsigned short uwHi = (unsigned short)SND_pc_fn_wClampIndexADPCM(SND_pc_M_wLow16(ulIndex >> 16),SND_pc_M_wLow16(ulDelta >> 16),0x7FFF);
	return ((unsigned long)uwHi << 16) | uwLo;
}

/* the nibble's magnitude lanes (pand, pcmpgtw, por 1): lane 0 always, lanes 1 and 3 test bits 0 and 2 of
 * the register's high dword, lane 2 bit 1 of its low dword */
static void SND_pc_fn_vMagnitudeLanesADPCM(SND_pc_tdstQwordADPCM stNibble,short *pwLanes)
{
	unsigned long ulA = stNibble.ulLo & 7, ulB = stNibble.ulHi & 7;
	pwLanes[0] = 1;
	pwLanes[1] = (ulB & 1) ? 1 : 0;
	pwLanes[2] = (ulA & 2) ? 1 : 0;
	pwLanes[3] = (ulB & 4) ? 1 : 0;
}

/* pmaddwd of the 4 step parts with the lanes, both dwords summed */
static long SND_pc_fn_lDiffADPCM(const short *pwParts,const short *pwLanes)
{
	unsigned long ulSum = 0;
	long k;
	for (k = 0; k < 4; k++)
		ulSum += (unsigned long)((long)pwParts[k] * pwLanes[k]);
	return SND_pc_fn_lSigned32ADPCM(ulSum);
}

void SND_pc_fn_vDecodeMonoADPCM(unsigned long ulNbSamples,char **pp_cSrc,long **pp_lDest,SND_pc_tdstDecoderStateADPCM *p_stState)
{
	SND_pc_tdstQwordADPCM stSrc, stNibble;
	const unsigned char *p_ucSrc;
	long *p_lDest;
	unsigned long ulIndex, ulNibble;
	long lPred, lProduct, k;
	short wStep, a_wParts[4], a_wLanes[4];

	if (!ulNbSamples)
		return;
	p_ucSrc = (const unsigned char *)*pp_cSrc;
	p_lDest = *pp_lDest;
	stSrc.ulLo = stSrc.ulHi = SND_pc_M_ulLow32(p_stState->a7_l[3]);
	lPred = p_stState->a7_l[1];
	ulIndex = SND_pc_M_ulLow32(p_stState->a7_l[2]);
	wStep = SND_pc_M_wLow16(SND_pc_a_lStepTableADPCM[ulIndex]);
	do
	{
		if (p_stState->a7_l[0])
		{
			/* second sample of the byte: its low nibble */
			p_stState->a7_l[0]--;
			stNibble = stSrc;
		}
		else
		{
			/* a new byte, high nibble first: a new dword at a 4-aligned pointer, else the next byte */
			p_stState->a7_l[0]++;
			if (((size_t)p_ucSrc & 3) == 0)
				stSrc.ulLo = stSrc.ulHi = SND_pc_fn_ulReadDwordADPCM(p_ucSrc);
			else
				stSrc = SND_pc_fn_stShiftQwordADPCM(stSrc,8);
			stNibble = SND_pc_fn_stShiftQwordADPCM(stSrc,4);
			p_ucSrc++;
		}
		ulNibble = stNibble.ulLo & 0xF;
		ulIndex = SND_pc_fn_ulNextIndexADPCM(ulIndex,SND_pc_a_lIndexTableADPCM[ulNibble]);
		/* step * 1, 2, 4, 8 >> 3 from pmulhw and pmullw: floor(step * k / 8) */
		for (k = 0; k < 4; k++)
		{
			lProduct = (long)wStep << k;
			a_wParts[k] = SND_pc_M_wLow16(((unsigned long)(lProduct >> 16) << 13) + (((unsigned long)lProduct & 0xFFFF) >> 3));
		}
		SND_pc_fn_vMagnitudeLanesADPCM(stNibble,a_wLanes);
		if (ulNibble & 8)
			lPred = SND_pc_fn_lSigned32ADPCM((unsigned long)lPred - (unsigned long)SND_pc_fn_lDiffADPCM(a_wParts,a_wLanes));
		else
			lPred = SND_pc_fn_lSigned32ADPCM((unsigned long)lPred + (unsigned long)SND_pc_fn_lDiffADPCM(a_wParts,a_wLanes));
		lPred = SND_pc_fn_wSat16ADPCM(lPred);
		*p_lDest++ = lPred;
		wStep = SND_pc_M_wLow16(SND_pc_a_lStepTableADPCM[ulIndex]);
	} while (--ulNbSamples);
	p_stState->a7_l[1] = lPred;
	p_stState->a7_l[3] = SND_pc_fn_lSigned32ADPCM(stSrc.ulLo);
	p_stState->a7_l[2] = SND_pc_fn_lSigned32ADPCM(ulIndex);
	*pp_cSrc = (char *)p_ucSrc;
	*pp_lDest = p_lDest;
}

/* one channel of a stereo frame */
static long SND_pc_fn_lDecodeChannelADPCM(SND_pc_tdstQwordADPCM stNibble,unsigned long *p_ulIndex,long lPred,long lStepEntry)
{
	unsigned long ulNibble = stNibble.ulLo & 0xF;
	unsigned short uwStep8 = (unsigned short)(SND_pc_M_wLow16(lStepEntry)) >> 3;	/* psrlw 3: low 3 bits lost */
	short a_wParts[4], a_wLanes[4];
	long k, lDiff;

	*p_ulIndex = SND_pc_fn_ulNextIndexADPCM(*p_ulIndex,SND_pc_a_lIndexTableADPCM[ulNibble]);
	for (k = 0; k < 4; k++)
		a_wParts[k] = SND_pc_M_wLow16((unsigned long)uwStep8 << k);	/* pmullw by 1, 2, 4, 8 */
	SND_pc_fn_vMagnitudeLanesADPCM(stNibble,a_wLanes);
	lDiff = SND_pc_fn_lDiffADPCM(a_wParts,a_wLanes);
	if (ulNibble & 8)
		lPred = SND_pc_fn_lSigned32ADPCM((unsigned long)lPred - (unsigned long)lDiff);
	else
		lPred = SND_pc_fn_lSigned32ADPCM((unsigned long)lPred + (unsigned long)lDiff);
	return SND_pc_fn_wSat16ADPCM(lPred);
}

void SND_pc_fn_vDecodeStereoADPCM(unsigned long ulNbSamples,char **pp_cSrc,long **pp_lDest,SND_pc_tdstDecoderStateADPCM *p_stState)
{
	SND_pc_tdstQwordADPCM stSrc, stLeft, stRight;
	const unsigned char *p_ucSrc;
	long *p_lDest;
	unsigned long ulIndexL, ulIndexR;
	long lStepL, lStepR;

	if (!ulNbSamples)
		return;
	p_ucSrc = (const unsigned char *)*pp_cSrc;
	p_lDest = *pp_lDest;
	stSrc.ulLo = stSrc.ulHi = SND_pc_M_ulLow32(p_stState->a7_l[6]);
	ulIndexL = SND_pc_M_ulLow32(p_stState->a7_l[5]);
	ulIndexR = SND_pc_M_ulLow32(p_stState->a7_l[2]);
	lStepL = SND_pc_a_lStepTableADPCM[ulIndexL];
	lStepR = SND_pc_a_lStepTableADPCM[ulIndexR];
	do
	{
		/* left = first byte of the pair, right = second; high nibbles one frame, low nibbles the next */
		stLeft = stSrc;
		stRight = SND_pc_fn_stShiftDwordsADPCM(stSrc,8);
		if (p_stState->a7_l[0])
			p_stState->a7_l[0] = 0;
		else
		{
			p_stState->a7_l[0] = 1;
			if (((size_t)p_ucSrc & 3) == 0)
			{
				stSrc.ulLo = stSrc.ulHi = SND_pc_fn_ulReadDwordADPCM(p_ucSrc);
				stLeft = stSrc;
				stRight = SND_pc_fn_stShiftQwordADPCM(stSrc,8);
			}
			else
			{
				stSrc = SND_pc_fn_stShiftQwordADPCM(stSrc,16);
				stLeft = SND_pc_fn_stShiftQwordADPCM(stLeft,16);
				stRight = SND_pc_fn_stShiftQwordADPCM(stRight,16);
			}
			stLeft = SND_pc_fn_stShiftQwordADPCM(stLeft,4);
			stRight = SND_pc_fn_stShiftQwordADPCM(stRight,4);
			p_ucSrc += 2;
		}
		p_stState->a7_l[4] = SND_pc_fn_lDecodeChannelADPCM(stLeft,&ulIndexL,p_stState->a7_l[4],lStepL);
		p_stState->a7_l[1] = SND_pc_fn_lDecodeChannelADPCM(stRight,&ulIndexR,p_stState->a7_l[1],lStepR);
		*p_lDest++ = p_stState->a7_l[4];
		*p_lDest++ = p_stState->a7_l[1];
		lStepL = SND_pc_a_lStepTableADPCM[ulIndexL];
		lStepR = SND_pc_a_lStepTableADPCM[ulIndexR];
	} while (--ulNbSamples);
	p_stState->a7_l[5] = SND_pc_fn_lSigned32ADPCM(ulIndexL);
	p_stState->a7_l[2] = SND_pc_fn_lSigned32ADPCM(ulIndexR);
	p_stState->a7_l[6] = SND_pc_fn_lSigned32ADPCM(stSrc.ulLo);
	p_stState->a7_l[3] = SND_pc_fn_lSigned32ADPCM(stSrc.ulLo >> 8);
	*pp_cSrc = (char *)p_ucSrc;
	*pp_lDest = p_lDest;
}

/*---- mixer */
/* the MMX lane: sat16(sat16(mix) + ((sample * gain word) >> 16 << 1)), stored sign-extended */
static long SND_pc_fn_lMixLaneADPCM(long lMix,short wSample,short wGain)
{
	short wScaled = SND_pc_M_wLow16((unsigned long)(((long)wSample * wGain) >> 16) << 1);
	return SND_pc_fn_wSat16ADPCM((long)SND_pc_fn_wSat16ADPCM(lMix) + wScaled);
}

/* the gain word of the MMX parts: psllw 1 on both words of the gain, then packssdw */
static short SND_pc_fn_wGainWordADPCM(unsigned long ulGain)
{
	unsigned long ulDword = ((((ulGain >> 16) << 1) & 0xFFFF) << 16) | ((ulGain << 1) & 0xFFFF);
	return SND_pc_fn_wSat16ADPCM(SND_pc_fn_lSigned32ADPCM(ulDword));
}

/* the scalar tail: mix += (sample * gain) >> 14, 32 bits, no saturation */
static long SND_pc_fn_lTailADPCM(long lMix,long lSample,unsigned long ulGain)
{
	long lProduct = SND_pc_fn_lSigned32ADPCM((unsigned long)lSample * ulGain);
	return SND_pc_fn_lSigned32ADPCM((unsigned long)lMix + (unsigned long)(lProduct >> 14));
}

void SND_pc_fn_vMixVoiceADPCM(long *p_lMix,long *p_lMixEnd,long *p_lDecoded,SND_pc_tdstVoiceADPCM *p_stVoice)
{
	unsigned long ulGainL, ulGainR, ulFade, ulStep, ulPos, ulIndex;
	long *p_lBlockEnd;
	long k, bStereo;
	short wGainL, wGainR, wL, wR;

	if (p_lMix == p_lMixEnd)
		return;
	ulGainL = SND_pc_M_ulLow32(p_stVoice->lGainLeft);
	ulGainR = SND_pc_M_ulLow32(p_stVoice->lGainRight);
	if (p_stVoice->bFade)
	{
		ulFade = SND_pc_M_ulLow32(p_stVoice->ulFadeStep * p_stVoice->ulFadeLength);
		ulGainL = SND_pc_fn_ulMulHigh32ADPCM(ulFade,ulGainL);
		ulGainR = SND_pc_fn_ulMulHigh32ADPCM(ulFade,ulGainR);
	}
	wGainL = SND_pc_fn_wGainWordADPCM(ulGainL);
	wGainR = SND_pc_fn_wGainWordADPCM(ulGainR);
	ulStep = SND_pc_M_ulLow32(p_stVoice->ulStep);
	bStereo = (p_stVoice->wNbChannels & 3) == 2;
	p_lBlockEnd = p_lMix + ((p_lMixEnd - p_lMix) & ~(ptrdiff_t)7);
	ulPos = 0;
	ulIndex = 0;
	/* MMX part: 4 frames per step, at least one step */
	do
	{
		for (k = 0; k < 4; k++)
		{
			if (ulStep == 0x200)
			{
				/* native rate: the decoded samples in order (saturated to 16 bits) */
				wL = SND_pc_fn_wSat16ADPCM(bStereo ? p_lDecoded[2 * k] : p_lDecoded[k]);
				wR = bStereo ? SND_pc_fn_wSat16ADPCM(p_lDecoded[2 * k + 1]) : wL;
			}
			else
			{
				/* any rate: frame floor(pos / 512); mono takes the low 16 bits, stereo saturates */
				if (bStereo)
				{
					wL = SND_pc_fn_wSat16ADPCM(p_lDecoded[2 * ulIndex]);
					wR = SND_pc_fn_wSat16ADPCM(p_lDecoded[2 * ulIndex + 1]);
				}
				else
					wL = wR = SND_pc_M_wLow16(p_lDecoded[ulIndex]);
				ulPos = SND_pc_M_ulLow32(ulPos + ulStep);
				ulIndex = ulPos >> 9;
			}
			p_lMix[2 * k] = SND_pc_fn_lMixLaneADPCM(p_lMix[2 * k],wL,wGainL);
			p_lMix[2 * k + 1] = SND_pc_fn_lMixLaneADPCM(p_lMix[2 * k + 1],wR,wGainR);
		}
		if (ulStep == 0x200)
			p_lDecoded += bStereo ? 8 : 4;
		p_lMix += 8;
	} while (p_lMix < p_lBlockEnd);
	/* scalar tail with the gains themselves; native rate goes on from the advanced samples */
	if (p_lMix < p_lMixEnd)
	{
		do
		{
			if (bStereo)
			{
				p_lMix[0] = SND_pc_fn_lTailADPCM(p_lMix[0],p_lDecoded[2 * ulIndex],ulGainL);
				p_lMix[1] = SND_pc_fn_lTailADPCM(p_lMix[1],p_lDecoded[2 * ulIndex + 1],ulGainR);
			}
			else
			{
				p_lMix[0] = SND_pc_fn_lTailADPCM(p_lMix[0],p_lDecoded[ulIndex],ulGainL);
				p_lMix[1] = SND_pc_fn_lTailADPCM(p_lMix[1],p_lDecoded[ulIndex],ulGainR);
			}
			ulPos = SND_pc_M_ulLow32(ulPos + ulStep);
			ulIndex = ulPos >> 9;
			p_lMix += 2;
		} while (p_lMix < p_lMixEnd);
	}
}

/*---- output */
void SND_pc_fn_vSaturateMixToPcm16ADPCM(long *p_lMix,long *p_lMixEnd,void **pp_vDest)
{
	short *p_wDest;
	long *p_lBlockEnd;
	long k;

	if (p_lMixEnd <= p_lMix)
		return;
	p_wDest = (short *)*pp_vDest;
	p_lBlockEnd = p_lMix + ((p_lMixEnd - p_lMix) & ~(ptrdiff_t)15);
	while (p_lMix < p_lBlockEnd)
	{
		for (k = 0; k < 16; k++)
			p_wDest[k] = SND_pc_fn_wSat16ADPCM(p_lMix[k]);
		p_lMix += 16;
		p_wDest += 16;
	}
	/* one frame per step: an odd number of longs writes one frame past the end */
	while (p_lMix < p_lMixEnd)
	{
		p_wDest[0] = SND_pc_fn_wSat16ADPCM(p_lMix[0]);
		p_wDest[1] = SND_pc_fn_wSat16ADPCM(p_lMix[1]);
		p_lMix += 2;
		p_wDest += 2;
	}
	*pp_vDest = p_wDest;
}

#endif /* OPTIMIZED_FOR_MMX_WITH_ASM */
