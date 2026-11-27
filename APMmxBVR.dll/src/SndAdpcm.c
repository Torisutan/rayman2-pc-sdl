

















#include "SNDinc.h"

#if defined(_DLL_COMPILATION_MODE)
#include "snddll.h"
#endif

#include "sndres.h"
#include "sndhard.h"
#include "sndadpcm_pc.h"
#include <stdio.h>
#include <string.h>
#include <math.h>

#line 25 "SndAdpcm.c"


static SND_pc_tdstCriticalSection	SND_pc_s_stCriticalSectionADPCM;						/* 0x100163D0 */
static char						SND_pc_s_a_cStreamBuffer[SND_pc_C_lStreamBufferSize];	/* 0x100163E8 */
static SND_pc_tdstVoiceADPCM	*SND_pc_s_a_pstHandle[SND_pc_C_lNbVoicesADPCM];		/* 0x10024E48 */
static unsigned long			SND_pc_s_ulNbThemeVoices;								/* 0x10024EC0 */
static unsigned long			SND_pc_s_ulNbVoices;									/* 0x10024EC4 */
static long						SND_pc_s_lBufferStream;									/* 0x10024EC8 */
static unsigned long			SND_pc_s_ulNbSamplesStream;								/* 0x10024ECC */
static unsigned long			SND_pc_s_ulTimeStream;									/* 0x10024ED0 */
static long						SND_pc_s_a_lMixBuffer[2 * SND_pc_C_lMixBufferSamples];	/* 0x10024ED8 */
static char						SND_pc_s_szError[256];									/* 0x1002AC98 */
static SndBool					SND_pc_s_bInCallbackStream;								/* 0x1002AD98 */
static long						SND_pc_s_a_lDecodeBuffer[2 * SND_pc_C_lMixBufferSamples * 32];	/* 0x1002ADA0 */
static SND_pc_tdstVoiceADPCM	SND_pc_s_stFadeVoice;									/* 0x100E65A0 */
static SND_pc_tdstHeaderADPCM	SND_pc_s_stStreamHeader;								/* 0x100E6688 */
static SndBool					SND_pc_s_bInMix;										/* 0x100E66EC */
static SND_pc_tdstVoiceADPCM	SND_pc_s_a_stThemeVoice[SND_pc_C_lNbThemeVoicesADPCM];	/* 0x100E66F0 */
static SND_pc_tdstVoiceADPCM	SND_pc_s_a_stVoice[SND_pc_C_lNbVoicesADPCM];			/* 0x100E67D8 */
static SndBool					SND_pc_s_bPausedADPCM;									/* 0x100E8290 */
static long						SND_pc_s_lBufferMemory;									/* 0x100E8298 */
static unsigned long			SND_pc_s_ulNbSamplesMemory;								/* 0x100E829C */
static unsigned long			SND_pc_s_ulTimeMemory;									/* 0x100E82A0 */
static SndBool					SND_pc_s_bReverseStereoADPCM;							/* 0x100E82A4 */
static SndBool					SND_pc_s_bInitADPCM;									/* 0x100E82A8 */
static SndBool					SND_pc_s_bInCallbackMemory;								/* 0x100E82AC */

/* 0x10011030 */
long SND_pc_g_lSizeOfVoicesADPCM = sizeof(SND_pc_s_a_stVoice);

void SND_pc_fn_vInitializeCriticalSectionADPCM(SND_pc_tdstCriticalSection *p_stCriticalSection)
{
	InitializeCriticalSection(p_stCriticalSection);
}

void SND_pc_fn_vDeleteCriticalSectionADPCM(SND_pc_tdstCriticalSection *p_stCriticalSection)
{
	DeleteCriticalSection(p_stCriticalSection);
}

void SND_pc_fn_vEnterCriticalSectionADPCM(SND_pc_tdstCriticalSection *p_stCriticalSection)
{
	EnterCriticalSection(p_stCriticalSection);
}

void SND_pc_fn_vLeaveCriticalSectionADPCM(SND_pc_tdstCriticalSection *p_stCriticalSection)
{
	LeaveCriticalSection(p_stCriticalSection);
}


long SND_pc_fn_lOpenStreamADPCM(char *szFileName,SND_pc_tdstVoiceADPCM *p_stVoice,SND_pc_tdstHeaderADPCM **pp_stHeader)
{
	char	szPath[256];
	long	lErr;

	SND_fn_vResolveFileName(szFileName,szPath);
	p_stVoice->hFile = SND_fn_hOpenFileReadSnd(szPath);
	if (p_stVoice->hFile == INVALID_HANDLE_VALUE)
	{
		SND_fn_vDisplayError(E_uwSndCannotOpenFile,szPath);
		lErr = 0xD;
	}
	else
	{
		SND_fn_dwSeekFileSnd(p_stVoice->hFile,p_stVoice->p_stRes ? p_stVoice->p_stRes->uRes.stSample.uData.stStream.ulOffsetFirst : 0,0);
		SND_fn_dwReadFileSnd(p_stVoice->hFile,sizeof(SND_pc_tdstHeaderADPCM),&SND_pc_s_stStreamHeader);
		p_stVoice->stLoopState = SND_pc_s_stStreamHeader.stLoopState;
		lErr = FUN_10001bb0(SND_pc_s_stStreamHeader.ulSignature);
		if (lErr == 0)
		{
			SND_fn_dwReadFileSnd(p_stVoice->hFile,8,SND_pc_s_a_cStreamBuffer);
			SND_fn_dwSeekFileSnd(p_stVoice->hFile,-8,1);
			p_stVoice->p_cData = SND_pc_s_a_cStreamBuffer;
			p_stVoice->p_cCurrent = SND_pc_s_a_cStreamBuffer;
			lErr = 0;
			*pp_stHeader = &SND_pc_s_stStreamHeader;
		}
	}
	return lErr;
}


long SND_pc_fn_lCloseStreamADPCM(SND_pc_tdstVoiceADPCM *p_stVoice)
{
	if (p_stVoice->hFile != INVALID_HANDLE_VALUE)
	{
		SND_fn_vCloseFileSnd(p_stVoice->hFile);
		p_stVoice->hFile = INVALID_HANDLE_VALUE;
		return 0;
	}
	return 1;
}

long FUN_10001150(unsigned long ulNbSamples,SND_pc_tdstVoiceADPCM *p_stVoice)
{
	unsigned long	ulNbBytes;
	unsigned long	ulLoop;

	if (p_stVoice->ulPos & 1)
		ulNbSamples--;
	if (p_stVoice->wNbChannels == 1)
	{
		ulNbBytes = (ulNbSamples & 1) + (ulNbSamples >> 1);
		ulLoop = (p_stVoice->ulLoopStart & 1) + (p_stVoice->ulLoopStart >> 1);
	}
	else
	{
		ulNbBytes = (ulNbSamples & 1) + ulNbSamples;
		ulLoop = p_stVoice->ulLoopStart;
	}
	if (p_stVoice->p_stRes)
		ulLoop += p_stVoice->p_stRes->uRes.stSample.uData.stStream.ulOffsetFirst;
	if ((unsigned long)(p_stVoice->p_cCurrent - p_stVoice->p_cData) == ulLoop)
	{
		SND_fn_dwSeekFileSnd(p_stVoice->hFile,ulLoop + sizeof(SND_pc_tdstHeaderADPCM),0);
		p_stVoice->p_cData = SND_pc_s_a_cStreamBuffer + (p_stVoice->p_cData - p_stVoice->p_cCurrent);
		p_stVoice->p_cCurrent = SND_pc_s_a_cStreamBuffer;
		p_stVoice->p_cEnd = p_stVoice->p_cData + (p_stVoice->ulNbSamples >> (2 - p_stVoice->wNbChannels));
	}
	if (ulNbSamples)
	{
		SND_fn_dwReadFileSnd(p_stVoice->hFile,ulNbBytes,SND_pc_s_a_cStreamBuffer);
		p_stVoice->p_cData = SND_pc_s_a_cStreamBuffer + (p_stVoice->p_cData - p_stVoice->p_cCurrent);
		p_stVoice->p_cEnd = SND_pc_s_a_cStreamBuffer + (p_stVoice->p_cEnd - p_stVoice->p_cCurrent);
		p_stVoice->p_cCurrent = SND_pc_s_a_cStreamBuffer;
	}
	return 0;
}


void SND_pc_fn_vSetVolumePanADPCM(SND_pc_tdstVoiceADPCM *p_stVoice,long lVolume,long lPan)
{
	p_stVoice->lVolume = lVolume;
	p_stVoice->lPan = lPan;
	p_stVoice->lGainRight = lPan * (p_stVoice->lStaticVolume * p_stVoice->lVolume) >> 7;
	p_stVoice->lGainLeft = (127 - lPan) * (p_stVoice->lStaticVolume * p_stVoice->lVolume) >> 7;
}

SndBool SND_pc_fn_bIsReverseStereoADPCM(void)
{
	return SND_pc_s_bReverseStereoADPCM;
}


void SND_pc_fn_vApplyReverseStereoADPCM(SndBool bActive)
{
	SND_pc_tdstVoiceADPCM	*p_stVoice;
	int						i;

	if ((bActive && !SND_pc_s_bReverseStereoADPCM) || (!bActive && SND_pc_s_bReverseStereoADPCM))
	{
		for (i = 0, p_stVoice = SND_pc_s_a_stVoice; i < SND_pc_C_lNbVoicesADPCM; i++, p_stVoice++)
			SND_pc_fn_vSetVolumePanADPCM(p_stVoice,p_stVoice->lVolume,127 - p_stVoice->lPan);
		for (i = 0, p_stVoice = SND_pc_s_a_stThemeVoice; i < SND_pc_C_lNbThemeVoicesADPCM; i++, p_stVoice++)
			SND_pc_fn_vSetVolumePanADPCM(p_stVoice,p_stVoice->lVolume,127 - p_stVoice->lPan);
	}
	SND_pc_s_bReverseStereoADPCM = bActive;
}


