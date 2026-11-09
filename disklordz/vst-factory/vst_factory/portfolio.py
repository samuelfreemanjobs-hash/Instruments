from __future__ import annotations

import json
from pathlib import Path
from typing import Any


def load_product_library() -> dict[str, Any]:
    path = Path(__file__).resolve().parents[1] / "data/product-library.json"
    return json.loads(path.read_text(encoding="utf-8"))


def _product_row_plugin_name(item: dict[str, Any]) -> str:
    return str(
        item.get("plugin_name")
        or item.get("factory_project_name")
        or item.get("name")
        or item.get("id")
    )


def _factory_project_slice(proj: dict[str, Any]) -> dict[str, Any]:
    return {
        "id": proj["id"],
        "state": proj.get("state"),
        "plugin_class": proj.get("plugin_class"),
        "apc_path": proj.get("apc_path"),
        "vst_path": proj.get("vst_path"),
    }


def merge_with_db_projects(library: dict[str, Any], projects: list[dict[str, Any]]) -> list[dict[str, Any]]:
    by_name = {p["plugin_name"]: p for p in projects}
    linked: set[str] = set()
    rows: list[dict[str, Any]] = []

    for item in library.get("products", []):
        row = dict(item)
        pname = _product_row_plugin_name(item)
        row["plugin_name"] = pname
        match_key = item.get("factory_project_name") or pname
        if match_key in by_name:
            row["factory_project"] = _factory_project_slice(by_name[match_key])
            row["source"] = "library+db"
            linked.add(match_key)
        else:
            row["source"] = "library"
        rows.append(row)

    for pname, proj in by_name.items():
        if pname in linked:
            continue
        rows.append(
            {
                "plugin_name": pname,
                "source": "db",
                "factory_project": _factory_project_slice(proj),
            }
        )
    return rows
