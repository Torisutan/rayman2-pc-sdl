#ifndef _SNDTYPE_H_
#define _SNDTYPE_H_

#if (defined(__cplusplus)&& !defined(CPA_ForceCplusplus))
extern "C" {
#endif

#include "SND\snddef.h"

#ifdef M_SET_ALIGNMENT
#pragma pack(push)
#pragma pack(M_SET_ALIGNMENT)
#endif

#if (defined(U64) || defined(NITRO)) && !defined(ALBankFile)
#include "libaudio.h"

#endif

#include "SND\sndhandl.h"

typedef long SndReal;
#define C_SNDREAL_1 0x10000
#define C_SNDREAL_0 0x00000

typedef long SndRealTwin;

#define M_SndRealTwinToSndReal(rtwin,pr1,pr2) *(pr1)=(((rtwin)>>16)<<8);*(pr2)=(((rtwin)<<16)>>8);

#define M_SndRealToSndRealTwin(r1,r2) ((SndRealTwin)(((r1)<<8)&xFFFF0000)&(((r2>>8)&0xFFFF)))

typedef struct _SND_tdstBlockEvent SND_tdstBlockEvent;
typedef struct _SndVector SndVector;
typedef struct _MicroLink MicroLink;
typedef struct _SoundParam SoundParam;
typedef struct _ExtraSoundParam ExtraSoundParam;
typedef struct _RollOffParam RollOffParam;
typedef struct _SwitchParam SwitchParam;
typedef struct _SampleParam SampleParam;
typedef struct _MicroParam MicroParam;
typedef union _SND_tduRefEvt SND_tduRefEvt;
typedef struct _tdstSndPolyEvent tdstSndPolyEvent;
typedef struct _tdstAllRetObjectSound tdstAllRetObjectSound;
typedef struct _tdstAllRetMicroSound tdstAllRetMicroSound;
typedef union _tduRefRes tduRefRes;

typedef struct _tdstBlockResourceDisk tdstBlockResourceDisk;
typedef struct _tdstBlockResourceMem tdstBlockResourceMem;
typedef struct _tdstSndPolyResource tdstSndPolyResource;
typedef struct _tdstInfoTrackDisk tdstInfoTrackDisk;
typedef struct _tdstInfoTrackMem tdstInfoTrackMem;
typedef struct _tdstTypeSampleDisk tdstTypeSampleDisk;
typedef struct _tdstSampleMemory        tdstSampleMemory;
typedef struct _tdstSampleStreaming     tdstSampleStreaming;
typedef union _tduDataSample tduDataSample;
typedef struct _tdstTypeSampleMem tdstTypeSampleMem;
typedef struct _tdstTypeMidiDisk tdstTypeMidiDisk;
typedef struct _tdstMidiMemory tdstMidiMemory;
typedef struct _tdstMidiStreaming tdstMidiStreaming;
typedef union _tduDataMidi tduDataMidi;
typedef struct _tdstTypeMidiMem tdstTypeMidiMem;
typedef struct _tdstTypeCD tdstTypeCD;
typedef struct _tdstTypeSequenceDisk tdstTypeSequenceDisk;
typedef struct _tdstTypeSequenceMem     tdstTypeSequenceMem;
typedef union _tduResDisk tduResDisk;
typedef union _tduResMem tduResMem;
typedef struct _tdstTypeSwitchDisk   tdstTypeSwitchDisk;
typedef struct _tdstTypeSwitchMem       tdstTypeSwitchMem;
typedef struct _tdstRefResList  tdstRefResList;
typedef struct _tdstRefRes      tdstRefRes;
typedef struct _tdstTypeSplitDisk      tdstTypeSplitDisk;
typedef struct _tdstTypeSplitMem       tdstTypeSplitMem;
typedef struct _tdstRangeSplitDisk  tdstRangeSplitDisk;
typedef struct _tdstRangeSplitListDisk tdstRangeSplitListDisk;
typedef struct _tdstRangeSplitMem  tdstRangeSplitMem;
typedef struct _tdstRangeSplitListMem tdstRangeSplitListMem;
typedef struct _tdstSwitchElement tdstSwitchElement;
typedef struct _tdstSwitchList tdstSwitchList;
typedef union _tduBlockResource tduBlockResource;
typedef struct _tdstThemeBridge tdstThemeBridge;
typedef struct _tdstThemePart tdstThemePart;
typedef struct _tdstThemePartOutro tdstThemePartOutro;
typedef struct _tdstTypeThemeDisk tdstTypeThemeDisk;
typedef struct _tdstTypeThemeMem tdstTypeThemeMem;
typedef struct _tdstRandomElement tdstRandomElement;
typedef struct _tdstTypeRandomDisk       tdstTypeRandomDisk;
typedef struct _tdstTypeRandomMem       tdstTypeRandomMem;

typedef struct _tdstInfoTrackList tdstInfoTrackList;

typedef struct tdstSndBankDescriptorList_ tdstSndBankDescriptorList;
typedef struct _tdstSndBankDescriptor	tdstSndBankDescriptor;
typedef struct _tdstSndBankHeader tdstSndBankHeader;
typedef struct _tdstSndBankHead tdstSndBankHead;

#ifdef FILE_MODE_C_WIN95
	typedef HANDLE tdhSndFile;
#else
	#ifdef	DREAMCAST
		typedef GDFS tdhSndFile;
	#else
		#ifdef FILE_MODE_C_STANDARD
			typedef int tdhSndFile;
		#else
			typedef long tdhSndFile;
		#endif
	#endif
#endif

typedef long tdhSndFileAsync;

union _SND_tduRefEvt{
	tdxId				Id;
	SND_tdstBlockEvent	*pstPtr;
} ;

union _tduRefRes {
        tdxId					Id;
        tdstBlockResourceMem    *pstPtr;
} ;

struct _SndVector {
	SndReal x ;
	SndReal y ;
	SndReal z ;
};

struct _SampleParam{
	SndReal Freq;
	unsigned char ucVol;
	unsigned char ucPan;
	unsigned char ucSpace;
	unsigned char ucReverb;
} ;

extern SampleParam C_SAMPLEPARAM_MUET;
extern SampleParam C_SAMPLEPARAM_NEUTRE;

struct _MicroLink{
    long    lId;
    SndBool bLinked;
};

#ifdef NB_MICRO_MAX
#define NB_LINK NB_MICRO_MAX
#else
#define NB_LINK 8
#endif

#define C_SOUNDPARAM_NO_POSITION 1
#define C_SOUNDPARAM_NO_DOPPLER 2
#define C_SOUNDPARAM_COEF_VOL 4
#define C_SOUNDPARAM_COEF_FREQ 8
#define C_SOUNDPARAM_ROLLOFF 16
#define C_SOUNDPARAM_SWITCH 32

struct _RollOffParam {
	SndReal		rDistSatur;
	SndReal		rDistBackGround;
};

struct _SwitchParam {
	long	lObjectType;
	long	lObjectId;
};

struct _ExtraSoundParam {
	RollOffParam	stRollOff;
	SwitchParam		stSwitch;
};

struct _SoundParam{
	SndReal     Freq;
	unsigned char ucVol;
	unsigned char ucPan;
	unsigned char ucSpace;
	unsigned char ucReverb;
	SndVector   Pos;
	SndVector   Vit;
	int         iFlags ;
	int         iNbLinks;
	MicroLink   astMicro[NB_LINK];
	ExtraSoundParam	xExtra;
} ;

#define C_ROLLOFFPARAM_INFINITE 0x7FFFFFFF
#define C_ROLLOFFPARAM_DEFAULT 0x7FFFFFFE

#define SND_M_GetExtraParamFromSoundParam(pstSoundParam)	(ExtraSoundParam*)((char*)pstSoundParam+sizeof(SoundParam)-sizeof(ExtraSoundParam)+(pstSoundParam->iNbLinks-NB_LINK)*sizeof(MicroLink))

#define SND_M_lGetSizeOfSoundParamFromNbLink(nb) (sizeof(SoundParam)+(nb-NB_LINK)*sizeof(MicroLink))
#define SND_M_lGetSizeOfSoundParam( _pObj) (SND_M_lGetSizeOfSoundParamFromNbLink(_pObj->iNbLinks))

struct _MicroParam{
	SndVector	pos;
	SndVector	vit;
	SndVector	dirnor;
	SndVector	dirtan;
} ;

typedef SndReal (*td_pfn_rRetSoundCoord)(long obj);
typedef unsigned char (*td_pfn_ucRetSoundMidi)(long obj);
typedef void (*td_pfn_vRetSoundExtraCoef)(long obj,SND_tduRefEvt evt,SndReal* pitch,SndReal* pan,SndReal* vol);
typedef long (*td_pfn_lRetSoundLong)(long obj);
typedef SND_tdxHandleToSoundInfo (*td_pfn_lRetSoundSwitch)(long obj,long type_switch);
typedef SndBool (*td_pfn_bRetSoundLink)(long obj,long micro);
typedef void (*td_pfn_vCopyString)(long obj,char* str,long size_str);
typedef SndBool (*td_pfn_bRetRollOffFactor)(long obj,RollOffParam* rolloff);

struct _tdstAllRetObjectSound{
	td_pfn_rRetSoundCoord		X,Y,Z,VX,VY,VZ;
	td_pfn_ucRetSoundMidi		REVERB;
	td_pfn_vRetSoundExtraCoef	ExtraCoef;
	td_pfn_lRetSoundSwitch		Switch;
    td_pfn_bRetSoundLink        MicroLink;
};

struct _tdstAllRetMicroSound{
	td_pfn_rRetSoundCoord		X,Y,Z,VX,VY,VZ;
	td_pfn_rRetSoundCoord		DNX,DNY,DNZ,DTX,DTY,DTZ;
	td_pfn_lRetSoundLong		Filtre;
};

#define C_SOUNDPAN_MEDIAN 64
#define C_SOUNDPAN_LEFT 0
#define C_SOUNDPAN_RIGHT 127

#define C_SOUNDVOL_MINI 0
#define C_SOUNDVOL_MAXI 127

#define C_SOUNDSPACE_FRONT 0
#define C_SOUNDSPACE_BACK 127

#define NO_REVERB 0

#define M_VolLeft(vol,pan)			(((int)(vol)*(pan))>>7)
#define M_VolRight(vol,pan)			(((int)(vol)*(127-(pan)))>>7)
#define M_VolFront(vol,space)		(((int)(vol)*(127-(space)))>>7)
#define M_VolBack(vol,space)		(((int)(vol)*(space))>>7)

#define M_GetVol(voll,volr,volb)	((int)(voll)+(volr)+(volb))
#define M_GetPan(voll,volr,volb)	(((voll)==0) ? 0 : ((int)127*(voll)/((voll)+(volr))))
#define M_GetSpace(voll,volr,volb)	((((voll)+(volr))==0) ? 127 : ((int)127*(volb)/((voll)+(volr))))

#define C_CREATEMICRO_FAILED -1
#define C_PLAY_FAILED -1
#define C_GETVOLUME_FAILED 255
#define C_INIT_FAILED 2
#define C_INIT_OK 3
#define C_INIT_RUNNING 4
#define C_INIT_UNCOMPLETED 5

#define LOW_LATENCY 0x1000
extern SND_tduRefEvt SND_EVT_FANTOME;

typedef enum{
    SAMPLE_INVALID,
    SAMPLE_PCM,
    SAMPLE_MPEG,
	SAMPLE_AIFF,
	SAMPLE_ADPCM,
	SAMPLE_ADPCM_TO_CONVERT,
    SAMPLE_FOR_ALIGNEMENT=SND_ENUM_ALIGNEMENT
} tdeTypeZip;

typedef struct _tdstFormatPCM {
	unsigned long	ulNbSamples;
	unsigned short	uwResolution;
	unsigned short	uwNbChannels;
	unsigned long	ulFreq;
} tdstFormatPCM;
typedef struct _tdstFormatPCM tdstFormatMPEG;
typedef struct _tdstFormatPCM tdstFormatADPCM;

typedef struct _SND_tdstFormat {
	tdeTypeZip	eZip;
	union {
		tdstFormatPCM	stPCM;
		tdstFormatMPEG	stMPEG;
		tdstFormatADPCM	stADPCM;
	} uFormat;
} SND_tdstFormat;

typedef struct _SND_tdstStackBuffer {
	unsigned long	ulNbSamples;
	void			*ptrData;
} SND_tdstStackBuffer;

typedef void (SND_CALL *td_pfn_vBufferClientPoped)(long user_id,long poped_id);
typedef void (SND_CALL *td_pfn_vRefreshBufferClient)(long user_id,unsigned long first,unsigned long nb,void* ptr_first_sample);

typedef enum _SND_tdstTypeCallback {
		BUFFER_ASYNCHRONE,
		BUFFER_SYNCHRONE,
		BUFFER_STACK,
		ALIGN_FOR_BUFFER_CLIENT=0x12345678
} SND_tduTypeCallback;

typedef struct _SND_tdstCallback {
	SND_tduTypeCallback	eType;
	union {
		td_pfn_vRefreshBufferClient CallbackASynchrone;
		td_pfn_vRefreshBufferClient CallbackSynchrone;
		td_pfn_vBufferClientPoped	CallbackStack;
	} uCallback;
	union {
		SndReal	rCallbackPeriod;
		long	lNbSampleToPush;
	} uInfo;
} SND_tdstCallback;

typedef struct _virtual_module *SND_tdhVirtualModule;

typedef const char * (*tdpfnResNameCallback)(unsigned long);

#ifdef M_SET_ALIGNMENT
#pragma pack(pop)
#endif

#if (defined(__cplusplus)&& !defined(CPA_ForceCplusplus))
}
#endif

#endif
