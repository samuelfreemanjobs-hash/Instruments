"""Offline QA metrics for rendered drums."""

from __future__ import annotations

import numpy as np
from numpy.typing import NDArray

FloatArray = NDArray[np.floating]


def rms(audio: FloatArray) -> float:
    return float(np.sqrt(np.mean(np.square(audio))))


def crest_factor(audio: FloatArray, eps: float = 1e-9) -> float:
    peak = float(np.max(np.abs(audio)))
    return peak / (rms(audio) + eps)


def zero_crossing_rate(audio: FloatArray) -> float:
    if len(audio) < 2:
        return 0.0
    signs = audio[:-1] * audio[1:]
    return float(np.mean(signs < 0))


def sub_peak_hz(audio: FloatArray, sr: int, lo: float = 38.0, hi: float = 52.0) -> float:
    n = len(audio)
    if n < 64:
        return 0.0
    spectrum = np.abs(np.fft.rfft(audio))
    freqs = np.fft.rfftfreq(n, 1.0 / sr)
    mask = (freqs >= lo) & (freqs <= hi)
    if not np.any(mask):
        return 0.0
    idx = int(np.argmax(spectrum[mask]))
    return float(freqs[mask][idx])


def fingerprint(audio: FloatArray, sr: int) -> dict[str, float]:
    return {
        "rms": rms(audio),
        "crest_factor": crest_factor(audio),
        "zero_crossing_rate": zero_crossing_rate(audio),
        "sub_peak_hz": sub_peak_hz(audio, sr),
    }


def match_score_against_reference(
    synth: dict[str, float],
    ref: dict[str, float],
    zcr_weight: float = 1000.0,
) -> float:
    """Enterprise-style crest/ZCR distance score (0–100, higher is better)."""
    zcr_delta = abs(synth["zero_crossing_rate"] - ref["zero_crossing_rate"])
    crest_delta = abs(synth["crest_factor"] - ref["crest_factor"])
    penalty = zcr_delta * zcr_weight + crest_delta * 10.0
    return max(0.0, min(100.0, 100.0 - penalty))


def passes_basic_qa(
    audio: FloatArray,
    sr: int,
    min_peak: float = 0.5,
    sub_lo: float = 38.0,
    sub_hi: float = 52.0,
    require_sub: bool = False,
) -> tuple[bool, str]:
    peak = float(np.max(np.abs(audio)))
    if peak < min_peak:
        return False, f"peak {peak:.3f} below {min_peak}"
    if require_sub:
        sub = sub_peak_hz(audio, sr, sub_lo, sub_hi)
        if sub < sub_lo or sub > sub_hi:
            return False, f"sub peak {sub:.1f} Hz outside {sub_lo}-{sub_hi}"
    return True, "ok"


def refine_until_pass(
    audio: FloatArray,
    mutate_fn,
    max_attempts: int = 3,
    min_rms: float = 0.2,
) -> FloatArray:
    """Batch-only grit loop (replaces Evolutionary Critic agent)."""
    current = audio
    for attempt in range(max_attempts):
        if rms(current) >= min_rms:
            return current
        current = mutate_fn(current, attempt)
    return current
