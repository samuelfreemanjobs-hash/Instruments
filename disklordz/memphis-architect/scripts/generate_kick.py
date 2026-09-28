#!/usr/bin/env python3
"""Minimal Memphis phonk kick twin — procedural envelope (CI parity with browser kick layers)."""

from __future__ import annotations

import math
import struct
import wave
from pathlib import Path

SR = 44100


def adsr(t: float, a: float, d: float, s: float, r: float, gate: float) -> float:
    if t < a:
        return t / a if a > 0 else 1.0
    if t < a + d:
        return 1 - (1 - s) * ((t - a) / d)
    if t < gate:
        return s
    if t < gate + r:
        return s * (1 - (t - gate) / r) if r > 0 else 0.0
    return 0.0


def render_kick(duration: float = 0.28, root_hz: float = 50.0, pitch_mod: float = 36.0) -> list[float]:
    n = int(SR * (duration + 0.05))
    gate = duration
    phase = 0.0
    out: list[float] = []
    for i in range(n):
        t = i / SR
        ae = adsr(t, 0.001, 0.12, 0.0, 0.05, gate)
        beater = (
            math.exp(-t / 0.00075) * math.sin(2 * math.pi * 3200 * t) * ae if t < 0.006 else 0.0
        )
        pitch_env = math.exp(-t / 0.022)
        f0 = root_hz * (2 ** ((pitch_mod * pitch_env) / 12))
        phase += (2 * math.pi * f0) / SR
        body = math.sin(phase) * ae * 0.82
        sub = math.sin(phase * 0.5) * ae * 0.45
        out.append(beater + body + sub)
    return out


def write_wav(path: Path, samples: list[float]) -> None:
    peak = max(1e-9, max(abs(s) for s in samples))
    norm = 0.95 / peak
    path.parent.mkdir(parents=True, exist_ok=True)
    with wave.open(str(path), "wb") as wf:
        wf.setnchannels(1)
        wf.setsampwidth(2)
        wf.setframerate(SR)
        frames = bytearray()
        for s in samples:
            v = int(max(-1, min(1, s * norm)) * 32767)
            frames += struct.pack("<h", v)
        wf.writeframes(frames)


def main() -> None:
    out = Path(__file__).resolve().parent.parent / "artifacts" / "memphis_kick_twin.wav"
    write_wav(out, render_kick())
    print(f"wrote {out}")


if __name__ == "__main__":
    main()
