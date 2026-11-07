from __future__ import annotations

"""Outer-loop handoffs: Grok Bot / Grok Build, Cursor Cloud, Claude Code.

Grok operates a persistent cloud VM (browser + terminal). This module does not
call xAI APIs directly — it packages structured briefs your PWA or Grok can act on.
"""

from typing import Literal

from pydantic import BaseModel, Field

from coding_agent_stack.schema import ImplementationPlan


class DelegateRequest(BaseModel):
    target: Literal["grok", "grok_build", "cursor_cloud", "claude_code"]
    user_prompt: str
    plan: ImplementationPlan | None = None


class DelegateResponse(BaseModel):
    target: str
    title: str
    brief_markdown: str
    suggested_cli: str = ""
    suggested_cursor_prompt: str = ""
    metadata: dict = Field(default_factory=dict)


def build_handoff(req: DelegateRequest) -> DelegateResponse:
    plan = req.plan
    plan_block = ""
    if plan:
        plan_block = (
            f"\n\n## Architect plan\n- Goal: {plan.goal}\n- Harness: {plan.harness}\n"
            f"- Read first: {', '.join(plan.files_to_read) or '(none)'}\n"
            f"- Risks: {', '.join(plan.risks) or '(none)'}\n"
        )

    base = f"{req.user_prompt}{plan_block}"

    if req.target == "grok":
        brief = (
            "# Grok Bot — Outer Loop task\n\n"
            "You are the **cloud general contractor**. Do not paste huge codeblocks here.\n"
            "1. Open GitHub repo for this project and read open PRs/issues.\n"
            "2. Compile a concise implementation brief for **Grok Build** or **Cursor Cloud Agent**.\n"
            "3. Run safe diagnostics in your cloud terminal; never touch local user files.\n\n"
            f"## User request\n{base}\n"
        )
        return DelegateResponse(
            target="grok",
            title="Grok outer loop",
            brief_markdown=brief,
            metadata={"role": "cloud_general_contractor"},
        )

    if req.target == "grok_build":
        brief = (
            "# Grok Build — coding harness\n\n"
            "Execute multi-file edits in your isolated environment from this spec.\n"
            "Run compile/tests before marking done.\n\n"
            f"{base}\n"
        )
        return DelegateResponse(
            target="grok_build",
            title="Grok Build handoff",
            brief_markdown=brief,
            metadata={"role": "coding_harness"},
        )

    if req.target == "cursor_cloud":
        prompt = (
            "Read ARCHITECTURE.md, AGENTS.md, and docs/VST_AI_PLUGIN_MAP.md if touching plugins.\n"
            f"Task: {req.user_prompt}\n"
            "Run vst-testing-ops CI profile before PR. Evidence required."
        )
        if plan:
            prompt += f"\nArchitect files_to_read: {plan.files_to_read}"
        return DelegateResponse(
            target="cursor_cloud",
            title="Cursor Cloud Agent",
            brief_markdown=prompt,
            suggested_cursor_prompt=prompt,
            metadata={"role": "multi_file_editor"},
        )

    cli = f'claude -p "{req.user_prompt[:500]}"'  # Claude Code style; user adjusts
    brief = (
        "# Claude Code (local foreground)\n\n"
        "Fast terminal/git/single-file work on the laptop.\n\n"
        f"```{req.user_prompt[:2000]}```\n"
    )
    return DelegateResponse(
        target="claude_code",
        title="Claude Code local",
        brief_markdown=brief,
        suggested_cli=cli,
        metadata={"role": "desktop_operator"},
    )
