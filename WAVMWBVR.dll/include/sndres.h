#ifndef _SNDRES_H_
#define _SNDRES_H_

#ifdef SND_SCRIPT_VERSION
#if defined(SNDLST2_StaticIsOptimised) || defined(LST2_StaticIsOptimised)
#error Macros not coherent
#endif
#endif

#ifdef SND_HYBRID_VERSION
#define LST2_StaticIsOptimised
#define SNDLST2_StaticIsOptimised
#endif

#ifdef SND_BINARY_VERSION
#define LST2_StaticIsOptimised
#define SNDLST2_StaticIsOptimised
#endif

#if ((!defined(SND_SCRIPT_VERSION))&&((!defined(SND_HYBRID_VERSION))&&(!defined(SND_BINARY_VERSION))))
#error neither SCRIPT,HYBRID, or BINARY defined
#endif

#include "sndlst2.h"

#include "sndhard.h"

#define SND_C_RES_FANTOME_PTR NULL
#define SND_C_RES_FANTOME_ID -1

extern tduRefRes SND_RES_FANTOME;

typedef enum _tdeTypeResource{
        TYPE_INVALID,
        TYPE_SAMPLE,
        TYPE_MIDI,
        TYPE_CDAUDIO,
        TYPE_SEQUENCE,
        TYPE_SWITCH_OLD,
        TYPE_SPLIT,
        TYPE_THEME_OLD,
        TYPE_SWITCH,
        TYPE_THEME_OLD2,
		TYPE_RANDOM,
        TYPE_THEME,
        TYPE_FOR_ALIGNEMENT=32767
} tdeTypeResource;
#define NB_TYPES_RESOURCES 12

typedef enum{
        TYPE_EXTERNAL,
        TYPE_MEGAFILE,
        TYPE_BIGFILE,
        STORAGE_FOR_ALIGNEMENT=32767
} tdeTypeStorage;

struct _tdstRandomElement {
        tduRefRes                   uRes;
		SndReal						rProb;
		SndBool						bCanBeChosenTwice;
        SNDLST2_M_StaticChildDeclaration(tdstRandomElement*,tdstTypeRandomMem*)
} ;

struct _tdstTypeRandomMem
{
        unsigned long               ulNbElements;
		SndReal						rProbNothing;
		SndBool						bNothingCanBeChosenTwice;
		long						lLastResChoosen;
		SNDLST2_M_StaticParentDeclaration(tdstRandomElement*)
};

struct _tdstTypeRandomDisk
{
        unsigned long               ulNbElements;
		SndReal						rProbNothing;
		SndBool						bNothingCanBeChosenTwice;
		long						lLastResChoosen;
		SNDLST2_M_StaticParentDeclaration(tdstRandomElement*)
};

typedef struct _tdstThemeBridgeOld tdstThemeBridgeOld;
typedef struct _tdstThemePartOld tdstThemePartOld;
typedef struct _tdstTypeThemeOld tdstTypeThemeOld;

typedef tdstThemePartOld* hThemePartOld;
typedef tdstThemeBridgeOld* hThemeBridgeOld;
SNDLST2_M_StaticUseListOf(hThemeBridgeOld);

struct _tdstThemeBridgeOld {
    tduRefRes   uResTarget;
    tduRefRes   uRes;
    SNDLST2_M_StaticElementDeclaration(hThemeBridgeOld)
};

struct _tdstThemePartOld {
    tduRefRes   uRes;
    SNDLST2_M_StaticChildDeclaration(tdstThemePartOld*,tdstTypeThemeOld*)

	SNDLST2_M_StaticAnchorDeclaration(hThemeBridgeOld) hLstThemeBridge;
};

struct _tdstTypeThemeOld {
    unsigned long   ulStartLoop;
    unsigned long   ulEndLoop;
    tduRefRes       uBridgeIn;
    SNDLST2_M_StaticParentDeclaration(tdstThemePartOld*)
};

typedef struct _tdstThemeBridgeOld2 tdstThemeBridgeOld2;
typedef struct _tdstThemePartOld2 tdstThemePartOld2;
typedef struct _tdstTypeThemeDiskOld2 tdstTypeThemeDiskOld2;
typedef struct _tdstTypeThemeMemOld2 tdstTypeThemeMemOld2;

typedef tdstThemePartOld2* hThemePartOld2;
typedef tdstThemeBridgeOld2* hThemeBridgeOld2;
SNDLST2_M_StaticUseListOf(hThemeBridgeOld2);

typedef enum _tdeTypeTransitionOld2{
		TRANSITION_CROSSFADE_Old2,
		TRANSITION_ATENDOFPART_Old2,
        TRANSITION_FOR_ALIGNEMENT_Old2=SND_ENUM_ALIGNEMENT
} tdeTypeTransitionOld2;

