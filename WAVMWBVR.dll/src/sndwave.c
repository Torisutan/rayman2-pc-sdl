#include "SNDinc.h"

#ifndef NO_ACP_SOUND
#if defined(_DLL_COMPILATION_MODE)
#include "snddll.h"
#else
#include "sndthrd.h"
#endif
#include "sndres.h"
#include "sndwave.h"
#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <process.h>

typedef struct _SND_pc_tdstSampleVoice {
	unsigned long			ulPos;
	long					lInc;
	unsigned long			ulEnd;
	unsigned long			ulLoop;
	void					*pvData;
	long					lVol;
	tdstBlockResourceMem	*pRes;
	SND_td_pfn_vSoundCallback fn_callback;
	long					lPan;
	SndBool					b8Bits;
	SndBool					bResLoop;
	SndBool					bPlaying;
	SndBool					bLooping;
	SndBool					bCallback;
	SndBool					bPitchable;
	long					l3C;
	SndBool					bAudible;
	long					lSpace;
	long					par_callback;
	long					prio;
	long					lNbIncSteps;
	long					lIncRampLength;
	long					lIncStep;
	SndBool					bPaused;
	SndBool					bStereo;
	SndBool					bBuffer;
	long					l68;
	long					l6C;
} SND_pc_tdstSampleVoice;

#define SND_pc_C_NbVoices 64

typedef struct _SND_pc_tdstBufferClient *SND_pc_p_tdstBufferClient;

SNDLST2_M_DynamicUseListOf(SND_pc_p_tdstBufferClient);

typedef struct _SND_pc_tdstBufferClient {
	SND_tduTypeCallback		eType;
	long					voice;
	SndBool					bPaused;
	tdstBlockResourceMem	*pRes;
	tdstBlockResourceMem	stRes;
	unsigned long			ulNbSamples;
	long					lShift;
	union {
		struct {
			unsigned long				ulLastPos;
			td_pfn_vRefreshBufferClient	callback;
			long						user_id;
		} stAsync;
		struct {
			td_pfn_vRefreshBufferClient	callback;
			long						user_id;
		} stSync;
	} uCallback;
	SNDLST2_M_DynamicElementDeclaration(SND_pc_p_tdstBufferClient)
} SND_pc_tdstBufferClient;

typedef struct _SND_pc_tdstBufferClientList {
	SNDLST2_M_DynamicAnchorDeclaration(SND_pc_p_tdstBufferClient) pList;
} SND_pc_tdstBufferClientList;

typedef struct _SND_pc_tdstInitMessage {
	SND_tdstInitStruct	*pInitStruct;
	int					iResult;
} SND_pc_tdstInitMessage;

long			SND_pc_lEffect_198;
long			SND_pc_lEffectState;

#define SND_pc_C_NbBuffers 100

SND_pc_tdhDevice	SND_pc_hWaveOut = NULL;
SND_pc_tdhThread	SND_pc_hThread = NULL;
WAVEFORMATEX	SND_pc_stWaveFormat = {0};
long			SND_pc_lNbStats = 0;
SND_pc_tdhLock	SND_pc_hMutexDX = NULL;
double			SND_pc_dAvgInQueue = 0;
UINT			SND_pc_uiMsgLibereDriver = 0;
DWORD			SND_pc_dwThreadId = 0;
long			SND_pc_lNbUnderrun = 0;
long			SND_pc_a_lBufferState[SND_pc_C_NbBuffers] = {0};
long			SND_pc_lLastLockedOperation = 0;
double			SND_pc_dAvgQueued = 0;
double			SND_pc_dAvgDone = 0;
char			SND_pc_szDebugStats[0x100] = {0};
UINT			SND_pc_uiMsgInitSnd = 0;
char			*SND_pc_a_pBuffers[SND_pc_C_NbBuffers] = {0};
SndBool			SND_pc_b8BitsOutput = FALSE;
WAVEHDR			SND_pc_a_stWaveHdr[SND_pc_C_NbBuffers] = {0};
SND_pc_tdstBufferClientList	SND_pc_stBufferClients = {0};
SND_pc_tdhLock	SND_pc_hSampleMutex = NULL;
UINT			SND_pc_uiMsgDesInitSnd = 0;
UINT			SND_pc_uiMsgPrendDriver = 0;
SndBool			SND_pc_bDebugStats = FALSE;
SndBool			SND_pc_bReverse = FALSE;
SndBool			SND_pc_bDolby = FALSE;
long			SND_pc_l27C74 = 0;
long			SND_pc_lNextBuffer = 0;
SndBool			SND_pc_bDriverTaken = FALSE;
SndBool			SND_pc_bInitDone = FALSE;
SndBool			SND_pc_bReleased = FALSE;
SndBool			SND_pc_bForceReleased = FALSE;

long			SND_pc_l27C8C = 0;
long			SND_pc_a27C90[13] = {0};

extern HWND		SND_pc_hMainWindow;
extern HINSTANCE SND_pc_hProcessInstance;
extern HINSTANCE SND_pc_hFirstProcessInstance;

SND_pc_tdstPerfCounter	SND_pc_stPerfFrequency_9785;
SND_pc_tdstPerfCounter	SND_pc_stPerfCounter;
SND_pc_tdstSampleVoice		SND_pc_a64_stVoice_9095[SND_pc_C_NbVoices];

SndBool	SND_pc_bStereo = TRUE;
long	SND_pc_lBufferSize = 0x7D00;
long	SND_pc_lNbBuffers = 100;
long	SND_pc_l21E04 = 25;
long	SND_pc_lBufferLength = 0x370;
long	SND_pc_lLatency = 0x1588;
SND_pc_tdstSampleVoice *SND_pc_pFirstVoice = &SND_pc_a64_stVoice_9095[0];
SND_pc_tdstSampleVoice *SND_pc_pLastVoice = &SND_pc_a64_stVoice_9095[SND_pc_C_NbVoices];
long	SND_pc_l21E18 = 0x70;
SndBool	SND_pc_bRetryDriver = TRUE;
long	SND_pc_l21E20 = 0x10000;
long	SND_pc_l21E24 = 0x4000;
SampleParam C_SAMPLEPARAM_NEUTRE={C_SNDREAL_1,C_SOUNDVOL_MAXI,C_SOUNDPAN_MEDIAN,C_SOUNDSPACE_FRONT,NO_REVERB};
long	SND_pc_l21E30 = 200;
long	SND_pc_l21E34 = 0x400;
long	SND_pc_l21E38 = 8;
char	SND_pc_szTargetLabel[] = "mw";

char			*SND_pc_pBufferData_73344;

SND_pc_tdstPerfCounter	SND_pc_stMixStart;
SND_pc_tdstPerfCounter	SND_pc_stMixEnd_2638;
double			SND_pc_dMixTime_12;
double			SND_pc_dMixTotal_5463;
long			SND_pc_lNbMix_5999;

/* 0x10007e60 */
typedef struct _SND_pc_tdstReverbParam {
	float	fLowCut;
	float	fHighCut;
	long	lFilterType;
	long	lNbTaps;
	SndBool	bReady;
	short	*pwCoefs;
} SND_pc_tdstReverbParam;
SND_pc_tdstReverbParam SND_pc_stReverbParam;

/* 0x100082b0 */
short			*SND_pc_pw28464_5;
short			*SND_pc_pw35D98_9554;

long			SND_pc_lNbAudible_1293;

typedef struct _SND_pc_tdstCallbackThread {
	SND_td_pfn_vSoundCallback	fn_callback;
	long						par_callback;
} SND_pc_tdstCallbackThread;

/* 0x1000D380 */
void	SND_pc_fn_vClearReverbSend(long lOffset,long lLength);
void	SND_pc_fn_vMixMono16NativeSend(SND_pc_tdstSampleVoice *pVoice,char *pBuffer,long lLength,long bSide,long lOffset,long lUnused);
void	SND_pc_fn_vMixMono16StepSend(SND_pc_tdstSampleVoice *pVoice,char *pBuffer,long lLength,long bSide,long lOffset,long lUnused);
void	SND_pc_fn_vMixReverbFir32(char *pBuffer,short *pwCoefs,long lLength,long lUnused);
void	SND_pc_fn_vApplyEchoDelay(char *pBuffer,long lLength,long lMode);
void	SND_pc_fn_vApplySweptComb(char *pBuffer,long lLength,long lUnused);
void	SND_pc_fn_vApplySideEcho(char *pBuffer,long lLength);
void	SND_pc_fn_vFillBuffer(char *pBuffer,long lValue,long lLength);
void	SND_pc_fn_vMixMono16Native(SND_pc_tdstSampleVoice *pVoice,char *pBuffer,long lLength,long lUnused,long bAdd);
void	SND_pc_fn_vMixMono16HalfRate(SND_pc_tdstSampleVoice *pVoice,char *pBuffer,long lLength,long lUnused,long bAdd);
void	SND_pc_fn_vMixMono16DoubleRate(SND_pc_tdstSampleVoice *pVoice,char *pBuffer,long lLength,long lUnused,long bAdd);
void	SND_pc_fn_vMixMono16Step(SND_pc_tdstSampleVoice *pVoice,char *pBuffer,long lLength,long bSide,long bAdd);
void	SND_pc_fn_vMixMono16Comb(SND_pc_tdstSampleVoice *pVoice,char *pBuffer,long lLength,long lSlot,long lUnused);
void	SND_pc_fn_vMixStereo16Native(SND_pc_tdstSampleVoice *pVoice,char *pBuffer,long lLength,long bSide,long bAdd);
void	SND_pc_fn_vMixStereo16HalfRate(SND_pc_tdstSampleVoice *pVoice,char *pBuffer,long lLength,long bSide,long bAdd);
void	SND_pc_fn_vMixStereo16DoubleRate(SND_pc_tdstSampleVoice *pVoice,char *pBuffer,long lLength,long bSide,long bAdd);
void	SND_pc_fn_vMixStereo16Step(SND_pc_tdstSampleVoice *pVoice,char *pBuffer,long lLength,long bSide,long bAdd);

