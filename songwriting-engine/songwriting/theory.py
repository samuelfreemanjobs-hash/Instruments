"""Pitch, scale, chord parsing, voicing candidates, and voice-leading costs."""

from __future__ import annotations

import re
from itertools import combinations

PITCH_CLASSES = {
    "C": 0, "C#": 1, "Db": 1, "D": 2, "D#": 3, "Eb": 3,
    "E": 4, "F": 5, "F#": 6, "Gb": 6, "G": 7, "G#": 8,
    "Ab": 8, "A": 9, "A#": 10, "Bb": 10, "B": 11,
}

SCALES = {
    "major": (0, 2, 4, 5, 7, 9, 11),
    "natural_minor": (0, 2, 3, 5, 7, 8, 10),
    "harmonic_minor": (0, 2, 3, 5, 7, 8, 11),
    "melodic_minor": (0, 2, 3, 5, 7, 9, 11),
    "dorian": (0, 2, 3, 5, 7, 9, 10),
    "mixolydian": (0, 2, 4, 5, 7, 9, 10),
    "major_pentatonic": (0, 2, 4, 7, 9),
    "minor_pentatonic": (0, 3, 5, 7, 10),
    "blues": (0, 3, 5, 6, 7, 10),
}

_CHORD_PATTERN = re.compile(
    r"^(?P<root>[A-G](?:#|b)?)(?P<quality>maj|min|m|dim|aug|sus2|sus4)?"
    r"(?P<extension>13|11|9|7|6)?(?P<add>add9)?$"
)


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


def parse_chord(symbol: str) -> tuple[int, tuple[int, ...]]:
    match = _CHORD_PATTERN.fullmatch(symbol)
    if not match:
        raise ValueError(f"unsupported chord symbol: {symbol!r}")
    root = pitch_class(match.group("root"))
    quality = match.group("quality") or ""
    extension = match.group("extension")
    add = match.group("add")

    if quality in ("m", "min"):
        intervals = [0, 3, 7]
    elif quality == "dim":
        intervals = [0, 3, 6]
    elif quality == "aug":
        intervals = [0, 4, 8]
    elif quality == "sus2":
        intervals = [0, 2, 7]
    elif quality == "sus4":
        intervals = [0, 5, 7]
    else:
        intervals = [0, 4, 7]

    if extension == "6":
        intervals.append(9)
    elif extension == "7":
        intervals.append(11 if quality == "maj" else 10)
    elif extension == "9":
        intervals.extend((11, 14) if quality == "maj" else (10, 14))
    elif extension == "11":
        intervals.extend((10, 14, 17))
    elif extension == "13":
        intervals.extend((10, 14, 21))
    if add:
        intervals.append(14)
    return root, tuple(sorted({(root + interval) % 12 for interval in intervals}))


def voicing_candidates(
    symbol: str,
    low: int = 48,
    high: int = 84,
    *,
    rootless: bool = False,
    max_span: int = 24,
) -> tuple[tuple[int, ...], ...]:
    root, pitch_classes = parse_chord(symbol)
    required = list(pitch_classes)
    if rootless and len(required) > 3:
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
