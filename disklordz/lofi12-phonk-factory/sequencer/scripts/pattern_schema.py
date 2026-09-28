"""Lofi-12 sequencer pattern schema (4 or 6 tracks × 16 steps)."""

from __future__ import annotations

import json
from dataclasses import dataclass, field
from pathlib import Path
from typing import Any

STEPS = 16
TRACKS = 4
FORMAT = "LOFI12_STEP_PATTERN"
VERSION = 1
VERSION_SIX = 2

SIX_TRACK_LAYOUT = (
    ("Kick", 1, 1),
    ("Snare", 2, 4),
    ("Hat", 3, 7),
    ("Cowbell", 4, 10),
    ("Perc", 5, 13),
    ("OpenHat", 6, 8),
)

# Slot 1–16 → MIDI note (chromatic from C2). Adjust in UI if your bank root differs.
SLOT_TO_NOTE = {i: 35 + i for i in range(1, 17)}


def default_six_track_pattern() -> list[TrackPattern]:
    return [
        TrackPattern(name, midi_channel=ch, default_note=SLOT_TO_NOTE[slot])
        for name, ch, slot in SIX_TRACK_LAYOUT
    ]


@dataclass
class StepCell:
    on: bool = False
    note: int = 36  # slot 1 default
    velocity: int = 100


@dataclass
class TrackPattern:
    name: str
    midi_channel: int = 1
    default_note: int = 36
    steps: list[StepCell] = field(default_factory=list)

    def __post_init__(self) -> None:
        if not self.steps:
            self.steps = [StepCell(note=self.default_note) for _ in range(STEPS)]


@dataclass
class Pattern:
    bpm: float = 84.0
    steps: int = STEPS
    tracks: list[TrackPattern] = field(default_factory=list)
    send_clock: bool = False
    version: int = VERSION
    fx: dict[str, float] | None = None

    def __post_init__(self) -> None:
        if not self.tracks:
            self.tracks = [
                TrackPattern("Kick", default_note=SLOT_TO_NOTE[1]),
                TrackPattern("Snare", default_note=SLOT_TO_NOTE[4]),
                TrackPattern("Hat", default_note=SLOT_TO_NOTE[7]),
                TrackPattern("Cowbell", default_note=SLOT_TO_NOTE[10]),
            ]

    def to_dict(self) -> dict[str, Any]:
        ver = VERSION_SIX if len(self.tracks) >= 6 else self.version
        out: dict[str, Any] = {
            "format": FORMAT,
            "version": ver,
            "bpm": self.bpm,
            "steps": self.steps,
            "sendClock": self.send_clock,
            "tracks": [
                {
                    "name": t.name,
                    "midiChannel": t.midi_channel,
                    "defaultNote": t.default_note,
                    "steps": [
                        {"on": s.on, "note": s.note, "velocity": s.velocity} for s in t.steps
                    ],
                }
                for t in self.tracks
            ],
        }
        if self.fx:
            out["fx"] = self.fx
        return out

    @staticmethod
    def from_dict(data: dict[str, Any]) -> Pattern:
        tracks = []
        for tr in data.get("tracks", []):
            cells = [
                StepCell(
                    on=bool(s.get("on")),
                    note=int(s.get("note", 36)),
                    velocity=int(s.get("velocity", 100)),
                )
                for s in tr.get("steps", [])
            ]
            while len(cells) < STEPS:
                cells.append(StepCell(note=int(tr.get("defaultNote", 36))))
            tracks.append(
                TrackPattern(
                    name=str(tr.get("name", "Track")),
                    midi_channel=int(tr.get("midiChannel", 1)),
                    default_note=int(tr.get("defaultNote", 36)),
                    steps=cells[:STEPS],
                )
            )
        return Pattern(
            bpm=float(data.get("bpm", 84)),
            steps=int(data.get("steps", STEPS)),
            send_clock=bool(data.get("sendClock", False)),
            tracks=tracks or Pattern().tracks,
            version=int(data.get("version", VERSION)),
            fx=data.get("fx"),
        )


def load_pattern(path: Path) -> Pattern:
    return Pattern.from_dict(json.loads(path.read_text()))


def save_pattern(path: Path, pattern: Pattern) -> None:
    path.write_text(json.dumps(pattern.to_dict(), indent=2))
