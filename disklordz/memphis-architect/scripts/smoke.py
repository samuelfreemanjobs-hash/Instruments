#!/usr/bin/env python3
"""Smoke test: render kick + snare WAVs and validate RIFF headers."""

from __future__ import annotations

import struct
import sys
import tempfile
from pathlib import Path

ROOT = Path(__file__).resolve().parent
sys.path.insert(0, str(ROOT))

from generate_kick import generate_memphis_phonk_kick  # noqa: E402
from generate_snare import generate_memphis_phonk_snare  # noqa: E402


def read_wav_header(path: Path) -> tuple[int, int, int]:
    data = path.read_bytes()[:44]
    if data[:4] != b"RIFF" or data[8:12] != b"WAVE":
        raise ValueError(f"bad RIFF: {path}")
    channels = struct.unpack_from("<H", data, 22)[0]
    sample_rate = struct.unpack_from("<I", data, 24)[0]
    bits = struct.unpack_from("<H", data, 34)[0]
    return channels, sample_rate, bits


def main() -> int:
    with tempfile.TemporaryDirectory() as td:
        t = Path(td)
        kick = generate_memphis_phonk_kick(t / "kick.wav")
        snare = generate_memphis_phonk_snare(t / "snare.wav", seed=660)

        for p in (kick, snare):
            if p.stat().st_size < 1000:
                print(f"FAIL: {p} too small")
                return 1
            ch, sr, bits = read_wav_header(p)
            if (ch, sr, bits) != (1, 44100, 24):
                print(f"FAIL: {p} header {ch=} {sr=} {bits=}")
                return 1

    print("memphis-architect smoke: ok")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
