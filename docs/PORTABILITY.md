# Portability assumptions

What the code assumes about the machine it runs on, measured, with the sites. Written for the
ports in PLAN Phase 5 step 8 (`WIN64`, Linux, consoles). Nothing here is changed yet; each item
says who owns the change (the decompilation, through upstream, or the fork).

State: 26.11.2026, `main` at ff1a53e, upstream 27c4e53. Rerun the measurement after large merges.

## How it was measured

- **Pointer size:** every translation unit of the SDL gate configuration (the compile commands
  CMake exports for clang-cl) compiled again as x86-64, syntax only (`/Zs`), with only these
  diagnostics on: `-Wpointer-to-int-cast`, `-Wint-to-pointer-cast`, `-Wint-to-void-pointer-cast`,
  `-Wvoid-pointer-to-int-cast`, `-Wshorten-64-to-32`, `-Wint-conversion`,
  `-Wincompatible-pointer-types`. The same run as x86 (`-m32`) gives the findings that do not
  depend on pointer size (34 incompatible pointer types, 8 incompatible function pointer types,
  5 int conversions: warnings inventory, decomp request); they are subtracted below.
- **`long` size (LP64, Linux and macOS: `long` is 64 bits; Windows keeps it 32 even on x64):**
  not measured. A Linux target cannot parse the MSVC and Windows SDK headers the sources still
  include; it needs a Linux toolchain (first Linux build, PLAN step 8). The known sites are
  listed below from reading the code.
- **Packing, byte order:** read from the code (`#pragma pack`, file readers).
- **Inline asm:** listed in PLAN Phase 4 "MTH target selection" (x86 only; not repeated here).

## 1. Level data holds 32-bit pointers (the main blocker for 64-bit)

The level files are memory images (architecture.md section 4 "Relocation"): `.sna` blocks are
copied into MMG blocks as they are, and the relocation tables name the 4-byte slots that hold
pointers, to which the block's shift is added (`MMG/SNA_reloc.c` `SNA_pc_vLoadAndRelocate`).
`.gpt` is read dword by dword into engine globals (`SNA_pc_ulReadAndRelocateLong`, 20 calls: 19 in
`GAM/GAM_engine.c`, 1 in `GAM/GAM_levelload.c`).

So every structure that lives in a level block must have exactly the 32-bit MSVC layout: 4-byte
pointers, 4-byte `long`, MSVC x86 alignment, little-endian. On a 64-bit target the same C structs
are larger and the images do not fit. The 79 layout asserts below (field offsets and struct sizes
pinned to the retail values) fail on x64 for exactly this reason; they are correct guards, not
bugs.

Options (decision open, PLAN Phase 5 "Open decisions", 64-bit data):
- widen at load: read the 32-bit image and write each structure out in the target's layout (needs
  a description of every relocated structure: the relocation tables only say where pointers are,
  not what type the block holds);
- convert offline: the original engine did this for its cartridge target. Its binary level format
  (`include/BLC/`, `U64_tdst*` structures, packed to 1, `U64_tdxIndex` indices instead of pointers)
  is in the headers, unused on PC. A converter from the PC files to an index-based format, loaded
  by the original's binary loaders, would follow the original engine structure;
- keep pointer-holding data 32-bit (an allocator in the low 4 GB, 32-bit handles): fragile, not
  available on every target.

## 2. Pointers kept in integers (pointer size)

Measured on x64 (above): **171 pointer-to-integer casts and 301 integer-to-pointer casts** that do
not occur on x86, plus **12 narrowings** of `size_t`/`intptr_t` results to 32 bits. Two kinds:

