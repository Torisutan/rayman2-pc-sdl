/*
//////////////////////////////////////////////////////////////
//	SNDMIX_PC.C  (WAVMWBVR.dll)
//////////////////////////////////////////////////////////////
//	C branch of the sample driver's mixer: the 21 entry points
//	of sndmix_pc.h, beside the MMX asm (mix1..mix5*.asm), which
//	is built instead under OPTIMIZED_FOR_MMX_WITH_ASM.
//////////////////////////////////////////////////////////////
*/
/* Each function computes what its asm counterpart computes, buffer for buffer and byte for byte, quirks
 * included (checked against the asm on the same input): 16-bit lanes with the asm's saturation
 * (paddsw/psubsw), its high-half products (pmulhw), its pair sums (pmaddwd), its shifts and its loop
 * structure (do-while on lLength >> 4 where the asm has no zero test, extra or skipped blocks at the end
 * of a voice, the position each wrap gives). Comments name the asm's behaviour where it is not what the
 * prototype comment of sndmix_pc.h would suggest.
 *
 * Data. The asm reads its delay lines across the bounds of the arrays sndwave.c declares (an 0x8000-byte
 * ring behind an 0x3FF0-byte array, the send ring running on into the next array): in the retail image
 * those arrays lie back to back. The C branch keeps each line here with its full extent, and its send
 * cursor as an offset (the asm keeps an address in SND_pc_a27C90[3]). The plain counters stay
 * sndwave.c's longs (role names of sndmix_pc.h). */
#include "SNDinc.h"

#if !defined(OPTIMIZED_FOR_MMX_WITH_ASM)

#include "sndres.h"
#include "sndwave.h"
#include "sndmix_pc.h"
#include <string.h>

/*---- data */
/* send ring: a 0x40-byte head, then one word per frame of the 100 output buffers. StepSend wraps its
 * cursor at 0xAC20, NativeSend does not: the last buffer's send ends at 0xAC40 (in the retail image it
 * runs on over the voice echo indices, which only the mixers without a caller read) */
#define SND_pc_C_lReverbSendSize	0xAC40
#define SND_pc_C_lRingSize			0x8000	/* the delay lines, in bytes */

extern long				SND_pc_l27C8C;
extern long				SND_pc_a27C90[13];
extern long				SND_pc_l21E30;
extern long				SND_pc_l21E34;
extern long				SND_pc_l21E38;
extern unsigned char	SND_pc_a560A0_9930[0x2AF80];

static char		SND_pc_s_a_cReverbSend[SND_pc_C_lReverbSendSize];
/* send cursor, a byte offset into the ring (the asm keeps an address in SND_pc_a27C90[3], null until the
 * first send: a reverb pass before any send would fault there; here it starts at the head) */
static long		SND_pc_s_lReverbSendCursor = 0x40;
static char		SND_pc_s_a_cEchoRing[SND_pc_C_lRingSize];
static char		SND_pc_s_a_cSweptCombRing[SND_pc_C_lRingSize];
static char		SND_pc_s_a_cVoiceEchoRing[SND_pc_C_lRingSize];
/* + 8: with a delay that is not a multiple of 16 the asm writes one qword at 0x8000 before it wraps */
static char		SND_pc_s_a_cVoiceCombRing[SND_pc_C_lRingSize + 8];
static long		SND_pc_s_a_lVoiceEchoIndex[4];
static long		SND_pc_s_a_lVoiceEchoSideIndex[2];
static long		SND_pc_s_a_lVoiceCombIndex[2];	/* written by vMixMono16Comb, read only by vApplySweptComb */
static long		SND_pc_s_a_lVoiceCombDelay[4];
static char		SND_pc_s_a_cFirHistory[0x1000];

/*---- lane arithmetic (the MMX operations the asm uses, one 16-bit lane at a time) */
/* low 16 bits of a register value, as the lane a punpcklwd broadcast takes */
#define SND_pc_M_wLow(ul)		((short)(unsigned short)((unsigned long)(ul) & 0xFFFF))

static short SND_pc_fn_wAddSat(long lA,long lB)	/* paddsw */
{
	long lSum = lA + lB;
	return (short)(lSum > 32767 ? 32767 : (lSum < -32768 ? -32768 : lSum));
}

static short SND_pc_fn_wSubSat(long lA,long lB)	/* psubsw */
{
	return SND_pc_fn_wAddSat(lA,-lB);
}

static short SND_pc_fn_wMulHigh(short wA,short wB)	/* pmulhw */
{
	return (short)(((long)wA * wB) >> 16);
}

/* pmaddwd lane then psrad 16, as the low word: bits 16..31 of wA0 * wGain + wA1 * wGain (32 bits) */
static short SND_pc_fn_wPairGain(short wA0,short wA1,short wGain)
{
	unsigned long ulSum = (unsigned long)((long)wA0 * wGain) + (unsigned long)((long)wA1 * wGain);
	return SND_pc_M_wLow((ulSum & 0xFFFFFFFFUL) >> 16);
}

static short SND_pc_fn_wRead(const char *p)
{
	short w;
	memcpy(&w,p,sizeof(w));
	return w;
}

static void SND_pc_fn_vWrite(char *p,short w)
{
	memcpy(p,&w,sizeof(w));
}

/* stores 4 lanes at pDst, added with saturation when bAdd */
static void SND_pc_fn_vPut4(short *pDst,const short *pwLanes,long bAdd)
{
	long k;
	for (k = 0; k < 4; k++)
		pDst[k] = bAdd ? SND_pc_fn_wAddSat(pDst[k],pwLanes[k]) : pwLanes[k];
}

/* L/R interleave of 4 frames (punpcklwd / punpckhwd), stored as two qwords */
static void SND_pc_fn_vPutFrames(short *pDst,const short *pwL,const short *pwR,long bAdd)
{
	short a_wLo[4], a_wHi[4];
	a_wLo[0] = pwL[0]; a_wLo[1] = pwR[0]; a_wLo[2] = pwL[1]; a_wLo[3] = pwR[1];
	a_wHi[0] = pwL[2]; a_wHi[1] = pwR[2]; a_wHi[2] = pwL[3]; a_wHi[3] = pwR[3];
	SND_pc_fn_vPut4(pDst,a_wLo,bAdd);
	SND_pc_fn_vPut4(pDst + 4,a_wHi,bAdd);
}

/* the two gain words of the asm's "ror eax, 16; add eax, ebx" form: lanes 0 and 2 take the low word,
 * lanes 1 and 3 the high word */
static void SND_pc_fn_vSplitGain(unsigned long ulGain,short *p_wLo,short *p_wHi)
{
	unsigned long ulSum = ((((ulGain >> 16) | (ulGain << 16)) & 0xFFFFFFFFUL) + ulGain) & 0xFFFFFFFFUL;
	*p_wLo = SND_pc_M_wLow(ulSum);
	*p_wHi = SND_pc_M_wLow(ulSum >> 16);
}

/* "imul bl" on lVol: AX = (signed char)lVol * (signed char)bl, the upper half of lVol kept, then << 1 */
static unsigned long SND_pc_fn_ulGain8(long lVol,long lFactor)
{
	unsigned long ulAx = (unsigned long)((long)(signed char)lVol * (signed char)lFactor) & 0xFFFF;
	return ((((unsigned long)lVol & 0xFFFF0000UL) | ulAx) << 1) & 0xFFFFFFFFUL;
}

static void SND_pc_fn_vStopVoice(SND_pc_tdstSampleVoice *pVoice)
{
	pVoice->bPlaying = 0;
	pVoice->ulPos = 0;
}

/*---- Dolby (surround) mix path */
void SND_pc_fn_vClearReverbSend(long lOffset,long lLength)
{
	unsigned long ulNb = (unsigned long)lLength >> 3;
	if (ulNb)
		memset(SND_pc_s_a_cReverbSend + 0x40 + lOffset,0,ulNb * 8);
}

