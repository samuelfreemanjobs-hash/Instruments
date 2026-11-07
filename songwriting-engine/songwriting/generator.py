"""Configuration loading and deterministic four-track composition pipeline."""

from __future__ import annotations

import json
import random
import uuid
from pathlib import Path
from typing import Any

from .harmony import harmonic_cycle, validate_reharm
from .models import SCHEMA_VERSION, ChordEvent, ExportInfo, Note, Project, Section, Track
from .timing import beats_to_ticks, ticks_to_beats
from .theory import (
    SCALES,
    choose_voicings,
    parse_chord,
    parse_chord_spec,
    pitch_class,
    scale_pitch_classes,
    voicing_candidates,
)

STYLE_PROFILES = {
    "west_coast_smoker": {
        "pad_range": (52, 79), "ep_range": (60, 84), "swing": 0.12,
        "ep_density": 0.52, "lead_density": 0.38, "bass_fifth_probability": 0.28,
        "rootless_ep": True,
    },
    "quiet_storm": {
        "pad_range": (48, 84), "ep_range": (58, 82), "swing": 0.07,
        "ep_density": 0.42, "lead_density": 0.3, "bass_fifth_probability": 0.2,
        "rootless_ep": True,
    },
    "gospel_rnb": {
        "pad_range": (48, 81), "ep_range": (57, 84), "swing": 0.1,
        "ep_density": 0.7, "lead_density": 0.52, "bass_fifth_probability": 0.38,
        "rootless_ep": True,
    },
    "dusty_jazz_rap": {
        "pad_range": (50, 78), "ep_range": (59, 82), "swing": 0.16,
        "ep_density": 0.4, "lead_density": 0.3, "bass_fifth_probability": 0.24,
        "rootless_ep": True,
    },
    "screw_half_time": {
        "pad_range": (48, 78), "ep_range": (58, 80), "swing": 0.18,
        "ep_density": 0.32, "lead_density": 0.22, "bass_fifth_probability": 0.2,
        "rootless_ep": False,
    },
}

TRACK_LAYOUT = {
    "pad": ("Poly Pad", "DX7-style poly pad"),
    "electric_piano": ("Electric Piano", "FM electric piano (DX7 E.Piano)"),
    "bass": ("Synth Bass", "Moog-style sub bass"),
    "lead": ("Lead", "Poly brass / FM bell lead"),
}
_PROJECT_NAMESPACE = uuid.UUID("6f1f4a52-2d0e-4a8e-9a43-4b9c5a7e1d10")

DEFAULT_SECTIONS = [
    {"type": "intro", "bars": 8},
    {"type": "verse", "bars": 16},
    {"type": "hook", "bars": 8},
    {"type": "verse", "bars": 16},
    {"type": "hook", "bars": 8},
    {"type": "bridge", "bars": 8},
    {"type": "outro", "bars": 8},
]


def load_config(path: str | Path) -> dict[str, Any]:
    try:
        with Path(path).open("r", encoding="utf-8") as handle:
            config = json.load(handle)
    except (OSError, json.JSONDecodeError) as error:
        raise ValueError(f"cannot read configuration {path}: {error}") from error
    validate_config(config)
    return config


def validate_config(config: dict[str, Any]) -> None:
    for field in ("project", "music", "style", "harmony", "arrangement", "tracks", "export"):
        if not isinstance(config.get(field), dict):
            raise ValueError(f"configuration requires an object named {field!r}")
    project, music, style = config["project"], config["music"], config["style"]
    if not isinstance(project.get("title"), str) or not project["title"].strip():
        raise ValueError("project.title must be a non-empty string")
    if not isinstance(project.get("seed"), int):
        raise ValueError("project.seed must be an integer")
    tempo = music.get("tempo_bpm")
    if not isinstance(tempo, int) or not 20 <= tempo <= 300:
        raise ValueError("music.tempo_bpm must be an integer from 20 to 300")
    meter = music.get("time_signature")
    if not isinstance(meter, list) or len(meter) != 2 or meter[0] < 1 or meter[1] not in (2, 4, 8, 16):
        raise ValueError("music.time_signature must be [numerator, 2|4|8|16]")
    scale_pitch_classes(music.get("key", ""), music.get("scale", ""))
    swing = music.get("swing", 0.0)
    if not isinstance(swing, (int, float)) or not 0 <= swing <= 0.5:
        raise ValueError("music.swing must be between 0 and 0.5")
    profile_name = style.get("profile")
    if profile_name not in STYLE_PROFILES:
        raise ValueError(f"unsupported style profile: {profile_name!r}")
    progression = config["harmony"].get("progression")
    if not isinstance(progression, list) or not progression:
        raise ValueError("harmony.progression must be a non-empty list of supported chord symbols")
    for symbol in progression:
        parse_chord(symbol)
    bars_per_chord = config["harmony"].get("bars_per_chord", 1)
    if not isinstance(bars_per_chord, int) or bars_per_chord < 1:
        raise ValueError("harmony.bars_per_chord must be a positive integer")
    validate_reharm(config["harmony"].get("reharm"))
    sections = config["arrangement"].get("sections", DEFAULT_SECTIONS)
    if not sections or any(not isinstance(section.get("bars"), int) or section["bars"] <= 0 for section in sections):
        raise ValueError("arrangement.sections must contain positive integer bar lengths")
    requested = config["tracks"]
    if not any(requested.get(name, False) for name in ("pad", "electric_piano", "bass", "lead")):
        raise ValueError("at least one of pad, electric_piano, bass, or lead must be enabled")
    ppq = config["export"].get("midi_ppq", 960)
    if not isinstance(ppq, int) or ppq < 24:
        raise ValueError("export.midi_ppq must be an integer of at least 24")


