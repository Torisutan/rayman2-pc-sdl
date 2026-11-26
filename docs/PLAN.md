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
   Platform calls in shared code go through names the module owns, defined per platform in the
   module's header, as the original engine does (`OPENFILESND_READ` in `WAVMWBVR.dll/include/SND/sndfile.h`,
   `TypeCriticalSection` in `MMG/MMG_alloc.h`). Never `#define` a system API name (`CreateFileA`,
   `WaitForSingleObject`, ...) to something else, and no Win32 types in a non-Win32 branch
   (Phase 5, "Platform glue"), except names the original engine keeps on every target and
   declares per platform itself (`HWND`, `GetTickCount`).

## Phases

### Phase 1: modern toolchain, no behaviour change
- A CMake build with current MSVC (and clang-cl), **32-bit x86**. The level files store 32-bit
  pointers that the loader rebases (`SNA_pc_vLoadAndRelocate`), and every intended target (PSP,
  Wii U) is 32-bit too, so 32-bit stays the model throughout. (2026-11-24: 64-bit targets are in
  scope as well, `WIN64` and later `OSX`; Phase 5, platform macros.)
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
  clang-cl is selectable (see the head of `CMakeLists.txt`); all 225 TUs compile
  (2026-11-23, clang 23.1; 198 before the decompilation fixes, the last one the `INO/INO.h`
  include order in `GAM/GAM_init.c`). `GAM_pc_fn_vLoadGliDll` is C
  under `RAYMAN2_NO_PROTECTION` (its asm only served the protection gate; the naked version
  crashes clang). clang-cl links (`/SAFESEH:NO`, the VC6
  `DINPUT.LIB` has no SEH tables) and runs the regression: 1860 frames, every field identical to
  the MSVC baseline except `actor.matrix` from frame 961 (per-target arithmetic, see Phase 2).
  Its own baseline: `build/regress/clang_baseline.txt`.
  The uninitialised-read pass on the 198 TUs that compile is in BUGS.md (Checked).
- **Crack behaviour (decided).** The post-link crack (`tools/crack.py`) only partly translates
  to modern codegen (21 of 29 patches; the result crashes). Instead Phase 2 comes first: the
  protection is removed, not emulated, and excluded from the build with `#ifndef
  RAYMAN2_NO_PROTECTION`, compiled into BOTH builds. Excluding instead of deleting keeps upstream
  merges clean; the code is deleted later. Measured chain: VC5 cracked = VC5 + define on every
  game field (protection state such as the LCG leaves the hash), then VC5 + define = modern +
  define.
- **Regression harness.** `Rayman2.exe/src/app/regress.c` (`RAYMAN2_REGRESS`), `tools/regress.py`,
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
  intermediate precision, not a bug. See "modern build regression reference" below.
- **Decided: regression reference.** With the protection actors inert, `rnd.index` no longer
  follows the original from the first draw they would have made. From now on the reference is
  VC5 + define (`tools/build_ref.py --define RAYMAN2_NO_PROTECTION --define
  RAYMAN2_LOCAL_UBIINI`), not VC5 cracked.
- **Decided: modern build regression reference.** Measured cause of the `actor.matrix` difference
  (Morb_00 frame 64): `POS_fn_vNormalizeMatrix` (from `MEC_fn_vCorrectMatrix`) gets bit-identical
  input in both exes. VC5 keeps the sum of squares, the square root and `inv_norm` on the x87 stack
  (control word 0x027F, 53-bit precision) and rounds to float only at the stores; modern MSVC
  (`/fp:precise`, SSE2 and x87 alike) rounds each intermediate to float. Not CRT math, not an asm
  helper, not an engine bug. The 1-ulp start feeds back through the dynamics: by frame ~980 a
  rotation element is off by 1.54 and the translation by 0.39; no other hashed field differs in 1860
  frames. x87 with `/fp:fast` diverges too (from frame 63).
  **Tried (removed again):** a `double` branch (`OPTIMIZED_FOR_PC_FLOATS_LIKE_X87`) in
  `MTH3D_M_vNormalizeVector` moved the first difference to frame 210. The next site,
  `POS_fn_vQuatToMat` (from `POS_fn_vRotatePositionAroundAxisNoTranslation`, AI 3D procedure;
  sin/cos identical), is register-allocation bound: VC5 keeps some products on the x87 stack and
  spills others to float locals. Over 180 calls, all-`double` matches VC5 131 times, a model of
  VC5's exact spills 180 times. A precision rule cannot reproduce VC5; only copying its register
  allocation, site by site, can.
  **Original engine structure:** MTH chooses the real type and the math path per target (PC: float
  with `OPTIMIZED_FOR_PC_FLOATS_WITH_ASM`; `U64`: float on a single-precision FPU with its own asm;
  `NITRO`: 19.12 fixed point). Engine code such as `POS_fn_vQuatToMat` is one source for all
  targets, so each target computes its own numbers; no target reproduces the PC's. Cross-target bit
  equality was never a property of the engine. **Decided:** follow the original: per-target
  arithmetic, each target checked bit-exact against its own baseline (same compiler and flags, last
  green commit, recorded by `tools/regress.py`), starting with the modern x86 build
  (`build/regress/modern_baseline.txt`, from the commit that drops the branch). Check:
  `regress.py diff BASELINE NEW` gates; `regress.py diff VC5 NEW --report actor.matrix` gates the
  other fields and reports the matrix. VC5 + define
  stays the reference for every other hashed field and for `actor.matrix` up to its first
  difference, reported, not gating. The `double` branch was dropped (it bought 146 frames, not
  equality). Target selection belongs in one MTH target block (Phase 5): x64 and other targets take
  `OPTIMIZED_FOR_PC_FLOATS` without `_WITH_ASM` (the original C branch), as U64 did.
- `ubi.ini` is read from the working directory (`RAYMAN2_LOCAL_UBIINI`, DEVIATIONS 1), as the
  current installs ship it. The per-user location is planned in "User folder" (below).
- **Unreferenced leftovers excluded (2026-11-23):** the resolver, code pool, kernel32 search and
  process enumeration in `MMG/MMG_init.c` (all but `MMG_fn_vInit`), `MMG_pc_fn_vNopGate`, the
  import tables in `MMG/pc_protect.c`, and the `SNA_g_lProtectionGate` gates of live functions
  (`MMG_fn_vInit`, `FUN_00405850`, `SNA_fn_ulFRead`, `SNA_pc_bOpenSnaFamilyFile`,
  `SNA_pc_pReadGptTable`). The gate variable is never written, so every gate was dead. Link
  maps (modern, VC5 with the define) contain none of these symbols; regressions identical. Still to do: delete the excluded code;
  the gate variables' declarations in `MMG/MMG_globals.h` go with it.

### Phase 3: fix the original engine's bugs
- Keep a catalogue in `docs/BUGS.md`: symptom, cause, function, fix. Known candidates from the
  decompilation include uninitialised reads (for example `bEnabledWrite` in `GAM_fn_vEngine`,
  which is read before `GLD_bRequestWriteToViewport` sets it) and the ini parser's skipped-rate
  path.
