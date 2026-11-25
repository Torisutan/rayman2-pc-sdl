#!/usr/bin/env python3
"""Build APMmxBVR.dll (the ADPCM sound driver: decodes .APM music and voices into voices of the WAV
driver) with the retail toolchain, and check it. Same build as tools/build_wavmw.py, for this DLL.

Toolchain: MSVC 5.0 SP3 cl (APMmxBVR.dll/tu.json base_flags: /O2 /Gy /MT, static C runtime), the MMX
decoder/mixer block (src/mmx/*.asm) assembled by ML 6.11d /coff, the version resource by VC5's rc,
LINK 5.10 /DLL at 0x10000000 against VC5's kernel32 (LIBCMT comes in through the objects). snddll.c is
WAVMWBVR.dll/src/snddll.c (one source, two DLLs; the differences come from APMmxBVR.dll/include, which
is searched before WAVMWBVR.dll/include). ML 6.11d is the assembler that matches the original: it writes
no @comp.id record (the original's Rich header names no assembler) and aligns sections to 16 bytes. It
knows no MMX instructions, so the sources write them as bytes (the instruction is in the comment), as
Intel's MMX macro include did for it. ML 6.11d ships in the Windows 98 DDK (bin/win98/ml.exe). Any later
ML also assembles the sources; only the Rich header then differs.

The link time stamp comes from the link environment, not from the sources: with --apm it is copied
from your original DLL after linking (only that field), so the build can be compared whole.

Paths (given once, then remembered in local.json):
  --vc5 DIR     MSVC 5.0 SP3 (bin/, include/, lib/, redist/)
  --ml611d EXE  ml.exe 6.11d
  --apm DLL     your game's APMmxBVR.dll (GOG install: DLL/APMmxBVR.dll), for --check and the time stamp

Output: build/APMmxBVR/APMmxBVR.dll and .map.
--check: every function of APMmxBVR.dll/status.json at its link address against the original DLL,
relocation sites (from the TU's .obj) masked, plus a whole-file comparison. Exit 1 if an EXACT row regressed.

Usage: python tools/build_apmmx.py [--vc5 DIR] [--ml611d EXE] [--apm DLL] [--check] [--out DIR]
"""
import os
import sys

sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
import build_wavmw  # noqa: E402

if __name__ == "__main__":
    build_wavmw.configure("APMmxBVR", "apm", "ml611d")
    sys.exit(build_wavmw.main(__doc__))
