#!/usr/bin/env python3
"""Build GliDX6vf.dll (the Direct3D 6 renderer DLL) with the retail toolchain, and check it.

Toolchain: MSVC 5.0 SP3 cl /O2 /Gy /MD, LINK 5.10 /DLL at 0x10000000, against VC5's msvcrt.lib,
kernel32.lib, user32.lib and the DirectX 6 SDK ddraw.lib. That ddraw.lib is rebuilt here from the
VC6 DDRAW.LIB (the same library one DirectX release earlier) plus the two DirectX 6 imports it lacks,
DirectDrawEnumerateExA/ExW, whose hints (10, 11) are the gap the VC6 lib leaves.

Every TU in GliDX6vf.dll/tu.json is compiled in link order (the order decides the DLL layout).
GLI_DX6_FIXES stays undefined: that is the original driver. Code under #ifdef GLI_DX6_FIXES holds
fixes (Windows 10 window handling, NULL guards, diagnostics) for builds that do not aim at bytes.

Paths (given once, then remembered in local.json):
  --vc5 DIR     MSVC 5.0 SP3 (bin/, include/, lib/, redist/)
  --vc6 DIR     Visual C++ 6.0 (VC98/Lib/DDRAW.LIB)
  --gli DLL     your game's GliDX6vf.dll (GOG install: DLL/GliDX6vf.dll), for --check

Output: build/GliDX6vf/GliDX6vf.dll and .map.
--check: every function of GliDX6vf.dll/status.json at its link address against the original DLL,
relocation sites (from the TU's .obj) masked. Exit 1 if an EXACT row regressed.

Usage: python tools/build_glidx6.py [--vc5 DIR] [--vc6 DIR] [--gli DLL] [--check] [--out DIR]
"""
import argparse
import concurrent.futures as cf
import json
import os
import struct
import subprocess
import sys

HERE = os.path.dirname(os.path.abspath(__file__))
sys.path.insert(0, HERE)
import build  # noqa: E402  (local.json paths)
import objfn  # noqa: E402
import retail  # noqa: E402

DLL_DIR = os.path.join(build.ROOT, "GliDX6vf.dll")
SRC = os.path.join(DLL_DIR, "src")
INC = os.path.join(DLL_DIR, "include")
OUT = os.path.join(build.BUILD, "GliDX6vf")
ADD_IMPORTS = [("_DirectDrawEnumerateExA@12", 10, "_DirectDrawEnumerateA@8"),
               ("_DirectDrawEnumerateExW@12", 11, "_DirectDrawEnumerateExA@12")]