void FUN_10007e60(SND_pc_tdstReverbParam *pParam,long lFreq)
{
	double	*pCoef;
	double	dSum;
	float	fLow;
	float	fHigh;
	long	lHalf;
	long	i;
	long	j;

	fLow = pParam->fLowCut / (lFreq * 0.5f);
	fHigh = pParam->fHighCut / (lFreq * 0.5f);
	lHalf = pParam->lNbTaps / 2;
	pCoef = (double *)SND_fn_pvMallocSnd(pParam->lNbTaps * sizeof(double));
	switch (pParam->lFilterType)
	{
	case 1:
		pCoef[lHalf] = fHigh;
		for (i = 1; i < lHalf; i++)
			pCoef[lHalf - i] = pCoef[lHalf + i] = sin(i * fHigh * 3.1415) / ((float)i * 3.1415);
		pCoef[0] = sin(lHalf * fHigh * 3.1415) / ((float)lHalf * 3.1415);
		break;
	case 2:
		pCoef[lHalf] = 1.0f - fLow;
		for (i = 1; i < lHalf; i++)
			pCoef[lHalf - i] = pCoef[lHalf + i] = -(sin(i * fLow * 3.1415) / ((float)i * 3.1415));
		pCoef[0] = -(sin(lHalf * fLow * 3.1415) / ((float)lHalf * 3.1415));
		break;
	case 3:
		pCoef[lHalf] = fHigh - fLow;
		for (i = 1; i < lHalf; i++)
			pCoef[lHalf + i] = pCoef[lHalf - i] = (sin(i * fHigh * 3.1415) - sin(i * fLow * 3.1415)) / ((float)i * 3.1415);
		pCoef[0] = (sin(lHalf * fHigh * 3.1415) - sin(lHalf * fLow * 3.1415)) / ((float)lHalf * 3.1415);
		break;
	case 4:
		pCoef[0] = 0;
		pCoef[lHalf] = 0;
		for (i = 1; i < lHalf; i++)
		{
			if (i % 2)
				pCoef[lHalf - i] = -4.0 / ((float)i * 3.1415);
			else
				pCoef[lHalf - i] = 0;
			pCoef[lHalf + i] = -pCoef[lHalf - i];
		}
		break;
	case 5:
		for (i = -lHalf; i < lHalf; i++)
		{
			dSum = 0.0;
			for (j = -128; j < 128; j++)
			{
				if ((j - i) % 2)
				{
					if (j != 0)
						dSum += ((sin(j * fHigh * 3.1415) - sin(j * fLow * 3.1415)) / ((float)j * 3.1415)) * (-4.0 / ((j - i) * 3.1415));
					else
						dSum += (fHigh - fLow) * (-4.0 / ((j - i) * 3.1415));
				}
			}
			pCoef[i + lHalf] = dSum;
		}
		break;
	default:
		pCoef[0] = 1.0;
		pCoef[lHalf] = 1.0;
		for (i = 1; i < lHalf; i++)
		{
			pCoef[lHalf - i] = 1.0;
			pCoef[lHalf + i] = 1.0;
		}
		break;
	}
	dSum = 0.0;
	for (i = 0; i < pParam->lNbTaps; i++)
		dSum += fabs(pCoef[i]);
	pParam->pwCoefs = (short *)SND_fn_pvMallocSnd(pParam->lNbTaps * sizeof(short));
	for (i = 0; i < pParam->lNbTaps; i++)
		pParam->pwCoefs[i] = (short)(pCoef[i] * 65535.0);
	pParam->bReady = TRUE;
	SND_fn_vFreeSnd(pCoef);
}

void FUN_100082a0(SND_pc_tdstReverbParam stParam)
{
	SND_fn_vFreeSnd(stParam.pwCoefs);
}

void FUN_100082b0(void)
{
	SND_pc_pw28464_5 = (short *)SND_fn_pvMallocSnd(SND_pc_lBufferSize * sizeof(short) + 8);
	memset(SND_pc_pw28464_5,0,SND_pc_lBufferSize * sizeof(short));
	SND_pc_pw35D98_9554 = SND_pc_pw28464_5 + SND_pc_lBufferSize;
}

void FUN_10008300(void)
{
	SND_fn_vFreeSnd(SND_pc_pw28464_5);
}

void SND_pc_fn_vRunVoiceCallback(SND_pc_tdstCallbackThread *pThread)
{
	pThread->fn_callback(pThread->par_callback);
	SND_pc_M_vFreeCallback(pThread);
}

void __stdcall SND_pc_fn_vStartVoiceCallbackThread(SND_pc_tdstSampleVoice *pVoice)
{
	SND_pc_tdstCallbackThread *pThread;

	pThread = (SND_pc_tdstCallbackThread *)SND_pc_M_pvAllocCallback(sizeof(SND_pc_tdstCallbackThread));
	pThread->fn_callback = pVoice->fn_callback;
	pThread->par_callback = pVoice->par_callback;
	SND_pc_M_vStartCallbackThread((void (__cdecl *)(void *))SND_pc_fn_vRunVoiceCallback,pThread);
}

void FUN_10008360(SND_pc_tdstSampleVoice *pVoice,char *pBuffer,long lLength)
{
	if (!pVoice->bPitchable)
	{
		switch (pVoice->lInc)
		{
		case 0x80:
			SND_pc_fn_vMixStereo16HalfRate(pVoice,pBuffer,lLength,0,1);
			break;
		case 0x100:
			SND_pc_fn_vMixStereo16Native(pVoice,pBuffer,lLength,0,1);
			break;
		case 0x200:
			SND_pc_fn_vMixStereo16DoubleRate(pVoice,pBuffer,lLength,0,1);
			break;
		default:
			SND_pc_fn_vMixStereo16Step(pVoice,pBuffer,lLength,0,1);
			break;
		}
	}
	else
		SND_pc_fn_vMixStereo16Step(pVoice,pBuffer,lLength,0,1);
}

void FUN_100083f0(SND_pc_tdstSampleVoice *pVoice,char *pBuffer,long lLength)
{
	if (!pVoice->bPitchable)
	{
		switch (pVoice->lInc)
		{
		case 0x80:
			SND_pc_fn_vMixMono16HalfRate(pVoice,pBuffer,lLength,0,1);
			break;
		case 0x100:
			SND_pc_fn_vMixMono16Native(pVoice,pBuffer,lLength,0,1);
			break;
		case 0x200:
			SND_pc_fn_vMixMono16DoubleRate(pVoice,pBuffer,lLength,0,1);
			break;
		default:
			SND_pc_fn_vMixMono16Step(pVoice,pBuffer,lLength,0,1);
			break;
		}
	}
	else
		SND_pc_fn_vMixMono16Step(pVoice,pBuffer,lLength,0,1);
}

void FUN_10008480(char *pBuffer,long lLength)
{
	SND_pc_tdstSampleVoice *pVoice;
	long lNb;
	long lVolume;
	SndBool bFilter;

	pVoice = SND_pc_pFirstVoice;
	lNb = 0;
	bFilter = FALSE;
	lVolume = 0;
	SND_pc_lNbAudible_1293 = 0;
	SND_pc_fn_vFillBuffer(pBuffer,0,lLength);
	if (lLength & 0xF)
		bFilter = TRUE;
	do
	{
		if (lLength && pVoice->bPlaying && !pVoice->bPaused)
		{
			if (pVoice->bAudible)
			{
				lVolume += pVoice->lVol;
				SND_pc_lNbAudible_1293++;
				if (pVoice->bPitchable)
				{
					if (SND_pc_lEffect_198 == 6)
					{
						switch (pVoice->l3C)
						{
						case 0:
							SND_pc_fn_vMixMono16Step(pVoice,pBuffer,lLength,0,1);
							lNb++;
							break;
						case 1:
							SND_pc_fn_vMixMono16Step(pVoice,pBuffer,lLength,1,1);
							lNb++;
							bFilter = TRUE;
							break;
						case 0x100:
							SND_pc_fn_vMixMono16Comb(pVoice,pBuffer,lLength,lNb,1);
							break;
						}
					}
					else
					{
						if (pVoice->bStereo)
							FUN_10008360(pVoice,pBuffer,lLength);
						else
							FUN_100083f0(pVoice,pBuffer,lLength);
						lNb++;
					}
				}
				else
				{
					if (SND_pc_lEffect_198 == 6)
					{
						switch (pVoice->l3C)
						{
						case 0:
							SND_pc_fn_vMixMono16Native(pVoice,pBuffer,lLength,0,1);
							break;
						case 1:
							SND_pc_fn_vMixMono16Native(pVoice,pBuffer,lLength,0,1);
							bFilter = TRUE;
							break;
						case 0x100:
							SND_pc_fn_vMixMono16Native(pVoice,pBuffer,lLength,0,1);
							break;
						}
					}
					else
					{
						if (pVoice->bStereo)
							FUN_10008360(pVoice,pBuffer,lLength);
						else
							FUN_100083f0(pVoice,pBuffer,lLength);
						lNb++;
					}
				}
			}
			else
			{
				pVoice->ulPos += pVoice->lInc * ((unsigned long)lLength >> 1);
				if (pVoice->ulPos >= pVoice->ulEnd)
				{
					if (pVoice->bResLoop)
						pVoice->ulPos = pVoice->ulLoop;
					else
						pVoice->bPlaying = FALSE;
				}
			}
			if (!pVoice->bPlaying && pVoice->bCallback)
			{
				pVoice->bCallback = FALSE;
				SND_pc_fn_vStartVoiceCallbackThread(pVoice);
			}
		}
		pVoice++;
	} while (pVoice < SND_pc_pLastVoice);
	if (lLength)
	{
		switch (SND_pc_lEffect_198)
		{
		case 1:
			SND_pc_fn_vApplyEchoDelay(pBuffer,lLength,0);
			break;
		case 2:
			SND_pc_fn_vApplyEchoDelay(pBuffer,lLength,1);
			break;
		case 3:
			SND_pc_fn_vApplyEchoDelay(pBuffer,lLength,2);
			break;
		case 4:
			SND_pc_fn_vApplyEchoDelay(pBuffer,lLength,3);
			break;
		case 5:
			SND_pc_fn_vApplySweptComb(pBuffer,lLength,0);
			break;
		case 6:
			if (bFilter)
				SND_pc_fn_vApplySideEcho(pBuffer,lLength);
			break;
		}
	}
}