void SND_pc_fn_vMixMono16NativeSend(struct _SND_pc_tdstSampleVoice *pVoice,char *pBuffer,long lLength,long bSide,long lOffset,long lUnused)
{
	short wGainL, wGainR, wGainSend;
	short a_wS[4], a_wL[4], a_wR[4];
	unsigned long ulNb;
	long lPos, lEnd, k;
	const char *pData;
	short *pDst;
	char *pSend;

	if (lOffset == 0)
		memcpy(SND_pc_s_a_cReverbSend,SND_pc_s_a_cReverbSend + 0xABE0,0x40);
	SND_pc_s_lReverbSendCursor = 0x40 + lOffset;
	wGainL = SND_pc_M_wLow(((unsigned long)pVoice->lVol * pVoice->lPan * (0x80 - pVoice->lSpace) & 0xFFFFFFFFUL) >> 5);
	wGainR = SND_pc_M_wLow(((unsigned long)pVoice->lVol * (0x80 - pVoice->lPan) * (0x80 - pVoice->lSpace) & 0xFFFFFFFFUL) >> 5);
	wGainSend = SND_pc_M_wLow((unsigned long)pVoice->lVol * pVoice->lSpace << 1);
	ulNb = (unsigned long)lLength >> 4;
	/* positions as byte offsets of the 16-bit samples (ulPos >> 7) */
	lPos = (long)((pVoice->ulPos & 0xFFFFFFFFUL) >> 7);
	lEnd = (long)((pVoice->ulEnd & 0xFFFFFFFFUL) >> 7);
	pData = (const char *)pVoice->pvData;
	pDst = bSide ? (short *)SND_pc_g_a_wSideBuffer : (short *)pBuffer;
	do
	{
		if (lPos >= lEnd)
		{
			/* the block is skipped: nothing written, the output does not advance */
			if (!pVoice->bResLoop)
			{
				SND_pc_fn_vStopVoice(pVoice);
				return;
			}
			lPos = (long)((pVoice->ulLoop & 0xFFFFFFFFUL) >> 7);
			continue;
		}
		pSend = SND_pc_s_a_cReverbSend + SND_pc_s_lReverbSendCursor;
		SND_pc_s_lReverbSendCursor += 8;
		for (k = 0; k < 4; k++)
		{
			a_wS[k] = SND_pc_fn_wRead(pData + lPos + 2 * k);
			SND_pc_fn_vWrite(pSend + 2 * k,SND_pc_fn_wAddSat(SND_pc_fn_wMulHigh(a_wS[k],wGainSend),SND_pc_fn_wRead(pSend + 2 * k)));
			a_wL[k] = SND_pc_fn_wMulHigh(a_wS[k],wGainL);
			a_wR[k] = SND_pc_fn_wMulHigh(a_wS[k],wGainR);
		}
		lPos += 8;
		SND_pc_fn_vPutFrames(pDst,a_wL,a_wR,1);
		pDst += 8;
	} while (--ulNb);
	pVoice->ulPos = (unsigned long)lPos << 7;
}

void SND_pc_fn_vMixMono16StepSend(struct _SND_pc_tdstSampleVoice *pVoice,char *pBuffer,long lLength,long bSide,long lOffset,long lUnused)
{
	short wGainL, wGainR, wGainSend;
	short a_wS[4], a_wL[4], a_wR[4];
	unsigned long ulNb, ulPos;
	long lEnd, k, bCheck;
	const short *pData;
	short *pDst;
	char *pSend;

	SND_pc_s_lReverbSendCursor = 0x40 + lOffset;
	wGainL = SND_pc_M_wLow(((unsigned long)pVoice->lVol * pVoice->lPan * (0x80 - pVoice->lSpace) & 0xFFFFFFFFUL) >> 6);
	wGainR = SND_pc_M_wLow(((unsigned long)pVoice->lVol * (0x80 - pVoice->lPan) * (0x80 - pVoice->lSpace) & 0xFFFFFFFFUL) >> 6);
	wGainSend = SND_pc_M_wLow((unsigned long)pVoice->lVol * (0x80 - pVoice->lSpace) << 1);
	ulNb = (unsigned long)lLength >> 4;
	if (!ulNb)
		return;
	ulPos = pVoice->ulPos;
	lEnd = (long)pVoice->ulEnd;
	pData = (const short *)pVoice->pvData;
	pDst = bSide ? (short *)SND_pc_g_a_wSideBuffer : (short *)pBuffer;
	do
	{
		/* gather 4 samples, nearest; the end is tested before each fetch. At the end the asm steps once
		 * more, wraps (pos + lInc - ulEnd + ulLoop) and gathers the whole block again from the loop.
		 * (Its other exits from that step need pos + lInc to pass 2^31.) */
		k = 0;
		bCheck = 1;
		while (k < 4)
		{
			if (bCheck && (long)ulPos >= lEnd)
			{
				ulPos += pVoice->lInc;
				ulPos -= lEnd;
				if (!pVoice->bResLoop)
				{
					SND_pc_fn_vStopVoice(pVoice);
					return;
				}
				ulPos += pVoice->ulLoop;
				k = 0;
				bCheck = 0;
				continue;
			}
			a_wS[k++] = pData[(ulPos & 0xFFFFFFFFUL) >> 8];
			ulPos += pVoice->lInc;
			bCheck = (k < 4);
		}
		if (0x40 + 0xABE0 - SND_pc_s_lReverbSendCursor <= 0)
			SND_pc_s_lReverbSendCursor = 0x40;
		pSend = SND_pc_s_a_cReverbSend + SND_pc_s_lReverbSendCursor;
		for (k = 0; k < 4; k++)
		{
			SND_pc_fn_vWrite(pSend + 2 * k,SND_pc_fn_wAddSat(SND_pc_fn_wMulHigh(a_wS[k],wGainSend),SND_pc_fn_wRead(pSend + 2 * k)));
			a_wL[k] = SND_pc_fn_wMulHigh(a_wS[k],wGainL);
			a_wR[k] = SND_pc_fn_wMulHigh(a_wS[k],wGainR);
		}
		SND_pc_s_lReverbSendCursor += 8;
		SND_pc_fn_vPutFrames(pDst,a_wL,a_wR,1);
		pDst += 8;
	} while (--ulNb);
	pVoice->ulPos = ulPos;
}

void SND_pc_fn_vMixReverbFir32(char *pBuffer,short *pwCoefs,long lLength,long lUnused)
{
	unsigned long ulNb, ulSum;
	long lSrc, lTap;
	short *pDst;
	short wTap;
	long d, k;

	lSrc = SND_pc_s_lReverbSendCursor - (long)((unsigned long)lLength >> 1);
	ulNb = (unsigned long)lLength >> 4;
	if (!ulNb)
		return;
	pDst = (short *)pBuffer;
	do
	{
		for (d = 0; d < 4; d++)
		{
			/* 32 taps ending one frame before frame d, summed in 32 bits, >> 13 */
			ulSum = 0;
			for (k = 0; k < 32; k++)
			{
				/* after StepSend wrapped its cursor, the window starts before the ring: the asm reads
				 * the memory in front of it (in the retail image the voices), here zeros (DEVIATIONS) */
				lTap = lSrc + 2 * (d + k - 32);
				if (lTap >= 0)
					ulSum += (unsigned long)((long)SND_pc_fn_wRead(SND_pc_s_a_cReverbSend + lTap) * pwCoefs[k]);
			}
			wTap = SND_pc_M_wLow((ulSum & 0xFFFFFFFFUL) >> 13);	/* psrad 13: bits 13..28 */
			pDst[2 * d] = SND_pc_fn_wAddSat(pDst[2 * d],wTap);
			pDst[2 * d + 1] = SND_pc_fn_wSubSat(pDst[2 * d + 1],wTap);
		}
		pDst += 8;
		lSrc += 8;
	} while (--ulNb);
}

