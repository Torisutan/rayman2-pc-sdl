# Changelog

All notable changes of this fork. Upstream's decompilation progress is not listed here; it arrives
through regular merges of [rayman2-pc-decomp](https://github.com/Torisutan/rayman2-pc-decomp).

## Unreleased

### Game
- **Original engine bugs fixed** (`RAYMAN2_FIX_BUGS`, CMake `R2_FIX_BUGS`, on): crashes and
  undefined behaviour of the retail engine, gameplay unchanged. Catalogue and evidence in
  [docs/BUGS.md](docs/BUGS.md); fixed so far: #1-#6 (uninitialised reads in the engine loop,
  sphere culling and magnets, NULL writes in the AbsoluteAxis AI fields, an out-of-bounds index on
  an empty temp path, a read after free at exit).
- **Faster loading** (`RAYMAN2_FAST_LOADING`, CMake `R2_FAST_LOADING`, on): the loading screen
  no longer waits for a synchronised screen flip at every progress step; the bar is refreshed at
  most every 100 ms. Level loads take about 0.4 s instead of 0.8-1.4 s; image and bar stay.

## v0.1.0 (2026-11-23)

First release: the decompiled engine as a modern 32-bit Windows build, without copy protection,
with DPI awareness and widescreen. Source only; bring your own game.

### Build
- CMake build with current MSVC (and clang-cl as a second compiler, not complete yet), 32-bit x86.
  Translation units and link order come from upstream's `tu.json`, so new upstream files build
  without edits.
- The GOG exe's resources (icons, DPI-aware manifest) are linked into the build; the window is no
  longer bitmap-scaled by Windows.
- `tools/build_ref.py`: original-toolchain reference build with the fork's defines.

### Game
- **Copy protection removed at the source** (`RAYMAN2_NO_PROTECTION`): no CD check, no hidden
  import resolver, no code executed from level data, no tamper checks in the scripts; level
  relocation tables are read from the first copy in `LEVELS0.DAT`. DEVIATIONS 2-6.
- **`ubi.ini` from the game folder** (`RAYMAN2_LOCAL_UBIINI`) instead of the Windows folder, as
  current installs ship it. DEVIATIONS 1.
- **Widescreen** (`RAYMAN2_WIDESCREEN`). The ratio is computed from the resolution, nothing to
  configure:
  - 3D view Hor+: a wider screen sees more on the sides, the vertical view stays as at 4:3;
  - text, health bar, icons, logos and the loading screen are drawn in a centred 4:3 band;
  - HUD, menu cursor and menu objects (3D objects drawn over the screen) keep their 4:3 size and
    place;
  - 4:3 and taller resolutions (5:4) are unchanged.
  DEVIATIONS 7-9.

### Testing
- Regression harness (`RAYMAN2_REGRESS`, `tools/regress.py`): fixed timing, fixed random seeds and
  a scripted input sequence on a level; the engine state is hashed every frame and compared with a
  recorded baseline. Runs at any display mode (`--mode WxH`).
- `tools/run.py`: runs the game in a sandbox that never writes into your install; direct level
  load, screenshots, timed key presses, display mode.

### Known limitations
- Still needs the install's renderer and sound DLLs and runs on Windows only.
- The modern build's floating-point results differ slightly from the 1999 build (one matrix field
  in the regression hash, from intermediate rounding); every other hashed field matches.
- Widescreen: the windowed/fullscreen switch, window resizing and 21:9 are not tested yet.
