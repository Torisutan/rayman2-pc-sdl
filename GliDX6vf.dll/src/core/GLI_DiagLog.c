#include "GliDX6vf/GLI_DiagLog.h"

#ifdef GLI_DX6_FIXES

#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <stdio.h>
#include <stdarg.h>

unsigned long GLI_g_DiagSendCount = 0;

static int s_state = 0;
static int s_verbose = 0;
static HANDLE s_hFile = INVALID_HANDLE_VALUE;
static unsigned long s_frame = 0;

static void gli_diag_exe_path(char *out, const char *name)
{
    char exe[MAX_PATH];
    DWORD n;
    DWORD i;
    DWORD dirLen = 0;

    n = GetModuleFileNameA(NULL, exe, sizeof(exe));
    if (n == 0 || n >= sizeof(exe)) {
        lstrcpynA(out, name, MAX_PATH);
        return;
    }

    for (i = 0; i < n; ++i) {
        if (exe[i] == '\\' || exe[i] == '/') {
            dirLen = i + 1;
        }
    }

    if (dirLen == 0 || dirLen + (DWORD)lstrlenA(name) >= (DWORD)MAX_PATH) {
        lstrcpynA(out, name, MAX_PATH);
        return;
    }

    memcpy(out, exe, dirLen);
    lstrcpyA(out + dirLen, name);
}

int GLI_DiagEnabled(void)
{
    if (s_state == 0) {
        int enabled = 0;
        char buf[8];
        char sentinel[MAX_PATH];

        if (GetEnvironmentVariableA("GLIDX6VF_DIAG", buf, sizeof(buf)) > 0) {
            enabled = 1;
        } else {
            gli_diag_exe_path(sentinel, "glidx6vf_diag.on");
            if (GetFileAttributesA(sentinel) != INVALID_FILE_ATTRIBUTES) {
                enabled = 1;
            }
        }
        s_state = enabled ? 1 : 2;
    }
    return s_state == 1;
}

int GLI_DiagVerbose(void)
{
    if (s_verbose == 0) {
        int on = 0;
        char buf[8];
        char sentinel[MAX_PATH];

        if (!GLI_DiagEnabled()) {
            return 0;
        }
        if (GetEnvironmentVariableA("GLIDX6VF_DIAG_VERBOSE", buf, sizeof(buf)) > 0) {
            on = 1;
        } else {
            gli_diag_exe_path(sentinel, "glidx6vf_diag_verbose.on");
            if (GetFileAttributesA(sentinel) != INVALID_FILE_ATTRIBUTES) {
                on = 1;
            }
        }
        s_verbose = on ? 1 : 2;
    }
    return s_verbose == 1;
}

void GLI_DiagLog(const char *fmt, ...)
{
    va_list ap;
    char line[1024];
    int prefixLen;
    int bodyLen;
    int total;
    DWORD written;

    if (!GLI_DiagEnabled()) {
        return;
    }

    if (s_hFile == INVALID_HANDLE_VALUE) {
        char path[MAX_PATH];
        static const char header[] = "=== GliDX6vf_reimpl standalone diagnostic log ===\r\n";

        gli_diag_exe_path(path, "glidx6vf_reimpl_diag.log");
        s_hFile = CreateFileA(path, GENERIC_WRITE, FILE_SHARE_READ, NULL,
                              CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
        if (s_hFile == INVALID_HANDLE_VALUE) {
            s_state = 2;
            return;
        }
        WriteFile(s_hFile, header, (DWORD)(sizeof(header) - 1), &written, NULL);
    }

    prefixLen = snprintf(line, sizeof(line), "[%8lu ms f%-4lu] ",
                         (unsigned long)GetTickCount(), s_frame);
    if (prefixLen < 0 || prefixLen >= (int)sizeof(line)) {
        prefixLen = 0;
    }

    va_start(ap, fmt);
    bodyLen = vsnprintf(line + prefixLen, sizeof(line) - (size_t)prefixLen - 2, fmt, ap);
    va_end(ap);
    if (bodyLen < 0) {
        bodyLen = 0;
    }

    total = prefixLen + bodyLen;
    if (total > (int)sizeof(line) - 2) {
        total = (int)sizeof(line) - 2;
    }
    line[total]     = '\r';
    line[total + 1] = '\n';

    WriteFile(s_hFile, line, (DWORD)(total + 2), &written, NULL);
}

unsigned long GLI_DiagNextFrame(void)
{
    return ++s_frame;
}

#endif
