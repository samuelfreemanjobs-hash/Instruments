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


def blackman_harris(n: int) -> np.ndarray:
    a0, a1, a2, a3 = 0.35875, 0.48829, 0.14128, 0.01168
    i = np.arange(n, dtype=np.float64)
    return (
        a0
        - a1 * np.cos(2.0 * math.pi * i / (n - 1))
        + a2 * np.cos(4.0 * math.pi * i / (n - 1))
        - a3 * np.cos(6.0 * math.pi * i / (n - 1))
    )


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


def trim_integer_cycles(x: np.ndarray, sr: int, expected_hz: float) -> np.ndarray:
    if expected_hz <= 0:
        return x
    period = sr / expected_hz
    n_periods = int(len(x) / period)
    if n_periods < 8:
        return x
    n = int(round(n_periods * period))
    n = min(n, len(x))
    return x[-n:]


def analyse_saw(path: Path, expected_hz: float) -> dict:
    x, sr = load_mono(path)
    x = trim_integer_cycles(x, sr, expected_hz)
    n_trim = len(x)
    start = int(n_trim * 0.12)
    end = int(n_trim * 0.98)
    x = x[start:end]
    n_fft = 1 << int(math.floor(math.log2(min(len(x), int(sr * 4)))))
    seg = x[-n_fft:]
    win = blackman_harris(n_fft)
    spec = np.fft.rfft(seg * win)
    mag = np.abs(spec) / (np.sum(win) * 0.5)
    freqs = np.fft.rfftfreq(n_fft, 1.0 / sr)
    mag_db = 20.0 * np.log10(np.maximum(mag, 1e-12))

    bin_hz = sr / n_fft

    def nearest_bin(hz: float) -> int:
        return int(round(hz / bin_hz))

    fund_bin = nearest_bin(expected_hz)
    fund_bin = max(2, min(len(mag_db) - 2, fund_bin))
    fund_hz = float(freqs[fund_bin])
    fund_db = float(mag_db[fund_bin])

    max_harm = int((sr * 0.48) / fund_hz)
    harmonic_bins: set[int] = set()
    for h in range(1, max_harm + 1):
        hb = nearest_bin(fund_hz * h)
        for w in range(-4, 5):
            if 0 <= hb + w < len(mag_db):
                harmonic_bins.add(hb + w)

    worst_db = -200.0
    worst_hz = 0.0
    min_spur_hz = fund_hz * 0.85
    max_spur_hz = sr * 0.46
    for k, f in enumerate(freqs):
        if k in harmonic_bins or f < min_spur_hz or f > max_spur_hz:
            continue
        if mag_db[k] > worst_db:
            worst_db = float(mag_db[k])
            worst_hz = float(f)

    rel_margin = fund_db - worst_db
    peak = float(np.max(np.abs(x)))
    peak_db = 20.0 * math.log10(peak + 1e-12)
    return {
        "path": str(path),
        "sample_rate": sr,
        "fft_size": n_fft,
        "fund_hz": fund_hz,
        "fund_db": fund_db,
        "worst_inharmonic_hz": worst_hz,
        "worst_inharmonic_db": worst_db,
        "worst_below_fund_db": rel_margin,
        "peak_dbfs": peak_db,
        "expected_hz": expected_hz,
        "freqs": freqs,
        "mag_db": mag_db - fund_db,
    }


def plot_overlay(before: dict, after: dict, out_png: Path) -> None:
    fig, ax = plt.subplots(figsize=(12, 5), dpi=120)
    ax.plot(before["freqs"], before["mag_db"], color="#ff6b6b", alpha=0.75, label="before (naive saw)")
    ax.plot(after["freqs"], after["mag_db"], color="#4ecdc4", alpha=0.9, label="after (polyBLEP saw + mip PWM/sub)")
    fund = after["fund_hz"]
    for h in range(1, 13):
        ax.axvline(fund * h, color="#888888", linestyle=":", linewidth=0.6, alpha=0.35)
    ax.set_xlim(0, min(12000, after["sample_rate"] / 2))
    ax.set_ylim(-120, 6)
    ax.set_xlabel("Frequency (Hz)")
    ax.set_ylabel("Level vs fundamental (dB)")
    worst = after["worst_below_fund_db"]
    ax.set_title(
        f"C7 dry saw @ {after['sample_rate']} Hz — inharmonic margin {worst:.1f} dB (fund {fund:.1f} Hz, FFT {after['fft_size']})"
    )
    ax.grid(True, which="both", alpha=0.25)
    ax.legend(loc="upper right")
    fig.tight_layout()
    fig.savefig(out_png)
    plt.close(fig)


def main() -> int:
    out_dir = Path(sys.argv[1] if len(sys.argv) > 1 else "/opt/cursor/artifacts/junova-elite-audio")
    expected = float(sys.argv[2]) if len(sys.argv) > 2 else 2093.0

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