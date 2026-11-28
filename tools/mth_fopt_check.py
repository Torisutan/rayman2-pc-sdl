#!/usr/bin/env python3
"""Check the C branch of MTH_fopt.h's table functions against the x87 branch, over all 2^32 inputs.

MTH_fn_fSquareRootOpt, MTH_fn_fInverseOpt and MTH_fn_fInvSquareRootOpt read a table built by
MTH_fn_vInit (src/MTH/MTH_fopt.c). Targets without x86 inline assembly (MTH_C_DEV) take their C
branch. This tool builds tools/mth_fopt_check/ and runs every 32-bit pattern through them:

  python tools/mth_fopt_check.py           MSVC 5.0 (paths from local.json, as tools/build.py):
                                           x87 branch and C branch in one exe, compared input by
                                           input; prints the checksum of each
  python3 tools/mth_fopt_check.py --gcc    gcc -m32 (Linux, WSL): C branch only; its checksums
                                           must equal the x87 ones recorded in check.c

Exit code 0 when everything matches. Run the MSVC 5.0 build outside a VS developer shell.
"""
import argparse
import os
import subprocess
import sys

sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
import build  # noqa: E402

SRC = os.path.join(build.ROOT, "tools", "mth_fopt_check")
FOPT_C = os.path.join(build.EXE_DIR, "src", "MTH", "MTH_fopt.c")
OUT = os.path.join(build.BUILD, "mth_fopt_check")


def run(cmd, env=None):
    p = subprocess.run(cmd, cwd=OUT, env=env, capture_output=True, text=True, errors="replace")
    if p.returncode:
        print(" ".join(cmd))
        print((p.stdout + p.stderr).strip()[-3000:])
        sys.exit("build failed")


def build_vc5(a):
    env = build.env(a, "vc5sp3")
    cl = build.cl_exe(a, "vc5sp3")
    base = ["/nologo", "/c", "/O2", "/I" + build.INC, "/DINLINE=static __inline"]
    run([cl] + base + ["/Foforms_asm.obj", os.path.join(SRC, "forms.c")], env)
    run([cl] + base + ["/DMTH_C_DEV", "/Foforms_c.obj", os.path.join(SRC, "forms.c")], env)
    run([cl] + base + ["/Fofopt.obj", FOPT_C], env)
    run([cl] + base + ["/DMTH_FOPT_CHECK_ASM", "/Focheck.obj", os.path.join(SRC, "check.c")], env)
    exe = os.path.join(OUT, "mth_fopt_check.exe")
    run([os.path.join(a.vc5, "bin", "link.exe"), "/NOLOGO", "/OUT:" + exe,
         "check.obj", "forms_asm.obj", "forms_c.obj", "fopt.obj"], env)
    return exe


def build_gcc():
    exe = os.path.join(OUT, "mth_fopt_check")
    run(["gcc", "-m32", "-O2", "-I" + build.INC, "-DINLINE=static inline", "-DMTH_C_DEV",
         "-o", exe, os.path.join(SRC, "check.c"), os.path.join(SRC, "forms.c"), FOPT_C, "-lm"])
    return exe


def main():
    ap = argparse.ArgumentParser(description=__doc__.split("\n")[0])
    build.add_path_args(ap)
    ap.add_argument("--gcc", action="store_true", help="C branch with gcc -m32 (Linux)")
    a = ap.parse_args()
    os.makedirs(OUT, exist_ok=True)
    exe = build_gcc() if a.gcc else build_vc5(build.resolve_paths(a, need=("vc5",)))
    sys.exit(subprocess.run([exe]).returncode)


if __name__ == "__main__":
    main()
