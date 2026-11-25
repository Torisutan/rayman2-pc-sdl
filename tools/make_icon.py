"""Write the modern build's icon (Rayman2.exe/src/app/Rayman2.rc includes it).

The design is simple geometry, drawn here so no opaque binary sits in the tree: a rounded square
with a blue gradient and a gold ring. One PNG image per size (16, 32, 48, 256 px), packed into
an .ico. Standard library only.

    python tools/make_icon.py OUT.ico
"""
import math
import struct
import sys
import zlib

SIZES = (16, 32, 48, 256)
TOP, BOTTOM = (0x1E, 0x3A, 0x8A), (0x0E, 0x74, 0x90)
GOLD = (0xF5, 0xC5, 0x42)


def cover(d):
    """Coverage of a pixel whose centre lies d pixels inside an edge (box filter, 1 px wide)."""
    return min(1.0, max(0.0, d + 0.5))


def pixel(x, y, n):
    """RGBA at pixel (x, y) of an n x n image."""
    u, v = (x + 0.5) / n - 0.5, (y + 0.5) / n - 0.5        # centre at 0, edges at +-0.5
    # rounded square: half side 0.47, corner radius 0.20
    h, r = 0.47, 0.20
    qx, qy = abs(u) - (h - r), abs(v) - (h - r)
    out = math.hypot(max(qx, 0.0), max(qy, 0.0)) + min(max(qx, qy), 0.0) - r
    a = cover(-out * n)
    t = (v + 0.5)
    rgb = [TOP[i] + (BOTTOM[i] - TOP[i]) * t for i in range(3)]
    # gold ring: radii 0.19 .. 0.30
    d = math.hypot(u, v)
    ring = min(cover((d - 0.19) * n), cover((0.30 - d) * n))
    rgb = [rgb[i] + (GOLD[i] - rgb[i]) * ring for i in range(3)]
    return bytes([int(c + 0.5) for c in rgb] + [int(a * 255 + 0.5)])


def png(n):
    raw = b"".join(b"\0" + b"".join(pixel(x, y, n) for x in range(n)) for y in range(n))

    def chunk(tag, data):
        return struct.pack(">I", len(data)) + tag + data + struct.pack(">I", zlib.crc32(tag + data))
    return (b"\x89PNG\r\n\x1a\n" + chunk(b"IHDR", struct.pack(">IIBBBBB", n, n, 8, 6, 0, 0, 0))
            + chunk(b"IDAT", zlib.compress(raw, 9)) + chunk(b"IEND", b""))


def ico():
    images = [png(n) for n in SIZES]
    out = struct.pack("<HHH", 0, 1, len(images))
    offset = 6 + 16 * len(images)
    for n, img in zip(SIZES, images):
        out += struct.pack("<BBBBHHII", n % 256, n % 256, 0, 0, 1, 32, len(img), offset)
        offset += len(img)
    return out + b"".join(images)


if __name__ == "__main__":
    with open(sys.argv[1], "wb") as f:
        f.write(ico())
