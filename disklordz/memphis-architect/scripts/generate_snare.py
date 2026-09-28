#!/usr/bin/env python3
"""MEMPHIS-DR660 snare Engine B — mirrors index.html synthesizeSnare()."""

from __future__ import annotations

import math
import random
import wave
from pathlib import Path


def generate_memphis_phonk_snare(
    filename: str | Path = "memphis_phonk_snare.wav",
    sample_rate: int = 44100,
    duration_sec: float = 0.35,
    membrane_root: float = 220.0,
    pitch_mod_st: float = 24.0,
    pitch_decay_sec: float = 0.018,
    noise_decay_sec: float = 0.19,
    flam_delay_sec: float = 0.004,
    lofi_sr: float = 26040.0,
    bit_depth: int = 12,
    clip_drive_db: float = 5.5,
    seed: int | None = None,
) -> Path:
    if seed is not None:
        random.seed(seed)

    sr = sample_rate
    total_samples = int(sr * duration_sec)
    composite = [0.0] * total_samples

    phase = 0.0
    for i in range(total_samples):
        t = i / sr
        p_env = math.exp(-t / pitch_decay_sec)
        inst_freq = membrane_root * (2.0 ** (pitch_mod_st * p_env / 12.0))
        phase += 2.0 * math.pi * inst_freq / sr
        osc = 0.8 * math.sin(phase) + 0.2 * math.asin(math.sin(phase)) * (2.0 / math.pi)
        body_amp = 1.0 if t <= 0.003 else math.exp(-(t - 0.003) / 0.085)
        composite[i] += osc * body_amp * 0.75

    cf, bw = 2400.0, 1800.0
    w0 = 2.0 * math.pi * cf / sr
    alpha_bp = math.sin(w0) * math.sinh(
        math.log(2.0) / 2.0 * (bw / cf) * w0 / math.sin(w0)
    )
    b0, b1, b2 = alpha_bp, 0.0, -alpha_bp
    a0 = 1 + alpha_bp
    a1 = -2 * math.cos(w0)
    a2 = 1 - alpha_bp
    d1 = d2 = 0.0
    att_time = 0.0025

    for i in range(total_samples):
        t = i / sr
        x = random.gauss(0, 1)
        y = (b0 * x) + d1
        d1 = (b1 * x) - (a1 * y) / a0 + d2
        d2 = (b2 * x) - (a2 * y) / a0
        noise_bp = y / a0
        noise_amp = (
            (t / att_time)
            if t < att_time
            else math.exp(-(t - att_time) / noise_decay_sec)
        )
        composite[i] += noise_bp * noise_amp * 0.55

    for i in range(total_samples):
        t = i / sr
        clap_raw = random.uniform(-1, 1)
        t_clap = t - flam_delay_sec
        clap_amp = 0.0
        if t_clap >= 0:
            clap_amp += 0.6 * math.exp(-t_clap / 0.035)
            if t_clap >= 0.002:
                clap_amp += 0.8 * math.exp(-(t_clap - 0.002) / 0.035)
            if t_clap >= flam_delay_sec:
                clap_amp += 1.0 * math.exp(-(t_clap - flam_delay_sec) / 0.035)
        composite[i] += clap_raw * clap_amp * 0.45

    dec_step = sr / lofi_sr
    q_levels = 2 ** (bit_depth - 1)
    drive = 10.0 ** (clip_drive_db / 20.0)
    hp_alpha = math.exp(-2.0 * math.pi * 105.0 / sr)
    lp_alpha = math.exp(-2.0 * math.pi * 11500.0 / sr)
    hp_prev = quant_prev = 0.0
    tape_colored = 0.0
    out = [0.0] * total_samples
    peak = 0.0

    for i in range(total_samples):
        idx = int(math.floor(math.floor(i / dec_step) * dec_step))
        idx = min(idx, total_samples - 1)
        quantized = round(composite[idx] * q_levels) / q_levels
        hp_sig = hp_alpha * (hp_prev + quantized - quant_prev)
        quant_prev = quantized
        hp_prev = hp_sig
        tape_colored = (1 - lp_alpha) * hp_sig + lp_alpha * tape_colored
        clipped = math.tanh(tape_colored * drive)
        out[i] = clipped
        peak = max(peak, abs(clipped))

    target = 10.0 ** (-0.3 / 20.0)
    norm = target / peak if peak > 0 else 1.0
    out = [s * norm for s in out]

    path = Path(filename)
    raw_bytes = bytearray()
    for s in out:
        val = int(round(max(-1.0, min(1.0, s)) * 8388607.0))
        b = val.to_bytes(4, byteorder="little", signed=True)
        raw_bytes.extend(b[:3])

    with wave.open(str(path), "wb") as wav:
        wav.setnchannels(1)
        wav.setsampwidth(3)
        wav.setframerate(sr)
        wav.writeframes(raw_bytes)
    return path


if __name__ == "__main__":
    p = generate_memphis_phonk_snare(seed=660)
    print(f"wrote {p} ({p.stat().st_size} bytes)")
