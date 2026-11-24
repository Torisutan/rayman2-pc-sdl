#!/usr/bin/env python3
"""Build Rayman2.exe with the retail toolchain (MSVC 5.0 SP3 cl 11.00.7022 + LINK 5.10).

Every TU in Rayman2.exe/tu.json is compiled with base_flags plus its own flags and toolchain,
then linked in tu.json order (the order decides the exe layout).

Not shipped, bring your own (given once, then remembered in local.json):
  --vc5 DIR     MSVC 5.0 SP3: DIR/bin (cl.exe, link.exe), DIR/include, DIR/lib, DIR/redist
  --vc6 DIR     Visual C++ 6.0 (DIR/VC98/...): DINPUT.LIB + DXGUID.LIB (VC5 ships neither) and
                the VC6 cl for the TUs tu.json marks "vc6-8168"
  --gog EXE     your GOG Rayman2.exe: its .rsrc (icons + manifest) for the link, and the
                reference for crack.py / byte_match.py / check.py

Output: build/Rayman2.exe (the original image, copy protection included) and build/Rayman2.map;
with --crack also build/Rayman2_cracked.exe (tools/crack.py), which runs from the game folder.

Usage: python tools/build.py [--vc5 DIR] [--vc6 DIR] [--gog EXE] [--crack] [--out build] [--jobs N]
"""
import argparse
import concurrent.futures as cf
import json
import os
import struct
import subprocess
import sys

ROOT = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))
EXE_DIR = os.path.join(ROOT, "Rayman2.exe")
SHIM = os.path.join(EXE_DIR, "shim")
INC = os.path.join(EXE_DIR, "include")
LOCAL = os.path.join(ROOT, "local.json")
BUILD = os.path.join(ROOT, "build")


def add_path_args(ap):
    ap.add_argument("--vc5", help="MSVC 5.0 SP3 directory")
    ap.add_argument("--vc6", help="Visual C++ 6.0 directory (contains VC98/)")
    ap.add_argument("--gog", help="your GOG Rayman2.exe")


def resolve_paths(a, need=("vc5", "vc6", "gog")):
    """Fill --vc5/--vc6/--gog (/--gli/--ml/--wav) from local.json; remember the ones given on the command line."""
    cfg = json.load(open(LOCAL)) if os.path.exists(LOCAL) else {}
    changed = False
    for k in ("vc5", "vc6", "gog", "gli", "ml", "wav"):
        v = getattr(a, k, None)
        if v:
            v = os.path.abspath(v)
            changed |= cfg.get(k) != v
            cfg[k] = v
        setattr(a, k, cfg.get(k))
        if k in need and not getattr(a, k):
            sys.exit("--%s is required (once; it is then remembered in local.json)" % k)
    if changed:
        with open(LOCAL, "w") as f:
            json.dump(cfg, f, indent=1)
    return a


def load_tus():
    cfg = json.load(open(os.path.join(EXE_DIR, "tu.json")))
    return cfg["link_order"], cfg["base_flags"]


def env(a, toolchain):
    e = dict(os.environ)
    if toolchain == "vc5sp3":
        # redist/ supplies msvcp50.dll, which link.exe needs through msdis100.dll.
        e["PATH"] = os.pathsep.join([os.path.join(a.vc5, "bin"), os.path.join(a.vc5, "redist"), e.get("PATH", "")])
        e["INCLUDE"] = ";".join([SHIM, os.path.join(a.vc5, "include"), INC])
        e["LIB"] = os.path.join(a.vc5, "lib")
    elif toolchain == "vc6-8168":
        e["PATH"] = os.pathsep.join([os.path.join(a.vc6, "VC98", "Bin"),
                                     os.path.join(a.vc6, "Common", "MSDev98", "Bin"), e.get("PATH", "")])
        e["INCLUDE"] = ";".join([SHIM, os.path.join(a.vc6, "VC98", "Include"), INC])
    else:
        sys.exit("unknown toolchain %r in tu.json" % toolchain)
    return e


def cl_exe(a, toolchain):
    if toolchain == "vc5sp3":
        return os.path.join(a.vc5, "bin", "cl.exe")
    return os.path.join(a.vc6, "VC98", "Bin", "CL.EXE")


def obj_path(out, src):
    return os.path.join(out, os.path.dirname(src), os.path.splitext(os.path.basename(src))[0] + ".obj")


def compile_one(a, base, tu, out, flags=None):
    """Compile one tu.json entry into `out`. -> (src, obj, error or None)."""
    obj = obj_path(out, tu["src"])
    os.makedirs(os.path.dirname(obj), exist_ok=True)
    if os.path.exists(obj):
        os.remove(obj)
    fl = tu["flags"] if flags is None else flags
    cmd = [cl_exe(a, tu["toolchain"])] + base + ["/" + f.lstrip("/") for f in fl] + \
          ["/Fo" + obj, os.path.join(EXE_DIR, tu["src"])]
    p = subprocess.run(cmd, cwd=os.path.dirname(obj), env=env(a, tu["toolchain"]),
                       capture_output=True, text=True, errors="replace")
    if p.returncode == 0 and os.path.exists(obj):
        return tu["src"], obj, None
    text = p.stdout + p.stderr
    lines = [l for l in text.splitlines() if "error" in l.lower() or "fatal" in l.lower()]
    return tu["src"], obj, (lines[0] if lines else text.strip()[-200:]).strip()


