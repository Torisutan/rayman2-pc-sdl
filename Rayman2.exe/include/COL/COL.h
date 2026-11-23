#pragma once

typedef struct HIE_tdstEngineObject_ HIE_tdstEngineObject;
typedef struct ZDX_tdstCollSet_ ZDX_tdstCollSet;

void __cdecl fn_vCollSetAlloc(HIE_tdstEngineObject *engineObject);
void __cdecl fn_vCollSetCopyClone(HIE_tdstEngineObject *pDest,
                                      HIE_tdstEngineObject *pSrc);
struct HIE_tdstSuperObject;
struct POS_tdstCompletePosition_;
void __cdecl COL_fn_vNewStaticCollisionForCharacter(struct HIE_tdstSuperObject *_hCharacter,
                                                    struct HIE_tdstSuperObject *_hSector,
                                                    struct POS_tdstCompletePosition_ *_p_stStartMatrix,
                                                    struct POS_tdstCompletePosition_ *_p_stEndMatrix);
