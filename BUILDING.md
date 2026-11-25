# Building

## Requirements (not included)

- **MSVC 5.0 SP3** (cl 11.00.7022, LINK 5.10), laid out as `bin/`, `include/`, `lib/`, `redist/`.
- **Visual C++ 6.0** (`VC98/`). Provides `DINPUT.LIB` and `DXGUID.LIB`, which VC5 lacks, and the
  compiler for the few TUs that `tu.json` marks `vc6-8168`.
- **Your own GOG `Rayman2.exe`** (MD5 `ef4954015d8b67e3534c9dc40ca3ad6e`). It supplies the resources
  for the link, the original code image for byte comparison, and the bytes for the crack step.
- Python 3 with `capstone` (`pip install capstone`), on Windows (the toolchain is Win32).

The three paths are given once and then remembered in `local.json`.

## Build

```
python tools/build.py --vc5 <MSVC5 dir> --vc6 <VC6 dir> --gog <path to GOG Rayman2.exe> --crack
```

| Output | Contents |
|---|---|
| `build/Rayman2.exe` | The original image, copy protection included (the byte-match target) |
| `build/Rayman2_cracked.exe` | The same plus the GOG crack's behaviour (`--crack`). Copy it into the game folder to run it |
| `build/Rayman2.map` | LINK map |

## Byte matching

```
python tools/check.py                          # every function of status.json in the linked exe
python tools/byte_match.py NAME                # compile one function's TU, diff it against retail
python tools/byte_match.py NAME --asm          # side-by-side disassembly, retail | ours
python tools/byte_match.py NAME --flags G6     # try other compiler flags for the TU
```

`check.py` reports regressions of byte-exact functions and functions that became byte-exact.
Relocated operands (calls, globals) are masked; they are compared by address only once the whole
layout matches.

The crack step (`tools/crack.py`) reproduces each GOG crack patch at the owning function in our
layout. It reads every byte it copies from your GOG exe at run time.

## GliDX6vf.dll

The Direct3D 6 renderer (`DLL/GliDX6vf.dll` in the game folder) is built on its own, with the same
MSVC 5.0 SP3 compiler and LINK 5.10 (`/O2 /Gy /MD`, `/DLL`, base `0x10000000`). Its `ddraw.lib` comes
from the DirectX 6 SDK; `tools/build_glidx6.py` rebuilds that library from VC6's `DDRAW.LIB`, so it
needs `--vc6` too.

```
python tools/build_glidx6.py --vc5 <MSVC5 dir> --vc6 <VC6 dir> --gli <game>/DLL/GliDX6vf.dll --check
```

| Output | Contents |
|---|---|
| `build/GliDX6vf/GliDX6vf.dll` | The DLL. Copy it over `DLL/GliDX6vf.dll` in the game folder (keep a backup) to run it |
| `build/GliDX6vf/GliDX6vf.map` | LINK map |

`--check` compares every function of `GliDX6vf.dll/status.json` at its link address with your
`GliDX6vf.dll`, relocated operands masked, and reports regressions of byte-exact functions.

The default build is the original driver. Code under `#ifdef GLI_DX6_FIXES` holds fixes (Windows 10
window handling, NULL guards, diagnostics) that are not part of the original; the byte-match build
leaves `GLI_DX6_FIXES` undefined.

## WAVMWBVR.dll

The sound driver (`DLL/WAVMWBVR.dll`) uses the same MSVC 5.0 SP3 compiler with the static C runtime
(`/O2 /Gy /MT`) and LINK 5.10 (`/DLL /DEBUG`, base `0x10000000`). Its MMX mixer is MASM source: it needs
`ml.exe` 6.1x (Visual C++ 6.0 SP5 or the Processor Pack ship it as `VC98/Bin/ml.exe`), which assembles
to OMF objects that LINK converts, as in the original build.

```
python tools/build_wavmw.py --vc5 <MSVC5 dir> --ml <path to ml.exe> --wav <game>/DLL/WAVMWBVR.dll --check
```

| Output | Contents |
|---|---|
| `build/WAVMWBVR/WAVMWBVR.DLL` | The DLL. Copy it over `DLL/WAVMWBVR.dll` in the game folder (keep a backup) to run it |
| `build/WAVMWBVR/WAVMWBVR.map` | LINK map |

With `--wav`, the fields the linker takes from the build machine (link time stamp, PDB path, output
path in the debug directory) are copied from your DLL after linking; nothing else is. `--check`
compares every function of `WAVMWBVR.dll/status.json` at its link address with your DLL, relocated
operands masked, reports regressions of byte-exact functions and counts the differing bytes of the
whole file.

## APMmxBVR.dll

The ADPCM sound driver (`DLL/APMmxBVR.dll`) builds the same way (`/O2 /Gy /MT`, LINK 5.10 `/DLL`
without debug information, base `0x10000000`); its MMX block is MASM source assembled with
`ml.exe` **6.11d** `/coff` (it ships in the Windows 98 DDK as `bin/win98/ml.exe`). 6.11d knows no MMX
instructions, so the sources write them as bytes with the instruction in the comment. It compiles
`WAVMWBVR.dll/src/snddll.c` too, with `APMmxBVR.dll/include` searched before `WAVMWBVR.dll/include`.

```
python tools/build_apmmx.py --vc5 <MSVC5 dir> --ml611d <path to ml.exe 6.11d> --apm <game>/DLL/APMmxBVR.dll --check
```

| Output | Contents |
|---|---|
| `build/APMmxBVR/APMmxBVR.dll` | The DLL. Copy it over `DLL/APMmxBVR.dll` in the game folder (keep a backup) to run it |
| `build/APMmxBVR/APMmxBVR.map` | LINK map |

With `--apm`, the link time stamp is copied from your DLL after linking; nothing else is. ML 6.11d is
the assembler that matches the original: it does not record itself in the objects (the original's Rich
header, the linker's list of the tools that built the objects, names no assembler) and aligns their
sections to 16 bytes. A later `ml.exe` assembles the same sources, but adds itself to the Rich header.
`--check` works as for `WAVMWBVR.dll`.