void FUN_100086f0(char *pBuffer,long lLength)
{
	SND_pc_tdstSampleVoice *pVoice;
	long lNb;
	long lVolume;
	SndBool bFilter;

	pVoice = SND_pc_pFirstVoice;
	lNb = 0;
	bFilter = FALSE;
	lVolume = 0;
	SND_pc_lNbAudible_1293 = 0;
	SND_pc_fn_vFillBuffer(pBuffer,0,lLength);
	SND_pc_fn_vClearReverbSend((pBuffer - SND_pc_pBufferData_73344) / 2,lLength / 2);
	if (lLength & 0xF)
		bFilter = TRUE;
	do
	{
		if (lLength && pVoice->bPlaying)
		{
			if (pVoice->bAudible)
			{
				lVolume += pVoice->lVol;
				SND_pc_lNbAudible_1293++;
				if (pVoice->bPitchable)
				{
					if (SND_pc_lEffect_198 == 6)
					{
						switch (pVoice->l3C)
						{
						case 0:
							SND_pc_fn_vMixMono16Step(pVoice,pBuffer,lLength,0,1);
							lNb++;
							break;
						case 1:
							SND_pc_fn_vMixMono16Step(pVoice,pBuffer,lLength,0,1);
							lNb++;
							bFilter = TRUE;
							break;
						case 0x100:
							SND_pc_fn_vMixMono16Comb(pVoice,pBuffer,lLength,lNb,1);
							break;
						}
					}
					else
					{
						if (pVoice->bStereo)
							FUN_10008360(pVoice,pBuffer,lLength);
						else
							SND_pc_fn_vMixMono16StepSend(pVoice,pBuffer,lLength,0,(pBuffer - SND_pc_pBufferData_73344) / 2,1);
						break;
					}
				}
				else
				{
					if (SND_pc_lEffect_198 == 6)
					{
						switch (pVoice->l3C)
						{
						case 0:
						case 1:
						case 0x100:
							SND_pc_fn_vMixMono16NativeSend(pVoice,pBuffer,lLength,0,(pBuffer - SND_pc_pBufferData_73344) / 2,1);
							break;
						}
					}
					else
					{
						if (pVoice->bStereo)
							FUN_10008360(pVoice,pBuffer,lLength);
						else
							SND_pc_fn_vMixMono16NativeSend(pVoice,pBuffer,lLength,0,(pBuffer - SND_pc_pBufferData_73344) / 2,1);
						break;
					}
				}
			}
			else
			{
				pVoice->ulPos += pVoice->lInc * ((unsigned long)lLength >> 2);
				if (pVoice->ulPos > pVoice->ulEnd)
				{
					if (pVoice->bResLoop)
						pVoice->ulPos = pVoice->ulLoop;
					else
						pVoice->bPlaying = FALSE;
				}
			}
			if (!pVoice->bPlaying && pVoice->bCallback)
				SND_pc_fn_vStartVoiceCallbackThread(pVoice);
		}
		pVoice++;
	} while (pVoice < SND_pc_pLastVoice);
	if (lLength)
	{
		switch (SND_pc_lEffect_198)
		{
		case 1:
			SND_pc_fn_vApplyEchoDelay(pBuffer,lLength,0);
			break;
		case 2:
			SND_pc_fn_vApplyEchoDelay(pBuffer,lLength,1);
			break;
		case 3:
			SND_pc_fn_vApplyEchoDelay(pBuffer,lLength,2);
			break;
		case 4:
			SND_pc_fn_vApplyEchoDelay(pBuffer,lLength,3);
			break;
		case 5:
			SND_pc_fn_vApplySweptComb(pBuffer,lLength,0);
			break;
		case 6:
			if (bFilter)
				SND_pc_fn_vApplySideEcho(pBuffer,lLength);
			break;
		}
	}
	if (SND_pc_lNbAudible_1293)
		SND_pc_fn_vMixReverbFir32(pBuffer,SND_pc_stReverbParam.pwCoefs,lLength,(pBuffer - SND_pc_pBufferData_73344) / 2);
}

void FUN_100089a0(short *pBuffer,unsigned long ulLength,long lThreshold)
{
	short *pRight;
	short *pPrevL;
	short *pPrevR;
	unsigned long n;
	long i;
	short sShift;
	short sDiff;

	if (pBuffer != (short *)SND_pc_pBufferData_73344)
	{
		pPrevL = pBuffer - 2;
		pPrevR = pBuffer - 1;
	}
	else
	{
		pPrevL = (short *)(SND_pc_pBufferData_73344 + SND_pc_lBufferSize) - 2;
		pPrevR = (short *)(SND_pc_pBufferData_73344 + SND_pc_lBufferSize) - 1;
	}
	sDiff = abs(*pPrevL - *pBuffer);
	pRight = pBuffer + 2;
	if (lThreshold < 0x200)
		lThreshold = 0x200;
	if (sDiff > lThreshold)
	{
		for (n = 32, sShift = 5; sShift; n >>= 1, sShift--)
		{
			if (ulLength > n * 4)
			{
				for (i = 1; i < n; i++)
				{
					*pBuffer = (short)((*pPrevL * (long)(n - i) + *pBuffer * i) >> sShift);
					pBuffer += 2;
				}
				break;
			}
		}
	}
	sDiff = abs(*pPrevR - *pRight);
	if (sDiff > lThreshold)
	{
		for (n = 32, sShift = 5; sShift; n >>= 1, sShift--)
		{
			if (ulLength > n * 4)
			{
				for (i = 1; i < n; i++)
				{
					*pRight = (short)((*pPrevR * (long)(n - i) + *pRight * i) >> sShift);
					pRight += 2;
				}
				break;
			}
		}
	}
}

long	SND_pc_fn_lOpenMixerVolume(SND_pc_tdhDevice hWaveOut);
unsigned long SND_pc_fn_ulGetMixerVolume(void);
void	SND_pc_fn_vSetMixerVolume(unsigned long ulVolume);
long	SND_pc_fn_lCloseMixerVolume(void);

void	SND_pc_fn_vSelectAudibleVoices(void);

void FUN_10008ae0(void)
{
	SND_pc_stReverbParam.fLowCut = 100.0f;
	SND_pc_stReverbParam.fHighCut = 7000.0f;
	SND_pc_stReverbParam.lNbTaps = 0x20;
	SND_pc_stReverbParam.lFilterType = 5;
	SND_pc_stReverbParam.bReady = FALSE;
	FUN_10007e60(&SND_pc_stReverbParam,SND_pc_stWaveFormat.nSamplesPerSec);
}

void FUN_10008b30(void)
{
	FUN_100082a0(SND_pc_stReverbParam);
}

void SND_fn_vGetTargetLabelSample(char* name,int nb_char)
{
	strncpy(name,SND_pc_szTargetLabel,nb_char);
}

void SND_fn_vSetupTargetSample(void)
{
}

SndBool SND_fn_bCanSetupTargetSample(void)
{
	return TRUE;
}

void FUN_10008b90(void)
{
}

DWORD SND_pc_fn_ulWaitObjectOrBreak(HANDLE hHandle,DWORD dwMilliseconds)
{
	DWORD dwResult;

	dwResult = SND_pc_M_ulWaitLock(hHandle,50000);
	if (dwResult == SND_pc_C_ulWaitTimedOut)
		SND_pc_M_vBreak();
	return dwResult;
}

void FUN_10008bd0(SND_pc_tdstBufferClient *pBuffer)
{
	unsigned long ulPos;
	long lNb;

	ulPos = ((SND_pc_a64_stVoice_9095[pBuffer->voice].ulPos < SND_pc_a64_stVoice_9095[pBuffer->voice].ulEnd) ? SND_pc_a64_stVoice_9095[pBuffer->voice].ulPos : SND_pc_a64_stVoice_9095[pBuffer->voice].ulEnd) >> 8;
	lNb = ulPos - pBuffer->uCallback.stAsync.ulLastPos;
	if (lNb >= 0)
		pBuffer->uCallback.stAsync.callback(pBuffer->uCallback.stAsync.user_id,pBuffer->uCallback.stAsync.ulLastPos,lNb,(char *)pBuffer->stRes.uRes.stSample.uData.stMem.pvPtrFirst + (pBuffer->uCallback.stAsync.ulLastPos << pBuffer->lShift));
	else
	{
		pBuffer->uCallback.stAsync.callback(pBuffer->uCallback.stAsync.user_id,pBuffer->uCallback.stAsync.ulLastPos,pBuffer->ulNbSamples - pBuffer->uCallback.stAsync.ulLastPos,(char *)pBuffer->stRes.uRes.stSample.uData.stMem.pvPtrFirst + (pBuffer->uCallback.stAsync.ulLastPos << pBuffer->lShift));
		pBuffer->uCallback.stAsync.callback(pBuffer->uCallback.stAsync.user_id,0,ulPos,pBuffer->stRes.uRes.stSample.uData.stMem.pvPtrFirst);
	}
	pBuffer->uCallback.stAsync.ulLastPos = (pBuffer->ulNbSamples != ulPos) ? ulPos : 0;
}

void FUN_10008c60(SND_pc_tdstBufferClient *pBuffer)
{
	pBuffer->uCallback.stSync.callback(pBuffer->uCallback.stSync.user_id,0,pBuffer->ulNbSamples,pBuffer->stRes.uRes.stSample.uData.stMem.pvPtrFirst);
	SND_pc_a64_stVoice_9095[pBuffer->voice].ulPos = 0;
}

void SND_pc_fn_vMixAndWriteBuffer(long lBuffer)
{
	char szError[256];
	SND_pc_tdstPerfCounter stDelta;
	SND_pc_tdstBufferClient *pBuffer;
	SND_pc_tdstBufferClient *pNext;
	MMRESULT mmr;
	long i;

	SND_pc_M_ulLockOrBreak(SND_pc_hSampleMutex);
	SND_pc_lLastLockedOperation = 3;
	SND_pc_a_stWaveHdr[lBuffer].dwUser = lBuffer;
	SND_pc_a_stWaveHdr[lBuffer].dwFlags |= WHDR_DONE;
	SND_pc_M_bGetPerfCounter(&SND_pc_stMixStart);
	if (SND_pc_bDolby)
		FUN_100086f0(SND_pc_a_pBuffers[lBuffer],SND_pc_lBufferLength);
	else
		FUN_10008480(SND_pc_a_pBuffers[lBuffer],SND_pc_lBufferLength);
	SNDLST2_M_DynamicForEachMovingElementOf(&SND_pc_stBufferClients.pList,pBuffer,pNext,i)
	{
		if (!pBuffer->bPaused && (pBuffer->voice == -1 || !SND_pc_a64_stVoice_9095[pBuffer->voice].bPaused))
		{
			switch (pBuffer->eType)
			{
			case BUFFER_ASYNCHRONE:
				FUN_10008bd0(pBuffer);
				break;
			case BUFFER_SYNCHRONE:
				FUN_10008c60(pBuffer);
				break;
			}
		}
	}
	SND_pc_M_bGetPerfCounter(&SND_pc_stMixEnd_2638);
	stDelta.QuadPart = SND_pc_stMixEnd_2638.QuadPart - SND_pc_stMixStart.QuadPart;
	SND_pc_dMixTime_12 = (double)stDelta.QuadPart / (double)SND_pc_stPerfFrequency_9785.QuadPart;
	SND_pc_dMixTotal_5463 += SND_pc_dMixTime_12;
	if (++SND_pc_lNbMix_5999 >= 100)
	{
		stDelta.QuadPart = SND_pc_stMixEnd_2638.QuadPart - SND_pc_stPerfCounter.QuadPart;
		SND_pc_dMixTotal_5463 /= (double)stDelta.QuadPart / (double)SND_pc_stPerfFrequency_9785.QuadPart;
#ifdef SUPERVISATER
		dbgSND_fn_vSetProcessTimeSample((float)SND_pc_dMixTotal_5463);
#endif
		SND_pc_stPerfCounter = SND_pc_stMixEnd_2638;
		SND_pc_lNbMix_5999 = 0;
		SND_pc_dMixTotal_5463 = 0;
	}
	SND_pc_M_bReleaseLock(SND_pc_hSampleMutex);
	if (SND_pc_hMutexDX)
		SND_pc_M_ulLockOrBreak(SND_pc_hMutexDX);
	mmr = SND_pc_M_ulWriteBuffer(SND_pc_hWaveOut,&SND_pc_a_stWaveHdr[lBuffer]);
	if (mmr)
	{
		SND_pc_M_ulGetDeviceErrorText(mmr,szError,256);
		SND_pc_M_vErrorBox(szError,"erreur avec waveWrite");
	}
	if (SND_pc_hMutexDX)
		SND_pc_M_bReleaseLock(SND_pc_hMutexDX);
}

