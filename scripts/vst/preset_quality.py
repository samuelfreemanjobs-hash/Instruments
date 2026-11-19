#!/usr/bin/env python3
"""Quality scoring for VST Preset Agent daily batches — only promote chart-grade trap patches."""

from __future__ import annotations

import math
from dataclasses import dataclass
from typing import Any, Callable

MIN_DAILY_SCORE = 88.0

# Slots per daily run (8 total per synth) — fixed roles, not random filler.
WAVE_DAILY_ROLES = (
    "trap_808",
    "trap_808",
    "trap_pluck",
    "trap_pluck",
    "trap_reese",
    "trap_lead",
    "trap_phonk",
    "trap_drill_stab",
)

PLUGG_DAILY_ROLES = (
    "trap_808",
    "trap_reese",
    "trap_pluck",
    "trap_pluck",
    "trap_supersaw",
    "trap_lead",
    "trap_stab",
    "trap_glide_lead",
)

WAVE_GOLDEN: dict[str, dict[str, Any]] = {
    "trap_808": {
        "wtPosition": 0.59,
        "filterCutoff": 0.27,
        "filterRes": 0.12,
        "subLevel": 0.98,
        "drive": 0.42,
        "crush": 0.18,
        "ampAttack": 0.002,
        "ampDecay": 0.48,
        "ampSustain": 0.96,
        "ampRelease": 0.32,
        "glideMs": 95.0,
    },
    "trap_pluck": {
        "wtPosition": 0.73,
        "filterCutoff": 0.67,
        "filterRes": 0.38,
        "filtEnvAmt": 0.84,
        "ampAttack": 0.001,
        "ampDecay": 0.085,
        "ampSustain": 0.0,
        "ampRelease": 0.26,
        "glideMs": 34.0,
        "stereoWidth": 0.22,
    },
    "trap_reese": {
        "wtPosition": 0.77,
        "detuneCents": -11.0,
        "filterCutoff": 0.41,
        "filterRes": 0.35,
        "drive": 0.48,
        "subLevel": 0.55,
        "ampDecay": 0.42,
        "ampSustain": 0.72,
    },
    "trap_lead": {
        "wtPosition": 0.71,
        "wtScan": 0.18,
        "filterCutoff": 0.58,
        "glideMs": 105.0,
        "tapeWobble": 0.12,
        "stereoWidth": 0.38,
        "ampSustain": 0.68,
    },
    "trap_phonk": {
        "wtPosition": 0.66,
        "filterCutoff": 0.38,
        "crush": 0.45,
        "drive": 0.64,
        "filterDestructive": True,
        "tapeWobble": 0.22,
        "subLevel": 0.62,
    },
    "trap_drill_stab": {
        "wtPosition": 0.84,
        "filterCutoff": 0.54,
        "filterRes": 0.52,
        "ampDecay": 0.13,
        "ampSustain": 0.0,
        "drive": 0.44,
        "filtEnvAmt": 0.72,
    },
}

PLUGG_GOLDEN: dict[str, dict[str, Any]] = {
    "trap_808": {
        "osc1Wave": "saw",
        "osc2Wave": "saw",
        "subLevel": 0.92,
        "filterCutoff": 0.33,
        "drive": 0.4,
        "ampDecay": 0.5,
        "ampSustain": 0.88,
        "glideMs": 120.0,
    },
    "trap_reese": {
        "osc1Wave": "saw",
        "osc2Wave": "saw",
        "unisonAmount": 0.88,
        "osc2DetuneCents": -16.0,
        "unisonSpreadCents": 28.0,
        "subLevel": 0.68,
        "filterCutoff": 0.38,
        "drive": 0.35,
    },
    "trap_pluck": {
        "osc2Octave": 1,
        "osc2DetuneCents": 10.0,
        "filtEnvAmt": 0.82,
        "ampDecay": 0.078,
        "ampSustain": 0.0,
        "ampRelease": 0.3,
        "glideMs": 32.0,
        "chorus": 0.28,
    },
    "trap_supersaw": {
        "osc1Wave": "saw",
        "osc2Wave": "saw",
        "unisonAmount": 0.92,
        "unisonSpreadCents": 30.0,
        "chorus": 0.48,
        "filterCutoff": 0.55,
        "drive": 0.28,
        "glideMs": 75.0,
    },
    "trap_lead": {
        "unisonAmount": 0.55,
        "chorus": 0.52,
        "stereoWidth": 0.42,
        "glideMs": 115.0,
        "drift": 0.2,
        "filterCutoff": 0.72,
    },
    "trap_stab": {
        "drive": 0.52,
        "filterRes": 0.48,
        "ampDecay": 0.14,
        "ampSustain": 0.04,
        "filtEnvAmt": 0.86,
        "crush": 0.15,
    },
    "trap_glide_lead": {
        "unisonAmount": 0.48,
        "glideMs": 145.0,
        "filterCutoff": 0.6,
        "chorus": 0.36,
        "ampSustain": 0.62,
    },
}