void FUN_100012f0(SND_pc_tdstVoiceADPCM *p_stVoice,SndReal rDuration)
{
	double	dNbSamples;

	dNbSamples = (rDuration * (1.0 / 65536.0)) * 22050.0;
	p_stVoice->ulFadeLength = (unsigned long)floor(dNbSamples - -0.5);
	p_stVoice->ulFadeStep = (unsigned long)floor(4294967296.0 / dNbSamples);
}

long FUN_10001360(void)
{
	memset(SND_pc_s_a_stVoice,0,sizeof(SND_pc_s_a_stVoice));
	memset(SND_pc_s_a_stThemeVoice,0,sizeof(SND_pc_s_a_stThemeVoice));
	memset(SND_pc_s_a_pstHandle,0,sizeof(SND_pc_s_a_pstHandle));
	SND_pc_s_ulNbVoices = 0;
	SND_pc_s_ulNbThemeVoices = 0;
	return 0;
}

long FUN_100013a0(tdstBlockResourceMem *p_stRes,SampleParam *par,SND_td_pfn_vSoundCallback fn_callback,long par_callback,tdstBlockResourceMem *p_stNextRes,SND_pc_tdstVoiceADPCM ***ppp_stHandle,unsigned long ulTime)
{
	SND_pc_tdstVoiceADPCM	**pp_stHandle;
	SND_pc_tdstVoiceADPCM	*p_stVoice;
	SND_pc_tdstVoiceADPCM	*p_stLast;
	SndBool					bFound;
	SndBool					bReuse;
	unsigned long			i;
	SND_pc_tdstVoiceADPCM	*p_stNextVoice;
	SND_pc_tdstHeaderADPCM	*p_stHeader;
	long					lErr;

	if (SND_pc_s_ulNbVoices == SND_pc_C_lNbVoicesADPCM)
		return 3;
	if (p_stRes == NULL)
		return 1;
	if (p_stNextRes && SND_pc_s_ulNbThemeVoices == SND_pc_C_lNbThemeVoicesADPCM)
		return 9;
	if (p_stRes->eType != TYPE_SAMPLE || p_stRes->uRes.stSample.uwResolution != 16
		|| (p_stRes->uRes.stSample.eZip != SAMPLE_ADPCM && p_stRes->uRes.stSample.eZip != SAMPLE_ADPCM_TO_CONVERT))
		return 0x10;
	if (!p_stRes->uRes.stSample.bStream)
	{
		p_stHeader = (SND_pc_tdstHeaderADPCM *)p_stRes->uRes.stSample.uData.stMem.pvPtrFirst;
		lErr = FUN_10001bb0(p_stHeader->ulSignature);
		if (lErr)
			return lErr;
	}
	bFound = FALSE;
	bReuse = FALSE;
	p_stLast = &SND_pc_s_a_stVoice[SND_pc_s_ulNbVoices];
	for (p_stVoice = SND_pc_s_a_stVoice; !bFound && p_stVoice < p_stLast; p_stVoice++)
	{
		if (!p_stVoice->bActive)
		{
			if (p_stNextRes && p_stVoice->bTheme)
				bReuse = bFound = TRUE;
			if (!p_stNextRes && !p_stVoice->bTheme)
				bReuse = bFound = TRUE;
		}
		if (bFound)
			break;
	}
	if (bFound)
		pp_stHandle = p_stVoice->pp_stHandle;
	else
	{
		pp_stHandle = SND_pc_s_a_pstHandle;
		while (*pp_stHandle)
			pp_stHandle++;
		p_stVoice = p_stLast;
	}
	*pp_stHandle = p_stVoice;
	i = 0;
	do
	{
		memset(p_stVoice,0,sizeof(SND_pc_tdstVoiceADPCM));
		if (p_stNextRes)
			p_stVoice->bTheme = TRUE;
		lErr = FUN_10001fc0(p_stVoice,pp_stHandle,p_stRes,fn_callback,par_callback);
		if (lErr)
			goto fail;
		if (i == 0)
		{
			lErr = FUN_10001cb0(p_stVoice,p_stRes,&p_stHeader);
			if (lErr)
				goto fail;
			p_stVoice->ulTime = ulTime;
			FUN_10001dc0(p_stVoice,par);
		}
		p_stVoice->ulTime = ulTime;
		if (p_stNextRes)
		{
			i++;
			if (i < 2)
			{
				lErr = FUN_10002040(&p_stNextVoice);
				if (lErr)
					goto fail;
				p_stVoice->p_stNext = p_stNextVoice;
				p_stHeader = (SND_pc_tdstHeaderADPCM *)p_stNextRes->uRes.stSample.uData.stMem.pvPtrFirst;
				p_stRes = p_stNextRes;
				p_stVoice = p_stNextVoice;
			}
		}
	} while (p_stNextRes && i < 2);
	*ppp_stHandle = pp_stHandle;
	if (!bReuse)
		SND_pc_s_ulNbVoices++;
	return 0;

fail:
	*pp_stHandle = NULL;
	return lErr;
}


long SND_pc_fn_lStopVoiceADPCM(SND_pc_tdstVoiceADPCM **pp_stHandle)
{
	SND_pc_tdstVoiceADPCM	*p_stVoice;
	SND_pc_tdstVoiceADPCM	**pp_stSlot;

	if (SND_pc_s_ulNbVoices == 0)
		return 1;
	p_stVoice = *pp_stHandle;
	if (!FUN_10002480(p_stVoice,pp_stHandle))
		return 0xE;
	if ((char *)p_stVoice > (char *)SND_pc_s_a_stVoice + SND_pc_g_lSizeOfVoicesADPCM && p_stVoice < SND_pc_s_a_stVoice)
		return 1;
	if (p_stVoice->bStream)
	{
		SND_pc_fn_lCloseStreamADPCM(p_stVoice);
		if (p_stVoice->p_stNext)
			SND_pc_fn_lCloseStreamADPCM(p_stVoice->p_stNext);
	}
	if (p_stVoice->fn_callback && !p_stVoice->bCallbackDone)
		p_stVoice->fn_callback(p_stVoice->par_callback);
	*pp_stHandle = NULL;
	pp_stSlot = SND_pc_s_a_pstHandle;
	do
	{
		if (*pp_stSlot >= p_stVoice)
			(*pp_stSlot)--;
		pp_stSlot++;
	} while (pp_stSlot < &SND_pc_s_a_pstHandle[SND_pc_C_lNbVoicesADPCM]);
	if (p_stVoice == &SND_pc_s_a_stVoice[SND_pc_s_ulNbVoices - 1])
	{
		memset(p_stVoice,0,sizeof(SND_pc_tdstVoiceADPCM));
		SND_pc_s_ulNbVoices--;
		return 0;
	}
	memmove(p_stVoice,p_stVoice + 1,(char *)&SND_pc_s_a_stVoice[SND_pc_s_ulNbVoices - 1] - (char *)p_stVoice);
	p_stVoice = &SND_pc_s_a_stVoice[SND_pc_s_ulNbVoices - 1];
	SND_pc_s_ulNbVoices--;
	memset(p_stVoice,0,sizeof(SND_pc_tdstVoiceADPCM));
	return 0;
}

void FUN_10001700(SND_pc_tdstVoiceADPCM **pp_stHandle)
{
	SND_pc_tdstVoiceADPCM	*p_stVoice;

	if (pp_stHandle)
	{
		p_stVoice = *pp_stHandle;
		if (FUN_10002480(p_stVoice,pp_stHandle))
		{
			if (p_stVoice->p_stNext)
				p_stVoice->p_stNext->p_cData = NULL;
			SND_pc_fn_lStopVoiceADPCM(pp_stHandle);
		}
	}
}



long FUN_10001740(SND_pc_tdstVoiceADPCM *p_stVoice,unsigned long ulTime)
{
	SND_pc_tdstVoiceADPCM	*p_stNext;
	SND_pc_tdstHeaderADPCM	*p_stHeader;
	long					lRet;
	long					lErr;

	lRet = 1;
	if (p_stVoice->bLoop)
	{
		if (!p_stVoice->bTheme)
		{
			if (p_stVoice->wNbChannels == 1)
				p_stVoice->p_cCurrent = p_stVoice->p_cData + (p_stVoice->ulLoopStart >> 1);
			else
				p_stVoice->p_cCurrent = p_stVoice->p_cData + p_stVoice->ulLoopStart;
			p_stVoice->ulPos = p_stVoice->ulLoopStart;
			p_stVoice->stCurrentState = p_stVoice->stLoopState;
			lRet = 0;
			goto end;
		}
	}
	else if (!p_stVoice->bTheme)
	{
		p_stVoice->bEnded = TRUE;
		p_stVoice->ulEndTime = (p_stVoice->ulNbSamples >> 1) + p_stVoice->ulTime;
		if (!p_stVoice->bStream)
			p_stVoice->bActive = FALSE;
		goto callback;
	}
	p_stNext = p_stVoice->p_stNext;
	if (p_stNext)
	{
		if (p_stVoice->bFade)
		{
			if (p_stVoice->bFadeDone)
			{
				if (p_stNext != &SND_pc_s_stFadeVoice)
				{
					if (p_stNext->bStream)
						SND_pc_fn_lCloseStreamADPCM(p_stNext);
					p_stNext->p_cData = NULL;
					p_stNext = &SND_pc_s_stFadeVoice;
				}
				lRet = 1;
			}
			p_stNext->bFade = p_stVoice->bFade;
			p_stNext->bFadeDone = p_stVoice->bFadeDone;
			p_stNext->ulFadeLength = p_stVoice->ulFadeLength;
			p_stNext->ulFadeStep = p_stVoice->ulFadeStep;
		}
		if (p_stVoice->bStream)
			SND_pc_fn_lCloseStreamADPCM(p_stVoice);
		p_stHeader = (SND_pc_tdstHeaderADPCM *)p_stNext->p_stRes->uRes.stSample.uData.stMem.pvPtrFirst;
		lErr = FUN_10001cb0(p_stNext,p_stNext->p_stRes,&p_stHeader);
		FUN_10002090(p_stVoice,p_stNext);
		if (lErr == 0)
		{
			lRet = 0;
			FUN_100024e0(p_stNext);
			*p_stVoice = *p_stNext;
			if (p_stNext)
				p_stNext->p_cData = NULL;
		}
	}
callback:
	if (p_stVoice->fn_callback && (!p_stVoice->bFade || p_stVoice->bFadeDone))
	{
		p_stVoice->bCallbackDone = TRUE;
		p_stVoice->fn_callback(p_stVoice->par_callback);
	}
end:
	return lRet;
}


