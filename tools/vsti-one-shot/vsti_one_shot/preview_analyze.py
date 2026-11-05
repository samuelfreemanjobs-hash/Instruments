"""Analyze short preview WAVs for preset matcher (no LLM)."""

from __future__ import annotations

import math
import struct
import wave
from dataclasses import dataclass
from pathlib import Path


@dataclass(frozen=True)
class PreviewMetrics:
    duration_sec: float
    peak: float
    attack_ms: float
    sustain_ratio: float  # energy after 80ms / total (higher = longer tail)
    brightness: float  # mean abs first difference, normalized


def read_mono_samples(path: Path) -> tuple[list[float], int]:
    with wave.open(str(path), "rb") as w:
        nch = w.getnchannels()
        rate = w.getframerate()
        width = w.getsampwidth()
        nframes = w.getnframes()
        raw = w.readframes(nframes)

    if width != 2:
        raise ValueError(f"expected 16-bit WAV, got width={width}")

    count = len(raw) // 2
    samples = struct.unpack(f"<{count}h", raw)
    if nch == 1:
        mono = [s / 32768.0 for s in samples]
    else:
        mono = []
        for i in range(0, len(samples), nch):
            mono.append(sum(samples[i : i + nch]) / (32768.0 * nch))
    return mono, rate


def analyze_preview_wav(path: Path) -> PreviewMetrics:
    samples, rate = read_mono_samples(path)
    if not samples:
        return PreviewMetrics(0.0, 0.0, 0.0, 0.0, 0.0)

    duration_sec = len(samples) / rate
    peak = max(abs(s) for s in samples) or 1e-9

    attack_idx = 0
    thresh = peak * 0.25
    for i, s in enumerate(samples):
        if abs(s) >= thresh:
            attack_idx = i
            break
    attack_ms = 1000.0 * attack_idx / rate

    total_e = sum(s * s for s in samples)
    post = int(rate * 0.08)
    tail_e = sum(s * s for s in samples[post:]) if post < len(samples) else 0.0
    sustain_ratio = tail_e / (total_e or 1.0)

    diffs = [abs(samples[i] - samples[i - 1]) for i in range(1, len(samples))]
    brightness = (sum(diffs) / len(diffs)) / peak if diffs else 0.0

    return PreviewMetrics(
        duration_sec=duration_sec,
        peak=peak,
        attack_ms=attack_ms,
        sustain_ratio=sustain_ratio,
        brightness=brightness,
    )


def score_metrics_for_kind(metrics: PreviewMetrics, kind: str) -> float:
    """Higher = better fit for recipe slot kind."""
    if metrics.peak < 1e-6:
        return -100.0

    kind = kind.lower()
    score = 0.0

    if kind == "stab":
        score += max(0.0, 12.0 - metrics.attack_ms * 0.05)
        score += max(0.0, 1.0 - metrics.sustain_ratio) * 20.0
    elif kind == "key":
        score += max(0.0, metrics.sustain_ratio * 15.0)
        score += min(metrics.attack_ms * 0.02, 4.0)
    elif kind == "bell":
        score += max(0.0, 8.0 - abs(metrics.sustain_ratio - 0.45) * 20.0)
        score += metrics.brightness * 8.0
    elif kind == "pad_hit":
        score += metrics.sustain_ratio * 18.0
        score += max(0.0, 6.0 - metrics.brightness * 3.0)
    else:
        score += metrics.sustain_ratio * 10.0

    score += min(metrics.duration_sec, 2.0) * 2.0
    return score
