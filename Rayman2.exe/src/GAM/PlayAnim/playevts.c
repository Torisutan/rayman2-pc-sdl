#include "GAM/PlayAnim.h"
#include "GAM/STRUCTUR/State.h"
#include "GAM/GAM_types.h"
#include "GAM/GAM_globals.h"
#include "GAM/ObjsTbls.h"
#include "TMP/TmpMem.h"

/* 0x0042DE30 */
extern tdstAnim3d *__cdecl fn_p_stGetAnimInState(void *hState);

/* 0x0041B0F0 */
unsigned char __cdecl GAM_fn_ucGetFirstCallOfEvent(tdstEvent *_p_stEvent);

#define fn_p_GetEventsAddress(p_stAnim, ulFrame, hTable) \
    ((tdstEvent *(__cdecl *)(tdstAnim3d *, unsigned long, tdxHandleToObjectsTablesList)) \
        fn_hGetNextAlwaysActiveCharNode)((p_stAnim), (ulFrame), (hTable))

static __inline void fn_v3dDataAllocateEventActivation(MS_tdxHandleTo3dData h_3dData, unsigned long ulNumber)
{
    h_3dData->d_ucEventActivation = (unsigned char *)TMP_M_p_Malloc(sizeof(unsigned char) * ulNumber);
}

static __inline void fn_v3dDataSetNextEvent(MS_tdxHandleTo3dData h_3dData, unsigned char ucNextEvent)
{
    h_3dData->ucNextEvent = ucNextEvent;
}

#define C_ucNormal  0
#define C_ucReverse 1
#define C_ucStopEvent 2
#define C_ucNeverPlay     		255

#include "GAM/STRUCTUR/Anim_s.h"

static __inline unsigned char fn_uc3dDataGetNextEvent(MS_tdxHandleTo3dData h_3dData)
{
	return(h_3dData->ucNextEvent);
}
static __inline void fn_v3dDataAddNextEvent(MS_tdxHandleTo3dData h_3dData,char _cNextEvent)
{
	h_3dData->ucNextEvent = (unsigned char)((long)(h_3dData->ucNextEvent)+(long)_cNextEvent);
}
static __inline void fn_v3dDataFreeEventActivation(MS_tdxHandleTo3dData h_3dData)
{
	if (h_3dData->d_ucEventActivation)
		TMP_M_Free(h_3dData->d_ucEventActivation );
}
static __inline unsigned char fn_uc3dDataGetEventActivationByIndex(MS_tdxHandleTo3dData h_3dData,unsigned long ulIndex)
{
	return(h_3dData->d_ucEventActivation[ulIndex]);
}
static __inline void fn_v3dDataAddEventActivationByIndex(MS_tdxHandleTo3dData h_3dData,unsigned long ulIndex,char _cValue)
{
	h_3dData->d_ucEventActivation[ulIndex] = (unsigned char)((long)(h_3dData->d_ucEventActivation[ulIndex])+(long)_cValue);
}

void *GAM_fn_p_xGetEventOfEvent(tdstEvent *_p_stEvent);
unsigned char GAM_fn_ucGetPeriodOfEvent(tdstEvent *_p_stEvent);
void __cdecl fn_vPlayEvent(tdstEvent *p_stEvent, unsigned char ucMode, HIE_tdstSuperObject *p_stSuperObject);

#include "SND/SND.h"

extern unsigned long __cdecl fn_ulStandardGameGetCustomBitsSO(void *hSupObj);
extern void __cdecl fn_vStandardGameSetCustomBitsSO(void *hSupObj, unsigned long ulCustomBits);
extern void __cdecl fn_v3dDataSetUserEventFlag(MS_tdxHandleTo3dData h_3dData, unsigned char ucFlagIndex, int bSet, MTH3D_tdstVector *p_stPosition);

