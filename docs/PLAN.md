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
  current installs ship it. A per-user location comes later, chosen by an ini setting (ROADMAP,
  advanced settings menu).
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
- **HUD at the screen edges (2026-11-23).** The in-game HUD is drawn by the AGO gauge, icon and
  text tables (`AGO_vDisplayGAUGES`; measured: no superimposed objects), which the menus use
  too. The original separates them by `GAM_g_bEngineFrozen` (pause icons 15-24 and text slots
  below 15 draw while frozen). While not frozen, an element lying entirely in the left or right
  third of the 4:3 layout keeps its 4:3 distance to that screen edge (`GLI_fn_vSetSafeAreaAnchor`,
  DEVIATIONS 11). Measured at 1920x1080: head and health bar at the left edge, lum and cage
  counters with icons at the right edge; pause menu and language menu unchanged (the pause
  menu freezes the engine; the menu text lies in the middle third). Regression identical.
- **Verified (screenshots, 2026-11-23):** at 1920x1080 and 1680x1050 the menu text, Murfy
  cursor, HUD (head, health bar, lum/cage counters and icons), pause menu (text, counters,
  masks; swirl background full width) and the loading vignette and bar keep 4:3 proportions
  in the band; the 3D view is Hor+. 1280x960 is unchanged.
- **Not verified yet:** the logo path (`AGO_vDisplayLogo`) and script text
  (`fn_p_stCode4TextAffiche`) were not on screen in the test runs; windowed/fullscreen toggle
  and window resize are not tested; 21:9 needs windowed mode (no such monitor mode here).

### Loading screen: bounded refresh rate (`RAYMAN2_FAST_LOADING`)
CMake option `R2_FAST_LOADING` (on); VC5 reference: `tools/build_ref.py --define RAYMAN2_FAST_LOADING`.
- **Cause.** `GLI_AddToProgressBar` redraws the loading screen whenever the bar moves by more than
  1/50 of its width: `GLI_RefreshUiSurfaceIfVisible` copies the vignette to the back buffer and
  waits for a synchronised flip (`GLD_bFlipDeviceWithSynchro`), up to ~50 times per load.
- **Change.** The bar is still drawn into its buffer at every step; the refresh runs at most once
  per 100 ms (`GLI_pc_C_ulProgressRefreshPeriod`, `GLI/GLI_fastload.c`). The end of the load
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
  `vPrepareForGliFullScreen`, `vActivateGli`) and GLD keeps the window as `unsigned long`: an
  opaque window handle type is needed, for 64-bit as well;
- DirectDraw-shaped parts carry over: `bLockDevice` is bound but never called by the exe; display
  mode enumeration, windowed-mode variants and texture pixel-format negotiation (565/4444/1555
  callbacks) map to GL, GX2 and GU;
- on PC the window thread renders too (the pause / fullscreen re-entry path), which a GL context,
  bound to one thread, does not allow. Non-Win32 targets therefore run the engine loop on the main
  thread, as the consoles do; the engine events and the draw lock then have nothing to guard.

**Steps.**
1. **Done (2026-11-24): linked-in renderer drivers** (`USE_GLI_DX6`, CMake `R2_GLI_DX6`,
   on; DEVIATIONS 13). The Direct3D 6 driver is built from `GliDX6vf.dll/` into the exe.
   `GLI_fnl_InitDll` keeps its by-name binding; a driver linked into the exe answers it from a
   table (`GLI/GLI_linkdrv.c`), any other driver is still loaded from the DLL folder. Selected by
   ubi.ini as before (`GLI_DllFile=GliDX6`). Consoles will use the same table without DLLs.
   Measured: regression identical (Morb_00, 1860 frames; MSVC against `modern_baseline.txt`,
   clang-cl against `clang_baseline.txt`; the regression runs the install's driver, so it shows
   the DLL path is unchanged). Linked-in against retail DX6 driver, same ubi.ini: Morb_00 loads,
   screenshots match. On Windows 10 native DirectDraw lists the 16-bpp modes only under the
   `DWM8And16BitMitigation` compatibility layer, for the retail driver as for the linked-in one.
