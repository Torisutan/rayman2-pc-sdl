#!/usr/bin/env python3
"""Run a built Rayman2.exe in a sandbox and report whether it comes up (behaviour smoke test).

The sandbox (build/run) borrows the game data of your install (the folder of the GOG exe named in
local.json) without ever writing into it: DLL/, Data/World/Sound and every Data/World/Levels/<map>
are junctions, the other data files hardlinks (the game only reads them), and Data/Options,
Data/SaveGame and ubi.ini are fresh copies before every run. The exe is copied into the sandbox
and started there (it expects the game folder as its own folder and working directory).

  python tools/run.py EXE                       alive + visible window after --seconds?
  python tools/run.py EXE --level Morb_00       once the menu plays, load that level directly
  python tools/run.py EXE --shot 10,20          screen captures at those seconds (build/run/shots)
  python tools/run.py EXE --mode 1440x1080      display mode written into the sandbox's ubi.ini
  python tools/run.py EXE --keys 12:esc,13:down key presses at those seconds (DirectInput scancodes)
  python tools/run.py EXE --ini GLI_DllFile=GliDX6  a key of the sandbox's ubi.ini set (repeatable)

The direct level load writes the level-change request into the engine structure, as the engine's
own level change does (GAM_fn_vAskToChangeLevel): szNextLevelName, the mode-6 save flag, then
eEngineMode = 6. Addresses come from the exe's LINK map (EXE with .map next to it).

Exit code 0 = alive with a visible window at the end (and the level loaded, with --level).
"""
import argparse
import ctypes
import ctypes.wintypes as wt
import json
import os
import shutil
import subprocess
import sys
import time

ROOT = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))
BOX = os.path.join(ROOT, "build", "run")
DATA_RW = ("Options", "SaveGame")       # what the game writes: fresh copies per run

user32 = ctypes.windll.user32
kernel32 = ctypes.windll.kernel32
psapi = ctypes.windll.psapi


def game_dir():
    cfg = json.load(open(os.path.join(ROOT, "local.json")))
    if not cfg.get("gog"):
        sys.exit("local.json has no gog entry (run tools/build.py --gog EXE once)")
    return os.path.dirname(cfg["gog"])


def junction(link, target):
    if not os.path.exists(link):
        subprocess.check_call(["cmd", "/c", "mklink", "/J", link, target], stdout=subprocess.DEVNULL)


def mirror(dst, src):
    """dst mirrors src: subdirectories as junctions, files as hardlinks."""
    os.makedirs(dst, exist_ok=True)
    for name in os.listdir(src):
        s, d = os.path.join(src, name), os.path.join(dst, name)
        if os.path.isdir(s):
            junction(d, s)
        elif not os.path.exists(d):
            os.link(s, d)


def set_ini(ini, key, value):
    """KEY=value in the sandbox's ubi.ini (replaced, or added to its first section)."""
    lines = open(ini).read().splitlines()
    hit = [i for i, l in enumerate(lines) if l.split("=", 1)[0].strip().lower() == key.lower()]
    if hit:
        lines[hit[0]] = "%s=%s" % (key, value)
    else:
        first = next(i for i, l in enumerate(lines) if l.startswith("["))
        lines.insert(first + 1, "%s=%s" % (key, value))
    open(ini, "w").write("".join(l + chr(10) for l in lines))


def set_mode(ini, mode):
    """GLI_Mode line of the sandbox's ubi.ini set to mode ("WxH")."""
    w, h = (int(v) for v in mode.lower().split("x"))
    set_ini(ini, "GLI_Mode", "1 - %d x %d x 32" % (w, h))


SCAN = {"esc": 0x01, "enter": 0x1C, "space": 0x39, "up": 0xE048, "down": 0xE050, "left": 0xE04B,
        "right": 0xE04D, "a": 0x1E, "j": 0x24, "q": 0x10, "tab": 0x0F, "f1": 0x3B}


class KEYBDINPUT(ctypes.Structure):
    _fields_ = [("wVk", wt.WORD), ("wScan", wt.WORD), ("dwFlags", wt.DWORD), ("time", wt.DWORD),
                ("dwExtraInfo", ctypes.c_size_t)]


class INPUT(ctypes.Structure):
    _fields_ = [("type", wt.DWORD), ("ki", KEYBDINPUT), ("pad", ctypes.c_ubyte * 8)]


def press(key, hold=0.15):
    """One key press by scancode (DirectInput reads scancodes, not virtual keys)."""
    code = SCAN[key] if key in SCAN else int(key, 16)
    flags = 0x0008 | (0x0001 if code > 0xFF else 0)          # SCANCODE | EXTENDEDKEY
    for up in (0, 0x0002):
        i = INPUT(1, KEYBDINPUT(0, code & 0xFF, flags | up, 0, 0))
        user32.SendInput(1, ctypes.byref(i), ctypes.sizeof(INPUT))
        if not up:
            time.sleep(hold)


