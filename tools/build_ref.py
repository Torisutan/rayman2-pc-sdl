#!/usr/bin/env python3
"""VC5 reference build of this fork: tools/build.py (upstream, unchanged) plus the fork-only code.

Upstream's build compiles exactly Rayman2.exe/tu.json. The fork's own translation units
(Rayman2.exe/src/port) and defines are added here instead, so tu.json and build.py stay as
upstream has them.

  python tools/build_ref.py [--crack] [--out build/ref] [--define NAME ...]

RAYMAN2_REGRESS (the regression harness) is always on. Paths come from local.json as for
tools/build.py.
"""
import os
import sys

HERE = os.path.dirname(os.path.abspath(__file__))
sys.path.insert(0, HERE)
import build  # noqa: E402

FORK_TUS = ["src/port/regress.c"]


def main():
    argv, defines = [], ["RAYMAN2_REGRESS"]
    args = iter(sys.argv[1:])
    for a in args:
        if a == "--define":
            defines.append(next(args))
        else:
            argv.append(a)
    if "--out" not in argv:
        argv += ["--out", os.path.join(build.ROOT, "build", "ref")]
    upstream_load = build.load_tus

    def load_tus():
        tus, base = upstream_load()
        tus = tus + [{"src": s, "flags": [], "toolchain": "vc5sp3"} for s in FORK_TUS]
        return tus, base + ["/D" + d for d in defines]
    build.load_tus = load_tus
    sys.argv = [sys.argv[0]] + argv
    return build.main()


if __name__ == "__main__":
    sys.exit(main())
