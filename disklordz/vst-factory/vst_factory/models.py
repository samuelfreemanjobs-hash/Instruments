from __future__ import annotations

from enum import Enum
from typing import Any

from pydantic import BaseModel, Field, field_validator


class PluginClass(str, Enum):
    SYNTH = "synth"
    EFFECT = "effect"
    MIDI = "midi"
    SAMPLER = "sampler"


class FactoryState(str, Enum):
    INTAKE = "intake"
    DREAM = "dream"
    PLAN = "plan"
    DESIGN = "design"
    SCAFFOLD = "scaffold"
    IMPL = "impl"
    BUILD = "build"
    TEST = "test"
    REPAIR = "repair"
    SHIP = "ship"


class ProjectCreate(BaseModel):
    prompt: str = Field(..., min_length=8, max_length=16000)
    plugin_class: PluginClass = PluginClass.MIDI
    product_category: str | None = Field(
        default=None,
        description="Taxonomy id from product-taxonomy.json",
    )
    plugin_name: str | None = Field(
        default=None,
        description="PascalCase; generated from prompt if omitted",
    )
    product_category: str | None = Field(
        default=None,
        description="Taxonomy id e.g. fx.metering",
        max_length=128,
    )

    @field_validator("plugin_name")
    @classmethod
    def validate_name(cls, v: str | None) -> str | None:
        if v is None:
            return v
        if not v[0].isalpha() or not v.isalnum():
            raise ValueError("plugin_name must be PascalCase alphanumeric")
        return v


class ProjectOut(BaseModel):
    id: int
    plugin_name: str
    plugin_class: str
    state: str
    spec: dict[str, Any]
    apc_path: str | None = None
    vst_path: str | None = None


class RunOut(BaseModel):
    id: int
    project_id: int
    kind: str
    ok: bool
    output_tail: str
    created_at: str


class CursorPromptOut(BaseModel):
    project_id: int
    role: str
    prompt_text: str
