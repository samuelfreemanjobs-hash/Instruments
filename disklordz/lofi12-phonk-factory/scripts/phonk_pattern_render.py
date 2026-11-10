"""Render audio from sequencer pattern JSON (per-track / stem export)."""

from __future__ import annotations

from phonk_loop_fx import LoopFxParams, apply_loop_fx, fx_dict, fx_from_prompt
from phonk_loop_render import SRC_RATE
from phonk_machine_engine import PROFILES, resolve_engine_id
from phonk_synth import render_slot, resolve_phonk_params

TRACK_STEMS = ("kick", "snare", "hat", "cowbell", "perc", "openhat")
TRACK_VOICES = (
    "kick_808",
    "snare_memphis",
    "hat_closed",
    "cowbell_low",
    "snare_rim",
    "hat_open",
)


def _voice_for_note(track_index: int, note: int) -> str:
    slot = max(1, min(16, note - 35))
    slot_voice = {
        1: "kick_808",
        2: "kick_dist",
        4: "snare_memphis",
        5: "snare_rim",
        6: "clap",
        7: "hat_closed",
        8: "hat_open",
        10: "cowbell_low",
    }
    return slot_voice.get(slot, TRACK_VOICES[track_index])


def render_pattern_stem(
    pattern: dict,
    track_index: int,
    *,
    prompt: str = "memphis phonk",
    engine_id: str | None = None,
    fx: LoopFxParams | None = None,
    variation: int = 0,
) -> tuple[list[float], dict]:
    if track_index < 0 or track_index >= len(TRACK_STEMS):
        raise ValueError("track_index out of range")
    bpm = float(pattern.get("bpm", 84))
    tracks = pattern.get("tracks") or []
    if track_index >= len(tracks):
        raise ValueError("track missing in pattern")
    tr = tracks[track_index]
    steps = tr.get("steps") or []
    steps_per_bar = 16
    total_steps = min(len(steps), steps_per_bar)
    step_samples = int((60.0 / bpm) / 4 * SRC_RATE)
    n = step_samples * steps_per_bar
    out = [0.0] * n

    engine = resolve_engine_id(prompt, engine_id)
    params = resolve_phonk_params(prompt, variation)
    voice_default = TRACK_VOICES[track_index]
    hits = 0

    for si in range(total_steps):
        cell = steps[si]
        if not cell.get("on"):
            continue
        hits += 1
        note = int(cell.get("note", tr.get("defaultNote", 36)))
        vel = max(0.05, min(1.0, int(cell.get("velocity", 100)) / 127.0))
        voice = _voice_for_note(track_index, note) if note else voice_default
        pcm = render_slot(voice, params, engine)
        offset = si * step_samples
        for i, s in enumerate(pcm):
            idx = offset + i
            if idx >= len(out):
                break
            out[idx] += s * vel

    fx_params = fx if fx is not None else fx_from_prompt(prompt)
    out = apply_loop_fx(out, fx_params, seed=params.seed)
    stem = TRACK_STEMS[track_index]
    meta = {
        "bpm": bpm,
        "bars": 1,
        "durationSec": round(len(out) / SRC_RATE, 4),
        "sampleRate": SRC_RATE,
        "hitCount": hits,
        "stem": stem,
        "trackIndex": track_index,
        "trackName": tr.get("name", stem),
        "drumEngine": engine,
        "drumEngineLabel": PROFILES.get(engine, PROFILES["juicy_j"]).label,
        "fx": fx_dict(fx_params),
        "source": "sequencer_pattern",
    }
    return out, meta
