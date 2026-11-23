# Rayman2.exe architecture

How the PC `Rayman2.exe` is put together, and how this decompilation rebuilds it. Exported verbatim
to the public repository (`docs/architecture.md`), so it names no private paths or reference material.

Source paths in sections 1-4 are relative to `Rayman2.exe/src/`. Hex numbers next to a function
are its retail address.

## 1. Module map

The engine is split into modules named by a three-letter prefix. Each directory under `src/` is one
module, and its functions and globals carry the prefix (`GAM_fn_vEngine`, `MMG_fn_vInit`). Names
with a `pc_` infix are code that exists only in the PC build (window, threads, copy protection).
"Functions" counts the functions written so far (`Rayman2.exe/status.json`).

| Module | Owns | Main entry points | Functions |
|---|---|---|---|
| GAM | Game engine: WinMain, window, engine thread, engine modes, level loading, per-object update, the object "mini-structures" (`GAM/Actions/`: Brain, Dynam, 3dData, StdGame, CollSet, MSSound, ...), animation player (`GAM/PlayAnim/`) | `GAM_pc_fn_WinMain`, `fn_uiEngineMainLoop`, `GAM_fn_vEngine`, `fn_vInitLevelLoop` | 760 |
| AI | Script AI: tree interpreter (`Intell.c`), conditions, operators, functions, procedures, fields, camera behaviour (`CAM_*.c`), waypoints and graphs (`WParc.c`, `WPgraphe.c`) | `fn_bDoIntelligencePerso` (via `fn_vMakeCharacterThink`) | 703 |
| SND | Sound system and the ~110 entry points exported to the sound DLLs | `SND_fn_iInitSound`, `SND_fn_vSynchroSound` | 557 |
| GLI | Graphics interface: renderer DLL loading, device and viewport layer (`GLD.c`), cameras, materials, textures, matrix stack, lights, CD file loader | `GLI_xInitGli`, `GLD_bFlipDeviceWithSynchro`, `GLI_xSendListToViewport` | 347 |
| MEC | Mechanics: base, camera, collision and platform mechanics | `MEC_p_stDynamicsBaseMechanics` | 118 |
| MMG | Memory manager (channels, blocks, pools), SNA file reader and relocation (`SNA.c`, `SNA_reloc.c`), PC copy protection (`MMG_init.c`, `pc_protect.c`) | `MMG_fn_vInit`, `Mmg_fn_vFirstInitMmgModule`, `SNA_pc_vLoadAndRelocate` | 91 |
| IPT | Input: entry elements, keyword analysis, history, save | `IPT_fn_vReadInput`, `fn_vEngineReadInput` | 86 |
| GEO | Geometric objects and elements | `GEO_fn_vMorphAddInMainLoop` | 81 |
| SAI | Save-and-init: registers values and restores them at fixed moments (new game, level loaded, save loaded) | `SAI_fn_vFirstInit`, `SAI_fn_bInitAllWhen` | 69 |
| HIE | Super-object hierarchy, global matrices, matrix stack, hierarchy draw and picking | `HIE_fn_vPushMatrix`, `HIE_fn_vInvalidateAllGlobalMatrices` | 68 |
| AGO | HUD gauges and 2D overlays | `AGO_vInit`, `AGO_vDisplayGAUGES` | 52 |
| INO | Low-level input devices (DirectInput) | `INO_fn_wReadAllDevices` | 45 |
| FIL | Files: data directories, concatenated files | `FUN_00492770` (data directory) | 42 |
| MGT | Magnets | `MGT_fn_vTreatMagnets` | 39 |
| COL | Collision between geometric objects, octrees | `COL_fn_bDetectCollisionStaticGeomObjWithStaticGeomObj` | 34 |
| POS | Position matrices | `POS_fn_vSetIdentityMatrix`, `POS_fn_vGetTranslationVector` | 30 |
| INT | Segment and volume intersection tests | `INT_fn_bDetectIntersectSegmentWithBox` | 29 |
| SCT | Sectors: sector lists, sector draw, mirrors | `SCT_fn_vSendSectorWhereIAmToViewportWithMirror` | 28 |
| GMT | Game materials | `GMT_fn_hCreateGameMaterial` | 17 |
| MOR | Vertex morphing | `MOR_fn_vDoMorph` | 15 |
| MTH | Math: fast lookup tables (most math is header macros) | `MTH_fn_vInit` | 11 |
| ERM | Error manager | `ERM_fn_vUpdateLastError` | 9 |
| PRT | Particles | `PRT_fn_bComputeParticlesGeneration` | 9 |
| RND | Random tables | `RND_fn_vFirstInitRandom` | 7 |
| TMR | Timers | `TMR_fn_wStartTimer`, `TMR_fn_wReadTimer` | 7 |
| DNM | Dynamics objects | `fn_vDynamAlloc` | 6 |
| FON | Fonts and text | | 3 |
| PO, IPO, PCS, PIC, SPO | Small modules: physical objects, their instances, physical collide sets, picking, superimposed objects | | 0-2 each |
| ALW | Storage of the "always" objects (`g_stAlways`) | | 0 |

