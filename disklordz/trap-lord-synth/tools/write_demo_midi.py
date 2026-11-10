#!/usr/bin/env python3
"""Write original F harmonic minor demo (4 bars, 140 BPM, eighth notes)."""

from __future__ import annotations

import struct
from pathlib import Path

PPQ = 480
BPM = 140
MICROSEC_PER_Q = int(60_000_000 / BPM)
EIGHTH = PPQ // 2

# F harmonic minor fragment — original melody, neutral title only.
NOTES = [
    65,
    68,
    72,
    76,  # bar 1
    73,
    72,
    68,
    67,  # bar 2
    65,
    72,
    76,
    77,  # bar 3
    76,
    72,
    68,
    65,  # bar 4
]


def vlq(value: int) -> bytes:
    if value == 0:
        return b"\x00"
    buf = []
    while value > 0:
        buf.insert(0, value & 0x7F)
        value >>= 7
    for i in range(len(buf) - 1):
        buf[i] |= 0x80
    return bytes(buf)


def build_track() -> bytes:
    events: list[tuple[int, bytes]] = []
    tick = 0
    events.append((0, bytes([0x00, 0xFF, 0x51, 0x03]) + struct.pack(">I", MICROSEC_PER_Q)[1:]))
  # actually tempo meta: FF 51 03 tttttt
    events[0] = (0, bytes([0x00, 0xFF, 0x51, 0x03]) + MICROSEC_PER_Q.to_bytes(3, "big"))

    for note in NOTES:
        events.append((tick, bytes([0x90, note, 100])))
        events.append((tick + EIGHTH, bytes([0x80, note, 0])))
        tick += EIGHTH

    events.append((tick, bytes([0xFF, 0x2F, 0x00])))
    events.sort(key=lambda e: e[0])

    out = bytearray()
    last = 0
    for abs_tick, payload in events:
        delta = abs_tick - last
        last = abs_tick
        out.extend(vlq(delta))
        out.extend(payload)
    return bytes(out)


def write_smf(path: Path) -> None:
    track = build_track()
    header = b"MThd" + struct.pack(">IHHH", 6, 0, 1, PPQ)
    body = b"MTrk" + struct.pack(">I", len(track)) + track
    path.parent.mkdir(parents=True, exist_ok=True)
    path.write_bytes(header + body)


if __name__ == "__main__":
    out = Path(__file__).resolve().parents[1] / "assets" / "demo_f_harmonic_minor_140bpm.mid"
    write_smf(out)
    print(f"Wrote {out}")