def _nearest_pitch(pc: int, low: int, high: int, target: int) -> int:
    options = [pitch for pitch in range(low, high + 1) if pitch % 12 == pc]
    if not options:
        raise ValueError(f"pitch class {pc} has no note in range {low}..{high}")
    return min(options, key=lambda pitch: (abs(pitch - target), pitch))


def _make_sections(config: dict[str, Any]) -> tuple[Section, ...]:
    sections = config["arrangement"].get("sections", DEFAULT_SECTIONS)
    result = []
    start_bar = 0
    for section in sections:
        result.append(Section(section["type"], start_bar, section["bars"]))
        start_bar += section["bars"]
    return tuple(result)


def project_id(config: dict[str, Any]) -> str:
    """Same configuration (including seed) → same id."""
    return str(uuid.uuid5(_PROJECT_NAMESPACE, json.dumps(config, sort_keys=True, separators=(",", ":"))))


def _on_tick_grid(notes: list[Note], ppq: int) -> tuple[Note, ...]:
    snapped = []
    for note in notes:
        start = beats_to_ticks(note.start_beats, ppq)
        end = max(start + 1, beats_to_ticks(note.start_beats + note.duration_beats, ppq))
        snapped.append(Note(note.pitch, ticks_to_beats(start, ppq), ticks_to_beats(end - start, ppq), note.velocity))
    return tuple(sorted(snapped, key=lambda n: (n.start_beats, n.pitch)))


def _track(role: str, channel: int, program: int, notes: list[Note], ppq: int) -> Track:
    name, hint = TRACK_LAYOUT[role]
    return Track(name, role, channel, program, _on_tick_grid(notes, ppq), id=role, instrument_hint=hint)


