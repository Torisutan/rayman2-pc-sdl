# Sound: the exe and its driver DLLs

The boundary between `Rayman2.exe`'s `SND` module and the sound drivers it loads, measured on the
retail files and the decompiled sources (26.11.2026, fork `main` at 4519296, upstream b102d4d).
Input for PLAN "Sound" steps 2 to 4: linking the drivers into the exe as the Direct3D 6 driver was
(DEVIATIONS 13), then platform branches inside them.

## 1. Which drivers load

`SND_pc_fn_vLoadDriversFromIni` (`SND/SND.c`, line ~3065): `DLL/Snd_Cpa.ini` `[Options]`
switches each kind on (`WAV_ON_OFF`, `ADPCM_ON_OFF`, `MPEG_ON_OFF`, `CD_ON_OFF`, `RECORD_ON_OFF`,
`SUPERVISATER_ON_OFF`; `Sound_ON_OFF=OFF` switches all off). For each kind that is on,
`SND_pc_fn_hLoadDriverDll(section)` reads that section: `Unconditionnal=NAME` loads `NAME` directly;
otherwise `Test0`, `Test1`, ... name test functions, looked up in the exe and in the modules listed
under `[DLL_Test]`, and the first test that returns true selects `File<n>`; else `Default`. The file
loaded is `DLL\<NAME>bvr.dll` (the suffix is always `bvr`; the `*SVR.dll` files of the install are
never named).

The install's `Snd_Cpa.ini`:

| Kind | Section | Loaded | Handle (exe) |
|---|---|---|---|
| samples, 3D, mixer, output (`WAV_ON_OFF=ON`) | `[DLL_Sxd]` | `WAVMWbvr.dll` if `_SND_fn_bTestSnd_MMX@0` (exe) is true, else `WAVPWbvr.dll` | `SND_pc_hModuleSxd` |
| `.apm` ADPCM streams (`ADPCM_ON_OFF=ON`) | `[DLL_Adpcm]` | `APMMXbvr.dll` if MMX, else `APMPWbvr.dll` | `SND_pc_hModuleAdpcm` |
| MPEG (`MPEG_ON_OFF=ON`) | none | **nothing**: no `[DLL_MPEG]` section, every key reads `None`, the handle stays `NULL` and the exe takes its no-op defaults | `SND_pc_hModuleMpeg` |
| CD audio, MIDI (`CD_ON_OFF=OFF`) | `[DLL_Cd]` | nothing | `SND_pc_hModuleCd` |
| recording (no key) | `[DLL_Record]` | nothing | `SND_pc_hModuleRecord` |
| sound debugger (`SUPERVISATER_ON_OFF=OFF`) | fixed name `SNDdbgV.DLL` | nothing | `SND_pc_hModuleDbg` |

So two drivers run in the game: `WAVMWBVR` and `APMmxBVR` (both decompiled, upstream folders of the
same names). The MPEG drivers (`MPGMXBVR.dll`, `MPGP5BVR.dll`) are never loaded.

## 2. Who calls whom

```
Rayman2.exe ──GetProcAddress──> WAVMWBVR (69 of its 72 exports; table filled in SND_pc_fn_vLoadSxdProcs)
     │   └──GetProcAddress──> APMmxBVR (1 export: _SND_fn_bCanFreeDataADPCM@0)
     │
     ├── WAVMWBVR ──GetProcAddress──> Rayman2.exe (callbacks) and ──> APMmxBVR (its 59 ...SxdADPCM exports)
     └── APMmxBVR ──GetProcAddress──> Rayman2.exe (callbacks) and ──> WAVMWBVR (Sxd functions)
```

- No static imports between them: neither driver's import table names the exe or the other driver.
  Every call goes through `GetProcAddress` with the decorated `__stdcall` name (`_SND_fn_...@n`).