long FUN_100018d0(SndBool bStream,unsigned long ulNbSamples,void *p_vDest,unsigned long ulTime)
{
	SND_pc_tdstVoiceADPCM	*p_stVoice;
	unsigned long			ulNb;
	unsigned long			ulNbLeft;
	unsigned long			ulRest;
	unsigned long			ulNbBytes;
	unsigned long			ulChunk;
	unsigned long			ulStep;
	unsigned long			ulVoice;
	long					lNbMixed;
	SndBool					bEnd;
	long					*p_lDecoded;
	long					*p_lMixEnd;
	long					*p_lEnd;
	void					*p_vOut;

	if (SND_pc_s_ulNbVoices == 0)
		return 3;


	lNbMixed = 0;
	ulNbLeft = ulNbSamples;
	p_vOut = p_vDest;
	while (ulNbLeft > 0)
	{
		if (ulNbLeft < SND_pc_C_lMixBufferSamples)
			ulNb = ulNbLeft;
		else
			ulNb = SND_pc_C_lMixBufferSamples;
		memset(SND_pc_s_a_lMixBuffer,0,ulNb << 3);
		ulNbLeft -= ulNb;
		p_lMixEnd = &SND_pc_s_a_lMixBuffer[ulNb * 2];
		for (ulVoice = 0, p_stVoice = SND_pc_s_a_stVoice; ulVoice < SND_pc_s_ulNbVoices; p_stVoice++, ulVoice++)
		{
			if (p_stVoice->bPaused || p_stVoice->bEnded || p_stVoice->bStream != bStream)
				continue;
			lNbMixed++;
			p_lDecoded = SND_pc_s_a_lDecodeBuffer;
			ulNbBytes = ulNb * p_stVoice->ulStep;
			if (ulNbBytes & 0x1FF)
				ulRest = (ulNbBytes >> 9) + 1;
			else
				ulRest = ulNbBytes >> 9;
			do
			{
				if (ulRest + p_stVoice->ulPos > p_stVoice->ulNbSamples)
				{
					ulChunk = p_stVoice->ulNbSamples - p_stVoice->ulPos;
					bEnd = TRUE;
				}
				else
				{
					ulChunk = ulRest;
					bEnd = FALSE;
				}
				if (p_stVoice->bFade)
				{
					if (p_stVoice->bFadeDone)
						p_stVoice->bFade = FALSE;
					else
					{
						ulStep = (ulChunk << 9) / p_stVoice->ulStep;
						if (p_stVoice->ulFadeLength > ulStep)
							p_stVoice->ulFadeLength -= ulStep;
						else
						{
							p_stVoice->ulFadeLength = 0;
							p_stVoice->bFadeDone = TRUE;
							bEnd = TRUE;
						}
					}
				}
				if (p_stVoice->bStream)
					FUN_10001150(ulChunk,p_stVoice);
				p_stVoice->ulPos += ulChunk;
				if (p_stVoice->ulPos > p_stVoice->ulNbSamples && p_stVoice->bLoop)
					p_stVoice->ulPos -= p_stVoice->ulNbSamples;
				if (p_stVoice->wNbChannels == 1)
					FUN_10005a70(ulChunk,&p_stVoice->p_cCurrent,&p_lDecoded,&p_stVoice->stCurrentState);
				else
					FUN_10005c38(ulChunk,&p_stVoice->p_cCurrent,&p_lDecoded,&p_stVoice->stCurrentState);
				if (bEnd)
					bEnd = FUN_10001740(p_stVoice,ulTime);
				ulRest -= ulChunk;
			} while (ulRest && !p_stVoice->bEnded && !bEnd);
			p_lEnd = &SND_pc_s_a_lMixBuffer[(ulNb - (ulRest << 9) / p_stVoice->ulStep) * 2];
			if (p_lEnd > p_lMixEnd)
				p_lEnd = p_lMixEnd;
			FUN_10005960(SND_pc_s_a_lMixBuffer,p_lEnd,SND_pc_s_a_lDecodeBuffer,p_stVoice);
		}
		if (lNbMixed == 0)
			return 0x12;
		FUN_100055c0(SND_pc_s_a_lMixBuffer,p_lMixEnd,&p_vOut);
	}
	return 0;
}


long FUN_10001bb0(unsigned long ulSignature)
{
	char			a4_cSignature[4];
	unsigned long	ulVersion;

	a4_cSignature[0] = (char)ulSignature;
	a4_cSignature[1] = (char)(ulSignature >> 8);
	a4_cSignature[2] = (char)(ulSignature >> 16);
	a4_cSignature[3] = (char)(ulSignature >> 24);
	if (a4_cSignature[0] >= 'A' && a4_cSignature[0] <= 'Z')
		a4_cSignature[0] += 'a' - 'A';
	if (a4_cSignature[1] >= 'A' && a4_cSignature[1] <= 'Z')
		a4_cSignature[1] += 'a' - 'A';
	if (a4_cSignature[0] != 'v' || a4_cSignature[1] != 's')
		return 0xFFFF0006;
	ulVersion = ((((unsigned long)(unsigned char)a4_cSignature[3] << 8 | (unsigned char)a4_cSignature[2]) << 8
		| (unsigned char)a4_cSignature[1]) << 8) | (unsigned char)a4_cSignature[0];
	if (ulVersion == 0x32317376)
		return 0;
	return (ulVersion << 16) | 6;
}

long FUN_10001c30(SND_pc_tdstVoiceADPCM **pp_stHandle,SampleParam *par)
{
	SND_pc_tdstVoiceADPCM	*p_stVoice;
	unsigned long			i;

	if (pp_stHandle)
	{
		p_stVoice = *pp_stHandle;
		if (!FUN_10002480(p_stVoice,pp_stHandle))
			return 0xE;
		if (p_stVoice->bActive)
		{
			i = 0;
			do
			{
				FUN_10001dc0(p_stVoice,par);
				i++;
				if (p_stVoice->p_stNext && i < 2)
					p_stVoice = p_stVoice->p_stNext;
			} while (p_stVoice && i < 2);
			return 0;
		}
		SND_pc_fn_lStopVoiceADPCM(pp_stHandle);
		return 8;
	}
	return 0xB;
}

long FUN_10001cb0(SND_pc_tdstVoiceADPCM *p_stVoice,tdstBlockResourceMem *p_stRes,SND_pc_tdstHeaderADPCM **pp_stHeader)
{
	SND_pc_tdstHeaderADPCM	*p_stHeader;
	unsigned long			ulNbSamples;
	long					lErr;

	if (!p_stVoice->bStream)
	{
		p_stHeader = *pp_stHeader;
		p_stVoice->p_cData = (char *)p_stHeader + sizeof(SND_pc_tdstHeaderADPCM);
		p_stVoice->p_cCurrent = (char *)p_stHeader + sizeof(SND_pc_tdstHeaderADPCM);
	}
	else
	{
		lErr = SND_pc_fn_lOpenStreamADPCM(p_stRes->uRes.stSample.uData.stStream.szFileName,p_stVoice,pp_stHeader);
		p_stHeader = *pp_stHeader;
		if (lErr)
			return lErr;
	}
	if (p_stRes->uRes.stSample.uwNbChannels != p_stHeader->uwNbChannels)
		return 0xC;
	p_stVoice->wNbChannels = p_stHeader->uwNbChannels;
	ulNbSamples = p_stHeader->ulNbSamples;
	if (p_stHeader->ulLoopStart != -1)
	{
		p_stVoice->bLoop = TRUE;
		ulNbSamples = p_stHeader->ulLoopLength + p_stHeader->ulLoopStart;
	}
	else
		p_stVoice->bLoop = FALSE;
	p_stVoice->ulNbSamples = ulNbSamples;
	p_stVoice->uwFreq = (unsigned short)p_stHeader->ulFreq;
	p_stVoice->stLoopState = p_stHeader->stLoopState;
	if (p_stVoice->wNbChannels == 1)
	{
		p_stVoice->p_cEnd = p_stVoice->p_cData + (ulNbSamples >> 1);
		if (p_stVoice->bLoop)
			p_stVoice->ulLoopStart = p_stHeader->ulLoopStart;
		else
			p_stVoice->ulLoopStart = 0;
		p_stVoice->ulLoopLength = p_stHeader->ulLoopLength;
		p_stVoice->stState = p_stHeader->stState;
		p_stVoice->stCurrentState.a7_l[2] = p_stHeader->stState.a7_l[2];
	}
	else
	{
		p_stVoice->p_cEnd = p_stVoice->p_cData + ulNbSamples;
		p_stVoice->ulLoopStart = p_stHeader->ulLoopStart;
		p_stVoice->ulLoopLength = p_stHeader->ulLoopLength;
		p_stVoice->stState = p_stHeader->stState;
	}
	FUN_100024e0(p_stVoice);
	*pp_stHeader = p_stHeader;
	return 0;
}


