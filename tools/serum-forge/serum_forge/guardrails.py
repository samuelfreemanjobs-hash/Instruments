"""Production guardrails from acoustic engineering rules."""

from __future__ import annotations

import math

from serum_forge.models import PatchArchetype, SerumSymbolPatch

MAX_FILTER_RESONANCE = 0.85
MAX_REVERB_WET = 0.4
MAX_MASTER = 0.9
MAX_FM_WARP_DEPTH = 0.55


def headroom_compensation_db(voice_count: int) -> float:
    n = max(1, voice_count)
    return -10.0 * math.log10(n)


def apply_archetype_defaults(patch: SerumSymbolPatch) -> SerumSymbolPatch:
    data = patch.model_dump()
    arch = patch.archetype
    if arch == PatchArchetype.bass_sub:
        data.update(
            {
                "sub_level": max(data["sub_level"], 0.55),
                "sub_octave": -1,
                "osc_a_pan_spread": 0.0,
                "osc_a_unison_voices": 1,
                "filter_a_type": "MgLow24",
                "filter_a_keytrack": 1.0,
                "env1_attack": min(data["env1_attack"], 0.08),
                "fx_reverb_wet": min(data["fx_reverb_wet"], 0.12),
            }
        )
    elif arch == PatchArchetype.lead_pluck:
        data.update(
            {
                "osc_b_enable": True,
                "osc_b_level": max(data["osc_b_level"], 0.35),
                "env1_sustain": min(data["env1_sustain"], 0.05),
                "env1_decay": min(max(data["env1_decay"], 0.15), 0.45),
                "env1_attack": min(data["env1_attack"], 0.05),
                "filter_a_type": "MgLow12",
            }
        )
    elif arch == PatchArchetype.pad_ambient:
        data.update(
            {
                "osc_a_unison_voices": max(data["osc_a_unison_voices"], 8),
                "osc_a_pan_spread": max(data["osc_a_pan_spread"], 0.5),
                "env1_attack": max(data["env1_attack"], 0.35),
                "env1_sustain": max(data["env1_sustain"], 0.75),
                "env1_release": max(data["env1_release"], 0.55),
                "fx_reverb_wet": min(max(data["fx_reverb_wet"], 0.18), MAX_REVERB_WET),
            }
        )
    return SerumSymbolPatch.model_validate(data)


def clamp_patch(patch: SerumSymbolPatch) -> SerumSymbolPatch:
    data = patch.model_dump()
    voices = data["osc_a_unison_voices"] + (1 if data["osc_b_enable"] else 0)
    comp_db = headroom_compensation_db(voices)
    scale = 10 ** (comp_db / 20.0)
    data["osc_a_level"] = min(data["osc_a_level"] * scale, 1.0)
    if data["osc_b_enable"]:
        data["osc_b_level"] = min(data["osc_b_level"] * scale, 1.0)
    data["filter_a_resonance"] = min(data["filter_a_resonance"], MAX_FILTER_RESONANCE)
    data["fx_reverb_wet"] = min(data["fx_reverb_wet"], MAX_REVERB_WET)
    data["master_volume"] = min(data["master_volume"], MAX_MASTER)
    if data["osc_a_warp_mode"] == "FM from B":
        data["osc_a_warp_amount"] = min(data["osc_a_warp_amount"], MAX_FM_WARP_DEPTH)
    if data["osc_b_warp_mode"] == "FM from B":
        data["osc_b_warp_amount"] = min(data["osc_b_warp_amount"], MAX_FM_WARP_DEPTH)
    return apply_archetype_defaults(SerumSymbolPatch.model_validate(data))