- **The exe's side** (`SND/SND.c`): `SND_pc_fn_pfnGetDriverProc` wraps `GetProcAddress`;
  `SND_pc_fn_vLoadSxdProcs` (63 names), `SND_pc_fn_vLoadMidiCdProcs` (51, CD and MIDI: not loaded),
  `SND_pc_fn_vLoadRecordProcs` (11, not loaded), `SND_pc_fn_vLoadDbgProcs` (28, not loaded),
  `SND_pc_fn_vLoadCanFreeDataMPEG` / `...ADPCM` (1 each). A missing name falls back to a no-op
  default in the exe (`SND_pc_fn_*Default`). The `.apm` streams are therefore played by the WAV
  driver, which forwards to the ADPCM driver; the exe never calls the ADPCM driver's play functions.
- **The drivers' side**: one source, `WAVMWBVR.dll/src/snddll.c`, built into both drivers (the
  APM build reads it with `APMmxBVR.dll/include` first). It declares 295 stubs with the exe's and the
  other driver's names (`SND_M_DllFunction` / `SND_M_DllProcedure`): each stub calls a function
  pointer, filled by `SND_DllInitEntryPoints_*(hModule)` with `GetProcAddress`, or a dummy. The
  module handles come from the exe: `SND_DllInitEntryPoints_StaticFunctions(pInitStruct->
  hProcessInstance)` binds the exe's callbacks, among them `SND_fn_vGetHModuleWav/Adpcm/Mpeg/Cd/
  Record/Dbg`, which hand out the other drivers' handles. `SND_fn_iInitSxdADPCM` (APM) binds the exe,
  then the WAV driver (`SND_pc_DllInitEntryPoints_SxdFunctions(SND_pc_hWavModule)`).
- **The exe's exports** (110): 93 of them are names in the drivers' lookup tables (both drivers
  carry the same set, from `snddll.c`); 17 are named by no shipped driver
  (`SND_PrintUsedStaticMemory`, `SND_fn_bInitDataLoadSnd`, `_SND_fn_bEnterCriticalSectionForDriverThreadSnd@0`,
  `_SND_fn_bGetCurrentDirectory@8`, `_SND_fn_bTryToEnterCriticalSectionThreadSnd@0`,
  `_SND_fn_rPseudoNormeRealSnd@12`, `_SND_fn_rSqrtRealSnd@4`, `_SND_fn_rtSndRealToSndRealTwin@8`,
  `_SND_fn_vDesInitErrorSnd@0`, `_SND_fn_vEnterCriticalSectionForErrorDisplay@0`,
  `_SND_fn_vInitErrorSnd@4`, `_SND_fn_vQuitCriticalSectionForErrorDisplay@0`,
  `_SND_fn_vSndRealTwinToSndReal@12`, and the data `g_a_stSndBlocksInfo`, `g_ucSndModuleId`,
  `gcSimulateNotU64`, `gcSimulateRelease`). Counted as names present in each binary; which ones a
  run binds depends on the `SND_DllInitEntryPoints_*` functions it calls.

## 3. Data handed over

- `SND_tdstInitStruct` (`snddef.h`): `hProcessInstance` (the exe's module, used as the
  `GetProcAddress` handle), `hMainWindow`, `hMutexDX`; `int bidon` on `GAMECONSOLE`/`GENERIC` targets
  (the original's console form).
- Files: neither driver imports a file call (`CreateFile`, `ReadFile`, ...). Every read goes through
  the exe's `SND_fn_hOpenFileReadSnd`, `SND_fn_dwReadFileSnd`, `SND_fn_dwSeekFileSnd`,
  `SND_fn_dwFileSndLength`, `SND_fn_vCloseFileSnd`, `SND_fn_bLoadDataInMem[AndSwap]`
  (`SND/sndfile.c`): the drivers hold the exe's `tdhSndFile` handles (Win32 `HANDLE`s today).
- Memory: the drivers allocate through the exe's `SND_fn_pvMallocSnd[Ex]`, `SND_fn_vFreeSnd[Ex]`
  (MMG blocks), besides their own static C runtime (both are built `/MT`: LIBCMT inside each DLL).
- Banks and events: `SND_fn_pGetBinEvent`, `SND_fn_pGetBinRes`, `SND_fn_bLoadBinaryBank`, ... (exe).

## 4. Threads and locks

| Side | What | Where |
|---|---|---|
| WAV driver | output thread: `CreateThread(FUN_100093c0)`, priority above normal, a message loop (`GetMessageA`); commands posted with `PostThreadMessageA` (registered messages for init, de-init, take/release the device) and answered through an event | `WAVMWBVR.dll/src/sndwave.c` ~997-1511 |
| WAV driver | `waveOutOpen(..., CALLBACK_THREAD)` to that thread, `waveOutPrepareHeader`/`Write`/`Reset`/`Close`; mixer controls (`mixer*`, 9 calls) | `sndwave.c` ~839 |
| WAV driver | sample mutex `SND_pc_hSampleMutex` (`CreateMutexA`, `WaitForSingleObject` around voice changes), transition mutex `SND_pc_hTransMutex` | `sndwave.c` ~967, 1727-1816; `Win95/pc_sndtrans.c` ~62, 357 |
| WAV driver | the exe's sound lock through callbacks: `SND_fn_vEnterCriticalSectionThreadSnd_`, `SND_fn_vQuitCriticalSectionThreadSnd`, `SND_fn_vWaitForValueThreadSnd`, `SND_fn_vInitThreadSnd`/`DesInit` | `snddll.c` stubs |
| ADPCM driver | no threads of its own, no `CreateThread`/`waveOut` imports: it decodes on the WAV driver's thread through the Sxd functions | `APMmxBVR.dll/src` |
| exe | sound mutex `SND_pc_g_hSoundMutex` (`CreateMutexA`, owner at creation), error-display critical section and busy event | `SND/SndThrd.c` 128-129, 257 |
| exe | sequencer callback thread (`CreateThread`) | `SND/sndseq.c` 386, 428 |

## 5. Name clashes when linked into one image

Measured: the defined external symbols of the VC5 reference build's objects (exe, 225 objects), of
`tools/build_wavmw.py` and of `tools/build_apmmx.py` (driver objects), intersected (`llvm-nm
--defined-only --extern-only`):

| Pair | Common names | From |
|---|---|---|
| exe and WAVMWBVR | 132 | `snddll.c` 101 (the exe callbacks' stubs), `Sndxd.c` 30 (the exe has same-named Sxd wrappers that call through its function table), `sndwave.c` 2, `pc_mixervol.c` 1 |
| exe and APMmxBVR | 128 | `snddll.c` 127, `pc_sndxdadpcm.c` 1 (`_SND_fn_bCanFreeDataADPCM@0`) |
| WAVMWBVR and APMmxBVR | 384 | `snddll.c` 325 (same source in both), `pc_sndxdadpcm.c` 59 (the ADPCM exports against the WAV driver's stubs of the same names) |

String-literal COMDATs (`??_C@...`) are among them and merge; the rest are real duplicate
definitions. Linking the drivers in therefore cannot keep `snddll.c` as it is: in one image the
stubs and the functions they stand for have the same names. The linked-in form needs the binding
layer replaced (the drivers' calls going to the exe's functions directly, as the original's
non-DLL targets do: `snddll.c` is PC DLL glue) and the ADPCM/WAV pair resolved the same way, while
`Snd_Cpa.ini` still selects and any other driver still loads from the `DLL` folder. Also to decide
then: two static C runtimes (`/MT`) become the exe's one.

## 6. Platform-specific code inside the drivers (for PLAN Sound step 3)

- MMX mixer asm: `WAVMWBVR.dll/src/mixer/*.asm` (built with ML; `tools/build_wavmw.py --ml`) and
  `APMmxBVR.dll/src/mmx` (ML 6.11d). `Snd_Cpa.ini` names `WAVPW`/`APMPW` as the fallback when the
  MMX test fails; whether they are the same code without MMX is unchecked (not decompiled).
- Output: waveOut and the mixer API (`WINMM`), the output thread and its message loop, events and
  mutexes (KERNEL32), `MessageBoxA` (USER32).
- The exe side of sound (Win32 in the SDL build, measured 25.11.2026): `SND`, `SndThrd`, `sndfile`,
  `sndseq`, 37 of the 49 Win32 functions left.
