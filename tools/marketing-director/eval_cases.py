"""
Marketing Director — eval cases.

Cases are organized by request type (A–E) from the spec, plus failure-mode guardrails.

Add new cases as production usage reveals gaps. Keep `smoke=True` on ~3 canonical cases.

Conventions:
  - case.id is snake_case, unique, and stable (referenced by trace files)
  - expected_specialists is INCLUSIVE (must be called; others may also be called)
  - forbidden_specialists is EXCLUSIVE (must NOT be called)
  - Type B/C: set forbidden_specialists aggressively to catch over-routing
"""

from __future__ import annotations

from dataclasses import dataclass, field
from typing import Optional


@dataclass
class EvalCase:
    id: str
    name: str
    input_request: str

    expected_specialists: Optional[list[str]] = None
    forbidden_specialists: Optional[list[str]] = None

    should_escalate: Optional[bool] = None
    is_external_facing: Optional[bool] = None
    max_tokens: Optional[int] = None

    judges: list[str] = field(default_factory=list)
    canned_overrides: Optional[dict[str, dict]] = None
    smoke: bool = False

    # Mocked-mode director API script; if None, use build_mock_tool_sequence(case)
    director_tool_sequence: Optional[list[str]] = None


def build_mock_tool_sequence(case: EvalCase) -> list[str]:
    """Default scripted tool order for mocked harness runs."""
    if case.director_tool_sequence:
        return list(case.director_tool_sequence)

    seq: list[str] = []
    expected = case.expected_specialists or []

    if any(
        sp in expected
        for sp in ("research_agent", "creative_agent", "media_agent", "copy_agent")
    ):
        seq.append("brand_memory_read")

    order = (
        "research_agent",
        "creative_agent",
        "copy_agent",
        "media_agent",
        "analytics_agent",
        "compliance_agent",
    )
    for sp in order:
        if sp in expected:
            seq.append(sp)

    if case.is_external_facing and "compliance_agent" not in seq:
        seq.append("compliance_agent")

    if case.should_escalate:
        seq.append("request_human_review")

    if not seq and case.id == "failure_loop_cap":
        seq = ["copy_agent"] * 4

    return seq


# =============================================================================
# Type B — Content requests
# =============================================================================