void FUN_10008ed0(void)
{
	int i;

	SND_pc_lBufferSize = SND_pc_lBufferLength * SND_pc_lNbBuffers;
	SND_pc_pBufferData_73344 = (char *)SND_fn_pvMallocSnd(SND_pc_lBufferSize + 8);
	memset(SND_pc_pBufferData_73344,0,SND_pc_lBufferSize);
	for (i = 0; i < SND_pc_lNbBuffers; i++)
		SND_pc_a_pBuffers[i] = SND_pc_pBufferData_73344 + i * SND_pc_lBufferLength;
	FUN_100082b0();
	FUN_10008ae0();
}

void FUN_10008f40(void)
{
	SND_fn_vFreeSnd(SND_pc_pBufferData_73344);
	SND_pc_pBufferData_73344 = NULL;
	FUN_10008300();
	FUN_10008b30();
}

void SND_pc_fn_vOpenWaveOut(void)
{
	char szError[256];
	MMRESULT mmr;
	int i;

	if (SND_pc_hMutexDX)
		SND_pc_M_ulLockOrBreak(SND_pc_hMutexDX);
	SND_pc_stWaveFormat.wFormatTag = WAVE_FORMAT_PCM;
	SND_pc_stWaveFormat.nChannels = 2;
	SND_pc_stWaveFormat.nSamplesPerSec = 22050;
	SND_pc_stWaveFormat.wBitsPerSample = 16;
	SND_pc_stWaveFormat.nAvgBytesPerSec = 88200;
	SND_pc_stWaveFormat.nBlockAlign = 4;
	SND_pc_stWaveFormat.cbSize = 0;
	if (SND_pc_bRetryDriver)
	{
		mmr = SND_pc_M_ulOpenDevice(&SND_pc_hWaveOut,&SND_pc_stWaveFormat);
		if (mmr)
		{
			SND_pc_M_ulGetDeviceErrorText(mmr,szError,256);
			SND_pc_bDriverTaken = FALSE;
		}
		else
		{
			SND_pc_bDriverTaken = TRUE;
			for (i = 0; i < SND_pc_lNbBuffers; i++)
			{
				memset(&SND_pc_a_stWaveHdr[i],0,sizeof(WAVEHDR));
				SND_pc_a_stWaveHdr[i].lpData = SND_pc_a_pBuffers[i];
				SND_pc_a_stWaveHdr[i].dwBufferLength = SND_pc_lBufferLength;
				SND_pc_a_stWaveHdr[i].dwFlags = 0;
				SND_pc_a_stWaveHdr[i].dwLoops = 0;
				SND_pc_a_stWaveHdr[i].dwUser = i;
				if (SND_pc_bDriverTaken)
				{
					mmr = SND_pc_M_ulPrepareBuffer(SND_pc_hWaveOut,&SND_pc_a_stWaveHdr[i]);
					if (mmr)
					{
						SND_pc_M_ulGetDeviceErrorText(mmr,szError,256);
						SND_pc_M_ulCloseDevice(SND_pc_hWaveOut);
						SND_pc_bDriverTaken = FALSE;
					}
					if (SND_pc_bDriverTaken)
					{
						mmr = SND_pc_M_ulWriteBuffer(SND_pc_hWaveOut,&SND_pc_a_stWaveHdr[i]);
						if (mmr)
						{
							SND_pc_M_ulGetDeviceErrorText(mmr,szError,256);
							SND_pc_M_vErrorBox(szError,"erreur avec waveWrite");
							SND_pc_M_ulCloseDevice(SND_pc_hWaveOut);
							SND_pc_bDriverTaken = FALSE;
						}
					}
				}
				SND_pc_a_lBufferState[i] = 0;
				SND_pc_lNextBuffer = (i + 1) % SND_pc_lNbBuffers;
			}
		}
	}
	if (SND_pc_bDriverTaken)
		SND_pc_fn_lOpenMixerVolume(SND_pc_hWaveOut);
	else
		SND_pc_fn_lOpenMixerVolume(NULL);
	if (SND_pc_hMutexDX)
		SND_pc_M_bReleaseLock(SND_pc_hMutexDX);
}

void FUN_10009140(void)
{
	SND_pc_fn_vOpenWaveOut();
}

void SND_pc_fn_vCloseWaveOut(void)
{
	if (SND_pc_bInitDone && SND_pc_bDriverTaken)
	{
		if (SND_pc_hMutexDX)
			SND_pc_M_ulLockOrBreak(SND_pc_hMutexDX);
		SND_pc_fn_lCloseMixerVolume();
		SND_pc_bDriverTaken = FALSE;
		SND_pc_M_ulResetDevice(SND_pc_hWaveOut);
		SND_pc_M_ulCloseDevice(SND_pc_hWaveOut);
		if (SND_pc_hMutexDX)
			SND_pc_M_bReleaseLock(SND_pc_hMutexDX);
	}
}

void SND_pc_fn_vDesInitWaveOut(void)
{
	int i;

	if (SND_pc_bInitDone)
	{
		SND_pc_bRetryDriver = FALSE;
		if (SND_pc_bDriverTaken)
		{
			SND_pc_fn_lCloseMixerVolume();
			SND_pc_bDriverTaken = FALSE;
			SND_pc_M_ulResetDevice(SND_pc_hWaveOut);
			SND_pc_M_ulCloseDevice(SND_pc_hWaveOut);
		}
		SND_pc_bInitDone = FALSE;
		SND_pc_M_ulLockOrBreak(SND_pc_hSampleMutex);
		SND_pc_lLastLockedOperation = 9;
		for (i = 0; i < SND_pc_C_NbVoices; i++)
			SND_fn_vStopSample(i);
		FUN_10008f40();
		SND_pc_M_bReleaseLock(SND_pc_hSampleMutex);
		SND_pc_M_bDeleteLock(SND_pc_hSampleMutex);
	}
}

void FUN_10009250(void)
{
	SND_fn_vDesInitSample();
}

int SND_pc_fn_iInitWaveOut(SND_pc_tdstInitMessage *pMsg)
{
	char szText[80];

	SND_pc_stWaveFormat.wFormatTag = WAVE_FORMAT_PCM;
	SND_pc_stWaveFormat.nChannels = 2;
	SND_pc_stWaveFormat.nSamplesPerSec = 22050;
	SND_pc_stWaveFormat.wBitsPerSample = 16;
	SND_pc_stWaveFormat.nAvgBytesPerSec = 88200;
	SND_pc_stWaveFormat.nBlockAlign = 4;
	SND_pc_stWaveFormat.cbSize = 0;
	if (!SND_fn_bTestSnd_WinMM(&SND_pc_stWaveFormat))
	{

		sprintf(szText,"%d bits,%d Hz,%s SoundDriver",SND_pc_stWaveFormat.wBitsPerSample,SND_pc_stWaveFormat.nSamplesPerSec,(SND_pc_stWaveFormat.wBitsPerSample == 2) ? "stereo" : "mono");
		SND_M_DisplayError(5,szText);
		SND_pc_bDriverTaken = FALSE;
		SND_pc_bRetryDriver = FALSE;
		return C_INIT_FAILED;
	}
	SND_pc_hMainWindow = pMsg->pInitStruct->hMainWindow;
	SND_pc_hProcessInstance = pMsg->pInitStruct->hProcessInstance;
	if (!SND_pc_hFirstProcessInstance)
		SND_pc_hFirstProcessInstance = SND_pc_hProcessInstance;
	SND_pc_hMutexDX = pMsg->pInitStruct->hMutexDX;
	SND_pc_bDriverTaken = TRUE;
	SND_pc_bRetryDriver = TRUE;
	SND_pc_hSampleMutex = SND_pc_M_hCreateLock(TRUE);
	FUN_10008ed0();
	FUN_10009140();
	SND_pc_l27C74 = 0;
	SND_pc_b8BitsOutput = (SND_pc_stWaveFormat.wBitsPerSample == 8);
	SND_fn_vSetStereoSample(TRUE);
	SND_pc_M_bReleaseLock(SND_pc_hSampleMutex);
	atexit(FUN_10009250);
	return SND_pc_bDriverTaken ? C_INIT_OK : C_INIT_FAILED;
}

DWORD WINAPI SND_pc_fn_ulWaveOutThread(LPVOID lpParameter)
{
	MSG msg;
	char szText[128];
	SND_pc_tdhEvent hEvent;
	LPARAM lParam;
	long lBuffer;
	long lNbDone;
	long lNbInQueue;
	DWORD *pdwFlags;
	DWORD dwFlags;
	long i;
	long lNext;
	long lNbQueued;

	memset(&msg,0,sizeof(msg));
#ifndef DISABLE_RECORD
	SND_fn_bManageMsgForRecord((long)&msg);
#endif
	SND_pc_M_bPeekMessage(&msg);
	SND_pc_M_bSetEvent((SND_pc_tdhEvent)lpParameter);
	SND_pc_M_vRaiseThreadPriority();
	while (1)
	{
		if (!SND_pc_M_bGetMessage(&msg))
			break;
		hEvent = (SND_pc_tdhEvent)msg.wParam;
		lParam = msg.lParam;
#ifndef DISABLE_RECORD
		if (SND_fn_bManageMsgForRecord((long)&msg))
			continue;
#endif
		if (msg.message == MM_WOM_DONE)
		{
			lBuffer = ((WAVEHDR *)lParam)->dwUser;
			SND_pc_a_lBufferState[lBuffer % SND_pc_lNbBuffers] = 1;
			if (SND_pc_bDebugStats)
			{
				lNbDone = 0;
				lNbInQueue = 0;
				for (i = 0, pdwFlags = &SND_pc_a_stWaveHdr[0].dwFlags; i < SND_pc_lNbBuffers; i++, pdwFlags += sizeof(WAVEHDR) / sizeof(DWORD))
				{
					dwFlags = *pdwFlags;
					if (dwFlags & WHDR_DONE)
						lNbDone++;
					if ((dwFlags & WHDR_INQUEUE) && !(dwFlags & WHDR_DONE))
						lNbInQueue++;
					if (SND_pc_lNbStats == 4)
					{
						if (i == lBuffer % SND_pc_lNbBuffers)
							strcat(SND_pc_szDebugStats,"recu:");
						sprintf(szText,"%d:%x\n",i,dwFlags);
						strcat(SND_pc_szDebugStats,szText);
					}
				}
				SND_pc_lNbStats++;
				for (i = lBuffer + 1; i < lBuffer + SND_pc_lNbBuffers; i++)
				{
					if (!(SND_pc_a_stWaveHdr[i % SND_pc_lNbBuffers].dwFlags & WHDR_INQUEUE) || (SND_pc_a_stWaveHdr[i].dwFlags & WHDR_DONE))
						break;
				}
				lNbQueued = i - lBuffer - 1;
				SND_pc_dAvgQueued = (SND_pc_dAvgQueued * (SND_pc_lNbStats - 1) + lNbQueued) / SND_pc_lNbStats;
				SND_pc_dAvgDone = (SND_pc_dAvgDone * (SND_pc_lNbStats - 1) + lNbDone) / SND_pc_lNbStats;
				SND_pc_dAvgInQueue = (SND_pc_dAvgInQueue * (SND_pc_lNbStats - 1) + lNbInQueue) / SND_pc_lNbStats;
				if (!lNbQueued)
					SND_pc_lNbUnderrun++;
			}
			if (SND_pc_bDriverTaken)
			{
				lNext = SND_pc_lNextBuffer % SND_pc_lNbBuffers;
				SND_pc_fn_vSelectAudibleVoices();
				while (lNext != lBuffer)
				{
					if ((lNext - lBuffer + SND_pc_lNbBuffers) % SND_pc_lNbBuffers >= SND_pc_l21E04)
						break;
					if (!SND_pc_a_lBufferState[(lNext + SND_pc_lNbBuffers - SND_pc_l21E04 + 1) % SND_pc_lNbBuffers])
						break;
					SND_pc_fn_vMixAndWriteBuffer(lNext);
					SND_pc_a_lBufferState[lNext] = 0;
					lNext = (lNext + 1) % SND_pc_lNbBuffers;
				}
				SND_pc_lNextBuffer = lNext % SND_pc_lNbBuffers;
			}
		}
		else if (msg.message == SND_pc_uiMsgInitSnd)
		{
			SND_pc_fn_iInitWaveOut((SND_pc_tdstInitMessage *)lParam);
			((SND_pc_tdstInitMessage *)lParam)->iResult = C_INIT_OK;
			SND_pc_M_bSetEvent(hEvent);
		}
		else if (msg.message == SND_pc_uiMsgLibereDriver)
		{
			SND_pc_M_ulLockOrBreak(SND_pc_hSampleMutex);
			SND_pc_lLastLockedOperation = 5;
			SND_pc_fn_vCloseWaveOut();
			SND_pc_M_bReleaseLock(SND_pc_hSampleMutex);
			SND_pc_M_bSetEvent(hEvent);
		}
		else if (msg.message == SND_pc_uiMsgPrendDriver)
		{
			SND_pc_M_ulLockOrBreak(SND_pc_hSampleMutex);
			SND_pc_lLastLockedOperation = 7;
			SND_pc_fn_vOpenWaveOut();
			SND_pc_M_bReleaseLock(SND_pc_hSampleMutex);
			SND_pc_M_bSetEvent(hEvent);
		}
		else if (msg.message == SND_pc_uiMsgDesInitSnd)
			break;
	}
	SND_pc_fn_vDesInitWaveOut();
	SND_pc_M_bSetEvent(hEvent);
	SND_pc_M_vExitThread();
	return 0;
}

