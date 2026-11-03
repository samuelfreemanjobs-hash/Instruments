from __future__ import annotations

import json
from pathlib import Path

import pytest

from songwriting.generator import generate_project, load_config
from songwriting.harmony import harmonic_cycle
from songwriting.midi import export_midi, load_project, save_project, validate_midi
from songwriting.prompt import config_from_prompt
from songwriting.theory import (
    MAX_VOICED_TONES,
    choose_voicings,
    parse_chord,
    parse_chord_spec,
    scale_pitch_classes,
    voice_leading_cost,
    voicing_candidates,
    voicing_pitch_classes,
)

ROOT = Path(__file__).resolve().parents[1]
SAMPLE = ROOT / "configs" / "smoker_sketch.json"


def test_scale_and_chord_pitch_classes():
    assert scale_pitch_classes("D", "natural_minor") == (2, 4, 5, 7, 9, 10, 0)
    assert set(parse_chord("Dm9")[1]) == {2, 5, 9, 0, 4}
    assert set(parse_chord("Bbmaj7")[1]) == {10, 2, 5, 9}
    assert set(parse_chord("A7")[1]) == {9, 1, 4, 7}
    assert set(parse_chord("Cadd9")[1]) == {0, 4, 7, 2}


def test_unsupported_chords_fail_explicitly():
    with pytest.raises(ValueError, match="unsupported chord symbol"):
        parse_chord("C7#13")


@pytest.mark.parametrize(
    "symbol, expected",
    [
        ("A7b9", {9, 1, 4, 7, 10}),
        ("C7#9", {0, 4, 7, 10, 3}),
        ("Bm7b5", {11, 2, 5, 9}),
        ("Bø7", {11, 2, 5, 9}),
        ("C#dim7", {1, 4, 7, 10}),
        ("Fmaj7#11", {5, 9, 0, 4, 11}),
        ("CmMaj7", {0, 3, 7, 11}),
        ("G13sus", {7, 0, 2, 4, 5, 9}),
        ("C7sus4", {0, 5, 7, 10}),
        ("C6/9", {0, 4, 7, 9, 2}),
        ("C11", {0, 7, 10, 2, 5}),
        ("Cmaj11", {0, 4, 7, 11, 2, 5}),
        ("E7(b9b13)", {4, 8, 11, 2, 5, 0}),
        ("Dm9/G", {2, 5, 9, 0, 4}),
    ],
)
def test_extended_and_altered_chord_vocabulary(symbol, expected):
    assert set(parse_chord(symbol)[1]) == expected


def test_slash_chord_bass_drives_bass_track():
    spec = parse_chord_spec("Dm9/G")
    assert spec.root == 2 and spec.bass == 7
    config = json.loads(SAMPLE.read_text(encoding="utf-8"))
    config["harmony"]["progression"] = ["Dm9/G", "Cmaj9"]
    project = generate_project(config)
    bass = next(track for track in project.tracks if track.role == "bass")
    first = next(note for note in bass.notes if note.start_beats == 0)
    assert first.pitch % 12 == 7


def test_thick_chords_voice_at_most_five_tones():
    tones = voicing_pitch_classes("Cmaj13#11")
    assert len(tones) == MAX_VOICED_TONES
    assert 7 not in tones and {4, 11, 6, 9}.issubset(tones)
    assert voicing_candidates("Cmaj13#11", 48, 81)


def test_new_scales_and_modes():
    assert scale_pitch_classes("C", "bebop_dominant") == (0, 2, 4, 5, 7, 9, 10, 11)
    assert scale_pitch_classes("E", "phrygian")[1] == 5
    assert config_from_prompt("88 BPM in F lydian, 8 bars.")["music"]["scale"] == "lydian"


def test_voicings_cover_chord_tones_and_respect_range():
    candidates = voicing_candidates("Dm9", 52, 79, max_span=24)
    assert len(candidates) > 1
    for candidate in candidates:
        assert all(52 <= note <= 79 for note in candidate)
        assert set(note % 12 for note in candidate) == set(parse_chord("Dm9")[1])
        assert candidate[-1] - candidate[0] <= 24


def test_rootless_candidates_omit_root_but_keep_extensions():
    candidates = voicing_candidates("Dm9", 52, 84, rootless=True)
    assert candidates
    assert all(2 not in {note % 12 for note in candidate} for candidate in candidates)
    assert all({5, 9, 0, 4}.issubset({note % 12 for note in candidate}) for candidate in candidates)


def test_voice_leading_cost_rewards_common_tones_and_small_motion():
    close = voice_leading_cost((60, 64, 67), (60, 65, 69))
    distant = voice_leading_cost((60, 64, 67), (72, 77, 81))
    assert close < distant
    path, _, total = choose_voicings(("C", "F", "G7"), 48, 78)
    assert len(path) == 3
    assert total >= 0


def test_generation_is_seeded_and_has_four_coordinated_tracks():
    config = load_config(SAMPLE)
    first = generate_project(config)
    second = generate_project(config)
    assert first.to_dict() == second.to_dict()
    assert first.total_bars == 72
    assert len(first.tracks) == 4
    assert len(first.chords) == 72
    assert [event.symbol for event in first.chords[:4]] == ["Dm9", "Gm9", "Bbmaj7", "A7"]
    assert all(track.notes for track in first.tracks)


