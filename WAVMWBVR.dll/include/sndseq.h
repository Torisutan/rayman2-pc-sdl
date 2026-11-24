#ifndef __SNDSEQ__H__
#define __SNDSEQ__H__

SndBool		SND_CALL	SND_fn_bSetParamSeq(long voice,SoundParam *par);
SndBool		SND_fn_bTestInitSeq(void);
SndBool		SND_CALL SND_fn_bTestIsPlayingSeq(long voice);
int			SND_fn_iInitSeq(void);
long		SND_fn_lPlaySeq(tduRefRes res,SoundParam *par,long prio,SND_td_pfn_vSoundCallback fn_callback,long par_callback,SndBool* valid);
#ifndef NO_ACP_LDBIN
SndBool		SND_fn_bLoadResBinarySeq(tdstBlockResourceDisk *disk,tdstBlockResourceMem *mem,char *pDataBloc);
#endif

void		SND_fn_vDesInitSeq(void);
void		SND_CALL	SND_fn_vPauseSeq(long voice);
void		SND_CALL	SND_fn_vResumeSeq(long voice);
void		SND_CALL	SND_fn_vRemoveCallbackSeq(long voice);
void		SND_CALL	SND_fn_vStopSeq(long voice);
SndReal		SND_CALL	SND_fn_rGetPosSeq(long voice);
SndReal		SND_CALL	SND_fn_rGetLengthSeq(long voice);

#endif
