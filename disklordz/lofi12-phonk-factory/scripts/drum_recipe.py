"""Versioned deterministic drum recipes (Phase 1 Drum Factory)."""

from __future__ import annotations

import hashlib
import json
from dataclasses import asdict, dataclass, field
from typing import Any

RECIPE_SCHEMA_VERSION = 1


@dataclass(frozen=True)
class FusionMacros:
    """Seven continuous macro controls, each 0..1."""

    memphis_phonk_blend: float = 0.0  # 0=Memphis, 1=Phonk
    drive: float = 0.35
    vintage_character: float = 0.25  # maps to ZOH + bit depth
    transient_aggression: float = 0.5
    low_frequency_weight: float = 0.35
    metallic_character: float = 0.5
    density_swing: float = 0.5

    def clamped(self) -> FusionMacros:
        def c(v: float) -> float:
            return max(0.0, min(1.0, float(v)))

        return FusionMacros(
            memphis_phonk_blend=c(self.memphis_phonk_blend),
            drive=c(self.drive),
            vintage_character=c(self.vintage_character),
            transient_aggression=c(self.transient_aggression),
            low_frequency_weight=c(self.low_frequency_weight),
            metallic_character=c(self.metallic_character),
            density_swing=c(self.density_swing),
        )


@dataclass(frozen=True)
class DrumRecipe:
    recipe_id: str
    schema_version: int = RECIPE_SCHEMA_VERSION
    factory_mode: str = "memphis_underground"
    voice_kind: str = "kick"
    random_seed: int = 0
    sample_rate: int = 44100
    fusion: FusionMacros = field(default_factory=FusionMacros)
    voice_params: dict[str, Any] = field(default_factory=dict)

    def provenance_hash(self) -> str:
        payload = {
            "schema_version": self.schema_version,
            "factory_mode": self.factory_mode,
            "voice_kind": self.voice_kind,
            "random_seed": self.random_seed,
            "sample_rate": self.sample_rate,
            "fusion": asdict(self.fusion.clamped()),
            "voice_params": self.voice_params,
        }
        blob = json.dumps(payload, sort_keys=True, separators=(",", ":"))
        return hashlib.sha256(blob.encode()).hexdigest()

    def to_dict(self) -> dict[str, Any]:
        return {
            "recipe_id": self.recipe_id,
            "schema_version": self.schema_version,
            "factory_mode": self.factory_mode,
            "voice_kind": self.voice_kind,
            "random_seed": self.random_seed,
            "sample_rate": self.sample_rate,
            "fusion": asdict(self.fusion.clamped()),
            "voice_params": self.voice_params,
            "provenance_hash": self.provenance_hash(),
        }
