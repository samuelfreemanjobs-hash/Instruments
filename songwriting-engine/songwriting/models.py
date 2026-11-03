"""Canonical project representation; musical positions are quarter-note beats."""

from __future__ import annotations

from dataclasses import asdict, dataclass
from typing import Any


@dataclass(frozen=True, slots=True)
class Note:
    pitch: int
    start_beats: float
    duration_beats: float
    velocity: int


@dataclass(frozen=True, slots=True)
class Track:
    name: str
    role: str
    channel: int
    program: int
    notes: tuple[Note, ...]


@dataclass(frozen=True, slots=True)
class ChordEvent:
    symbol: str
    start_beats: float
    duration_beats: float
    root_pitch_class: int
    pitch_classes: tuple[int, ...]
    voicing: tuple[int, ...]
    alternative_voicings: tuple[tuple[int, ...], ...]
    voice_leading_cost: float


@dataclass(frozen=True, slots=True)
class Section:
    kind: str
    start_bar: int
    bars: int


@dataclass(frozen=True, slots=True)
class Project:
    schema_version: int
    title: str
    seed: int
    tempo_bpm: int
    time_signature: tuple[int, int]
    key: str
    scale: str
    swing: float
    style: str
    ppq: int
    sections: tuple[Section, ...]
    chords: tuple[ChordEvent, ...]
    tracks: tuple[Track, ...]

    @property
    def total_bars(self) -> int:
        return sum(section.bars for section in self.sections)

    def to_dict(self) -> dict[str, Any]:
        return asdict(self)

    @classmethod
    def from_dict(cls, data: dict[str, Any]) -> Project:
        sections = tuple(Section(**section) for section in data["sections"])
        chords = tuple(
            ChordEvent(
                **{
                    **chord,
                    "pitch_classes": tuple(chord["pitch_classes"]),
                    "voicing": tuple(chord["voicing"]),
                    "alternative_voicings": tuple(
                        tuple(voicing) for voicing in chord["alternative_voicings"]
                    ),
                }
            )
            for chord in data["chords"]
        )
        tracks = tuple(
            Track(
                **{
                    **track,
                    "notes": tuple(Note(**note) for note in track["notes"]),
                }
            )
            for track in data["tracks"]
        )
        return cls(
            **{
                **data,
                "time_signature": tuple(data["time_signature"]),
                "sections": sections,
                "chords": chords,
                "tracks": tracks,
            }
        )

    def validate(self) -> None:
        if self.schema_version != 1:
            raise ValueError("schema_version must be 1")
        if not self.title.strip():
            raise ValueError("project title cannot be empty")
        if not 20 <= self.tempo_bpm <= 300:
            raise ValueError("tempo_bpm must be between 20 and 300")
        numerator, denominator = self.time_signature
        if numerator < 1 or denominator not in (2, 4, 8, 16):
            raise ValueError("time_signature must use a positive numerator and denominator 2, 4, 8, or 16")
        if not 0 <= self.swing <= 0.5:
            raise ValueError("swing must be between 0 and 0.5")
        if self.ppq < 24:
            raise ValueError("ppq must be at least 24")
        if not self.sections or not self.chords:
            raise ValueError("project must contain sections and chord events")
        expected_bar = 0
        for section in self.sections:
            if section.start_bar != expected_bar or section.bars <= 0:
                raise ValueError("sections must have positive lengths and contiguous start bars")
            expected_bar += section.bars
        track_names = [track.name for track in self.tracks]
        if len(track_names) != len(set(track_names)):
            raise ValueError("track names must be unique")
        beats_per_bar = numerator * 4 / denominator
        for track in self.tracks:
            if not 0 <= track.channel <= 15 or not 0 <= track.program <= 127:
                raise ValueError(f"invalid MIDI channel or program for track {track.name}")
            for note in track.notes:
                if not 0 <= note.pitch <= 127 or not 1 <= note.velocity <= 127:
                    raise ValueError(f"invalid MIDI pitch or velocity in track {track.name}")
                if note.start_beats < 0 or note.duration_beats <= 0:
                    raise ValueError(f"invalid note timing in track {track.name}")
                if note.start_beats + note.duration_beats > self.total_bars * beats_per_bar + 1e-9:
                    raise ValueError(f"note exceeds arrangement in track {track.name}")
