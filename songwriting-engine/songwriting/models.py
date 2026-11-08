"""Canonical project representation; musical positions are quarter-note beats."""

from __future__ import annotations

from dataclasses import asdict, dataclass, field
from typing import Any

SCHEMA_VERSION = 2
VALIDATION_STATUSES = ("unvalidated", "valid", "invalid")


@dataclass(frozen=True, slots=True)
class Note:
    pitch: int
    start_beats: float
    duration_beats: float
    velocity: int


@dataclass(frozen=True, slots=True)
class ControllerEvent:
    """``kind`` is ``"cc"`` (number 0-127, value 0-127) or ``"pitch_bend"`` (value -8192..8191)."""

    kind: str
    start_beats: float
    value: int
    number: int | None = None


@dataclass(frozen=True, slots=True)
class Track:
    name: str
    role: str
    channel: int
    program: int
    notes: tuple[Note, ...]
    id: str = ""
    instrument_hint: str = ""
    muted: bool = False
    solo: bool = False
    controllers: tuple[ControllerEvent, ...] = ()


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
    bass_pitch_class: int | None = None
    function: str = "chord"
    quality: str = ""
    extensions: tuple[str, ...] = ()


@dataclass(frozen=True, slots=True)
class SectionVariation:
    density: float = 1.0
    muted_roles: tuple[str, ...] = ()
    fill: bool = False


@dataclass(frozen=True, slots=True)
class Section:
    kind: str
    start_bar: int
    bars: int
    variation: SectionVariation = field(default_factory=SectionVariation)
    material: str = "main"


@dataclass(frozen=True, slots=True)
class ExportInfo:
    midi_format: int = 1
    ppq: int = 960
    warnings: tuple[str, ...] = ()
    validation_status: str = "unvalidated"


