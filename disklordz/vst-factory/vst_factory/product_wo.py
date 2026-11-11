from __future__ import annotations

from typing import Any


def build_product_wo(project: dict[str, Any]) -> dict[str, Any]:
    """Build a product work-order snapshot from a factory project row.

    Stored via FactoryDB.upsert_product_wo and served on GET .../product-wo.
    """
    spec = project.get("spec") or {}
    name = project.get("plugin_name") or spec.get("plugin_name") or "Unknown"
    return {
        "schema_version": "product-wo/v1",
        "project_id": project.get("id"),
        "plugin_name": name,
        "plugin_class": project.get("plugin_class") or spec.get("plugin_class"),
        "state": project.get("state"),
        "product_category": spec.get("product_category"),
        "track": spec.get("track", "apc_greenfield"),
        "autonomy_mode": spec.get("autonomy_mode", "collaborative"),
        "dsp_modules": list(spec.get("dsp_modules") or []),
        "gui_modules": list(spec.get("gui_modules") or []),
        "gates": {
            "approve_architecture": "required_before_scaffold",
            "approve_release": "required_before_ship",
        },
        "acceptance_tests": list(spec.get("acceptance_tests") or []),
        "paths": {
            "apc_path": project.get("apc_path"),
            "vst_path": project.get("vst_path"),
        },
        "title": f"Factory WO — {name}",
        "summary": (spec.get("prompt") or "")[:500],
    }
