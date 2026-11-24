#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#ifndef GLI_DX6_FIXES

#define GLI_DiagEnabled() 0
#define GLI_DiagVerbose() 0
#define GLI_DiagNextFrame() ((void)0)
#define GLI_DiagLog (void)sizeof
#define GLI_M_DiagCountSend() ((void)0)
#define GLI_M_DiagResetSends() ((void)0)

#else

#define GLI_M_DiagCountSend() (++GLI_g_DiagSendCount)
#define GLI_M_DiagResetSends() (GLI_g_DiagSendCount = 0)

int GLI_DiagEnabled(void);

int GLI_DiagVerbose(void);

void GLI_DiagLog(const char *fmt, ...);

unsigned long GLI_DiagNextFrame(void);

#endif

extern unsigned long GLI_g_DiagSendCount;

#ifdef __cplusplus
}
#endif