void SND_pc_fn_vMixFirHistory(char *pSrc,char *pDst,short *pwCoefs,long lNbFrames,long lLength)
{
	unsigned long ulNb, a_ulSum[4];
	const char *pIn, *p;
	char *pOut;
	long lTap, j;
	short a_wOut[4];

	/* the asm walks a pointer: the copy of pSrc's first lNbFrames frames behind the history, then pSrc
	 * itself once lNbFrames frames are out; taps reach lNbFrames frames back */
	memcpy(SND_pc_s_a_cFirHistory + 4 * lNbFrames,pSrc,4 * lNbFrames);
	pIn = SND_pc_s_a_cFirHistory + 4 * lNbFrames;
	ulNb = (unsigned long)lLength >> 3;
	if (!ulNb)
		return;
	pOut = pDst;
	do
	{
		if ((unsigned long)(pOut - pDst) >> 1 == (unsigned long)(2 * lNbFrames))
			pIn = pSrc + 4 * lNbFrames;
		a_ulSum[0] = a_ulSum[1] = a_ulSum[2] = a_ulSum[3] = 0;
		/* 4 taps per step, from lNbFrames frames back up to the current frame (L, R of this frame and of
		 * the next) */
		for (lTap = 0; 2 * lTap - 2 * lNbFrames != 0; lTap += 4)
		{
			p = pIn + 4 * lTap - 4 * lNbFrames;
			for (j = 0; j < 4; j++)
			{
				a_ulSum[0] += (unsigned long)((long)SND_pc_fn_wRead(p + 4 * j) * pwCoefs[lTap + j]);
				a_ulSum[1] += (unsigned long)((long)SND_pc_fn_wRead(p + 4 * j + 2) * pwCoefs[lTap + j]);
				a_ulSum[2] += (unsigned long)((long)SND_pc_fn_wRead(p + 4 * j + 4) * pwCoefs[lTap + j]);
				a_ulSum[3] += (unsigned long)((long)SND_pc_fn_wRead(p + 4 * j + 6) * pwCoefs[lTap + j]);
			}
		}
		for (j = 0; j < 4; j++)
			a_wOut[j] = SND_pc_M_wLow((a_ulSum[j] & 0xFFFFFFFFUL) >> 16);	/* psrad 16: bits 16..31 */
		SND_pc_fn_vWrite(pOut,SND_pc_fn_wAddSat(SND_pc_fn_wRead(pOut),a_wOut[0]));
		SND_pc_fn_vWrite(pOut + 2,SND_pc_fn_wAddSat(SND_pc_fn_wRead(pOut + 2),a_wOut[1]));
		SND_pc_fn_vWrite(pOut + 4,SND_pc_fn_wAddSat(SND_pc_fn_wRead(pOut + 4),a_wOut[2]));
		SND_pc_fn_vWrite(pOut + 6,SND_pc_fn_wAddSat(SND_pc_fn_wRead(pOut + 6),a_wOut[3]));
		pOut += 8;
		pIn += 8;
	} while (--ulNb);
	memcpy(SND_pc_s_a_cFirHistory,pSrc + lLength - 4 * lNbFrames,4 * lNbFrames);
}

/*---- post-mix effects */
void SND_pc_fn_vApplyEchoDelay(char *pBuffer,long lLength,long lMode)
{
	unsigned long ulNb = (unsigned long)lLength >> 3;
	long lIndex = SND_pc_g_lEchoRingIndex;
	long lDelay, lShift, lTap, k;
	short *pBuf = (short *)pBuffer;
	short wY;

	switch (lMode)
	{
	case 0: lDelay = 0x4000; lShift = 2; break;	/* ring = y - y/4 */
	case 1: lDelay = 0x4000; lShift = 1; break;
	case 2: lDelay = 0x4000; lShift = 2; break;
	case 3: lDelay = 0x2000; lShift = 1; break;
	default: lDelay = 0x1000; lShift = 1; break;
	}
	do
	{
		lTap = lIndex < lDelay ? lIndex + lDelay : lIndex - lDelay;
		for (k = 0; k < 4; k++)
		{
			wY = SND_pc_fn_wAddSat(pBuf[k],SND_pc_fn_wRead(SND_pc_s_a_cEchoRing + lTap + 2 * k));
			pBuf[k] = wY;
			if (lMode == 0)
				SND_pc_fn_vWrite(SND_pc_s_a_cEchoRing + lIndex + 2 * k,(short)(wY - (wY >> 2)));
			else
				SND_pc_fn_vWrite(SND_pc_s_a_cEchoRing + lIndex + 2 * k,(short)(wY >> lShift));
		}
		lIndex += 8;
		if (lIndex >= 2 * lDelay)
			lIndex = 0;
		pBuf += 4;
	} while (--ulNb);
	SND_pc_g_lEchoRingIndex = lIndex;
}

/* the swept delay's triangle: every 200 qwords one step of 8 bytes, turning at 0x400 and 0x2000 */
static void SND_pc_fn_vSweepStep(void)
{
	if (--SND_pc_g_lSweepCounter == 0)
	{
		SND_pc_g_lSweepCounter = 200;
		if (SND_pc_g_lSweepStep == 8)
		{
			if (SND_pc_g_lSweepDelay >= 0x2000)
				SND_pc_g_lSweepStep = -8;
		}
		else if (SND_pc_g_lSweepDelay <= 0x400)
			SND_pc_g_lSweepStep = 8;
		SND_pc_g_lSweepDelay += SND_pc_g_lSweepStep;
	}
}

/* the swept comb's delayed qword. Below the delay the asm reads at index + 0x8000 + delay, past the end of
 * the ring: in the retail image three sndwave.c variables (0x18 bytes: here zeros, DEVIATIONS), the comb
 * mixer's index (8 bytes), then the echo ring; with the sweep's delays (0x400 and up) the echo ring */
static void SND_pc_fn_vSweptCombTap(long lIndex,long lDelay,short *pwTap)
{
	long lTap = lIndex < lDelay ? lIndex + SND_pc_C_lRingSize + lDelay : lIndex - lDelay;
	long k;
	unsigned long ulIndex;

	if (lTap < SND_pc_C_lRingSize)
		for (k = 0; k < 4; k++)
			pwTap[k] = SND_pc_fn_wRead(SND_pc_s_a_cSweptCombRing + lTap + 2 * k);
	else if ((lTap -= SND_pc_C_lRingSize) >= 0x20)
		for (k = 0; k < 4; k++)
			pwTap[k] = SND_pc_fn_wRead(SND_pc_s_a_cEchoRing + lTap - 0x20 + 2 * k);
	else if (lTap >= 0x18)
		for (k = 0; k < 4; k++)
		{
			ulIndex = (unsigned long)SND_pc_s_a_lVoiceCombIndex[k >> 1];
			pwTap[k] = SND_pc_M_wLow((k & 1) ? ulIndex >> 16 : ulIndex);
		}
	else
		pwTap[0] = pwTap[1] = pwTap[2] = pwTap[3] = 0;
}

void SND_pc_fn_vApplySweptComb(char *pBuffer,long lLength,long lUnused)
{
	unsigned long ulNb = (unsigned long)lLength >> 3;
	long lIndex = SND_pc_g_lSweptCombIndex;
	long lDelay = SND_pc_g_lSweepDelay;
	long k;
	short *pBuf = (short *)pBuffer;
	short wY, a_wTap[4];

	while (ulNb--)
	{
		SND_pc_fn_vSweptCombTap(lIndex,lDelay,a_wTap);
		for (k = 0; k < 4; k++)
		{
			wY = SND_pc_fn_wSubSat(pBuf[k],a_wTap[k]);
			SND_pc_fn_vWrite(SND_pc_s_a_cSweptCombRing + lIndex + 2 * k,(short)(wY >> 2));
			pBuf[k] = wY;
		}
		/* the delay steps in its global; this loop reads it again only on the next call */
		SND_pc_g_lSweepDelay = lDelay;
		SND_pc_fn_vSweepStep();
		lDelay = SND_pc_g_lSweepDelay;
		if (lIndex >= 0x7FF8)
			lIndex = 0;
		lIndex += 8;
		pBuf += 4;
	}
	SND_pc_g_lSweptCombIndex = lIndex;
	SND_pc_g_lSweepDelay = lDelay;
}