- One fix per commit, each marked `/* FIX(BUGS.md#n) */` at the site.
- **Decided (2026-11-23):**
  - **Fenced:** fixes under `#ifdef RAYMAN2_FIX_BUGS` (CMake option `R2_FIX_BUGS`, on; VC5
    reference: `tools/build_ref.py --define RAYMAN2_FIX_BUGS`), the original code untouched in the
    `#else` branch.
  - **Scope:** crashes and undefined behaviour only (NULL reads, uninitialised reads, dangling
    pointers, overruns). Gameplay-visible quirks (glitches, exploits) are not touched in this
    phase; if ever, under a separate opt-in define, since players and speedrunners may rely on them.
  - **Status (2026-11-23):** `docs/BUGS.md` lists 6 confirmed bugs, all fixed (#1-#6), regression
    unchanged by each (Morb_00, 1860 frames), so `modern_baseline.txt` stays. The ini parser's
    `ParticuleRate` path was checked and is well defined. Open leads in BUGS.md.
  - **Regression:** a fix may change the hashes (the original read undefined values). Every change
    is explained in the commit and in BUGS.md, and the modern baseline is re-recorded from that
    commit (the previous one is kept).

### Display: widescreen with 4:3 UI (`RAYMAN2_WIDESCREEN`)
CMake option `R2_WIDESCREEN` (on); VC5 reference: `tools/build_ref.py --define RAYMAN2_WIDESCREEN`.
- **Goal.** At a mode wider than 4:3 the 3D view widens (Hor+) and every UI element keeps its 4:3
  proportions inside a centred 4:3 band. At 4:3, and taller (5:4), nothing changes (k = 1).
- **Ratio.** Computed from the resolution, never configured: k = (height / width) / 0.75 in
  physical units (device pixel dimensions), clamped to 1; band offset (1 - k) / 2. Recomputed
  whenever the engine adjusts a camera to a viewport (device creation, mode switch, viewport ratio
  change), in `GLI/GLI_ratio.c`.
- **Inventory at 16:9 (2026-11-23), draw path per element:**

  | Element | Path |
  |---|---|
  | 3D view (game, cinematics) | `GLI_xAdjustCameraToViewport2`, perspective camera |
  | Loading vignette and progress bar | 16-bit bitmap to the back buffer (`GLI_pc_fn_vWrite16bBitmapToBackBuffer`), rect = overlay viewport clip |
  | Text (menus, pause, dialogues), health bar, 2D icons | `GLI_vDraw2DSpriteWithPercent` (JFFTXT, gauges, `AGO_vDisplayIcones`) |
  | Logos | `GLI_vDraw2DSpritePixels` |
  | Murfy cursor, HUD head/lum/cage icons, pause masks | superimposed objects, AI procedures 409/410: `GLI_vGet3DVertexFromScreenPos` + scale fix viewport width / 640 |
  | Script text (`fn_p_stCode4TextAffiche`) | same, scale width / 640 and height / 480 |
  | Cinematic letterbox | viewport clip + black polygons; the viewport stays full size, nothing to do |

- **Camera.** The camera AI (`CAM_fn_vCameraManagement`) rewrites `xAlphaX` from the camera's
  focal with the stored ratio, so widened angles passed by the `GLI_xSetCameraAspectAndRatio`
  callers would not survive. The camera keeps its 4:3 angles; the perspective projection divides
  tan(xAlphaX / 2) by k.
- **2D.** Percent and pixel sprites map into the band (full-width ones keep the full width);
  the loading vignette is squeezed into the band of its own bitmap; superimposed objects map
  their screen position into the band and scale to its width (DEVIATIONS 7-9).
- **HUD at the screen edges (2026-11-23, groups 2026-11-24).** The in-game HUD is drawn by the
  AGO gauge, icon and text tables (`AGO_vDisplayGAUGES`; measured: no superimposed objects), which
  the menus use too. The original separates them by `GAM_g_bEngineFrozen` (pause icons 15-24 and
  text slots below 15 draw while frozen). While not frozen, the boxes of all gauges, icons and
  strings drawn in the frame are grouped where they touch (gap up to 1 % of the 4:3 layout), and
  each group gets one anchor: left screen edge when it starts in the left third and ends in the
  left half, right edge mirrored, else the centred band (`AGO_fn_vComputeSafeAreaAnchors`,
  `GLI_fn_vComputeSafeAreaAnchors`, DEVIATIONS 11). A group keeps its 4:3 inner layout.
  Why groups: the tables do not pair an icon with its counter (measured: the hall of doors puts
  the lum counter in text slot 1 beside icon 1, Morb_00 in text slot 15 beside icon 2), so the
  pairing is only spatial. Measured boxes (percent of the 4:3 layout):

  | Screen | Group | Extent | Anchor |
  |---|---|---|---|
  | Morb_00 | head icon 0 + health gauge 0 | 2-25.8 | left |
  | Morb_00 | lum/cage icons 2, 3 + texts 15, 16 | 68-96.5 | right |
  | Hall of doors (`Mapmonde`) | lum/cage icons 1, 0 + texts 1 ("0/1000", 13-34.2), 0 | 7-34.2 | left |
  | Hall of doors | level panel: icons 2, 3, texts 16, 17 and the level name 15 (6-72.1) | 6-72.6 | centre |
  | Language menu (not frozen) | one group per line | 1.5-67.8 | centre |

  The earlier per-element rule (whole element in an outer third) split the hall's lum counter
  from its icon (PLAYTEST log 1); groups fixed it (screenshots at 1920x1080; at 4:3 and taller
  every anchor maps to the same place, k = 1).
- **Verified (screenshots, 2026-11-23):** at 1920x1080 and 1680x1050 the menu text, Murfy
  cursor, HUD (head, health bar, lum/cage counters and icons), pause menu (text, counters,
  masks; swirl background full width) and the loading vignette and bar keep 4:3 proportions
  in the band; the 3D view is Hor+. 1280x960 is unchanged.
- **Not verified yet:** the logo path (`AGO_vDisplayLogo`) and script text
  (`fn_p_stCode4TextAffiche`) were not on screen in the test runs; windowed/fullscreen toggle
  and window resize are not tested; 21:9 needs windowed mode (no such monitor mode here).

### User folder: `ubi.ini` and saves per user (planned, 24.11.2026)
User decision: the game folder decides by a marker file, not by an ini setting.
- **`portable.txt` in the game folder:** everything stays local, as today: `ubi.ini` in the game
  folder (DEVIATIONS 1), saves in `Data/SaveGame`.
- **No `portable.txt`:** `ubi.ini` and the save folder live in `Documents/UbiSoft/Rayman 2`
  (`ubi.ini`, `SaveGame/`), created when missing. Documents comes from the system: on Windows the
  Documents known folder (not a fixed `C:/Users/...` path, it can be redirected); in the SDL build
  `SDL_GetUserFolder(SDL_FOLDER_DOCUMENTS)`, which also serves Linux. If no Documents folder can be
  found, fall back to the game folder and say so in the log.
- **Game folder:** the working directory, as `RAYMAN2_LOCAL_UBIINI` already assumes (the installs
  start the game there).
- **What moves, from the code:** the ini path is the one of DEVIATIONS 1 (`FUN_00405a30`,
  `GAM_pc_fn_WinMain`, `GLI_pc_fn_vRead/WriteIniString`). The save path is the data directory plus a
  name from `Game.dsb` (`GAM_pc_fn_vReadExternalCtrlDirectories`, tag `0x2e`, stored by
  `FUN_004926d0`, read by the save code through `FUN_00492700`), so one site sets it to the user
  folder instead. Other directories of that table stay in the game folder.
- **Switch:** `RAYMAN2_USER_FOLDER`, CMake `R2_USER_FOLDER` (on); off keeps today's behaviour.
  New code in a new file (`GAM/GAM_userdir.c`), a DEVIATIONS row, a CHANGELOG entry (where the
  saves are now, and how to stay portable).
- **Proposed, to be confirmed:** on the first start without a per-user `ubi.ini`, copy the game
  folder's `ubi.ini` (it holds the renderer choice the installer made), and copy
  `Data/SaveGame` when the user folder has no saves yet. Copy only: never overwrite, never delete
  the game folder's files.
- **Open:** whether `Data/Options` (`Current.cfg`, the in-game options) moves with the saves; its
  directory's tag in `Game.dsb` is unchecked. `DLL/Snd_Cpa.ini` stays in the game folder (driver
  choice, not user data).
- **Tests stay out of Documents:** `tools/run.py` and `tools/regress.py` put `portable.txt` in
  their sandbox. Measure: both modes start, save to a slot and load it; regression identical.
- **Order:** with Phase 4, after the wider regression (it touches saves, which the wider
  regression and the save slot work cover).

### Loading screen: bounded refresh rate (`RAYMAN2_FAST_LOADING`)
CMake option `R2_FAST_LOADING` (on); VC5 reference: `tools/build_ref.py --define RAYMAN2_FAST_LOADING`.
- **Cause.** `GLI_AddToProgressBar` redraws the loading screen whenever the bar moves by more than
  1/50 of its width: `GLI_RefreshUiSurfaceIfVisible` copies the vignette to the back buffer and
  waits for a synchronised flip (`GLD_bFlipDeviceWithSynchro`), up to ~50 times per load.
- **Change.** The bar is still drawn into its buffer at every step; the refresh runs at most once
  per 100 ms (`GLI_C_ulProgressRefreshPeriod`, `GLI/GLI_fastload.c`). The end of the load
  shows the full image again (`FUN_0045edf0`), so the screen always ends on the full bar.
  DEVIATIONS 10.
- **Measured (2026-11-23, `tools/run.py --level`, mode-6 request to level playing, 3 runs, 0.1 s
  resolution):**

  | Build | Morb_00 | Rhop_10 |
  |---|---|---|
  | before | 0.8-0.9 s | 0.9-1.4 s |
  | per-step refresh skipped (test patch) | 0.3-0.4 s | 0.4 s |
  | loading screen disabled (the five functions `ret`, lower bound) | 0.3 s | 0.3-0.4 s |
  | `RAYMAN2_FAST_LOADING` | 0.4 s | 0.4 s |

  The per-step refreshes are the whole cost; showing the vignette is not. Regression identical
  (Morb_00, 1860 frames). Not checked by eye yet: the bar visibly advancing during a load.

### Phase 4: make the codebase solid
- Clean warnings at a high level (`/W4`, `-Wall -Wextra`), static analysis, and AddressSanitizer
  runs of the regression sequence.
- Replace the remaining raw struct offsets and casts with typed fields.
- Document every assumption about endianness, pointer size and struct packing at the place it is
  made (`docs/PORTABILITY.md`). Phase 5 works from this list.
- **Order (user, 2026-11-24):** Phase 5 steps 1, 3, 4 and 5 ran ahead of this phase. Before more
  Phase 5 work, one task per session: a playtest, a wider regression (more levels, recorded
  input), then this phase, then closing Phase 2.
- **Wider regression (done, 2026-11-26).** `tools/regress.py suite EXE OUTDIR` runs the scenarios of
  `tools/regress/suite.txt` (level, frames, input script; the file says why each was chosen):
  `morb_00` (the first run, unchanged), `menu` (no level request: language and options screens,
  name entry, the new-game save, the level change into the Jail_10 intro), `ski_10` (walks into
  the exit portal: save prompt, level change from play into the hall of doors), `boat01`
  (damage, fire, enemies), `water_10` (lums, several enemies). `regress.py diff` takes two suite
  folders. The input scripts are written in the script format (`R2_REGRESS_RECORD` writes the same
  format from a real keyboard); each was checked against an empty script: the input changes the
  state from the first keys on (Learn_10 was dropped: its opening cutscene ignores input for
  ~1490 frames). New hashed fields: `world.nb` and `world.matrix`, the number and the global
  matrices of the dynamic world's actors (enemies, AI, scripted objects); every earlier field of
  Morb_00 unchanged against `modern_baseline.txt`.
  **Baselines** (`build/regress/base_<gate>/`, each recorded twice, both runs identical):
  `base_modern` (SDL, MSVC and ANSI builds give identical files), `base_clang`, `base_vc5`. The VC5
  reference now gets the same feature defines as the CMake builds (`--define` each of
  `RAYMAN2_NO_PROTECTION`, `RAYMAN2_LOCAL_UBIINI`, `RAYMAN2_WIDESCREEN`, `RAYMAN2_FAST_LOADING`,
  `RAYMAN2_FIX_BUGS`; out `build/ref_npf`): with only the first two, the widescreen 4:3 UI scales
  a menu actor (0.75) and Water_10's random index differs from its first frame.
  **Cross-target (measured):** VC5 and modern differ in `world.matrix` from the first frame (1 ulp
  in four menu actors after load: per-target arithmetic, Phase 2) and in `actor.matrix` from the
  level start. With gameplay the drift reaches other fields: `world.nb` (morb_00 frame 1206,
  ski_10 501, water_10 515), clang-cl and VC5 agreeing there against MSVC; ski_10 on clang-cl
  also `eng.prevlevel` (776). Every such difference comes after the matrices differ. So the gate
  is each target against its own baseline; `diff base_vc5 NEW --report actor.matrix --report
  world.matrix` is a report (menu and boat01 stay identical in every other field).
