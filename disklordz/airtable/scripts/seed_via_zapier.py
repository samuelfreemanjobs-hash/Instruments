#!/usr/bin/env python3
"""Print Zapier create_record payloads for bootstrap seed (for manual MCP or Zapier Zaps)."""

from __future__ import annotations

import json
from pathlib import Path

SEED = Path(__file__).resolve().parent.parent / "seed" / "bootstrap.json"
TABLE_MAP = {
    "product_families": "Product Families",
    "products": "Products",
    "projects": "Projects",
    "work_orders": "Agent Work Orders",
    "automations": "Automations",
}


def main() -> None:
    seed = json.loads(SEED.read_text(encoding="utf-8"))
    for key, table in TABLE_MAP.items():
        for row in seed.get(key) or []:
            print(json.dumps({"tableName": table, "fields": row}, indent=2))
            print("---")


if __name__ == "__main__":
    main()
