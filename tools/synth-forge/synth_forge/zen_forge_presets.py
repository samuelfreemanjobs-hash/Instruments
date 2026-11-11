"""ZEN-Core Patch Forge genre profiles."""

from __future__ import annotations

import random

from synth_forge.models import SynthParameters
from synth_forge.safety import clamp_for_hardware

ZEN_PROFILES: dict[str, dict[str, tuple[float, float]]] = {
    "memphis_lead": {
        "filter_cutoff": (0.55, 0.92),
        "filter_resonance": (0.35, 0.72),
        "detune": (0.25, 0.55),
        "amp_attack": (0.02, 0.15),
        "amp_decay": (0.15, 0.45),
    },
    "rnb_pad": {
        "amp_attack": (0.4, 0.75),
        "amp_release": (0.5, 0.9),
        "filter_cutoff": (0.35, 0.65),
        "detune": (0.12, 0.35),
        "lfo_depth": (0.1, 0.35),
    },
    "fm_pluck": {
        "amp_attack": (0.0, 0.05),
        "amp_decay": (0.08, 0.25),
        "amp_sustain": (0.0, 0.2),
        "filter_cutoff": (0.7, 0.98),
    },
    "sub_808": {
        "filter_cutoff": (0.1, 0.35),
        "osc1_wave": (0.0, 0.15),
        "amp_attack": (0.0, 0.08),
        "amp_sustain": (0.75, 0.98),
        "detune": (0.0, 0.08),
    },
    "ambient_texture": {
        "amp_attack": (0.5, 0.95),
        "amp_release": (0.6, 0.95),
        "lfo_rate": (0.05, 0.35),
        "lfo_depth": (0.2, 0.65),
        "delay_feedback": (0.25, 0.65),
    },
}

PROFILE_LABELS: dict[str, str] = {
    "memphis_lead": "Dark Trap / Memphis Lead",
    "rnb_pad": "90s R&B Lush Pad",
    "fm_pluck": "FM / Bell Pluck",
    "sub_808": "Sub / 808 Bass",
    "ambient_texture": "Ambient Texture",
}


def apply_zen_profile(category: str, rng: random.Random | None = None) -> SynthParameters:
    rng = rng or random.Random()
    rules = ZEN_PROFILES.get(category, ZEN_PROFILES["memphis_lead"])
    data = SynthParameters(category=category).model_dump()
    for name, (lo, hi) in rules.items():
        data[name] = lo + rng.random() * (hi - lo)
    return clamp_for_hardware(SynthParameters.model_validate(data))


def procedural_name(profile: str, index: int) -> str:
    short = profile.replace("_", " ").title()[:14]
    return f"FRG {short} {index:02d}"
