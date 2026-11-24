#ifndef _SND_SCRIPT_H_
#define _SND_SCRIPT_H_

CPA_EXPORT void SND_fn_vUnRegisterEvent(char *_szCompleteSectionName);
CPA_EXPORT void SND_fn_vUnRegisterEventByPtr(SND_tdstBlockEvent *_pEvent);
CPA_EXPORT void SND_fn_vUnRegisterAllEvents(void);

CPA_EXPORT void SND_fn_vCollectReleasedEvents(void);
CPA_EXPORT SCR_tdst_Link_Table* SND_fn_pGetSoundEventLinkTable(void);

#endif
