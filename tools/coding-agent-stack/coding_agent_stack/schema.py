from __future__ import annotations

from typing import Literal

from pydantic import BaseModel, Field


class PlanStep(BaseModel):
    action: Literal["read", "grep", "sandbox_python", "harness", "note"]
    detail: str
    args: dict = Field(default_factory=dict)


class ImplementationPlan(BaseModel):
    """Architect output — consumed by Sandbox Coder and QA Reviewer."""

    goal: str
    summary: str = ""
    files_to_touch: list[str] = Field(default_factory=list)
    files_to_read: list[str] = Field(default_factory=list)
    steps: list[PlanStep] = Field(default_factory=list)
    harness: Literal["agent_tests", "vst_compile", "none"] = "agent_tests"
    harness_target: str = ""
    max_qa_retries: int = 2
    risks: list[str] = Field(default_factory=list)
