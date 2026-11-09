from __future__ import annotations

import json
from pathlib import Path
from typing import Any


def load_product_library() -> dict[str, Any]:
    path = Path(__file__).resolve().parents[1] / "data/product-library.json"
    return json.loads(path.read_text(encoding="utf-8"))


def merge_with_db_projects(library: dict[str, Any], projects: list[dict[str, Any]]) -> list[dict[str, Any]]:
    by_name = {p["plugin_name"]: p for p in projects}
    rows: list[dict[str, Any]] = []
    for item in library.get("products", []):
        row = dict(item)
        fn = item.get("factory_project_name")
        if fn and fn in by_name:
            row["sqlite_state"] = by_name[fn].get("state")
            row["factory_project_id"] = by_name[fn].get("id")
        rows.append(row)
    return rows
