"""Scale-lock helpers for MPC500 melodic tools."""

from __future__ import annotations

import json
from pathlib import Path

REPO = Path(__file__).resolve().parents[2]
SCALE_PATH = REPO / "mpc500" / "lanes" / "phonk" / "scale-lock.json"
CHORDS_PATH = REPO / "mpc500" / "lanes" / "phonk" / "chord-triggers.json"
PATTERNS_PATH = REPO / "mpc500" / "lanes" / "phonk" / "melodic-patterns.json"

ROOT_PC = {
    "C": 0,
    "C#": 1,
    "DB": 1,
    "D": 2,
    "EB": 3,
    "D#": 3,
    "E": 4,
    "F": 5,
    "F#": 6,
    "GB": 6,
    "G": 7,
    "AB": 8,
    "G#": 8,
    "A": 9,
    "BB": 10,
    "A#": 10,
    "B": 11,
}


def load_json(path: Path) -> dict:
    return json.loads(path.read_text(encoding="utf-8"))


def root_pitch_class(root: str) -> int:
    key = root.strip().upper().replace(" ", "")
    if key not in ROOT_PC:
        raise ValueError(f"Unknown root: {root}")
    return ROOT_PC[key]


def midi_from_octave_pc(octave: int, pc: int) -> int:
    """Octave in scientific pitch (C4 = octave 4)."""
    return 12 * (octave + 1) + (pc % 12)


def degree_to_midi(scale: dict, degree: int, octave: int) -> int:
    """1-based scale degree in locked natural minor (or configured intervals)."""
    intervals = scale["intervalsSemis"]
    r_pc = root_pitch_class(scale["root"])
    d0 = degree - 1
    semis = intervals[d0 % len(intervals)] + 12 * (d0 // len(intervals))
    pc = (r_pc + semis) % 12
    oct_add = (r_pc + semis) // 12
    return midi_from_octave_pc(octave + oct_add, pc)


def chord_notes_by_index(index: int) -> list[int]:
    data = load_json(CHORDS_PATH)
    for t in data["triggers"]:
        if t["index"] == index:
            return list(t["notes"])
    raise KeyError(f"chord index {index}")
