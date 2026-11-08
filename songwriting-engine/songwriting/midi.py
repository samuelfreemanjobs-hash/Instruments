"""Standard MIDI export and structural re-import validation using mido."""

from __future__ import annotations

import json
from collections import Counter
from pathlib import Path
from typing import Any

import mido

from .models import Project
from .timing import beats_to_ticks


def save_project(project: Project, path: str | Path) -> None:
    project.validate()
    destination = Path(path)
    destination.parent.mkdir(parents=True, exist_ok=True)
    destination.write_text(json.dumps(project.to_dict(), indent=2) + "\n", encoding="utf-8")


def load_project(path: str | Path) -> Project:
    try:
        data = json.loads(Path(path).read_text(encoding="utf-8"))
        project = Project.from_dict(data)
        project.validate()
        return project
    except (OSError, json.JSONDecodeError, KeyError, TypeError) as error:
        raise ValueError(f"cannot load project {path}: {error}") from error


def _conductor_events(project: Project) -> list[tuple[int, int, mido.Message | mido.MetaMessage]]:
    events: list[tuple[int, int, mido.Message | mido.MetaMessage]] = [
        (0, -3, mido.MetaMessage("set_tempo", tempo=mido.bpm2tempo(project.tempo_bpm))),
        (0, -2, mido.MetaMessage("time_signature", numerator=project.time_signature[0], denominator=project.time_signature[1])),
    ]
    beats_per_bar = project.time_signature[0] * 4 / project.time_signature[1]
    for section in project.sections:
        tick = beats_to_ticks(section.start_bar * beats_per_bar, project.ppq)
        events.append((tick, -1, mido.MetaMessage("marker", text=section.kind.title())))
    return events


def _track_events(
    project: Project, track_index: int, *, include_name: bool = True
) -> list[tuple[int, int, mido.Message | mido.MetaMessage]]:
    track = project.tracks[track_index]
    events: list[tuple[int, int, mido.Message | mido.MetaMessage]] = []
    if include_name:
        events.append((0, -2, mido.MetaMessage("track_name", name=track.name)))
    events.append((0, -1, mido.Message("program_change", channel=track.channel, program=track.program)))
    for note in track.notes:
        start_tick = beats_to_ticks(note.start_beats, project.ppq)
        end_tick = beats_to_ticks(note.start_beats + note.duration_beats, project.ppq)
        events.append((start_tick, 1, mido.Message("note_on", channel=track.channel, note=note.pitch, velocity=note.velocity)))
        events.append((end_tick, 0, mido.Message("note_off", channel=track.channel, note=note.pitch, velocity=0)))
    for event in track.controllers:
        tick = beats_to_ticks(event.start_beats, project.ppq)
        if event.kind == "cc":
            message = mido.Message("control_change", channel=track.channel, control=event.number, value=event.value)
        else:
            message = mido.Message("pitchwheel", channel=track.channel, pitch=event.value)
        events.append((tick, 0, message))
    return events


def _append_absolute_events(midi_track: mido.MidiTrack, events: list[tuple[int, int, mido.Message | mido.MetaMessage]]) -> None:
    previous_tick = 0
    for tick, _, message in sorted(events, key=lambda item: (item[0], item[1])):
        midi_track.append(message.copy(time=tick - previous_tick))
        previous_tick = tick
    midi_track.append(mido.MetaMessage("end_of_track", time=0))


def export_midi(project: Project, path: str | Path, midi_type: int = 1) -> Path:
    project.validate()
    if midi_type not in (0, 1):
        raise ValueError("MIDI type must be 0 or 1")
    destination = Path(path)
    destination.parent.mkdir(parents=True, exist_ok=True)
    midi_file = mido.MidiFile(type=midi_type, ticks_per_beat=project.ppq)
    conductor = _conductor_events(project)
    if midi_type == 1:
        conductor_track = mido.MidiTrack()
        conductor_track.append(mido.MetaMessage("track_name", name="Conductor"))
        _append_absolute_events(conductor_track, conductor)
        midi_file.tracks.append(conductor_track)
        for index, _ in enumerate(project.tracks):
            midi_track = mido.MidiTrack()
            _append_absolute_events(midi_track, _track_events(project, index))
            midi_file.tracks.append(midi_track)
    else:
        combined = mido.MidiTrack()
        combined.append(mido.MetaMessage("track_name", name=project.title))
        events = list(conductor)
        for index, _ in enumerate(project.tracks):
            events.extend(_track_events(project, index, include_name=False))
        _append_absolute_events(combined, events)
        midi_file.tracks.append(combined)
    midi_file.save(destination)
    return destination


def inspect_midi(path: str | Path) -> dict[str, Any]:
    midi_file = mido.MidiFile(path)
    track_names = []
    notes_by_track: dict[str, int] = {}
    active: dict[tuple[int, int], int] = {}
    unmatched_offs = 0
    unmatched_ons = 0
    tempos = []
    meters = []
    for track in midi_file.tracks:
        name = next((message.name for message in track if message.type == "track_name"), "")
        if name:
            track_names.append(name)
        note_count = 0
        for message in track:
            if message.type == "set_tempo":
                tempos.append(mido.tempo2bpm(message.tempo))
            elif message.type == "time_signature":
                meters.append([message.numerator, message.denominator])
            elif message.type == "note_on" and message.velocity > 0:
                key = (message.channel, message.note)
                active[key] = active.get(key, 0) + 1
                note_count += 1
            elif message.type in ("note_off", "note_on"):
                key = (message.channel, message.note)
                if active.get(key, 0):
                    active[key] -= 1
                else:
                    unmatched_offs += 1
        if name:
            notes_by_track[name] = note_count
    unmatched_ons = sum(active.values())
    return {
        "type": midi_file.type,
        "ticks_per_beat": midi_file.ticks_per_beat,
        "track_names": track_names,
        "notes_by_track": notes_by_track,
        "note_count": sum(notes_by_track.values()),
        "tempos_bpm": [round(value, 5) for value in tempos],
        "time_signatures": meters,
        "unmatched_note_ons": unmatched_ons,
        "unmatched_note_offs": unmatched_offs,
    }


def validate_midi(path: str | Path, project: Project, midi_type: int) -> dict[str, Any]:
    report = inspect_midi(path)
    expected_note_count = sum(len(track.notes) for track in project.tracks)
    expected_tempo = project.tempo_bpm
    required_meter = list(project.time_signature)
    failures = []
    if report["type"] != midi_type:
        failures.append(f"expected MIDI type {midi_type}, got {report['type']}")
    if report["ticks_per_beat"] != project.export.ppq:
        failures.append("MIDI PPQ does not match project")
    if report["note_count"] != expected_note_count:
        failures.append(f"expected {expected_note_count} notes, got {report['note_count']}")
    if not report["tempos_bpm"] or abs(report["tempos_bpm"][0] - expected_tempo) > 0.001:
        failures.append("tempo metadata is missing or incorrect")
    if required_meter not in report["time_signatures"]:
        failures.append("time signature metadata is missing or incorrect")
    if report["unmatched_note_ons"] or report["unmatched_note_offs"]:
        failures.append("MIDI note-on/note-off pairing is unbalanced")
    musical_names = {track.name for track in project.tracks}
    present_names = set(report["track_names"])
    if midi_type == 1 and not musical_names.issubset(present_names):
        failures.append("one or more named musical tracks are missing")
    if midi_type == 0 and len(report["track_names"]) != 1:
        failures.append("Type 0 MIDI must contain one combined track")
    report["valid"] = not failures
    report["failures"] = failures
    return report
