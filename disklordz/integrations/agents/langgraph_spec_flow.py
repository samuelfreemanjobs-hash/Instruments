#!/usr/bin/env python3
"""Minimal LangGraph-shaped flow: prompt -> spec dict (stdlib fallback)."""

from __future__ import annotations

import json
import sys


def main() -> None:
    prompt = " ".join(sys.argv[1:]) or "cyber funk drums"
    try:
        from langgraph.graph import StateGraph
        from typing import TypedDict

        class State(TypedDict):
            prompt: str
            spec: dict

        def parse(state: State) -> State:
            return {
                "prompt": state["prompt"],
                "spec": {
                    "mode": "one_shot",
                    "engine": "creative" if "wild" in state["prompt"].lower() else "studio",
                    "bpm": 96,
                    "key": "A minor",
                },
            }

        g = StateGraph(State)
        g.add_node("parse", parse)
        g.set_entry_point("parse")
        g.set_finish_point("parse")
        app = g.compile()
        out = app.invoke({"prompt": prompt, "spec": {}})
        print(json.dumps(out["spec"]))
    except ImportError:
        print(
            json.dumps(
                {
                    "mode": "one_shot",
                    "engine": "studio",
                    "bpm": 96,
                    "key": "A minor",
                    "fallback": True,
                }
            )
        )


if __name__ == "__main__":
    main()
