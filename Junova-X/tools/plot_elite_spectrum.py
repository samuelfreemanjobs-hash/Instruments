#!/usr/bin/env python3
"""C7 saw spectrum overlay — WAVE-9090-style harmonic/alias-aware FFT metrics."""
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

HARN_LOCK_MARGIN_DB = 100.0


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
    return x[-n:]


def synthesize_additive_saw(sr: int, f0: float, n_samples: int, peak: float = 0.12) -> np.ndarray:
    """Pure band-limited saw: harmonics strictly below Nyquist."""
    t = np.arange(n_samples, dtype=np.float64) / sr
    max_h = int(math.floor(0.499 * sr / f0))
    x = np.zeros_like(t)
    for h in range(1, max_h + 1):
        x += (-2.0 / (math.pi * h)) * np.sin(2.0 * math.pi * h * f0 * t)
    peak_x = np.max(np.abs(x)) + 1e-12
    return (x / peak_x) * peak


def is_harmonic_hz(hz: float, expected_fund_hz: float, sr: float) -> bool:
    if hz < expected_fund_hz * 0.9:
        return True
    for h in range(1, 129):
        hf = h * expected_fund_hz
        if abs(hz - hf) < expected_fund_hz * 0.015:
            return True
        if hf > 0.5 * sr:
            alias_hz = sr - hf
            if alias_hz > 0.0 and abs(hz - alias_hz) < expected_fund_hz * 0.02:
                return True
    return False


