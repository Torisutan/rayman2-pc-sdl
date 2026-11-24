# Playtest checklist

A human plays, the findings are recorded here. The regression run (PLAN Phase 1) covers one level
without input; this list covers what it cannot see: menus, input devices, focus changes, sound,
saves, the display options.

Every finding ends up in exactly one place:
- **`docs/BUGS.md`** row: a crash or undefined behaviour of the original engine (Phase 3 rules).
- **`docs/PLAN.md`** open item: a fork regression or a missing feature of a backend.
- **"original behaviour"**: the retail exe does the same (checked in the same sandbox, same
  driver). Noted in the log below, nothing else to do.

## Builds

| Name | Build | Driver | Start |
|---|---|---|---|
| SDL-GL | `build-sdl` (`R2_USE_SDL`) | the install's OpenGL driver (`GliOGLvf.dll`) | `python tools/run.py build-sdl/Release/Rayman2.exe --seconds 5 --keep` |
| SDL-DX6 | `build-sdl` | the linked-in Direct3D 6 driver | as SDL-GL, plus the DX6 keys below |
| W32-DX6 | `build-cmake` (Win32 path, comparison) | the linked-in Direct3D 6 driver | `python tools/run.py build-cmake/Release/Rayman2.exe --seconds 5 --keep`, plus the DX6 keys |
| W32-GL | `build-cmake` | the install's OpenGL driver | `python tools/run.py build-cmake/Release/Rayman2.exe --seconds 5 --keep` |