void SND_pc_fn_vApplySideEcho(char *pBuffer,long lLength)
{
	unsigned long ulNb = (unsigned long)lLength >> 3;
	long lIndex = SND_pc_g_lEchoRingIndex;
	long lTap, k;
	short *pBuf = (short *)pBuffer;
	short *pSide = (short *)SND_pc_g_a_wSideBuffer;
	short wY;

	do
	{
		lTap = lIndex < 0x4000 ? lIndex + 0x4000 : lIndex - 0x4000;
		for (k = 0; k < 4; k++)
		{
			wY = SND_pc_fn_wAddSat(pSide[k],SND_pc_fn_wRead(SND_pc_s_a_cEchoRing + lTap + 2 * k));
			pSide[k] = 0;
			pBuf[k] = SND_pc_fn_wAddSat(pBuf[k],wY);
			SND_pc_fn_vWrite(SND_pc_s_a_cEchoRing + lIndex + 2 * k,(short)(wY >> 1));
		}
		lIndex += 8;
		if (lIndex >= 0x8000)
			lIndex = 0;
		pBuf += 4;
		pSide += 4;
	} while (--ulNb);
	SND_pc_g_lEchoRingIndex = lIndex;
}

void SND_pc_fn_vFillBuffer(char *pBuffer,long lValue,long lLength)
{
	unsigned long ulNb = (unsigned long)lLength >> 3;
	unsigned int uiValue = (unsigned int)lValue;	/* the dword, twice per qword */

	for (ulNb *= 2; ulNb; ulNb--, pBuffer += 4)
		memcpy(pBuffer,&uiValue,4);
}

/*---- mono 16-bit voices */
/* gain words of the Native, HalfRate and DoubleRate mixers ("imul bl": low bytes of lVol and pan) */
static void SND_pc_fn_vGains8(SND_pc_tdstSampleVoice *pVoice,short *p_wGainL,short *p_wGainR)
{
	*p_wGainR = SND_pc_M_wLow(SND_pc_fn_ulGain8(pVoice->lVol,pVoice->lPan));
	*p_wGainL = SND_pc_M_wLow(SND_pc_fn_ulGain8(pVoice->lVol,0x80 - pVoice->lPan));
}

void SND_pc_fn_vMixMono16Native(struct _SND_pc_tdstSampleVoice *pVoice,char *pBuffer,long lLength,long lUnused,long bAdd)
{
	short wGainL, wGainR;
	short a_wS[4], a_wL[4], a_wR[4];
	unsigned long ulNb;
	long lPos, lEnd, lRest, k;
	const short *pData = (const short *)pVoice->pvData;
	short *pDst = (short *)pBuffer;

	SND_pc_fn_vGains8(pVoice,&wGainL,&wGainR);
	ulNb = (unsigned long)lLength >> 4;
	lPos = (long)((pVoice->ulPos & 0xFFFFFFFFUL) >> 8);
	lEnd = (long)((pVoice->ulEnd & 0xFFFFFFFFUL) >> 8) - 4;
	do
	{
		for (k = 0; k < 4; k++)
		{
			a_wS[k] = pData[lPos + k];
			a_wL[k] = (short)(SND_pc_fn_wMulHigh(a_wS[k],wGainL) << 1);
			a_wR[k] = (short)(SND_pc_fn_wMulHigh(a_wS[k],wGainR) << 1);
		}
		lPos += 4;
		SND_pc_fn_vPutFrames(pDst,a_wL,a_wR,bAdd);
		pDst += 8;
		if (lPos > lEnd)
		{
			/* end of the data: the last (end & 3) samples once more, at half gain, into a block that
			 * starts (end & 3) frames after the start of the block just written (it overlaps it and runs
			 * on past it) */
			lRest = lEnd & 3;
			if (lRest)
			{
				pDst = pDst - 8 + 2 * lRest;
				for (k = 0; k < 4; k++)
				{
					a_wS[k] = (k >= 4 - lRest) ? pData[lEnd + k] : 0;
					a_wL[k] = SND_pc_fn_wMulHigh(a_wS[k],wGainL);
					a_wR[k] = SND_pc_fn_wMulHigh(a_wS[k],wGainR);
				}
				SND_pc_fn_vPutFrames(pDst,a_wL,a_wR,bAdd);
				pDst += 8;
			}
			if (!pVoice->bResLoop)
			{
				SND_pc_fn_vStopVoice(pVoice);
				return;
			}
			lPos = (long)((pVoice->ulLoop & 0xFFFFFFFFUL) >> 8);
		}
	} while (--ulNb);
	pVoice->ulPos = (unsigned long)lPos << 8;
}

/* HalfRate block: samples s0, s1 give frames s0, (s0+s1)/2, s1, (s1+s2)/2 (pmulhw first, then the
 * midpoint, then << 1) */
static void SND_pc_fn_vHalfRateBlock(const short *pwS,short wGainL,short wGainR,short *pDst,long bAdd)
{
	short a_wA[4], a_wB[4], a_wL[4], a_wR[4];
	long k;

	for (k = 0; k < 4; k++)
	{
		a_wA[k] = SND_pc_fn_wMulHigh(pwS[k],wGainL);
		a_wB[k] = SND_pc_fn_wMulHigh(pwS[k],wGainR);
	}
	for (k = 0; k < 2; k++)
	{
		a_wL[2 * k] = (short)(a_wA[k] << 1);
		a_wL[2 * k + 1] = (short)((SND_pc_fn_wAddSat(a_wA[k],a_wA[k + 1]) >> 1) << 1);
		a_wR[2 * k] = (short)(a_wB[k] << 1);
		a_wR[2 * k + 1] = (short)((SND_pc_fn_wAddSat(a_wB[k],a_wB[k + 1]) >> 1) << 1);
	}
	SND_pc_fn_vPutFrames(pDst,a_wL,a_wR,bAdd);
}

void SND_pc_fn_vMixMono16HalfRate(struct _SND_pc_tdstSampleVoice *pVoice,char *pBuffer,long lLength,long lUnused,long bAdd)
{
	short wGainL, wGainR;
	short a_wS[4];
	unsigned long ulNb;
	long lPos, lEnd, k;
	const short *pData = (const short *)pVoice->pvData;
	short *pDst = (short *)pBuffer;

	SND_pc_fn_vGains8(pVoice,&wGainL,&wGainR);
	ulNb = (unsigned long)lLength >> 4;
	lPos = (long)((pVoice->ulPos & 0xFFFFFFFFUL) >> 8);
	lEnd = (long)((pVoice->ulEnd & 0xFFFFFFFFUL) >> 8) - 3;
	do
	{
		for (k = 0; k < 4; k++)
			a_wS[k] = pData[lPos + k];
		SND_pc_fn_vHalfRateBlock(a_wS,wGainL,wGainR,pDst,bAdd);
		pDst += 8;
		lPos += 2;
		if (lPos >= lEnd)
		{
			/* end of the data: one extra block (not counted) from the last samples and the first ones of
			 * the data */
			if (lPos - (lEnd + 3) == 1)
			{
				a_wS[0] = pData[lPos];
				a_wS[1] = pData[0];
				a_wS[2] = pData[1];
				a_wS[3] = 0;
			}
			else
			{
				a_wS[0] = pData[lPos];
				a_wS[1] = pData[lPos + 1];
				a_wS[2] = pData[0];
				a_wS[3] = pData[1];
			}
			SND_pc_fn_vHalfRateBlock(a_wS,wGainL,wGainR,pDst,bAdd);
			pDst += 8;
			if (!pVoice->bResLoop)
			{
				SND_pc_fn_vStopVoice(pVoice);
				return;
			}
			lPos = (long)((pVoice->ulLoop & 0xFFFFFFFFUL) >> 8);
		}
	} while (--ulNb);
	pVoice->ulPos = (unsigned long)lPos << 8;
}

