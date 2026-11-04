#!/usr/bin/env python3
"""Offline FX: saturation, bitcrush, filter, tape wobble — prep for MPC resampling."""

from __future__ import annotations

import argparse
import json
import math
import random
import struct
import sys
import wave
from pathlib import Path

SAMPLE_RATE = 44100

PRESETS: dict[str, dict] = {
    "phonk_crush": {"bits": 8, "downsample": 3, "drive": 1.8, "lp_hz": 3200, "wow_depth": 0.0},
    "phonk_tape": {"bits": 12, "downsample": 1, "drive": 1.35, "lp_hz": 4800, "wow_depth": 0.004},
    "phonk_filter": {"bits": 16, "downsample": 1, "drive": 1.1, "lp_hz": 800, "wow_depth": 0.0},
    "phonk_heavy": {"bits": 6, "downsample": 4, "drive": 2.4, "lp_hz": 2600, "wow_depth": 0.006},
}


def read_wav(path: Path) -> tuple[list[float], int]:
    with wave.open(str(path), "r") as wf:
        sr = wf.getframerate()
        n = wf.getnframes()
        raw = wf.readframes(n)
        width = wf.getsampwidth()
        if width != 2:
            raise SystemExit("Only 16-bit PCM WAV supported")
        ints = struct.unpack(f"<{n}h", raw)
        ch = wf.getnchannels()
        if ch == 2:
            ints = [(ints[i] + ints[i + 1]) // 2 for i in range(0, len(ints), 2)]
        samples = [x / 32768.0 for x in ints]
    return samples, sr


def write_wav(path: Path, samples: list[float], sr: int) -> None:
    path.parent.mkdir(parents=True, exist_ok=True)
    ints = [int(max(-1.0, min(1.0, s)) * 32767) for s in samples]
    with wave.open(str(path), "w") as wf:
        wf.setnchannels(1)
        wf.setsampwidth(2)
        wf.setframerate(sr)
        wf.writeframes(struct.pack(f"<{len(ints)}h", *ints))


def soft_clip(x: float, drive: float) -> float:
    return math.tanh(x * drive) / math.tanh(drive)


def bitcrush(x: float, bits: int) -> float:
    levels = 2**bits
    return round(x * levels) / levels


def onepole_lp(samples: list[float], cutoff_hz: float, sr: int) -> list[float]:
    if cutoff_hz <= 0:
        return samples
    rc = 1.0 / (2 * math.pi * cutoff_hz)
    dt = 1.0 / sr
    alpha = dt / (rc + dt)
    y = 0.0
    out: list[float] = []
    for s in samples:
        y = y + alpha * (s - y)
        out.append(y)
    return out


def apply_wow(samples: list[float], depth: float, sr: int) -> list[float]:
    if depth <= 0:
        return samples
    out: list[float] = []
    phase = 0.0
    for i, s in enumerate(samples):
        phase += 0.35 / sr
        mod = 1.0 + depth * math.sin(2 * math.pi * 0.6 * i / sr)
        idx = int(i + mod * depth * sr * 0.02)
        idx = max(0, min(len(samples) - 1, idx))
        out.append(samples[idx] * 0.92 + s * 0.08)
    return out


def process(samples: list[float], sr: int, cfg: dict) -> list[float]:
    ds = int(cfg.get("downsample", 1))
    if ds > 1:
        samples = samples[::ds]
        sr = max(4000, sr // ds)
    out: list[float] = []
    for s in samples:
        s = soft_clip(s, cfg.get("drive", 1.2))
        s = bitcrush(s, int(cfg.get("bits", 12)))
        out.append(s)
    out = onepole_lp(out, float(cfg.get("lp_hz", 4000)), sr)
    out = apply_wow(out, float(cfg.get("wow_depth", 0)), sr)
    peak = max(abs(x) for x in out) or 1.0
    return [x / peak * 0.9 for x in out]


def main() -> int:
    parser = argparse.ArgumentParser(description="Offline phonk FX chain")
    parser.add_argument("--in", dest="inp", required=True, type=Path)
    parser.add_argument("--out", required=True, type=Path)
    parser.add_argument("--preset", choices=list(PRESETS.keys()), default="phonk_crush")
    args = parser.parse_args()

    cfg = PRESETS[args.preset]
    samples, sr = read_wav(args.inp.expanduser().resolve())
    out = process(samples, sr, cfg)
    dest = args.out.expanduser().resolve()
    write_wav(dest, out, sr)
    print(json.dumps({"preset": args.preset, "out": str(dest), "samples": len(out)}, indent=2))
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
