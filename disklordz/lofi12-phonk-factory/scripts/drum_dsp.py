"""Core DSP blocks for DiskLordz Drum Factory (synthesized one-shots, Phase 1)."""

from __future__ import annotations

import math
from typing import Iterable

SRC_RATE = 44100

# One-shot peak / loudness targets (not loop -14 LUFS — see drum_qa.QA_TARGETS)
ONE_SHOT_PEAK_DBFS = -1.0
ONE_SHOT_RMS_DBFS = -18.0


def sigmoid_saturation(x: float, k: float) -> float:
    """Unity-gain at origin: y(0)=0 slope 1. Spec's (3+k)x/(1+k|x|) had 3x gain at k=0."""
    k = max(0.0, k)
    ax = abs(x)
    if ax < 1e-12:
        return x
    return x * (1.0 + k) / (1.0 + k * ax)


def apply_saturation(samples: list[float], k: float) -> list[float]:
    if k <= 0:
        return list(samples)
    return [sigmoid_saturation(s, k) for s in samples]


def zero_order_hold(samples: list[float], hold: int) -> list[float]:
    if hold <= 1:
        return list(samples)
    out: list[float] = []
    for i, s in enumerate(samples):
        if i % hold == 0:
            hold_val = s
        out.append(hold_val)
    return out


def bit_depth_reduce(samples: list[float], bits: int) -> list[float]:
    bits = int(max(2, min(16, bits)))
    levels = 2 ** (bits - 1)
    return [round(s * levels) / levels for s in samples]


def dc_offset(samples: list[float]) -> float:
    if not samples:
        return 0.0
    return sum(samples) / len(samples)


def remove_dc(samples: list[float], threshold: float = 1e-4) -> list[float]:
    off = dc_offset(samples)
    if abs(off) <= threshold:
        return list(samples)
    return [s - off for s in samples]


def one_pole_hp(samples: list[float], fc_hz: float = 5.0, sr: int = SRC_RATE) -> list[float]:
    if not samples:
        return []
    rc = 1.0 / (2 * math.pi * fc_hz)
    dt = 1.0 / sr
    alpha = rc / (rc + dt)
    out: list[float] = []
    y_prev = 0.0
    x_prev = samples[0]
    for x in samples:
        y = alpha * (y_prev + x - x_prev)
        out.append(y)
        y_prev = y
        x_prev = x
    return out


def peak_abs(samples: Iterable[float]) -> float:
    return max((abs(s) for s in samples), default=0.0)


def true_peak_estimate(samples: list[float]) -> float:
    """Oversample x4 linear — sufficient for QA gate on one-shots."""
    if len(samples) < 2:
        return peak_abs(samples)
    peak = 0.0
    for i in range(len(samples) - 1):
        a, b = samples[i], samples[i + 1]
        for k in range(4):
            t = k / 4.0
            s = a * (1 - t) + b * t
            peak = max(peak, abs(s))
    return peak


def rms_db(samples: list[float]) -> float:
    if not samples:
        return -120.0
    e = sum(s * s for s in samples) / len(samples)
    if e < 1e-20:
        return -120.0
    return 10.0 * math.log10(e)


def normalize_peak(samples: list[float], peak_linear: float) -> list[float]:
    m = peak_abs(samples) or 1.0
    g = peak_linear / m
    return [s * g for s in samples]


def normalize_rms(samples: list[float], target_db: float) -> list[float]:
    cur = rms_db(samples)
    if cur < -100:
        return list(samples)
    gain_db = target_db - cur
    gain = 10 ** (gain_db / 20.0)
    return [s * gain for s in samples]


def low_shelf_boost(samples: list[float], amount: float, fc_hz: float = 55.0, sr: int = SRC_RATE) -> list[float]:
    """Light LF weight: blend with one-pole LP of the signal."""
    if amount <= 0:
        return list(samples)
    rc = 1.0 / (2 * math.pi * fc_hz)
    dt = 1.0 / sr
    alpha = dt / (rc + dt)
    lp: list[float] = []
    y = 0.0
    for x in samples:
        y += alpha * (x - y)
        lp.append(y)
    return [samples[i] + lp[i] * amount * 0.6 for i in range(len(samples))]


def hard_clip(samples: list[float], threshold: float = 0.92) -> list[float]:
    return [max(-threshold, min(threshold, s)) for s in samples]


def one_pole_lp(samples: list[float], fc_hz: float, sr: int = SRC_RATE) -> list[float]:
    if not samples:
        return []
    rc = 1.0 / (2 * math.pi * fc_hz)
    dt = 1.0 / sr
    alpha = dt / (rc + dt)
    out: list[float] = []
    y = samples[0]
    for x in samples:
        y += alpha * (x - y)
        out.append(y)
    return out


def acoustic_features(samples: list[float], sr: int = SRC_RATE) -> tuple[float, ...]:
    """8-D feature vector for near-duplicate rejection."""
    if not samples:
        return (0.0,) * 8
    n = len(samples)
    rms = math.sqrt(sum(s * s for s in samples) / n)
    pk = peak_abs(samples)
    zcr = sum(1 for i in range(1, n) if samples[i - 1] * samples[i] < 0) / max(1, n - 1)
    attack_idx = min(range(n), key=lambda i: abs(samples[i]), default=0)
    attack_t = attack_idx / sr
    decay_t = 0.0
    thr = pk * 0.37
    for i in range(attack_idx, n):
        if abs(samples[i]) < thr:
            decay_t = (i - attack_idx) / sr
            break
    centroid = 0.0
    if rms > 1e-9:
        flux = sum(abs(samples[i] - samples[i - 1]) for i in range(1, n)) / max(1, n - 1)
        centroid = min(1.0, flux * 8.0)
    return (
        rms,
        pk,
        zcr,
        attack_t,
        decay_t,
        centroid,
        dc_offset(samples),
        sum(samples) / n,
    )


def feature_distance(a: tuple[float, ...], b: tuple[float, ...]) -> float:
    return math.sqrt(sum((x - y) ** 2 for x, y in zip(a, b)))
