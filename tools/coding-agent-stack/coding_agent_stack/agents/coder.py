from __future__ import annotations

from dataclasses import dataclass, field

from coding_agent_stack.schema import ImplementationPlan, PlanStep
from coding_agent_stack.sandbox import LocalSandbox
from coding_agent_stack.tools import dispatch_tool


@dataclass
class CoderResult:
    log: list[str] = field(default_factory=list)
    sandbox_files: dict[str, str] = field(default_factory=dict)
    tool_calls: int = 0


def _run_step(step: PlanStep, sandbox: LocalSandbox, result: CoderResult) -> None:
    if step.action == "read":
        rel = step.args.get("rel_path", step.detail.split(": ")[-1])
        out = dispatch_tool("read_file", {"rel_path": rel})
        result.log.append(f"[read] {rel}\n{out[:2500]}")
        result.tool_calls += 1
    elif step.action == "grep":
        out = dispatch_tool(
            "grep_repo",
            {
                "pattern": step.args.get("pattern", "processBlock"),
                "glob": step.args.get("glob", "*.cpp"),
            },
        )
        result.log.append(f"[grep]\n{out[:2500]}")
        result.tool_calls += 1
    elif step.action == "sandbox_python":
        code = step.args.get("code", "print('ok')")
        use_e2b = bool(step.args.get("use_e2b", False))
        out = dispatch_tool("execute_python", {"code": code, "use_e2b": use_e2b})
        result.log.append(f"[sandbox_python]\n{out[:2500]}")
        result.tool_calls += 1
    elif step.action == "harness":
        target = step.args.get("target", "")
        if step.detail.lower().find("vst") >= 0 or target:
            out = dispatch_tool("harness_vst_compile", {"target": target})
        else:
            out = dispatch_tool("harness_agent_tests", {})
        result.log.append(f"[harness]\n{out[:6000]}")
        result.tool_calls += 1
    elif step.action == "note":
        result.log.append(f"[note] {step.detail}")
    else:
        result.log.append(f"[skip] unknown action {step.action}")


def sandbox_coder_execute(plan: ImplementationPlan, use_e2b: bool = False) -> CoderResult:
    """Sandbox Coder — tools only; repo writes stay in sandbox dir when added."""
    result = CoderResult()
    sandbox = LocalSandbox()
    for step in plan.steps:
        if step.action == "sandbox_python" and use_e2b:
            step.args["use_e2b"] = True
        _run_step(step, sandbox, result)
    scaffold = sandbox.root / "draft.py"
    scaffold.write_text(
        f"# Draft for: {plan.goal[:200]}\n# Generated in isolated sandbox — not applied to repo.\n",
        encoding="utf-8",
    )
    result.sandbox_files[str(scaffold.name)] = scaffold.read_text(encoding="utf-8")
    return result