`src/main.c` is the exe entry point. `src/standalone/` holds generated storage for globals that
have no other owner yet.

**Dependency shape.** GAM sits on top and calls into nearly every module. Below it are three
chains:
- **Objects:** GAM → AI (thinking) and MEC/DNM (mechanics) → COL, INT, GEO for collision → POS
  and MTH.
- **Rendering:** GAM → SCT (sectors) and HIE (hierarchy) → GLI → the renderer DLL that
  `GLI_xInitGli` drives through function pointers.
- **Loading:** GAM → MMG's SNA reader → the memory manager.

MTH, POS, ERM and MMG form the bottom layer that most modules include. SND depends only on ERM
and MMG. It talks to the rest of the game through its exports and through
`SND_fn_vSynchroSound`, which GAM calls once per frame.

## 2. Boot path

**Two threads.** `WinMain` in `src/main.c` only forwards to `GAM_pc_fn_WinMain`
(`GAM/pc_winmain.c`, 0x00401D40). That thread owns the window and the message loop. The engine
runs in a second thread.

**`GAM_pc_fn_WinMain` in order:**
1. Build `<Windows dir>\UbiSoft\ubi.ini`, creating the directory, and a default file
   (`[Rayman2] Choose = 1`) when it is missing.
2. Allow one instance only: return if `hPrevInstance` is set, or if the mutex `RAYMAN2EXE`
   already exists.
3. `GAM_pc_fn_bInitDisplayAndSync`: probe the display bit depth and create the Win32
   synchronisation objects (draw semaphore and engine events).
4. Store the instance handle and main thread id, copy the command line, `SetErrorMode`.
5. `MMG_fn_vInit(1)`: seed the LCG random pool from `QueryPerformanceCounter`, allocate the
   memory-manager table, and resolve the protection's private imports
   (`MMG_pc_fn_vResolveModuleApi`).
6. `fn_vInitEngineWhenInitApplication` (`GAM/GAM_init.c`, 0x00401000): error channels,
   `Mmg_fn_vFirstInitMmgModule(10)`, `GMT_fn_vInitFor3DOS`, then `FUN_00403690`, which:
   - reads `Ubi.ini` (`FUN_00405a30`: GLI DLL, driver, device, mode, texture memory, particle rate);
   - loads the GLI renderer DLL (`FUN_00405850` → `GLI_fnl_InitDll`; on failure a message box and
     exit);
   - sets the engine mode to `E_EM_ModeStartingProgram`;
   - initialises input (`IPT_fn_vFirstInitInput`), random tables (`RND_fn_vFirstInitRandom`),
     SAI (`SAI_fn_vFirstInit`), and the AI and game memory (`fn_vFirstInitAIMemory`,
     `fn_vFirstInitGameMemory`).
