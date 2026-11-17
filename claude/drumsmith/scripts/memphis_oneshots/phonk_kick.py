#!/usr/bin/env python3
"""Memphis phonk kick — pitch-swept sine + SP-1200 decimation + tape LPF + soft clip.

Original synthesis only (MEMPHIS-660 Engine B). Writes 24-bit mono WAV via stdlib wave.
"""

from __future__ import annotations

import argparse
import math
import wave
from pathlib import Path

import numpy as np

# Common roots for kit / DAW key locking
NOTE_HZ = {
    "C1": 32.70,
    "D1": 36.71,
    "E1": 41.20,
    "F1": 43.65,
    "F#1": 46.25,
    "G1": 49.00,
    "A1": 55.00,
}


def generate_memphis_phonk_kick(
    sample_rate: int = 44100,
    root_freq: float = 43.65,
    duration: float = 0.25,
    pitch_mod_depth: float = 48.0,
    pitch_decay: float = 0.022,
    lofi_sr: float = 26040.0,
    bit_depth: int = 12,
    clip_drive_db: float = 4.0,
    tape_lpf_hz: float = 12000.0,
) -> np.ndarray:
    """Return float32 mono buffer in [-1, 1], normalized to about -0.3 dBFS."""
    total_samples = int(sample_rate * duration)
    t = np.linspace(0.0, duration, total_samples, endpoint=False)

    pitch_env = np.exp(-t / pitch_decay)
    inst_freq = root_freq * (2.0 ** (pitch_mod_depth * pitch_env / 12.0))
    phase = 2.0 * math.pi * np.cumsum(inst_freq) / sample_rate
    sine_wave = np.sin(phase)

    amp_decay = 0.140
    amp_env = np.ones_like(t)
    decay_mask = t > 0.005
    amp_env[decay_mask] = np.exp(-(t[decay_mask] - 0.005) / amp_decay)
    signal = sine_wave * amp_env

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

    drive_linear = 10.0 ** (clip_drive_db / 20.0)
    clipped = np.tanh(filtered * drive_linear)

    peak = float(np.max(np.abs(clipped))) or 1.0
    out = (clipped / peak) * (10.0 ** (-0.3 / 20.0))
    return out.astype(np.float32)


def write_wav_24bit_mono(path: Path, samples: np.ndarray, sample_rate: int = 44100) -> None:
    int24 = (samples * 8388607.0).astype(np.int32)
    raw = bytearray()
    for s in int24:
        raw.extend(int(s).to_bytes(4, byteorder="little", signed=True)[:3])
    path.parent.mkdir(parents=True, exist_ok=True)
    with wave.open(str(path), "wb") as wav_file:
        wav_file.setnchannels(1)
        wav_file.setsampwidth(3)
        wav_file.setframerate(sample_rate)
        wav_file.writeframes(bytes(raw))


def main() -> None:
    parser = argparse.ArgumentParser(description="Generate a Memphis phonk kick oneshot")
    parser.add_argument("--key", default="F1", help="Note name (F1, D1, …) or omit and use --freq")
    parser.add_argument("--freq", type=float, default=None, help="Override fundamental Hz")
    parser.add_argument("--out", type=Path, default=Path("memphis_phonk_kick.wav"))
    parser.add_argument("--drive-db", type=float, default=4.0)
    parser.add_argument("--duration", type=float, default=0.25)
    args = parser.parse_args()

    root = args.freq if args.freq is not None else NOTE_HZ.get(args.key.upper().replace("♯", "#"), 43.65)
    buf = generate_memphis_phonk_kick(
        root_freq=root,
        duration=args.duration,
        clip_drive_db=args.drive_db,
    )
    write_wav_24bit_mono(args.out, buf)
    print(f"wrote {args.out} ({len(buf)} samples @ 44100, root={root:.2f} Hz)")


if __name__ == "__main__":
    main()
