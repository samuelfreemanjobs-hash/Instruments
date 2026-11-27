"""Minimal Standard MIDI File Type 0 writer (no mido dependency)."""

from __future__ import annotations

import struct

from drum_pattern_intel import GM, PatternNote

PPQ = 480


def _vlq(value: int) -> bytes:
    if value <= 0:
        return bytes([0])
    out: list[int] = []
    v = value
    while v > 0:
        out.insert(0, v & 0x7F)
        v >>= 7
    for i in range(len(out) - 1):
        out[i] |= 0x80
    return bytes(out)


def write_pattern_smf(
    path,
    notes: list[PatternNote],
    *,
    bpm: float,
    track_name: str = "Drums",
) -> None:
    us_per_qn = int(60_000_000 / max(30.0, bpm))
    step_ticks = PPQ // 4
    by_tick: dict[int, list[PatternNote]] = {}
    for n in notes:
        tick = n.step * step_ticks
        by_tick.setdefault(tick, []).append(n)

    track = bytearray()
    track.extend(_vlq(0))
    track.extend(bytes([0xFF, 0x51, 0x03]))
    track.extend(struct.pack(">I", us_per_qn)[1:4])
    track.extend(_vlq(0))
    track.extend(bytes([0xFF, 0x03, len(track_name)]))
    track.extend(track_name.encode("latin-1", errors="replace"))

    last_tick = 0
    for tick in sorted(by_tick.keys()):
        delta = tick - last_tick
        for n in by_tick[tick]:
            track.extend(_vlq(delta))
            track.extend(bytes([0x99, GM[n.instrument], min(127, n.velocity)]))
            delta = 0
            off = max(step_ticks - 20, 40)
            track.extend(_vlq(off))
            track.extend(bytes([0x89, GM[n.instrument], 0]))
        last_tick = tick + step_ticks

    track.extend(_vlq(step_ticks))
    track.extend(bytes([0xFF, 0x2F, 0x00]))

    def chunk(tag: bytes, data: bytes) -> bytes:
        return tag + struct.pack(">I", len(data)) + data

    header = struct.pack(">4sIHHH", b"MThd", 6, 0, 1, PPQ)
    path.parent.mkdir(parents=True, exist_ok=True)
    path.write_bytes(header + chunk(b"MTrk", bytes(track)))
