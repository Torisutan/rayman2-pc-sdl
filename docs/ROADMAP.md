# Roadmap

Milestones of this fork. The detailed plan, its rules and measurements are in [PLAN.md](PLAN.md);
this page is the overview. Versions are targets, not promises; order can change when measurements
say so.

| Version | Milestone | Status |
|---|---|---|
| v0.1 | Modern Windows build, protection removed, DPI aware, widescreen | **done** |
| v0.2 | Cleanup and original bugs fixed | next |
| v0.3 | Solid codebase, portability inventory | planned |
| v0.4 | Own sound drivers (no retail sound DLLs) | planned |
| v0.5 | Platform layer and own renderer (no retail DLLs at all) | planned |
| v0.6 | Frame rate decoupled from game logic | planned |
| v1.0 | Windows, Linux and macOS | planned |
| later | Homebrew ports (PSP, Wii U) | open |
| later | Reading the Dreamcast level files | open |

## v0.1: modern build (done)
CMake build with current MSVC, copy protection removed at the source, `ubi.ini` from the game
folder, regression harness, DPI awareness, widescreen (Hor+ 3D, 4:3 UI). See
[CHANGELOG.md](../CHANGELOG.md).

## v0.2: cleanup and bug fixes (PLAN Phases 2-3)
- Exclude the remaining unreferenced protection code, then delete the excluded code.
  Exclusion done (2026-11-23); deletion still to do.
- clang-cl builds every file (inline asm it rejects, prototype mismatches).
  224 of 225 today; the last one (a clang crash) is a fix in the decompilation (PLAN Phase 1).
- Original engine bugs catalogued in `docs/BUGS.md` and fixed one per commit (`RAYMAN2_FIX_BUGS`).
  First pass done (2026-11-23): 6 fixed; open leads listed there.
- Faster loading: the loading screen waited for a synchronised screen flip at every progress step.
  Done (2026-11-23, `RAYMAN2_FAST_LOADING`): level loads 0.8-1.4 s -> 0.4 s.
- `ubi.ini` in a per-user location.
- Widescreen: test the windowed/fullscreen switch, window resizing and 21:9.

## v0.3: solid codebase (PLAN Phase 4)
- Warnings cleaned at a high level, static analysis, AddressSanitizer runs of the regression
  sequence.
- Remaining raw struct offsets and casts replaced by typed fields.
- `docs/PORTABILITY.md`: every endianness, pointer-size and packing assumption, where it is made.

## v0.4: sound (PLAN "Sound")
The game-side `SND` module stays. The retail driver DLLs are replaced by in-tree drivers behind the
same interface: `.bnm` bank reader, `.apm` ADPCM decoder, a software mixer, one output backend per
platform. Checked against the retail DLLs by comparing the mixed output of a scripted scene.

## v0.5: platform layer and renderer (PLAN Phase 5)
- Backends behind the engine's existing seams: renderer (`GLI_DRV`), input (`INO`), files (`FIL`),
  timers (`TMR`), plus window, events and threads (today in `GAM/pc_winmain.c`).
- An in-tree OpenGL / GLES renderer; no retail DLL needed any more.
- 64-bit x86 build.

## v0.6: frame rate decoupled from game logic
Today the engine runs one logic step per displayed frame: `fn_vActualizeEngineClock` measures the
frame time and `fn_ulDetermineUsefulDT` rounds it to a whole number of frame lengths (1 to 8), and
the flip waits for the display. Goal: the logic keeps the original step, and the display runs at
any refresh rate, drawing positions interpolated between the last two logic steps.
- The logic step stays as is, so the regression run is unchanged by construction.
- Inventory first: everything the draw path reads that moves per step (actor matrices, camera,
  particles, animations, texture and sinus effects) and what the step changes outside the
  hashed state.
- Display side: interpolate transforms per drawn frame, decide what is not interpolated (UI,
  cycling textures), and make the flip and frame pacing independent of the step.

## v1.0: desktop platforms
Windows, Linux and macOS from one source, each checked against its own regression baseline.

## Later: Dreamcast level files
The engine reads the PC release's level files. Goal: also load the levels of the Dreamcast
release, beside the PC ones. Inventory first: which files and structures differ from the PC
format (container, relocation, textures, sound banks), and whether each difference is read at
load time or converted offline. Nothing about the format is assumed until measured.

## Later: homebrew ports
PSP (memory budget to measure first) and Wii U (big-endian data: swap at load time or convert
offline).

## Open decisions
- Backend library: SDL3, SDL2 or native code per platform.
- Big-endian data: swap at load time or convert offline.
- License: none so far, same as upstream.
