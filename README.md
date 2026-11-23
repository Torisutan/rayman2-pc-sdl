> **rayman2-pc-sdl** is a fork of [rayman2-pc-decomp](https://github.com/Torisutan/rayman2-pc-decomp).
> It turns the decompilation into a portable engine (modern compilers, no copy protection, fixed
> original bugs, later multiplatform), keeping the original engine structure. See
> [docs/PLAN.md](docs/PLAN.md). The rest of this README is upstream's.

# rayman2-pc-decomp

A decompilation of the PC version of **Rayman 2: The Great Escape** (`Rayman2.exe`, GOG release,
MD5 `ef4954015d8b67e3534c9dc40ca3ad6e`). The goal is C source that rebuilds a **byte-identical**
`Rayman2.exe` with the original toolchain (MSVC 5.0 SP3, LINK 5.10, x87).

## Status

3267 functions reimplemented: 2862 byte-exact, 405 not yet byte-exact (per-function list: `Rayman2.exe/status.json`).

The full program builds, links and runs. It is not yet byte-identical to retail.

## Layout

| Path | Contents |
|---|---|
| `Rayman2.exe/src/` | Translation units, one engine module per directory (AI, GAM, GLI, MTH, SND, ...) |
| `Rayman2.exe/include/` | Module headers |
| `Rayman2.exe/shim/` | `stdint.h` / `intrin.h` stand-ins for the VC5 compiler |
| `Rayman2.exe/tu.json` | Link order plus compiler flags and toolchain for each TU |
| `Rayman2.exe/status.json` | Each function's retail address, size, source file and match verdict |
| `Rayman2.exe/exports.txt` | The retail export table, which the sound plugin DLLs import from the exe |
| `Rayman2.exe/crack.json` | Translation tables for the post-link crack step |
| `tools/build.py` | Retail-toolchain build (`--crack` also writes a runnable cracked copy) |
| `tools/check.py`, `tools/byte_match.py` | Byte comparison against retail: whole exe, or one function with an asm diff |
| `tools/crack.py` | Post-link crack step |

`/* 0x00XXXXXX */` comments give the retail address of the function or global they sit next to.

## Documentation

- [BUILDING.md](BUILDING.md): toolchain, build, byte-matching tools
- [docs/architecture.md](docs/architecture.md): engine structure, toolchain and link facts, method

## Legal

This repository contains no game data, executables or toolchain binaries. Building and running
it needs your own copy of the game. Rayman is a trademark of Ubisoft. This project is not
affiliated with or endorsed by Ubisoft.
