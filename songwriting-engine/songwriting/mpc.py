"""Akai MPC500 export: 96 PPQ sequences, section loops for Song mode, per-track files.

The MPC500 sequencer runs at 96 PPQ and shows at most 16 characters of a file name, so every
file is re-timed to that grid (same-pitch overlaps trimmed) and named ``<CODE>_<PART>.MID``.
"""

from __future__ import annotations

import json
import re
from dataclasses import replace
from pathlib import Path
from typing import Any

from .midi import export_midi, validate_midi
from .models import ChordEvent, ControllerEvent, Note, Project, Section, Track
from .timing import beats_to_ticks, ticks_to_beats

MPC500_PPQ = 96
MPC500_NAME_MAX = 16
_TRACK_CODES = {"pad": "PAD", "electric_piano": "EP", "bass": "BAS", "lead": "LD"}


def mpc_code(project: Project) -> str:
    """e.g. west_coast_smoker @ 95 in D → WCS95D (≤ 8 chars so every file name stays ≤ 16)."""
    initials = "".join(word[0] for word in project.style.split("_") if word).upper()
    key = re.sub(r"[^A-Z]", "", project.key.upper().replace("#", "S"))
    return f"{initials[:3]}{project.tempo_bpm}{key}"[:8]


def requantize(project: Project, ppq: int) -> Project:
    """Snap every note to a coarser grid; same-pitch notes never overlap after snapping."""
    tracks = []
    for track in project.tracks:
        snapped = []
        for note in track.notes:
            start = beats_to_ticks(note.start_beats, ppq)
            end = max(start + 1, beats_to_ticks(note.start_beats + note.duration_beats, ppq))
            snapped.append([note.pitch, start, end, note.velocity])
        snapped.sort(key=lambda item: (item[0], item[1]))
        kept: list[list[int]] = []
        for item in snapped:
            previous = kept[-1] if kept and kept[-1][0] == item[0] else None
            if previous and previous[2] > item[1]:
                previous[2] = item[1]
                if previous[2] <= previous[1]:
                    kept.pop()
            kept.append(item)
        notes = tuple(
            sorted(
                (Note(p, ticks_to_beats(s, ppq), ticks_to_beats(e - s, ppq), v) for p, s, e, v in kept),
                key=lambda n: (n.start_beats, n.pitch),
            )
        )
        controllers = tuple(
            replace(event, start_beats=ticks_to_beats(beats_to_ticks(event.start_beats, ppq), ppq))
            for event in track.controllers
        )
        tracks.append(replace(track, notes=notes, controllers=controllers))
    return replace(project, tracks=tuple(tracks), export=replace(project.export, ppq=ppq))


def slice_section(project: Project, index: int) -> Project:
    """One section as a standalone project starting at beat 0; notes are clipped at the section end."""
    numerator, denominator = project.time_signature
    beats_per_bar = numerator * 4 / denominator
    section = project.sections[index]
    start = section.start_bar * beats_per_bar
    end = start + section.bars * beats_per_bar

    def clip_note(note: Note) -> Note | None:
        if not start <= note.start_beats < end:
            return None
        return Note(note.pitch, note.start_beats - start, min(note.duration_beats, end - note.start_beats), note.velocity)

    def clip_chord(chord: ChordEvent) -> ChordEvent | None:
        chord_start = max(chord.start_beats, start)
        chord_end = min(chord.start_beats + chord.duration_beats, end)
        if chord_end <= chord_start:
            return None
        return replace(chord, start_beats=chord_start - start, duration_beats=chord_end - chord_start)

    def clip_controller(event: ControllerEvent) -> ControllerEvent | None:
        return replace(event, start_beats=event.start_beats - start) if start <= event.start_beats < end else None

    tracks: list[Track] = []
    for track in project.tracks:
        notes = tuple(n for n in map(clip_note, track.notes) if n)
        controllers = tuple(c for c in map(clip_controller, track.controllers) if c)
        tracks.append(replace(track, notes=notes, controllers=controllers))
    return replace(
        project,
        id=f"{project.id}:{index}",
        title=f"{project.title} {section.kind}",
        sections=(Section(section.kind, 0, section.bars, section.variation, section.material),),
        chords=tuple(c for c in map(clip_chord, project.chords) if c),
        tracks=tuple(tracks),
    )


