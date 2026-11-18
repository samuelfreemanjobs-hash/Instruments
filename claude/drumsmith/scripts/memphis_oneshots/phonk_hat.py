#!/usr/bin/env python3
"""Memphis EDGE closed hat — metallic noise + SP-1200 grit for kit oneshots."""

from __future__ import annotations

import argparse
import math
from pathlib import Path

import numpy as np

try:
    from .phonk_kick import write_wav_16bit_mono, write_wav_24bit_mono
    from .phonk_snare import _rbj_bandpass
except ImportError:
    from phonk_kick import write_wav_16bit_mono, write_wav_24bit_mono  # type: ignore
    from phonk_snare import _rbj_bandpass  # type: ignore


def generate_memphis_phonk_hat(
    sample_rate: int = 44100,
    duration: float = 0.08,
    center_hz: float = 9000.0,
    q: float = 1.8,
    noise_decay: float = 0.028,
    lofi_sr: float = 26040.0,
    bit_depth: int = 12,
    clip_drive_db: float = 2.5,
    tape_lpf_hz: float = 13000.0,
    seed: int | None = 808,
) -> np.ndarray:
    """Return float32 mono closed-hat buffer (~-0.3 dBFS peak)."""
    rng = np.random.default_rng(seed)
    total_samples = int(round(sample_rate * duration))
    t = np.linspace(0.0, duration, total_samples, endpoint=False)

    raw = rng.normal(0.0, 1.0, total_samples)
    metallic = _rbj_bandpass(raw.astype(np.float64), sample_rate, center_hz, q)
    # Second partial for EDGE bite
    bite = _rbj_bandpass(raw.astype(np.float64), sample_rate, center_hz * 1.35, q * 0.9)
    noise = 0.7 * metallic + 0.3 * bite

    amp = np.exp(-t / noise_decay)
    # 0.5 ms attack
    att = min(int(0.0005 * sample_rate), total_samples)
    if att > 0:
        amp[:att] *= np.linspace(0.0, 1.0, att)
    signal = noise * amp

    step = sample_rate / lofi_sr
    indices = (np.floor(np.arange(total_samples) / step) * step).astype(int)
    indices = np.clip(indices, 0, total_samples - 1)
    resampled = signal[indices]
    q_levels = 2 ** (bit_depth - 1)
    quantized = np.round(resampled * q_levels) / q_levels

    alpha = math.exp(-2.0 * math.pi * tape_lpf_hz / sample_rate)
    filtered = np.zeros_like(quantized)
    for i in range(1, total_samples):
        filtered[i] = (1 - alpha) * quantized[i] + alpha * filtered[i - 1]

    drive = 10.0 ** (clip_drive_db / 20.0)
    clipped = np.tanh(filtered * drive)
    peak = float(np.max(np.abs(clipped))) or 1.0
    out = (clipped / peak) * (10.0 ** (-0.3 / 20.0))
    return out.astype(np.float32)


def main() -> None:
    parser = argparse.ArgumentParser(description="Generate a Memphis phonk closed-hat oneshot")
    parser.add_argument("--out", type=Path, default=Path("memphis_phonk_hat.wav"))
    parser.add_argument("--center-hz", type=float, default=9000.0)
    parser.add_argument("--pcm16", action="store_true", help="Write kit PCM16 instead of 24-bit")
    parser.add_argument("--seed", type=int, default=808)
    args = parser.parse_args()

    buf = generate_memphis_phonk_hat(center_hz=args.center_hz, seed=args.seed)
    if args.pcm16:
        write_wav_16bit_mono(args.out, buf)
    else:
        write_wav_24bit_mono(args.out, buf)
    print(f"wrote {args.out} ({len(buf)} samples @ 44100, center={args.center_hz:.0f} Hz)")


if __name__ == "__main__":
    main()
