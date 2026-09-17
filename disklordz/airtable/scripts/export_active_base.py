#!/usr/bin/env python3
"""Write ACTIVE_BASE.json table map from Meta API (requires AIRTABLE_API_KEY + AIRTABLE_BASE_ID)."""

from __future__ import annotations

import json
import os
import sys
from pathlib import Path

SCRIPT_DIR = Path(__file__).resolve().parent
sys.path.insert(0, str(SCRIPT_DIR))
from _meta_api import list_tables  # noqa: E402

ACTIVE = SCRIPT_DIR.parent / "ACTIVE_BASE.json"
AUTOMATION_ENV = SCRIPT_DIR.parents[2] / "automation" / ".env"


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
        print("Set AIRTABLE_API_KEY and AIRTABLE_BASE_ID", file=sys.stderr)
        sys.exit(1)
    tables = list_tables(token, base_id)
    by_name = {t["name"]: t["id"] for t in tables}
    out = {
        "note": "Canonical DiskLordz OS base — set AIRTABLE_BASE_ID to base_id in Cursor and GitHub secrets.",
        "base_name": "DiskLordz OS",
        "base_id": base_id,
        "base_url": f"https://airtable.com/{base_id}",
        "tables": by_name,
        "legacy_bases": {
            "interim_os_in_business_idea_tracker": "appwaEPUOCXBoq8jB",
            "storefront_crm_disklordz_factory": "appykxDsfaMFC2hGp",
        },
    }
    ACTIVE.write_text(json.dumps(out, indent=2) + "\n", encoding="utf-8")
    print(f"Wrote {ACTIVE} ({len(by_name)} tables)")


if __name__ == "__main__":
    main()