def build_dll_folder(exe, game):
    """DLL/: a junction to the install's, or, when the build has driver modules of its own (DLL/
    beside the exe, e.g. GliGL3vf.dll), a folder of hardlinks to the install's files with the
    build's copied over them. Libraries beside the exe (SDL3.dll) are copied beside the sandbox's."""
    box, gdll = os.path.join(BOX, "DLL"), os.path.join(game, "DLL")
    own = os.path.join(os.path.dirname(exe), "DLL")
    if os.path.isjunction(box) != (not os.path.isdir(own)) and os.path.lexists(box):
        if os.path.isjunction(box):
            os.rmdir(box)                       # the link only, never the install's folder
        else:
            for name in os.listdir(box):        # hardlinks and copies only (mirror() of files)
                os.remove(os.path.join(box, name))
            os.rmdir(box)
    if os.path.isdir(own):
        os.makedirs(box, exist_ok=True)
        for name in os.listdir(gdll):
            s, d = os.path.join(gdll, name), os.path.join(box, name)
            if os.path.isfile(s) and not os.path.exists(d):
                os.link(s, d)
        for name in os.listdir(own):
            d = os.path.join(box, name)
            if os.path.exists(d):
                os.remove(d)                    # a hardlink of the install's file: unlinked only
            shutil.copyfile(os.path.join(own, name), d)
    else:
        junction(box, gdll)
    for name in os.listdir(os.path.dirname(exe)):
        if name.lower().endswith(".dll"):
            shutil.copyfile(os.path.join(os.path.dirname(exe), name), os.path.join(BOX, name))


def build_sandbox(exe):
    game = game_dir()
    gdata = os.path.join(game, "Data")
    os.makedirs(BOX, exist_ok=True)
    build_dll_folder(exe, game)
    data = os.path.join(BOX, "Data")
    os.makedirs(data, exist_ok=True)
    for name in os.listdir(gdata):
        s, d = os.path.join(gdata, name), os.path.join(data, name)
        if name in DATA_RW:
            if os.path.exists(d):
                shutil.rmtree(d)
            shutil.copytree(s, d)
        elif name == "World":
            os.makedirs(d, exist_ok=True)
            junction(os.path.join(d, "Sound"), os.path.join(s, "Sound"))
            mirror(os.path.join(d, "Levels"), os.path.join(s, "Levels"))
        elif os.path.isdir(s):
            junction(d, s)
        elif not os.path.exists(d):
            os.link(s, d)
    shutil.copyfile(os.path.join(game, "ubi.ini"), os.path.join(BOX, "ubi.ini"))
    dst = os.path.join(BOX, "Rayman2.exe")
    shutil.copyfile(exe, dst)
    return dst


def windows_of(pid):
    out = []
    proto = ctypes.WINFUNCTYPE(wt.BOOL, wt.HWND, wt.LPARAM)

    def cb(h, _):
        p = wt.DWORD()
        user32.GetWindowThreadProcessId(h, ctypes.byref(p))
        if p.value == pid and user32.IsWindowVisible(h):
            buf = ctypes.create_unicode_buffer(256)
            user32.GetWindowTextW(h, buf, 256)
            out.append((h, buf.value))
        return True
    user32.EnumWindows(proto(cb), 0)
    return out


def shoot(path):
    """Primary screen into path. The GL renderer presents through independent flip, which GDI
    captures as white, so this uses Desktop Duplication (ffmpeg ddagrab)."""
    r = subprocess.run(["ffmpeg", "-hide_banner", "-loglevel", "error", "-y", "-f", "lavfi",
                        "-i", "ddagrab=output_idx=0:framerate=10", "-vf", "hwdownload,format=bgra",
                        "-frames:v", "1", path], capture_output=True, text=True)
    return r.returncode == 0 and os.path.exists(path)


def map_symbols(map_path):
    """{name without leading _ / @n: VA} from a LINK map (VC5 and current link)."""
    syms = {}
    for line in open(map_path, errors="replace"):
        p = line.split()
        if len(p) >= 3 and ":" in p[0] and len(p[2]) == 8:
            try:
                va = int(p[2], 16)
            except ValueError:
                continue
            syms.setdefault(p[1].lstrip("_").split("@")[0], va)
    return syms


def image_base(hproc):
    mods = (ctypes.c_void_p * 1)()
    need = wt.DWORD()
    if not psapi.EnumProcessModulesEx(hproc, mods, ctypes.sizeof(mods), ctypes.byref(need), 0x01):
        return None
    return mods[0]


E_EM_ModeChangeLevel, E_EM_ModePlaying = 6, 9


