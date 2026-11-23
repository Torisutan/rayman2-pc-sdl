# rayman2-pc-sdl: plan

This fork turns the byte-matching decompilation (upstream:
[rayman2-pc-decomp](https://github.com/Torisutan/rayman2-pc-decomp)) into a portable,
maintainable game engine. The long-term goal is multiplatform: PC (Windows, Linux, macOS) and
homebrew targets such as PSP and Wii U. You need your own copy of the game data. None is included.

## Rules

1. **Keep the original engine structure.** Keep the module split (`GAM`, `GLI`, `SND`, `MMG`, ...),
   the function and global names, the file layout and the data structures. Deviate only when
   there is no other way. Each deviation gets a comment saying why, and an entry in
   [DEVIATIONS.md](DEVIATIONS.md).
2. **Stay mergeable with upstream.** Fetch upstream with `git fetch upstream && git merge
   upstream/main`. Keep changes inside upstream files small and local. New code goes in new files.
3. **One concern per commit.** Bug fixes, protection removal and portability changes are separate
   commits, so each one can be reviewed and reverted on its own.
4. **Measure before and after.** Every phase ends with the regression run (below) unchanged, except
   where the phase intends a change.

## Phases

### Phase 1: modern toolchain, no behaviour change
- A CMake build with current MSVC (and clang-cl), **32-bit x86**. The level files store 32-bit
  pointers that the loader rebases (`SNA_pc_vLoadAndRelocate`), and every intended target (PSP,
  Wii U) is 32-bit too, so 32-bit stays the model throughout.
- First build the sources as they are. Current 32-bit MSVC still accepts inline asm, naked
  functions and `_emit`, so only real incompatibilities change at this step (headers, shims,
  declarations).
- Inline asm must go eventually, because no other target compiler accepts the MSVC dialect. It
  goes in later phases, where its context disappears:
  - `__declspec(naked)` gates and `_emit` NOP runs, which are protection code: Phase 2;
  - inline-asm math helpers and `#pragma optimize` frame tricks, which become plain C: Phase 4.
- The game still uses the retail renderer and sound DLLs at this stage.
- **Status.** `CMakeLists.txt` builds with MSVC 19.51 (no source edits; 136 warnings at the default
  level, cleaned in Phase 4). Without the crack it behaves like the VC5 build: alive, no window.
  clang-cl is selectable (see the head of `CMakeLists.txt`), but 32 TUs do not compile yet: inline
  asm clang rejects (labels and gotos across asm blocks, C statements in naked functions), mostly
  in the protection files; about 60 prototype mismatches between headers and definitions that
  MSVC only warns about (C4028); a few argument-count and type-name errors.
- **Crack behaviour (decided).** The post-link crack (`tools/crack.py`) only partly translates
  to modern codegen (21 of 29 patches; the result crashes). Instead Phase 2 comes first: the
  protection is removed, not emulated, and excluded from the build with `#ifndef
  RAYMAN2_NO_PROTECTION`, compiled into BOTH builds. Excluding instead of deleting keeps upstream
  merges clean; the code is deleted later. Measured chain: VC5 cracked = VC5 + define on every
  game field (protection state such as the LCG leaves the hash), then VC5 + define = modern +
  define.
- **Regression harness.** `Rayman2.exe/src/port/regress.c` (`RAYMAN2_REGRESS`), `tools/regress.py`,
  `tools/build_ref.py`. Two runs of the VC5 cracked build on Morb_00 (1860 frames) are identical.
- **Regression run:** load a fixed level, play a recorded input sequence for N frames, and hash
  the engine state each frame (engine structure, main character position, random state). The
  upstream exe produces the reference hashes. This harness is the first deliverable, because every
  later phase relies on it.

### Phase 2: remove the copy protection
Remove it at the source, not by patching (upstream `docs/architecture.md`, section 2, lists every
check):
- the private import resolver and the LCG code-pool stubs (`MMG_init.c`, `SNA` file I/O, which
  become plain file calls);
- the CD check and the CD path for `LEVELS0.DAT` (`FUN_00448e30`, `SNA_pc_vVerifyCdLevelsFile`);
- the level-load tamper block (`MMG/pc_protect.c`) and the AI integrity probes
  (`fn_p_stMiscProtectProcedure`, `fn_p_stComputeXor`, `AI_pc_fn_p_stComputeNotU`). The protection
  actors in the levels must still see "OK" values, otherwise they show "CD missing";
- the relocation-table rekeying and the checksum bit-flip. Level relocation tables are read
  either directly from `LEVELS0.DAT` with the selector math kept as plain data access, or from
  tables extracted once to loose files. **Open decision.**
- `ubi.ini` moves out of the Windows directory into a per-user config location.

### Phase 3: fix the original engine's bugs
- Keep a catalogue in `docs/BUGS.md`: symptom, cause, function, fix. Known candidates from the
  decompilation include uninitialised reads (for example `bEnabledWrite` in `GAM_fn_vEngine`,
  which is read before `GLD_bRequestWriteToViewport` sets it) and the ini parser's skipped-rate
  path.
- One fix per commit, each marked `/* FIX(BUGS.md#n) */` at the site.

### Phase 4: make the codebase solid
- Clean warnings at a high level (`/W4`, `-Wall -Wextra`), static analysis, and AddressSanitizer
  runs of the regression sequence.
- Replace the remaining raw struct offsets and casts with typed fields.
- Document every assumption about endianness, pointer size and struct packing at the place it is
  made (`docs/PORTABILITY.md`). Phase 5 works from this list.

### Phase 5 (later): platform layer and ports
The engine already has seams at the right places: the renderer function table (`GLI_DRV`), input
devices (`INO`), the sound driver interface (`SND`), files (`FIL`) and timers (`TMR`). The platform
layer puts backends behind those seams instead of adding a new layer on top, which keeps the
engine structure intact. What is still missing is window/event handling and thread/sync
primitives, which today live in `GAM/pc_winmain.c`.
- **Backend library:** SDL3, SDL2 or per-platform native code. **Open decision.** SDL covers the
  PCs, and community SDL ports exist for PSP and Wii U.
- **Renderer:** a new in-tree driver behind `GLI_DRV`: OpenGL / GLES first, later GU (PSP) and
  GX2 (Wii U).
- **Big-endian targets (Wii U):** the data files are little-endian. The choices are byte-swapping
  at load time, which needs type information for every relocated block, or an offline data
  converter. **Open decision.** Decide it when Phase 4's portability list exists.
- **Memory:** PSP has 32-64 MB in total. Measure MMG's block budgets per level before
  committing to that port.

## Sound

**Today.** The `SND` module (in the exe) manages events, banks, 3D positioning (`sndspace`,
`sndvect`), themes and sequences. It loads driver DLLs from the game's `DLL` folder at run time,
and they call back into the ~110 functions the exe exports. The data is `.bnm` banks and `.apm`
streams (ADPCM, used for music and voices).

**Plan:**
1. **Keep the `SND` module as it is.** It is decompiled, almost all of it byte-exact, and it already
   does the game-side logic, including 3D attenuation and panning.
2. **Replace the DLL boundary with in-tree drivers** that implement the same interface the DLLs
   register through the exports. The first step is to document that interface (exports, call
   order, buffer formats) in `docs/SOUND.md`, by observing the retail DLLs. Drivers:
   - a `.bnm` bank reader and an `.apm` ADPCM decoder, written from the file formats;
   - a software mixer: voices with volume, pan, pitch and loop, streaming for `.apm`;
   - one output backend per platform behind the platform layer (SDL audio or miniaudio on PC,
     native audio on the consoles).
3. **Order:** after Phase 4 and before the renderer. It is smaller, and it removes the first
   retail DLL dependency. Check it against the retail DLLs by capturing the mixed output of a
   scripted scene from both.

## Open decisions
- Backend library: SDL3, SDL2 or native code.
- Level relocation tables: read from `LEVELS0.DAT` or extract them once.
- Big-endian data: swap at load time or convert offline.
- License: none so far, same as upstream.
