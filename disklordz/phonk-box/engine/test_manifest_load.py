#!/usr/bin/env python3
"""Contract: spark-le-manifest.json → 8 non-empty pad buffers."""
from __future__ import annotations

import sys
from pathlib import Path

ROOT = Path(__file__).resolve().parents[3]
MANIFEST = (
    ROOT
    / "docs/disklordz/spark-le/sprint-kits/session-1-gangsta-trap/spark-le-manifest.json"
)

from phonk_engine import load_kit_from_manifest  # noqa: E402


def main() -> None:
    if not MANIFEST.is_file():
        print(f"missing fixture: {MANIFEST}", file=sys.stderr)
        sys.exit(1)
    pads, bpm, preset = load_kit_from_manifest(MANIFEST)
    assert preset == "lane-trap-or-die", preset
    assert bpm == 140.0, bpm
    assert len(pads) == 8
    for i, p in enumerate(pads):
        assert len(p) > 100, f"pad {i + 1} too short"
    print("test_manifest_load: OK")


if __name__ == "__main__":
    main()
