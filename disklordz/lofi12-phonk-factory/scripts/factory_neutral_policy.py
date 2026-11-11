"""Neutral naming policy — no artist / producer / label strings in factory outputs."""

from __future__ import annotations

import json
import re
from pathlib import Path

# Substrings matched case-insensitively (factory listing, SKUs, paths, presets).
BLOCKED_SUBSTRINGS: tuple[str, ...] = (
    "juicy j",
    "juicy_j",
    "juicyj",
    "dj paul",
    "dj_paul",
    "djpaul",
    "dj toomp",
    "dj_toomp",
    "toomp",
    "memphis_trinity",
    "three 6",
    "three6",
    "triple 6",
    "36 mafia",
    "zaytoven",
    "gucci",
    "jeezy",
    "snowman",
    "snow man",
    "trap or die",
    "shawty pimp",
    "shawty_pimp",
    "skinny pimp",
    "kingpin_skinny",
    "kingpin skinny",
    "dj zirk",
    "grid_zirk",
    "toy wright",
    "toy_wright",
    "dl-toomp",
    "dl-juicy",
    "dl-djpaul",
    "dl-djtoomp",
    "t.i.",
    "snow ",
    "metro boomin",
    "mike will",
    "drumba boy",
    "young dolph",
    "est g",
    "project pat",
    "gangsta grillz",
)

BLOCKED_REGEX: tuple[re.Pattern[str], ...] = tuple(
    re.compile(p, re.IGNORECASE)
    for p in (
        r"\bjuicy\b",
        r"\btoomp\b",
        r"\btrinity\b",
        r"\bproducer\b",  # channel / credit references in copy
        r"\bzirk\b",
        r"\bshawty\b",
        r"\bgucci\b",
    )
)

# Paths skipped when scanning the factory tree (hold policy definitions only).
SCAN_SKIP_DIRS = frozenset({"__pycache__", ".git", "references/gui"})

SCAN_SKIP_FILES = frozenset(
    {
        "factory_neutral_policy.py",
        "test_neutral_naming_policy.py",
    }
)

SCAN_SUFFIXES = frozenset(
    {".py", ".md", ".json", ".js", ".html", ".txt", ".css", ".ts", ".tsx"}
)


def find_policy_violations(text: str) -> list[str]:
    lowered = text.lower()
    hits: list[str] = []
    for sub in BLOCKED_SUBSTRINGS:
        if sub.lower() in lowered:
            hits.append(sub)
    for rx in BLOCKED_REGEX:
        if rx.search(text):
            hits.append(rx.pattern)
    return hits


def scan_text_blobs(blobs: list[tuple[str, str]]) -> list[tuple[str, str]]:
    violations: list[tuple[str, str]] = []
    for context, text in blobs:
        for hit in find_policy_violations(text):
            violations.append((context, hit))
    return violations


def scan_factory_tree(factory_root: Path) -> list[tuple[str, str]]:
    blobs: list[tuple[str, str]] = []
    for path in factory_root.rglob("*"):
        if path.is_dir():
            if path.name in SCAN_SKIP_DIRS:
                continue
            continue
        if path.name in SCAN_SKIP_FILES:
            continue
        if path.suffix.lower() not in SCAN_SUFFIXES:
            continue
        if any(part in SCAN_SKIP_DIRS for part in path.parts):
            continue
        try:
            blobs.append((str(path.relative_to(factory_root)), path.read_text(encoding="utf-8")))
        except UnicodeDecodeError:
            continue
    return scan_text_blobs(blobs)


def scan_catalog(catalog_path: Path) -> list[tuple[str, str]]:
    data = json.loads(catalog_path.read_text(encoding="utf-8"))
    blobs: list[tuple[str, str]] = []
    for product in data.get("products", []):
        sku = product.get("sku", "")
        blobs.append((f"catalog:sku:{sku}", sku))
        blobs.append((f"catalog:title:{sku}", product.get("title", "")))
        for key in ("prompt", "factoryMode", "preset"):
            val = product.get(key)
            if val:
                blobs.append((f"catalog:{key}:{sku}", str(val)))
    return scan_text_blobs(blobs)


def scan_kit_paths(kit_dir: Path) -> list[tuple[str, str]]:
    blobs: list[tuple[str, str]] = []
    for path in kit_dir.rglob("*"):
        if path.is_file():
            rel = str(path.relative_to(kit_dir))
            blobs.append((f"kit:path:{rel}", rel))
    manifest = kit_dir / "manifest.json"
    if manifest.is_file():
        blobs.append((f"kit:manifest", manifest.read_text(encoding="utf-8")))
    return scan_text_blobs(blobs)