def generate_project(config: dict[str, Any], *, created_at: str | None = None) -> Project:
    validate_config(config)
    project_config = config["project"]
    music = config["music"]
    style_config = config["style"]
    harmony = config["harmony"]
    export = config["export"]
    profile = STYLE_PROFILES[style_config["profile"]]
    rng = random.Random(project_config["seed"])
    sections = _make_sections(config)
    total_bars = sum(section.bars for section in sections)
    numerator, denominator = music["time_signature"]
    beats_per_bar = numerator * 4 / denominator
    progression = tuple(harmony["progression"])
    bars_per_chord = harmony.get("bars_per_chord", 1)
    reharm = validate_reharm(harmony.get("reharm"))
    pad_low, pad_high = profile["pad_range"]
    total_beats = total_bars * beats_per_bar
    cycle = harmonic_cycle(
        progression, bars_per_chord, beats_per_bar, reharm, gospel=style_config["profile"] == "gospel_rnb"
    )
    cycle_beats = len(progression) * bars_per_chord * beats_per_bar
    voicing_path, transition_costs, _ = choose_voicings(tuple(c.symbol for c in cycle), pad_low, pad_high)
    candidates_by_symbol = {c.symbol: voicing_candidates(c.symbol, pad_low, pad_high) for c in cycle}

    chord_events: list[ChordEvent] = []
    cycle_start = 0.0
    while cycle_start < total_beats:
        for index, item in enumerate(cycle):
            start = cycle_start + item.offset_beats
            if start >= total_beats:
                break
            spec = parse_chord_spec(item.symbol)
            chord_events.append(
                ChordEvent(
                    symbol=item.symbol,
                    start_beats=start,
                    duration_beats=min(item.duration_beats, total_beats - start),
                    root_pitch_class=spec.root,
                    pitch_classes=spec.pitch_classes,
                    voicing=voicing_path[index],
                    alternative_voicings=tuple(candidates_by_symbol[item.symbol][:8]),
                    voice_leading_cost=transition_costs[index],
                    bass_pitch_class=spec.bass,
                    function=item.function,
                    quality=spec.quality,
                    extensions=spec.extensions,
                )
            )
        cycle_start += cycle_beats
    ppq = export.get("midi_ppq", 960)
    warnings = []
    if total_beats % cycle_beats:
        warnings.append(
            f"arrangement of {total_bars} bars ends mid-progression "
            f"({len(progression) * bars_per_chord}-bar chord loop)"
        )
    scale_pcs = set(scale_pitch_classes(music["key"], music["scale"]))
    density = float(style_config.get("rhythmic_density", 0.5))
    ep_density = profile["ep_density"] * density * 2
    lead_density = profile["lead_density"] * float(style_config.get("melodic_density", 0.5)) * 2
    swing = float(music.get("swing", profile["swing"]))
    track_data: list[Track] = []

    if config["tracks"].get("pad", True):
        notes = []
        for chord in chord_events:
            duration = chord.duration_beats * (0.94 if chord.duration_beats <= beats_per_bar else 0.97)
            for pitch in chord.voicing:
                notes.append(Note(pitch, chord.start_beats, duration, rng.randint(58, 76)))
        track_data.append(_track("pad", 0, 89, notes, ppq))

    if config["tracks"].get("electric_piano", True):
        notes = []
        for chord in chord_events:
            bar_count = max(1, round(chord.duration_beats / beats_per_bar))
            for local_bar in range(bar_count):
                bar_start = chord.start_beats + local_bar * beats_per_bar
                hits = [(0.0, 0.68), (2.5, 0.42)]
                for beat, probability in hits:
                    if beat >= beats_per_bar or rng.random() > min(0.95, probability * ep_density):
                        continue
                    swing_offset = swing * 0.25 if beat % 1 == 0.5 else 0.0
                    start = bar_start + beat + swing_offset
                    remaining = min(chord.start_beats + chord.duration_beats, total_beats) - start
                    if remaining <= 0:
                        continue
                    tones = [pitch for pitch in chord.voicing if pitch % 12 != chord.root_pitch_class]
                    if not tones:
                        tones = list(chord.voicing)
                    for pitch in tones[: min(4, len(tones))]:
                        ep_pitch = pitch + 12
                        if ep_pitch > 96:
                            ep_pitch = pitch
                        notes.append(Note(ep_pitch, start, min(0.42, remaining), rng.randint(62, 96)))
        track_data.append(_track("electric_piano", 1, 4, notes, ppq))

    if config["tracks"].get("bass", True):
        notes = []
        for chord in chord_events:
            bass_pc = chord.root_pitch_class if chord.bass_pitch_class is None else chord.bass_pitch_class
            root_pitch = _nearest_pitch(bass_pc, 30, 48, 38)
            notes.append(Note(root_pitch, chord.start_beats, min(1.65, chord.duration_beats), rng.randint(82, 105)))
            if chord.duration_beats >= beats_per_bar and rng.random() < profile["bass_fifth_probability"]:
                fifth = (chord.root_pitch_class + 7) % 12
                fifth_pitch = _nearest_pitch(fifth, 30, 48, 38)
                bass_start = chord.start_beats + min(2.0, beats_per_bar / 2)
                notes.append(Note(fifth_pitch, bass_start, min(0.72, chord.start_beats + chord.duration_beats - bass_start), rng.randint(72, 94)))
        track_data.append(_track("bass", 2, 38, notes, ppq))

    if config["tracks"].get("lead", True):
        notes = []
        for chord in chord_events:
            root, chord_pcs = parse_chord(chord.symbol)
            in_scale_chord_tones = [pc for pc in chord_pcs if pc in scale_pcs]
            pool = in_scale_chord_tones or [pc for pc in chord_pcs if pc in scale_pcs]
            if not pool:
                pool = sorted(scale_pcs)
            bar_count = max(1, round(chord.duration_beats / beats_per_bar))
            for local_bar in range(bar_count):
                bar_start = chord.start_beats + local_bar * beats_per_bar
                for beat in (0.5, 1.5, 3.0):
                    if beat >= beats_per_bar or rng.random() > min(0.95, lead_density):
                        continue
                    pc = rng.choice(pool)
                    pitch = _nearest_pitch(pc, 67, 86, 76)
                    start = bar_start + beat + (swing * 0.25 if beat % 1 == 0.5 else 0.0)
                    remaining = min(0.62, chord.start_beats + chord.duration_beats - start)
                    if remaining > 0:
                        notes.append(Note(pitch, start, remaining, rng.randint(55, 88)))
        track_data.append(_track("lead", 3, 81, notes, ppq))

    result = Project(
        schema_version=SCHEMA_VERSION,
        id=project_id(config),
        title=project_config["title"],
        created_at=created_at,
        seed=project_config["seed"],
        tempo_bpm=music["tempo_bpm"],
        time_signature=(numerator, denominator),
        key=music["key"],
        scale=music["scale"],
        swing=swing,
        style=style_config["profile"],
        sections=sections,
        chords=tuple(chord_events),
        tracks=tuple(track_data),
        export=ExportInfo(midi_format=export.get("format", 1), ppq=ppq, warnings=tuple(warnings)),
    )
    result.validate()
    return result