long FUN_10001dc0(SND_pc_tdstVoiceADPCM *p_stVoice,SampleParam *par)
{
	float	fRatio;
	long	lPan;

	if (p_stVoice->uwFreq == 22050)
		fRatio = 1.0f;
	else
	{
		fRatio = p_stVoice->uwFreq * (1.0f / 22050.0f);
		if (fRatio == 0.0f)
			fRatio = 1.0f;
	}
	if (p_stVoice->bPitchable)
		p_stVoice->ulStep = (unsigned long)((unsigned long)(par->Freq >> 7) * fRatio);
	else
		p_stVoice->ulStep = (unsigned long)(fRatio * 512.0f);
	if (p_stVoice->ulStep > 0x4000)
		p_stVoice->ulStep = 0x4000;
	else if (p_stVoice->ulStep < 0x80)
		p_stVoice->ulStep = 0x80;
	if (p_stVoice->wNbChannels == 2)
	{
		p_stVoice->bPanable = FALSE;
		if (p_stVoice->ulStep > 0x2000)
			p_stVoice->ulStep = 0x2000;
	}
	if (p_stVoice->bVolable)
		p_stVoice->lVolume = par->ucVol;
	else
		p_stVoice->lVolume = 127;
	if (p_stVoice->bPanable)
		lPan = par->ucPan;
	else
		lPan = 64;
	if (SND_pc_s_bReverseStereoADPCM && p_stVoice->wNbChannels == 1)
		lPan = 127 - lPan;
	SND_pc_fn_vSetVolumePanADPCM(p_stVoice,p_stVoice->lVolume,lPan);
	return 0;
}

long SND_pc_fn_lPauseVoiceADPCM(SND_pc_tdstVoiceADPCM **pp_stHandle)
{
	SND_pc_tdstVoiceADPCM	*p_stVoice;

	if (pp_stHandle)
	{
		p_stVoice = *pp_stHandle;
		if (!FUN_10002480(p_stVoice,pp_stHandle))
			return 0xE;
		p_stVoice->bPaused = TRUE;
		return 0;
	}
	return 0xB;
}

long SND_pc_fn_lResumeVoiceADPCM(SND_pc_tdstVoiceADPCM **pp_stHandle)
{
	SND_pc_tdstVoiceADPCM	*p_stVoice;

	if (pp_stHandle)
	{
		p_stVoice = *pp_stHandle;
		if (!FUN_10002480(p_stVoice,pp_stHandle))
			return 0xE;
		p_stVoice->bPaused = FALSE;
		return 0;
	}
	return 0xB;
}

SndBool SND_pc_fn_bIsVoicePlayingADPCM(SND_pc_tdstVoiceADPCM **pp_stHandle)
{
	if (pp_stHandle)
	{
		if ((*pp_stHandle)->bActive)
			return TRUE;
		SND_pc_fn_lStopVoiceADPCM(pp_stHandle);
	}
	return FALSE;
}

long SND_pc_fn_lRemoveCallbackVoiceADPCM(SND_pc_tdstVoiceADPCM **pp_stHandle)
{
	SND_pc_tdstVoiceADPCM	*p_stVoice;

	if (pp_stHandle)
	{
		p_stVoice = *pp_stHandle;
		if (!FUN_10002480(p_stVoice,pp_stHandle))
			return 0xE;
		p_stVoice->fn_callback = NULL;
		p_stVoice->par_callback = 0;
		return 0;
	}
	return 0xB;
}

long FUN_10001fc0(SND_pc_tdstVoiceADPCM *p_stVoice,SND_pc_tdstVoiceADPCM **pp_stHandle,tdstBlockResourceMem *p_stRes,SND_td_pfn_vSoundCallback fn_callback,long par_callback)
{
	p_stVoice->fn_callback = fn_callback;
	p_stVoice->par_callback = par_callback;
	p_stVoice->bCallbackDone = FALSE;
	p_stVoice->bPaused = FALSE;
	p_stVoice->bStream = p_stRes->uRes.stSample.bStream;
	p_stVoice->bLoop = p_stRes->uRes.stSample.bLoop;
	p_stVoice->bPitchable = p_stRes->uRes.stSample.bPitchable;
	p_stVoice->bPanable = p_stRes->uRes.stSample.bPanable;
	p_stVoice->bVolable = p_stRes->uRes.stSample.bVolable;
	p_stVoice->bEnded = FALSE;
	p_stVoice->bActive = TRUE;
	p_stVoice->pp_stHandle = pp_stHandle;
	p_stVoice->hFile = INVALID_HANDLE_VALUE;
	p_stVoice->wNbChannels = p_stRes->uRes.stSample.uwNbChannels;
	p_stVoice->uwFreq = (*pp_stHandle)->uwFreq;
	p_stVoice->p_stRes = p_stRes;
	p_stVoice->lStaticVolume = p_stRes->ucVolume;
	return 0;
}

long FUN_10002040(SND_pc_tdstVoiceADPCM **pp_stVoice)
{
	SND_pc_tdstVoiceADPCM	*p_stVoice;
	SndBool					bFound;

	if (SND_pc_s_ulNbThemeVoices < SND_pc_C_lNbThemeVoicesADPCM)
	{
		bFound = FALSE;
		p_stVoice = SND_pc_s_a_stThemeVoice;
		SND_pc_s_a_stThemeVoice[0].bTheme = TRUE;
		while (!bFound && p_stVoice < &SND_pc_s_a_stThemeVoice[SND_pc_C_lNbThemeVoicesADPCM])
		{
			if (p_stVoice->p_cData == NULL)
				bFound = TRUE;
			else
				p_stVoice++;
		}
		if (bFound)
		{
			*pp_stVoice = p_stVoice;
			return 0;
		}
	}
	return 9;
}


long FUN_10002090(SND_pc_tdstVoiceADPCM *p_stVoice,SND_pc_tdstVoiceADPCM *p_stNext)
{
	if (p_stVoice == NULL || p_stNext == NULL)
		return 1;
	p_stNext->ulTime = p_stVoice->ulTime;
	p_stNext->lVolume = p_stVoice->lVolume;
	p_stNext->lPan = p_stVoice->lPan;
	p_stNext->pp_stHandle = p_stVoice->pp_stHandle;
	p_stNext->bTheme = TRUE;
	if (p_stVoice->uwFreq == p_stNext->uwFreq)
		p_stNext->ulStep = p_stVoice->ulStep;
	else
		p_stNext->ulStep = (unsigned long)((unsigned long)(p_stVoice->ulStep * 22050 / p_stVoice->uwFreq) * (p_stNext->uwFreq * (1.0f / 22050.0f)));
	if (p_stVoice->bFade)
	{
		p_stNext->lGainRight = p_stVoice->lGainRight;
		p_stNext->lGainLeft = p_stVoice->lGainLeft;
		return 0;
	}
	SND_pc_fn_vSetVolumePanADPCM(p_stNext,p_stNext->lVolume,p_stNext->lPan);
	return 0;
}

long FUN_10002180(SND_pc_tdstVoiceADPCM **pp_stHandle,tdstBlockResourceMem *p_stRes)
{
	SND_pc_tdstVoiceADPCM	*p_stVoice;
	SND_pc_tdstVoiceADPCM	*p_stNext;
	long					lErr;

	if (pp_stHandle)
	{
		p_stVoice = *pp_stHandle;
		if (!FUN_10002480(p_stVoice,pp_stHandle))
			return 0xE;
		if (p_stVoice->p_stNext)
			p_stNext = p_stVoice->p_stNext;
		else
		{
			lErr = FUN_10002040(&p_stNext);
			if (lErr)
				return lErr;
			p_stVoice->p_stNext = p_stNext;
		}
		memset(p_stNext,0,sizeof(SND_pc_tdstVoiceADPCM));
		FUN_10001fc0(p_stNext,p_stVoice->pp_stHandle,p_stRes,p_stVoice->fn_callback,p_stVoice->par_callback);
		return 0;
	}
	return 0xB;
}

SndBool FUN_10002210(SND_pc_tdstVoiceADPCM **pp_stHandle,tdstBlockResourceMem *p_stRes,SndReal rDuration)
{
	SND_pc_tdstVoiceADPCM	*p_stVoice;

	if (pp_stHandle)
	{
		p_stVoice = *pp_stHandle;
		if (!FUN_10002480(p_stVoice,pp_stHandle))
			return 0xE;
		p_stVoice->bFade = TRUE;
		FUN_100012f0(p_stVoice,rDuration);
		p_stVoice->bFadeDone = FALSE;
		if (p_stVoice->p_stNext)
			p_stVoice->p_stNext->p_stNext = &SND_pc_s_stFadeVoice;
		else
			p_stVoice->p_stNext = &SND_pc_s_stFadeVoice;
		memset(&SND_pc_s_stFadeVoice,0,sizeof(SND_pc_s_stFadeVoice));
		SND_pc_s_stFadeVoice.bTheme = TRUE;
		FUN_10001fc0(&SND_pc_s_stFadeVoice,p_stVoice->pp_stHandle,p_stRes,p_stVoice->fn_callback,p_stVoice->par_callback);
		return TRUE;
	}
	return FALSE;
}

long FUN_100022c0(SND_pc_tdstVoiceADPCM **pp_stHandle,unsigned long *p_ulLength)
{
	SND_pc_tdstVoiceADPCM	*p_stVoice;

	if (p_ulLength == NULL)
		return 1;
	if (pp_stHandle)
	{
		p_stVoice = *pp_stHandle;
		if (!FUN_10002480(p_stVoice,pp_stHandle))
			return 0xE;
		if (!p_stVoice->bLoop)
			*p_ulLength = p_stVoice->ulNbSamples;
		else
			*p_ulLength = p_stVoice->ulLoopStart + p_stVoice->ulLoopLength;
		return 0;
	}
	return 0xB;
}

