#!/usr/bin/env python3
"""Vectorized Trap kick + 808 blueprint (NumPy) — DDSP/ML and JUCE reference curves."""

from __future__ import annotations

import math
from typing import Literal

import numpy as np

SR = 44100


def adsr_envelope(n: int, a: float, d: float, s: float, r: float, gate: float, sr: float = SR) -> np.ndarray:
    t = np.arange(n, dtype=np.float64) / sr
    env = np.zeros(n, dtype=np.float64)
    for i in range(n):
        ti = t[i]
        if ti < a:
            env[i] = ti / a if a > 0 else 1.0
        elif ti < a + d:
            env[i] = 1.0 - (1.0 - s) * ((ti - a) / d)
        elif ti < gate:
            env[i] = s
        elif ti < gate + r:
            env[i] = s * (1.0 - (ti - gate) / r) if r > 0 else 0.0
        else:
            env[i] = 0.0
    return env


def render_trap_kick(
    duration: float = 0.28,
    root_hz: float = 50.0,
    pitch_mod_semitones: float = 36.0,
    pitch_decay: float = 0.022,
    sr: int = SR,
) -> np.ndarray:
    """Hard trap kick: beater click + pitch-mod sine body + sub octave."""
    n = int(sr * (duration + 0.05))
    t = np.arange(n, dtype=np.float64) / sr
    gate = duration
    ae = adsr_envelope(n, 0.001, 0.12, 0.0, 0.05, gate, sr)

    beater = np.zeros(n, dtype=np.float64)
    click = t < 0.006
    beater[click] = np.exp(-t[click] / 0.00075) * np.sin(2 * np.pi * 3200 * t[click]) * ae[click]

    pitch_env = np.exp(-t / pitch_decay)
    f0 = root_hz * np.power(2.0, (pitch_mod_semitones * pitch_env) / 12.0)
    phase = np.cumsum(2 * np.pi * f0 / sr)
    body = np.sin(phase) * ae * 0.82
    sub = np.sin(phase * 0.5) * ae * 0.45
    return beater + body + sub


def render_trap_808(
    duration: float = 1.8,
    root_hz: float = 47.0,
    glide_ms: float = 120.0,
    glide_semitones: float = 7.0,
    glide_exponent: float = 2.2,
    saturation: Literal["tanh", "clip"] = "tanh",
    sr: int = SR,
) -> np.ndarray:
    """Deep 808: sine + optional triangle harmonic layer + pitch glide."""
    n = int(sr * duration)
    t = np.arange(n, dtype=np.float64) / sr
    ae = adsr_envelope(n, 0.003, 0.8, 0.85, 1.2, duration * 0.95, sr)

    glide_t = max(1e-6, glide_ms / 1000.0)
    target = root_hz * (2.0 ** (glide_semitones / 12.0))
    u = np.clip(t / glide_t, 0.0, 1.0)
    freq = root_hz + (target - root_hz) * np.power(u, glide_exponent)

    phase = np.cumsum(2 * np.pi * freq / sr)
    sine = np.sin(phase)
    tri = np.sin(phase) + 0.28 * np.sin(phase * 3)
    mix = sine * 0.78 + tri * 0.22 * 0.35

    if saturation == "tanh":
        mix = np.tanh(mix * 1.4)
    else:
        mix = np.clip(mix * 1.2, -1.0, 1.0)

    return mix * ae


def normalize_peak(samples: np.ndarray, peak: float = 0.95) -> np.ndarray:
    m = float(np.max(np.abs(samples))) or 1e-9
    return (samples / m) * peak
