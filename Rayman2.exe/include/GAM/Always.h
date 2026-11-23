#ifndef RAYMAN2_GAM_ALWAYS_H
#define RAYMAN2_GAM_ALWAYS_H

struct tdstEngineObject_;
unsigned char __cdecl fn_ucIsAnAlwaysObject(struct tdstEngineObject_ *p_stEngineObject);   /* 0x0040BC60 */
void          __cdecl fn_vLastDesinitAlwaysStructure(void);             /* 0x0040BB50 */
struct HIE_tdstSuperObject;
unsigned char __cdecl fn_ucIsAlwaysGeneratedByMe(struct HIE_tdstSuperObject *_hAlways,
                                                 struct HIE_tdstSuperObject *_hGenerator); /* 0x0040C370 */

#endif