def _write(project: Project, path: Path, midi_type: int, files: list[dict[str, Any]], **info: Any) -> None:
    if len(path.stem) > MPC500_NAME_MAX:
        raise ValueError(f"MPC500 file name {path.stem!r} exceeds {MPC500_NAME_MAX} characters")
    export_midi(project, path, midi_type)
    report = validate_midi(path, project, midi_type)
    if not report["valid"]:
        raise ValueError(f"MPC500 export {path.name} failed validation: {report['failures']}")
    files.append({"file": path.name, "type": midi_type, "notes": report["note_count"], **info})


def export_mpc500(project: Project, out_dir: str | Path, code: str | None = None) -> dict[str, Any]:
    destination = Path(out_dir)
    destination.mkdir(parents=True, exist_ok=True)
    code = re.sub(r"[^A-Z0-9]", "", (code or mpc_code(project)).upper())[:8] or "SONG"
    grid = requantize(project, MPC500_PPQ)
    files: list[dict[str, Any]] = []

    _write(grid, destination / f"{code}_SONG.MID", 1, files, part="song", bars=grid.total_bars)
    for index, section in enumerate(grid.sections):
        name = f"{code}_S{index + 1:02d}{section.kind[:3].upper()}"
        _write(slice_section(grid, index), destination / f"{name}.MID", 1, files, part="section", section=section.kind, bars=section.bars)
    for track in grid.tracks:
        name = f"{code}_{_TRACK_CODES.get(track.role, track.role[:3].upper())}"
        _write(replace(grid, tracks=(track,)), destination / f"{name}.MID", 0, files, part="track", role=track.role, channel=track.channel + 1)

    manifest = {
        "format": "SONGWRITING_MPC500_EXPORT",
        "version": 1,
        "project_id": project.id,
        "title": project.title,
        "code": code,
        "tempo_bpm": project.tempo_bpm,
        "time_signature": list(project.time_signature),
        "ppq": MPC500_PPQ,
        "files": files,
        "tracks": [
            {"role": t.role, "name": t.name, "midi_channel": t.channel + 1, "instrument_hint": t.instrument_hint}
            for t in grid.tracks
        ],
    }
    (destination / "mpc500-export.json").write_text(json.dumps(manifest, indent=2) + "\n", encoding="utf-8")
    (destination / "SONG.TXT").write_text(_song_sheet(grid, code, files), encoding="utf-8", newline="")
    return manifest


def _song_sheet(project: Project, code: str, files: list[dict[str, Any]]) -> str:
    lines = [
        f"{code}  {project.title}",
        f"{project.tempo_bpm} BPM  {project.time_signature[0]}/{project.time_signature[1]}  {project.key} {project.scale.replace('_', ' ')}  {project.total_bars} bars  {MPC500_PPQ} PPQ",
        "",
        "FULL SONG",
        f"  {code}_SONG.MID",
        "",
        "SONG MODE (one sequence per section, in order)",
    ]
    for step, item in enumerate((f for f in files if f["part"] == "section"), start=1):
        lines.append(f"  step {step:02d}  {item['file']:<20} {item['section']:<7} {item['bars']} bars")
    lines += ["", "TRACKS (single-track files; set the MPC track to MIDI out on this channel)"]
    for item, track in zip((f for f in files if f["part"] == "track"), project.tracks):
        lines.append(f"  {item['file']:<16} ch {item['channel']:<2} {track.name:<15} {track.instrument_hint}")
    return "\r\n".join(lines) + "\r\n"
