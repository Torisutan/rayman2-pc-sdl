#include "GliDX6vf/GliDX6vf.h"
#include "GliDX6vf/GliDX6vf_State.h"

#include <math.h>

#define GLI_DDCAPS_DWCAPS_OFFSET 0x4
#define GLI_DDCAPS_DWCAPS2_OFFSET 0x8
#define GLI_DDCAPS_DWPALCAPS_OFFSET 0x18
#define GLI_DDCAPS_DDSCAPS_DWCAPS2_OFFSET 0x16C

#define GLI_DDCAPS_3D 0x00000001u
#define GLI_DDCAPS_PALETTE 0x00008000u
#define GLI_DDCAPS2_NONLOCALVIDMEM 0x00000200u
#define GLI_DDCAPS2_CANRENDERWINDOWED 0x00080000u
#define GLI_DDPCAPS_8BIT 0x00000004u
#define GLI_DDPCAPS_ALLOW256 0x00000040u
#define GLI_DDSCAPS2_TEXTUREMANAGE 0x00400000u
#define GLI_DDWAITVB_BLOCKBEGIN 0x00000001u

#define GLI_CAPS_FLAGS_OFFSET 0x4
#define GLI_CAPS_FEATURES_OFFSET 0x8
#define GLI_CAPS_REFRESH_OFFSET 0x28

#define GLI_TIMER_HIGH_FREQUENCY 1
#define GLI_REFRESH_SAMPLE_COUNT 0x46
#define GLI_REFRESH_SUM_START_INDEX 5
#define GLI_REFRESH_SUM_COUNT 0x3c

#define GLI_DWORD_AT(base, offset) (*(DWORD *)((unsigned char *)(base) + (offset)))

HRESULT __cdecl GLI_ReadDirectDrawCaps(void *gliCaps, DWORD capsSelector, void *directDraw4)
{
    GLI_IDirectDraw4 *ddraw;
    GLI_DDCAPS_DX6 helCaps;
    GLI_DDCAPS_DX6 halCaps;
    GLI_DDCAPS_DX6 *selectedCaps;
    HRESULT result;
    DWORD caps;
    DWORD caps2;

    ddraw = (GLI_IDirectDraw4 *)directDraw4;
    ZeroMemory(&helCaps, sizeof(helCaps));
    ZeroMemory(&halCaps, sizeof(halCaps));
    helCaps.dwSize = GLI_DDCAPS_DX6_SIZE;
    halCaps.dwSize = GLI_DDCAPS_DX6_SIZE;

    result = ddraw->lpVtbl->GetCaps(ddraw, &halCaps, &helCaps);
    if (result != 0) {
        char message[0x200];

        FormatMessageA(FORMAT_MESSAGE_FROM_SYSTEM, NULL, result, 0, message, sizeof(message), NULL);
        return result;
    }

    selectedCaps = (capsSelector & 1) ? &halCaps : &helCaps;
    caps = GLI_DWORD_AT(selectedCaps, GLI_DDCAPS_DWCAPS_OFFSET);

    if ((caps & GLI_DDCAPS_3D) != 0) {
        GLI_DWORD_AT(gliCaps, GLI_CAPS_FLAGS_OFFSET) |= 0x1;
    }
    if (((caps & GLI_DDCAPS_PALETTE) != 0) &&
        ((GLI_DWORD_AT(selectedCaps, GLI_DDCAPS_DWPALCAPS_OFFSET) & GLI_DDPCAPS_8BIT) != 0) &&
        ((GLI_DWORD_AT(selectedCaps, GLI_DDCAPS_DWPALCAPS_OFFSET) & GLI_DDPCAPS_ALLOW256) != 0)) {
        GLI_DWORD_AT(gliCaps, GLI_CAPS_FEATURES_OFFSET) |= 0x1;
    }
    caps2 = GLI_DWORD_AT(selectedCaps, GLI_DDCAPS_DWCAPS2_OFFSET);
    if ((caps2 & GLI_DDCAPS2_NONLOCALVIDMEM) != 0) {
        GLI_DWORD_AT(gliCaps, GLI_CAPS_FLAGS_OFFSET) |= 0x2;
    }
    if ((caps2 & GLI_DDCAPS2_CANRENDERWINDOWED) != 0) {
        GLI_DWORD_AT(gliCaps, GLI_CAPS_FLAGS_OFFSET) |= 0x28;
    }
    if ((GLI_DWORD_AT(selectedCaps, GLI_DDCAPS_DDSCAPS_DWCAPS2_OFFSET) &
         GLI_DDSCAPS2_TEXTUREMANAGE) != 0) {
        GLI_DWORD_AT(gliCaps, GLI_CAPS_FEATURES_OFFSET) |= 0x4;
    }

    if (GLI_g_TimerGetCounter == NULL || GLI_g_TimerTickPerSecond == NULL) {
        GLI_DWORD_AT(gliCaps, GLI_CAPS_REFRESH_OFFSET) = 0x42700000u;
        return 0;
    }

    {
        unsigned int samples[GLI_REFRESH_SAMPLE_COUNT];
        LARGE_INTEGER previousCounter;
        DWORD sampleIndex;
        DWORD sum;
        unsigned long ticksPerSecond;

        result = ddraw->lpVtbl->WaitForVerticalBlank(ddraw, GLI_DDWAITVB_BLOCKBEGIN, NULL);
        if ((LONG)result < 0) {
            GLI_DWORD_AT(gliCaps, GLI_CAPS_REFRESH_OFFSET) = 0;
            return result;
        }

        ((GLI_TimerGetCounterFunction)GLI_g_TimerGetCounter)(GLI_TIMER_HIGH_FREQUENCY, &previousCounter);

        for (sampleIndex = 0; sampleIndex < GLI_REFRESH_SAMPLE_COUNT; ++sampleIndex) {
            LARGE_INTEGER currentCounter;

            result = ddraw->lpVtbl->WaitForVerticalBlank(ddraw, GLI_DDWAITVB_BLOCKBEGIN, NULL);
            if ((LONG)result < 0) {
                break;
            }

            ((GLI_TimerGetCounterFunction)GLI_g_TimerGetCounter)(GLI_TIMER_HIGH_FREQUENCY, &currentCounter);
            samples[sampleIndex] = currentCounter.LowPart - previousCounter.LowPart;
            previousCounter = currentCounter;
        }

        if (sampleIndex < GLI_REFRESH_SAMPLE_COUNT) {
            GLI_DWORD_AT(gliCaps, GLI_CAPS_REFRESH_OFFSET) = 0;
            return result;
        }

        GLI_vSortValuesAscending(samples, GLI_REFRESH_SAMPLE_COUNT);
        sum = 0;
        for (sampleIndex = 0; sampleIndex < GLI_REFRESH_SUM_COUNT; ++sampleIndex) {
            sum += samples[GLI_REFRESH_SUM_START_INDEX + sampleIndex];
        }

        ticksPerSecond = ((GLI_TimerTickPerSecondFunction)GLI_g_TimerTickPerSecond)(GLI_TIMER_HIGH_FREQUENCY);
        *(float *)((unsigned char *)gliCaps + GLI_CAPS_REFRESH_OFFSET) =
            (float)floor(0.5 - (-60.0 / ((double)sum / (double)ticksPerSecond)));
    }

    return result;
}
