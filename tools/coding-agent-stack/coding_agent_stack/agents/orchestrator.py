from __future__ import annotations

from typing import Any

from coding_agent_stack.schema import ImplementationPlan


def orchestrate(user_prompt: str) -> dict[str, Any]:
    """Route intent and set team metadata (no LLM required in stub path)."""
    lower = user_prompt.lower()
    lane = "general"
    if any(k in lower for k in ("vst", "juce", "cmake", "plugin", "compile")):
        lane = "vst"
    elif any(k in lower for k in ("github", "pr", "pull request", "review")):
        lane = "pr_review"
    elif any(k in lower for k in ("grok", "delegate", "cursor cloud")):
        lane = "outer_loop"
    return {
        "lane": lane,
        "user_prompt": user_prompt,
        "use_sandbox_writes": True,
        "delegate_eligible": lane in ("outer_loop", "pr_review"),
    }
