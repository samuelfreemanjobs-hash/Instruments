#!/usr/bin/env python3
"""Generate v0 stub drum one-shots (44.1 kHz mono WAV) for SaaS preview packs."""

from __future__ import annotations

import math
import struct
import wave
from pathlib import Path

SAMPLE_RATE = 44100
ROOT = Path(__file__).resolve().parents[2] / "website" / "public" / "samples"

NAMES = ("kick", "snare", "hat_closed", "hat_open", "rim", "clap")


def write_wav(path: Path, samples: list[float]) -> None:
    path.parent.mkdir(parents=True, exist_ok=True)
    peak = max(abs(s) for s in samples) or 1.0
    scale = 0.89 / peak
    with wave.open(str(path), "w") as w:
        w.setnchannels(1)
        w.setsampwidth(2)
        w.setframerate(SAMPLE_RATE)
        frames = b"".join(
            struct.pack("<h", int(max(-1.0, min(1.0, s * scale)) * 32767))
            for s in samples
        )
        w.writeframes(frames)


def sine(freq: float, t: float) -> float:
    return math.sin(2 * math.pi * freq * t)


def noise(t: float, seed: int) -> float:
    return math.sin(t * 9973.0 + seed * 0.17) * math.sin(t * 12347.0 + seed)


def kick(pitch: float, decay: float) -> list[float]:
    n = int(SAMPLE_RATE * 0.45)
    out: list[float] = []
    for i in range(n):
        t = i / SAMPLE_RATE
        env = math.exp(-t / decay)
        f = pitch * (1.0 + 4.0 * math.exp(-t * 30))
        out.append(sine(f, t) * env)
    return out


def snare(body: float, snap: float) -> list[float]:
    n = int(SAMPLE_RATE * 0.25)
    out: list[float] = []
    for i in range(n):
        t = i / SAMPLE_RATE
        env = math.exp(-t / 0.12)
        tone = sine(180, t) * body
        nse = noise(t, 42) * snap * math.exp(-t / 0.04)
        out.append((tone + nse) * env)
    return out


def hat(decay: float, bright: float) -> list[float]:
    n = int(SAMPLE_RATE * 0.08)
    out: list[float] = []
    for i in range(n):
        t = i / SAMPLE_RATE
        env = math.exp(-t / decay)
        out.append(noise(t, 7) * bright * env + sine(8000, t) * 0.05 * env)
    return out


def rim(metal: float) -> list[float]:
    n = int(SAMPLE_RATE * 0.06)
    out: list[float] = []
    for i in range(n):
        t = i / SAMPLE_RATE
        env = math.exp(-t / 0.02)
        out.append((sine(420, t) + sine(840, t) * 0.5) * metal * env)
    return out


def clap(wide: float) -> list[float]:
    n = int(SAMPLE_RATE * 0.18)
    out: list[float] = []
    for i in range(n):
        t = i / SAMPLE_RATE
        env = math.exp(-t / 0.09)
        burst = 0.0
        for off in (0.0, 0.012, 0.024):
            if t >= off:
                burst += noise(t - off, 99) * math.exp(-(t - off) / 0.02)
        out.append(burst * wide * env)
    return out


PRESETS: dict[str, dict[str, object]] = {
    "boulevard-86": {
        "kick": lambda: kick(52, 0.22),
        "snare": lambda: snare(0.35, 0.25),
        "hat_closed": lambda: hat(0.025, 0.35),
        "hat_open": lambda: hat(0.06, 0.4),
        "rim": lambda: rim(0.45),
        "clap": lambda: clap(0.35),
    },
    "midnight-circuit": {
        "kick": lambda: kick(38, 0.35),
        "snare": lambda: snare(0.2, 0.55),
        "hat_closed": lambda: hat(0.018, 0.55),
        "hat_open": lambda: hat(0.04, 0.6),
        "rim": lambda: rim(0.65),
        "clap": lambda: clap(0.5),
    },
    "disklordz-screw": {
        "kick": lambda: kick(34, 0.55),
        "snare": lambda: snare(0.55, 0.15),
        "hat_closed": lambda: hat(0.04, 0.2),
        "hat_open": lambda: hat(0.09, 0.22),
        "rim": lambda: rim(0.3),
        "clap": lambda: clap(0.25),
    },
    "terminal-mirage": {
        "kick": lambda: kick(58, 0.18),
        "snare": lambda: snare(0.3, 0.35),
        "hat_closed": lambda: hat(0.015, 0.45),
        "hat_open": lambda: hat(0.035, 0.5),
        "rim": lambda: rim(0.55),
        "clap": lambda: clap(0.4),
    },
    "mpc-ready-808": {
        "kick": lambda: kick(45, 0.28),
        "snare": lambda: snare(0.25, 0.4),
        "hat_closed": lambda: hat(0.022, 0.45),
        "hat_open": lambda: hat(0.05, 0.48),
        "rim": lambda: rim(0.5),
        "clap": lambda: clap(0.42),
    },
}


def main() -> None:
    for preset_id, generators in PRESETS.items():
        for name in NAMES:
            gen = generators[name]
            assert callable(gen)
            write_wav(ROOT / preset_id / f"{name}.wav", gen())  # type: ignore[operator]
    print(f"Wrote stub kits under {ROOT}")


if __name__ == "__main__":
    main()
