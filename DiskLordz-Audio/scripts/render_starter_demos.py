#!/usr/bin/env python3
"""Write starter progression MIDI demos (matches ProgressionLibrary.cpp presets)."""

from __future__ import annotations

import struct
import sys
from pathlib import Path

# Minimal SMF writer (type 0, one track).
PRESETS = [
    ("dark_trap_f_minor", 5, [1, 6, 3, 7], [3, 4, 4, 4]),
    ("drift_phonk_gsharp_minor", 8, [1, 4, 7, 6], [3, 3, 4, 4]),
    ("memphis_d_minor", 2, [1, 7, 4, 1], [3, 4, 3, 3]),
]

NATURAL_MINOR = [0, 2, 3, 5, 7, 8, 10]
MAJOR_INTERVALS = [0, 4, 7]


def degree_to_pc(root: int, degree: int) -> int:
    return (root + NATURAL_MINOR[degree - 1]) % 12


def chord_notes(root_pc: int, degree: int, quality: int) -> list[int]:
    bass = 52 + degree_to_pc(root_pc, degree)
    if quality == 3:
        return [bass, bass + 3, bass + 7]
    return [bass, bass + MAJOR_INTERVALS[1], bass + MAJOR_INTERVALS[2]]


def vlq(value: int) -> bytes:
    buffer = [value & 0x7F]
    value >>= 7
    while value:
        buffer.insert(0, (value & 0x7F) | 0x80)
        value >>= 7
    return bytes(buffer)


def write_midi(path: Path, root: int, degrees: list[int], qualities: list[int], bpm: int = 140) -> None:
    tpq = 480
    us_per_q = int(60_000_000 / bpm)
    events = bytearray()

    def emit(delta: int, msg: bytes) -> None:
        events.extend(vlq(delta))
        events.extend(msg)

    emit(0, bytes([0xFF, 0x51, 0x03]) + us_per_q.to_bytes(3, "big"))
    emit(0, bytes([0xFF, 0x58, 0x04, 0x04, 0x02, 0x18, 0x08]))

    ticks_per_bar = tpq * 4
    t = 0
    for degree, quality in zip(degrees, qualities, strict=True):
        notes = chord_notes(root, degree, quality)
        for n in notes:
            emit(0 if t == 0 else 0, bytes([0x90, n, 100]))
        emit(ticks_per_bar, bytes([0x80, notes[0], 0]))
        for n in notes[1:]:
            emit(0, bytes([0x80, n, 0]))
        t += ticks_per_bar

    emit(0, bytes([0xFF, 0x2F, 0x00]))

    header = b"MThd" + struct.pack(">IHHH", 6, 0, 1, tpq)
    track = b"MTrk" + struct.pack(">I", len(events)) + events
    path.write_bytes(header + track)


def main() -> int:
    out = Path(sys.argv[1] if len(sys.argv) > 1 else "/opt/cursor/artifacts/chord-engine-demos")
    out.mkdir(parents=True, exist_ok=True)
    for name, root, degrees, qualities in PRESETS:
        write_midi(out / f"{name}.mid", root, degrees, qualities)
        print(f"Wrote {out / f'{name}.mid'}")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())