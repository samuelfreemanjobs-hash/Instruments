#!/usr/bin/env python3
"""Next human-facing task for DiskLordz Audio Coach."""

from __future__ import annotations

import json
from pathlib import Path
from typing import Any

ROOT = Path(__file__).resolve().parents[1]
DATA = ROOT / "data"


def _load(name: str) -> Any:
    return json.loads((DATA / name).read_text(encoding="utf-8"))


def progress() -> dict:
    path = DATA / "progress.json"
    if path.is_file():
        return json.loads(path.read_text(encoding="utf-8"))
    return json.loads((DATA / "progress.template.json").read_text(encoding="utf-8"))


def next_task() -> dict | None:
    checklist = _load("sprint-checklist.json")
    playbooks = _load("task-playbooks.json")
    done = set(progress().get("completed_ids") or [])
    items = checklist.get("items") or []

    for item in items:
        tid = item["id"]
        if tid in done:
            continue
        book = playbooks.get(tid) or {}
        return {
            "id": tid,
            "label": item.get("label", tid),
            "phase": item.get("phase"),
            "headline": book.get("headline", item.get("label", tid)),
            "why": book.get("why", ""),
            "owner": book.get("owner", "you"),
            "minutes": book.get("minutes", 30),
            "steps": book.get("steps") or ["Open the Coach app and follow the checklist."],
            "done_when": book.get("done_when", "Marked complete in Coach."),
        }
    return None


def sprint_stats() -> tuple[int, int]:
    checklist = _load("sprint-checklist.json")
    done = set(progress().get("completed_ids") or [])
    items = checklist.get("items") or []
    n_done = sum(1 for i in items if i["id"] in done)
    return n_done, len(items)


def format_outreach_markdown(task: dict | None, *, progress_path: str | None = None) -> str:
    n_done, total = sprint_stats()
    if task is None:
        return (
            f"## DiskLordz Audio Coach\n\n"
            f"**Sprint complete** ({n_done}/{total}).\n\n"
            f"Next: open weekly review and plan beta — `python3 disklordz/audio-command/scripts/weekly_review.py --write`"
        )

    owner_line = "**Your move**" if task["owner"] == "you" else "**Delegate to Cursor** (you just approve)"
    steps = "\n".join(f"{i + 1}. {s}" for i, s in enumerate(task["steps"]))
    path = progress_path or "disklordz/audio-command/pwa/index.html"

    return f"""## DiskLordz Audio — your task right now

**{task['headline']}**

{owner_line} · ~{task['minutes']} min · Sprint **{n_done + 1}/{total}**

**Why:** {task['why'] or 'Moves Milestone 001 forward.'}

### Do this
{steps}

**Done when:** {task['done_when']}

---
Open Coach: `{path}` · Mark done when finished so the next task surfaces automatically.
"""


def format_slack_blocks(task: dict | None) -> dict:
    text = format_outreach_markdown(task)
    return {"text": text}


if __name__ == "__main__":
    t = next_task()
    print(format_outreach_markdown(t))
