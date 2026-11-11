#!/usr/bin/env python3
"""Launch TRAP LORD SYNTH Standalone under Xvfb and save full-editor PNGs."""

from __future__ import annotations

import struct
import subprocess
import sys
import zlib
from pathlib import Path

ROOT = Path(__file__).resolve().parents[1]
BUILD = ROOT / "build"
STANDALONE = BUILD / "TrapLordSynth_artefacts" / "Release" / "Standalone" / "TRAP LORD SYNTH"
ARTIFACTS = Path("/opt/cursor/artifacts")
SHOTS = (
    ("Pads", ARTIFACTS / "trap-lord-editor-pad-preset.png"),
    ("Swells", ARTIFACTS / "trap-lord-editor-swell-preset.png"),
)


def build_standalone() -> None:
    subprocess.check_call(
        ["cmake", "--build", str(BUILD), "-j", "--target", "TrapLordSynth_Standalone"],
        cwd=ROOT,
    )
    if not STANDALONE.exists():
        raise SystemExit(f"Standalone missing: {STANDALONE}")


def png_valid(path: Path) -> bool:
    data = path.read_bytes()
    if data[:8] != b"\x89PNG\r\n\x1a\n":
        return False
    return data.endswith(b"IEND\xaeB`\x82")


def png_non_blank(path: Path) -> bool:
    data = path.read_bytes()
    if data[:8] != b"\x89PNG\r\n\x1a\n":
        return False
    offset = 8
    width = height = 0
    raw = b""
    while offset < len(data):
        if offset + 8 > len(data):
            break
        length = struct.unpack(">I", data[offset : offset + 4])[0]
        ctype = data[offset + 4 : offset + 8]
        offset += 8
        chunk = data[offset : offset + length]
        offset += length + 4
        if ctype == b"IHDR" and len(chunk) >= 8:
            width, height = struct.unpack(">II", chunk[:8])
        elif ctype == b"IDAT":
            raw += chunk
        elif ctype == b"IEND":
            break
    if width < 8 or height < 8 or not raw:
        return False
    pixels = zlib.decompress(raw)
    stride = width * 4 + 1
    if len(pixels) < stride * height:
        return False
    first = None
    for y in range(0, height, max(1, height // 12)):
        row = pixels[y * stride + 1 : y * stride + 1 + width * 4]
        for x in range(0, len(row), 16):
            px = row[x : x + 3]
            if first is None:
                first = px
            elif px != first:
                return True
    return first is not None and any(b > 8 for b in first)


def capture(category: str, out: Path) -> None:
    out.parent.mkdir(parents=True, exist_ok=True)
    env = {
        **dict(__import__("os").environ),
        "TRAP_LORD_SCREENSHOT_CATEGORY": category,
        "TRAP_LORD_SCREENSHOT_PATH": str(out),
    }
    cmd = [
        "xvfb-run",
        "-a",
        "-s",
        "-screen 0 1280x800x24 +extension RANDR",
        str(STANDALONE),
    ]
    print("+", " ".join(cmd), f"(category={category})")
    subprocess.check_call(cmd, env=env, cwd=STANDALONE.parent, timeout=120)
    if not out.exists():
        raise SystemExit(f"Screenshot not written: {out}")
    if not png_valid(out):
        raise SystemExit(f"PNG not well-formed (bad IEND): {out}")
    if not png_non_blank(out):
        raise SystemExit(f"Screenshot appears blank: {out}")
    print(f"OK {out} ({out.stat().st_size} bytes)")


def main() -> int:
    ARTIFACTS.mkdir(parents=True, exist_ok=True)
    if not BUILD.exists():
        subprocess.check_call(
            [
                "cmake",
                "-B",
                str(BUILD),
                "-DCMAKE_BUILD_TYPE=Release",
                "-DCMAKE_CXX_COMPILER=g++-12",
                "-DCMAKE_C_COMPILER=gcc-12",
            ],
            cwd=ROOT,
        )
    build_standalone()
    for cat, path in SHOTS:
        capture(cat, path)
    return 0


if __name__ == "__main__":
    sys.exit(main())
