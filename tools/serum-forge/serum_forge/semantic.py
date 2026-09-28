"""Heuristic prompt → SerumSymbolPatch (constrained symbolic synthesis)."""

from __future__ import annotations

import re

from serum_forge.models import PatchArchetype, SerumSymbolPatch

KEYWORD_ARCHETYPE = [
    (re.compile(r"\b(bass|sub|808|low)\b", re.I), PatchArchetype.bass_sub),
    (re.compile(r"\b(pluck|lead|stab|arp)\b", re.I), PatchArchetype.lead_pluck),
    (re.compile(r"\b(pad|ambient|atmos|drone)\b", re.I), PatchArchetype.pad_ambient),
]


def intent_to_patch(prompt: str, *, archetype: PatchArchetype | None = None) -> SerumSymbolPatch:
    text = prompt.strip()
    arch = archetype
    if arch is None:
        for pattern, candidate in KEYWORD_ARCHETYPE:
            if pattern.search(text):
                arch = candidate
                break
    if arch is None:
        arch = PatchArchetype.pad_ambient

    name = text[:48] if len(text) <= 48 else text[:45] + "..."
    patch = SerumSymbolPatch(name=name, description=text, archetype=arch, tags=_tags(text))

    if re.search(r"\b(dusty|lofi|tape|grit)\b", text, re.I):
        patch = patch.model_copy(
            update={
                "noise_level": 0.22,
                "fx_dist_drive": 0.25,
                "filter_a_cutoff": 0.38,
            }
        )
    if re.search(r"\b(bright|supersaw|trance)\b", text, re.I):
        patch = patch.model_copy(
            update={
                "osc_a_unison_voices": 8,
                "filter_a_cutoff": 0.62,
                "osc_a_level": 0.68,
            }
        )
    if re.search(r"\bfm\b", text, re.I):
        patch = patch.model_copy(
            update={
                "osc_b_enable": True,
                "osc_b_level": 0.5,
                "osc_a_warp_mode": "FM from B",
                "osc_a_warp_amount": 0.35,
            }
        )
    return patch


def _tags(text: str) -> list[str]:
    tokens = re.findall(r"[a-zA-Z]{4,}", text.lower())
    seen: list[str] = []
    for t in tokens[:8]:
        if t not in seen:
            seen.append(t)
    return seen
