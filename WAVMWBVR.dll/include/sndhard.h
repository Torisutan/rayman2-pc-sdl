#ifndef _SNDHARD_H_
#define _SNDHARD_H_

struct _tdstTypeSampleDisk {
	SndBool			bPitchable;
	SndBool			bVolable;
	SndBool			bPanable;
	SndBool			bSpacable;
	SndBool			bReverbable;
	SndBool			bStream;
	SndBool			bLoop;
	unsigned long	ulStartLoop;
	unsigned long	pc_ulLoopLength;
	unsigned long	ulFreq;
	unsigned short	uwResolution;
	unsigned short	uwNbChannels;
	tdeTypeZip		eZip;
	char			szFileName[13];
};

struct _tdstSampleMemory {
	void			*pvPtrFirst;
	unsigned long	ulNbEchLoop;
	unsigned long	ulNbEch;
};

struct _tdstSampleStreaming {
	char			szFileName[13];
	unsigned long	ulOffsetFirst;
	unsigned long	ulOffsetLoop;
	unsigned long	ulOffsetLast;
};

union _tduDataSample {
	tdstSampleMemory	stMem;
	tdstSampleStreaming	stStream;
};

struct _tdstTypeSampleMem {
	SndBool			bPitchable;
	SndBool			bVolable;
	SndBool			bPanable;
	SndBool			bSpacable;
	SndBool			bReverbable;
	SndBool			bStream;
	SndBool			bLoop;
	tduDataSample	uData;
	unsigned long	ulInc0;
	unsigned long	ulFreq;
	unsigned short	uwResolution;
	unsigned short	uwNbChannels;
	tdeTypeZip		eZip;
};

struct _tdstTypeMidiDisk {
	unsigned long flags;
	unsigned long ulStartLoop;
	unsigned long ulEndLoop;
	unsigned long ulNbLoops;
	unsigned long ulBnkSeq;
};

struct _tdstMidiMemory {
	unsigned int	ulMidiDeviceId;
	unsigned long	ulLength;
	unsigned long	ulNbLoops;
};

struct _tdstMidiStreaming {
	char			szFichier[13];
	unsigned long	ulOffset;
	int				lLoop;
	int				lLast;
};

union _tduDataMidi {
	tdstMidiMemory		stMem;
	tdstMidiStreaming	stStream;
};

struct _tdstTypeMidiMem {
	SndBool			bStream;
	SndBool			bVolable;
	SndBool			bLoop;
	unsigned long	ulNbLoops;
	unsigned long	ulStartLoop;
	unsigned long	ulEndLoop;
	tduDataMidi		uData;
};

struct _tdstTypeCD {
	short			wTrackId;
	SndBool			bVolable;
	SndBool			bLoop;
	int				lNbLoops;
	unsigned long	ulStartLoop;
	unsigned long	ulEndLoop;
	unsigned long	ulStartingFrame;
	unsigned long	ulEndingFrame;
};

struct _tdstSndBankHeader
{
	long lVersion;
};

typedef struct _tdstSndBankPolyRes tdstSndBankPolyRes;
typedef struct _tdstSndBankPolyEvent tdstSndBankPolyEvent;

struct _tdstSndBankPolyRes
{
	SNDLST2_M_StaticParentDeclaration(tduBlockResource*)
};

struct _tdstSndBankPolyEvent
{
	SNDLST2_M_StaticParentDeclaration(SND_tdstBlockEvent*)
};

#define SND_INVALID_BINID 0xFFFFFFFF

struct _tdstSndBankHead
{
	tdstSndBankHeader		stHeader;
	tdstSndBankPolyEvent	stEventList;
	tdstSndBankPolyRes		stResList;
	unsigned long			ulOffsetMPEG;
	unsigned long			ulOffsetMidi;
	unsigned long			ulOffsetSample;
	unsigned long			ulOffsetADPCM;
	unsigned long			ulOffsetStreaming;
	unsigned long			ulTotalSize;
};

#endif
