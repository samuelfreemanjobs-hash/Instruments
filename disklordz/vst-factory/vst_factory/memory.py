from __future__ import annotations

import json
from pathlib import Path
from typing import Any


def package_root() -> Path:
    return Path(__file__).resolve().parents[1]


def load_company_memory() -> dict[str, Any]:
    path = package_root() / "memory/company.json"
    return json.loads(path.read_text(encoding="utf-8"))


def _slug_variants(slug: str) -> list[str]:
    import re

    kebab = re.sub(r"(?<!^)(?=[A-Z])", "-", slug).lower()
    return list(dict.fromkeys([slug, slug.lower(), kebab.replace("_", "-")]))


def load_product_memory(slug: str) -> dict[str, Any] | None:
    root = package_root() / "memory/products"
    for variant in _slug_variants(slug):
        path = root / f"{variant}.json"
        if path.is_file():
            return json.loads(path.read_text(encoding="utf-8"))
    return None


def load_dependency_graph() -> dict[str, Any]:
    path = package_root() / "memory/dependency-graph.json"
    return json.loads(path.read_text(encoding="utf-8"))


def format_memory_for_prompt(slug: str | None = None) -> tuple[str, str]:
    company = load_company_memory()
    lines = ["Company rules:"] + [f"- {r}" for r in company.get("rules", [])]
    company_text = "\n".join(lines)
    product_text = ""
    if slug:
        prod = load_product_memory(slug)
        if prod:
            product_text = json.dumps(prod, indent=2)
    return company_text, product_text