void SND_pc_fn_vSelectAudibleVoices(void)
{
	SND_pc_tdstSampleVoice *a10_pVoice[10];
	SND_pc_tdstSampleVoice **ppVoice;
	SND_pc_tdstSampleVoice *pVoice;
	long lNb;
	int i;

	pVoice = SND_pc_pFirstVoice;
	lNb = 0;
	if (SND_pc_bDriverTaken)
	{
		memset(a10_pVoice,0,sizeof(a10_pVoice));
		ppVoice = a10_pVoice;
		do
		{
			i = 0;
			pVoice->bAudible = FALSE;
			if (pVoice->lVol > 3 && pVoice->bPlaying && !pVoice->bPaused)
			{
				if (lNb < 10)
				{
					lNb++;
					*ppVoice++ = pVoice;
				}
				else
				{
					for (i = 0; i < 10; i++)
					{
						if (a10_pVoice[i]->prio < pVoice->prio)
							break;
						if (a10_pVoice[i]->prio == pVoice->prio && a10_pVoice[i]->lVol < pVoice->lVol)
							break;
					}
					if (i < 10)
						a10_pVoice[i] = pVoice;
				}
			}
			pVoice++;
		} while (pVoice < SND_pc_pLastVoice);
		for (i = 0; i < lNb; i++)
			a10_pVoice[i]->bAudible = TRUE;
	}
	else
	{
		for (i = 0; i < 10; i++)
			SND_pc_a64_stVoice_9095[i].bAudible = FALSE;
	}
}

long FUN_10009880(tdstBlockResourceMem *pRes,SndReal rFreq)
{
	if (!rFreq)
		return 1;
	return SND_fn_rMulRealRealSnd(pRes->uRes.stSample.ulInc0,rFreq);
}

SndBool FUN_100098a0(void)
{
	return TRUE;
}

SndBool FUN_100098b0(long voice,SampleParam *par)
{
	SND_pc_tdstSampleVoice *pVoice;
	tdstBlockResourceMem *pRes;
	long lInc;
	long lDelta;
	long lNbSteps;

	pVoice = &SND_pc_a64_stVoice_9095[voice];
	pRes = pVoice->pRes;
	if (pVoice->bPlaying)
	{
		pVoice->lVol = (par->ucVol * pVoice->pRes->ucVolume) >> 7;
		if (pVoice->bStereo && !pVoice->bBuffer)
			pVoice->lVol >>= 1;
		lInc = FUN_10009880(pRes,par->Freq);
		pVoice->lNbIncSteps = 0;
		if (pVoice->lIncRampLength)
		{
			lDelta = abs(lInc - pVoice->lInc);
			lNbSteps = (lDelta + pVoice->lIncRampLength - 1) / pVoice->lIncRampLength;
			if (lNbSteps)
			{
				pVoice->lIncStep = lDelta / lNbSteps;
				if (lInc > pVoice->lInc)
					pVoice->lNbIncSteps = lNbSteps;
				else
					pVoice->lNbIncSteps = -lNbSteps;
			}
		}
		else
		{
			pVoice->lNbIncSteps = 0;
			pVoice->lInc = lInc;
		}
		pVoice->lIncRampLength = 0;
		if (SND_pc_bStereo)
		{
			if (SND_pc_bReverse)
				pVoice->lPan = 0x7F - par->ucPan;
			else
				pVoice->lPan = par->ucPan;
			pVoice->lPan = max(min(pVoice->lPan,0x7F),0);
		}
		else
			pVoice->lPan = 0x40;
		pVoice->lSpace = par->ucSpace;
		return TRUE;
	}
#ifdef SUPERVISATER
	dbgSND_fn_vDelVoiceSample(voice);
#endif
	return FALSE;
}

void FUN_100099f0(long voice)
{
	SND_pc_a64_stVoice_9095[voice].bLooping = FALSE;
	SND_pc_a64_stVoice_9095[voice].bPlaying = FALSE;
#ifdef SUPERVISATER
	dbgSND_fn_vDelVoiceSample(voice);
#endif
	if (SND_pc_a64_stVoice_9095[voice].bCallback)
	{
		SND_pc_a64_stVoice_9095[voice].bCallback = FALSE;
		SND_pc_a64_stVoice_9095[voice].fn_callback(SND_pc_a64_stVoice_9095[voice].par_callback);
	}
}

void FUN_10009a40(void)
{
}

/* 0x10009a50 */
typedef int SND_pc_tdKnob9A50_0,SND_pc_tdKnob9A50_1,SND_pc_tdKnob9A50_2,SND_pc_tdKnob9A50_3,SND_pc_tdKnob9A50_4;

long FUN_10009a50(tdstBlockResourceMem *pRes,SampleParam *par,long prio,SND_td_pfn_vSoundCallback fn_callback,long par_callback,SndBool bBuffer)
{
	SND_pc_tdstSampleVoice *pVoice;
	long voice;

	for (voice = 0; voice < SND_pc_C_NbVoices; voice++)
		if (!SND_pc_a64_stVoice_9095[voice].bPlaying)
			break;
	if (voice == SND_pc_C_NbVoices)
	{
		SND_M_DisplayErrorEx(0x17,"",3);
		return C_PLAY_FAILED;
	}
	pVoice = &SND_pc_a64_stVoice_9095[voice];
	pVoice->bBuffer = bBuffer;
	pVoice->prio = prio;
	pVoice->pvData = pRes->uRes.stSample.uData.stMem.pvPtrFirst;
	pVoice->ulPos = 0;
	pVoice->ulEnd = pRes->uRes.stSample.uData.stMem.ulNbEch << 8;
	pVoice->ulLoop = pRes->uRes.stSample.uData.stMem.ulNbEchLoop << 8;
	pVoice->pRes = pRes;
	pVoice->bResLoop = pRes->uRes.stSample.bLoop;
	pVoice->bLooping = FALSE;
	pVoice->bCallback = FALSE;
	pVoice->lIncRampLength = 0;
	pVoice->lNbIncSteps = 0;
	pVoice->l3C = 0;
	pVoice->bPitchable = pRes->uRes.stSample.bPitchable;
	if (pRes->uRes.stSample.uwNbChannels == 2)
		pVoice->bStereo = TRUE;
	else
		pVoice->bStereo = FALSE;
	pVoice->lVol = (par->ucVol * pRes->ucVolume) >> 7;
	if (pVoice->bStereo && !bBuffer)
		pVoice->lVol >>= 1;
	if (pVoice->lVol < 3)
		pVoice->bAudible = FALSE;
	else
		pVoice->bAudible = TRUE;
	pVoice->lInc = FUN_10009880(pRes,par->Freq);
	if (pRes->uRes.stSample.uwResolution == 16)
		pVoice->b8Bits = FALSE;
	else
		pVoice->b8Bits = TRUE;
	if (SND_pc_bStereo)
	{
		if (SND_pc_bReverse)
			pVoice->lPan = 0x7F - par->ucPan;
		else
			pVoice->lPan = par->ucPan;
		pVoice->lPan = max(min(pVoice->lPan,0x7F),0);
	}
	else
		pVoice->lPan = 0x40;
	pVoice->lSpace = par->ucSpace;
	pVoice->bPaused = FALSE;
	if (fn_callback)
	{
		pVoice->bCallback = TRUE;
		pVoice->fn_callback = fn_callback;
		pVoice->par_callback = par_callback;
	}
	else if (pRes->uRes.stSample.bLoop)
		pVoice->bLooping = TRUE;
	pVoice->pRes = pRes;
	pVoice->bPlaying = TRUE;
#ifdef SUPERVISATER
	dbgSND_fn_vAddVoiceSample(voice,*(tduRefRes *)&pRes,par);
#endif
	return voice;
}

long FUN_10009c10(void)
{
	return -1;
}

void SND_pc_fn_vTakeWaveOut(void)
{
	SND_pc_tdhEvent hEvent;

	if (SND_pc_M_ulGetCurrentThreadId() != SND_pc_dwThreadId)
	{
		hEvent = SND_pc_M_hCreateEvent();
		if (!SND_pc_M_bPostMessage(SND_pc_dwThreadId,SND_pc_uiMsgPrendDriver,(WPARAM)hEvent,0))
			SND_M_DisplayError(0x1D,"Error on PostThreadMessage");
		SND_pc_M_ulWaitEventOrBreak(hEvent);
		SND_pc_M_bDeleteEvent(hEvent);
	}
	else
	{
		SND_pc_M_ulLockOrBreak(SND_pc_hSampleMutex);
		SND_pc_lLastLockedOperation = 6;
		SND_pc_fn_vOpenWaveOut();
		SND_pc_M_bReleaseLock(SND_pc_hSampleMutex);
	}
}

