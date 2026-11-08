#!/usr/bin/env python3
"""Ingest hand-curated preset names into lineage DB (duplicate guard)."""

from __future__ import annotations

import hashlib
import re
import sys
from pathlib import Path

REPO = Path(__file__).resolve().parents[2]
sys.path.insert(0, str(REPO / "scripts/vst"))

from vst_lineage import PresetRecord, VstLineage  # noqa: E402

NAME_RE = re.compile(r'\{\s*"([^"]+)"\s*,\s*"([^"]+)"')


def ingest_cpp(product: str, path: Path) -> int:
    text = path.read_text(encoding="utf-8")
    count = 0
    db = VstLineage()
    for name, category in NAME_RE.findall(text):
        phash = hashlib.sha256(f"{product}:{name}".encode()).hexdigest()[:32]
        rec = PresetRecord(product, name, category, phash, "{}", "ingest_hand", None)
        if db.insert(rec):
            count += 1
    db.close()
    return count


def main() -> int:
    total = 0
    total += ingest_cpp("wave9090", REPO / "Wave9090/Source/Presets/FactoryPresets.cpp")
    total += ingest_cpp("pluggva", REPO / "PluggVA/Source/Presets/FactoryPresets.cpp")
    print(f"ingested_new: {total}")
    db = VstLineage()
    print(f"lineage_total: {db.count()}")
    db.close()
    return 0


if __name__ == "__main__":
    sys.exit(main())
