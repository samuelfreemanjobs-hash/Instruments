#!/usr/bin/env python3
"""MEMPHIS-660 kick Engine B — mirrors disklordz/memphis-architect/index.html synthesizeKick()."""

from __future__ import annotations

import math
import wave
from pathlib import Path


def get_adsr(
    t: float, a: float, d: float, s: float, r: float, gate: float
) -> float:
    if t < a:
        return t / a if a > 0 else 1.0
    if t < a + d:
        return 1.0 - (1.0 - s) * ((t - a) / d) if d > 0 else s
    if t < gate:
        return s
    if t < gate + r:
        return s * (1.0 - (t - gate) / r) if r > 0 else 0.0
    return 0.0


def generate_memphis_phonk_kick(
    filename: str | Path = "memphis_phonk_kick.wav",
    sample_rate: int = 44100,
    root_freq: float = 43.65,
    gate_length_sec: float = 0.25,
    pitch_mod_depth: float = 48.0,
    pitch_decay_sec: float = 0.022,
    lofi_sr: float = 26040.0,
    bit_depth: int = 12,
    clip_drive_db: float = 4.0,
    amp_a: float = 0.0,
    amp_d: float = 0.14,
    amp_s: float = 0.0,
    amp_r: float = 0.05,
    flt_type: str = "tape",
    flt_cut: float = 12000.0,
    flt_res: float = 0.7,
) -> Path:
    sr = sample_rate
    total_duration = gate_length_sec + amp_r
    total_samples = int(sr * total_duration)
    raw = [0.0] * total_samples

    phase = 0.0
    for i in range(total_samples):
        t = i / sr
        pitch_env = math.exp(-t / pitch_decay_sec)
        inst_freq = root_freq * (2.0 ** (pitch_mod_depth * pitch_env / 12.0))
        phase += 2.0 * math.pi * inst_freq / sr
        amp_env = get_adsr(t, amp_a, amp_d, amp_s, amp_r, gate_length_sec)
        raw[i] = math.sin(phase) * amp_env

    step = sr / lofi_sr
    q_levels = 2 ** (bit_depth - 1)
    drive = 10.0 ** (clip_drive_db / 20.0)
    prev_filtered = 0.0
    ic1eq = ic2eq = 0.0
    out = [0.0] * total_samples
    peak = 0.0

    for i in range(total_samples):
        idx = int(math.floor(math.floor(i / step) * step))
        idx = min(idx, total_samples - 1)
        quantized = round(raw[idx] * q_levels) / q_levels

        if flt_type == "tape":
            alpha = math.exp(-2.0 * math.pi * flt_cut / sr)
            filtered = (1 - alpha) * quantized + alpha * prev_filtered
            prev_filtered = filtered
        else:
            g = math.tan(math.pi * flt_cut / sr)
            k = 1.0 / flt_res
            a1 = 1.0 / (1.0 + g * (g + k))
            a2 = g * a1
            a3 = g * a2
            v3 = quantized - ic2eq
            v1 = a1 * ic1eq + a2 * v3
            v2 = ic2eq + a2 * ic1eq + a3 * v3
            ic1eq = 2.0 * v1 - ic1eq
            ic2eq = 2.0 * v2 - ic2eq
            if flt_type == "lp":
                filtered = v2
            elif flt_type == "bp":
                filtered = v1
            else:
                filtered = quantized - k * v1 - v2

        clipped = math.tanh(filtered * drive)
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
    p = generate_memphis_phonk_kick()
    print(f"wrote {p} ({p.stat().st_size} bytes)")
