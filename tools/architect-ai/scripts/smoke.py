#!/usr/bin/env python3
"""Offline smoke: imports, context loader, system prompt contract."""

from __future__ import annotations

import sys
import tempfile
from pathlib import Path

ROOT = Path(__file__).resolve().parent.parent
sys.path.insert(0, str(ROOT))

from architect_ai.context import read_context_paths  # noqa: E402
from architect_ai.prompts import ARCHITECTAI_SYSTEM_PROMPT, COSI_REVIEW_APPENDIX  # noqa: E402
from architect_ai.session import ArchitectSession  # noqa: E402


class _StubModel:
    def chat(self, system: str, messages: list[dict[str, str]]) -> str:
        assert "ArchitectAI" in system
        assert messages[-1]["role"] == "user"
        return "stub ok"


def main() -> int:
    assert "COSI" in ARCHITECTAI_SYSTEM_PROMPT
    assert "Components" in COSI_REVIEW_APPENDIX

    with tempfile.TemporaryDirectory() as td:
        p = Path(td) / "sample.py"
        p.write_text("def domain(): return 1\n", encoding="utf-8")
        ctx = read_context_paths([p])
        assert "domain" in ctx

    session = ArchitectSession(_StubModel(), force_cosi=True)
    out = session.ask("hello")
    assert out == "stub ok"
    assert len(session.history) == 2

    print("architect-ai smoke: ok")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
