#!/usr/bin/env python3
"""Write audition manifest (2 presets per category). Run package_audition_artifacts.py for WAV/ZIP."""

from __future__ import annotations

import json
import sys
from pathlib import Path

ROOT = Path(__file__).resolve().parents[1]
BANK = ROOT / "assets/factory/preset_bank.json"
OUT = ROOT / "assets/factory/audition_manifest.json"

ORDER = [
    "Pads",
    "Leads",
    "Synth Keys",
    "Swells",
    "Brass",
    "Strings",
    "Church Keys/EP",
    "Organ",
    "Bells",
    "Bass",
    "Choir",
    "Hits/FX",
]


def main() -> int:
    data = json.loads(BANK.read_text(encoding="utf-8"))
    presets = data["presets"]
    by_cat: dict[str, list] = {}
    for i, p in enumerate(presets):
        by_cat.setdefault(p["category"], []).append({"index": i, "name": p["name"]})
    manifest = []
    for cat in ORDER:
        picks = by_cat.get(cat, [])[:2]
        for slot, pick in enumerate(picks, start=1):
            manifest.append({"category": cat, "slot": slot, **pick})
    OUT.parent.mkdir(parents=True, exist_ok=True)
    OUT.write_text(json.dumps({"auditions": manifest, "wavCount": len(manifest)}, indent=2), encoding="utf-8")
    print(f"Wrote {OUT} ({len(manifest)} entries)")
    for m in manifest:
        print(f"  {m['category']} #{m['slot']}: idx {m['index']} {m['name']}")
    return 0


if __name__ == "__main__":
    sys.exit(main())
