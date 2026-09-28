#!/usr/bin/env python3
"""Build Type-0 MIDI with Juno-106 APR + note for KR-106 render_midi (golden scenarios)."""

from __future__ import annotations

import json
import struct
import sys
from pathlib import Path

ROOT = Path(__file__).resolve().parents[2]
FIXTURES = Path(__file__).resolve().parents[1] / "fixtures" / "kr106_scenarios.json"


def vlq(n: int) -> bytes:
    buf: list[int] = [n & 0x7F]
    n >>= 7
    while n:
        buf.insert(0, (n & 0x7F) | 0x80)
        n >>= 7
    return bytes(buf)


def sysex_event(payload: bytes) -> bytes:
    """MIDI track bytes: F0 + len + payload (payload excludes F0/F7)."""
    return bytes([0xF0]) + vlq(len(payload)) + payload


def make_apr(sliders: dict[str, int], sw1: int, sw2: int, channel: int = 0) -> bytes:
    s = [0] * 16
    for key, val in sliders.items():
        cc = int(key, 16) if isinstance(key, str) else int(key)
        if 0 <= cc < 16:
            s[cc] = max(0, min(127, int(val)))
    body = bytes([0x41, 0x31, channel & 0x0F, 0]) + bytes(s) + bytes([sw1 & 0xFF, sw2 & 0xFF])
    return body


def write_mid(
    path: Path,
    *,
    note: int,
    velocity: int,
    hold_ticks: int,
    apr: bytes,
    tpq: int = 480,
) -> None:
    track = b""
    tick = 0

    def append_delta_events(delta: int, chunks: bytes) -> None:
        nonlocal track, tick
        track += vlq(delta) + chunks
        tick += delta

    # Settle after patch load
    append_delta_events(0, sysex_event(apr))
    append_delta_events(tpq // 2, b"")  # 0.25s at 120bpm if tpq=480

    append_delta_events(0, bytes([0x90, note & 0x7F, velocity & 0x7F]))
    append_delta_events(hold_ticks, bytes([0x80, note & 0x7F, 0]))
    append_delta_events(0, b"\xFF\x2F\x00")

    header = b"MThd" + struct.pack(">IHHH", 6, 0, 1, tpq)
    trk = b"MTrk" + struct.pack(">I", len(track)) + track
    path.write_bytes(header + trk)


def main() -> int:
    scenario_id = sys.argv[1] if len(sys.argv) > 1 else "ab03-chorus-i"
    out = Path(sys.argv[2]) if len(sys.argv) > 2 else Path(f"/tmp/kr106-{scenario_id}.mid")

    data = json.loads(FIXTURES.read_text(encoding="utf-8"))
    scenarios = data.get("scenarios", {})
    if scenario_id not in scenarios:
        print(f"Unknown scenario {scenario_id!r}; known: {', '.join(scenarios)}", file=sys.stderr)
        return 1

    spec = scenarios[scenario_id]
    note = int(spec.get("note", 60))
    seconds = float(spec.get("seconds", 3.0))
    tpq = 480
    bpm = 120
    hold = max(1, int(seconds * tpq * bpm / 60))

    apr = make_apr(spec.get("sliders", {}), int(spec["sw1"]), int(spec["sw2"]))
    write_mid(out, note=note, velocity=100, hold_ticks=hold, apr=apr, tpq=tpq)
    print(out, scenario_id, note, hold)
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
