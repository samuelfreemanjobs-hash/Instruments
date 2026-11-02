"""Minimal Airtable helpers (stdlib only)."""

from __future__ import annotations

import json
import os
import urllib.parse
import urllib.request
from pathlib import Path

API_ROOT = "https://api.airtable.com/v0"
AUTOMATION_ENV = Path(__file__).resolve().parents[1] / ".env"


def load_dotenv(path: Path | None = None) -> None:
    path = path or AUTOMATION_ENV
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
