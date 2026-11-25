#!/usr/bin/env python3
"""Regression run: same input, same engine state hash per frame, in any build.

The exe must be built with the harness (RAYMAN2_REGRESS: CMake option R2_REGRESS, on by default;
tools/build_ref.py for the VC5 reference). See Rayman2.exe/src/app/regress.c for what is pinned
and hashed.

  python tools/regress.py run EXE OUT [--level Morb_00] [--frames 1800] [--input SCRIPT]
                                      [--record SCRIPT] [--timeout 300]
      run EXE in the sandbox (tools/run.py) and write the per-frame hash file OUT
  python tools/regress.py diff A B [--report FIELD ...]
      compare two hash files: first differing frame and the fields that differ there.
      --report FIELD: FIELD does not gate; its first difference is reported. Use it to compare
      builds whose arithmetic differs (docs/PLAN.md Phase 2: modern build against VC5 with
      --report actor.matrix); the modern build gates against its own baseline.

Exit code 0 = run complete / files identical.
"""
import argparse
import os
import subprocess
import sys
import time

HERE = os.path.dirname(os.path.abspath(__file__))
sys.path.insert(0, HERE)
import run  # noqa: E402

DEFAULT_INPUT = os.path.join(HERE, "regress", "morb_00.txt")


def do_run(a):
    subprocess.run(["taskkill", "/F", "/IM", "Rayman2.exe"], capture_output=True)
    time.sleep(0.5)
    exe = run.build_sandbox(os.path.abspath(a.exe))
    if a.mode:
        run.set_mode(os.path.join(run.BOX, "ubi.ini"), a.mode)
    out = os.path.abspath(a.out)
    if os.path.exists(out):
        os.remove(out)
    env = dict(os.environ, R2_REGRESS_OUT=out, R2_REGRESS_FRAMES=str(a.frames))
    if a.level:
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


def parse(line):
    parts = line.split()
    return int(parts[0]), parts[1], dict(p.split("=", 1) for p in parts[2:])


def do_diff(a):
    la, lb = open(a.a).read().splitlines(), open(a.b).read().splitlines()
    reported = {}                                   # field -> first frame where it differs
    rc = 0
    for x, y in zip(la, lb):
        if x == y:
            continue
        fa, ha, da = parse(x)
        fb, hb, db = parse(y)
        keys = [k for k in list(da) + [k for k in db if k not in da] if da.get(k) != db.get(k)]
        for k in keys:
            if k in a.report and k not in reported:
                reported[k] = (fa, da.get(k), db.get(k))
        gating = [k for k in keys if k not in a.report]
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
        elif a.report:
            print("identical: %d frames (except reported fields)" % len(la))
        else:
            print("identical: %d frames" % len(la))
    for k in a.report:
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
    r.add_argument("--level", default="Morb_00")
    r.add_argument("--frames", type=int, default=1800)
    r.add_argument("--input", default=DEFAULT_INPUT)
    r.add_argument("--record")
    r.add_argument("--timeout", type=float, default=300)
    r.add_argument("--mode", help="display mode WxH (default: the install's ubi.ini, 1920x1080)")
    d = sub.add_parser("diff")
    d.add_argument("a")
    d.add_argument("b")
    d.add_argument("--report", action="append", default=[], metavar="FIELD",
                   help="field reported at its first difference, not gating (repeatable)")
    a = ap.parse_args()
    return do_run(a) if a.cmd == "run" else do_diff(a)


if __name__ == "__main__":
    sys.exit(main())
