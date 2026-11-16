#!/usr/bin/env python3
"""Draw a 128-frame rotary filmstrip. Uses Pillow when it is installed, else PPM."""

from __future__ import annotations

import argparse
import math
from pathlib import Path


def frame_pixels(size: int, frame: int, frames: int) -> list[tuple[int, int, int]]:
    angle = -math.pi * 0.75 + (1.5 * math.pi) * (frame / max(1, frames - 1))
    pixels: list[tuple[int, int, int]] = []
    center = (size - 1) / 2.0
    radius = size * 0.38
    for y in range(size):
        for x in range(size):
            dx = x - center
            dy = y - center
            dist = math.hypot(dx, dy)
            if dist > radius:
                pixels.append((26, 28, 30))
                continue
            shade = int(180 + 40 * (1.0 - dist / radius))
            pointer = math.atan2(dy, dx)
            delta = abs((pointer - angle + math.pi) % (2 * math.pi) - math.pi)
            if delta < 0.08 and dist > radius * 0.45:
                pixels.append((210, 161, 90))
            else:
                pixels.append((shade, shade - 8, shade - 16))
    return pixels


def write_ppm(path: Path, width: int, height: int, rgb: list[tuple[int, int, int]]) -> None:
    body = bytearray()
    for red, green, blue in rgb:
        body.extend((red & 255, green & 255, blue & 255))
    path.write_bytes(f"P6\n{width} {height}\n255\n".encode("ascii") + body)


def main() -> int:
    parser = argparse.ArgumentParser(description="Render a knob filmstrip.")
    parser.add_argument("--out", type=Path, required=True)
    parser.add_argument("--frames", type=int, default=128)
    parser.add_argument("--size", type=int, default=64)
    args = parser.parse_args()
    rows: list[tuple[int, int, int]] = []
    for frame in range(args.frames):
        rows.extend(frame_pixels(args.size, frame, args.frames))
    args.out.parent.mkdir(parents=True, exist_ok=True)
    png = args.out if args.out.suffix.lower() == ".png" else args.out.with_suffix(".png")
    try:
        from PIL import Image
    except ImportError:
        ppm = png.with_suffix(".ppm")
        write_ppm(ppm, args.size, args.size * args.frames, rows)
        print(f"filmstrip={ppm} frames={args.frames} format=ppm")
        return 0
    image = Image.new("RGB", (args.size, args.size * args.frames))
    image.putdata(rows)
    image.save(png)
    print(f"filmstrip={png} frames={args.frames} format=png")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
