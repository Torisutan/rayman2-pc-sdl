#include "SNDinc.h"

#ifndef NO_ACP_SOUND

#if defined(_DLL_COMPILATION_MODE)
#include "snddll.h"
#endif
#include "sndmixvol_pc.h"

MIXERCONTROL		SND_pc_stMixerControl = {0};
MIXERCONTROLDETAILS	SND_pc_stMixerDetails = {0};
MIXERLINECONTROLS	SND_pc_stMixerLineControls = {0};
DWORD				SND_pc_dw27D90 = 0;
MIXERLINE			SND_pc_stMixerLine = {0};
MIXERCAPS			SND_pc_stMixerCaps = {0};
DWORD				SND_pc_dwMixerDevs = 0;
MIXERCONTROLDETAILS_UNSIGNED SND_pc_stMixerValue = {0};
UINT				SND_pc_uMixerId = 0;
HMIXER				SND_pc_hMixer = NULL;
long				SND_pc_lMixerOk = 0;

SND_pc_tdhWindow	SND_pc_hMainWindow = NULL;
SND_pc_tdhInstance	SND_pc_hFirstProcessInstance = NULL;
SND_pc_tdhInstance	SND_pc_hProcessInstance = NULL;

long SND_pc_fn_lOpenMixerVolume(SND_pc_tdhDevice hWaveOut)
{
	SND_pc_dwMixerDevs = mixerGetNumDevs();
	if (SND_pc_dwMixerDevs == 0) {
		SND_fn_vDisplayError(0x3D,"");
		return 1;
	}
	if (hWaveOut != NULL) {
		if (mixerGetID((HMIXEROBJ)hWaveOut,&SND_pc_uMixerId,MIXER_OBJECTF_HWAVEOUT) != 0) {
			if (mixerGetID(0,&SND_pc_uMixerId,0) != 0) {
				SND_fn_vDisplayError(0x3D,"");
				return 1;
			}
		}
	}
	else {
		if (mixerGetID(0,&SND_pc_uMixerId,0) != 0) {
			SND_fn_vDisplayError(0x3E,"");
			return 1;
		}
	}
	if (mixerOpen(&SND_pc_hMixer,SND_pc_uMixerId,0,0,0) != 0) {
		SND_fn_vDisplayError(0x3F,"");
		return 1;
	}
	if (mixerGetDevCapsA((UINT)SND_pc_hMixer,&SND_pc_stMixerCaps,sizeof(MIXERCAPS)) != 0) {
		SND_fn_vDisplayError(0x40,"");
		return 1;
	}
	SND_pc_dw27D90 = SND_pc_stMixerCaps.cDestinations;
	SND_pc_stMixerLine.cbStruct = sizeof(MIXERLINE);
	SND_pc_stMixerLine.dwComponentType = MIXERLINE_COMPONENTTYPE_SRC_WAVEOUT;
	SND_pc_lMixerOk = (mixerGetLineInfoA((HMIXEROBJ)SND_pc_hMixer,&SND_pc_stMixerLine,
		MIXER_OBJECTF_HMIXER | MIXER_GETLINEINFOF_COMPONENTTYPE) == 0);
	SND_pc_stMixerLineControls.cbStruct = sizeof(MIXERLINECONTROLS);
	SND_pc_stMixerLineControls.dwLineID = SND_pc_stMixerLine.dwLineID;
	SND_pc_stMixerLineControls.dwControlType = MIXERCONTROL_CONTROLTYPE_VOLUME;
	SND_pc_stMixerLineControls.cControls = 1;
	SND_pc_stMixerLineControls.cbmxctrl = sizeof(MIXERCONTROL);
	SND_pc_stMixerLineControls.pamxctrl = &SND_pc_stMixerControl;
	SND_pc_lMixerOk = (mixerGetLineControlsA((HMIXEROBJ)SND_pc_hMixer,&SND_pc_stMixerLineControls,
		MIXER_OBJECTF_HMIXER | MIXER_GETLINECONTROLSF_ONEBYTYPE) == 0);
	SND_pc_stMixerDetails.cbStruct = sizeof(MIXERCONTROLDETAILS);
	SND_pc_stMixerDetails.dwControlID = SND_pc_stMixerControl.dwControlID;
	SND_pc_stMixerDetails.cChannels = 1;
	SND_pc_stMixerDetails.cMultipleItems = SND_pc_stMixerControl.cMultipleItems;
	SND_pc_stMixerDetails.cbDetails = sizeof(MIXERCONTROLDETAILS_UNSIGNED);
	SND_pc_stMixerDetails.paDetails = &SND_pc_stMixerValue;
	return 0;
}

unsigned long SND_pc_fn_ulGetMixerVolumeMax(void)
{
	return SND_pc_stMixerControl.Bounds.dwMaximum;
}

unsigned long SND_pc_fn_ulGetMixerVolumeMin(void)
{
	return SND_pc_stMixerControl.Bounds.dwMinimum;
}

unsigned long SND_pc_fn_ulGetMixerVolume(void)
{
	unsigned long ulMin;
	unsigned long ulStep;

	if (SND_pc_lMixerOk != 0) {
		ulMin = SND_pc_fn_ulGetMixerVolumeMin();
		ulStep = (SND_pc_fn_ulGetMixerVolumeMax() - ulMin) / 100;
		if (mixerGetControlDetailsA((HMIXEROBJ)SND_pc_hMixer,&SND_pc_stMixerDetails,
			MIXER_OBJECTF_HMIXER | MIXER_GETCONTROLDETAILSF_VALUE) != 0) {
			return 1;
		}
		return (SND_pc_stMixerValue.dwValue - SND_pc_fn_ulGetMixerVolumeMin()) / ulStep;
	}
	return 0;
}

void SND_pc_fn_vSetMixerVolume(unsigned long ulVolume)
{
	unsigned long ulMin;
	unsigned long ulStep;

	if (SND_pc_lMixerOk == 0) {
		return;
	}
	ulMin = SND_pc_fn_ulGetMixerVolumeMin();
	ulStep = (SND_pc_fn_ulGetMixerVolumeMax() - ulMin) / 100;
	SND_pc_stMixerValue.dwValue = SND_pc_fn_ulGetMixerVolumeMin() + ulStep * ulVolume;
	mixerSetControlDetails((HMIXEROBJ)SND_pc_hMixer,&SND_pc_stMixerDetails,
		MIXER_OBJECTF_HMIXER | MIXER_SETCONTROLDETAILSF_VALUE);
}

long SND_pc_fn_lIsMixerVolumeOk(void)
{
	return SND_pc_lMixerOk;
}

long SND_pc_fn_lCloseMixerVolume(void)
{
	if (mixerClose(SND_pc_hMixer) != 0) {
		SND_fn_vDisplayError(0x41,"");
		return 1;
	}
	return 0;
}

#endif
