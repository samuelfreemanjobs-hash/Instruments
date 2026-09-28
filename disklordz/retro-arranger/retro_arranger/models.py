"""1980s retro-modern MIDI arranger — session band agents."""

from __future__ import annotations

from dataclasses import dataclass, field
from enum import Enum


class SectionId(str, Enum):
    intro = "intro"
    verse = "verse"
    pre_chorus = "pre_chorus"
    chorus = "chorus"
    bridge = "bridge"
    solo = "solo"
    outro = "outro"


@dataclass
class ArrangementSpec:
    title: str = "Quiet Storm Night Drive"
    key_root: str = "A"
    scale: str = "dorian"  # dorian | mixolydian | aeolian
    bpm: int = 98
    time_sig: tuple[int, int] = (4, 4)
    swing_pct: float = 58.0  # 54–62 authentic swing on 16ths
    sections: list[tuple[SectionId, int]] = field(
        default_factory=lambda: [
            (SectionId.intro, 4),
            (SectionId.verse, 8),
            (SectionId.pre_chorus, 4),
            (SectionId.chorus, 8),
            (SectionId.bridge, 4),
            (SectionId.solo, 8),
            (SectionId.outro, 4),
        ]
    )

    @property
    def total_bars(self) -> int:
        return sum(b for _, b in self.spec_sections)

    @property
    def spec_sections(self) -> list[tuple[SectionId, int]]:
        return self.sections


@dataclass
class MidiNote:
    tick: int
    note: int
    velocity: int
    duration_ticks: int
    channel: int = 0


@dataclass
class MidiControl:
    tick: int
    control: int
    value: int
    channel: int = 0


@dataclass
class StemTrack:
    stem_id: str
    file_name: str
    program: int
    channel: int
    notes: list[MidiNote] = field(default_factory=list)
    controls: list[MidiControl] = field(default_factory=list)
    pitch_bends: list[tuple[int, int]] = field(default_factory=list)  # tick, value 0-16383


@dataclass
class ArrangementResult:
    spec: ArrangementSpec
    stems: list[StemTrack]
    ticks_per_beat: int = 480