2. Phase 4 prerequisites: typed fields for the structures the driver reads, `docs/PORTABILITY.md`.
3. **Done (2026-11-24): C runtime branches** beside the Win32 code, selected by
   `FILE_MODE_C_WIN95` (set in `SND/snddef.h` as in the original engine) and forced on Windows by
   `RAYMAN2_C_RUNTIME` (CMake `R2_C_RUNTIME`, off):
   - level data files: the Win32 file calls of `SNA.c`, `SNA_reloc.c` and `GAM_init.c` go to C
     runtime streams (`MMG/SNA_file.c`, included as `MMG/SNA_file.h`);
   - ini files: `Get/WritePrivateProfileStringA` in `GAM_init.c`, `GLI/Init_Gli_helpers.c` and
     `SND/SND.c` go to `FIL/FIL_ini.c` (`FIL/FIL_ini.h`). A differential test against the Win32
     functions (1043 reads, 234 writes on `ubi.ini`, the sound ini and edge cases) gives the same
     results and file bytes, except deleting an indented key, which the engine never does;
   - MMG system memory: the original `CPA_MALLOC`/`CPA_FREE` in `MMG/CpaMmg.h`, `GlobalAlloc`
     or `malloc`.
   The call sites stay as they are: each file includes the header that maps the Win32 names.
   Measured (Morb_00, 1860 frames): identical against `modern_baseline.txt` with and without
   `R2_C_RUNTIME`, VC5 + define identical against `np_b`. With `R2_C_RUNTIME` no object imports
   these Win32 functions except `SND/sndfile.c`.
   **Not in this step:** `sndfile`'s file functions stay Win32: the retail sound DLLs look them up
   by name and use their handles (two also call `ReadFile` on them), so they change with the own
   sound drivers (Sound, below). Timers, tick counts and critical sections need a backend (the C
   runtime has no portable monotonic clock or mutex on every target): they go with step 4.
