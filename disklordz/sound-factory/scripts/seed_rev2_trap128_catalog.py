#!/usr/bin/env python3
"""Build REV2-TRAP-128 preset catalog (128 Prophet-style trap multisamples)."""

from __future__ import annotations

import argparse
import json
from pathlib import Path

ROOT = Path(__file__).resolve().parents[1]

LANES = ("jeezy", "shawty_redd", "gucci", "neutral")
CATEGORIES = (
    "trap_lead",
    "dark_pad",
    "brass_stab",
    "sub_808",
    "pluck",
    "choir",
    "bell",
    "noise_rise",
)


def main() -> None:
    p = argparse.ArgumentParser()
    p.add_argument("--out", type=Path, default=ROOT / "catalog" / "rev2_trap128_catalog.json")
    args = p.parse_args()

    slots = []
    for i in range(128):
        lane = LANES[i % len(LANES)]
        cat = CATEGORIES[i % len(CATEGORIES)]
        slot_id = f"REV2_{i + 1:03d}"
        slots.append(
            {
                "slotId": slot_id,
                "productId": "REV2-TRAP-128",
                "category": cat,
                "lane": lane,
                "instrumentId": f"{slot_id}_{cat}",
                "name": f"Rev2 Trap {cat} ({lane}) {i + 1}",
                "referenceQuery": f"trap {lane} {cat} prophet synth 2000s",
            }
        )

    catalog = {
        "format": "DISKLORDZ_FACTORY_CATALOG",
        "version": 1,
        "productId": "REV2-TRAP-128",
        "title": "Prophet Rev2–style trap multisample bank (128)",
        "vstiRoadmap": "JUCE port referencing clean-room trap_synth + future DSP upgrade",
        "targetCount": 128,
        "slots": slots,
    }
    args.out.parent.mkdir(parents=True, exist_ok=True)
    args.out.write_text(json.dumps(catalog, indent=2), encoding="utf-8")
    print(json.dumps({"out": str(args.out), "count": 128}, indent=2))


if __name__ == "__main__":
    main()
