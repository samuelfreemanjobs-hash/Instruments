#!/usr/bin/env python3
"""Export scale-locked bass/melody MIDI from melodic-patterns.json."""

from __future__ import annotations

import argparse
import json
import struct
import sys
from pathlib import Path

sys.path.insert(0, str(Path(__file__).resolve().parent))
from _scale import PATTERNS_PATH, SCALE_PATH, chord_notes_by_index, degree_to_midi, load_json  # noqa: E402

PPQ = 480


def vlq(value: int) -> bytes:
    buffer = value & 0x7F
    out: list[int] = []
    while True:
        out.insert(0, buffer)
        if value >> 7 == 0:
            break
        value >>= 7
        buffer = (value & 0x7F) | 0x80
    return bytes(out)


def midi_file(events: list[tuple[int, bytes]]) -> bytes:
    body = b""
    last = 0
    for t, msg in sorted(events, key=lambda x: x[0]):
        delta = max(0, t - last)
        body += vlq(delta) + msg
        last = t
    body += vlq(0) + bytes([0xFF, 0x2F, 0x00])
    track = b"MTrk" + struct.pack(">I", len(body)) + body
    header = b"MThd" + struct.pack(">IHHH", 6, 1, 1, PPQ)
    return header + track


def note_on(ch: int, note: int, vel: int = 100) -> bytes:
    return bytes([0x90 | (ch & 0x0F), note & 0x7F, vel & 0x7F])


def note_off(ch: int, note: int) -> bytes:
    return bytes([0x80 | (ch & 0x0F), note & 0x7F, 0x40])


def pattern_to_events(pattern: dict, scale: dict, bpm: int) -> list[tuple[int, bytes]]:
    ticks_per_step = PPQ // 4  # 16th at 4/4
    ch = int(pattern.get("channel", 2))
    events: list[tuple[int, bytes]] = []

    if "grid" in pattern:
        for hit in pattern["grid"]:
            step = int(hit["step"]) - 1
            note = degree_to_midi(scale, int(hit["degree"]), int(hit.get("octave", 3)))
            start = step * ticks_per_step
            dur = int(hit.get("lenSteps", 1)) * ticks_per_step
            events.append((start, note_on(ch, note)))
            events.append((start + dur, note_off(ch, note)))

    if "chordHits" in pattern:
        for hit in pattern["chordHits"]:
            step = int(hit["step"]) - 1
            start = step * ticks_per_step
            notes = chord_notes_by_index(int(hit["chordIndex"]))
            for n in notes:
                events.append((start, note_on(ch, n, 90)))
                events.append((start + ticks_per_step, note_off(ch, n)))

    tempo_us = int(60_000_000 / bpm)
    tempo_bytes = tempo_us.to_bytes(3, byteorder="big")
    events.insert(0, (0, bytes([0xFF, 0x51, 0x03]) + tempo_bytes))
    return events


def main() -> int:
    parser = argparse.ArgumentParser(description="Generate scale-locked MIDI patterns")
    parser.add_argument("--pattern", required=True, help="Pattern id from melodic-patterns.json")
    parser.add_argument("--out", required=True, type=Path)
    parser.add_argument("--bpm", type=int, default=140)
    args = parser.parse_args()

    patterns = load_json(PATTERNS_PATH)
    scale = load_json(SCALE_PATH)
    pat = next((p for p in patterns["patterns"] if p["id"] == args.pattern), None)
    if pat is None:
        raise SystemExit(f"Unknown pattern: {args.pattern}")

    events = pattern_to_events(pat, scale, args.bpm)
    data = midi_file(events)
    out = args.out.expanduser().resolve()
    out.parent.mkdir(parents=True, exist_ok=True)
    out.write_bytes(data)
    print(json.dumps({"pattern": args.pattern, "bpm": args.bpm, "out": str(out)}, indent=2))
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