void SND_pc_fn_vReleaseWaveOut(void)
{
	SND_pc_tdhEvent hEvent;

	if (SND_pc_bInitDone && SND_pc_bDriverTaken)
	{
		if (SND_pc_M_ulGetCurrentThreadId() != SND_pc_dwThreadId)
		{
			hEvent = SND_pc_M_hCreateEvent();
			if (!SND_pc_M_bPostMessage(SND_pc_dwThreadId,SND_pc_uiMsgLibereDriver,(WPARAM)hEvent,0))
				SND_M_DisplayError(0x1D,"Error on PostThreadMessage");
			SND_pc_M_ulWaitEventOrBreak(hEvent);
			SND_pc_M_bDeleteEvent(hEvent);
			SND_pc_bDriverTaken = FALSE;
		}
		else
		{
			SND_pc_M_ulLockOrBreak(SND_pc_hSampleMutex);
			SND_pc_lLastLockedOperation = 4;
			SND_pc_fn_vCloseWaveOut();
			SND_pc_M_bReleaseLock(SND_pc_hSampleMutex);
		}
	}
}

void SND_fn_vSetReverseSample(SndBool active)
{
	int i;

	SND_pc_M_ulLockOrBreak(SND_pc_hSampleMutex);
	SND_pc_lLastLockedOperation = 10;
	if ((active && !SND_pc_bReverse) || (!active && SND_pc_bReverse))
	{
		for (i = 0; i < SND_pc_C_NbVoices; i++)
			SND_pc_a64_stVoice_9095[i].lPan = 0x7F - SND_pc_a64_stVoice_9095[i].lPan;
	}
	SND_pc_bReverse = active;
	SND_pc_M_bReleaseLock(SND_pc_hSampleMutex);
}

SndBool SND_fn_bGetReverseSample(void)
{
	return SND_pc_bReverse;
}

__declspec(dllexport) void SND_CALL SND_fn_vSetDolbySample(SndBool active)
{
}

__declspec(dllexport) SndBool SND_CALL SND_fn_bGetDolbySample(void)
{
	return SND_pc_bDolby;
}

long SND_fn_lPlaySample(tduRefRes res,SampleParam *par,long prio,SND_td_pfn_vSoundCallback fn_callback,long par_callback)
{
	if (res.pstPtr->bIsLoaded && (SND_pc_bDriverTaken || res.pstPtr->uRes.stSample.bLoop))
		return FUN_10009a50(res.pstPtr,par,prio,fn_callback,par_callback,FALSE);
	return C_PLAY_FAILED;
}

void SND_fn_vRemoveCallbackSample(long voice)
{
}

SndBool SND_fn_bSetParamSample(long voice,SampleParam *par)
{
#ifdef SUPERVISATER
	dbgSND_fn_vSetParVoiceSample(voice,par);
#endif
	return FUN_100098b0(voice,par);
}

SndBool SND_fn_bTestIsPlayingSample(long voice)
{
#ifdef SUPERVISATER
	if (!SND_pc_a64_stVoice_9095[voice].bPlaying)
		dbgSND_fn_vDelVoiceSample(voice);
#endif
	return SND_pc_a64_stVoice_9095[voice].bPlaying;
}

void SND_fn_vStopSample(long voice)
{
	SND_pc_M_ulLockOrBreak(SND_pc_hSampleMutex);
	SND_pc_lLastLockedOperation = 1;
	FUN_100099f0(voice);
	SND_pc_M_bReleaseLock(SND_pc_hSampleMutex);
}

void SND_fn_vPauseSample(long voice)
{
	SND_pc_a64_stVoice_9095[voice].bPaused = TRUE;
	SND_pc_a64_stVoice_9095[voice].lNbIncSteps = 0;
	SND_pc_a64_stVoice_9095[voice].lIncRampLength = 0;
#ifdef SUPERVISATER
	dbgSND_fn_vPauseVoiceSample(voice);
#endif
}

void SND_fn_vResumeSample(long voice)
{
	SND_pc_a64_stVoice_9095[voice].bPaused = FALSE;
	SND_pc_a64_stVoice_9095[voice].lNbIncSteps = 0;
	SND_pc_a64_stVoice_9095[voice].lIncRampLength = 0;
#ifdef SUPERVISATER
	dbgSND_fn_vResumeVoiceSample(voice);
#endif
}

void SND_fn_vSetStereoSample(SndBool active)
{
	SND_pc_bStereo = active;
}

SndBool SND_fn_bGetStereoSample(void)
{
	return SND_pc_bStereo;
}

int SND_fn_iInitSample(SND_tdstInitStruct *pInitStruct)
{
	SND_pc_tdhEvent hEvent;
	SND_pc_tdstInitMessage *pMsg;
	int iResult;

	SND_pc_uiMsgInitSnd = SND_pc_M_uiRegisterMessage("MSG_InitSnd");
	SND_pc_uiMsgDesInitSnd = SND_pc_M_uiRegisterMessage("MSG_DesInitSnd");
	SND_pc_uiMsgLibereDriver = SND_pc_M_uiRegisterMessage("MSG_LibereDriver");
	SND_pc_uiMsgPrendDriver = SND_pc_M_uiRegisterMessage("MSG_PrendDriver");

	hEvent = SND_pc_M_hCreateEvent();
	SND_pc_hThread = SND_pc_M_hCreateThread(SND_pc_fn_ulWaveOutThread,hEvent,&SND_pc_dwThreadId);
	if (SND_pc_hThread == NULL)
	{
		SND_M_DisplayError(0x1E,"");
		return C_INIT_FAILED;
	}
	SND_pc_M_ulWaitEventOrBreak(hEvent);
	SND_pc_M_bDeleteEvent(hEvent);

	pMsg = (SND_pc_tdstInitMessage *)SND_fn_pvMallocSndEx(0,sizeof(SND_pc_tdstInitMessage));
	pMsg->pInitStruct = pInitStruct;
	pMsg->iResult = C_INIT_FAILED;
	hEvent = SND_pc_M_hCreateEvent();
	if (!SND_pc_M_bPostMessage(SND_pc_dwThreadId,SND_pc_uiMsgInitSnd,(WPARAM)hEvent,(LPARAM)pMsg))
		SND_M_DisplayError(0x1D,"Error on PostThreadMessage");
	SND_pc_M_ulWaitEventOrBreak(hEvent);

	SND_pc_M_bGetPerfFrequency(&SND_pc_stPerfFrequency_9785);
	SND_pc_M_bGetPerfCounter(&SND_pc_stPerfCounter);

	while (1)	/* 0x1000acb0 */
	{
		if (SND_pc_bDriverTaken)
			break;
		switch (snd_erreur_init(0x1F))
		{
		case 2:
			break;
		default:
			SND_M_DisplayError(0x11,"");
		case 3:
			SND_pc_bRetryDriver = FALSE;
			break;
		case 4:
			SND_pc_bRetryDriver = FALSE;
			exit(0);
			break;
		}
		if (SND_pc_bDriverTaken || !SND_pc_bRetryDriver)
			break;
		SND_pc_fn_vTakeWaveOut();
	}

	SND_pc_M_bDeleteEvent(hEvent);
	iResult = pMsg->iResult;
	SND_fn_vFreeSndEx(0,pMsg);
	SND_pc_bInitDone = TRUE;
	return iResult;
}

SndBool SND_fn_bTestInitSample(void)
{
	return SND_pc_bInitDone != FALSE;
}

void SND_fn_vDesInitSample(void)
{
	SND_pc_tdhEvent hEvent;

	if (SND_pc_bInitDone)
	{
		if (SND_pc_M_bThreadEnded(SND_pc_hThread))
		{
			SND_pc_fn_vDesInitWaveOut();
			return;
		}
		hEvent = SND_pc_M_hCreateEvent();
		if (!SND_pc_M_bPostMessage(SND_pc_dwThreadId,SND_pc_uiMsgDesInitSnd,(WPARAM)hEvent,0))
		{
			GetLastError();
			SND_pc_fn_vDesInitWaveOut();
			return;
		}
		SND_pc_M_ulWaitEventOrBreak(hEvent);
		SND_pc_M_bDeleteEvent(hEvent);
		SND_pc_M_ulJoinThreadOrBreak(SND_pc_hThread);
		SND_pc_M_bDeleteThread(SND_pc_hThread);
	}
}

int SND_fn_iInitBnkSample(long num)
{
	return C_INIT_FAILED;
}

SndBool SND_fn_bTestInitBnkSample(long num)
{
	return TRUE;
}

void SND_fn_vDesInitBnkSample(long num)
{
}

void SND_fn_vDesInitAllBnkSample(void)
{
}

void SND_fn_vSetEffectSample(long num)
{
	SND_pc_lEffectState = 0;
	switch (num)
	{
	case 0:
		SND_pc_lEffect_198 = 0;
		break;
	case 1:
		SND_pc_lEffect_198 = 1;
		break;
	case 2:
		SND_pc_lEffect_198 = 2;
		break;
	case 3:
		SND_pc_lEffect_198 = 3;
		break;
	case 4:
		SND_pc_lEffect_198 = 4;
		break;
	case 5:
		SND_pc_lEffect_198 = 5;
		break;
	case 6:
		SND_pc_lEffect_198 = 6;
		break;
	}
}

void SND_fn_vUnLoadResSample(tdstBlockResourceMem* mem)
{
	if (!mem->eStorage && mem->uRes.stSample.uData.stMem.pvPtrFirst)
		SND_fn_vFreeSnd(mem->uRes.stSample.uData.stMem.pvPtrFirst);
	mem->uRes.stSample.uData.stMem.pvPtrFirst = NULL;
	mem->eType = 0;
}

