# Bugs of the original engine

Bugs that the retail executable itself has: uninitialised reads, NULL reads and writes, dangling
pointers and overruns. The decompilation keeps them, so it matches retail byte for byte. This fork
fixes them under `RAYMAN2_FIX_BUGS` (CMake `R2_FIX_BUGS`). The original code stays in the `#else`
branch, and each site is marked `/* FIX(BUGS.md#n) */`. Scope: crashes and undefined behaviour
only. Gameplay quirks are not touched (docs/PLAN.md, Phase 3).

Not listed here: places where the decompiled C differs from retail. Those are fixed in the
decompilation and reach this fork through upstream merges.

## Confirmed

| # | Function (file), retail address | Symptom | Cause | Fix | Reachable | Status |
|---|---|---|---|---|---|---|
| 1 | `GAM_fn_vEngine` (`GAM/GAM_GameEng.c`), 0x0040ADA0 | Whether magnets run this frame (`MGT_fn_vTreatMagnets`) depends on whatever is on the stack. | `bEnabledWrite` is tested (0x0040ADF9 `mov eax,[esp+8]`) before `GLD_bRequestWriteToViewport` writes it later in the frame (0x0040AF51). The read is uninitialised every frame. | Start it at 1. That is the only value the PC `GLD_bRequestWriteToViewport` ever stores. | Every frame | Proposed |
| 2 | `GEO_lMainCullingSphere` (`GEO/GEO.c`), 0x0041EE90 | Objects whose parent was already found in front of the near plane are never far-culled or faded by the far transparency zone. In C the result is undefined. | `xDotProduct` (sphere front Z, `stCenter.z - radius`) is computed only when the near-plane result is not yet known. The far-plane test reads it either way. Retail keeps it in the `p_stMatrix` argument slot (`fst [ebp+0x14]`), so on that path it compares the matrix pointer's bits as a float: a tiny positive value, never beyond the far plane. | Keep retail's visible result without the undefined read: when the near result is already known, the sphere counts as not beyond the far plane. (Computing `stCenter.z - radius` instead would fade or cull distant objects that retail shows; not done.) | Every frame, hierarchy culling | Proposed |
| 3 | `MGT_pc_fn_ucRefreshMagnetModification` (`MGT/MGT.c`), 0x004417A0 | When the magnet is no longer active, the "changed" result and the "remove" flag can be garbage. The effects are a redundant `GEO_vEndModifyObject2` and the destruction of an already empty modification. | On the inactive-magnet path, `ucRet` and `ucRemoved` are only set when a point is blended. Retail returns byte 3 of the `p_stMod` argument and reads an uninitialised stack byte for the remove flag. The active path starts both at 0. | Start both at 0 on that path too. | When a magnet stops influencing a deformed object | Proposed |
| 4 | `fn_ucGetFieldAbsoluteAxisX` (`AI/Fields.c`), 0x0048D0D0; field table rows AbsoluteAxisX/Y/Z | A script reading the AbsoluteAxisX, Y or Z field crashes (write to address 0). Y and Z would return the X axis. | Calls `POS_fn_vGetRotationMatrix(m, NULL, NULL, NULL)`, which writes the three axes through NULL, then reads floats at 0x0/0x4/0x8. Y and Z share the X getter (identical bodies). | Get the axes into local vectors; return the I, J and K column for X, Y and Z. | Only through scripts; no retail caller known (a call would crash) | Proposed |
| 5 | `FUN_00492860` (`GAM/GAM_init.c`), 0x00492860 | If `GetTempPathA` fails, the byte before the path buffer is read, and is overwritten with 0 if it is a slash. | `szTemp[strlen(szTemp) - 1]` with an empty string indexes -1. | Test the length first. | Only when `GetTempPathA` fails | Proposed |
| 6 | `GAM_pc_fn_vFreeMmgTableAndHelperDll` (`GAM/pc_winmain.c`), 0x004494B0 | At exit, the PSAPI handle is read from the table after the table is freed (and through NULL if it was never allocated). | `free(table)` comes before `FreeLibrary(table[2])`. | Read the handle before freeing, and skip it if the table is NULL. | At exit, only when the protection loaded PSAPI. Not reached with `RAYMAN2_NO_PROTECTION`, which never sets `MMG_pc_g_ulPsapiLoaded`. | Proposed |

## Checked, no fix needed

| Function, retail address | Finding |
|---|---|
| `GAM_pc_fn_szFormatSaveSlotDate`, 0x00410DB0 | Retail returns a pointer to a stack buffer (dangling). This source already returns a static buffer, so it is safe here. |
| `FUN_00405a30` (ubi.ini, `ParticuleRate`), 0x00405A30 | `Null` sets the rate to 0.0 and nothing else matches it. `Low`/`Medium`/`High` set 1/2/3, and any other value keeps the default. Retail does the same (one call per matched value). Well defined. |
| `GLI_fn_lRequestNewDisplayMode`, 0x004222C0 | The deferred-mode path returns no value (retail returns whatever `EAX` holds). Its only caller (AI procedure) ignores the result. |
| `fn_uiEngineMainLoop`, 0x00401540 | No return value, but it ends in `GAM_pc_fn_vEngineThreadExit`, which never returns (`Sleep(INFINITE)`). |
| Compiler leads (C4701/C4703, 2026-11-23) in `GEO/Element.c`, `GEO/GeoObj.c`, `GMT/GMT.c`, `HIE/HIE.c`, `HIE/HieMtStk.c`, `SCT/Sector.c`, `MMG/MMG_alloc.c`, `COL/COL.c`, `HIE/HieDraw.c`, `MEC/MEC.c`, `MEC/MECBhv.c`, `MEC/MECCol.c`, `MEC/MECTools.c`, `INT/Inters.c`, `INO/INO.c`, `AI/Procedure.c`, `GEO/GEO.c` (box culling) | False positives. The allocation macro always assigns for a non-zero size. Paired tests use the same predicate for the write and the read. The switches only ever see the handled values. |

## Open

| Function, retail address | Lead | Needed |
|---|---|---|
| `IPT_fn_bIsKeyWordValid_JoystickAxeValue` (`IPT/IPT_Kwan.c`) | `dfPosition` is uninitialised if the entry's action number is not an axis (X..V). | Check whether the input data or its parser can produce a non-axis action. |
| `GLI_pc_vTransformPointsByObject`, 0x0042CA40 | Retail writes overlapping 3-float output vectors (8-byte stride), and its loop reads a frame slot that nothing writes. This source does not reproduce it yet. | First settle what retail computes (decompilation). Then decide whether the result is defined. |
| clang-cl `-Wuninitialized -Wsometimes-uninitialized -Warray-bounds`; a `/RTC1` run of the regression | Not run yet. | Further leads. |
