"""WAV IO helpers for sound-factory."""

from __future__ import annotations

import hashlib
import struct
import wave
from pathlib import Path

SAMPLE_RATE = 44100


def write_mono_wav(path: Path, samples: list[float], target_peak: float = 0.89) -> str:
    peak = max((abs(s) for s in samples), default=0.0)
    scale = (target_peak / peak) if peak > 1e-9 else 1.0
    path.parent.mkdir(parents=True, exist_ok=True)
    with wave.open(str(path), "w") as w:
        w.setnchannels(1)
        w.setsampwidth(2)
        w.setframerate(SAMPLE_RATE)
        frames = b"".join(
            struct.pack("<h", int(max(-1.0, min(1.0, s * scale)) * 32767)) for s in samples
        )
        w.writeframes(frames)
    return hashlib.sha256(path.read_bytes()).hexdigest()


def read_mono_wav(path: Path) -> tuple[list[float], int]:
    with wave.open(str(path), "r") as w:
        rate = w.getframerate()
        n = w.getnframes()
        raw = w.readframes(n)
        width = w.getsampwidth()
        if width != 2:
            raise ValueError(f"unsupported sample width: {width}")
        samples = []
        for i in range(0, len(raw), 2):
            samples.append(struct.unpack("<h", raw[i : i + 2])[0] / 32768.0)
        return samples, rate