- **The original engine's interfaces carry handles in `long`:** sound voices and object ids
  (`SND_fn_lSendRequestSound`, `SND_fn_vStopSnd`, `SND_fn_lPlaySnd`: `SND/sndplay.c`,
  `SND/sndtheme.c`, callers in `AI/Procedure.c`, `GAM/Objinit.c`), the AI value union (pointers in
  `uParam.lValue`: `AI/Procedure.c`, `AI/AICond.c`), error reports (`ERM_fn_vUpdateLastError`
  takes an `unsigned long` object), the MMG allocator's block chain (`unitAlloc` =
  `unsigned long`, `MMG/CpaMmg.h`: `MMG/MMG_alloc.c`, `MMG/MMG_pool_alloc.c`), the window and
  instance handed out by `GAM_pc_fn_vGetAppContext` (`GAM/pc_winmain.c:106-107`). On a 64-bit
  target these need a pointer-sized integer type in the module header, one branch per target, as
  the original engine declares its base types per platform. Fork change, after the decomp's types
  are settled.
- **Decompilation placeholders:** `int`/`unsigned int` locals and parameters that hold pointers
  (`MGT/MGT.c` walks the hierarchy in `int iChild`, `GAM/GAM_levelload.c`, `GAM/GAM_engine.c`
  saves engine handles in `int`, `HIE/HieDraw.c`, `SCT/Sectmir.c`, `AI/FuncRay2.c` way points).
  These belong to the decompilation (decomp request "warnings", category 2: pointers kept in
  `unsigned long`).
- **The Direct3D 6 driver** (`GliDX6vf.dll/`, 29 sites) stores pointers in `unsigned int` staging
  slots. It is replaced by the GL driver on every new target (PLAN Phase 5 step 6); not to be
  ported.
- `GAM/GAM_save_slot.c:506` redeclares `_alloca` with a 32-bit size (conflicting type on x64;
  decomp, missing prototypes).

## 3. `long` is 32 bits (LP64)

Not measured (above). Known kinds of site:
- **File formats declared with `long`:** `FIL/File.h` `FIL_tdstRealGFFileHeader` (`.gf` texture
  header), `FIL/File.c` `FIL_tdstPcBmpFileHeader`, the `.gpt` dword reads, the relocation table
  entries, sound bank headers (`SND/sndldbin.c`), save files (section 5). With 64-bit `long` each
  read takes 8 bytes.
- **Float bit tricks through `long *`:** `MTH/MTH_flt.h` (`MTH_M_bEqualZeroOpt`,
  `MTH_M_bDifferentZeroOpt`, sign tests: `register long *pA = (long *)&xA`) and the other casts of
  an address to `long *` / `unsigned long *` (143 in `Rayman2.exe/`, floats among them). On LP64
  each reads 8 bytes, from a float only 4 are its own.