void SND_pc_fn_vMixMono16DoubleRate(struct _SND_pc_tdstSampleVoice *pVoice,char *pBuffer,long lLength,long lUnused,long bAdd)
{
	short wGainL, wGainR;
	short a_wS[4], a_wOut[4];
	unsigned long ulNb;
	long lPos, lEnd, k;
	const short *pData = (const short *)pVoice->pvData;
	short *pDst = (short *)pBuffer;
	long bStop;

	SND_pc_fn_vGains8(pVoice,&wGainL,&wGainR);
	ulNb = (unsigned long)lLength >> 3;
	lPos = (long)((pVoice->ulPos & 0xFFFFFFFFUL) >> 8);
	lEnd = (long)((pVoice->ulEnd & 0xFFFFFFFFUL) >> 8) - 4;
	do
	{
		for (k = 0; k < 4; k++)
			a_wS[k] = pData[lPos + k];
		lPos += 4;
		bStop = 0;
		if (lPos > lEnd)
		{
			if (!pVoice->bResLoop)
				bStop = 1;	/* this block is still written */
			else
			{
				/* the loop's first two samples are added onto the block's last two */
				lPos = (long)((pVoice->ulLoop & 0xFFFFFFFFUL) >> 8);
				a_wS[2] = SND_pc_fn_wAddSat(a_wS[2],pData[lPos]);
				a_wS[3] = SND_pc_fn_wAddSat(a_wS[3],pData[lPos + 1]);
				lPos += 2;
			}
		}
		/* two frames from four samples: pair sums times the gain, >> 16 */
		for (k = 0; k < 2; k++)
		{
			a_wOut[2 * k] = SND_pc_fn_wPairGain(a_wS[2 * k],a_wS[2 * k + 1],wGainL);
			a_wOut[2 * k + 1] = SND_pc_fn_wPairGain(a_wS[2 * k],a_wS[2 * k + 1],wGainR);
		}
		SND_pc_fn_vPut4(pDst,a_wOut,bAdd);
		pDst += 4;
		if (bStop)
		{
			SND_pc_fn_vStopVoice(pVoice);
			return;
		}
	} while (--ulNb);
	pVoice->ulPos = (unsigned long)lPos << 8;
}

/* nearest sample, lanes in reversed order (the asm shifts each sample in from the bottom), so the frames of
 * a block come out last first; lanes 0, 2 and 1, 3 take the two words of each gain */
void SND_pc_fn_vMixMono16StepSwapPan(struct _SND_pc_tdstSampleVoice *pVoice,char *pBuffer,long lLength,long bSide,long bAdd)
{
	short wGainLo, wGainHi, wGainLo2, wGainHi2;
	short a_wD[4], a_wA[4], a_wB[4], a_wLo[4], a_wHi[4];
	unsigned long ulNb, ulPos;
	long lEnd, k;
	const short *pData = (const short *)pVoice->pvData;
	short *pDst;

	SND_pc_fn_vSplitGain(((unsigned long)pVoice->lVol * pVoice->lPan << 1) & 0xFFFFFFFFUL,&wGainLo,&wGainHi);
	SND_pc_fn_vSplitGain(((unsigned long)pVoice->lVol * (0x80 - pVoice->lPan) << 1) & 0xFFFFFFFFUL,&wGainLo2,&wGainHi2);
	ulNb = (unsigned long)lLength >> 4;
	ulPos = pVoice->ulPos;
	lEnd = (long)pVoice->ulEnd;
	pDst = bSide ? (short *)SND_pc_g_a_wSideBuffer : (short *)pBuffer;
	do
	{
		if (bSide == 1)
		{
			/* side path: one end test per block, a wrap restarts at ulLoop and skips the block */
			if ((long)ulPos >= lEnd)
			{
				if (!pVoice->bResLoop)
				{
					SND_pc_fn_vStopVoice(pVoice);
					return;
				}
				ulPos = pVoice->ulLoop;
				continue;
			}
			for (k = 3; k >= 0; k--)
			{
				a_wD[k] = pData[(ulPos & 0xFFFFFFFFUL) >> 8];
				ulPos += pVoice->lInc;
			}
		}
		else
		{
			/* the end is tested before each fetch; a wrap goes on at pos - ulEnd + ulLoop */
			for (k = 3; k >= 0; k--)
			{
				if ((long)ulPos >= lEnd)
				{
					ulPos -= lEnd;
					if (!pVoice->bResLoop)
					{
						SND_pc_fn_vStopVoice(pVoice);
						return;
					}
					ulPos += pVoice->ulLoop;
				}
				a_wD[k] = pData[(ulPos & 0xFFFFFFFFUL) >> 8];
				ulPos += pVoice->lInc;
			}
		}
		for (k = 0; k < 4; k++)
		{
			a_wA[k] = (short)(SND_pc_fn_wMulHigh(a_wD[k],(k & 1) ? wGainHi : wGainLo) << 1);
			a_wB[k] = (short)(SND_pc_fn_wMulHigh(a_wD[k],(k & 1) ? wGainHi2 : wGainLo2) << 1);
		}
		a_wLo[0] = a_wA[0]; a_wLo[1] = a_wB[0]; a_wLo[2] = a_wA[1]; a_wLo[3] = a_wB[1];
		a_wHi[0] = a_wA[2]; a_wHi[1] = a_wB[2]; a_wHi[2] = a_wA[3]; a_wHi[3] = a_wB[3];
		if (bSide == 1)
		{
			/* the side path stores the two halves the other way round */
			SND_pc_fn_vPut4(pDst,a_wHi,bAdd);
			SND_pc_fn_vPut4(pDst + 4,a_wLo,bAdd);
		}
		else
		{
			SND_pc_fn_vPut4(pDst,a_wLo,bAdd);
			SND_pc_fn_vPut4(pDst + 4,a_wHi,bAdd);
		}
		pDst += 8;
	} while (--ulNb);
	pVoice->ulPos = ulPos;
}

void SND_pc_fn_vMixMono16Step(struct _SND_pc_tdstSampleVoice *pVoice,char *pBuffer,long lLength,long bSide,long bAdd)
{
	short wGainL, wGainR;
	short a_wS[4], a_wL[4], a_wR[4];
	unsigned long ulNb, ulPos;
	long lEnd, k;
	const short *pData = (const short *)pVoice->pvData;
	short *pDst;

	wGainR = SND_pc_M_wLow((unsigned long)pVoice->lVol * pVoice->lPan << 1);
	wGainL = SND_pc_M_wLow((unsigned long)pVoice->lVol * (0x80 - pVoice->lPan) << 1);
	ulPos = pVoice->ulPos;
	lEnd = (long)pVoice->ulEnd;
	ulNb = (unsigned long)lLength >> 4;
	pDst = bSide ? (short *)SND_pc_g_a_wSideBuffer : (short *)pBuffer;
	do
	{
		/* the end is tested before each fetch; a wrap goes on at ulLoop + lInc - (pos - ulEnd) */
		for (k = 0; k < 4; k++)
		{
			if ((long)ulPos >= lEnd)
			{
				ulPos = pVoice->lInc - (ulPos - lEnd) + pVoice->ulLoop;
				if (!pVoice->bResLoop)
				{
					SND_pc_fn_vStopVoice(pVoice);
					return;
				}
			}
			a_wS[k] = pData[(ulPos & 0xFFFFFFFFUL) >> 8];
			ulPos += pVoice->lInc;
		}
		for (k = 0; k < 4; k++)
		{
			a_wL[k] = (short)(SND_pc_fn_wMulHigh(a_wS[k],wGainL) << 1);
			a_wR[k] = (short)(SND_pc_fn_wMulHigh(a_wS[k],wGainR) << 1);
		}
		SND_pc_fn_vPutFrames(pDst,a_wL,a_wR,bAdd);
		pDst += 8;
	} while (--ulNb);
	pVoice->ulPos = ulPos;
}