- **Warnings (inventory 2026-11-26; open).** Measured with extra build folders, configured as the
  gate builds plus `CMAKE_C_FLAGS` `/W4` (MSVC, with and without SDL) and clang-cl `/W4` (= `-Wall
  -Wextra`), every TU rebuilt; a site counts once per file, line and warning. Before: MSVC 882
  sites in 28 warnings, clang-cl 1714 in 35. Almost all are in upstream files; the fork's own
  files and lines had 24 (MSVC) and 80 (clang-cl). Dispositions:
  - **Fork code, fixed** (the commit after this inventory): the missing `fn_vOneTrame` prototype in
    `GAM/sdl_main.c`; the regression recorder wrote the frame number instead of the key code for
    the second and later keys held in one frame (`app/regress.c`; the suite's scripts are written by
    hand, so no baseline depended on it); `GLI/GLI_linkdrv.c` keeps its entry points as a generic
    `void (__cdecl *)(void)` and converts to `FARPROC` once, where it answers like
    `GetProcAddress` (42 calling-convention and 20 function-type casts in clang-cl).
  - **C runtime deprecations** (C4996, 318 of 320 sites: `strcpy`, `sprintf`, `fopen`, ...):
    `_CRT_SECURE_NO_WARNINGS` for both targets in `CMakeLists.txt`. The sources use the standard
    functions on every target; the `*_s` variants are Microsoft only. `GetVersionExA` (Win32 API
    deprecation, 2) stays reported. After this: MSVC 563, clang-cl 1426; no warning in a fork-only
    file.
  - **Declarations and types, to the decomp** (a request, as for the window handle): calls to
    undeclared functions (C4013 / `-Wimplicit-function-declaration`, 16), declarations inside
    function bodies (C4210, 96), argument and assignment types that differ from the declaration
    (C4133, C4047, C4024, C4057, C4113 and clang's pointer-type warnings, about 90; the pointers
    kept in `unsigned long` among them are 64-bit blockers), the two missing return values
    (`fn_uiEngineMainLoop`, `GLI_fn_lRequestNewDisplayMode`), and `%d` given a `long` (24, wrong
    where `long` is 64-bit; also a PORTABILITY item). Fixing them here would edit upstream files
    line by line (rule 2); upstream carries them once the decomp has them.
  - **Possible defects (checked 2026-11-26, Phase 3 rules; BUGS.md "Checked").** Each against
    the retail asm and the original source: no retail bug. Original behaviour: `AGO/AGO.c` (the
    precedence makes `AGO_ulGetElderOne` always return -1, so a full particle table drops new
    particles; callers test for NULL), `HIE/HiePick.c` (`!a & 0x200`, a test that never fires).
    Defined: `IPT/IPT_Kwan.c` And/Or (bitwise on purpose; call order unspecified, result the
    same). Indentation only: `AI/cam_tool.c`, `SND/SND.c`. To the decomp request (items 5 and 6):
    `GAM/textfrmt.c` `p += f(++p)` (undefined in C; retail, MSVC and clang-cl all compute
    `(p + 1) + f`, other compilers need not: a port item) and the camera state enum
    (`AI/cam_base.c`, a placeholder enum with `#define`d states, same values as the original).
    Uninitialised reads: the leads outside the files of the 2026-11-23 pass are false positives
    (paired tests) or unreachable (mouse keywords: measured under cdb, never evaluated; the game
    has no mouse input). No code change, regression not affected.
  - **Protection leftovers:** unused variables whose only uses are fenced by
    `RAYMAN2_NO_PROTECTION` (most of C4101/C4189 in `MMG/SNA.c`, `MMG/MMG_init.c`, `AI/Function.c`,
    `AI/Procedure.c`, `GAM/GAM_engine.c`). They go with the excluded code (closing Phase 2).
  - **Not changed** (style of the original code or of a matching decompilation; changing them
    would rewrite upstream files, rule 2): unused parameters (C4100, 155), narrowing assignments
    and truncating casts (C4244, C4310), initializers without every field or brace (clang 514 and
    195), `x = x` self-assignments (56, AI), unused constants and static functions, `#pragma
    optimize` arguments clang does not know (20), signed/unsigned comparisons, shadowed locals,
    function-pointer casts in the engine's own tables (SND, AI, GAM, about 100 in clang-cl).
    They stay listed; the build keeps its default warning level until the decomp's batch has
    landed, then the remaining set is decided again.
