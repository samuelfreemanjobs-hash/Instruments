#!/usr/bin/env python3
"""Phase-2 stub: ambient / cloud-rap one-shots (separate from phonk Memphis engine).

Not wired into phonk loops by default — run standalone when you want chill textures.
"""

from __future__ import annotations

import argparse
import json
import math
import struct
import wave
from pathlib import Path

SR = 44100


def _write(path: Path, samples: list[float]) -> None:
    peak = max(abs(s) for s in samples) or 1.0
    path.parent.mkdir(parents=True, exist_ok=True)
    with wave.open(str(path), "w") as w:
        w.setnchannels(1)
        w.setsampwidth(2)
        w.setframerate(SR)
        w.writeframes(b"".join(struct.pack("<h", int(s / peak * 0.85 * 32767)) for s in samples))


def soft_kick() -> list[float]:
    n = int(SR * 0.5)
    out: list[float] = []
    for i in range(n):
        t = i / SR
        env = math.exp(-t / 0.35)
        f = 90 * (1 + 2 * math.exp(-t * 20))
        out.append(math.sin(2 * math.pi * f * t) * env * 0.9)
    return out


def vinyl_noise() -> list[float]:
    n = int(SR * 2.0)
    return [((i * 7919) % 997) / 997 * 0.08 - 0.04 for i in range(n)]


def pad_stab() -> list[float]:
    n = int(SR * 1.2)
    out: list[float] = []
    for i in range(n):
        t = i / SR
        env = math.exp(-t / 0.5) * (1 - math.exp(-t / 0.05))
        s = math.sin(2 * math.pi * 220 * t) + math.sin(2 * math.pi * 330 * t) * 0.4
        out.append(s * env * 0.35)
    return out


def main() -> None:
    p = argparse.ArgumentParser(description="Cloud/ambient one-shot stub (phase 2)")
    p.add_argument("--out", type=Path, required=True)
    args = p.parse_args()
    files = {
        "soft_kick.wav": soft_kick(),
        "vinyl_layer.wav": vinyl_noise(),
        "pad_stab.wav": pad_stab(),
    }
    for name, pcm in files.items():
        _write(args.out / name, pcm)
    (args.out / "manifest.json").write_text(
        json.dumps({"format": "CLOUD_ONE_SHOT_STUB", "files": list(files.keys())}, indent=2)
    )
    print(f"Wrote cloud stub pack to {args.out}")


if __name__ == "__main__":
    main()
