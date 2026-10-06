from __future__ import annotations

import math
from datetime import datetime, timezone
from typing import Any, Literal
from uuid import uuid4

from pydantic import BaseModel, Field


Rating = Literal["keep", "favorite", "discard", "unset"]


class SynthParameters(BaseModel):
    """Normalized synthesis controls (0–1 unless noted)."""

    osc1_wave: float = 0.25
    osc2_wave: float = 0.5
    filter_cutoff: float = 0.55
    filter_resonance: float = 0.35
    amp_attack: float = 0.12
    amp_decay: float = 0.35
    amp_sustain: float = 0.72
    amp_release: float = 0.38
    lfo_rate: float = 0.18
    lfo_depth: float = 0.15
    delay_feedback: float = 0.22
    master_volume: float = 0.72
    detune: float = 0.08
    pitch_glide: float = 0.05
    category: str = "generic"

    def to_timbre_vector(self) -> list[float]:
        keys = [
            "osc1_wave",
            "osc2_wave",
            "filter_cutoff",
            "filter_resonance",
            "amp_attack",
            "amp_decay",
            "amp_sustain",
            "amp_release",
            "lfo_rate",
            "lfo_depth",
            "delay_feedback",
            "master_volume",
            "detune",
            "pitch_glide",
        ]
        return [float(getattr(self, k)) for k in keys]

    @staticmethod
    def cosine_similarity(a: list[float], b: list[float]) -> float:
        if len(a) != len(b) or not a:
            return 0.0
        dot = sum(x * y for x, y in zip(a, b))
        na = math.sqrt(sum(x * x for x in a))
        nb = math.sqrt(sum(y * y for y in b))
        if na == 0 or nb == 0:
            return 0.0
        return dot / (na * nb)


class Preset(BaseModel):
    id: str = Field(default_factory=lambda: str(uuid4()))
    name: str = "Untitled"
    synth_id: str = "minilogue_xd"
    parameters: SynthParameters
    parent_ids: list[str] = Field(default_factory=list)
    rating: Rating = "unset"
    timbre_vector: list[float] = Field(default_factory=list)
    created_at: datetime = Field(default_factory=lambda: datetime.now(timezone.utc))

    def model_post_init(self, __context: Any) -> None:
        if not self.timbre_vector:
            object.__setattr__(self, "timbre_vector", self.parameters.to_timbre_vector())


class BatchRun(BaseModel):
    id: str = Field(default_factory=lambda: str(uuid4()))
    synth_id: str
    count: int
    preset_ids: list[str] = Field(default_factory=list)
    created_at: datetime = Field(default_factory=lambda: datetime.now(timezone.utc))


class BatchGenerateRequest(BaseModel):
    synth_id: str = "minilogue_xd"
    count: int = Field(default=8, ge=1, le=64)
    category: str = "generic"
    seed: int | None = None
    parent_preset_id: str | None = None


class PromptGenerateRequest(BaseModel):
    prompt: str = Field(..., min_length=3, max_length=500)
    synth_id: str = "minilogue_xd"
    count: int = Field(default=4, ge=1, le=32)


class CloneAudioRequest(BaseModel):
    synth_id: str = "minilogue_xd"
    count: int = Field(default=6, ge=1, le=32)
    variation: float = Field(default=0.15, ge=0.0, le=0.5)
