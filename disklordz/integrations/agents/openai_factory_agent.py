#!/usr/bin/env python3
"""OpenAI Agents SDK stub for product-factory batch planning (openai/openai-agents-python)."""

from __future__ import annotations

import json
import os
import sys


def main() -> None:
    prompt = " ".join(sys.argv[1:]) or "phonk kit SKU metadata"
    if not os.environ.get("OPENAI_API_KEY"):
        print(
            json.dumps(
                {
                    "status": "stub",
                    "error": "openai_not_configured",
                    "hint": "Set OPENAI_API_KEY or use /api/factory/batch",
                    "prompt_echo": prompt[:120],
                }
            )
        )
        sys.exit(0)
    try:
        import agents  # noqa: F401
    except ImportError:
        print(
            json.dumps(
                {
                    "error": "openai_agents_not_installed",
                    "hint": "pip install openai-agents",
                }
            )
        )
        sys.exit(1)
    print(json.dumps({"status": "ready", "message": "Wire Agent workflow for factory SKUs"}))


if __name__ == "__main__":
    main()