def resources(data):
    """[(type, name, lang, bytes)] from a PE32 resource directory; type/name are int or str."""
    pe = struct.unpack_from("<I", data, 0x3C)[0]
    nsec = struct.unpack_from("<H", data, pe + 6)[0]
    opt = struct.unpack_from("<H", data, pe + 20)[0]
    assert struct.unpack_from("<H", data, pe + 24)[0] == 0x10B, "PE32 only"
    rsrc = struct.unpack_from("<I", data, pe + 24 + 96 + 2 * 8)[0]
    secs = []
    for k in range(nsec):
        vsize, rva, rawsize, rawptr = struct.unpack_from("<IIII", data, pe + 24 + opt + 40 * k + 8)
        secs.append((rva, max(vsize, rawsize), rawptr))

    def off(rva):
        for s_rva, size, raw in secs:
            if s_rva <= rva < s_rva + size:
                return raw + rva - s_rva
        raise ValueError("RVA 0x%X outside every section" % rva)

    base = off(rsrc)

    def ident(v):
        if v & 0x80000000:
            o = base + (v & 0x7FFFFFFF)
            n = struct.unpack_from("<H", data, o)[0]
            return data[o + 2:o + 2 + 2 * n].decode("utf-16-le")
        return v

    def entries(d):
        named, ids = struct.unpack_from("<HH", data, base + d + 12)
        for k in range(named + ids):
            name, target = struct.unpack_from("<II", data, base + d + 16 + 8 * k)
            yield ident(name), target & 0x7FFFFFFF

    out = []
    for typ, t1 in entries(0):
        for name, t2 in entries(t1):
            for lang, t3 in entries(t2):
                rva, size = struct.unpack_from("<II", data, base + t3)
                o = off(rva)
                out.append((typ, name, lang, data[o:o + size]))
    return out


def res_bytes(gog):
    """The GOG exe's .rsrc as a .res file LINK 5.10 accepts."""
    def rid(v):
        return struct.pack("<HH", 0xFFFF, v) if isinstance(v, int) else v.upper().encode("utf-16-le") + b"\0\0"
    out = bytearray(struct.pack("<IIHHHHIHHII", 0, 32, 0xFFFF, 0, 0xFFFF, 0, 0, 0, 0, 0, 0))
    for typ, name, lang, blob in resources(open(gog, "rb").read()):
        names = rid(typ) + rid(name)
        names += b"\0" * (-len(names) % 4)
        tail = struct.pack("<IHHII", 0, 0x1030, lang, 0, 0)
        out += struct.pack("<II", len(blob), 8 + len(names) + len(tail)) + names + tail + blob
        out += b"\0" * (-len(out) % 4)
    return bytes(out)


def main():
    ap = argparse.ArgumentParser(description=__doc__, formatter_class=argparse.RawDescriptionHelpFormatter)
    add_path_args(ap)
    ap.add_argument("--crack", action="store_true", help="also write Rayman2_cracked.exe (tools/crack.py)")
    ap.add_argument("--out", default=BUILD)
    ap.add_argument("--jobs", type=int, default=6)
    a = resolve_paths(ap.parse_args())
    out = os.path.abspath(a.out)
    tus, base = load_tus()
    for dp, _, fs in os.walk(out):
        for f in fs:
            if f.endswith(".obj"):
                os.remove(os.path.join(dp, f))
    with cf.ThreadPoolExecutor(a.jobs) as ex:
        fails = [r for r in ex.map(lambda t: compile_one(a, base, t, out), tus) if r[2]]
    print("compile: %d/%d OK" % (len(tus) - len(fails), len(tus)))
    for src, _obj, err in fails:
        print("  FAIL %s: %s" % (src, err))
    if fails:
        return 1

    exe = os.path.join(out, "Rayman2.exe")
    rsp = os.path.join(out, "link.rsp")
    with open(rsp, "w") as f:
        f.write("\n".join('"%s"' % obj_path(out, t["src"]) for t in tus) + "\n")
    res = os.path.join(out, "Rayman2.res")
    with open(res, "wb") as f:
        f.write(res_bytes(a.gog))
    exports = [l.strip() for l in open(os.path.join(EXE_DIR, "exports.txt")) if l.strip()]
    lib6 = os.path.join(a.vc6, "VC98", "Lib")
    # /OPT:NOREF keeps every function; /OPT:ICF folds identical COMDATs as retail did.
    cmd = [os.path.join(a.vc5, "bin", "link.exe"), "/NOLOGO", "/SUBSYSTEM:WINDOWS", "/MACHINE:IX86",
           "/OPT:NOREF", "/OPT:ICF", "/MAP:" + os.path.join(out, "Rayman2.map"), "/OUT:" + exe] + \
        ["/EXPORT:" + n for n in exports] + ["@" + rsp, res] + \
        ["/NODEFAULTLIB:libc.lib", "msvcrt.lib", "kernel32.lib", "user32.lib", "gdi32.lib", "winmm.lib",
         "advapi32.lib", "shell32.lib", os.path.join(lib6, "DINPUT.LIB"), os.path.join(lib6, "DXGUID.LIB")]
    p = subprocess.run(cmd, cwd=out, env=env(a, "vc5sp3"), capture_output=True, text=True, errors="replace")
    if p.returncode or not os.path.exists(exe):
        print("LINK FAILED")
        print((p.stdout + p.stderr).strip()[-3000:])
        return 1
    print("LINK OK -> %s (%d bytes)" % (exe, os.path.getsize(exe)))
    if a.crack:
        sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
        import crack
        cracked = os.path.join(out, "Rayman2_cracked.exe")
        patches, _ = crack.apply(exe, cracked, os.path.join(out, "Rayman2.map"), a.gog)
        print("crack: %d patches -> %s" % (len(patches), cracked))
    return 0


if __name__ == "__main__":
    sys.exit(main())
