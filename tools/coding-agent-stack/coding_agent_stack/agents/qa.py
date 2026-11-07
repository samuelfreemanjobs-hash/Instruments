from __future__ import annotations

from dataclasses import dataclass

from coding_agent_stack.agents.coder import CoderResult
from coding_agent_stack.harness import run_pytest_smoke, run_vst_autopilot_compile
from coding_agent_stack.schema import ImplementationPlan
from coding_agent_stack.tools import dispatch_tool


@dataclass
class QAResult:
    passed: bool
    output: str
    fix_instructions: str = ""


def qa_review(plan: ImplementationPlan, coder: CoderResult) -> QAResult:
    """QA Reviewer — run harness; on failure, instruction bundle for Coder retry."""
    if plan.harness == "vst_compile":
        extra = ["--no-configure"]
        if plan.harness_target:
            extra.extend(["--target", plan.harness_target])
        hr = run_vst_autopilot_compile(extra)
    elif plan.harness == "agent_tests":
        hr = run_pytest_smoke()
    else:
        return QAResult(True, "No harness required.", "")

    if hr.ok:
        return QAResult(True, hr.output[:8000], "")

    error_log = ""
    if hr.error_log_path and hr.error_log_path.is_file():
        error_log = hr.error_log_path.read_text(encoding="utf-8", errors="replace")[-4000:]

    fix = (
        "QA failed. Re-read error_log.txt and adjust sandbox draft only.\n"
        f"Command: {hr.command}\n"
        f"Output tail:\n{hr.output[-3000:]}\n"
    )
    if error_log:
        fix += f"\nerror_log.txt:\n{error_log}"

    # Optional: re-run read of error log via tool for coder context
    if plan.harness == "vst_compile":
        dispatch_tool("read_file", {"rel_path": "vst-testing-ops/error_log.txt", "max_bytes": 8000})

    return QAResult(False, hr.output[:8000], fix)