7. `GAM_fn_bCreateMainWindow`: register class `Rayman2` with `GAM_fn_WndProc`, create the window,
   title "Rayman II", then `fn_bCreateMainDisplayScreen`.
8. Turn the screensaver off, `GAM_pc_fn_vCreateEngineThread` (`CreateThread` on
   `GAM_pc_fn_ulEngineThreadProc` → `fn_uiEngineMainLoop`), then spin on
   `GAM_pc_fn_ulGetQuitFlag` until the engine thread sets that flag.
9. The message loop (`GetMessageA`, with `FUN_00402580` as the pre-translate hook), then shutdown:
   `GAM_pc_fn_vKillEngineThreadAfterExit`, restore the screensaver, `FUN_00401b20`,
   `GAM_pc_fn_vFreeMmgTableAndHelperDll`.

**First engine init.** The engine thread starts in mode `E_EM_ModeStartingProgram`, so the first
`GAM_fn_vChooseTheGoodInit` (`GAM/GAM_engine.c`) calls `fn_vFirstInitEngine`. In order, that
function:
- sets the texture file to `\world\levels\fix.ptx`;
- initialises the math tables (`MTH_fn_vInit`) and the engine clock;
- creates the 3D device (`GLI_xInitGli` calls the DLL's `xInitDriver` with the window handle and
  display mode) and the game viewports (`GAM_fn_vCreateGameDeviceAndViewPorts`);
- initialises sound (`SND_fn_vSetSoftDirectory`, `SND_fn_iInitSound`, `SND_fn_bInitDataLoadSnd`),
  then animation sound and micros;
- loads the fix data (`GAM_pc_fn_vLoadFixStreams`, section 4) and the game options;
- creates the animation heaps (`PLA_fn_vCreateAndInitHeaps`) and the matrix stack.

The mode then becomes `E_EM_ModeEnterGame`. `fn_vInitGameLoop` takes the next level name if one
is set, else the first level name, and calls `fn_vInitLevelLoop`, which loads it (section 4).

**Copy protection.** The checks live in MMG, SNA and a few AI functions. Several of these
functions open with a gate on `SNA_g_lProtectionGate`: when it is set, they only record two code
addresses and return. `tools/crack.py` reproduces the GOG crack on our layout. Its tables list
each change with its reason:

| Where | What the original does | What the crack changes |
|---|---|---|
| `MMG_pc_fn_vResolveModuleApi`, `MMG_pc_fn_vInitProcessEnumeration`, `MMG_pc_fn_vResolveAllImports`, `MMG_pc_fn_vLoadAllModuleHandles` (`MMG/MMG_init.c`) | Resolve private imports by walking module export tables (PSAPI or Toolhelp) | Entry becomes `ret` |
| `FUN_00448e30` (`MMG/MMG_init.c`) | CD check | Stores `'.'` in `g_cCDROM` (CD drive = current directory) and returns |
| SNA file I/O (`SNA_pc_vInitStream`, `SNA_pc_bOpenFile`, `SNA_pc_ulSeek`, the raw readers, `SNA_pc_fn_bReadFile`, ...) | Every Win32 file call goes through a 15-byte stub written into an LCG-picked slot of a code pool, called, then overwritten with random bytes | Direct import calls |
| `SNA_pc_vVerifyCdLevelsFile` (`MMG/SNA_reloc.c`) | Opens `<CD>:\<data>\LEVELS0.DAT` at fix load and stores six check values from its header | Path without the drive letter, direct calls |
| `SNA_pc_bOpenEmbeddedSna`, `SNA_pc_fn_hOpenFileAtSinusOffset` | Find a level's relocation tables inside `LEVELS0.DAT` (section 4) | Direct calls |
| `MMG_pc_fn_vFillTamperApiTable` (`MMG/pc_protect.c`) | Fills the API table used by the level-load tamper block | Points it at the crack's fake CD file system |
| `fn_p_stMiscProtectProcedure` (`AI/Procedure.c`), `fn_p_stComputeXor`, `AI_pc_fn_p_stComputeNotU` (`AI/Function.c`) | AI integrity probes that hash the running exe image into script variables | Read the crack's untouched copy of the image (`.extra`) instead |
| `GAM_pc_fn_WinMain`, `FUN_00405a30`, `GLI_pc_fn_vReadIniString`, `GLI_pc_fn_vWriteIniString` | `ubi.ini` in the Windows directory | `.\Ubi.ini` (current directory) |
| `GAM_fn_WndProc` | `WM_USER` → `GAM_pc_fn_vDispatchWmUser` | That call is NOPed |

