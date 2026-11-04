#!/usr/bin/env python3
"""Render 16 scale-locked chord stabs as mono WAVs for MPC500 MELODIC/CHORDS."""

from __future__ import annotations

import argparse
import json
import math
import struct
import sys
import wave
from pathlib import Path

sys.path.insert(0, str(Path(__file__).resolve().parent))
from _scale import CHORDS_PATH, load_json  # noqa: E402

SAMPLE_RATE = 44100
DURATION_S = 0.45


def synth_chord(notes: list[int], velocity: float = 0.35) -> list[float]:
    n_samples = int(SAMPLE_RATE * DURATION_S)
    buf = [0.0] * n_samples
    freqs = [440.0 * (2.0 ** ((n - 69) / 12.0)) for n in notes]
    for freq in freqs:
        for i in range(n_samples):
            t = i / SAMPLE_RATE
            env = math.exp(-4.0 * t / DURATION_S)
            buf[i] += math.sin(2 * math.pi * freq * t) * env * velocity
    peak = max(abs(x) for x in buf) or 1.0
    return [x / peak * 0.89 for x in buf]


def write_wav(path: Path, samples: list[float]) -> None:
    path.parent.mkdir(parents=True, exist_ok=True)
    ints = [int(max(-1.0, min(1.0, s)) * 32767) for s in samples]
    with wave.open(str(path), "w") as wf:
        wf.setnchannels(1)
        wf.setsampwidth(2)
        wf.setframerate(SAMPLE_RATE)
        wf.writeframes(struct.pack(f"<{len(ints)}h", *ints))


def main() -> int:
    parser = argparse.ArgumentParser(description="Render CH01..CH16 chord WAVs")
    parser.add_argument("--out", required=True, type=Path, help="Output dir (MELODIC/CHORDS)")
    args = parser.parse_args()

    data = load_json(CHORDS_PATH)
    out = args.out.expanduser().resolve()
    written: list[str] = []

    for t in data["triggers"]:
        idx = t["index"]
        name = f"CH{idx:02d}.WAV"
        samples = synth_chord(t["notes"])
        write_wav(out / name, samples)
        written.append(name)

    print(json.dumps({"written": written, "out": str(out)}, indent=2))
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
