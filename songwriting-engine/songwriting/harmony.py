"""Reharmonization: approach chords inserted before each chord change.

Every progression slot keeps its chord; when a technique applies, the tail of the slot is
handed to an approach chord that resolves into the next slot's root.
"""

from __future__ import annotations

from dataclasses import dataclass
from typing import Any

from .theory import FLAT_NAMES, SHARP_NAMES, parse_chord_spec

REHARM_OPTIONS = ("secondary_dominants", "tritone_subs", "passing_chords")


@dataclass(frozen=True, slots=True)
class CycleChord:
    symbol: str
    offset_beats: float
    duration_beats: float
    function: str


def validate_reharm(reharm: Any) -> dict[str, bool]:
    if reharm is None:
        return {name: False for name in REHARM_OPTIONS}
    if not isinstance(reharm, dict):
        raise ValueError("harmony.reharm must be an object of booleans")
    unknown = set(reharm) - set(REHARM_OPTIONS)
    if unknown:
        raise ValueError(f"unsupported harmony.reharm options: {sorted(unknown)}")
    if any(not isinstance(value, bool) for value in reharm.values()):
        raise ValueError("harmony.reharm values must be true or false")
    return {name: reharm.get(name, False) for name in REHARM_OPTIONS}


def approach_chord(current: str, target: str, reharm: dict[str, bool], *, gospel: bool) -> tuple[str, str] | None:
    """Return (symbol, function) leading from ``current`` into ``target``, or None."""
    here = parse_chord_spec(current)
    there = parse_chord_spec(target)
    if current == target:
        return None
    goal = there.root
    if reharm["passing_chords"] and (goal - here.root) % 12 == 2:
        return f"{SHARP_NAMES[(here.root + 1) % 12]}dim7", "passing_dim"
    if reharm["tritone_subs"]:
        return f"{FLAT_NAMES[(goal + 1) % 12]}{'13' if gospel and not there.is_minor else '9'}", "tritone_sub"
    if reharm["secondary_dominants"]:
        if here.is_dominant and (here.root - goal) % 12 == 7:
            return None
        quality = "7b9" if there.is_minor else ("13" if gospel else "7")
        return f"{FLAT_NAMES[(goal + 7) % 12]}{quality}", "secondary_dominant"
    return None


def harmonic_cycle(
    progression: tuple[str, ...],
    bars_per_chord: int,
    beats_per_bar: float,
    reharm: dict[str, bool],
    *,
    gospel: bool = False,
) -> tuple[CycleChord, ...]:
    slot = bars_per_chord * beats_per_bar
    approach = 2.0 if slot >= 8 else 1.0
    cycle: list[CycleChord] = []
    for index, symbol in enumerate(progression):
        offset = index * slot
        target = progression[(index + 1) % len(progression)]
        inserted = approach_chord(symbol, target, reharm, gospel=gospel) if slot >= 2 * approach else None
        if inserted is None:
            cycle.append(CycleChord(symbol, offset, slot, "chord"))
            continue
        approach_symbol, function = inserted
        cycle.append(CycleChord(symbol, offset, slot - approach, "chord"))
        cycle.append(CycleChord(approach_symbol, offset + slot - approach, approach, function))
    return tuple(cycle)