The C source is the uncracked original. The crack is applied after linking, to a copy of the exe.
A few crack runs that are not needed to boot are not translated yet; `tools/crack.py` lists them.

## 3. Main loop

**Thread loop.** `fn_uiEngineMainLoop` (`GAM/pc_winmain.c`, 0x00401210) is the body of the
engine thread:

```
while (!end of game) {                      FUN_00401100
    GAM_pc_fn_vChooseTheGoodInit();         init for the current engine mode
    while (!end of step)                    FUN_00401130
        FUN_00401160();                     one frame (pause handshake + fn_vOneTrame)
    GAM_pc_fn_vChooseTheGoodDesInit();      de-init for the current engine mode
}
GAM_pc_fn_vEngineThreadExit();              never returns
```

`FUN_00401160` first checks the engine events. When the window thread has paused the engine
(for example on focus loss), it signals `ISPAUSEDEVT`, calls `fn_vSetEngineInPaused`, blocks until
the frame or exit event fires, then calls `fn_vResetEngineFromPaused`. After that it runs one
frame.

**One frame.** `fn_vOneTrame` (0x004049E0) calls four function pointers in order:

| Slot | Set to | Does |
|---|---|---|
| `p_fn_vNetEngine` | empty | network hook (unused) |
| `p_fn_vEngine` | `GAM_fn_vEngine` | update and draw |
| `p_fn_vDisplayAll` | `fn_vDisplayAll` | send to the screen |
| `p_fn_vReadInput` | `fn_vEngineReadInput` | input for the next frame |

`GAM_fn_vChooseTheGoodInit` installs `GAM_fn_vEngine` and `fn_vDisplayAll` when it enters a game
or a level. Until then they are empty stubs.

**`GAM_fn_vEngine`** (`GAM/GAM_GameEng.c`, 0x0040ADA0):
1. **Sound:** `SND_fn_vSynchroSound`.
2. **World update**, skipped while the engine is paused:
   - `HIE_fn_vInvalidateAllGlobalMatrices`, main-character swap, `fn_vUpdateDynamicHierarchy`,
     `fn_vKillAllAlwaysInNonActiveSectors`;
   - magnets (`MGT_fn_vTreatMagnets`);
   - `fn_vTreatDynamicHierarchyOfPlatForms`, then `fn_vTreatDynamicHierarchy`. This walks the
     dynamic world and calls `fn_vTreatCharactersInSector` for each active actor:
     - **animation:** `PLA_fn_bDoAnimPlayerForCharacter`, unless the frame is skipped;
     - **AI:** `fn_vMakeCharacterReact` → `fn_vMakeCharacterThink` (reflex, then the
       intelligence script);
     - **mechanics:** `fn_vMakeCharacterMechanicallyReact`, which calls the dynamics function for
       the actor's mechanics type (`MEC_p_stDynamicsBaseMechanics` or
       `DNM_p_stDynamicsCameraMechanics`);
     - then particles (`fn_vMSPrtSrcSourceGenerateParticles`), lights (`fn_vUpdateLight`) and
       the sector change test.

     The actor's custom bits can switch off AI, mechanics or animation, or switch them off only
     when the actor is too far from the main character.
   - Cameras (`CAM_fn_vCameraManagement`) and cycling textures.