def analyse_inharmonics(x: np.ndarray, sr: float, expected_fund_hz: float) -> dict:
    n_target = 1 << int(math.floor(math.log2(min(len(x), int(sr * 4)))))
    n_target = max(n_target, 65536)
    seg = x[-n_target:]
    win = blackman_harris(len(seg))
    spec = np.fft.fft(seg * win)
    n = len(spec)

    k_fund = int(round(expected_fund_hz * n / sr))
    fund_mag = 0.0
    for k in range(max(1, k_fund - 3), min(n // 2, k_fund + 4)):
        fund_mag = max(fund_mag, abs(spec[k]))
    fund_mag /= n
    fund_db = 20.0 * math.log10(fund_mag + 1e-12)

    worst_mag = 0.0
    worst_hz = 0.0
    for k in range(1, n // 2):
        hz = k * sr / n
        if is_harmonic_hz(hz, expected_fund_hz, sr):
            continue
        mag = abs(spec[k]) / n
        if mag > worst_mag:
            worst_mag = mag
            worst_hz = hz
    worst_db = 20.0 * math.log10(worst_mag + 1e-12)
    rel_margin = fund_db - worst_db

    freqs = np.fft.fftfreq(n, 1.0 / sr)[: n // 2]
    mag_lin = np.abs(spec[: n // 2]) / n
    rel_mag_db = 20.0 * np.log10(np.maximum(mag_lin, 1e-12)) - fund_db

    peak_db = 20.0 * math.log10(float(np.max(np.abs(x))) + 1e-12)
    return {
        "fund_hz": expected_fund_hz,
        "fund_db": fund_db,
        "worst_inharmonic_hz": worst_hz,
        "worst_inharmonic_db": worst_db,
        "worst_below_fund_db": rel_margin,
        "peak_dbfs": peak_db,
        "sample_rate": sr,
        "fft_size": n,
        "freqs": freqs,
        "mag_db": rel_mag_db,
    }


def validate_harness(sr: int, expected_hz: float) -> float:
    n = 1 << 18
    x = synthesize_additive_saw(sr, expected_hz, n)
    x = trim_integer_cycles(x, sr, expected_hz)
    start = int(len(x) * 0.05)
    stats = analyse_inharmonics(x[start:], sr, expected_hz)
    margin = stats["worst_below_fund_db"]
    if margin < HARN_LOCK_MARGIN_DB:
        raise RuntimeError(
            f"Spectrum harness failed: additive saw margin {margin:.2f} dB < {HARN_LOCK_MARGIN_DB} dB"
        )
    return margin


def analyse_wav(path: Path, sr_hint: float, expected_hz: float) -> dict:
    x, sr = load_mono(path)
    assert sr == sr_hint or abs(sr - sr_hint) < 1.0
    x = trim_integer_cycles(x, sr, expected_hz)
    start = int(len(x) * 0.12)
    end = int(len(x) * 0.98)
    stats = analyse_inharmonics(x[start:end], sr, expected_hz)
    stats["path"] = str(path)
    stats["expected_hz"] = expected_hz
    return stats


def plot_overlay(before: dict, after: dict, out_png: Path) -> None:
    fig, ax = plt.subplots(figsize=(12, 5), dpi=120)
    ax.plot(before["freqs"], before["mag_db"], color="#ff6b6b", alpha=0.75, label="before (naive saw)")
    ax.plot(after["freqs"], after["mag_db"], color="#4ecdc4", alpha=0.9, label="after (FFT mip + crossfade)")
    fund = after["fund_hz"]
    for h in range(1, 13):
        ax.axvline(fund * h, color="#888888", linestyle=":", linewidth=0.6, alpha=0.35)
    ax.set_xlim(0, min(12000, after["sample_rate"] / 2))
    ax.set_ylim(-120, 6)
    ax.set_xlabel("Frequency (Hz)")
    ax.set_ylabel("Level vs fundamental (dB)")
    worst = after["worst_below_fund_db"]
    ax.set_title(
        f"C7 dry saw @ {after['sample_rate']:.0f} Hz — inharmonic margin {worst:.1f} dB "
        f"(fund {fund:.1f} Hz, FFT {after['fft_size']})"
    )
    ax.grid(True, which="both", alpha=0.25)
    ax.legend(loc="upper right", fontsize=10)
    fig.tight_layout()
    fig.savefig(out_png)
    plt.close(fig)


def main() -> int:
    out_dir = Path(sys.argv[1] if len(sys.argv) > 1 else "/opt/cursor/artifacts/junova-elite-audio")
    expected = float(sys.argv[2]) if len(sys.argv) > 2 else 2093.0
    sr = 44100.0

    harness_margin = validate_harness(sr, expected)
    print(json.dumps({"harness_additive_saw_margin_db": harness_margin}, indent=2))

    harness_wav = out_dir / "c7-additive-harness.wav"
    out_dir.mkdir(parents=True, exist_ok=True)
    ref = synthesize_additive_saw(int(sr), expected, int(sr * 3))
    ref = trim_integer_cycles(ref, int(sr), expected)
    with wave.open(str(harness_wav), "wb") as wf:
        wf.setnchannels(1)
        wf.setsampwidth(2)
        wf.setframerate(int(sr))
        pcm = np.clip(ref * 32767.0, -32768, 32767).astype(np.int16)
        wf.writeframes(pcm.tobytes())

    before_wav = out_dir / "c7-saw-before.wav"
    after_wav = out_dir / "c7-saw-elite.wav"
    direct_wav = out_dir / "c7-mip-bank-direct.wav"
    if not before_wav.is_file() or not after_wav.is_file():
        print("Missing C7 WAV inputs", file=sys.stderr)
        return 1

    if direct_wav.is_file():
        direct = analyse_wav(direct_wav, sr, expected)
        metrics_direct = {k: v for k, v in direct.items() if k not in ("freqs", "mag_db")}
        print(json.dumps({"mip_bank_direct_margin_db": direct["worst_below_fund_db"]}, indent=2))
    else:
        metrics_direct = None

    before = analyse_wav(before_wav, sr, expected)
    after = analyse_wav(after_wav, sr, expected)
    plot_overlay(before, after, out_dir / "c7-spectrum-overlay.png")

    metrics = {
        "harness_additive_saw_margin_db": harness_margin,
        "before": {k: v for k, v in before.items() if k not in ("freqs", "mag_db")},
        "after": {k: v for k, v in after.items() if k not in ("freqs", "mag_db")},
    }
    if metrics_direct is not None:
        metrics["mip_bank_direct"] = metrics_direct
    (out_dir / "spectrum-metrics.json").write_text(json.dumps(metrics, indent=2))
    print(json.dumps({"worst_inharmonic_margin_db": after["worst_below_fund_db"]}, indent=2))
    return 0


if __name__ == "__main__":
    raise SystemExit(main())