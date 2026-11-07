from __future__ import annotations

import json
import re

from coding_agent_stack.agents.orchestrator import orchestrate
from coding_agent_stack.config import get_settings
from coding_agent_stack.schema import ImplementationPlan, PlanStep


def architect_plan(user_prompt: str, meta: dict | None = None) -> ImplementationPlan:
    """Headless Architect — JSON implementation spec (stub or LLM-augmented later)."""
    meta = meta or orchestrate(user_prompt)
    lane = meta.get("lane", "general")
    settings = get_settings()

    files_read = ["docs/VST_AI_PLUGIN_MAP.md"] if lane == "vst" else ["ARCHITECTURE.md"]
    if lane == "pr_review":
        files_read = ["AGENTS.md"]

    steps: list[PlanStep] = []
    for rel in files_read:
        steps.append(PlanStep(action="read", detail=f"Load context: {rel}", args={"rel_path": rel}))

    harness = "agent_tests"
    harness_target = ""
    if lane == "vst":
        harness = "vst_compile"
        steps.append(
            PlanStep(
                action="harness",
                detail="Verify JUCE build via autopilot compile",
                args={"target": ""},
            )
        )
    else:
        steps.append(
            PlanStep(action="harness", detail="Self-test coding-agent-stack", args={})
        )

    steps.append(
        PlanStep(
            action="note",
            detail="Summarize findings for phone UI; multi-file edits → delegate to Cursor Cloud or Grok Build",
            args={},
        )
    )

    risks = []
    if lane == "vst":
        risks.append("Audio thread: no alloc/logging in processBlock")
    if meta.get("delegate_eligible"):
        risks.append("Large diffs should not run on phone — use outer-loop handoff")

    return ImplementationPlan(
        goal=user_prompt,
        summary=f"Lane={lane}; repo={settings.repo_root.name}",
        files_to_touch=[],
        files_to_read=files_read,
        steps=steps,
        harness=harness,
        harness_target=harness_target,
        max_qa_retries=2,
        risks=risks,
    )


def architect_plan_json(user_prompt: str) -> str:
    return json.dumps(architect_plan(user_prompt).model_dump(), indent=2)


def architect_with_llm(user_prompt: str) -> ImplementationPlan | None:
    settings = get_settings()
    if not settings.openai_api_key:
        return None
    try:
        from langchain_core.messages import HumanMessage, SystemMessage
        from langchain_openai import ChatOpenAI
    except ImportError:
        return None

    sys = (
        "You are the Headless Architect. Output ONLY valid JSON matching ImplementationPlan: "
        "goal, summary, files_to_touch, files_to_read, steps[{action,detail,args}], "
        "harness (agent_tests|vst_compile|none), harness_target, max_qa_retries, risks."
    )
    model = ChatOpenAI(model=settings.agent_model, temperature=0, api_key=settings.openai_api_key)
    raw = model.invoke([SystemMessage(content=sys), HumanMessage(content=user_prompt)])
    text = raw.content if isinstance(raw.content, str) else str(raw.content)
    m = re.search(r"\{.*\}", text, re.DOTALL)
    if not m:
        return None
    data = json.loads(m.group())
    return ImplementationPlan.model_validate(data)
