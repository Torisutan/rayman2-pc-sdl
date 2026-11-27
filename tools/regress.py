#!/usr/bin/env python3
"""Regression run: same input, same engine state hash per frame, in any build.

The exe must be built with the harness (RAYMAN2_REGRESS: CMake option R2_REGRESS, on by default;
tools/build_ref.py for the VC5 reference). See Rayman2.exe/src/app/regress.c for what is pinned
and hashed.

  python tools/regress.py run EXE OUT [--level Morb_00] [--frames 1800] [--input SCRIPT]
                                      [--record SCRIPT] [--timeout 300] [--dx6]
      run EXE in the sandbox (tools/run.py) and write the per-frame hash file OUT
  python tools/regress.py suite EXE OUTDIR [--only NAME ...] [--suite FILE] [--dx6]
      run every scenario of tools/regress/suite.txt (level, frames, input script) into
      OUTDIR/NAME.txt
  python tools/regress.py diff A B [--report FIELD ...]
      compare two hash files: first differing frame and the fields that differ there. With two
      suite folders, every scenario file of A against the same file in B.
      --report FIELD: FIELD does not gate; its first difference is reported. Use it to compare
      builds whose arithmetic differs (docs/PLAN.md Phase 2: modern build against VC5 with
      --report actor.matrix); the modern build gates against its own baseline.

--dx6: the Direct3D 6 driver (docs/PLAYTEST.md DX6 keys) instead of the install's ubi.ini driver.
The display mode is part of the keys; --mode, given too, replaces it.

Exit code 0 = run complete / files identical.
"""
import argparse
import os
import subprocess
import sys
import time

HERE = os.path.dirname(os.path.abspath(__file__))
DX6_KEYS = [("GLI_DllFile", "GliDX6"), ("GLI_Dll", "DirectX6"), ("GLI_Driver", "display"),
            ("GLI_Device", "Direct3D HAL"), ("GLI_Mode", "1 - 1920 x 1080 x 16")]
sys.path.insert(0, HERE)
import run  # noqa: E402

DEFAULT_INPUT = os.path.join(HERE, "regress", "morb_00.txt")
DEFAULT_SUITE = os.path.join(HERE, "regress", "suite.txt")


def do_run(a):
    subprocess.run(["taskkill", "/F", "/IM", "Rayman2.exe"], capture_output=True)
    time.sleep(0.5)
    exe = run.build_sandbox(os.path.abspath(a.exe))
    if a.dx6:
        for key, value in DX6_KEYS:
            run.set_ini(os.path.join(run.BOX, "ubi.ini"), key, value)
    if a.mode:
        run.set_mode(os.path.join(run.BOX, "ubi.ini"), a.mode)
    out = os.path.abspath(a.out)
    if os.path.exists(out):
        os.remove(out)
    env = dict(os.environ, R2_REGRESS_OUT=out, R2_REGRESS_FRAMES=str(a.frames))
    if a.level and a.level != "-":
        env["R2_REGRESS_LEVEL"] = a.level
    if a.record:
        env["R2_REGRESS_RECORD"] = os.path.abspath(a.record)
    else:
        env["R2_REGRESS_INPUT"] = os.path.abspath(a.input)
    t0 = time.time()
    p = subprocess.Popen([exe], cwd=run.BOX, env=env)
    try:
        rc = p.wait(a.timeout)
    except subprocess.TimeoutExpired:
        p.kill()
        rc = None
    n = sum(1 for _ in open(out)) if os.path.exists(out) else 0
    print("%s: %d frames in %.0fs, %s" % (os.path.basename(out), n, time.time() - t0,
                                          "exit 0" if rc == 0 else "TIMEOUT" if rc is None
                                          else "exit 0x%08X" % (rc & 0xFFFFFFFF)))
    return 0 if rc == 0 else 1


def read_suite(path):
    """[(name, level or None, frames, input script)] from a suite file."""
    out = []
    for line in open(path):
        f = line.split("#", 1)[0].split()
        if f:
            out.append((f[0], None if f[1] == "-" else f[1], int(f[2]),
                        os.path.join(os.path.dirname(os.path.abspath(path)), f[3])))
    return out


