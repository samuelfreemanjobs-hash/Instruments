#!/usr/bin/env python3
"""Spectral + envelope metrics for Trap Lord headless WAV QA."""

from __future__ import annotations

import csv
import math
import struct
import wave
from dataclasses import dataclass
from pathlib import Path

import numpy as np

SAMPLE_RATE_DEFAULT = 44100


def read_wav_mono(path: Path) -> tuple[np.ndarray, int]:
    path = Path(path)
    with wave.open(str(path), "rb") as wf:
        ch = wf.getnchannels()
        sw = wf.getsampwidth()
        sr = wf.getframerate()
        n = wf.getnframes()
        raw = wf.readframes(n)

    if sw == 3:
        samples = []
        step = ch * 3
        for i in range(0, len(raw), step):
            frame = []
            for c in range(ch):
                off = i + c * 3
                b = raw[off : off + 3]
                v = struct.unpack("<i", b + b"\x00")[0]
                if v >= 1 << 23:
                    v -= 1 << 24
                frame.append(v / float(1 << 23))
            samples.append(float(np.mean(frame)))
        x = np.array(samples, dtype=np.float64)
    elif sw == 2:
        ints = np.frombuffer(raw, dtype=np.int16).astype(np.float64)
        if ch > 1:
            ints = ints.reshape(-1, ch).mean(axis=1)
        x = ints / 32768.0
    elif sw == 4:
        floats = np.frombuffer(raw, dtype="<f4")
        if ch > 1:
            floats = floats.reshape(-1, ch).mean(axis=1)
        x = floats.astype(np.float64)
    else:
        raise ValueError(f"Unsupported sample width {sw} in {path}")

    return x, sr


@dataclass
class AudioMetrics:
    path: str
    peak: float
    rms: float
    dc_offset: float
    tail_rms_db: float
    centroid_hz: float
    fundamental_hz: float
    fundamental_ratio: float
    sub30_energy_ratio: float = 0.0
    category: str = ""


def envelope_rms(x: np.ndarray, win: int) -> np.ndarray:
    if win < 8:
        win = 8
    kernel = np.ones(win, dtype=np.float64) / win
    return np.sqrt(np.convolve(x * x, kernel, mode="same"))


def spectral_features(x: np.ndarray, sr: int) -> tuple[float, float, float]:
    if len(x) < 512:
        return 0.0, 0.0, 0.0
    w = np.hanning(len(x))
    spec = np.abs(np.fft.rfft(x * w))
    freqs = np.fft.rfftfreq(len(x), 1.0 / sr)
    power = spec * spec
    total = float(np.sum(power))
    if total <= 1e-20:
        return 0.0, 0.0, 0.0
    centroid = float(np.sum(freqs * power) / total)
    band = (freqs >= 70.0) & (freqs <= 900.0)
    if not np.any(band):
        band = freqs > 1.0
    band_spec = spec.copy()
    band_spec[~band] = 0.0
    peak_bin = int(np.argmax(band_spec))
    fundamental = float(freqs[peak_bin]) if peak_bin < len(freqs) else 0.0
    fundamental_ratio = float(spec[peak_bin] / max(np.max(spec), 1e-12))
    return centroid, fundamental, fundamental_ratio


def analyze_file(path: Path, category: str = "", tail_fraction: float = 0.12) -> AudioMetrics:
    x, sr = read_wav_mono(path)
    if not np.all(np.isfinite(x)):
        raise ValueError(f"non-finite samples in {path}")

    peak = float(np.max(np.abs(x)))
    rms = float(np.sqrt(np.mean(x * x)))
    dc = float(np.mean(x))
    tail_start = int(len(x) * (1.0 - tail_fraction))
    tail = x[tail_start:]
    tail_rms = float(np.sqrt(np.mean(tail * tail)) if len(tail) else 0.0)
    tail_rms_db = 20.0 * math.log10(max(tail_rms, 1e-12))

    centroid, fundamental, fund_ratio = spectral_features(x, sr)

    w = np.hanning(len(x))
    spec = np.abs(np.fft.rfft(x * w))
    freqs = np.fft.rfftfreq(len(x), 1.0 / sr)
    power = spec * spec
    total_p = float(np.sum(power)) + 1e-20
    sub30 = float(np.sum(power[freqs < 30.0]))
    sub30_ratio = sub30 / total_p

    return AudioMetrics(
        path=path.name,
        peak=peak,
        rms=rms,
        dc_offset=dc,
        tail_rms_db=tail_rms_db,
        centroid_hz=centroid,
        fundamental_hz=fundamental,
        fundamental_ratio=fund_ratio,
        sub30_energy_ratio=sub30_ratio,
        category=category,
    )


def assert_wav_format(path: Path, sample_rate: int = 44100, sample_width_bytes: int = 3) -> None:
    path = Path(path)
    with wave.open(str(path), "rb") as wf:
        if wf.getframerate() != sample_rate:
            raise AssertionError(f"{path.name}: sample rate {wf.getframerate()} != {sample_rate}")
        if wf.getsampwidth() != sample_width_bytes:
            raise AssertionError(f"{path.name}: width {wf.getsampwidth()} != {sample_width_bytes}")


def feature_vector(x: np.ndarray, sr: int) -> np.ndarray:
    env = envelope_rms(x, max(1, sr // 100))
    env = env[:: max(1, len(env) // 256)]
    w = np.hanning(len(x))
    spec = np.abs(np.fft.rfft(x * w))
    spec = spec[:512]
    vec = np.concatenate([spec, env])
    vec = vec / (np.linalg.norm(vec) + 1e-12)
    return vec


def correlation(a: Path, b: Path) -> float:
    xa, sra = read_wav_mono(a)
    xb, srb = read_wav_mono(b)
    if sra != srb:
        raise ValueError("sample rate mismatch")
    n = min(len(xa), len(xb))
    fa = feature_vector(xa[:n], sra)
    fb = feature_vector(xb[:n], srb)
    return float(np.dot(fa, fb))


def write_metrics_csv(rows: list[AudioMetrics], out: Path) -> None:
    out = Path(out)
    with out.open("w", newline="", encoding="utf-8") as f:
        w = csv.writer(f)
        w.writerow(
            [
                "file",
                "category",
                "peak",
                "rms",
                "dc_offset",
                "tail_rms_dbfs",
                "centroid_hz",
                "fundamental_hz",
                "fundamental_ratio",
                "sub30_energy_ratio",
            ]
        )
        for m in rows:
            w.writerow(
                [
                    m.path,
                    m.category,
                    f"{m.peak:.6f}",
                    f"{m.rms:.6f}",
                    f"{m.dc_offset:.6f}",
                    f"{m.tail_rms_db:.2f}",
                    f"{m.centroid_hz:.1f}",
                    f"{m.fundamental_hz:.1f}",
                    f"{m.fundamental_ratio:.4f}",
                    f"{m.sub30_energy_ratio:.4f}",
                ]
            )
