#!/usr/bin/env python3
"""Post-render FX lanes for Black Box kits: pitch, tape, cassette, lofi.

Mirrors Converter Night / elite parallel banks — applied at package time so
RAW cores stay clean and process siblings are honest derivatives.
"""

from __future__ import annotations

import math
import sys
import wave
import zipfile
from pathlib import Path

import numpy as np

ROOT = Path(__file__).resolve().parents[2]
for p in (str(ROOT), str(ROOT / "scripts")):
    if p not in sys.path:
        sys.path.insert(0, p)

from scripts.memphis_oneshots.phonk_kick import write_wav_16bit_mono  # noqa: E402

SR = 44100


def read_wav_mono(path: Path) -> np.ndarray:
    with wave.open(str(path), "rb") as w:
        assert w.getnchannels() == 1
        assert w.getsampwidth() == 2
        assert w.getframerate() == SR
        raw = w.readframes(w.getnframes())
    return np.frombuffer(raw, dtype=np.int16).astype(np.float64) / 32767.0


def write_wav_mono(path: Path, samples: np.ndarray) -> None:
    write_wav_16bit_mono(path, normalize(samples))


def zip_tree(src: Path, zip_path: Path, arc_root: str) -> None:
    """Shared zip helper for era + Dirty Circuits packs."""
    zip_path.parent.mkdir(parents=True, exist_ok=True)
    with zipfile.ZipFile(zip_path, "w", compression=zipfile.ZIP_DEFLATED) as zf:
        for path in sorted(src.rglob("*")):
            if path.is_file():
                zf.write(path, arcname=str(Path(arc_root) / path.relative_to(src)))


def normalize(x: np.ndarray, ceiling_db: float = -0.1) -> np.ndarray:
    peak = float(np.max(np.abs(x))) or 1.0
    return ((x / peak) * (10.0 ** (ceiling_db / 20.0))).astype(np.float32)


def one_pole_lpf(x: np.ndarray, cutoff_hz: float, sample_rate: int = SR) -> np.ndarray:
    alpha = math.exp(-2.0 * math.pi * cutoff_hz / sample_rate)
    y = np.zeros_like(x)
    for i in range(1, len(x)):
        y[i] = (1 - alpha) * x[i] + alpha * y[i - 1]
    return y


def pitch_shift(buf: np.ndarray, semitones: float, sample_rate: int = SR) -> np.ndarray:
    """Cheap rate-change pitch (no formant preserve) — intentional machine trash."""
    if semitones == 0:
        return buf.astype(np.float64)
    ratio = 2.0 ** (semitones / 12.0)
    n = len(buf)
    new_n = max(1, int(round(n / ratio)))
    src = np.linspace(0.0, n - 1, new_n)
    return np.interp(src, np.arange(n), buf.astype(np.float64))


def bitcrush(buf: np.ndarray, bit_depth: int = 10, hold_sr: float = 22050.0, sample_rate: int = SR) -> np.ndarray:
    x = buf.astype(np.float64)
    step = sample_rate / hold_sr
    idx = (np.floor(np.arange(len(x)) / step) * step).astype(int)
    idx = np.clip(idx, 0, len(x) - 1)
    held = x[idx]
    q = 2 ** (bit_depth - 1)
    return np.round(held * q) / q


def cassette_fx(
    buf: np.ndarray,
    *,
    drive_db: float = 4.0,
    tape_lpf_hz: float = 10500.0,
    wow_depth_ms: float = 0.35,
    wow_hz: float = 0.55,
    flutter_depth_ms: float = 0.08,
    flutter_hz: float = 6.5,
    hiss_db: float = -42.0,
    seed: int = 96,
    sample_rate: int = SR,
) -> np.ndarray:
    """Cassette desk: soft clip → dark LPF → wow/flutter → light hiss."""
    x = buf.astype(np.float64)
    driven = np.tanh(x * (10.0 ** (drive_db / 20.0)))
    filtered = one_pole_lpf(driven, tape_lpf_hz, sample_rate)

    # Fractional delay wow/flutter (circular buffer)
    max_delay = int(sample_rate * 0.005) + 8
    delay_buf = np.zeros(max_delay, dtype=np.float64)
    write_pos = 0
    out = np.zeros_like(filtered)
    for i, s in enumerate(filtered):
        t = i / sample_rate
        delay_ms = (
            wow_depth_ms * (0.5 + 0.5 * math.sin(2 * math.pi * wow_hz * t))
            + flutter_depth_ms * math.sin(2 * math.pi * flutter_hz * t)
        )
        delay_samp = max(1.0, delay_ms * 0.001 * sample_rate)
        read_pos = (write_pos - delay_samp) % max_delay
        i0 = int(math.floor(read_pos)) % max_delay
        i1 = (i0 + 1) % max_delay
        frac = read_pos - math.floor(read_pos)
        delayed = delay_buf[i0] * (1 - frac) + delay_buf[i1] * frac
        out[i] = delayed
        delay_buf[write_pos] = s
        write_pos = (write_pos + 1) % max_delay

    rng = np.random.default_rng(seed)
    hiss = rng.normal(0.0, 10.0 ** (hiss_db / 20.0), len(out))
    hiss = one_pole_lpf(hiss, 6000.0, sample_rate)
    return out + hiss


def process_lane(buf: np.ndarray, lane: str, *, seed: int = 0, sample_rate: int = SR) -> np.ndarray:
    """RAW / TAPE / LOFI / CASSETTE parallel siblings of the same hit."""
    x = buf.astype(np.float64)
    if lane == "RAW":
        out = x
    elif lane == "TAPE":
        driven = np.tanh(x * (10.0 ** (3.0 / 20.0)))
        out = one_pole_lpf(driven, 10500.0, sample_rate)
    elif lane == "LOFI":
        crushed = bitcrush(x, bit_depth=10, hold_sr=22050.0, sample_rate=sample_rate)
        out = np.tanh(crushed * (10.0 ** (5.0 / 20.0)))
    elif lane == "CASSETTE":
        out = cassette_fx(x, seed=96 + seed)
    else:
        raise ValueError(f"unknown lane: {lane}")
    return normalize(out)
