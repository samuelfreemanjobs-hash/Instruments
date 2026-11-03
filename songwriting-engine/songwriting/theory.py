"""Pitch, scale, chord parsing, voicing candidates, and voice-leading costs."""

from __future__ import annotations

import re
from dataclasses import dataclass
from itertools import combinations

PITCH_CLASSES = {
    "C": 0, "C#": 1, "Db": 1, "D": 2, "D#": 3, "Eb": 3,
    "E": 4, "F": 5, "F#": 6, "Gb": 6, "G": 7, "G#": 8,
    "Ab": 8, "A": 9, "A#": 10, "Bb": 10, "B": 11,
}

FLAT_NAMES = ("C", "Db", "D", "Eb", "E", "F", "Gb", "G", "Ab", "A", "Bb", "B")
SHARP_NAMES = ("C", "C#", "D", "D#", "E", "F", "F#", "G", "G#", "A", "A#", "B")

SCALES = {
    "major": (0, 2, 4, 5, 7, 9, 11),
    "natural_minor": (0, 2, 3, 5, 7, 8, 10),
    "harmonic_minor": (0, 2, 3, 5, 7, 8, 11),
    "melodic_minor": (0, 2, 3, 5, 7, 9, 11),
    "dorian": (0, 2, 3, 5, 7, 9, 10),
    "phrygian": (0, 1, 3, 5, 7, 8, 10),
    "lydian": (0, 2, 4, 6, 7, 9, 11),
    "mixolydian": (0, 2, 4, 5, 7, 9, 10),
    "locrian": (0, 1, 3, 5, 6, 8, 10),
    "major_pentatonic": (0, 2, 4, 7, 9),
    "minor_pentatonic": (0, 3, 5, 7, 10),
    "blues": (0, 3, 5, 6, 7, 10),
    "bebop_dominant": (0, 2, 4, 5, 7, 9, 10, 11),
    "bebop_major": (0, 2, 4, 5, 7, 8, 9, 11),
    "bebop_dorian": (0, 2, 3, 4, 5, 7, 9, 10),
}

_CHORD_PATTERN = re.compile(
    r"^(?P<root>[A-G](?:#|b)?)"
    r"(?P<quality>mMaj|mmaj|maj|M|min|m|dim|o|°|aug|\+|ø|sus2|sus4|sus)?"
    r"(?P<extension>6/9|69|13|11|9|7|6)?"
    r"(?P<suspension>sus2|sus4|sus)?"
    r"(?P<alterations>(?:\(?(?:b5|#5|b9|#9|#11|b13|add9|add11|add13|add2|add4)\)?)*)"
    r"(?:/(?P<bass>[A-G](?:#|b)?))?$"
)
_ALTERATION = re.compile(r"b5|#5|b9|#9|#11|b13|add9|add11|add13|add2|add4")

_TRIADS = {
    "": (0, 4, 7), "maj": (0, 4, 7), "M": (0, 4, 7),
    "m": (0, 3, 7), "min": (0, 3, 7), "mMaj": (0, 3, 7), "mmaj": (0, 3, 7),
    "dim": (0, 3, 6), "o": (0, 3, 6), "°": (0, 3, 6), "ø": (0, 3, 6),
    "aug": (0, 4, 8), "+": (0, 4, 8),
    "sus2": (0, 2, 7), "sus4": (0, 5, 7), "sus": (0, 5, 7),
}
_MAJOR_SEVENTH = {"maj", "M", "mMaj", "mmaj"}
_DIMINISHED = {"dim", "o", "°"}
_MINOR = {"m", "min", "mMaj", "mmaj"}

#: Voicings above this many pitch classes drop the fifth, then the root, then the natural 9th.
MAX_VOICED_TONES = 5


@dataclass(frozen=True, slots=True)
class ChordSpec:
    symbol: str
    root: int
    pitch_classes: tuple[int, ...]
    intervals: frozenset[int]
    bass: int | None
    quality: str
    extensions: tuple[str, ...]

    @property
    def is_dominant(self) -> bool:
        return 4 in self.intervals and 10 in self.intervals

    @property
    def is_minor(self) -> bool:
        return 3 in self.intervals and 4 not in self.intervals


def pitch_class(note: str) -> int:
    try:
        return PITCH_CLASSES[note]
    except KeyError as error:
        raise ValueError(f"unsupported pitch name: {note!r}") from error


def scale_pitch_classes(key: str, scale: str) -> tuple[int, ...]:
    if scale not in SCALES:
        raise ValueError(f"unsupported scale: {scale!r}")
    root = pitch_class(key)
    return tuple((root + interval) % 12 for interval in SCALES[scale])