struct _tdstThemePartOld2 {
    tduRefRes   uRes;
	SndBool_field_decl(bLoopStart);
	SndBool_field_decl(bLoopEnd);
	long		lNbLoops;
	tdeTypeTransitionOld2	eTransition;
    SNDLST2_M_StaticChildDeclaration(tdstThemePartOld2*,tdstTypeThemeMemOld2*)
};

struct _tdstTypeThemeDiskOld2 {
    unsigned long   ulStartLoop;
    unsigned long   ulNbParts;
    SNDLST2_M_StaticParentDeclaration(tdstThemePartOld2*)
};

struct _tdstTypeThemeMemOld2 {
    unsigned long   ulStartLoop;
    unsigned long   ulNbParts;
    SNDLST2_M_StaticParentDeclaration(tdstThemePartOld2*)
};

typedef enum _tdeTypeTransition{
		TRANSITION_CROSSFADE,
		TRANSITION_ATENDOFPART,
		TRANSITION_OUTRO,
        TRANSITION_FOR_ALIGNEMENT=SND_ENUM_ALIGNEMENT
} tdeTypeTransition;

typedef  tdstThemePart* hThemePart;

#define DEFAULT_FADEDURATION M_IntToRealSnd(0)

struct _tdstThemePart {
    tduRefRes   uRes;
	unsigned char		bLoopStart;
	unsigned char		bLoopEnd;
	short				wNbLoops;
	tdeTypeTransition	eTransition;
	SndReal				rFadeDuration;
    SNDLST2_M_StaticChildDeclaration(tdstThemePart*,tdstTypeThemeMem*)
};

struct _tdstThemePartOutro {
    tduRefRes   uRes;
	tdeTypeTransition	eTransition;
	SndReal		rFadeDuration;
};

#define C_THEME_NO_LOOP -1
#define C_THEME_PLAY_ONCE 0
#define C_THEME_LOOP_INFINITE 0x7FFFFFFF

struct _tdstTypeThemeDisk {
    unsigned long   ulStartLoop;
    unsigned long   ulEndLoop;
	unsigned short	uwNbLoops;
    unsigned short	uwNbParts;
    tdstThemePartOutro*	pstPartOutro;
	SndReal			rFadeDuration;
    SNDLST2_M_StaticParentDeclaration(tdstThemePart*)
};

struct _tdstTypeThemeMem {
    unsigned long	ulStartLoop;
    unsigned long	ulEndLoop;
    unsigned short	uwNbParts;
	unsigned short	uwNbLoops;
    tdstThemePartOutro*	pstPartOutro;
	SndReal			rFadeDuration;
    SNDLST2_M_StaticParentDeclaration(tdstThemePart*)
};

typedef struct _tdstTypeSwitchOld       tdstTypeSwitchOld;

struct _tdstTypeSwitchOld
{
		SNDLST2_M_StaticParentDeclaration(tdstSwitchElement*)
        unsigned long               ulNbElements;
        long                        lTypeSwitch;
        SND_tdxHandleToSoundInfo    hDefault;
		SndBool						bDynamic;
};

struct _tdstSwitchElement {
        tduRefRes                   uRes;
        SND_tdxHandleToSoundInfo    hIndice;
        SNDLST2_M_StaticChildDeclaration(tdstSwitchElement*,tdstTypeSwitchMem*)
} ;

#define C_SWITCH_NODEFAULT ((SND_tdxHandleToSoundInfo)-1)

struct _tdstTypeSwitchMem
{
		SNDLST2_M_StaticParentDeclaration(tdstSwitchElement*)
        long						lTypeSwitch;
        SND_tdxHandleToSoundInfo	hDefault;
        unsigned short				uwNbElements;
		unsigned char				bDynamic;
		unsigned char				bMaintainIfSwitched;
};

struct _tdstTypeSwitchDisk
{
		SNDLST2_M_StaticParentDeclaration(tdstSwitchElement*)
        long						lTypeSwitch;
        SND_tdxHandleToSoundInfo	hDefault;
        unsigned short				uwNbElements;
		unsigned char				bDynamic;
		unsigned char				bMaintainIfSwitched;
};

struct _tdstTypeSplitDisk
{
        unsigned long   ulNbRanges ;
        SndBool			bStayActive;

}  ;

struct _tdstRangeSplitDisk {
	tduRefRes   uRes ;
	SndReal	    rPitchA ;
	SndReal	    rPitchB ;
	SndReal	    rPitchC ;
	SndReal	    rPitchD ;
	SndReal	    rCoefPitch ;
	unsigned char ucVolume ;
	SNDLST2_M_StaticChildDeclaration(tdstRangeSplitDisk*,tdstRangeSplitListDisk*)
}  ;