4. A second platform file beside `pc_winmain.c`, same `GAM_pc.h` API: window and events, engine
   loop on the main thread; timers, tick counts and the MMG/SND critical sections through the same
   backend. First an inventory of what the window thread does (pause, focus, mode switch, power
   broadcast).

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
   | Draw semaphore around viewport/camera changes and flips (`GLD.c`, `Basic.c`, `GAM_GameEng.c`, `ToolCam.c`, the progress screen in `Texture.c`/`ui_state.c`); `"DrawSem"` given to the driver | both threads | nothing to guard; kept as an object (no wait ever blocks). Neither the Direct3D 6 driver nor GOG's `GliOGLvf.dll` waits on `"DrawSem"` |

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
   "not responding" after 5 s without messages). Measured (2026-11-24, SDL build, GOG's OpenGL
   driver, direct load from the menu, warm file cache): the eight largest levels by data size
   (Rhop_10 4.6 MB ... Morb_00 2.4 MB) load in 0.4 to 1.1 s, the menu plays 1.2 to 1.8 s after
   start. No pump during loads; revisit if a target loads much slower (consoles, cold media).

   **Done (2026-11-24): SDL window, events and main-thread loop** (`GAM/sdl_main.c`, CMake
   `R2_USE_SDL`, DEVIATIONS 14). On Windows the SDL window's HWND goes to the GLI driver,
   DirectInput and the sound driver unchanged; the draw semaphore stays a Win32 semaphore until
   its users move (locks, below). Measured: regression identical against `modern_baseline.txt`
   (SDL build) and unchanged for MSVC, clang-cl and VC5 + define with the fences; Morb_00 loads in
   about 5 s with GOG's OpenGL driver. Focus test (Alt+Tab away and back, then close): pause title
   and sound release on focus loss, resumed play on return, clean exit 0.2 s after `WM_CLOSE`.
   Open: on focus return the SDL build comes back as a full-size window with a caption instead
   of exclusive fullscreen; the Win32 build stops at "restoring data" in the same test with the
   same driver and does not quit on `WM_CLOSE` afterwards, so there is nothing to compare yet.
   Check both with the linked-in Direct3D 6 driver.

   **Done (2026-11-24): timers under `USE_SDL`** (`TMR/sdl_timer.c`, DEVIATIONS 15). The PC timer
   functions of `GLI/cpu_timer.c` and `GLI/timer_sub2.c` are fenced; the SDL file gives
   `fn_vTimerGetCounter`, `fn_ulTimerTickPerSecond`, `fn_ulTimerEventTickPerSecond` as the original
   console timer code does, and `GetTickCount` (`TMR/sdl_timer.h`, for `RND.c` and
   `GLI_fastload.c`). Channels: 0 milliseconds, 1 and 2 SDL's performance counter. Checked:
   the engine creates only channel-1 timers (`Basic.c`, `GLD.c`) and the Direct3D 6 driver reads
   channel 1 through `TimerGetCounter`; channel 2 (the time stamp counter, its rate measured by a
   one-second busy wait in `TMR_pc_fn_ulCalibrateTscFreq` at init) is read by nothing in the exe.
   GOG's `GliOGLvf.dll` is unchecked. The calibration keeps the low 32 bits of the one-second delta,
   which wraps above about 4.29 GHz; with nothing reading the channel that has no effect, and the
   PC path stays as it is. Measured: regression identical (SDL, MSVC, clang-cl, VC5 + define);
   the SDL build starts without the one-second busy wait (Morb_00 run: menu at 1.2 s, level at
   3.6 s, plays); no SDL object imports
   `timeGetTime`, `GetTickCount`, `QueryPerformance*` or `timeBeginPeriod`.

   **Done (2026-11-24): MMG lock under `USE_SDL`.** `g_csMmg` takes the original engine's
   `TypeCriticalSection` (`MMG/MMG_alloc.h`): an SDL mutex, reentrant as the critical section.
   `MMG/CpaMmg.h` maps the Win32 critical section calls of the MMG files to it, as it maps their
   `GlobalAlloc` (step 3); the call sites stay. No other file that includes `CpaMmg.h` uses a
   critical section (`SND/SndThrd.c` keeps its own, Win32, see Sound). Measured: regression
   identical (SDL, MSVC, clang-cl, VC5 + define); only `SndThrd.obj` imports the critical section
   functions in the SDL build.

   **Done (2026-11-24): draw semaphore under `USE_SDL`.** An SDL semaphore (maximum count 1, as the
   Win32 one) created in `GAM/sdl_main.c`; `GAM/GAM_DrawSem.h` maps `WaitForSingleObject` and
   `ReleaseSemaphore` in its six users, which call them on no other object; `GAM_GameEng.c`'s own
   prototypes of the two are fenced. The renderer driver still gets it as `"DrawSem"` and does
   not wait on it (above). Measured: regression identical (SDL, MSVC, clang-cl, VC5 + define);
   in the SDL build only `SndThrd.obj` imports `WaitForSingleObject`, nothing
   `ReleaseSemaphore`/`CreateSemaphoreA`.
   Still Win32 in the SDL build outside SND (Windows-only for now, open for Linux): `Sleep` (seven
   files), `GetCurrentThreadId` for the MMG channel of the calling thread (`GLD.c`), the
   single-instance mutex `RAYMAN2EXE` (`sdl_main.c`), and `SetEvent` in `GLI_FatalExitOrSleep`,
   on a path taken only when an engine thread exists.

   **Done (2026-11-24): SDL window hidden until the device exists** (user decision). On PC the
   320x200 window is on screen while the driver starts (Win32 build: 328x237 incl. frame for 1.2 s;
   most of it the time stamp counter calibration). The SDL window is created hidden and shown when
   `fn_bCreateMainDisplayScreen` has succeeded. Measured: window log at startup shows no small
   window, the first visible one is the fullscreen window at 0.17 s; regression identical (SDL).

   **Focus test with the Direct3D 6 driver (2026-11-24)** (16-bpp mode, `DWM8And16BitMitigation`;
   Alt+Tab away at 12 s, back at 18 s, `WM_CLOSE` at 26 s, Morb_00). Original retail exe (install's
   `GliDX6vf.dll`), VC5 + define and the MSVC Win32 build (linked-in driver) behave alike: pause
   title and minimized on loss; on return the window comes back at 1920x1080 but stays white with
   the pause title (the "restoring data" title is never set), and `WM_CLOSE` is ignored: the
   window thread is blocked, the process has to be killed. Original behaviour on this system,
   not a fork regression. The SDL build with the same driver: pause on loss,
   "restoring data" then play on return, exit 0.2 s after `WM_CLOSE`.
   Where the Win32 build blocks (2026-11-24, cdb attached non-invasively to the hung MSVC Win32
   build, three runs alike): the window thread, handling the restore (`WM_SYSCOMMAND` ->
   `WM_WINDOWPOSCHANGED` -> `WM_SIZE`), is in DirectDraw's subclass window procedure, which waits
   without timeout for DirectDraw's exclusive-mode mutex (`__DDrawExclMode__`) before it puts the
   display mode back. The engine thread is paused in `GAM_pc_fn_vWaitForEitherEngineEvent`. The
   driver took exclusive mode on the engine thread (`GLI_xInitGli` -> `xInitDriver` ->
   `SetCooperativeLevel`, reached from `GAM_pc_fn_ulEngineThreadProc`), and `vActivateGli(hWnd, 0)`
   on focus loss only sets a flag, so the engine thread still holds the mutex: it waits for the
   window thread to resume it, the window thread waits for its mutex. A deadlock between the two
   threads; the SDL build has one thread, which already owns the mutex. The owner itself could not
   be read (the attach lacks the access to query the mutant); it follows from where
   `SetCooperativeLevel` runs. Not fixed: the SDL build does not have it, and a fix in the Win32
   path would change the original threading. Open: a `docs/BUGS.md` entry with a fix (release
   exclusive mode on the engine thread before it pauses) only if the user wants the Win32 path
   usable on this system.

   **Done (2026-11-24): focus return to fullscreen (SDL build).** The decompiled names on the
   activation path are inverted: on loss `GAM_pc_fn_vHandleActivateEnterFullScreen` ->
   `GLD_bSwapDeviceMode` -> the driver's `vActivateGli(hWnd, 0)`; on return
   `GAM_pc_fn_vHandleActivateExitToWindowed` -> `GLD_bSwapToWindowedMode` ->
   `vActivateGli(hWnd, 1)`, which in the Direct3D 6 driver recreates the exclusive fullscreen
   device. With that driver the SDL build already came back as it started: on this system (16 bpp
   under `DWM8And16BitMitigation`) the exclusive mode shows as a captioned 1920x1080 window from
   the start, in the Win32 build as well. GOG's OpenGL driver makes the window a popup itself
   (`SetWindowLongA`, measured style `0x96000000`); SDL 3.4 puts its own style back
   (`0x16CF0000`, caption and frame) whenever the window leaves the minimized state, in its
   `WM_WINDOWPOSCHANGED` handling, whichever call restores it (`SDL_RestoreWindow` or the system
   restore of `pc_winmain.c`). On focus loss, before minimizing, SDL is now told whether the
   driver left the window without a caption (`SDL_SetWindowBordered`); a borderless SDL window
   gets no non-client area. Measured: the OpenGL focus test returns fullscreen (screenshot,
   taskbar hidden); Direct3D 6 unchanged; regression identical (SDL).
