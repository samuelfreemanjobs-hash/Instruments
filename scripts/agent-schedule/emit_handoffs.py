#!/usr/bin/env python3
"""Emit git inbox handoffs for scheduled agent jobs (see disklordz/automation/scheduled_work.yaml)."""

from __future__ import annotations

import argparse
import json
import re
import subprocess
import sys
from datetime import datetime, timezone
from pathlib import Path

try:
    import yaml
except ImportError:
    yaml = None  # type: ignore

REPO_ROOT = Path(__file__).resolve().parents[2]
SCHEDULE_PATH = REPO_ROOT / "disklordz" / "automation" / "scheduled_work.yaml"
INBOX = REPO_ROOT / "disklordz" / "vst-factory" / "inbox"


def load_schedule() -> dict:
    if yaml is None:
        raise SystemExit("PyYAML required: pip install pyyaml")
    return yaml.safe_load(SCHEDULE_PATH.read_text(encoding="utf-8"))


def open_handoff_wo_ids() -> set[str]:
    ids: set[str] = set()
    if not INBOX.is_dir():
        return ids
    for path in INBOX.glob("HO-*.json"):
        try:
            doc = json.loads(path.read_text(encoding="utf-8"))
        except json.JSONDecodeError:
            continue
        if doc.get("status") != "open":
            continue
        wo = doc.get("work_order") or {}
        wid = wo.get("id")
        if wid:
            ids.add(str(wid))
    return ids


def new_handoff_id() -> str:
    day = datetime.now(timezone.utc).strftime("%Y%m%d")
    rand = subprocess.check_output(["openssl", "rand", "-hex", "2"], text=True).strip()
    return f"HO-{day}-{rand}"


def emit_job(job: dict, default_branch: str) -> Path | None:
    wo_id = job["work_order_id"]
    if wo_id in open_handoff_wo_ids():
        print(f"skip (open handoff exists): {wo_id}")
        return None

    hid = new_handoff_id()
    branch = job.get("branch") or default_branch
    doc = {
        "handoff_id": hid,
        "direction": "schedule_to_agent",
        "from_agent": "workflow-automation-agent",
        "to_agent": job["to_agent"],
        "created_at": datetime.now(timezone.utc).strftime("%Y-%m-%dT%H:%M:%SZ"),
        "work_order": {
            "id": wo_id,
            "title": job["title"],
            "acceptance_criteria": (job.get("acceptance_criteria") or "").strip(),
            "branch": branch,
        },
        "context_paths": list(job.get("context_paths") or []),
        "artifacts": [],
        "status": "open",
        "notes": (job.get("notes") or "").strip(),
        "schedule_job_id": job["id"],
    }
    INBOX.mkdir(parents=True, exist_ok=True)
    path = INBOX / f"{hid}.json"
    path.write_text(json.dumps(doc, indent=2) + "\n", encoding="utf-8")
    print(f"wrote {path} -> {job['to_agent']} ({wo_id})")
    return path


def jobs_for_cron(schedule: dict, cron: str) -> list[dict]:
    return [j for j in schedule.get("jobs") or [] if j.get("cron") == cron]


def main() -> int:
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument(
        "--cron",
        help="Cron expression matching scheduled_work.yaml entries (UTC)",
    )
    parser.add_argument("--dry-run", action="store_true")
    parser.add_argument("--list", action="store_true", help="List all scheduled jobs")
    args = parser.parse_args()

    schedule = load_schedule()
    default_branch = schedule.get("default_branch") or "main"

    if args.list:
        for j in schedule.get("jobs") or []:
            print(f"{j.get('cron')}\t{j.get('id')}\t{j.get('to_agent')}\t{j.get('work_order_id')}")
        return 0

    if not args.cron:
        parser.error("--cron is required unless --list is used")

    jobs = jobs_for_cron(schedule, args.cron)
    if not jobs:
        print(f"No jobs for cron {args.cron!r}", file=sys.stderr)
        return 1

    written: list[Path] = []
    for job in jobs:
        if args.dry_run:
            print(f"would emit: {job['id']} -> {job['to_agent']}")
            continue
        p = emit_job(job, default_branch)
        if p:
            written.append(p)

    if args.dry_run:
        return 0

    if not written:
        print("No new handoffs emitted.")
        return 0

    return 0


if __name__ == "__main__":
    raise SystemExit(main())
