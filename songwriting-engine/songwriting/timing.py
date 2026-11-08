"""Beat ↔ MIDI tick conversion. Projects store quarter-note beats; ticks exist only at the edges."""

from __future__ import annotations

DEFAULT_PPQ = 960


def beats_to_ticks(beats: float, ppq: int = DEFAULT_PPQ) -> int:
    return round(beats * ppq)


def ticks_to_beats(ticks: int, ppq: int = DEFAULT_PPQ) -> float:
    return ticks / ppq


def snap_beats(beats: float, ppq: int = DEFAULT_PPQ) -> float:
    """Round a beat position onto the tick grid so stored beats convert to ticks exactly."""
    return ticks_to_beats(beats_to_ticks(beats, ppq), ppq)
