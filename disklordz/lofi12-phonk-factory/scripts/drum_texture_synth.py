"""Synthesized dark loop textures (pads, bells) — no samples."""

from __future__ import annotations

import math
import random

from drum_dsp import SRC_RATE, apply_saturation, bit_depth_reduce, one_pole_lp, zero_order_hold

NOTE_FREQ = {
    "Am": 110.0,
    "Dm": 146.83,
    "Em": 164.81,
    "Bm": 123.47,
    "Fm": 87.31,
}


def _reverb_tail(samples: list[float], decay: float, mix: float) -> list[float]:
    if not samples:
        return []
    out = list(samples)
    delay_len = int(SRC_RATE * 0.031)
    wet = [0.0] * len(samples)
    fb = min(0.92, decay)
    buf = [0.0] * delay_len
    idx = 0
    for i, dry in enumerate(samples):
        tap = buf[idx]
        wet[i] = dry + tap * mix
        buf[idx] = dry * mix + tap * fb
        idx = (idx + 1) % delay_len
    return [out[i] * (1 - mix) + wet[i] for i in range(len(samples))]


def synth_minor_pad(
    *,
    bars: int,
    bpm: float,
    key: str,
    seed: int,
    duration_sec: float | None = None,
) -> list[float]:
    rng = random.Random(seed)
    root = NOTE_FREQ.get(key, NOTE_FREQ["Am"])
    third = root * (2 ** (3 / 12))
    fifth = root * (2 ** (7 / 12))
  # minor voicing
    beat = 60.0 / bpm
    bar_sec = beat * 4
    dur = duration_sec or bar_sec * bars
    n = int(SRC_RATE * dur)
    out: list[float] = []
    for i in range(n):
        t = i / SRC_RATE
        env = (1 - math.exp(-t / 0.08)) * math.exp(-t / (bar_sec * bars * 0.85))
        det = 1.0 + rng.uniform(-0.004, 0.004)
        s = (
            math.sin(2 * math.pi * root * det * t)
            + math.sin(2 * math.pi * third * det * t) * 0.72
            + math.sin(2 * math.pi * fifth * det * t) * 0.45
        )
        wobble = 1.0 + 0.012 * math.sin(t * 4.2 + seed * 0.01)
        out.append(s * env * 0.22 * wobble)
    out = one_pole_lp(out, fc_hz=2400.0)
    out = zero_order_hold(out, 3)
    out = bit_depth_reduce(out, 9)
    out = apply_saturation(out, 1.8)
    return _reverb_tail(out, decay=0.78, mix=0.55)


def synth_eerie_bell_motif(
    *,
    bpm: float,
    key: str,
    seed: int,
    bars: int = 2,
) -> list[float]:
    rng = random.Random(seed + 41)
    root = NOTE_FREQ.get(key, NOTE_FREQ["Dm"]) * 2
    beat = 60.0 / bpm
    bar_sec = beat * 4
    dur = bar_sec * bars
    n = int(SRC_RATE * dur)
    motif = [0, 3, 2, 5]
    step_sec = bar_sec / 4
    out = [0.0] * n
    for rep in range(bars):
        for i, deg in enumerate(motif):
            hit_t = rep * bar_sec + i * step_sec * 0.95
            freq = root * (2 ** (deg / 12))
            hit_len = int(SRC_RATE * min(0.35, step_sec * 0.9))
            start = int(hit_t * SRC_RATE)
            for j in range(hit_len):
                idx = start + j
                if idx >= n:
                    break
                t = j / SRC_RATE
                env = math.exp(-t / 0.14) * (1 - math.exp(-t / 0.004))
                bell = math.sin(2 * math.pi * freq * t) + math.sin(2 * math.pi * freq * 2.71 * t) * 0.35
                bell *= env * (0.35 + rng.uniform(0, 0.08))
                out[idx] += bell
    out = one_pole_lp(out, fc_hz=5200.0)
    out = apply_saturation(out, 2.2)
    return _reverb_tail(out, decay=0.82, mix=0.62)