3. **Frame sync:** wait on the draw semaphore, flip (`GLD_bFlipDeviceWithSynchro`), advance the
   engine clock (`fn_vActualizeEngineClock`), clear the viewport.
4. **Render:** `GLI_pc_fn_vBeginScene`, `GLD_bRequestWriteToViewport`, morphs
   (`GEO_fn_vMorphAddInMainLoop`), then from the active camera's sector
   `SCT_fn_vSendSectorWhereIAmToViewportWithMirror`. Last comes the mirror effect
   (`GLI_pc_fn_vDisplayMagicMirror`) or the shadow quad (`GLI_pc_fn_bSubmitAngleQuad`).

**`fn_vDisplayAll`**: `GLI_xSendListToViewport`, `GLD_bWriteToViewportFinished`, the fix
viewport overlay (`fn_vDisplayFix`), release of the draw semaphore, `GLI_pc_fn_vEndScene`.

**`fn_vEngineReadInput`** (`IPT/IPT_Anal.c`): in normal input mode, `IPT_fn_vReadInput` (reads
all devices through INO), then `fn_vAnalyse3DOSSpecialInputActions`.

**Engine modes** (`GAM_g_stEngineStructure.eEngineMode`, enum in `GAM_types.h`). The
init/de-init pair around each run of frames moves between them:

| Mode | Init (`GAM_fn_vChooseTheGoodInit`) | De-init (`fn_vChooseTheGoodDesInit`) |
|---|---|---|
| 1 StartingProgram | `fn_vFirstInitEngine`, then falls through to EnterGame | |
| 2 StoppingProgram | | `fn_vDesInitLevelLoop`, engine shutdown |
| 3 EnterGame | `fn_vInitGameLoop` (loads the first level) → Playing | |
| 5 EnterLevel | `fn_vInitLevelLoop` → Playing | |
| 6 ChangeLevel | | optional save (`fn_bPlayerSaveGameAll`, `fn_bLevelSaveGameAll`), `fn_vDesInitLevelLoop`, next level name → EnterLevel |
| 7 DeadLoop | `fn_vInitDeadLoop` → Playing | |
| 8 PlayerDead | | `fn_vDesInitDeadLoop` → DeadLoop |
| 9 Playing | (frames run) | |

A level change is therefore one ChangeLevel de-init followed by one EnterLevel init. The pause
is not a mode. It is the event handshake above, together with `fn_bIsEngineInPause`, which makes
`GAM_fn_vEngine` skip the world update.

## 4. Data and memory

**Files.** Paths are built from the data directory (`FUN_00492770`):

| File | Holds | Loaded by |
|---|---|---|
| `Fix.sna`, `Fix.gpt`, `Fix.ptx`, `Fix.snd` | Data shared by all levels | `GAM_pc_fn_vLoadFixStreams` (`GAM/GAM_engine.c`) |
| `<lvl>\<lvl>.sna` | The level's memory blocks | `SNA_pc_vLoadAndRelocate` |
| `<lvl>\<lvl>.gpt` | Global pointers: engine structure, object types, always-models, animation stacks, main character | `fn_vInitLevelLoop` through `SNA_pc_vOpenGptFile` / `SNA_pc_ulReadAndRelocateLong` |
| `<lvl>\<lvl>.ptx` | Textures | `FUN_00423610` sets it as the texture file; read during `FUN_00421820` (table kind 3) |
| `<lvl>\<lvl>.snd` | Sound event fix-ups | `SNA_pc_vOpenSndFile`, `SNA_pc_vReadAndRelocateSnd` |
| `.rtb` `.rtp` `.rts` `.rtt` `.rtl` | Relocation tables for the `.sna`, `.gpt`, `.snd` and `.ptx` loads (table kinds 0-3) and kind 4 | `SNA_pc_pReadGptTable` |
| `LEVELS0.DAT` | The levels' relocation tables, and the CD check header | `SNA_pc_bOpenEmbeddedSna`, `SNA_pc_vVerifyCdLevelsFile` |

