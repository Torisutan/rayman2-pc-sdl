#!/usr/bin/env python3
"""Build WAVMWBVR.dll (the sound driver DLL: software mixer, 3D sample layer, waveOut output) with the
retail toolchain, and check it.

Toolchain: MSVC 5.0 SP3 cl (WAVMWBVR.dll/tu.json base_flags: /O2 /Gy /MT, static C runtime), the MMX
mixer (src/mixer/*.asm) assembled by ML 6.1x to OMF (LINK converts it, as in the retail build), the
version resource by VC5's rc, LINK 5.10 /DLL at 0x10000000 /DEBUG against VC5's winmm, kernel32 and
user32 libraries (LIBCMT comes in through the objects).

Every source in tu.json is built in link order (the order decides the DLL layout). The names of the
uninitialized globals are part of the layout: LINK places them in the order of the compiler's name
hash, so several PC-only names carry a numeric suffix that reproduces the original addresses.

Some header fields come from the link environment, not from the sources: the link time stamp, the
PDB path in the CodeView record and the output path in the MISC debug record. With --wav they are
copied from your original DLL after linking (only those fields), so the build can be compared whole.

Paths (given once, then remembered in local.json):
  --vc5 DIR     MSVC 5.0 SP3 (bin/, include/, lib/, redist/)
  --ml EXE      ml.exe 6.1x (for example VC98/Bin/ml.exe of Visual C++ 6.0 SP5 / the Processor Pack)
  --wav DLL     your game's WAVMWBVR.dll (GOG install: DLL/WAVMWBVR.dll), for --check and the header fields

Output: build/WAVMWBVR/WAVMWBVR.DLL and .map.
--check: every function of WAVMWBVR.dll/status.json at its link address against the original DLL,
relocation sites (from the TU's .obj) masked, plus a whole-file comparison. Exit 1 if an EXACT row regressed.

Usage: python tools/build_wavmw.py [--vc5 DIR] [--ml EXE] [--wav DLL] [--check] [--out DIR]
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

NAME = "WAVMWBVR"       # the DLL's directory is <NAME>.dll/, the output build/<NAME>/ (tools/build_apmmx.py
ARG = "wav"             # calls configure() for APMmxBVR.dll); ARG = the option naming your original DLL
DLL_DIR = SRC = INC = OUT = None


ML = "ml"               # the option naming the assembler (tools/build_apmmx.py: ml611d)


def configure(name, arg, ml="ml"):
    global NAME, ARG, ML, DLL_DIR, SRC, INC, OUT
    NAME, ARG, ML = name, arg, ml
    DLL_DIR = os.path.join(build.ROOT, name + ".dll")
    SRC = os.path.join(DLL_DIR, "src")
    INC = os.path.join(DLL_DIR, "include")
    OUT = os.path.join(build.BUILD, name)


configure(NAME, ARG)


def load_cfg():
    return json.load(open(os.path.join(DLL_DIR, "tu.json")))


def src_path(entry):
    """A link_order entry's source: <NAME>.dll/src/<src>, or `path` (from the repository root) for a source
    shared with another DLL's tree."""
    return os.path.join(build.ROOT, entry["path"]) if "path" in entry else os.path.join(SRC, entry["src"])


def obj_path(out, src):
    ext = ".res" if src.endswith(".rc") else ".obj"
    return os.path.join(out, os.path.dirname(src), os.path.splitext(os.path.basename(src))[0] + ext)


def vc5_env(a, cfg):
    e = dict(os.environ)
    e["PATH"] = os.pathsep.join([os.path.join(a.vc5, "bin"), os.path.join(a.vc5, "redist"), e.get("PATH", "")])
    inc = [os.path.join(build.ROOT, d) for d in cfg["include"]] if "include" in cfg else [INC]
    e["INCLUDE"] = ";".join(inc + [os.path.join(a.vc5, "include")])
    e["LIB"] = os.path.join(a.vc5, "lib")
    return e


def build_one(a, cfg, entry, out):
    src = entry["src"]
    obj = obj_path(out, src)
    os.makedirs(os.path.dirname(obj), exist_ok=True)
    if os.path.exists(obj):
        os.remove(obj)
    path = src_path(entry)
    if src.endswith(".c"):
        cmd = [os.path.join(a.vc5, "bin", "cl.exe")] + cfg["base_flags"] + ["/Fo" + obj, path]
    elif src.endswith(".asm"):
        cmd = [getattr(a, ML), "/nologo", "/c", "/Cp"] + cfg["asm_flags"] + ["/Fo" + obj, path]
    elif src.endswith(".rc"):
        cmd = [os.path.join(a.vc5, "bin", "rc.exe"), "/fo" + obj, path]
    else:
        return src, "unknown source type"
    p = subprocess.run(cmd, cwd=os.path.dirname(obj), env=vc5_env(a, cfg), capture_output=True, text=True,
                       errors="replace")
    if p.returncode == 0 and os.path.exists(obj):
        return src, None
    lines = [l for l in (p.stdout + p.stderr).splitlines() if "error" in l.lower()]
    return src, (lines[0] if lines else (p.stdout + p.stderr).strip()[-200:]).strip()


def _sections(b):
    pe = struct.unpack_from("<I", b, 0x3C)[0]
    nsec = struct.unpack_from("<H", b, pe + 6)[0]
    opt = struct.unpack_from("<H", b, pe + 20)[0]
    o = pe + 24
    secs = [struct.unpack_from("<IIII", b, o + opt + 40 * k + 8) for k in range(nsec)]   # vsize, va, rawsize, rawptr

    def off(rva):
        for vs, va, rs, rp in secs:
            if va <= rva < va + max(vs, rs):
                return rp + rva - va
        raise ValueError("rva %X outside the sections" % rva)
    return pe, o, off


