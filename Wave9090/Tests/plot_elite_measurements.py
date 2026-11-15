#!/usr/bin/env python3
"""Plot aliasing before/after CSV from Wave9090EliteMeasurements."""
from __future__ import annotations

import csv
import sys
from pathlib import Path

import matplotlib

matplotlib.use("Agg")
import matplotlib.pyplot as plt

ARTIFACTS = Path("/opt/cursor/artifacts")


def load_csv(path: Path) -> tuple[list[float], list[float]]:
    xs: list[float] = []
    ys: list[float] = []
    with path.open() as f:
        for row in csv.reader(f):
            if len(row) < 2:
                continue
            xs.append(float(row[0]))
            ys.append(float(row[1]))
    return xs, ys


def main() -> int:
    before = ARTIFACTS / "wave9090-aliasing-before.csv"
    after = ARTIFACTS / "wave9090-aliasing-after.csv"
    if not before.exists() or not after.exists():
        print("missing CSV inputs", file=sys.stderr)
        return 1

    bx, by = load_csv(before)
    ax, ay = load_csv(after)
    plt.figure(figsize=(10, 4.5))
    plt.plot(bx, by, label="Legacy mip rule (before)", alpha=0.85)
    plt.plot(ax, ay, label="Elite mip rule (after)", alpha=0.85)
    plt.xlim(0, 22050)
    plt.ylim(-120, 0)
    plt.xlabel("Frequency (Hz)")
    plt.ylabel("Magnitude (dB)")
    plt.title("WAVE-9090 C8 morph spectrum — aliasing comparison @ 44.1 kHz")
    plt.legend(loc="upper right")
    plt.grid(True, alpha=0.25)
    out = ARTIFACTS / "wave9090-aliasing-spectrum.png"
    plt.tight_layout()
    plt.savefig(out, dpi=150)
    print(f"Wrote {out}")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())