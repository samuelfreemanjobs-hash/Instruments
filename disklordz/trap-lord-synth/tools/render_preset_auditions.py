#!/usr/bin/env python3
"""Pick one preset per category and write audit manifest for audition renders."""

from __future__ import annotations

import json
import sys
from pathlib import Path

ROOT = Path(__file__).resolve().parents[1]
BANK = ROOT / "assets/factory/preset_bank.json"
OUT = ROOT / "assets/factory/audition_manifest.json"


def main() -> int:
    data = json.loads(BANK.read_text(encoding="utf-8"))
    presets = data["presets"]
    by_cat: dict[str, list] = {}
    for i, p in enumerate(presets):
        by_cat.setdefault(p["category"], []).append({"index": i, "name": p["name"]})
    manifest = []
    for cat in sorted(by_cat):
        pick = by_cat[cat][0]
        manifest.append({"category": cat, **pick})
    OUT.parent.mkdir(parents=True, exist_ok=True)
    OUT.write_text(json.dumps({"auditions": manifest}, indent=2), encoding="utf-8")
    print(f"Wrote {OUT} ({len(manifest)} entries)")
    for m in manifest:
        print(f"  {m['category']}: #{m['index']} {m['name']}")
    return 0


if __name__ == "__main__":
    sys.exit(main())