def test_lead_stays_in_requested_scale():
    project = generate_project(load_config(SAMPLE))
    scale_pcs = set(scale_pitch_classes(project.key, project.scale))
    lead = next(track for track in project.tracks if track.role == "lead")
    assert lead.notes
    assert all(note.pitch % 12 in scale_pcs for note in lead.notes)


def test_project_json_round_trip(tmp_path):
    project = generate_project(load_config(SAMPLE))
    path = tmp_path / "project.json"
    save_project(project, path)
    assert load_project(path) == project


@pytest.mark.parametrize("midi_type", [0, 1])
def test_export_reimports_with_valid_tracks_metadata_and_note_pairs(tmp_path, midi_type):
    project = generate_project(load_config(SAMPLE))
    path = export_midi(project, tmp_path / f"song-{midi_type}.mid", midi_type)
    report = validate_midi(path, project, midi_type)
    assert report["valid"], report["failures"]
    assert report["note_count"] == sum(len(track.notes) for track in project.tracks)
    if midi_type == 1:
        assert {track.name for track in project.tracks}.issubset(report["track_names"])
        assert len([name for name in report["track_names"] if name != "Conductor"]) == 4
    else:
        assert len(report["track_names"]) == 1


def test_invalid_configuration_is_rejected():
    config = json.loads(SAMPLE.read_text(encoding="utf-8"))
    config["harmony"]["progression"] = ["C7#13"]
    with pytest.raises(ValueError, match="unsupported chord symbol"):
        generate_project(config)
    config["harmony"]["progression"] = ["Dm9"]
    config["harmony"]["reharm"] = {"modal_interchange": True}
    with pytest.raises(ValueError, match="unsupported harmony.reharm"):
        generate_project(config)


def test_reharm_inserts_resolving_approach_chords():
    reharm = {"secondary_dominants": True, "tritone_subs": False, "passing_chords": True}
    cycle = harmonic_cycle(("Cmaj9", "Dm9", "G13", "Cmaj9"), 1, 4.0, reharm)
    assert [(c.symbol, c.offset_beats, c.duration_beats, c.function) for c in cycle] == [
        ("Cmaj9", 0.0, 3.0, "chord"),
        ("C#dim7", 3.0, 1.0, "passing_dim"),
        ("Dm9", 4.0, 3.0, "chord"),
        ("D7", 7.0, 1.0, "secondary_dominant"),
        ("G13", 8.0, 4.0, "chord"),
        ("Cmaj9", 12.0, 4.0, "chord"),
    ]
    tritone = harmonic_cycle(("Dm9", "Cmaj9"), 1, 4.0, {"secondary_dominants": False, "tritone_subs": True, "passing_chords": False})
    assert [c.symbol for c in tritone] == ["Dm9", "Db9", "Cmaj9", "Eb9"]


def test_reharm_off_matches_plain_progression():
    config = load_config(SAMPLE)
    plain = generate_project(config)
    config["harmony"]["reharm"] = {"secondary_dominants": False, "tritone_subs": False, "passing_chords": False}
    assert generate_project(config).to_dict() == plain.to_dict()


def test_gospel_prompt_reharmonizes_and_exports(tmp_path):
    config = config_from_prompt("72 BPM gospel in Eb major. Chords: Ebmaj9, Fm9, Bb13, Ebmaj9. 8 bars.", seed=7)
    assert all(config["harmony"]["reharm"].values())
    project = generate_project(config)
    functions = {chord.function for chord in project.chords}
    assert {"chord", "passing_dim"}.issubset(functions) and functions & {"tritone_sub", "secondary_dominant"}
    path = export_midi(project, tmp_path / "gospel.mid", 1)
    assert validate_midi(path, project, 1)["valid"]
    plain = config_from_prompt("72 BPM gospel in Eb major, no reharm. Chords: Ebmaj9, Fm9. 8 bars.")
    assert not any(plain["harmony"]["reharm"].values())


def test_prompt_generation_extracts_music_settings_and_harmony():
    config = config_from_prompt(
        "95 BPM, D minor, smoky West Coast. Chords: Dm9, Gm9, Bbmaj7, A7. 32 bars.",
        seed=1234,
    )
    assert config["music"]["tempo_bpm"] == 95
    assert config["music"]["key"] == "D"
    assert config["music"]["scale"] == "natural_minor"
    assert config["harmony"]["progression"] == ["Dm9", "Gm9", "Bbmaj7", "A7"]
    assert config["project"]["seed"] == 1234
    project = generate_project(config)
    assert project.total_bars == 32
    assert project.sections[-1].kind == "outro"


def test_prompt_rejects_unsupported_chord_tokens():
    with pytest.raises(ValueError, match="unsupported chord symbol"):
        config_from_prompt("Make a progression: C7#13, Dm7")


def test_prompt_accepts_altered_chords():
    config = config_from_prompt("95 BPM D minor smoker. Chords: Dm9, G13, Bbmaj7#11, A7b9. 16 bars.")
    assert config["harmony"]["progression"] == ["Dm9", "G13", "Bbmaj7#11", "A7b9"]
    assert generate_project(config).total_bars == 16


def test_prompt_preserves_flat_key_spelling():
    config = config_from_prompt("82 BPM, Bb minor quiet storm, 16 bars.")
    assert config["music"]["key"] == "Bb"
    assert config["music"]["scale"] == "natural_minor"
