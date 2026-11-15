"""Classify plugin-queue jobs and pick free vs Cursor agents."""

from __future__ import annotations

import re
from typing import Any, Literal

JobTier = Literal["easy", "hard"]
AgentId = Literal["gemini", "aider", "jules", "cursor", "cline"]

EASY_HINTS = (
    r"merge\s*conflict",
    r"\bci\b",
    r"lint",
    r"workflow",
    r"\.ya?ml",
    r"docs?\b",
    r"markdown",
    r"readme",
    r"typo",
    r"test\s*fix",
    r"unit\s*test",
    r"failing\s*check",
    r"cmake\s*fix",
    r"clang",
)

HARD_HINTS = (
    r"\bdsp\b",
    r"juce",
    r"\bgui\b",
    r"plugineditor",
    r"\bmvp\b",
    r"processblock",
    r"\bsynth\b",
    r"\beffect\b",
    r"wavetable",
    r"vst3",
    r"pluginval",
)

FREE_AGENT_ORDER: tuple[AgentId, ...] = ("gemini", "aider", "jules")


def classify_job(
    item: dict,
    *,
    pr: dict | None = None,
    task_text: str | None = None,
) -> JobTier:
    """Return easy vs hard tier for routing."""
    explicit = (item.get("job_tier") or item.get("tier") or "").strip().lower()
    if explicit in ("easy", "hard"):
        return explicit  # type: ignore[return-value]

    blob_parts = [
        item.get("title") or "",
        item.get("acceptance") or "",
        task_text or "",
    ]
    if pr:
        blob_parts.extend([pr.get("title") or "", pr.get("body") or ""])
    blob = "\n".join(blob_parts).lower()

    for pat in EASY_HINTS:
        if re.search(pat, blob, re.I):
            return "easy"
    for pat in HARD_HINTS:
        if re.search(pat, blob, re.I):
            return "hard"

    # Plugin queue defaults: MVP / DSP work is hard unless easy hints matched above.
    if item.get("id", "").startswith("plugin-"):
        return "hard"
    return "easy"


def secret_available(name: str, env: dict[str, str]) -> bool:
    return bool((env.get(name) or "").strip())


def pick_easy_agent(env: dict[str, str]) -> AgentId | None:
    """First configured free agent in fallback order."""
    if secret_available("GEMINI_API_KEY", env):
        return "gemini"
    if secret_available("OPENROUTER_API_KEY", env):
        return "aider"
    # Jules uses GitHub issue label `jules` — needs repo token + Jules GitHub app, not always a secret.
    if secret_available("GITHUB_TOKEN", env) or secret_available("GH_TOKEN", env):
        return "jules"
    return None


def pick_agent(
    item: dict,
    *,
    pr: dict | None,
    task_text: str | None,
    env: dict[str, str],
) -> tuple[JobTier, AgentId]:
    tier = classify_job(item, pr=pr, task_text=task_text)
    if tier == "hard":
        return tier, "cursor"
    free = pick_easy_agent(env)
    if free:
        return tier, free
    if secret_available("CURSOR_API_KEY", env):
        return tier, "cursor"
    return tier, "jules"


def pr_assignment_key(pr_number: int | None, slice_id: str) -> str:
    if pr_number is not None:
        return f"pr:{pr_number}"
    return f"slice:{slice_id}"


def job_already_assigned(state: dict, pr_number: int | None, slice_id: str) -> bool:
    key = pr_assignment_key(pr_number, slice_id)
    assigned = state.get("assigned_agents") or {}
    return key in assigned


def record_assignment(
    state: dict,
    *,
    agent: AgentId,
    pr_number: int | None,
    slice_id: str,
    tier: JobTier,
    extra: dict[str, Any] | None = None,
) -> None:
    key = pr_assignment_key(pr_number, slice_id)
    row: dict[str, Any] = {
        "agent": agent,
        "tier": tier,
        "slice_id": slice_id,
        "pr": pr_number,
    }
    if extra:
        row.update(extra)
    state.setdefault("assigned_agents", {})[key] = row