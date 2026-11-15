#!/usr/bin/env python3
"""Labelled C7 saw spectrum overlay for JunovaEliteProof WAVs."""
from __future__ import annotations

import json
import math
import struct
import sys
import wave
from pathlib import Path

import numpy as np

try:
    import matplotlib.pyplot as plt
except ImportError:
    import subprocess

    subprocess.check_call([sys.executable, "-m", "pip", "install", "-q", "matplotlib"])
    import matplotlib.pyplot as plt


def load_mono(path: Path) -> tuple[np.ndarray, int]:
    with wave.open(str(path), "rb") as wf:
        sr = wf.getframerate()
        n = wf.getnframes()
        raw = wf.readframes(n)
        if wf.getsampwidth() == 2:
            fmt = f"<{len(raw) // 2}h"
            data = np.array(struct.unpack(fmt, raw), dtype=np.float64) / 32768.0
        else:
            raise ValueError(f"unsupported width {wf.getsampwidth()}")
        if wf.getnchannels() > 1:
            data = data.reshape(-1, wf.getnchannels())[:, 0]
    return data, sr


def analyse_saw(path: Path, expected_hz: float) -> dict:
    x, sr = load_mono(path)
    n = 1 << int(math.floor(math.log2(min(len(x), int(sr * 2)))))
    seg = x[-n:]
    win = np.hanning(n)
    spec = np.fft.rfft(seg * win)
    mag = np.abs(spec) / n
    freqs = np.fft.rfftfreq(n, 1.0 / sr)
    mag_db = 20.0 * np.log10(np.maximum(mag, 1e-12))

    bin_hz = sr / n
    center = int(expected_hz / bin_hz)
    lo = max(1, center - 8)
    hi = min(len(mag_db) - 1, center + 8)
    fund_bin = lo + int(np.argmax(mag_db[lo : hi + 1]))
    fund_hz = float(freqs[fund_bin])
    fund_db = float(mag_db[fund_bin])

    worst_db = -200.0
    worst_hz = 0.0
    for k, f in enumerate(freqs):
        if k <= 1:
            continue
        is_harm = False
        for h in range(1, 16):
            if abs(f - h * fund_hz) < sr / n * 2.5:
                is_harm = True
                break
        if not is_harm and mag_db[k] > worst_db:
            worst_db = float(mag_db[k])
            worst_hz = float(f)

    peak = float(np.max(np.abs(x)))
    peak_db = 20.0 * math.log10(peak + 1e-12)
    return {
        "path": str(path),
        "sample_rate": sr,
        "fund_hz": fund_hz,
        "fund_db": fund_db,
        "worst_inharmonic_hz": worst_hz,
        "worst_inharmonic_db": worst_db,
        "worst_below_fund_db": fund_db - worst_db,
        "peak_dbfs": peak_db,
        "expected_hz": expected_hz,
        "freqs": freqs,
        "mag_db": mag_db,
    }


def plot_overlay(before: dict, after: dict, out_png: Path) -> None:
    fig, ax = plt.subplots(figsize=(12, 5), dpi=120)
    ax.plot(before["freqs"], before["mag_db"], color="#ff6b6b", alpha=0.75, label="before (naive saw)")
    ax.plot(after["freqs"], after["mag_db"], color="#4ecdc4", alpha=0.9, label="after (polyBLEP + FIR OS)")
    fund = after["fund_hz"]
    for h in range(1, 13):
        ax.axvline(fund * h, color="#888888", linestyle=":", linewidth=0.6, alpha=0.35)
    ax.set_xlim(0, min(12000, after["sample_rate"] / 2))
    ax.set_ylim(-100, max(before["mag_db"].max(), after["mag_db"].max()) + 6)
    ax.set_xlabel("Frequency (Hz)")
    ax.set_ylabel("Magnitude (dB)")
    worst = after["worst_below_fund_db"]
    ax.set_title(f"C7 saw @ 44.1 kHz — worst inharmonic {worst:.1f} dB below fundamental ({fund:.1f} Hz)")
    ax.grid(True, which="both", alpha=0.25)
    ax.legend(loc="upper right")
    fig.tight_layout()
    fig.savefig(out_png)
    plt.close(fig)


def main() -> int:
    out_dir = Path(sys.argv[1] if len(sys.argv) > 1 else "/opt/cursor/artifacts/junova-elite-audio")
    expected = float(sys.argv[2]) if len(sys.argv) > 2 else 2093.0
    # Proof renders at 88.2 kHz; analysis uses file-native sample rate.

    before_wav = out_dir / "c7-saw-before.wav"
    after_wav = out_dir / "c7-saw-elite.wav"
    if not before_wav.is_file() or not after_wav.is_file():
        print("Missing C7 WAV inputs", file=sys.stderr)
        return 1

    before = analyse_saw(before_wav, expected)
    after = analyse_saw(after_wav, expected)
    plot_overlay(before, after, out_dir / "c7-spectrum-overlay.png")

    metrics = {
        "before": {k: v for k, v in before.items() if k not in ("freqs", "mag_db")},
        "after": {k: v for k, v in after.items() if k not in ("freqs", "mag_db")},
    }
    (out_dir / "spectrum-metrics.json").write_text(json.dumps(metrics, indent=2))
    print(json.dumps({"worst_inharmonic_margin_db": after["worst_below_fund_db"]}, indent=2))
    return 0


if __name__ == "__main__":
    raise SystemExit(main())