def env_fields(path):
    """The link-environment fields of a DLL: time stamp, CodeView record, MISC name + reserved bytes."""
    b = open(path, "rb").read()
    pe, o, off = _sections(b)
    f = {"timestamp": struct.unpack_from("<I", b, pe + 8)[0]}
    drva, dsz = struct.unpack_from("<II", b, o + 96 + 6 * 8)
    for i in range(dsz // 28):
        e = off(drva) + 28 * i
        typ, size = struct.unpack_from("<II", b, e + 12)
        ptr = struct.unpack_from("<I", b, e + 24)[0]
        if typ == 2:
            f["cv"] = b[ptr:ptr + size]
        elif typ == 4:
            f["misc"] = b[ptr + 9:ptr + size]
    return f


def stamp(dll, ref):
    """Write the link-environment fields of the original DLL `ref` into `dll` (nothing else)."""
    f = env_fields(ref)
    b = bytearray(open(dll, "rb").read())
    pe, o, off = _sections(b)
    ts = f["timestamp"]
    struct.pack_into("<I", b, pe + 8, ts)
    erva = struct.unpack_from("<I", b, o + 96)[0]
    if erva:
        struct.pack_into("<I", b, off(erva) + 4, ts)
    drva, dsz = struct.unpack_from("<II", b, o + 96 + 6 * 8)
    for i in range(dsz // 28):
        e = off(drva) + 28 * i
        struct.pack_into("<I", b, e + 4, ts)
        typ, size = struct.unpack_from("<II", b, e + 12)
        ptr = struct.unpack_from("<I", b, e + 24)[0]
        if typ == 2 and "cv" in f:
            rec = f["cv"]
            if ptr + len(rec) > len(b):
                raise ValueError("CodeView record does not fit")
            b[ptr:ptr + size] = bytes(size)
            b[ptr:ptr + len(rec)] = rec
            struct.pack_into("<I", b, e + 16, len(rec))
        elif typ == 4 and "misc" in f:
            m = f["misc"][:size - 9]
            b[ptr + 9:ptr + 9 + len(m)] = m
    if struct.unpack_from("<I", b, o + 64)[0]:                # checksum, only if the image has one
        struct.pack_into("<I", b, o + 64, 0)
        s = 0
        for k in range(0, len(b) - len(b) % 2, 2):
            s += struct.unpack_from("<H", b, k)[0]
            s = (s & 0xFFFF) + (s >> 16)
        struct.pack_into("<I", b, o + 64, (s & 0xFFFF) + len(b))
    open(dll, "wb").write(b)


def check(a, out, cfg):
    by_name = retail.read_map(os.path.join(out, NAME + ".map"))
    ours_raw = open(os.path.join(out, cfg["out_name"]), "rb").read()
    orig_raw = open(getattr(a, ARG), "rb").read()
    ours, orig = retail.Image(ours_raw), retail.Image(orig_raw)
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
    nd = sum(x != y for x, y in zip(ours_raw, orig_raw)) + abs(len(ours_raw) - len(orig_raw))
    print("whole file: %s" % ("IDENTICAL" if ours_raw == orig_raw else "%d bytes differ" % nd))
    return not regressed


def main(doc=__doc__):
    ap = argparse.ArgumentParser(description=doc, formatter_class=argparse.RawDescriptionHelpFormatter)
    ap.add_argument("--vc5")
    ap.add_argument("--" + ML, help="ml.exe (%s)" % ("6.1x" if ML == "ml" else ML[2:]))
    ap.add_argument("--" + ARG, help="your game's %s.dll (for --check and the header fields)" % NAME)
    ap.add_argument("--check", action="store_true")
    ap.add_argument("--out", default=OUT)
    ap.add_argument("--jobs", type=int, default=6)
    a = build.resolve_paths(ap.parse_args(), need=("vc5", ML) + ((ARG,) if "--check" in sys.argv else ()))
    out = os.path.abspath(a.out)
    cfg = load_cfg()
    entries = cfg["link_order"]
    srcs = [t["src"] for t in entries]
    with cf.ThreadPoolExecutor(a.jobs) as ex:
        fails = [r for r in ex.map(lambda t: build_one(a, cfg, t, out), entries) if r[1]]
    print("compile: %d/%d OK" % (len(srcs) - len(fails), len(srcs)))
    for src, err in fails:
        print("  FAIL %s: %s" % (src, err))
    if fails:
        return 1
    dll = os.path.join(out, cfg["out_name"])
    rsp = os.path.join(out, "link.rsp")
    with open(rsp, "w") as f:
        f.write("\n".join('"%s"' % obj_path(out, s) for s in srcs) + "\n")
    cmd = [os.path.join(a.vc5, "bin", "link.exe"), "/NOLOGO", "/DLL", "/MACHINE:IX86", "/SUBSYSTEM:WINDOWS",
           "/BASE:0x10000000", "/MAP:" + os.path.join(out, NAME + ".map"), "/OUT:" + dll] + cfg["ldflags"] + \
        ["@" + rsp] + cfg["libs"]
    p = subprocess.run(cmd, cwd=out, env=vc5_env(a, cfg), capture_output=True, text=True, errors="replace")
    if p.returncode or not os.path.exists(dll):
        print("LINK FAILED")
        print((p.stdout + p.stderr).strip()[-3000:])
        return 1
    orig = getattr(a, ARG)
    if orig:
        stamp(dll, orig)
    print("LINK OK -> %s (%d bytes)%s" % (dll, os.path.getsize(dll), "; header fields from " + orig if orig else ""))
    if a.check:
        return 0 if check(a, out, cfg) else 1
    return 0


if __name__ == "__main__":
    sys.exit(main())