5. **Done (2026-11-24): `INO` backend through SDL** (`INO/sdl_ino.c`, `INO/INO_sdl.h`,
   DEVIATIONS 16). The device layer of `INO/INO.c` stays shared (devices, historic records, key
   frames, demo readers); its DirectInput parts are fenced `#ifndef USE_SDL`, and `INO_sdl.h`
   maps their names for the shared code to the SDL functions, so the call sites stay. Keyboard:
   SDL's keyboard state through a `DIK_` -> SDL scancode table (the set-1 scan codes, as SDL's
   own Windows table), same active key list; AZERTY found as on PC (the key at Q's place gives
   'a'). Mouse: relative motion, wheel, four buttons. Gamepads: one joystick device per SDL
   gamepad present at init, laid out as DirectInput shows an XInput gamepad. As with
   DirectInput's foreground mode no device gives input while the window has no keyboard focus.
   The regression hooks still replace the readers (`port/regress.c`). Measured: regression
   identical (SDL; MSVC, clang-cl and VC5 + define with the fences); no SDL-build object
   references DirectInput or the keyboard layout functions; live (OpenGL, Morb_00): Esc opens the
   in-game menu, Down (an extended key) moves its selection. Not tested: mouse (enabled only by a
   data command) and gamepads (none attached).
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
| `FILE_MODE_C_WIN95` | feature | Win32 API for level data files, ini files and MMG memory; set in `SND/snddef.h` unless `GAMECONSOLE`, `USE_SDL` or `RAYMAN2_C_RUNTIME`; C runtime otherwise |
| `RAYMAN2_C_RUNTIME` | fork | takes the C runtime branches on Windows too, to check them against the regression run (CMake `R2_C_RUNTIME`) |

- New code goes beside the original branch: `#if defined(USE_SDL) ... #else /* original Win32 */
  ... #endif /* USE_SDL */`. The Win32 path needs no macro of its own.
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
- **Original engine style for backend code**, as it writes its platform code: one backend
  file per module beside the PC one, named after the backend as the PC files are after theirs
  (`GAM/pc_winmain.c` -> `GAM/sdl_main.c`, `INO` reader beside `INO_pc_Read*`); functions keep
  the module prefix with the backend in place of `pc` (`INO_sdl_...`, `GAM_sdl_...`), Hungarian
  type prefixes, `/* */` comments, file headers and description blocks as in the fork's other
  files; the whole file fenced `#ifdef USE_SDL ... #endif /* USE_SDL */`, its PC counterpart
  `#ifndef USE_SDL` with the original code untouched.
- To check at the first build: SDL3 from vcpkg for 32-bit x86 Windows.

**Open decisions.**
- **64-bit data (decided: in scope, 2026-11-24).** `WIN64` and `OSX` need it. The level files store
  32-bit pointers, so every pointer-carrying structure loaded from level data has to be widened at
  load. How (per-type load tables or an offline converter, as for big-endian data) is open; decide
  it with the big-endian question, from Phase 4's portability list.
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
