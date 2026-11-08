#!/usr/bin/env python3
"""Draft GenerationSpec JSON via pydantic-ai (optional)."""

from __future__ import annotations

import json
import sys


def main() -> None:
    prompt = " ".join(sys.argv[1:]) or "memphis phonk cowbell 92 bpm"
    try:
        from pydantic import BaseModel, Field
        from pydantic_ai import Agent
    except ImportError:
        print(
            json.dumps(
                {
                    "error": "pydantic_ai_not_installed",
                    "hint": "pip install -r disklordz/integrations/requirements-optional.txt",
                }
            )
        )
        sys.exit(1)

    class Spec(BaseModel):
        mode: str = Field(default="one_shot")
        engine: str = Field(default="studio")
        bpm: int = Field(default=92, ge=60, le=200)
        key: str = Field(default="F# minor")

    agent = Agent(
        "openai:gpt-4o-mini",
        result_type=Spec,
        system_prompt="Map drum prompts to Disklordz GenerationSpec fields.",
    )
    result = agent.run_sync(prompt)
    print(result.data.model_dump_json())


if __name__ == "__main__":
    main()