- **AddressSanitizer (done, 2026-11-26).** The regression suite under MSVC `/fsanitize=address`
  (build folder configured as the SDL gate build plus `CMAKE_C_FLAGS` `/fsanitize=address /Zi`,
  linker `/DEBUG /INCREMENTAL:NO`; the runtime DLL from the compiler's `bin/Hostx86/x86` on `PATH`;
  `ASAN_OPTIONS` `log_path` (quoted, the path has a colon), `windows_hook_rtl_allocators=true`,
  `detect_stack_use_after_return=1`). Two findings: an overlapping `memcpy` in the camera's
  history shifts (original engine source; BUGS.md #9, fixed) and the centre vector of
  `HIE_fn_vSendSuperObjectToViewport` kept in three locals (a decompilation difference; decomp
  request item 7). After both, every scenario ends without a report and equals `base_modern`. With
  the fix, SDL, MSVC, ANSI and VC5 equal their baselines; clang-cl changed as intended (its inlined
  copy ran from the end and corrupted one history entry per call), `base_clang` re-recorded (twice, identical) at 409d70b and
  now equal to `base_modern` in every field and scenario: the clang-cl drift described under "Wider
  regression" (`world.matrix` from frame 62, `world.nb`, `eng.prevlevel`) was this bug, not
  per-target arithmetic. VC5 against modern is unchanged (menu actors 1 ulp after load). Not covered: allocations inside the memory
  manager's heaps (one `malloc` per heap). Instrumenting them (poisoning in MMG) would be a fork
  change of its own; not planned.
  Rerun 26.11.2026 after the decomp's fix of the centre vector (upstream b102d4d, merged e31360d):
  no report in any scenario, all five equal `base_modern`.
- **MTH target selection (done 2026-11-26).** The original engine
  chooses the MTH path in one header, `MTH/MTH_def.h` ("Detect Targets": `_MSC_VER` -> `VISUAL`
  -> `MTH_PC_DEV`, `U64` -> `MTH_U64_DEV`, `NITRO` -> `MTH_NITRO_DEV`, else `MTH_UNKOWN_DEV`; only
  `MTH_PC_DEV` sets `OPTIMIZED_FOR_PC_FLOATS_WITH_ASM`). The decompilation sets the same macros
  unconditionally in `MTH_flt.h` and `MTH3D.h`; asked of the decomp: `MTH_def.h` in the original's
  form (byte-neutral under VC5). The fork then adds its targets as a branch beside `MTH_PC_DEV`
  (asm only on 32-bit x86 MSVC / clang-cl, not `WIN64`).
  **Asm off, measured** (temporary build, `OPTIMIZED_FOR_PC_FLOATS_WITH_ASM` undefined, gate MSVC
  configuration): the C branch of `MTH3dopt.h` builds after two decompilation gaps are closed
  (asked of the decomp): a local copy of `MTH_M_bDifferentZeroOpt` in `AI/CAM_Tool.c` (not in the
  original) collides with the header's C form, and `MTH3D_M_fn_vMulMatrixMatrixWithoutBufferORG`
  (the C branch of the matrix product; the original defines it in `MTH/MTH_Debug.c`) has no
  definition. The suite runs, twice identical; against `base_modern` it differs as per-target
  arithmetic predicts: `world.matrix` from frame 0, `actor.matrix` from 63, and through the drift
  `world.nb` (Morb_00 frame 1206, Water_10 515) and `eng.prevlevel` (Ski_10 776); `boat01` and
  `menu` equal apart from the matrices. Its own baseline is recorded once the target block is in
  (`build/regress/noasm_a` is only the measurement).
  **Asm left after that** (outside the protection, task "close Phase 2", and the `USE_SDL`-fenced
  PC timer and window code): `MTH_fopt.h` table functions (`MTH_fn_fSquareRootOpt`, inverse,
  inverse square root): asm in the original's PC branch, C only in its `U64`/`NITRO` branches, so
  a non-x86 target needs a branch there (**open:** which condition; the original only knows "PC"
  = not `U64`/`NITRO`); local copies of MTH asm (`AI/Function.c` `AI_FUNC_fn_fSquareRootOpt`,
  `GLI/pc_light_pipeline.c` `GLI_pc_xFastSqrt`, `GAM/effect.c` `FP_M_vAddVectorASM`) and
  `GLI/Texture.c` `gli_ftol_truncate` (asked of the decomp: use the MTH forms);
  `AI/FuncRay2.c` `fn_p_stGetCPUCounter` (`rdtsc`, a debug AI function: needs a timer branch),
  `SND/SND.c` `SND_pc_fn_vProbeCpu` (CPUID/MMX, chooses the sound driver: sound task),
  `MMG/SNA.c` `SNA_pc_bOpenFile` `__asm int 3` (bad open mode: needs a branch per target).
  **Done (26.11.2026):** the decomp added `MTH_def.h` in the original's form (upstream b102d4d,
  with the `CAM_Tool.c` copy removed and the MTH copies in `Function.c`, `pc_light_pipeline.c`,
  `effect.c` replaced by the MTH forms). The fork adds `MTH_C_DEV` beside `MTH_PC_DEV` (targets
  `WIN64`, `LINUX`, `OSX`, `PSP`, `WIIU`: `OPTIMIZED_FOR_PC_FLOATS` without the asm; Platform
  macros table) and `src/MTH/MTH_Debug.c` (the C branch's matrix product, body as the original).
  Measured: SDL, MSVC, ANSI, clang-cl and VC5 + define equal their baselines; a measurement build
  defining `WIN64` on x86 (`build-noasm`, `CMAKE_C_FLAGS` `/DWIN32 /D_WINDOWS /DWIN64`) takes the C
  branch, runs the suite twice identically, and is recorded as `base_noasm` (its own baseline;
  against `base_modern` it differs as measured above). Left: the C branches of `MTH_fopt.h` (decomp
  order M8: the original's `U64`/`NITRO` branches), then the fork's targets join those condition
  lists; the other asm listed above.
- **Portability list (done, 2026-11-26).** `docs/PORTABILITY.md`: every pointer-size, `long`-size,
  packing and byte-order assumption with its sites. Pointer size measured (the SDL gate's
  translation units compiled again as x86-64 with clang-cl, syntax only, pointer diagnostics on,
  minus what x86 reports): 171 pointer-to-integer and 301 integer-to-pointer casts, 12 narrowings,
  79 layout asserts (retail offsets, correct guards: level blocks are 32-bit memory images). LP64
  not measurable without a Linux toolchain; its known sites are listed. Decisions it feeds: Phase 5
  "Open decisions" (64-bit data, big-endian, `long` on LP64). Handles in `long` in the original's
  interfaces (sound voices, AI values, MMG chain) are fork work later; `int` locals holding
  pointers are the decomp's (request "warnings", category 2).
- **Playtest (2026-11-24):** checklist in `docs/PLAYTEST.md` (start/exit, game and saves,
  options, focus, input devices, widescreen and loading screen; SDL and Win32 builds, both
  drivers). Findings go to BUGS.md, to this plan, or are logged as original behaviour. First
  round (24.11.2026, SDL build, OpenGL): focus changes and level transitions fine; HUD split in
  the hall of doors (Widescreen "Open"); **open:** loading a save slot loads the current progress
  or the 88 % slot whichever is chosen, one slot folder is empty (log 2).
- **Save slots (read from the code 24.11.2026; driven on retail and fork 26.11.2026: no difference).** The running game lives in
  `Data/SaveGame/HiddenSlot` (`General.sav` plus one file per level, written by `fn_bPlayerSaveGameAll` and
  `fn_bLevelSaveGameAll`; when the engine calls them is unchecked);
  the menu slots are `Data/SaveGame/SlotN`. Saving to a slot (`fn_bSaveGameInSlot`) deletes
  `SlotN` and copies `HiddenSlot` into it; without a `HiddenSlot` it reports success and copies
  nothing. Loading (`fn_bLoadGameFromSlot`) deletes `HiddenSlot` and copies `SlotN` into it; an
  empty `SlotN` leaves an empty `HiddenSlot`. The menu lists the slots sorted by the date of
  their `General.sav` (folder date when there is none) and maps the menu position to a slot
  number through that list. An empty slot folder (log 2: `Slot2`) is a slot never saved into.
  Not matching retail yet (decomp status): `GAM_pc_fn_ucSortSaveSlotsByDate`,
  `fn_bSaveGameInSlot`, `GAM_pc_fn_bCopyDirectoryTree`, `GAM_pc_fn_bDeleteDirectoryTree`,
  `GAM_pc_fn_ucGrowSaveSlotList`, `GAM_pc_fn_lEnsureDefaultSaveSlot`,
  `GAM_pc_fn_ulEraseSaveSlotDir`; the sort, for one, copies the score text of only one of the two
  slots it swaps. Whether retail does the same is unchecked, so log 2 may be original
  behaviour or a decomp difference. Next: compare these seven with retail (`tools/byte_match.py
  NAME --asm`; fixes belong to the decomp, so they arrive through upstream), then play one fixed
  sequence (new game, save, second game, save, load each from the menu and during play) on the
  retail exe and on the fork, with copies of `Data/SaveGame`.
  Seen 26.11.2026 (fork, regression `menu` scenario, stopped at the "game saved" message after a
  new game): a new `Slot3` folder exists and is empty, and there is no `HiddenSlot`.
  **Driven, 26.11.2026.** 21 scenarios played automatically on the retail exe and the SDL build
  (local tooling: menus driven by key presses at fixed seconds, only while the game window has
  the foreground; screenshots; afterwards the tree of `Data/SaveGame` and `Data/Options`: folders,
  files, sizes, hashes, dates). Fixtures from the reference install's saves, frozen once per run
  and copied fresh per scenario: as they are (Slot1 82.6 %, Slot2 an empty folder), no saves, no
  `HiddenSlot`, only `HiddenSlot`, slot dates set apart; and "after" another scenario (quit and
  restart). Scenarios: the load list (as is, dated, none, only `HiddenSlot`), new game (as is,
  none, no `HiddenSlot`), load each entry from the menu, save during play (into the used slot
  under another name, under the same name, into a free entry), restart after a new game, a save
  and an erase, load the newly saved slot, load during play, erase each entry (yes, no).
  **Result: every tree identical on retail and fork** (dates compared as order only; the
  screenshots agree, apart from the fork's widescreen layout). So the seven non-matching
  functions behave as retail in everything the menus reach, and the playtest findings are
  original behaviour:
  - the menu deletes `HiddenSlot` when it starts; a new game writes a 2-file `HiddenSlot`
    (`General.sav`, `Menu_0.sav`) and an **empty** new `SlotN` folder, filled only by the first
    save during play;
  - an empty slot folder lists as "9AA 00.0%" with the folder date (the name of the game that
    created it: the name entry's up key turns A into 9) and loading it starts a new game (intro);
  - the list is sorted newest first by `General.sav` date, folder date without one;
  - saving under the name the slot already has is refused ("... ist bereits in diesem
    Verzeichnis ..."); under another name it overwrites (Slot1 then dated now) or fills a new
    `SlotN`; the overwrite and erase questions default to "Nein";
  - checked in the retail asm as well: the sort's swap copies one score tail only one way, and
    the slot walk of `fn_bSaveGameInSlot` stops on the "new slot" flag set by the AI save
    procedure; both as in retail.
  Not driven by the script (identical on both, but the step did not happen): overwriting the
  empty Slot2 (the name entry keeps "9AA"), loading from the in-game menu (the list does not take
  the selection at the timings used), saving without a `HiddenSlot` (the menu always recreates
  it). The set reruns in about 35 minutes per exe; run it again after any save-code change (the
  saves port: Win32 file calls in `GAM_save_slot.c`, `SAI_Save.c`).
### Phase 5 (later): platform layer and ports
The engine already has seams at the right places: the renderer function table (`GLI_DRV`), input
devices (`INO`), the sound driver interface (`SND`), files (`FIL`) and timers (`TMR`). The platform
layer puts backends behind those seams instead of adding a new layer on top, which keeps the
engine structure intact.

**Inventory (2026-11-24).** The game modules (AI, COL, DNM, MEC, GEO, HIE, IPT, POS, PRT, SCT,
MTH, ...) make no Win32 calls. The platform code sits in seven places:

| Area | Where | Win32 use |
|---|---|---|
| Window, message loop, threads | `GAM/pc_winmain.c`, its API in `GAM/GAM_pc.h` | window and message loop; the engine runs in a thread of its own (`GAM_pc_fn_vCreateEngineThread`), driven by five named events; a draw semaphore |
| Draw lock | `GLD.c`, `Texture.c`, `ui_state.c`, `GAM_GameEng.c`, `Basic.c`, `ToolCam.c` | `WaitForSingleObject`/`ReleaseSemaphore` on the draw semaphore, which the renderer driver also gets (`"DrawSem"`) |
| Renderer binding | `GLI/Init_Gli.c` (`GLI_fnl_InitDll`) | `LoadLibraryA` + 43 `GetProcAddress`; about 25 callbacks and 8 data blocks the other way (`SetCommonFct`/`SetCommonData`) |
| Input | `INO/INO.c` | DirectInput, already split per device type (`INO_pc_ReadKeyboard`, `_ReadMouse`, `_ReadJoystick*`) |
| Sound | `SND/SND.c`, `SndThrd.c`, `sndseq.c`, `sndfile.c` | driver DLLs, threads, mutexes (see Sound) |
| Files | `MMG/SNA*.c`, `SND/sndfile.c` | `CreateFileA`/`ReadFile` directly; `FIL` already uses the C runtime |
| Small helpers | `TMR`, `GLI/cpu_timer.c`, `GLI/timer_sub2.c`, `RND`, `MMG` allocators, ini reads | performance counter, `timeGetTime`/`GetTickCount`, `GlobalAlloc`, critical sections, 26 `GetPrivateProfileStringA` calls |

**Original engine structure.** The PC renderer DLL boundary and the engine thread are PC-only:
the console targets (`U64`, `NITRO`) link GLI statically, with the platform files beside the
common ones, and call `fn_uiEngineMainLoop` directly instead of from a thread.

**The `GLI_DRV` interface** stays the renderer interface. What a new driver has to cope with:
- the driver reads the engine's `GLI_tdstInternalGlobalValuesFor3dEngine` by raw offsets (63 sites
  in the Direct3D 6 driver), so a new driver needs the typed struct first (Phase 4, typed fields);
- four entry points take an `HWND` (`xInitDriver`, `bPrepareForGliWindowed`,
  `vPrepareForGliFullScreen`, `vActivateGli`). Decided 25.11.2026, as the original engine: the
  type stays `HWND` on every target, no new handle type. The original's console builds declare
  `HWND` themselves (`typedef int HWND;` in a console types header under `NITRO` / `U64`); a
  non-Windows target of the fork declares it pointer-sized in its types header, and
  `GAM/sdl_main.c` puts SDL's window in it there (on Windows the window's real `HWND`). GLD's window
  is `HWND` end to end since the decomp's e4d0867 (merge 2a88a7f: `GLD_tdstDeviceAttributes` as
  the original's, `HWND` in `GLD_bSetFullScreenModeForDevice` and the driver calls;
  `sdl_main.c`'s copies follow). Left for 64-bit: `GAM_pc_fn_vGetAppContext` (`pc_winmain.c`)
  hands instance and window out as `unsigned long`;
- DirectDraw-shaped parts carry over: `bLockDevice` is bound but never called by the exe; display
  mode enumeration, windowed-mode variants and texture pixel-format negotiation (565/4444/1555
  callbacks) map to GL, GX2 and GU;
- on PC the window thread renders too (the pause / fullscreen re-entry path), which a GL context,
  bound to one thread, does not allow. Non-Win32 targets therefore run the engine loop on the main
  thread, as the consoles do; the engine events and the draw lock then have nothing to guard.

**Steps.** Finished steps keep one entry; their measurements are in the commit messages.
1. **Done: linked-in renderer drivers** (23fd887; `USE_GLI_DX6`, CMake `R2_GLI_DX6`, on;
   DEVIATIONS 13). The Direct3D 6 driver from `GliDX6vf.dll/` is built into the exe and answers
   `GLI_fnl_InitDll`'s by-name binding from a table (`GLI/GLI_linkdrv.c`); any other driver is
   still loaded from the DLL folder, chosen by `GLI_DllFile` as before. Consoles use the same
   table without DLLs. On Windows 10 native DirectDraw lists the 16-bpp modes only under the
   `DWM8And16BitMitigation` compatibility layer, for the retail driver as for the linked-in one.
2. Phase 4 prerequisites: typed fields for the structures the driver reads, `docs/PORTABILITY.md`.
3. **Done: C runtime branches** (cfe971f, d6db94b, d16feaf) beside the Win32 code, selected by
   `FILE_MODE_C_WIN95` (set in `SND/snddef.h` as in the original engine) and forced on Windows by
   `RAYMAN2_C_RUNTIME` (CMake `R2_C_RUNTIME`, off): level data files (`MMG/SNA_file.c`), ini files
   (`FIL/FIL_ini.c`; differential test against the Win32 functions: same results and bytes except
   deleting an indented key, which the engine never does), MMG system memory (`CPA_MALLOC`/
   `CPA_FREE`). Since the platform glue each module header carries the branch.
   **Not in this step:** `sndfile`'s file functions stay Win32: the retail sound DLLs look them up
   by name and use their handles (two also call `ReadFile` on them), so they change with the own
   sound drivers (Sound, below).
4. A second platform file beside `pc_winmain.c`, same `GAM_pc.h` API: window and events, engine
   loop on the main thread; timers, tick counts and locks through the same backend.

   **Inventory (2026-11-24).** Two threads on PC. The *window thread* (`GAM_pc_fn_WinMain`):
   ubi.ini bootstrap, single-instance mutex `RAYMAN2EXE`, desktop depth probe (GDI), the draw
   semaphore and five named events, `MMG_fn_vInit`, `fn_vInitEngineWhenInitApplication`, window
   class `Rayman2` + window (320x200 client, caption, resizable frame), `Erm_fn_vSetWndHdl4Request`,
   `fn_bCreateMainDisplayScreen` (GLD device and viewport on the HWND), screensaver off, engine
   thread started; it spins until the engine thread has opened the events (`GAM_g_ulQuitFlag`),
   hides the cursor and runs the message loop until `WM_QUIT`, then waits for `ISDEADEVT`,
   terminates the thread and releases device, viewport and the MMG table. The *engine thread*
   runs `fn_uiEngineMainLoop`, which ends in `GAM_pc_fn_vEngineThreadExit` (posts `WM_QUIT`, sets
   `ISDEADEVT`, sleeps forever).

   The per-frame wait point is `FUN_00401160`, before every `fn_vOneTrame` (and the regression
   hook): when neither `EXEFRAMEEVT` nor `EXITEVT` is set, it sets `ISPAUSEDEVT`, puts the engine in
   pause and blocks in `GAM_pc_fn_vWaitForEitherEngineEvent`. `FUN_00401100`/`FUN_00401130` (end of
   game / end of step) poll `EXITEVT`. `MSGBOX` is created and never used; `RAYMAN2EXE` is never
   released.

   | Window-thread work | Trigger | SDL main-thread loop |
   |---|---|---|
   | Focus loss: `GAM_pc_fn_vEnterPauseOnDeactivate` (debounce 2 s / 30 s at level init, engine paused at its next frame, title "pause", sound driver released; from fullscreen `GAM_pc_fn_vDispatchWmActivate` swaps the device mode and minimizes) | `WM_ACTIVATEAPP` off, `WM_CANCELMODE`, `SC_MINIMIZE`, caption clicks while windowed | needed: window focus-lost event, SDL title / minimize / restore |
   | Focus return: `GAM_pc_fn_vRestoreEngineOnActivate` (fullscreen back, title, sound restored, engine resumed) | `WM_ACTIVATEAPP` on | needed: focus-gained event |
   | Display mode change from the options menu: `GLI_fn_lRequestNewDisplayMode` posts `WM_USER 0x25`, `GAM_pc_fn_vResumeAfterFocusReturn` pauses the engine, applies the mode under the draw lock, resumes | engine thread request | needed: a request flag served at the wait point, between frames (nothing to pause) |
   | Error message box: `Erm_fn_iMessageBox` sends `WM_USER 0x24` (cursor shown, pause, `MessageBoxA`) | engine thread request | ERM's own no-window path (`MessageBox(NULL, ...)`) on the main thread |
   | `WM_USER 0x21` (title text) and `0x22` (activate) | nobody posts them | not needed |
   | Close: `WM_CLOSE`/`SC_CLOSE` -> `WM_DESTROY` -> `GAM_pc_fn_vSignalEngineAndPostQuit` (sound released, `EXITEVT`) | user | needed: quit / close-requested event |
   | Alt+Enter (`WM_SYSKEYDOWN` `VK_RETURN`): input entries reset, so the key does not reach the game | user | needed: key event with Alt |
   | Alt, `SC_KEYMENU`, screensaver / monitor power swallowed; screensaver off for the run | user, system | SDL does it (keeps `SC_KEYMENU` from the menu loop, screensaver off while video is up) |
   | Power suspend / resume: engine thread suspended under the draw lock, sound paused | `WM_POWERBROADCAST` | not needed: one thread, the system suspends the process |
   | Minimum track size (`WM_GETMINMAXINFO`), `WM_MOVE`/`WM_SIZE` swallowed during play | system | not carried over (window chrome only) |
   | Draw semaphore around viewport/camera changes and flips (`GLD.c`, `Basic.c`, `GAM_GameEng.c`, `ToolCam.c`, the progress screen in `Texture.c`/`ui_state.c`); `"DrawSem"` given to the driver | both threads | nothing to guard: no draw lock (`GAM/GAM_pc_DrawLock.h` empty under `USE_SDL`), `fn_hGetDrawSemaphore` gives NULL as on the original's consoles. Neither the Direct3D 6 driver nor GOG's `GliOGLvf.dll` waits on, releases or closes `"DrawSem"` (no wait or semaphore function in either binary; both start with it NULL), so they get NULL |

   Outside `pc_winmain.c` the API is used by: `GAM_engine.c` (`fn_hGetDrawSemaphore`,
   `GAM_pc_fn_vGetAppContext` for the sound init), `GAM_init.c` (`GAM_pc_fn_hGetAppInstance` for
   input), `Init_Gli.c` (`"DrawSem"`; `GLI_FatalExitOrSleep` sets `ISDEADEVT` and sleeps when an
   engine thread exists, else exits), `Init_Gli_helpers.c` (`GAM_pc_fn_vPostResumeAfterFocus`),
   `INO.c` (`g_hInstance`), `ERM` (`WM_USER` message box), `main.c` (`WinMain`).

   **File split.** The decompiled `pc_winmain.c` also holds engine code that the original keeps
   in common files (`fn_vOneTrame`, `fn_uiEngineMainLoop`, `fn_bCreateMainDisplayScreen`, the fix
   camera and viewports). That code stays shared; only the Win32 functions (window, message loop,
   thread, events, focus) are fenced `#ifndef USE_SDL`, and `GAM/sdl_main.c` gives the same
   functions for the SDL loop. SDL pumps nothing while a level loads (Windows marks a window
   "not responding" after 5 s without messages); the largest levels load in 0.4 to 1.1 s, so no
   pump during loads. Revisit if a target loads much slower (consoles, cold media).

   Done (each measured in its commit):
   - **SDL window, events and main-thread loop** (0a68c74; `GAM/sdl_main.c`, CMake `R2_USE_SDL`,
     DEVIATIONS 14). On Windows the SDL window's HWND goes to the GLI driver, DirectInput and the
     sound driver unchanged.
   - **Timers** (efd0b39; `TMR/sdl_timer.c`, DEVIATIONS 15): `fn_vTimerGetCounter`,
     `fn_ulTimerTickPerSecond`, `fn_ulTimerEventTickPerSecond` as the original console timer code
     gives them; channel 0 milliseconds, 1 and 2 SDL's performance counter. The engine creates
     only channel-1 timers and the Direct3D 6 driver reads channel 1; channel 2 (the time stamp
     counter, its rate measured by a one-second busy wait at init) is read by nothing in the exe
     (GOG's `GliOGLvf.dll` unchecked). Its calibration keeps the low 32 bits of the delta, which
     wraps above about 4.29 GHz; no effect while nothing reads it. `GetTickCount` is the system's
     on `WIN32`, `TMR/sdl_timer.c`'s elsewhere (edf95ba).
   - **MMG lock** (e8bae53, glue f628850): `TypeCriticalSection` is an SDL mutex, reentrant as the
     critical section. `SND/SndThrd.c` keeps its own Win32 one (Sound).
   - **Draw lock** (3c22ed4, replaced by f12d3b2): none with one engine thread (platform glue).
   - **Window hidden until the device exists** (e1fac45, user decision): no 320x200 window while
     the driver starts.
   - **Focus return to fullscreen** (3ed919f). The decompiled names on the activation path are
     inverted: on loss `GAM_pc_fn_vHandleActivateEnterFullScreen` -> `GLD_bSwapDeviceMode` ->
     `vActivateGli(hWnd, 0)`; on return `GAM_pc_fn_vHandleActivateExitToWindowed` ->
     `GLD_bSwapToWindowedMode` -> `vActivateGli(hWnd, 1)`. GOG's OpenGL driver makes the window a
     popup itself (`SetWindowLongA`); SDL 3.4 puts its own style back whenever the window leaves
     the minimized state, so on focus loss SDL is told whether the driver left the window without
     a caption (`SDL_SetWindowBordered`).
   - Still Win32 in the SDL build outside SND (open for Linux): `Sleep` on the engine-thread path
     of `Init_Gli.c` and the rows of the platform glue inventory below.

   **Focus test with the Direct3D 6 driver (2026-11-24; dec93b9, d752d75).** Original retail exe,
   VC5 + define and the MSVC Win32 build behave alike on this system (16 bpp under
   `DWM8And16BitMitigation`): on focus return the window stays white with the pause title and
   ignores `WM_CLOSE`. Cause: a deadlock. The driver took DirectDraw's exclusive mode on the
   engine thread (`xInitDriver` -> `SetCooperativeLevel`), `vActivateGli(hWnd, 0)` on loss only
   sets a flag, so the engine thread still holds `__DDrawExclMode__` while paused; the window
   thread restoring the window waits for that mutex, the engine thread for the window thread.
   The SDL build (one thread) returns and plays. Not fixed (it would change the original
   threading). Open: a `docs/BUGS.md` entry with a fix (release exclusive mode on the engine
   thread before it pauses) only if the user wants the Win32 path usable on this system.
5. **Done: `INO` backend through SDL** (b8b0278; `INO/sdl_ino.c`, `INO/sdl_ino.h`, DEVIATIONS 16).
   The device layer of `INO/INO.c` stays shared; its DirectInput parts are fenced `#ifndef
   USE_SDL` and `sdl_ino.h` maps their names to the SDL functions. Keyboard through a `DIK_` -> SDL
   scancode table (AZERTY found as on PC), mouse (relative motion, wheel, four buttons), one
   joystick device per SDL gamepad present at init, laid out as DirectInput shows an XInput
   gamepad; no input without keyboard focus, as DirectInput's foreground mode. Not tested: mouse
   (enabled only by a data command) and gamepads (none attached).
6. OpenGL driver behind `GLI_DRV`, specified by the Direct3D 6 driver, compared by screenshots on
   Windows first.
7. Sound (below) at any point, independently.
8. Linux (compiler, asm C branches from Phase 4, calling-convention macros, case-sensitive data
   paths); `WIN64` (pointer-carrying level data widened at load); the consoles (GX2 and GU drivers
   linked in, big-endian data, memory). macOS (`OSX`) is in scope but postponed until after these.

**Platform macros (decided 2026-11-24).** Modelled on the original engine's three kinds: target
macros named after the platform (`U64`, `NITRO`, `WIN32`), a group macro derived from them
(`GAMECONSOLE`, `SND/snddef.h`), and `USE_`/feature macros for a choice within a target
(`USE_PROFILER`, `OPTIMIZED_FOR_PC_FLOATS[_WITH_ASM]`, `FILE_MODE_C_WIN95`). Every `#endif` names
its condition.

| Macro | Kind | Meaning |
|---|---|---|
| `WIN32` | target | Windows and the Win32 API. Kept as it is; also defined on 64-bit Windows, as `_WIN32` is |
| `WIN64` | target | 64-bit Windows, in addition to `WIN32`: only where pointer size or x86-only code matters |
| `LINUX` | target | Linux |
| `OSX` | target | macOS (in scope, postponed) |
| `PSP` | target | PSP |
| `WIIU` | target | Wii U |
| `GAMECONSOLE` | group | any console: `snddef.h`'s list plus `PSP` and `WIIU` |
| `USE_SDL` | backend | window, events, input, audio output and timers through SDL (SDL3; SDL2 on `WIIU`), on any target that takes it (Windows too, to compare against the Win32 path) |
| `USE_GLI_DX6` | renderer | Direct3D 6 driver linked in (`GLI_DllFile=GliDX6`; CMake `R2_GLI_DX6`) |
| `USE_GLI_GL` | renderer | own OpenGL / GLES driver linked in (`GliGL`) |
| `USE_GLI_GX2` | renderer | Wii U driver linked in (`GliGX2`) |
| `USE_GLI_GU` | renderer | PSP driver linked in (`GliGU`) |
| `OPTIMIZED_FOR_PC_FLOATS_WITH_ASM` | feature | MTH asm branch: 32-bit x86 MSVC / clang-cl only, not with `WIN64` |
| `OPTIMIZED_FOR_PC_FLOATS` | feature | MTH C branch: every other target (Phase 2) |
| `MTH_C_DEV` | MTH target | set in `MTH/MTH_def.h` beside the original's `MTH_PC_DEV`/`MTH_U64_DEV`/`MTH_NITRO_DEV` when `WIN64`, `LINUX`, `OSX`, `PSP` or `WIIU` is defined: `OPTIMIZED_FOR_PC_FLOATS` without `..._WITH_ASM` (Phase 4 "MTH target selection") |
| `FILE_MODE_C_WIN95` | feature | Win32 API for level data files, ini files and MMG memory; set in `SND/snddef.h` unless `GAMECONSOLE`, `USE_SDL` or `RAYMAN2_C_RUNTIME`; C runtime otherwise |
| `RAYMAN2_C_RUNTIME` | fork | takes the C runtime branches on Windows too, to check them against the regression run (CMake `R2_C_RUNTIME`) |

- New code goes beside the original branch. Fence by what the code depends on: Win32-only code
  in fork files by the target (`#ifdef WIN32 ... #endif /* WIN32 */`, never `_WIN32`; the CMake
  build defines `WIN32` for every Windows target), a backend choice by its `USE_` macro
  (`#ifdef USE_SDL ... #else ... #endif /* USE_SDL */`). In upstream files the Win32 code stays
  the `#else` branch (the VC5 reference build does not define `WIN32` on its command line; its
  `ole2.h`, which `windows.h` includes, does `#define WIN32 100`, so after a `windows.h` include
  `WIN32` holds under VC5 too, e.g. in `app/regress.c`). `#ifndef USE_SDL` alone
  does not mean "Win32": it stops holding once a non-Windows target exists.
- Fences classified (25.11.2026). In upstream files every `USE_SDL` fence is a backend choice
  with its counterpart in an SDL file: `pc_winmain.c` (window, message loop, engine thread ->
  `GAM/sdl_main.c`), `INO.c` (DirectInput -> `INO/sdl_ino.c`), `cpu_timer.c`, `timer_sub2.c`
  (-> `TMR/sdl_timer.c`), the draw lock (`GAM_pc_DrawLock.h`, `GAM_GameEng.c`, its
  `ReleaseSemaphore` declaration now in the Win32 `#else`), the MMG lock (`CpaMmg.h`,
  `MMG_alloc.h`), `Erm_fn_iMessageBox` (`ERM/CpaError.c` -> `ERM/sdl_erm.c`). Fork files: the Win32 calls of `sdl_main.c` fenced `WIN32` (single-instance
  mutex, `SetErrorMode`, the Windows-folder `ubi.ini`; other targets take the game folder until
  "User folder"); `GLI_linkdrv.[ch]` stay Win32 as a whole (Direct3D 6 only exists there): the
  `WIN32` fence for the DLL fallback and neutral handle types come with the second linked-in
  driver (`USE_GLI_GL`), not before. Fork `#endif`s name their condition; file-local macros
  carry the module prefix (`SNA_M_bIsNoFile`, `GLI_M_LinkedProc`), as the original's
  `<MOD>_M_*` (d0703c0, f02ea3e).
- Backend function names (decided 25.11.2026, as the original engine: its file functions are
  `SND_fn_*` whatever the file mode, the platform only in branches and file names):
  `<MOD>_fn_*`, no platform infix, no suffix. `FIL_fn_ulGetProfileString`, `SNA_fn_hOpenFile`,
  `MMG_fn_vEnterCriticalSection`, `GLI_fn_hLoadDriver`, ... (were `FIL_pc_fn_*C`,
  `SNA_pc_fn_*C`, `MMG_sdl_fn_*`, `GLI_pc_fn_*`). The decomp's own `<MOD>_pc_*` names stay.

**Platform glue (decided 2026-11-25).** Below the seams (files, ini, locks, semaphores, tick
counts, `Sleep`, thread ids) shared code calls module-owned names, not system API names:
- each module gets neutral macros or functions in its own header (names below), with one
  branch per platform. In headers the decomp shares, the Win32 branch is the `#else`, as in the
  original engine (`#if (defined(U64) || defined(NITRO) || defined(PSX))` ... `#else` Win32 in
  its MMG header): the VC5 reference build defines neither `WIN32` nor `USE_SDL`, only the
  compiler's `_WIN32`. The other branches come first (`USE_SDL`, `GAMECONSOLE`, or
  `FILE_MODE_C_WIN95` unset for the C runtime);
- the call sites use the neutral name. Under VC5 the Win32 branch expands to the same call, so
  the change is byte-neutral and belongs in the decomp (arrives through upstream); the fork adds
  only the non-Win32 branches;
- neutral types for handles and results (`SNA_tdhFile`, ...), so the non-Win32 branches compile
  without `windows.h`; except where the original engine keeps the Win32 name on every target
  (`HWND`, `GetTickCount`: a non-Windows target declares them in its types header or timer file);
- SDL only in backend files (`sdl_*.c`): a header branch for `USE_SDL` calls a backend function,
  not SDL itself;
- replaces the current Win32 name renames. No new renames meanwhile: the local pre-commit check
  rejects added lines that `#define` a Win32 API name, none left (19 at first; the allow list is
  empty since the TMR step).

Inventory (25.11.2026; system calls in the SDL build measured per object with the import list of
the game's binaries; sound left to Sound, below):

| Module | Today | Users | Neutral name | Name from |
|---|---|---|---|---|
| MMG lock | **done** (f628850): `USE_SDL` branch in `MMG/CpaMmg.h` -> `MMG/sdl_mmg.c` (`MMG_fn_*`), Win32 `#else` | MMG allocators | `Mmg_MCreateCriticalSection()`, `Mmg_MEnterCriticalSection()`, `Mmg_MLeaveCriticalSection()`, `Mmg_MDeleteCriticalSection()` on `g_csMmg` | original engine (empty on the consoles) |
| MMG memory | **done** (f628850): C runtime branch in `MMG/PrivMmg.h` without `FILE_MODE_C_WIN95` | MMG allocators | `CPA_MALLOC`, `CPA_FREE` | original engine |
| ERM | **done** (729219e): `Erm_fn_iMessageBox` per platform, as the original (whole function per platform; its consoles print and return 0): Win32 the `#else` of a `USE_SDL` fence in `ERM/CpaError.c`, SDL in `ERM/sdl_erm.c` (`SDL_ShowMessageBox`, parent the focused window; stderr and 0 if SDL cannot show it). Style and result keep the Win32 encoding on every target: the callers pass numbers (`0x40035`, `0x40010`) and test numbers (2 = cancel); `sdl_erm.c` spells them `ERM_C_*`, no `MB_*`/`ID*` outside Win32 (the original's consoles declared `MB_*` in their types header with values of their own and ignored them) | `GAM_init.c`, `GLI/Cache.c` (CD and driver messages) | `Erm_fn_iMessageBox` | original engine |
| SNA level files | **done** (5e07224, e027e35): C runtime branch in `MMG/SNA_pc.h` without `FILE_MODE_C_WIN95` -> `MMG/SNA_file.c` (`SNA_fn_*`, handle `void *`); Win32 values through `SNA_pc_C_*` | `SNA.c`, `SNA_reloc.c` (also the fork's no-protection branches), `GAM_init.c` | `SNA_pc_M_hOpenFile`, `_bReadFile`, `_ulSeekFile`, `_bCloseFile`, `_ulGetFileAttributes`, `_bSetFileAttributes`; type `SNA_tdhFile` | fork, shaped as SND's `tdhSndFile` / `SND_fn_hOpenFileReadSnd` (SNA's own source is not at hand) |
| FIL ini | **done** (cf740d7, 6d31e2b): C runtime branch in `FIL/FIL_pc.h` without `FILE_MODE_C_WIN95` -> `FIL/FIL_ini.c` (`FIL_fn_*`) | `GAM_init.c`, `GLI/Init_Gli_helpers.c`, `SND/SND.c` | `FIL_pc_M_ulGetProfileString`, `FIL_pc_M_bWriteProfileString` | fork |
| GAM draw lock | **done** (f12d3b2): empty `USE_SDL` branch in `GAM/GAM_pc_DrawLock.h` and in `GAM_GameEng.c`'s copy (an include there changes a function's bytes); the driver gets NULL | `GLD.c`, `Basic.c`, `GAM_GameEng.c`, `ToolCam.c`, `Texture.c`, `ui_state.c` | `GAM_pc_M_ulWaitDrawSemaphore`, `GAM_pc_M_bReleaseDrawSemaphore`; **empty** with one engine thread: the original's console build has no draw lock (`fn_hGetDrawSemaphore` returns 0) | fork |
| TMR tick count | **done** (edf95ba): `GetTickCount` declared and defined in `TMR/sdl_timer.[ch]` only `#ifndef WIN32` | `RND.c`, `GLI_fastload.c` | **none**: the original's console timer code declares its own `GetTickCount(void)`; on Windows the system's serves | original engine |
| GLI driver binding | **done** (109cdcf): `USE_GLI_DX6` branch in `GLI/GLI_pc.h` -> `GLI/GLI_linkdrv.c` (`GLI_fn_*`; linked-in drivers, DLL fallback), Win32 `#else` | `GLI_fnl_InitDll` | `GLI_pc_M_hLoadDriver`, `GLI_pc_M_pfnGetDriverProc` | fork |
| GLI window handle | **done** (7cc0a2a, e4d0867): `HWND` on every target, GLD's window fields `HWND` | `GLD.c`, `Init_Gli.c`, `pc_winmain.c`, `sdl_main.c` | `HWND`, declared by a non-Windows target's types header | original engine (console types headers) |
| GLI fatal exit | **done** (8225e50): the engine-thread path is the `#else` of a `USE_SDL` fence (no engine thread there), only `GLI_RT_exit(-2)` left | `GLI_FatalExitOrSleep`, path taken only with an engine thread | none | original engine (consoles: no engine thread) |
| GLD thread id | **done** (5f1b3da): `MMG_pc_M_ulGetCurrentThreadId()` at the 8 `GLD.c` sites (from the decomp); `USE_SDL` branch in `MMG/MMG_pubaccess.h` -> `MMG_fn_ulGetCurrentThreadId` (`MMG/sdl_mmg.c`, `SDL_GetCurrentThreadID`), Win32 `#else`. The real id, not channel 0 as on the original's consoles: the sound code still asks with `GetCurrentThreadId` and must get the same channel | MMG channel of the calling thread (`Mmg_fn_ucGiveChannelId`) | `MMG_pc_M_ulGetCurrentThreadId` | decomp, original engine style |
| Saves | `CopyFileA`, `DeleteFileA`, `Find*FileA`, `RemoveDirectoryA`, `Get/SetCurrentDirectoryA` direct (`GAM_save_slot.obj`); `DeleteFileA` (`SAI_Save.obj`) | save slots, SAI | open, with the per-user folder (PLAN "User folder") | |
| FIL temp path | **done** (5452b54): `FIL_pc_M_ulGetTempPath` in `GAM_init.c` (from the decomp); C runtime branch in `FIL/FIL_pc.h` without `FILE_MODE_C_WIN95` -> `FIL/FIL_path.c` (`TMP`, `TEMP`, `TMPDIR`, else `/tmp`) | fills `FIL_PATH_BUFFER(10)`, read only by the saves (`TT.tmp`); `FUN_00492860`/`FUN_004928c0` not yet named | `FIL_pc_M_ulGetTempPath` | fork, as the ini calls |
| Misc direct | **done**: `ShellExecuteA` (f7831e7) only on Windows, SDL backend included: `LINUX`/`OSX`/`PSP`/`WIIU` take `fn_p_stNoneProcedure` in the procedure table; `FreeLibrary` (91073b9) excluded with the protection (PSAPI handle and MMG table exist only with it); `GetClientRect` (7d780ea, result never read) `#ifndef USE_SDL` | `AI/ConvProc.c`, `AI/Procedure.c`, `pc_winmain.c` | none | original engine (`ShellExecute` mapped to its dummy except on `WIN32`) |
| Fork-only | `sdl_main.obj` (`CreateMutexA`, `GetLastError`, `GetWindowLongA`, `SetErrorMode`, `GetWindowsDirectoryA`, `CreateDirectoryA`, under `WIN32`), `regress.obj` (C runtime branch beside it since bb2f980: `getenv`, `exit`) | | stays, fenced `WIN32` | |

Migrated 25.11.2026 (TMR, MMG, GAM draw lock, FIL, SNA, GLI, window handle): the decomp put
`<MOD>_pc_M_*` names at the call sites, Win32 branch only (merge f628850 and later); the fork
adds the other branches. ERM and the rest go with the targets that need them.
Why: a rename keeps the Win32 types at every call site (a `windows.h` stand-in on Linux and the
consoles, i.e. Win32 emulation), reroutes every call of that name in the file (an upstream merge
that adds an unrelated `WaitForSingleObject` would silently reach the SDL semaphore), and depends
on include order.
- No linked-in driver takes the name `GliOGL`: GOG ships a `GliOGLvf.dll`, which a linked-in
  driver of that name would shadow.
- The fork's own options keep the `RAYMAN2_` prefix (`RAYMAN2_WIDESCREEN`, `RAYMAN2_FIX_BUGS`, ...),
  so a fork option and a platform choice are told apart by name.

**Backend library (decided 2026-11-24): SDL3 for the PCs** (Windows, Linux, later macOS) and
PSP (SDL3 has a PSP port); SDL2 on Wii U (the port available there). Getting it: Visual Studio's
vcpkg in manifest mode (`vcpkg.json`, triplet `x86-windows`).
- SDL3 covers what the engine's seams need: window and events (`GAM_pc.h`), keyboard, mouse and
  gamepads with its controller mapping table (`INO`), audio output (`SND`'s output backend; the
  mixer, bank reader and ADPCM decoder stay the engine's own), timers, threads and locks, an
  OpenGL / GLES context for the GL driver, and a per-user folder (`SDL_GetPrefPath`, for the
  per-user `ubi.ini` and saves). License zlib.
- Not used: SDL's 2D renderer and GPU API (they would replace GLI, not sit behind `GLI_DRV`, and
  cover no console), and its file streams (the C runtime branches serve, step 3).
- SDL stays out of the engine: it is called only from backend files behind the seams. The Wii U
  build needs SDL2 equivalents of the SDL3 calls those files make; keeping SDL to these files
  keeps that difference small (a few `#if` in them, or an SDL2 variant of a backend file).
- **Original engine style for backend code**, as its console files are written (the DS and
  U64 share `<MOD>/Specif/u_*.c`; the project's file list picks a platform's files, their own
  `#if`s are feature switches): one backend file per module beside the PC one, the backend in
  the file name, header as its file (`GAM/pc_winmain.c` -> `GAM/sdl_main.c`, `INO/sdl_ino.c` +
  `INO/sdl_ino.h`); CMake's file list picks them, as the original's project files do. No
  central platform folder (the original has none); a `<MOD>/Specif/` folder for the fork's
  backend files only once a module has several per target, then for every module at once. The
  platform is never in a prefix. Names as the original writes them (counted in its sources):
  a function that stands in for a PC function keeps its name (`fn_vTimerGetCounter` in both
  timer files); other external functions `<MOD>_fn_*`; file-static functions `fn_*`; file-static
  globals `gs_*`, shared globals `g_*`; file constants `C_*`, header constants `<MOD>_C_*`;
  macros `<MOD>_M_*`; types `<MOD>_tdst*`. A platform word appears only inside a name whose value
  is the platform's own (the original's `g_uNextNitroCache`, `Nitro_Init3D`; here
  `gs_p_stSDLWindow`, `INO_fn_wReadMouseSDL` beside the decomp's `INO_pc_wReadMouseDI`).
  Hungarian type prefixes, `/* */` comments, file headers and description blocks as in the
  fork's other files; the whole file fenced `#ifdef USE_SDL ... #endif /* USE_SDL */`, its PC
  counterpart `#ifndef USE_SDL` with the original code untouched. The decomp's own names
  (`<MOD>_pc_*`) stay as they are.
**Open decisions.**
- **64-bit data (decided: in scope, 2026-11-24).** `WIN64` and `OSX` need it. The level files store
  32-bit pointers, so every pointer-carrying structure loaded from level data has to be widened at
  load. How (per-type load tables or an offline converter, as for big-endian data) is open; decide
  it with the big-endian question, from Phase 4's portability list.
  Measured 26.11.2026 (`docs/PORTABILITY.md` sections 1, 2): 79 layout asserts pin level-block
  structures to retail offsets, 472 pointer/integer casts besides. A third option exists in the
  headers: the original's own cartridge level format (`include/BLC/`, index-based, packed), so an
  offline converter could target the original's binary loaders.
- **`long` on LP64 (Linux, macOS; new 26.11.2026):** the code assumes a 32-bit `long` in file
  headers, `.gpt` dwords and float bit tests (`docs/PORTABILITY.md` section 3). Fixed-width types
  per target in the base headers, or `-m32`-like data models where available; decide before the
  first Linux build, measured with a Linux toolchain then.
- **Big-endian targets (Wii U):** the data files are little-endian. The choices are byte-swapping
  at load time, which needs type information for every relocated block, or an offline data
  converter. Decide it when Phase 4's portability list exists.
- **Key codes (decided 2026-11-24): DirectInput key codes on every target.** Checked: the
  original engine's key constants are `DIK_` codes (`C_ucKey_1 = DIK_1`, ...) and its script key table (key name -> value, compiled into the level data)
  uses them, with an AZERTY/QWERTY swap table beside it. In the exe IPT indexes the 256-byte
  keyboard state directly with the key values of the entry elements (`IPT_Kwan.c`,
  `IPT_Hist.c`); `ubi.ini` holds no bindings. The input backends map their own key codes to
  `DIK_` (SDL: scancode table in `INO/sdl_ino.c`).
- **Gamepad triggers:** DirectInput puts an XInput gamepad's two triggers on one Z axis, and the
  PC reader turns "Z down" into button 5. Which trigger moves Z which way is unchecked (needs a
  gamepad on the DirectInput build); until then the SDL backend leaves Z centred.
- **Direct3D 6 driver:** 26 of its 130 functions do not match their original bytes yet; which of
  them behave differently is unchecked.
- **Memory:** PSP has 32-64 MB in total. Measure MMG's block budgets per level before
  committing to that port.

## Sound

**Today.** The `SND` module (in the exe) manages events, banks, 3D positioning (`sndspace`,
`sndvect`), themes and sequences. It loads driver DLLs from the game's `DLL` folder at run time,
chosen by `DLL/Snd_Cpa.ini`, and they call back into the ~110 functions the exe exports. The data
is `.bnm` banks and `.apm` streams (ADPCM, used for music and voices). Drivers named by the
install's `Snd_Cpa.ini`:

| Section | Driver (MMX test passes) | Fallback | Source |
|---|---|---|---|
| `DLL_Sxd` (samples, 3D, mixer, output) | `WAVMWBVR.dll` | `WAVPWBVR.dll` | decompiled: `WAVMWBVR.dll/` (upstream, 758 of 767 functions byte-exact, `tools/build_wavmw.py`) |
| `DLL_Adpcm` (`.apm` streams) | `APMMXBVR.dll` | `APMPWBVR.dll` | none yet |

The `DLL` folder also holds MPEG drivers (`MPGMXBVR.dll`, `MPGP5BVR.dll`) and `MPEG_ON_OFF=ON`,
but `Snd_Cpa.ini` has no section for them; whether the game loads them is unchecked.

**Plan** (rewritten 24.11.2026 now that the sample driver is decompiled; the earlier plan wrote a
mixer and decoders from the file formats):
1. **Keep the `SND` module as it is.** It is decompiled, almost all of it byte-exact, and it already
   does the game-side logic, including 3D attenuation and panning.
2. **Link `WAVMWBVR` into the exe**, as the Direct3D 6 driver was (DEVIATIONS 13): its entry
   points through a table instead of `LoadLibraryA`/`GetProcAddress`, selected as before by
   `Snd_Cpa.ini`, the DLL path unchanged for any other driver. First an inventory of the
   boundary (exports both ways, call order, threads), in `docs/SOUND.md`; name clashes between the
   exe's `SND` and the driver's sources are to be measured, not assumed.
3. **Platform branches inside the driver, in the original engine's style:** the MMX mixer (`src/mixer/*.asm`) stays as
   written under its condition, with a C branch beside it for targets without it; the waveOut
   output, its thread and locks get an SDL audio branch under `USE_SDL`. Mixer and streaming code
   stay shared.
4. **ADPCM streams:** `APMMXBVR.dll` is not decompiled. Either it arrives through upstream like
   `WAVMWBVR`, or an `.apm` decoder is written from the file format behind the same interface.
   Decide when step 3 is done; until then the SDL build keeps loading it on Windows.
5. **Check:** the linked-in driver against the retail DLL with the same data (mixed output of a
   scripted scene captured from both), then the SDL output branch against the waveOut branch.
6. **Order:** unchanged, after Phase 4 and before the renderer.

## Animation: smoother playback (to evaluate)

**Status:** idea, not decided (user, 2026-11-24). Evaluate first, then choose one of the two
options, or neither. No code before the evaluation is written down here.

**Options:**
1. **Interpolate animation frames to 60 fps.** Blend between two stored animation frames at
   display time, so motion is smooth when the display runs faster than the rate the animations
   were stored at.
2. **Use the Dreamcast version's animations.** Only if they are smoother and the engine can read
   them. The data comes from the player's own copy; nothing of it goes into this repo.

**Evaluate first (never guess):**
- **PC data:** the rate the animations are stored at, and how the engine picks a frame. Is the
  frame chosen by time or by engine tick? Does it already blend between frames anywhere? Where
  do display rate and logic rate meet: the fixed-frame-rate setting in the ini, and the timer
  code?
- **Coupling:** does game logic read the current animation frame? Events on animation frames,
  sound triggers, collision zones attached to channels. Interpolation has to stay display-only,
  so logic sees exactly the frames it sees today.
- **Dreamcast data:** is the animation format the same as the PC's or a different one? Is the
  stored rate higher, and the same for every family? Do the files map one to one onto the PC
  level files? A different format means a converter, and that is a separate tool, not engine
  code.
- **Cost:** for option 1, where it lives. The channel and matrix computation is in the engine;
  the drawing is in the driver. For option 2, the work of loading data from another platform.

**Rules when it is built:**
- Behind a define (like `RAYMAN2_WIDESCREEN`), off in the regression build, so the regression
  stays identical.
- Its own measurement, frame captures at a fixed time step, with the define on and off.

**Order:** after Phase 4, and not before the playtest and the wider regression. Placing it
relative to sound and the GL driver is the user's call.

## Open decisions
- Platform layer: see Phase 5, open decisions.
- Cross-build bit equality: does anything need it (demo playback, save data that stores
  positions)? Unchecked; per-target arithmetic (Phase 2, modern build reference) assumes not.
- Big-endian data: swap at load time or convert offline.
- License: none so far, same as upstream.
- Renderer choice in the game (user goal, 2026-11-25). The linked-in drivers are additive and
  chosen by name (`GLI_DllFile`), so a choice "after restart" only writes that key. Switching
  while running is new work: the driver is loaded once (`GLI_fnl_InitDll` from `GAM_init.c`)
  and never released; it needs driver release, a new window (Direct3D 6 changes the window's
  styles, an SDL GL context needs an OpenGL window) and all textures uploaded again. Unchecked:
  whether the engine can reload its textures, and whether the options menu is built in level
  data (then a new entry needs a data change or a menu of the fork's own). The window handle
  stays `HWND` (Phase 5, `GLI_DRV` interface): a linked-in GL driver on Windows gets the real
  `HWND` (SDL can wrap it), elsewhere SDL's window.