void SND_fn_vConvertResDiskToMemSample(tdstBlockResourceDisk *disk,tdstBlockResourceMem *mem,void* ptrBegin)
{
	mem->Id = disk->Id;
	mem->eType = disk->eType;
	mem->eStorage = disk->eStorage;
	mem->ucVolume = disk->ucVolume;
	mem->bIsLoaded = TRUE;
	mem->uRes.stSample.eZip = SAMPLE_PCM;
	mem->uRes.stSample.bPitchable = disk->uRes.stSample.bPitchable;
	mem->uRes.stSample.bVolable = disk->uRes.stSample.bVolable;
	mem->uRes.stSample.bPanable = disk->uRes.stSample.bPanable;
	mem->uRes.stSample.bSpacable = disk->uRes.stSample.bSpacable;
	mem->uRes.stSample.bReverbable = disk->uRes.stSample.bReverbable;
	mem->uRes.stSample.bStream = disk->uRes.stSample.bStream;
	mem->uRes.stSample.bLoop = disk->uRes.stSample.bLoop;
	mem->uRes.stSample.ulInc0 = (disk->uRes.stSample.ulFreq << 8) / SND_pc_stWaveFormat.nSamplesPerSec;
	mem->uRes.stSample.ulFreq = disk->uRes.stSample.ulFreq;
	mem->uRes.stSample.uwResolution = disk->uRes.stSample.uwResolution;
	mem->uRes.stSample.uwNbChannels = disk->uRes.stSample.uwNbChannels;
	mem->uRes.stSample.uData.stMem.ulNbEchLoop = disk->uRes.stSample.ulStartLoop;
	if (disk->uRes.stSample.bLoop)
		mem->uRes.stSample.uData.stMem.ulNbEch = disk->uRes.stSample.pc_ulLoopLength + disk->uRes.stSample.ulStartLoop;
	else
		mem->uRes.stSample.uData.stMem.ulNbEch = disk->ulDataSize;
	if (mem->uRes.stSample.uData.stMem.ulNbEch > disk->ulDataSize)
		mem->uRes.stSample.uData.stMem.ulNbEch = disk->ulDataSize;
	if (mem->uRes.stSample.uwResolution == 16)
	{
		mem->uRes.stSample.uData.stMem.ulNbEchLoop >>= 1;
		mem->uRes.stSample.uData.stMem.ulNbEch >>= 1;
	}
	else
		SND_M_DisplayError(0x15,"8 bits samples are not allowed");
	if (mem->uRes.stSample.uwNbChannels == 2)
	{
		mem->uRes.stSample.uData.stMem.ulNbEchLoop >>= 1;
		mem->uRes.stSample.uData.stMem.ulNbEch >>= 1;
	}
	if (ptrBegin)
		mem->uRes.stSample.uData.stMem.pvPtrFirst = ptrBegin;
	else
		mem->uRes.stSample.uData.stMem.pvPtrFirst = NULL;
	mem->uRes.stSample.uData.stMem.ulNbEchLoop &= ~1;
	mem->uRes.stSample.uData.stMem.ulNbEch &= ~1;
}

SndBool SND_fn_bLoadResScriptSample(tdstBlockResourceDisk *disk,tdstBlockResourceMem *mem)
{
	char szFileName[256];
	void *ptr;

	ptr = SND_fn_pvMallocSnd(disk->ulDataSize);
	SND_fn_vResolveFileName(disk->uRes.stSample.szFileName,szFileName);
	if (!SND_fn_bLoadDataInMemAndSwap(ptr,disk->ulDataSize,szFileName,disk->ulDataOffset,disk->uRes.stSample.eZip == SAMPLE_AIFF))
	{
		SND_M_DisplayError(0x0B,szFileName);
		if (ptr)
			SND_fn_vFreeSnd(ptr);
		ptr = NULL;
	}
	SND_fn_vConvertResDiskToMemSample(disk,mem,ptr);
	mem->eStorage = 0;
	return TRUE;
}

SndBool SND_fn_bLoadResBinarySample(tdstBlockResourceDisk *disk,tdstBlockResourceMem *mem,char *pDataBloc)
{
	char szFileName[256];
	void *ptr;

	if (disk->eStorage)
		ptr = pDataBloc + disk->ulDataOffset;
	else
	{
		ptr = SND_fn_pvMallocSnd(disk->ulDataSize);
		SND_fn_vResolveFileName(disk->uRes.stSample.szFileName,szFileName);
		if (!SND_fn_bLoadDataInMem(ptr,disk->ulDataSize,szFileName,disk->ulDataOffset))
		{
			SND_M_DisplayError(0x0B,szFileName);
			SND_fn_vFreeSnd(ptr);
			ptr = NULL;
		}
	}
	SND_fn_vConvertResDiskToMemSample(disk,mem,ptr);
	return TRUE;
}

SndBool SND_fn_bIsResLoadedSample(tdstBlockResourceMem *mem)
{
	if (mem->uRes.stSample.bStream)
		return TRUE;
	return mem->uRes.stSample.uData.stMem.pvPtrFirst != NULL;
}

tduRefRes SND_fn_tdGetResForVoiceSample(long voice)
{
	tduRefRes res;

	res.pstPtr = SND_pc_a64_stVoice_9095[voice].pRes;
	return res;
}

void FUN_1000a4d0(void)
{
	SNDLST2_M_DynamicInitAnchor(&SND_pc_stBufferClients.pList);
}

long FUN_1000a4f0(SND_tdstFormat* pformat,SND_tdstCallback* pCallback,SampleParam* par,long user_id)
{
	SND_pc_tdstBufferClient *pBuffer;
	void *pData;

	if (pformat->uFormat.stPCM.uwResolution != 16 || !SND_pc_bDriverTaken)
		return -1;

	pBuffer = (SND_pc_tdstBufferClient *)SND_fn_pvMallocSnd(sizeof(SND_pc_tdstBufferClient));
	SNDLST2_M_DynamicInitElement(pBuffer);
	pBuffer->eType = BUFFER_ASYNCHRONE;
	pBuffer->pRes = &pBuffer->stRes;
	pBuffer->ulNbSamples = pformat->uFormat.stPCM.ulNbSamples;
	pBuffer->lShift = (pformat->uFormat.stPCM.uwResolution != 8) + (pformat->uFormat.stPCM.uwNbChannels != 1);
	pBuffer->uCallback.stAsync.callback = pCallback->uCallback.CallbackASynchrone;
	pBuffer->uCallback.stAsync.user_id = user_id;
	pBuffer->uCallback.stAsync.ulLastPos = 0;
	pBuffer->bPaused = FALSE;
	pBuffer->pRes->Id = -1;
	pBuffer->stRes.eType = 1;
	pBuffer->stRes.eStorage = 0;
	pBuffer->stRes.ucVolume = 0x7F;
	pBuffer->stRes.bIsLoaded = TRUE;
	pBuffer->stRes.uRes.stSample.bPitchable = TRUE;
	pBuffer->stRes.uRes.stSample.bVolable = TRUE;
	pBuffer->stRes.uRes.stSample.bPanable = TRUE;
	pBuffer->stRes.uRes.stSample.bSpacable = TRUE;
	pBuffer->stRes.uRes.stSample.bReverbable = TRUE;
	pBuffer->stRes.uRes.stSample.bStream = FALSE;
	pBuffer->stRes.uRes.stSample.bLoop = TRUE;
	pBuffer->stRes.uRes.stSample.uData.stMem.pvPtrFirst = pData = SND_fn_pvMallocSnd(pBuffer->ulNbSamples << pBuffer->lShift);
	pBuffer->stRes.uRes.stSample.uData.stMem.ulNbEchLoop = 0;
	pBuffer->stRes.uRes.stSample.uData.stMem.ulNbEch = pformat->uFormat.stPCM.ulNbSamples;
	pBuffer->stRes.uRes.stSample.ulInc0 = (pformat->uFormat.stPCM.ulFreq << 8) / SND_pc_stWaveFormat.nSamplesPerSec;
	pBuffer->stRes.uRes.stSample.ulFreq = pformat->uFormat.stPCM.ulFreq;
	pBuffer->stRes.uRes.stSample.uwResolution = pformat->uFormat.stPCM.uwResolution;
	pBuffer->stRes.uRes.stSample.uwNbChannels = pformat->uFormat.stPCM.uwNbChannels;
	memset(pData,0,pBuffer->ulNbSamples << pBuffer->lShift);
	pBuffer->uCallback.stAsync.callback(user_id,0,pformat->uFormat.stPCM.ulNbSamples,pBuffer->stRes.uRes.stSample.uData.stMem.pvPtrFirst);
	pBuffer->voice = FUN_10009a50(pBuffer->pRes,par,0,NULL,0,TRUE);

	SND_pc_M_ulWaitLock(SND_pc_hSampleMutex,SND_pc_C_ulWaitForever);
	SND_pc_lLastLockedOperation = 11;
	SNDLST2_M_DynamicIsolate(pBuffer);
	SNDLST2_M_DynamicAddTail(&SND_pc_stBufferClients.pList,pBuffer);
	SND_pc_M_bReleaseLock(SND_pc_hSampleMutex);

	return (long)pBuffer;
}

long FUN_1000a720(SND_tdstFormat* pformat,SND_tdstCallback* pCallback,SampleParam* par,long user_id)
{
	SND_pc_tdstBufferClient *pBuffer;
	void *pData;

	if (pformat->uFormat.stPCM.uwResolution != 16 || !SND_pc_bDriverTaken)
		return -1;

	pBuffer = (SND_pc_tdstBufferClient *)SND_fn_pvMallocSnd(sizeof(SND_pc_tdstBufferClient));
	SNDLST2_M_DynamicInitElement(pBuffer);
	pBuffer->eType = BUFFER_SYNCHRONE;
	pBuffer->pRes = &pBuffer->stRes;
	pBuffer->ulNbSamples = (unsigned long)((double)SND_pc_lBufferLength * pformat->uFormat.stPCM.ulFreq / ((double)SND_pc_stWaveFormat.nBlockAlign * SND_pc_stWaveFormat.nSamplesPerSec));
	pBuffer->lShift = (pformat->uFormat.stPCM.uwResolution != 8) + (pformat->uFormat.stPCM.uwNbChannels != 1);
	pBuffer->uCallback.stSync.callback = pCallback->uCallback.CallbackSynchrone;
	pBuffer->uCallback.stSync.user_id = user_id;
	pBuffer->bPaused = FALSE;
	pBuffer->pRes->Id = -1;
	pBuffer->stRes.eType = 1;
	pBuffer->stRes.eStorage = 0;
	pBuffer->stRes.ucVolume = 0x7F;
	pBuffer->stRes.bIsLoaded = TRUE;
	pBuffer->stRes.uRes.stSample.bPitchable = TRUE;
	pBuffer->stRes.uRes.stSample.bVolable = TRUE;
	pBuffer->stRes.uRes.stSample.bPanable = TRUE;
	pBuffer->stRes.uRes.stSample.bSpacable = TRUE;
	pBuffer->stRes.uRes.stSample.bReverbable = TRUE;
	pBuffer->stRes.uRes.stSample.bStream = FALSE;
	pBuffer->stRes.uRes.stSample.bLoop = TRUE;
	pBuffer->stRes.uRes.stSample.uData.stMem.pvPtrFirst = pData = SND_fn_pvMallocSnd(pBuffer->ulNbSamples << pBuffer->lShift);
	pBuffer->stRes.uRes.stSample.uData.stMem.ulNbEchLoop = 0;
	pBuffer->stRes.uRes.stSample.uData.stMem.ulNbEch = pBuffer->ulNbSamples;
	pBuffer->stRes.uRes.stSample.ulInc0 = (pformat->uFormat.stPCM.ulFreq << 8) / SND_pc_stWaveFormat.nSamplesPerSec;
	pBuffer->stRes.uRes.stSample.ulFreq = pformat->uFormat.stPCM.ulFreq;
	pBuffer->stRes.uRes.stSample.uwResolution = pformat->uFormat.stPCM.uwResolution;
	pBuffer->stRes.uRes.stSample.uwNbChannels = pformat->uFormat.stPCM.uwNbChannels;
	memset(pData,0,pBuffer->ulNbSamples << pBuffer->lShift);
	pBuffer->voice = FUN_10009a50(pBuffer->pRes,par,0,NULL,0,TRUE);

	SND_pc_M_ulWaitLock(SND_pc_hSampleMutex,SND_pc_C_ulWaitForever);
	SND_pc_lLastLockedOperation = 12;
	SNDLST2_M_DynamicIsolate(pBuffer);
	SNDLST2_M_DynamicAddTail(&SND_pc_stBufferClients.pList,pBuffer);
	SND_pc_M_bReleaseLock(SND_pc_hSampleMutex);

	return (long)pBuffer;
}