long FUN_10002320(SND_pc_tdstVoiceADPCM **pp_stHandle,double *p_dPos,unsigned long ulTime)
{
	SND_pc_tdstVoiceADPCM	*p_stVoice;
	unsigned long			ulPos;
	double					dEnd;
	long					lErr;
	unsigned long			ulStart;
	unsigned long			ulLength;

	lErr = 0xB;
	if (p_dPos == NULL)
		lErr = 1;
	else if (pp_stHandle)
	{
		p_stVoice = *pp_stHandle;
		if (FUN_10002480(p_stVoice,pp_stHandle))
		{
			ulPos = p_stVoice->ulPos;
			if (p_stVoice->bLoop)
			{
				ulStart = p_stVoice->ulLoopStart;
				ulLength = p_stVoice->ulLoopLength;
				if (ulPos > ulStart + ulLength)
					ulPos = (ulPos - ulStart) % ulLength + ulStart;
			}
			else if (ulPos > p_stVoice->ulNbSamples)
				ulPos = p_stVoice->ulNbSamples;
			if (!p_stVoice->bStream)
				*p_dPos = (double)ulPos / (double)p_stVoice->uwFreq - 240.0 / 22050.0;
			else
			{
				if (p_stVoice->bActive)
				{
					if (ulPos > (unsigned long)(p_stVoice->uwFreq * 9 / 10))
						*p_dPos = (double)ulPos / p_stVoice->uwFreq - 0.9;
					else
						*p_dPos = 0;
				}
				if (p_stVoice->bEnded)
				{
					dEnd = (double)(ulTime - p_stVoice->ulEndTime) * (2.5 / 22050.0);
					*p_dPos += dEnd;
					if (dEnd >= 0.9)
						p_stVoice->bActive = FALSE;
				}
			}
			if (ulPos == p_stVoice->ulNbSamples && !p_stVoice->bActive)
				lErr = 0xF;
			else
				lErr = 0;
		}
	}
	return lErr;
}


SndBool FUN_10002480(SND_pc_tdstVoiceADPCM *p_stVoice,SND_pc_tdstVoiceADPCM **pp_stHandle)
{
	SndBool	bValid;

	bValid = FALSE;
	if (p_stVoice == NULL)
		return FALSE;
	if (p_stVoice >= SND_pc_s_a_stVoice && p_stVoice <= &SND_pc_s_a_stVoice[SND_pc_C_lNbVoicesADPCM]
		&& ((char *)p_stVoice - (char *)SND_pc_s_a_stVoice) % sizeof(SND_pc_tdstVoiceADPCM) == 0)
		bValid = TRUE;
	if (pp_stHandle && bValid && pp_stHandle != p_stVoice->pp_stHandle)
		bValid = FALSE;
	return bValid;
}

void FUN_100024e0(SND_pc_tdstVoiceADPCM *p_stVoice)
{

	if (p_stVoice->wNbChannels == 1)
		p_stVoice->stCurrentState = p_stVoice->stState;
	else
		p_stVoice->stCurrentState = p_stVoice->stState;
}


long FUN_10002500(tdstBlockResourceDisk *p_stDisk,SND_pc_tdstHeaderADPCM *p_stHeader)
{
	long	lErr;

	if (p_stDisk == NULL || p_stHeader == NULL)
		lErr = 1;
	else
	{
		lErr = FUN_10001bb0(p_stHeader->ulSignature);
		if (lErr == 0)
		{
			p_stDisk->uRes.stSample.uwNbChannels = p_stHeader->uwNbChannels;
			if (p_stHeader->uwNbChannels == 1)
				p_stDisk->ulDataSize = p_stHeader->ulNbSamples << 1;
			else
				p_stDisk->ulDataSize = p_stHeader->ulNbSamples << 2;
			if (p_stHeader->ulLoopStart != -1)
			{
				p_stDisk->uRes.stSample.bLoop = TRUE;
				p_stDisk->uRes.stSample.ulStartLoop = p_stHeader->ulLoopStart * p_stHeader->uwNbChannels * 2;
				p_stDisk->uRes.stSample.pc_ulLoopLength = p_stHeader->ulLoopLength * p_stHeader->uwNbChannels * 2;
				return 0;
			}
			p_stDisk->uRes.stSample.bLoop = FALSE;
			p_stDisk->uRes.stSample.ulStartLoop = 0;
			return 0;
		}
	}
	return lErr;
}


long FUN_100025a0(SND_pc_tdstHeaderADPCM *p_stHeader,void *p_vDest)
{
	SND_pc_tdstDecoderStateADPCM	stState;
	char							*p_cSrc;
	long							*p_lBuffer;
	long							*p_lEnd;
	long							*p_lOut;
	long							lErr;

	lErr = FUN_10001bb0(p_stHeader->ulSignature);
	if (lErr == 0)
	{
		stState = p_stHeader->stState;
		if (p_stHeader->uwNbChannels == 1)
		{
			p_lBuffer = (long *)SND_fn_pvMallocSnd(p_stHeader->ulNbSamples << 2);
			p_lOut = p_lBuffer;
			if (p_lBuffer)
			{
				p_lEnd = p_lBuffer + p_stHeader->ulNbSamples;
				p_cSrc = (char *)p_stHeader + sizeof(SND_pc_tdstHeaderADPCM);
				FUN_10005a70(p_stHeader->ulNbSamples,&p_cSrc,&p_lOut,&stState);
			}
			else
			{
				p_lEnd = p_lOut;
				lErr = 0x11;
			}
		}
		else
		{
			p_lBuffer = (long *)SND_fn_pvMallocSnd(p_stHeader->ulNbSamples << 3);
			p_lOut = p_lBuffer;
			if (p_lBuffer)
			{
				p_lEnd = p_lBuffer + p_stHeader->ulNbSamples * 2;
				p_cSrc = (char *)p_stHeader + sizeof(SND_pc_tdstHeaderADPCM);
				FUN_10005c38(p_stHeader->ulNbSamples,&p_cSrc,&p_lOut,&stState);
			}
			else
			{
				p_lEnd = p_lOut;
				lErr = 0x11;
			}
		}
		if (lErr != 0x11)
		{
			FUN_100055c0(p_lBuffer,p_lEnd,&p_vDest);
			SND_fn_vFreeSnd(p_lBuffer);
		}
	}
	return lErr;
}


void FUN_10002680(tdstBlockResourceDisk *disk,tdstBlockResourceMem *mem,void *ptrBegin)
{
	mem->Id = disk->Id;
	mem->eType = disk->eType;
	mem->eStorage = disk->eStorage;
	mem->ucVolume = disk->ucVolume;
	mem->bIsLoaded = TRUE;
	mem->uRes.stSample.eZip = SAMPLE_ADPCM;
	mem->uRes.stSample.bPitchable = disk->uRes.stSample.bPitchable;
	mem->uRes.stSample.bVolable = disk->uRes.stSample.bVolable;
	mem->uRes.stSample.bPanable = disk->uRes.stSample.bPanable;
	mem->uRes.stSample.bSpacable = disk->uRes.stSample.bSpacable;
	mem->uRes.stSample.bReverbable = disk->uRes.stSample.bReverbable;
	mem->uRes.stSample.bStream = disk->uRes.stSample.bStream;
	mem->uRes.stSample.bLoop = disk->uRes.stSample.bLoop;
	mem->uRes.stSample.ulFreq = disk->uRes.stSample.ulFreq;
	mem->uRes.stSample.uwResolution = disk->uRes.stSample.uwResolution;
	mem->uRes.stSample.uwNbChannels = disk->uRes.stSample.uwNbChannels;
	if (disk->uRes.stSample.bStream)
	{
		strcpy(mem->uRes.stSample.uData.stStream.szFileName,disk->uRes.stSample.szFileName);
		mem->uRes.stSample.uData.stStream.ulOffsetFirst = disk->ulDataOffset;
		if (!disk->uRes.stSample.bLoop)
		{
			mem->uRes.stSample.uData.stStream.ulOffsetLoop = 0;
			mem->uRes.stSample.uData.stStream.ulOffsetLast = disk->ulDataSize + disk->ulDataOffset;
		}
		else
		{
			mem->uRes.stSample.uData.stStream.ulOffsetLoop = disk->ulDataOffset + disk->uRes.stSample.ulStartLoop;
			mem->uRes.stSample.uData.stStream.ulOffsetLast = disk->uRes.stSample.pc_ulLoopLength + disk->uRes.stSample.ulStartLoop + disk->ulDataOffset;
		}
	}
	else
	{
		mem->uRes.stSample.uData.stMem.ulNbEchLoop = disk->uRes.stSample.ulStartLoop;
		mem->uRes.stSample.uData.stMem.ulNbEch = disk->uRes.stSample.pc_ulLoopLength;
		mem->uRes.stSample.uData.stMem.pvPtrFirst = ptrBegin;
	}
}








static char SND_pc_s_szSourceFileADPCM[] = __FILE__;					/* 0x10011038 */
static char SND_pc_s_szNoActiveVoiceADPCM[] = "No active voice.";		/* 0x10011048 */
static char SND_pc_s_szAddressNonAffectedADPCM[] = "Address non affected.";	/* 0x10011060 */
static char SND_pc_s_szNoThemeADPCM[] = "None possibility to play theme.";	/* 0x10011078 */
static char SND_pc_s_szNotADPCMDataADPCM[] = "Is not ADPCM datas.";	/* 0x10011098 */
static char SND_pc_s_szBadFormatADPCM[] = "Bad format.";				/* 0x100110B0 */