def parse_chord_spec(symbol: str) -> ChordSpec:
    match = _CHORD_PATTERN.fullmatch(symbol)
    if not match:
        raise ValueError(f"unsupported chord symbol: {symbol!r}")
    root = pitch_class(match.group("root"))
    quality = match.group("quality") or ""
    extension = match.group("extension")
    suspension = match.group("suspension")
    alterations = _ALTERATION.findall(match.group("alterations") or "")

    intervals = set(_TRIADS[quality])
    if suspension:
        intervals -= {3, 4}
        intervals.add(2 if suspension == "sus2" else 5)

    if extension in ("7", "9", "11", "13") or quality == "ø":
        if quality in _MAJOR_SEVENTH:
            intervals.add(11)
        elif quality in _DIMINISHED:
            intervals.add(9)
        else:
            intervals.add(10)
    if extension == "6":
        intervals.add(9)
    elif extension in ("69", "6/9"):
        intervals.update((9, 14))
    elif extension == "9":
        intervals.add(14)
    elif extension == "11":
        intervals.update((14, 17))
        if quality == "":
            intervals.discard(4)
    elif extension == "13":
        intervals.update((14, 21))
        if quality in _MINOR:
            intervals.add(17)

    replacements = {"b5": (7, 6), "#5": (7, 8), "b9": (14, 13), "#9": (14, 15), "#11": (17, 18), "b13": (21, 20)}
    additions = {"add9": 14, "add2": 14, "add11": 17, "add4": 17, "add13": 21}
    for alteration in alterations:
        if alteration in replacements:
            natural, altered = replacements[alteration]
            intervals.discard(natural)
            intervals.add(altered)
        else:
            intervals.add(additions[alteration])

    bass = match.group("bass")
    pcs = frozenset(interval % 12 for interval in intervals)
    return ChordSpec(
        symbol=symbol,
        root=root,
        pitch_classes=tuple(sorted({(root + interval) % 12 for interval in intervals})),
        intervals=pcs,
        bass=pitch_class(bass) if bass else None,
        quality=_quality_name(quality, suspension, pcs),
        extensions=tuple(([("6/9" if extension == "69" else extension)] if extension else []) + alterations),
    )


def _quality_name(quality: str, suspension: str | None, intervals: frozenset[int]) -> str:
    if quality == "ø" or (quality in _MINOR and {3, 6, 10} <= intervals and 7 not in intervals):
        return "half_diminished"
    if quality in _DIMINISHED:
        return "diminished"
    if quality in ("aug", "+"):
        return "augmented"
    if quality in ("mMaj", "mmaj"):
        return "minor_major"
    if quality in _MINOR:
        return "minor"
    if suspension or quality.startswith("sus"):
        return "suspended"
    if quality in _MAJOR_SEVENTH:
        return "major"
    return "dominant" if 10 in intervals else "major"


def parse_chord(symbol: str) -> tuple[int, tuple[int, ...]]:
    spec = parse_chord_spec(symbol)
    return spec.root, spec.pitch_classes


def voicing_pitch_classes(symbol: str) -> tuple[int, ...]:
    """Chord tones to voice: thick extended chords shed the fifth, root, then natural 9th."""
    spec = parse_chord_spec(symbol)
    tones = set(spec.pitch_classes)
    for interval in (7, 0, 2):
        if len(tones) <= MAX_VOICED_TONES:
            break
        tones.discard((spec.root + interval) % 12)
    return tuple(sorted(tones))


def voicing_candidates(
    symbol: str,
    low: int = 48,
    high: int = 84,
    *,
    rootless: bool = False,
    max_span: int = 24,
) -> tuple[tuple[int, ...], ...]:
    root = parse_chord_spec(symbol).root
    required = list(voicing_pitch_classes(symbol))
    if rootless and len(required) > 3 and root in required:
        required.remove(root)
    legal_notes = [note for note in range(low, high + 1) if note % 12 in required]
    candidates: set[tuple[int, ...]] = set()
    for count in range(1, len(required) + 1):
        for notes in combinations(legal_notes, count):
            if set(note % 12 for note in notes) == set(required) and notes[-1] - notes[0] <= max_span:
                candidates.add(notes)
    return tuple(sorted(candidates, key=lambda notes: (sum(notes) / len(notes), notes)))


def voice_leading_cost(previous: tuple[int, ...], current: tuple[int, ...]) -> float:
    """Minimum monotonic voice assignment with explicit cost for added/removed voices."""
    previous = tuple(sorted(previous))
    current = tuple(sorted(current))
    gap_cost = 9.0
    rows, cols = len(previous), len(current)
    costs = [[0.0] * (cols + 1) for _ in range(rows + 1)]
    for i in range(1, rows + 1):
        costs[i][0] = costs[i - 1][0] + gap_cost
    for j in range(1, cols + 1):
        costs[0][j] = costs[0][j - 1] + gap_cost
    for i in range(1, rows + 1):
        for j in range(1, cols + 1):
            movement = abs(previous[i - 1] - current[j - 1])
            movement += max(0, movement - 12) * 1.5
            if previous[i - 1] == current[j - 1]:
                movement -= 2.0
            costs[i][j] = min(
                costs[i - 1][j - 1] + movement,
                costs[i - 1][j] + gap_cost,
                costs[i][j - 1] + gap_cost,
            )
    return costs[rows][cols]


def choose_voicings(symbols: tuple[str, ...], low: int, high: int, *, rootless: bool = False):
    candidate_sets = [voicing_candidates(symbol, low, high, rootless=rootless) for symbol in symbols]
    if any(not candidates for candidates in candidate_sets):
        raise ValueError("no legal voicing candidates for the selected range")
    paths: list[tuple[float, tuple[tuple[int, ...], ...]]] = [
        (0.0, (candidate,)) for candidate in candidate_sets[0]
    ]
    for candidates in candidate_sets[1:]:
        paths = [
            min(
                (cost + voice_leading_cost(path[-1], candidate), path + (candidate,))
                for cost, path in paths
            )
            for candidate in candidates
        ]
    total_cost, path = min(paths, key=lambda item: (item[0], item[1]))
    transition_costs = [0.0]
    transition_costs.extend(
        voice_leading_cost(path[index - 1], path[index]) for index in range(1, len(path))
    )
    return path, tuple(transition_costs), total_cost