long SND_fn_lCreateNewBufferSample(unsigned long nb_samples,unsigned short uwResolution,unsigned short uwNbChannels,unsigned long ulFreq,SampleParam* par,td_pfn_vRefreshBufferClient callback,long user_id)
{
	SND_tdstFormat stFormat;
	SND_tdstCallback stCallback;

	memset(&stFormat,0,sizeof(stFormat));
	stFormat.uFormat.stPCM.ulNbSamples = nb_samples;
	stFormat.uFormat.stPCM.uwResolution = uwResolution;
	stFormat.uFormat.stPCM.uwNbChannels = uwNbChannels;
	stFormat.uFormat.stPCM.ulFreq = ulFreq;
	memset(&stCallback,0,sizeof(stCallback));
	stCallback.eType = BUFFER_ASYNCHRONE;
	stCallback.uCallback.CallbackASynchrone = callback;
	stFormat.eZip = SAMPLE_PCM;
	return SND_fn_lCreateNewBufferExSample(&stFormat,&stCallback,par,user_id);
}

void SND_fn_vSetParamBufferSample(long id_buffer,SampleParam *par)
{
	if (id_buffer != -1)
	{
		if (par)
			SND_fn_bSetParamSample(((SND_pc_tdstBufferClient *)id_buffer)->voice,par);
		else
			SND_fn_bSetParamSample(((SND_pc_tdstBufferClient *)id_buffer)->voice,&C_SAMPLEPARAM_NEUTRE);
	}
}

void SND_fn_vDeleteBufferSample(long id_buffer)
{
	SND_pc_tdstBufferClient *pBuffer;

	SND_pc_M_ulWaitLock(SND_pc_hSampleMutex,SND_pc_C_ulWaitForever);
	pBuffer = (SND_pc_tdstBufferClient *)id_buffer;
	SND_pc_lLastLockedOperation = 13;
	if (id_buffer != -1)
	{
		FUN_100099f0(pBuffer->voice);
		if (pBuffer->stRes.uRes.stSample.uData.stMem.pvPtrFirst)
			SND_fn_vFreeSnd(pBuffer->stRes.uRes.stSample.uData.stMem.pvPtrFirst);
		SNDLST2_M_DynamicIsolate(pBuffer);
		SND_fn_vFreeSnd(pBuffer);
	}
	SND_pc_M_bReleaseLock(SND_pc_hSampleMutex);
}

void SND_fn_vPauseBufferSample(long id_buffer)
{
	if (id_buffer != -1)
	{
		((SND_pc_tdstBufferClient *)id_buffer)->bPaused = TRUE;
		SND_fn_vPauseSample(((SND_pc_tdstBufferClient *)id_buffer)->voice);
	}
}

void SND_fn_vResumeBufferSample(long id_buffer)
{
	if (id_buffer != -1)
	{
		((SND_pc_tdstBufferClient *)id_buffer)->bPaused = FALSE;
		SND_fn_vResumeSample(((SND_pc_tdstBufferClient *)id_buffer)->voice);
	}
}

void* SND_fn_pvGetPtrBufferSample(long id_buffer)
{
	return NULL;
}

SndReal SND_fn_rGetPosBufferSample(long id_buffer)
{
	if (id_buffer != -1)
	{
		switch (((SND_pc_tdstBufferClient *)id_buffer)->eType)
		{
		case BUFFER_ASYNCHRONE:
			return SND_fn_rGetPosSample(((SND_pc_tdstBufferClient *)id_buffer)->voice);
		case BUFFER_SYNCHRONE:
			return SND_fn_rGetPosSample(((SND_pc_tdstBufferClient *)id_buffer)->voice);
		default:
			return SND_fn_rGetPosSample(((SND_pc_tdstBufferClient *)id_buffer)->voice);
		}
	}
	return -2;
}

long SND_fn_lCreateNewBufferExSample(SND_tdstFormat* pformat,SND_tdstCallback* pCallback,SampleParam* par,long user_id)
{
	switch (pCallback->eType)
	{
	case BUFFER_ASYNCHRONE:
		return FUN_1000a4f0(pformat,pCallback,par,user_id);
	case BUFFER_SYNCHRONE:
		return FUN_1000a720(pformat,pCallback,par,user_id);
	default:
		return -1;
	}
}

long SND_fn_lGetPosBufferExSample(long id_buffer)
{
	return -1;
}

long SND_fn_lPushBufferExSample(long id_buffer,SND_tdstStackBuffer* pStack)
{
	return -1;
}

long SND_fn_lPlayTransitionSample(tduRefRes FirstRes,tduRefRes NextRes,SND_td_pfn_vSoundCallback transition_callback,long param_callback,SampleParam* par)
{
	return C_PLAY_FAILED;
}

SndBool SND_fn_bSetParamTransitionSample(long voice,SampleParam* par)
{
	return FALSE;
}

SndBool SND_fn_bSetNextTransitionSample(long voice,tduRefRes new_res)
{
	return FALSE;
}

SndBool SND_fn_bDoTransitionWithFadeSample(long voice,tduRefRes new_res)
{
	return FALSE;
}

SndBool SND_fn_bDoTransitionWithFadeSample2( long voice, tduRefRes new_res, SndReal rDuration )
{
	return FALSE;
}

void SND_fn_vStopTransitionSample(long voice)
{
}

void SND_fn_vPauseTransitionSample(long voice)
{
}

void SND_fn_vResumeTransitionSample(long voice)
{
}

__declspec(dllexport) SndBool SND_CALL SND_fn_bCanFreeDataSample(void)
{
	return FALSE;
}

void FUN_1000acb0(void)
{
	int i;

	SND_pc_fn_vTakeWaveOut();

	while (1)
	{
		if (SND_pc_bDriverTaken)
			break;
		if (SND_pc_bRetryDriver)
		{
			switch (snd_erreur_init(0x1F))
			{
			case 2:
				break;
			default:
				SND_M_DisplayError(0x11,"");
			case 3:
				SND_pc_bRetryDriver = FALSE;
				break;
			case 4:
				SND_pc_bRetryDriver = FALSE;
				exit(0);
				break;
			}
		}
		if (SND_pc_bDriverTaken || !SND_pc_bRetryDriver)
			break;
		SND_pc_fn_vTakeWaveOut();
	}
	if (!SND_pc_bRetryDriver)
	{
		for (i = 0; i < SND_pc_C_NbVoices; i++)
			SND_fn_vStopSample(i);
	}
}

void SND_fn_vReleaseDriverSample(void)
{
	if (!SND_pc_bReleased)
	{
		SND_pc_bReleased = TRUE;
		SND_pc_fn_vReleaseWaveOut();
	}
}

void SND_fn_vForceReleaseDriverSample(void)
{
	if (!SND_pc_bForceReleased)
	{
		SND_pc_bForceReleased = TRUE;
		SND_pc_fn_vReleaseWaveOut();
	}
}

void SND_fn_vRestoreDriverSample(void)
{
	if (SND_pc_bReleased)
	{
		SND_pc_bReleased = FALSE;
		if (!SND_pc_bForceReleased)
			FUN_1000acb0();
	}
}

void SND_fn_vForceRestoreDriverSample(void)
{
	if (SND_pc_bForceReleased)
	{
		SND_pc_bForceReleased = FALSE;
		if (!SND_pc_bReleased)
			FUN_1000acb0();
	}
}

SndReal SND_fn_rGetPosSample(long voice)
{
	SND_pc_tdstSampleVoice *pVoice;

	pVoice = &SND_pc_a64_stVoice_9095[voice];
	if (pVoice->bPlaying)
	{
		if ((pVoice->ulPos >> 8) > (unsigned long)SND_pc_lLatency)
			return SND_fn_rDivRealRealSnd((pVoice->ulPos >> 8) - SND_pc_lLatency,pVoice->pRes->uRes.stSample.ulFreq);
		return 0;
	}
	return -2;
}

SndReal SND_fn_rGetLengthSample(long voice)
{
	SND_pc_tdstSampleVoice *pVoice;

	pVoice = &SND_pc_a64_stVoice_9095[voice];
	if (pVoice->bPlaying)
		return SND_fn_rDivRealRealSnd(pVoice->pRes->uRes.stSample.uData.stMem.ulNbEch,pVoice->pRes->uRes.stSample.ulFreq);
	return -1;
}

void SND_fn_vSetSoundVolumeSample(unsigned char vol)
{
	SND_pc_fn_vSetMixerVolume(vol * 100 / 127);
}

unsigned char SND_fn_ucGetSoundVolumeSample(void)
{
	return SND_pc_fn_ulGetMixerVolume() * 127 / 100;
}

void SND_fn_vSynchroSample()
{
}

void SND_fn_vSetNbVoiceWishedSample(long nb)
{
}

long SND_fn_lGetNbVoiceOptimumSample()
{
	return 0x7FFFFFFF;
}

long SND_fn_lGetNbVoiceWishedSample()
{
	return 0x7FFFFFFF;
}

SndBool SND_fn_bIsResourceLoopingSample(tduRefRes res)
{
	return res.pstPtr->uRes.stSample.bLoop;
}

SndBool FUN_1000aef0(tduRefRes res,unsigned char ucVolume)
{
	res.pstPtr->ucVolume = ucVolume;
	return TRUE;
}

unsigned char	SND_pc_a2B160_2222[0x6C40];
unsigned char	SND_pc_a31DA0_74744[0x3FE0];
unsigned char	SND_pc_a35D80_779[0x10];
unsigned char	SND_pc_a35D90_618[8];
unsigned char	SND_pc_a35DA0_5989[0x3FF0];
unsigned char	SND_pc_a39DA0_785[0x4000];
unsigned char	SND_pc_a3DE00_8880[0x4000];
unsigned char	SND_pc_a41E20_1278[0x3FE0];
unsigned char	SND_pc_a45E18[8];
unsigned char	SND_pc_a45E20[0x3FF0];
unsigned char	SND_pc_a49E20[0x4000];
unsigned char	SND_pc_a4E080[0x8000];
unsigned char	SND_pc_a560A0_9930[0x2AF80];
unsigned char	SND_pc_a81020_10[0x10];

unsigned char	SND_pc_a28480_99[0x10C0];
unsigned char	SND_pc_a3DDC0_9864[0x1C];
unsigned char	SND_pc_a45E10_849[8];
unsigned char	SND_pc_a4DE40_7192[0x200];
unsigned char	SND_pc_a4E060[0x20];
unsigned char	SND_pc_a81038_0[8];

#endif
