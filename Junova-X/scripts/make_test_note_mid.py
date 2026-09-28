#!/usr/bin/env python3
"""Minimal Type-0 MIDI: one note on, hold, note off (for reference plugin renders)."""

from __future__ import annotations

import struct
import sys


def vlq(n: int) -> bytes:
    buf: list[int] = [n & 0x7F]
    n >>= 7
    while n:
        buf.insert(0, (n & 0x7F) | 0x80)
        n >>= 7
    return bytes(buf)


def write_mid(path: str, note: int, velocity: int, hold_ticks: int, tpq: int = 480) -> None:
    # Track: delta, note on, delta hold, note off, end
    track = b""
    track += vlq(0) + bytes([0x90, note & 0x7F, velocity & 0x7F])
    track += vlq(hold_ticks) + bytes([0x80, note & 0x7F, 0])
    track += vlq(0) + b"\xFF\x2F\x00"

    header = b"MThd" + struct.pack(">IHHH", 6, 0, 1, tpq)
    trk = b"MTrk" + struct.pack(">I", len(track)) + track
    with open(path, "wb") as f:
        f.write(header + trk)


def main() -> None:
    note = int(sys.argv[1]) if len(sys.argv) > 1 else 60
    seconds = float(sys.argv[2]) if len(sys.argv) > 2 else 2.0
    out = sys.argv[3] if len(sys.argv) > 3 else "/tmp/test-note.mid"
    tpq = 480
    bpm = 120
    ticks_per_sec = tpq * bpm / 60.0
    hold = max(1, int(seconds * ticks_per_sec))
    write_mid(out, note, 100, hold, tpq)
    print(out, note, hold)


if __name__ == "__main__":
    main()