/* the 4 reversed lanes of the Echo/EchoSide/Comb mixers: no end test while fetching */
static void SND_pc_fn_vFetchReversed(SND_pc_tdstSampleVoice *pVoice,unsigned long *p_ulPos,short *pwD)
{
	const short *pData = (const short *)pVoice->pvData;
	long k;
	for (k = 3; k >= 0; k--)
	{
		pwD[k] = pData[(*p_ulPos & 0xFFFFFFFFUL) >> 8];
		*p_ulPos += pVoice->lInc;
	}
}

/* their gains: lanes A (lPan) and B (0x80 - lPan) of 4 reversed samples, no << 1; frames 0-1 in pwLo,
 * frames 2-3 in pwHi */
static void SND_pc_fn_vScaleReversed(SND_pc_tdstSampleVoice *pVoice,const short *pwD,short *pwLo,short *pwHi)
{
	short wALo, wAHi, wBLo, wBHi, a_wA[4], a_wB[4];
	long k;

	SND_pc_fn_vSplitGain(SND_pc_fn_ulGain8(pVoice->lVol,pVoice->lPan),&wALo,&wAHi);
	SND_pc_fn_vSplitGain(SND_pc_fn_ulGain8(pVoice->lVol,0x80 - pVoice->lPan),&wBLo,&wBHi);
	for (k = 0; k < 4; k++)
	{
		a_wA[k] = SND_pc_fn_wMulHigh(pwD[k],(k & 1) ? wAHi : wALo);
		a_wB[k] = SND_pc_fn_wMulHigh(pwD[k],(k & 1) ? wBHi : wBLo);
	}
	pwLo[0] = a_wA[0]; pwLo[1] = a_wB[0]; pwLo[2] = a_wA[1]; pwLo[3] = a_wB[1];
	pwHi[0] = a_wA[2]; pwHi[1] = a_wB[2]; pwHi[2] = a_wA[3]; pwHi[3] = a_wB[3];
}

static long SND_pc_fn_lEchoTap(long lIndex)
{
	return lIndex < 0x4000 ? lIndex + 0x4000 : lIndex - 0x4000;
}

void SND_pc_fn_vMixMono16Echo(struct _SND_pc_tdstSampleVoice *pVoice,char *pBuffer,long lLength,long lSlot,long lUnused)
{
	short a_wD[4], a_wLo[4], a_wHi[4];
	unsigned long ulPos;
	long lIndex, lTap, k;
	short *pDst = (short *)pBuffer;

	SND_pc_g_lEchoLoopCount = (long)((unsigned long)lLength >> 4);
	lIndex = SND_pc_s_a_lVoiceEchoIndex[lSlot];
	ulPos = pVoice->ulPos;
	for (;;)
	{
		SND_pc_fn_vFetchReversed(pVoice,&ulPos,a_wD);
		SND_pc_fn_vScaleReversed(pVoice,a_wD,a_wLo,a_wHi);
		if ((long)ulPos >= (long)pVoice->ulEnd)
		{
			/* the block is not written, the output still advances */
			if (!pVoice->bResLoop)
			{
				SND_pc_fn_vStopVoice(pVoice);
				break;
			}
			ulPos = pVoice->ulLoop;
		}
		else
		{
			for (k = 0; k < 4; k++)
				SND_pc_fn_vWrite(SND_pc_s_a_cVoiceEchoRing + lIndex + 2 * k,a_wLo[k]);
			lTap = SND_pc_fn_lEchoTap(lIndex);
			lIndex += 8;
			for (k = 0; k < 4; k++)
				pDst[k] = SND_pc_fn_wAddSat(SND_pc_fn_wAddSat(a_wLo[k],SND_pc_fn_wRead(SND_pc_s_a_cVoiceEchoRing + lTap + 2 * k)),pDst[k]);
			for (k = 0; k < 4; k++)
				SND_pc_fn_vWrite(SND_pc_s_a_cVoiceEchoRing + lIndex + 2 * k,a_wHi[k]);
			lTap = SND_pc_fn_lEchoTap(lIndex);
			for (k = 0; k < 4; k++)
				pDst[4 + k] = SND_pc_fn_wAddSat(SND_pc_fn_wAddSat(a_wHi[k],SND_pc_fn_wRead(SND_pc_s_a_cVoiceEchoRing + lTap + 2 * k)),pDst[4 + k]);
			lIndex += 8;
			if (lIndex >= 0x8000)
				lIndex = 0;
		}
		pDst += 8;
		if (--SND_pc_g_lEchoLoopCount == 0)
		{
			pVoice->ulPos = ulPos;
			break;
		}
	}
	SND_pc_s_a_lVoiceEchoIndex[lSlot] = lIndex;
}

void SND_pc_fn_vMixMono16EchoSide(struct _SND_pc_tdstSampleVoice *pVoice,char *pBuffer,long lLength,long lSlot,long lUnused)
{
	short a_wD[4], a_wLo[4], a_wHi[4];
	unsigned long ulPos;
	long lIndex, lTap, k;
	short wY;
	/* output into the echo ring, from its start, not into pBuffer */
	short *pDst = (short *)SND_pc_s_a_cEchoRing;

	SND_pc_g_lEchoLoopCount = (long)((unsigned long)lLength >> 4);
	lIndex = SND_pc_s_a_lVoiceEchoSideIndex[lSlot];
	/* the asm loads ulPos, then reuses the register for lSlot * 4: the fetch starts there */
	ulPos = (unsigned long)lSlot << 2;
	for (;;)
	{
		SND_pc_fn_vFetchReversed(pVoice,&ulPos,a_wD);
		SND_pc_fn_vScaleReversed(pVoice,a_wD,a_wLo,a_wHi);
		if ((long)ulPos >= (long)pVoice->ulEnd)
		{
			if (!pVoice->bResLoop)
			{
				SND_pc_fn_vStopVoice(pVoice);
				break;
			}
			ulPos = pVoice->ulLoop;
		}
		else
		{
			lTap = SND_pc_fn_lEchoTap(lIndex);
			for (k = 0; k < 4; k++)
			{
				wY = SND_pc_fn_wAddSat(a_wLo[k],SND_pc_fn_wRead(SND_pc_s_a_cVoiceEchoRing + lTap + 2 * k));
				SND_pc_fn_vWrite(SND_pc_s_a_cVoiceEchoRing + lIndex + 2 * k,(short)(wY >> 1));
				pDst[k] = SND_pc_fn_wAddSat(wY,pDst[k]);
			}
			lIndex += 8;
			lTap = SND_pc_fn_lEchoTap(lIndex);
			for (k = 0; k < 4; k++)
			{
				wY = SND_pc_fn_wAddSat(a_wHi[k],SND_pc_fn_wRead(SND_pc_s_a_cVoiceEchoRing + lTap + 2 * k));
				SND_pc_fn_vWrite(SND_pc_s_a_cVoiceEchoRing + lIndex + 2 * k,(short)(wY >> 1));
				pDst[4 + k] = SND_pc_fn_wAddSat(wY,pDst[4 + k]);
			}
			lIndex += 8;
			if (lIndex >= 0x8000)
				lIndex = 0;
		}
		pDst += 8;
		if (--SND_pc_g_lEchoLoopCount == 0)
		{
			pVoice->ulPos = ulPos;
			break;
		}
	}
	SND_pc_s_a_lVoiceEchoSideIndex[lSlot] = lIndex;
}

/* comb ring value: (y/2 + y) / 2, saturated */
static short SND_pc_fn_wCombFeed(short wY)
{
	return (short)(SND_pc_fn_wAddSat(wY >> 1,wY) >> 1);
}

