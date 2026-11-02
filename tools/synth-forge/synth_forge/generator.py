from __future__ import annotations

import random
from typing import Iterable

import numpy as np

from synth_forge.models import Preset, SynthParameters
from synth_forge.safety import clamp_for_hardware

ARCHETYPES: dict[str, dict[str, float]] = {
    "luxury_west_coast": {
        "osc1_wave": 0.2,
        "filter_cutoff": 0.62,
        "filter_resonance": 0.28,
        "lfo_rate": 0.08,
        "lfo_depth": 0.35,
        "delay_feedback": 0.18,
    },
    "bounce_brass": {
        "osc1_wave": 0.85,
        "amp_attack": 0.02,
        "amp_decay": 0.15,
        "filter_cutoff": 0.78,
        "filter_resonance": 0.55,
    },
    "g_funk_whistle": {
        "osc1_wave": 0.05,
        "osc2_wave": 0.1,
        "pitch_glide": 0.45,
        "lfo_depth": 0.4,
        "filter_cutoff": 0.82,
    },
    "phonk_reese": {
        "detune": 0.55,
        "osc1_wave": 0.95,
        "filter_resonance": 0.72,
        "filter_cutoff": 0.42,
    },
    "dx_ep": {
        "osc1_wave": 0.15,
        "osc2_wave": 0.35,
        "filter_cutoff": 0.5,
        "amp_sustain": 0.55,
    },
    "hyperpop_lead": {
        "filter_cutoff": 0.92,
        "filter_resonance": 0.65,
        "detune": 0.25,
        "master_volume": 0.82,
    },
}

CATEGORY_ALIASES: dict[str, str] = {
    "cardo": "luxury_west_coast",
    "cruising": "luxury_west_coast",
    "mannie": "bounce_brass",
    "bounce": "bounce_brass",
    "g-funk": "g_funk_whistle",
    "whistle": "g_funk_whistle",
    "reese": "phonk_reese",
    "phonk": "phonk_reese",
    "dx7": "dx_ep",
    "rhodes": "dx_ep",
    "hyperpop": "hyperpop_lead",
    "rage": "hyperpop_lead",
}


def resolve_archetype(category: str) -> str:
    key = category.lower().replace(" ", "_")
    if key in ARCHETYPES:
        return key
    for token, archetype in CATEGORY_ALIASES.items():
        if token in key:
            return archetype
    return "luxury_west_coast" if "pad" in key else "generic"


def _base_from_archetype(category: str) -> SynthParameters:
    archetype = resolve_archetype(category)
    if archetype == "generic":
        return SynthParameters(category=category)
    hints = ARCHETYPES[archetype]
    data = SynthParameters(category=category).model_dump()
    for k, v in hints.items():
        if k != "category":
            data[k] = float(v)
    return SynthParameters.model_validate(data)


def _mutate(params: SynthParameters, sigma: float, rng: random.Random) -> SynthParameters:
    data = params.model_dump()
    for name in SynthParameters.model_fields:
        if name == "category":
            continue
        val = float(data[name])
        data[name] = float(np.clip(val + rng.gauss(0, sigma), 0.0, 1.0))
    return clamp_for_hardware(SynthParameters.model_validate(data))


def morph(parents: Iterable[SynthParameters], weight: float | None = None) -> SynthParameters:
    parents = list(parents)
    if not parents:
        return clamp_for_hardware(SynthParameters())
    if len(parents) == 1:
        return clamp_for_hardware(parents[0])
    w = weight if weight is not None else 1.0 / len(parents)
    acc = {k: 0.0 for k in parents[0].model_dump() if k != "category"}
    for p in parents:
        for k in acc:
            acc[k] += float(getattr(p, k)) * w
    acc["category"] = parents[0].category
    return clamp_for_hardware(SynthParameters.model_validate(acc))


def generate_batch(
    synth_id: str,
    count: int,
    *,
    category: str = "generic",
    seed: int | None = None,
    parent: SynthParameters | None = None,
) -> list[Preset]:
    rng = random.Random(seed)
    base = parent or _base_from_archetype(category)
    presets: list[Preset] = []
    chain = base
    for i in range(count):
        chain = _mutate(chain, sigma=0.08 + rng.random() * 0.06, rng=rng)
        chain = SynthParameters.model_validate({**chain.model_dump(), "category": category})
        presets.append(
            Preset(
                name=f"{category}_{i+1:03d}",
                synth_id=synth_id,
                parameters=chain,
            )
        )
    return presets
