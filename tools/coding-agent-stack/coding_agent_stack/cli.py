from __future__ import annotations

import argparse
import json
import sys

from coding_agent_stack.config import get_settings
from coding_agent_stack.graph import run_agent_llm, run_agent_stub
from coding_agent_stack.team_graph import run_team


def main() -> None:
    parser = argparse.ArgumentParser(description="Run coding agent once from CLI")
    parser.add_argument("prompt", nargs="*", help="User prompt")
    parser.add_argument("--stub", action="store_true", help="Force stub harness mode")
    parser.add_argument("--team", action="store_true", help="Run Architect→Coder→QA team")
    args = parser.parse_args()
    prompt = " ".join(args.prompt) or "Run agent harness tests"
    settings = get_settings()
    if args.team:
        team = run_team(prompt, force_stub=args.stub)
        print(team.final_text)
        result_mode = team.mode
        tool_calls = team.tool_calls
    elif args.stub or (not settings.openai_api_key and not settings.anthropic_api_key):
        result = run_agent_stub(prompt)
        print(result.final_text)
        result_mode = result.mode
        tool_calls = result.tool_calls
    else:
        result = run_agent_llm(prompt)
        print(result.final_text)
        result_mode = result.mode
        tool_calls = result.tool_calls
    if "--json" in sys.argv:
        print(json.dumps({"mode": result_mode, "tool_calls": tool_calls}))


if __name__ == "__main__":
    main()