void SND_pc_fn_vMixMono16Comb(struct _SND_pc_tdstSampleVoice *pVoice,char *pBuffer,long lLength,long lSlot,long lUnused)
{
	short a_wD[4], a_wLo[4], a_wHi[4];
	unsigned long ulPos;
	long lIndex, lTap, k;
	short *pDst = (short *)pBuffer;

	SND_pc_g_lEchoLoopCount = (long)((unsigned long)lLength >> 4);
	/* as EchoSide, the fetch starts at lSlot * 4. The ring index starts at the slot's delay (the asm
	 * loads the slot's index, then overwrites it), which is also the sweep's delay. lSlot counts the
	 * voices mixed so far: past slot 3 the asm reads beyond its 4-entry table; here the delay is 0
	 * (DEVIATIONS) */
	ulPos = (unsigned long)lSlot << 2;
	lIndex = (lSlot >= 0 && lSlot < 4) ? SND_pc_s_a_lVoiceCombDelay[lSlot] : 0;
	SND_pc_g_lSweepDelay = lIndex;
	for (;;)
	{
		SND_pc_fn_vFetchReversed(pVoice,&ulPos,a_wD);
		SND_pc_fn_vScaleReversed(pVoice,a_wD,a_wLo,a_wHi);
		if ((long)ulPos >= (long)pVoice->ulEnd)
		{
			if (!pVoice->bResLoop)
			{
				SND_pc_fn_vStopVoice(pVoice);
				break;
			}
			ulPos = pVoice->ulLoop;
		}
		else
		{
			lTap = lIndex < SND_pc_g_lSweepDelay ? lIndex + 0x8000 - SND_pc_g_lSweepDelay : lIndex - SND_pc_g_lSweepDelay;
			for (k = 0; k < 4; k++)
			{
				a_wLo[k] = SND_pc_fn_wSubSat(a_wLo[k],SND_pc_fn_wRead(SND_pc_s_a_cVoiceCombRing + lTap + 2 * k));
				SND_pc_fn_vWrite(SND_pc_s_a_cVoiceCombRing + lIndex + 2 * k,SND_pc_fn_wCombFeed(a_wLo[k]));
				pDst[k] = SND_pc_fn_wAddSat(a_wLo[k],pDst[k]);
			}
			lIndex += 8;
			/* the asm subtracts the second qword's delayed value only when the index is below the delay
			 * (the other branch subtracts it from the first qword, already stored) */
			if (lIndex < SND_pc_g_lSweepDelay)
			{
				lTap = lIndex + 0x8000 - SND_pc_g_lSweepDelay;
				for (k = 0; k < 4; k++)
					a_wHi[k] = SND_pc_fn_wSubSat(a_wHi[k],SND_pc_fn_wRead(SND_pc_s_a_cVoiceCombRing + lTap + 2 * k));
			}
			for (k = 0; k < 4; k++)
			{
				SND_pc_fn_vWrite(SND_pc_s_a_cVoiceCombRing + lIndex + 2 * k,SND_pc_fn_wCombFeed(a_wHi[k]));
				pDst[4 + k] = SND_pc_fn_wAddSat(a_wHi[k],pDst[4 + k]);
			}
			lIndex += 8;
			SND_pc_fn_vSweepStep();
			if (lIndex >= 0x8000)
				lIndex = 0;
		}
		pDst += 8;
		if (--SND_pc_g_lEchoLoopCount == 0)
		{
			pVoice->ulPos = ulPos;
			break;
		}
	}
	/* index and delay go back to slot 0 only */
	SND_pc_s_a_lVoiceCombIndex[0] = lIndex;
	SND_pc_s_a_lVoiceCombDelay[0] = SND_pc_g_lSweepDelay;
}

/*---- stereo 16-bit voices (frames of two words; no pan) */
static void SND_pc_fn_vScaleFrames(const short *pwS,short wGain,short *pwOut)
{
	long k;
	for (k = 0; k < 4; k++)
		pwOut[k] = SND_pc_fn_wMulHigh(pwS[k],wGain);
}

static void SND_pc_fn_vReadFrames(const short *pData,long lFrame,long lNbFrames,short *pwS)
{
	memcpy(pwS,pData + 2 * lFrame,lNbFrames * 2 * sizeof(short));
}

void SND_pc_fn_vMixStereo16Native(struct _SND_pc_tdstSampleVoice *pVoice,char *pBuffer,long lLength,long bSide,long bAdd)
{
	short wGain = SND_pc_M_wLow((unsigned long)pVoice->lVol << 8);
	short a_wS1[4], a_wS2[4], a_wOut1[4];
	/* the second qword's register: on an end at the first fetch the asm stores whatever it holds, the
	 * previous block's output (before the first block: 0, see DEVIATIONS) */
	short a_wOut2[4] = {0,0,0,0};
	unsigned long ulNb = (unsigned long)lLength >> 4;
	long lPos = (long)((pVoice->ulPos & 0xFFFFFFFFUL) >> 8);
	long lEnd = (long)((pVoice->ulEnd & 0xFFFFFFFFUL) >> 8);
	const short *pData = (const short *)pVoice->pvData;
	short *pDst = bSide ? (short *)SND_pc_g_a_wSideBuffer : (short *)pBuffer;
	long k, lShifted;

	do
	{
		SND_pc_fn_vReadFrames(pData,lPos,2,a_wS1);
		lPos += 2;
		if (lPos >= lEnd)
		{
			if (!pVoice->bResLoop)
			{
				/* end: stale second qword, first one at half gain */
				SND_pc_fn_vScaleFrames(a_wS1,wGain,a_wOut1);
				SND_pc_fn_vPut4(pDst,a_wOut1,bAdd);
				SND_pc_fn_vPut4(pDst + 4,a_wOut2,bAdd);
				SND_pc_fn_vStopVoice(pVoice);
				return;
			}
			lPos = (long)((pVoice->ulLoop & 0xFFFFFFFFUL) >> 8);
		}
		SND_pc_fn_vReadFrames(pData,lPos,2,a_wS2);
		lPos += 2;
		if (lPos >= lEnd)
		{
			if (!pVoice->bResLoop)
			{
				/* end: both qwords at half gain */
				SND_pc_fn_vScaleFrames(a_wS1,wGain,a_wOut1);
				SND_pc_fn_vScaleFrames(a_wS2,wGain,a_wOut2);
				SND_pc_fn_vPut4(pDst,a_wOut1,bAdd);
				SND_pc_fn_vPut4(pDst + 4,a_wOut2,bAdd);
				SND_pc_fn_vStopVoice(pVoice);
				return;
			}
			lPos = (long)((pVoice->ulLoop & 0xFFFFFFFFUL) >> 8);
		}
		SND_pc_fn_vScaleFrames(a_wS1,wGain,a_wOut1);
		SND_pc_fn_vScaleFrames(a_wS2,wGain,a_wOut2);
		/* psllq: the qword shifts left by one, each lane takes the top bit of the lane below */
		for (k = 3; k >= 0; k--)
		{
			lShifted = ((unsigned short)a_wOut1[k] << 1) | (k ? ((unsigned short)a_wOut1[k - 1] >> 15) : 0);
			a_wOut1[k] = SND_pc_M_wLow(lShifted);
			lShifted = ((unsigned short)a_wOut2[k] << 1) | (k ? ((unsigned short)a_wOut2[k - 1] >> 15) : 0);
			a_wOut2[k] = SND_pc_M_wLow(lShifted);
		}
		SND_pc_fn_vPut4(pDst,a_wOut1,bAdd);
		SND_pc_fn_vPut4(pDst + 4,a_wOut2,bAdd);
		/* the register holds what was stored */
		memcpy(a_wOut2,pDst + 4,sizeof(a_wOut2));
		pDst += 8;
	} while (--ulNb);
	pVoice->ulPos = (unsigned long)lPos << 8;
}

