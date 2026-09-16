#!/usr/bin/env python3
"""List Meta API field-create operations for an existing base (PAT or Zapier MCP)."""

from __future__ import annotations

import json
import sys
from pathlib import Path

SCRIPT_DIR = Path(__file__).resolve().parent
sys.path.insert(0, str(str(SCRIPT_DIR)))
from _meta_api import list_tables, schema_field_to_airtable, meta_request  # noqa: E402

SCHEMA_PATH = SCRIPT_DIR.parent / "base-schema.json"
ACTIVE_PATH = SCRIPT_DIR.parent / "ACTIVE_BASE.json"
BASE_ID = "appwaEPUOCXBoq8jB"


def load_table_ids() -> dict[str, str]:
    if ACTIVE_PATH.is_file():
        data = json.loads(ACTIVE_PATH.read_text(encoding="utf-8"))
        return dict(data.get("tables") or {})
    return {}


def primary_name_from_api(table: dict) -> str:
    pf = table.get("primaryFieldId")
    for f in table.get("fields") or []:
        if f.get("id") == pf:
            return f.get("name") or ""
    return (table.get("fields") or [{}])[0].get("name", "")


def existing_field_names(table: dict) -> set[str]:
    return {f.get("name") for f in table.get("fields") or [] if f.get("name")}


def field_ops_from_schema(schema_tables: list[dict], api_tables: list[dict]) -> list[dict]:
    by_name = {t["name"]: t for t in api_tables}
    ops: list[dict] = []
    for spec in schema_tables:
        name = spec["name"]
        at = by_name.get(name)
        if not at:
            ops.append({"op": "missing_table", "table": name})
            continue
        table_id = at["id"]
        have = existing_field_names(at)
        primary = primary_name_from_api(at)
        for field in spec.get("fields") or []:
            fname = field["name"]
            if fname in have:
                continue
            if fname == primary:
                continue
            body = schema_field_to_airtable(field, len(have))
            ops.append(
                {
                    "op": "create_field",
                    "table": name,
                    "table_id": table_id,
                    "url": f"https://api.airtable.com/v0/meta/bases/{BASE_ID}/tables/{table_id}/fields",
                    "body": body,
                }
            )
            have.add(fname)
    return ops


def apply_with_pat(token: str, ops: list[dict]) -> None:
    for op in ops:
        if op["op"] != "create_field":
            print(f"SKIP {op}", file=sys.stderr)
            continue
        meta_request("POST", op["url"], token, op["body"])
        print(f"OK {op['table']}.{op['body']['name']}")


def main() -> None:
    schema = json.loads(SCHEMA_PATH.read_text(encoding="utf-8"))
    token = __import__("os").environ.get("AIRTABLE_API_KEY")
    if token:
        api_tables = list_tables(token, BASE_ID)
        ops = field_ops_from_schema(schema.get("tables") or [], api_tables)
        apply_with_pat(token, ops)
        return

    # Offline: emit ops JSON for Zapier airtable_make_api_mutating_request
    ids = load_table_ids()
    ops = []
    for spec in schema.get("tables") or []:
        table_id = ids.get(spec["name"])
        if not table_id:
            ops.append({"op": "missing_table_id", "table": spec["name"]})
            continue
        primary = (spec.get("fields") or [{}])[0]["name"]
        have = {primary}
        if spec["name"] == "Product Families":
            have.add("name")
        for field in spec.get("fields") or []:
            fname = field["name"]
            if fname in have:
                continue
            body = schema_field_to_airtable(field, len(have))
            ops.append(
                {
                    "op": "create_field",
                    "table": spec["name"],
                    "table_id": table_id,
                    "url": f"https://api.airtable.com/v0/meta/bases/{BASE_ID}/tables/{table_id}/fields",
                    "body": body,
                }
            )
            have.add(fname)
    out = SCRIPT_DIR.parent / ".payloads" / "field_ops.json"
    out.parent.mkdir(parents=True, exist_ok=True)
    out.write_text(json.dumps(ops, indent=2), encoding="utf-8")
    print(f"Wrote {len(ops)} field ops to {out}")
    print(json.dumps(ops[:3], indent=2))


if __name__ == "__main__":
    main()
