#!/usr/bin/env python3
"""Memphis phonk snare — membrane + band-passed wires + flam clap, then SP/tape/clip.

MEMPHIS-DR660 Engine B for kit oneshots. numpy + stdlib wave only.
"""

from __future__ import annotations

import argparse
import math
import wave
from pathlib import Path

import numpy as np

try:
    from .phonk_kick import write_wav_24bit_mono
except ImportError:  # python path/script invocation
    from phonk_kick import write_wav_24bit_mono  # type: ignore

NOTE_HZ = {
    "C3": 130.81,
    "D3": 146.83,
    "E3": 164.81,
    "F3": 174.61,
    "F#3": 185.00,
    "G3": 196.00,
    "A3": 220.00,
}


def _rbj_bandpass(signal: np.ndarray, sample_rate: float, center_hz: float, q: float) -> np.ndarray:
    """RBJ constant-skirt bandpass, Direct Form I."""
    w0 = 2.0 * math.pi * center_hz / sample_rate
    alpha = math.sin(w0) / (2.0 * q)
    b0 = alpha
    b1 = 0.0
    b2 = -alpha
    a0 = 1.0 + alpha
    a1 = -2.0 * math.cos(w0)
    a2 = 1.0 - alpha
    b0, b1, b2 = b0 / a0, b1 / a0, b2 / a0
    a1, a2 = a1 / a0, a2 / a0
    y = np.zeros_like(signal)
    x1 = x2 = y1 = y2 = 0.0
    for i, x0 in enumerate(signal):
        yn = b0 * x0 + b1 * x1 + b2 * x2 - a1 * y1 - a2 * y2
        # Clamp state to keep RBJ DF1 stable on hot noise inputs
        if not math.isfinite(yn) or abs(yn) > 8.0:
            yn = math.copysign(min(8.0, abs(yn) if math.isfinite(yn) else 0.0), yn if yn != 0 else 1.0)
            y1 = y2 = 0.0
        y[i] = yn
        x2, x1 = x1, x0
        y2, y1 = y1, yn
    return np.nan_to_num(y, nan=0.0, posinf=0.0, neginf=0.0)


def generate_memphis_phonk_snare(
    sample_rate: int = 44100,
    duration: float = 0.35,
    membrane_root: float = 220.0,
    pitch_mod_st: float = 24.0,
    pitch_decay: float = 0.018,
    noise_decay: float = 0.190,
    flam_delay_ms: float = 4.0,
    lofi_sr: float = 26040.0,
    bit_depth: int = 12,
    clip_drive_db: float = 5.5,
    seed: int | None = 660,
) -> np.ndarray:
    """Return float32 mono buffer normalized near -0.3 dBFS."""
    rng = np.random.default_rng(seed)
    total_samples = int(round(sample_rate * duration))
    t = np.linspace(0.0, duration, total_samples, endpoint=False)

    # 1) Membrane body
    pitch_env = np.exp(-t / pitch_decay)
    inst_freq = membrane_root * (2.0 ** (pitch_mod_st * pitch_env / 12.0))
    phase = 2.0 * math.pi * np.cumsum(inst_freq) / sample_rate
    membrane_osc = 0.8 * np.sin(phase) + 0.2 * (2.0 / math.pi) * np.arcsin(np.sin(phase))
    body_amp = np.zeros_like(t)
    hold = t <= 0.003
    body_amp[hold] = 1.0
    body_amp[~hold] = np.exp(-(t[~hold] - 0.003) / 0.085)
    membrane_layer = membrane_osc * body_amp * 0.75

    # 2) Noise wires (BP ~2.4 kHz)
    raw_noise = rng.normal(0.0, 1.0, total_samples)
    noise_bp = _rbj_bandpass(raw_noise.astype(np.float64), sample_rate, 2400.0, 1.2)
    noise_amp = np.zeros_like(t)
    att_time = 0.0025
    att = t < att_time
    noise_amp[att] = t[att] / att_time
    noise_amp[~att] = np.exp(-(t[~att] - att_time) / noise_decay)
    noise_layer = noise_bp * noise_amp * 0.55

    # 3) Flam / clap impulses
    delay_samples = int((flam_delay_ms / 1000.0) * sample_rate)
    clap_raw = rng.uniform(-1.0, 1.0, total_samples)
    clap_amp = np.zeros_like(t)
    for offset_ms, gain in ((0.0, 0.6), (2.0, 0.8), (flam_delay_ms, 1.0)):
        offset_idx = int((offset_ms / 1000.0) * sample_rate)
        if offset_idx >= total_samples:
            continue
        sub_t = t[offset_idx:] - t[offset_idx]
        clap_amp[offset_idx:] += gain * np.exp(-sub_t / 0.035)
    clap_layer = np.roll(clap_raw * clap_amp * 0.45, delay_samples)
    clap_layer[:delay_samples] = 0.0

    composite = membrane_layer + noise_layer + clap_layer

    # 4) 12-bit + 26.04 kHz hold
    dec_step = sample_rate / lofi_sr
    sample_indices = (np.floor(np.arange(total_samples) / dec_step) * dec_step).astype(int)
    sample_indices = np.clip(sample_indices, 0, total_samples - 1)
    resampled = composite[sample_indices]
    q_steps = 2 ** (bit_depth - 1)
    quantized = np.round(resampled * q_steps) / q_steps

    # 5) HPF ~105 Hz then tape LPF ~11.5 kHz
    hp_alpha = math.exp(-2.0 * math.pi * 105.0 / sample_rate)
    hp_sig = np.zeros_like(quantized)
    for i in range(1, total_samples):
        hp_sig[i] = hp_alpha * (hp_sig[i - 1] + quantized[i] - quantized[i - 1])

    lp_alpha = math.exp(-2.0 * math.pi * 11500.0 / sample_rate)
    tape_colored = np.zeros_like(hp_sig)
    for i in range(1, total_samples):
        tape_colored[i] = (1 - lp_alpha) * hp_sig[i] + lp_alpha * tape_colored[i - 1]

    # 6) Soft clip + normalize
    drive_gain = 10.0 ** (clip_drive_db / 20.0)
    clipped = np.tanh(tape_colored * drive_gain)
    peak = float(np.max(np.abs(clipped))) or 1.0
    normalized = (clipped / peak) * (10.0 ** (-0.3 / 20.0))
    return normalized.astype(np.float32)


def main() -> None:
    parser = argparse.ArgumentParser(description="Generate a Memphis phonk snare oneshot")
    parser.add_argument("--key", default="A3", help="Note name for membrane (e.g. F#3, A3)")
    parser.add_argument("--freq", type=float, default=None, help="Override membrane Hz")
    parser.add_argument("--out", type=Path, default=Path("memphis_phonk_snare.wav"))
    parser.add_argument("--drive-db", type=float, default=5.5)
    parser.add_argument("--flam-ms", type=float, default=4.0)
    parser.add_argument("--seed", type=int, default=660)
    args = parser.parse_args()

    root = args.freq if args.freq is not None else NOTE_HZ.get(
        args.key.upper().replace("♯", "#"), 220.0
    )
    buf = generate_memphis_phonk_snare(
        membrane_root=root,
        clip_drive_db=args.drive_db,
        flam_delay_ms=args.flam_ms,
        seed=args.seed,
    )
    write_wav_24bit_mono(args.out, buf)
    print(f"wrote {args.out} ({len(buf)} samples @ 44100, membrane={root:.2f} Hz)")


if __name__ == "__main__":
    main()
