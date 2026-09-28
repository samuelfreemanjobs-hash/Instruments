"""Memphis phonk kit — loop → labeled one-shot slices."""

from __future__ import annotations

from dataclasses import dataclass
from enum import StrEnum
from pathlib import Path

import numpy as np

try:
    import librosa
except ImportError:  # pragma: no cover
    librosa = None


class HitLabel(StrEnum):
    KICK = "kick"
    SNARE = "snare"
    HAT = "hat"
    COWBELL = "cowbell"
    PERC = "perc"


@dataclass(frozen=True)
class SliceSpec:
    start: int
    end: int
    label: HitLabel
    confidence: float


def _sub_ratio(mono: np.ndarray, sr: int, fmax: float = 80.0) -> float:
    spec = np.abs(librosa.stft(mono, n_fft=2048, hop_length=512))
    freqs = librosa.fft_frequencies(sr=sr, n_fft=2048)
    power = spec**2
    total = float(power.sum()) + 1e-12
    sub = float(power[freqs <= fmax].sum())
    return sub / total


def _centroid_hz(mono: np.ndarray, sr: int) -> float:
    return float(librosa.feature.spectral_centroid(y=mono, sr=sr).mean())


def _zcr(mono: np.ndarray) -> float:
    return float(librosa.feature.zero_crossing_rate(mono).mean())


def classify_hit(mono: np.ndarray, sr: int) -> tuple[HitLabel, float]:
    """
    Heuristic label from Memphis fingerprint rules (sub%, centroid, ZCR).

    Not a replacement for human QC — good enough to bootstrap folders for DDSP.
    """
    sub = _sub_ratio(mono, sr)
    cent = _centroid_hz(mono, sr)
    zcr = _zcr(mono)
    dur_ms = 1000.0 * len(mono) / sr

    # 808 / kick: low centroid (fundamental-heavy) even when harmonics spread sub%
    if cent < 1000 and (sub >= 0.18 or dur_ms >= 80):
        return HitLabel.KICK, min(1.0, 0.5 + sub)
    if cent >= 4200 and sub < 0.35:
        return HitLabel.COWBELL, min(1.0, (cent - 3500) / 2000)
    if zcr >= 0.12 and dur_ms < 180 and cent > 2500:
        return HitLabel.HAT, min(1.0, zcr * 3)
    if 1500 <= cent <= 4200 and 0.08 <= sub <= 0.45:
        return HitLabel.SNARE, 0.55
    return HitLabel.PERC, 0.4


def detect_onset_frames(
    mono: np.ndarray,
    sr: int,
    *,
    hop_length: int = 512,
    backtrack: bool = True,
) -> np.ndarray:
    if librosa is None:
        raise ImportError("librosa is required for onset slicing")
    return librosa.onset.onset_detect(
        y=mono,
        sr=sr,
        hop_length=hop_length,
        backtrack=backtrack,
        units="samples",
    )


def slice_loop_to_hits(
    mono: np.ndarray,
    sr: int,
    *,
    pre_ms: float = 8.0,
    post_ms: float = 420.0,
    min_gap_ms: float = 90.0,
    max_hits: int = 128,
) -> list[tuple[np.ndarray, SliceSpec]]:
    """
    Onset-detect a loop, slice one-shots, peak-normalize each slice, classify label.
    """
    if librosa is None:
        raise ImportError("librosa is required for onset slicing")
    if mono.ndim != 1:
        mono = mono.reshape(-1)
    onsets = detect_onset_frames(mono, sr)
    if onsets.size == 0:
        return []

    pre = int(sr * pre_ms / 1000.0)
    post = int(sr * post_ms / 1000.0)
    min_gap = int(sr * min_gap_ms / 1000.0)

    merged: list[int] = [int(onsets[0])]
    for o in onsets[1:]:
        if int(o) - merged[-1] >= min_gap:
            merged.append(int(o))
        if len(merged) >= max_hits:
            break

    out: list[tuple[np.ndarray, SliceSpec]] = []
    for start in merged:
        s0 = max(0, start - pre)
        s1 = min(len(mono), start + post)
        clip = mono[s0:s1].astype(np.float64, copy=True)
        peak = np.max(np.abs(clip))
        if peak < 1e-5:
            continue
        clip /= peak
        label, conf = classify_hit(clip, sr)
        out.append((clip, SliceSpec(start=s0, end=s1, label=label, confidence=conf)))
    return out


def load_mono_wav(path: Path, target_sr: int = 44100) -> tuple[np.ndarray, int]:
    if librosa is None:
        raise ImportError("librosa is required")
    y, sr = librosa.load(path, sr=target_sr, mono=True)
    return y, sr


def write_wav_24(path: Path, mono: np.ndarray, sr: int) -> None:
    import soundfile as sf

    path.parent.mkdir(parents=True, exist_ok=True)
    sf.write(path, mono, sr, subtype="PCM_24")
