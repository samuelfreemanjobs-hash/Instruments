#!/usr/bin/env python3
"""Plot aliasing before/after at true C8 with harmonic markers and report."""
from __future__ import annotations

import csv
import sys
from pathlib import Path

import matplotlib

matplotlib.use("Agg")
import matplotlib.pyplot as plt
import numpy as np

ARTIFACTS = Path("/opt/cursor/artifacts")
FUND_HZ = 440.0 * (2.0 ** ((108 - 69) / 12.0))  # MIDI 108 = C8


def load_csv(path: Path) -> tuple[np.ndarray, np.ndarray]:
    xs: list[float] = []
    ys: list[float] = []
    with path.open() as f:
        for row in csv.reader(f):
            if len(row) < 2:
                continue
            xs.append(float(row[0]))
            ys.append(float(row[1]))
    return np.array(xs), np.array(ys)


def harmonic_lines(xmax: float) -> list[float]:
    hs: list[float] = []
    n = 1
    while n * FUND_HZ <= xmax:
        hs.append(n * FUND_HZ)
        n += 1
    return hs


def main() -> int:
    before = ARTIFACTS / "wave9090-aliasing-before.csv"
    after = ARTIFACTS / "wave9090-aliasing-after.csv"
    report = ARTIFACTS / "wave9090-aliasing-report.txt"
    if not before.exists() or not after.exists():
        print("missing CSV inputs", file=sys.stderr)
        return 1

    bx, by = load_csv(before)
    ax, ay = load_csv(after)
    plt.figure(figsize=(11, 5))
    plt.plot(bx, by, label="Before (legacy full-band morph)", alpha=0.85, linewidth=0.9)
    plt.plot(ax, ay, label="After (FFT per-octave mips + crossfade)", alpha=0.85, linewidth=0.9)

    for h in harmonic_lines(22050):
        plt.axvline(h, color="#444", alpha=0.18, linewidth=0.8)

    plt.axvline(FUND_HZ, color="#0cf", alpha=0.45, linewidth=1.2, label=f"C8 fundamental ({FUND_HZ:.1f} Hz)")
    plt.xlim(0, 12000)
    plt.ylim(-130, 0)
    plt.xlabel("Frequency (Hz)")
    plt.ylabel("Magnitude (dBFS)")
    plt.title("WAVE-9090 oscillator @ 44.1 kHz — C8 (MIDI 108) wavetable morph 0.62")
    plt.legend(loc="upper right")
    plt.grid(True, alpha=0.25)
    out = ARTIFACTS / "wave9090-aliasing-spectrum.png"
    plt.tight_layout()
    plt.savefig(out, dpi=150)
    print(f"Wrote {out}")

    if report.exists():
        print("---")
        print(report.read_text())

    return 0


if __name__ == "__main__":
    raise SystemExit(main())