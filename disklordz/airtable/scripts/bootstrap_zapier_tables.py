#!/usr/bin/env python3
"""Add DiskLordz tables to an existing base via Zapier MCP helper output.

This script prints curl-free JSON payloads for each table POST to Meta API.
Run create_base_from_schema.py when AIRTABLE_API_KEY works from your network.

For Cloud Agents (no direct api.airtable.com egress), use Zapier execute_zapier_write_action
with action _zap_raw_request per printed payload, or run locally with PAT.
"""

from __future__ import annotations

import json
import sys
from pathlib import Path

SCRIPT_DIR = Path(__file__).resolve().parent
sys.path.insert(0, str(SCRIPT_DIR))
from _meta_api import table_spec_from_schema  # noqa: E402

SCHEMA = SCRIPT_DIR.parent / "base-schema.json"


def main() -> None:
    base_id = sys.argv[1] if len(sys.argv) > 1 else "appREPLACE_ME"
    schema = json.loads(SCHEMA.read_text(encoding="utf-8"))
    for table in schema.get("tables") or []:
        spec = table_spec_from_schema(table)
        payload = {
            "zapier": {
                "url": f"https://api.airtable.com/v0/meta/bases/{base_id}/tables",
                "method": "POST",
                "body": json.dumps(spec),
            },
            "table": table["name"],
        }
        print(json.dumps(payload))
        print("---")


if __name__ == "__main__":
    main()