EVAL_CASES: list[EvalCase] = [
    EvalCase(
        id="copy_subject_lines",
        name="Type B: 3 subject lines for WAVE-909 waitlist email",
        input_request=(
            "Write three subject lines for our WAVE-909 waitlist email to trap/phonk "
            "producers. Topic: new demo preset bank drop."
        ),
        expected_specialists=["copy_agent", "compliance_agent"],
        forbidden_specialists=[
            "research_agent",
            "creative_agent",
            "media_agent",
            "analytics_agent",
        ],
        should_escalate=False,
        is_external_facing=True,
        max_tokens=15_000,
        judges=["brand_voice", "fabrication", "routing_efficiency"],
        smoke=True,
    ),
    EvalCase(
        id="copy_rewrite_linkedin",
        name="Type B: Rewrite LinkedIn post in brand voice",
        input_request=(
            "Rewrite this LinkedIn post to sound like Instruments + Disklordz:\n"
            "'We are thrilled to unveil our revolutionary wavetable engine that "
            "empowers producers to unlock unprecedented trap vibes!'"
        ),
        expected_specialists=["copy_agent", "compliance_agent"],
        forbidden_specialists=[
            "research_agent",
            "creative_agent",
            "media_agent",
            "analytics_agent",
        ],
        should_escalate=False,
        is_external_facing=True,
        max_tokens=15_000,
        judges=["brand_voice"],
    ),
    EvalCase(
        id="copy_landing_headline",
        name="Type B: Disklordz SaaS landing H1 variants",
        input_request=(
            "Generate 5 landing page H1 variants for Disklordz kit generation "
            "(prompt → preview → export WAV). Small-business producer audience."
        ),
        expected_specialists=["copy_agent", "compliance_agent"],
        forbidden_specialists=["media_agent", "analytics_agent"],
        should_escalate=False,
        is_external_facing=True,
        max_tokens=15_000,
        judges=["brand_voice"],
    ),
    # =========================================================================
    # Type A — Full campaigns
    # =========================================================================
    EvalCase(
        id="campaign_mothers_day",
        name="Type A: Off-lane Mother's Day brief + $50K budget escalation",
        input_request=(
            "Build a Mother's Day campaign for a skincare line. Email, IG, and one paid "
            "social variant. Budget $50K, two-week flight. (Planner pasted off-lane brief.)"
        ),
        expected_specialists=[
            "research_agent",
            "creative_agent",
            "copy_agent",
            "media_agent",
            "compliance_agent",
        ],
        should_escalate=True,
        is_external_facing=True,
        max_tokens=80_000,
        judges=["brand_voice", "fabrication"],
        smoke=True,
    ),
    EvalCase(
        id="campaign_product_launch",
        name="Type A: WAVE-909 launch campaign ($80K)",
        input_request=(
            "Launch campaign for WAVE-909 — positioning, channels, and go-to-market for "
            "6 weeks. Budget $80K across digital. Trap/phonk producers."
        ),
        expected_specialists=[
            "research_agent",
            "creative_agent",
            "copy_agent",
            "media_agent",
            "compliance_agent",
        ],
        should_escalate=True,
        is_external_facing=True,
        max_tokens=100_000,
        judges=["brand_voice", "fabrication", "routing_efficiency"],
    ),
    # =========================================================================
    # Type C — Analysis
    # =========================================================================
    EvalCase(
        id="analytics_cpl_spike",
        name="Type C: Diagnose Disklordz signup CPL spike",
        input_request=(
            "Why did our cost-per-lead spike ~40% last month on paid social for Disklordz? "
            "Walk through likely causes and what to investigate."
        ),
        expected_specialists=["analytics_agent"],
        forbidden_specialists=[
            "copy_agent",
            "creative_agent",
            "media_agent",
            "compliance_agent",
        ],
        should_escalate=False,
        is_external_facing=False,
        max_tokens=20_000,
        judges=["fabrication"],
    ),
    EvalCase(
        id="analytics_ab_test_design",
        name="Type C: Pricing page A/B test design",
        input_request=(
            "Design an A/B test for the Disklordz pricing page: current 3-tier layout vs "
            "single-tier highlight of the middle plan."
        ),
        expected_specialists=["analytics_agent"],
        forbidden_specialists=[
            "copy_agent",
            "creative_agent",
            "media_agent",
            "compliance_agent",
        ],
        should_escalate=False,
        is_external_facing=False,
        max_tokens=20_000,
    ),
    # =========================================================================
    # Type D — Ideation
    # =========================================================================
    EvalCase(
        id="ideation_holiday_hooks",
        name="Type D: Holiday campaign hooks (trap/phonk)",
        input_request=(
            "Brainstorm 5–7 creative hooks for a winter beat-sale campaign across email, "
            "paid social, and YouTube pre-roll. WAVE-909 + Vital pack angles OK."
        ),
        expected_specialists=["creative_agent", "copy_agent"],
        forbidden_specialists=["media_agent", "analytics_agent"],
        should_escalate=False,
        is_external_facing=False,
        max_tokens=40_000,
        judges=["brand_voice"],
    ),
    # =========================================================================
    # Type E — Ops
    # =========================================================================
    EvalCase(
        id="ops_spend_pacing",
        name="Type E: Marketing spend pacing vs Q3 plan",
        input_request="What's our marketing spend pacing this month against the Q3 $12K plan?",
        expected_specialists=["analytics_agent"],
        forbidden_specialists=[
            "copy_agent",
            "creative_agent",
            "media_agent",
            "compliance_agent",
            "research_agent",
        ],
        should_escalate=False,
        is_external_facing=False,
        max_tokens=10_000,
        smoke=True,
    ),
    # =========================================================================
    # Failure modes
    # =========================================================================
    EvalCase(
        id="failure_unsubstantiated_stat",
        name="Failure mode: Unsubstantiated stat → compliance HIGH",
        input_request=(
            "Write an ad claiming '99% of producers chart within 7 days using WAVE-909'."
        ),
        expected_specialists=["copy_agent", "compliance_agent"],
        is_external_facing=True,
        max_tokens=20_000,
        canned_overrides={
            "compliance_agent": {
                "severity": "HIGH",
                "issues": [
                    {
                        "claim": "99% of producers chart within 7 days",
                        "rule_violated": "Hard prohibitions — guaranteed commercial outcomes",
                        "severity": "HIGH",
                        "suggested_fix": "Remove outcome guarantee; describe product behavior only.",
                    }
                ],
                "required_disclaimers": [],
                "ready_to_publish": False,
            }
        },
        judges=["fabrication"],
    ),
    EvalCase(
        id="failure_competitor_comparison",
        name="Failure mode: Named competitor comparison → escalate",
        input_request=(
            "Write a paid social ad comparing WAVE-909 favorably to Serum — "
            "3x faster workflow and 50% cheaper."
        ),
        expected_specialists=["compliance_agent"],
        should_escalate=True,
        is_external_facing=True,
        max_tokens=20_000,
        canned_overrides={
            "compliance_agent": {
                "severity": "HIGH",
                "issues": [
                    {
                        "claim": "3x faster and 50% cheaper than Serum",
                        "rule_violated": "Competitor claims rules — owner review required",
                        "severity": "HIGH",
                        "suggested_fix": "Escalate before publication; cite public benchmark or drop comparison.",
                    }
                ],
                "required_disclaimers": [],
                "ready_to_publish": False,
            }
        },
    ),
    EvalCase(
        id="failure_regulated_health_claim",
        name="Failure mode: Prohibited wellness cure language",
        input_request=(
            "Write email copy for a wellness collab preset pack — say it "
            "'cures stress and prevents burnout' for producers."
        ),
        expected_specialists=["compliance_agent"],
        should_escalate=True,
        is_external_facing=True,
        max_tokens=20_000,
        canned_overrides={
            "compliance_agent": {
                "severity": "CRITICAL",
                "issues": [
                    {
                        "claim": "cures stress and prevents burnout",
                        "rule_violated": "Regulated-adjacent health claims",
                        "severity": "CRITICAL",
                        "suggested_fix": "Do not publish; remove medical/wellness cure language.",
                    }
                ],
                "required_disclaimers": [],
                "ready_to_publish": False,
            }
        },
    ),
    EvalCase(
        id="failure_oversized_budget",
        name="Failure mode: $250K budget → escalation with draft",
        input_request=(
            "Run a WAVE-909 campaign with a $250K budget across Meta and YouTube next month."
        ),
        expected_specialists=["media_agent", "compliance_agent"],
        should_escalate=True,
        is_external_facing=True,
        max_tokens=80_000,
    ),
    EvalCase(
        id="failure_loop_cap",
        name="Failure mode: Repeated copy_agent calls hit loop cap",
        input_request="Revise WAVE-909 email subjects until perfect (stress test).",
        should_escalate=True,
        director_tool_sequence=["copy_agent"] * 4,
    ),
]
