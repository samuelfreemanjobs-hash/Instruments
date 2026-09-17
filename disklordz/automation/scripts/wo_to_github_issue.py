#!/usr/bin/env python3
"""Sprint A1: Create GitHub issue from an Agent Work Order (Airtable row or CLI)."""

from __future__ import annotations

import argparse
import json
import os
import subprocess
import sys
import urllib.parse
import urllib.request
from pathlib import Path

API_ROOT = "https://api.airtable.com/v0"
AUTOMATION_ENV = Path(__file__).resolve().parents[1] / ".env"


def load_dotenv(path: Path) -> None:
    if not path.is_file():
        return
    for line in path.read_text(encoding="utf-8").splitlines():
        line = line.strip()
        if not line or line.startswith("#") or "=" not in line:
            continue
        k, v = line.split("=", 1)
        os.environ.setdefault(k.strip(), v.strip())


def airtable_get_wo(work_order_id: str) -> dict:
    token = os.environ["AIRTABLE_API_KEY"]
    base = os.environ["AIRTABLE_BASE_ID"]
    table = os.environ.get("AIRTABLE_TABLE_WORK_ORDERS", "Agent Work Orders")
    formula = urllib.parse.quote(f"{{work_order_id}}='{work_order_id}'")
    url = f"{API_ROOT}/{base}/{urllib.parse.quote(table)}?maxRecords=1&filterByFormula={formula}"
    req = urllib.request.Request(url, headers={"Authorization": f"Bearer {token}"})
    with urllib.request.urlopen(req, timeout=60) as resp:
        data = json.loads(resp.read().decode("utf-8"))
    recs = data.get("records") or []
    if not recs:
        raise SystemExit(f"Work order not found: {work_order_id}")
    return recs[0]


def patch_wo(record_id: str, fields: dict) -> None:
    token = os.environ["AIRTABLE_API_KEY"]
    base = os.environ["AIRTABLE_BASE_ID"]
    table = os.environ.get("AIRTABLE_TABLE_WORK_ORDERS", "Agent Work Orders")
    url = f"{API_ROOT}/{base}/{urllib.parse.quote(table)}/{record_id}"
    body = json.dumps({"fields": fields, "typecast": True}).encode("utf-8")
    req = urllib.request.Request(
        url,
        data=body,
        headers={"Authorization": f"Bearer {token}", "Content-Type": "application/json"},
        method="PATCH",
    )
    with urllib.request.urlopen(req, timeout=60):
        pass


def create_issue(repo: str, title: str, body: str) -> str:
    proc = subprocess.run(
        ["gh", "issue", "create", "--repo", repo, "--title", title, "--body", body],
        capture_output=True,
        text=True,
        check=True,
    )
    url = proc.stdout.strip()
    return url


def main() -> None:
    load_dotenv(AUTOMATION_ENV)
    parser = argparse.ArgumentParser()
    parser.add_argument("--work-order-id", default="WO-2026-002")
    parser.add_argument("--repo", default=os.environ.get("GITHUB_REPO", "samuelfreemanjobs-hash/Instruments"))
    parser.add_argument("--dry-run", action="store_true")
    args = parser.parse_args()

    if not args.dry_run:
        if not os.environ.get("AIRTABLE_API_KEY") or not os.environ.get("AIRTABLE_BASE_ID"):
            print("Missing AIRTABLE_API_KEY or AIRTABLE_BASE_ID", file=sys.stderr)
            sys.exit(1)

    if args.dry_run:
        title = f"[{args.work_order_id}] Sprint A1 — Work Order → GitHub Issue"
        body = "Dry run — configure Airtable secrets to link WO rows."
        print(title)
        print(body)
        return

    rec = airtable_get_wo(args.work_order_id)
    fields = rec["fields"]
    wo_id = fields.get("work_order_id", args.work_order_id)
    title = f"[{wo_id}] {fields.get('title', 'Work order')}"
    body = (
        f"## Work Order\n{wo_id}\n\n"
        f"**Team:** {fields.get('assigned_team')} / {fields.get('assigned_role')}\n\n"
        f"### Acceptance criteria\n{fields.get('acceptance_criteria', '')}\n"
    )
    if fields.get("github_issue"):
        print(f"Already linked: {fields['github_issue']}")
        return

    issue_url = create_issue(args.repo, title, body)
    patch_wo(rec["id"], {"github_issue": issue_url, "status": "In Progress"})
    print(f"Created {issue_url}")


if __name__ == "__main__":
    main()
