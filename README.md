# rayman2-pc-sdl

A portable engine for **Rayman 2: The Great Escape**, built from the decompiled PC source.

This is a fork of [rayman2-pc-decomp](https://github.com/Torisutan/rayman2-pc-decomp), which
rebuilds the retail `Rayman2.exe` (GOG release) byte for byte with the original 1999 toolchain.
This fork takes that source further: it builds with current compilers, removes the copy
protection, fixes the original engine's bugs and adds modern display support. The long-term goal is
a multiplatform engine (Windows, Linux, macOS, later homebrew targets such as PSP and Wii U).

The original engine structure stays: module split (`GAM`, `GLI`, `SND`, `MMG`, ...), names, file
layout, data structures and coding style. Every departure is listed with its reason in
[docs/DEVIATIONS.md](docs/DEVIATIONS.md).

You need your own copy of the game. This repository contains no game data and no executables.

## Status (v0.1.1)

- Builds with current MSVC as a 32-bit Windows exe; runs the whole game with the retail game
  data and the install's renderer and sound DLLs.
- Copy protection removed at the source (no CD check, no tamper checks).
- DPI aware; widescreen with Hor+ 3D and 4:3 UI at any resolution (16:9, 16:10, ...).
- Original engine crashes fixed, faster loading, HUD at the screen edges at wide modes.
- Every change is measured with a regression harness: a fixed input sequence, the engine state
  hashed every frame, compared against a recorded baseline.

See [CHANGELOG.md](CHANGELOG.md) for the details and [docs/ROADMAP.md](docs/ROADMAP.md) for what
comes next.

## Building

Prebuilt: download `Rayman2.exe` from [Releases](https://github.com/Torisutan/rayman2-pc-sdl/releases)
(needs the Visual C++ 2015-2022 x86 redistributable). To build it yourself:

Requirements (not included):
- Visual Studio 2022 or later with the C++ x86 tools, CMake 3.20+, Python 3.
- Visual C++ 6.0, only for `DINPUT.LIB` (current SDKs ship only the DirectInput 8 library).

Create `local.json` in the repository root:

```json
{ "vc6": "C:\\path\\to\\VC6" }
```

Then:

```
cmake -S . -B build-cmake -A Win32
cmake --build build-cmake --config Release
```

Copy `build-cmake/Release/Rayman2.exe` into your game folder (keep a backup of the original). The
game reads `ubi.ini` from its own folder, as current installs ship it.

CMake options (all on by default):

| Option | Define | Effect |
|---|---|---|
| `R2_NO_PROTECTION` | `RAYMAN2_NO_PROTECTION` | Copy protection excluded |
| `R2_LOCAL_UBIINI` | `RAYMAN2_LOCAL_UBIINI` | `ubi.ini` from the game folder, not the Windows folder |
| `R2_WIDESCREEN` | `RAYMAN2_WIDESCREEN` | Hor+ 3D view and 4:3 UI at wide resolutions |
| `R2_REGRESS` | `RAYMAN2_REGRESS` | Regression harness (inactive unless started by `tools/regress.py`) |

The original-toolchain build of upstream (byte matching) is described in
[BUILDING.md](BUILDING.md).

## Tools

| Command | Purpose |
|---|---|
| `python tools/run.py EXE --level Morb_00 --shot 10` | Run the game in a sandbox (your install is never written), load a level, take screenshots |
| `python tools/regress.py run EXE OUT` | Regression run: fixed input, per-frame state hashes |
| `python tools/regress.py diff A B` | Compare two regression runs |
| `python tools/build_ref.py --define ...` | Original-toolchain build with the fork's defines (reference) |

## Documentation

- [docs/ROADMAP.md](docs/ROADMAP.md): milestones
- [docs/PLAN.md](docs/PLAN.md): the detailed plan, rules, measurements and open decisions
- [docs/DEVIATIONS.md](docs/DEVIATIONS.md): every departure from the original engine structure
- [docs/architecture.md](docs/architecture.md): engine structure, toolchain and link facts
- [BUILDING.md](BUILDING.md): the original-toolchain build and byte-matching tools (upstream)

## Credits

- [rayman2-pc-decomp](https://github.com/Torisutan/rayman2-pc-decomp): the decompilation this
  fork is built on.
- [Ray2Fix](https://github.com/spitfirex86/Ray2Fix) by Spitfire_x86 (MIT): the Hor+ formula and the
  full-width sprite rule of the widescreen support follow it.

## Legal

This repository contains no game data, executables or toolchain binaries. The prebuilt
`Rayman2.exe` on the Releases page is compiled from this source with the default options. It
contains no game data, assets or original Ubisoft binaries, and it runs only with your own copy of
the game (GOG release). Rayman is a trademark of Ubisoft. This project is not
affiliated with or endorsed by Ubisoft.
