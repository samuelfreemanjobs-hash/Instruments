from __future__ import annotations

import json
from dataclasses import dataclass, field
from typing import Any, Literal, TypedDict

from coding_agent_stack.agents.architect import architect_plan, architect_with_llm
from coding_agent_stack.agents.coder import CoderResult, sandbox_coder_execute
from coding_agent_stack.agents.orchestrator import orchestrate
from coding_agent_stack.agents.qa import QAResult, qa_review
from coding_agent_stack.config import get_settings
from coding_agent_stack.schema import ImplementationPlan, PlanStep


class TeamState(TypedDict, total=False):
    user_prompt: str
    meta: dict[str, Any]
    plan: dict[str, Any]
    coder: dict[str, Any]
    qa: dict[str, Any]
    qa_passed: bool
    qa_attempts: int
    phase: str


@dataclass
class TeamRunResult:
    final_text: str
    mode: str
    plan: ImplementationPlan
    qa_passed: bool
    tool_calls: int
    phases: list[str] = field(default_factory=list)
    plan_json: str = ""


def _format_final(plan: ImplementationPlan, coder: CoderResult, qa: QAResult, meta: dict) -> str:
    parts = [
        f"## Goal\n{plan.goal}",
        f"## Architect summary\n{plan.summary}",
        "### Plan JSON",
        "```json",
        json.dumps(plan.model_dump(), indent=2)[:12000],
        "```",
        f"## Lane\n{meta.get('lane')}",
        f"## QA\n{'PASSED' if qa.passed else 'FAILED'}",
    ]
    if not qa.passed and qa.fix_instructions:
        parts.extend(["### Fix instructions (for Coder retry)", qa.fix_instructions[:4000]])
    parts.append("### Coder log (tail)")
    parts.append("\n".join(coder.log)[-8000:])
    if meta.get("delegate_eligible"):
        parts.append(
            "\n> Multi-file production edits: POST /delegate with target `grok` or `cursor_cloud`."
        )
    return "\n".join(parts)


def run_team_pipeline(user_prompt: str, force_stub: bool = False) -> TeamRunResult:
    """Orchestrator → Architect → Coder → QA (+ optional Coder retry)."""
    settings = get_settings()
    meta = orchestrate(user_prompt)
    phases = ["orchestrator"]

    plan: ImplementationPlan | None = None
    if not force_stub and settings.openai_api_key:
        plan = architect_with_llm(user_prompt)
        mode = "llm_architect" if plan else "stub"
    else:
        mode = "stub"
    if plan is None:
        plan = architect_plan(user_prompt, meta)
    phases.append("architect")

    qa_attempts = 0
    max_retries = plan.max_qa_retries
    coder = sandbox_coder_execute(plan, use_e2b=bool(settings.e2b_api_key))
    phases.append("coder")

    qa = qa_review(plan, coder)
    qa_attempts += 1
    phases.append("qa")

    while not qa.passed and qa_attempts <= max_retries:
        if plan.harness == "vst_compile":
            plan.steps.append(
                PlanStep(
                    action="read",
                    detail="QA retry: error log",
                    args={"rel_path": "vst-testing-ops/error_log.txt"},
                )
            )
        else:
            plan.steps.append(
                PlanStep(action="note", detail=qa.fix_instructions[:2000], args={})
            )
        coder = sandbox_coder_execute(plan, use_e2b=bool(settings.e2b_api_key))
        phases.append("coder_retry")
        qa = qa_review(plan, coder)
        qa_attempts += 1
        phases.append("qa_retry")

    final = _format_final(plan, coder, qa, meta)
    return TeamRunResult(
        final_text=final,
        mode=mode,
        plan=plan,
        qa_passed=qa.passed,
        tool_calls=coder.tool_calls,
        phases=phases,
        plan_json=json.dumps(plan.model_dump(), indent=2),
    )


def compile_langgraph_team():
    try:
        from langgraph.graph import END, START, StateGraph
    except ImportError:
        return None

    def orchestrator_node(state: TeamState) -> TeamState:
        meta = orchestrate(state["user_prompt"])
        return {"meta": meta, "phase": "orchestrator", "qa_attempts": 0}

    def architect_node(state: TeamState) -> TeamState:
        plan = architect_plan(state["user_prompt"], state.get("meta"))
        return {"plan": plan.model_dump(), "phase": "architect"}

    def coder_node(state: TeamState) -> TeamState:
        plan = ImplementationPlan.model_validate(state["plan"])
        settings = get_settings()
        coder = sandbox_coder_execute(plan, use_e2b=bool(settings.e2b_api_key))
        return {
            "coder": {"log": coder.log, "tool_calls": coder.tool_calls},
            "phase": "coder",
        }

    def qa_node(state: TeamState) -> TeamState:
        plan = ImplementationPlan.model_validate(state["plan"])
        coder_data = state.get("coder", {})
        coder = CoderResult(log=coder_data.get("log", []), tool_calls=coder_data.get("tool_calls", 0))
        qa = qa_review(plan, coder)
        attempts = int(state.get("qa_attempts", 0)) + 1
        return {
            "qa": {"passed": qa.passed, "output": qa.output, "fix": qa.fix_instructions},
            "qa_passed": qa.passed,
            "qa_attempts": attempts,
            "phase": "qa",
        }

    def route_after_qa(state: TeamState) -> Literal["coder", "end"]:
        if state.get("qa_passed"):
            return "end"
        plan = ImplementationPlan.model_validate(state["plan"])
        if int(state.get("qa_attempts", 0)) > plan.max_qa_retries:
            return "end"
        return "coder"

    g = StateGraph(TeamState)
    g.add_node("orchestrator", orchestrator_node)
    g.add_node("architect", architect_node)
    g.add_node("coder", coder_node)
    g.add_node("qa", qa_node)
    g.add_edge(START, "orchestrator")
    g.add_edge("orchestrator", "architect")
    g.add_edge("architect", "coder")
    g.add_edge("coder", "qa")
    g.add_conditional_edges("qa", route_after_qa, {"coder": "coder", "end": END})
    return g.compile()


def run_team_langgraph(user_prompt: str) -> TeamRunResult | None:
    app = compile_langgraph_team()
    if app is None:
        return None
    out = app.invoke({"user_prompt": user_prompt, "qa_attempts": 0})
    plan = ImplementationPlan.model_validate(out["plan"])
    qa_passed = bool(out.get("qa_passed"))
    coder = CoderResult(
        log=out.get("coder", {}).get("log", []),
        tool_calls=out.get("coder", {}).get("tool_calls", 0),
    )
    qa = QAResult(
        passed=qa_passed,
        output=out.get("qa", {}).get("output", ""),
        fix_instructions=out.get("qa", {}).get("fix", ""),
    )
    meta = out.get("meta", {})
    final = _format_final(plan, coder, qa, meta)
    return TeamRunResult(
        final_text=final,
        mode="langgraph",
        plan=plan,
        qa_passed=qa_passed,
        tool_calls=coder.tool_calls,
        phases=["langgraph"],
        plan_json=json.dumps(plan.model_dump(), indent=2),
    )


def run_team(user_prompt: str, force_stub: bool = False) -> TeamRunResult:
    if not force_stub:
        lg = run_team_langgraph(user_prompt)
        if lg is not None:
            return lg
    return run_team_pipeline(user_prompt, force_stub=force_stub)