void SND_pc_fn_vMixStereo16HalfRate(struct _SND_pc_tdstSampleVoice *pVoice,char *pBuffer,long lLength,long bSide,long bAdd)
{
	short wGain = SND_pc_M_wLow((unsigned long)pVoice->lVol << 8);
	short a_wAB[4], a_wCD[4], a_wOut1[4], a_wOut2[4];
	unsigned long ulNb = (unsigned long)lLength >> 4;
	long lPos = (long)((pVoice->ulPos & 0xFFFFFFFFUL) >> 8);
	long lEnd = (long)((pVoice->ulEnd & 0xFFFFFFFFUL) >> 8);
	const short *pData = (const short *)pVoice->pvData;
	short *pDst = bSide ? (short *)SND_pc_g_a_wSideBuffer : (short *)pBuffer;

	do
	{
		/* frames A, B at the position; C, D one frame on (C = B unless it wrapped) */
		SND_pc_fn_vReadFrames(pData,lPos,2,a_wAB);
		lPos += 1;
		if (lPos >= lEnd)
		{
			if (!pVoice->bResLoop)
			{
				/* end: A at gain, then B, B unscaled */
				a_wOut1[0] = a_wOut1[2] = SND_pc_fn_wMulHigh(a_wAB[0],wGain);
				a_wOut1[1] = a_wOut1[3] = SND_pc_fn_wMulHigh(a_wAB[1],wGain);
				a_wOut2[0] = a_wOut2[2] = a_wAB[2];
				a_wOut2[1] = a_wOut2[3] = a_wAB[3];
				SND_pc_fn_vPut4(pDst,a_wOut1,bAdd);
				SND_pc_fn_vPut4(pDst + 4,a_wOut2,bAdd);
				SND_pc_fn_vStopVoice(pVoice);
				return;
			}
			lPos = (long)((pVoice->ulLoop & 0xFFFFFFFFUL) >> 8);
		}
		SND_pc_fn_vReadFrames(pData,lPos,2,a_wCD);
		lPos += 1;
		if (lPos >= lEnd)
		{
			if (!pVoice->bResLoop)
			{
				/* end: A, then B at gain */
				a_wOut1[0] = a_wOut1[2] = SND_pc_fn_wMulHigh(a_wAB[0],wGain);
				a_wOut1[1] = a_wOut1[3] = SND_pc_fn_wMulHigh(a_wAB[1],wGain);
				a_wOut2[0] = a_wOut2[2] = SND_pc_fn_wMulHigh(a_wAB[2],wGain);
				a_wOut2[1] = a_wOut2[3] = SND_pc_fn_wMulHigh(a_wAB[3],wGain);
				SND_pc_fn_vPut4(pDst,a_wOut1,bAdd);
				SND_pc_fn_vPut4(pDst + 4,a_wOut2,bAdd);
				SND_pc_fn_vStopVoice(pVoice);
				return;
			}
			lPos = (long)((pVoice->ulLoop & 0xFFFFFFFFUL) >> 8);
		}
		/* frames A, A+C, C, B+D (pair sums times the gain, >> 16) */
		a_wOut1[0] = SND_pc_fn_wPairGain(a_wAB[0],a_wAB[0],wGain);
		a_wOut1[1] = SND_pc_fn_wPairGain(a_wAB[1],a_wAB[1],wGain);
		a_wOut1[2] = SND_pc_fn_wPairGain(a_wAB[0],a_wCD[0],wGain);
		a_wOut1[3] = SND_pc_fn_wPairGain(a_wAB[1],a_wCD[1],wGain);
		a_wOut2[0] = SND_pc_fn_wPairGain(a_wCD[0],a_wCD[0],wGain);
		a_wOut2[1] = SND_pc_fn_wPairGain(a_wCD[1],a_wCD[1],wGain);
		a_wOut2[2] = SND_pc_fn_wPairGain(a_wAB[2],a_wCD[2],wGain);
		a_wOut2[3] = SND_pc_fn_wPairGain(a_wAB[3],a_wCD[3],wGain);
		SND_pc_fn_vPut4(pDst,a_wOut1,bAdd);
		SND_pc_fn_vPut4(pDst + 4,a_wOut2,bAdd);
		pDst += 8;
	} while (--ulNb);
	pVoice->ulPos = (unsigned long)lPos << 8;
}

void SND_pc_fn_vMixStereo16DoubleRate(struct _SND_pc_tdstSampleVoice *pVoice,char *pBuffer,long lLength,long bSide,long bAdd)
{
	short wGain = SND_pc_M_wLow((unsigned long)pVoice->lVol << 8);
	short a_wAB[4], a_wCD[4], a_wOut[4];
	unsigned long ulNb = (unsigned long)lLength >> 3;
	long lPos = (long)((pVoice->ulPos & 0xFFFFFFFFUL) >> 8);
	long lEnd = (long)((pVoice->ulEnd & 0xFFFFFFFFUL) >> 8);
	const short *pData = (const short *)pVoice->pvData;
	short *pDst = bSide ? (short *)SND_pc_g_a_wSideBuffer : (short *)pBuffer;

	do
	{
		SND_pc_fn_vReadFrames(pData,lPos,2,a_wAB);
		lPos += 2;
		if (lPos >= lEnd)
		{
			if (!pVoice->bResLoop)
			{
				SND_pc_fn_vStopVoice(pVoice);	/* the block is not written */
				return;
			}
			lPos = (long)((pVoice->ulLoop & 0xFFFFFFFFUL) >> 8);
		}
		SND_pc_fn_vReadFrames(pData,lPos,2,a_wCD);
		lPos += 2;
		if (lPos >= lEnd)
		{
			if (!pVoice->bResLoop)
			{
				SND_pc_fn_vStopVoice(pVoice);
				return;
			}
			lPos = (long)((pVoice->ulLoop & 0xFFFFFFFFUL) >> 8);
		}
		/* the asm's lane pairing: L = A.R + B.L, R = A.R + B.R (C, D alike) */
		a_wOut[0] = SND_pc_fn_wPairGain(a_wAB[1],a_wAB[2],wGain);
		a_wOut[1] = SND_pc_fn_wPairGain(a_wAB[1],a_wAB[3],wGain);
		a_wOut[2] = SND_pc_fn_wPairGain(a_wCD[1],a_wCD[2],wGain);
		a_wOut[3] = SND_pc_fn_wPairGain(a_wCD[1],a_wCD[3],wGain);
		SND_pc_fn_vPut4(pDst,a_wOut,bAdd);
		pDst += 4;
	} while (--ulNb);
	pVoice->ulPos = (unsigned long)lPos << 8;
}

void SND_pc_fn_vMixStereo16Step(struct _SND_pc_tdstSampleVoice *pVoice,char *pBuffer,long lLength,long bSide,long bAdd)
{
	short wGain = SND_pc_M_wLow((unsigned long)pVoice->lVol << 8);
	short a_wS[8], a_wOut[8];
	unsigned long ulNb = (unsigned long)lLength >> 4;
	unsigned long ulPos = pVoice->ulPos;
	long lEnd = (long)pVoice->ulEnd;
	const short *pData = (const short *)pVoice->pvData;
	short *pDst = bSide ? (short *)SND_pc_g_a_wSideBuffer : (short *)pBuffer;
	long k;

	do
	{
		/* the end is tested before each fetch; a wrap goes on at pos - ulEnd + ulLoop */
		for (k = 0; k < 4; k++)
		{
			if ((long)ulPos >= lEnd)
			{
				ulPos -= lEnd;
				if (!pVoice->bResLoop)
				{
					SND_pc_fn_vStopVoice(pVoice);	/* the block is not written */
					return;
				}
				ulPos += pVoice->ulLoop;
			}
			SND_pc_fn_vReadFrames(pData,(long)((ulPos & 0xFFFFFFFFUL) >> 8),1,a_wS + 2 * k);
			ulPos += pVoice->lInc;
		}
		for (k = 0; k < 8; k++)
			a_wOut[k] = (short)(SND_pc_fn_wMulHigh(a_wS[k],wGain) << 1);
		SND_pc_fn_vPut4(pDst,a_wOut,bAdd);
		SND_pc_fn_vPut4(pDst + 4,a_wOut + 4,bAdd);
		pDst += 8;
	} while (--ulNb);
	pVoice->ulPos = ulPos;
}

#endif /* OPTIMIZED_FOR_MMX_WITH_ASM */
