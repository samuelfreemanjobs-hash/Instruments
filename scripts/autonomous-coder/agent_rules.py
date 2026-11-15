"""Shared agent instructions for Cursor, Gemini, Aider, Jules, and Cline."""

from __future__ import annotations

from pathlib import Path

REPO = Path(__file__).resolve().parents[2]
RULES_MD = REPO / "disklordz/autonomous-coder/agent-rules.md"


def agent_rules_block() -> str:
    if RULES_MD.is_file():
        return RULES_MD.read_text(encoding="utf-8").strip()
    return "Follow AGENTS.md. Never merge. Never commit secrets."