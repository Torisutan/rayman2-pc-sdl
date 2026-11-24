#ifndef _SNDEVENT_H_
#define _SNDEVENT_H_

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

#ifdef M_SET_ALIGNMENT
#pragma pack(push)
#pragma pack(M_SET_ALIGNMENT)
#endif

typedef enum {
        EVT_INVALID,
		EVT_SON_PLAY ,
		EVT_SON_STOP ,
		EVT_SON_STOP_ALL ,
		EVT_SON_STOP_N_GO,
		EVT_SON_STOP_N_GO_CROSSFADE ,
		EVT_SON_PITCH ,
		EVT_SON_VOLUME ,
		EVT_SON_PAN ,
		EVT_SON_EFFET,
		EVT_SON_EXTRA,
		EVT_SON_CHANGE_VOLUME,
        FOR_ALIGNEMENT=32767
} tdeTypeEvent ;

#define NB_TYPES_EVT 12

typedef struct {
	SndReal		rDelta;
} tdstDeTune;

#define NO_DETUNING C_SNDREAL_0

typedef struct {
	tduRefRes	uResource;
	tdstDeTune	stDeTune;
} tdstEvtPlay;

typedef struct {
	SND_tduRefEvt  uEvt;
} tdstEvtStop;

typedef struct {
	SND_tduRefEvt	uEvtStop;
	SND_tduRefEvt	uEvtGo;
} tdstEvtStopNGo;

typedef struct {
	SND_tduRefEvt	uEvtStop;
	SND_tduRefEvt	uEvtGo;
	int			iTime;
} tdstEvtStopNGoFade;

typedef struct {
	tduRefRes	uResource;
	SndReal		CoefA;
	SndReal		CoefB;
	tdstDeTune	stDeTune;
} tdstEvtExtraParam;

typedef struct {
	tduRefRes	uResource;
	SndRealTwin	rtCoefPitch;
	SndRealTwin	rtCoefVol;
	SndRealTwin	rtCoefPan;
} tdstEvtExtraAll;

typedef struct {
	int	iIdEffect;
} tdstEvtEffect;

typedef struct {
	SndBool	bConcernWithObjectType;
	long	lNewVolume;
} tdstEvtchangeVolume;

typedef union {
		tdstEvtPlay			stPlay;
		tdstEvtStop			stStop;
		tdstEvtStopNGo		stStopNGo;
		tdstEvtStopNGoFade	stStopNGoFade;
		tdstEvtExtraParam	stExtraParam;
		tdstEvtExtraAll		stExtraAll;
		tdstEvtEffect		stEffect;
		tdstEvtchangeVolume stChangeVolume;
} tduEvtParams;

struct _SND_tdstBlockEvent {
	tdxId			Id;
	tdeTypeEvent	eType;
	unsigned char	bStopable;
	unsigned char	bDynamic;
	tduEvtParams	uParams;

	SNDLST2_M_StaticChildDeclaration(SND_tdstBlockEvent*,tdstSndPolyEvent*)
};

struct _tdstSndPolyEvent {

	SNDLST2_M_StaticParentDeclaration(SND_tdstBlockEvent*)
};

#ifdef M_SET_ALIGNMENT
#pragma pack(pop)
#endif

#endif
