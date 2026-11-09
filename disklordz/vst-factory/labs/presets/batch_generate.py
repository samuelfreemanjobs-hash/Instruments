#!/usr/bin/env python3
"""Stub preset batch generator — V2 expands with APVTS-aware mutation."""

from __future__ import annotations

import argparse
import json
from pathlib import Path


def main() -> int:
    p = argparse.ArgumentParser()
    p.add_argument("--plugin", required=True)
    p.add_argument("--count", type=int, default=8)
    p.add_argument("--category", default="factory")
    args = p.parse_args()
    out_dir = Path(f"vst/{args.plugin}/Presets")
    out_dir.mkdir(parents=True, exist_ok=True)
    manifest = {
        "plugin": args.plugin,
        "count": args.count,
        "category": args.category,
        "status": "stub",
        "note": "Implement APVTS XML generation in V2",
    }
    (out_dir / "_batch_manifest.json").write_text(json.dumps(manifest, indent=2) + "\n", encoding="utf-8")
    print(json.dumps(manifest))
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
