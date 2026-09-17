#!/usr/bin/env python3
"""Verify Airtable PAT and list tables in DiskLordz base."""

from __future__ import annotations

import os
import sys
from pathlib import Path

# Allow running from repo without package install
sys.path.insert(0, str(Path(__file__).resolve().parent))
from _meta_api import list_tables

AUTOMATION_ENV = Path(__file__).resolve().parents[2] / "automation" / ".env"


def load_dotenv(path: Path) -> None:
    if not path.is_file():
        return
    for line in path.read_text(encoding="utf-8").splitlines():
        line = line.strip()
        if not line or line.startswith("#") or "=" not in line:
            continue
        k, v = line.split("=", 1)
        os.environ.setdefault(k.strip(), v.strip())


def main() -> None:
    load_dotenv(AUTOMATION_ENV)
    token = os.environ.get("AIRTABLE_API_KEY")
    base_id = os.environ.get("AIRTABLE_BASE_ID")
    if not token or not base_id:
        print("Missing AIRTABLE_API_KEY or AIRTABLE_BASE_ID in env or automation/.env", file=sys.stderr)
        sys.exit(1)

    tables = list_tables(token, base_id)
    print(f"OK — base {base_id} has {len(tables)} tables:")
    for t in sorted(tables, key=lambda x: x.get("name", "")):
        print(f"  - {t.get('name')} ({t.get('id')})")
    expected = {
        "Products",
        "Product Families",
        "Projects",
        "Agent Work Orders",
        "Automations",
    }
    names = {t.get("name") for t in tables}
    missing = expected - names
    if missing:
        print(f"WARNING: missing expected tables: {', '.join(sorted(missing))}", file=sys.stderr)
        sys.exit(2)
    print("Core tables present.")


if __name__ == "__main__":
    main()