@dataclass(frozen=True, slots=True)
class Project:
    schema_version: int
    id: str
    title: str
    created_at: str | None
    seed: int
    tempo_bpm: int
    time_signature: tuple[int, int]
    key: str
    scale: str
    swing: float
    style: str
    sections: tuple[Section, ...]
    chords: tuple[ChordEvent, ...]
    tracks: tuple[Track, ...]
    export: ExportInfo

    @property
    def ppq(self) -> int:
        return self.export.ppq

    @property
    def total_bars(self) -> int:
        return sum(section.bars for section in self.sections)

    def to_dict(self) -> dict[str, Any]:
        return asdict(self)

    @classmethod
    def from_dict(cls, data: dict[str, Any]) -> Project:
        data = _migrate(data)
        sections = tuple(
            Section(
                **{
                    **section,
                    "variation": SectionVariation(
                        **{
                            **section.get("variation", {}),
                            "muted_roles": tuple(section.get("variation", {}).get("muted_roles", ())),
                        }
                    ),
                }
            )
            for section in data["sections"]
        )
        chords = tuple(
            ChordEvent(
                **{
                    **chord,
                    "pitch_classes": tuple(chord["pitch_classes"]),
                    "voicing": tuple(chord["voicing"]),
                    "alternative_voicings": tuple(
                        tuple(voicing) for voicing in chord["alternative_voicings"]
                    ),
                    "extensions": tuple(chord.get("extensions", ())),
                }
            )
            for chord in data["chords"]
        )
        tracks = tuple(
            Track(
                **{
                    **track,
                    "notes": tuple(Note(**note) for note in track["notes"]),
                    "controllers": tuple(ControllerEvent(**event) for event in track.get("controllers", ())),
                }
            )
            for track in data["tracks"]
        )
        export = data["export"]
        return cls(
            **{
                **data,
                "time_signature": tuple(data["time_signature"]),
                "sections": sections,
                "chords": chords,
                "tracks": tracks,
                "export": ExportInfo(**{**export, "warnings": tuple(export.get("warnings", ()))}),
            }
        )

    def validate(self) -> None:
        if self.schema_version != SCHEMA_VERSION:
            raise ValueError(f"schema_version must be {SCHEMA_VERSION}")
        if not self.id.strip():
            raise ValueError("project id cannot be empty")
        if not self.title.strip():
            raise ValueError("project title cannot be empty")
        if not 20 <= self.tempo_bpm <= 300:
            raise ValueError("tempo_bpm must be between 20 and 300")
        numerator, denominator = self.time_signature
        if numerator < 1 or denominator not in (2, 4, 8, 16):
            raise ValueError("time_signature must use a positive numerator and denominator 2, 4, 8, or 16")
        if not 0 <= self.swing <= 0.5:
            raise ValueError("swing must be between 0 and 0.5")
        if self.export.ppq < 24:
            raise ValueError("ppq must be at least 24")
        if self.export.midi_format not in (0, 1):
            raise ValueError("export.midi_format must be 0 or 1")
        if self.export.validation_status not in VALIDATION_STATUSES:
            raise ValueError(f"export.validation_status must be one of {VALIDATION_STATUSES}")
        if not self.sections or not self.chords:
            raise ValueError("project must contain sections and chord events")
        expected_bar = 0
        for section in self.sections:
            if section.start_bar != expected_bar or section.bars <= 0:
                raise ValueError("sections must have positive lengths and contiguous start bars")
            if not 0 < section.variation.density <= 2:
                raise ValueError("section variation density must be in (0, 2]")
            expected_bar += section.bars
        track_names = [track.name for track in self.tracks]
        if len(track_names) != len(set(track_names)):
            raise ValueError("track names must be unique")
        track_ids = [track.id for track in self.tracks]
        if any(not track_id for track_id in track_ids) or len(track_ids) != len(set(track_ids)):
            raise ValueError("track ids must be non-empty and unique")
        beats_per_bar = numerator * 4 / denominator
        end_beats = self.total_bars * beats_per_bar + 1e-9
        for track in self.tracks:
            if not 0 <= track.channel <= 15 or not 0 <= track.program <= 127:
                raise ValueError(f"invalid MIDI channel or program for track {track.name}")
            for note in track.notes:
                if not 0 <= note.pitch <= 127 or not 1 <= note.velocity <= 127:
                    raise ValueError(f"invalid MIDI pitch or velocity in track {track.name}")
                if note.start_beats < 0 or note.duration_beats <= 0:
                    raise ValueError(f"invalid note timing in track {track.name}")
                if note.start_beats + note.duration_beats > end_beats:
                    raise ValueError(f"note exceeds arrangement in track {track.name}")
            for event in track.controllers:
                if not 0 <= event.start_beats <= end_beats:
                    raise ValueError(f"controller event outside arrangement in track {track.name}")
                if event.kind == "cc":
                    if event.number is None or not 0 <= event.number <= 127 or not 0 <= event.value <= 127:
                        raise ValueError(f"invalid CC event in track {track.name}")
                elif event.kind == "pitch_bend":
                    if not -8192 <= event.value <= 8191:
                        raise ValueError(f"invalid pitch bend in track {track.name}")
                else:
                    raise ValueError(f"unsupported controller kind {event.kind!r} in track {track.name}")


def _migrate(data: dict[str, Any]) -> dict[str, Any]:
    """Upgrade older project documents to the current schema."""
    version = data.get("schema_version")
    if version == SCHEMA_VERSION:
        return data
    if version != 1:
        raise ValueError(f"unsupported project schema_version {version!r}")
    upgraded = {key: value for key, value in data.items() if key != "ppq"}
    upgraded["schema_version"] = SCHEMA_VERSION
    upgraded.setdefault("id", f"legacy-{data['seed']}-{data['title']}")
    upgraded.setdefault("created_at", None)
    upgraded["export"] = {"midi_format": 1, "ppq": data.get("ppq", 960)}
    upgraded["tracks"] = [{**track, "id": track.get("id") or track["role"]} for track in data["tracks"]]
    return upgraded
