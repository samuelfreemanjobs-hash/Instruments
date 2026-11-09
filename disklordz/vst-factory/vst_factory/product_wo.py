from __future__ import annotations

from typing import Any


def default_components(state: str) -> dict[str, dict[str, Any]]:
    pct = {
        "dream": 5,
        "plan": 10,
        "design": 15,
        "scaffold": 25,
        "impl": 35,
        "build": 55,
        "test": 70,
        "repair": 60,
        "ship": 85,
    }.get(state, 5)
    label = "pending" if pct < 70 else "in_progress"
    return {
        k: {"percent": pct if k != "docs" else max(5, pct - 10), "status": label}
        for k in ("dsp", "midi", "gui", "presets", "tests", "docs")
    }


def build_product_wo(proj: dict[str, Any]) -> dict[str, Any]:
    spec = proj["spec"]
    return {
        "schema": "product-wo/v1",
        "product_name": proj["plugin_name"],
        "prompt": spec.get("prompt", ""),
        "product_category": spec.get("product_category"),
        "autonomy_mode": spec.get("autonomy_mode", "collaborative"),
        "components": default_components(proj["state"]),
        "factory_project_id": proj["id"],
    }
