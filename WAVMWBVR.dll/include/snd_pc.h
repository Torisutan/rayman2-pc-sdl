#ifndef _SND_PC_H_
#define _SND_PC_H_

#ifdef __cplusplus
extern "C" {
#endif

long	FUN_10006310(tduRefRes res,SoundParam *par,long prio,SND_td_pfn_vSoundCallback fn_callback,long par_callback);
void	FUN_10006500(long voice);
SndBool	FUN_10006510(long voice,SoundParam *par);
SndBool	FUN_100065b0(long voice);
void	FUN_100065e0(long voice);
void	FUN_10006620(long voice);
void	FUN_10006630(long voice);
int		FUN_10006640(SND_tdstInitStruct *pInitStruct);
void	FUN_10006680(void);
SndReal	FUN_100066c0(long voice);
SndReal	FUN_100066f0(long voice);
SndBool	FUN_100067e0(tdstBlockResourceDisk *disk,tdstBlockResourceMem *mem);
SndBool	FUN_10006810(tdstBlockResourceDisk *disk,tdstBlockResourceMem *mem,char *pDataBloc);
void	FUN_10006830(tdstBlockResourceMem *mem);
SndBool	FUN_10006840(tduRefRes res,unsigned char ucVolume);
long	FUN_10006d30(tduRefRes FirstRes,tduRefRes NextRes,SND_td_pfn_vSoundCallback transition_callback,long param_callback,SoundParam *par);
SndBool	FUN_10006ec0(long voice,SoundParam *par);
SndBool	FUN_10006f10(long voice,tduRefRes new_res);
SndBool	FUN_10006f40(long voice,tduRefRes new_res,SndReal rDuration);
SndBool	FUN_10006fa0(long voice,tduRefRes new_res);
void	FUN_10006fc0(long voice);
void	FUN_10006ff0(long voice);
void	FUN_10007010(long voice);
int		FUN_10007030(SND_tdstInitStruct *pInitStruct);

/* 0x1000BEC0 */
typedef struct _SND_pc_tdstVirtualVoiceFunctions {
	long	(SND_CALL *pfn_lPlay)(tduRefRes res,SoundParam *par,long prio,SND_td_pfn_vSoundCallback fn_callback,long par_callback);
	void	(SND_CALL *pfn_vStop)(long voice);
	SndBool	(SND_CALL *pfn_bSetParam)(long voice,SoundParam *par);
	void	(SND_CALL *pfn_vPause)(long voice);
	void	(SND_CALL *pfn_vResume)(long voice);
	SndReal	(SND_CALL *pfn_rGetPos)(long voice);
	SndReal	(SND_CALL *pfn_rGetLength)(long voice);
	void	(SND_CALL *pfn_vRemoveCallback)(long voice);
	SndBool	(SND_CALL *pfn_bTestIsPlaying)(long voice);
} SND_pc_tdstVirtualVoiceFunctions;

long	FUN_10007930(SND_tdhVirtualModule hModule,tduRefRes res,SoundParam *par,long prio,SND_td_pfn_vSoundCallback fn_callback,long par_callback);
void	FUN_10007ac0(SND_tdhVirtualModule hModule,long voice);
SndBool	FUN_10007af0(SND_tdhVirtualModule hModule,long voice,SoundParam *par);
SndBool	FUN_10007b90(SND_tdhVirtualModule hModule,long voice);
void	FUN_10007c10(SND_tdhVirtualModule hModule,long voice);
void	FUN_10007c80(SND_tdhVirtualModule hModule,long voice);
void	FUN_10007ca0(SND_tdhVirtualModule hModule,long voice);
SndReal	FUN_10007cd0(SND_tdhVirtualModule hModule,long voice);
SndReal	FUN_10007cf0(SND_tdhVirtualModule hModule,long voice);
SND_tdhVirtualModule FUN_10007d10(long lNbVoices,SND_pc_tdstVirtualVoiceFunctions *pFunctions);
void	FUN_10007dd0(SND_tdhVirtualModule hModule);
void	FUN_10007e30(SND_tdhVirtualModule hModule);
void	FUN_10007e50(SND_tdhVirtualModule hModule,long lNbVoices);

#ifdef __cplusplus
}
#endif

#endif
