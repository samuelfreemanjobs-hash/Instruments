#!/usr/bin/env python3
"""Emit a trap/phonk loop blueprint JSON (+ optional minimal Type-0 MIDI). Stdlib only."""
from __future__ import annotations

import argparse
import json
import struct
import sys
from pathlib import Path

from dsp_math import loop_samples

# pitch per lane for sketch MIDI (GM drum)
LANE_PITCH = {
    "KICK": 36,
    "SNARE": 38,
    "CLAP": 39,
    "HAT": 42,
    "808": 36,  # same key as kick sketch; product notes pitched 808 separately
    "PERC": 37,
    "FX": 49,
    "COWBELL": 56,
}

PROFILES = {
    "drift-phonk": {"default_bpm": 74, "feel": "half-time cruise", "hat_density": "medium"},
    "detroit-trap": {"default_bpm": 140, "feel": "half-time snare", "hat_density": "high"},
    "southern-trap": {"default_bpm": 130, "feel": "bounce", "hat_density": "high"},
    "memphis-tape": {"default_bpm": 120, "feel": "loose tape", "hat_density": "medium"},
    "cloud-rap": {"default_bpm": 128, "feel": "airy space", "hat_density": "low"},
}


def _vlq(n: int) -> bytes:
    out = [n & 0x7F]
    n >>= 7
    while n:
        out.append(0x80 | (n & 0x7F))
        n >>= 7
    return bytes(reversed(out))


def write_type0_midi(path: Path, bpm: float, ppq: int, events: list[tuple[int, int, int]]) -> None:
    """events: list of (tick, pitch, velocity) note-ons; short fixed duration."""
    tempo = int(round(60_000_000 / bpm))
    track = bytearray()
    track += b"\x00\xff\x51\x03" + struct.pack(">I", tempo)[1:]
    last = 0
    dur = max(1, ppq // 8)
    # sort and emit note-on / note-off pairs
    ordered = sorted(events, key=lambda e: e[0])
    offs = [(t + dur, p, 0) for t, p, v in ordered]
    timeline = sorted([(t, p, v, 1) for t, p, v in ordered] + [(t, p, 0, 0) for t, p, _ in offs])
    for tick, pitch, vel, is_on in timeline:
        delta = tick - last
        last = tick
        track += _vlq(delta)
        status = 0x90 if is_on and vel > 0 else 0x80
        track += bytes([status, pitch & 0x7F, vel & 0x7F])
    track += b"\x00\xff\x2f\x00"
    header = b"MThd" + struct.pack(">IHHH", 6, 0, 1, ppq)
    mtrk = b"MTrk" + struct.pack(">I", len(track)) + bytes(track)
    path.write_bytes(header + mtrk)


def default_steps(genre: str, bars: int, grid: int = 16) -> dict[str, list[dict]]:
    """Sparse original patterns — not artist clones. Velocities tagged A in blueprint."""
    steps_per_bar = grid
    total = bars * steps_per_bar
    kick = [{"step": i, "vel": 100} for i in range(0, total, steps_per_bar)]  # downbeats
    snare = [{"step": i + steps_per_bar // 2, "vel": 110} for i in range(0, total, steps_per_bar)]
    if genre in ("detroit-trap", "southern-trap"):
        # extra kick on offbeat of bar 0 only
        if total > 10:
            kick.append({"step": 10, "vel": 85})
    hats = []
    dens = PROFILES.get(genre, {}).get("hat_density", "medium")
    stride = 1 if dens == "high" else 2 if dens == "medium" else 4
    for s in range(0, total, stride):
        hats.append({"step": s, "vel": 70 if s % 4 else 90})
    # short roll at end of last bar
    if dens != "low" and total >= 4:
        for s in range(total - 4, total):
            hats.append({"step": s, "vel": 60 + (s - (total - 4)) * 10})
    return {
        "KICK": kick,
        "SNARE": snare,
        "HAT": hats,
        "808": [{"step": 0, "vel": 100, "note": "set key in product spec"}],
    }


def build_blueprint(name: str, genre: str, bpm: float, bars: int, sr: int, grid: int) -> dict:
    if genre not in PROFILES:
        raise ValueError(f"unknown genre {genre}; choose from {sorted(PROFILES)}")
    samples_bar = loop_samples(1, bpm, sr)
    lanes = default_steps(genre, bars, grid)
    return {
        "schema": "drumsmith-loop-blueprint-v1",
        "name": name,
        "genre_profile": genre,
        "bpm": bpm,
        "bars": bars,
        "sample_rate": sr,
        "grid_subdivisions_per_bar": grid,
        "samples_per_bar": samples_bar,
        "samples_per_bar_tag": "C",
        "feel": PROFILES[genre]["feel"],
        "promise": f"Original {genre} drum loop sketch — not a recreation of any artist or brand kit",
        "lanes": lanes,
        "808": {"key": "TBD", "glide": "A", "length_bars": min(2, bars)},
        "variants": [
            {"id": "full", "bars": bars},
            {"id": "sparse", "bars": bars, "notes": "drop hats / thin kicks — A"},
        ],
        "status": "agent-demo",
        "device_exports": "UNVERIFIED",
    }


def blueprint_to_midi_events(blueprint: dict, ppq: int) -> list[tuple[int, int, int]]:
    grid = int(blueprint["grid_subdivisions_per_bar"])
    grid_ticks = (ppq * 4) // grid
    events = []
    for lane, hits in blueprint["lanes"].items():
        if lane == "808":
            continue  # pitched separately
        pitch = LANE_PITCH.get(lane, 37)
        for h in hits:
            events.append((int(h["step"]) * grid_ticks, pitch, int(h.get("vel", 100))))
    return events


def main(argv=None) -> int:
    ap = argparse.ArgumentParser(description="Trap/phonk loop architect → blueprint JSON")
    ap.add_argument("--name", default="trap-loop")
    ap.add_argument("--genre", default="detroit-trap", choices=sorted(PROFILES))
    ap.add_argument("--bpm", type=float, default=None)
    ap.add_argument("--bars", type=int, default=2)
    ap.add_argument("--sr", type=int, default=44100)
    ap.add_argument("--grid", type=int, default=16)
    ap.add_argument("--out", type=Path, required=True, help="Blueprint JSON path")
    ap.add_argument("--midi-out", type=Path, help="Optional sketch MIDI Type-0")
    ap.add_argument("--ppq", type=int, default=480)
    args = ap.parse_args(argv)
    bpm = args.bpm if args.bpm is not None else PROFILES[args.genre]["default_bpm"]
    bp = build_blueprint(args.name, args.genre, bpm, args.bars, args.sr, args.grid)
    args.out.parent.mkdir(parents=True, exist_ok=True)
    args.out.write_text(json.dumps(bp, indent=2) + "\n", encoding="utf-8")
    if args.midi_out:
        write_type0_midi(args.midi_out, bpm, args.ppq, blueprint_to_midi_events(bp, args.ppq))
    print(json.dumps({"wrote": str(args.out), "midi": str(args.midi_out) if args.midi_out else None, "samples_per_bar": bp["samples_per_bar"]}))
    return 0


if __name__ == "__main__":
    sys.exit(main())
