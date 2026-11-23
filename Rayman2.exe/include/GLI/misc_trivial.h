#pragma once

/* 0x00421800 */
void          __cdecl FUN_00421820(void);
void          __cdecl GLI_pc_fn_vBeginScene(unsigned short hDevice); /* 0x420b00 */
void          __cdecl GLI_pc_fn_vEndScene(unsigned short hDevice);   /* 0x420b10 */
void          __cdecl FUN_00421860(unsigned int arg);
int           __cdecl GLI_fn_cWindowedModeIsOptimized(void);   /* 0x422020 */
void          __cdecl GLI_fn_vOptimizedWindowedMode(void);     /* 0x422030 */
void          __cdecl GLI_fn_vNonOptimizedWindowedMode(void);  /* 0x422040 */
void          __cdecl GLI_fn_vCloseBigFileTextures(void);
void          __cdecl FUN_00428da0(int base, short *p, int idx, int arg4);
void          __cdecl GLI_vSetLensFlareRef(void *p_vRef);      /* 0x00428DD0 */

void          __cdecl FUN_00423610(const char *name);          /* 0x004a05c0 */
void          __cdecl FUN_00423990(unsigned long arg1_unused,
                                        unsigned long *p_vec3,
                                        unsigned long scalar);
void          __cdecl FUN_004239c0(void);                      /* 0x00504080 */
void          __cdecl FUN_004239d0(void);                      /* 0x00504080 */
void          __cdecl FUN_004239e0(void);                      /* 0x00504080 */
void          __cdecl GLI_fn_SinusEffect_vSetState(unsigned char v);      /* 0x0050409c */
unsigned char __cdecl GLI_fn_SinusEffect_vGetState(void);                  /* 0x0050409c */
char          __cdecl GLI_fn_SinusEffect_cIsSecondViewportAffected(void);  /* 0x00504088 */

void          __cdecl GLI_fnv_SetDllName(const char *name); /* 0x00504564 */
void          __cdecl GLI_xClearViewingList(void);                     /* 0x00504848 */
void          __cdecl GLI_xSendListToViewport(unsigned long arg);      /* 0x00504870 */
void          __cdecl GLI_ClearZBufferRegion(long lXStart,long lXEnd,long lYStart,long lYEnd); /* 0x0050485c */
void          __cdecl FUN_00422000(int arg);
void          __cdecl FUN_004225d0(unsigned long *p_dst, unsigned long *p_src);
char          __cdecl GLI_cRegisterLensFlare(void *p_vRef);    /* 0x00428DE0 */
void          __cdecl GLI_vUnregisterLensFlare(void *p_vRef);  /* 0x00428E60 */

void          __cdecl FUN_0042a980(unsigned long arg, unsigned long);
int           __cdecl GLI_lGetVisualSetNumbertOfLOD(void *p);   /* 0x0042AF60 */
unsigned long __cdecl GLI_hGetRLI(void *p, unsigned int idx);   /* 0x0042B2B0 */

#define C_lMaxLightInViewport 300
struct GLD_tdstViewportAttributes_;
struct GLI_tdstLight_;
extern void *GLI_gs_aDEF_p_CurrentListOfLight[C_lMaxLightInViewport];
#define gs_CurrentNumberOfLightInViewport g_lActiveLightCount
void __cdecl GLI_vAddLightToViewport(struct GLD_tdstViewportAttributes_ *p_stVpt,
                                     struct GLI_tdstLight_ *p_stLight);  /* 0x00426B80 */
void __cdecl GLI_vResetTableOfActiveLightsForChar(char _cValue);          /* 0x00426A50 */
void __cdecl GLI_vSetLightActivityForChar(int iNum, unsigned char ucState); /* 0x00426A70 */
