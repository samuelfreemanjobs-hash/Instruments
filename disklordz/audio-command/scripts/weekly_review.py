#!/usr/bin/env python3
"""Generate DiskLordz Audio weekly development review markdown."""

from __future__ import annotations

import argparse
import json
from datetime import datetime, timezone
from pathlib import Path

ROOT = Path(__file__).resolve().parents[1]
DATA = ROOT / "data"
REVIEWS = ROOT / "reviews"


def load_json(name: str) -> dict:
    path = DATA / name
    if not path.is_file():
        raise SystemExit(f"missing {path}")
    return json.loads(path.read_text(encoding="utf-8"))


def iso_week() -> str:
    now = datetime.now(timezone.utc)
    return f"{now.year}-W{now.isocalendar().week:02d}"


def progress_snapshot() -> dict:
    committed = DATA / "progress.json"
    if committed.is_file():
        return json.loads(committed.read_text(encoding="utf-8"))
    return json.loads((DATA / "progress.template.json").read_text(encoding="utf-8"))


def render(progress: dict, checklist: dict, milestones: dict) -> str:
    items = checklist.get("items", [])
    completed = set(progress.get("completed_ids") or [])
    done = [i for i in items if i["id"] in completed]
    open_items = [i for i in items if i["id"] not in completed]

    phase = progress.get("current_phase", "foundation")
    matching = [p for p in milestones.get("phases", []) if p.get("checklist_phase") == phase]
    phase_blurb = matching[0] if matching else {}

    lines = [
        f"# DiskLordz Audio — weekly review ({iso_week()})",
        "",
        f"_Generated {datetime.now(timezone.utc).strftime('%Y-%m-%d %H:%M UTC')}_",
        "",
        "## Snapshot",
        "",
        f"| Field | Value |",
        f"|-------|-------|",
        f"| Phase | {phase} |",
        f"| Sprint progress | {len(done)} / {len(items)} |",
        f"| Build status | {progress.get('build_status', 'unknown')} |",
        f"| Open bugs | {progress.get('open_bugs', 0)} |",
        f"| Next milestone | {progress.get('next_milestone', 'Milestone 001')} |",
        "",
    ]
    if phase_blurb:
        lines.extend(
            [
                f"**Year-1 focus:** {phase_blurb.get('name')} → _{phase_blurb.get('deliverable')}_",
                "",
            ]
        )

    lines.extend(["## Completed (sprint)", ""])
    if done:
        lines.extend(f"- {i['label']}" for i in done)
    else:
        lines.append("- (none logged in progress.json — use PWA export or edit data/progress.json)")

    lines.extend(["", "## Outstanding (sprint)", ""])
    if open_items:
        lines.extend(f"- {i['label']}" for i in open_items)
    else:
        lines.append("- All sprint items marked complete")

    notes = progress.get("notes") or {}
    weekly = notes.get("weekly") if isinstance(notes, dict) else None
    lines.extend(["", "## Operator notes", "", weekly or "_(none)_", ""])

    lines.extend(
        [
            "## Fleet",
            "",
            "Run `python3 disklordz/audio-command/scripts/fleet_briefing.py` for agent roster.",
            "",
            "## Docs",
            "",
            "- docs/DISKLORDZ_MASTER_PLAN.md",
            "- docs/DPF_TECHNICAL_ARCHITECTURE.md",
            "- docs/CHORD_ENGINE_PRD.md",
            "",
            "## Engineering lane",
            "",
            "VST/C++: Cursor Cloud + `golden-wav-qa` (`run_business.py --profile ci`).",
            "Business/planning: `audio-business-copilot` agent.",
            "",
        ]
    )
    return "\n".join(lines)


def main() -> None:
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--write", action="store_true", help="Write to reviews/YYYY-Www.md")
    args = parser.parse_args()

    progress = progress_snapshot()
    checklist = load_json("sprint-checklist.json")
    milestones = load_json("milestones-year1.json")
    md = render(progress, checklist, milestones)

    if args.write:
        REVIEWS.mkdir(parents=True, exist_ok=True)
        out = REVIEWS / f"{iso_week()}.md"
        out.write_text(md, encoding="utf-8")
        print(f"wrote {out}")
    else:
        print(md)


if __name__ == "__main__":
    main()