def ddraw_lib(vc6_lib, out):
    """VC6 DDRAW.LIB + the DirectX 6 short import members -> out (a COFF archive)."""
    b = open(vc6_lib, "rb").read()
    ms, i = [], 8
    while i < len(b):
        sz = int(b[i + 48:i + 58])
        ms.append((b[i:i + 60], b[i + 60:i + 60 + sz]))
        i += 60 + sz + (sz & 1)
    first, second, objs = ms[0], ms[1], ms[2:]
    d = second[1]
    n = struct.unpack_from("<I", d, 0)[0]
    offs = list(struct.unpack_from("<%dI" % n, d, 4))
    p = 4 + 4 * n
    ns = struct.unpack_from("<I", d, p)[0]
    idx = struct.unpack_from("<%dH" % ns, d, p + 4)
    names = d[p + 4 + 2 * ns:].split(b"\0")[:ns]
    cur, at = 8 + sum(60 + len(m[1]) + (len(m[1]) & 1) for m in ms[:2]), []
    for m in objs:
        at.append(cur)
        cur += 60 + len(m[1]) + (len(m[1]) & 1)
    syms = {k: [] for k in range(len(objs))}
    for nm, ix in zip(names, idx):
        syms[at.index(offs[ix - 1])].append(nm)
    entries = [(m, syms[k]) for k, m in enumerate(objs)]
    th, td = next(m for m in objs if m[1][:4] == b"\0\0\xff\xff")
    for sym, hint, after in ADD_IMPORTS:
        body = sym.encode() + b"\0DDRAW.dll\0"
        data = td[:4] + struct.pack("<HHIIHH", 0, 0x14C, struct.unpack_from("<I", td, 8)[0], len(body), hint, 12) + body
        pos = next(k for k, (_, s) in enumerate(entries) if after.encode() in s) + 1
        entries.insert(pos, ((th, data), [sym.encode(), b"__imp_" + sym.encode()]))
    allsyms = [(s, k) for k, (_, ss) in enumerate(entries) for s in ss]
    ssyms = sorted(allsyms, key=lambda x: x[0])
    strtab = b"".join(s + b"\0" for s, _ in allsyms)
    l1 = 4 + 4 * len(allsyms) + len(strtab)
    l2 = 8 + 4 * len(entries) + 2 * len(allsyms) + sum(len(s) + 1 for s, _ in ssyms)
    cur, moffs = 8 + 60 + l1 + (l1 & 1) + 60 + l2 + (l2 & 1), []
    for (_, dd), _ in entries:
        moffs.append(cur)
        cur += 60 + len(dd) + (len(dd) & 1)
    fd = struct.pack(">I", len(allsyms)) + b"".join(struct.pack(">I", moffs[k]) for _, k in allsyms) + strtab
    sd = struct.pack("<I", len(entries)) + struct.pack("<%dI" % len(entries), *moffs) + \
        struct.pack("<I", len(allsyms)) + struct.pack("<%dH" % len(allsyms), *[k + 1 for _, k in ssyms]) + \
        b"".join(s + b"\0" for s, _ in ssyms)

    def mem(h, dd):
        return h[:48] + ("%-10d" % len(dd)).encode() + h[58:] + dd + (b"\n" if len(dd) & 1 else b"")
    with open(out, "wb") as f:
        f.write(b"!<arch>\n" + mem(first[0], fd) + mem(second[0], sd) +
                b"".join(mem(h, dd) for (h, dd), _ in entries))


def load_tus():
    """-> (link order, base flags); sets INC to tu.json's `include` (header dirs from the repository root,
    in search order: the DLL's own, then the exe's shared GLI declarations) when it has one."""
    global INC
    cfg = json.load(open(os.path.join(DLL_DIR, "tu.json")))
    if "include" in cfg:
        INC = ";".join(os.path.join(build.ROOT, d) for d in cfg["include"])
    return [t["src"] for t in cfg["link_order"]], cfg["base_flags"]


def obj_path(out, src):
    return os.path.join(out, os.path.dirname(src), os.path.splitext(os.path.basename(src))[0] + ".obj")


def vc5_env(a):
    e = dict(os.environ)
    e["PATH"] = os.pathsep.join([os.path.join(a.vc5, "bin"), os.path.join(a.vc5, "redist"), e.get("PATH", "")])
    e["INCLUDE"] = ";".join([INC, os.path.join(a.vc5, "include")])
    e["LIB"] = os.path.join(a.vc5, "lib")
    return e


def compile_one(a, base, src, out):
    obj = obj_path(out, src)
    os.makedirs(os.path.dirname(obj), exist_ok=True)
    if os.path.exists(obj):
        os.remove(obj)
    p = subprocess.run([os.path.join(a.vc5, "bin", "cl.exe")] + base + ["/Fo" + obj, os.path.join(SRC, src)],
                       cwd=os.path.dirname(obj), env=vc5_env(a), capture_output=True, text=True, errors="replace")
    if p.returncode == 0 and os.path.exists(obj):
        return src, None
    lines = [l for l in (p.stdout + p.stderr).splitlines() if "error" in l.lower()]
    return src, (lines[0] if lines else (p.stdout + p.stderr).strip()[-200:]).strip()


