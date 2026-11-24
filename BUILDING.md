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
