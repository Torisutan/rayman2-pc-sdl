#pragma once

#include "POS/POS_types.h"

struct GLI_tdstLight_;
struct GEO_tdstColor_;
float         __cdecl GLI_fn_xFogGetFar(struct GLI_tdstLight_ *p_stLight);        /* 0x00487850 */
float         __cdecl GLI_fn_xFogGetNear(struct GLI_tdstLight_ *p_stLight);       /* 0x004270E0 */
float         __cdecl GLI_fn_xFogGetInfinite(struct GLI_tdstLight_ *p_stLight);   /* 0x004270F0 */
float         __cdecl GLI_fn_xFogGetBlendNear(struct GLI_tdstLight_ *p_stLight);  /* 0x00427100 */
float         __cdecl GLI_fn_xFogGetBlendFar(struct GLI_tdstLight_ *p_stLight);   /* 0x00427110 */

void          __cdecl GLI_vFogGetColor(void *p_stLight, void *p_stOutColor);  /* 0x00426F90 */

#define GLI_C_lFogLight 6
long          __cdecl GLI_lGetLightType(void *p_stLight);                     /* 0x004194B0 */
void          __cdecl GLI_xGetLightBackgroundColor(struct GLI_tdstLight_ *p_stLight, struct GEO_tdstColor_ *p_stColor); /* 0x00427090 */

unsigned char __cdecl GLI_ucGetObjectLighted(void *p_stLight);   /* 0x004270C0 */
void          __cdecl GLI_vSetObjectLighted(void *p_stLight, unsigned char value); /* 0x004270D0 */

void          __cdecl GLI_xSetLightState(void *p_stLight, long b_lOnOff);   /* 0x00426F40 */
void          __cdecl GLI_xGetLightState(void *p_stLight, long *pb_lOnOff); /* 0x00426F50 */

void          __cdecl GLI_vSetLightNearFar(void *p_stLight, float xNear, float xFar);          /* 0x00426FE0 */
void          __cdecl GLI_xSetLightMatrix(void *p_stLight, POS_tdstCompletePosition *p_stMatrix); /* 0x00426FC0 */
void          __cdecl GLI_vGetLightNearFar(void *p_stLight, float *p_xNear, float *p_xFar);     /* 0x00427000 */
void          __cdecl GLI_xSetLightColor(struct GLI_tdstLight_ *p_stLight, struct GEO_tdstColor_ *p_stColor); /* 0x00426F60 */
void          __cdecl GLI_vSetLightType(void *p_stLight, long lNewType);                        /* 0x00427080 */
void          __cdecl GLI_vGetLightAlphas(void *p_stLight, float *p_xLittleAlpha, float *p_xBigAlpha); /* 0x00427060 */
void          __cdecl GLI_vSetLightAlphas(void *p_stLight, float xLittleAlpha, float xBigAlpha);       /* 0x00427020 */

void          __cdecl GLI_vFogOn(void);                          /* 0x00424180 */
void          __cdecl GLI_vFogOff(void);                         /* 0x00424190 */

/* 0x004A7248 */
extern int GLI_g_lGlobalFogIsOn_storage;
#define GLI_g_cGlobalFogIsOn_ADDR  ((void *)&GLI_g_lGlobalFogIsOn_storage)
#define GLI_g_cGlobalFogIsOn  (*(char *)GLI_g_cGlobalFogIsOn_ADDR)