**SNA stream.** `SNA_fn_ulFRead` (`MMG/SNA.c`, 0x00450D90) reads every one of these files.
Encoded streams are XORed with a Park-Miller keystream (multiplier 16807, modulus 2^31-1). For a
`.sna` file the 4-byte header is the seed, `0x6AB5CC79`. The reader keeps a running checksum of
the data.

**Relocation.** A snapshot file stores memory blocks with the pointers they held when the file
was written.
- `SNA_pc_vLoadAndRelocate` (`MMG/SNA_reloc.c`) reads each block that the static load lists
  select into the MMG block with the same module and block number. It records the shift between
  the block's old base (stored in the file) and its new one. Blocks the lists do not select are
  skipped.
- The block's relocation table then gives the pointer slots inside it. Each slot is an offset
  plus the target block's module and block number, and the target block's shift is added to the
  pointer.
- `.gpt` reads work slot by slot: `SNA_pc_ulReadAndRelocateLong` reads one dword and adds the
  shift if the current slot covers it.
- Fix tables are loose files next to `Fix.sna`. A level's tables are found in `LEVELS0.DAT`, at
  an offset computed from the table selector (`SNA_pc_fn_hOpenFileAtSinusOffset`). The selector
  starts as the level's index in the level-name table. Between loads
  the table selector is rekeyed (`gam_drm_rekey_reloc_table` in `GAM/GAM_levelload.c`).
- If the stream checksum differs from the table header, one table state word is flipped.
  Protection actors later detect the flip, which shows "CD missing" in the game.

**Level load order** (`fn_vInitLevelLoop`, `GAM/GAM_levelload.c`, 0x00403730):
1. `.ptx` textures.
2. `.sna` blocks.
3. `.gpt`, which provides:
   - the fix-object links and always-models;
   - the object-type lists;
   - the whole `GAM_g_stEngineStructure`, keeping mode, level names and timer;
   - the main character and the animation stacks.
4. `.snd`.
5. Object init (`fn_vInitAllObjectsWhenMapJustLoaded`) and cameras (`CAM_fn_vInitCameras`).
6. Saved game, if any (`fn_bLoadLevelSaveGameAfterLoadLevel`, then `SAI_fn_bInitAllWhen`).
7. Map music, dynamics table (`InitArrayOfDynamicFunction`), input reset, `AGO_vInit`.
8. The language from `Ubi.ini`.

**Memory manager (MMG).**
- **Channels:** `Mmg_fn_vFirstInitMmgModule(10)` allocates the channel tables with
  `GlobalAlloc` and creates the global lock `g_csMmg`. Every allocation names a channel, and
  `Mmg_fn_ucGiveChannelId` hands one out per thread.
- **Blocks:** each module registers blocks (`Mmg_fn_v_InitMmg`, `g_a_p_stMmgModuleBlocksInfo`),
  and `Mmg_fn_v_SetModeAlloc` chooses which block the next allocations come from.
- **Allocator families** (`MMG/MMG_pool_alloc.c`): `fn_p_vStaAlloc`/`fn_vStaFree` (static, inside
  a block), `fn_p_vDynAlloc`/`fn_vDynFree` (dynamic) and `fn_p_vGenAlloc`/`fn_vGenFree` (general
  heap). Each has a `Realloc` in `MMG/MMG_alloc.c`. Blocks carry an overflow guard word.
- **Fix and level memory:** SAI sets up separate fix and level static memory
  (`SAI_fn_vInitFixMemory`, `SAI_fn_vInitLevelMemory` in `SAI/SAI_main.c`).

## 5. Toolchain and link