NAME_PARTS = {
    "trap_808": (
        ("Southside", "Metro", "808 Mafia", "TM88", "Cassius", "Wheezy", "Tay Keith", "London", "Hard"),
        ("Sub", "Weight", "Bounce", "Tail", "Glide", "Knock", "Low", "Chest"),
    ),
    "trap_pluck": (
        ("Zaytoven", "Plugg", "Carti", "Nardo", "Pi'erre", "Maaly", "Honorable"),
        ("Pluck", "Bell Pluck", "Strike", "Riff", "Blink", "Tick"),
    ),
    "trap_reese": (
        ("Drill", "Memphis", "Detroit", "Reese", "Slime", "Offset"),
        ("Undertow", "Growl", "Slide", "Spread", "Night"),
    ),
    "trap_lead": (
        ("Carti", "Gunna", "Thug", "Future", "Lil Baby", "Travis"),
        ("Float", "Slide", "Hook", "Space", "Bleed"),
    ),
    "trap_phonk": (
        ("Memphis", "Phonk", "Screw", "Cowbell", "Doom"),
        ("Crush", "Dust", "Grim", "Haze", "Tape"),
    ),
    "trap_drill_stab": (
        ("NY", "UK", "Drill", "Chicago", "Brooklyn"),
        ("Stab", "Pierce", "Hit", "Cut", "Rip"),
    ),
    "trap_supersaw": (
        ("Brick", "Supra", "Arena", "Chart"),
        ("Saw", "Stack", "Wall", "Anthem"),
    ),
    "trap_stab": (
        ("Rage", "Hard", "Trap", "Dark"),
        ("Stab", "Chop", "Slap", "Knife"),
    ),
    "trap_glide_lead": (
        ("Atlanta", "Slime", "Plugg", "Whole"),
        ("Glide", "Port", "Sweep", "Drift"),
    ),
}


@dataclass(frozen=True)
class ScoredPreset:
    role: str
    name: str
    category: str
    params: dict[str, Any]
    score: float
    notes: list[str]


def _clamp01(x: float) -> float:
    return max(0.0, min(1.0, x))


def micro_variant(base: dict[str, Any], slot: int, role: str) -> dict[str, Any]:
    """Tiny deterministic offsets — musical, not noise."""
    out = dict(base)
    phase = (slot + 1) * 0.073 + len(role) * 0.017
    for key, val in list(out.items()):
        if isinstance(val, bool):
            continue
        if not isinstance(val, (int, float)):
            continue
        wobble = math.sin(phase + hash(key) % 7) * 0.012
        if key in ("filterCutoff", "subLevel", "drive", "crush", "wtPosition", "unisonAmount"):
            out[key] = _clamp01(float(val) + wobble)
        elif key in ("ampDecay", "ampSustain", "ampRelease", "filtEnvAmt", "filterRes"):
            out[key] = _clamp01(float(val) + wobble * 0.8)
        elif key in ("glideMs", "detuneCents", "osc2DetuneCents", "unisonSpreadCents"):
            out[key] = float(val) + wobble * 18.0
        elif key == "osc2Octave":
            out[key] = int(val)
    return out


def make_name(role: str, slot: int, day: str) -> str:
    a, b = NAME_PARTS[role]
    return f"{a[slot % len(a)]} {b[(slot // 2) % len(b)]} ({day[-4:]})"