long SND_pc_fn_lPlayADPCM(tduRefRes res,SampleParam *par,long prio,SND_td_pfn_vSoundCallback fn_callback,long par_callback)
{
	SND_pc_tdstVoiceADPCM	**pp_stHandle;
	char					*szMessage;

	if (SND_pc_s_bInitADPCM)
	{
		SND_pc_fn_vEnterCriticalSectionADPCM(&SND_pc_s_stCriticalSectionADPCM);
		switch (FUN_100013a0(res.pstPtr,par,fn_callback,par_callback,NULL,&pp_stHandle,SND_pc_s_ulTimeMemory))
		{
		case 0:
			break;
		case 3:
			szMessage = SND_pc_s_szNoActiveVoiceADPCM;
			pp_stHandle = (SND_pc_tdstVoiceADPCM **)-1;
			break;
		case 1:
			szMessage = SND_pc_s_szAddressNonAffectedADPCM;
			pp_stHandle = (SND_pc_tdstVoiceADPCM **)-1;
			break;
		case 9:
			szMessage = SND_pc_s_szNoThemeADPCM;
			pp_stHandle = (SND_pc_tdstVoiceADPCM **)-1;
			break;
		case 0x10:
			szMessage = SND_pc_s_szNotADPCMDataADPCM;
			pp_stHandle = (SND_pc_tdstVoiceADPCM **)-1;
			break;
		default:
			szMessage = SND_pc_s_szBadFormatADPCM;
			pp_stHandle = (SND_pc_tdstVoiceADPCM **)-1;
			break;
		}
		SND_pc_fn_vLeaveCriticalSectionADPCM(&SND_pc_s_stCriticalSectionADPCM);
		if (pp_stHandle == (SND_pc_tdstVoiceADPCM **)-1)
			SND_fn_vDisplayError(E_uwSndSystemError,szMessage);
	}
	return (long)pp_stHandle;
}

void SND_pc_fn_vRemoveCallbackADPCM(long voice)
{
	if (SND_pc_s_bInitADPCM)
	{
		SND_pc_fn_vEnterCriticalSectionADPCM(&SND_pc_s_stCriticalSectionADPCM);
		SND_pc_fn_lRemoveCallbackVoiceADPCM((SND_pc_tdstVoiceADPCM **)voice);
		SND_pc_fn_vLeaveCriticalSectionADPCM(&SND_pc_s_stCriticalSectionADPCM);
	}
}

SndBool SND_pc_fn_bSetParamADPCM(long voice,SampleParam *par)
{
	SndBool	bRet;

	bRet = FALSE;
	if (SND_pc_s_bInitADPCM)
	{
		SND_pc_fn_vEnterCriticalSectionADPCM(&SND_pc_s_stCriticalSectionADPCM);
		if (FUN_10001c30((SND_pc_tdstVoiceADPCM **)voice,par) == 0)
			bRet = TRUE;
		SND_pc_fn_vLeaveCriticalSectionADPCM(&SND_pc_s_stCriticalSectionADPCM);
	}
	return bRet;
}

SndBool SND_pc_fn_bTestIsPlayingADPCM(long voice)
{
	if (SND_pc_s_bInitADPCM && SND_pc_fn_bIsVoicePlayingADPCM((SND_pc_tdstVoiceADPCM **)voice))
		return TRUE;
	return FALSE;
}

void SND_pc_fn_vStopADPCM(long voice)
{
	if (SND_pc_s_bInitADPCM && voice)
	{
		SND_pc_fn_vEnterCriticalSectionADPCM(&SND_pc_s_stCriticalSectionADPCM);
		SND_pc_fn_lStopVoiceADPCM((SND_pc_tdstVoiceADPCM **)voice);
		SND_pc_fn_vLeaveCriticalSectionADPCM(&SND_pc_s_stCriticalSectionADPCM);
	}
}

void SND_pc_fn_vPauseADPCM(long voice)
{
	if (SND_pc_s_bInitADPCM)
	{
		SND_pc_fn_vEnterCriticalSectionADPCM(&SND_pc_s_stCriticalSectionADPCM);
		SND_pc_fn_lPauseVoiceADPCM((SND_pc_tdstVoiceADPCM **)voice);
		SND_pc_fn_vLeaveCriticalSectionADPCM(&SND_pc_s_stCriticalSectionADPCM);
	}
}

void SND_pc_fn_vResumeADPCM(long voice)
{
	if (SND_pc_s_bInitADPCM)
	{
		SND_pc_fn_vEnterCriticalSectionADPCM(&SND_pc_s_stCriticalSectionADPCM);
		SND_pc_fn_lResumeVoiceADPCM((SND_pc_tdstVoiceADPCM **)voice);
		SND_pc_fn_vLeaveCriticalSectionADPCM(&SND_pc_s_stCriticalSectionADPCM);
	}
}

SndBool SND_pc_fn_bIsResourceLoopingADPCM(tduRefRes res)
{
	return res.pstPtr->uRes.stSample.bLoop;
}

SndReal SND_pc_fn_rGetPosADPCM(long voice)
{
	double	dPos;
	long	lErr;
	SndReal	rPos;

	rPos = -1;
	if (SND_pc_s_bInitADPCM)
	{
		lErr = FUN_10002320((SND_pc_tdstVoiceADPCM **)voice,&dPos,SND_pc_s_ulTimeStream);
		if (lErr)
		{
			if (lErr == 0xF)
				rPos = -2;
		}
		else
			rPos = (SndReal)(dPos * 65536.0);
	}
	return rPos;
}

SndReal SND_pc_fn_rGetLengthADPCM(long voice)
{
	SndReal			rLength;
	unsigned long	ulLength;

	rLength = -1;
	if (SND_pc_s_bInitADPCM && FUN_100022c0((SND_pc_tdstVoiceADPCM **)voice,&ulLength) == 0)
		rLength = (SndReal)(((double)ulLength * (1.0 / 22050.0)) * 65536.0);
	return rLength;
}

unsigned char SND_pc_fn_ucGetSoundVolumeADPCM(void)
{
	return 0;
}

void SND_pc_fn_vSetReverseStereoADPCM(SndBool active)
{
	SND_pc_fn_vApplyReverseStereoADPCM(active);
}

SndBool SND_pc_fn_bGetReverseStereoADPCM(void)
{
	return SND_pc_fn_bIsReverseStereoADPCM();
}


int SND_pc_fn_iInitADPCM(SND_tdstInitStruct *pInitStruct)
{
	SND_tdstCallback	stCallback;
	SND_tdstFormat		stFormat;
	int					iRet;

	iRet = 2;
	if (FUN_10001360() == 0)
	{
		stFormat.eZip = SAMPLE_PCM;
		stFormat.uFormat.stPCM.ulNbSamples = 0;
		stFormat.uFormat.stPCM.uwResolution = 16;
		stFormat.uFormat.stPCM.uwNbChannels = 2;
		stFormat.uFormat.stPCM.ulFreq = 22050;
		memset(&stCallback,0,sizeof(stCallback));
		stCallback.eType = BUFFER_SYNCHRONE;
		stCallback.uCallback.CallbackSynchrone = SND_pc_fn_vRefreshBufferMemoryADPCM;
		SND_pc_s_lBufferMemory = SND_fn_lCreateNewBufferExSxd(&stFormat,&stCallback,NULL,0);
		if (SND_pc_s_lBufferMemory != -1)
		{
			stFormat.uFormat.stPCM.ulNbSamples = 17640;
			stCallback.eType = BUFFER_ASYNCHRONE;
			stCallback.uCallback.CallbackASynchrone = SND_pc_fn_vRefreshBufferStreamADPCM;
			stCallback.uInfo.rCallbackPeriod = 0;
			SND_pc_s_lBufferStream = SND_fn_lCreateNewBufferExSxd(&stFormat,&stCallback,NULL,0);
			if (SND_pc_s_lBufferStream != -1)
			{
				SND_pc_fn_vInitializeCriticalSectionADPCM(&SND_pc_s_stCriticalSectionADPCM);
				iRet = 3;
				SND_pc_s_bPausedADPCM = FALSE;
				SND_pc_s_bInitADPCM = TRUE;
			}
			else
				SND_fn_vDeleteBufferSxd(SND_pc_s_lBufferMemory);
		}
	}
	return iRet;
}

/* 0x10002BD0 */
SndBool SND_pc_fn_bGetStereoADPCM(void)
{
	return TRUE;
}

SndBool SND_pc_fn_bTestInitADPCM(void)
{
	return TRUE;
}

void SND_pc_fn_vDesInitADPCM(void)
{
	if (SND_pc_s_bInitADPCM)
	{
		SND_fn_vDeleteBufferSxd(SND_pc_s_lBufferMemory);
		SND_fn_vDeleteBufferSxd(SND_pc_s_lBufferStream);
		SND_pc_fn_vDeleteCriticalSectionADPCM(&SND_pc_s_stCriticalSectionADPCM);
	}
}

/* 0x10002C10 */
long SND_pc_fn_lCreateNewBufferADPCM(unsigned long nb_samples,unsigned short uwResolution,unsigned short uwNbChannels,unsigned long ulFreq,SampleParam* par,td_pfn_vRefreshBufferClient callback,long user_id)
{
	return -1;
}

SndReal SND_pc_fn_rGetPosBufferADPCM(long id_buffer)
{
	return -1;
}

long SND_pc_fn_lCreateNewBufferExADPCM(SND_tdstFormat* pformat,SND_tdstCallback* pCallback,SoundParam* par,long user_id)
{
	return -1;
}

long SND_pc_fn_lGetPosBufferExADPCM(long id_buffer)
{
	return -1;
}

long SND_pc_fn_lPushBufferExADPCM(long id_buffer,SND_tdstStackBuffer* pStack)
{
	return -1;
}

/* 0x10002C20 */
void SND_pc_fn_vSetSoundVolumeADPCM(unsigned char vol)
{
}

void SND_pc_fn_vSetStereoADPCM(SndBool active)
{
}

void SND_pc_fn_vSetEffectADPCM(long num)
{
}

void SND_pc_fn_vSetParamBufferADPCM(long id_buffer,SampleParam *par)
{
}

void SND_pc_fn_vDeleteBufferADPCM(long id_buffer)
{
}

void SND_pc_fn_vReleaseDriverADPCM(void)
{
}