void fn_vInitAllEvents(HIE_tdstSuperObject *p_stSuperObject, unsigned short uwFirstFrame)
{
    MS_tdxHandleTo3dData h_3dData;
    tdxHandleToState h_State;
    tdstAnim3d *p_stAnim;
    tdstEvent *p_stEvent;
    unsigned char i;

    h_3dData = M_GetMSHandle(p_stSuperObject, 3dData);
    h_State  = fn_h3dDataGetCurrentState(h_3dData);
    if (h_State == NULL) return;
    p_stAnim = fn_p_stGetAnimInState(h_State);

    p_stEvent = fn_p_GetEventsAddress(p_stAnim, uwFirstFrame, fn_h3dDataGetCurrentObjectsTable(h_3dData));

    if (p_stEvent != NULL) {

        fn_v3dDataAllocateEventActivation(h_3dData, p_stAnim->ucNumberOfEvents);

        if (uwFirstFrame) {

            for (i = 0; (i < p_stAnim->ucNumberOfEvents) && (p_stEvent[i].uwFrameNumber < uwFirstFrame); i++);

            fn_v3dDataSetNextEvent(h_3dData, i);
        }

        else {
            if (fn_scGetSpeedInState(h_State) > 0)
                fn_v3dDataSetNextEvent(h_3dData, 0);
            else
                fn_v3dDataSetNextEvent(h_3dData, (unsigned char)(p_stAnim->ucNumberOfEvents - 1));
        }

        for (i = 0; i < p_stAnim->ucNumberOfEvents; i++, p_stEvent++)
            fn_v3dDataSetEventActivationByIndex(h_3dData, i, GAM_fn_ucGetFirstCallOfEvent(p_stEvent));
    }
}

void fn_vDesInitAllEvents(HIE_tdstSuperObject *p_stSuperObject,tdxHandleToState hNextState,int bStopEvents)
{
	MS_tdxHandleTo3dData h_3dData;
	tdxHandleToState h_State;
	tdstAnim3d * p_stAnim;
	tdstEvent * p_stEvent;
	int i,j;
	unsigned short uwFrameNumber;

	h_3dData = M_GetMSHandle( p_stSuperObject, 3dData );
	h_State  = fn_h3dDataGetCurrentState(h_3dData);
	if ( h_State == NULL )
		return;
	p_stAnim = fn_p_stGetAnimInState(h_State);

	if ((p_stAnim)&&(p_stAnim->ucNumberOfEvents!=0))
	{
		uwFrameNumber=h_3dData->uwCurrentFrame;
		if ( uwFrameNumber >= p_stAnim -> uwNumberOfFrames )
			uwFrameNumber = ( unsigned short ) ( p_stAnim -> uwNumberOfFrames - 1 );

		p_stEvent=	fn_p_GetEventsAddress( p_stAnim,uwFrameNumber,fn_h3dDataGetCurrentObjectsTable(h_3dData) );

		if ( p_stEvent != NULL )
		{
			if ( bStopEvents )
			{
				tdstAnim3d * p_stNextAnim = NULL;
				tdstEvent * p_stNextEvent = NULL;
				tdstEvent * p_stNextEvent2;

				if (hNextState)
				{
					p_stNextAnim = fn_p_stGetAnimInState(hNextState);
					if ((p_stNextAnim)&&(p_stNextAnim->ucNumberOfEvents!=0))
					{
						p_stNextEvent = fn_p_GetEventsAddress(p_stNextAnim,0,fn_h3dDataGetCurrentObjectsTable(h_3dData));

						if(p_stNextEvent!=NULL)
						{
							p_stNextEvent2 = (tdstEvent*)TMP_M_p_Malloc(sizeof(tdstEvent) * p_stNextAnim->ucNumberOfEvents);
							memcpy(p_stNextEvent2,p_stNextEvent,sizeof(tdstEvent) * p_stNextAnim->ucNumberOfEvents);
							p_stNextEvent=p_stNextEvent2;

							p_stEvent = fn_p_GetEventsAddress(p_stAnim,uwFrameNumber,fn_h3dDataGetCurrentObjectsTable(h_3dData));

							for (i = 0; i < p_stAnim->ucNumberOfEvents;i++,p_stEvent++)
							{
								int bNeverPlayFound = FALSE;

								if (GAM_fn_ucGetFirstCallOfEvent(p_stEvent) == C_ucNeverPlay )
								{

									p_stNextEvent=p_stNextEvent2;

									for (j = 0;j < p_stNextAnim->ucNumberOfEvents;j++,p_stNextEvent++)
									{

										if (GAM_fn_ucGetFirstCallOfEvent(p_stNextEvent) == C_ucNeverPlay)
										{

											if (GAM_fn_p_xGetEventOfEvent(p_stNextEvent) == GAM_fn_p_xGetEventOfEvent(p_stEvent))
											{
												bNeverPlayFound = TRUE;
												break;
											}
										}

										else
											break;
									}

									if (!bNeverPlayFound)
										fn_vPlayEvent(p_stEvent,C_ucStopEvent,p_stSuperObject);
								}

								else
									break;
							}
							TMP_M_Free(p_stNextEvent2);
						}
						else
						{

							p_stEvent = fn_p_GetEventsAddress(p_stAnim,0,fn_h3dDataGetCurrentObjectsTable(h_3dData));

							for (i = 0; i < p_stAnim->ucNumberOfEvents;i++,p_stEvent++)
							{
								if (GAM_fn_ucGetFirstCallOfEvent(p_stEvent) == C_ucNeverPlay )
									fn_vPlayEvent(p_stEvent,C_ucStopEvent,p_stSuperObject);
							}
						}
					}
					else
					{

						p_stEvent = fn_p_GetEventsAddress(p_stAnim,0,fn_h3dDataGetCurrentObjectsTable(h_3dData));

						for (i = 0; i < p_stAnim->ucNumberOfEvents;i++,p_stEvent++)
						{
							if (GAM_fn_ucGetFirstCallOfEvent(p_stEvent) == C_ucNeverPlay )
								fn_vPlayEvent(p_stEvent,C_ucStopEvent,p_stSuperObject);
						}
					}
				}
			}
		}
	}

	fn_v3dDataSetNextEvent(h_3dData,0);

	fn_v3dDataFreeEventActivation(h_3dData);
}

