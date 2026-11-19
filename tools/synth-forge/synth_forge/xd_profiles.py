"""Minilogue XD smart-randomize constraint profiles."""

from __future__ import annotations

from synth_forge.models import SynthParameters
from synth_forge.safety import clamp_for_hardware

XD_PROFILES: dict[str, dict[str, tuple[float, float]]] = {
    "xd_bass": {
        "amp_attack": (0.0, 0.25),
        "amp_decay": (0.2, 0.55),
        "amp_sustain": (0.55, 0.95),
        "filter_cutoff": (0.15, 0.45),
        "filter_resonance": (0.1, 0.5),
        "detune": (0.0, 0.2),
    },
    "xd_pad": {
        "amp_attack": (0.35, 0.85),
        "amp_release": (0.45, 0.9),
        "amp_sustain": (0.6, 0.95),
        "filter_cutoff": (0.35, 0.7),
        "filter_resonance": (0.05, 0.45),
        "detune": (0.1, 0.35),
        "lfo_depth": (0.05, 0.35),
    },
    "xd_pluck": {
        "amp_attack": (0.0, 0.12),
        "amp_decay": (0.1, 0.35),
        "amp_sustain": (0.0, 0.35),
        "filter_cutoff": (0.45, 0.85),
    },
    "xd_lead": {
        "filter_cutoff": (0.45, 0.85),
        "filter_resonance": (0.2, 0.65),
        "amp_attack": (0.02, 0.2),
        "detune": (0.05, 0.3),
    },
    "xd_scifi_fx": {
        "filter_cutoff": (0.6, 1.0),
        "filter_resonance": (0.4, 0.85),
        "lfo_rate": (0.25, 0.95),
        "lfo_depth": (0.35, 0.95),
        "delay_feedback": (0.2, 0.65),
    },
}

PROFILE_ALIASES: dict[str, str] = {
    "bass": "xd_bass",
    "pad": "xd_pad",
    "pluck": "xd_pluck",
    "keys": "xd_pluck",
    "lead": "xd_lead",
    "sci-fi": "xd_scifi_fx",
    "scifi": "xd_scifi_fx",
    "fx": "xd_scifi_fx",
}


def resolve_xd_profile(category: str) -> str | None:
    key = category.lower().replace(" ", "_").replace("/", "_")
    if key in XD_PROFILES:
        return key
    if key.startswith("xd_") and key in XD_PROFILES:
        return key
    for token, profile in sorted(PROFILE_ALIASES.items(), key=lambda item: -len(item[0])):
        if token in key:
            return profile
    return None


def apply_xd_profile(params: SynthParameters, category: str, rng) -> SynthParameters:
    profile_key = resolve_xd_profile(category)
    if profile_key is None:
        return params
    rules = XD_PROFILES[profile_key]
    data = params.model_dump()
    for name, (lo, hi) in rules.items():
        if name not in data:
            continue
        data[name] = lo + rng.random() * (hi - lo)
    data["category"] = profile_key
    return clamp_for_hardware(SynthParameters.model_validate(data))
