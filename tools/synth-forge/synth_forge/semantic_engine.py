from __future__ import annotations

import re

from synth_forge.generator import generate_batch, resolve_archetype
from synth_forge.models import Preset, PromptGenerateRequest, SynthParameters
from synth_forge.safety import clamp_for_hardware

STYLE_PATTERNS: list[tuple[re.Pattern[str], str]] = [
    (re.compile(r"cardo|luxury|cruising|larry\s*june", re.I), "luxury_west_coast"),
    (re.compile(r"mannie|cash\s*money|bounce|triton", re.I), "bounce_brass"),
    (re.compile(r"g-?funk|dre|snoop|whistle|battlecat", re.I), "g_funk_whistle"),
    (re.compile(r"payroll|detroit|talkbox", re.I), "phonk_reese"),
    (re.compile(r"phonk|reese|drift|wave\s*phonk", re.I), "phonk_reese"),
    (re.compile(r"dx7|fm\s*bell|rhodes|electric\s*piano|r&b|smooth\s*jazz", re.I), "dx_ep"),
    (re.compile(r"hyperpop|rage|hypersaw", re.I), "hyperpop_lead"),
    (re.compile(r"\bpad\b", re.I), "luxury_west_coast"),
    (re.compile(r"\bbass\b", re.I), "phonk_reese"),
    (re.compile(r"\blead\b", re.I), "g_funk_whistle"),
]


def infer_category(prompt: str) -> str:
    text = prompt.strip()
    for pattern, category in STYLE_PATTERNS:
        if pattern.search(text):
            return category
    return resolve_archetype(text)


def prompt_to_parameters(prompt: str) -> SynthParameters:
    category = infer_category(prompt)
    base = generate_batch("minilogue_xd", 1, category=category)[0].parameters
    data = base.model_dump()
    if re.search(r"warm|lush", prompt, re.I):
        data["filter_cutoff"] = min(data["filter_cutoff"], 0.55)
        data["lfo_depth"] = max(data["lfo_depth"], 0.25)
    if re.search(r"bright|punch", prompt, re.I):
        data["filter_cutoff"] = max(data["filter_cutoff"], 0.75)
        data["amp_attack"] = min(data["amp_attack"], 0.08)
    if re.search(r"aggressive|scream", prompt, re.I):
        data["filter_resonance"] = min(0.84, data["filter_resonance"] + 0.1)
    data["category"] = category
    return clamp_for_hardware(SynthParameters.model_validate(data))


def generate_from_prompt(req: PromptGenerateRequest) -> list[Preset]:
    seed_params = prompt_to_parameters(req.prompt)
    return generate_batch(
        req.synth_id,
        req.count,
        category=seed_params.category,
        parent=seed_params,
    )