void SND_pc_fn_vRestoreDriverADPCM(void)
{
}

void SND_pc_fn_vGetTargetLabelADPCM(char* name,int nb_char)
{
}

void SND_pc_fn_vSetupTargetADPCM(void)
{
}

void SND_pc_fn_vSynchroADPCM(void)
{
}

void SND_pc_fn_vPauseBufferADPCM(long id_buffer)
{
	if (SND_pc_s_bInitADPCM && !SND_pc_s_bPausedADPCM)
	{
		SND_fn_vPauseBufferSxd(SND_pc_s_lBufferMemory);
		SND_fn_vPauseBufferSxd(SND_pc_s_lBufferStream);
		SND_pc_s_bPausedADPCM = TRUE;
	}
}

void SND_pc_fn_vResumeBufferADPCM(long id_buffer)
{
	if (SND_pc_s_bInitADPCM && SND_pc_s_bPausedADPCM)
	{
		SND_fn_vResumeBufferSxd(SND_pc_s_lBufferMemory);
		SND_fn_vResumeBufferSxd(SND_pc_s_lBufferStream);
		SND_pc_s_bPausedADPCM = FALSE;
	}
}

/* 0x10002CB0 */
void *SND_pc_fn_pvGetPtrBufferADPCM(long id_buffer)
{
	return NULL;
}

SndBool SND_pc_fn_bIsResLoadedADPCM(tdstBlockResourceMem *mem)
{
	return FALSE;
}

SndBool SND_pc_fn_bCanFreeDataADPCM(void)
{
	return FALSE;
}

SndBool SND_pc_fn_bCanSetupTargetADPCM(void)
{
	return FALSE;
}

SndBool SND_pc_fn_bLoadResScriptADPCM(tdstBlockResourceDisk *disk,tdstBlockResourceMem *mem)
{
	char	szPath[256];
	void	*p_vData;
	SndBool	bRet;

	bRet = FALSE;
	if (SND_pc_s_bInitADPCM)
	{
		if (!disk->uRes.stSample.bStream)
		{
			p_vData = SND_fn_pvMallocSnd(disk->ulDataSize);
			if (p_vData)
			{
				SND_fn_vResolveFileName(disk->uRes.stSample.szFileName,szPath);
				if (!SND_fn_bLoadDataInMem(p_vData,disk->ulDataSize,szPath,disk->ulDataOffset))
				{
					sprintf(SND_pc_s_szError,"Src %s\nData %s\nDisk %x",SND_pc_s_szSourceFileADPCM,disk->uRes.stSample.szFileName,disk);
					SND_fn_vDisplayError(E_uwSndErrorReadingFile,SND_pc_s_szError);
					SND_fn_vFreeSnd(p_vData);
					p_vData = NULL;
				}
				else
					bRet = TRUE;
			}
		}
		else
		{
			bRet = TRUE;
			p_vData = NULL;
		}
		if (bRet)
		{
			FUN_10002680(disk,mem,p_vData);
			mem->eStorage = TYPE_EXTERNAL;
		}
	}
	return bRet;
}

SndBool SND_pc_fn_bLoadResBinaryADPCM(tdstBlockResourceDisk *disk,tdstBlockResourceMem *mem,char *pDataBloc)
{
	char	szPath[256];
	void	*p_vData;

	if (!SND_pc_s_bInitADPCM)
		return FALSE;
	if (disk->uRes.stSample.bStream)
		p_vData = NULL;
	else if (disk->eStorage)
		p_vData = disk->ulDataOffset + pDataBloc;
	else
	{
		p_vData = SND_fn_pvMallocSnd(disk->ulDataSize);
		SND_fn_vResolveFileName(disk->uRes.stSample.szFileName,szPath);
		if (!SND_fn_bLoadDataInMem(p_vData,disk->ulDataSize,szPath,disk->ulDataOffset))
		{
			sprintf(SND_pc_s_szError,"Src %s\nData %s\nDisk %x",SND_pc_s_szSourceFileADPCM,disk->uRes.stSample.szFileName,disk);
			SND_fn_vDisplayError(E_uwSndErrorReadingFile,SND_pc_s_szError);
			SND_fn_vFreeSnd(p_vData);
			p_vData = NULL;
		}
	}
	FUN_10002680(disk,mem,p_vData);
	return TRUE;
}

SndBool SND_pc_fn_bLoadResScriptAndConvertToWAVADPCM(tdstBlockResourceDisk *disk,tdstBlockResourceMem *mem)
{
	tdstBlockResourceDisk	stDiskPCM;
	char					szPath[256];
	char					*szFileName;
	void					*p_vData;
	void					*p_vPCM;

	if (!SND_pc_s_bInitADPCM)
		return FALSE;
	if (!disk->uRes.stSample.bStream)
	{
		p_vData = SND_fn_pvMallocSnd(disk->ulDataSize);
		if (p_vData == NULL)
			return FALSE;
		szFileName = disk->uRes.stSample.szFileName;
		SND_fn_vResolveFileName(szFileName,szPath);
		if (!SND_fn_bLoadDataInMem(p_vData,disk->ulDataSize,szPath,disk->ulDataOffset))
		{
			sprintf(SND_pc_s_szError,"Src %s\nData %s\nDisk %x",SND_pc_s_szSourceFileADPCM,szFileName,disk);
			SND_fn_vDisplayError(E_uwSndErrorReadingFile,SND_pc_s_szError);
			SND_fn_vFreeSnd(p_vData);
			return FALSE;
		}
		stDiskPCM = *disk;
		if (!FUN_10003600(disk,p_vData,&stDiskPCM))
		{
			sprintf(SND_pc_s_szError,"Src %s\nData %s\nDisk %x\nConvertion block res failed",SND_pc_s_szSourceFileADPCM,szFileName,disk);
			SND_fn_vDisplayError(E_uwSndErrorReadingFile,SND_pc_s_szError);
			SND_fn_vFreeSnd(p_vData);
			return FALSE;
		}
		p_vPCM = SND_fn_pvMallocSnd(stDiskPCM.ulDataSize);
		if (!FUN_10003650(disk,p_vData,p_vPCM))
		{
			sprintf(SND_pc_s_szError,"Src %s\nData %s\nDisk %x\nConvertion data failed",SND_pc_s_szSourceFileADPCM,szFileName,disk);
			SND_fn_vDisplayError(E_uwSndErrorReadingFile,SND_pc_s_szError);
			SND_fn_vFreeSnd(p_vData);
			SND_fn_vFreeSnd(p_vPCM);
			return FALSE;
		}
		if (!disk->eStorage)
			SND_fn_vFreeSnd(p_vData);
		SND_fn_vConvertResDiskToMemSxd(&stDiskPCM,mem,p_vPCM);
		mem->eStorage = TYPE_EXTERNAL;
		return TRUE;
	}
	return SND_pc_fn_bLoadResScriptADPCM(disk,mem);
}

SndBool SND_pc_fn_bLoadResBinaryAndConvertToWAVADPCM(tdstBlockResourceDisk *disk,tdstBlockResourceMem *mem,char *pDataBloc)
{
	tdstBlockResourceDisk	stDiskPCM;
	char					szPath[256];
	void					*p_vData;
	void					*p_vPCM;

	if (!SND_pc_s_bInitADPCM)
		return FALSE;
	if (!disk->uRes.stSample.bStream)
	{
		if (disk->eStorage)
			p_vData = disk->ulDataOffset + pDataBloc;
		else
		{
			p_vData = SND_fn_pvMallocSnd(disk->ulDataSize);
			SND_fn_vResolveFileName(disk->uRes.stSample.szFileName,szPath);
			if (!SND_fn_bLoadDataInMem(p_vData,disk->ulDataSize,szPath,disk->ulDataOffset))
			{
				sprintf(SND_pc_s_szError,"Src %s\nData %s\nDisk %x\nConvertion data failed",SND_pc_s_szSourceFileADPCM,disk->uRes.stSample.szFileName,disk);
				SND_fn_vDisplayError(E_uwSndErrorReadingFile,SND_pc_s_szError);
				SND_fn_vFreeSnd(p_vData);
				return FALSE;
			}
		}
		stDiskPCM = *disk;
		if (!FUN_10003600(disk,p_vData,&stDiskPCM))
		{
			sprintf(SND_pc_s_szError,"Src %s\nData %s\nDisk %x\nConvertion block failed",SND_pc_s_szSourceFileADPCM,disk->uRes.stSample.szFileName,disk);
			SND_fn_vDisplayError(E_uwSndErrorReadingFile,SND_pc_s_szError);
			SND_fn_vFreeSnd(p_vData);
			return FALSE;
		}
		p_vPCM = SND_fn_pvMallocSnd(stDiskPCM.ulDataSize);
		if (!FUN_10003650(disk,p_vData,p_vPCM))
		{
			SND_fn_vDisplayError(E_uwSndInvalidTypeOfResource,"Cannot convert ADPCM resource");
			SND_fn_vFreeSnd(p_vData);
			SND_fn_vFreeSnd(p_vPCM);
			return FALSE;
		}
		if (!disk->eStorage)
			SND_fn_vFreeSnd(p_vData);
		SND_fn_vConvertResDiskToMemSxd(&stDiskPCM,mem,p_vPCM);
		mem->eStorage = TYPE_EXTERNAL;
		return TRUE;
	}
	return SND_pc_fn_bLoadResBinaryADPCM(disk,mem,pDataBloc);
}

void SND_pc_fn_vUnLoadResADPCM(tdstBlockResourceMem *mem)
{
	if (!mem->uRes.stSample.bStream && mem->uRes.stSample.uData.stMem.pvPtrFirst && !mem->eStorage)
		SND_fn_vFreeSnd(mem->uRes.stSample.uData.stMem.pvPtrFirst);
	mem->uRes.stSample.uData.stMem.pvPtrFirst = NULL;
	mem->eType = 0;
}

