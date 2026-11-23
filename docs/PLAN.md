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
5. **Write in the original engine's style.** Names (module prefix, type-prefixed identifiers such
   as `ul`, `p_st`, `fn_v`), `/* */` comments, and its way of fencing platform code: platform- or
   compiler-specific code sits under a preprocessor condition, next to a branch for the other
   targets, and every `#endif` names its condition (`#endif /* NAME */`). New targets (x64, PSP,
   Wii U, ...) get another branch beside the existing code. Do not rewrite the existing branch.
   This applies to every edit, not only to protection removal.

## Phases

### Phase 1: modern toolchain, no behaviour change
- A CMake build with current MSVC (and clang-cl), **32-bit x86**. The level files store 32-bit
  pointers that the loader rebases (`SNA_pc_vLoadAndRelocate`), and every intended target (PSP,
  Wii U) is 32-bit too, so 32-bit stays the model throughout.
- First build the sources as they are. Current 32-bit MSVC still accepts inline asm, naked
  functions and `_emit`, so only real incompatibilities change at this step (headers, shims,
  declarations).
- No other target compiler accepts the MSVC inline-asm dialect, so the asm leaves the other
  targets' builds in two ways:
  - `__declspec(naked)` gates and `_emit` NOP runs are protection code: they are excluded with
    the protection in Phase 2;
  - inline-asm math helpers and `#pragma optimize` frame tricks stay as they are for x86 MSVC,
    under a platform condition (rule 5). A portable C branch goes beside them for the other
    targets: Phase 4.
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
check). Excluded with `#ifndef RAYMAN2_NO_PROTECTION` (CMake option `R2_NO_PROTECTION`, on;
VC5 reference: `tools/build_ref.py --define RAYMAN2_NO_PROTECTION --define RAYMAN2_LOCAL_UBIINI`).
Each removed mechanism has a row in [DEVIATIONS.md](DEVIATIONS.md) (2-6).
- **Done:** the CD check; the private import resolver, LCG code pool and call stubs (the file
  layer calls Win32 directly); the level-load tamper block that runs code from level data; the
  random relocation-table copy selection, decoy reads, key/state recording and checksum
  bit-flip; the AI integrity probes.
- **Decided: protection actors are inert.** Actors flagged `GAM_C_CustBitProtection` do not think,
  and the AI functions of the PC protection block flag their caller. Their checks compare against
  the original code image, which a rebuilt engine cannot reproduce, and no fixed "OK" values are
  substituted.
- **Decided: relocation tables** are decoded from `LEVELS0.DAT`, always from the first copy (load
  key byte 2 = 0). No extracted files.
- **Measured (Morb_00, 1860 frames).** VC5 + define against VC5 cracked: every hashed field is
  identical except `rnd.index` from frame 708, because the protection actors no longer draw from
  the shared random generator. Reading copy 0 instead of random copies changes nothing.
  Modern MSVC + define against VC5 + define: only `actor.matrix` differs, from frame 64: float
  intermediate precision, not a bug. See open decisions.
- **Decided: regression reference.** With the protection actors inert, `rnd.index` no longer
  follows the original from the first draw they would have made. From now on the reference is
  VC5 + define (`tools/build_ref.py --define RAYMAN2_NO_PROTECTION --define
  RAYMAN2_LOCAL_UBIINI`), not VC5 cracked.
- `ubi.ini` is read from the working directory (`RAYMAN2_LOCAL_UBIINI`, DEVIATIONS 1), as the
  current installs ship it. Moving it to a per-user config location is still to do.
- **Still compiled but unreferenced:** the resolver, pool and gate definitions in
  `MMG/MMG_init.c`, the naked `SNA_g_lProtectionGate` gates, `MMG_pc_g_a_stImport`. Exclude them
  too (clang-cl rejects most of that asm), then delete the excluded code later.

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
- Modern build regression reference. **Measured cause** of the `actor.matrix` difference (Morb_00
  frame 64): `POS_fn_vNormalizeMatrix` (from `MEC_fn_vCorrectMatrix`) gets bit-identical input in
  both exes. VC5 keeps the sum of squares, the square root and `inv_norm` on the x87 stack (control
  word 0x027F, 53-bit precision) and rounds to float only at the stores; modern MSVC
  (`/fp:precise`, SSE2 and x87 alike) rounds each intermediate to float. Not CRT math, not an asm
  helper, not an engine bug. The 1-ulp start feeds back through the dynamics: by frame ~980 a
  rotation element is off by 1.54 and the translation by 0.39; no other hashed field differs in
  1860 frames. x87 with `/fp:fast` diverges too (from frame 63).
  **Tried:** a `double` branch (`OPTIMIZED_FOR_PC_FLOATS_LIKE_X87`, CMake `R2_FLOATS_LIKE_X87`,
  DEVIATIONS 7) in `MTH3D_M_vNormalizeVector` moves the first difference to frame 210. The next
  site, `POS_fn_vQuatToMat` (from `POS_fn_vRotatePositionAroundAxisNoTranslation`, AI 3D
  procedure; sin/cos identical), is register-allocation bound: VC5 keeps some products on the x87
  stack and spills others to float locals. Over 180 calls, all-`double` matches VC5 131 times, a
  model of VC5's exact spills 180 times. A precision rule cannot reproduce VC5; only copying its
  register allocation, site by site, can.
  **Original engine structure:** MTH chooses the real type and the math path per target (PC:
  float with `OPTIMIZED_FOR_PC_FLOATS_WITH_ASM`; `U64`: float on a single-precision FPU with its
  own asm; `NITRO`: 19.12 fixed point). Engine code such as `POS_fn_vQuatToMat` is one source for all targets, so each target
  computes its own numbers; no target reproduces the PC's. Cross-target bit equality was never a
  property of the engine.
  **Proposal (recommended):** follow the original: per-target arithmetic, each target checked
  bit-exact against its own baseline (same compiler and flags, last green commit, recorded by
  `tools/regress.py`), starting with the modern x86 build. VC5 + define stays the reference for
  every other hashed field and for `actor.matrix` up to its first difference, reported, not
  gating. Drop the `OPTIMIZED_FOR_PC_FLOATS_LIKE_X87` branch (it buys 146 frames, not equality).
  Target selection belongs in one MTH target block (Phase 5): x64 and other targets take
  `OPTIMIZED_FOR_PC_FLOATS` without `_WITH_ASM` (the original C branch), as U64 did.
  **Open question:** does anything need cross-build bit equality (demo playback, save data that
  stores positions)? Unchecked.
- Big-endian data: swap at load time or convert offline.
- License: none so far, same as upstream.
