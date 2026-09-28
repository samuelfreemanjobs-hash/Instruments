from __future__ import annotations

from enum import Enum
from typing import Any

from pydantic import BaseModel, Field, field_validator


class PatchArchetype(str, Enum):
    bass_sub = "bass_sub"
    lead_pluck = "lead_pluck"
    pad_ambient = "pad_ambient"


class SerumSymbolPatch(BaseModel):
    """Reduced ParamSpec (~core subset) for LLM / optimizer I/O.

    Full Serum state is ~600 CBOR fields; this schema targets 40 primary controls
    with the remainder implied at Serum defaults during external packager merge.
    """

    name: str = Field(..., min_length=1, max_length=64)
    author: str = "serum-forge"
    description: str = ""
    tags: list[str] = Field(default_factory=list, max_length=12)
    archetype: PatchArchetype = PatchArchetype.pad_ambient

    osc_a_wt_position: float = Field(0.0, ge=0.0, le=1.0)
    osc_a_level: float = Field(0.72, ge=0.0, le=1.0)
    osc_a_warp_mode: str = Field("Off")
    osc_a_warp_amount: float = Field(0.0, ge=0.0, le=1.0)
    osc_a_unison_voices: int = Field(1, ge=1, le=16)
    osc_a_unison_detune: float = Field(0.08, ge=0.0, le=1.0)
    osc_a_pan_spread: float = Field(0.35, ge=0.0, le=1.0)

    osc_b_enable: bool = False
    osc_b_wt_position: float = Field(0.0, ge=0.0, le=1.0)
    osc_b_level: float = Field(0.0, ge=0.0, le=1.0)
    osc_b_warp_mode: str = Field("Off")
    osc_b_warp_amount: float = Field(0.0, ge=0.0, le=1.0)

    sub_level: float = Field(0.0, ge=0.0, le=1.0)
    sub_octave: int = Field(-1, ge=-2, le=0)
    noise_level: float = Field(0.0, ge=0.0, le=1.0)

    filter_a_type: str = Field("MgLow12")
    filter_a_cutoff: float = Field(0.45, ge=0.0, le=1.0)
    filter_a_resonance: float = Field(0.25, ge=0.0, le=0.85)
    filter_a_keytrack: float = Field(0.5, ge=0.0, le=1.0)

    env1_attack: float = Field(0.05, ge=0.0, le=1.0)
    env1_decay: float = Field(0.35, ge=0.0, le=1.0)
    env1_sustain: float = Field(0.7, ge=0.0, le=1.0)
    env1_release: float = Field(0.4, ge=0.0, le=1.0)

    env2_attack: float = Field(0.02, ge=0.0, le=1.0)
    env2_decay: float = Field(0.5, ge=0.0, le=1.0)
    env2_sustain: float = Field(0.0, ge=0.0, le=1.0)
    env2_release: float = Field(0.3, ge=0.0, le=1.0)

    lfo1_rate: float = Field(0.2, ge=0.0, le=1.0)
    lfo1_depth: float = Field(0.0, ge=0.0, le=1.0)
    lfo1_sync: bool = True

    fx_reverb_wet: float = Field(0.15, ge=0.0, le=0.4)
    fx_reverb_decay: float = Field(0.35, ge=0.0, le=1.0)
    fx_dist_drive: float = Field(0.0, ge=0.0, le=0.75)
    master_volume: float = Field(0.75, ge=0.0, le=0.9)

    mod_matrix: list[dict[str, Any]] = Field(default_factory=list, max_length=32)

    @field_validator("osc_a_warp_mode", "osc_b_warp_mode")
    @classmethod
    def warp_modes(cls, v: str) -> str:
        allowed = {
            "Off",
            "Bend+",
            "Bend-",
            "Sync",
            "FM from B",
            "AM from B",
            "Quantize",
        }
        if v not in allowed:
            raise ValueError(f"unsupported warp mode: {v}")
        return v

    def to_intermediate_json(self) -> dict[str, Any]:
        return {
            "metadata": {
                "name": self.name,
                "author": self.author,
                "description": self.description,
                "tags": self.tags,
                "archetype": self.archetype.value,
            },
            "parameters": self.model_dump(exclude={"name", "author", "description", "tags"}),
        }
