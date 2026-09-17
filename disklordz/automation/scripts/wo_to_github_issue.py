#!/usr/bin/env python3
"""Sprint A1: Create a GitHub issue from an Airtable Agent Work Order."""

from __future__ import annotations

import argparse
import json
import os
import sys
import urllib.error
import urllib.parse
import urllib.request
from pathlib import Path

API_ROOT = "https://api.airtable.com/v0"
GITHUB_API_ROOT = "https://api.github.com"
AUTOMATION_ENV = Path(__file__).resolve().parents[1] / ".env"
DEFAULT_LABELS = ("cloud-agent", "work-order")


def load_dotenv(path: Path) -> None:
    if not path.is_file():
        return
    for line in path.read_text(encoding="utf-8").splitlines():
        line = line.strip()
        if not line or line.startswith("#") or "=" not in line:
            continue
        key, value = line.split("=", 1)
        os.environ.setdefault(key.strip(), value.strip())


def airtable_get_wo(work_order_id: str) -> dict:
    token = os.environ["AIRTABLE_API_KEY"]
    base = os.environ["AIRTABLE_BASE_ID"]
    table = os.environ.get("AIRTABLE_TABLE_WORK_ORDERS", "Agent Work Orders")
    formula = urllib.parse.quote(f"{{work_order_id}}='{work_order_id}'")
    url = f"{API_ROOT}/{base}/{urllib.parse.quote(table)}?maxRecords=1&filterByFormula={formula}"
    req = urllib.request.Request(url, headers={"Authorization": f"Bearer {token}"})
    with urllib.request.urlopen(req, timeout=60) as resp:
        data = json.loads(resp.read().decode("utf-8"))
    records = data.get("records") or []
    if not records:
        raise SystemExit(f"Work order not found: {work_order_id}")
    return records[0]


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


def create_issue(repo: str, title: str, body: str, labels: tuple[str, ...]) -> str:
    token = os.environ.get("GITHUB_TOKEN")
    if not token:
        raise SystemExit("Missing GITHUB_TOKEN")
    owner, separator, name = repo.partition("/")
    if not separator or not owner or not name:
        raise SystemExit(f"Invalid GitHub repository: {repo!r}; expected OWNER/REPOSITORY")

    url = f"{GITHUB_API_ROOT}/repos/{urllib.parse.quote(owner)}/{urllib.parse.quote(name)}/issues"
    payload = json.dumps({"title": title, "body": body, "labels": list(labels)}).encode("utf-8")
    req = urllib.request.Request(
        url,
        data=payload,
        headers={
            "Accept": "application/vnd.github+json",
            "Authorization": f"Bearer {token}",
            "Content-Type": "application/json",
            "X-GitHub-Api-Version": "2022-11-28",
        },
        method="POST",
    )
    try:
        with urllib.request.urlopen(req, timeout=60) as resp:
            result = json.loads(resp.read().decode("utf-8"))
    except urllib.error.HTTPError as error:
        detail = error.read().decode("utf-8", errors="replace")
        raise SystemExit(f"GitHub issue creation failed ({error.code}): {detail}") from error

    issue_url = result.get("html_url")
    if not issue_url:
        raise SystemExit("GitHub returned an issue without html_url")
    return issue_url


def main() -> None:
    load_dotenv(AUTOMATION_ENV)
    parser = argparse.ArgumentParser()
    parser.add_argument("--work-order-id", required=True)
    parser.add_argument("--repo", default=os.environ.get("GITHUB_REPO", "samuelfreemanjobs-hash/Instruments"))
    parser.add_argument("--dry-run", action="store_true")
    args = parser.parse_args()

    if args.dry_run:
        title = f"[{args.work_order_id}] Work Order → GitHub Issue"
        print(title)
        print("Dry run — no Airtable or GitHub calls.")
        return

    missing = [name for name in ("AIRTABLE_API_KEY", "AIRTABLE_BASE_ID", "GITHUB_TOKEN") if not os.environ.get(name)]
    if missing:
        print(f"Missing {', '.join(missing)}", file=sys.stderr)
        sys.exit(1)

    record = airtable_get_wo(args.work_order_id)
    fields = record["fields"]
    work_order_id = fields.get("work_order_id", args.work_order_id)
    title = f"[{work_order_id}] {fields.get('title', 'Work order')}"
    body = (
        f"## Work Order\n{work_order_id}\n\n"
        f"**Team:** {fields.get('assigned_team')} / {fields.get('assigned_role')}\n\n"
        f"**Product:** {fields.get('product', '—')}\n\n"
        f"### Acceptance criteria\n{fields.get('acceptance_criteria', '')}\n\n"
        f"---\n_Airtable → GitHub via `wo_to_github_issue.py` (Sprint A1)._"
    )
    if fields.get("github_issue"):
        print(f"Already linked: {fields['github_issue']}")
        return

    issue_url = create_issue(args.repo, title, body, DEFAULT_LABELS)
    patch_wo(record["id"], {"github_issue": issue_url, "status": "In Progress"})
    print(f"Created {issue_url}")


if __name__ == "__main__":
    main()
