"""Parametric trap / Prophet-Rev2-inspired multisample source (clean-room synthesis)."""

from __future__ import annotations

import math
from dataclasses import dataclass
from typing import Literal

SAMPLE_RATE = 44100

ProducerLane = Literal["jeezy", "shawty_redd", "gucci", "neutral"]


@dataclass(frozen=True)
class SynthPatch:
    osc1_wave: Literal["saw", "square", "sine"] = "saw"
    osc2_wave: Literal["saw", "square", "sine"] = "saw"
    osc2_detune_cents: float = 7.0
    osc_mix: float = 0.55
    sub_level: float = 0.35
    cutoff_hz: float = 2200.0
    resonance: float = 0.25
    filter_env_amount: float = 0.45
    attack_s: float = 0.008
    decay_s: float = 0.35
    sustain: float = 0.55
    release_s: float = 0.4
    drive: float = 1.0


LANE_PATCH: dict[ProducerLane, SynthPatch] = {
    "jeezy": SynthPatch(
        osc1_wave="saw",
        osc2_wave="square",
        osc2_detune_cents=12.0,
        cutoff_hz=2800.0,
        resonance=0.18,
        filter_env_amount=0.55,
        decay_s=0.42,
        sustain=0.5,
        sub_level=0.4,
    ),
    "shawty_redd": SynthPatch(
        osc1_wave="square",
        osc2_wave="square",
        osc2_detune_cents=18.0,
        cutoff_hz=3400.0,
        resonance=0.32,
        filter_env_amount=0.65,
        attack_s=0.002,
        decay_s=0.28,
        sustain=0.35,
        drive=1.15,
    ),
    "gucci": SynthPatch(
        osc1_wave="saw",
        osc2_wave="saw",
        osc2_detune_cents=-14.0,
        cutoff_hz=1600.0,
        resonance=0.42,
        filter_env_amount=0.5,
        decay_s=0.55,
        sustain=0.62,
        sub_level=0.5,
    ),
    "neutral": SynthPatch(),
}


def _osc(wave: str, phase: float) -> float:
    if wave == "sine":
        return math.sin(phase)
    if wave == "square":
        return 1.0 if math.sin(phase) >= 0 else -1.0
    # saw
    return 2.0 * (phase / (2 * math.pi) - math.floor(phase / (2 * math.pi) + 0.5))


def midi_to_hz(note: int) -> float:
    return 440.0 * (2.0 ** ((note - 69) / 12.0))


def render_note(
    midi_note: int,
    velocity: int,
    duration_s: float,
    patch: SynthPatch,
    seed: int = 0,
) -> list[float]:
    n = max(1, int(SAMPLE_RATE * duration_s))
    freq = midi_to_hz(midi_note)
    freq2 = freq * (2.0 ** (patch.osc2_detune_cents / 1200.0))
    vel = max(0.05, velocity / 127.0)
    out: list[float] = []
    phase1 = phase2 = 0.0
    filter_state = 0.0
    env = 0.0
    env_stage = "attack"
    release_start = duration_s - patch.release_s

    for i in range(n):
        t = i / SAMPLE_RATE
        if env_stage == "attack":
            env += 1.0 / max(1, int(patch.attack_s * SAMPLE_RATE))
            if env >= 1.0:
                env = 1.0
                env_stage = "decay"
        elif env_stage == "decay":
            env -= (1.0 - patch.sustain) / max(1, int(patch.decay_s * SAMPLE_RATE))
            if env <= patch.sustain:
                env = patch.sustain
                env_stage = "sustain"
        elif env_stage == "sustain" and t >= release_start:
            env_stage = "release"
        if env_stage == "release":
            env -= patch.sustain / max(1, int(patch.release_s * SAMPLE_RATE))
            if env < 0:
                env = 0.0

        inc1 = 2 * math.pi * freq / SAMPLE_RATE
        inc2 = 2 * math.pi * freq2 / SAMPLE_RATE
        phase1 += inc1
        phase2 += inc2
        raw = patch.osc_mix * (_osc(patch.osc1_wave, phase1) + _osc(patch.osc2_wave, phase2) * 0.5)
        raw += patch.sub_level * math.sin(phase1 * 0.5)
        raw *= patch.drive * vel

        cutoff = patch.cutoff_hz * (1.0 + patch.filter_env_amount * env)
        alpha = min(0.99, cutoff / (cutoff + SAMPLE_RATE))
        filter_state = alpha * filter_state + (1 - alpha) * raw
        out.append(filter_state * env)

    # tiny seed-based drift for uniqueness without noise hash
    if seed:
        for i in range(len(out)):
            out[i] *= 1.0 + 0.0001 * math.sin(i * 0.01 + seed)

    return out


def default_multisample_roots(low: int = 36, high: int = 84, step: int = 3) -> list[int]:
    roots = list(range(low, high + 1, step))
    return roots if roots else [60]
