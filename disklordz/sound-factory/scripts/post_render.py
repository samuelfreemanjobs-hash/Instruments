"""Post-render limiter (~-6 dBTP) and mono sub sanity check."""

from __future__ import annotations

import math
from pathlib import Path

from wav_util import read_mono_wav, write_mono_wav


def apply_limiter(path: Path, ceiling: float = 0.501) -> None:
    samples, _ = read_mono_wav(path)
    peak = max((abs(s) for s in samples), default=0.0)
    if peak <= ceiling or peak < 1e-9:
        return
    scale = ceiling / peak
    write_mono_wav(path, [s * scale for s in samples], target_peak=ceiling)


def mono_sub_energy_ok(path: Path, max_low_ratio: float = 0.92) -> bool:
    """If almost all energy is sub-only, patch may be unusable as lead."""
    samples, rate = read_mono_wav(path)
    if len(samples) < 64:
        return True
    # crude: compare full-band vs one-pole lowpass-ish average
    low = 0.0
    state = 0.0
    alpha = 0.02
    for s in samples:
        state = state + alpha * (s - state)
        low += state * state
    total = sum(s * s for s in samples) + 1e-12
    return (low / total) <= max_low_ratio


def post_process_wav(path: Path) -> None:
    apply_limiter(path)
    # Sub-dominance is evaluated in factory_qa v2 (`mono_sub_only`), not here.