- Every layout assert of section 1 that involves a `long` field.
The original engine targets were all ILP32; the fork's choice (fixed-width types per platform in
the base headers, as the original declares its types per target) is open (PLAN Phase 5 "Open
decisions", `long` on LP64).

## 4. Packing

Explicit packing, all on file or retail-layout structures:

| Site | Pack | What |
|---|---|---|
| `FIL/File.h:29` | 1 | `.gf` texture file header |
| `FIL/File.c:641` | 1 | `.bmp` file header |
| `SAI/SAI_types.h:38` | 4 | save list entries (`SAI_tdstListEntry`: pointers and an `__int64` in a union) |
| `MMG/MMG_globals.h:124` | 1 | protection module table (goes with the protection code, Phase 2) |
| `include/BLC/*.h` (13 of the 14 headers) | 1 | the original's cartridge level format, unused on PC (section 1) |
| `WAVMWBVR.dll/include/SND/sndtype.h:12`, `sndevent.h:30` | `M_SET_ALIGNMENT` | sound bank and event structures shared with the sample driver |

Implicit: every structure in the level images (section 1) relies on MSVC x86 natural alignment,
which differs on 64-bit (8-byte pointers) and on some console ABIs.

## 5. Byte order

All data files are little-endian and read straight into memory, no swapping anywhere:

| Reader | Files |
|---|---|
| `MMG/SNA.c` `SNA_fn_ulFRead` (23 calls: 17 in `MMG/SNA_reloc.c`, 6 in `GAM/GAM_init.c`) | `.sna`, `.gpt`, `.snd`, `.ptx`, relocation tables, `LEVELS0.DAT` |
| `GLI/misc_trivial.c` (`fread`, 12), `GLI/Texture.c` `FIL_fn_lReadFileInConcatFile` | texture tables, `.gf` |
| `FIL/File.c`, `FIL/FIL_ini.c` | `.gf`/`.bmp` headers, ini files (text, neutral) |
| `SND/sndldbin.c` `SND_fn_dwReadFileSnd` (6) | `.bnm` sound banks |
| `GAM/GAM_save_slot.c` (`fread`, 19), `SAI/SAI_Save.c` (8), `SAI/SAI_Codec.c` (2) | save slots |
| `IPT/IPT_Save.c` (`fread` through `_imp__fread`) | input configuration |
| sound drivers (`WAVMWBVR.dll/`, `APMmxBVR.dll/`) | sample data, `.apm` streams |

Endianness also enters through the float bit tricks (section 3; neutral as long as floats and
integers share the byte order, which holds on every planned target) and the colour packing in the
GLI (32-bit ARGB words built with shifts: neutral in registers, not when stored and read as bytes).
Planned targets: x86, x64 and ARM are little-endian; Wii U (PowerPC) is big-endian. The original
engine's answer for its big-endian cartridge target was offline conversion (section 1).
**Decision open** (PLAN Phase 5 "Open decisions", big-endian targets).

## 6. Sites: pointer size (x64 minus x86)

Paths relative to `Rayman2.exe/` unless they start with a driver folder; line numbers in the
sources at the state above.

### 6.1 Pointer -> integer (171)
- `GliDX6vf.dll/src/clip/clip_staging.c`: 77
- `src/AGO/AGO.c`: 1280, 1631
- `src/AI/AICond.c`: 1852, 1859
- `src/AI/AIKeyWord.c`: 259, 261
- `src/AI/ConvVar.c`: 93
- `src/AI/Fields.c`: 227, 238
- `src/AI/FuncRay2.c`: 1613
- `src/AI/Function.c`: 671, 1448
- `src/AI/Procedure.c`: 2043, 4323, 5099, 5162, 5186, 5187
- `src/COL/COL.c`: 25, 27
- `src/DNM/DNM.c`: 58, 60
- `src/GAM/Actions/CollSet.c`: 413, 415, 436, 438
- `src/GAM/Actions/MSlight.c`: 157
- `src/GAM/Actions/SectInfo.c`: 125, 126, 238, 279
- `src/GAM/Always.c`: 240
- `src/GAM/Basic.c`: 498, 500
- `src/GAM/GAM_GameEng.c`: 221, 227, 930, 979
- `src/GAM/GAM_engine.c`: 319, 581, 679, 865
- `src/GAM/GAM_init.c`: 820
- `src/GAM/GAM_levelload.c`: 408, 514, 523, 605
- `src/GAM/Micros.c`: 445, 455, 465
- `src/GAM/Objinit.c`: 305, 390, 422, 433, 505, 581, 773
- `src/GAM/PlayAnim/a3x_int.c`: 36, 43
- `src/GAM/PlayAnim/playevts.c`: 379, 391
- `src/GAM/TextFrmt.c`: 278, 328
- `src/GAM/ToolCam.c`: 208, 212
- `src/GAM/pc_winmain.c`: 106, 107, 1257, 1265
- `src/GLI/Material.c`: 89
- `src/GLI/Object.c`: 145, 783
- `src/GLI/Texture.c`: 1308, 1336
- `src/GLI/misc_trivial.c`: 740
- `src/GLI/pc_light_pipeline.c`: 492
- `src/GMT/GMT.c`: 217, 218
- `src/HIE/HieDraw.c`: 240
- `src/INO/INO.c`: 344
- `src/IPT/IPT_Anal.c`: 182
- `src/IPT/IPT_Kwan.c`: 755, 756
- `src/IPT/IPT_Main.c`: 445
- `src/MGT/MGT.c`: 220, 221, 430, 431, 434, 468, 513, 585, 586, 610, 689, 763, 777, 823, 835
- `src/MGT/MGT_pending.c`: 72, 83
- `src/MMG/MMG_alloc.c`: 192, 202, 229, 239, 260, 269, 275, 316, 331, 337, 397, 403, 445, 451
- `src/MMG/MMG_module.c`: 93, 138
- `src/MMG/MMG_pool_alloc.c`: 171, 236, 279, 300
- `src/MMG/SNA_reloc.c`: 439, 444, 630, 638, 659, 738, 739
- `src/SAI/SAI_Codec.c`: 135, 271
- `src/SAI/SAI_Priv.c`: 252, 318
- `src/SAI/SAI_Save.c`: 128
- `src/SCT/Sectmir.c`: 164, 180, 211, 213, 221
- `src/SCT/Sector.c`: 656
- `src/SND/SND.c`: 1749, 1790, 3451, 3471, 3563
- `src/SND/sndldbin.c`: 161, 165, 275, 277
- `src/SND/sndplay.c`: 233, 271, 476, 533, 535, 611, 745, 828, 830, 852, 854, 1179, 1181, 1183
- `src/SND/sndrndom.c`: 152
- `src/SND/sndswtch.c`: 138
- `src/SND/sndtheme.c`: 352, 371, 414, 426

### 6.2 Integer -> pointer (301)
- `GliDX6vf.dll/src/clip/GLI_DRV_vSendSpriteToClip.c`: 97, 98, 99, 100, 102, 105
- `GliDX6vf.dll/src/clip/GLI_DRV_vSendSpriteToClipWithColors.c`: 100, 103
- `GliDX6vf.dll/src/clip/GLI_DRV_vSendSpriteToClipWithUV.c`: 49, 50, 51, 52, 65, 68, 75, 80, 82, 83
- `GliDX6vf.dll/src/clip/GLI_DRV_xSendListToViewport.c`: 197, 233, 235
- `GliDX6vf.dll/src/clip/GLI_SendSpriteTrianglesViaClip.c`: 19, 63, 65, 67, 99, 101, 103
- `src/AGO/AGO.c`: 848, 1087, 1119, 1123, 1127, 1136, 1140, 1157, 1161, 1166, 1800, 2003, 2008
- `src/AI/AICond.c`: 254, 941, 950, 1042, 1083, 1091, 1092, 1099, 1100, 1105
- `src/AI/AI_Proto.c`: 437, 443
- `src/AI/ConvVar.c`: 251
- `src/AI/FuncRay2.c`: 650, 679, 1611
- `src/AI/Functio1.c`: 291
- `src/AI/Function.c`: 825
- `src/AI/Procedure.c`: 306, 427, 1695, 2699, 2701, 2709, 5001, 5100, 5110, 5123, 5127, 5163, 5166, 5187, 5189, 5216, 6559, 7614, 7615, 7616, 7617, 7622, 7623, 7628
- `src/AI/StrIntel.c`: 74
- `src/FIL/File.c`: 714
- `src/GAM/Actions/CollSet.c`: 248
- `src/GAM/Actions/MSSound.c`: 19
- `src/GAM/Basic.c`: 454
- `src/GAM/GAM_GameEng.c`: 787, 789, 793, 1043, 1045, 1047
- `src/GAM/GAM_engine.c`: 281, 325, 328, 329, 338, 339, 340, 341, 342, 343, 344, 345, 346, 347, 348, 349, 350, 582, 622, 898
- `src/GAM/GAM_init.c`: 1245, 1246, 1249, 1256, 1266, 1267, 1270, 1278
- `src/GAM/GAM_levelload.c`: 222, 227, 362, 402, 404, 405, 407, 409, 417, 418, 446, 463, 468, 473, 534, 536, 553, 600, 602, 669, 670
- `src/GAM/Micros.c`: 116, 121, 222, 225, 419
- `src/GAM/Objinit.c`: 219, 220, 610, 743, 747, 749, 756, 758, 762, 763, 764, 765, 766, 773, 777
- `src/GAM/TextFrmt.c`: 509, 519, 582
- `src/GAM/pc_winmain.c`: 1184
- `src/GLI/Camera.c`: 127, 128, 129
- `src/GLI/GLD.c`: 359, 544, 603
- `src/GLI/Material.c`: 89
- `src/GLI/Object.c`: 783
- `src/GLI/Texture.c`: 1073, 1187, 1263
- `src/GLI/pc_magic_mirror.c`: 131
- `src/HIE/HieDraw.c`: 405, 654, 659, 1055
- `src/IPT/IPT_Main.c`: 481
- `src/MGT/MGT.c`: 103, 115, 219, 220, 221, 437, 476, 487, 513, 531, 534, 535, 570, 595, 626, 692, 697, 700, 781, 782, 783, 786, 787, 788, 799, 803, 831, 833, 834, 889, 917, 964, 967, 984, 994, 1013, 1014
- `src/MGT/MGT_pending.c`: 74, 93
- `src/MMG/MMG_alloc.c`: 156, 184, 366, 377, 435, 443
- `src/MMG/MMG_module.c`: 95, 140
- `src/MMG/MMG_pool_alloc.c`: 140, 153, 244, 267
- `src/MMG/SNA_reloc.c`: 462, 465, 499, 501, 630, 637, 659, 739
- `src/SAI/SAI_Priv.c`: 249, 252, 318
- `src/SAI/SAI_Save.c`: 126
- `src/SND/SND.c`: 3133, 3220, 3221, 3222
- `src/SND/sndplay.c`: 238, 250, 259, 589, 603, 742, 745, 748, 771, 786, 796, 806, 812, 953, 957, 961, 981, 984, 988, 997, 1006, 1010, 1014, 1023, 1035, 1038, 1041, 1054, 1076, 1094, 1126
- `src/SND/sndrndom.c`: 164, 193, 208, 219, 228, 237
- `src/SND/sndswtch.c`: 156, 203, 221, 235, 244, 253
- `src/SND/sndtheme.c`: 272, 497, 516, 533, 558, 568

### 6.3 `size_t` / intptr narrowed to 32 bits (12)
- `src/AI/Procedure.c`: 5383
- `src/FIL/File.c`: 440, 444, 468, 848, 870, 1046
- `src/GAM/GAM_levelload.c`: 157
- `src/GAM/GAM_save_slot.c`: 293, 463
- `src/MMG/MMG_pool_alloc.c`: 201
- `src/MMG/SNA.c`: 66

### 6.4 Layout asserts (retail offsets and sizes) (79)
- `include/DNM/DNM_types.h`: 320, 384
- `include/GAM/GAM_types.h`: 348, 349, 350, 351, 352, 353, 354, 355
- `include/IPT/IPT.h`: 26, 27, 28, 52
- `include/IPT/IPT_Str.h`: 34, 35, 36
- `include/MGT/MGTStruc.h`: 40, 41, 42, 43, 44, 45, 46, 47
- `src/COL/CollGO.c`: 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 26, 27, 28, 29
- `src/COL/CsEltEdg.c`: 14, 15, 16, 17
- `src/COL/CsSphElt.c`: 17, 18, 19, 20, 21, 22, 23, 24, 25, 26, 27, 28, 29, 30, 31, 32, 33, 34, 35, 36, 37, 38, 39, 40, 41, 42, 43, 44, 45, 46
- `src/TMR/TMR.c`: 7
