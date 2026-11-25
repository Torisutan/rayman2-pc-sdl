#pragma once

#define GAM_pc_M_ulWaitDrawSemaphore(hSemaphore)   WaitForSingleObject((hSemaphore), 0xFFFFFFFF)
#define GAM_pc_M_bReleaseDrawSemaphore(hSemaphore) ReleaseSemaphore((hSemaphore), 1, 0)