struct _tdstRangeSplitListDisk{
	SNDLST2_M_StaticParentDeclaration(tdstRangeSplitDisk *)
}  ;

struct _tdstRangeSplitMem {
	tduRefRes   uRes ;
	SndReal	    rPitchA ;
	SndReal	    rPitchB ;
	SndReal	    rPitchC ;
	SndReal	    rPitchD ;
	SndReal	    rCoefPitch ;
	unsigned char ucVolume ;
	SNDLST2_M_StaticChildDeclaration(tdstRangeSplitMem*,tdstRangeSplitListMem*)
    SndReal     rCoefMagAB;
    SndReal     rCoefMagCD;
}  ;

struct _tdstRangeSplitListMem{
	SNDLST2_M_StaticParentDeclaration( tdstRangeSplitMem *)
}  ;

struct _tdstTypeSplitMem
{
	unsigned long           ulNbRanges;
    SndBool					bStayActive;
	tdstRangeSplitListMem   stRangeList;
} ;

struct _tdstInfoTrackDisk {
        unsigned long ulFadeIn;
        unsigned long ulFadeOut;

        unsigned long ulCrossFade;
        tduRefRes       uRes;
} ;

struct _tdstInfoTrackMem {
        SNDLST2_M_StaticChildDeclaration(tdstInfoTrackMem*,tdstInfoTrackList*)
        unsigned long ulFadeIn;
        unsigned long ulFadeOut;

        unsigned long ulCrossFade;
        tduRefRes       uRes;
} ;

struct _tdstInfoTrackList
{

        SNDLST2_M_StaticParentDeclaration(tdstInfoTrackMem*)
};

struct _tdstTypeSequenceDisk {
        SndBool			bLoop;
        unsigned long	ulStartLoop;
        unsigned long	ulEndLoop;
        unsigned short	uwNbLoops;
        unsigned short	uwNbElements;
} ;

struct _tdstTypeSequenceMem {
        SndBool			bLoop;
        unsigned long	ulStartLoop;
        unsigned long	ulEndLoop;
        unsigned short	uwNbLoops;
        unsigned short	uwNbElements;
        tdstInfoTrackList stResList;
} ;

union _tduResDisk {
        tdstTypeSampleDisk      stSample ;
        tdstTypeMidiDisk        stMidi ;
        tdstTypeCD              stCD;
        tdstTypeSequenceDisk    stSequence ;
        tdstTypeSwitchDisk      stSwitch ;
        tdstTypeSwitchOld       stSwitchOld ;
        tdstTypeSplitDisk       stSplit;
        tdstTypeThemeDisk       stTheme;
        tdstTypeRandomDisk		stRandom;
        tdstTypeThemeOld        stThemeOld;
        tdstTypeThemeDiskOld2   stThemeOld2;
} ;

struct _tdstBlockResourceDisk {
        tdxId			Id;
        tdeTypeResource	eType;
        tdeTypeStorage	eStorage;

        unsigned long	ulDataSize;
        unsigned long	ulDataOffset;
        unsigned char	ucVolume;
        tduResDisk		uRes;
};

union _tduResMem {
        tdstTypeSampleMem   stSample ;
        tdstTypeMidiMem     stMidi ;
        tdstTypeCD          stCD;
        tdstTypeSequenceMem stSequence ;
        tdstTypeSwitchMem   stSwitch ;
        tdstTypeSplitMem    stSplit;
        tdstTypeThemeMem    stTheme;
        tdstTypeRandomMem	stRandom;
} ;

struct _tdstBlockResourceMem {
        tdxId           Id;
        tdeTypeResource eType;
        tdeTypeStorage  eStorage;
        unsigned char   ucVolume;
        SndBool			bIsLoaded;
        tduResMem       uRes ;

        SNDLST2_M_StaticChildDeclaration(tdstBlockResourceMem*,tdstSndPolyResource*)
};

union _tduBlockResource {
    tdstBlockResourceMem stMem;
    tdstBlockResourceDisk stDisk;
};

struct _tdstSndPolyResource {

        SNDLST2_M_StaticParentDeclaration(tdstBlockResourceMem*)
};

struct _tdstRefRes {
        tduRefRes       uRes;
        SNDLST2_M_StaticChildDeclaration(tdstRefRes*,tdstRefResList*)
} ;

struct _tdstRefResList
{
        SNDLST2_M_StaticParentDeclaration(tdstRefRes*)
};

#endif
