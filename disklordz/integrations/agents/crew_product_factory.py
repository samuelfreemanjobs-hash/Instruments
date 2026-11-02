#!/usr/bin/env python3
"""Outline CrewAI roles for storefront pack generation (optional)."""

from __future__ import annotations

import json
import sys


def main() -> None:
    prompt = " ".join(sys.argv[1:]) or "phonk drum pack SKU"
    try:
        from crewai import Agent, Crew, Task
    except ImportError:
        print(
            json.dumps(
                {
                    "error": "crewai_not_installed",
                    "hint": "pip install -r disklordz/integrations/requirements-optional.txt",
                    "roles": [
                        "lane_writer",
                        "metadata_qa",
                        "folder_layout",
                    ],
                }
            )
        )
        sys.exit(0)

    writer = Agent(
        role="Lane writer",
        goal="Draft Disklordz pack prompts with lane vocabulary",
        backstory="Expert in phonk, drift, and French touch lanes.",
    )
    qa = Agent(
        role="Metadata QA",
        goal="Ensure BPM, key, and SKU fields match GenerationSpec",
        backstory="Prevents bad storefront ZIPs.",
    )
    task = Task(
        description=f"Propose pack structure for: {prompt}",
        expected_output="JSON with folders 01_KICKS..04_PERC and 3 sample prompts",
        agent=writer,
    )
    crew = Crew(agents=[writer, qa], tasks=[task], verbose=False)
    result = crew.kickoff()
    print(json.dumps({"result": str(result)}))


if __name__ == "__main__":
    main()
