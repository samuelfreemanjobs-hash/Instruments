#!/usr/bin/env python3
"""Convert phonk_groove bar into Lofi-12 sequencer pattern JSON."""

from __future__ import annotations

import argparse
import sys
from pathlib import Path

SCRIPT_DIR = Path(__file__).resolve().parent
FACTORY_SCRIPTS = SCRIPT_DIR.parent.parent / "scripts"
sys.path.insert(0, str(FACTORY_SCRIPTS))
sys.path.insert(0, str(SCRIPT_DIR))

from pattern_schema import (  # noqa: E402
    SLOT_TO_NOTE,
    Pattern,
    StepCell,
    TrackPattern,
    default_six_track_pattern,
    save_pattern,
)
from phonk_groove import STEPS_PER_BAR, build_bar, validate_bpm  # noqa: E402
from phonk_memphis_lanes import resolve_memphis_lane  # noqa: E402
from phonk_synth import resolve_phonk_params  # noqa: E402

INSTRUMENT_TRACK = {
    "kick": 0,
    "kick_dist": 0,
    "snare": 1,
    "clap": 1,
    "hat": 2,
    "cowbell": 3,
    "rim": 4,
    "hat_open": 5,
}

SLOT_BY_KEY = {
    "kick": 1,
    "kick_dist": 2,
    "snare": 4,
    "clap": 6,
    "rim": 13,
    "hat": 7,
    "hat_open": 8,
    "cowbell": 10,
}


def groove_to_pattern(prompt: str, bpm: float, variation: int = 0) -> Pattern:
    lane = resolve_memphis_lane(prompt, None)
    params = resolve_phonk_params(prompt, variation, lane)
    bar = build_bar(0, bpm=bpm, seed=params.seed, variation=variation, lane=lane)
    tracks = default_six_track_pattern()
    for tr in tracks:
        tr.steps = [StepCell(note=tr.default_note) for _ in range(STEPS_PER_BAR)]

    for hit in bar.hits:
        ti = INSTRUMENT_TRACK.get(hit.instrument)
        if ti is None:
            continue
        step = hit.step
        if step < 0 or step >= STEPS_PER_BAR:
            continue
        slot = SLOT_BY_KEY.get(hit.instrument, 1)
        note = SLOT_TO_NOTE.get(slot, 36)
        vel = max(1, min(127, int(hit.velocity * 127)))
        cell = tracks[ti].steps[step]
        cell.on = True
        cell.note = note
        cell.velocity = vel

    return Pattern(bpm=validate_bpm(bpm), tracks=tracks, version=2)


def main() -> None:
    p = argparse.ArgumentParser()
    p.add_argument("--prompt", default="juicy j dj paul dirty memphis 86")
    p.add_argument("--bpm", type=float, default=0)
    p.add_argument("--variation", type=int, default=0)
    p.add_argument("--out", type=Path, required=True)
    args = p.parse_args()
    bpm = args.bpm if args.bpm > 0 else 86.0
    pat = groove_to_pattern(args.prompt, bpm, args.variation)
    save_pattern(args.out, pat)
    print(f"Wrote {args.out}")


if __name__ == "__main__":
    main()
