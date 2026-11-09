from __future__ import annotations

import json
from functools import lru_cache
from pathlib import Path
from typing import Any


def modules_root() -> Path:
    return Path(__file__).resolve().parents[1] / "modules"


def templates_root() -> Path:
    return Path(__file__).resolve().parents[1] / "templates"


@lru_cache(maxsize=1)
def load_catalog() -> dict[str, Any]:
    path = modules_root() / "catalog.json"
    return json.loads(path.read_text(encoding="utf-8"))


def _load_relative(rel: str) -> dict[str, Any]:
    path = modules_root() / rel
    return json.loads(path.read_text(encoding="utf-8"))


def load_product_taxonomy() -> dict[str, Any]:
    return _load_relative("product-taxonomy.json")


def taxonomy_category(category_id: str) -> dict[str, Any] | None:
    tax = load_product_taxonomy()
    for cat in tax.get("categories", []):
        if cat.get("id") == category_id:
            return cat
    return None


def plugin_class_module(plugin_class: str) -> dict[str, Any]:
    mapping = {
        "synth": "plugin-classes/synth.json",
        "effect": "plugin-classes/effect.json",
        "midi": "plugin-classes/midi.json",
        "sampler": "plugin-classes/sampler.json",
    }
    rel = mapping.get(plugin_class)
    if not rel:
        raise ValueError(f"unknown plugin_class: {plugin_class}")
    return _load_relative(rel)


def list_dsp_blocks() -> list[dict[str, Any]]:
    catalog = load_catalog()
    blocks: list[dict[str, Any]] = []
    for rel in catalog.get("dsp", []):
        data = _load_relative(rel)
        blocks.extend(data.get("blocks", []))
    return blocks


def list_gui_blocks() -> list[dict[str, Any]]:
    catalog = load_catalog()
    blocks: list[dict[str, Any]] = []
    for rel in catalog.get("gui", []):
        data = _load_relative(rel)
        blocks.extend(data.get("blocks", []))
    return blocks


def render_template(rel_path: str, **kwargs: str) -> str:
    """Simple {{key}} substitution (no Jinja dependency in Phase 1)."""
    path = templates_root() / rel_path
    text = path.read_text(encoding="utf-8")
    for key, value in kwargs.items():
        text = text.replace("{{" + key + "}}", value)
    return text


DEFAULT_FORMATS = {
    "vst3": True,
    "standalone": True,
    "au": False,
    "clap": False,
    "aax": False,
    "lv2": False,
}


def build_spec_from_modules(
    *,
    plugin_name: str,
    plugin_class: str,
    prompt: str,
    product_category: str | None = None,
) -> dict[str, Any]:
    pcm = plugin_class_module(plugin_class)
    dsp_modules = list(pcm.get("suggested_dsp_modules", []))
    gui_modules = list(pcm.get("suggested_gui_modules", []))
    acceptance = [
        f"tools/build-vst.sh {plugin_name} succeeds",
        f"run_business.py --profile factory --plugin {plugin_name}",
    ]
    category_id = product_category
    if product_category:
        cat = taxonomy_category(product_category)
        if cat:
            plugin_class = cat.get("plugin_class", plugin_class)
            dsp_modules = list(cat.get("dsp_modules", dsp_modules))
            gui_modules = list(cat.get("gui_modules", gui_modules))
            acceptance.extend(cat.get("acceptance_tests", []))
    elif not category_id:
        category_id = f"{plugin_class}.generic"
    return {
        "schema_version": "factory-spec/v2",
        "plugin_name": plugin_name,
        "plugin_class": plugin_class,
        "product_category": category_id,
        "target_format": "vst3",
        "target_os": "windows_x64",
        "prompt": prompt,
        "track": "apc_greenfield",
        "formats": dict(DEFAULT_FORMATS),
        "commercial": {
            "release_ready": False,
            "license_checklist_passed": False,
            "build_kind": "development",
        },
        "compatibility": {},
        "memory_refs": {
            "company": "company",
            "product_slug": plugin_name,
            "technical_tags": [],
        },
        "buses": pcm.get("default_buses", {}),
        "dsp_modules": dsp_modules,
        "gui_modules": gui_modules,
        "parameters": [],
        "acceptance_tests": acceptance,
        "module_refs": {
            "plugin_class_id": pcm.get("id"),
            "workflow_id": "workflow/scaffold-default",
            "product_category": category_id,
        },
    }


def materialize_creative_brief(plugin_class: str, plugin_name: str, prompt: str, spec: dict[str, Any]) -> str:
    pcm = plugin_class_module(plugin_class)
    rel = (pcm.get("apc_template") or "templates/apc/creative-brief-synth.md").replace("templates/", "")
    return render_template(
        rel,
        plugin_name=plugin_name,
        prompt=prompt,
        dsp_modules=", ".join(spec.get("dsp_modules", [])),
        gui_modules=", ".join(spec.get("gui_modules", [])),
    )