class LevelPoke:
    """Once the engine plays (mode 9; the menu is a level too) for `settle` seconds, request the
    level change; report when szLevelName is the target and the mode is back to 9."""

    def __init__(self, hproc, map_path, level, preferred_base=0x400000, settle=2.0):
        syms = map_symbols(map_path)
        eng, save = syms.get("GAM_g_stEngineStructure_storage"), syms.get("GAM_pc_g_ucMode6SaveFlag_storage")
        if eng is None or save is None:
            sys.exit("--level: engine structure / save flag not in %s" % map_path)
        self.h, self.level, self.settle = hproc, level, settle
        self.cells = {"mode": eng, "name": eng + 0x01, "next": eng + 0x1F, "save": save}
        self.preferred = preferred_base
        self.delta = None
        self.t_play = self.t_poke = self.t_loaded = None

    def _va(self, k):
        if self.delta is None:
            base = image_base(self.h)
            self.delta = (base - self.preferred) if base else 0
        return self.cells[k] + self.delta

    def _read(self, k, n):
        b = (ctypes.c_ubyte * n)()
        ok = kernel32.ReadProcessMemory(self.h, ctypes.c_void_p(self._va(k)), b, n, None)
        return bytes(b) if ok else None

    def _write(self, k, data):
        kernel32.WriteProcessMemory(self.h, ctypes.c_void_p(self._va(k)), data, len(data), None)

    def step(self, now):
        if self.t_loaded is not None:
            return
        m, s = self._read("mode", 1), self._read("name", 30)
        mode, name = (m[0] if m else None), (s.split(b"\0")[0].decode("latin-1") if s else None)
        if self.t_poke is None:
            if mode != E_EM_ModePlaying:
                self.t_play = None
            elif self.t_play is None:
                self.t_play = now
                print("level: playing %r at %.1fs" % (name, now))
            elif now - self.t_play >= self.settle:
                self._write("next", self.level.encode("ascii") + b"\0")
                self._write("save", b"\0")
                self._write("mode", bytes([E_EM_ModeChangeLevel]))
                self.t_poke = now
                print("level: change to %s requested at %.1fs" % (self.level, now))
        elif mode == E_EM_ModePlaying and name == self.level:
            self.t_loaded = now
            print("level: %s loaded at %.1fs" % (self.level, now))


def main():
    ap = argparse.ArgumentParser(description=__doc__, formatter_class=argparse.RawDescriptionHelpFormatter)
    ap.add_argument("exe")
    ap.add_argument("--seconds", type=float, default=20)
    ap.add_argument("--level")
    ap.add_argument("--shot", default="", help="comma-separated seconds")
    ap.add_argument("--map", help="LINK map of EXE (default: EXE.map, else Rayman2.map beside it)")
    ap.add_argument("--mode", help="display mode WxH (default: the install's ubi.ini)")
    ap.add_argument("--keys", default="", help="comma-separated SECONDS:KEY (name or hex scancode)")
    ap.add_argument("--ini", action="append", default=[], metavar="KEY=VALUE",
                    help="set a key of the sandbox's ubi.ini (repeatable)")
    ap.add_argument("--keep", action="store_true", help="leave the game running at the end")
    a = ap.parse_args()
    map_path = a.map or os.path.splitext(os.path.abspath(a.exe))[0] + ".map"
    if not os.path.exists(map_path):
        map_path = os.path.join(os.path.dirname(os.path.abspath(a.exe)), "Rayman2.map")
    # a previous run that crashed or hung keeps the sandbox exe locked
    subprocess.run(["taskkill", "/F", "/IM", "Rayman2.exe"], capture_output=True)
    time.sleep(0.5)
    exe = build_sandbox(os.path.abspath(a.exe))
    if a.mode:
        set_mode(os.path.join(BOX, "ubi.ini"), a.mode)
    for kv in a.ini:
        set_ini(os.path.join(BOX, "ubi.ini"), *kv.split("=", 1))
    keys = sorted((float(t), k) for t, k in (e.split(":") for e in a.keys.split(",") if e))
    shots = sorted(float(s) for s in a.shot.split(",") if s)
    if shots:
        os.makedirs(os.path.join(BOX, "shots"), exist_ok=True)
    p = subprocess.Popen([exe], cwd=BOX)
    hproc = kernel32.OpenProcess(0x0010 | 0x0020 | 0x0008 | 0x0400, False, p.pid)  # VM_READ|WRITE|OP|QUERY
    poke = LevelPoke(hproc, map_path, a.level) if a.level else None
    t0 = time.time()
    first_window = None
    while time.time() - t0 < a.seconds and p.poll() is None:
        now = time.time() - t0
        if first_window is None and windows_of(p.pid):
            first_window = now
            print("window at %.1fs: %r" % (now, windows_of(p.pid)[0][1]))
        if poke:
            poke.step(now)
        while keys and now >= keys[0][0]:
            key = keys.pop(0)[1]
            press(key)
            print("key %s at %.1fs" % (key, now))
        while shots and now >= shots[0]:
            path = os.path.join(BOX, "shots", "%03d.png" % int(shots.pop(0)))
            print("shot %s: %s" % (path, "ok" if shoot(path) else "FAILED"))
        time.sleep(0.1)
    alive = p.poll() is None
    win = alive and bool(windows_of(p.pid))
    print("after %.0fs: %s" % (time.time() - t0, ("alive, window" if win else "alive, NO window") if alive
                                else "exited with 0x%08X" % (p.returncode & 0xFFFFFFFF)))
    ok = win and (poke is None or poke.t_loaded is not None)
    if poke and poke.t_loaded is None:
        print("level %s NOT loaded" % a.level)
    if alive and not a.keep:
        p.kill()
    return 0 if ok else 1


if __name__ == "__main__":
    sys.exit(main())
