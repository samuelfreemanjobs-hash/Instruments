from __future__ import annotations

import json
from dataclasses import replace
from pathlib import Path

import mido

from songwriting.cli import main as cli_main
from songwriting.generator import generate_project, load_config
from songwriting.models import Note
from songwriting.mpc import (
    MPC500_NAME_MAX,
    MPC500_PPQ,
    export_mpc500,
    mpc_code,
    requantize,
    slice_section,
)
from songwriting.prompt import config_from_prompt
from songwriting.timing import beats_to_ticks, ticks_to_beats

ROOT = Path(__file__).resolve().parents[1]
SAMPLE = ROOT / "configs" / "smoker_sketch.json"


def _project():
    return generate_project(load_config(SAMPLE))


def test_code_and_names_fit_mpc500_limits(tmp_path):
    project = _project()
    assert mpc_code(project) == "WCS95D"
    manifest = export_mpc500(project, tmp_path)
    names = [Path(f["file"]).stem for f in manifest["files"]]
    assert all(len(name) <= MPC500_NAME_MAX for name in names)
    assert len(names) == len(set(names))
    assert {f["part"] for f in manifest["files"]} == {"song", "section", "track"}
    assert len([f for f in manifest["files"] if f["part"] == "section"]) == len(project.sections)
    sheet = (tmp_path / "SONG.TXT").read_bytes()
    assert sheet.startswith(b"WCS95D") and b"\r\r\n" not in sheet and b"\r\n" in sheet


def test_requantize_lands_on_96_ppq_without_same_pitch_overlap():
    grid = requantize(_project(), MPC500_PPQ)
    assert grid.ppq == MPC500_PPQ
    for track in grid.tracks:
        by_pitch: dict[int, list[Note]] = {}
        for note in track.notes:
            for beats in (note.start_beats, note.duration_beats):
                assert ticks_to_beats(beats_to_ticks(beats, MPC500_PPQ), MPC500_PPQ) == beats
            by_pitch.setdefault(note.pitch, []).append(note)
        for notes in by_pitch.values():
            notes.sort(key=lambda n: n.start_beats)
            for first, second in zip(notes, notes[1:]):
                assert first.start_beats + first.duration_beats <= second.start_beats + 1e-9


def test_requantize_trims_colliding_notes():
    project = _project()
    lead = project.tracks[3]
    crowded = (Note(72, 0.0, 1.0, 90), Note(72, 0.004, 0.5, 80), Note(72, 0.5, 0.5, 70))
    grid = requantize(replace(project, tracks=project.tracks[:3] + (replace(lead, notes=crowded),)), MPC500_PPQ)
    notes = grid.tracks[3].notes
    assert [n.start_beats for n in notes] == [0.0, 0.5]
    assert notes[0].duration_beats == 0.5


def test_sections_partition_the_song():
    grid = requantize(_project(), MPC500_PPQ)
    total = sum(len(t.notes) for t in grid.tracks)
    sliced = [slice_section(grid, i) for i in range(len(grid.sections))]
    assert sum(len(t.notes) for s in sliced for t in s.tracks) == total
    for section, part in zip(grid.sections, sliced):
        part.validate()
        assert part.total_bars == section.bars and part.sections[0].start_bar == 0


def test_every_file_reimports_at_96_ppq(tmp_path):
    manifest = export_mpc500(_project(), tmp_path)
    for item in manifest["files"]:
        midi = mido.MidiFile(tmp_path / item["file"])
        assert midi.ticks_per_beat == MPC500_PPQ and midi.type == item["type"]
    track_files = [f for f in manifest["files"] if f["part"] == "track"]
    assert [f["channel"] for f in track_files] == [1, 2, 3, 4]


def test_gospel_reharm_song_exports(tmp_path):
    config = config_from_prompt("72 BPM gospel in Eb major. Chords: Ebmaj9, Fm9, Bb13, Ebmaj9. 16 bars.", seed=3)
    manifest = export_mpc500(generate_project(config), tmp_path, code="church72")
    assert manifest["code"] == "CHURCH72"
    assert all(f["file"].startswith("CHURCH72_") for f in manifest["files"])


def test_cli_export_mpc500(tmp_path):
    out = tmp_path / "gen"
    assert cli_main(["generate", "--config", str(SAMPLE), "--output-dir", str(out), "--mpc500-dir", str(tmp_path / "mpc")]) == 0
    assert (tmp_path / "mpc" / "WCS95D_SONG.MID").exists()
    assert cli_main(["export-mpc500", "--project", str(out / "Smoker_Sketch_001.json"), "--out", str(tmp_path / "again")]) == 0
    assert json.loads((tmp_path / "again" / "mpc500-export.json").read_text(encoding="utf-8"))["ppq"] == MPC500_PPQ
