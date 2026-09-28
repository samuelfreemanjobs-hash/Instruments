#!/usr/bin/env python3
"""Create or update Agent Work Orders in Airtable from a JSON seed file."""

from __future__ import annotations

import argparse
import json
import os
import sys
import urllib.parse
import urllib.request
from pathlib import Path

API_ROOT = "https://api.airtable.com/v0"
DEFAULT_SEED = Path(__file__).resolve().parents[2] / "airtable/seed/work-orders-junova-2026.json"
AUTOMATION_ENV = Path(__file__).resolve().parents[1] / ".env"


def load_dotenv(path: Path) -> None:
    if not path.is_file():
        return
    for line in path.read_text(encoding="utf-8").splitlines():
        line = line.strip()
        if not line or line.startswith("#") or "=" not in line:
            continue
        key, value = line.split("=", 1)
        os.environ.setdefault(key.strip(), value.strip())


def airtable_request(method: str, url: str, body: dict | None = None) -> dict:
    token = os.environ["AIRTABLE_API_KEY"]
    data = None
    headers = {"Authorization": f"Bearer {token}"}
    if body is not None:
        data = json.dumps(body).encode("utf-8")
        headers["Content-Type"] = "application/json"
    req = urllib.request.Request(url, data=data, headers=headers, method=method)
    with urllib.request.urlopen(req, timeout=60) as resp:
        return json.loads(resp.read().decode("utf-8"))


def find_wo(base: str, table: str, work_order_id: str) -> dict | None:
    formula = urllib.parse.quote(f"{{work_order_id}}='{work_order_id}'")
    url = f"{API_ROOT}/{base}/{urllib.parse.quote(table)}?maxRecords=1&filterByFormula={formula}"
    data = airtable_request("GET", url)
    records = data.get("records") or []
    return records[0] if records else None


def upsert_record(base: str, table: str, fields: dict, dry_run: bool) -> str:
    wo_id = fields["work_order_id"]
    existing = find_wo(base, table, wo_id) if not dry_run else None
    if dry_run:
        action = "would upsert"
        return f"{action} {wo_id}: {fields.get('title', '')}"

    if existing:
        rid = existing["id"]
        url = f"{API_ROOT}/{base}/{urllib.parse.quote(table)}/{rid}"
        airtable_request("PATCH", url, {"fields": fields, "typecast": True})
        return f"Updated {wo_id} ({rid})"

    url = f"{API_ROOT}/{base}/{urllib.parse.quote(table)}"
    result = airtable_request("POST", url, {"fields": fields, "typecast": True})
    return f"Created {wo_id} ({result['id']})"


def main() -> None:
    load_dotenv(AUTOMATION_ENV)
    parser = argparse.ArgumentParser()
    parser.add_argument("--seed", type=Path, default=DEFAULT_SEED)
    parser.add_argument("--base-id", default=os.environ.get("AIRTABLE_BASE_ID", ""))
    parser.add_argument("--table", default="Agent Work Orders")
    parser.add_argument("--dry-run", action="store_true")
    args = parser.parse_args()

    seed = json.loads(args.seed.read_text(encoding="utf-8"))
    base = args.base_id or os.environ.get("AIRTABLE_BASE_ID", "")
    table = args.table or seed.get("table", "Agent Work Orders")

    if not args.dry_run:
        if not os.environ.get("AIRTABLE_API_KEY") or not base:
            print("Set AIRTABLE_API_KEY and AIRTABLE_BASE_ID (or --base-id).", file=sys.stderr)
            sys.exit(1)

    for rec in seed["records"]:
        print(upsert_record(base, table, rec, args.dry_run))


if __name__ == "__main__":
    main()
