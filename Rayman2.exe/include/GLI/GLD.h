#pragma once

typedef short   GLD_tdhDevice;
typedef short   GLD_tdhViewport;

void * __cdecl GLD_pGetViewport(short hDev, short hOldVP);

void * __cdecl GLD_pc_pGetDevice(short hDev);

/* 0x004211C0 */
long __cdecl GLD_pc_bRecreateDevice(short hDev, unsigned long *p_ulDevice);

long __cdecl GLD_bWriteToViewportFinished(short hDev_unused, short hVpPacked);

long __cdecl GLD_bClearDeviceWithChoice(short hDev_unused, short choice);
/* 0x004204b0 */
long __cdecl GLD_bClearViewportWithChoice(GLD_tdhDevice hDev_unused,
                                          GLD_tdhViewport hViewport_unused, short choice);

long __cdecl GLD_bClearDevice(short hDev);

/* 0x00421620 */
long __cdecl GLD_bSetBackgroundColorForDevice(GLD_tdhDevice hDev,
                                              unsigned long crRGBA);

long __cdecl GLD_bSwapToWindowedMode(short hDev, int bRebuildMode);

long __cdecl GLD_bSwapDeviceMode(short hDev, int bRebuildMode);

long __cdecl GLD_bReleaseViewportAndDevice(short hDev, short hVP);

long __cdecl GLD_bDeleteViewport(short hDev_unused, short hVpPacked);

long __cdecl GLD_bDeleteDevice(short hDev);

long __cdecl GLD_bCreateViewport(GLD_tdhDevice  hDev,
                                  unsigned long *p_ulViewAttribs,
                                  GLD_tdhViewport *p_hVP);

/* 0x00420a80 */
long __cdecl GLD_bGetViewportAttributes(GLD_tdhDevice hNotUsed, GLD_tdhViewport hVP, void *pstViewAttrib);

struct GLD_tdstDeviceAttributes_;
long __cdecl GLD_bCreateDevice(struct GLD_tdstDeviceAttributes_ *pstDevAttrib, short *p_hDev);

/* 0x00420630 */
long __cdecl GLD_pc_lFindFreeDeviceHandle(short *p_hDev);

/* 0x00420d90 */
void __cdecl GLD_pc_fn_vRestartFrameTimer(void);

long __cdecl GLD_pc_lComputeWaitFrameForSmoothSynchro(long lMode);
void __cdecl GLD_pc_vSimulateFlipSynchroWithoutVSync(long lFrameCount);

/* 0x00420360 */
long __cdecl GLD_pc_bFirstTimeInit(void);

long __cdecl GLD_bGiveSpecificAttributesToViewport(short hDev_unused,
                                                    short hVpPacked,
                                                    void          *p_vAttribs);

long __cdecl GLD_bRequestWriteToViewport(short hDev_unused,
                                          short hVpPacked,
                                          unsigned long *p_ulOutAttribs,
                                          unsigned long *p_ulOutCanWrite);

long __cdecl GLD_bWriteToViewportFinished2D(short hDev_unused,
                                             short hVpPacked);

long __cdecl GLD_bFlipDeviceWithSynchro(GLD_tdhDevice hDev_unused, int eSynchro_unused);

long __cdecl GLD_bRequestWriteToViewport2D(short hDev_unused,
                                            short hVpPacked,
                                            unsigned long *p_ulOutAttribs,
                                            long          *p_lOutCanWrite);

void __cdecl GLD_vGetFrontBufferIn24BitsAA(short hGLDDevice,
                                            short hGLDViewport,
                                            unsigned long lWidth,
                                            unsigned long lHeight,
                                            unsigned char *p_lBufferDst);

void __cdecl fn_vChangeViewPortPercent(unsigned long _ulMode,
                                            unsigned long _ulWhat,
                                            unsigned long _ulWidth,
                                            unsigned long _ulHeight,
                                            unsigned long _ulClipTop,
                                            unsigned long _ulClipLeft,
                                            unsigned long _ulClipBottom,
                                            unsigned long _ulClipRight,
                                            int           _iPosX,
                                            int           _iPosY);
