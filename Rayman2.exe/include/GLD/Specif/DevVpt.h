#pragma once

#include "GLD/GLD_Pub.h"

typedef struct GLD_stViewport
{
    struct GLD_tdstViewportAttributes_ stViewAttrib;
    BOOL                                bIsLocked;
} GLD_tdstViewport, *GLD_tdpstViewport;

typedef struct GLD_tdstDeviceAttributes_
{
    unsigned long dwHeight;
    unsigned long dwWidth;
    float         xInvPixelDimensionX;
    float         xInvPixelDimensionY;
    char         *p_cVirtualScreen;
    long          lPitch;
    short        *p_sZBuffer;
    unsigned long dwFullScreenModeX;
    unsigned long dwFullScreenModeY;
    unsigned long dwFullScreenModeBpp;
    HWND          hFullScreenModeWnd;
    HWND          hNormalModeWnd;
    BOOL          bFullScreen;
    unsigned int  uiTypeDriver;
    POINT         pClientOnPrimary;
    RECT          rtClientRect;
    RECT          rtWindowRect;
} GLD_tdstDeviceAttributes;

typedef struct GLD_stDevice
{
    struct GLD_tdstDeviceAttributes_ stDevAttrib;
    struct GLD_stViewport           *pstViewports[16];
    int                              lWriteInProgress;
} GLD_tdstDevice, *GLD_tdpstDevice;
