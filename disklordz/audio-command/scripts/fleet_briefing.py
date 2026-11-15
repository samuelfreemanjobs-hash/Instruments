#!/usr/bin/env python3
"""Print which profit agents should review the DiskLordz Audio plan this phase."""

from __future__ import annotations

import json
import sys
from pathlib import Path

ROOT = Path(__file__).resolve().parents[1]
DATA = ROOT / "data"


def main() -> None:
    phase = "foundation"
    progress_path = DATA / "progress.json"
    if progress_path.is_file():
        phase = json.loads(progress_path.read_text(encoding="utf-8")).get("current_phase", phase)
    elif len(sys.argv) > 1:
        phase = sys.argv[1]

    advisors = json.loads((DATA / "fleet-advisors.json").read_text(encoding="utf-8"))
    block = advisors.get("phases", {}).get(phase)
    if not block:
        print(f"Unknown phase {phase!r}. Known: {', '.join(advisors.get('phases', {}))}")
        raise SystemExit(1)

    print(f"# Fleet briefing — phase: {phase}\n")
    for key in ("primary", "engineering", "secondary"):
        if key in block:
            print(f"## {key}")
            for aid in block[key]:
                print(f"- `{aid}` → disklordz/agents/profit/{aid}/skill.md")
            print()
    if block.get("note"):
        print(f"Note: {block['note']}\n")

    print("## Canonical docs (all agents)")
    for doc in advisors.get("always_read_docs", []):
        print(f"- {doc}")


if __name__ == "__main__":
    main()