void fn_vPlayEventsSinceLastOneNoLoop( HIE_tdstSuperObject *p_stSuperObject )
{
	MS_tdxHandleTo3dData h_3dData;
	tdxHandleToState h_State;
	tdstAnim3d * p_stAnim;
	tdstEvent * p_stEvent;

	h_3dData = M_GetMSHandle( p_stSuperObject, 3dData );
	h_State  = fn_h3dDataGetCurrentState(h_3dData);
	if ( h_State == NULL ) return;
	p_stAnim = fn_p_stGetAnimInState(h_State);

	p_stEvent=	fn_p_GetEventsAddress( p_stAnim, h_3dData->uwCurrentFrame,fn_h3dDataGetCurrentObjectsTable(h_3dData) );

	if ( p_stEvent != NULL )
	{
		if ( fn_scGetSpeedInState(h_State) > 0 )
		{

			while ( fn_uc3dDataGetNextEvent(h_3dData) !=p_stAnim->ucNumberOfEvents && p_stEvent[ fn_uc3dDataGetNextEvent(h_3dData) ].uwFrameNumber <= fn_uw3dDataGetCurrentFrame(h_3dData))
			{

				if ( fn_uc3dDataGetEventActivationByIndex(h_3dData,fn_uc3dDataGetNextEvent(h_3dData)) == 0 )
				{

					fn_vPlayEvent( &p_stEvent[ fn_uc3dDataGetNextEvent(h_3dData) ], C_ucNormal ,p_stSuperObject);

					fn_v3dDataSetEventActivationByIndex(h_3dData,fn_uc3dDataGetNextEvent(h_3dData),GAM_fn_ucGetPeriodOfEvent(&(p_stEvent[ fn_uc3dDataGetNextEvent(h_3dData) ])));
				}
				else
				{

					if ( fn_uc3dDataGetEventActivationByIndex(h_3dData,fn_uc3dDataGetNextEvent(h_3dData)) != C_ucNeverPlay )
						fn_v3dDataAddEventActivationByIndex(h_3dData,fn_uc3dDataGetNextEvent(h_3dData),-1);
				}

				fn_v3dDataAddNextEvent(h_3dData,1);
			}
		}

		else
		{

			while ( fn_uc3dDataGetNextEvent(h_3dData) != 255 && p_stEvent[ fn_uc3dDataGetNextEvent(h_3dData) ].uwFrameNumber >= fn_uw3dDataGetCurrentFrame(h_3dData))
			{

				if ( fn_uc3dDataGetEventActivationByIndex(h_3dData,fn_uc3dDataGetNextEvent(h_3dData)) == 0 )
				{
					fn_vPlayEvent( &p_stEvent[ fn_uc3dDataGetNextEvent(h_3dData) ], C_ucReverse ,p_stSuperObject);
					fn_v3dDataSetEventActivationByIndex(h_3dData,fn_uc3dDataGetNextEvent(h_3dData),GAM_fn_ucGetPeriodOfEvent(&(p_stEvent[ fn_uc3dDataGetNextEvent(h_3dData) ])));
				}
				else
				{

					if ( fn_uc3dDataGetEventActivationByIndex(h_3dData,fn_uc3dDataGetNextEvent(h_3dData) ) != C_ucNeverPlay )
						fn_v3dDataAddEventActivationByIndex(h_3dData,fn_uc3dDataGetNextEvent(h_3dData),-1);
				}
				fn_v3dDataAddNextEvent(h_3dData,-1);

			}
		}
	}
}