**Compiler.** Retail was built with MSVC 5.0 SP3 (`cl` 11.00.7022) and LINK 5.10, generating x87
floating-point code. Every TU gets `/O2 /Gy /Dinline=__inline /DRAYMAN2_STANDALONE`, plus the TU's
own flags from `Rayman2.exe/tu.json`: for example `/G6` (Pentium Pro scheduling) or `/MD` (several
SND files). One TU, `SND/sndreal.c`, was compiled with a Visual C++ 6.0 compiler (`vc6-8168` in
`tu.json`). Retail compiled each TU once with one flag set, so a file whose functions need
different flags is really several retail TUs and gets split.

**Layout.** `/Gy` puts every function in its own COMDAT, and the linker places them in object
order. The link order in `tu.json` therefore decides the address of every function. It does not
equal retail's yet, so each function is compared on its own (section 6). `/OPT:NOREF` keeps every
function. `/OPT:ICF` folds identical bodies the way retail did, which is why several functions share
one address (empty stubs, trivial getters).

**Libraries.** The C runtime is imported from `MSVCRT.dll` only (`/NODEFAULTLIB:libc.lib`,
`msvcrt.lib`). Objects built without `/MD` reach it through linker thunks (`jmp [__imp__ftol]`).
`DINPUT.LIB` and `DXGUID.LIB` (the DirectX 5 era versions) come from Visual C++ 6.0, because VC5
ships neither.

**Exports.** The exe exports ~110 sound-system entry points (`Rayman2.exe/exports.txt`). The
sound plugin DLLs fetch them from the exe with `GetProcAddress`.

**Resources.** `.rsrc` is taken from the GOG exe at build time. It holds the icons and GOG's
DPI-aware manifest; without the manifest Windows stretches the window. The modern build links with
`/MANIFEST:NO`, since the linker's default manifest would otherwise replace it.

**Source file names.** Retail embedded the original build's file names (`__FILE__`) in a few memory
logging calls. Those TUs set them with `#line N "name"`. Otherwise the exe would contain the path it
was built in, and builds would differ between machines.

**The GOG exe.** It is the original image with a crack applied on top. Its `.extra` section keeps
an untouched copy of the original `.text`, and the byte runs where `.text` and `.extra` differ are
the crack's edits. `.extra2` holds the crack's virtual CD file table. The tools rebuild the original
image from the GOG exe and compare our build against it. `tools/crack.py` reproduces each crack
run at the owning function in our layout (entry `ret`, call NOPs, offset patches, or the cracked
body transplanted into a `.crack` section) and produces a runnable copy.

## 6. Status and method

**Goal.** C source that rebuilds a byte-identical `Rayman2.exe` with the retail toolchain.

**Verdicts** (`Rayman2.exe/status.json`, one row per function: retail address, size, source file):

| Verdict | Meaning |
|---|---|
| EXACT | Our compiled bytes equal retail's, with relocated operands (call targets, global addresses) masked |
| MISMATCH | The function is written, but its bytes are not yet identical to retail's |

Relocated operands are masked because the global layout does not match yet. Once it does, the exe
as a whole is compared.

**Conventions.**
- A `/* 0x00XXXXXX */` comment gives the retail address of the function or global next to it.
- Retail bugs are kept. Fixing them is work for a fork, because a fix changes bytes.
- Globals are real storage and callees are C symbols. No code uses a raw retail address.

**Working on a function.**
```
python tools/byte_match.py NAME --asm          # diff it, side by side
python tools/byte_match.py NAME --flags G6     # try other TU flags
python tools/build.py && python tools/check.py # whole exe: regressions and new matches
```
Typical causes of a MISMATCH that can be fixed from C: statement shape (whole-struct copy against
field-by-field copy, loop form), branch order, the type or signedness of a field, a cached pointer
against a re-read global, and the compiler flags of the TU. Register allocation can also change when
the declarations above the function in the same TU change.