def do_suite(a):
    os.makedirs(a.outdir, exist_ok=True)
    rc = 0
    for name, level, frames, script in read_suite(a.suite):
        if a.only and name not in a.only:
            continue
        r = argparse.Namespace(exe=a.exe, out=os.path.join(a.outdir, name + ".txt"), level=level,
                               frames=frames, input=script, record=None, timeout=a.timeout, mode=a.mode,
                               dx6=a.dx6)
        rc |= do_run(r)
    return rc


def parse(line):
    parts = line.split()
    return int(parts[0]), parts[1], dict(p.split("=", 1) for p in parts[2:])


def do_diff(a):
    if os.path.isdir(a.a):
        rc = 0
        for name in sorted(n for n in os.listdir(a.a) if n.endswith(".txt")):
            print("%s:" % name[:-4])
            b = os.path.join(a.b, name)
            if not os.path.exists(b):
                print("  missing in %s" % a.b)
                rc = 1
                continue
            rc |= diff_files(os.path.join(a.a, name), b, a.report)
        return rc
    return diff_files(a.a, a.b, a.report)


def diff_files(path_a, path_b, report):
    la, lb = open(path_a).read().splitlines(), open(path_b).read().splitlines()
    reported = {}                                   # field -> first frame where it differs
    rc = 0
    for x, y in zip(la, lb):
        if x == y:
            continue
        fa, ha, da = parse(x)
        fb, hb, db = parse(y)
        keys = [k for k in list(da) + [k for k in db if k not in da] if da.get(k) != db.get(k)]
        for k in keys:
            if k in report and k not in reported:
                reported[k] = (fa, da.get(k), db.get(k))
        gating = [k for k in keys if k not in report]
        if gating:
            print("first difference at frame %d (mode %s, level %s)" % (fa, da.get("mode"), da.get("lvl")))
            for k in gating:
                print("  %-14s %s | %s" % (k, da.get(k), db.get(k)))
            rc = 1
            break
    else:
        if len(la) != len(lb):
            print("identical for %d frames, then lengths differ: %d | %d" % (min(len(la), len(lb)), len(la), len(lb)))
            rc = 1
        elif report:
            print("identical: %d frames (except reported fields)" % len(la))
        else:
            print("identical: %d frames" % len(la))
    for k in report:
        if k in reported:
            print("  reported: %-14s first difference at frame %d (%s | %s)" % ((k,) + reported[k]))
        elif rc == 0:
            print("  reported: %-14s identical" % k)
    return rc


def main():
    ap = argparse.ArgumentParser(description=__doc__, formatter_class=argparse.RawDescriptionHelpFormatter)
    sub = ap.add_subparsers(dest="cmd", required=True)
    r = sub.add_parser("run")
    r.add_argument("exe")
    r.add_argument("out")
    r.add_argument("--level", default="Morb_00", help='"-": none, the input script starts in the menu')
    r.add_argument("--frames", type=int, default=1800)
    r.add_argument("--input", default=DEFAULT_INPUT)
    r.add_argument("--record")
    r.add_argument("--timeout", type=float, default=300)
    r.add_argument("--mode", help="display mode WxH (default: the install's ubi.ini, 1920x1080)")
    r.add_argument("--dx6", action="store_true", help="the Direct3D 6 driver (docs/PLAYTEST.md DX6 keys)")
    u = sub.add_parser("suite")
    u.add_argument("exe")
    u.add_argument("outdir")
    u.add_argument("--suite", default=DEFAULT_SUITE)
    u.add_argument("--only", action="append", default=[], metavar="NAME", help="scenario to run (repeatable)")
    u.add_argument("--timeout", type=float, default=300)
    u.add_argument("--mode", help="display mode WxH (default: the install's ubi.ini, 1920x1080)")
    u.add_argument("--dx6", action="store_true", help="the Direct3D 6 driver (docs/PLAYTEST.md DX6 keys)")
    d = sub.add_parser("diff")
    d.add_argument("a")
    d.add_argument("b")
    d.add_argument("--report", action="append", default=[], metavar="FIELD",
                   help="field reported at its first difference, not gating (repeatable)")
    a = ap.parse_args()
    return {"run": do_run, "suite": do_suite, "diff": do_diff}[a.cmd](a)


if __name__ == "__main__":
    sys.exit(main())
