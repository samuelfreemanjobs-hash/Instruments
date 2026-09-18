"""Lightweight JSON schema checks for business agent outputs (stdlib only)."""

from __future__ import annotations

from typing import Any


def _require(obj: dict[str, Any], key: str, errors: list[str]) -> Any:
    if key not in obj:
        errors.append(f"missing required key: {key}")
        return None
    return obj[key]


def _type_name(v: Any) -> str:
    if v is None:
        return "null"
    if isinstance(v, bool):
        return "bool"
    if isinstance(v, int) and not isinstance(v, bool):
        return "integer"
    if isinstance(v, float):
        return "number"
    if isinstance(v, str):
        return "string"
    if isinstance(v, list):
        return "array"
    if isinstance(v, dict):
        return "object"
    return type(v).__name__


def validate_sku_brief(data: dict[str, Any]) -> list[str]:
    errors: list[str] = []
    if data.get("schema_version") != "1":
        errors.append("schema_version must be '1'")
    for key in (
        "product_id",
        "sku_name",
        "lane",
        "research_question",
        "findings",
        "gaps_and_opportunities",
        "planner_recommendation",
        "information_gaps",
    ):
        _require(data, key, errors)
    lane = data.get("lane")
    if lane and lane not in {
        "juce-factory",
        "content-vital",
        "disklordz",
        "tr808-pwa",
        "hise",
    }:
        errors.append(f"invalid lane: {lane}")
    findings = data.get("findings")
    if findings is not None:
        if not isinstance(findings, list) or len(findings) < 1:
            errors.append("findings must be a non-empty array")
        else:
            for i, f in enumerate(findings):
                if not isinstance(f, dict):
                    errors.append(f"findings[{i}] must be object")
                    continue
                if "finding" not in f or "confidence" not in f:
                    errors.append(f"findings[{i}] needs finding + confidence")
                elif f.get("confidence") not in ("high", "medium", "low"):
                    errors.append(f"findings[{i}].confidence invalid")
    rec = data.get("planner_recommendation")
    if isinstance(rec, dict):
        if rec.get("decision") not in ("proceed", "defer", "kill", "needs_human"):
            errors.append("planner_recommendation.decision invalid")
        if not rec.get("rationale"):
            errors.append("planner_recommendation.rationale required")
    elif rec is not None:
        errors.append("planner_recommendation must be object")
    return errors


def validate_ship_evaluation(data: dict[str, Any]) -> list[str]:
    errors: list[str] = []
    if data.get("schema_version") != "1":
        errors.append("schema_version must be '1'")
    for key in (
        "artifact_type",
        "artifact_paths",
        "original_request",
        "overall_quality",
        "quality_assessment",
        "assessment_summary",
        "ready_for_approval",
    ):
        _require(data, key, errors)
    qa = data.get("quality_assessment")
    if qa and qa not in ("ACCEPT", "REVISE", "REJECT"):
        errors.append("quality_assessment must be ACCEPT|REVISE|REJECT")
    oq = data.get("overall_quality")
    if oq is not None and (not isinstance(oq, int) or oq < 1 or oq > 10):
        errors.append("overall_quality must be integer 1-10")
    paths = data.get("artifact_paths")
    if paths is not None and (not isinstance(paths, list) or len(paths) < 1):
        errors.append("artifact_paths must be non-empty array")
    rd = data.get("release_decision")
    if rd and rd not in (
        "RELEASE BLOCKED",
        "RC OK",
        "BETA OK",
        "RETAIL BLOCKED",
    ):
        errors.append("release_decision invalid")
    return errors


def validate_content_draft(data: dict[str, Any]) -> list[str]:
    errors: list[str] = []
    if data.get("schema_version") != "1":
        errors.append("schema_version must be '1'")
    for key in ("content_type", "sku_or_product", "title", "body_markdown", "metadata"):
        _require(data, key, errors)
    body = data.get("body_markdown")
    if isinstance(body, str) and len(body) < 50:
        errors.append("body_markdown too short (min 50 chars)")
    meta = data.get("metadata")
    if isinstance(meta, dict):
        if meta.get("publish_status") not in ("draft", "owner_review", "published"):
            errors.append("metadata.publish_status invalid")
    return errors


def validate_director_manifest(data: dict[str, Any]) -> list[str]:
    errors: list[str] = []
    if data.get("schema_version") != "1":
        errors.append("manifest schema_version must be '1'")
    for key in ("slug", "status", "user_request"):
        _require(data, key, errors)
    status = data.get("status")
    if status and status not in {
        "planned",
        "in_progress",
        "complete",
        "blocked",
        "incomplete",
    }:
        errors.append("manifest status invalid")
    expected = data.get("specialists_expected")
    if expected is not None and not isinstance(expected, list):
        errors.append("specialists_expected must be array")
    return errors


def validate_specialist_output(data: dict[str, Any], prefix: str = "") -> list[str]:
    errors: list[str] = []
    p = prefix

    def err(msg: str) -> None:
        errors.append(f"{p}{msg}")

    if data.get("schema_version") != "1":
        err("schema_version must be '1'")
    agent = data.get("agent")
    if agent not in {
        "research_agent",
        "creative_agent",
        "copy_agent",
        "media_agent",
        "analytics_agent",
    }:
        err("agent must be a specialist id")
    if data.get("status") not in ("ok", "incomplete", "error"):
        err("status must be ok|incomplete|error")
    output = data.get("output")
    if output is not None and not isinstance(output, dict):
        err("output must be object")
    return errors


def validate_compliance_verdict(data: dict[str, Any], prefix: str = "") -> list[str]:
    errors: list[str] = []
    p = prefix

    def err(msg: str) -> None:
        errors.append(f"{p}{msg}")

    if data.get("schema_version") != "1":
        err("schema_version must be '1'")
    sev = data.get("severity")
    if sev not in {"NONE", "LOW", "MEDIUM", "HIGH", "CRITICAL"}:
        err("severity invalid")
    if "ready_to_publish" not in data:
        err("missing ready_to_publish")
    issues = data.get("issues")
    if issues is not None and not isinstance(issues, list):
        err("issues must be array")
    if sev in {"MEDIUM", "HIGH", "CRITICAL"} and data.get("ready_to_publish") is True:
        err("ready_to_publish must be false when severity >= MEDIUM")
    return errors