long SND_pc_fn_lPlayTransitionADPCM(tduRefRes FirstRes,tduRefRes NextRes,SND_td_pfn_vSoundCallback transition_callback,long param_callback,SampleParam *par)
{
	SND_pc_tdstVoiceADPCM	**pp_stHandle;

	pp_stHandle = (SND_pc_tdstVoiceADPCM **)-1;
	if (SND_pc_s_bInitADPCM)
	{
		SND_pc_fn_vEnterCriticalSectionADPCM(&SND_pc_s_stCriticalSectionADPCM);
		if (FUN_100013a0(FirstRes.pstPtr,par,transition_callback,param_callback,NextRes.pstPtr,&pp_stHandle,SND_pc_s_ulTimeMemory))
			pp_stHandle = (SND_pc_tdstVoiceADPCM **)-1;
		SND_pc_fn_vLeaveCriticalSectionADPCM(&SND_pc_s_stCriticalSectionADPCM);
	}
	return (long)pp_stHandle;
}

SndBool SND_pc_fn_bSetParamTransitionADPCM(long voice,SampleParam *par)
{
	SndBool	bRet;

	bRet = FALSE;
	if (SND_pc_s_bInitADPCM && voice)
	{
		SND_pc_fn_vEnterCriticalSectionADPCM(&SND_pc_s_stCriticalSectionADPCM);
		bRet = FUN_10001c30((SND_pc_tdstVoiceADPCM **)voice,par) == 0;
		SND_pc_fn_vLeaveCriticalSectionADPCM(&SND_pc_s_stCriticalSectionADPCM);
	}
	return bRet;
}

SndBool SND_pc_fn_bSetNextTransitionADPCM(long voice,tduRefRes new_res)
{
	SndBool	bRet;

	bRet = FALSE;
	if (SND_pc_s_bInitADPCM)
	{
		SND_pc_fn_vEnterCriticalSectionADPCM(&SND_pc_s_stCriticalSectionADPCM);
		if (FUN_10002180((SND_pc_tdstVoiceADPCM **)voice,new_res.pstPtr) == 0)
			bRet = TRUE;
		SND_pc_fn_vLeaveCriticalSectionADPCM(&SND_pc_s_stCriticalSectionADPCM);
	}
	return bRet;
}

SndBool SND_pc_fn_bDoTransitionWithFadeADPCM(long voice,tduRefRes new_res,SndReal rDuration)
{
	SndBool	bRet;

	bRet = FALSE;
	if (SND_pc_s_bInitADPCM)
	{
		SND_pc_fn_vEnterCriticalSectionADPCM(&SND_pc_s_stCriticalSectionADPCM);
		bRet = FUN_10002210((SND_pc_tdstVoiceADPCM **)voice,new_res.pstPtr,rDuration);
		SND_pc_fn_vLeaveCriticalSectionADPCM(&SND_pc_s_stCriticalSectionADPCM);
	}
	return bRet;
}

void SND_pc_fn_vStopTransitionADPCM(long voice)
{
	if (SND_pc_s_bInitADPCM)
	{
		SND_pc_fn_vEnterCriticalSectionADPCM(&SND_pc_s_stCriticalSectionADPCM);
		FUN_10001700((SND_pc_tdstVoiceADPCM **)voice);
		SND_pc_fn_vLeaveCriticalSectionADPCM(&SND_pc_s_stCriticalSectionADPCM);
	}
}

void SND_pc_fn_vPauseTransitionADPCM(long voice)
{
	if (SND_pc_s_bInitADPCM)
	{
		SND_pc_fn_vEnterCriticalSectionADPCM(&SND_pc_s_stCriticalSectionADPCM);
		SND_pc_fn_lPauseVoiceADPCM((SND_pc_tdstVoiceADPCM **)voice);
		SND_pc_fn_vLeaveCriticalSectionADPCM(&SND_pc_s_stCriticalSectionADPCM);
	}
}

void SND_pc_fn_vResumeTransitionADPCM(long voice)
{
	if (SND_pc_s_bInitADPCM)
	{
		SND_pc_fn_vEnterCriticalSectionADPCM(&SND_pc_s_stCriticalSectionADPCM);
		SND_pc_fn_lResumeVoiceADPCM((SND_pc_tdstVoiceADPCM **)voice);
		SND_pc_fn_vLeaveCriticalSectionADPCM(&SND_pc_s_stCriticalSectionADPCM);
	}
}


void SND_CALL SND_pc_fn_vRefreshBufferMemoryADPCM(long user_id,unsigned long first,unsigned long nb,void *ptr_first_sample)
{
	if (SND_pc_s_bInitADPCM && !SND_pc_s_bInCallbackMemory)
	{
		long	lErr;

		SND_pc_s_bInCallbackMemory = TRUE;
		SND_pc_fn_vEnterCriticalSectionADPCM(&SND_pc_s_stCriticalSectionADPCM);
		SND_pc_s_bInMix = TRUE;
		lErr = FUN_100018d0(FALSE,nb,ptr_first_sample,SND_pc_s_ulTimeMemory);
		SND_pc_s_ulNbSamplesMemory = nb;
		SND_pc_s_ulTimeMemory += nb;
		if (lErr == 3 || lErr == 0x12)
			memset(ptr_first_sample,0,nb << 2);
		SND_pc_s_bInMix = FALSE;
		SND_pc_fn_vLeaveCriticalSectionADPCM(&SND_pc_s_stCriticalSectionADPCM);
		SND_pc_s_bInCallbackMemory = FALSE;
	}
}


void SND_CALL SND_pc_fn_vRefreshBufferStreamADPCM(long user_id,unsigned long first,unsigned long nb,void *ptr_first_sample)
{
	if (SND_pc_s_bInitADPCM)
	{
		long	lErr;

		SND_pc_s_bInCallbackStream = TRUE;
		SND_pc_fn_vEnterCriticalSectionADPCM(&SND_pc_s_stCriticalSectionADPCM);
		SND_pc_s_bInMix = TRUE;
		lErr = FUN_100018d0(TRUE,nb,ptr_first_sample,SND_pc_s_ulTimeStream);
		SND_pc_s_ulNbSamplesStream = nb;
		SND_pc_s_ulTimeStream += nb;
		if (lErr == 3 || lErr == 0x12)
			memset(ptr_first_sample,0,nb << 2);
		SND_pc_s_bInMix = FALSE;
		SND_pc_fn_vLeaveCriticalSectionADPCM(&SND_pc_s_stCriticalSectionADPCM);
		SND_pc_s_bInCallbackStream = FALSE;
	}
}

SndBool SND_pc_fn_bCheckVersionResourceADPCM(tdstBlockResourceDisk *disk)
{
	char	szPath[256];
	void	*p_vData;
	SndBool	bRet;

	bRet = FALSE;
	p_vData = SND_fn_pvMallocSnd(disk->ulDataSize);
	SND_fn_vResolveFileName(disk->uRes.stSample.szFileName,szPath);
	if (!SND_fn_bLoadDataInMem(p_vData,disk->ulDataSize,szPath,disk->ulDataOffset))
	{
		SND_fn_vDisplayError(E_uwSndErrorReadingFile,SND_pc_s_szSourceFileADPCM);
		SND_fn_vFreeSnd(p_vData);
	}
	else
	{
		if (FUN_10001bb0(((SND_pc_tdstHeaderADPCM *)p_vData)->ulSignature) == 0)
			bRet = TRUE;
		SND_fn_vFreeSnd(p_vData);
	}
	return bRet;
}

long SND_pc_fn_lGetNbVoiceWishedADPCM(void)
{
	return 16;
}


SndBool FUN_10003600(tdstBlockResourceDisk *disk,void *p_vData,tdstBlockResourceDisk *p_stDiskPCM)
{
	SndBool	bRet;

	bRet = FALSE;
	*p_stDiskPCM = *disk;
	p_stDiskPCM->eType = TYPE_SAMPLE;
	p_stDiskPCM->eStorage = 0;
	p_stDiskPCM->ulDataOffset = 0;
	p_stDiskPCM->uRes.stSample.eZip = SAMPLE_PCM;
	p_stDiskPCM->uRes.stSample.szFileName[0] = 0;
	p_stDiskPCM->uRes.stSample.bStream = FALSE;
	if (FUN_10002500(p_stDiskPCM,(SND_pc_tdstHeaderADPCM *)p_vData) == 0)
		bRet = TRUE;
	return bRet;
}

SndBool FUN_10003650(tdstBlockResourceDisk *disk,void *p_vData,void *p_vPCM)
{
	SndBool	bRet;

	bRet = FALSE;
	if (FUN_100025a0((SND_pc_tdstHeaderADPCM *)p_vData,p_vPCM) == 0)
		bRet = TRUE;
	return bRet;
}

SndBool SND_pc_fn_bSetResourceStaticVolumeADPCM(tduRefRes res,unsigned char ucVolume)
{
	int	i;

	res.pstPtr->ucVolume = ucVolume;
	for (i = 0; i < SND_pc_C_lNbVoicesADPCM; i++)
		if (SND_pc_s_a_stVoice[i].p_stRes && SND_pc_s_a_stVoice[i].p_stRes == res.pstPtr)
			SND_pc_s_a_stVoice[i].lStaticVolume = ucVolume;
	for (i = 0; i < SND_pc_C_lNbThemeVoicesADPCM; i++)
		if (SND_pc_s_a_stThemeVoice[i].p_stRes && SND_pc_s_a_stThemeVoice[i].p_stRes == res.pstPtr)
			SND_pc_s_a_stThemeVoice[i].lStaticVolume = ucVolume;
	if (SND_pc_s_stFadeVoice.p_stRes && SND_pc_s_stFadeVoice.p_stRes == res.pstPtr)
		SND_pc_s_stFadeVoice.lStaticVolume = ucVolume;
	return TRUE;
}
