from __future__ import annotations

import json
from pathlib import Path
from typing import Any


def package_data() -> Path:
    return Path(__file__).resolve().parents[1] / "data"


def load_product_library() -> dict[str, Any]:
    """Load static product catalog from data/product-library.json."""
    path = package_data() / "product-library.json"
    return json.loads(path.read_text(encoding="utf-8"))


def merge_with_db_projects(
    library: dict[str, Any],
    projects: list[dict[str, Any]],
) -> list[dict[str, Any]]:
    """Merge library SKUs with live SQLite projects for GET /api/portfolio.

    Library entries are the base; matching plugin_name projects overlay
    factory state (id, state, paths). DB-only projects appear as extras.
    """
    by_name: dict[str, dict[str, Any]] = {}
    for product in library.get("products", []):
        name = product.get("plugin_name") or product.get("sku")
        if not name:
            continue
        entry = dict(product)
        entry["plugin_name"] = name
        entry["source"] = "library"
        entry["factory_project"] = None
        by_name[name] = entry

    for proj in projects:
        name = proj.get("plugin_name")
        if not name:
            continue
        factory = {
            "id": proj.get("id"),
            "state": proj.get("state"),
            "plugin_class": proj.get("plugin_class"),
            "apc_path": proj.get("apc_path"),
            "vst_path": proj.get("vst_path"),
            "product_category": (proj.get("spec") or {}).get("product_category"),
        }
        if name in by_name:
            by_name[name]["factory_project"] = factory
            by_name[name]["source"] = "library+db"
        else:
            by_name[name] = {
                "sku": name,
                "plugin_name": name,
                "plugin_class": proj.get("plugin_class"),
                "product_category": (proj.get("spec") or {}).get("product_category"),
                "track": (proj.get("spec") or {}).get("track"),
                "status": "in_factory",
                "source": "db",
                "factory_project": factory,
            }

    return list(by_name.values())
