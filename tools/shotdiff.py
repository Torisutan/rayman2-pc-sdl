#!/usr/bin/env python3
"""Compare two sets of screenshots of the same scenes (renderer comparison, docs/PLAN.md
"OpenGL driver", measuring item 2).

  python tools/shotdiff.py A B [--tolerance 24] [--out DIR]

A and B are two images or two folders; folders are compared file by file (same names, e.g.
the build/run/shots of a Direct3D 6 run and of an OpenGL run taken at the same seconds). For
each pair it prints the share of pixels whose largest channel difference exceeds the
tolerance (filtering, dithering and 16-bit against 32-bit colour stay below it) and the mean
absolute difference. --out writes a difference image per pair (differing pixels red over the
grey first image).

Exit code 0 = every pair compared (the numbers are a report, not a gate).
"""
import argparse
import os
import sys

from PIL import Image, ImageChops


def compare(a_path, b_path, tolerance, out_path=None):
    a = Image.open(a_path).convert("RGB")
    b = Image.open(b_path).convert("RGB")
    if b.size != a.size:
        b = b.resize(a.size, Image.BILINEAR)
    diff = ImageChops.difference(a, b)
    r, g, bl = diff.split()
    worst = ImageChops.lighter(ImageChops.lighter(r, g), bl)
    hist = worst.histogram()
    total = a.size[0] * a.size[1]
    over = sum(hist[tolerance + 1:])
    mean = sum(i * n for i, n in enumerate(hist)) / total
    if out_path:
        mask = worst.point(lambda v: 255 if v > tolerance else 0)
        base = a.convert("L").convert("RGB")
        red = Image.new("RGB", a.size, (255, 0, 0))
        Image.composite(red, base, mask).save(out_path)
    return over * 100.0 / total, mean


def main():
    ap = argparse.ArgumentParser(description=__doc__.split("\n")[0])
    ap.add_argument("a")
    ap.add_argument("b")
    ap.add_argument("--tolerance", type=int, default=24, help="largest channel difference still equal (0-255)")
    ap.add_argument("--out", help="folder for difference images")
    args = ap.parse_args()

    if os.path.isdir(args.a):
        names = sorted(n for n in os.listdir(args.a)
                       if n.lower().endswith((".png", ".bmp")) and os.path.exists(os.path.join(args.b, n)))
        pairs = [(os.path.join(args.a, n), os.path.join(args.b, n), n) for n in names]
    else:
        pairs = [(args.a, args.b, os.path.basename(args.a))]
    if not pairs:
        sys.exit("no image pairs")
    if args.out:
        os.makedirs(args.out, exist_ok=True)
    for a_path, b_path, name in pairs:
        out = os.path.join(args.out, os.path.splitext(name)[0] + "_diff.png") if args.out else None
        share, mean = compare(a_path, b_path, args.tolerance, out)
        print("%-24s %6.2f%% pixels differ (> %d), mean difference %.1f" % (name, share, args.tolerance, mean))
    return 0


if __name__ == "__main__":
    sys.exit(main())
