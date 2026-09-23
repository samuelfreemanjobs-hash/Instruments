#!/usr/bin/env python3
"""Build JZ400 catalog (400 non-drum instrument slots) from album track metadata."""

from __future__ import annotations

import argparse
import json
from pathlib import Path

ROOT = Path(__file__).resolve().parents[1]
ALBUMS = ROOT / "catalog" / "jz400_albums.json"

# Non-drum instrument archetypes per track (808 bass allowed per product brief)
ARCHETYPES = (
    "lead_synth",
    "pad",
    "bass_808",
    "brass_stab",
    "string_lick",
    "bell",
    "choir_hit",
    "fx_sweep",
)


def main() -> None:
    p = argparse.ArgumentParser()
    p.add_argument("--target", type=int, default=400)
    p.add_argument("--out", type=Path, default=ROOT / "catalog" / "jz400_catalog.json")
    args = p.parse_args()

    meta = json.loads(ALBUMS.read_text(encoding="utf-8"))
    slots: list[dict] = []
    idx = 0
    for album in meta["albums"]:
        for track in album["tracks"]:
            if track.lower() in ("intro", "outro", "skit", "interlude"):
                continue
            for arch in ARCHETYPES:
                if len(slots) >= args.target:
                    break
                idx += 1
                slot_id = f"JZ400_{idx:04d}"
                slots.append(
                    {
                        "slotId": slot_id,
                        "productId": "JZ400",
                        "albumId": album["id"],
                        "albumTitle": album["title"],
                        "trackTitle": track,
                        "archetype": arch,
                        "lane": "jeezy",
                        "instrumentId": f"{slot_id}_{arch}",
                        "referenceQuery": f"Young Jeezy {track} {arch} trap instrumental",
                    }
                )
            if len(slots) >= args.target:
                break
        if len(slots) >= args.target:
            break

    # If metadata shorter than target, fill synthetic continuation slots
    while len(slots) < args.target:
        idx += 1
        slot_id = f"JZ400_{idx:04d}"
        slots.append(
            {
                "slotId": slot_id,
                "productId": "JZ400",
                "albumId": "extended_catalog",
                "albumTitle": "Extended mixtape / deep cut index",
                "trackTitle": f"DeepCut_{idx}",
                "archetype": ARCHETYPES[idx % len(ARCHETYPES)],
                "lane": "jeezy",
                "instrumentId": f"{slot_id}_{ARCHETYPES[idx % len(ARCHETYPES)]}",
                "referenceQuery": "Young Jeezy trap synth 2000s Atlanta",
            }
        )

    catalog = {
        "format": "DISKLORDZ_FACTORY_CATALOG",
        "version": 1,
        "productId": "JZ400",
        "title": "JZ400 — Young Jeezy type multisample library",
        "targetCount": args.target,
        "slots": slots[: args.target],
    }
    args.out.parent.mkdir(parents=True, exist_ok=True)
    args.out.write_text(json.dumps(catalog, indent=2), encoding="utf-8")
    print(json.dumps({"out": str(args.out), "count": len(catalog["slots"])}, indent=2))


if __name__ == "__main__":
    main()
