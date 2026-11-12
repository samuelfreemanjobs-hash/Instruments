"""Tick math, swing, and scale helpers."""

from __future__ import annotations

import random

NOTE_NAMES = ["C", "C#", "D", "D#", "E", "F", "F#", "G", "G#", "A", "A#", "B"]
_FLAT_MAP = {"DB": "C#", "EB": "D#", "GB": "F#", "AB": "G#", "BB": "A#"}


def root_midi(root: str, octave: int = 3) -> int:
    name = root.strip().upper()
    if name in _FLAT_MAP:
        name = _FLAT_MAP[name]
    if name not in NOTE_NAMES:
        raise ValueError(f"unsupported root: {root}")
    idx = NOTE_NAMES.index(name)
    return 12 * (octave + 1) + idx


SCALES = {
    "dorian": [0, 2, 3, 5, 7, 9, 10],
    "mixolydian": [0, 2, 4, 5, 7, 9, 10],
    "aeolian": [0, 2, 3, 5, 7, 8, 10],
}


def scale_pitch(root: str, degree: int, octave: int, scale: str) -> int:
    base = root_midi(root, octave)
    steps = SCALES.get(scale, SCALES["dorian"])
    deg = degree % len(steps)
    oct_add = degree // len(steps)
    return base + steps[deg] + 12 * oct_add


def bar_tick(bar: int, beat: float, tpq: int) -> int:
    return int((bar * 4 + beat) * tpq)


def apply_swing_16th(tick: int, tpq: int, swing_pct: float, is_offbeat_16th: bool) -> int:
    if not is_offbeat_16th:
        return tick
    # Push offbeat 16ths later for swing feel
    sixteenth = tpq // 4
    shift = int(sixteenth * (swing_pct / 100.0 - 0.5) * 2)
    return tick + max(0, shift)


def humanize_ticks(tick: int, tpq: int, seed: int, max_shift: int = 6) -> int:
    rng = random.Random(seed)
    shift = rng.randint(-max_shift, max_shift)
    return max(0, tick + shift)
