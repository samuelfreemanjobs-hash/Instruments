#!/usr/bin/env python3
"""Play Lofi-12 pattern JSON via MIDI OUT (requires mido + python-rtmidi)."""

from __future__ import annotations

import argparse
import sys
import time
from pathlib import Path

SCRIPT_DIR = Path(__file__).resolve().parent
sys.path.insert(0, str(SCRIPT_DIR))

from pattern_schema import Pattern, load_pattern  # noqa: E402


def list_ports() -> list[str]:
    import mido

    return mido.get_output_names()


def play_pattern(pattern: Pattern, port_name: str, loops: int, duration_sec: float) -> None:
    import mido

    out = mido.open_output(port_name)
    step_dur = (60.0 / pattern.bpm) / 4
    deadline = time.monotonic() + duration_sec if duration_sec > 0 else None
    loop_count = 0
    try:
        while True:
            if deadline and time.monotonic() >= deadline:
                break
            if loops and loop_count >= loops:
                break
            for step in range(pattern.steps):
                t0 = time.monotonic()
                for tr in pattern.tracks:
                    cell = tr.steps[step]
                    if not cell.on:
                        continue
                    msg = mido.Message(
                        "note_on",
                        channel=max(0, tr.midi_channel - 1),
                        note=cell.note,
                        velocity=cell.velocity,
                    )
                    out.send(msg)
                    off = mido.Message(
                        "note_off",
                        channel=max(0, tr.midi_channel - 1),
                        note=cell.note,
                        velocity=0,
                    )
                    out.send(off)
                elapsed = time.monotonic() - t0
                time.sleep(max(0.0, step_dur - elapsed))
            loop_count += 1
    finally:
        out.close()


def main() -> None:
    p = argparse.ArgumentParser()
    p.add_argument("--pattern", type=Path, required=True)
    p.add_argument("--port", default=None, help="MIDI OUT name (see --list-ports)")
    p.add_argument("--list-ports", action="store_true")
    p.add_argument("--loops", type=int, default=4)
    p.add_argument("--seconds", type=float, default=0, help="0 = use --loops only")
    args = p.parse_args()

    if args.list_ports:
        for name in list_ports():
            print(name)
        return

    try:
        import mido  # noqa: F401
    except ImportError as e:
        raise SystemExit("Install: pip install mido python-rtmidi") from e

    pat = load_pattern(args.pattern)
    names = list_ports()
    if not names:
        raise SystemExit("No MIDI output ports found")
    port = args.port or names[0]
    if port not in names:
        raise SystemExit(f"Port not found: {port}. Available: {names}")
    play_pattern(pat, port, args.loops, args.seconds)
    print(f"Played {args.loops} loops @ {pat.bpm} BPM → {port}")


if __name__ == "__main__":
    main()
