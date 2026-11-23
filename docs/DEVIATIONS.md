# Deviations from the original engine structure

Every place where this fork departs from the original module split, names, file layout or data
structures. Each entry gives the place, the change, and why there was no other way.

| # | Where | Change | Why |
|---|---|---|---|
| 1 | `GAM/GAM_init.c` (`FUN_00405a30`), `GAM/pc_winmain.c` (`GAM_pc_fn_WinMain`), `GLI/Init_Gli_helpers.c` (`GLI_pc_fn_vRead/WriteIniString`); `RAYMAN2_LOCAL_UBIINI` | `ubi.ini` is read and written in the working directory (the game folder) instead of the Windows directory (`<windir>\Ubi.ini`, `<windir>\UbiSoft\ubi.ini`). | Current installs ship `ubi.ini` beside the exe and the Windows directory is not writable; without it the graphics DLL is not found. Interim: Phase 2 moves it to a per-user location. |