def score_wave(role: str, p: dict[str, Any]) -> tuple[float, list[str]]:
    notes: list[str] = []
    score = 100.0

    def penalize(msg: str, pts: float) -> None:
        nonlocal score
        score -= pts
        notes.append(msg)

    wt = float(p.get("wtPosition", 0.35))
    if role in ("trap_808", "trap_phonk") and wt < 0.54:
        penalize("808/phonk should use trap wavetable lane (wt>=0.54)", 12)
    if role in ("trap_pluck", "trap_drill_stab") and not (0.65 <= wt <= 0.92):
        penalize("pluck/stab wtPosition out of sweet spot", 10)

    if role == "trap_808":
        if float(p.get("subLevel", 0)) < 0.88:
            penalize("808 subLevel weak", 18)
        if float(p.get("filterCutoff", 1)) > 0.34:
            penalize("808 filter too bright", 14)
        if float(p.get("ampSustain", 0)) < 0.88:
            penalize("808 sustain short", 12)

    if role == "trap_pluck":
        if float(p.get("ampSustain", 1)) > 0.08:
            penalize("pluck must be near-zero sustain", 20)
        if float(p.get("filtEnvAmt", 0)) < 0.72:
            penalize("pluck needs strong filt env", 14)
        decay = float(p.get("ampDecay", 0.2))
        if not (0.06 <= decay <= 0.12):
            penalize("pluck decay not chart-tight", 8)

    if role == "trap_phonk":
        if not p.get("filterDestructive"):
            penalize("phonk needs filterDestructive", 10)
        if float(p.get("crush", 0)) < 0.32:
            penalize("phonk crush too clean", 10)

    if role == "trap_drill_stab":
        if float(p.get("ampSustain", 1)) > 0.12:
            penalize("drill stab sustain too long", 12)

    return max(0.0, score), notes


def score_plugg(role: str, p: dict[str, Any]) -> tuple[float, list[str]]:
    notes: list[str] = []
    score = 100.0

    def penalize(msg: str, pts: float) -> None:
        nonlocal score
        score -= pts
        notes.append(msg)

    if role == "trap_808":
        if float(p.get("subLevel", 0)) < 0.82:
            penalize("VA 808 sub weak", 16)
        if float(p.get("filterCutoff", 1)) > 0.4:
            penalize("VA 808 filter bright", 12)

    if role == "trap_pluck":
        if float(p.get("ampSustain", 1)) > 0.08:
            penalize("VA pluck sustain", 18)
        if int(p.get("osc2Octave", 0)) != 1:
            penalize("VA pluck wants osc2 +1 oct", 10)

    if role == "trap_reese":
        if float(p.get("unisonAmount", 0)) < 0.75:
            penalize("reese needs unison", 12)
        if float(p.get("osc2DetuneCents", 0)) > -8:
            penalize("reese detune should be negative", 8)

    if role == "trap_supersaw" and float(p.get("unisonAmount", 0)) < 0.85:
        penalize("supersaw stack thin", 12)

    return max(0.0, score), notes


def build_wave_slot(role: str, slot: int, day: str) -> ScoredPreset:
    base = WAVE_GOLDEN[role]
    params = micro_variant(base, slot, role)
    score, notes = score_wave(role, params)
    return ScoredPreset(role, make_name(role, slot, day), "TRAP", params, score, notes)


def build_plugg_slot(role: str, slot: int, day: str) -> ScoredPreset:
    base = PLUGG_GOLDEN[role]
    params = micro_variant(base, slot, role)
    score, notes = score_plugg(role, params)
    return ScoredPreset(role, make_name(role, slot, day), "TRAP", params, score, notes)


def daily_wave_presets(day: str) -> list[ScoredPreset]:
    out: list[ScoredPreset] = []
    for i, role in enumerate(WAVE_DAILY_ROLES):
        preset = build_wave_slot(role, i, day)
        if preset.score >= MIN_DAILY_SCORE:
            out.append(preset)
    return out


def daily_plugg_presets(day: str) -> list[ScoredPreset]:
    out: list[ScoredPreset] = []
    for i, role in enumerate(PLUGG_DAILY_ROLES):
        preset = build_plugg_slot(role, i, day)
        if preset.score >= MIN_DAILY_SCORE:
            out.append(preset)
    return out
