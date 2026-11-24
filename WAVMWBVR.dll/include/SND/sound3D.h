#ifndef _SOUND3D_H_
#define _SOUND3D_H_

#ifdef __cplusplus
extern "C" {
#endif

#include "SND/sndtype.h"

#undef CPA_EXPORT
#if defined(CPA_WANTS_IMPORT)
#define CPA_EXPORT __declspec(dllimport)
#elif defined(CPA_WANTS_EXPORT)
#define CPA_EXPORT __declspec(dllexport)
#else
#define CPA_EXPORT
#endif

extern CPA_EXPORT long SND_fn_lSendRequestSound(long indice,long type,SND_tduRefEvt evt,long prio,SND_td_pfn_vSoundCallback proc);
extern CPA_EXPORT void SND_fn_vKillObjectSound(long indice);
extern CPA_EXPORT void SND_fn_vKillObjectSound2(long indice,long type);
extern CPA_EXPORT void SND_fn_vKillAllObjectTypeSound(long type);
extern CPA_EXPORT void SND_fn_vKillAllObjectSound(void);
extern CPA_EXPORT void SND_fn_vKillAllObjectButTypeSound(long indice);

extern CPA_EXPORT long SND_fn_lAddVolumeLineSound(void);
#define SND_fn_lAddObjectTypeSound(ptrfn) SND_fn_lAddObjectTypeSound2(ptrfn,SND_C_LINE_FANTOME)
extern CPA_EXPORT long SND_fn_lAddObjectTypeSound2(tdstAllRetObjectSound *ptrfct,long lLine);
extern CPA_EXPORT void SND_fn_vSetRetObjectTypeSound(long type,tdstAllRetObjectSound *ptrfct);
extern CPA_EXPORT void SND_fn_vSetInfoForTypeSound(long type,td_pfn_vCopyString);
extern CPA_EXPORT void SND_fn_vGetInfoForObjectSound(long id,long type,char* str,long size_ptr);
extern CPA_EXPORT void SND_fn_vSetRollOffFunctionForTypeSound(long type,td_pfn_bRetRollOffFactor fct);
extern CPA_EXPORT void SND_fn_vSetRetMicroSound(tdstAllRetMicroSound *ptrfct);
extern CPA_EXPORT long SND_fn_lGetVolumeLineSound(long lLine);
extern CPA_EXPORT void SND_fn_vSetVolumeLineSound(long lLine,long lNewVolume);

extern CPA_EXPORT void SND_fn_vChangeVolumeSound(long indice,long type,long lNewVolume);
extern CPA_EXPORT void SND_fn_vResetVolumeSound(long indice,long type);
extern CPA_EXPORT long SND_fn_lGetVolumeObjectSound(long indice,long type);

extern CPA_EXPORT void SND_fn_vChangeVolumeAllObjectButOneSound(long indice,long type,long lNewVolume);
extern CPA_EXPORT void SND_fn_vResetVolumeAllObjectButOneSound(long indice,long type);

extern CPA_EXPORT void SND_fn_vChangeVolumeTypeSound(long type,long lNewVolume);
extern CPA_EXPORT void SND_fn_vResetVolumeTypeSound(long type);
extern CPA_EXPORT long SND_fn_lGetVolumeTypeSound(long type);

extern CPA_EXPORT void SND_fn_vChangeVolumeButOneTypeSound(long type,long lNewVolume);
extern CPA_EXPORT void SND_fn_vResetVolumeButOneTypeSound(long type);

extern CPA_EXPORT void SND_fn_vChangeVolumeAllObjectSound(long type,long lNewVolume);
extern CPA_EXPORT void SND_fn_vResetVolumeAllObjectSound(long type);

extern CPA_EXPORT void SND_fn_vChangeVolumeAllTypeSound(long lNewVolume);
extern CPA_EXPORT void SND_fn_vResetVolumeAllTypeSound(void);

extern CPA_EXPORT void SND_fn_vSetEffectSound(long id_effect);

extern CPA_EXPORT void SND_fn_vSynchroSound(void);
extern CPA_EXPORT int SND_fn_iInitSound(SND_tdstInitStruct *pInitStruct);
extern CPA_EXPORT int SND_fn_iTestInitSound(void);
extern CPA_EXPORT void SND_fn_vDesInitSound(void);

extern CPA_EXPORT void SND_fn_vSetDopplerFactorSound(SndReal factor);
extern CPA_EXPORT void SND_fn_vSetDefaultRollOffSound(RollOffParam *rolloff);

extern CPA_EXPORT SndReal SND_fn_rGetPosSound(long handle);
extern CPA_EXPORT SndReal SND_fn_rGetLengthSound(long handle);

extern CPA_EXPORT void SND_fn_vPauseSound(void);
extern CPA_EXPORT void SND_fn_vResumeSound(void);
extern CPA_EXPORT void SND_fn_vPauseObjectSound(long indice,long type);
extern CPA_EXPORT void SND_fn_vResumeObjectSound(long indice,long type);
extern CPA_EXPORT void SND_fn_vPauseTypeSound(long type);
extern CPA_EXPORT void SND_fn_vResumeTypeSound(long type);
extern CPA_EXPORT void SND_fn_vPauseButOneTypeSound(long type);
extern CPA_EXPORT void SND_fn_vResumeButOneTypeSound(long type);

extern void SND_fn_vKillAllObjectSound2(long _lType);
extern void SND_CALL SND_fn_vFadeOutTheme(void);

extern CPA_EXPORT long SND_fn_lCreateMicroSound(void);
extern CPA_EXPORT void SND_fn_vDestroyMicroSound(long indice);
extern CPA_EXPORT void SND_fn_vDestroyAllMicroSound(void);

extern CPA_EXPORT SndBool SND_fn_bReleaseDriverSound(void);
extern CPA_EXPORT SndBool SND_fn_bRestoreDriverSound(void);
extern CPA_EXPORT void SND_fn_vReleaseDriverSound(void);
extern CPA_EXPORT void SND_fn_vRestoreDriverSound(void);
extern CPA_EXPORT void SND_fn_vForceReleaseDriverSound(void);
extern CPA_EXPORT void SND_fn_vForceRestoreDriverSound(void);

extern CPA_EXPORT long SND_fn_lGetVolumeSound(void);
extern CPA_EXPORT void SND_fn_vSetVolumeSound(long vol);

extern CPA_EXPORT void SND_fn_vSetStereoSound(SndBool active);
extern CPA_EXPORT SndBool SND_fn_bGetStereoSound(void);
extern CPA_EXPORT SndBool SND_fn_bIsMusicPlayingSound(void);
extern CPA_EXPORT void SND_fn_vSetReverseStereoSound(SndBool active);
extern CPA_EXPORT SndBool SND_fn_bGetReverseStereoSound(void);

extern CPA_EXPORT void SND_fn_vGetTargetLabelSound(char* name,int nb_char);
extern CPA_EXPORT void SND_fn_vSetupTargetSound(void);
extern CPA_EXPORT SndBool SND_fn_bCanSetupTargetSound(void);

extern CPA_EXPORT long SND_fn_lCreateNewBufferSound(unsigned long nb_samples,unsigned short uwResolution,unsigned short uwNbChannels,unsigned long ulFreq,SoundParam* par,td_pfn_vRefreshBufferClient callback,long user_id);
extern CPA_EXPORT void SND_fn_vDeleteBufferSound(long id_buffer);
extern CPA_EXPORT void SND_fn_vPauseBufferSound(long id_buffer);
extern CPA_EXPORT void SND_fn_vResumeBufferSound(long id_buffer);
extern CPA_EXPORT SndReal SND_fn_rGetPosBufferSound(long id_buffer);
extern CPA_EXPORT void SND_fn_vSetParamBufferSound(long id_buffer,SoundParam* par);
extern CPA_EXPORT long SND_fn_lCreateNewBufferExSound(SND_tdstFormat* pformat,SND_tdstCallback* pCallback,SoundParam* par,long user_id);
extern CPA_EXPORT long SND_fn_lGetPosBufferExSound(long id_buffer);
extern CPA_EXPORT long SND_fn_lPushBufferExSound(long id_buffer,SND_tdstStackBuffer* pStack);

extern CPA_EXPORT SND_tdstBlockEvent* SND_fn_pstGetLastEventForObjectTypeSound(long type);

extern CPA_EXPORT long	SND_fn_lGetNbVoiceOptimumSound(void);
extern CPA_EXPORT long	SND_fn_lGetNbVoiceWishedSound(void);
extern CPA_EXPORT void	SND_fn_vSetNbVoiceWishedSound(long nb);

extern CPA_EXPORT SND_tduRefEvt SND_fn_uGenerateEventPlaySound(void* data,SND_tdstFormat* pformat,SndBool loop);
extern CPA_EXPORT SND_tduRefEvt SND_fn_uGenerateEventStopSound(SND_tduRefEvt event);
extern CPA_EXPORT void SND_fn_vDestroyEventSound(SND_tduRefEvt event);

extern CPA_EXPORT SndBool	SND_fn_bTestFormatRecordSound(SND_tdstFormat* pformat,int flags);
extern CPA_EXPORT long		SND_fn_lCreateBufferRecordSound(SND_tdstFormat* pformat,SND_tdstCallback* pCallback);
extern CPA_EXPORT SndBool	SND_fn_bIsBufferFullDuplexRecordSound(long id_buffer);
extern CPA_EXPORT SndBool	SND_fn_bStartRecordSound(long id_buffer);
extern CPA_EXPORT void		SND_fn_vStopRecordSound(long id_buffer);
extern CPA_EXPORT SndBool	SND_fn_bIsWorkingRecordSound(long id_buffer);
extern CPA_EXPORT long		SND_fn_lPushBufferToRecordSound(long id_buffer,void* ptr,int size_ptr);
extern CPA_EXPORT SndBool	SND_fn_bManageMsgForRecordSound(long p_msg);
extern CPA_EXPORT unsigned char SND_fn_ucGetInstantVuMeterRecordSound(void);
#define FORMATREC_RESOLUTION 1
#define FORMATREC_FREQUENCY 2
#define FORMATREC_NBCHANNEL 4
#define FORMATREC_ZIP 8

#define EVT_FANTOME_ID -1
#define EVT_FANTOME_PTR NULL
#define SND_C_OBJET_FANTOME -1

#define SND_C_TYPE_FANTOME 0

#define SND_C_LINE_FANTOME ((long)NULL)

#define SND_C_POS_UNKNOWN -1
#define SND_C_POS_ENDED -2

#define SND_C_LENGTH_UNKNOWN -1

#define C_INITSOUND_FAILED 0
#define C_INITSOUND_OK 1

#define SND_C_EVT_CALLBACK_FAILED -1
#define SND_C_EVT_FAILED -2

#define SND_C_NO_DOPPLER (M_IntToRealSnd(0))
#define SND_C_NORMAL_DOPPLER (M_IntToRealSnd(1))

#define NO_FILTRE 0
#define NO_REVERB 0
#define NO_EFFECT 0

#ifdef __cplusplus
}
#endif

#endif