DX6 keys (the install's `ubi.ini` names the OpenGL driver; the whole set is needed):
`--ini GLI_DllFile=GliDX6 --ini GLI_Dll=DirectX6 --ini GLI_Driver=display
--ini "GLI_Device=Direct3D HAL" --ini "GLI_Mode=1 - 1920 x 1080 x 16"`.

Build both at the commit under test (`cmake --build build-sdl --config Release -j 8`,
`cmake --build build-cmake --config Release -j 8`) and note the commit in the log.

**Saves and options.** `tools/run.py` copies the exe into the sandbox (`build/run`) and resets
`Data/Options`, `Data/SaveGame` and `ubi.ini` at every start, and it ends any running
`Rayman2.exe` first. To keep a save across a restart (step G3), start `build\run\Rayman2.exe`
directly from that folder instead of going through `run.py` again.

## Known, not to report again

- Win32 build with the Direct3D 6 driver: after Alt+Tab back the window stays white with the pause
  title and ignores closing (deadlock on DirectDraw's exclusive-mode mutex; the retail exe does
  the same on this system, PLAN Phase 5 focus test). End it with Task Manager.
- SDL-DX6: a small window shows for about 0.17 s at start.
- Gamepad triggers do nothing in the SDL build (PLAN open decision "Gamepad triggers").
- Gamepads plugged in after the start are not seen (as with DirectInput).
- The mouse moves nothing unless the game itself enables it.

## Checklist

Mark each cell `ok`, `-` (not tried) or the number of a log entry.

### S: start and exit
| # | Step | Expected | SDL-GL | SDL-DX6 | W32-DX6 | W32-GL |
|---|---|---|---|---|---|---|
| S1 | Start | Intro/logos, then the menu; no stray or extra window; menu music plays | | | | |
| S2 | Quit from the menu | The process ends within about a second (Task Manager), no error box | | | | |
| S3 | Close the window (Alt+F4) in the menu | Same as S2 | | | | |

### G: game, levels, saves
| # | Step | Expected | SDL-GL | SDL-DX6 | W32-DX6 | W32-GL |
|---|---|---|---|---|---|---|
| G1 | New game, play the first level for a few minutes | Controls, camera, sound and music as in retail | | | | |
| G2 | Reach one level transition | Loading screen (see W3), next level plays | | | | |
| G3 | Save the way the game offers it, quit, start again (directly, see above), load | Same place and progress | | | | |

### O: options menu
| # | Step | Expected | SDL-GL | SDL-DX6 | W32-DX6 | W32-GL |
|---|---|---|---|---|---|---|
| O1 | Change the display mode, if the menu offers it | New mode, picture correct, menu still usable | | | | |
| O2 | ~~Rebind keys~~ | Retail has no key-binding option (log 5) | n/a | n/a | n/a | n/a |
| O3 | Change the sound and music volumes | Audible change; kept after restart | | | | |

### F: focus and window
| # | Step | Expected | SDL-GL | SDL-DX6 | W32-DX6 | W32-GL |
|---|---|---|---|---|---|---|
| F1 | Alt+Tab away during play, wait, come back | Away: pause title, sound stops. Back: fullscreen again, sound resumes, play continues | | | | |
| F2 | Same in the menu | Same | | | | |
| F3 | Minimise (Win+Down or taskbar) and restore | Same as F1. Watch for a crash: a minimise once ended a run with 0xC000041D (BUGS.md, Open) | | | | |
| F4 | Close the window while away/paused | Process ends | | | | |

### I: input devices
| # | Step | Expected | SDL-GL | SDL-DX6 | W32-DX6 | W32-GL |
|---|---|---|---|---|---|---|
| I1 | Keyboard with an AZERTY layout (Win+Space to switch), in menu and game | Same keys as retail with that layout | | | | |
| I2 | Mouse, where the game uses it | Same as retail | | | | |
| I3 | Gamepad: every button, both sticks, D-pad, in menu and game | Note what each one does (log) | | | | |
| I4 | Gamepad triggers, each on its own | W32 builds: note what each trigger does (answers which trigger moves DirectInput's Z axis which way). SDL builds: nothing (known) | | | | |

### W: widescreen and loading screen (16:9 mode)
| # | Step | Expected | SDL-GL | SDL-DX6 | W32-DX6 | W32-GL |
|---|---|---|---|---|---|---|
| W1 | Play, open the in-game menu | 3D view wider (not stretched); menus 4:3 in the middle; HUD at the screen corners | | | | |
| W2 | Logos and script text on screen (not seen in tests so far) | 4:3 proportions; note where they appeared | | | | |
| W3 | Watch a level load | The bar visibly advances and ends full | | | | |

### R: anything else
Anything that looks, sounds or reacts differently from the retail game: one log entry each.

## Log

| # | Date | Commit | Build | Step | What happened | Retail does the same? | Recorded as |
|---|---|---|---|---|---|---|---|
| 1 | 24.11.2026 | 05a2305 | SDL-GL | W1 | Hall of doors at 1920x1080: the lum and cage icons and the cage counter sit at the left screen edge, the lum counter (826/999) stays in the 4:3 band, so icon and counter are far apart (Screenshot 19). Before 15d20d9 all of it stayed in the band together. Cause: DEVIATIONS 11 decides per element; the lum counter reaches past the left third of the 4:3 layout (its right edge at 36 %), the icon and the cage counter do not. | no (fork change) | fixed: PLAN, Widescreen "HUD at the screen edges" (anchor per group); DEVIATIONS 11 |
| 2 | 24.11.2026 | 05a2305 | SDL-GL | G3 | Two slots in the menu, 0 % and 88 %. Whichever is chosen, the game loads either the current progress or the 88 % slot. On disk `Slot1` holds 68 files dated 14.06.2026, `Slot2` (created 24.11.2026 22:59) is empty. Steps: new game (0 % slot), quit, start again, load the 88 % slot, then load the other slot during play. The slot code (`GAM/GAM_save_slot.c`) is unchanged from the export, but 7 of its functions do not match retail yet (PLAN). | user: retail's slots are odd too; not compared step by step | PLAN, Phase 4 "Save slots", open |
| 3 | 24.11.2026 | 05a2305 | SDL-GL | I3 | Gamepad works, but the buttons do other things than expected. Today's layout is DirectInput's XInput order (DEVIATIONS 16); the Dreamcast layout is handoff task 2b. | n/a | task 2b |
| 4 | 24.11.2026 | 05a2305 | SDL-GL | I3 | No rumble. The decompiled exe declares DirectInput's effect interface but never calls `CreateEffect`, so the PC game most likely has no rumble (from the code; not played on retail). | yes (from code) | original behaviour |
| 5 | 24.11.2026 | - | - | O2 | Retail has no key-binding option in its menu. | yes | original behaviour |
| 6 | 24.11.2026 | 05a2305 | SDL-GL | F1, G2 | Alt+Tab and a level transition work. | - | ok |
