#!/usr/bin/env python3
"""Lightweight WAV metrics for factory-audio profile (optional numpy)."""

from __future__ import annotations

import argparse
import json
import struct
import sys
import wave
from pathlib import Path


def analyze_wav(path: Path) -> dict:
    with wave.open(str(path), "rb") as w:
        nch = w.getnchannels()
        rate = w.getframerate()
        nframes = w.getnframes()
        sampwidth = w.getsampwidth()
        raw = w.readframes(nframes)
    if sampwidth != 2:
        return {"ok": False, "error": f"unsupported sampwidth {sampwidth}"}
    count = len(raw) // 2
    peaks: list[float] = []
    sumsq = 0.0
    for i in range(0, len(raw), 2):
        sample = struct.unpack("<h", raw[i : i + 2])[0] / 32768.0
        peaks.append(abs(sample))
        sumsq += sample * sample
    peak = max(peaks) if peaks else 0.0
    rms = (sumsq / max(count, 1)) ** 0.5
    clipped = sum(1 for p in peaks if p > 0.99)
    silent = peak < 1e-6
    return {
        "ok": True,
        "path": str(path),
        "channels": nch,
        "sample_rate": rate,
        "frames": nframes,
        "peak": peak,
        "rms": rms,
        "clip_samples": clipped,
        "silent": silent,
    }


def main(argv: list[str] | None = None) -> int:
    p = argparse.ArgumentParser()
    p.add_argument("wav", type=Path)
    args = p.parse_args(argv)
    if not args.wav.is_file():
        print(json.dumps({"ok": False, "error": "missing file"}))
        return 1
    result = analyze_wav(args.wav)
    print(json.dumps(result, indent=2))
    if not result.get("ok"):
        return 1
    if result.get("silent") and "meter" not in args.wav.name.lower():
        return 2
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
