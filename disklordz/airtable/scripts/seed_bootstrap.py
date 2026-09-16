#!/usr/bin/env python3
"""Insert bootstrap rows from seed/bootstrap.json (idempotent on primary id fields)."""

from __future__ import annotations

import json
import os
import sys
import urllib.error
import urllib.parse
import urllib.request
from pathlib import Path

SEED_PATH = Path(__file__).resolve().parent.parent / "seed" / "bootstrap.json"
AUTOMATION_ENV = Path(__file__).resolve().parents[2] / "automation" / ".env"
API_ROOT = "https://api.airtable.com/v0"

TABLE_MAP = {
    "product_families": "Product Families",
    "products": "Products",
    "projects": "Projects",
    "work_orders": "Agent Work Orders",
    "automations": "Automations",
}

ID_FIELD = {
    "Product Families": "family_id",
    "Products": "product_id",
    "Projects": "project_id",
    "Agent Work Orders": "work_order_id",
    "Automations": "automation_id",
}


def load_dotenv(path: Path) -> None:
    if not path.is_file():
        return
    for line in path.read_text(encoding="utf-8").splitlines():
        line = line.strip()
        if not line or line.startswith("#") or "=" not in line:
            continue
        k, v = line.split("=", 1)
        os.environ.setdefault(k.strip(), v.strip())


def api_get(url: str, token: str) -> dict:
    req = urllib.request.Request(url, headers={"Authorization": f"Bearer {token}"})
    with urllib.request.urlopen(req, timeout=60) as resp:
        return json.loads(resp.read().decode("utf-8"))


def api_post(url: str, token: str, body: dict) -> dict:
    data = json.dumps(body).encode("utf-8")
    req = urllib.request.Request(
        url,
        data=data,
        headers={"Authorization": f"Bearer {token}", "Content-Type": "application/json"},
        method="POST",
    )
    with urllib.request.urlopen(req, timeout=60) as resp:
        return json.loads(resp.read().decode("utf-8"))


def record_exists(token: str, base_id: str, table: str, id_field: str, value: str) -> bool:
    formula = urllib.parse.quote(f"{{{id_field}}}='{value}'")
    url = f"{API_ROOT}/{base_id}/{urllib.parse.quote(table)}?maxRecords=1&filterByFormula={formula}"
    try:
        data = api_get(url, token)
        return bool(data.get("records"))
    except urllib.error.HTTPError:
        return False


def create_records(token: str, base_id: str, table: str, rows: list[dict]) -> int:
    id_field = ID_FIELD[table]
    created = 0
    url = f"{API_ROOT}/{base_id}/{urllib.parse.quote(table)}"
    for row in rows:
        rid = row.get(id_field) or row.get("work_order_id")
        if rid and record_exists(token, base_id, table, id_field, rid):
            print(f"  skip {table}: {rid} exists")
            continue
        fields = {k: v for k, v in row.items() if v is not None}
        api_post(url, token, {"fields": fields, "typecast": True})
        print(f"  created {table}: {rid}")
        created += 1
    return created


def main() -> None:
    load_dotenv(AUTOMATION_ENV)
    token = os.environ.get("AIRTABLE_API_KEY")
    base_id = os.environ.get("AIRTABLE_BASE_ID")
    if not token or not base_id:
        print("Set AIRTABLE_API_KEY and AIRTABLE_BASE_ID in automation/.env", file=sys.stderr)
        sys.exit(1)

    seed = json.loads(SEED_PATH.read_text(encoding="utf-8"))
    total = 0
    for key, table in TABLE_MAP.items():
        rows = seed.get(key) or []
        if rows:
            print(f"Seeding {table}...")
            total += create_records(token, base_id, table, rows)
    print(f"Done. {total} new record(s).")


if __name__ == "__main__":
    main()
