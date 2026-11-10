"""Prepare mono PCM for LIVEN Lofi-12 sampling constraints."""

from __future__ import annotations

import math
import struct
import wave
from pathlib import Path

LOFI_RATE_24K = 24000
LOFI_RATE_12K = 12000
MAX_SEC_24K = 2.0
MAX_SEC_12K = 4.0


def _linear_resample(samples: list[float], src_rate: int, dst_rate: int) -> list[float]:
    if src_rate == dst_rate or not samples:
        return list(samples)
    ratio = dst_rate / src_rate
    out_len = max(1, int(len(samples) * ratio))
    out: list[float] = []
    for i in range(out_len):
        src_pos = i / ratio
        i0 = int(src_pos)
        i1 = min(i0 + 1, len(samples) - 1)
        frac = src_pos - i0
        out.append(samples[i0] * (1.0 - frac) + samples[i1] * frac)
    return out


def soft_clip(x: float, drive: float = 1.35) -> float:
    y = x * drive
    return math.tanh(y) / math.tanh(drive)


def apply_grit(samples: list[float], amount: float) -> list[float]:
    if amount <= 0:
        return samples
    drive = 1.0 + amount * 2.2
    out = [soft_clip(s, drive) for s in samples]
    # light decimation shimmer for phonk
    if amount > 0.35:
        step = 2 if amount < 0.7 else 3
        for i in range(0, len(out), step):
            hold = out[i]
            for j in range(i, min(i + step, len(out))):
                out[j] = hold
    return out


def trim_tail(samples: list[float], max_seconds: float, sample_rate: int) -> list[float]:
    max_len = int(sample_rate * max_seconds)
    if len(samples) <= max_len:
        return samples
    return samples[:max_len]


def normalize(samples: list[float], peak: float = 0.89) -> list[float]:
    m = max(abs(s) for s in samples) or 1.0
    scale = peak / m
    return [s * scale for s in samples]


def prepare_for_lofi12(
    samples: list[float],
    *,
    src_rate: int,
    dst_rate: int = LOFI_RATE_24K,
    grit: float = 0.0,
) -> list[float]:
    max_sec = MAX_SEC_12K if dst_rate == LOFI_RATE_12K else MAX_SEC_24K
    resampled = _linear_resample(samples, src_rate, dst_rate)
    resampled = trim_tail(resampled, max_sec, dst_rate)
    resampled = apply_grit(resampled, grit)
    return normalize(resampled)


def write_wav_mono(path: Path, samples: list[float], sample_rate: int) -> None:
    path.parent.mkdir(parents=True, exist_ok=True)
    with wave.open(str(path), "w") as w:
        w.setnchannels(1)
        w.setsampwidth(2)
        w.setframerate(sample_rate)
        frames = b"".join(
            struct.pack("<h", int(max(-1.0, min(1.0, s)) * 32767)) for s in samples
        )
        w.writeframes(frames)


def read_wav_mono(path: Path) -> tuple[list[float], int]:
    with wave.open(str(path), "rb") as w:
        if w.getnchannels() != 1:
            raise ValueError(f"expected mono WAV: {path}")
        rate = w.getframerate()
        raw = w.readframes(w.getnframes())
        width = w.getsampwidth()
    if width != 2:
        raise ValueError(f"expected 16-bit WAV: {path}")
    samples = [struct.unpack("<h", raw[i : i + 2])[0] / 32768.0 for i in range(0, len(raw), 2)]
    return samples, rate