def check(a, out):
    by_name = retail.read_map(os.path.join(out, "GliDX6vf.map"))
    ours = retail.Image(open(os.path.join(out, "GliDX6vf.dll"), "rb").read())
    orig = retail.Image(open(a.gli, "rb").read())
    rows = json.load(open(os.path.join(DLL_DIR, "status.json")))
    exact, regressed, improved, errors = 0, [], [], []
    for row in rows:
        obj = obj_path(out, row["src"])
        try:
            raw, relocs, _ = objfn.extract(obj, row["name"])
        except (OSError, ValueError) as ex:
            errors.append((row, str(ex)))
            continue
        n = row["len"]
        fixed = [i for i in range(n) if not any(r["off"] <= i < r["off"] + 4 for r in relocs)]
        va = retail.our_va(by_name, row["name"], os.path.basename(obj).lower())
        if va is None:     # /OPT:ICF folded identical bodies onto one address
            for cand, _ob in by_name.get(retail.norm(row["name"]) or "", []):
                if all(ours.read(cand, n)[i] == raw[i] for i in fixed if i < len(raw)):
                    va = cand
                    break
        if va is None:
            errors.append((row, "not in the linked DLL"))
            continue
        g, w = ours.read(va, n), orig.read(int(row["addr"], 16), n)
        ok = all(g[i] == w[i] for i in fixed)
        exact += ok
        if ok and row["verdict"] != "EXACT":
            improved.append(row)
        elif not ok and row["verdict"] == "EXACT":
            regressed.append(row)
    print("check: %d/%d functions byte-exact in the linked DLL" % (exact, len(rows)))
    for r in regressed:
        print("  REGRESSED %s %s" % (r["addr"], r["name"]))
    for r in improved:
        print("  now exact %s %s" % (r["addr"], r["name"]))
    for r, e in errors:
        print("  error %s %s: %s" % (r["addr"], r["name"], e))
    return not regressed


def main():
    ap = argparse.ArgumentParser(description=__doc__, formatter_class=argparse.RawDescriptionHelpFormatter)
    ap.add_argument("--vc5")
    ap.add_argument("--vc6")
    ap.add_argument("--gli", help="your game's GliDX6vf.dll (for --check)")
    ap.add_argument("--check", action="store_true")
    ap.add_argument("--out", default=OUT)
    ap.add_argument("--jobs", type=int, default=6)
    a = build.resolve_paths(ap.parse_args(), need=("vc5", "vc6") + (("gli",) if "--check" in sys.argv else ()))
    out = os.path.abspath(a.out)
    tus, base = load_tus()
    with cf.ThreadPoolExecutor(a.jobs) as ex:
        fails = [r for r in ex.map(lambda s: compile_one(a, base, s, out), tus) if r[1]]
    print("compile: %d/%d OK" % (len(tus) - len(fails), len(tus)))
    for src, err in fails:
        print("  FAIL %s: %s" % (src, err))
    if fails:
        return 1
    lib = os.path.join(out, "DDRAW.LIB")
    ddraw_lib(os.path.join(a.vc6, "VC98", "Lib", "DDRAW.LIB"), lib)
    dll = os.path.join(out, "GliDX6vf.dll")
    rsp = os.path.join(out, "link.rsp")
    with open(rsp, "w") as f:
        f.write("\n".join('"%s"' % obj_path(out, s) for s in tus) + "\n")
    cmd = [os.path.join(a.vc5, "bin", "link.exe"), "/NOLOGO", "/DLL", "/MACHINE:IX86", "/SUBSYSTEM:WINDOWS",
           "/BASE:0x10000000", "/MAP:" + os.path.join(out, "GliDX6vf.map"), "/OUT:" + dll, "@" + rsp,
           "msvcrt.lib", "kernel32.lib", "user32.lib", lib]
    p = subprocess.run(cmd, cwd=out, env=vc5_env(a), capture_output=True, text=True, errors="replace")
    if p.returncode or not os.path.exists(dll):
        print("LINK FAILED")
        print((p.stdout + p.stderr).strip()[-3000:])
        return 1
    print("LINK OK -> %s (%d bytes)" % (dll, os.path.getsize(dll)))
    if a.check:
        return 0 if check(a, out) else 1
    return 0


if __name__ == "__main__":
    sys.exit(main())
