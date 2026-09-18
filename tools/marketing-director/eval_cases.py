"""Eval cases for Marketing Director test harness."""

from __future__ import annotations

from dataclasses import dataclass, field


@dataclass
class EvalCase:
    id: str
    name: str
    input_request: str
    expected_specialists: list[str] | None = None
    forbidden_specialists: list[str] | None = None
    should_escalate: bool | None = None
    is_external_facing: bool | None = None
    max_tokens: int | None = None
    smoke: bool = False
    judges: list[str] = field(default_factory=list)
    canned_overrides: dict[str, dict] | None = None
    """Mock director API tool sequence (tool names). Overrides expected_specialists for scripting."""
    director_tool_sequence: list[str] | None = None


EVAL_CASES: list[EvalCase] = [
    EvalCase(
        id="copy_subject_lines",
        name="WAVE-909 waitlist email subject lines",
        input_request=(
            "Write three subject lines for a WAVE-909 waitlist email to trap/phonk producers. "
            "External-facing; must pass compliance."
        ),
        expected_specialists=["copy_agent", "compliance_agent"],
        forbidden_specialists=["media_agent", "analytics_agent"],
        is_external_facing=True,
        smoke=True,
        judges=["brand_voice", "fabrication", "routing_efficiency"],
        director_tool_sequence=[
            "brand_memory_read",
            "copy_agent",
            "compliance_agent",
        ],
    ),
    EvalCase(
        id="copy_only_efficient",
        name="Simple copy request — no full pipeline",
        input_request="Draft three Instagram captions for TR-808 PWA install CTA (internal draft).",
        expected_specialists=["copy_agent"],
        forbidden_specialists=[
            "media_agent",
            "research_agent",
            "creative_agent",
            "analytics_agent",
        ],
        is_external_facing=False,
        director_tool_sequence=["copy_agent"],
    ),
    EvalCase(
        id="campaign_with_media",
        name="Campaign with budget under authorization cap",
        input_request=(
            "Two-week Disklordz SaaS campaign, $8k budget, email + paid social. "
            "Need media mix and compliance on external copy."
        ),
        expected_specialists=["media_agent", "compliance_agent"],
        forbidden_specialists=[],
        is_external_facing=True,
        director_tool_sequence=["brand_memory_read", "media_agent", "compliance_agent"],
    ),
    EvalCase(
        id="human_review_escalation",
        name="Explicit human review escalation",
        input_request="Plan $60k multi-channel launch for WAVE-909 — needs owner approval.",
        should_escalate=True,
        is_external_facing=True,
        director_tool_sequence=[
            "brand_memory_read",
            "media_agent",
            "compliance_agent",
            "request_human_review",
        ],
    ),
    EvalCase(
        id="loop_cap_escalation",
        name="Repeated copy_agent calls trigger loop cap",
        input_request="Revise email subject lines until perfect (stress test).",
        should_escalate=True,
        director_tool_sequence=[
            "copy_agent",
            "copy_agent",
            "copy_agent",
            "copy_agent",
        ],
    ),
    EvalCase(
        id="analytics_only",
        name="Performance question — analytics path",
        input_request="Diagnose last 30d Disklordz signups trend; no new copy needed.",
        expected_specialists=["analytics_agent"],
        forbidden_specialists=["copy_agent", "creative_agent"],
        director_tool_sequence=["analytics_agent"],
    ),
]
