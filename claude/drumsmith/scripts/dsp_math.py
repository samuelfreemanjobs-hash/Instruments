#!/usr/bin/env python3
"""Hand-verifiable timing and level helpers for DRUMSMITH (stdlib only)."""

from __future__ import annotations

import math
from typing import Iterable


def bar_length_samples(sample_rate: int, bpm: float, beats_per_bar: int = 4) -> int:
    """Integer samples for one bar (truncates toward zero, matches DAW integer bars)."""
    if sample_rate <= 0 or bpm <= 0 or beats_per_bar <= 0:
        raise ValueError("sample_rate, bpm, beats_per_bar must be positive")
    return int(sample_rate * beats_per_bar * 60.0 / bpm)


def beat_length_samples(sample_rate: int, bpm: float) -> int:
    return int(sample_rate * 60.0 / bpm)


def ms_to_samples(ms: float, sample_rate: int) -> int:
    return int(sample_rate * ms / 1000.0)


def samples_to_ms(samples: int, sample_rate: int) -> float:
    return 1000.0 * samples / sample_rate


def linear_to_dbfs(peak: float) -> float:
    if peak <= 0.0:
        return float("-inf")
    return 20.0 * math.log10(peak)


def dbfs_to_linear(dbfs: float) -> float:
    return 10.0 ** (dbfs / 20.0)


def glide_samples_for_pitch_drop(
    sample_rate: int,
    start_hz: float,
    end_hz: float,
    glide_ms: float,
) -> int:
    """Samples for an expo pitch glide segment (duration only; not synthesis)."""
    if start_hz <= 0 or end_hz <= 0 or glide_ms <= 0:
        raise ValueError("invalid glide parameters")
    return ms_to_samples(glide_ms, sample_rate)


def approximate_true_peak_linear(samples: Iterable[int], full_scale: int) -> float:
    """Sample-peak meter (not ITU true-peak). Tag results as approximate in QA copy."""
    peak = 0
    for s in samples:
        a = abs(s)
        if a > peak:
            peak = a
    return peak / float(full_scale)


def int24_full_scale() -> int:
    return (1 << 23) - 1