void fn_vPlayLastEventsBeforeRestart( HIE_tdstSuperObject *p_stSuperObject )
{
	MS_tdxHandleTo3dData h_3dData;
	tdxHandleToState h_State;
	tdstAnim3d * p_stAnim;
	tdstEvent * p_stEvent;

	  char i;

	h_3dData = M_GetMSHandle( p_stSuperObject, 3dData );
	h_State  = fn_h3dDataGetCurrentState(h_3dData);
	if ( h_State == NULL ) return;
	p_stAnim = fn_p_stGetAnimInState(h_State);

	p_stEvent=	fn_p_GetEventsAddress( p_stAnim, h_3dData->uwCurrentFrame-1,fn_h3dDataGetCurrentObjectsTable(h_3dData) );

	if ( p_stEvent != NULL )
	{
		if ( fn_scGetSpeedInState(h_State) > 0 )
		{

			for ( i = fn_uc3dDataGetNextEvent(h_3dData); i < p_stAnim->ucNumberOfEvents; i++ )
			{

				if ( fn_uc3dDataGetEventActivationByIndex(h_3dData,i) == 0 )
				{
					fn_vPlayEvent( &p_stEvent[ i ], C_ucNormal ,p_stSuperObject);
					fn_v3dDataSetEventActivationByIndex(h_3dData, i, GAM_fn_ucGetPeriodOfEvent(&(p_stEvent[ i ])));
				}
				else
				{

					if ( fn_uc3dDataGetEventActivationByIndex(h_3dData,i) != C_ucNeverPlay )
						fn_v3dDataAddEventActivationByIndex(h_3dData, i , -1);
				}
			}

			fn_v3dDataSetNextEvent(h_3dData,0);
		}
		else
		{

			if ( fn_uc3dDataGetNextEvent(h_3dData) != 255 )
			{
				for ( i = fn_uc3dDataGetNextEvent(h_3dData); i >= 0; i-- )
				{

					if ( fn_uc3dDataGetEventActivationByIndex(h_3dData,i) == 0 )
					{
						fn_vPlayEvent( &p_stEvent[ i ], C_ucReverse ,p_stSuperObject);
						fn_v3dDataSetEventActivationByIndex(h_3dData, i, GAM_fn_ucGetPeriodOfEvent(&(p_stEvent[ i ])));
					}
					else
					{

						if ( fn_uc3dDataGetEventActivationByIndex(h_3dData,i) != C_ucNeverPlay )
							fn_v3dDataAddEventActivationByIndex(h_3dData, i , -1);
					}
				}
			}

			fn_v3dDataSetNextEvent(h_3dData,(unsigned char)(p_stAnim->ucNumberOfEvents - 1));
		}
	}
}

