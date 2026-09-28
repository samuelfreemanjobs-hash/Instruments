#!/usr/bin/env python3
"""Idempotent upsert of Products, Projects, and Work Orders from a bundle JSON seed."""

from __future__ import annotations

import argparse
import json
import os
import sys
import urllib.parse
import urllib.request
from pathlib import Path

API_ROOT = "https://api.airtable.com/v0"
DEFAULT_SEED = Path(__file__).resolve().parents[2] / "airtable/seed/plugin-tracks-2026.json"
AUTOMATION_ENV = Path(__file__).resolve().parents[1] / ".env"

TABLE_ORDER = [
    ("products", "Products", "product_id"),
    ("projects", "Projects", "project_id"),
    ("work_orders", "Agent Work Orders", "work_order_id"),
]


def load_dotenv(path: Path) -> None:
    if not path.is_file():
        return
    for line in path.read_text(encoding="utf-8").splitlines():
        line = line.strip()
        if not line or line.startswith("#") or "=" not in line:
            continue
        key, value = line.split("=", 1)
        os.environ.setdefault(key.strip(), value.strip())


def airtable_request(method: str, url: str, token: str, body: dict | None = None) -> dict:
    headers = {"Authorization": f"Bearer {token}"}
    data = None
    if body is not None:
        data = json.dumps(body).encode("utf-8")
        headers["Content-Type"] = "application/json"
    req = urllib.request.Request(url, data=data, headers=headers, method=method)
    with urllib.request.urlopen(req, timeout=60) as resp:
        return json.loads(resp.read().decode("utf-8"))


def find_by_id(base: str, table: str, id_field: str, id_value: str, token: str) -> dict | None:
    formula = urllib.parse.quote(f"{{{id_field}}}='{id_value}'")
    url = f"{API_ROOT}/{base}/{urllib.parse.quote(table)}?maxRecords=1&filterByFormula={formula}"
    data = airtable_request("GET", url, token)
    records = data.get("records") or []
    return records[0] if records else None


def upsert(base: str, table: str, id_field: str, fields: dict, token: str, dry_run: bool) -> str:
    key = fields[id_field]
    label = fields.get("name") or fields.get("title") or key
    if dry_run:
        return f"would upsert {table} {key}: {label}"

    existing = find_by_id(base, table, id_field, key, token)
    if existing:
        rid = existing["id"]
        url = f"{API_ROOT}/{base}/{urllib.parse.quote(table)}/{rid}"
        airtable_request("PATCH", url, token, {"fields": fields, "typecast": True})
        return f"Updated {table} {key}"

    url = f"{API_ROOT}/{base}/{urllib.parse.quote(table)}"
    airtable_request("POST", url, token, {"fields": fields, "typecast": True})
    return f"Created {table} {key}"


def main() -> None:
    load_dotenv(AUTOMATION_ENV)
    parser = argparse.ArgumentParser()
    parser.add_argument("--seed", type=Path, default=DEFAULT_SEED)
    parser.add_argument("--base-id", default=os.environ.get("AIRTABLE_BASE_ID", ""))
    parser.add_argument("--dry-run", action="store_true")
    args = parser.parse_args()

    seed = json.loads(args.seed.read_text(encoding="utf-8"))
    base = args.base_id or os.environ.get("AIRTABLE_BASE_ID", "")
    token = os.environ.get("AIRTABLE_API_KEY", "")

    if not args.dry_run and (not token or not base):
        print("Set AIRTABLE_API_KEY and AIRTABLE_BASE_ID.", file=sys.stderr)
        sys.exit(1)

    for section, table, id_field in TABLE_ORDER:
        for rec in seed.get(section, []):
            print(upsert(base, table, id_field, rec, token, args.dry_run))


if __name__ == "__main__":
    main()
