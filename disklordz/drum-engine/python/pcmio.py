"""PCM helpers for kit prep. Stdlib only so a fresh clone can run without pip."""

from __future__ import annotations

import math
import struct
import wave
from pathlib import Path


def read_wav(path: Path) -> tuple[list[float], int, int]:
    with wave.open(str(path), "rb") as handle:
        channels = handle.getnchannels()
        width = handle.getsampwidth()
        rate = handle.getframerate()
        frames = handle.getnframes()
        raw = handle.readframes(frames)
    if width not in (2, 3, 4):
        raise ValueError(f"{path.name}: unsupported sample width {width}")
    samples: list[float] = []
    step = width * channels
    for offset in range(0, len(raw), step):
        mixed = 0.0
        for channel in range(channels):
            start = offset + channel * width
            chunk = raw[start : start + width]
            if width == 2:
                value = struct.unpack_from("<h", chunk)[0] / 32768.0
            elif width == 3:
                integer = int.from_bytes(chunk, "little", signed=False)
                if integer & 0x800000:
                    integer -= 0x1000000
                value = integer / 8388608.0
            else:
                value = struct.unpack_from("<i", chunk)[0] / 2147483648.0
            mixed += value
        samples.append(mixed / channels)
    return samples, rate, channels


def write_wav24(path: Path, samples: list[float], rate: int) -> None:
    path.parent.mkdir(parents=True, exist_ok=True)
    frames = bytearray()
    for sample in samples:
        clamped = max(-1.0, min(1.0, sample))
        integer = int(round(clamped * 8388607.0))
        integer = max(-8388608, min(8388607, integer))
        if integer < 0:
            integer += 0x1000000
        frames.extend(integer.to_bytes(3, "little"))
    with wave.open(str(path), "wb") as handle:
        handle.setnchannels(1)
        handle.setsampwidth(3)
        handle.setframerate(rate)
        handle.writeframes(frames)


def resample_linear(samples: list[float], source_rate: int, target_rate: int) -> list[float]:
    if source_rate == target_rate or len(samples) < 2:
        return list(samples)
    ratio = source_rate / target_rate
    length = max(1, int(len(samples) * target_rate / source_rate))
    output: list[float] = []
    for index in range(length):
        position = index * ratio
        left = int(position)
        frac = position - left
        right = min(left + 1, len(samples) - 1)
        output.append(samples[left] * (1.0 - frac) + samples[right] * frac)
    return output


def trim_leading_zero_cross(samples: list[float], threshold: float = 1.0e-3) -> list[float]:
    start = 0
    while start < len(samples) and abs(samples[start]) < threshold:
        start += 1
    if start == 0 or start >= len(samples):
        return list(samples)
    cross = start
    while cross > 0 and samples[cross - 1] * samples[cross] > 0.0:
        cross -= 1
    return samples[cross:]


def peak_normalize(samples: list[float], db: float) -> list[float]:
    peak = max((abs(sample) for sample in samples), default=0.0)
    if peak < 1.0e-8:
        return list(samples)
    gain = (10.0 ** (db / 20.0)) / peak
    return [sample * gain for sample in samples]


def soft_clip(samples: list[float], drive: float = 1.5) -> list[float]:
    normalise = math.tanh(drive)
    return [math.tanh(sample * drive) / normalise for sample in samples]


def peak_db(samples: list[float]) -> float:
    peak = max((abs(sample) for sample in samples), default=0.0)
    if peak < 1.0e-12:
        return -120.0
    return 20.0 * math.log10(peak)