unsigned char GAM_fn_ucGetTypeOfEvent(tdstEvent *_p_stEvent);
static __inline HIE_tdstSuperObject *fn_hGetSuperObjectInChannel(CHN_tdxHandleToChannelArray hChannelArray, unsigned short uwChannelNumber);

void __cdecl fn_vPlayEvent( tdstEvent * p_stEvent, unsigned char ucMode ,HIE_tdstSuperObject *p_stSuperObject)
{
    SND_tduRefEvt SoundEvt;

	switch( GAM_fn_ucGetTypeOfEvent(p_stEvent) )
	{
	case C_ucSOUND_EVENT :
		SoundEvt.pstPtr=GAM_fn_p_xGetEventOfEvent(p_stEvent);
		SND_fn_lSendRequestSound((long)p_stSuperObject,GAM_g_lSoundObjectTypeAnim,SoundEvt.pstPtr,0,NULL);
		break;

	case C_ucGENERATE_EVENT :
		break;

	case C_ucGENERIC_EVENT :
		{
			long lEvent;
			unsigned char ucTypeOfEvent;
			HIE_tdstSuperObject *hSupObjTakenObject;

			lEvent=(long)GAM_fn_p_xGetEventOfEvent(p_stEvent);
			ucTypeOfEvent=(unsigned char)(lEvent&0xff);

			if ( (ucTypeOfEvent >= C_ucEvent_User0) && (ucTypeOfEvent <= C_ucEvent_User7) )
			{
				MTH3D_tdstVector stPosition;

				hSupObjTakenObject = fn_hGetSuperObjectInChannel(fn_h3dDataGetChannelSOList(M_GetMSHandle(p_stSuperObject,3dData)),p_stEvent->uwChannelNumber);
				if ( hSupObjTakenObject )
				{
					unsigned long ulCustomBits = fn_ulStandardGameGetCustomBitsSO(p_stSuperObject);

					if(!(ulCustomBits & GAM_C_CustBitNeedModuleMatrices))
					{
						fn_vStandardGameSetCustomBitsSO(p_stSuperObject,ulCustomBits|GAM_C_CustBitNeedModuleMatrices);
						PLA_fn_vRefreshGlobalMatrixUnderCharacter(p_stSuperObject);
						fn_vStandardGameSetCustomBitsSO(p_stSuperObject,ulCustomBits);
					}

					POS_fn_vGetTranslationVector(HIE_fn_hGetSuperObjectGlobalMatrix(hSupObjTakenObject), &stPosition);
					fn_v3dDataSetUserEventFlag
						(
						M_GetMSHandle(p_stSuperObject,3dData),
						(unsigned char) (ucTypeOfEvent - C_ucEvent_User0),
						TRUE,
						&stPosition
						);
				}
			}
		}
		break;
	default :
		break;
	}
}

unsigned short __cdecl fn_uwGetNbEngineFrameSinceLastMechEvent(HIE_tdstSuperObject *p_stSuperObject)
{
    return M_GetMSHandle(p_stSuperObject, 3dData)->uwNbEngineFrameSinceLastMechEvent;
}

unsigned char GAM_fn_ucGetTypeOfEvent(tdstEvent *_p_stEvent)
{
    return _p_stEvent->p_stfEventInTBL->ucTypeOfEvent;
}

/* 0x0041B0E0 */
void *GAM_fn_p_xGetEventOfEvent(tdstEvent *_p_stEvent)
{
    return _p_stEvent->p_stfEventInTBL->p_xEvent;
}

/* 0x0041B0F0 */
unsigned char __cdecl GAM_fn_ucGetFirstCallOfEvent(tdstEvent *_p_stEvent)
{
    return _p_stEvent->p_stfEventInTBL->ucFirstCall;
}

/* 0x0041B100 */
unsigned char GAM_fn_ucGetPeriodOfEvent(tdstEvent *_p_stEvent)
{
    return _p_stEvent->p_stfEventInTBL->ucPeriod;
}

static __inline HIE_tdstSuperObject *fn_hGetSuperObjectInChannel(CHN_tdxHandleToChannelArray hChannelArray, unsigned short uwChannelNumber)
{
	return (hChannelArray) ? hChannelArray[uwChannelNumber].hSupObject : NULL;
}
